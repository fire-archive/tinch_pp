#include "matchable_seq.h"
#include "tinch_pp/erl_object.h"

using namespace tinch_pp;

matchable_seq::matchable_seq(const msg_seq& erlang_msg)
  : matchable_msg(erlang_msg)
{
}


bool matchable_seq::match(const erl::object& pattern) const
{
  msg_seq_iter first = matchable_msg.begin();
  msg_seq_iter last = matchable_msg.end();

  return pattern.match(first, last);
}

