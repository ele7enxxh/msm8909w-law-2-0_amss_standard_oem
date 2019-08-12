/*===========================================================================
                        mcfg_validate.h

DESCRIPTION

   External header file for modem_proc
 
Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_utils/inc/mcfg_validate.h#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
1/18/12      as/cy Create

===========================================================================*/

#ifndef MCFG_VALIDATE_H
#define MCFG_VALIDATE_H

#include "mcfg_common.h"
#include "nv_items.h"

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/

/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

/*==============================================================================

FUNCTION  mcfg_validate_config() 

DESCRIPTION
  This is the modem validation routine called to compare a modem
  configuration with current NV / EFS item settings

DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  mcfg_error_e_type - MCFG_ERR_NONE if success, otherwise some error code

SIDE EFFECTS
  None

===========================================================================*/
mcfg_error_e_type mcfg_validate_config (mcfg_config_type_e_type current_mcfg_type,
                                        mcfg_config_id_s_type *mcfg_current_id, 
                                        mcfg_sub_id_type_e_type mcfg_subs_num);

#endif /* MCFG_VALIDATE_H */

