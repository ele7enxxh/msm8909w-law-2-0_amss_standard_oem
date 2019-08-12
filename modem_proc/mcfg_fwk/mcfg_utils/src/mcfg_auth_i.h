#ifndef MCFG_AUTH_I_H
#define MCFG_AUTH_I_H
/*==============================================================================

                M O D E M   C O N F I G   A U T H
 
GENERAL DESCRIPTION
  Internal header for MCFG AUTH library

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/src/mcfg_auth_i.h#1 $ 
$DateTime: 2016/12/13 08:00:27 $ 
$Author: mplcsds1 $ 
$Change: 11985219 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/

#include "comdef.h"

#include "msg.h"

#include "mcfg_common.h"
#include "mcfg_utils.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_auth_check_config

  DESCRIPTION
    This function authenticates the config file

  DEPENDENCIES
    None

  PARAMETERS
      config [in] config structure including config size and buffer pointer to be authenticated
    
  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_auth_check_config
(
  mcfg_config_s_type *config
);

/*===========================================================================

  FUNCTION mcfg_auth_init

  DESCRIPTION
    Initialize mcfg auth

  DEPENDENCIES
    None

  PARAMETERS
    None
    
  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_auth_init
(
  void
);

#endif /* MCFG_AUTH_I_H */
