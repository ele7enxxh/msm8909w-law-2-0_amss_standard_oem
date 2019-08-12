/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_MECH_PARAMS_H
#define TLP11_MECH_PARAMS_H

#include "tlP11Types.h"
#include "tlP11Objects.h"
#include "tlP11So.h"

#pragma pack(push, mech, 1)

typedef struct {
  CK_BBOOL valid;
  CK_BBOOL changed;
  CK_BYTE spacer[2];
} tlp11_mechanism_params_header_t;

/** RSA OAEP Encryption */
typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_MECHANISM_TYPE hashAlg;
  CK_RSA_PKCS_MGF_TYPE mgf;
  CK_BYTE  lHash[SHA256_HASH_LEN];
  CK_ULONG ulHashLen;
} tlp11_CK_RSA_PKCS_OAEP_PARAMS_t;

/** RSA PSS signing*/
typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_MECHANISM_TYPE    hashAlg;
  CK_RSA_PKCS_MGF_TYPE mgf;
  CK_ULONG             sLen;
} tlp11_CK_RSA_PKCS_PSS_PARAMS_t;

/** DES/3DES key derivation through data encryption */
typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE      iv[8];
  CK_BYTE      *data;
  CK_ULONG     len;
} tlp11_CK_DES_CBC_ENCRYPT_DATA_PARAMS_t;

/** AES CBC data encryption key derivation mechanism */
typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE      iv[16];
  CK_BYTE      *data;
  CK_ULONG     len;
} tlp11_CK_AES_CBC_ENCRYPT_DATA_PARAMS_t;

/** AES ECB data encryption key derivation mechanism */
typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE     *data;
  CK_ULONG    len;
} tlp11_CK_KEY_DERIVATION_STRING_DATA_t;

/** IV for all _CBC and _CBC_PAD mechanisms*/
typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE      iv[IV_MAX_LEN];
  CK_ULONG     iv_len;
} tlp11_IV_PARAMS_t;

/** PBE key generation */
typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE  iv[IV_MAX_LEN];                        /**< IV, if mechanism supports it */
  CK_ULONG iv_len;                        /**< IV, if mechanism supports it */
  char password[PASSWORD_MAX_LEN];           /**< used if so_password is invalid AND !useSecureKeypad */
  CK_ULONG password_len;              /**< length of password passed or requested to user */
  CK_BYTE  salt[SALT_MAX_LEN];                  /**< Salt, 8 bytes for PBKDF1, no limit for PBKDF2 */
  CK_ULONG salt_len;                  /**< Salt, 8 bytes for PBKDF1, no limit for PBKDF2 */
  CK_ULONG iterations;
} tlp11_CK_PBE_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE  iv[IV_MAX_LEN];                        /**< IV, if mechanism supports it */
  CK_ULONG iv_len;                        /**< IV, if mechanism supports it */
  tlp11_container_object_so_t so_key;
  CK_BYTE  salt[SALT_MAX_LEN];                  /**< Salt, 8 bytes for PBKDF1, no limit for PBKDF2 */
  CK_ULONG salt_len;                  /**< Salt, 8 bytes for PBKDF1, no limit for PBKDF2 */
  CK_ULONG iterations;
} tlp11_CK_PBE_GENERIC_SECRET_KEY_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  char      so[SO_CONTAINER_MAX_LEN];     /* SO to unwrap */
  CK_ULONG  soLen;  /* length of the SO */
} tlp11_CK_SO_KEY_WRAP_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE  iv[IV_MAX_LEN];                        /**< IV, if mechanism supports it */
  CK_ULONG flags;                 /**< Attributes specified as flags */
  CK_BYTE  mac[SHA256_HASH_LEN];  /**< SHA256 HMAC of (Key+flags+wrapping_key) with KMac*/
  CK_BYTE  eKmac[SHA256_HASH_LEN+AES_BLOCK_SIZE]; /**< KMac encrypted with wrapping_key*/
} tlp11_CK_WRAPKEY_AES_CBC_PAD_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_VERSION version;
} tlp11_CK_SSL3_PRE_MASTER_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE  clientRandom[32];
  CK_ULONG ulClientRandomLen;
  CK_BYTE  serverRandom[32];
  CK_ULONG ulServerRandomLen;
} tlp11_CK_SSL3_RANDOM_DATA_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  tlp11_CK_SSL3_RANDOM_DATA_t RandomInfo;
  CK_VERSION version;
} tlp11_CK_SSL3_MASTER_KEY_DERIVE_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  tlp11_container_object_so_t so_clientMacSecret;
  tlp11_container_object_so_t so_serverMacSecret;
  tlp11_container_object_so_t so_clientKey;
  tlp11_container_object_so_t so_serverKey;
  CK_BYTE      IVClient[MD5_HASH_LEN];
  CK_BYTE      IVServer[MD5_HASH_LEN];
} tlp11_CK_SSL3_KEY_MAT_OUT_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_ULONG                ulMacSizeInBits;
  CK_ULONG                ulKeySizeInBits;
  CK_ULONG                ulIVSizeInBits;
  CK_BBOOL                bIsExport;
  tlp11_CK_SSL3_RANDOM_DATA_t     RandomInfo;
} tlp11_CK_SSL3_KEY_MAT_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE      seed[SEED_MAX_LEN];
  CK_ULONG     ulSeedLen;
  CK_BYTE      label[LABEL_LEN];
  CK_ULONG     ulLabelLen;
  CK_ULONG     ulOutputLen;
  CK_BYTE      output[PRF_OUT_MAX_LEN];
} tlp11_CK_TLS_PRF_PARAMS_t;

typedef struct  {
  tlp11_mechanism_params_header_t header;
  CK_ULONG ulCounterBits;
  CK_BYTE_PTR pCb;
} tlp11_CK_AES_CTR_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_ULONG bitIndex;
} tlp11_CK_EXTRACT_PARAMS_t;

/** Used by CKM_CONCATENATE_BASE_AND_KEY */
typedef struct {
  tlp11_mechanism_params_header_t header;
  tlp11_container_object_so_t so_key;
} tlp11_CK_OBJECT_HANDLE_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE  salt[SALT_MAX_LEN];                  /**< Salt, 8 bytes for PBKDF1, no limit for PBKDF2 */
  CK_ULONG salt_len;                  /**< Salt, 8 bytes for PBKDF1, no limit for PBKDF2 */
  CK_ULONG iterations;
  CK_ULONG prf;              /**< either  1 for CKM_SHA_1_HMAC, or 2 for CKM_SHA256_HMAC */
  char password[GENERIC_KEY_MAX_LEN];           /**< password */
  CK_ULONG password_len;              /**< length of password*/
} tlp11_CK_PKCS5_PBKD2_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_BYTE  salt[SALT_MAX_LEN];                  /**< Salt, 8 bytes for PBKDF1, no limit for PBKDF2 */
  CK_ULONG salt_len;                  /**< Salt, 8 bytes for PBKDF1, no limit for PBKDF2 */
  CK_ULONG iterations;
  CK_ULONG prf;              /**< either  1 for CKM_SHA_1_HMAC, or 2 for CKM_SHA256_HMAC */
  tlp11_container_object_so_t so_key;
} tlp11_CK_PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  CK_ULONG length;
} tlp11_CK_MAC_GENERAL_PARAMS_t;

typedef struct {
  tlp11_mechanism_params_header_t header;
  UTF8CHAR pin[OTP_PIN_MAX_LEN];
  size_t pin_len;
  uint8_t challenge[OTP_CHALLENGE_LEN];
  size_t challenge_len;
  CK_ULONG time;
  CK_ULONG time_step;
  CK_ULONG number_of_steps;
  uint8_t counter[OTP_COUNTER_LEN]; // 8 bytes
  size_t counter_len;
  CK_BBOOL pin_valid;
  CK_BBOOL challenge_valid;
  CK_BBOOL time_valid;
  CK_BBOOL counter_valid;
  CK_FLAGS flags;
  CK_ULONG output_length;
  CK_BBOOL output_length_valid;
  uint8_t spacer[3];
  CK_ULONG output_format;
} tlp11_CK_OTP_PARAMS_t;

/**
 * This structure is returned in the buffer pointed by C_Sign/C_SignFinal,
 * NOT as the mechanism parameter.
 */
typedef struct {
  uint8_t returned_template[(9*3+2)*sizeof(CK_ULONG)];
  UTF8CHAR pin[OTP_PIN_MAX_LEN];
  size_t pin_len;
  uint8_t challenge[OTP_CHALLENGE_LEN];
  size_t challenge_len;
  CK_ULONG time;
  CK_ULONG time_step;
  uint8_t counter[OTP_COUNTER_LEN];
  CK_BBOOL pin_valid;
  CK_BBOOL challenge_valid;
  CK_BBOOL time_valid;
  CK_BBOOL counter_valid;
  CK_FLAGS flags;
  CK_ULONG output_length;
  CK_ULONG output_format;
  CK_ULONG value;
  CK_BBOOL output_length_valid;
  uint8_t spacer[3];
} tlp11_CK_OTP_SIGNATURE_INFO_t;

typedef union {
  tlp11_mechanism_params_header_t header;
  tlp11_CK_RSA_PKCS_OAEP_PARAMS_t RSA_PKCS_OAEP_PARAMS;
  tlp11_CK_RSA_PKCS_PSS_PARAMS_t RSA_PKCS_PSS_PARAMS;
  tlp11_CK_DES_CBC_ENCRYPT_DATA_PARAMS_t DES_CBC_ENCRYPT_DATA_PARAMS;
  tlp11_CK_AES_CBC_ENCRYPT_DATA_PARAMS_t AES_CBC_ENCRYPT_DATA_PARAMS;
  tlp11_CK_PBE_PARAMS_t PBE_PARAMS;
  tlp11_CK_PBE_GENERIC_SECRET_KEY_PARAMS_t PBE_GENERIC_SECRET_KEY_PARAMS;
  tlp11_CK_SO_KEY_WRAP_PARAMS_t SO_KEY_WRAP_PARAMS;
  tlp11_CK_SSL3_PRE_MASTER_t SSL3_PREMASTER_SECRET;
  tlp11_CK_SSL3_RANDOM_DATA_t SSL3_RANDOM_DATA;
  tlp11_CK_SSL3_MASTER_KEY_DERIVE_PARAMS_t SSL3_MASTER_KEY_DERIVE_PARAMS;
  tlp11_CK_SSL3_KEY_MAT_OUT_t SSL3_KEY_MAT_OUT;
  tlp11_CK_SSL3_KEY_MAT_PARAMS_t SSL3_KEY_MAT_PARAMS;
  tlp11_CK_TLS_PRF_PARAMS_t TLS_PRF_PARAMS;
  tlp11_CK_AES_CTR_PARAMS_t AES_CTR_PARAMS;
  tlp11_CK_KEY_DERIVATION_STRING_DATA_t KEY_DERIVATION_STRING_DATA;
  tlp11_CK_EXTRACT_PARAMS_t EXTRACT_PARAMS;
  tlp11_CK_OBJECT_HANDLE_t OBJECT_HANDLE;
  tlp11_CK_PKCS5_PBKD2_PARAMS_t PKCS5_PBKD2_PARAMS;
  tlp11_CK_PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS_t PKCS5_PBKD2_GENERIC_SECRET_KEY_PARAMS;
  tlp11_IV_PARAMS_t IV_PARAMS;
  tlp11_CK_MAC_GENERAL_PARAMS_t MAC_GENERAL_PARAMS;
  tlp11_CK_WRAPKEY_AES_CBC_PAD_PARAMS_t WRAPKEY_AES_CBC_PAD_PARAMS;
  tlp11_CK_OTP_PARAMS_t OTP_PARAMS;
} tlp11_mechanism_param_t;

#pragma pack(pop, mech)

#endif /* TLP11_MECH_PARAMS_H */
