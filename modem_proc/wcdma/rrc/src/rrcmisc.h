#ifndef RRCMISC_H
#define RRCMISC_H
/*===========================================================================
                  R R C  M I S C  P R O C E D U R E 

DESCRIPTION
  This header file contains function prototypes for the RRC Misc Procedure.
  The RRC Misc Procedure is created for implementation purpose to handle
  processing of unknown or unsupported message type.It also process the Paging
  Control Request command from MM and updates suitable RRC Data.

Copyright (c) 2001, 2003, 2005-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcmisc.h_v   1.1   14 Sep 2001 21:19:02   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcmisc.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/14   sp      Made changes for Mav debug snapshot
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
01/03/13   ad      Made changes to reduce the redundant F3s
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/24/12   sks     Added support for QCHAT specific dynamic DRX.
04/25/12   as      Made changes to trigger cell update/drx req when UE recived OTA with 
                   redirection info and cell reselection in progress
04/10/12   gv      Made changes to remove unused heap allocation and related unused code
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
05/13/10   ss      Fixed compiler warnings.
05/07/10   prk     Fixed compiler warnings.
03/29/10   dm      Made changes for allocating tmc SIB heap.
03/15/10   gkg     Made changes to put changes for "Multiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
02/01/10   dm      Added code for handling Multiple DRX.
05/11/09   ps      Feature to dynamically allocate cc/oc/toc/sib_event ptr
05/08/09   ss      Updated Copyright Information
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
09/05/06   vm      Added tx to disconnected reason in the function call
                   rrc_transition_to_disconnected_state.
11/09/05   kc      Prototype for taking UE to disconnected state
11/04/05  da/vr    Made changes to detect UE inactivity and take appropriate actions.
08/17/05   vm      Added support for not barring a cell if SIM has not been
                   read (if SIM is present).
                   Added prototype for new function rrcmisc_rrc_rcvd_sim_insert_req().
09/13/01   rj      Updated heading to reflect paging control request processing.
05/16/01   rj      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "rrccmd_v.h"
#ifdef FEATURE_INACTIVITY_HANDLING
#include "rrcdata_v.h"
#include "uecomdef.h"
#endif /* FEATURE_INTACTIVITY_HANDLING */
#include "rrcrbcommon.h"
#include "rrcueci.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


extern boolean rrc_high_mobility_ind_in_rcr;


#ifdef FEATURE_WCDMA_REL7_SPECRS 

#define RRCTMR_COUEC_TIMER_IN_MS 300000UL

typedef enum
{
  CHANGE_OF_CAP_REFUSED,
  CHANGE_OF_CAP_ACCEPTED,
  CHANGE_OF_CAP_WITH_RECONFIG
} rrccouec_cnf_e_type;

/*Following enum declares whether RNC supports Change in
    UE capability or not.*/
typedef enum
{
  /*We need to have this enum instead of boolean because we need to differentiate
      between the case when "cap change support' is not given and when "cap chnage support"
      is given as FALSE. */
  RNC_CAP_CHANGE_SUPPORTED,
  RNC_CAP_CHANGE_UNSUPPORTED,
  RNC_CAP_CHANGE_NONE
} rrc_rnc_cap_change_support_e_type;

typedef enum 
{
  RRCCOUEC_INITIAL,               /* COUEC initial substate      */

  RRCCOUEC_WAIT_FOR_UECI_CNF, /*COUEC waits for UECI CNF*/

  RRCCOUEC_WAIT_FOR_COUEC_TIMER_TO_EXPIRE 
                                                 /* COUEC waits for Timer COUEC to expire */
} rrccouec_substate_e_type;
/* Below #defines are added for logging purpose only */
#define rrccouec_substate_e_type_value0 RRCCOUEC_INITIAL
#define rrccouec_substate_e_type_value1 RRCCOUEC_WAIT_FOR_UECI_CNF
#define rrccouec_substate_e_type_value2 RRCCOUEC_WAIT_FOR_COUEC_TIMER_TO_EXPIRE 

typedef struct
{
  rrccouec_substate_e_type substate;
  uint8 num_of_retries;
  rrc_proc_e_type proc_id;
  rrc_ue_cap_update_during_rrc_conn_type ue_cap_change_requested;
} rrccouec_int_data_type;


extern rrccouec_int_data_type rrccouec;
extern boolean rrc_rnc_cap_change_support; 
extern rrc_rnc_cap_change_support_e_type rrc_ordered_rnc_cap_change_support;
extern rrc_ue_cap_update_during_rrc_conn_type rrc_ue_cap_during_rrc_conn;
#endif /*FEATURE_WCDMA_REL7_SPECRS*/


typedef enum 
{
  RRCMISC_MULTIPLE_DRX_INITIAL,               /* MISC Multiple DRX initial substate      */
  RRCMISC_WAIT_FOR_BPLMN_SUSPEND_CNF, /*MISC waits for BPLMN SUSPEND CNF*/
} rrcmisc_substate_e_type;

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION rrcmisc_rrc_rcvd_sim_insert_req  

DESCRIPTION
  This function is currently called by CSP before a cell bar request is 
  sent to L1. If the SIM has not been read, then cell bar request is not
  sent to L1. The reason being that it is possible that the cell UE attempts
  to camp on is reserved for Operator use. If UE has high access class(es) 
  present then the cell should not be barred else the cell should be barred
  for 1280s (~21mins). Till SIM is read it is not known whether High ACs are 
  present or not and hence such a cell is not barred and in addition UE
  does not camp on this cell (during cell selection processs only and not
  due to cell access restrictions change via SIB modification). 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: SIM has been read
  FALSE: SIM has not been read

===========================================================================*/
boolean rrcmisc_rrc_rcvd_sim_insert_req(void);

/*===========================================================================

FUNCTION RRC_TRANSITION_TO_DISCONNECTED_STATE

DESCRIPTION

  This function is used to send initiate cell selection command to CSP
  with destination state as disconnected. CSP on receiving this command
  releases all channels and takes UE to idle.
  The reason why UE is being taken to Idle is also sent to CSP. E.g., is
  tx to disconnected due to an emergency call when UE is OOS in Connected Mode
  or whether it is due to RL failure with T314/T315 = 0, etc.
  
DEPENDENCIES

  None

RETURN VALUE
none

SIDE EFFECTS

  None

===========================================================================*/
void rrc_transition_to_disconnected_state
(    
  rrc_proc_e_type rrc_proc,
  rrc_tx_to_discon_reason_e_type tx_to_discon_reason
);
/*===========================================================================

FUNCTION rrcmisc_init_procedure

DESCRIPTION
  This function initializes everything that the RRC Misc procedure
  needs to have initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcmisc_init_procedure(void);

/*===========================================================================

FUNCTION rrcmisc_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the RRC Misc procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC)

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcmisc_procedure_event_handler(rrc_cmd_type *cmd_ptr);


#ifdef FEATURE_INACTIVITY_HANDLING

/*===========================================================================

FUNCTION RRCMISC_HANDLE_PS_RAB_INACTIVITY

DESCRIPTION
  Get/Reset Inactivity flag in RLC
  
DEPENDENCIES
  None.

RETURN VALUE
  Success : If values were successfully set/reset
  Failue  : Future use
  
===========================================================================*/

uecomdef_status_e_type rrcmisc_handle_ps_rab_inactivity(uint8 instance_id);


/*===========================================================================

FUNCTION RRCMISC_CHECK_FOR_PS_RAB_INACTIVITY

DESCRIPTION
  Check for PS RAB Inactivity based on the status returned by RLC
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  If Inactivity is detected
  FALSE - If PS RABs are active

===========================================================================*/
boolean rrcmisc_check_for_ps_rab_inactivity();


#endif /* FEATURE_INTACTIVITY_HANDLING */

/*===========================================================================

FUNCTION rrcmisc_initiate_cell_update

DESCRIPTION
  This procedure initiates the Cell Update procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcmisc_initiate_cell_update(void);

/*===========================================================================

FUNCTION rrcmisc_process_T320_expiry

DESCRIPTION
  This procedure handles the Expiry of timer T320.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcmisc_process_T320_expiry(rrc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION rrc_cell_pch_ura_pch_handler

DESCRIPTION
  Used to handle state change to/from Cell PCH (or URA PCH)
  
DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/

void rrcmisc_state_change_handler
(
  rrc_cmd_type *cmd_ptr
);


#ifdef FEATURE_WCDMA_REL7_SPECRS 
/*===========================================================================

FUNCTION rrccouec_is_proc_active

DESCRIPTION
  This function tells whether COUEC procedure is active or not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
boolean rrccouec_is_proc_active
(
  void
);

/*===========================================================================

FUNCTION rrccouec_process_completion_of_ueci_req

DESCRIPTION
  This function is the procedure  handler of COUEC procedure. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrccouec_process_completion_of_ueci_req
(
  rrccouec_cnf_e_type ueci_cnf
);

/*===========================================================================

FUNCTION rrccouec_process_completion_cap_change_with_reconfig

DESCRIPTION
  This function is called from Reconfig procedures to notify that cap change is complete. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_process_completion_cap_change_with_reconfig
(
  void
);

/*===========================================================================

FUNCTION rrccouec_clear_procedure

DESCRIPTION
  This function clears the COUEC  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_clear_procedure
(
  void
);

/*===========================================================================

FUNCTION rrccouec_process_initial_substate

DESCRIPTION
  This function is the substate event handler of COUEC procedure. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_process_initial_substate
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
);

/*===========================================================================

FUNCTION rrccouec_process_wait_for_timer_couec_to_expire

DESCRIPTION
  This function is the substate event handler of COUEC procedure. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_process_wait_for_timer_couec_to_expire
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
);

/*===========================================================================

FUNCTION rrccouec_process_wait_for_ueci_cnf

DESCRIPTION
  This function is the substate event handler of COUEC procedure. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_process_wait_for_ueci_cnf
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
);

/*===========================================================================

FUNCTION rrccouec_init_procedure

DESCRIPTION
  This function initializes everything that the RRC COUEC procedure
  needs to have initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_init_procedure
(
  void
);

/*===========================================================================

FUNCTION rrccouec_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the RRC COUEC procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC)

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION rrccouec_save_ue_cap

DESCRIPTION
  This function saves the requested UE  capabilities. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_save_ue_cap
(
  void
);

/*===========================================================================

FUNCTION rrccouec_send_couec_cnf

DESCRIPTION
  This function sends  RRC_COUEC_CNF comamnd to the requested procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_send_couec_cnf
(
  rrc_couec_cnf_e_type couec_cnf
);

/*===========================================================================

FUNCTION rrc_save_rnc_cap_change_support

DESCRIPTION
 This function updates global "rrc_rnc_cap_change_support" based on the OTA IE value.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rrc_save_rnc_cap_change_support
(
  void
);
#endif/*FEATURE_WCDMA_REL7_SPECRS*/

/*====================================================================
FUNCTION: rrc_init_dynamic_ptrs()

DESCRIPTION:
  This function is called to allocate OC, CC and TOC pointers, sib heap

DEPENDENCIES:
  None.

RETURN VALUE:
  None
  
SIDE EFFECTS:

====================================================================*/
void rrc_init_dynamic_ptrs(
    void
    );
/*====================================================================
FUNCTION: rrcllc_free_config_ptrs()

DESCRIPTION:
  This function is called to free the allocated OC, CC and TOC pointers. sib

DEPENDENCIES:
  None.

RETURN VALUE:
  None
  
SIDE EFFECTS:

====================================================================*/
void rrc_free_dynamic_ptrs(
  void
  );

/*===========================================================================

FUNCTION          RRCCSP_CHECK_INITIATE_CELL_SELECTION_HANDLING

DESCRIPTION       This function is called to check if CSP is in proper substate to handle 
                         RRC_INITIATE_CELL_SELECTION_IND to transition to DCH or FACH.

DEPENDENCIES      None

RETURN VALUE      TRUE/FALSE

SIDE EFFECTS      None
===========================================================================*/
boolean rrccsp_check_initiate_cell_selection_handling
(
  rrcrb_directed_cell_info_type *directed_cell_info
);
#endif /* RRCMISC_H */

/*====================================================================
FUNCTION: rrc_get_qchat_specific_drx_cycle_lenght()

DESCRIPTION:
  This function is called to get the QCHAT specific DRX cycle length.

DEPENDENCIES:
  None.

RETURN VALUE:
  DRX cycle length
  
SIDE EFFECTS:

====================================================================*/
uint32 rrc_get_qchat_specific_drx_cycle_length( void );

/*====================================================================
FUNCTION: rrccouec_return_substate()

DESCRIPTION:
 returns substate

DEPENDENCIES:
  None.

RETURN VALUE:
  rrccouec_substate_e_type
  
SIDE EFFECTS:

====================================================================*/

//rrccouec_substate_e_type rrccouec_return_substate(void);
