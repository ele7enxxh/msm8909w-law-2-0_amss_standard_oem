#ifndef RRCCUI_H
#define RRCCUI_H
/*===========================================================================
              R R C   C E L L   U P D A T E   H E A D E R   F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  Cell Update Procedure.
  
Copyright (c) 2002-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccui.h_v   1.6   28 Jun 2002 15:49:48   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrccui.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/14   sr      Made changes to trigger tranistion to idle if TA duraitonis mroe than re-establishment timer values
24/01/13   sg      Made changes to log CU and UU status as external event
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/24/12   db       Made changes to restore HRNTI and mapping info on fach to dch phy chn failure
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
06/05/09   ps      Made changes to not start the T305 timer if value is infinity
05/29/09   ps      Made changes to make OOS timer as 24 hrs
05/08/09   ss      Updated Copyright Information
12/17/08   gkg     Added the signature of function rrccu_get_rb_id_unrec_error().
02/13/08   rm      Added prototype for rrccu_set_tvm_ind_in_cu under FEATURE_TVM_IND_IN_CU
09/05/06   da      Removed prototype of rrccu_transition_to_disconnected_state()
08/28/06   da      Added support for OOS area handling during transition from DCH.
08/25/06   da      Setting RRCTMR_T_OOS_TIMER_IN_MS to infinity under 
                   FEATURE_DEEP_SLEEP_REL6_OUT_OF_SERVICE_ENHANCEMENT
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/15/06   sgk     Moved rrccu substate type def to rrccu.h, added 
                   RRCCU_PENDING_CONFIG cause to rrccu status.
11/08/05   da      Added new CU substate: RRCCU_WAIT_FOR_BPLMN_SUSPEND_CNF
07/08/05   vm      FEATURE_OUT_OF_SERVICE_ENHANCEMENT: Added support for OOS in 
                   Connected Mode feature.
                   If the feature is defined then the value of OOS timer is 30mins
                   otherwise it is 40s.
06/27/05   da      Changed prototype of rrccu_build_cell_update_msg to return
                   rrccu_status_e_type.
04/15/05   da      Added prototype for rrccu_process_oos_area_handling.
03/02/05	ttl			 Support RRC ASN1 Release 5.
11/23/04   da      Changed RRCTMR_PHYCHANFAILURE_TIMER_IN_MS with value 10s 
                   to RRCTMR_CPHYCHANFAIL_TIMER_IN_MS with value 8s.
09/22/04   da      Defined RRCTMR_PHYCHANFAILURE_TIMER_IN_MS as 10s
09/15/04   sk      Added definition for RRCTMR_T_OOS_TIMER_IN_MS
08/30/04  sgk      Moved protoypes for rrccu_clear_ura_id and 
                   rrcc_update_ura_id to rrccu.h as they were being called in 
                   rrcrbe.c
07/15/04   sk      Added support to handle RL failure/RLC unrecoverable error
                   Added new subatate CU_WAIT_FOR_RNTI_UPDATE_CNF
                   Added function prototypes
12/03/03   sk      Support for Radio lonk failure. under feature RL_FAILURE_HANDLING
11/12/03   sk      Added a substate to handle PCCH establishment in Cell_PCH/URA_PCH state
11/11/03   kc      Added additional state to wait for SRB2 RE-EST for SRNS relocation.
02/04/03   ram     Checked in lint changes.
08/07/02   rj      Added prototype for rrccu_reset_registration_info.
08/07/02   rj      Moved prototype for rrccu_register_for_cell_update_to_complete
                   to rrccu.h since it is an external interface to other 
                   RRC procedures.
08/07/02   rj      Added prototype for rrccu_register_for_cell_update_to_complete                   
06/28/02   rj      Added prototype for function rrccu_process_re_est_timers.
06/17/02   rj      Added prototype rrccu_process_rlc_status_ind to process
                   RRC_CRLC_STATUS_IND command.
03/12/02   rj      Deleted additional comma in enum type rrccu_substate_e_type.
03/08/02   bu      Removed definitions for functions rrcpcr_build_pcr_complete_message,
                   rrctcr_build_tcr_complete_message and
                   rrcrbrc_build_rbrc_complete_message.
03/07/02   rj      Added prototypes for functions rrccu_stop_cu_timers and
                   rrccu_inform_cell_update_complted.
03/05/02   rj      Added prototype for function rrccu_delete_crnti.
02/28/02   rj      Added code function prototypes for complete Cell Update 
                   procedure implementation. Also added functions to build
                   Radio Bearer Recnfiguration Complete, Transport Channel
                   Reconfiguration Complete and Physical Channel Reconfiguration
                   complete messages since these procedures are not supported
                   in current release.
02/15/02   rj      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "rrcdata_v.h"
#include "rrccmd_v.h"
#include "rrcrbcommon.h"
#include "rrclogging.h"
/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

#define RRCCU_INVALID_TRANSACTION_ID 0xFF   /* This used to initialize RRCCU
                                              Transaction Id */
#define RRCTMR_T_OOS_TIMER_IN_MS RRCTMR_INFINITY_VAL   /* set a very large value for OOS timer value */  
#define RRCTMR_CPHYCHANFAIL_TIMER_IN_MS 8000 /* Timer for phy chann failure */

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/



typedef enum
{
  RRCCU_SUCCESS,                            /* Successful Status */      
  RRCCU_FAILURE_MINOR,                      /* Minor Failure Status */
  RRCCU_FAILURE_MAJOR,                       /* Major Failure status */
  RRCCU_PENDING_CONFIG                      /* this cause is set when there 
                                               is an ongoing configuration 
											   and CU is initiated with rl
											   failure or rlc unrecoverable 
											   error */
} rrccu_status_e_type;

typedef enum
{
  RRCCU_T314_T315_ZERO,                    /* Indicates both T314 and T315
                                              are zero */
  RRCCU_T314_ZERO,                         /* Indicates T314 is zero and 
                                              T315 is non-zero */
  RRCCU_T315_ZERO,                         /* Indicates T315 is zero and 
                                              T314 is non-zero */
  RRCCU_T314_T315_NON_ZERO                 /* Indicates both T314 and T315
                                              are non-zero */
}rrccu_re_est_timers_status_e_type;



typedef struct
{
  uint32 no_of_reg_procs;          /* Total number procedures to informed
                                      up on completion of CU */
  rrc_proc_e_type rrc_proc[MAX_NUM_OF_PROCEDURES];
                                   /* Details of procedures that 
                                      are registerd */
}rrccu_reg_proc_type;

typedef enum
{
  RRCCU_INCLUDE_FAILURE_CAUSE,         /* Indicates to include Failure cause */ 
                                     
  RRCCU_NOT_INCLUDE_FAILURE_CAUSE      /* Indicates not to include Failure 
                                          cause */
}rrccu_failure_status_e_type;

typedef enum
{
  RRCCU_NO_RESP_MSG,                  /* No Response Msg is required */

  RRCCU_UMIC_COMPLETE_RESP_MSG,       /* Xmit Utran Mobility Information
                                         Confirm Msg */
  RRCCU_RBR_COMPLETE_RESP_MSG,        /* Xmit Radio Bearer Release Complete
                                         Msg */
  RRCCU_RBRC_COMPLETE_RESP_MSG,       /* Xmit Radio Bearer Reconfiguration
                                         Complete Msg */ 
  RRCCU_TCR_COMPLETE_RESP_MSG,        /* Xmit Transport Channel Reconfiguration
                                         Complete Msg */ 
  RRCCU_PCR_COMPLETE_RESP_MSG         /* Xmit Physical Channel Reconfiguration
                                         Complete Msg */ 
}rrccu_resp_msg_e_type;
  
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION RRCCU_CONVERT_T307_TO_MS

DESCRIPTION
  This function converts the rrc_T_307 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrccu_convert_t307_to_ms
(
  rrc_T_307 rrccu_t_307                    /* enum for Timer T307 */
);
/*===========================================================================

FUNCTION RRCCU_CONVERT_T302_TO_MS

DESCRIPTION
  This function converts the rrc_T_302 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrccu_convert_t302_to_ms
(
  rrc_T_302 rrccu_t_302                    /* enum for Timer T302 */
);
/*===========================================================================

FUNCTION RRCCU_CONVERT_T316_TO_MS

DESCRIPTION
  This function converts the rrc_T_316 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrccu_convert_t316_to_ms
(
  rrc_T_316 rrccu_t_316                    /* enum for Timer T316 */
);

/*===========================================================================

FUNCTION RRCCU_CONVERT_T314_TO_MS

DESCRIPTION
  This function converts the rrc_T_314 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrccu_convert_t314_to_ms
(
  rrc_T_314 rrccu_t_314                    /* enum for Timer T314 */
);

/*===========================================================================

FUNCTION RRCCU_CONVERT_T315_TO_MS

DESCRIPTION
  This function converts the rrc_T_315 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  int

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrccu_convert_t315_to_ms
(
  rrc_T_315 rrccu_t_315                    /* enum for Timer T315 */
);
/*===========================================================================

FUNCTION RRCCU_VALIDATE_CELL_UPDATE_INITIATE_REQ

DESCRIPTION

  This function validates RRC_CELL_UPDATE_INITIATE_REQ command and stores
  initiates procedure if it needs confirmation after Cell Update procedure
  successfully compelted.
  
DEPENDENCIES


RETURN VALUE

  rrccu_status_e_type: returns RRCCU_SUCCESS if it successfully validates
  RRC_CELL_UPDATE_INITIATE_REQ. Otherwise it returns
  RRCCU_FAILURE_MINOR.

SIDE EFFECTS

  None

===========================================================================*/
rrccu_status_e_type rrccu_validate_cell_update_initiate_req
(    
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION RRCCU_CHECK_RE_EST_TIMERS

DESCRIPTION

  This function reads the values of Timers T314 and T315 from the SIB DB and
  returns details of the counters.
  
DEPENDENCIES


RETURN VALUE

  rrccu_re_est_timers_status_e_type: returns RRCCU_T314_T315_ZERO if both T314 and 
  T315 are zero. Returns RRCCU_T314_ZERO if T314 is zero and T315 is a non-zero.
  Returns RRCCU_T315_ZERO if T315 is non-zero and T315 is a zero and returns
  RRCCU_T314_T315_NON_ZERO if both T314 and T315 are non-zero.

SIDE EFFECTS

  None

===========================================================================*/
rrccu_re_est_timers_status_e_type rrccu_check_re_est_timers
(    
  void
);
/*===========================================================================

FUNCTION RRCCU_ESTABLISH_RB0

DESCRIPTION
  This function sends RRC_CHANNEL_CONFIG_REQ to RRC LLC to establish RB0
  
DEPENDENCIES


RETURN VALUE
  rrccu_status_e_type: returns RRCCU_SUCCESS if it successfully sends
  RRC_CHANNEL_CONFIG_REQ to the RRC LLC. Otherwise it returns
  RRCCU_FAILURE_MAJOR.


SIDE EFFECTS

  None

===========================================================================*/
void rrccu_establish_rb0
(    
  rrc_rb_cfg_e_type action,                   /* Type of action RB 0 */
  rrc_state_e_type dest_state,                /* Next state to transition is
                                                 expected to take place */
  boolean confirm_required                    /* This indicates whether
                                                 RRC_CHANNEL_CONFIG_CNF is 
                                                 required or not */  
);
/*===========================================================================

FUNCTION RRCCU_BUILD_CELL_UPDATE_MSG

DESCRIPTION
  This function builds the CELL UPDATE message with all required
  parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
rrccu_status_e_type rrccu_build_cell_update_msg
(
  rrc_UL_CCCH_Message *msg_ptr,            /* Pointer to uplink CCCH message*/
  rrccu_failure_status_e_type status       /* Indicates whether to include
                                              failure cause or not */
);

/*===========================================================================

FUNCTION rrccu_send_cell_update_msg

DESCRIPTION

  This function builds the CELL UPDATE message and enqueues the same in the 
  Uplink queue. Also starts the timer T302.
  
DEPENDENCIES


RETURN VALUE
  rrccu_status_e_type: returns RRCCU_SUCCESS if it successfully enqueues
  CELL UPDATE message in the Uplink queue. Otherwise it returns
  RRCCU_FAILURE_MAJOR

SIDE EFFECTS

  None

===========================================================================*/
rrccu_status_e_type rrccu_send_cell_update_msg
(    
  rlc_lc_id_type ul_lc_id,                 /* Logical Channel Id */
  rrccu_failure_status_e_type status       /* Indicates whether to include
                                              failure cause or not */
);
/*===========================================================================

FUNCTION rrccu_send_rrc_initiate_cell_selection_ind

DESCRIPTION
  This function builds RRC_INITIATE_CELL_SELECTION_IND command to post to
  CSP procedure with the cause specified.
DEPENDENCIES
  None

RETURN VALUE
  rrccu_status_e_type: It returns RRCCU_SUCCESS if it successfully sends 
  RRC_INITIATE_CELL_SELECTION_IND command to CSP. Otherwise it returns 
  RRCCU_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_send_rrc_initiate_cell_selection_ind
(
  rrc_cell_sel_cause_e_type cause,           /* Cause to trigger Cell
                                                selection */
  rrc_state_e_type destination_state         /* Destination RRC state */
);

/*===========================================================================

FUNCTION RRCCU_PROCESS_CU_INITIAL

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in RRCCU_INITIAL substate.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cu_initial
(    
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION rrcu_process_timer_t315_expired

DESCRIPTION
  This function processes RRC_T315_EXPIRED_IND event. It checks whether T314
  is already expired. If T314 expired, then it will bring down RRC Connection
  and resets the CU state machine. Otherwise it releases the RBs associated 
  with T315 and remain in the same CU substate.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcu_process_timer_t315_expired
(    
  void
);
/*===========================================================================

FUNCTION rrcu_process_timer_t314_expired

DESCRIPTION
  This function processes RRC_T314_EXPIRED_IND event. It checks whether T315
  is already expired. If T315 expired, then it will bring down RRC Connection
  and resets the CU state machine. Otherwise it releases the RBs associated 
  with T314 and remain in the same CU substate.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcu_process_timer_t314_expired
(    
  void
);
/*===========================================================================

FUNCTION RRCU_PROCESS_CU_WAIT_FOR_UE_TO_CAMP_ON

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in RRCCU_WAIT_FOR_UE_TO_CAMP_ON substate.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcu_process_cu_wait_for_ue_to_camp_on
(    
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION RRCCU_PROCESS_CU_WAIT_FOR_CELL_UPDATE_CONFIRM

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in RRCCU_WAIT_FOR_CELL_UPDATE_CONFIRM substate.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cu_wait_for_cell_update_confirm
(    
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION RRCCU_PROCESS_CU_WAIT_FOR_RB_EST_CNF

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in RRCCU_WAIT_FOR_RB_EST_CNF substate.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cu_wait_for_rb_est_cnf
(    
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION RRCCU_PROCESS_CU_WAIT_FOR_L2_ACK

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in RRCCU_WAIT_FOR_L2_ACK substate.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cu_wait_for_l2_ack
(    
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION rrccu_process_cu_wait_for_rb0_est_cnf

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in RRCCU_WAIT_FOR_RB0_EST_CNF substate.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cu_wait_for_rb0_est_cnf
(    
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION RRCCU_PROCESS_CU_WAIT_FOR_RB_REL_CNF

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in RRCCU_WAIT_FOR_RB_REL_CNF substate.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cu_wait_for_rb_rel_cnf
(    
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION RRCCU_PROCESS_CU_WAIT_FOR_BPLMN_SUSPEND_CNF

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in RRCCU_WAIT_FOR_BPLMN_SUSPEND_CNF substate.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cu_wait_for_bplmn_suspend_cnf
(    
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION RRCCU_PROCESS_CU_WAIT_FOR_RB_PROC_COMPL_IND

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in RRCCU_WAIT_FOR_RB_PROC_COMPL_IND substate.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cu_wait_for_rb_proc_compl_ind
(    
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION RRCCU_PROCESS_CELL_DCH_STATE

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in CELL_DCH state of Connected  mode.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cell_dch_state
(    
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION RRCCU_PROCESS_CELL_FACH_STATE

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in CELL_FACH state of Connected  mode.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cell_fach_state
(  
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION RRCCU_PROCESS_DISCONNECTED_STATE

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in DISCONNECTED state of Idle  mode.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_disconnected_state
(  
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION RRCCU_PROCESS_CONNECTING_STATE

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in CONNECTING state of Idle  mode.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_connecting_state
(  
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION RRCCU_PROCESS_CELL_PCH_URA_PCH_STATE

DESCRIPTION
  This function processes all events that are dispatched to Cell Update 
  procedure in CELL_PCH and URA_PCH states of Connected  mode.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_cell_pch_ura_pch_state
(  
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION rrccu_resend_cell_update_msg

DESCRIPTION
  This function compares V302 with N302 and trasmits CELL UPDATE if 
  V302 <= N302. Otherwise it brings down RRC Connection.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_resend_cell_update_msg
(    
  rrccu_failure_status_e_type status       /* Indicates whether to include
                                              failure cause or not */
);
/*===========================================================================

FUNCTION RRCCU_PROCESS_CELL_UPDATE_CONFIRM

DESCRIPTION
  This function processes CELL UPDATE CONFIRM message received in response to
  Cell Update message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
  
void rrccu_process_cell_update_confirm
(    
  void *msg_ptr,                            /* Pointer to Downlink msg */
  uint32 dl_sdu_type                         /* Downlink SDU type */
);
/*===========================================================================

FUNCTION RRCCU_SEND_RESPONSE_MSG

DESCRIPTION
  This function transmits response message to CELL UPDATE CONFIRM message if
  required.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_send_response_msg
(    
  void
);
/*===========================================================================

FUNCTION RRCCU_DELETE_CRNTI

DESCRIPTION
  This function sends RRC_RNTI_UPDATE_REQ to RRC LLC to delete the C-RNTI
  
DEPENDENCIES


RETURN VALUE
  rrccu_status_e_type: returns RRCCU_SUCCESS if it successfully sends
  RRC_CHANNEL_CONFIG_REQ to the RRC LLC. Otherwise it returns
  RRCCU_FAILURE_MAJOR.


SIDE EFFECTS

  None

===========================================================================*/
void rrccu_delete_crnti
(  
  void
);
/*===========================================================================

FUNCTION RRCCU_STOP_CU_TIMERS

DESCRIPTION
  This function stops all Cell Update procedure related timers. This
  function will be used when CU procedure non-gracefully terminates and
  RRC transitions to Disconnected State.


DEPENDENCIES
  None

RETURN VALUE
  int

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_stop_cu_timers
(
  void              
);
/*===========================================================================

FUNCTION RRCCU_INFORM_CELL_UPDATE_COMPLETED

DESCRIPTION

  This function checks the database and sends RRC_CELL_UPDATE_COMPLETE_CNF
  command to all RRC procedures that waiting for Cell Update completetion.
  
DEPENDENCIES


RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
void rrccu_inform_cell_update_completed
(    
  void
);
/*===========================================================================

FUNCTION RRCCU_PROCESS_RLC_STATUS_IND

DESCRIPTION
  This function processes RRC_CRLC_STATUS_IND command received from RLC
  when RLC reset is occured.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_rlc_status_ind
(    
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION rrccu_process_re_est_timers

DESCRIPTION
  This function checks the timers T314 and T315 and releases the RBs
  accordingly and starts Cell Selection to camp-on a suitable cell and 
  transition to CELL_FACH state.
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_process_re_est_timers
(
  boolean
);
/*===========================================================================

FUNCTION rrccu_process_oos_area_handling

DESCRIPTION

  This function is called when Cell update is initiated with cause 
  RRC_REENTERED_SERVICE_AREA by CSP.  It triggers out of service area handling.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_process_oos_area_handling
(
  void 
);
/*===========================================================================

FUNCTION RRCCU_RESET_REGISTRATION_INFO

DESCRIPTION
  This function will reset the registration information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrccu_reset_registration_info
(    
  void  
);

/*===========================================================================

FUNCTION RRCCU_SEND_STATE_CHANGE_IND

DESCRIPTION
  This function sends RRC_CHANNEL_CONFIG_REQ to RRC LLC to changes the RRC
  state either to CELL_PCH or URA_PCH
  
DEPENDENCIES


RETURN VALUE
  rrccu_status_e_type: returns RRCCU_SUCCESS if it successfully sends
  RRC_CHANNEL_CONFIG_REQ to the RRC LLC. Otherwise it returns
  RRCCU_FAILURE_MAJOR.


SIDE EFFECTS

  None

===========================================================================*/
void rrccu_send_state_change_ind
(    
  rrc_state_e_type rrc_state                  /* destination state */
);



/*===========================================================================
      
FUNCTION RRCCU_IS_URA_UPDATE_REQD

DESCRIPTION
  
This function will return TRUE if the URA update is required 
else it will return FALSE.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/


boolean rrccu_is_ura_update_reqd
(
 void
);


/*===========================================================================

FUNCTION rrccu_process_rlc_status_ind_in_non_initial_sub_states

DESCRIPTION
  This function processes RRC_CRLC_STATUS_IND command received from RLC
  when RLC reset is occured and CU is active

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrccu_process_rlc_status_ind_in_non_initial_sub_states
(    
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION RRCCU_GET_CELL_UPDATE_STARTED_STATUS_WITH_CAUSE

DESCRIPTION

  This function checks whether Cell Update procedure is started or not.
  
DEPENDENCIES


RETURN VALUE

  A value of rrccu_cell_update_status_e_type indicating the status of the
  function.

SIDE EFFECTS

  None

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrccu_cell_update_started_status_e_type rrccu_get_cell_update_started_status_with_cause
(    
  rrc_CellUpdateCause       *cu_cause_ptr
);

/*===========================================================================
FUNCTION rrccu_get_rb_id_unrec_error()

DESCRIPTION
  This function return the RB id on which RLC unrecoverable error has occured.
  
DEPENDENCIES
  None

RETURN VALUE
  RB Id.
  
SIDE EFFECTS
  None
===========================================================================*/
rrc_RB_Identity rrccu_get_rb_id_unrec_error
(
  void
);

/*===========================================================================

FUNCTION rrccu_status_for_reselection

DESCRIPTION
  This function will check whether CU is in a state where reselection
  can be continued. This API is to prevent situations such as reselection
  when oc is set and lower layers are being configured. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean rrccu_status_for_reselection
(    
  void  
);

/*===========================================================================

FUNCTION rrccu_set_tvm_ind_in_cu

DESCRIPTION
  This function will set the traffic volume indicator in cell update message. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrccu_set_tvm_ind_in_cu
(
  rrc_UL_CCCH_Message *msg_ptr
);

/*===========================================================================
FUNCTION rrccu_process_dl_sdu_ind_on_ccch

DESCRIPTION
  This function processes DL SDU IND command if it has come over CCCH.

  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void rrccu_process_dl_sdu_ind_on_ccch
(
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION rrccu_process_dl_sdu_ind_on_dcch

DESCRIPTION
  This function processes DL SDU IND command if it has come over DCCH.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void rrccu_process_dl_sdu_ind_on_dcch
(
  rrc_cmd_type * cmd_ptr
);

/*===========================================================================
FUNCTION rrccu_process_after_oc_config_set_r8

DESCRIPTION
  This function processes the REl 8 CELL UPDATE CONFIRM after OC is set by CU CNF.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void rrccu_process_after_oc_config_set_r8
(
  rrc_CellUpdateConfirm_r8_IEs *cu_confirm_ptr
  ,rrcrb_directed_cell_info_type          directed_cell_info
);

/*===========================================================================
FUNCTION rrcu_process_cu_cnf_after_validation_r8

DESCRIPTION
  This function processes the REl 8 CELL UPDATE CONFIRM message after doing the initial validation
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void rrcu_process_cu_cnf_after_validation_r8
(
  void *msg_ptr,                           /* Pointer to Downlink Msg */
  rrc_CellUpdateConfirm_r8_IEs *cu_confirm_ptr,
  uint32 dl_sdu_type                       /* Downlink SDU type */  
  ,rrcrb_directed_cell_info_type          directed_cell_info
);

/*===========================================================================
FUNCTION rrccu_process_freq_redirection_info_r8

DESCRIPTION
  This function processes the REl 8 CELL UPDATE CONFIRM message for Freq Redirection.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean rrccu_process_freq_redirection_info_r8
(
  rrc_CellUpdateConfirm_r8_IEs * cu_confirm_ptr,
  rrcrb_directed_cell_info_type * directed_cell_info_ptr
);


/*===========================================================================
FUNCTION RRCCU_PROCESS_CELL_UPDATE_CONFIRM_R8

DESCRIPTION
  This function processes the REl 8 CELL UPDATE CONFIRM message received in response to
  Cell Update message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void rrccu_process_cell_update_confirm_r8
(    
  void *msg_ptr,                           /* Pointer to Downlink Msg */
  rrc_RRC_TransactionIdentifier rcvd_transaction_id,
  rrc_CellUpdateConfirm_r8_IEs *cu_confirm_ptr,
  uint32 dl_sdu_type                       /* Downlink SDU type */
);

/*===========================================================================

FUNCTION RRCCU_RESPONSE_TO_CELL_UPDATE_CONFIRM_R8

DESCRIPTION
  This function evaluates CELL UPDATE CONFIRM message and decides what
  response message has to be sent
  Cell Update message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
rrccu_resp_msg_e_type rrccu_find_response_to_cell_update_confirm_r8
(
  rrc_CellUpdateConfirm_r8_IEs *cucnf_ptr
);

/*===========================================================================

FUNCTION rrccu_validate_rb_info_r8

DESCRIPTION
  This function validates the Rb related IEs in Cu Cnf
  Cell Update message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrccu_validate_rb_info_r8
(
   rrc_CellUpdateConfirm_r8_IEs * cu_confirm_ptr
);

/*===========================================================================

FUNCTION rrccu_update_chan_config_rb_r8

DESCRIPTION
 
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/

uint32 
rrccu_update_chan_config_rb_r8
(
 rrc_CellUpdateConfirm_r8_IEs * cu_confirm_ptr,
 rrc_rb_config_type * rb_info
 );

/*===========================================================================
FUNCTION rrccu_validate_rel_rb_exist_in_reconfig_list_r8

DESCRIPTION
  This function checks if the Cu Cnf has the same RB ID in the RB Release list
   and also in RB Reconfig/RB Affected List
  
DEPENDENCIES
  None

RETURN VALUE
  True, if same RB ID is present in RB Release list and also in RB Reconfig/RB Affected List
  else
  FALSE
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrccu_validate_rel_rb_exist_in_reconfig_list_r8
(
  rrc_CellUpdateConfirm_r8_IEs * cu_confirm_ptr
);

#ifdef FEATURE_WCDMA_HS_FACH
boolean rrccu_get_phy_chan_fail_indication
(
  void
);
#endif

#ifdef FEATURE_WCDMA_REL9

/*===========================================================================
FUNCTION rrcu_process_cu_cnf_after_validation_r9

DESCRIPTION
  This function processes the REl 9 CELL UPDATE CONFIRM message after doing the initial validation
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void rrcu_process_cu_cnf_after_validation_r9
(
  void *msg_ptr,                           /* Pointer to Downlink Msg */
  rrc_CellUpdateConfirm_r9_IEs *cu_confirm_ptr,
  uint32 dl_sdu_type                       /* Downlink SDU type */  
  ,rrcrb_directed_cell_info_type          directed_cell_info
);

/*===========================================================================
FUNCTION rrccu_process_after_oc_config_set_r9

DESCRIPTION
  This function processes the REl 9 CELL UPDATE CONFIRM after OC is set by CU CNF.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void rrccu_process_after_oc_config_set_r9
(
  rrc_CellUpdateConfirm_r9_IEs *cu_confirm_ptr
  ,rrcrb_directed_cell_info_type          directed_cell_info
);


/*===========================================================================
FUNCTION rrccu_process_freq_redirection_info_r9

DESCRIPTION
  This function processes the REl 9 CELL UPDATE CONFIRM message for Freq Redirection.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean rrccu_process_freq_redirection_info_r9
(
  rrc_CellUpdateConfirm_r9_IEs * cu_confirm_ptr,
  rrcrb_directed_cell_info_type * directed_cell_info_ptr
);


/*===========================================================================

FUNCTION rrccu_validate_rb_info_r9

DESCRIPTION
  This function validates the Rb related IEs in Cu Cnf
  Cell Update message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrccu_validate_rb_info_r9
(
   rrc_CellUpdateConfirm_r9_IEs * cu_confirm_ptr
);

/*===========================================================================
FUNCTION rrccu_update_chan_config_rb_r9

DESCRIPTION
 
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
uint32 rrccu_update_chan_config_rb_r9
(
 rrc_CellUpdateConfirm_r9_IEs * cu_confirm_ptr,
 rrc_rb_config_type * rb_info
);

/*===========================================================================
FUNCTION rrccu_validate_rel_rb_exist_in_reconfig_list_r9

DESCRIPTION
  This function checks if the Cu Cnf has the same RB ID in the RB Release list
   and also in RB Reconfig/RB Affected List
  
DEPENDENCIES
  None

RETURN VALUE
  True, if same RB ID is present in RB Release list and also in RB Reconfig/RB Affected List
  else
  FALSE
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrccu_validate_rel_rb_exist_in_reconfig_list_r9
(
  rrc_CellUpdateConfirm_r9_IEs * cu_confirm_ptr
);

/*===========================================================================

FUNCTION RRCCU_RESPONSE_TO_CELL_UPDATE_CONFIRM_R9

DESCRIPTION
  This function evaluates CELL UPDATE CONFIRM message and decides what
  response message has to be sent
  Cell Update message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
rrccu_resp_msg_e_type rrccu_find_response_to_cell_update_confirm_r9
(
  rrc_CellUpdateConfirm_r9_IEs *cucnf_ptr
);


/*===========================================================================
FUNCTION RRCCU_PROCESS_CELL_UPDATE_CONFIRM_R9

DESCRIPTION
  This function processes the REl 9 CELL UPDATE CONFIRM message received in response to
  Cell Update message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void rrccu_process_cell_update_confirm_r9
(    
  void *msg_ptr,                           /* Pointer to Downlink Msg */
  rrc_RRC_TransactionIdentifier rcvd_transaction_id,
  rrc_CellUpdateConfirm_r9_IEs *cu_confirm_ptr,
  uint32 dl_sdu_type                       /* Downlink SDU type */
);

#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */

/*===========================================================================
FUNCTION rrccu_log_cu_status

DESCRIPTION   This function logs the CU status

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void rrccu_log_cu_status(rrc_cu_uu_status_e_type cu_status, boolean is_cu);
/*===========================================================================
FUNCTION   rrccu_map_cu_uu_cause_for_event_logging

DESCRIPTION   This function maps the CU and UU legacy causes to rrc_cu_uu_cause_e_type type for event logging

DEPENDENCIES  None

RETURN VALUE  rrc_cu_uu_cause_e_type

SIDE EFFECTS  None
===========================================================================*/
rrc_cu_uu_cause_e_type rrccu_map_cu_uu_cause_for_event_logging(boolean is_cu);
/*===========================================================================
FUNCTION   rrccu_get_cu_uu_failure_cause_for_event_logging

DESCRIPTION   This function maps the CU and UU legacy failure causes to rrc_cu_uu_failure_cause_e_type type for event logging

DEPENDENCIES  None

RETURN VALUE  rrc_cu_uu_failure_cause_e_type

SIDE EFFECTS  None
===========================================================================*/
rrc_cu_uu_failure_cause_e_type rrccu_get_cu_uu_failure_cause_for_event_logging(boolean is_cu);

                         
#endif /* RRCCUI_H */ 
