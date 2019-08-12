/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlP11Utils.h"
#include "tlLog.h"
#include "tlP11Glob.h"


/** Process a GetTokenChallenge command
 * @param shared buffer containing the command
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGetTokenChallenge(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint64_t time = 0;
  size_t hashLen = SHA256_HASH_LEN;
  tlApiCrSession_t digestSession;
  size_t randomLen = 0;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdGetTokenChallenge.params.so_token.value;

  do {
    TL_LOG_I("processCmdGetTokenChallenge+");

    ret = processTokenGroupSession(
        &tci_message->cmdGetTokenChallenge.params.so_token,
        &tci_message->cmdGetTokenChallenge.params.so_group,
        tci_message->cmdGetTokenChallenge.params.sessionId,
        g_token_0_ptr,
        &g_session_group_0,
        NULL
        );

    if (ret != CKR_OK) {
      break;
    }

    // generate the challenge
    time = UINT64_MAX;
    if ((g_tsSource != TS_SOURCE_ILLEGAL) &&
        (!IS_OK(tlApiGetTimeStamp(&time,g_tsSource)))) {
      time = UINT64_MAX;
    }
    randomLen = TOKEN_CHALLENGE_RANDOM_LENGTH;
    result = tlApiRandomGenerateData(TLAPI_ALG_PSEUDO_RANDOM,g_session_group_0.authInfo.challenge,&randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
    if (randomLen != TOKEN_CHALLENGE_RANDOM_LENGTH) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
          randomLen,TOKEN_CHALLENGE_RANDOM_LENGTH);
      ret = CKR_FUNCTION_FAILED;
      break;
    }
    memscpy(g_session_group_0.authInfo.challenge+TOKEN_CHALLENGE_RANDOM_LENGTH,TOKEN_CHALLENGE_LEN-TOKEN_CHALLENGE_RANDOM_LENGTH,&time,sizeof(time));

    // hash the challenge
    result = CRYPTO_OP(tlApiMessageDigestInit(&digestSession,TLAPI_ALG_SHA256));
    if (IS_STUBBED(result)) {
      TL_LOG_W("STUBBED: tlApiMessageDigestInit");
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("Failed to init Hash function");
      ret = TL2CK(result);
      break;
    }
    result = CRYPTO_OP(tlApiMessageDigestDoFinal(
        digestSession,
        g_session_group_0.authInfo.challenge,
        TOKEN_CHALLENGE_LEN,
        tci_message->rspGetTokenChallenge.params.value,
        &hashLen));
    if (IS_STUBBED(result)) {
      TL_LOG_W("STUBBED: tlApiMessageDigestDoFinal");
      result = TLAPI_OK;
      memscpy(tci_message->rspGetTokenChallenge.params.value,
          TOKEN_CHALLENGE_WRAPPED_LEN,
          g_session_group_0.authInfo.challenge,
          min(TOKEN_CHALLENGE_LEN,hashLen));
    }
    if (!IS_OK(result)) {
      TL_LOG_E("Failed to init Hash function");
      ret = TL2CK(result);
      break;
    }
    // save the challenge in the session group object
    g_session_group_0.changed = 1;


  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspGetTokenChallenge.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);


	TL_LOG_I("processCmdGetTokenChallenge- ret:0x%08X",ret);
  return ret;
}
