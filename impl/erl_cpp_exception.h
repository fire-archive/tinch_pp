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
#ifndef ERL_CPP_EXCEPTION_H
#define ERL_CPP_EXCEPTION_H

#include "types.h"
#include <exception>
#include <string>

// TODO: Rename the file and move it to the public API!

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

// A mailbox or Erlang process may set-up links to each other.
// In case a link breaks, the exception below is raised immediately in the next receive operation.
// Links get broken for different reasons, but typically:
//  - An error in network communication.
//  - The remote process sends an exit signal.
//  - The remote process terminates.
class link_broken : public erl_cpp_exception
{
public:
  link_broken(const std::string& reason, const e_pid& pid);

  virtual ~link_broken() throw();

  std::string reason() const { return reason_; }

  e_pid broken_pid() const { return pid_; }

private:
  std::string reason_;
  e_pid pid_;
}; 

}

#endif
 
