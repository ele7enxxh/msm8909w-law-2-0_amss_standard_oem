#ifndef RFDEVICE_HDET_WCDMA_H
#define RFDEVICE_HDET_WCDMA_H
/*!
   @file
   rfdevice_hdet_wcdma.h

   @brief
   Interface for WCDMA tech-specific HDET functionality.
*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_hdet_wcdma.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/31/16   sub     Fix QSEQ Max Power Dip
07/18/14   aro     Add critical section protection during HDET read
05/15/14   aro     Interfaces to get ADC trigger script and perform HDET read
10/15/12   kai     Changed setup to hdet_init
09/26/12   vb      Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfdevice_hdet_wcdma_intf.h"
#include "rfdevice_wcdma_type_defs.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

extern "C"
{
#include "rfdevice_type_defs.h"
}

class rfdevice_hdet_wcdma : public rfdevice_class
{
  public:
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    function initializes hdet circuitry in WCDMA mode 
  
    @details
    This function initializes hdet circuitry in WCDMA mode i.e. programs the HW 
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
    ) { return 0;}
  
  /*----------------------------------------------------------------------------*/
    /*!
      @brief
      function returns the hdet config data to be written to WCDMA mdsp. 
  
      @details
      This function returns the hdet scripts/config data to be written to WCDMA mdsp. 
  
      @param data
      HDET scripts to be filled in W MDSP
  
      @return
      boolean indicating pass or fail
    */
    virtual boolean get_mdsp_config_data
    (
      rfdevice_hdet_mdsp_config_data_type* data
    ) { return 0;}
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    function reads the ADC value from HDET connected to incident power path in 
    the context of WCDMA MDSP trigger
  
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
    ) { return 0;}
  
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
    rfcom_wcdma_band_type rf_band,
    boolean mdsp_triggered_read, 
    uint16* hdet_val,
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action
   ){ return 0;}
  
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
    rfcom_wcdma_band_type rf_band,
    rf_device_execution_type dev_action,
    rf_buffer_intf* buff_obj_ptr
  ){ return 0;}

  /*!
    @brief
    Get Script to trigger Power Meter
   
    @details
    This function is called to generate script to trigger WTR4905 ADC to perform power estimation.
   
    @param buff_obj_ptr
    Pointer to structure where Trigger scripts will be populated
   
    @return
    Flag indicating the status of script generation
  */
  virtual boolean get_fw_trigger_script
  (
    rf_buffer_intf* buff_obj_ptr,
    boolean is_ftm    
  ){ return 0;}
    
  /*!
    @brief
    Perform estimated Power Read

    @details
    This function perform the read of the power estimated by the hardware. For FW triggered read,
    ADC trigger is not done in the function. However for non-FW triggered reads, ADC trigger is also
    done as a part of this API.

    @param mdsp_triggered_read
    Flag indicating if MDSP has already triggered the read or not. If MDSP
    has triggered conversion, device driver will only read the results else
    it will trigger conversion within software.

    @param *hdet_output
    Container to store HDET reading and quality indicator 

    @return
    Flag indicating the success of the read
  */
  virtual boolean read_hdet
  (
     boolean mdsp_triggered_read,
     rfdevice_wcdma_hdet_read_type* hdet_output
  ){ return 0;}

  /*!
    @brief
    Interface to enter critical section
  
    @details
    This function will acquire lock to assign critical section for the successding 
    code. The code will be set to critical section  until the lock is released.
    Thus there should not be an attempt to re-acquire the same lock without 
    releasing it. This will result in Deadlock scenario.
   
    @return
    Success/failure status flag
  */
  virtual boolean enter_critical_section
  (
     void
  ){ return 0;}

  /*!
    @brief
    Disable Critical Section
  
    @details
    This function will release lock which was acquired for the preceecing code.
   
    @return
    Success/failure status flag
  */
  virtual boolean leave_critical_section
  (
     void
  ){ return 0;}
};

#endif /*RFDEVICE_HDET_WCDMA_H*/

