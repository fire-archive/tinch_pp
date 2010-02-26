#ifndef NODE_ACCESS_H
#define NODE_ACCESS_H

#include "types.h"

namespace tinch_pp {

class node_access
{
protected:
  ~node_access() {};
public:
  virtual std::string name() const = 0;
  
  virtual std::string cookie() const = 0;

  virtual void deliver(const msg_seq& msg, const pid_t& to) = 0;

  virtual void deliver(const msg_seq& msg, const std::string& to) = 0;

  virtual void deliver(const msg_seq& msg, 
		                     const std::string& to_name, 
		                     const std::string& on_given_node,
		                     const pid_t& from_pid) = 0;

  virtual void receive_incoming(const msg_seq& msg, const pid_t& to) = 0;

  virtual void receive_incoming(const msg_seq& msg, const std::string& to) = 0;
};

}

#endif
