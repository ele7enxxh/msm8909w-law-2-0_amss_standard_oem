#ifndef RFDEVICE_RXTX_COMMON_CLASS
#define RFDEVICE_RXTX_COMMON_CLASS
/*!
  @file rfdevice_rxtx_common_class.h 

  @brief
  This is the rf device factory which creates all devices.



*/

/*===========================================================================

  Copyright (c) 2012 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_rxtx_common_class.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------- 
11/26/15   scs     Debug NV control to enable/disable WTR devices
11/16/15   sd      Added support to retrieve FBRX ADC Clock frequency
11/03/15   sd      Added support for debug dump of Autopin sample data
10/20/15   sd      Added API to get WTR capability for Autopin PAR captures
10/06/15   sd      Added support for AutoPin PAR
08/03/15   sma     Added ULCA 20+20 Tx LUT power offset 
02/24/15   aks     Device driver updates to enable ADC Vcm Cal 
02/19/15   hs      Added support for retention.
02/10/15   dps     Added support for Tx RSB calibration on WTR devices
11/24/14   aks     Updated get_tx_lut_scripts to accept bw union as an input
11/13/14   hs      Added API to set the JDET reading timer.
10/08/14   sd      Added support for Therm enable, therm read and hdet calculation
09/18/14   rsr     Added support to initialize manufacture/product and revision ID of transceiver.
09/09/14   lm      Added reg_dump() to print register debug info
08/17/14   tks     Added support for overriding fbrx gain state
08/15/14   sd      Added tech argument to FBRX Enable API
08/14/14   shb     Updated fbrx_setup/disable APIs to support slow_ramp ON and 
                   OFF sequence for LDO 26
08/05/14   sd      Added suppport for ILPC
07/25/14   sd      Added frequency arg to FBRX Enable API to handle band
                   dependent sequences
06/24/14   sma     Added LNA gain state for FBRX RSB  
06/16/14   sbm     HDET online brinup
06/12/14   hs      Added suppport for JDET.
05/29/14   shb     Added frequency arg to FBRX APIs to handle band dependent 
                   sequences
05/23/14   hm      Added API support to indicate PMIC 1V rail shutoff 
05/22/14   bm      Added API for light sleep
04/14/14   bm      Changes to decouple GPS and WWAN device ON/OFF state changes 
03/28/14   npi     Update communication protocol for device
03/10/14   hm      FBRX enable API enhancement to add delay after LDO enable, if configured
03/03/14   hs      Added two parameters to power_on()to allow the power on 
                   sequence to be logged in the script.
02/14/14   shb     Added API to retrieve FBRX RSB coefficients
02/05/14   hm      Added FBRX DC Cal enable and disable API. Renamed some API's
02/05/14   shb     Added read_therm API in WTR common class interface
01/16/14   hm      STG API implementation to enable and disable tone generation
12/09/13   rp      Added variable and methods to keep track of instance number of 
                   given rxtx device. Part of "==" operator overload change.
11/01/13   shb     Corrected variable name
10/11/13   shb     Added FBRX setup API. Updated FBRx enable/disable for 
                   immediate execution support
10/04/13   shb     Added APIs to return dummy tx trigger script, fbrx_enable and
                   fbrx_disable scripts
10/04/13   shb     Implemented get_tx_lut_scripts() for TXAGC
09/16/13   shb     Implemented virtual functions for common interface in 
                   rfdevice_rxtx_common_class()
08/22/13 shb/sr    Added get/set_tech_instance to support C++ tech interfaces
08/15/12   shb     Fixed compiler errors 
05/24/12   vss     Initial revision.


===========================================================================*/

#include "rfdevice_intf_cmd.h"
#include "rfdevice_class.h"
#include "rfdevice_cmn_type_defs.h"
#include "rfcommon_msg.h"

#ifdef __cplusplus

class rfdevice_rxtx_common_class:public rfdevice_class
{
  protected:
  rfdevice_cmn_type* common_device;
  uint32 device_asic_id;
  rfdevice_class *tech_instance[RFCOM_NUM_MODES][RFDEVICE_RXTX_MAX][RFDEVICE_TRX_MAX_PATHS];
  rfdevice_comm_proto_enum_type rf_device_comm_protocol; /*! Communication protocol in use */

  public:
  rfdevice_rxtx_common_class();

  rfdevice_rxtx_common_class(uint16 m_id, uint16 p_id, uint16 rev_id); 

  virtual rfdevice_cmn_type* get_instance();

  virtual uint32 get_device_asic_id();

  virtual rfdevice_comm_proto_enum_type get_comm_protocol();

  virtual rfdevice_class* get_tech_instance(rfcom_mode_enum_type mode, 
                                            rfdevice_rxtx_enum_type rxtx, 
                                            rfdevice_trx_phy_path_enum_type phy_path);
  virtual boolean set_tech_instance(rfcom_mode_enum_type mode, 
                                    rfdevice_rxtx_enum_type rxtx, 
                                    rfdevice_trx_phy_path_enum_type phy_path,
                                    rfdevice_class *rfdev);

  virtual ~rfdevice_rxtx_common_class();

  /* Below this line are APIs that need to be implemented by new devices implementing
  C++ interface directly. Adapter must implement the APIs for legacy devices using
  C style function pointer table */
  /*! Interface to set Iref of interest */
  virtual boolean set_dac_iref( rfdevice_dac_iref_val iref_val,
                                boolean program_iref,
                                uint32 *agc_gainword )
  {
    RF_MSG( RF_ERROR, "set_dac_iref not implemented in derived common class!");
    return FALSE;
  }
  
  /*! Interface to Enable or disable ET path */
  virtual boolean set_et_path( rfm_mode_enum_type mode,
                               rfm_fb_attenuation_state_type fb_state,
                               boolean enable_et_path )
  {
    RF_MSG( RF_ERROR, "set_et_path not implemented in derived common class!");
    return FALSE;
  }
  
  /*! Common Device Dispatch function */
  virtual boolean cmd_dispatch( rfdevice_cmd_enum_type cmd,
                                void* data )
  {
    RF_MSG( RF_ERROR, "cmd_dispatch not implemented in derived common class!");
    return FALSE;
  }
  
  /*! Interface to Enable/Disable TCXO buffer */
  virtual rfdev_cmn_status_type power_up( rf_device_execution_type execution_type,
                                          rf_buffer_intf* pwrup_setup_script,
                                          rfdev_cmn_vote_status_type rfdev_vote,
                                          rfdev_cmn_voting_type rfdev_voting_type )
  {
    RF_MSG( RF_ERROR, "power_up not implemented in derived common class!");
    return RFDEV_OFF;
  }
  
  /*! Interface to Enable/Disable TCXO buffer */
  virtual rfdev_cmn_status_type power_up_gps( rf_device_execution_type execution_type,
                                              rf_buffer_intf* pwrup_setup_script,
                                              rfdev_cmn_vote_status_type rfdev_vote,
                                              rfdev_cmn_voting_type rfdev_voting_type )
  {
    RF_MSG( RF_ERROR, "power_up_gps not implemented in derived common class!");
    return RFDEV_OFF;
  }

  /*! Interface to Enable/Disable TCXO buffer */
  virtual rfdev_cmn_status_type enable_wwan( rf_device_execution_type execution_type,
                                             rf_buffer_intf* pwrup_setup_script,
                                             rfdev_cmn_vote_status_type rfdev_vote,
                                             rfdev_cmn_voting_type rfdev_voting_type)
  {
    RF_MSG( RF_ERROR, "enable_wwan not implemented in derived common class!");
    return RFDEV_OFF;
  }
  /*! Interface to Enable/Disable TCXO buffer */
  virtual rfdev_cmn_status_type enable_gps( rf_device_execution_type execution_type,
                                            rf_buffer_intf* pwrup_setup_script,
                                            rfdev_cmn_vote_status_type rfdev_vote,
                                            rfdev_cmn_voting_type rfdev_voting_type )
  {
    RF_MSG( RF_ERROR, "enable_gps not implemented in derived common class!");
    return RFDEV_OFF;
  }
  /*! Interface to perform WAN or GPS reset/shut off */
  virtual boolean power_reset(rfdev_cmn_power_down_type rfdev_power_down_vote)
  {
    RF_MSG( RF_ERROR, "power_reset not implemented in derived common class!");
    return FALSE;
  }

  /*! Interface to perform light sleep or partial shutdown */
  virtual boolean light_sleep(rf_device_execution_type execution_type,
                              rf_buffer_intf* light_sleep_setup_script)
  {
    RF_MSG( RF_ERROR, "light_sleep not implemented in derived common class!");
    return FALSE;
  }

  /*! Backup any state/shadow inside device */
  virtual boolean backup_state( uint8* backup_token )
  {
    RF_MSG( RF_ERROR, "backup_state not implemented in derived common class!");
    return FALSE;
  }

  /*! Restores device state to a previously backed up version represented by the 
    backup_token */
  virtual boolean restore_state( uint8 backup_token )
  {
    RF_MSG( RF_ERROR, "restore_state not implemented in derived common class!");
    return FALSE;
  }

  /*! Triggers device to execute internal device calibration */
  virtual boolean do_internal_device_cal( rfdevice_cmn_int_dev_cal_data_type* cal_data )
  {
    RF_MSG( RF_ERROR, "do_internal_device_cal not implemented in derived common "
                      "class!");
    return FALSE;
  }

  /*! Returns legacy C style device pointer */
  virtual void* get_legacy_tech_device( rfm_mode_enum_type rfm_mode,
                                        rfdevice_chain_enum_type chain )
  {
    RF_MSG( RF_ERROR, "get_legacy_tech_device not implemented in derived "
                      "common class!");
    return NULL;
  }

  /*! Enter critical section lock for a device */
  virtual boolean enter_critical_section( void )
  {
    RF_MSG( RF_ERROR, "enter_critical_section not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Leave critical section lock for a device */
  virtual boolean leave_critical_section( void )
  {
    RF_MSG( RF_ERROR, "leave_critical_section not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Returns TX scripts and IREF per RGI for a given tech, band, pa_state */
  virtual boolean get_tx_lut_scripts( rfm_mode_enum_type tech,
                                      rfcom_band_type_u band_u,
                                      rfcom_bw_type_u bw_u,
                                      uint8 pa_state,
                                      uint8 rgi,
                                      rf_device_execution_type execution_type,
                                      rfdevice_tx_lut_data_type* lut_data,
                                      rfdevice_tx_power_offset_bw_type* pwr_offset_bw)
  {
    RF_MSG( RF_ERROR, "get_tx_lut_scripts not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Returns fbrx gain state script to set a desired gain state */
  virtual boolean override_fbrx_gain_state( rfm_mode_enum_type tech,
                                            rfcom_band_type_u band,
                                            uint8 gain_state, 
                                            uint8 tx_bw, 
                                            rf_device_execution_type execution_type,
                                            boolean fbrx_gain_state_override, 
                                            rf_buffer_intf* script_buf )
  {
    RF_MSG( RF_ERROR, "override_fbrx_gain_state not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! RF device register dump function*/
  virtual boolean reg_dump( rfdevice_reg_dump_cmd_type reg_dump_type )
  {
	RF_MSG( RF_ERROR, "reg_dump not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Returns dummy TX trigger script that can replace trigger script returned
  by get_tx_lut_scripts() as a nop write*/
  virtual boolean get_tx_dummy_trigger_script( rf_buffer_intf* dummy_script )
  {
    RF_MSG( RF_ERROR, "get_tx_dummy_trigger_script not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Fetches data to setup FBRX before enabling it */
  virtual boolean fbrx_setup( rf_device_execution_type execution_type,
                              rf_buffer_intf* fbrx_setup_script,
                              uint8* fbrx_setup_t_us,
                              rfdevice_fbrx_scenario_type fbrx_ldo_scenario )
  {
    RF_MSG( RF_ERROR, "get_fbrx_setup_script not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Fetches data to trigger FBRX setup per FBRx gain state */
  virtual boolean fbrx_enable( uint8 fbrx_gain_state,
                               rf_device_execution_type execution_type,
                               rf_buffer_intf* fbrx_enable_script,
                               boolean add_delay_before_enable_writes,
                               uint32 tx_freq_khz,
                               rfm_mode_enum_type rfm_mode )
  {
    RF_MSG( RF_ERROR, "get_fbrx_enable_script not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Fetches script to disable FBRX */
  virtual boolean fbrx_disable( rf_device_execution_type execution_type,
                                rf_buffer_intf* fbrx_disable_script,
                                rfdevice_fbrx_scenario_type fbrx_ldo_scenario )
  {
    RF_MSG( RF_ERROR, "get_fbrx_disable_script not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /* overload the operator '==' to use it to compare two rfdevice rxtx devices pointers*/
  virtual boolean operator==(rfdevice_rxtx_common_class cmn_rxtx_device)
  {
    return ((device_asic_id == cmn_rxtx_device.get_device_asic_id()) &&
            (rf_device_id == cmn_rxtx_device.rfdevice_id_get())&&
            (rf_device_type == cmn_rxtx_device.rfdevice_type_get()))?TRUE:FALSE;
  }

  /*! Fetches script to enable single tone generator */
  virtual boolean stg_enable( uint64 stg_freq, 
                              rf_device_execution_type exec_type,
                              rf_buffer_intf* script_buffer,
                              rfm_mode_enum_type mode,
                              rfcom_band_type_u band,
                              rfcom_bw_type_u bw,
                              uint8 fbrx_gain_state )
  {
    RF_MSG( RF_ERROR, "enable_stg not implemented in derived "
                      "common class!");
    return FALSE;
  }

  
  /*! Fetches script to disable single tone generator */
  virtual boolean stg_disable( rf_device_execution_type exec_type,
                               rf_buffer_intf* script_buffer,
                               uint64 stg_freq_hz,
                               rfm_mode_enum_type mode,
                               rfcom_band_type_u band,
                               rfcom_bw_type_u bw,
                               uint8 fbrx_gain_state )
  {
    RF_MSG( RF_ERROR, "disable_stg not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Reads the thermistor inside device */
  virtual boolean read_therm( uint16* therm_read )
  {
    RF_MSG( RF_ERROR, "read_therm not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Fetches script to enable fbrx dc cal */
  virtual boolean fbrx_dc_cal_enable( rf_device_execution_type exec_type,
                                      rf_buffer_intf* script_buffer,
                                      uint64 tx_lo_freq_hz,
                                      rfm_mode_enum_type mode,
                                      rfcom_band_type_u band,
                                      rfcom_bw_type_u bw,
                                      uint8 fbrx_gain_state )
  {
    RF_MSG( RF_ERROR, "fbrx_dc_cal_enable not implemented in derived "
                      "common class!");
    return FALSE;
  }

  
  /*! Fetches script to disable FBRX DC Cal */
  virtual boolean fbrx_dc_cal_disable( rf_device_execution_type exec_type,
                                       rf_buffer_intf* script_buffer,
                                       uint64 tx_lo_freq_hz,
                                       rfm_mode_enum_type mode,
                                       rfcom_band_type_u band,
                                       rfcom_bw_type_u bw,
                                       uint8 fbrx_gain_state )
  {
    RF_MSG( RF_ERROR, "fbrx_dc_cal_disable not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Returns FBRX RSB coefficients for a tech/band */
  virtual boolean fbrx_get_rsb_coeff( rfm_mode_enum_type tech,
                                      rfcom_band_type_u band,
                                      uint8 fbrx_gain_state,
                                      rfdevice_fbrx_rsb_data_type* rsb_coeff )
  {
    RF_MSG( RF_ERROR, "fbrx_get_rsb_coeff not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Returns whether 1v rail shutoff from PMIC is supported */
  virtual boolean get_1v_rail_shutoff_support(boolean *is_shutoff_supported)
  {
    RF_MSG( RF_ERROR, "get_1v_rail_shutoff_support not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Setup the Rx chains for Rx ADC Vcm Cal  */
  virtual boolean rx_adc_vcm_cal_setup( rfdevice_rx_vco_path_enum_type rx_vco )
  {
    RF_MSG(RF_ERROR, "rx_adc_vcm_cal_setup not implemented in derived class " )  ;
    return FALSE;
  }

  /*! Setup the Tx chains for FBRx ADC Vcm Cal  */
  virtual boolean fbrx_adc_vcm_cal_setup( )
  {
    RF_MSG(RF_ERROR, "fbrx_adc_vcm_cal_setup not implemented in derived class " )  ;
    return FALSE;
  }


  /*! Setup FBRX ADC */
  virtual boolean fbrx_adc_setup(rf_device_execution_type execution_type,
                                 rf_buffer_intf* hdet_trigger_script)
  {
    RF_MSG( RF_ERROR, "fbrx_adc_setup not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Trigger FBRX ADC */
  virtual boolean fbrx_adc_hdet_trigger (rf_device_execution_type execution_type,
                                         rf_buffer_intf* script_buf)
  {
     RF_MSG( RF_ERROR, "fbrx_adc_hdet_trigger not implemented in derived "
                       "common class!");
     return FALSE;
  }

  /*! Enable FBRX ADC */
  virtual boolean fbrx_adc_enable (rf_device_execution_type execution_type,
                                   rf_buffer_intf* hdet_trigger_script)
  {
    RF_MSG( RF_ERROR, "fbrx_adc_enable not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Returns the JDET read script for a praticular sample */
  virtual boolean rxsig_get_jdet_read_script( rfdevice_cmn_jdet_sample_type sample, 
                                              rf_buffer_intf* script_buf)
  {
  	RF_MSG( RF_ERROR, "rxsig_get_jdet_read_script not implemented in derived "
  										"common class!");
  	return FALSE;
  }

  /*! Turns on the JDET path */
  virtual boolean rxsig_jdet_enable( rf_device_execution_type exec_type,
                                     rf_buffer_intf* script_buf)
  {
  	RF_MSG( RF_ERROR, "rxsig_jdet_enable not implemented in derived "
  										"common class!");
  	return FALSE;
  }

  virtual boolean rxsig_jdet_enable_set_timer( uint16 holdoff_time_us,
                                               rf_device_execution_type exec_type,
                                               rf_buffer_intf* script_buf)
  {
  	RF_MSG( RF_ERROR, "rxsig_jdet_enable_set_timer not implemented in derived "
                      "common class!");
  	return FALSE;
  }

  /*! Turns off the JDET path */
  virtual boolean rxsig_jdet_disable( rf_device_execution_type exec_type,
                                      rf_buffer_intf* script_buf)
  {
  	RF_MSG( RF_ERROR, "rxsig_jdet_disable not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Disable FBRX ADC */
  virtual boolean fbrx_adc_disable(rf_device_execution_type execution_type,
                                   rf_buffer_intf* hdet_trigger_script)
  {
    RF_MSG( RF_ERROR, "fbrx_adc_enable not implemented in derived "
                      "common class!");
    return FALSE;
  }

  /*! Read FBRX ADC HDET */
  virtual void fbrx_adc_read_hdet(boolean* is_hdet_valid, 
                                  uint16* hdet_reading)
  {
    RF_MSG( RF_ERROR, "fbrx_adc_read_hdet not implemented in derived "
                      "common class!");
  }

  /*! Fetches script for ILPC Read */
  virtual boolean fbrx_ilpc_read( rf_device_execution_type execution_type,
                                  rf_buffer_intf* fbrx_ilpc_read_script )
  {
    RF_MSG( RF_HIGH, "fbrx_ilpc_read not implemented in derived "
                     "common class!");
    return FALSE;
  }

  /*! Retrieve ILPC accumulation information */
  virtual boolean fbrx_ilpc_get_accum_info( rfdevice_fbrx_ilpc_accum_data_type* fbrx_ilpc_accum_data )
  {
    RF_MSG( RF_HIGH, "fbrx_ilpc_get_accum_info not implemented in derived "
                     "common class!");
    return FALSE;
  }

  /*! Setup and enable therm */
  virtual boolean therm_enable( rf_device_execution_type execution_type,
                                rf_buffer_intf* script_buf )
  {
    RF_MSG( RF_HIGH, "therm_enable not implemented in derived "
                     "common class!");
    return FALSE;
  }

  /*! Setup and enable therm */
  virtual boolean therm_read_and_disable( uint16* therm_val )
  {
    RF_MSG( RF_HIGH, "therm_read_and_disable not implemented in derived "
                     "common class!");
    return FALSE;
  }

  /*! Setup and enable therm */
  virtual boolean calculate_hdet_reading( int32 sum_i,
                                          int32 sum_q,
                                          uint64 sum_i_q_sqr,
                                          uint32 fbrx_accum,
                                          uint8 fbrx_accum_overflow,
                                          uint16* hdet_reading )
  {
    RF_MSG( RF_HIGH, "calculate_hdet_reading not implemented in derived "
                     "common class!");
    return FALSE;
  }

  /*! Set the retention-enabled flag */
  virtual boolean set_retention_enabled( boolean enabled )
  {
    RF_MSG( RF_HIGH, "set_retention_enabled not implemented in derived "
                     "common class!");
    return FALSE;
  }

  /*! Get the retention-supported flag */
  virtual boolean get_retention_supported( boolean *supported )
  {
    RF_MSG( RF_HIGH, "get_retention_supported not implemented in derived "
                     "common class!");
    return FALSE;
  }

  /*! Interface to power on/off wtr to/from Low Power Mode */
  virtual boolean power_lpm( boolean to_enable )
  {
    RF_MSG( RF_ERROR, "power_lpm not implemented in derived common class!");
    return FALSE;
  }

  /*! Builds script to setup Autopin and trigger */
  virtual boolean fbrx_autopin_enable( rf_device_execution_type execution_type,
                                       rf_buffer_intf* fbrx_autopin_script,
                                       uint8 fbrx_gain_state,
                                       uint16 fbrx_capture_length,
                                       boolean debug_enable )
  {
    RF_MSG( RF_ERROR, "fbrx_autopin_enable not implemented in derived common class!");
    return FALSE;
  }

  /*! Read/compute Autopin parameters and ramp down LDO */
  virtual boolean fbrx_autopin_read_and_disable( rfdevice_fbrx_autopin_data_type* fbrx_autopin_data,
                                                 boolean debug_enable,
                                                 uint8 *autopin_debug_mem )
  {
    RF_MSG( RF_ERROR, "fbrx_autopin_read_and_disable not implemented in derived common class!");
    return FALSE;
  }

  /*! Get device capability for Autopin PAR captures */
  virtual boolean get_autopin_par_support(boolean *is_par_supported)
  {
    boolean status = TRUE;

    RF_MSG( RF_ERROR, "get_autopin_par_support not implemented in derived common class!");

    if (is_par_supported != NULL)
    {
      *is_par_supported = FALSE;
    }
    else
    {
      status = FALSE;
    }

    return status;
  }

  /*! Get FBRX ADC clock frequency in MHz */
  virtual boolean get_fbrx_adc_clock_freq
  (
    rfm_mode_enum_type tech,
    uint16 band,
    uint16 chan,
    double *fbrx_adc_clock_freq
  )
  {
    RF_MSG( RF_ERROR, "get_fbrx_adc_clock_freq not implemented in derived common class!");

    if (fbrx_adc_clock_freq != NULL)
    {
      *fbrx_adc_clock_freq = 0.0;
    }

    return FALSE;
  }

  /*! Interface to enable/disable WTR LPM state machine */
  virtual boolean enable_wtr_pwr_state_machine(boolean enable_wtr_lpm_sm)
  {
    RF_MSG( RF_ERROR, "enable_wtr_pwr_state_machine not implemented in derived common class!");
    return FALSE;
  }

};

#else /* #ifdef __cplusplus */

struct rfdev_trx_cmn;
typedef struct rfdev_trx_cmn rfdevice_rxtx_common_class;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper API to compare two rfdevice_rxtx_common_class objects.
  Can be used in .c files to compare two common rxtx class objects.

  @param cmn_rxtx_device1, cmn_rxtx_device2
  Pointer to common class devices to be compared
  
  @return 
  TRUE if both the devices are same else FALSE.
*/
boolean
rfdevice_compare_common_rxtx_devices
(
  rfdevice_rxtx_common_class* cmn_rxtx_device_1,
  rfdevice_rxtx_common_class* cmn_rxtx_device_2
);

#endif

#endif
