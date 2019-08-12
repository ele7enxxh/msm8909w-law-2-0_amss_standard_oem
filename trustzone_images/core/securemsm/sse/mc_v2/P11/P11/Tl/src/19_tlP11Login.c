/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
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

/** Starts a Login process on a secure keypad
 * This function will return BEFORE any key has been pressed, but AFTER the
 * keypad has been acquired.
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdLogin(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiCrSession_t digestSession;
  tlApiCrSession_t signSession;
  uint32_t index;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdLogin.params.so_token.value;

  do {
    TL_LOG_I("processCmdLogin+");

    ret = processTokenGroupSession(
        &tci_message->cmdLogin.params.so_token,
        &tci_message->cmdLogin.params.so_group,
        tci_message->cmdLogin.params.sessionId,
        g_token_0_ptr,
        &g_session_group_0,
        &index
        );

    if (ret != CKR_OK) {
      break;
    }

    if ((tci_message->cmdLogin.params.user == CKU_USER) ||
        (tci_message->cmdLogin.params.user == CKU_SO)) {
      // valid authentication
      if (tci_message->cmdLogin.params.user == g_session_group_0.authInfo.authEntity) {
        ret = CKR_USER_ALREADY_LOGGED_IN;
        break;
      }
      if (g_session_group_0.authInfo.authEntity != CKU_NONE) {
        ret = CKR_USER_ANOTHER_ALREADY_LOGGED_IN;
        break;
      }
    }

    if (tci_message->cmdLogin.params.user == CKU_USER) {
      if (g_token_0_ptr->pinLen == 0) {
        ret = CKR_USER_PIN_NOT_INITIALIZED;
        break;
      }
      if ((g_token_0_ptr->info.flags & CKF_USER_PIN_LOCKED) &&
        (unlockToken(g_token_0_ptr) & CKF_USER_PIN_LOCKED) ){
        ret = CKR_PIN_LOCKED;
        break;
      }
      // ok to proceed, let's get the PIN
      if ((g_config.arg.flags & CKF_PROTECTED_AUTHENTICATION_PATH) &&
          (g_token_0_ptr->info.flags & CKF_PROTECTED_AUTHENTICATION_PATH)) {
        // this version of the command CANNOT be called on token supporting
        // the secure keypad
        TL_LOG_E("This token does supports secure keypad!");
        ret = CKR_FUNCTION_FAILED;
        break;
      }

      if (tci_message->cmdLogin.params.pinLen != g_token_0_ptr->pinLen) {
        TL_LOG_E("Passed PIN, wrong length");
        ret = CKR_PIN_INCORRECT;
        break;
      }

      if (0 != timesafe_memcmp(tci_message->cmdLogin.params.pin,g_token_0_ptr->pin,g_token_0_ptr->pinLen)) {
        TL_LOG_E("PIN doesn't match");
        ret = CKR_PIN_INCORRECT;
        break;
      }
    } else if (tci_message->cmdLogin.params.user == CKU_SO) {
      if (g_token_0_ptr->associatedKey.header.valid == 0) {
        TL_LOG_E("No key, SO not allowed to login");
        ret = CKR_USER_TYPE_INVALID;
        break;
      }
      if ((g_token_0_ptr->info.flags & CKF_SO_PIN_LOCKED) &&
        (unlockToken(g_token_0_ptr) & CKF_SO_PIN_LOCKED) ){
        ret = CKR_PIN_LOCKED;
        break;
      }
      {
        uint64_t time;
        uint64_t now;
        // check the the current challenge is valid
        int i=0;
        for (i=0; i<TOKEN_CHALLENGE_LEN; i++) {
          if (g_session_group_0.authInfo.challenge[i] != 0)
            break;
        }
        if (i==TOKEN_CHALLENGE_LEN) {
          TL_LOG_E("Empty recorded challenge");
          ret = CKR_PIN_INCORRECT;
          break;
        }
        // check the time for the recorded challenge
        // we only perform the check if we have a timing system
        if (g_tsSource != TS_SOURCE_ILLEGAL) {
          memscpy(&time,sizeof(time),g_session_group_0.authInfo.challenge+TOKEN_CHALLENGE_RANDOM_LENGTH,sizeof(time));
          now = UINT64_MAX;
          if ((g_tsSource != TS_SOURCE_ILLEGAL) &&
              (!IS_OK(tlApiGetTimeStamp(&now,g_tsSource)))) {
            now = UINT64_MAX;
          }
          TL_LOG_I("now=%llu, time=%llu, challengeLifespan=%d",
              now,time,g_config.arg.challengeLifespan);
          if (now - time > g_config.arg.challengeLifespan) {
            TL_LOG_E("Challenge expired!");
            ret = CKR_PIN_INCORRECT;
            break;
          }
        }
      }
      {
        size_t hashLen = SHA256_HASH_LEN;
        // => buffer={rA,SHA256(challenge),label} -> 32+32+32=96B
        uint8_t buffer[SHA256_HASH_LEN+SHA256_HASH_LEN+TOKEN_LABEL_LEN];
        memscpy(buffer,sizeof(buffer),tci_message->cmdLogin.params.pin,SHA256_HASH_LEN);
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
            buffer+SHA256_HASH_LEN,
            &hashLen));
        if (IS_STUBBED(result)) {
          TL_LOG_W("STUBBED: tlApiMessageDigestDoFinal");
          result = TLAPI_OK;
          memscpy(buffer+SHA256_HASH_LEN,
              sizeof(buffer)-SHA256_HASH_LEN,
              g_session_group_0.authInfo.challenge,
              min(TOKEN_CHALLENGE_LEN,hashLen));
        }
        if (!IS_OK(result)) {
          TL_LOG_E("Failed to init Hash function");
          ret = TL2CK(result);
          break;
        }
        // append the label
        memscpy(buffer+2*SHA256_HASH_LEN,
            sizeof(buffer)-(2*SHA256_HASH_LEN),
            g_token_0_ptr->info.label,
            TOKEN_LABEL_LEN);
        // buffer ready
        if (g_token_0_ptr->associatedKey.header.type == CKK_AES) {
          // pin = {rA,Hk(rA,rB,label)} -> 32+32=64B
          uint8_t hk[SHA256_HASH_LEN];
          size_t hkLen = SHA256_HASH_LEN;
          uint8_t *key = NULL;
          uint32_t keyLen = 0;
          tlApiKey_t signKey = {0}; /*lint !e708*/
          tlApiSymKey_t symSignKey = {0};
          TL_LOG_I("AES token");
          if (tci_message->cmdLogin.params.pinLen != (TOKEN_CHALLENGE_WRAPPED_LEN+SHA256_HASH_LEN)) {
            TL_LOG_E("Answer: wrong length. %d vs %d",
                tci_message->cmdLogin.params.pinLen,
                TOKEN_CHALLENGE_WRAPPED_LEN+SHA256_HASH_LEN);
            ret = CKR_PIN_INCORRECT;
            break;
          }
          // Hk(rA,SHA256(challenge),label)
          key = g_token_0_ptr->associatedKey.symmetric.value;
          keyLen = g_token_0_ptr->associatedKey.symmetric.len;
          symSignKey.len = keyLen;
          symSignKey.key = key;
          signKey.symKey = &symSignKey;
          result = CRYPTO_OP(tlApiSignatureInit(&signSession,&signKey,TLAPI_MODE_SIGN,TLAPI_ALG_HMAC_SHA_256));
          if (IS_STUBBED(result)) {
            TL_LOG_W("STUBBED: tlApiSignatureInit");
            result = TLAPI_OK;
          }
          if (!IS_OK(result)) {
            TL_LOG_E("Failed to initialise signature session");
            ret = TL2CK(result);
            break;
          }
          // sign
          result = CRYPTO_OP(tlApiSignatureSign(
              signSession,
              buffer,
              sizeof(buffer),
              hk,
              &hkLen));
          if (IS_STUBBED(result)) {
            memscpy(hk,
                sizeof(hk),
                buffer,
                min(hkLen,sizeof(buffer)));
            TL_LOG_W("STUBBED: tlApiSignatureSign");
            result = TLAPI_OK;
          }
          if (!IS_OK(result)) {
            TL_LOG_E("Failed to generate Hk");
            ret = TL2CK(result);
            break;
          }
          // we have Hk, now compare it with what we have been given
          // pin = {rA,Hk(rA,rB,label)} -> 32+32=64B
          if (0 != timesafe_memcmp(tci_message->cmdLogin.params.pin+SHA256_HASH_LEN,hk,SHA256_HASH_LEN)) {
            TL_LOG_E("Answer: wrong pin");
            DUMP_HEX("Expected",hk,sizeof(hk));
            DUMP_HEX("Received",(uint8_t*)tci_message->cmdLogin.params.pin+SHA256_HASH_LEN,SHA256_HASH_LEN);
            ret = CKR_PIN_INCORRECT;
            break;
          }
          TL_LOG_I("Comparison OK");

          // authentication ok!
        } else if (g_token_0_ptr->associatedKey.header.type == CKK_RSA) {
          // pin = {rA,Sk(rA,rB,label)} -> 32+256=288B
          tlApiKey_t signKey = {0};  /*lint !e708*/
          tlApiRsaKey_t rsaKey = {0};
          bool signValid = 0;
          TL_LOG_I("RSA token");
          if (tci_message->cmdLogin.params.pinLen != (TOKEN_CHALLENGE_WRAPPED_LEN+MODULUS_MAX_LEN)) {
            TL_LOG_E("Answer: wrong length. %d vs %d",
                tci_message->cmdLogin.params.pinLen,
                TOKEN_CHALLENGE_WRAPPED_LEN+MODULUS_MAX_LEN);
            ret = CKR_PIN_INCORRECT;
            break;
          }
          // Sk(rA,SHA256(challenge),label)
          rsaKey.exponent.value = g_token_0_ptr->associatedKey.asymmetric.exponent;
          rsaKey.exponent.len = g_token_0_ptr->associatedKey.asymmetric.exponent_len;
          rsaKey.modulus.value = g_token_0_ptr->associatedKey.asymmetric.modulus;
          rsaKey.modulus.len = g_token_0_ptr->associatedKey.asymmetric.modulus_len;

          signKey.rsaKey = &rsaKey;
          result = CRYPTO_OP(tlApiSignatureInitWithData(&signSession,&signKey,TLAPI_MODE_VERIFY,TLAPI_SIG_RSA_SHA256_PSS,NULL,SHA256_HASH_LEN));
          if (IS_STUBBED(result)) {
            TL_LOG_W("STUBBED: tlApiSignatureInit");
            result = TLAPI_OK;
          }
          if (!IS_OK(result)) {
            TL_LOG_E("Failed to initialise signature session");
            ret = TL2CK(result);
            break;
          }
          // verify
          result = CRYPTO_OP(tlApiSignatureVerify(
              signSession,
              buffer,
              sizeof(buffer),
              (uint8_t*)tci_message->cmdLogin.params.pin+SHA256_HASH_LEN,
              tci_message->cmdLogin.params.pinLen-SHA256_HASH_LEN,
              &signValid));
          if (IS_STUBBED(result)) {
            TL_LOG_W("STUBBED: tlApiSignatureVerify");
            signValid = ( 0 == timesafe_memcmp(
                buffer,tci_message->cmdLogin.params.pin+SHA256_HASH_LEN,
                min(sizeof(buffer),tci_message->cmdLogin.params.pinLen-SHA256_HASH_LEN)));
            result = TLAPI_OK;
          }
          if (!IS_OK(result)) {
            TL_LOG_E("Failed to verify Sk");
            ret = TL2CK(result);
            break;
          }
          if (!signValid) {
            TL_LOG_E("Verification failed");
            DUMP_HEX("Expected",buffer,min(sizeof(buffer),tci_message->cmdLogin.params.pinLen-SHA256_HASH_LEN));
            DUMP_HEX("Got",(uint8_t*)tci_message->cmdLogin.params.pin+SHA256_HASH_LEN,min(sizeof(buffer),tci_message->cmdLogin.params.pinLen-SHA256_HASH_LEN));
            ret = CKR_PIN_INCORRECT;
            break;
          }
          // authentication ok!
        } else {
          TL_LOG_E("Token with unknown key!");
          ret = CKR_GENERAL_ERROR;
          break;
        }
      }
      break;
    } else if (tci_message->cmdLogin.params.user == CKU_CONTEXT_SPECIFIC) {
      ret = CKR_OPERATION_NOT_INITIALIZED;
      break;
    } else {
      ret = CKR_USER_TYPE_INVALID;
      break;
    }
  } while(0);

  // now, tidy up token
  do {
    if (ret == CKR_OK) {
      // login was successful
      g_session_group_0.authInfo.usageNumLeft = g_token_0_ptr->usageNum;
      g_session_group_0.authInfo.lastAuthTime = 0;
      if (g_tsSource != TS_SOURCE_ILLEGAL) {
        if (!IS_OK(tlApiGetTimeStamp(&g_session_group_0.authInfo.lastAuthTime,g_tsSource))) {
          g_session_group_0.authInfo.lastAuthTime = 0;
        }
      }
      if (tci_message->cmdLogin.params.user == CKU_USER) {
        g_session_group_0.authInfo.authEntity = CKU_USER;
        g_token_0_ptr->loginAttempts = 0;
        g_token_0_ptr->info.flags &= ~CKF_USER_PIN_COUNT_LOW;
        g_token_0_ptr->info.flags &= ~CKF_USER_PIN_FINAL_TRY;
        g_token_0_ptr->info.flags &= ~CKF_USER_PIN_LOCKED;
      } else {
        // remote login
        g_session_group_0.authInfo.authEntity = CKU_SO;
        g_token_0_ptr->remoteLoginAttempts = 0;
        g_token_0_ptr->info.flags &= ~CKF_SO_PIN_COUNT_LOW;
        g_token_0_ptr->info.flags &= ~CKF_SO_PIN_FINAL_TRY;
        g_token_0_ptr->info.flags &= ~CKF_SO_PIN_LOCKED;
      }
    } else if (ret == CKR_PIN_INCORRECT) {
      // login failed, we need to update the lock flags
      if (tci_message->cmdLogin.params.user == CKU_USER) {
        g_token_0_ptr->loginAttempts++;
        g_token_0_ptr->info.flags |= CKF_USER_PIN_COUNT_LOW;
        if (g_token_0_ptr->loginAttempts == g_config.arg.maxLocalLoginAttempts-1) {
          g_token_0_ptr->info.flags |= CKF_USER_PIN_FINAL_TRY;
        } else if (g_token_0_ptr->loginAttempts == g_config.arg.maxLocalLoginAttempts) {
          lockToken(g_token_0_ptr,CKU_USER);
        }
      } else {
        // remote login
        g_token_0_ptr->remoteLoginAttempts++;
        g_token_0_ptr->info.flags |= CKF_SO_PIN_COUNT_LOW;
        if (g_token_0_ptr->remoteLoginAttempts == g_config.arg.maxRemoteLoginAttempts-1) {
          g_token_0_ptr->info.flags |= CKF_SO_PIN_FINAL_TRY;
        } else if (g_token_0_ptr->remoteLoginAttempts == g_config.arg.maxRemoteLoginAttempts) {
          lockToken(g_token_0_ptr,CKU_SO);
        }
      }
    } else {
      // for all other error values we don't actually have to update anything
      break;
    }
    result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspLogin.params.so_group));
    if (!IS_OK(result)) {
      TL_LOG_E("wrapSessionGroup: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
    result = wrapToken(g_token_0_ptr,&(tci_message->rspLogin.params.so_token));
    if (!IS_OK(result)) {
      TL_LOG_E("wrapToken: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
  } while(0);

  onAuthFailed(ret);

	TL_LOG_I("processCmdLogin- ret:0x%08X",ret);
  return ret;
}
