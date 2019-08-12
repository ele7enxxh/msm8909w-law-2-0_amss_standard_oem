/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_API_PARAMS_H_
#define TLP11_API_PARAMS_H_

#include "tci.h"
#include "tlP11Token.h"
#include "tlP11Objects.h"
#include "tlP11Types.h"
#include "tlP11So.h"


#define SALT_MAX_LEN 32
#define MECHANISM_LIST_LEN    100
#define ATTRIBUTE_ARRAY_LEN   32

/**
 * Termination codes
 */
#define EXIT_ERROR                      ((uint32_t)(-1))

/*
 * @endcond
 */

/******************************************************************************/
/* (1) C_Initialize */
/******************************************************************************/
/** Initialize
 * Empty call, used to activate the container
 *  */
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
} cmdInitialize_t;
typedef struct {
  tciResponseHeader_t  header;         /**< Command header */
} rspInitialize_t;

/******************************************************************************/
/* (7) C_GetTokenInfo */
/******************************************************************************/
/** GetTokenInfo */
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
} cmdGetTokenInfo_t;
/** GetTokenInfo Response */
typedef struct {
  CK_TOKEN_INFO *pInfo;
} params_rspGetTokenInfo_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  params_rspGetTokenInfo_t    params;
} rspGetTokenInfo_t;

/******************************************************************************/
/* (8) C_GetMechanismList */
/******************************************************************************/
/** GetMechanismList command */
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
} cmdGetMechanismList_t;
/** GetMechanismList response */
typedef struct {
  CK_MECHANISM_TYPE mechanisms[MECHANISM_LIST_LEN];
  uint32_t mechanisms_len;
} params_rspGetMechanismList_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  params_rspGetMechanismList_t    params;
} rspGetMechanismList_t;

/******************************************************************************/
/* (9) C_GetMechanismInfo */
/******************************************************************************/
/** GetMechanismInfo command */
typedef struct {
  CK_MECHANISM_TYPE mechanism;
} params_cmdGetMechanismInfo_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdGetMechanismInfo_t params;
} cmdGetMechanismInfo_t;
/** GetMechanismInfo response */
typedef struct {
  CK_MECHANISM_INFO info;
} params_rspGetMechanismInfo_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdGetMechanismInfo_t)];
  params_rspGetMechanismInfo_t    params;
} rspGetMechanismInfo_t;

/******************************************************************************/
/* (10) C_InitToken */
/******************************************************************************/
/** InitToken
 * This command is used to initialise a token bound to the local user only.
 */
typedef struct {
  UTF8CHAR label[TOKEN_LABEL_LEN];
  UTF8CHAR pin[TOKEN_PIN_LEN];
  uint32_t pinLen;
} params_cmdInitToken_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdInitToken_t params;
} cmdInitToken_t;
/** InitToken response */
typedef struct {
  tlp11_token_so_t so_token;
} params_rspInitToken_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdInitToken_t)];
  params_rspInitToken_t params;
} rspInitToken_t;

/******************************************************************************/
/* (101) InitToken_SK_Init */
/******************************************************************************/
/** InitToken
 * This command is used to initialise a token bound to the local user only,
 * using the Secure Keypad.
 */
typedef struct {
  UTF8CHAR label[TOKEN_LABEL_LEN];
} params_cmdInitTokenSKInit_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdInitTokenSKInit_t params;
} cmdInitTokenSKInit_t;
/** InitToken response */
typedef struct {
  tlp11_keypad_event_t event;
} params_rspInitTokenSKInit_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdInitTokenSKInit_t)];
  params_rspInitTokenSKInit_t params;
} rspInitTokenSKInit_t;

/******************************************************************************/
/* (102) C_InitToken_SK_Continue */
/******************************************************************************/
/** InitToken
 * This command is used to initialise a token bound to the local user only.
 */
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
} cmdInitTokenSKContinue_t;
/** InitToken response */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_keypad_event_t event;
} params_rspInitTokenSKContinue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  params_rspInitTokenSKContinue_t params;
} rspInitTokenSKContinue_t;

/******************************************************************************/
/* (103) SK_Cancel */
/******************************************************************************/
/** InitToken
 * This command is used to initialise a token bound to the local user only.
 */
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
} cmdSKCancel_t;
/** InitToken response */
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
} rspSKCancel_t;

/******************************************************************************/
/* (104) InitTokenSO */
/******************************************************************************/
/** UnwrapKey */
typedef struct {
  UTF8CHAR label[TOKEN_LABEL_LEN];
  uint8_t wrappedData[SO_CONTAINER_MAX_LEN];
  uint32_t wrappedDataLen;
} params_cmdInitTokenSO_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdInitTokenSO_t params;
} cmdInitTokenSO_t;
/** WrapKey response */
typedef struct {
  tlp11_token_so_t so_token;
} params_rspInitTokenSO_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdInitTokenSO_t)];
  params_rspInitTokenSO_t params;
} rspInitTokenSO_t;

/******************************************************************************/
/* (11) C_InitPIN */
/******************************************************************************/
/** InitPIN */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  UTF8CHAR pin[TOKEN_PIN_LEN];
  uint32_t pinLen;
} params_cmdInitPIN_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdInitPIN_t params;
} cmdInitPIN_t;
/** InitPIN response */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
} params_rspInitPIN_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdInitPIN_t)];
  params_rspInitPIN_t params;
} rspInitPIN_t;

/******************************************************************************/
/* (111) C_InitPINSKInit */
/******************************************************************************/
/** InitPIN */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
} params_cmdInitPINSKInit_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdInitPINSKInit_t params;
} cmdInitPINSKInit_t;
/** InitPIN response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_keypad_event_t event;
} params_rspInitPINSKInit_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdInitPINSKInit_t)];
  params_rspInitPINSKInit_t params;
} rspInitPINSKInit_t;

/******************************************************************************/
/* (111) C_InitPINSKContinue */
/******************************************************************************/
/** InitPIN */
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
} cmdInitPINSKContinue_t;
/** InitPIN response */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_keypad_event_t event;
} params_rspInitPINSKContinue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  params_rspInitPINSKContinue_t params;
} rspInitPINSKContinue_t;

/******************************************************************************/
/* (12) C_SetPIN */
/******************************************************************************/
/** SetPIN */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  UTF8CHAR oldPin[TOKEN_PIN_LEN];
  uint32_t oldPinLen;
  UTF8CHAR newPin[TOKEN_PIN_LEN];
  uint32_t newPinLen;
} params_cmdSetPIN_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdSetPIN_t params;
} cmdSetPIN_t;
/** SetPIN response */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
} params_rspSetPIN_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdSetPIN_t)];
  params_rspSetPIN_t params;
} rspSetPIN_t;

/******************************************************************************/
/* (121) C_SetPINSKInit */
/******************************************************************************/
/** SetPIN */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
} params_cmdSetPINSKInit_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdSetPINSKInit_t params;
} cmdSetPINSKInit_t;
/** SetPIN response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_keypad_event_t event;
} params_rspSetPINSKInit_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdSetPINSKInit_t)];
  params_rspSetPINSKInit_t params;
} rspSetPINSKInit_t;

/******************************************************************************/
/* (121) C_SetPINSKContinue */
/******************************************************************************/
/** SetPIN */
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
} cmdSetPINSKContinue_t;
/** SetPIN response */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_keypad_event_t event;
} params_rspSetPINSKContinue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  params_rspSetPINSKContinue_t params;
} rspSetPINSKContinue_t;


/******************************************************************************/
/* (13) C_OpenSession */
/******************************************************************************/
/** OpenSession */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t flags;
} params_cmdOpenSession_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdOpenSession_t params;
} cmdOpenSession_t;
/** OpenSession response */
typedef struct {
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
} params_rspOpenSession_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdOpenSession_t)];
  params_rspOpenSession_t params;
} rspOpenSession_t;

/******************************************************************************/
/* (14) C_CloseSession */
/******************************************************************************/
/** CloseSession */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
} params_cmdCloseSession_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdCloseSession_t params;
} cmdCloseSession_t;
/** CloseSession response */
typedef struct {
  tlp11_session_group_so_t so_group; /**< Filled ONLY if there are open sessions */
} params_rspCloseSession_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdCloseSession_t)];
  params_rspCloseSession_t params;
} rspCloseSession_t;

/******************************************************************************/
/* (19) C_Login */
/******************************************************************************/
/** Login.
 * Used for both local and remote user
 */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  CK_USER_TYPE user;
  char pin[TOKEN_RESPONSE_MAX_LEN];
  uint32_t  pinLen;                   /**< if 0, login happens on the secure keypad*/
} params_cmdLogin_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdLogin_t params;
} cmdLogin_t;
/** Login Response */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
} params_rspLogin_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdLogin_t)];
  params_rspLogin_t params;
} rspLogin_t;

/******************************************************************************/
/* (191) Login_SK_Init */
/******************************************************************************/
/** Login on Secure Keypad for Local User.
 * Note: started, no other command can be executed till is has been completed
 */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
} params_cmdLoginSKInit_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdLoginSKInit_t params;
} cmdLoginSKInit_t;
/** Login Response */
typedef struct {
  tlp11_keypad_event_t event;
} params_rspLoginSKInit_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdLoginSKInit_t)];
  params_rspLoginSKInit_t params;
} rspLoginSKInit_t;

/******************************************************************************/
/* (191) Login_SK_Continue */
/******************************************************************************/
/** Login on Secure Keypad for Local User.
 */
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
} cmdLoginSKContinue_t;
/** Login Response */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  tlp11_keypad_event_t event;
} params_rspLoginSKContinue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  params_rspLoginSKContinue_t params;
} rspLoginSKContinue_t;

/******************************************************************************/
/* (20) C_Logout */
/******************************************************************************/
/** Logout.
 * Used for both local and remote user
 */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
} params_cmdLogout_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdLogout_t params;
} cmdLogout_t;
/** Logout Response */
typedef struct {
  tlp11_session_group_so_t so_group;
} params_rspLogout_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdLogout_t)];
  params_rspLogout_t params;
} rspLogout_t;

/******************************************************************************/
/* (21) C_CreateObject */
/******************************************************************************/
/** CreateObject
 * Create an object from a plaintext input
 */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_t object;
} params_cmdCreateObject_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdCreateObject_t params;
} cmdCreateObject_t;
/** CreateObject Response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_object;
} params_rspCreateObject_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdCreateObject_t)];
  params_rspCreateObject_t params;
} rspCreateObject_t;

/******************************************************************************/
/* (22) C_CopyObject */
/******************************************************************************/
/** CopyObject
 * Copy an object. Change the values of CKA_TOKEN, CKA_PRIVATE and
 * CKA_MODIFIABLE
 */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_object;
  uint32_t token;
  uint32_t prvt;
  uint32_t modifiable;
} params_cmdCopyObject_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdCopyObject_t params;
} cmdCopyObject_t;
/** CopyObject Response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_object;
} params_rspCopyObject_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdCopyObject_t)];
  params_rspCopyObject_t params;
} rspCopyObject_t;

/******************************************************************************/
/* (250) C_GetULongAttributeValue */
/******************************************************************************/
/** GetAttributeValue */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_object;
  CK_ATTRIBUTE_TYPE attribute;
} params_cmdGetULongAttributeValue_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdGetULongAttributeValue_t params;
} cmdGetULongAttributeValue_t;
/** GetAttributeValue response */
typedef struct {
  tlp11_session_group_so_t so_group;
  CK_ULONG value;
} params_rspGetULongAttributeValue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdGetULongAttributeValue_t)];
  params_rspGetULongAttributeValue_t params;
} rspGetULongAttributeValue_t;
/******************************************************************************/
/* (251) C_GetArrayAttributeValue */
/******************************************************************************/
/** GetArrayAttributeValue */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_object;
  CK_ATTRIBUTE_TYPE attribute;
  uint32_t len;
} params_cmdGetArrayAttributeValue_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdGetArrayAttributeValue_t params;
} cmdGetArrayAttributeValue_t;
/** GetArrayAttributeValue response */
typedef struct {
  tlp11_session_group_so_t so_group;
  uint8_t value[FIELD_MAX_LEN];
  uint32_t len;
} params_rspGetArrayAttributeValue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdGetArrayAttributeValue_t)];
  params_rspGetArrayAttributeValue_t params;
} rspGetArrayAttributeValue_t;

/******************************************************************************/
/* (253) C_GetTokenChallenge */
/******************************************************************************/
/** GetTokenChallenge */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
} params_cmdGetTokenChallenge_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdGetTokenChallenge_t params;
} cmdGetTokenChallenge_t;
/** GetTokenChallenge response */
typedef struct {
  tlp11_session_group_so_t so_group;
  uint8_t value[TOKEN_CHALLENGE_WRAPPED_LEN];
} params_rspGetTokenChallenge_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdGetTokenChallenge_t)];
  params_rspGetTokenChallenge_t params;
} rspGetTokenChallenge_t;

/******************************************************************************/
/* (26) C_SetAttributeValue */
/******************************************************************************/
/** SetAttributeValue */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_object;
  CK_ATTRIBUTE_TYPE attribute;
  uint32_t value;
} params_cmdSetAttributeValue_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdSetAttributeValue_t params;
} cmdSetAttributeValue_t;
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_object;
} params_rspSetAttributeValue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdSetAttributeValue_t)];
  params_rspSetAttributeValue_t params;
} rspSetAttributeValue_t;

/******************************************************************************/
/* (261) C_SetArrayAttributeValue */
/******************************************************************************/
/** SetArrayAttributeValue */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_object;
  CK_ATTRIBUTE_TYPE attribute;
  uint8_t value[FIELD_MAX_LEN];
  uint32_t len;
} params_cmdSetArrayAttributeValue_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdSetArrayAttributeValue_t params;
} cmdSetArrayAttributeValue_t;
/** SetArrayAttributeValue response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_object;
} params_rspSetArrayAttributeValue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdSetArrayAttributeValue_t)];
  params_rspSetArrayAttributeValue_t params;
} rspSetArrayAttributeValue_t;

/******************************************************************************/
/* (262) C_SetTemplateAttributeValue */
/******************************************************************************/
/** SetTemplateAttributeValue command, used by FindObject */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_object;
  CK_ATTRIBUTE_TYPE attribute;
  tlp11_attribute_template_t value;
} params_cmdSetTemplateAttributeValue_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdSetTemplateAttributeValue_t params;
} cmdSetTemplateAttributeValue_t;
/** SetTemplateAttributeValue response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_object;
} params_rspSetTemplateAttributeValue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdSetTemplateAttributeValue_t)];
  params_rspSetTemplateAttributeValue_t params;
} rspSetTemplateAttributeValue_t;

/******************************************************************************/
/* (30) C_EncryptInit */
/******************************************************************************/
/** CryptoInit */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_object;
  CK_MECHANISM_TYPE mechanism;
  tlp11_mechanism_param_t param;
  uint32_t operation; /**< flag indicating the operation: CKF_ENCRYPT, ... */
} params_cmdCryptoInit_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdCryptoInit_t params;
} cmdCryptoInit_t;
/** CryptoInit response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_object; /**< possibly changed if OTP key */
} params_rspCryptoInit_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdCryptoInit_t)];
  params_rspCryptoInit_t params;
} rspCryptoInit_t;

/******************************************************************************/
/* (301) CryptoAbort */
/******************************************************************************/
/** CryptoAbort */
typedef struct {
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
} params_cmdCryptoAbort_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdCryptoAbort_t params;
} cmdCryptoAbort_t;
/** CryptoAbort response */
typedef struct {
  tlp11_session_group_so_t so_group;
} params_rspCryptoAbort_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdCryptoAbort_t)];
  params_rspCryptoAbort_t params;
} rspCryptoAbort_t;

/******************************************************************************/
/* (32) C_CryptoUpdate */
/******************************************************************************/
/** CryptoUpdate */
typedef struct {
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  uint8_t *data;
  uint32_t dataLen;
  uint32_t dataOutLen;
  uint32_t doFinal;
} params_cmdCryptoUpdate_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdCryptoUpdate_t params;
} cmdCryptoUpdate_t;
/** CryptoUpdate response */
typedef struct {
  tlp11_session_group_so_t so_group;
  uint8_t *data;
  size_t dataLen;
} params_rspCryptoUpdate_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdCryptoUpdate_t)];
  params_rspCryptoUpdate_t params;
} rspCryptoUpdate_t;

/******************************************************************************/
/* (41) C_DigestKey (DigestUpdate, input from a key) */
/******************************************************************************/
/** DigestKey */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_object;
} params_cmdDigestKey_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdDigestKey_t params;
} cmdDigestKey_t;
/** DigestKey response */
typedef struct {
  tlp11_session_group_so_t so_group;
} params_rspDigestKey_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdDigestKey_t)];
  params_rspDigestKey_t params;
} rspDigestKey_t;

/******************************************************************************/
/* (59) C_GenerateKey */
/******************************************************************************/
/** GenerateKey */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_t object;
  CK_MECHANISM_TYPE mechanism;
  tlp11_mechanism_param_t param; /**< change in-place for output parameters, e.g. IV*/
} params_cmdGenerateKey_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdGenerateKey_t params;
} cmdGenerateKey_t;
/** cmdGenerateKey response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_object;
} params_rspGenerateKey_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdGenerateKey_t)];
  params_rspGenerateKey_t params;
} rspGenerateKey_t;

/******************************************************************************/
/* (591) C_GenerateKeySKInit */
/******************************************************************************/
/** GenerateKeySKInit */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_t object;
  CK_MECHANISM_TYPE mechanism;
  tlp11_mechanism_param_t param;
} params_cmdGenerateKeySKInit_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdGenerateKeySKInit_t params;
} cmdGenerateKeySKInit_t;
/** cmdGenerateKeySKInit response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_keypad_event_t event;
} params_rspGenerateKeySKInit_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdGenerateKeySKInit_t)];
  params_rspGenerateKeySKInit_t params;
} rspGenerateKeySKInit_t;

/******************************************************************************/
/* (592) C_GenerateKeySKContinue */
/******************************************************************************/
/** GenerateKeySKContinue */
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
} cmdGenerateKeySKContinue_t;
/** cmdGenerateKeySKContinue response */
typedef struct {
  tlp11_container_object_so_t so_object;
  tlp11_keypad_event_t event;
} params_rspGenerateKeySKContinue_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  params_rspGenerateKeySKContinue_t params;
} rspGenerateKeySKContinue_t;

/******************************************************************************/
/* (60) C_GenerateKeyPair */
/******************************************************************************/
/** GenerateKeyPair */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_t pub;
  tlp11_container_object_t prv;
  CK_MECHANISM_TYPE mechanism;
  tlp11_mechanism_param_t param;
} params_cmdGenerateKeyPair_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdGenerateKeyPair_t params;
} cmdGenerateKeyPair_t;
/** GenerateKeyPair response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_public;
  tlp11_container_object_so_t so_private;
} params_rspGenerateKeyPair_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdGenerateKeyPair_t)];
  params_rspGenerateKeyPair_t params;
} rspGenerateKeyPair_t;

/******************************************************************************/
/* (61) C_WrapKey */
/******************************************************************************/
/** WrapKey */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_wrapping_key;
  tlp11_container_object_so_t so_key;
  CK_MECHANISM_TYPE mechanism;
  tlp11_mechanism_param_t param;
  size_t wrappedDataLen;
} params_cmdWrapKey_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdWrapKey_t params;
} cmdWrapKey_t;
/** WrapKey response */
typedef struct {
  tlp11_session_group_so_t so_group;
  uint8_t wrappedData[WRAPPED_DATA_MAX_LEN];
  size_t wrappedDataLen;
} params_rspWrapKey_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdWrapKey_t)];
  params_rspWrapKey_t params;
} rspWrapKey_t;

/******************************************************************************/
/* (62) C_UnwrapKey */
/******************************************************************************/
/** UnwrapKey */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_unwrapping_key;
  tlp11_container_object_t key;
  CK_MECHANISM_TYPE mechanism;
  tlp11_mechanism_param_t param;
  uint8_t wrappedData[WRAPPED_DATA_MAX_LEN];
  uint32_t wrappedDataLen;
} params_cmdUnwrapKey_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdUnwrapKey_t params;
} cmdUnwrapKey_t;
/** WrapKey response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_key;
} params_rspUnwrapKey_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdUnwrapKey_t)];
  params_rspUnwrapKey_t params;
} rspUnwrapKey_t;


/******************************************************************************/
/* (63) C_DeriveKey */
/******************************************************************************/
/** DeriveKey */
typedef struct {
  tlp11_token_so_t so_token;
  tlp11_session_group_so_t so_group;
  uint32_t sessionId;
  tlp11_container_object_so_t so_base_key;
  tlp11_container_object_t key;
  CK_MECHANISM_TYPE mechanism;
  tlp11_mechanism_param_t param;
} params_cmdDeriveKey_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdDeriveKey_t params;
} cmdDeriveKey_t;
/** DeriveKey response */
typedef struct {
  tlp11_session_group_so_t so_group;
  tlp11_container_object_so_t so_key;
} params_rspDeriveKey_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdDeriveKey_t)];
  params_rspDeriveKey_t params;
} rspDeriveKey_t;

/******************************************************************************/
/* (65) C_GenerateRandom */
/******************************************************************************/
/** DeriveKey */
typedef struct {
  uint32_t dataLen;
} params_cmdGenerateRandom_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdGenerateRandom_t params;
} cmdGenerateRandom_t;
/** DeriveKey response */
typedef struct {
  uint8_t *data;
} params_rspGenerateRandom_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdGenerateRandom_t)];
  params_rspGenerateRandom_t params;
} rspGenerateRandom_t;

#endif // TLP11_API_PARAMS_H_
