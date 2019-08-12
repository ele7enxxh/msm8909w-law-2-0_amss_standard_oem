/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlP11Utils.h"
#include "tlP11Validation.h"
#include "tlLog.h"
#include "tlP11Glob.h"
#include "tlP11Mechanisms.h"

/** Process a GenerateKey command
 * The object is passed in as clear, and returned as secured
 * Only symmetric secret keys can be created with this function
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGenerateKey(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = SESSION_ACCESS_RW;
  tlp11_mechanism_t mech;
  uint32_t index;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdGenerateKey.params.so_token.value;

  do {
    TL_LOG_I("processCmdGenerateKey+");

    ret = processTokenGroupSession(
        &tci_message->cmdGenerateKey.params.so_token,
        &tci_message->cmdGenerateKey.params.so_group,
        tci_message->cmdGenerateKey.params.sessionId,
        g_token_0_ptr,
        &g_session_group_0,
        &index
        );

    if (ret != CKR_OK) {
      break;
    }

    // copy input locally, since we will modify it  tlp11_container_object_t
    tlp11_container_object_t *container_object_0_ptr = &tci_message->cmdGenerateKey.params.object;

    // check access, before inspecting the object
    if (container_object_0_ptr->storage.prvt) {
      access |= SESSION_ACCESS_PRIVATE;
    }
    if (container_object_0_ptr->storage.token) {
      access |= SESSION_ACCESS_TOKEN;
    }
    ret = checkAccess(g_token_0_ptr,&g_session_group_0,index,access);
    if (ret != CKR_OK) {
      break;
    }

    // check passed object and sanitize it
    // we do not rely on header.object_type being properly set
    if ((container_object_0_ptr->object.cls != CKO_SECRET_KEY) &&
        (container_object_0_ptr->object.cls != CKO_OTP_KEY)){
      TL_LOG_E(" Can only create CKO_SECRET_KEY or CKO_OTP_KEY");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    ret = getMechanismData(
      tci_message->cmdGenerateKey.params.mechanism,
      &mech);
    if (ret != CKR_OK) {
      TL_LOG_E(" Failed to get mechanism info");
      break;
    }

    if ((mech.info.flags & CKF_GENERATE) == 0) {
      TL_LOG_E(" Mechanism does not support requested operation 0x%08X",
          CKF_GENERATE);
      ret = CKR_MECHANISM_INVALID;
      break;
    }
    if ((g_token_0_ptr->enabledFunctions & CKF_GENERATE) == 0) {
      TL_LOG_E(" Token does not support requested operation 0x%08X",
          CKF_GENERATE);
      ret = CKR_FUNCTION_NOT_SUPPORTED;
      break;
    }

    // key will be local, ignore input on this
    container_object_0_ptr->key.local = 1;

    // if this is a DES key write the length, impled my mech
    if (container_object_0_ptr->key.key_type == CKK_DES) {
      container_object_0_ptr->secret_key_simple.len = DES_KEY_LEN;
    } else if (container_object_0_ptr->key.key_type == CKK_DES3) {
      container_object_0_ptr->secret_key_simple.len = DES3_KEY_LEN;
    }

    if (container_object_0_ptr->object.cls == CKO_SECRET_KEY) {
      if ((container_object_0_ptr->secret_key_simple.len*8 < mech.info.ulMinKeySize) ||
          (container_object_0_ptr->secret_key_simple.len*8 > mech.info.ulMaxKeySize)) {
        TL_LOG_E(" Key length outside supported range: %d vs %d-%d",
            container_object_0_ptr->secret_key_simple.len,
            mech.info.ulMinKeySize,mech.info.ulMaxKeySize);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    } else if ((container_object_0_ptr->otp.len*8 < mech.info.ulMinKeySize) ||
               (container_object_0_ptr->otp.len*8 > mech.info.ulMaxKeySize)) {
      TL_LOG_E(" Key length outside supported range: %d vs %d-%d",
          container_object_0_ptr->secret_key_simple.len,
          mech.info.ulMinKeySize,mech.info.ulMaxKeySize);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    // perform secret checks
    if (container_object_0_ptr->key.key_gen_mechanism != CK_UNAVAILABLE_INFORMATION) {
      TL_LOG_E("Inconsistent KEY_GEN_MECHANISM: 0x%08X",
          container_object_0_ptr->key.key_gen_mechanism);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    container_object_0_ptr->key.key_gen_mechanism = mech.mechanism;
    if (container_object_0_ptr->secret_key.trusted == 1) {
      // can be set to true only by the SO
      TL_LOG_E("CKA_TRUSTED = 1 can be set only on objects imported as Secure Objects");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (mech.functions.generate == NULL) {
      TL_LOG_E("No function associated with mechanism");
      ret = CKR_GENERAL_ERROR;
      break;
    }
    // now check the key type
    if (container_object_0_ptr->key.key_type == CKK_AES) {
      // AES
      TL_LOG_I(" -->AES key");
      container_object_0_ptr->header.object_type = tlp11_type_aes_key;
      ret = validateAESSecretKey(&container_object_0_ptr->secret_key_simple);
    } else if (container_object_0_ptr->key.key_type == CKK_GENERIC_SECRET){
      TL_LOG_I(" -->GENERIC_SECRET key");
      container_object_0_ptr->header.object_type = tlp11_type_generic_key;
      ret = validateGenericSecretKey(&container_object_0_ptr->secret_key_simple);
    } else if (container_object_0_ptr->key.key_type == CKK_DES){
      TL_LOG_I(" -->DES key");
      container_object_0_ptr->header.object_type = tlp11_type_des_key;
      ret = validateDESSecretKey(&container_object_0_ptr->secret_key_simple);
    } else if (container_object_0_ptr->key.key_type == CKK_DES3){
      TL_LOG_I(" -->DES3 key");
      container_object_0_ptr->header.object_type = tlp11_type_des3_key;
      ret = validateDESSecretKey(&container_object_0_ptr->secret_key_simple);
    } else if (container_object_0_ptr->key.key_type == CKK_HOTP){
      TL_LOG_I(" -->HOTP key");
      container_object_0_ptr->header.object_type = tlp11_type_otp_key;
      ret = validateOTPSecretKey(&container_object_0_ptr->otp);
    } else {
      TL_LOG_I(" -->Unsupported now");
      ret = CKR_MECHANISM_INVALID;
      break;
    }
    if (ret != CKR_OK) {
      TL_LOG_E("Error validating key: 0x%08X",ret);
      break;
    }
    // ad-hoc: PBKDF2 is technically a key generation mechanism, but is in practice
    // a key derivation mechanism. We check it as such, with an ad-hoc case
    if ((mech.mechanism == CKM_PKCS5_PBKD2) ||
        (mech.mechanism == CKM_PKCS5_PBKD2_GENERIC_SECRET_KEY)) {
      ret = validateSecureTemplateSecretKey(&container_object_0_ptr->secret_key,CKF_DERIVE);
    } else {
      ret = validateSecureTemplateSecretKey(&container_object_0_ptr->secret_key,CKF_GENERATE);
    }
    if (ret != CKR_OK) {
      TL_LOG_E("Key doesn't respect Secure Template");
      break;
    }
    // are mechanism and key compatible?
    if ((!IS_TYPE(container_object_0_ptr,mech.compatibleType)) &&
        (!BELONGS_TO_TYPES(container_object_0_ptr,mech.compatibleType))){
      TL_LOG_E(" Key inconsistent with supplied mechanism");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    tci_message->cmdGenerateKey.params.param.header.changed = 0;
    ret = (*mech.functions.generate)(
        &tci_message->cmdGenerateKey.params.param,
        container_object_0_ptr,
        g_token_0_ptr,
        &g_session_group_0,
        index);
    if (ret != CKR_OK) {
      TL_LOG_E("Error generating key: 0x%08X",ret);
      break;
    }

    // object is good to go
    if (container_object_0_ptr->storage.token) {
      container_object_0_ptr->header.stamp = g_token_0_ptr->tokenStamp;
      TL_LOG_I("Associating token stamp: 0x%08X",g_token_0_ptr->tokenStamp);
    } else {
      container_object_0_ptr->header.stamp = g_session_group_0.sessions[index].sessionStamp;
      TL_LOG_I("Associating session(%d) stamp: 0x%08X",index,g_session_group_0.sessions[index].sessionStamp);
    }

    // wrap the object
    result = wrapObject(
        container_object_0_ptr,
        &tci_message->rspGenerateKey.params.so_object
        );

    if (!IS_OK(result)) {
      TL_LOG_E("wrapObject: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }

  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspGenerateKey.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

	TL_LOG_I("processCmdGenerateKey- ret:0x%08X",ret);
  return ret;
}
