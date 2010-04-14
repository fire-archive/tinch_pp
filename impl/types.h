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
#ifndef TYPES_H
#define TYPES_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/cstdint.hpp>
#include <vector>
#include <string>

namespace tinch_pp {

typedef std::vector<char> msg_seq;
typedef std::back_insert_iterator<msg_seq> msg_seq_out_iter;
typedef msg_seq::iterator msg_seq_iter;
typedef msg_seq::const_iterator msg_seq_citer;

// TODO: boost types!
typedef int creation_number_type;
typedef int port_number_type;

typedef boost::uint32_t time_type_sec;

// This is me giving up - I can't the binary generators to work with attributes transformations :-(
struct serializable_string
{
  explicit serializable_string(const std::string& a_val)
    : size(a_val.size()),
      val(a_val)
  {}

  size_t size;
  std::string val;
};

bool operator ==(const serializable_string& s1, const serializable_string& s2);

struct pid_t
{
  pid_t(const std::string& a_node_name,
        boost::uint32_t a_id,
        boost::uint32_t a_serial,
        boost::uint32_t a_creation)
  : node_name(a_node_name),
    id(a_id), serial(a_serial), creation(a_creation) {}

  // Creates an invalid pid (not that nice, but necessary(?)
  // when matching received messages).
  pid_t()
    : node_name(""), id(0), serial(0), creation(0) {}

  std::string node_name;
  boost::uint32_t id;
  boost::uint32_t serial;
  boost::uint32_t creation;
};

bool operator ==(const pid_t& p1, const pid_t& p2);

struct serializable_pid_t
{
  explicit serializable_pid_t(const pid_t& p)
  : node_name(p.node_name),
    id(p.id), serial(p.serial), creation(p.creation) {}

  serializable_string node_name;
  boost::uint32_t id;
  boost::uint32_t serial;
  boost::uint32_t creation;
};

struct new_reference_type
{
   new_reference_type(const std::string& a_node_name,
		      boost::uint32_t a_creation,
		      const msg_seq& a_id)
  : node_name(a_node_name),
    creation(a_creation),
    id(a_id.begin(), a_id.end()) {}

  // Creates an invalid new_reference (not that nice, but necessary(?)
  // when matching received messages).
  new_reference_type()
    : node_name(""), creation(0) {}

  std::string node_name;
  boost::uint32_t creation;
  msg_seq id; // should be regarded as uninterpreted data
};

bool operator ==(const new_reference_type& r1, const new_reference_type& r2);

// Intended for easier serialization.
struct new_reference_g_type
{
  new_reference_g_type(const new_reference_type& ref)
    : id_length(ref.id.size() / sizeof(boost::uint32_t)),
      node_name(ref.node_name),
      creation(ref.creation),
      id(ref.id.begin(), ref.id.end()) {}

  size_t id_length;
  serializable_string node_name;
  boost::uint32_t creation;
  msg_seq id; // should be regarded as uninterpreted data
};

}

// Adapt in the global namespace.
BOOST_FUSION_ADAPT_STRUCT(
   tinch_pp::serializable_string,
   (size_t, size)
   (std::string, val))

BOOST_FUSION_ADAPT_STRUCT(
   tinch_pp::pid_t,
   (std::string, node_name)
   (boost::uint32_t, id)
   (boost::uint32_t, serial)
   (boost::uint32_t, creation))

BOOST_FUSION_ADAPT_STRUCT(
   tinch_pp::serializable_pid_t,
   (tinch_pp::serializable_string, node_name)
   (boost::uint32_t, id)
   (boost::uint32_t, serial)
   (boost::uint32_t, creation))

BOOST_FUSION_ADAPT_STRUCT(
   tinch_pp::new_reference_type,
   (std::string, node_name)
   (boost::uint32_t, creation)
   (tinch_pp::msg_seq, id))

BOOST_FUSION_ADAPT_STRUCT(
   tinch_pp::new_reference_g_type,
   (size_t, id_length)
   (tinch_pp::serializable_string, node_name)
   (boost::uint32_t, creation)
   (tinch_pp::msg_seq, id))

#endif
