/*!
  @file
  rflm_api_tds.h

  @brief
  LM API definitions for TDSCDMA
  
  @detail
  Provides the following API's for TDS FW to call:
  - Execute RF script
  - RF slot update
  - rx-agc calculation
  - get LNA  
  - tx-AGC update
  - get mtpl and min pwr
  
  @todo
  - comment function prototypes
  - add conversion macro for dBm units  
  - enum for modtype (prefer not to include fw .h files)
  - add FBRX output parameters / handling of FBRx uKernel output
  - add interface for PLL update for frequency error correction
  - add interface for PLL on/off for power savings

*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_tds.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/15   nsh     Expose WTR dump API to FW
12/30/14   jak     Added RFNV_TDSCDMA_<band>_MPR_VAL_V2_I support
12/05/14   jyu     Updated API interface to allow FW query status of last RF tune
11/24/14   whz     Add actual Tx power in rflm_tds_txagc_calc_in_t
09/16/14   jyu     Support to pass WB ADC to FW
09/09/14   jyu     FW-FED interface change for single Tx sharing
08/19/14   whz     Added a bound to RX_LIN_STATE type
08/05/14   jyu     Added support to separate T2L and T2G cleanup scripts
04/30/14   jyu     Added sgtdsBackOffMtpl
04/22/14   jhe     Add fbrx cal api
03/24/14   jyu     Added support for asdiv
01/30/14   jyu     Added API support for FW to request/release FED service
01/30/14   jyu     Added API support for DSDx 
01/21/14   jps     Fix fbrx include file errors
01/16/14   jps     Added interface for FBRX 
01/15/14   jyu     Fixed RxAGC bug for IFREQ
12/13/13   jyu     Exposed rxagc and txagc flags
11/26/13   jyu     Added new interface for IRAT_CLEANUP, BHO Rx one-time init, 
                   return freq and script execution time to FW
11/21/13   nsh     Add agc mode/ifreq idx in one time LNA update
11/12/13   nsh     Maitain the old and new interface at the same time for easy transistion
11/08/13   nsh     Back out FED-FW RxAGC changes
11/06/13   nsh     FED-FW RxAGC Interface change
10/21/13   jps     Hook up drif configuration
10/02/13   hdz     Added tds api verison number
09/17/13   jyu     Updated info
07/29/13   jyu     Initial verison
==============================================================================*/

#ifndef RFLM_API_TDS_H
#define RFLM_API_TDS_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_cmn.h"
#include "rflm_rxagc_api.h"
#include "rflm_api_fbrx_fw.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
/*! @brief Version of the rflm tds module.
*/
#define RFLM_API_TDS_MAJOR_VERSION            0
#define RFLM_API_TDS_MINOR_VERSION            5
#define RFLM_API_TDS_VERSION    \
              ((RFLM_API_TDS_MAJOR_VERSION << 16) | RFLM_API_TDS_MINOR_VERSION )



#define RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT 3
#define RFLM_TDS_NUM_RX_CHAINS                   2
#define RFLM_TDS_MAX_NUM_TX_SLOTS                6

#define RFLM_RXAGC_TDS_POWER_SCAN_CELL_IDX  0
#define RFLM_RXAGC_TDS_ACQ_CELL_IDX  1
#define RFLM_RXAGC_TDS_TRK_CELL_IDX  2
#define RFLM_RXAGC_TDS_TRK_INTER_FREQ_CELL_IDX_BASE 3

#define RFLM_TDS_NUM_LIN_STATES                   2
#define RFLM_TDS_RX_LIN_STATE_DEFAULT 0
#define RFLM_TDS_RX_TX_MAX_NUM_MPR_BINS	17

extern boolean rflm_tds_disable_rxagc;
extern boolean rflm_tds_disable_txagc;

extern boolean rflm_tds_debug_rxagc;

/*==============================================================================

  RF script exec/slot update related API input/output structure definitions
  
==============================================================================*/

/*==============================================================================
  For rflm_tds_exec_rf_script
==============================================================================*/

typedef enum
{  
  RFLM_TDS_EXEC_ONE_TIME_BHO_SCRIPT,  // one-time BHO
  RFLM_TDS_EXEC_LNA_SCRIPT,           // LNA programing in ACQ mode and for Ts6->Ts0 transition
  RFLM_TDS_EXEC_IRAT_STARTUP_SCRIPT,  
  RFLM_TDS_EXEC_IRAT_CLEANUP_SCRIPT,
  RFLM_TDS_EXEC_TX_WAKEUP_SCRIPT,     // TX wakeup script mainly for single Tx sharing
  RFLM_TDS_EXEC_TX_SLEEP_SCRIPT,      // TX sleep script mainly for single Tx sharing
  RFLM_TDS_EXEC_RX_TUNE_SCRIPT,       // Rx tune script mainly for DR-DSDS (in ACQ mode)
  RFLM_TDS_EXEC_RX_OFF_SCRIPT,        // Rx Off script mainly for DR-DSDS (in ACQ mode)
} rflm_tds_exec_script_type_e;

typedef struct
{
  rflm_handle_tx_t txLmHandle;                ///< TxLM Handle (used when switch Tx)
  rflm_handle_rx_t rxLMHandle[RFLM_TDS_NUM_RX_CHAINS];
                                              ///< RxLM Handle (used when switch Rx)
  boolean          switchTx;                  ///< Indicate if switch Rx (FALSE) or tx (TRUE)
} rflm_tds_exec_one_time_bho_script_t;

typedef enum
{
  RFLM_TDS_CHAIN_0,
  RFLM_TDS_CHAIN_1,
  RFLM_TDS_CHAIN_BOTH
} rflm_tds_rx_chain_e;

typedef enum
{
  RFLM_TDS_RX_LIN_STATE_LOW = RFLM_TDS_RX_LIN_STATE_DEFAULT,
  RFLM_TDS_RX_LIN_STATE_HIGH,
  RFLM_TDS_RX_LIN_STATE_INVALID
}rflm_tds_rx_lin_state_e;

typedef enum
{
  //New Intf
  RFLM_TDS_ACQ_MODE,
  RFLM_TDS_TRK_MODE,
  RFLM_TDS_TRK_IDLE_MODE,
  RFLM_TDS_TRK_IRAT_MODE,
  RFLM_TDS_PWR_SCAN_MODE,
  RFLM_TDS_IFREQ_MODE,
  RFLM_TDS_STEP0_FAIL_MODE,

  //Old Intf
  RFLM_TDS_HOME_MODE,
  RFLM_TDS_IRAT_MODE,
  
  RFLM_TDS_MODE_INVALID
} rflm_tds_rx_mode_e;

typedef enum
{
  RFLM_TDS_NB_PWR_EST,
  RFLM_TDS_WB_PWR_EST,
  RFLM_TDS_MAX_PWR_EST_TYPE
} rflm_tds_rx_pwr_est_e;


typedef struct
{
  rflm_handle_rx_t rxLMHandle[RFLM_TDS_NUM_RX_CHAINS];
                                              ///< RxLM Handle for up to two chains
  rflm_tds_rx_chain_e antSel;                 ///< Ant_Sel type
  uint8 rxGainState[RFLM_TDS_NUM_RX_CHAINS];  ///< Rx gain state for up to two chains
  rflm_tds_rx_mode_e mode;					  ///< AGC mode which could map to Rxlm mode							
  uint8 ifreqScriptBufIndex;                  ///< IFREQ script buffer index (same for both chains)
  uint32 freqInHz;                            ///< Freq in Hz
  boolean isIrat;                             ///< Flag to indicate if IRAT
  rflm_tds_rx_lin_state_e linState;
} rflm_tds_exec_lna_script_t;

typedef struct
{
  rflm_handle_rx_t iratLmHandle;              ///< IRAT LM Handle
  uint8 iratStartupScriptBuf;                 ///< IRAT startup Script buffer index
} rflm_tds_exec_irat_startup_script_t;

typedef enum
{
  RFLM_TDS_MEASURE_GSM,
  RFLM_TDS_MEASURE_LTE
} rflm_tds_meas_tech_e;

typedef struct
{
  rflm_handle_rx_t homeLmHandle[RFLM_TDS_NUM_RX_CHAINS];              
                                              ///< Home LM Handle
  uint8 iratCleanupScriptBuf;                 ///< IRAT cleanup Script buffer index
  void (*rflm_tds_callback_func)(void *);
  void * rflm_tds_callback_func_arg;
  rflm_tds_meas_tech_e measTech;
} rflm_tds_exec_irat_cleanup_script_t;

/* Rx config type definition for slot update and rx tune script */
typedef enum
  {
    DSDX_IND_IRAT,                              ///< IRAT gap
    DSDX_IND_DSDS,                              ///< DSDS QTA gap
    DSDX_IND_DSDA                               ///< DSDA gap
  } rflm_tds_dsdx_ind_e;

typedef struct
{
  rflm_handle_rx_t rxLmHandle[RFLM_TDS_NUM_RX_CHAINS];
                                              ///< RxLM handle 
  uint8 ifreqScriptBufIdx;                    ///< IFREQ Script buffer index
  uint8 homeTargetScriptBufIndex;             ///< to indicate using home buffer or target buffer
  boolean rxdEn;                              ///< RxD enable                 
  boolean ifreqEn;                            ///< IFREQ enable        
  uint8 gainState[RFLM_TDS_NUM_RX_CHAINS];    ///< Rx gain state for up to two chains
  uint32 refTime;                             ///< Ref time in USTRM unit
  rflm_tds_dsdx_ind_e dsdxInd;                ///< Additional information to differentiate IRAT, DSDS, DSDA
                                              ///  corresponding slot_type shares as RFLM_TDS_IRAT
  rflm_tds_rx_lin_state_e lin_state;
  rflm_tds_meas_tech_e measTech;
} rflm_tds_slot_update_rx_cfg_t;


/*! @brief  Input parameters for rflm_tds_exec_rf_script API */

typedef struct
{
  rflm_ustmr_t    effectTime;                ///< Time for script to take effect
  rflm_tds_exec_script_type_e scriptType;    ///< Script type to execute
  union
  {
    rflm_tds_exec_one_time_bho_script_t oneTimeBhoScriptInfo;
    rflm_tds_exec_lna_script_t lnaScriptInfo;
    rflm_tds_exec_irat_startup_script_t iratStartupScriptInfo;
    rflm_tds_exec_irat_cleanup_script_t iratCleanupScriptInfo;
    rflm_tds_slot_update_rx_cfg_t rxTuneScriptInfo;  // Rx tune script mainly for DR-DSDS (in ACQ mode)
    rflm_tds_slot_update_rx_cfg_t rxOffScriptInfo;   // Rx Off script mainly for DR-DSDS (in ACQ mode)
  } scriptInfo;                               ///< Pass additional info for the script execution
                                              ///< ONE_TIME_BHO: rflm_tds_exec_one_time_bho_script_t
                                              ///< LNA: rflm_tds_intf_exec_lna_script_t
                                              ///< IRAT_STARTUP: rflm_tds_intf_exec_irat_startup_script_t
                                              ///< IRAT_CLEANUP: rflm_tds_intf_exec_irat_cleanup_script_t
} rflm_tds_exec_rf_script_in_t;


/*! @brief  Output parameters for rflm_tds_exec_rf_script API */

typedef struct
{
  rflm_ustmr_t    est_done_time;              ///< estimated completion time  
} rflm_tds_exec_rf_script_out_t;


/*==============================================================================
  For rflm_tds_slot_update
==============================================================================*/

typedef enum
{
  RFLM_TDS_IDLE,                              ///< slot is idle
  RFLM_TDS_DL,                                ///< slot is scheduled for DL activitities, 
                                              ///< e.g., DL DCH, IFREQ
  RFLM_TDS_UL,                                ///< slot is scheduled for UL activitities, 
                                              ///< e.g., UL DCH, UppCH
  RFLM_TDS_UPPTS_ECLIPSED,                    ///< the slot is considered as "idle" from FW pesp., 
                                              ///< but this slot is occupied by UpPTS (partially)
  RFLM_TDS_IRAT                               ///< slot is scheduled for T2X IRAT activities
                                              ///< in other word, this slot is "occupied"
} rflm_tds_slot_type_e;


typedef enum
{
  RFLM_TDS_TX_NORMAL = 1,
  RFLM_TDS_TX_UPPCH = 2,
  RFLM_TDS_TX_PARTIAL_SLOT = 3
} rflm_tds_slot_update_tx_type_e;

/* Tx config type definition for slot update */
typedef struct
{
  rflm_handle_tx_t txLmHandle;                ///< TxLM handle
  rflm_tds_slot_update_tx_type_e txSlotType;  ///< Tx slot type, e.g., UppCH/Normal Tx            
  uint8 homeTargetScriptBufIndex;              ///< to indicate using home buffer or target buffer      
  uint32 refTime;                             ///< Ref time in USTRM unit
  boolean fbrx_available;                     ///< Flag to indicate if FBRX resource is available
  uint16  partialSlotLengthCx1;            ///< partial slot length    
} rflm_tds_slot_update_tx_cfg_t;

typedef union
{
  rflm_tds_slot_update_rx_cfg_t rxCfg;        ///< Rx config info 
                                              ///< Will use Rx config info for 
                                              ///< Slot type = DL, IRAT and IDLE
  rflm_tds_slot_update_tx_cfg_t txCfg;        ///< Tx config info
                                              ///< Will use Tx config info for 
                                              ///< Slot type = UL and UPPTS_ECLIPSED
} rflm_tds_slot_update_cfg_t;

/* Single event type definition for slot update */
typedef struct 
{
  rflm_tds_slot_type_e slotType;               ///< Evt transition state info
  rflm_tds_slot_update_cfg_t cfg;
} rflm_tds_slot_update_slot_cfg_t;

/* Slot name enum */
typedef enum
{
  RFLM_TDS_TS0 = 0,
  RFLM_TDS_UPPTS,
  RFLM_TDS_TS1,
  RFLM_TDS_TS2,
  RFLM_TDS_TS3,
  RFLM_TDS_TS4,
  RFLM_TDS_TS5,
  RFLM_TDS_TS6,
  RFLM_TDS_TS_INVALID
} rflm_tds_slot_num_e;

/*! @brief  Input parameters for rflm_tds_slot_update API */

typedef struct
{
  uint16 currSfm;                              ///< current subframe #
  rflm_tds_slot_num_e currSlot;                ///< current slot #
  uint8 numofSlot;                             ///< Num of state info for this scheduling point, up to 3 events
  rflm_tds_slot_update_slot_cfg_t slot[RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT];    
                                               ///< Slot config info
} rflm_tds_slot_update_in_t;

/* Head-start status enum */
typedef enum
{
  RFLM_TDS_HEADSTART_NONE,
  RFLM_TDS_HEADSTART_SCHEDULED,
  RFLM_TDS_HEADSTART_COMPLETED,
  RFLM_TDS_HEADSTART_CANCELLED
} rflm_tds_head_start_status_e;


/*! @brief  Output parameters for rflm_tds_slot_update API */
typedef struct
{
  rflm_drif_out_cfg_t   drif_out_cfg[RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT-1]; 
                                        ///< DRIF output config
  boolean drif_out_cfg_valid[RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT-1];
                                        ///< Flag to indicate which drif_out_cfg array index has valid data
  boolean fbrx_enable[RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT-1];
                                        ///< Flag to indicate if FBRX needs to be triggered or not
 
  int32 fbrx_trigger_offset[RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT-1];
                                        ///< FBRX trigger offset relative to slot boundary (Units: CX1)
  rflm_fbrx_dynamic_param_t fbrx_dyn[RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT-1]; 
                                        ///< FBRX dynamic parameters
 
  rflm_fbrx_pwr_dependent_param_t fbrx_pwr[RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT-1];  
                                        ///< FBRX power dependent parameters
  uint32  freqInKHz;
                                        ///< Freq info (in KHz) for the script got executed
  uint32  lastScriptCompleteTime;
                                        ///< The execution time of the last script of the last event (USTRM)
  rflm_tds_head_start_status_e headStartStatus;
                                        ///< Head start status
 // uint32 jdetValue[2]; // 0: TS0 JDET value, 1: non-TS0 JDET value
} rflm_tds_slot_update_out_t;


/*==============================================================================

  rx-agc API input/output structure definitions
  
==============================================================================*/

/*==============================================================================
  For rflm_tds_rxagc_calc
==============================================================================*/
/*! @brief  Input parameters for rflm_tds_rxagc_calc API */



typedef struct
{  
  rflm_handle_rx_t rxLMHandle[RFLM_TDS_NUM_RX_CHAINS];            
                                              ///< RxLM handles 
  rflm_tds_rx_chain_e chain;                  ///< Chain type (for FW use only)
                                              ///< Chain type must match the RxLM handle passed in 
                                              ///< for example, if RxLM is for Home, chain type must
                                              ///< be HOME_CHAIN_X type as well. Same applies to IFREQ
                                              ///< and IRAT
  rflm_tds_rx_mode_e mode;												
  rflm_tds_rx_pwr_est_e pwrEstType;           ///< Power estimate type (same for both chains)
  uint8 ifreqScriptBufIndex;                  ///< IFREQ script buffer index (same for both chains)
  int32 dBFS256[RFLM_TDS_NUM_RX_CHAINS];      ///< dBFS in S16Q8 format for up to two chains
  rflm_rxagc_lna_state_e currGainState[RFLM_TDS_NUM_RX_CHAINS];///< Current Rx gain state for up to two chains
  rflm_rxagc_lna_state_e currGainStateHL[RFLM_TDS_NUM_RX_CHAINS];///< Current Rx gain state for up to two chains for HL state
  rflm_tds_rx_lin_state_e lin_state;          ///HL/LL state
} rflm_tds_rxagc_calc_in_t;


/*! @brief  Output parameters for rflm_tds_rxagc_update API */

typedef struct
{
  rflm_rxagc_lna_state_e nextGainState[RFLM_TDS_NUM_RX_CHAINS];                     ///< Next Rx gain state for up to two chains
  rflm_rxagc_lna_state_e nextGainStateHL[RFLM_TDS_NUM_RX_CHAINS];                     ///< Next Rx gain state for up to two chains for HL state
  rflm_tds_rx_lin_state_e lin_state;
} rflm_tds_rxagc_calc_out_t;


/*==============================================================================
  For rflm_tds_rxagc_get_lna_offset
==============================================================================*/

/*! @brief  Input parameters for rflm_tds_rxagc_get_lna_offset API */

typedef struct
{
  rflm_handle_rx_t rxLMHandle[RFLM_TDS_NUM_RX_CHAINS];            
                                              ///< RxLM handles 
  rflm_tds_rx_chain_e chain;                  ///< Chain type (for FW use only)
                                              ///< Chain type must match the RxLM handle passed in 
                                              ///< for example, if RxLM is for Home, chain type must
                                              ///< be HOME_CHAIN_X type as well. Same applies to IFREQ
                                              ///< and IRAT
  rflm_tds_rx_mode_e mode;												
  uint8 ifreqScriptBufIndex;                  ///< IFREQ script buffer index
  uint8 gainState[RFLM_TDS_NUM_RX_CHAINS];    ///< Rx gain state for up to two chain
  rflm_tds_rx_lin_state_e linState;
} rflm_tds_rxagc_get_lna_offset_in_t;

typedef struct
{
 uint16 currSfm;                              ///< current subframe #
 uint32 jdetRead[7];
}rflm_tds_jdet_out_t;

/*! @brief  Input parameters for rflm_tds_rxagc_get_lna_offset API */

typedef struct
{
  int32 lnaOffset[RFLM_TDS_NUM_RX_CHAINS];    ///< LNA offset in dB for up to two chains
  int32 lnaOffsetHL[RFLM_TDS_NUM_RX_CHAINS];    ///< LNA offset in dB for up to two chains for HL state
  rflm_tds_rx_lin_state_e linState;
} rflm_tds_rxagc_get_lna_offset_out_t;


/*==============================================================================

  tx-agc API input/output structure definitions
  
==============================================================================*/

/*==============================================================================
  For rflm_tds_txagc_calc
==============================================================================*/

/*! @brief  Input parameters for rflm_tds_txagc_calc API */

typedef struct
{
  rflm_handle_tx_t txLmHandle;                ///< TxLM handles
  int16 txPwrdBmQ9[RFLM_TDS_MAX_NUM_TX_SLOTS];///< Desired output analog power to be used for TxAGC LUT, PA in unit??? support up to 7 slots
  boolean txPwrValid[RFLM_TDS_MAX_NUM_TX_SLOTS];
                                              ///< Indicate if Tx pwr is valid, support up to 7 slots
  int16 txActualOutputPwrdBmQ9[RFLM_TDS_MAX_NUM_TX_SLOTS];///< Desired final output power to compare with HDET support up to 7 slots
} rflm_tds_txagc_calc_in_t;

/*! @brief  Output parameters for rflm_tds_txagc_calc API */
typedef struct
{
  int16 digitalBackoffS16Q9[RFLM_TDS_MAX_NUM_TX_SLOTS];               
                                              ///< Digital pwr backoff, unit of dB in 16Q9 format
                                              ///< for example, -5<->-5*256   
                                              ///< support up to 7 slots
  uint16 gainState[RFLM_TDS_MAX_NUM_TX_SLOTS];
  uint16 gainIndex[RFLM_TDS_MAX_NUM_TX_SLOTS];
} rflm_tds_txagc_calc_out_t;


/*==============================================================================
  For rflm_tds_txagc_get_mtpl
==============================================================================*/

/*! @brief  Input parameters for rflm_tds_txagc_get_mtpl API */

typedef struct
{
  rflm_handle_tx_t txLmHandle;                 ///< TxLM handle
} rflm_tds_txagc_get_mtpl_in_t;

/*! @brief  Output parameters for rflm_tds_txagc_get_mtpl API */

typedef struct
{
  int16 mtplS16Q9;                            ///< MTPL, unit of dBm in 16Q9 format
                                              ///< for example, 23dBm<->23*512
  int16 sgtdsBackOffMtplS16Q9;                ///< MTPL, unit of dBm in 16Q9 format
                                              ///< for example, 23dBm<->23*512
  int16 minPwrS16Q9;                          ///< Min pwr, unit of dBm in 16Q9 format
} rflm_tds_txagc_get_mtpl_out_t;


/*==============================================================================
For rflm_tds_txagc_get_mpr
==============================================================================*/

/*! @brief  Input parameters for rflm_tds_txagc_get_mpr API */

typedef struct
{
rflm_handle_tx_t txLmHandle;				 ///< TxLM handle
} rflm_tds_txagc_get_mpr_in_t;


/*! @brief  Output parameters for rflm_tds_txagc_get_mpr API */

typedef struct
{
int16  mprValue[RFLM_TDS_RX_TX_MAX_NUM_MPR_BINS];	 /* MPR value (for MTPL backoff) */
} rflm_tds_txagc_get_mpr_out_t;

/*==============================================================================
  For rflm_tds_rx_config
==============================================================================*/

/*! @brief  Input parameters for rflm_tds_rx_config API */

typedef struct
{
  rflm_handle_rx_t rxLmHandle[RFLM_TDS_NUM_RX_CHAINS];  
                                              ///< RxLM handle
  uint8 homeTargetScriptBufIndex;             ///< to indicate using home buffer or target buffer
} rflm_tds_rx_config_in_t;


/*==============================================================================
  For rflm_tds_txagc_get_mtpl
==============================================================================*/

/*! @brief  Input parameters for rflm_tds_tx_config API */

typedef struct
{
  rflm_handle_tx_t txLmHandle;                ///< TxLM handle
  uint8 homeTargetScriptBufIndex;             ///< to indicate using home buffer or target buffer
} rflm_tds_tx_config_in_t;


/*==============================================================================
  For rflm_tds_schedule_asd
==============================================================================*/

typedef struct
{
  rflm_handle_rx_t rxLmHandle[2];                ///< RxLM handle
  uint32 refTime;                                ///< Ref time in USTRM unit
} rflm_tds_schedule_asd_in_t;


/*==============================================================================
  For rflm_tds_fbrx_cal_update
==============================================================================*/

/*! @brief  Input parameters for rflm_tds_fbrx_cal_update API */
typedef struct
{
  uint8 reserved; /* placeholder */
} rflm_tds_fbrx_cal_update_in_t;


/*! @brief  Output parameters for rflm_tds_fbrx_cal_update API 
 - run_fbrx: flag indicating whether or not to trigger fbrx 
 - fbrx_dynamic_params, fbrx_pwr_dep_params: uK parameters */
typedef struct
{
  boolean                         run_fbrx; 
  rflm_fbrx_dynamic_param_t       fbrx_dyn;
  rflm_fbrx_pwr_dependent_param_t fbrx_pwr;
} rflm_tds_fbrx_cal_update_out_t;

/*==============================================================================
  For rflm_tds_get_timing_parameters
==============================================================================*/

/*! @brief  Input parameters for FED timing API */
/*! @brief  Output parameters for rflm_tds_quert_wb_adc API 
 - rx0WbAdcIdx: Rx0 WB ADC index (0xFF is invalid value)
 - rx1WbAdcIdx: Rx1 WB ADC index (0xFF is invalid value) */
typedef struct
{
  uint8                         rx0WbAdcIdx; 
  uint8                         rx1WbAdcIdx; 
} rflm_tds_query_wb_adc_out_t;

typedef struct
{
  uint8 reserved; /* place holder */
} rflm_tds_get_timing_param_in_t;

/*! @brief  Output parameters (all in Cx1) for FED timing API */

typedef struct
{
  uint32 txWakeUpTimeCx1;                        /// Time for TX wake up
  uint32 offToTxTuneTimeCx1;                     /// Time for Off to TX  Script execution
  uint32 txToOffTimeCx1;                         /// Time for TX to OFF Script execution
  uint32 txSleepTimeCx1;                         /// Time for TX Sleep

  uint32 rxWakeUpTimeCx1;                        /// Time for RX wake up
  uint32 offToRxTuneTimeCx1;                     /// Time for Off to RX  Script execution
  uint32 rxToOffTimeCx1;                         /// Time for RX to OFF Script execution
  uint32 rxSleepTimeCx1;                         /// Time for RX Sleep

} rflm_tds_get_timing_param_out_t;

typedef enum
{
  RFLM_TDS_RF_TUNE_INFO_READY,
  RFLM_TDS_RF_TUNE_NOT_SCHEDULED,
  RFLM_TDS_RF_TUNE_INFO_NOT_READY  
} rflm_tds_get_rf_tune_late_time_status_e;

typedef struct
{
  rflm_tds_get_rf_tune_late_time_status_e returnStatus;
  int32 rfTuneLateTime; // USTMR count
  uint32 rfTuneExpectedCompleteTime; // USTMR count
} rflm_tds_get_rf_tune_late_time_out_t;


//extern rflm_tds_rx_lin_state_e lin_state;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
 rflm_tds_exec_rf_script
 
  @brief
    API to execute one specific RF script
 
  @details
    API to execute one specific RF script (specified by the script_type)
 
==============================================================================*/
extern rflm_err_t rflm_tds_exec_rf_script( rflm_tds_exec_rf_script_in_t  *in,
                                           rflm_tds_exec_rf_script_out_t *out );


/*==============================================================================
 rflm_tds_slot_update
 
  @brief
    API to schedule RF scripts for slot mode update
 
  @details
    API to schedule RF scripts for slot mode update. This API will
    program RF scripts, TxAGC, RxAGC but it doesn't include programing
    of RxLM/TxLM which should be handled separately. Support up to 3 event
    transition info structure. At TS0 scheduling point, it might include
    up to 3 transitions. At other scheduling point, it includes one transition
 
==============================================================================*/


extern rflm_err_t rflm_tds_slot_update( rflm_tds_slot_update_in_t  *in,
                                        rflm_tds_slot_update_out_t *out );

/*==============================================================================
  rflm_tds_rxagc_calc
 
  @brief 
    API to calculate RxAGC to determine LNA gain state
 
  @details
    API to calculate RxAGC to determine LNA gain state
 

==============================================================================*/
extern rflm_err_t rflm_tds_rxagc_calc( rflm_tds_rxagc_calc_in_t  *in,
                                       rflm_tds_rxagc_calc_out_t *out );

/*==============================================================================
  rflm_tds_rxagc_get_lna_offset
 
  @brief 
    API to get LNA offset
 
  @details
    API to get LNA offset
 

==============================================================================*/
extern rflm_err_t rflm_tds_rxagc_get_lna_offset ( rflm_tds_rxagc_get_lna_offset_in_t  *in,
                                                  rflm_tds_rxagc_get_lna_offset_out_t *out );



/*==============================================================================
 rflm_tds_txagc_calc
 
  @brief 
    API to calculate TxAGC 
 
  @details
    API to calculate TxAGC (return MTPL, digital_backoff 
    given the request output power) for the given tx handle
  
==============================================================================*/

extern rflm_err_t rflm_tds_txagc_calc( rflm_tds_txagc_calc_in_t  *in,
                                       rflm_tds_txagc_calc_out_t *out );


/*==============================================================================
 rflm_tds_txagc_get_mtpl
 
  @brief 
    API to get MTPL and min Pwr
 
  @details
    API to get MTPL and min Pwr for the given Tx handle
  
==============================================================================*/

extern rflm_err_t rflm_tds_txagc_get_mtpl( rflm_tds_txagc_get_mtpl_in_t *in,
                                           rflm_tds_txagc_get_mtpl_out_t *out );

/*==============================================================================
 rflm_tds_txagc_get_mpr
 
  @brief 
    API to get MPR
 
  @details
    API to get MPR for the given Tx handle
  
==============================================================================*/

extern rflm_err_t rflm_tds_txagc_get_mpr( rflm_tds_txagc_get_mpr_in_t *in,
                                           rflm_tds_txagc_get_mpr_out_t *out );
boolean rflm_tds_is_fbrx_enabled(void);


/*==============================================================================
 rflm_tds_rx_config
 
  @brief 
    API from FW to FED to allow FED preparation for Rx activities
 
  @details
    API from FW to FED to allow FED preparation for Rx activities
  
==============================================================================*/

extern rflm_err_t rflm_tds_rx_config( rflm_tds_rx_config_in_t *in );


/*==============================================================================
 rflm_tds_tx_config
 
  @brief 
    API from FW to FED to allow FED preparation for Tx activities
    E.g., TxAGC script initialization
 
  @details
    API from FW to FED to allow FED preparation for Tx activities
  
==============================================================================*/

extern rflm_err_t rflm_tds_tx_config( rflm_tds_tx_config_in_t *in );


/*==============================================================================
 rflm_tds_rxagc_find_cell_id
 
  @brief 
    Find the right cell id depending on the rflm rx mode
 
  @details
    
  
==============================================================================*/
extern uint8 rflm_tds_rxagc_find_cell_id(rflm_tds_rx_mode_e mode, uint8 ifreqScriptBufIndex);


/*==============================================================================
 rflm_tds_request_service
 
  @brief 
    FW call this API to request FED service
 
  @details
    
  
==============================================================================*/
extern rflm_err_t rflm_tds_request_service(void);

/*==============================================================================
 rflm_tds_release_service
 
  @brief 
    FW call this API to release FED service
 
  @details
    
  
==============================================================================*/
extern rflm_err_t rflm_tds_release_service(void);


/*==============================================================================
 rflm_tds_schedule_asd
 
  @brief 
    FW call this API to schedule asd
 
  @details
    
  
==============================================================================*/
extern rflm_err_t rflm_tds_schedule_asd(rflm_tds_schedule_asd_in_t* in);


/*==============================================================================
 rflm_tds_fbrx_cal_update
 
  @brief 
    FW call this API to get FBRX setting in FTM calibration mode
 
  @details
    
  
==============================================================================*/
extern rflm_err_t rflm_tds_fbrx_cal_update( rflm_handle_tx_t handle,
                                            rflm_tds_fbrx_cal_update_in_t  *in,
                                            rflm_tds_fbrx_cal_update_out_t *out );

extern rflm_err_t rflm_tds_jdet_ts0_read
( 
  rflm_tds_jdet_out_t *out
);

extern rflm_err_t rflm_tds_jdet_non_ts0_read
( 
  rflm_tds_jdet_out_t *out
);


extern rflm_err_t rflm_tds_get_timing_param
( 
  rflm_tds_get_timing_param_in_t *in,
  rflm_tds_get_timing_param_out_t *out
);


/*==============================================================================
 rflm_tds_query_wb_adc
 
  @brief
    API to query WB ADC info
 
  @details
    API to query WB ADC info
 
==============================================================================*/
extern rflm_err_t rflm_tds_query_wb_adc(rflm_tds_query_wb_adc_out_t *out );

extern rflm_err_t rflm_tds_get_rf_tune_late_time
(
  rflm_tds_slot_num_e slot,
  rflm_tds_get_rf_tune_late_time_out_t *out
);

extern void rflm_tds_dump_wtr(int dump_mode);



#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_API_TDS_H */
