#ifndef SECAPI_UTIL_H
#define SECAPI_UTIL_H

/** 
   @file secapi_util.h
   @brief Cipher and integrity protection of the data buffer.
   This file contains the definitions of the constants, data structures and 
   interfaces contained in the Cryptographic Utility Library. 
 */
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The crypto_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      crypto_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the crypto_secutilapi group 
      description in the crypto_mainpage.dox file. 
===========================================================================*/

/*===========================================================================
  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/crypto/secapi_util.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/14   nk      Kasumi ISR API support
10/03/13   jk      added A new wrapper over the existing CE API for key derivation.
01/24/12   nk      Added NPA Clock voting APIs
09/01/11   sp      Added Support for SecureTar/NV Preload Feature.
01/26/11   sd      (Tech Pubs) Edited/added Doxygen comments and markup.
10/07/10   sd      (Tech Pubs) Edited/added Doxygen comments and markup.
06/24/10   af      (Tech Pubs) Edited/added Doxygen comments and markup.
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
04/01/10   bm      added support for encrypting and authenticating data
01/08/10   bm      added support to encrypt/decrypt data of lengths more than 64k
12/18/09   ejt     added secapi_util_create_hmac_with_hw_key
08/28/09   ejt     removed #include "customer.h" and FEATURE flags for CMI compliance
02/23/09   ejt     added routine secapi_util_read_hw_key_api_type 
05/21/07   avm     corrected RPC meta comments

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "IxErrno.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "secapi.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
/** @addtogroup crypto_secutilapi 
  @{ */  

/**
  Default value of the initial vector for an AES encryption.
*/
extern uint8 secapi_util_AESIV[SECAPI_ENC_AES128_BLOCK_BYTE_LEN];

/* Feature flag to enable clients to check if this feature is supported */
#define SEC_AUTH_IMG

/* Version number to keep track of Architectural changes */
#define SEC_AUTH_IMG_MAJOR_VER 1
#define SEC_AUTH_IMG_MINOR_VER 1

/** Structure to store Efuse specific params
*/
typedef struct
{
  uint8*      root_hash_ptr; /* Ptr to hash of pub key of trusted root */
  uint32      root_hash_len; /* Len of public key hash of trusted root */
  uint32      oem_id;
  uint64      msm_hw_id;     /* MSM Id to be used for signing */
  uint64      sw_vsn_id;     /* Software version id */
} secapi_util_auth_img_param_t;

typedef enum
{
  SECAPI_UTIL_ENC,
  SECAPI_UTIL_DEC,
  SECAPI_UTIL_MAX_OP = 0x7fffffff
} secapi_util_op_type;

#define SECAPI_UTIL_MUTEX_TYPE rex_crit_sect_type

#define SECAPI_UTIL_MUTEX_ENTER()\
        { if(secapi_util_mutex_init == 0) \
          { \
            rex_init_crit_sect(&secapi_util_mutex); \
            secapi_util_mutex_init = 1; \
           } \
          rex_enter_crit_sect(&secapi_util_mutex);\
        }

#define SECAPI_UTIL_MUTEX_EXIT() rex_leave_crit_sect(&secapi_util_mutex);
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/**
  Encrypts the input data using the hardware primary key. On platforms where 
  there is no hardware key support, a dummy key is used.

  @param[in] data                    Pointer to the plaintext data.
  @param[in] data_len                Length of the plaintext data.
  @param[out] encrypted_data         Pointer to the ciphertext data.
  @param[out] encrypted_data_len_ptr Pointer to hold the length of the 
                                     ciphertext data.

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_util_encrypt_with_hw_key (char *data,
  unsigned int data_len, char *encrypted_data,
  unsigned int *encrypted_data_len_ptr);

/**
  Decrypts the input data using the hardware primary key. On platforms where 
  there is no hardware key support, a dummy key is used. 

  @param[in] data                    Pointer to the ciphertext data.
  @param[in] data_len                Length of the ciphertext data.
  @param[out] decrypted_data         Pointer to the decrypted data.
  @param[out] decrypted_data_len_ptr Pointer to hold the length of the 
                                     decrypted data.

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_util_decrypt_with_hw_key (char *data,
  unsigned int data_len, char *decrypted_data,
  unsigned int *decrypted_data_len_ptr);

/**
  Encrypts the input data using the hardware primary key with the option to 
  select a task stack for cryptography. On platforms where there is no hardware 
  key support, a dummy key is used. 
  
  If is_light_weight is TRUE, the library uses the call's task stack for 
  cryptography. Otherwise, the library uses the SEC task stack for cryptography. 

  @param[in] data                    Pointer to the plaintext data.
  @param[in] data_len                Length of the plaintext data.
  @param[out] encrypted_data         Pointer to the ciphertext data.
  @param[out] encrypted_data_len_ptr Pointer to hold the length of the ciphertext data.
  @param[in] is_light_weight         TRUE or FALSE. 

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_util_encrypt32_with_hw_key (void *data,
  uint32 data_len, void *encrypted_data,
  uint32 *encrypted_data_len_ptr,
  boolean is_light_weight);

/**
  Decrypts the input data using the hardware primary key with the option to 
  select a task stack for cryptography. On platforms where there is no hardware 
  key support, a dummy key is used. 

  If is_light_weight is TRUE, the library uses the call's task stack for 
  cryptography. Otherwise, the library uses the SEC task stack for 
  cryptography. 

  @param[in] data                    Pointer to the plaintext data.
  @param[in] data_len                Length of the plaintext data.
  @param[out] decrypted_data         Pointer to the decrypted data.
  @param[out] decrypted_data_len_ptr Pointer to hold the length of the 
                                     decrypted data.
  @param[in] is_light_weight         TRUE or FALSE. 

  @return 
  IxErrNoType.

  @dependencies
  None. 
*/
IxErrnoType secapi_util_decrypt32_with_hw_key (void *data,
  uint32 data_len, void *decrypted_data,
  uint32 *decrypted_data_len_ptr,
  boolean is_light_weight);


/**
  Encrypts the user data and generates the authentication/integrity protection 
  data (referred to as meta data in the API). Encryption is done with an 
  AES-128 CBC-bit key with meta data that includes HMAC SHA-256. The user 
  stores the meta data along with the encrypted data.  

  @param[in] input_data           Pointer to the plaintext data.
  @param[in] input_data_len       Byte length of the plaintext data, in 
                                  multiples of 16 bytes.
  @param[out] output_data         Pointer to the ciphertext data. The output 
                                  data length is equal to the input data length
                                  because the function expects the input data 
                                  length to be an AES block multiple of 16 bytes 
                                  with no padding.
  @param[out] output_data_len_ptr Pointer to hold the byte length of the 
                                  ciphertext data. output_data_len_ptr is the 
                                  same value as input_data_len.
  @param[out] meta_data_out       Pointer to the meta data, which includes HMAC 
                                  SHA-256.
  @param[out] meta_data_out_len   Byte length of meta_data_out; 64 bytes.

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_util_encrypt_authenticate (void    *input_data, 
                                              uint32  input_data_len, 
                                              void    *output_data, 
                                              uint32  *output_data_len_ptr, 
                                              void    *meta_data_out, 
                                              uint32  meta_data_out_len);
 
/**
  Decrypts the user data and checks the authenticity/integrity of the data. 
  The metadata contains the authenticity/integrity protection data. Encrypted 
  user data is decrypted by this function after passing the 
  authenticity/integrity check. 

  @param[in] input_data           Pointer to the cipherext data.
  @param[in] input_data_len       Byte length of the ciphertext data, in 
                                  multiples of 16 bytes.
  @param[out] output_data         Pointer to the plaintext data.
  @param[out] output_data_len_ptr Pointer to hold the byte length of the 
                                  plaintext data. output_data_len_ptr is the same 
                                  value as input_data_len.
  @param[out] meta_data_in        Pointer to the meta data, which includes HMAC SHA-256.
  @param[out] meta_data_in_len    Byte length of the meta_data_out; 64 bytes.

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_util_decrypt_authenticate (void    *input_data, 
                                              uint32  input_data_len, 
                                              void    *output_data, 
                                              uint32  *output_data_len_ptr, 
                                              void    *meta_data_in, 
                                              uint32  meta_data_in_len);
 
/**
  Determines the function to use to read the hardware device key on the modem 
  processor.  

  @return 
  A Boolean value that decides the type of kernel interface to be used 
  in order to read the hardware device key based on different targets:
  - TRUE -- API2.
  - FALSE -- API1.

  @dependencies 
  None.
*/
boolean secapi_util_read_hw_key_api_type(void);

/**
  Creates a SHA-256 HMAC hash using the primary hardware key. 

  @param[in] msg_ptr          Pointer to the input message to be hashed.
  @param[in] msg_len          Length of the input message in bytes.
  @param[out] msg_digest_ptr  Pointer to the output HMAC buffer (memory 
                              provided by the caller). This buffer must be
                              32bytes long
                              (SECAPI_HSH_SHA_DIGEST_SIZE).

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_util_create_hmac_with_hw_key 
(
  uint8*   msg_ptr,
  uint16   msg_len,
  uint8*   msg_digest_ptr
);
/** @} */  /* end_addtogroup_crypto_secutilapi */



/**
  Creates a SHA-256 HMAC hash using the primary hardware key. 

  @param[in] msg_ptr          Pointer to the input message to be hashed.
  @param[in] msg_len          Length of the input message in bytes.
  @param[out] msg_digest_ptr  Pointer to the output HMAC buffer (memory 
                              provided by the caller). This buffer must be
                              32bytes long
                              (SECAPI_HSH_SHA_DIGEST_SIZE).
 @param[out] msg_digest_len   20 (SHA-1) 0r 32 bytes (SHA-256)

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_util_create_hmac2_with_hw_key 
(
  uint8*   msg_ptr,
  uint32   msg_len,
  uint8*   msg_digest_ptr,
  uint32   msg_digest_len
);

/**
  Parses the signed image for secure header, image, signature and certificates.
  Verfies the signature of passed in image against passed in attestation
  certificate.
 
  @param[in]  signed_img_ptr  Pointer to signed image
  @param[in]  signed_img_len  Length of the signed image
  @param[out] img_ptr         Pointer to start of the image
  @param[out] img_len         Length of the image. Memory allocated by caller.
  @param[in]  efuse_param_ptr Pointer to struct containing efuse params.
                              If this pointer is NULL then we try to read the
                              values directly from corresponding efuses.

  @return
  TRUE - If Image signature and certificate chain are valid. \n
  FALSE - Signature validation failed or cert chain is not valid

  @dependencies
  None.
*/
boolean secapi_util_auth_img_verify
(
  uint8*                       signed_img_ptr,
  uint32                       signed_img_len,
  uint32*                      img_offset,
  uint32*                      img_len,
  secapi_util_auth_img_param_t *efuse_param_ptr
);

/**
  Derives a Key using known label, context strings and using Primary HW key or a user provided Key.

  This is an implementation of the key derivation algorithm based on
  AES_CMAC from FIPS Special Publication 800-108.

  @param[in]  input_key        Pointer to the input Key.
                               This should be NULL if Primary HW Key are to be used for Ciphering.
			       
  @param[in]  input_key_len    Length of the 'input_key' in bytes. It should be either 16 or 32. Otherwise, we use 32 as default.

  @param[in]  label            Pointer to a string constant.
                               Contention for label=> Product:usage (e.g., ¡°remoteFS:encryption¡±) *No NULLs

  @param[in]  label_len        Length of the string constant 'label'.

  @param[in]  context          Pointer to a string constant.
                               Convention for context => 16 bytes Nonce (e.g., "4902384903414267")
   
  @param[in]  context_len      Length of the string constant 'context'.

  @param[out]  output_key      Pointer to the generated Key.
                               The memory for this should be provided by the caller.

  @param[in]   output_key_len  Length of the Key to be generated in bytes.
                               This is size of the buffer that the 'output_key'
                               points to. output_key_len is supported in 0< output_key_len <= (2**32-1)/8


  @return
  E_SUCCESS If a valid derived key is obtained 
  E_FAILURE If key derivation failed 
  E_NOT_AVAILABLE If crypto HW feature unavailable.

  @dependencies
  None.
*/
IxErrnoType secapi_util_kdf
(
  const void                  *input_key,
  size_t                      input_key_len,
  const void                  *label,
  size_t                      label_len,
  const void                  *context,
  size_t                      context_len,
  void                        *output_key,
  size_t                      output_key_len
);

/**
   Provides an Node Power Architecture voting mechanism for power optimisation 
   The call to this function dynamically votes for maximum the bus and hardware block speeds.
   This would increase the Crypto Performance.

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_clk_enable
(
  void
);


/**
   Provides an Node Power Architecture un-voting mechanism for power optimisation 
   The call to this function dynamically un votes all the frequency of relevant bus 
   and hardware block. This function indicates the NPA that the bus or the hardware block 
   is no longer used. 

  @return 
  IxErrNoType.

  @dependencies
  None.
*/
IxErrnoType secapi_clk_disable
(
  void
);

/**
  Kasumi decrypt API for ISR context. This is an API provided to mutils 
  so that packets can be decrypted in ISR environment, with no sec related
  thread overheads. This is a direct call to core API for Kasumi.
 
  @param[in]  in_msg_ptr  Pointer to input message
  @param[in]  in_bit_len  Length of the input message in bits
  @param[out] iv_ptr      Pointer to input vector
  @param[out] key_ptr     Pointer to input key.
  

  @return
  E_SUCCESS if success, another error code on failure

  @dependencies
  None.
*/
secerrno_enum_type secapi_util_kasumi_eea1_isr(
                     uint8* in_msg_ptr,
                     uint32 in_bit_len,
                     uint8* iv_ptr,
                     uint8* key_ptr);


#ifdef __cplusplus
}
#endif

#endif /* SECAPI_UTIL_H */
