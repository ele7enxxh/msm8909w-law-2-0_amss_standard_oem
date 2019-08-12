#ifndef DES_CORE_H
#define DES_CORE_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/unifiedcrypto/core/des/inc/des_core.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/16/13   nk      initial version
===========================================================================*/
#include "des_tab.h"


/*===========================================================================
FUNCTION check_parity()

DESCRIPTION
  Check parity of DES key.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static int check_parity
(
  des_cblock   *key
);

/*===========================================================================

FUNCTION des_set_key

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
 return 0  if key parity is odd (correct),
 return -1 if key parity error,
 return -2 if illegal weak key.

SIDE EFFECTS
  None.
===========================================================================*/
int des_set_key
(
  des_cblock*              key,
  des_key_schedule         schedule
);

/*===========================================================================

FUNCTION des_encrypt

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
int des_encrypt
(
  uint32*                  input,
  uint32*                  output,
  des_key_schedule         ks,
  boolean_t                encrypt
);

/*===========================================================================

FUNCTION DES_CBC_ENCRYPT

DESCRIPTION
  DES encrypt a sequence of blocks of plaintext.

DEPENDENCIES
  The CBC mode IV value from the caller is not updated upon return,
  so the caller needs to update the IV.

RETURN VALUE
  Zero

SIDE EFFECTS
  None.
===========================================================================*/
int des_cbc_encrypt
(
  des_cblock*                   input,
  des_cblock*                   output,
  uint16                        length,
  des_key_schedule              schedule,
  des_cblock*                   ivec,
  des_op_mode                   op_mode
);

/*===========================================================================

FUNCTION DES_CBC_DECRYPT

DESCRIPTION
  DES decrypt a sequence of blocks of ciphertext.

DEPENDENCIES
  The CBC mode IV value from the caller is not updated upon return,
  so the caller needs to update the IV.

RETURN VALUE
  Zero

SIDE EFFECTS
  None.
===========================================================================*/
int des_cbc_decrypt
(
  des_cblock*                   input,
  des_cblock*                   output,
  uint16                        length,
  des_key_schedule              schedule,
  des_cblock*                   ivec,
  des_op_mode                   op_mode
);

#endif /* DES_CORE_H */
