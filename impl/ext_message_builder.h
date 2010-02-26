#ifndef EXT_MESSAGE_BUILDER_H
#define EXT_MESSAGE_BUILDER_H

#include "types.h"

namespace tinch_pp {

// Sending a message to a node involves several, low-level steps.
// These functions encapsulate them.
 
 msg_seq build_send_msg(const msg_seq& msg, const pid_t& destination_pid);

 msg_seq build_reg_send_msg(const msg_seq& msg, const pid_t& self, const std::string& destination_name);

}

#endif
