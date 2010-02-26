#include "tinch_pp/erl_string.h"
#include "tinch_pp/erl_any.h"
#include "string_matcher.h"
#include "ext_term_grammar.h"
#include "term_conversions.h"
#include <boost/bind.hpp>
#include <list>
#include <algorithm>
#include <cassert>

using namespace tinch_pp;
using namespace tinch_pp::erl;
using namespace boost;
using namespace std;

namespace {

typedef tinch_pp::detail::string_matcher matcher_s;

erl::int_ create_int(char val)
{
  const uint32_t int_val = val;
  return erl::int_(int_val);
}

bool match_string_value(msg_seq_iter& f, const msg_seq_iter& l, const string& val)
{
  // We're reusing the list implementation:
  std::list<int_> as_ints;
  std::transform(val.begin(), val.end(), back_inserter(as_ints), create_int);

  return matcher_s::match(as_ints, f, l);
}

char shrink_int(const erl::int_& val)
{
  return static_cast<char>(val.value());
}

bool assign_matched_string(msg_seq_iter& f, const msg_seq_iter& l, string* to_assign)
{
  assert(to_assign != 0);
  
  std::list<int_> as_ints;
  const bool res = matcher_s::assign_match(&as_ints, f, l);

  for_each(as_ints.begin(), as_ints.end(), bind(&string::push_back, to_assign,
                                                bind(shrink_int, ::_1)));

  return res;
}

bool match_any_string(msg_seq_iter& f, const msg_seq_iter& l, const any& match_any)
{
  std::string ignore;
  msg_seq_iter start = f;

  return assign_matched_string(f, l, &ignore) ? match_any.save_matched_bytes(msg_seq(start, f)) : false;
}

}

estring::estring(const std::string& a_val)
  : val(a_val),
    to_assign(0),
    match_fn(bind(match_string_value, ::_1, ::_2, cref(val))) {}

estring::estring(std::string* a_to_assign)
 : to_assign(a_to_assign),
   match_fn(bind(assign_matched_string, ::_1, ::_2, to_assign)) {}

estring::estring(const any& match_any)
  : to_assign(0),
    match_fn(bind(match_any_string, ::_1, ::_2, cref(match_any))) {}


void estring::serialize(msg_seq_out_iter& out) const
{
  const serializable_string s(val);
  term_to_binary<string_ext_g>(out, s);
}

bool estring::match(msg_seq_iter& f, const msg_seq_iter& l) const
{
  return match_fn(f, l);
}
