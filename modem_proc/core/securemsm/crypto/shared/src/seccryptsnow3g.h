#ifndef SECENCARMSNOW3G_H
#define SECENCARMSNOW3G_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                  D E S   C I P H E R I N G   R O U T I N E S


DESCRIPTION
  This is the header file for the Security Services SNOW3G ciphering module.

EXTERNALIZED FUNCTIONS
  secenc_snow3g_encrypt()
  secenc_snow3g_decrypt()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Generate the subkeys before the first encrypt or decrypt.

 Copyright (c) 2000-2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/seccryptsnow3g.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Remove FEATURE_SEC wrap
09/04/03   qxu     create file.    	
===========================================================================*/


/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "seccrypt.h"
#include "seccrypti.h"
#include "secsnow3g.h"
/*===========================================================================

                         DEFINITIONS AND CONSTANTS

===========================================================================*/
#define F8_KEY_NATIVE SNOW3G_KEY_NATIVE 

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/



/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECENC_SNOW3G_ENCRYPT()

DESCRIPTION
  Using the SNOW3G cipher algorithm, encrypt the plaintext at in_msg_ptr
  and write the ciphertext output to out_msg_ptr.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS        on sucessful completion

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenc_snow3g_encrypt
(
  seccrypt_instance_type*  instance_ptr,
  IOVEC_T*                 in_msg,
  IOVEC_T*                 out_msg
);

/*===========================================================================

FUNCTION SECENC_SNOW3G_DECRYPT()

DESCRIPTION
  Using the SNOW3G cipher algorithm, decrypt the ciphertext at in_msg_ptr
  and write the plaintext output to out_msg_ptr.

DEPENDENCIES
  None

RETURN VALUE
    E_SUCCESS        on sucessful completion

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenc_snow3g_decrypt
(
  seccrypt_instance_type*  instance_ptr,
  IOVEC_T*                 in_msg,
  IOVEC_T*                 out_msg
);

secerrno_enum_type sechsh_snow3g_create_mac
(
  seccrypt_instance_type*  instance_ptr
);


#endif /* SECENCARMSNOW3G_H */
