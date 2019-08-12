#ifndef DS707_S101_H
#define DS707_S101_H

/** 
  @file ds707_s101.h
  @brief 
   Handles Optimized Handoff Implementation.
   Contains S101 State Machine
*/

/*===========================================================================

                            D S 7 0 7 _ S101
GENERAL DESCRIPTION
  This file contains APIs that other modules invoke whenever there is a 
  change in tunnel mode, prereg state, system throttle etc. Used to 
  transition the S101 state

EXTERNALIZED FUNCTIONS


 Copyright (c) 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_s101.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/11   Ack     Created module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "sys.h"
#include "dstask_v.h"

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
                          PUBLIC DATA DECLARATIONS
===========================================================================*/
/* define end reason for DS707 S101 
 * since it shares with HDR end reason, it starts from 0x8000 
 */
#define DS707_S101_DOWN_REASON_UNSPECIFIED          0
#define DS707_S101_DOWN_REASON_CLIENT_END           0x8000
#define DS707_S101_DOWN_REASON_CALL_INVALID_CONFIG  0x8001
#define DS707_S101_DOWN_REASON_CALL_DISALLOWED      0x8002

typedef enum
{
  DS707_S101_SM_NON_TUNNEL = 0,          /* S0: Sys !LTE (i.e in 1X/HDR/GW) */
  DS707_S101_SM_TUNNEL_CALL_ALLOWED,     /* S1: Sys in LTE but Tunnel Call Disallowed */
  DS707_S101_SM_TUNNEL_CALL_DISALLOWED,  /* S2: Sys in LTE and Tunnel Call Allowed */
  DS707_S101_SM_MAX
}ds707_s101_sm_e;

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
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
);

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
);


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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);
/*===========================================================================
FUNCTION DS707_S101_IS_VIRT_CONN_THROTTLED
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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

#endif /* FEATURE_DATA_OPTHO */
#endif /* DS707_S101_H */
