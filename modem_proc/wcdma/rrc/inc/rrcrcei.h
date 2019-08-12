#ifndef RRCRCEI_H
#define RRCRCEI_H
/*===========================================================================
              R R C R C E I  H E A D E R  F I L E

DESCRIPTION
 The RRCRCE module consists of RRC CONNECTION ESTABLISHMENT PROCEDURE 
 implementation. The 3GPP TS 25.331 RRC Protocol specification is used as
 basis for implementation. 
  
Copyright (c) 2001,2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrcei.h_v   1.24   03 Jun 2002 11:30:38   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcrcei.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/14   sp      Made changes for Mav debug snapshot
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
01/03/13   ad      Made changes to reduce the redundant F3s
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/18/10   as      Fixed compiler warnings
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss      Updated Copyright Information
09/22/07   pk      Made the return type of rrcrce_send_rrc_connection_request_msg() void.            .
06/15/07   kp      Added a new macro for starting 3sec Delay timer in sending Signalling
                   connection release as part of removing GCF NV flag.
06/05/07   pk      Added support to validate the cell access status for the requested 
                   domain before procedding further if one of the following happens during
                   the connection setup process.
                   1. Successful cell reselection.
                   2. NW Redirection to the redirected freq/RAT fails.
05/24/06   segu    A new function prototype i.e. rrcrce_validate_cell_access_status()
                   is added that validates current cell access status against the
                   cause received in RRC_EST_REQ
11/22/05   sgk     Added define RRCRCE_ZERO_TIMER_VALUE_FOR_RRC_CON_REL 0
09/28/05   vm      Added support for redirection to another frequency + psc via
                   RRC Connection Setup message. Modified the type of arguments
                   passed to function rrcrce_send_redirection_req().
07/05/05   vm      Added support for new feature FEATURE_FACH_FREQ_REDIRECTION_IN_CONN_SETUP.
                   Added new enum type rrcrce_conn_rsp_e_type and modified the type and number
                   of arguments passed to rrcrce_send_redirection_req() function.
04/05/05   da      Defined RRCRCE_TIMER_VALUE_FOR_SIG_CON_REL as 40s.
04/04/05   bu      Added support for inter-RAT redirection feature
                   . Added new RCE substates: RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF.
                   . Added prototype for new function
                     rrcrce_process_rce_wait_for_rb0_phychan_resetup_cnf_substate().
11/24/04   da      Added RRCTMR_PHYCHANFAILURE_TIMER_IN_MS for physical channel fail
                   timer.
11/18/04   vk      Declared timers to delay RRC Connecton Release. Declared
                   new sub-state RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE and
                   fn rrcrce_process_rce_wait_for_rrc_timer_expire_substate()
                   that handles timer expiration
08/30/04   sgk     Removed the prototype for function rrcrce_verify_ue_identity
                   as it is already defined in rrcrce.h
06/07/04   vk      Added prototype for function rrcrce_send_rrc_abort_cnf()
11/19/03   vk      Added a prototype for function 
                   rrcrce_process_rce_wait_for_rb0_rel_cnf_substate and
                   added a new substate RRCRCE_WAIT_FOR_RB0_REL_CNF to
                   RCE state machine
04/24/03   rj      Added a prototype for function 
                   rrcrce_process_rce_wait_for_sig_con_rel_compl_substate and
                   added new substate RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL to
                   RCE state machine.
02/27/03   rj      Added prototype rrcrce_convert_t300_to_ms.
02/04/03   ram     Checked in lint changes, updated copyright.
12/31/02   rj      Added a substate RRCRCE_WAIT_FOR_RESELCT_COMPL  to support
                   cell reselection in Connecting State and added a prototype
                   rrcrce_process_rce_wait_for_reselct_compl_substate for that
                   substate.
06/03/02   rj      Added prototype rrcrce_process_rce_wait_for_sib7_current.
06/02/02   rj      Added additional substate RRCRCE_WAIT_FOR_SIB7_CURRENT to
                   not to send first RACH message on UL until SIB7 is current.
03/04/02   rj      Added substate RRCRCR_WAIT_FOR_SRB_REL_CNF to wait until
                   RRC goes Idle Disconnected state and then send RRC_ABORT_CNF
                   in response to RRC_ABORT_REQ command from MM. Added 
                   prototype for rrcrce_process_rce_wait_for_srb_rel_cnf_substate.
02/28/02   rj      Added additional formal parameter destination state for
                   function prototype rrcrce_send_rrc_initiate_cell_selection_ind.                   
02/14/02   rj      Updated copyright for 2002.
02/14/02   rj      Added a new substate RRCRCE_WAIT_FOR_CU_COMPLETE_CNF for 
                   CELL_FACH state implementation and added prototypes for
                   rrcrce_process_rce_wait_for_cu_complete_cnf_substate and
                   rrcrce_send_cell_update_initiate_req.
10/17/01   rj      Added prototype rrcrce_send_rrc_initiate_cell_selection_ind
                   to trigger cell selection.
09/14/01   rj      Updated comments for function rrcrce_compare_arrays.
09/06/01   rj      Deleted rrcrce_get_max_no_of_reported_cells_on_rach function
                   prototype since this functionality is moved to RRCMEAS module.
08/30/01   rj      Added lengths for all IEs in number of bits for RRC 
                   Connection Setup and RRC Connection Reject to peek into
                   OTA message. Modified formal parameters for functions
                   rrcrce_verify_ue_identity, 
                   rrcrce_validate_rrc_connection_setup_msg and
                   rrcrce_append_start_list.
08/08/01   rj      Removed the code under Feature FEATURE AL1 since it is
                   no longer required.
08/03/01   rj      Changed L2 ACK sanity timer to 29 secs since upper layer
                   (CC) has a timer value 30 secs.
07/26/01   rj      Added prototypes for functions rrcrce_send_rrc_rel_ind,
                   rrcrce_process_rce_wait_for_wait_time_expire_substate, and
                   rrcrce_process_rce_wait_for_redirect_cnf_substate. Added a
                   additional paramter to indicate whether confirmation from 
                   RRC LLC is  required or not to the functions
                   rrcrce_send_rrc_channel_config_req_for_rb0 and
                   rrcrce_send_rrc_channel_config_req_for_srb.
07/20/01   rj      Since functions rrcrce_append_ue_radio_access_capability and 
                   rrcrce_append_gsm_specific_capability are moved to RRCUECI
                   module and function rrcrce_copy_capability_update_requirement
                   is moved to RRCUECE module, the declarations for the same are
                   deleted.
06/29/01   rj      Increased RRCRCE_TIMER_VALUE_FOR_L2_ACK value to 2 minutes
                   from 1 sec for DCH debugging. This should be undone once
                   DCH setup is successful.
06/28/01   rj      Deleted code that was wrapped with feature 
                   FEATURE_RRC_RELEASE_1_0_BETA. This code was processing
                   RRC Connection Reject message.
06/22/01   ram     Moved the branched change for VST back to mainline
                   since VST testing will be continued for a long time.
06/08/01   rj      Added 2 minute L2 Ack sanity timer for VST testing.
06/06/01   rj      Changed formal parameters to the functions 
                   rrcrce_append_ue_radio_access_capability and 
                   rrcrce_append_gsm_specific_capability.
05/15/01   rj      Added define constant RRCRCE_INVALID_TRANSACTION_ID to 
                   initialize Transaction Ids on RRCRCE proc initiation.
04/30/01   rj      Deleted rrcrce_free_capability_update_requirement function
                   since it is no longer required after UE Capability
                   Update Requirement changed to internal format.
03/30/01   rj      Modified prototypes for rrcrce_update_initial_ue_identity,
                   rrcrce_update_establishment_cause,
                   rrcrce_send_rre_est_cnf.Added new function prototypes 
                   rrcrce_send_rrc_anort_ind and rrcrce_validate_rrc_est_req.
02/21/01   rj      Added #define constants RRCRCE_MCC_LENGTH and 
                   RRCRCE_MNC_LENGTH for the updates to the function
                   rrcrce_update_initial_ue_identity_choice. Modified the 
                   value for RRCRCE_SAMPLE_START_VALUE to match Anritsu
                   test suite. 
02/12/01   rj      Fixed some typos in the comments.
02/08/01   rj      Updated comments. Deleted rrcrce_send_rrc_channel_config_req
                   and renamed rrcrce_resend_rrc_connection_request_msg
                   function to rrcrce_send_rrc_connection_request_msg. Added 
                   rrcrce_send_rrc_channel_config_req_for_srb.
01/25/01   rj      Moved all declarations that internal to RRCRCE module to
                   this file.
01/18/01   rj      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "comdef.h"

#include "queue.h"

#include "rex.h"

#include "rrccmd_v.h"

#include "uecomdef.h"

#include "rrcasn1.h"
/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

/*  NAS times out after 15 seconds if it did not receive response for 
    RRC_EST_REQ. The timer for UE to camp on is selected 14 seconds */
#define RRCRCE_TIMER_VALUE_FOR_UE_TO_CAMP_ON  14000 
#define RRCRCE_TIMER_VALUE_FOR_RRC_CON_REL  3000
#define RRCRCE_TIMER_VALUE_FOR_SIG_CON_REL 40000
/* When NAS indicates that it is not a severe scenario, start timer for 2 secs */
#define RRCRCE_TIMER_VALUE_SMALL_FOR_RRC_CON_REL  2000
#define RRCRCE_ZERO_TIMER_VALUE_FOR_RRC_CON_REL 0  

/*Delay time for sending  Signalling connection release*/
#define RRCRCE_DELAY_TIMER_VALUE_FOR_SIG_CON_REL 3000  

/* This sanity timer value is selected to 29 secs since Upper layer (CC) has
   a timer for value 30 secs. */
#define RRCRCE_TIMER_VALUE_FOR_L2_ACK    29000

/* Wait time for receiving a new cell indication when initiating Cell selection a
fter physical channel failure happened. */
#define RRCTMR_PHYCHANFAILURE_TIMER_IN_MS 10000

#define RRCRCE_INVALID_TRANSACTION_ID 0xFF /* This used to initialize RRCRCE
                                              Transaction Id */

/* The maximum number of digits in MCC */
#define RRCRCE_MCC_LENGTH  3
/* The maximum number of digits in MNC */
#define RRCRCE_MNC_LENGTH  3

/* The following Bit lengths  are for December version of 25.331 specification. 
   If ASN1 for future revisions changes, then these values needs to be 
   updated accordingly */

#define NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE 0x01
                                          /* Number of bits for Critical
                                             externsion choice */
#define NUM_OF_BITS_FOR_NON_CRITIAL_EXT_PRESENT 0x01
                                          /* Number of bits for Non-Critical
                                             externsion choice */
#define NUM_OF_BITS_FOR_RRC_CONN_SETUP_OPTIONAL_FIELDS 0x0A
                                          /* Number of bits for Optional fields
                                             in RRC Connection Setup message */
#define NUM_OF_BITS_FOR_RRC_CONN_REJECT_OPTIONAL_FIELDS 0x01
                                          /* Number of bits for Optional fields
                                             in RRC Connection Reject message */
#define NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE 0x03
                                          /* Number of bits for Initial UE Id
                                             choice */
#define NUM_OF_BITS_FOR_VARIABLE_IMSI_LENGTH 0x04
                                          /* Number of bits for varibale IMSI 
                                             length */
#define NUM_OF_BITS_FOR_FIXED_IMSI_LENGTH 0x06
                                          /* Number of bits for fixed IMSI 
                                             length */
#define NUM_OF_BITS_FOR_DIGIT  0x04       /* Number of bits for a digit */ 

#define NUM_OF_BITS_FOR_TMSI  0x20        /* Number of bits for TMSI */

#define NUM_OF_BITS_FOR_PTMSI 0x20        /* Number of bits for PTMSI */

#define NUM_OF_BITS_FOR_OPTIONAL_MNC  0x01/* Number of bits for optional MNC 
                                             digit */

/*The reason why RCE procedure has its own macro for detecting RCE OTA is because it's not Reconfig
OTA. Otherwise, we would have used RRCRB_R8_CRITICAL_EXT_PRESENT_PTR from rrcrbcommon.h*/

#define RRCRCE_R8_CRITICAL_EXT_PRESENT \
  ((msg_ptr->message.u.rrcConnectionSetup.u.  \
    later_than_r3.criticalExtensions.t ==  \
    T_rrc_RRCConnectionSetup_criticalExtensions_5_criticalExtensions) && \
    (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_4_criticalExtensions) && \
    (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_3_criticalExtensions) &&\
     (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_2_criticalExtensions) &&\
     (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_1_r8)) 

                                             

#ifdef FEATURE_WCDMA_REL9

#define RRCRCE_R9_CRITICAL_EXT_PRESENT \
  ((msg_ptr->message.u.rrcConnectionSetup.u.  \
    later_than_r3.criticalExtensions.t ==  \
    T_rrc_RRCConnectionSetup_criticalExtensions_5_criticalExtensions) && \
    (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_4_criticalExtensions) && \
    (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_3_criticalExtensions) &&\
     (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_2_criticalExtensions) &&\
     (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_1_later_r8_criticalExtensions) && \
     (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_later_r8_criticalExtensions_r9)) 
#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */


                                             
/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

/*==========================================================================
  Type for defining the substate of RRC Connection Establishment Procedure.
===========================================================================*/

typedef enum 
{
  RRCRCE_INITIAL,               /* RCE initial sub state              */
  RRCRCE_WAIT_FOR_UE_CAMP_ON,   /* RCE waits for the UE to camp on */
  RRCRCE_WAIT_FOR_SIB7_CURRENT, /* RCE waits for SIB & to become current */
  RRCRCE_WAIT_FOR_RB0_SETUP_CNF,/* RCE waits for RB0 conf from RRC LLC     */
  RRCRCE_WAIT_FOR_RRC_CONN_SETUP,/* RCE waits for RRC Connection Setup Msg  */
  RRCRCE_WAIT_FOR_RESELCT_COMPL, /* RCE waits for reselection to complete */
  RRCRCE_WAIT_FOR_REDIRECT_CNF, /* RCE waits for conf from CSP procedure   */
  RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE, 
                                /* RCE waits for timer "Wait Time" to expire*/
  RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE, 
                                /* RCE waits for timer to expire*/
  RRCRCE_WAIT_FOR_SRB_SETUP_CNF,/* RCE waits for Signalling RBs setup conf */
  RRCRCE_WAIT_FOR_CU_COMPLETE_CNF,  
                                /* RCE waits for Cell Update to complete and 
                                   a valid C-RNTI is available to transmit */
  RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL,
                                /* RCE waits for Signalling Connection 
                                   Release  */
  RRCRCE_WAIT_FOR_L2_ACK,       /* RCE waits for L2-ACK for RRC Connection */
                                /* Setup Complete message */
  RRCRCR_WAIT_FOR_SRB_REL_CNF,  /* RCE waits for the confrimation SRB 
                                   release */                                  
  RRCRCE_FINAL,                  /* RCE remains in this sub state until RRC */
                                /* Connection is released */
  RRCRCE_WAIT_FOR_RB0_REL_CNF,
  RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF /* Physical channel resetup for RBO after
                                           * Inter-RAT redirection failure
                                           */
} rrcrce_substate_e_type;

/* Below #defines are added for logging purpose only */
#define rrcrce_substate_e_type_value0 RRCRCE_INITIAL
#define rrcrce_substate_e_type_value1 RRCRCE_WAIT_FOR_UE_CAMP_ON
#define rrcrce_substate_e_type_value2 RRCRCE_WAIT_FOR_SIB7_CURRENT
#define rrcrce_substate_e_type_value3 RRCRCE_WAIT_FOR_RB0_SETUP_CNF
#define rrcrce_substate_e_type_value4 RRCRCE_WAIT_FOR_RRC_CONN_SETUP
#define rrcrce_substate_e_type_value5 RRCRCE_WAIT_FOR_RESELCT_COMPL
#define rrcrce_substate_e_type_value6 RRCRCE_WAIT_FOR_REDIRECT_CNF
#define rrcrce_substate_e_type_value7 RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE
#define rrcrce_substate_e_type_value8 RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE
#define rrcrce_substate_e_type_value9 RRCRCE_WAIT_FOR_SRB_SETUP_CNF
#define rrcrce_substate_e_type_value10 RRCRCE_WAIT_FOR_CU_COMPLETE_CNF 				  
#define rrcrce_substate_e_type_value11 RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL
#define rrcrce_substate_e_type_value12 RRCRCE_WAIT_FOR_L2_ACK						  
#define rrcrce_substate_e_type_value13 RRCRCR_WAIT_FOR_SRB_REL_CNF	 
#define rrcrce_substate_e_type_value14 RRCRCE_FINAL
#define rrcrce_substate_e_type_value15 RRCRCE_WAIT_FOR_RB0_REL_CNF
#define rrcrce_substate_e_type_value16 RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF

typedef enum
{
  RRCRCE_CONNECTION_REJECT,     /* Inter-Freq or Inter-RAT redirection received by 
                                 * the UE in RRC CONNECTION REJECT message */
  RRCRCE_CONNECTION_SETUP       /* Inter-Freq redirection received by 
                                 * the UE in RRC CONNECTION SETUP message */
}rrcrce_conn_rsp_e_type;


/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION rrcrce_process_disconnected_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in Disconnected state of Idle mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_disconnected_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_connecting_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in Connecting state of Idle mode. The connecting 
  state is a transient state created to honor Paging Type 1 message during the
  RRC Connection Establishment procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_connecting_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in CELL_FACH state of Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_cell_dch_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in CELL_DCH state of Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_cell_dch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_cell_ura_pch_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure either in CELL_PCH state or URA_PCH state of 
  Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_cell_ura_pch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_rce_initial_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_INITIAL substate of Disconnected State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_initial_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_ue_camp_on_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_UE_CAMP_ON  substate of 
  Disconnected State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_ue_camp_on_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_rb0_setup_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_RB0_SETUP_CNF substate of 
  Disconnected State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_rb0_setup_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_conn_setup_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_CONN_SETUP substate of Connecting
  State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_conn_setup_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_srb_setup_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_SRB_SETUP_CNF substate of 
  Connecting State
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_srb_setup_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_l2_ack_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_L2_ACK_CNF substate of either 
  CELL_FACH or CELL_DCH State.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_l2_ack_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================
FUNCTION rrcrce_process_rce_final_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE__FINAL substate of either CELL_FACH or
  CELL_DCH State.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_final_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

/*===========================================================================

FUNCTION rrcrce_build_rrc_connection_request_msg

DESCRIPTION
  This function builds the RRC CONNECTION REQUEST message with all required
  parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_build_rrc_connection_request_msg
(
  rrc_UL_CCCH_Message *msg_ptr               /* Pointer to uplink CCCH message*/
);

/*===========================================================================

FUNCTION rrcrce_compare_arrays

DESCRIPTION
  This function checks whether two arrays of specified length are equal or
  not. This function uses the standard library memcmp function.

DEPENDENCIES
  This function converts type of arrays into charecter arrays before comparing.
  Hence length should be multiplied suitably if integer or any other types are
  compared.

RETURN VALUE
  TRUE if both are same. otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcrce_compare_arrays
(
  void *str1,                               /* First array */
  void *str2,                               /* Second array */
  uint32 length                             /* Length of the array */
);
/*===========================================================================

FUNCTION rrcrce_send_rrc_connection_request_msg

DESCRIPTION
  This function builds the RRC CONNECTION REQUEST message and places the same
  in the watermark queue to transmit OTA. Even if for some reason connection 
  request is not sent, then also T300 will be started so that sending connection 
  request can be retried after its expiry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_connection_request_msg
(
  void
);
/*===========================================================================
FUNCTION rrcrce_send_rrc_connection_setup_complete_message

DESCRIPTION
  This function builds the RRC CONNECTION SETUP COMPLETE message and places
  the same in the watermark queue for transmission.
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : It returns RRCRCE_SUCCESS if it successfully builds 
  the RRC CONNECTION SETUP COMPLETE message and calls the Send Chain. 
  Otherwise it returns RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_send_rrc_connection_setup_complete_message
(
  void
);
/*===========================================================================

FUNCTION rrcrce_get_t300_from_serv_cell_db

DESCRIPTION
  This function reads the serving cell DB and returns the T300 value from the
  UE Timers and Constants in idle mode field

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : Returns RRCRCE_SUCCESS If it successful retrieves
  T300 from the SIB database. Otherwise it returns RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_get_t300_and_n300_from_serv_cell_db
(
  rrc_T_300 *rrcrce_t_300                     /* Pointer to T300 */ 
);
/*===========================================================================

FUNCTION rrcrce_get_n300_from_serv_cell_db

DESCRIPTION
  This function reads the serving cell DB and returns the N300 value from the
  UE Timers and Constants in idle mode field

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : Returns RRCRCE_SUCCESS If it successful retrieves
  N300 from the SIB database. Otherwise it returns RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_get_n300_from_serv_cell_db
(
  rrc_N_300 *rrcrce_n_300                     /* Pointer to N300 */  
);

/*===========================================================================
FUNCTION rrcrce_append_start_list

DESCRIPTION
  This function appends the START list to the RRC CONNECTION SETUP COMPLETE 
  message.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcrce_append_start_list
(
  struct rrc_STARTList *start_list_ext     /* Pointer to STARTList*/
);

/*===========================================================================
FUNCTION rrcrce_send_redirection_req

DESCRIPTION
  This function builds the RRC_REDIRECT_REQ command with all parameters
  received through RRC CONNECTION REJECT message and sends the command to
  Cell Selection Procedure
  
DEPENDENCIES
  None

RETURN VALUE
  none
SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_redirection_req
(
  rrc_DL_CCCH_Message   *msg_ptr,
  boolean                psc_info_present
);

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================
FUNCTION rrcrce_send_lte_redirection_req

DESCRIPTION
  This function builds the RRC_REDIRECT_REQ command with all parameters
  received through RRC CONNECTION REJECT message and sends the command to
  Cell Selection Procedure
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_lte_redirection_req
(
  rrc_DL_CCCH_Message    *msg_ptr
);
#endif

/*===========================================================================

FUNCTION rrcrce_send_rrc_est_cnf

DESCRIPTION
  This function builds RRC_EST_CNF command to post to MM with 
  success  status of RRC Connection Establishment procedure.
DEPENDENCIES
  None

RETURN VALUE
void

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_est_cnf
(
  void
);

/*===========================================================================

FUNCTION rrcrce_send_rrc_abort_ind

DESCRIPTION
  This function builds RRC_ABORT_IND command to post to MM with failure 
  status of RRC Connection Establishment procedure.
DEPENDENCIES
  None

RETURN VALUE
void

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_abort_ind
(
  rrc_abort_cause_e_type cause               /* Cause for RRC Connection 
                                                Failure */
);


/*==========================================================================

FUNCTION rrcrce_update_initial_ue_identity

DESCRIPTION
  This function updates contents of Initial UE Identity Choice and Initial
  UE Identity received from NAS RRC_EST_REQ primitive.
DEPENDENCIES
  None

RETURN VALUE
  None: 

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_update_initial_ue_identity
(
  rrc_InitialUE_Identity *rrc_initial_ue_identity_ext
                                             /* Initial UE Indentity to be 
                                                sent over the air */
);
/*==========================================================================

FUNCTION rrcrce_update_establishment_cause

DESCRIPTION
  This function gets equivalent external RRC Connection Establishment cause
  for the internal RRC Connection Established cause received from the NAS 
  through RRC_EST_REQ primitive.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
rrc_EstablishmentCause rrcrce_update_establishment_cause
(
  void
);
/*===========================================================================

FUNCTION rrcrce_send_rrc_channel_config_req_for_rb0

DESCRIPTION
  This function builds RRC_CHANNEL_CONFIG_REQ command and  requests RRC LLC
  to either setup or modify or release  Radio Bearer 0.
DEPENDENCIES
  None

RETURN VALUE
void

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_channel_config_req_for_rb0
(
               
  rrc_rb_cfg_e_type action,                   /* Type of action RB 0 */
  rrc_state_e_type next_state,                /* Next state to transition is
                                                 expected to take place */
  boolean confirm_required                    /* This indiactes whether
                                                 RRC_CHANNEL_CONFIG_CNF is 
                                                 required or not */
);
/*===========================================================================

FUNCTION rrcrce_send_rrc_channel_config_req_for_srb

DESCRIPTION
  This function builds RRC_CHANNEL_CONFIG_REQ command and  requests RRC LLC
  to either setup or modify or release  Signalling Radio Bearers 1 through 4.
DEPENDENCIES
  None

RETURN VALUE
void

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_channel_config_req_for_srb
(
               
  rrc_rb_cfg_e_type action,                   /* Type of action on SRBs */
  rrc_state_e_type next_state,                /* Next state to transition is
                                                 expected to take place */
  boolean confirm_required                    /* This indiactes whether
                                                 RRC_CHANNEL_CONFIG_CNF is 
                                                 required or not */
);
/*===========================================================================

FUNCTION rrcrce_validate_rrc_connection_setup_msg

DESCRIPTION
  This function validates whether RRC CONNECTION SETUP message contain min.
  RB info to setup RRC Connection.

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : It returns RRCRCE_SUCCESS if it contains RB 1,RB 2,
  RB 3 and optionally RB 4 info. Otherwise it returns RRCRCE_FAILURE_MINOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_validate_rrc_connection_setup_msg
(
  rrc_DL_CCCH_Message *msg_ptr,              /* Pointer to Downlink CCCH Msf */
  uint32 dl_sdu_type,                         /* Downlink SDU type */
  rrc_msg_ver_enum_type* msg_version_ptr
);
/*===========================================================================

FUNCTION rrcrce_validate_rrc_est_req

DESCRIPTION
  This function checks whether RRC_EST_REQ contains a valid Initial UE
  indentity.
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: It returns RRCRCE_SUCCESS if it finds a valid
  Initial UE Identity in RRC_EST_REQ command to the MM.
  Otherwise it returns RRCRCE_FAILURE_MINOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_validate_rrc_est_req
(
  rrc_cmd_type *cmd_ptr /* Pointer to RRC Command */
);

/*===========================================================================

FUNCTION rrcrce_validate_access_class

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: It returns RRCRCE_SUCCESS if it finds a valid
  Initial UE Identity in RRC_EST_REQ command to the MM.
  Otherwise it returns RRCRCE_FAILURE_MINOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_validate_cell_access_status
(
  rrc_cn_domain_identity_e_type   rrc_cn_domain_id,
  rrc_establish_cause_e_type   establishment_cause,
  rrc_abort_cause_e_type  *cause              /* cause */
);

/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_redirect_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_REDIRECT_CNF substate of 
  Connecting State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_redirect_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the received 
                                                command */
);
/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_rrc_timer_expire_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_WAIT_TIME_EXPIRE substate of 
  Connecting State
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_rrc_timer_expire_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_wait_time_expire_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_WAIT_TIME_EXPIRE substate of 
  Connecting State
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_wait_time_expire_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

/*===========================================================================

FUNCTION rrcrce_send_rrc_rel_ind

DESCRIPTION
  This function builds RRC_REL_IND command to post to MM with the 
  RRC Connection Release cause.
DEPENDENCIES
  None

RETURN VALUE
none

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_rel_ind
(
  void
);

/*===========================================================================

FUNCTION rrcrce_send_rrc_initiate_cell_selection_ind

DESCRIPTION
  This function builds RRC_INITIATE_CELL_SELECTION_IND command to post to
  CSP procedure with the cause specified.
DEPENDENCIES
  None

RETURN VALUE
none

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_initiate_cell_selection_ind
(
  rrc_cell_sel_cause_e_type cause,           /* Cause to trigger Cell
                                                selection */
  rrc_state_e_type dest_state                /* Destination State */
);

/*===========================================================================

FUNCTION rrcrce_send_cell_update_initiate_req

DESCRIPTION
  This function send the cell update initiate request if UE does not have
  a valid C-RNTI in CELL_FACH state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrce_send_cell_update_initiate_req
(
  rrc_proc_e_type procedure
);
/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_cu_complete_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT__FOR_CU_COMPLETE_CNF sub state of 
  CELL_FACH State
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_cu_complete_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_srb_rel_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCR_WAIT_FOR_SRB_REL_CNF substate of any RRC 
  state.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_srb_rel_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_sib7_current

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_SIB7_CURRENT  substate of 
  Disconnected State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_sib7_current
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_reselct_compl_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_RESELCT_COMPL substate of 
  Connecting State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_reselct_compl_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the received 
                                                command */
);
/*===========================================================================

FUNCTION rrcrce_convert_t300_to_ms

DESCRIPTION
  This function converts the rrc_T_300 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  int

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrcrce_convert_t300_to_ms
(
  rrc_T_300 rrcrce_t_300                   /* enum for Timer T300 */
);
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
void rrcrce_process_rce_wait_for_sig_con_rel_compl_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_rb0_rel_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_RB0_REL_CNF substate of any RRC 
  state.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_rb0_rel_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);
/*===========================================================================

FUNCTION rrcrce_send_rrc_abort_cnf

DESCRIPTION
  This function builds RRC_ABORT_CNF command to post to MM in response to 
  RRC_ABORT_REQ.
DEPENDENCIES
  None

RETURN VALUE
void

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_abort_cnf
(
  void
);

/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_rb0_phychan_resetup_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF  substate of 
  Connecting State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_rb0_phychan_resetup_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);

#endif /* RRCRCEI_H */

