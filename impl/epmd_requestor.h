#ifndef EPMD_REQUESTOR_H
#define EPMD_REQUESTOR_H

// Encapsulates the communication with EPMD (Erlang Port Mapper Daemon).
// EPMD requests are rare so, for simplicity, all requests are modelled 
// as syncrhonous.
#include "types.h"
#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <string>

namespace tinch_pp {

class epmd_requestor : boost::noncopyable
{
public:
  epmd_requestor(boost::asio::io_service& io_service, const std::string& epmd_host, port_number_type epmd_port);

  void connect();

  creation_number_type alive2_request(const std::string& node_name, port_number_type incoming_connections);

  port_number_type port_please2_request(const std::string& peer_node);

private:
  boost::asio::io_service& io_service;
  boost::asio::ip::tcp::socket epmd_socket;

  std::string epmd_host;
  port_number_type epmd_port;
};


}

#endif
