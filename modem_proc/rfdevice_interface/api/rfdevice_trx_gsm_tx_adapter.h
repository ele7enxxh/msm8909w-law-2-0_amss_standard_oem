#ifndef RFDEVICE_TRX_GSM_TX_ADAPTER_H
#define RFDEVICE_TRX_GSM_TX_ADAPTER_H
/*!
   @file
   rfdevice_trx_gsm_tx_adapter.h

   @brief

*/

/*===========================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_gsm_tx_adapter.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
05/21/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
08/28/14   hm    Added/Modified APi's for TX DSDA support
04/10/14   dps   Changed return type for get_num_therm_regs_to_read API to allow for a
                 negative return value for failure cases
03/11/14   dps   Added API to get number of therm registers to read;
                 removed prog_cal_tx_burst_sbi API
12/20/13   dps   Added set_rfc_gsm_core_ptr and enter_mode APIs
12/10/13   rp    Added an API to get common device pointer associated with 
                 GSM tech class instance.
11/12/13   dps   Added get_critical_section_ptr; removed device enum params
09/20/13   dps   Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"
#ifdef FEATURE_GSM
#include "rfdevice_gsm_type_defs.h"
#include "rfdevice_trx_gsm_tx.h"

class rfdevice_trx_gsm_tx_adapter:public rfdevice_trx_gsm_tx
{

public:
  rfdevice_trx_gsm_tx_adapter(rfdevice_rxtx_common_class *cmn_rxtx_dev,
                              rfdevice_trx_phy_path_enum_type chain);

  virtual ~rfdevice_trx_gsm_tx_adapter();

  /*----------------------------------------------------------------------------*/
  /*! 
    @brief
    Enters critical section lock for the device
  */
  boolean enter_critical_section( void );

  /*----------------------------------------------------------------------------*/
  /*! 
    @brief
    Leaves critical section lock for the device
  */
  boolean leave_critical_section( void );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function initializes the RF Device for GSM operations.
    
    @param * script_data_ptr
    Pointer to rfdevice_meas_script_data_type
   
    @retval TRUE
    device script to enter GSM mode created or executed successfully
   
    @retval FALSE
    GSM device enter mode failed
  */
  boolean enter_mode(rfdevice_meas_script_data_type* script_data_ptr);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function reads the thermistor value from the RF Device HKADC.

    @details
    This functions read thermistor value inside of HKADC. 

    @return
    temperature uint16 12 bits resolution therm_value.
    0xFFFF indicates invalid result
    low result value indicates high temperature
  */
  uint16 therm_read( void );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function setup the RF device to handle band dependent Tx setting

    @details
    It should be called way before the actual RX/TX burst.

    @param band
    Band to be set

    @param * script_data_ptr
    Pointer to rfdevice_meas_script_data_type
  */
  boolean set_tx_band(rfcom_gsm_band_type band,
                      rfdevice_meas_script_data_type* script_data_ptr);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function tunes the rf device synthesizer to the specified channel for the
    sync Tx burst.

    @param arfcn
    channel to be tuned
  */
  boolean tune_tx(uint16 arfcn);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function sets rgi. Note that rgi and power are related.
    i.e. As rgi increases power increases.

    @param rgi
    RF Gain Index

    @param mod_type
    Modulation type

    @param slot_num
    Number of slots

    @param set_script
    Flag to add settings to the script, or write direct SSBI

  */
  boolean set_rgi(uint16 rgi,
                  rfgsm_modulation_type mod_type,
                  uint8 slot_num,
                  boolean set_script);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function will enable/disable kv adaptation.

    @param flag
    Flag to enable/disable kv adaptation
  */
  boolean enable_kv_cal(boolean flag);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function executes the KV calibration routine.

    @param kv_value

    @param num_iter
  */
  boolean execute_kv_cal(rfgsmlib_kv_code_type *kv_value,
                         uint8 num_iter);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function set the kv calibration code based on the value passed in.

    @param code
  */
  boolean set_kv_cal_code(uint16 code);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function computes final KV code

    @details
    This function computes the final KV code based on the average KV and 
    acquisition code.  The desired code is 0x88 (value for Reg4D).  The KV 
    and acquisition codes are inversely proportion.  The equation to compute
    the final KV code is: final_kv = (acq_code - 0x88) + kv_code

    @param kv
    Averaged KV code

    @param r1
    Averaged Fine Tune Code
  */
  int16 compute_final_kv_cal(int16 kv,
                             uint16 r1);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function returns the start time for first SBI transaction sent during a TX burst.

    @details
    SBI clock regime must be enabled.

    @retval
    Start time for first SBI transcation sent during a TX burst
  */
  int16 get_tx_sbi_start_delta( void );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function turns off Phase modulation in DPLL. It is used
    only for Linear AMAM cal. 

    @details
    This function will will turn off phase modulation in DPLL. This is required
    Linear AMAM cal.

    @param on_off
    FALSE will turn phase modulation OFF, TRUE will turn phase modulation ON

    @param rf_buffer_intf*
    pointer to script buffer
  */
  boolean set_tx_phase_const(boolean on_off,
                             rf_buffer_intf *buffer);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Tx band specific data to device from RFC

    @param rfcom_band
    rfcom_band whose port is being set

    @param data_array
    Array of int32 containing device Tx data for band

    @param array_size
    Size of array of int32 data to be pushed to device

    @return Status flag indicating success or failure
  */
  boolean tx_set_band_data(rfcom_gsm_band_type rfcom_band,
                           int32* data_array,
                           uint8 array_size);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @param buffer
  */
  boolean tx_set_trans_script(rf_buffer_intf *buffer);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @param buffer
  */
  boolean tx_set_stop_script(rf_buffer_intf *buffer);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @param arfcn
   
    @param extcal
   
    @param buffer
  */
  boolean prog_cal_tx_start_script(uint16 arfcn,
                                   int8 extcal, 
                                   rf_buffer_intf *buffer);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @param buffer
  */
  uint16 get_therm_read(rf_buffer_intf *buffer);


  /*----------------------------------------------------------------------------*/
  /*                          Combined Rx/Tx Functions                          */
  /*----------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Provides generic get and set functionality for the device specific
    parameters.

    @details
    It provides IOCTL kind of interface to get and set device specific
    parameters.

    @param path
    RF Path of the device for which cmd to be performed.
   
    @param cmd
    Specifies the command to performed by the device.
   
    @param data
    Input and/or Output parameter for the command.

    @retval
    generic return value which depends on the 'cmd' parameter.

    @pre
    It depends on the 'cmd' parameter. For some commands device specific init()
    function need to be called. For some there may not be any pre-condition prior
    to using this function.
  */
  int32 cmd_dispatch( rf_path_enum_type path,
                      rfdevice_cmd_enum_type cmd,
                      void *data);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Configures rf gsm device to operate on the specified band/port 
    (rx_band_port) for the given band (rfcom_band)

    @details
    Ensures rfm_band and tx_band_port are valid and updates instance data to
    store this band/port info for future use

    @param rfcom_band
    rfcom_band whose port is being set

    @param rx_band_port
    rx band/port enum for the rfdevice.

    @param tx_band_port
    tx band/port enum for the rfdevice
  */
  boolean set_band_port(rfcom_gsm_band_type rfcom_band,
                        uint8 rx_band_port,
                        uint8 tx_band_port);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    The function generates start/tune script for TX

    @details
    The function generates start/tune script for TX

    @param oper_mode
    Parameter to indicate Stand alone or IRAT mode.
    used to determine WXE cell
   
    @param *script_infor
    Parameter to indicate type of burst and to retrieve timing information

    @param * buffer
    Pointer to script buffer
   
    @param wtr_config_bitmask
    Pointer to a bitmask which will be populated for logging WTR configuration

    @return
    Success/Failure flag
  */
  boolean set_start_script(rfdevice_operation_mode oper_mode, 
                           rfgsmlib_sbi_burst_struct *script_infor, 
                           rf_buffer_intf *buffer,
                           uint16 *buffer_mask,
                           rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    The function generates start/tune script for TX

    @details
    The function generates start/tune script for TX

    @param oper_mode
    Parameter to indicate Stand alone or IRAT mode.
    used to determine WXE cell
   
    @param *script_infor
    Parameter to indicate type of burst and to retrieve timing information

    @param * script_data_ptr
    Pointer to rfdevice_meas_script_data_type

    @return
    Success/Failure flag
  */
  boolean set_start_script(rfdevice_operation_mode oper_mode, 
                           rfgsmlib_sbi_burst_struct *script_infor, 
                           rfdevice_meas_script_data_type* script_data_ptr);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    The function sets the rfc_gsm_core_ptr to point to the right data
    structure, containing, mon/burst timing info, etc.

    @details
    The function sets the rfc_gsm_core_ptr to point to the right data
    structure, containing, mon/burst timing info, etc.
    
    @param *rfc_gsm_core_ptr
    ptr to the rfc data structure which has the timing info.
  */
  boolean set_rfc_gsm_core_ptr(rfc_gsm_core_config_type *rfc_gsm_core_ptr);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get rfdevice_rxtx_common_class pointer associated with this instance of device.
    
    @return 
    Pointer to rxtx common device associated with current instance of device.
  */
  rfdevice_rxtx_common_class* get_common_device_pointer();

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Get the number of therm registers to read on this device.
  */
  int16 get_num_therm_regs_to_read();

private:

  rfdevice_gsm_type *device;

  rfdevice_rxtx_common_class* common_rxtx_dev;

};
#endif

#endif /* RFDEVICE_TRX_GSM_TX_ADAPTER_H */
