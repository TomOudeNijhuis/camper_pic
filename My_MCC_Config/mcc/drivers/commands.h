/*
 * commands.h - Opcode dispatch for binary protocol.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "protocol.h"

void commands_dispatch(const protocol_frame_t *f);

#ifdef __cplusplus
}
#endif

#endif /* COMMANDS_H */
