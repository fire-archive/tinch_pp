#include "constants.h"

namespace {

  // Distribution capabilities.
  const boost::uint32_t extended_references = 4;
  const boost::uint32_t extended_pid_ports = 0x100;
  const boost::uint32_t dist_header_atom_cache = 0x2000;

}

namespace tinch_pp {
namespace constants {

  const int node_type = 72; // hidden node (i.e. not native Erlang)
  const boost::uint16_t supported_version = 5; // R6B and later
  const boost::uint32_t capabilities = extended_references | extended_pid_ports;// | dist_header_atom_cache;

  const int magic_version = 131;
  const int pass_through = 112;
  const int ctrl_msg_send = 2;
  const int ctrl_msg_reg_send = 6;

  const int float_digits = 31;
}
}
