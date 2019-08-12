#ifndef DS707_TM_MGR_H
#define DS707_TM_MGR_H
/*===========================================================================
                         D S 7 0 7 _ T M _ M G R
GENERAL DESCRIPTION
  This file contains 3GPP2 Thermal Mitigation Functionality.

FUNCTIONS
 DS3GPP2_TM_POWERUP_INIT
   Performs power-up initialization of TM Manager and registers with DS3G.


 Copyright (c) 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk.h_v   1.2   19 Nov 2002 19:23:58   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_tm_mgr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
10/09/14   jc         First version of file.
===========================================================================*/


#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dstask_v.h"
#include "ds3geventmgr.h"
#include "ds3gcmif.h"

/*===========================================================================
                            TYPEDEFS
===========================================================================*/

typedef struct
{
  ds3geventmgr_th_mit_action_type action;

} ds707_TM_mgr_therm_mit_notify_cmd_info_type;

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS3GPP22_TM_MGR_POWERUP_INIT

DESCRIPTION   This function performs power-up initialization of 3GPP2 
              Thermal Mitigation Manager.This includes initialization of
              some parameters that are used to store current TM state and
              registers with DS3G for Thermal mitigation events.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_tm_mgr_powerup_init
(
  void
);

/*===========================================================================
FUNCTION      DS3GPP22_TM_MGR_PROCESS_THERM_MIT_CMD

DESCRIPTION   This function is registered as a call back function for 
              Thermal Mitigation events and processes actions that are passed
              by DS3G mgr.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_tm_mgr_process_therm_mit_cmd
(
   ds_cmd_type *ds_cmd_ptr
);
/*===========================================================================
FUNCTION      DS3GPP22_TM_MGR_IS_CALL_ALLOWED

DESCRIPTION   This function is called during Routing and before Physlink 
              bringup to check if the call is allowed according to
              curent mitigation level.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_tm_mgr_is_call_allowed
(
   ps_iface_apn_enum_type  apn_type
);
#endif /* DS707_TM_MGR_H  */
