/*!
   @file
   rfc_tdscdma_transceiver_hdet_adapter.cpp

   @brief
   Definition of the TDSCDMA Transceiver device to HDET interface adapter.
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/src/rfc_tdscdma_transceiver_hdet_adapter.cpp#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/16/13   zg      Updates to support C++ class based TDSCDMA RX and TX interface
03/27/13   ms      Add HDET read script
03/22/13   ms      Featurize TDSCDMA
03/13/13   ms      Initial version.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_msg.h"

#ifdef FEATURE_TDSCDMA
#include "rfc_tdscdma_transceiver_hdet_adapter.h"
#include "rfdevice_tdscdma_intf.h"
#include "rfdevice_tdscdma_type_defs.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  TDSCDMA Transciever device to HDET interface adapter constructor.
 
  @details
  Must be initialized with the transceiver pointer that this object is
  adapting.
 
  @param cdma_tx_ptr
  Pointer to the TDSCDMA transceiver device that the adapter will use.
*/
rfc_tdscdma_transceiver_hdet_adapter::rfc_tdscdma_transceiver_hdet_adapter
(
  rftdscdma_device_tx_type* tdscdma_tx_ptr
) : tx_device(tdscdma_tx_ptr)
{
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  TDSCDMA Transciever device to HDET interface adapter destructor.
*/
rfc_tdscdma_transceiver_hdet_adapter::~rfc_tdscdma_transceiver_hdet_adapter()
{
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the HDET get_mdsp_config_data() interface to the TDSCDMA transceiver
  RFDEVICE_GET_HDET_MDSP_CONFIG_DATA command interface.
 
  @param data
  HDET scripts to be filled in TDS MDSP
 
  @return
  boolean indicating pass or fail
*/
boolean rfc_tdscdma_transceiver_hdet_adapter::get_mdsp_config_data(rfdevice_hdet_mdsp_config_data_type* data)
{
  /* All references in TDSCDMA MC to the RFDEVICE_GET_HDET_MDSP_CONFIG_DATA
     command use hard-coded device and band parameters, so propagating
     that to here. */
  rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                   RFCOM_TDSCDMA_MODE,
                                   RFCOM_BAND_TDSCDMA_B34,
                                   RFDEVICE_GET_HDET_MDSP_CONFIG_DATA,
                                   data);

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stub function for HDET init in the TDSCDMA HDET interface which has no
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
rfc_tdscdma_transceiver_hdet_adapter::hdet_init
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
  Adapts the HDET get_mdsp_config_data() interface to the TDSCDMA transceiver
  RFDEVICE_GET_HDET_MDSP_CONFIG_DATA command interface.
 
  @details
  The TDSCDMA transceiver read HDET interface does not support scripting,
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
rfc_tdscdma_transceiver_hdet_adapter::read_mdsp_triggered_incident
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint16* hdet_val
)
{
  if ( dev_action != RFDEVICE_EXECUTE_IMMEDIATE )
  {
    RF_MSG_1(RF_ERROR,
             "rfc_tdscdma_transceiver_hdet_adapter::read_mdsp_triggered_incident: "
             "Immediate execution is the only option supported. (%d)",
             dev_action);
    return FALSE;
  }

  /* All references in TDSCDMA MC to the RFDEVICE_GET_HDET_MDSP_CONFIG_DATA
     command use hard-coded device references, but band is used to look
     up the RFC device.  Since band is not provided in this function,
     must default to something. */
  rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                   RFCOM_TDSCDMA_MODE,
                                   RFCOM_BAND_TDSCDMA_B34,
                                   RFDEVICE_GET_MDSP_TRIGGERED_HDET,
                                   hdet_val);

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adapts the Read the current HDET value from device  interface to the TDSCDMA transceiver
  rfdevice_tdscdma_tx_hdet_read.
 
  @details
  The TDSCDMA transceiver read HDET interface does not support scripting,
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
 
  @param buff_obj_ptr
  pointer to buffer where the device can populate the script
  @return
  Indicates if the API was successful.
*/
boolean
rfc_tdscdma_transceiver_hdet_adapter::tx_hdet_read
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  boolean mdsp_triggered_read,
  uint16 *hdet_value,
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
)
{
  boolean ret_val = TRUE;

  if(tx_device != NULL &&
     tx_device->func_tbl != NULL &&
     tx_device->func_tbl->read_hdet_fp != NULL)
  {
    tx_device->func_tbl->read_hdet_fp(rfm_dev, 
                                      tx_device,
                              mdsp_triggered_read,
                              hdet_value,
                                      dev_action,
                              buff_obj_ptr);
  }
  else
  {
    RF_MSG_1(RF_ERROR,
             "tx_hdet_read failed: Null pointer from tx_device. device = %d ",
             rfm_dev);

    ret_val = FALSE;
  }

   /* Debug MSG */
  if(ret_val)
  {
    RF_MSG( MSG_LEGACY_MED, "get_hdet_setting success!");
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
 
  @param buff_obj_ptr
  pointer to buffer where the device can populate the script
 
*/

boolean
rfc_tdscdma_transceiver_hdet_adapter::tx_hdet_setup
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr,
  void* data
)
{
  boolean ret_val = TRUE;

  if(tx_device != NULL &&
     tx_device->func_tbl != NULL &&
     tx_device->func_tbl->setup_hdet_fp != NULL)
  {
    tx_device->func_tbl->setup_hdet_fp(rfm_dev, 
                                       tx_device,
                                       dev_action,
                                buff_obj_ptr,
                                data);
  }
  else
  {
    RF_MSG_1(RF_ERROR,
             "tx_hdet_read failed: Null pointer from tx_device. device = %d ",
             rfm_dev);

    ret_val = FALSE;
  }

  /* Debug MSG */
  if(ret_val)
  {
    RF_MSG( MSG_LEGACY_MED, "get_hdet_setting success!");
  }

  return(ret_val);  

}

/*----------------------------------------------------------------------------*/
boolean
rfc_tdscdma_transceiver_hdet_adapter::tx_hdet_script_read
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr,
  void* data
)
{
  boolean ret_val = TRUE;

  if(tx_device != NULL &&
     tx_device->func_tbl != NULL &&
     tx_device->func_tbl->setup_hdet_fp != NULL)
  {
    tx_device->func_tbl->setup_hdet_fp(rfm_dev, 
                                       tx_device,
                                       dev_action,
                                buff_obj_ptr,
                                data);
  }
  else
  {
    RF_MSG_1(RF_ERROR,
             "tx_hdet_read failed: Null pointer from tx_device. device = %d ",
             rfm_dev);

    ret_val = FALSE;
  }

  /* Debug MSG */
  if(ret_val)
  {
    RF_MSG( MSG_LEGACY_MED, "get_hdet_setting success!");
  }

  return(ret_val);  
}

/*----------------------------------------------------------------------------*/
boolean
rfc_tdscdma_transceiver_hdet_adapter::calc_hdet_measurement
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rfdevice_tdscdma_calc_hdet_measurement_type*  calc_hdet_meas
)
{
  boolean ret_val = TRUE;

  if(tx_device != NULL &&
     tx_device->func_tbl != NULL &&
     tx_device->func_tbl->calc_hdet_measurement_fp != NULL)
  {
    tx_device->func_tbl->calc_hdet_measurement_fp(rfm_dev, 
                                                  tx_device,
                                                  calc_hdet_meas);
  }
  else
  {
    RF_MSG_1(RF_ERROR,
             "tx_calc_hdet_measurement failed: Null pointer. rf_band = %d ",
             rf_band);
    ret_val = FALSE;
  }
   /* Debug MSG */
  if(ret_val)
  {
    RF_MSG( MSG_LEGACY_MED, "get_hdet_setting success!");
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
void rfc_tdscdma_transceiver_hdet_adapter::init()
{
}


#endif /*FEATURE_TDSCDMA*/
