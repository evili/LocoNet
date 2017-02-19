#ifndef _DEBUG_H
#define _DEBUG_H

#include "Arduino.h"

#if defined(HAVE_HWSERIAL1)
HardwareSerial console = Serial1;
#else
HardwareSerial console = Serial;
#endif /* HAVE_HWSERIAL1 */
#define _DEBUG_(x) console.println(x)
#define _KLDEBUG_(x)  console.print(x)
#else
#define _DEBUG_(x)
#define _KLDEBUG_(x)

#define CMD_BUFFER_LEN 32

#define START_CMD '<'
#define END_CMD '>'

enum Console_State
{
  CMD_BEFORE_START,
  CMD_STARTED,
  CMD_END,
};

typedef struct opc_s {
  char *name;
  uint8_t code;
  uint8_t n_args;
} opc_map_t;

const opc_map_t OPC_Map[] = {
  {"BUSY", OPC_BUSY, 0},
  {"GPCOFF", OPC_GPOFF, 0},
  {"GPCON", OPC_GPON, 0},
  {"IDLE", OPC_IDLE, 0},
  {"LOCO_SPD", OPC_LOCO_SPD, 2},
  {"LOCO_DIRF", OPC_LOCO_DIRF, 2},
  {"LOCO_SND", OPC_LOCO_SND, 2},
  {"SW_REQ", OPC_SW_REQ, 2},
  {"SW_REP", OPC_SW_REP, 2},
  {"INPUT_REP", OPC_INPUT_REP, 2},
  {"UNKNOWN", OPC_UNKNOWN, 0},
  {"LONG_ACK", OPC_LONG_ACK, 2},
  {"SLOT_STAT1", OPC_SLOT_STAT1, 2},
  {"CONSIST_FUNC", OPC_CONSIST_FUNC, 2},
  {"UNLINK_SLOTS", OPC_UNLINK_SLOTS, 2},
  {"LINK_SLOTS", OPC_LINK_SLOTS, 2},
  {"MOVE_SLOTS", OPC_MOVE_SLOTS, 2},
  {"RQ_SL_DATA", OPC_RQ_SL_DATA, 2},
  {"SW_STATE", OPC_SW_STATE, 2},
  {"SW_ACK", OPC_SW_ACK, 2},
  {"LOCO_ADR", OPC_LOCO_ADR, 2},
  {"MULTI_SENSE", OPC_MULTI_SENSE, 2},
  {"PEER_XFER", OPC_PEER_XFER, -1},
  {"SL_RD_DATA", OPC_SL_RD_DATA, -1},
  {"IMM_PACKET", OPC_IMM_PACKET, -1},
  {"IMM_PACKET_2", OPC_IMM_PACKET_2, -1},
  {"WR_SL_DATA", OPC_WR_SL_DATA, -1}
};

void getConsoleInput();

#endif /* _CONSOLE_H */
