#ifndef DS707_NW_INIT_QOS_MGR_H
#define DS707_NW_INIT_QOS_MGR_H

/*===========================================================================

                        DS707 NW INIT QOS MGR
                        
GENERAL DESCRIPTION
      
EXTERNALIZED FUNCTIONS

  
INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_nw_init_qos_mgr.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/08/13    sk     Created module

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_NIQ_EHRPD
#include "list.h"
#include "ps_iface_defs.h"
#include "ps_qos_defs.h"
#include "ds707_qosdefs.h"
#include "ds_tft_bldr.h"

/*===========================================================================

                           EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Definition: Maximum number of PS flows in network initiated QOS module
---------------------------------------------------------------------------*/
#define DS707_NIQ_MAX_FLOW_NUM  DS707_NUM_FWD_PS_FLOWS + DS707_NUM_REV_PS_FLOWS

/*---------------------------------------------------------------------------
  Struct definition: DS707_NW_INIT_QOS_FLOW_META_INFO_TYPE
  ts_id:                Transation ID of this flow.
  qos_spec:             QOS spec of this flow.
  op_opcode:            Opcode used for this flow.
  Current supporting opcode: 
  DS_TFT_BLDR_OPCODE_INITIATE_FLOW_REQUEST 
  DS_TFT_BLDR_OPCODE_INITIATE_DELETE_FLTRS 
  DS_TFT_BLDR_OPCODE_INITIATE_REPLACE_FLTRS 
  Note: ts_id 0 is invalid since the first bit should be 1 if RESV sending
  by the network.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                          ts_id;
  qos_spec_type                   qos_spec;
  uint8                           op_code;
}ds707_nw_init_qos_flow_info_type;

/*---------------------------------------------------------------------------
  Struct definition: DS707_NW_INIT_QOS_FLOW_LIST
---------------------------------------------------------------------------*/
typedef struct ds707_nw_init_qos_flow_s
{
  list_link_type                             link;
  /*-------------------------------------------------------------------------
  NW Initiated QOS flow pointer
  -------------------------------------------------------------------------*/
  ps_flow_type                              *flow_ptr;

  /*-------------------------------------------------------------------------
  Flow meta info.
  -------------------------------------------------------------------------*/
  ds707_nw_init_qos_flow_info_type          flow_meta_info;

 /*-------------------------------------------------------------------------
  QOS modify struct.
  When RESV modify msg comes, temporarily save the modify_qos_spec and ts_id.
  After modify flow succeed, merge this structure to qos_spec
  -------------------------------------------------------------------------*/
  ds707_nw_init_qos_flow_info_type         *flow_modify_info;

  /*-------------------------------------------------------------------------
  Data inactivity timer for this flow.  
  -------------------------------------------------------------------------*/
  rex_timer_type                            niq_inactivity_timer;
}ds707_nw_init_qos_flow_type;

/*---------------------------------------------------------------------------
  Struct definition: DS707_NW_INIT_QOS_IFACE_CB
---------------------------------------------------------------------------*/
typedef struct ds707_nw_init_qos_iface_cb_s
{
  list_link_type                 link;
  ps_iface_type                 *ps_iface_ptr;
  /*-------------------------------------------------------------------------
  Bearer control mode value for this iface.
  -------------------------------------------------------------------------*/
  //uint8                          BCM_val;
  /*-------------------------------------------------------------------------
  The network initiated QOS flow list.
  -------------------------------------------------------------------------*/
  list_type                     *nw_init_qos_flow_list;
  /*---------------------------------------------------------------------------
  Define the PPP resync waiting timer.
  ---------------------------------------------------------------------------*/
  rex_timer_type                niq_ppp_resync_timer;
  //define this control block ptr
}ds707_nw_init_qos_iface_cb_type;

/*---------------------------------------------------------------------------
  Enum Definition: DS707_NW_INIT_QOS_BCM_VAL
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_NW_INIT_QOS_BCM_CANNOT_REGONIZE = 0x00,
  DS707_NW_INIT_QOS_BCM_MS_ONLY         = 0x01,
  DS707_NW_INIT_QOS_BCM_MS_AND_NW       = 0x02,
  DS707_NW_INIT_QOS_BCM_NOT_VALID       = 0xFF
}ds707_nw_init_qos_bcm_val;

/*---------------------------------------------------------------------------
  Enum Definition: DS707_NW_INIT_QOS_ERROR_TYPE,
  It classified all the errors in the RESV msg handling.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_NW_INIT_QOS_ERROR_NONE = 0,
  DS707_NW_INIT_QOS_ERROR_PS_FLOW_CREATING,
  DS707_NW_INIT_QOS_ERROR_PS_FLOW_ACTIVATING,
  DS707_NW_INIT_QOS_ERROR_FLOW_INVALID_STATE,
  DS707_NW_INIT_QOS_ERROR_FLOW_MODIFY,
  DS707_NW_INIT_QOS_ERROR_UNKNOWN_IFACE,
  DS707_NW_INIT_QOS_ERROR_UNKNOWN_FLOW_ID,
  DS707_NW_INIT_QOS_ERROR_UNKNOW_OPCODE,
  DS707_NW_INIT_QOS_ERROR_INVALID_FLOW_SPEC,
  DS707_NW_INIT_QOS_ERROR_ACTIVATE_CMD,
  DS707_NW_INIT_QOS_ERROR_MODIFY_CMD,
  DS707_NW_INIT_QOS_ERROR_DEL_CMD,
  DS707_NW_INIT_QOS_ERROR_INTERNAL,
  DS707_NW_INIT_QOS_ERROR_RESV_PARSING,
  DS707_NW_INIT_QOS_ERROR_NO_NW_INIT_QOS,
  DS707_NW_INIT_QOS_DUP_RESV_MSG,
  DS707_NW_INIT_QOS_DUP_FLOW_DETECTED,
  DS707_NW_INIT_QOS_ERROR_INVALID_RESV,
  DS707_NW_INIT_QOS_NONE_AN_PROF_SUPPORT,
  DS707_NW_INIT_QOS_RESOURCE_NOT_SUPPORT
}ds707_nw_init_qos_error_type;

/*---------------------------------------------------------------------------
  Enum Definition: DS707_NW_INIT_QOS_IND_TYPE.
  These indication sent by the secondary package manager module.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_NW_INIT_QOS_IND_NONE = 0,
  DS707_NW_INIT_QOS_IND_FLOW_ACTIVATE_SUCCESS,
  DS707_NW_INIT_QOS_IND_FLOW_ACTIVATE_FAILURE,
  DS707_NW_INIT_QOS_IND_FLOW_MODIFY_SUCCESS,
  DS707_NW_INIT_QOS_IND_FLOW_MODIFY_FAILURE,
  DS707_NW_INIT_QOS_IND_FLOW_DEL_SUCCESS,
  DS707_NW_INIT_QOS_IND_FLOW_DEL_FAILURE,
  DS707_NW_INIT_QOS_IND_FLOW_SUSPEND,
  DS707_NW_INIT_QOS_IND_FLOW_GO_NULL
}ds707_nw_init_qos_ind_type;

/*---------------------------------------------------------------------------
  Struct definition: ds707_nw_init_qos_resv_flow_type
  Flow info parsed from RESV msg.

  Fields are:
    flow_id                    : flow_id get from RESV msg.
    dirn                       : direction of the flow.
    qos_spec                   : request/modify qos_spec for this flow.
    op_code                    : operation code for this flow.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                               flow_id;
  ds707_direction_enum_type           dirn;
  qos_spec_type                       qos_spec;
  uint8                               op_code;
}ds707_nw_init_qos_resv_flow_type;

/*---------------------------------------------------------------------------
  Struct definition: ds707_nw_init_qos_rcvd_resv_msg_type
  RESV msg received from the network

  Fields are:
    ts_id               : transaction ID used in this msg
    dirn                : The direction of this flow.
    num_flows           : The number of flows in the passed flow_list
    flow_list           : The list of flows requested by the network
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                              ts_id;
  ds_tft_bldr_tft_error_code          error_code;
  uint8                               num_flows;
  ds707_nw_init_qos_resv_flow_type  * flow_list;
}ds707_nw_init_qos_rcvd_resv_msg_type;

/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_IS_BCM_SUPPORTED

DESCRIPTION   Query if the Bearer Control Indicator(BCM) parameter should be 
              included in PCO option of VSNCP configure-request.
              It is called by the 3GPP2 MH when triggering VSNCP
              configure-request.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     Include BCM option.
              FALSE:    Does not include BCM option.

===========================================================================*/
boolean ds707_nw_init_qos_mgr_is_bcm_supported(void);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_IS_FWD_RESV_ON_FROM_UE_SUPPORTED

DESCRIPTION   Query if the NW Init QOS Fwd reservation on from UE is 
              supported or not.

DEPENDENCIES  None.

RETURN VALUE  TRUE:     Includse BCM option.
              FALSE:    Does not include BCM option.

===========================================================================*/
boolean ds707_nw_init_qos_mgr_is_fwd_resv_on_from_ue_supported(void);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_VSNCP_SUCCESS_IND

DESCRIPTION   The VSNCP success indication received from 3GPP2 MH.

DEPENDENCIES  None.

RETURN VALUE  None
===========================================================================*/
void ds707_nw_init_qos_mgr_vsncp_success_ind
(
  ps_iface_type   *iface_ptr,
  uint8            bcm_val
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_INIT

DESCRIPTION   Initiate the QOS manager module. It is called during the power 
              up. 

DEPENDENCIES  None.

RETURN VALUE  None.
===========================================================================*/
void ds707_nw_init_qos_mgr_init(void);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_PROC_RESV

DESCRIPTION   Called by DQOS module to notify a RESV msg received from 
              the network. 

DEPENDENCIES  Should be called only from the DS task

RETURN VALUE  True: processing success. 
              False: processing failure. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_nw_init_qos_mgr_proc_resv
(
  ds707_nw_init_qos_rcvd_resv_msg_type                *resv_msg,
  ps_iface_type                                       *ps_iface_ptr
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_IFACE_EVENTS_HANDLER

DESCRIPTION   Process the iface related events.It is called from 
              ds707_sec_pkt_mgr. 

DEPENDENCIES  Should be called only from the DS task.

RETURN VALUE  None.

===========================================================================*/
void ds707_nw_init_qos_mgr_iface_events_handler
(
  ps_iface_type                    * this_iface_ptr,
  ps_iface_event_enum_type           event,
  ps_iface_event_info_u_type       * event_info_ptr
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_PROC_IND

DESCRIPTION   Called by the secondary package manager to send the indication.

DEPENDENCIES  Should be called only from the DS task

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_nw_init_qos_mgr_proc_ind
(
  ds707_nw_init_qos_ind_type       ind,
  ps_flow_type                  ** ps_flow_ptrs,
  uint8                            num_of_flows,
  ps_iface_type                  * iface_ptr
);

/*===========================================================================
FUNCTION       DS707_NW_INIT_QOS_MGR_START_TIMER

DESCRIPTION    Starts the timers in network initiated QOS module. 
               If the timer already started, no need to start again.
 
DEPENDENCIES  None

RETURN VALUE  TRUE: Start the timer successfully.

              FALSE: Fail to start the timer.

SIDE EFFECTS  Starts Timer
===========================================================================*/
boolean ds707_nw_init_qos_mgr_start_timer
(
  rex_timer_type                  *timer_ptr,
  rex_timer_cnt_type               duration
);

/*===========================================================================
FUNCTION       DS707_NW_INIT_QOS_MGR_START_INACTIVITY_TIMER

DESCRIPTION    Starts the data inactivity timer in the network initiated QOS 
               module. Whenever it called, always reset the timer no matter
               the timer is already running or not.
 
DEPENDENCIES  None

RETURN VALUE  TRUE:     Timer started
                        Timer already running

              FALSE:    Timer id not valid

SIDE EFFECTS  Starts Timer
===========================================================================*/
boolean ds707_nw_init_qos_mgr_start_inactivity_timer
(
  rex_timer_type            *timer_ptr
);

/*===========================================================================
FUNCTION DS707_NW_INIT_QOS_MGR_INACTIVITY_TIMER_CB

DESCRIPTION
    This callback function called whenever the data inactivity timer expires.
 
PARAMETERS 
    param: flow_ptr is passed. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_nw_init_qos_mgr_inactivity_timer_cb
(
  unsigned long      param
);

/*===========================================================================
FUNCTION       DS707_NW_INIT_QOS_MGR_STOP_TIMER

DESCRIPTION    Stop the Timer if it is running.

DEPENDENCIES  None

RETURN VALUE  TRUE: Timer stopped
                    Timer already stopped

              FALSE: Timer ID invalid

SIDE EFFECTS  Stops timer
===========================================================================*/
boolean  ds707_nw_init_qos_mgr_stop_timer
(
  rex_timer_type             *Timer_ptr
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_PPP_RESYNC_WAIT_TIMER_NV_READ

DESCRIPTION   This function get nw_init_ppp_resync_wait_timer NV value.

DEPENDENCIES  None.

RETURN VALUE  NV reading value for this timer.

===========================================================================*/
uint8 ds707_nw_init_qos_mgr_ppp_resync_wait_timer_nv_read
(  
  void
);

/*===========================================================================

FUNCTION DS707_NW_INIT_QOS_MGR_PPP_RESYNC_TIMER_CB

DESCRIPTION
    This callback function called whenever the PPP resync timer is expired. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_nw_init_qos_mgr_ppp_resync_timer_cb
(
  unsigned long      param
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_PROC_SESSION_CHG

DESCRIPTION   Called by the secondary package manager module to notify 
              the session change indication.

DEPENDENCIES  Should be called only from the DS task

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_nw_init_qos_mgr_proc_session_chg
(
  ds707_sys_qos_state_enum_type         qos_session_state
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_UTIL_GET_FLOW_NUM_OF_IFACE

DESCRIPTION   Utility function to get the flow number of an iface.

DEPENDENCIES  Should be called only from the DS task

RETURN VALUE  The num of the flows for this iface.

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds707_nw_init_qos_mgr_util_get_flow_num_of_iface
(
  ps_iface_type                     *iface_ptr
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_UTIL_IF_NIQ_FLOW_EXIST

DESCRIPTION   Utility function to check if the network initiated QOS flow 
              existed. 

DEPENDENCIES  None

RETURN VALUE  True: the network initiated QOS flow exists. 
              False: the newtork initiated QOS flow does not exist 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_nw_init_qos_mgr_util_if_niq_flow_exist(void);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_DUP_FLOW_DETECTED

DESCRIPTION   Check if RESV msg tries to request/modify the same filter as 
              the flows already exisit for an iface. 

DEPENDENCIES  None.

RETURN VALUE  True: Same filter detected. 
              False: No matching filter found.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_nw_init_qos_mgr_dup_flow_detected
(
  ps_iface_type                            *iface_ptr,
  ds707_nw_init_qos_rcvd_resv_msg_type     *resv_msg,
  ps_flow_type                            **dup_flow_ptr
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_DUP_RESV_DETECTED

DESCRIPTION   Check if the duplicate RESV msg received by the UE.

DEPENDENCIES  None.

RETURN VALUE  True: duplicate RESV msg received. 
              False: Not duplicate RESV msg.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_nw_init_qos_mgr_dup_resv_detected
(
  ps_iface_type                            *iface_ptr,
  ds707_nw_init_qos_rcvd_resv_msg_type     *resv_msg,
  ps_flow_type                            **dup_flow_ptr
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_RL_DATA_ACTIVITY_START

DESCRIPTION   This function is called from C2K MH up when tx data arrival on 
              a QoS flow 
              If the flow is suspended, generates FLOW RESUME CMD
              If the inactivity timer is running, re-starts the
              inactivity timer for the flow

DEPENDENCIES  None.

RETURN VALUE  True: processing succeed. 
              False: processing failed. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_nw_init_qos_rl_data_activity_start
(
  ps_flow_type  *flow_ptr
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_UTIL_GET_PS_FLOW

DESCRIPTION   Utility function to get all the network initiated QOS flows 
              of an ifaces.

DEPENDENCIES  None.

RETURN VALUE  Flow_ptrs[] - array of all the PS flow ptrs. 
              num_ds_flows - num of entries in the flow_ptrs array. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_nw_init_qos_util_get_ps_flows
(
  ps_iface_type          *iface_ptr,
  ps_flow_type           *flow_ptrs[],
  uint8                  *num_flows
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_IS_AN_SUP_PROF_CONTAINED

DESCRIPTION   Check if the profiles request in the RESV msg contains the 
              profile supported by AN. 

DEPENDENCIES  None.

RETURN VALUE  TRUE: the profile requested is AN supported profile. 
              FALSE: None of the profiles requested in resv_msg is AN
                     supported profile.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_nw_init_qos_mgr_is_an_sup_prof_contained
(
  ds707_nw_init_qos_rcvd_resv_msg_type     *resv_msg
);
/*===========================================================================

FUNCTION DS707_NW_INIT_QOS_MGR_SESSION_OPEN_CLEANUP

DESCRIPTION
    It called when the new session opened is not on EHRPD

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_nw_init_qos_mgr_session_open_cleanup(void);
/*===========================================================================

FUNCTION DS707_NW_INIT_QOS_MGR_PPP_RESYNC_TIMER_EXPIRE_PROC

DESCRIPTION
    It called for ppp resync timer expire cmd processing

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_nw_init_qos_mgr_ppp_resync_timer_expire_proc(void);

/*===========================================================================

FUNCTION DS707_NW_INIT_QOS_MGR_DS_CMD_PROCESSING

DESCRIPTION
    It is called from DS task for the ds_cmd processing in network initiated
    QOS manager.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_nw_init_qos_mgr_ds_cmd_processing
(
  ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_GET_BCM_OF_IFACE

DESCRIPTION   Query the BCM of that iface. 
 
PARAMETERS    iface_ptr: the iface ptr. 
              bmc_val: buff to hold the return BCM value. 

DEPENDENCIES  None.

RETURN VALUE  

===========================================================================*/
boolean ds707_nw_init_qos_mgr_get_bcm_of_iface
(
  ps_iface_type   *iface_ptr,
  uint8           *bcm_val
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_GET_PPP_RESYNC_RUNNING_FOR_IFACE

DESCRIPTION   Search the flow list by the iface in ds707_nw_init_qos_iface_list 
              and returns only if the PPP resync timer is running. 

DEPENDENCIES  None.

RETURN VALUE  The corresponding flow_list
===========================================================================*/
boolean ds707_nw_init_qos_mgr_is_ppp_resync_running_for_iface
(
  ps_iface_type       *iface_ptr
);

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_NOTIFY_CONN_OPEN

DESCRIPTION   Notify NW init QOS MGR if the traffic channel has come up. 
              This function turns on FWd reservation for NW init flows
              if KKidle state is 1. 

DEPENDENCIES  None.

RETURN VALUE  None.
===========================================================================*/
void ds707_nw_init_qos_mgr_notify_conn_open();

/*===========================================================================
FUNCTION      DS707_NW_INIT_QOS_MGR_NOTIFY_HANDOFF_TO_QOS_AWARE_SYS

DESCRIPTION   Notify NW init QOS MGR that UE has moved to QOS aware system. 
              This function turns on FWd reservation for NW init flows
              if KKidle state is 0. 

DEPENDENCIES  None.

RETURN VALUE  None.
===========================================================================*/
void ds707_nw_init_qos_mgr_notify_handoff_to_qos_aware_sys();

#endif /* FEATURE_NIQ_EHRPD */
#endif /* DS707_NW_INIT_QOS_MGR_H */

