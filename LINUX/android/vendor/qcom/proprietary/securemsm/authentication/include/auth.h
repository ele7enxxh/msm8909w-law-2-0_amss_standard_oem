/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/

#pragma once

#include "qso.h"

#ifdef __RVCT__
#pragma anon_unions
#endif

#define QC_USERNAME_MAX_LEN 128 /**< In line with FIDO UAFV1 */
#define QC_SEC_APP_NAME_LEN 32 /**< As in lkthread.h */
#define QC_APP_ID_MAX_LEN   512 /**< In line with FIDO UAFV1 */
#define QC_AES_IV_SIZE      16
#define QC_AUTH_NONCE       32
#define QC_SDTT_HASH_SIZE   32
#define QC_AUTH_ID_SIZE     32
#define QC_EXTENSION_DATA_MAX_LEN 4096
#define QC_UVT_VERSION_MAJOR    1
#define QC_UVT_VERSION_MINOR   0
#define QC_SD_VERSION_MAJOR    1
#define QC_SD_VERSION_MINOR    0
/**
 * This file is shared between HLOS and QSEE, so QSEE definitions cannot
 * be used directly.
 */

/** Size of a QSEE encapsulated message
 *
 * Note: the following MUST be kept in sync with QSEE Message definitions
 *
 */
#define QSEE_MESSAGE_HEADER_LEN 84
#define QSEE_MESSAGE_MAC_LEN    32

#define ENC_SIZE(x) ((x) + QSEE_MESSAGE_HEADER_LEN + QSEE_MESSAGE_MAC_LEN)

/** ECC keys */
#define QC_ECC_256_PRV_KEY_LEN 36 /**< 9 uint32 */

#pragma pack(push, auth, 1)

typedef enum {
  QC_AUTH_CODE_OK  = 0, /**< Authentication was ok */
  QC_AUTH_CODE_ERR = 1, /**< Generic error */
  QC_AUTH_CODE_CANCEL = 2, /**< Cancelled by user */
  QC_AUTH_CODE_MAX = QC_AUTH_CODE_CANCEL,
  QC_AUTH_CODE_NA = 0x7FFFFFFF
} qc_auth_code_t;

/** Authentication token
*/
typedef struct {
  struct {
    uint16_t major;
    uint16_t minor;
  } version;
  qc_auth_code_t result;
  uint64_t userId;
  uint64_t uptime;
  uint8_t nonce[QC_AUTH_NONCE]; /**< Sha256(finalChallenge) for FIDO */
  uint32_t extDataLength;
  uint8_t extData[QC_EXTENSION_DATA_MAX_LEN];
} qc_authentication_token_t;

typedef union {
  uint8_t value[QSO_SIZE(0,sizeof(qc_authentication_token_t))];
  qc_authentication_token_t token;
  struct {
    QSoHeader_t header;
    qc_authentication_token_t token;
    uint8_t tag[SW_AES_BLOCK_BYTE_LEN];
  } wrapped;
} qc_so_authentication_token_t;

#define QC_USER_VERIFICATION_TOKEN_LEN ENC_SIZE(sizeof(qc_authentication_token_t))

typedef struct {
  uint32_t messageLen;
  union {
    /** Secure UVT */
    uint8_t message[QC_USER_VERIFICATION_TOKEN_LEN];
    /** Non-secure UVT */
    struct {
      char authName[QC_SEC_APP_NAME_LEN]; /**< Non-secure authenticator name, \0 terminated */
      uint8_t authId[QC_AUTH_ID_SIZE]; /**< Non-secure authenticator ID */
      qc_authentication_token_t token;
    };
  };
} qc_user_verification_token_t;

/** Secure Display Token
 * SDT, content agnostic
 * */
typedef struct {
  struct {
    uint16_t major;
    uint16_t minor;
  } version;
  qc_auth_code_t result;
  struct {
    uint64_t start;
    uint64_t stop;
  } uptime;
  uint8_t nonce[QC_AUTH_NONCE]; /**< Sha256(finalChallenge) for FIDO */
} qc_secure_display_token_t;

/** Content on screen during a transaction confirmation
 * If displayed by SD this is verifiable since coming as part of an enc message
 * If displayed by HLOS, we just use it and hope for better
 *  */
typedef struct {
  char appId[QC_APP_ID_MAX_LEN+1];
  uint8_t hash[QC_SDTT_HASH_SIZE];
} qc_transaction_content_t;

/** secure display content */
typedef struct {
  qc_secure_display_token_t sdt;
  qc_transaction_content_t content;
} qc_secure_display_transaction_token_t;

/** Generic display content */
typedef union {
  uint8_t message[ENC_SIZE(sizeof(qc_secure_display_transaction_token_t))]; /**< SDTT */
  qc_transaction_content_t content; /**< plain from HLOS */
} qc_transaction_display_t;

/** Content on screen during an authentication
 * If displayed by SD this is verifiable since coming as part of an enc message
 * If displayed by HLOS, we just use it and hope for better
 *  */
typedef struct {
  char appId[QC_APP_ID_MAX_LEN+1];
} qc_auth_content_t;

/** secure display content */
typedef struct {
  qc_secure_display_token_t sdt;
  qc_auth_content_t content;
} qc_secure_display_auth_token_t;

/** Generic display content */
typedef union {
  uint8_t message[ENC_SIZE(sizeof(qc_secure_display_auth_token_t))]; /**< SDAT */
  qc_auth_content_t content; /**< plain from HLOS */
} qc_auth_display_t;

#pragma pack(pop, auth)
