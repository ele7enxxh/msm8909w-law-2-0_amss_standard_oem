#ifndef RF_CDMA_EPT_H
#define RF_CDMA_EPT_H

/*! 
  @file
  rf_cdma_ept.h
 
  @brief
  This file contains prototypes and definitions to be used by Centralized 
  1x EPT.
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_ept.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/08/14   pk      Fix for RevB ACPR Performance issue (avoid DPD WTR setting)
08/26/13   shb     Updates for new C++ transceiver device interface
08/01/13   Saul    XPT. ET delay vs temp FTM char support
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
06/19/13   APU     Added new NV for ET delay. 
06/21/13   jmf     [xPT] ET Delay implementation for Calv4
03/26/13   aro     Renamed interface to indicate xpt_resource
03/26/13   aro     MC Callflow illustration
03/19/13   aro     Added XPT CDMA cal deconfig
03/19/13   aro     Added call to XPT cal config from protocal IQ capture
02/28/13   hdz     Added last_trig to ept data structure
02/27/13   aro     Mainlined debug flag for Tx resource manager
02/27/13   aro     Interface to determine XPT mode of operation based on MSM
                   and NV
02/22/13   aro     Migrate XPT functions to common EPT module
02/22/13   aro     Migrated the tx resource manager callback function to
                   common CDMA EPT module
02/22/13   aro     Header file inclusion
02/21/13   aro     Updated configure XPT interface to push band and NV chain
                   parameter for calling function
02/21/13   aro     CDMA Tx Resource manager implementation
01/22/13   aro     Removed unused variables
01/21/13   aro     Cleaned up IQ Capture and XPT config interfaces
01/18/13   aro     Removed memory allocation for IQ capture
11/16/12   Saul    Sample Capture. Use common response.
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
07/17/12   aro     Compile Error Fix
07/16/12   aro     Added support for dynamic memory allocation for IQ
                   capture buffers
06/25/12   aro     Added EPT override count
06/25/12   aro     Created EPT calibration module in Data structure
06/14/12   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_time_profile.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_nv.h"
#include "lm_types.h"
#include "rfm_cdma_band_types.h"
#include "rfm_device_types.h"
#include "rfcommon_tx_manager.h"
#include "rfcommon_core_xpt.h"
#include "rf_cdma_temp_comp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by/during RF CDMA EPT
  calibration. There should not be stray global variables. If global variable is 
  required, it must be contained within this structure. 
*/
typedef struct
{
  uint8 count_iq_capture;
  /*!< Variable to store the count of IQ Capture command sent */

} rf_cdma_ept_calibration_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by/during RF CDMA EPT.
  There should not be stray global variables. If global variable is required, 
  it must be contained within this structure. 
*/
typedef struct
{
  rf_cdma_ept_calibration_data_type cal_data;
  /*!< Structure containing EPT Calibration data */

} rf_cdma_ept_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing data to be passed to Tx resource manager to be used
  by CDMA callback functions
*/
typedef struct
{
  rfm_mode_enum_type cdma_mode;
  /*!< CDMA mode indicating if it is 1x or HDR mode */

  rfm_cdma_band_class_type band;
  /*!< CDMA band for which tx resource manager is to be configured */

  rfm_cdma_chan_type chan;
  /*!< CDMA channel for which tx resource manager is to be configured */

  xpt_cfg_type xpt_mode;
  /*!< XPT mode of operation for CDMA */

  rf_cdma_nv_path_type tx_nv_path;
  /*!< Tx NV Path for the given device */

  rf_cdma_msm_static_txlm_cfg_type static_cfg;
  /*!< Structure containing static TxLM configuration */

  rf_cdma_msm_dynamic_txlm_cfg_type dynamic_cfg;
  /*!< Structure containing dynamic TxLM configuration */

  lm_handle_type master_txlm_handle;
  /*!< TxLM handle pertaining to the master mnodulation Tx path */

} rf_cdma_resource_mgr_cb_data_type;

/*----------------------------------------------------------------------------*/
rf_tx_resource_mgr_init_ret_type 
rf_cdma_resource_mgr_init_xpt
(
  rfm_device_enum_type device,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_resource_mgr_deinit_xpt
(
  rfm_device_enum_type device,
  lm_handle_type slave_txlm_handle,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_resource_mgr_prep_xpt
(
  rfm_device_enum_type device,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_resource_mgr_setup_xpt
(
  rfm_device_enum_type device,
  lm_handle_type slave_txlm_handle,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_resource_mgr_cleanup_xpt
(
  rfm_device_enum_type device,
  lm_handle_type slave_txlm_handle,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
uint32 
rf_cdma_xpt_get_cfg_param
(
  rfm_device_enum_type device, 
  uint8 xpt_cfg_index 
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_xpt_is_enabled
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
xpt_cfg_type
rf_cdma_xpt_get_operation_mode
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_xpt_override_et_delay_value
(
  rfm_device_enum_type device,
  int32 delay_value,
  uint32 channel
);

/*----------------------------------------------------------------------------*/
int32
rf_cdma_xpt_get_path_delay 
(
   rfdevice_trx_cdma_tx* tx_device,
   rf_cdma_tx_cal_type*     tx_cal    ,
   rfm_cdma_band_class_type band ,
   uint32                   channel,
   boolean enable_temp_update,
   boolean temp_read_valid,
   uint16 temp_sensor_read
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_xpt_is_enabled_for_hdr
(
  uint8 num_carriers
);

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_EPT_H */


