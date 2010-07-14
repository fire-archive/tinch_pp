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
#include "tinch_pp/rpc.h"
#include "tinch_pp/node.h"
#include "tinch_pp/mailbox.h"
#include "tinch_pp/erlang_types.h"
#include "tinch_pp/exceptions.h"

using namespace tinch_pp;
using namespace tinch_pp::erl;
namespace fusion = boost::fusion;

namespace {

// {self, { call, Mod, Fun, Args, user}}
typedef fusion::tuple<atom, atom, atom, rpc_argument_type, atom> call_type;
typedef fusion::tuple<pid, e_tuple<call_type> > rpc_call_type;

matchable_ptr receive_rpc_reply(mailbox_ptr mbox,
                                const std::string& remote_node,
                                const module_and_function_type& remote_fn);
}

rpc::rpc(node_ptr own_node)
   : mbox(own_node->create_mailbox())
{
}

matchable_ptr rpc::blocking_rpc(const std::string& remote_node,
                                const module_and_function_type& remote_fn,
                                const rpc_argument_type& arguments)
{
   const call_type call(atom("call"), atom(remote_fn.first), atom(remote_fn.second), arguments, atom("user"));

   const rpc_call_type rpc_call(pid(mbox->self()), e_tuple<call_type>(call));

   mbox->send("rex", remote_node, e_tuple<rpc_call_type>(rpc_call));

   return receive_rpc_reply(mbox, remote_node, remote_fn);
}

namespace {

matchable_ptr receive_rpc_reply(mailbox_ptr mbox,
                                const std::string& remote_node,
                                const module_and_function_type& remote_fn)
{
   // { rex, Term }
   matchable_ptr result = mbox->receive();

   matchable_ptr reply_part;

   if(!result->match(make_e_tuple(atom("rex"), any(&reply_part)))) {
      const std::string reason = "RPC: Unexpected result from call to " + remote_node + 
                                 ", function " + remote_fn.first + ":" + remote_fn.second;
      throw tinch_pp_exception(reason);
   }

   return reply_part;
}

}
