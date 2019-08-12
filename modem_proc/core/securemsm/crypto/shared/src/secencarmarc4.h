#ifndef SECENCARMARC4_H
#define SECENCARMARC4_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                  A R C 4   C I P H E R I N G   R O U T I N E S


DESCRIPTION
  This is the header file for the Security Services ARC4 ciphering module.

EXTERNALIZED FUNCTIONS
  secencarm_arc4_encrypt()
  secencarm_arc4_decrypt()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Generate the subkeys before the first encrypt or decrypt.

 Copyright (c) 2000-2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secencarmarc4.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC
08/31/05   df      Changes to support IOVEC
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

FUNCTION SECENCARM_ARC4_ENCRYPT()

DESCRIPTION
  Using the ARC4 cipher algorithm, encrypt the plaintext at in_msg_ptr
  and write the ciphertext output to out_msg_ptr.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS        on sucessful completion

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secencarm_arc4_encrypt
(
  seccrypt_instance_type*  instance_ptr,
  IOVEC_T*                 in_msg,
  IOVEC_T*                 out_msg
);

/*===========================================================================

FUNCTION SECENCARM_ARC4_DECRYPT()

DESCRIPTION
  Using the ARC4 cipher algorithm, decrypt the ciphertext at in_msg_ptr
  and write the plaintext output to out_msg_ptr.

DEPENDENCIES
  None

RETURN VALUE
    E_SUCCESS        on sucessful completion

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secencarm_arc4_decrypt
(
  seccrypt_instance_type*  instance_ptr,
  IOVEC_T*                 in_msg,
  IOVEC_T*                 out_msg
);

#endif /* SECENCARMARC4_H */
