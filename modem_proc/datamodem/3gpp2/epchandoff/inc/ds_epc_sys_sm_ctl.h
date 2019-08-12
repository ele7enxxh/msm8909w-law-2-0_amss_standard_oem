#ifndef DS_EPC_SYS_SM_CTL_H
#define DS_EPC_SYS_SM_CTL_H
/*===========================================================================

                      DS_EPC_SYS_SM_CTL. H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2009 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_sys_sm_ctl.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
02/08/13    fjia    If FEATURE_DATA_WLAN_MAPCON is defined, don't compile 
                    this file. 
09/01/11    fjia    Created Module
===========================================================================*/
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_EPC_HANDOFF
#ifndef FEATURE_DATA_WLAN_MAPCON
#include "stm2.h"
#include "dstask_v.h"
#include "ps_iface_defs.h"
#include "ds_epc_pdn_ctl.h"
#include "ps_sys_event.h"
#include "ps_sys.h"


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
FUNCTION      DS_EPC_SYS_3GPP2_BEARER_CHG_PROC

DESCRIPTION   Called by ds707 Mode Handler for bearer tech changed.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_3gpp2_bearer_chg_proc
(
  uint32  so_mask,
  uint32  rat_mask
);

/*===========================================================================
FUNCTION      DS_EPC_HDLR_HANDOFF_INIT_CMD_PROCESS

DESCRIPTION

PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_hdlr_handoff_init_cmd_process
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_GET_CURR_STATE

DESCRIPTION   Get current system state info.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
stm_state_t ds_epc_sys_ctl_get_curr_state(void);

/*===========================================================================
FUNCTION      DS_EPC_PS_SS_LTE_ATTACH_INIT_EV_HDLR

DESCRIPTION   Posted when lte attach procedure is started.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_ps_ss_lte_attach_init_ev_hdlr
(
  uint16  attach_pf_id
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_LTE_ATTACH_INIT_CMD_PROC

DESCRIPTION   Process LTE_ATTACH_INIT cmd.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_lte_attach_init_cmd_proc
(
  uint16  attach_pf_id
);

/*===========================================================================
FUNCTION      DS_EPC_PS_SS_BEARER_TECH_CHG_EV_HDLR

DESCRIPTION   Post by PS framework for system bearer tech change in DS task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_ps_ss_bearer_tech_chg_ev_hdlr
(
  uint32   tech_type,
  uint32   so_mask,
  uint32   rat_mask
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_BEARER_TECH_CHG_CMD_PROC

DESCRIPTION   Process bearer tech change command in DS task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_bearer_tech_chg_cmd_proc
( 
  uint32 tech_type,
  uint32 so_mask, 
  uint32 rat_mask
);

/*===========================================================================
FUNCTION      DS_EPC_PS_SS_EHRPD_S101_STATUS_CHG_EV_HDLR

DESCRIPTION   Posted when there is a change in availability of S101 tunnel

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_ps_ss_eHRPD_s101_status_chg_ev_hdlr(void);

/*===========================================================================
FUNCTION      DS_EPC_SYS_EHRPD_S101_STATUS_CHG_CMD_PROC

DESCRIPTION   Process S101 tunnel status update cmd from DS task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_ehrpd_s101_status_chg_cmd_proc(void);

/*===========================================================================
FUNCTION      DS_EPC_DATA_CTX_TIMER_EXP_HDLR

DESCRIPTION   Function processes EPC data context timer expiry on DS task
              context.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_data_ctx_timer_exp_hdlr(void);

/*===========================================================================
FUNCTION      DS_EPC_DATA_CTX_TIMER_EXP_CMD_PROC

DESCRIPTION   Process epc data context timer expire cmd in ds task.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_data_ctx_timer_exp_cmd_proc(void);

/*===========================================================================
FUNCTION      DS_EPC_SYS_SM_DS3GEVENTMGR_EVENT_CB

DESCRIPTION   Callback function for ds3geventmgr

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_sm_ds3geventmgr_event_cb
(
  ds3geventmgr_event_type                        event_name,
  ds3geventmgr_filter_type                     * filter_info_ptr,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
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
  boolean hdr_actv_ho_in_prog
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_REG_DS3GEVENTMGR_EVENT 

DESCRIPTION   Register ds3g event

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_reg_ds3geventmgr_event
(
  ds3geventmgr_event_type                     event_id,
  ps_sys_tech_enum_type                       tech_type,
  void                                      * user_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_HANDLE_LTE_ATTACH_INIT_FAIL

DESCRIPTION   Send LTE_ATTACH_INIT_RESPONCE ioctl to LTE MH in case EPC 
              receives LTE_ATTACH_INIT event, but failure happens during event
              handling.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_sys_handle_lte_attach_init_fail(void);

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

#endif /* FEATURE_DATA_WLAN_MAPCON */
#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_SYS_SM_CTL_H */
