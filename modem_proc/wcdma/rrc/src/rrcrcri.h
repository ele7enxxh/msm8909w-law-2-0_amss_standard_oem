#ifndef RRCRCRI_H
#define RRCRCRI_H
/*===========================================================================
              R R C R C R I  H E A D E R  F I L E

DESCRIPTION
 The RRCRCR module consists of RRC CONNECTION RELEASE PROCEDURE 
 implementation. The 3GPP TS 25.331 RRC Protocol specification is used as
 basis for implementation. 
  
Copyright (c) 2001-2003, 2005-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrcri.h_v   1.5   14 Feb 2002 14:58:30   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrcri.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/03/14   sp      Made changes to save procedure substates in crash debug info
12/28/12   sn      Changes to reduce wait for l2 ack sanity timer from 29s to 10s 
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/17/10   rl      Fixed the compiler error
11/03/10   rl      Fixed review comments
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
11/04/09   sks     Added code to support inter-RAT redirection via connection reject
                   and both inter-RAT and inter-freq redirection via connection release.
05/08/09   ss      Updated Copyright Information
09/25/06   vm      Removed prototype of function 
                   rrcrcr_send_rrc_channel_config_req_for_srb_release().
                   For details pls see CR 102193.
11/09/05   kc      Removed updating release cause from internal header and moved it to
                   rrcrcr.h, since rrcpg1 will also use this header
02/14/02   rj      Added a new substate RRCRCR_WAIT_FOR_CELL_UPDATE_COMP_CNF
                   for CELL_FACH state implementation.Added prototype for
                   rrcrcr_process_rcr_wait_for_cell_update_comp_cnf_substate.
08/03/01   rj      Changed L2 ACK sanity timer to 29 secs since upper layer
                   (CC) has a timer value 30 secs.
07/27/01   rj      Moved function prototype rrcrcr_send_rrc_rel_ind to 
                   RRCRCE module. Added code to set the boolean whether
                   RRC_CHANNEL_CONFIG_CNF is required for RRC_CHANNLE_CONFIG_REQ.
02/26/01   rj      Implemented flow chart review comments. Deleted substate
                   RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_STATUS and function
                   rrcrcr_process_rcr_wait_for_l2ack_for_rrc_status_substate.
02/12/01   rj      Inlcuded rrcintif.h and rrcmmif.h. Added 
                   rrcrcr_error_indication_status_e_type. Added prototypes for
                   functions rrcrcr_process_disconnected_state,
                   rrcrcr_get_t308_from_serv_cell_db, rrcrcr_convert_t308_to_ms,
                   rrcrcr_send_rrc_channel_config_req_for_srb_release,
                   rrcrcr_send_connection_rel_complete_msg,
                   rrcrcr_update_release_cause, and rrcrcr_send_rrc_rel_ind.
02/06/01   rj      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "wcdma_variation.h"
#include "rrccmd_v.h"
#include "rrcmmif.h"
#include "rrcintif.h"

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/
 /* Sanity timer value is reduced from 29seconds to 10seconds to reduce
    delay in connection release*/
#define RRCRCR_TIMER_VALUE_FOR_L2_ACK 10000 /* This sanity check timer 
                                               to receive L2 ack */

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

/*==========================================================================
  Type for defining the substate of RRC Connection Release Procedure.
===========================================================================*/

typedef enum 
{
  RRCRCR_INITIAL,               /* RCR initial substate  */

  RRCRCR_WAIT_FOR_T308_TIMER_EXPIRE, 
                                /* RCR waits for the Timer T308 to expire */
  RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_CONN_REL_COMPLETE,
                                /* RCR waits for L2 ACK for transmission 
                                   of RRC Connection Release Complete Msg  */
  RRCRCR_WAIT_FOR_CELL_UPDATE_COMP_CNF,
                                /* RCR waits for CELL UPDATE to complete so
                                   that a valid C_RNTI is available to 
                                   transmit in CELL_FACH state */
  RRCRCR_WAIT_FOR_SRB_RELEASE_CNF
                                /* RCR waits for RRC LLC to Release of 
                                   SRBs */
} rrcrcr_substate_e_type;

typedef enum
{
  RRCRCR_SUCCESS,               /* Successful Status */
  RRCRCR_FAILURE_MAJOR,         /* Major Failure status */
  RRCRCR_FAILURE_MINOR          /* Minor Failure Status */
}rrcrcr_status_e_type;

typedef enum
{
  RRCRCR_INCLUDE_ERROR_INDICATION,     /* Indicates to include Error 
                                          Indication */
  RRCRCR_NOT_INCLUDE_ERROR_INDICATION  /* Indicates not to include Error
                                          Indication */
}rrcrcr_error_indication_status_e_type;


/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION rrcrcr_process_disconnected_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in DISCONNECTED state of Idle  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_disconnected_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
);
/*===========================================================================

FUNCTION rrcrcr_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in CELL_FACH state of Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
);
/*===========================================================================

FUNCTION rrcrcr_process_cell_dch_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in CELL_DCH state of Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_cell_dch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
);
/*===========================================================================

FUNCTION rrcrcr_process_rcr_initial_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in RRCRCR_INITIAL substate of either CELL_FACH State or
  CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_initial_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
);
/*===========================================================================

FUNCTION rrcrcr_process_rcr_wait_for_t308_timer_expire_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in RRCRCR_WAIT_FOR_T308_TIMER_EXPIRE  substate of 
  CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_wait_for_t308_timer_expire_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
);

/*===========================================================================

FUNCTION rrcrcr_process_rcr_wait_for_l2ack_for_rrc_conn_rel_complete_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_CONN_REL_COMPLETE
  substate of CELL_FACH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_wait_for_l2ack_for_rrc_conn_rel_complete_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
);
/*===========================================================================

FUNCTION rrcrcr_process_rcr_wait_for_srb_release_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Release procedure in RRCRCR_WAIT_FOR_SRB_RELEASE_CNF substate of either
  CELL_DCH State or CELL_FACH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_wait_for_srb_release_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
);

/*===========================================================================

FUNCTION rrcrcr_convert_t308_to_ms

DESCRIPTION
  This function converts the rrc_T_308 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  int

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrcrcr_convert_t308_to_ms
(
  rrc_T_308 rrcrcr_t_308                   /* enum for Timer T308 */
);


/*===========================================================================

FUNCTION rrcrcr_send_connection_rel_complete_msg

DESCRIPTION
  This function builds RRC CONNECTION RELEASE COMPLETE message and send the
  same to the SEND CHAIN to transmit either in Unacknowledged mode or 
  Acknowledged mode.
DEPENDENCIES
  None

RETURN VALUE
  rrcrcr_state_e_type: It returns RRCRCR_SUCCESS if it successfully places 
  RRC CONNECTION RELEASE COMPLETE SDU to the SEND CHAIN. Otherwise it returns
  RRCRCR_FAILURE_MAJOR

SIDE EFFECTS
  None
===========================================================================*/
rrcrcr_status_e_type rrcrcr_send_connection_rel_complete_msg
(
  rrc_RB_Identity rb_id,                    /* Radio bearer Id */ 
  uecomdef_logch_mode_e_type mode,          /* Mode of transmit */  
  rrcrcr_error_indication_status_e_type error_ind_status
                                            /* Indicates whether to include
                                               Error Indication field in the 
                                               RRC Connection Rel comlete Msg*/                                         
);

/*===========================================================================

FUNCTION rrcrcr_process_rcr_wait_for_cell_update_comp_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in RRCRCR_WAIT_FOR_CELL_UPDATE_COMP_CNF  substate of 
  CELL_FACH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_wait_for_cell_update_comp_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
);

/*==========================================================================
FUNCTION rrcrcr_save_redirection_info

DESCRIPTION
  This function saves the redirection info received in RRC
  Connection Release message. This is info is then later passed on to
  CSP for further processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_save_redirection_info
(
  rrc_RedirectionInfo_r6 *redirection_info_ptr
);

/*==========================================================================
FUNCTION rrcrcr_send_redirection_req

DESCRIPTION
  This function builds the RRC_REDIRECT_REQ command with all parameters
  received through RRC CONNECTION RELEASE message and sends the command to
  Cell Selection Procedure
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcrcr_send_redirection_req
(
  void
);
/*===========================================================================
FUNCTION rrcrcr_get_current_sub_state()
       
DESCRIPTION
  This function will return current RRC Connection Release 
  procedure's sub state.
DEPENDENCIES
  None

RETURN VALUE
  return current rcr sub state.
SIDE EFFECTS
none
===========================================================================*/
rrcrcr_substate_e_type  rrcrcr_get_current_sub_state(void);

#ifdef FEATURE_WCDMA_TO_LTE
/*========================================================================
FUNCTION rrcrcr_save_lte_redirection_info

DESCRIPTION
  This function saves the LTE redirection info received in RRC
  Connection Release message. This info is then later passed on to
  CSP for further processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void rrcrcr_save_lte_redirection_info
(
  rrc_EUTRA_TargetFreqInfoList *EUTRA_target_freq_info_ptr
);
#endif
/*========================================================================
FUNCTION rrc_qsh_log_conn_rel_params

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void rrc_qsh_log_conn_rel_params(rrc_log_rrc_conn_rel_ota *conn_rel_params);

#endif /* RRCRCRI_H */

