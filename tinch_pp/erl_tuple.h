#ifndef ERL_TUPLE_H
#define ERL_TUPLE_H

#include "erl_object.h"
#include "impl/ext_term_grammar.h"
#include <boost/fusion/tuple/tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/algorithm/query/all.hpp>
#include <boost/fusion/include/all.hpp>
#include <boost/bind.hpp>

namespace tinch_pp {
namespace erl {

/// A wrapper around boost::fusion::tuple (basically a TR1 representation). 
/// For you as a client, simply work with the full boost::fusion::tuple API and just 
/// wrap it in an instance of this class (using make_tuple below) in the call
/// to mailbox::send.
template<typename Tuple>
class tuple : public object
{
public:
  tuple(const Tuple& t)
    : contained(t),
      tuple_length(boost::fusion::tuple_size<Tuple>::value)
  {
  }

  virtual void serialize(msg_seq_out_iter& out) const
  { 
    using namespace boost;
    
    // TODO: do we have to care for larger tuples? Problem with fusion (C++ template limitations)?
    small_tuple_head_g g;
    karma::generate(out, g, tuple_length);

    // Serialize each contained element.
    fusion::for_each(contained, bind(&object::serialize, ::_1, boost::ref(out)));
  }

  virtual bool match(msg_seq_iter& f, const msg_seq_iter& l) const
  {
    using namespace boost;

    size_t parsed_length = 0;
    small_tuple_head_ext tuple_head_p;

    const bool success = qi::parse(f, l, tuple_head_p, parsed_length);
    const bool tuple_matched = success && (tuple_length == parsed_length);

    return tuple_matched && fusion::all(contained, bind(&object::match, ::_1, boost::ref(f), cref(l)));
  }

private:
  Tuple contained;
  size_t tuple_length;
};


}
}

#endif
