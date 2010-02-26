#ifndef MATCHABLE_H
#define MATCHABLE_H

#include "impl/types.h"
#include <boost/shared_ptr.hpp>

namespace tinch_pp {

  namespace erl {
    class object;
  }

// A matchable is returned from a successfull call to mailbox::receive().
// It encapsulates a message received from another mailbox.
// A matchable supports pattern matching on its contained message.
class matchable
{
public:
  virtual ~matchable() {}

  // Matches the contained message against the given pattern.
  // Reference-variables in the pattern are bound to the values matching them.
  // Returns true upon a successfull match.
  virtual bool match(const erl::object& pattern) const = 0;
};

typedef boost::shared_ptr<matchable> matchable_ptr;

}

#endif
