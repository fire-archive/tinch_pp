#ifndef CTRL_MSG_DISPATCHER_H
#define CTRL_MSG_DISPATCHER_H

#include "types.h"
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace tinch_pp {

class node_connection_access;
typedef boost::shared_ptr<node_connection_access> access_ptr;

class operation_dispatcher;
typedef boost::shared_ptr<operation_dispatcher> dispatcher_ptr;

// A control message is a tuple, sent between connected nodes, where the first element 
// indicates which distributed operation it encodes. Examples of operations include: send, registered send,
// exit, etc.
// This class parses the message and dispatches to the handle responsible for the encoded operation.
class ctrl_msg_dispatcher : boost::noncopyable
{
public:
  ctrl_msg_dispatcher(access_ptr operation_handler);

  void dispatch(msg_seq& msg) const;

private:
  access_ptr operation_handler;

  dispatcher_ptr operations_chain;
};

}

#endif
