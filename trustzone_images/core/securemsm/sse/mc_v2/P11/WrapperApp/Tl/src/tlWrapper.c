/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary.
 */

#define MAIN_TL_C
#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlWrapperApi.h"
#include "tlLog.h"

DECLARE_TRUSTLET_MAIN_STACK(0x1000);

#define DECLARE_TL_FUNC(name) \
  extern tciReturnCode_t name( tciMessage_t* tci_message)

DECLARE_TL_FUNC(processCmdWrap);
DECLARE_TL_FUNC(processCmdUnwrap);

typedef struct {
  uint32_t cmdId;
  tciReturnCode_t (*func)(tciMessage_t* tci_message);
} tl_command_t;

static const tl_command_t commands[]={
  {TLWRAPPER_CMD_WRAP,processCmdWrap},
  {TLWRAPPER_CMD_UNWRAP,processCmdUnwrap},
};

tciMessage_t tl_work_data;
/**
 * Trustlet entry.
 */
_TLAPI_ENTRY void tlMain(
    const addr_t tciBuffer,
    const uint32_t tciBufferLen
) {
    // Initialisation sequence
    tciReturnCode_t ret = E_UNKNOWN_COMMAND;
    tciCommandId_t commandId;
    unsigned int i = 0;

    // Check if the size of the given TCI is sufficient
    if ((NULL == tciBuffer) || (sizeof(tci_t) > tciBufferLen))
    {
        // TCI too small -> end Trustlet
        tlApiExit(EXIT_ERROR);
    }

    tci_t* tci = (tci_t*)tciBuffer;
    tciMessage_t* tci_message = &tl_work_data;

    // The Trustlet main loop running infinitely
    for (;;) {

        // Wait for a notification to arrive (INFINITE timeout is recommended -> not polling!)
        tlApiWaitNotification(TLAPI_INFINITE_TIMEOUT);

        memscpy(tci_message, sizeof(tciMessage_t), &(tci->message), sizeof(tciMessage_t));

        // Dereference commandId once for further usage
        commandId = tci->message.commandHeader.commandId;
        TL_LOG_INIT((char*)tci->log);

        // Check if the message received is (still) a response
        if (!IS_CMD(commandId)) {
            // Tell the NWd a response is still pending (optional)
            tlApiNotify();
            continue;
        }

        // Process command message
        ret = E_UNKNOWN_COMMAND;
        for (i=0; i<LENGTH(commands); i++) {
            if ((commands[i].cmdId == commandId) && (commands[i].func != NULL)) {
                ret = (*(commands[i].func))(tci_message);
                break;
            }
        }

        memscpy(tci, sizeof(tci_t), tci_message, sizeof(tciMessage_t));
        // Set up response header -> mask response ID and set return code
        tci->message.responseHeader.responseId = RSP_ID(commandId);
        tci->message.responseHeader.returnCode = ret;

        // Notify back the TLC
        tlApiNotify();
    }

}
#ifndef DEVLIB
void dumpHex(const char *tag, const uint8_t *buffer, unsigned int len) {
  char output[256];
  memset(output,0,sizeof(output));
  TL_LOG_I("dumpHex %s (%d bytes @ 0x%08X)",tag,len, (unsigned int)buffer);
  int j=0;
  size_t writtenTotal = 0;
  int written = 0;
  for (int i=0; i<len; i++) {
    written = snprintf(output+j*3,sizeof(output)-writtenTotal,"%02X ",buffer[i]);
    if (written >= sizeof(output)-writtenTotal) {
      // this is never supposed to happen with a 256 chars line... since we
      // write 3 chars (+0) at a time, 16 times per line
      // print what we have and reset the line
      TL_LOG_I("%s",output);
      j=0;
      writtenTotal = 0;
      continue;
    } else if (written < 0) {
      // this is again not supposed to happen... forget about this byte and go on
      // nothing has been written, so there is nothing to "roll back"
    } else {
      writtenTotal += written;
    }
    *(output+j*3+3)=0; // make sure there is an ending zero
    if (j == 15) { // 16 hex values written
      TL_LOG_I("%s",output);
      j=0;
      writtenTotal = 0;
    } else {
      j++;
    }
  }
  if (j>0 && j<15) {
    TL_LOG_I("%s",output);
  }
}
#endif
