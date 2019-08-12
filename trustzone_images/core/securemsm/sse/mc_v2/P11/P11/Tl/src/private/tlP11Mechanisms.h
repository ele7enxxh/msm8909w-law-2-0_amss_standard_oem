/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_MECHANISMS_H
#define TLP11_MECHANISMS_H

#include "tlP11Types.h"

#define DECLARE_CIPHER_INIT_FUNC(name) \
  CK_RV name(tlp11_session_t *session, \
      CK_MECHANISM_TYPE mech, \
      uint32_t operation, \
      const tlp11_mechanism_param_t *params, \
      const tlp11_container_object_t *key)

#define DECLARE_CIPHER_UPDATE_FUNC(name) \
  CK_RV name(tlp11_session_t *session, \
      CK_MECHANISM_TYPE mech, \
      uint8_t *dataIn, \
      size_t dataInLen, \
      uint8_t *dataOut, \
      size_t *dataOutLen)

#define DECLARE_CIPHER_FINAL_FUNC(name) \
  CK_RV name(tlp11_session_t *session, \
      CK_MECHANISM_TYPE mech, \
      uint8_t *dataIn, \
      size_t dataInLen, \
      uint8_t *dataOut, \
      size_t *dataOutLen)

#define DECLARE_DIGEST_INIT_FUNC(name) \
  CK_RV name(tlp11_session_t *session, \
      CK_MECHANISM_TYPE mech)

#define DECLARE_DIGEST_UPDATE_FUNC(name) \
  CK_RV name(tlp11_session_t *session, \
      CK_MECHANISM_TYPE mech, \
      uint8_t *dataIn, \
      size_t dataInLen)

#define DECLARE_DIGEST_FINAL_FUNC(name) \
  CK_RV name(tlp11_session_t *session, \
      CK_MECHANISM_TYPE mech, \
      uint8_t *dataIn, \
      size_t dataInLen, \
      uint8_t *dataOut, \
      size_t *dataOutLen)

#define DECLARE_SIGN_VERIFY_INIT_FUNC(name) \
  CK_RV name(tlp11_session_t *session, \
      CK_MECHANISM_TYPE mech, \
      uint32_t operation, \
      tlp11_mechanism_param_t *params, \
      tlp11_container_object_t *key)

#define DECLARE_SIGN_VERIFY_UPDATE_FUNC(name) \
  CK_RV name(tlp11_session_t *session, \
      CK_MECHANISM_TYPE mech, \
      uint8_t *dataIn, \
      size_t dataInLen)

#define DECLARE_SIGN_VERIFY_FINAL_FUNC(name) \
  CK_RV name(tlp11_session_t *session, \
      CK_MECHANISM_TYPE mech, \
      uint8_t *dataIn, \
      size_t dataInLen, \
      uint8_t *dataOut, \
      size_t *dataOutLen)

#define DECLARE_GENERATE_KEY_FUNC(name) \
  CK_RV name(tlp11_mechanism_param_t *params, \
      tlp11_container_object_t *key, \
      const tlp11_token_t *token, \
      tlp11_session_group_t *group, \
      const uint32_t index)

#define DECLARE_GENERATE_KEY_PAIR_FUNC(name) \
  CK_RV name(tlp11_mechanism_param_t *params, \
      tlp11_container_object_t *pub, \
      tlp11_container_object_t *prv)

#define DECLARE_WRAP_FUNC(name) \
  CK_RV name(CK_MECHANISM_TYPE mech, \
      tlp11_mechanism_param_t *params, \
      tlp11_container_object_t *wrappingKey, \
      tlp11_container_object_t *key, \
      uint8_t *wrappedData, \
      size_t *wrappedLen)

#define DECLARE_UNWRAP_FUNC(name) \
  CK_RV name(CK_MECHANISM_TYPE mech, \
      const tlp11_mechanism_param_t *params, \
      tlp11_container_object_t *unwrappingKey, \
      tlp11_container_object_t *newKey, \
      const uint8_t *wrappedData, \
      size_t wrappedLen)

#define DECLARE_DERIVE_FUNC(name) \
  CK_RV name(CK_MECHANISM_TYPE mech, \
      tlp11_mechanism_param_t *params, \
      tlp11_container_object_t *baseKey, \
      tlp11_container_object_t *newKey, \
      const tlp11_token_t *token, \
      tlp11_session_group_t *group, \
      const uint32_t index)


typedef struct {
  DECLARE_CIPHER_INIT_FUNC((*cipher_init));
  DECLARE_CIPHER_UPDATE_FUNC((*cipher_update));
  DECLARE_CIPHER_UPDATE_FUNC((*cipher_final));
  DECLARE_DIGEST_INIT_FUNC((*digest_init));
  DECLARE_DIGEST_UPDATE_FUNC((*digest_update));
  DECLARE_DIGEST_FINAL_FUNC((*digest_final));
  DECLARE_SIGN_VERIFY_INIT_FUNC((*sign_verify_init));
  DECLARE_SIGN_VERIFY_UPDATE_FUNC((*sign_verify_update));
  DECLARE_SIGN_VERIFY_FINAL_FUNC((*sign_verify_final));
  DECLARE_GENERATE_KEY_FUNC((*generate));
  DECLARE_GENERATE_KEY_PAIR_FUNC((*generate_pair));
  DECLARE_WRAP_FUNC((*wrap));
  DECLARE_UNWRAP_FUNC((*unwrap));
  DECLARE_DERIVE_FUNC((*derive));
} mechanism_func_t;

typedef struct {
  CK_MECHANISM_TYPE mechanism;
  CK_MECHANISM_INFO info;
  uint32_t compatibleType;
  mechanism_func_t functions;
} tlp11_mechanism_t;

extern tlp11_mechanism_t mechanisms[];

CK_RV getMechanismData(
    CK_MECHANISM_TYPE mechanism,
    tlp11_mechanism_t *pMech);

#endif /* TLP11_MECH_PARAMS_H */
