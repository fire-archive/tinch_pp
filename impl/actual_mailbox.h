// Copyright (c) 2010, Adam Petersen <adam@adampetersen.se>. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//   1. Redistributions of source code must retain the above copyright notice, this list of
//      conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright notice, this list
//      of conditions and the following disclaimer in the documentation and/or other materials
//      provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY Adam Petersen ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Adam Petersen OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
