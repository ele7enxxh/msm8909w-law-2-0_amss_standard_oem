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
#include "tlP11Validation.h"
#include "tlLog.h"
#include "tlP11Glob.h"

/** Process a CreateObject command
 * The object is passed in as clear, and returned as secured
 * The token to with it has to be associated is passed as secure
 * Only the following types of objects can be created in this way:
 * X.509 certificates, RSA Public Keys, Data objects.
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdCreateObject(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = SESSION_ACCESS_RW;
  uint32_t index;
  tlp11_container_object_t* container_object_0_ptr = (tlp11_container_object_t*)tci_message->rspCreateObject.params.so_object.value;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdCreateObject.params.so_token.value;

  do {
    TL_LOG_I("processCmdCreateObject+");

    ret = processTokenGroupSession(
        &tci_message->cmdCreateObject.params.so_token,
        &tci_message->cmdCreateObject.params.so_group,
        tci_message->cmdCreateObject.params.sessionId,
        g_token_0_ptr,
        &g_session_group_0,
        &index
        );

    if (ret != CKR_OK) {
      break;
    }

    // copy input locally, since we will modify it
    memscpy(container_object_0_ptr,SO_CONTAINER_OBJECT_MAX_LEN,&tci_message->cmdCreateObject.params.object,sizeof(tlp11_container_object_t));

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
    switch(container_object_0_ptr->object.cls) {
    case CKO_DATA:
      TL_LOG_I("--> data object");
      container_object_0_ptr->header.object_type = tlp11_type_data;
      ret = validateData(&container_object_0_ptr->data);
      break;
    case CKO_PUBLIC_KEY:
        TL_LOG_I("--> public key");
        if (container_object_0_ptr->key.key_type == CKK_RSA) {
          container_object_0_ptr->header.object_type = tlp11_type_rsa_public_key;
          if (container_object_0_ptr->key.local != 0) {
            // CKA_LOCAL = 1 only for keys generated locally
            TL_LOG_E("CKA_LOCAL = 1 only for keys generated locally");
            ret = CKR_ATTRIBUTE_READ_ONLY;
            break;
          }
          if (container_object_0_ptr->key.key_gen_mechanism != CK_UNAVAILABLE_INFORMATION) {
            TL_LOG_E("CKA_KEY_GEN_MECHANISM != CK_UNAVAILABLE_INFORMATION: 0x%08X",container_object_0_ptr->key.key_gen_mechanism);
            // must NOT be specified
            ret = CKR_ATTRIBUTE_READ_ONLY;
            break;
          }
          if (container_object_0_ptr->public_key.trusted == 1) {
            // can be set to true only by the SO
            TL_LOG_E("CKA_TRUSTED = 1 can be set only on objects imported as Secure Objects");
            ret = CKR_TEMPLATE_INCONSISTENT;
            break;
          }
          container_object_0_ptr->rsa_public_key.modulus_bits = container_object_0_ptr->rsa_public_key.modulus_len*8;
          ret = validateRSAPublicKey(&container_object_0_ptr->rsa_public_key);
          if (ret == CKR_OK)
            ret = validateSecureTemplateKeyPair(&container_object_0_ptr->public_key,NULL,0);
        } else {
          // no other public keys supported
          ret = CKR_TEMPLATE_INCONSISTENT;
        }
      break;
    case CKO_CERTIFICATE:
        TL_LOG_I("--> certificate");
        if (container_object_0_ptr->certificate.category == CKC_X_509) {
          container_object_0_ptr->header.object_type = tlp11_type_certificate_x509;
          if (container_object_0_ptr->certificate.trusted == 1) {
            // can be set to true only by the SO
            TL_LOG_E("CKA_TRUSTED = 1 can be set only on objects imported as Secure Objects");
            ret = CKR_TEMPLATE_INCONSISTENT;
            break;
          }
          if ((container_object_0_ptr->certificate.check_value[0] == 0) &&
              (container_object_0_ptr->certificate.check_value[1] == 0) &&
              (container_object_0_ptr->certificate.check_value[2] == 0)) {
            // overwrite it
            result = computeChecksum(
                container_object_0_ptr->certificate.check_value,
                container_object_0_ptr->x509.value,
                container_object_0_ptr->x509.value_len,
                tlp11_type_certificate_x509
                );
            if (!IS_OK(result)) {
              TL_LOG_E("Failed to to compute checksum");
              ret = TL2CK(result);
              break;
            }
          }
          ret = validateX509Certificate(&container_object_0_ptr->x509);
        } else {
          // no other certificate supported
          ret = CKR_TEMPLATE_INCONSISTENT;
        }
      break;
    default:
      TL_LOG_E("unsupported class: 0x%08X",container_object_0_ptr->object.cls);
      ret = CKR_ARGUMENTS_BAD;
      break;
    }
    if (ret != CKR_OK) {
      break;
    }

    // object is good to go

    if (container_object_0_ptr->storage.token) {
      container_object_0_ptr->header.stamp = g_token_0_ptr->tokenStamp;
    } else {
      container_object_0_ptr->header.stamp = g_session_group_0.sessions[index].sessionStamp;
    }

    // wrap the object
    result = wrapObject(
        container_object_0_ptr,
        &tci_message->rspCreateObject.params.so_object
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
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspCreateObject.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

	TL_LOG_I("processCmdCreateObject- ret:0x%08X",ret);
  return ret;
}
