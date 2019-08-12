/*! 
 @file rflte_core_xpt.h 
 
 @brief 
  This is the header file file for ET related implementation for the LTE technology. 
 
 @details 
 
*/ 
  
/*=========================================================================== 
  
Copyright (c) 2009 - 2012 by QUALCOMM Technologies, Inc.  All Rights Reserved. 
  
                           EDIT HISTORY FOR FILE 
  
This section contains comments describing changes made to this file. 
Notice that changes are listed in reverse chronological order. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rflte_core_xpt.h#1 $
  
When       who     what, where, why 
------------------------------------------------------------------------------- 
05/07/13   jf      SV hook up for XPT
10/12/12   jf      initial version
  
============================================================================*/ 
#ifndef RFLTE_CORE_XPT_H
#define RFLTE_CORE_XPT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "rflte_mc.h"
#include "rflte_mdsp.h"
#include "rfcommon_tx_manager.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing data to be passed to Tx resource manager to be used
  by LTE callback functions
*/
typedef struct
{
   xpt_cfg_type xpt_mode;
   rfcom_lte_band_type band;
   rfm_device_enum_type device;
   rfcom_lte_bw_type bw;
   txlm_chain_type et_chain;
   lm_handle_type et_txlm_buf_idx;
}rflte_core_xpt_resource_mgr_cb_data_type;

boolean rflte_core_xpt_request_master_tx(rflte_mc_carrier_info_type carrier_info, 
                                         rfcom_lte_band_type band,
                                         rfcom_lte_bw_type bw);

boolean rflte_core_xpt_release_master_tx(rflte_mc_carrier_info_type carrier_info, 
                                         rfcom_lte_band_type band);

void rflte_core_xpt_request_xpt_resource(rflte_mc_carrier_info_type carrier_info, 
                                        rfcom_lte_band_type band,
                                        rfcom_lte_bw_type bw);

void rflte_core_xpt_release_xpt_resource(rflte_mc_carrier_info_type carrier_info, 
                                         rfcom_lte_band_type band);

rf_tx_resource_mgr_init_ret_type 
rflte_core_xpt_resource_mgr_init
(
  rfm_device_enum_type device,
  void *cb_data
);

boolean rflte_core_xpt_resource_mgr_deinit
(
  rfm_device_enum_type device,
  lm_handle_type slave_txlm_handle,
  void *cb_data
);

boolean rflte_core_xpt_resource_mgr_prep_slave
(
  rfm_device_enum_type device,
  void *cb_data
);

boolean rflte_core_xpt_resource_mgr_setup_slave
(
  rfm_device_enum_type device,
  lm_handle_type slave_txlm_handle,
  void *cb_data
);

boolean rflte_core_xpt_resource_mgr_cleanup_slave
(
  rfm_device_enum_type device,
  lm_handle_type slave_txlm_handle,
  void *cb_data
);

#ifdef __cplusplus
}
#endif

#endif

