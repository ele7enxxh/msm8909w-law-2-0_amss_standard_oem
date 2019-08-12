#ifndef DS_DSSNET4_H
#define DS_DSSNET4_H
/*===========================================================================

                  D S 7 0 7 _ I F A C E 4 _ H  D L R . H
                   
                          Formerly:  dssnet4.h
                   
DESCRIPTION
  The Data Services Sockets Network state machine header file for IPv4.
  (This is an INTERNAL sockets file)

EXTERNALIZED FUNCTIONS
  dssnet4_sm_create()
    Creates an instance of the DSSNET4 State machine
  dssnet4_sm_post_event()
    Posts events to the DSSNET4 state machine
  dssnet4_sm_process_event()
    Processes events posted to the dssnet4 State machine. 
  is707_get_ppp_auth_info_from_nv()
    Retrieves the 1x PPP user ID and password from NV and stores them in 
    the provided PPP configuration structure.    
  dssnet4_set_in_qnc_call()
    This function notifies dssnet if the mobile is in a QNC call. Called by
    ds707_pkt_mgr.
  dssnet4_override_qcmip()
    Override dsat707_qcmip value with 2.

 Copyright (c)2004-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_iface4_hdlr.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/05/14    ms     DSDA Phase 2 changes.
06/30/14    niv    Added wrapper functions for NV_REFRESH 
11/02/12    ssb    Moving utility functions of MIP to SIP Fallback 
                   Stats to ds707_pkt_mgr
05/04/11    ttv    Made changes to compute mip statistics for diag.
03/15/11    mg     Global variable cleanup
01/14/11    ack    Added PHY_LINK_DOWN_EV reg/dereg to dssnet4/6 SM
10/12/10    mg     Changes for error code propogation to ps_iface
07/26/10    gc     Adding code to report call error due to PPP failure
07/29/09    spn    Remember if the call was made on eHRPD or not.
04/23/09    ls     Add the support to report MIP rrp code
03/09/09    sa     AU level CMI modifications.
12/05/08    sk     EAP support for EHRPD
11/15/07    sc     Added support to send PPP auth starting event to IFACE
12/07/06    as     Added new event and state for MIP deregistration
09/11/06    as     Added new function to set te2client and added a new flag
                   to the dssnet __dssnet4_sm_cb_s called is_te2_client
06/29/06    spn    Removed the MIP Featurization for unified Library.
10/12/05    mct    Added DSSNET4_FLOW_DELETED_EV.
09/10/05    gr     Merged changes to lock out GPS in the middle of a PPP 
                   resync during a handup from 1x to DO
07/22/05    kvd    Added mip_event handle to dssnet instance. added one more
                   arg to post_event.
04/20/05    mct    Remove reference to phys_link down.
06/12/04    ifk    Added ppp_event_handle to dssnet4 control block.
05/27/04    kvd    Externalized dssnet4_1x_mip_support().
04/27/04    kvd    Added new APIs set_ipcp_dns_opt & mip_perf_start
03/20/04    ak     Changed name at top of file, to match file name change.
03/18/04    aku    Register for Phys link down and gone indications when
                   common dssnet bringup function is call and deregister
                   when transitioning to CLOSED state.
03/12/04    aku    Register for phys link up indication only when common 
                   dssnet bringup cmd is called. Deregister this indication
                   when processing the PHYS_LINK_UP_EV.
01/13/04    aku     Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "ps_iface.h"
#include "ps_ppp.h"
#include "nv.h"
#include "dstask_v.h"
#include "ds3gflowmgr.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
struct __dssnet4_sm_cb_s; /* the state machine data structure              */

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
TYPEDEF DSSNET4_SM_EVENT_TYPE

DESCRIPTION
  Type and static variable defining events received by the dssnet4 sm.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSNET4_PHY_LINK_UP_EV       = 0,
  DSSNET4_PHY_LINK_GONE_EV     = 1,
  DSSNET4_PHY_LINK_DOWN_EV     = 2,
  DSSNET4_START_SIP_EV         = 3,  
  DSSNET4_LINK_UP_WITH_SIP_EV  = 4,
  DSSNET4_LINK_DOWN_EV         = 5,
  DSSNET4_LINK_RESYNC_EV       = 6,
#ifdef FEATURE_DS_MOBILE_IP
  DSSNET4_START_MIP_EV         = 7,  
  DSSNET4_LINK_UP_WITH_MIP_EV  = 8,
  DSSNET4_MIP_UP_SUCCESS_EV    = 9,
  DSSNET4_MIP_FAILURE_EV       = 10,
  DSSNET4_MIP_DEREGED_EV       = 11,
#endif /* FEATURE_DS_MOBILE_IP */
  DSSNET4_PPP_CLOSE_EV         = 12,
  DSSNET4_FLOW_DELETED_EV      = 13,
  DSSNET4_PPP_AUTH_STARTED_EV  = 14,
  DSSNET4_MAX_EV        
} dssnet4_sm_event_type;

/*---------------------------------------------------------------------------
TYPEDEF DSSNET4_SM_EVENT_TYPE

DESCRIPTION
  Type and static variable defining events received by the dssnet4 sm.
---------------------------------------------------------------------------*/
typedef struct __dssnet4_sm_cb_s dssnet4_sm_cb_type;

/*-------------------------------------------------------------------------
    Define a global structure to store mip information
  -------------------------------------------------------------------------*/
typedef struct
{
  byte     mip_rrp_code;
  ppp_fail_reason_e_type  reason;
} dssnet4_mip_ppp_info_type;

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSNET4_SM_CREATE()

DESCRIPTION
  This function is is used to create an instance of the dssnet4 state 
  machine.

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
int dssnet4_sm_create
(
  dssnet4_sm_cb_type *dssnet4_cb_ptr,     /* Ptr to DSSNET6 SM instance    */
  ps_iface_type      *ps_iface_ptr        /* Interface Ptr                 */
);

/*===========================================================================
FUNCTION DSSNET4_SM_POST_EVENT()

DESCRIPTION
  This function is used to post events to the dssnet4 state machine by other 
  modules

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
int dssnet4_sm_post_event
(
  dssnet4_sm_cb_type      *dssnet4_cb_ptr,   /* Ptr to DSSNET4 SM instance */
  dssnet4_sm_event_type   event,             /* dssnet4 event type         */
  void                    *event_data
);

/*===========================================================================
FUNCTION DSSNET4_SM_PROCESS_EVENT()

DESCRIPTION
  This function is used to process events posted to the dssnet4 state 
  machine. 

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_sm_process_event
(
  ds_cmd_type *cmd_ptr                          /* Pointer to ds_cmd_type  */
);

/*===========================================================================
FUNCTION IS707_GET_PPP_AUTH_INFO_FROM_NV

DESCRIPTION
  This function retrieves the 1x PPP user ID and password from NV
  and stores them in the provided PPP configuration structure.

DEPENDENCIES
  The NV task must be started and running.

RETURN VALUE
  None

SIDE EFFECTS
  Each call to 'nv_get' results in a Wait and watchdog kicking
===========================================================================*/
void is707_get_ppp_auth_info_from_nv
(
  ppp_auth_info_type *ppp_auth_info_ptr,
  nv_stat_enum_type (* nv_get)(nv_items_enum_type, nv_item_type *, sys_modem_as_id_e_type)
);

/*===========================================================================
FUNCTION DSSNET4_SET_IN_QNC_CALL()

DESCRIPTION
  This function notifies dssnet if the mobile is in a QNC call. Called by
  ds707_pkt_mgr.

DEPENDENCIES
  None.

PARAMETERS
  dssnet4 instance handle
  boolean     in_qnc_call

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_set_in_qnc_call
(
  dssnet4_sm_cb_type  *instance_ptr,
  boolean              in_qnc_call
);

/*===========================================================================
FUNCTION DSSNET4_OVERRIDE_QCMIP()

DESCRIPTION
  Override dsat707_qcmip value with 2.

  RMSM calls this to force dssnet to close in the qcmip=1 case, 
  so it can bring up netmodel SimpleIP TE2 calls on its own.

  RMSM calls this to force_sip_fallback_to_fail_so_rmsm_can_do_it()

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_override_qcmip
( 
  dssnet4_sm_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION DSSNET4_SET_TE_CLIENT()

DESCRIPTION
  This is a flag set by RMSM to allow dssnet to check if the 
  call is a laptop call or a sockets call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_set_te_client
( 
  dssnet4_sm_cb_type *instance_ptr            /* dssnet4 instance ptr      */
);


/*===========================================================================
FUNCTION DSSNET4_SET_IPCP_DNS_OPT()

DESCRIPTION
  This function enables/disables the dns negotiation option for ipcp.

DEPENDENCIES
  None.

PARAMETERS
  dssnet4 instance handle - dssnet instance corresponding to the ppp instance
                            for which this dns option holds good
  boolean     dns_option  - value of DNS option for IPCP

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet4_set_ipcp_dns_opt
(
  dssnet4_sm_cb_type *instance_ptr,
  boolean dns_option
);

/*===========================================================================
FUNCTION DSSNET4_START_MIP_PERF_STATS()

DESCRIPTION
  This function starts collecting call-setup stats for MIP calls

DEPENDENCIES
  None.

PARAMETERS
  dssnet4 instance handle - dssnet instance corresponding to the ppp instance
                            for which the stats are being collected

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void dssnet4_start_mip_perf_stats
(
  dssnet4_sm_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION DSSNET4_REG_PHYS_LINK_EVENTS()

DESCRIPTION
  The common dssnet_bring_up_iface_cmd() invokes this function to register
  for the phys link events.
  
DEPENDENCIES
  None.

RETURN VALUE
  0: on success
 -1: on failure

SIDE EFFECTS
  None.
===========================================================================*/
int dssnet4_reg_phys_link_events
( 
  dssnet4_sm_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION DSSNET4_1X_MIP_SUPPORT()

DESCRIPTION
  Check if mobile can establish mobile IP call.

  To do MIP in an IS-835 network,
    - p_rev must be greater than or equal to 6,
    - qcmdr must be equal to 3,
    - MIP meta state machine must be initialized

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - mobile IP is supported
  FALSE - mobile IP is not supported

SIDE EFFECTS
  None
===========================================================================*/
boolean dssnet4_1x_mip_support
(
  void
);

/*===========================================================================
FUNCTION DSSNET4_REG_PPP_RESYNC_DONE_CB()

DESCRIPTION
  This function Registers the call back to notify the completion of 
  PPP resync. It could be a successful or a failure resync

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  TRUE if registration succeeded else FALSE
  

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssnet4_reg_ppp_resync_done_cb
(
  dssnet4_sm_cb_type *instance_ptr,
  void (* ppp_resync_done_cb)(void *) 
);

/*===========================================================================
FUNCTION       DS707_IFACE4_HDLR_PROCESS_CONN_DELAY_TIMER_EV

DESCRIPTION    This function handles the connection release delay timer expiry
               event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_iface4_hdlr_process_conn_delay_timer_ev
(
  dssnet4_sm_cb_type  *instance_ptr
);

#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================
FUNCTION DSSNET4I_CONVERT_MIP_RRP_CODE

DESCRIPTION    This function converts MIP RRP failure code into 
               ps_iface_net_down_reason_type 

DEPENDENCIES   None.

RETURN VALUE   ps_iface_net_down_reason_type

SIDE EFFECTS   None

===========================================================================*/

ps_iface_net_down_reason_type dssnet4i_convert_mip_rrp_code
(
  byte mip_rrp_code
);
/*===========================================================================
FUNCTION DSSNET4I_SET_MIP_RRP_CODE

DESCRIPTION    This function sets the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_set_mip_rrp_code
(
  dssnet4_sm_cb_type *instance_ptr,
  byte mip_rrp_code
);
/*===========================================================================
FUNCTION DSSNET4I_GET_MIP_RRP_CODE

DESCRIPTION    This function gets the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
byte dssnet4i_get_mip_rrp_code
(
  dssnet4_sm_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION DSSNET4I_CLEAR_MIP_RRP_CODE

DESCRIPTION    This function clears the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_clear_mip_rrp_code
(
  dssnet4_sm_cb_type *instance_ptr
);

#endif
/*===========================================================================
FUNCTION DSSNET4I_GET_PS_CALL_END_REASON

DESCRIPTION    TThis function gets end_call_reason and writes to the dssnet4 
               instance_ptr. If mip_rrp_code is set, mip_rrp_code shall
               be converted to PS reason

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_get_ps_call_end_reason
(
  dssnet4_sm_cb_type *instance_ptr
);
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE DATA STRUCTURES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
TYPEDEF dssnet4i_state_type

DESCRIPTION
  Type and static variable defining the state of the network.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSNET4I_CLOSED_STATE                 = 0,
  DSSNET4I_LINK_OPENING_WITH_SIP_STATE  = 1,
#ifdef FEATURE_DS_MOBILE_IP
  DSSNET4I_LINK_OPENING_WITH_MIP_STATE  = 2,
  DSSNET4I_WAITING_FOR_MIP_REG_STATE    = 3,
  DSSNET4I_WAITING_FOR_MIP_DEREG_STATE  = 4,
#endif /* FEATURE_DS_MOBILE_IP */
  DSSNET4I_NET_UP_STATE                 = 5,
  DSSNET4I_LINK_RESYNC_STATE            = 6,
  DSSNET4I_LINK_CLOSING_STATE           = 7,
  DSSNET4I_MAX_STATE
} dssnet4i_state_type;

/*---------------------------------------------------------------------------
TYPEDEF DSSNET4_IFACE_DOWN_IND_CB_TYPE

DESCRIPTION
  This is the type for the callbacks which can be registered for dssnet4
  iface down event

PARAMETERS
  this_iface_ptr: IFACE pointer
  down_reason   : Network going down reason

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*dssnet4_iface_down_ind_cb_type)
(
  ps_iface_type                  *this_iface_ptr,
  ps_iface_net_down_reason_type   down_reason
);

/*---------------------------------------------------------------------------
STRUCT __DSSNET4_SM_CB_S

DESCRIPTION
  DSSNET4 SM control block.
---------------------------------------------------------------------------*/
struct __dssnet4_sm_cb_s
{
  dssnet4_sm_cb_type         *this_sm_ptr;     /* pointer to ourself       */
  dssnet4i_state_type        state;            /* Current state            */
  ps_iface_type              *ps_iface_ptr;         /* associated ps_iface */
  void                       *phys_link_up_cbuf;      /* phys up event buf */
  void                       *phys_link_gone_cbuf;    /* phys gome ev buf  */
  void                       *phys_link_down_cbuf;    /* phys down ev buf  */
  void                       *iface_flow_deleted_cbuf;/* flow deleted      */
  boolean                    qnc_call;                /* QNC call?         */
  boolean                    disable_ipcp_dns_opt; /* IPCP DNS opt needed? */
  boolean                    te2_call_override_qcmip; /* no sip fallback   */
  boolean                    is_te2_client;       /* set for laptop client */
  boolean                    is_ehrpd_call;
  /*-------------------------------------------------------------------------
    To complete the PPP resync without interruption from GPS, 
    a lock is maintained by the 707 pkt manager to lock out GPS during the
    process of PPP resync and the following MIP registration. This callback
    is registered by 707 pkt manager when the resync is initiated during 
    handup from 1x to 1x-EVDO. IFACE handlers invokes the call back when 
    the dssnet goes into NET UP state or NULL state
  -------------------------------------------------------------------------*/
  void                       (* ppp_resync_done_cb)(void *);  
  /*-------------------------------------------------------------------------
    Removing the MIP featurization for unified library.
  -------------------------------------------------------------------------*/
  boolean                    want_mip;        /* try mobile ip first?      */
  boolean                    will_sip;        /* can fallback to simple ip */
  boolean                    work_mip;        /* is mobile ip established? */
  boolean                    dssnet_started_mip_call;/* dssnet started MIP */
  boolean                    mip_call_started;   /* is MIP in progress */ 
  boolean                    local_abort;       /* local abort flag        */     

  void                       *ppp_event_handle;/* PPP event handle         */
  void                       *mip_event_handle;/* MIP event handle         */
  /*--------------------------------------------------------------------------
    variable to store mip and ppp information
  --------------------------------------------------------------------------*/
  dssnet4_mip_ppp_info_type dssnet4_mip_ppp_info;
  ps_iface_net_down_reason_type call_end_reason;

  dssnet4_iface_down_ind_cb_type  iface_down_ind_cb; /*iface down ind callback*/
  boolean  is_bringup_phys_link;                    /* Bringup physlink later */
};

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION       DS707_IFACE4_HDLR_GET_EAP_PARAMETER

DESCRIPTION    Function gets eap parameter
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_iface4_hdlr_get_eap_parameter
(
  ppp_auth_info_type *ppp_auth_info_ptr
);
#endif /* FEATURE_EHRPD */
/*===========================================================================
FUNCTION DSSNET4I_CONVERT_PPP_FAILURE_CODE

DESCRIPTION    This function converts PPP failure code into 
               ps_iface_net_down_reason_type 

DEPENDENCIES   None.

RETURN VALUE   ps_iface_net_down_reason_type

SIDE EFFECTS   None

===========================================================================*/

ps_iface_net_down_reason_type dssnet4i_convert_ppp_failure_code
(
  ppp_fail_reason_e_type reason
);

/*===========================================================================
FUNCTION DSSNET4I_SET_PPP_FAILURE_CODE

DESCRIPTION    This function sets the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_set_ppp_failure_code
(
  dssnet4_sm_cb_type *instance_ptr,
   ppp_fail_reason_e_type reason
);

/*===========================================================================
FUNCTION DSSNET4I_GET_PPP_FAILURE_CODE

DESCRIPTION    This function gets the PPP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
ppp_fail_reason_e_type dssnet4i_get_ppp_failure_code
(
  dssnet4_sm_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION DSSNET4I_CLEAR_PPP_FAILURE_CODE

DESCRIPTION    This function clears the MIP RRP failure code

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4i_clear_ppp_failure_code
(
  dssnet4_sm_cb_type *instance_ptr
);

/*=========================================================================
FUNCTION       dssnet4_iface_down_ind_callback_reg

DESCRIPTION    This function registers for v4 iface down ind event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
int dssnet4_iface_down_ind_callback_reg
(
  dssnet4_sm_cb_type             *instance_ptr,
  dssnet4_iface_down_ind_cb_type  cb_fn
);

/*=========================================================================
FUNCTION       DSSNET4_GET_INSTANCE_STATE

DESCRIPTION    Returns dssnet4 instance state
               
DEPENDENCIES   NONE

RETURN VALUE   Failure - DSSNET4I_MAX_STATE
               Success - dssnet4 instance state

SIDE EFFECTS   NONE
=========================================================================*/
dssnet4i_state_type dssnet4_get_instance_state
(
  dssnet4_sm_cb_type *dssnet4_sm_cb
);

/*=========================================================================
FUNCTION       DSSNET4_GET_INSTANCE_CALL_END_REASON

DESCRIPTION    Returns dssnet4 instance call end reason
               
DEPENDENCIES   NONE

RETURN VALUE   Failure - PS_NET_DOWN_REASON_MAX
               Success - dssnet4 instance call end reason

SIDE EFFECTS   NONE
=========================================================================*/
ps_iface_net_down_reason_type dssnet4_get_instance_call_end_reason
(
  dssnet4_sm_cb_type *dssnet4_sm_cb
);

/*===========================================================================
FUNCTION DSSNET4_SET_PS_CALL_END_REASON

DESCRIPTION    This function sets end_call_reason to ps. 

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4_set_ps_call_end_reason
(
  dssnet4_sm_cb_type *instance_ptr,           /* dssnet4 instance ptr      */
  ps_iface_net_down_reason_type call_end_reason
);

/*===========================================================================
FUNCTION DSSNET4_SET_PS_LOCAL_ABORT

DESCRIPTION    This function sets local_abort to ps. 

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4_set_ps_local_abort
(
  dssnet4_sm_cb_type      *instance_ptr,           /* dssnet4 instance ptr */
  boolean                 local_abort              /* local abort          */    
);
/*===========================================================================
FUNCTION DSSNET4_RESET_CALL_END_REASON

DESCRIPTION    This function resets end_call_reason to 
               PS_NET_DOWN_REASON_NOT_SPECIFIED 

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4_reset_call_end_reason
(
  dssnet4_sm_cb_type *dssnet4_sm_cb           /* dssnet4 instance ptr      */
);

/*===========================================================================
FUNCTION DSSNET4_RESET_LOCAL_ABORT

DESCRIPTION    This function resets local_abort to 
               FALSE 

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void dssnet4_reset_local_abort
(
  dssnet4_sm_cb_type *dssnet4_sm_cb           /* dssnet4 instance ptr      */
);

/*=========================================================================
FUNCTION       DSSNET4_SET_IS_BRINGUP_PHYS_LINK

DESCRIPTION    Set is_bringup_phys_link flag which will be used to remember 
               that phys link needs to be brought up again after it is
               cleaned up. This is set to TRUE when phys link bringup fails
               due to phys link being in going_null state
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet4_set_is_bringup_phys_link
(
  dssnet4_sm_cb_type            *instance_ptr,
  boolean                        is_bringup_phys_link
);

/*=========================================================================
FUNCTION       DSSNET4_GET_IS_BRINGUP_PHYS_LINK

DESCRIPTION    Get is_bringup_phys_link flag, which will be used to remember 
               that phys link needs to be brought up again after it is
               cleaned up.
               
DEPENDENCIES   NONE

RETURN VALUE   is_bringup_phys_link value

SIDE EFFECTS   NONE
=========================================================================*/
boolean dssnet4_get_is_bringup_phys_link
(
  dssnet4_sm_cb_type            *instance_ptr
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS_DSSNET4_H */
