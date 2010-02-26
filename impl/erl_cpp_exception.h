#ifndef ERL_CPP_EXCEPTION_H
#define ERL_CPP_EXCEPTION_H

#include <exception>
#include <string>

// All exceptions thrown by tinch_pp are of this type.
namespace tinch_pp {

class erl_cpp_exception : public std::exception
{
public:
  erl_cpp_exception(const std::string& reason);

  virtual ~erl_cpp_exception() throw();

  virtual const char *what() const throw();

private:
  std::string reason;
};

class connection_io_error : public erl_cpp_exception
{
public:
  connection_io_error(const std::string& reason, 
		      const std::string& node_name);

  virtual ~connection_io_error() throw();

  std::string node() const;

private:
  std::string node_name;
};

class mailbox_receive_tmo : public erl_cpp_exception
{
public:
  mailbox_receive_tmo();

  virtual ~mailbox_receive_tmo() throw();
};

}

#endif
 
