#ifndef RFDEVICE_TRX_WCDMA_TX_ADAPTER_H
#define RFDEVICE_TRX_WCDMA_TX_ADAPTER_H
/*!
   @file
   rfdevice_trx_wcdma_tx_adapter.h

   @brief

*/

/*===========================================================================

Copyright (c) 2012 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_wcdma_tx_adapter.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
05/21/15   dps    Added support for power logging using rfdevice_wtr_config_bitmask
10/13/14   bm     Added rf_band argument to cmd_dispatch function
12/15/13   tks    Removed tx tune chan and tx tune multi chan interface
12/04/13   tks    Added support for tx set channel api
11/07/13   rp     Added and API to return common device class pointer.
08/12/13   hm     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

#ifdef FEATURE_WCDMA
#include "rfdevice_wcdma_type_defs.h"
#include "rfdevice_trx_wcdma_tx.h"

class rfdevice_trx_wcdma_tx_adapter:public rfdevice_trx_wcdma_tx
{

public:
  rfdevice_trx_wcdma_tx_adapter(rfdevice_rxtx_common_class *cmn_rxtx_dev,
                                            rfdevice_trx_phy_path_enum_type phy_path);

  virtual ~rfdevice_trx_wcdma_tx_adapter();

  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	It places the RF Tx Device to its default state.
  
	@details
	It resets the chip and initializes Tx device registers for the idle state.
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
                   rf_buffer_intf* script_buffer);
  
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	It configures the RF Tx Device for the requested mode of operation.
  
	@details
	It configures the RF Tx device chain for the specified mode or technology.
   
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
                             rf_buffer_intf* script_buffer);
  
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Programs the RTR for Tx mode of operation.
  
	@details
	Places the RTR into Tx mode of operation.
   
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
	RF device will be placed in Tx mode of operation
  */
  virtual boolean enable(rfm_device_enum_type rfm_dev,
                       rf_path_enum_type path,
                       rfcom_wcdma_band_type rf_band,
                       rf_device_execution_type execution_type,
                       rf_buffer_intf* script_buffer);
  
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Programs the Tx PLL based on the band and channel.
  
	@details
	Programs the RF Tx synthesizer (PLL) as per the band and channel.
  
	@param path
	Logical RF Path for which PLL to be programmed.
   
	@param band   
	Specifies the RF band the PLL is to be loaded for
   
	@param chan   
	Specifies the RF uplink channel within the specified RF band
   
	@param dpd_state
	Indicates if we need to enable TX path for digital pre-distortion or without
	pre-distortion support. To support digital pre-distortion, RF device may need
	to choose different SSBI settings to widen bandwidth
  
	@param execution_type 
	Indicates whether API should execute SSBI internally or populate
	script_buffer pointer with SSBI script. 
   
	@param *script_buffer
	void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally 
   
  @param wtr_config_bitmask
  Pointer to a bitmask which will be populated for logging WTR configuration 
   
	@pre
	RF Device init() must have been called prior to calling this function.
  
	@post
	RF synthesizer will be tuned for the requested band and channel.
  */
  virtual boolean set_channel( rfcom_mode_enum_type rf_mode, 
                               rfcom_wcdma_band_type rf_band,
                               rfdevice_wcdma_tx_tune_param_type tx_params, 
                               rf_device_execution_type exec_type,
                               rf_buffer_intf* script_buffer,
                               rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL ); 
  
  virtual boolean tune_to_channel( rfdevice_wcdma_tx_tune_param_type tx_params, 
                                   rfdevice_meas_script_data_type* script_data_ptr,
                                   rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL ); 

  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Disables the Rx or Tx mode of operation.
  
	@details
	When the RTR is in the Rx Mode, the disable operation is not applicable to
	primary chain.
	When the RTR is in Tx mode, Tx will be disabled and if the RTR is only used
	for Tx then the RTR will be placed into sleep mode.
	When the RTR is RxTx mode, first Tx disbale should be called before disabling
	the Rx.
  
	@param path
	Logical RF Path to be disabled.
   
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
	Rf Device will be placed either Rx or Tx or Sleep mode of operation
	depending on the use of RTR.
  */
  virtual boolean disable(rfm_device_enum_type rfm_dev,  
                        rf_path_enum_type path,
                        rf_device_execution_type execution_type,
                        rf_buffer_intf* script_buffer);
  
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Provides generic get and set functionality for the Tx device specific
	parameters.
  
	@details
	It provides IOCTL kind of interface to get and set Tx device specific
	parameters.
  
	@param path
	Logical RF Path for which cmd to be performed
	@param cmd
	Specifies the command to be performed by the Tx device
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
                                rf_path_enum_type path,
                                rfcom_wcdma_band_type rf_band,                                
                                rfdevice_cmd_enum_type cmd,
                                void *data);
  
  /*----------------------------------------------------------------------------*/
  /*!
	 @brief
	 Configures WCDMA TX device to operate on the specified band/port 
	 (tx_band_port) for the given band (rfcom_band)
	 
	 @details
	 Ensures rfcom_band and tx_band_port are valid and updates instance data to
	 store this band/port info for future use
  
	 Device should be created before calling this function
	 
	 @param *tx_device
	 TX device to be updated
  
	 @param rfcom_band
	 rfm_band whose port is being set
  
	 @param tx_band_port
	 band/port enum will be typecast to wcdma_tx_port_data_type and will
	 be used in all functions to apply autogenerated SSBI settings
  */
  virtual boolean set_port(rfm_device_enum_type rfm_dev,
                            rfcom_wcdma_band_type rfcom_band,
                            uint8 tx_band_port,
                            uint32 tx_band_pa_lut_map);
  
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Push Tx band specific data to device
	
	@param *rx_device
	"this" pointer to TX device that needs to be configured
   
	@param band
	WCDMA band whose data is being pushed
   
	@param data_array
	Array of int32 containing device Tx data for band
   
	@param array_size
	Size of array of int32 data to be pushed to device
  */
  virtual boolean set_band_data(rfm_device_enum_type rfm_dev,
                                   rfcom_wcdma_band_type band,
                                   int32* data_array,
                                   uint8 array_size,
                                   uint32 tx_band_pa_lut_map);
  
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Read the current therm value from device 
  
	@details
	 
	@param tx_device
	Pointer to the wcdma device which needs to be programmed
   
	@param therm_value
	uint16 value to obtain the current therm value
   
	@param exec_type
	enum to indicate direct hw writes or prepare script
   
	@param script_buffer
	populate the buffer with script 
   
	@return
	Success/failure status flag
  */
  virtual boolean read_therm(rfm_device_enum_type rfm_dev,
                                uint16 *therm_value, 
                                rf_device_execution_type exec_type,
                                rf_buffer_intf* script_buffer);
   
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Configure RX_ON signal by SSBI
  
	@details
	Configure RX_ON signal by SSBI
   
	@param band
	band is set for the mode
   
	@param execution_type 
	Indicates whether API should execute SSBI internally or populate
	script_buffer pointer with SSBI script. 
   
	@param *script_buffer
	void Pointer to buffer_intf object where SSBI script needs to be appended.
	If NULL, script will have to be sent to hardware internally  
  
	@retval 
  
	@post
	Configure RX_ON signal by SSBI
  */
  
  virtual boolean set_rf_on(rfm_device_enum_type rfm_dev,
                          rfcom_wcdma_band_type rf_band,
                          boolean rf_on,
                          rf_device_execution_type execution_type,
                          rf_buffer_intf* script_buffer);
  
  
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Get script for CPC DTX 
  
	@details 
	
	@param tx_device
	pointer to the wcdma device which needs to be programmed
   
	@param band
	enum indicating the current wcdma band of operation
   
	@param tx_on_off
	boolean value to incdicate tx on and tx off separately 
   
	@param exec_type
	enum to incidate direct hw writes or prepare script
   
	@param script_buffer
	populate the buffer with script 
   
	@return
	Success/failure status flag
  */
  virtual boolean get_cpc_dtx_val(rfm_device_enum_type rfm_dev,
                                     rfcom_wcdma_band_type band,
                                     boolean tx_on_off, 
                                     rf_device_execution_type exec_type,
                                     rf_buffer_intf* script_buffer);
  
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Programs the Tx PLL based on the band and channel.
  
	@details
	Programs the RF Tx synthesizer (PLL) as per the band and channel.
  
	@param path
	Logical RF Path for which PLL to be programmed.
   
	@param band   
	Specifies the RF band the PLL is to be loaded for
   
	@param chan   
	Specifies the RF uplink channel within the specified RF band
   
	@param dpd_state
	Indicates if we need to enable TX path for digital pre-distortion or without
	pre-distortion support. To support digital pre-distortion, RF device may need
	to choose different SSBI settings to widen bandwidth
  
	@param execution_type 
	Indicates whether API should execute SSBI internally or populate
	script_buffer pointer with SSBI script. 
   
	@param *script_buffer
	void Pointer to buffer_intf object where SSBI script needs to be appended.
	If NULL, script will have to be sent to hardware internally
	@pre
	RF Device init() must have been called prior to calling this function.
  
	@post
	RF synthesizer will be tuned for the requested band and channel.
  */
  virtual boolean tune_to_default_port(rfm_device_enum_type rfm_dev,
                                       rfcom_mode_enum_type mode,
                                       rfcom_wcdma_band_type rf_band,
                                       uint16 chan,
                                       rfdevice_dpd_enum_type dpd_state,
                                       rf_device_execution_type execution_type,
                                       rf_buffer_intf* script_buffer);
  
  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Obtain the TX PLL Lock Status from the device 
	
	@param rfm_dev
	Logical RFM Device
   
	@param *pll_lock_status
	Pointer to TX PLL Lock status
   
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
                                   boolean *pll_lock_status, 
                                   boolean *data_valid_status, 
                                   rf_device_execution_type exec_type, 
                                   rf_buffer_intf* script_buffer); 

  /*----------------------------------------------------------------------------*/
  /*!
	@brief
	Returns the ET delay compensation to be applied for WCDMA
  
	@details
	Delay compensation value depends on TX BBF RC error, TX process
   
	@param tx_device
	"this" pointer to WCDMA Tx device of operation.
   
	@param delay_comp_x10ns
	Delay compensation to be applied in 1/10th of a ns unit
   
	@retval TRUE
	Indicates the API was successful and a correct delay comp value has been
	returned
   
	@retval FALSE
	Indicates API failed due to bad parameters
  */
  virtual boolean get_et_delay_comp(int16* et_delay_comp_x10ns);

  /*----------------------------------------------------------------------------*/
  /*!
	@brief
   
	@param rfm_dev
   
	@param band
  
	@param *rsb_data
	pointer to rsb data type
	
	@return
	Booelan flag indicating suuccess or failure 
  */
  virtual boolean calc_fbrx_rsb_nv_params
  (
    rfm_device_enum_type rfm_dev,
    rfcom_wcdma_band_type rf_band,
    rfdevice_wcdma_bw_type tx_bw,
    rfdevice_rsb_cal_data_type *rsb_data
  );


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
  virtual boolean enter_critical_section();

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable Critical Section

  @details
  This function will release lock which was acquired for the preceecing code.
 
  @return
  Success/failure status flag
*/
  virtual boolean leave_critical_section();

/*----------------------------------------------------------------------------*/
/*!
	 @brief
	 Required implementation of the common init() function required by all 
	 classes derived from rfdevice_class.
		 
	 @details
	 No operations are needed to initialize this adapter class.
*/
  virtual void init();

/*----------------------------------------------------------------------------*/
  rfdevice_rxtx_common_class* get_common_device_pointer(void);


private:

  rfwcdma_device_tx_type *tx_device;

  rfdevice_rxtx_common_class* common_rxtx_dev;
 
};

#endif //FEATURE_WCDMA

#endif /* RFDEVICE_TRX_WCDMA_TX_ADAPTER_H */
