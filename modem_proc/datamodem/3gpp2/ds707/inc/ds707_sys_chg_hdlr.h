#ifndef DS_SYS_CHG_HDLR_H
#define DS_SYS_CHG_HDLR_H

/*===========================================================================
            D S   S Y S T E M   C H A N G E   H A N D L E R         

GENERAL DESCRIPTION
  This module maintains the knowledge of the current system the DS C2K Mode 
  handler is present on.  All the modules that are interested in this 
  information should refer to the state of this module. 
  
  This module generates the data_bearer_tech indication, maintains if the 
  C2K mode handler is on a DO system, or on a 1x-RTT system. If on a DO 
  system will also have the knowledge if it is connected to an EPC core 
  or a non-EPC core.

  
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
   

 Copyright (c) 2009-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_sys_chg_hdlr.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
06/30/14    niv       Added wrapper functions & API change for 
                      NV_REFRESH 
04/16/10   ls         Fixed featurization issue 
03/15/10   ls         Slient Redial feature
07/06/09   spn        Initial Revision.


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "sys.h"
#include "dstask_v.h"
#include "cm.h"

typedef enum
{
  DS707_SYS_INFO_NO_SYSTEM      = 0,
  DS707_SYS_INFO_EPC_SYSTEM     = 1,
  DS707_SYS_INFO_NON_EPC_SYSTEM = 2
} ds707_sys_info_enum_type;

typedef enum
{
  DS707_EHSR_STATE = 0,
  DS707_AUSR_STATE = 1
} ds707_silent_redial_state_type;

#ifdef FEATURE_EHRPD
typedef enum
{
  DS707_SYS_CHG_SM_NULL = 0,             /* S0: NULL/Init State */
  DS707_SYS_CHG_SM_EPC_TO_NONEPC,        /* S1: In process of EPC to NONEPC Transition */
  DS707_SYS_CHG_SM_NONEPC_TO_EPC,        /* S2: In process of NONEPC to EPC Transition */
  DS707_SYS_CHG_SM_WAIT_FOR_DORM,        /* S3: Waiting for phys link to go down */
  DS707_SYS_CHG_SM_1X_SP_CALL_IN_PROG,   /* S4: In process of 1X Special Call Bringup */
  DS707_SYS_CHG_SM_1X_SP_CALL_CLEANUP,   /* S4: In process of 1X Special Call Cleanup */
  DS707_SYS_CHG_SM_1X_DEREG_IN_PROG,     /* S4: In process of 1X stack deregistration */ 
  DS707_SYS_CHG_SM_MAX
}ds707_sys_chg_sm_e;

typedef enum
{
  DS707_SYS_CHG_LTE_ATTACH_INIT = 0,
  DS707_SYS_CHG_NONEPC_TO_EPC = 1
} ds707_sys_chg_nonepc_cleanup_trigger_e;
#endif /* FEATURE_EHRPD */

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/


/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_INIT
 
DESCRIPTION    Initializes the module.
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_hdlr_init(void); 

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_SILENT_REDIAL_INFO_INIT
 
DESCRIPTION    Initializes the module.
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_hdlr_silent_redial_info_init(void);

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_READ_NONEPC_CALL_CLEANUP
 
DESCRIPTION    This function is used to read the EFS items for 
               NONEPC call cleanup
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_hdlr_read_nonepc_call_cleanup(void);

#ifdef FEATURE_HDR
/*=========================================================================== 
 
FUNCTION       DS_SYS_CHG_HDLR_HDR_SESSION_CHANGED
 
DESCRIPTION    To be called to notify that the HDR session is opened
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_hdlr_hdr_session_changed(void); 

/*=========================================================================== 
 
FUNCTION       DS_SYS_CHG_HDLR_GET_ACTIVE_HDR_PROTOCOL
 
DESCRIPTION    
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
sys_active_prot_e_type ds707_sys_chg_hdlr_get_active_hdr_protocol(void); 

#endif /* FEATURE_HDR */

/*=========================================================================== 
 
FUNCTION       DS_SYS_CHG_HDLR_SYS_MODE_UPDATED
 
DESCRIPTION    
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
boolean ds707_sys_chg_hdlr_sys_mode_updated(sys_sys_mode_e_type new_sys_mode); 
  
/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_IS_CURRENT_SYSTEM_EPC
 
DESCRIPTION    
   
DEPENDENCIES   None.
 
RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/ 
ds707_sys_info_enum_type ds707_sys_chg_hdlr_get_current_system(void);

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_IS_SESS_OPEN
 
DESCRIPTION    

DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
boolean ds707_sys_chg_hdlr_is_sess_open(void);

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_HDLR_CHG_SYS_INFO
 
DESCRIPTION    
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_hdlr_chg_sys_info
(
  ds707_sys_info_enum_type sys_info
);

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_TRANSITION_STATE
 
DESCRIPTION    This function processes silent redial state change to the 
               new state. 
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_sr_transition_state
(
  ds707_silent_redial_state_type new_state
);

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_PROCESS_CALL_END
 
DESCRIPTION    This function is called when DS recieves call end from CM and 
               phys link is in COMING_UP or RESUMING state i.e. call end
               happend while call origination. 
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_sr_process_call_end
(
  void
);

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_PROCESS_CALL_CONNECT
 
DESCRIPTION    This function is called when DS recieves phys link up event.
               If the silent redial state is EHSR and the call is connected
               on eHRPD then it resets the counters for failure_count and
               total_failure_count
   
DEPENDENCIES   None.
 
RETURN VALUE   None.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_sr_process_call_connect(void);

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_PROCESS_CALL_ORIG
 
DESCRIPTION    This function is called when DS originates a call to CM. It 
               performs some additional processing like stopping the new
               call attempt timer before originating the CM call. 
   
DEPENDENCIES   None.
 
RETURN VALUE   BOOLEAN.
 
SIDE EFFECTS   None.
===========================================================================*/ 
void ds707_sys_chg_sr_process_call_orig
(
  void
);

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_SR_PROCESS_EPC_HANDOFF
 
DESCRIPTION    This function is called on EPC to non-EPC or non-EPC to EPC 
               handoff. It transitions sr_state accordingly. 
   
DEPENDENCIES   None.
 
RETURN VALUE   BOOLEAN.
 
SIDE EFFECTS   None.
===========================================================================*/
void ds707_sys_chg_sr_process_epc_handoff
(
  void
);

/*=========================================================================== 
 
FUNCTION       DS707_SYS_CHG_IS_IN_EHSR_STATE
 
DESCRIPTION    This is an utility function that returns TRUE if ds is in 
               EHSR state currently. 
   
DEPENDENCIES   None.
 
RETURN VALUE   BOOLEAN.
 
SIDE EFFECTS   None.
===========================================================================*/ 
boolean ds707_sys_chg_is_in_ehsr_state
(
  void
);

/*===========================================================================
FUNCTION       DS707_SYS_CHG_NEW_CALL_ATTEMPT_TIMER_CB

DESCRIPTION    Function is called when the Timer for a new call atttempt 
               is expired. 
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_sys_chg_new_call_attempt_timer_cb
(
  void * /*callback data unused*/
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_PROCESS_CMD

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  
===========================================================================*/
void ds707_sys_chg_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION      DS707_SYS_CHG_SR_PROCESS_LPM_MODE

DESCRIPTION  Will reset Silent redial state machine  and clean up the failure
             count and total failure count. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sys_chg_sr_process_lpm_mode
(
  void
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_IS_NONEPC_SP_CALL_CLEANUP_NEEDED

DESCRIPTION   Checks if any special call cleanup procedure needs to be done and 
              kick starts the cleanup procedure 

DEPENDENCIES  None

RETURN VALUE  TRUE  - If cleanup needed 
              FALSE - Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sys_chg_is_nonepc_sp_call_cleanup_needed
(
  ds707_sys_chg_nonepc_cleanup_trigger_e cleanup_trigger,
  ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_EPC_TO_NONEPC_IND

DESCRIPTION   Triggered when EPC_TO_NONEPC event is posted 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_epc_to_nonepc_ind
(
  void
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_NONEPC_TO_EPC_IND

DESCRIPTION   Triggered when NONEPC_TO_EPC event is posted 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_nonepc_to_epc_ind
(
  void
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_ALL_PDN_DOWN_IND

DESCRIPTION   Triggered when all PDNs are cleaned up 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_all_pdn_down_ind
(
  void
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_IFACE_RECONFIG_IND

DESCRIPTION   Triggered when EPC issues iface reconfig ioctl due to SRAT 
              REcovery 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_iface_reconfig_ind
(
  void
);

/*===========================================================================
FUNCTION      DS707_QUERY_SYS_CHG_SM_STATE

DESCRIPTION   Returns the Sys Chg SM state

DEPENDENCIES  None

RETURN VALUE  Sys Chg SM State

SIDE EFFECTS  None
===========================================================================*/
ds707_sys_chg_sm_e ds707_query_sys_chg_sm_state
(
  void
);
#endif /*FEATURE_EHRPD*/

/*===========================================================================
FUNCTION      DS707_SYS_CHG_SET_NOTIFY_DSD

DESCRIPTION   Sets the Notify_DSD flag. Set when DSD reports pref sys chg and 
              MH needs to do some processing/cleanup before DSD can report the
              change to external clients

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_set_notify_dsd
(
  void
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_CLEAR_NOTIFY_DSD

DESCRIPTION   Clears the Notify_DSD flag

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_clear_notify_dsd
(
  void
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_IS_DSD_NOTIFICATION_PENDING

DESCRIPTION   returns Notify_DSD flag value

DEPENDENCIES  None

RETURN VALUE  TRUE - If notification is pending 
              FALSE - Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sys_chg_is_dsd_notification_pending
(
  void
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_SEND_DSD_NOTIFICATION

DESCRIPTION   Sends notification to DSD confirmation MH has completed 
              pref sys ind processing

DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_send_dsd_notification
(
  void
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_PROCESS_CLEANUP_PPP_STATE

DESCRIPTION   Called when CM notifies the current state of 
              CM_AC_CMD_CLEAR_1XPPP cmd processing

DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
void ds707_sys_chg_process_cleanup_ppp_state
(
  cm_ac_state_e_type      state,        /* CLEAR_1XPPP command state*/
  cm_ac_end_reason_e_type end_reason    /* CLEAR_1XPPP state end reason */
);

/*===========================================================================
FUNCTION      DS707_SYS_CHG_SR_PROCESS_NV_REFRESH_IND

DESCRIPTION  Will reset Silent redial state machine and clean 
             up the failure count and total failure count. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sys_chg_sr_process_nv_refresh_ind
(
  void
);


#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif /* DS_SYS_CHG_HDLR_H */
