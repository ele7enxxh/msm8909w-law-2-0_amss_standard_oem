#ifndef _ECC_SHARED_H
#define _ECC_SHARED_H

/*===========================================================================

                             Unified Crypto 

GENERAL DESCRIPTION
Provide ECDH and ECDSA cryptography. Currently support only curve P256 which
is 128 bits security level, which is recommended by NIST

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/unifiedcrypto/shared/ecc/inc/ecc_shared.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/14   qxu    add ECDH_mult_basepoint API for ECC.
10/16/12   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ecc_core.h"
#include "uc_env.h"
#include "ecc_core.h"
#include "uc_comdef.h"


/**
 * @brief generate public key and private key of ECC
 *        same key is used for ECDH and ECDSA
 *
 * @param pubkey       [out] pointer to ECC public key
 * @param privkey      [out] pointer to ECC private key
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type 
SW_ECC_PubPrivate_Key_generate(affine_point_t *pubkey, bigval_t *privkey);

/**
 * @brief generate shared key out of Alice' public key and Bob's private key
 *
 * @param shared_key   [out] shared key bewteen Alice and Bob
 * @param pubkey       [in] pointer to ECC public key
 * @param privkey      [in] pointer to ECC private key
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type 
SW_ECDH_Shared_Key_Derive(affine_point_t *shared_key, bigval_t *privkey,affine_point_t *pubkey);

/**
 * @brief sign data with ECC private key
 *
 * @param msgdgst   [in] msg that needs to be signed
 * @param privkey   [in] pointer to private key to be used for signing
 * @param sig       [out] pointer to the buff that the signed message will be stored
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type
SW_ECDSA_Sign(bigval_t const *msgdgst, bigval_t const *privkey, ECDSA_sig_t *sig);


/**
 * @brief verify data with ECC public key
 *
 * @param msgdgst   [in] msg that needs to be authenticated
 * @param pubvkey   [in] pointer to public key to be used for signing
 * @param sig       [in] pointer to the buff that the signed message will be stored
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type
SW_ECDSA_Verify(bigval_t const *msgdgst, affine_point_t const *pubkey, ECDSA_sig_t const *sig);

/* 
 * k is given, not an output.
 */
sw_crypto_errno_enum_type
SW_ECDH_mult_basepoint(affine_point_t *P1, const bigval_t *k);

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
int
SW_ECDH_generate(affine_point_t *P1, bigval_t *k);

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
boolean_t
SW_ECDH_derive(bigval_t *tgt, bigval_t const *k, affine_point_t const *Q);
#endif /* _ECC_SHARED_H */
