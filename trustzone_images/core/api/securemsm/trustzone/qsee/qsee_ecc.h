#ifndef QSEE_ECC_H
#define QSEE_ECC_H

/**
@file qsee_ecc.h
@brief Provide ECC API wrappers
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
08/14/14   qxu     Added a new API: qsee_SW_ECDH_mult_basepoint
12/21/12   ah      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/**
 * Define either ECC_P256 or ECC_P192 to control which polynomial is used.
 * Of course, the binary must have been compiled with the same .h file
 * contents as are used by applications.
 *
 * Arguably, to avoid branching in version control systems, this
 * should be supplied by a -DP256 or -DP192 at compile time. It would be
 * good to include a run-time check to make sure the
 * library was compiled with the same modulus as the include file
 * specifies, but it seems require that the main "functions" be
 * macros, and I'm not sure I want to do that.
 */

#define ECC_P256

#ifdef ECC_P256
#define QSEE_BIGLEN 9
#endif

#ifdef ECC_P192
#define QSEE_BIGLEN 7
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct {
      /** The representation is little endian by word and native endian within each word.*/
      uint32 data[QSEE_BIGLEN];
} QSEE_bigval_t;


typedef struct {
      QSEE_bigval_t x;
      QSEE_bigval_t y;
      boolean infinity;
} QSEE_affine_point_t;

typedef struct {
      QSEE_bigval_t r;
      QSEE_bigval_t s;
} QSEE_ECDSA_sig_t;

/* Generic ECC parameters type */
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

#define QSEE_ECC_MAX_MODULUS_BITS (521 + 8) 
#define QSEE_ECC_BIGNUM_WORDS ((QSEE_ECC_MAX_MODULUS_BITS + 24 + 31) / 32)

typedef struct {
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
} QSEE_qrlbn_modulus_data_t;

typedef struct {
   uint32_t data[QSEE_ECC_BIGNUM_WORDS];
} QSEE_qrlbn_ecc_bigval_t;

typedef struct {
   QSEE_qrlbn_ecc_bigval_t X;
   QSEE_qrlbn_ecc_bigval_t Y;
   QSEE_qrlbn_ecc_bigval_t Z;
   int flags;
} QSEE_qrlbn_ecc_point_t;

typedef struct {
   QSEE_qrlbn_modulus_data_t md;
   QSEE_qrlbn_ecc_bigval_t modulus_storage;
   QSEE_qrlbn_ecc_bigval_t montgomery_R_storage;
   QSEE_qrlbn_ecc_bigval_t montgomery_R2_storage;
} QSEE_qrlbn_ecc_modulus_data_t;

typedef struct {
   QSEE_qrlbn_ecc_bigval_t x;
   QSEE_qrlbn_ecc_bigval_t y;
   int flags;
} QSEE_qrlbn_ecc_affine_point_t;

typedef struct {
  int version;
   QSEE_qrlbn_ecc_modulus_data_t modulus;
   QSEE_qrlbn_ecc_modulus_data_t base_point_order;
   QSEE_qrlbn_ecc_bigval_t a;
   QSEE_qrlbn_ecc_bigval_t b;
   QSEE_qrlbn_ecc_bigval_t a_mont;  // montgomery
   QSEE_qrlbn_ecc_bigval_t b_mont;  // montgomery
   QSEE_qrlbn_ecc_affine_point_t affine_base_point; 
   QSEE_qrlbn_ecc_point_t base_point; // montgomery
   QSEE_qrlbn_ecc_bigval_t ts_z;  // montgomery, for Tonelli Shanks sqrt
   uint64_t cofactor;
} QSEE_qrlbn_ecc_domain_t;

typedef struct {
   QSEE_qrlbn_ecc_bigval_t r;
   QSEE_qrlbn_ecc_bigval_t s;
} QSEE_qrlbn_ECDSA_sig_t;

typedef enum {
  QSEE_qrlbn_tag_m,       // limiteed to 32 * modulus_words - 24 bits
  QSEE_qrlbn_tag_X,       // limited to 32 * modulus_words - 24 bits
  QSEE_qrlbn_tag_Y,       // limited to 32 * modulus_words - 24 bits
  QSEE_qrlbn_tag_a,       // limited to 32 * modulus_words - 24 bits
  QSEE_qrlbn_tag_b,       // limited to 32 * modulus_words - 24 bits
  QSEE_qrnbn_tag_n,       // limited to 32 * base_point_order_num_words - 24 bits
  QSEE_qrlbn_tag_privkey, // 32 * base_point_order_num_words - 24 bits
  QSEE_qrlbn_tag_hash,    // no limit
  QSEE_qrlbn_tag_r,       // 32 * base_point_order_num_words - 24 bits
  QSEE_qrlbn_tag_s,       // 32 * base_point_order_num_words -24 bits
  QSEE_qrlbn_tag_p,
  QSEE_qrlbn_tag_q,
} QSEE_qrlbn_field_tag_t;  

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief generate public key and private key of ECC
 *        same key is used for ECDH and ECDSA
 *
 * @param pubkey       [out] pointer to ECC public key
 * @param privkey      [out] pointer to ECC private key
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_SW_ECC_PubPrivate_Key_generate(QSEE_affine_point_t *pubkey, QSEE_bigval_t *privkey);

/**
 * @brief generate shared key out of Alice' public key and Bob's private key
 *
 * @param shared_key   [out] shared key bewteen Alice and Bob
 * @param pubkey       [in] pointer to ECC public key
 * @param privkey      [in] pointer to ECC private key
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_SW_ECDH_Shared_Key_Derive(QSEE_affine_point_t *shared_key, QSEE_bigval_t *privkey, QSEE_affine_point_t *pubkey);

/**
 * @brief sign data with ECC private key
 *
 * @param msgdgst   [in] msg that needs to be signed
 * @param privkey   [in] pointer to private key to be used for signing
 * @param sig       [out] pointer to the buff that the signed message will be stored
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_SW_ECDSA_Sign(QSEE_bigval_t const *msgdgst, QSEE_bigval_t const *privkey, QSEE_ECDSA_sig_t *sig);

/**
 * @brief verify data with ECC public key
 *
 * @param msgdgst   [in] msg that needs to be authenticated
 * @param pubvkey   [in] pointer to public key to be used for signing
 * @param sig       [in] pointer to the buff that the signed message will be stored
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_SW_ECDSA_Verify(QSEE_bigval_t const *msgdgst, QSEE_affine_point_t const *pubkey, QSEE_ECDSA_sig_t const *sig);

/**
 * @brief generate the public key. the function multiplies the (non-exposed) base point 
 * by the large integer k and places the result in the affine point structure pointed to by P1
 *
 * @param P1   [out] pointer to public key 
 * @param k    [in]  pointer to private key
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_SW_ECDH_mult_basepoint(QSEE_affine_point_t *P1, const QSEE_bigval_t *k);


/**
 * @brief Converts a hash value to a bigval_t.
 *
 * @param tgt         [in] Pointer to destination buffer
 * @param hashp       [in] Pointer to hash buffer
 * @param hashlen     [in] Hash buffer size
 *
 * @return 
 *
 */
void qsee_ECC_hash_to_bigval(QSEE_bigval_t *tgt, void const *hashp, unsigned int hashlen);

/**
 * @brief inject error for testing 
 *
 * @param buf          [in] Data to inject error
 * @param len          [in] Length of data
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_toggle_random_bit(void *buf, size_t len);

/**
 * @brief software base random bytes generator
 *
 * @param buf         [in] Pointer to random bytes buffer
 * @param len         [in] Buffer size
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_get_random_bytes(void *buf, int len);

/**
 * @brief in_curveP
 *
 * @param P         [in] Pointer to affine point type var.
 *
 * @return TRUE on success, FALSE otherwise
 *
 */
boolean qsee_in_curveP(QSEE_affine_point_t const *P);

/**
 * @brief Generates a secret value, k, and a point, P1, to send to the other
 * party.  k will satisfy 1 <= k < order(P).
 *
 * @param P1   [out] pointer to a point 
 * @param k    [in]  pointer to a secret value
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_ECDH_generate(QSEE_affine_point_t *P1, QSEE_bigval_t *k);

/**
 * @brief ECDH_derive takes the point sent by the other party, and verifies
 * that it is a valid point.  If 1 <= k < orderP and the point is
 * valid, it stores the result in *tgt and returns B_TRUE.  If the
 * point is invalid it returns B_FALSE.
 *
 * @param tgt  [out]  pointer to X coordinate of the derived point
 * @param k    [in]   pointer to a secret value
 * @param Q    [in]   pointer to a point 
 *
 * @return TRUE if k and Q is valid, FALSE if k or Q is invalid
 *
 */
boolean qsee_SW_ECDH_derive(QSEE_bigval_t *tgt, QSEE_bigval_t const *k, QSEE_affine_point_t const *Q);

 /**
 * @Initializes a domain from curve hex strings and the cofactor.
 *
 * @param dp             [out] pointer to ECC domain context
 * @param modulus        [in]  pointer to modulus 
 * @param a              [in]  pointer to a 
 * @param b              [in]  pointer to b  
 * @param x              [in]  pointer to x  
 * @param y              [in]  pointer to y   
 * @param n              [in]  pointer to n    
 * @param cofactor       [in] cofactor    
 *
 * @return 0 on success, -1 on error
 *
 */

int qsee_SW_GENERIC_ECC_init(QSEE_qrlbn_ecc_domain_t * dp, char *modulus, char *a, 
	                         char* b, char *x, char *y, char *n, unsigned cofactor);

/**
 * @brief sign data with ECC private key
 *
 * @param msgdgst           [in]  pointer to msg that needs to be signed
 * @param msgdgst_len       [in]  msg length in Bytes
 * @param privkey           [in]  pointer to private key for signing
 * @param sig               [out] pointer to signed msg, which is signature
 * @param sig_len           [out] signature size in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECDSA_sign(uint8_t *msgdgst, 
                               uint32_t msgdgst_len, 
      	                       QSEE_qrlbn_ecc_bigval_t *privkey,                                   
                               QSEE_qrlbn_ECDSA_sig_t *  sig,
                               QSEE_qrlbn_ecc_domain_t *dp); 

/**
 * @brief verify data with ECC public key
 *
 * @param msgdgst           [in]  pointer to msg that needs to be signed
 * @param msgdgst_len       [in]  msg length in Bytes
 * @param pubkey            [in]  pointer to public key for verification
 * @param sig               [out] pointer to signed msg to be verfied
 * @param sig_len           [out] sig_len in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECDSA_verify(uint8_t *msgdgst, 
                                 uint32_t msgdgst_len, 
                                 QSEE_qrlbn_ecc_affine_point_t *pubkey, 
                                 QSEE_qrlbn_ECDSA_sig_t *sig,
                                 QSEE_qrlbn_ecc_domain_t *dp);

/**
 * @brief generate shared key out of Alice' public key and Bob's private key
 *
 * @param shared_key   [out] shared key bewteen Alice and Bob
 * @param pubkey       [in] pointer to ECC public key
 * @param privkey      [in] pointer to ECC private key
 *
 * @return 0 on success, -1 on error
 *
 */

int qsee_SW_GENERIC_ECDH_shared_key_derive(QSEE_qrlbn_ecc_bigval_t *shared_key, 
                                           QSEE_qrlbn_ecc_bigval_t *privkey, 
                                           QSEE_qrlbn_ecc_affine_point_t *pubkey, 
                                           QSEE_qrlbn_ecc_domain_t *dp);

/**
 * @brief generate public key and private key of ECC
 *
 * @param privkey              [out] pointer to private key 
 * @param pubkey               [out] pointer to public key
 * @param dp                   [in]  ECC domain context
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECC_keypair_generate(QSEE_qrlbn_ecc_bigval_t *privkey, 
                                         QSEE_qrlbn_ecc_affine_point_t  *pubkey, 
                                         QSEE_qrlbn_ecc_domain_t *dp);

 /**
 * @brief Convert data from Binary to Bigval
 *
 * @param dst              [out] pointer to destination data 
 * @param src              [in]  pointer to source data
 * @param srclen           [in]  length of source data in bytes 
 * @param dp               [in]  ECC domain context 
 * @param tag              [in]  Data type tag
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECC_binary_to_bigval(QSEE_qrlbn_ecc_bigval_t *dst, 
                                         const void *src, 
                                         size_t srclen,
                                         const QSEE_qrlbn_ecc_domain_t *dp, 
                                         QSEE_qrlbn_field_tag_t tag);

/**
 * @brief Convert data from Bigval to Binary
 *
 * @param dst              [out] pointer to destination data
 * @param dstlen           [in]  length of destination data in bytes 
 * @param src              [in]  pointer to source data 
 * @param dp               [in]  ECC domain context 
 * @param tag              [in]  Data type tag
 *
 * @return 0 on success, -1 on error
 *
 */
int qsee_SW_GENERIC_ECC_bigval_to_binary(uint8_t *dst,
                                         size_t dstlen,
                                         const QSEE_qrlbn_ecc_bigval_t *src, 
                                         const QSEE_qrlbn_ecc_domain_t *dp, 
                                         QSEE_qrlbn_field_tag_t tag); 
 
/**
 * @brief Compare mem. It is not constant time comparison and intended for only testing purpose
 *
 * @param a              [in]  pointer to data
 * @param b              [in]  pointer to data
 * @param mdp            [in]  ECC modulus data
 *
 * @return 0 on success, -1 on error *
 */
int qsee_SW_GENERIC_ECC_compare(const uint32_t *a, 
                                const uint32_t *b, 
                                const QSEE_qrlbn_modulus_data_t *mdp);
#endif /*QSEE_ECC_H*/

