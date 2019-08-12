#ifndef RFDEVICE_HDET_CMN_H
#define RFDEVICE_HDET_CMN_H
/*!
  @file
  rfdevice_hdet_cmn.h

  @brief
  Technology independent HDET interfaces.
*/

/*===========================================================================

Copyright (c) 2012 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_hdet_cmn.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/29/13   adk     Read and store temp (K-sensor) values for QFE TDET
03/13/13   aca     Tuner power characterization
01/23/13   ndb     API updates for WTR1605 compatibility 
10/26/12   kai     Added program_k0_val, removed do_k_sensor_cal
10/16/12   kai     Added API getting tech specific HDET object pointer
10/16/12   kai     Added trigger_flag to get_ref_val
10/15/12   kai     Added common APIs and FTM APIs
09/26/12   vb      Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfdevice_hdet_cmn_intf.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

class rfdevice_hdet_cmn : public rfdevice_class
{
public:
/*----------------------------------------------------------------------------*/
/*!
  @brief
  function for all common hdet auto cal routines

  @details
  rfdevice_hdet_cmn_auto_cal()

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    

  @return
  boolean indicating pass or fail
*/
  virtual boolean auto_cal
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  function sets ADC mux to HDET connected to incident power path. 

  @details
  This function will configure the ADC to incident power HDET.

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    
  
  @return
  boolean indicating pass or fail  
*/
  virtual boolean hkadc_set_incident_path
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  function sets ADC mux to HDET connected to relfected power path. 

  @details
  This function will configure the ADC to reflected power HDET.

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    
  
  @return
  boolean indicating pass or fail  
*/
  virtual boolean hkadc_set_ref_path
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  function reads the ADC value from HDET connected to incident power path. 

  @details
  This function will read the ADC value from HDET connected to incident power
  path.

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    
  
  @param hdet_inc_pwr
  pointer to hdet incident power value
  
  @param trigger_flag
  Flag indicates whether trigger HKADC before reading inc_pwr value.
  Set to TRUE if need to trigger HDET read

  @return
  boolean indicating pass or fail  
*/
  virtual boolean get_incident_val
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action,
    uint16* hdet_inc_pwr,
    boolean trigger_flag
  ) = 0;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  function reads the power of the pid of interest. 

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    
  
  @param hdet_pwr
  pointer to hdet power value
  
  @param hdet_pid
  Pid of interest

  @return
  boolean indicating pass or fail  
*/

virtual boolean get_power
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint8 hdet_pid,
  uint16* hdet_pwr
) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  function reads the ADC value from HDET connected to relected power path. 

  @details
  This function will read the ADC value from HDET connected to reflected power
  path.

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    
  
  @param hdet_ref_pwr
  pointer to hdet relected power value
  
  @param trigger_flag
  Flag indicates whether trigger HKADC before reading ref_pwr value.
  Set to TRUE if need to trigger HDET read  
  
  @return
  boolean indicating pass or fail  
*/
  virtual boolean get_ref_val
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action,
    uint16* hdet_ref_pwr,
    boolean trigger_flag
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function performs DC cal and stores the DC offset values to the
  instance
  
  @details
  This function performs DC cal and stores the DC offset values to the
  instance. It's used in FTM mode and online mode. No change on the GS.
  
  This API does not populate external script

  @return
  boolean indicating pass or fail 
*/
  virtual boolean do_hdet_dc_cal( void ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function disables HDET
  
  @details
  This function disables HDET
  
  External script can be used along with this API

  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    
  
  @return
  boolean indicating pass or fail  
*/
  virtual boolean disable_hdet
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function get the pointer to tech specific hdet instance

  @details
  This function get the pointer to tech specific hdet instance.
  The tech specific hdet instance will be created only once.
 
  @return
  void pointer to tech specific hdet instance
*/
  virtual void* get_hdet_tech_object( rfm_mode_enum_type tech ) = 0;

/*******************************************************************************
                                 K Sensor APIs
*******************************************************************************/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the instantaneous K-sensor val (k2 - k1).
  
  @details
  This function returns the instantaneous K-sensor val (k2 - k1).
  
  This API does not populate external script
    
  @param k_val
  pointer to the K-sensor val

  @return
  boolean indicating pass (TRUE) or fail (FALSE)  
*/
  virtual boolean get_k_sensor_val
  (
    uint16* k_val
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function does temperature compensation for QFE TDET.

  @details
  This function updates the temp comp parameters in the TDET object.

  If TDET is not being used then this API returns FALSE.

  @param rf_buff
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to HW or build scripts

  @return
  boolean indicates pass (TRUE) or fail (FALSE)
*/
  virtual boolean update_temp_comp_param( rf_buffer_intf* rf_buff,
                                          rf_device_execution_type dev_action ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns temperature compensation data to QFE
  
  @details
  This function updates the temp comp parameters in the TDET object.

  If TDET is not being used then this API returns FALSE.

  @param temp_comp_data_ptr
  ptr to the temp comp data. This data will be stored in the TDET object.

  @return
  boolean indicates pass (TRUE) or fail (FALSE)
*/
  virtual boolean update_temp_comp_param_cb( qfe_script_type* temp_comp_data_ptr ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Program k0 value to instance
  
  @details
  This function program k0 value to instance and store converted agc val 
  based on k0 to instance if it is valid
    
  @param k0_val
  k0 value

  @return
  boolean indicating pass (TRUE) or fail (FALSE)  
*/
  virtual boolean program_k0_val
  (
    uint16 k0_val
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  update K sensor TxAGC compensation value
  
  @details
  This function reads the k sensor value, calculates the required 
  TxAGC compensation value with respect to k0 and stores/updates it in instance.
  This function is called when doing Tx temp comp
  
  This API does not populate external script

  @return
  boolean indicating pass (TRUE) or fail (FALSE)  
*/
  virtual boolean update_k_comp( void ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the TxAGC compensation value based on k sensor
  correction
  
  @details
  This function returns the TxAGC compensation value based on k sensor
  correction
  
  This API does not populate external script

  @return int16
  the TxAGC compensation value based on k sensor
  correction
*/
  virtual int16 get_k_comp( void ) = 0;

/*******************************************************************************
                                    FTM APIs
*******************************************************************************/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the HDET gain state.
  
  @details
  This function sets the HDET gain state. HDET inititalization must have been
  completed before this API can be called.
  
  External script can be used along with this API
 
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    

  @param hdet_gain_state
  HDET gain state to be set

  @hdet_pid
  PID of interest. Defalted to 0.

  @return
  boolean indicating pass (TRUE) or fail (FALSE)   
*/
virtual boolean set_gain_state
  (
    rf_buffer_intf* buff_obj_ptr,
    rf_device_execution_type dev_action,
    rfdevice_hdet_gain_state_type hdet_gain_state,
    uint8 hdet_pid = 0
  ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets for power read.
  
  @param buff_obj_ptr
  void pointer to CCS buffer object

  @param dev_action
  indicates if it is immediate write to hw or build scripts    

  @param num_avg
  Averaging

  @wait_time
  Time to wait between trigger and read

  @return
  boolean indicating pass (TRUE) or fail (FALSE)   
*/

virtual boolean setup_power_read
(
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action,
  uint8 num_avg,
  uint16 wait_time
) = 0;




/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get DC offset values
  
  @details
  This function gets the DC offset values for both incident and reflected     
  power HDETs for the current HDET gain state in use.
  
  This API does not populate external script 
 
  @return
  boolean indicating pass (TRUE) or fail (FALSE)   
*/
  virtual boolean get_dc_offset_val
  (
    uint16* hdet_inc_dc_offset_val,
    uint16* hdet_ref_dc_offset_val
  ) = 0;

 virtual boolean get_gated_incident_val
 (
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, //to be removed
    uint16 *hdet_inc_pwr,
    boolean trigger_flag,int delay,int number_of_samples
  ) = 0;

};

#endif /*RFDEVICE_HDET_CMN_H*/

