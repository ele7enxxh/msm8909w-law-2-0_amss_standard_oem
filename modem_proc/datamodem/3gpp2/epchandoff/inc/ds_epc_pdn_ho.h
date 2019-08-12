#ifndef DS_EPC_PDN_HO_H
#define DS_EPC_PDN_HO_H
/*===========================================================================
                    DS_EPC_PDN_HO_SUB_SM_HDLR.H

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2011-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_pdn_ho.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/09/11    fjia    Created Module
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "__ds_epc_pdn_ho_sub_sm_int.h"
#include "ds_epc_sm_defs.h"
#include "ds_epc_pdn_ctl.h"
#include "ps_acl.h"
#include "sys.h"
#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds_epc_ho_throttle_mgr.h"
#endif 
/*---------------------------------------------------------------------------
reconfig physical iface reason
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_PDN_HO_RESYNC_SRAT = 0,
  DS_EPC_PDN_HO_RESYNC_TRAT = 1,
  DS_EPC_PDN_HO_RESYNC_MAX
}ds_epc_pdn_ho_assoc_rat_enum_type;

#ifdef FEATURE_DATA_WLAN_MAPCON
/*---------------------------------------------------------------------------
HO throttle decision type used by func ds_epc_pdn_ho_throttle_decision()
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_PDN_HO_THROTTLE_PENDING_THROTTLE = 0,
  DS_EPC_PDN_HO_THROTTLE_SET_THROTTLE = 1,
  DS_EPC_PDN_HO_THROTTLE_COMP_SUCCESS = 2,
  DS_EPC_PDN_HO_THROTTLE_ERROR = 3
}ds_epc_pdn_ho_throttle_decis_type;
#endif 

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_SM_INSTANCE_INIT()

DESCRIPTION   
	 
PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_sm_instance_init( uint32 sm_id, void *info );

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_SM_INSTANCE_DEACTIVE()

DESCRIPTION   
	 
PARAMETERS    cmd_ptr   - Ptr to the command information

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_sm_instance_deactive( uint32 sm_id, void *info);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_GET_STATE

DESCRIPTION   

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
stm_state_t ds_epc_pdn_ho_get_state( uint32 sm_id );

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_SM_INPUT_HANDLER()

DESCRIPTION   
	 
PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_sm_input_handler
( 
  epc_pdn_ho_sub_sm_input_event_type input, 
  ds_epc_cb_type *epc_pdn_instance,
  void *info
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_REG_TRAT_HANDOFF_EVENTS

DESCRIPTION: Register TRAT UP/DOWN events.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_pdn_ho_reg_trat_handoff_events
(
  ds_epc_cb_type     * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_RESET_HANDOFF_DATA

DESCRIPTION  Reset handoff related data.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_reset_handoff_data(ds_epc_cb_type *instance_ptr);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_IS_CTXT_FLAG_ON

DESCRIPTION  Check if a given context flag is set on a given pdn.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_is_ctxt_flag_on
(
  ds_epc_cb_type *instance_ptr,
  uint8           ctxt_flag_mask
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_SET_CTXT_FLAG_MASK

DESCRIPTION  set handoff context flag mask. 
            e.g., transfer blocking cause.
             - block when
                 -- mismatch
                 -- srat context
                 -- ip comparison
                 -- primary addr apply
                 -- trat down unrecoverable failure and pdn is not throttled

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_set_ctxt_flag_mask
(
  ds_epc_cb_type *instance_ptr,
  uint8           ctxt_blk_cause_mask
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_RESET_CTXT_FLAG_MASK

DESCRIPTION  reset handoff context flag for the input.
             - reset
               -- pdn is found throttled -- unblock this pdn for unrecoverable
                  cause
               -- S101 prereg allow received  -- unblock for the following
                  -- ip comparison
                  -- primary addr apply
                  -- trat down unrecoverable failure
               -- epc iface down - unblock all

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_reset_ctxt_flag_mask
(
  ds_epc_cb_type *instance_ptr,
  uint8           ctxt_unblk_mask
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_IS_PREREG_LOCALLY_DISABLED

DESCRIPTION  Check if the prereg is locally disabled.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_is_prereg_locally_disabled 
(
  ds_epc_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_PROCESS_TRAT_IFACE_UP_EV

DESCRIPTION   process TRAT UP event for ho sm.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_process_trat_iface_up_ev
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr,
  boolean                    srat_down //Indicate if SRAT already in down state
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_PROCESS_TRAT_IFACE_DOWN_EV

DESCRIPTION   process TRAT DOWN event for ho sm.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_process_trat_iface_down_ev
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_TRAT_IFACE_DOWN_EVENT_HANDLER_CB

DESCRIPTION   This callback function is called when one of the registered
              ps_iface events occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_trat_iface_down_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_PROCESS_TRAT_IFACE_CONFIG_EV_PREREG

DESCRIPTION   process TRAT CONFIGURING event for ho sm.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_process_trat_iface_config_ev_prereg
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_PROCESS_TRAT_IFACE_UP_EV_PREREG

DESCRIPTION   process TRAT UP event for ho sm.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_process_trat_iface_up_ev_prereg
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_PROCESS_TRAT_IFACE_DOWN_EV_PREREG

DESCRIPTION   process TRAT DOWN event in prereg for ho sm.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_process_trat_iface_down_ev_prereg
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_REG_TRAT_IFACE_CONFIG_EVENT

DESCRIPTION: alloc and register trat iface IFACE_CONFIG event.

DEPENDENCIES  None.

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_pdn_ho_reg_trat_iface_config_event
(
  ds_epc_cb_type     * instance_ptr
);

/*===========================================================================
FUNCTION      ds_epc_pdn_ho_dereg_trat_iface_config_event

DESCRIPTION: de-alloc and de-register trat iface IFACE_CONFIG event.

DEPENDENCIES  None.

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ho_dereg_trat_iface_config_event
(
  ds_epc_cb_type     * instance_ptr
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_REG_SRAT_IFACE_EVENT

DESCRIPTION: alloc and register srat iface IFACE_DOWN event.

DEPENDENCIES  None.

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_pdn_ho_reg_srat_iface_event
(
  ds_epc_cb_type     * instance_ptr
);

/*===========================================================================
FUNCTION      ds_epc_pdn_ho_dereg_srat_iface_event

DESCRIPTION: de-alloc and de-register srat iface IFACE_DOWN event.

DEPENDENCIES  None.

RETURN VALUE  TRUE/FALSE

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ho_dereg_srat_iface_event
(
  ds_epc_cb_type     * instance_ptr
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_HANDOFF_SWAP

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_handoff_swap
(
  ds_epc_cb_type            * instance_ptr, //EPC instance pointer
  boolean                     srat_down
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_RESYNC_RAT

DESCRIPTION  resonfigure an associated rat.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_resync_rat
(
  ds_epc_cb_type                     *instance_ptr,
  ds_epc_pdn_ho_assoc_rat_enum_type   rat_type
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_ABORT_HANDOFF

DESCRIPTION  abort handoff.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_abort_handoff
(
  ds_epc_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_TEARDOWN_TRAT

DESCRIPTION   Tear down the EPC trat

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_teardown_trat
(
  ds_epc_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_RECOVER_SRAT

DESCRIPTION   resync and cancel srat timer cleanup if it is running for the 
              EPC instance.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_recover_srat
(
  ds_epc_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_RECOVER_SRAT

DESCRIPTION   Process srat timer expiry

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_srat_timer_expiry
(
  ds_epc_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_PROC_IFACE_HANDOFF

DESCRIPTION   This function processes handoff for one EPC instance.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_proc_iface_handoff
(
  ds_epc_cb_type                  * instance_ptr,
  ps_iface_handoff_class_enum_type  handoff_class
);

/*===========================================================================
FUNCTION      DS_EPC_CTRL_RESET_TRAT_DOWN_REASON

DESCRIPTION   This cfunction reset netdown reason.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_ctrl_reset_trat_down_reason
( 
  ds_epc_cb_type              * instance_ptr
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      ds_epc_pdn_ho_is_trat_down_PGW_failure

DESCRIPTION   This function checks the cause code from physical iface down to 
              see if it PGW failure.

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: PGW failure/
                          non PGW failure
SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_is_trat_down_PGW_failure
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr
);
/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_GET_NET_DOWN_REASON

DESCRIPTION   Retrieves iface net down reason from iface down event info.
			  
DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
ps_iface_net_down_reason_type ds_epc_pdn_ho_get_net_down_reason
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_GET_IFACE_RAT

DESCRIPTION   Get the iface rat info by iface down event info
			  
DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
ds_sys_rat_ex_enum_type ds_epc_pdn_ho_get_iface_rat
(
  ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_GET_TRAT_IFACE_PTR

DESCRIPTION   Retrieves TRAT iface pointer from input instance pointer 
              and returns the same.
			  
DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
ps_iface_type* ds_epc_pdn_ho_get_trat_iface_ptr(
                             ds_epc_iface_ev_data_type *epc_iface_ev_data_ptr);

/*===========================================================================
FUNCTION      DS_EPC_SYS_CTL_SEND_PDN_HO_THROTTLE_IOCTL

DESCRIPTION   Sends PS SYS IOCTL to ti the TRAT MH to throttle HO.

DEPENDENCIES  DEPENDENCIES  TRAT iface should be valid to send the IOCTL.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
int16 ds_epc_pdn_ho_send_pdn_ho_throttle_ioctl(ds_epc_cb_type *instance_ptr,
                                              ps_iface_type  *trat_iface_ptr,
                            ps_iface_net_down_reason_type   call_end_reason);

#endif /*FEATURE_DATA_WLAN_MAPCON*/	
/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_PROC_HO_RSP_SUCCESS

DESCRIPTION: The function is called after a HO RSP sucess is received by
             a pre-reg completed PDN. Resync TRAT.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_proc_ho_rsp_success
(
  ds_epc_cb_type  * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_TRAT_IFACE_UP_EVENT_HANDLER_CB

DESCRIPTION   This callback function is called when one of the registered
              ps_iface events occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_trat_iface_up_event_handler_cb
(
  ps_iface_type             * this_iface_ptr,
  ps_iface_event_enum_type    event,
  ps_iface_event_info_u_type  event_info,
  void                      * user_data
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_TRAT_IFACE_CONFIG_EVENT_HANDLER_CB

DESCRIPTION   This callback function is called when one of the registered
              ps_iface events occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_trat_iface_config_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_SRAT_IFACE_DOWN_EVENT_HANDLER_CB

DESCRIPTION   This callback function is called when one of the registered
              ps_iface events occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_srat_iface_down_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_TRAT_IFACE_DOWN_EVENT_HANDLER_CB

DESCRIPTION   This callback function is called when one of the registered
              ps_iface events occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_proc_input
(
  epc_pdn_ho_sub_sm_input_event_type input, 
  ds_epc_cb_type *epc_pdn_instance,
  void *info
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_SM_SET_USER_DATA

DESCRIPTION   

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_sm_set_user_data( uint32 sm_id, void *user_data);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_SET_HANDOFF_CLASS

DESCRIPTION   This function sets the handoff class in acl policy saved in the 
              EPC instance.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_set_handoff_class
(
  ds_epc_cb_type                  * instance_ptr,
  ps_iface_handoff_class_enum_type  handoff_class
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_COMPARE_PRIMARY_IP_ADDR

DESCRIPTION   This function compares v4 or v6 addresses stored in the srat and 
              trat.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_compare_primary_ip_addr
(
  ds_epc_cb_type              * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_APPLY_IP_CFG

DESCRIPTION   This function applies the IP config from srat to trat 
              for one EPC instance.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_epc_status_type ds_epc_pdn_ho_apply_ip_cfg
(
  ds_epc_cb_type              * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_IS_PDN_TRANSFERRABLE

DESCRIPTION: check if EPC should try handoff.

DEPENDENCIES:

RETURN VALUE  TRUE/FALSE for pass/no pass

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_is_pdn_transferrable
(
  ds_epc_cb_type     * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_SM_GET_PDN_INSTANCE_PTR

DESCRIPTION   
	 
PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_epc_cb_type* ds_epc_pdn_ho_sm_get_pdn_instance_ptr
(
  stm_state_machine_t*  sm
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_SM_GET_PDN_INSTANCE_PTR

DESCRIPTION   Verify trat event info and print
	 
PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_verify_trat_ev
(
  ds_epc_iface_ev_data_type*    epc_iface_ev_data_ptr
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_VERIFY_SRAT_EV

DESCRIPTION   Verify srat event info and print
	 
PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_verify_srat_ev
(
  ds_epc_iface_ev_data_type*    epc_iface_ev_data_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_FLOW_DISABLE

DESCRIPTION   Disable the flow on the logical iface if the SRAT service is 
              not available during the handoff.

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_flow_disable
(
  ds_epc_cb_type *instance_ptr 
);
/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_FLOW_ENABLE

DESCRIPTION   Enable the flow on the logical iface.

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_flow_enable
(
  ds_epc_cb_type *instance_ptr 
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_FLOW_FLAG_RESET

DESCRIPTION   Reset the is_disable flag to FALSE.  

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_flow_flag_reset
(
  ds_epc_cb_type *instance_ptr 
);
/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_FLOW_CTL_BY_SRAT_STATUS

DESCRIPTION   Enable flow when srat is available; Disable flow when srat is not 
              available. 

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_flow_ctl_by_srat_status
(
   ds_epc_cb_type *instance_ptr
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_EPC_IFACE_TEAR_DOWN_ON_CAUSE_CODE

DESCRIPTION   This function checks the cause code from physical iface down
              event info and determines if the EPC iface should be torn down.

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: yes/no

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_iface_tear_down_on_cause_code
(
  ps_iface_net_down_reason_type iface_call_end_reason
);
/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_HANDOFF_FAILURE

DESCRIPTION   Call PS framework for handoff failure and reset the handoff data.
              And enable flow when FEATURE_DATA_WLAN_MAPCON is on. 

PARAMETERS    

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_handoff_failure
(
  ds_epc_cb_type *instance_ptr,
  int16          *ps_errno 
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_EPC_PDN_GET_APN_FROM_POLICY

DESCRIPTION   Get the APN name from current EPC inst's ploicy. 
              Can only used during post proc. 

PARAMETERS    Current EPC instance ptr 
         

DEPENDENCIES  None

RETURN VALUE  pointer to the APN name

SIDE EFFECTS  None
===========================================================================*/
ds_profile_info_type* ds_epc_pdn_get_apn_baseon_policy
(
  ds_epc_cb_type * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_REG_COMP_TRAT_HANDOFF_EVENTS

DESCRIPTION: Register comp inst's TRAT UP/DOWN events.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_pdn_ho_reg_comp_trat_handoff_events
(
  ds_epc_cb_type     * instance_ptr,
  ps_iface_type      * comp_trat_iface_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_DEREG_COMP_TRAT_HANDOFF_EVENTS

DESCRIPTION:  De-register comp inst's TRAT UP/DOWN events.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_pdn_ho_dereg_comp_trat_handoff_events 
(
  ds_epc_cb_type     * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_COMP_TRAT_IFACE_UP_EVENT_HANDLER_CB 

DESCRIPTION   This callback function is called when one of the registered
              ps_iface events occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_comp_trat_iface_up_event_handler_cb
(
  ps_iface_type             * this_iface_ptr,
  ps_iface_event_enum_type    event,
  ps_iface_event_info_u_type  event_info,
  void                      * user_data
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_COMP_TRAT_IFACE_DOWN_EVENT_HANDLER_CB

DESCRIPTION   This callback function is called when one of the registered
              ps_iface events occurs.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_pdn_ho_comp_trat_iface_down_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_THROTTLE_REPORT

DESCRIPTION   Once EPC state machine has decide to throtle, call this function 
              to process throttle request. This function will decide will the
              throttle request send to iwlan MH or throttle mgr base on TRAT
              tech type.

PARAMETERS    Current EPC instance ptr

DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_throttle_report
(
  ps_iface_type                 * trat_iface_ptr,
  ds_epc_cb_type                * instance_ptr,
  void                          * payload 
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_THROTTLE_DECISION

DESCRIPTION   Determine if throttle needed

PARAMETERS    Current EPC instance ptr

DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_throttle_decision
(
  ds_epc_cb_type                * instance_ptr,
  void                          * payload
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_EPC_PDN_SET_LOCAL_ABORT

DESCRIPTION   This function sets the local abort flag by calling IFACE IOCTL 
               
			  
DEPENDENCIES  None.

RETURN VALUE 
              void 

SIDE EFFECTS  None  
===========================================================================*/
void ds_epc_pdn_set_local_abort
(
  ps_iface_type   *ps_iface_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PDN_HO_IF_TEAR_DOWN_PDN_LOCAL

DESCRIPTION   Determine if tear down PDN locally.

PARAMETERS    Current EPC instance ptr 

DEPENDENCIES  None

RETURN VALUE  TRUE: tear down the PDN locally. 
              FALSE: Don't need to tear down PDN locally.

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_pdn_ho_if_tear_down_pdn_local
(
  ds_epc_cb_type                    * instance_ptr,
  ps_iface_name_enum_type             trat_name
);

#endif /*FEATURE_EPC_HANDOFF*/
#endif /*DS_EPC_PDN_HO_H*/
