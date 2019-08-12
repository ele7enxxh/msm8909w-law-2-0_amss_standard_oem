#ifndef SECAPIDRM_H
#define SECAPIDRM_H

/*===========================================================================

                          KEY MANAGEMENT ALGORITHM 

                            USED IN OMA DRM V2.0
							 
FILE:  secapidrm.h

DESCRIPTION:
   Realize the algorithms for key management and distribution using 
   RSA, AES, and SHA-1 as crypto primitives

   References:

   Section 7, OMA DRM Specificiatoin V2.0, Dec. 10 2004

   Design Notes:
   
   To simplify coding, the input Z is a random number with the same size as
   RSA modulus. So I2OSP() is implicitly satisfied since Z is defined as a 
   byte string in the same way as modulus.  
   
EXTERNALIZED FUNCTIONS

   aes_wrap(), aes_unwrap: 
      AES Wrapping alogrithm, refer to RFC 3394
   kdf(): 
      key derivation function defined in Section 7, OMA DRM V2.0
   rsaes_kem_kws(), rsaes_kem_kws_inv(): 
      an asymmetric encrytion scheme defined in ANSI X9.44 Draft 6
	  and used in OMA DRM V2.0

 Copyright (c) 2005-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*=========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secapidrm.h#1 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC
06/10/09   ejt     Removed FEATURE_SEC_IPC
02/25/05   sb      Created file.

============================================================================*/

#include "core_variation.h"
#include "comdef.h"
#include "rex.h"
#include "task.h"
#include "assert.h"
#include "msg.h"
#include "err.h"

#include "secerrno.h"
#include "secapi.h"
#include "seccrypt.h"

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
  General CAPI types and definitions
===========================================================================*/


// Storage required when supporting asynchronus callbacks
typedef union secapidrm_key_storage_s
{
	struct rsa_data_s
	{
		secapi_async_exec_cb_param_data_type async_cb;
		secapi_handle_type                   aes_handle;
		uint8*                               aes_cipher_text;
		uint32                               mod_len;
		uint32                               result[SECAPI_PKX_MAX_KEY_SIZE >> 2];
	} rsa;
	
	uint32  aes_unwrapped_key[SECAPI_ENC_AES256_KEY_BYTE_LEN >> 2];
} secapidrm_key_storage_type;



typedef enum secapidrm_rsa_key_enum
{
	SECAPIDRM_RSA_KEY_TEST,
	SECAPIDRM_RSA_KEY_DRM_DEVKEY
} secapidrm_rsa_key_type;


// Storage for Key parametes. Only used for setting test keys
typedef struct secapidrm_rsa_key_params_struct
{
	uint8*                 private_key;
	uint8*                 public_key;
	uint8*                 modulus;
	uint32                 modulus_len;
	uint8*                 private_key_iv;
	uint8*                 msm_dev_key;
	secapidrm_rsa_key_type key_type;
} secapidrm_rsa_key_params_type;




/*===========================================================================

FUNCTION AES_WRAP()

DESCRIPTION
	Wrap the input data using AES encryption algorithm, 64 bits more in output

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_ABORTED

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type
secapidrm_aes_wrap(
	secapi_handle_type           handle,
	uint8                        *plaintext,
	uint32                       plaintext_len,
	uint8                        *kek,
	uint8                        *ciphertext,
	uint32                       ciphertext_len
	);

/*===========================================================================

FUNCTION AES_UNWRAP()

DESCRIPTION
	Unwrap and restore key data using AES decryption algorithm

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_ABORTED/E_FAILURE
	E_FAILURE: integrity test fails. A possible reason is
	           that aes_wrap() does not use the default IV to 
			   initialize register A.
	           
SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type
secapidrm_aes_unwrap(
	secapi_handle_type  handle,
	uint8*              ciphertext,
	uint32              ciphertext_len,
	uint8*              kek,
	uint8*              plaintext,
	uint32              plaintext_len
	);


/*===========================================================================

FUNCTION SECAPIDRM_KDF()

DESCRIPTION
	Key derivation function using SHA-1, refer to ANSI X9.44

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type
secapidrm_kdf(
	uint8*  secrettext,
	uint32  secrettext_len, 
	uint8*  other_info,
	uint32  other_info_len,
	uint8*  key_data,
	uint32  key_len
	);

/*===========================================================================

FUNCTION RSAES_KEM_KWS()

DESCRIPTION
	Distribute wrapped key data using receiver's RSA public key.
	A random number Z is used to improve secrecy.

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_FAILURE

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type
secapidrm_rsaes_kem_kws(
	secapi_handle_type                     cipher_handle,
	secapi_handle_type                     modexp_handle,
	secapidrm_rsa_key_type                 rsa_key,
	uint8*                                 secrettext,
	uint8*                                 key,
	uint8*                                 outputtext,
	uint32                                 outputtext_len,
	secapi_async_exec_cb_param_data_type*  async_cb
	);

/*===========================================================================

FUNCTION RSAES_KEM_KWS_INV()

DESCRIPTION
	Restore wrapped key data using receiver's RSA private key.
	A random number Z is reconstructed from wrapped key data.

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_FAILURE

SIDE EFFECTS
	None.
===========================================================================*/
secerrno_enum_type
secapidrm_rsaes_kem_kws_inv(
	secapi_handle_type                     cipher_handle,
	secapi_handle_type                     modexp_handle,
	secapidrm_rsa_key_type                 rsa_key,
	uint8*                                 inputtext,
	uint32                                 inputtext_len,
	secapidrm_key_storage_type*            key_data,
	secapi_async_exec_cb_param_data_type*  async_cb
	);

#define SECAPIDRM_TEST
#ifdef	SECAPIDRM_TEST
/*===========================================================================

FUNCTION SECAPIDRM_RSA_PARAMS_INITIALIZE()

DESCRIPTION
	Initialize RSA Keys

DEPENDENCIES
	None

RETURN VALUE
	E_SUCCESS/E_NOT_ALLOWED/E_NO_MEMORY/E_ABORTED

SIDE EFFECTS
	None.
===========================================================================*/   
secerrno_enum_type
secapidrm_rsa_params_initialize
(
	secapidrm_rsa_key_params_type*   params
);
#endif

#endif
