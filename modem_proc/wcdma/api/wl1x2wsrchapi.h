#ifndef WL1X2WSRCHAPI_H
#define WL1X2WSRCHAPI_H

/*===========================================================================
                WL1 X2W SRCH APIs header file

DESCRIPTION
  This file is to export API for other tech who wants to search WCDMA
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None. All are passive inline function calling RF modules based on feature.
  All initialization requirment of respective RF modules should be followed

Copyright (c) 2006 - 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 
$DateTime: 
$Author: 

when       who     what, where, why
---------------------------------------------------------------------------------------------------------
07/11/14   cc      Fix a wrong type of cleanup_wcdma API 
06/24/14   cc      Moving G2W drivers to WL1 folders
04/27/14   cc      Initiate this file
===========================================================================*/

#include "wfw_sw_cmd_proc_intf.h"
#include "custgsmdrv.h"
#include "trm.h"

/* =======================================================================
**                        Macros
** ======================================================================= */
#define WL1_X2W_SRCH_MAX_PSC_PER_FREQ  32

#define WL1_X2W_SRCH_LPQ_MAX_TASKS  41
/* Max tasks for Reconf List Search */
#define WL1_X2W_MDSP_LIST_SRCH_MAX_TASKS WL1_X2W_SRCH_LPQ_MAX_TASKS

#define WL1_X2W_SRCH_MAX_DED_MODE_CELLS 32

#define WL1_X2W_SRCH_MAX_PN_PILOTS 120

#ifdef FEATURE_GSM_COEX_SW_CXM
#define wl1_x2w_srch_mdsp_start_mode_ue_type WfwStartCmdUeTypeEnum
#endif

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

typedef enum
{
  /* IRAT G2W Idle Mode */
  WL1_X2W_SRCH_G2W_IDLE_MODE = WFW_IRAT_G2W_IDLE_MODE,
  /* IRAT G2W Dedicated Mode */
  WL1_X2W_SRCH_G2W_DEDICATED_MODE = WFW_IRAT_G2W_DEDICATED_MODE,
  WL1_X2W_SRCH_L2W_IDLE_MODE = WFW_IRAT_L2W_IDLE_MODE,
  WL1_X2W_SRCH_L2W_IDLE_RECONF_MODE = WFW_IRAT_L2W_IDLE_RECONF_MODE,
  WL1_X2W_SRCH_L2W_DEDICATED_MODE = WFW_IRAT_L2W_DEDICATED_MODE,
  WL1_X2W_SRCH_L2W_DEDICATED_CELL_ID_MODE = WFW_IRAT_L2W_DEDICATED_CELL_ID_MODE
} wl1_x2w_srch_wfw_mode_enum_type;

/* X2W WFW status */
typedef enum
{
  WL1_X2W_WFW_DISABLED = 0,
  WL1_X2W_WFW_SUSPENDED,
  WL1_X2W_WFW_ENABLED
} wl1_x2w_srch_wfw_status_enum_type;

/* X2W Search Substate machine */
typedef enum
{
  WL1_X2W_SRCH_NO_SEARCH,
  WL1_X2W_SRCH_STEP1_SEARCH,
  WL1_X2W_SRCH_NASTT_SEARCH,
  WL1_X2W_SRCH_PN_SEARCH,
  WL1_X2W_SRCH_STAGE1_SEARCH,
  WL1_X2W_SRCH_SECOND_STAGE1_SEARCH,
  WL1_X2W_SRCH_STAGE1_STAGE2_SEARCH,
  WL1_X2W_SRCH_STAGE2_STAGE1_SEARCH,
  WL1_X2W_SRCH_STAGE2_SEARCH,
  WL1_X2W_SRCH_RECONF_SEARCH,
  WL1_X2W_SRCH_NUM_SEARCHES
} wl1_x2w_srch_drv_search_state_enum_type;

/* X2W Outer Main State machine */
typedef enum
{
  WL1_X2W_INACTIVE_STATE,
  WL1_X2W_START_WFW_STATE,
  WL1_X2W_MODE_RF_CFG_STATE,
  WL1_X2W_RF_STOPPED_STATE,
  WL1_X2W_SEARCH_STATE,
  WL1_X2W_SLEEP_WFW_STATE,
  WL1_X2W_ABORT_STATE,
  WL1_X2W_MODE_IRAT_CFG_STATE,
  WL1_X2W_MODE_RFTUNE_STEP1_STATE,
  WL1_X2W_NUM_STATES
} wl1_x2w_srch_drv_state_enum_type;

/*===========================================================================
                        STRUCT
===========================================================================*/
/* Input into X2W id search from other tech */
/* input into list search */
typedef struct 
{
  uint16 scr_code;
  boolean cell_diversity;
  uint32 pn_pos;
  uint16 nc;
  uint32 win_size;
} wl1_x2w_srch_list_cell_struct_type;

/* ID search cell type */
typedef struct 
{
  uint16 scr_code;
  uint16 code_group;
  boolean cell_diversity;
} wl1_x2w_srch_id_cell_type;

/* ID search main input struct */
typedef struct 
{
  uint16 freq;
  uint16 num_cells;
  wl1_x2w_srch_id_cell_type id_cell[WL1_X2W_SRCH_MAX_PSC_PER_FREQ];
} wl1_x2w_srch_srchdrv_id_parms_type;

/* PN after NAS-TT results struct type */
typedef struct 
{
  uint16 scr_code;
  uint32 max_pos;
  uint32 max_eng;
  int16  rscp;
  int16  ecio;
} wl1_x2w_srch_pn_peak_struct_type ;

/* List search Results struct */
typedef struct 
{
  uint16 scr_code;
  uint32 max_pos;
  uint32 max_eng;
} wl1_x2w_srch_list_srch_peak_struct_type;

typedef struct 
{
  uint16 num_tasks;
  uint16 frequency;
  wl1_x2w_srch_list_srch_peak_struct_type cell[WL1_X2W_MDSP_LIST_SRCH_MAX_TASKS];
} wl1_x2w_srch_list_srch_struct_type;

/* PN after NAS-TT Results to be passed back */
typedef struct 
{
  uint16 num_peaks;
  uint16 frequency;
  int16 rssi;
  wl1_x2w_srch_pn_peak_struct_type srch_pn_pilots[WL1_X2W_SRCH_MAX_PN_PILOTS];
} wl1_x2w_srch_pn_after_nastt_results_type;

/* List search input parameter struct from GL1. */
typedef struct 
{
  uint16 freq;
  uint16 num_tasks;
  wl1_x2w_srch_list_cell_struct_type list_srch_tasks[WL1_X2W_MDSP_LIST_SRCH_MAX_TASKS];
} wl1_x2w_srch_g2w_list_input_task_type;

/* Dedicated mode input parameters from GL1 */
/* Input used for stage1 search */
typedef struct 
{
  uint16 scr_code;
  /* If not available, use NO_PN_POS */
  uint16 pn_pos;
} wl1_x2w_srch_g2w_ded_mode_cell_type; 

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
  wl1_x2w_srch_g2w_ded_mode_cell_type ded_mode_cell[WL1_X2W_SRCH_MAX_DED_MODE_CELLS];
} wl1_x2w_srch_g2w_ded_mode_srch_parms_type;

/* G2W dedicated search output structure. It is used for sending results back to GL1 */
typedef struct 
{
  uint16 frequency;
  int16 rssi;
  uint16 scr_code;
  uint16 pn_pos;
  uint32 tot_eng;
  int16 cell_rscp;
  int16 cell_ecno;
} wl1_x2w_srch_g2w_ded_mode_list_srch_struct_type;

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
} wl1_x2w_srch_g2w_eng_parm_struct_type;

/* Control Info struct for G2W State machine */
typedef struct
{
  /* G2W Search Mode -- Idle or Dedicated */
  wl1_x2w_srch_wfw_mode_enum_type mode;
  /* Flag to indicate if the RF is switched on */
  boolean srch_rf_on;
  /* Flag to indicate GL1 abort is delayed till AGC tuning 
  and SR done/RF ON ISR is complete */
  boolean abort_delayed;
  /* G2W state machine state */
  wl1_x2w_srch_drv_state_enum_type        driver_state;
  /* G2W search state in Idle or Dedicated */
  wl1_x2w_srch_drv_search_state_enum_type search_state;
  /* indicate if a WFW START CMD has been issued and therefore a
     response is still pending */
  boolean wfw_start_sent;
} wl1_x2w_srch_ctrl_info_struct_type;

typedef struct
{
  /* G2W mode: Idle or dedicated */
  wl1_x2w_srch_wfw_mode_enum_type mode;
  /* RxLM buffer Index */
  uint32 g2w_rxlm_buf_idx;
  /* Home RxLM buffer Index */
  uint32 g_home_rxlm_buf_idx;
  /* RF action time in USTMR/xo  */
  uint32 rf_action_time;
  /* RF startup script buffer index */
  uint8 startup_rf_buf_idx;
  /* RF cleanup script buffer index */
  uint8 cleanup_rf_buf_idx;
  /* W channel */
  uint16 W_chn;
  /* G serving channel */
  uint16 G_chn;
  /* W Band type */
  rfcom_wcdma_band_type W_band;
  /* G serving band */
  rfcom_gsm_band_type G_band;
  /* Freq error in Q6 Hz format */
  int32 freq_err_q6_hz;
  /* Inverse WCDMA DL freq */
  uint32 inv_dlo_freq;
  /* RF Device IDs */
  rfm_device_enum_type  source_rf_device;
  rfm_device_enum_type  target_rf_device;
} wl1_x2w_srch_g2w_startup_params_struct_type;

typedef struct
{
  uint32 startup_time;
  uint32 cleanup_time;
  uint16 pll_settling_time;
} wl1_x2w_srch_rf_timing_params_struct_type;

/*===========================================================================
                         VARIABLES
===========================================================================*/

/*--------------------TYPEDEF: CALLBACK_FN--------------------------------*/

/* Callback functions to GL1 modules reporting the results of G2W Idle and
   Dedicated searches */

/* Callback functions for idle */
typedef void (*wl1_x2w_srch_g2w_list_srch_result_cb)
               (wl1_x2w_srch_list_srch_struct_type *, gas_id_t gas_id);

typedef void (*wl1_x2w_srch_g2w_pn_after_nastt_srch_result_cb)
               (wl1_x2w_srch_pn_after_nastt_results_type *pn_after_nastt_ptr, gas_id_t gas_id);

typedef void (*wl1_x2w_srch_g2w_srch_abort_cb)(gas_id_t gas_id);

/* Callback functions for Dedicated mode */
typedef void (*wl1_x2w_srch_g2w_ded_mode_srch_result_cb)
               (wl1_x2w_srch_g2w_ded_mode_list_srch_struct_type *list_ptr,gas_id_t gas_id);

/* Callback function type for returning WFW status */
typedef void (*wl1_x2w_srch_g2w_wfw_status_cb_func_type) 
               (wl1_x2w_srch_wfw_status_enum_type, gas_id_t gas_id);

/*===========================================================================
                        Extern Functions
===========================================================================*/

/*===========================================================================
FUNCTION     WL1_X2W_SRCH_G2W_WFW_SW_INIT

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
extern void wl1_x2w_srch_g2w_wfw_sw_init(gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_WCDMA_CFG_INIT

DESCRIPTION
  This function is exported to GL1 to initialize G2W configuration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_x2w_srch_wcdma_cfg_init(
  /* Callback used for PN after NAS-TT search in idle */ 
  wl1_x2w_srch_g2w_pn_after_nastt_srch_result_cb  pn_after_nastt_srch_cb,
  /* Callback for Reconf List search */
  wl1_x2w_srch_g2w_list_srch_result_cb list_srch_cb,
  /* Callback for abort */
  wl1_x2w_srch_g2w_srch_abort_cb  srch_abort_cb,
  /* Callback for WFW status */
  wl1_x2w_srch_g2w_wfw_status_cb_func_type wfw_status_cb,
  /* RF Device ID */
  rfm_device_enum_type  source_rf_device,
  rfm_device_enum_type  target_rf_device,
  gas_id_t gas_id
);


/*===========================================================================
FUNCTION WL1_X2W_SRCH_WCDMA_DED_MODE_CFG_INIT

DESCRIPTION
  This function initializes parameters for G2W Dedicated mode search called by GL1

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wl1_x2w_srch_wcdma_ded_mode_cfg_init(
  /* Dedicated mode result cb handler */
  wl1_x2w_srch_g2w_ded_mode_srch_result_cb ded_mode_srch_cb,
  /* Reconf list search result handler */
  wl1_x2w_srch_g2w_list_srch_result_cb list_srch_cb,
  /* Callback for WFW status */
  wl1_x2w_srch_g2w_wfw_status_cb_func_type wfw_status_cb,
  gas_id_t                    gas_id);

#ifdef FEATURE_WCDMA
/*===========================================================================
FUNCTION     WL1_X2W_SRCH_G2W_CLEANUP_WCDMA_MEAS

DESCRIPTION  This function cleans up the G2W measurements because GL1 is going 
             INACT and shutting down drivers. It disables WFW app if WFW is not
             running, else it ERROR FATALs because GL1 did not send an abort
             if trying to cleanup during a G2W search. 

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  G2W searches will be terminated. ERR_FATALs if an abort was not 
              sent before if issued during a search. 
===========================================================================*/
extern void wl1_x2w_srch_g2w_cleanup_wcdma_meas(
  #if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
  boolean umts_get_denied,
  #endif
  gas_id_t gas_id);
#endif
/*===========================================================================
FUNCTION     WL1_X2W_SRCH_G2W_INITIATE_SEARCH

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
extern void wl1_x2w_srch_g2w_initiate_search(
  wl1_x2w_srch_wfw_mode_enum_type search_type,
  uint16 uarfcn,
  uint16 start_time,
  #ifdef FEATURE_GSM_COEX_SW_CXM
  wl1_x2w_srch_mdsp_start_mode_ue_type wfw_multi_sim_mode,
  #endif
  gas_id_t gas_id);

/*===========================================================================
FUNCTION     WL1_X2W_SRCH_G2W_INITIATE_DEDICATED_SEARCH

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
extern void wl1_x2w_srch_g2w_initiate_dedicated_search(
  wl1_x2w_srch_wfw_mode_enum_type search_type,
  uint16 uarfcn,
  uint16 start_time,
  #ifdef FEATURE_GSM_COEX_SW_CXM
  wl1_x2w_srch_mdsp_start_mode_ue_type multi_sim_mode,
  #endif
  gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_WCDMADRV_PROGRAM_LIST_SRCH

DESCRIPTION
  This function programs reconfirmation list search from GL1
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void wl1_x2w_srch_wcdmadrv_program_list_srch (
  /* Input parameters to program a Reconf list search during idle G2W */
  wl1_x2w_srch_g2w_list_input_task_type *srchlist_ptr,gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_WCDMADRV_PROGRAM_DED_MODE_SEARCH

DESCRIPTION
  This function programs parameters for G2W Dedicated mode search called by GL1

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean wl1_x2w_srch_wcdmadrv_program_ded_mode_search(
  /* Input search parameters to program a dedicated search */
  wl1_x2w_srch_g2w_ded_mode_srch_parms_type *srch_cell_ptr,
  /* Input parameters to program a Reconf list search after Dedicated search */
  wl1_x2w_srch_g2w_list_input_task_type *list_srch_ptr,gas_id_t gas_id);

/*===========================================================================
FUNCTION     WL1_X2W_SRCH_G2W_STARTUP

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
extern void wl1_x2w_srch_g2w_startup(
  wl1_x2w_srch_g2w_startup_params_struct_type *g2w_params,
  gas_id_t gas_id
);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_WCDMADRV_ABORT_SEARCH

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
extern void wl1_x2w_srch_wcdmadrv_abort_search (gas_id_t gas_id);

/*===========================================================================

FUNCTION WL1_X2W_SRCH_GET_ECIO_RSCP_RSSI

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
extern void wl1_x2w_srch_get_ecio_rscp_rssi(
  wl1_x2w_srch_g2w_eng_parm_struct_type *cell_eng_ptr,
  gas_id_t gas_id);


/*===========================================================================

FUNCTION wl1_x2w_srch_ecio_logarithmic_average

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
extern int16  wl1_x2w_srch_ecio_logarithmic_average( 
  int16  current_ecio, 
  int16  previous_ecio);

/*===========================================================================

FUNCTION      WL1_X2W_SRCH_G2W_MDSP_GET_RSSI

DESCRIPTION   This function returns the instantaneous RSSI value from the FW variable.
 
DEPENDENCIES  None.

RETURN VALUE  int16.

SIDE EFFECTS  None

===========================================================================*/
extern int16 wl1_x2w_srch_mdsp_get_rssi(gas_id_t gas_id);

/*===========================================================================

FUNCTION      WL1_X2W_SRCH_GET_RF_TIMING_PARAMS

DESCRIPTION   This function returns IRAT RF tune time info

DEPENDENCIES  

RETURN VALUE  
  None

SIDE EFFECTS  
  None

===========================================================================*/
extern void wl1_x2w_srch_get_rf_timing_params(
  wl1_x2w_srch_rf_timing_params_struct_type *rf_timing_params,
  gas_id_t gas_id);

#endif