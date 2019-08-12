/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#define MAIN_C

#include "sselog.h"
#include "sse_error.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlTestHdmiApi.h"
#include "tlGlob.h"

DECLARE_TRUSTLET_MAIN_STACK(0x1000);

#define DECLARE_TL_FUNC(name) \
  extern tciReturnCode_t name(tlTestHdmiCommandPayload_t* command, tlTestHdmiResponsePayload_t* response)

DECLARE_TL_FUNC(processCmdStatus);

typedef struct {
  uint32_t cmdId;
  tciReturnCode_t (*func)(tlTestHdmiCommandPayload_t* command, tlTestHdmiResponsePayload_t* response);
} tl_command_t;

static const tl_command_t commands[]={
  {TLTESTHDMI_CMD_STATUS,processCmdStatus},
};

/**
 * Trustlet entry.
 */
_TLAPI_ENTRY void tlMain(
    const addr_t tciBuffer, 
    const uint32_t tciBufferLen
) {
  // Initialisation sequence
  tciReturnCode_t ret = E_SSE_UNKNOWN_FUNCTION;
  tciCommandId_t commandId;
  unsigned int i = 0;

  // Check if the size of the given TCI is sufficient
  if ((NULL == tciBuffer) || (sizeof(tci_t) > tciBufferLen)) 
  {
    // TCI too small -> end Trustlet
    tlApiExit(EXIT_ERROR);
  }

  tci_t* tci = (tci_t*)tciBuffer;

  // The Trustlet main loop running infinitely
  for (;;) {

    // Wait for a notification to arrive (INFINITE timeout is recommended -> not polling!)
    tlApiWaitNotification(TLAPI_INFINITE_TIMEOUT);

    // Dereference commandId once for further usage
    commandId = tci->message.header.command.commandId;
    TL_LOG_INIT((char*)tci->log, LOG_SIZE);

    // Check if the message received is (still) a response
    if (!IS_CMD(commandId)) {
      // Tell the NWd a response is still pending (optional)
      tlApiNotify();
      continue;
    }

    // Process command message
    ret = E_SSE_UNKNOWN_FUNCTION;
    for (i=0; i<LENGTH(commands); i++) {
      if ((commands[i].cmdId == commandId) && (commands[i].func != NULL)) {
        g_command = tci->message.payload.command; // copy it in BSS, protected
        ret = (*(commands[i].func))(&g_command,&tci->message.payload.response);
        break;
      }
    }

    // Set up response header -> mask response ID and set return code
    tci->message.header.response.responseId = RSP_ID(commandId);
    tci->message.header.response.returnCode = ret;
    
    // Notify back the TLC
    tlApiNotify();
  }

}
