#ifndef _QRLBN_ECC_TYPES_H
#define _QRLBN_ECC_TYPES_H

/*

  Copyright © 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

#include "qrlbn_arith_types.h"

/*
 * Define a type so that declaring bignums is simple.
 */

#define ECC_MAX_MODULUS_BITS (768 + 8) 
#define _ECC_BIGNUM_WORDS ((ECC_MAX_MODULUS_BITS + 24 + 31) / 32)

typedef struct {
   uint32_t data[_ECC_BIGNUM_WORDS];
} qrlbn_ecc_bigval_t;

/*
 * An ECC point.  For the point at infinity, the 
 * QRLBN_POINT_AT_INFINITY flag is set
 * and the x and y values are ignored. 
 */
typedef struct {
   qrlbn_ecc_bigval_t x;
   qrlbn_ecc_bigval_t y;
   int flags;
} qrlbn_ecc_affine_point_t;


/*
 * An ECC point in Jacobian coordinates with X, Y, and Z montgomery
 * encoded.  If the QRLBN_IS_POINT_AT_INFINITY flag is set, the X, Y,
 * and Z values must have values whose absolute value is less than a
 * few times the modulus, but the actual values do not otherwise
 * matter.  (More specifically, sometimes they get used in a
 * computation that gets abandoned, but a crazy value could cause an
 * overflow, which would cause an error to be indicated.)
 */
typedef struct {
   qrlbn_ecc_bigval_t X;
   qrlbn_ecc_bigval_t Y;
   qrlbn_ecc_bigval_t Z;
   int flags;
} qrlbn_ecc_point_t;
#define QRLBN_POINT_AFFINE 1
#define QRLBN_POINT_AT_INFINITY 2 
#define QRLBN_POINT_HAS_BLINDING 4  // for future use



typedef struct {
   qrlbn_modulus_data_t md;
   qrlbn_ecc_bigval_t modulus_storage;
   qrlbn_ecc_bigval_t montgomery_R_storage;
   qrlbn_ecc_bigval_t montgomery_R2_storage;
} qrlbn_ecc_modulus_data_t;


/*
 * Domain parameters type.  
 */

typedef struct {
  int version;
   qrlbn_ecc_modulus_data_t modulus;
   qrlbn_ecc_modulus_data_t base_point_order;
   qrlbn_ecc_bigval_t a;  // montgomery
   qrlbn_ecc_bigval_t b;  // montgomery
   qrlbn_ecc_affine_point_t affine_base_point; 
   qrlbn_ecc_point_t base_point; // montgomery
   uint64_t cofactor;
} qrlbn_ecc_domain_t;

typedef struct {
      qrlbn_ecc_bigval_t r;
      qrlbn_ecc_bigval_t s;
} qrlbn_ECDSA_sig_t;


// tags to use for specifying fields in certain operations
// The comments indicate the restrictions enforced by
// qrlbn_binary_to_bigval.

typedef enum {
  qrlbn_tag_m,     // limiteed to 32 * modulus_words - 24 bits
  qrlbn_tag_X,     // limited to 32 * modulus_words - 24 bits
  qrlbn_tag_Y,     // limited to 32 * modulus_words - 24 bits
  qrlbn_tag_a,     // limited to 32 * modulus_words - 24 bits
  qrlbn_tag_b,     // limited to 32 * modulus_words - 24 bits
  qrnbn_tag_n,     // limited to 32 * base_point_order_num_words - 24 bits
  qrlbn_tag_privkey, // 32 * base_point_order_num_words - 24 bits
  qrlbn_tag_hash,  // no limit
  qrlbn_tag_r,     // 32 * base_point_order_num_words - 24 bits
  qrlbn_tag_s,     // 32 * base_point_order_num_words -24 bits
} qrlbn_field_tag_t;


#endif /* _ECC_H */
