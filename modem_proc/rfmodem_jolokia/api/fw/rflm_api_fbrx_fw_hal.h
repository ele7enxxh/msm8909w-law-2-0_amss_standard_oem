/*!
  @file
  rflm_api_fbrx_fw_hal.h

  @brief
  RF LM API definitions for FBRx HAL
  
  @detail



*/

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/fw/rflm_api_fbrx_fw_hal.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/24/14    aa     Added support to store lmem sample captures for log packet 0x184A
07/07/14    aa     Added RSB IQMC changes for mulitple gain state support
05/12/14    ka     Add BW to update rsb api 
05/02/14    aa     Added fbrx lm buffer settings 
03/20/14    ka     Added support for storing captures during cal  
03/14/14    aa     Fix Compiler Errors 
03/05/14    aa     Fix Linker Errors
02/26/14   dej     Fix Linker Errors
11/26/13    ka     Added store results api 
11/01/13    ka     Initial version
==============================================================================*/

#ifndef RFLM_API_FBRX_FW_HAL_H
#define RFLM_API_FBRX_FW_HAL_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_fbrx.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_fbrx_msm_configure_chain( rflm_handle_rx_t handle );

/*----------------------------------------------------------------------------*/
extern void rfcommon_fbrx_msm_activate_chain( void );

/*----------------------------------------------------------------------------*/
extern void rfcommon_fbrx_msm_bridge_config( boolean enable );

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_fbrx_msm_store_data( uint16 tx_data_len, 
                                             uint16 rx_data_len );

/*----------------------------------------------------------------------------*/
extern void rfcommon_fbrx_msm_store_data_internal( uint32 *tx_data_ptr,
                                                   uint32 *rx_data_ptr,
                                                   uint16 tx_data_len, 
                                                   uint16 rx_data_len );

/*----------------------------------------------------------------------------*/
extern uint8 rfcommon_fbrx_msm_convert_tx_cfg_to_mode( rfcommon_fbrx_tx_cfg_type cfg);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_fbrx_msm_update_rsb_coeff( rflm_handle_rx_t handle, 
                                                   int32 rsb_coeff_a,
                                                   int32 rsb_coeff_b,
                                                   rflm_tech_id_t tech,
                                                   rfcommon_fbrx_tx_cfg_type bandwidth );

/*----------------------------------------------------------------------------*/

extern boolean rfcommon_fbrx_msm_allocate_rxlm_buffer( rflm_handle_rx_t *handle);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_fbrx_msm_deallocate_rxlm_buffer( rflm_handle_rx_t handle);

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_fbrx_msm_update_static_settings( rflm_handle_rx_t fbrx_lm_handle, 
                                                         rfcommon_fbrx_tx_cfg_type cfg );

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_fbrx_msm_update_dynamic_settings_for_xptcap( rflm_handle_rx_t fbrx_lm_handle );

/*----------------------------------------------------------------------------*/
extern boolean rfcommon_fbrx_msm_write_iqmc_to_hw( rflm_handle_rx_t fbrx_lm_handle, uint32 iqmc_coeff );

/*----------------------------------------------------------------------------*/
extern void rfcommon_fbrx_msm_store_lmem_sample_captures( uint32 *tx_data_ptr,
                                                          uint32 *rx_data_ptr,
                                                          uint16 tx_data_len, 
                                                          uint16 rx_data_len,  
                                                          rflm_fbrx_dm_template_t* fbrx_ptr,
                                                          uint8  run_idx );
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_API_FBRX_FW_HAL_H */
