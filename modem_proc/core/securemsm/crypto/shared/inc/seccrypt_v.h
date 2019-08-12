#ifndef SECCRYPT_V_H
#define SECCRYPT_V_H

/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
                
                 C R Y P T O G R A P H I C    I N T E R F A C E

                        H E A D E R    F I L E  
 

 Copyright (c) 2008 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/inc/seccrypt_v.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework 
06/10/09   ejt     removed CS_ISIGNAL
12/11/08   sm      original

===========================================================================*/

/*---------------------------------------------------------------------------
  CS PKX library functions
---------------------------------------------------------------------------*/
secerrno_enum_type seccryptcs_pkx_new
(
  secapi_handle_type     *handle_ptr,
  secapi_algo_enum_type  algo
);
secerrno_enum_type seccryptcs_pkx_delete
(
  secapi_handle_type     *handle_ptr
);

secerrno_enum_type seccryptcs_pkx_set_param
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secpkx_param_enum_type        param,
  secpkx_param_data_type*       param_data_ptr
);

secerrno_enum_type seccryptcs_pkx_modexp
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  uint8*                        result_ptr
);

secerrno_enum_type seccryptcs_pkx_encrypt
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        plaintext_ptr,
  uint16                        plaintext_len,
  uint8*                        ciphertext_ptr,
  uint16*                       ciphertext_len_ptr
);

secerrno_enum_type seccryptcs_pkx_decrypt
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        ciphertext_ptr,
  uint16                        ciphertext_len,
  uint8*                        plaintext_ptr,
  uint16*                       plaintext_len_ptr
);

secerrno_enum_type seccryptcs_pkx_sign
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        msg_hash_ptr,
  uint16                        msg_hash_len,
  uint8*                        signature_ptr,
  uint16*                       signature_len_ptr
);

secerrno_enum_type seccryptcs_pkx_authenticate
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        signature_ptr,
  uint16                        signature_len,
  uint8*                        msg_hash_ptr,
  uint16*                       msg_hash_len_ptr
);

/*---------------------------------------------------------------------------
  CS HASH/HMAC library functions
---------------------------------------------------------------------------*/
secerrno_enum_type seccryptcs_hsh_create_digest
(
    secapi_handle_type      handle,
    secapi_algo_enum_type   algo,
    uint8*                  msg_ptr,
    uint16                  msg_len,
    uint8*                  msg_digest
);

secerrno_enum_type seccryptcs_hsh_create_hmac
(
  secapi_handle_type      handle,
  secapi_algo_enum_type   algo,
  uint8*                  msg_ptr,
  uint16                  msg_len,
  uint8*                  key_ptr,
  uint16                  key_len,
  uint8*                  hmac
);

#endif /* SECCRYPT_V_H */

