#ifndef NODE_CONNECTION_STATE_H
#define NODE_CONNECTION_STATE_H

#include "types.h"
#include <boost/shared_ptr.hpp>

namespace tinch_pp {

class node_connection_access;
typedef boost::shared_ptr<node_connection_access> access_ptr;

class connection_state
{
public:
  connection_state(access_ptr a_access)
    : access(a_access)
  {
  }

  virtual ~connection_state() = 0;

  // TODO: make all these functions report an error (add a std::string name() method)!!!!!!!!!!!!!!!1
  virtual void initiate_handshake(const std::string& node_name) {}

  virtual void read_incoming_handshake() {}

  virtual void send(const msg_seq& msg, const pid_t& destination_pid) {}

  virtual void send(const msg_seq& msg, const pid_t& self, const std::string& destination_name) {}

  // The error is delegated to the current state because we want to react differently 
  // depending on if we have established a connection or not.
  virtual void handle_io_error(const std::string& error) const;

protected:
  access_ptr access;
};

typedef boost::shared_ptr<connection_state> connection_state_ptr;

connection_state_ptr initial_state(access_ptr access);

}

#endif
