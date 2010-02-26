#ifndef STRING_MATCHER_H
#define STRING_MATCHER_H

#include "ext_term_grammar.h"
#include "tinch_pp/erlang_value_types.h"
#include <boost/bind.hpp>
#include <algorithm>
#include <list>

namespace tinch_pp {
namespace detail {
  
struct string_matcher
{
  typedef std::list<erl::int_> list_type;

  static bool match(const list_type& val, msg_seq_iter& f, const msg_seq_iter& l)
  {
    using namespace boost;

    size_t parsed_length = 0;
    string_head_ext string_head_p;

    const bool success = qi::parse(f, l, string_head_p, parsed_length);
    const bool length_matched = success && (val.size() == parsed_length);

    // When packed as a string, there's no encoding-tag prepended to the individual elements
    return length_matched && (val.end() == std::find_if(val.begin(), val.end(), 
							                                                  bind(&erl::int_::match_string_element, 
                                                               ::_1, boost::ref(f), cref(l)) == false));
  }

  static bool assign_match(list_type* val, msg_seq_iter& f, const msg_seq_iter& l)
  {
    using namespace boost;

    size_t parsed_length = 0;
    string_head_ext string_head_p;

    bool success = qi::parse(f, l, string_head_p, parsed_length);

    for(size_t i = 0; success && (i < parsed_length); ++i) {
      erl::int_::value_type matched_value;

      success =  qi::parse(f, l, qi::byte_, matched_value);
      val->push_back(erl::int_(matched_value));
    }

    return success;
  }
};

}
}

#endif
