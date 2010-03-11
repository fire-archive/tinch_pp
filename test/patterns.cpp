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
#include "tinch_pp/node.h"
#include "tinch_pp/mailbox.h"
#include "tinch_pp/erlang_types.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/program_options.hpp>
#include <iostream>

using namespace tinch_pp;
using namespace tinch_pp::erl;
using namespace boost;
using namespace boost::assign;

// USAGE:
// ======
// 1. Start an Erlang node with the cookie abcdef.
// 2. Start the Erlang program reflect_msg:
//          (testnode@127.0.0.1)4> reflect_msg:start_link().
// 3. Start this program. The program will send different messages 
// to reflect_msg, which echoes the messages back.

namespace {

void echo_atom(mailbox_ptr mbox);

void echo_nested_tuples(mailbox_ptr mbox, const std::string& name);

void echo_list(mailbox_ptr mbox);

void echo_empty_tuple(mailbox_ptr mbox);

void echo_string(mailbox_ptr mbox, const std::string& msg_name);

void echo_float(mailbox_ptr mbox, const std::string& msg_name);

// TODO: we can send such a list, but not match it...yet!
void echo_heterogenous_list(mailbox_ptr mbox);

typedef boost::function<void (mailbox_ptr)> sender_fn_type;

}

void print_usage()
{
  std::cout << "USAGE:" << std::endl;
  std::cout << "======" << std::endl;
  std::cout << "1. Start an Erlang node with a cookie set and a full node name." << std::endl;
  std::cout << "2. Start the Erlang program reflect_msg:" << std::endl;
  std::cout << "\t(testnode@127.0.0.1)4> reflect_msg:start_link()." << std::endl;
  std::cout << "3. Start this program (with thesame cookie)." << std::endl;
  std::cout << "\tThe program will send different messages to reflect_msg, which echoes the messages back." << std::endl;
  std::cout << "======" << std::endl;
}

int main(int ac, char* av[])
{
  std::string node_name;
  std::string cookie;

  boost::program_options::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("node", boost::program_options::value<std::string>(&node_name)->default_value("net_adm_test_node@127.0.0.1"), "set node")
    ("cookie", boost::program_options::value<std::string>(&cookie)->default_value("abcdef"), "set cookie");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
  boost::program_options::notify(vm);

  if(vm.count("help"))
    {
      print_usage();
      std::cout << std::endl;
      std::cout << desc << std::endl;
      return 1;
    }

  node my_node(node_name.c_str(), cookie.c_str());

  mailbox_ptr mbox = my_node.create_mailbox();

  const sender_fn_type senders[] = {bind(echo_atom, ::_1), bind(echo_atom, ::_1),
                                    bind(echo_nested_tuples, ::_1, "start"), bind(echo_nested_tuples, ::_1, "next"),
                                    bind(echo_empty_tuple, ::_1),
                                    bind(echo_list, ::_1), bind(echo_list, ::_1),
                                    bind(echo_string, ::_1, "start"), bind(echo_string, ::_1, "next"),
                                    bind(echo_float, ::_1, "start"), bind(echo_float, ::_1, "next")};
  const size_t number_of_senders = sizeof senders / sizeof senders[0];

  for(size_t i = 0; i < number_of_senders; ++i)
    senders[i](mbox);
}

namespace {

const std::string remote_node_name("testnode@127.0.0.1");
const std::string to_name("reflect_msg");

// All messages have the following format: {echo, self(), Msg}

void echo_atom(mailbox_ptr mbox)
{
  mbox->send(to_name, remote_node_name, erl::make_tuple(atom("echo"), pid(mbox->self()), atom("hello")));

  const matchable_ptr reply = mbox->receive();

  std::string name;

  if(reply->match(atom("hello")))
    std::cout << "Matched atom(hello)" << std::endl;
  else if(reply->match(atom(&name)))
    std::cout << "Matched atom(" << name << ")" << std::endl;
  else
    std::cerr << "No match - unexpected message!" << std::endl;
}

void echo_nested_tuples(mailbox_ptr mbox, const std::string& name)
{
  mbox->send(to_name, remote_node_name, erl::make_tuple(atom("echo"), pid(mbox->self()), 
                                                        erl::make_tuple(atom(name), 
                                                        erl::make_tuple(atom("nested"), int_(42)))));
  const matchable_ptr reply = mbox->receive();

  if(reply->match(erl::make_tuple(atom("start"), erl::any())))
    std::cout << "Matched {start, _}" << std::endl;
  else if(reply->match(erl::make_tuple(atom("next"), erl::make_tuple(atom("nested"), int_(42)))))
    std::cout << "Matched {next, {nested, 42}}" << std::endl;
  else
    std::cerr << "No match - unexpected message!" << std::endl;
}

void echo_empty_tuple(mailbox_ptr mbox)
{
  mbox->send(to_name, remote_node_name, erl::make_tuple(atom("echo"), pid(mbox->self()), 
                                                        erl::make_tuple()));
  const matchable_ptr reply = mbox->receive();

  if(reply->match(erl::make_tuple()))
    std::cout << "Matched empty tuple {}" << std::endl;
  else
    std::cerr << "No match - unexpected message!" << std::endl;
}

void echo_string(mailbox_ptr mbox, const std::string& msg_name)
{
  mbox->send(to_name, remote_node_name, erl::make_tuple(atom("echo"), pid(mbox->self()), 
                                                        erl::make_tuple(atom(msg_name), estring("my string"))));

  const matchable_ptr reply = mbox->receive();

  std::string matched_val;

  if(reply->match(erl::make_tuple(atom("start"), erl::any())))
    std::cout << "Matched string {start, _}" << std::endl;
  else if(reply->match(erl::make_tuple(atom("next"), estring(&matched_val))))
    std::cout << "Matched string {start, " << matched_val << "}" << std::endl;
  else
    std::cerr << "No match - unexpected message!" << std::endl;
}

void echo_float(mailbox_ptr mbox, const std::string& msg_name)
{
  const double value = 1234567.98765;
  mbox->send(to_name, remote_node_name, erl::make_tuple(atom("echo"), pid(mbox->self()), 
                                                        erl::make_tuple(atom(msg_name), float_(value))));

  const matchable_ptr reply = mbox->receive();

  double matched_val;

  if(reply->match(erl::make_tuple(atom("start"), erl::any())))
    std::cout << "Matched float {start, _}" << std::endl;
  else if(reply->match(erl::make_tuple(atom("next"), float_(&matched_val))))
    std::cout << "Matched float {start, " << matched_val << "}" << std::endl;
  else
    std::cerr << "No match - unexpected message!" << std::endl;
}

void echo_list(mailbox_ptr mbox)
{
  const std::list<erl::object_ptr> send_numbers = list_of(make_int(1))(make_int(2))(make_int(1000));

  mbox->send(to_name, remote_node_name, erl::make_tuple(atom("echo"), pid(mbox->self()), 
                                                        erl::make_tuple(atom("numbers"), make_list(send_numbers))));

  const matchable_ptr reply = mbox->receive();

  std::list<int_> numbers;

  if(reply->match(erl::make_tuple(atom("numbers"), make_list(&numbers))))
    std::cout << "Matched {numbers, List} with List size = " << numbers.size() << std::endl;
  else if(reply->match(erl::make_tuple(atom("start"), erl::any())))
    std::cout << "Matched {start, _}" << std::endl;
  else
    std::cerr << "No match - unexpected message!" << std::endl;
}

}
