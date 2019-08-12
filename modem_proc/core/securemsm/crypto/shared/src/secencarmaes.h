#ifndef SECENCARMAES_H
#define SECENCARMAES_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                  D E S   C I P H E R I N G   R O U T I N E S


DESCRIPTION
  This is the header file for the Security Services AES ciphering module.

EXTERNALIZED FUNCTIONS
  secencarm_arc4_encrypt()
  secencarm_arc4_decrypt()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Generate the subkeys before the first encrypt or decrypt.

 Copyright (c) 2000-2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secencarmaes.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC wrap.
08/31/05   df      Changes to support IOVEC
02/25/05   sb      Added support for AES128 decryption of private key
02/02/04   rwh     Split the ARM ciphers into separate files.

===========================================================================*/


/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "comdef.h"


/*===========================================================================

                         DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/



/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECENCARM_AES_ENCRYPT()

DESCRIPTION
  Using the AES cipher algorithm, encrypt the plaintext at in_msg_ptr
  and write the ciphertext output to out_msg_ptr.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS        on sucessful completion

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secencarm_aes_encrypt
(
  seccrypt_instance_type*  instance_ptr,
  IOVEC_T*                 in_msg,
  IOVEC_T*                 out_msg
);

/*===========================================================================

FUNCTION SECENCARM_AES_DECRYPT()

DESCRIPTION
  Using the AES cipher algorithm, decrypt the ciphertext at in_msg_ptr
  and write the plaintext output to out_msg_ptr.

DEPENDENCIES
  None

RETURN VALUE
    E_SUCCESS        on sucessful completion

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secencarm_aes_decrypt
(
  seccrypt_instance_type*  instance_ptr,
  IOVEC_T*                 in_msg,
  IOVEC_T*                 out_msg
);

/*===========================================================================

FUNCTION SECENC_AES_DECRYPT_PRIV_KEY()

DESCRIPTION
  Using the AES cipher algorithm, decrypt input pirivate 'key' using the
  input 'iv' value.

DEPENDENCIES
  None

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_BAD_DATA       Data verification failed
    E_NOT_SUPPORTED  Option hasn't been coded yet
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secencarm_aes_decrypt_priv_key
(
  uint8*                       in_data_ptr,
  uint8*                       iv, 
  uint8*                       key,
  uint16*                      msg_byte_len,
  secapi_enc_op_mode_enum_type op_mode
);

#endif /* SECENCARMAES_H */
