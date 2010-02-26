#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <boost/cstdint.hpp>

namespace tinch_pp {
namespace constants {

  // Constants used to establish communication between nodes.
  extern const int node_type;
  extern const boost::uint16_t supported_version;
  extern const boost::uint32_t capabilities;

  // Constants used for message exchange with nodes.
  extern const int magic_version;
  extern const int pass_through;
  extern const int ctrl_msg_send;
  extern const int ctrl_msg_reg_send;

  // Constants for the message format.
  extern const int float_digits;
}
}

#endif
