#ifndef _QRLBN_ARITH_TAGS_H
#define _QRLBN_ARITH_TAGS_H

/*

  Copyright © 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

// tags to use for specifying fields in certain operations
// The comments indicate the restrictions enforced by
// qrlbn_binary_to_bigval.

typedef enum {
  qrlbn_tag_m,     // limited to 32 * modulus_words - 24 bits
  qrlbn_tag_X,     // limited to 32 * modulus_words - 24 bits
  qrlbn_tag_Y,     // limited to 32 * modulus_words - 24 bits
  qrlbn_tag_a,     // limited to 32 * modulus_words - 24 bits
  qrlbn_tag_b,     // limited to 32 * modulus_words - 24 bits
  qrlbn_tag_n,     // ecc base point order, limited to 32 * base_point_order_num_words - 24 bits
  qrlbn_tag_privkey, // 32 * base_point_order_num_words - 24 bits
  qrlbn_tag_hash,  // no limit
  qrlbn_tag_r,     // 32 * base_point_order_num_words - 24 bits
  qrlbn_tag_s,     // 32 * base_point_order_num_words -24 bits
  qrlbn_tag_p,
  qrlbn_tag_q,
#if 0
  qrlbn_tag_RSA_n,
  qrlbn_tag_RSA_p,
  qrlbn_tag_RSA_q,
  qrlbn_tag_RSA_dP,
  qrlbn_tag_RSA_dQ,
  qrlbn_tag_RSA_qinv,
  qrlbn_tag_RSA_exponent,
  qrlbn_tag_RSA_data
#endif
} qrlbn_field_tag_t;



#endif
