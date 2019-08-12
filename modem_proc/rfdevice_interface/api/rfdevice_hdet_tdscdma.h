#ifndef RFDEVICE_HDET_TDSCDMA_H
#define RFDEVICE_HDET_TDSCDMA_H
/*!
   @file
   rfdevice_hdet_tdscdma.h

   @brief
   Interface for TDSCDMA tech-specific HDET functionality.
*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_hdet_tdscdma.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/04/14   whz     Add critical section for HDET
03/22/13   ms      Featurize TDSCDMA
03/22/13   ms      Compilation fix for Nikel
02/28/13   ms      Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rf_buffer_intf.h"
#include "rfdevice_class.h"


extern "C"
{
#include "rfdevice_type_defs.h"
}

#ifdef FEATURE_TDSCDMA
#include "rfdevice_tdscdma_type_defs.h"

class rfdevice_hdet_tdscdma : public rfdevice_class
{
public:
/*----------------------------------------------------------------------------*/
/*!
  @brief
  function initializes hdet circuitry in TDSCDMA mode 

  @details
  This function initializes hdet circuitry in TDSCDMA mode i.e. programs the HW 
  with the required bandwidth

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    

  @return
  boolean indicating pass or fail
*/
  virtual boolean hdet_init
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action,
    uint32 tx_freq
  ) = 0;

/*----------------------------------------------------------------------------*/
  /*!
    @brief
    function returns the hdet config data to be written to TDSCDMA mdsp. 

    @details
    This function returns the hdet scripts/config data to be written to TDSCDMA mdsp. 

    @param data
    HDET scripts to be filled in TDS MDSP

    @return
    boolean indicating pass or fail
  */
  virtual boolean get_mdsp_config_data
  (
    rfdevice_hdet_mdsp_config_data_type* data
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  function reads the ADC value from HDET connected to incident power path in 
  the context of TDSCDMA MDSP trigger

  @details

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    
  
  @param hdet_val
  hdet value read from hardware

  @return
  boolean indicating pass or fail
*/
  virtual boolean read_mdsp_triggered_incident
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action,
    uint16* hdet_val
  ) = 0;

  /*----------------------------------------------------------------------------*/

/*!
  @brief
  Read the current HDET value from device 
  
  @details
  
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
 
*/

 virtual boolean tx_hdet_read
 (
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  boolean mdsp_triggered_read, 
  uint16* hdet_val,
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
 )= 0;

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
 virtual boolean tx_hdet_setup
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr,
  void* data
)=0;

 virtual boolean tx_hdet_script_read
(   
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr,
  void* data
)=0;

virtual boolean calc_hdet_measurement
(
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rfdevice_tdscdma_calc_hdet_measurement_type*  calc_hdet_meas
)=0;

/* FIXME: pure virtual? */
virtual boolean enter_critical_section
(
  void
){return FALSE;}

/* FIXME: pure virtual? */
virtual boolean leave_critical_section
(
  void
){return FALSE;}

};

#endif

#endif /*RFDEVICE_HDET_TDSCDMA_H*/

