/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlLog.h"
#include "tlP11Utils.h"
#include "tlP11Glob.h"
#include "asn1.h"
#include "tlP11Validation.h"

#define TL_RESERVED_MEMORY_END    0x00100000

CK_RV validateBufferLocation(void *ptr) {
  CK_RV rv = CKR_OK;
#ifndef DEVLIB
  uint32_t ptrVal = (uint32_t)ptr;
  // NULL is ok
  if ((ptrVal > 0) && (ptrVal < TL_RESERVED_MEMORY_END)) {
    TL_LOG_E("Pointer at invalid address: 0x%08X",ptr);
    rv = CKR_GENERAL_ERROR;
  }
#endif
  return rv;
}

/*lint -e{788}*/
CK_RV TL2CK(tlApiResult_t err) {
  CK_RV rv = CKR_OK;

  switch(err) {
  case TLAPI_OK: rv = CKR_OK; break;
  case E_TLAPI_NOT_IMPLEMENTED: rv = CKR_FUNCTION_FAILED; break;
  case E_TLAPI_UNKNOWN: rv = CKR_GENERAL_ERROR; break;
  case E_TLAPI_UNKNOWN_FUNCTION: rv = CKR_FUNCTION_FAILED; break;
  case E_TLAPI_INVALID_INPUT: rv = CKR_ARGUMENTS_BAD; break;
  case E_TLAPI_INVALID_RANGE: rv = CKR_ARGUMENTS_BAD; break;
  case E_TLAPI_BUFFER_TOO_SMALL: rv = CKR_BUFFER_TOO_SMALL; break;
  case E_TLAPI_INVALID_TIMEOUT: rv = CKR_ARGUMENTS_BAD; break;
  case E_TLAPI_TIMEOUT: rv = CKR_GENERAL_ERROR; break;
  case E_TLAPI_NULL_POINTER: rv = CKR_ARGUMENTS_BAD; break;
  case E_TLAPI_INVALID_PARAMETER: rv = CKR_ARGUMENTS_BAD; break;

  case E_TLAPI_COM_WAIT: rv = CKR_GENERAL_ERROR; break;
  case E_TLAPI_COM_ERROR: rv = CKR_GENERAL_ERROR; break;

  case E_TLAPI_CR_HANDLE_INVALID: rv = CKR_ARGUMENTS_BAD; break;

  case E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE: rv = CKR_MECHANISM_INVALID; break;
  case E_TLAPI_CR_ALGORITHM_NOT_SUPPORTED: rv = CKR_MECHANISM_INVALID; break;
  case E_TLAPI_CR_WRONG_INPUT_SIZE: rv = CKR_ARGUMENTS_BAD; break;
  case E_TLAPI_CR_WRONG_OUPUT_SIZE: rv = CKR_ARGUMENTS_BAD; break;
  case E_TLAPI_CR_INCONSISTENT_DATA: rv = CKR_ARGUMENTS_BAD; break;
  case E_TLAPI_CR_OUT_OF_RESOURCES: rv = CKR_ARGUMENTS_BAD; break;
  case E_TLAPI_SO_WRONG_CHECKSUM: rv = CKR_GENERAL_ERROR; break;
  default: rv = 0x90000000 + (uint32_t)err; break;
  }
  return rv;
}

CK_RV zeroAttributeTemplate(tlp11_attribute_template_t *templ) {
  CK_RV rv = CKR_OK;
  size_t i = 0;
  TL_LOG_I("zeroAttributeTemplate+");
  do {
    if (!templ) {
      TL_LOG_E("NULL input");
      rv = CKR_ARGUMENTS_BAD;
      break;
    }
    templ->array_attribute.type = CK_UNAVAILABLE_INFORMATION;
    for (i=0; i<MAX_ULONG_ATTRIBUTES_IN_TEMPLATE; i++) {
      templ->ulong_attributes[i].type=CK_UNAVAILABLE_INFORMATION;
    }
    for (i=0; i<MAX_BBOOL_ATTRIBUTES_IN_TEMPLATE; i++) {
      templ->bbool_attributes[i].type=CK_UNAVAILABLE_INFORMATION;
    }
  } while(0);
  TL_LOG_I("zeroAttributeTemplate-: 0x%08X",rv);
  return rv;
}

void onAuthFailed(const CK_ULONG ret) {
  if ((ret != CKR_OK) && (ret != CKR_SK_CONTINUE)) {
    TL_LOG_E("Command failed, forcing reloading.");
    loginFailed = 1;
  }
}

#ifdef SK_ENABLED
CK_RV getPIN(
    const uint32_t minLen,
    const uint32_t maxLen,
    UTF8CHAR *_pin,
    uint32_t *_pinLen,
    tlp11_keypad_event_t *event,
    uint32_t *_pinFailed
    )
{
  CK_RV ret = CKR_SK_CONTINUE;
  tlApiResult_t result = TLAPI_OK;
  uint32_t c = 0;

  do {
    TL_LOG_I("getPIN+");

    if ((!_pin) ||
        (!_pinLen) ||
        (!event) ||
        (!_pinFailed) ||
        (minLen > maxLen)) {
      TL_LOG_E("Inconsistent input");
      ret = CKR_ARGUMENTS_BAD;
      break;
    }

#if 1
    result = tlApiGetC(&c,TLAPI_INFINITE_TIMEOUT);
#else
    if (*_pinLen == minLen)
      c = RETURN_CHAR;
    else
      c = *_pinLen + '0';
    result = CKR_STUBBED;
#endif
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiGetC: %d",result);
      ret = TL2CK(result);
      break;
    }
    if (c == ESCAPE_CHAR) {
      TL_LOG_I("SK entry cancelled!");
      *event = tlp11_keypd_event_cancel;
      ret = CKR_SK_CANCELLED;
      break;
    }

    // user is actually entering the PIN
    if (c == RETURN_CHAR) {
      *event = tlp11_keypd_event_password_done;
      break;
    }
    *event = tlp11_keypd_event_char;
    // normal key, store it
    if (*_pinLen >= maxLen) {
      // user exceeding max pin length, this is a failure
      // we record the failure but do not notify it now
      *_pinFailed = 1;
      break;
    }
    _pin[(*_pinLen)++]=(UTF8CHAR)c;
    // we're good to return
  } while(0);
  TL_LOG_I("getPIN-: 0x%08X",ret);
  return ret;
}

CK_RV verifySavePIN(
    uint32_t *_pinFailed,
    UTF8CHAR  *_pin,
    uint32_t *_pinLen,
    UTF8CHAR  *_pinVerification,
    uint32_t *_pinVerLen,
    tlp11_token_t *token
    ) {
  CK_RV ret = CKR_OK;

  TL_LOG_I("verifySavePIN+");
  do {

    if (!_pinFailed || !_pin || !_pinLen || !_pinVerification || !_pinVerLen || !token) {
      TL_LOG_E("Null input!");
      ret = CKR_GENERAL_ERROR;
      break;
    }

    if (*_pinFailed == 1) {
      TL_LOG_E("PIN too long");
      ret = CKR_PIN_LEN_RANGE;
      break;
    }
    if ((*_pinLen < token->info.ulMinPinLen) ||
        (*_pinLen > token->info.ulMaxPinLen)) {
      TL_LOG_E("PIN out of token range");
      ret = CKR_PIN_LEN_RANGE;
      break;
    }
    // compare the first PIN and what entered as confirmation
    if (*_pinLen != *_pinVerLen) {
      TL_LOG_E("PIN length != confirmation length");
      ret = CKR_PIN_INCORRECT;
      break;
    }
    if (0 != timesafe_memcmp(_pin,_pinVerification,*_pinLen)) {
      TL_LOG_E("PIN != confirmation");
      ret = CKR_PIN_INCORRECT;
      break;
    }
    token->pinLen = *_pinLen;
    memscpy(token->pin,TOKEN_PIN_LEN,_pin,token->pinLen);
    memset(_pin,0,sizeof(_pin));
    memset(_pinVerification,0,sizeof(_pinVerification));
    *_pinLen = 0;
    *_pinVerLen = 0;
    pinMaxLen = 0;
    *_pinFailed = 0;
    pinEntered = 0;
  } while(0);
  TL_LOG_I("verifySavePIN-: 0x%08X",ret);
  return ret;
}
#endif

tlApiResult_t computeChecksum(
    uint8_t *checksum,
    uint8_t *src,
    const uint32_t len,
    const tlp11_object_type_enum_t objType)
{
  tlApiResult_t result = TLAPI_OK;
  tlApiCrSession_t sessionHandle;
  TL_LOG_I("computeChecksum+");
  do {
    if ((objType == tlp11_type_aes_key) ||
        (objType == tlp11_type_des_key) ||
        (objType == tlp11_type_des3_key)){

      uint8_t buffer[AES256_KEY_LEN]={0};
      uint8_t dest[AES256_KEY_LEN]={0};
      size_t dest_len = AES256_KEY_LEN;
      tlApiKey_t key = {0}; /*lint !e708*/
      tlApiSymKey_t symKey = {0};
      tlApiCipherAlg_t alg;
      if (objType == tlp11_type_aes_key) {
        if (len == 16) {
          alg = TLAPI_ALG_AES_128_ECB_NOPAD;
        } else if (len == 32) {
          alg = TLAPI_ALG_AES_256_ECB_NOPAD;
        } else {
          TL_LOG_E("Wrong key length");
          result = E_TLAPI_CR_INCONSISTENT_DATA;
          break;
        }
      } else if (objType == tlp11_type_des_key) {
        alg = TLAPI_ALG_DES_ECB_NOPAD;
      } else if (objType == tlp11_type_des3_key) {
        // WARNING: MC does not support DES3 ECB
        alg = TLAPI_ALG_DES_ECB_NOPAD;
      } else {
        TL_LOG_E("Unsupported secret key type!");
        result = E_TLAPI_CR_INCONSISTENT_DATA;
        break;
      }
      symKey.key = src;
      symKey.len = len;
      key.symKey = &symKey;

      result = CRYPTO_OP(tlApiCipherInit(&sessionHandle,alg,TLAPI_MODE_ENCRYPT,&key));
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherInit: %d",result);
        break;
      }
      // now encrypt
      result = CRYPTO_OP(tlApiCipherDoFinal(sessionHandle,buffer,len,dest,&dest_len));
      if (IS_STUBBED(result)) {
        dest_len = 3;
        memscpy(dest,dest_len,src,min(len,3));
        result = TLAPI_OK;
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherInit: %d",result);
        break;
      }
      if (dest_len < 3) {
        TL_LOG_E("Returned data too short: %d",dest_len);
        break;
      }
      memscpy(checksum,3,dest,3);
    } else {
      uint8_t buffer[SHA1_HASH_LEN] = {0}; // we need this to keep the full hash
      size_t hash_len = sizeof(buffer);
      result = tlApiMessageDigestInit(&sessionHandle,TLAPI_ALG_SHA1);
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiMessageDigestInit: 0x%08X",result);
        break;
      }
      result = tlApiMessageDigestDoFinal(
          sessionHandle,
          src,
          len,
          buffer,
          &hash_len);
      if (IS_STUBBED(result)) {
        memscpy(checksum,3,src,min(3,len));
        result = TLAPI_OK;
      }
      if (!IS_OK(result) || (hash_len < 3)) {
        TL_LOG_E("tlApiMessageDigestDoFinal: %d",result);
        result = E_TLAPI_UNKNOWN;
        break;
      }
      memscpy(checksum,3,buffer,3);
      TL_LOG_I("Checksum: %02X %02X %02X",checksum[0],checksum[1],checksum[2]);
    }
  } while(0);
  TL_LOG_I("computeChecksum-: 0x%08X",result);
  return result;
}

tlApiResult_t wrapObject(
    tlp11_container_object_t *object,
    tlp11_container_object_so_t *so_object)
{
  tlApiResult_t result = TLAPI_OK;
  uint32_t plaintext = 0;
  uint32_t obj_len = 0;
  mcSoLifeTime_t lifetime = MC_SO_LIFETIME_DUMMY;
  TL_LOG_I("wrapObject+");
  do {
    if (!object || !so_object) {
       TL_LOG_E("NULL parameter!");
       result = E_TLAPI_INVALID_PARAMETER;
       break;
    }

    if (object->storage.token) {
      lifetime = MC_SO_LIFETIME_PERMANENT;
    } else {
      lifetime = MC_SO_LIFETIME_POWERCYCLE;
    }

    /*lint -e{788}*/
    switch (object->header.object_type) {
    case tlp11_type_storage:
    case tlp11_type_certificate:
    case tlp11_type_key:
    case tlp11_type_secret_key:
      // this are abstract types, not final objects, and cannot be wrapped
      TL_LOG_E("Trying to wrap an abstract type object");
      result = E_TLAPI_INVALID_INPUT;
      break;
    case tlp11_type_certificate_x509:
      plaintext = sizeof(tlp11_certificate_x509_t); //offsetof(tlp11_certificate_x509_t,so_enc_start);
      obj_len = sizeof(tlp11_certificate_x509_t);
      break;
    case tlp11_type_data:
      plaintext = offsetof(tlp11_data_t,so_enc_start);
      obj_len = sizeof(tlp11_data_t);
      break;
    case tlp11_type_rsa_private_key:
      plaintext = offsetof(tlp11_rsa_private_key_t,so_enc_start);
      obj_len = sizeof(tlp11_rsa_private_key_t);
      break;
    case tlp11_type_rsa_public_key:
      plaintext = sizeof(tlp11_rsa_public_key_t); //offsetof(tlp11_rsa_public_key_t,so_enc_start);
      obj_len = sizeof(tlp11_rsa_public_key_t);
      break;
    case tlp11_type_aes_key:
    case tlp11_type_des3_key:
    case tlp11_type_des_key:
    case tlp11_type_generic_key:
    case tlp11_type_rc4_key:
      plaintext = offsetof(tlp11_secret_key_simple_t,so_enc_start);
      obj_len = sizeof(tlp11_secret_key_simple_t);
      break;
    case tlp11_type_otp_key:
      plaintext = offsetof(tlp11_otp_secret_key_t,so_enc_start);
      obj_len = sizeof(tlp11_otp_secret_key_t);
      break;
    default:
      TL_LOG_E("wrapObject default error invalid input");
      result = E_TLAPI_INVALID_INPUT;
      break;
    }
    if (result != TLAPI_OK) {
      break;
    }
    so_object->len = SO_CONTAINER_OBJECT_MAX_LEN;
    so_object->changed = 0;
    object->header.changed = 0; // volatile, this field matters only in TL
    object->header.magic = MAGIC_OBJECT;
    object->header.version = (g_config.arg.firmwareVersion_major << 16) | g_config.arg.firmwareVersion_minor;

    //TL_LOG_I("plaintext: %d",plaintext);
    //TL_LOG_I("encLen: %d",obj_len-plaintext);
    //TL_LOG_I("obj_len: %d",obj_len);
    //TL_LOG_I("so_object->len: %d",so_object->len);
    //TL_LOG_I("object @ 0x%08X",object);
    //TL_LOG_I("so_object->value @ 0x%08X",so_object->value);
    //TL_LOG_I("Object len %d", so_object->len);
    result = CRYPTO_OP(tlApiWrapObjectExt(
        object,
        plaintext,
        obj_len-plaintext,
        (mcSoHeader_t *)(so_object->value),
        &(so_object->len),
        MC_SO_CONTEXT_TLT,
        lifetime,
        NULL,
        TLAPI_WRAP_DEFAULT));
    //TL_LOG_I("Object len %d", so_object->len);
    //TL_LOG_I("result: 0x%08X", result);

    if (IS_STUBBED(result)) {
      mcSoHeader_t *header=(mcSoHeader_t*)(so_object->value);
      TL_LOG_W("STUBBED: tlApiWrapObject");
      header->type = MC_SO_TYPE_REGULAR;
      header->version = 0;
      header->context = MC_SO_CONTEXT_TLT;
      header->plainLen = plaintext;
      header->encryptedLen = obj_len-plaintext;
      memscpy(so_object->value+sizeof(mcSoHeader_t),SO_CONTAINER_OBJECT_MAX_LEN-sizeof(mcSoHeader_t),object,obj_len);
      so_object->len=obj_len+sizeof(mcSoHeader_t);
      result = TLAPI_OK;
    }
    if (result == TLAPI_OK) {
      so_object->changed = 1;
    }

  } while (0);
  TL_LOG_I("wrapObject- 0x%08X",result);
  return result;
}

tlApiResult_t unwrapObject(
    const tlp11_container_object_so_t *container,
    tlp11_container_object_t *destination) {
  tlApiResult_t result = TLAPI_OK;
  size_t len = sizeof(tlp11_container_object_t);
  TL_LOG_I("unwrapObject+");

  if (!container || !destination) {
    TL_LOG_E("NULL parameter!");
    return E_TLAPI_INVALID_PARAMETER;
  }

  {
    result = CRYPTO_OP(tlApiUnwrapObjectExt(
        (mcSoHeader_t *)container->value,
        sizeof(container->value),
        destination,
        &len,
        TLAPI_UNWRAP_PERMIT_CONTEXT_MASK));
  }

  if (IS_STUBBED(result)) {
    memscpy(destination,sizeof(tlp11_container_object_t),container->value+sizeof(mcSoHeader_t),min(len,container->len-sizeof(mcSoHeader_t)));
    result = TLAPI_OK;
  }
  if (result == TLAPI_OK) {
    destination->header.changed = 0;
    if (destination->header.magic != MAGIC_OBJECT) {
      result = E_TLAPI_INVALID_INPUT;
    }
  }
  // check version
  if (((destination->header.version & 0xFFFF0000) >> 16) != g_config.arg.firmwareVersion_major) {
    TL_LOG_E("Version mismatch, TL cannot handle this object");
    result = E_TLAPI_INVALID_INPUT;
  }

  TL_LOG_I("unwrapObject- 0x%08X",result);
  return result;
}

tlApiResult_t wrapToken(
    tlp11_token_t *token,
    tlp11_token_so_t *so_token)
{
  tlApiResult_t result = TLAPI_OK;
  uint32_t plaintext = offsetof(tlp11_token_t,so_enc_start);

  TL_LOG_I("wrapToken+");
  if (token && so_token) {
    // wrap the token
    token->changed = 0;
    token->magic = MAGIC_TOKEN;
    so_token->len = SO_TOKEN_MAX_LEN;
    so_token->changed = 0;

    result = CRYPTO_OP(tlApiWrapObjectExt(
                token,
                plaintext,
                sizeof(tlp11_token_t)-plaintext,
                (mcSoHeader_t *)(so_token->value),
                &(so_token->len),
                MC_SO_CONTEXT_TLT,
                MC_SO_LIFETIME_PERMANENT,
                NULL,
                TLAPI_WRAP_DEFAULT));

    if (!IS_OK(result)) {
      TL_LOG_E("tlApiWrapObject: 0x%08X",result);
    }
    if (IS_STUBBED(result)) {
      mcSoHeader_t *header=(mcSoHeader_t*)(so_token->value);
      TL_LOG_I(" Stubbing");
      header->type = MC_SO_TYPE_REGULAR;
      header->version = 0;
      header->context = MC_SO_CONTEXT_TLT;
      header->lifetime = MC_SO_LIFETIME_PERMANENT;
      header->plainLen = plaintext;
      header->encryptedLen = sizeof(tlp11_token_t)-plaintext;
      memscpy(so_token->value+sizeof(mcSoHeader_t),SO_TOKEN_MAX_LEN-sizeof(mcSoHeader_t),token,sizeof(tlp11_token_t));
      so_token->len = sizeof(tlp11_token_t)+sizeof(mcSoHeader_t);
      result = TLAPI_OK;
      TL_LOG_I(" done");
    }
    if (result == TLAPI_OK) {
      so_token->changed = 1;
    }
  }
  TL_LOG_I("wrapToken- 0x%08X",result);
  return result;
}

tlApiResult_t unwrapToken(
    const tlp11_token_so_t *so_token,
    tlp11_token_t *token){
  tlApiResult_t result = TLAPI_OK;
  size_t token_len = sizeof(tlp11_token_t);

  TL_LOG_I("unwrapToken+");
  if (token && so_token) {
    // unwrap the token
    {
      result = CRYPTO_OP(tlApiUnwrapObjectExt(
          (mcSoHeader_t *)so_token->value,
          sizeof(so_token->value),
          token,
          &token_len,
          TLAPI_UNWRAP_DEFAULT));
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiUnwrapObject: 0x%08X",result);
    }
    if (IS_STUBBED(result)) {
      memscpy(token,sizeof(tlp11_token_t),so_token->value+sizeof(mcSoHeader_t),sizeof(tlp11_token_t));
      result = TLAPI_OK;
    }
    if (result == TLAPI_OK) {
      token->changed = 0;
      if (token->magic != MAGIC_TOKEN) {
        result = E_TLAPI_INVALID_INPUT;
      } else if (token->info.firmwareVersion.major != g_config.arg.firmwareVersion_major) {
        TL_LOG_E("Version mismatch, TL cannot handle this object");
        result = E_TLAPI_INVALID_INPUT;
      }
    }
  }
  TL_LOG_I("unwrapToken- 0x%08X",result);
  return result;
}

tlApiResult_t wrapSessionGroup(
    tlp11_session_group_t *group,
    tlp11_session_group_so_t *so_group)
{
  tlApiResult_t result = TLAPI_OK;
  uint32_t plaintext = offsetof(tlp11_session_group_t,so_enc_start);

  TL_LOG_I("wrapSessionGroup+");
  if (group && so_group) {
    // wrap the token
    so_group->len = SO_SESSION_GROUP_MAX_LEN;
    so_group->changed = 0;
    group->changed = 0;
    group->magic = MAGIC_SESSION;
    result = CRYPTO_OP(tlApiWrapObjectExt(
                group,
                plaintext,
                sizeof(tlp11_session_group_t)-plaintext,
                (mcSoHeader_t *)(so_group->value),
                &(so_group->len),
                MC_SO_CONTEXT_TLT,
                MC_SO_LIFETIME_POWERCYCLE,
                NULL,
                TLAPI_WRAP_DEFAULT));

    if (!IS_OK(result)) {
      TL_LOG_E("tlApiWrapObject: 0x%08X",result);
    }
    if (IS_STUBBED(result)) {
      mcSoHeader_t *header=(mcSoHeader_t*)(so_group->value);
      header->type = MC_SO_TYPE_REGULAR;
      header->version = 0;
      header->context = MC_SO_CONTEXT_TLT;
      header->lifetime = MC_SO_LIFETIME_POWERCYCLE;
      header->plainLen = plaintext;
      header->encryptedLen = sizeof(tlp11_session_group_t)-plaintext;
      memscpy(so_group->value+sizeof(mcSoHeader_t),SO_SESSION_GROUP_MAX_LEN-sizeof(mcSoHeader_t),group,sizeof(tlp11_session_group_t));
      so_group->len = sizeof(tlp11_session_group_t)+sizeof(mcSoHeader_t);
      result = TLAPI_OK;
    }
    if (result == TLAPI_OK) {
      so_group->changed = 1;
    }
  }
  TL_LOG_I("wrapSessionGroup- 0x%08X",result);
  return result;
}

/** UnWrap a group of sessions.
 */
tlApiResult_t unwrapSessionGroup(
    const tlp11_session_group_so_t *so_group,
    tlp11_session_group_t *group){
  tlApiResult_t result = TLAPI_OK;
  size_t group_len = sizeof(tlp11_session_group_t);

  TL_LOG_I("unwrapSessionGroup+");
  if (group && so_group) {
    // unwrap the token
    {
      result = CRYPTO_OP(tlApiUnwrapObjectExt(
          (mcSoHeader_t *) so_group->value,
          sizeof(so_group->value),
          group,
          &group_len,
          TLAPI_UNWRAP_DEFAULT));
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiUnwrapObject: 0x%08X",result);
    }
    if (IS_STUBBED(result)) {
      memscpy(group,sizeof(tlp11_session_group_t),so_group->value+sizeof(mcSoHeader_t),sizeof(tlp11_session_group_t));
      result = TLAPI_OK;
    }
    if (result == TLAPI_OK) {
      group->changed = 0;
      if (group->magic != MAGIC_SESSION) {
        result = E_TLAPI_INVALID_INPUT;
      }
    }
  }
  TL_LOG_I("unwrapSessionGroup- 0x%08X",result);
  return result;
}

tlApiResult_t wrapTime(
    const tlp11_lock_time_t *time,
    tlp11_lock_time_so_t *so_time)
{
  tlApiResult_t result = TLAPI_OK;
  uint32_t plaintext = sizeof(tlp11_lock_time_t);

  TL_LOG_I("wrapTime+");
  if (time && so_time) {
    TL_LOG_I("localLockTime=%llu",time->localLockTime);
    TL_LOG_I("remoteLockTime=%llu",time->remoteLockTime);
    // wrap the token
    so_time->len = SO_TIME_MAX_LEN;
    result = CRYPTO_OP(tlApiWrapObjectExt(
                time,
                plaintext,
                sizeof(tlp11_lock_time_t)-plaintext,
                (mcSoHeader_t *)(so_time->value),
                &(so_time->len),
                MC_SO_CONTEXT_TLT,
                MC_SO_LIFETIME_POWERCYCLE,
                NULL,
                TLAPI_WRAP_DEFAULT));

    if (!IS_OK(result)) {
      TL_LOG_E("tlApiWrapObject: 0x%08X",result);
    }
    if (IS_STUBBED(result)) {
      mcSoHeader_t *header=(mcSoHeader_t*)(so_time->value);
      header->type = MC_SO_TYPE_REGULAR;
      header->version = 0;
      header->context = MC_SO_CONTEXT_TLT;
      header->lifetime = MC_SO_LIFETIME_POWERCYCLE;
      header->plainLen = plaintext;
      header->encryptedLen = sizeof(uint64_t)-plaintext;
      memscpy(so_time->value+sizeof(mcSoHeader_t),SO_TIME_MAX_LEN-sizeof(mcSoHeader_t),time,sizeof(uint64_t));
      so_time->len = sizeof(uint64_t)+sizeof(mcSoHeader_t);
      result = TLAPI_OK;
    }
  }
  TL_LOG_I("wrapTime- 0x%08X",result);
  return result;
}


tlApiResult_t unwrapTime(
    const tlp11_lock_time_so_t *so_time,
    tlp11_lock_time_t *time)
{
  tlApiResult_t result = TLAPI_OK;
  size_t len = sizeof(tlp11_lock_time_t);

  TL_LOG_I("unwrapTime+");
  if (time && so_time) {
    // unwrap the token
    {
      result = CRYPTO_OP(tlApiUnwrapObjectExt(
          (mcSoHeader_t *)so_time->value,
          sizeof(so_time->value),
          time,
          &len,
          TLAPI_UNWRAP_DEFAULT));
    }
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiUnwrapObject: 0x%08X",result);
    }
    if (IS_STUBBED(result)) {
      memscpy(time,sizeof(tlp11_lock_time_t),so_time->value+sizeof(mcSoHeader_t),sizeof(uint64_t));
      result = TLAPI_OK;
    }
    TL_LOG_I("localLockTime=%llu",time->localLockTime);
    TL_LOG_I("remoteLockTime=%llu",time->remoteLockTime);
  }
  TL_LOG_I("unwrapTime- 0x%08X",result);
  return result;
}

CK_FLAGS unlockToken(tlp11_token_t *token)
{
  tlp11_lock_time_t time;
  CK_RV rv = CKR_OK;
  uint64_t now = 0;
  tlApiResult_t result = TLAPI_OK;
  uint32_t lockObjectExpired = 0;
  CK_FLAGS haveToWrap = 0; // we keep here the flags to restore
  TL_LOG_I("unlockToken+");
  do {
    if (!token) {
      TL_LOG_E("Bad argument");
      result = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    if (((token->info.flags & CKF_USER_PIN_LOCKED) == 0) &&
        ((token->info.flags & CKF_SO_PIN_LOCKED) == 0)) {
      // not locked!
      break;
    }
    if (token->locks.len == 0) {
      TL_LOG_W("Inconsistency: lock flag on, but no lock recorded! Removing lock");
      lockObjectExpired = 1;
      break;
    }
    // open the lock object
    result = unwrapTime(&token->locks,&time);
    if (!IS_OK(result)) {
      TL_LOG_I("unwrapTime: 0x%08X. Unlock the token",result);
      if (g_config.arg.unlockAtBoot) {
        //unlock the token, since the lock has expired
        lockObjectExpired = 1;
        break;
      } else {
        // update the lock to now
        memset(&token->locks,0,sizeof(token->locks));
        if ((token->info.flags & CKF_USER_PIN_LOCKED) != 0) {
          rv = lockToken(token,CKU_USER);
          if (rv != CKR_OK) {
            TL_LOG_E("Failed to update CKU_USER lock, unlocking for good");
            lockObjectExpired = 1;
            break;
          }
        }
        if ((token->info.flags & CKF_SO_PIN_LOCKED) != 0) {
          rv = lockToken(token,CKU_SO);
          if (rv != CKR_OK) {
            TL_LOG_E("Failed to update CKU_SO lock, unlocking for good");
            lockObjectExpired = 1;
            break;
          }
        }

      }
    }
    // lock happened in this boot session, check the time
    if (g_tsSource == TS_SOURCE_ILLEGAL) {
      TL_LOG_W("No support for timers, can only unlock at boot");
      break;
    }

    result = tlApiGetTimeStamp(&now,g_tsSource);
    if (!IS_OK(result)) {
      TL_LOG_E("Failed to get counter, cannot unlock");
      break;
    }
    TL_LOG_I("Now: %llu",now);

    if (token->info.flags & CKF_USER_PIN_LOCKED) {
      TL_LOG_I("Checking for CKU_USER. localUnlockTime=%lu, localLockTime=%llu, now=%llu",
            g_config.arg.localUnlockTime,
            time.localLockTime,
            now);
      if ((g_config.arg.localUnlockTime != UINT32_MAX) &&
          (((now < time.localLockTime) &&
           (((UINT64_MAX - time.localLockTime) + now) > g_config.arg.localUnlockTime)) ||
          (now - time.localLockTime > g_config.arg.localUnlockTime))) {
        TL_LOG_I("Unlocking local");
        // good to unlock local
        token->info.flags &= ~CKF_USER_PIN_LOCKED;
        time.localLockTime = 0;
        haveToWrap |= CKF_USER_PIN_LOCKED;
        token->loginAttempts = 0;
      }
    }
    if (token->info.flags & CKF_SO_PIN_LOCKED) {
      TL_LOG_I("Checking for CKU_SO. remoteUnlockTime=%lu, remoteLockTime=%llu, now=%llu",
            g_config.arg.remoteUnlockTime,
            time.localLockTime,
            now);
      if ((g_config.arg.remoteUnlockTime != UINT32_MAX) &&
          (((now < time.remoteLockTime) &&
           ((UINT64_MAX - time.remoteLockTime) + now > g_config.arg.remoteUnlockTime)) ||
          (now - time.remoteLockTime > g_config.arg.remoteUnlockTime))) {
        TL_LOG_I("Unlocking remote");
        // good to unlock remote
        token->info.flags &= ~CKF_SO_PIN_LOCKED;
        time.remoteLockTime = 0;
        haveToWrap |= CKF_SO_PIN_LOCKED;
        token->remoteLoginAttempts = 0;
      }
    }
    if (haveToWrap) {
      result = wrapTime(&time,&token->locks);
      if (!IS_OK(result)) {
        // nothing we can really do here, just keep it as it was and restore the lock
        TL_LOG_E("Failed to wrap the lock, restoring it");
        token->info.flags |= haveToWrap;
      }
      token->changed = 1;
    }

  } while(0);
  if (token && lockObjectExpired) {
    token->info.flags &= ~CKF_USER_PIN_LOCKED;
    token->info.flags &= ~CKF_SO_PIN_LOCKED;
    memset(&token->locks,0,sizeof(token->locks));
    token->changed = 1;
    TL_LOG_I("Flags=0x%08X",token->info.flags);
  }
  TL_LOG_I("unlockToken-");
  if (token)
    return token->info.flags;
  else
    return 0;
}

CK_RV lockToken(tlp11_token_t *token, CK_USER_TYPE user)
{
  tlp11_lock_time_t time;
  uint64_t now = 0;
  tlApiResult_t result = TLAPI_OK;
  CK_RV rv = CKR_OK;
  TL_LOG_I("lockToken+");
  do {
    if (!token || ((user != CKU_SO) && (user != CKU_USER))) {
      TL_LOG_E("Bad argument");
      result = E_TLAPI_INVALID_PARAMETER;
      break;
    }
    time.localLockTime=0;
    time.remoteLockTime=0;
    if (g_tsSource == TS_SOURCE_ILLEGAL) {
      TL_LOG_W("No support for timers");
      now = UINT64_MAX;
    } else {
      result = tlApiGetTimeStamp(&now,g_tsSource);
      if (!IS_OK(result)) {
        TL_LOG_E("Failed to get counter, assuming max!");
        now = UINT64_MAX;
      }
    }
    // anything meaningful in the lock?
    if (token->locks.len > 0) {
      result = unwrapTime(&token->locks,&time);
      if (!IS_OK(result)) {
        TL_LOG_E("unwrapTime: 0x%08X. Consider it unlocked",result);
        time.localLockTime = 0;
        time.remoteLockTime = 0;
        token->info.flags &= ~CKF_USER_PIN_LOCKED;
        token->info.flags &= ~CKF_SO_PIN_LOCKED;
        token->changed = 1;
      }
    }
    if (user == CKU_USER) {
      TL_LOG_I("localLockTime = %llu",now);
      time.localLockTime = now;
    } else {
      TL_LOG_I("remoteLockTime = %llu",now);
      time.remoteLockTime = now;
    }
    // proceed to wrap
    result = wrapTime(&time,&token->locks);
    if (!IS_OK(result)) {
      TL_LOG_E("wrapTime: 0x%08X. leave it untouched",result);
      rv = CKR_GENERAL_ERROR;
      break;
    }
    if (time.localLockTime > 0) {
      TL_LOG_I("Locking for CKU_USER");
      token->info.flags |= CKF_USER_PIN_LOCKED;
    }
    if (time.remoteLockTime > 0) {
      TL_LOG_I("Locking for CKU_SO");
      token->info.flags |= CKF_SO_PIN_LOCKED;
    }
    token->changed = 1;
  } while(0);
  TL_LOG_I("lockToken-: 0x%08X",rv);
  return rv;
}

CK_RV checkAccess(
    const tlp11_token_t *token,
    tlp11_session_group_t *group, // not const, auth info might change
    const uint32_t index,
    const uint32_t access
    ) {

  CK_RV result = CKR_OK;
  uint64_t now = 0;

  TL_LOG_I("checkAccess+");

  if (!token || !group)
    return CKR_GENERAL_ERROR;

  do {
    // paranoid, these point to inconsistency problems
    if ((token->info.flags & CKF_TOKEN_INITIALIZED) == 0) {
      // this should never happen!
      TL_LOG_E("Token not initialized!");
      result = CKR_GENERAL_ERROR;
      break;
    }

    // first, make sure that the session group belongs to the token
    if (group->tokenStamp != token->tokenStamp) {
      TL_LOG_E("Session group / Token mismatch");
      result = CKR_GENERAL_ERROR;
      break;
    }

    // update authentication status, since it might have expired
    if (g_tsSource != TS_SOURCE_ILLEGAL) {
      if (!IS_OK(tlApiGetTimeStamp(&now,g_tsSource))) {
        now = UINT64_MAX;
      }
      if ((group->authInfo.authEntity != CKU_NONE) &&
          (now - group->authInfo.lastAuthTime > token->authSpan) &&
          (token->authSpan > 0)) {
        TL_LOG_I("Authentication expired! authSpan=%lu, lastAuthTime:%llu, now:%llu",
            token->authSpan,
            group->authInfo.lastAuthTime,
            now);
        group->authInfo.authEntity = CKU_NONE;
        group->changed = 1;
      }
    }

    if ((group->authInfo.authEntity != CKU_NONE) &&
        (group->authInfo.usageNumLeft == 0) &&
        (token->usageNum > 0)) {
      TL_LOG_I("Number of authorised operations exceeded!");
      group->authInfo.authEntity = CKU_NONE;
      group->changed = 1;
    }

    // start checking access permissions
    if ((access & SESSION_ACCESS_RW) && // need RW access
        (access & SESSION_ACCESS_TOKEN) && // on a token object
        !(group->sessions[index].access & SESSION_ACCESS_RW) ) // but session is RO
    {
      TL_LOG_E("No RW access!");
      result = CKR_SESSION_READ_ONLY;
      break;
    }

    if ((access & SESSION_ACCESS_PRIVATE) && // want to access a private object
        (group->authInfo.authEntity == CKU_NONE)) // but user is not logged in
    {
      TL_LOG_E("No private access!");
      result = CKR_USER_NOT_LOGGED_IN;
      break;
    }

    if ((access & SESSION_ACCESS_RW) && // need RW access
        (access & SESSION_ACCESS_TOKEN) && // on a token object
        (group->authInfo.authEntity == CKU_NONE) ) // but user is not logged in
    {
      TL_LOG_E("No RW access!");
      result = CKR_USER_NOT_LOGGED_IN;
      break;
    }

    //TL_LOG_I("UsageNum: %d",token->usageNum);
    //TL_LOG_I("usageNumLeft: %d",group->authInfo.usageNumLeft);
    //TL_LOG_I("Access: 0x%08X",access);
    // counting access to private objects or general write access to token objects
    if (((access & SESSION_ACCESS_PRIVATE) ||
         ((access & SESSION_ACCESS_RW) && (access & SESSION_ACCESS_TOKEN))) &&
        (token->usageNum > 0) &&
        (group->authInfo.usageNumLeft > 0)){
      group->authInfo.usageNumLeft--;
      TL_LOG_I("Decreasing usageNumLeft to: %d",group->authInfo.usageNumLeft);
      group->changed = 1;
    }

  } while (0);

  TL_LOG_I("checkAccess- 0x%08X",result);
  return result;
}

CK_RV getSessionCount(
    const tlp11_session_group_t *group,
        uint32_t *sessionCount,
        uint32_t *rwSessionCount,
        uint32_t *index // returned, first empty one
    )
{
  if (!group || (!sessionCount && !rwSessionCount))
    return CKR_GENERAL_ERROR;
  if (sessionCount)
    (*sessionCount)=0;
  if (rwSessionCount)
    (*rwSessionCount)=0;
  if (index)
    (*index) = TOKEN_MAX_SESSION_COUNT;
  for (unsigned int i=0; i<TOKEN_MAX_SESSION_COUNT; i++) {
    if (group->sessions[i].sessionStamp != 0) {
      if (sessionCount)
        (*sessionCount)++;
      if (rwSessionCount && (group->sessions[i].access & SESSION_ACCESS_RW))
        (*rwSessionCount)++;
    } else if ((index) && (*index == TOKEN_MAX_SESSION_COUNT)) {
      *index = i;
    }
  }
  return CKR_OK;
}

CK_RV processTokenGroup(
    const tlp11_token_so_t *so_token,
    const tlp11_session_group_so_t *so_group,
    tlp11_token_t *token,
    tlp11_session_group_t *group
    ) {
  tlApiResult_t result = TLAPI_OK;

  if (!token || !group) {
    return CKR_GENERAL_ERROR;
  }

  if (so_token) {
    result = unwrapToken(so_token,token);
    if (!IS_OK(result)) {
      TL_LOG_E("unwrapToken: %d",result);
      return TL2CK(result);
    }
  }
  if (so_group) {
    result = unwrapSessionGroup(so_group,group);
    if (!IS_OK(result)) {
      TL_LOG_E("unwrapSessionGroup: %d",result);
      return TL2CK(result);
    }
  }

  if (token->tokenStamp != group->tokenStamp) {
    TL_LOG_E("Token and Group stamps don't match");
    return CKR_GENERAL_ERROR;
  }

  return CKR_OK;
}

CK_RV processTokenGroupSession(
    const tlp11_token_so_t *so_token,
    const tlp11_session_group_so_t *so_group,
    const uint32_t sessionId,
    tlp11_token_t *token,
    tlp11_session_group_t *group,
    uint32_t *index
    ) {
  CK_RV ret = CKR_OK;
  uint32_t idx;

  ret = processTokenGroup(
      so_token,
      so_group,
      token,
      group
      );
  if (ret != CKR_OK) {
    return ret;
  }

  idx = SESSION_HANDLE_TO_INDEX(sessionId);
  if (idx >= token->info.ulMaxSessionCount) {
    return CKR_SESSION_HANDLE_INVALID;
  }
  if (group->sessions[idx].sessionStamp == 0) {
    return CKR_SESSION_CLOSED;
  }

  if (index) {
    *index = idx;
    TL_LOG_I("Session %d, stamp: 0x%08X",idx,group->sessions[idx].sessionStamp);
  }

  return CKR_OK;
}

CK_RV processTokenGroupSessionObject(
    const tlp11_token_so_t *so_token,
    const tlp11_session_group_so_t *so_group,
    const uint32_t sessionId,
    const tlp11_container_object_so_t *so_object,
    tlp11_token_t *token,
    tlp11_session_group_t *group,
    uint32_t *index,
    tlp11_container_object_t *object
    ) {
  CK_RV ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;

  if (!object) {
    return CKR_GENERAL_ERROR;
  }

  ret = processTokenGroupSession(
      so_token,
      so_group,
      sessionId,
      token,
      group,
      index
      );
  if (ret != CKR_OK) {
    return ret;
  }

  if (so_object) {
    result = unwrapObject(so_object,object);
    if (!IS_OK(result)) {
      TL_LOG_E("unwrapObject: 0x%08X",result);
      return TL2CK(result);
    }
  }

  if (object->storage.token) {
    if (object->header.stamp != token->tokenStamp) {
      TL_LOG_E("Object stamp doesn't match token stamp: 0x%08X vs 0x%08X",
          object->header.stamp,
          token->tokenStamp);
      return CKR_OBJECT_HANDLE_INVALID;
    }
  } else { // session object
    ret = CKR_OBJECT_HANDLE_INVALID;
    for(unsigned int i=0; i<token->info.ulMaxSessionCount; i++) {
      TL_LOG_I("Checking session stamp for index %d: 0x%08X",i,group->sessions[i].sessionStamp);
      if (object->header.stamp == group->sessions[i].sessionStamp) {
        ret = CKR_OK;
        break;
      }
    }
    if (ret != CKR_OK) {
      TL_LOG_E("Object stamp doesn't match any session stamp: 0x%08X",object->header.stamp);
      return ret;
    }
  }
  return CKR_OK;

}

/*lint -e{715} key and mechanisms not referenced*/
CK_RV mechanismIsAllowed(CK_MECHANISM_TYPE mechanism, const tlp11_key_t *key)
{
  CK_RV ret = CKR_OK;
  TL_LOG_I("mechanismIsAllowed+");
  // stubbed now, we do not restrict to the list
#if 0
  do {
    bool bFound = FALSE;
    if (!key) {
      ret = CKR_GENERAL_ERROR;
      break;
    }
    for (int i=0; i<MAX_ALLOWED_MECHANISMS; i++) {
      if (key->allowed_mechanisms[i] == mechanism) {
        bFound = TRUE;
        break;
      }
    }
    if (!bFound)
      ret = CKR_MECHANISM_INVALID;
  }while(0);
#endif
  TL_LOG_I("mechanismIsAllowed-: 0x%08X",ret);
  return ret;
}

#ifndef DEVLIB
void dumpHex(const char *tag, const uint8_t *buffer, unsigned int len) {
#ifdef TL_LOG_GLOBAL
  char output[256];
  memset(output,0,sizeof(output));
  TL_LOG_I("dumpHex %s (%d bytes @ 0x%08X)",tag,len,buffer);
  int j=0;
  for (unsigned int i=0; i<len; i++) {
    snprintf(output+j*3, 4, "%02X ", buffer[i]);
    *(output+j*3+3)=0;
    if (j == 15) {
      TL_LOG_I("%s",output);
      j=0;
    } else {
      j++;
    }
  }
  if (j>0 && j<15) {
    TL_LOG_I("%s",output);
  }
#endif
}
#endif

void fillTokenInfo(CK_TOKEN_INFO *info) {
  TL_LOG_I("fillTokenInfo+");
  size_t lenTmp = 0;
  do {
    if (!info) {
      break;
    }
    memset(info->manufacturerID,' ',TOKEN_MANUFACTURER_ID_LEN);
    lenTmp = strlen((const char *)g_config.arg.manufacturerID);
    memscpy(info->manufacturerID,
        TOKEN_MANUFACTURER_ID_LEN,
        g_config.arg.manufacturerID,
        min(lenTmp,TOKEN_MANUFACTURER_ID_LEN));
    memset(info->model,' ',TOKEN_MODEL_LEN);
    lenTmp = strlen((const char *)g_config.arg.model);
    memscpy(info->model,
        TOKEN_MODEL_LEN,
        g_config.arg.model,
        min(lenTmp,TOKEN_MODEL_LEN));
    memset(info->serialNumber,' ',TOKEN_SERIAL_NUM_LEN);
    lenTmp = strlen((const char *)g_config.arg.serialNumber);
    memscpy(info->serialNumber,
        TOKEN_SERIAL_NUM_LEN,
        g_config.arg.serialNumber,
        min(lenTmp,TOKEN_SERIAL_NUM_LEN));
    info->flags = g_config.arg.flags;
    info->ulMaxSessionCount = min(g_config.arg.ulMaxSessionCount,TOKEN_MAX_SESSION_COUNT);
    info->ulSessionCount = CK_UNAVAILABLE_INFORMATION;
    info->ulMaxRwSessionCount = min(g_config.arg.ulMaxRwSessionCount,info->ulMaxSessionCount);
    info->ulRwSessionCount = CK_UNAVAILABLE_INFORMATION;
    info->ulTotalPublicMemory = CK_UNAVAILABLE_INFORMATION;
    info->ulFreePublicMemory = CK_UNAVAILABLE_INFORMATION;
    info->ulTotalPrivateMemory = CK_UNAVAILABLE_INFORMATION;
    info->ulFreePrivateMemory = CK_UNAVAILABLE_INFORMATION;
    info->hardwareVersion.major = g_config.arg.hardwareVersion_major;
    info->hardwareVersion.minor = g_config.arg.hardwareVersion_minor;
    info->firmwareVersion.major = g_config.arg.firmwareVersion_major;
    info->firmwareVersion.minor = g_config.arg.firmwareVersion_minor;
    info->ulMaxPinLen = g_config.arg.ulMaxPinLen;
    info->ulMinPinLen = g_config.arg.ulMinPinLen;
    if (g_tsSource == TS_SOURCE_SECURE_RTC) {
      TL_LOG_I("CKF_CLOCK_ON_TOKEN available");
      info->flags |= CKF_CLOCK_ON_TOKEN;
    }
  }while(0);
  TL_LOG_I("fillTokenInfo-");
}

CK_RV completeWrapping(
    tlApiCrSession_t sessionHandle,
    tlp11_container_object_t *key,
    uint8_t *wrappedData,
    size_t *wrappedLen)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint8_t *src = NULL;
  uint32_t src_len = 0;

  TL_LOG_I("completeWrapping+");

  if (!key || !wrappedData || !wrappedLen || (*wrappedLen == 0))
    return CKR_GENERAL_ERROR;

  do {
    if (IS_TYPE(key,tlp11_type_secret_key)) {
      // we can write straight to the key
      if (IS_TYPE(key, tlp11_type_otp_key)) {
        src_len = key->otp.len;
        src = key->otp.value;
      } else {
        src_len = key->secret_key_simple.len;
        src = key->secret_key_simple.value;
      }
      result = CRYPTO_OP(tlApiCipherDoFinal(sessionHandle,src,src_len,wrappedData,wrappedLen));
      if (IS_STUBBED(result)) {
        *wrappedLen = min(src_len,*wrappedLen);
        memscpy(wrappedData,*wrappedLen,src,*wrappedLen);
        result = TLAPI_OK;
      }
      if (result == E_TLAPI_BUFFER_TOO_SMALL) {
        TL_LOG_E("Buffer too small, return error");
        *wrappedLen=WRAPPED_DATA_MAX_LEN;
        rv = CKR_BUFFER_TOO_SMALL;
        break;
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
        rv = TL2CK(result);
        break;
      }
    } else if (IS_TYPE(key,tlp11_type_rsa_private_key)) {
      // we need a temporary buffer into which decrypt the ASN.1 representation of the key
      // max size we accept:
      /*
      PrivateKeyInfo ::= SEQUENCE {                             --> 4 bytes preamble
         version Version,                                       --> 3 bytes, max 2+4=6
         privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,     --> 15 bytes
         privateKey PrivateKey:
           SEQUENCE {                                           --> 4 bytes preamble
            version           Version,                          --> max 6 bytes
            modulus           INTEGER,  -- n                    --> max 257+4
            publicExponent    INTEGER,  -- e                    --> max 2+4
            privateExponent   INTEGER,  -- d                    --> max 257+4
            prime1            INTEGER,  -- p                    --> max 129+3
            prime2            INTEGER,  -- q                    --> max 129+3
            exponent1         INTEGER,  -- d mod (p-1)          --> max 129+3
            exponent2         INTEGER,  -- d mod (q-1)          --> max 129+3
            coefficient       INTEGER,  -- (inverse of q) mod p --> max 129+3

            grand total: 1223
       */
      uint8_t asnBuf[1223] = {0};
      size_t asnBufLen = sizeof(asnBuf);
      // encode the content
      result = DER_encode_PrivateKeyInfo(&key->rsa_private_key,asnBuf,&asnBufLen);
      if (!IS_OK(result)) {
        TL_LOG_E("DER_encode_PrivateKeyInfo: 0x%08X",result);
        rv = TL2CK(result);
        break;
      }
      result = CRYPTO_OP(tlApiCipherDoFinal(sessionHandle,asnBuf,asnBufLen,wrappedData,wrappedLen));
      if (IS_STUBBED(result)) {
        *wrappedLen = min(asnBufLen,*wrappedLen);
        memscpy(wrappedData,*wrappedLen,asnBuf,*wrappedLen);
        result = TLAPI_OK;
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
        rv = TL2CK(result);
        break;
      }
    }
    //DUMP_HEX("wrappedData",wrappedData,*wrappedLen);
  } while(0);
  TL_LOG_I("completeWrapping- 0x%08X",rv);
  return rv;
}

CK_RV completeUnwrapping(
    tlApiCrSession_t sessionHandle,
    tlp11_container_object_t *newKey,
    const uint8_t *wrappedData,
    const size_t wrappedLen)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint8_t *dst = NULL;
  uint32_t *dst_len = NULL;

  TL_LOG_I("completeUnwrapping+");

  if (!newKey || !wrappedData)
    return CKR_GENERAL_ERROR;

  do {
    if (IS_TYPE(newKey,tlp11_type_secret_key)) {
      // we can write straight to the key
      uint8_t decBuffer[SECRET_KEY_MAX_LEN+AES_BLOCK_SIZE] = {0};
      size_t decBufferLen = sizeof(decBuffer);
      if (IS_TYPE(newKey, tlp11_type_otp_key)) {
        newKey->otp.len=sizeof(newKey->otp.value);
        dst_len=&(newKey->otp.len);
        dst=newKey->otp.value;
      } else {
        newKey->secret_key_simple.len=sizeof(newKey->secret_key_simple.value);
        dst_len=&newKey->secret_key_simple.len;
        dst=newKey->secret_key_simple.value;
      }
      // now decrypt
      //DUMP_HEX("wrappedData",wrappedData,wrappedLen);
      /*lint -e64 type mismatch (ptrs to nominal): uint32_t* vs size_t**/
      result = CRYPTO_OP(tlApiCipherDoFinal(sessionHandle,wrappedData,wrappedLen,decBuffer,&decBufferLen));
      if (IS_STUBBED(result)) {
        *dst_len = min(*dst_len,wrappedLen);
        memscpy(dst,*dst_len,wrappedData,*dst_len);
        result = TLAPI_OK;
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
        rv = TL2CK(result);
        break;
      }
      // check if the key was large enough to hold the decrypted value
      if (*dst_len < decBufferLen) {
        TL_LOG_E("Destination too small for decrypted key: %d vs %d",
            *dst_len,decBufferLen);
        rv = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      *dst_len = decBufferLen;
      memscpy(dst,decBufferLen,decBuffer,decBufferLen);
      result = computeChecksum(
        newKey->secret_key.check_value,
        dst,
        *dst_len,
        newKey->header.object_type);
      if (!IS_OK(result)) {
        TL_LOG_E(" Failed to compute checksum");
        rv = TL2CK(result);
        break;
      }
    } else if (IS_TYPE(newKey,tlp11_type_rsa_private_key)) {
      // we need a temporary buffer into which decrypt the ASN.1 representation of the key
      // max size we accept:
      /*
      PrivateKeyInfo ::= SEQUENCE {                             --> 4 bytes preamble
         version Version,                                       --> 3 bytes, max 2+4=6
         privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,     --> 15 bytes
         privateKey PrivateKey:
           SEQUENCE {                                           --> 4 bytes preamble
            version           Version,                          --> max 6 bytes
            modulus           INTEGER,  -- n                    --> max 257+4
            publicExponent    INTEGER,  -- e                    --> max 2+4
            privateExponent   INTEGER,  -- d                    --> max 257+4
            prime1            INTEGER,  -- p                    --> max 129+3
            prime2            INTEGER,  -- q                    --> max 129+3
            exponent1         INTEGER,  -- d mod (p-1)          --> max 129+3
            exponent2         INTEGER,  -- d mod (q-1)          --> max 129+3
            coefficient       INTEGER,  -- (inverse of q) mod p --> max 129+3

            grand total: 1223
       */
      uint8_t buffer[1223] = {0};
      size_t bufferLen = sizeof(buffer);
      // now decrypt
      result = CRYPTO_OP(tlApiCipherDoFinal(sessionHandle,wrappedData,wrappedLen,buffer,&bufferLen));
      if (IS_STUBBED(result)) {
        bufferLen = min(bufferLen,wrappedLen);
        memscpy(buffer,bufferLen,wrappedData,bufferLen);
        result = TLAPI_OK;
      }
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiCipherDoFinal: 0x%08X",result);
        rv = TL2CK(result);
        break;
      }
      // decode the content
      newKey->rsa_private_key.modulus_len=MODULUS_MAX_LEN;
      newKey->rsa_private_key.public_exponent_len=PUBLIC_EXPONENT_LEN;
      newKey->rsa_private_key.exponent_len=PRIVATE_EXPONENT_MAX_LEN;
      newKey->rsa_private_key.prime1_len=PRIME1_MAX_LEN;
      newKey->rsa_private_key.prime2_len=PRIME2_MAX_LEN;
      newKey->rsa_private_key.exponent1_len=EXPONENT1_MAX_LEN;
      newKey->rsa_private_key.exponent2_len=EXPONENT2_MAX_LEN;
      newKey->rsa_private_key.coefficient_len=COEFFICIENT_MAX_LEN;
      result = DER_decode_PrivateKeyInfo(
          buffer,
          bufferLen,
          &newKey->rsa_private_key
          );
      if (!IS_OK(result)) {
        TL_LOG_E("DER_decode_PrivateKeyInfo: 0x%08X",result);
        rv = TL2CK(result);
        break;
      }
      rv = cryptoValidateRSAPrivateKey(&newKey->rsa_private_key);
      if (rv != CKR_OK) {
        TL_LOG_E("New key failed validation!");
        break;
      }
    }
  } while(0);
  TL_LOG_I("completeUnwrapping- 0x%08X",rv);
  return rv;
}

tlApiResult_t tlApiGetTimeStamp(
    timestamp_t *ts,
    tsSource_t  source
)
{
  return E_TLAPI_UNKNOWN_FUNCTION;
}

/* timesafe_memcmp() should perform a memcmp() in constant time (proportional to len) */
/* Function returns 1 if the strings are different and 0 if the strings are the same  */
/* This prevents any information about difference between two buffers being leaked.   */

int timesafe_memcmp(const void* ptr1, const void* ptr2, size_t len)
{
    uint8_t rc = 0;
    uint8_t* p1 = (uint8_t*) ptr1;
    uint8_t* p2 = (uint8_t*) ptr2;

    while (len > 0)
    {
        /* *p1 XOR * p2 will be zero if and only if *p1 == *p2                  */
        /* The OR means that once rc becomes non-zero, it will remain that way. */
        rc = rc | (*p1 ^ *p2);

        /* Setup for next iteration                                             */
        len--;
        p1++;
        p2++;
    }

    /* Reduce rc to 1 if rc is non-zero and 0 if rc is zero.                    */
    rc = (rc >> 4) | (rc & 0x0F);
    rc = (rc >> 2) | (rc & 0x03);
    rc = (rc >> 1) | (rc & 0x01);

    return (int) rc;
}
