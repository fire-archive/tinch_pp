#include "tinch_pp/type_makers.h"
#include "tinch_pp/erlang_value_types.h"

namespace tinch_pp {
namespace erl {

object_ptr make_atom(const std::string& s)
{
   return object_ptr(new atom(s));
}

object_ptr make_pid(const tinch_pp::pid_t& p)
{
   return object_ptr(new pid(p));
}

object_ptr make_int(boost::int32_t i)
{
  return object_ptr(new int_(i));
}

}
}
