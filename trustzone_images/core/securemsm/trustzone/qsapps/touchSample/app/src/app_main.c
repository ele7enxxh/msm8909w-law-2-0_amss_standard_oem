/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#define MAIN_C

#include "sselog.h"
#include <comdef.h>
#include <rt_misc.h>
#include "touchSampleApi.h"
#include "glob.h"

#define DECLARE_FUNC(name) \
  extern tciReturnCode_t name(touchSampleCommandPayload_t* command, touchSampleResponsePayload_t* response)

DECLARE_FUNC(processCmdStart);
DECLARE_FUNC(processCmdStop);
DECLARE_FUNC(processCmdGetData);

typedef struct {
  uint32_t cmdId;
  tciReturnCode_t (*func)(touchSampleCommandPayload_t* command, touchSampleResponsePayload_t* response);
} command_t;

static const command_t commands[]={
  {TOUCHSAMPLE_CMD_START,processCmdStart},
  {TOUCHSAMPLE_CMD_STOP,processCmdStop},
  {TOUCHSAMPLE_CMD_GETDATA,processCmdGetData},
};


/** 
 * Sample app name 
 * Modify the app name to your specific app name  
 */
char TZ_APP_NAME[] = {"touchSample"};

void tz_app_init() {
  ENTER;
  EXIT;
}

void tz_app_cmd_handler(void* req, uint32 cmdlen, void* rsp, uint32 rsplen) {

  tci_t *tci = (tci_t*)req;
  uint32 ret = 0;
  uint32_t commandId = 0;
  uint32_t i = 0;

  ENTER;

  
  if (req == NULL || rsp == NULL) {
    return;
  }
  
  // validating pointers. We expect the req and rsp to be the same pointer,
  // being the only 2 members of a union
  if (req != rsp) {
    return;
  }
  
  if (cmdlen + rsplen < sizeof(tci_t)) {
    return;
  }
  
  do {
    // Dereference commandId once for further usage
    commandId = tci->message.header.command.commandId;
    TL_LOG_INIT((char*)tci->log, LOG_SIZE);

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
  } while (0);

  EXIT;
}

void tz_app_shutdown(void) {
  ENTER;
  EXIT;
}
