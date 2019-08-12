#ifndef RFGNSS_CORE_H
#define RFGNSS_CORE_H

/*
   @file
   rfgnss_core.h

   @brief
   This file contains RF GNSS core definitions, enumerations and prototypes.

   @details
*/

/*===========================================================================
Copyright (c) 1999 - 2002, 2006, 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008, 2009, 2010, 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/gen8/rf/core/src/rfgnss_core.h#1 $
$DateTime: 2016/12/13 07:59:49 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/19/14   rsk     GNSS memory clean-up, remove some header includes and
                   add few required structures locally.
06/21/13   jn      Factory calibrated PGA option for WWAN co-exist.
05/08/13   dbz     Implemented Beidou BP2 gain step calibration
03/28/13   gy      Add TDSCDMA band, channel status
03/14/13   sar     Fixed compiler errors in APQ target. 
02/26/13   dbz     Mainline IMD feature for DIME PLs 
02/11/13   dbz/gy  Added API to disable/enable gnss pga gain backoff
02/11/13   gy      Added gnss rfsw repartition
07/17/12   jr      Added support for non-ELNA configuration. 
02/24/12   dbz     Implemented new ADC power up/down sequence
01/31/12   jn      Added support for run-time selection of notch filter settings.
12/08/11   dbz     Add GNSS IMD feature
10/03/11   gy      Supported GPS vote for A0 in normal demod quality
10/03/11   dbz     Added timer based hysteresis loop for notch filter programming
08/25/11   dbz     Add PGA tracking loop control
08/05/11   dbz     Clean up obsolete RFC referecnes
04/23/11   gy      Ported for nikel
11/11/10   av      Save the last good known PGA value
10/05/10   av      Merge changes from Genesis
09/10/10   hp      Update to LTE GPS B13 Gain Adjust Algorithm  
08/26/10   hp      Added API's for GPS Notch for LTE and EVDO 
07/13/10   dbz     Added suport for GNSS Demod quality control
06/30/10   hp      Added Support for LTE GPS B13 Gain Adj for SVLTE-Fusion
06/02/10   dbz     Merged Mode 5 SRS support from Dev brach
05/27/10   dbz     Added support for FE cal for cucurrent ARS/SRS config
05/25/10   dbz     Added support for managing PMIC XO resonator
04/09/10   dbz     Added support for WB GNSS sync path support
                   for Callisto 1.x 
01/06/10   dbz     Added rfm_gnss_debug support
12/17/09   dbz     Changed rfgnss_notch_cfg_type 
10/21/09   dbz     Enabled RF status report 
10/16/09   hp      Fixed Compiler Warnings
10/16/09   dbz     Added support for GNSS spur mitigation algorithms
10/15/09   hp      Added function prototypes needed to support ELNA API & added rfgnss_core_card_init() & rfgnss_core_card_adc()
09/30/09   dbz     Added RF status report support 
09/28/09   gy      Code clean up to resolve compilation warnings
09/13/09   gy      Added pause/resume for DPO support
09/10/09   gy      Added GPIO initialization based on card file
08/31/09   gy      redefined set_ext_lna_on function
08/30/09   dbz     Added API to setup baseband for bootup FE cal
07/29/09   dbz     Added RTR8600 DC/gain/FE/RC/R cal algorithms
04/15/09   dbz     Refactor core functions
03/10/09   gy      Code clean up
12/20/08   gy      Initial version to port rf gnss core.

============================================================================*/

#include "rfa_variation.h"
#include "rfcom.h"
#include "rfgnss_mc.h"
#include "rfc_pm.h"
#include "rfdevice_type_defs.h"
#include "customer.h"
// #include "rfgnss_msm.h"
#include "gnss_api.h"
#include "timer.h"

/*----------------------------------------------------------------------------*/
/* taken from rfgnss_msm.h */

typedef struct
{
  double i;
  double q;
}rfgnss_adc_mean_type;

typedef struct
{
  double i;
  double q;
}rfgnss_adc_ampl_type;

typedef struct 
{
  gnss_BpIfDownConvType gnss_bp1_if;
  gnss_BpIfDownConvType gnss_bp3_if;
  gnss_BpEqCoefType gnss_bp1_eq;
  gnss_BpEqCoefType gnss_bp3_eq;

} rfgnss_msm_rfasic_type; 

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function converts device number to path number.

   @param

   @retval

*/
rf_path_enum_type rfgnss_core_device_to_path( rfcom_device_enum_type device );

typedef struct {
  timer_group_type rfgnss_wwan_tx_timer_group;
  timer_type rfgnss_wwan_tx_timer;
  boolean wwan_tx_timer_on;
} rfgnss_wwan_tx_timer_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Do RF GPS core baseband initialization.

   @details

   @param None

   @retval None

*/
#ifndef T_MDM9615
void rfgnss_core_baseband_init( void );
#endif


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function gets the current rf library.

   @param None

   @retval rf_library_type

*/
rfgnss_rf_library_type rfgps_get_rf_library(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Do RF GPS core initialization.

   @details

   @param None

   @retval None

*/
void rfgnss_core_init( rfgnss_mode_cfg_enum_type rfgnss_mode );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configure vregs, gpio and device function pointers. Initialize the device.

   @details

   @param path :

   @retval None

*/
void rfgnss_core_config( rfgnss_path_enum_type rfgnss_path, rfgnss_mode_cfg_enum_type rfgnss_mode );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configure GNSS RF devices.

   @details

   @param path :

   @retval None

*/
void rfgnss_core_config_devices( rfgnss_path_enum_type rfgnss_path);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Do RF GPS DC cancellation.

   @details

   @param device: rfcom device enum

   @retval None

*/
void rfgnss_core_dc_cancellation( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Adjusts GPS Rx front end gain.

   @details

   @param device: rfcom device enum

   @retval None

*/
void rfgnss_core_gain_adjust( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Wakeup bbrx for GPS.

   @details

   @param None

   @retval None

*/
void rfgnss_core_adc_wakeup(gnss_adc_enum_type gnss_adc, rfgnss_adc_op_type wakeup);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Set bbrx mode for GPS.

   @details

   @param None

   @retval None

*/
boolean rfgnss_core_adc_set_mode(gnss_adc_enum_type gnss_adc, rfgnss_mode_cfg_enum_type rfgnss_mode );

/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function configures GNSS Card.

   @details
   This function is called from rfgnss_mc_configure() and sets the RF Card appropriately.  

   @param device : RF Multimode device
   @param gnss_mode : GNSS Mode
*/
void rfgnss_core_rf_card_init ( rfcom_device_enum_type device, rfgnss_mode_cfg_enum_type rfgnss_mode );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function configures GNSS ADC.

   @details
   This function is called from rfgnss_mc_configure() and sets the GNSS ADC appropriately.  

   @param device : RF Multimode device
   @param gnss_mode : GNSS Mode
*/
void rfgnss_core_adc_init ( rfcom_device_enum_type device, rfgnss_mode_cfg_enum_type rfgnss_mode );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function configures GNSS baseband processor.

  @details

*/
void rfgnss_core_bbc_init(rfgnss_mode_cfg_enum_type rfgnss_mode);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function puts GNSS device into sleep mode

  @details
  
*/
void rfgnss_core_sleep(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function reads current GNSS state.

   @details

   @param None

   @retval ON: GPS is active or OFF: GPS is inactive
*/
boolean rfgnss_core_get_state(void);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function performs the R/RC tune cal for the RF device

   @details

   @param None

   @retval None
*/
void rfgnss_core_do_r_rc_tune_cal( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function reads the GPS receiver chip ID. 

   @details

   @param None

   @retval 
*/
extern uint8 rfgnss_core_get_chip_id( void );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function returns the current network band. 

   @details

   @param None

   @retval None
*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
extern void rfgnss_core_set_network_band(sys_band_class_e_type band);
#endif
/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function gets the baseband frequency offset (rotator) for this device. 

   @details

   @param None

   @retval None
*/
extern float rfgnss_core_get_bb_freq_offset(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function indicates to GPS device that the GSM band has changed.  
   This can be used to update the device's blanking setting when GSM band 
   changes (handover) during a GPS session. 

   @details

   @param band: new GSM band

   @retval None
*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
void rfgnss_core_indicate_gsm_band_change( rfi_band_type band);
#endif
/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function cancels the I&Q DC for gps chain. Description of this 
   algorithm can be found in document number DOCUMENT NUMBER NOT AVAILBLE. 
   Contact Tim Pals.

   @details

   @param None

   @retval None
*/
void rfgnss_core_sw_dc_cancellation(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function periodic cancels the I&Q DC for gps chain. It should be called
   by GPSSRCH task every 1 second after GPS receiver is turned on.

   @details

   @param None

   @retval None
*/
void rfgnss_core_sw_periodic_dc_update(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function determines the scale factor to convert Aries Base band I/Q 
   mean to millivolts.

   @details

   @param None

   @retval scale factor
*/
void rfgnss_core_get_scale_factor(float * adc_scale_factor);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function adjusts bbrx vref as needed. 

   @details

   @param None

   @retval scale factor
*/
void rfgnss_core_sw_gain_adjust(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function PGA gain to factory value. 

   @details

   @param None 

   @retval None
*/
void rfgnss_core_gain_use_factory(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function is periodically called by gnss software. The RF driver will then
   query the gnss software for dc and amplitude values and update the RF as needed.

   @param device : RF Multimode device
*/
int32 rfgnss_core_trigger_bbc_update(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles needed library support for resuming GPS when it shares 
   second rx chain with 1x page. Applicable when 1x target uses RF ASIC 
   typically found on UMTS target

   @details

   @param None

   @retval None
*/
void rfgnss_core_resume( void );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles needed library support for pausing GPS when it shares 
   second rx chain with 1x page. Applicable when 1x target uses RF ASIC 
   typically found on UMTS target

   @details

   @param None

   @retval None
*/
void rfgnss_core_pause( void );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function configures external GPS LNA via GPIO(s).

   @details

   @param None

   @retval None
*/
void rfgnss_core_set_ext_lna_on( boolean ext_lna_on, rf_path_enum_type path );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function cleans up the MSM and RTR for bootup cal. It's called after the
   GNSS RTR Front-end cal
 

   @details

   @param None

   @retval None
*/
void rfgnss_core_do_boot_procedure_cleanup(void);

/*! @todo gordon: to be removed after debugging */
/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function gets Pre_Mean_Mon for debugging.

   @details

   @param None

   @retval None
*/
void rfgnss_core_get_premean_mon(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function setup the MSM and RTR for bootup cal. It's called before the
   GNSS RTR Front-end cal
 

   @details

   @param None

   @retval None
*/
void rfgnss_core_do_boot_procedure_setup(rfgnss_mode_cfg_enum_type rfgnss_mode);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function gets Pre_Mean_Mon and pre_amp.

   @details

   @param None

   @retval None
*/
void rfgnss_core_get_pre_mean( gnss_MeanMonType *rfgnss_pri_premean_mon);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function gets Pre_Mean_Mon and pre_amp.

   @details

   @param None

   @retval None
*/
void rfgnss_core_get_pre_amp( gnss_AdcProcAmplMonType *rfgnss_pri_preamp_mon);

/*----------------------------------------------------------------------------*/
/*!
   @brief
 
  This function performs the Front End calibration on platforms that require
  it.

    
   @param None

   @retval None
    boolean -- TRUE if cal success, else FALSE
    band_tune and selectivity_tune updated with results if cal is success
    band_tune_energy -- peak energy found during band_tuning
    selectivity_tune_energy -- energy found at optimal selectivity_tune value.

    The reported energies can be used to find the suppression obtained.  The
    algorithm below has a target value of 10 dB.

*/
void rfgnss_core_fe_calibration( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function setup the MSM for bootup cal. It's called before the
   GNSS RTR Front-end cal
 

   @details

   @param None

   @retval None
*/
void rfgnss_core_do_boot_procedure_msm_setup(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This API sets up the environment based of presence or absence or an external LNA. 
*/
void rfgnss_core_has_ext_lna (boolean gnss_has_ext_lna);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Sets up the environment for the card to configure depending on if external LNA is used or not. 
*/
void rfgnss_core_configure_ext_lna (boolean gnss_has_ext_lna);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Sets up the environment for intelleciver to configure depending on if external LNA is used or not. 
*/
void rfgnss_core_intelliceiver_configure_ext_lna (boolean gnss_has_ext_lna);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf static status log and send the log packet to GNSS SW.

   @details

   @param None

   @retval None
*/
void rfgnss_core_rf_status_log(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf dynamic status log and send the log packet to GNSS SW.

   @details

   @param None

   @retval None
*/
void rfgnss_core_rf_dynamic_status_log(void);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function returns the rf gnss status (On/Off).

   @details

   @param None

   @retval None
*/
boolean rfgnss_core_gnss_onoff_status(void);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf gnss 1x band/chan state variable.

   @details

   @param None

   @retval None
*/
void rfgnss_core_1x_band_chan(rf_card_band_type band, word chan);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf gnss lte band/chan state variable.

   @details

   @param None

   @retval None
*/
void rfgnss_core_lte_band_chan(rfcom_lte_band_type band, uint16 chan);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf gnss evdo band/chan state variable.

   @details

   @param None

   @retval None
*/
void rfgnss_core_evdo_band_chan(rf_card_band_type band, uint32 chan);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf gnss 1x band/chan state variable.

   @details

   @param None

   @retval None
*/
void rfgnss_core_wg_chan(uint16 channel);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
void rfgnss_core_program_notch_filter(rfgnss_notch_cfg_type notch_cfg);


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
  This function is used for debug purposes. The function queries
  the GPS state and returns TRUE if GPS is ON and FALSE if GPS is 
  turned OFF.

   @param device : RF Multimode device
 
          Boolean - TRUE if GPS State is ON and FALSE if turned OFF.
*/
boolean rfgnss_core_debug(boolean verbose, rfcom_device_enum_type device);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the Tx flag and lowers the PGA gain
   if the hysteresis timer is OFF and GPS state is ON.

   @details

   @param boolean

   @retval None
*/
void rfgnss_core_tx_indicator(boolean enable);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function clears the hysteresis timer and restores the PGA gain. 

   @details

   @param timer_cb_data_type

   @retval None
*/
void rfgnss_core_tx_hysteresis_callback(timer_cb_data_type data);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function clears the hysteresis timer.

   @details

   @param None

   @retval None
*/
void rfgnss_core_tx_stop_hysteresis(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function initializes the state machine and defines the timer at init.

   @details

   @param None

   @retval None
*/
void rfgnss_core_boot_gain_adjust_init(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function lowers the PGA gain during GPS session when LTE Tx turns ON.

   @details

   @param None

   @retval None
*/
void rfgnss_core_gain_backoff(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function restores the PGA gain during GPS session when LTE Tx turns OFF.

   @details

   @param None

   @retval None
*/
void rfgnss_core_gain_restore(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function queries the nav version. The same modem
   can have different versions of the GPS engine.

   @param device : RF Multimode device
*/
void rfgnss_core_get_nav_version(void);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Adjusts GPS Rx front end gain and save the values
   that will be used later when LTE Tx turns ON.

   @details

   @param device: rfcom device enum

   @retval None

*/
void rfgnss_core_boot_gain_adjust( rfcom_device_enum_type device );


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the state machine at exit, pause and
   resume. 

   @details

   @param None

   @retval None
*/
void rfgnss_core_gain_adjust_state_update(void);


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function configures the PMIC XO bit.

   @details


   @param 
   @param 
*/

void rfgnss_core_config_pmic_xo_reso ( boolean on_off );

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function setup the MSM for bootup cal. It's called before the
   GNSS RTR Front-end cal


   @details

   @param None

   @retval None
*/
void rfgnss_core_do_boot_procedure_msm_setup_mode(rfgnss_mode_cfg_enum_type gnss_mode,
                                                  rfgnss_lin_type_enum_type gnss_linearity);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Perform GNSS demod quality control

   @param device : demod quality control
     RFGNSS_DEMOD_QUALITY_HIGH: Error-free demod is required
     RFGNSS_DEMOD_QUALITY_NORM: Error-free demod is not required


   @pre
   At least one call to rfm_enter_mode() to start a GNSS session must have been
   executed prior calling this pause function
*/
void rfgnss_core_demod_quality_control_npler(rfgnss_demod_quality_enum_type rfgnss_demod_quality);

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Perform GNSS demod quality control

   @param: demod quality control
     RFGNSS_DEMOD_QUALITY_HIGH: Error-free demod is required
     RFGNSS_DEMOD_QUALITY_NORM: Error-free demod is not required


   @pre
   At least one call to rfm_enter_mode() to start a GNSS session must have been
   executed prior calling this pause function
*/
void rfgnss_core_demod_quality_control_pm(rfgnss_demod_quality_enum_type rfgnss_demod_quality);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function saves the last good known PGA value.

   @details

   @param None

   @retval None
*/
void rfgnss_core_save_pga(void);

void rfgnss_core_pga_tracking_loop_control(boolean enable);

/*----------------------------------------------------------------------------*/
void rfgnss_core_notch_filter_timer_init(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
void rfgnss_core_program_notch_callback(timer_cb_data_type data);

/*----------------------------------------------------------------------------*/
void rfgnss_core_clear_notch_timer(void);


void rfgnss_core_imd_band_chan(void);

void rfgnss_core_imd_tx_onoff(boolean enable);

void rfgnss_core_wwan_tx_timer_init(rfcom_device_enum_type device);


void rfgnss_core_adc_clk_ctl(boolean enable);

void rfgnss_core_determine_lna(void);
/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for dev on/off

*/
boolean rfgnss_core_resource_vote(rfgnss_device_status_type dev_status, rfgnss_device_vote_type dev_vote);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS device sequence execute command dispatch

*/
boolean rfgnss_core_device_seq_execute(rfm_gnss_device_seq_cmd_type cmd, void *data);


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function is used to disable/enable GNSS PGA gain backoff

   @param enable: TRUE to enable or FALSE to disable

*/
void rfgnss_core_pga_gain_backoff_control(boolean enable);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Update notches with TDSCDMA band and channel information
 
*/
void rfgnss_core_tdscdma_band_chan(rfcom_tdscdma_band_type band, uint16 chan); 

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function is used to trigger BP2 gain step calibration

   @param enable: TRUE to enable or FALSE to disable

*/
void rfgnss_core_bp2_gainstep_adjust(void);

#endif /* RFGNSS_CORE_H */
