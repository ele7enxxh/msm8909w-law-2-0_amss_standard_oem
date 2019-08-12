#ifndef RFDEVICE_TRX_CDMA_TX_H
#define RFDEVICE_TRX_CDMA_TX_H
/*!
   @file
   rfdevice_trx_cdma_tx.h

   @brief

*/

/*===========================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_cdma_tx.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
05/19/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
03/06/15   hs    Added a parameter in create_tune_script() to indicate whether the 
                 channel passed in is a Rx channel or not.
09/02/14   shb   CDMA TX Tune API update to support TX WXEfication
12/09/13   rp    Added an interface to retrieve common device pointer associated with 
                 given tech trx class.
11/07/13   rp    Added and API to return common device class pointer.
08/29/13   shb   Added FEATURE_1X for modem_lite build compilation
08/27/13  shb/sr Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "rf_buffer_intf.h"
#ifdef FEATURE_CDMA1X
#include "rfdevice_cdma_type_defs.h"

#ifdef __cplusplus
#include "rfdevice_class.h"
#include "rfdevice_rxtx_common_class.h"

class rfdevice_trx_cdma_tx:public rfdevice_class
{

public:
  rfdevice_trx_cdma_tx();

  virtual ~rfdevice_trx_cdma_tx();

  /*----------------------------------------------------------------------------*/
  /*! 
    @brief
    Enters critical section lock for the device
  */
  virtual boolean enter_critical_section( void ) = 0;

  /*----------------------------------------------------------------------------*/
  /*! 
    @brief
    Leaves critical section lock for the device
  */
  virtual boolean leave_critical_section( void ) = 0;


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Program CDMA Tx tune script
    
    @details
    This function will generate the RTR Tx device tuning script for FW. The RF
    device passed into this function will be mapped into a physical path of the
    current RTR chipset by querying RFC.
  
    This function will not perform any SBI transaction or any actions on hardware
    
    @param band
    Tx band for which the script is to be generated
   
    @param chan
    Tx channel for which the script is to be generated
   
    @param tx_bw_khz
    Bandwidth of TX baseband in kHz. Used to determine analog RF baseband settings
    of transmit path
   
    @param dpd_state
    Indicates if we need to enable TX path for digital pre-distortion or without
    pre-distortion support. To support digital pre-distortion, device may need
    to choose different SSBI settings to widen bandwidth in some cases
     
    @param *script_data_ptr
    Data struct that contains pointers to the buffers to be populated
    
    script_data_ptr->preload_action
    Indicates if preload script should be executed immediately/internally
    or populated in preload_buf for execution later
    
    script_data_ptr->preload_buf
    Pointer to preload script buffer. Script will be populated here if
    script_data_ptr->preload_action == RFDEVICE_MEAS_CREATE_SCRIPT
    
    script_data_ptr->meas_action
    Indicates if trigger script should be executed immediately/internally
    or populated in trigger_buf for execution later
    
    script_data_ptr->meas_buf
    Pointer to trigger script buffer. Script will be populated here if
    script_data_ptr->meas_action == RFDEVICE_MEAS_CREATE_SCRIPT
    
    script_data_ptr->buffer_mask
    Unique id identifying the WXE buffer being used for this TX script
   
    @param is_chan_rx
    Flag to indicate whether the chan passed is a RX channel or not

    @param wtr_config_bitmask
    Pointer to a bitmask which will be populated for logging WTR configuration

  */
  virtual boolean create_tune_script(rfm_mode_enum_type mode,
                                     rfm_cdma_band_class_type band,
                                     rfm_cdma_chan_type chan,
                                     uint32 tx_bw_khz,
                                     rfdevice_dpd_enum_type dpd_state,
                                     rfdevice_meas_script_data_type *script_data_ptr,
                                     boolean is_chan_rx = FALSE,
                                     rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL
                                     ) = 0;

  /*----------------------------------------------------------------------------*/
  
  /*!
    @brief
    Creates a script that puts TX in RTR to sleep
    
    @details
    This function will generate a RTR Rx Device script for FW. The RF
    device passed into this function will be mapped into a physical path of the
    current RTR chipset by querying RFC.
  
    This function will not perform any SBI transaction or any actions on hardware
    
    @param device
    RF Device for which Rx sleep script is to be generated
   
    @param *script_data
    data structure in which the SBI transactions are to be populated  
  */
  virtual boolean create_sleep_script(rf_buffer_intf *buf_obj_ptr) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Generic interface for RF TX device functions that do not merit its own
    function pointer in device virtual function table
  
    @details
    It provides IOCTL kind of interface to get and set device specific
    parameters. Must be used for functions that are specific to a RF device to
    avoid forcing other devices that do not support that functionality to
    implement the function. Also may be used for simple device data queries 
    that do not deserve a seperate function in virtual function table.
    
    @param cmd
    Specifies the command to performed by the device. Should be from enum
    rfdevice_cmd_enum_type
  
    @param *data
    Void data pointer to handle input and/or output parameter for the command.
  */
  virtual boolean cmd_dispatch(rfdevice_cmd_enum_type cmd,
                               void *data) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This API takes in the band and PA state and table_name (part of tx_lut_data), 
    and returns a pointer to the correct LUT table
  
    @details
    The purpose of this API is to return a pointer to a LUT table that matches the 
    table_name which is sent as part of input parameter in tx_lut_data
     
    @param *tx_lut_data
    The data capsule that contains input and output variables related to LUT
  
    @param table_type
    high, mid or low LUT table to be used 
  
  */
  virtual boolean get_lut_table(rfdevice_lut_data_type* tx_lut_data,
                                rfdevice_cdma_lut_type table_type) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Return therm reading and quality indicator
  
    @details
    This API reads the therm, and also checks to see if the therm is in a valid 
    state
    
    @param mdsp_triggered_read
    Flag indicating if MDSP has already triggered therm read or not. 
   
    @param *therm_output
    Container to store therm reading and quality indicator 
  
    @param pt_daa
    Pointer to user data
  */
  virtual boolean read_therm(boolean mdsp_triggered_read,
                             rfdevice_cdma_therm_read_type *therm_output) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get SSBI scripts for therm operation thru MDSP, used for DIME only
    
    @details
    Gets SSBI sequences to turn thermistor ON, trigger HKADC conversion, turn 
    thermistor OFF, and to Setup Thermistor
    
    @param *temp_comp_settings
    Pointer to buffer where thermistor scripts will be populated
   
    @param *therm param
    Pointer to structure storing thermistor params (settling time/conversin time)
   
    @return
    TRUE if tx_device and func_tbl are valid and get_therm_scripts func ptr could
    be called. FALSE otherwise.
  */
  virtual boolean get_therm_scripts(rf_buffer_intf *temp_comp_settings,
                                    rfdevice_cdma_therm_param_type *therm_param) = 0;
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Setup initial scripts for therm operation thru MDSP
    
    @details
    Do initial sequences to turn thermistor ON
    
    @param mode
    Mode, i.e., 1x,hdr,or lte etc., of current operation
  
    @param pt_data
    Pointer to user data 
  */
  virtual boolean setup_therm(rfcom_mode_enum_type mode) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Query thermistor parameters from device
   
    @details
    This function is used to query all thermistor parameters pertaining to the
    current Tx device. This includes pdet settling time, hkadc conversion time.
    
    @param therm_param
    Container where thermistor parameters will be populated by the device driver 
  */
  virtual boolean get_therm_param(rfdevice_cdma_therm_param_type *therm_param) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Provides the recommended DAC Iref to be used for the current Tx Band
    
    @param *iref_data
    Pointer to structure where Iref info will be populated
  
    @return
    TRUE if tx_device and func_tbl are valid and get_hdet_scripts func ptr could
    be called. FALSE otherwise.
  */
  virtual boolean get_dac_cal_iref(rfdevice_cdma_dac_cal_iref_type* iref_data) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Tx band specific data to device
    
    @param band
    CDMA band whose data is being pushed
   
    @param data_array
    Array of int32 containing device Tx data for band
   
    @param array_size
    Size of array of int32 data to be pushed to device
  */
  virtual boolean set_band_data(rfm_cdma_band_class_type band,
                                int32* data_array,
                                uint8 array_size) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Tx band specific data to device
    
    @param band
    CDMA band whose data is being pushed
   
    @param data_array
    Array of int32 containing device Tx data for band
   
    @param array_size
    Size of array of int32 data to be pushed to device
   
    @param *cdma_tx_pa_lut
    Pointer to array for storing LUT information
  */
  virtual boolean get_lut_data(rfm_cdma_band_class_type band,
                               int32* data_array,
                               uint8 array_size,
                               rfdevice_cdma_lut_type *cdma_tx_pa_lut) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get SSBI script for enabling/disabling Tx chain
    
    @param rf_on
    TRUE for RF ON event, FALSE for RF OFF event
   
    @param script_buffer
    Pointer to the buffer script for storing on/off settings
  */
  virtual boolean get_tx_on_off_script(boolean rf_on,
                                       rf_buffer_intf* script_buffer,
                                       int16 delay) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Tx band specific data to device
    
    @param mode
    RF Mode for which RF Device is to be tuned  
   
    @param rf_band
    Tx band for which the device is configured
   
    @param chan
    Tx channel for which the device is configured
   
    @param tx_bw_khz
    Bandwidth of TX baseband in kHz. Used to determine analog RF baseband settings
    of transmit path
   
    @param dpd_state
    Indicates if we need to enable TX path for digital pre-distortion or without
    pre-distortion support. To support digital pre-distortion, device may need
    to choose different SSBI settings to widen bandwidth in some cases
  
    @execution_type
    indicates immediate write or populate script buffer
     
    @param *buf_obj_ptr
    Data structure in which the all transactions are to be populated
   
  */
  virtual boolean tune_default_port(rfcom_mode_enum_type mode,  
                                    rfm_cdma_band_class_type rf_band,
                                    rfm_cdma_chan_type chan,
                                    uint32 tx_bw_khz,
                                    rfdevice_dpd_enum_type dpd_state,
                                    rf_device_execution_type execution_type,
                                    rf_buffer_intf *buf_obj_ptr) = 0;
  
  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Returns the ET delay compensation to be applied
  
    @details
    ET delay compensation value depends on RC error, process, TX port being used.
   
    This device API will return the delay comp based on these inputs
   
    @param delay_comp_x10ns
    OUTPUT. Delay compensation to be applied in 1/10th of a ns unit
   
    @retval TRUE
    Indicates the API was successful and a correct delay comp value has been
    returned
   
    @retval FALSE
    Indicates API failed due to bad parameters
  */
  virtual boolean get_et_delay_compensation(int16* delay_comp_x10ns) = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get rfdevice_rxtx_common_class pointer associated with this instance of device.

    @return 
    Pointer to rxtx common device associated with current instance of device.
  */
  virtual 
  rfdevice_rxtx_common_class* get_common_device_pointer() = 0;

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean tx_pwr_sleep( void )
  {
    RF_MSG( RF_ERROR, "tx_pwr_sleep: Derived class does not support this API!" );
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean tx_pwr_wakeup (void ) 
  {
    RF_MSG( RF_ERROR, "tx_pwr_wakeup: Derived class does not support this API!" );
    return FALSE;
  }
};

#else

struct rfdev_cdma_tx;
typedef struct rfdev_cdma_tx rfdevice_trx_cdma_tx;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
*/
rfdevice_rxtx_common_class* 
rfdevice_cdma_tx_get_common_device_pointer(rfdevice_trx_cdma_tx* tx_device);


#endif /* #ifdef __cplusplus */

#endif /* FEATURE_CDMA1X */

#endif /* RFDEVICE_TRX_CDMA_TX_H */
