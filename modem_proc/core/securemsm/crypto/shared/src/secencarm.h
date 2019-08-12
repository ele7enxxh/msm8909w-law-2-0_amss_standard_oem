#ifndef SECENCARM_H
#define SECENCARM_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                     C I P H E R I N G   R O U T I N E S


DESCRIPTION
  This is the header file for the Security Services ciphering module.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2000-2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secencarm.h#1 $
  $DateTime: 2010/08/23 07:56:34
  $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC wrap.
10/31/03   yhw     Added AES128 ARM support.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                         DEFINITIONS AND CONSTANTS

===========================================================================*/

//#define DES_KEY_SZ 	(sizeof(des_cblock))
//#define DES_ENCRYPT	1
//#define DES_DECRYPT	0

//#define AES_KS_LENGTH 64


/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/* DES definitions */
//typedef uint8 des_cblock[8];
//typedef struct des_ks_struct
//{
//  des_cblock _;
//} des_key_schedule[16];

//typedef enum arc4EncDec
//{
//  ARC4_ENCRYPT = 1, 
//  ARC4_DECRYPT = 2
//} arc4_enc_dec_enum_type;

//typedef uint8 arc4_sbox[256];
//typedef struct arc4_ks_struct /* This could move to the ARC4 file. */
//{
//  arc4_sbox sbox;
//} arc4_key_schedule;

/* AES definitions */
//typedef struct                      /* the AES context for encryption   */
//{   uint32    k_sch[AES_KS_LENGTH]; /* the encryption key schedule      */
//    uint32    n_rnd;                /* the number of cipher rounds      */
//    uint32    n_blk;                /* the number of bytes in the state */
//} aes_ctx;


/*---------------------------------------------------------------------------
  ARM-spcific cipher instance definition
---------------------------------------------------------------------------*/
struct secencarm_instance_s
{
  boolean  enc_subkeys_generated;
  boolean  dec_subkeys_generated;
};


/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECENCARM_POWERUP_INIT()

DESCRIPTION
  This function does one-time initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
extern void secencarm_powerup_init( void );

/*===========================================================================

FUNCTION SECENCARM_PLATFORM_INIT()

DESCRIPTION
  Re-initialization of ARM platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void secencarm_platform_init
(
  seccrypt_instance_type*      instance_ptr
);

/*===========================================================================

FUNCTION SECENCARM_ENCRYPT()

DESCRIPTION
  This function will encrypt the passed message using the instance-specific
  algorithm.
  Based on the MAC mode, it will create the specified MAC if necessary.

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
secerrno_enum_type secencarm_encrypt
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECENCARM_DECRYPT()

DESCRIPTION
  This function will decrypt the passed message using the instance-specific
  algorithm.
  Based on the MAC mode, it will verify the specified MAC if necessary.

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
secerrno_enum_type secencarm_decrypt
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECENCARM_OP_COMPLETE_CB()

DESCRIPTION
  This function is called by the SECCRYPT_OP_COMPLETION_CMD
  processor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All registered entries are cleared.
===========================================================================*/
extern void secencarm_op_complete_cb
(
  seccrypt_instance_type*  instance_ptr
);


/*===========================================================================

                     INTERNAL FUNCTION DECLARATIONS

===========================================================================*/




#endif /* SECENCARM_H */
