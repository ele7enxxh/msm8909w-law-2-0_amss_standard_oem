#ifndef WL1_X2W_SRHC_DRV_H
#define WL1_X2W_SRHC_DRV_H

/*===========================================================================
   FILE: wl1x2wsrchdrv.h

GENERAL DESCRIPTION
   This file contains all of the definitions necessary to interface between
   Searcher module and L1 mDSP Sync interface module or mDSP.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2002-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/15   mm      API for initializing critical section for G2W driver mutex
06/24/14   cc      Moving G2W drivers to WL1 folders
17/12/13   cja     CR590767 Bolt G2W - add gap start time to g2w startup
02/12/14   js      CR595142 :  T+G compilation errors removed
01/08/13   cja     CR511042 Separate RF ID for active and CM.
31/07/13   cja     CR498800 Use TRM to get IRAT RF ID
11/07/13   cja     CR511042 Separate RF ID for active and CM.
20/06/13   cs      Major Triton TSTS Syncup
18/06/13   cja     CR498800 Use TRM to get IRAT RF ID
19/03/13   pg      CR463973 Ensure WFW START wait signal is set on receiving WFW response  
20/02/13   cja     CR454583 Correct g2w state for rex set on WFW start
31/10/12   pg      CR409233: updates for G2W on Dime modem 
16/10/12   mm      Added function to validate G2W state before issuing a search.
02/04/12   mm      CR 348067 Wait for GL1 abort after FW AGC tuning completes
03/15/12   mm      CR 344531 G2W drivers new design 
28/11/11   cja     CR321479 Do not sleep if G2W WFW active
10/11/11   cja     CR314495 Fix Klocwork errors
01/11/11   cja     CR306187 Remove unused functions
11/01/11   cgc     Fix compiler warnings removed PACKED from int array
21/09/10   cja     Fix Lint and KlocWork errors
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
23/23/10   ws      Integrate G2W driver from MDM8200
30/03/09   ws      Added FEATURE_GSM_GPRS_QDSP6 and FEATURE_GSM_GPRS_POSEIDON
                   to replace FEATURE_GSM_GPRS_MUSTANG
16/03/09   ws      Added PACKED_POST for QDSP6 targets
19/02/09   scm     Increase number of dedicated results possible.
19/01/09   cs      Fix weird double spacing issues in previous change
02/01/09   nt      Change of MUSTANG featurisation
12/17/08   scm     Extern wcdma_srch_id_srch_parms.
12/15/08   scm     Extern boolean g2w_dedicated_mode_on.
12/12/08   scm     Implement idle and dedicated reconfirmation PN search.
12/10/08   scm     Move some defines from g2wsrchdrv.c to g2wsrchdrv.h.
12/10/08   scm     Extern data for g2w_store_stage1_result() support.
09/25/08   scm     More mods to support FEATURE_GSM_GPRS_MUSTANG feature.
09/22/08   scm     More G2W FEATURE_GSM_GPRS_MUSTANG mods.
09/16/08   scm     Firmware API mods inside FEATURE_GSM_GPRS_MUSTANG.
03/04/07   ws      Corrected Featurisation for GSM only builds
11/09/05   am      Added API for the gl1_wcdma_ecio_logarithmic_average
04/20/05   cpe     Updated STTD field for G2W searches
04/03/05   rc      Added function definition for gl1_srch_compute_rscp()
                   and thresholds for calculating CPICH RSCP.
02/04/05   gfr     Added prototypes for programming sample ram functions.
01/30/04   jc      Support for automatic Reconf list search after Stage2.
06/24/03   cpe     Added G2W dedicated mode prototype functions, structures
                   and externs.
02/25/03   cpe     Updated the sample RAM related variables.
01/31/03   cpe     Rolled back the sample RAM values to 0x3cca and 2492
01/31/03   cpe     Added two new sample ram related constants for step3
11/22/02   cpe     Changed a few parameters in structure
                   g2w_srch_step3_peak_struct_type and
                   gl1_wcdma_step3_peaks_type
07/23/02   cpe     Created file.
11/04/02   cpe     Added a new structure for returning EcIo, RSCP and RSSI
                   back to GL1 module.

===========================================================================*/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif


#if ( defined(FEATURE_INTERRAT_HANDOVER_GTOW) || defined(FEATURE_INTERRAT_CELL_RESELECTION_GTOW))
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "wl1x2wsrchmdspsync.h"
#include "wl1x2wsrchapi.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================
                        DATA DEFINITIONS
===========================================================================*/

/* Max number of tasks to be sent to stage2 */
#define WL1_X2W_SRCH_STAGE1_LIMIT_NUM  35

/*---------Parameters for Calulating CPICH RSCP------------------------------*/

/* Minimum CPICH RSCP, corresponding to -116dBm */
#define WL1_X2W_SRCH_MIN_CPICH_RSCP  232
  
/* Minimum Usable CPICH Ec/Io Value for CPICH RSCP calculation,
   Corresponding to -24.5 dB */
#define WL1_X2W_SRCH_MIN_CPICH_ECIO  49

/*---------------------START WFW timeout periods----------------------------*/

/* Definitions for START WFW timeout periods */
#define WL1_X2W_SRCH_START_WFW_WAIT_MSEC                50

#define WL1_X2W_SRCH_DEDIC_SRCH_START_WFW_WAIT_MSEC     10

#define WL1_X2W_SRCH_SRCH_MSG_LOG_MAX 20

/*============================================================================ 
                      ENUMERATIONS
=============================================================================*/

/*============================================================================
 						  STRUCTS
=============================================================================*/

typedef struct
{
  /* Flag for Dedicated mode search programmed and parameters, if TRUE */
  boolean g2w_ded_mode_srch_programmed;
  wl1_x2w_srch_g2w_ded_mode_srch_parms_type ded_mode_srch_parms;
  /* Flag for Ded mode Reconf search programmed and params, if TRUE */           
  boolean g2w_reconf_srch_programmed;
  wl1_x2w_srch_g2w_list_input_task_type ded_mode_reconf_srch_parms;
} wl1_x2w_srch_g2w_ded_mode_parms_type;

/* WSRCHLTEDRV msg log structure type */
typedef struct 
{
  msgr_umid_type id;
} wl1_x2w_srch_msg_struct_type;

typedef struct 
{
  uint8 idx;
  wl1_x2w_srch_msg_struct_type array[WL1_X2W_SRCH_SRCH_MSG_LOG_MAX]; 
} wl1_x2w_srch_msg_log_struct_type;

/*=============================================================================
                          EXTERN VARIABLES 
=============================================================================*/

/*=============================================================================
                           FUNCTION PROTOTYPES
=============================================================================*/

/*=========================================================================

FUNCTION      WL1_X2W_SRCH_INIT_CRIT_SECT

DESCRIPTION   This function initializes the G2W search driver critical section.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
==========================================================================*/
extern void wl1_x2w_srch_init_g2w_crit_sect(void);
/*=========================================================================

FUNCTION      WL1_X2W_SRCH_G2W_DRV_INIT

DESCRIPTION   This function initializes the G2W search driver structures and
              regsiters callback for FW-to-SW event done

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
==========================================================================*/
void wl1_x2w_srch_g2w_drv_init(gas_id_t gas_id);

/*===========================================================================

FUNCTION        WL1_X2W_SRCH_G2W_DRV_VALIDATE_STATE

DESCRIPTION     This function does a validity check if the G2W search is already aborted
                and G2W state is G2W_ABORT_STATE.

DEPENDENCIES    None.

RETURN VALUE    TRUE, if state is valid
                FALSE, if G2W state is G2W_ABORT_STATE

SIDE EFFECTS    None.

===========================================================================*/
boolean wl1_x2w_srch_g2w_drv_validate_state(gas_id_t gas_id);

/*===========================================================================

FUNCTION        WL1_X2W_SRHC_DRV_ABORT

DESCRIPTION     This function is the pre-cb function for an ABORT cmd. It sets
                the G2W state to ABORT and waits for the cmd done ISR to put
                FW to sleep.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
void wl1_x2w_srch_drv_abort(gas_id_t gas_id);

/*===========================================================================

FUNCTION        WL1_X2W_SRCH_G2W_ENG_SORT_STAGE1_PATHS

DESCRIPTION     This function sorts the paths list in the decreasing order of
                energies. It uses bubble sort algorithm to achieve its goal.
                This function operates on the wl1_x2w_srch_mdsp_sync_stage1_result[] data structure
                by default and will be kept so unless there is need to use
                this function on other path lists as well.

DEPENDENCIES    None

INPUT PARAMS    None

RETURN VALUE    None

SIDE EFFECTS    wl1_x2w_srch_mdsp_sync_stage1_result[] is sorted per energy of each path
===========================================================================*/
void wl1_x2w_srch_g2w_eng_sort_stage1_paths(gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_G2W_PROCESS_DED_MODE_STAGE1_RESULTS

DESCRIPTION
  This function processes the results of a stage1 search

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wl1_x2w_srch_g2w_process_ded_mode_stage1_results(gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_G2W_PROCESS_DED_MODE_STAGE2_RESULTS

DESCRIPTION
  This function processes the results of a stage2 search.
  Note that this function is executed in interrupt context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wl1_x2w_srch_g2w_process_ded_mode_stage2_results(gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_G2W_PROCESS_RECONF_SEARCH_RESULT

DESCRIPTION
  This function processes the results of a Reconf search.
  Note that this function is executed in interrupt context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void wl1_x2w_srch_g2w_process_reconf_search_result(gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_G2W_DRV_PROGRAM_PN_AFTER_NASTT_SEARCH

DESCRIPTION
  This function programs PN after NAS-TT search in G2W idle mode. It uses the
  peaks found in the NAS-TT search for the PN search. There is also a debug
  option in which the PN search is conducted on the whole frame instead of
  the PN window.

DEPENDENCIES
  NAS-TT search must have been completed.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void wl1_x2w_srch_g2w_drv_program_pn_after_nastt_search (gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_G2W_DRV_PROGRAM_STAGE1_SEARCH

DESCRIPTION
  This function sends stage1 search request to the mDSP after setting the
  stage1 search parameter values of g2w_ded_mode_stage1_srch_q_parms structure
  for the cells to be searched.

DEPENDENCIES

RETURN VALUE None

SIDE EFFECTS
  Stage 1 search in dedicated mode is started.

===========================================================================*/
void wl1_x2w_srch_g2w_drv_program_stage1_search (gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_G2W_DRV_PROGRAM_STAGE2_SEARCH

DESCRIPTION
  This function sends stage2 search request to the mDSP after setting the
  stage2 search parameter values of g2w_ded_mode_stage2_srch_q_parms structure
  for the cells to be searched.

DEPENDENCIES

RETURN VALUE
   TRUE, if the command is sent to mDSP
  FALSE, otherwise

SIDE EFFECTS
  Stage 2 search in dedicated mode is started.

===========================================================================*/
void wl1_x2w_srch_g2w_drv_program_stage2_search (gas_id_t gas_id);

/*===========================================================================
FUNCTION        WL1_X2W_SRCH_G2W_DRV_DED_MODE_PROGRAM_RECONFIRM_SRCH

DESCRIPTION     This function programs a reconfirmation list search.

DEPENDENCIES    None

RETURN VALUE    void

SIDE EFFECTS
===========================================================================*/
void wl1_x2w_srch_g2w_drv_program_reconfirm_srch(gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_G2W_DRV_RFM_MEAS_COMMON_SCRIPT_EXIT

DESCRIPTION
  This function performs the G2W RF measurement exit.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Needs to be paired with rfm_meas_common_script_enter()

===========================================================================*/
void wl1_x2w_srch_g2w_drv_rfm_meas_common_script_exit( void );

/*===========================================================================

FUNCTION WL1_X2W_SRCH_G2W_DRV_DO_IDLE_SEARCH

DESCRIPTION
  This function is the state machine function for Idle W searches.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if the command is sent to FW
  FALSE, otherwise

SIDE EFFECTS
  Search commands will be sent to FW

===========================================================================*/
void wl1_x2w_srch_g2w_drv_do_idle_search(
  wl1_x2w_srch_drv_search_state_enum_type g2w_srch_state,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_G2W_DRV_DO_DEDICATED_SEARCH

DESCRIPTION
  This function is the state machine function for Dedicated mode W searches.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if the command is sent to FW
  FALSE, otherwise

SIDE EFFECTS
  Search commands will be sent to FW

===========================================================================*/
void wl1_x2w_srch_g2w_drv_do_dedicated_search(
  wl1_x2w_srch_drv_search_state_enum_type g2w_srch_state,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION    WL1_X2W_SRCH_G2W_DRV_CONTINUE_SRCH

DESCRIPTION
  This function is the main state machine function for G2W. It is installed as
  a callback handler for all FW-to-SW ISR events. Based on the event triggered,
  the states would be moved in the state and the next cmd to WFW would be issued.
  This would not be called as cb from cmd done isr only when ABORT is the next cmd
  from the cmd queue that's being sent to FW.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Search commands will be sent to FW

===========================================================================*/
void wl1_x2w_srch_g2w_drv_continue_srch(
  wl1_x2w_srch_mdsp_event_type g2w_mdsp_sync_event,
  gas_id_t gas_id);

#endif /* FEATURE_INTERRAT_HANDOVER_GTOW || definedFEATURE_INTERRAT_CELL_RESELECTION_GTOW */
#endif /* G2W_SRCH_DRV_H */
