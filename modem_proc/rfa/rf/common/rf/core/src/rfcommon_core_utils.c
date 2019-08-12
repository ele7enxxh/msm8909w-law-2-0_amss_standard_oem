/*!
  @file
  rfcommon_core_utils.c

  @brief
  This file contains all type definitions for the RFCOMMON Utilities module.
*/

/*=============================================================================
Copyright (c) 2013- 2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core_utils.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/04/16   cv      Add LTE B66 support
05/27/15   zhh    FBRX ILPC optimize
11/03/14   ndb     Fix the compiler error in W/T removed builds
11/03/14   ndb     Fix the compiler error in LTE removed builds
10/27/14   pk      Featurized the CDMA code to fix the compiler error on 
                   1x compiled out build
10/16/14   ka      KW fix
10/15/14   JJ      Add fix in rfcommon_core_get_tx_device_ptr for HDR
10/08/14   ka      Add common api to return device pointer based on tech and band
10/02/14   ck      Add an utility api to convert rfcom band type to sys band type
07/24/14   tks     Add conversion support for cdma bands 
07/22/14   tks     Add conversion support from sys to rfcom band
05/29/14   pl      Add conversion utility from tech bands to rfc_bands
09/06/13   jhe     Get right tds band info for both ftm and online
07/22/13   cri     Get band info from online state machine for LTE
05/28/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/06/13   kb      Add TDS support to get_tx_chain_from_device()
04/05/13   gvn     FTM IRAT Timing packet support
04/05/13   kg      Adding API to return tech string
03/05/13   vrb     Replacing RFC accessor param init_req boolean by req enum
02/08/13   cri     Initial version of common core utilities

============================================================================*/

#include "rfcom.h"
#include "rfm_internal.h"
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rfcommon_core_utils.h"
#include "rfcommon_msg.h"
#include "rfc_common.h"
#ifdef FEATURE_WCDMA
#include "rfc_wcdma_data.h"
#include "rfwcdma_mc.h"
#include "rfdevice_wcdma_intf.h"
#endif
#ifdef FEATURE_CDMA1X
#include "rfc_cdma.h"
#include "rf_cdma_data.h"
#include "rf_cdma_utils_freq.h"
#include "rfdevice_trx_cdma_tx.h"
#endif
#ifdef FEATURE_LTE
#include "rfc_lte_data.h"
#include "rflte_mc.h"
#include "rfdevice_lte_interface.h"
#endif

#include <stringl/stringl.h>
#ifdef FEATURE_WCDMA
extern rfcom_wcdma_band_type ftm_curr_wcdma_mode;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "rf_tdscdma_mc.h"
#include "rfc_tdscdma_data.h"
#include "rfdevice_trx_tdscdma_tx.h"
#include "rfc_card_tdscdma.h"
extern rfcom_tdscdma_band_type ftm_tdscdma_current_mode;
#endif


/*----------------------------------------------------------------------------*/
/*!
  @brief
 Mode-to-string table. Derived from rfm_mode_enum_type
*/
static const char* mode_to_string_tbl[RFM_NUM_MODES] = 
{
    "park",            // RFM_PARKED_MODE
    "wcdma",           // RFM_IMT_MODE
    "gsm",             // RFM_EGSM_MODE
    "cdma",            // RFM_1X_MODE,
    "gps",             // RFM_GPS_MODE
    "cdma",            // RFM_1XEVDO_MODE
    "cdma",            // RFM_SVDO_MODE
    "gnss",            // RFM_GNSS_MODE
    "lte",             // RFM_LTE_MODE
    "lte",             // RFM_LTE_FTM_MODE
    "tdscdma",         // RFM_TDSCDMA_MODE
    "invalid",
    "invalid",
    "invalid",
    "tdscdma",
    "gsm",
    "gsm",
    "wlan",
   
};
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Converts device to tx chain.
 
  @param device
  The device to convert to tx chain

  @return
  Corresponding tx chain for given device
*/
uint8 rfcommon_core_get_tx_chain_from_device( rfm_device_enum_type device )
{
  rfm_mode_enum_type current_mode;
  rfc_device_info_type *device_info_ptr;
  rfc_cfg_params_type cfg;
  uint8 modem_chain;
  #ifdef FEATURE_CDMA1X
  const rf_cdma_data_status_type *dev_status_r;
  #endif
  
  /* Get the current RFM mode */
  current_mode = rfm_get_current_mode(device);
  modem_chain = 0;
  cfg.alternate_path = 0;

  switch(current_mode)
  {
     case RFCOM_WCDMA_MODE:
       #ifdef FEATURE_WCDMA
       cfg.band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;
       cfg.req = RFC_REQ_DEFAULT_GET_DATA;
       cfg.logical_device = device;
       cfg.rx_tx = RFC_CONFIG_TX;
       /* Get RFC information */
       if(rfc_wcdma_data_get_device_info(&cfg, &device_info_ptr) == TRUE)
       {
         modem_chain = device_info_ptr->modem_chain;
       }
       else
       {
         RF_MSG_1( RF_ERROR, "rfcommon_core_get_tx_chain_from_device:"
                   " could not get chain from dev %d", device ); 
       }
       #endif
       break;

     case RFCOM_1X_MODE:
     case RFCOM_1XEVDO_MODE:
       #ifdef FEATURE_CDMA1X
       dev_status_r = rf_cdma_get_device_status( device );
       if(dev_status_r != NULL)
       {
       modem_chain = rfc_cdma_get_tx_modem_chain( device, dev_status_r->curr_band);
       }
       else
       {
         RF_MSG( RF_ERROR, "rfcommon_core_get_tx_chain_from_device: "
                           "Null Device status pointer" ); 
       }
       #endif
       break;

     case RFCOM_LTE_MODE:
     case RFCOM_LTE_FTM_MODE:
       #ifdef FEATURE_LTE
       cfg.band = rflte_mc_sm_get_band(RFLTE_MC_PCELL_IDX);
       cfg.req = RFC_REQ_DEFAULT_GET_DATA;
       cfg.logical_device = device;
       cfg.rx_tx = RFC_CONFIG_TX;
       /* Get RFC information */
       if (rfc_lte_data_get_device_info(&cfg, &device_info_ptr) == TRUE)
       {
         modem_chain = device_info_ptr->modem_chain;
       }
       else
       {
         RF_MSG_1( RF_ERROR, "rfcommon_core_get_tx_chain_from_device:"
                   " could not get chain from dev %d", device ); 
       }
       #endif
       break;

     case RFCOM_TDSCDMA_MODE:
       #ifdef FEATURE_TDSCDMA
       cfg.band = rf_tdscdma_mc_state.curr_band_tx;
       cfg.req = RFC_REQ_DEFAULT_GET_DATA;
       cfg.logical_device = device;
       cfg.rx_tx = RFC_CONFIG_TX;
       
       /* Get RFC information */
       if(rfc_tdscdma_get_devices_info_data(&cfg, &device_info_ptr) == TRUE)
       {
         modem_chain = device_info_ptr->modem_chain;
       }
       else
       {
         RF_MSG_1( RF_ERROR, "rfcommon_core_get_tx_chain_from_device:"
                   " could not get chain from dev %d", device ); 
       }
       #endif
       break;

     default:
       RF_MSG_1( RF_ERROR, "rfcommon_core_get_tx_chain_from_device:"
                   " invalid mode %d", device ); 
       break;
  }
  return modem_chain;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Converts tech into string
 
  @param mode

  @return
  tech string
*/
boolean rfcommon_core_get_tech_str( rfm_mode_enum_type mode , char str[] )
{
  boolean ret = TRUE;
  if(mode < RFM_INVALID_MODE )
  {
    strlcpy(str,mode_to_string_tbl[mode],MAX_TECH_STRING_SIZE);
  }
  else
  {
    ret = FALSE;
  }

  return ret;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Calls ftm function to build log packet for IRAT timing info
 
  @param num_trx_trans, num_non_trx_trans, estimated_exe_time

  @return
  
*/
void rfcommon_core_log_irat_timing_data ( uint16 num_trx_trans, uint16 num_non_trx_trans, uint16 estimated_exe_time )
{
  /*Debug only to be deleted*/
  MSG_3( MSG_SSID_RF, MSG_LEGACY_HIGH,
         "irat timing params to ftm: trx_trans %d, non_trx_trans %d, estimated exe time %d", num_trx_trans, num_non_trx_trans, estimated_exe_time );
  #ifdef FTM_HAS_LOGGING
  ftm_log_common_irat_timing( num_trx_trans, num_non_trx_trans, estimated_exe_time );
  #endif
}

/*----------------------------------------------------------------------------*/
rf_card_band_type rfcommon_core_convert_wcdma_band_to_rfc_band
(
  rfcom_wcdma_band_type band
)
{
  rf_card_band_type rc_band = RF_MAX_BAND;
  switch (band)
  {
  case RFCOM_BAND_IMT:
    rc_band = RF_BCI_BAND;
    break;
  case RFCOM_BAND_1900:
    rc_band = RF_BCII_BAND;
    break;
  case RFCOM_BAND_BC3:
    rc_band = RF_BCIII_BAND;
    break;
  case RFCOM_BAND_BC4:
    rc_band = RF_BCIV_BAND;
    break;
  case RFCOM_BAND_800:
    rc_band = RF_BCVI_BAND;
    break;
  case RFCOM_BAND_BC8:
    rc_band = RF_BCVIII_BAND;
    break;
  case RFCOM_BAND_BC9:
    rc_band = RF_BCIX_BAND;
    break;
  case RFCOM_BAND_BC11:
    rc_band = RF_BCXI_BAND;
    break;
  case RFCOM_BAND_BC19:
    rc_band = RF_BCXIX_BAND;
    break;
  default:
    RF_MSG_1( RF_ERROR, "rfcommon_core_convert_lte_band_to_rfc_band:"
              " unsupported lte_band %d",band );     
    break;
  }
  return rc_band;
}

/*----------------------------------------------------------------------------*/
rf_card_band_type rfcommon_core_convert_gsm_band_to_rfc_band
(
  rfcom_gsm_band_type band
)
{
  rf_card_band_type rc_band = RF_MAX_BAND;
  switch (band)
  {
  case RFCOM_BAND_GSM850:
    rc_band = RF_GSM_850_BAND;
    break;
  case RFCOM_BAND_GSM900:
    rc_band = RF_EGSM_BAND;
    break;
  case RFCOM_BAND_GSM1800_B:
  case RFCOM_BAND_GSM1800:
    rc_band = RF_DCS_BAND;
    break;
  case RFCOM_BAND_GSM1900:
    rc_band = RF_GSM_1900_BAND;
    break;
  default:
    RF_MSG_1( RF_ERROR, "rfcommon_core_convert_gsm_band_to_rfc_band:"
              " unsupported gsm_band %d",band );     
    break;
  }
  return rc_band;
}

/*----------------------------------------------------------------------------*/
rf_card_band_type rfcommon_core_convert_tds_band_to_rfc_band
(
  rfcom_tdscdma_band_type band
)
{
  rf_card_band_type rc_band = RF_MAX_BAND;
  switch (band)
  {
  case RFCOM_BAND_TDSCDMA_B34:
    rc_band = RF_TDSCDMA_B34_BAND;
    break;
  case RFCOM_BAND_TDSCDMA_B39:
    rc_band = RF_TDSCDMA_B39_BAND;
    break;
  case RFCOM_BAND_TDSCDMA_B40:
    rc_band = RF_TDSCDMA_B40_BAND;
    break;
  case RFCOM_BAND_TDSCDMA_B40_B:
    rc_band = RF_TDSCDMA_B40_B_BAND;
    break;
  case RFCOM_BAND_TDSCDMA_B34_B:
    rc_band = RF_TDSCDMA_B34_B_BAND;
    break;
  case RFCOM_BAND_TDSCDMA_B39_B:
    rc_band = RF_TDSCDMA_B39_B_BAND;
    break;
  default:
    RF_MSG_1( RF_ERROR, "rfcommon_core_convert_tds_band_to_rfc_band:"
              " unsupported tds_band %d",band );     
    break;
  }
  return rc_band;
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Utility function to convert sys band enum into corresponding tech specific
  rfcom band enum or rfm band enum type
 
  @param band
  sys band class enum info
 
  @return
  current band in rfcom band or rfm band enum format

*/
int rfcommon_core_convert_sys_band_to_rfcom_band(sys_band_class_e_type band)
{
  int rf_card_band = -1 ;

  switch(band)
  {
    /*--------------------- GSM bands ----------------*/
    case SYS_BAND_GSM_850:
      rf_card_band = RFCOM_BAND_GSM850;
      break;
    
    case SYS_BAND_GSM_EGSM_900:
    case SYS_BAND_GSM_PGSM_900:
    case SYS_BAND_GSM_RGSM_900:
      rf_card_band = RFCOM_BAND_GSM900;
      break;
    
    case SYS_BAND_GSM_DCS_1800:
      rf_card_band = RFCOM_BAND_GSM1800;
      break;
    
    case SYS_BAND_GSM_PCS_1900:
      rf_card_band = RFCOM_BAND_GSM1900;
      break;
    
    /*------------------- WCDMA bands ----------------*/
    case SYS_BAND_WCDMA_I_IMT_2000:
      rf_card_band = RFCOM_BAND_IMT;
      break;
    
    case SYS_BAND_WCDMA_II_PCS_1900:
      rf_card_band = RFCOM_BAND_1900;
      break;
    
    case SYS_BAND_WCDMA_III_1700:
      rf_card_band = RFCOM_BAND_BC3;
      break;
    
    case SYS_BAND_WCDMA_IV_1700:
      rf_card_band = RFCOM_BAND_BC4;
      break;
    
    case SYS_BAND_WCDMA_V_850:
      rf_card_band = RFCOM_BAND_800;
      break;
    
    case SYS_BAND_WCDMA_VI_800:
      rf_card_band = RFCOM_BAND_800;
      break;
    
    case SYS_BAND_WCDMA_VIII_900:
      rf_card_band = RFCOM_BAND_BC8;
      break;
    
    case SYS_BAND_WCDMA_IX_1700:
      rf_card_band = RFCOM_BAND_BC9;
      break;
    
    case SYS_BAND_WCDMA_XI_1500:
      rf_card_band = RFCOM_BAND_BC11;
      break; 
    
    case SYS_BAND_WCDMA_XIX_850:
      rf_card_band = RFCOM_BAND_BC19; 
      break; 
    
    /*--------------------- LTE bands ------------------ */
    case SYS_BAND_LTE_EUTRAN_BAND1: 
      rf_card_band = RFCOM_BAND_LTE_B1;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND2: 
      rf_card_band = RFCOM_BAND_LTE_B2;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND3: 
      rf_card_band = RFCOM_BAND_LTE_B3;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND4: 
      rf_card_band = RFCOM_BAND_LTE_B4;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND5: 
      rf_card_band = RFCOM_BAND_LTE_B5;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND6: 
      rf_card_band = RFCOM_BAND_LTE_B6;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND7: 
      rf_card_band = RFCOM_BAND_LTE_B7;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND8: 
      rf_card_band = RFCOM_BAND_LTE_B8;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND9: 
      rf_card_band = RFCOM_BAND_LTE_B9;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND10:
      rf_card_band = RFCOM_BAND_LTE_B10;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND11:
      rf_card_band = RFCOM_BAND_LTE_B11;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND12:
      rf_card_band = RFCOM_BAND_LTE_B12;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND13:
      rf_card_band = RFCOM_BAND_LTE_B13;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND14:
      rf_card_band = RFCOM_BAND_LTE_B14;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND17:
      rf_card_band = RFCOM_BAND_LTE_B17;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND18:
      rf_card_band = RFCOM_BAND_LTE_B18;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND19:
      rf_card_band = RFCOM_BAND_LTE_B19;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND20:
      rf_card_band = RFCOM_BAND_LTE_B20;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND21:
      rf_card_band = RFCOM_BAND_LTE_B21;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND23:
      rf_card_band = RFCOM_BAND_LTE_B23;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND25:
      rf_card_band = RFCOM_BAND_LTE_B25;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND26:
      rf_card_band = RFCOM_BAND_LTE_B26;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND27:
      rf_card_band = RFCOM_BAND_LTE_B27;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND28:
      rf_card_band = RFCOM_BAND_LTE_B28;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND29:
      rf_card_band = RFCOM_BAND_LTE_B29;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND30:
      rf_card_band = RFCOM_BAND_LTE_B30;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND33:
      rf_card_band = RFCOM_BAND_LTE_B33;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND34:
      rf_card_band = RFCOM_BAND_LTE_B34;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND35:
      rf_card_band = RFCOM_BAND_LTE_B35;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND36:
      rf_card_band = RFCOM_BAND_LTE_B36;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND37:
      rf_card_band = RFCOM_BAND_LTE_B37;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND38:
      rf_card_band = RFCOM_BAND_LTE_B38;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND39:
      rf_card_band = RFCOM_BAND_LTE_B39;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND40:
      rf_card_band = RFCOM_BAND_LTE_B40;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND41:
      rf_card_band = RFCOM_BAND_LTE_B41;
      break; 
    
    case SYS_BAND_LTE_EUTRAN_BAND66:
      rf_card_band = RFCOM_BAND_LTE_B66;
      break; 

    case SYS_BAND_LTE_EUTRAN_BAND125: 
#ifdef LTE_BAND_NUM 
    case SYS_BAND_LTE_EUTRAN_BAND252: 
#endif
      rf_card_band = RFCOM_BAND_LTE_B125; 
      break; 

    case SYS_BAND_LTE_EUTRAN_BAND126:
      rf_card_band = RFCOM_BAND_LTE_B126; 
      break;

    case SYS_BAND_LTE_EUTRAN_BAND127:
#ifdef LTE_BAND_NUM 
    case SYS_BAND_LTE_EUTRAN_BAND255: 
#endif
      rf_card_band = RFCOM_BAND_LTE_B127; 
      break;

    /*---------------------- TDSCDMA bands --------------------*/
    case SYS_BAND_TDS_BANDA:
      rf_card_band = RFCOM_BAND_TDSCDMA_B34;
      break; 
    
    case SYS_BAND_TDS_BANDF:
      rf_card_band = RFCOM_BAND_TDSCDMA_B39; 
      break; 
    
    case SYS_BAND_TDS_BANDE: 
      rf_card_band = RFCOM_BAND_TDSCDMA_B40; 
      break; 
    
    /*----------------------- CDMA bands ---------------------*/
    case SYS_BAND_BC0: 
      rf_card_band = RFM_CDMA_BC0; 
      break; 
                    
    case SYS_BAND_BC1:              
      rf_card_band = RFM_CDMA_BC1; 
      break; 
    
     case SYS_BAND_BC3:              
       rf_card_band = RFM_CDMA_BC3; 
       break; 
    
    case SYS_BAND_BC4:              
      rf_card_band = RFM_CDMA_BC4; 
      break; 
    
    case SYS_BAND_BC5:              
      rf_card_band = RFM_CDMA_BC5; 
      break; 
    
    case SYS_BAND_BC6:              
      rf_card_band = RFM_CDMA_BC6; 
      break; 
    
    case SYS_BAND_BC7:    
      rf_card_band = RFM_CDMA_BC7; 
      break; 
                
     case SYS_BAND_BC8:   
       rf_card_band = RFM_CDMA_BC8; 
       break; 
                  
    case SYS_BAND_BC9:              
      rf_card_band = RFM_CDMA_BC9; 
      break; 
    
    case SYS_BAND_BC10:             
      rf_card_band = RFM_CDMA_BC10; 
      break; 
    
    case SYS_BAND_BC11:             
      rf_card_band = RFM_CDMA_BC11; 
      break; 
    
    case SYS_BAND_BC12:             
      rf_card_band = RFM_CDMA_BC12; 
      break; 
    
    case SYS_BAND_BC13:             
      rf_card_band = RFM_CDMA_BC13; 
      break; 
    
    case SYS_BAND_BC14:             
      rf_card_band = RFM_CDMA_BC14; 
      break; 
    
    case SYS_BAND_BC15:             
      rf_card_band = RFM_CDMA_BC15; 
      break; 
    
    case SYS_BAND_BC16:             
      rf_card_band = RFM_CDMA_BC16; 
      break; 
    
    
    case SYS_BAND_BC17:             
      rf_card_band = RFM_CDMA_BC17; 
      break; 
    
    case SYS_BAND_BC18:             
      rf_card_band = RFM_CDMA_BC18; 
      break; 
    
    case SYS_BAND_BC19:             
      rf_card_band = RFM_CDMA_BC19; 
      break; 
    
    default:
      rf_card_band = -1; 
      break;
  }

  return( rf_card_band );
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Utility function to convert rfcom band enum into corresponding sys band enum 
 
  @param tech
  rfm_mode_enum_type

  @param band
  rfcom_band_type_u

  @return sys_band
  sys_band_class_e_type

*/


sys_band_class_e_type rfcommon_core_convert_rfcom_band_to_sys_band(rfm_mode_enum_type tech, rfcom_band_type_u band)
{
  sys_band_class_e_type sys_band = SYS_BAND_CLASS_NONE;
  switch(tech)
  {
  case RFM_IMT_MODE:	
    switch(band.wcdma_band)
    {
    case RFCOM_BAND_IMT:
      sys_band = SYS_BAND_WCDMA_I_IMT_2000;
      break;

    case RFCOM_BAND_1900:
      sys_band = SYS_BAND_WCDMA_II_PCS_1900;
      break;

    case RFCOM_BAND_BC3:
      sys_band = SYS_BAND_WCDMA_III_1700;
      break;
    
    case RFCOM_BAND_BC4:
      sys_band = SYS_BAND_WCDMA_IV_1700;
      break;

    case RFCOM_BAND_800:
    #ifdef FEATURE_WCDMA_850
      sys_band = SYS_BAND_WCDMA_V_850;
    #else
      sys_band = SYS_BAND_WCDMA_VI_800;
    #endif
      break;

    case RFCOM_BAND_BC8:
      sys_band = SYS_BAND_WCDMA_VIII_900;
      break;
		
    case RFCOM_BAND_BC9:
      sys_band = SYS_BAND_WCDMA_IX_1700;
      break;
		
    case RFCOM_BAND_BC11:
      sys_band = SYS_BAND_WCDMA_XI_1500;
      break;

    case RFCOM_BAND_BC19:
      sys_band = SYS_BAND_WCDMA_XIX_850;
      break;
					
    default:
      sys_band = SYS_BAND_WCDMA_I_IMT_2000;
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid WCDMA rfcom band. Default to SYS_BAND_WCDMA_I_IMT_2000");								
      break;

    }
    break;

  case RFM_EGSM_MODE:
  case RFM_EGSM_MODE_2:
  case RFM_EGSM_MODE_3:
    switch(band.gsm_band)
    {
    case RFCOM_BAND_GSM850:
      sys_band = SYS_BAND_GSM_850;
      break;
		
    case RFCOM_BAND_GSM900:
      sys_band = SYS_BAND_GSM_EGSM_900;
      break;
		
    case RFCOM_BAND_GSM1800:
    case RFCOM_BAND_GSM1800_B:
      sys_band = SYS_BAND_GSM_DCS_1800;
      break;

    case RFCOM_BAND_GSM1900:
      sys_band = SYS_BAND_GSM_PCS_1900;
      break;
				
    default:
      sys_band = SYS_BAND_GSM_850;
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid GSM rfcom band. Default to SYS_BAND_GSM_850");					
      break;
    }			
    break;
			
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
    switch(band.cdma_band)
	{
    case RFM_CDMA_BC0:
      sys_band = SYS_BAND_BC0;
      break;
    case RFM_CDMA_BC1:				
      sys_band = SYS_BAND_BC1;
      break;
    case RFM_CDMA_BC3:
      sys_band = SYS_BAND_BC3;
      break;
    case RFM_CDMA_BC4:
      sys_band = SYS_BAND_BC4;
      break;
    case RFM_CDMA_BC5:
      sys_band = SYS_BAND_BC5;
      break;
    case RFM_CDMA_BC6:
      sys_band = SYS_BAND_BC6;
      break;
    case RFM_CDMA_BC7:
      sys_band = SYS_BAND_BC7;
      break;
    case RFM_CDMA_BC8:
      sys_band = SYS_BAND_BC8;
      break;
    case RFM_CDMA_BC9:
      sys_band = SYS_BAND_BC9;
      break;
    case RFM_CDMA_BC10:
      sys_band = SYS_BAND_BC10;
      break;
    case RFM_CDMA_BC11:
      sys_band = SYS_BAND_BC11;
      break;
    case RFM_CDMA_BC12:
      sys_band = SYS_BAND_BC12;
      break;
    case RFM_CDMA_BC13:
      sys_band = SYS_BAND_BC13;
      break;
    case RFM_CDMA_BC14:
      sys_band = SYS_BAND_BC14;
      break;
    case RFM_CDMA_BC15:
      sys_band = SYS_BAND_BC15;
      break;
    case RFM_CDMA_BC16:
      sys_band = SYS_BAND_BC16;
      break;
    case RFM_CDMA_BC17:
      sys_band = SYS_BAND_BC17;
      break;
    case RFM_CDMA_BC18:
      sys_band = SYS_BAND_BC18;
      break;
    case RFM_CDMA_BC19:
      sys_band = SYS_BAND_BC19;
      break;
    case RFM_CDMA_BC20:
      sys_band = SYS_BAND_BC_MAX;
      break;
    default:
      sys_band = SYS_BAND_BC0;
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid CDMA rfcom band. Default to SYS_BAND_BC0");			
      break;
    }
    break;
	
  case RFM_LTE_MODE:
  case RFM_LTE_FTM_MODE:
    switch(band.lte_band)
    {
    case RFCOM_BAND_LTE_B1:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND1;
      break;
    case RFCOM_BAND_LTE_B2:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND2;
      break;
    case RFCOM_BAND_LTE_B3:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND3;
      break;
    case RFCOM_BAND_LTE_B4:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND4;
      break;
    case RFCOM_BAND_LTE_B5:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND5;
      break;
    case RFCOM_BAND_LTE_B6:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND6;
      break;
    case RFCOM_BAND_LTE_B7:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND7;
      break;
    case RFCOM_BAND_LTE_B8:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND8;
      break;
    case RFCOM_BAND_LTE_B9:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND9;
      break;
    case RFCOM_BAND_LTE_B10:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND10;
      break;
    case RFCOM_BAND_LTE_B11:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND11;
      break;
    case RFCOM_BAND_LTE_B12:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND12;
      break;
    case RFCOM_BAND_LTE_B13:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND13;
      break;
    case RFCOM_BAND_LTE_B14:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND14;
      break;
    case RFCOM_BAND_LTE_B17:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND17;
      break;
    case RFCOM_BAND_LTE_B18:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND18;
      break;
    case RFCOM_BAND_LTE_B19:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND19;
      break;
    case RFCOM_BAND_LTE_B20:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND20;
      break;
    case RFCOM_BAND_LTE_B21:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND21;
      break;
    case RFCOM_BAND_LTE_B23:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND23;
      break;
    case RFCOM_BAND_LTE_B24:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND24;
      break;
    case RFCOM_BAND_LTE_B25:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND25;
      break;
    case RFCOM_BAND_LTE_B26:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND26;
      break;
    case RFCOM_BAND_LTE_B27:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND27;
      break;
    case RFCOM_BAND_LTE_B28:
    case RFCOM_BAND_LTE_B28_B:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND28;
      break;
    case RFCOM_BAND_LTE_B29:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND29;
      break;
    case RFCOM_BAND_LTE_B30:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND30;
      break;
    case RFCOM_BAND_LTE_B31:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND31;
      break;
    case RFCOM_BAND_LTE_B33:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND33;
      break;
    case RFCOM_BAND_LTE_B34:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND34;
      break;
    case RFCOM_BAND_LTE_B35:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND35;
      break;
    case RFCOM_BAND_LTE_B36:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND36;
      break;
    case RFCOM_BAND_LTE_B37:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND37;
      break;
    case RFCOM_BAND_LTE_B38:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND38;
      break;
    case RFCOM_BAND_LTE_B39:
    case RFCOM_BAND_LTE_B39_B:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND39;
      break;
    case RFCOM_BAND_LTE_B40:
    case RFCOM_BAND_LTE_B40_B:					
      sys_band = SYS_BAND_LTE_EUTRAN_BAND40;
      break;
    case RFCOM_BAND_LTE_B41:
    case RFCOM_BAND_LTE_B41_B:
    case RFCOM_BAND_LTE_B41_C:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND41;
      break;
    case RFCOM_BAND_LTE_B42:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND42;
      break;
    case RFCOM_BAND_LTE_B43:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND43;
      break;
    case RFCOM_BAND_LTE_B66:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND66;
      break;
    case RFCOM_BAND_LTE_B125:
#ifdef LTE_BAND_NUM 
      sys_band = SYS_BAND_LTE_EUTRAN_BAND252; 
#endif
      break; 

    case RFCOM_BAND_LTE_B126:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND126; 
      break; 

    case RFCOM_BAND_LTE_B127:
#ifdef LTE_BAND_NUM 
      sys_band = SYS_BAND_LTE_EUTRAN_BAND255; 
#endif
      break; 

    default:
      sys_band = SYS_BAND_LTE_EUTRAN_BAND1;
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid LTE rfcom band. Default to SYS_BAND_LTE_EUTRAN_BAND1");
      break;
	}
	break;
			
  case RFM_TDSCDMA_MODE:
  case RFM_TDSCDMA_MODE_FTM:
    switch(band.tdscdma_band)
	{
    case RFCOM_BAND_TDSCDMA_B34:
    case RFCOM_BAND_TDSCDMA_B34_B:
      sys_band = SYS_BAND_TDS_BANDA;
      break;
    case RFCOM_BAND_TDSCDMA_B39:
    case RFCOM_BAND_TDSCDMA_B39_B:
      sys_band = SYS_BAND_TDS_BANDF;
      break;
    case RFCOM_BAND_TDSCDMA_B40:
    case RFCOM_BAND_TDSCDMA_B40_B:
      sys_band = SYS_BAND_TDS_BANDE;
      break;
    default:
      sys_band = SYS_BAND_TDS_BANDA;
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid TDS rfcom band. Default to SYS_BAND_TDS_BANDA");
      break;
    }
	break;
  default:
    sys_band = SYS_BAND_CLASS_NONE;
	MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "invalid tech and cannot result the band");
	break;
  }
  return sys_band;
  
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Common utility function to get the tx device pointer based on tech and band 
 
  @param device
  rfm_device_enum_type
 
  @param tech
  rfm_mode_enum_type

  @param band
  rfcom_band_type_u

  @return dev_ptr
  rfdevice_rxtx_common_class

*/
rfdevice_rxtx_common_class* rfcommon_core_get_tx_device_ptr
(
  rfm_device_enum_type device, 
  rfm_mode_enum_type tech, 
  rfcom_band_type_u band
)
{
  rfdevice_rxtx_common_class* dev_ptr = NULL; 
  #if defined(FEATURE_CDMA1X) || defined(FEATURE_HDR)
  const rfc_cdma_logical_device_params_type *logical_dev = NULL; 
  #endif
  #ifdef FEATURE_TDSCDMA
  rfdevice_trx_tdscdma_tx** tds_device = NULL;
  #endif
  uint8 device_idx = 0;

  switch (tech) 
  {

  #ifdef FEATURE_TDSCDMA
  case RFCOM_WCDMA_MODE:
    dev_ptr = rfdevice_wcdma_tx_get_common_dev_ptr(device, band.wcdma_band);
    break;
  #endif

  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    #if defined(FEATURE_CDMA1X) || defined(FEATURE_HDR)
    logical_dev = rfc_cdma_get_logical_device_params( device );
    if (logical_dev != NULL)
    {
      if (band.cdma_band < RFM_CDMA_MAX_BAND)
      {
        dev_ptr = rfdevice_cdma_tx_get_common_device_pointer(logical_dev->tx_device[band.cdma_band]);
      }
    }
    #endif
    break;

  #ifdef FEATURE_TDSCDMA
  case RFCOM_TDSCDMA_MODE:
    tds_device = (rfdevice_trx_tdscdma_tx**) rfc_tdscdma_get_rf_device_obj(device,
                                                                           RFC_CONFIG_TX,
                                                                           band.tdscdma_band,
                                                                           RFDEVICE_TRANSCEIVER);
    if (tds_device != NULL)
     {
       while (tds_device[device_idx] != NULL)
       {
         dev_ptr = rfdevice_tdscdma_tx_get_common_device_pointer(tds_device[device_idx]);
         device_idx++;
       }
     }
     else
     {
       MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
             "rfdevice_tdscdma_get_cmn_device_ptr fail: Null pointer detected. device: %d, band: %d",
             device, band.tdscdma_band);
     }
    break;
  #endif
  
  #ifdef FEATURE_LTE
  case RFCOM_LTE_MODE:
  case RFCOM_LTE_FTM_MODE:

    dev_ptr = rfdevice_lte_tx_get_common_dev_ptr(device, band.lte_band);
    break;

  #endif
  default:
    dev_ptr = NULL;
    break;
  }

  return dev_ptr;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Converts device to tx chain.
 
  @param device
  The device to convert to tx chain

  @return
  Corresponding tx chain for given device
*/
uint32
rfcommon_core_get_wtr_instance_from_device
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rfm_mode_enum_type current_mode
)
{
  rfc_device_info_type *device_info_ptr;
  rfc_cfg_params_type cfg;
  uint32 wtr_instance;

  cfg.alternate_path = 0;
  wtr_instance = 0;

  switch(current_mode)
  {
     case RFCOM_WCDMA_MODE:
       #ifdef FEATURE_WCDMA
       cfg.band = band;
       cfg.req = RFC_REQ_DEFAULT_GET_DATA;
       cfg.logical_device = device;
       cfg.rx_tx = RFC_CONFIG_TX;
       /* Get RFC information */
       if(rfc_wcdma_data_get_device_info(&cfg, &device_info_ptr) == TRUE)
       {
         wtr_instance = device_info_ptr->rf_asic_info[RFDEVICE_TRANSCEIVER].instance;
         RF_MSG_2( RF_LOW, "rfcommon_core_get_wtr_instance_from_device:"
                   " wtr_instance %d on dev %d", wtr_instance, device ); 
       }
       else
       {
         RF_MSG_1( RF_ERROR, "rfcommon_core_get_wtr_instance_from_device:"
                   " could not get chain from dev %d", device ); 
       }
       #endif
       break;

     default:
       RF_MSG_1( RF_ERROR, "rfcommon_core_get_wtr_instance_from_device:"
                   " invalid mode %d", current_mode ); 
       break;
  }
  
  return wtr_instance;
}
