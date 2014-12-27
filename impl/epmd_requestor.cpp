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
#include "epmd_requestor.h"
#include "epmd_protocol.h"
#include "tinch_pp/exceptions.h"
#include "utils.h"

namespace asio = boost::asio;
using boost::asio::ip::tcp;

// TODO: extract a TCP/IP async reader? As mixin?

namespace {

  void send_to_epmd(tcp::socket& epmd_socket, const tinch_pp::msg_seq& epmd_msg);

  tinch_pp::msg_seq alive2_req(const std::string& node_name, const int port);

  tinch_pp::creation_number_type receive_alive_response(tcp::socket& epmd_socket);

  tinch_pp::msg_seq port_please2_req(const std::string& peer_node);

  tinch_pp::port_number_type receive_port_resp(tcp::socket& one_shot, const std::string& peer_node);
}

  tinch_pp::epmd_requestor::epmd_requestor(boost::asio::io_service& a_io_service,
			       const std::string& a_epmd_host, 
			       port_number_type a_epmd_port)
  : io_service(a_io_service),
    epmd_socket(io_service),
    epmd_host(a_epmd_host),
    epmd_port(a_epmd_port)
{
}

void tinch_pp::epmd_requestor::connect()
{
  utils::connect_socket(io_service, epmd_socket, epmd_host, epmd_port);
}

tinch_pp::creation_number_type tinch_pp::epmd_requestor::alive2_request(const std::string& node_name, port_number_type incoming_connections)
{
  send_to_epmd(epmd_socket, alive2_req(node_name, incoming_connections));
  return receive_alive_response(epmd_socket);
}

tinch_pp::port_number_type tinch_pp::epmd_requestor::port_please2_request(const std::string& peer_node)
{
   // This is a one-shot request (i.e. establish a new connection uniquely for this request-response sequence).
  tcp::socket one_shot(io_service);
  utils::connect_socket(io_service, one_shot, epmd_host, epmd_port);

  send_to_epmd(one_shot, port_please2_req(peer_node));
  
  return receive_port_resp(one_shot, peer_node);
}

namespace {

void send_to_epmd(tcp::socket& epmd_socket, const tinch_pp::msg_seq& epmd_msg)
{
  boost::system::error_code error;

  asio::write(epmd_socket, asio::buffer(epmd_msg), asio::transfer_all(), error);

  if(error) {
    throw tinch_pp::tinch_pp_exception("Write failure to EPMD: "); // TODO: add error!
  }
}

tinch_pp::msg_seq alive2_req(const std::string& node_name, const int port)
{
  tinch_pp::epmd::alive2_req_params params(node_name, port);
  tinch_pp::epmd::alive2_req req;
  tinch_pp::msg_seq m;
  
  tinch_pp::utils::generate(m, req, params);

  return m;
}

tinch_pp::creation_number_type receive_alive_response(tcp::socket& epmd_socket)
{
  tinch_pp::msg_seq msg(16);
  boost::system::error_code error;

  epmd_socket.read_some(asio::buffer(msg), error); // TODO: encapsule: read length, read until.

  if(error)
    throw tinch_pp::tinch_pp_exception("Failed to read ALIVE2_RESP from EPMD");

  tinch_pp::epmd::alive2_resp_result response;
  tinch_pp::epmd::alive2_resp alive2_resp;
  tinch_pp::utils::parse(msg, alive2_resp, response);

  if(response.result != 0)
    throw tinch_pp::tinch_pp_exception("Failed to register node at EPMD, result = " +
			    boost::lexical_cast<std::string>(response.result));

  return response.creation;
}

tinch_pp::msg_seq port_please2_req(const std::string& peer_node)
{
  const size_t request_length = 1 + peer_node.size();
  tinch_pp::msg_seq m;

  tinch_pp::utils::generate(m, karma::big_word(request_length) << karma::byte_(122) << karma::string, peer_node);

  return m;
}

tinch_pp::port_number_type receive_port_resp(tcp::socket& one_shot, const std::string& peer_node)
{
  tinch_pp::msg_seq msg(128);
  boost::system::error_code error;

  one_shot.read_some(asio::buffer(msg), error); // TODO: encapsule: read length, read until.

  if(error)
    throw tinch_pp::tinch_pp_exception("Failed to read PORT2_RESP from EPMD");

  // Parse in two steps; in case we failed, nothing more than a result code is returned.
  int result = 1; // failure
  tinch_pp::epmd::port2_resp_result port2_resp_result;

  tinch_pp::utils::parse(msg, port2_resp_result, result);

  if(result != 0) 
    throw tinch_pp::tinch_pp_exception("EPMD denies the port number for the node = " + peer_node + ". Connection aborted.");

  // In the second parse step we only parse until the port number. There's more information, but we 
  // ignore it for now (TODO: perhaps it's a good idea to verify versions?).
  int port = 0;
  tinch_pp::utils::parse(msg, qi::omit[port2_resp_result] >> qi::big_word, port);

  return port;
}

}
