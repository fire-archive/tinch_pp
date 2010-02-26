#ifndef NODE_CONNECTOR_H
#define NODE_CONNECTOR_H

// Our distributed C++ node is connected to other, remote nodes through TCP/IP.
// A connection is established either explicitly (by pinging a node) or implicitly 
// by the first message sent to another node.
// This class is responsible for establishing the connections. The first step is to 
// identify where the node is. That's done through a request to EPMD on that host.
// Once a connection has been established, it is maintained by this class.
#include "node_connection.h"
#include "types.h"
#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <string>
#include <map>

namespace tinch_pp {

class node_access;

class node_connector : boost::noncopyable
{
public:
  node_connector(node_access& node, 
		 boost::asio::io_service& io_service);

  void start_accept_incoming(port_number_type port_no);

  node_connection_ptr get_connection_to(const std::string& node_name);

  void drop_connection_to(const std::string& node_name);

  std::vector<std::string> connected_nodes() const;

private:
  node_access& node;
  boost::asio::io_service& io_service;

  typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ptr;
  acceptor_ptr incoming_connections_acceptor;

  node_connection_ptr make_new_connection(const std::string& peer_node_name, 
					  boost::unique_lock<boost::mutex>& lock);

  bool wait_for_handshake_result(boost::unique_lock<boost::mutex>& lock);

  void trigger_accept();

  // Threading: everything below is executed in the async_io context - take care!
  //

  // The asynchronous callbacks are encapsulated in the following functions:
  void handle_accept(node_connection_ptr new_connection, 
		     const boost::system::error_code& error);
  
  void handshake_success(bool succeeded);

  void handshake_success_as_B(node_connection_ptr potentially_connected, bool succeeded);

  // Remember the connections to other nodes...
  typedef std::map<std::string /* node name */, node_connection_ptr> node_connections_type;
  node_connections_type node_connections;

  // ..and protect the node_connections:
  boost::condition_variable handshake_cond;
  mutable boost::mutex node_connections_mutex;
  boost::optional<bool> handshake_done;

  // When establishing a connection, our node must provide a challenge for the peer node.
  // And according to the Erlang documentation, "the challenges are expected to be very random numbers."
  boost::uniform_int<boost::uint32_t> challenge_dist;

  typedef boost::minstd_rand base_generator_type;
  base_generator_type generator;
  boost::variate_generator<base_generator_type&, boost::uniform_int<boost::uint32_t> > challenge_generator;
};

}

#endif
