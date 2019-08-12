#ifndef RFCOMMON_MDSP_TYPES_H
#define RFCOMMON_MDSP_TYPES_H

/*! 
  @file
  rfcommon_mdsp_types.h
 
  @brief
  This module defines the common defintions used by RF COMMON RF-MDSP
  Internal(RF) interface.
*/

/*==============================================================================

  Copyright (c) 2012 -  Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_mdsp_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/27/14   dr      Adding ANT_TUNER support for Tabasco 
03/26/14   ra      Fix DimePM compile error
03/21/14   ra      Support FW cleanup of deprecated files
11/06/13   cd      Add support to provide RFLM handle for RFLM overrides
10/21/13   cd      Add IREF override functionality
07/15/13   cri     Immediate MSM register programming for tx override
06/17/13   ndb     Remove the featurization for the tuner enum's which needs to be used in 
                   common framework
06/14/13   ndb     Added GSM Buf ID and enums for CL event status
06/12/13   ndb     Added tuner buffer for TDSCDMA
05/01/13   rmb     Added Tuner Buffers for Sinle Band and Dual Band Support in UMTS.
03/27/13   zhw     Added featurization for tuner related code
03/21/13   zhw     Added support for Antenna Tuner Open Loop common module 
02/08/13   cri     Get Tx chain info for Tx override from RFC
12/21/12   cd      IQ gain and envelope scale override values are 32 bit
12/18/12   cd      Add support for delay value programming to common FW
11/08/12   cd      Changes to support common FW Tx AGC override
10/19/12   Saul    COMMON XPT. Removed maintenance of mask from common data.
08/28/12   rsr     Added support for TXAGC override.
08/22/12   Saul    CDMA. Changed load dpd num words val.
08/02/12   Saul    COMMON. Added support to allocate/write/free DPD tables.
08/02/12   Saul    Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcmd_msg.h"
#include "fw_rf_common_intf.h"


#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
#if defined(FEATURE_BOLT_MODEM) || defined(FEATURE_TABASCO_MODEM) 
typedef enum
{
  FW_RF_QTF_ANT_TUNER_BUF_ID_GSM,
  FW_RF_QTF_ANT_TUNER_BUF_ID_GSM_G0 =
    FW_RF_QTF_ANT_TUNER_BUF_ID_GSM,
  FW_RF_QTF_ANT_TUNER_BUF_ID_GSM_G1,
  FW_RF_QTF_ANT_TUNER_BUF_ID_1X,
  FW_RF_QTF_ANT_TUNER_BUF_ID_DO,
  FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS,
  FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS_C0 =
    FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS,
  FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS_C1,
  FW_RF_QTF_ANT_TUNER_BUF_ID_LTE,
  FW_RF_QTF_ANT_TUNER_BUF_ID_LTE_C0 = 
    FW_RF_QTF_ANT_TUNER_BUF_ID_LTE,
  FW_RF_QTF_ANT_TUNER_BUF_ID_LTE_C1,
  FW_RF_QTF_ANT_TUNER_BUF_ID_TDSCDMA,
  FW_RF_QTF_ANT_TUNER_NUM_TECHS
} fw_rf_qtf_ant_tuner_tech_type;

/*! @brief QTF tuner ID enum. */
typedef enum
{
  FW_RF_QTF_ANT_TUNER_PRIMARY,
  FW_RF_QTF_ANT_TUNER_SECONDARY,
  FW_RF_QTF_ANT_TUNER_NUM_PER_TECH
} fw_rf_qtf_ant_tuner_buf_type; 


#endif 

#define RF_TUNER_BUF_ID_1X        FW_RF_QTF_ANT_TUNER_BUF_ID_1X
#define RF_TUNER_BUF_ID_DO        FW_RF_QTF_ANT_TUNER_BUF_ID_DO
#define RF_TUNER_BUF_ID_UMTS      FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS
#define RF_TUNER_BUF_ID_UMTS_SB   FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS_C0 /* FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS_C0 is same as FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS */
#define RF_TUNER_BUF_ID_UMTS_DB   FW_RF_QTF_ANT_TUNER_BUF_ID_UMTS_C1
#define RF_TUNER_BUF_ID_LTE       FW_RF_QTF_ANT_TUNER_BUF_ID_LTE
#define RF_TUNER_BUF_ID_LTE_C0    FW_RF_QTF_ANT_TUNER_BUF_ID_LTE_C0
#define RF_TUNER_BUF_ID_LTE_C1    FW_RF_QTF_ANT_TUNER_BUF_ID_LTE_C1
#define RF_TUNER_BUF_ID_TDSCDMA   FW_RF_QTF_ANT_TUNER_BUF_ID_TDSCDMA
#define RF_TUNER_BUF_ID_GSM       FW_RF_QTF_ANT_TUNER_BUF_ID_GSM

#define RF_TUNER_CL_OCCURRED      FW_RF_QTF_ANT_TUNER_UPDATE_CL_OCCURRED
#define RF_TUNER_CL_TIMEOUT       FW_RF_QTF_ANT_TUNER_UPDATE_CL_TIMEOUT


#define RF_TUNER_BUF_ID_NUM_TECHS FW_RF_QTF_ANT_TUNER_NUM_TECHS

#define RF_TUNER_CHAIN_0          FW_RF_QTF_ANT_TUNER_PRIMARY
#define RF_TUNER_CHAIN_1          FW_RF_QTF_ANT_TUNER_SECONDARY
#define RF_TUNER_CHAIN_NUM        FW_RF_QTF_ANT_TUNER_NUM_PER_TECH

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Maximum number of continuous DPD tables writes
*/
#define RFCOMMON_MDSP_MAX_NUM_CONT_DPD_TABLE_WRITES  12

/*----------------------------------------------------------------------------*/
/*! 
  @brief Information regarding one continuous block of data to be 
         written to the DPD tables */
typedef struct 
{
  /*! @brief TXC bank ID to write data */
  uint8 bank_id;

  /*! @brief Start addr within bank (0-255) */
  uint8 start_addr;

  /*! @brief Number of words to write in a bank (if the table is 16 bit value, 
    RF should still send the writes as 32 bit writes) */
  uint16 num_words;

  /*! @brief Physical address from where to copy table to TXC memory */
  uint32 *data_phys_addr;

} rfcommon_dpd_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure DPD Info.
*/
typedef struct
{
  /*! DPD information per block */
  rfcommon_dpd_info_type tables[ RFCOMMON_MDSP_MAX_NUM_CONT_DPD_TABLE_WRITES ];
  
} rfcommon_mdsp_dpd_info_type;

/*! @brief Enum for XPT mode types. */
typedef enum
{
  RFCOMMON_MDSP_XPT_MODE_NOPT = -1,
  RFCOMMON_MDSP_XPT_MODE_APT,
  RFCOMMON_MDSP_XPT_MODE_EPT,
  RFCOMMON_MDSP_XPT_MODE_ET
} rfcommon_mdsp_xpt_mode_enum_type; 

typedef enum
{
  RFCOMMON_MDSP_DPD_SIZE_32  = 32,
  RFCOMMON_MDSP_DPD_SIZE_64  = 64,
  RFCOMMON_MDSP_DPD_SIZE_128 = 128,
  RFCOMMON_MDSP_DPD_SIZE_256 = 256,
  RFCOMMON_MDSP_DPD_SIZE_512 = 512,
}rfcommon_mdsp_dpd_tbl_size_enum_type;

/*! @brief Struct for DPD table id. */
typedef PACK(struct) 
{
  /*! @brief Bank ID */
  uint8 mem_bank_id;
  /*! @brief offset within bank */
  uint16 start_offset;
  /*! @brief Number of 16bit writes in table */
  rfcommon_mdsp_dpd_tbl_size_enum_type tbl_size;
}rfcommon_mdsp_dpd_table_info_t;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM common dispatch request packet to be used to override TXAGC parameters.
*/
typedef PACK(struct)
{
  /*!< xPT mode to set. One of RFCOM_xPT_MODE_NOPT, RFCOM_xPT_MODE_APT,RFCOM_xPT_MODE_EPT,RFCOM_xPT_MODE_ET */
  rfcommon_mdsp_xpt_mode_enum_type xpt_mode;
  /*!< Address of AMAM table to load. THis address is returned by Load_DPD FTM command */
  rfcommon_mdsp_dpd_table_info_t amam_addr;
  /*!< Address of AMPM table to load. THis address is returned by Load_DPD FTM command */
  rfcommon_mdsp_dpd_table_info_t ampm_addr;
  /*!< Address of detrough table to load. THis address is returned by Load_DPD FTM command. Should be 0xFFFF in EPT and APT mode. */
  rfcommon_mdsp_dpd_table_info_t detrough_addr;
  /*!< Digital gain (gain post calibration) to override */  
  uint32 iq_gain;
  /*!< Env Scale value to override */
  uint32 env_scale; 
  /*! total amount of delay from all DM blocks, unit in ns, Q17 format, precision to 0.1ns */
  int32 delay;
  /*! TxLM handle information for the Tx path on which delay value must be adjusted */
  uint32 delay_txlm_handle;
  /*! Modem chain used for Tx */
  uint8 modem_chain;
  /*! Flag for latching registers immediately */
  boolean latch_immediate;
  /*! Max TXC sample count. Used for immediate programming within SF/BPG/CPG limits */
  uint32 max_sample_count;
  /*! IREF baseband override */
  uint16 iref_uamp_x10;
  /*! RFLM handle - Will replace delay_txlm_handle */
  uint32 rflm_handle;

} rfcommon_mdsp_txagc_oride_params_type;

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_MDSP_TYPES_H */
