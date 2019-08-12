/*!
  @file
  rfcommon_core.c

  @brief
  This file contains all type declarations and definitions for the RFCOMMON
  Main Control layer.

  @details
  @todo anupamav Add GRFC functions too here. In present
  WCDMA baseline all are GPIOs but should migrate to GRFCs.

*/

/*=============================================================================
Copyright (c) 2008 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/14   zbz     MCS-RF interface changes for GSM and LTE/TDS coex support
08/15/14   kg      Framework to read RFC EEPROM and populate local structure.
06/13/14   kab     Add 3xCA support
02/05/14   st       DAC Vref Initialization - Mission Mode Setup
02/04/14   cd      Add Tx lin data initialization for memory savings
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
09/17/13   sb      Support for SGLTE/SGTDS coexistence
08/28/13   vs      Added rfcommon_core_path_to_receive_chain API 
08/16/13   ndb     Clean-up the SPI based antenna tuner support
07/31/13   vbh     Bypass DAL device calls in rfcommon_core_init for RUMI if HW ID undefined
05/06/13   rmb     Change ERR_FATAL to ERR_MSG in device_to_path() and device_to_cell_type() APIs. 
05/03/13   rmb     Added rfcommon_core_convert_rfm_device_to_cell_type() API.
05/01/13   rmb     Modified default return value as INVALID in
                   rfcommon_core_path_to_device() and rfcommon_core_device_to_path() APIs.
03/28/13   jtn     Support gps-only build.
03/11/13   sar     Updated file for APQ, GNSS only target.
11/27/12   aki     Added rfcommon_core_deinit
10/23/12   sr      Initialize the ant_tuner mutex.  
10/16/12   tsr     Cleanup GSM bringup flags  
10/11/12   sr      rfmeas_mdsp_init() api call to initialize the rf_buffer for IRAT,
09/19/12   sbm     Clean up un-used. rfcommon_core_set_tx_on(),
08/27/12   jfc     Rename rfcommon_msm_init_sbi to rfcommon_msm_init()
08/12/12   vbh     Modified the mapping between logical and physical path 
08/07/12   shb     Deleted rfdevice_interface.h
08/06/12   tsr     Changes for GSM RUMI bring-up
07/11/12   tsr     Changes for DIME GSM RUMI bring-up
06/17/12   pl      Update rfcommon interface to use device instead of path
04/19/12   jfc     Remove unused RFD includes
02/15/12   pv      RF Common Core Device Manager Implementation.  
02/14/12   shb     Moved rfcommon_msm_init_sbi() before rfc_set_card()
01/11/12   kg/sn   Initializing sbi tlmm after rfc init to aviod overwriting of gpio129(pa_on7) to gpio
11/23/11   swb     Map deprecated RFCOM_RECEIVER_DIV to device 1 (instead of DIV placeholder)
11/14/11   aro     Mainlined the debug flags
10/11/11   sty     Removed rf_common_init_critical_section() from 
                   rfcommon_core_init(). This is done in rfm_init()
09/27/11   aro     Added code to load internal device calibration data
09/23/11   aro     Removed old RC Tuner implementation
09/21/11   aro     Changed rfcommon_core_init() to return boolean
09/21/11   aro     Added new RC Tuner interface
08/10/11   tnt     Initial Ant Tuner support 
08/10/11   pl      Remove all references to legacy RFC structure and related API
07/13/11   sty     Deleted FEATURE_RUMI3_BRINGUP code
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
07/06/11   tnt     Remove polarity for GRFC engines setting
07/06/11   aro     Updated Lock interfaces to pass the Lock Data
06/23/11   pl      Remove LTE from GRFC/GPIO in common/core
06/08/11   tnt     Add rfcommon_core_gnd_pa_on()
06/07/11   shb     Removed one time RF device Mutex initialization call, it
                   will be done inside device when it is created as part of
                   static initialization
05/12/11   tnt     Configure RT for TX engine in rfcommon_core_config_band_gpio()
04/27/11   aro     Featurized GPIO Config for RUMI
04/25/11   sty     KW fixes
04/22/11   tnt     Add  rfcommon_core_get_rf_spi_id() to retrieve SPI ID from RFC
04/21/11   aro     Added Critical Section Init in Core init
04/19/11   sty     Featurized code around bypass_tlmm_for_rumi
04/18/11   xw      Deleted VCO coarse tune 
04/13/11   sty     Bypass DAL calls for RUMI bringup using debug flag
04/13/11   sty     RUMI3 bringup changes
03/23/11   sty     Deleted unused header file clkregim.h 
02/28/11   tnt     Merge to MDM9K PLF110
02/10/11   tnt     Move rfcommon_msm_init_sbi() from rfcommon_mc_init() to 
                                     rfcommon_core_init() so rf_tlmm_dal_handle is initialized before reference
02/10/11   pl      Remove Warnings
02/08/11   sty     Deleted rfcommon_custom_config_pa_temp
02/04/11   pl      Mainline RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE
02/03/11   pl      Move rfcommon_core_config_card_gpio() back to enter_mode()
                   GPIO config in boot up time is not possible due to IRC dsetup delay.
02/03/11   aro     Moved new Device interface to separate file
02/03/11   sty     Featurized rfcommon_custom_config_pa_temp
02/03/11   sty     Added rfcommon_custom_config_pa_temp()
01/27/11   dw      modify common core GRFC/GPIO functions to distribute tech dependent RFC calls 
01/31/11   sty     Added rfcommon_custom_pa_on2_config
01/24/11   whc     Gaurantee 250uS timing for SMPS_EN, disable at tx_disable
01/18/11   pl      Remove obsolete interfaces
01/17/11   pl      Move functionalities of rfcommon_core_config_card_gpio() to
                   constructor of rfc_intf class, retiring rfcommon_core_config_card_gpio()
01/13/11   pl      Use rfcom_lte_band_type instead of rf_card_band_type
01/10/11   sty     Added call to rfcommon_core_config_card_gpio for 
                   configuring the GPIO/GRFCs
01/05/11   pl      Integrate with RFC LTE re-arch, added mode parameters to interfaces
11/23/10   tnt     Remove F3 message when rx_rf_control ptr/tx_grfc_engine_ptr are NULL
                   These ptr are there for future expansion but not set for current target
10/20/10   vb      RFDevice Mutex intialization using the cmn_cmd_dispatch  
10/19/10   vb      Fixed compilation error 
10/19/10   vb      All device functions are now accessed thru device interface functions
09/29/10   tnt     Add #39 change back in
09/29/10   tnt     Remove #39 check-in for PLF85 release
09/28/10   pl      Remove GSM clock manipulation in GRFC configuration function
09/17/10   tnt     Add rfcommon_core_config_card_gpio()
09/13/10   shb     Replaced raw device struct function access with interface
09/03/10   tnt     Bug fix: remove tx grfc config in rfcommon_core_config_band_gpio()
08/22/10   tnt     Add rfcommon_core_set_gpio_based_on_tx_band()
07/29/10   aro     Common GRFC Design Implementation for 1xHDR
07/29/10    ka     Restore previous changes 
07/29/10    ka     Change WCDMA GRFC polarity to match other techs. 
07/28/10   aki     Re-applying Tim's GRFC, basically going back to #31 now that PLF is done 
07/28/10   aki     Reverting back to #29 + rfnv changes, Tim will apply GRFC on top of this  
07/28/10   aki     Removed call to rfcommon_rfnv_init will move to rfcommon_mc_init
07/10/10   tnt     RFC re-architecture to support GRFC
07/09/10   aki     Merging RFNV FTM changes from dev/rfnv_dev
07/01/10   av      Remove Intlock/Intfree
06/29/10   ka      Added GRFC support. 
06/08/10   tnt     KW fix
06/07/10   av      Lint fixes
05/19/10   tnt     Enable RC-tuner for LTE build only
03/24/10   ka      WPLT compilation fix.
03/19/10   lcl     Pull in fixes from QSC6295
03/12/10   kguo    Pulled in fixes from main\latest for rfcommon_core_set_grfc_based_on_rx_band 
                   and rfcommon_core_config_gpio 
03/03/10   Vish    Merged from prerel/poseidon2.1.x: 01/20/10 - 01/28/10
01/28/10   sar     Removed lint warnings.
01/26/10   sar     Lint fixes.
01/21/10   xw      Deleted extern rf1x_rxdevice
01/20/10   shb     Added call for RC Tuner calibration in rfcommon_core_init()
                   in C2K only builds
02/10/10   ka      Removed wait for frame tick.
02/01/10   sty     Removed array_size from extern-ed variable
01/28/10   mkv/pl  Merge mdm9k bring and main\latest
12/07/09   dyc     Featurize for C2K only builds.
11/24/09   sr      Changes needed to make use of only one instance of DAL
                   handle to TLMM
11/19/09   jfc     Lint Fix: Add include file; cast enum type to int
11/13/09   bmg     Removed unneeded (unused) SBI function prototypes.
11/03/09   pl      featurized rxdevice for WCDMA
10/25/09   dyc     Featurization for mode specific builds.
10/22/09   dyc     Support 1x only builds for VCO CT calls.
10/10/09   pl      Added FEATURE_LTE for featurization of rfcommon_vco_coarse_tune
10/07/09   mkv/pl  Check for GPIO_CONFIG_END instead of NULL
08/21/09   sar     Updated File for CMI conformance.
08/17/09   shb     Added support for Diversity in rfcom_to_path table
08/13/09   ckl     Added callback function initialization.
08/13/09   shb     Added RC Tuner Calibration to rfcommon_core_init()
08/12/09   xw      Added rfcommon_vco_coarse_tune().
06/05/09   vrb     Added coarse tune function call from commom_core_init()
03/13/09   sr      workaround for sbi include file issue and gpio_config() change
03/25/009  sr      check for rfc_core_config_ptr=NULL bfore using it.
03/18/009  sr      included rfcommon_core_types.h file
03/18/009  dyc     Updated rfcommon_sbi_info_type reference
03/13/09   sr      changes reflect sbi_info type and tlmn api signature changed
03/12/09   sar     Added changes needed for 1x
03/10/09   ckl     Fixed merge conflicts.
03/09/09   ckl     Code cleanup
01/30/09   av      Made rfcommon_core truly common wrt to configuring GPIOs
                   and added rfcommon_core_util_device_to_path_map()
10/14/08   av      Initial version of common core

============================================================================*/

#include "rfcom.h"
#include "msg.h"
#include "msm.h"
#include "rfcommon_core_error.h"
#include "rfcommon_core.h"
#include "rfcommon_core_cb.h"
#include "rfc_card.h"
#include "rfcommon_core_types.h"
#include "rfcommon_core_txlin.h"
#include "rfdevice_cmn_intf.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfcommon_msm.h"
#include "rfmeas_mdsp.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"
#endif
#include "rfcommon_locks.h"
#include "rfc_class.h"
#include "rfm_internal.h"
#include "rfcommon_core_device_manager.h"
#if defined( FEATURE_WCDMA )
#include "rfdevice_wcdma_intf.h"
#include "rfc_card_wcdma.h"
#endif
#if defined( FEATURE_CDMA1X )
#include "rfdevice_cdma_interface.h"
#endif
#if defined( FEATURE_LTE )
#include "rfc_card_lte.h"
#endif

#include "DDITlmm.h"
#include "DALSys.h"
#include "rfcommon_msg.h"
#include "rfcommon_cmd_processing.h"


/* function prototypes */
extern rf_hw_type rfcommon_nv_get_rf_config(void);

DalDeviceHandle * rf_tlmm_dal_handle = NULL;
static DALResult rf_tlmm_dal_handle_status;


static rf_common_coex_info_type rfcom_coex_filter_tbl = 
{
    FALSE,
    FALSE,
    {
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
        RFC_FILTERED_PATH_FALSE,
    }
};



rf_path_enum_type rfcom_to_path[RFCOM_MAX_DEVICES] =
{
    RF_PATH_0,    /* RFCOM_TRANSCEIVER_0    */
    RF_PATH_1,    /* RFCOM_RECEIVER_1,     */
    RF_PATH_1,    /* RFCOM_RECEIVER_2,   (treat same as path 1 */
    RF_PATH_1,    /* RFCOM_RECEIVER_3,   (treat same as path 1 */
    RF_PATH_1,    /* RFCOM_RECEIVER_DIV, (treat same as path 1 */
    RF_PATH_1     /* RFCOM_RECEIVER_DUAL (treat same as path 1 */
};

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

  @retval rfc_is_initialized: 0 if rfc_set_card reports failure
  (due to illegal RF card request), 1 if successful
*/
boolean rfcommon_core_init(rf_path_enum_type path)
{
  boolean rfc_is_initialized = TRUE;
  rf_hw_type rf_hw;
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
/* get the installed hw info from NV */
  rf_hw = (rf_hw_type)rfcommon_nv_get_rf_config();
#else
  rf_hw = RF_HW_WGR7640;
#endif

   rf_tlmm_dal_handle_status = DAL_DeviceAttach(DALDEVICEID_TLMM, &rf_tlmm_dal_handle);

   if(rf_tlmm_dal_handle_status == DAL_SUCCESS)
   {
     (void) DalDevice_Open(rf_tlmm_dal_handle,DAL_OPEN_SHARED);
   }
   else
   {
     MSG_FATAL("DAL Device failed to attach!!!", 0, 0, 0);
   }

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
/* Initialize MSM Block */
  rfcommon_msm_init();

 /* initialize the mdsp interface layer for IRAT. */
  rfmeas_mdsp_init();
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

  /* read the RFC EEPROM */
  rfc_read_eeprom();

  /* initialize the RFC data structures */
  rfc_is_initialized &= rfc_set_card(rf_hw);

  /* Enable VREGs in autocal mode BEFORE calling WTR1605 constructor so
  that WTR1605 can communicate with the chipset (say for process/revision
  detection) during initialization. This is a requirement of WTR1605
  device constructor to have VREGs and A0 buffer ON for SSBI communication
  with device. In addition to the VREGS, devices also need RFC_SSBI, RFC_RFFE GRFC's
  to be turned on.*/
  rfc_common_enable_autocal_vregs(TRUE);

  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  	 rxlm_init();
  #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  
  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  	 txlm_init();
  #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  
  #ifndef FEATURE_GNSS_ONLY_NO_WWAN
  rfcommon_msm_dac_mission_mode_enter();
  #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
  
  /*Disable vregs after device creation is completed.*/
  rfc_common_enable_autocal_vregs(FALSE);

 /* Initialize the Device Manager structures */
  rfc_is_initialized &= rfcommon_core_dev_mgr_init();

  rfc_is_initialized &= rfc_load_internal_device_calibration();

  /* Initializes the callback services */
  rfcommon_core_cb_init();

  /* Initialize tx linearizer config data */
  rfc_is_initialized &= rfcommon_core_txlin_init_data();

  return (int32)rfc_is_initialized;

} /* rfcommon_core_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deintialize 

*/
void rfcommon_core_deinit(void)
{
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  rfmeas_mdsp_deinit();
#endif
} /* rfcommon_core_deinit */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the scaled thermistor value from the internal or external
  thermistor as appropriate for the current target.

  @details
*/
void rfcommon_core_calibrate_therm( uint8 therm_min, uint8 therm_max )
{
  MSG_ERROR("rfcommon_core_calibrate_therm():  not implemented ", 0, 0, 0);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function handles rfm device to path mapping.

*/
rf_path_enum_type rfcommon_core_device_to_path( rfcom_device_enum_type device )
{
  rf_path_enum_type temp_path;
  /* Hard coded for bring-up. Needs to be fixed.*/

  switch (device)
  {
    case RFM_DEVICE_0:
    case RFM_DEVICE_2:
    case RFM_DEVICE_4:
      temp_path = RF_PATH_0;
      break;

    case RFM_DEVICE_1:
    case RFM_DEVICE_3:
    case RFM_DEVICE_5:
      temp_path = RF_PATH_1;
      break;

    default :
      RF_MSG_1(RF_ERROR, "Invalid device: no path found for Device: %d",device);
      temp_path = RF_PATH_1;
      break;
  }

  return temp_path;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function handles path to rfm device mapping.

*/
rfcom_device_enum_type rfcommon_core_path_to_device( rf_path_enum_type path )
{
  rfcom_device_enum_type temp_device;

  switch (path)
  {
    case RF_PATH_0:
      temp_device = RFM_DEVICE_0;
      break;

    case RF_PATH_1:
      temp_device = RFM_DEVICE_1;
      break;

    default :
      MSG_ERROR("invalid device: no path found.",0,0,0);
      temp_device = RFM_INVALID_DEVICE;
      break;
  }

  return temp_device;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function handles rfm device to cell type mapping.
 
  @details
  This function gives the cell type that is associated with the logical RFM device.

*/
rfcom_cell_type rfcommon_core_convert_rfm_device_to_cell_type( rfcom_device_enum_type device )
{
  rfcom_cell_type cell_type = RFCOM_INVALID_CELL;
  
  switch (device)
  {
    case RFM_DEVICE_0:
    case RFM_DEVICE_1:
      cell_type = RFCOM_PRIMARY_CELL;
      break;

    case RFM_DEVICE_2:
    case RFM_DEVICE_3:
      cell_type = RFCOM_SECONDARY_CELL;
      break;

    case RFM_DEVICE_4:
    case RFM_DEVICE_5:
      cell_type = RFCOM_SECONDARY_CELL;
      break;

    default :
      RF_MSG_1(RF_ERROR, "Invalid device: no cell found for Device: %d",device);
      break;
  }

  return cell_type;

}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function handles RF path to Receive chain mapping
 
  @details
   This function handles RF path to Receive chain mapping. Will be handled by RFC later.

*/

rfagc_receive_chain_type rfcommon_core_path_to_receive_chain(rf_path_enum_type path)
{
  rfagc_receive_chain_type temp_chain =  RFAGC_RECEIVE_CHAIN_0;

   switch (path)
   {
     case RF_PATH_0:
       temp_chain = RFAGC_RECEIVE_CHAIN_0;
       break;
  
     case RF_PATH_1:
       temp_chain = RFAGC_RECEIVE_CHAIN_1;
       break;
  
     default :
       MSG_ERROR("Invalid Path: no chain found.",0,0,0);
       break;
   }
  
   return temp_chain;

}


void rfcommon_core_set_filtered_path_status(boolean rx_flag, boolean tx_flag)
{
  rfcom_coex_filter_tbl.rx_filter_path_flag = rx_flag;
  rfcom_coex_filter_tbl.tx_filter_path_flag = tx_flag;    
  /* This is a temporary solution. Needs to be cleaned up*/
  rfcom_coex_filter_tbl.coex_tech_filter_status[RFM_EGSM_MODE] = rx_flag;
}

boolean rfcommon_core_get_rx_filter_status()
{
  return rfcom_coex_filter_tbl.rx_filter_path_flag;
}

boolean rfcommon_core_get_tx_filter_status()
{
  return rfcom_coex_filter_tbl.tx_filter_path_flag;
}


rfc_coex_filter_status_type rfcommon_core_get_coex_filter_status(rfm_mode_enum_type mode)
{
    if (mode > RFCOM_NUM_MODES)
    {
       return RFC_FILTERED_PATH_FALSE;
    }

    return rfcom_coex_filter_tbl.coex_tech_filter_status[mode];

}



/*---------------------------------------------------------------------------*/
/*
  @brief
  This API is called by MCS through common MSGR dispatcher when the indicator to use filtered path or unfiltered path needs to be updated. During
  SGLTE coexistence of GSM DCS with LTE B39 or TDS B34/39, the flag needs to be set else cleared.  
 
  @details
  1.  At bootup, the static flag maintained in RF driver specifying “use filtered path or not” is FALSE by default.
      MCS does not have to reset RFs state.  This means RF uses the regular path (bypassing special filter).
  2.	Device gets into a single tech call (L/W/G/T/C2K) or SGLTE call but not in the specific coex combination (GSM DCS + LTE B39 or TDS B34/39),
      MCS does not send anything to RF. RF should already be in proper state (Using regular filter path).
  3.	When G and T/L are in SGLTE call and in the coex band combination(GSM DCS + LTE B39 or TDS B34/39), MCS shall send RF indication with TRUE
      through MSGR. RF will update its static variable and switch to filtered path.
  4.	When G and T/L get out of coex combination or one or both call ends, MCS shall send RF indication with FALSE.
      RF will update its variable and switch to bypass path.
  5.	Again if step 2 happens, no indications sent from MCS.
 
  @params
  pointer to MSGR msg structure and response UMID
 
  @retval

*/



rfm_dispatch_outcome_type rfcommon_core_use_filtered_path( rfm_rf_cmn_bypass_filtered_path_req_s* rfm_msg_ptr, uint32 umid_cnf)
{


  rfm_rf_cmn_bypass_filtered_path_rsp_s rsp_msg;
  rfm_dispatch_outcome_type cmd_rsp = RFM_DISPATCH_SUCCESSFUL;
  errno_enum_type msgr_send_status = E_SUCCESS;

  boolean rsp_required = FALSE;
  boolean tx_filtered_path_flag = FALSE;
  boolean rx_filtered_path_flag = FALSE;

  rsp_msg.cmd_result  = RFM_DISPATCH_UNSUCCESSFUL;


  if (rfm_msg_ptr != NULL)
  {
    rsp_msg.hdr = rfm_msg_ptr->hdr;
    rsp_required = rfm_msg_ptr->rsp_required;
    rx_filtered_path_flag = rfm_msg_ptr->use_filtered_path_flag;
    tx_filtered_path_flag = rfm_msg_ptr->use_gsm_aggr_filter_path_flag;

    /* Update Static flag : GSM is victim*/
    rfcommon_core_set_filtered_path_status(rx_filtered_path_flag,tx_filtered_path_flag );
    rsp_msg.cmd_result = RFM_DISPATCH_SUCCESSFUL;
  }
  else
  {
    cmd_rsp = RFM_DISPATCH_NULL_PTR;
    return cmd_rsp;
  }


   RF_MSG_3(RF_MED, "rfcommon_core_use_filtered_path, RF received MSGR cmd , gsm rx filtered path status: %d, gsm tx filtered path status: %d, blocking response requested:%d",
           rfcommon_core_get_rx_filter_status(), 
           rfcommon_core_get_tx_filter_status(),
           rsp_required);

  if (rsp_required == TRUE)
  {

    msgr_init_hdr( &(rsp_msg.hdr), MSGR_RFA_RF_COMMON, umid_cnf );
    msgr_send_status = msgr_send( &(rsp_msg.hdr), sizeof(rfm_rf_cmn_bypass_filtered_path_rsp_s) );
    if (msgr_send_status != E_SUCCESS)
    {
      RF_MSG_1(RF_ERROR, "rfcommon_core_use_filtered_path, RF can't send RFA_RF_COMMON_BYPASS_FILTERED_PATH_RSP, %d",msgr_send_status);
      cmd_rsp = RFM_DISPATCH_UNSUCCESSFUL;
    }
    else
    {

      RF_MSG_1(RF_MED, "rfcommon_core_use_filtered_path, RF sent RFA_RF_COMMON_BYPASS_FILTERED_PATH_RSP successfully, %d",msgr_send_status);     
      cmd_rsp = RFM_DISPATCH_SUCCESSFUL;
    }

  }


  return cmd_rsp;

}
