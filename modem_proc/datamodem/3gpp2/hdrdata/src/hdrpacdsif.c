
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H I G H     D A T A     R A T E

          PACKET APPLICATION CONTROLLER DATA SERVICES EXTERNAL FILE

GENERAL DESCRIPTION
  This file contains functionality that needs interaction between HDR Call
  Processing and Data Services.
  

EXTERNALIZED FUNCTIONS
  hdrpacdsif_process_smp_session_closed
    Processes the HDR session closed indication.
      
  hdrpacdsif_process_cmd
    This function processes commands for the HDRPACDSIF
    
  hdrpacdsif_powerup_init
    Does the powerup initialization for the module  

  hdrpacdsif_clear_sn_ppp_status
    This function provides an interface to write the SN_PPP_STATUS to NV. It
    sends a command to ds to accomplish the task

  hdrpacdsif_is_sn_ppp_up
    This function provides an interface to query the state of the SN PPP.

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2003-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/hdrpacdsif.c#1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
06/30/14   niv    Added wrapper functions & API change for 
                  NV_REFRESH 
11/18/11   vpk    HDR SU API cleanup
09/08/11   ash    Added support for eHRPD to HRPD fallback.
05/12/11   op     Migrated to MSG 2.0 macros
04/01/10   ls     For KDDI, it only tears down iface while data call is 
                  connected on HDR and receiving HDR session close. If 
                  the data call is connected on 1x and receiving HDR 
                  session close, we should not tear down the call
07/24/09   ss     Fixed issue of AT not closing HDR session when call 
                  origination on 1x fails during HDR to 1x handdown.
06/30/09   sn     Handled IPV6 disabled (in NV) case in down ind cback handler.
04/29/09   ms     Fixed Lint High errors.
01/21/09   sn     Fixed issue with IFACE events not getting registered in a 
                  race condition.
10/06/08   sn     Fix to make sure HDR SMP deactivate is not sent when 1x data 
                  call ends.
07/01/08   psng   KlocWork Fixes
11/20/07   ms     Fixed compiler warnings
10/08/07   sc     Chaged implementation of is_sn_ppp_up(). Also changed
                  hdrpacdsif_process_smp_session_closed() so that it doesnt
                  tear down the coming up PPP
09/26/07   ms     Removed the check for nv item altogether in the functions 
                  hdrpacdsif_handdown_to_1x() and hdrpacdsif_clear_sn_ppp_status().                  
09/11/07   ms    Replaced ds_get_nv_item() with hdrcom_util_read_nv_item() in functions
                  hdrpacdsif_handdown_to_1x() and hdrpacdsif_clear_sn_ppp_status(). Included
                  hdrutil.h
08/01/07   sk     Fixed featurization error
07/22/07   sk     If NV item NV_HDR_EMPA_SUPPORTED_I is set, the HDR session
                  is not tied to PPP.
07/02/07   sc     Replaced hdrpacdsif_tear_down_iface() with 
                  ps_iface_go_null_cmd() for bothe V4 anc V6 ifaces.
04/17/07   ara    Fixed Lint errors
04/11/07   ara    Added fix for CR 107287. This fix will prevent the PPP tear down of the 
                  IPV6 iface if there is a PPP tear down of the IPV4 iface and the IPV6
                  iface is still up.
02/06/07   as     Move to HDRPACDSIF_REG_FOR_IFACE_UP when processing
                  smp_session_closed
07/25/06   ksu    EMPA: 1x RLP code review comments
05/30/06   ksu    EMPA: RLP init code review comments
05/04/06   ksu    EMPA: RSP part 2
11/07/05   vrk    Call ps_iface_tear_down_cmd multiple times to tear down 
                  ppp if multiple apps are running
10/27/05   ksu    EMPA: octet stream, octet sequencing
07/18/05   vrk    replace ps_iface_tear_down_cmd with ps_flow_go_null_ind 
                  and ps_phys_link_gone_ind when session ends
03/09/05   gr/vrk SN_PPP_STATUS clear - end key and session close message
09/09/04   vas    Preliminary QOS support.
06/23/04   mpa    Added hdrpacdsif_is_sn_ppp_up()
03/20/04    ak    Updates to work with new pkt mgr interfaces.
03/09/04    gr    Added code to provide an interface to write SN_PPP_STATUS
                  to NV. The NV Write is invoked from the ds context.
03/02/04    gr    Changes for silently closing HDR session when PPP is
                  closed after Hand Down to 1x
11/14/03    vr    Added posting of HDRPAC_SESSION_CLOSED_CMD to process
                  part of Session Closed IND in DS task context.
10/21/03   vas    Added handling of switch from HDR to 1X.
09/23/03   vas    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "data_msg.h"

#if defined(FEATURE_HDR)
  #include "hdrrlp.h"
  #include "dshdr_an_mgr.h"
  #include "hdrds.h"
  #include "hdrpac.h"
  #include "hdrpacdsif.h"
  #include "hdrpac_mdm.h"
  #include "dstaski.h"
  #include "ps_iface.h"
  #include "ds707_pkt_mgr.h"
  #include "ds707_nv_util.h"
  #include "ds3gcfgmgr.h"
  #include "ds707_extif.h"
  #include "ds707_pkt_mgri.h"
  #include "dssocket.h"
  #include "nv.h"
  #include "hdrcom_api.h"
  #include "ds707_pdn_context.h"
  #include "ds707_roaming.h"

  #ifdef FEATURE_HDR_QOS
    #include "hdrmrlpctl.h"
  #endif /* FEATURE_HDR_QOS */
  #include "data_err_suppr.h"

/*===========================================================================

                              TYPEDEFS            

===========================================================================*/

typedef void hdrpacdsif_reg_func_type( void ); /*lint !e10*/
LOCAL hdrpacdsif_reg_func_type *hdrpacdsif_sn_init_func_ptr = NULL;
LOCAL hdrpacdsif_reg_func_type *hdrpacdsif_sn_cleanup_func_ptr = NULL;

LOCAL boolean hdr_session_is_open = FALSE;

/*----------------------------------------------------------------------
 Enum to remember the state of the V4/V6 iface by HDRPAC. Registering for
Iface_Up event tells the call is already down and registering for Iface_down
event tells that the call is already Up.
----------------------------------------------------------------------*/
typedef enum
{
  HDRPACDSIF_REG_FOR_IFACE_UP = 0,
  HDRPACDSIF_REG_FOR_IFACE_DOWN = 1,
  HDRPACDSIF_NOT_REG = 2
} hdrpacdsif_state_enum_type;

/*----------------------------------------------------------------------
  Call back pointers for each iface, i.e. each PDN instance
----------------------------------------------------------------------*/
LOCAL struct
{
  void *sn_iface_up_cback_ptr;
  void *sn_iface_coming_up_cback_ptr;
  void *sn_iface_routeable_cback_ptr;
  void *sn_iface_down_cback_ptr;
  hdrpacdsif_state_enum_type iface_state;
} hdrpacdsif[DS707_MAX_IFACES];

/* Keeps track of whether we have handed down to 1x */
LOCAL boolean hdrpacdsif_handdown = FALSE;

/* NV Item to save the PPP status */
LOCAL nv_item_type hdrpacdsif_nv_item;

/*===========================================================================

                     FORWARD  FUNCTION DECLARATIONS

===========================================================================*/

LOCAL void hdrpacdsif_iface_up_ind_cback_hdlr
(
void
);

LOCAL void hdrpacdsif_iface_down_ind_cback_hdlr
(
void
);

LOCAL void hdrpacdsif_clear_sn_ppp_status_hdlr
(
void
);

/*===========================================================================

                        FUNCTION DEFINITIONS            

===========================================================================*/


/*===========================================================================
FUNCTION      HDRPACDSIF_PROCESS_SMP_SESSION_CLOSED

DESCRIPTION   Processes the HDR session closed indication.
              It commands DS to end it's data session and then registers
              for the Iface up indication

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_process_smp_session_closed( void )
{

  ps_iface_type      *iface_ptr;
  int16                         ret_val;
  int16                         ps_errno;
  uint8               curr_iface_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

 

  for ( curr_iface_index = 0; 
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {      
    iface_ptr = 
      ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    if ( !PS_IFACE_IS_VALID (iface_ptr) || 
         !PS_IFACE_IS_IN_USE (iface_ptr) )
    {
      /* If it is an invalid IFACE nothing to do continue to other ifaces */
      continue;
    }
         

  /*----------------------------------------------------------------------
    Only if the PPP is Up, that is, then tear down the PPP. If the PPP has
    not been established, then do not tear it down.
  -----------------------------------------------------------------------*/
  
  /*-------------------------------------------------------------------
    Note that DS is commanded to kill PPP after deregistering the Iface
    down event. This is so that HDRPACDSIF does not receive the down event 
    and close the HDR session again.
    The module should register for the Iface Up command after giving the
    command to tear down the Iface. Otherwise, the Iface Up callback will
    be called immediately when registering, since the state is already Up.
    
    The Data Session Abort should not be a graceful close, since there is 
    no HDR session and no data traffic can be sent. For the CDMA SN Iface
    the default is grace_ppp_shutdown is set to False.
  -------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
    Tear down the SN Iface only if it is in UP, ROUTEABLE OR LINGERING
    state. As per KDDI scheme3 requirements, DO NOT tear down a COMING UP 
    PPP. Doc 80-V6103-1 D, section V, #8.
    Tear down the iface only when the data call is on HDR. If the data call
    is on 1x, we should not tear down the call
  -----------------------------------------------------------------------*/
    if ( ds707_pkt_is_um_iface_up(iface_ptr) &&
       ( ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR ))
    {
          DATA_HDR_MSG1 (MSG_LEGACY_MED, "Tear down SN Iface 0x:%x. Reg Iface Up & Routeable", iface_ptr);
  
      /*---------------------------------------------------------------------
        Deregister the Iface down Event
      ---------------------------------------------------------------------*/
          ps_iface_event_cback_dereg(iface_ptr,
                                 IFACE_DOWN_EV,
                                   hdrpacdsif[curr_iface_index].sn_iface_down_cback_ptr);
          hdrpacdsif[curr_iface_index].iface_state = HDRPACDSIF_NOT_REG;  
      
  
      /*-----------------------------------------------------------------
        When HDR session close messg is received instead of calling tear 
        down command multiple times, we will call ps_iface_go_null_cmd. 
        This will cause ref count of iface to goto zero and hence the PPP 
        will be torn down.
      -----------------------------------------------------------------*/
          ret_val = ps_iface_go_null_cmd ( iface_ptr, &ps_errno, NULL );
      if ((ret_val < 0) && (ps_errno != DS_EWOULDBLOCK))
      {
        DATA_HDR_MSG1(MSG_LEGACY_ERROR, "iface go null fail, errno: %d",
                      ps_errno);
      }
    
      /*---------------------------------------------------------------------
         Register for the Iface Up Event
       ---------------------------------------------------------------------*/
      DATA_HDR_MSG1 (MSG_LEGACY_MED, "IFACE 0x%x - Reg for Iface Up & Routeable Events", iface_ptr);
      ps_iface_event_cback_reg(iface_ptr,
                               IFACE_UP_EV,
                               hdrpacdsif[curr_iface_index].sn_iface_up_cback_ptr
                              );  /*lint !e534*/
      hdrpacdsif[curr_iface_index].iface_state = HDRPACDSIF_REG_FOR_IFACE_UP;
    
      ps_iface_event_cback_reg(iface_ptr,
                               IFACE_ROUTEABLE_EV,
                                   hdrpacdsif[curr_iface_index].sn_iface_routeable_cback_ptr
                              );  /*lint !e534*/
    }
    else
    {
      DATA_HDR_MSG0 (MSG_LEGACY_MED, 
                    "PPP not UP or CDSN is 1x. Do not tear down SN Iface V4");
    }
  } /* For each iface */
} /* hdrpacdsif_process_smp_session_closed() */


/*===========================================================================

FUNCTION HDRPACDSIF_HANDDOWN_TO_1X

DESCRIPTION
  This function is called when DS 707 hands down from HDR to 1X. It queues
  up a command to hdrpacdsif informing it that this event has occured.

DEPENDENCIES
  None
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrpacdsif_handdown_to_1x(void)
{
  hdrpac_cmd_type hdrpac_cmd;           /* Build command into local struct */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

  

  /*------------------------------------------------------------------------
  Remove the check for NV item, as of now rely on checking the NV item later in
  DS cmd handler. Revisit Later.
  ------------------------------------------------------------------------ */

  /*-------------------------------------------------------------------------
  Fill in all members of the fcp_cmd_type
  -------------------------------------------------------------------------*/
  hdrpac_cmd.name = HDRPAC_SN_IFACE_HANDDOWN_TO_1X_CMD;
  hdrpac_cmd.sender = HDRHAI_PACKET_APPLICATION_CONTROLLER;

  /*-----------------------------------------------------------------------
  Queue the command and tag it for the Packet Application Controller
  Currently no other task ever sends the cmd, so we should never get here
  -----------------------------------------------------------------------*/

  hdrds_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd ,
                    sizeof(hdrpac_cmd_type) );
} /* hdrpacdsif_handdown_to_1x() */

/*===========================================================================
FUNCTION      HDRPACDSIF_SN_IFACE_UP_IND_CBACK

DESCRIPTION   This is the callback function called when the IFACE comes up.  
              It will queue up a command to be processed in the DS task.
              
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715 can't use arg_not_used macro since this field is union  715*/
LOCAL void hdrpacdsif_sn_iface_up_ind_cback
(
ps_iface_type                *iface_ptr,
ps_iface_event_enum_type      event,
ps_iface_event_info_u_type    event_info,
void                         *user_info_ptr
)
{
  hdrpac_cmd_type hdrpac_cmd;           /* Build command into local struct */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(user_info_ptr);
  DATA_MDM_ARG_NOT_USED(event);
  
  if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

  

  /*-------------------------------------------------------------------------
  Fill in all members of the fcp_cmd_type
  -------------------------------------------------------------------------*/
  hdrpac_cmd.name = HDRPAC_SN_IFACE_UP_CMD;
  hdrpac_cmd.sender = HDRHAI_PACKET_APPLICATION_CONTROLLER;

  /*-----------------------------------------------------------------------
  Queue the command and tag it for the Packet Application Controller
  Currently no other task ever sends the cmd, so we should never get here
  -----------------------------------------------------------------------*/
  hdrds_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd ,
                   sizeof(hdrpac_cmd_type) );
} /* hdrpacdsif_sn_iface_up_ind_cback() */
/*lint -restore Restore lint error 715*/
/*===========================================================================
FUNCTION      HDRPACDSIF_SN_IFACE_COMING_UP_IND_CBACK

DESCRIPTION   This is the callback function called when the IFACE comes up.  
              It will queue up a command to be processed in the DS task.
              
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715 can't use arg_not_used macro since this field is union  715*/
LOCAL void hdrpacdsif_sn_iface_coming_up_ind_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
)
{
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  hdrpac_cmd_type hdrpac_cmd;           /* Build command into local struct */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#endif //FEATURE_DEDICATED_C2K_MODE_DATA
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(user_info_ptr);
  DATA_MDM_ARG_NOT_USED(event);
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  DATA_HDR_MSG0 (MSG_LEGACY_MED, "DS707 IFACE coming up ind cback");

  /*-------------------------------------------------------------------------
  Fill in all members of the fcp_cmd_type
  -------------------------------------------------------------------------*/
  hdrpac_cmd.name = HDRPAC_SN_IFACE_COMING_UP_CMD;
  hdrpac_cmd.sender = HDRHAI_PACKET_APPLICATION_CONTROLLER;

  /*-----------------------------------------------------------------------
  Queue the command and tag it for the Packet Application Controller
  Currently no other task ever sends the cmd, so we should never get here
  -----------------------------------------------------------------------*/
  hdrds_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd ,
                   sizeof(hdrpac_cmd_type) );
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */
} /* hdrpacdsif_sn_iface_coming_up_ind_cback() */

/*lint -restore Restore lint error 715*/
/*===========================================================================
FUNCTION      HDRPACDSIF_SN_IFACE_ROUTEABLE_IND_CBACK

DESCRIPTION   This is the callback function called when the IFACE becomes
              routeable. It will queue up a command to be processed in the
              DS task.
              
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715 can't use arg_not_used macro since this field is union  715*/
LOCAL void hdrpacdsif_sn_iface_routeable_ind_cback
(
ps_iface_type                *iface_ptr,
ps_iface_event_enum_type      event,
ps_iface_event_info_u_type    event_info,
void                         *user_info_ptr
)
{
  hdrpac_cmd_type hdrpac_cmd;           /* Build command into local struct */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(user_info_ptr);
  DATA_MDM_ARG_NOT_USED(event);
  if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

  

  /*-------------------------------------------------------------------------
  Fill in all members of the fcp_cmd_type
  -------------------------------------------------------------------------*/
  hdrpac_cmd.name = HDRPAC_SN_IFACE_ROUTEABLE_CMD;
  hdrpac_cmd.sender = HDRHAI_PACKET_APPLICATION_CONTROLLER;

  /*-----------------------------------------------------------------------
  Queue the command and tag it for the Packet Application Controller
  Currently no other task ever sends the cmd, so we should never get here
  -----------------------------------------------------------------------*/
  hdrds_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd ,
                   sizeof(hdrpac_cmd_type) );
} /* hdrpacdsif_sn_iface_routeable_ind_cback() */

/*lint -restore Restore lint error 715*/

/*===========================================================================
FUNCTION      HDRPACDSIF_SN_IFACE_DOWN_IND_CBACK

DESCRIPTION   This is the callback function called when the IFACE goes
              down. It will queue up a command to be processed in the
              DS task.
              
              
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
/*lint -save -e715 can't use arg_not_used macro since this field is union  715*/
LOCAL void hdrpacdsif_sn_iface_down_ind_cback
(
ps_iface_type                *iface_ptr,
ps_iface_event_enum_type      event,
ps_iface_event_info_u_type    event_info,
void                         *user_info_ptr
)
{
  hdrpac_cmd_type hdrpac_cmd;           /* Build command into local struct */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MDM_ARG_NOT_CONST(iface_ptr);
  DATA_MDM_ARG_NOT_CONST(user_info_ptr);
  DATA_MDM_ARG_NOT_USED(event);
  /*Mark this as a quick solution for disable HDR TIED TO PPP. Complete soluction 
    for all the old JCDMA features will be done in future*/
  //if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

  

  /*-------------------------------------------------------------------------
    Fill in all members of the PAC command type
  -------------------------------------------------------------------------*/
  hdrpac_cmd.name = HDRPAC_SN_IFACE_DOWN_CMD;
  hdrpac_cmd.sender = HDRHAI_PACKET_APPLICATION_CONTROLLER;

  /*-----------------------------------------------------------------------
    Queue the command and tag it for the Packet Application Controller
    Currently no other task ever sends the cmd, so we should never get here
  -----------------------------------------------------------------------*/
  hdrds_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd ,
                   sizeof(hdrpac_cmd_type) );
} /* hdrpacdsif_sn_iface_down_ind_cback() */
/*lint -restore Restore lint error 715*/
/*===========================================================================
FUNCTION      HDRPACDSIF_SN_IFACE_UP_IND_CBACK_HDLR

DESCRIPTION   Called when the IFACE comes up.  Processes the Iface up Event
              
DEPENDENCIES  Needs to run in the ds task context

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void hdrpacdsif_sn_iface_up_ind_cback_hdlr
(
void
)
{
  ps_iface_type             *iface_ptr;
  ps_iface_state_enum_type    iface_state;
  nv_stat_enum_type status;                         /* status from NV call */

  /* boolean to determine 
     when to write_sn_ppp_status_to_nv */
  boolean                     write_sn_ppp_status_to_nv = FALSE;  
  uint8                       curr_iface_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

  

  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index ++ )
  {
    iface_state = IFACE_DOWN;  
    iface_ptr = 
      ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    iface_state = ps_iface_state(iface_ptr);

    /*---------------------------------------------------------------------
    Check if HDRPACDS is registered for Iface Up. If so, then check if
    theIface came up on an HDR system. If so, then need to register for
    the Iface down Event.

    IFACE_GOING_DOWN check is added to take care of case where PPP close is 
    called before HDRPAC handles IFACE_UP indication. PPP close changes IFACE 
    state to IFACE_GOING_DOWN. This check will ensure that IFACE DOWN event 
    gets registered.
    ---------------------------------------------------------------------*/
  if ( (
         (iface_state == IFACE_UP) || 
         (iface_state == IFACE_ROUTEABLE)||
         (iface_state == IFACE_GOING_DOWN)
       )
       #ifndef FEATURE_DEDICATED_C2K_MODE_DATA
       &&
       (ds707_extif_is_pkt_data_sess_on_hdr() == TRUE )
       #endif
     )
  {
    /*---------------------------------------------------------------------
      Deregister for the Iface up Event
    ---------------------------------------------------------------------*/
      ps_iface_event_cback_dereg(iface_ptr,
                               IFACE_UP_EV,
                                 hdrpacdsif[curr_iface_index].sn_iface_up_cback_ptr);
      hdrpacdsif[curr_iface_index].iface_state = HDRPACDSIF_NOT_REG;

      ps_iface_event_cback_dereg(iface_ptr,
                               IFACE_ROUTEABLE_EV,
                               hdrpacdsif[curr_iface_index].sn_iface_routeable_cback_ptr);

    /*---------------------------------------------------------------------
      Register for the Iface Down Event
    ---------------------------------------------------------------------*/
    DATA_HDR_MSG0 (MSG_LEGACY_MED, "Reg for Iface Down");
      ps_iface_event_cback_reg(iface_ptr,
                               IFACE_DOWN_EV,
                               hdrpacdsif[curr_iface_index].sn_iface_down_cback_ptr
                              );  /*lint !e534*/
      hdrpacdsif[curr_iface_index].iface_state = HDRPACDSIF_REG_FOR_IFACE_DOWN;
      write_sn_ppp_status_to_nv = TRUE;
    }
  } /* For each iface */  
  
  if( ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR )
  {
    hdr_session_is_open = TRUE;
  }
  if (write_sn_ppp_status_to_nv)
  {
    /*-------------------------------------------------------------------------
      Get SN_PPP_STATUS flag from NV.
    -------------------------------------------------------------------------*/
    
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_SN_PPP_STATUS_I, &hdrpacdsif_nv_item, ds707_curr_subs_id());
    if (( status == NV_NOTACTIVE_S ) ||
        ( status == NV_DONE_S))
    {
      /*-----------------------------------------------------------------------
         Set the SN_PPP_STATUS flag and write it to NV
      -----------------------------------------------------------------------*/

      hdrpacdsif_nv_item.sn_ppp_status = TRUE;    
      ds3gcfgmgr_write_legacy_nv_ex( NV_SN_PPP_STATUS_I, &hdrpacdsif_nv_item, ds707_curr_subs_id() ); /*lint !e534*/
      

      
    }
	
    else
    /*lint -save -e641 supress converting enum to int warning 641*/
    {
      ERR_FATAL( "Bad NV read status %d", status, 0, 0 );
    }
	/*lint -restore Restore lint error 641*/
  } /* if (write_sn_ppp_status_to_nv) */
} /* hdrpacdsif_sn_iface_up_ind_cback_hdlr() */


/*===========================================================================
FUNCTION      HDRPACDSIF_SN_IFACE_UP_IND_CBACK_HDLR

DESCRIPTION   Called when the IFACE comes up.  Processes the Iface up Event
              
DEPENDENCIES  Needs to run in the ds task context

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void hdrpacdsif_sn_iface_coming_up_ind_cback_hdlr
( 
  void
)
{
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  ps_iface_type             *iface_ptr;
  uint8                     curr_iface_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 

  for ( curr_iface_index = 0; 
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {      
    iface_ptr = 
      ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);

    /*---------------------------------------------------------------------
      Register for the Iface Down Event
    ---------------------------------------------------------------------*/
    DATA_HDR_MSG0 (MSG_LEGACY_MED, "Reg for Iface Down");
    ps_iface_event_cback_reg(iface_ptr,
                             IFACE_DOWN_EV,
                             hdrpacdsif[curr_iface_index].sn_iface_down_cback_ptr
                            );
    hdrpacdsif[curr_iface_index].iface_state = HDRPACDSIF_REG_FOR_IFACE_DOWN;
  }  
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */
} /* hdrpacdsif_sn_iface_coming_up_ind_cback_hdlr() */

/*===========================================================================
FUNCTION      HDRPACDSIF_SN_IFACE_DOWN_IND_CBACK_HDLR

DESCRIPTION   Called when the IFACE goes down.  Processes the Iface down Event
              
DEPENDENCIES  Needs to run in the ds task context

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void hdrpacdsif_sn_iface_down_ind_cback_hdlr
(
void
)
{
  ps_iface_type               *iface_ptr;
  ps_iface_state_enum_type    iface_state;  
  nv_stat_enum_type           status;               /* status from NV call */
  /* CREATE THE WRITE TO NV boolean, set to true when both   */
  boolean                     write_sn_ppp_status_to_nv = FALSE;
  uint8                       curr_iface_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  /*---------------------------------------------------------------------
    If FEATURE_DEDICATED_C2K_MODE is defined then add a check to see if 
    AT is in Hybrid ON mode. If AT is in Hybrid ON mode only then we need 
    to deactivate the HDR session.
    Hybrid mode == OFF means AT is in either 1X only or DO only mode.
    In both these cases we will not bring down the session.
  ---------------------------------------------------------------------*/
  if (ds707_pkt_mgr_get_current_hybrid_pref() == FALSE)
  {
    DATA_HDR_MSG0(MSG_LEGACY_HIGH, "AT not in Hybrid mode. Do not kill HDR session");
  }
  else
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */
  {
    for ( curr_iface_index = 0;
          curr_iface_index < DS707_MAX_IFACES;
          curr_iface_index++ )
    {      
      iface_state = IFACE_STATE_INVALID;
      iface_ptr = 
        ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
      iface_state = ps_iface_state(iface_ptr);
      if ((iface_state != IFACE_DOWN) && (iface_state != IFACE_DISABLED))
      {
        /* If tehre is atleast one IFACE that is being used */
        break;
      }
    }

    /* If there is not even one IFACE that is being used */
    if ( curr_iface_index == DS707_MAX_IFACES )
    {
      /*-----------------------------------------------------------------------
        Close the HDR Session - For the data session currently on HDR, close
      the HDR session gracefully by sending session close. For the data session
      currently on 1x after handdown, close the HDR session silently.
      -----------------------------------------------------------------------*/

      DATA_HDR_MSG0 (MSG_LEGACY_HIGH, "SN Iface down rcvd. Kill HDR Session");

      if ( hdrpacdsif_handdown == FALSE )
      {
        if( hdr_session_is_open )
        {
          
          hdrcp_smp_deactivate(HDRHAI_PACKET_APPLICATION_CONTROLLER, FALSE);
        }
      }
      else
      {
        DATA_HDR_MSG0 (MSG_LEGACY_MED, "Silently Close HDR Session");
        hdrcp_smp_deactivate(HDRHAI_PACKET_APPLICATION_CONTROLLER, TRUE);
      }

      /* Restore the handdown status to FALSE */
      hdrpacdsif_handdown = FALSE;
      hdr_session_is_open = FALSE;
      write_sn_ppp_status_to_nv = TRUE;
    }

    /*---------------------------------------------------------------------
      Check if HDRPACDS is registered for Iface Up. If so, then check if
      theIface came up on an HDR system. If so, then need to register for
      the Iface down Event.

      IFACE_COMING_UP check is added to take care of case where PPP open may 
      get called before HDRPAC handles IFACE_DOWN indication. This check will 
      ensure that IFACE UP, COMING UP and ROUTEABLE events get registered.
    ---------------------------------------------------------------------*/
    for ( curr_iface_index = 0;
          curr_iface_index < DS707_MAX_IFACES;
          curr_iface_index++ )
    {      
      iface_state = IFACE_STATE_INVALID;
      iface_ptr = 
        ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
      iface_state = ps_iface_state(iface_ptr);
      if ((iface_state == IFACE_DOWN) || (iface_state == IFACE_COMING_UP))
      {
        /*---------------------------------------------------------------------
          Deregister the Iface down Event
        ---------------------------------------------------------------------*/
        ps_iface_event_cback_dereg(iface_ptr,
                                   IFACE_DOWN_EV,
                                   hdrpacdsif[curr_iface_index].sn_iface_down_cback_ptr);

        hdrpacdsif[curr_iface_index].iface_state = HDRPACDSIF_NOT_REG;

        /*---------------------------------------------------------------------
          Register for the Iface Up Event
        ---------------------------------------------------------------------*/
        DATA_HDR_MSG0 (MSG_LEGACY_MED, "Reg for Iface Up, Coming_up & Routeable Events");
        ps_iface_event_cback_reg(iface_ptr,
                                 IFACE_UP_EV,
                                   hdrpacdsif[curr_iface_index].sn_iface_up_cback_ptr
                                );  /*lint !e534*/
        hdrpacdsif[curr_iface_index].iface_state = HDRPACDSIF_REG_FOR_IFACE_UP;

    #ifdef FEATURE_DEDICATED_C2K_MODE_DATA
        ps_iface_event_cback_reg(iface_ptr,
                               IFACE_COMING_UP_EV,
                               hdrpacdsif[curr_iface_index].sn_iface_coming_up_cback_ptr
                              );
  #endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

        ps_iface_event_cback_reg(iface_ptr,
                                 IFACE_ROUTEABLE_EV,
                                   hdrpacdsif[curr_iface_index].sn_iface_routeable_cback_ptr
                                );  /*lint !e534*/
      }
    } /* For each iface */  


    if (write_sn_ppp_status_to_nv)
    {

      /*-------------------------------------------------------------------------
        Get SN_PPP_STATUS flag from NV.
      -------------------------------------------------------------------------*/
      status = ds3gcfgmgr_read_legacy_nv_ex( NV_SN_PPP_STATUS_I, &hdrpacdsif_nv_item, ds707_curr_subs_id());
      if (( status == NV_NOTACTIVE_S ) ||
          ( status == NV_DONE_S))
      {
        /*-----------------------------------------------------------------------
             Set the SN_PPP_STATUS flag and write it to NV
        -----------------------------------------------------------------------*/
        hdrpacdsif_nv_item.sn_ppp_status = FALSE;
        ds3gcfgmgr_write_legacy_nv_ex( NV_SN_PPP_STATUS_I, &hdrpacdsif_nv_item, ds707_curr_subs_id() ); /*lint !e534*/
        DATA_HDR_MSG0 (MSG_LEGACY_MED, "NV Item SN_PPP_STATUS set to FALSE");
      }
      /*lint -save -e641 supressing converting enum to int error 641*/
      else
      {
        ERR_FATAL( "Bad NV read status %d", status, 0, 0 );
      }
      /*lint -restore Restore lint error 641*/

    }

  }

} /* hdrpacdsif_sn_iface_down_ind_cback_hdlr() */



/*===========================================================================
FUNCTION      HDRPACDSIF_PROCESS_SN_IFACE_HANDDOWN_TO_1X

DESCRIPTION   This command handler is called when the CDMA SN IFACE hands 
              down from HDR to 1X
              
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void hdrpacdsif_process_sn_iface_handdown_to_1x( void )
{
  if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

  
  hdrpacdsif_handdown = TRUE; 
} /* hdrpacdsif_process_sn_iface_handdown_to_1x() */



/*===========================================================================

FUNCTION HDRPACDSIF_PROCESS_CMD

DESCRIPTION
  This function processes commands for the HDRPACDSIF

DEPENDENCIES
  None
  
PARAMETERS
  pac_cmd_ptr - Pointer to the stucture containing the command

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -e715 pac_cmd_ptr*/
void hdrpacdsif_process_cmd( hdrpac_cmd_type *pac_cmd_ptr )
{
    DATA_MDM_ARG_NOT_CONST(pac_cmd_ptr);
    if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
    {
      return;
    }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    /*-------------------------------------------------------------------------
    Determine which command has been received.
    -------------------------------------------------------------------------*/
    switch ( pac_cmd_ptr->name )
    {
      case HDRPAC_SN_IFACE_UP_CMD:
      case HDRPAC_SN_IFACE_ROUTEABLE_CMD:
        hdrpacdsif_sn_iface_up_ind_cback_hdlr();
        break;
  
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    case HDRPAC_SN_IFACE_COMING_UP_CMD:
      hdrpacdsif_sn_iface_coming_up_ind_cback_hdlr();
      break;
#endif

      case HDRPAC_SN_IFACE_DOWN_CMD:
        hdrpacdsif_sn_iface_down_ind_cback_hdlr();
        break;
  
      case HDRPAC_SN_IFACE_HANDDOWN_TO_1X_CMD:
        hdrpacdsif_process_sn_iface_handdown_to_1x(); 
        break; 
  
      case HDRPAC_SESSION_CLOSED_CMD:
        hdrpacdsif_process_smp_session_closed();
        break;
  
      case HDRPAC_CLEAR_SN_PPP_STATUS_CMD:
        hdrpacdsif_clear_sn_ppp_status_hdlr();
        break;
  
      default:
        /*---------------------------------------------------------------------
        Unrecognized command.
        ---------------------------------------------------------------------*/
        DATA_HDR_MSG1(MSG_LEGACY_ERROR, "PAC DS IF: Recd Unrecognized cmd:%d",
                      pac_cmd_ptr->name);
        break;
    }  

} /* hdrpacdsif_process_cmd() */
/*lint -restore*/

/*===========================================================================
FUNCTION      HDRPACDSIF_POWERUP_INIT

DESCRIPTION   Does the powerup initialization for the module

DEPENDENCIES  Should be called only after CDMA SN iface is created, which
              implies that it should be called after ds707_pkt_mgr_init has
              been called. 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_powerup_init( void )
{


  ps_iface_type             *iface_ptr;
  uint8                     curr_iface_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {
      iface_ptr = 
        ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    /*-----------------------------------------------------------------------
      Allocate memory for the Iface Up/Down callback functions
    -----------------------------------------------------------------------*/
    hdrpacdsif[curr_iface_index].sn_iface_up_cback_ptr = 
    ps_iface_alloc_event_cback_buf(hdrpacdsif_sn_iface_up_ind_cback,
                                   NULL
                                  );
  
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    hdrpacdsif[curr_iface_index].sn_iface_coming_up_cback_ptr = 
    ps_iface_alloc_event_cback_buf(hdrpacdsif_sn_iface_coming_up_ind_cback,
                                   NULL
                                  );
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

    hdrpacdsif[curr_iface_index].sn_iface_routeable_cback_ptr = 
    ps_iface_alloc_event_cback_buf(hdrpacdsif_sn_iface_routeable_ind_cback,
                                   NULL
                                  );
  
    hdrpacdsif[curr_iface_index].sn_iface_down_cback_ptr = 
    ps_iface_alloc_event_cback_buf(hdrpacdsif_sn_iface_down_ind_cback,
                                   NULL
                                  );
  
    /*-----------------------------------------------------------------------
      Register for the Iface Up and Iface Routeable Events.
      In Relay model calls, only IFace Routeable event occurs.
    -----------------------------------------------------------------------*/
    ps_iface_event_cback_reg(iface_ptr,
                             IFACE_UP_EV,
                             hdrpacdsif[curr_iface_index].sn_iface_up_cback_ptr
                            );  /*lint !e534*/
    hdrpacdsif[curr_iface_index].iface_state = HDRPACDSIF_REG_FOR_IFACE_UP;
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    ps_iface_event_cback_reg(iface_ptr,
                           IFACE_COMING_UP_EV,
                           hdrpacdsif[curr_iface_index].sn_iface_coming_up_cback_ptr
                          );
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

  
    ps_iface_event_cback_reg(iface_ptr,
                               IFACE_ROUTEABLE_EV,
                             hdrpacdsif[curr_iface_index].sn_iface_routeable_cback_ptr
                              );  /*lint !e534*/
  
  } /* For each iface */

} /* hdrpacdsif_powerup_init() */

/*===========================================================================
FUNCTION      HDRPACDSIF_POST_SMP_SESSION_CLOSED_CMD_TO_DS

DESCRIPTION   Sends a command to DS task that session closed indication was
              received by PAC.

DEPENDENCIES  None. 

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_post_smp_session_closed_cmd_to_ds( void )
{

  hdrpac_cmd_type hdrpac_cmd;           /* Build command into local struct */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*Mark this as a quick solution for disable HDR TIED TO PPP. Complete soluction 
    for all the old JCDMA features will be done in future*/
  //if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

  /*-------------------------------------------------------------------------
  Fill in all members of the fcp_cmd_type
  -------------------------------------------------------------------------*/
  hdrpac_cmd.name = HDRPAC_SESSION_CLOSED_CMD;
  hdrpac_cmd.sender = HDRHAI_PACKET_APPLICATION_CONTROLLER;

  
  /*-----------------------------------------------------------------------
  Queue the command and tag it for the Packet Application Controller
  Currently no other task ever sends the cmd, so we should never get here
  -----------------------------------------------------------------------*/
  hdrds_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd ,
                   sizeof(hdrpac_cmd_type) );
}

/*===========================================================================
FUNCTION      HDRPACDSIF_CLEAR_SN_PPP_STATUS

DESCRIPTION   This function provides an interface to write the SN_PPP_STATUS
              NV Item. It will be used to clear the NV Item at power up
              
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void hdrpacdsif_clear_sn_ppp_status( void )
{
  hdrpac_cmd_type hdrpac_cmd;           /* Build command into local struct */

  if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*------------------------------------------------------------------------
  Remove the check for NV item, as of now rely on checking the NV item later in
  DS cmd handler. Revisit Later.
  ------------------------------------------------------------------------ */

    /*-------------------------------------------------------------------------
    Fill in all members of the fcp_cmd_type
    -------------------------------------------------------------------------*/
    hdrpac_cmd.name = HDRPAC_CLEAR_SN_PPP_STATUS_CMD;
    hdrpac_cmd.sender = HDRHAI_PACKET_APPLICATION_CONTROLLER;

    /*-----------------------------------------------------------------------
    Queue the command and tag it for the Packet Application Controller
    Currently no other task ever sends the cmd, so we should never get here
    -----------------------------------------------------------------------*/
    hdrds_queue_cmd( HDRHAI_PACKET_APPLICATION_CONTROLLER, &hdrpac_cmd ,
                     sizeof(hdrpac_cmd_type) );

} /* hdrpacdsif_clear_sn_ppp_status() */

/*===========================================================================
FUNCTION      HDRPACDSIF_CLEAR_SN_PPP_STATUS_HDLR

DESCRIPTION   Called when a clear SN PPP status command is received 
              
DEPENDENCIES  Needs to run in the ds task context

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void hdrpacdsif_clear_sn_ppp_status_hdlr(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  {
    return;
  }

  
  hdrpacdsif_nv_item.sn_ppp_status = FALSE;
  ds3gcfgmgr_write_legacy_nv_ex( NV_SN_PPP_STATUS_I, &hdrpacdsif_nv_item, ds707_curr_subs_id());  /*lint !e534*/

} /* hdrpacdsif_sn_iface_up_ind_cback_hdlr() */

/*===========================================================================
FUNCTION      HDRPACDSIF_IS_SN_PPP_UP

DESCRIPTION   This function provides an interface to query the SN PPP status.
              
DEPENDENCIES  None.

RETURN VALUE  
  TRUE  - SN PPP is up (hdrpacdsif registered for IFace DOWN event)
  FALSE - SN PPP is down (hdrpacdsif registered for IFace UP event)

SIDE EFFECTS  None.
===========================================================================*/
boolean hdrpacdsif_is_sn_ppp_up( void )
{
  uint8 curr_iface_index;
  boolean ret_val = FALSE;

  
  /* If any of the ifaces is registered for IFACE down, it means that SN PPP 
     is up */
  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {
    if ( hdrpacdsif[curr_iface_index].iface_state == HDRPACDSIF_REG_FOR_IFACE_DOWN )
    {
      ret_val = TRUE;
      break;
    }
  }

  return ret_val;

} /* hdrpacdsif_is_sn_ppp_up() */

/*===========================================================================
FUNCTION      HDRPACDSIF_GET_DATA_EHRPD_CAPABILITY

DESCRIPTION   This function provides an interface to query if eHRPD 
              capability is allowed or not.
              
DEPENDENCIES  None.

RETURN VALUE  
  TRUE  - eHRPD is allowed
  FALSE - eHRPD is not allowed

SIDE EFFECTS  None.
===========================================================================*/
boolean hdrpacdsif_get_data_ehrpd_capability( void )
{
  boolean is_ehrpd_capability_allowed;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  is_ehrpd_capability_allowed = ds707_is_ehrpd_capability_allowed();

  DATA_HDR_MSG1 (MSG_LEGACY_HIGH, 
                 "HDR Query: eHRPD allowed = %d", 
                 is_ehrpd_capability_allowed );
  return is_ehrpd_capability_allowed; 
}


#endif /* FEATURE_HDR */

