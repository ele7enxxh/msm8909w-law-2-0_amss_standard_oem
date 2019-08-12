#ifndef SECHSHARM_H
#define SECHSHARM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       S E C U R I T Y   S E R V I C E S

                       H A S H I N G   F U N C T I O N S

                   ( A R M   I M P L E M E N T A T I O N S )

GENERAL DESCRIPTION
  This sofware module contains the source code for the Security Services
  Crypto-Interface layer for message digest specific to the ARM.

EXTERNAL FUNCTIONS

  sechsh_create_digest() - Creates the message digest

Copyright (c) 2003-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/sechsharm.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework - Removed FEATURE_SEC wrap.
09/21/04   df      added proto for sechsharm_platform_init to support dsp-arm
                   failover.
05/04/04   rwh     Removed ARM-platform-specific fields.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                     CONSTANTS AND TYPE DECLARATIONS

===========================================================================*/

typedef struct 
{
  void*     dummy_ptr;
} sechsharm_ctx_type;

struct sechsharm_instance_s
{
  sechsharm_ctx_type  ctx;
};

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

extern void sechsharm_powerup_init( void );

/*===========================================================================

FUNCTION SECHSHARM_CREATE_DIGEST

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

secerrno_enum_type sechsharm_create_digest
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECHSHARM_PLATFORM_INIT()

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
extern void sechsharm_platform_init
(
  seccrypt_instance_type*      instance_ptr
);

extern void sechsharm_op_complete_cb
(
  seccrypt_instance_type*  instance_ptr
);

#endif /* SECHSHARM_H */
