#ifndef RFDEVICE_RXTX_COMMON_ADAPTER
#define RFDEVICE_RXTX_COMMON_ADAPTER
/*!
  @file rfdevice_rxtx_common_adapter.h 

  @brief
  Adapter common device that wraps around legacy C style common device
  interface


*/

/*===========================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_rxtx_common_adapter.h#1 $

when       who     what, where, why
--------------------------------------------------------------------------------
08/03/15   sma     Added ULCA 20+20 Tx LUT power offset 
02/10/15   dps     Added support for Tx RSB calibration on WTR devices
11/24/14   aks     Updated get_tx_lut_scripts to accept bw union as an input
08/15/14   sd      Added tech argument to FBRX Enable API
08/14/14   shb     Updated fbrx_setup/disable APIs to support slow_ramp ON and 
                   OFF sequence for LDO 26
07/25/14   sd      Added frequency arg to FBRX Enable API to handle band
                   dependent sequences
06/24/14   sma     Added LNA gain state for FBRX RSB   
05/29/14   shb     Added frequency arg to FBRX APIs to handle band dependent 
                   sequences
05/22/14   bm      Added API to support device light sleep
04/14/14   bm      Changes to decouple GPS and WWAN device ON/OFF state changes 
04/08/14   sma     Added support legacy device for bolt
03/28/14   npi     Update communication protocol for device
03/13/14 shb/fh    DIMEPM 20 bring up power up interface changes 
03/10/14   hm      FBRX enable API enhancement to add delay after LDO enable, if configured
02/25/14   aa      Interface to Enable/Disable TCXO buffer by SSBI retention
02/05/14   hm      FBRX API rename
12/110/13  rp      Changes to capture the asic/instance ID during device creation.
09/16/13   shb     Initial revision.


===========================================================================*/

#include "rfdevice_rxtx_common_class.h"

#ifdef __cplusplus

class rfdevice_rxtx_common_adapter : public rfdevice_rxtx_common_class
{

public:
  /*! Constructor for adapter class that saves pointer to legacy common 
    device */
  rfdevice_rxtx_common_adapter( rfdevice_rxtx_common_class* cmn_device, 
                                uint32 asic_id,
                                rfdevice_comm_proto_enum_type device_comm_protocol = RFDEVICE_COMM_PROTO_SSBI);
  
  /*! Interface to set Iref of interest */
  boolean set_dac_iref( rfdevice_dac_iref_val iref_val,
                        boolean program_iref,
                        uint32 *agc_gainword );
  
  
  /*! Interface to Enable or disable ET path */
  boolean set_et_path( rfm_mode_enum_type mode,
                       rfm_fb_attenuation_state_type fb_state,
                       boolean enable_et_path );
  
  /*! Common Device Dispatch function */
  boolean cmd_dispatch( rfdevice_cmd_enum_type cmd,
                        void* data );

  /*! Interface to Enable/Disable TCXO buffer */
  rfdev_cmn_status_type power_up
    ( 
       rf_device_execution_type execution_type,
       rf_buffer_intf* pwrup_setup_script,
       rfdev_cmn_vote_status_type rfdev_vote,
       rfdev_cmn_voting_type rfdev_voting_type
    );
  
  /*! Interface to Enable/Disable WTR GPS */
  rfdev_cmn_status_type power_up_gps
    ( 
       rf_device_execution_type execution_type,
       rf_buffer_intf* pwrup_setup_script,
       rfdev_cmn_vote_status_type rfdev_vote,
       rfdev_cmn_voting_type rfdev_voting_type
    );

  /*! Interface to Enable/Disable WWAN on WTR */
  rfdev_cmn_status_type enable_wwan
    ( 
       rf_device_execution_type execution_type,
       rf_buffer_intf* pwrup_setup_script,
       rfdev_cmn_vote_status_type rfdev_vote,
       rfdev_cmn_voting_type rfdev_voting_type
    );

  /*! Interface to Enable/Disable GPS on WTR */
  rfdev_cmn_status_type enable_gps
    ( 
       rf_device_execution_type execution_type,
       rf_buffer_intf* pwrup_setup_script,
       rfdev_cmn_vote_status_type rfdev_vote,
       rfdev_cmn_voting_type rfdev_voting_type
    );
  
  /*! Interface to Enable/Disable TCXO buffer by SSBI retention*/
  rfdev_cmn_status_type partial_power_up
    ( 
       rfdev_cmn_vote_status_type rfdev_vote,
       rfdev_cmn_voting_type rfdev_voting_type
    );

  /*! Interface to put RF device to light sleep */
  boolean light_sleep
  (
	rf_device_execution_type execution_type,
	rf_buffer_intf* light_sleep_setup_script
  );
  
  /*! Interface to perform WAN or GPS reset/shut off */
  boolean power_reset
    (
       rfdev_cmn_power_down_type rfdev_power_down_vote
    );

  /*! Backup any state/shadow inside device */
  boolean backup_state( uint8* backup_token );

  /*! Restores device state to a previously backed up version represented by the 
    backup_token */
  boolean restore_state( uint8 backup_token );

  /*! Triggers device to execute internal device calibration */
  boolean do_internal_device_cal( rfdevice_cmn_int_dev_cal_data_type* cal_data );

  /*! Returns legacy C style device pointer */
  void* get_legacy_tech_device( rfm_mode_enum_type rfm_mode,
                                rfdevice_chain_enum_type chain );

  /*! Enter critical section lock for a device */
  boolean enter_critical_section( void );

  /*! Leave critical section lock for a device */
  boolean leave_critical_section( void );

  /* Functions below this are supported only in new C++ based interface and not
  in legacy C function pointer table */

  /*! Returns TX scripts and IREF per RGI for a given tech, band, pa_state */
  boolean get_tx_lut_scripts( rfm_mode_enum_type tech,
                              rfcom_band_type_u band_u,
                              rfcom_bw_type_u bw_u,
                              uint8 pa_state,
                              uint8 rgi,
                              rf_device_execution_type execution_type,
                              rfdevice_tx_lut_data_type* lut_data,
                              rfdevice_tx_power_offset_bw_type* pwr_offset_bw  );
  
  /*! Returns dummy TX trigger script that can replace trigger script returned
  by get_tx_lut_scripts() as a nop write*/
  boolean get_tx_dummy_trigger_script( rf_buffer_intf* dummy_script );

  /*! Fetches data to setup FBRX before enabling it */
  boolean fbrx_setup( rf_device_execution_type execution_type,
                      rf_buffer_intf* fbrx_setup_script,
                      uint8* fbrx_setup_t_us,
                      rfdevice_fbrx_scenario_type fbrx_ldo_scenario );
  
  /*! Fetches data to trigger FBRX setup per FBRx gain state */
  boolean fbrx_enable( uint8 fbrx_gain_state,
                       rf_device_execution_type execution_type,
                       rf_buffer_intf* fbrx_enable_script,
                       boolean add_delay_before_enable_writes,
                       uint32 tx_freq_khz,
                       rfm_mode_enum_type rfm_mode );
  
  /*! Fetches script to disable FBRX */
  boolean fbrx_disable( rf_device_execution_type execution_type,
                        rf_buffer_intf* fbrx_disable_script,
                        rfdevice_fbrx_scenario_type fbrx_ldo_scenario );

   /*! Reads the thermistor inside device */
  boolean read_therm( uint16* therm_read );

  /*! Fetches script to enable single tone generator */
  boolean stg_enable( uint64 stg_freq, 
                      rf_device_execution_type exec_type,
                      rf_buffer_intf* script_buffer,
                      rfm_mode_enum_type mode,
                      rfcom_band_type_u band,
                      rfcom_bw_type_u bw,
                      uint8 fbrx_gain_state );

  /*! Fetches script to disable single tone generator */
  boolean stg_disable( rf_device_execution_type exec_type,
                       rf_buffer_intf* script_buffer,
                       uint64 stg_freq_hz,
                       rfm_mode_enum_type mode,
                       rfcom_band_type_u band,
                       rfcom_bw_type_u bw,
                       uint8 fbrx_gain_state );

  /*! Fetches script to enable fbrx dc cal */
  boolean fbrx_dc_cal_enable( rf_device_execution_type exec_type,
                              rf_buffer_intf* script_buffer,
                              uint64 tx_lo_freq_hz,
                              rfm_mode_enum_type mode,
                              rfcom_band_type_u band,
                              rfcom_bw_type_u bw,
                              uint8 fbrx_gain_state );

  /*! Fetches script to disable FBRX DC Cal */
  boolean fbrx_dc_cal_disable( rf_device_execution_type exec_type,
                               rf_buffer_intf* script_buffer,
                               uint64 tx_lo_freq_hz,
                               rfm_mode_enum_type mode,
                               rfcom_band_type_u band,
                               rfcom_bw_type_u bw,
                               uint8 fbrx_gain_state );

 /*! Returns FBRX RSB coefficients for a tech/band */
  boolean fbrx_get_rsb_coeff( rfm_mode_enum_type tech,
                              rfcom_band_type_u band,
                              uint8 fbrx_gain_state,
                              rfdevice_fbrx_rsb_data_type* rsb_coeff );
  

private:
  /*! Pointer to a legacy common device which does not implement 
    the virtual functions in rfdevice_rxtx_common_class */
  rfdevice_rxtx_common_class* legacy_common_device;
  
}; /* class rfdevice_rxtx_common_adapter::public rfdevice_rxtx_common_class */

#endif /* #ifdef __cplusplus */

#endif
