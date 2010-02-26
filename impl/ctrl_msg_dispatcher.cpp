#include "ctrl_msg_dispatcher.h"
#include "erl_cpp_exception.h"
#include "tinch_pp/erlang_types.h"
#include "constants.h"
#include "matchable_range.h"
#include "node_connection_access.h"
#include "utils.h"
#include <boost/lexical_cast.hpp>

using namespace tinch_pp;
using namespace tinch_pp::erl;

namespace tinch_pp {

void check_term_version(msg_seq_iter& first, const msg_seq_iter& last)
{
  const boost::uint8_t term_version = *first++;

  if(constants::magic_version != term_version) {
    
    const std::string reason = "Erroneous term version received. Got = " + 
                                 boost::lexical_cast<std::string>(term_version) +
                                 ", expected = " + boost::lexical_cast<std::string>(constants::magic_version);
    throw erl_cpp_exception(reason);
  }
}

void parse_header(msg_seq_iter& first, const msg_seq_iter& last)
{
  using namespace qi;

  if(!parse(first, last, omit[big_dword] >>  byte_(constants::pass_through)))
    throw erl_cpp_exception("Invalid header in ctrl-message: " + utils::to_printable_string(first, last));
  
  check_term_version(first, last);
}

// Each distributed Erlang operation has its own dispatcher.
// The dispatchers are implemented as a Chain Of Responsibility (design pattern, GoF).
class operation_dispatcher
{
public:
  operation_dispatcher(access_ptr a_access) 
    : access(a_access) {}
  
  operation_dispatcher(access_ptr a_access, dispatcher_ptr a_next)
    : access(a_access),
      next(a_next) {}

  virtual ~operation_dispatcher() {}

  void dispatch(msg_seq_iter& first, const msg_seq_iter& last) const
  {
    msg_seq_iter remember_first = first;

    if(!this->handle(first, last))
      try_next_in_chain(remember_first, last);
  }

  virtual bool handle(msg_seq_iter& first, const msg_seq_iter& last) const = 0;

private:
  void try_next_in_chain(msg_seq_iter& first, const msg_seq_iter& last) const
  {
    if(!next)
      throw erl_cpp_exception("Unsupported distributed operation. Action = operation ignored. Msg = " + 
                              utils::to_printable_string(first, last));

    next->dispatch(first, last);
  }

protected:
  access_ptr access;

private:
  dispatcher_ptr next;
};

// SEND operation:  tuple of {2, Cookie, ToPid} 
// This operation delivers the payload following the ctrl-msg to the ToPid.
class send_handler : public operation_dispatcher
{
public:
  send_handler(access_ptr access, dispatcher_ptr next) 
    : operation_dispatcher(access, next) {}

  virtual bool handle(msg_seq_iter& f, const msg_seq_iter& l) const
  {
    bool handled = false;
    pid_t to_pid;
     
    if(matchable_range(f, l).match(make_tuple(int_(constants::ctrl_msg_send), atom(any()), pid(&to_pid)))) {
      check_term_version(f, l);
      
      const msg_seq payload(f, l);
      access->deliver_received(payload, to_pid);
      handled = true;
    }

    return handled;
  }
};

// REG_SEND operation: tuple of {6, FromPid, Cookie, ToName}
// This operation delivers the payload following the ctrl-msg to a mailbox 
// matching the given ToName.
class reg_send_handler : public operation_dispatcher
{
public:
  reg_send_handler(access_ptr access) 
  : operation_dispatcher(access) {}

  reg_send_handler(access_ptr access, dispatcher_ptr next) 
    : operation_dispatcher(access, next) {}

  virtual bool handle(msg_seq_iter& f, const msg_seq_iter& l) const
  {
    bool handled = false;
    std::string to_name;
    pid_t from_pid; // what should I do with this one?
     
    if(matchable_range(f, l).match(make_tuple(int_(constants::ctrl_msg_reg_send), pid(&from_pid),
					                                         atom(any()), atom(&to_name)))) {
      check_term_version(f, l);
      
      const msg_seq payload(f, l);
      access->deliver_received(payload, to_name);
      handled = true;
    }

    return handled;
  }
};

}


ctrl_msg_dispatcher::ctrl_msg_dispatcher(access_ptr access)
  : operation_handler(access),
    operations_chain(new send_handler(access, dispatcher_ptr(
		     new reg_send_handler(access))))
{
}

void ctrl_msg_dispatcher::dispatch(msg_seq& msg) const
{
  // On each successfully parsed element, the first-iterator is advanced.
  msg_seq_iter first = msg.begin();
  msg_seq_iter last = msg.end();

  parse_header(first, last);

  // Header OK => now check the ctrl-message.
  operations_chain->dispatch(first, last);
}

