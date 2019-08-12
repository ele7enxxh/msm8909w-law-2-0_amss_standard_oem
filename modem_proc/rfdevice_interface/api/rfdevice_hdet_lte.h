#ifndef RFDEVICE_HDET_LTE_H
#define RFDEVICE_HDET_LTE_H
/*!
   @file
   rfdevice_hdet_lte.h

   @brief
   APIs for various LTE HDET functionality supported by HDET device driver
   Enabling/Disabling/reading HDET, returning hdet parameters for device,
   generating HDET operation scripts for firmware operation.
*/

/*===========================================================================

Copyright (c) 2012-13 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_hdet_lte.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/16/14   sbm     HDET online brinup
01/15/13   pv      API updates for Transceiver compatibility.
11/29/12   av      Using proper featurization and removing featurization 
                    that is not needed
11/20/12   gvn     Featurize LTE for Triton 
10/20/12   kai     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

#ifdef FEATURE_LTE
#include "rfdevice_hdet_lte_intf.h"
#endif

extern "C"
{
#include "rfdevice_type_defs.h"
}

#ifdef FEATURE_LTE

class rfdevice_hdet_lte:public rfdevice_class
{
public:
/*----------------------------------------------------------------------------*/
/*!
  @brief
  function initializes HDET circuitry in LTE mode 

  @details
  This function initializes HDET circuitry in LTE mode i.e. programs the HW 
  with the required bandwidth

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    

  @param tx_freq
  Indicates Tx freq.

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.

  @return
  boolean indicating pass or fail
*/
virtual boolean hdet_init
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  uint32 tx_freq,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
) =0;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the HDET power detector circuit.

  @details
  This function is used if the driver wants to manually manage the HDET
  circuit to reduce overhead in readings or to insure the power detector
  has sufficient time to stabilize.
 
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param hdet_data
  Indicates hdet params needed to enable HDET circuitry

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.
 
  @return
  boolean indicating pass or fail
*/
virtual boolean enable_hdet
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_enable_type *hdet_data,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable the HDET power detector circuit.

  @details
  This function is used if the driver wants to manually manage the HDET
  circuit to reduce overhead in readings or to insure the power detector
  has sufficient time to stabilize.
 
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.
 
  @return
  boolean indicating pass or fail
*/
virtual boolean disable_hdet
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  do_full_hdet.
  
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param hdet_data
  Indicates hdet params needed to enable HDET circuitry

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.
 
  @return
  boolean indicating pass or fail
*/
virtual boolean do_full_hdet
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_settings_type *hdet_data,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
) =0;

virtual boolean do_hdet_read
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_settings_type *hdet_data
) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  get_hdet_setting
  
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param lte_hdet_therm_settings
  Indicates hdet params for specific settings
 
  @param script_data_ptr
  Script data pointer which will return the script from RF device.

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.

  @return
  boolean indicating pass or fail
*/
virtual boolean get_hdet_setting
(
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
  rfdevice_lte_script_data_type* script_data_ptr,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
) =0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  get_hdet_val

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
   @param lte_hdet_therm_settings
  Indicates hdet params for specific settings

  @param device
  Indicates Logical RFM device

  @param band
  Indicates RF LTE Band for Tx.
 
  @return
  boolean indicating pass or fail
*/
virtual boolean get_hdet_val
(
  rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    rfdevice_lte_hdet_therm_data_type* lte_hdet_therm_settings,
  rfm_device_enum_type device,
  rfcom_lte_band_type band
) =0;

};
#endif /*FEATURE_LTE */
#endif /*RFDEVICE_HDET_LTE_H*/

