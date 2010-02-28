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
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <utility>

using namespace tinch_pp;
using namespace tinch_pp::erl;

// USAGE:
// ======
// See print_usage below.

namespace {

void print_usage()
{
  std::cout << "USAGE:" << std::endl;
  std::cout << "======" << std::endl;
  std::cout << "1. Start an Erlang node with a cookie set and a full node name." << std::endl;
  std::cout << "2. Start net_kernel_sim with the same cookie" << std::endl;
  std::cout << "3. Ping net_kernel_sim node." << std::endl;
  std::cout << "\t(testnode@127.0.0.1)1> net_adm:ping('net_adm_test_node@127.0.0.1')" << std::endl;
  std::cout << "4. Stop the program with:" << std::endl;
  std::cout << "\t(testnode@127.0.0.1)2> erlang:send({net_kernel,'net_adm_test_node@127.0.0.1'}, stop)." << std::endl;
  std::cout << "======" << std::endl;
}

void run_emulation(const std::string& node_name, const std::string& cookie);

void net_adm_emulator(mailbox_ptr mbox);

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

  run_emulation(node_name, cookie);  
}

namespace {

void run_emulation(const std::string& node_name, const std::string& cookie)
{
  node my_node(node_name.c_str(), cookie.c_str());

  my_node.publish_port(0xACDC);

  mailbox_ptr net_adm_mailbox = my_node.create_mailbox("net_kernel");
  boost::thread net_adm(boost::bind(&net_adm_emulator, net_adm_mailbox));

  net_adm.join();
}

void net_adm_emulator(mailbox_ptr mbox)
{
  bool is_running = true;

  while(is_running) {
    const matchable_ptr rec_msg = mbox->receive();
  
    // {atom, {pid, new_ref}, {atom, atom}}
    //
    // Sample msg = {$gen_call, {pid, ref}, { is_auth, anode@zarathustra }}
    std::string node;
    new_reference_type monitor_ref;
    tinch_pp::pid_t sender;

    if(rec_msg->match(erl::make_tuple(atom("$gen_call"), 
							                               make_tuple(pid(&sender), ref(&monitor_ref)),
							                               make_tuple(atom("is_auth"), atom(&node))))) {
      std::cout << "net_adm: Received ping from " << node << std::endl;
      // Reply: {Mref, Reply} where Reply = yes
      mbox->send(sender, make_tuple(ref(monitor_ref), atom("yes")));
    } else if(rec_msg->match(atom("stop"))) {
      std::cout << "net_adm: terminating upon request." << std::endl;
      is_running = false;
    } else {
      std::cout << "...but failed to match!" << std::endl;
    }
  }
}

}
