/*!
   @file rfc_vreg_mgr_wtr1605_sv.c

   @brief


   @details
   This file implements the VREG Mgr for WTR1605_SV varients of rf-cards.

*/
/*===========================================================================
Copyright (c) 2012 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/vreg_mgr/wtr1605_sv/src/rfc_vreg_mgr_wtr1605_sv.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
05/10/16   vs      Include W+W feature changes
01/21/16   as      Add chip id for Feero Lite
11/09/15   vr      Moved the bolt changes under FEATURE
11/06/15   vr      Update PAM IDs based on Chip Family
                   Added PAM ID for MSM8937. 
10/13/15   par     Update PAM ID for 9x07 RF cards
09/23/15   par     Update PAM ID for MDM9x07
04/07/15   aak     Disable WTR4905 retention
03/02/15   aak     Enable WTR4905 retention
02/20/15   aak     Changes for retention for WTR4905
12/04/14   aak     Turn off MPP in sleep, instead of putting into sink mode
11/26/14   ry      Fix compiler error
11/26/14   ry      Changes for Tabasco in PAM table usage
11/25/14   ry      Changes to use RF1 PAM tables for WTR2955 RFCs and
                   RF2 PAM tables for WTR4905 RFCs
11/03/14   ry      Changes to use RF1 PAM tables to power up WTR2955
10/13/14   dr      Adding Featurization for Tabasco
10/04/14   bm      Removed references to NA2, CHILE_SAWLESS and JAP RFC IDs
09/18/14   vv      API support to detect rf sleep status
09/17/14   dr      Adding JOLOKIA featurization
09/11/14   aak     Remove featurization for MDM9x45
08/12/14   aak     Move PAM calls for LTE light sleep to the vreg manager
08/12/14   sb      Set XO drive strength based on the WTRs present in an RFC
08/08/14   aak     Use 1x PAM table as the common PAM table instead of GSM
07/28/14   aak     Set drive strength for A0/A1 clocks
07/24/14   aak     Shut off MPP for DAC Vref in CDRX LTE sleep
07/23/14   aak     Use PAM mode to turn on RFCLK1/RFCLK2 depending on number of
                   WTR devices present on the RF card
07/14/14   aak     Use same PAM tables for all RF techs
07/09/14   aak     Changes for API to control MPP for DAC Vref
07/08/14   jr      Changes for TSTS support
07/07/14   aak     Choose sync client for RPM sleep set based on RF PAM table used
07/07/14   aak     Choose RF PAM tables based on the chipset
07/07/14   aak     Add 4905-only Jap and China RF cards to use RF2 PAM tables
06/23/14   sty     Made LDO9 changes specific to BOLT/9x35 modem - not THOR
06/23/14   sty     Made LDO9 changes specific to 9x35 modem
06/11/14   krg     Featurize out changes to enable/disable 1V rail for 8994
05/30/14   sty     Changes to enable/disable 1V rail based on RF Clocks
05/29/14   aak     Use RF2 PAM tables for 4905-only RF cards
05/29/14   aak     Use GSM PAM table for DSDS on DPM2.0
05/22/14   aak     Use EVDO PAM table instead of WCDMA in Autocal NPA request
05/20/14   aak     Use GSM PAM table for DSDS mode
05/04/14   aak     Choose correct PAM table based on PMIC/Modem and RF card used
12/26/13   aak     Choose GPS PAM table depending on the RF card
12/18/13   aak     MPP for DAC Vref needs to be turned on during DAC Cal
10/24/13   aak     Remove featurizations
10/22/13   aak     Remove TDSCDMA featurization from autocal function
10/04/13   aak     Voltage scaling always ON for WTR3925
10/03/13   aak     Remove FEATURE_DIME_MODEM from vreg_mgr files
09/09/13   aak     Do not set XO drive strength for SGLTE RF cards
09/03/13   aak     No wait in RF SW for Immediate PAM request if a tech is already ON
08/03/13   aak     Changes for voting for resources for concurrent techs
07/26/13   aak     Shut off DAC MPP when all techs are off
07/22/13   aak     Don't set XO drive strength for WTR1605 SGLTE RF card
07/10/13   JJ      Remove lagacy flag rumi_bup_cdma 
07/03/13   aak     MPP for DAC Vref always ON for SV RF cards
06/24/13   aak     Voltage scaling based on both device processes for SV RF cards
06/21/13   vs      Featurize PM_MPP_DAC_VREF calls for 8926 
05/13/13   aak     Voltage scaling based on Tx SAW for LTE
05/07/13   aak     Replace RFM_EGSM_MODE_2 with RFM_NUM_MODES
05/02/13   aak     Use GSM_2 tech only for Triton
04/30/13   aak     Changes for second GSM client for DSDS/DSDA
04/12/13   aak     Featurize changes to include GSM_2 client only for Triton
04/12/13   aak     Create LTE PAM table only if FEATURE_LTE is defined
04/13/13   aak     Resolve KW error
04/13/13   aak     Change to use different PAM handles for second GSM in DSDA
03/25/13   aak     8x26/8x10 does not need a MPP for the DAC Vref
03/17/13   lp      Disable setting lowest XO drive strength for Triton
03/12/13   ms      Featurize TDSCDMA for linker issues
03/11/13   sar     Updated file for APQ, GNSS only target.
03/07/13  shb/aak Set lowest XO drive strength at bootup
03/06/13   gy     Featurized GNSS NPA registration 
03/05/13   vrb    Replacing RFC accessor param init_req boolean by req enum
01/25/13   aak    Enable sending sleep PAM request from RF to NPA
                  RF NPA required sync client to enable forming of the sleep set
12/19/12   aak    Resolve KW errors
12/15/12   aak    Backing out changes for sending the sleep PAM
12/12/12   aak    Enable sending sleep PAM request from RF to NPA
12/12/12   aak    RF NPA required sync client to enable forming of the sleep set
11/29/12   aak    Fix to resolve concurrency issue between GPS and WAN technologies
11/27/12   aak    Change to make NPA client handles NPA_CLIENT_SUPPRESSIBLE from NPA_CLIENT_REQUIRED
10/26/12   gvn    Remove gsm & lte bring-up flags which were included in merge
10/25/12   gvn    Featurize LTE to support LTE FW API removal on Triton
10/11/12   php    LTE RUMI flag clean up
09/27/12   aak    Adding 100us delay for vregs to settle before RFFE transactions in autocal  
08/28/12   aak    Enable voting with NPA for resources. Disable sending sleep request for now.  
08/24/12   aak    Fixes for sending NPA requests from RF SW on Dime 
08/23/12   vss    Move flags from rfc constructor to autocal_vregs API 
08/02/12   kb     Ported missing changes from CL 2585079 from nikel and comented out nikel specific code. 
07/23/12   aak    Initial check-in for Dime  
07/20/12   tks    Fix compile errors  
07/18/12   sr     new struct type to capture the signal info.
07/09/12   sr     Commented-out the device info query from RFC..
07/09/12   aak    Move Quiet mode API from device to RFC  
07/02/12   aak    Use same PAM request for LTE, 1x and DO on SV RF cards 
06/20/12   aak    Use non-SV PAM for SVVZ on 9x15 
06/04/11   aak    Change for schedule wake-up for TDSCDMA 
05/22/12   aak    Change for PAM sent for SVDO/SVLTE 
05/14/12   aak    Change for LTE and 1x to send a combined PAM to MCPM in SV-LTE mode
05/03/12   sb     Fixed KW warning
04/27/12   aak    Change the check for max PAM id for GPS  
04/26/12   aak    Initialise NPA client handles to 0 in the constructor  
04/20/12   aak    SVOC and SVSP to use non-SV PAM since only A0 clock is present 
04/19/12   aak    Changes to change to C++ interface  
04/02/11   sr     Initial revision.
============================================================================

                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfa_variation.h"
#include "msg.h"
#include "pm.h"
#include "npa.h"
#include "pmapp_npa.h"
#include "DALSys.h"
#include "mcpm_api.h"
#ifdef FEATURE_RF_HAS_NPA_SCHEDULED_REQUEST_API
#include "npa_scheduler.h"
#endif
#include "rfm_internal.h"
}

#include "rfc_vreg_mgr_wtr1605_sv.h" 
#include "rfcommon_nv_mm.h"
#include "rfdevice_lte_interface.h"
#include "rfc_common_data.h"
#include "DDIChipInfo.h"
#include "rfdevice_cmn_intf.h"


static rfm_mode_enum_type tech_resource_data[RFM_NUM_MODES]; 

static volatile boolean rfc_sv_npa_voting = TRUE;

//RF NPA sync client to enable forming of the sleep set
static npa_client_handle rf_sleep_set_sv_client_handle = NULL;

#if ((defined (FEATURE_BOLT_MODEM)) && (!defined(FEATURE_BOLT_PLUS_MODEM)))
  /* needed only for 9x35 -- but THOR and Bolt+ have BOLT_MODEM defined - so 
     need to look for is_BOLT_defined and THOR_no_defined */
  /* For LDO9/S5 */
static npa_client_handle rf_L9_S5_sv_client_handle = NULL;
#endif

//Count number of WTRs on the RF card
static uint8 num_trx = 0;
static boolean num_trx_once = FALSE;

//For VoLTE light sleep/wakeup
static npa_client_handle rf_light_sleep_lte_client_handle = NULL;

/*===========================================================================
                           Functions
===========================================================================*/
rfc_vreg_mgr * rfc_vreg_mgr_wtr1605_sv::get_instance()
{
  if (vreg_mgr_ptr == NULL)
  {
    vreg_mgr_ptr = (rfc_vreg_mgr *)new rfc_vreg_mgr_wtr1605_sv();
  }
  return(vreg_mgr_ptr);
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   constructor.

   @details

*/
rfc_vreg_mgr_wtr1605_sv::rfc_vreg_mgr_wtr1605_sv(void)
: rfc_vreg_mgr()
{
  uint8 tech = 0;

  //Wait in case needed after command send to NPA for vregs/clocks to settle. Set to 650us right now.
  vreg_por_wait = 650;

  /* initialize NPA ID for all techs and all voltage scaling  */
  for (tech = RFM_PARKED_MODE; tech < RFM_NUM_MODES; tech++ )
  {
    tech_resource_data[tech] = (rfm_mode_enum_type)tech;
    rf_wan_gps_npa_handle[tech] = 0;
    last_rf_wan_gps_npa_id[tech] = PMIC_NPA_MODE_ID_RF_SLEEP;
  }

  init_rf_npa_handle();
  init_chip_info();
  //init_npa_rxtx_table();
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   Destructor.

   @details

*/
rfc_vreg_mgr_wtr1605_sv::~rfc_vreg_mgr_wtr1605_sv()
{
  // Do nothing
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the VREG management for Self Cal

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::enable_autocal_vregs(boolean on_off)
{
   if (rfc_sv_npa_voting == FALSE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"RFC_NSV: No NPA voting for Autocal vregs. Rails need to be turned on "
                                      "in PMIC Init",0);
    return;
  }

   /* Find out number of WTRs on the RF card*/
   if (num_trx_once == FALSE) 
   {
      uint8 iter = 0;

      rfc_common_data *rfc_data = rfc_common_data::get_instance();
      rfc_logical_device_info_type *logical_devices_cfg = NULL;
      logical_devices_cfg = rfc_data->get_logical_device_cfg();

      if (logical_devices_cfg == NULL) 
      {
         ERR_FATAL( "logical_devices_cfgis NULL",0, 0, 0);
      }

      /* Find out number of RF transceiver devices on the RF card being used */
      for (iter = 0; logical_devices_cfg[iter].rf_device_id != RFDEVICE_INVALID; iter++)
      {
        if (logical_devices_cfg[iter].rf_device_type == RFDEVICE_TRANSCEIVER)
        {
           num_trx++;
        }
      }

      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Number of WTR devices on the RF card = %d",num_trx);

      if (num_trx == 0) 
      {
         //Force number of WTRs to 1
         num_trx = 1;
      }

      num_trx_once = TRUE;
   }


  if(!rfc_set_xo_drive_strength())
  {

    RF_MSG( RF_ERROR, "Set desired XO drive strength failed!");
  }

  init_npa_rxtx_table();

  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Entering rfc_vreg_mgr_wtr1605_sv::enable_autocal_vregs(%d)", on_off);

  if (on_off == TRUE)
  {
     npa_issue_required_request (rf_wan_gps_npa_handle[RFM_1XEVDO_MODE], 
                                 npa_rxtx_table[RFM_1XEVDO_MODE][RFC_VOLTAGE_SCALE_ON]); 
     MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Autocal PAM mode %d", npa_rxtx_table[RFM_1XEVDO_MODE][RFC_VOLTAGE_SCALE_ON]);

     DALSYS_BusyWait(vreg_por_wait);

     /* Extra delay to let vregs settle */
     DALSYS_BusyWait(100); 

     // Set MPP5 to Analog Output 1.25V ref since PAM does not set it
     pm_mpp_config_analog_output(PM_MPP_DAC_VREF, PM_MPP__AOUT__LEVEL_VREF_1p25_Volts, PM_MPP__AOUT__SWITCH_ON);

     MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Issuing PMIC_NPA_MODE_ID_RF_AUTOCAL in rfc_vreg_mgr_wtr1605_sv::enable_autocal_vregs()",0);
     last_rf_wan_gps_npa_id[RFM_1XEVDO_MODE] = npa_rxtx_table[RFM_1XEVDO_MODE][RFC_VOLTAGE_SCALE_ON];
  }
  else
  {
#if ((defined (FEATURE_BOLT_MODEM)) && (!defined(FEATURE_BOLT_PLUS_MODEM)))

    /* indicates if WTR power rail shutdown is supported */
    boolean rail_shutdown_supported; 

    /* hard-coding to instance-0 for now */ 
    rfdevice_rxtx_common_class* dev_ptr = rfc_common_get_cmn_device_object(0);
    /* check if WTR device support selective shutdown of 1v power rail */
    rfdevice_cmn_get_1v_rail_shutoff_support(dev_ptr, &rail_shutdown_supported);

    /* WTR supports 1v rail shutdown, so the _MODE_1 request will be sent in 
       auto-cal API -- follow RF clocks */
    if ( rail_shutdown_supported == TRUE )
    {
       if (num_trx == 1) 
       {
          //WTR2.0 or greater, Number of WTRs = 1
          npa_issue_required_request ( rf_L9_S5_sv_client_handle, PMIC_NPA_MODE_ID_RF_MODE_3 );
       }
       else
       {
          //WTR2.0 or greater, Number of WTRs = 2
          npa_issue_required_request ( rf_L9_S5_sv_client_handle, PMIC_NPA_MODE_ID_RF_MODE_1 );
       }
    }
    else
    {
       if (num_trx == 1) 
       {
          /* set to all-on mode by defualt. If WTR supports 1v rail shutdown, 
          the _MODE_1 request will be sent in auto-cal API - force enable */

          //WTR1.0, Number of WTRs = 1
          npa_issue_required_request ( rf_L9_S5_sv_client_handle, PMIC_NPA_MODE_ID_RF_MODE_4 );
       }
       else
       {
          //WTR1.0, Number of WTRs = 2
          npa_issue_required_request ( rf_L9_S5_sv_client_handle, PMIC_NPA_MODE_ID_RF_MODE_2 );
       }
    }
#endif

#if 0
    /* Check if retention is supported */
    if (num_trx == 1) 
    {
       boolean nv_retention_supported = FALSE;
       boolean wtr_retention_supported = FALSE;
       boolean retention_supported = FALSE;

       rfdevice_rxtx_common_class* dev_ptr = rfc_common_get_cmn_device_object(0);

       rfdevice_cmn_get_retention_supported(dev_ptr, &wtr_retention_supported);

       if (rfcommon_nv_mm_tbl.rf_pm_config != 1) 
       {
         nv_retention_supported = TRUE;
       }

       retention_supported = (nv_retention_supported && wtr_retention_supported);

       if (retention_supported) 
       {
          npa_issue_required_request(rf_sleep_set_sv_client_handle, PMIC_NPA_MODE_ID_RF_MODE_2);
          MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Retention enabled for WTR4905", 0);
       }

       rfdevice_cmn_set_retention_enabled(dev_ptr, retention_supported);
    }
#endif

     npa_issue_required_request (rf_wan_gps_npa_handle[RFM_1XEVDO_MODE], PMIC_NPA_MODE_ID_RF_SLEEP); 

     // Turn OFF MPP5
     pm_mpp_enable(0, PM_MPP_DAC_VREF, FALSE);

     MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Issuing PMIC_NPA_MODE_ID_RF_SLEEP in rfc_vreg_mgr_wtr1605_sv::enable_autocal_vregs()",0);
     last_rf_wan_gps_npa_id[RFM_1XEVDO_MODE] =  PMIC_NPA_MODE_ID_RF_SLEEP;
  }

  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Exiting rfc_vreg_mgr_wtr1605_sv::enable_autocal_vregs()",0,0);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::enable_mpp_dac_vref(boolean on_off)
{
   if (on_off == TRUE)
   {
      // Turn ON MPP for DAC Vref
      pm_mpp_config_analog_output(PM_MPP_DAC_VREF, PM_MPP__AOUT__LEVEL_VREF_1p25_Volts, PM_MPP__AOUT__SWITCH_ON);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"MPP for DAC Vref turned on",0);
   }
   else 
   {
      // Turn OFF MPP for DAC Vref
      pm_mpp_enable(0, PM_MPP_DAC_VREF, FALSE);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"MPP for DAC Vref turned off",0);
   }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::lte_light_sleep_vregs(void)
{
   npa_set_request_attribute( rf_light_sleep_lte_client_handle, NPA_REQUEST_FIRE_AND_FORGET);

#if ((defined (FEATURE_BOLT_MODEM)) && (!defined(FEATURE_BOLT_PLUS_MODEM)))

   if (num_trx == 1) 
   {
      npa_issue_required_request (rf_light_sleep_lte_client_handle, PMIC_NPA_MODE_ID_RF_MODE_2);
   }
   else
   {
      npa_issue_required_request (rf_light_sleep_lte_client_handle, PMIC_NPA_MODE_ID_RF_MODE_1);
   }
#else
   npa_issue_required_request (rf_light_sleep_lte_client_handle, PMIC_NPA_MODE_ID_RF_MODE_1);
#endif

   // Turn OFF MPP6
   pm_mpp_enable(0, PM_MPP_DAC_VREF, FALSE);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::lte_light_sleep_wakeup_vregs(void)
{
   npa_set_request_attribute( rf_light_sleep_lte_client_handle, NPA_REQUEST_FIRE_AND_FORGET);
   npa_issue_required_request (rf_light_sleep_lte_client_handle, 0);

   pm_mpp_config_analog_output(PM_MPP_DAC_VREF, PM_MPP__AOUT__LEVEL_VREF_1p25_Volts, PM_MPP__AOUT__SWITCH_ON);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the VREG management for Self Cal

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::manage_power(rfc_vreg_param &vreg_param)
{
  rfm_mode_enum_type mode = RFM_INVALID_MODE;
  rfm_path_state rf_state = RF_PATH_MAX_STATE;
  rfc_voltage_scaling_type vol_scale = RFC_VOLTAGE_SCALE_INVALID;
  uint8 npa_id = PMIC_NPA_MODE_ID_RF_SLEEP;

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfc_get_lock_data());

  mode = vreg_param.get_mode();
  //Check to see mode is valid
  if ((mode <  RFM_PARKED_MODE) || (mode >= RFM_INVALID_MODE))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid mode passed(mode=%d)hence return",mode);
    return;
  }
  MSG_5(MSG_SSID_RF, MSG_LEGACY_MED,"Enter WTR1605_SV: mode=%d, band=%d, chip_num=%d,path=%d, rf state=%d", 
        vreg_param.get_mode(), vreg_param.get_band(), vreg_param.get_rfchip_number(),
        vreg_param.get_path(), vreg_param.get_rf_state());

  rf_state = vreg_param.get_rf_state();

  if ((rf_state == RF_PATH_RX_STATE) || (rf_state == RF_PATH_RXTX_STATE))
  {
    vol_scale = get_voltage_scaling_state(vreg_param);
    npa_id = npa_rxtx_table[mode][vol_scale];
    if (npa_id == PMIC_NPA_MODE_ID_RF_SLEEP)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_vreg_mgr_wtr1605_sv::manage_power() trying to use un-initialized npa_id = %d",npa_id);
    }

    send_npa_rf_id(mode, npa_id);

    if (rf_state == RF_PATH_RXTX_STATE)
    {
      if (rfc_sv_npa_voting == TRUE)
      {
        // Set MPP5 to Analog Output 1.25V ref since PAM does not set it
        pm_mpp_config_analog_output(PM_MPP_DAC_VREF, PM_MPP__AOUT__LEVEL_VREF_1p25_Volts, PM_MPP__AOUT__SWITCH_ON);
      }
    }
  }
  else if (rf_state == RF_PATH_SLEEP_STATE) 
  {
    send_sleep_npa_id(vreg_param);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid state in rfc_vreg_mgr_wtr1605_sv::manage_power() %d",rf_state);
  }

  //Save info for future reference
  save_mode_info(vreg_param);

  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Exit WTR1605_SV",0);

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rfc_get_lock_data());
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the VREG management for Self Cal

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::manage_sv_power(rfc_vreg_param &vreg_param)
{
  // Not needed
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the VREG management for Self Cal

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::cdma_manage_power(rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for GSM mode

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::gsm_manage_power (rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for WCDMA mode

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::wcdma_manage_power(rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for TDSCDMA mode

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::tdscdma_manage_power (rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for GPS mode

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::gps_manage_power(rfc_vreg_param &vreg_param)
{
  rfm_path_state rf_state = RF_PATH_MAX_STATE;
  rfc_voltage_scaling_type vol_scale = RFC_VOLTAGE_SCALE_INVALID;
  uint8 npa_id = PMIC_NPA_MODE_ID_GPS_MODE_0;
  rfm_mode_enum_type mode = RFM_INVALID_MODE;

  mode = vreg_param.get_mode();
  //Check to see mode is valid
  if (( (mode <  RFM_PARKED_MODE) || (mode >= RFM_INVALID_MODE)))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid mode passed(mode=%d)hence return",mode);
    return;
  }

  rf_state = vreg_param.get_rf_state();

  MSG_5(MSG_SSID_RF, MSG_LEGACY_MED,"Enter WTR1605_SV GPS: mode=%d, band=%d, chip_num=%d,path=%d, rf state=%d", 
        vreg_param.get_mode(), vreg_param.get_band(), vreg_param.get_rfchip_number(),
        vreg_param.get_path(), vreg_param.get_rf_state());

  if (rf_state == RF_PATH_RX_STATE) 
  {
    vol_scale = get_voltage_scaling_state(vreg_param);
    npa_id = npa_rxtx_table[mode][vol_scale];
    send_npa_rf_gps_id(vreg_param.get_mode(), npa_id);
  }
  else
  {
    if (rf_state == RF_PATH_SLEEP_STATE) 
    {
      send_npa_rf_gps_id(vreg_param.get_mode(), PMIC_NPA_MODE_ID_GPS_MODE_0); 
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid state in rfc_vreg_mgr_wtr1605_sv::gps_manage_power() %d",rf_state);
    }

    //Save info for future reference
    save_mode_info(vreg_param);

    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Exit WTR1605_SV GPS",0);
  }

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific VREG management for LTE mode

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::lte_manage_power(rfc_vreg_param &vreg_param)
{
  manage_power(vreg_param);
}
/* ----------------------------------------------------------------------- */
/*!
   @brief
   

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::get_awake_resource_request(rfm_resource_info* params)
{
  uint8 npa_id = PMIC_NPA_MODE_ID_RF_SLEEP;
  rfc_voltage_scaling_type voltage_scaling = RFC_VOLTAGE_SCALE_ON;

  voltage_scaling = rfc_voltage_scaling_state[params->tech_A.mode];

  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Tech A mode = %d, state=%d",params->tech_A.mode, params->tech_A.path_state);

  switch (params->tech_A.mode) 
  {
    case RFCOM_WCDMA_MODE:
    case RFM_IMT_MODE_2:
    case RFCOM_GSM_MODE:
    case RFM_EGSM_MODE_2:
    case RFM_EGSM_MODE_3:
    case RFCOM_1X_MODE:
    case RFCOM_1XEVDO_MODE:
    case RFCOM_LTE_MODE:
    case RFM_TDSCDMA_MODE:
      if ((params->tech_A.path_state == RF_PATH_RX_STATE) || (params->tech_A.path_state == RF_PATH_RXTX_STATE))
      {
        npa_id = npa_rxtx_table[params->tech_A.mode][voltage_scaling];
      }
      else if (params->tech_A.path_state == RF_PATH_SLEEP_STATE) 
      {
        npa_id = PMIC_NPA_MODE_ID_RF_SLEEP;   
      }
      else
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid tech mode state in rfc_vreg_mgr_wtr1605_sv::get_awake_resource_request() mode=%d, state=%d",
              params->tech_A.mode, params->tech_A.path_state);
      }

      params->npa_id     = npa_id;
      params->npa_handle = rf_wan_gps_npa_handle[params->tech_A.mode];
      break;

    default:
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid tech mode in rfc_vreg_mgr_wtr1605_sv::get_awake_resource_request() mode=%d, state=%d",
              params->tech_A.mode, params->tech_A.path_state);
      break;
  }

  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Exiting rfc_vreg_mgr_wtr1605_sv::get_awake_resource_request() npa_id=%d, npa_handle=%d", params->npa_id, params->npa_handle);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::set_quiet_mode(rfm_mode_enum_type mode, boolean enable)
{
  // Quiet mode controlled via PAM
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   

   @details

*/
rfc_voltage_scaling_type rfc_vreg_mgr_wtr1605_sv::get_voltage_scaling_state(rfc_vreg_param &vreg_param)
{
  /* Voltage scaling always ON for WTR3925 */
  return (RFC_VOLTAGE_SCALE_ON);  
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::send_npa_rf_id(rfcom_mode_enum_type mode, uint8 npa_id)
{
  boolean mcpmnpa_status = MCPMNPA_STAT_NULL;

  if (rfc_sv_npa_voting == FALSE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"RFC_NSV: No NPA voting from RF. Rails need to be turned on in PMIC Init",0);
    /* no NPA support on RUMI */
    return;
  }

  if (npa_id >= PMIC_NPA_MODE_ID_RF_MODE_MAX) 
  {
    MSG_ERROR("Invalid WAN NPA id in rfc_vreg_mgr_wtr1605_sv::send_npa_rf_id() %d",npa_id,0,0);
    return;
  }

  if (npa_id == last_rf_wan_gps_npa_id[mode]) //same NPA id as the last one
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV: Same WAN NPA id as last sent - saved id=%d, new id=%d",
          last_rf_wan_gps_npa_id[mode],npa_id);
    return;
  }

  // Send NPA id only if different from last one

  // RF talks to MCPM instead of NPA
  mcpmnpa_status = MCPM_Send_RF_Req(mode, npa_id, rf_wan_gps_npa_handle[mode], 0, FALSE);
    
  if ((mcpmnpa_status != MCPMNPA_STAT_OPT_SUCCESS) && (mcpmnpa_status != MCPMNPA_STAT_OPT_IMM_REQ_SENT))
  {
    // RF sends PAM request to NPA
    npa_issue_required_request (rf_wan_gps_npa_handle[mode], npa_id);
    DALSYS_BusyWait(vreg_por_wait);
    print_npa_rf_id_msg(npa_id);
  }
  else if (mcpmnpa_status == MCPMNPA_STAT_OPT_IMM_REQ_SENT)
  {
    uint8 tech = 0;
    boolean wait_in_rfsw = TRUE;

    //Immediate request sent by MCPM. RF waits for 650us if not other techs are ON
    for (tech=0; tech<RFM_NUM_MODES; tech++) 
    {
       if (tech != mode) 
       {
          if (last_rf_wan_gps_npa_id[tech] != PMIC_NPA_MODE_ID_RF_SLEEP) 
          {
             wait_in_rfsw = FALSE;
             break;
          }
       }
    }

    if (wait_in_rfsw == TRUE) 
    {
       MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV: Wait in RF SW for Immediate Req - new tech=%d", mode, 0);
       DALSYS_BusyWait(vreg_por_wait);
    }
    else
    {
       MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV: No Wait in RF SW for Immediate Req - new tech=%d, old tech=%d", 
             mode,tech);
    }
  }
  else if (mcpmnpa_status == MCPMNPA_STAT_OPT_SUCCESS)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_NSV: RF PAM id %d not sent by MCPM to NPA", npa_id, 0);
  }
    
  last_rf_wan_gps_npa_id[mode] = npa_id;
    
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::send_npa_rf_gps_id(rfcom_mode_enum_type mode, uint8 npa_id)
{
  if (rfc_sv_npa_voting == FALSE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"RFC_NSV: No NPA voting from RF. Rails need to be turned on in PMIC Init",0);
    /* no NPA support on RUMI */
    return;
  }

  // GPS_MODE_9 enum is no longer defined in pmapp_npa.h
  if (npa_id >= PMIC_NPA_MODE_ID_GPS_MODE_MAX) 
  {
    MSG_ERROR("Invalid WAN NPA id in rfc_vreg_mgr_wtr1605_sv::send_npa_gps_id() %d",npa_id,0,0);
    return;
  }

  if (npa_id == last_rf_wan_gps_npa_id[mode]) //same NPA id as the last one
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV: Same GPS NPA id as last sent - saved id=%d, new id=%d",
          last_rf_wan_gps_npa_id[mode],npa_id);
    return;
  }

    // Send NPA id only if different from last one
  npa_issue_required_request (rf_wan_gps_npa_handle[mode], npa_id);
  if (npa_id != PMIC_NPA_MODE_ID_GPS_MODE_0)
  {
    DALSYS_BusyWait(vreg_por_wait);
  }
  print_npa_rf_gps_id_msg(npa_id);
  last_rf_wan_gps_npa_id[mode] = npa_id;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::send_sleep_npa_id(rfc_vreg_param &vreg_param)
{
  if (rfc_sv_npa_voting == FALSE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"RFC_NSV: No NPA voting from RF. Rails need to be turned on in PMIC Init",0);
    /* no NPA support on RUMI */
    return;
  }

#ifdef FEATURE_RF_HAS_NPA_SLEEP_ENABLED
  boolean mcpmnpa_status = MCPMNPA_STAT_NULL;
  boolean send_sleep_pam = FALSE;
  //rfc_rf_chip_number chip_num = vreg_param.get_rfchip_number();

  if (vreg_param.get_path() == RF_PATH_0)
  {
    if (rfc_state_info[vreg_param.get_mode()][RF_PATH_1] == RF_PATH_SLEEP_STATE) 
    {
       send_sleep_pam = TRUE;
    }
  }
  else // RF path 1
  {
    if (rfc_state_info[vreg_param.get_mode()][RF_PATH_0] == RF_PATH_SLEEP_STATE) 
    {
      send_sleep_pam = TRUE;
    }
  }

  if ((send_sleep_pam == TRUE) && (PMIC_NPA_MODE_ID_RF_SLEEP != last_rf_wan_gps_npa_id[vreg_param.get_mode()]))
  {
    // RF talks to MCPM instead of NPA
    mcpmnpa_status = MCPM_Send_RF_Req(vreg_param.get_mode(), PMIC_NPA_MODE_ID_RF_SLEEP, 
                                      rf_wan_gps_npa_handle[vreg_param.get_mode()], 0, TRUE);

    if ((mcpmnpa_status != MCPMNPA_STAT_OPT_SUCCESS) && (mcpmnpa_status != MCPMNPA_STAT_OPT_IMM_REQ_SENT))
    {
       npa_issue_required_request (rf_wan_gps_npa_handle[vreg_param.get_mode()], PMIC_NPA_MODE_ID_RF_SLEEP);
       MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV: sleep PAM sent for path %d", vreg_param.get_path(),0);
    }
    else
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV: sleep PAM sent by MCPM for path %d", vreg_param.get_path(),0);
    }

    last_rf_wan_gps_npa_id[vreg_param.get_mode()] = PMIC_NPA_MODE_ID_RF_SLEEP;
    save_mode_info(vreg_param);

   boolean shut_off_dac_mpp = TRUE;
   uint8 tech = 0;

   for (tech = RFM_PARKED_MODE; tech < RFM_NUM_MODES; tech++)
   {
      if (last_rf_wan_gps_npa_id[tech] != PMIC_NPA_MODE_ID_RF_SLEEP)
      {
         shut_off_dac_mpp = FALSE;
         MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV: Tech %d not in sleep, keep DAC MPP on", tech,0);
         break;
      }
   }
   if (shut_off_dac_mpp ==  TRUE)
   {
      // Turn OFF MPP5
      pm_mpp_enable(0, PM_MPP_DAC_VREF, FALSE);

      MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV: All techs in sleep, shut off DAC MPP", 0, 0);
   }

    print_npa_rf_id_msg(PMIC_NPA_MODE_ID_RF_SLEEP);
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV: sleep PAM not sent for path %d", vreg_param.get_path(),0);
  }
#endif
}


/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::save_mode_info(rfc_vreg_param &vreg_param)
{
   if (vreg_param.get_path() < 2) 
   {
      rfc_state_info[vreg_param.get_mode()][vreg_param.get_path()] = vreg_param.get_rf_state();

      MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV save device info: mode = %d, path=%d, state=%d", 
                           vreg_param.get_mode(), vreg_param.get_path(), vreg_param.get_rf_state());
   }
   else
   {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"WTR1605_SV - error: path > 2 %d", vreg_param.get_path() );
   }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::init_rf_npa_handle(void)
{
   uint8 tech = 0;
   DalDeviceHandle               * hChipinfo = NULL;
   DalChipInfoFamilyType           family;
   DALResult                       ret;
  
   if (rfc_sv_npa_voting == FALSE)
   {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"RFC_NSV: No NPA handles requested from RF. Rails need to be turned on "
                                      "in PMIC Init",0);
     /* no NPA support on RUMI */
     return;
   }
  
   ret = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipinfo);
   ASSERT(ret == DAL_SUCCESS && hChipinfo != NULL);
   DalChipInfo_GetChipFamily(hChipinfo, &family);
   
   if (family == DALCHIPINFO_FAMILY_MDM9x07 || family == DALCHIPINFO_FAMILY_MSM8937 || family == DALCHIPINFO_FAMILY_MSM8917 )
    {
      //One time set up a required sync client to have Sleep Set formed in the RPM
      rf_sleep_set_sv_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF1_TECH_1X, 
                                                              "rf_sv_sleep_config_client", NPA_CLIENT_REQUIRED);
      if (rf_sleep_set_sv_client_handle == NULL)
      {
        ERR_FATAL( "NULL client handle not created for rf_sleep_set_sv_client_handle ",0, 0, 0);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_handle for rf_sleep_set_sv_client_handle has been created!!",0);
        npa_issue_required_request(rf_sleep_set_sv_client_handle, PMIC_NPA_MODE_ID_RF_SLEEP);
      }

      /* Get NPA client for VoLTE light sleep/wakeup PAM table */
      rf_light_sleep_lte_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF1_TECH_LTE, 
                                                                "rf_light_sleep_lte_config_client", NPA_CLIENT_REQUIRED);
      if (rf_light_sleep_lte_client_handle == NULL)
      {
        ERR_FATAL( "NULL client handle not created for rf_light_sleep_lte_client_handle ",0, 0, 0);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_light_sleep_lte_client_handle has been created!!",0);
      }
    
      //Enable WTR2955 using RF_1 PAM tables
      rf_wan_gps_npa_handle[RFM_1X_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_1x", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_IMT_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_wcdma", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_IMT_MODE_2] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_wcdma2", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_1XEVDO_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_evdo", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_LTE_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_lte", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_TDSCDMA_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_tdscdma", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_EGSM_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm1", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_EGSM_MODE_2] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm2", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_EGSM_MODE_3] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm3", NPA_CLIENT_SUPPRESSIBLE);
    }
    if ( family == DALCHIPINFO_FAMILY_MSM8909 )
    {
      /* Use RF2 PAM tables for all the WTR4905 RFCs on JO/TA*/
      //One time set up a required sync client to have Sleep Set formed in the RPM
      rf_sleep_set_sv_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF2_TECH_1X, 
                                                              "rf_sv_sleep_config_client", NPA_CLIENT_REQUIRED);
      if (rf_sleep_set_sv_client_handle == NULL)
      {
        ERR_FATAL( "NULL client handle not created for rf_sleep_set_sv_client_handle ",0, 0, 0);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_handle for rf_sleep_set_sv_client_handle has been created!!",0);
        npa_issue_required_request(rf_sleep_set_sv_client_handle, PMIC_NPA_MODE_ID_RF_SLEEP);
      }

      /* Get NPA client for VoLTE light sleep/wakeup PAM table */
      rf_light_sleep_lte_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF2_TECH_LTE, 
                                                                 "rf_light_sleep_lte_config_client", NPA_CLIENT_REQUIRED);
      if (rf_light_sleep_lte_client_handle == NULL)
      {
        ERR_FATAL( "NULL client handle not created for rf_light_sleep_lte_client_handle ",0, 0, 0);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_light_sleep_lte_client_handle has been created!!",0);
      }

      // Enable WTR4905 using the RF_2 PAM tables
      rf_wan_gps_npa_handle[RFM_1X_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF2_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_1x", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_IMT_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF2_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_wcdma", NPA_CLIENT_SUPPRESSIBLE);
     rf_wan_gps_npa_handle[RFM_IMT_MODE_2] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF2_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_wcdma2", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_1XEVDO_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF2_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_evdo", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_LTE_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF2_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_lte", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_TDSCDMA_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF2_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_tdscdma", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_EGSM_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF2_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm1", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_EGSM_MODE_2] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF2_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm2", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_EGSM_MODE_3] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF2_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm3", NPA_CLIENT_SUPPRESSIBLE);
    }   

    if (family == DALCHIPINFO_FAMILY_MSM8952 || family == DALCHIPINFO_FAMILY_MSM8976 )
    {
    /* Use RF1 PAM tables for all the WTR2955 RFCs on JO/TA */
    //One time set up a required sync client to have Sleep Set formed in the RPM
      rf_sleep_set_sv_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF1_TECH_1X, 
                                                            "rf_sv_sleep_config_client", NPA_CLIENT_REQUIRED);
    if (rf_sleep_set_sv_client_handle == NULL)
    {
      ERR_FATAL( "NULL client handle not created for rf_sleep_set_sv_client_handle ",0, 0, 0);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_handle for rf_sleep_set_sv_client_handle has been created!!",0);
      npa_issue_required_request(rf_sleep_set_sv_client_handle, PMIC_NPA_MODE_ID_RF_SLEEP);
    }

    /* Get NPA client for VoLTE light sleep/wakeup PAM table */
      rf_light_sleep_lte_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF1_TECH_LTE, 
                                                            "rf_light_sleep_lte_config_client", NPA_CLIENT_REQUIRED);
    if (rf_light_sleep_lte_client_handle == NULL)
    {
      ERR_FATAL( "NULL client handle not created for rf_light_sleep_lte_client_handle ",0, 0, 0);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_light_sleep_lte_client_handle has been created!!",0);
    }

    // Enable WTR4905 using the RF_2 PAM tables
      rf_wan_gps_npa_handle[RFM_1X_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_1x", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_IMT_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_wcdma", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_IMT_MODE_2] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_wcdma2", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_1XEVDO_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_evdo", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_LTE_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_lte", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_TDSCDMA_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_tdscdma", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_EGSM_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm1", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_EGSM_MODE_2] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm2", NPA_CLIENT_SUPPRESSIBLE);
      rf_wan_gps_npa_handle[RFM_EGSM_MODE_3] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm3", NPA_CLIENT_SUPPRESSIBLE);
    } 
 
#if ((defined (FEATURE_BOLT_MODEM)) && (!defined(FEATURE_BOLT_PLUS_MODEM)))

  rf_L9_S5_sv_client_handle = npa_create_sync_client( 
                                                PMIC_NPA_GROUP_ID_RF1_TECH_INIT, 
                                                "rf_L9_S5_sleep_config_client", 
                                                NPA_CLIENT_REQUIRED );

  if (rf_L9_S5_sv_client_handle == NULL)
  {
    ERR_FATAL( " NULL client handle not created for rf_L9_S5_sv_client_handle ",
               0, 0, 0 );
  }

 // For stacked HW MDM9x35
  if ((DalChipInfo_ChipId() != DALCHIPINFO_ID_MDM9630) && (DalChipInfo_ChipId() != DALCHIPINFO_ID_MDM9230)
      && (DalChipInfo_ChipId() != DALCHIPINFO_ID_MDM8630) && (DalChipInfo_ChipId() != DALCHIPINFO_ID_MDM9330))
  {
    //One time set up a required sync client to have Sleep Set formed in the RPM
    rf_sleep_set_sv_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF1_TECH_1X, 
                                                            "rf_sv_sleep_config_client", NPA_CLIENT_REQUIRED);
    if (rf_sleep_set_sv_client_handle == NULL)
    {
      ERR_FATAL( "NULL client handle not created for rf_sleep_set_sv_client_handle ",0, 0, 0);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_handle for rf_sleep_set_sv_client_handle has been created!!",0);
      npa_issue_required_request(rf_sleep_set_sv_client_handle, PMIC_NPA_MODE_ID_RF_SLEEP);
    }

    /* Get NPA client for VoLTE light sleep/wakeup PAM table */
    rf_light_sleep_lte_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF1_TECH_LTE, 
                                                            "rf_light_sleep_lte_config_client", NPA_CLIENT_REQUIRED);
    if (rf_light_sleep_lte_client_handle == NULL)
    {
      ERR_FATAL( "NULL client handle not created for rf_light_sleep_lte_client_handle ",0, 0, 0);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_light_sleep_lte_client_handle has been created!!",0);
    }

    rf_wan_gps_npa_handle[RFM_1X_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_1x", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_IMT_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_wcdma", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_IMT_MODE_2] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_wcdma2", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_1XEVDO_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_evdo", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_LTE_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_lte", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_TDSCDMA_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_tdscdma", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_EGSM_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm1", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_EGSM_MODE_2] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm2", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_EGSM_MODE_3] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X, "rfc_vreg_mgr_wtr1605_sv_gsm3", NPA_CLIENT_SUPPRESSIBLE);
  }
  else //Non-stacked HW MDM9x30
  {
    //One time set up a required sync client to have Sleep Set formed in the RPM
    rf_sleep_set_sv_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB, 
                                                            "rf_sv_sleep_config_client", NPA_CLIENT_REQUIRED);
    if (rf_sleep_set_sv_client_handle == NULL)
    {
      ERR_FATAL( "NULL client handle not created for rf_sleep_set_sv_client_handle ",0, 0, 0);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_handle for rf_sleep_set_sv_client_handle has been created!!",0);
      npa_issue_required_request(rf_sleep_set_sv_client_handle, PMIC_NPA_MODE_ID_RF_SLEEP);
    }

    /* Get NPA client for VoLTE light sleep/wakeup PAM table */
    rf_light_sleep_lte_client_handle = npa_create_sync_client( PMIC_NPA_GROUP_ID_RF1_TECH_LTE_MBB, 
                                                            "rf_light_sleep_lte_config_client", NPA_CLIENT_REQUIRED);
    if (rf_light_sleep_lte_client_handle == NULL)
    {
      ERR_FATAL( "NULL client handle not created for rf_light_sleep_lte_client_handle ",0, 0, 0);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_light_sleep_lte_client_handle has been created!!",0);
    }

    rf_wan_gps_npa_handle[RFM_1X_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB, "rfc_vreg_mgr_wtr1605_sv_1x", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_IMT_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB, "rfc_vreg_mgr_wtr1605_sv_wcdma", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_1XEVDO_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB, "rfc_vreg_mgr_wtr1605_sv_evdo", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_LTE_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB, "rfc_vreg_mgr_wtr1605_sv_lte", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_TDSCDMA_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB, "rfc_vreg_mgr_wtr1605_sv_tdscdma", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_EGSM_MODE] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB, "rfc_vreg_mgr_wtr1605_sv_gsm1", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_EGSM_MODE_2] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB, "rfc_vreg_mgr_wtr1605_sv_gsm2", NPA_CLIENT_SUPPRESSIBLE);
    rf_wan_gps_npa_handle[RFM_EGSM_MODE_3] = npa_create_schedulable_client(PMIC_NPA_GROUP_ID_RF1_TECH_1X_MBB, "rfc_vreg_mgr_wtr1605_sv_gsm3", NPA_CLIENT_SUPPRESSIBLE);
  }
 #endif
 
  rf_wan_gps_npa_handle[RFM_1X_MODE]->resource_data = &tech_resource_data[RFM_1X_MODE];
  rf_wan_gps_npa_handle[RFM_IMT_MODE]->resource_data = &tech_resource_data[RFM_IMT_MODE];
  rf_wan_gps_npa_handle[RFM_IMT_MODE_2]->resource_data = &tech_resource_data[RFM_IMT_MODE_2];
  rf_wan_gps_npa_handle[RFM_1XEVDO_MODE]->resource_data = &tech_resource_data[RFM_1XEVDO_MODE];
  rf_wan_gps_npa_handle[RFM_LTE_MODE]->resource_data = &tech_resource_data[RFM_LTE_MODE];
  rf_wan_gps_npa_handle[RFM_TDSCDMA_MODE]->resource_data = &tech_resource_data[RFM_TDSCDMA_MODE];
  rf_wan_gps_npa_handle[RFM_EGSM_MODE]->resource_data = &tech_resource_data[RFM_EGSM_MODE];
  rf_wan_gps_npa_handle[RFM_EGSM_MODE_2]->resource_data = &tech_resource_data[RFM_EGSM_MODE_2];
  rf_wan_gps_npa_handle[RFM_EGSM_MODE_3]->resource_data = &tech_resource_data[RFM_EGSM_MODE_3];

  if (rf_wan_gps_npa_handle[RFM_IMT_MODE] == NULL || 
      rf_wan_gps_npa_handle[RFM_IMT_MODE_2] == NULL || 
      rf_wan_gps_npa_handle[RFM_1X_MODE] == NULL  || 
      rf_wan_gps_npa_handle[RFM_1XEVDO_MODE] == NULL ||
      rf_wan_gps_npa_handle[RFM_LTE_MODE] == NULL ||
      rf_wan_gps_npa_handle[RFM_TDSCDMA_MODE] == NULL || 
      rf_wan_gps_npa_handle[RFM_EGSM_MODE] == NULL   || 
      rf_wan_gps_npa_handle[RFM_EGSM_MODE_2] == NULL ||
      rf_wan_gps_npa_handle[RFM_EGSM_MODE_3] == NULL )
  {
    ERR_FATAL( "NULL client handle created for rfc_vreg_mgr_wtr1605_sv_wan ",0, 0, 0);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED," rf_handle for rfc_vreg_mgr_wtr1605_sv_wan has been created!!",0);
  }
} 

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::init_chip_info(void)
{
   uint8 tech = 0;

   for (tech = RFM_PARKED_MODE; tech < RFM_NUM_MODES; tech++ )
   {
      rfc_state_info[tech][0] = RF_PATH_SLEEP_STATE;
      rfc_state_info[tech][1] = RF_PATH_SLEEP_STATE;
   }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
#if ((defined (FEATURE_BOLT_MODEM)) && (!defined(FEATURE_BOLT_PLUS_MODEM)))
void rfc_vreg_mgr_wtr1605_sv::init_npa_rxtx_table(void)
{
  uint8 tech = 0;
  uint8 pam_mode = 0;

  /* Use PAM table mode 1 if both A0 and A1 need to be turned on*/
  /* Use PAM table mode 2 if only A0 needs to be turned on*/
  if (num_trx == 1) 
  {
     pam_mode = PMIC_NPA_MODE_ID_RF_MODE_2;
  }
  else
  {
     pam_mode = PMIC_NPA_MODE_ID_RF_MODE_1;
  }

  for (tech = RFM_PARKED_MODE; tech < RFM_NUM_MODES; tech++ )
  {
     npa_rxtx_table[tech][RFC_VOLTAGE_SCALE_ON] = pam_mode;
  }

}
#else
void rfc_vreg_mgr_wtr1605_sv::init_npa_rxtx_table(void)
{
  uint8 vol_scale = 0;
  uint8 tech = 0;

  /* initialize NPA ID for all techs and all voltage scaling  */
  for (tech = RFM_PARKED_MODE; tech < RFM_NUM_MODES; tech++ )
  {
    for (vol_scale = RFC_VOLTAGE_SCALE_ON; vol_scale < RFC_VOLTAGE_SCALE_MAX; vol_scale++ )
    {
       if (tech != RFM_GNSS_MODE) 
       {
         npa_rxtx_table[tech][vol_scale] = PMIC_NPA_MODE_ID_RF_SLEEP;
       }
       else
       {
         npa_rxtx_table[tech][vol_scale] = PMIC_NPA_MODE_ID_GPS_MODE_0;
       }
    }
  }

  for (tech = RFM_PARKED_MODE; tech < RFM_NUM_MODES; tech++ )
  {
     if (tech != RFM_GNSS_MODE) {
       npa_rxtx_table[tech][RFC_VOLTAGE_SCALE_ON] = PMIC_NPA_MODE_ID_RF_MODE_1;
       npa_rxtx_table[tech][RFC_VOLTAGE_SCALE_OFF] = PMIC_NPA_MODE_ID_RF_MODE_2;
     }
     else
     {
        npa_rxtx_table[tech][RFC_VOLTAGE_SCALE_ON] = PMIC_NPA_MODE_ID_GPS_MODE_1;
        npa_rxtx_table[tech][RFC_VOLTAGE_SCALE_OFF] = PMIC_NPA_MODE_ID_GPS_MODE_2;
     }
  }

}
#endif
/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::print_npa_rf_id_msg(uint8 npa_id)
{
  switch(npa_id)
  {
    case PMIC_NPA_MODE_ID_RF_SLEEP:
      MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Issuing command to NPA with mode:PMIC_NPA_MODE_ID_RF_SLEEP",0,0);
      break;

    case PMIC_NPA_MODE_ID_RF_MODE_1:
      MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Issuing command to NPA with mode:PMIC_NPA_MODE_ID_RF_MODE_1",0,0);
      break;

    case PMIC_NPA_MODE_ID_RF_MODE_2:
      MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Issuing command to NPA with mode:PMIC_NPA_MODE_ID_RF_MODE_2",0,0);
      break;

    default:
      MSG_ERROR("Invalid NPA id being sent %d", npa_id,0,0);
      break;
  }}

/* ----------------------------------------------------------------------- */
/*!
   @brief

   @details

*/
void rfc_vreg_mgr_wtr1605_sv::print_npa_rf_gps_id_msg(uint8 npa_id)
{
  switch(npa_id)
  {
    case PMIC_NPA_MODE_ID_GPS_MODE_0:
      MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Issuing command to NPA with mode:PMIC_NPA_MODE_ID_GPS_MODE_0",0,0);
      break;

    case PMIC_NPA_MODE_ID_GPS_MODE_1:
      MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Issuing command to NPA with mode:PMIC_NPA_MODE_ID_GPS_MODE_1",0,0);
      break;

    case PMIC_NPA_MODE_ID_GPS_MODE_2:
      MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Issuing command to NPA with mode:PMIC_NPA_MODE_ID_GPS_MODE_2",0,0);
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid NPA id being sent %d", npa_id);
      break;
  }
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   Checks if the rf is asleep.
 
   @details
   Checks if the rf is asleep by looping through all rfm modes to check if they are in sleep.
 
   @return
   boolean TRUE for sleep and FALSE for wakeup
*/
boolean rfc_vreg_mgr_wtr1605_sv::rf_vreg_is_asleep()
{
  uint8 tech = RFM_PARKED_MODE;

  for (tech = RFM_PARKED_MODE; tech < RFM_NUM_MODES;tech++ )
  {

    if (last_rf_wan_gps_npa_id[tech] != PMIC_NPA_MODE_ID_RF_SLEEP)
    {
      return FALSE;
    }

  }

  return TRUE;

}


/* ----------------------------------------------------------------------- */
/*!
/*!
   @brief
   shim layer API(c function)to check if the rf is asleep.
 
   @details
   shim layer API(c function)to check if rf is asleep by calling the api rf_vreg_is_asleep() from the vreg_mgr
 
   @return
   boolean TRUE for sleep and FALSE for null pointer/wakeup
*/
boolean rf_is_asleep()
{

  rfc_vreg_mgr_wtr1605_sv* rfc_vreg_mgr =(rfc_vreg_mgr_wtr1605_sv*)rfc_vreg_mgr_wtr1605_sv::get_instance();

  /*NULL pointer check*/
  if (rfc_vreg_mgr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_is_asleep()- rfc_vreg_mgr_wtr1605_sv::get_instance() returned NULL ");
    return FALSE;
  }

  return rfc_vreg_mgr->rf_vreg_is_asleep();

}
