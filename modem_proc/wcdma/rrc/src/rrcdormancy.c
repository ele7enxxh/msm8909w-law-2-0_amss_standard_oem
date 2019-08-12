#ifndef RRCDORMANCY_H
#define RRCDORMANCY_H
/*===========================================================================
                          R R C  FAST  DORMANCY

DESCRIPTION

 This module defines rrc fast dormancy feature in RRC.

 This module has functionality related to 3GPP defined Fast 
 Dormancy implementation & switch back to proprietary Fast 
 Dormancy in case NW doesnt support.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Queues must be initialized by calling rrc_init_command_queues()
  at RRC task startup.

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcdormancy.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/15   ad      Made changes to log important parms from OTAs
09/03/14   sp      Made changes to save procedure substates in crash debug info
08/14/14   bc      Made changes to start T323 with value 3 seconds on receiving FD request when Network indicates T323 as 0.
06/09/14   sn      Changes to initialize SCRR if active while aborting dormancy procedure
06/12/14   gv      Made changes to check for the presence of PS RAB before triggering SCRI on T323 expiry
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/18/14   sa      Made the changes to notify about the PCH state in and out of PCH state to CM
09/25/13   bc      Added code to support exchange of AS ID when LTE feature is enabled
08/05/13   sp      Made changes to notify PCH state info to NAS
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
03/08/13   sr      Made changes to send SCRI for fast dormancy req received during DCH->FACH transition
01/29/13   mp      Made the changes to notify the PCH in and out for DS
10/26/12   gv      Mainline the changes under FEATURE_3GPP_FAST_DORMANCY_ENHANCEMENT
08/13/12   gv      Made changes to check for CS domain status before sending SCRI for T323 expiry
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/08/12   gv      Made changes to reset the RLC activity flag on receiving every
                   new FD request from application
10/04/11   gv      Modified the function close session ind by adding 
                   cause as new argument
09/13/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.106.00
09/07/11   gv      Added code to support fast dormancy enhancement feature
07/19/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.102.00
07/12/11   kp      Added code to not trigger Fast Dormancy in URA_PCH state
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/28/11   kp      Added code to send close_session_ind to NAS 
                   after receiving RRC_SIG_CONN_RELEASE_CNF
02/15/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.91.50
01/31/11   ad      Fixed compilation warnings
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/16/10   as      Added code to fill as_id for fast dormancy cnf
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
09/27/10   ss      Added code to support as_id changes for Fast Dormancy
08/27/10   rm      Made changes to index SIB with correct SIB type
08/21/10   ss      Fixed compilation warnings.
05/07/10   prk     Fixed compiler warnings.
04/01/09   ss      Added functionality related to 3gpp defined Fast Dormancy.
12/30/09   kp      Created file and added functionality related to 3gpp defined Fast Dormancy.
===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "uecomdef.h"
#include "comdef.h"

#include "rrcmmif.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "rrcdt.h"
#include "rrcdormancy.h"
#include "rrcccm.h"
#include "rrcintif.h"
#include "rrcrce.h"
#include "rrcrcr.h"
#include "rrcsend.h"
#include "rrclcm.h"
#include "rrctmr.h"
#include "rrccu.h"
#include "rrcscmgr.h"
#include "rrciho.h"
#include "rrccho.h"
#include "rrcsmc.h"
#include "rrcsibdb.h"
#include "rrcllc.h"
#include "rlcrrcif_v.h"
#include "rlcrrcif.h"
#include "rrcdata.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#include "ds3gpp_api.h"
#include "mmtask_v.h"
#include "sys_v.h"
#include "cmwll.h"
#include "rrcnv.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
rrcdormancy_substate_e_type rrcdormancy_substate;
rrcdormancy_fast_dormancy_req_type rrcdormancy_saved_fast_dormancy_req;
  
/*Stores T323 timer and N/W support Status */ 
rrcdormancy_conn_timers_and_constants_type rrcdormancy_conn_timers_and_constants;

/* Stores the count that guard sending of SCRI in CELL_PCH/URA_PCH states */
unsigned short rrc_v_316;

/* Macro which define max SCRI re tries */
#define RRC_DORMANCY_MAX_SCRI_RETRY 5

/* Macro which define timer value to be started in case of PCH to FACH 
 * transition which pending FD
 */
#define RRC_DORMANCY_T323_RETRY_TIMER 3000

uint8  rrc_max_scri_for_fd = RRC_DORMANCY_MAX_SCRI_RETRY;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** static function declarations
** ----------------------------------------------------------------------- */

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_get_T323_timer_value  

  DESCRIPTION
    This function check returns the value of T323 timer provided by Network
    in milliseconds.

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
static uint32 rrcdormancy_get_T323_timer_value
(
  void
);

/*===========================================================================
  
  FUNCTION rrcdormancy_process_pending_fast_dormancy_req
  
  DESCRIPTION
    This function processes rrcdormancy_process_pending_fast_dormancy_req
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
 ===========================================================================*/
static boolean rrcdormancy_process_pending_fast_dormancy_req
(
  void
);


/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
/* Any forward declarations go here */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */
  
/*===========================================================================
    
    FUNCTION rrcdormancy_init_saved_fast_dormancy_req  
    
    DESCRIPTION
      This function init rrcdormancy_saved_fast_dormancy_req
    
    DEPENDENCIES
      None.
    
    RETURN VALUE
      None.
  ===========================================================================*/
static void rrcdormancy_init_saved_fast_dormancy_req
(
  void
)
{
  WRRC_MSG0_HIGH("RRCDORM:init rrcdormancy_saved_fast_dormancy_req");
  rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status = FALSE;
  rrcdormancy_saved_fast_dormancy_req.mui = 0;
  rrcdormancy_saved_fast_dormancy_req.mui_status = RRCDORMANCY_MUI_STATUS_INVALID;
  rrcdormancy_substate = RRCDORMANCY_INITIAL;
}

/*===========================================================================
  
  FUNCTION rrcdormancy_save_fast_dormancy_req  
  
  DESCRIPTION
    This function save RRC_FAST_DORMANCY_REQ command received from MM task.
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
  
===========================================================================*/
static rrcdormancy_status_e_type rrcdormancy_save_fast_dormancy_req
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrcdormancy_status_e_type save_status = RRCDORMANCY_FAILURE;

  if(cmd_ptr->cmd_hdr.cmd_id == RRC_FAST_DORMANCY_REQ)
  {
    if(rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status == FALSE)
    {
      WRRC_MSG0_HIGH("RRCDORM:Save fast_dormancy_req");
      rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status = TRUE;
      rrcdormancy_saved_fast_dormancy_req.mui_status = RRCDORMANCY_MUI_STATUS_INVALID;
      rrcdormancy_saved_fast_dormancy_req.mui = 0;
      save_status = RRCDORMANCY_SUCCESS;
    }
    else
    {
      WRRC_MSG0_HIGH("RRCDORM:Multiple Dormancy Requests not allowed");  
    }
  }
  else
  {
    WRRC_MSG1_ERROR("RRCDORM:Invalid cmd 0x%x",cmd_ptr->cmd_hdr.cmd_id);
  }
  
  return save_status;
}

/*===========================================================================

FUNCTION rrcdormancy_send_cell_update_initiate_req

DESCRIPTION
  This function send the cell update initiate request if we are in
  URA_PCH or CELL_PCH states.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcdormancy_send_cell_update_initiate_req
(
  rrc_proc_e_type rrcdormancy_procedure
)
{
  rrc_cmd_type *int_cmd_ptr;
  //This initiates a CELL Update procedure and enters the substate
  //WAIT_FOR_CELL_UPDATE_COMP_CNF. 
  if( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {     
    /* Fill in the command parameters for RRC_CELL_UPDATE_INITIATE_REQ */
    int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
    int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_UL_DATA_TRANSMISSION;
    int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
    int_cmd_ptr->cmd.initiate_cell_update.procedure = rrcdormancy_procedure;
        
    rrc_put_int_cmd(int_cmd_ptr);             
  }
}/*rrcdt_send_cell_update_initiate_req*/

/*===========================================================================
  
  FUNCTION rrcdormancy_map_internal_cause_to_fast_dormancy_cnf_cause  
  
  DESCRIPTION
    This function maps internal cause of type rrcdormancy_status_e_type
    to cause to be sent in RRC_FAST_DORMANCY_CNF command to NAS
  
  DEPENDENCIES
    none.
  
  RETURN VALUE
    none 
===========================================================================*/
static rrc_fast_dormancy_cnf_status_e_type 
rrcdormancy_map_internal_cause_to_fast_dormancy_cnf_cause
(
  rrcdormancy_validation_status_e_type dormancy_cause
)
{
  rrc_fast_dormancy_cnf_status_e_type rrc_fast_dormancy_cnf_cause;
  switch(dormancy_cause)
  {
    case RRCDORMANCY_VALIDATION_SUCCESS:
    case RRCDORMANCY_VALIDATION_FAIL_CS_DOMAIN_OPEN:
    case RRCDORMANCY_VALIDATION_FAIL_UE_OOS:
    case RRCDORMANCY_VALIDATION_FAIL_T323_TIMER_RUNNING:
    case RRCDORMANCY_VALIDATION_FAIL:
    case RRCDORMANCY_VALIDATION_FAIL_UE_LEAVING_CONN_MODE_SCAN:
      rrc_fast_dormancy_cnf_cause = RRC_FAST_DORMANCY_FAILURE_APP_RETRY;
      break;

    case RRCDORMANCY_VALIDATION_FAIL_DOMAIN_NOT_OPEN:
    case RRCDORMANCY_VALIDATION_FAIL_RRC_CON_NOT_OPEN:
      rrc_fast_dormancy_cnf_cause = RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY;
      break;
           
    default:
      rrc_fast_dormancy_cnf_cause = RRC_FAST_DORMANCY_FAILURE_APP_RETRY;
      break;
  }
  WRRC_MSG2_HIGH("RRCDORM:dormancy_cause=%d,rrc_fast_dormancy_cnf_cause=%d",
    dormancy_cause,
    rrc_fast_dormancy_cnf_cause);
  return rrc_fast_dormancy_cnf_cause;
}
/*===========================================================================

FUNCTION rrcdormancy_send_scri_for_fast_dormancy

DESCRIPTION
  Process SCRI request as a result of processing Fast Dormancy.

DEPENDENCIES
  None.

RETURN VALUE
  void

===========================================================================*/
static void rrcdormancy_send_scri_for_fast_dormancy
(
boolean l2_ack_processing_required
)
{
  rrc_UL_DCCH_Message *msg_ptr;                 /* Pointer to uplink DCCH message */  
  rlc_lc_id_type ul_lc_id;                      /* Uplink logical channel ID */
  rrcsend_status_e_type send_status;            /* send status */
  uint8 msg_mui;                                /* MUI for the SCRR */
  
  /* Reset RLC activity_flag as we are sending SCRI now */
  rlc_reset_activity_flag();

  /* Check to see if there is any saved message and send SigConnReq */
  if(rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status == TRUE)
  {
    /* Determine the logical channel id for the Uplink DCCH */
    ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
      DCCH_AM_RADIO_BEARER_ID, UE_MODE_ACKNOWLEDGED_DATA);
      
    if (ul_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      /* The logical channel id was not found */
      WRRC_MSG0_HIGH("RB 2 not found by LCM");
      rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
      return;
    }
    else
    {
      /* Build a Signalling Connection Release Request Message. Start by allocating
       * an Uplink DCCH message. This memory is released by rrcsend_send_ul_sdu().
       */
      msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc(sizeof(struct rrc_UL_DCCH_Message));
      
      /* Call error fatal if the memory allocation failed */
      if (msg_ptr == NULL)
      {
        ERR_FATAL("malloc(%d) failure", sizeof(struct rrc_UL_DCCH_Message),0, 0);
      }
      
      /* Identify this message as a Signalling Connection Release Indication Message */
      msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_signallingConnectionReleaseIndication;
      
      /* Initialize the bit mask for Signalling Connection Release Indication Message */
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.signallingConnectionReleaseIndication);
      
      /* Set the CN Domain ID from the MM primitive */
      msg_ptr->message.u.signallingConnectionReleaseIndication.cn_DomainIdentity = rrc_CN_DomainIdentity_ps_domain;

      /*setting the IE "Signalling Connection Release Indication Cause" to "UE Requested PS Data session end"*/

      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.signallingConnectionReleaseIndication, laterNonCriticalExtensions);

      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.signallingConnectionReleaseIndication.laterNonCriticalExtensions);

      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.signallingConnectionReleaseIndication.laterNonCriticalExtensions, v860nonCriticalExtentions);

      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.signallingConnectionReleaseIndication.laterNonCriticalExtensions.v860nonCriticalExtentions);
  
      msg_ptr->message.u.signallingConnectionReleaseIndication.laterNonCriticalExtensions.
        v860nonCriticalExtentions.signallingConnectionReleaseIndication_v860ext.
        signallingConnectionReleaseIndicationCause = rrc_SignallingConnectionReleaseIndication_v860ext_signallingConnectionReleaseIndicationCause_uERequestedPSDataSessionEnd;
  
#ifdef FEATURE_QSH_EVENT_METRIC
      rrc_qsh_log_scri_params(msg_ptr->message.u.signallingConnectionReleaseIndication.laterNonCriticalExtensions.
        v860nonCriticalExtentions.signallingConnectionReleaseIndication_v860ext.
        signallingConnectionReleaseIndicationCause,RRC_PS_DOMAIN_CN_ID);
#endif
  
      /* Send the Signalling Connection Release Request Message on the Uplink DCCH */
      send_status = rrcsend_send_ul_sdu_with_mui(RRC_PROCEDURE_DORMANCY,msg_ptr,ul_lc_id,
                                                 RRCSEND_L2ACK_REQUIRED, &msg_mui);

      if(rrc_max_scri_for_fd != 0)
      {
        /* Decrement rrc_max_scri_for_fd as we have sent one SCRI */
        rrc_max_scri_for_fd --;
      }

      if(FALSE == l2_ack_processing_required)
      {
        WRRC_MSG1_HIGH("SigConRel for Pending Fast dormancy sent cause %d ", 
            rrcdormancy_saved_fast_dormancy_req.mui_status);

        rrcdormancy_init_saved_fast_dormancy_req();
        
        return;
      }

      if (send_status == RRCSEND_SUCCESS)
      {
        rrcdormancy_substate = RRCDORMANCY_WAIT_FOR_L2ACK;
        rrcdormancy_saved_fast_dormancy_req.mui= msg_mui;
  
        /* update the mui_status cause and also the flag to indicate that close_sess_req
               * or sig_con_rel_req has been already sent */
        rrcdormancy_saved_fast_dormancy_req.mui_status = RRCDORMANCY_MUI_SCRI_CNF; 

        WRRC_MSG1_HIGH("SigConRel for Fast dormancy sent cause %d ", 
                  rrcdormancy_saved_fast_dormancy_req.mui_status);
      }
      else
      {
        WRRC_MSG1_HIGH("Error %d sending SCRI for Fast dormancy", send_status);
        rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
      }
    }
  }
}/*rrcscrr_send_sig_conn_rel_req*/

/*===========================================================================
  
  FUNCTION rrcdormancy_reset_n_send_fast_dormancy_cnf  
  
  DESCRIPTION
    This function sends RRC_FAST_DORMANCY_CNF command to NAS and reset dormancy
  
  DEPENDENCIES
    none.
  
  RETURN VALUE
    none 
===========================================================================*/
void rrcdormancy_reset_n_send_fast_dormancy_cnf
(
  rrc_fast_dormancy_cnf_status_e_type cnf_cause
)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

  /* Allocate memory for RRC_FAST_DORMANCY_CNF command */
  if( (cmd_ptr =  mm_rrc_get_cmd_buf(RRC_FAST_DORMANCY_CNF)) != NULL)
  {
    /* Fill in parameters fro RRC_FAST_DORMANCY_CNF command */
    cmd_ptr->cmd.rrc_fast_dormancy_cnf.status = cnf_cause ;
#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rrc_fast_dormancy_cnf.as_id = rrc_get_as_id();
#endif

    rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */

    WRRC_MSG1_HIGH("RRCDORM:Sent RRC_FAST_DORMANCY_CNF to MM with status =%d / Dispatching out MM cmd",cnf_cause);

    rrcdormancy_init_saved_fast_dormancy_req();

  }
}

/*===========================================================================
  
  FUNCTION rrcdormancy_send_fast_dormancy_cnf  
  
  DESCRIPTION
    This function sends RRC_FAST_DORMANCY_CNF command to NAS without resetting dormancy.
  
  DEPENDENCIES
    none.
  
  RETURN VALUE
    none 
===========================================================================*/
static void rrcdormancy_send_fast_dormancy_cnf
(
  rrc_fast_dormancy_cnf_status_e_type cnf_cause
)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

  /* Allocate memory for RRC_FAST_DORMANCY_CNF command */
  if( (cmd_ptr =  mm_rrc_get_cmd_buf(RRC_FAST_DORMANCY_CNF)) != NULL)
  {
    /* Fill in parameters fro RRC_FAST_DORMANCY_CNF command */
    cmd_ptr->cmd.rrc_fast_dormancy_cnf.status = cnf_cause ;

#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rrc_fast_dormancy_cnf.as_id = rrc_get_as_id();
#endif

    rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */

    WRRC_MSG1_HIGH("RRCDORM:Sent RRC_FAST_DORMANCY_CNF to MM with status =%d / Dispatching out MM cmd",cnf_cause);
  }
}


/*===========================================================================
  
  FUNCTION 
    rrcdormancy_check_if_nw_support_fast_dormancy  

  DESCRIPTION
    This function check dormancy support of N/W

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
boolean rrcdormancy_check_if_nw_support_fast_dormancy
(
  void
)
{
  if(rrcdormancy_conn_timers_and_constants.T323_nw_support == TRUE)
    return TRUE;
  else
    return FALSE;
}

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_get_nw_support_of_fast_dormancy_from_sib  

  DESCRIPTION
    This function check dormancy support of N/W after SIB reading 

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
boolean rrcdormancy_get_nw_support_of_fast_dormancy_from_sib
(
  void
)
{
  rrc_SysInfoType1 *sib1_ptr;
  boolean status = FALSE;
  
  sib1_ptr = (rrc_SysInfoType1 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB1);
  if(sib1_ptr != NULL)
  {
    if(sib1_ptr->m.v3a0NonCriticalExtensionsPresent)
    {
      if(sib1_ptr->v3a0NonCriticalExtensions.m.v860NonCriticalExtentionsPresent)
      {
        if(sib1_ptr->v3a0NonCriticalExtensions.v860NonCriticalExtentions.sysInfoType1_v860ext.
          ue_ConnTimersAndConstants.m.t_323Present)
        {
          status = TRUE;
        }
      }
    }
  }
  return status;
}


/*===========================================================================
  
  FUNCTION 
    rrcdormancy_stop_n_update_t323_timer_status  

  DESCRIPTION
    This function stops gurard timer for SRNS Relocation and update status of
    T323 to IDLE/RUNNING

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
rrcdormancy_timer_status_e_type rrcdormancy_stop_n_update_t323_timer_status
(
  void
)
{
  rrcdormancy_timer_status_e_type status = RRCDORMANCY_T323_TIMER_INVALID;

  WRRC_MSG2_HIGH("Current T323 VALUE: %d STATUS: %d",rrcdormancy_conn_timers_and_constants.T323_duration,
    rrcdormancy_conn_timers_and_constants.T323_current_status);  

  switch(rrcdormancy_conn_timers_and_constants.T323_current_status)
  {
    case RRCDORMANCY_SRNS_GUARD_TIMER_RUNNING:
      rrctmr_stop_timer(RRCTMR_T_323_TIMER);

    /* Do nothing for other status */ 
    case RRCDORMANCY_T323_TIMER_INVALID:
    case RRCDORMANCY_T323_TIMER_IDLE:
      status = RRCDORMANCY_T323_TIMER_IDLE;
      break;
      
    case RRCDORMANCY_T323_TIMER_RUNNING:
      status = RRCDORMANCY_T323_TIMER_RUNNING;
      break;      
  }  
  

  return status;
}

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_disable_fast_dormancy  

  DESCRIPTION
    This function disables dormancy support of network

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
void rrcdormancy_disable_fast_dormancy
(
void
)
{
  rrcdormancy_conn_timers_and_constants.T323_nw_support = FALSE;
  rrcdormancy_conn_timers_and_constants.T323_duration = rrc_T_323_s0;
  rrcdormancy_conn_timers_and_constants.T323_current_status = RRCDORMANCY_T323_TIMER_INVALID;
}

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_reset_v316  

  DESCRIPTION
    This function reset V316 counter.

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
static void rrcdormancy_reset_v316
(
void
)
{
  WRRC_MSG0_HIGH("Resetting V316");
  rrc_v_316 = 0;
}

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_check_drx_cycle_n_v316_to_send_scri  

  DESCRIPTION
    This function checks drx cycle and decides whether scri needs to be send or not. The function returns TRUE
    if scri needs to be sended else returns FALSE.

  DEPENDENCIES
    none.

  RETURN VALUE
    boolean
===========================================================================*/
static boolean rrcdormancy_check_drx_cycle_n_v316_to_send_scri
(
void
)
{
  uint32 cs_drx=0, ps_drx=0, drx_in_use=0, shorter_cn_drx=0;

  rrcllc_get_drx_info_values(&cs_drx, &ps_drx, &drx_in_use);

  /* We may not have a UTRAN DRX cycle length, so we can send SCRI */
  if(drx_in_use == 0)
  {
    WRRC_MSG0_HIGH("drx_in_use is 0, returning TRUE");
    return TRUE;
  }

  if(cs_drx < ps_drx)
    shorter_cn_drx = cs_drx;
  else
    shorter_cn_drx = ps_drx;

  WRRC_MSG2_HIGH("shorter_cn_drx:%d rrc_v_316:%d",shorter_cn_drx,rrc_v_316);

  /* DRX cycle length in use is shorter than the shorter CN domain specific DRX 
     * cycle length for the PS domain and CS domain */
  if(drx_in_use < shorter_cn_drx)
  {
    return TRUE;
  }
  else 
  {
    /* DRX cycle length in use is equal to or longer than the shorter CN domain specific 
         * DRX cycle length for the PS domain and CS domain, and V316 < 1*/
    if(rrc_v_316 < 1)
    {
      /* Increment V316 by 1*/
      rrc_v_316++;
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
}

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_check_n_reset_v316  

  DESCRIPTION
    This function checks the conditions for resetting v316 counter and reset
    if required.

  DEPENDENCIES
    none.

  RETURN VALUE
    none
===========================================================================*/
void rrcdormancy_check_n_reset_v316
(
  rrc_proc_e_type rrc_procedure
)
{
  uint8 rab_idx = 0;
  uint32 rb_count = 0;
  uint32 local_rlc_wm_size=0;
  rrc_RB_Identity local_rb_id=0;
  uint8 local_lc_id=0;
  rrc_state_e_type curr_state = rrc_get_state();
  
  if((curr_state == RRC_STATE_CELL_PCH) ||
      (curr_state == RRC_STATE_URA_PCH ))
  {
    for(rab_idx = 0; rab_idx < MAX_RAB_TO_SETUP; rab_idx++)
    {
      if((rrc_est_rabs.rabs[rab_idx].rab_id != RRC_INVALID_RAB_ID) && 
          (rrc_est_rabs.rabs[rab_idx].cn_domain == RRC_PS_DOMAIN_CN_ID) &&
          (rrc_est_rabs.rabs[rab_idx].num_rbs_for_rab > 0))
      {
        WRRC_MSG1_HIGH("PS domain RAB:%d",rrc_est_rabs.rabs[rab_idx].rab_id);
        for (rb_count=0; rb_count<rrc_est_rabs.rabs[rab_idx].num_rbs_for_rab; rb_count++)
        {
          local_rb_id = rrc_est_rabs.rabs[rab_idx].rb_for_rab[rb_count].rb_id;
          local_lc_id = rrclcm_get_ul_rlc_lc_id(local_rb_id);
          local_rlc_wm_size = rlc_rrc_get_wm_info_req(local_lc_id);
          
          MSG_HIGH("RB:%d LC_ID:%d WM_SIZE:%d",local_rb_id,local_lc_id,local_rlc_wm_size);

          if((rrc_procedure == RRC_PROCEDURE_IDT) || (local_rlc_wm_size > 0))
          {
            /* Reset V316 and return */ 
            rrcdormancy_reset_v316();
            return;
          }
        }
      }
    }
  }
}

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_update_dormancy_for_srns_hho_cco  

  DESCRIPTION
    This function updates dormacy procedure if SRNS relocation is in progress. 
    If so it will abort & send RRC_FAST_DORMANCY_CNF command to NAS.

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
void rrcdormancy_update_dormancy_for_srns_hho_cco
(
  rrc_proc_e_type rrc_proc
)
{
  WRRC_MSG1_HIGH("function rrcdormancy_update_dormancy_for_srns_hho_cco is called from PROC:rrc_proc_e_type_value%d",rrc_proc);

  /* Reset V316 */ 
  rrcdormancy_reset_v316();

  switch(rrc_proc)
  {
    case RRC_PROCEDURE_UMI: 
      {
        if(FALSE == rrcdormancy_check_if_nw_support_fast_dormancy())        
        {
          /* Falling back to legacy SCRI. Stop T323 timer if it is running */
          rrcdormancy_stop_or_restart_timer_T323(RRCDORM_STOP_T323_TIMER_VALUE,
            RRCDORMANCY_T323_TIMER_INVALID);

          /* It is expected that UE doesnt receive l2 ack for the SCRI which is sent as network is sending UMI */
          if(rrcdormancy_substate == RRCDORMANCY_WAIT_FOR_SIG_CON_REL_COMPL)
          {
            rrcscrr_abort_ps_rel_proc();
          }

          rrcdormancy_check_and_abort_dormancy_req(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
        }
      }
      break;  
    case RRC_PROCEDURE_RBE:      
    case RRC_PROCEDURE_RBRC:
    case RRC_PROCEDURE_PCR:       
    case RRC_PROCEDURE_TCR: 
    case RRC_PROCEDURE_RBR:        
    case RRC_PROCEDURE_CU: 
    case RRC_PROCEDURE_CHO:      
    case RRC_PROCEDURE_IHO:      
      {

        if((rrc_proc != RRC_PROCEDURE_CHO) &&
           (rrc_proc != RRC_PROCEDURE_IHO) &&
           (RRC_STATE_CELL_FACH == rrc_get_state())&&
           (TRUE == rrcdormancy_get_nw_support_of_fast_dormancy_from_sib()))
        {
          WRRC_MSG0_HIGH("SRNS NEW RNC supports 3GPP Dormancy");
        }
        else
        {
          WRRC_MSG0_HIGH("SRNS NEW RNC support for 3GPP Dormancy Unknown");
          /* It is not expected to receive l2 ack for SCRI message sent out of RRC */
          if(rrcdormancy_substate == RRCDORMANCY_WAIT_FOR_SIG_CON_REL_COMPL)
          {
            rrcscrr_abort_ps_rel_proc();
          }
          rrcdormancy_check_and_abort_dormancy_req(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);    
          /* Disable Fast Dormancy till N/W indicates support of Fast Dormancy */
          rrcdormancy_disable_fast_dormancy();
          /* If T323 is not running START it with SRNS guard timer duration */
          rrcdormancy_stop_or_restart_timer_T323(RRCDORM_SRNS_GUARD_TIMER_VALUE,
            RRCDORMANCY_SRNS_GUARD_TIMER_RUNNING);
        }
      }      
      break;

    default:
      WRRC_MSG1_HIGH("Invalid Proc rrc_proc_e_type_value%d in update_dormancy_for_srns",rrc_proc);
  }
}

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_get_T323_timer_value  

  DESCRIPTION
    This function check returns the value of T323 timer provided by Network
    in milliseconds.

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
static uint32 rrcdormancy_get_T323_timer_value
(
  void
)
{
  uint32 ret_val = 0;

  if(rrcdormancy_conn_timers_and_constants.T323_nw_support == TRUE)
  {
    switch(rrcdormancy_conn_timers_and_constants.T323_duration)
    {
      case rrc_T_323_s0:
        ret_val = 0;
        break;

      case rrc_T_323_s5:
        ret_val = 5000;
        break;

      case rrc_T_323_s10:
        ret_val = 10000;
        break;

      case rrc_T_323_s20:
        ret_val = 20000;
        break;

      case rrc_T_323_s30:
        ret_val = 30000;
        break;

      case rrc_T_323_s60:
        ret_val = 60000;
        break;

      case rrc_T_323_s90:
        ret_val = 90000;
        break;

      case rrc_T_323_s120:
        ret_val = 120000;
        break;        

      default:
        ret_val = 0;
    }
  }
  return ret_val;
}

/*===========================================================================
  
  FUNCTION rrcdormancy_process_T323_expiry  
  
  DESCRIPTION
 This function process T323 timer expiry.
  
  DEPENDENCIES
    none.
  
  RETURN VALUE
    none 
===========================================================================*/
static void rrcdormancy_process_T323_expiry
(
  rrc_cmd_type *cmd_ptr
)
{

  /* In case PS RAB is not yet up and T323 expires there is a chance that PS RAB establishment 
   * is in progress for uplink/downlink data and we may wrongly think that there is no data 
   * leading to sending of SCRI in the uplink which will not be anticipated by the NW. Therefore
   * check for the presence of PS RAB before triggering SCRI 
   */
  if((rlc_query_data_channel_activity() == NO_DATA_TRANSFERRED) && rrc_is_ps_domain_rab_present())
  {
    /* No data activity, mark num_pending_dormancy_req to TRUE */
    WRRC_MSG1_HIGH("RRCDORM: RRC state %d",rrc_get_state());
    if(((rrc_get_state() == RRC_STATE_CELL_DCH) || 
        (rrc_get_state() == RRC_STATE_CELL_FACH)) &&
       (rrc_max_scri_for_fd != 0)
      )
    {
      WRRC_MSG1_HIGH("RRCDORM: in %d (FACH/DCH) increment pending ",rrc_get_state());
      rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req++;
    }
    else if(((rrc_get_state() == RRC_STATE_URA_PCH) || 
             (rrc_get_state() == RRC_STATE_CELL_PCH)) &&
             (rrc_max_scri_for_fd != 0)
           )
    {
      WRRC_MSG1_HIGH("RRCDORM: in %d(PCH) increment pending and set max_scri ",rrc_get_state());
      rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req++;
      return;
    }
  }
  else
  {
    /* There is some data activity on PS RB, reset num_pending_dormancy_req */
    rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req =0;
  }

  if(rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req != 0)
  {
    WRRC_MSG1_HIGH("Pending Dormancy requests:%d",rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req);
    /* If all validation goes well then process fast dormancy req and start T323 timer*/
    if(TRUE == rrcdormancy_process_pending_fast_dormancy_req())
    {
      if(TRUE == rrcdormancy_check_if_nw_support_fast_dormancy())
      {
        rrcdormancy_stop_or_restart_timer_T323(rrcdormancy_get_T323_timer_value(),
          RRCDORMANCY_T323_TIMER_RUNNING); 
        return;
      }
      else
      {
        WRRC_MSG0_HIGH("N/W dint support T323. Do Not Start timer");
      } 
    }
  }
  
  if(TRUE == rrcdormancy_check_if_nw_support_fast_dormancy())
    rrcdormancy_conn_timers_and_constants.T323_current_status = RRCDORMANCY_T323_TIMER_IDLE;
  else
    rrcdormancy_conn_timers_and_constants.T323_current_status = RRCDORMANCY_T323_TIMER_INVALID;

  WRRC_MSG1_HIGH("Resetting T323 Status: %d",rrcdormancy_conn_timers_and_constants.T323_current_status);
}

/*===========================================================================
  
  FUNCTION rrcdormancy_stop_or_restart_timer_T323  

  DESCRIPTION
    This function check if T323 is already running then STOP it and restart with the given time.
    IF timer_value == 0, then it STOPS it.
    
  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
void rrcdormancy_stop_or_restart_timer_T323
(
  unsigned long timer_value,
  rrcdormancy_timer_status_e_type timer_status
)
{
  uint64 remaining_value = 0;
  
  WRRC_MSG2_HIGH("Current T323 VALUE: %d STATUS: %d",rrcdormancy_conn_timers_and_constants.T323_duration,
    rrcdormancy_conn_timers_and_constants.T323_current_status);  

  if(timer_value == RRCDORM_STOP_T323_TIMER_VALUE)
  {
    if(0 != rrctmr_get_remaining_time(RRCTMR_T_323_TIMER))
    {
       rrctmr_stop_timer(RRCTMR_T_323_TIMER);
    }
    rrcdormancy_conn_timers_and_constants.T323_current_status = timer_status;

    WRRC_MSG1_HIGH("STOP T323, STATUS: %d",timer_status);  
    return;
  }
    
  remaining_value = rrctmr_get_remaining_time(RRCTMR_T_323_TIMER);

  /* start T323 timer with value of 3 seconds if T323 value given by network is 0 */
  if(timer_value == 0)
  {
    timer_value = RRC_DORMANCY_T323_RETRY_TIMER;
  }

  if(remaining_value < timer_value)
  {
    if(remaining_value != 0)
    {
      rrctmr_stop_timer(RRCTMR_T_323_TIMER);
    }
    rrctmr_start_timer(RRCTMR_T_323_TIMER, timer_value);
    
   rrcdormancy_conn_timers_and_constants.T323_current_status = timer_status;    

   WRRC_MSG2_HIGH("START T323, VALUE: %d STATUS: %d",timer_value,timer_status);  
  } 
  
}

/*===========================================================================
  
  FUNCTION rrcdormancy_check_and_abort_dormancy_req  
  
  DESCRIPTION
 This function check if dormancy req is in progress. if so it will abort & send 
 RRC_FAST_DORMANCY_CNF command to NAS.
  
  DEPENDENCIES
    none.
  
  RETURN VALUE
    none 
===========================================================================*/
void rrcdormancy_check_and_abort_dormancy_req
(
  rrc_fast_dormancy_cnf_status_e_type abort_type
)
{
  if((rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status == TRUE) ||
     (rrcdormancy_saved_fast_dormancy_req.mui_status != RRCDORMANCY_MUI_STATUS_INVALID))
  {
    WRRC_MSG0_HIGH("Dormancy_req is in progress,lets abort");
    rrcdormancy_reset_n_send_fast_dormancy_cnf(abort_type);
  }
  else
  {
    WRRC_MSG0_HIGH("No dormancy_req in progress");
  }
}

/*===========================================================================
  
  FUNCTION rrcdormancy_check_dormancy_initiated_scrr_active  
  
  DESCRIPTION
 This function check whether dormancy initiated SCRR is active or NOT.
  
  DEPENDENCIES
    none.
  
  RETURN VALUE
    none 
===========================================================================*/
boolean rrcdormancy_check_dormancy_initiated_scrr_active
(
  void
)
{
  if((rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status == TRUE) &&
     (rrcdormancy_substate == RRCDORMANCY_WAIT_FOR_SIG_CON_REL_COMPL) &&
     (rrcdormancy_saved_fast_dormancy_req.mui_status == RRCDORMANCY_MUI_SIG_CON_REL_CNF))
  {
    WRRC_MSG0_HIGH("Dormancy initiated SCRI in progress");    
    return TRUE;
  }
  else
    return FALSE;
}

/*===========================================================================
  
  FUNCTION rrcdormancy_is_dormancy_active  
  
  DESCRIPTION
 This function check if dormancy req is ACTIVE or NOT.
  
  DEPENDENCIES
    none.
  
  RETURN VALUE
    none 
===========================================================================*/
boolean rrcdormancy_is_dormancy_active
(
  void
)
{
  if((rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status == TRUE) ||
     (rrcdormancy_saved_fast_dormancy_req.mui_status != RRCDORMANCY_MUI_STATUS_INVALID))
  {
    WRRC_MSG0_HIGH("Dormancy is Active");
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================
  
  FUNCTION rrcdorm_check_n_send_cnf_for_failure_conditions  
  
  DESCRIPTION
 This function check if dormancy req is in progress. if so it will abort & send 
 RRC_FAST_DORMANCY_CNF command to NAS.
  
  DEPENDENCIES
    none.
  
  RETURN VALUE
    boolean 
===========================================================================*/
boolean rrcdorm_check_n_send_cnf_for_failure_conditions
(
  void
)
{
  rrc_state_e_type rrc_state = rrc_get_state();
  
  if((TRUE == rrcrb_is_srns_in_progress()) ||
     (TRUE == rrcsmc_is_rlc_re_est_for_srns_active()))
  {
    WRRC_MSG0_HIGH("SRNS in progress. Send FAILURE for Dormancy");
    
    rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);    
    return TRUE;    
  }

  if(FALSE == rrcscrr_is_cn_domain_open(RRC_PS_DOMAIN_CN_ID))
  {
    WRRC_MSG2_HIGH("SCRR active for PS domain. Aborting Dormancy dorm-state:%d rrc-state:%d",
      rrcdormancy_substate, rrc_state);
    rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);    
    return TRUE;
  }
  else if(TRUE == is_iho_w2g_active())
  {
    WRRC_MSG2_HIGH("IRAT-HO active. Aborting Dormancy dorm-state:%d rrc-state:%d",
      rrcdormancy_substate, rrc_state);
   
    rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);    
    return TRUE;
  }
  else if(TRUE == rrcidt_is_idt_active(RRC_PS_DOMAIN_CN_ID))
  {
    WRRC_MSG2_HIGH("IDT-PS is active.  Aborting Dormancy dorm-state:%d rrc-state:%d",
      rrcdormancy_substate, rrc_state);
    rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
    return TRUE;
  }
  else if(TRUE == rrcidt_is_idt_active(RRC_CS_DOMAIN_CN_ID))
  {
    WRRC_MSG2_HIGH("IDT-CS is active.  Aborting Dormancy dorm-state:%d rrc-state:%d",
      rrcdormancy_substate, rrc_state);
    rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
    return TRUE;
  }
  else if(TRUE == rrccho_is_wtog_cco_active())
  {
    WRRC_MSG2_HIGH("CCO is active.  Aborting Dormancy dorm-state:%d rrc-state:%d",
      rrcdormancy_substate, rrc_state);
    
    rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
    return TRUE;    
  }

  return FALSE;
}

/*===========================================================================
  
  FUNCTION rrcdormancy_validate_fast_dormancy_req  
  
  DESCRIPTION
    This function validates RRC_FAST_DORMANCY_REQ command received from MM task.
  
  DEPENDENCIES
    This function should be called only if the command is RRC_FAST_DORMANCY_REQ .
  
  RETURN VALUE
    rrcdormancy_status_e_type: It returns RRCDORMANCY_VALIDATION_SUCCESS if it is successfully validates 
    the RRC_FAST_DORMANCY_REQ command. Otherwise it returns fail cause
  
===========================================================================*/
static rrcdormancy_validation_status_e_type rrcdormancy_validate_fast_dormancy_req
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrcdormancy_validation_status_e_type status = RRCDORMANCY_VALIDATION_SUCCESS; 
  rrc_csp_scan_e_type curr_scan = rrccsp_get_curr_scan();

  if(cmd_ptr->cmd_hdr.cmd_id != RRC_FAST_DORMANCY_REQ)
  {
    WRRC_MSG1_ERROR("RRCDORM:Invalid cmd 0x%x",cmd_ptr->cmd_hdr.cmd_id);
    return (RRCDORMANCY_VALIDATION_FAIL);
  }

  /* Check to see if a signalling session to the requested PS domain is open */
  if ((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
      (rrcscrr_is_cn_domain_open(RRC_PS_DOMAIN_CN_ID) == FALSE))
  {
    WRRC_MSG0_ERROR("RRCDORM:PS domain not open");
    status = RRCDORMANCY_VALIDATION_FAIL_DOMAIN_NOT_OPEN;
  }
  
  /* Check to see if a signalling session to the requested PS domain is open */
  if ((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
      (rrcscrr_is_cn_domain_open(RRC_CS_DOMAIN_CN_ID) == TRUE)&&
      (TRUE == rrcdormancy_check_if_nw_support_fast_dormancy()))
  {
    WRRC_MSG0_ERROR("RRCDORM:CS domain is also open");
    status = RRCDORMANCY_VALIDATION_FAIL_CS_DOMAIN_OPEN;
  }
  
  /* Lets check if RRC is in IDLE mode */
  if ((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
       (rrc_get_state() == RRC_STATE_DISCONNECTED))
  {
    WRRC_MSG0_ERROR("RRCDORM:RRC in IDLE");
    status = RRCDORMANCY_VALIDATION_FAIL_RRC_CON_NOT_OPEN;
  }

  /* Lets check if RRC is in URA_PCH mode */
  if ((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
       (rrc_get_state() == RRC_STATE_URA_PCH)&&
       (FALSE == rrcdormancy_check_if_nw_support_fast_dormancy()))
  {
    WRRC_MSG0_ERROR("RRCDORM:RRC in URA_PCH");
    status = RRCDORMANCY_VALIDATION_FAIL_RRC_CON_NOT_OPEN;
  }

  /*Lets check if RRC connection is getting release */
  if ((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
       ((rrcrce_get_rrc_connection_status() == RRCRCE_RRC_CONN_NOT_ESTABLISHED)||
        (rrcrcr_is_proc_active() == TRUE)))
  {
    WRRC_MSG2_ERROR("RRCDORM:RRC connection not open[RCE state-%d,RCR state-%d]",
      rrcrce_get_rrc_connection_status(),
      rrcrcr_is_proc_active());
    status = RRCDORMANCY_VALIDATION_FAIL_RRC_CON_NOT_OPEN;
  }
    
   /*  DCH_TO_FACH check is introduced because RRC is rejecting the FD request
       and this happens when app level timer expiry colliding with NW level timer expiry 
       thereby UE not sending SCRI */
  if((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
     (RRC_NOT_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_DORMANCY))&& 
     (!(curr_scan == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN)))
  {
    /* For FAST  DORMANCY feature, If UE is in OOS, 
        * Then return RRC_FAST_DORMANCY_CNF with failuer cause 
        * NAS needs to return the FAST DORMANCY status Immediately to upper layer
        */
    WRRC_MSG0_ERROR("RRCDORM:UE in OOS");
    status = RRCDORMANCY_VALIDATION_FAIL_UE_OOS;
  }

  if((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
     (curr_scan == RRC_CSP_LEAVING_CONN_MODE_SCAN))
  {
    WRRC_MSG0_HIGH("UE is LeavingConnModeScan");
    status = RRCDORMANCY_VALIDATION_FAIL_UE_LEAVING_CONN_MODE_SCAN;
  }

  if((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
     (0 != rrctmr_get_remaining_time(RRCTMR_T_323_TIMER))
    )
  {
    /* For FAST  DORMANCY feature, If T323 timer is running, 
        * Then return RRC_FAST_DORMANCY_CNF with failure cause 
        * NAS needs to return the FAST DORMANCY status Immediately to upper layer
        */
    WRRC_MSG2_ERROR("Timer T323 is running RRC State:%d Dormancy Substate",
      rrc_get_state(), rrcdormancy_substate);

    rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req++;

    status = RRCDORMANCY_VALIDATION_FAIL_T323_TIMER_RUNNING;
  }

  
  /*Fast dormancy feature disabled*/
  if((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
     (rrc_dormancy_support_nv == RRC_NV_DORMANCY_DISABLED)
    )
  {
    status = RRCDORMANCY_VALIDATION_FAIL_RRC_CON_NOT_OPEN;
  }

  /*Network doesnt support FD and legacy dormancy disabled*/
  if((status == RRCDORMANCY_VALIDATION_SUCCESS) &&
     ((rrc_dormancy_support_nv == RRC_NV_LEGACY_DORMANCY_DISABLED)&&
      (FALSE == rrcdormancy_check_if_nw_support_fast_dormancy()))
    )
  {
    status = RRCDORMANCY_VALIDATION_FAIL;
  }

  if(rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status == TRUE)
  {
    WRRC_MSG0_ERROR("RRCDORM:One fast_dormancy_req is in progress,2nd req not supported");
    status = RRCDORMANCY_VALIDATION_FAIL;
  }

  return (status);
} /* rrcdormancy_validate_fast_dormancy_req */

/*===========================================================================
  
  FUNCTION rrcdormancy_send_sig_conn_rel_req
  
  DESCRIPTION
    This function sends RRC_SIG_CONN_RELEASE_REQ to DT
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
  ===========================================================================*/
static void rrcdormancy_send_sig_conn_rel_req
(
  void
)
{
  rrc_cmd_type *rrcdormancy_sig_conn_req_ptr; 


  /* Allocates the buffer to RRC Internal command */
  if((rrcdormancy_sig_conn_req_ptr =  rrc_get_int_cmd_buf()) != NULL)
  { 
    rrcdormancy_sig_conn_req_ptr->cmd_hdr.cmd_id = RRC_SIG_CONN_RELEASE_REQ;
    rrcdormancy_sig_conn_req_ptr->cmd.rrc_sig_conn_req.cn_domain_id =
      RRC_PS_DOMAIN_CN_ID;
    rrcdormancy_sig_conn_req_ptr->cmd.rrc_sig_conn_req.sig_rel_complete_cnf_required = 
      TRUE;
    rrcdormancy_sig_conn_req_ptr->cmd.rrc_sig_conn_req.rrc_proc = 
      RRC_PROCEDURE_DORMANCY;
    /* sends the command to Signalling Connection Release Request Proc */
    rrc_put_int_cmd(rrcdormancy_sig_conn_req_ptr);   
  }

  rrcdormancy_substate = RRCDORMANCY_WAIT_FOR_SIG_CON_REL_COMPL;
  rrcdormancy_saved_fast_dormancy_req.mui_status = RRCDORMANCY_MUI_SIG_CON_REL_CNF;
}


/*===========================================================================
  
  FUNCTION rrcdormancy_process_fast_dormancy_req
  
  DESCRIPTION
    This function processes fast_dormancy_req
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
 ===========================================================================*/
static boolean rrcdormancy_process_fast_dormancy_req
(
  void
)
{
  boolean status = FALSE;
  
  if(TRUE == rrcdorm_check_n_send_cnf_for_failure_conditions())
  {
    /* Code to send _DORMANCY_CNF is inside the function */
    return FALSE;
  }

  if(FALSE == rrcdormancy_check_if_nw_support_fast_dormancy())
  {
    WRRC_MSG0_HIGH("NW Didn't suppport Dormancy.");
    rrcdormancy_send_sig_conn_rel_req();
    return FALSE;
  }

  switch(rrc_get_state())
  {
	case RRC_STATE_URA_PCH:
	case RRC_STATE_CELL_PCH:
    if(FALSE == rrcdormancy_check_drx_cycle_n_v316_to_send_scri())
    {
      WRRC_MSG1_HIGH("Drop SCRI in rrc_state:%d ", rrc_get_state());
      rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
      return FALSE;
    }
    if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_DORMANCY))
    {
      /* Wait until Cell update procedure is completed */
      rrcdormancy_substate = RRCDORMANCY_WAIT_FOR_CU_COMP_CNF;  
    }
    else /* Cell Update procedure is inactive */
    {
#ifdef FEATURE_WCDMA_REL7_SPECRS 
      /*Check if T320 timer is running then send cnf with failure 
           * Till this timer is running, CU wont be initiated */
      if(0 != rrctmr_get_remaining_time(RRCTMR_T_320_TIMER))
      {
        /* CU guard Timer is running */
        WRRC_MSG2_ERROR("CU Guard Timer T320 is running RRC State:%d Dormancy Substate",
        rrc_get_state(), rrcdormancy_substate);        
        rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
      }
      else 
#endif
      {
        /* If Dormancy request is sent recently then defer next dormancy till T323 expiry */
        if(rrctmr_get_remaining_time(RRCTMR_T_323_TIMER))
        {
          rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
          rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req++;
        }
        else
        {
          /* Cell Update with uplink data transmission */
          rrcdormancy_send_cell_update_initiate_req(RRC_PROCEDURE_DORMANCY);
          rrcdormancy_substate = RRCDORMANCY_WAIT_FOR_CU_COMP_CNF;  
        }
      }
    }    
    break;
  
  case RRC_STATE_CELL_DCH:
  case RRC_STATE_CELL_FACH:
    if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_DORMANCY))
    {
      /* Wait until Cell update procedure is completed */
      rrcdormancy_substate = RRCDORMANCY_WAIT_FOR_CU_COMP_CNF;  
    }
    else /* Cell Update procedure is inactive */
    {
      /* Build and send the Signalling Connection Release Request to Send Chain */            
      /* Re try SCRI only if rrc_max_scri_for_fd is non zero */
      if(rrc_max_scri_for_fd != 0)
      {
        rrcdormancy_send_scri_for_fast_dormancy(TRUE);
      }
      status = TRUE;
    }    
    break;

  default:
    WRRC_MSG1_HIGH("SCRR:Invalid Event in rrc state: %d",rrc_get_state());
    break;  
  }  

  return status;
}


/*===========================================================================
  
  FUNCTION rrcdormancy_process_pending_fast_dormancy_req
  
  DESCRIPTION
    This function processes rrcdormancy_process_pending_fast_dormancy_req
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    boolean.
  
 ===========================================================================*/
static boolean rrcdormancy_process_pending_fast_dormancy_req
(
  void
)
{
  boolean status = FALSE;
  
  WRRC_MSG2_HIGH("Pending SCRR: %d RRC State:%d",rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req,
      rrc_get_state());

  if( (TRUE == rrcrb_is_srns_in_progress()) 
      || (TRUE == rrcsmc_is_rlc_re_est_for_srns_active())
      || (FALSE == rrcscrr_is_cn_domain_open(RRC_PS_DOMAIN_CN_ID))
      || (TRUE == is_iho_w2g_active())
      || (TRUE == rrcidt_is_idt_active(RRC_PS_DOMAIN_CN_ID))
      || (TRUE == rrcidt_is_idt_active(RRC_CS_DOMAIN_CN_ID))
      || (TRUE == rrccho_is_wtog_cco_active())
      || (FALSE == rrcdormancy_check_if_nw_support_fast_dormancy())
      || (TRUE == rrcscrr_is_cn_domain_open(RRC_CS_DOMAIN_CN_ID))
    )
  {
    rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req = 0;
    WRRC_MSG0_HIGH("Aborting pending SCRR");
    return FALSE;
  }

  switch(rrc_get_state())
  {
    case RRC_STATE_URA_PCH:
    case RRC_STATE_CELL_PCH:
      WRRC_MSG1_HIGH("RRC State %d Aborting pending SCRR",rrc_get_state());
      rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req = 0;
      break;
      
    case RRC_STATE_CELL_DCH:
    case RRC_STATE_CELL_FACH:
      if(RRCCU_STARTED == rrccu_get_cell_update_started_status())
      {
        WRRC_MSG0_HIGH("CU Started Aborting pending SCRR");
        rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req = 0;
      }
      else 
      {
        /* Check if dormancy already in progress */
        if((rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status == TRUE) ||
            (rrcdormancy_saved_fast_dormancy_req.mui_status != RRCDORMANCY_MUI_STATUS_INVALID))
        {
          /* Do not process pending dormancy requests */
          WRRC_MSG0_HIGH("Fast Dormancy Active. Aborting pending SCRR");
          rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req = 0;
        }
        else
        {
          /* Build and send the Signalling Connection Release Request to Send Chain */
          WRRC_MSG0_HIGH("Sending pending SCRR");
          rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status = TRUE;

        /* Re try SCRI only if rrc_max_scri_for_fd is non zero */
          if(rrc_max_scri_for_fd != 0)
          {
            rrcdormancy_send_scri_for_fast_dormancy(FALSE);
          }
          status = TRUE;
        }
      }    
      break;

    default:
      WRRC_MSG1_HIGH("Pending SCRR:Invalid Event in rrc state: %d",rrc_get_state());
      rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req = 0;
      break;
  }  
  rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req = 0;
  return status;
}

/*===========================================================================
  
  FUNCTION rrcdormancy_validate_and_process_fast_dormancy_req
  
  DESCRIPTION
    This function processes rrcdormancy_validate_and_process_fast_dormancy_req
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
  ===========================================================================*/
  
static void rrcdormancy_validate_and_process_fast_dormancy_req
(
  rrc_cmd_type *cmd_ptr
)
{
  rrcdormancy_validation_status_e_type validation_status;
  /* Set rrc_max_scri_for_fd to MAX */
  rrc_max_scri_for_fd = RRC_DORMANCY_MAX_SCRI_RETRY;


  rlc_reset_activity_flag();

  if((validation_status = rrcdormancy_validate_fast_dormancy_req(cmd_ptr)) != 
     RRCDORMANCY_VALIDATION_SUCCESS)
  {
    rrcdormancy_reset_n_send_fast_dormancy_cnf(
    rrcdormancy_map_internal_cause_to_fast_dormancy_cnf_cause(validation_status)
    );
    return;
  }

  if(RRCDORMANCY_SUCCESS != rrcdormancy_save_fast_dormancy_req(cmd_ptr))
  {
    /* If there are multiple dormancy requests, then just trash it */
    WRRC_MSG2_HIGH("RRCDORM:Failed to process Fast Dormancy Req in: %d, %d",
                rrc_get_state(),rrcdormancy_substate);
    rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);
    return;
  }

  /* If all validation goes well then process fast dormancy req and start T323 timer*/
  if(TRUE == rrcdormancy_process_fast_dormancy_req())
  {
    if(TRUE == rrcdormancy_check_if_nw_support_fast_dormancy())
    {
      rrcdormancy_stop_or_restart_timer_T323(rrcdormancy_get_T323_timer_value(),
        RRCDORMANCY_T323_TIMER_RUNNING); 
    }
    else
    {
      WRRC_MSG0_HIGH("N/W dint support T323. Do Not Start timer");
    }    
  }
}
/*===========================================================================
  
  FUNCTION rrcdormancy_process_wait_for_l2_ack_substate
  
  DESCRIPTION
    This function processes all events that are  dispatched
    to fast dormancy  procedure. in RRCDORMANCY_WAIT_FOR_L2ACK
    substate
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
  ===========================================================================*/
static void rrcdormancy_process_wait_for_l2_ack_substate
(
  rrc_cmd_type *cmd_ptr
)
{
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_L2ACK_IND:
      if((rrcdormancy_saved_fast_dormancy_req.mui_status == RRCDORMANCY_MUI_SCRI_CNF) && 
         (cmd_ptr->cmd.downlink_l2ack_ind.mui == rrcdormancy_saved_fast_dormancy_req.mui))
      {
        MSG_HIGH("L2 ACK for SCRI Status:%d, mui_status:%d, mui=%d",
          cmd_ptr->cmd.downlink_l2ack_ind.status,
          rrcdormancy_saved_fast_dormancy_req.mui_status,
          rrcdormancy_saved_fast_dormancy_req.mui);

        //send fast-dormancy-cnf status based on l2-ack status
        if(SUCCESS == cmd_ptr->cmd.downlink_l2ack_ind.status)
        {
          rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);
        }
        else
        {          
          rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
          WRRC_MSG0_HIGH("L2 ACK Failed for SCRI. Mark Pending to be sent later");
          rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req++;
        }    
      }
      else
      {
        WRRC_MSG0_ERROR("Invalid L2 ACK for Fast Dormancy SCRI is rcvd");
      }
      break;

    case RRC_FAST_DORMANCY_REQ:
      WRRC_MSG2_HIGH("RRCDORM:Invalid Event: 0x%x for dormancy in substate:%d", 
          cmd_ptr->cmd_hdr.cmd_id,rrcdormancy_substate);
      /* Do not process next dormancy req till first one complets */
      rrcdormancy_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
      break;

    default:
      WRRC_MSG2_HIGH("RRCDORM:Invalid Event: %d for dormancy in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcdormancy_substate);
      break;

  }/*end of switch*/
} /* rrcdormancy_process_wait_for_l2_ack_substate */

/*===========================================================================

FUNCTION rrcdormancy_process_wait_for_cu_cnf_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to fast dormancy  procedure. in RRCDORMANCY_WAIT_FOR_CU_CNF
  substate

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcdormancy_process_wait_for_cu_cnf_substate
(
  rrc_cmd_type *cmd_ptr
)
{
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if((rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status == TRUE) &&
         (rrcdormancy_saved_fast_dormancy_req.mui_status == RRCDORMANCY_MUI_STATUS_INVALID))
      {
        if(rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status == TRUE)
        {
          WRRC_MSG0_HIGH("Processing dormancy after CU_COMP_CNF");
          /* If all validation goes well then process fast dormancy req and start T323 timer*/
          if(TRUE == rrcdormancy_process_fast_dormancy_req())
          {
            if(TRUE == rrcdormancy_check_if_nw_support_fast_dormancy())
            {
              rrcdormancy_stop_or_restart_timer_T323(rrcdormancy_get_T323_timer_value(),
                RRCDORMANCY_T323_TIMER_RUNNING); 
            }
            else
            {
              WRRC_MSG0_HIGH("N/W dint support T323. Do Not Start timer");
            }            
          }
        }
      }
      else
      {
         MSG_HIGH("RRCDORM:No Dormancy to process substate:%d REQ:%d MUI:%d", 
            rrcdormancy_substate,
            rrcdormancy_saved_fast_dormancy_req.fast_dormancy_req_status,
            rrcdormancy_saved_fast_dormancy_req.mui_status);
      }
      break;

    case RRC_FAST_DORMANCY_REQ:
      /* Do not process next dormancy req till first one complets */
      rrcdormancy_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
      break;

    default:
      WRRC_MSG2_HIGH("RRCDORM:Invalid Event: %d for dormancy in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcdormancy_substate);
      break;

  }  
   
} /* rrcdormancy_process_wait_for_cu_cnf_substate */

/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_sig_con_rel_compl_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL sub state either 
  CELL_FACH State or CELL_DCH state.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcdormancy_process_wait_for_sig_con_rel_compl_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_SIG_CONN_RELEASE_CNF:
      WRRC_MSG1_HIGH("Status rcvd from SCRR: %d", 
                  cmd_ptr->cmd.rrc_sig_conn_cnf.status);
      //send fast-dormancy-cnf status based on l2-ack status
      if(SUCCESS == cmd_ptr->cmd.rrc_sig_conn_cnf.status)
      {
        rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);
      }
      else
      {
        rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_FAILURE_APP_RETRY);
      }
      /*PS doamin would have been closed by now...so send CLOSE_SESSION_IND to NAS now*/
      rrcscr_send_close_session_ind(RRC_PS_DOMAIN_CN_ID, RRC_MAX_SESSION_CLOSE_CAUSE);
      break;

    case RRC_FAST_DORMANCY_REQ:
      /* Do not process next dormancy req till first one complets */
      rrcdormancy_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_SUCCESS_APP_RETRY);
      break;

    default:
      WRRC_MSG2_HIGH("RRCDORM:Invalid Event: %d for dormancy in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcdormancy_substate);
  }
}

/*===========================================================================

FUNCTION rrcdormancy_process_dormancy_initial_substate

DESCRIPTION
  This function processes all events that are  dispatched
  to fast dormancy  procedure. in RRCDORMANCY_INITIAL
  substate

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcdormancy_process_dormancy_initial_substate
(
  rrc_cmd_type *cmd_ptr
)
{
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_FAST_DORMANCY_REQ:
      WRRC_MSG1_HIGH("DORMANCY_REQ rcvd in %d substate",rrcdormancy_substate);
      rrcdormancy_validate_and_process_fast_dormancy_req(cmd_ptr);
      break;

    default:
      WRRC_MSG2_HIGH("RRCDORM:Invalid Event: %d for dormancy in substate:%d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcdormancy_substate);
      break;
  }/*end of switch*/
} /* rrcdormancy_process_dormancy_initial_substate */

/*===========================================================================

FUNCTION rrcdormancy_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to for the fast dormancy  procedure.
  in CELL_FACH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcdormancy_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcdormancy_substate)
  {
    case RRCDORMANCY_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_dormancy_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);
         break;
      }
      break;

    case RRCDORMANCY_WAIT_FOR_CU_COMP_CNF: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a close session request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_CELL_UPDATE_COMPLETE_CNF:
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_wait_for_cu_cnf_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);

          break;
      }
      break;

    case RRCDORMANCY_WAIT_FOR_L2ACK: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a close session request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_DOWNLINK_L2ACK_IND:
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_wait_for_l2_ack_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);

          break;
      }
      break;

    case RRCDORMANCY_WAIT_FOR_SIG_CON_REL_COMPL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_SIG_CONN_RELEASE_CNF:
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_wait_for_sig_con_rel_compl_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);
          break;
      }
      break;
      
    default:   
      /*lint -e793*/        
      WRRC_MSG1_HIGH("RRCDORM:Invalid substate: %d in CELL FACH",
                rrcdormancy_substate);
      /*lint +e793*/
      break;
  }  
}/* rrcdormancy_process_cell_fach_state */
/*===========================================================================

FUNCTION rrcdormancy_process_cell_dch_state

DESCRIPTION
  This function processes all events that are dispatched to for the fast dormancy  procedure.
  in CELL_DCH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcdormancy_process_cell_dch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcdormancy_substate)
  {
    case RRCDORMANCY_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_dormancy_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG2_HIGH("RRCDORM:Invalid event %d in Substate: %d",
                    cmd_ptr->cmd_hdr.cmd_id, rrcdormancy_substate);
         break;
      }
      break;

    case RRCDORMANCY_WAIT_FOR_CU_COMP_CNF: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a close session request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_CELL_UPDATE_COMPLETE_CNF:
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_wait_for_cu_cnf_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);

          break;
      }
      break;

    case RRCDORMANCY_WAIT_FOR_L2ACK: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a close session request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_DOWNLINK_L2ACK_IND:
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_wait_for_l2_ack_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);

          break;
      }
      break;
      
    case RRCDORMANCY_WAIT_FOR_SIG_CON_REL_COMPL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_SIG_CONN_RELEASE_CNF:
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_wait_for_sig_con_rel_compl_substate(cmd_ptr);
          break;
    
        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);
          break;
      }
      break;

    default:   
      /*lint -e793*/        
      WRRC_MSG1_HIGH("RRCDORM:Invalid substate: %d in CELL FACH",
                rrcdormancy_substate);
      /*lint +e793*/
      break;
  }  
}/* rrcdormancy_process_cell_dch_state */

/*===========================================================================

FUNCTION rrcdormancy_process_cell_ura_pch_state

DESCRIPTION
  This function processes all events that are dispatched to for the fast dormancy  procedure.
  in CELL/URA_PCH state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcdormancy_process_cell_ura_pch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{

  switch (rrcdormancy_substate)
  {
    case RRCDORMANCY_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_dormancy_initial_substate(cmd_ptr);
          break;
        
        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);
         break;
      }
      break;

    case RRCDORMANCY_WAIT_FOR_CU_COMP_CNF: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_CELL_UPDATE_COMPLETE_CNF:
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_wait_for_cu_cnf_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);

          break;
      }
      break;

    case RRCDORMANCY_WAIT_FOR_L2ACK: 
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        /* we can either get a complete confirmation or get a close session request
         * for a different CN domain.  Process both in wait for cell update comp cnf
         */
        case RRC_DOWNLINK_L2ACK_IND:
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_wait_for_l2_ack_substate(cmd_ptr);
          break;

        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);

          break;
      }
      break;

    case RRCDORMANCY_WAIT_FOR_SIG_CON_REL_COMPL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_SIG_CONN_RELEASE_CNF:
        case RRC_FAST_DORMANCY_REQ:
          rrcdormancy_process_wait_for_sig_con_rel_compl_substate(cmd_ptr);
          break;
    
        default:
          WRRC_MSG1_HIGH("RRCDORM:Invalid event in Substate: %d",
                    rrcdormancy_substate);
          break;
      }
      break;

    default:   
      /*lint -e793*/        
      WRRC_MSG1_HIGH("RRCDORM:Invalid substate: %d in CELL FACH",
                rrcdormancy_substate);
      /*lint +e793*/
      break;
  }  
}/* rrcdormancy_process_cell_ura_pch_state */

/*===========================================================================

FUNCTION rrcdormancy_process_idle_state

DESCRIPTION
  This function processes all events that are dispatched to for the fast dormancy  procedure.
  in IDLE state of Connected mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcdormancy_process_idle_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_FAST_DORMANCY_REQ:
      WRRC_MSG0_HIGH("Not processing Fast Dormancy SCRI");
      rrcdormancy_reset_n_send_fast_dormancy_cnf(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);
      break;
   
    default:
      WRRC_MSG1_ERROR("RRCDORM:Invalid cmd_id i: 0X%x",
                    cmd_ptr->cmd_hdr.cmd_id);
      break;
  }
}/* rrcdormancy_process_idle_state */


/*===========================================================================

FUNCTION rrcdormancy_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the fast dormancy  procedure.
  The following events are handled by this procedure:

    RRC_FAST_DORMANCY_REQ Primitive (received from MM)
    RRC_CELL_UPDATE_COMPLETE_CNF primitive (received from CELL UPDATE Procedure)

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcdormancy_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
  uint64 remaining_value = 0;

  rrc_state_e_type rrc_state = rrc_get_state();
  cm_rpt_type *cm_cmd_ptr = NULL;
  mm_cmd_type *mm_cmd_ptr = NULL;

  WRRC_MSG2_HIGH_OPT("function rrcdormancy_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d and rrcdormancy_substate = rrcdormancy_substate_e_type_value%d", rrc_state, rrcdormancy_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_DORMANCY,rrcdormancy_substate,cmd_ptr->cmd_hdr.cmd_id);

  if(RRC_STATE_CHANGE_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    MSG_HIGH("RRCDORM:State-Change-Ind dorm-state:%d rrc-state:%d new-state:%d",
       rrcdormancy_substate,rrc_state,cmd_ptr->cmd.state_change_ind.new_state);
    if (RRC_STATE_CELL_PCH == cmd_ptr->cmd.state_change_ind.new_state
    || RRC_STATE_URA_PCH == cmd_ptr->cmd.state_change_ind.new_state
    )
    {
      cm_cmd_ptr = cm_get_cmd_buf();
      cm_cmd_ptr->hdr.cmd = CM_RRC_PCH_STATE_IND;
#ifdef FEATURE_DUAL_SIM
      cm_cmd_ptr->asubs_id = rrc_get_as_id();
#endif
      cm_cmd_ptr->cmd.pch_state_info.is_in_pch_state = TRUE;
      cm_put_cmd(cm_cmd_ptr);

      mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_PCH_STATE_IND);
      mm_cmd_ptr->cmd.rrc_pch_state_ind.pch_state_info.is_in_pch_state = TRUE;
 #ifdef FEATURE_DUAL_SIM    
      mm_cmd_ptr->cmd.rrc_pch_state_ind.as_id = rrc_get_as_id();
#endif
      mm_put_cmd(mm_cmd_ptr);
      ds_3gpp_update_pch_state
      ( 
        SYS_SYS_MODE_WCDMA,
        DS_3GPP_EVENT_IN_PCH
      );
    }
    else if ((RRC_STATE_CELL_PCH == cmd_ptr->cmd.state_change_ind.previous_state)
    || (RRC_STATE_URA_PCH == cmd_ptr->cmd.state_change_ind.previous_state))
    {
      cm_cmd_ptr = cm_get_cmd_buf();
      cm_cmd_ptr->hdr.cmd = CM_RRC_PCH_STATE_IND;
#ifdef FEATURE_DUAL_SIM
      cm_cmd_ptr->asubs_id = rrc_get_as_id();
#endif
      cm_cmd_ptr->cmd.pch_state_info.is_in_pch_state = FALSE;
      cm_put_cmd(cm_cmd_ptr);

      mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_PCH_STATE_IND);
      mm_cmd_ptr->cmd.rrc_pch_state_ind.pch_state_info.is_in_pch_state = FALSE;
#ifdef FEATURE_DUAL_SIM
      mm_cmd_ptr->cmd.rrc_pch_state_ind.as_id = rrc_get_as_id();
#endif
      mm_put_cmd(mm_cmd_ptr);
      ds_3gpp_update_pch_state
      ( 
        SYS_SYS_MODE_WCDMA,
        DS_3GPP_EVENT_OUT_OF_PCH
      );
    }

    switch (cmd_ptr->cmd.state_change_ind.new_state)
    {
      case RRC_STATE_DISCONNECTED:
        WRRC_MSG0_HIGH("RRCDORM:State-Change-Ind to IDLE, STOP T_323_TIMER");
        rrctmr_stop_timer(RRCTMR_T_323_TIMER);
        /* Code fall through. Make Lint Happy */
      case RRC_STATE_CONNECTING:
          rrcdormancy_reset_v316();
        /* Code fall through. Make Lint Happy */
      case RRC_STATE_CELL_PCH:
      case RRC_STATE_URA_PCH:
        rrcdormancy_check_and_abort_dormancy_req(RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY);
        break;
      case RRC_STATE_CELL_FACH:
        WRRC_MSG0_HIGH("RRCDORM:State-Change-Ind to FACH ");
        /* for PCH->FACH, check if no data activity
         * if yes -> num_pending_dormancy_req is TRUE & NW support 3gpp FD start T_323, at expiry we will re try FD
         * if no -> re set num_pending_dormancy_req
         */
        if((rlc_query_data_channel_activity() == NO_DATA_TRANSFERRED) &&
           (rrc_check_psrab_data_is_present() == FALSE))
        {
          WRRC_MSG1_HIGH("RRCDORM:Value of max_scri %d",rrc_max_scri_for_fd);
          if(rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req != 0)
          {
            WRRC_MSG1_HIGH("RRCDORM: Pending Dormancy requests:%d in new FACH state",rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req);
            if(TRUE == rrcdormancy_check_if_nw_support_fast_dormancy())
            {
              remaining_value = rrctmr_get_remaining_time(RRCTMR_T_323_TIMER);
              if(remaining_value == 0)
              {
                rrctmr_start_timer(RRCTMR_T_323_TIMER, RRC_DORMANCY_T323_RETRY_TIMER);
              }
              return;
            }
            else
            {
              WRRC_MSG0_HIGH("N/W dint support T323. Do Not Start timer");
              rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req =0;
            } 
          }
        }
        else
        {
          rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req =0;
        }
        break;

      default:
        MSG_HIGH("Unexpected StateChangeInd dorm-state:%d rrc-state:%d new-state:%d",
          rrcdormancy_substate,rrc_state,cmd_ptr->cmd.state_change_ind.new_state);
    }
  }
  else if(RRC_T323_EXPIRED_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    rrcdormancy_process_T323_expiry(cmd_ptr);
  }
  else
  {
    switch (rrc_state)
    {
      WRRC_MSG2_HIGH("RRCDORM:state %d handler dormancy Substate: %d",rrc_state,
            rrcdormancy_substate);

      case RRC_STATE_CELL_FACH:    
        rrcdormancy_process_cell_fach_state(cmd_ptr); 
        break;

      case RRC_STATE_CELL_DCH:  
        rrcdormancy_process_cell_dch_state(cmd_ptr); 
        break;

      case RRC_STATE_CELL_PCH:
      case RRC_STATE_URA_PCH:
        rrcdormancy_process_cell_ura_pch_state(cmd_ptr);
        break;

      case RRC_STATE_DISCONNECTED:
      case RRC_STATE_CONNECTING:
        rrcdormancy_process_idle_state(cmd_ptr);
        break;

      default:  
        WRRC_MSG1_HIGH("RRCDORM:Event in an invalid State: %d",rrc_state);
        break;
     }  
  }
} /* rrcdormancy_procedure_event_handler */

/*===========================================================================

FUNCTION rrc_dormancy_init

DESCRIPTION

  This function initializes the dormancy  procedure.
  This function needs to be called once only, at powerup.

DEPENDENCIES

  None.

RETURN VALUE

  None.
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_dormancy_init( void )
{


  rrcdormancy_init_saved_fast_dormancy_req();

  rrcdormancy_saved_fast_dormancy_req.num_pending_dormancy_req = 0;

  /* Disable fast dormancy support during init
     * This will be enabled when N/W indicates support of Timer T323 */
  rrcdormancy_disable_fast_dormancy();
  /* Reset V316 */ 
  rrcdormancy_reset_v316();

 /* Also send registration to state change manager. We need to know when
  RRC goes to Idle mode (disconnected state). The BCCH will be cleared at
  that time so we need to clear our internal variables that keep track of
  the BCCH status. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_DORMANCY,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED /* To State    */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_DORMANCY,  /* Procedure name */
                             RRC_STATE_DISCONNECTED, /* From State     */
                             RRC_STATE_WILDCARD /* To State    */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_DORMANCY,  /* Procedure name */
                             RRC_STATE_CELL_PCH, /* From State     */
                             RRC_STATE_CELL_FACH /* To State    */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_DORMANCY,  /* Procedure name */
                             RRC_STATE_URA_PCH, /* From State     */
                             RRC_STATE_CELL_FACH /* To State    */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_DORMANCY,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_CELL_PCH /* To State    */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_DORMANCY,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_URA_PCH /* To State    */
                           );

}

#endif  /*RRCDORMANCY_H*/
