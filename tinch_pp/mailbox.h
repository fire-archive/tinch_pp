#ifndef MAILBOX_H
#define MAILBOX_H

#include "impl/types.h"
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace tinch_pp {

namespace erl {
  class object;
}
class matchable;
typedef boost::shared_ptr<matchable> matchable_ptr;

struct pid_t;

/// A mailbox is the distributed equivalence to an Erlang process.
/// Each mailbox is associated with its own PID.
/// Messages from other nodes are received through the mailbox and,  
/// correspondingly, outgoing messages are sent from the mailbox.
///
/// Lifetime: a mailbox is only valid as long as its node, from 
/// which it is created, exists.
///
/// In the current version of tinch++, a mailbox is implicitly closed as 
/// it goes out of scope (future versions will include an explicit close-method).
class mailbox : boost::noncopyable
{
public:
  virtual ~mailbox() {}

  virtual pid_t self() const = 0;

  virtual std::string name() const = 0;

  /// Sends the message to a remote pid.
  virtual void send(const pid_t& to, const erl::object& message) = 0;

  /// Sends the message to the named mailbox created on the same tinch++ node as this mailbox.
  /// This functionality can be seen as a thread-safe queue for the application.
  virtual void send(const std::string& to_name, const erl::object& message) = 0;

  /// Sends the message to the process registered as to_name on the given node.
  virtual void send(const std::string& to_name, const std::string& node, const erl::object& message) = 0;

  /// Blocks until a message is received in this mailbox.
  /// Returns the received messages as a matchable allowing Erlang-style pattern matching.
  virtual matchable_ptr receive() = 0;

  /// Blocks until a message is received in this mailbox or until the given time has passed.
  /// Returns the received messages as a matchable allowing Erlang-style pattern matching.
  /// In case of a time-out, an tinch_pp::receive_tmo_exception is thrown.
  virtual matchable_ptr receive(time_type_sec tmo) = 0;
};

}

#endif
