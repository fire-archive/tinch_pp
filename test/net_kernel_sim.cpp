#include "tinch_pp/node.h"
#include "tinch_pp/mailbox.h"
#include "tinch_pp/erlang_types.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>

using namespace tinch_pp;
using namespace tinch_pp::erl;

// USAGE:
// ======
// 1. Start an Erlang node with the cookie abcdef.
// 2. (testnode@127.0.0.1)1> net_adm:ping('net_adm_test_node@127.0.0.1').
// 3. Stop the program with:
//       (testnode@127.0.0.1)2> erlang:send({net_kernel,'net_adm_test_node@127.0.0.1'}, stop).

namespace {

void net_adm_emulator(mailbox_ptr mbox);
}

int main()
{
  node my_node("net_adm_test_node@127.0.0.1", "abcdef");

  my_node.publish_port(0xACDC);

  mailbox_ptr net_adm_mailbox = my_node.create_mailbox("net_kernel");
  boost::thread net_adm(boost::bind(&net_adm_emulator, net_adm_mailbox));

  net_adm.join();
}

namespace {

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
