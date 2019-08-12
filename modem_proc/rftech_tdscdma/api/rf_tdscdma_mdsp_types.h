#ifndef RF_TDSCDMA_MDSP_TYPES_H
#define RF_TDSCDMA_MDSP_TYPES_H

/*! 
  @file
  rf_tdscdma_mdsp_types.h
 
  @brief
  This module defines the 1xHDR common defintions used by TDSCDMA RF-MDSP
  Internal(RF) interface.
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Technologies Proprietary

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma_mdsp_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/29/14   ych     Updated for 8 PA gain states 
12/18/13   jps     Add new TFW state
10/10/13   jyu     Integrated with DM interface
09/10/13   qzh     Add command FTM_TDSCDMA_BER_TPC_CONTROL_REQ to support TPC override feature
           kb      [XPT] Added new message type and enum to support switch to new waveform for DPD calibration
06/03/13   nsh	   Add Support for Get Multi Synth in Dime
05/09/13   jyu     Removed tfw_rf_sm_intf_smps_mode_e
02/26/13   ms      Add freq for dyn update
02/22/13   jyu     NikeL merge
12/12/12   zg      Added IQ capture support
09/10/12   ms      Diversity support 
04/14/12   ycl     Added RxAGC threshold. 
02/14/12   ycl     Added support for dyn update for LNA swpts.
01/19/12   jyu     Added support to pupulate HDET/therm write/read scripts 
12/20/11   ycl     Update Function Prototype.
09/21/11   ycl     Add HDET override flag.
09/06/11   ycl     Chnage to match TFW interface.
08/23/11   ycl     Include common MSG file. 
08/15/11   ycl     RF-MDSP Interface types redifined.  
06/23/11   ycl     Initial Revision. 

==============================================================================*/

#include "comdef.h"
#include "rfnv_tdscdma.h"
#include "rfcommon_msg.h"
#include "rfnv_tdscdma.h"
#include "tfw_sw_intf_msg.h"
#ifdef FEATURE_RFA_D3925
#include "tfw_rf_sm_intf.h"
#endif /* #ifdef FEATURE_RFA_D3925 */

#ifdef __cplusplus
extern "C" {
#endif

//The followings are defined in rfnv_tdscdma_h reuse here
//#define RF_TDSCDMA_PATHS_TABLE_SIZ      2
//#define RF_TDSCDMA_MAX_LNA_GAIN_STATES 4
//#define RF_TDSCDMA_NUM_TX_LINEARIZERS 4
//#define RF_TDSCDMA_TX_LIN_MASTER_SIZ 64
//#define RF_TDSCDMA_MAX_CAL_CHAN 16
//#define RF_TDSCDMA_TX_TEMP_COMP_SIZ 8
//#define RF_TDSCDMA_MPR_SIZ 7
//#define RF_TDSCDMA_HDET_VS_AGC_SIZ 16
//#define RF_TDSCDMA_LNA_GAIN_STATES_PAIRWISE_SIZE 12
                                                               

/*----------------------------------------------------------------------------*/
/*!
  @brief
    RF Chain Enum with upper RF driver layer to program correct chain buffers.
    Could expand the enum to include multiple Rx/Tx chains.
*/
typedef enum
{
  RF_TDSCDMA_MDSP_CHAIN_0 = RF_PATH_0, /*!< MDSP Rx Chain 0 */
  RF_TDSCDMA_MDSP_CHAIN_1 = RF_PATH_1, /*!< MDSP Tx Chain 1, Not supported now */
  RF_TDSCDMA_MDSP_CHAIN_NUM /*!< Maximum number of MDSP Chain */
} rf_tdscdma_mdsp_chain_enum_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
    RF/MDSP Enum to designate the correct memory section to program.
*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
    RF/MDSP Enum to designate the correct memory section to program.
*/

typedef enum
{
  RF_TDSCDMA_MDSP_RF_GAIN_BUF_TYPE,       /* RFA Gain Buffer Type */
  RF_TDSCDMA_MDSP_RX_BUF_TYPE,            /* Rx Buffer Type */
  RF_TDSCDMA_MDSP_IFREQ_BUF_TYPE,         /* Infreq Buffer Type */
  RF_TDSCDMA_MDSP_IRAT_BUF_TYPE,          /* InRAT Buffer Type */
  RF_TDSCDMA_MDSP_TX_BUF_TYPE,            /* Tx Buffer Type */
  RF_TDSCDMA_MDSP_TXAGC_LUT_BUF_TYPE,     /* TxAGC LUT Buffer Type */
  RF_TDSCDMA_MDSP_DPD_LUT_BUF_TYPE,       /* DPD LUT Buffer Type */
  RF_TDSCDMA_MDSP_FIRST_SWP_LUT_BUF_TYPE, /* First sweep LUT Buffer Type */
  RF_TDSCDMA_MDSP_ONETIME_BHO_BUF_TYPE,   /* Onetime BHO Buffer Type */
  RF_TDSCDMA_MDSP_HDET_WRITE_BUF_TYPE,    /* HDET WRITE Buffer Type */
  RF_TDSCDMA_MDSP_HDET_READ_BUF_TYPE,     /* HDET READ Buffer Type */
  RF_TDSCDMA_MDSP_THERM_WRITE_BUF_TYPE,   /* THERM WRITE Buffer Type */
  RF_TDSCDMA_MDSP_THERM_READ_BUF_TYPE,    /* THERM READ Buffer Type */
  RF_TDSCDMA_MDSP_MAX_RXTX_BUF_NUM        /* Number of Buffer Type */
} rf_tdscdma_mdsp_rxtx_buf_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum for Override Interface types to communicate with upper RF Driver layer
*/

typedef enum
{
  RF_TDSCDMA_MDSP_RX_GAIN_STATE_OVERRIDE,       /*!< Rx Gain state override */
  RF_TDSCDMA_MDSP_TXAGC_OVERRIDE,               /*!< Tx AGC override */
  RF_TDSCDMA_MDSP_TXAGC_LUT_INDEX_OVERRIDE,     /*!< TxAGC LUT index override */
  RF_TDSCDMA_MDSP_PA_ON_OVERRIDE,               /*!< PA On/Off override */
  RF_TDSCDMA_MDSP_PA_RANGE_OVERRIDE,            /*!< PA Range override */
  RF_TDSCDMA_MDSP_TXAGC_G0_OVERRIDE,            /*!< Tx G0/G1 override */
  RF_TDSCDMA_MDSP_TXAGC_G1_OVERRIDE,            /*!< Tx G0/G1 override */
  RF_TDSCDMA_MDSP_HDET_OVERRIDE,                /*!< HDET override */
  RF_TDSCDMA_MDSP_SLOT_CONFIG_OVERRIDE,        /*!< Rx Burst Config override */
  RF_TDSCDMA_MDSP_TPC_OVERRIDE,                 /*!< Tx TPC pattern override */
  RF_TDSCDMA_MDSP_OVERRIDE_NUM                  /*!< Number of override defined */
} rf_tdscdma_mdsp_override_type;

typedef struct
{
  uint8* nb_ptr;
  uint8* wb_ptr;
}rf_tdscdma_mdsp_rx_swpt_pair_type;

typedef struct
{
  uint8 pa_index;
  uint8 xpt_mode;
}rf_tdscdma_mdsp_xpt_mode_pair_type;

typedef struct
{
  union 
  {
    int16 tx_freqtemp_adj[RFCOM_TXLIN_NUM_LINEARIZERS];     /* Temp comp/Freq comp adjustment, updated when freq/temp changes, S16Q9 dBm format */
    int16 max_power_limit;        /* MTPL according to Temp/HDET values, S16Q9 dBm */
    int16 hdet_interval;          /* HDET measurement interval (in sub-frame uint) */
    int16 mpr[RF_TDSCDMA_MPR_SIZ];
    int16 rx_gain_freqcomp[RF_TDSCDMA_MAX_LNA_GAIN_STATES];
    rf_tdscdma_mdsp_rx_swpt_pair_type rx_swpt[2];
    uint32 chan_freq;
    rf_tdscdma_mdsp_xpt_mode_pair_type xpt_mode;
    int16 pin_comp;
  };
} rf_tdscdma_mdsp_dyn_param_type;

#ifdef FEATURE_RFA_D3925
typedef struct
{
  union
  {
	tfw_rf_sm_rx_freq_cfg_t rx_buf;
    tfw_rf_sm_tx_freq_cfg_t tx_buf;
    tfw_rf_sm_inter_freq_cfg_t ifreq_buf;
  };
} rf_tdscdma_mdsp_dyn_buf_type;

typedef struct
{
  union
  {
	tfw_rf_sm_inter_freq_cfg_t ifreq_buf;
	tfw_rf_sm_rx_freq_cfg_t    rx_buf;
	tfw_rf_sm_tx_freq_cfg_t    tx_buf;
  };
} rf_tdscdma_mdsp_smem_ssbi_buf_prt_type;
#endif /* #ifdef FEATURE_RFA_D3925 */

typedef enum
{
  RF_TDSCDMA_MDSP_DYN_TXAGC_ADJ_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_MAX_PWR_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_HDET_INTVL_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_MPR_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_RX_GAIN_FREQCOMP_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_RX_LNA_SWPT_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_RX_CHAN_FREQ_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_TX_CHAN_FREQ_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_XPT_MODE_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_XPT_PIN_COMP_UPDATE_ID,
  RF_TDSCDMA_MDSP_DYN_MAX_UPDATE_ID
} rf_tdscdma_mdsp_dyn_update_id_type;

typedef enum
{
  RF_TDSCDMA_MDSP_HOME_BUF,
  RF_TDSCDMA_MDSP_TGT_BUF,
  RF_TDSCDMA_MDSP_MAX_BUF
} rf_tdscdma_mdsp_home_target_buf_type;

//Temporary definition for this type
typedef struct {
  /* Index number for the next index to the results[] */
  uint8 index;
  /* Measurement result in dB10 */
  int16  dlPowerLevel;
  /* Measurement result in dB10 for RxDiversity*/
  int16  dlPowerLevelDiv;
} rf_tdscdma_mdsp_slow_cali_results_type;

/* TFW State Defintion */
typedef enum {
  RF_TDSCDMA_MDSP_TFW_STATE_INACTIVE 	= TFW_STATE_INACTIVE,
  RF_TDSCDMA_MDSP_TFW_STATE_STANDBY 	= TFW_STATE_STANDBY,
  RF_TDSCDMA_MDSP_TFW_STATE_ACQUISITION = TFW_STATE_ACQUISITION,
  RF_TDSCDMA_MDSP_TFW_STATE_TRACKING 	= TFW_STATE_TRACKING,
  RF_TDSCDMA_MDSP_TFW_STATE_FTM_CAL     = TFW_STATE_FTM_CAL,
  RF_TDSCDMA_MDSP_TFW_STATE_FTM         = TFW_STATE_FTM,
  RF_TDSCDMA_MDSP_TFW_STATE_FTM_ACQ     = TFW_STATE_FTM_ACQ,
  RF_TDSCDMA_MDSP_TFW_STATE_FTM_TRK     = TFW_STATE_FTM_TRK,
  RF_TDSCDMA_MDSP_TFW_STATE_FTM_CX8     = TFW_STATE_FTM_CX8,
  RF_TDSCDMA_MDSP_TFW_STATE_INVALID 	= TFW_STATE_INVALID,
} rf_tdscdma_mdsp_tfw_state_t;

#ifndef FEATURE_TRITON_MODEM
typedef enum {
  RF_TDSCDMA_MDSP_TFW_XPT_MODE_OFF = TFW_XPT_TX_BURST_MODE_OFF,
  RF_TDSCDMA_MDSP_TFW_XPT_MODE_SPL_BURST = TFW_XPT_TX_SPL_BURST_MODE,
  RF_TDSCDMA_MDSP_TFW_XPT_MODE_DPD_BURST = TFW_XPT_TX_DPD_BURST_MODE,
  RF_TDSCDMA_MDSP_TFW_XPT_NUM_MODE = TFW_XPT_TX_NUM_MODE,
}rf_tdscdma_mdsp_tfw_xpt_mode_t;
#else
/* Triton doesn't support xPT currently */
typedef enum {
  RF_TDSCDMA_MDSP_TFW_XPT_MODE_OFF = 0,
  RF_TDSCDMA_MDSP_TFW_XPT_MODE_SPL_BURST = 1,
  RF_TDSCDMA_MDSP_TFW_XPT_MODE_DPD_BURST = 2,
  RF_TDSCDMA_MDSP_TFW_XPT_NUM_MODE = 3,
}rf_tdscdma_mdsp_tfw_xpt_mode_t;
#endif

/* MSG Enum type */
typedef enum {
  RF_TDSCDMA_MDSP_STATE_CFG_MSG = 0,
  RF_TDSCDMA_MDSP_RXLM_CFG_MSG,
  RF_TDSCDMA_MDSP_RXDIVLM_CFG_MSG,
  RF_TDSCDMA_MDSP_TXLM_CFG_MSG,
  RF_TDSCDMA_MDSP_CONT_RX_MSG,
  RF_TDSCDMA_MDSP_CONT_TX_MSG,
  RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG,
  RF_TDSCDMA_MDSP_FTM_RF_RX_BURST_MSG,
  RF_TDSCDMA_MDSP_FTM_RF_TX_BURST_MSG,
  RF_TDSCDMA_MDSP_GET_MULTI_SYNTH_MSG,
  RF_TDSCDMA_MDSP_XPT_TX_MSG,
} rf_tdscdma_mdsp_msg_t;

typedef struct {
  uint8 bus;
  uint8 reg_addr;
  uint8 reg_val;
} rf_tdscdma_mdsp_ssbi_format_type;

typedef struct {
  uint8 rx_gain_state_num;
  rf_tdscdma_mdsp_ssbi_format_type rxgain_ssbi[4];
} rf_tdscdma_mdsp_rx_agc_gain_param_type;

typedef struct {
  int16 min_tx_pwr;
  rf_tdscdma_mdsp_ssbi_format_type tx_agc_starting_ssbi;
  rf_tdscdma_mdsp_ssbi_format_type tx_slot_ctl_ssbi;
  //tx_slot_mode_lut
  //tfw_rf_sm_intf_smps_mode_e tx_smps_mode;
  //Idle rx/tx ssbi
  //Hdet ssbi
  //Therm ADC ssbi
//  tfw_rf_sm_intf_smps_mode_e txPaSmpsMode;
//  int16  smpsTimeAdj;
//pdet_power_threshold;
} rf_tdscdma_mdsp_init_param_type;

#define RF_TDSCDMA_MDSP_SSBI_TRASNATION_TIMEX10  25  //leave some margin between Rx SSBI and Rx_on

#ifdef __cplusplus
}
#endif

#endif /* RF_TDSCDMA_MDSP_TYPES_H */
