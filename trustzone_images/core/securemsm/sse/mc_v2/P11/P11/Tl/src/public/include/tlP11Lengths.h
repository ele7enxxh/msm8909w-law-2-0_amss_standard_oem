/*
 * Copyright (c) 2012 - 2013 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_LENGTHS_H
#define TLP11_LENGTHS_H

#define LABEL_LEN 64 /**< Maximum length for CKA_LABEL */
#define APPLICATION_LEN 64 /**< Maximum length for CKA_APPLICATION */
#define CHECK_VALUE_LEN 3 /**< Length of CKA_CHECK_VALUE */

#define SUBJECT_MAX_LEN 256
#define MODULUS_MAX_LEN (256)
#define PRIVATE_EXPONENT_MAX_LEN (256)
#define PUBLIC_EXPONENT_LEN (4)
#define PRIME1_MAX_LEN (128)
#define PRIME2_MAX_LEN (128)
#define EXPONENT1_MAX_LEN (128)
#define EXPONENT2_MAX_LEN (128)
#define COEFFICIENT_MAX_LEN (128)

#define GENERIC_KEY_MAX_LEN SSL3_PREKEY_LEN
#define GENERIC_KEY_MIN_LEN 16
#define PBKD2_MIN_ITERATIONS  10000 // as iOS 4

#define OBJECT_ID_MAX_LEN 32

#define ISSUER_MAX_LEN 256
#define SERIAL_NUMBER_MAX_LEN 32
//#define X509_VALUE_MAX_LEN 2048
#define X509_VALUE_MAX_LEN 1408 // we had to reduce this so that it can be carried into an SO from TSM
#define URL_MAX_LEN 256

#define AES_BLOCK_SIZE_SHIFT  4
#define AES_BLOCK_SIZE  (1 << AES_BLOCK_SIZE_SHIFT)
#define AES128_KEY_LEN  16
#define AES256_KEY_LEN  32
#define SSL3_PREKEY_LEN 48

#define SSL3_PRE_KEY_GEN_LEN   46
#define SSL3_MASTER_SECRET_LEN 48
#define SSL3_PRE_KEY_OFFSET    2
#define SSL3_C_HELLO_RAND_MAX_LEN 32
#define SSL3_S_HELLO_RAND_MAX_LEN 32

#define TLS_PRF_SHA256_LEN SHA256_HASH_LEN

#define SECRET_KEY_MAX_LEN SSL3_PREKEY_LEN

#define MD5_HASH_LEN 16
#define SHA1_HASH_LEN 20
#define SHA256_HASH_LEN 32
#define PUBLIC_KEY_HASH_LEN SHA1_HASH_LEN
#define DES_BLOCK_SIZE_SHIFT  3
#define DES_BLOCK_SIZE  (1 << DES_BLOCK_SIZE_SHIFT)
#define DES_KEY_LEN     (DES_BLOCK_SIZE)
#define DES3_KEY_LEN    (DES_BLOCK_SIZE * 3)
#define SO_ID_LEN       64
#define SEED_MAX_LEN    256
#define PRF_OUT_MIN_LEN 32
#define PRF_OUT_MAX_LEN 256


#define PASSWORD_MIN_LEN   4
#define PASSWORD_MAX_LEN  32

#define IV_MAX_LEN      AES_BLOCK_SIZE
#define SALT_MIN_LEN 8
#define SALT_MAX_LEN 32
//#define DATA_MAX_LEN 1024
#define DATA_MAX_LEN 2560
/* Approximation of an RSA 2048b private key with all fields set, should be 1223 max */
#define WRAPPED_DATA_MAX_LEN 1300


#define ID_MAX_LEN  SHA1_HASH_LEN /**< for CKA_ID */

#define MAX_ALLOWED_MECHANISMS  32

#define MAX_ULONG_ATTRIBUTES_IN_TEMPLATE 4
#define MAX_BBOOL_ATTRIBUTES_IN_TEMPLATE 8

#define OTP_COUNTER_LEN 8 // sizeof(uint64_t)
#define OTP_USER_ID_LEN 32
#define OTP_SERVICE_ID_LEN 32
#define OTP_SERVICE_LOGO_LEN 1808
//#define OTP_SERVICE_LOGO_LEN 512
#define OTP_SERVICE_LOGO_TYPE_LEN 32
#define OTP_KEY_MIN_LEN 8
#define OTP_KEY_MAX_LEN 32
#define OTP_PIN_MAX_LEN 32
#define OTP_CHALLENGE_LEN 128
#define OTP_LENGTH_MIN 4
#define OTP_LENGTH_MAX 10

#define FIELD_MAX_LEN DATA_MAX_LEN

#endif /* TLP11_LENGTHS_H */
