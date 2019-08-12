/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _ASN1_H
#define _ASN1_H

#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Types.h"
#include "tlP11Objects.h"

#define RSA_PUB_KEY_BER_MAX_LEN (4+261+261)

tlApiResult_t DER_decode_RSAPublicKey(
    const uint8_t *src,
    const uint32_t src_length,
    uint8_t *public_exponent,
    uint32_t *public_exponent_len,
    uint8_t *modulus,
    uint32_t *modulus_len);

tlApiResult_t DER_decode_PrivateKeyInfo(
    const uint8_t *src,
    const uint32_t src_length,
    tlp11_rsa_private_key_t *key);

tlApiResult_t DER_encode_PrivateKeyInfo(
    const tlp11_rsa_private_key_t *key,
    uint8_t *dst,
    size_t *dst_len);


#endif
