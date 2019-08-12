/*===========================================================================
                     D S _ D S D _ F M C _  I F . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_fmc_if.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/11/11    sk    Created Module

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "data_msg.h"

#include "ps_sys.h"
#include "ps_sys_event.h"
#include "ps_sys_conf.h"
#include "ds_dsd_ext.h"
#include "ds_dsd_ext_i.h"
#include "ds_dsd_fmc_if.h"
#include "ds_dsd_pref_sys.h"
#include "rex.h"

#include "ds3gmgr.h"  //NOTE ruishi added to fix warning for ds3g_get_ps_subs_id. Can be removed after we pass subs id from mppm in fn ds_dsd_fmc_if_available

/*===========================================================================

                         GLOBAL DATA DECLARATIONS

===========================================================================*/
boolean ds_dsd_fmc_state = FALSE;

/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_FMC_IF_GET_FMC_STATE

DESCRIPTION   Utility function to get FMC state.

DEPENDENCIES  None.

RETURN VALUE  FMC state.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_fmc_if_get_fmc_state(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return ds_dsd_fmc_state;
}/* ds_dsd_fmc_if_fmc_available() */

/*===========================================================================
FUNCTION      DS_DSD_FMC_IF_SET_FMC_STATE

DESCRIPTION   Utility function to set FMC state.

DEPENDENCIES  None.

RETURN VALUE  SO MASK.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_fmc_if_set_fmc_state
(
  boolean fmc_state
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  rex_enter_crit_sect( &ds_dsd_critical_section );
  ds_dsd_fmc_state = fmc_state;
  rex_leave_crit_sect( &ds_dsd_critical_section );
  return;
}/* ds_dsd_fmc_if_set_fmc_state() */

/*===========================================================================
FUNCTION      ds_dsd_fmc_if_available

DESCRIPTION   This function notifies FMC availability to DSD

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_fmc_if_available
(
  boolean  fmc_available
)
{
  ps_sys_system_status_type dsd_sys_info;
  int16                     ps_errno;
  dsd_radio_type            dsd_pref_sys;
  ds3gsubsmgr_subs_id_e_type subs_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_MSG1(MSG_LEGACY_MED,"ds_dsd_fmc_if_available: available=%d",
            fmc_available);

  /* Cache FMC availability */
  ds_dsd_fmc_if_set_fmc_state(fmc_available);

  /* TODO: check if this needs to be per sub */
  subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(ds3g_get_ps_subs_id());

  dsd_pref_sys = ds_dsd_pref_sys_get_pref_system(subs_id);
  if(fmc_available == TRUE)
  {
    dsd_sys_info.preferred_network = PS_SYS_NETWORK_3GPP2;
    dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].rat_mask 
      = PS_SYS_RAT_3GPP2_FMC;

    if((dsd_pref_sys == DS_DSD_RADIO_1X) || (dsd_pref_sys == DS_DSD_RADIO_HDR))
    {
      dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].rat_mask =
        dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].rat_mask |
        ds_dsd_common_get_radio_rat_mask(dsd_pref_sys,
                                         FALSE,
                                         subs_id); 
	}

    dsd_sys_info.network_info[PS_SYS_NETWORK_3GPP2].so_mask = 0;

    DATA_MSG0(MSG_LEGACY_MED,"Notify external client");
    ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                    PS_SYS_CONF_SYSTEM_STATUS,
                       (ps_sys_subscription_enum_type)subs_id,
                       (void *)&dsd_sys_info,
                       &ps_errno);
  }
  else
  {
    ds_dsd_ext_notify_pref_system_external(DS_DSD_RADIO_INVALID,
                                           dsd_pref_sys,
                                           subs_id);
  }
  return;
}/* ds_dsd_fmc_if_available() */

