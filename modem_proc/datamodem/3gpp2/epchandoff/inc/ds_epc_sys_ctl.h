#ifndef DS_EPC_SYS_CTL_H
#define DS_EPC_SYS_CTL_H
/*===========================================================================

                      DS_EPC_SYS_CTL.H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2012-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_sys_ctl.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/20/12   fjia    Create the module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_EPC_HANDOFF
#ifdef FEATURE_DATA_WLAN_MAPCON
#include "dstask_v.h"
#include "ps_iface_defs.h"
#include "ds_epc_pdn_ctl.h"
#include "ds_epc_sm_defs.h"
#include "ps_sys_event.h"
#include "ds_mppm_ext_i.h"
#include "ps_sys.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------
Define the prefer system per PDN structure
--------------------------------------------------------------------------*/
typedef struct ds_epc_per_pdn_pref_sys_info_s ds_epc_per_pdn_pref_sys_info_type;
struct ds_epc_per_pdn_pref_sys_info_s
{
  uint8                         pref_index;
  uint8                         apn_name_len;
  char                          apn_name[DS_SYS_MAX_APN_LEN];
  boolean                       lte_attach_in_progress;
  uint8                         num_avail_rats;
  mppm_radio_type               avail_rat_index[DS_MPPM_RADIO_MAX];
};

/*-------------------------------------------------------------------------
Define the prefer system info type used by EPC module
--------------------------------------------------------------------------*/
typedef struct ds_epc_prefer_sys_info_s      ds_epc_prefer_sys_info_type;
struct ds_epc_prefer_sys_info_s
{
  ds_mppm_tid_type                        mppm_tid;
  uint8                                   num_pdn_config;
  uint8                                   default_index;
  ds_mppm_dsd_event_info_type             global_sys_info;
  ds_epc_per_pdn_pref_sys_info_type       per_pdn_sys_info[DS_MAX_PDN_COUNT];
};

/*-------------------------------------------------------------------------
Define the ds_epc_sys_info which contains 
prefer system info + tunnel call info. TODO: need to revisit.
--------------------------------------------------------------------------*/
typedef struct ds_epc_sys_info_s              ds_epc_sys_info_type;
struct ds_epc_sys_info_s
{
  ds_epc_prefer_sys_info_type                 pref_sys_info;
  boolean                                     lte_tunnel_enabled;
};

/*===========================================================================
FUNCTION      DS_EPC_SYS_CHG_CMD_PROC

DESCRIPTION   

PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_sys_chg_cmd_proc
(
  ds_cmd_type *cmd_ptr 
);                     

/*===========================================================================
FUNCTION      DS_EPC_SYS_EHRPD_S101_STATUS_CMD_PROC

DESCRIPTION   Process DS_EPC_eHRPD_S101_STATUS_CHG_PROC_CMD  command 
              from DS task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_ehrpd_s101_status_cmd_proc
(
  boolean tunnel_call_allowed,
  boolean lcp_throttled,
  boolean virtual_con_throttled,
  boolean hdr_actv_ho_in_prog,
  uint32  subs_id
);

#if 0
/*===========================================================================
FUNCTION      DS_EPC_IS_HDR_ACTV_HO_IN_PROG

DESCRIPTION   Utility function to get if HDR active ho is in progress

DEPENDENCIES  None.

RETURN VALUE  True if in progress, FALSE otherwise

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_is_hdr_actv_ho_in_prog(void);

/*===========================================================================
FUNCTION      DS_EPC_SET_HDR_ACTV_HO_IN_PROG

DESCRIPTION   Utility function to set the value of the flag: 
              HDR active ho is in progress

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_set_hdr_actv_ho_in_prog(boolean value);
#endif

/*===========================================================================
FUNCTION      DS_EPC_PREF_SYS_CHG_CBACK

DESCRIPTION   Called by MPPM module when the WWAN or WLAN system changes, or 
              the certain PDN policy changes. 

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pref_sys_chg_cback
(               
  ds_mppm_event_type         mppm_event,
  ds_mppm_event_info_s_type  *mppm_event_info_ptr,
  void                       *data_ptr
);
/*===========================================================================
FUNCTION      DS_EPC_PREF_SYS_CHG_CMD_PROC

DESCRIPTION   Process the prefer system change in DS task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pref_sys_chg_cmd_proc
(
  ds_epc_prefer_sys_info_type      *sys_info
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_TRANSLATE_SYS_TO_META_EV

DESCRIPTION   Utility function to translate the system status(Tech type, 
              so_mask, rat_mask) to the meta SM event. 

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_sys_ctl_translate_sys_to_meta_ev
(
  uint8                                    pref_idx,
  epc_pdn_meta_sm_input_event_type        *event,
  uint32                                   subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_REG_DS3GEVENTMGR_EVENT 

DESCRIPTION   Register ds3geventmgr event for subscription ids.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_ctl_reg_ds3geventmgr_event
(
  ds3geventmgr_event_type                     event_id,
  ps_sys_tech_enum_type                       tech_type,
  void                                      * user_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_DS3GEVENTMGR_EVENT_CB

DESCRIPTION   Callback function for registering ds3geventmgr event

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_ds3geventmgr_event_cb
(
  ds3geventmgr_event_type        event_id,
  ds3geventmgr_filter_type   *filter_info_ptr,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_GET_PREF_SYS_BY_APN_NAME

DESCRIPTION   Get the pref system of an APN.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_sys_ctl_get_pref_sys_per_apn
(
  uint16   apn_str_len,
  char    *apn_str,
  uint8   *pref_index,
  uint32   subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_GET_SYS_INFO

DESCRIPTION   Get the pointer of ds_epc_pref_sys_info.

PARAMETERS    sub_id: subscription id. 

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_prefer_sys_info_type* ds_epc_sys_ctl_get_sys_info
(
  uint32    subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_IS_LTE_TUNNEL_ENABLE

DESCRIPTION   Utility function to get if lte tunnel is enable or not

DEPENDENCIES  None.

RETURN VALUE  True if in progress, FALSE otherwise

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_is_lte_tunnel_enable
(
  uint32    subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_UPDATE_EPC_PREF_SYS

DESCRIPTION   Process the prefer system change in DS task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_sys_ctl_update_epc_pref_sys
( 
  epc_pdn_meta_sm_input_event_type          event,
  ds_epc_cb_type                            *epc_inst_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_IF_WLAN_AVAILABLE

DESCRIPTION   Check if WLAN is available

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_sys_ctl_if_wlan_available(void);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_IF_SYSTEM_AVAILABLE

DESCRIPTION   Check if SYSTEM is available. This function can only called in 
              DS task.
              THIS IS NOT A UTILITY FUNCTION and can only be used if the 
              checking system is EHRPD/LTE/IWLAN.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
boolean ds_epc_sys_ctl_if_system_available
(
  uint8  radio_idx,
  uint32 subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_IF_SRAT_AVAILABLE

DESCRIPTION   Check if SRAT is available. This function can only called in 
              DS task.
              THIS IS NOT A UTILITY FUNCTION and can only be used when we 
              trigger the handoff. EPC instance should be up/routable 
              and SRAT have to be one of EHRPD/LTE/IWLAN.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
boolean ds_epc_sys_ctl_if_srat_available(ds_epc_cb_type *instance_ptr,
                                             ps_iface_type *trat_iface_ptr);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_INIT

DESCRIPTION   Initialize the EPC system ctrl during power up.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_sys_ctl_init(void);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_GET_IS_ATTACH_PDN

DESCRIPTION   Get the is_attach_pdn flag of an APN. It searches the APN name in 
              ds_epc_pref_sys_info.per_pdn_sys_info table, if it cannot
              find the matching APN, returns FALSE.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_sys_ctl_get_is_attach_pdn
(
  uint16   apn_str_len,
  char    *apn_str,
  uint32   subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_GET_SYS_INFO_BY_APN_NAME

DESCRIPTION   Get the per_pdn_sys_info for a particular APN. If the APN is 
              not in the entry of MPPM table, return NULL. 

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
boolean  ds_epc_sys_ctl_get_sys_info_by_apn_name
(
  uint16                               apn_str_len,
  char                                *apn_str,
  ds_epc_per_pdn_pref_sys_info_type  **per_pdn_sys_info,
  uint32                               subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_IF_SYS_AVAIL_FOR_PDN

DESCRIPTION   Check if a particular radio is available for an EPC instance. 
 
PARAMETER     instance_ptr: EPC instance ptr. 
              sys_event: The system event corresponding to a radio.
              e.g. LTE -> EPC_PDN_META_SSM_LTE_EV.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
boolean ds_epc_sys_ctl_if_sys_avail_for_pdn
(
  ds_epc_cb_type                         *instance_ptr,
  epc_pdn_meta_sm_input_event_type        sys_event
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/

#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_SYS_CTL_H */
