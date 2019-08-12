#ifndef RRCUMI_H
#define RRCUMI_H
/*===========================================================================
                  R R C  UTRAN MOBILITY INFORMATION

DESCRIPTION
  This header file contains function prototypes for UTRAN Mobility Info
  procedure

Copyright (c) 2002, 2003, 2007-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcumi.h_v   1.1   04 Mar 2002 14:36:26   kiranc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcumi.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/15   ad      Made changes to log important parms from OTAs
02/19/14   sp      Made changes for Mav debug snapshot
09/03/14   sp      Made changes to save procedure substates in crash debug info
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
05/24/10   ss      Corrected typo for EATURE_SRNS_RELOCATION to FEATURE SRNS RELOCATION
05/07/10   prk     Fixed compiler warnings.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
05/08/09   ss      Updated Copyright Information
11/11/03   kc      Added UMI support for SRNS relocation.
04/03/02   kc      Added changes for UTRAN Mob Info Confirm message and for
                   RNTI Update Request and Cnf messages
10/17/01   kc      Initial check in for UTRAN Mobility Information.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "rrcasn1.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

/*==========================================================================
  Type for defining the substate of RRC UMI Procedure.
===========================================================================*/

typedef enum
{
  RRCUMI_FAILURE,                /* Failure status */
  RRCUMI_SUCCESS                /* Successful Status */
}rrcumi_status_e_type;

typedef enum 
{
  RRCUMI_INITIAL,                 /* UMI initial substate */
  RRCUMI_WAIT_FOR_RNTI_UPDATE_CNF,       /* wait for rnti update cnf from llc
                                           files which update c_rnti and u_rnti*/
  RRCUMI_WAIT_FOR_CHAN_CFG_CNF,     /* Waiting for confirmation of channel  
                                    config from LLC                        */                      
  RRCUMI_WAIT_FOR_L2_ACK,           /* Waiting for Layer 2 ack for uplink
                                    message transmission                   */
  RRCUMI_WAIT_FOR_REESTABLISH_SRB2, /*waiting for re-establishment of srb2 */
  RRCUMI_MAX_SUBSTATES

} rrcumi_substate_e_type;

/* Indicates whether we received new values for Conn mode UE timers and consts */
typedef enum 
{
  RRCUMI_UE_TIMER_CONST_NOT_RCVD,
  RRCUMI_UE_TIMER_CONST_RCVD
} rrcumi_ue_timer_const_rcvd_status_e_type; 

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION rrcumi_init_procedure

DESCRIPTION
  This function initializes required parameters for UTRAN Mobility Info
  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void rrcumi_init_procedure(void);
/*===========================================================================
FUNCTION rrcumi_get_current_substate()

DESCRIPTION
  This function will return current UMI substate
DEPENDENCIES
  None

RETURN VALUE
  return current umi sub state.
SIDE EFFECTS
none
===========================================================================*/
uint8  rrcumi_get_current_substate(void);


/*===========================================================================

FUNCTION rrcumi_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for UTRAN Mobility Info procedure
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC) containing UTRAN
    Mobility Info message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void rrcumi_procedure_event_handler(rrc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION rrcumi_process_utran_mobility_info_command

DESCRIPTION
  This function reads the contents of the UTRAN Mobility Info and
  sends the SYNC_IND to MM layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcumi_process_utran_mobility_info_command
(
  rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
);

/*===========================================================================

FUNCTION rrcumi_build_utran_mobility_info_confirm_msg

DESCRIPTION
  This function builds the utran mobility info confirm message.
  This function is called by cell update and UMI procedure.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcumi_build_utran_mobility_info_confirm_msg
(
  rrc_utran_mob_info_cnf_type* utran_mob_inf_cnf,
  rrc_UL_DCCH_Message *msg_ptr
);
/*===========================================================================

FUNCTION rrcumi_check_ue_timer_constants_update_status

DESCRIPTION
  Update global rrc_ue_conn_timers_and_constants with timers and constants
  from SIB1.  Function provided for SIB procedure to query if we should
  indeed update connected mode timers and constants database.
  
DEPENDENCIES
  None

RETURN VALUE
  RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_REQ
  :  If SIB Procedure needs to update rrc_ue_conn_timers_and_constants
  RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_NOT_REQ
  : If SIB Procedure need not update rrc_ue_conn_timers_and_constants  
  
SIDE EFFECTS
  None
===========================================================================*/
rrc_conn_timer_constants_update_status_e_type 
rrcumi_check_ue_timer_constants_update_status
(
  boolean new_cell_ind
);

/*===========================================================================

FUNCTION   rrcumi_is_srns_in_progress

DESCRIPTION
  This function returns if SRNS is in progress with UMI procedure
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
boolean rrcumi_is_srns_in_progress
(
void
);
/*===========================================================================

FUNCTION rrcumi_return_substate

DESCRIPTION
Returns current umi substate

DEPENDENCIES
  None.

RETURN VALUE
  rrcumi_substate_e_type.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrcumi_substate_e_type rrcumi_return_substate(void);

#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================

FUNCTION rrcqsh_log_umi_ota_params

DESCRIPTION
Returns current umi substate

DEPENDENCIES
  None.

RETURN VALUE
  rrcumi_substate_e_type.

===========================================================================*/
void rrcqsh_log_umi_ota_params(void);
#endif
#endif /* RRCUMI_H */
