#ifndef SECCRYPTKASUMI_H
#define SECCRYPTKASUMI_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                       C I P H E R I N G   R O U T I N E S


DESCRIPTION
  This is the header file for the Security Services SNOW3G ciphering module.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Generate the subkeys before the first encrypt or decrypt.

 Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/seccryptkasumi.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/13   nk      initial version
===========================================================================*/
#include "comdef.h"

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECENC_KASUMI_ENCRYPT()

DESCRIPTION
  Using the KASUMI cipher algorithm, encrypt the plaintext at in_msg
  and write the ciphertext output to out_msg.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS        on sucessful completion

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenc_kasumi_encrypt
(
  seccrypt_instance_type*  instance_ptr,
  IOVEC_T*                 in_msg,
  IOVEC_T*                 out_msg
);

/*===========================================================================

FUNCTION SECENC_KASUMI_DECRYPT()

DESCRIPTION
  Using the KASUMI cipher algorithm, decrypt the ciphertext at in_msg
  and write the plaintext output to out_msg.

DEPENDENCIES
  None

RETURN VALUE
    E_SUCCESS        on sucessful completion

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenc_kasumi_decrypt
(
  seccrypt_instance_type*  instance_ptr,
  IOVEC_T*                 in_msg,
  IOVEC_T*                 out_msg
);


/*===========================================================================

FUNCTION SECENC_KASUMI_CREATE_MAC()

DESCRIPTION
  Using the KASUMI intergity algorithm, create a MAC 
  
DEPENDENCIES
  None

RETURN VALUE
    E_SUCCESS        on sucessful completion

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sechsh_kasumi_create_mac
(
  seccrypt_instance_type*  instance_ptr
);

#endif /* SECCRYPTKASUMI_H */
