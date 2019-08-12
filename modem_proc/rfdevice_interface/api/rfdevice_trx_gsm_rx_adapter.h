#ifndef RFDEVICE_TRX_GSM_RX_ADAPTER_H
#define RFDEVICE_TRX_GSM_RX_ADAPTER_H
/*!
   @file
   rfdevice_trx_gsm_rx_adapter.h

   @brief

*/

/*===========================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_gsm_rx_adapter.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
09/16/15   sd    Added support for Rx AGC reload
05/21/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
05/12/15   sd    Device driver updates for RFC re-design for Split Rx/Tx
11/12/14   zg    Added API of get_vco_path
08/28/14   shb   Deprecated API to return critical_section_ptr
07/28/14   dps   Added support for LIF feature
07/25/14   hm    Added API to indicate whether Enhanced RX is supported
06/18/14   hm    GSM DRX support to return PRX or PRX+DRX script
02/24/14   dps   Added rx_init API for rx burst timing
12/20/13   dps   Removed unused APIs
12/10/13   rp    Added an API to get common device pointer associated with 
                 GSM tech class instance.
11/22/13   dps   Replaced build_meas_script and build_meas_script_v2 with an
                 updated version; Updated set_start_script API
11/12/13   dps   Added get_critical_section_ptr; removed device enum params; 
                 Renamed API from build_meas_script_type to build_meas_script
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
#include "rfdevice_trx_gsm_rx.h"

class rfdevice_trx_gsm_rx_adapter:public rfdevice_trx_gsm_rx
{

public:
  rfdevice_trx_gsm_rx_adapter(rfdevice_rxtx_common_class *cmn_rxtx_dev,
                               rfdevice_trx_phy_path_enum_type phy_path);

  virtual ~rfdevice_trx_gsm_rx_adapter();

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
    This function will set the GSM band

    @param
    band - Band to be set

    @param execution_type
    Indicates whether API should execute SSBI internally or populate
    script_buffer pointer with SSBI script to set rx band

    @param *script_buffer
    void Pointer to buffer_intf object where SSBI script needs to be appended.
    If NULL, script will have to be sent to hardware internally
  */
  boolean set_rx_band(rfcom_gsm_band_type band,
                      rf_device_execution_type execution_type,
                      rf_buffer_intf* script_buffer);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function tunes the rf device synthesizer to the specified channel for the
    sync Rx burst.

    @details
    Make sure rfgsm_set_band is called first.

    @param
    arfcn - Channel to be tuned
   
    @param
    enh_rx_on_flag - Indicates whether GSM enhanced Rx mode is active

    @param
    enh_rx_freq_offset_hz - Indicates the frequency offset to be applied for enhanced Rx burst
  */
  boolean tune_rx
  (
    uint16 arfcn,
    boolean enh_rx_on_flag, 
    int32 enh_rx_freq_offset_hz
  );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function configures the Rx gain for the RF Device.  Note, it also 
    configure the X-tia addresses since the addresses are RF gain dependent.

    @param
    rx_gain_range - RX gain range
  */
  boolean set_rx_gain_range( int32 rx_gain_range );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This is called during coarse tune cal or PLL lock test

    @details
    This will write the RF Device Rx script SBI registers directly instead of
    going through mdsp fw.
     
    @param band
    rx band we would like the SBI register to be set.
   
    @param arfcn
    Rx channel for which PLL registers need to be calculated.
   
    @param coarse_tune_cal_flag
    TRUE if the coarse tune cal is active.
  */
  boolean tune_rx_direct(rfcom_gsm_band_type band,
                         uint16 arfcn,
                         boolean coarse_tune_cal_flag);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Rx band specific data to device from RFC

    @param rfcom_band
    rfcom_band whose port is being set

    @param data_array
    Array of int32 containing device Rx data for band

    @param array_size
    Size of array of int32 data to be pushed to device

    @return Status flag indicating success or failure
  */
  boolean rx_set_band_data(rfcom_gsm_band_type rfcom_band,
                           int32* data_array,
                           uint8 array_size);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief

    @param buffer
  */
  boolean rx_set_stop_script( rf_buffer_intf *buffer,
                              rfdevice_trx_gsm_rx *paired_device,
                              rfdevice_rx_mode_type rx_mode );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function initializes the Rx for GSM operations on target WTR
    If source and target WTRs are different

    @param execution_type
    Indicates whether API should execute SSBI internally or populate
    script_buffer pointer with SSBI script to enter GSM mode

    @param *script_buffer
    Void Pointer to buffer_intf object where SSBI script needs to be appended.
    If NULL, script will have to be sent to hardware internally

    @retval TRUE
    device script to enter GSM mode created or executed successfully

    @retval FALSE
    GSM device enter mode failed

  */

  boolean rxonly_init(rf_device_execution_type execution_type,
                      rf_buffer_intf* script_buffer);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function disables Rx 

    @param execution_type
    Indicates whether API should execute SSBI internally or populate
    script_buffer pointer with SSBI script to enter GSM mode

    @param *script_buffer
    Void Pointer to buffer_intf object where SSBI script needs to be appended.
    If NULL, script will have to be sent to hardware internally

    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate if tech is using Rx chain for PRx/DRx operation

    @retval TRUE
    device script to disable rx is created or executed successfully

    @retval FALSE
    GSM device disable RX failed
  */

  boolean rx_disable(rf_device_execution_type execution_type,
                     rf_buffer_intf* script_buffer,
                     rfdevice_trx_gsm_rx *paired_device,
                     rfdevice_rx_mode_type rx_mode);


  /*----------------------------------------------------------------------------*/
  /*                          Combined Rx/Tx Functions                          */
  /*----------------------------------------------------------------------------*/


  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    This function initializes the RF Device for GSM operations.
    
    @param execution_type
    Indicates whether API should execute SSBI internally or populate
    script_buffer pointer with SSBI script to enter GSM mode
   
    @param *script_buffer
    Void Pointer to buffer_intf object where SSBI script needs to be appended.
    If NULL, script will have to be sent to hardware internally
   
    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate if tech is using Rx chain for PRx/DRx operation

    @retval TRUE
    device script to enter GSM mode created or executed successfully
   
    @retval FALSE
    GSM device enter mode failed
  */
  boolean enter_mode(rf_device_execution_type execution_type,
                     rf_buffer_intf* script_buffer,
                     rfdevice_trx_gsm_rx *paired_device,
                     rfdevice_rx_mode_type rx_mode);

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
  int32 cmd_dispatch(rf_path_enum_type path,
                     rfdevice_cmd_enum_type cmd,
                     void *data);

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
    The function generates SBI to switch mode to GSM. It is used for XtoG
    startup and GtoX cleanup

    @details
    The function generates SBI to switch mode to GSM. It is used for XtoG
    startup and GtoX cleanup

    @param gsm_meas_param
    Parameters needed to generate the script
   
    @param *script_data_ptr
    Structure with input/output parameters needed for IRAT scripts.

    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate if tech is using Rx chain for PRx/DRx operation

    @return
    Success/Failure flag
  */
  boolean build_meas_script
  (
    rfdevice_gsm_meas_param_type gsm_meas_param,
    rfdevice_meas_script_data_type* script_data_ptr,
    rfdevice_trx_gsm_rx *paired_device,
    rfdevice_rx_mode_type rx_mode
  );

  /*----------------------------------------------------------------------------*/
  boolean set_start_script(rfdevice_operation_mode oper_mode, 
                           rfgsmlib_sbi_burst_struct *script_infor, 
                           rf_buffer_intf *buffer,
                           uint16 *buffer_mask,
                           rfdevice_trx_gsm_rx *paired_device,
                           rfdevice_rx_mode_type rx_mode,
                           boolean inc_drx = FALSE, 
                           rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
                           rfdevice_wtr_config_bitmask_type *wtr_config_bitmask_drx = NULL,
                           boolean use_alt_gain = FALSE);

  /*----------------------------------------------------------------------------*/
  boolean rx_init(rfdevice_trx_gsm_rx *paired_device,
                  rfdevice_rx_mode_type rx_mode);

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    The function indicates whether enhanced rx is supported

    @details
    The function indicates whether enhanced rx is supported

    @param is_enh_rx_supported
    pointer to a boolean flag. TRUE if enhanced rx is supported, FALSE if not.
   
    @return
    Success/Failure flag
  */
  boolean get_enh_rx_support(boolean *is_enh_rx_supported);
	
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
    Query Rx VCO path data from TRX reciever

    @param data_array
    Array of int32 containing device Rx data for band

    @param array_size
    Size of array of int32 data to be pushed to device

    @return
    VCO path number 
  */
  rfdevice_rx_vco_path_enum_type get_vco_path
  (
    int32* data_array,
    uint8 array_size
  );

private:

  rfdevice_gsm_type *device;

  rfdevice_rxtx_common_class* common_rxtx_dev;

};

#endif

#endif /* RFDEVICE_TRX_GSM_RX_ADAPTER_H */
