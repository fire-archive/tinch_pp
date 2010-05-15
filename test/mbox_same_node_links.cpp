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
#include <iostream>

using namespace tinch_pp;
using namespace tinch_pp::erl;

// USAGE:
// ======
// 1. Simply run the program - no need to start Erlang (EPMD) for this test!
//
// This test links mailboxes located on the same tinch++ node.
// The feature is typically used in the case where tinch++ serves as
// an ordered, bidirectional queue between different threads in an application.

namespace {

  void break_link(node& my_node);

  void break_in_other_direction(node& my_node);

  void unlink_and_break(node& my_node);
}

int main()
{
  node my_node("queue_test@127.0.0.1", "qwerty");

  break_link(my_node);

  break_in_other_direction(my_node);

  unlink_and_break(my_node);
}

namespace {

const time_type_sec tmo = 2;

void break_link(node& my_node)
{
  mailbox_ptr worker_mbox = my_node.create_mailbox("worker");
  mailbox_ptr control_mbox = my_node.create_mailbox("controller");

  worker_mbox->link(control_mbox->self());

  worker_mbox->close();

  try {
    (void) control_mbox->receive(tmo);
    std::cerr << "Failed to report a broken link!" << std::endl;
  } catch(const link_broken&) {
    std::cout << "Success - broken link reported." << std::endl;
  }
}

void break_in_other_direction(node& my_node)
{
  mailbox_ptr worker_mbox = my_node.create_mailbox("worker");
  mailbox_ptr control_mbox = my_node.create_mailbox("controller");

  worker_mbox->link(control_mbox->self());

  control_mbox->close();

  try {
    (void) worker_mbox->receive(tmo);
    std::cerr << "Failed to report a broken link (reversed direction)!" << std::endl;
  } catch(const link_broken&) {
    std::cout << "Success - broken link reported (reversed direction)." << std::endl;
  }
}

void unlink_and_break(node& my_node)
{
  mailbox_ptr worker_mbox = my_node.create_mailbox("worker");
  mailbox_ptr control_mbox = my_node.create_mailbox("controller");

  worker_mbox->link(control_mbox->self());

  control_mbox->unlink(worker_mbox->self());

  control_mbox->close();
  worker_mbox->close();

  std::cout << "Successfull unlink." << std::endl;
}

}
