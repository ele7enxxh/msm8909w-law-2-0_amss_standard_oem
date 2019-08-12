/*!
   @file
   rfc_card_gsm.c

   @brief
   This file contains all the GSM mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2011 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/rfc/src/rfc_card_gsm.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
12/15/14   sml     Enable DAC VREF only during Tx enable
12/15/14   sml     Rewrite ASD API to reduce wake-up timeline                   
12/02/14   zbz     Make rfc gsm timing ptr per device
10/27/14   vv      Add papm device info api support
10/23/14   sc      Enable event type control from RFSW
09/24/14   hoh     Add multislot APT control feature
09/23/14   zbz     changes to support GSM and LTE/TDS coexistence related RFC changes
08/22/14   rr      Changes to support nv c3
08/04/14   tws     Stub functionlaity for RUMI pre-silicon
07/31/14   tsr     GSM Alternate path support
07/31/14   sw      Support both rffe and grfc-based xsw for ASD
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/04/14   jr      Changes for TSTS support
06/02/14   sml     Adding necessary check condition in the case where tuners are not present 
                   and ASD switch is present to avoid un-necessary prints
05/19/14   tsr     Only Query device support for RX configuration on a RXD device 
                   to avoid uncessary f3 from Device info
04/29/14   sc      Manage Vregs based on subscription
04/29/14   tsr     Mainline GSM RFC config v2 apis 
04/24/14   sw      Introduce rffe-based api for ASD processing
04/14/14   sw      Disable tuners during X2G iRAT scenarios (port from DI3)
04/07/14   sc      Vote vregs based only on single GSM tech
03/25/14   zbz     Add api to get device_id info
03/20/14   sw      Introduce api to get signal-specific timing info
03/17/14   kg      Port from Dime
02/11/14   as/sr   Re-design asdiv_tuner to support multiple RF configurations
03/05/14   tsr     Removed unwanted parameters to tx_band_config api
02/25/14   tsr     Update RX PLL setting time from device driver to rfgsm core rx timing struct
02/24/14   tsr     Mainline device driver script retrieval
02/11/14   vv      backedout the previous change
11/12/13   dps     Updates to support C++ class based GSM Rx and Tx interface
10/01/13   tsr     Added support retrieve individual device scripts
10/01/13   sb      Disable SGLTE Coexistence filtered path cal data
09/25/13   sb      Added changes to support GSM TX calibration/online mode operation in coexistence
09/17/13   sb      Support for SGLTE/SGTDS coexistence
08/28/13   sml     Extracting ASD script depending on device id through
                   call back data
08/27/13   sb      Add changes to update RX timing for GSM RXD
08/02/13   sml     Fixing Complier warnings
08/01/13   sb      Added interface chanegs to support GSM RX diversity
07/30/13   sml     Correcting definition as per RF common changes
07/29/13   sml/svi ASD support for GSM tech
07/29/13   tws     Use new RF_TASK_1 for GSM.
07/08/13   yzw     Add ASM support for FTM command FTM_GET_RFFE_DEVICE_INFO
06/20/13   sb      Add hooks to populate Spur channel list from RFC
06/17/13   svi     "TX_GTR_THRES" NV values interpretation fix.
05/30/13   tws     Add support for GSM sub 3
05/30/13   tws     Mainline the context checking.
05/07/13   tws     Support DUAL_SIM for rfc voting
04/08/13   tc      Add QPOET/QTUNER timing override facility
04/02/13   sb      Fix to return correct device object pointer using device type
03/27/13   sb      Added support for Enhanced APT
03/21/13   yzw     Add device driver support to query PA device information {mfg_id, prd_id, rev_id}
03/14/13   aka     Fix to select correct instance from RFC for multiple instances of same device
02/27/13   lcl     Added support for two or more of the same devices type.
02/15/13   sb      Added FTM hook to override tuner tune code 
02/05/13   sb      Created separate APIs to configure RF front end devices based on whether
                   they are called every TX/RX burst or only when the band is changed.
01/3/13    av      Changes to support chain2 for GSM
12/20/12   ggs     Add framework for appending rf_on grfc info from rfc  
12/19/12   sb      Optimize GSM APT
12/11/12   sb      Add hooks for GSM APT 
10/16/12   tsr     Cleanup of GSM Bringup flags 
10/01/12   sb      Made changes to add rfm device to rf device interface and uppler layer APIs 
09/24/12   sb      Made changes to update TXLM,RXLM modem chain
09/04/12   sr      added Tx_gain over-ride API.
08/31/12   sb      Made changes to ensure VREG management with RUMI bringup flag true 
08/13/12   tsr     Replaced Bringup feature with rumi_bup_gsm flag to facilitate Bringup
08/10/12   sr      Made changes to add the rf_on signal to transition tx script.
08/07/12   sr      added modulation type parameter for tx_gain onfig api
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/02/12   sr      renamed rfc_common_gsmctl.h to rfc_common_tdd.h as it is shared with TDS.
07/10/12   rmb     Removed critical section enter/exit and added these in rfc_common.c 
                   for wakeup/sleep function.
04/05/12   sr      made changes to put all the GPIOs into low power mode during sleep.
02/28/12   dej     Hooked up PA start/stop delta adjustments in FTM mode. 
                   ( Added rfc_gsm_tx_timing_adjust(band) ) 
11/05/11    aak     Enable NPA calls from RFSW for power management on 9x15 
10/11/11    sr     added rfc tx disable/enable sleep/wakeup calls to configure 
                   RFC signals for tx disable/enable  sleep/wakeup state.
10/11/11    sr     optimized code for ISR over-run issue.
08/25/11   aak     Enable RF NPA voting only for MSM8960 
08/17/11   av      Set GSM GRFCs to default/low state when mode transition to GSM. 
                   Temp change till we change the RF/FW interface to handle grfc logic
                   in burst scripts. 
08/15/11   sr      klockwork fixes.
08/08/11   bmg     Added extern "C" to rxf external structures to fix off-
                   target builds
08/06/11   sr      RFC Cleanup: removed un-used code.
04/28/11   rsr     Support for GSM to use NPA APIs
03/03/11   sr     off-target error fix.
02/18/11   sr      changed to call get_instance() for gsm rfc. 
11/21/11   sr     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfc_card.h"
#include "rfc_card_gsm.h"
#include "rfgsm_core.h"
#include "rfgsm_core_types.h"
#include "rfcommon_core_error.h"
#include "mdsp_intf.h"
#include "rfc_common_tdd.h" 
#include "rfm_internal.h"
#include "rfdevice_gsm_type_defs.h"
}

#include "rfc_gsm.h"
#include "rfc_gsm_data.h"
#include "rfdevice_rxtx_common_class.h"

#include "rfdevice_logical_component_types.h"
#include "rfdevice_trx_gsm_rx.h"
#include "rfcommon_concurrency_manager.h"

#define RFC_GSM_MAX_TECH_SUBSCRIPTIONS 3



/* ----------------------------------------------------------------------- */
/*!
  @brief
  Initializes the RFC data during power-up.

  @details
  Initializes the RFC data during power-up.

  @parameter
  None

*/
void rfc_gsm_init_once( rfm_device_enum_type rfm_dev )
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->init_once(rfm_dev);

  return;
} /* rfc_gsm_config_mode() */


/* ----------------------------------------------------------------------- */
/*!
  @brief
  Sets the specified path to the technology mode of operation.

  @details
  This funtion will call the card specific version of the set mode API.

  @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.

*/
void rfc_gsm_mode_config( rfm_device_enum_type rfm_dev, rf_buffer_intf* enter_mode_buffer, uint8 alt_path)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->config_mode(rfm_dev, enter_mode_buffer, alt_path);

  return;
} /* rfc_gsm_config_mode() */

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function returns top level ptr to the GSM configuration.

  @details
  The function will return GSM configuration such as GPIO/GRFC settings along
  with any other parameter data structures for a particular band.  The void*
  allows for casting for usage under various band types.

  @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
  config_data type of data to obtain

  @Returns
  void * ptr of the GSM configuration ptr.

*/
void *rfc_gsm_mode_config_get (rfm_device_enum_type rfm_dev, rfc_mode_config_data_enum_type config_data)
{
  void *data_config_ptr = NULL;
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return (data_config_ptr);
  }

  data_config_ptr = rfc_obj->get_mode_config_data(rfm_dev, config_data);

  return (data_config_ptr);
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Disbable Tuners during X2G iRAT case

  @details
   Disbable Tuners during X2G iRAT case

  @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
   buffer: cleanup script buffer
   band: Disbale the tuner on band

*/

void rfc_gsm_disable_rffe( rfm_device_enum_type rfm_dev, rf_buffer_intf *buffer, rfcom_gsm_band_type band )
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->disable_rffe(rfm_dev, buffer, band);

  return;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides generic get and set functionality for the RFC specific
  parameters.

  @details
  It provides IOCTL kind of interface to get and set RFC specific
  parameters.

  @param rfm_dev : Logical Path for which RFC signals to be configured.
  @param cmd  : Specifies the command to performed by the device.
  @param data : Input and/or Output parameter for the command.

  @retval ret_val : generic return value which depends on the 'cmd' parameter.

*/
int32 rfc_gsm_command_dispatch( rfm_device_enum_type rfm_dev, int32 cmd, void *data )
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return 0;
  }

  rfc_obj->command_dispatch(rfm_dev, cmd, data);

  return 1;
}


/* ----------------------------------------------------------------------- */
/*!
  @brief
  Sets the specified path to the RF Rx band.

  @details
  This function will call the card specific version of the set Tx band API.
  This function is called every RX burst.

  @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
  band RF band
  asm_start buffer, asm_stop_buffer
  atuner_start_buffer, atuner_stop_buffer
  grfc_start_buffer, grfc_stop_buffer
  arfcn

*/
void rfc_gsm_rx_burst_config( rfm_device_enum_type rfm_dev,
                              rfcom_gsm_band_type band,
                              uint16 arfcn,
                              uint8 alt_path)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->rx_burst_config( rfm_dev, 
                            band,
                            arfcn,
                            alt_path);

  return;
} /* rfc_gsm_rx_burst_config() */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns config structure containing nv container and modem chain
   also returns true if the rfm_dev/band/rx/tx is supported.

   @details
   This function returns config structure containing nv container and modem chain
   also returns true if the rfm_dev/band/rx/tx is supported.

   @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
   rfcom_gsm_band_type  GSM band for which RFC info requested.
   rfc_rxtx_enum_type rx config or tx config
   rfc_cfg_info_type data to obtain

   @Returns
   boolean : TRUE if the combination is supported
   cfg_struct: for information on the chain

*/

boolean rfc_gsm_get_cfg_info( rfm_device_enum_type rfm_dev,  rfcom_gsm_band_type rfgsm_band, rfc_rxtx_enum_type rx_tx, rfc_cfg_info_type *cfg_struct, uint32 alt_path )
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return FALSE;
  }

  return(rfc_obj->get_cfg_info(rfm_dev, rfgsm_band, rx_tx, cfg_struct, alt_path));

} /* rfc_gsm_config_rx_band() */


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns top level ptr to the requested Rx band configuration.

   @details
   The function will return Rx configuration such as GPIO/GRFC settings along
   with any other parameter data structures for a particular band.  The void*
   allows for casting for usage under various band types.

   @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
   rfcom_gsm_band_type  GSM band for which RFC info requested.
   rfc_band_config_data_enum_type type of data to obtain

   @Returns
   void * ptr of the current Rx band configuration ptr.

*/
void *rfc_gsm_rx_band_config_get(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                                 rfc_band_config_data_enum_type config_data)
{
    void *data_config_ptr = NULL;
    rfc_gsm *rfc_obj = rfc_gsm::get_instance();

    if ( rfc_obj == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
      return (data_config_ptr);
    }

    data_config_ptr = rfc_obj->rx_band_config_get(rfm_dev, band, config_data);

    return (data_config_ptr);
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Sets the specified path to the RF Tx band.

  @details
  This function will call the card specific version of the set Tx band API. 

  @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
  band RF band

*/
void rfc_gsm_tx_band_config( rfm_device_enum_type rfm_dev, 
							               rfcom_gsm_band_type band,
                             rf_buffer_intf  *tx_band_script)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->tx_band_config(rfm_dev, band, tx_band_script);

  return;
} /* rfc_gsm_config_tx_band() */

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This API obtains the FE device settings needed for every TX burst

  @details
  It retrieves settings for different FE devices like PA, ASM, QPOET and Atuner including any GRFCS.
  The settings are retrived into the dedicated tx buffer foe each of above device(stored in rfgsm_core_handles)
  based on rfm_device, band and arfcn.
 
  @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
   band RF band
   arfcn

*/
void rfc_gsm_tx_burst_config( rfm_device_enum_type rfm_dev,
                              rfcom_gsm_band_type band,
                              uint16 arfcn )
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->tx_burst_config( rfm_dev,
                               band,
                               arfcn);

  return;
} /* rfc_gsm_tx_burst_config() */


#ifdef FEATURE_RF_ASDIV
/* ----------------------------------------------------------------------- */
/*!
  @brief
  This API appends the ASD GRFCs

  @details
  It populates the GRFC depending upon the antenna Position required to be set
  to
 
  @parameter
   command: Antenna position to be set to
   start_buffer: buffer pointer where the GRFC has to be populated to

*/

boolean rfc_gsm_append_txd_grfc( uint32 command, rf_buffer_intf *start_buffer, void* cb_data )
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return FALSE;
  }

  (rfc_obj->append_txd_grfc(command, start_buffer, cb_data));

  return TRUE;
} 

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This API appends the ASD RFFEs

  @details
  It populates the RFFE depending upon the antenna Position required to be set
  to
 
  @parameter
   command: Antenna position to be set to
   start_buffer: buffer pointer where the RFFE has to be populated to

*/

boolean rfc_gsm_append_asdiv_rffe( uint32 command, rf_buffer_intf *start_buffer, void* cb_data )
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"Append_asdiv_rffe: GSM RFC Obj is NOT created. ",0);
    return FALSE;
  }

  (rfc_obj->append_asdiv_rffe(command, start_buffer, cb_data));

  return TRUE;
} 

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This API appends the ASD writes (rffe or grfc)

  @details
  It populates the writes depending upon the antenna Position required to be set
  to. It will either populate the GRFC or RFFE scripts depending on the XSW HW
 
  @parameter
   command: Antenna position to be set to
   start_buffer: buffer pointer where the RFFE has to be populated to

*/

boolean rfc_gsm_append_asdiv( uint32 command, rf_buffer_intf *start_buffer, void* cb_data )
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rfc_gsm_append_asdiv: GSM RFC Obj is NOT created. ",0);
    return FALSE;
  }

  /* Only one of these calls will populate (append) the buffer with appropriate writes:
     - 3rd party XSW uses grfc => append_asdiv_grfc will populate the script buffer
     - QFE XSW uses rffe => append_asdiv_rffe will populate the script buffer 
  */
  if(!(rfc_obj->append_txd_grfc(command, start_buffer, cb_data)))
  {
    (rfc_obj->append_asdiv_rffe(command, start_buffer, cb_data));
  }

  return TRUE;
} 

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This API Gets the signal type's timing info

  @details
  This API Gets the signal type's timing info
 
  @parameter
   rfm_dev : GSM devive id
   rfgsm_band : GSM band 
   sig_info : timing info is required for which signal type
   start_delta : Timing info is stored in this

*/

boolean rfc_gsm_get_timing_info(  rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfgsm_band, rfdevice_type_enum_type dev_type, rfc_signal_type sig_info, int16 *start_delta )
{
  
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();
  
   if ( rfc_obj == NULL )
   {
	 MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
	 return FALSE;
   }

   (rfc_obj->get_timing_info(rfm_dev, rfgsm_band, dev_type, sig_info, start_delta));

   return TRUE;
}

#endif
/* ----------------------------------------------------------------------- */


/*!
  @brief
  Configure the rf-card for the specified Tx gain.

  @details
  Configure the rf-card for the specified Tx gain.

  @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
   band : RF band
   mod_type : Modulation type, GMSK or 8PSK
   pwr_index : Tx power index
   pa_range : PA Gain state
   tx_trans_buffer : Transition buffer to update the gain information.

*/
void rfc_gsm_tx_gain_config( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                             rfgsm_modulation_type mod_type, uint8 slot_num, int32 pwr_in_dbm, 
                             uint8 pa_range, rf_buffer_intf *tx_trans_buffer, boolean bias_flag, 
                             uint16 bias , uint8 apt_mode, boolean override_start_time)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->tx_gain_config(rfm_dev, band, mod_type, slot_num, 
                          pwr_in_dbm, pa_range, tx_trans_buffer, 
                          bias_flag, bias, apt_mode, override_start_time);

  return;
}

/* ----------------------------------------------------------------------- */
/*!

  @brief
   Retrieve the rfc information from the rf card and append the RF_ON disable/enable
   transactions for the rf-card configuration to the transition script

  @details
   RF_ON can be either ssbi or grfc controlled. If RF_ON GRFC exists in RFC AG,
   use GRFC, otherwise use ssbi control
 
  @todo
   This function will be used to populate transactions for a dedicated RF_OFF_ON_TRANSITION_EVENT

  @parameter
  rfm_dev : Logical Path for which RFC signals to be configured.
   tx_trans_buffer : Transition buffer to update the rf_on information

*/
void rfc_gsm_append_rf_off_on_entries( rfm_device_enum_type rfm_dev, rf_buffer_intf *tx_trans_buffer )
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->append_rf_off_on_entries(rfm_dev, tx_trans_buffer);

  return;
}

/* ----------------------------------------------------------------------- */

/*!

  @brief
   Retrieve the rf_on control information from the rf card
 
  @details
   RF_ON can be either ssbi or grfc controlled. If RF_ON GRFC exists in RFC AG,
   return TRUE, otherwise return FALSE
 
  @parameter
  rfm_dev : Logical Path for which RFC signals to be configured.
   tx_trans_buffer : Transition buffer to update the rf_on information

*/

boolean rfc_gsm_rf_on_grfc_ctl( rfm_device_enum_type rfm_dev )
{
  boolean ret_val = FALSE;
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return ret_val;
  }

  ret_val = rfc_obj->rf_on_grfc_ctl( rfm_dev );

  return ret_val;
}
/* ----------------------------------------------------------------------- */
/*!
   @brief
  Configure the rf-card for the specified PA Range.

  @details
  Configure the rf-card for the specified PA Range.

  @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
   band : RF band
   mod_type : Modulation type, GMSK or 8PSK
   pa_range : PA Gain state
   tx_trans_buffer : Transition buffer to update the gain information.

*/
void rfc_gsm_tx_gain_override( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                               rfgsm_modulation_type mod_type, uint8 pa_range, 
                               rf_buffer_intf *tx_trans_buffer, boolean bias_flag, 
                               uint16 bias , uint8 apt_mode, boolean override_start_time)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->tx_gain_override(rfm_dev, band, mod_type, pa_range, tx_trans_buffer, bias_flag, bias, apt_mode, override_start_time);

  return;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns top level ptr to the requested Tx band configuration.

   @details
   The function will return Tx configuration such as GPIO/GRFC settings along
   with any other parameter data structures for a particular band.  The void*
   allows for casting for usage under various band types.

   @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
   rfcom_gsm_band_type  GSM band for which RFC info requested.
   rfc_band_config_data_enum_type type of data to obtain

   @Returns
   void * ptr of the current tx band configuration ptr.

*/
void *rfc_gsm_tx_band_config_get(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                                 rfc_band_config_data_enum_type config_data)
{
    void *data_config_ptr = NULL;
    rfc_gsm *rfc_obj = rfc_gsm::get_instance();

    if ( rfc_obj == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
      return (data_config_ptr);
    }

    data_config_ptr = rfc_obj->tx_band_config_get(rfm_dev, band, config_data);

    return (data_config_ptr);
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the VREGs for GSM mode of operation.

   @details
   This function configures the VREGs for GSM mode of operation.

   @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.
   band:  GSM band for which VREGS to be configures.
   state : GSM mode of operation.

   @Returns None
*/
void rfc_gsm_manage_vregs ( rf_path_enum_type path, rfcom_gsm_band_type band, rfm_path_state state, uint8 sub_id )
{
  rfm_mode_enum_type mode[RFC_GSM_MAX_TECH_SUBSCRIPTIONS] = {RFM_EGSM_MODE, RFM_EGSM_MODE_2, RFM_EGSM_MODE_3};
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }
  
  /* Validate the subscription ID */
  if( sub_id >= RFC_GSM_MAX_TECH_SUBSCRIPTIONS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rfc_gsm_manage_vregs sub ID %d invalid",sub_id);
    return;
  }

#ifndef T_RUMI_EMULATION
  if( state == RF_PATH_SLEEP_STATE )
  {
    /* disable the rf signals, put the signals into low power mode */
    rfc_common_sleep(path, mode[sub_id]);
  }
  else if ( state == RF_PATH_RX_STATE)
  {
    /* enable the rf signals, if the signals are in low power mode 
        during Rx state */
    rfc_common_wakeup(path, mode[sub_id]);
  }

  rfc_obj->manage_vregs(path, band, state, mode[sub_id]);
#endif //T_RUMI_EMULATION
  
  return;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures RFC Signals into sleep state.

   @details
   This function configures RFC Signals into sleep state.

   @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.

   @Returns None
*/
void rfc_gsm_sleep(rfm_device_enum_type rfm_dev)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }
  rfc_obj->sleep(rfm_dev);

  return;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures RFC Signals into wakeup state.

   @details
   This function configures RFC Signals into wakeup state.

   @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.

   @Returns None
*/
void rfc_gsm_wakeup(rfm_device_enum_type rfm_dev,uint8 alt_path)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }
  rfc_obj->wakeup(rfm_dev, alt_path);

  return;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures RFC Signals into tx disable state.

   @details
   This function configures RFC Signals into tx disable state.

   @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.

   @Returns None
*/
void rfc_gsm_tx_disable(rfm_device_enum_type rfm_dev)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }
  rfc_obj->tx_disable(rfm_dev);

  return;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures RFC Signals into tx enable state.

   @details
   This function configures RFC Signals into tx enable state.

   @parameter
   rfm_dev : Logical Path for which RFC signals to be configured.

   @Returns None
*/
void rfc_gsm_tx_enable(rfm_device_enum_type rfm_dev)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }
  rfc_obj->tx_enable(rfm_dev);

  return;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function constructs the GRFC scripts.

  @details
  This function constructs the GRFC scripts.

  @param None.
*/
void rfc_gsm_tx_timing_adjust(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return;
  }
  rfc_obj->tx_timing_adjust(rfm_dev, band);

  return;
}

/*!
  @brief
  This function gets the device information based on a particular band, logical device and RX/TX.

  @details
  This function gets the device information based on a particular band, logical device and RX/TX.

  @param rfc_cfg_params_type *,rfc_device_info_type **
*/

boolean rfc_gsm_data_get_device_info(rfc_cfg_params_type *cfg, rfc_device_info_type **device_info_pptr)
{
   rfc_gsm_data *gsm_data_ptr = rfc_gsm_data::get_instance();
    
   if(cfg == NULL)
   {
     return FALSE;
   }

   if(device_info_pptr == NULL)
   {
     return FALSE;
   }
 
   if( gsm_data_ptr->devices_cfg_data_get(cfg,device_info_pptr) == FALSE)
   {
     return FALSE;
   }
   
   return TRUE;
}

/*!
  @brief
  This function finds if multislot PAPM control is supported based on a particular band and logical device.

  @details
  This function finds if multislot PAPM control is supported based on a particular band and logical device.

*/
boolean rfc_gsm_get_papm_multislot_ctl_support(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band)
{
  boolean ret_val = FALSE;
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return ret_val;
  }

  ret_val = rfc_obj->is_papm_multislot_ctl_supported(rfm_dev, band);

  return ret_val;
}

/*!
  @brief
  This function gets the RF device object pointer  based on a particular band, logical device, RX/TX and device type.

  @details
  This function gets the RF device object pointer  based on a particular band, logical device, RX/TX and device type.

*/


void *rfc_gsm_get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                 rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type, uint32 instance)
{
    rfc_gsm *rfc_obj = rfc_gsm::get_instance();
    if ( rfc_obj == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rfc_gsm_get_rf_device_obj(): GSM RFC Obj is NOT created. ",0);
      return NULL;
    }

    return( (void*)rfc_obj->get_rf_device_obj(rfm_dev,rx_tx,band,dev_type, instance));

}

/*!
  @brief
  This function gets the RF device list  based on a particular band, logical device, RX/TX and device type.

  @details
  This function gets the RF device list  based on a particular band, logical device, RX/TX and device type.

*/

void** rfc_gsm_get_rf_device_list(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                  rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type)
{
    rfc_gsm *rfc_obj = rfc_gsm::get_instance();
    if ( rfc_obj == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rfc_gsm_get_rf_device_list(): GSM RFC Obj is NOT created. ",0);
      return NULL;
    }

    return( (void **)rfc_obj->get_rf_device_list(rfm_dev, rx_tx, band, dev_type));

}

/*!
  @brief
  This function gets a supported band for a given logical device and RX/TX. 
  @details
  This is used to retrieve the appropriate physicl device
  pointer for a given band.

*/
rfcom_gsm_band_type rfc_gsm_get_supported_band(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx)
{
    rfc_gsm *rfc_obj = rfc_gsm::get_instance();
    if ( rfc_obj == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rfc_gsm_get_supported_band(): GSM RFC Obj is NOT created. ",0);
      return RFCOM_NUM_GSM_BANDS;
    }

    return( rfc_obj->get_supported_band(rfm_dev,rx_tx));

}

/*!
  @brief
  This function returns the pointer to the GSM device object, which specifies
  the physical device, based on a particular band, logical device,
  RX/TX and device type. 
 
  @details
  This function returns the pointer to the GSM device object, which specifies
  the physical device, based on a particular band, logical device, RX/TX and
  device type. 
*/

void *rfc_gsm_get_phy_device(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                 rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type)
{
    rfc_device_info_type *device_info_ptr = NULL;
    rfc_gsm_data *rfc_data = NULL; 
    uint32 instance = 0;
    rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, rx_tx, band, RFC_REQ_DEFAULT_GET_DATA};

    cfg.alternate_path = 0;

    rfc_data = rfc_gsm_data::get_instance(); 
    if (rfc_data == NULL)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
      return NULL;
    }

    rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
    if (device_info_ptr == NULL)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"device_info_ptr is NULL for rfm_dev %d", rfm_dev);
      return NULL;
    }


    /*loop through all instances of device types*/

    for (instance = 0; instance< RFC_MAX_DEVICE_INSTANCES_PER_BAND;instance++)
    {
      return (void *)rfc_gsm_get_rf_device_obj(rfm_dev,rx_tx,band,dev_type, instance);
    }

    return NULL;
}

/*!
  @brief
  This function returns the pointer to the C++ RX or TX adapter object
  containing the physical device pointers for path0 and path1 based on a
  particular band, logical device, RX/TX and logical device type instance. 
 
  @details
  Each different pysical device instance will have a different corresponding
  RF Common Device pointer containing related interface function pointers
  and state variables.
 
  For a given physical device this function returns the pointer to
  path0 (PRX+TX) and path1(DRX only).
 
  This API also runs a check and auto-corrects the band passed in case it does
  not fetch any physical device pointer for the given logical device.
 
*/

void *rfc_gsm_get_phy_device_ptr(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                 rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type, uint32 instance)
{
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_gsm_data *rfc_data = NULL; 
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, rx_tx, band, RFC_REQ_DEFAULT_GET_DATA};

  cfg.alternate_path = 0;

  rfc_data = rfc_gsm_data::get_instance(); 
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm_get_phy_device_ptr(),RFC GSM Data Object is NOT Created", 0);
    return NULL;
  }

  // Always return NULL for TX on non-TX capable devices or RX on non-RX capable devices
  if( (rfgsm_core_get_device_tx_capability(rfm_dev) == FALSE && cfg.rx_tx == RFC_CONFIG_TX ) ||
      (rfgsm_core_get_device_rx_capability(rfm_dev) == FALSE && cfg.rx_tx == RFC_CONFIG_RX ) )
  {
    return NULL;
  }

  if ( band >= RFCOM_NUM_GSM_BANDS )
  {
    /*Band passed is not supported. Check supported band*/
    band = rfc_gsm_get_supported_band(rfm_dev,rx_tx);
    cfg.band = band;
  }

  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

  if( device_info_ptr == NULL )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,
          "rfc_gsm_get_phy_device_ptr(), logical device: %d has device info ptr NULL for band %d", 
          rfm_dev,band );

    return NULL;
  }
  
  return (void *)rfc_gsm_get_rf_device_obj(rfm_dev,rx_tx,band,dev_type, instance);
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API gets the information of the RFFE devices for GSM bands
  
  @params device
  RFM device 
   
  @params band 
  RFM GSM band 

  @params rffe_device_type 
  RFFE device type

  @params mfg_id 
  Passing in parameter to store manufacture id 

  @params prd_id 
  Passing in parameter to store product id 
  
  @params rev_id 
  Passing in parameter to store revision id 
   
  @return
  TRUE on success and FALSE on failure
 
*/
boolean rfc_gsm_get_rffe_device_info(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, rfdevice_type_enum_type rffe_device_type, 
                                                                                                                               uint16 *mfg_id, uint16 *prd_id,uint16 *rev_id)
{
    rfdevice_class *dev_obj_ptr = NULL;

	if(rffe_device_type == RFDEVICE_PA)
	{
		dev_obj_ptr = (rfdevice_class *)rfc_gsm_get_rf_device_obj(rfm_dev,RFC_CONFIG_TX,band,RFDEVICE_PA,0);
	}
	else if(rffe_device_type == RFDEVICE_ASM)
	{
		dev_obj_ptr = (rfdevice_class *)rfc_gsm_get_rf_device_obj(rfm_dev,RFC_CONFIG_RX,band,RFDEVICE_ASM,0);
	}
	else if(rffe_device_type == RFDEVICE_PAPM)
	{
		dev_obj_ptr = (rfdevice_class *)rfc_gsm_get_rf_device_obj(rfm_dev,RFC_CONFIG_TX,band,RFDEVICE_PAPM,0);
	}

    if ( dev_obj_ptr != NULL )
    {
		if( FALSE == dev_obj_ptr->get_device_info(mfg_id, prd_id, rev_id))
		{
			MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
			return FALSE;
		}		
	}
	else
	{
		MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_gsm_get_rffe_device_info(), Null device!",0);
		return FALSE;
	}

	return TRUE;	
}


boolean rfc_gsm_populate_spur_channel_list()
{

  // create instance
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();


  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return FALSE;
  }

  if(rfc_obj->populate_spur_chan_list()!= TRUE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"Populate spur channel list failed. ",0);
    return FALSE;
  }

  return TRUE;



}


/* Wrapper function to get the Rf path for the corresponding logical device, band and instance
   RF path 0 = PRx + TX, RF path 1 = DRx*/

uint32 rfc_gsm_get_rf_path(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx, rfcom_gsm_band_type band, uint32 instance)
{
    rfc_gsm *rfc_obj = rfc_gsm::get_instance();
    if ( rfc_obj == NULL )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rfc_gsm_get_rf_path(): GSM RFC Obj is NOT created. ",0);
      return(0xFFFF);
    }

    return( rfc_obj->get_rf_path(rfm_dev, rx_tx, band, instance));

}


/*!
  @brief
  This API updates the rx timing parameters such as number of ssbis before pll, bumber of ssbis from the pll and RX burst processing time from the pll
  
*/

boolean rfc_gsm_update_rx_timing_params(uint32 rx_burst_time, 
										uint16 num_sbi_before_pll, 
										uint16 num_sbi_from_pll,
                                        uint16 rx_pll_settling_time_qs,
                                        rfm_device_enum_type rfm_dev)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"rfc_gsm_update_rx_timing_params(): GSM RFC Obj is NOT created. ",0);
    return FALSE;
  }

  return(rfc_obj->update_rx_timing_params( rx_burst_time,
										   num_sbi_before_pll,
										   num_sbi_from_pll,
                                           rx_pll_settling_time_qs,
                                           rfm_dev));

}

/*===========================================================================*/
/*! @fn rfc_gsm_set_device_capability                                            */
/*! @brief This function queries and set the RX and TX capability of a RFM device for a given GSM Band*/
/*! @details This function queries rfc_gsm_data and set the RX and TX capability of a RFM device for a given GSM Band*/
/*! @param rfm_dev - RF logical device to check                              */
/*! @retval boolean result - is PRx device or not                            */
boolean rfc_gsm_set_device_capability(rfm_device_enum_type rfm_dev,
                                      rf_path_enum_type rf_path )
{
  uint8 rfgsm_band;
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_gsm_data *rfc_data = NULL; 
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  rfc_cfg_params_type cfg = { rf_path, rfm_dev, RFC_CONFIG_TX, RFCOM_NUM_GSM_BANDS, RFC_REQ_DEFAULT_GET_DATA};

  rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfm_dev );
  RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr,"rfgsm_core_handle_ptr is NULL!", FALSE);

  rfc_data = rfc_gsm_data::get_instance(); 
  if (rfc_data == NULL)
  {
	MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GSM Data Object is NOT Created", 0);
	return FALSE;
  }

  cfg.alternate_path = 0;

 // Get the TX and RX support for each band
  for (rfgsm_band = (uint8)RFCOM_BAND_GSM850; rfgsm_band < (uint8)RFCOM_NUM_GSM_BANDS; rfgsm_band++)
  {
    cfg.band = (rfcom_gsm_band_type)rfgsm_band;
    cfg.rx_tx = RFC_CONFIG_TX;

    if( rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr ) == TRUE )
    {
      rfgsm_core_handle_ptr->device_capability.tx_supported_band_mask |= (1 << rfgsm_band);
    }

    cfg.rx_tx = RFC_CONFIG_RX;

    if( rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr ) == TRUE )
    {
      rfgsm_core_handle_ptr->device_capability.rx_supported_band_mask |= (1 << rfgsm_band);
    }  
  } 
  return TRUE; 
}



boolean rfc_band_split_cfg_data_get(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx, rfcom_gsm_band_type rfgsm_band, rfc_band_split_info_type **band_split_cfg_ptr)
{
   rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, rx_tx, rfgsm_band};
   rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance(); 

   cfg.alternate_path = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_band_split_cfg_data_get, RFC GSM Data Object is NOT Created", 0);
    return FALSE;
  }

  if (( rfc_data->band_split_cfg_data_get(&cfg,band_split_cfg_ptr )) != TRUE)
  {

    return FALSE;
  }

  return TRUE;

}



boolean rfc_get_split_band_type(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,rfcom_gsm_band_type rfgsm_band, uint16 arfcn, rfc_gsm_coex_band_info_type* final_split_band_info)
{

  rfc_band_split_info_type *band_split_cfg_ptr = NULL;
  uint8 i=0;
  boolean flag =  FALSE;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, rx_tx, rfgsm_band};
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance(); 

  cfg.alternate_path = 0;

  if ( final_split_band_info == NULL )
  {
    return flag;
  }
  

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_get_split_band_type, RFC GSM Data Object is NOT Created", 0);
    return flag;
  }

  if (( rfc_data->band_split_cfg_data_get(&cfg,&band_split_cfg_ptr )) != TRUE)
  {

    return flag;
  }

  final_split_band_info->band = rfgsm_band;

  while ( ( (uint32)(band_split_cfg_ptr->band_info[i].band) != RFC_INVALID_PARAM) && (i < RFCOM_NUM_GSM_BANDS) )
  {

    if((arfcn >= band_split_cfg_ptr->band_info[i].start_chan) && (arfcn <= band_split_cfg_ptr->band_info[i].stop_chan)
	&& (final_split_band_info->status == band_split_cfg_ptr->band_info[i].status))
    {
      final_split_band_info->band = (rfcom_gsm_band_type)band_split_cfg_ptr->band_info[i].band;
      flag = TRUE;
      break;
    }
    
    i++;
  }

  return flag;

}



rfcom_gsm_band_type rfc_get_unfiltered_band_type(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,rfcom_gsm_band_type rfgsm_band)
{

  rfc_band_split_info_type *band_split_cfg_ptr = NULL;
  uint8 i=0;
  rfc_cfg_params_type cfg = { RF_PATH_0, rfm_dev, rx_tx, rfgsm_band};
  rfc_gsm_data *rfc_data = rfc_gsm_data::get_instance(); 
  rfcom_gsm_band_type unfiltered_band = rfgsm_band;
  
  cfg.alternate_path = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_get_unfiltered_band_type, RFC GSM Data Object is NOT Created", 0);
    return unfiltered_band;
  }

  if (( rfc_data->band_split_cfg_data_get(&cfg,&band_split_cfg_ptr )) != TRUE)
  {

    return unfiltered_band;
  }


  while ( ( (uint32)(band_split_cfg_ptr->band_info[i].band) != RFC_INVALID_PARAM) && (i < RFCOM_NUM_GSM_BANDS) )
  {

    if(band_split_cfg_ptr->band_info[i].status == RFC_FILTERED_PATH_FALSE)
    {
      unfiltered_band = (rfcom_gsm_band_type)band_split_cfg_ptr->band_info[i].band;
      break;
    }
    
    i++;
  }

  return unfiltered_band;

}



rfdevice_id_enum_type rfc_gsm_get_device_id(rfm_device_enum_type rfm_dev,
                                            rfc_rxtx_enum_type rx_tx,
                                            rfcom_gsm_band_type band, 
                                            rfdevice_type_enum_type dev_type, 
                                            uint32 instance)
{

  rfdevice_trx_gsm_rx* rx_device = NULL;
  rfdevice_id_enum_type device_id = RFDEVICE_INVALID;

  rx_device = (rfdevice_trx_gsm_rx*)rfc_gsm_get_phy_device_ptr(rfm_dev, rx_tx, band, dev_type, instance);
  if (rx_device == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, " rx_device is NULL! ");
    return RFDEVICE_INVALID;
  }

  device_id = rx_device->rfdevice_id_get();
  if (device_id >= RFDEVICE_MAX_NUM)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, " Invalid device id %d ", device_id);
    return RFDEVICE_INVALID;
  }

  return device_id;

}


/*===========================================================================*/
/*! @fn rfc_gsm_ip2_cal_config                                            */
/*! @brief This function configures the RX/TX front end devices as needed for IP2 cal
 */


boolean rfc_gsm_ip2_cal_config( rfm_device_enum_type rfm_dev,
                              rfcom_gsm_band_type band,
                              uint16 arfcn,
                              void* setup_script_ptr,
                              void* cleanup_script_ptr)
{
  rfc_gsm *rfc_obj = rfc_gsm::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"GSM RFC Obj is NOT created. ",0);
    return FALSE;
  }

  rfc_obj->ip2_cal_device_config(rfm_dev, band, arfcn, setup_script_ptr, cleanup_script_ptr);

  return TRUE;
} /* rfc_gsm_rx_burst_config() */
