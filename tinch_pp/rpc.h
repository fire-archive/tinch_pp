#ifndef RPC_H
#define RPC_H

#include "erlang_value_types.h"
#include "erl_list.h"
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <utility>
#include <string>

namespace tinch_pp {

typedef std::pair<std::string, std::string> module_and_function_type;

/// The arguments may be of different types. Thus, we need to 
/// heap allocate them in order to provide polymorphic behaviour.
/// Implementation note: I don't like that requirement; investigate 
/// a cleaner way (intrusive containers)?
typedef erl::list<erl::object_ptr> rpc_argument_type;

class node;
class mailbox;
typedef boost::shared_ptr<mailbox> mailbox_ptr;
class matchable;
typedef boost::shared_ptr<matchable> matchable_ptr;

/// This is a convenience class for remote procedure calls.
/// A remote procedure call is a method to call a function on a remote node and collect the answer.
///
/// NOTE: The rpc protocol expects the remote node to be an Erlang node. If it's not, invoking an 
/// rpc has unpredictable results (i.e. do NOT do it unless you really, really know what you do).
class rpc : boost::noncopyable
{
public:
   rpc(node& own_node);

   /// Invokes the given remote function on the remote node.
   /// This call blocks until a reply is received.
   matchable_ptr blocking_rpc(const std::string& remote_node,
                              const module_and_function_type& remote_fn,
                              const rpc_argument_type& arguments);
   // TODO: implement on a rainy day...

   //matchable_ptr blocking_rpc(const std::string& remote_node,
                              //const module_and_function_type& remote_fn,
                              //const rpc_argument_type& arguments,
                              //time_type tmo);

   //matchable_ptr async_rpc(const std::string& remote_node,
                              //const module_and_function_type& remote_fn,
                              //const rpc_argument_type& arguments,
                              //const rpc_callback& calback);

   //matchable_ptr async_rpc(const std::string& remote_node,
                              //const module_and_function_type& remote_fn,
                              //const rpc_argument_type& arguments,
                              //const rpc_callback& calback,
                              //time_type tmo);

private:
   mailbox_ptr mbox;
};

}

#endif
