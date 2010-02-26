#ifndef MATCHABLE_RANGE_H
#define MATCHABLE_RANGE_H

#include "tinch_pp/matchable.h"

namespace tinch_pp {

class matchable_range : public matchable
{
public:
  matchable_range(msg_seq_iter& first, const msg_seq_iter& last);

  virtual bool match(const erl::object& pattern) const;

private:
  msg_seq_iter& first;
  const msg_seq_iter& last;
};

}

#endif
