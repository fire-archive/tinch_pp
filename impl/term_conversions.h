#ifndef TERM_CONVERSIONS_H
#define TERM_CONVERSIONS_H

#include "types.h"
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/karma_generate.hpp>

namespace tinch_pp {

template<typename G, typename T>
bool term_to_binary(msg_seq_out_iter& out, const T& t)
{
  G g;
  return boost::spirit::karma::generate(out, g, t);
}

template<typename Parser, typename T>
bool binary_to_term(msg_seq_iter& f, const msg_seq_iter& l, T& attr)
{
  Parser p;

  return boost::spirit::qi::parse(f, l, p, attr);
}

}

#endif
