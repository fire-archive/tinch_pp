#ifndef NODE_H
#define NODE_H

#include "impl/node_connector.h"
#include "impl/epmd_requestor.h"
#include "impl/node_access.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>
#include <string>
#include <map>

namespace tinch_pp {

class mailbox;
typedef boost::shared_ptr<mailbox> mailbox_ptr;
class actual_mailbox;

/// A node represents one, distributed C++ node connected to the EPMD.
/// A node is responsible for establishing connections with other 
/// nodes on the Erlang infrastructure. There are two ways to estblish connections:
/// 1) The C++ node as originator (by sending a message or pinging), or 
/// 2) another, remote node initiates the connection handshake.
/// As a node registers itself at EPMD, it provides a port number. The node is 
/// responsible for listening to incoming connections at that port (case 2 above).
/// 
/// Networking strategy:
/// Calls to EPMD are rare. Thus, all communication with EPMD is synchronous.
/// The communication between connected nodes uses asynchronous TCP/IP.
///
/// Threading:
/// A separate thread is used for asynchronous I/O. Shared data is protected 
/// through mutexes.
class node : node_access,
             boost::noncopyable 
{
public:
  // Creates a node using the default cookie (read from your user.home).
  //node(const std::string& node_name);

  node(const std::string& node_name, const std::string& cookie);

  ~node();

  //node(const std::string& node_name, const std::string& cookie, int incoming_connections_port);

  /// If you want other nodes to connect to this one, it has to be registered at EPMD.
  /// This method does that.
  void publish_port(port_number_type incoming_connections_port);

  /// Attempts to establish a connection to the given node.
  /// Note that connections are established implicitly as the first message to a node is sent.
  bool ping_peer(const std::string& peer_node_name);

  /// Create an unnamed mailbox to communicate with other mailboxes and/or Erlang process.
  /// All messages are sent and received through mailboxes.
  mailbox_ptr create_mailbox();

  /// Create an named mailbox to communicate with other mailboxes and/or Erlang process.
  /// Messages can be sent to this mailbox by using its registered name or its pid.
  mailbox_ptr create_mailbox(const std::string& registered_name);

  // TODO: provide an explicit way to close the mailbox once we got linked processes.
  // Consider reason sent (see Jinterface node::close(mbox).

  /// Returns a vector with the names of all nodes connected to this one.
  std::vector<std::string> connected_nodes() const;

private:
  // Take care - order of initialization matters (io_service always first).
  boost::asio::io_service io_service;
  // Prevent the io_service.run() from finishing (we don't want it to return 
  // before the node dies).
  boost::asio::io_service::work work;

  epmd_requestor epmd;

  std::string node_name_;
  std::string cookie_;
 
  node_connector connector;

   // A separate thread that runs the io_service in order to perform aynchronous requests...
  boost::thread async_io_runner;
  // ...and executes the following function:
  void run_async_io();

  // The Pids are built up from the node_name and the following variables:
  boost::uint32_t pid_id; // calculated
  boost::uint32_t serial; // calculated
  int creation;           // default 0 (zero), obtained from EPMD when registered.

  pid_t make_pid();
  void update_pid_fields();

  typedef boost::shared_ptr<actual_mailbox> actual_mailbox_ptr;
  typedef std::map<pid_t, actual_mailbox_ptr> mailboxes_type;
  mailboxes_type mailboxes;

  // Closing a mailbox isn't done right from a design perspective.
  // I'll have a look at it when implementing linked-processes.
  bool is_destructing;

  typedef std::map<std::string, actual_mailbox_ptr> registered_mailboxes_type;
  registered_mailboxes_type registered_mailboxes;

  boost::mutex mailboxes_lock;
  typedef boost::lock_guard<boost::mutex> mutex_guard;

  void remove(mailbox_ptr mailbox);
  void remove(const pid_t& id, const std::string& name);

private:
  // Implementation of node_access.
  //
  virtual std::string name() const { return node_name_; }
  
  virtual void close_mailbox(const pid_t& id, const std::string& name);

  virtual std::string cookie() const { return cookie_; }

  virtual void deliver(const msg_seq& msg, const pid_t& to);

  virtual void deliver(const msg_seq& msg, const std::string& to);

  virtual void deliver(const msg_seq& msg, const std::string& to_name, 
		                     const std::string& on_given_node, const pid_t& from_pid);

  virtual void receive_incoming(const msg_seq& msg, const pid_t& to);

  virtual void receive_incoming(const msg_seq& msg, const std::string& to);
};

}

#endif
