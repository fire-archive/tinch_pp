#include "tinch_pp/node.h"
#include "tinch_pp/rpc.h"
#include "tinch_pp/mailbox.h"
#include "tinch_pp/erlang_types.h"
#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>
#include <iostream>

using namespace tinch_pp;
using namespace tinch_pp::erl;
using namespace boost::assign;

// USAGE:
// ======
// 1. Start an Erlang node with the cookie abcdef.
// 2. Start the Erlang program reflect_msg:
//          (testnode@127.0.0.1)4> reflect_msg:start_link().
// 3. Start this program (will íssue an RPC and get the same message back).

int main()
{
  const std::string own_node_name("net_adm_test_node@127.0.0.1");
  const std::string remote_node_name("testnode@127.0.0.1");

  node my_node(own_node_name, "abcdef");

  rpc rpc_invoker(my_node);

  const std::list<erl::object_ptr> msg_to_echo = list_of(make_atom("hello"));

  matchable_ptr reply = rpc_invoker.blocking_rpc(remote_node_name, 
                                                 module_and_function_type("reflect_msg", "echo"), 
                                                 make_list(msg_to_echo));

  if(reply->match(make_tuple(atom("ok"), atom("hello"))))
    std::cout << "RPC reply correct!" << std::endl;
  else
    std::cerr << "Unexpected RPC reply!" << std::endl;
}
