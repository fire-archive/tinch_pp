#ifndef ERLANG_TYPES_H
#define ERLANG_TYPES_H

/// This is just a convenience file to include all supported Erlang types.
/// If you want to control the compile-time dependencies, please consider 
/// to include only the specific file your program actually needs.
#include "erlang_value_types.h"
#include "erl_tuple.h"
#include "erl_list.h"
#include "erl_any.h"
#include "erl_string.h"
#include "type_makers.h"

#endif
