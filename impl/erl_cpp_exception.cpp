#include "erl_cpp_exception.h"

using namespace tinch_pp;

erl_cpp_exception::erl_cpp_exception(const std::string& a_reason)
  : reason("erl_cpp exception: " + a_reason)
{
}

erl_cpp_exception::~erl_cpp_exception() throw()
{
}

const char* erl_cpp_exception::what() const throw()
{
  return reason.c_str();
}

connection_io_error::connection_io_error(const std::string& reason, 
					 const std::string& a_node_name)
  : erl_cpp_exception(reason),
    node_name(a_node_name)
{
}

connection_io_error::~connection_io_error() throw()
{
}

std::string connection_io_error::node() const
{
  return node_name;
}

mailbox_receive_tmo::mailbox_receive_tmo()
  : erl_cpp_exception("Timed out (user requested) while waiting for a message to arrive.")
{
}

mailbox_receive_tmo::~mailbox_receive_tmo() throw()
{
}
