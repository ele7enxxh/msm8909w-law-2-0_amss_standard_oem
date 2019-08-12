#ifndef RF_TDSCDMA_MDSP_SMEM_INTF_H
#define RF_TDSCDMA_MDSP_SMEM_INTF_H
/*! 
  @file
  rf_tdscdma_mdsp_smem_intf.h
 
  @brief
  This file defines the Shared Memory Interface between Q6SW and Q6FW.

  @addtogroup RF_MDSP_TDSCDMA_SMEM_INTF
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/qdsp6_tdscdma/inc/rf_tdscdma_mdsp_smem_intf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/12/14   jyu     MDSP/SMEM cleanup
11/07/13   nsh     FED RxAGC Feature
10/10/13   jyu     Integrated with DM interface
09/06/13   jyu     MDSP cleanup
06/25/13   ms      Added rx_ifreq_div_script_ptr
02/26/13   ms      Pass rf_chain to update diversity chain
02/08/13   jyu     Removed hard-coded device info
08/01/12   jyu     Added event script support for DIME  
07/16/12   hy      Add TD-SCDMA FTM Rx AGC and Tx AGC logs 
07/09/12   ycl     Fast Tx off and DAC update checked in 
03/19/12   ycl     Update Tx On/Off SMEM pointer. 
02/14/12   ycl     Update interface to support LNA swpt updates. 
01/19/12   jyu     Added support to populate HDET/therm write/read scripts 
12/20/11   ycl     Update SharedMemory portion.
09/06/11   ycl     Change to match TFW interface.
08/15/11   ycl     Change to match MDSP Shared Memory changes.  
06/23/11   ycl     Initial version. 
 
==============================================================================*/

#include "comdef.h"
#include "rfnv_tdscdma.h"
#include "tfw_const.h"
#include "rf_tdscdma_mdsp_types.h"
#include "tfw_sw_intf_msg.h"
#include "rflm_tds_dm_intf.h"

#ifdef __cplusplus
extern "C" {
#endif
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold all SMEM Interface to be sent to TDSCDMA MDSP from RF.

  @details
  This structure shall be used to hold all the info to be sent to TDSCDMA MDSP
  from RF using Shared Memory. Once RF Upper Layer calls any RF-MDSP function, 
  then the parameters will be populated on one (or some) of the fields in SMEM; 

  @todo
*/

typedef enum
{
  RF_TDSCDMA_MDSP_RF_BASE_SMEM_TYPE,        /* RF BASE SMEM Type*/
  RF_TDSCDMA_MDSP_RF_GAIN_SMEM_TYPE,      	/* RF Gain Buffer SMEM Type*/
  RF_TDSCDMA_MDSP_RF_IDLE_SSBI_SMEM_TYPE, 	/* Rx AGC Buffer SMEM Type*/
  RF_TDSCDMA_MDSP_RX_SMEM_TYPE,           	/* Rx Buffer SMEM Type*/
  RF_TDSCDMA_MDSP_RX_IFREQ_SMEM_TYPE,      	/* IFREQ Buffer SMEM Type */
  RF_TDSCDMA_MDSP_RX_IRAT_SMEM_TYPE,         /* IRAT Buffer SMEM Type */
  RF_TDSCDMA_MDSP_TX_SMEM_TYPE,				/* Tx Buffer SMEM Type */     
  RF_TDSCDMA_MDSP_FTM_SLOWCAL_SMEM_TYPE,    /* Slow Cal Results SMEM Type */
  RF_TDSCDMA_MDSP_ONETIME_BANDCHG_SMEM_TYPE,/* Tx Buffer SMEM Type */  
  RF_TDSCDMA_MDSP_TXAGC_LUT_SMEM_TYPE,      /* Tx LUT Buffer SMEM Type*/
  RF_TDSCDMA_MDSP_DPD_LUT_SMEM_TYPE,        /* DPD LUT Buffer SMEM Type*/
  RF_TDSCDMA_MDSP_FIRST_SWEEP_LUT_SMEM_TYPE,   /* First sweep LUT Buffer SMEM Type*/
  RF_TDSCDMA_MDSP_HDET_WRITE_SSBI_SMEM_TYPE,   /* HDET WRITE Buffer SMEM Type*/     
  RF_TDSCDMA_MDSP_HDET_READ_SSBI_SMEM_TYPE,    /* HDET READ Buffer SMEM Type*/     
  RF_TDSCDMA_MDSP_THERM_WRITE_SSBI_SMEM_TYPE,  /* THERM WRITE Buffer SMEM Type*/     
  RF_TDSCDMA_MDSP_THERM_READ_SSBI_SMEM_TYPE,   /* THERM READ Buffer SMEM Type*/       
  RF_TDSCDMA_MDSP_RF_AGC_LOG_SMEM_TYPE,   /* RF AGC LOG READ Buffer SMEM Type*/     
  RF_TDSCDMA_MDSP_SMEM_TYPE_NUM             /* Number of Buffer SMEM Types */
} rf_tdscdma_mdsp_smem_type;

typedef struct {
  /*! @brief  Flag to indicate whether RF/MDSP SMEM is initialized */
  boolean smem_initialized;
  /*! @brief  Ptr to SMEM base Addr */
  tfw_sw_intf_t* base_addr;
  /*! @brief Ptr to SMEM Common Struct */
  rflm_tds_rf_sm_intf_t* rf_sm_base_addr;
  /*! @brief TX RF Analog Gain parameters */
  void* rf_gainparams;
  /*! @brief Home(F1) OFF-to-Tx and Tx-to-OFF script */
  void* txOnOffScripts;
  /*! @brief OFF(Idle)-to-Home(F1)|Target(F3) frequency scripts */
  void* rx_script_ptr[TFW_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  /*! @brief OFF(Idle)-to-Measurement(F2) frequency scripts */
  void* rx_irat_script_ptr[TFW_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  /*! @brief OFF(Idle)-to-Measurement(F2) frequency scripts */
  void* rx_ifreq_script_ptr[TFW_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  /*! @brief TX Home(F1)-to-Target(F3)frequency script */
  void* tx_script_ptr[TFW_RX_TX_MAX_NUM_TX_BUFFERS];
  void* txagc_lut_ptr[TFW_RX_TX_MAX_NUM_TX_BUFFERS];
  void* dpd_lut_ptr[TFW_RX_TX_MAX_NUM_TX_BUFFERS];
  void* first_sweep_lut_ptr[TFW_RX_TX_MAX_NUM_TX_BUFFERS];
  /*! \brief PDET measurement configuration & results */
  void* tx_pdet_params_ptr;
  /*! \brief TEMP measuerment configuration & results */
  void* tx_temp_params_ptr;
  /*! \brief RF calibration results */
  void* ftm_slowcalresults_ptr;
  /*! \brief HDET write Script */
  void* hdet_write_script_ptr;
  /*! \brief HDET read Script */
  void* hdet_read_script_ptr;
  /*! \brief Therm write Script */
  void* therm_write_script_ptr;
  /*! \brief Therm read Script */
  void* therm_read_script_ptr;
  /*! \brief Onetime BHO Script */
  void* bho_onetime_script_ptr;
  /*! \brief CCS Event Table */
  rflm_tds_ccs_event_t* ccs_event_table_ptr;
  

  /*! \brief RF AGC log*/
  tfw_agc_result_buf_t* tfw_agc_result;
  /*! @brief InterFreq Diversity scripts */
  void* rx_ifreq_div_script_ptr;

} rf_tdscdma_mdsp_smem_ptr_type;

  rf_tdscdma_mdsp_smem_ptr_type rf_tdscdma_mdsp_smem_ptr;
  /*!< RF Variable to store all parameters related to TDSCDMA MDSP Interface */

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------*/
/*! Pointer to Shared Memory */
//extern rf_tdscdma_mdsp_smem_ptr_type rf_tdscdma_mdsp_smem_ptr;

/*============================================================================*/
/*!
  @name SMEM Interface - LUT Update

  @brief
  This section contains Macros related to LUT Table Update. This will be used 
  internally by RF-MDSP Module to write MDSP Data to Shares Memory Interface.
*/

void rf_tdscdma_mdsp_smem_init
( 
  void* tech_buff_ptr
);

//Publish one time init RFA Shared Memory
void  rf_tdscdma_mdsp_smem_build_onetime_script_buf
( 
  rfm_device_enum_type device,  
  void* init_vars
);

void* rf_tdscdma_mdsp_smem_get_smem_ptr
(
  rf_tdscdma_mdsp_chain_enum_type rf_chain,
  rf_tdscdma_mdsp_smem_type rf_mdsp_smem_type,
  uint8 index
);

void * rf_tdscdma_mdsp_smem_get_buf_ptr
(
  rf_tdscdma_mdsp_chain_enum_type rf_chain,
  rf_tdscdma_mdsp_rxtx_buf_type rxtx_buf,
  uint8 rf_buf_index
);


int16 rf_tdscdma_mdsp_smem_rxagc_convert( uint8 rfnv_rxagc );




/*! \{ */


/*! @} */

#endif /* RF1X_MDSP_SMEM_INTF_H */
