/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#define MAIN_C

#include "sselog.h"
#include <comdef.h>
#include <rt_misc.h>
#include "tlSecureTouchApi.h"
#include "glob.h"
#include "TouchApiDefs.h"

#define DECLARE_TL_FUNC(name) \
extern tciReturnCode_t name(tlSecureTouchCommandPayload_t *command, tlSecureTouchResponsePayload_t *response)

DECLARE_TL_FUNC(processCmdSetup);
DECLARE_TL_FUNC(processCmdProcess);
DECLARE_TL_FUNC(processCmdTeardown);

typedef struct {
  uint32_t cmdId;
  tciReturnCode_t(*func)(tlSecureTouchCommandPayload_t *command, tlSecureTouchResponsePayload_t *response);
} tl_command_t;

static const tl_command_t commands[] = {
  { TLSECURETOUCH_CMD_SETUP, processCmdSetup },
  { TLSECURETOUCH_CMD_PROCESS, processCmdProcess },
  { TLSECURETOUCH_CMD_TEARDOWN, processCmdTeardown },
};

const tl_config_t g_config = {
  TL_MAGIC_0,
  TL_CONFIG_VERSION_MAJOR,
  TL_CONFIG_VERSION_MINOR,
  {
    760,
    1424
  },
  {0},
  TL_MAGIC_1
};

/**
 * Sample app name
 * Modify the app name to your specific app name
 */
char TZ_APP_NAME[] = {"secureTouch"};

void tz_app_init() {
  ENTER;
  g_sid = DR_SID_INVALID;
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
