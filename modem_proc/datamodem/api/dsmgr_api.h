#ifndef DSMGR_API_H
#define DSMGR_API_H
/*===========================================================================

                   D A T A   S E R V I C E S   M A N A G E R

                    E X T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This file contains functions and definitions exported by DSMGR to other 
  Modem Sub Systems.

Copyright (c) 2009 - 2010 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/dsmgr_api.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
07/07/10   ms      Data statistics support for Field Test Display.
10/15/09   vs      SU Level Api Defeaturization
09/04/09   sn      Initial version of DSMGR API header file exported to other 
                   modem sub systems.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "sys.h"                               /* system wide common types */
#include "ds_flow_control.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3G_IS_PREF_PKT_MODE

DESCRIPTION   This function determines if the specified mode is the currently 
              preferred mode for packet data calls.
  
DEPENDENCIES  None.

RETURN VALUE  TRUE: mode is preferred pkt mode.
              FALSE: mode is not preferred pkt mode.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3g_is_pref_pkt_mode
(
  sys_sys_mode_e_type  mode
);

/*===========================================================================
FUNCTION      DS3G_IS_PREF_PKT_MODE_EX

DESCRIPTION   This function determines if the specified mode is the currently 
              preferred mode for packet data calls on passed sub.
  
DEPENDENCIES  None.

RETURN VALUE  TRUE: mode is preferred pkt mode.
              FALSE: mode is not preferred pkt mode.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds3g_is_pref_pkt_mode_ex
(
  sys_sys_mode_e_type     mode,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_INBOUND_FLOW

DESCRIPTION
  This function controls the enabling or diabling of inbound flow ctl.
  This function was required because inbound flow ctl. can be changed 
  from several places for different reasons, and we need an arbitrator
  function to handle those changes.
  
  Basically, the function enables inbound flow, when no caller wants it
  disabled. If even one caller wants the flow disabled, the inbound flow
  is disabled.
  
DEPENDENCIES
  This is the only function that should be used to toggle the inbound flow.
  This is important because the function keeps track of the state of inbound
  flow, and if the state of inbound flow is changed

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_siolib_set_inbound_flow
(
  uint32 in_mask,                        /* input mask of calling function */
  ds3g_flow_e_type flow  /* specifies if flow is to be enabled or disabled */
);

/*===========================================================================
FUNCTION DS3G_GET_STATS_INFO

DESCRIPTION
  This function is an Interface to CM to get the required DS Statistics
  information from DS. This function calls the API provided by the
  HDRRLPSTATS module to get the statistics.

DEPENDENCIES
  None

PARAMETERS
  ds_info - Structure that gets filled with statistics for the mask
            set by CM in ds3g_update_stats_mask().

RETURN VALUE
  True if statistics data is succesfully copied to ds_info pointer.
  Else FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_get_stats_info 
(
  sys_ds_information_s_type  *ds_info
);

/*===========================================================================
FUNCTION DS3G_UPDATE_STATS_MASK

DESCRIPTION
  This function is an Interface to CM to Set the DS Statistics Mask;
  This function posts a command to DS task.

DEPENDENCIES
  None

PARAMETERS
  Bitmask of type uint16

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_update_stats_mask
(
  uint16  bitmask
);
#endif /* DSMGR_API_H */
