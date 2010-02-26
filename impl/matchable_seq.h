#ifndef MATCHABLE_SEQ_H
#define MATCHABLE_SEQ_H

#include "tinch_pp/matchable.h"

namespace tinch_pp {

class matchable_seq : public matchable
{
public:
  matchable_seq(const msg_seq& erlang_msg);

 virtual bool match(const erl::object& pattern) const;

private:
  mutable msg_seq matchable_msg;
};

}

#endif
