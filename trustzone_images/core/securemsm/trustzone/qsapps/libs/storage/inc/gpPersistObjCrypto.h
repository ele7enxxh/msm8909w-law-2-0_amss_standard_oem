#ifndef _GP_PERSIST_OBJ_CRYPTO_H_
#define _GP_PERSIST_OBJ_CRYPTO_H_

/**
@file   gpPersistObjCrypto.h
@brief  Defintions for cryptogrpahic module in the gp Persistent Object 
        implementation.
*/

/*=============================================================================
Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/libs/storage/inc/gpPersistObjCrypto.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/15   mr      Initial Implementation
=============================================================================*/

/*===========================================================================
===
=== The gpPersistObjCrypto module provides an interface between the
=== cryptographic resources needed by the PersistentObject implementation
=== and the low level cryptographic services provided by the Trustzone
=== crypto driver.
===
=== This module provides the following protection and verification of the
=== below security features:
===     - Confidentiality (AES256 CBC)
===     - Integrity       (HMAC SHA256, HASH SHA256)
=== through the interface defined in this file.
===
=== Version 1.0
===   As part of version 1.0, there are some restrictions on the usage of
===   these APIs.
===     1. The IV and HMAC/HASH sizes currently only support usage with
===        AES256 and SHA256 algorithms. Use the macro definitions below to
===        guarentee that you are allocating the correc sizes.
===     2. The only valid value of 'handle' in each of the APIs provided below
===        is NULL. This parameter will be used for further expansion and
===        GP use cases.
===
===========================================================================*/

#include "gpPersistObjHandler.h"
#include "gpTypes.h"
#include "qsee_cipher.h"
#include "qsee_hmac.h"
#include "qsee_hash.h"

/* Crypto Attribute Sizes */
#define GPCRYPTO_IV_SIZE    QSEE_AES256_IV_SIZE
#define GPCRYPTO_HMAC_SIZE  QSEE_SHA256_HASH_SZ
#define GPCRYPTO_HASH_SIZE  QSEE_SHA256_HASH_SZ

/*===========================================================================
=== gpCrypto_Encrypt
===
=== Encrypt data specified using iv provided by the caller and unique key
=== generated internally for application. Encryption operation will be done
=== in place within the user allocated buffer provided.
===
=== Parameters:
===   - [in]handle    Used by GP Persistent Object Framework to specify meta
===                   data for additional functionality. Pass NULL for non-GP
===                   use cases.
===   - [in/out]data  Pointer to buffer containing data to encrypt, which will
===                   also contain the output ciphertext after encryption. Caller
===                   is responsible for memory management of buffer.
===   - [in]dataLen   Length of the data buffer allocated by caller.
===   - [out]iv       Pointer to the iv (initialization vector) container in
===                   which a random value will be generated.
===   - [in]ivLen     Length of the iv buffer allocated by the caller.
===
=== Returns
===   - TEE_SUCCESS        When encrypt operation is successfull.
===   - TEE_ERROR_GENERIC  When encrypt operation fails.
===
===========================================================================*/
TEE_Result gpCrypto_Encrypt
(
  gpHandle* handle,
  void*  data,
  size_t dataLen,
  void*  iv,
  size_t ivLen
);

/*===========================================================================
=== gpCrypto_Protect
===
=== Protect the data specified by generating a hash and/or hmac into the
=== specified containers.
===
=== Parameters:
===   - [in]handle    Used by GP Persistent Object Framework to specify meta
===                   data for additional functionality. Pass NULL for non-GP
===                   use cases.
===   - [in]data      Pointer to buffer containing data to hash and/or hmac.
===                   Caller is responsible for memory management of buffer.
===   - [in]dataLen   Length of the data buffer allocated by caller.
===   - [out]hash     Pointer to the hash output container where the generated
===                   hash over the data will be stored. Caller is responsible
===                   for memory management of buffer.
===   - [in]hashLen   Length of the hash container allocated by the caller.
===   - [out]hmac     Pointer to the hmac output container where the generated
===                   hmac over the data will be stored. Caller is responsible
===                   for memory management of buffer.
===   - [in]hmacLen   Length of the hmac container allocated by the caller.
===
=== Returns
===   - TEE_SUCCESS        When hash and/or hmac operation is successfull.
===   - TEE_ERROR_GENERIC  When hash and/or hmac operation fails.
===
===========================================================================*/
TEE_Result gpCrypto_Integrity_Protect
(
  gpHandle* handle,
  void*  data,
  size_t dataLen,
  void*  hash,
  size_t hashLen,
  void*  hmac,
  size_t hmacLen
);

/*===========================================================================
=== gpCrypto_Decrypt
===
=== Decrypt data specified using iv provided by the caller and unique key
=== generated internally for application. Decryption operation will be done
=== in place within the user allocated buffer provided.
===
=== Parameters:
===   - [in]handle    Used by GP Persistent Object Framework to specify meta
===                   data for additional functionality. Pass NULL for non-GP
===                   use cases.
===   - [in/out]data  Pointer to buffer containing data to decrypt, which will
===                   also contain the output plaintext after decryption. Caller
===                   is responsible for memory management of buffer.
===   - [in]dataLen   Length of the data buffer allocated by caller.
===   - [in]iv        Pointer to the iv (initialization vector) container which
===                   includes valid information set by the caller.
===   - [in]ivLen     Length of the iv buffer allocated by the caller.
===
=== Returns
===   - TEE_SUCCESS        When decrypt operation is successfull.
===   - TEE_ERROR_GENERIC  When decrypt operation fails.
===
===========================================================================*/
TEE_Result gpCrypto_Decrypt
(
  gpHandle* handle,
  void*  data,
  size_t dataLen,
  void*  iv,
  size_t ivLen
);

/*===========================================================================
=== gpCrypto_Verify
===
=== Verifies the data specified by comparing the hash and/or hmac provided in
=== the specified containers with the ones generated from the data.
===
=== Parameters:
===   - [in]handle    Used by GP Persistent Object Framework to specify meta
===                   data for additional functionality. Pass NULL for non-GP
===                   use cases.
===   - [in]data      Pointer to buffer containing data to hash and/or hmac.
===                   Caller is responsible for memory management of buffer.
===   - [in]dataLen   Length of the data buffer allocated by caller.
===   - [in]hash      Pointer to the hash input container which will be compared
===                   to the generated hash over the data. Caller is responsible
===                   for memory management of buffer.
===   - [in]hashLen   Length of the hash container allocated by the caller.
===   - [in]hmac      Pointer to the hmac input container which will be compared
===                   to the generated hmac over the data. Caller is responsible
===                   for memory management of buffer.
===   - [in]hmacLen   Length of the hmac container allocated by the caller.
===
=== Returns
===   - TEE_SUCCESS        When hash and/or hmac comparison is successfull.
===   - TEE_ERROR_GENERIC  When hash and/or hmac comparison fails.
===
===========================================================================*/
TEE_Result gpCrypto_Integrity_Verify
(
  gpHandle* handle,
  void*  data,
  size_t dataLen,
  void*  hash,
  size_t hashLen,
  void*  hmac,
  size_t hmacLen
);

#endif //_GP_PERSIST_OBJ_CRYPTO_H_
