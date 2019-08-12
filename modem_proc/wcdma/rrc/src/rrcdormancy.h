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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcdormancy.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/14   bc      Made changes to start T323 with value 3 seconds on receiving FD request when Network indicates T323 as 0.
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
02/15/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.91.50
01/31/11   ad      Fixed compilation warnings
08/21/10   ss      Fixed compilation warnings.
04/01/09   ss      Added functionality related to 3gpp defined Fast Dormancy.
12/30/09   kp      Created file.& added functionality related to 3gpp defined Fast Dormancy.
===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

#include "uecomdef.h"

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Provides the duration for RRC Fast Dormancy URA PCH Guard timer in milliseconds 
  * RRC will defer processing of dormancy request if this timer is runnning in URA/PCH States.
*/
#define RRCDORM_URA_PCH_GUARD_TIMER_VALUE     5000
#define RRCDORM_SRNS_GUARD_TIMER_VALUE        5000
#define RRCDORM_STOP_T323_TIMER_VALUE         0xFFFFFFFF


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum 
{
  RRCDORMANCY_INITIAL,                         /* RRCDORMANCY initial sub state */
  RRCDORMANCY_WAIT_FOR_CU_COMP_CNF,            /* RRCDORMANCY waits for confirmation from Cell 
                                                                                       Update Procedure complete */ 
  RRCDORMANCY_WAIT_FOR_L2ACK,                  /* RRCDORMANCY waits for l2 ACK for Initial Direct
                                                                                       Transfer message */
  RRCDORMANCY_WAIT_FOR_SIG_CON_REL_COMPL       /* RRCDORMANCY waits for Signalling Connection Release 
                                                                                    * Needed for fall back to legacy dormancy.*/                                                            
} rrcdormancy_substate_e_type;

/* Below #defines are added for logging purpose only */
#define rrcdormancy_substate_e_type_value0 RRCDORMANCY_INITIAL					 
#define rrcdormancy_substate_e_type_value1 RRCDORMANCY_WAIT_FOR_CU_COMP_CNF			 																					 
#define rrcdormancy_substate_e_type_value2 RRCDORMANCY_WAIT_FOR_L2ACK 				 																					 
#define rrcdormancy_substate_e_type_value3 RRCDORMANCY_WAIT_FOR_SIG_CON_REL_COMPL 


typedef enum
{
  RRCDORMANCY_VALIDATION_SUCCESS,                  /* Successful Status */      
  RRCDORMANCY_VALIDATION_FAIL_DOMAIN_NOT_OPEN,     /* Failure Status: PS domain not open */
  RRCDORMANCY_VALIDATION_FAIL_CS_DOMAIN_OPEN,      /* Failure Status: CS domain is also open */
  RRCDORMANCY_VALIDATION_FAIL_RRC_CON_NOT_OPEN,    /* Failure Status: RRC connection not open */
  RRCDORMANCY_VALIDATION_FAIL_UE_OOS,              /* Failure Status: UE OOS */
  RRCDORMANCY_VALIDATION_FAIL_T323_TIMER_RUNNING,  /* Failure Status: T323 timer is running */
  RRCDORMANCY_VALIDATION_FAIL_UE_LEAVING_CONN_MODE_SCAN, /* Failure Status: UE is leaving Connected Mode Scan */
  RRCDORMANCY_VALIDATION_FAIL                      /* Failure status:others */
} rrcdormancy_validation_status_e_type;

typedef enum
{
  RRCDORMANCY_MUI_STATUS_INVALID,                 /* No outstanding MUI's */
  RRCDORMANCY_MUI_SIG_CON_REL_CNF,                /* No outstanding MUI's, but waiting for sig-con-rel-cnf */
  RRCDORMANCY_MUI_SCRI_CNF                        /* Outstanding MUI for SCRI-cnf */
}rrcdormancy_fast_dormancy_mui_status_type;

typedef enum
{
  RRCDORMANCY_SUCCESS,                            /* Successful Status */      
  RRCDORMANCY_FAILURE                             /* Minor Failure Status */
} rrcdormancy_status_e_type;

typedef enum
{
  RRCDORMANCY_T323_TIMER_INVALID,         
  RRCDORMANCY_T323_TIMER_IDLE,    
  RRCDORMANCY_T323_TIMER_RUNNING,    
  RRCDORMANCY_SRNS_GUARD_TIMER_RUNNING            
  //RRCDORMANCY_URA_PCH_GUARD_TIMER_RUNNING         
} rrcdormancy_timer_status_e_type;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/*Structure to save fast dormancy req*/
typedef struct
{
  /* This flag indicate the status of MUIs for Signalling
     Connection Request */
  rrcdormancy_fast_dormancy_mui_status_type mui_status;
 
  /* This variable to stores the corresponding MUIs of the
     Signalling Connection Release Request message that is transmitted */
  uint8               mui;

   /* This flag indicates whether fast_dormancy_req is stored or not */
  boolean             fast_dormancy_req_status;

  /* Count the number of pending dormancy requests */
  uint32              num_pending_dormancy_req;
   
}rrcdormancy_fast_dormancy_req_type;


typedef struct
{
  /* If T323 timer is supported by N/W then it shows the support of Fast Dormancy.
     * Provides value of T323 timer. Assuming default value = 0 */
  boolean                           T323_nw_support;
  rrc_T_323                         T323_duration;    
  rrcdormancy_timer_status_e_type   T323_current_status;
}rrcdormancy_conn_timers_and_constants_type;

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */
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
void rrc_dormancy_init( void );

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
);

/*===========================================================================
  
  FUNCTION rrcdormancy_reset_n_send_fast_dormancy_cnf  
  
  DESCRIPTION
    This function sends RRC_FAST_DORMANCY_CNF command to NAS
  
  DEPENDENCIES
    none.
  
  RETURN VALUE
    none 
===========================================================================*/
void rrcdormancy_reset_n_send_fast_dormancy_cnf
(
  rrc_fast_dormancy_cnf_status_e_type cnf_cause
);

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
);

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
boolean rrcdormancy_check_dormancy_initiated_scrr_active
(
  void
);

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
);

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_check_n_restart_timer_T323  

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
);

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_check_if_nw_support_fast_dormancy  

  DESCRIPTION
    This function check if N/W supports dormancy or not

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
boolean rrcdormancy_check_if_nw_support_fast_dormancy
(
  void
);

/*===========================================================================
  
  FUNCTION 
    rrcdormancy_stop_n_update_t323_timer_status  

  DESCRIPTION
    This function stops gurard timers for SRNS or CELL_PCH/URA_PCH states and update status of
    T323 to IDLE/RUNNING

  DEPENDENCIES
    none.

  RETURN VALUE
    none 
===========================================================================*/
rrcdormancy_timer_status_e_type rrcdormancy_stop_n_update_t323_timer_status
(
  void
);


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
);

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
);

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
);


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
boolean rrcdormancy_is_dormancy_active
(
 void
);

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
);

