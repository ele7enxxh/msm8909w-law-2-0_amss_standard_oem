#ifndef RRCMCM_H
#define RRCMCM_H
/*===========================================================================
         R R C   M O D E   C O N T R O L   M O D U L E 

DESCRIPTION

  This module is the header module for the mode control module.
  This contains the external interface functions to be used by
  by other RRC modules. This module is only used for dual-mode.
  
Copyright (c) 2001-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcmcm.h_v   1.3   12 Jul 2002 11:55:38   xgao  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcmcm.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/16   vs      Changes to cleanup unnecessary f3s
05/18/15   sas     Made changes to indicate RLC about the LTA/QTA start/stop status.
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
07/08/11   gv      Added code to reset he last mcm activation cause whenever 
                   a fresh acquisition happens
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/21/11   rl      Added code to support L2W CGI
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/23/10   rm      Removed usage of FEATURE_BST
06/14/10   rm      Added code to support BST search on WCDMA
05/10/10   sks     Made changes to not change the establishment_cause in case of registration
                   attempt after GTOW redirection success.
06/15/09   sks     Made changes to support messaging interface with RR for writing 
                   ACQ-DB to NV during power off.
05/08/09   ss      Updated Copyright Information
11/20/08   ps      Added rrcmcm_get_substate() to get the MCM substate
10/24/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
06/09/08   pk      Added support for FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE.
05/02/06   segu    Added support for single to dual mode changes
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
12/12/05   kc/vr   Added a buffer to store change_mode_req in cmd store
11/08/05   vm      Added prototype for function rrcmcm_send_irat_abort_ind().
10/04/05   vr      Function definition to update RRC Mode from IRAT_IN_PROGRESS
                   to Active
07/20/05   vr      Added RRCMCM_INTERRAT_CC_ORDER_WTOG_FAILED cause to 
                   rrcmcm_act_cause type to support Spec Compliant WTOG CCO
06/02/05   vr      Support for Interrat reselection aborts in idle mode
12/17/04   vr      Added support for WCDMA->1x handover feature
11/22/04   vr      Added support for BPLMN search
03/09/04   bu      Added prototype for rrcmcm_set_establishment_cause().
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
08/15/03   kc      Added interface function to update dual mode status flag
05/23/03   kc      Added ENUMs to handle CHANGE_MODE to FTM/ONLINE at runtime.
04/08/03   kc      Changed is_process_required parameter to cmd_ptr from cmd_id
02/14/03   kc      Added Activation Cause type and function declaration to
                   update the activation cause.
01/28/03   sk      Moved the constant definitions and typedefs from rrcmcm.c 
                   to rrcmcm.h (lint error fix)
10/25/02   xfg     Made changes from the code review
07/12/02   xfg     Changed the feature define tag to cover the entire module
05/31/02   xfg     Removed sys_plmn_selection.h from includes
05/10/02   xfg     Change FEATURE_CHEETAH_BASELINE to FEATURE DUALMODE BASELINE
04/22/02   xfg     Created file.


===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "sys.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* define the max numbers of mode change notifications */
#define RRCMCM_MAX_NOTIFICATIONS  MAX_NUM_OF_PROCEDURES

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Enumerated type of the different substates for this procedure           */
typedef enum
{
  RRCMCM_INITIAL,                   /* Initial Substate                    */
  RRCMCM_WAIT_START_CNF,            /* Waiting for START confirmation from
                                       L1                                  */
  RRCMCM_WAIT_STOP_CNF,             /* Waiting for STOP confirmation from
                                       LLC                                 */
  RRCMCM_WAIT_SUSPEND_CNF,          /* Waiting for SUSPEND confirmation
                                       from L1                             */
  RRCMCM_WAIT_RESUME_CNF,           /* Waiting for RESUME confirmation from
                                       L1                                  */
  RRCMCM_WAIT_STOP_CNF_FOR_MODE_CHANGE,
  RRCMCM_MAX_STATES
} rrcmcm_state_e_type;
/* Below #defines are for logging purpose only */
#define rrcmcm_state_e_type_value0 RRCMCM_INITIAL,				   
#define rrcmcm_state_e_type_value1 RRCMCM_WAIT_START_CNF
#define rrcmcm_state_e_type_value2 RRCMCM_WAIT_STOP_CNF,			   
#define rrcmcm_state_e_type_value3 RRCMCM_WAIT_SUSPEND_CNF,		   
#define rrcmcm_state_e_type_value4 RRCMCM_WAIT_RESUME_CNF,		   
#define rrcmcm_state_e_type_value5 RRCMCM_WAIT_STOP_CNF_FOR_MODE_CHANGE,
#define rrcmcm_state_e_type_value6 RRCMCM_MAX_STATES

/* RRC Mode Change Table type                                              */
typedef struct
{
  rrc_mode_e_type from_mode;
  rrc_mode_e_type to_mode;
  rrc_proc_e_type procedure;
} rrcmcm_mode_change_table_type;

/* Command Received Indication Type */
typedef enum
{
  RRCMCM_CMD_NONE,
  RRCMCM_CMD_SERVICE_REQ,
  RRCMCM_CMD_STOP_WCDMA_REQ,
  RRCMCM_CMD_PLMN_LIST_REQ,
  RRCMCM_CMD_MODE_CHANGE,
  RRCMCM_CMD_INTERRAT_PLMN_SRCH_REQ,
  RRCMCM_CMD_MODE_CHANGE_REQ,
  RRCMCM_CMD_DEACT_REQ,
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif
#ifdef FEATURE_LTE_TO_WCDMA
  RRCMCM_CMD_LTE_IRAT_REQ,
  RRCMCM_CMD_LTE_PLMN_SRCH_REQ,
#endif
  RRCMCM_CMD_MAX
} rrcmcm_cmd_flag_e_type;

/* Cause for Mode change to be requested */
typedef enum
{
  RRCMCM_ACT_CAUSE_NONE,
  RRCMCM_INTERRAT_CELL_RESELECTION,
  RRCMCM_INTERRAT_CELL_REDIRECTION,
  RRCMCM_INTERRAT_HANDOVER_GTOW,
  RRCMCM_INTERRAT_CC_ORDER_GTOW,
  RRCMCM_INTERRAT_CC_ORDER_WTOG_FAILED
} rrcmcm_activation_cause;  

/* New types */
/* Enum to indicate a buffered ABORT message from RR */
typedef enum
{
  RRCMCM_RR_ABORT_NONE, 
  RRCMCM_RR_INTERRAT_PLMN_ABORT_REQ,
  RRCMCM_RR_INTERRAT_RESELECTION_ABORT_REQ,
  RRCMCM_RR_INTERRAT_REDIRECT_ABORT_REQ,
#ifdef FEATURE_LTE_TO_WCDMA
  RRCMCM_LTE_ABORT_PLMN_SRCH_REQ,
  RRCMCM_LTE_ABORT_RESEL_REQ,
  RRCMCM_LTE_ABORT_REDIR_REQ,
  RRCMCM_LTE_ABORT_CGI_REQ,
#endif
  RRCMCM_RR_ABORT_INVLD   /* exception cases,not used currently */
} rrcmcm_abort_flag_e_type;

/* Struct to save mode/proc of a change_mode 
   when MCM is already processing a change_mode_req  */
typedef struct
{
  rrc_mode_e_type mode;
  rrc_proc_e_type proc;
}rrcmcm_change_mode_req_type;

typedef struct
{
  rrc_cmd_type        rrcmcm_mm_req_cmd;                      /* MM commands */ 
  rrc_rr_cmd_type     rrcmcm_interrat_reselection_req_cmd;    /* Inter-RAT cell reselection command */
  rrcmcm_cmd_flag_e_type  rrcmcm_cmd_flag;                    /* Internal command flag - for deferred 
                                                                 command processing */
  rrcmcm_abort_flag_e_type  rrcmcm_abort_flag;                /* Flag to indicate a pending 
                                                                 BPLMN_SEARCH_ABORT_REQ */ 
  rrcmcm_change_mode_req_type rrcmcm_change_mode_req;         /* change_mode_req mode in case of race-conditions */
  rrc_rr_stop_wcdma_cause_e_type rrc_rr_stop_wcdma_cause;
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
  #error code not present
#endif
#ifdef FEATURE_LTE_TO_WCDMA
  rrc_cmd_type rrcmcm_lte_irat_cmd;
#endif
} rrcmcm_cmd_store_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================

FUNCTION  RRCMCM_INIT_PROCEDURE

DESCRIPTION

  This function initializes the Mode Contro module.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_init_procedure( void );



/*===========================================================================

FUNCTION RRCMCM_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Mode Control module. This function is called by the
  dispatcher whenever an event is received for this module.  
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrcmcm_event_handler(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
);


/*===========================================================================

FUNCTION   RRCMCM_GET_SUBSTATE

DESCRIPTION

  This function retrieves the RRC MCM SUBSTATE.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  Returns rrcmcm_state in rrcmcm_state_e_type

SIDE EFFECTS

  None.

===========================================================================*/
rrcmcm_state_e_type rrcmcm_get_substate( void );
/*===========================================================================

FUNCTION  RRCMCM_GET_RRC_MODE

DESCRIPTION

  This function returns the current RRC mode.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  Returns RRC mode in rrc_mode_e_type.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
rrc_mode_e_type rrcmcm_get_rrc_mode(void);
/*===========================================================================

FUNCTION  RRCMCM_IS_DUALMODE_ENABLED_no_f3

DESCRIPTION

  This function returns True if inter-RAT operation is enabled, otherwise,
  it returns False.

DEPENDENCIES

  None.

RETURN VALUE

  Return rrc_dual_mode_enabled status in boolean.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcmcm_is_dualmode_enabled_no_f3(void);

/*===========================================================================

FUNCTION  RRCMCM_IS_DUALMODE_ENABLED

DESCRIPTION

  This function returns True if inter-RAT operation is enabled, otherwise,
  it returns False.

DEPENDENCIES

  None.

RETURN VALUE

  Return rrc_dual_mode_enabled status in boolean.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
boolean rrcmcm_is_dualmode_enabled(void);

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif

/*===========================================================================

FUNCTION  RRCMCM_IS_PROCESS_REQUIRED

DESCRIPTION

  This function checks if the further process for an incoming command is.
  required based the current RRC mode.n If it is required, True is returned, 
  otherwise, False is returned.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcmcm_is_process_required(
rrc_cmd_type *cmd_ptr
);

/*===========================================================================

FUNCTION  RRCMCM_REGISTER_FOR_MODE_CHANGE

DESCRIPTION

  This function registers a notification when a RRC mode change occurs for a
  given condition. The from_mode is the mode from which the transition occurs 
  abd the to_mode is the mode to which the tansition occurs.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_register_for_mode_change(
  rrc_proc_e_type procedure,
  rrc_mode_e_type from_mode,
  rrc_mode_e_type to_mode
);

/*===========================================================================

FUNCTION  RRCMCM_UPDATE_WCDMA_ACTIVATION_CAUSE

DESCRIPTION

  This function updates the cause of wcdma activation.  Activation could be
  due to either cell reselection or due to GtoW Handover
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_wcdma_activation_cause( rrcmcm_activation_cause act_cause );
/*===========================================================================

FUNCTION  rrcmcm_update_dual_mode_enabled_status

DESCRIPTION

  This function updates the status of the flag whether if dual mode is
  enabled or not.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_dual_mode_enabled_status( boolean status );

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif
/*===========================================================================

FUNCTION  RRCMCM_SET_ESTABLISHMENT_CAUSE

DESCRIPTION

  This function sets the global RRC establishment cause appropriately 
  based on the last INTER-RAT GTOW procedure. No change otherwise.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_set_establishment_cause(void);

/*===========================================================================

FUNCTION   RRCMCM_UPDATE_RRC_MODE_TO_ACTIVE

DESCRIPTION

  This function set the current RRC mode to active.Currently called 
  by CHO procedure after the GTOW PCCO is successful.
  
  Only IRAT_IN_PROGRESS to ACTIVE is allowed
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_rrc_mode_to_active(void);

/*===========================================================================

FUNCTION   RRCMCM_SEND_IRAT_ABORT_IND

DESCRIPTION

  This function sends a RRC_IRAT_ABORT_IND to the corresponsing
  Interrat procedure to abort the ongoing Interrat operation 
  inorder to service a higher priority request from MM
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_send_irat_abort_ind(void);

/*===========================================================================
FUNCTION rrcmcm_get_rrc_est_cause

DESCRIPTION
  Returns establishment cause

DEPENDENCIES
  None

RETURN VALUE
  Returns establishment cause

SIDE EFFECTS
  None
===========================================================================*/
rrc_establish_cause_e_type rrcmcm_get_rrc_est_cause(void);


/*===========================================================================
FUNCTION rrcmcm_send_dual_mode_info_to_meas_ueci

DESCRIPTION
  This function will send dual mode info to measuremnt and UE Capability information module
   in the mode status indication primitive

DEPENDENCIES
  None

RETURN VALUE
  Returns None

SIDE EFFECTS
  None
===========================================================================*/
void rrcmcm_send_dual_mode_info_to_meas_ueci(rrc_cmd_type *cmd_ptr);


#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
#error code not present
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST
/*===========================================================================

FUNCTION   RRCMCM_UPDATE_MODE_CAPABILITY

DESCRIPTION

  This function updates GSM, LTE mode capability from NAS message
DEPENDENCIES

  None.

RETURN VALUE

  Returns a boolean value.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_mode_capability
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr
);
/*===========================================================================

FUNCTION  RRCMCM_IS_MODE_ENABLED_IN_RAT_PRI_LIST

DESCRIPTION

  This function returns True if mode passed as argument is enabled in RAT PRI LIST, otherwise FALSE

DEPENDENCIES

  None.

RETURN VALUE

  Return TRUE is LTE mode is enabled in RAT PRI LIST

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcmcm_is_mode_enabled_in_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr,
  sys_sys_mode_e_type mode
);
/*===========================================================================

FUNCTION   RRCMCM_UPDATE_MODE_CAPABILITY_FROM_SERVICE_REQ

DESCRIPTION

  This function updates GSM, LTE mode capability from RRC_SERVICE_REQ.
DEPENDENCIES

  None.

RETURN VALUE

  Returns a boolean value.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_mode_capability_from_service_req
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr
);
#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
/*===========================================================================

FUNCTION  RRCMCM_IS_LTE_MODE_ENABLED

DESCRIPTION

  This function returns True if LTE mode is enabled, otherwise FALSE

DEPENDENCIES

  None.

RETURN VALUE

  Return rrc_lte_mode_enabled status in boolean.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
boolean rrcmcm_is_lte_mode_enabled
(
  void
);

/*===========================================================================

FUNCTION  rrcmcm_update_lte_mode_enabled_status

DESCRIPTION

  This function updates the status of the flag whether if LTE mode is
  enabled or not.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_update_lte_mode_enabled_status( boolean status );
#endif
#endif

/*===========================================================================

FUNCTION   RRCMCM_SEND_PLMN_LIST_CNF_WITH_FAILURE

DESCRIPTION

  This function sends a RRC_PLMN_LIST_CNF to MM to with the
  proper failure cause.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcmcm_send_plmn_list_cnf_with_failure
( 
  sys_network_selection_mode_e_type network_select_mode , 
  uint8 trans_id
  , sys_service_search_e_type svc_srch_bgnd
);

/*===========================================================================

FUNCTION   RRCMCM_RESET_LAST_ACT_CAUSE

DESCRIPTION

  This function resets the mcm activation cause.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcmcm_reset_last_act_cause( void );

/*===========================================================================

FUNCTION  RRC_SEND_RLC_TA_STATUS

DESCRIPTION

  This function sends signal 'RLC_UL_TA_IND' to RLC to indicate the START/STOP status of LTA/QTA 
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrc_send_rlc_ta_status(rlc_ta_ind_e_type tune_away_ind, rlc_ta_success_e_type tune_away_succ);

#endif /* RRCMCM_H */



