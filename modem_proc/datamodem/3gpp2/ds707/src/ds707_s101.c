/** 
  @file ds707_s101.c
  @brief 
   Handles Optimized Handoff Implementation.
   Contains S101 State Machine
*/

/*===========================================================================

                            D S 7 0 7 _ S101
GENERAL DESCRIPTION
  This file implements the S101 State Machine and the API definitions that
  trigger the state transition

EXTERNALIZED FUNCTIONS 
 
INTERNAL FUNCTIONS  


 Copyright (c) 2011 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_s101.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/14   sc      Fixed array out of bounds in ds707_s101_transition_state 
12/23/11   sb      Fixed compiler warnings
11/29/11   Ack     Changed DATA_IS707_MSGX to MSG_SPRINTF_X
08/22/11   Ack     Created module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include "data_msg.h"
#include "ds707_sec_pkt_mgr_util.h"
#include "ds707_pdn_context.h"
#include "hdrpac.h"
#include "ds707_pkt_mgr.h"
#include "ps_sys_conf.h"
#include "ps_sys_event.h"
#include "ps_sys_ioctl.h"
#include "ds707_devmgr.h"
#include "hdrrlplog.h"
#include "ds707_cback.h"
#include "ds_dsd_ext_i.h"
#include "ds3gmgr.h"
#include "cm.h"
#include "ds707_pkt_mgr_hdlr.h"

#ifdef FEATURE_DATA_OPTHO
#include "ds707_s101.h"
#include "ds707_s101_vctm.h"
#include "ds707_s101_dctm.h"
#include "ds707_s101_defs.h"

/*===========================================================================
                          LOCAL  DATA DECLARATIONS
===========================================================================*/
/** Represents the different Tunnel Modes that HDR stack 
    broadcasts
*/
typedef enum
{
  DS707_S101_TUNNEL_MIN           = -1,
  DS707_S101_TUNNEL_MODE_DISABLED =  0,
  DS707_S101_TUNNEL_MODE_ENABLED  =  1,
  DS707_S101_TUNNEL_ACTIVEHO_MODE =  2,
  DS707_S101_TUNNEL_MODE_NO_SRV   =  3, /* In LTE coverage but no service */
  DS707_S101_TUNNEL_MAX
} ds707_s101_tunnel_mode_e;

/** Represents the Prereg HO State Machine */
typedef enum
{
  DS707_S101_HO_SM_NULL = 0,
  DS707_S101_HO_SM_ACTIVE_HO,
  DS707_S101_HO_SM_IRMI_SENT,
  DS707_S101_HO_SM_PHYSLINK_BRINGUP_REQUESTED,
  DS707_S101_H0_SM_MAX
} ds707_s101_prereg_ho_sm_e;

/** Structure to store the PPP, VSNCP state - used to report
 *  prereg status changes to HDR */
typedef struct
{
  boolean ppp_up;     /**<LCP,AUTH is up/down */
  boolean vsncp_up;   /**<VSNCP is up/down */
}ds707_s101_prereg_status;

/** S101 State Machine structure
*/
typedef struct 
{
  ds707_s101_tunnel_mode_e tunnel_mode_val;  /**<Tunnel Mode              */
  boolean prereg_allowed;                    /**<Prereg State             */
  boolean lcp_throttled;                     /**<LCP Throttle State       */
  boolean virtual_connection_throttled;      /**<Virt Conn Throttle State */
  boolean sys_mode_lte;                      /**<UE camped on LTE or not  */
  boolean ehrpd_mode;                        /**<eHRPD Mode               */
  boolean hdr_sess_open;                     /**<HDR Session State        */
  boolean ho_to_L_in_prog;                   
    /**<To identify if Handoff to LTE is in progress */
  ds707_s101_prereg_status last_reported_prereg_info; 
    /**<Stores last reported PPP, VSNCP state */
  ds707_s101_sm_e s101_sm_state;             /**<S101 SM State            */
} ds707_s101_sm_s;

/** S101 Prereg HO SM Structure */
typedef struct
{
  ds707_s101_prereg_ho_sm_e s101_prereg_ho_sm_state;  /**<S101 Prereg HO SM State */
} ds707_s101_prereg_ho_sm_s;

/** Static variable to store S101 State Machine info
*/
static ds707_s101_sm_s ds707_s101_sm;

/** Static variable to store S101 Prereg H0 State Machine info
*/
static ds707_s101_prereg_ho_sm_s ds707_s101_prereg_ho_sm;

/** Static variable to indicate if HDR is doing active HO
*/
static boolean ds707_s101_actv_ho_in_prog;

/*---------------------------------------------------------------------------
    Critical section 
---------------------------------------------------------------------------*/
static rex_crit_sect_type ds707_s101_crit_sect;

/*---------------------------------------------------------------------------
  Used mainly for msg printing
  These need to be in sync with the ds707_s101_sm_e enum
---------------------------------------------------------------------------*/
static char *s101_sm_names[DS707_S101_SM_MAX] =
{
  "Not In Tunnel",
  "Tunnel Call Allowed",
  "Tunnel Call Disallowed"
};

/* These need to be in sync with ds707_s101_prereg_ho_sm_e enum */
static char *s101_ho_sm_names[DS707_S101_H0_SM_MAX] =
{
  "NULL",
  "Active HO",
  "IRMI SENT",
  "PhysLink BringUp Requested"
};

/*===========================================================================
                          FORWARD DECLARATION
===========================================================================*/

static void ds707_s101_tunnel_mode_chg_action
(
  ds707_s101_tunnel_mode_e prev_tunnel_mode_val,
  ds707_s101_tunnel_mode_e curr_tunnel_mode_val
);

static void ds707_s101_ho_to_l_chg_info
(
  boolean ho_to_l_in_prog
);

/*===========================================================================
                          LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION DS707_S101_SYS_CHG_NOTIFICATION
===========================================================================*/
/**
  Sends PS_SYS_EHRPD_S101_STATUS_CHG_EV

  @param tunnel_call_allowed  [in]Tunnel Allowed: TRUE/FALSE

  @return 
  None
	
  @dependencies 
  None
*/
static void ds707_s101_sys_chg_notification
(
  boolean tunnel_call_allowed,
  boolean hdr_actv_ho_in_prog
)
{
  ps_sys_3gpp2_s101_status_change_type status_chg_info;
  int16 ps_errno;
  ds3geventmgr_filter_type           eventmgr_input;

  DATA_IS707_MSG_SPRINTF_3(MSG_LEGACY_HIGH, 
                "S101: S101 Sys Chg: Tunnel Call Allowed = %s, "
                "LCP Throttled: %s, Virt Conn Throttled: %s",
                ((tunnel_call_allowed)? "TRUE": "FALSE"),
                ((ds707_s101_sm.lcp_throttled)? "TRUE": "FALSE"),
                ((ds707_s101_sm.virtual_connection_throttled)? "TRUE": "FALSE"));
  DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "hdr_actv_ho_in_prog: %s",
                           ((hdr_actv_ho_in_prog)? "TRUE": "FALSE"));


  /* If UE moved from 1X->LTE or if UE powered up on LTE, the
     last_good_sys will be set to NON_EPC. Since we are enabling tunnel
     call while in LTE, set the last_good_sys to EPC so prereg can be
     performed */
  if (TRUE == tunnel_call_allowed && 
      DS707_SYS_INFO_EPC_SYSTEM != ds707_get_last_good_sys()) 
  {
    ds707_set_last_good_sys(DS707_SYS_INFO_EPC_SYSTEM);
  }

  status_chg_info.tunnel_call_allowed = tunnel_call_allowed;
  status_chg_info.lcp_throttled = ds707_s101_sm.lcp_throttled;
  status_chg_info.virtual_connection_throttled = 
                   ds707_s101_sm.virtual_connection_throttled;
  status_chg_info.ehrpd_active_handoff_in_prog = hdr_actv_ho_in_prog;

  /** Set the PS conf so the register clients get notified about
   *  the conf change */
  if (DSS_SUCCESS != ps_sys_conf_set(PS_SYS_TECH_3GPP2,
                                     PS_SYS_CONF_3GPP2_S101_STATUS_CHANGE,
                                     (void *)&status_chg_info,
                                     &ps_errno))
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "S101: Status Chg conf set err: %d", ps_errno);
  }

  memset( (void*)&eventmgr_input, 
          0, 
          sizeof(ds3geventmgr_filter_type ));  
  eventmgr_input.tech = PS_SYS_TECH_3GPP2;
  eventmgr_input.ps_subs_id = PS_SYS_PRIMARY_SUBS;
  if( TRUE != ds3geventmgr_set_event_info(DS3GEVENTMGR_S101_STATUS_CHG_EV,
                              (void *)&status_chg_info,
                              &eventmgr_input))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "S101: Status Chg conf set error to ds3geventmgr");  
  }

  return;
} /* ds707_s101_sys_chg_notification */

/*===========================================================================
FUNCTION DS707_S101_CONV_TUNNEL_MODE_VAL
===========================================================================*/
/**
  Util function to convert tunnel mode val from uint8 to
  ds707_s101_tunnel_mode_e

  @param tun_mode             [in]  Tunnel Mode Value in uint8

  @return 
  tun_mode                    [out] Tunnel Mode Value in
                                    ds707_s101_tunnel_mode_e
 	
  @dependencies 
  None
*/
static ds707_s101_tunnel_mode_e ds707_s101_conv_tunnel_mode_val
(
  uint8 tun_mode
)
{
  ds707_s101_tunnel_mode_e tunnel_mode_val = DS707_S101_TUNNEL_MIN;
  switch (tun_mode)
  {
    case 0:
      tunnel_mode_val = DS707_S101_TUNNEL_MODE_DISABLED;
      break;

    case 1:
      tunnel_mode_val = DS707_S101_TUNNEL_MODE_ENABLED;
      break;

    case 2:
      tunnel_mode_val = DS707_S101_TUNNEL_ACTIVEHO_MODE;
      break;

    case 3:
      tunnel_mode_val = DS707_S101_TUNNEL_MODE_NO_SRV;
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "S101: Invalid Tunnel Mode Val change: %d",
                    tun_mode);
      return tunnel_mode_val;
      break;
  }
    return tunnel_mode_val;
} /* ds707_s101_conv_tunnel_mode_val */

/*===========================================================================
FUNCTION DS707_S101_OK_TO_ALLOW_TUNNEL_CALL
===========================================================================*/
/**
  Util function that determines if S101 SM should transition to
  DS707_S101_SM_TUNNEL_CALL_ALLOWED by checking tunnel mode value,
  prereg value, LCP throttle info, virtual connection throttle info, 
  lte sys mode, ehrpd mode and hdr session state

  @param
  None

  @return 
  TRUE  -- Ok to allow tunnel call \n
  FALSE -- Otherwise \n
	
  @dependencies 
  None
*/
static boolean ds707_s101_ok_to_allow_tunnel_call
(
  void
)
{
  if (ds707_s101_sm.tunnel_mode_val == DS707_S101_TUNNEL_MODE_ENABLED &&
      ds707_s101_sm.prereg_allowed == TRUE &&
      ds707_s101_sm.lcp_throttled == FALSE &&
      ds707_s101_sm.virtual_connection_throttled == FALSE &&
      ds707_s101_sm.sys_mode_lte == TRUE &&
      ds707_s101_sm.ehrpd_mode == TRUE &&
      ds707_s101_sm.hdr_sess_open == TRUE &&
      ds707_s101_sm.ho_to_L_in_prog == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: All conditions met. Ok to allow Tunnel Call");
    return TRUE;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Not all conditions met to allow Tunnel Call");
    DATA_IS707_MSG_SPRINTF_3(MSG_LEGACY_HIGH, 
                             "Tunnel Mode:%d, Prereg:%s, LCP Throttled:%s",
                             ds707_s101_sm.tunnel_mode_val,
                             ((ds707_s101_sm.prereg_allowed)? "TRUE":"FALSE"),
                             ((ds707_s101_sm.lcp_throttled)? "TRUE":"FALSE")
                             );
    DATA_IS707_MSG_SPRINTF_3(MSG_LEGACY_HIGH, 
                             "Virt Conn Throttled:%s, "
                             "eHRPD Mode:%s, HDR Session State:%s",
                             ((ds707_s101_sm.virtual_connection_throttled)?
                               "TRUE":"FALSE"),
                             ((ds707_s101_sm.ehrpd_mode)? "TRUE":"FALSE"),
                             ((ds707_s101_sm.hdr_sess_open)? "OPEN":"CLOSED")
                             );
    DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                             "Sys_Mode_LTE:%s, ho_to_L_in_prog:%s",
                             ((ds707_s101_sm.sys_mode_lte)? "TRUE":"FALSE"),
                             ((ds707_s101_sm.ho_to_L_in_prog)? "TRUE":"FALSE")
                             );
    return FALSE;
  }
} /* ds707_s101_ok_to_allow_tunnel_call */

/**
 *  S101 Transition Table 
 *  |===========================================================================================================================================================================================|
 *  |           | E0:Sys Mode  | E1:Sys Mode  | E2:eHRPD Mode | E3:eHRPD Mode | E4:Prereg   | E5:Prereg | E6:LCP    | E7:LCP      | E8: Tun Mode | E9: Tun Mode | E10:Virt Conn | E11:Virt Conn |
 *  |           |  chg:!LTE    |  chg: LTE    |      Set      |     Unset     |    TRUE     |   FALSE   | Throttled | Unthrottled |      0/2     |      1       | Throttled     | Unthrottled   |
 *  |-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 *  |S0: None   |      --	     |      S2	    |      --	      |       --	    |    Error	  |     --	  |     --	  |     --	    |       --	   |      --      |      --       |     --        | 
 *  |-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 *  |S1: Tunnel |      S0	     |      --	    |      --	      |       S2	    |     --	    |     S2	  |     S2    |  	Error	    |       S2     |	    --      |      S2       |    Error      |
 *  |Allowed    |              |              |               |               |             |           |           |             |              |              |               |               |
 *  |-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 *  |S2: Tunnel |      S0	     |      --	    | If tun cond.  |       --      |If tun cond. |     --    |     --    |If tun cond. |       --     |If tun cond.  |      --       |If tun cond.   |
 *  |Disallowed |              |              | met: S1       |               |met: S1      |           |           |met: S1      |              |met: S1       |               |met: S1        |
 *  |===========================================================================================================================================================================================|
 * 
 *  Tun Cond. Met => Tunnel mode Value   == 1
 *                   Prereg              == TRUE
 *                   eHRPD Mode          == TRUE
 *                   LCP Throttled       == FALSE
 *                   Virt Conn Throttled == FALSE
 */
/*===========================================================================
FUNCTION DS707_S101_TRANSITION_STATE
===========================================================================*/
/**
  Deals with S101 State transitions

  @param new_state  [in] New S101 state

  @return 
  None
	
  @dependencies 
  None
*/
static void ds707_s101_transition_state
(
  ds707_s101_sm_e new_state
)
{
  ds707_s101_sm_e          old_state  = ds707_s101_sm.s101_sm_state;
  ds707_s101_tunnel_mode_e tun_mode_val = DS707_S101_TUNNEL_MIN;
  ds707_s101_tunnel_mode_e prev_tunnel_mode_val = ds707_s101_sm.tunnel_mode_val;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ds707_s101_sm.s101_sm_state = new_state;

  DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                  "S101: S101 SM transitioning from: %d state to: %d state",
                  old_state, 
                  new_state);

  if (old_state == ds707_s101_sm.s101_sm_state)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "S101: Error: SM transitioning to same state: No-Op");
    return;
  }

  if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == old_state)
  {
    /* i.e S101 SM is moving out of tunnel allowed state */
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Notify EPC that tunnel call is no longer allowed");
    ds707_s101_sys_chg_notification(FALSE,
                                    ds707_s101_is_hdr_actv_ho_in_prog());
  }

  switch (new_state)
  {
    case DS707_S101_SM_NON_TUNNEL:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Transitioned to Not_In_Tun state "
                      "- Setting ho_to_L_in_prog to FALSE;");
      ds707_s101_sm.ho_to_L_in_prog = FALSE;
      break;
    }

    case DS707_S101_SM_TUNNEL_CALL_ALLOWED:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Enable Phys link flow:Mask: "
                      "DS_FLOW_NO_SERVICE_MASK"
                      "S101: Notify EPC that tunnel call is allowed");
      ds707_phys_link_enable_flow(DS_FLOW_NO_SERVICE_MASK);   
      

      ds707_s101_sys_chg_notification(TRUE,
                                      ds707_s101_is_hdr_actv_ho_in_prog());    
      break;
    }

    case DS707_S101_SM_TUNNEL_CALL_DISALLOWED:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Disable Phys link flow:Mask: "
                      "DS_FLOW_NO_SERVICE_MASK");
      ds707_phys_link_disable_flow(DS_FLOW_NO_SERVICE_MASK);   

      if (DS707_S101_SM_NON_TUNNEL == old_state)
      {
            
        /** Reset VSNCP cache. When prereg occurs and the first VSNCP
         *  comes up, we'll report to HDR */
        ds707_s101_sm.last_reported_prereg_info.vsncp_up = FALSE;

        /** If transitioning from Non Tunnel->Disallowed, Query Tunnel
         *  Mode, Prereg val, LCP, VC throttle info. eHRPD mode cannot
         *  be queried here since we are processing during sys change to
         *  LTE and eHRPD mode of operation will return true only after
         *  we move to tunnel allowed state. */

        /* OPTHO_ToDo: Add LCP throttle and Virt Conn throttle query*/

        ds707_s101_sm.prereg_allowed = hdrpac_is_prereg_allowed();

        tun_mode_val = ds707_s101_conv_tunnel_mode_val(
                                      hdrpac_get_current_tunnel_mode());
        if (DS707_S101_TUNNEL_MIN != tun_mode_val && 
            prev_tunnel_mode_val != tun_mode_val
           )
        {
          ds707_s101_sm.tunnel_mode_val = tun_mode_val;
          ds707_s101_tunnel_mode_chg_action(prev_tunnel_mode_val, 
                                            ds707_s101_sm.tunnel_mode_val);
        }

        if (ds707_pdnctx_are_all_pdns_down() && 
            (DEVMGR_LINGER_STATE == ds707_devmgr_get_state() ||
             DEVMGR_CLOSED_STATE == ds707_devmgr_get_state()))
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                          "S101: All PDNs Down and Devmgr in Linger/Closed State."
                          "Set ho_to_L_in_prog to FALSE");
          ds707_s101_sm.ho_to_L_in_prog = FALSE;
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                          "S101: Not all PDNs Down or Devmgr not in Linger/Closed State."
                          "Set ho_to_L_in_prog to TRUE");
          /** When all PDNs go down and/or devmgr moves to closed
           *  state, S101 will be notified and ho_to_L_in_prog will be set
           *  to FALSE */
          ds707_s101_sm.ho_to_L_in_prog = TRUE;
        }
  
        /** Check if it is ok to move to tunnel allowed state */
        if (ds707_s101_ok_to_allow_tunnel_call())
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                          "S101: All conditions met. Moving from tunnel call disallowed"
                          "to allowed state");
          ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_ALLOWED);
        }  
      }
      break;
    }

  default:
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "S101: Invalid Transition");
    break;

  } /* switch (new_state) */

  return;
} /* ds707_s101_transition_state */

/**
 *  S101 Prereg Handoff Transition Table 
 *  |=============================================================================|
 *  |             | E0:Prereg HO |   E1: IRMI   |    E2: IRMI   | E3: Call        |
 *  |             | Complete CMD |    SUCCESS   |     FAILURE   | Connected/Ended |
 *  |-----------------------------------------------------------------------------|
 *  |S0: Null     |   C0 => S1 / |      --	    |      --	      |       --	      | 
 *  |             |  !C0 => S2   |              |               |                 |
 *  |-----------------------------------------------------------------------------|
 *  |S1: IRMI     |      --	     |      S0	    |    C1 => S2 /	|       --	      |
 *  |SENT         |              |              |   !C1 => S0   |                 |
 *  |-----------------------------------------------------------------------------|
 *  |S2: PHYSLINK |      --	     |      --	    |       --      |       S0        |
 *  |REQUESTED    |              |              |               |                 |
 *  |=============================================================================|
 * 
 *  Conditions:
 *  C0: IDLE Handoff
 *  C1: S101 SM = Non Tunnel State && eHRPD Mode Set (i.e eHRPD Direct Mode)
 */
/*===========================================================================
FUNCTION DS707_S101_PREREG_HO_SM_TRANSITION_STATE
===========================================================================*/
/**
  Deals with S101 Prereg Handoff State transitions

  @param new_state  [in] New S101 Prereg HO SM state

  @return 
  None
	
  @dependencies 
  None
*/
static void ds707_s101_prereg_ho_sm_transition_state
(
  ds707_s101_prereg_ho_sm_e new_state
)
{
  ds707_s101_prereg_ho_sm_e      old_state  = ds707_s101_prereg_ho_sm.s101_prereg_ho_sm_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (old_state == new_state)
  {
    DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_ERROR, 
                    "S101: Error: Prereg HO SM transitioning to same state: %s",
                    s101_ho_sm_names[old_state]);
    return;
  }

  ds707_s101_prereg_ho_sm.s101_prereg_ho_sm_state = new_state;

  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                "S101: S101 prereg H0 SM transitioning from: %s state to: %s state",
                s101_ho_sm_names[old_state], 
                s101_ho_sm_names[ds707_s101_prereg_ho_sm.s101_prereg_ho_sm_state]);

  /** For now there is no transition associated processing */

} /* ds707_s101_prereg_ho_sm_transition_state */

/*===========================================================================
FUNCTION DS707_S101_QUERY_PREREG_H0_SM_STATE
===========================================================================*/
/**
  Returns the current S101 Prereg HO State Machine state

  @param
  None

  @return 
  S101 Prereg HO tate Machine State Enum
 	
  @dependencies 
  None
*/
static ds707_s101_prereg_ho_sm_e ds707_s101_query_prereg_ho_sm_state
(
  void
)
{
  DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "S101: Prereg HO SM in %s state", 
                  s101_ho_sm_names[ds707_s101_prereg_ho_sm.s101_prereg_ho_sm_state]);
  return ds707_s101_prereg_ho_sm.s101_prereg_ho_sm_state;

} /* ds707_s101_query_prereg_ho_sm_state */

/*===========================================================================
FUNCTION DS707_S101_SEND_PREREG_HANDOFF_RESPONSE
===========================================================================*/
/**
  Takes care of the prereg handoff completion procedure

  @param
  ho_success     [in] prereg handoff succeeded/failed

  @return 
  None
 	
  @dependencies 
  None
*/
static void ds707_s101_send_prereg_handoff_response
( 
  boolean ho_success
)
{
  int16 ps_errno;
  ps_sys_event_3gpp2_ehrpd_prereg_handoff_complete_rsp_type resp;
  ds3geventmgr_filter_type           eventmgr_input;

  resp.prereg_handoff_succeeded = ho_success;

  /** Do not check if prereg ho sm is not in Null state. Some
   *  times MH might have got a complete prereg ho cmd from EPC
   *  incorrectly and MH has to send a ho failure resp. */

  if (ho_success)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Sending Prereg HO Success to EPC");

  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Sending Prereg HO Failure to EPC");   
  }
    
  /** Send the response to EPC via event indication */
  if (DSS_SUCCESS != ps_sys_event_ind(PS_SYS_TECH_3GPP2,
                                      PS_SYS_EVENT_3GPP2_EHRPD_PREREG_HANDOFF_COMPLETE_RSP,
                                      (void *)&resp,
                                      &ps_errno))
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "S101: Prereg HO resp err: %d", ps_errno);

    /** No way to recover since EPC and MH will go out of sync if
     *  we return here */
    ASSERT(FALSE);
    return;
  }

  memset( (void*)&eventmgr_input, 
          0, 
          sizeof(ds3geventmgr_filter_type ));
  eventmgr_input.tech = PS_SYS_TECH_3GPP2;
  eventmgr_input.ps_subs_id = PS_SYS_PRIMARY_SUBS;
  if( TRUE != ds3geventmgr_set_event_info(DS3GEVENTMGR_EHRPD_PREREG_HANDOFF_COMPLETE_RESP_EV,
                              (void *)&resp,
                              &eventmgr_input))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Failed to generate DS3GMGR PREREG HO COMPLETE event");  
    /** No way to recover since EPC and MH will go out of sync if
     *  we return here */
    ASSERT(FALSE);
    return;
  }

  /** If prereg HO SM is not in Null state, transition it to
   *  NULL state */
    
  if (DS707_S101_HO_SM_NULL != ds707_s101_query_prereg_ho_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: sent prereg ho resp. Transitioning Prereg "
                    "HO sm to NULL");
    ds707_s101_prereg_ho_sm_transition_state(DS707_S101_HO_SM_NULL);
  }
  
} /* ds707_s101_send_prereg_handoff_success */

/*===========================================================================
FUNCTION DS707_S101_BRINGUP_PHYS_LINK
===========================================================================*/
/**
  Utility function to bringup the primary phys link

  @param
  None

  @return 
  None
 	
  @dependencies 
  None
*/
static void ds707_s101_bringup_phys_link
( 
  void
)
{
  ps_phys_link_type          *phys_link_ptr;
  int                        ret_val;
  int16                      err_num; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /** For S101 case, we should bring up phys link only if sys
   *  is on eHRPD and S101 SM is in Non Tunnel State    */

  if (!ds707_pkt_is_ehrpd_mode_of_operation() ||
      DS707_S101_SM_NON_TUNNEL != ds707_s101_query_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "S101: System not in eHRPD and/or "
                    "S101 SM != Non Tunnel State. "
                    "Not bringing up phys link");
    ds707_s101_send_prereg_handoff_response(FALSE);
    return;
  }



  /** Since S101 SM in Non Tunnel State, the call will be brought
   *  up via CM */
  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  ret_val = ps_phys_link_up_cmd( phys_link_ptr, &err_num, NULL);
  if (ret_val == 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "S101: Phys Link already up. Posting HO success to EPC");
    ds707_s101_send_prereg_handoff_response(TRUE);
    return;
  }
  else if ((ret_val < 0) && (err_num != DS_EWOULDBLOCK))
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "S101: phys link bringup failed.err_num:%d,"
                    "Posting HO failure to EPC", err_num);

    ds707_s101_send_prereg_handoff_response(FALSE);
    return;
  }

  /** Move the S101 Prereg HO SM to PHYSLINK_BRINGUP_REQUESTED
   *  state. It'll be moved to NULL state once the call is
   *  connected/ended via CM */
   ds707_s101_prereg_ho_sm_transition_state(DS707_S101_HO_SM_PHYSLINK_BRINGUP_REQUESTED);
} /* ds707_s101_bringup_phys_link */

/*===========================================================================
FUNCTION DS707_S101_COMPLETE_PREREG_HANDOFF
===========================================================================*/
/**
  Takes care of the prereg handoff completion procedure

  @param
  prereg_handoff_info     [in] contains prereg related handoff info

  @return 
  None
 	
  @dependencies 
  None
*/
static void ds707_s101_complete_prereg_handoff
(
  ps_sys_ioctl_3gpp2_prereg_handoff_complete_cmd_type prereg_handoff_info
)
{
  ds707_pdn_context_cb_type  *pdncntx_cb_ptr            = NULL;
  uint8                      curr_pdn_index            = 0;
  //acl_handoff_ctxt_type      *handover_ip_config_handle = NULL;
  boolean                    bringup_phys_link         = FALSE;
  ds707_s101_prereg_ho_sm_e  s101_prereg_ho_sm_state   = DS707_S101_HO_SM_NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                  "S101: Processing S101 Prereg handoff complete cmd");

  /** While in Tunnel mode, we log RLP packets with time stamp
   *  as 65535. Flush the log collected till now, so we don't
   *  end up with a log collected partially in tunnel and
   *  partially in direct mode.
   *  
   *  When RLP is not inited (no TCA) when moving from tunnel
   *  to direct mode, the log packets are flushed when RLP is
   *  closed. However in active HO case, we acquire TCA in
   *  tunnel and use it in direct mode. Hence it's necessary to
   *  flush the log packets here */
#ifdef FEATURE_HDRRLP_LOGGING
   hdrrlplog_submit_all_logs();
#endif 

  /** Ensure that S101 SM is still in Non Tunnel State and prereg
   *  ho sm is in NULL/Active HO state */
  s101_prereg_ho_sm_state = ds707_s101_query_prereg_ho_sm_state();
  
  if (DS707_S101_SM_NON_TUNNEL != ds707_s101_query_sm_state() ||
      (DS707_S101_HO_SM_NULL != s101_prereg_ho_sm_state &&
       DS707_S101_HO_SM_ACTIVE_HO != s101_prereg_ho_sm_state)
      )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "S101: Prereg handoff complete cmd while S101 SM "
                    "in tunnel state or S101 prereg ho SM not in "
                    "null/activeho state. Posting prereg HO failure to "
                    "EPC");
    ds707_s101_send_prereg_handoff_response(FALSE);
  }

  /* OPTHO_ToDo: Revisit this. See if you need to move this into 
     ds707_s101_send_prereg_handoff_response(TRUE) */
  /** Update Handoff class and Restart PDN inactivity timer if the
   *  PDN is in use */
  for (curr_pdn_index = 0; 
       curr_pdn_index < DS707_MAX_PDN_INSTANCES; 
       curr_pdn_index++)
  {
    pdncntx_cb_ptr = ds707_get_pdncntx_cb_from_instance(curr_pdn_index);
    if (DS707_PDNCNTX_CB_IS_VALID(pdncntx_cb_ptr))
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                      "S101: In Use PDN_CB_PTR:0x%X", pdncntx_cb_ptr);

      /* OPTHO_ToDo: The Handoff Policy stored in PDN CB is deleted immediately after   
        VSNCP configure. So the handover_ip_config_handle is always going to 
        be null - Revisit this if needed */
      #if 0
      handover_ip_config_handle =      
        (acl_handoff_ctxt_type*) pdncntx_cb_ptr->handover_ip_config_handle;

      if ((NULL != handover_ip_config_handle) &&   
          (PS_IFACE_HANDOFF_CLASS_EPC_PREREG == 
           handover_ip_config_handle->handoff_class)
         )
      {
        DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                        "S101: For PDN_CB_PTR:0x%X, updating handoffclass from %d"
                        "to PS_IFACE_HANDOFF_CLASS_EPC",
                        pdncntx_cb_ptr, 
                        handover_ip_config_handle->handoff_class);
        handover_ip_config_handle->handoff_class = 
          PS_IFACE_HANDOFF_CLASS_EPC;

        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "S101: Restarting PDN inactivity timer");
      }   
      #endif 
      ds707_pdncntx_inactivity_timer_start(pdncntx_cb_ptr);
    }
  }/* for */
 
  if (prereg_handoff_info.all_pdn_ctxts_prereg)
  {

    
    if (DS707_S101_HO_SM_NULL == s101_prereg_ho_sm_state)
    {


      if (hdrpac_send_irmi())
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101:All LTE PDN ctxts are preregistered,"
                      " HO SM in NULL State -"
                      "HDR notified to send IRMI");


        /** Move the S101 Prereg HO SM to IRMI_SENT state. It'll be
         *  moved to NULL state once we get the IRMI success response or
         *  when the call is connected/ended via CM  */
        ds707_s101_prereg_ho_sm_transition_state(DS707_S101_HO_SM_IRMI_SENT);
        return;
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101:All LTE PDN ctxts are preregistered,"
                      " HO SM in NULL State -"
                      "Idle Handoff. IRMI not allowed");

        bringup_phys_link = TRUE;
      }
    }
    else if (DS707_S101_HO_SM_ACTIVE_HO == s101_prereg_ho_sm_state)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: HO SM in Active HO state - Active Handoff");
      bringup_phys_link = TRUE;
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "S101: HO SM in incorrect state");

      ds707_s101_send_prereg_handoff_response(FALSE);
      return;
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Some LTE PDN ctxts are preregistered");
    bringup_phys_link = TRUE;
  }

  if (bringup_phys_link)
  {
    ds707_s101_bringup_phys_link();
    return;
  }
} /* ds707_s101_complete_prereg_handoff */


/*===========================================================================
FUNCTION DS707_S101_EVENT_CBACK
===========================================================================*/
/**
  This cback is triggered whenever phys link changes
 
  @param event            [in] Event that got triggered
         event_info_ptr   [in] Details related to the event
 
  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
  DATA_IS707_MSG3(MSG_LEGACY_MED, "S101: rcvd phys link event: %d, "
                                  "going dormant: %d, ho_to_L_in_prog: %d ",
                                  event, event_info_ptr->going_dormant, 
                                  ds707_s101_sm.ho_to_L_in_prog);

  switch(event)
  {
    case DS707_CBACK_PHYS_LINK_DOWN_EV:
    case DS707_CBACK_TUN_PHYS_LINK_DOWN_EV:
    {
      if (event_info_ptr->going_dormant == FALSE && 
          ds707_s101_sm.ho_to_L_in_prog == TRUE)
      {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, "S101: Phy link Down NULL");
          ds707_s101_ho_to_l_chg_info(FALSE);
      }
      break;
    }

    default:
      break;
  }
} /* ds707_s101_event_cback */

/*===========================================================================
                         EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION DS707_S101_SM_INIT
===========================================================================*/
/**
  S101 State Machine init on power up

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_sm_init
(
  void
)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "S101: DS707 S101 SM power up init");

  /** Init the S101 SM variable with default values */
  ds707_s101_sm.tunnel_mode_val              = DS707_S101_TUNNEL_MODE_DISABLED;
  ds707_s101_sm.prereg_allowed               = FALSE;
  ds707_s101_sm.lcp_throttled                = FALSE;
  ds707_s101_sm.virtual_connection_throttled = FALSE;
  ds707_s101_sm.sys_mode_lte                 = FALSE;
  ds707_s101_sm.ehrpd_mode                   = FALSE;
  ds707_s101_sm.hdr_sess_open                = FALSE;
  ds707_s101_sm.ho_to_L_in_prog              = FALSE;
  ds707_s101_sm.last_reported_prereg_info.ppp_up    = FALSE;
  ds707_s101_sm.last_reported_prereg_info.vsncp_up  = FALSE;
  ds707_s101_sm.s101_sm_state                = DS707_S101_SM_NON_TUNNEL;

  /** Init the S101 Prereg H0 SM variable with default values */
  ds707_s101_prereg_ho_sm.s101_prereg_ho_sm_state     = DS707_S101_HO_SM_NULL;

  /** Init actv_ho_in_prog flag */
  ds707_s101_actv_ho_in_prog = FALSE;

  /** Init Critical section  */
  rex_init_crit_sect(&ds707_s101_crit_sect);

  if (ds707_cback_register( DS707_CBACK_PHYS_LINK_DOWN_EV,
                            ds707_s101_event_cback
                           ) == FALSE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_FATAL,
                     "S101: Could not reg PHYS_LINK_DOWN_EV cback" );
  }

  if (ds707_cback_register( DS707_CBACK_TUN_PHYS_LINK_DOWN_EV, 
                            ds707_s101_event_cback) == FALSE)
  {
    DATA_IS707_MSG0( MSG_LEGACY_FATAL,
                     "S101: Could not reg TUN_PHYS_LINK_DOWN_EV cback" );
  }

  return;
} /* ds707_s101_sm_init */

/*===========================================================================
FUNCTION DS707_S101_SYS_CHG_INFO
===========================================================================*/
/**
  Invoked when a system change is detected

  @param new_mode        [in] New System Mode
  @param old_mode        [in] Old System Mode

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_sys_chg_info
(
  sys_sys_mode_e_type           new_mode,
  sys_sys_mode_e_type           old_mode
)
{
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                  "S101: Sys Mode Change: Old: %d, New: %d",
                  old_mode, new_mode);
  DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                "S101: S101 SM Curr State: %s",
                s101_sm_names[ds707_s101_sm.s101_sm_state]);

  if (new_mode == SYS_SYS_MODE_LTE)
  {
    ds707_s101_sm.sys_mode_lte = TRUE;

    if (DS707_S101_SM_NON_TUNNEL == ds707_s101_sm.s101_sm_state)
    {
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_DISALLOWED);
    }
    else if (DS707_S101_SM_TUNNEL_CALL_DISALLOWED == ds707_s101_sm.s101_sm_state &&
             ds707_s101_ok_to_allow_tunnel_call()
            )
    {
      /** The conditions necessary to allow tunnel call are met.
       *  Transition to tunnel allowed state */
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_ALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Already in Tunnel Allowed State. "
                      "Ignore Sys Mode Change to LTE");
    }
  }
  else if (new_mode == SYS_SYS_MODE_NO_SRV)
  {
    ds707_s101_sm.sys_mode_lte = FALSE;

    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_DISALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Sys Mode Change to No Srv. "
                      "S101 not in tunnel allowed state - "
                      "No-Op for Sys Mode change");
    }
    return;
  }
  else 
  {
    ds707_s101_sm.sys_mode_lte = FALSE;

    /** S101 SM will transition to Non Tunnel as soon as the
     *  system camps on a non-LTE RAT (e.g. G/W/1x/HRPD/eHRPD). So
     *  enabling the flow , clearing pdn throttle is independent of
     *  S101 moving to non-tunnel state.
     *  E.g: L->G->eHRPD. When sys moved to G, S101 moves to
     *  non-tunnel state, however enabling flow, clearing pdn
     *  throttle should be done only when system camps on eHRPD */
    if (new_mode == SYS_SYS_MODE_HDR || new_mode == SYS_SYS_MODE_CDMA) 
    { 
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Sys mode changed to 1x/HDR."
                      "Enable Phys link flow: Mask: DS_FLOW_NO_SERVICE_MASK");
      ds707_phys_link_enable_flow(DS_FLOW_NO_SERVICE_MASK);   
    }

    if (new_mode == SYS_SYS_MODE_HDR && 
        ds707_pkt_is_ehrpd_mode_of_operation())
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: UE moved to eHRPD direct Mode. "
                      "Clear PDNs throttled in tun mode (if any)");
      ds707_pdncntx_clear_pdn_throttled_in_tunnel_mode();
    }

    /** UE camped on some system other than LTE */
    if (DS707_S101_SM_NON_TUNNEL != ds707_s101_sm.s101_sm_state)
    {
      ds707_s101_transition_state(DS707_S101_SM_NON_TUNNEL);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Already in Not_In_Tun state. "
                      "Ignore Sys Mode Change to !LTE");
    }
  }
  return;
} /* ds707_s101_sys_chg_info */


/*===========================================================================
FUNCTION DS707_S101_EHRPD_MODE
===========================================================================*/
/**
  Invoked when eHRPD Mode is Set/Unset

  @param is_set          [in] eHRPD Mode: TRUE/FALSE

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_ehrpd_mode
(
  boolean is_set
)
{
  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
            "S101: eHRPD Mode Change: eHRPD Mode = %s, S101 SM Curr State: %s", 
             ((is_set)? "TRUE": "FALSE"), s101_sm_names[ds707_s101_sm.s101_sm_state]);


  ds707_s101_sm.ehrpd_mode = is_set;

  if (is_set)
  {
    if (ds707_pkt_mgr_get_current_sys_mode() == SYS_SYS_MODE_HDR)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: UE moved to eHRPD direct Mode. "
                      "Clear PDN throttle");
      ds707_pdncntx_clear_pdn_throttled_in_tunnel_mode();
    }

    if (DS707_S101_SM_TUNNEL_CALL_DISALLOWED == ds707_s101_sm.s101_sm_state &&
        ds707_s101_ok_to_allow_tunnel_call()
       )
    {
      /** The conditions necessary to allow tunnel call are met.
       *  Transition to tunnel allowed state */
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_ALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: eHRPD Mode Set. S101 not in tunnel disallowed "
                      "state and/or tunnel call allow conditions not "
                      "met - Ignoring eHRPD Mode change");
    }
  }
  else
  {
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_DISALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: eHRPD Mode Unset. S101 not in tunnel allowed "
                      "state - Ignoring eHRPD Mode change");
    }
  }
  return;
} /* ds707_s101_ehrpd_mode */

/*===========================================================================
FUNCTION DS707_S101_LCP_THROTTLED
===========================================================================*/
/**
  Invoked when system is Throttled due to LCP/AUTH failure
  or unthrottled when the LCP throttle timer expires

  @param is_throttled          [in] LCP Throttled: TRUE/FALSE

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_lcp_throttled
(
  boolean is_throttled
)
{
  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
             "S101: LCP Throttle Info Change: LCP Throttled = %s"
             "S101 SM Curr State: %s", 
             ((is_throttled)? "TRUE":"FALSE"),s101_sm_names[ds707_s101_sm.s101_sm_state]);


  ds707_s101_sm.lcp_throttled = is_throttled;

  if (is_throttled)
  {
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_DISALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: LCP Throttled. S101 not in tunnel allowed "
                      "state - Ignoring LCP throttle ind");
    }
  }
  else
  {
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "S101: Err: LCP Unthrottled in Tunnel Call Allowed state");
    }
    else if (DS707_S101_SM_TUNNEL_CALL_DISALLOWED == ds707_s101_sm.s101_sm_state &&
             ds707_s101_ok_to_allow_tunnel_call()
            )
    {
      /** The conditions necessary to allow tunnel call are met.
       *  Transition to tunnel allowed state */
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_ALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: LCP Unthrottled. S101 in Not_In_Tun state or "
                      "tunnel call allow conditions not met - "
                      "Ignoring LCP unthrottle ind");
    }
  }
  return;
} /* ds707_s101_lcp_throttled */

/*===========================================================================
FUNCTION DS707_S101_VIRT_CONN_THROTTLED
===========================================================================*/
/**
  Invoked when virtual connection is throttled due to tunnel phys link
  bringup failure or unthrottled when the Virtual Connection throttle
  timer expires

  @param is_throttled          [in] Virt Conn Throttled: TRUE/FALSE

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_virt_conn_throttled
(
  boolean is_throttled
)
{
  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                "S101: Virt Conn Throttle Info Change: "
                "Virt Conn Throttled = %s"
                 "S101 SM Curr State: %s", 
                ((is_throttled)? "TRUE":"FALSE"),
                s101_sm_names[ds707_s101_sm.s101_sm_state]);


  ds707_s101_sm.virtual_connection_throttled = is_throttled;

  if (is_throttled)
  {
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_DISALLOWED);
    }
    else
    {
       DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Virt Conn Throttled. S101 not in tunnel allowed "
                      "state - Ignoring Virt Conn throttle ind");
    }
  }
  else
  {
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "S101: Err: Virt Conn Unthrottled in Tunnel Call Allowed state");
    }
    else if (DS707_S101_SM_TUNNEL_CALL_DISALLOWED == ds707_s101_sm.s101_sm_state &&
             ds707_s101_ok_to_allow_tunnel_call()
            )
    {
      /** The conditions necessary to allow tunnel call are met.
       *  Transition to tunnel allowed state */
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_ALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Virt Conn Unthrottled. S101 in Not_In_Tun state or "
                      "tunnel call allow conditions not met - "
                      "Ignoring Virt Conn unthrottle ind");
    }
  }
  return;

} /* ds707_s101_virt_conn_throttled */

/*===========================================================================
FUNCTION DS707_S101_TUNNEL_MODE_CHG_ACTION
===========================================================================*/
/**
  Triggered when there is a valid tunnel mode transition

  @param prev_tunnel_mode_val             [in] Previous Tunnel Mode Value
  @param curr_tunnel_mode_val             [in] Current Tunnel Mode Value

  @return 
  None
 	
  @dependencies 
  None
*/
static void ds707_s101_tunnel_mode_chg_action
(
  ds707_s101_tunnel_mode_e prev_tunnel_mode_val,
  ds707_s101_tunnel_mode_e curr_tunnel_mode_val
)
{
  /** When tunnel mode transitions, we need to perform some
   *  action */
  switch (prev_tunnel_mode_val)
  {
    case DS707_S101_TUNNEL_MODE_DISABLED:
    {
      /** When transitioning from Tunnel Mode 0 to Tunnel Mode 1,
       *  notify HDR about Prereg Status */
      if (DS707_S101_TUNNEL_MODE_ENABLED  == curr_tunnel_mode_val)
      {
        /** Report PPP status */
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "S101: Reporting PPP prereg"
                        "and VSNCP prereg status to HDR");
    
        if (ds707_devmgr_up_or_lingering())
        {
          hdrpac_set_prereg_ppp_status(TRUE);
          ds707_s101_sm.last_reported_prereg_info.ppp_up = TRUE;
        }
        else
        {
          hdrpac_set_prereg_ppp_status(FALSE);
          ds707_s101_sm.last_reported_prereg_info.ppp_up = FALSE;
        }

        /** Report VSNCP status when transitioning from tunnel 
         *  mode 0 to 1 */

        hdrpac_set_prereg_vsncp_status
                   (ds707_s101_sm.last_reported_prereg_info.vsncp_up);
      }

      /** When transitioning from Tunnel Mode 0 to Tunnel Mode 1/2,
       *  reset the HO SM if it was in Active HO state. Ideally
       *  transition 0 -> 2 is not possible */
      if (DS707_S101_HO_SM_ACTIVE_HO == ds707_s101_query_prereg_ho_sm_state())
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                        "S101: Reset HO SM to NULL since tun mode val "
                        "changed 0 -> 1/2");
        ds707_s101_prereg_ho_sm_transition_state(DS707_S101_HO_SM_NULL);
      }

      break;
    }

    case DS707_S101_TUNNEL_MODE_ENABLED:
    case DS707_S101_TUNNEL_MODE_NO_SRV:
    {
      /** No Action to take  */
      break;
    }

    case DS707_S101_TUNNEL_ACTIVEHO_MODE:
    {
      /** When transitioning from Tunnel Mode 2 to Tunnel Mode 0, move
       *  the HO SM to Active HO state */
      if (DS707_S101_TUNNEL_MODE_DISABLED == curr_tunnel_mode_val)
      {
        if (DS707_S101_HO_SM_NULL == ds707_s101_query_prereg_ho_sm_state())   
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                          "S101: tun mode val changed 2 -> 0. "
                          "Move HO SM to Active HO state");
          ds707_s101_prereg_ho_sm_transition_state(DS707_S101_HO_SM_ACTIVE_HO);
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                          "S101: tun mode val changed 2 -> 0. "
                          "But HO SM was not in NULL state");
          /** We don't need to assert here since whatever the HO SM is
           *  waiting on (either IRMI resp or phys link bringup) will take
           *  care of transitioning the HO SM to NULL */
        }
      }
      break;
    }

    default:
    {  
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid prev tunnel mode val");
      break;
    }
  } /* Switch(prev_tunnel_mode_val)  */

} /* ds707_s101_tunnel_mode_chg_action */

/*===========================================================================
FUNCTION DS707_S101_TUNNEL_MODE_CHG_INFO
===========================================================================*/
/**
  Triggered by HDRPAC when HDR notifies about the tunnel mode change

  @param tun_mode             [in] Tunnel Mode Value

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_tunnel_mode_chg_info
(
  uint8 tun_mode
)
{
  ds707_s101_tunnel_mode_e tunnel_mode_val = DS707_S101_TUNNEL_MIN;
  ds707_s101_tunnel_mode_e prev_tunnel_mode_val = 
                                     ds707_s101_sm.tunnel_mode_val;

  tunnel_mode_val = ds707_s101_conv_tunnel_mode_val(tun_mode);

  if (DS707_S101_TUNNEL_MIN == tunnel_mode_val)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "S101: Invalid Tunnel Mode Value: %d - "
                    "Ignoring tunnel mode val change",
                    tunnel_mode_val);
    return;
  }

  DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                  "S101: Tunnel Mode Val Change: "
                  "Prev Mode: %d, New Mode: %d", 
                  prev_tunnel_mode_val, tunnel_mode_val);

  if (prev_tunnel_mode_val == tunnel_mode_val)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: No new change in Tunnel Mode Value "
                      "- Ignoring tunnel mode change ind");
    return;
  }
  
  ds707_s101_sm.tunnel_mode_val = tunnel_mode_val;

  ds707_s101_tunnel_mode_chg_action(prev_tunnel_mode_val, 
                                    ds707_s101_sm.tunnel_mode_val);

  DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                "S101: S101 SM Curr State: %s",
                s101_sm_names[ds707_s101_sm.s101_sm_state]);

  if (DS707_S101_TUNNEL_MODE_ENABLED == ds707_s101_sm.tunnel_mode_val)
  {
    if (DS707_S101_SM_TUNNEL_CALL_DISALLOWED == ds707_s101_sm.s101_sm_state &&
        ds707_s101_ok_to_allow_tunnel_call()
       )
    {
      /** The conditions necessary to allow tunnel call are met.
       *  Transition to tunnel allowed state */
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_ALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Tunnel Mode enabled. S101 not in disallowed "
                      "state and/or tunnel call allow conditions not met - "
                      "Ignoring tunnel mode change ind");
    }
  }
  else
  {
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_DISALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Tunnel Mode disabled/activeho. S101 not in allowed "
                      "state - Ignoring tunnel mode change ind");
    }
  }
  return;
} /* ds707_s101_tunnel_mode_chg_info */

/*===========================================================================
FUNCTION DS707_S101_PREREG_CHG_INFO
===========================================================================*/
/**
  Triggered by HDRPAC when HDR notifies about the prereg val change

  @param in_prereg_zone       [in] Prereg = TRUE/FALSE

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_prereg_chg_info
(
  boolean in_prereg_zone
)
{
  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                "S101: In Prereg Zone: %s"
                "S101 SM Curr State: %s", 
                (in_prereg_zone)? "TRUE": "FALSE",
                s101_sm_names[ds707_s101_sm.s101_sm_state]);


  ds707_s101_sm.prereg_allowed = in_prereg_zone;

  if (in_prereg_zone)
  {
    if (DS707_S101_SM_NON_TUNNEL == ds707_s101_sm.s101_sm_state)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "S101: Err: Sys in Prereg Zone while in non Tunnel state");
    }
    else if (DS707_S101_SM_TUNNEL_CALL_DISALLOWED == ds707_s101_sm.s101_sm_state &&
             ds707_s101_ok_to_allow_tunnel_call()
            )
    {
      /** The conditions necessary to allow tunnel call are met.
       *  Transition to tunnel allowed state */
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_ALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Prereg TRUE. S101 already in allowed "
                      "state or tunnel call allow conditions not met - "
                      "Ignoring prereg change");
    }
  }
  else
  {
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_DISALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Prereg FALSE. S101 not in allowed "
                      "state - Ignoring prereg change");
    }
  }
  return;
} /* ds707_s101_prereg_chg_info */

/*===========================================================================
FUNCTION DS707_S101_HO_TO_L_CHG_INFO
===========================================================================*/
/**
  Triggered when all PDNs go down and devmgr is in lingering state or if
  devmgr goes to closed state

  @param ho_to_l_in_prog       [in] Handoff to L in progress or not

  @return 
  None
 	
  @dependencies 
  None
*/
static void ds707_s101_ho_to_l_chg_info
(
  boolean ho_to_l_in_prog
)
{
  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                "S101: Setting ho_to_l_in_prog to: %s"
                "S101 SM Curr State: %s", 
                (ho_to_l_in_prog)? "TRUE": "FALSE",
                s101_sm_names[ds707_s101_sm.s101_sm_state]);

  ds707_s101_sm.ho_to_L_in_prog = ho_to_l_in_prog;

  if (ds707_s101_sm.ho_to_L_in_prog)
  {
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      /** This should not happen since ho_to_L_in_prog will be set to 
      true only when S101 transitions from Non-Tunnel to Disallowed state */ 
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_DISALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: ho_to_L_in_prog TRUE. S101 not in allowed "
                      "state - Ignoring ho_to_L_in_prog change");
    }
  }
  else
  {
    if (DS707_S101_SM_NON_TUNNEL == ds707_s101_sm.s101_sm_state)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: ho_to_L_in_prog FALSE."
                      "Ignore in non Tunnel state");
    }
    else if (DS707_S101_SM_TUNNEL_CALL_DISALLOWED == ds707_s101_sm.s101_sm_state &&
             ds707_s101_ok_to_allow_tunnel_call()
            )
    {
      /** The conditions necessary to allow tunnel call are met.
       *  Transition to tunnel allowed state */
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_ALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: ho_to_L_in_prog FALSE. S101 already in allowed "
                      "state or tunnel call allow conditions not met - "
                      "Ignoring ho_to_L_in_prog change");
    }
  }
  return;

} /* ds707_s101_ho_to_l_chg_info */

/*===========================================================================
FUNCTION DS707_S101_IS_TUNNEL_CALL_ALLOWED
===========================================================================*/
/**
  Returns TRUE is the S101 SM is in tunnel call allowed state

  @param
  None

  @return 
  TRUE  -- if S101 SM is in Tunnel Call Allowed State \n
  FALSE -- Otherwise \n
 	
  @dependencies 
  None
*/
boolean ds707_s101_is_tunnel_call_allowed
(
  void
)
{
  if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
  {
    DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, "S101: %s state", 
                  s101_sm_names[ds707_s101_sm.s101_sm_state]);
    return TRUE;
  }
  else
  {
    DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                  "S101: %s state. Tunnel Call not allowed", 
                  s101_sm_names[ds707_s101_sm.s101_sm_state]);
    return FALSE;
  }
} /* ds707_s101_is_tunnel_call_allowed */

/*===========================================================================
FUNCTION DS707_S101_QUERY_SM_STATE
===========================================================================*/
/**
  Returns the current S101 State Machine state

  @param
  None

  @return 
  S101 State Machine State Enum
 	
  @dependencies 
  None
*/
ds707_s101_sm_e ds707_s101_query_sm_state
(
  void
)
{
  DATA_IS707_MSG1(MSG_LEGACY_LOW, "S101: In %d state", 
                ds707_s101_sm.s101_sm_state);
  return ds707_s101_sm.s101_sm_state;
} /* ds707_s101_query_sm_state */

/*===========================================================================
FUNCTION DS707_S101_PROCESS_CMD
===========================================================================*/
/**
  Function that processes S101 specific cmds

  @param
  cmd_ptr         [in] pointer to cmd data

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_process_cmd
(
  const ds_cmd_type* cmd_ptr
)
{
  if (NULL == cmd_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "S101: ds707_s101_process_cmd: cmd_ptr is NULL");
    return;    
  }

  /** Call appropriate handling function based on command id */
  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_S101_COMPLETE_PREREG_HANDOFF:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: Processing DS_CMD_707_S101_COMPLETE_PREREG_HANDOFF");
      ds707_s101_complete_prereg_handoff(
        ((ds707_s101_process_cmd_info_type* ) 
         cmd_ptr->cmd_payload_ptr)->prereg_handoff_info);
      break;
    }

    default:
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "S101: ds707_s101_process_cmd: Invalid Cmd passed");
      break;
    }
  }
} /* ds707_s101_process_cmd */

/*===========================================================================
FUNCTION DS707_S101_IRMI_RESPONSE
===========================================================================*/
/**
  Called when HDR sends HDRPAC indication for the sent IRMI.
  Depending on whether the IRMI was accepted or not,
  this function notifies EPC appropriately

  @param
  irmi_accepted         [in] IRMI Accepted/Failed

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_irmi_response
(
  boolean irmi_accepted
)
{
  DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                "S101: IRMI response recvd: IRMI Accepted = %s",
                ((irmi_accepted)? "TRUE":"FALSE")
                );

  if (DS707_S101_HO_SM_IRMI_SENT != ds707_s101_query_prereg_ho_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: IRMI response recvd while Prereg HO SM not in"
                    "IRMI Sent State - Ignore response");
    return;
  }

  if (irmi_accepted)
  {
    ds707_s101_send_prereg_handoff_response(TRUE);
  }
  else
  {
    /** We should bring up phys link only if sys is on eHRPD and
     *  S101 SM still in Non Tunnel State */

    if (!ds707_pkt_is_ehrpd_mode_of_operation() ||
        DS707_S101_SM_NON_TUNNEL != ds707_s101_query_sm_state() )
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "IRMI failed. System not in eHRPD / "
                      "S101 SM != Non Tunnel State. Not "
                      "bringing up phys link "
                      "and posting prereg ho failure to EPC");
      ds707_s101_send_prereg_handoff_response(FALSE);
    }
    else
    {
      ds707_s101_bringup_phys_link();
    }
  }
} /* ds707_s101_irmi_response */

/*===========================================================================
FUNCTION DS707_S101_PROCESS_COLOR_CODE_CHG
===========================================================================*/
/**
  Called when HDR sends HDRPAC indication for color code change.
  This funtions calls into s101_dctm and s101_vctm modules to clear the
  throttling state (if currently throttled). 

  @param
  color_code         [in] new color code

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_process_color_code_chg
(
  uint8 color_code
)
{
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                  "S101:Color code changed to: %d,", 
                  color_code);
  /**
   * Reset S101_VCTM throttling count and timer values. We 
   * currently do not maintain VCTM throttling per color code or 
   * subnet id. So upon receiving color code change event from hdr 
   * we will clear the throttling table for the current color 
   * code. 
   */
  ds707_s101_vctm_reset_throttle();

  /**
   * Reset S101_DCTM throttling count and timer values. We 
   * currently do not maintain DCTM throttling per color code or 
   * subnet id. So upon receiving color code change event from hdr 
   * we will clear the throttling table for the current color 
   * code. 
   */
  ds707_s101_dctm_reset_throttle();

} /*ds707_s101_process_color_code_chg*/
/*===========================================================================
FUNCTION DS707_S101_CALL_CONNECTED_VIA_CM
===========================================================================*/
/**
  Function that gets called from call connected hdlr.
  If the prereg handoff was in progress, sends handoff complete
  success response to EPC

  @param
  None

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_call_connected_via_cm
(
  void
)
{
  if (DS707_S101_HO_SM_PHYSLINK_BRINGUP_REQUESTED != ds707_s101_query_prereg_ho_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Rcvd Call connected via CM while S101 didn't "
                    "initiate the physlink bringup - Ignore");
    return;
  }

  /** If S101 did request physlink bring up, ensure that the call
   *  connected was on eHRPD and S101 SM is in non tunnel state.
   *  i.e. "eHRPD direct mode"
   *  Scenario 1: It is possible in ping pong scenarios that
   *  system moved from LTE->eHRPD->1X quickly. When we moved to
   *  eHRPD, DS would have started Prereg HO and sent IRMI. On
   *  IRMI failure, DS attempts phys link bring up. At this moment
   *  if system moves to 1X and if sys was configured to be
   *  hybrid, then the call connect will succeed on 1x. So we need
   *  to confirm that the sys is still in eHRPD when the call
   *  connects. Scenario 2: While Prereg HO is in progress sys
   *  might move back to LTE and S101 SM could potentially move to
   *  Tunnel allowed state. So confirm that S101 is still in Non
   *  Tunnel State.  */

  if (ds707_pkt_is_ehrpd_mode_of_operation() && 
      DS707_S101_SM_NON_TUNNEL == ds707_s101_query_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Rcvd Call connected via CM while in eHRPD. "
                    "Sending success resp to EPC");
    ds707_s101_send_prereg_handoff_response(TRUE);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Rcvd Call connected via CM while not in eHRPD"
                    "and/or S101 SM != Non Tunnel State. "
                    "Sending failure resp to EPC");
    ds707_s101_send_prereg_handoff_response(FALSE);
  }
 
} /* ds707_s101_call_connected_via_cm */

/*===========================================================================
FUNCTION DS707_S101_CALL_ENDED_VIA_CM
===========================================================================*/
/**
  Function that gets called from call ended hdlr. If the prereg
  handoff was in progress, sends handoff complete failure response to EPC

  @param
  None

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_call_ended_via_cm
(
  void
)
{
  if (DS707_S101_HO_SM_PHYSLINK_BRINGUP_REQUESTED != ds707_s101_query_prereg_ho_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Rcvd Call ended via CM while S101 didn't "
                    "initiate the physlink bringup - Ignore");
    return;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Rcvd Call ended via CM while Prereg HO is"
                    "in progress. Sending failure resp to EPC");
    ds707_s101_send_prereg_handoff_response(FALSE);
  }
} /* ds707_s101_call_ended_via_cm */

/*===========================================================================
FUNCTION DS707_S101_IS_LCP_THROTTLED
===========================================================================*/
/**
  Returns TRUE is the S101 call lcp throttled

  @param
  None

  @return 
  TRUE  -- if S101 SM is the S101 call lcp throttled \n
  FALSE -- Otherwise \n
 	
  @dependencies 
  None
*/
boolean ds707_s101_is_lcp_throttled
(
  void
)
{
  return ds707_s101_sm.lcp_throttled;
} /* ds707_s101_is_lcp_throttled */

/*===========================================================================
FUNCTION DS707_S101_IS_VIRTUAL_CONNECTION_THROTTLED
===========================================================================*/
/**
  Returns TRUE is the S101 call virtual connection throttled

  @param
  None

  @return 
  TRUE  -- if S101 SM is the S101 call virtual connection throttled \n
  FALSE -- Otherwise \n
 	
  @dependencies 
  None
*/
boolean ds707_s101_is_virt_conn_throttled
(
  void
)
{
  return ds707_s101_sm.virtual_connection_throttled;
} /* ds707_s101_is_virt_conn_throttled */

/*===========================================================================
FUNCTION DS707_S101_IS_SYS_MODE_LTE
===========================================================================*/
/**
  Returns TRUE is the Sys mode is LTE

  @param
  None

  @return 
  TRUE  -- if Sys mode is LTE \n
  FALSE -- Otherwise \n
 	
  @dependencies 
  None
*/
boolean ds707_s101_is_sys_mode_lte
(
  void
)
{
  return ds707_s101_sm.sys_mode_lte;
} /* ds707_s101_is_sys_mode_lte  */

/*===========================================================================
FUNCTION DS707_S101_IS_EHRPD_MODE
===========================================================================*/
/**
  Returns TRUE is the S101 call in eHRPD mode

  @param
  None

  @return 
  TRUE  -- if S101 SM is the S101 call in eHRPD mode \n
  FALSE -- Otherwise \n
 	
  @dependencies 
  None
*/
boolean ds707_s101_is_ehrpd_mode
(
  void
)
{
  return ds707_s101_sm.ehrpd_mode;
} /* ds707_s101_is_ehrpd_mode */

/*===========================================================================
FUNCTION DS707_S101_IS_HDR_SESSION_OPEN
===========================================================================*/
/**
  Returns TRUE is the HDR session is open

  @param
  None

  @return 
  TRUE  -- if HDR session is open \n
  FALSE -- Otherwise \n
 	
  @dependencies 
  None
*/
boolean ds707_s101_is_hdr_session_open
(
  void
)
{
  return ds707_s101_sm.hdr_sess_open;
} /* ds707_s101_is_hdr_session_open */

/*===========================================================================
FUNCTION DS707_S101_IS_TUNNEL_MODE
===========================================================================*/
/**
  Returns TRUE is the S101 call in tunnel mode

  @param
  None

  @return 
  TRUE  -- if S101 SM is the S101 call in tunnel mode \n
  FALSE -- Otherwise \n
 	
  @dependencies 
  None
*/
boolean ds707_s101_is_tunnel_mode
(
  void
)
{
  if (ds707_s101_sm.tunnel_mode_val == DS707_S101_TUNNEL_MODE_ENABLED ||
      ds707_s101_sm.tunnel_mode_val == DS707_S101_TUNNEL_ACTIVEHO_MODE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* ds707_s101_is_ehrpd_mode */

/*===========================================================================
FUNCTION DS707_S101_IS_PREREG_ALLOWED
===========================================================================*/
/**
  Returns TRUE is the S101 call in prereg allowed mode

  @param
  None

  @return 
  TRUE  -- if S101 SM is the S101 call in prereg allowed mode \n
  FALSE -- Otherwise \n
 	
  @dependencies 
  None
*/
boolean ds707_s101_is_prereg_allowed
(
  void
)
{
  return ds707_s101_sm.prereg_allowed;
} /* ds707_s101_is_prereg_allowed */

/*===========================================================================
FUNCTION DS707_S101_PPP_AUTH_EV
===========================================================================*/
/**
  PPP Devmgr notifies S101 via this function about a
  Auth Success/Failure Event

  @param auth_success          [in] Auth Succeeded: TRUE/FALSE

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_ppp_auth_ev
(
  boolean auth_success
)
{
  if (DS707_S101_SM_NON_TUNNEL == ds707_s101_query_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Recvd PPP Auth Ev in Non Tunnel State - Ignore");
    return;
  }

  DATA_IS707_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                "S101: Recvd PPP Auth %s event in tunnel mode", 
                (auth_success)? "SUCCESS": "FAILURE");

  /** For now, we only care about auth success event and report
   *  it as a PPP session Opened event to HDR */
  if (auth_success)
  {
    if (ds707_s101_sm.last_reported_prereg_info.ppp_up == auth_success)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: No Change in PPP state since last report "
                      "to HDR - Ignore");
      return;
    }

    hdrpac_set_prereg_ppp_status(auth_success);

    ds707_s101_sm.last_reported_prereg_info.ppp_up = auth_success;
  }

} /* ds707_s101_ppp_auth_ev */

/*===========================================================================
FUNCTION DS707_S101_PPP_LCP_EV
===========================================================================*/
/**
  PPP Devmgr notifies S101 via this function about a
  LCP Up/Down Event

  @param lcp_up          [in] LCP Up: TRUE/FALSE

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_ppp_lcp_ev
(
  boolean lcp_up
)
{
  if (DS707_S101_SM_NON_TUNNEL == ds707_s101_query_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Recvd PPP LCP Ev in Non Tunnel State - Ignore");
    return;
  }

  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                "S101: Recvd PPP LCP %s event in tunnel mode,"
                "ho_to_L_in_prog:%s", 
                ((lcp_up)? "UP": "DOWN"),
                ((ds707_s101_sm.ho_to_L_in_prog)? "TRUE":"FALSE"));

  if (FALSE == lcp_up && TRUE == ds707_s101_sm.ho_to_L_in_prog)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: PPP down, Wait for Phy link null");
  }


  /** For now, we only care about LCP Down event and report
   *  it as a PPP session closed event to HDR */
  if (!lcp_up)
  {
    if (ds707_s101_sm.last_reported_prereg_info.ppp_up == lcp_up)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: No Change in PPP state since last report "
                      "to HDR - Ignore");
      return;
    }

    hdrpac_set_prereg_ppp_status(lcp_up);

    ds707_s101_sm.last_reported_prereg_info.ppp_up = lcp_up;
  }

} /* ds707_s101_ppp_lcp_ev */

/*===========================================================================
FUNCTION DS707_S101_PPP_VSNCP_EV
===========================================================================*/
/**
  S101 notified via this function whenever a VSNCP comes up or when all
  VSNCPs go down

  @param vsncp_up          [in] VSNCP Up: TRUE/FALSE

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_ppp_vsncp_ev
(
  boolean vsncp_up
)
{
  if (DS707_S101_SM_NON_TUNNEL == ds707_s101_query_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Recvd VSNCP Ev in Non Tunnel State - Ignore");
    return;
  }


  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                "S101: Recvd VSNCP %s event in tunnel mode, "
                "ho_to_L_in_prog:%s", 
                ((vsncp_up)? "UP": "DOWN"),
                ((ds707_s101_sm.ho_to_L_in_prog)? "TRUE":"FALSE"));

  if (FALSE == vsncp_up &&
      TRUE == ds707_s101_sm.ho_to_L_in_prog)
  {
    if (DEVMGR_LINGER_STATE == ds707_devmgr_get_state() ||
        DEVMGR_CLOSED_STATE == ds707_devmgr_get_state())
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: All PDNs down and devmgr in lingering/closed state."
                    "Set ho_to_L_in_prog to FALSE");
      ds707_s101_ho_to_l_chg_info(FALSE);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: All PDNs down but devmgr not in lingering/closed state."
                    "Wait for PPP down ev to set ho_to_L_in_prog to FALSE");
    }
  }

  if (ds707_s101_sm.last_reported_prereg_info.vsncp_up == vsncp_up)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: No Change in VSNCP state since last report "
                    "to HDR - Ignore");
    return;
  }

  
  hdrpac_set_prereg_vsncp_status(vsncp_up);
  
  ds707_s101_sm.last_reported_prereg_info.vsncp_up = vsncp_up;

} /* ds707_s101_ppp_vsncp_ev */

/*===========================================================================
FUNCTION DS707_S101_HDR_SESSION_CHG_EV
===========================================================================*/
/**
  This event is triggered whenever HDR session changes
 
  @param sess_open          [in] HDR Session Open: TRUE/FALSE
 
  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_hdr_session_chg_ev
(
  boolean sess_open
)
{
  DATA_IS707_MSG_SPRINTF_2(MSG_LEGACY_HIGH, 
                "S101: HDR Sess Change: HDR Sess State = %s"
                "S101 SM Curr State: %s", 
                ((sess_open)? "OPEN": "CLOSED"),
                s101_sm_names[ds707_s101_sm.s101_sm_state]);

  ds707_s101_sm.hdr_sess_open = sess_open;

  if (DS707_S101_SM_NON_TUNNEL == ds707_s101_query_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Recvd HDR Session chg in Non Tunnel State - No-Op");
    return;
  }

  if (sess_open)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Recvd HDR Session Open in Tunnel Mode. "
                    "Re-Sending Prereg status info");
  
    /** Send the last reported prereg status again to HDR.
     *  Reason: When HDR session closes in tunnel mode, HDR sends
     *  prereg = False to the network. When it opens the session
     *  again it needs to let the network know if we are
     *  preregistered are not. However since HDR doesn't cache the
     *  prereg status, it needs DS to resend the prereg status
     *  info */
  
    hdrpac_set_prereg_ppp_status(ds707_s101_sm.last_reported_prereg_info.ppp_up);
    hdrpac_set_prereg_vsncp_status(ds707_s101_sm.last_reported_prereg_info.vsncp_up);

    if (DS707_S101_SM_TUNNEL_CALL_DISALLOWED == ds707_s101_sm.s101_sm_state &&
        ds707_s101_ok_to_allow_tunnel_call()
       )
    {
      /** The conditions necessary to allow tunnel call are met.
       *  Transition to tunnel allowed state */
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_ALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: HDR Sess Opened. S101 not in tunnel disallowed "
                      "state and/or tunnel call allow conditions not "
                      "met - No-Op for HDR Session Open Event");
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "S101: Recvd HDR Session Close in Tunnel Mode.");
    if (DS707_S101_SM_TUNNEL_CALL_ALLOWED == ds707_s101_sm.s101_sm_state)
    {
      ds707_s101_transition_state(DS707_S101_SM_TUNNEL_CALL_DISALLOWED);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "S101: HDR Sess Closed. S101 not in tunnel allowed "
                      "state - No-Op for HDR Session Close Event");
    }
  }

} /* ds707_s101_hdr_session_chg_ev */

/*===========================================================================
FUNCTION DS707_S101_SET_HDR_ACTV_HO_IN_PROG
===========================================================================*/
/**
  This event is triggered whenever HDR acquired traffic channel over tunnel
  for active ho
 
  @param
  None

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_set_hdr_actv_ho_in_prog
(
  void
)
{
  ds_dsd_event_info_type  dsd_event_info;
  uint32                       rat_mask = 0;
  uint32                       so_mask = 0;

  if (ds707_s101_is_hdr_actv_ho_in_prog())
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                  "S101: actv ho already in prog"        
                  "-Ignore set_hdr_actv_ho_in_prog");
    return;
  }

  memset(&dsd_event_info,0,sizeof(dsd_event_info));

  DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                  "S101: Setting ds707_s101_actv_ho_in_prog to TRUE and"         
                  "notify DSD that HDR is in full service");

  rex_enter_crit_sect(&ds707_s101_crit_sect);
  ds707_s101_actv_ho_in_prog = TRUE;
  rex_leave_crit_sect(&ds707_s101_crit_sect);

  ds707_s101_sys_chg_notification(ds707_s101_is_tunnel_call_allowed(),
                                  TRUE); 

  /* Notify DSD that HDR radio is acquired! . Note that it is assumed
     that HDR is in full service and hence DSD is notified that HDR
     is full service. */
  ds707_pkt_mgr_get_hdr_sess_info(&rat_mask, &so_mask);
  dsd_event_info.event = DS_DSD_CLIENT_RADIO_ACQUIRED_EV;
  dsd_event_info.data.attach_radio.srv_status = SYS_SRV_STATUS_SRV;
  dsd_event_info.data.attach_radio.is_remote = 0;
  dsd_event_info.data.attach_radio.so_mask   = so_mask;
  dsd_event_info.data.attach_radio.rat_mask  = rat_mask;

  ds_dsd_ext_event_notify(DS_DSD_RADIO_HDR, &dsd_event_info);

} /* ds707_s101_set_hdr_actv_ho_in_prog */

/*===========================================================================
FUNCTION DS707_S101_SET_HDR_ACTV_HO_COMPLETE
===========================================================================*/
/**
  This event is triggered when CM reports HDR is in service via SS chg event
 
  @param
  None

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_set_hdr_actv_ho_complete
(
  void
)
{
  if (!ds707_s101_is_hdr_actv_ho_in_prog())
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                  "S101: Actv ho is not in prog"        
                  "-Ignore hdr_actv_ho_complete");
    return;
  }

  DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                  "S101: HDR Actv ho complete. "                                 
                  "Setting ds707_s101_actv_ho_in_prog to FALSE");

  rex_enter_crit_sect(&ds707_s101_crit_sect);
  ds707_s101_actv_ho_in_prog = FALSE;
  rex_leave_crit_sect(&ds707_s101_crit_sect);

} /* ds707_s101_set_hdr_actv_ho_complete */

/*===========================================================================
FUNCTION DS707_S101_SET_HDR_ACTV_HO_FAILED
===========================================================================*/
/**
  This event is triggered when HDR doesn't complete active ho and the HDR
  stack gets deactivated
 
  @param
  None

  @return 
  None
 	
  @dependencies 
  None
*/
void ds707_s101_set_hdr_actv_ho_failed
(
  void
)
{
  ds_dsd_event_info_type  dsd_event_info;
  sys_srv_status_e_type   lte_srv_status = SYS_SRV_STATUS_NO_SRV;
  ps_phys_link_type       *phys_link_ptr;
  int16                   err_num;
  uint32                       rat_mask = 0;
  uint32                       so_mask = 0;

  if (!ds707_s101_is_hdr_actv_ho_in_prog())
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                  "S101: HDR is deactivated when actv ho is not in prog"         
                  "-Ignore");
    return;
  }

  memset(&dsd_event_info,0,sizeof(dsd_event_info));

  DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                  "S101: HDR Actv ho failed. "                                
                  "Setting ds707_s101_actv_ho_in_prog to FALSE");

  rex_enter_crit_sect(&ds707_s101_crit_sect);
  ds707_s101_actv_ho_in_prog = FALSE;
  rex_leave_crit_sect(&ds707_s101_crit_sect);

  /*Notify DSD that HDR is lost. 
    Also, look at the cached data and report LTE information */
  if (FALSE == ds3g_is_hdr_in_srvc())
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                    "S101: notify DSD that HDR is Lost");
    ds707_pkt_mgr_get_hdr_sess_info(&rat_mask, &so_mask);
    memset(&dsd_event_info,0,sizeof(dsd_event_info));
    dsd_event_info.event = DS_DSD_CLIENT_RADIO_LOST_EV;
    dsd_event_info.data.attach_radio.rat_mask = DS_SYS_RAT_3GPP2_NULL_BEARER;
    dsd_event_info.data.attach_radio.srv_status = SYS_SRV_STATUS_NO_SRV;
    dsd_event_info.data.attach_radio.so_mask   = so_mask;
    dsd_event_info.data.attach_radio.rat_mask  = rat_mask;

    ds_dsd_ext_event_notify(DS_DSD_RADIO_HDR, &dsd_event_info);

    //Check cached radio info and report LTE service if in limited or full srvc
    lte_srv_status = ds3g_get_lte_srv_status();

    if (SYS_SRV_STATUS_LIMITED == lte_srv_status ||
        SYS_SRV_STATUS_SRV == lte_srv_status)
    {
      memset(&dsd_event_info,0,sizeof(dsd_event_info));
      dsd_event_info.event = DS_DSD_CLIENT_RADIO_ACQUIRED_EV;
      dsd_event_info.data.attach_radio.srv_status = lte_srv_status;
      dsd_event_info.data.attach_radio.is_remote = 0;
      ds_dsd_ext_event_notify(DS_DSD_RADIO_LTE, &dsd_event_info);
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "S101: CM has somehow notified that HDR is in service"
                    "Donot notify DSD on active ho fail");
  }

  if (DS707_S101_HO_SM_PHYSLINK_BRINGUP_REQUESTED == 
           ds707_s101_query_prereg_ho_sm_state())
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "S101: Actv HO failed while S101 HO SM is in Phys link"
                    " bring up requested state - tear down phys link and send "
                    " Prereg complete fail to EPC");

    phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
    ps_phys_link_down_cmd( phys_link_ptr, &err_num, NULL);

    ds707_s101_send_prereg_handoff_response(FALSE);
  }

} /* ds707_s101_set_hdr_actv_ho_failed */

/*===========================================================================
FUNCTION DS707_S101_IS_HDR_ACTV_HO_IN_PROG
===========================================================================*/
/**
  Utility API to query if HDR Active HO is in progress
 
  @param
  None

  @return 
  TRUE  -- if active ho is in progress
  FALSE -- Otherwise
 	
  @dependencies 
  None
*/
boolean ds707_s101_is_hdr_actv_ho_in_prog
(
  void
)
{
  boolean actv_ho_in_prog = FALSE;

  rex_enter_crit_sect(&ds707_s101_crit_sect);
  actv_ho_in_prog = ds707_s101_actv_ho_in_prog;
  rex_leave_crit_sect(&ds707_s101_crit_sect);

  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "S101: ds707_s101_actv_ho_in_prog = %d ",                      
                  actv_ho_in_prog);
  return actv_ho_in_prog;
}

#endif /* FEATURE_DATA_OPTHO */

