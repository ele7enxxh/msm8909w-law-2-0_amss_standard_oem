/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlLog.h"
#include "tlP11Api.h"
#include "tlP11Utils.h"
#include "tlP11Validation.h"
#include "tlP11Mechanisms.h"

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_SIGN_VERIFY_INIT_FUNC(hotp_sign_init)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlApiSigAlg_t alg = TLAPI_ALG_HMAC_SHA1;
  tlApiKey_t mcKey = {0}; /*lint !e708*/
  tlApiSymKey_t symKey = {0};
  tlApiSigMode_t mode;
  CK_BBOOL bRequestingTime = CK_FALSE;
  CK_BBOOL bRequestingCounter = CK_FALSE;
  TL_LOG_I("hotp_sign_init+");
  do {
    // check the input
    if (!session || !key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (!IS_TYPE(key,tlp11_type_otp_key)) {
      TL_LOG_E(" Wrong key type");
      rv = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }
    // check the key
    if (operation == CKF_SIGN) {
      mode = TLAPI_MODE_SIGN;
      if (key->secret_key.sign == 0) {
        TL_LOG_E(" Key doesn't support signing");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
    } else if (operation == CKF_VERIFY) {
      if (key->secret_key.verify == 0) {
        TL_LOG_E(" Key doesn't support signature verification");
        rv = CKR_KEY_FUNCTION_NOT_PERMITTED;
        break;
      }
      // because of how HMAC OTP works, verification always implies signature generation
      mode = TLAPI_MODE_SIGN;
    } else {
      TL_LOG_E(" Sign function called for operation other than sign/verify: 0x%08X",operation);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    // copy over the key
    session->context.key.symmetric.len = key->otp.len;
    memscpy(session->context.key.symmetric.value, AES256_KEY_LEN, key->otp.value,key->otp.len);
    symKey.key = session->context.key.symmetric.value;
    symKey.len = session->context.key.symmetric.len;
    mcKey.symKey = &symKey;

    switch (mech) {
    case CKM_HOTP:
      alg = TLAPI_ALG_HMAC_SHA1;
      break;
    default:
      TL_LOG_E(" Unsupported mechanism: 0x%08X",mech);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (rv != CKR_OK) break;

    if ((params == NULL) || (params->header.valid == 0)) {
      TL_LOG_E(" NULL param input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (params->OTP_PARAMS.output_format != CK_OTP_FORMAT_DECIMAL) {
      TL_LOG_E("Format 0x%08X not supported",params->OTP_PARAMS.output_format);
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (params->OTP_PARAMS.flags & CKF_USER_FRIENDLY_OTP) {
      TL_LOG_E("User-friendly mode not supported");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    // check for validity of parameters against
    if (key->otp.time_requirement == CK_OTP_PARAM_MANDATORY) {
      TL_LOG_I("Time requirement mandatory in the key");
      if (params->OTP_PARAMS.flags & CKF_EXCLUDE_TIME) {
        TL_LOG_E("Time mandatory by the key, CKF_EXCLUDE_TIME not allowed");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if (params->OTP_PARAMS.time_valid == CK_FALSE) {
        TL_LOG_E("Time mandatory by the key, a valid value has to be supplied");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    }
    if (key->otp.counter_requirement == CK_OTP_PARAM_MANDATORY) {
      TL_LOG_I("Counter requirement mandatory in the key");
      if (params->OTP_PARAMS.flags & CKF_EXCLUDE_COUNTER) {
        TL_LOG_E("Counter mandatory by the key, CKF_EXCLUDE_COUNTER not allowed");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
      if (params->OTP_PARAMS.counter_valid == CK_FALSE) {
        TL_LOG_E("Counter mandatory by the key, a valid value has to be supplied");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    }
    // what should go in the buffer: either time or counter
    if ((params->OTP_PARAMS.flags & CKF_EXCLUDE_TIME) == 0) {
      bRequestingTime = CK_TRUE;
    }
    if ((params->OTP_PARAMS.flags & CKF_EXCLUDE_COUNTER) == 0) {
      bRequestingCounter = CK_TRUE;
    }
    if (bRequestingTime && bRequestingCounter) {
      TL_LOG_E("Time and Counter not supported at the same time");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    } else if (!bRequestingTime && !bRequestingCounter) {
      TL_LOG_E("Either Time or Counter has to be requested");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }

    if (bRequestingTime &&
        (key->otp.time_requirement == CK_OTP_PARAM_NOT_SUPPORTED)) {
      TL_LOG_E("Requesting time, but time is not supported with this key");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (bRequestingCounter &&
        (key->otp.counter_requirement == CK_OTP_PARAM_NOT_SUPPORTED)) {
      TL_LOG_E("Requesting counter, but counter is not supported with this key");
      rv = CKR_MECHANISM_PARAM_INVALID;
      break;
    }
    if (params->OTP_PARAMS.output_length_valid) {
      if ((params->OTP_PARAMS.output_length < OTP_LENGTH_MIN) ||
          (params->OTP_PARAMS.output_length > OTP_LENGTH_MAX)) {
        TL_LOG_E("CK_OUTPUT_LENGTH out of range: %d",params->OTP_PARAMS.output_length);
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    }

    // ok, we can save a copy of the parameters, and possibly modify them in place
    session->context.params.otp = params->OTP_PARAMS;

    // if counter was requested
    if (bRequestingCounter) {
      // increasing counter by 1 if CKF_NEXT_OTP on counter request
      if (params->OTP_PARAMS.flags & CKF_NEXT_OTP) {
        *((uint64_t*)(&session->context.params.otp.counter)) += 1;
      }

      // here we cheat. If we are requested to use the counter from the key,
      // we copy over the counter to the parameters and mark it as valid
      if ((key->otp.counter_requirement == CK_OTP_PARAM_IGNORED) ||
          ((key->otp.counter_requirement == CK_OTP_PARAM_OPTIONAL) &&
           (params->OTP_PARAMS.counter_valid == CK_FALSE))) {
        session->context.params.otp.counter_valid = CK_TRUE;
        if (params->OTP_PARAMS.flags & CKF_NEXT_OTP) {
          *((uint64_t*)(&key->otp.counter)) += 1;
        }
        memscpy(session->context.params.otp.counter,OTP_COUNTER_LEN,key->otp.counter,OTP_COUNTER_LEN);
        TL_LOG_I("Using key counter: %llu",*((uint64_t*)(&key->otp.counter)));
        *((uint64_t*)(&key->otp.counter)) += 1;
        key->header.changed = 1;
      }
    }
    // if time was requested
    else {
      // increasing counter by timestep if CKF_NEXT_OTP on counter request
      if (params->OTP_PARAMS.flags & CKF_NEXT_OTP) {
        *((uint64_t*)(&session->context.params.otp.time)) += *((CK_ULONG*)(&session->context.params.otp.time_step));
        (*((uint64_t*)(&session->context.params.otp.number_of_steps)))++;

      }

      // if we are requested to use the time from the trustlet
      if ((key->otp.time_requirement == CK_OTP_PARAM_IGNORED) ||
          ((key->otp.time_requirement == CK_OTP_PARAM_OPTIONAL) &&
           (params->OTP_PARAMS.time_valid == CK_FALSE))) {
        TL_LOG_E("Time is currently not supported");
        rv = CKR_MECHANISM_PARAM_INVALID;
        break;
      }
    }

    if (params->OTP_PARAMS.output_length_valid == CK_FALSE) {
      session->context.params.otp.output_length = key->otp.length;
    }

    result = CRYPTO_OP(tlApiSignatureInit(
          &session->context.cryptoSession,
          &mcKey,
          mode,
          alg));
    if (IS_STUBBED(result)) {
      session->context.cryptoSession=0x12345678;
      result = TLAPI_OK;
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiSignatureInit: 0x%08X",result);
      rv=TL2CK(result);
      break;
    }
    DUMP_HEX("key",symKey.key,symKey.len);
  } while(0);
  TL_LOG_I("hotp_sign_init-: %08X",rv);
  return rv;
}

/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_SIGN_VERIFY_FINAL_FUNC(hotp_sign_final)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  tlp11_CK_OTP_PARAMS_t *params = NULL;
  tlp11_CK_OTP_SIGNATURE_INFO_t *info = NULL;
  uint8_t input[OTP_COUNTER_LEN] = {0};
  size_t input_len = sizeof(input);
  uint8_t output[SHA1_HASH_LEN] = {0};
  size_t output_len = sizeof(output);
  size_t offset = 0;

  TL_LOG_I("hotp_sign_final+");
  do {
    if (session->context.cryptoSession != CR_SID_INVALID) {
      if (session->context.params.otp.header.valid == CK_FALSE) {
        TL_LOG_E("Invalid parameters saved!");
        rv = CKR_GENERAL_ERROR;
        break;
      }
      if (session->context.mechanism != CKM_HOTP) {
        TL_LOG_E("Invalid mechanism!");
        rv = CKR_GENERAL_ERROR;
        break;
      }
      params = &session->context.params.otp;
        // we ignore passed input data (might be null)
        // and build a buffer from the content of the parameters
        //DUMP_HEX("Counter",params->counter,sizeof(params->counter));

      //prepare input
      if ((params->flags & CKF_EXCLUDE_TIME) == 0) {
        uint8_t *timev = (uint8_t*)(&params->number_of_steps);
        for (unsigned int i=0; i<sizeof(params->number_of_steps); i++) {
          input[i] = timev[sizeof(params->number_of_steps)-i-1];
        }
        input_len = sizeof(params->time);
      } else {
        for (int i=0; i<OTP_COUNTER_LEN; i++) {
          input[i] = params->counter[OTP_COUNTER_LEN-i-1];
        }
        input_len = OTP_COUNTER_LEN;
      }
      DUMP_HEX("Counter/Time",input,input_len);
      result = CRYPTO_OP(tlApiSignatureSign(
          session->context.cryptoSession,
          input,
          input_len,
          output,
          &output_len));
      if (IS_STUBBED(result)) {
        result = TLAPI_OK;
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiSignatureSign: 0x%08X",result);
        rv=TL2CK(result);
        break;
      }
      // now set the output buffer
      if (dataOut == NULL) {
        TL_LOG_E("NULL output buffer");
        rv = CKR_ARGUMENTS_BAD;
        break;
      }
      if (*dataOutLen < sizeof(tlp11_CK_OTP_SIGNATURE_INFO_t)) {
        TL_LOG_E("Output buffer too small");
        rv = CKR_BUFFER_TOO_SMALL;
        break;
      }
      // move over to output the parameter structure
      info = (tlp11_CK_OTP_SIGNATURE_INFO_t*)dataOut;

      //DUMP_HEX("HS",output,output_len);
      info->pin_len=params->pin_len;
      memscpy(info->pin,OTP_PIN_MAX_LEN,params->pin,OTP_PIN_MAX_LEN);
      info->challenge_len=params->challenge_len;
      memscpy(info->challenge,OTP_CHALLENGE_LEN,params->challenge,OTP_CHALLENGE_LEN);
      info->time = params->time;
      info->time_step = params->time_step;
      memscpy(info->counter,OTP_COUNTER_LEN,params->counter,OTP_COUNTER_LEN);
      info->pin_valid = params->pin_valid;
      info->challenge_valid = params->challenge_valid;
      info->time_valid = params->time_valid;
      info->counter_valid = params->counter_valid;
      info->output_length_valid = params->output_length_valid;
      info->flags = params->flags;
      info->output_length = params->output_length;
      info->output_format = params->output_format;

      // copy over the truncated version
      offset = (output[output_len-1] & 0x0F);
      TL_LOG_I("Offset: %d", offset);

      info->value = ((output[offset] & 0x7F) << 24)
          | ((output[offset+1] & 0xFF) << 16)
          | ((output[offset+2] & 0xFF) << 8)
          | (output[offset+3] & 0xFF);

      *dataOutLen = sizeof(tlp11_CK_OTP_SIGNATURE_INFO_t);
      TL_LOG_I("Value: %08X", info->value);

    } else {
      TL_LOG_E("Invalid MC CryptoSession!");
      rv = CKR_OPERATION_NOT_INITIALIZED;
      break;
    }
  } while(0);

  TL_LOG_I("hotp_sign_final-: %08X",rv);
  return rv;
}
