/*!
  @file
  rflm_api_fbrx.h

  @brief
  RF LM API definitions for FBRx
  
  @detail

  
  @todo


*/

/*==============================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/fbrx/rflm_api_fbrx.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/15   zhh    save the mtpl_adj_db10 for SW
05/27/15   zhh    FBRX ILPC optimize
04/01/15    gh     Added support for handling stale data and handling LDO_OFF CB
03/16/15    gh     Added support for flush log buffer for fbrx layer
03/16/15    gh     Add support for additional ILPC immediate correction
03/12/15   oei/gh  Add env_scale to get_exp_gain_in_t and ilpc_est_data_t
01/06/15    yb     Added status type for FBRx measurements
11/25/14    ka     NV 11 ILPC fixes
12/10/14    aa     Store the GPS state globally to fix FBRx slow ramp issue
12/07/14    ndb    Added FBRx CL support (Update only the interface)
11/18/14   aro     Renamed SW based ILPC variable to better suit the feature
11/10/14    ka     Add SVLTE flag
09/17/14    gh     Update rflm_fbrx_capture_cfg_t to support power meter HDET coexistence
08/05/14    aa     [1] Added FBRX results log packet 0x1849 v3 revision changes
                   [2] Added support for new sample capture log packet 0x184A
08/01/14    aa     Added support for rsb comp during calv3 opcode based fbrx setup
07/31/14    aa     Added api to set the gps state and update it to the fbrx dm
07/09/14    ka     Ensure FBRx does not run if tx is off. Fix LDO on timing.
07/15/14    aa     Added RSB IQMC changes for mulitple gain state support
06/25/14    bn     Adding on_event_enqueued field
06/17/14    aa     Added support to enable FBRX DTR updates
06/12/14    JJ     Added FBRx disable offset time for CDMA
06/10/14    ka     Support for RB-based freq comp 
05/15/14    aa     Added tx freq info for fbrx temp comp
05/15/14    ka     Store last ldo turn on time
05/02/14    aa     Added rflm_fbrx_ccs_event_type_t 
04/23/14    ka     Adjust WTR off time
04/21/14    ka     Add FBRx ET adjustment factor
04/17/14    aa     Populate fbrx ccs event settings to dm for gtest
04/10/14    ka     Move mem init prototype to api file
04/09/14    aa     Update FBRX CCS Enable/Disable Scripts in FBRx DM
04/07/14    ka     Store additional gain est offset
04/07/14    ka     Store additional gain est offset
04/03/14    ka     Store filtered error and swp offset
03/27/14    ka     Update lin structure and cal gain state
03/18/14    aa     Update exp gain data results into buffer
03/12/14    aa     Latest FBRx changes for ILPC correction
03/07/14    aa     Added support for FBRx Log Packets
03/07/14    aa     Latest FBRx changes for droop cal
02/27/14    ka     Update LTE default gcomp
02/24/14    ka     Change txagc to int16
02/24/14    aa     Updated num rx taps 
02/21/13    aa     Added support for fbrx control via nv 
02/13/14    aa     Added support for FBRx RSB correction
02/18/14    ka     Added mtpl flag
02/17/14    ka     Updates for use with MTPL
02/13/14    aa     Update fbrx temp comp
02/13/14    aa     Write fbrx wbdc settings to mkernel
02/13/14    ka     Add carrier index to exp gain input
02/13/14    ka     Latest api updates
02/11/14    aa     Added support for fbrx dc and temp comp data
02/06/14    aa     Added support for fbrx based tx lin update
02/05/14    aa     Updated params for droop cal data
02/03/14    aa     Move enum tx_cfg_type from rfcommon_fbrx_types.h
01/30/14    aa     Updated gain cal params & added fbrx mode support
01/22/14    ka     Added additional logging and overrides
12/13/13    ka     Update gain parameters
11/25/13    aa     Updated DM for droop cal & added api to get fbrx gain state
11/19/13    aa     Updated fbrx DM data to support multiple cal channels 
11/18/13    ka     Added option to never run fbrx
11/13/13    ka     Added override mode
10/30/13    ka     Updated for latest uK APIs
10/30/13    ka     Added bridge config
10/10/13    ka     Added activate chain
09/27/13    ka     Updated get exp gain and run decision
09/12/13    ka     Added FBRx DM support
08/28/13    ka     Gtest support
08/15/13    ka     Initial version
==============================================================================*/

#ifndef RFLM_API_FBRX_H
#define RFLM_API_FBRX_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_cmn.h"
#include "rflm_txagc_api.h"
#include "rflm_api_fbrx_fw.h"


#include "atomic_ops.h"

/*==============================================================================

            EXTERNAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/
#ifndef FEATURE_D3925_FED
#define RFLM_FBRX_NUM_GAIN_STATES 4     
#define RFLM_FBRX_NUM_MODES 4           
#define RFLM_FBRX_NUM_CAL_CHANNELS 16   
#define RFLM_FBRX_NUM_FILTER_TAPS 32
#define RFLM_FBRX_DEFAULT_DYNAMIC_GAIN_COMP -17701
#define RFLM_FBRX_DEFAULT_DYNAMIC_GAIN_COMP_LTE -12888
#define RFLM_FBRX_NUM_TEMP_BINS 8
#define RFLM_FBRX_NUM_TEMP_FREQ 16
#define RFLM_FBRX_RX_CAP_LEN_MODE1 367
#define RFLM_FBRX_TX_CAP_LEN_MODE1 391
#define RFLM_FBRX_RX_CAP_LEN_MODE3 195
#define RFLM_FBRX_TX_CAP_LEN_MODE3 219
#define RFLM_FBRX_MAX_RUNS_PER_SF 3
#define RFLM_FBRX_MAX_BUFFER_SIZE 3
#define RFLM_FBRX_MAX_LIN_BUFFER_SIZE 3
#define RFLM_FBRX_TXAGC_HIST_SIZE 3

#define RFLM_FBRX_SCRIPT_DATA_BUF_SIZE_WORDS 20
/*! @brief FBRx Setup Margin to be subtracted from the slot boundary, 30 us = 576 USMTR units */
#define RFLM_FBRX_SETUP_TIME_OFFSET_USTMR     576
/*! @brief FBRx Disable Margin to be added to the slot boundary, 100 us = 1920 USMTR units */
#define RFLM_FBRX_DISABLE_TIME_OFFSET_USTMR   1920
/*! @brief FBRx Disable Margin to be added to the slot boundary for CDMA, 
  30 us = 576 USMTR units */
#define RFLM_FBRX_IMM_TIME_OFFSET_USTMR   96
#define RFLM_CDMA_FBRX_DISABLE_TIME_OFFSET_USTMR   576
#define RFLM_FBRX_LDO_TIME_THRESH 1920
#define RFLM_FBRX_RUN_THRES -450
#define RFLM_FBRX_ACCUM_PWR_THRESH -10
#define RFLM_FBRX_CLAMPING_PWR_THRESH  (RFLM_FBRX_ACCUM_PWR_THRESH - 20)
#define RFLM_FBRX_ACCUM_ERR_THRESH 0x3D7


// XXX ioei: SAMPLE CAPTURE script can be very, very large. Revisit.
#define RFLM_FBRX_SAMPLE_CAPTURE_SCRIPT_SIZE_MAX 8
//
#define RFLM_FBRX_POWER_MEASURE_SCRIPT_SIZE_MAX 5*sizeof(ccs_rf_transaction_rffe_ext_rw_t)

// returns 0.707*current threshold: use Q15, 0.707*2^15 = 23167
#define RFLM_FBRX_RET_LOW_THR(a) ((23167*a)>>15)
/*----------------------------------------------------------------------------*/

/*==============================================================================

            EXTERNAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*----------------------------------------------------------------------------*/
/*! @brief Various commands supported by FBRx module */
typedef enum
{
  RFLM_FBRX_UPDATE_TEMP_COMP=0,       ///< to update exp. gain temp comp
  RFLM_FBRX_UPDATE_FREQ_COMP=1,       ///< to update exp. gain freq comp
  RFLM_FBRX_UPDATE_CAL_DATA =2,       ///< to update exp. gain cal data
  RFLM_FBRX_UPDATE_OVERRIDE_DATA =3,  ///< to update override data                                 
  RFLM_FBRX_GET_TEMP_COMP   =4,       ///< to get exp. gain temp comp
  RFLM_FBRX_GET_FREQ_COMP   =5,       ///< to get exp. gain freq comp
} rflm_fbrx_cmd_e;

/*----------------------------------------------------------------------------*/
/*! @brief Method to decide when to run fbrx */
typedef enum
{
  RFLM_FBRX_RUN_ALWAYS = 0,           ///< indicates run fbrx every tx agc
  RFLM_FBRX_RUN_OPT_1  = 1,           ///< use optimized run decision
  RFLM_FBRX_RUN_NEVER  = 2,           ///< indicates to never run fbrx 
  RFLM_FBRX_RUN_TIMER_ONLY  = 3,      ///< indicates to run only based on timer 
  RFLM_FBRX_RUN_LOFT,                 ///< RFSW dictates LOFT over ILPC
  RFLM_FBRX_RUN_OVERRIDE              ///< For reporting. OV dictated via override_ctl
} rflm_fbrx_run_method_e;

/*----------------------------------------------------------------------------*/
/*! @brief Enums for FBRx run requested Type */
typedef enum
{
  RFLM_FBRX_RUN_POWER_MEAS_TYPE = 1, ///< indicates FBRx Run request for Power measurements
  RFLM_FBRX_RUN_VSWR_MEAS_TYPE  = 2, ///< indicates FBRx Run request for VSWR measurements
  RFLM_FBRX_RUN_NO_MEAS_TYPE    = 0xFF
} rflm_fbrx_meas_req_type_e;
/*----------------------------------------------------------------------------*/

/*! @brief FBRx Run counter index for each TxAGC iteration for current Slot per SF */
typedef enum
{
  RFLM_FBRX_RUN_COUNTER_1 = 0, ///< indicates fbrx run counter every tx agc
  RFLM_FBRX_RUN_COUNTER_2 = 1, ///< indicates fbrx run counter every tx agc
  RFLM_FBRX_RUN_COUNTER_3 = 2, ///< indicates fbrx run counter every tx agc
} rflm_fbrx_run_counter_e;

/*----------------------------------------------------------------------------*/
/*! @brief List of the enums for non-time critical (i.e. non issue seq) FBRx CCS events */
typedef enum
{
  RFLM_FBRX_CCS_SETUP_EVENT,     ///< indicates FBRx Setup Event (LDO OFF to ON)
  RFLM_FBRX_CCS_DISABLE_EVENT,   ///< indicates FBRx Disable Event (LDO ON to OFF)
  RFLM_FBRX_CCS_READBACK_POWER_EVENT,  ///< indicates FBRX sample capture Readback event
  RFLM_FBRX_CCS_READBACK_SAMPLES_EVENT,  ///< indicates FBRX sample capture Readback event
  RFLM_FBRX_CCS_ALWAYS_ON_EVENT,  ///< indicates FBRx Event (LDO ALWAYS ON)
  RFLM_FBRX_CCS_NUM_MAX_EVENT,   ///< indicates max number of FBRx CCS events supported
} rflm_fbrx_ccs_event_type_e;

/*----------------------------------------------------------------------------*/
/*! @brief List of the enums for VSWR measurements result status */
typedef enum
{
  RFLM_FBRX_MEAS_SUCCESS,             ///< indicates FBRX measurement is success
  RFLM_FBRX_MEAS_BAD_LSE,             ///< indicates FBRx measurement has LSE out of limits
  RFLM_FBRX_MEAS_BAD_GAIN_EST,   ///< indicates FBRx measurement has  bad incident gain
} rflm_fbrx_meas_status_e;

/*----------------------------------------------------------------------------*/
/*! @brief Data structure for passing tuner update event based script to RFLM. */
typedef struct
{
  /*! @brief Header and tasks for tuner update event */
  rflm_ccs_rf_event_header_task_type header_task;
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[RFLM_FBRX_SCRIPT_DATA_BUF_SIZE_WORDS];
} ALIGN(4) rflm_fbrx_ccs_event_infor_type;

/*----------------------------------------------------------------------------*/
/*! @brief Enum used to hold possible FBRx Tx configurations */
typedef enum
{
  RFCOMMON_FBRX_UMTS_DC,            ///< UMTS DCHSUPA  
  RFCOMMON_FBRX_MCDO_7X,            ///< 7X multi-carrrier DO 
  RFCOMMON_FBRX_LTE_20,             ///< LTE 20MHz  
  RFCOMMON_FBRX_LTE_15,             ///< LTE 15MHz  
  RFCOMMON_FBRX_SB_SVDO,            ///< SV SBDO  
  RFCOMMON_FBRX_LTE_20_PLUS_20,     ///< LTE 20 + 20 MHz  
  RFCOMMON_FBRX_1X,                 ///< 1X/C2K  
  RFCOMMON_FBRX_MCDO_3X,            ///< 3X multi-carrrier DO  
  RFCOMMON_FBRX_UMTS_SC,            ///< UMTS single carrier Tx  
  RFCOMMON_FBRX_TDS,                ///< TDSCDMA  
  RFCOMMON_FBRX_LTE_1P4,            ///< LTE 1.4MHz 
  RFCOMMON_FBRX_LTE_3,              ///< LTE 3MHz  
  RFCOMMON_FBRX_LTE_5,              ///< LTE 5MHz  
  RFCOMMON_FBRX_LTE_10,             ///< LTE 20MHz 
  RFCOMMON_FBRX_NUM_TX_CONFIG_MAX,  ///< MAX Number of Tx Bandwidth config supported 
} rfcommon_fbrx_tx_cfg_type;

/*----------------------------------------------------------------------------*/

typedef enum
{
  RFLM_GPS_INACTIVE = 0,
  RFLM_GPS_ACTIVE = 1,
}rflm_fbrx_gps_state_type;

/*==============================================================================

            EXTERNAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*------------------------------------------------------------------------------
RFLM fbrx event template type, SW to fbrx main interface
------------------------------------------------------------------------------*/
/*!  @brief fbrx DM data for one external cal point */
typedef struct
{
  int16 tx_pwr;         ///< tx power at which cal was performed
  int32 gain;           ///< fbrx gain from cal
  uint8 ubias;          ///< ubias value from cal 
  uint16 rx_scale;      ///< rx scale value from cal
  uint16 predet_index;  ///< shift in Tx samples to be applied if time-alignment is disabled
} rflm_fbrx_gain_cal_data_t;

typedef struct
{
  rflm_fbrx_gain_cal_data_t gain_state[RFLM_FBRX_NUM_GAIN_STATES];
} rflm_fbrx_gain_idx_data_t;

typedef struct
{
  rflm_fbrx_gain_idx_data_t fbrx_mode[RFLM_FBRX_NUM_MODES];
} rflm_fbrx_gain_cal_params_t;

/*----------------------------------------------------------------------------*/
/*!  @brief expected gain data for each slot per SF */
typedef struct
{
  uint8  fbrx_gain_state;         ///< fbrx gain state (0,1,2 or 3) 
  uint8  last_rb_start;           ///< rb start value
  uint8  last_rb_blocks;          ///< number of rb blocks 
  uint8  last_carrier_index;      ///< carrier index  

  uint8  ubias_new;               ///< ubias
  uint8  valid_slot_idx;          ///< Slot indexes for each valid FBRx run 
  uint8  pa_state_idx;            ///< PA state indexes for each valid FBRx run
  uint8  rgi_row_idx;             ///< RGI indexes for each valid FBRx run

  int8   modulator_adjustment_db10; ///< Adjustment to baseband power applied by the modulator
  uint8  cal_ubias;               ///< ubias used during fbrx gain cal 
  uint8  fw_abort_status;         ///< FW Abort Status (1 = Abort)                                       
  rflm_fbrx_es_iq_t es_iq_sel;    ///< Select envelope scaling or IQ scaling 

  uint16 last_cal_gain;           ///< fbrx cal gain value
  int16  last_tx_agc;             ///< txagc in dB10 uints  
  int16  last_temp_comp;          ///< temp comp value to apply
  int16  delta_prx;               ///< power delta for gain error computation                            

  int16  gain_error_dB10;         ///< gain error in dB10 units  
  int16  gain_est_offset;         ///< Additional offset to be applied due to scale saturation

  int16  cal_pwr;                 ///< tx power at which fbrx gain cal was done                            
  uint16 cal_rxscale;             ///< rx_scale value used during fbrx gain cal

  uint16 rxscale_new;             ///< rx scale factor
  uint16 txscale_new;             ///< tx scale factor after modulator adjustment                                  
                                    
  uint16 threshold_gain;          ///< threshold gain passed to fbrx micro-kernel
  uint16 current_gain;            ///< current gain passed by tech txagc

  uint16 bad_sample_capture_idx;  ///< bad sample capture counter index
  uint16 thresh_gain_high;          ///< threshold gain passed to fbrx micro-kernel
  uint16 thresh_gain_low;          ///< threshold gain passed to fbrx micro-kernel

  rflm_fbrx_meas_req_type_e fbrx_meas_req_type; ///< Type of the FBRx Measurement request
} rflm_fbrx_exp_gain_slot_data_t;

/*!  @brief expected gain data for each SF */
typedef struct
{
  rflm_fbrx_exp_gain_slot_data_t slot_data[RFLM_FBRX_MAX_RUNS_PER_SF];
} rflm_fbrx_exp_gain_sf_data_t;



/*!  @brief fbrx DM data used to calculate expected gain */
typedef struct
{
  int16 temp_comp[RFLM_FBRX_NUM_GAIN_STATES]; ///< most recent temp comp value to apply to exp. gain           
  int32 freq_comp;                            ///< most recent freq comp value to apply to exp. gain
  rflm_fbrx_gain_cal_params_t cal_params[RFLM_FBRX_NUM_CAL_CHANNELS];  ///< gain cal data

  rflm_fbrx_exp_gain_sf_data_t  sf_data[RFLM_FBRX_MAX_BUFFER_SIZE];  

  uint8 read_buf_idx;
  uint8 write_buf_idx;           
} rflm_fbrx_exp_gain_data_t;

/*----------------------------------------------------------------------------*/
/*!  @brief FBRX DM control data */
typedef struct
{
  rflm_tech_id_t tech;                  ///< rflm tech id ( 0=1X, 1=HDR, 2=LTE, 3=WCDMA, 4=TDSCDMA, 5=GSM )                                        
  uint8 fbrx_band;                      ///< rfcom_band_type_u band 
  uint8 rfm_mode;                       ///< rfm_mode_enum_type rfm tech
  rfcommon_fbrx_tx_cfg_type bandwidth;  ///< rfcommon_fbrx_tx_cfg_type tech bandwidth 
  
  uint8 fbrx_mode;                      ///< flag indicating the fbrx mode (0,1,2 or 3)
  boolean mem_in_use;                   ///< flag indicating if this buffer is in use      
  boolean ftm_cal_mode;                 ///< flag indicating FTM Cal mode status 
  boolean mtpl_mode_enable;             ///< flag indicating if fbrx mtpl mode is enabled or disabled  

  boolean txlin_update_flag;            ///< flag indicating if ILPL correction is enabled or disabled
  boolean txfe_update_flag;             ///< flag indicating if immediate DTR updates is enabled or disabled
  boolean all_rgi_update_flag;          ///< flag to indicate the LIN update method 
  boolean on_event_enqueued;            ///< flag to indicate that on_event has been enqueud (used by W tech)  

  boolean uk_results_valid_flag[RFLM_FBRX_MAX_RUNS_PER_SF];  ///< flag indicating which set(s) of results have been updated 
  uint8 lin_buff_index;

  int log_results_context_id;           ///< rflm log packet context id for logging fbrx uk results 
  int log_captures_context_id;          ///< rflm log packet context id for logging fbrx sample captures  
  int32 fbrx_nv_ctrl_status;            ///< FBRX NV Control data for current tech 
  uint32 min_tx_freq;                   ///< min tx freq for temp comp computation
  uint32 max_tx_freq;                   ///< max tx freq for temp comp computation
  uint32 cur_tx_freq;                   ///< current tx freq for temp comp computation
  rflm_handle_rx_t fbrx_lm_handle;      ///< FBRx LM handle
  int16 min_pwr_correction_accum;
  boolean min_pwr_accum_clear_requested_flag;
  rflm_ustmr_t last_ldo_time;           ///< Time at which the FBRx LDO was last turned on 
  rflm_ustmr_t fbrx_start_time;         ///< Time at which CMN FW starts the FBRx 
  rflm_ustmr_t fbrx_end_time;           ///< Time at which CMN FW stops FBRX 
  uint32 num_fbrx_fw_skip_cnt;          ///< Number of valid FBRX skips from bootup 
  rflm_fbrx_gps_state_type rfcommon_gps_state;            ///< GPS state if active or not
  boolean enable_sw_based_ilpc;            ///< Enable SW based FBRX ILPC 
  boolean fbrx_ldo_on;                  ///< FBRX LDO On or not   
  boolean c2k_is_in_svlte_mode;         ///< C2K is active in SVLTE mode
  uint8 fbrx_ilpc_activated;            ///< FBRx ILPC actived history - keeps track of current
                                        ///< (LSB) and 7 prev states.
} rflm_fbrx_ctl_data_t;

/*----------------------------------------------------------------------------*/
/*! @brief FBRx DM Droop Cal data */
typedef PACK (struct)
{
  uint8 nv_version;                               ///< Reserved : NV revision
  uint8 group_delay;                              ///< Rx Filter Group Delay
  uint8 num_filter_taps;                          ///< Number of FIR filter coefficients to be used 
  int32 filter_coeff[RFLM_FBRX_NUM_FILTER_TAPS];  ///< FIR filter coefficients 
}rflm_fbrx_droop_data_t;

/*----------------------------------------------------------------------------*/
/*! @brief FBRx DM RSB coefficients data */
typedef struct
{
  int32 a_coeff;    ///< A coefficient for IQ Mismatch 
  int32 b_coeff;    ///< B coefficient for IQ Mismatch 
  uint32 iqmc;      ///< IQ Mismatch compensation: Group IQMC_A_B_COEFFS:
} rflm_fbrx_rsb_data_t;

/*----------------------------------------------------------------------------*/
/*! @brief FBRx DM Rx DC compensation data */
typedef struct
{
  int32 dc_i;       ///< DC compensation for I channel 
  int32 dc_q;       ///< DC compensation for Q channel 
} rflm_fbrx_dc_params_type;

typedef struct
{
  rflm_fbrx_dc_params_type gain_state[RFLM_FBRX_NUM_GAIN_STATES];
} rflm_fbrx_dc_data_t;

/*----------------------------------------------------------------------------*/
/*! @brief FBRx DM Temp compensation data */
typedef struct
{
  int16 temp[RFLM_FBRX_NUM_TEMP_BINS][RFLM_FBRX_NUM_TEMP_FREQ];
} rflm_fbrx_temp_params_type;

typedef struct
{
  rflm_fbrx_temp_params_type gain_state[RFLM_FBRX_NUM_GAIN_STATES];
} rflm_fbrx_temp_comp_data_t;

/*----------------------------------------------------------------------------*/
/*!  @brief fbrx DM data for the uK gain estimate results */
typedef struct
{
  ///< flag indicating if the txlin updates is required for each slot per SF
  boolean apply_update[RFLM_FBRX_MAX_RUNS_PER_SF];  
  ///< gain error in dB10 units for each slot per SF
  int16 gain_err[RFLM_FBRX_MAX_RUNS_PER_SF];  
  ///< table containing the updated tx smps biased power 
  int16 filt_error_db10[RFLM_TXAGC_PA_STATE_MAX][RFLM_TXAGC_TX_LIN_APT_SIZE]; 
  ///< table of which PA states have been updated
  int8 pa_state_update[RFLM_TXAGC_PA_STATE_MAX];  
  ///< PA state to update
  uint8 pa_state_idx[RFLM_FBRX_MAX_RUNS_PER_SF];
  ///< RGI to update
  uint8 rgi_row_idx[RFLM_FBRX_MAX_RUNS_PER_SF];
  ///< Tx power to update
  int16 tx_pwr_db10[RFLM_FBRX_MAX_RUNS_PER_SF];
  ///< Offset to switchpoints based on tx lin update
  int16 swp_offset;
  uint8 num_valid_results;
} rflm_fbrx_lin_update_t;

/*----------------------------------------------------------------------------*/
/*!  @brief fbrx run decision control data */
typedef struct
{
  ///< table of which RGIs have been updated
  int8 fbrx_updates[RFLM_TXAGC_PA_STATE_MAX][RFLM_TXAGC_TX_LIN_APT_SIZE]; 
  ///< ustmr time that update bits were last cleared
  uint32 last_time_cleared; 
  ///< method to decide when to run fbrx
  rflm_fbrx_run_method_e run_method;  
  boolean fbrx_available;
  ///< directive to run LOFT/RSB compensation
  boolean do_loft;
  ///< ustmr duration for maximum execution of get_run_decision()
  uint32 max_exec_time;
  ///< Store the last PA state
  uint8 last_pa_state;
  int16 tx_agc_hist[RFLM_FBRX_TXAGC_HIST_SIZE];
  int8  tpc_run_length;
  int8  prev_tx_agc_delta_sign;
  int8  zero_delta_run_length;
  boolean power_ramp_active;
} rflm_fbrx_run_dec_data_t;


/*----------------------------------------------------------------------------*/
/*!  @brief fbrx uk params override control data type */
typedef struct
{
  uint16  predet_tau;       ///< Pre-determined tau value
  uint16  predet_index;     ///< Pre-determined shift in Tx samples to be applied if time-alignment is disabled
  uint32  current_gain;     ///< Current gain value
  uint32  current_loft;     ///< Current loft value
  uint32  gain_fbrx;        ///< FBRx expected gain value
  uint16  threshold_ls;     ///< The threshold LS error value
  uint16  threshold_gain;   ///< The threshold Gain value
  uint16  threshold_loft;   ///< The threshold Loft value
  boolean txfe_update;      ///< Enable/Disable Immediate Updates to Baseband 
  uint16  scale_rx;         ///< Rx Scale Factor 
  uint16  scale_tx;         ///< Tx Scale Factor 
  uint8   input_bias;       ///< Input Bias to be applied
  uint32  dc_i;             ///< DC compensation for I channel 
  uint32  dc_q;             ///< DC compensation for Q channel 
  uint32  fbrx_rx_tx_delay; ///< Tx/Rx delay control for alignment 
  uint8   num_corr_lags;    ///< Number of cross-correlation required
  uint8   num_filter_taps;  ///< Number of Rx Filter Taps to be used
} rflm_fbrx_uk_override_t;


/*----------------------------------------------------------------------------*/
/*!  @brief FBRx override control data type */
typedef struct
{
  boolean override_mode;              ///< flag indicating if override mode is being used
  boolean enable;                     ///< flag indicating to force enable or disable
  boolean use_count;                  ///< flag indicating if fbrx should run for only count iterations
  uint16  counter;                    ///< number of iterations to force enable if use_count is TRUE
  boolean override_uk_params;         ///< flag indicating if uk params should be overriden 
  rflm_fbrx_uk_override_t uk_params;  ///< fbrx uk parameters to be overriden 
  uint8   ubias;                      ///< ubias value               
  uint16  scale_rx;                   ///< Rx scale value                
  uint8   gain_state;                 ///< FBRx gain state
  boolean do_loft;                    ///< flag indicating to run LOFT/RSB compensation (TRUE) or ILPC (FALSE)

  uint32 txc_num_samples;
  uint32 txc_sample_count_action_time;
} rflm_fbrx_override_ctl_t;


/*----------------------------------------------------------------------------*/
/*!  @brief fbrx TXC programming parameters */
typedef struct 
{
  uint32 num_samples;
  uint32 sample_count_action_time;
  uint8 chain;
  boolean do_loft;
} rflm_fbrx_txc_cfg_t;

/*----------------------------------------------------------------------------*/
/*!  @brief FBRX readback actions configuration */
typedef struct 
{
  rflm_ccs_rf_tq_handle_t *tq_handle;
  uint32* ccs_mem_addr;
  uint32 ccs_script_size;
  rflm_fbrx_ccs_event_infor_type* fbrx_script_ptr;

  uint32 stmr_action_time;
  
  rflm_fbrx_ccs_event_type_e event_id;
  rflm_ccs_rf_task_callback_t callback;
  void* callback_arg;
} rflm_fbrx_readback_cfg_t;


typedef enum {
  RFLM_FBRX_SETUP,
  RFLM_FBRX_ACTION,
  RFLM_FBRX_DONE,
  RFLM_FBRX_END_IST,
  RFLM_FBRX_START_POST_PROC,
  RFLM_FBRX_END_POST_PROC,
  RFLM_FBRX_MAX_NUM_STMR_LOGS
} rflm_fbrx_stmr_log_e;

typedef struct {
  uint32 stmr_capture_duration;
  uint32 num_txc_captured_samples;
  uint32 inv_num_txc_captured_samples_Q16;
  uint32 inv_num_wtr_captured_samples_Q16;

  rflm_ccs_rf_tq_handle_t* tq_handle;
  uint32* ccs_mem_addr;
  uint32 ccs_script_size;
  uint32 stmr_logs[RFLM_FBRX_MAX_NUM_STMR_LOGS];

  uint16 num_wtr_captured_samples;                                    ///< Number of WTR samples to be captured
  boolean power_meter_hdet_coex;                                      ///< flag indicating if FBRx ILPC and power meter HDET coexist
  void (*power_meter_hdet_cb)(boolean, rflm_handle_tx_t, void*);      ///< power meter HDET CB used when FBRx ILPC is done
  void (*enqueue_ldo_off_cb)(rflm_handle_tx_t, rflm_ustmr_t, boolean);///< CB for scheduling LDO OFF
} rflm_fbrx_capture_cfg_t;


/*----------------------------------------------------------------------------*/
/*! @brief  Data parameters for ILPC gain estimation                          */

typedef enum
{
  RFLM_FBRX_ILPC_SCHEME_TYPE_2 = 0,
  RFLM_FBRX_ILPC_SCHEME_TYPE_3
} rflm_fbrx_ilpc_scheme_e;

typedef enum {
  ILPC_INACTIVE,
  ILPC_1DB_STEP,
  ILPC_2DB_STEP,
  ILPC_3DB_STEP,
  ILPC_NO_CORRECTION_BIG_STEP,
  ILPC_NO_CORRECTION_NO_PA_CHANGE,
  ILPC_NO_CORRECTION_BEYOND_LIMITS,
  ILPC_STALE_PREV_DATA,
  ILPC_NO_CORRECTION_FLOOR_SIG
} rflm_fbrx_ilpc_decision_e;

typedef struct
{
  /*! @brief Squared sum I 49Q31 (TXC_REF_RMS_POWER_<M,L>_Aa) ie SUM_II */
  uint64 txfe_squared_sum_i;
  /*! @brief Squared sum Q 49Q31 (TXC_REF_POWER_SUM_QQ_<M,L>_Aa) */
  uint64 txfe_squared_sum_q;
  /*! @brief Sum I 32Q15 (TXC_REF_POWER_SUM_I_Aa) */
  int32 txfe_sum_i;
  /*! @brief Sum Q 32Q15 (TXC_REF_POWER_SUM_Q_Aa) */
  int32 txfe_sum_q;

  /*! @brief Squared sum I 40Q25 */
  uint64 wtr_squared_sum_i_and_q;
  /*! @brief Sum I 32Q16 */
  int32 wtr_sum_i;
  /*! @brief Sum Q 32Q16 */
  int32 wtr_sum_q;

  /*! Params for gain estimation:
        1/NUM_TXFE_CAPTURED_SAMPLES Q32 unsigned
        1/NUM_WTR_CAPTURED_SAMPLES Q32 unsigned
        TX_TO_FBRX_NUM_SAMPLE_RATIO
        RF_COUPLER_GAIN
        shift_N: LOG2(MAX(NUM_TXFE_CAPTURED_SAMPLES, NUM_WTR_CAPTURED_SAMPLES))
   */
  uint32 inv_num_txc_captured_samples_Q16;
  uint32 inv_num_wtr_captured_samples_Q16;
  /*! 0 = TYPE_2, 1 = TYPE_3 */
  rflm_fbrx_ilpc_scheme_e ilpc_gain_est_scheme;

  /*! TX chain index: 0,1 */
  uint8 chain;


  /*! @brief Current slot PA state */
  uint8 pa_state_curr;
  /*! @brief Previous slot PA state */
  uint8 pa_state_prev;
  /*! @brief Current slot TX power, dB10 */
  rflm_db10_t tx_pwr_curr;
  /*! @brief Previous slot TX power, dB10 */
  rflm_db10_t tx_pwr_prev;


  /*! @brief Current HW TXFE GAIN value */
  uint32 txc_iq_gain_current;
  /*! @brief Current ILPC Gain Estimate Q10 */
  uint32 gain_est_curr;
  /*! @brief Previous ILPC Gain Estimate Q10 */
  uint32 gain_est_prev;
  /*! Hardware Gain value, updated */
  uint32 txc_iq_gain;
  /*! Gain correction in dB Q24 */
  int32 gain_correction_dB;

  rflm_fbrx_ilpc_decision_e decision; 

  /*! @brief Current XPT mode: APT, EPT, ... */
  rflm_txagc_xpt_mode_t  xpt_mode_curr;
  /*! @brief Previous XPT mode: APT, EPT, ... */
  rflm_txagc_xpt_mode_t  xpt_mode_prev;
  uint32 txc_env_scale_current;
  /*! Hardware Envelope Scale value, updated */
  uint32 txc_env_scale;

  /*! @brief CCS memory pointer and READBACK script size */
  uint32* ccs_mem_addr;
  uint32 ccs_script_size;

  /*! @brief tx error accum */
  int16 tx_err_accum;
  int16 lut_rvt;
  boolean temp_comp_ind;
  int16 mtpl_adj_db10;

  /*! STMR logs */
  struct {
    uint32 get_exp_gain_start;
    uint32 get_exp_gain_end;
    uint32 tx_action_time;
    uint32 ilpc_est_callback_start;
    uint32 ilpc_est_callback_end;
  } stmr_log;

  struct {
    uint32 txfe_U1;
    uint32 txfe_U2;
    uint32 wtr_U1;
    uint32 wtr_U2;
    uint32 g_est_ratio;
    uint32 g_error;
  } intermediate;

  boolean stale_data;

} rflm_fbrx_ilpc_est_data_t;


typedef struct {
  uint32 stmr;  
  rflm_fbrx_ilpc_decision_e decision;
  uint8 pa_state;
  int16 txpwr_curr_db10;
  int16 txpwr_prev_db10;
  
  uint64 txfe_squared_sum_i;
  uint64 txfe_squared_sum_q;
  int32 txfe_sum_i;
  int32 txfe_sum_q;

  uint64 wtr_squared_sum_i_and_q;
  int32 wtr_sum_i;
  int32 wtr_sum_q;

  uint32 gain_est_curr;
  uint32 gain_est_prev;

  uint32 g_error;

  uint32 txc_iq_gain_current;
  uint32 txc_iq_gain_new;
  int32 gain_correction_db;
  
  rflm_txagc_xpt_mode_t  xpt_mode_curr;
  rflm_txagc_xpt_mode_t  xpt_mode_prev;
  uint32 txc_env_scale_current;
  uint32 txc_env_scale_new;

  int32 tx_err_accum;
  int32 lut_rvt;
} rflm_fbrx_ilpc_log_t;



/*VSWR measurements data structure */
typedef struct
{
  int16 incident_gain_est;
  int16 incident_pwr_dbm10;
  int16 reflected_gain_est;
  int16 reflected_pwr_dbm10;
  int16 txagc_dB10;
  rflm_fbrx_meas_status_e meas_status[2]; /* meas status for reflected and incident power*/
}rflm_fbrx_vswr_meas_data_t;

/*==============================================================================

            Function Pointer decalartions

==============================================================================*/
/*! @brief  call back function for post processing the  VSWR measurements results  */
typedef void (*rflm_fbrx_tech_post_process_vswr_measurement_fp_type)( 
              rflm_handle_tx_t handle,
              rflm_fbrx_vswr_meas_data_t vswr_meas_data
              );

#define RFLM_LOG_ILPC_SIZE 4
/*------------------------------------------------------------------------------
RFLM FBRX Event Template Type :  SW to FBRX Main Interface
------------------------------------------------------------------------------*/

/*!  @brief FBRx DM template type */
typedef struct
{
  rflm_fbrx_exp_gain_data_t      exp_gain_data;     ///< data required to calculate exp. gain
  rflm_fbrx_run_dec_data_t       run_decision_data; ///< data used to decide when to run fbrx
  rflm_fbrx_ctl_data_t           fbrx_ctl;          ///< internal control data
  rflm_fbrx_override_ctl_t       override_ctl;      ///< fbrx override control params 
  int16                          fbrx_swp[RFLM_FBRX_NUM_GAIN_STATES]; ///< FBRx Switchpoints
  union {
    struct {
      rflm_fbrx_droop_data_t         droop_cal_data;    ///< droop cal params data for current fbrx mode
      rflm_fbrx_dc_data_t            dc_cal_data;       ///< dc cal data for current fbrx mode
      rflm_fbrx_temp_comp_data_t     temp_comp_data;    ///< temp comp data for current fbrx mode
      rflm_fbrx_rsb_data_t           rsb_data[RFLM_FBRX_NUM_GAIN_STATES];  ///< FBRx RSB coefficients
    };
    rflm_fbrx_ilpc_log_t ilpc_log[RFLM_LOG_ILPC_SIZE];
  };
  uint8 log_idx;
  uint8 log_packet_idx;

  rflm_fbrx_update_results_in_t  results_log;       ///< store the uK processing results
  rflm_fbrx_lin_update_t         lin_upd_data[RFLM_FBRX_MAX_LIN_BUFFER_SIZE];      ///< data required for txlin updates

  rflm_fbrx_capture_cfg_t        ilpc_meas_config;
  rflm_fbrx_capture_cfg_t        lrc_meas_config;
  rflm_fbrx_readback_cfg_t       readback_config;
  rflm_fbrx_ilpc_est_data_t      ilpc_gain_est_data;
  void*                          fbrx_script_ptr[RFLM_FBRX_CCS_NUM_MAX_EVENT];  ///< pointer to the fbrx script data (rfmeas_event_infor_type* ) 
  rflm_fbrx_tech_post_process_vswr_measurement_fp_type post_process_vswr_meas_fp;/* Call back function for post processing the
                                                                                    VSWR measurements*/
}rflm_fbrx_dm_template_t;
#endif


/*==============================================================================
 
  FBRx API input/output structure definitions
 
==============================================================================*/

/*==============================================================================
  For rflm_fbrx_ilpc_txfe_gain_conversion
==============================================================================*/
#define RFLM_ILPC_NUM_LIMIT_SETS 2
#define RFLM_ILPC_NUM_GAIN_STEP_SIZES 3

typedef struct{
  uint32 lower_limit;      //< Q20
  uint32 upper_limit;      //< Q20
  uint32 allowable_error;  //< Q10
  uint32 allowable_error_imm; //< Q10
} rflm_ilpc_correction_t;

extern rflm_ilpc_correction_t rflm_ilpc_correction[RFLM_ILPC_NUM_LIMIT_SETS*RFLM_ILPC_NUM_GAIN_STEP_SIZES];

/*==============================================================================
  For rflm_fbrx_get_exp_gain
==============================================================================*/

/*----------------------------------------------------------------------------*/
/*! @brief Input parameters for rflm_fbrx_get_exp_gain API */
/*! @brief Current Slot index data (LTE) */
typedef struct
{
  rflm_fbrx_run_counter_e run_idx; ///< Run index counter for current slot per SF                 
  uint8 cur_slot_idx;    ///< Current Slot Index                          
  uint8 pa_idx;          ///< PA state indexes for each valid FBRx run
  uint8 rgi_idx;         ///< RGI indexes for each valid FBRx run
}rflm_fbrx_tx_slot_data_t;

typedef struct
{
  int16  cal_pwr;
  uint16 cal_gain;
  uint16 cal_predet_index;
  uint8  cal_ubias;
  uint16 cal_rx_scale;
} rflm_fbrx_rb_freq_comp_type;

/*! @brief  Input parameters for rflm_fbrx_get_exp_gain API */
typedef struct
{
  rflm_tech_id_t tech_id;
  int32          tx_pwr_db10;
  int32          iq_scaling;
  uint8          start_rb;
  uint8          num_rb;
  rfcommon_fbrx_tx_cfg_type bandwidth;
  uint8          carrier_index;
  rflm_fbrx_tx_slot_data_t tx_slot_data;
  int8           modulator_adjustment_db10;
  rflm_txagc_xpt_mode_t  xpt_mode;
  rflm_fbrx_rb_freq_comp_type rb_freq_params;
  boolean        uses_rb_freq_comp;
  rflm_fbrx_es_iq_t es_iq_sel;      ///< Select envelope scaling or IQ scaling 
  uint32 current_gain;              ///< current gain passed by tech txagc
  uint16 thresh_gain_high;          ///< threshold gain passed to fbrx micro-kernel
  uint16 thresh_gain_low;          ///< threshold gain passed to fbrx micro-kernel
  uint32         ustmr_action_time;
  uint32         txc_sample_count_action_time;
  uint32         iq_gain_fw;                      //< TXC_IQ_GAIN value
  uint32         env_scale_fw;                    //< TXC_ENV_SCALE value
  uint8          chain;
  boolean        do_loft;
  rflm_fbrx_meas_req_type_e fbrx_meas_req_type;
} rflm_fbrx_get_exp_gain_in_t;

/*! @brief  Output parameters for rflm_fbrx_get_exp_gain API */
typedef struct
{
  ///< FBRx expected gain based on external cal 
  uint32 exp_gain; 
  ///< TXFE gain calibration factor 
  uint32 txfe_gain;
  ///< Threshold for Total LS Error 
  uint32 threshold_ls;
  ///< Input bias to be applied to format conversion from FBRx buffer 
  uint8 input_bias;
  ///< DC compensation for Q channel 
  uint32 wbdc_q;
  ///< DC compensation for I channel 
  uint32 wbdc_i;
  ///< Rx Scale Factor 
  uint16 scale_rx;
  ///< Shift in Tx samples to be applied if time-alignment is disabled
  uint16 predet_idx;  
  ///< Tx Scale Factor 
  uint16 scale_tx;
} rflm_fbrx_get_exp_gain_out_t;

/*==============================================================================
  For rflm_fbrx_arbitrator
==============================================================================*/
/*! @brief  Input parameters for rflm_fbrx_arbitrator API */
typedef struct
{
  rflm_tech_id_t tech_id;
} rflm_fbrx_arbitrator_in_t;

/*! @brief  Output parameters for rflm_fbrx_arbitrator API */
typedef struct
{
  boolean fbrx_use_allowed;
} rflm_fbrx_arbitrator_out_t;

/*==============================================================================
  For rflm_fbrx_get_gain_state
==============================================================================*/
/*! @brief  Input parameters for rflm_fbrx_get_gain_state API */
typedef struct
{
  int32          tx_pwr_db10;
} rflm_fbrx_get_gain_state_in_t;

/*! @brief  Output parameters for rflm_fbrx_get_gain_state API */
typedef struct
{
  uint8          fbrx_gain_state;
} rflm_fbrx_get_gain_state_out_t;

#ifndef FEATURE_D3925_FED
/*==============================================================================
  For rflm_fbrx_get_run_decision
==============================================================================*/
/*! @brief  Input parameters for rflm_fbrx_get_run_decision API */
typedef struct
{
  int8 pa_state;
  int16 row_index;
  int16 tx_agc;
} rflm_fbrx_get_run_decision_in_t;

/*! @brief  Output parameters for rflm_fbrx_get_run_decision API */
typedef struct
{
  boolean run_fbrx;
  boolean do_loft;
  rflm_fbrx_run_method_e run_method;  //< For reporting only
} rflm_fbrx_get_run_decision_out_t;
#endif

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
  FBRx API's prototypes
==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns FBRx expected gain.

  @details
  This API calculates the FBRx expected gain, which depends on tx power and
  temperature.

  @param *in    : pointer to input data structure. 
  @param *out   : pointer to data structure where results will be returned.

  @retval Returns an error status

===========================================================================*/
extern rflm_err_t rflm_fbrx_get_exp_gain( rflm_handle_tx_t handle,
                                          rflm_fbrx_get_exp_gain_in_t  *in,
                                          rflm_fbrx_get_exp_gain_out_t *out);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Performs arbitration for FBRx.

  @details
  This API returns TRUE if a given tech/chain has permission to use the FBRx
  resource.

  @param *in    : pointer to input data structure. 
  @param *out   : pointer to data structure where results will be returned.

  @retval Returns an error status

===========================================================================*/
extern rflm_err_t rflm_fbrx_arbitrator( rflm_handle_tx_t handle,
                                        rflm_fbrx_arbitrator_in_t  *in,
                                        rflm_fbrx_arbitrator_out_t *out );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Decides if FBRx should be run for a given TxAGC iteration.

  @details

  @param *in    : pointer to input data structure. 
  @param *out   : pointer to data structure where results will be returned.

  @retval Returns an error status

===========================================================================*/
extern rflm_err_t rflm_fbrx_get_run_decision( rflm_handle_tx_t handle,
                                              rflm_fbrx_get_run_decision_in_t  *in,
                                              rflm_fbrx_get_run_decision_out_t *out );


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configures chain for FBRx.

  @details

  @retval TRUE if success, FALSE if failure

===========================================================================*/
extern boolean rflm_fbrx_configure_chain( rflm_handle_rx_t handle );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Activates chain for FBRx.

  @details


===========================================================================*/
extern void rflm_fbrx_activate_chain( rflm_handle_rx_t handle );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configures bridge for FBRx.

  @details


===========================================================================*/
extern void rflm_fbrx_bridge_config( boolean enable );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Decides the FBRx gain state for a given TxAGC iteration and the FBRX gain switch points.

  @details

  @param *in    : pointer to input data structure. 
  @param *out   : pointer to data structure where results will be returned.

  @retval Returns an error status

===========================================================================*/
extern rflm_err_t rflm_fbrx_get_gain_state( rflm_handle_tx_t handle,
                                            rflm_fbrx_get_gain_state_in_t  *in,
                                            rflm_fbrx_get_gain_state_out_t *out );

/*----------------------------------------------------------------------------*/
/*! 

  @brief
  Updates the droop cal RX filter settings from FBRx DM for current fbrx mode 

  @param rflm_handle_tx_t : Tx Handle 
  @param grp_delay        : pointer to update the rx group delay 
  @param filter_coeff     : pointer to update the rx filter coefficients
  @param out_mode         : pointer to update the rx filter mode (real or complex)
  @param out_num_rx_taps  : pointer to update the number of rx filter taps 
 
  @retval Returns an error status

===========================================================================*/
extern rflm_err_t rflm_fbrx_update_droop_settings( rflm_handle_tx_t tx_handle, 
                                                   uint8 *out_grp_delay, 
                                                   uint32 *out_filter_coeff,
                                                   rflm_fbrx_rx_filter_mode_t *out_mode,
                                                   uint16 *out_num_rx_taps);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update the Tx Lin LUT with FBRx error correction value for a given TxAGC iteration.

  @details

  @param rflm_handle_tx_t  : Tx Handle 

  @retval Returns an error status

===========================================================================*/
extern rflm_err_t rflm_fbrx_update_tx_smps_pwr ( rflm_handle_tx_t handle_id);
extern rflm_err_t rflm_fbrx_update_tx_linearizer ( rflm_handle_tx_t handle_id);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the fbrx dm template data.

  @details

  @param fbrx_template_ptr  : pointer to the fbrx dm template

  @retval void

===========================================================================*/
extern void rflm_fbrx_init_mem( rflm_fbrx_dm_template_t *fbrx_template_ptr );
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Allocate and associate script memory for unique fbrx handle and event id.

  @details

  @param fbrx_handle  : fbrx lm handle
  @param event_id  : fbrx ccs event id

  @retval void* : pointer to the allocated script memory

===========================================================================*/
extern void* rflm_fbrx_allocate_get_script_buf_ptr
(
  rflm_handle_tx_t             fbrx_handle,
  rflm_fbrx_ccs_event_type_e   event_id
);
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the fbrx dm template data.

  @details

  @param tx_handle  : tx handle
  @param fbrx_handle  : fbrx lm handle

  @retval : true or false

===========================================================================*/ 
extern boolean rflm_fbrx_populate_ccs_event_settings
( 
  rflm_handle_tx_t tx_handle, 
  rflm_handle_tx_t fbrx_handle 
); 


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Schedule WTR and TXC readback actions 

  @param handle  : tx handle

  @retval : rflm_err_t
 
===========================================================================*/ 
extern rflm_err_t rflm_fbrx_schedule_readback_actions(rflm_handle_tx_t handle);


/*----------------------------------------------------------------------------*/
extern void rflm_fbrx_set_gps_state(  rflm_fbrx_gps_state_type gps_state );
/*!
  @brief
  Configures parameters required for FBRX-based ILPC

  @param handle_id : tx handle
  @param tq_handle : TQ handle
  @param ccs_mem_addr : Start of CCS memory available for READBACK event
  @param ccs_script_size : Size of CCS READBACK event

  @retval Returns an error status

===========================================================================*/
extern rflm_err_t rflm_fbrx_ilpc_config_event_param
(
  uint32 handle_id,
  rflm_ccs_rf_tq_handle_t* tq_handle,
  uint32* ccs_mem_addr,
  uint32 ccs_script_size
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Flushes the FBRx log
  @param handle  : tx handle
  @retval : rflm_err_t
 
===========================================================================*/ 
extern rflm_err_t rflm_fbrx_flush_log(rflm_handle_tx_t handle);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Flags that the FBRx ILPC data has gone stale
  @param handle  : tx handle
  @retval : rflm_err_t
 
===========================================================================*/ 
extern rflm_err_t rflm_fbrx_ilpc_set_stale_data_flag(rflm_handle_tx_t handle);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the tx error accum
  @param handle  : tx handle
  @retval : rflm_err_t 
===========================================================================*/ 
extern rflm_err_t rflm_fbrx_ilpc_set_lut_rvt(rflm_handle_tx_t handle);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update lut_rvt and tx error accum during temp comp

  @param handle_id : tx handle

  @retval Returns an error status

===========================================================================*/
rflm_err_t rflm_fbrx_update_temp_comp_ind(rflm_handle_tx_t handle, boolean temp_comp_ind);

#if 0
extern atomic_word_t rflm_fbrx_wtr_in_use;
STATIC_INLINE boolean rflm_fbrx_try_lock_wtr(void)
{
  return (atomic_compare_and_set(&rflm_fbrx_wtr_in_use, (atomic_plain_word_t) FALSE, (atomic_plain_word_t) TRUE));
}

STATIC_INLINE boolean rflm_fbrx_free_lock_wtr(void)
{
  return (atomic_compare_and_set(&rflm_fbrx_wtr_in_use, (atomic_plain_word_t) TRUE, (atomic_plain_word_t) FALSE));
}
#endif
#ifdef __cplusplus
} // extern "C"
#endif


#endif /* RFLM_API_FBRX_H */
