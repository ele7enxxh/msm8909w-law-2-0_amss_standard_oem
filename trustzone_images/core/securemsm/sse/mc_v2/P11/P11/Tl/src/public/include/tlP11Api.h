/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_API_H_
#define TLP11_API_H_

#include "tci.h"
#include "tlP11Token.h"
#include "tlP11Objects.h"
#include "tlP11Types.h"
#include "tlP11ApiParams.h"

#define LOG_SIZE 8192

/**
 * @cond
 * Command ID's for communication Trustlet Connector -> Trustlet.
 */

#define TLP11_CMD_INITIALIZE                1
#define TLP11_CMD_GET_TOKEN_INFO            7
#define TLP11_CMD_GET_MECHANISM_LIST        8
#define TLP11_CMD_GET_MECHANISM_INFO        9
#define TLP11_CMD_INIT_TOKEN               10
#define TLP11_CMD_INIT_TOKEN_SK_INIT       101
#define TLP11_CMD_INIT_TOKEN_SK_CONTINUE   102
#define TLP11_CMD_SK_CANCEL                103
#define TLP11_CMD_INIT_TOKEN_SO            104
#define TLP11_CMD_INIT_PIN                 11
#define TLP11_CMD_INIT_PIN_SK_INIT         111
#define TLP11_CMD_INIT_PIN_SK_CONTINUE     112
#define TLP11_CMD_SET_PIN                  12
#define TLP11_CMD_SET_PIN_SK_INIT          121
#define TLP11_CMD_SET_PIN_SK_CONTINUE      122
#define TLP11_CMD_OPEN_SESSION             13
#define TLP11_CMD_CLOSE_SESSION            14
#define TLP11_CMD_LOGIN                    19
#define TLP11_CMD_LOGIN_SK_INIT            191
#define TLP11_CMD_LOGIN_SK_CONTINUE        192
#define TLP11_CMD_LOGOUT                   20
#define TLP11_CMD_CREATE_OBJECT            21
#define TLP11_CMD_COPY_OBJECT              22
#define TLP11_CMD_GET_ULONG_ATTRIBUTE_VALUE      250
#define TLP11_CMD_GET_ARRAY_ATTRIBUTE_VALUE      251
#define TLP11_CMD_GET_TOKEN_CHALLENGE   253
#define TLP11_CMD_SET_ATTRIBUTE_VALUE      26
#define TLP11_CMD_SET_ARRAY_ATTRIBUTE_VALUE      261
#define TLP11_CMD_SET_TEMPLATE_ATTRIBUTE_VALUE   262

#define TLP11_CMD_CRYPTO_INIT              30
#define TLP11_CMD_CRYPTO_ABORT             301
#define TLP11_CMD_CRYPTO_UPDATE            32
#define TLP11_CMD_DIGEST_KEY               41

#define TLP11_CMD_GENERATE_KEY              59
#define TLP11_CMD_GENERATE_KEY_SK_INIT      591
#define TLP11_CMD_GENERATE_KEY_SK_CONTINUE  592
#define TLP11_CMD_GENERATE_KEY_PAIR         60
#define TLP11_CMD_WRAP_KEY                  61
#define TLP11_CMD_UNWRAP_KEY                62
#define TLP11_CMD_DERIVE_KEY                63
#define TLP11_CMD_GENERATE_RANDOM           65

#define MECHANISM_LIST_LEN    100
#define ATTRIBUTE_ARRAY_LEN   32

/**
 * Termination codes
 */
#define EXIT_ERROR                      ((uint32_t)(-1))

/*
 * @endcond
 */


/**
 * TCI message data.
 */
typedef union {
  tciCommandHeader_t        commandHeader;
  tciResponseHeader_t       responseHeader;

  cmdGetTokenInfo_t         cmdGetTokenInfo;
  rspGetTokenInfo_t         rspGetTokenInfo;

  cmdGetMechanismList_t     cmdGetMechanismList;
  rspGetMechanismList_t     rspGetMechanismList;

  cmdGetMechanismInfo_t     cmdGetMechanismInfo;
  rspGetMechanismInfo_t     rspGetMechanismInfo;

  cmdInitToken_t            cmdInitToken;
  rspInitToken_t            rspInitToken;

  cmdInitTokenSKInit_t      cmdInitTokenSKInit;
  rspInitTokenSKInit_t      rspInitTokenSKInit;

  cmdInitTokenSKContinue_t  cmdInitTokenSKContinue;
  rspInitTokenSKContinue_t  rspInitTokenSKContinue;

  cmdInitTokenSO_t          cmdInitTokenSO;
  rspInitTokenSO_t          rspInitTokenSO;

  cmdSKCancel_t             cmdSKCancel;
  rspSKCancel_t             rspSKCancel;

  cmdInitPIN_t              cmdInitPIN;
  rspInitPIN_t              rspInitPIN;

  cmdInitPINSKInit_t        cmdInitPINSKInit;
  rspInitPINSKInit_t        rspInitPINSKInit;

  cmdInitPINSKContinue_t    cmdInitPINSKContinue;
  rspInitPINSKContinue_t    rspInitPINSKContinue;

  cmdSetPIN_t               cmdSetPIN;
  rspSetPIN_t               rspSetPIN;

  cmdSetPINSKInit_t         cmdSetPINSKInit;
  rspSetPINSKInit_t         rspSetPINSKInit;

  cmdSetPINSKContinue_t     cmdSetPINSKContinue;
  rspSetPINSKContinue_t     rspSetPINSKContinue;

  cmdOpenSession_t          cmdOpenSession;
  rspOpenSession_t          rspOpenSession;

  cmdCloseSession_t         cmdCloseSession;
  rspCloseSession_t         rspCloseSession;

  cmdLogin_t                cmdLogin;
  rspLogin_t                rspLogin;

  cmdLoginSKInit_t          cmdLoginSKInit;
  rspLoginSKInit_t          rspLoginSKInit;

  cmdLoginSKContinue_t      cmdLoginSKContinue;
  rspLoginSKContinue_t      rspLoginSKContinue;

  cmdLogout_t               cmdLogout;
  rspLogout_t               rspLogout;

  cmdCreateObject_t         cmdCreateObject;
  rspCreateObject_t         rspCreateObject;

  cmdCopyObject_t           cmdCopyObject;
  rspCopyObject_t           rspCopyObject;

  cmdGetArrayAttributeValue_t    cmdGetArrayAttributeValue;
  rspGetArrayAttributeValue_t    rspGetArrayAttributeValue;

  cmdGetULongAttributeValue_t    cmdGetULongAttributeValue;
  rspGetULongAttributeValue_t    rspGetULongAttributeValue;

  cmdGetTokenChallenge_t    cmdGetTokenChallenge;
  rspGetTokenChallenge_t    rspGetTokenChallenge;

  cmdSetAttributeValue_t    cmdSetAttributeValue;
  rspSetAttributeValue_t    rspSetAttributeValue;

  cmdSetArrayAttributeValue_t    cmdSetArrayAttributeValue;
  rspSetArrayAttributeValue_t    rspSetArrayAttributeValue;

  cmdSetTemplateAttributeValue_t    cmdSetTemplateAttributeValue;
  rspSetTemplateAttributeValue_t    rspSetTemplateAttributeValue;

  cmdCryptoInit_t           cmdCryptoInit;
  rspCryptoInit_t           rspCryptoInit;

  cmdCryptoAbort_t          cmdCryptoAbort;
  rspCryptoAbort_t          rspCryptoAbort;

  cmdCryptoUpdate_t         cmdCryptoUpdate;
  rspCryptoUpdate_t         rspCryptoUpdate;

  cmdDigestKey_t            cmdDigestKey;
  rspDigestKey_t            rspDigestKey;

  cmdGenerateKey_t          cmdGenerateKey;
  rspGenerateKey_t          rspGenerateKey;

  cmdGenerateKeySKInit_t    cmdGenerateKeySKInit;
  rspGenerateKeySKInit_t    rspGenerateKeySKInit;

  cmdGenerateKeySKContinue_t    cmdGenerateKeySKContinue;
  rspGenerateKeySKContinue_t    rspGenerateKeySKContinue;

  cmdGenerateKeyPair_t      cmdGenerateKeyPair;
  rspGenerateKeyPair_t      rspGenerateKeyPair;

  cmdUnwrapKey_t            cmdUnwrapKey;
  rspUnwrapKey_t            rspUnwrapKey;

  cmdWrapKey_t            cmdWrapKey;
  rspWrapKey_t            rspWrapKey;

  cmdDeriveKey_t            cmdDeriveKey;
  rspDeriveKey_t            rspDeriveKey;

  cmdGenerateRandom_t       cmdGenerateRandom;
  rspGenerateRandom_t       rspGenerateRandom;

} tciMessage_t;

/**
 * Overall TCI structure.
 */
typedef struct {
    tciMessage_t    message;                    /**< TCI message */
    uint8_t         log[LOG_SIZE];       /**< size = len + log_len*/
}tci_t;

 
/**
 * Trustlet UUID.
 */
#define TL_P11_UUID { { 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } }

#endif // TLP11_API_H_
