#ifndef ERLANG_VALUE_TYPES_H
#define ERLANG_VALUE_TYPES_H

#include "erl_object.h"
#include <boost/function.hpp>

namespace tinch_pp {
namespace erl {

class any;

// As an object is used in a match, it's either a value-match or a type-match.
// In the latter case, we want to assign the matched value. That difference 
// in behaviour is encapsulated by a match_fn.
typedef boost::function<bool (msg_seq_iter&, const msg_seq_iter&)> match_fn_type;

class int_ : public object
{
public:
  typedef boost::int32_t value_type;

  explicit int_(boost::int32_t val);

  explicit int_(boost::int32_t* to_assign);

  explicit int_(const any& match_any_int);

  virtual void serialize(msg_seq_out_iter& out) const;

  virtual bool match(msg_seq_iter& f, const msg_seq_iter& l) const;

  // Special case: Erlang packs a list of small values (<= 255) into a string-sequence.
  // Those values can only be matched by int_.
  bool match_string_element(msg_seq_iter& f, const msg_seq_iter& l) const;

  boost::int32_t value() const { return val; }

private:
  boost::int32_t val;
  boost::int32_t* to_assign;

  match_fn_type match_fn;
};

class pid : public object
{
public:
  typedef pid_t value_type;

  explicit pid(const pid_t& val);

  // Used for assigning a match during pattern matching.
  explicit pid(pid_t* pid_to_assign);

  explicit pid(const any& match_any);

  virtual void serialize(msg_seq_out_iter& out) const;

  virtual bool match(msg_seq_iter& f, const msg_seq_iter& l) const;

  pid_t value() const { return val; }

private:
  pid_t val;
  pid_t* pid_to_assign;

  match_fn_type match_fn;
};

class float_ : public object
{
public:
  typedef double value_type;

  explicit float_(double val);

  // Used for assigning a match during pattern matching.
  explicit float_(double* float_to_assign);

  explicit float_(const any& match_any);

  virtual void serialize(msg_seq_out_iter& out) const;

  virtual bool match(msg_seq_iter& f, const msg_seq_iter& l) const;

  double value() const { return val; }

private:
  double val;
  double* float_to_assign;

  match_fn_type match_fn;
};

class atom : public object
{
public:
  typedef std::string value_type;

  explicit atom(const std::string& val);

  explicit atom(std::string* to_assign);

  explicit atom(const any& match_any);

  virtual void serialize(msg_seq_out_iter& out) const;

  virtual bool match(msg_seq_iter& f, const msg_seq_iter& l) const;

  std::string value() const { return val; }

private:
  std::string val;
  std::string* to_assign;

  match_fn_type match_fn;
};

// Encode a reference object (an object generated with make_ref/0).
class ref : public object
{
public:
  typedef new_reference_type value_type;

  explicit ref(const new_reference_type& val);

  // Used for assigning a match during pattern matching.
  explicit ref(new_reference_type* pid_to_assign);

  explicit ref(const any& match_any);

  virtual void serialize(msg_seq_out_iter& out) const;

  virtual bool match(msg_seq_iter& f, const msg_seq_iter& l) const;

  new_reference_type value() const { return val; }

private:
  new_reference_type val;
  new_reference_type* to_assign;

  match_fn_type match_fn;
};

}
}

#endif
