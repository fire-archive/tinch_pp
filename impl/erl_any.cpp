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
#include "tinch_pp/erl_any.h"
#include "tinch_pp/erlang_types.h"
#include "tinch_pp/exceptions.h"
#include "term_conversions.h"
#include "ext_term_grammar.h"
#include "matchable_seq.h"
#include <boost/optional.hpp>
#include <boost/assign/list_of.hpp>
#include <cassert>

using namespace tinch_pp::erl;
using namespace boost::assign;

namespace {

bool match_int(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   return int_(instance).match(f, l);
}

bool match_atom(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   return atom(instance).match(f, l);
}

bool match_string(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   return e_string(instance).match(f, l);
}

bool match_float(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   return float_(instance).match(f, l);
}

bool match_reference(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
  return tinch_pp::erl::ref(instance).match(f, l);
}

bool match_pid(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   return pid(instance).match(f, l);
}

bool match_binary(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   return binary(instance).match(f, l);
}

bool match_tuple(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   tinch_pp::msg_seq_iter start = f;

   size_t parsed_length = 0;
   bool match = tinch_pp::binary_to_term<tinch_pp::small_tuple_head_ext>(f, l, parsed_length);
   assert(match); // already checked in the previous dispatch-mechanism

   instance.save_matched_bytes(tinch_pp::msg_seq(start, f));

   for(size_t i = 0; match && (i < parsed_length); ++i)
      match &= instance.match(f, l);

   return match;
}

bool match_list(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   tinch_pp::msg_seq_iter start = f;

   size_t parsed_length = 0;
   bool match = tinch_pp::binary_to_term<tinch_pp::list_head_ext>(f, l, parsed_length);
   assert(match); // already checked in the previous dispatch-mechanism

   instance.save_matched_bytes(tinch_pp::msg_seq(start, f));

   for(size_t i = 0; match && (i < parsed_length); ++i)
      match &= instance.match(f, l);

   return match;
}

boost::optional<int> extract_type_tag(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l)
{
   boost::optional<int> tag;

   if(f != l)
      tag = static_cast<int>(*f);

   return tag;
}

}

const any::dynamic_element_matcher_type any::dynamic_element_matcher = 
   map_list_of
      //    Type                    Match function
      (type_tag::small_integer,     std::bind(match_int,        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::integer,           std::bind(match_int,        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::atom_ext,          std::bind(match_atom,       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::small_tuple,       std::bind(match_tuple,      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::list,              std::bind(match_list,       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::string_ext,        std::bind(match_string,     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::pid,               std::bind(match_pid,        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::new_reference_ext, std::bind(match_reference,  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::float_ext,         std::bind(match_float,      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::binary_ext,        std::bind(match_binary,     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
      (type_tag::bit_binary_ext,    std::bind(match_binary,     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

bool any::match_dynamically(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   bool matched = false;

   if(const boost::optional<term_id_type> tag = extract_type_tag(f, l)) {
      dynamic_element_matcher_type::const_iterator m = dynamic_element_matcher.find(*tag);

      // TODO: Once we support all types, this should be considered an erronoues term (raise exception).
      if(m != dynamic_element_matcher.end()) {
         const term_matcher_type matcher = m->second;
         matched = matcher(f, l, instance);
      }
   }

   return matched;
}

any::any()
   : to_assign(&placeholder)
{}

any::any(matchable_ptr* a_to_assign)
   : to_assign(a_to_assign)
{}

bool any::match(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l) const
{
   const bool res = match_dynamically(f, l, *this);
   to_assign->reset(new matchable_seq(matched_raw_bytes));

   return res;
}

bool any::save_matched_bytes(const msg_seq& raw_msg_part) const
{
   matched_raw_bytes.insert(matched_raw_bytes.end(), raw_msg_part.begin(), raw_msg_part.end());

   return true;
}
