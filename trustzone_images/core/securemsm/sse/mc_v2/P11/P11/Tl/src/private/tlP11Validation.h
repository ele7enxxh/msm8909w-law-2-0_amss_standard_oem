/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_VALIDATION_H
#define TLP11_VALIDATION_H

#include "tlP11Objects.h"
#include <stddef.h>

//CK_RV validateDate(CK_DATE *date);

// Validate keys
/** Validate a Storage object
 */
CK_RV validateStorage(const tlp11_storage_t *storage);

/** Validate a Data object
 */
CK_RV validateData(const tlp11_data_t *data);

/** Validate an RSA public key
 */
CK_RV validateKey(const tlp11_key_t *key);

/** Validate an RSA public key
 */
CK_RV validatePublicKey(const tlp11_public_key_t *key);

/** Validate an RSA public key
 */
CK_RV validateRSAPublicKey(const tlp11_rsa_public_key_t *key);

/** Validate a private key
 */
CK_RV validatePrivateKey(tlp11_private_key_t *key);

/** Validate an RSA private key
 */
CK_RV validateRSAPrivateKey(tlp11_rsa_private_key_t *key);

/** Checks that the RSA variables are consistent performing an encryption/decryption
 */
CK_RV cryptoValidateRSAPrivateKey(tlp11_rsa_private_key_t *key);

/** Validate an X.509 certificate
 */
CK_RV validateCertificate(const tlp11_certificate_t *certificate);

/** Validate an X.509 certificate
 */
CK_RV validateX509Certificate(tlp11_certificate_x509_t *x509);

/** Validate a Secret Key
 */
CK_RV validateSecretKey(tlp11_secret_key_t *secret);

/** Validate a Generic Secret Key
 */
CK_RV validateGenericSecretKey(tlp11_secret_key_simple_t *generic);

/** Validate an AES secret key
 */
CK_RV validateAESSecretKey(tlp11_secret_key_simple_t *aes);

/** Validate a DES secret key
 */
CK_RV validateDESSecretKey(tlp11_secret_key_simple_t *des);

/** Validate an OTP secret key
 */
CK_RV validateOTPSecretKey(tlp11_otp_secret_key_t *otp);

/** Validate a generic container object
 */
CK_RV validateObject(tlp11_container_object_t *object);

/** Apply Secure Template validation to a secret key
 */
CK_RV validateSecureTemplateSecretKey(const tlp11_secret_key_t *key, uint32_t operation);

/** Apply Secure Template validation to a public/private key pair
 */
CK_RV validateSecureTemplateKeyPair(
    const tlp11_public_key_t *pub,
    const tlp11_private_key_t *prv,
    uint32_t operation);

#endif /* TLP11_VALIDATION_H */
