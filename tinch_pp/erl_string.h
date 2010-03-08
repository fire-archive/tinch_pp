#ifndef ERL_STRING_H
#define ERL_STRING_H

#include "erl_object.h"
#include <boost/function.hpp>
#include <string>

namespace tinch_pp {
namespace erl {

class any;

// As an object is used in a match, it's either a value-match or a type-match.
// In the latter case, we want to assign the matched value. That difference 
// in behaviour is encapsulated by a match_fn.
typedef boost::function<bool (msg_seq_iter&, const msg_seq_iter&)> match_fn_type;

/// String does NOT have a corresponding Erlang representation, but is an 
/// optimization for sending lists of bytes (integer in the range 0-255) 
/// more efficiently over the distribution.
class estring : public object
{
public:
  explicit estring(const std::string& val);

  explicit estring(std::string* to_assign);

  explicit estring(const any& match_any);

  virtual void serialize(msg_seq_out_iter& out) const;

  virtual bool match(msg_seq_iter& f, const msg_seq_iter& l) const;

  std::string value() const { return val; }

private:
  std::string val;
  std::string* to_assign;

  match_fn_type match_fn;
};

}
}

#endif
