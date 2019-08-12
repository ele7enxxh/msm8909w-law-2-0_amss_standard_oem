#ifndef MACCHIATO_ECIESAD_H
#define MACCHIATO_ECIESAD_H
/*===========================================================================
 Copyright (c) 2014 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/* 
 * ECIES (Elliptic Curve Integrated Encryption Scheme)
 * with fixed options following INCITS/ISO/IEC 18033-2:2006[2008]
 * and support for clear Associated Data
 *
 * - Curve: NIST P256
 * - KDF:   KDF2 based on SHA-256
 * - ENC:   AES-128 in CBC mode with SC1 padding: needs 128 bit key
 * - MAC:   HMAC–SHA-256–256:                     needs 256 bit key
 * - Optional shared bitstrings: NONE
 * - Variant: we generate also an IV for the encryption/decryption
 *            (I.e. the IV is not zero) if GENERATE_IV == 1
 *
 * The parameters have been chosen to have matching security level
 * (a 256 bit ECC offers 2^128 security, as AES-128, SHA-256 and
 *  HMAC–SHA-256–256)
 * 
 * The file contains the headers describing the API for
 * 
 * - ECIES encryption/decryption with algorithms chosen as above.
 *
 * Author: Roberto Avanzi, Corporate Product Security
 */

 /*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/14   cz      Ported the code to QSEE
09/30/14   ra      Author and created the initial Version.

===========================================================================*/
#include "ecc_shared.h"

// Variation of SC1 and ECIES that generates material also for an IV 
// to be used in SC1

//#define GENERATE_IV 0
#define GENERATE_IV 1

// for debugging
#define OUTPUT_DEBUG_VALUES 0

// Limit for the length of the AD and plaintext in the AD versions
#define MAX_DATA_LEN ((size_t)((1ULL<<31)-AES_BLOCKSIZE))

int macchiato_ECIESAD_encrypt(
    const MACC_POINT_TYPE  * recipient_public_key,
    const uint8_t         * AD,
    const uint32_t          AD_len,
    const uint8_t         * msg,
    const uint32_t          msg_len,
    uint8_t               * out,
    uint32_t              * out_len);

int macchiato_ECIESAD_decrypt(
    const MACC_KEY_TYPE  * private_key,
    const uint8_t        * cryptogram,
    const uint32_t         cryptogram_len,
    uint8_t              * out,
    uint32_t             * out_len,
    uint8_t             ** AD_start_p,
    uint32_t             * AD_len_p);

int macchiato_ECIESAD_get_AD(
    const uint8_t       * cryptogram,
    const uint32_t        cryptogram_len,
    uint8_t             ** AD_start,
	uint32_t              * AD_len);


#endif // End of #define MACCHIATO_ECIESAD_H

