/**
 * @addtogroup TLAPI_CR
 * @{
 * The MobiCore OS provides Trustlets with access to cryptographic calculations.
 *
 * Depending on the platform, cryptographic functions are implemented in a software library or using a hardware cryptographic engine.
 * Depending on the used cryptographic driver, certain functions may or may not exist.
 *
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */
#ifndef __TLAPICRYPTO_H__
#define __TLAPICRYPTO_H__

#include "TlApi/TlApiCommon.h"
#include "TlApi/TlApiError.h"

/** Handle of a crypto session. */
typedef uint32_t tlApiCrSession_t;

/** Invalid crypto session id returned in case of an error. */
#define CR_SID_INVALID      0xffffffff

/**
 * Algorithm ID is composed of group flags and a consecutive number.
 * The upper 16bit are used for grouping, whereas the lower 16bit
 * are available to distinguish algorithms within each group.
 */

/** Algorithm type flags. */
#define AF_CIPHER           (1U << 24)
#define AF_SIG              (2U << 24)
#define AF_MD               (4U << 24)
#define AF_RNG              (8U << 24)


/** Subgroups of cipher algorithms. */
#define AF_CIPHER_AES       (1U << 16)
#define AF_CIPHER_3DES      (2U << 16)
#define AF_CIPHER_DES       (4U << 16)
#define AF_CIPHER_RSA       (8U << 16)

/** List of Cipher algorithms.
 * An algorithm in this list is to be interpreted as a combination of cryptographic algorithm,
 * paddings, block sizes and other information.
 */
typedef enum {
    /*------- AES ciphers start here -------*/
    TLAPI_ALG_AES_128_CBC_NOPAD         = (AF_CIPHER | AF_CIPHER_AES | 1),      /**< AES (block length 128) with key size 128 in CBC mode, no padding. */
    TLAPI_ALG_AES_128_CBC_ISO9797_M1    = (AF_CIPHER | AF_CIPHER_AES | 2),      /**< AES (block length 128) with key size 128 in CBC mode, padding according to the ISO 9797 method 1 scheme. */
    TLAPI_ALG_AES_128_CBC_ISO9797_M2    = (AF_CIPHER | AF_CIPHER_AES | 3),      /**< AES (block length 128) with key size 128 in CBC mode, padding according to the ISO 9797 method 2 (ISO 7816-4, EMV'96) scheme. */
    TLAPI_ALG_AES_128_CBC_PKCS5         = (AF_CIPHER | AF_CIPHER_AES | 4),      /**< AES (block length 128) with key size 128 in CBC mode, padding according to the PKCS#5 scheme. */
    TLAPI_ALG_AES_128_CBC_PKCS7         = (AF_CIPHER | AF_CIPHER_AES | 6),      /**< AES (block length 128) with key size 128 in CBC mode, padding according to the PKCS#7 scheme. */
    TLAPI_ALG_AES_128_ECB_NOPAD         = (AF_CIPHER | AF_CIPHER_AES | 7),      /**< AES (block length 128) with key size 128 in ECB mode, no padding. */
    TLAPI_ALG_AES_128_ECB_ISO9797_M1    = (AF_CIPHER | AF_CIPHER_AES | 8),      /**< AES (block length 128) with key size 128 in ECB mode, padding according to the ISO 9797 method 1 scheme. */
    TLAPI_ALG_AES_128_ECB_ISO9797_M2    = (AF_CIPHER | AF_CIPHER_AES | 9),      /**< AES (block length 128) with key size 128 in ECB mode, padding according to the ISO 9797 method 2 (ISO 7816-4, EMV'96) scheme. */
    TLAPI_ALG_AES_128_ECB_PKCS5         = (AF_CIPHER | AF_CIPHER_AES | 0xa),    /**< AES (block length 128) with key size 128 in ECB mode, padding according to the PKCS#5 scheme. */
    TLAPI_ALG_AES_128_ECB_PKCS7         = (AF_CIPHER | AF_CIPHER_AES | 0xc),    /**< AES (block length 128) with key size 128 in ECB mode, padding according to the PKCS#7 scheme. */
    TLAPI_ALG_AES_128_CTR_NOPAD         = (AF_CIPHER | AF_CIPHER_AES | 0xd),    /**< AES (block length 128) with key size 128 in CTR mode, no padding. */
    TLAPI_ALG_AES_256_CBC_NOPAD         = (AF_CIPHER | AF_CIPHER_AES | 0x10),   /**< AES (block length 128) with key size 256 in CBC mode, no padding. */
    TLAPI_ALG_AES_256_CBC_ISO9797_M1    = (AF_CIPHER | AF_CIPHER_AES | 0x11),   /**< AES (block length 128) with key size 256 in CBC mode, padding according to the ISO 9797 method 1 scheme. */
    TLAPI_ALG_AES_256_CBC_ISO9797_M2    = (AF_CIPHER | AF_CIPHER_AES | 0x12),   /**< AES (block length 128) with key size 256 in CBC mode, padding according to the ISO 9797 method 2 (ISO 7816-4, EMV'96) scheme. */
    TLAPI_ALG_AES_256_CBC_PKCS5         = (AF_CIPHER | AF_CIPHER_AES | 0x13),   /**< AES (block length 128) with key size 256 in CBC mode, padding according to the PKCS#5 scheme. */
    TLAPI_ALG_AES_256_CBC_PKCS7         = (AF_CIPHER | AF_CIPHER_AES | 0x15),   /**< AES (block length 128) with key size 256 in CBC mode, padding according to the PKCS#7 scheme. */
    TLAPI_ALG_AES_256_ECB_NOPAD         = (AF_CIPHER | AF_CIPHER_AES | 0x16),   /**< AES (block length 128) with key size 256 in ECB mode, no padding. */
    TLAPI_ALG_AES_256_ECB_ISO9797_M1    = (AF_CIPHER | AF_CIPHER_AES | 0x17),   /**< AES (block length 128) with key size 256 in ECB mode, padding according to the ISO 9797 method 1 scheme. */
    TLAPI_ALG_AES_256_ECB_ISO9797_M2    = (AF_CIPHER | AF_CIPHER_AES | 0x18),   /**< AES (block length 128) with key size 256 in ECB mode, padding according to the ISO 9797 method 2 (ISO 7816-4, EMV'96) scheme. */
    TLAPI_ALG_AES_256_ECB_PKCS5         = (AF_CIPHER | AF_CIPHER_AES | 0x19),   /**< AES (block length 128) with key size 256 in ECB mode, padding according to the PKCS#5 scheme. */
    TLAPI_ALG_AES_256_ECB_PKCS7         = (AF_CIPHER | AF_CIPHER_AES | 0x1b),   /**< AES (block length 128) with key size 256 in ECB mode, padding according to the PKCS#7 scheme. */
    TLAPI_ALG_AES_256_CTR_NOPAD         = (AF_CIPHER | AF_CIPHER_AES | 0x1c),   /**< AES (block length 128) with key size 256 in CTR mode, no padding. */

    /*------- Triple-DES ciphers start here -------*/
    TLAPI_ALG_3DES_CBC_ISO9797_M1       = (AF_CIPHER | AF_CIPHER_3DES | 1),  /**< Triple DES in outer CBC mode, padding according to the ISO 9797 method 1 scheme. */
    TLAPI_ALG_3DES_CBC_ISO9797_M2       = (AF_CIPHER | AF_CIPHER_3DES | 2),  /**< Triple DES in outer CBC mode, padding according to the ISO 9797 method 2 (ISO 7816-4, EMV'96) scheme. */
    TLAPI_ALG_3DES_CBC_NOPAD            = (AF_CIPHER | AF_CIPHER_3DES | 3),  /**< Triple DES in outer CBC mode, no padding. */
    TLAPI_ALG_3DES_CBC_PKCS5            = (AF_CIPHER | AF_CIPHER_3DES | 4),  /**< Triple DES in outer CBC mode, padding according to the PKCS#5 scheme. */

    /*------- DES ciphers start here -------*/
    TLAPI_ALG_DES_CBC_ISO9797_M1        = (AF_CIPHER | AF_CIPHER_DES | 1),   /**< DES in CBC mode or triple DES in outer CBC mode, padding according to the ISO 9797 method 1 scheme. */
    TLAPI_ALG_DES_CBC_ISO9797_M2        = (AF_CIPHER | AF_CIPHER_DES | 2),   /**< DES in CBC mode or triple DES in outer CBC mode, padding according to the ISO 9797 method 2 (ISO 7816-4, EMV'96) scheme. */
    TLAPI_ALG_DES_CBC_NOPAD             = (AF_CIPHER | AF_CIPHER_DES | 3),   /**< DES in CBC mode or triple DES in outer CBC mode, no padding. */
    TLAPI_ALG_DES_CBC_PKCS5             = (AF_CIPHER | AF_CIPHER_DES | 4),   /**< DES in CBC mode or triple DES in outer CBC mode, padding according to the PKCS#5 scheme. */
    TLAPI_ALG_DES_ECB_ISO9797_M1        = (AF_CIPHER | AF_CIPHER_DES | 5),   /**< DES in ECB mode, padding according to the ISO 9797 method 1 scheme. */
    TLAPI_ALG_DES_ECB_ISO9797_M2        = (AF_CIPHER | AF_CIPHER_DES | 6),   /**< DES in ECB mode, padding according to the ISO 9797 method 2 (ISO 7816-4, EMV'96) scheme. */
    TLAPI_ALG_DES_ECB_NOPAD             = (AF_CIPHER | AF_CIPHER_DES | 7),   /**< DES in ECB mode, no padding. */
    TLAPI_ALG_DES_ECB_PKCS5             = (AF_CIPHER | AF_CIPHER_DES | 8),   /**< DES in ECB mode, padding according to the PKCS#5 scheme. */

    /*------- RSA ciphers start here -------*/
    TLAPI_ALG_RSA_ISO14888              = (AF_CIPHER | AF_CIPHER_RSA | 1),   /**< RSA, padding according to the ISO 14888 scheme. */
    TLAPI_ALG_RSA_NOPAD                 = (AF_CIPHER | AF_CIPHER_RSA | 2),   /**< RSA, no padding. */
    TLAPI_ALG_RSA_PKCS1                 = (AF_CIPHER | AF_CIPHER_RSA | 3),   /**< RSA, padding according to the PKCS#1 (v1.5) scheme. */

    /*------- RSA CRT ciphers start here -------*/
    TLAPI_ALG_RSACRT_ISO14888           = TLAPI_ALG_RSA_ISO14888,
    TLAPI_ALG_RSACRT_NOPAD              = TLAPI_ALG_RSA_NOPAD,
    TLAPI_ALG_RSACRT_PKCS1              = TLAPI_ALG_RSA_PKCS1
} tlApiCipherAlg_t;


#define TLAPI_ALG_DES_BLOCK_SIZE 0x08
#define TLAPI_ALG_AES_BLOCK_SIZE 0x10

/** Subgroups of signature algorithms. */
#define AF_SIG_DES      (1U << 16)
#define AF_SIG_HMAC     (2U << 16)
#define AF_SIG_RSA      (4U << 16)

/** List of Signature algorithms.
 * An algorithm in this list is to be interpreted as a combination of cryptographic algorithm,
 * paddings, block sizes and other information.
 */
typedef enum {
    /*------- Retail MAC's start here -------*/
    TLAPI_ALG_DES_MAC4_NOPAD                = (AF_SIG | AF_SIG_DES | 1),   /**< 4-byte MAC (most significant 4 bytes of encrypted block) using DES in CBC mode or triple DES in outer CBC mode. */
    TLAPI_ALG_DES_MAC4_PKCS5                = (AF_SIG | AF_SIG_DES | 2),   /**< 4-byte MAC (most significant 4 bytes of encrypted block) using DES in CBC mode or triple DES in outer CBC mode. */
    TLAPI_ALG_DES_MAC8_ISO9797_1_M2_ALG3    = (AF_SIG | AF_SIG_DES | 3),   /**< 8-byte MAC using a 2-key DES3 key according to ISO9797-1 MAC algorithm 3
                                                                            with method 2 (also EMV'96, EMV'2000), where input data is padded
                                                                            using method 2 and the data is processed as described
                                                                            in MAC Algorithm 3 of the ISO 9797-1 specification. */
    TLAPI_ALG_DES_MAC8_ISO9797_M1           = (AF_SIG | AF_SIG_DES | 4),   /**< 8-byte MAC using DES in CBC mode or triple DES in outer CBC mode. */
    TLAPI_ALG_DES_MAC8_ISO9797_M2           = (AF_SIG | AF_SIG_DES | 5),   /**< 8-byte MAC using DES in CBC mode or triple DES in outer CBC mode. */
    TLAPI_ALG_DES_MAC8_NOPAD                = (AF_SIG | AF_SIG_DES | 6),   /**< 8-byte MAC using DES in CBC mode or triple DES in outer CBC mode. */
    TLAPI_ALG_DES_MAC8_PKCS5                = (AF_SIG | AF_SIG_DES | 7),   /**< 8-byte MAC using DES in CBC mode or triple DES in outer CBC mode. */

    /*------- SHA MAC's start here -------*/
    TLAPI_ALG_HMAC_SHA_256                  = (AF_SIG | AF_SIG_HMAC | 1),  /**< HMAC following the steps found in RFC: 2104 using SHA-256 as the hashing algorithm. */
    TLAPI_ALG_HMAC_SHA1                     = (AF_SIG | AF_SIG_HMAC | 2),  /**< HMAC following the steps found in RFC: 2104 using SHA1 as the hashing algorithm. */

    /*------- RSA starts here -------*/
    TLAPI_SIG_RSA_SHA_ISO9796               = (AF_SIG | AF_SIG_RSA | 1),   /**< 20-byte SHA-1 digest, padded according to the ISO 9796-2 scheme as specified in EMV '96 and EMV 2000, encrypted using RSA. */
    TLAPI_SIG_RSA_SHA_ISO9796_MR            = (AF_SIG | AF_SIG_RSA | 2),   /**< 20-byte SHA-1 digest, padded according to the ISO9796-2 specification and encrypted using RSA. */
    TLAPI_SIG_RSA_SHA_PKCS1                 = (AF_SIG | AF_SIG_RSA | 3),   /**< 20-byte SHA-1 digest, padded according to the PKCS#1 (v1.5) scheme, and encrypted using RSA. */
    TLAPI_SIG_RSA_SHA256_PSS                = (AF_SIG | AF_SIG_RSA | 4),   /**< RSASSA-PSS-VERIFY, ContenDigest-SHA256, MfgDigest-SHA256. */
    TLAPI_SIG_RSA_SHA1_PSS                  = (AF_SIG | AF_SIG_RSA | 5),   /**< RSASSA-PSS-VERIFY, ContenDigest-SHA1, MfgDigest-SHA1. */

    /*------- RSA CRT ciphers start here -------*/
    TLAPI_SIG_RSACRT_SHA_ISO9796            = TLAPI_SIG_RSA_SHA_ISO9796,
    TLAPI_SIG_RSACRT_SHA_ISO9796_MR         = TLAPI_SIG_RSA_SHA_ISO9796_MR,
    TLAPI_SIG_RSACRT_SHA_PKCS1              = TLAPI_SIG_RSA_SHA_PKCS1,
    TLAPI_SIG_RSACRT_SHA256_PSS             = TLAPI_SIG_RSA_SHA256_PSS,
    TLAPI_SIG_RSACRT_SHA1_PSS               = TLAPI_SIG_RSA_SHA1_PSS

} tlApiSigAlg_t;

#define TLAPI_ALG_HMAC_SHA_256_SIZE 0x20
#define TLAPI_ALG_HASH_SHA1_SIZE 0x14
#define TLAPI_ALG_HASH_SHA256_SIZE 0x20

/** List of Message Digest algorithms. */
typedef enum {
    TLAPI_ALG_MD2       = (AF_MD | 1),   /**< Message Digest algorithm MD2. */
    TLAPI_ALG_MD5       = (AF_MD | 2),   /**< Message Digest algorithm MD5. */
    TLAPI_ALG_SHA1      = (AF_MD | 3),   /**< Message Digest algorithm SHA-1. */
    TLAPI_ALG_SHA256    = (AF_MD | 4)    /**< Message Digest algorithm SHA-256. */
} tlApiMdAlg_t;

/** List of Random Data Generation algorithms. */
typedef enum {
    TLAPI_ALG_SECURE_RANDOM = (AF_RNG | 1),   /**< Random data which is considered to be cryptographically secure. */
    TLAPI_ALG_PSEUDO_RANDOM = (AF_RNG | 2)    /**< Pseudo random data, most likely a returning pattern. */
} tlApiRngAlg_t;

/** List of Key Pair types. */
typedef enum {
    TLAPI_RSA       = 0x00000001   /**< RSA public and RSA normal / crt private key. */
} tlApiKeyPairType_t;

/** Symmetric key structure. */
typedef struct {
    uint8_t *key;   /**< Pointer to the key. */
    uint32_t len;   /**< Byte length of the key. */
} tlApiSymKey_t;


/** Structure presenting long integer
 * If integer is not present, value must be null and len zero.
 *  */
typedef struct {
    uint8_t *value;         /**< Pointer to value. Byte array in big endian format */
    uint32_t len;           /**< Byte length of value. */
} tlApiLongInt_t;

/** RSA key structure.
 *
 * Key structure may contain public key, both public key and private key or Private key only.
 *
 * In all cases public exponent must be present.
 *
 * if any of publicKey, privateKey or privateCrtKey is not used, its pointers must be null
 * and lengths zero.
 *
 * @note Although the platform itself is running on little endian, the crypto library expects
 *       buffers to be in big endian format.!
 *
 * @note Key component modulus, P and Q must have non-zero most significant byte (if present).
 *
 * @note Private operations are done using Crt key, if present. Normal private key is used only when
 *       Crt key is not present.
 *
 *
 *
 */
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


/** Union of key structure pointers.
 * Enables generic interfaces.
 */
typedef union {
    tlApiSymKey_t *symKey;              /**< Pointer to symmetric key. */
    tlApiRsaKey_t *rsaKey;              /**< Pointer to RSA key. */
} tlApiKey_t;

/** Union of key pair pointers.
 * Enables generic interfaces.
 */
typedef union {
    tlApiRsaKey_t *rsaKeyPair;       /**< Pointer to RSA key structure. */
} tlApiKeyPair_t;

/** Main operation modes for cipher. */
typedef enum {
    TLAPI_MODE_ENCRYPT = 0x00000000,    /**< Encryption mode. */
    TLAPI_MODE_DECRYPT = 0x00000001     /**< Decryption mode. */
} tlApiCipherMode_t;

/** Main operation modes for signature. */
typedef enum {
    TLAPI_MODE_SIGN = 0x00000000,   /**< Signature generation mode. */
    TLAPI_MODE_VERIFY = 0x00000001  /**< Message and signature verification mode. */
} tlApiSigMode_t;

/** Aborts a crypto session.
 * Afterwards sessionHandle is not valid anymore.
 *
 * @param sessionHandle Handle of session to be aborted.
 *
 * @returns TLAPI_OK    if operation was successful.
 * @returns E_TLAPI_CR_HANDLE_INVALID   if no valid handle was provided.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiCrAbort(
    tlApiCrSession_t    sessionHandle);

/** Generates a key pair.
 * The key components are generated according to requested type and length.
 *
 * The caller has to set addresses in the key pair structure and initialize public key exponent.
 * Generated key components are written to those addresses. It is in the responsibility of the
 * caller to provide sufficient space and set length parameters of each of the buffer length elements
 * in key structures appropriately.
 * Length informations of generated components will be overwritten with the actual length of the
 * generated key pair structure elements.
 *
 * For RSA the passed length value identifies the length of the modulus, the generated exponent
 * can be of shorter length.
 *
 * For RSA CRT keys components P, Q, DP, DQ and QInv should have at least length of half
 * of the modulus (rounded up).
 *
 * @note MobiCore supports up to 2048 bit RSA keys as standard feature.

 * @note Public exponent must have non-zero most significant byte. Also public exponent must be odd.
 *
 * @param [in,out]  keyPair Reference to key pair structure.
 * @param           type    See enum keyPairType_t.
 * @param           len     Requested byte length of keys.
 *
 * @returns TLAPI_OK                        if operation was successful.
 * @returns E_TLAPI_NULL_POINTER            if one parameter is a null pointer.
 * @returns E_TLAPI_INVALID_RANGE           if buffer is not within the drivers range.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE if combination of algorithm and mode and key length is not available.
 * @returns E_TLAPI_CR_WRONG_OUTPUT_SIZE    if provided buffer length of one of the buffers is too small.
 * @returns E_TLAPI_DRV_UNKNOWN             if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER         if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiGenerateKeyPair(
    tlApiKeyPair_t      *keyPair,
    tlApiKeyPairType_t  type,
    size_t              len);

/** Initializes a new cipher session.
 * A handle for the new session is generated.
 * The session is associated with the key. Mode and algorithm type are set.
 * If this method does not return with TLAPI_OK then there is no valid handle value.
 * No crypto session is opened in case of an error.
 *
 * @param [out] pSessionHandle  Reference to generated Cipher session handle (undefined in case of error).
 * @param       alg             See enum cipherMode_t.
 * @param       mode            TLAPI_MODE_ENCRYPT or TLAPI_MODE_DECRYPT.
 * @param       key             Key for this session. Key data is not copied but stored as reference.
 *                              Must maintain unchanged during session!
 *
 * @returns TLAPI_OK                        if operation was successful.
 * @returns E_TLAPI_NULL_POINTER            if one parameter is a null pointer.
 * @returns E_TLAPI_INVALID_RANGE           if buffer is not within the drivers memory range.
 * @returns E_TLAPI_INVALID_INPUT           if provided mode is unknown or RSA cipher modulus length is zero.
 * @returns E_TLAPI_CR_OUT_OF_RESOURCES     if no more session could be created.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE if combination of algorithm and mode and key length is not available.
 * @returns E_TLAPI_DRV_UNKNOWN             if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER         if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiCipherInit(
    tlApiCrSession_t    *pSessionHandle,
    tlApiCipherAlg_t    alg,
    tlApiCipherMode_t   mode,
    const tlApiKey_t    *key);

/** Initializes a new cipher session.
 * A handle for the new session is generated.
 * The session is associated with the key. Mode and algorithm type are set.
 * If this method does not return with TLAPI_OK then there is no valid handle value.
 * No crypto session is opened in case of an error.
 *
 * @note If the buffer/bufferLen contains invalid or inconsistent data (wrong length or length = 0,
 *       null, ...) a default value = 0 is taken and NO error is returned.
 * @note If the used algorithm doesn't use additional algorithm specific data, the given values are
 *       ignored and don't result in an error.
 * @note MobiCore supports up to 2048 bit RSA keys as standard feature.
 *
 * @param [out] pSessionHandle Reference to generated Cipher session handle (undefined in case of error).
 * @param       alg            See enum cipherMode_t.
 * @param       mode           TLAPI_MODE_ENCRYPT or TLAPI_MODE_DECRYPT.
 * @param       key            Key for this session. Key data is not copied but stored as reference.
 *                             Must maintain unchanged during session!
 * @param       buffer         Reference to algorithm specific data (initial values).
 * @param       bufferLen      Length of buffer containing algorithm specific data.
 *
 * @returns TLAPI_OK                        if operation was successful.
 * @returns E_TLAPI_NULL_POINTER            if one parameter is a null pointer.
 * @returns E_TLAPI_INVALID_RANGE           if buffer is not within the drivers memory range.
 * @returns E_TLAPI_INVALID_INPUT           if provided mode is unknown or RSA cipher modulus length is zero.
 * @returns E_TLAPI_CR_OUT_OF_RESOURCES     if no more session could be created.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE if combination of algorithm and mode and key length is not available.
 * @returns E_TLAPI_DRV_UNKNOWN             if some unknown error occurred.
 * @returns E_TLAPI_CR_INCONSISTENT_DATA    if key type doesn't match the algorithm.
 * @returns E_TLAPI_UNMAPPED_BUFFER         if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiCipherInitWithData(
    tlApiCrSession_t    *pSessionHandle,
    tlApiCipherAlg_t    alg,
    tlApiCipherMode_t   mode,
    const tlApiKey_t    *key,
    const uint8_t       *buffer,
    size_t              bufferLen);

/** Encrypts/decrypts the input data. Input data does not have to be multiple of block size.
 * Subsequent calls to this method are possible. Unless one or several calls of this function
 * have supplied sufficient input data no output is generated.
 * Input and output buffer may be the same (input data gets buffered block by block).
 *
 * @param          sessionHandle Handle of a running Cipher session.
 * @param          srcData       Reference to input data to be encrypted/decrypted.
 * @param          srcLen        Byte length of input data to be encrypted/decrypted.
 * @param          destData      Reference to result area.
 * @param [in,out] destLen       [in] Byte length of output buffer. [out] Byte length of generated output data.
 *
 * @returns TLAPI_OK                    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER        if one parameter is a null pointer (session is not being closed).
 * @returns E_TLAPI_INVALID_RANGE       if buffer is not within the drivers memory range (session is not being closed).
 * @returns E_TLAPI_CR_HANDLE_INVALID   if session invalid or not owned by req. client (session is not being closed).
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if during tlApiCipherInit() provided RSA padding is not available (function needs to check that input data does not exceed block size of RSA cipher).
 * @returns E_TLAPI_CR_WRONG_INPUT_SIZE  if [in]srcLen is inconsistent with algorithm requirements.
 * @returns E_TLAPI_CR_WRONG_OUTPUT_SIZE if [in]destLen is inconsistent with algorithm requirements.
 * @returns E_TLAPI_DRV_UNKNOWN          if some unknown error occurred.
 * @returns E_TLAPI_CR_INCONSISTENT_DATA if key type doesn't match the algorithm.
 * @returns E_TLAPI_UNMAPPED_BUFFER      if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiCipherUpdate(
    tlApiCrSession_t    sessionHandle,
    const uint8_t       *srcData,
    size_t              srcLen,
    uint8_t             *destData,
    size_t              *destLen);

/** Encrypts/decrypts the input data.
 * Processes data that has not been processed by previous calls
 * to tlApiCipherUpdate() as well as data supplied in srcData.
 * Completes the cipher session.
 * Afterwards the session is closed and sessionHandle invalid.
 * Input and output buffer may be the same (input data gets buffered block by block).
 *
 * @param          sessionHandle Handle of a running Cipher session.
 * @param          srcData       Reference to input data to be encrypted/decrypted.
 * @param          srcLen        Byte length of input data to be encrypted/decrypted.
 * @param          destData      Reference to result area.
 * @param [in,out] destLen       [in] Byte length of buffer for output data. [out] Byte length of generated output.
 *
 * @returns TLAPI_OK                    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER        if one parameter is a null pointer (session is not being closed).
 * @returns E_TLAPI_INVALID_RANGE       if buffer is not within the drivers memory range (session is not being closed).
 * @returns E_TLAPI_CR_HANDLE_INVALID   if session invalid or not owned by req. client (session is not being closed).
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if algorithm is not supported.
 * @returns E_TLAPI_CR_WRONG_OUTPUT_SIZE if [in]destLen is inconsistent with algorithm requirements.
 * @returns E_TLAPI_INVALID_INPUT        if RSA modulus length is invalid.
 * @returns E_TLAPI_DRV_UNKNOWN          if some unknown error occurred.
 * @returns E_TLAPI_CR_INCONSISTENT_DATA if algorithm could not work with the input data (e.g. wrong padding).
 * @returns E_TLAPI_UNMAPPED_BUFFER      if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiCipherDoFinal(
    tlApiCrSession_t    sessionHandle,
    const uint8_t       *srcData,
    size_t              srcLen,
    uint8_t             *destData,
    size_t              *destLen);

/** Initializes a new signature session and returns the handle for the new session for further
 * usage.
 * The session is associated with the key. Mode and algorithm type are set.
 * If this method does not return with TLAPI_OK then there is no valid handle value.
 * No crypto session is opened in case of an error.
 *
 * @param [out] pSessionHandle Reference to generated Signatures session handle (undefined in case of error).
 * @param       key            Key for this session. Key data is not copied but stored as reference.
 *                             Must maintain unchanged during session!
 * @param       mode           TLAPI_MODE_SIGN or TLAPI_MODE_VERIFY.
 * @param       alg            see enum of algorithms.
 *
 * @returns TLAPI_OK                        if operation was successful.
 * @returns E_TLAPI_NULL_POINTER            if one parameter is a null pointer.
 * @returns E_TLAPI_INVALID_RANGE           if buffer is not within the drivers memory range.
 * @returns E_TLAPI_INVALID_INPUT           if provided mode is unknown.
 * @returns E_TLAPI_CR_OUT_OF_RESOURCES     if no more session could be created.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if algorithm is not supported.
 * @returns E_TLAPI_CR_INCONSISTENT_DATA    if hash operation failed or key type doesn't match the algorithm.
 * @returns E_TLAPI_DRV_UNKNOWN             if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER         if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiSignatureInit(
    tlApiCrSession_t    *pSessionHandle,
    const tlApiKey_t    *key,
    tlApiSigMode_t      mode,
    tlApiSigAlg_t       alg);

/** Initializes a new signature session.
 * A handle for the new session is generated. The session is associated with the key.
 * Mode and algorithm type are set.
 * If this method does not return with TLAPI_OK then there is no valid handle value.
 * No crypto session is opened in case of an error.
 *
 * @note If the buffer/bufferLen contains invalid or inconsistent data (wrong length or length = 0,
 *       null, ...) a default value = 0 is taken and NO error is returned.
 *
 * @note If the used algorithm doesn't use additional algorithm specific data, the given values are
 *       ignored and don't result in an error.
 *
 * @note For PSS signatures and verification bufferLen is interpreted as salt length, and buffer may be NULL.
 *
 * @note For TLAPI_SIG_RSA_SHA_ISO9796_MR verify this function begins the verification sequence by recovering
 *       the message encoded within the signature itself and initializing the internal hash function.
 *       Therefore, the signature data needs to be provided in the buffer!
 *
 * @note MobiCore supports up to 2048 bit RSA keys as standard feature.
 *
 * @param [out]	pSessionHandle Reference to generated Signatures session handle (undefined in case of error).
 * @param 		key            Key for this session. Key data is not copied but stored as reference.
 *                             Must maintain unchanged during session!
 * @param 		mode           TLAPI_MODE_SIGN or TLAPI_MODE_VERIFY.
 * @param 		alg            see enum of algorithms.
 * @param 		buffer         Reference to algorithm specific data like seed for hash or salt for PSS.
 * @param 		bufferLen      Length of buffer containing algorithm specific data.
 *
 * @returns TLAPI_OK                if operation was successful.
 * @returns E_TLAPI_NULL_POINTER    if one parameter is a null pointer.
 * @returns E_TLAPI_INVALID_RANGE   if buffer is not within the drivers memory range.
 * @returns E_TLAPI_INVALID_INPUT           if provided mode is unknown.
 * @returns E_TLAPI_CR_OUT_OF_RESOURCES     if no more session could be created.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if algorithm is not supported.
 * @returns E_TLAPI_CR_INCONSISTENT_DATA    if hash operation failed or key type doesn't match the algorithm.
 * @returns E_TLAPI_DRV_UNKNOWN             if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiSignatureInitWithData(
    tlApiCrSession_t    *pSessionHandle,
    const tlApiKey_t    *key,
    tlApiSigMode_t      mode,
    tlApiSigAlg_t       alg,
    const uint8_t       *buffer,
    size_t              bufferLen);

/** Accumulates data for a signature calculation.
 * Input data does not have to be multiple of blocksize.
 * Subsequent calls of this method are possible.
 * tlApiSignatureSign() or tlApiSignatureVerify() have to be called to complete
 * the signature operation.
 *
 * @param sessionHandle Handle of a running Signature session.
 * @param message       Reference to message to be signed/verified.
 * @param messageLen    Byte length of message.
 *
 * @returns TLAPI_OK                    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER        if one parameter is a null pointer (session is not being closed).
 * @returns E_TLAPI_INVALID_RANGE       if buffer is not within the drivers memory range (session is not being closed).
 * @returns E_TLAPI_CR_HANDLE_INVALID   if session invalid or not owned by req. client (session is not being closed).
 * @returns E_TLAPI_CR_WRONG_OUTPUT_SIZE
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if algorithm is not supported.
 * @returns E_TLAPI_CR_INCONSISTENT_DATA    if there was a problem with the algorithm.
 * @returns E_TLAPI_DRV_UNKNOWN         if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER     if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiSignatureUpdate(
    tlApiCrSession_t    sessionHandle,
    const uint8_t       *message,
    size_t              messageLen);

/** Signs the message.
 * Finishes the signature session. Afterwards the session is closed and sessionHandle invalid.
 * message pointer may be NULL if messageLen = 0.
 *
 * @param          sessionHandle Handle of a running Signature session.
 * @param          message       Reference to message to be signed.
 * @param          messageLen    Byte length of message.
 * @param [in,out] signature     Reference to generated signature.
 * @param [in,out] signatureLen  [in] Byte length of signature buffer. [out] Byte length of generated signature.
 *
 * @returns TLAPI_OK                    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER        if one parameter is a null pointer (session is not being closed).
 * @returns E_TLAPI_INVALID_RANGE       if buffer is not within the drivers memory range (session is not being closed).
 * @returns E_TLAPI_CR_HANDLE_INVALID   if no valid handle was provided (session is not being closed).
 * @returns E_TLAPI_CR_OUT_OF_RESOURCES if required subsession could not be created.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if algorithm is not supported.
 * @returns E_TLAPI_CR_WRONG_OUTPUT_SIZE        if [in]signatureLen is too short.
 * @returns E_TLAPI_INVALID_INPUT
 * @returns E_TLAPI_DRV_UNKNOWN         if some unknown error occurred.
 * @returns E_TLAPI_CR_INCONSISTENT_DATA    if the crypto library could not calculate a signature.
 * @returns E_TLAPI_UNMAPPED_BUFFER     if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiSignatureSign(
    tlApiCrSession_t    sessionHandle,
    const uint8_t       *message,
    size_t              messageLen,
    uint8_t             *signature,
    size_t              *signatureLen);

/** Generates a signature for the supplied message and compares it to the supplied signature.
 * The generated signature is not presented to the caller.
 * Finishes the signature session. Afterwards the session is closed and sessionHandle invalid.
 * Message pointer may be NULL if messageLen = 0. Null pointer ex
 *
 * @param sessionHandle Handle of a running Signature session.
 * @param message       Reference to message to be verified.
 * @param messageLen    Byte length of message.
 * @param signature     Reference to signature to be verified.
 * @param signatureLen  Byte length of signature.
 * @param validity      Reference to verification result. TRUE if verified, otherwise FALSE.
 *
 * @returns TLAPI_OK                    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER        if one parameter is a null pointer (session is not being closed).
 * @returns E_TLAPI_INVALID_RANGE       if buffer is not within the drivers memory range (session is not being closed).
 * @returns E_TLAPI_CR_HANDLE_INVALID   if no valid handle was provided (session is not being closed).
 * @returns E_TLAPI_CR_OUT_OF_RESOURCES if required subsession could not be created.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if algorithm is not supported.
 * @returns E_TLAPI_CR_WRONG_OUTPUT_SIZE        if [in]signatureLen is too short.
 * @returns E_TLAPI_INVALID_INPUT
 * @returns E_TLAPI_DRV_UNKNOWN         if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER     if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiSignatureVerify(
    tlApiCrSession_t    sessionHandle,
    const uint8_t       *message,
    size_t              messageLen,
    const uint8_t       *signature,
    size_t              signatureLen,
    bool                *validity);

/** Initializes a new Message Digest session.
 * A handle for the new session is generated. The algorithm type is set.
 * If this method does not return with TLAPI_OK then there is not valid handle returned.
 * No crypto session is opened in case of an error.
 *
 * @param pSessionHandle Reference to generated Message Digest session handle (undefined in case of error).
 * @param algorithm      See enum mdAlg_t.
 *
 * @returns TLAPI_OK    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER    if one parameter is a null pointer.
 * @returns E_TLAPI_CR_OUT_OF_RESOURCES if no more session could be created.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if algorithm is not supported.
 * @returns E_TLAPI_CR_INCONSISTENT_DATA if hash operation failed.
 * @returns E_TLAPI_DRV_UNKNOWN         if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiMessageDigestInit(
    tlApiCrSession_t    *pSessionHandle,
    tlApiMdAlg_t        algorithm);

/** Initializes a new Message Digest session.
 * A handle for the new session is generated. The algorithm type is set.
 * Initializes a hash algorithm with a specified initialization vector. The initialization vector and the length of the
 * previously hashed data needs to be provided to the function in big endian format.
 * This may be used to calculate a part of the hash outside of the MobiCore TEE and then finish
 * the hash in the secure world.
 * If this method does not return with TLAPI_OK then there is no valid handle value.
 * No crypto session is opened in case of an error.
 *
 * @param pSessionHandle Reference to generated Message Digest session handle (undefined in case of error).
 * @param alg           See mdAlg_t.
 * @param buffer        Reference to previously calculated hash data.
 * @param lengthOfDataHashedPreviously     Byte array in big endian format containing length of previously calculated hash.
 *
 * @returns TLAPI_OK    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER    if one parameter is a null pointer.
 * @returns E_TLAPI_CR_OUT_OF_RESOURCES if no more session could be created.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if algorithm is not supported.
 * @returns E_TLAPI_CR_INCONSISTENT_DATA if hash operation failed.
 * @returns E_TLAPI_DRV_UNKNOWN         if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiMessageDigestInitWithData(
    tlApiCrSession_t    *pSessionHandle,
    tlApiMdAlg_t        alg,
    const uint8_t       *buffer,
    const uint8_t       lengthOfDataHashedPreviously[8]);

/** Accumulates message data for hashing.
 * The message does not have to be blocksize aligned. Subsequent calls to this method are possible.
 *
 * @param sessionHandle Handle of a running session Message Digest session.
 * @param message       Reference to message to be hashed.
 * @param messageLen    Byte length of input data to be hashed.
 *
 * @returns TLAPI_OK    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER    if one parameter is a null pointer (session is not being closed).
 * @returns E_TLAPI_INVALID_RANGE       if buffer is not within the drivers memory range (session is not being closed).
 * @returns E_TLAPI_CR_HANDLE_INVALID   if no valid handle was provided. (session is not being closed).
 * @returns E_TLAPI_CR_INCONSISTENT_DATA if hash operation failed.
 * @returns E_TLAPI_DRV_UNKNOWN         if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER     if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiMessageDigestUpdate(
    tlApiCrSession_t    sessionHandle,
    const uint8_t       *message,
    size_t              messageLen);

/** Hashes the message.
 * Finishes the message digest session.
 * Afterwards the session is closed and sessionHandle invalid.
 *
 * @param          sessionHandle Handle of a running session Message Digest session.
 * @param          message       Reference to message to be hashed.
 * @param          messageLen    Byte length of message.
 * @param          hash          Reference to generated hash.
 * @param [in,out] hashLen       [in] Byte length of hash buffer. [out] Byte length of generated hash data.
 *
 * @returns TLAPI_OK    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER    if one parameter is a null pointer (session is not being closed).
 * @returns E_TLAPI_INVALID_RANGE       if buffer is not within the drivers memory range (session is not being closed).
 * @returns E_TLAPI_CR_HANDLE_INVALID   if no valid handle was provided. (session is not being closed).
 * @returns E_TLAPI_CR_INCONSISTENT_DATA if hash operation failed.
 * @returns E_TLAPI_DRV_UNKNOWN         if some unknown error occurred.
 * @returns E_TLAPI_UNMAPPED_BUFFER     if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiMessageDigestDoFinal(
    tlApiCrSession_t    sessionHandle,
    const uint8_t       *message,
    size_t              messageLen,
    uint8_t             *hash,
    size_t              *hashLen);

/** Generates random data.
 *
 * @param alg           See enum randomDataGenerationAlg_t.
 * @param randomBuffer  Reference to generated random data.
 * @param [in,out] randomLen    [in] Byte length of desired random length.
 *                              [out] Byte length of generated random data.
 *
 * @returns TLAPI_OK    if operation was successful.
 * @returns E_TLAPI_NULL_POINTER    if one parameter is a null pointer.
 * @returns E_TLAPI_INVALID_RANGE   if buffer is not within the drivers memory range.
 * @returns E_TLAPI_CR_ALGORITHM_NOT_AVAILABLE  if algorithm is unknown.
 * @returns E_TLAPI_DRV_UNKNOWN     for any other errors.
 * @returns E_TLAPI_UNMAPPED_BUFFER if one buffer is not entirely mapped in Trustlet address space.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiRandomGenerateData(
    tlApiRngAlg_t   alg,
    uint8_t         *randomBuffer,
    size_t          *randomLen);

#endif // __TLAPICRYPTO_H__

/** @} */
