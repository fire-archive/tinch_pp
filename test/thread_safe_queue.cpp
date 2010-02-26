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
// 1. Simply run the program - no need to start Erlang (EPMD) for this test!
//
// This test uses the tinch++ feature of sending messages to mailboxes on the 
// same tinch++ node. Such a feature can be used as an ordered, bidirectional 
// queue between different threads in an application.

namespace {

void control_thread(mailbox_ptr mbox, size_t times);

void worker_thread(mailbox_ptr mbox);

}

int main()
{
  node my_node("queue_test@127.0.0.1", "qwerty");

  // The worker won't leave until all orders are done.
  const size_t number_of_orders = 10;

  mailbox_ptr worker_mbox = my_node.create_mailbox("worker");
  boost::thread worker(boost::bind(&worker_thread, worker_mbox));

  mailbox_ptr control_mbox = my_node.create_mailbox("controller");
  boost::thread controller(boost::bind(&control_thread, control_mbox, number_of_orders));
  
  controller.join();
  worker.join();
}

namespace {

void control_thread(mailbox_ptr mbox, size_t times)
{
  if(times > 0) {
    std::cout << "controller: requesting order " << times << std::endl;

    mbox->send("worker", atom("work"));

    const matchable_ptr result = mbox->receive();

    if(!result->match(atom("done")))
      throw std::exception("control_thread: unexpected reply from worker");

    std::cout << "controller: worker finished order " << times << std::endl;

    control_thread(mbox, --times);
  } else {
    mbox->send("worker", atom("go home"));
    // terminating...
  }
}

void worker_thread(mailbox_ptr mbox)
{
  const bool long_hard_day = true;

  while(long_hard_day) {
    const matchable_ptr request = mbox->receive();

    if(request->match(atom("work")))
      mbox->send("controller", atom("done"));
    else if(request->match(atom("go home")))
      return;
    else
      std::cerr << "Me, the worker, do not understand the controller..." << std::endl;
  }
}

}
