#ifndef SECENCHW_H
#define SECENCHW_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                     C I P H E R I N G   R O U T I N E S


DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secenchw.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC
06/18/09   bm      second version to suite CE_DAL, CE_HAL approach
                   funtional changes when initing, releasing the HW platform
11/12/07   df      created
===========================================================================*/


#include "comdef.h"

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/
#define SECENCHW_BLOCK_SIZE 16
#define SECENCHW_SWAP_FALSE FALSE
#define SECENCHW_SWAP_TRUE TRUE

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/
struct secenchw_instance_s
{
   uint32   ce_cntx;
   boolean  enc_use_hwkeys;
   boolean  dec_use_hwkeys;
   boolean  enc_subkeys_generated;
   boolean  dec_subkeys_generated;

   /* ctr mode temp buf */
   uint8*   tbuf_ptr;
};

/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION SECENCW_POWERUP_INIT()

DESCRIPTION
  This function does all one-time initialization of DSP-specific cipher data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void secenchw_powerup_init( void );

/*===========================================================================

FUNCTION SECENCHW_PLATFORM_INIT()

DESCRIPTION
  Re-initialization of HW platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void secenchw_platform_init( seccrypt_instance_type* instance_ptr );

/*===========================================================================
FUNCTION SECENCHW_ENCRYPT()

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
secerrno_enum_type secenchw_encrypt( seccrypt_instance_type*  instance_ptr );

/*===========================================================================
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
secerrno_enum_type secenchw_decrypt( seccrypt_instance_type*  instance_ptr );

/*===========================================================================

FUNCTION SECENCHW_START_CIPHER_OP()

DESCRIPTION
  This function send a START command to the cipher applet.

DEPENDENCIES
  The ciphering applet is ready to execute.

RETURN VALUE
  Status of state transition:
    E_SUCCESS if successful
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenchw_start_cipher_op
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECENCHW_CHECK_CIPHER_OP()

DESCRIPTION
  This function verifies the outcome of a cipher operation.

DEPENDENCIES
  None

RETURN VALUE
  Status of state transition:
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenchw_check_cipher_op
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECENCHW_OP_COMPLETE_CB()

DESCRIPTION
  This function is used as command handler for a ciphering-HW
  specific completion command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void secenchw_op_complete_cb(  seccrypt_instance_type*  instance_ptr );

/*===========================================================================

FUNCTION SECENCHW_FINISH_OP()

DESCRIPTION
  This function is used as command handler for a ciphering-HW
  specific completion command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type secenchw_finish_op
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state  
);
#endif // SECENCHW_H
