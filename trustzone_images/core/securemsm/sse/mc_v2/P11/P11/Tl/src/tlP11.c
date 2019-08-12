/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define MAIN_TL_C

#ifndef TL_LOG_FILE
#define TL_LOG_FILE
#endif

#include "stringl.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlLog.h"
#include "tlP11Utils.h"
#include "tlP11Glob.h"

/*lint -esym(714,_stack_tlMain_entries) -esym(768,_stack_tlMain_entries) -esym(765,_stack_tlMain_entries)*/
/*lint -esym(714,_stack_tlMain_array) -esym(768,_stack_tlMain_array) -esym(765,_stack_tlMain_array)*/
// it needs to accomodate a session group
DECLARE_TRUSTLET_MAIN_STACK(0x1000); /*lint !e509 !e19*/

#define DECLARE_TL_FUNC(name) \
  extern tciReturnCode_t name(tciMessage_t* tci_message)

DECLARE_TL_FUNC(processCmdInitialize);
DECLARE_TL_FUNC(processCmdGetTokenInfo);
DECLARE_TL_FUNC(processCmdGetMechanismList);
DECLARE_TL_FUNC(processCmdGetMechanismInfo);
DECLARE_TL_FUNC(processCmdInitToken);
DECLARE_TL_FUNC(processCmdInitTokenSO);
DECLARE_TL_FUNC(processCmdInitPIN);
DECLARE_TL_FUNC(processCmdSetPIN);
DECLARE_TL_FUNC(processCmdOpenSession);
DECLARE_TL_FUNC(processCmdCloseSession);
DECLARE_TL_FUNC(processCmdLogin);
DECLARE_TL_FUNC(processCmdLogout);
DECLARE_TL_FUNC(processCmdCreateObject);
DECLARE_TL_FUNC(processCmdCopyObject);
DECLARE_TL_FUNC(processCmdGetArrayAttributeValue);
DECLARE_TL_FUNC(processCmdGetULongAttributeValue);
DECLARE_TL_FUNC(processCmdGetTokenChallenge);
DECLARE_TL_FUNC(processCmdSetAttributeValue);
DECLARE_TL_FUNC(processCmdSetArrayAttributeValue);
DECLARE_TL_FUNC(processCmdSetTemplateAttributeValue);
DECLARE_TL_FUNC(processCmdCryptoInit);
DECLARE_TL_FUNC(processCmdCryptoAbort);
DECLARE_TL_FUNC(processCmdCryptoUpdate);
DECLARE_TL_FUNC(processCmdDigestKey);
DECLARE_TL_FUNC(processCmdGenerateKey);
DECLARE_TL_FUNC(processCmdGenerateKeyPair);
DECLARE_TL_FUNC(processCmdWrapKey);
DECLARE_TL_FUNC(processCmdUnwrapKey);
DECLARE_TL_FUNC(processCmdDeriveKey);
DECLARE_TL_FUNC(processCmdGenerateRandom);
#ifdef SK_ENABLED
DECLARE_TL_FUNC(processCmdInitTokenSKInit);
DECLARE_TL_FUNC(processCmdInitTokenSKContinue);
DECLARE_TL_FUNC(processCmdSKCancel);
DECLARE_TL_FUNC(processCmdInitPINSKInit);
DECLARE_TL_FUNC(processCmdInitPINSKContinue);
DECLARE_TL_FUNC(processCmdSetPINSKInit);
DECLARE_TL_FUNC(processCmdSetPINSKContinue);
DECLARE_TL_FUNC(processCmdLoginSKInit);
DECLARE_TL_FUNC(processCmdLoginSKContinue);
DECLARE_TL_FUNC(processCmdGenerateKeySKInit);
DECLARE_TL_FUNC(processCmdGenerateKeySKContinue);
#endif

typedef struct {
  uint32_t cmdId;
  tciReturnCode_t (*func)(tciMessage_t* tci_message);
} tl_command_t;

static const tl_command_t commands[]={
  {TLP11_CMD_INITIALIZE,processCmdInitialize},
  {TLP11_CMD_GET_TOKEN_INFO,processCmdGetTokenInfo},
  {TLP11_CMD_GET_MECHANISM_LIST,processCmdGetMechanismList},
  {TLP11_CMD_GET_MECHANISM_INFO,processCmdGetMechanismInfo},
  {TLP11_CMD_INIT_TOKEN,processCmdInitToken},
  {TLP11_CMD_INIT_TOKEN_SO,processCmdInitTokenSO},
  {TLP11_CMD_INIT_PIN,processCmdInitPIN},
  {TLP11_CMD_SET_PIN,processCmdSetPIN},
  {TLP11_CMD_OPEN_SESSION,processCmdOpenSession},
  {TLP11_CMD_CLOSE_SESSION,processCmdCloseSession},
  {TLP11_CMD_LOGIN,processCmdLogin},
  {TLP11_CMD_LOGOUT,processCmdLogout},
  {TLP11_CMD_CREATE_OBJECT,processCmdCreateObject},
  {TLP11_CMD_COPY_OBJECT,processCmdCopyObject},
  {TLP11_CMD_GET_ULONG_ATTRIBUTE_VALUE,processCmdGetULongAttributeValue},
  {TLP11_CMD_GET_ARRAY_ATTRIBUTE_VALUE,processCmdGetArrayAttributeValue},
  {TLP11_CMD_GET_TOKEN_CHALLENGE,processCmdGetTokenChallenge},
  {TLP11_CMD_SET_ATTRIBUTE_VALUE,processCmdSetAttributeValue},
  {TLP11_CMD_SET_ARRAY_ATTRIBUTE_VALUE,processCmdSetArrayAttributeValue},
  {TLP11_CMD_SET_TEMPLATE_ATTRIBUTE_VALUE,processCmdSetTemplateAttributeValue},
  {TLP11_CMD_CRYPTO_INIT,processCmdCryptoInit},
  {TLP11_CMD_CRYPTO_ABORT,processCmdCryptoAbort},
  {TLP11_CMD_CRYPTO_UPDATE,processCmdCryptoUpdate},
  {TLP11_CMD_DIGEST_KEY,processCmdDigestKey},
  {TLP11_CMD_GENERATE_KEY,processCmdGenerateKey},
  {TLP11_CMD_GENERATE_KEY_PAIR,processCmdGenerateKeyPair},
  {TLP11_CMD_WRAP_KEY,processCmdWrapKey},
  {TLP11_CMD_UNWRAP_KEY,processCmdUnwrapKey},
  {TLP11_CMD_DERIVE_KEY,processCmdDeriveKey},
  {TLP11_CMD_GENERATE_RANDOM,processCmdGenerateRandom},
};

#ifdef SK_ENABLED
/* These commands initiate a secure keypad entry */
static const tl_command_t commandsSKInit[]={
  {TLP11_CMD_LOGIN_SK_INIT,processCmdLoginSKInit},
  {TLP11_CMD_INIT_TOKEN_SK_INIT,processCmdInitTokenSKInit},
  {TLP11_CMD_INIT_PIN_SK_INIT,processCmdInitPINSKInit},
  {TLP11_CMD_SET_PIN_SK_INIT,processCmdSetPINSKInit},
  {TLP11_CMD_GENERATE_KEY_SK_INIT,processCmdGenerateKeySKInit},
};

static const tl_command_t commandsSKContinue[]={
  {TLP11_CMD_LOGIN_SK_CONTINUE,processCmdLoginSKContinue},
  {TLP11_CMD_INIT_TOKEN_SK_CONTINUE,processCmdInitTokenSKContinue},
  {TLP11_CMD_INIT_PIN_SK_CONTINUE,processCmdInitPINSKContinue},
  {TLP11_CMD_SET_PIN_SK_CONTINUE,processCmdSetPINSKContinue},
  {TLP11_CMD_GENERATE_KEY_SK_CONTINUE,processCmdGenerateKeySKContinue},
};

static const tl_command_t commandSKCancel=
  {TLP11_CMD_SK_CANCEL,processCmdSKCancel};

static uint32_t bProcessingSKEntry = 0;
static uint32_t commandSKContinue = 0xFFFFFFFF;
#endif

static tciMessage_t g_workArea;

/**
 * Trustlet entry.
 */
_TLAPI_ENTRY void tlMain(
    const addr_t tciBuffer,
    const uint32_t tciBufferLen
) {
  // Initialisation sequence
  tciReturnCode_t ret = CKR_UNKNOWN_COMMAND;
  tciCommandId_t commandId;
  unsigned int i = 0;
  loginFailed = 0;
  timestamp_t ts = 0;

  // Check if the size of the given TCI is sufficient
  if ((NULL == tciBuffer) || (sizeof(tci_t) > tciBufferLen))
  {
    // TCI too small -> end Trustlet
    tlApiExit(EXIT_ERROR);
  }

  tci_t* tci = (tci_t*)tciBuffer;
  tciMessage_t* tci_message = &g_workArea;

  // we check the availability of timers
  if (TLAPI_OK == tlApiGetTimeStamp(&ts,TS_SOURCE_SECURE_RTC)) {
    g_tsSource = TS_SOURCE_SECURE_RTC;
  } else if (TLAPI_OK == tlApiGetTimeStamp(&ts,TS_SOURCE_SOFTCNT)) {
    g_tsSource = TS_SOURCE_SOFTCNT;
  } else {
    g_tsSource = TS_SOURCE_ILLEGAL;
    // in this case, we allow tokens to be unlocked at boot IF an unlock time
    // was set
    if ((g_config.arg.localUnlockTime != UINT32_MAX) ||
        (g_config.arg.remoteUnlockTime != UINT32_MAX)) {
      g_config.arg.unlockAtBoot = 1;
    }
  }

  // The Trustlet main loop running infinitely
  for (;;) {

    // Wait for a notification to arrive (INFINITE timeout is recommended -> not polling!)
    tlApiWaitNotification(TLAPI_INFINITE_TIMEOUT);

    memscpy(tci_message,sizeof(tciMessage_t),&(tci->message),sizeof(tciMessage_t));
    // Dereference commandId once for further usage
    commandId = tci->message.commandHeader.commandId; /*lint !e613*/
    TL_LOG_INIT((char*)tci->log); /*lint !e613*/

    // Check if the message received is (still) a response
    if (!IS_CMD(commandId)) {
      // Tell the NWd a response is still pending (optional)
      tlApiNotify();
      continue;
    }

    if (loginFailed) {
      ret = CKR_FUNCTION_FAILED;
    } else {
#ifdef SK_ENABLED
      if (bProcessingSKEntry == 1) {
        // we are performing a SK entry
        if (commandId == commandSKContinue) {
          // find and process the "continue" command
          ret = CKR_UNKNOWN_COMMAND;
          for (i=0; i<LENGTH(commandsSKContinue); i++) {
            if ((commandsSKContinue[i].cmdId == commandId) && (commandsSKContinue[i].func != NULL)) {
              ret = (*(commandsSKContinue[i].func))(tci_message);
              break;
            }
          }
          if (ret != CKR_SK_CONTINUE) {
            TL_LOG_I("SK entry done: 0x%08X",ret);
            bProcessingSKEntry = 0;
            commandSKContinue = 0xFFFFFFFF;
            tlApiReleaseKeypad();
          }
        } else if (commandId == commandSKCancel.cmdId) {
          // find and process the "cancel" command
          ret = (*(commandSKCancel.func))(tci_message);
          TL_LOG_I("SK entry cancelled");
          bProcessingSKEntry = 0;
          commandSKContinue = 0xFFFFFFFF;
          if (ret != CKR_OK) {
            TL_LOG_E("Failed to release keypad? Masking");
            ret = CKR_OK;
          }
        } else {
          TL_LOG_E("Performing a Secure Keypad entry, command invalid now: %d",commandId);
          ret = CKR_INVALID_COMMAND;
        }
      } else {
        // first check if the command is supposed to initiate a SK entry
        ret = CKR_UNKNOWN_COMMAND;
        for (i=0; i<LENGTH(commandsSKInit); i++) {
          if ((commandsSKInit[i].cmdId == commandId) && (commandsSKInit[i].func != NULL)) {
            ret = (*(commandsSKInit[i].func))(tci_message);
            break;
          }
        }
        if (ret == CKR_SK_CONTINUE) {
          TL_LOG_I("SK entry started");
          bProcessingSKEntry = 1;
          commandSKContinue = commandsSKContinue[i].cmdId; /*lint !e661*/
        } else if (ret == CKR_UNKNOWN_COMMAND) {
#endif
          // Process command message
          ret = CKR_UNKNOWN_COMMAND;
          for (i=0; i<LENGTH(commands); i++) {
            if ((commands[i].cmdId == commandId) && (commands[i].func != NULL)) {
              ret = (*(commands[i].func))(tci_message);
              break;
            }
          }
#ifdef SK_ENABLED
        } else {
          // SK init command failed! do nothing
        }
      }
#endif
    }

    memscpy(&(tci->message),sizeof(tciMessage_t),tci_message,sizeof(tciMessage_t));
    // Set up response header -> mask response ID and set return code
    tci->message.responseHeader.responseId = RSP_ID(commandId); /*lint !e613*/
    tci->message.responseHeader.returnCode = ret; /*lint !e613*/

    // Notify back the TLC
    tlApiNotify();
  }

}
