



/*!
   @file
   rfgnss_core.c

   @brief

   @details
   This file contains rf gnss core functions.

*/

/*===========================================================================
Copyright (c) 1999 - 2002, 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007, 2008, 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2011 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/gen8/rf/core/src/rfgnss_core.c#1 $
$DateTime: 2016/12/13 07:59:49 $
$Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/15   vs      Fix exception with LPM feature on unsupported WTR
11/30/15   zg      Added support for two WTRs power voting
02/13/15   dps     Fixed KW errors
11/04/14   jr      Fix compiler warnings for WCDMA compile out flavor 
10/17/14   hs      Added WTR4905 command support.
09/19/14   rsk     GNSS memory clean-up, removed obsolete GNSS MSM function calls.
08/15/14   lm      Added GNSS B20/28 mode cases in rfgnss_core_device_seq_execute()
07/31/14   ndb     Added support for extended EARFCN for LTE
06/26/14   hs      Added WTR4905 command support.
12/12/13   krg     Fixed compiler warnings.
11/18/13   shb     GNSS fix
10/17/13   jn(rs)  Added WTR3925 command support.
08/28/13   jn      Added support for navrf to perform band tuning
07/22/13   gy      Added support to configure eLNA state
06/21/13   jn      Factory calibrated PGA option for WWAN co-exist.
06/12/13   gy      Checked gnss dispatch function ptr
05/31/13   jr/dbz  Sent LTE B13 TX off indicator to GNSS SW.
05/08/13   dbz     Implemented Beidou BP2 gain step calibration
04/30/13   gy      Added new sequence commands
04/24/13   dbz     New ADC DPO sequence
04/11/13   dbz     Beidou Triband mode support
04/08/13   dbz     Removed DC correction in PGA gain backoff API
03/28/13   gy      Add TDSCDMA band, channel status
03/14/13   gy      Merged pga gain backoff status update
03/14/13   sar     Fixed compiler errors in APQ target. 
03/12/13   jr/gy   Restored PGA Gain in gain backoff API 
03/10/13   gy      Fixed resource vote status
03/07/13   dbz     Mainline IMD feature for DIME PLs 
03/06/13   gy      Added Coarse DAC Read Sequence
02/20/13   dbz/gy  Added LTE B14 pga gain backoff support
02/11/13   dbz/gy  Added API to disable/enable gnss pga gain backoff
02/11/13   gy      Added gnss rfsw repartition
02/06/13   gy      Enhanced rfgnss_core_rf_status_log function
11/08/12   gy/dbz  Fixed Tx indicator notification
10/24/12   dbz     Added support for Beidou/Compass support
10/19/12   dbz     Enabled enter mode PGA gain and DCOC algo on DIME
10/03/12   dbz     Use new GPS device for power/buffer voting
08/30/12   jr      Applied gain adjustment proportional to the PGA backoff
08/30/12   dbz     Merge gainstep change for PGA backoff
08/13/12   dbz     Disabled enter mode gain and DC adjustment for bringup
08/10/12   dbz     Added delay to ADC writes after IDDQ_N bit is toggled
07/17/12   jr      Added support for non-ELNA configuration.
05/28/12   jr      Changed WTR chip rev name from 'WTR1605 rev%d' to 'WTR1605r%d' 
03/27/12   jn      Added WTR chip rev to status.
03/02/12   sar     Removed unused/disabled code.
02/24/12   dbz     Implemented new ADC power up/down sequence
02/17/12   dbz     Added IMD PGA gain intelligent blanking feature
02/15/12   dbz/pv  RF Common Core Device Manager Implementation.  
02/15/12   jn      Fix compiler warning.
02/15/12   jn      Fix Klockwork error.
02/14/12   jn      Fix compiler warnings.
02/13/12   jn      Added support for run-time selection of notch filter settings.
02/13/12   dbz     Add RF status logging for WTR1605
12/08/11   dbz     Add GNSS IMD feature
11/22/11   dbz     Remove access to REX TCB internal structure members
10/03/11   gy      Supported GPS vote for A0 in normal demod quality
10/03/11   dbz     Added timer based hysteresis loop for notch filter programming
09/12/11   dbz     Added LTE B13 gain adjustment algorithm
09/09/11   gy      Added support to get RF Device revision
08/25/11   dbz     Add PGA tracking loop control
08/25/11   aak     Changes to enable NPA voting from RFSW for resources needed  
08/11/11   dbz     Clean up RFC command dispatcher
08/08/11   dbz     Clean up obsolete RFC referecnes
07/27/11   dbz     Added run time support for multi RF asic configs
07/15/11   gy      New rfcard eLNA control 
07/06/11   gy      Support gnss on different devices
06/24/11   dbz     Implemented ADC pre amp and pre mean status update
06/10/11   dbz     Implemented new RFC abstractions for GNSS 
06/07/11   gy      Set eLNA on by default
05/12/11   dbz     Disable vreg management for bringup
05/04/11   gy     Added rfc_gnss_manage_vregs() for bringup
04/23/11   gy      Ported for nikel
01/26/11   sar/bmg Replaced legacy clk regime api with DAL API.
11/11/10   av      Save the last good known PGA value
11/10/10   av      Check for the LTE Tx flag after gain adjust 
11/09/10   av      Update the bb_gain_step when PGA is backed off
11/02/10   av      Update the RF status window when PGA gain changes
10/27/10   av      Fix RTR function for GLONASS test
10/05/10   av      Merge changes from Genesis
09/10/10   hp      Update to LTE GPS B13 Gain Adjust Algorithm  
09/02/10   kma     Updated RTR POR warmup time to 650us and TX wamrup
                   to 200us for LDO
08/26/10   hp      Added API's for GPS Notch for LTE and EVDO 
07/22/10   ra      Add support for RTR9205 device in static log update
07/13/10   dbz     Added suport for GNSS Demod quality control
07/10/10   dbz     Added support for Npler manager
07/02/10   hp      Added support for programming notch filters for SVLTE
06/30/10   hp      Added Support for LTE GPS B13 Gain Adj for SVLTE-Fusion
06/25/10   dbz     Lint error fix
06/16/10   dbz     WB and NB LL ELNA mode support, DVT Mode 1/5 SRS support
06/02/10   dbz     Merged Mode 5 SRS support from Dev brach
05/27/10   dbz     Added support for FE cal for cucurrent ARS/SRS config
05/25/10   dbz     Added support for managing PMIC XO resonator
04/16/10   dbz     Added optimized DPO algorithms
04/09/10   dbz     Added support for WB GNSS sync path support
                   for Callisto 1.x
03/19/10   dbz     Fix lint errors
03/10/10   dbz     Enable the query of ELNA GPIO status for rfm_gnss_debug
02/12/10   ra/dbz  Changed the ADC sampling clock for GPLL0 288Mhz
02/10/10   dbz     ELNA: changed ELNA management during enter mode 
                         Added ELNA management during DPO and GNSS Exit
02/02/10   kma     Moved PMIC XO voting to RFC management
02/02/10   aak     Remove compiler warnings.  
02/01/10   aak     TCXO AO power collapse for GPS 
02/01/10   dbz     Removed power rail voting, rfc_pm is turned on
01/28/10   sar     Removed deprecated file tlmm_gpio.h.
01/26/10   sar     Lint fixes.
01/06/10   dbz     Added rfm_gnss_debug support
12/17/09   dbz     Changed rfgnss_notch_cfg_type 
12/07/09   sar     Replaced call to attach/open DAL with get DAL handle function.
12/03/09   dbz     Enabled ELNA mode 2 with updated settings from auto-gen
12/02/09   dbz     Reverted to regular Mode 2 given incorrect ELNA Mode 2 settings
11/23/09   jfc     Lint fix: cast enum type to int; include DALSys.h
11/19/09   hp      Removed check for ELNA in rfgnss_core_sleep()
11/18/09   gy      Fixed LPM SNR loss
11/12/09   dbz     Fixed FE cal issue in new baseline w/ clkrgm changes
11/10/09   dbz     Moved device specifc typedefs into RTR library, cleanup intf
11/09/09   dbz     Changed to generic RTR naming convention
11/06/09   dbz     Added the GNSS RTR function manager based on 
                   config/linearity/ELNA mode
11/03/09   dbz     Added support to put RTR/GNSS ADC to sleep after FE cal
10/30/09   dbz     Supressed spur mitigation algorithms messages
10/28/09   dyc     Mode specific build support.
10/28/09   dbz     Added power management for GNSS
10/23/09   dbz     Seperate common/RTR specific func, cmd dispatcher, 
                   RTR library cleanup 
10/21/09   dbz     Enabled RF status report 
10/16/09   dbz     Added GNSS spur mitigation algorithms
10/15/09   hp      Added functions needed to support ELNA API & added 
                   rfgnss_core_card_init() & rfgnss_core_card_adc()
10/13/09   dbz     Added GNSS ADC RC Tuning algorithm in rfgnss_core_card_init()
10/13/09   dbz     Added support for GNSS ELNA Mode 2/Device Mode overhaul
10/07/09   sar     Updated TLMM interface to DAL.
09/30/09   dbz     Added RF status report support 
09/28/09   gy      Code clean up to resolve compilation warnings.
08/27/09   sar     Updated tlmm and gpio interface  for CMI.
09/13/09   gy      Added pause/resume for DPO support
09/10/09   gy      Added GPIO initialization based on card file
08/30/09   gy      Cleaned up adc control bring-up code
08/27/09   dbz     Addded FE cal at bootup setup and cleanup
08/13/09   dbz     Added multi-modes support (Mode 1,2 and 5) for algorithms
08/12/09   dbz     Added RTR8600 FE band/selectivity tuning algorithms 
08/07/09   dbz     Added RTR8600 R/RC tuning algorithms 
08/04/09   dbz     Added RTR8600 power-based PGA gain adjustment algorithms
08/03/09   dbz     Added RTR8600 PGA DC algorithms 
07/29/09   dbz     Added RTR8600 TIA DC algorithms 
06/29/09   dbz     Merged Gen8 engineering codebase into source
04/15/09   dbz     Refactor core functions
04/07/09   dbz     Remove VREG managment
03/12/09   gy      Undo commented out GPS BB functions
03/10/09   gy      Comment out GPS BB functions because GPS SW is not in CRM)
03/10/09   gy      Ported rgr functions
11/25/08   gy      Initial revision.

============================================================================*/
#include "rfa_variation.h"
#include "rfcom.h"
#include "rfcommon_core.h"
#include "rfdevice_gnss_intf.h"
#include "rfgnss_core.h"
#include "rfc_card.h"
#include "rfc_card_gnss.h"
#include "rfc_pm.h"
#include "rfdevice_type_defs.h"
#include "rex.h"
#include "DDITlmm.h"
#include "msg.h"
#include "DALSys.h"
#include "timer.h"
#include "gnss_api.h"
#include "rfcommon_nv_mm.h"
#include "rfcommon_core_device_manager.h"

extern size_t strlcpy(char *dst, const char *src, size_t size);
extern void rfgnss_msm_notch_filter_init( void );
rtr_gnss_function_type rtr_gnss_function;

static boolean rfgnss_cur_state = OFF;

/* gnss pga gain backoff is disabled by default */
static boolean rfgnss_pga_gain_backoff_flag = FALSE;


rfgnss_if_mode_enum_type rfgnss_if_mode = RFGNSS_IF_MODE_LIF;
gnss_linearity_mode_type gnss_lin_mode = GNSS_LINEARITY_MODE_HIGH;
static gnss_set_IF_frequency_type gnss_IF_freq = GNSS_60p53_KHZ;

boolean gnss_external_lna_ctrl = TRUE;

/* GNSS Status */
boolean rfgnss_status=FALSE;

/* GNSS Spur: WWAN 1x Band/Chan*/
rf_card_band_type rfgnss_1x_band;
word rfgnss_1x_chan;

/* GNSS Spur: EVDO Band/Chan*/
rf_card_band_type rfgnss_evdo_band;
uint32 rfgnss_evdo_chan;

/* GNSS Spur: WWAN LTE Band/Chan*/
rfcom_lte_band_type rfgnss_lte_band;
rfcom_lte_earfcn_type rfgnss_lte_chan;

/* GNSS Spur: WWAN TDSCDMA Band/Chan*/
rfcom_tdscdma_band_type rfgnss_tdscdma_band;
uint16 rfgnss_tdscdma_chan;

/* GNSS Spur: system determination 1x/UMTS*/
rfgnss_notch_cfg_type rfgnss_spur_system_det=RFGNSS_NOTCH_CFG_NONE;

/* GNSS Spur: WWAN WCDMA/GSM Chan*/
uint16 rfgnss_wg_channel;

boolean pga_blanking_band_flag=FALSE;
boolean pga_blanking_flag=FALSE;
boolean wwan_tx_instantanous_falg=FALSE;


gnss_RfStatusType rf_status_log={
  GNSS_RF_PATH_PRX,
  {"RTR8600",
    0,
    0,
    0,
    0,
  }
};

gnss_RfDynamicStatusType rf_dynamic_status_log={
  GNSS_RF_PATH_PRX,
  {{{0,0},{0,0}},
    0,
    0,
    0,
    0,
  }

};

rfgnss_device_rtr_status_type rfgnss_rtr_status;

typedef struct
{
  char *rf_gps_device;
}rfgnss_core_debug_device_string_type;

typedef struct
{
  char *result;
}rfgnss_core_debug_result_string_type;

rfgnss_core_debug_device_string_type rfgnss_core_debug_device_string[]=
{
  {"GNSS RF ASIC"},
  {"GNSS ADC"},
  {"RF ASIC Voltage Rail"},
  {"GNSS Ext LNA"}
};

rfgnss_core_debug_result_string_type rfgnss_core_debug_result[]=
{
  {"FAILED"},
  {"PASSED"}
};

typedef enum 
{
  RF_ASIC,
  GPS_ADC,
  RF_ASIC_LDO,
  GPS_EXT_LNA
}rfgnss_core_debug_device_enum;

extern rfgnss_gain_adjust_type rfgnss_gain_adjust;
extern int8 rfgnss_pga_cal_nv;
extern float rfgnss_adc_Vref;
extern rfgnss_demod_quality_enum_type cur_demod_status;

#define GNSS_TX_HYSTERESIS_TIMER 30

#define GNSS_NOTCH_FILTER_TIMER 10

#define GNSS_WWAN_TX_TIMER 10

extern rfgnss_notch_filter_timer_type rfgnss_notch_filter_timer;


//rfgnss_wwan_tx_timer_type rfgnss_wwan_tx_timer_inst;


/* DPO switch */
boolean rfgnss_core_dpo_enabled = TRUE;

/* RF Device Revision */
static uint8 rfdevice_encoded_rev = 0;

#ifdef FEATURE_WCDMA
extern rfcom_wcdma_band_type rfm_convert_chan_to_band( uint16 channel );
#endif

/* LTE B13 TX off state*/
/* This flag will be use for send LTE B13 off state to gnss */
static boolean rfgnss_lte_b13_txoff_indication=FALSE;

extern void rfdevice_gnss_default_func(void); 

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Do RF GPS core baseband initialization.

   @details

   @param None

   @retval None

*/
#ifndef T_MDM9615
void rfgnss_core_baseband_init( void )
{
//rfgnss_msm_notch_filter_init();
}
#endif

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function converts device number to path number.

   @param device enum

   @retval path enum

*/
rf_path_enum_type rfgnss_core_device_to_path( rfcom_device_enum_type device )
{
  rf_path_enum_type temp_path;

  switch (device)
  {
  case RFCOM_TRANSCEIVER_0 :
    temp_path = RF_PATH_0;
    break;

  case RFCOM_RECEIVER_1 :
    temp_path = RF_PATH_1;
    break;

  case RFCOM_RECEIVER_2 :
    temp_path = RF_PATH_2;
    break;

  case RFCOM_RECEIVER_3 :
    temp_path = RF_PATH_3;
    break;

  default :
    MSG_ERROR("invalid GNSS device assigment.",0,0,0);
    temp_path = RF_PATH_3;
    break;
  }

  return temp_path;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Generate RTR function based on RF config/linearity/ELNA flag

   @details

   @param None

   @retval None

*/
void rfgnss_core_determine_rtr_func( rfgnss_mode_cfg_enum_type rfgnss_mode)
{
  switch (rfgnss_mode)
  {
  case RFGNSS_MODE_CFG_NB:
    /* NB modes are depreciated */
    MSG_HIGH("Error: Narrow Band RF GNSS Mode no longer supported!", 0, 0, 0);
    break;
      
  case RFGNSS_MODE_CFG_GLONASS_TEST:
  case RFGNSS_MODE_CFG_WB:
    if (!gnss_external_lna_ctrl)
    {
      if ( gnss_lin_mode == GNSS_LINEARITY_MODE_LOW )
      {
        rtr_gnss_function = RTR_GNSS_FUNCTION5;
      }
      else
      {
        rtr_gnss_function = RTR_GNSS_FUNCTION6;
      }
      break;
    }
    else
    {
      if ( gnss_lin_mode == GNSS_LINEARITY_MODE_LOW )
      {
        rtr_gnss_function = RTR_GNSS_FUNCTION3;
      }
      else
      {
        rtr_gnss_function = RTR_GNSS_FUNCTION3;
      }
      break;
    }

  case RFGNSS_MODE_CFG_RxD:
    /* to be supported */
    break;

  case RFGNSS_MODE_CFG_BEIDOU:
    /* Beidou/Compass mode */

    rtr_gnss_function = RTR_GNSS_FUNCTION2;

    break;


   default:
      MSG_HIGH("Error: Unsupported RF GNSS Mode!", 0, 0, 0);
   break;
  
  }
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   Do RF GPS core initialization.

   @details

   @param None

   @retval None

*/
void rfgnss_core_init( rfgnss_mode_cfg_enum_type rfgnss_mode )
{

  if (rfgnss_ds->init_fp != NULL)
  {
    /* update GPS receiver state */
    rfgnss_cur_state = ON;

    switch (rfgnss_mode)
    {
    case RFGNSS_MODE_CFG_NB:
      if ( gnss_lin_mode == GNSS_LINEARITY_MODE_LOW )
      {
//rfgnss_msm_set_sample_clk_rate ( GNSS_ADC_PROC_SAMPLE_RATE_QTR );
      }
      break;
    case RFGNSS_MODE_CFG_WB:
    case RFGNSS_MODE_CFG_BEIDOU:
//rfgnss_msm_set_sample_clk_rate( GNSS_ADC_PROC_SAMPLE_RATE_FULL );
      break;

    case RFGNSS_MODE_CFG_RxD:
      /* to be supported */
      break;

    case RFGNSS_MODE_CFG_GLONASS_TEST:
//rfgnss_msm_set_sample_clk_rate( GNSS_ADC_PROC_SAMPLE_RATE_FULL );
      break;

   default:
      MSG_HIGH("Error: Unsupported RF GNSS Mode!", 0, 0, 0);
   break;

    }

    rfgnss_core_determine_rtr_func(rfgnss_mode);

    rfcommon_core_dev_power_vote(RFM_DEVICE_GPS, RFCMN_CORE_DEV_ON, RFCMN_CORE_DEV_CRITICAL_VOTE);
    //check RFNV_SPARE_1_I value to know LPM feature
    if (rfcommon_nv_get_lpm_feature_enabled())
    {
      if(rfgnss_ds->pwr_wakeup_fp != NULL)
      {
       rfgnss_ds->pwr_wakeup_fp(); //vote for powering on WTR
      }
    }
    /* This is is required only if TX off indication received when GPS is off */
    /* send LTE TX off indication to GNSS SW */
    if(rfgnss_lte_b13_txoff_indication)
    {
//rfgnss_msm_rf_lte_b13_txstatus(FALSE);
      /* change back to false once TX off indication sent */
      rfgnss_lte_b13_txoff_indication = FALSE;
    }



    (void) rfgnss_ds->init_fp(rtr_gnss_function);

    /* update the RF static status log */
    rf_status_log.z_Data.u_OpMode = rfgnss_mode;
  }
  else
  {
    MSG_ERROR("rfgnss_core_init() is set to NULL!", 0, 0, 0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configure vregs, gpio and device function pointers. Initialize the device.

   @details

   @param path :

   @retval None

*/
void rfgnss_core_config( rfgnss_path_enum_type rfgnss_path, rfgnss_mode_cfg_enum_type rfgnss_mode )
{
  rfgnss_core_init(rfgnss_mode);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Do RF GPS DC cancellation.

   @details

   @param path :

   @retval None

*/
void rfgnss_core_dc_cancellation( rfcom_device_enum_type device )
{   

  rfgnss_core_sw_dc_cancellation();

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Adjusts GPS Rx front end gain.

   @details

   @param path :

   @retval None

*/
void rfgnss_core_gain_adjust( rfcom_device_enum_type device )
{
#if 1
  rex_sleep(2);
  rfgnss_gain_adjust.rfgnss_gain_adjust_running = TRUE;
  /* If enter_mode is called multiple times without exiting
  and the tx_flag changes in between, keep the gain still backed off*/
  pga_blanking_flag = FALSE;

  if (wwan_tx_instantanous_falg)
  {
    pga_blanking_flag = TRUE;
  }
  if ( (rfgnss_gain_adjust.rfgnss_tx == RFGNSS_TX_OFF && 
       rfgnss_gain_adjust.hysteresis_timer_on == FALSE) )
  {
     rfgnss_core_sw_gain_adjust();
   
     /* Check to see if the LTE Tx turned ON during gain adjust.
        If yes, use the gain determined at bootup */
      if (rfgnss_gain_adjust.rfgnss_tx_on_during_gain_adjust) 
      {
        rfgnss_core_gain_backoff();
        rfgnss_gain_adjust.rfgnss_tx_on_during_gain_adjust=FALSE;
      }
      else
      {
        if (pga_blanking_flag)
        {
          /* TX jamming event happens during the PGA gain cal, use last know good value */

          MSG(MSG_SSID_RF,MSG_LEGACY_HIGH, "GNSS IMD PGA gain jamming event happened during enter mode. Using last good known value");
          if (rfgnss_ds->cmd_dispatch_fp != NULL)
          {
            rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GAIN_RESTORE, &rfgnss_rtr_status);
          }
        }
        else
        {
           /* TX jamming event didn't happen during the PGA gain cal */
          MSG(MSG_SSID_RF,MSG_LEGACY_HIGH, "Normal GNSS PGA gain during enter mode, no IMD event");
          /* Save the last good known PGA value */
          rfgnss_core_save_pga();
        }
      }
   
  }
  else
  {
    /* Use PGA gain from bootup or the last good know value */
    rfgnss_core_gain_backoff();
    MSG(MSG_SSID_RF,MSG_LEGACY_HIGH, "LTE Tx ON during enter mode. Using last good known value");
  }
  rfgnss_gain_adjust.rfgnss_gain_adjust_running = FALSE;
#endif
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Adjusts GPS Rx front end gain at bootup

   @details

   @param path :

   @retval None

*/
void rfgnss_core_boot_gain_adjust( rfcom_device_enum_type device )
{
  rex_sleep(2);
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GAIN_ADJUST_BOOT, &rfgnss_rtr_status);
  }
  else
  {
    MSG_LOW("boot_gain_adjust_fp set to NULL!",0,0,0);
  }

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Wakeup GNSS ADC.

   @details

   @param None

   @retval None

*/

void rfgnss_core_adc_wakeup(gnss_adc_enum_type gnss_adc, rfgnss_adc_op_type wakeup)
{
  switch (gnss_adc)
  {
  case GNSS_ADC_RTR8600:
    break;

  case GNSS_ADC_NIKEL:
//rfgnss_msm_adc_wakeup(wakeup);
    break;

  case GNSS_ADC_RGR6240:
    break;

    default:
      break;

  }

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Set GNSS ADC mode.

   @details

   @param None

   @retval None

*/
boolean rfgnss_core_adc_set_mode(gnss_adc_enum_type rfgnss_adc, rfgnss_mode_cfg_enum_type rfgnss_mode )
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GET_REVISION, &rfdevice_encoded_rev);
  }
  else
  {
    MSG_LOW("sw_periodic_dc_update_fp set to NULL!",0,0,0);
  }

  switch (rfgnss_adc)
  {
  case GNSS_ADC_RTR8600:
    break;

  case GNSS_ADC_NIKEL:
//rfgnss_msm_adc_set_mode(RFGNSS_ADC_MODE_5, rfdevice_encoded_rev);
    break;

  case GNSS_ADC_RGR6240:
    break;

  default:
    break;

  }

  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function configures GNSS Card.

   @details
   This function is called from rfgnss_mc_configure() and sets the RF Card appropriately.  

   @param device : RF Multimode device
   @param gnss_mode : GNSS Mode
*/

void rfgnss_core_rf_card_init ( rfcom_device_enum_type device, rfgnss_mode_cfg_enum_type rfgnss_mode )
{
  MSG_HIGH("Entering rfgnss_core_card_init(%d, %d)", device, rfgnss_mode, 0);

    MSG_HIGH(" Entering rfgnss_core_rf_card_init(%d, %d), before gnss_mode_config", device, rfgnss_mode, 0);

  /*! @todo: convert device to path so that gnss is device independent */
  rfc_gnss_mode_config(RF_PATH_3);
    MSG_HIGH(" Entering rfgnss_core_rf_card_init(%d, %d), after gnss_mode_config", device, rfgnss_mode, 0);

  if (cur_demod_status == RFGNSS_DEMOD_QUALITY_NORMAL)
  {
    rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_RX_STATE);
    /* todo: need to change to the following when power management change is made */
    //rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_DEMOD_NORM_STATE);
  }
  else
  {
    rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_RX_STATE);
    /* todo: need to change to the following when power management change is made */
    //rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_DEMOD_HIGH_STATE);
  }

    DALSYS_BusyWait(RTR_HW_POR_WARMUP_TIME);

  if (gnss_external_lna_ctrl == TRUE)
    (void)rfc_gnss_command_dispatch(RF_PATH_3, RFC_GNSS_ELNA_ON, NULL);
  else
    (void)rfc_gnss_command_dispatch(RF_PATH_3, RFC_GNSS_ELNA_OFF, NULL);

    rfgnss_core_config(RFGNSS_PATH_0, rfgnss_mode);

  MSG_HIGH("Exiting rfgnss_core_card_init()", 0, 0, 0);
}



/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function configures GNSS ADC.

   @details
   This function is called from rfgnss_mc_configure() and sets the GNSS ADC appropriately.  

   @param device : RF Multimode device
   @param gnss_mode : GNSS Mode
*/

void rfgnss_core_adc_init ( rfcom_device_enum_type device, rfgnss_mode_cfg_enum_type rfgnss_mode )
{
  MSG_HIGH("Entering rfgnss_core_adc_init(%d, %d)", device, rfgnss_mode, 0);

    rfgnss_core_adc_wakeup(GNSS_ADC_NIKEL, RFGNSS_ADC_ENABLE );

    /* To avoid glitches, set ADC 5us after IDDQ_N = 1  */
    DALSYS_BusyWait(5);

    (void)rfgnss_core_adc_set_mode( GNSS_ADC_NIKEL, rfgnss_mode );

    /* wait 10us for ADC register to settle */
    DALSYS_BusyWait(10);

    /* turn on ADC clock */
    rfgnss_core_adc_clk_ctl (TRUE);

  MSG_HIGH("Exiting rfgnss_core_adc_init()", 0, 0, 0);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function configures GNSS baseband processor.

  @details

*/
void rfgnss_core_bbc_init(rfgnss_mode_cfg_enum_type rfgnss_mode)
{
  rfgnss_msm_rfasic_type rfgnss_rfasic_specific;

  /* query device for RF asic specific settings */

  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GET_RFASIC_SPECIFC, &rfgnss_rfasic_specific);
  }
  else
  {
    MSG_LOW("rfgnss_ds->cmd_dispatch_fp set to NULL!",0,0,0);
  }

//rfgnss_msm_set_rfasic_specific(&rfgnss_rfasic_specific);

//rfgnss_msm_bbc_init(rfgnss_mode, rfgnss_if_mode);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
  This function puts GNSS device into sleep mode

   @details

*/
void rfgnss_core_sleep( void )
{
  if (rfgnss_ds->sleep_fp != NULL)
  {
    rfgnss_ds->sleep_fp();
    //check RFNV_SPARE_1_I value to know LPM feature
    if (rfcommon_nv_get_lpm_feature_enabled())
    {
      if(rfgnss_ds->pwr_sleep_fp != NULL)
      {
       rfgnss_ds->pwr_sleep_fp(); //vote for powering off WTR
      }
    }
    rfcommon_core_dev_power_vote(RFM_DEVICE_GPS, RFCMN_CORE_DEV_OFF, RFCMN_CORE_DEV_NO_VOTE);

    /* update GPS receiver state */
    rfgnss_cur_state = OFF;

    /* turn off the ELNA */
    (void)rfc_gnss_command_dispatch(RF_PATH_3, RFC_GNSS_ELNA_OFF, NULL);

    rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_SLEEP_STATE);

  }
  else
  {
    MSG_ERROR("rfgnss_core_sleep() is set to NULL!", 0, 0, 0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function reads current GNSS state.

   @details

   @param None

   @retval ON: GPS is active or OFF: GPS is inactive
*/
boolean rfgnss_core_get_state( void )
{
  return(rfgnss_cur_state);
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function reads the GPS receiver chip ID.

   @details

   @param None

   @retval
*/
uint8 rfgnss_core_get_chip_id( void )
{ 
  return 1;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function performs the R/RC tune cal for the RF device

   @details

   @param None

   @retval None
*/
void rfgnss_core_do_r_rc_tune_cal( rfcom_device_enum_type device )
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_RC_TUNE_CAL, &rfgnss_rtr_status);
  }
  else
  {
    MSG_LOW("rfgnss_core_do_r_rc_tune_cal() set to NULL!",0,0,0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function performs the FE cal for the RF device

   @details

   @param None

   @retval None
*/
void rfgnss_core_fe_calibration( rfcom_device_enum_type device )
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_FE_CAL, &rfgnss_rtr_status);
  }
  else
  {
    MSG_LOW("rfgnss_core_do_r_rc_tune_cal() set to NULL!",0,0,0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function gets the baseband frequency offset (rotator) for this device.

   @details

   @param None

   @retval None
*/
float rfgnss_core_get_bb_freq_offset( void)
{
  float freq_offset;

  switch ((gnss_device_enum_type)rfgnss_ds->device_id )
  {
  case GNSS_DEV_RTR6285:
    if (gnss_IF_freq ==GNSS_60p53_KHZ)
    {
      freq_offset = 60000.53;
    }
    else
    {
      freq_offset = (1575420000.0 - 19200000.0 * (4.0 * 20.0 + 1.0 + 0.5 + 1843746.5/8388608.0 + 1.0/3.0));            /* RTR6285 is true ZIF GPS */
    }
    break;

    /*! @todo gordon: what is the setting for RTR8600? */
  case GNSS_DEV_RTR8600:
    freq_offset = 0;
    break;

  default:
    freq_offset = ( 60.0*1000.0/113.0);  /* all other GPS receivers use this value */
    break;
  }

  return(freq_offset);
}


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
void rfgnss_core_indicate_gsm_band_change( rfi_band_type band)
{
}
#endif
/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function calls the coarse and fine DC tuning algorithms and cancels the
   I&Q DC for gps chain.

   @details

   @param None

   @retval None
*/
void rfgnss_core_sw_dc_cancellation( void )
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_DC_CANCELLATION, &rfgnss_rtr_status);
  }
  else
  {
    MSG_LOW("sw_dc_cancellation_fp set to NULL!",0,0,0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Gain adjustment

   @details

   @param None

   @retval None
*/
void rfgnss_core_sw_gain_adjust( void )
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GAIN_ADJUST, &rfgnss_rtr_status);
  }
  else
  {
    MSG_LOW("sw_gain_adjust_fp set to NULL!",0,0,0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Gain setting to factory value

   @details

   @param None

   @retval None
*/
void rfgnss_core_gain_use_factory(void) 
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GAIN_USE_FACTORY, &rfgnss_pga_cal_nv);
  }
  else
  {
    MSG_LOW("gain_use_factory_fp set to NULL!",0,0,0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Periodic Dc update

   @details

   @param None

   @retval None
*/
void rfgnss_core_sw_periodic_dc_update( void )
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_DC_UPDATE, &rfgnss_rtr_status);
  }
  else
  {
    MSG_LOW("sw_periodic_dc_update_fp set to NULL!",0,0,0);
  }
}


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
void rfgnss_core_resume(void)
{
  if (rfgnss_core_dpo_enabled)
  {
    if (rfgnss_ds->resume_fp != NULL)
    {
      /* turn on the Vregs */
      if (cur_demod_status == RFGNSS_DEMOD_QUALITY_NORMAL)
      {
        rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_RX_STATE);
        /* todo: need to change to the following when power management change is made */
        //rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_DEMOD_NORM_STATE);
      }
      else
      {
        rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_RX_STATE);
        /* todo: need to change to the following when power management change is made */
        //rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_DEMOD_HIGH_STATE);
      }
  
      DALSYS_BusyWait(RTR_HW_POR_WARMUP_TIME);
  
      /* turn on the ELNA */
      if (gnss_external_lna_ctrl == TRUE)
        (void)rfc_gnss_command_dispatch(RF_PATH_3, RFC_GNSS_ELNA_ON, NULL);
      else
        (void)rfc_gnss_command_dispatch(RF_PATH_3, RFC_GNSS_ELNA_OFF, NULL);

      rfcommon_core_dev_power_vote(RFM_DEVICE_GPS, RFCMN_CORE_DEV_ON, RFCMN_CORE_DEV_NON_CRITICAL_VOTE);

      //check RFNV_SPARE_1_I value to know LPM featur
      if (rfcommon_nv_get_lpm_feature_enabled())
      {
        if(rfgnss_ds->pwr_wakeup_fp != NULL)
        {
         rfgnss_ds->pwr_wakeup_fp(); //vote for powering on WTR  
        }
      }
      (void)rfgnss_ds->resume_fp();

      /* set the sample clock */

//rfgnss_msm_set_sample_clk_rate( GNSS_ADC_PROC_SAMPLE_RATE_FULL );
    }
    else
    {
      MSG_ERROR("rfgnss_core_resume() is set to NULL!", 0, 0, 0);
    }
  }
}

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
void rfgnss_core_pause(void)
{
  if (rfgnss_core_dpo_enabled)
  {
    if (rfgnss_ds->pause_fp != NULL)
    {
      rfgnss_ds->pause_fp();
      //check RFNV_SPARE_1_I value to know LPM feature
      if (rfcommon_nv_get_lpm_feature_enabled())
      {
       if(rfgnss_ds->pwr_sleep_fp != NULL)
       {
         rfgnss_ds->pwr_sleep_fp(); //vote for powering off WTR
       }
      }
      rfcommon_core_dev_power_vote(RFM_DEVICE_GPS, RFCMN_CORE_DEV_OFF, RFCMN_CORE_DEV_NO_VOTE);
  
      /* turn off the ELNA */
      (void)rfc_gnss_command_dispatch(RF_PATH_3, RFC_GNSS_ELNA_OFF, NULL);
  
      /* vote off the Vregs */
      rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_SLEEP_STATE);  
    }
    else
    {
      MSG_ERROR("rfgnss_core_pause() is set to NULL!", 0, 0, 0);
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function is periodically called by gnss software. The RF driver will then
   query the gnss software for dc and amplitude values and update the RF as needed.

   @param device : RF Multimode device
*/
int32 rfgnss_core_trigger_bbc_update(void)
{
  rfgnss_core_sw_periodic_dc_update();
  return(TRUE);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function configures external GPS LNA via GPIO(s).

   @details

   @param None

   @retval None
*/
void rfgnss_core_set_ext_lna_on( boolean ext_lna_on, rf_path_enum_type path )
{
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function setup the MSM for bootup cal. It's called before the
   GNSS RTR Front-end cal
 

   @details

   @param None

   @retval None
*/
void rfgnss_core_do_boot_procedure_msm_setup(void)
{

  /* setup the Callisto core for FE cal */

//rfgnss_msm_bootup_fe_cal_setup();
}

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
                                                  rfgnss_lin_type_enum_type gnss_linearity)
{

  /* setup the Callisto core for FE cal */

//rfgnss_msm_bootup_fe_cal_setup_mode(gnss_mode, gnss_linearity);
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function setup the RTR for bootup cal. It's called before the
   GNSS RTR Front-end cal
 

   @details

   @param None

   @retval None
*/
void rfgnss_core_do_boot_procedure_setup(rfgnss_mode_cfg_enum_type rfgnss_mode)
{
  /*! todo: Dongbo: need to optimize for 1.1. release*/ 

  rfc_gnss_mode_config(RF_PATH_3);
  if (cur_demod_status == RFGNSS_DEMOD_QUALITY_NORMAL)
  {
    rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_RX_STATE);
    /* todo: need to change to the following when power management change is made */
    //rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_DEMOD_NORM_STATE);
  }
  else
  {
    rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_RX_STATE);
    /* todo: need to change to the following when power management change is made */
    //rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_DEMOD_HIGH_STATE);
  }

  DALSYS_BusyWait(RTR_HW_POR_WARMUP_TIME);

  if (gnss_external_lna_ctrl == TRUE)
    (void)rfc_gnss_command_dispatch(RF_PATH_3, RFC_GNSS_ELNA_ON, NULL);
  else
    (void)rfc_gnss_command_dispatch(RF_PATH_3, RFC_GNSS_ELNA_OFF, NULL);

  /* turn off ADC clock, make sure ADC clk is off when write RTR and ADC reg */
  rfgnss_core_adc_clk_ctl (FALSE);

  /* setup RTR and MSM*/
  rfgnss_core_config(RFGNSS_PATH_0, rfgnss_mode);
  rfgnss_core_bbc_init(rfgnss_mode);


  /* setup GNSS ADC*/
  rfgnss_core_adc_wakeup(GNSS_ADC_NIKEL, RFGNSS_ADC_ENABLE );
  (void)rfgnss_core_adc_set_mode( GNSS_ADC_NIKEL, rfgnss_mode );

  /* wait 10us for ADC register to settle */
  DALSYS_BusyWait(10);

  /* turn off ADC clock, make sure ADC clk is off when write RTR and ADC reg */
  rfgnss_core_adc_clk_ctl (TRUE);

  rex_sleep (1);
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function cleans up the MSM and RTR for bootup cal. It's called after the
   GNSS RTR Front-end cal
 

   @details

   @param None

   @retval None
*/
void rfgnss_core_do_boot_procedure_cleanup(void)
{
  //int i;
  //char local_device[GNSS_RF_STATUS_MAX_STR_LEN] = "RTR8600";

  /* turn off ADC clock before programming RTR and ADC registers */
  rfgnss_core_adc_clk_ctl (FALSE);

//rfgnss_msm_bootup_fe_cal_cleanup();
  rfgnss_core_sleep();
  /* determine the rf asic found in the rf card.
     This information is used solely to provide
     information in QXDM RF Status Window */
  /* if( !(RF_HW_QRF92X5_CONFIG1A == (rf_hw_type)rfcommon_nv_get_rf_config()) &&
      !(RF_HW_VU401_QRF92X5_CONFIG1A == (rf_hw_type)rfcommon_nv_get_rf_config()) )
  {
    for(i=0;i<GNSS_RF_STATUS_MAX_STR_LEN;i++)
    {
      rf_status_log.z_Data.u_StringData[i] = local_device[i];
    }
  } */
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function gets Pre_Mean_Mon for debugging.

   @details

   @param None

   @retval None
*/
void rfgnss_core_get_premean_mon(void)
{
//  gnss_MeanMonType rfgnss_pri_premean_mon;
//  gnss_AdcProcAmplMonType rfgnss_pri_preamp_mon;
//
//  double i_est_offset, q_est_offset;
//  /* get and update dc mean */
//  /* start dc mean measurement */
////rfgnss_msm_meas_cmd(MEAS_START);
//  rex_sleep(1);
////rfgnss_msm_meas_cmd(MEAS_STOP);
//  if ( (rtr_gnss_function ==  RTR_GNSS_FUNCTION4)
//       ||  (rtr_gnss_function ==  RTR_GNSS_FUNCTION7))
//  {
////rfgnss_msm_get_dc_mean(GNSS_ADC_PROC_PRX_ASYNC, &rfgnss_pri_premean_mon);
//  } else
//  {
////rfgnss_msm_get_dc_mean(GNSS_ADC_PROC_1_SYNC, &rfgnss_pri_premean_mon);
//  }
////rfgnss_msm_get_pre_gain(GNSS_BP1_LBW, &rfgnss_pri_preamp_mon);
//  if (rfgnss_pri_premean_mon.u_SampleCorrupt == FALSE)
//  {
//    i_est_offset = rfgnss_pri_premean_mon.d_MeanMonI*10000;
//    q_est_offset = rfgnss_pri_premean_mon.d_MeanMonQ*10000;
//    MSG_HIGH("Estimated Pre Mean: i_est_offset=%d, q_est_offset=%d", (int)i_est_offset,(int)q_est_offset, 0);
//
//    i_est_offset = rfgnss_pri_preamp_mon.d_AmplMonI*10000;
//    q_est_offset = rfgnss_pri_preamp_mon.d_AmplMonQ*10000;
//    MSG_HIGH("Estimated BP1 Pre Ampl: i_est_ampl=%d, q_est_ampl=%d", (int)i_est_offset, (int)q_est_offset, 0);
//  }
}


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This API sets up the environment based of presence or absence or an external LNA. 
   
   @param device : RF Multimode device

   @param gnss_mode : GNSS Mode

*/

void rfgnss_core_has_ext_lna (boolean gnss_has_ext_lna)
{
  MSG_HIGH("Entering rfgnss_core_has_ext_lna",0,0,0);
  rfgnss_core_configure_ext_lna (gnss_has_ext_lna);
  rfgnss_core_intelliceiver_configure_ext_lna (gnss_has_ext_lna);
//rfgnss_msm_bbc_configure_ext_lna (gnss_has_ext_lna);
  MSG_HIGH("Exiting rfgnss_core_has_ext_lna",0,0,0);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Determine LNA type based on RF config card

   @details

   @param None

   @retval None

*/

void rfgnss_core_determine_lna(void)
{

  rfc_gnss_lna_config_type lna;

  rfc_gnss_get_LNA_config(&lna);	
  switch (lna)
  {
  case RFC_GNSS_ELNA:
	rfgnss_core_configure_ext_lna(TRUE);
	break;

  case RFC_GNSS_NO_ELNA:
	rfgnss_core_configure_ext_lna (FALSE);
	break;

  case RFC_GNSS_ILNA:
	//rfgnss_core_configure_ext_lna (TRUE);
	break;
	
  default:
	rfgnss_core_configure_ext_lna (TRUE);
	break;
	
  }
  
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Sets up the environment for the card to configure depending on if external LNA is used or not. 
   
   @param device : RF Multimode device

   @param gnss_mode : GNSS Mode

*/

void rfgnss_core_configure_ext_lna (boolean gnss_has_ext_lna)
{
  gnss_external_lna_ctrl = gnss_has_ext_lna;
  
  /* setting device lna state variable */
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GET_LNA_STATE, &gnss_has_ext_lna);
  }
  else
  {
    MSG_LOW("rfgnss_ds->cmd_dispatch_fp set to NULL!",0,0,0);
  }
  
  MSG_HIGH("Exiting rfgnss_core_configure_ext_lna",0,0,0);
}


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Sets up the environment for intelleciver to configure depending on if external LNA is used or not. 
   
   @param device : RF Multimode device

   @param gnss_mode : GNSS Mode

*/

void rfgnss_core_intelliceiver_configure_ext_lna (boolean gnss_has_ext_lna)
{
  if (gnss_external_lna_ctrl == TRUE)
  {
    /* Pause intelliceiver */
  }
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf static status log and send the log packet to GNSS SW.

   @details

   @param None

   @retval None
*/
void rfgnss_core_rf_status_log(void)
{
  char temp_str[GNSS_RF_STATUS_MAX_STR_LEN];

  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GET_RTR_STATUS, &rfgnss_rtr_status);
  }
  else
  {
    MSG_ERROR("get_dac_func() is set to NULL!", 0, 0, 0);
  }

  rf_status_log.z_Data.w_PGAGain = rfgnss_rtr_status.pga_gain;
  rf_status_log.z_Data.u_VcoTuneCode = (uint8) 0;

  switch (rfgnss_ds->device_id )
  {
    case RTR8600:
      (void) snprintf(temp_str, GNSS_RF_STATUS_MAX_STR_LEN, "RTR8600r%d", rfgnss_rtr_status.vco_tune);
      break;

    case WTR1605:
    (void) snprintf(temp_str, GNSS_RF_STATUS_MAX_STR_LEN, "WTR1605r%d", rfgnss_rtr_status.vco_tune);
      break;

    case WFR2600:
      (void) snprintf(temp_str, GNSS_RF_STATUS_MAX_STR_LEN, "WFR2600r%d", rfgnss_rtr_status.vco_tune);
      break;

    case WTR2605:
      (void) snprintf(temp_str, GNSS_RF_STATUS_MAX_STR_LEN, "WTR2605r%d", rfgnss_rtr_status.vco_tune);
      break;

    case WTR1625:
      (void) snprintf(temp_str, GNSS_RF_STATUS_MAX_STR_LEN, "WTR1625r%d", rfgnss_rtr_status.vco_tune);
      break;

    case WGR7640:
      (void) snprintf(temp_str, GNSS_RF_STATUS_MAX_STR_LEN, "WGR7640r%d", rfgnss_rtr_status.vco_tune);
      break;

    default:
      (void) snprintf(temp_str, GNSS_RF_STATUS_MAX_STR_LEN, "INVALIDr%d", rfgnss_rtr_status.vco_tune);
      break;
  }
    strlcpy(rf_status_log.z_Data.u_StringData,temp_str,GNSS_RF_STATUS_MAX_STR_LEN);
//rfgnss_msm_rf_status_report(&rf_status_log);

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf dynamic status log and send the log packet to GNSS SW.

   @details

   @param None

   @retval None
*/
void rfgnss_core_rf_dynamic_status_log(void)
{

//rfgnss_adc_mean_type rfgnss_pri_premean_mon;
//rfgnss_adc_ampl_type rfgnss_pri_preamp_mon;

  /* Assuming the GNSS SW already set the pre_mean and pre_mon */

//rfgnss_msm_get_adc_mean(GNSS_ADC_PROC_1_SYNC, &rfgnss_pri_premean_mon);
//rfgnss_msm_get_adc_amp(GNSS_BP1_LBW, &rfgnss_pri_preamp_mon);


//rf_dynamic_status_log.z_Data.q_LBWPreAmpI = (uint32) (rfgnss_pri_preamp_mon.i);
//rf_dynamic_status_log.z_Data.q_LBWPreAmpQ = (uint32) (rfgnss_pri_preamp_mon.q);
//
//rf_dynamic_status_log.z_Data.w_ADCPreMeanI = (int32) (rfgnss_pri_premean_mon.i);
//rf_dynamic_status_log.z_Data.w_ADCPreMeanQ = (int32) (rfgnss_pri_premean_mon.q);

  /* Get the DAC codes */

  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GET_RTR_STATUS, &rfgnss_rtr_status);
  }
  else
  {
    MSG_ERROR("get_dac_func() is set to NULL!", 0, 0, 0);
  }


//rf_dynamic_status_log.z_Data.z_DcDac[0].u_DacI=(uint8) (rfgnss_rtr_status.tia_dac_i);
//
//rf_dynamic_status_log.z_Data.z_DcDac[0].u_DacQ=(uint8) (rfgnss_rtr_status.tia_dac_q);
//
//rf_dynamic_status_log.z_Data.z_DcDac[1].u_DacI=(uint8) (rfgnss_rtr_status.pga_dac_i);
//
//rf_dynamic_status_log.z_Data.z_DcDac[1].u_DacQ=(uint8) (rfgnss_rtr_status.pga_dac_q);


//rfgnss_msm_rf_dynamic_status_report(&rf_dynamic_status_log);

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function returns the rf gnss status (On/Off).

   @details

   @param None

   @retval None
*/
boolean rfgnss_core_gnss_onoff_status(void)
{
  return rfgnss_status;
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf gnss 1x band/chan state variable.

   @details

   @param None

   @retval None
*/
void rfgnss_core_1x_band_chan(rf_card_band_type band, word chan)
{

  rfgnss_1x_band = band;
  rfgnss_1x_chan = chan;

  /*
  MSG_3(MSG_SSID_RF, MSG_LVL_HIGH, "RFGNSS Core notch filter info for 1x, notch_cfg=%d, band=%d, chan=%d!",
        rfgnss_spur_system_det, rfgnss_1x_band, rfgnss_1x_chan);
  */
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf gnss evdo band/chan state variable.

   @details

   @param None

   @retval None
*/
void rfgnss_core_evdo_band_chan(rf_card_band_type band, uint32 chan)
{

  rfgnss_evdo_band = band;
  rfgnss_evdo_chan = chan;

  /*
  MSG_3(MSG_SSID_RF, MSG_LVL_HIGH, "RFGNSS Core notch filter info for EVDO, notch_cfg=%d, band=%d, chan=%d!",
        rfgnss_spur_system_det, rfgnss_evdo_band, rfgnss_evdo_chan);
  */
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function sets the rf gnss 1x band/chan state variable.

   @details

   @param None

   @retval None
*/
void rfgnss_core_wg_chan(uint16 channel)
{

  rfgnss_wg_channel = channel;

  /*
  MSG_2(MSG_SSID_RF, MSG_LVL_HIGH, "RFGNSS Core notch filter info for W/G, notch_cfg=%d, chan=%d!", 
        rfgnss_spur_system_det, rfgnss_wg_channel);
  */

}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Resume gnss operation after entering gnss pause mode.

   @param device : RF Multimode device
*/
void rfgnss_core_program_notch_filter(rfgnss_notch_cfg_type notch_cfg)
{

  /* check GPS status, then call HAL to program notch filter*/

  if (rfgnss_core_gnss_onoff_status())
  {
    /* GNSS is On, call 10 ms timer */
    MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"10ms notch timer");
    timer_set( &rfgnss_notch_filter_timer.rfgnss_notch_timer,
               GNSS_NOTCH_FILTER_TIMER, 0, T_MSEC );

    rfgnss_notch_filter_timer.notch_timer_on=TRUE;
  }

  return;
}


/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
  Mobile Message Print utility for RF GPS debug

   @param 
*/
void rfgnss_core_debug_verbose_report(boolean verbose,
                                      boolean rfgnss_core_gps_debug_status, 
                                      rfgnss_core_debug_device_enum rf_gps_device)
{
  if (verbose)
  {
    MSG_SPRINTF_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFGNSS_core_debug: %s %s.",
                  rfgnss_core_debug_device_string[rf_gps_device].rf_gps_device,
                  rfgnss_core_debug_result[rfgnss_core_gps_debug_status].result);

  }
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
boolean rfgnss_core_debug(boolean verbose, rfcom_device_enum_type device)
{
  boolean rfgnss_gps_debug_status = TRUE;
  boolean rfgnss_adc_debug_status = TRUE;
  boolean rfgnss_ext_lna_debug_status = TRUE;

  rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_RTR_HEALTH, &rfgnss_gps_debug_status);

  rfgnss_core_debug_verbose_report(verbose, rfgnss_gps_debug_status, RF_ASIC);

  if ( rfgnss_status )
  {
//rfgnss_msm_get_adc_state();
    rfgnss_core_debug_verbose_report(verbose, rfgnss_adc_debug_status, GPS_ADC);
  }


  return(rfgnss_gps_debug_status && 
         rfgnss_adc_debug_status && 
         rfgnss_ext_lna_debug_status);

}



/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the Tx flag and lowers the PGA gain
   if the hysteresis timer is OFF and GPS state is ON.

   @details

   @param boolean

   @retval None
*/
void rfgnss_core_tx_indicator( boolean enable )
{
  if ((rfgnss_gain_adjust.rf_tx_band_config == RFCOM_BAND_LTE_B13) ||
      (rfgnss_gain_adjust.rf_tx_band_config == RFCOM_BAND_LTE_B14) )
  {
    rfgnss_gain_adjust.rfgnss_tx = (rfgnss_tx_type) enable;
    MSG_1(MSG_SSID_RF,MSG_LEGACY_HIGH,"LTE B13/B14 Tx = %d", (int) enable);
    if (enable)
    {
      /* Update the flag so that if the Tx turned ON during
         gain adjust we can use the gain determined at bootup */
      if (rfgnss_gain_adjust.rfgnss_gain_adjust_running) 
      {
        rfgnss_gain_adjust.rfgnss_tx_on_during_gain_adjust=TRUE;
      }

      if ( rfgnss_core_get_state() )
      {
        if ( ( rfgnss_core_gnss_onoff_status() ) && (rfgnss_gain_adjust.hysteresis_timer_on == FALSE) )
        {
          rfgnss_core_gain_backoff();
          MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"Keep the last known good PGA gain");
        }
        /* Hysteresis 30 seconds */
        MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"30s hysteresis timer");
        timer_set( &rfgnss_gain_adjust.rfgnss_tx_hysteresis_timer,
                   GNSS_TX_HYSTERESIS_TIMER, 0, T_SEC );
      }
      /* TX off state is required to send it to GNSS SW at time of GPS exit */ 
      rfgnss_lte_b13_txoff_indication = FALSE;
    }
    else
    {
      /* this is TX off and used it for send tx off indication to GNSS SW*/ 
      rfgnss_lte_b13_txoff_indication = TRUE; 


    }
  }
  
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function clears the hysteresis timer and restores the PGA gain.

   @details

   @param timer_cb_data_type

   @retval None
*/
void rfgnss_core_tx_hysteresis_callback(timer_cb_data_type data)
{
  rfgnss_core_tx_stop_hysteresis();
  /* Restore the gain */
  rfgnss_core_gain_restore();
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function clears the hysteresis timer.

   @details

   @param None

   @retval None
*/
void rfgnss_core_tx_stop_hysteresis(void)
{
  /* Clear timer */
  (void)timer_clr( &rfgnss_gain_adjust.rfgnss_tx_hysteresis_timer, T_NONE );
  rfgnss_gain_adjust.hysteresis_timer_on = FALSE;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function initializes the state machine and defines the timer at init.

   @details

   @param rfcom_device_enum_type

   @retval None
*/
void rfgnss_core_boot_gain_adjust_init(rfcom_device_enum_type device)
{
  /* Define Hysteresis timer and init state*/
  timer_def( &rfgnss_gain_adjust.rfgnss_tx_hysteresis_timer,
             &rfgnss_gain_adjust.rfgnss_timer_group, NULL, (rex_sigs_type)0,
             rfgnss_core_tx_hysteresis_callback, (timer_cb_data_type)device );

  rfgnss_gain_adjust.rfgnss_tx = RFGNSS_TX_OFF;
  rfgnss_gain_adjust.hysteresis_timer_on = FALSE;
  rfgnss_gain_adjust.rf_tx_band_config = RFCOM_BAND_LTE_INVALID;
  rfgnss_gain_adjust.rfgnss_gain_adjust_running=FALSE;
  rfgnss_gain_adjust.rfgnss_tx_on_during_gain_adjust=FALSE;
}


/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function lowers the PGA gain during GPS session when LTE Tx turns ON.

   @details

   @param None

   @retval None
*/
void rfgnss_core_gain_backoff(void)
{
  
  uint8 reduced_pga;
  
  if (rfgnss_pga_gain_backoff_flag == TRUE)
  {
    if (rfgnss_ds->cmd_dispatch_fp != NULL)
    {
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GAIN_BACKOFF, &rfgnss_rtr_status);
    
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GET_REDUCED_PGA_BACKOFF_GAIN, &reduced_pga);
    
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Reduced PGA value is %d dB!", reduced_pga);
    
//rfgnss_msm_update_bb_gain_step(TRUE, reduced_pga);
      rfgnss_core_rf_status_log();
    }
    else
    {
      MSG_ERROR( "rfgnss_core_gain_backoff() is set to NULL!", 0, 0, 0);
    }

    MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"Gain backed off by 6 dB");
  }
  else
  {
    if (rfgnss_ds->cmd_dispatch_fp != NULL)
    {
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GAIN_RESTORE, &rfgnss_rtr_status);
    }
  }

//rfgnss_msm_rf_lte_b13_txstatus(TRUE);

    /* Hysteresis 30 seconds */
    MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"30s hysteresis timer");
    timer_set( &rfgnss_gain_adjust.rfgnss_tx_hysteresis_timer,
               GNSS_TX_HYSTERESIS_TIMER, 0, T_SEC );

    /* Update the timer flag */
    rfgnss_gain_adjust.hysteresis_timer_on = TRUE;
  }

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function restores the PGA gain during GPS session when LTE Tx turns OFF.

   @details

   @param None

   @retval None
*/
void rfgnss_core_gain_restore(void)
{
  if (rfgnss_pga_gain_backoff_flag == TRUE)
  { /* Restore PGA gain*/
    MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"Restore the gain");

    if (rfgnss_ds->cmd_dispatch_fp != NULL)
    {
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GAIN_RESTORE, &rfgnss_rtr_status);
	
      /* Second argument is dummy*/
//rfgnss_msm_update_bb_gain_step(FALSE, 0);
      rfgnss_core_rf_status_log();
    }
    else
    {
      MSG_ERROR( "rfgnss_core_gain_restore() is set to NULL!", 0, 0, 0);
    }
  }
//rfgnss_msm_rf_lte_b13_txstatus(FALSE);

  /* LTE TX off indication sent */
  rfgnss_lte_b13_txoff_indication = FALSE;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function queries the nav version. The same modem
   can have different versions of the GPS engine.

   @param device : RF Multimode device
*/
void rfgnss_core_get_nav_version(void)
{
//rfgnss_msm_get_nav_version();
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function updates the state machine at exit, pause and
   resume. 

   @details

   @param None

   @retval None
*/

void rfgnss_core_gain_adjust_state_update(void)
{

  /* send LTE TX off indication to GNSS SW */
  if(rfgnss_lte_b13_txoff_indication)
  {
//rfgnss_msm_rf_lte_b13_txstatus(FALSE);
    /* change back to false once TX off indication sent */
    rfgnss_lte_b13_txoff_indication = FALSE;
  }

  /* Exit*/
    if (rfgnss_gain_adjust.hysteresis_timer_on)
    {
    (void)timer_clr( &rfgnss_gain_adjust.rfgnss_tx_hysteresis_timer, T_NONE );
    rfgnss_gain_adjust.hysteresis_timer_on = FALSE;
    MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"GPS OFF, state update - clear timer");
  }
  }

/*!
   @brief
   Update notches with LTE band and channel information
 
   @details
   

   @param  LTE band and channel
   
*/
void rfgnss_core_lte_band_chan(rfcom_lte_band_type band, uint16 chan)
{
  rfgnss_lte_band = band;
  rfgnss_lte_chan = chan;
}

/*!
   @brief
   Update notches with TDSCDMA band and channel information
 
   @details
   

   @param  TDSCDMA band and channel
   
*/
void rfgnss_core_tdscdma_band_chan(rfcom_tdscdma_band_type band, uint16 chan)
{
  rfgnss_tdscdma_band = band;
  rfgnss_tdscdma_chan = chan;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function configures the PMIC XO bit.

   @details


   @param 
   @param 
*/

void rfgnss_core_config_pmic_xo_reso  ( boolean on_off )
{
  // rfc_gnss_command_dispatch(RF_PATH_3, (int32) RFC_GNSS_DISABLE_XO_RESO_SHUTDOWN, & on_off);
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
void rfgnss_core_demod_quality_control_npler(rfgnss_demod_quality_enum_type rfgnss_demod_quality)
{
  boolean on_off;

  if (rfgnss_demod_quality == RFGNSS_DEMOD_QUALITY_HIGH)
  {
    on_off = TRUE;
    rfcommon_core_dev_power_vote(RFM_DEVICE_GPS, RFCMN_CORE_DEV_ON, RFCMN_CORE_DEV_CRITICAL_VOTE);
    //check RFNV_SPARE_1_I value to know LPM feature
    if (rfcommon_nv_get_lpm_feature_enabled())
    {
      if(rfgnss_ds->pwr_wakeup_fp != NULL)
      {
       rfgnss_ds->pwr_wakeup_fp(); //vote for powering on WTR
      }
     }
  }
  else if (rfgnss_demod_quality == RFGNSS_DEMOD_QUALITY_NORMAL)
  {
    on_off = FALSE;
    rfcommon_core_dev_power_vote(RFM_DEVICE_GPS, RFCMN_CORE_DEV_ON, RFCMN_CORE_DEV_NON_CRITICAL_VOTE);
    //check RFNV_SPARE_1_I value to know LPM feature
    if (rfcommon_nv_get_lpm_feature_enabled())
    {
      if(rfgnss_ds->pwr_wakeup_fp != NULL)
      {
       rfgnss_ds->pwr_wakeup_fp(); //vote for powering on WTR
      }
     }
  }

  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CRITICAL_DEMOD_NOTIFY, &on_off);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   Perform GNSS demod quality control

   @param : demod quality control
     RFGNSS_DEMOD_QUALITY_HIGH: Error-free demod is required
     RFGNSS_DEMOD_QUALITY_NORM: Error-free demod is not required


   @pre
   At least one call to rfm_enter_mode() to start a GNSS session must have been
   executed prior calling this pause function
*/
void rfgnss_core_demod_quality_control_pm(rfgnss_demod_quality_enum_type rfgnss_demod_quality)
{
  /* make sure GPS is on */
  if (rfgnss_cur_state == ON)
  {
    if (rfgnss_demod_quality == RFGNSS_DEMOD_QUALITY_NORMAL)
    {
      rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_RX_STATE);
      /* todo: need to change to the following when power management change is made */
      //rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_DEMOD_NORM_STATE);
    }
    else
    {
      rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_RX_STATE);
      /* todo: need to change to the following when power management change is made */
      //rfc_gnss_manage_vregs(RF_PATH_3, RFCOM_GNSS_MODE, RF_PATH_DEMOD_HIGH_STATE);
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function saves the last good known PGA value.

   @details

   @param None

   @retval None
*/

void rfgnss_core_save_pga(void)
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_SAVE_PGA, &rfgnss_rtr_status);
  }
  else
  {
    MSG_ERROR( "rfgnss_core_save_pga() is set to NULL!", 0, 0, 0);
  }
  
  MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"Last good known PGA value saved");
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function is called from a FTM command to backoff
   the PGA gain.

   @details

   @param None

   @retval None
*/

void rfgnss_core_set_pga_backoff(void)
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GAIN_BACKOFF, &rfgnss_rtr_status);
//rfgnss_msm_update_bb_gain_step(TRUE);
    rfgnss_core_rf_status_log();
//rfgnss_msm_rf_lte_b13_txstatus(TRUE);
  }
  else
  {
    MSG_ERROR( "rfgnss_core_gain_backoff() is set to NULL!", 0, 0, 0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function is called from a FTM command to restore
   the PGA gain.

   @details

   @param None

   @retval None
*/
void rfgnss_core_set_pga_restore(void)
{
  if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GAIN_RESTORE, &rfgnss_rtr_status);
//rfgnss_msm_update_bb_gain_step(FALSE);
    rfgnss_core_rf_status_log();
//rfgnss_msm_rf_lte_b13_txstatus(FALSE);

    /* change back to false once TX off indication sent */
    rfgnss_lte_b13_txoff_indication = FALSE;
  }
  else
  {
    MSG_ERROR( "rfgnss_core_gain_restore() is set to NULL!", 0, 0, 0);
  }
}
/*----------------------------------------------------------------------------*/
/*!
   @brief
    GNSS PGA tracking loop control

   @details

   @param None

   @retval None
*/
void rfgnss_core_pga_tracking_loop_control(boolean enable)
{

    if (rfgnss_ds->cmd_dispatch_fp != NULL)
  {
    rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_SET_PGA_LOOP_STATUS, &enable);
  }
  else
  {
    MSG_ERROR( "rfgnss_ds set pga loop status () is set to NULL!", 0, 0, 0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function initializes the notch filter timer structure.

   @details

   @param rfcom_device_enum_type

   @retval None
*/
void rfgnss_core_notch_filter_timer_init(rfcom_device_enum_type device)
{
  /* Define Notch filter timer and init state */
  timer_def( &rfgnss_notch_filter_timer.rfgnss_notch_timer,
             &rfgnss_notch_filter_timer.rfgnss_notch_timer_group, NULL, (rex_sigs_type)0,
             rfgnss_core_program_notch_callback, (timer_cb_data_type)device );

  rfgnss_notch_filter_timer.rfgnss_notch_cfg=RFGNSS_NOTCH_CFG_NONE;
  rfgnss_notch_filter_timer.notch_timer_on=FALSE;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function calls the HAL to program the notch filter.

   @details

   @param rfcom_device_enum_type

   @retval None
*/
void rfgnss_core_program_notch_callback(timer_cb_data_type data)
{
  /* Update the timer variable to FALSE */
  rfgnss_notch_filter_timer.notch_timer_on=FALSE;

  /* check GPS status */
  if (rfgnss_core_gnss_onoff_status())
  {
    MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"Programming notch filters after timer expiration");
    /* GNSS is On, call the HAL layer to program notch filter */
//rfgnss_msm_program_notch_filter(rfgnss_notch_filter_timer.rfgnss_notch_cfg);
  }

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function clears the notch filter timer structure.

   @details

   @param rfcom_device_enum_type

   @retval None
*/

void rfgnss_core_clear_notch_timer(void)
{
  MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"Clearing notch filter timer");
  (void)timer_clr(&rfgnss_notch_filter_timer.rfgnss_notch_timer,T_NONE);
  rfgnss_notch_filter_timer.notch_timer_on=FALSE;
}






void rfgnss_core_wwan_tx_callback(timer_cb_data_type data)
{
  /* Update the timer variable to FALSE */
  //rfgnss_wwan_tx_timer_inst.wwan_tx_timer_on=FALSE;

  /* notify GPS SW of TX Off event */

//rfgnss_msm_wwan_tx(FALSE);
  MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"Notify GPS SW of WWAN TX Off event after timer expiration");
}

void rfgnss_core_clear_wwan_tx_timer(void)
{
  MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"WWAN TX turned back ON within timer limit, clearing WWAN TX Off timer");
  //(void)timer_clr(&rfgnss_wwan_tx_timer_inst.rfgnss_wwan_tx_timer,T_NONE);
  //rfgnss_wwan_tx_timer_inst.wwan_tx_timer_on=FALSE;
}


void rfgnss_core_wwan_tx_timer_init(rfcom_device_enum_type device)
{
#if 0
  /* Define Notch filter timer and init state */
  timer_def( &rfgnss_wwan_tx_timer_inst.rfgnss_wwan_tx_timer,
             &rfgnss_wwan_tx_timer_inst.rfgnss_wwan_tx_timer_group, NULL, (rex_sigs_type)0,
             rfgnss_core_wwan_tx_callback, (timer_cb_data_type)device );

  rfgnss_wwan_tx_timer_inst.wwan_tx_timer_on=FALSE;
#endif
}

void rfgnss_core_imd_band_chan(void)
{
  gnss_WwanStateEnum cur_wwan;
  gnss_RfBandType cur_band;
  uint32 cur_chan;

  cur_wwan = (gnss_WwanStateEnum) rfgnss_spur_system_det;


  switch (cur_wwan)
  {

  case GNSS_WWAN_STATE_WCDMA:

#ifdef FEATURE_WCDMA
    /* convert the WCDMA channel to band */

    cur_band.e_WCDMABand = ((gnss_RfcomWcdmaBandEnum)rfm_convert_chan_to_band((uint16)rfgnss_wg_channel));

    cur_chan = (uint32) rfgnss_wg_channel;

    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFGNSS IMD WWAN band/chan change event: WCDMA, band=%d, chan=%d!", 
      (uint16)rfm_convert_chan_to_band((uint16)rfgnss_wg_channel), cur_chan);

//rfgnss_msm_wwan_band_chan( cur_wwan, cur_band, cur_chan);

    if (((gnss_RfcomWcdmaBandEnum)rfm_convert_chan_to_band((uint16)rfgnss_wg_channel) == (gnss_RfcomWcdmaBandEnum) GNSS_RFCOM_BAND_800) ||
         ((gnss_RfcomWcdmaBandEnum)rfm_convert_chan_to_band((uint16)rfgnss_wg_channel) == (gnss_RfcomWcdmaBandEnum) GNSS_RFCOM_BAND_BC8))
    {
      pga_blanking_band_flag = TRUE;
      MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"GNSS IMD PGA WCDMA tunes to offending BC");
    }
    else
#endif
    {
      pga_blanking_band_flag = FALSE;
    }

    break;

  case GNSS_WWAN_STATE_1X:

    /* convert the 1x band/channel */

    cur_band.e_RfBand = (gnss_RfCardBandEnum) rfgnss_1x_band;

    cur_chan = (uint32) rfgnss_1x_chan;

    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFGNSS IMD WWAN band/chan change event: 1x, band=%d, chan=%d!", 
      (uint16)rfgnss_1x_band, cur_chan);

//rfgnss_msm_wwan_band_chan( cur_wwan, cur_band, cur_chan);

    if (((gnss_RfCardBandEnum)rfgnss_1x_band == (gnss_RfCardBandEnum) GNSS_RF_BC0_BAND) ||
         ((gnss_RfCardBandEnum)rfgnss_1x_band == (gnss_RfCardBandEnum) GNSS_RF_BC10_BAND))
    {
      pga_blanking_band_flag = TRUE;
      MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"GNSS IMD PGA 1x tunes to offending BC");
    }
    else
    {
      pga_blanking_band_flag = FALSE;
    }

    break;

  case GNSS_WWAN_STATE_EVDO:

    /* convert the HDR band/channel */

    cur_band.e_RfBand = (gnss_RfCardBandEnum) rfgnss_evdo_band;

    cur_chan = (uint32) rfgnss_evdo_chan;

    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFGNSS IMD WWAN band/chan change event: EVDO, band=%d, chan=%d!", 
      (uint16)rfgnss_evdo_band, cur_chan);

//rfgnss_msm_wwan_band_chan( cur_wwan, cur_band, cur_chan);

    if (((gnss_RfCardBandEnum)rfgnss_evdo_band == (gnss_RfCardBandEnum) GNSS_RF_BC0_BAND) ||
         ((gnss_RfCardBandEnum)rfgnss_evdo_band == (gnss_RfCardBandEnum) GNSS_RF_BC10_BAND))
    {
      pga_blanking_band_flag = TRUE;
      MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"GNSS IMD PGA HDR tunes to offending BC");
    }
    else
    {
      pga_blanking_band_flag = FALSE;
    }

    break;

   case GNSS_WWAN_STATE_LTE:

    /* convert the LTE band/channel */

    cur_band.e_LTEBand = (gnss_RfcomLTEBandEnum) rfgnss_lte_band;

    cur_chan = (uint32) rfgnss_lte_chan;

    if (rfgnss_lte_band < RFCOM_BAND_LTE_B33 )
    {
      /* if it's FDD LTE*/

      MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFGNSS IMD WWAN band/chan change event: LTE FDD, band=%d, chan=%d!", 
        (uint16)rfgnss_lte_band, cur_chan);
//rfgnss_msm_wwan_band_chan( cur_wwan, cur_band, cur_chan);

      if (((gnss_RfcomLTEBandEnum)rfgnss_lte_band == (gnss_RfcomLTEBandEnum) GNSS_RFCOM_BAND_LTE_B5) ||
           ((gnss_RfcomLTEBandEnum)rfgnss_lte_band == (gnss_RfcomLTEBandEnum) GNSS_RFCOM_BAND_LTE_B6))
      {
        pga_blanking_band_flag = TRUE;
        MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"GNSS IMD PGA LTE tunes to offending BC");
      }
      else
      {
        pga_blanking_band_flag = FALSE;
      }

    }

    break;

  case GNSS_WWAN_STATE_TDSCDMA:
    /* convert tdscdma band/channel */

    cur_band.e_TDSCDMABand = (gnss_RfcomTdscdmaBandEnum)rfgnss_tdscdma_band;

    cur_chan = (uint32) rfgnss_tdscdma_chan;

    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "RFGNSS IMD WWAN band/chan change event: TDSCDMA, band=%d, chan=%d!", 
       (uint16)rfgnss_tdscdma_band, rfgnss_tdscdma_chan);
//rfgnss_msm_wwan_band_chan( cur_wwan, cur_band, cur_chan);

    /* set pga blanking to FALSE */
    pga_blanking_band_flag = FALSE;

    break;

  default:
    break;
  }
}

void rfgnss_core_imd_tx_notify(boolean enable)
{

  if (enable == TRUE)
  {
    /* send the notification to GPS immediately */

    if (pga_blanking_band_flag)
    {
      pga_blanking_flag = TRUE;
      wwan_tx_instantanous_falg = TRUE;
    }
 #if 0   
    if (rfgnss_wwan_tx_timer_inst.wwan_tx_timer_on == TRUE)
    {
      /* if it's already true, means now in the 10sec hysteresis loop, clear the timer */

      rfgnss_core_clear_wwan_tx_timer();

    } else
    {
      /* if not enabled, TX is currently off, send ON to GPS */

//rfgnss_msm_wwan_tx(enable);

    }
 #endif   
  } else
  {
    /* TX is tuned off, set up timer*/

    MSG(MSG_SSID_RF,MSG_LEGACY_HIGH,"10 Sec WWAN TX Off timer");
 #if 0   
    timer_set( &rfgnss_wwan_tx_timer_inst.rfgnss_wwan_tx_timer,
               GNSS_WWAN_TX_TIMER, 0, T_SEC );

    rfgnss_wwan_tx_timer_inst.wwan_tx_timer_on=TRUE;
#endif
    if (pga_blanking_band_flag)
    {
      wwan_tx_instantanous_falg = FALSE;
    }

  }
}

void rfgnss_core_imd_tx_onoff(boolean enable)
{
  gnss_WwanStateEnum cur_wwan;

  cur_wwan = (gnss_WwanStateEnum) rfgnss_spur_system_det;

  switch (cur_wwan)
  {

//case RFGNSS_NOTCH_CFG_GSM:

    /* do nothing, don't pass TX status to GPS */

//  break;

  case GNSS_WWAN_STATE_1X:

    /* pass TX status to GPS  */

    rfgnss_core_imd_tx_notify(enable);

    break;

  case GNSS_WWAN_STATE_EVDO:

    /* pass TX status to GPS  */

    rfgnss_core_imd_tx_notify(enable);

    break;

  case GNSS_WWAN_STATE_WCDMA:

    /* pass TX status to GPS  */

    rfgnss_core_imd_tx_notify(enable);

    break;

  case GNSS_WWAN_STATE_LTE:

    if (rfgnss_lte_band < RFCOM_BAND_LTE_B33 )
    {
      /* if it's FDD LTE*/
      /* pass TX status to GPS  */
  
      rfgnss_core_imd_tx_notify(enable);
    }

    break;

  default:
    break;
  }

}


void rfgnss_core_adc_clk_ctl(boolean enable)
{
//rfgnss_msm_adc_clk_ctl(enable);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS vote for dev on/off

*/
boolean rfgnss_core_resource_vote(rfgnss_device_status_type dev_status, rfgnss_device_vote_type dev_vote)
{
  boolean ret_value = TRUE;
  //check RFNV_SPARE_1_I value to know LPM feature
  if (rfcommon_nv_get_lpm_feature_enabled())
  {
    if (dev_status == RFCMN_CORE_DEV_OFF ) 
    {
     if(rfgnss_ds->pwr_sleep_fp != NULL)
     {
      rfgnss_ds->pwr_sleep_fp(); //vote for powering off WTR 
     }
    }
  }
  rfcommon_core_dev_power_vote(RFM_DEVICE_GPS, (rfcmn_core_dev_status_type)dev_status, (rfcmn_core_dev_vote_type)dev_vote);  
  //check RFNV_SPARE_1_I value to know LPM feature
  if (rfcommon_nv_get_lpm_feature_enabled())
  {
    if (dev_status == RFCMN_CORE_DEV_ON)
    {
      if(rfgnss_ds->pwr_wakeup_fp != NULL)
      {
       rfgnss_ds->pwr_wakeup_fp(); //vote for powering on WTR
      }
    }
   }
  return ret_value;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
   GNSS device sequence execute command dispatch

*/
boolean rfgnss_core_device_seq_execute(rfm_gnss_device_seq_cmd_type cmd, void *data)
{
  boolean ret_value = TRUE;

  if (rfgnss_ds->cmd_dispatch_fp == (gnss_cmd_dispatch_func_ptr_type) rfdevice_gnss_default_func )
  {
     MSG_HIGH("Device cmd dispatch fp set to rfdevice_gnss_default_func!",0,0,0);
     return FALSE;
  }
  switch (cmd)
  {
    case RFGNSS_DEVICE_CMD_M5_LL_INIT:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_M5_LL_INIT, data);
      break;

    case RFGNSS_DEVICE_CMD_M5_ELNA_INIT:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_M5_ELNA_INIT, data);
	  break;

    case RFGNSS_DEVICE_CMD_M5_HL_INIT:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_M5_HL_INIT, data);
      ret_value = TRUE;
	  break; 

    case RFGNSS_DEVICE_CMD_M5HT_INIT:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_M5HT_INIT, data);
      ret_value = TRUE;
    break; 

    case RFGNSS_DEVICE_CMD_M5ET_INIT:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_M5ET_INIT, data);
      ret_value = TRUE;
    break; 

    case RFGNSS_DEVICE_CMD_M5ET1_INIT:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_M5ET1_INIT, data);
      ret_value = TRUE;
    break; 

    case RFGNSS_DEVICE_CMD_MTE_INIT:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_MTE_INIT, data);
      ret_value = TRUE;
    break; 

    case RFGNSS_DEVICE_CMD_COMPASS_ELNA_INIT:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_COMPASS_ELNA_INIT, data);
      break;
  
    case RFGNSS_DEVICE_CMD_SLEEP: 
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_SLEEP, data);
      break;
  
    case RFGNSS_DEVICE_CMD_RESUME: 
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_RESUME, data);
     
     break;
  
    case RFGNSS_DEVICE_CMD_PAUSE:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_PAUSE, data);
      break;
  
    case RFGNSS_DEVICE_CMD_M5_IF:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_M5_IF, data);
      break;
  
    case RFGNSS_DEVICE_CMD_M5_BP1_DOWNCONV_IF:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_M5_BP1_DOWNCONV_IF, data);
      break;
  
    case RFGNSS_DEVICE_CMD_M5_BP3_DOWNCONV_IF:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_M5_BP3_DOWNCONV_IF, data); 
      break;
  
    case RFGNSS_DEVICE_CMD_CHIP_OFF:
      break;
  
    case RFGNSS_DEVICE_CMD_ENABLE_TG:
       rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_ENABLE_TG, data);   
      break;
  
    case RFGNSS_DEVICE_CMD_DISABLE_TG:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DISABLE_TG, data);
      break;
  
    case RFGNSS_DEVICE_CMD_WRITE_BT_CODE:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_WRITE_BT_CODE, data);
      break;
  
    case RFGNSS_DEVICE_CMD_SET_TXATTN:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_SET_TXATTN, data);
      break;
  
    case RFGNSS_DEVICE_CMD_WRITE_SEL_CODE:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_WRITE_SEL_CODE, data);
      break;
  
    case RFGNSS_DEVICE_CMD_DCOC_WRITE_FINE_DAC: 
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DCOC_WRITE_FINE_DAC, data);
      break;
  
    case RFGNSS_DEVICE_CMD_DCOC_WRITE_COARSE_DAC: 
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DCOC_WRITE_COARSE_DAC, data);
      break;
  
    case RFGNSS_DEVICE_CMD_DCOC_FINE1_PRE_CONFIG:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DCOC_FINE1_PRE_CONFIG, data);
      break;
  
    case RFGNSS_DEVICE_CMD_DCOC_FINE1_POST_CONFIG:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DCOC_FINE1_POST_CONFIG, data);
      break;
  
    case RFGNSS_DEVICE_CMD_DCOC_COARSE_PRE_CONFIG: 
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DCOC_COARSE_PRE_CONFIG, data);
      break;
  
    case RFGNSS_DEVICE_CMD_DCOC_FINE2_PRE_CONFIG:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DCOC_FINE2_PRE_CONFIG, data);
      break;
  
    case RFGNSS_DEVICE_CMD_DCOC_FINE2_POST_CONFIG:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DCOC_FINE2_POST_CONFIG, data);
      break;
  
    case RFGNSS_DEVICE_CMD_DCOC_READ_FINE_DAC:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DCOC_READ_FINE_DAC, data);
      break;
  
    case RFGNSS_DEVICE_CMD_DCOC_READ_COARSE_DAC:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_DCOC_READ_COARSE_DAC, data); 
      break;
 
    case RFGNSS_DEVICE_CMD_PGA_WRITE: 
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_PGA_WRITE, data);
      break;
  
    case RFGNSS_DEVICE_CMD_WRITE_R_TUNE: 
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_WRITE_R_TUNE, data);
      break;
      
    case RFGNSS_DEVICE_CMD_WRITE_RC_TUNE: 
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_WRITE_RC_TUNE, data);
      break;
      
    case RFGNSS_DEVICE_CMD_SET_MIXER_NOTCH:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_SET_MIXER_NOTCH, data);
      break;

    case RFGNSS_DEVICE_CMD_GET_RF_STATUS:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_GET_RF_STATUS, data);
      break;

    case RFGNSS_DEVICE_CMD_GET_RF_HEALTH:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_GET_RF_HEALTH, data);
      break;

    case RFGNSS_DEVICE_CMD_GET_CHIP_REVISION:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_GET_CHIP_REVISION, data);
      break;

    case RFGNSS_DEVICE_CMD_GET_CHIP_PROCESS:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_GET_CHIP_PROCESS, data);
      break;


    case RFGNSS_DEVICE_CMD_IELNA_CONFIG:
      rfgnss_ds1->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_IELNA_CONFIG, data);
      break;

    case RFGNSS_DEVICE_CMD_GET_LNA_STATE:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_GET_LNA_STATE, data);
      break;

    case RFGNSS_DEVICE_CMD_CALCULATE_BT_GNSS_ELNA:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_CALCULATE_BT_GNSS_ELNA, data);
      break;

    case RFGNSS_DEVICE_CMD_QFUSE_READ_GPS:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_QFUSE_READ_GPS, data);
      break;

    case RFGNSS_DEVICE_CMD_ADC_CAL_PREP:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_ADC_CAL_PREP, data);
      break;

    case RFGNSS_DEVICE_CMD_ADC_CAL_AFTER:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_ADC_CAL_AFTER, data);
      break;

    case RFGNSS_DEVICE_CMD_SE_DCOC:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_SE_DCOC, data);
      break;

    case RFGNSS_DEVICE_CMD_ENTER_2H_MODE:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_ENTER_2H_MODE, data); 
      break;

    case RFGNSS_DEVICE_CMD_EXIT_2H_MODE:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_EXIT_2H_MODE, data); 
      break; 

	case RFGNSS_DEVICE_CMD_ENTER_B20_B28_MODE:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_ENTER_B20_B28_MODE, data); 
      break;

    case RFGNSS_DEVICE_CMD_EXIT_B20_B28_MODE:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_EXIT_B20_B28_MODE, data); 
      break;
	  
    case RFGNSS_DEVICE_CMD_SET_FINE_GAIN:
      rfgnss_ds->cmd_dispatch_fp(RFDEVICE_GNSS_CMD_SET_FINE_GAIN, data);
      break;
      
    default:
      MSG_HIGH("Invalid Device cmd dispatch!",0,0,0);
      ret_value = FALSE;
      break;
  }

  return ret_value;
}
/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function is used to disable/enable GNSS PGA gain backoff

   @param enable: TRUE to enable or FALSE to disable

*/
void rfgnss_core_pga_gain_backoff_control(boolean enable)
{
  rfgnss_pga_gain_backoff_flag = enable;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief

   @details
   This function is used to trigger BP2 gain step calibration

   @param enable: TRUE to enable or FALSE to disable

*/
void rfgnss_core_bp2_gainstep_adjust(void)
{
//rfgnss_msm_bp2_gainstep_adjust();
}


