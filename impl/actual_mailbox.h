#ifndef ACTUAL_MAILBOX_H
#define ACTUAL_MAILBOX_H

#include "tinch_pp/mailbox.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <list>

namespace tinch_pp {

class node_access;

class actual_mailbox : public mailbox
{
public:
  actual_mailbox(node_access& node, const pid_t& own_pid, boost::asio::io_service& service);

  actual_mailbox(node_access& node, const pid_t& own_pid, 
                 boost::asio::io_service& service, 
                 const std::string& own_name);

  // Implementation of the mailbox-interface:
  //
  virtual pid_t self() const;

  virtual std::string name() const;

  virtual void send(const pid_t& to, const erl::object& message);

  virtual void send(const std::string& to_name, const erl::object& message);

  virtual void send(const std::string& to_name, const std::string& node, const erl::object& message);

  // Blocks until a message is received in this mailbox.
  // Returns the received messages as a matchable allowing Erlang-style pattern matching.
  virtual matchable_ptr receive();

  // Blocks until a message is received in this mailbox or until the given time has passed.
  // Returns the received messages as a matchable allowing Erlang-style pattern matching.
  // In case of a time-out, an tinch_pp::mailbox_receive_tmo is thrown.
  virtual matchable_ptr receive(time_type_sec tmo);

  // The public interface for the implementation (i.e. the owning node):
  //
  void on_incoming(const msg_seq& msg);

private:
  void wait_for_at_least_one_message(boost::unique_lock<boost::mutex>& lock);

  msg_seq pick_first_msg();

  void receive_tmo(const boost::system::error_code& error);

private:
  node_access& node;
  pid_t own_pid;
  boost::asio::io_service& service;
  std::string own_name;

  typedef std::list<msg_seq> received_msgs_type;
  received_msgs_type received_msgs;

  // The client typically blocks in a receive until a message arrives.
  // The messages arrive (from a node_connection) in another thread context.
  boost::condition_variable message_received_cond;
  boost::mutex received_msgs_mutex;
  bool message_ready;
};

}

#endif
