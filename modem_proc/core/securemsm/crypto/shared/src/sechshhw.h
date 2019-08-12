#ifndef SECHSHHW_H
#define SECHSHHW_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
      
                        H A S H I N G   R O U T I N E S

DESCRIPTION
  This is the header file for the Security Services hashing module.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2009 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/sechshhw.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC wrap.
06/18/09   bm      second version to suite CE_DAL, CE_HAL approach
                   funtional changes when initing, releasing the HW platform
11/12/07   df      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Type for a hash hw instance, included by the generic seccrypt instance
---------------------------------------------------------------------------*/
struct sechshhw_instance_s
{
   void *dummy;
};

/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION SECHSHW_POWERUP_INIT()

DESCRIPTION
  This function does all one-time initialization of DSP-specific hash data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void sechshhw_powerup_init( void );

/*===========================================================================

FUNCTION SECHSHHW_PLATFORM_INIT()

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
void sechshhw_platform_init(  seccrypt_instance_type* instance_ptr );

/*===========================================================================

FUNCTION SECHSHHW_CREATE_DIGEST

DESCRIPTION
  This function will create a message digest using the 
  specified algorithm executing on the ARM.

DEPENDENCIES
  The hash mode should be properly set by calling sechsh_set_param.

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sechshhw_create_digest(
                                         seccrypt_instance_type*  instance_ptr
                                         );

/*===========================================================================

FUNCTION SECHSHHW_START_HASH_OP()

DESCRIPTION
  This function send a START command to the hash applet.

DEPENDENCIES
  The hashing applet is ready to execute.

RETURN VALUE
  Status of state transition:
    E_SUCCESS if successful
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sechshhw_start_hash_op
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECHSHHW_CHECK_HASH_OP()

DESCRIPTION
  This function verifies the outcome of a hash operation.

DEPENDENCIES
  None

RETURN VALUE
  Status of state transition:
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sechshhw_check_hash_op
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECHSHHW_CONTINUE_HASH_OP()

DESCRIPTION
  This function sends a CONTINUE command to the hash applet.

DEPENDENCIES
  The hashing applet is ready to execute.

RETURN VALUE
  Status of state transition:
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sechshhw_continue_hash_op
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state  
);

/*===========================================================================

FUNCTION SECHSHHW_OP_COMPLETE_CB()

DESCRIPTION
  This function is used as command handler for a hashing-HW
  specific completion command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechshhw_op_complete_cb(  seccrypt_instance_type*  instance_ptr );

/*===========================================================================

FUNCTION SECHSHHW_FINISH_OP()

DESCRIPTION
  This function is used as command handler for a hashing-HW
  specific completion command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sechshhw_finish_op
(
seccrypt_instance_type*             instance_ptr,
seccrypts_instance_state_enum_type  old_state,
seccrypts_instance_state_enum_type  new_state  
);
#endif // SECHSHHW_H
