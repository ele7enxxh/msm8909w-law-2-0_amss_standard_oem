/*!
  @file
  tfw_sw_intf_msg.h

  @brief
  TD-SCDMA SW Interface definitions

*/

/*=========================================================================== 
 
  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Confidential and Proprietary 

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_tdscdma/api/tfw_sw_intf_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/14   WH      CR/640248,FR 19316: TFW:  SIR base PCH_FET decoding
04/09/14   WH      CR638795, use energy metric and err count to reduce fpach false alarm
04/01/14   WH      FR 19658: TFW: LVA and MTVA
01/29/14   CL      Add fast cell update fields in tfw_dl_dpch_sir_per_cctrch_t
12/15/13   WH      Add FET for tfw_dl_sccpch_config_t
12/07/13   WH      add UMID for tfw_inSync_config_cmd_t
05/08/13   HK      Add UMID for tfw_xpt_capture_cmd_t and tfw_xpt_tx_cmd_t
01/09/13   VK      Add TX HW Block enable/ disable interface support
11/27/12   JP      Merge NikeL CL 2922049 Added sw/fw msg to enable/disable adc clock gating
12/03/12   BL      Add STATE enums for RSB cal support:
                   TFW_STATE_FTM_ACQ = TFW_STATE_FTM, TFW_STATE_FTM_TRK,
                   TFW_STATE_FTM_CX8
08/08/12   BL      Add codeType and DOB start address to dob_status_fifo
07/19/12   JP      Add rsvd field at tfw_hs_dob_status_t
07/03/12   JP      Add rsvd field at tfw_nonHS_dob_status_t
===========================================================================*/
#ifndef TFW_SW_INTF_MSG_H
#define TFW_SW_INTF_MSG_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"
#include "cxm_intf_types.h"

#ifdef __cplusplus
}  // extern "C"
#endif
#include "tfw_const.h"
#include "tfw_sw_intf_srch.h"
#include "tfw_sw_intf_measurement.h"
#include "tfw_sw_intf_cell.h"
#include "tfw_rf_sm_intf.h"
#include "tfw_sw_intf_rx_tx.h"
#include "tfw_sw_intf_irat.h"
#include "tfw_sw_intf_log.h"
#include "tfw_sw_intf_multi_sim.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Message router related define */

/*! \brief The text string for the external TFW Task Queue. */
#define TFW_MSG_Q_NAME              "TFW_Q"

#define MSGR_MODULE_FW                0x00
// #define MSGR_FW_MAX_MODULES              1

/*! @brief TFW module identifier for message header */
#define MSGR_TDSCDMA_FW    MSGR_TECH_MODULE(MSGR_TECH_TDSCDMA, MSGR_MODULE_FW)

                                                                    

/*! \brief UMID for software command messages */
enum
{
  /*! \brief The first message command in the enum */
  TFW_CMD_FIRST = MSGR_UMID_BASE(MSGR_TDSCDMA_FW, MSGR_TYPE_CMD),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, STATE_CFG,                   0x0,  tfw_state_cfg_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, PANIC_STOP,                  0x1,  tfw_panic_stop_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, RX_CALIBRATION,              0x2,  tfw_rx_calibration_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, TX_CALIBRATION,              0x3,  tfw_tx_calibration_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, RX_CONFIG,                   0x4,  tfw_rx_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, TX_CONFIG,                   0x5,  tfw_tx_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, RX_TX_OVERRIDE_CONFIG,       0x6,  tfw_rx_tx_override_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, IQ_CAPTURE,                  0x7,  tfw_iq_capture_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, RX_CONFIG_UPDATE,            0x8,  tfw_rx_config_update_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, FTM_RX,                      0x9,  tfw_ftm_rx_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, FTM_TX,                      0xA,  tfw_ftm_tx_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, IQMC_TEMP_COMPENSATION,      0xB,  tfw_iqmc_temp_compensation_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, ARD,                      	  0xC,  tfw_ard_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, FTM_GET_MULTI_SYNTH_STATE,   0xD,  tfw_ftm_get_multi_synth_state_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, RX_CHAIN_IMM_REL,            0xE,  tfw_rx_chain_imm_rel_cmd_t),

  /** Search Configuration */                                    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, SRCH_POWER_SCAN,             0x10, tfw_srch_power_scan_cmd_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, SRCH_GAP_DETECT,             0x11, tfw_srch_gap_detect_cmd_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, SRCH_SYNCDL_MIDAMBLE_DETECT, 0x12, tfw_srch_syncdl_midamble_detect_cmd_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, SRCH_REACQ,                  0x13, tfw_srch_reacq_cmd_t), 
  /** Measurement Configuration */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, MEASURE_RSCP,                0x20, tfw_measure_rscp_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, MEASURE_ISCP,                0x21, tfw_measure_iscp_cmd_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, MEASURE_ALL_RSCP,            0x22, tfw_measure_all_rscp_cmd_t), 
  /** Cell Configuration */                                       
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, CELL_SERVING_JDCS,           0x30, tfw_cell_serving_jdcs_cmd_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, BATON_HANDOVER,              0x31, tfw_baton_handover_cmd_t), 
  /** DL Channel Configuration */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DL_MIDAMBLE_CONFIG_TABLE,    0x40, tfw_dl_midamble_config_table_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DL_PICH_CONFIG,              0x41, tfw_dl_pich_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DL_PCCPCH_CONFIG,            0x42, tfw_dl_pccpch_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DL_SCCPCH_CONFIG,            0x43, tfw_dl_sccpch_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DL_DPCH_CONFIG,              0x44, tfw_dl_dpch_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DL_HS_CONFIG,                0x45, tfw_dl_hs_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, FPACH_CONFIG,                0x46, tfw_fpach_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, EAGCH_CONFIG,                0x47, tfw_eagch_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, EHICH_CONFIG,                0x48, tfw_ehich_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, NON_SCHLD_GRANT_CONFIG,      0x49, tfw_non_schld_grant_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, RX_TIME_SYNC,                0x4A, tfw_rx_time_sync_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, SLOT_SUSPENSION_CONFIG,      0x4B, tfw_slot_suspension_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, L1_INSYNC_CONFIG,      	  0x4C, tfw_inSync_config_cmd_t),
  /** UL Channel Configuration */                               
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, UPPCH_TX_PWR_TIMING_CONFIG,  0x50, tfw_uppch_tx_pwr_timing_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, PRACH_CONFIG,                0x51, tfw_prach_config_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DPCH_TX_PWR_TIMING_CONFIG,   0x52, tfw_dpch_tx_pwr_timing_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, UL_DPCH_CONFIG,              0x53, tfw_ul_dpch_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, ERUCCH_CONFIG,               0x54, tfw_erucch_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, EUL_CONFIG,                  0x55, tfw_eul_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, EPUCH_CONFIG,                0x56, tfw_epuch_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, UL_DPCH_CANCEL,              0x57, tfw_ul_dpch_cancel_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DPCH_TX_PWR_TIMING_UPDATE,   0x58, tfw_dpch_tx_pwr_timing_update_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, HSSICH_OVERRIDE,             0x59, tfw_hssich_override_cmd_t),
  /** Power Control Related Configuration */                    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DL_DPCH_TARGETSIR_UPDATE,    0x60, tfw_dl_dpch_targetSIR_update_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, HSSCCH_TARGETSIR_UPDATE,     0x61, tfw_hsscch_targetSIR_update_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, EAGCH_TARGETSIR_UPDATE,      0x62, tfw_eagch_targetSIR_update_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, PATHLOSS_UPDATE,             0x63, tfw_pathloss_update_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, HSPDSCH_SIR_UPDATE,          0x66, tfw_hspdsch_sir_update_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, ADC_CLOCK_GATING_CONFIG,     0x67, tfw_adc_clock_gating_config_cmd_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, HSDPA_FLOW_CONTORL,          0x68, tfw_hsdpa_flow_control_cmd_t), 
  /** Antenna switching */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, ANTENNA_SWITCHING_CONFIG,    0x6A, tfw_antenna_switching_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, AOL_ABORT_CONFIG,            0x6B, tfw_aol_abort_config_cmd_t),
  /** DL Demback Configuration */                               
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DL_TRCH_UPDATE,              0x70, tfw_dl_trCh_update_cmd_t),    
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DL_CCTRCH_CONFIG,            0x71, tfw_dl_ccTrCh_config_cmd_t),   
  /** XPT commands */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, XPT_CAPTURE,                 0x7A,  tfw_xpt_capture_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, XPT_TX,                      0x7B,  tfw_xpt_tx_cmd_t), 
  /** IRAT configuration */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, IRAT_GAP_DETECT,             0x80, tfw_irat_gap_detect_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, IRAT_SYNCDL_MIDAMBLE_DETECT, 0x81, tfw_irat_syncdl_midamble_detect_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, IRAT_MEASURE_RSCP,           0x82, tfw_irat_measure_rscp_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, IRAT_T2X_GAP_CFG,            0x83, tfw_irat_t2x_gap_cfg_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, IRAT_T2X_CLEANUP,            0x84, tfw_irat_t2x_cleanup_cmd_t),
  
 /** Multi Sim DSDx related configuration */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, MULTI_SIM_DSDS_QTA_GAP,      0x85, tfw_multi_sim_dsds_qta_gap_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, MULTI_SIM_CONFIG,            0x86, tfw_multi_sim_config_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, WLAN_COEX_POLICY,            0x87, tfw_wlan_coex_policy_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, WLAN_COEX_METRIC_CTR,        0x88, tfw_wlan_coex_mertic_ctr_cmd_t),
    
  /** Diag log configuration */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, DIAG_LOG_MASK,               0x90, tfw_diag_log_mask_cmd_t),
  /** Tx mempool logging configuration */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, TX_MP_LOG_CONFIG,            0x91, tfw_tx_mp_log_config_cmd_t),
  /** New JDS management interface */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, TS0_CELL_UPDT,               0x92, tfw_ts0_cell_updt_cmd_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, NONTS0_CELL_UPDT,            0x93, tfw_nonts0_cell_updt_cmd_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, SERVING_CELL_UPDT,           0x94, tfw_serving_cell_updt_cmd_t),
 /** TX APT message */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, TX_APT_CONFIG,               0x95, tfw_tx_apt_config_cmd_t),
  /** TX HW Block Enabled/ Disabled Status Interface */
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMD, TX_HW_BLOCK_CONFIG,          0x96, tfw_tx_hw_block_config_cmd_t),
 /*! \brief The last command message in the enum */                    
  TFW_CMD_LAST = TDSCDMA_FW_CMD_TX_HW_BLOCK_CONFIG,
  MSGR_DEFINE_UMID(TDSCDMA, FW, CMDI, DUMMY,    0x20, NULL)
};


/*! \brief Indications/Updates the TD-SCDMA FW module sends. */
// Payload input for documentation only.          
enum
{
  TFW_IND_FIRST = MSGR_UMID_BASE(MSGR_TDSCDMA_FW, MSGR_TYPE_IND),
  MSGR_DEFINE_UMID(TDSCDMA, FW, IND, UL_TPC_UDT,                  0x0,  tfw_ul_tpc_update_ind_t),
  /** Cell Indication */
  MSGR_DEFINE_UMID(TDSCDMA, FW, IND, CELL_INFO,                   0x1,  tfw_cell_info_ind_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, IND, ERROR_DETECTED,              0x2,  tfw_error_detected_ind_t),
  
  MSGR_DEFINE_UMID(TDSCDMA, FW, IND, ARD,                         0x3,  tfw_ard_ind_t),
  
  /* This is the message TFW will send to TL1 after executing the RF cleanup scripts*/
  MSGR_DEFINE_UMID(TDSCDMA, FW, IND, DSDS_CLEANUP,                0x4,  tfw_dsds_cleanup_ind_t), 
    
  /* This is the error message TFW will send to TL1 if wlan coex 'TX blank' command is not honored*/
  MSGR_DEFINE_UMID(TDSCDMA, FW, IND, WLAN_TXFRMDNL,               0x5,  tfw_wlan_txfrmdnl_ind_t),
    
  TFW_IND_LAST = TDSCDMA_FW_IND_ERROR_DETECTED 
};


/*! \brief Responses to commands from the TD-SCDMA FW module */
enum
{
  TFW_RSP_FIRST = MSGR_UMID_BASE(MSGR_TDSCDMA_FW, MSGR_TYPE_RSP),
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, STATE_CFG,                   0x0,  tfw_state_cfg_rsp_t),
  /** Search Response */                                          
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, SRCH_POWER_SCAN,             0x10, tfw_srch_power_scan_rsp_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, SRCH_GAP_DETECT,             0x11, tfw_srch_gap_detect_rsp_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, SRCH_SYNCDL_MIDAMBLE_DETECT, 0x12, tfw_srch_syncdl_midamble_detect_rsp_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, SRCH_REACQ,                  0x13, tfw_srch_reacq_rsp_t), 
  /** Measurement Response */
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, MEASURE_RSCP,                0x20, tfw_measure_rscp_rsp_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, MEASURE_ISCP,                0x21, tfw_measure_iscp_rsp_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, MEASURE_ALL_RSCP,            0x22, tfw_measure_all_rscp_rsp_t), 
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, ANTENNA_SWITCHING_CONFIG,    0x27, tfw_antenna_switching_config_rsp_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, IQ_CAPTURE,                  0x30, tfw_iq_capture_rsp_t),

  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, BATON_HANDOVER,              0x31, tfw_baton_handover_rsp_t),
  /*New JDS management interface*/
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, TS0_CELL,                    0x32, tfw_ts0_cell_rsp_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, NONTS0_CELL,                 0x33, tfw_nonts0_cell_rsp_t),
  /** IRAT Response */
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, IRAT_T2X_CLEANUP,            0x40, tfw_irat_t2x_cleanup_rsp_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, FTM_RX_RSP,                  0x50, tfw_ftm_rx_cmd_rsp_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, FTM_TX_RSP,                  0x51, tfw_ftm_tx_cmd_rsp_t),
  MSGR_DEFINE_UMID(TDSCDMA, FW, RSP, FTM_GET_MULTI_SYNTH_STATE,   0x52, tfw_ftm_get_multi_synth_state_rsp_t),  
  /*! \brief The last response message in the enum */                   
  TFW_RSP_LAST = TDSCDMA_FW_RSP_FTM_GET_MULTI_SYNTH_STATE
};



/** DL Per Physical Channel Format Info.
    It constains both slot format and CCTrCH index this physical channel is
    assigned to. */
typedef struct
{
  /** Slot format for this code channel. */
  /** Valid range [0..25]. 
   \li 0: invalid format, unused entry should be initialized with 0
   \li [1..25]: 1st to 25th entry in slot format table  
   */ 
  uint8  slotFormatIdx : 5;
  /*! \brief CCTrCH that this code channel is assigned to, valid : SCCPCH, DPCH 
    for DPCH, valid [0..2], for SCCPCH valid [0], Note the cctrch with index
    0 is mapped to use tfw_ccTrCh_table_t index 0, similarly 1 is
    mapped to use with tfw_ccTrCh_table_t index 1, 2 is mapped to use with
    tfw_ccTrCh_table_t index 2.
 */
  uint8  ccTrchIndex : 3;
} tfw_dl_phyChan_format_info_t;

/** DL Per Slot Physical Channel Info. */
typedef struct
{
  /** Slot format for each of the 16 code channel in this time slot.
      Entry 0 corresponds to the lowest code channel in the slot */
  tfw_dl_phyChan_format_info_t  phyInfo[16];
  /** Total number of code channels that carry TPC.
      = 1+ Ntpc if a Time slot carries TPC, else = 0. Valid [0..16] */
  uint32  NumChanWithTPC;
} tfw_dl_slotFormat_info_t ;

/** DL Non HS channel common info. */
typedef struct
{           
  /** Time slot physical info for non HS channels.
      Up to 6 DL slots   */
  tfw_dl_slotFormat_info_t   slotInfo[6];
} tfw_dl_nonHS_chan_common_info_t ;

/** Midamble configuration per DL time slot */
typedef struct
{
  /** Midamble allocation mode. When this slot is not assigned, set all
      fields to 0 */
  /** Valid value : 
      \li TFW_MIDAMBLE_ALLOC_DEFAULT
      \li TFW_MIDAMBLE_ALLOC_COMMON
      \li TFW_MIDAMBLE_ALLOC_UE_SPECIFIC
      */
  uint16 allocMode : 2;
  /*! \brief Midamble Config Number. Valid : [2,4,6,8,10,12,14,16]*/
  uint16 configK : 5;
  uint16 : 1;
  /*! \brief Midamble shift for UE Specific allocation. Valid : 0..(configK-1)
      Valid only in UE Specific allocation. Otherwise, it should be set to 0*/
  uint16 shift : 8;
} tfw_midamble_config_t ;

/** DL Midamble Configuraiton Table */

/* Channel Number for midmable config */
/* Single Slot Channel */
#define TFW_MIDAMBLE_CFG_PCCPCH               0
#define TFW_MIDAMBLE_CFG_PICH                 1
#define TFW_MIDAMBLE_CFG_NON_SCHLD_EHICH      2
#define TFW_MIDAMBLE_CFG_SCCPCH               3
#define TFW_MIDAMBLE_CFG_S_SLOT_LAST_CHAN     TFW_MIDAMBLE_CFG_SCCPCH
/* Multi-slot Channel */
#define TFW_MIDAMBLE_CFG_DPCH                 4
#define TFW_MIDAMBLE_CFG_M_SLOT_1ST_CHAN      TFW_MIDAMBLE_CFG_DPCH
#define TFW_MIDAMBLE_CFG_HSSCCH               5
#define TFW_MIDAMBLE_CFG_EAGCH                6
#define TFW_MIDAMBLE_CFG_EHICH                7
#define TFW_MIDAMBLE_CFG_M_SLOT_LAST_CHAN     TFW_MIDAMBLE_CFG_EHICH

/* number of channels that are of single slot */
#define TFW_MIDAMBLE_CFG_S_SLOT_CHAN_NUM  (TFW_MIDAMBLE_CFG_S_SLOT_LAST_CHAN + 1)
/* number of channels that are ofmultiple slots */
#define TFW_MIDAMBLE_CFG_M_SLOT_CHAN_NUM  (TFW_MIDAMBLE_CFG_M_SLOT_LAST_CHAN - TFW_MIDAMBLE_CFG_M_SLOT_1ST_CHAN + 1) 

typedef struct
{
  /*! \brief Midamble configuration. Up to 6 DL slots. When a slot is not used,
  the corresponding configuration shall be set to 0 to indicate "un-used" */
  tfw_midamble_config_t   slot[6];
  /*! \brief Channel Type for this configuration.  */
  /** Valid value : 
      \li 0 : TFW_MIDAMBLE_CFG_PCCPCH
      \li 1 : TFW_MIDAMBLE_CFG_PICH
      \li 2 : TFW_MIDAMBLE_CFG_NON_SCHLD_EHICH
      \li 3 : TFW_MIDAMBLE_CFG_SCCPCH
      \li 4 : TFW_MIDAMBLE_CFG_DPCH
      \li 5 : TFW_MIDAMBLE_CFG_HSSCCH
      \li 6 : TFW_MIDAMBLE_CFG_EAGCH
      \li 7 : TFW_MIDAMBLE_CFG_EHICH
      */
  uint16  chanType;
  /*! \brief Channel Enable or Disable with respect to this midamble configuration.
  Valid [0, 1]. 0 : disable, 1: enable */
  uint16  enable;
} tfw_dl_midamble_config_table_t ;

/** DL PICH Configuration */
typedef struct
{
  /** Action Type.  
    PICH decode is done only once per cmd. FW will report the result regardless 
    of detection of indicator. When command with enable is sent after 1st enable, 
    SW still needs to send the whole set of parameters though they are the same as
    before. SW needs to send disable cmd when PICH is to be torn down after paging 
    indicator is detected */
  /** Valid : 
      \li TFW_CMD_ACTION_DISABLE
      \li TFW_CMD_ACTION_ENABLE
      */
  /* Word 0 */
  uint32  actionType : 8;
  /*! \brief Time slot used by PICH. Valid values (0,2,3,4,5,6) */
  uint32  timeSlot : 8;
  /*! \brief Length (in bit) of one paging indicator, valid values 4, 8, 16 */
  uint32  pagingIndLen : 8;
  /*! \brief starting bit lcoation within subframe for paging indicator , 0..87 */
  uint32  qValue : 8;

  /* Word 1 */
  /*! \brief Time (in subframe) when PICH indicator should be searched */
  uint16  actionTime;         
  /*! \brief SF16 code channels for PICH. Bit 0..15 for code 1..16*/
  uint16  phyChanBitMap;
} tfw_dl_pich_config_t ;

/** DL PCCPCH Concfiguraiton for ACQ blind decoding. */
typedef struct {
  /*! \brief Deep search freq bin step size
  @verbatim 
   Resolution: 1 Hz 
   Recommeded systems configuration: 400
  @endverbatim 
  */ 
  uint16                    deepSrchFreqStepSize;
  /*! \brief Number of deep search sided freq bins
  @verbatim 
   Total freq bins = 2*deepSrchNumSidedFreqBins + 1
   Recommeded systems configuration for init acq: 5
  @endverbatim 
  */ 
  uint16                    deepSrchNumSidedFreqBins;
} tfw_dl_pccpch_blind_tti_freq_config_t; 

/** DL PCCPCH Concfiguraiton. */
typedef struct ALIGN(8)
{
  /* Word 0 */
  /** Action Type. */
  /** Valid : 
      \li TFW_CMD_ACTION_DISABLE
      \li TFW_CMD_ACTION_ENABLE
      \li TFW_CMD_ACTION_RECONFIG
      */
  uint32  actionType : 2;
  /** TSTD Enable. */
  /** Valid : 
      \li TFW_CMD_TSTD_DISABLE
      \li TFW_CMD_TSTD_ENABLE
      */
  uint32  tstdEnable : 1;
  uint32  :1;
  /** PCCPCH operation mode. */
  /** valid range. [0..2]
      \li 0: normal
      \li 1: Phase based TTI detection (not compatible with reconfig)
      \li 2: Blind TTI detection (not compatible with reconfig)
      */
  uint32  ttiDetectMode : 2;
  uint32  :2;
  /*! \brief SW internal CCTrCH index used in DOB, Valid range : ??. Note that
  this is only used in DOB for SW table ID purpose. FW does not modify*/
  uint32  swCcTrChIndex : 8;
  /*! \brief Time (in subframe) when the configuration should be applied. It is
   assumed that TTI will start at the subframe of action time in normal mode. */
  uint32  actionTime : 16; 
  
  /* Word 1 */
  /*! \brief Channel present repetition period. Valid only when ttiDetectMode 
  above is "normal". Valid values: 1, 2,4,8,16,32,64 frames. 1 is used to indicate
  all frames are on */
  uint32  chanOnRepPeriod : 8;
  /*! \brief DRMB storage direction for DRM. 0: symbols written in descending 
    addr order , 1: symbols written in ascending addr order. valid only if
    ttiDetectMode is normal mode. */
  uint32  drmbStorageDir : 1;
  uint32  :7;
  /*! \brief DRMB start address for PCCPCH, in unit of line (128 bytes). Valid
  only if ttiDetectMode is normal mode */
  uint32  drmbStartAddr : 15;
  uint32  :1;

  /* Word 2 & 3 */
  /*! \brief Channel Present on-off bit mask to be applied at action time.  
  Bit[0..(chanOnRepPeriod-1)] with each bit indicates if Physical channel is present in
  the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
  shall be set to 0 when chanOnRepPeriod is less than 64.*/
  uint64  chanPresentMask ALIGN(8);

  /* Word 4 */
  /*! \brief Freq configuration for blind TTI detection */
  tfw_dl_pccpch_blind_tti_freq_config_t blindFreqCfg;


} tfw_dl_pccpch_config_t ;

/** DL SCCPCH Configuration. */
typedef struct ALIGN(8)
{
  /* Word 0 */
  /*! \brief Action Type. Valid : TFW_CMD_ACTION_DISABLE, TFW_CMD_ACTION_ENABLE,
  TFW_CMD_ACTION_RECONFIG */
  uint32  actionType : 2;
  /*! \brief 2nd Interleave mode , valid:  0: frame based, 1: time slot based */
  uint32  secondIntlvMode : 1;
  uint32  tdmMode: 1; /* used to be TDM mode bit. No longer used in FW */
  /*! \brief CCTrCH Index associated with this SCCPCH, valid values: [0].
  The same CCTrCH is also mapped to use with tfw_ccTrCh_table_t index 0 */
  uint32  cctrchIndex : 3;
  /*! \brief Indication to insert an entry in DOB even if TRCH has zero TB. Valid value:
  0 : no insertion. 1: insertion */
  uint32  insertDobForZeroTB : 1;
  /*! \brief Time slot used by SCCPCH. Valid values (0,2,3,4,5,6) */
  uint32  timeSlot : 3;
  /*! \brief Indication to enable FET, frame early termination, Valid [0,1]
  0 : FET disabled, 1: FET enabled. 

  Notes : 
  1. This field needs to be populated and valid all the time. 
  2. When FET is enabled with action type == TFW_CMD_ACTION_ENABLE, channel is
  enabled at 10 ms frame boundary. Only immediate drop will be supported for FET enabled channel.

  */
  uint32  fetEn : 1;
  /*! \brief Offset in 5ms from 10 ms mark of a TTI that the 1st FET decode shall take place.
  Only valid if fetEn is set.
  Valid value, for 20 ms TTI [0,1], 0: 1st decode takes place at 10 ms into a TTI,
                                    1: 1st decode takes place at 15 ms into a TTI,
               for 40 ms TTI [0;;5] 0: 1st decode takes place at 10 ms into a TTI,
                                    1: 1st decode takes place at 15 ms into a TTI,
                                    2: 1st decode takes place at 20 ms into a TTI,
                                    3: 1st decode takes place at 25 ms into a TTI,
                                    4: 1st decode takes place at 30 ms into a TTI,
                                    5: 1st decode takes place at 35 ms into a TTI */
  uint32  fetDecOffset : 3;
  uint32  :1;

  /*! \brief Channel Present repetition period. Valid regardless of TDM mode. 
    Valid values: 1, 2,4,8,16,32,64 frames. 1 is used to indicate all frames are on */
  uint32  chanOnRepPeriod : 8;
  /*! \brief SFN number within TTI at the action time when config is applied  Valid :0..255.*/
  uint32  ttiSfn : 8;

  /* Word 1*/
  /*! \brief Time (in subframe) when the configuration should be applied */
  uint32  actionTime : 16;
  /*! \brief 16 bit SF16 code channels mask per DL slot. */
  uint32  phyChanBitMap : 16;

  /* Word 2 & 3 */
  /*! \brief Channel Present on-off bit mask to be applied at action time.  
  Bit[0..(chanOnRepPeriod-1)] with each bit indicates if Physical channel is present in
  the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
  should be set to 0  when chanOnRepPeriod is less than 64. But they will be ignored by FW*/
  uint64  chanPresentMask ALIGN(8);

} tfw_dl_sccpch_config_t ;


/** DL Per DPCH Configuration. */
typedef struct ALIGN(8)
{
  /* Word 0 */
  /*! \brief Action Type. Valid value : TFW_CMD_ACTION_DISABLE, 
    TFW_CMD_ACTION_ENABLE, TFW_CMD_ACTION_RECONFIG */
  uint32  actionType : 2;
  /*! \brief CCTrCH Index associated with this DPCH, valid values: [0..2] 
    Note that dpch with cctrchIndex 0 is mapped to use tfw_ccTrCh_table_t index 0, 
    similarly, 1 is mapped to use with tfw_ccTrCh_table_t index 1 and 2 is 
    mapped to use with tfw_ccTrCh_table_t index 2.
  */
  uint32  cctrchIndex : 3;
  /*! \brief 2nd Interleave mode , valid:  0: frame based, 1: time slot based */
  uint32  secondIntlvMode : 1;
  uint32  tdmMode: 1; /* used to be TDM mode bit. No longer used in FW */
  /*! \brief LVA enable, valid: 0: disabled. 1: enabled */
  uint32  lvaEnable : 1;
  uint32  : 8;
  /*! \brief CFN number at the action time when config is applied  Valid :0..255*/
  uint32  cfn : 8;
  /*! \brief Channel Present repetition period. Valid regardless of TDM mode. 
    Valid values: 1, 2,4,8,16,32,64 frames. 1 is used when all frames are on */
  uint32  chanOnRepPeriod : 8;

  /* Word 1 & 2 */
  /*! \brief Channel Present on-off bit mask to be applied at action time.  
  Bit[0..(chanOnRepPeriod-1)] with each bit indicates if Physical channel is present in
  the corresponding frame. 1: phy channel is present. 0: not present. Bit[chanOnRepPeriod..63] 
  should be set to 0  when chanOnRepPeriod is less than 64. But they will be ignored by FW*/
  uint64  chanPresentMask ALIGN(8);

  /* Word 3 to 5 */
  /*! \brief 16 bit SF16 code channels mask per DL slot. */
  uint16  phyChanBitMap[6];


} tfw_dl_per_dpch_config_t ;

/** TFW multiple DL DPCH structure. */
typedef struct
{
  /*! \brief up to 3 DPCH. Valid values [1,2,3]*/
  uint32  numDPCH : 8;
  /*! \brief Valid : TFW_CMD_TSTD_DISABLE & TFW_CMD_TSTD_ENABLE */
  uint32  tstdEnable : 1;
  /*! \brief Indication to run LVA. 1: Run LVA, 0: disable LVA */
  uint32  lvaEnable : 1;
  uint32  : 6;
  /*! \brief Time when the configuration should be applied, in unit of subframe */
  uint32  actionTime : 16;

  tfw_dl_per_dpch_config_t  dpch[3];

} tfw_dl_dpch_config_t ;


/** TFW HS Control Channel Information.
    Control channels include both HS-SCCH and HS-SICH */
typedef struct
{
  /*! \brief Time slot used by HS-SCCH, valid value: [0,2,3,4,5,6] */
  uint32  scchTimeSlot : 3;
  /*! \brief 16SF for the 1st physical channel of HS-SCCH, valid: [1..15]*/
  uint32  scchCode1 : 5;
  /*! \brief 16SF for the 2nd physical channel of HS-SCCH, valid: [2:16] && 
    scchCode2 > scchCode1 */
  uint32  scchCode2 : 5;
  /*! \brief Time slot used by HS-SICH, valid value: [0,2,3,4,5,6] */
  uint32  sichTimeSlot : 3;
  /*! \brief Absolute Midamble Shift to be used for HS-SICH. 
  Valid [0, 128/k, 2*(128/k), .., (k-1)*(128/k) ], where K is midamble 
  configuration [2,4,6,8,10,12,14,16] */
  uint32  midambleShift : 8;
  /*! \brief 16SF for physical channel of HS-SICH, valid: [1..16]*/
  uint32  sichCode1 :5;
  uint32  :3;

} tfw_hs_ctrlChan_info_t ;

/** HS-SICH Power Control Configuration.  */
typedef struct
{
  /*! \brief Power difference between Ack and Nack (in 1 dB), valid : -7..8 dB */
  int16   ackNakPwrOffset;      
  /*! \brief For open loop HS-SICH power control, valid : -120..-58 dB, in 1 dB*/
  int16   prxHSSich;           
  /*! \brief Gap in frame for pathloss compensation decision, valid: 0..255.
  TFW_DEMBACK_HS_SICH_ALWAYS_CLOSED_LOOP_GAP_CNT(256) is used by SW to indicate
  always closed loop operation when there is no explicit IE for gap count*/
  uint16  maxPwrCtrlGap;

  /*! \brief if 1, UE perform compensation for pathloss when HS-SICH 
    transmission gap is less than maxPwrCtrlGap. Valid : [0,1]*/
  uint8  compEnable;
  /*! \brief HS-SICH closed loop TPC step size. Valid : 1,2,3 (dB) */
  uint8  tpcStep;

} tfw_hssich_pwrCtrl_config_t ;

/** HS Control Channel Configuration Structure. */
typedef struct
{
  uint32  irFlush : 1;
  uint32  : 7;
  /*! \brief Number of HS control channel pairs. Valid values : 1..4*/
  uint32  numCtrlChannel : 3;       
  uint32  : 5;
  /*! \brief H-RNTI for this UE */ 
  uint32  hrnti : 16;

  /*! \brief Up to 4 pairs of HS-SCCH/HS-SICH control channel info */
  tfw_hs_ctrlChan_info_t  info[4];

  /*! \brief Power control information for HS-SICH */
  tfw_hssich_pwrCtrl_config_t   hssichPwrConfig;

} tfw_hs_ctrlChan_config_t ;


/** HS Harq process DRMB Info. */
typedef struct
{
  /*! \brief DRMB start address for this harq process, in unit of line (128 bit)
  SW should make sure drmb size for this harq is big enough to cover rounding and
  gap between system, P1 and P2 */
  uint32  harqDrmbStartAddr : 16;
  /*! \brief Virtual IR buf size for this harq process, in bit */
  uint32  harqDrmbSize : 16;      
} tfw_hs_drmb_t ;

/** HS Data Channel Configuration. */
typedef struct
{
  /* Word 0 */
  /*! \brief Number of harq prcesses in this configuration. Valid : 1..8 */
  uint32  numHarqProcess : 4;
  /*! \brief DRMB storage direction for DRM2. 0: symbols written in descending 
    addr order , 1: symbols written in ascending addr order */
  uint32  drmbStorageDir : 1;
  /*! \brief HS configuration database index used in DECOB */
  uint32  dbIdx : 1;
  uint32  :2; 
  /*! \brief for TB size LUT. 0: bit aligned tables, 1: octet aligned tables*/
  uint32  tfriTableType : 1;
  /*! \brief UE category for TB size LUT. valid range: [1..15] when 
    tfriTableType == 0, [1..23] when tfriTableType == 1 */
  uint32  ueCategory : 5;
  uint32  :18;
   
  /* Word 1 to 8 */
  /*! \brief up to 8 HARQ processes */
  tfw_hs_drmb_t   drmb[8];
} tfw_hs_dataChan_config_t ;

/** HS Channel Configuration. */
typedef struct
{
  /*! \*/
  /** Action Type. */
  /** Valid value : 
      \li TFW_CMD_ACTION_DISABLE
      \li TFW_CMD_ACTION_ENABLE
      \li TFW_CMD_ACTION_RECONFIG
      */
  uint32  actionType : 2;
  /*! \brief time threshold to flush all HSDPA HARQ, unit is in subframes
    please, set it to 0 to disable HARQ flush feature. Flush threshold will 
    be used only when certain criteria are met, like forced RF loss due to 
    DSDS, hsHarqFlushTh has 6 bits reserved, the max value is  63*5=315 ms 
  */
  uint32  iRFlushThSbFrmCnt : 6;  
  uint32  hsCtrlChanUpdate : 1;
  uint32  hsDataChanUpdate : 1;
  uint32  hsPdschMidambleUpdate : 1;
  uint32  : 5;
  /*! \brief Time (in subframe) when the configuration should be applied */
  uint32  actionTime : 16;
  
  /** HS Ctrl Channel Config. Valid only if hsCtrlChanUpdate =1 */
  tfw_hs_ctrlChan_config_t    hsCtrlChanCfg;
  /** HS Data Channel Config. Valid only if hsDataChanUpdate = 1 */
  tfw_hs_dataChan_config_t    hsDataChanCfg;
  /** HS-PDSCH Midmable Config. Valid only if hsPdschMidambleUpdate = 1 */
  tfw_midamble_config_t       hsPdschMidambleCfg; 

} tfw_hs_config_t ;


/** TFW DL CCTrCH Configuration table structure. */
typedef struct
{
  /* word 0 */
  /*! \brief SW CCTrCH index used in DOB, Valid range : ?? Note that
  this is only used in DOB for SW Table ID purpose. FW does not modify*/
  uint32  swCcTrChIndex : 8;
  /*! \brief DRMB storage direction for DRM. 0: symbols written in descending 
    addr order , 1: symbols written in ascending addr order */
  uint32  drmbStorageDir : 1;
  uint32  :7;
  /*! \brief DRMB start address for this CCTRCH, in unit of line (128 bytes)*/
  uint32  drmbStartAddr : 15;
  uint32  :1;

  /* word 1 */
  /*! \brief Number of TrCH in this CCTrCH .Valid range [1..8]*/
  uint32  numTrCH : 4;
  uint32  : 4;
  /*! \brief TrCH Info pool base index for this CCTrCH. TrCH pool index for this 
    CCTrCH starts from trchInfoBase to trchInfoBase+(numTrCH-1)
    Valid range [0..(TFW_TRCH_INFO_POOL_SIZE-numTrCH ] */
  uint32  trchInfoBase : 4;
  uint32  : 4;
  /*! \brief Total number of TF for this CCTrCH. Valid range [1..64] */
  uint32  totalNumTF : 6;
  uint32  : 2;
  /*! \brief TF Info pool base index for this CCTrCH. TF pool index for this 
    CCTrCH starts from tfInfoBase (relative to the beginning of the whole TF
    pool table) to tfInfoBase+ totalNumTF-1. Valid range
    [0..(TFW_TF_INFO_POOL_SIZE - totalNumTF)]*/
  uint32  tfInfoBase : 8;

  /* word 2 */
  /*! \brief number of TFC for this CCTrCH. Valid range [1..128] */
  uint32  numTFC : 8;
  uint32  : 8;
  /*! \brief TFC Info pool base index for this CCTrCH. TFC pool index for this 
    CCTrCH starts from tfcInfoBase (relative to the whole TFC pool table)
    to tfcInfoBase+ numTFC-1. Valid range [0..(TFW_TFC_INFO_POOL_SIZE - numTFC)]*/
  uint32  tfcInfoBase : 16;

  /* word 3, TTI Order */
  /*! \brief TrCH number within a CCTrCT according to TTI decending order. 
    valid range [0..7] && less than numTrCH */
  uint32  tti1 : 4;
  uint32  tti2 : 4;
  uint32  tti3 : 4;
  uint32  tti4 : 4;
  uint32  tti5 : 4;
  uint32  tti6 : 4;
  uint32  tti7 : 4;
  uint32  tti8 : 4;

} tfw_ccTrCh_table_t ;

/** TFW multiple CCTrCH Configuration structure. Up to 3 CCTrCH. When
    configuration for a CCTrCH table only updates TFC info base and TF info
    base, there is no reconfiguraiton in that CCTrCH. Corresponding run time
    info bases update will take place in the frame when action time expires.
     */
typedef struct
{
  /*! \brief Valid values 0: no update, 1: update */
  uint32  ccTrCh0Update : 1;
  /*! \brief Valid values 0: no update, 1: update */
  uint32  ccTrCh1Update : 1;
  /*! \brief Valid values 0: no update, 1: update */
  uint32  ccTrCh2Update : 1;
  /*! \brief Time when the configuration should be applied, in unit of subframe */
  uint32  : 13;
  uint32  actionTime : 16;
  /*! \brief CCTrCH table config for demback. For DPCH, up to 3 CCTrCH. The array 
  index shall match the cctrchid used in DL DPCH channel config. For SCCPCH, array
  index 0 shall be used always since there is only 1 CCTrCH */
  tfw_ccTrCh_table_t  ccTrCh[3];

} tfw_dl_ccTrCh_config_t ;

/** Transport Channel Dynamic Parameter Update. */
typedef struct
{
  /*! \brief Trch Update Bit Mask. Valid Bit[0..7] with each bit for each TrCH
  in TrCH info pool. Bit[0] for TrCH 0, Bit[1] for TrCh 1, and etc. Valid value
  [0,1], 0: no update, 1: update. Note that update will be effective immediately */
  uint8   updateBitMask;
  /** TRCH Decode Enable or Disable Control. Valid [0,1], 0: disable, 1:
      enable */
  uint8   enableBitMask;
} tfw_dl_trCh_update_config_t ;


/*! \brief Demback coding type definitions */
#define TFW_DBACK_CODE_TYPE_CONV_1_2      0
#define TFW_DBACK_CODE_TYPE_CONV_1_3      1
#define TFW_DBACK_CODE_TYPE_TURBO         2
#define TFW_DBACK_CODE_TYPE_UNCODED       3

/**
    \defgroup fSmiTrchInfoPool TRCH Info Pool
    \ingroup fSharedMemoryInterface
    */
/*\{*/

/** Transport Channel Configuraiton. */
typedef struct
{
  /*! \brief 1: TrCH is disabled, 0: TrCH is not disabled */
  uint16  disable : 1;
  /*! \brief Decode type 0: (1/2 Covolutional code), 1: (1/3 Covolutional code) 
    2: (1/3 Turbo code) 3: uncoded */
  uint16  codeType : 2;
  /*! \brief CRC length. Valid values 0: 0 bit, 1: 8 bit, 2: 12 bit, 
    3: 16 bit, 4: 24 bit */
  uint16  crcLength : 3;
  uint16  : 2 ;
  /*! \brief number of TF for this TrCH, valid values [1..32] */
  uint16  numTF : 6;
  /*! \brief TTI for this TrCH. 0: 10 ms, 1: 20ms, 2: 40ms, 3: 80ms*/
  uint16  tti : 2;

} tfw_trch_config_t ;


/*! @brief TrCH info pool. Maximum number of TrCH anytime is 8. 
    Double the size for back to back seamless configuration. FW has exclusive
    access right of the part of pool in the frame when action time expires
     */
typedef struct
{
  tfw_trch_config_t trchInfo[TFW_TRCH_INFO_POOL_SIZE];

} tfw_trch_info_pool_t ;

/*\}*/


/**
    \defgroup fSmiTfPool TF Pool
    \ingroup fSharedMemoryInterface
    */
/*\{*/
/*! @brief TF pool entry structure for one TF of a TrCH in a CCTrCH*/
typedef struct
{
  /* Word 0 */
  /*! \brief Transport block size in bit, valid [1..5000] */
  uint32 trBlkSize:13;              
  /*! \brief Coded block size in bit, valid [1..5114] */                       
  uint32 codeBlkSize:13;            
  /*! \brief Number of Transport blocks per TTI, Valid range : [1..32] */
  uint32 numTrBlk:6;               


  /* Word 1 */
  /*! \brief for n iteration programmed, 2*n+1 half iteration */
  uint32 maxIterationNum:4;
  /*! Valid [0..51]*/    
  uint32 primeNumIndex:6; 
  /*! \brief Number of coded blocks per TTI, valid [0..31] */     
  uint32 numCodeBlk:5;         
  uint32 lastRowBitExch:1;     
  uint32 drmScaleFactor:2;     
  uint32 turboBetaSel:4;       
  /*! \brief Yamamoto threshold value. Used for quality bit determination */             
  uint32 yamThreshold:4; 
  /*! \brief Number of filler bits for this TrCH, */      
  uint32 numFillerBits:6; 

  /* Word 2 */
  uint32 numColumn:8;          
  uint32 lastColumn:8;         
  uint32 primeNum:9;           
  uint32 lastRow:5;            
  uint32 numRowIndex:2;        

} tfw_tf_pool_t ;

/*! @brief TFW TF Info Pool table. Maximum TF anytime is 64. Pool size is 
  increased to allow back to back SW configuration and FW run time
  access. FW has exclusive access for the part of pool from the frame
  configuration action time expires till next config. */

typedef struct
{
  tfw_tf_pool_t   tfIdx[TFW_TF_INFO_POOL_SIZE];        
} tfw_tf_pool_table_t ;
/*\}*/


/**
    \defgroup fSmiTfcPool TFC Pool
    \ingroup fSharedMemoryInterface
    */
/*\{*/
/** Per TrCH RM structure */
typedef struct
{
  /** eini for RM. For a=2 */
  uint16  eini1[8];
  /** eini for RM. For a=1 */
  uint16  eini2[8];
  /*! \brief ePlus divided by a, for RM param calc. */
  uint16  ePlusOverA; 
  /*! \brief Ratematched size in bit */            
  uint16  rateMatchedSize; 
  /*! \brief eMinus divied by a, for RM param calc*/
  uint16  eMinusOverA;       

  /*! \brief DeltaNi is Odd number or not, valid 1: odd, 0: even */     
  uint16  deltaNOdd:1;          
  /*! \brief Rate matching type. Valid 0: no RM, 1: repetition, 2: puncture  */                  
  uint16  rmType:2;
  uint16  :13;                        

} tfw_trch_rm_t ;

/*! @brief TFC Pool entry structure of one TFC for a CCTrCH. There 
  are up to 8 TrCH in a TFC. Each field within a TFC indicates the TF index
  for this CCTrCH. 0 means the 1st TF of the 1st TrCH in
  this CCTrCH. 1 means the 2nd TF of the 1st TrCH and etc. Up to 64 TF
  in a CCTrCH. Valid range : [0..63] */
typedef struct
{
  /*! \brief Per TrCH RM parameters. Up to 8 TrCH */
  tfw_trch_rm_t   rm[8];

  /* 2 words */
  /*! \brief each byte contains one TF of a TrCH within TFC for a CCTrCH */
  uint8   tfi[8];

  /* 1 word */
  /*! \brief number of physical channels used in this TFC. Valid : [1..64] */
  uint32  phyNumUsed : 7;
  uint32  : 9 ;
  /*! \brief Indication that this TFC entry is invalid.*/
  uint32  invalid : 1;
  uint32  : 15 ;

} tfw_tfc_pool_t;

/*! @brief TFW TFC Info Pool. Maximum TFC anytime is 128. Pool size is 
  increased to allow back to back SW configuration and FW run time
  access. 
 
  All entries that belong to a CCTrCH have to be contiguous and linear. Pool
  index of the 1st entry for a CCTrCH is indicated as tfcInfoBase (relative
  to the beginning of the whole Info Pool table) within the corresponding
  CCTrCH table structure. For example, there are 3 CCTrCHs with 30 TFCs for
  the 1st CCTrCH and 40 TFCs for the 2nd CCTrCH and 50 for the 3rd CCTrCH.
  The corresponding infoBase could be 0, 30 and 40, which indicate that the
  1st CCTrCH has pool entry index from 0 to 29. The 2nd CCTrCH gas pool entry
  index from 30 to 69. The 3rd CCTrCH has pool entry index from 70 to 119.
 
  Linear means all pool entries for a CCTrCH cannot wrap around the end of
  the whole pool table.
 
  FW has exclusive access for the part of pool from the frame a
  configuration action time expires till the frame the next configuration
  action time expires. For example, assuming FW is uisng entry 0..127 in
  frame n-1 while SW is making a new configuraiton in entry 128 to 255 for
  action time in frame x. Once action time expires in frame n, FW will have
  exclusive access right of entry 128 to 255 till the frame (inclusive)
  when the next configuration acton time expires. 
 */
typedef struct
{
  tfw_tfc_pool_t  tfcIdx[TFW_TFC_INFO_POOL_SIZE];
} tfw_tfc_pool_table_t;
/*\}*/

/**
    \defgroup fSmiNonHsDob Non-HS DecOB Status Fifo
    \ingroup fSharedMemoryInterface
    */
/*\{*/
/** Per Entry Data Structure in Non HS DOB status FIFO. */
typedef struct
{
  /*! \brief The extra information word. Depending on channels, the meaning is
  different. For PCCPCCH, it will give STMR wall timer count at TTI. Not used
  for other channels */
  /* Word 0*/
  union
  {
    /*! \brief STMR wall timer counter at begin of TTI, relevent only for blind & 
    S1/S1 PCCPCH. In other cases, chipX8 and subFrmCnt are for RX time. */
    /** frame count and chip x8 count */
    struct
    {
      uint32  chipX8cnt : 16;
      uint32  subFrmCnt : 13;
      uint32  : 3;
    };
    /*! \brief STMR wall timer counter including both subframe and chipx8 count 
    at begin of TTI, relevent only for blind & S1/S2 PCCPCH */
    uint32  stmrWallTimeCnt;
    /*! \brief */
    uint32  rxTimeCnt;
  };

  /* Word 1*/
  /*! \brief 1: TFCI decode fail, 0: TFCI decode successful */        
  uint32 tfciDecFail : 1; 
  /*! \brief Special burst detection, 0: no SB, 1: SB detected in TTI, not used for pccpch */              
  uint32 sbDetect : 1;
  /*! \brief Indication of last TrCH decode in a CCTrCH to use this config ID after a
  reconfiguration or disable. 1 : yes, 0: no. When there is no reconfig or enable cmd
  it shall be 0. Note that if reconfig or disable cmd comes before maximum TTI
  of TrcH within a CCTrCH, there maybe no TrcH decode*/
  uint32  lastTrChInd : 1;
  /*! \brief TFI error for a Trch, 0: no Error, 1: Inconsistent TFI detected in TTI.
  To be ignored in pccpch or if sbDetect is set */              
  uint32 tfiError : 1;
  /*! \brief Partial TTI Punctured Indication From Slot Suspenstion. 0: No partial puncture
  i.e. no slots in a TTI has been punctured by slot suspension. 1: Partial puncture, 
  i.e. at least one slot in a TTI has been punctured from slot suspension.  */
  uint32  puncturedInd : 1;
  /*! \brief FET enable indicagtion for this TRCH. 
  Valid [0,1] 0: FET is disabled, 1:FET is enabled */
  uint32  fetInd : 1;

  /*! \brief Partial TTI has been de-sensed. 0: No de-sensed
  i.e. no slots in a TTI has been de-sensed by multi-sim 1: Partial de-sensed, 
  i.e. at least one slot in a TTI has been de-sensed by multi-sim .  */
  uint32  deSensed : 1;

#if 0
  /*! \brief Lva indication. 1: LVA can be ued for this TRCH. 0: LVA is not used */
  uint32  lvaInd : 1;
  /*! \brief Indication: Lva decode done and write back to Dob*/
  uint32  lvdWriteBackInd:1; 
  uint32 : 7;
#endif 
  /*! \brief which sfm inside TTI FET is done. 0:(invalid), 1: 10ms sfm, 2: 15ms sfm, 3: 20 ms sfm*/
  uint32  fetTTISfm: 2;
  /*! \brief average Sir of two subframes before decoding subframes, int 7Q1*/
  uint32  fetSir:7;
  
  /*! \brief RRC CCTrCH index this TrCH belongs to . Valid range ??*/                         
  uint32  cctrchId : 8; 
  /*! \brief Arriving index of TrCH in CCTrCH, Valid [0..7]  */           
  uint32 trChId:3;
  uint32 codeType:2;
  /*! \brief demback CCTRCH index this TrCH belongs to. Only valid
  for SCCPCH and DPCH. Valid [0..2] */
  uint32 dbackCctrchId : 3;
          
  /* Word 2*/
  /*! \brief TrCH start addr in DOB */
  uint32 trChStartAddr : 15;      
  uint32 : 1;
  /*! \brief TB size in bit, */
  uint32 trBlkSize : 13;     

  /*! \brief Lva indication. 1: LVA can be ued for this TRCH. 0: LVA is not used */
  uint32  lvaInd : 1;
  /*! \brief Indication: Lva decode done and write back to Dob*/
  uint32  lvdWriteBackInd:1; 
  uint32 : 1;

  /* Word 3*/
  /*! \brief End of TTI CFN for a TrCH, Valid [0..255] */
  uint32  cfn : 8; 
  /*! \brief TF index for this TrCH within a CCTrCH. TF index does not 
    reset to 0 when going across different TrCHs witin a CCTrCH. Valid [0..63]*/    
  uint32  tfId : 8;                
  /*! \brief Decoded TFCI, Valid range [0..127]*/
  uint32  decodedTfci : 8; 
  /*! \brief Reserved bits : crc word in UMPLT mode or other debug info */
  uint32  rsvd: 8;
  
} tfw_nonHS_dob_status_t;

/** Non HS DOB Status FIFO. Write index will be updated when DOB status FIFO
    entry is updated. */
typedef struct
{
  tfw_nonHS_dob_status_t    status[TFW_NON_HS_DECOB_STATUS_FIFO_SIZE];
  /*! \brief Base address of SVD DECOB */
  uint32  svdDobStartAddr;
  /*! \brief Base address of TD DECOB */
  uint32  tdDobStartAddr;
  /*! \brief write index in non HS DOB status FIFO, 
    valid [0..(TFW_NON_HS_DECOB_STATUS_FIFO_SIZE-1) */
  uint16  writeIndex;  
} tfw_nonHS_dob_status_fifo_t;
/*\}*/




/**
    \defgroup fSmiHsDobStatus HS DecOB Status Fifo
    \ingroup fSharedMemoryInterface
    */
/*\{*/
/** Per Entry Data Structure in HS DOB status FIFO. */
typedef struct
{
  /* Word 0 */
  uint16  valid : 1;
  /*! \brief Which HS info index is used for this harq */
  uint16  hsInfoIdx:1; 
  /*! \brief 1: new transmission, 0: Retransmission*/               
  uint16  hsNewData:1;                
  uint16  :5; 
  /*! \brief Harq process ID, Valid [0..7] */                        
  uint16  hsHarqId:3;    
  
  /*! \brief Forced IR flush indicator, will be set to 1 after a HARQ flush*/                        
  uint16  forcedIrFlushInd:1;
  uint16  :4;                         

  /*! \brief TrCH start addr in DOB */
  uint16  hsTrChStartAddr : 15;         
  uint16  : 1;                         

  /* Word1 */
  /*! \brief SFN when this Harq process arrives */
  uint16  hsSubFrmNum : 13;             
  uint16  rvsd: 3;                         

  /*! \brief HS TB size in bit, */
  uint16  hsTrBlkSize;                

} tfw_hs_dob_status_t;

/** HS DOB Status FIFO. Write index will be updated when DOB status FIFO entry
    is updated. */
typedef struct
{
  tfw_hs_dob_status_t    status[TFW_HS_DECOB_STATUS_FIFO_SIZE];
  /*! \brief Base address of TD DECOB */
  uint32  tdDobStartAddr;
  /*! \brief write index in HS DOB status FIFO, 
    valid [0..(TFW_HS_DECOB_STATUS_FIFO_SIZE-1) */
  uint16  writeIndex;  
} tfw_hs_dob_status_fifo_t;
/*\}*/


/**
    \defgroup fSmiUlPwrInfo UL Power Info
    \ingroup fSharedMemoryInterface
    */
/*\{*/


/** Closed loop TPC info for UL DPCH  */
typedef struct 
{
  /** Up to 1 special slot + 5 UL Time Slots,[0] is for UpPTS and [1..5] are for
      TS1 to TS5, and up to 2 phys channels per slot */
  struct
  {
    /** Valid [0,1] :
        \li 0 : Slot invalid info, the rest of parameters are ignored.
        \li 1 : Slot valid info. At least one code is present.
         */
    uint8   valid;

    /** Valid [0,1]. 0: Total TX power for this slot has not hit Max */
    uint8   overMaxPwrInd;
    /** Valid [0,1]. 0: Total TX power for this slot has not hit Min */
    uint8   underMinPwrInd;
    /* unused */
    uint8   :8;
    /*! Ajdusted max TX power w/o MPR in this slot w/ <TBD> 1/16 dB resolution, 
    ie. 4 fractional bits*/
    int16   pwrMaxAdjusted;
    /** total TX power in this slot with <TBD> 1/16 dB resolution, i.e. 4
        fractional points. */
    int16   txPwr;
    /** computed power, the power level prior to the limiting
     *  with <TBD> 1/16 dB resolution, i.e. 4 fractional points.  */
    int16   totalPwr;
    /** UP to 2 code channels in a UL Slot */
    struct
    {
      /** Indication if OVSF code channel is present in current subframe. 0: not
          present, 1: present */
      uint8   ulPhyChanPresent;
      /** UL CCTrCH.  Valid[0,1], 0: 1st UL CCTRCH, 1: 2nd CCTrCH */
      uint8   ulCCTrchId;
      /** OVSF code. Valid[1..16] */
      uint8   code;
      /* unused */
      uint8   :8;
      /*! Code Channel power in this slot w/ <TBD> 1/16 dB resolution, ie. 4 fractional bits*/
      int16   channelPwr;
      /*! Back Off power in this code channel w/ <TBD> 1/16 dB resolution, ie. 4 fractional bits*/
      int16   chanBackOffPwr;
    } chan[TFW_MAX_UL_PHY_PER_SLOT];

  } phyChan[TFW_MAX_UL_SLOT+1];
} tfw_ul_tx_pwr_info_t ;


/** Closed loop TPC info for HS-SICH */
typedef struct
{
  /** lst 7 bit of subframe number when HS-SICH will be transmitted. Valid
      [0..127, 255]. 255 indicats there is no HS-SICH to be transmitted */
  uint8   sfm;
  /** Time slot when HS-SICH will be transmitted in the subframe indicated.
      Valid[0,2,3,4,5,6]. Only valid if sfm above is not 255.  */
  uint8   ts;
  /** HS-SICH Tx power with resolution of 1/16 dB resolution. It includes
      prxHssich + pathloss + closed loop TPC. Only valid if sfm above is not
      255. */
  int16   txPwr;
} tfw_hssich_pwr_info_t ;

/** Tx Power info that is used to calculate Power headroom in HSUPA or power
    events. */
typedef struct 
{
  /** TX power info in subframe sfmNum */
  tfw_ul_tx_pwr_info_t      txPwr;
  /** next HS-SICH to be transmitted after current subframe */
  tfw_hssich_pwr_info_t     hssichPwr;
  /** Subframe number when this information is updated. Valid [0..8191]. This
      should be the first info to access. If subframe number does not match the
      subframe when this info is accessed, the rest of info is either old or
      incomplete. */
  uint16    sfmNum; 
} tfw_ul_pwr_info_t ;
/*\}*/

/**
    \defgroup fSmiUlTimingInfo UL Timing Info
    \ingroup fSharedMemoryInterface
    */
/*\{*/

/** Timing advance info for each UL time slot */
typedef struct 
{
  /** Subframe number when UL Slots timing info here will be applied. Valid
      [0..8191]. This should be the first info to access. If subframe number
      does not match the subframe when this info is accessed, the rest of info
      is either old or incomplete. */
  uint16    sfmNum; 
  /** Bit Mask to indicate which UL time slot will be transmitted and has
      corresponding valid timing info. Bit 0: UPPCH, Bit 1: TS1...Bit 5: TS 5 */
  uint16    txBitMask; 
  /** Timing relative to DL slots in 1/8 chip. Index 0 for UPPCH, Index [1..5]
      for UL Ts[1..5] */
  int16     timeVsDl[(TFW_MAX_UL_SLOT+1)]; 

} tfw_ul_timing_info_t ;
/*\}*/

typedef struct
{
  /*! \brief Time slot used by E-AGCH, valid value: [0,2,3,4,5,6] */
  uint16  eagchTimeSlot : 6;
  /*! \brief 16SF for the 1st physical channel of E-AGCH, valid: [1..15]*/
  uint16  eagchCode1 : 5;
  /*! \brief 16SF for the 2nd physical channel of E-AGCH, valid: [2:16] && 
    eagchCode2 > eagchCode1 */
  uint16  eagchCode2 : 5;
} tfw_per_eagch_config_t ;
/** E-AGCH Channel Configuration. */
typedef struct
{
  /* Word 0 */
  /*! \brief Action Type. Valid value : TFW_CMD_ACTION_DISABLE, 
    TFW_CMD_ACTION_ENABLE, TFW_CMD_ACTION_RECONFIG */
  uint32  actionType : 2;
  uint32  rdi : 1;
  uint32  : 13;
  /*! \brief Time when the configuration should be applied, in unit of subframe */
  uint32  actionTime : 16;
  /* Word 1 */
  uint32  numEAGCH : 16;
  uint32  eRNTI : 16;

  tfw_per_eagch_config_t  info[TFW_EAGCH_CNT_MAX];  
} tfw_eagch_config_t;

typedef struct
{
  /*! \brief Time slot used by E-HICH, valid value: [0,2,3,4,5,6] */
  uint16  ehichTimeSlot : 6;
  /*! \brief EI for E-HICH Index, valid: [0..3] */
  uint16  ehichIndex : 2;
  /*! \brief 16SF for the physical channel of E-HICH, valid: [1..16]*/
  uint16  ehichCode : 5;
  uint16  : 3;
} tfw_per_ehich_config_t ;

/** E-HICH Channel Configuration for Scheduled Transmission.  */
typedef struct
{
  /* Word 0 */
  /*! \brief Action Type. Valid value : TFW_CMD_ACTION_DISABLE, 
    TFW_CMD_ACTION_ENABLE, TFW_CMD_ACTION_RECONFIG */
  uint32  actionType : 2;
  /*! \brief Number of E-HICH for this configuration. Valid [1..4]*/
  uint32  numEHICH : 3;
  uint32  :3;
  /*! \brief Number of time slots between start of the last active slot of E-DCH
   TTI and start of E-HICH [4..15] */
  uint32  nEHICH : 8;
  /*! \brief Time when the configuration should be applied, in unit of subframe */
  uint32  actionTime : 16;

  tfw_per_ehich_config_t  info[TFW_SCHLD_EHICH_CNT_MAX];
} tfw_ehich_config_t;

typedef struct
{
  /* Word 0 */
  /*! \brief Action Type. Valid value : TFW_CMD_ACTION_DISABLE, 
    TFW_CMD_ACTION_ENABLE */
  uint32  actionType : 2;
  /*! \brief 16SF for the physical channel of E-HICH, valid: [1..16]*/
  uint32  ehichCode : 5;
  /*! \brief Number of time slots between start of the last active slot of E-DCH
   TTI and start of E-HICH [4..15] */
  uint32  nEHICH : 4;
  /*! \brief Signature group index [0..19] */
  uint32  sigGrpIndex : 5;
  /*! \brief Time slot used by E-HICH, valid value: [0,2,3,4,5,6] */
  uint32  ehichTimeSlot : 3;
  /*! \brief Time when the configuration should be applied, in unit of subframe */
  uint32  actionTime : 13;

  /* Word 1 */
  tfw_midamble_config_t   midAmbleCfg;

} tfw_non_schld_ehich_config_t ;

/** UpPCH Tx Power Static parameter configuration  */
typedef struct
{
  /*! \brief Maximum Allowed Combined TX output power. Valid <TBD> 
  [-50dBm..33dBm] in 1/16 dBm step. Note that FW only keeps one copy of maxTxPwr
  and will overwrite with the latest message with maxTxPwr*/
  int16    maxTxPwr;
  /*! \brief Initial timing advance for UpPCH in Cx8. Default 48*8 Cx8.*/
  int16    timeAdv;
  /*! \brief Timing adjustment for UpPCH relative to DwPCH. 
  Valid [0..127] in 16 chip step */
  uint8    nUpPCHShift;
  /*! \brief Flag indicating whether or not to use the timeAdv */
  boolean  useInitialTimeAdv;

} tfw_uppch_tx_pwr_timing_config_t ;

/** UL DPCH Initial Per UL OVSF Code Channel Configuration */
typedef struct
{
  /** OVSF Physical Channel Present Indication. */
  /** Valid [0,1] :
      \li 0 : not present, the rest of parameters are ignored.
      \li 1 : present
       */
  uint32  valid : 1;
  
  /** OVSF Code. Valid [1..sf], sf=1,2,4,8,16 */
  uint32  chanCode : 5;
  /** Spreading factor for DPCH. Valid [1,2,4,8,16] */
  uint32  sf : 5;
  /*! \brief Source CCTrCH Index, need to be consistent with CCTrCH number in
  TX encode. Valid [0,1], 0: 1st CCTrCH, 1: 2nd CCTrCH */
  uint32  cctrchId : 1;
  uint32  : 5;
  /** Number of SS TPC bits in an OVSF Code Channel. Valid [0,2,4,8,16,32] */
  uint32  numSsTpc : 6;
  uint32  : 2;
  /** Number of TFCI bits in an OVSF Code Channel. Valid [0,4,8,16,32] */
  uint32  numTFCI : 6;
  uint32  : 1;
  /** Uplink CCTrCh-Timeslot pair index (for UL power control of CCTrChs */
  /** Valid [0...9] */
  uint8   cctrchTsIndex : 4;           
  /** Uplink time-slot index (for UL timing control of CCTrCHs) */
  /** Valid [0...4] */
  uint8   tsIndex : 3;
  uint8   : 1;

} tfw_ul_dpch_initial_per_phy_config_t ;

/** DPCH Tx Power and Timing Static parameter configuration  */
typedef struct
{
  /*! \brief Maximum Allowed Combined TX output power in a slot. Valid <TBD> 
  [-50dBm..33dBm] in 1/16 dBm step. Note that FW only keeps one copy of maxTxPwr
  and will overwrite with the latest message with maxTxPwr*/
  int16   maxTxPwr;
  /*! \brief TPC Step Size. Valid [1,2,3] in dB*/
  uint8   tpcStep;
  /*! \brief UL Timing Advance Control. 
  Valid [TFW_UL_DPCH_TIME_ADV_CTRL_ENABLE, TFW_UL_DPCH_TIME_ADV_CTRL_DISABLE]. Note
  that though it is an optional IE, SW should enable timeAdv */
  uint8   timeAdvCtrl;
  /*! \brief Step size for the adjustment of Uplink transmission timing.
  Valid [1..8] in 1/8 chip step */
  uint8   timeAdjStep;
  /*! \brief Frequency of the adjustment of the uplink transmission timing.
  Valid [1..8] in subframe */
  uint8   timeAdjFreqM;

  /*! \brief Flag used to indicate that tpcAccInitVal is valid and shall be applied.
  This is mainly used in HO failure recovery. Valid [0,1]. 1: use tpcAccInitVal,
  0: otherwise*/
  uint8   useTPCAccInit;
  /*! \brief Valid when useTPCAccInit = 1. In dB. */
  int8    tpcAccInitVal;
  /*! \brief Flag used to indicate that timeAdv is valid and shall be applied.
  This is mainly used in HO failure recovery. Valid [TRUE,FALSE]. TRUE: use timeAdv,
  FALSE: otherwise */
  boolean useInitialTimeAdv;
  
  /*! \brief used to enabling and disabling SS freezing and unfreezing workround, 
  true: disable SS freezing. false: enable ss freezing  */
  boolean disableSSFreeze;  
  
  /*! \brief Valid when useInitialTimeAdv = TRUE. In 1/8 chip*/
  int16   timeAdv;
    
  /*! \brief Number of CCTrCH of DPCH type in this configuration. Valid [1,2] */
  uint8   numCCTrCH;

  /*! \brief Number of UL CCTrCH-TS pairs. Valid [0...10] */
  uint8   numCCTrCHTimeSlotPairs;
  /*! \brief Number of UL time slots carrying CCTrCHs. Valid [0...5] */
  uint8   numCCTrCHTimeSlots;
  /*! \brief UL DPCH tx power boost according to DL DPCH SNR ON/OFF */
  boolean enableTxPwrBoost;  

  /*! \brief UL DPCH  tx power increase step */
  uint8  txPwrBoostStep;
  
  /*! \brief DL DPCH SNR difference threshold */
  uint8  snrDiffThres;

  /*! \brief control word for UE test mode
  b0: callbox power difference level setting, 0~15dB */
  uint8 txPwrDiffThres;

  /*! \brief OVSF Code Channel Configuration in all UL Slot.
  There are up to 2 code channels in each time slot and up to 5 time slots.
  There is a valid bit in each code channel to indicate if the physical
  channel is present. When there is only one code channel present in a time 
  slot, the first channel (even) needs to be populated. When there are two code
  channels present in a time slot, the first code channel shall have a lower
  SC (Spreading Code Number) than that in the 2nd (odd) code channel. */
  tfw_ul_dpch_initial_per_phy_config_t phy[TFW_MAX_UL_SLOT][TFW_MAX_UL_PHY_PER_SLOT];

} tfw_dpch_tx_pwr_timing_config_t ;

/** DPCH Tx Power and Timing Static parameter update after initial DPCH
    config */
typedef struct
{
  /*! \brief TPC Step Size. Valid [1,2,3] in dB*/
  uint8   tpcStep;
  /*! \brief Step size for the adjustment of Uplink transmission timing.
  Valid [1..8] in 1/8 chip step */
  uint8   timeAdjStep;
  /*! \brief Frequency of the adjustment of the uplink transmission timing.
  Valid [1..8] in subframe */
  uint8   timeAdjFreqM;
} tfw_dpch_tx_pwr_timing_update_t ;

/** HS-SICH override command */
typedef struct
{
  /*! \brief action time: frame number 
  Valid: [0-4095] */
  uint16   frameNum : 12;
  /*! \brief subframe bit mask to disable
  Valid: [0b10: disable HS-SICH in even subframe only]
         [0b01: disable HS-SICH in odd subframe only]
         [0b11: disable HS-SICH in both even and odd subframes.] */
  uint16   subframeMask : 2;
  uint16 : 2;
  /*! \brief disable HS-SICH.
  Valid [TRUE:  disable HS-SICH]
        [FALSE: no action] */
  boolean  disableHssich;
} tfw_hssich_override_t ;

/** UpPCH and FPACH composite Channel Configuration for PRACH or E-RUCCH
    Transmission. */
typedef struct
{
  /*! \brief Action Type. Valid value : TFW_CMD_ACTION_DISABLE, 
    TFW_CMD_ACTION_ENABLE, TFW_CMD_ACTION_RECONFIG */
  /** Valid value : 
      \li TFW_CMD_ACTION_DISABLE : random access procedure aborted
      \li TFW_CMD_ACTION_ENABLE : send UL sync and mointor FPACH
       */
  /* Word 0 */
  uint32  actionType : 1;
  /** Maximum number of subframes after UL sync to monitor ack from FPACH.
      Valid [1..4] */
  /*! \brief Time slot used by FPACH, valid value: [0,2,3,4,5,6] */
  uint32  fpachTimeSlot : 6;
  uint32  : 1;
  /*! \brief Maximum number of sub-frames to wait for the network 
  acknowledgement to a sent signature. Valid[1..4]*/
  uint32  wt : 3;
  /*! \brief 16SF for the physical channel of fpach, valid: [1..16]*/
  uint32  fpachCode : 5;
  /** Sequence number for this random access procedure. Valid range [0..255] */
  uint32  txSeqNum : 8;
  /** UL Sync Code. Valid range [0..255] */
  uint32  ulSyncCode : 8;
  /* Word 1 */
  /** Sub-frame number to start transmission on UpPCH. Valid [0..8191] */
  uint16  ulSyncTxSubFrm;

  /*! \brief UL Sync TX Power including step and pathloss compensation, if any.
  Valid <TBD> [-50dBm..33dBm] in 1/16 dBm step*/
  int16   ulSyncTxPwr;
  /*! \brief Flag indicating whether or not to use the timeAdv */
  boolean  useInitialTimeAdv;  
  /*! \brief Initial timing advance for UpPCH in Cx8. Default 48*8 Cx8.*/
  int16    timeAdv;
  /* Word 2 including stuffing */
  /** Midamble configuration for DL time slot that constains FPACH. */
  tfw_midamble_config_t   midambleCfg;

} tfw_uppch_fpach_config_t;

  /*-----------------------------------------------------------------------*/
  /*  NEW DATA STRUCTURE (WAV_IB) FOR CHANNEL ENC BASE ADDRESS CALCULATION */
  /*-----------------------------------------------------------------------*/
/** WAV_IB struct to be used for DPCH, PRACH/ERUCCH, EPUCH Channel ENC Configuration */
typedef struct
{
  /*! \brief Array of Number of DATA1 & DATA2 Samples in array for Ch0/Ch1 Encoding*/
  uint16   data1NumSamp;
  uint16   data2NumSamp;

} tfw_wavib_t ;
  /*-----------------------------------------------------------------------*/

/** Physical Random Access Channel Configuration. */
typedef struct
{
  /* Word 0 */
  /*! \brief Message TTI length in subframe. Valid [1,2,4] for 5,10,20ms. value 4 (20ms)
  is only valid for PRACH and not valid for E-RUCCH */
  uint16  msgTTIlen : 4;
  /*! \brief Time slot used by PRACH, valid value: [1,2,3,4,5] */
  uint16  timeSlot : 4;
  /*! \brief Absolute Midamble Shift to be used for PRACH. 
  Valid [0, 128/k, 2*(128/k), .., (k-1)*(128/k) ], where K is midamble 
  configuration [2,4,6,8,10,12,14,16] */
  uint16  midambleShift : 8; 
  /*! \brief 16SF for the physical channel of PRACH, valid: [1..16]*/

  uint16  chanCode : 8;
  /** Spreading factor for PRACH or E-RUCCH. */
  /** Valid value : [4,8,16] for PRACH, [8,16] for E-RUCCH  */
  uint16  sf : 5;
  uint16  : 3;

  /* Word 1 */
  /*! \brief Sub-frame number to start transmission on PRACH. Valid [0..8191] */
  uint16  txSubFrm;
  /*! \brief Starting position of the UpPCH from FPACH in 1/8 chip. 
  Valid[0..8191] */
  uint16  upPchPos ;

  /* Word 2 and stuffing */
  /*! \brief PRACH TX power includes BetaD and compensation for pathloss, if any. 
  Valid <TBD> [-50dBm .. 33dBm] in 1/16 dBm step */
  int16   prachTxPwr;

  /*! \brief Array of Channel ENC Ping_Pong Flag for Ch0/Ch1 Encoding */
  uint8    encPingPongFlag;

  /*! \brief WAV_IB Channel Encoder for PRACH/ERUCCH configuration */
  tfw_wavib_t   encConfig;

} tfw_prach_config_t ;

/** UL DPCH Per UL OVSF Code Channel Configuration */
typedef struct
{

  /** OVSF Physical Channel Present Indication. */
  /** Valid [0,1] :
      \li 0 : not present, the rest of parameters are ignored.
      \li 1 : present
       */
  uint32  valid : 1;
  
  /** OVSF Code. Valid [1..sf], sf=1,2,4,8,16 */
  uint32  chanCode : 5;
  /** Spreading factor for DPCH. Valid [1,2,4,8,16] */
  uint32  sf : 5;
  /*! \brief Source CCTrCH Index, need to be consistent with CCTrCH number in
  TX encode. Valid [0,1], 0: 1st CCTrCH, 1: 2nd CCTrCH */
  uint32  cctrchId : 1;
  /** Number of SS TPC bits in an OVSF Code Channel. Valid [0,2,4,8,16,32] */
  uint32  numSsTpc : 6;
  /** Number of TFCI bits in an OVSF Code Channel. Valid [0,2,4,8,16,32] */
  uint32  numTFCI : 6;
  /*! \brief Absolute Midamble Shift to be used for DPCH
  Valid [0, 128/k, 2*(128/k), .., (k-1)*(128/k) ], where K is midamble 
  configuration [2,4,6,8,10,12,14,16] */
  uint32  midambleShift : 8;

  /*! \brief WAV_IB Channel Encoder for DPCH configuration */
  tfw_wavib_t   encConfig;

} tfw_ul_dpch_per_phy_config_t ;


/** UL DPCH Per CCtrCH Configuration */
typedef struct
{
  /** CCTrCH info present indication. 
  Valid :
  \li TFW_UL_CCTRCH_VALID : CCTRCH info present
  \li TFW_UL_CCTRCH_INVALID : CCTRCH info not present, ignore the rest of content */
  uint16  valid : 1;
  /*! \brief UL CCTrCH DTX indication. Valid only when "valid" = TFW_UL_CCTRCH_VALID
  \li 1: This CCTrCH is DTXed 
  \li 0 : This CCTrCH is not DTXed 
  */
  uint16  dtx : 1;
  /*! \brief UL Special Burst indication. Valid only when "valid" = TFW_UL_CCTRCH_VALID
  \li 1 : This CCTrCH contains special burst
  \li 0 : This CCTrCH does not constain special burst */
  uint16  sb : 1;
  /*! \brief Uncoded TFCI for this CCTrCH. Valid Range [0..1023]. =0 for Special
  Burst or DTX */
  uint16  uncodedTFCI : 10;
  uint16  : 3;
  /*! \brief DPCH TX power including BetaD gain (from TFC) and pathloss 
  compensation, if any, but excluding Gama gain (from SF) per CCTrCH.
  Valid <TBD> [-50dBm..33dBm] in 1/16 dBm step */
  int16   dpchTxPwr; 

} tfw_ul_dpch_per_cctrch_config_t ;


/** UL DPCH Configuration */
typedef struct
{
  /*! \brief Array of Channel ENC Ping_Pong Flag for Ch0/Ch1 Encoding */
  uint8    encPingPongFlag;

  /*! \brief OVSF Code Channel Configuration in all UL Slot.
  There are up to 2 code channels in each time slot and up to 5 time slots.
  There is a valid bit in each code channel to indicate if the physical
  channel is present. The sequence of the physical channel shall match to
  the initial DPCH channel configuration. E.g. if there are two CCTrCh with
  one physical channel mapped to each CCTrCH in the initial DPCH configuration,
  the physical channel, after RM, shall update info in place.
  */
  tfw_ul_dpch_per_phy_config_t phy[TFW_MAX_UL_SLOT][TFW_MAX_UL_PHY_PER_SLOT];
  /*! \brief UL CCTrCH configuration. The first CCTrCH corresponds to the first
  CCTrCH in TX Encode and the 2nd CCTrCH record corresponds to the 2nd CCTrCH in
  the TX Encode 
  */
  tfw_ul_dpch_per_cctrch_config_t ccTrch[TFW_MAX_UL_CCTRCH_NUM];
} tfw_ul_dpch_config_t ;

/** UL E-PUCH Static Configuration */
typedef struct
{

  /*! \brief Step size for the adjustment of Uplink transmission timing.
  May not be needed if available from DPCH Valid [1..8] in 1/8 chip step */
  uint8   timeAdjStep;
  /*! \brief Frequency of the adjustment of the uplink transmission timing.
  May not be needed if available from DPCH. Valid [1..8] in subframe */
  uint8   timeAdjFreqM;

} tfw_eul_config_t ;

/** UL E-PUCH & E-UCCH Configuration */
typedef struct
{
  /*! \brief Time Slots that are used for E-PUCH.*/
  /** Valid : Bit[0..4] represents Time Slot [1..5]
      \li 1 : Time Slot is used for E-PUCH
      \li 0 : Time Slot is not used
      */
  uint16  timeSlotBitMask : 5;
  /*! \brief Number of time slots that contains E-PUCH. Valid [1..5] */
  uint16  numTs : 3;
  /*! \brief Parameter used to calc number of instances for E-UCCH. 
  truncate(numInstance/numTs)  */
  uint16  eucchParamK : 4;
  /*! \brief Parameter used to calc number of instances for E-UCCH. */
  uint16  eucchParamL : 3;
  /*! \brief E-PUCH scheduled type. Valid[0,1] 0: non scheduled, 1: scheduled*/
  uint16  scheduleType: 1;
   
  /** OVSF Code. Valid [1..16] */
  uint16  chanCode : 8;
  /** Spreading factor for EPUCH. Valid value [1,2,4,8,16] */
  uint16  sf : 5;
  uint16  : 1;
  /*! \brief HARQ Process ID. Valid [0..3] */
  uint16  harqId : 2;

  /*! \brief Transport Block Size */
  uint16  tbs : 6;
  /*! \brief Retransmission Sequence Number. Valid [0..3] */
  uint16  rsn : 2;
  /*! \brief EHICH Channel that is paired for this E-PUCH. Valid [0..3] for
  scheduled. Valid[4] for non-scheduled */
  uint16  ei : 8;

  /*! \brief TX gain for E-PUCH. */
  /*! Code Channel power in this slot w/ <TBD> 1/512 dB resolution, ie. 9 
  fractional bits. It should include total power for E-PUCH and also include 
  compensation for pathloss,if any */
  int16   epuchTxPwr;
  
  /*! \brief Absolute Midamble Shift to be used for E-PUCH in each slot[1..5] 
  Valid [0, 128/k, 2*(128/k), .., (k-1)*(128/k) ], where K is midamble 
  configuration [2,4,6,8,10,12,14,16] when the correspoding bit (slot) in 
  timeSlotBitMask above is set */
  uint8   midambleShift[5];

  /*! \brief Modulation type: 0 (QPSK) or 1 (16QAM) */
  uint8   modType : 1;
  /*! \brief FW write RmRAM Bank index flag, 0: SW write; 1:FW write */
  uint8   fwWriteRmramBankFlag : 1;
  /*! \brief RmRAM Bank Index for next sub frame, 0 or 1 */
  uint8  rmramBankIdx : 1;
  /*! \brief  5 LSB of sub frame number when this command is sent to FW */
  uint8  curr_sub_fn : 5;
  /*! \brief  EUL mod ctl value sent to FW */
  uint32 modCtlValue;

  /*! \brief Array of Channel ENC Ping_Pong Flag for Ch0/Ch1 Encoding */
  uint8    encPingPongFlag;

  /*! \brief WAV_IB Channel Encoder for EPUCH configuration */
  tfw_wavib_t   encConfig[TFW_MAX_UL_SLOT];

} tfw_epuch_config_t ;

/** Tx mempool logging Configuration */
typedef struct {
  /*! \brief Logging ENABLE/DISABLE 
  @verbatim
   0: Disable
   1: Enable
   Takes effect at the beginning of next subframe.
  @endverbatim 
  */
  boolean          enableFlag;
  /*! \brief number of subframes to log
  @verbatim
   -1: log continuously until disabled.
   >0: log these many subframes and stop automatically thereafter.
  @endverbatim 
  */
  int32          nSubframes;
  /*! \brief Number of samples to log in a subframe @ cx1
  @verbatim
   valid values: >0, <2*864
  @endverbatim 
  */
  uint32         nSamples : 16;
  /*! \brief Crash at end of subframe
  @verbatim
     Flag to force a crash at the end of the subframe
  @endverbatim 
  */
  uint32         crashFlag : 1;
  /*! \brief which TS to log UP3 input
  @verbatim
   0:   Enable UP3 input logging for UPPCH slot
   1-5: Enable UP3 input logging for TSN
  @endverbatim 
  */
  uint32         reflogTS : 3;
  /*! \brief Logging point selection
  @verbatim
   0: TxR0 output (logging rate cx22.5; downsampled by 4 @ cx90)
   1: TxC0 input (logging rate cx8)
  @endverbatim 
  */
  uint32         logPoint : 3;
  uint32         : 3;

  /*! \brief Logging start offset in cx1 
  @verbatim
   0:  Log from the beginning of the transmission
   >0: Logging starts after nSamplesStartOffset cx1 samples from
       the beginning of the transmission in a subframe.
       max value permissible <5*864
  @endverbatim 
  */
  uint16         nSamplesStartOffset;
  /*! \brief Mask to specify which TS to log WAV output
  @verbatim
    Mask: 0b111111 logs all TS 
          0b000001 logs UPPCH slot
          0b100000 logs TS5
          0b000000 logs no TS
  @endverbatim 
  */
  uint32         wavoutTSMask : 6;
  /*! \brief start subframe number 
  @verbatim
   <2^13:  start logging at the given subframe number
   >=2^13: (invalid subframe number,) start immediately
  @endverbatim 
  */
  uint32         nStartSf : 16;
} tfw_tx_mempool_logging_cfg_t;

/*\}*/

/** DL flow control*/
typedef struct{
  /*! \brief hdsdpa flow control rtbs cap table.
  @verbatim
  RTBS CAP table will be used to cap the RTBS when FC is turned on
  and valid only when it is an on command (TDSRLC_FC_FW_CMD_ON). SW 
  need to make sure table content are within the range [1,63]
  @endverbatim 
  */
  uint8          rtbsCapTable[TFW_RTBS_CAP_TABLE_SIZE];
  /*! \brief hdsdpa flow control rtbs cap index
  @verbatim
  RTBS CAP index will be applied to RTBS CAP table when DL FC is on.
  the index need to be within range [0,TFW_RTBS_CAP_TABLE_SIZE-1]
  @endverbatim 
  */
  uint16         rtbsCapIndex;
  /*! \brief hdsdpa flow control type
  @verbatim
  TDSRLC_FC_FW_CMD_OFF = 0 turn off HSDPA FC
  TDSRLC_FC_FW_CMD_ON  = 1 turn on HSDPA FC
  @endverbatim 
  */
  uint16         rlcFcFwCmd : 1;
  uint16         : 15;
} tfw_hsdpa_flow_control_t;



  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR TSCDMA FW Operational State command           */
  /*-----------------------------------------------------------------------*/
/**
    \defgroup fMiscellaneous Miscellaneous Commands & Responses */
/*\{*/

/*\}*/

/**
    \defgroup fStateConfig STATE_CFG Command & Response 
    \ingroup fMiscellaneous
     */
/*\{*/

/** TFW State Definition. */
typedef enum {
  TFW_STATE_INACTIVE = 0,
  TFW_STATE_STANDBY,
  TFW_STATE_ACQUISITION,
  TFW_STATE_TRACKING,
  TFW_STATE_FTM,
  TFW_STATE_FTM_ACQ = TFW_STATE_FTM,
  TFW_STATE_FTM_TRK,
  TFW_STATE_FTM_CX8,
  TFW_STATE_IRAT,
  TFW_STATE_TRACK_IDLE,
  TFW_STATE_FTM_CAL,
  TFW_STATE_INVALID
} tfw_state_t;

/** STATE_CFG command & response. Firmware sends a response
    (tfw_state_cfg_rsp_t) after processing the message if
    TFW App is enabled through the message. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type       hdr;
  /*! \brief Set new state of TFW. */
  tfw_state_t                state;
  /*! \brief Serving Cell info when entering traffic state. 
  @verbatim 
   1) tfw_serving_cell_cfg_t defined in tfw_sw_intf_cell.h   
   2) only checked when entering traffic state 
  @endverbatim 
  */
  tfw_serving_cell_cfg_t     servingCell;

  /*! \brief SW indication that FW is in LTA gap
  @verbatim 
  SW will set the LTA indication flag to true when state transition is requested due to LTA. 
  In all other Cases, SW should send the flag to false;
  @endverbatim 
  */
  boolean     ltaInd;
} tfw_state_cfg_cmd_t; 

/** Response to STATE_CFG command.   
    \sa tfw_state_cfg_cmd_t.  
 */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  tfw_state_t             state;
  // uint16                    hdrVersion;
  /** Physical address of the shared memory interface. \sa hdrfw_shared_memory_interface_t */
  // uint32                    sharedMemoryAddr;           

} tfw_state_cfg_rsp_t;
/*\}*/

/** Response to ANTENNA_SWITCH_CFG command.   
    \sa tfw_state_cfg_cmd_t.  
 */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  uint32                    switchStatus;           
} tfw_antenna_switching_config_rsp_t;
/*\}*/

/**
    \defgroup fError Detection Info and Indication
    \ingroup fMiscellaneous
     */
/*\{*/
/*! @brief DETECTED ERROR INFO
 *
 *   This is the information w.r.t to detected errors
 */
typedef struct {
  /*! \brief Wall Time when error ind is sent */
  uint32    wallTime;
  /*! \brief Error Bit Mask <TBD> */
  /** Error Bit Mask   */
#define TFW_ERROR_INFO_DM_BRIDGE_STUCK_BIT      0
  uint32    errorBitMask;
} tfw_error_info_t;                     


/*! @brief ERROR_DETECTED Indication
 *
 *   This is the message definition for ERROR_DETECTED Indication
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type             hdr;
  /*! \brief Detected Error Info */
  tfw_error_info_t                 errorInfo;
} tfw_error_detected_ind_t;                     
/*\}*/


/**
    \defgroup fPanic PANIC Command 
    \ingroup fMiscellaneous
    \brief DEBUG USE only!
     */
/*\{*/
/** PANIC_STOP Command. Upon receving this command, FW will
 *  perform a panic ASSERT */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type       hdr;
  /*! \brief Reason of panic stop. FW will simply log this reason and ASSERT*/
  uint32                     reason;
} tfw_panic_stop_cmd_t ;
/*\}*/

/**
    \defgroup fRxTimingSync RX_TIME_SYNC Command
    \ingroup fMiscellaneous
    */
/*\{*/
/** RX Time Sync Command. Time Offset between Wall Time Counter and RX Time
    Counter. */
typedef struct
{
  /* Word 0*/
  /*! \brief Offset between RX Time counter and Wall Time counter 
  @verbatim
  RX Time Counter = Wall Time Counter - Offset Counter
  where RX Time = W.Z corrsponding to Wall Time = X.Y with Offset = L.M,
  Z, Y and M are chipX8 counts range from 0 to 6400*8-1 and wrap around 6400*8
  W, X and L are subframe numbers range from 0 to 8191 and wrap around 8192

  M = (Y >= Z)? (Y-Z) : (6400*8 + Y -Z);
  borrow = (Y >= Z)? 0 : 1;
  L = (X >= (W + borrow))? (X - W - borrow) : (8192 + X - W - borrow);

  @endverbatim 
  */
  union
  {
    /** sub frame count and chip x8 count */
    struct
    {
      uint32  chipX8cnt : 16;
      uint32  subFrmCnt : 13;
      uint32  : 3;
    };
    /*! \brief Offset counter including both subframe and chipx8 count 
    at begin of TTI, relevent only for blind & S1/S2 PCCPCH */
    uint32  offsetCnt;
  };

  /* Word 1 */
  /*! \brief Indication if freqOffset is valid. Usually set after PCCPCH 
  blind detection. = 1 if freqOffset is valid. = 0, if invalid*/
  uint16  validOffset;
  /*! \brief Winning hypotheses frequency offset in 1 Hz to be applied to
  working frequency in serving cell. Valid only if validOffset
  == 1*/
  int16   freqOffset;

} tfw_rx_time_sync_t;

/** TFW RX Time Sync message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  tfw_rx_time_sync_t    syncCfg;
} tfw_rx_time_sync_cmd_t;

/** TFW Slot Mask Config message command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief The RX subframe number of the radio frame when this message is intended 
  to take place. It is used for error checking. SW still needs to send this msg in the frame before 
  action time.*/
  uint16                    actionTime;
  /*! \brief Sequence number for this message. SW should increment the sequence
  number for each new DMO Slot Mask Config message. Valid range [0..65535] */
  uint16                    seqNum;
  /*! \brief The slot mask that is used to suspend DL and UL active slots.*/
  union
  {
    /** Valid : Bit[0] for TS 0, Bit[1] for Special Slot, Bit[2..7] for TS[1..6]
        \li 1 : Slot is not suspended.
        \li 0 : Slot is suspended.
        */
    uint8       slotMaskPerSubframe[2];
    uint16      slotMaskPerFrame;
  };

} tfw_slot_suspension_config_cmd_t;  
/*\}*/

/** TFW InSync message command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type		hdr;
  /*! \brief The RX subframe number of the radio frame when this message is intended 
  to take place. It is used for error checking. SW still needs to send this msg in the frame before 
  action time.*/
  uint16					actionTime;
  /*! \brief Sequence number for this message. SW should increment the sequence
  number for each new DMO Slot Mask Config message. Valid range [0..65535] */
  uint16					seqNum;
  /*! \brief The slot mask that is used to suspend DL and UL active slots.*/
  boolean					l1InSync;  
} tfw_inSync_config_cmd_t; 

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR TSCDMA SW to FW Channel Config Messages       */
  /*-----------------------------------------------------------------------*/
/**
    \defgroup fChConfig DL Physical Channel Commands
    SW to FW channel configuraiton message commands. Except where it is 
    explicitly specified, the command shall be sent in the frame before the
    frame where new configuration will take place. There is no configuraiton
    window for the command. However, if the command comes in too late, the 
    command will be processed in the next frame and the corresponding 
    configuration will not be effective until the frame after. An indication
    will be sent to SW for error handle */
/*\{*/

/** DL Midamble Configuration message command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Midamble configuration for DL slots */
  tfw_dl_midamble_config_table_t  cfg;
} tfw_dl_midamble_config_table_cmd_t ;

/** TFW DL PICH Config message command.
    This command is sent to detect paging indicator. The command shall be sent
    in the subframe before the subframe that contains indicator.    */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief PICH config */
  tfw_dl_pich_config_t         pichCfg;

} tfw_dl_pich_config_cmd_t;

/** TFW DL PCCPCH Config message command.
    This command is sent to configure PCCPCH. The command shall be sent in the
    frame before the frame new configuration takes place. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief DL PCCPCH config */
  tfw_dl_pccpch_config_t    pccpchCfg;

} tfw_dl_pccpch_config_cmd_t ;

/** TFW DL SCPCCPCH Config message command.
    The command shall be sent in the frame before the frame new SCCPCH
    configuration takes place. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief DL SCCPCH config */
  tfw_dl_sccpch_config_t    sccpchCfg;
  /*! \brief SCCPCH time slot format */
  tfw_dl_slotFormat_info_t  slotInfo;

} tfw_dl_sccpch_config_cmd_t ;

/** TFW DL DPCH Config message command.
    The command shall be sent in the frame before the frame the new
    configuration takes place */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief DL DPCH config */
  tfw_dl_dpch_config_t      dpchCfg;
  /*! \brief Time slot formats for DPCH- Up to 6 DL slots*/
  tfw_dl_slotFormat_info_t   slotInfo[6];

} tfw_dl_dpch_config_cmd_t ;

/** TFW DL HS Channel Config message command. The command shall
    be sent in the frame before the frame the new HS control
    channel configuraiton takes place */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief DL HS channels config */
  tfw_hs_config_t  hsCfg;

} tfw_dl_hs_config_cmd_t ;

/** TFW DL HS Data Channel Config message command.
    The command shall be sent in the frame before the frame the new HS data
    channel configuration takes place */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief DL HS data channel config */
  tfw_hs_dataChan_config_t  hsDataCfg;

} tfw_dl_hs_dataChan_config_cmd_t ;


/** TFW FPACH Channel Config message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief UL Fast Physical Access channel config */
  tfw_uppch_fpach_config_t  fpachCfg;

} tfw_fpach_config_cmd_t ;

/** TFW E-AGCH Channel Config message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief E-DCH Absolute Grant channel config */
  tfw_eagch_config_t  eagchCfg;

} tfw_eagch_config_cmd_t ;

/** TFW E-HICH Channel Config message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief E-DCH HARQ Indication channel config */
  tfw_ehich_config_t  ehichCfg;

} tfw_ehich_config_cmd_t ;

/** TFW non scheduled grant Config message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  tfw_non_schld_ehich_config_t  cfg;
} tfw_non_schld_grant_config_cmd_t ;


/*\}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR TSCDMA FW UL Channel Configuration Message    */
  /*-----------------------------------------------------------------------*/
/**
    \defgroup fULchanCfg UL Physical Channel Configuration
    Message specific to UL operation.  */
/*\{*/

/** TFW UpPCH TX Power and Timing Configuration message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief UpPCH TX Power Config */
  tfw_uppch_tx_pwr_timing_config_t     cfg;

} tfw_uppch_tx_pwr_timing_config_cmd_t ;

/** TFW PRACH Channel Configuration message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief UL Physical Random Access Channel config */
  tfw_prach_config_t        cfg;

} tfw_prach_config_cmd_t ;


/** TFW UL DPCH TX Power and Timing Configuration message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief DPCH TX Power and Timing Config */
  tfw_dpch_tx_pwr_timing_config_t     cfg;

} tfw_dpch_tx_pwr_timing_config_cmd_t ;

/** TFW UL DPCH TX Power and Timing Update message command used after initial
    DPCH config */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief DPCH TX Power and Timing Config */
  tfw_dpch_tx_pwr_timing_update_t     update;

} tfw_dpch_tx_pwr_timing_update_cmd_t ;

/** TFW UL HS-SICH override command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief HS-SICH override Config */
  tfw_hssich_override_t     cfg;

} tfw_hssich_override_cmd_t ;

/** TFW UL DPCH Channel Configuration message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief UL DPCH Configuration.*/
  tfw_ul_dpch_config_t      cfg;

} tfw_ul_dpch_config_cmd_t ;

/** TFW E-RUCCH Channel Configuration message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief UL Physical Random Access Channel config */
  tfw_prach_config_t        cfg;

} tfw_erucch_config_cmd_t ;

/** TFW E-PUCH Static Configuration */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! EUL Midamble and Timing Adjustment Configuration */
  tfw_eul_config_t          cfg;

} tfw_eul_config_cmd_t ;

/** TFW E-PUCH Channel Configuration message command. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief UL E-PUCH Channel Configuration */
  tfw_epuch_config_t        cfg;

} tfw_epuch_config_cmd_t ;

/** TFW UL DPCH TX cancellation message command. This command is sent to
    cancel the DPCH transmission from a tfw_ul_dpch_config_cmd sent in the
    same frame. All code channels and slots that are contained in the UL DPCH
    channel config will not be transmitted. It will be effective in the frame
    after the frame that receives this command and only for one frame. */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} tfw_ul_dpch_cancel_cmd_t ;

/** TFW tx mempool logging Configuration message command. */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type          hdr;
  /*! \brief MemPool Logging config */
  tfw_tx_mempool_logging_cfg_t  cfg;
} tfw_tx_mp_log_config_cmd_t;

/** TX_HW_BLOCK_CONFIG command. This command is used to inform
  the FW about the status of the TX HW block */ 
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief TX HW block enable/disable config */
  uint16                    txBlockEnable;
} tfw_tx_hw_block_config_cmd_t ;
/*\}*/

  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR TSCDMA FW Power Control Related Commands      */
  /*-----------------------------------------------------------------------*/
/**
    \defgroup fPwrCtrl Power Control Related Commands
    Message specific to Power Control. */
/*\{*/

/*\}*/

/**
    \defgroup fPCTargetSir TARGET_SIR_UPDATE Commands
    \ingroup fPwrCtrl
*/
typedef struct
{
  /*! \brief DPCH Target SIR. Valid TBD */
  /** Up to 3 CCTrCH for DL PCH. The first one is mapped to CCTrCH 0. The 2nd
      one is mapped to CCTrCH 1. The 3rd one is mapped to CCTrCH 2. SW needs
      to maintain the latest target SIR for all 3 CCTrCH. When a CCTrCH is
      not used, the corresponding target SIR shall be set to 0. The format is
      Q6.10 in dB, that is 10 fractional bits */
  int16   targetSIR[3];

} tfw_dl_dpch_targetSIR_update_t ;

/*\{*/
/** TFW DL Channel Target SIR Update Message Command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Target SIR Update for DL DPCH Power Control */
  tfw_dl_dpch_targetSIR_update_t update;

} tfw_dl_dpch_targetSIR_update_cmd_t ;

/** TFW HS-SCCH Channel Target SIR Update Message Command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Target SIR Update for HS-SCCH Power Control. Q6.10 in dB, i.e.,
   10 fractional bits */
  int16   targetSIR;
} tfw_hsscch_targetSIR_update_cmd_t ;

/** TFW HS-PDSCH Channel SIR Update Message Command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Flag that initialize the SIR filtering operation. 
  1: Filter needs init , 0: no init */
  uint16  initSet : 1;
  /*! \brief flag to enable or disable OL CQI  */
  uint16 olCqiEnable : 1;
  uint16  : 14;
  /*! \brief Outer loop SIR adjustment factor , Q6.10 in dB */
  int16   sirAdj;
} tfw_hspdsch_sir_update_cmd_t ;

/** TFW E-AGCH Channel Target SIR Update Message Command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Target SIR Update for E-AGCH Power Control. Q6.10 in dB, i.e.
  10 fractional bits. */
  int16   targetSIR;
} tfw_eagch_targetSIR_update_cmd_t ;
/*\}*/

/**
    \defgroup fPCPathLoss PATHLOSS_UPDATE Command
    \ingroup fPwrCtrl
*/
/*\{*/
/** TFW DL Pathloss Update Message Command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Pathloss. Valid [0..(128*16-1)] in 1/16 dB step or
  equivalently 0..127.9375 dB */
  int32                     pathLoss;
} tfw_pathloss_update_cmd_t ;
/*\}*/



/** TFW HSDPA Flow Control Message Command  */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief HSDPA flow control message structure */
  tfw_hsdpa_flow_control_t   dpaFcUpdate;
} tfw_hsdpa_flow_control_cmd_t ;

/*\{*/
/** TFW Antenna switching Command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief Antenna Swiching flag. 
      1 - Execute switching 0 - Disable switching */
  uint32                     switchConfig;
  /*! \brief Registration priority for Cxm. 
       Only valid in DSDA mode */
  uint32                     cxmPriority;
} tfw_antenna_switching_config_cmd_t ;
/*\}*/

/*\{*/
/** TFW AOL ABORT Command */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief AOL ABORT flag. 
      1 - Abort pending AOL */
  uint32                     abortConfig;
} tfw_aol_abort_config_cmd_t ;
/*\}*/

/**
    \defgroup fPCTpcTrace DL_PWRCTRL_TPC_TRACE Command
    \ingroup fPwrCtrl
*/
typedef struct
{
  uint8   updateMask;
  
} tfw_dl_pwrCtrl_tpc_trace_t ;

/*\{*/
/** TFW TPC Update Message for DL Power Control */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief TPC History for DL Power Control */
  tfw_dl_pwrCtrl_tpc_trace_t  tpcTrace;

} tfw_dl_pwrCtrl_tpc_trace_cmd_t ;
/*\}*/

/**
    \defgroup fPCTpcInd UL_TPC_UPDATE Indication
    \ingroup fPwrCtrl
    */
/*\{*/
/** Indication to SW for UL TPC
 */
typedef struct {
  
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
} tfw_ul_tpc_update_ind_t;
/*\}*/


  /*-----------------------------------------------------------------------*/
  /*     DATA STRUCTURES FOR TSCDMA FW Demback Configuration Message       */
  /*-----------------------------------------------------------------------*/
/**
    \defgroup fDembackCfg Demback Configuration Message
    Message specific to Demback operation. Some configuration requires the
    corrsponding channel configuration in Demfront. The message shall be sent
    in the frame before the frame when new configuraiton will take place. */
/*\{*/

/** TFW DL CCTrCH update command. This command shall be sent in the frame
    before the frame the new CCTrCH configuraiton will take place. Up to 3
    CCTrCH can be configured in the same frame */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief DL CCTrCH update */
  tfw_dl_ccTrCh_config_t  ccTrChCfg;

} tfw_dl_ccTrCh_config_cmd_t ;

/** TFW DL Trch update command. This command is used to update dynamic
    parameters of a TrCH in TrCH pool. The update will be effectively after
    command is processed, i.e., there is no action time.   */
typedef struct
{
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  /*! \brief DL TrCH update */
  tfw_dl_trCh_update_config_t   trChUpdateCfg;

} tfw_dl_trCh_update_cmd_t ;

/*\}*/









/**
    \defgroup fSmiFpachResults FPACH Result Buffer
    \ingroup fSharedMemoryInterface
    */
/*\{*/

/** FPACH Decoded Result Indication Data Structure */
typedef struct 
{
  /*! \brief FPACH Status. Valid [TFW_UL_SYNC_NO_ACK, TFW_UL_SYNC_ACK, 
  TFW_UL_SYNC_ABORT] */
  uint8   status;
  /*! \brief Received Signature Reference Number. Valid only if status is ACK*/
  uint8   sigRefNo;
  /** Sequence number for this random access procedure. Valid range [0..255] */
  uint8   txSeqNum;
  /** UL Sync Code. Valid range [0..255] */
  uint8   ulSyncCode;
  /** Sub-frame number to start transmission on UpPCH. Valid [0..8191] */
  uint16  ulSyncTxSubFrm;
  /*! \brief Received Relative Sub-Frame Number. Valid only if status is ACK */
  uint8   relSubFrm;
  /*! \brief Received Transmit Power Level Command for RACH message. 
  Valid only if status is ACK*/
  int8   prxPRACH;
  /*! \brief Received starting position of the UpPCH. Valid [0..8191] in 1/8 
  chip. Valid only if status is ACK */
  uint16  upPchPos;
  /*! \brief Energy Metric for fpach decoding*/
  uint16  energyMetric;
  /*! \brief errorCnt for fpach decoding*/
  uint32  errorCnt : 7;
  uint32  :25;
} tfw_fpach_result_buf_t ;

/** PICH Decoded Result Indication Data Structure */
typedef struct
{
  /** PICH soft symbol. signed number masked to lower 20 bits, i.e.
      Bit[31..20]=0. Bit[19..0] = 20 bit signed number */
  uint32 metric;
  /** Subframe number when (air) PICH is decoded. Valid[0..8191] */
  uint16 subframe;
  /** Subframe number when (air) PICH should be decoded. Valid[0..8191] */
  uint16 actionTime;
  /** Indication if PICH goes through decode. 1: PICH is decode. 0: PICH is
      not decoded */
  uint8  decoded;
  uint8  non_used;
  /** qvalue that is sent to FW for PICH decode */
  uint8  qvalue;
  /** 1 : PI is detected. 0: PI is not detected */
  uint8  pi;
} tfw_pich_result_buf_t ;
/*\}*/


/**
    \defgroup fSmiEagchResults E-AGCH Result Buffer
    \ingroup fSharedMemoryInterface
    */
/*\{*/
/** EAGCH Decoded Results */
typedef struct 
{
  /** Sub frame number in which E-AGCH is received. Valid [0..8191]  */
  uint16  subFrmNum : 13;
  uint16  eagchIdx : 2;
  /** is backoff happen with this grant. 0 stands for no backoff. Valid [0..1]	*/
  uint16  eagchMode : 1; 
  /** the MTPL of timeslot which are backoff-ed. Valid [0..79], 0 map to -55, 79 map to 24 */
  uint16  txBackoffMtplQ4 : 11;
  /** backoff-ed timeslot related info. Valid Bit[4..0] for TS1 to TS5,
  bit4->TS1, bit3->TS2,..,bit0->TS5 */
  uint16  txBackoffBitMask : 5;

  /** Power resource related info */
  uint32  prri : 5;
  /** Code resource related info */
  uint32  crri : 5;
  /** Time slot resource related info. Valid Bit[4..0] for TS1 to TS5,
      bit4->TS1, bit3->TS2,..,bit0->TS5 */
  uint32  trri : 5;
  /** E-DCH cyclic sequence number. Valid [0..7] */
  uint32  ecsn : 3;
  /** Resource Duartion Indication if RDI is present. */
  uint32  durationInd : 3;
  /** E-HICH Indicator. Valid [0..3] */
  uint32  ei : 2;
  /** E-UCCH Number Indicator. Valid [0..7] */
  uint32  eni : 3;
  /** TPC to control E-PUCH */
  uint32  tpc : 2;
  /** SS to control E-PUCH */
  uint32  ss : 2;
  uint32  : 2;

  /** EAGCH Decode Status. May not be needed */
  uint16  : 4;
  uint16  errorCnt : 7;
  /** The good/bad quality bit for coded data for E-AGCH */
  uint16  qualityBit : 1;
  uint16  : 4;
  /** Sign-extended value of the accumulated energy metric for coded data for
    E-AGCH */
  int16   energy_metric : 16;
} tfw_eagch_result_buf_t ;
/*\}*/


/**
    \defgroup fSmiEhichNonSchedResult E-HICH Non-Scheduled Result Buffer
    \ingroup fSharedMemoryInterface
    */
/*\{*/
/** Non scheduled E-HICH Result Indication Data Structure. SW
    should check corresponding counter to know if there is
    updated info */
typedef struct 
{
  /*! \brief Subframe number when corresponding E-PUCH is transmitted.
  Valid [0,8191]. When subFrameNum is not valid, the info below is also invalid */
  uint16  subFrameNumEpuch;
  /*! \brief Subframe number when corresponding E-HICH is transmitted.
  Valid [0,8191]. When subFrameNum is not valid, the info below is also invalid */
  uint16  subFrameNumEhich;
  /*! \brief Ack or Nack for the corresponding E-PUCH. Valid [0,1], 
  0: Nack, 1: ACK*/
  uint16   ackNack : 1;
  /*! \brief Nack overwrite indication due to DMO. Valid [0,1], 
  0: No Nack Overwrite, 1: NACK overwrite*/
  uint16   nackOverwriteInd : 1;
  uint16   : 14;
  /*! \brief TPC Value. Only valid if txScheduledType indicates it is 
  non-scheduled transmission. Valid [0,1,2], 0: Down, 1: Up, 2: hold */
  uint16  tpc : 2 ;
  /*! \brief SS Value. Only valid if txScheduledType indicates it is 
  non-scheduled transmission. Valid [0,1,2], 0: Down, 1: Up, 2: hold */
  uint16  ss : 2 ;
  uint16  : 12 ;

} tfw_non_schld_ehich_result_buf_t ;
/*\}*/


/**
    \defgroup fSmiEHICHResult E-HICH Result Buffer
    \ingroup fSharedMemoryInterface
    */
/*\{*/

/** Scheduled E-HICH Result Indication Data Structure. SW should read a
    corresponding counter to know if there is info update */
typedef struct
{
  /*! \brief Subframe number when corresponding E-PUCH is transmitted.
  Valid [0,8191]. When subFrameNum is not valid, the info below is also invalid */
  uint16  subFrameNumEpuch;
  /*! \brief Subframe number when corresponding E-HICH is transmitted.
  Valid [0,8191]. When subFrameNum is not valid, the info below is also invalid */
  uint16  subFrameNumEhich;
  /*! \brief EI for E-HICH Index, valid: [0..3] */
  uint16  ehichIndex;
  /*! \brief Ack or Nack for the corresponding E-PUCH. Valid [0,1], 
  0: Nack, 1: ACK*/
  uint16   ackNack : 1;
  /*! \brief Nack overwrite indication due to DMO. Valid [0,1], 
  0: No Nack Overwrite, 1: NACK overwrite*/
  uint16   nackOverwriteInd : 1;
  /*!if the according EPUCH slots are backed off in DSDA mode, this flag is set to 1, 
        0 is no backoff */
  uint16   epuchBackoffFlag : 1;
  uint16   : 13;

} tfw_schld_ehich_result_buf_t ;
/*\}*/


/**
    \defgroup fSmiSCCHResult HS-SCCH Result Buffer
    \ingroup fSharedMemoryInterface
    */
/*\{*/

/** HS SCCH Decoded bits. Only Type 1 is specified */
typedef struct ALIGN(8)
{
  union
  {
    /** Decoded bits including CRC attachment */
    /** Only lower 48 bits are valid in Type 1 */
    uint64  rawDecodedBits ALIGN(8);
    struct 
    {
      uint32  rawDecodedBitsData0;
      uint32  rawDecodedBitsData1;
    };
    struct 
    {
      /*! Bit fields refer to 20.222, type1 HS-SCCH*/
      /*! Word 0 */
      uint32  coveredCrc : 16;
      /*! HS-SCCH Cyclic Sequence Number [0..7]*/
      uint32  hcsn : 3;
      /*! New data indicator */
      uint32  nd : 1;
      /*! Redundancy and constellaton version */
      uint32  xrv : 3;
      /*! Harq number [0..7]*/
      uint32  hap : 3;
      /*! Transport Block Size Index */
      uint32  tbs : 6;

      /*! Word 1*/
      /*! Modulation scheme [0] QPSK or 64QAM, [1] 16QAM*/
      uint32  ms : 1;
      /*! Time slot information for HSPDSCH BIT[4..0] for [TS2..TS6] */
      uint32  ts : 5;
      /*! PDSCH OVSF Channel Stop */
      uint32  kStop : 4;
      /*! PDSCH OVSF Channel Start*/
      uint32  kStart : 4;
      uint32  : 18;
    } type1;
    /*! Other types are not supported */
  };
} tfw_hsscch_decoded_bits_t;

#define TFW_HS_SCCH_DECODE_RESULT_SUCCESS       1
#define TFW_HS_SCCH_DECODE_RESULT_FAIL          0

typedef struct
{
  /** Subframe number where HS-SICH needs to be scheduled. Valid [0..8191]. Need
      to match current subframe that HS-SICH is scheduled to transmit. If
      Invalid bit is set, HSSICH is not transmitted in the subframe that is
      specified here */
  uint16          hssichAirSfm : 13 ;
  /** invalid indicator. 0: HS-SICH is transmitted. 1: HS-SICH is not
      transmitted */
  uint16          invalid : 1;
  uint16          : 2;
  /* last HS-SICH channel power, 16Q9, valid only if invalid above == 0 */
  int16           hssichTxPwr;
  /* Time Slot number where HS-SICH needs to be scheduled. valid only if invalid above == 0  
     Valid [1 to 5] */
  uint8           timeSlot;
  /* OVSF code number. valid: [0..sf-1], valid only if invalid above == 0 */
  uint8           chanCode;
}tfw_hssich_for_upa_t;

/** HS-SCCH Decode Result Indication Data Structure. SW should read a
    corresponding counter to know if there is info update */
typedef struct
{
  /** HS-SCCH Decoded info */
  tfw_hsscch_decoded_bits_t   data;

  /** HS SCCH Decode Status. */
  /** Valid 
   \li TFW_HS_SCCH_DECODE_RESULT_SUCCESS : winning HS-SCCH has 
       valid decoded content below
   \li TFW_HS_SCCH_DECODE_RESULT_FAIL : no winning HS-SCCH
   */
  /** subframe for HSSCCH over the air. Valid [0..8191] */
  uint16  subframe;
  uint8   scchDecodeStatus;
  /** Harq process ID. Valid[0..7] */
  uint8   harqProcessId;
  /** Time Slot that contains HS-PDSCH. Valid Bit[4..0] Bit4:TS2.. Bit0:TS6 */
  uint8   tsInfo;
  /** Not used. Only Type 1 is supported */
  uint8   hsscchType;
  /** The HS-SCCH Index that has pass CRC. Valid only if scchDecodeStatus
      indicates CRC has passed . Valid [0..3]*/
  uint8   winningId;
  /** Additional information from HS-SCCH decode when CRC fails . */
/* make sure bit matches definition */
#define TFW_HS_SCCH_INVALID_PARAM_CHAN_START_STOP_BIT     0
#define TFW_HS_SCCH_INVALID_PARAM_TBS_BIT                 1
#define TFW_HS_SCCH_INVALID_PARAM_TS_BIT                  2
#define TFW_HS_SCCH_INVALID_PARAM_HARQ_BIT                3
#define TFW_HS_SCCH_INVALID_PARAM_SF1_BIT                 4
#define TFW_HS_SCCH_INVALID_PARAM_UL_CONFLICT             5

  /** Invalid Chan Start or Stop, Bit[0] Invalid TBS. Bit[1], Invalid TS
      Bit[2]. Bit[3], invalid Harq ID 1: invalid, 0: valid */
  uint8   extraInfo;
  /*! \brief Partial TTI has been de-sensed. 0: No de-sensed
  i.e. no slots in a TTI has been de-sensed by multi-sim 1: Partial de-sensed, 
  i.e. at least one slot in a TTI has been de-sensed by multi-sim .  */
  uint8   deSensedInd;
  tfw_hssich_for_upa_t    hssichInfo;
} tfw_scch_result_buf_t ;
/*\}*/


/**
    \defgroup fSmiDlDpchSir DL DPCH SIR Result Buffer
    \ingroup fSharedMemoryInterface
    */
/*\{*/

typedef struct
{
  /* Word 0*/
  /** Indication for DTM. Valid[0..1]. 0: means a DTM frame off. The rest of
      info shall be ignored. 1: means a DTM frame on. The rest of info is
      valid. */
  uint32  dtmInd : 1;
  /** Indication for Special Burst. 0: means no special burst is detected in
      this radio frame. 1: means special burst is detected. */
  uint32  sbInd : 1;
  /*! \brief Indication for strong Special Burst. 0:means weak special burst. 
  1: means strong special burst  */
  uint32  sbRating : 1;
  /*! \brief Partial TTI has been de-sensed. 0: No de-sensed
  i.e. no slots in a TTI has been de-sensed by multi-sim 1: Partial de-sensed, 
  i.e. at least one slot in a TTI has been de-sensed by multi-sim .  */
  uint32  deSensedInd : 1;
   /*! \brief Punctured Indication From Slot Suspenstion. 0: No puncture
  i.e. no slots in the frame has been punctured by slot suspension. 1: puncture, 
  i.e. at least one slot in the frame has been punctured from slot suspension.  */
  uint32  puncturedInd : 1;
  uint32  : 3;
  /*! \brief SW CCTrCH index Valid range : ?? Note that
  this is the same as in CCTRCH reported in DOB for SW Table ID purpose. 
  FW does not modify*/
  uint32  swCcTrChIndex : 8;
  /** Subframe number for the radio frame when SIR status is based upon. Valid
      range [0..8191]. */
  uint32  timeStamp: 16 ;

  /* Word 1*/
  /*! \brief SIR Update. Q6.10 in dB, i.e. 10 fractional bits. If sbInd == 1,
  SIR is for Special burst. Otherwise, it is for data burst */
  int16   sir;
  /*! \brief Decoded TFCI in this frame. If value is greater than 128, 
  it indicates TFCI is out of range or invalid */
  uint16  tfci;
} tfw_dl_dpch_sir_per_cctrch_t;

typedef struct
{
  /** DPCH SIR status. Up to 3 CCTRCH. Note that the sequence, or index of
      cctrch in this status report is the same as in Demback cctrch. It is a
      one to one mapping.  */
  tfw_dl_dpch_sir_per_cctrch_t  cctrch[3];
} tfw_dl_dpch_sir_status_t ;

/** DL DPCH SIR Result Data Structure */
typedef struct
{
  /** FW will use buffer w/ index 0 to update status in odd frames based on SIR
      from the previous (even) frame, and use buffer w/ index 1 to update status
      in odd frames. SW access will be the opposite of FW access. That is, in
      even frames (RX Time), SW can access buffer w/ index 0, and access buffer
      w/ index 1 for odd frames. That is, in RX time, frame n+1, FW will write
      to buffer frame[(n & 0x1)] and SW can read buffer frame[(n-1) & 0x1] */
  tfw_dl_dpch_sir_status_t    frame[2];
} tfw_dl_dpch_sir_result_buf_t ;

/** Structure to process TX HW BLOCK status */
typedef struct 
{
    /** Current Tx block enabled/disabled status*/
    uint16 enabled;
}tfw_ul_hw_cfg_tx_block_enabled_t;
/*\}*/

/** Structure to pass ENC_IB and WAV_IB offsets from WAV uK to SW */
typedef struct
{
  /** Flag that indicates if offsets are valid */
  uint8 valid;
  /** BYTE base addr of AXI LMEM0 interface: MODEM_LMEM_RAM_START */
  uint32 vpe0_lmem_start_addr;
  /** BYTE offset of ENC_IB */
  uint32 enc_ib_start_offset;
  /** BYTE offset of WAV_IB */
  uint32 wav_ib_start_offset;
  /** Size in BYTEs of UPPTS */
  uint32 wav_ib_uppts_size;
  /** Size in BYTEs of CTL (one instance) */
  uint32 wav_ib_ctl_size;
  /** Size in BYTEs of DPCH (one instance) */
  uint32 wav_ib_dpch_size;
  /** Size in BYTEs of ERUCCH (one instance) */
  uint32 wav_ib_erucch_size;
  /** Size in BYTEs of EPUCH (one instance) */
  uint32 wav_ib_epuch_size;
} tfw_vpe0_lmem_map_t ;
/*\}*/



/**
    \defgroup fSharedMemoryInterface Shared Memory Interface
    This section contains information for the shared memory used between Software and Firmware.
    */
/*\{*/
/** Various Info Update flags */
typedef enum
{
  /** DL Channel Decode Done Flags. */
  TFW_PICH_DEC_DONE_FLAG,
  TFW_FPACH_DEC_DONE_FLAG,
  TFW_NONHS_DEC_DONE_FLAG,
  TFW_HS_DEC_DONE_FLAG,
  TFW_HS_SCCH_DEC_DONE_FLAG,
  TFW_EAGCH_DEC_DONE_FLAG,
  TFW_SCHLD_EHICH_DEC_DONE_FLAG,
  TFW_NON_SCHLD_EHICH_DEC_DONE_FLAG,
  /** Power related update flags */
  TFW_UL_PWR_INFO_UPDATE_FLAG,
  /** Misc info */
  TFW_SW_CMD_ERROR_FLAG,
  TFW_NONHS_CCTRCH0_NOTHING_TO_UPDATE_FLAG,
  TFW_NONHS_CCTRCH1_NOTHING_TO_UPDATE_FLAG,
  TFW_NONHS_CCTRCH2_NOTHING_TO_UPDATE_FLAG,
  TFW_INFO_UPDATE_FLAGS
} tfw_info_update_flags_e;


/* max step0 sample buf size in byte (1 sfm + alpha) - 7680*10/8 = 9600 */
#define TFW_PBS_MAX_STEP0_SAMPLE_BUFF_SZ  9600
/* max step12 sample buf size in byte (10 sfm) - 6400*10*10/8 = 80000 */
#define TFW_PBS_MAX_STEP1_SAMPLE_BUFF_SZ  80000 
/* max trk sample buf size in byte - 36 frames worth of samples = (6400*2*2*36*20pcfl/8) = 2304000 */
#define TFW_PBS_MAX_TRK_SAMPLE_BUFF_SZ  (36*2*12800*20 / 8)

typedef struct
{
  /** Step0 IQ sample buffer address for ant0, ant1 */
  uint32 step0SampleAddr[2];
  /** Step0 IQ sample length in byte for both ant0, ant1.  */
  uint32 step0SampleLen;
  /** Step12 IQ sample buffer addres for ant0, ant1. */
  uint32 step12SampleAddr[2];
  /** Step12 IQ sample length in byte for both ant0, ant1.  */
  uint32 step12SampleLen;
  /** Step12 IQ sample buffer addres for ant0, ant1. */
  uint32 trkSampleAddr[2];
  /** Step12 IQ sample length in byte for both ant0, ant1.  */
  uint32 trkSampleLen;
} tfw_ext_iq_buff_t;

/** Overall SHARED MEMORY INTERFACE data structure   */
typedef struct 
{
  /*! \brief SW Write and FW read buffer */
  /** Makign the 1st buffer cache aligned */
  struct ALIGN(32)  
  {
    /** NonHS TFC Pool Table */
    tfw_tfc_pool_table_t        tfwNonHsTFCPoolTable;
    /** NonHS TF Pool Table */
    tfw_tf_pool_table_t         tfwNonHsTFPoolTable;
    /** NonHs TrCh Info Pool Table */
    tfw_trch_info_pool_t        tfwNonHsTrchPoolTable;
    /* NonHS Dob Status Fifo read index */
    uint16  tfwNonHsDobStatFifoReadIndex;
    /* HS Dob Status Fifo read index */
    uint16  tfwHsDobStatFifoReadIndex;
    /** Pathloss in dB Q4 */
    uint16 pathLoss;
    /** SW busy flag */
    boolean swSmemBusyFlag;
    /** Only for CCS IQ Streaming: Pointers of IQ sample buffers */
    tfw_ext_iq_buff_t extIqBuff;
  };
  
  /* unused buffer for cache operation purpose */
  struct ALIGN(32)
  {
    uint32  dummyBuf[8];
  };
  
  //Need some buffer alignment here for cache
  /*! \brief FW Write and SW read buffer */
  /** Makign the 1st buffer cache aligned */
  struct ALIGN(32)
  {
    /** TFW HS DOB Status FIFO */
    /** Check tfwInfoUpdateFlagCounter[TFW_HS_DEC_DONE_FLAG] */
    tfw_hs_dob_status_fifo_t      tfwHsDobStatusFifo;  
    /** TFW HS SCCH Decode Result Buffer */
    tfw_scch_result_buf_t         tfwHsScchResultInd;
    /** TFW Non HS DOB Status FIFO */
    /** Check tfwInfoUpdateFlagCounter[TFW_NONHS_DEC_DONE_FLAG] */
    tfw_nonHS_dob_status_fifo_t   tfwNonHSDobStatusFifo;

    /** TFW DL DPCH SIR Status FIFO */
    tfw_dl_dpch_sir_result_buf_t  tfwDlDpchSirStatusInfo;

    /** Power info for power headroom calc in HSUPA */
    /** Check tfwInfoUpdateFlagCounter[TFW_UL_PWR_INFO_UPDATE_FLAG] */
    tfw_ul_pwr_info_t             tfwUlPwrInfo;

    /** Timing info for UL relative to corresponding DL slots */
    tfw_ul_timing_info_t          tfwUlTimingAdvInfo;

    /** TFW EAGCH Decode Result Buffer  */
    /** Check tfwInfoUpdateFlagCounter[TFW_EAGCH_DEC_DONE_FLAG] */
    tfw_eagch_result_buf_t        tfwEagchResultInd;

    /**  TFW Scheduled EHICH Decode Result Buffer */
    /** Check tfwInfoUpdateFlagCounter[TFW_SCHLD_EHICH_DEC_DONE_FLAG] */
    tfw_schld_ehich_result_buf_t  tfwSchldEhichResultInd[2];

    /**  TFW Non Scheduled EHICH Decode Result Buffer */
    /** Check tfwInfoUpdateFlagCounter[TFW_NON_SCHLD_EHICH_DEC_DONE_FLAG] */
    tfw_non_schld_ehich_result_buf_t    tfwNonSchldEhichResultInd;

    /** TFW FPACH Decode Result Buffer */ 
    /** Check tfwInfoUpdateFlagCounter[TFW_FPACH_DEC_DONE_FLAG] */
    tfw_fpach_result_buf_t        tfwFpachResultInd;

    /** TFW PICH Decode Result Buffer */
    /** Check tfwInfoUpdateFlagCounter[TFW_PICH_DEC_DONE_FLAG] */
    tfw_pich_result_buf_t         tfwPichResultInd;

    /*! \brief Misc proc done or information update flags counters. All counters 
    shall start with 0 upon TFW application enable. When there is an update, 
    the counter value will be incremented by one. SW can use this to determine 
    what information in shared memory has been udpated. */
    uint32  tfwInfoUpdateFlagCounter[TFW_INFO_UPDATE_FLAGS];
    /*! \brief RF AGC log*/
    tfw_agc_result_buf_t tfwAgcResult;
    /*! \brief RF calibration results */
    tfw_rf_sm_ftm_slow_cal_results_t ftmSlowCalResults;
    /*! \brief reAcq RF Ready Flag */
    boolean reAcqRfReady;
    /*! \brief reAcq RF Ready Flag */
    boolean reAcqLatched;
    /*! \brief FW SMEM busy flag */
    boolean fwSmemBusyFlag;
    /*! \brief ENC_IB and WAV_IB offsets in LMEM returned by WAV uK */
    tfw_vpe0_lmem_map_t tfwEncLmemOffsets;
    /*! \brief TFW - WLAN COEX Average Noise estimate */
    int32 tfwWlanCoexMetric;
    /*! \brief TFW - WLAN COEX Average Noise Valid status */
    uint32 tfwWlanCoexMetricValid;
    /*! \brief TFW - WLAN COEX Average Noise active TS over with it was averaged */
    uint32 tfwWlanCoexMetricTSCount;
  };

  tfw_ul_hw_cfg_tx_block_enabled_t tdsTxHWBlock;

} tfw_sw_intf_t ;
/*\}*/

/**
 *  \defgroup FUnionCmdIndRsp Union of Commands, Indications, & Responses */   
/*\{*/


/*! \brief Union of all indications sent to software */
typedef union {
  msgr_hdr_struct_type                   hdr;
  tfw_ul_tpc_update_ind_t                ulTpcUdtInd;
  tfw_cell_info_ind_t                    cellInfoInd;
  tfw_error_detected_ind_t               errorDetectedInd;
  tfw_ard_ind_t                          ardInd;
  tfw_dsds_cleanup_ind_t                 dsdsInd;
  tfw_wlan_txfrmdnl_ind_t                wlanTxfrmdnlInd;
} tfw_msg_ind_u;

/*! \brief Union of all responses sent to software */
typedef union {
  msgr_hdr_struct_type                   hdr;
  tfw_state_cfg_rsp_t                    stateCfgRsp;
  tfw_srch_power_scan_rsp_t              srchPowerScanRsp;       
  tfw_srch_gap_detect_rsp_t              srchGapDetectRsp;
  tfw_srch_syncdl_midamble_detect_rsp_t  srchSyncdlMidambleDetectRsp;
  tfw_srch_reacq_rsp_t                   srchReacqRsp; 
  tfw_measure_rscp_rsp_t                 measureRscpRsp;
  tfw_measure_iscp_rsp_t                 measureIscpRsp;
  tfw_measure_all_rscp_rsp_t             measureAllRscpRsp;
  tfw_antenna_switching_config_rsp_t     antennaSwitchingCfgRsp;
  tfw_iq_capture_rsp_t                   iqCaptureRsp;
  tfw_irat_t2x_cleanup_rsp_t             iratT2xCleanupRsp;
  tfw_ftm_rx_cmd_rsp_t                   ftmRxCmdRsp;
  tfw_ftm_tx_cmd_rsp_t                   ftmTxCmdRsp;
  tfw_ftm_get_multi_synth_state_rsp_t    ftmMultiSynthStateRsp;
  } tfw_msg_rsp_u;

#define TFW_MAX_IND_MSG_SIZE sizeof(tfw_msg_ind_u)
#define TFW_MAX_RSP_MSG_SIZE sizeof(tfw_msg_rsp_u)

#define TFW_NUM_CMD_MSG       (TFW_CMD_LAST - TFW_CMD_FIRST + 1)
#define TFW_NUM_IND_MSG       (TFW_IND_LAST - TFW_IND_FIRST + 1)
#define TFW_NUM_RSP_MSG       (TFW_RSP_LAST - TFW_RSP_FIRST + 1)
/*\}*/


#endif /* TFW_SW_INTF_MSG_H */

