/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   E P Z I D

                                 M A N A G E R

GENERAL DESCRIPTION
  This file contains functions used to handle EPZID changes.

EXTERNALIZED FUNCTIONS
  DS707_EPZID_MGR_PROCESS_CMD
    Processes DS task commands specific to EPZID.  
    
  DS707_EPZID_MGR_INIT
    Registers with for 707 events.  

INITIALIZATION AND SEQUENCING REQUIREMENTS
    Must call ds707_epzid_mgr_init() at startup.

   Copyright (c) 1999-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

   
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_epzid.c_v   1.2   24 Jan 2003 16:09:22   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_epzid_mgr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/18/12   jz      Seperate old JCDMA and Simplified JCDMA 
03/15/11   mg      Global variable cleanup
10/14/10   sa      Adding debug messages before ASSERT.
10/13/10   op      Migrated to MSG 2.0 macros
09/13/10   op      Changed DS707_CBACK_MSG macro to use MSG 2.0 format
02/15/09   ls      Set epzid flag when there is PZID/SID/NID change
02/12/09   ss      Off target Lint Fixes
08/05/08   ms      IS707B addendum Feature support.
07/26/07 ack/squ   Compiler Warning fix
05/10/07   ac      Lint fix
03/08/07   ara     Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X 
22/09/06   rsj     Added JCDMA_2 changes to disable EPZID re-origination
12/07/05   as/kvd  code cleanup due to changed in cm api.
09/09/05   vrk/gr  EPZID enhancements releated to L2/L3 ack
05/16/05   gr      Replaced pzid_orig_allowed with ok_orig_from_dorm call
04/26/05   gr      Added inc_call flag to the ds3g_pkt_call_info_type
04/05/05   gr      Initial version of the file to support IS707-B
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "ds707.h"
#include "ds707_epzid_mgr.h"
#include "ds707_pkt_mgri.h"
#include "ds707_pkt_mgr.h"
#include "dstask.h"
#include "err.h"
#include "mccsyobj.h"
#include "msg.h"
#include "data_msg.h"
#include "ds707_cback.h"
#include "ds707_epzid_util.h"
#include "ds707_epzid.h"
#include "ds707_sid.h"
#include "ds707_nid.h"
#include "ds707_drs.h"
#include "cm.h"
#include "ds_flow_control.h"

#ifdef FEATURE_DS_EPZID_HYSTERESIS
#include "ds707_epzid_hyst.h"
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

#include "ds707_roaming.h"
#include "data_err_suppr.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

LOCAL void  epzidi_dereg_recon(void);

/*---------------------------------------------------------------------------------
 * ds707_epzid_orig_status_pending = This flag is TRUE while waiting for L2 or 
 *                                   L3 ack notice from CM before adding new 
 *                                   EPZID entry to list.
 *                                   
 * ds707_epzid_orig_after_tc_exit = This flag is TRUE if EPZID orig failed 
 *                                  because Traffic Channel is in use.  
 *                                  
 * ds707_epzid_orig_after_holddown_tmr_exp = This flag is TRUE if EPZID orig failed 
 *                                           because HOLD DOWN TIMER is running
 ----------------------------------------------------------------------------------*/ 
LOCAL boolean ds707_epzid_orig_status_pending = FALSE;
LOCAL boolean ds707_epzid_orig_after_tc_exit = FALSE;
LOCAL boolean ds707_epzid_orig_after_holddown_tmr_exp = FALSE;

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      EPZIDI_CHANGE_CB

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called when a Pkt Zone ID change has been detected
              while the mobile is idle or active.  Mobile could be active
              in either a Vox call (and could be either SVD enabled or not)
              or could be active in any type of data call.
              
              Sends a command to DS, so that actual processing is done in 
              DS task context.
              
              This function is called from the context of the MC task.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_change_cb
(
  word sid,                         /* System   Identifier */
  word nid,                         /* Network  Identifier */
  byte pzid                         /* Pkt Zone Identifier */
)
{
  ds_cmd_type             *cmd_ptr       = NULL;            /* DS Task msg */
  ds707_epzid_type        *new_epzid_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "EPZID CHANGE - post msg to DS");

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_epzid_type));
  if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
  {
    ASSERT(0);
    return;
  }

  /*------------------------------------------------------------------------
    Save the new SID/NID/PZID in the EPZID format & post the command
  ------------------------------------------------------------------------*/  
  cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_EPZID_CHANGE;

  new_epzid_ptr = (ds707_epzid_type *)cmd_ptr->cmd_payload_ptr;
  memset(new_epzid_ptr, 0, sizeof(ds707_epzid_type));

  new_epzid_ptr->sid  = sid;
  new_epzid_ptr->nid  = nid;
  new_epzid_ptr->pzid = pzid;

  ds_put_cmd( cmd_ptr );

} /* epzidi_change_cb() */


/*===========================================================================
FUNCTION      EPZIDI_REGISTER

DESCRIPTION   This function register's the EPZID change call back and 
			   REG_SUBSTATE callback with MC

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void epzidi_register 
(
  void
)
{
  /*-----------------------------------------------------------------------
  Do not register with MC for epzid change and perform any additional 
  operations if FEATURE_JCDMA_2 is enabled.
  -----------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
  Simplified JCDMA: 
  4.2.6 Disable PZID/SID/NID reconnection [D1-No.62] 
  -----------------------------------------------------------------------*/
  if(!ds707_roaming_is_curr_mode_jcdma() &&
     !ds707_roaming_is_simplified_jcdma_mode())
  {

    ds707_epzid_type curr_epzid;
    memset(&curr_epzid, 0, sizeof(ds707_epzid_type));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*-----------------------------------------------------------------------
      Register EPZID change handler with mccsyobj_system
    -----------------------------------------------------------------------*/
    mccsyobj_reg_epzid_change_handler(ds707_epzid_change_cb);

    /*-----------------------------------------------------------------------
     It's possible the registration happened after the EPZID change callback
     would have occured (meaning we don't get a callback for the current
     EPZID).  So make sure current PZID is in the list
   -----------------------------------------------------------------------*/
    mccsyobj_get_epzid( &curr_epzid.sid, &curr_epzid.nid, &curr_epzid.pzid );

    /*---------------------------------------------------------------
    First save Save the Input EPZID for comparison 
    during the next EPZID change. 
    ---------------------------------------------------------------*/
    ds707_epzid_set_current_epzid(&curr_epzid);

    /*-----------------------------------------------------------------------
    PZID should be non-zero for a valid system. Following check ensures that
    the PZID retrieved from MC is valid
    -----------------------------------------------------------------------*/
    if (curr_epzid.pzid != 0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Just reg'd for EPZID - add curr epzid to list");

      /*-----------------------------------------------------------------------
     * The EPZID list is empty. So set the orig status flag to process the
     * phys_link up event to add the current (first) epzid into the list.
    -----------------------------------------------------------------------*/

      ds707_epzid_orig_status_pending = ds707_epzid_change_handler(&curr_epzid);

      if(ds707_epzid_orig_status_pending == FALSE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "EPZID list should be empty. reorig should be true");
      }
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Ignoring registration of epzid_change_cb with MC");
  }

} /* epzidi_register */

/*===========================================================================
FUNCTION      EPZIDI_REG_RECON

DESCRIPTION   This function registers EPZID based reconnection for packet
              calls.  It registers the pzid change callbacks and
              initializes the EPZID list to one entry.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is initiated (packet
              service goes into active state).
  
              Note, the idle change handler will reoriginate a packet call on
              detecting a EPZID change.
          
              This function can be called from the DS task.  Note that this 
              can be called multiple times in a data call, as the IFACE goes
              between ROUTEABLE and UP (could be Rm MIP call).  Protect
              against losing EPZID list by making callback hasn't been
              registered by checking max_pzid_list_len.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void  epzidi_reg_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
  Max List length 0 indicates EPZID based reconnection disabled. Enable the
  EPZID based reconnection by setting the Max list length = 1. If the max 
  list length is already > 0, reconnection is already enabled
  -----------------------------------------------------------------------*/
  if (ds707_epzid_get_epzid_max_list_len() == 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Starting EPZID based reconnection");
    
    ds707_epzid_update_max_list_len(1);

    /* Clear the EPZID List */
    ds707_epzid_init_list();

#ifdef FEATURE_DS_EPZID_HYSTERESIS
  /*-----------------------------------------------------------------------
   Initialize the PZID hysteresis state by default (as per standard)
  -------------------------------------------------------------------------*/
    ds707_epzid_set_hysteresis_enabled(TRUE);
#ifdef FEATURE_IS707B_ADDENDUM
    /*-----------------------------------------------------------------------
    Register the reg_substate change handler with mccsyobj_system
  -----------------------------------------------------------------------*/
  mccsyobj_epzid_reg_sub_state_handler(ds707_epzid_hyst_reg_substate_cb);
#endif /* FEATURE_IS707B_ADDENDUM*/
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

    epzidi_register();
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "EPZID callback already registered");
  }
}  /* epzidi_reg_recon() */

/*===========================================================================
FUNCTION      EPZIDI_CHECK_FOR_NEW_EPZID

DESCRIPTION   This function queries MC to find the current EPZID. If this
              EPZID is not on top of the EPZID list, it fakes a EPZID change 
              notification and hence invokes epzidi_change_cb

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void  epzidi_check_for_new_epzid(void)
{
  ds707_epzid_type        curr_epzid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mccsyobj_get_epzid( &curr_epzid.sid, &curr_epzid.nid, &curr_epzid.pzid );

  if (ds707_epzid_get_epzid_max_list_len() > 0)
  {
    if(ds707_epzid_is_first_epzid(&curr_epzid) == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Poke EPZID on potential new EPZID");
      ds707_epzid_change_cb(curr_epzid.sid,curr_epzid.nid, curr_epzid.pzid);
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "EPZID no longer registered");
  }
} /* epzidi_check_for_new_epzid() */

/*===========================================================================
FUNCTION      EPZIDI_REORIG_HDLR

DESCRIPTION   Called when either PZID, SID, or NID has changed, requiring
              mobile to come out of dormancy.

DEPENDENCIES  runs in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void epzidi_reorig_hdlr
(
  void
)
{
/*-------------------------------------------------------------------------
   If FEATURE_JCDMA_2 is used, then do not re-originate at all. Added
   precaution.
  -------------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
  Simplified JCDMA: 
  4.2.6 Disable PZID/SID/NID reconnection [D1-No.62] 
  -----------------------------------------------------------------------*/
  if(!ds707_roaming_is_curr_mode_jcdma() &&
     !ds707_roaming_is_simplified_jcdma_mode())
  {
    ps_phys_link_type        *phys_link_ptr;
    ds3g_pkt_call_info_type  pkt_info;
    int16                     err_num;
    int                       ret_val;
    boolean                  *ppp_resync_ptr;
    ds707_epzid_type          curr_epzid;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_HDR
    /*-------------------------------------------------------------------------
      We should originate a call only if our data session is on 1X.
    -------------------------------------------------------------------------*/
    if(ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Ignore EPZID change command in HDR mode");
      return;
    }
#endif

    phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

    if(phys_link_ptr == NULL)
    {
      DATA_ERR_FATAL("phys_link_ptr is NULL");
      return;
    }

    /*-------------------------------------------------------------------------
      See if it is okay to originate on PZID/SID/NID change.
      Conditions for Origination - 
           1. Phys link should be either DOWN/GOING_DOWN (Dormancy)
           2. HOLDDOWN_TIMER should not be running now
    -------------------------------------------------------------------------*/
    if( PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_RESUMING )
    {
      /* -----------------------------------------------------------------------
       * If the phys link is in resuming state (e.g. bcoz of a ping from laptop),
       * then set the orig status pending to be true
       * ----------------------------------------------------------------------*/
      DATA_IS707_MSG1(MSG_LEGACY_MED, "Phys Link %d in resuming state setting epzid orig pending true",
              phys_link_ptr);
      ds707_epzid_orig_status_pending = TRUE;
      return;
    }
    else if( PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_UP )
    {
      /* --------------------------------------------------------------------
       * If we receive an EPZID change in connected state, then add the epzid 
       * to the list without sending origination (standard specification)
       * In the connected state we go to init EPZID hysteris state if ppp
       * data is exchanged. So in most (except for n/w standard non compliance) 
       * connected stated EPZID list size is one. 
       * -----------------------------------------------------------------*/
      ds707_epzid_get_current_epzid(&curr_epzid);
      (void) ds707_epzid_add_epzid(&curr_epzid);
      ds707_epzid_orig_status_pending = FALSE;
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "EPZID change during call connected,EPZID added to list");
      return;
    }
    else if (ds707_pkti_ok_orig_from_dorm(
      phys_link_ptr,DS707_PKT_DORM_REASON_PZ_S_NID) == FALSE)
    {
      /* cannot reorig because data session is not dormant */ 
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "No EPZID reorig. In phys link state %d",
               PS_PHYS_LINK_GET_STATE(phys_link_ptr));
      return;
    }
    /* its ok to orig from dormancy but hold down timer prevents us from orig */
    else if ((DS_FLOW_CTRL_IS_BIT_SET(ps_phys_link_get_flow_mask(phys_link_ptr),
                                      DS_HOLDDOWN_TIMER_MASK)) == TRUE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "No EPZID reorig since hold down timer is running");
      ds707_epzid_orig_after_holddown_tmr_exp = TRUE;
      return;
    }
    /*-------------------------------------------------------------------------
    This struct is passed onto the iface.  PZID/SID/NID normallly have no
    data to send.  PZID/SID/NID reconnections override the holddown timer
    For Non TE Based origination force_cdma_only should always be FALSE
    Set the incoming call flag to FALSE for all origination attempts
    -------------------------------------------------------------------------*/
    pkt_info.drs_bit         = FALSE;
    pkt_info.use_hdown_timer = FALSE;
    pkt_info.new_network     = ds707_pkt_get_current_data_sess_network();
    pkt_info.force_cdma_only = FALSE;
    pkt_info.inc_call        = FALSE;

    ppp_resync_ptr   = ds707_get_ppp_resync_flag_ptr();

    pkt_info.drs_bit = ds707_drs_get_mip_drs_bit();
    *ppp_resync_ptr  = pkt_info.drs_bit;

#ifdef FEATURE_IS707B_ADDENDUM
    /*-------------------------------------------------------------------------
    We need to set EPZID flag here. Later on, when making 
    origination call to CM if the flag is set, we need to tell CM that it is 
    a IS707B data call
    ---------------------------------------------------------------------------*/

    pkt_info.epzid  = TRUE;

#endif 

    ret_val = ps_phys_link_up_cmd(phys_link_ptr, &err_num, &pkt_info);

    if ((ret_val < 0) && (err_num != DS_EWOULDBLOCK))
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "ps_iface reorig failed %d", err_num);
    }
    else
    {
      /* already sent a successful orig to CM */
      ds707_epzid_orig_after_holddown_tmr_exp = FALSE;
      ds707_epzid_orig_after_tc_exit = FALSE; 
      /* Waiting for L2 or L3 ACK */
      ds707_epzid_orig_status_pending = TRUE;
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "JCDMA 2 - EPZID reorigination being ignored in epzidi_reorig_hdlr");
  }
} /* epzidi_reorig_hdlr() */

/*===========================================================================
FUNCTION       EPZIDI_EVENT_CBACK

DESCRIPTION    Callback when a phys link event occurs.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void epzidi_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
  ds707_epzid_type curr_epzid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "phys link event %d, nest_level %d", 
                  event, ds707_cback_get_nest_level());
  
  switch(event)
  {
    /*-----------------------------------------------------------------------
    IFACE came up. Register with MC and enable reconnection - set max list
    length =1 and enable hysteresis by default
    -----------------------------------------------------------------------*/
    case DS707_CBACK_PHYS_LINK_COMING_UP_EV:
      epzidi_reg_recon();
#ifdef FEATURE_DS_EPZID_HYSTERESIS
      /*----------------------------------------------------------------------
        Invoke the EPZID Hystersis Module's DS707 Event Handler. EPZID Manager
        also registers for DS707 Events handled by the Hysteresis Engine
      ----------------------------------------------------------------------*/
      ds707_epzid_hyst_ds707_event_hdlr(event, event_info_ptr);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
      break;

    case DS707_CBACK_PHYS_LINK_UP_EV:
#ifdef FEATURE_DS_EPZID_HYSTERESIS
      /*----------------------------------------------------------------------
        If the last  Origination succeeded and if the orig_status flag is TRUE
        add the current EPZID into the list. Also add the current EPZID into
        the list if the list is Empty (First EPZID notified by MC)
      ----------------------------------------------------------------------*/
      if (ds707_epzid_orig_status_pending || 
          (ds707_epzid_get_epzid_list_len() == 0 ))
      {
      	ds707_epzid_get_current_epzid(&curr_epzid);
      	(void) ds707_epzid_add_epzid(&curr_epzid);
      	/* no longer waiting for L2 L3 ack */
        ds707_epzid_orig_status_pending = FALSE;
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "EPZID added to the list after call connected");
      }	
      /*----------------------------------------------------------------------
        Invoke the EPZID Hystersis Module's DS707 Event Handler. EPZID Manager
        also registers for DS707 Events handled by the Hysteresis Engine
      ----------------------------------------------------------------------*/
      ds707_epzid_hyst_ds707_event_hdlr(event, event_info_ptr);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
      break;
      
    /*-----------------------------------------------------------------------
    Phys link going down and if dormancy not allowed, deregister the EPZID
    notification with MC and also disable EPZID reconnection functionality
    -----------------------------------------------------------------------*/
    case DS707_CBACK_PHYS_LINK_DOWN_EV:
      if (event_info_ptr->going_dormant == FALSE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "phy link DOWN null - dereg EPZID");
        epzidi_dereg_recon();
      }
      else
      {
         /*--------------------------------------------------------------
         * This call back function will be called whenever phys_link 
         * goes down, but EPZID is interested only in this event
         * only when it has sent an origination and is expecting a
         * L2 or L3 ack
         ---------------------------------------------------------------*/
         if (ds707_epzid_orig_status_pending == TRUE)
         {
             /* -------------------------------------------------------------
             * If EPZID orig failed because TC is in use (e.g. voice call)
             * then remember to reorig when TC is free 
             * CM_CALL_END_INTERNAL_ERROR is returned if in a voice call
             * (in this case the call is rejected by CM right away and 
             *  does not go to MC or Base Station (BS)
             *
             * CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS condition is met when the 
             * request reached the Base Station (BS) and is rejected by the 
             * Base Station when Concurrent Services is not supported
             * ------------------------------------------------------------------*/ 
             if(
                ((event_info_ptr->call_end_info).end_status == 
                                                CM_CALL_END_CCS_NOT_SUPPORTED_BY_BS) 
                  || ((event_info_ptr->call_end_info).end_status ==  
                                                CM_CALL_END_INTERNAL_ERROR)
               )
             {
                DATA_IS707_MSG2(MSG_LEGACY_MED, " EPZID not added to list bcoz TC in use status:%d  fail_reason:%d",
                         (event_info_ptr->call_end_info).end_status,
                         (event_info_ptr->call_end_info).highest_call_fail_reason
                         ); 
                ds707_epzid_orig_after_tc_exit = TRUE;
              }
             /*-------------------------------------------------------------------
             * If the call end was due to a NORMAL RELEASE or if the call failed 
             * with any Highest Failure reason other than L3 failure 
             * (L2 Ack failure or any other lower layer failure), add the current
             * EPZID to the to the list
             * -----------------------------------------------------------------*/
             else
             {
                DATA_IS707_MSG2(MSG_LEGACY_MED, "Attempting to add EPZID with end_status:%d, fail_reason:%d",
                       (event_info_ptr->call_end_info).end_status,
                       (event_info_ptr->call_end_info).highest_call_fail_reason
                       ); 
                if( ((event_info_ptr->call_end_info).end_status == CM_CALL_END_REL_NORMAL) ||
                    (
                     ((event_info_ptr->call_end_info).end_status != CM_CALL_END_REL_NORMAL) 
                     &&
                     ((event_info_ptr->call_end_info).highest_call_fail_reason 
                                                                 == CM_CALL_ORIG_ERR_LAYER_L3)
                    )
                  )
                {
                  ds707_epzid_get_current_epzid(&curr_epzid);
                  (void) ds707_epzid_add_epzid(&curr_epzid);
                  #ifdef FEATURE_DS_EPZID_HYSTERESIS
                  /*----------------------------------------------------------
                  Start HT timer only if there was no HT running - i.e when 
                  the EPZID list has exactly two elements in the list after 
                  adding a new entry 
                  ----------------------------------------------------------*/
                  if (ds707_epzid_get_epzid_list_len() == 
                                        DS707_EPZID_MIN_LIST_SIZE_TO_START_HT)
                  {
                  DATA_IS707_MSG0(MSG_LEGACY_MED, "Starting HT after adding EPZID to list physlink down");
                  ds707_epzid_hyst_start_ht();
                  }
                  #endif /* FEATURE_DS_EPZID_HYSTERESIS */
                  /*-------------------------------------------------------------------
                  * Do not clear the orig_status_pending flag even though we got a 
                  * call failure for the current EPZID origination. If the next 
                  * Origination due to data or any other reason goes past L2/L3 ack
                  * clear the orig_status_pending flag
                  -----------------------------------------------------------------*/
                  ds707_epzid_orig_status_pending = FALSE;
                  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "EPZID added to the list after call end, end_status:%d, fail_reason:%d",
                                 (event_info_ptr->call_end_info).end_status,
                                 (event_info_ptr->call_end_info).highest_call_fail_reason
                                 ); 
                }/*end of if case in which we add epzid to the list */
                else
                {
                  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "EPZID call orig failed with end_status:%d, fail_reason:%d",
                             (event_info_ptr->call_end_info).end_status,
                             (event_info_ptr->call_end_info).highest_call_fail_reason
                             ); 
                }
             }
         }/* end of if (orig_status_pending = TRUE)*/
       } /*end of else part i.e. going_dormant=TRUE*/

#ifdef FEATURE_DS_EPZID_HYSTERESIS
      /*----------------------------------------------------------------------
        Invoke the EPZID Hystersis Module's DS707 Event Handler. EPZID Manager
        also registers for DS707 Events handled by the Hysteresis Engine
      ----------------------------------------------------------------------*/
      ds707_epzid_hyst_ds707_event_hdlr(event, event_info_ptr);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
      break;

    /*-----------------------------------------------------------------------
    Data(PPP) session aborted,deregister the EPZID notification with MC 
    and also disable EPZID reconnection functionality
    -----------------------------------------------------------------------*/
    case DS707_CBACK_ABORT_SESSION_EV:
      epzidi_dereg_recon();

#ifdef FEATURE_DS_EPZID_HYSTERESIS
      /*----------------------------------------------------------------------
        Invoke the EPZID Hystersis Module's DS707 Event Handler. EPZID Manager
        also registers for DS707 Events handled by the Hysteresis Engine
      ----------------------------------------------------------------------*/
      ds707_epzid_hyst_ds707_event_hdlr(event, event_info_ptr);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
      break;

    /*-----------------------------------------------------------------------
    Hold down timer expired - Start the delay timer 
    -----------------------------------------------------------------------*/
  case DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV:
    /* Place Debug messages for testing */
    /*-----------------------------------------------------------------------
    Simplified JCDMA: 
    4.2.6 Disable PZID/SID/NID reconnection [D1-No.62] 
    -----------------------------------------------------------------------*/
      if(ds707_roaming_is_curr_mode_jcdma() ||
         ds707_roaming_is_simplified_jcdma_mode())
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, "JCDMA 2 - In Hold Down Timer Stopped Event");
      }
      else
      {
         /* if epzid orig pending and waiting for hld_down timer exp */ 
         if(ds707_epzid_orig_after_holddown_tmr_exp)
         {
          /* attempt an epzid reorig */
          epzidi_reorig_hdlr(); 
         } 
      }
      break;

    /*----------------------------------------------------------------------
    Disable PZID reconnection for a SID change only if 
    the EPZID type does not include the SID. Force EPZID Hysteresis
    Engine to NULL state
    -----------------------------------------------------------------------*/
    case DS707_CBACK_SID_CHANGED_EV:
      if (ds707_epzid_get_active_epzid_type() == NV_EPZID_PZID)
      {
        ds707_epzid_disable_recon();

#ifdef FEATURE_DS_EPZID_HYSTERESIS
      /*----------------------------------------------------------------------
        Invoke the EPZID Hystersis Module's DS707 Event Handler. EPZID Manager
        also registers for DS707 Events handled by the Hysteresis Engine
      ----------------------------------------------------------------------*/
        ds707_epzid_hyst_ds707_event_hdlr(event, event_info_ptr);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
      }
      break;

    /*----------------------------------------------------------------------
      Network has changed. If the new network is HDR, clear the EPZID list
	  and disable reconnection. Set the max_list_len=1. If the new network
	  is CDMA, query MC to find the current EPZID and update the list  
    ----------------------------------------------------------------------*/
    case DS707_CBACK_NEW_NET_EV:
      if(ds707_pkt_is_handoff_from_1x_to_hdr(
         event_info_ptr->curr_network, event_info_ptr->new_network))
      {
        ds707_epzid_disable_recon();
      }
      else if(ds707_pkt_is_handoff_from_hdr_to_1x(
         event_info_ptr->curr_network, event_info_ptr->new_network))
      {
        /*----------------------------------------------------------------------
          There was a system change and the mobile moved to CDMA system just 
          now. Query MC to find the current EPZID and update the list  
        ----------------------------------------------------------------------*/
        epzidi_check_for_new_epzid();
      }
#ifdef FEATURE_DS_EPZID_HYSTERESIS
      /*----------------------------------------------------------------------
        Invoke the EPZID Hystersis Module's DS707 Event Handler. EPZID Manager
        also registers for DS707 Events handled by the Hysteresis Engine
      ----------------------------------------------------------------------*/
      ds707_epzid_hyst_ds707_event_hdlr(event, event_info_ptr);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
      break;

    default:
      break;
  }
} /* epzidi_event_cback() */


/*===========================================================================
FUNCTION      EPZIDI_PROCESS CHANGED_EPZID

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called when a Pkt Zone ID change has been detected
              while the mobile is idle or active.  Mobile could be active
              in either a Vox call (and could be either SVD enabled or not)
              or could be active in any type of data call.
       
              This function follows procedures from IS-707.A.5 sec.2.2.7. 
              If the new PZID was 0, it resets PZID based reconnection.
              It adds the new PZID to the visited PZID list.
              If the new PZID is not on the list of the PZIDs, this function
              sends a command to DS task to originate a packet call.
                                   
              This function is called from the context of the MC task.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void epzidi_process_changed_epzid
(
  
  ds707_epzid_type *new_epzid_ptr         /* New Enhanced Pkt Zone ID      */
)
{
  /*-----------------------------------------------------------------------
  Simplified JCDMA: 
  4.2.6 Disable PZID/SID/NID reconnection [D1-No.62] 
  -----------------------------------------------------------------------*/
  if(!ds707_roaming_is_curr_mode_jcdma() &&
     !ds707_roaming_is_simplified_jcdma_mode())
  {
    boolean sid_reorig   = FALSE;  /* SID re-origination indicator */
    boolean nid_reorig   = FALSE;  /* NID re-origination indicator */
    boolean epzid_reorig = FALSE;  /* EPZID re-origination indicator */
    boolean reorig_flag  = FALSE;  /*overall re-orig indicator(SID|NID|EPZID)*/
    ds707_epzid_type curr_epzid;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    if(new_epzid_ptr == NULL)
    {
	    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid pointer passed: %x", new_epzid_ptr);
      ASSERT(0);
      return;
    }
    /*-------------------------------------------------------------
     * To obtain the current EPZID for comparison, we should always
     * use the ds707_epzid_get_current_epzid function. This function
     * returns the current epzid that we are in. Also note that the 
     * function ds707_epzid_get_current_epzid_from_list function 
     * with index 0 can be used to obtain the last epzid corresponding 
     * to the last origination that was sent.
    -------------------------------------------------------------  */
    ds707_epzid_get_current_epzid(&curr_epzid);

    sid_reorig = ds707_sid_change_handler (
                                            curr_epzid.sid, 
                                            new_epzid_ptr->sid
                                          );

    nid_reorig = ds707_nid_change_handler (
                                            curr_epzid.nid,
                                            new_epzid_ptr->nid
                                          );

    switch(ds707_epzid_get_active_epzid_type())
    {
      case NV_EPZID_PZID:

        /*-------------------------------------------------------------
        SID change or a NID change would also enable the re-orig flag
        here since they are not a part of the EPZID
        -------------------------------------------------------------*/
        reorig_flag = (sid_reorig | nid_reorig);
        break;
    
      case NV_EPZID_PZID_SID:
    
        /*-------------------------------------------------------------
        NID change would also enable the re-orig flag here since it 
        is  not a part of the EPZID
        -------------------------------------------------------------*/
        reorig_flag = nid_reorig;
        break;
  
      case NV_EPZID_PZID_SID_NID:
	    break;

      default:
	    DATA_ERR_FATAL("Invalid EPZID Type");
	    break;
    }

    /*---------------------------------------------------------------
    First save Save the Input EPZID for comparison 
    during the next EPZID change. 
    ---------------------------------------------------------------*/
    ds707_epzid_set_current_epzid(new_epzid_ptr);

    /*-------------------------------------------------------------------------
    New PZID received was 0. Disable Reconnection and force 
    PZID Hysteresis State to GO NULL 
    -------------------------------------------------------------------------*/
    if ( new_epzid_ptr->pzid == 0) 
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Got new EPZID with PZID=0, force EPZID Hyst to NULL state");
#ifdef FEATURE_DS_EPZID_HYSTERESIS
      ds707_epzid_hysteresis_null_state();
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
    }
    else
    {
#ifdef FEATURE_DS_EPZID_HYSTERESIS
      /*-----------------------------------------------------------------------
      Transition Hysteresis to INIT state, but set the data_transfer_flag=FALSE 
      since there is no data exchange at this instant. 
      -----------------------------------------------------------------------*/
      if (ds707_epzid_get_epzid_list_len() == 0)
      {
        ds707_epzid_hysteresis_init_state(FALSE);
      }
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
    }

    /*-----------------------------------------------------------------------
    epzid_reorig_flag is used instead of reorig_flag here. This is because
    the epzid_reorig_flag is used to start HT on the oldest EPZID, if the 
    list has exactly two entries after adding the new EPZID
    -----------------------------------------------------------------------*/
    epzid_reorig = ds707_epzid_change_handler(new_epzid_ptr);

   /* -----------------------------------------------------------------------
    * If we move from EPZID1 -> EPZID2 -> EPZID1 when holddown timer is running
    * or when we are waiting for TC_EXIT, we should not orig after the expiry
    * of holddown timer or after TC_EXIT
    * -----------------------------------------------------------------------*/ 
    if(epzid_reorig == FALSE)
    {
       DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Resetting epzid reorig flags since new epzid is already in list");
		   /* Print the contents of the EPZID List */
       ds707_epzid_print_epzid_list();
       ds707_epzid_orig_after_tc_exit = FALSE;
       ds707_epzid_orig_after_holddown_tmr_exp = FALSE;
    }
     
    reorig_flag |= epzid_reorig;
 
    if (reorig_flag) 
    { 
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Originating after processing EPZID change");
      epzidi_reorig_hdlr();
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "JCDMA 2 - EPZID changes still being notified. Regorigination may still happen.");
  }

} /* epzidi_process_changed_epzid() */

/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_EPZID_MGR_INIT

DESCRIPTION    Initialize EPZID Manager module.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_epzid_mgr_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_COMING_UP_EV,
                            epzidi_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Can't register for phys link coming up event",0,0,0);
  }

  if (ds707_cback_register(
                             DS707_CBACK_PHYS_LINK_UP_EV,
                             epzidi_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Can't register for phys link up event",0,0,0);
  }

  if (ds707_cback_register(
                             DS707_CBACK_PHYS_LINK_DOWN_EV,
                             epzidi_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Can't register for phys link down event",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_ABORT_SESSION_EV,
                            epzidi_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Can't register for abort session event",0,0,0);
  }

 /*-----------------------------------------------------------------
   Do not register for DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV
   if JCDMA_2 is used. This event causes re-origination.
   -----------------------------------------------------------------*/
  /*-----------------------------------------------------------------------
  Simplified JCDMA: 
  4.2.6 Disable PZID/SID/NID reconnection [D1-No.62] 
  -----------------------------------------------------------------------*/
  if(!ds707_roaming_is_curr_mode_jcdma() &&
     !ds707_roaming_is_simplified_jcdma_mode())
  {
    if(ds707_cback_register(
                              DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV,
                              epzidi_event_cback
                            ) == FALSE)
    {
      ERR_FATAL("Can't register for holddown timer event",0,0,0);
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Ignoring DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV registration");
  }

  if (ds707_cback_register(
                            DS707_CBACK_SID_CHANGED_EV,
                            epzidi_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Can't register for sid changed event",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_NEW_NET_EV,
                            epzidi_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg NEW_NET_EV cback",0,0,0);
  }
  ds707_epzid_init_list();
} /* ds707_epzid_mgr_init() */

/*===========================================================================
FUNCTION      EPZIDI_MGR_DEREG_RECON

DESCRIPTION   This function deregisters PZID based reconnection for packet
              calls.  It sets the PZID change handlers to NULL and resets
              the visited PZID list.
   
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is ended (packet
              service goes into Inactive state) - not called when packet data
              service goes dormant.
   
              This function can be called from the DS task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void  epzidi_dereg_recon
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0(MSG_LEGACY_MED, "Stopping  EPZID based reconnection");
  /*-------------------------------------------------------------------------
    Deregister PZID change handler with mccsyobj_system
  -------------------------------------------------------------------------*/
  mccsyobj_reg_epzid_change_handler( NULL);

  #ifdef FEATURE_IS707B_ADDENDUM
  mccsyobj_epzid_reg_sub_state_handler(NULL);
  #endif /*  FEATURE_IS707B_ADDENDUM */

  /*-------------------------------------------------------------------------
     Reset the listed of visited packet zones.
  -------------------------------------------------------------------------*/
  ds707_epzid_update_max_list_len(0);
#ifdef FEATURE_DS_EPZID_HYSTERESIS
  ds707_epzid_set_hysteresis_enabled(FALSE); 
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
}  /* epzidi_dereg_recon() */


/*===========================================================================
FUNCTION DS707_EPZID_EXIT_TC_HDLR

DESCRIPTION
  This function contains the EPZID specific hanling of the EXIT_TC event
  generated by CM when the CDMA traffic channel is torn down. It checks 
  whether there is a pending origination waiting for the traffic channel 
  to be available, if so it calls the function to perform the origination.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void ds707_epzid_exit_tc_hdlr(ds_cmd_type *epzid_cmd_ptr)
{
  DATA_MDM_ARG_NOT_CONST(epzid_cmd_ptr);
  if(!ds707_roaming_is_curr_mode_jcdma() &&
     !ds707_roaming_is_simplified_jcdma_mode())
  {
    if((epzid_cmd_ptr->hdr).cmd_id != DS_CMD_707_EXIT_TC)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Serious ctrl flow error func only for DS_707_EXIT_TC_CMD");
      return;
    }

    if(ds707_epzid_orig_after_tc_exit == TRUE)
    {
      /* call the fuction to perform the origination */
      DATA_IS707_MSG0(MSG_LEGACY_LOW, "Calling epzidi_reorig_hdlr from exit_tc_hdlr");
      epzidi_reorig_hdlr();
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "JCDMA 2 - In Exit_TC Hndlr. Exit_TC handler still causing origination");
  }
}

/*===========================================================================
FUNCTION      DS707_EPZID_MGR_PROCESS_CMD

DESCRIPTION   This function processes PZID specific commands.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_mgr_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  byte socm_msg_id;
  boolean reorig_flag = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds_cmd_ptr == NULL)
  {
    DATA_ERR_FATAL("Invalid ds_cmd_ptr passed");
    return;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, "EPZID : rcvd cmd %d", ds_cmd_ptr->hdr.cmd_id);

  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_PKT_EPZID_CHANGE:
      epzidi_process_changed_epzid(
                          (ds707_epzid_type *)(ds_cmd_ptr->cmd_payload_ptr));
      break;

    case DS_CMD_707_PKT_EPZID_SOCM:
      if(NULL == ds_cmd_ptr->cmd_payload_ptr)
      {
        DATA_ERR_FATAL("cmd_payload_ptr is NULL");
        return;
      }

      socm_msg_id = ds707_epzid_parse_socm (
         ((ds707_epzid_socm_msg_type *)ds_cmd_ptr->cmd_payload_ptr)->cntl,
         ((ds707_epzid_socm_msg_type *)ds_cmd_ptr->cmd_payload_ptr)->list_len);
      /*---------------------------------------------------------------------
      SOCM Message triggers the Hysteresis Engine as follows:
      1. SOCM EPZID Disable Message forces Hysteresis Engine to NULL state
      2. SOCM EPZID Enable Message sends an Enable/Disable event
         to the Hysteresis engine based on the ds707_epzid_max_list_len
      ---------------------------------------------------------------------*/
      switch(socm_msg_id)
      {
        case DS707_EPZID_CNTL_DISABLE:
#ifdef FEATURE_DS_EPZID_HYSTERESIS
          ds707_epzid_hysteresis_null_state();
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
          break;

        case DS707_EPZID_CNTL_ENABLE:
#ifdef FEATURE_DS_EPZID_HYSTERESIS
          /*-----------------------------------------------------------------
    	  Enable pzid hysteresis when max list length is 1 
          -----------------------------------------------------------------*/
          if (ds707_epzid_get_epzid_max_list_len() == 1)
          {
            ds707_epzid_hyst_socm_enable_hyst();
          }
          else 
          {
            ds707_epzid_hyst_socm_disable_hyst();
          } 
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
          /*-----------------------------------------------------------------
          Register EPZID change handlers with mccsyobj_system 
          Commented out since this may not be needed. 
          epzidi_register();
          -----------------------------------------------------------------*/
          break;

        default:
	      break;
      }
      break;

    case DS_CMD_707_PKT_EPZID_DELAY_TIMER_EXPIRED:
      epzidi_check_for_new_epzid();
      break;

#ifdef FEATURE_DS_EPZID_HYSTERESIS
	     
    case DS_CMD_707_PKT_EPZID_HYS_DATA_READY:
    case DS_CMD_707_PKT_EPZID_HYS_SDB_DATA:
    case DS_CMD_707_PKT_EPZID_HYSTERESIS_TIMER_EXPIRED:
         
      /*-----------------------------------------------------------------------
      Simplified JCDMA: 
      4.2.6 Disable PZID/SID/NID reconnection [D1-No.62] 
      -----------------------------------------------------------------------*/
     if(!ds707_roaming_is_curr_mode_jcdma() &&
        !ds707_roaming_is_simplified_jcdma_mode())
     {
        reorig_flag = ds707_epzid_hysteresis_process_cmd(ds_cmd_ptr);
        /*Perform re-origination only if FEATURE_JCDMA_2 is not used.*/
        if(reorig_flag)
        {
          DATA_IS707_MSG0(MSG_LEGACY_MED, "Re-orig after processing EPZID HT expiry");
          epzidi_reorig_hdlr();
        }
      }
      break;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

   case DS_CMD_707_EXIT_TC:
     /*-----------------------------------------------------------------
     Only perform re-origination through exit_tc_hdlr if JCDMA_2
     is not enabled.
     -----------------------------------------------------------------*/
     if(!ds707_roaming_is_curr_mode_jcdma() &&
        !ds707_roaming_is_simplified_jcdma_mode())
     {
       ds707_epzid_exit_tc_hdlr(ds_cmd_ptr);
     }
     break;

#ifdef FEATURE_IS707B_ADDENDUM
    case DS_CMD_707_PKT_MGR_SET_HAT_TIMER:
      ds707_epzid_set_hat_hdlr(ds_cmd_ptr);
      break;	
#endif /* FEATURE_IS707B_ADDENDUM*/

    default:
      break;
  }
} /* ds707_epzid_process_cmd() */

#endif /* FEATURE_DATA_IS707 */
