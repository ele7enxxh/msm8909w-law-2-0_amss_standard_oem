/*!
   @file
   rfc_wcdma_transceiver_hdet_adapter.cpp

   @brief
   Definition of the WCDMA Transceiver device to HDET interface adapter.
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/src/rfc_wcdma_transceiver_hdet_adapter.cpp#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/02/14   dr      Added featurization for Non-W/TDS builds
03/11/13   hm      Updated HDET functions to directly call the C-function ptrs, 
                              instead of calling the functions in rfdevice_wcdma_intf file
                              done so because WTR does not provide HDET functionality and
                              hence these functions do not exist in base TRX class
03/11/13   sar     Updated file for APQ, GNSS only target.
02/15/15   ac      hdet read fix for wtr
02/01/13   ac      based on rfc the common api will call either tdet of trx hdet
12/03/12   bmg     Initial version.

==============================================================================*/
#include "comdef.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfc_wcdma_transceiver_hdet_adapter.h"
#include "rfdevice_wcdma_intf.h"
#include "rfcommon_msg.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  WCDMA Transciever device to HDET interface adapter constructor.
 
  @details
  Must be initialized with the transceiver pointer that this object is
  adapting.
 
  @param cdma_tx_ptr
  Pointer to the WCDMA transceiver device that the adapter will use.
*/
rfc_wcdma_transceiver_hdet_adapter::rfc_wcdma_transceiver_hdet_adapter
(
  rfwcdma_device_tx_type *wcdma_tx_device
)
{
  tx_device = wcdma_tx_device;
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  WCDMA Transciever device to HDET interface adapter destructor.
*/
rfc_wcdma_transceiver_hdet_adapter::~rfc_wcdma_transceiver_hdet_adapter()
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET get_mdsp_config_data() interface to the WCDMA transceiver
  RFDEVICE_GET_HDET_MDSP_CONFIG_DATA command interface.
 
  @param data
  HDET scripts to be filled in W MDSP
 
  @return
  boolean indicating pass or fail
*/
boolean rfc_wcdma_transceiver_hdet_adapter::get_mdsp_config_data(rfdevice_hdet_mdsp_config_data_type* data)
{
  /* All references in WCDMA MC to the RFDEVICE_GET_HDET_MDSP_CONFIG_DATA
     command use hard-coded device and band parameters, so propagating
     that to here. */
#ifdef FEATURE_WCDMA
  rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                 RFCOM_BAND_IMT,
                                 RFDEVICE_GET_HDET_MDSP_CONFIG_DATA,
                                 data);
#endif
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for HDET init in the WCDMA HDET interface which has no
  corresponding transceiver device function.
 
  @details
  Transceiver device HDET initialization occurs automatically as part of
  Tx enable.  The HDET adapter assumes that MC will have already enabled Tx
  in the transceiver before operating HDET.
 
  @param buff_obj_ptr
  Unused.
 
  @param dev_action
  Unused.
 
  @param tx_freq
  Unused.
 
  @return
  Always returns TRUE.
*/
boolean
rfc_wcdma_transceiver_hdet_adapter::hdet_init
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint32 tx_freq
)
{
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET get_mdsp_config_data() interface to the WCDMA transceiver
  RFDEVICE_GET_HDET_MDSP_CONFIG_DATA command interface.
 
  @details
  The WCDMA transceiver read HDET interface does not support scripting,
  so this function will exit with a failure if attempting to call it with
  a script parameter.
 
  @param buff_obj_ptr
  Unused.
 
  @param dev_action
  Unused.
 
  @param hdet_val
  Pointer to a variable to fill with the read-back HDET value.
 
  @return
  Indicates if the API was successful.
*/
boolean
rfc_wcdma_transceiver_hdet_adapter::read_mdsp_triggered_incident
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint16* hdet_val
)
{
  if ( dev_action != RFDEVICE_EXECUTE_IMMEDIATE )
  {
    RF_MSG_1(RF_ERROR,
             "rfc_wcdma_transceiver_hdet_adapter::read_mdsp_triggered_incident: "
             "Immediate execution is the only option supported. (%d)",
             dev_action);
    return FALSE;
  }

  /* All references in WCDMA MC to the RFDEVICE_GET_HDET_MDSP_CONFIG_DATA
     command use hard-coded device references, but band is used to look
     up the RFC device.  Since band is not provided in this function,
     must default to something. */
  #ifdef FEATURE_WCDMA
  rfdevice_wcdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                 RFCOM_BAND_IMT,
                                 RFDEVICE_GET_MDSP_TRIGGERED_HDET,
                                 hdet_val);
  #endif
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the Read the current HDET value from device  interface to the WCDMA transceiver
  rfdevice_wcdma_tx_hdet_read.
 
  @details
  The WCDMA transceiver read HDET interface does not support scripting,
  so this function will exit with a failure if attempting to call it with
  a script parameter.
 
  @param rfm_dev
  enum indicating the path configuration
 
  @param mdsp_triggered_read
  boolean to indicate if the hdet read is triggered by mDSP or sofwtare
 
  @param hdet_value
  uint16 variable to store the current hdet value 
 
  @param exec_type
  enum indicating direct hardware config or prepare script
 
  @param script_buffer
  pointer to buffer where the device can populate the script
  @return
  Indicates if the API was successful.
*/
boolean
rfc_wcdma_transceiver_hdet_adapter::tx_hdet_read
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  boolean mdsp_triggered_read,
  uint16 *hdet_value,
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
)
{
   
  boolean ret_val = TRUE;

  if( (tx_device != NULL) && 
      (tx_device->func_tbl != NULL) && 
      (tx_device->func_tbl->read_hdet_fp != NULL) )
  {
    /* Enter Device Critical Section, so that the device call is thread safe */
    rf_common_enter_critical_section(tx_device->critical_sect );

    ret_val = tx_device->func_tbl->read_hdet_fp( rfm_dev,
                                       tx_device,
                                       mdsp_triggered_read,
                                       hdet_value, 
                                       RFDEVICE_CREATE_SCRIPT,
                                       buff_obj_ptr );

    /* Leave Device Critical Section */
    rf_common_leave_critical_section(tx_device->critical_sect );
  }
  else
  {
    RF_MSG_1(RF_ERROR,"Null pointer access in tx_hdet_read", 0); 
    ret_val = FALSE;
  }

   /* Debug MSG */
  if(ret_val)
  {
    RF_MSG( RF_MED, "get_hdet_setting success!");
  }

  return(ret_val);  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup the HDET for subsequent hdet read by mDSP/SW 
  
  @details
  
  @param rfm_dev
  enum indicating the path configuration
 
  @param exec_type
  enum indicating direct hardware config or prepare script
 
  @param script_buffer
  pointer to buffer where the device can populate the script
 
*/

boolean
rfc_wcdma_transceiver_hdet_adapter::tx_hdet_setup
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr
)
{
  boolean ret_val = TRUE;

  if( (tx_device != NULL) && 
      (tx_device->func_tbl != NULL) && 
      (tx_device->func_tbl->setup_hdet_fp != NULL) )
  {
    /* Enter Device Critical Section, so that the device call is thread safe */
    rf_common_enter_critical_section(tx_device->critical_sect );

    ret_val = tx_device->func_tbl->setup_hdet_fp( rfm_dev,
                                        tx_device, 
                                        RFDEVICE_CREATE_SCRIPT,
                                        buff_obj_ptr ); 

    /* Leave Device Critical Section */
    rf_common_leave_critical_section(tx_device->critical_sect );
  }
  else
  {
    RF_MSG_1(RF_ERROR,"Null pointer access in tx_hdet_setup", 0);
    ret_val = FALSE;
  }

  /* Debug MSG */
  if(ret_val)
  {
    RF_MSG( RF_MED, "set_hdet_setting success!");
  }

  return(ret_val);  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Required implementation of the common init() function required by all
  classes derived from rfdevice_class.

  @details
  No operations are needed to initialize this adapter class.
*/
void rfc_wcdma_transceiver_hdet_adapter::init()
{
}
#endif
