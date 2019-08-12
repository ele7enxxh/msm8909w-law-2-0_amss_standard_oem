#ifndef _QRLBN_ECC_TYPES_H
#define _QRLBN_ECC_TYPES_H

/*

  Copyright © 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

#include "qrlbn_arith_types.h"
#include "qrlbn_arith_tags.h"

/*
 * Define a type so that declaring bignums is simple.
 */

#define ECC_MAX_MODULUS_BITS (521 + 8) 
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

// The flags fields are given below.  These values are also used in the 
// process_as  arg of qrlbn_import_point and qrlbn_X_coordinate_to_point 
// where they indicate the processing to be used.  (This
// override scheme is provided in response to legal advice.  
// Do not change it without consulting with Corporte Legal.)

#define QRLBN_POINT_UNCOMPRESSED        0x4   // A full Y value supplied
#define QRLBN_POINT_COMPRESSED_Y_LSB    0x2   // The LSB of Y in 
                                              // non-montgomery form 
                                              // is given using 
                                              // QRLBN_POINT_AFFINE_Y_ODD
                                              // Y in the point is ignored.
#define QRLBN_POINT_AFFINE_Y_ODD        0x1   // Ignored unless 
                                              // QRLBN_POINT_COMPRESSED_Y_LSB 
                                              // is set.  The "AFFINE" means
                                              // "if the point were in affine
                                              // form", not that it actaully is.
                                              // is actually in affine form.
#define QRLBN_POINT_COMPRESSED_Y_NONE   0x8   // Y in the point is ignored
#define QRLBN_POINT_IS_TRUSTED      0x10      // ok to skip checks


#define QRLBN_POINT_AFFINE          0x20
#define QRLBN_POINT_AT_INFINITY     0x40

#define QRLBN_POINT_IS_BLINDED      0x80  // for future use



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
   qrlbn_ecc_bigval_t a;
   qrlbn_ecc_bigval_t b;
   qrlbn_ecc_bigval_t a_mont;  // montgomery
   qrlbn_ecc_bigval_t b_mont;  // montgomery
   qrlbn_ecc_affine_point_t affine_base_point; 
   qrlbn_ecc_point_t base_point; // montgomery
   qrlbn_ecc_bigval_t ts_z;  // montgomery, for Tonelli Shanks sqrt
   uint64_t cofactor;
} qrlbn_ecc_domain_t;

typedef struct {
      qrlbn_ecc_bigval_t r;
      qrlbn_ecc_bigval_t s;
} qrlbn_ECDSA_sig_t;


#endif /* _ECC_H */
