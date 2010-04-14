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
#include "linker.h"
#include "mailbox_controller_type.h"
#include <boost/thread/locks.hpp>
#include "boost/bind.hpp"

using namespace tinch_pp;

namespace {
typedef boost::lock_guard<boost::mutex> mutex_guard;
}

namespace tinch_pp {
  bool operator==(const std::pair<pid_t, pid_t>& v1,
                  const std::pair<pid_t, pid_t>& v2);
}

linker::linker(mailbox_controller_type& a_mailbox_controller)
  : mailbox_controller(a_mailbox_controller) 
{
}

void linker::link(const pid_t& from, const pid_t& to)
{
  const mutex_guard guard(links_lock);

  remove_link_between(from, to);

  establish_link_between(from, to);
}

void linker::unlink(const pid_t& from, const pid_t& to)
{
  const mutex_guard guard(links_lock);

  remove_link_between(from, to);
}

void linker::break_links_for_local(const pid_t& dying_process)
{
  // report...
  
}

void linker::close_links_for_local(const pid_t& dying_process, const std::string& reason)
{
}

void linker::establish_link_between(const pid_t& pid1, const pid_t& pid2)
{
  established_links.push_back(std::make_pair(pid1, pid2));
}

void linker::remove_link_between(const pid_t& pid1, const pid_t& pid2)
{
  established_links.remove(std::make_pair(pid1, pid2));
  established_links.remove(std::make_pair(pid2, pid1));
}

namespace tinch_pp {
 
bool operator==(const std::pair<pid_t, pid_t>& v1,
                const std::pair<pid_t, pid_t>& v2)
{
  return (v1.first == v2.first) && (v1.second == v2.second);
}

}

