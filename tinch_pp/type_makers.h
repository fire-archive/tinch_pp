#ifndef TYPE_MAKERS_H
#define TYPE_MAKERS_H

#include "make_erl_tuple.h"
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace tinch_pp {

struct pid_t;
namespace erl {
  class object;
  typedef boost::shared_ptr<object> object_ptr;
}

namespace erl {

object_ptr make_atom(const std::string& s);

object_ptr make_pid(const tinch_pp::pid_t& p);

object_ptr make_int(boost::int32_t i);

}
}

#endif
