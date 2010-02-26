#include "matchable_range.h"
#include "tinch_pp/erl_object.h"

using namespace tinch_pp;

matchable_range::matchable_range(msg_seq_iter& a_first, const msg_seq_iter& a_last)
  : first(a_first),
    last(a_last)
{
}


bool matchable_range::match(const erl::object& pattern) const
{
  return pattern.match(first, last);
}
