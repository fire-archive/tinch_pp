#ifndef NETWORKER_H
#define NETWORKER_H

#include "node_connection_access.h"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

namespace tinch_pp {

template<typename State>
class networker
{
public:
  typedef networker<State> OwnType;
  typedef void (State::*ReadCallback)(utils::msg_lexer&);
  typedef void (State::*WriteCallback)();

  networker(access_ptr a_access, State* a_state)
    : access(a_access),
      state(a_state) {}

  void trigger_read(ReadCallback callback)
  {
    access->trigger_checked_read(boost::bind(callback, state, ::_1));
  }

  void trigger_write(const msg_seq& msg, WriteCallback callback)
  {
    access->trigger_checked_write(msg, boost::bind(callback, state));
  }

  void trigger_write(const msg_seq& msg)
  {
    access->trigger_checked_write(msg, boost::bind(&OwnType::ignore_write_notification, this));
  }

private:
  void ignore_write_notification()
  {
  }

private:
  access_ptr access;
  State* state;
};

}

#endif
