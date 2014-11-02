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
#include <cassert>
#include <map>

namespace {

boost::optional<int> extract_type_tag(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l)
{
   boost::optional<int> tag;

   if(f != l)
      tag = static_cast<int>(*f);

   return tag;
}

}

const tinch_pp::erl::any::dynamic_element_matcher_type tinch_pp::erl::any::dynamic_element_matcher =
      {
      //    Type                              Match function
      {tinch_pp::type_tag::small_integer,     [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance) {return tinch_pp::erl::int_(instance).match(f, l);}},
      {tinch_pp::type_tag::integer,           [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance) {return tinch_pp::erl::int_(instance).match(f, l);}},
      {tinch_pp::type_tag::atom_ext,          [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance) {return tinch_pp::erl::atom(instance).match(f, l);}},
      {tinch_pp::type_tag::small_tuple,       [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance) {
        tinch_pp::msg_seq_iter start = f;

        size_t parsed_length = 0;
        bool match = tinch_pp::binary_to_term<tinch_pp::small_tuple_head_ext>(f, l, parsed_length);
        assert(match); // already checked in the previous dispatch-mechanism

        instance.save_matched_bytes(tinch_pp::msg_seq(start, f));

        for(size_t i = 0; match && (i < parsed_length); ++i)
           match &= instance.match(f, l);

        return match;}},
      {tinch_pp::type_tag::list,              [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance){
        tinch_pp::msg_seq_iter start = f;

        size_t parsed_length = 0;
        bool match = tinch_pp::binary_to_term<tinch_pp::list_head_ext>(f, l, parsed_length);
        assert(match); // already checked in the previous dispatch-mechanism

        instance.save_matched_bytes(tinch_pp::msg_seq(start, f));

        for(size_t i = 0; match && (i < parsed_length); ++i)
         match &= instance.match(f, l);

        return match;
      }},
      {tinch_pp::type_tag::string_ext,        [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance) {
        return tinch_pp::erl::e_string(instance).match(f, l);
      }},
      {tinch_pp::type_tag::pid,               [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance){
        return tinch_pp::erl::pid(instance).match(f, l);
      }},
      {tinch_pp::type_tag::new_reference_ext, [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance) {
        return tinch_pp::erl::ref(instance).match(f, l);
      }},
      {tinch_pp::type_tag::float_ext,        [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance) {
        return tinch_pp::erl::float_(instance).match(f, l);
      }},
      {tinch_pp::type_tag::binary_ext,        [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance) {
        return tinch_pp::erl::binary(instance).match(f, l);
      }},
      {tinch_pp::type_tag::bit_binary_ext,    [&](tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance) {
         return tinch_pp::erl::binary(instance).match(f, l);
      }}};

bool tinch_pp::erl::any::match_dynamically(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l, const any& instance)
{
   auto matched = false;

   if(const boost::optional<tinch_pp::erl::any::term_id_type> tag = extract_type_tag(f, l)) {
      tinch_pp::erl::any::dynamic_element_matcher_type::const_iterator m = dynamic_element_matcher.find(*tag);

      // TODO: Once we support all types, this should be considered an erronoues term (raise exception).
      if(m != dynamic_element_matcher.end()) {
         const term_matcher_type matcher = m->second;
         matched = matcher(f, l, instance);
      }
   }

   return matched;
}

tinch_pp::erl::any::any()
   : to_assign(&placeholder)
{}

tinch_pp::erl::any::any(tinch_pp::matchable_ptr* a_to_assign)
   : to_assign(a_to_assign)
{}

bool tinch_pp::erl::any::match(tinch_pp::msg_seq_iter& f, const tinch_pp::msg_seq_iter& l) const
{
   const bool res = tinch_pp::erl::any::match_dynamically(f, l, *this);
   to_assign->reset(new tinch_pp::matchable_seq(matched_raw_bytes));

   return res;
}

bool tinch_pp::erl::any::save_matched_bytes(const tinch_pp::msg_seq& raw_msg_part) const
{
   tinch_pp::erl::any::matched_raw_bytes.insert(matched_raw_bytes.end(), raw_msg_part.begin(), raw_msg_part.end());

   return true;
}
