#ifndef LIST_MATCHER_H
#define LIST_MATCHER_H

#include "impl/ext_term_grammar.h"
#include <boost/bind.hpp>
#include <algorithm>

namespace tinch_pp {
namespace detail {

// TODO: We probably have to handle the reception of a list with hetereogenous types in 
// a specialization.
template<typename T>
struct list_matcher
{
  static bool match(const T& val, msg_seq_iter& f, const msg_seq_iter& l)
  {
    using namespace boost;

    size_t parsed_length = 0;
    list_head_ext list_head_p;

    const bool success = qi::parse(f, l, list_head_p, parsed_length);
    const bool length_matched = success && (val.size() == parsed_length);

    // TODO: this code only handles proper Erlang lists (the ones ending with a cdr of nil).
    // Ensure that we can handle improper lists( [a|b] ) too (parse one more element, different length check).
    const bool matched = length_matched && (val.end() == 
					                                        std::find_if(val.begin(), val.end(), 
                                                bind(&object::match, ::_1, boost::ref(f), cref(l)) == false));

    return matched && qi::parse(f, l, qi::byte_(tinch_pp::type_tag::nil_ext));
  }

  static bool assign_match(T* val, msg_seq_iter& f, const msg_seq_iter& l)
  {
    using namespace boost;

    size_t parsed_length = 0;
    list_head_ext list_head_p;

    bool success = qi::parse(f, l, list_head_p, parsed_length);

    for(size_t i = 0; success && (i < parsed_length); ++i) {
      typename T::value_type::value_type to_assign;
      typename T::value_type assigner(&to_assign);

      success = assigner.match(f, l);
      
      typename T::value_type matched_value(to_assign);
      val->push_back(matched_value);
    }

    // TODO: this code only handles proper Erlang lists (the ones ending with a cdr of nil).
    // Ensure that we can handle improper lists( [a|b] ) too (parse one more element, different length check).
    return success && qi::parse(f, l, qi::byte_(tinch_pp::type_tag::nil_ext));
  }
};

}

}

#endif
