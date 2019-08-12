#ifndef DS_DSSNET6_H
#define DS_DSSNET6_H
/*===========================================================================

                      D S G E N _ I F A C E 6 _ H D L R . H
                   
DESCRIPTION
  The Data Services Sockets Network state machine header file for IPv6.
  (This is an INTERNAL sockets file)

EXTERNALIZED FUNCTIONS
  dssnet6_sm_create()
    Creates an instance of the DSSNET6 State machine
  dssnet6_sm_post_event()
    Post events to the DSSNET6 state machine
  dssnet6_sm_process_event()
    Processes events posted to the dssnet6 State machine. 
    

 Copyright (c)2003-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/dsgen_iface6_hdlr.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/23/11    sn     Globals cleanup for free floating of tasks.
01/14/11    ack    Added PHY_LINK_DOWN_EV reg/dereg to dssnet4/6 SM
10/12/10    mg     Changes for error code propogation to ps_iface
07/29/09    spn    Remember if the call was made on eHRPD or not.
03/09/09   sa      AU level CMI modifications.
09/10/05    gr     Merged changes to lock out GPS in the middle of a PPP 
                   resync during a handup from 1x to DO
04/20/05    mct    Removed references to phys_link down as part of flow chgs.
08/03/04    kvd    Added pdsn_as_proxy_ipv6_dns_server flag to v6_sm_cb.
06/12/04    ifk    Added ppp_event_handle to dssnet6 control block.
04/21/04    mct	   Renamed ds_dssnet6.h dsgen_iface6_hdlr.h.
03/18/04    aku    Register for Phys link down and gone indications when
                   common dssnet bringup function is call and deregister
                   when transitioning to CLOSED state.
03/12/04    aku    Register for phys link up indication only when common 
                   dssnet bringup cmd is called. Deregister this indication
                   when processing the PHYS_LINK_UP_EV.
01/08/04    mvl    Restructured file layout, renamed various functions, and
                   changed initialization check
12/22/03    rc     Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#include "dstask_v.h"
#include "ps_ppp_defs.h"
#include "ps_ppp.h"
#include "ps_ip6_sm.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
struct __dssnet6_sm_cb_s; /* the state machine data structure              */

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF DSSNET6_SM_EVENT_TYPE

DESCRIPTION
  Type and static variable defining events received by the dssnet6 sm.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSNET6_PHY_LINK_UP_EV   = 0,
  DSSNET6_PHY_LINK_GONE_EV = 1,
  DSSNET6_LINK_UP_EV       = 2,
  DSSNET6_LINK_DOWN_EV     = 3,
  DSSNET6_LINK_RESYNC_EV   = 4,
  DSSNET6_NET_UP_EV        = 5,
  DSSNET6_NET_DOWN_EV      = 6,
  DSSNET6_PPP_CLOSE_CMD_EV = 7,
  DSSNET6_FLOW_DELETED_EV  = 8,
  DSSNET6_PHY_LINK_DOWN_EV = 9,
  DSSNET6_NET_ABORTED_EV   = 10, 
  DSSNET6_MAX_EV        
} dssnet6_sm_event_type;



/*---------------------------------------------------------------------------
TYPEDEF DSSNET6_SM_EVENT_TYPE

DESCRIPTION
  Type and static variable defining events received by the dssnet6 sm.
---------------------------------------------------------------------------*/
typedef struct __dssnet6_sm_cb_s dssnet6_sm_cb_type;



/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DSSNET6_INIT

DESCRIPTION   Called once at mobile power-up. Initializes critical section.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssnet6_init(void);

/*===========================================================================
FUNCTION DSSNET6_SM_CREATE()

DESCRIPTION
  This function is is used to initialize the dssnet6 state machine.

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
int dssnet6_sm_create
(
  dssnet6_sm_cb_type *dssnet6_cb_ptr,     /* Ptr to DSSNET6 SM instance    */
  ps_iface_type      *ps_iface_ptr,       /* Interface Ptr                 */
  ip6_sm_type        *ip6_cb_ptr,         /* Ptr to IP6 SM instance        */
  boolean            use_ppp,             /* Interface uses PPP or not     */
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DSSNET6_SM_UPDATE()

DESCRIPTION
  This function is is used to update the v6 sm

DEPENDENCIES
  None

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None
===========================================================================*/
int dssnet6_sm_update
(
  dssnet6_sm_cb_type    *instance_ptr,       /* Ptr to DSSNET6 SM instance */
  ps_iface_type         *ps_iface_ptr,       /* Interface Ptr              */
  ip6_sm_type           *ip6_inst_ptr,       /* Ptr to IP6 SM instance     */
  boolean                use_ppp,              /* Interface uses PPP or not  */
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================
FUNCTION DSSNET6_SM_POST_EVENT()

DESCRIPTION
  This function is used to post events to the dssnet6 State Machine by other 
  modules

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
int dssnet6_sm_post_event
(
  dssnet6_sm_cb_type     *dssnet6_cb_ptr,    /* Ptr to DSSNET6 SM instance */
  dssnet6_sm_event_type  event,               /* dssnet6 event type         */
  void                  *event_data
);

/*===========================================================================
FUNCTION DSSNET6_SM_PROCESS_EVENT()

DESCRIPTION
  This function is used to process events posted to the dssnet6 state 
  machine. 

DEPENDENCIES
  None.

RETURN VALUE
 -1 on failure
  0 on success

SIDE EFFECTS
  None.
===========================================================================*/
void dssnet6_sm_process_event
(
  ds_cmd_type *cmd_ptr                          /* Pointer to ds_cmd_type  */
);


/*===========================================================================
FUNCTION DSSNET6_REG_PHYS_LINK_EVENTS()

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
int dssnet6_reg_phys_link_events
( 
  dssnet6_sm_cb_type *instance_ptr
);
/*===========================================================================
FUNCTION DSSNET6_REG_PPP_RESYNC_DONE_CB()

DESCRIPTION
  This function Registers the call back to notify the completion of 
  PPP resync. It could be a successful or a failure resync

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  TRUE if registration succeeds and FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssnet6_reg_ppp_resync_done_cb
(
  dssnet6_sm_cb_type *instance_ptr,
  void (* ppp_resync_done_cb)(void *) 
);


/*===========================================================================
FUNCTION       DSGEN_IFACE6_HDLR_PROCESS_CONN_DELAY_TIMER_EV

DESCRIPTION    This function handles the connection release delay timer expiry
               event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsgen_iface6_hdlr_process_conn_delay_timer_ev
(
  dssnet6_sm_cb_type  *instance_ptr
);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE DATA STRUCTURES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
TYPEDEF dssnet6i_state_type

DESCRIPTION
  Type and static variable defining the state of the network.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSNET6I_CLOSED_STATE                = 0,
  DSSNET6I_LINK_OPENING_STATE          = 1,
  DSSNET6I_WAITING_FOR_NET_UP_STATE    = 2,
  DSSNET6I_NET_UP_STATE                = 3,
  DSSNET6I_LINK_RESYNC_STATE           = 4,
  DSSNET6I_LINK_CLOSING_STATE          = 5,
  DSSNET6I_WAITING_FOR_NET_DOWN_STATE  = 6,
  DSSNET6I_MAX_STATE
} dssnet6i_state_type;

/*---------------------------------------------------------------------------
TYPEDEF DSSNET6_EVENT_CBACK_TYPE

DESCRIPTION
  This is the type for the callbacks which can be registered for dssnet6 
  events

PARAMETERS
  instance: the instance that caused the indication.
  ind: the indication that occurred (will only be posted on an edge - e.g. UP
       will only happen when not already up).
  user_data: data that was given at registration time.

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*dssnet6_event_cback_type)
(
  struct __dssnet6_sm_cb_s *instance,
  dssnet6_sm_event_type  cur_event,
  dssnet6i_state_type cur_state,
  void *user_data
);

typedef void (*dssnet6_iface_down_ind_cb_type)
(
  ps_iface_type                  *this_iface_ptr,
  ps_iface_net_down_reason_type  down_reason
);

/*---------------------------------------------------------------------------
STRUCT __DSSNET6_SM_CB_S

DESCRIPTION
  DSSNET6 SM control block.
---------------------------------------------------------------------------*/
struct __dssnet6_sm_cb_s
{
  dssnet6_sm_cb_type  *this_sm_ptr;            /* pointer to ourself       */
  ip6_sm_type         *ip6_instance;           /* IPv6 SM instance         */
  dssnet6i_state_type state;                  /* Current state            */
  ps_iface_type       *ps_iface_ptr;           /* associated ps_iface      */
  void                *phys_link_up_cbuf;      /* Phy Link Up Ev buf       */
  void                *phys_link_down_cbuf;    /* Phy Link Down Ev buf       */
  void                *phys_link_gone_cbuf;    /* Phy Link Gone Ev buf     */
  void                *iface_flow_deleted_cbuf;/* flow deleted      */
  boolean             qnc_call;               /* PPP auth needed?         */
  /*-------------------------------------------------------------------------
    To complete the PPP resync without interruption from GPS, 
    a lock is maintained by the 707 pkt manager to lock out GPS during the
    process of PPP resync and the following MIP registration. This callback
    is registered by 707 pkt manager when the resync is initiated during 
    handup from 1x to 1x-EVDO. IFACE handlers invokes the call back when 
    the dssnet goes into NET UP state or NULL state
  -------------------------------------------------------------------------*/
  void                (* ppp_resync_done_cb)(void *);  
  boolean             use_ppp;                /* Use PPP or Not           */
  boolean             is_ehrpd_call;
  boolean             local_abort;            /* local abort              */
  void                *ppp_event_handle;       /* PPP event handle         */
  boolean pdsn_as_proxy_ipv6_dns_server;       /* TRUE if pdsn_as_proxy_ipv6
                                                  dns_server NV flag is ON */
  struct ps_in6_addr            nv_primary_dns;
  struct ps_in6_addr            nv_secondary_dns;

  dssnet6_event_cback_type event_cb;
  ps_iface_net_down_reason_type call_end_reason;

  dssnet6_iface_down_ind_cb_type  iface_down_ind_cb;
  void         *event_cb_user_data;
  boolean       is_bringup_phys_link;     /* Physlink needs to be brought up */
};

/*=========================================================================
FUNCTION       DSSNET6_EVENT_CALLBACK_REG

DESCRIPTION    This function handles the connection release delay timer expiry
               event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
int dssnet6_event_callback_reg
(
  dssnet6_sm_cb_type  *instance_ptr,
  dssnet6_event_cback_type  cback,
  void              *user_data
);
/*=========================================================================
FUNCTION       dssnet6_enter_crit_sect

DESCRIPTION    This function enters the dssnet6 crit sect
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_enter_crit_sect ( void );

/*=========================================================================
FUNCTION       dssnet6_leave_crit_sect

DESCRIPTION    This function leaves the dssnet6 crit sect
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_leave_crit_sect ( void );

/*=========================================================================
FUNCTION       dssnet6_iface_down_ind_callback_reg

DESCRIPTION    This function registers for v6 iface down ind event
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
int dssnet6_iface_down_ind_callback_reg
(
  dssnet6_sm_cb_type           *instance_ptr,
  dssnet6_iface_down_ind_cb_type  cb_fn
);

/*=========================================================================
FUNCTION       DSSNET6_GET_INSTANCE_STATE

DESCRIPTION    Returns dssnet6 instance state
               
DEPENDENCIES   NONE

RETURN VALUE   Failure - DSSNET6I_MAX_STATE
               Success - dssnet6 instance state

SIDE EFFECTS   NONE
=========================================================================*/
dssnet6i_state_type dssnet6_get_instance_state
(
  dssnet6_sm_cb_type *dssnet6_sm_cb
);

/*=========================================================================
FUNCTION       DSSNET6_GET_INSTANCE_CALL_END_REASON

DESCRIPTION    Returns dssnet6 instance call end reason
               
DEPENDENCIES   NONE

RETURN VALUE   Failure - PS_NET_DOWN_REASON_MAX
               Success - dssnet6 instance call end reason

SIDE EFFECTS   NONE
=========================================================================*/
ps_iface_net_down_reason_type dssnet6_get_instance_call_end_reason
(
  dssnet6_sm_cb_type *dssnet6_sm_cb
);

/*=========================================================================
FUNCTION       DSSNET6_RESET_CALL_END_REASON

DESCRIPTION    This function resets end_call_reason to 
               PS_NET_DOWN_REASON_NOT_SPECIFIED 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_reset_call_end_reason
(
  dssnet6_sm_cb_type            *dssnet6_sm_cb
);

/*=========================================================================
FUNCTION       DSSNET6_RESET_LOCAL_ABORT

DESCRIPTION    This function resets local_abort to 
               FALSE 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_reset_local_abort
(
  dssnet6_sm_cb_type            *dssnet6_sm_cb
);

/*=========================================================================
FUNCTION       DSSNET6_SET_PS_CALL_END_REASON

DESCRIPTION    This function sets the call end reason for the dssnet6 
               instance ptr 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_set_ps_call_end_reason
(
  dssnet6_sm_cb_type            *instance_ptr,
  ps_iface_net_down_reason_type  call_end_reason
);
/*=========================================================================
FUNCTION       DSSNET6_SET_PS_LOCAL_ABORT

DESCRIPTION    This function sets the local abort for the dssnet6 
               instance ptr 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_set_ps_local_abort
(
  dssnet6_sm_cb_type            *instance_ptr,
  boolean                       local_abort
);


/*=========================================================================
FUNCTION       DSSNET6_RESET_PS_CALL_END_REASON

DESCRIPTION    This function resets the call end reason for the dssnet6 
               instance ptr 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_reset_ps_call_end_reason
(
  dssnet6_sm_cb_type *instance_ptr
);

/*===========================================================================
FUNCTION DSSNET6I_GET_PS_CALL_END_REASON

DESCRIPTION    This function gets end_call_reason to ps. If CM has set call 
               end reason, propogate that. 

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/
void dssnet6_get_ps_call_end_reason
(
  dssnet6_sm_cb_type *instance_ptr
);

/*=========================================================================
FUNCTION       DSSNET6_SET_IS_BRINGUP_PHYS_LINK

DESCRIPTION    Set is_bringup_phys_link flag which will be used to remember 
               that phys link needs to be brought up again after it is
               cleaned up. This is set to TRUE when phys link bringup fails
               due to phys link being in going_null state
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
=========================================================================*/
void dssnet6_set_is_bringup_phys_link
(
  dssnet6_sm_cb_type            *instance_ptr,
  boolean                        is_bringup_phys_link
);

/*=========================================================================
FUNCTION       DSSNET6_GET_IS_BRINGUP_PHYS_LINK

DESCRIPTION    Get is_bringup_phys_link flag, which will be used to remember 
               that phys link needs to be brought up again after it is
               cleaned up.
               
DEPENDENCIES   NONE

RETURN VALUE   is_bringup_phys_link value

SIDE EFFECTS   NONE
=========================================================================*/
boolean dssnet6_get_is_bringup_phys_link
(
  dssnet6_sm_cb_type            *instance_ptr
);

#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
#endif /* DS_DSSNET6_H */
