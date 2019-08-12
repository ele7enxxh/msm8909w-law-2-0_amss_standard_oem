/*!
   @file
   rfc_lte_transceiver_hdet_adapter.cpp

   @brief
   APIs for various LTE HDET functionality supported by Transceivers based HDET device
   driver - Enabling/Disabling/reading HDET, returning hdet parameters for
   device, generating HDET operation scripts for firmware operation. 

*/

/*===========================================================================

Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/src/rfc_lte_transceiver_hdet_adapter.cpp#1 $

when       who   what, where, why
--------   ---   ------------------------------------------------------------------- 
03/11/13   sar     Updated file for APQ, GNSS only target.
01/29/13   pv    Helper API for Execution type to boolean
01/15/13   pv    Initial version

============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "comdef.h"
#include "rfcommon_msg.h"

#ifdef FEATURE_LTE
#include "rfc_lte_transceiver_hdet_adapter.h"
#include "rfdevice_lte_interface.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  constructor
 
  @param tx_device
  pointer to the instance of associated WTR1605 LTE tx device
  
*/
rfc_lte_transceiver_hdet_adapter::rfc_lte_transceiver_hdet_adapter
( 
  const rfdevice_lte_tx_type* tx_device
) 
{
  /* Initialize the HDET class for WTR device driver */
  this->current_hdet_mode = LTE_HDET_HPM;
  this->tx_device = tx_device;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  destructor
  
*/
rfc_lte_transceiver_hdet_adapter::~rfc_lte_transceiver_hdet_adapter()
{

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  dummy function
  
  @details
  dummy function
  
*/
void rfc_lte_transceiver_hdet_adapter::init()
{

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  function initializes HDET circuitry in LTE mode 

  @details
  This function initializes HDET circuitry in LTE mode i.e. programs the HW 
  with the required bandwidth and performed HDET Autocal and re-programs the
  Init settings.
 
  This API does not populate external script
 
   @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts   
 
  @param tx_freq
  TX frequency in KHz

  @return
  boolean indicating pass or fail
*/
boolean rfc_lte_transceiver_hdet_adapter::hdet_init
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  uint32 tx_freq,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
)
{
  boolean ret_val = TRUE;

  /* Construct the Script to be sent to Transceiver */
  rfdevice_lte_script_data_type script_data;
  script_data.script = buff_obj_ptr;
  script_data.write_to_hw_flag = this->helper(dev_action);
  
  rfdevice_lte_tx_cmd_dispatch(device, 
                               band, 
                               RFDEVICE_LTE_HDET_INIT, 
                               NULL, 
                               &script_data);
  rfdevice_lte_tx_cmd_dispatch(device, 
                               band, 
                               RFDEVICE_LTE_HDET_DO_AUTOCAL, 
                               NULL, 
                               &script_data);
  /*Init HDET after autocal*/
  rfdevice_lte_tx_cmd_dispatch(device, 
                               band, 
                               RFDEVICE_LTE_HDET_INIT, 
                               NULL, 
                               &script_data);

  return(ret_val);  
} /* hdet_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the HDET power detector circuit.

  @details
  This function is used if the driver wants to manually manage the HDET
  circuit.
 
  External script can be used along with this API
 
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts     
 
  @param hdet_data
  hdet_data provides the info about the various settings needs for enabling HDET.     
 
  @return
  boolean indicating pass or fail 
*/
boolean rfc_lte_transceiver_hdet_adapter::enable_hdet
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_enable_type *hdet_data,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
)
{

  boolean ret_val = TRUE;

  /* Construct the Script to be sent to Transceiver */
  rfdevice_lte_script_data_type script_data;
  script_data.script = buff_obj_ptr;
  script_data.write_to_hw_flag = this->helper(dev_action);

  rfdevice_lte_tx_cmd_dispatch( device,
                                band, 
                                RFDEVICE_LTE_HDET_ENABLE, 
                                hdet_data, &script_data);
  /* Debug MSG  */
  if(ret_val)
  {
    RF_MSG( MSG_LEGACY_MED, "enable_hdet success!");
  }

  return(ret_val);  
} /* enable_hdet */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable the HDET power detector circuit.

  @details
  This function is used if the driver wants to manually manage the HDET
  circuit.
 
  External script can be used along with this API
 
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts     
 
  @return
  boolean indicating pass or fail 
*/
boolean rfc_lte_transceiver_hdet_adapter::disable_hdet
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
)
{
   boolean ret_val = TRUE;

  /* Construct the Script to be sent to Transceiver */
  rfdevice_lte_script_data_type script_data;
  script_data.script = buff_obj_ptr;
  script_data.write_to_hw_flag = this->helper(dev_action);

  
  rfdevice_lte_tx_cmd_dispatch( device,
                                band, 
                                RFDEVICE_LTE_HDET_DISABLE, 
                                NULL, &script_data);
  /* Debug MSG */
  if(ret_val)
  {
    RF_MSG( MSG_LEGACY_MED, "disable_hdet success!");
  }

  return(ret_val);  
} /* disable_hdet */


boolean rfc_lte_transceiver_hdet_adapter::do_hdet_read
(
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    rfdevice_lte_hdet_settings_type *hdet_data
)
{
   return TRUE;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  do_full_hdet.
  
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @hdet_data
  Indicates the LTE hdet data. The HDET value is stored and sent back in the
  same structure.
 
  @return
  boolean indicating pass or fail
*/
boolean rfc_lte_transceiver_hdet_adapter::do_full_hdet
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_settings_type *hdet_data,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
)
{
  boolean ret_val = TRUE;

  /* Construct the Script to be sent to Transceiver */
  rfdevice_lte_script_data_type script_data;
  script_data.script = buff_obj_ptr;
  script_data.write_to_hw_flag = this->helper(dev_action);

  rfdevice_lte_tx_cmd_dispatch( device,
                                band, 
                                RFDEVICE_LTE_DO_FULL_HDET, 
                                hdet_data, &script_data);

  /* Debug MSG  */
  if(ret_val)
  {
    RF_MSG( MSG_LEGACY_MED, "do_full_hdet success!");
  }

  return(ret_val);  
} /* do_full_hdet */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  get_hdet_setting

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param lte_hdet_therm_settings
  hdet_data provides the info about the various settings needs for enabling HDET.
 
  @param script_data_ptr
  The script data ptr to retreive the HDET settings.
 
  @return
  boolean indicating pass or fail
*/
boolean rfc_lte_transceiver_hdet_adapter::get_hdet_setting
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
  rfdevice_lte_script_data_type* script_data_ptr,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
)
{
   boolean ret_val = TRUE;

  /* Construct the Script to be sent to Transceiver */
  rfdevice_lte_script_data_type script_data;
  script_data.script = buff_obj_ptr;
  script_data.write_to_hw_flag = this->helper(dev_action);

  /* Implementation of get_hdet_setting */
  rfdevice_lte_tx_cmd_dispatch( device, 
                                band, 
                                RFDEVICE_LTE_GET_TX_HDET_SETTINGS_V2, 
                                lte_hdet_therm_settings, 
                                &script_data );        

  // Debug MSG  
  if(ret_val)
  {
    RF_MSG( MSG_LEGACY_MED, "get_hdet_setting success!");
  }

  return(ret_val);  
} /* get_hdet_setting */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  get_hdet_val

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param lte_hdet_therm_settings
  hdet_data provides the info about the various settings needs for enabling HDET.
 
  @param script_data_ptr
  The script data ptr to retreive the HDET settings.
 
  @return
  boolean indicating pass or fail
*/
boolean rfc_lte_transceiver_hdet_adapter::get_hdet_val
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
)
{
  boolean ret_val = TRUE;

  /* Construct the Script to be sent to Transceiver */
  rfdevice_lte_script_data_type script_data ;
  script_data.script = buff_obj_ptr;
  script_data.write_to_hw_flag = this->helper(dev_action);

  rfdevice_lte_tx_cmd_dispatch( device,
                                band,
                                RFDEVICE_LTE_GET_TX_PDET_ADC_VALUE, 
                                lte_hdet_therm_settings, &script_data);
  /* Debug MSG */
  if(ret_val)
  {
    RF_MSG( MSG_LEGACY_MED, "get_hdet_setting success!");
  }

  return(ret_val);  
} /* get_hdet_setting */

/* Need this helper function to support all the script data going to device */
boolean rfc_lte_transceiver_hdet_adapter::helper(rf_device_execution_type dev_action)
{
   if (dev_action == RFDEVICE_EXECUTE_IMMEDIATE )
   {
      return TRUE;
   }
   else 
   {
      return FALSE;
   }
}

#endif /* defined FEATURE_LTE */
#endif
