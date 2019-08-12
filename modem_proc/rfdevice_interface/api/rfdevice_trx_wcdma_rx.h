#ifndef RFDEVICE_TRX_WCDMA_RX_H
#define RFDEVICE_TRX_WCDMA_RX_H
/*!
   @file
   rfdevice_trx_wcdma_rx.h

   @brief

*/

/*==============================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_wcdma_rx.h#1 $

when       who    what, where, why
--------   ---   --------------------------------------------------------------- 
09/16/15   sd    Interface changes for Rx AGC reload
05/21/15   dps    Added support for power logging using rfdevice_wtr_config_bitmask
05/15/15   ry     Device interface updates for RFC re-design for split Rx/Tx
12/12/14   dps    Added support for NV input to RxRSB calibration
11/13/14   zg     Added new API of get_vco_path
09/19/14   tks    Added support for db-dc spur mitigation
08/28/14   shb    Deprecated API to return critical_section_ptr
07/23/14   sma    Added commit to efs for writing RSB data once  
07/16/14   sma    Added Rx RSB temp comp per gain state support
12/17/13   rp     Added an interface to retrieve common device pointer associated with 
                  given tech trx class.
12/15/13   tks    Removed old tune to chan and tune to multi chan interface
11/11/13   tks    Added create_irat_script member function 
11/07/13   rp     Added and API to return common device class pointer.
08/12/13   hm     Initial version
==============================================================================*/

#include "comdef.h"
#include "rf_buffer_intf.h"

#ifdef FEATURE_WCDMA
#include "rfdevice_wcdma_type_defs.h"

#ifdef __cplusplus
#include "rfdevice_class.h"
#include "rfdevice_rxtx_common_class.h"

class rfdevice_trx_wcdma_rx:public rfdevice_class
{

public:
  rfdevice_trx_wcdma_rx();

  virtual ~rfdevice_trx_wcdma_rx();

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It places the RF Rx Device to its default state.

  @details
  It resets the chip and initializes Rx device registers for the idle state.
  This function must be called at power up and at anytime the
  device needs to be placed in its default state.

  @param path
  Logical RF Path to be initialized.
 
  @param mode
  Specifies the mode(technology) of operation.
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally 
 
  @retval

  @pre
  SBI clock regime must be enabled.

  @post
  RTR will be in its default state.
*/
  virtual boolean init(rfm_device_enum_type rfm_dev, 
                   rf_path_enum_type path,
                   rfcom_mode_enum_type mode,
                   rf_device_execution_type execution_type,
                   rf_buffer_intf* script_buffer) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It configures the RF Rx Device for the requested mode of operation.

  @details
  It configures the RF Rx device chain for the specified mode or technology.
 
  @param path
  Logical RF Path to be enabled
  
  @param mode
  Specifies the mode(technology) of operation
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally  

  @retval 

  @pre
  SBI clock regime must be enabled.

  @post
  RTR will be in its default state.
*/
  virtual boolean enter_mode(rfm_device_enum_type rfm_dev,
                             rf_path_enum_type path,
                             rfcom_mode_enum_type mode,
                             rf_device_execution_type execution_type,
                             rf_buffer_intf* script_buffer) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Programs the RTR for Rx mode of operation.

  @details
  Places the RTR into Rx mode of operation.

  @param path
  Logical RF Path to be enabled
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally  

  @retval 

  @pre
  RF device tune_to_chan() must be called.

  @post
  RF device will be placed in Rx mode of operation
*/
  virtual boolean enable(rfm_device_enum_type rfm_dev,
                       rf_path_enum_type path,
                       rfcom_wcdma_band_type rf_band,
                       rf_device_execution_type execution_type,
                       rf_buffer_intf* script_buffer) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Program the PLL based on the band and channel.

  @details
  Programs the RF synthesizer (PLL) as per the band and channel.

  @param path 
  RF Path of the device for which PLL to be programmed.
 
  @param band
  Specifies the RF band the PLL is to be loaded for
 
  @param chan[]
  Channel list
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally 
 
  @param wtr_config_bitmask
  Pointer to a bitmask which will be populated for logging WTR configuration

  @param use_alt_gain
  Parameter specifying whether to use default gain settings or
  the gain settings due to antenna switch
 
  @retval None.

  @pre
  RF Device init() must have been called prior to calling this function.

  @post
  RF synthesizer will be tuned for the requested band and channle.
*/
  virtual boolean set_channel ( rfm_device_enum_type rfm_dev, 
                                rfcom_mode_enum_type rf_mode, 
                                rfcom_wcdma_band_type rf_band,
                                rfdevice_wcdma_rx_tune_param_type rx_params, 
                                rf_device_execution_type exec_type,
                                rf_buffer_intf* script_buffer,
                                rf_wcdma_spur_cb_type rf_wcdma_program_wtr_notch_cb,
                                rfdevice_wcdma_spur_param_type* cb_data,
                                boolean enable_notch_support,
                                rfdevice_trx_wcdma_rx* paired_device,
                                rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
                                boolean use_alt_gain = FALSE ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function puts the RTR into sleep.

  @details
  RTR will be placed into sleep mode.

  @param path
  Logical RF Path to be put into sleep mode.
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally 

  @retval None

  @pre
  RF device init() must be called.

  @post
  Rf Device will be in sleep mode.
*/
  virtual boolean sleep(rfm_device_enum_type rfm_dev,
                      rf_path_enum_type path,
                      rf_device_execution_type execution_type,
                      rf_buffer_intf* script_buffer) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides generic get and set functionality for the Rx device specific
  parameters.

  @details
  It provides IOCTL kind of interface to get and set Rx device specific
  parameters.
 
  @param path
  Logical RF Path for which cmd to be performed
  @param cmd
  Specifies the command to be performed by the Rx device
  @param data
  Input and/or Output parameter for the command

  @retval ret_val
  generic return value which depends on the 'cmd' parameter.

  @pre
  It depends on the 'cmd' parameter. For some commands device specific init()
  function need to be called. For some there may not be any pre-condition prior
  to using this function.

  @post

*/
  virtual int32 cmd_dispatch(rfm_device_enum_type rfm_dev,
                             rfcom_wcdma_band_type rf_band,
                             rf_path_enum_type path,
                             rfdevice_cmd_enum_type cmd,
                             void *data) = 0;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configures WCDMA RX device to operate on the specified band/port 
   (rx_band_port) for the given band (rfcom_band)
   
   @details
   Ensures rfcom_band and rx_band_port are valid and updates instance data to
   store this band/port info for future use
   
   @param *rx_device
   RX device to be updated

   @param rfcom_band
   rfcom_band whose port is being set

   @param rx_band_port
   band/port enum will be typecast to wcdma_prx_band_type or
   wcdma_drx_band_type and will be used in all functions to apply
   autogenerated SSBI settings
*/
  virtual boolean set_port(rfm_device_enum_type rfm_dev,
                            rfcom_wcdma_band_type rfcom_band,
                            uint8 rx_band_port) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Rx band specific data to device
  
  @param *rx_device
  "this" pointer to RX device that needs to be configured
 
  @param band
  WCDMA band whose data is being pushed
 
  @param data_array
  Array of int32 containing device Rx data for band
 
  @param array_size
  Size of array of int32 data to be pushed to device
*/
  virtual boolean set_band_data(rfm_device_enum_type rfm_dev,
                                   rfcom_wcdma_band_type band,
                                   int32* data_array,
                                   uint8 array_size) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain Rx LNA gain state settings 
  
  @param *lna_state
  Strucuture containing info like band, carrier type etc, needed to
  obtain the specific LNA settings.
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
  virtual boolean get_lna_state(rfm_device_enum_type rfm_dev,
                                rfdevice_wcdma_lna_param_type *lna_state, 
                                rf_device_execution_type exec_type, 
                                rf_buffer_intf* script_buffer,
                                rfdevice_trx_wcdma_rx* paired_device ) = 0; 


/*----------------------------------------------------------------------------*/
/*!
  @brief
  WCDMA measurement script API for DIME using rf_buffer_intf

  @details
  This function prepares the sbi script for wcdma mode(single carrier or
  dual carrier), band & channel for measurement. This script is prepared
  one frame ahead of time as requested by WCDMA L1.
 
  This is the equivalent of wtr1605_wcdma_build_meas_script_v1 but returns the
  script in rf_buffer_intf* format
 
  @param param
  struct provides the necessary parameters like current band, target band,
  carrier type etc for building meas script

  @param buf
  Pointer to rf_buffer_intf object that will be populated with meas script
*/
  virtual boolean build_meas_script(rfm_device_enum_type rfm_dev,
                                           rfdevice_wcdma_meas_param_type param,
                                           rf_buffer_intf* buf,
                                           uint16 *current_size_autob) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare WCDMA measurement script for inter-rat and inter-freq scenarios
 
  @details
 
  @param rfm_dev
  current rfm device in use
 
  @param param
  structure which provides info reg measurement scenario, source & target
  techs, bands etc
 
  @param script_data_ptr
  Structure with input/output parameters needed for IRAT scripts.
  
  script_data_ptr->preload_action
  is an input param indicating whether preload_script should be executed
  internally by device, returned in buffer provided or skipped
  altogether (preload was already done)
  
  script_data_ptr->preload_buf
  Buffer ptr where preload script should be populated IF
  script_data_ptr->preload_action == RFDEVICE_MEAS_CREATE_SCRIPT.
  Preload script can be executed before the measurement gap without
  disrupting the source RAT.
  
  script_data_ptr->meas_action
  is an input param indicating whether trigger_script should be executed
  internally by device, returned in buffer provided or skipped
  altogether (meas script will be generated later)
  
  script_data_ptr->meas_buf
  Buffer ptr where meas script should be populated if
  script_data_ptr->meas_action == RFDEVICE_MEAS_CREATE_SCRIPT.
  Meas script should be executed within the IRAT measurment gap as its
  execution will immediately disrupt source RAT and tune for away measurement. 

  @param use_alt_gain
  Parameter specifying whether to use default gain settings or
  the gain settings due to antenna switch
 
  @return 
  success or failure flag 
*/

  virtual boolean create_irat_script( rfm_device_enum_type rfm_dev,
                                      rfcom_wcdma_band_type band,
                                      rfdevice_wcdma_meas_param_type param,
                                      rfdevice_meas_script_data_type* script_data_ptr,
                                      uint16 *current_size_autob,
                                      rfdevice_trx_wcdma_rx* paired_device,
                                      boolean use_alt_gain = FALSE ) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get script for CPC DRX 

  @details
  
  @param rx_device
  pointer to the wcdma device which needs to be programmed
 
  @param band
  enum indicating the current wcdma band of operation
 
  @param rx_on_off
  boolean value to incdicate rx on and rx off separately 
 
  @param exec_type
  enum to incidate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script 
 
  @return
  Success/failure status flag
*/
  virtual boolean get_cpc_drx_val(rfm_device_enum_type rfm_dev,
                                     rfcom_wcdma_band_type band,
                                     boolean rx_on_off, 
                                     rf_device_execution_type exec_type,
                                     rf_buffer_intf* script_buffer) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain Rx LNA offset setup 
  
  @param *offset
  Strucuture containing info like band, carrier type etc, needed to
  obtain the specific LNA settings.
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
  virtual boolean set_lna_offset(rfm_device_enum_type rfm_dev,
                                  rfdevice_wcdma_lna_phase_offset_type *offset, 
                                  rf_device_execution_type exec_type, 
                                  rf_buffer_intf* script_buffer) = 0; 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the RSB coefficients from the device 
  
  @param *coeff
  Strucuture containing info like band, carrier type etc, needed to
  obtain the specific rsb coefficients 
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script

  @param use_alt_gain
  Parameter specifying whether to use default gain settings or
  the gain settings due to antenna switch
 
  @return
  Success/failure status flag 
*/
  virtual boolean get_rsb_coeff(rfm_device_enum_type rfm_dev,
                                rfdevice_wcdma_rx_rsb_type *coeff, 
                                boolean *data_valid_status,
                                rf_device_execution_type exec_type, 
                                rf_buffer_intf* script_buffer,
                                rfdevice_wcdma_rx_rsb_cal_data_type* nv_rsb_data = NULL,
                                boolean use_alt_gain = FALSE
                                ) = 0; 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the RX PLL Lock Status from the device
  
  @param rfm_dev
  Logical RFM Device
 
  @param rf_band
  Current Band of RFM device
 
  @param *pll_lock_status
  Pointer to RX PLL Lock status
 
  @param *data_valid_status
  Pointer to status which says whether PLL is supported by this device
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
  virtual boolean pll_lock_detect(rfm_device_enum_type rfm_dev,
                                   rfcom_wcdma_band_type rf_band,
                                   boolean *pll_lock_status, 
                                   boolean *data_valid_status, 
                                   rf_device_execution_type exec_type, 
                                   rf_buffer_intf* script_buffer) = 0; 


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable Rx 
  
  @param *coeff
  Strucuture containing info like band, carrier type etc, needed to
  obtain the specific rsb coefficients 
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
  virtual boolean disable(rfm_device_enum_type rfm_dev,
                          rfcom_wcdma_band_type band, 
                          rf_device_execution_type exec_type, 
                          rf_buffer_intf* script_buffer,
                          rfdevice_trx_wcdma_rx* paired_device,
                          rfdevice_rx_mode_type rx_mode ) = 0; 

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *rx_device
  
  @param band
 
  @param *channel
 
  @param *data_valid_status
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
  virtual boolean get_stg_chan(rfm_device_enum_type rfm_dev, 
                                  rfcom_wcdma_band_type band, 
                                  uint16 *channel,
                                  boolean *data_valid_status, 
                                  rf_device_execution_type exec_type, 
                                  rf_buffer_intf* script_buffer) = 0; 

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *rx_device
  
  @param band
 
  @param *data
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
  virtual boolean config_stg(rfm_device_enum_type rfm_dev, 
                               rfcom_wcdma_band_type band, 
                               void *data,
                               rf_device_execution_type exec_type, 
                               rf_buffer_intf* script_buffer) = 0; 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface to just write WCDMA common table SSBI

  @details
  API to be used for IRAT to pre-configure device for measurement. The
  function will write all common table SSBI - both AutoA and AutoB.
 
  @param rfm_dev
  Logical device of operation
 
  @param rxdev
  "this" pointer to WCDMA RX device ptr
 
  @param rf_band
  Band for which SSBI is to be programmed. Use to determine port being used
  in conjunction with rfm_dev
 
  @param execution_type
  Indicates whether API should populate script_buffer or return a script
 
  @param script_buffer
  rf_buffer_intf object that will be populated with common settings if
  execution_type == RFDEVICE_CREATE_SCRIPT
 
  @return
  Success/failure status flag
*/
  virtual boolean set_common(rfm_device_enum_type rfm_dev,
                                 rfcom_wcdma_band_type rf_band,
                                 rf_device_execution_type execution_type,
                                 rf_buffer_intf* script_buffer) = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *rx_device
  
  @param band
 
  @param *channel
 
  @param *data_valid_status
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  NULL 
*/
  virtual boolean disable_stg(rfm_device_enum_type rfm_dev, 
                                rf_device_execution_type exec_type, 
                                rf_buffer_intf* script_buffer) = 0; 

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param band

  @param *rsb_data
  pointer to rsb data type

  @param use_alt_gain
  Parameter specifying whether to use default gain settings or
  the gain settings due to antenna switch
  
  @return
  Booelan flag indicating suuccess or failure 
*/

  virtual boolean rsb_temp_comp(rfm_device_enum_type rfm_dev, 
                                     rfcom_wcdma_band_type band,
                                     rfdevice_rx_wcdma_rsb_cal_data_type *rsb_data,
                                     boolean use_alt_gain = FALSE) = 0; 
/*----------------------------------------------------------------------------*/

  
/*!
  @brief commit_rsb_data_to_efs
   
   @param rfm_dev: RFM Device for which RSB params are desired
   
   @param rf_band: Band in use
   
   @param calibrated_list: list of calibrated devcie, band and gain state
 
   */


  virtual boolean commit_rsb_data_to_efs (
    rfm_device_enum_type rfm_dev,
    rfcom_wcdma_band_type rf_band, 
    rfdevice_rsb_cal_verfication_data_type* calibrated_list
    ) = 0;

/*----------------------------------------------------------------------------*/

/*!
  @brief
 
  @param rfm_dev
 
  @param band

  @param curr_band
  Number of carriers indicating single or dual carrier

  @param temperature
  thermistor reading

  @param *a_coeff 
  RSB coefficient A to be programmed in buffer

  @param *a_coeff 
  RSB coefficient A to be programmed in buffer

  @param use_alt_gain
  Parameter specifying whether to use default gain settings or
  the gain settings due to antenna switch
  
  @return
  Booelan flag indicating suuccess or failure 
*/
  virtual boolean rsb_coeff_temp_comp(rfm_device_enum_type rfm_dev,
                                      rfcom_wcdma_band_type band,
                                      rfcom_multi_carrier_hspa_id_type curr_bw,
                                      uint16 temperature,
                                      uint8 lna_gain_state ,
                                      int32* a_coeff,
                                      int32* b_coeff,
                                      rfdevice_wcdma_rx_rsb_cal_data_type* nv_rsb_data = NULL,
                                      boolean use_alt_gain = FALSE
                                      ) = 0; 

/*----------------------------------------------------------------------------*/
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
  virtual boolean enter_critical_section() = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable Critical Section

  @details
  This function will release lock which was acquired for the preceecing code.
 
  @return
  Success/failure status flag
*/
  virtual boolean leave_critical_section() = 0;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable Critical Section

  @details
  This function will release lock which was acquired for the preceecing code.
 
  @return
  Success/failure status flag
*/
  virtual uint32 get_instance_id();

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
  /*!
    @brief
    Query Rx VCO path data from TRX reciever

    @param data_array
    Array of int32 containing device Rx data for band

    @param array_size
    Size of array of int32 data to be pushed to device

    @return
    VCO path number 
  */
  virtual rfdevice_rx_vco_path_enum_type get_vco_path
  (
    int32* data_array,
    uint8 array_size
  )
  {
    RF_MSG( RF_ERROR, "rx_get_vco_path: Derived class does not support this API!" );
    return WTR_RX_INVALID_VCO_PATH;
  }

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean rx_pwr_sleep( void )
  {
    RF_MSG( RF_ERROR, "rx_pwr_sleep: Derived class does not support this API!" );
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual 
  boolean rx_pwr_wakeup( void )
  {
    RF_MSG( RF_ERROR, "rx_pwr_wakeup: Derived class does not support this API!" );
    return FALSE;
  }

protected:
  uint32 wtrInstanceId; //WTR instance ID

};

#else  /* for #ifdef __cplusplus */
struct rfdev_wcdma_rx;
typedef struct rfdev_wcdma_rx rfdevice_trx_wcdma_rx;
/*----------------------------------------------------------------------------*/
/*!
   @brief
   Get the lock variable used for critical section from C world
   
   @details
   This fucntion returns lock varibale for critical section from C world.
   This is used in IRAT scenarios to deterine whether both techs are on the same WTR.
   ******Need to remove this later and use get_instance_id. Doing this for now to not touch other Techs
*/
void* rfdevice_wcdma_rx_critical_section_var(rfdevice_trx_wcdma_rx* rx_device);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
*/
rfdevice_rxtx_common_class* 
rfdevice_wcdma_rx_get_common_device_pointer(rfdevice_trx_wcdma_rx* rx_device);

#endif /* #ifdef __cplusplus */

#endif //FEATURE_WCDMA

#endif /* RFDEVICE_TRX_WCDMA_RX_H */
