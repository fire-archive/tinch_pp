#ifndef ERL_OBJECT_H
#define ERL_OBJECT_H

#include "impl/types.h"
#include <boost/shared_ptr.hpp>

namespace tinch_pp {
namespace erl {

class object
{
public:
  virtual ~object() {};

  // Converts the object to its external, binary format.
  // This function is typically used by the implementation as 
  // part of a message send, i.e. not intended for clients.
  virtual void serialize(msg_seq_out_iter& out) const = 0;

  // Attempts to match the given sequence [f..l)
  // This function is typically used by the receive-mechanism and 
  // not intended for clients.
  virtual bool match(msg_seq_iter& f, const msg_seq_iter& l) const = 0;
};

typedef boost::shared_ptr<object> object_ptr;

}
}

#endif
