
/*!
   @file
   rfgnss_mc.c

   @brief
    This file contains all declarations and definitions necessary to use the GNSS
    Main Control.

   @details

*/

/*===========================================================================
Copyright (c) 1999 - 2002, 2006, 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008, 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2010, 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/gen8/rf/mc/src/rfgnss_mc.c#1 $
$DateTime: 2016/12/13 07:59:49 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/31/14   ndb     Added support for extended EARFCN for LTE
06/21/13   jn      Factory calibrated PGA option for WWAN co-exist.
05/07/13   dbz     Implemented Beidou BP2 gain step calibration
04/30/13   gy      Added API for GPS to vote rfc common to wakeup or sleep
04/09/13   dbz     Add new bootup FE and gain calibration API
04/08/13   dbz     Fixed PGA gain backoff related issue
03/28/13   gy      Add TDSCDMA band, channel status
03/14/13   gy      Enabled pga gain timer initialization
03/07/13   dbz     Mainline IMD feature for DIME PLs 
03/04/12   dbz     Reinitialize GNSS ADC when Nav Core goes into Power Collapse during DPO Idle
02/11/13   dbz/gy  Added API to disable/enable gnss pga gain backoff
02/11/13   gy      Added gnss rfsw repartition
11/16/12   dbz     Enable notch timer init
11/02/12   jfc     Replace rf_hal_enable_rfcmd_app with rf_hal_bus_enable 
11/01/12   dbz     Add support for GPS device
10/31/12   dbz     Enable GPS DPO algorithms
09/19/12   dbz     Added GPS voting for RFCMD vote
08/14/12   dbz     Disabled bootup cal for DIME bringup
05/21/12   gy      Added gnss on/off status API
04/26/12   vvr     Added FE CAL algorithms
03/29/12   dbz     Added ADC clock managment in DPO pause/resume
02/24/12   dbz     Implemented new ADC power up/down sequence
01/31/12   jn      Added support for run-time selection of notch filter settings.
12/09/11   dbz     Update to GNSS On/Off status 
12/09/11   dbz     Add GNSS IMD feature
12/02/11   dbz     Enabled 1x/DO GNSS notch programming
11/14/11   dbz     Added device type and changed 1x band type in TX indicator
10/03/11   gy      Supported GPS vote for A0 in normal demod quality
10/03/11   dbz     Added timer based hysteresis loop for notch filter programming
09/22/11   dbz     Using default gain code for bootup gain cal
09/13/11   dbz     Added LTE B13 gain adjustment algorithm
08/25/11   dbz     Add PGA tracking loop control
08/08/11   gy      Enabled rfgnss_init
08/02/11   dbz     Add new API for GNSS PGA tracking loop control
07/27/11   gy      Enabled gain/DC loop
07/06/11   gy      Support gnss on different devices
06/30/11   dbz     Disable gain/DC loop pending GPS SW dependency in CRM
06/29/11   dbz     Enabled gain/DC correction
04/23/11   gy      Ported for nikel
04/14/11   sar     Removed deprecated file rfc.h.
04/13/11   dbz     Disabled GNSS bootup FE cal pending dependency
11/24/10   dbz     Fixed race condition for LTE gain backoff algorithms
10/29/10   av      Update LTE bootup gain adjust algorithm for M5
10/11/10   hp      Fix LTE bootup gain adjust algorithm
10/05/10   av      Merge changes from Genesis
09/24/10   hp      Fixed compiler warnings
09/13/10   hp      Fixed compiler warnings
09/10/10   hp      Update to LTE GPS B13 Gain Adjust Algorithm
08/26/10   hp      Added API's for GPS Notch for LTE and EVDO
08/12/10   dbz     Fix complier warnings
07/13/10   dbz     Added suport for GNSS Demod quality control
07/10/10   dbz     Added support for Npler manager
06/30/10   hp      Added Support for LTE GPS B13 Gain Adj for SVLTE-Fusion
06/25/10   gy      Added GNSS Demod Quality API
06/02/10   dbz     Merged Mode 5 SRS support from Dev brach
05/27/10   dbz     Added support for FE cal for cucurrent ARS/SRS config
05/25/10   dbz     Added support for managing PMIC XO resonator for DPO
05/21/10   gy      Re-checked in RF DPO with power collapse
05/17/10   gy      Back out RF DPO with power collapse
04/16/10   dbz     Added optimized DPO algorithms
04/09/10   dbz     Added support for WB GNSS sync path support
                   for Callisto 1.x
03/22/10   gy      Turned off external lna after the bootup init
02/15/10   dbz     Adding runtime support for GNSS RTR FE cal band tuning
                   for both ELNA and non-ELNA HW
29/01/10   sr      Changed the <tech>_mc_init() signature to fix warnings in
                   <tech> specific builds
01/26/10   sar     Lint fixes.
01/06/10   dbz     Added rfm_gnss_debug support
12/17/09   dbz     Changed rfgnss_notch_cfg_type
11/30/09   bmg     Added common_init_successful parameter to initialization
                   Skip initialization if the common NV could not be loaded
                   Changed init function return to boolean
11/25/09   dbz     Optimized Gain adjusmtnet algo and DC correction timing
11/12/09   gy      Added rfgnss_mc_exit_mode
11/06/09   dbz     Added the GNSS RTR function manager based on
                   config/linearity/ELNA mode
11/04/09   dbz     Fixed the GPS exit chip sleep issue
10/22/09   dyc     Mode specific build support.
10/22/09   gy      Added DPO mobile messages
10/21/09   dbz     Enabled RF status report
10/16/09   dbz     Added support for GNSS spur mitigation algorithms
10/15/09   hp      Added function needed to support ELNA API
10/13/09   dbz     Added GNSS ADC RC Tuning algorithms
09/30/09   dbz     Added RF status report support
09/28/09   gy      Code clean up to resolve compilation warnings.
09/13/09   gy      Added pause/resume for DPO support
09/10/09   gy      Added GPIO initialization based on card file
09/09/09   dbz     New FE cal spproach/Enabled R/RC tuner cal
08/30/09   gy      Cleaned up Xo trim bring-up code
08/28/09   dbz     Enabled FE cal at bootup
07/29/09   dbz     Added RTR bootup cal algorithms
04/15/09   dbz     Refactor call flow to gnss_core
04/07/09   dbz     Adding rfgnss_mc_init to fix link error
03/12/09   gy      Uncmmented out GPS BB functions
03/10/09   gy      Comment out GPS BB functions because GPS SW is not in CRM)
03/10/09   gy      Code clean up
12/20/08   gy      Initial version to port gnss main control.
10/14/08   sr      Initial version of Gps Main Control.

============================================================================*/

#include "rfa_variation.h"
#include "rfcom.h"
#include "rfgnss_mc.h"
#include "rfgnss_core.h"
#include "rfm_gnss.h"
#include "rfc_pm.h"
#include "msg.h"
#include "timer.h"
#include "rfm_device_types.h"
#include "DALSys.h"
#include "rf_hal_common.h"
#include "rfc_common.h"
#include "gnss_api.h"

#define RFGNSS_WWAN_COEXIST_BIT 0x80
int8 rfgnss_pga_cal_nv = 0;

gnss_adc_enum_type rfgnss_adc;

extern gnss_linearity_mode_type gnss_lin_mode;

extern boolean rfgnss_status;

extern rfgnss_notch_cfg_type rfgnss_spur_system_det;

extern boolean gnss_external_lna_ctrl;

rfgnss_gain_adjust_type rfgnss_gain_adjust;

rfgnss_notch_filter_timer_type rfgnss_notch_filter_timer;

boolean rfgnss_wwan_tx_status = FALSE;

static rfgnss_demod_quality_enum_type cur_demod_status = RFGNSS_DEMOD_QUALITY_HIGH;

rf_hal_bus_client_handle* rf_gnss_hal_bus_client_handle;

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

  @todo Dongbo to Implement this function, should do the rfdevice fp init
  and prepare for bootup FE tuning

*/
boolean rfgnss_mc_init
(
rfcom_device_enum_type device,
boolean common_init_successful,
rex_tcb_type *caller_tcb_ptr,
rex_sigs_type task_nv_wait_sig,
void (*task_wait_func_ptr)( rex_sigs_type )
)
{
 #ifdef FECAL_DEBUG
    MSG_ERROR("FECAL_DEBUG: inside rfgnss_mc_init common_init_successful=%d ", common_init_successful,0,0);
 #endif
  
  if ( common_init_successful )
  {

#ifndef T_MDM9615
    /* select appropriate notch filter table */
    rfgnss_core_baseband_init();
#endif
    rfgnss_core_notch_filter_timer_init(device);

    rfgnss_core_wwan_tx_timer_init(device);

    return(TRUE);
  } /* if ( common_init_successful ) */
  else
  {
    return(FALSE);
  } /* ! if ( common_init_successful ) */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
   This is the main control to enter into the gnss mode.

   @param device : RF Multimode device
*/
uint32 rfgnss_mc_enter_mode(rfcom_device_enum_type device,
                            const void *user_data_ptr,
                            rfm_cb_handler_type cb_handler)
{
  MSG_HIGH("rfgnss_mc_enter(%d)", device, 0, 0);

  switch (device )
  {
  case RFCOM_RECEIVER_1:
  case RFCOM_RECEIVER_2:
  case RFCOM_RECEIVER_3:
  case RFM_DEVICE_GPS:

    /* set the PMIC XO reso bit */
    rfgnss_core_config_pmic_xo_reso(TRUE);

    break;

  default:
    MSG_ERROR("RF GNSS: UNSUPPORTED Device Descriptor",0,0,0);
    break;
  }
  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Configure GNSS mode operation after entering gnss mode.

   @param device : RF Multimode device
   @param gnss_mode : GNSS Mode
*/
int32 rfgnss_mc_configure ( rfcom_device_enum_type device, rfgnss_mode_cfg_enum_type rfgnss_mode )
{

  MSG_HIGH(" Entering rfgnss_mc_configure(%d, %d)", device, rfgnss_mode, 0);

  // vote for RFCMD app
  rf_hal_bus_enable(TRUE, &rf_gnss_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(GPS));

  rfgnss_core_rf_card_init (device, rfgnss_mode);

  rfgnss_core_adc_init(device, rfgnss_mode);

  rfgnss_core_bbc_init(rfgnss_mode);

#ifdef FEATURE_GNSS_ONLY_NO_WWAN
  MSG_HIGH("before rfgnss_pga_cal_nv=%02x %d", rfgnss_pga_cal_nv, rfgnss_pga_cal_nv, 0);

  if (gnss_ApqPgaCalNvReadWrite(&rfgnss_pga_cal_nv, TRUE) //read the NV item
      && (rfgnss_pga_cal_nv != -1) //case for NV item not initialized
      && (rfgnss_pga_cal_nv & RFGNSS_WWAN_COEXIST_BIT)) //bit set means use factory val
    rfgnss_core_gain_use_factory();
  else 
#endif
    rfgnss_core_gain_adjust(device);

#ifdef FEATURE_GNSS_ONLY_NO_WWAN
  MSG_HIGH("read rfgnss_pga_cal_nv=%02x %d", rfgnss_pga_cal_nv, rfgnss_pga_cal_nv, 0);
#endif

  if (rfgnss_mode == RFGNSS_MODE_CFG_BEIDOU) 
  {
    /* perform BP2 gain step calibration*/

    rfgnss_core_bp2_gainstep_adjust();
  }

  rfgnss_core_rf_status_log();

  MSG_HIGH("Exiting rf_enter_gps()", 0, 0, 0);

  /* update the RF Gnss On/Off status */

  rfgnss_status = TRUE;

  /* proram the notch filters */

  rfgnss_core_program_notch_filter(rfgnss_spur_system_det);

  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Exit gnss operation after entering gnss mode.

   @param device : RF Multimode device
*/
int32 rfgnss_mc_exit ( rfcom_device_enum_type device )
{

  MSG_HIGH("rfgnss_mc_exit", 0, 0, 0);

  /* update the RF Gnss On/Off status */

  rfgnss_status = FALSE;

  switch (device )
  {
  case RFCOM_RECEIVER_1:
  case RFCOM_RECEIVER_2:
  case RFCOM_RECEIVER_3:
  case RFM_DEVICE_GPS:  

    /* turn off ADC clock before programming RTR and ADC registers */
    rfgnss_core_adc_clk_ctl (FALSE);

    rfgnss_core_sleep();
    rfgnss_core_adc_wakeup(GNSS_ADC_NIKEL,RFGNSS_ADC_DISABLE);
    /* set the PMIC XO reso bit */
    rfgnss_core_config_pmic_xo_reso(FALSE);
    break;

  default:
    MSG_ERROR("RF GPS: UNSUPPORTED Device Descriptor",0,0,0);
    break;
  }

  rfgnss_core_gain_adjust_state_update();
  rfgnss_core_clear_notch_timer();

  // release vote for RFCMD app
  rf_hal_bus_enable(FALSE, &rf_gnss_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(GPS));

  MSG_HIGH("Exiting rf_exit_gps", 0, 0, 0);
  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
int32 rfgnss_mc_resume(rfcom_device_enum_type device)
{
  MSG_HIGH("Entering GNSS DPO Resume State.", 0, 0, 0);

  // vote for RFCMD app
  rf_hal_bus_enable(TRUE, &rf_gnss_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(GPS));

  rfgnss_core_resume();

  /* turn Off ADC clock before programming RTR and ADC registers */
  rfgnss_core_adc_clk_ctl (FALSE);

  rfgnss_core_adc_init(device, RFGNSS_MODE_CFG_WB);

  /* update the RF Gnss On/Off status */
  rfgnss_status = TRUE;

  /* proram the notch filters */
  rfgnss_core_program_notch_filter(rfgnss_spur_system_det);
  MSG_HIGH("Exiting GNSS DPO Resume State.", 0, 0, 0);

  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Pause gnss operation for fts or dpo.

   @param device : RF Multimode device

   @pre
   At least one call to rfm_enter_mode() to start a GNSS session must have been
   executed prior calling this pause function
*/
int32 rfgnss_mc_pause(rfcom_device_enum_type device)
{
  MSG_HIGH("Entering GNSS DPO Pause State.", 0, 0, 0);

  /* update the RF Gnss On/Off status */
  rfgnss_status = FALSE;

  rfgnss_core_pause();

  /* turn Off ADC clock before programming RTR and ADC registers */
  rfgnss_core_adc_clk_ctl (FALSE);

  rfgnss_core_adc_wakeup(GNSS_ADC_NIKEL,RFGNSS_ADC_DISABLE);

  // release vote for RFCMD app
  rf_hal_bus_enable(FALSE, &rf_gnss_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(GPS));

  MSG_HIGH("Exiting GNSS DPO Pause State.", 0, 0, 0);

  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function is periodically called by gnss software. The RF driver will then
   query the gnss software for dc and amplitude values and update the RF as needed.

   @param device : RF Multimode device
*/
int32 rfgnss_mc_trigger_bbc_update(void)
{
  (void)rfgnss_core_trigger_bbc_update();

  rfgnss_core_rf_dynamic_status_log();

  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function handles rf mode change.

   @param device : RF Multimode device
   @param to_mode : Mode or technology which will be using the transceiver next
*/
int32 rfgnss_mc_exit_mode(rfcom_device_enum_type device,
                          rfcom_mode_enum_type to_mode)
{
  return 0; /* no error */
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
int32 rfgnss_mc_1x_band_chan(rfm_cdma_band_class_type band, word chan)
{

  /* setup the system det for spur purposes */

  rfgnss_spur_system_det = RFGNSS_NOTCH_CFG_1X;

  rfgnss_core_1x_band_chan((rf_card_band_type)band, chan);

  rfgnss_mc_program_notch_filter(rfgnss_spur_system_det);

  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Provides information regarding RF Band and Chan for GNSS Notch Programming

   @details

   @param device : RF Multimode device
*/
int32 rfgnss_mc_evdo_band_chan(rfm_cdma_band_class_type band, uint32 chan)
{

  /* setup the system det for spur purposes */

  rfgnss_spur_system_det = RFGNSS_NOTCH_CFG_EVDO;

  rfgnss_core_evdo_band_chan((rf_card_band_type)band, chan);

  rfgnss_mc_program_notch_filter(rfgnss_spur_system_det);

  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
int32 rfgnss_mc_wg_chan(uint16 chan)
{
  rfgnss_core_wg_chan(chan);

  return(TRUE);
}


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
int32 rfgnss_mc_program_notch_filter(rfgnss_notch_cfg_type notch_cfg)
{

  static rfgnss_notch_cfg_type cur_notch_cfg = RFGNSS_NOTCH_CFG_NONE;

  /* setup the system det for spur purposes */
  rfgnss_spur_system_det = notch_cfg;

  /* check if it's GSM */

  if (notch_cfg == RFGNSS_NOTCH_CFG_GSM)
  {
    if (!(cur_notch_cfg == RFGNSS_NOTCH_CFG_GSM))
    {
      rfgnss_core_program_notch_filter(notch_cfg);
    }
  } else
  {  
    rfgnss_core_program_notch_filter(notch_cfg);


    
    rfgnss_core_imd_band_chan();

  }

  cur_notch_cfg = notch_cfg;

  rfgnss_notch_filter_timer.rfgnss_notch_cfg= notch_cfg;

  return(TRUE);
}



/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Dispatch rf gnss control commands according to the specified command enumeration
   number.

   @param device : RF Multimode device

   @pre
   At least one call to rfm_enter_mode() to start a GNSS session must have been
   executed prior calling this pause function
*/
int32 rfgnss_mc_cmd_dispatch(rfcom_device_enum_type device, int32 cmd, void *data)
{
  switch (cmd)
  {
  /* for future expansion */
  case RFGNSS_CMD1:
    break;
  case RFGNSS_CMD2:
    break;
  }
  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This API sets up the environment based of presence or absence or an external LNA.

   @param device : RF Multimode device

   @param gnss_mode : GNSS Mode

*/

int32 rfgnss_mc_has_ext_lna (boolean gnss_has_ext_lna)
{
  MSG_HIGH("Entering rfgnss_mc_has_ext_lna",0,0,0);
  rfgnss_core_has_ext_lna (gnss_has_ext_lna);
  MSG_HIGH("Exiting rfgnss_mc_has_ext_lna",0,0,0);
  return(TRUE);
}

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
boolean rfgnss_mc_debug(boolean verbose, rfcom_device_enum_type device)
{
  return rfgnss_core_debug(verbose, device);
}

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
int32 rfgnss_mc_demod_quality_control(rfgnss_demod_quality_enum_type rfgnss_demod_quality)
{
  MSG_HIGH("GNSS SW calls for Demod Quality Control: %d", (int)rfgnss_demod_quality, 0, 0);

  if (!(cur_demod_status == rfgnss_demod_quality))
  {
    rfgnss_core_demod_quality_control_pm (rfgnss_demod_quality);

    /* manage the Npler preamption */
    rfgnss_core_demod_quality_control_npler (rfgnss_demod_quality);
  }

  cur_demod_status = rfgnss_demod_quality;

  return(TRUE);
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the LTE Tx band

   @details


   @param
   rfcom_lte_band_type
*/
void rfgnss_mc_tx_band(rfcom_lte_band_type rf_tx_band)
{
  rfgnss_gain_adjust.rf_tx_band_config = rf_tx_band;
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the LTE Tx flag and lowers
   the PGA gain if needed

   @details


   @param  None

*/
void rfgnss_mc_tx_indicator( boolean enable )
{
  rfgnss_core_tx_indicator(enable);
}

/*!
   @brief
   Update notches with LTE band and channel information

   @details


   @param  LTE band and channel

*/
void rfgnss_mc_lte_band_chan(rfcom_lte_band_type band, rfcom_lte_earfcn_type chan)
{

  rfgnss_core_lte_band_chan(band,chan);


  /* setup the system det for spur purposes */
  MSG_HIGH("GNSS IMD LTE Band chan ", 0, 0, 0);
  rfgnss_spur_system_det = RFGNSS_NOTCH_CFG_LTE;
  rfgnss_mc_program_notch_filter(rfgnss_spur_system_det);

}

/*!
   @brief
   Update notches with TDSCDMA band and channel information

   @details


   @param  TDSCDMA band and channel

*/
void rfgnss_mc_tdscdma_band_chan(rfcom_tdscdma_band_type band, uint16 chan)
{

  rfgnss_core_tdscdma_band_chan(band,chan);

  /* setup the system det for spur purposes */
  MSG_HIGH("GNSS IMD TDSCDMA Band chan ", 0, 0, 0);
  rfgnss_spur_system_det = RFGNSS_NOTCH_CFG_TDSCDMA;
  rfgnss_mc_program_notch_filter(rfgnss_spur_system_det);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the WWAN Tx status flag.
 
   @details
   

   @param  Enable/Disable
   
*/
int32 rfgnss_mc_wwan_tx_status(rfm_device_enum_type device, boolean enable )
{
  if (device == RFM_DEVICE_0)
  {
    //rfgnss_wwan_tx_status = enable;
    //MSG_1(MSG_SSID_RF,MSG_LVL_HIGH,"WWAN TX events triggered, state = %d", (int16)enable);



    rfgnss_core_imd_tx_onoff(enable);

  }
  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS PGA tracking loop control

*/
void rfgnss_mc_pga_tracking_loop_control(boolean enable)
{

  MSG_HIGH("GNSS PGA tracking loop flag called from GNSS SW, flag set to %d", enable, 0, 0);
  rfgnss_core_pga_tracking_loop_control(enable);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function provides GNSS on/off status

   @param device: none

*/
boolean rfgnss_mc_get_state(void)
{
  return rfgnss_core_get_state();
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for dev on/off

*/
boolean rfgnss_mc_resource_vote(rfgnss_device_status_type dev_status, rfgnss_device_vote_type dev_vote)
{
  boolean ret_value = FALSE;

  ret_value = rfgnss_core_resource_vote(dev_status, dev_vote);
  return ret_value;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS device sequence execute command dispatch

*/
boolean rfgnss_mc_device_seq_execute(rfm_gnss_device_seq_cmd_type cmd, void *data)
{
  return rfgnss_core_device_seq_execute(cmd, data);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for ccs enable

*/
boolean rfgnss_mc_hal_bus_enable (boolean enable)
{
  return rf_hal_bus_enable(enable, &rf_gnss_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(GPS));
}
/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function is used to disable/enable GNSS PGA gain backoff

   @param device: none

*/
void rfgnss_mc_pga_gain_backoff_control(boolean enable)
{
  rfgnss_core_pga_gain_backoff_control(enable);
  MSG_HIGH("GNSS PGA gain backoff control: %d", enable, 0, 0);
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
 
   @details
   This function is used to initiate GNSS boot-up calibration from
   GNSS SW once NPA resources are available and GNSS SW can turn clocks
   ON.

*/
void rfgnss_mc_bootup_cal(rfcom_device_enum_type device)
{
  /* TBD: Check for common_init_successful ?*/

  // vote for RFCMD app
  rf_hal_bus_enable(TRUE, &rf_gnss_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(GPS));

  rfgnss_core_boot_gain_adjust_init(RFCOM_TRANSCEIVER_0);

  /* force Mode 5 */
  //rfgnss_core_do_boot_procedure_msm_setup();
  rfgnss_core_get_nav_version();
  rfgnss_core_do_boot_procedure_msm_setup_mode(RFGNSS_MODE_CFG_WB, RFGNSS_LIN_HL);

  gnss_lin_mode = GNSS_LINEARITY_MODE_HIGH;
  rfgnss_core_do_boot_procedure_setup(RFGNSS_MODE_CFG_WB);

  rfgnss_core_do_r_rc_tune_cal(RFCOM_TRANSCEIVER_0);

  rfgnss_core_fe_calibration(RFCOM_TRANSCEIVER_0);

  rfgnss_core_boot_gain_adjust(RFCOM_TRANSCEIVER_0);

  rfgnss_core_do_boot_procedure_cleanup();

  // release vote for RFCMD app
  rf_hal_bus_enable(FALSE, &rf_gnss_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(GPS));
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   API for NavRF to vote for RFC wakeup or sleep

*/
boolean rfgnss_mc_rfc_common_enable(boolean enable)
{
  boolean status = FALSE;

  if (enable)
  {
    status = rfc_common_wakeup(RF_PATH_3, RFM_GNSS_MODE);
  }
  else
  {
    status = rfc_common_sleep(RF_PATH_3, RFM_GNSS_MODE);
  }

  return status;
}
