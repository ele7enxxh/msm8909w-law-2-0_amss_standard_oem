#ifndef DS_DSD_DSC_H
#define DS_DSD_DSCM_H
/*===========================================================================

                      DS_DSD_DSC.H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2012-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_dsc.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/12/12    sk    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "comdef.h"


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  DS_DSC_COMMAND_INVALID              = 0,
  DS_DSC_PLMN_BLOCKING                = 1,
  DS_DSC_PS_DETACH                    = 2,
  DS_DSC_PLMN_BLOCKING_AND_PS_DETACH  = 3,
  DS_DSC_DISABLE_LTE                  = 4,
  DS_DSC_ENABLE_LTE                   = 5,
  DS_DSC_COMMAND_MAX = DS_DSC_ENABLE_LTE
}ds_dsd_dsc_command_type;


/*===========================================================================
FUNCTION      DS_DSD_DSC_COMMAND

DESCRIPTION   This is a DSC command interface
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_dsc_command
(
  sys_modem_as_id_e_type    subs_id,
  ds_dsd_dsc_command_type             command,
  uint32                              blocking_interval_value
);

/*===========================================================================
FUNCTION      DS_DSD_DSC_PUT_CMD

DESCRIPTION   Put DSC CMD to DS task
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  Commands CM module
===========================================================================*/
void ds_dsd_dsc_put_cmd
(
  ds_dsd_dsc_command_type cmd
);

#endif /* DS_DSD_DSC_H */
