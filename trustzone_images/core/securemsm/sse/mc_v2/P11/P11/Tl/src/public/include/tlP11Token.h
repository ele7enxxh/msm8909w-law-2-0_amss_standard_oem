/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_TOKEN_H
#define TLP11_TOKEN_H

#include "tlP11MechParams.h"

#ifdef TL_BUILD
#include "tlStd.h"
#include "TlApi/TlApi.h"
#else
// we need these for the context keys
typedef uint32_t    bool_t;     /**< boolean data type. */
typedef struct {
    uint8_t *key;   /**< Pointer to the key. */
    uint32_t len;   /**< Byte length of the key. */
} tlApiSymKey_t;

typedef struct {
    uint8_t *value;         /**< Pointer to value. Byte array in big endian format */
    uint32_t len;           /**< Byte length of value. */
} tlApiLongInt_t;

typedef struct {
    tlApiLongInt_t  exponent;           /**< Pointer to public exponent . */

    tlApiLongInt_t  modulus;            /**< Modulus (if public key present) . */

    tlApiLongInt_t  privateExponent;    /**< Private exponent (if private key present) . */

    struct {
        tlApiLongInt_t Q;         /**< Pointer to prime q (if private crt key present). */
        tlApiLongInt_t P;         /**< Pointer to prime p (if private crt key present). */
        tlApiLongInt_t DQ;        /**< Pointer to DQ := D mod(Q-1) (if private crt key present). */
        tlApiLongInt_t DP;        /**< Pointer to DP := D mod(P-1) (if private crt key present). */
        tlApiLongInt_t Qinv;      /**< Pointer to Q inverse (Qinv) := 1/Q mod P  (if private crt key present). */
    } privateCrtKey;

} tlApiRsaKey_t;

typedef union {
    tlApiSymKey_t *symKey;  /**< Pointer to symmetric key. */
    tlApiRsaKey_t *rsaKey;  /**< Pointer to RSA key. */
} tlApiKey_t;
#endif

#define TOKEN_CHALLENGE_LEN 32
#define TOKEN_CHALLENGE_RANDOM_LENGTH (TOKEN_CHALLENGE_LEN - sizeof(uint64_t))
#define TOKEN_CHALLENGE_WRAPPED_LEN SHA256_HASH_LEN
#define TOKEN_MIN_PIN_LEN 4
#define TOKEN_LABEL_LEN 32
#define TOKEN_MANUFACTURER_ID_LEN 32
#define TOKEN_MODEL_LEN 16
#define TOKEN_SERIAL_NUM_LEN  16
#define TOKEN_PIN_LEN 16
#define TOKEN_RESPONSE_MAX_LEN (TOKEN_CHALLENGE_WRAPPED_LEN + MODULUS_MAX_LEN) //68+256=324
#define TOKEN_MAX_SESSION_COUNT 4
#define TOKEN_STAMP_MASK  0x00001000 // bit 13 reserved for token stamp

/* Session handle */
#define SESSION_HANDLE_MASK_LOW  0x0000000F
#define SESSION_HANDLE_MASK_HIGH 0x000F0000
#define SESSION_HANDLE_TO_INDEX(handle) \
  (((handle) & SESSION_HANDLE_MASK_LOW) | (((handle) & SESSION_HANDLE_MASK_HIGH) >> 16))

#define SESSION_INDEX_TO_HANDLE(index,handle) \
  handle &= ~SESSION_HANDLE_MASK_HIGH; \
  handle &= ~SESSION_HANDLE_MASK_LOW; \
  handle |= (index & SESSION_HANDLE_MASK_LOW); \
  handle |= ((index & ~SESSION_HANDLE_MASK_LOW) << 12 ) & SESSION_HANDLE_MASK_HIGH;

#define SESSION_ACCESS_RW       0x00000001
#define SESSION_ACCESS_TOKEN    0x00000002
#define SESSION_ACCESS_PRIVATE  0x00000004

#pragma pack(push, token, 1)

typedef struct {
  uint32_t valid;
  CK_KEY_TYPE type;   /**< CKK_RSA / CKK_AES */
} tlp11_simple_key_header_t ;

/** Used to represent a symmetric key associated with the token*/
typedef struct {
  tlp11_simple_key_header_t header;
  uint32_t len; /**< Length in bytes, from 8 to 32 */
  uint8_t value[AES256_KEY_LEN];
} tlp11_simple_symmetric_key_t;

/** Used to represent an asymmetric key associated with the token */
typedef struct {
  tlp11_simple_key_header_t header;
  uint8_t exponent[PRIVATE_EXPONENT_MAX_LEN];
  uint32_t exponent_len;
  uint8_t modulus[MODULUS_MAX_LEN];
  uint32_t modulus_len; /**< In bytes, up to 256 */
} tlp11_simple_asymmetric_key_t;

/** Key associated with the token */
typedef union {
  tlp11_simple_key_header_t header;
  tlp11_simple_symmetric_key_t symmetric;
  tlp11_simple_asymmetric_key_t asymmetric;
} tlp11_simple_key_t;

/** Used to represent a symmetric key associated with the context */
typedef struct {
  tlApiKey_t mcKey;
  tlApiSymKey_t symKey;
  uint32_t len; /**< Length in bytes, from 8 to 32 */
  uint8_t value[AES256_KEY_LEN];
} tlp11_context_symmetric_key_t;

/** Used to represent an asymmetric key associated with the context */
typedef struct {
  tlApiKey_t mcKey;
  tlApiRsaKey_t rsaKey;
  uint8_t public_exponent[PUBLIC_EXPONENT_LEN];
  uint32_t public_exponent_len;
  uint8_t exponent[PRIVATE_EXPONENT_MAX_LEN];
  uint32_t exponent_len;
  uint8_t modulus[MODULUS_MAX_LEN];
  uint32_t modulus_len; /**< In bytes, up to 256 */
} tlp11_context_asymmetric_key_t;

/** Structure used to represent an active session on the token */
typedef struct {
  CK_SESSION_INFO info;
  uint32_t access; /**< Here we keep track whether the session is RO or RW */
  uint32_t sessionStamp;
  uint32_t handle;  /**< in the form: 0xXXXNXXXN, NN being the index this handle is stored in the array */
  struct {
    uint32_t cryptoSession; /**< MobiCore session handle */
    uint32_t operation; /**< flag indicating the current operation */
    uint32_t inputLen; /**< overall amount of input data processed so far */
    CK_MECHANISM_TYPE mechanism;
    union {
      tlp11_CK_RSA_PKCS_OAEP_PARAMS_t oaep;
      tlp11_CK_RSA_PKCS_PSS_PARAMS_t pss;
      tlp11_CK_MAC_GENERAL_PARAMS_t mac_general;
      tlp11_CK_OTP_PARAMS_t otp;
    } params;
    union {
      tlp11_context_symmetric_key_t symmetric;
      tlp11_context_asymmetric_key_t asymmetric;
    } key;

  } context;
} tlp11_session_t;

typedef struct {
  uint32_t authEntity; /**< which entity is currently authenticated. See CKU in pkcs11t.h */
  uint64_t lastAuthTime; /**< Timestamp of last successful user authentication */
  uint32_t usageNumLeft;
  uint8_t challenge[TOKEN_CHALLENGE_LEN]; /**< used for remote authentication */
} tlp11_token_auth_t;

/** Group of sessions associated with an application
 * All sessions open from an application share the same authentication status.
 * */
typedef struct {
  uint32_t tokenStamp;
  uint32_t changed; /**< volatile, for internal use only */
  uint32_t magic;
  tlp11_token_auth_t authInfo;
  DECLARE_ENC_START();
  tlp11_session_t sessions[TOKEN_MAX_SESSION_COUNT];
} tlp11_session_group_t;

typedef struct {
  uint64_t remoteLockTime;
  uint64_t localLockTime;
} tlp11_lock_time_t;

#define SO_TIME_MAX_LEN MC_SO_SIZE(0,sizeof(tlp11_lock_time_t))

typedef struct {
  size_t len;
  uint8_t value[SO_TIME_MAX_LEN];
} tlp11_lock_time_so_t;

/** Token.
 * A PKCS11 Token corresponds to a credential token
 * */
typedef struct {
  CK_TOKEN_INFO info;
  uint32_t tokenStamp;
  uint32_t tokenIsLocal;
  uint32_t changed; /**< volatile, for internal use only */
  uint32_t magic;
  DECLARE_ENC_START();
  uint32_t authSpan;  /**< Life span of an authentication on the token */
  uint32_t usageNum;
  uint32_t debugLockedTime; /**< for debug purposes, not used in production */
  tlp11_lock_time_so_t locks; /**< Time at which remote/local was locked out, secured */
  uint32_t loginAttempts;
  uint32_t remoteLoginAttempts;
  uint32_t enabledFunctions;
  uint32_t securityExtensions;
  UTF8CHAR pin[TOKEN_PIN_LEN]; /**< user PIN */
  uint32_t  pinLen;
  tlp11_simple_key_t associatedKey;
} tlp11_token_t;

#define SO_TOKEN_MAX_LEN (sizeof(tlp11_token_t) + SO_OVERHEAD_MAX)

typedef struct {
  uint32_t changed;
  size_t len;
  uint8_t value[SO_TOKEN_MAX_LEN];
} tlp11_token_so_t;

#define SO_SESSION_GROUP_MAX_LEN (sizeof(tlp11_session_group_t) + SO_OVERHEAD_MAX)

typedef struct {
  uint32_t changed;
  size_t len;
  uint8_t value[SO_SESSION_GROUP_MAX_LEN];
} tlp11_session_group_so_t;

#pragma pack(pop, token)

#endif /* TLP11_TOKEN_H */
