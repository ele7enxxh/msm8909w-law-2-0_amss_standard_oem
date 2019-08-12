#ifndef SECPKXARM_H
#define SECPKXARM_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
            P U B L I C   K E Y   E X C H A N G E   R O U T I N E S


DESCRIPTION
  This is the header file for the Security Services PKX module.
  The PKX module provides functions to execute public key exchange
  operations and signing / authentication procedures using the 
  RSA algorithm.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2000-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secpkxarm.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC wrap.
03/09/10   ejt     added support for secapi_pkx_decrypt()
06/10/09   ejt     updated file to use secapi APIs
04/23/09   ejt     refined Malloc creation of secpkxarm_instance_s arrays for PKX algorithm
04/15/09   ejt     changed secpkxarm_instance_s stuct part of seccrypt_instance so it
                   does not allocate the large arrays for PKX. These arrays are allocated
                   and freed only if a PKX algorithm is used.
11/12/04   rv      changes to correct LINT errors
09/21/04   df      changes to support dsp-arm failover:
                   added proto for secpkxarm_platform_init()
02/19/04   rwh     Added this comment block.
                   Replaced literal values in modmath parameter size defs.
09/23/03   om      Added dummy DSP function pointer.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/


#include "comdef.h"

extern void secpkxarm_powerup_init( void );

extern void secpkxarm_platform_init
(
  seccrypt_instance_type*      instance_ptr
);

extern void secpkxarm_op_complete_cb
(
  seccrypt_instance_type*  instance_ptr
);

#define SECPKXARM_MAX_BITS      (SECAPI_PKX_MAX_MOD_SIZE * 8) 

/* Definitions from Frank Quick's MM code */
#define MM_WindowSize 4
#define MM_WindowStorage ((1 << MM_WindowSize) - 1)
#define MM_WindowMask ((1 << MM_WindowSize) - 1)

#define MM_BITCT (8*sizeof(uint32))
#define MM_LOW ((uint32)(((uint32)1L << (MM_BITCT/2)) - 1))
#define MM_MSB ((uint32)((uint32)1L << (MM_BITCT-1)))

/* set Nmax equal to the maximum number of bits divided by BITCT */

#define MM_Nmax ((SECPKXARM_MAX_BITS + MM_BITCT - 1) / MM_BITCT)

int secpkxarm_apwrxN
(
  seccrypt_instance_type* instance_ptr,
  int n, 
  uint32 *a, 
  uint32 *x, 
  uint32 *ax,
  uint32 *p
);

/* Just use pointers at this point. The arrays in this struct will 
   be allocated only if a PKX algoritm is used */
struct secpkxarm_instance_s
{
  /* The ARM implementation requires the parameters to be */
  /* uint32-swapped, so keep a swapped copy here ...      */
  uint32 *base;
  uint32 *exponent;
  uint32 *modulus;
  uint32 *result;

  /*  temporary storage:
        pb:   copy of modulus, dimension Nmax
       axb:   intermediate result, dimension Nmax
        at:   temp used for multiplications, dimension 2*Nmax+1
        ap:   window elements, dimension WindowStorage*Nmax      */
  uint32 *apwrxNtemp;

  /* For RSA messages we need them 32-bit 0-padded. Optimize this later */
  uint8  *rsa_base;
};

/* Define typedef stuct for element of structure secpkxarm_instance_s so we can 
   malloc these arrays independent of the creation of the seccrypr_instance */
typedef struct
{
  uint32 apwrxNtemp[1 + ((4 + MM_WindowStorage) * MM_Nmax)];
} secpkxarm_instance_temp_type;

/*===========================================================================

FUNCTION SECPKX_ABORT

DESCRIPTION
  This function will Abort the Diffie-Helman operation currently underway for
  the instance specified by the handle pointer parameter.

  There is no callback function invocation upon the completion this
  operation

DEPENDENCIES
  None.

RETURN VALUE
  Status of request.
    E_SUCCESS, if successful

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type secpkxarm_abort
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECPKXARM_CREATE_KEY

DESCRIPTION
  This function will create a key using the Diffie-Helman algorithm. The
  key created is dependent on the passed key type: Private, Public, or
  Secret. The Key will be written to the passed address.

  This function will use previously stored values for the base, modulus,
  and key information needed by the Diffie-Helman algorithm. 

  The implementation uses Montgomery Reduction and is based on 
  Frank Quick's original modmath code.

DEPENDENCIES
  The passed Crypto-instance must be initialized to perform the Diffie-Helman
  algorithm

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any Diffie-Helman parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secpkxarm_create_key
(
  seccrypt_instance_type*      instance_ptr
);

secerrno_enum_type secpkxarm_encrypt
(
  seccrypt_instance_type*  instance_ptr 
);

secerrno_enum_type secpkxarm_decrypt
(
  seccrypt_instance_type*  instance_ptr 
);

secerrno_enum_type secpkxarm_authenticate
(
  seccrypt_instance_type*  instance_ptr 
);

secerrno_enum_type secpkxarm_sign
(
  seccrypt_instance_type*  instance_ptr 
);

secerrno_enum_type secpkxarm_mod_exp
(
  seccrypt_instance_type*  instance_ptr 
);

#endif /* SECPKXARM_H */
