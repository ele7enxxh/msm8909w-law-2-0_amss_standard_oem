/*!
  @file
  lte_LL1_ue_types.h

  @brief
  lte LL1 ue types

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_ue_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef LTE_LL1_UE_TYPES_H
#define LTE_LL1_UE_TYPES_H

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/


/*===========================================================================

      Typedefs

===========================================================================*/

/*! @brief  enum of UE RX modules for use in disable & enable mask */
typedef enum
{
  LTE_LL1_UE_MOD_UNUSED             = 0x01,     ///< was system time, now unused
  LTE_LL1_UE_MOD_SAMP_REC           = 0x02,     ///< sample ram recording
  LTE_LL1_UE_MOD_MSTMR_WB_SYNC      = 0x04,     ///< mstmr to WB counter synchronization
  LTE_LL1_UE_MOD_AGC                = 0x08,     ///< rx-AGC
  LTE_LL1_UE_MOD_FTL                = 0x10,     ///< frequency tracking loop
  LTE_LL1_UE_MOD_TTL                = 0x20,     ///< time tracking loop
  LTE_LL1_UE_MOD_CH_EST             = 0x40,     ///< channel estimation (and demfront)
  LTE_LL1_UE_MOD_FW_ANT_CLK_CFG     = 0x80,     ///< FW ant clk cfg val
  LTE_LL1_UE_MOD_SAMP_REC_SECONDARY = 0x100,    ///< Use secondary LTE sample server for sample recording
  LTE_LL1_UE_MOD_GAP                = 0x200,    ///< enable/disable for gap enter/exit, for L2x, L2L, L2nothing
  LTE_LL1_UE_MOD_TDD_RXON_TOGGLE    = 0x400,    ///< enable = Enable GRFC toggle in TDD, disable = stop GRFC toggle in TDD
  LTE_LL1_UE_MOD_COM                = 0x800,    ///< enable/disable COM loop
  LTE_LL1_UE_MOD_LIGHT_SLEEP        = 0x2000    ///< enable light sleep
} lte_LL1_ue_mod_e;

/*! @brief enum of UE demfront enable modules. */
typedef enum
{
  LTE_LL1_UE_DF_ENABLE_MODE_NORMAL = 0,         ///< Normal reset/resume enable: fresh start, wakeup, gap exit, etc.
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  LTE_LL1_UE_DF_ENABLE_MODE_AS     = 1,         ///< Antenna Switch enable: back-to-back demfront disable and enable. 
#endif
  LTE_LL1_UE_DF_ENABLE_MODE_OS        = 2,         ///< Optimized Switch enable for PMCH.

  LTE_LL1_UE_DF_ENABLE_MODE_RF_RETUNE = 3          ///< One ms gap to Retune. Reset Coeherent 
                                                   ///  estimate and resume the non-coehernt 
                                                   ///  estimate
} lte_LL1_ue_df_enable_mode_e;

/*! @brief  enum of resumeable UE RX modules.  */
typedef enum
{
  LTE_LL1_UE_RESUME_UNUSED       = 0x01,
  LTE_LL1_UE_RESUME_MOD_AGC      = 0x08,
  LTE_LL1_UE_RESUME_MOD_FTL      = 0x10,
  LTE_LL1_UE_RESUME_MOD_TTL      = 0x20,
  LTE_LL1_UE_RESUME_MOD_CH_EST   = 0x40,
  LTE_LL1_UE_RESUME_MOD_COM      = 0x800,
  LTE_LL1_UE_RESUME_MOD_DOPPLER  = 0x1000
} lte_LL1_ue_resume_mod_e;

/*! @brief  enum of frozen updates for UE RX modules. Freeze means freeze HW updates only */
typedef enum
{
  LTE_LL1_UE_FREEZE_UPDATE_MOD_AGC      = 0x08,   ///< Freeze LNA updates/ outer loop LNA
  LTE_LL1_UE_FREEZE_TDD_RXON_TOGGLE     = 0x400,  ///< Freeze GRFC toggling in FW, .
} lte_LL1_ue_freeze_update_mod_e;

#define LTE_LL1_VALID_FREEZE_UPDATE_MASK ( LTE_LL1_UE_FREEZE_UPDATE_MOD_AGC \
                                          | LTE_LL1_UE_FREEZE_TDD_RXON_TOGGLE)

/*! @brief  enum LTE app resource modes.  */
typedef enum
{
  LTE_LL1_UE_RES_MODE_NO_LTE,               ///< NO resources for LTE  
  LTE_LL1_UE_RES_MODE_LTE_IDLE,             ///< Just enough resources for just running STMR
  LTE_LL1_UE_RES_MODE_LTE_IRAT,             ///< For allowing other RAT to do measurement  
  LTE_LL1_UE_RES_MODE_LTE_SRCH_ONLY,        ///< Just enough resources to do LTE search   
  LTE_LL1_UE_RES_MODE_FULL_LTE              ///< Resources for fully operational LTE
} lte_LL1_ue_res_mode_e;

typedef enum
{
  LTE_LL1_UE_MODE_SYS_NO_CHANGE = 0,
  LTE_LL1_UE_MODE_SYS_START,
  LTE_LL1_UE_MODE_SYS_WAKEUP,
  LTE_LL1_UE_MODE_SYS_STOP
}lte_LL1_ue_mode_sys_e;

/*! @brief  enum LTE app processing modes.  */
typedef enum
{
  LTE_LL1_UE_PROC_MODE_ON_LINE,             ///< on-line: Process 1 symbol at a time
                                            ///<  as they are stored in sample buffer
  LTE_LL1_UE_PROC_MODE_ON_LINE_INIT_ACQ_PBCH,	//indicates UE is in init-acq state
  LTE_LL1_UE_PROC_MODE_ON_LINE_FSCAN,			//indicates UE is in freq scan state
  LTE_LL1_UE_PROC_MODE_ON_LINE_TRAFFIC,			//indicates UE is in connected state
  LTE_LL1_UE_PROC_MODE_ON_LINE_IRAT,                    //indicates UE is in IRAT state                                                                                                                
  LTE_LL1_UE_PROC_MODE_OFF_LINE,            ///< off-line: Store ~1m of samples, then
                                            ///<  process without collecting more samples
} lte_LL1_ue_proc_mode_e;

/*! @brief Wideband demfront operation mode enum. 
           Note: Different modes may require different clock/voltage settings.
*/
typedef enum {
  LTE_LL1_UE_WB_DF_POWER_SAVING_MODE,
  LTE_LL1_UE_WB_DF_FULL_MODE,
  LTE_LL1_UE_WB_DF_MODE_COUNT
} lte_LL1_ue_wb_df_mode_e;

/*! @brief  enum LTE gap types.  */
typedef enum{
  LTE_LL1_GAP_TYPE_NO_GAP,          ///< Indicates we are in lte connected state or l2l intra idle
  LTE_LL1_GAP_TYPE_X2L_INTER_IDLE,  ///< Indicates an X2L and L2L inter idle type of GAP
  LTE_LL1_GAP_TYPE_X2L_CONN,        ///< Indicates an X2L and L2L inter compressed type of GAP
  LTE_LL1_GAP_TYPE_WB_PROCESSING,   ///< Indicates an inter-frequency WB processing gap
  LTE_LL1_GAP_MAX_NUM_GAP_TYPES,    ///< total number of gap configurations
}lte_LL1_gap_type_e;

/*! @brief  enum LTE PING PONG sample buffer */
typedef enum{
  LTE_LL1_GAP_SAMPLE_BUFFER_PING = 0,
  LTE_LL1_GAP_SAMPLE_BUFFER_PONG = 1,
  LTE_LL1_GAP_NUM_SAMPLE_BUF
}lte_LL1_gap_sample_buff_select_e;

/*! @brief  enum of RX-AGC modes.  */
typedef enum
{
  LTE_LL1_RXAGC_MODE_FREQ_SCAN,
  LTE_LL1_RXAGC_MODE_ACQ,  //  <-- FDD only
  LTE_LL1_RXAGC_MODE_TRAFFIC,
  LTE_LL1_RXAGC_MODE_PBCH, // TDD Initial PBCH only
  LTE_LL1_RXAGC_MODE_INTER_FREQ,
  LTE_LL1_RXAGC_MODE_ONL_DRX,
  LTE_LL1_RXAGC_MODE_OFFLINE_DRX,
  LTE_LL1_RXAGC_MODE_X2L_IDLE, //Inter-RAT/Inter-Freq IDLE mode
  LTE_LL1_RXAGC_MODE_X2L_CONN, //Inter-RAT/Inter-Freq connected mode 
  LTE_LL1_RXAGC_MODE_TDD_SRCH_PIPELINE,     // X2L & Initial ACQ Pipeline AGC mode
  LTE_LL1_RXAGC_MODE_X2L_TDD_MEAS_LNA, // X2L Fixed LNa mode, REuse LTE_LL1_RXAGC_MODE_X2L_IDLE ?
  LTE_LL1_RXAGC_MODE_X2L_TDD_MEAS_VCELLTIME, // X2L Fixed LNa mode, Use VCELL Time to compute Gain State  
  LTE_LL1_RXAGC_MODE_DEFAULT=15
} lte_LL1_rxagc_mode_e;


typedef enum
{
  LTE_LL1_UE_INPUT_MODE_RF       = 0,       ///< RF input
  LTE_LL1_UE_INPUT_MODE_SURF_DIO = 1,       ///< digital I/Q on Surf
  LTE_LL1_UE_INPUT_MODE_RUMI_DIO = 2,       ///< digital I/Q on Rumi
  LTE_LL1_UE_INPUT_MODE_SIM      = 3,       ///< Simulation platform
  LTE_LL1_UE_INPUT_MAX_NUM_MODE
} lte_LL1_ue_input_mode_e;


typedef enum
{
  LTE_LL1_UE_WB_SAMP_RATE_30_72   = 0,      ///< 30.72 MHz sampling rate
  LTE_LL1_UE_WB_SAMP_RATE_15_36   = 1,      ///< 15.36 MHz sampling rate
  LTE_LL1_UE_WB_SAMP_RATE_7_68    = 2,      ///<  7.68 MHz sampling rate
  LTE_LL1_UE_WB_SAMP_MAX_NUM_RATE = 3       ///<  Mazimum number of supported rates
} lte_LL1_ue_wb_samp_rate_e;


typedef enum
{
  LTE_LL1_UE_WB_FFT_SIZE_2048 = 0,     ///< FFT_size 2048
  LTE_LL1_UE_WB_FFT_SIZE_1024 = 1,     ///< FFT_size 1024
  LTE_LL1_UE_WB_FFT_SIZE_512  = 2,     ///< FFT_size 512
  LTE_LL1_UE_WB_FFT_SIZE_MAX  = 3      ///< Mazimum number of supported WB FFT size
} lte_LL1_ue_wb_fft_size_e;


typedef enum
{
  LTE_LL1_UE_OUTPUT_MODE_RF       = 0,      ///< RF input
  LTE_LL1_UE_OUTPUT_MODE_SURF_DIO = 1,      ///< digital I/Q on Surf
} lte_LL1_ue_output_mode_e;

typedef struct
{
  int32 dvga_dB;
  int32 tot_rssi_accum_dB;
  uint8 lna_state_index;
  uint8 rxfft_wola_len;
  int8  rxfft_deficit_bias;
} lte_LL1_rxagc_start_params_s;

/*! @brief: Type for the shared mem source for RXAGC params */
typedef enum {
  LTE_LL1_RXAGC_SHARED_MEM_STANDALONE=0,
  LTE_LL1_RXAGC_SHARED_MEM_NCELL=1,
  LTE_LL1_RXAGC_SHARED_MEM_MAX_COUNT=2
} lte_LL1_rxagc_shared_mem_source_e;

#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_DVGA_DB              0
/*!@brief: default dvga values for cold start based on RSSI table,all in Q24.*/
#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_DVGA_DB_1_4MHZ         -763834507 
#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_DVGA_DB_3MHZ           -697140614  
#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_DVGA_DB_5MHZ           -659950209 
#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_DVGA_DB_10MHZ          -659950209 
#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_DVGA_DB_15MHZ          -680850310 
#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_DVGA_DB_20MHZ          -659950209 


#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_TOT_RSSI_ACCUM_DB    (-50<<24) 
  //Assume RSSI=-50dBm for a value in LNA state G2 range
#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_LNA_STATE_INDEX      2
#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_RXFFT_WOLA_LEN       4
#define LTE_LL1_RXAGC_START_PARAMS_DEFAULT_RXFFT_DEFICIT_BIAS   2

// agc settling time in ms
#define LTE_LL1_RXAGC_MODE_X2L_IDLE_SETTLING_DUR_MS               2
#define LTE_LL1_RXAGC_MODE_X2L_CONN_SETTLING_DUR_MS               0
#define LTE_LL1_RXAGC_MODE_TDD_SRCH_PIPELINE_SETTLING_DUR_MS      7
#define LTE_LL1_RXAGC_MODE_X2L_TDD_MEAS_VCELLTIME_SETTLING_DUR_MS 8
#define LTE_LL1_RXAGC_MODE_X2L_TDD_MEAS_LNA_SETTLING_DUR_MS       0

/*! @brief  enum other techs, use one of these tech's stmr to restore LTE o_stmr.  */

typedef enum
{
  LTE_LL1_IRAT_TIME_WCDMA,
  LTE_LL1_IRAT_TIME_GERAN,
  LTE_LL1_IRAT_TIME_DO,
  LTE_LL1_IRAT_TIME_CDMA1X,
  LTE_LL1_IRAT_TIME_NONE
} lte_LL1_irat_time_e;

/*! @brief List of LTE sample servers */
typedef enum
{
  LTE_LL1_SAMP_SERV_PRIMARY,
  LTE_LL1_SAMP_SERV_SECONDARY_PING,
  LTE_LL1_SAMP_SERV_SECONDARY = LTE_LL1_SAMP_SERV_SECONDARY_PING,
  LTE_LL1_SAMP_SERV_SECONDARY_PONG,
  LTE_LL1_SAMP_SERV_NUM_BUF
} lte_LL1_samp_serv_e;

/*! @brief LTE sample recording related information */
typedef struct
{
  lte_LL1_samp_serv_e       start_rec_buf;            ///< Which buffer to start recording into

  lte_LL1_samp_serv_e       stop_rec_buf;             ///< Which buffer to stop recording

  uint16                    start_rec_offset_Ts;      ///< sample recording start offset from DL subframe boundary
                                                      ///< This is in units of Ts.
                                                      ///< Range = 0 .. 30719

  uint16                    stop_rec_offset_Ts;       ///< sample recording stop offset from DL subframe boundary
                                                      ///< This is in units of Ts.
                                                      ///< Ranga = 0 .. 30719

  uint16                    samp_rec_time_ms;         ///< Duration of recording in secondary LTE sample server (in ms)
                                                      ///< To stop sample recording samp_rec_time_ms after sample recording start
                                                      ///<   * To be done without another config app request.
                                                      ///< Sample recording will be stopped after config app CNF is sent
                                                      ///< 0: Indicates that samp_rec_time_ms is don't care.
                                                      ///<    Continue recording until told to stop using another config app request

  uint16                    samp_rec_time_frac_Ts;    ///< @todo NEEDED ONLY IF samp_rec_time_ms IS NOT AN INTEGER
                                                      ///< In order to support x.y ms of recording where,
                                                      ///< x is the integer part; y is the fractional part
                                                      ///< y = samp_rec_time_frac (in Ts)
                                                      ///< Range = 0 .. 30719

  uint16                     start_rec_delay_ms;      ///< delaying regular SS start by start_rec_delay_Ts msec
                                                      ///< not to be used in conjunction of fractional SF operation
                                                      ///< to assert if samp_rec_time_ms != 0

} lte_LL1_samp_serv_s;

typedef struct
{
  /*! MSTMR reference time of the subframe on which the timing snapshot
    was captured */
  int64 subframe_ref_time;
  /*! The o_stmr value of the snapshot, LTE stmr, 22-bit binary, 30.72 MHz */
  int32 o_stmr;
  /*! ustmr value for snapshot */
  int32 u_stmr;
  /*! offset of snapshot timing from SF boundary */
  int32 snapshot_offset;    
  /*! Frame number of the snapshot */
  int32 sfn;
  /*! Subframe number of the snapshot */
  int32 subfn;
} lte_LL1_lte_time_snapshot_s;

/*! @brief this is the stmr snapshot of LTE and other tech;
   FW will do this stmr snapshot, store this info, 
   no need to send this set of data to SW for this purpose, but still send it for SW to map other tech time to LTE time;
   then SW will send timing transfer wakeup cmd;
   then FW will do the stmr snapshot again & math in lte_LL1_wakeup_timing_xfer() to restore LTE time and INT scheduling.
   the sleep/wakeup interval should "not" exceed stmr wrap time: W: 40.96 sec, G: because of frame count, very big. DO: =~ 109 sec

  Reference time snapshot.  This describes an absolute time reference
  between two RATs.  Both RATs must be active at the time of the snapshot

  sfn/subfn/subframe_ref_time
  ____ ______|____________________________|_________________________
                                   ^
             |----snapshot_offset--|
                                   |
                           ostmr/IRAT snapshot
   
*/
typedef struct {
  /*! The lte time snapshot */
  lte_LL1_lte_time_snapshot_s   lte_time;
} lte_LL1_stmr_snapshot_s;

/*! @brief Enum of RX_ON state used in RF chip Wakeup mode */
typedef enum
{
  LTE_LL1_RF_WAKEUP_RX_OFF,         ///< turn off Rx
  LTE_LL1_RF_WAKEUP_RX_ON           ///< turn on Rx
} lte_LL1_rf_wakeup_rx_on_e;

typedef struct
{
  /*! @brief To load EST1 I DC estimate value */
  uint32 est1_load_i;
  /*! @brief To load EST1 Q DC estimate value */
  uint32 est1_load_q;
  /*! @brief To load EST2 I DC estimate value */
  uint32 est2_load_i;
  /*! @brief To load EST2 Q DC estimate value */
  uint32 est2_load_q;
}lte_LL1_dcc_accum_cfg_s;


/*! @brief The DCC DAC table, for each LNA gain stage; 0 to 5 */
typedef struct
{
  lte_LL1_dcc_accum_cfg_s accum[6];
} lte_LL1_dcc_accum_cfg_table_s;


/*! @brief Enum for various resources that can be released/acquired by FW */
typedef enum
{
  LTE_LL1_RESOURCE_UNUSED = 0,
  LTE_LL1_RESOURCE_VPE = 0x01,
  LTE_LL1_RESOURCE_DEMBACK = 0x02,
} lte_LL1_resource_e;

#endif
