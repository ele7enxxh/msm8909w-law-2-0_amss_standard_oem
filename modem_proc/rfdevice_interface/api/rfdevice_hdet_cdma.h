#ifndef RFDEVICE_HDET_CDMA_H
#define RFDEVICE_HDET_CDMA_H
/*!
   @file
   rfdevice_hdet_cdma.h

   @brief
   APIs for various CDMA HDET functionality supported by HDET device driver
   Enabling/Disabling/reading HDET, returning HDET parameters for device,
   generating HDET operation scripts for firmware operation.
*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_hdet_cdma.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/22/14   sbo     Add critical section protection during HDET read
03/11/13   sar     Updated file for APQ, GNSS only target.
12/06/12   bmg     Added get_fw_trigger_script() interface.
10/15/12   kai     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfdevice_hdet_cdma_intf.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

extern "C"
{
#include "rfdevice_type_defs.h"
}

class rfdevice_hdet_cdma : public rfdevice_class
{
public:
/*----------------------------------------------------------------------------*/
/*!
  @brief
  function initializes HDET circuitry in CDMA mode 

  @details
  This function initializes HDET circuitry in CDMA mode i.e. programs the HW 
  with the required bandwidth

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    

  @return
  Boolean indicating pass or fail
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
  Setup TX RF device for HDET reads
 
  @details
  Will execute setup transmit device for HDET reads. This interface function
  should be called everytime before scheduling HDET reads.
 
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  Indicates if it is immediate write to hw or build scripts
 
  @param power_mode
  Specify whether HDET must be set up for high power mode or
  low power mode
 
  @return
  Boolean indicating pass or fail
*/
  virtual boolean setup_hdet
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action,
    rfdevice_cdma_hdet_power_mode_type power_mode
  ) = 0;

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
 
  @return
  boolean indicating pass or fail
*/
  virtual boolean enable_hdet
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action
  ) = 0;

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
 
  @return
  Boolean indicating pass or fail
*/
  virtual boolean disable_hdet
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return HDET reading and quality indicator

  @details
  This API reads the HDET, and also checks to see if the HDET is in a valid 
  state (i.e, if the HDET is enabled)

  @param mdsp_triggered_read
  Flag indicating if MDSP has already triggered HDET read or not. If MDSP
  has triggered conversion, device driver will only read HDET results else
  it will trigger conversion within software
 
  @param *hdet_output
  Container to store HDET reading and quality indicator 
 
  @return
  Boolean indicating pass or fail
*/
  virtual boolean read_hdet
  (
    boolean mdsp_triggered_read,
    rfdevice_cdma_hdet_read_type* hdet_output
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return average HDET reading for N samples of reads

  @details
  This API reads N samples of HDET and returns the average HDET value. This
  function should be called only after HDET is enabled. For the case when HDET
  is not enabled, this function will return invalid HDET read

  @param num_of_reads
  Number of HDET samples to be taken for averaging
 
  @return
  Average HDET read after sampling
*/
  virtual uint16 multi_read_hdet
  (
    uint16 num_of_reads
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query HDET parameters from device (hard coded)
 
  @details
  This function is used to query all HDET parameters pertaining to the
  current Tx device. This includes pdet settling time, hkadc conversion time
  as well as inputs to the hdet power limiting loop like polling periods, 
  error filter gains etc
 
  @param hdet_param
  Container where HDET parameters will be populated by the device driver
 
  @return
  boolean indicating pass or fail 
*/
  virtual boolean get_hdet_param
  (
    rfdevice_cdma_hdet_param_type* hdet_param
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get SSBI scripts for HDET operation thru MDSP
  
  @details
  Gets SSBI sequences to turn PDET ON, trigger HKADC conversion and turn PDET 
  OFF
 
  @param *hdet_scripts
  Pointer to structure where HDET scripts will be populated

  @return
  TRUE if tx_device and func_tbl are valid and get_hdet_scripts func ptr could
  be called. FALSE otherwise.
*/
  virtual boolean get_hdet_scripts
  (
    rfdevice_cdma_hdet_scripts_type* hdet_scripts
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get a script to enable and trigger HDET in rf_buffer format.
  
  @details
  The contents of the script will schedule the following events at the
  appropriate time relative to the Tx AGC update (at t=0) in order to latch
  the estimate of the current average power in the HDET chip:
  * Enable PDET circuit before the Tx AGC update
  * Trigger HKADC conversion after enough time has elapsed for the PDET
    analog filter to converge
  * Disable PDEt circuit after HKADC conversion is complete
 
  @param *buff_obj_ptr
  Pointer to an RF script buffer object that will be filled with the timed
  sequence.

  @return
  Boolean indicating success or failure constructing the script.
*/
  virtual boolean get_fw_trigger_script
  (
    rf_buffer_intf* buff_obj_ptr
  ) = 0;

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
#endif /*  FEATURE_CDMA1X */
#endif /*RFDEVICE_HDET_CDMA_H*/

