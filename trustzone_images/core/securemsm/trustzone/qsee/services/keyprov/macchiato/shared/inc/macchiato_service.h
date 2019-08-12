/**
 * Copyright 2014 QUALCOMM Technologies, Inc. 
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 * @file qsee_macchiato.h
 * @author	qxu
 * @version     1.0
 * @since       2014-05-16
 *
 * @brief header file for macchiato_key_service.c
 */

/*===========================================================================
   Copyright (c) 2014-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
   This file contains the definitions of the constants, data structures
   and interfaces that provide Macchiato RSA Key service support. It defines
   the external interfaces for Macchiato RSA key encrpytion/decryption, signature/
   verification, key generation.
  ===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

  when       who      what, where, why
  --------   ---      ------------------------------------
  02/24/15   bh       Update copyright year
  28/5/14    qxu      First version of qsee_macchiato.h 

===========================================================================*/

#ifndef MACCHIATO_SERVICE_H
#define MACCHIATO_SERVICE_H
#include <stddef.h>
//#include "macchiato_keyprov.h"
#include "ecc_shared.h"
#include "tzbsp_hash.h"
#include "tzbsp_cipher.h"
#include "qsee_macchiato.h" // error code reference

#define MACCHIATO_KEYTYPE_SIGN				1
#define MACCHIATO_KEYTYPE_ENC				2
/* 
#define MACCHIATO_MSG_MAX_NUM_SVCID        4  / *maximum 4 service ids* / 
#define MACCHIATO_MAX_MSG_HEADER_LEN       (MACCHIATO_MSG_TA_HASH_LEN \
                                                 + MACCHIATO_MSG_NUM_SVCID_LEN \
                                                 + MACCHIATO_MSG_MAX_NUM_SVCID * MACCHIATO_MSG_SVCID_LEN)
*/

typedef bigval_t MACC_KEY_TYPE;
typedef ECDSA_sig_t MACC_SIG_TYPE;
typedef affine_point_t MACC_POINT_TYPE;

/**
 * @brief  This function will pre-append the opaque data provided by TA with
 *  	   service ids list associated with the TA, hash the data blob with
 *  	   SHA-256 and sign signature with PKCS #1 padding.
 *
 * @param[in] opaque_data 		   the data buffer contained the data to be signed
 *  	 						         from TA
 * @param[in] opaque_data_len 	the length of the opaque data 
 * @param[out] signed_data 		the output data which has been signed, including
 *  							         Num SVC ID’s, SVC ID’s, Opaque Data. Data buffer
 * 								      provided by TA.
 * @param[out] signed_data_len	the size of the signed data 
 * @param[out] signature   		The signature
 * @param[in,out] siglen   		The max size and resulting size of the signature
 *
 * @return 0 on success, negative on failure
*/
int macchiato_service_ecc_sign
(
   unsigned char             *signed_data,
   unsigned int              signed_data_len,
   unsigned char             *signature,
   unsigned int              *signature_len
);

/**
 * @brief  This function will hash the signed data provided by TA with SHA-256, 
 *  	   calcuate the signature with PKCS #1 padding and verify it with
 *  	   provided signature.
 *
 * @param[in] signed_data  		The data buffer contained the signed data from 
 *  	 						         TA
 * @param[in] signed_data_len   The length of signed data 
 * @param[in] signature         The signature to be verified provided by TA
 * @param[in] signature_len     The size the signature 
 *
 * @return 0 on success, negative on failure
*/
int macchiato_service_ecc_verify
(
   unsigned char             *signed_data, 
   unsigned int               signed_data_len,
   unsigned char             *signature, 
   unsigned int               signature_len
);

/**
 * @brief This function will encrypt the plain message provided by TA with PKCS 
 *  	  #1 padding.
 *
 * @param[in] msg           The plaintext
 * @param[in] msglen        The length of the plaintext
 * @param[out] cipher       The ciphertext 
 * @param[in,out] cipherlen The max size and resulting size of the ciphertext 
 *
 * @return 0 on success, negative on failure
*/
int macchiato_service_ecc_encrypt
(
   const unsigned char     *msg, 
   unsigned int             msglen,
   unsigned char           *cipher,
   unsigned int            *cipherlen
);

/**
 * @brief  This function will decrpyed the cipher message provided by TA and 
 *  	   then v1.5 depad.
 *
 * @param[in] cipher        The ciphertext
 * @param[in] cipherlen     The length of the ciphertext(octets)
 * @param[out] msg          The plaintext 
 * @param[in,out] msglen    The max size and resulting size of the plaintext 
 *
 * @return 0 on success, negative on failure
*/
int macchiato_service_ecc_decrypt
(
   unsigned char           *cipher,
   unsigned int             cipherlen,
   unsigned char           *msg,
   unsigned int            *msglen
);

int macchiato_ecc_get_pubkey
(
   MACC_KEY_TYPE *privkey, 
   unsigned char *p_marshalled_key
);

int macchiato_ecc_sign
(
   MACC_KEY_TYPE *privkey,
   unsigned char *signed_data,
   unsigned int signed_data_len,
   unsigned char *signature,
   unsigned int  *signature_len
);

int macchiato_ecc_verify
(
   MACC_KEY_TYPE *privkey,
   unsigned char *signed_data,
   unsigned int signed_data_len,
   unsigned char *signature
);

unsigned int macchiato_get_oem_id
(
   void
);

macchiato_err_t macchiato_get_oem_pk_hash
(
   uint8  *oem_pk_hash_buf,
   uint32  buf_len
);

#endif //MACCHIATO_SERVICE_H

