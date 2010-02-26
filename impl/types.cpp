#include "types.h"

namespace tinch_pp {

bool operator ==(const serializable_string& s1, const serializable_string& s2)
{
   return s1.val == s2.val;
}

bool operator ==(const pid_t& p1, const pid_t& p2)
{
  return (p1.node_name == p2.node_name) && 
         (p1.id == p2.id) && 
         (p1.serial == p2.serial) && 
         (p1.creation == p2.creation);
}

bool operator ==(const new_reference_type& r1, const new_reference_type& r2)
{
  return (r1.node_name == r2.node_name) &&
         (r1.creation == r2.creation) &&
         (r1.id == r2.id);
}

}
