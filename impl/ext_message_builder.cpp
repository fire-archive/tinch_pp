#include "ext_message_builder.h"
#include "tinch_pp/erlang_types.h"
#include "constants.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <algorithm>

using namespace tinch_pp;
using namespace tinch_pp::erl;
using namespace boost;
using namespace std;

namespace {

// Design: The different send methods are similiar, just the control message differs.
// Thus, we parameterize with a generator (basically emulating closures).
typedef function<void (msg_seq_out_iter&)> add_ctrl_msg_fn;
   
void add_ctrl_message_send(msg_seq_out_iter& out, const tinch_pp::pid_t& destination_pid);

void add_ctrl_message_reg_send(msg_seq_out_iter& out, const tinch_pp::pid_t& self, const string& destination_name);

  msg_seq build(const msg_seq& payload, const add_ctrl_msg_fn& add_ctrl_msg);
}

namespace tinch_pp {
 
msg_seq build_send_msg(const msg_seq& payload, const pid_t& destination_pid)
{
  return build(payload, bind(&add_ctrl_message_send, _1, cref(destination_pid)));
}

msg_seq build_reg_send_msg(const msg_seq& payload, const pid_t& self, const string& destination_name)
{
  return build(payload, bind(&add_ctrl_message_reg_send, _1, cref(self), cref(destination_name)));
}

}

namespace {

 void add_message_header(msg_seq_out_iter& out)
  {
    *out++ = constants::pass_through;
    *out++ = constants::magic_version;
  }

// Used when sending to a PID.
  void add_ctrl_message_send(msg_seq_out_iter& out, const tinch_pp::pid_t& destination_pid)
  {
    add_message_header(out);

    const string no_cookie;

    // SEND: tuple of {2, Cookie, ToPid} 
    make_tuple(int_(constants::ctrl_msg_send), atom(no_cookie), pid(destination_pid)).serialize(out);
  }

// Used when sending to a registered name on a node.
  void add_ctrl_message_reg_send(msg_seq_out_iter& out, const tinch_pp::pid_t& self, const string& destination_name)
  {
    add_message_header(out);

    const std::string no_cookie;

    // REG_SEND: tuple of {6, FromPid, Cookie, ToName} 
    make_tuple(int_(constants::ctrl_msg_reg_send), pid(self), atom(no_cookie), atom(destination_name)).serialize(out);
  }

  const size_t header_size = 4;

  void fill_in_message_size(msg_seq& msg)
  {
    const size_t size = msg.size() - header_size;

    msg_seq_iter size_iter = msg.begin();
    karma::generate(size_iter, karma::big_dword, size);
  }

  void append_payload(msg_seq_out_iter& out, const msg_seq& payload)
  {
    // Preprend the magic version to the term (as they say in that movie, there can only be;
    // compound types don't include a version for each of their elements.
    *out++ = constants::magic_version;

    copy(payload.begin(), payload.end(), out);
  }

  msg_seq build(const msg_seq& payload, const add_ctrl_msg_fn& add_ctrl_msg)
  {
    msg_seq msg(header_size);
    msg_seq_out_iter out(msg);

    add_ctrl_msg(out);

    append_payload(out, payload);

    fill_in_message_size(msg);

    return msg;
  }

}
