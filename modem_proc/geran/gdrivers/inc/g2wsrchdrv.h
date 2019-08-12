#ifndef G2W_SRCH_DRV_H
#define G2W_SRCH_DRV_H

/*===========================================================================
   FILE: g2wsrchdrv.h

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


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/g2wsrchdrv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/2016 ng      CR984320 Doing RF_meas_exit() during G2W abort when script enter is done but scripts are not built
20/01/14   wz/zf   CR772571: Build G2W RF scripts one frame earlier
10/07/14   cc      G2W direct stage 2 search
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

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif


#if ( defined(FEATURE_INTERRAT_HANDOVER_GTOW) || defined(FEATURE_INTERRAT_CELL_RESELECTION_GTOW))
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "g2wsrchwfwintf.h"


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef FEATURE_G2W_UNIT_TEST
#include "g2wsrchparm.h"
#endif

/*===========================================================================
                        DATA DEFINITIONS
===========================================================================*/
#define G2W_SRCH_MAX_DED_MODE_CELLS 32

/* Max number of tasks to be sent to stage2 */
#define G2W_STAGE1_LIMIT_NUM  35

/*---------Parameters for Calulating CPICH RSCP------------------------------*/

/* Minimum CPICH RSCP, corresponding to -116dBm */
#define G2W_SRCH_MIN_CPICH_RSCP  232
  
/* Minimum Usable CPICH Ec/Io Value for CPICH RSCP calculation,
   Corresponding to -24.5 dB */
#define G2W_SRCH_MIN_CPICH_ECIO  49

/*---------------------START WFW timeout periods----------------------------*/

/* Definitions for START WFW timeout periods */
#define G2W_SRCH_START_WFW_WAIT_MSEC           50

#define G2W_DEDIC_SRCH_START_WFW_WAIT_MSEC     10



/*============================================================================ 
                      ENUMERATIONS
=============================================================================*/

/* G2W WFW status */
typedef enum
{
  G2W_WFW_DISABLED = 0,
  G2W_WFW_SUSPENDED,
  G2W_WFW_ENABLED
} g2w_wfw_status_enum_type;

/* G2W Search Substate machine */
typedef enum
{
  G2W_SRCH_NO_SEARCH,
  G2W_SRCH_STEP1_SEARCH,
  G2W_SRCH_NASTT_SEARCH,
  G2W_SRCH_PN_SEARCH,
  G2W_SRCH_STAGE1_SEARCH,
  G2W_SRCH_SECOND_STAGE1_SEARCH,
  G2W_SRCH_STAGE1_STAGE2_SEARCH,
  G2W_SRCH_STAGE2_STAGE1_SEARCH,
  G2W_SRCH_STAGE2_SEARCH,
  G2W_SRCH_STAGE2_DIRECT_SEARCH,
  G2W_SRCH_RECONF_SEARCH,
  G2W_SRCH_NUM_SEARCHES
} g2w_srch_state_enum_type;

/* G2W Outer Main State machine */
typedef enum
{
  G2W_INACTIVE_STATE,
  G2W_START_WFW_STATE,
  G2W_MODE_RF_CFG_STATE,
  G2W_RF_STOPPED_STATE,
  G2W_SEARCH_STATE,
  G2W_SLEEP_WFW_STATE,
  G2W_ABORT_STATE,
  G2W_NUM_STATES
} g2w_state_enum_type;

/*============================================================================
 						  STRUCTS
=============================================================================*/

/* List search input parameter struct from GL1. The field cell struct type is
   defined in g2wmdspsync.h */

typedef struct 
{
  uint16 freq;
  uint16 num_tasks;
  g2w_mdsp_list_cell_struct_type list_srch_tasks[G2W_MDSP_LIST_SRCH_MAX_TASKS];
} g2w_srch_list_input_task_type;


/* Dedicated mode input parameters from GL1 */
/* Input used for stage1 search */

typedef struct 
{
  uint16 scr_code;
  /* If not available, use NO_PN_POS */
  uint16 pn_pos;
} g2w_srch_ded_mode_cell_type;

typedef struct 
{
  /* UARFCN */
  uint16 freq;
  /* Number of cells to be searched for every
     SAMPLE_RAM_DONE_ISR */
  uint16 num_cells_to_be_searched_per_sample_ram;
  /* num of cells included */
  uint16 num_cells;
  /* Dedicated mode cells to be searched */
  g2w_srch_ded_mode_cell_type ded_mode_cell[G2W_SRCH_MAX_DED_MODE_CELLS];
} gl1_wcdma_srchdrv_ded_mode_srch_parms_type;


/* G2W dedicated search output structure. It is used for sending results back to
   GL1 through the callback handler */
typedef struct 
{
  uint16 frequency;
  int16 rssi;
  uint16 scr_code;
  uint16 pn_pos;
  uint32 tot_eng;
  int16 cell_rscp;
  int16 cell_ecno;
}gl1_wcdma_ded_mode_list_srch_struct_type;


/* Struct containing the cell energy values  */
typedef struct 
{
  /* energy */
  uint32 cell_eng;
  /* EcIo */ 
  int16 ecio;    
  /* RSCP */
  int16 rscp;
  /* RxAGC */     
  int16 rx_agc;   
} g2w_srch_eng_parm_struct_type;

typedef struct
{
  /* Flag for Dedicated mode search programmed and parameters, if TRUE */
  boolean g2w_ded_mode_srch_programmed;
  gl1_wcdma_srchdrv_ded_mode_srch_parms_type ded_mode_srch_parms;
  /* Flag for Ded mode Reconf search programmed and params, if TRUE */           
  boolean g2w_reconf_srch_programmed;
  g2w_srch_list_input_task_type ded_mode_reconf_srch_parms;
} gl1_wcdma_ded_mode_parms;

/* Control Info struct for G2W State machine */
typedef struct
{
  /* G2W Search Mode -- Idle or Dedicated */
  g2w_mdsp_mode_enum_type mode;
  /* Flag to indicate if the RF is switched on */
  boolean g2w_srch_rf_on;
  /* Flag to indicate GL1 abort is delayed till AGC tuning 
  and SR done/RF ON ISR is complete */
  boolean abort_delayed;
  /* G2W state machine state */
  g2w_state_enum_type g2w_state;
  /* G2W search state in Idle or Dedicated */
  g2w_srch_state_enum_type g2w_srch_substate;
  /* indicate if a WFW START CMD has been issued and therefore a
     response is still pending */
  boolean wfw_start_sent;
  /* indicate if a G2W RF scripts has been built. */
  boolean g2w_rf_script_built;
  boolean g2w_rf_script_entered;
} g2w_srch_ctrl_info_struct_type;


/*===========================================================================
                         VARIABLES
===========================================================================*/

/*--------------------TYPEDEF: CALLBACK_FN--------------------------------*/

/* Callback functions to GL1 modules reporting the results of G2W Idle and
   Dedicated searches */

/* Callback functions for idle */
typedef void (*g2w_srch_list_srch_result_cb)(g2w_mdsp_list_srch_struct_type *,gas_id_t gas_id);

typedef void (*g2w_srch_pn_after_nastt_srch_result_cb)
                (g2w_mdsp_pn_after_nastt_results_type *pn_after_nastt_ptr,gas_id_t gas_id);

typedef void (*g2w_srch_srch_abort_cb)(gas_id_t gas_id);

/* Callback functions for Dedicated mode */
typedef void (*g2w_srch_ded_mode_srch_result_cb)
             (gl1_wcdma_ded_mode_list_srch_struct_type *list_ptr,gas_id_t gas_id);

/* Callback function type for returning WFW status */

typedef void (*g2w_wfw_status_cb_func_type) (g2w_wfw_status_enum_type,gas_id_t gas_id);

/*=============================================================================
                          EXTERN VARIABLES 
=============================================================================*/
/* G2W Dedicated mode result object */
extern gl1_wcdma_ded_mode_list_srch_struct_type g2w_ded_mode_result[];


/*=============================================================================
                           FUNCTION PROTOTYPES
=============================================================================*/


/*-------------------------Functions exported to GL1-------------------------*/

/*===========================================================================
FUNCTION     G2W_SRCH_WFW_SW_INIT

DESCRIPTION
  This function should be called one time whenever GSM is entered.  It
  prepares GSM L1 software and WCDMA firmware for doing G2W searches.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void g2w_srch_wfw_sw_init(gas_id_t gas_id);

/*===========================================================================

FUNCTION GL1_WCDMA_CFG_INIT

DESCRIPTION
  This function is exported to GL1 to initialize G2W configuration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void gl1_wcdma_cfg_init(
  /* Callback used for PN after NAS-TT search in idle */ 
  g2w_srch_pn_after_nastt_srch_result_cb  pn_after_nastt_srch_cb,
  /* Callback for Reconf List search */
  g2w_srch_list_srch_result_cb list_srch_cb,
  /* Callback for abort */
  g2w_srch_srch_abort_cb  srch_abort_cb,
  /* Callback for WFW status */
  g2w_wfw_status_cb_func_type wfw_status_cb,
  /* RF Device ID */
  rfm_device_enum_type  source_rf_device,
  rfm_device_enum_type  target_rf_device,
  gas_id_t gas_id
);


/*===========================================================================
FUNCTION GL1_WCDMA_DED_MODE_CFG_INIT

DESCRIPTION
  This function initializes parameters for G2W Dedicated mode search called by GL1

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void gl1_wcdma_ded_mode_cfg_init(
  /* Dedicated mode result cb handler */
  g2w_srch_ded_mode_srch_result_cb ded_mode_srch_cb,
  /* Reconf list search result handler */
  g2w_srch_list_srch_result_cb list_srch_cb,
  /* Callback for WFW status */
  g2w_wfw_status_cb_func_type wfw_status_cb,
  gas_id_t                    gas_id);

/*===========================================================================
FUNCTION GL1_WCDMA_DED_MODE_TERMINATE

DESCRIPTION
  This function cleans up G2W related stuff when exiting Dedicated mode.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void gl1_wcdma_ded_mode_cfg_terminate(gas_id_t gas_id);
#ifdef FEATURE_WCDMA
/*===========================================================================
FUNCTION     G2W_SRCH_CLEANUP_WCDMA_MEAS

DESCRIPTION  This function cleans up the G2W measurements because GL1 is going 
             INACT and shutting down drivers. It disables WFW app if WFW is not
             running, else it ERROR FATALs because GL1 did not send an abort
             if trying to cleanup during a G2W search. 

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  G2W searches will be terminated. ERR_FATALs if an abort was not 
              sent before if issued during a search. 
===========================================================================*/
extern void g2w_srch_cleanup_wcdma_meas(gas_id_t gas_id);
#endif
/*===========================================================================
FUNCTION     G2W_SRCH_INITIATE_SEARCH

DESCRIPTION  This function is called to start a G2W search.  Since it is called from
             ISR context, it switches most of the processing over to task context.

DEPENDENCIES
  None.

PARAMETERS
  WfwG2WTypeEnum  search_type
  uint16          uarfcn
  uint16          start_time

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void g2w_srch_initiate_search(
  g2w_mdsp_mode_enum_type search_type,
  uint16 uarfcn,
  uint16 start_time,
  gas_id_t gas_id);

/*===========================================================================
FUNCTION     G2W_SRCH_INITIATE_DEDICATED_SEARCH

DESCRIPTION  This function is called to start a dedicated G2W search.

DEPENDENCIES None.

PARAMETERS
  WfwG2WTypeEnum  search_type
  uint16          uarfcn
  uint16          start_time

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void g2w_srch_initiate_dedicated_search(
  g2w_mdsp_mode_enum_type search_type,
  uint16 uarfcn,
  uint16 start_time,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION GL1_WCDMADRV_PROGRAM_LIST_SRCH

DESCRIPTION
  This function programs reconfirmation list search from GL1
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void gl1_wcdmadrv_program_list_srch (
  /* Input parameters to program a Reconf list search during idle G2W */
  g2w_srch_list_input_task_type *srchlist_ptr,gas_id_t gas_id);

/*===========================================================================

FUNCTION GL1_WCDMADRV_PROGRAM_DED_MODE_SEARCH

DESCRIPTION
  This function programs parameters for G2W Dedicated mode search called by GL1

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean gl1_wcdmadrv_program_ded_mode_search(
  /* Input search parameters to program a dedicated search */
  gl1_wcdma_srchdrv_ded_mode_srch_parms_type *srch_cell_ptr,
  /* Input parameters to program a Reconf list search after Dedicated search */
  g2w_srch_list_input_task_type *list_srch_ptr,gas_id_t gas_id);

/*===========================================================================
FUNCTION     g2w_srch_build_rf_script

DESCRIPTION  GL1 calls this function to build RF scripts for G2W measurement. 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Start-up and cleanup RF scripts are generated.
===========================================================================*/
extern void g2w_srch_build_rf_script(
  /* RxLM buffer Index */
  uint32 g2w_rxlm_buf_idx,
  /* Home RxLM buffer Index */
  uint32 g_home_rxlm_buf_idx,
  /* RF startup script buffer index */
  uint8 startup_rf_buf_idx,
  /* RF cleanup script buffer index */
  uint8 cleanup_rf_buf_idx,
  /* W channel */
  uint16 W_chn,
  /* W Band type */
  rfcom_wcdma_band_type W_band,
  /* RF Device IDs */
  rfm_device_enum_type  source_rf_device,
  rfm_device_enum_type  target_rf_device,
  gas_id_t gas_id);

/*===========================================================================
FUNCTION     G2W_STARTUP

DESCRIPTION
  GL1 calls this function to start G2W. This function is the trigger point for
  G2W drivers to start programming the RF. GL1 passes the RxLm buffer index,
  RF startup and cleanup script index and offset to gap start. G2W drivers pass
  this information to W FW and issue the command WFW_IRAT_X2W_MODE_CFG_CMD_ID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Start-up and cleanup RF scripts are generated. Calls the function to issue cmd to WFW.
===========================================================================*/
extern void g2w_startup(
  /* G2W mode: Idle or dedicated */
  g2w_mdsp_mode_enum_type mode,
  /* RxLM buffer Index */
  uint32 g2w_rxlm_buf_idx,
  /* RF action time in USTMR/xo  */
  uint32 rf_action_time,
  /* Freq error in Q6 Hz format */
  int32 freq_err_q6_hz,
  /* Inverse WCDMA DL freq */
  uint32 inv_dlo_freq,
  gas_id_t gas_id);

/*===========================================================================

FUNCTION GL1_WCDMADRV_ABORT_SEARCH

DESCRIPTION
  This function aborts any ongoing searches and sends an ABORT cmd to WFW.
  It does not store/process on the results of the search and sends a sleep
  cmd to WFW
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Any ongoing searches will be aborted

===========================================================================*/
extern void gl1_wcdmadrv_abort_search (gas_id_t gas_id);

/*===========================================================================

FUNCTION        G2W_SRCH_DRV_VALIDATE_STATE

DESCRIPTION     This function does a validity check if the G2W search is already aborted
                and G2W state is G2W_ABORT_STATE.
                
DEPENDENCIES    None.

RETURN VALUE    TRUE, if state is valid 
                FALSE, if G2W state is G2W_ABORT_STATE

SIDE EFFECTS    None.

===========================================================================*/
extern boolean g2w_srch_drv_validate_state(gas_id_t gas_id);

/*===========================================================================

FUNCTION GL1_WCDMA_GET_ECIO_RSCP_RSSI

DESCRIPTION
  This function converts an energy into the 6-bit Ec/Io value required
  for pilot strength measurements. It works by doing a binary search on
  the ecio_tab to find the closest value (without going over), and returning
  the index in the ecio_tab.

DEPENDENCIES
  None.

RETURN VALUE
  6-bits Ec/Io value. If the energy is out of range,  the return value is
  clipped to 0 or 63. The value returned is double that of the actual
  EcIo. For example a returned value of -13 corresponds to an actual
  EcIo of -6.5 dB.

  Similarly rscp value returned is also double the actual value.
  -200 returned corresponds to -100 dBm.

  RSSI value is returned in dBm units. A value of -90 represents an RxAGC
  value of -90 dBm.

SIDE EFFECTS

===========================================================================*/
extern void gl1_wcdma_get_ecio_rscp_rssi(g2w_srch_eng_parm_struct_type *cell_eng_ptr,gas_id_t gas_id);

#endif /* FEATURE_INTERRAT_HANDOVER_GTOW || definedFEATURE_INTERRAT_CELL_RESELECTION_GTOW */

/*===========================================================================

FUNCTION gl1_wcdma_ecio_logarithmic_average

DESCRIPTION
  This function calculates the  linear average of 16-bit previous and
  current Ec/Io values.It works by getting the actual linear values corresponding
  to the indexes pointed by current_ecio and previous_ecio in the ecio_tab.The
  average sum is calculated by
  average_eng= 0.9 * eng corresponding to current_ecio +
               0.1 * eng corresponding to previous_ecio

DEPENDENCIES
  The previous and current ecio values should be twice of the actual values.

RETURN VALUE
  16-bits Ec/Io value. If the energy is out of range,  the return value is
  clipped to 0 or 63.

SIDE EFFECTS

===========================================================================*/
extern int16  gl1_wcdma_ecio_logarithmic_average( 
  int16  current_ecio, int16 previous_ecio);

#endif /* G2W_SRCH_DRV_H */
