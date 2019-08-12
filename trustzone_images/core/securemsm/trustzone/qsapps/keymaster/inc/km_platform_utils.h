/* =================================================================================
 *  Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 * =================================================================================
 */
#ifndef KM_PLATFORM_UTILS_H_
#define KM_PLATFORM_UTILS_H_

/*
 * Included Files
 */
#include <comdef.h>
#include <IxErrno.h>
#include "qsee_core.h"
#include "qsee_cipher.h"
#include "qsee_crypto.h"
#include "qsee_prng.h"
#include "qsee_hmac.h"
#include "qsee_heap.h"
#include "qsee_hash.h"
#include "qsee_log.h"
#include "qsee_fs.h"
#include "qsee_sfs.h"
#include "qsee_services.h"
#include "qsee_timer.h"
#include "qsee_kdf.h"
#include "qsee_rsa.h"
#include "qsee_time.h"
#include "qsee_uf_aes.h"
#include "libstd/stringl/stringl.h"
#include "qsee_stor.h"
#include "qsee_fuse.h"
#include "qsee_ecc.h"

/*
 * Pre-processor Definitions
 */
/*
 * Pre-processor Definitions
 */
#define ENABLE_DEBUG 1
#ifdef ENABLE_DEBUG
#define KM_LOG_ERROR(xx_fmt, ...) QSEE_LOG(QSEE_LOG_MSG_ERROR, #xx_fmt, ##__VA_ARGS__)
#define KM_LOG_FATAL(xx_fmt, ...) QSEE_LOG(QSEE_LOG_MSG_FATAL, #xx_fmt, ##__VA_ARGS__)
#define KM_LOG_DEBUG(xx_fmt, ...) QSEE_LOG(QSEE_LOG_MSG_DEBUG, #xx_fmt, ##__VA_ARGS__)
#else
#define KM_LOG_ERROR(xx_fmt, ...)
#define KM_LOG_FATAL(xx_fmt, ...)
#define KM_LOG_DEBUG(xx_fmt, ...)
#endif

#define KM_MAX_OPERATIONS_ALLOWED 20
#define APPS_SECURE_STATE_MASK 0x05
#define MAX_FILE_NAME 256

typedef struct _km_hmac_ctx km_hmac_ctx;
typedef struct _km_cipher_ctx km_cipher_ctx;
typedef struct _km_hash_ctx km_hash_ctx;

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
typedef uint32_t KM_BLONG;
#define KM_MAX_KEY_SIZE          4128                                              ///< Maximum key size in bits
#define KM_BLONG_SIZE            sizeof(KM_BLONG)                                ///< Bytes per digit
#define KM_BN_BITS_PER_WORD      (KM_BLONG_SIZE * 8)                             ///< Bit per digit
#define KM_BLONGS_PER_KEY        (KM_MAX_KEY_SIZE + 8*KM_BLONG_SIZE - 1)/(8*KM_BLONG_SIZE)  ///< Digits per key

#define KM_MAX_PUB_EXP_SIZE      5
#define KM_ECDSA_NO_PAD_BUF_LEN (2 * 1024)

#define MAX_DER_HEADER_LEN   32
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/



/** hash descriptor */
typedef struct PKCS1_DER_ENC_DESC_
{
   /** name of hash */
   // char *name;
   /** Size of digest in octets */
   unsigned long hashsize;
   /** Input block size in octets */
   unsigned long blocksize;
   /** ASN.1 OID */
   unsigned long OID[16];
   /** Length of OID */
   unsigned long OIDlen;
   /**DER encoding of the DigestInfo header **/
   unsigned char DERheader[MAX_DER_HEADER_LEN];
   /** DigestInfo header Length of DER encoding */
   unsigned long DERheaderlen;
} PKCS1_DER_ENC_DESC;

typedef struct
{
    KM_BLONG a[KM_BLONGS_PER_KEY];
    int n;
} KM_BigInt;

/** KM_S_BIGINT type */
typedef struct KM_S_BIGINT_
{
    KM_BigInt bi;
    int sign;
} KM_S_BIGINT;

/** RSA PKCS key */
typedef struct KM_RSA_KEY
{
    /** Type of key, KM_RSA_KEY_PUBLIC for encryption or
     *  KM_RSA_KEY_PRIVATE/KM_RSA_KEY_PRIVATE_CRT for
     *  decryption or KM_RSA_KEY_PRIVATE_PUBLIC or
     *  KM_RSA_KEY_PRIVATE_CRT_PUBLIC for key generation */
    int type;
    /** RSA key bit length **/
    int bitLength;
    /** The public exponent */
    KM_S_BIGINT *e;
    /** The private exponent */
    KM_S_BIGINT *d;
    /** The modulus */
    KM_S_BIGINT *N;
    /** The p factor of N */
    KM_S_BIGINT *p;
    /** The q factor of N */
    KM_S_BIGINT *q;
    /** The 1/q mod p CRT param */
    KM_S_BIGINT *qP;
    /** The d mod (p - 1) CRT param */
    KM_S_BIGINT *dP;
    /** The d mod (q - 1) CRT param */
    KM_S_BIGINT *dQ;
} KM_RSA_KEY;

/** KM RSA Key Type  */
typedef enum
{
    /**used for ce_rsa_encrypt and ce_rsa_decrypt and other related api's */
    /** public key  */
    KM_RSA_KEY_PUBLIC = 0,
    /** private key in non CRT representation */
    KM_RSA_KEY_PRIVATE = 1,
    /** private key in CRT representation */
    KM_RSA_KEY_PRIVATE_CRT = 2,
    /**Used only for ce_rsa_key_gen*/
    /** private/public key pair */
    KM_RSA_KEY_PRIVATE_PUBLIC = 3,
    /** private CRT/public key pair */
    KM_RSA_KEY_PRIVATE_CRT_PUBLIC = 4,
    KM_RSA_KEY_INVALID = 0x7FFFFFFF,
} KM_RSA_KEY_TYPE;

/** KM RSA Key Size Macro */
#define KM_RSA_KEY_SIZE(key) ((key)->bi.n * KM_BLONG_SIZE)

/** Public Key algorithms in pkcs8 */
typedef enum
{
    KM_PKCS8_RSA_ENCRYPTION = 0,
    KM_PKCS8_ALGO_MAX,
    KM_PKCS8_INVALID = 0x7FFFFFFF,

} KM_PKCS8_ALGO_TYPE;

/** RSA private key parameters */
/** refer to Sect7.2, PKCS#1: RSA Encryption Standard */
typedef struct
{
    uint8 version;
    uint16 mod_len; /** modulus: n               */
    uint8* mod_data;
    uint16 pub_exp_len; /** public exponent: e       */
    uint8* pub_exp_data;
    uint16 priv_exp_len; /** private exponent: d      */
    uint8* priv_exp_data;
    uint16 prime1_len; /** prime 1: p               */
    uint8* prime1_data;
    uint16 prime2_len; /** prime 2: q               */
    uint8* prime2_data;
    uint16 exp1_len; /** exponent 1: d mod (p-1)  */
    uint8* exp1_data;
    uint16 exp2_len; /** exponent 1: d mod (q-1)  */
    uint8* exp2_data;
    uint16 coef_len; /** coefficient: q**{-1} mod p   */
    uint8* coef_data;

} KM_PKCS8_RSA_PRIVKEY_TYPE;

/** Union of all the private key types */
typedef struct
{
    uint8 ver;
    KM_PKCS8_ALGO_TYPE algo;
    union
    {
        KM_PKCS8_RSA_PRIVKEY_TYPE rsa;
    } key;
} KM_PKCS8_PRIVKEY_TYPE;

typedef struct KM_RSA_OAEP_PAD_INFO
{
    /** index of Hash & Mask generation function desired */
    int hashidx;
    /** Label to add to the message */
    unsigned char *label;
    /** Length of label */
    int labellen;
} KM_RSA_OAEP_PAD_INFO;

typedef struct KM_RSA_PSS_PAD_INFO
{
    /** index of Hash & Mask generation function desired */
    int hashidx;
    /** Length of salt */
    int saltlen;
} KM_RSA_PSS_PAD_INFO;

typedef enum
{
    KM_CIPHER_PARAM_KEY = 0,
    KM_CIPHER_PARAM_IV,
    KM_CIPHER_PARAM_MODE,
    KM_CIPHER_PARAM_PAD,
    KM_CIPHER_PARAM_NONCE,
    KM_CIPHER_PARAM_XTS_KEY,
    KM_CIPHER_PARAM_XTS_DU_SIZE,
    KM_CIPHER_PARAM_CCM_PAYLOAD_LEN,
    KM_CIPHER_PARAM_CCM_MAC_LEN,
    KM_CIPHER_PARAM_CCM_HDR_LEN,
    KM_CIPHER_PARAM_INVALID = 0x7FFFFFFF,
} KM_CIPHER_PARAM_ET;

/**
 * Enum for Cipher parameters
 */
typedef enum
{
    KM_SW_CIPHER_PARAM_DIRECTION = 0x01,
    KM_SW_CIPHER_PARAM_KEY = 0x02,
    KM_SW_CIPHER_PARAM_IV = 0x03,
    KM_SW_CIPHER_PARAM_MODE = 0x04,
    KM_SW_CIPHER_PARAM_BLOCKSIZE = 0x05,
    KM_SW_CIPHER_PARAM_KEY_SIZE = 0x06,
    KM_SW_CIPHER_PARAM_IV_SIZE = 0x07,
    KM_SW_CIPHER_PARAM_AAD = 0x08,     //Additional plaintext data for AES-GCM
    KM_SW_CIPHER_PARAM_TAG = 0x09,     //Calculated TAG by AES-GCM
    KM_SW_CIPHER_PARAM_MAX = 0x7FFFFFFF
} KM_SW_CipherParam;

typedef enum
{
    KM_HASH_PARAM_MODE = 0,
    KM_HASH_PARAM_HMAC_KEY = 1,
    KM_HASH_PARAM_SEQ = 2,
    KM_HASH_PARAM_INVALID = 0x7FFFFFFF,
} KM_HASH_PARAM_ET;

typedef enum
{
    KM_HASH_MODE_HASH = 0, // Plain SHA
    KM_HASH_MODE_HMAC = 1,  // HMAC SHA
    KM_HASH_MODE_INVALID = 0x7FFFFFFF,
} KM_HASH_MODE_ET;

typedef enum
{
    KM_AES128_KEY_SIZE = 128,
    KM_AES192_KEY_SIZE = 192,
    KM_AES256_KEY_SIZE = 256,
    /* Add more algorithms*/

    KM_ECDSA224_KEY_SIZE = 224,
    KM_ECDSA256_KEY_SIZE = 256,
    KM_ECDSA384_KEY_SIZE = 384,
    KM_ECDSA521_KEY_SIZE = 521,
    /* If adding an algorithm, increment the below value as well */
    MAX_RSA_KEY_SIZE = 4096,
    MAX_ECDSA_KEY_SIZE = 528,
    KM_KEY_SIZE_MAX = 7,

} KM_KEY_SIZE;

typedef enum
{
    KM_SHA1_HASH_SZ = 160,
    KM_SHA256_HASH_SZ = 256,
} KM_HASH_SIZE;

typedef struct _km_rsa_key_blob_type_t
{
    uint32_t modulus; /* Modulus (N) array [MSB...LSB] */
    uint32_t modulus_size; /* Modulus array length */
    uint32_t public_exponent; /* public exponent (e) array [MSB...LSB] */
    uint32_t public_exponent_size; /* public exponent array length */
    uint32_t private_exponent; /* Private Exponent (d) array [MSB...LSB] */
    uint32_t private_exponent_size; /* Private Exponent array length */
}__attribute__ ((packed)) km_rsa_key_blob_type_t;

typedef enum
{
    KM_AES_IV_SIZE = 16,
    KM_AES_GCM_IV_SIZE = 12,
    KM_AES_BLOCK_SIZE = 16,

    /* If adding an algorithm, increment the below value as well */
    KM_IV_SIZE_MAX = 1,
} KM_IV_SIZE;

#define KM_ECC_MAX_MODULUS_BITS (521 + 8)
#define KM_ECC_BIGNUM_WORDS ((KM_ECC_MAX_MODULUS_BITS + 24 + 31) / 32)

typedef struct
{
    char *m;
    char *a;
    char *b;
    char *G_x;
    char *G_y;
    char *n;
    uint32 key_size;         //size in bits
    uint32 h; //cofactor
}__attribute__ ((packed)) generic_ecc_params;

typedef struct
{
    unsigned flags;
    size_t allocated_words;
    size_t num_words;  // used words in bignums
    size_t modulus_bits;
    uint32_t *modulus;
    uint32_t *montgomery_R;
    uint32_t *montgomery_R2;
    uint32_t m_inv32; // used in computing qi for montgomery multiplication
    // int hi;  // The msb of the modulus lies in the lower 8 bits of m[hi]
    // or the upper 24 bits of m[hi - 1].
    // i.e. hi = (modulusbits + 24) / 32
    int quo_num_shift;  // Number of bits to shift {a[hi], a[hi-1]} in reduce()
                        // so that the binary point is just left of bit 7.
                        // i.e. quo_num_shift = modulusbits + 24 - 32 * hi
    uint64_t m_recip;  // Assuming binary point of modulus just left of
} km_qrlbn_modulus_data_t;

typedef struct
{
    uint32_t data[KM_ECC_BIGNUM_WORDS];
} km_qrlbn_ecc_bigval_t;

typedef struct
{
    km_qrlbn_ecc_bigval_t X;
    km_qrlbn_ecc_bigval_t Y;
    km_qrlbn_ecc_bigval_t Z;
    int flags;
} km_qrlbn_ecc_point_t;

typedef struct
{
    km_qrlbn_modulus_data_t md;
    km_qrlbn_ecc_bigval_t modulus_storage;
    km_qrlbn_ecc_bigval_t montgomery_R_storage;
    km_qrlbn_ecc_bigval_t montgomery_R2_storage;
} km_qrlbn_ecc_modulus_data_t;

typedef struct
{
    km_qrlbn_ecc_bigval_t x;
    km_qrlbn_ecc_bigval_t y;
    int flags;
} km_qrlbn_ecc_affine_point_t;

typedef struct
{
    int version;
    km_qrlbn_ecc_modulus_data_t modulus;
    km_qrlbn_ecc_modulus_data_t base_point_order;
    km_qrlbn_ecc_bigval_t a;  // montgomery
    km_qrlbn_ecc_bigval_t b;  // montgomery
    km_qrlbn_ecc_bigval_t a_mont;  // montgomery
    km_qrlbn_ecc_bigval_t b_mont;  // montgomery
    km_qrlbn_ecc_affine_point_t affine_base_point;
    km_qrlbn_ecc_point_t base_point; // montgomery
    km_qrlbn_ecc_bigval_t ts_z;  // montgomery, for Tonelli Shanks sqrt
    uint64_t cofactor;
} km_qrlbn_ecc_domain_t;

typedef struct
{
    km_qrlbn_ecc_bigval_t r;
    km_qrlbn_ecc_bigval_t s;
} km_qrlbn_ECDSA_sig_t;

typedef enum
{
    km_qrlbn_tag_m,       // limiteed to 32 * modulus_words - 24 bits
    km_qrlbn_tag_X,       // limited to 32 * modulus_words - 24 bits
    km_qrlbn_tag_Y,       // limited to 32 * modulus_words - 24 bits
    km_qrlbn_tag_a,       // limited to 32 * modulus_words - 24 bits
    km_qrlbn_tag_b,       // limited to 32 * modulus_words - 24 bits
    km_qrnbn_tag_n,      // limited to 32 * base_point_order_num_words - 24 bits
    km_qrlbn_tag_privkey, // 32 * base_point_order_num_words - 24 bits
    km_qrlbn_tag_hash,    // no limit
    km_qrlbn_tag_r,       // 32 * base_point_order_num_words - 24 bits
    km_qrlbn_tag_s,       // 32 * base_point_order_num_words -24 bits
    km_qrlbn_tag_p,
    km_qrlbn_tag_q,
} km_qrlbn_field_tag_t;

typedef struct _km_ecdsa_key_blob_type_t
{
    uint32_t keysize;
    uint32_t privkey;
    uint32_t privkey_size;
    uint32_t pubkey_x;
    uint32_t pubkey_x_size;
    uint32_t pubkey_y;
    uint32_t pubkey_y_size;
} km_ecdsa_key_blob_type_t;

typedef struct _km_ecdsa_sig_format_t
{
    uint32_t sig_r_offset;
    uint32_t sig_r_len;
    uint32_t sig_s_offset;
    uint32_t sig_s_len;
}__attribute__ ((packed))  km_ecdsa_sig_format_t;
/*
 * Public Data
 */

/*
 * Inline Functions
 */

/*
 * Public Function Prototypes
 */

uint32_t
keymaster_get_derived_key_size(uint8 key_type);
uint8 *
keymaster_get_derived_key(uint8 key_type);
void*
km_malloc(size_t size);
void
km_free(void *ptr);
size_t
km_memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);
void *
km_memset(void *dst, uint32_t value, uint32_t size);
int
km_memcmp(uint8* a, uint8* b, uint32_t len);

/**
 * @brief  This function will create a message digest hash using the
 *         algorithm specified.
 *
 * @param hash[in]         The hash algorithm
 * @param msg[in]          The message to hash
 * @param msg_len[in]      The length of the message
 * @param digest[in,out]   The digest to store
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
int
km_hash(keymaster_digest_t alg, const uint8 *msg, uint32_t msg_len,
                uint8 *digest, uint32_t digest_len);

/**
 * @brief Intialize a hash context for update and finish functions.
 *
 * @param alg[in] The algorithm standard to use
 * @param hash_ctx[out] The hash context
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_hash_init(km_hash_ctx *hash_ctx);
/**
 * @brief Resets hash/hmac context, will not reset keys.
 *
 * @param hash_ctx[in] The hash context
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_hash_reset(km_hash_ctx *hash_ctx);

/**
 * @brief Resets hash/hmac context, will not reset keys.
 *
 * @param hash_ctx[in] The hash context
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_hash_reset(km_hash_ctx *hash_ctx);

/**
 * @brief  This function will hash some data into the hash context
 *         structure, which Must have been initialized by
 *         qsee_hash_init().
 *
 * @param hash_ctx[in]    The hash context
 * @param[in]             Pointer to the msg to hash
 * @param[in]             Length of the msg to hash
 *
 * @return 0 on success, negative on failure
 *
 * @see qsee_hash_init
 */
int
km_hash_update(km_hash_ctx *hash_ctx, const uint8 *msg, uint32_t msg_len);

/**
 * @brief  Compute the digest hash value
 *
 * @param hash_ctx[in]     The hash context
 * @param digest[in]       Pointer to output message digest hash
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 *
 * @see qsee_hash_init
 */
int
km_hash_final(km_hash_ctx *hash_ctx, uint8 *digest, uint32_t digest_len);

/**
 * @brief Release all resources with a given hash context.
 *
 * @param hash_ctx[in] Hash context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_hash_free_ctx(km_hash_ctx *hash_ctx);

/**
 * @brief Modify the parameters for a given hash/hmac operation.
 *
 * @param hash_ctx[in]    Hash context
 * @param param_id[in]    The parameter to modify
 * @param param[in]       The parameter value to set.
 * @param param_len[in]   The length of the param (in bytes).
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_hash_set_param(km_hash_ctx *hash_ctx, KM_HASH_PARAM_ET param_id,
                const void *param, uint32_t param_len);

/**
 * @brief  This function will perform a simultaneous Hash/Cipher
 *         Encrypt operation.
 *
 * @param cipher_ctx[in]   The cipher context
 * @param hash_ctx[in]     The hash context
 * @param pt[in]           The input plaintext buffer
 * @param pt_len[in]       The input plaintext buffer length
 * @param ct[in,out]       The output ciphertext buffer
 * @param ct_len[in]       The output ciphertext buffer length.
 * @param digest[in,out]   The digest to store
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
int
km_hashcipher_encrypt(km_cipher_ctx *cipher_ctx, km_hash_ctx *hash_ctx,
                uint8 *pt, uint32_t pt_len, uint8 *ct, uint32_t ct_len,
                uint8 *digest, uint32_t digest_len);

/**
 * @brief  This function will perform a simultaneous Hash/Cipher
 *         Decrypt operation.
 *
 * @param cipher_ctx[in]   The cipher context
 * @param hash_ctx[in]     The hash context
 * @param ct[in]           The input ciphertext buffer
 * @param ct_len[in]       The input ciphertext buffer length.
 * @param pt[in,out]       The output plaintext buffer
 * @param pt_len[in]       The output plaintext buffer length
 * @param digest[in,out]   The digest to store
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
int
km_hashcipher_decrypt(const km_cipher_ctx *cipher_ctx,
                const km_hash_ctx *hash_ctx, uint8 *ct, uint32_t ct_len,
                uint8 *pt, uint32_t pt_len, uint8 *digest, uint32_t digest_len);

/**
 * @brief Intialize a cipher context for encrypt/decrypt operation
 *
 * @param[in] alg  The algorithm standard to use
 * @param[out] cipher_ctx The cipher ctx
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_cipher_init(km_cipher_ctx *cipher_ctx);

/**
 * @brief Resets cipher context, will not reset key
 *
 * @param[in] cipher_ctx The cipher ctx
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_cipher_reset(km_cipher_ctx *cipher_ctx);

/**
 * @brief Release all resources with a given cipher context.
 *
 * @param[in] cipher_ctx Cipher context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_cipher_free_ctx(km_cipher_ctx *cipher_ctx);

/**
 * @brief Modify the parameters for a given cipher operation.
 *
 * @param[in] cipher_ctx  Cipher context
 * @param[in] param_id    The parameter to modify
 * @param[in] param       The parameter value to set.
 * @param[in] param_len   The length of the param (in bytes).
 *
 * @return 0 on success,
 * negative on failure,
 * -E_NOT_SUPPORTED if an alogirthm or parameter is not currently supported.
 *
 */
int
km_cipher_set_param(km_cipher_ctx *cipher_ctx, KM_CIPHER_PARAM_ET param_id,
                void *param, uint32_t param_len);

/**
 * @brief Retrieve the parameters for a given cipher context.
 *
 * @param[in] cipher_ctx  Cipher context
 * @param[in] param_id    The parameter to retrieve
 * @param[in] param       The memory location to store the parameter.
 * @param[in] param_len   The length of the param (in bytes).
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_cipher_get_param(km_cipher_ctx *cipher_ctx, KM_CIPHER_PARAM_ET param_id,
                void *param, uint32_t *param_len);

/**
 * @brief This function encrypts the passed plaintext message using
 *        the specified algorithm. The memory allocated for the
 *        ciphertext must be large enough to hold the plaintext
 *        equivalent. If the output buffer is not large enough to
 *        hold the encrypted results, an error is returned.
 *
 * @param[in] cipher_ctx         The cipher context to create
 * @param[in] pt                 The input plaintext buffer
 * @param[in] pt_len             The input plaintext buffer length (in bytes)
 * @param[in,out] ct             The output ciphertext buffer
 * @param[in,out] ct_len         The output ciphertext buffer length. This
 *                               is modified to the actual ct bytes written.
 *
 * @return E_SUCCESS if successful
 *         E_INVALID_ARG if not multiple of block length
 *         E_FAILURE otherwise
 */
int
km_cipher_encrypt(km_cipher_ctx *cipher_ctx, uint8 *pt, uint32_t pt_len,
                uint8 *ct, uint32_t *ct_len);

/**
 * @brief This function decrypts the passed ciphertext message using
 *        the specified algorithm. The memory allocated for the
 *        plaintext must be large enough to hold the ciphertext
 *        equivalent. If the output buffer is not large enough to
 *        hold the decrypted results, an error is returned.
 *
 * @param[in] cipher_ctx         The cipher context to create
 * @param[in] ct                 The input ciphertext buffer
 * @param[in] ct_len             The input ciphertext buffer length (in bytes)
 * @param[in,out] pt             The output plaintext buffer
 * @param[in,out] pt_len         The output plaintext buffer length. This
 *                               is modified to the actual pt bytes written.
 *
 * @return E_SUCCESS if successful
 *         E_INVALID_ARG if not multiple of block length
 *         E_FAILURE otherwise
 */
int
km_cipher_decrypt(km_cipher_ctx *cipher_ctx, uint8 *ct, uint32_t ct_len,
                uint8 *pt, uint32_t *pt_len);

/**
 * @brief Create a hash MAC per FIPS pub 198 using the specified hash algorithm.
 *
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 * @param[in] key             - Pointer to input key to HMAC algorithm
 * @param[in] key_len         - Length of input key in bytes
 * @param[out] msg_digest     - Pointer to message digest (memory provided by caller)
 *
 * @return 0 on success, negative on failure
 */
int
km_hmac(keymaster_digest_t alg, const uint8 *msg, uint32_t msg_len,
                const uint8 *key, uint16 key_len, uint8 *msg_digest);

/**
 * @brief Intialize a hmac context for update and finish
 *        functions.
 *
 * @param alg[in] The algorithm standard to use
 * @param hmac_ctx[out] The hmac context
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_hmac_init(km_hmac_ctx *hmac_ctx);

/**
 * @brief  This function will hmac some data into the hmac
 *         context structure, which Must have been initialized
 *         by qsee_hmac_init().
 *
 * @param hmac_ctx[in]    The hmac context
 * @param[in] msg         Pointer to the msg to hash
 * @param[in] msg_len     Length of the msg to hash
 *
 * @return 0 on success, negative on failure
 *
 * @see qsee_hmac_init
 */
int
km_hmac_update(km_hmac_ctx *hmac_ctx, uint8 *msg, uint32_t msg_len);

/**
 * @brief  Compute the digest hmac value
 *
 * @param hmac_ctx[in]     The hmac context
 * @param digest[in]       Pointer to output message digest hmac
 * @param digest_len[in]   Length of the output message digest
 *                         hmac buffer in bytes. Must be 20
 *                         bytes for SHA1 and 32 bytes for
 *                         SHA256
 *
 * @return 0 on success, negative on failure
 *
 * @see qsee_hmac_init
 */
int
km_hmac_final(km_hmac_ctx *hmac_ctx, uint8 *digest, uint32_t digest_len);

/**
 * @brief Release all resources with a given hmac context.
 *
 * @param hmac_ctx[in] hmac context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int
km_platform_hmac_free_ctx(km_hmac_ctx *hmac_ctx);

uint32_t
km_platform_prng_getdata(uint8 *out, uint32_t out_len);

boolean
km_platform_crypto_lock_engine(boolean lock);

/**
 * @brief kdf key derivation algorithm.
 *
 * @details The software is a 3-level stack.
 * The lowest level is AES.
 * The second level is the CMAC algorithm from NIST SP 800-38B.
 * The third level is the counter-based algorithm from NIST SP 800-108, named KDF in the implementation.
 *
 * The inputs are a key derivation key (key, key_len ), a label
 * (label, label_len), and a context (context, context_len).
 * The output will be output_len bytes long stored at output.
 * All sensitive data is zeroized before return..
 *
 * When key is set to NULL, key_len will be ignored.
 * Qsee will set a 32 bytes key; which is not accessible from outside Trustzone; as the input key.
 *
 *
 * @param[in] key           key derivation key pointer
 * @param[in] key_len       key derivation key pointer length in byte
 * @param[in] label         key derivation label pointer
 * @param[in] label_len     key derivation label pointer length in byte
 * @param[in] context       key derivation context pointer
 * @param[in] context_len   key derivation context pointer length in byte
 * @param[in/out] output    derived key pointer
 * @param[in] output_len    derived key pointer length in byte
 *
 * @return
 * The return value follows OpenSSL: 0 for success, -1 for errors like
 * null pointers where not allowed, -2 for invalid values or overflow.
 */
int
km_kdf(const void *key, size_t key_len, const void *label, size_t label_len,
                const void *context, size_t context_len, void* output,
                size_t output_len);

int
km_sw_cipher_pad(km_cipher_ctx *cipher_ctx, const uint8 *pt, uint32_t* pt_len);
int
km_sw_cipher_remove_pad(const uint8 *pt, uint32_t* pt_len);
/**
 * @brief     set crypto/bimc/snoc bandwidth
 * @return    E_SUCCESS if success, -E_FAILURE otherwise

 */
uint32_t
km_platform_set_bandwidth(void *reqClient, uint32_t reqClientlen,
                uint32_t res_req, uint32_t level, uint32_t flags);

int
KM_SW_Cipher_Init(km_cipher_ctx *cipher_ctx);
int
KM_SW_CipherData(km_cipher_ctx *cipher_ctx, uint8 *inbuf, uint32_t inbuflen,
                uint8 *outbuf, uint32_t outbuflen);
int
KM_SW_Cipher_SetParam(km_cipher_ctx *cipher_ctx, KM_SW_CipherParam nParamID,
                void *pParam, uint32_t cParam);
int
KM_SW_Cipher_GetParam(km_cipher_ctx *cipher_ctx, KM_SW_CipherParam nParamID,
                void *pParam, uint32_t cParam);
int
KM_SW_Cipher_DeInit(km_cipher_ctx *cipher_ctx);

int
KM_SW_UpdateAAD(km_cipher_ctx *cipher_ctx, uint8_t *aad, uint32 aad_len);

int
KM_SW_UpdateData(km_cipher_ctx *handle, uint8 *inbuf, uint32_t inbuflen,
                uint8 *outbuf, uint32_t outbuflen);
int
KM_SW_FinalData(km_cipher_ctx *handle, uint8 *inbuf, uint32_t inbuflen,
                uint8 *outbuf, uint32_t outbuflen);

int
km_sfs_open(const char* path, int flags);
int
km_sfs_close(int fd);
int
km_sfs_getSize(int fd, uint32* size);
int
km_sfs_read(int fd, char *buf, int nbytes);
int
km_sfs_write(int fd, const char *buf, int nbytes);
int
km_sfs_rm(const char *path);
int
km_sfs_seek(int fd, int32_t offset, int whence);

uint64_t
htonl(uint64_t t);

uint32_t
htonl32(uint32_t t);

uint64_t
ntohl(uint64_t t);

uint32_t
ntoh32(uint32_t t);

/* Crypto related functions */
#endif /* KM_PLATFORM_UTILS_H_ */
