#include "tinch_pp/erl_any.h"
#include "tinch_pp/erlang_types.h"
#include "erl_cpp_exception.h"
#include "term_conversions.h"
#include "ext_term_grammar.h"
#include "matchable_seq.h"
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/assign/list_of.hpp>
#include <cassert>

using namespace tinch_pp;
using namespace tinch_pp::erl;
using namespace boost;
using namespace boost::assign;

namespace {

bool match_int(msg_seq_iter& f, const msg_seq_iter& l, const any& instance)
{
   return int_(instance).match(f, l);
}

bool match_atom(msg_seq_iter& f, const msg_seq_iter& l, const any& instance)
{
   return atom(instance).match(f, l);
}

bool match_string(msg_seq_iter& f, const msg_seq_iter& l, const any& instance)
{
   return estring(instance).match(f, l);
}

bool match_float(msg_seq_iter& f, const msg_seq_iter& l, const any& instance)
{
   return float_(instance).match(f, l);
}

bool match_reference(msg_seq_iter& f, const msg_seq_iter& l, const any& instance)
{
  return erl::ref(instance).match(f, l);
}

bool match_pid(msg_seq_iter& f, const msg_seq_iter& l, const any& instance)
{
   return pid(instance).match(f, l);
}

bool match_tuple(msg_seq_iter& f, const msg_seq_iter& l, const any& instance)
{
   msg_seq_iter start = f;

   size_t parsed_length = 0;
   bool match = binary_to_term<small_tuple_head_ext>(f, l, parsed_length);
   assert(match); // already checked in the previous dispatch-mechanism

   instance.save_matched_bytes(msg_seq(start, f));

   for(size_t i = 0; match && (i < parsed_length); ++i)
      match &= instance.match(f, l);

   return match;
}

bool match_list(msg_seq_iter& f, const msg_seq_iter& l, const any& instance)
{
   msg_seq_iter start = f;

   size_t parsed_length = 0;
   bool match = binary_to_term<list_head_ext>(f, l, parsed_length);
   assert(match); // already checked in the previous dispatch-mechanism

   instance.save_matched_bytes(msg_seq(start, f));

   for(size_t i = 0; match && (i < parsed_length); ++i)
      match &= instance.match(f, l);

   return match;
}

optional<int> extract_type_tag(msg_seq_iter& f, const msg_seq_iter& l)
{
   optional<int> tag;

   if(f != l)
      tag = static_cast<int>(*f);

   return tag;
}

}

const any::dynamic_element_matcher_type any::dynamic_element_matcher = 
   map_list_of
      //    Type                    Match function
      (type_tag::small_integer,     bind(match_int, ::_1, ::_2, ::_3))
      (type_tag::integer,           bind(match_int, ::_1, ::_2, ::_3))
      (type_tag::atom_ext,          bind(match_atom, ::_1, ::_2, ::_3))
      (type_tag::small_tuple,       bind(match_tuple, ::_1, ::_2, ::_3))
      (type_tag::list,              bind(match_list, ::_1, ::_2, ::_3))
      (type_tag::string_ext,        bind(match_string, ::_1, ::_2, ::_3))
      (type_tag::pid,               bind(match_pid, ::_1, ::_2, ::_3))
      (type_tag::new_reference_ext, bind(match_reference, ::_1, ::_2, ::_3))
      (type_tag::float_ext,         bind(match_float, ::_1, ::_2, ::_3));

bool any::match_dynamically(msg_seq_iter& f, const msg_seq_iter& l, const any& instance)
{
   bool matched = false;

   if(const optional<term_id_type> tag = extract_type_tag(f, l)) {
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

bool any::match(msg_seq_iter& f, const msg_seq_iter& l) const
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
