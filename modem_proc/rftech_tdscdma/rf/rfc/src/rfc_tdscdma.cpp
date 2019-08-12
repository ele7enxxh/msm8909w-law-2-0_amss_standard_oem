/*!
   @file
   rfc_tdscdma.cpp

   @brief
   This file contains all the TDSCDMA mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2011 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/rfc/src/rfc_tdscdma.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
06/14/16   jr      Reduce F3 messages
05/08/16   jr      Reduce F3 messages
03/05/15   vv      API support for rfc to capture modem chain to trx and VCO path mapping 
11/18/14   dyc     Support updating rfc_asdiv_config_info table from RFC info
11/02/14   qzh     Get PA related timing from RFC file instead of using default value for GRFC PA
10/21/14   px      Added RFM Device support for PAPM
10/20/14   qzh     Add support for GRFC PA after bug fixing
10/17/14   al      Back out previous change
10/16/14   qzh     Add support for GRFC PA
10/01/14   ych     Updated port state for antenna switch config
09/16/14   sn      ASM set band RFM Device support
09/16/14   jyu     Need to push all device information into RFC 
08/22/14   ych     Swap IMM definition for Asdiv XSW
08/21/14   ych     Updated CW warning
08/20/14   jyu     Need to pass port_state info to update band port
07/10/14   jyu     Asd bringup with rfc changes and device script 
06/16/14   tks     Added support for get updated port config 
06/06/14   zg      Added device data pushing from RFC.
05/06/14   jps     TDS port switching fixes
05/02/14   jps     TDS port switching fixes
04/26/14   jyu     KW fix
04/09/14   yb      Added support for coupler
03/25/14   yb      Added support for linearizer
03/24/14   jyu     Porting over below changes
03/18/14   qzh     Handle the RFC_DEVICE_MISSING_FATAL scenario for RF daughter card support on TDS
03/14/14   qzh     Enlarge signal placeholder num from 20 to 30 and add protection for signal num overflow
02/11/14  as/sr    Re-design asdiv_tuner to support multiple RF configurations
02/10/14   kb      [XPT] Add support for porper shutdown of WTR during XPT deconfig.
02/07/14   jr      Fix compiler warning messages
01/22/14   ms      Fix filtered band query for GRFC_notime
01/22/14   nsh     Fix KW warnings
12/16/13   ms      RF alternate path support
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech
12/13/13   nsh     Fix compiler warnings
11/20/13   rp      Changes to support class based implementation for Common 
                   device driver.
10/24/13   jz      Lift PA bias limitation to follow the bias from NV/Calibration
10/31/13   shb     Converted TRX common device interface to C++
10/30/13   jps     Updates and fixes for common TXAGC hookup
10/23/13   qzh     Fix KW warnings
09/18/13   zg      Removed a unused local variable.
09/18/13   ms      Add api for getting filtered band
09/16/13   zg      Updates to support C++ class based TDSCDMA RX and TX interface
09/09/13   ms      Add support for AsDiv tuner manager
07/26/13   ms      AsDiv support
05/20/13   kai     Get the feedback path attenuation state from rfc
05/20/13   al      Add function to support split band
05/09/13   vb      Bug fix: the Tuner Manager instance not correct for diversity path
05/06/13   vb      Bug fix for assiging the Tuner port object
05/06/13   kb      [XPT] Support to set up feedback path and device for EPT
05/06/13   vb      Support for Tuner manager factory
05/02/13   vb      KW Fix: Check for Tuner Manager validity
05/02/13   vb      Tuner CA changes 
03/29/13   jyu     Change to use RFM_MAX_WAN_DEVICES and 
                   RFC_MAX_DEVICE_INSTANCES_PER_BAND
03/26/13   jyu     Changed to PAPM device interface
03/21/13   ms      Tuner TDET support
03/05/13   vrb     Replace RFC accessor param init_req boolean by a req enum
02/07/13   jyu     Fixed a bug when initializing device ptr for transceiver
12/21/12   jyu     Modified the get_rf_device_obj function to return double pointer  
12/11/12   ms      KW fix
12/06/12   ms      APT support
11/19/12   jyu     Added initial Tuner support for TDSCDMA to enable HDET
11/19/12   ms      KW fix
10/30/12   sr      changes create tech rfc object if the tech is supported by 
                   the specific rf-card.
09/26/12   jyu     Changed to use logic device instead of logic path 
09/26/12   jyu     Added NULL ptr check before accessing 
09/14/12   jyu     Added interface to query TxLUT from RFC 
09/08/12   sr      Fixed bug to avoid crash (set ending flag to signal list)
08/24/12   jyu     Integrated power tracker device 
08/23/12   jyu     Integrated ASM and PA driver 
08/20/12   jyu     Added support to set/clean specific GRFC (immediate execution)
08/20/12   jyu     Added support to cleanup GRFCs 
08/13/12   jyu     Updated to support push device_ptr 
08/10/12   vrb     Update Band Port Enum to index 1 as per RFC AG Update
08/03/12   sr      initialized the rf_devices[] ptrs .
07/31/12   jyu     Fixed compilation errors 
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type.
07/17/12   sr      rfc clean-up for dime
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
07/02/12   sr      renamed rfc_common_gsmctl.h to rfc_common_tdd.h as it is shared with TDS.
06/29/12   sr      rfc api name change.
04/12/12   ycl     Added Sleep/Wakeup.
08/15/11   zg      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfc_card.h"
#include "rfc_tdscdma.h"
#include "rfc_common_tdd.h" 
#include "rfc_tdscdma_data.h"
#include "rfc_class.h"
#include "rfc_vreg_mgr.h"
#include "rfdevice_rxtx_common_class.h"
#include "rfdevice_tdscdma_intf.h"
#include "rfdevice_asm.h"
#include "rfdevice_papm.h"
#include "rfdevice_pa.h"
#include "rfdevice_coupler.h"
#include "rfcommon_atuner_manager_factory.h"
#include "rfc_common.h"
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma_mc.h"
#include "rfdevice_xsw.h"
#include "rfc_common_grfc_pa.h"

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_tuner_manager.h"
#endif
#include "rfcommon_atuner_manager_intf.h"
#include "assert.h"



#define SIG_LIST_NOTIME_MAX_LENGTH 30  // Maximal length of all signal lists
                                       // without timing.

extern int32 rfc_common_get_signal_num(int32 rf_sig_name, rfc_signal_type sig_type);


/* ----------------------------------------------------------------------- */
/*!
   @brief
   RF Card TDSCDMA CORE configuration table.

   @details
   Contains the static values for the CORE parameters for TDSCDMA mode of operation.
   The structure and type is specific to this RF card.
*/
rfc_tdscdma_core_config_type  rfc_tdscdma_core_config_table =
{
  NULL,  //*rx_timing
  TRUE,  //pwr_off_in_sleep
  500,   //rf_warmup_time
  {TRUE,3400,1023,96}, //enable,smps_default,smps_idle,over_drive_dur
};

/* ----------------------------------------------------------------------- */
/*!
   @brief
   Placeholder of RF Card TDSCDMA rfc signal list without timing.

   @details
*/
rfc_sig_cfg_type rf_card_sig_list_notime[SIG_LIST_NOTIME_MAX_LENGTH]; 


rfc_tdscdma *rfc_tdscdma::rfc_tdscdma_ptr = (rfc_tdscdma *)NULL;

/* Pointer to device class to hold common object */
static rfdevice_class *rfc_tdscdma_common_grfc_pa_device = NULL;

rfc_tdscdma * rfc_tdscdma::get_instance(void)
{
  return rfc_tdscdma_ptr;
}

// create singleton object.
rfc_tdscdma * rfc_tdscdma::create_instance(void)
{
  if (rfc_tdscdma_ptr == NULL)
  {
    rfc_tdscdma_ptr = (rfc_tdscdma *)new rfc_tdscdma();
  }

  return rfc_tdscdma_ptr;
}


// Constructor 
rfc_tdscdma::rfc_tdscdma()
{
  uint8 log_dev = 0;
  uint8 rx_tx = 0;
  uint8 band = 0;
  uint8 dev_type = 0;
  uint8 dev_instance = 0;

  /* Initialize the device init flag */
  device_init_complete_status = TRUE;

  // initialize the tdscdma_rf_devices
  for (log_dev = 0; log_dev < RFM_MAX_WAN_DEVICES; log_dev++)
  {
    for(rx_tx = 0; rx_tx < RFC_CONFIG_RXTX_MAX; rx_tx++)
    {
      for (band = 0; band < RFCOM_NUM_TDSCDMA_BANDS; band++)
      {
        for(dev_type = 0; dev_type < RFDEVICE_TYPE_MAX_NUM; dev_type++)
        {
          for (dev_instance = 0; dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND; dev_instance++)
          {
            tdscdma_rf_devices[log_dev][rx_tx][band][dev_type][dev_instance] = NULL;
          }
        }
      }
    }
  }

  device_init_complete_status &= init_rfdevices_for_all_bands();
}


// Destructor
rfc_tdscdma::~rfc_tdscdma()
{
  rfc_tdscdma_ptr = NULL;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific intializations for band
   changes.

   @details
   The band table which this RF card supports is not indexed in any way.
   Explicit logic will need to be provided to extract the band specific data
   structures.

*/
void rfc_tdscdma::rx_band_config(rfm_device_enum_type device,
                                 rfcom_tdscdma_band_type rftdscdma_band)
{

  MSG_2(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED,"Select TDSCDMA rx band enum %d on device%d",
         rftdscdma_band, device);

} /* rfc_tdscdma::rx_band_config() */



/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific intializations for band
   changes.

   @details
   The band table which this RF card supports is not indexed in any way.
   Explicit logic will need to be provided to extract the band specific data
   structures.

*/
void rfc_tdscdma::tx_band_config(rfm_device_enum_type device,
                                 rfcom_tdscdma_band_type rftdscdma_band)
{

  MSG_2(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED,"Select TDSCDMA tx band enum %d on device%d", 
        rftdscdma_band, device);

} /* rfc_tdscdma::tx_band_config() */


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific intializations for TDSCDMA mode

   @details

*/

void rfc_tdscdma::config_mode( rfm_device_enum_type device )
{
  MSG_1(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED,"Select TDSCDMA mode device%d", device);

  /* configure the initial rx GRFCs */
  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_NUM_TDSCDMA_BANDS, RFC_REQ_INIT};
  rfc_sig_cfg_type *rf_signals = NULL;

  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return;
  }

  cfg.alternate_path = 0;
  cfg.logical_device = device;

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }

  /* configure the initial Rx GRFCs */
  rfc_data->sig_cfg_data_get( &cfg, &rf_signals );
  rfc_common_tdd_config_signal ( rf_signals );

  /* configure the initial tx GRFCs */
  cfg.rx_tx = RFC_CONFIG_TX;  
  rfc_data->sig_cfg_data_get( &cfg, &rf_signals );
  rfc_common_tdd_config_signal ( rf_signals );

} /* rfc_tdscdma::config_mode() */



/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific meas data

   @details

*/
void *rfc_tdscdma::rx_band_config_get(rfm_device_enum_type device, 
                                      rfcom_tdscdma_band_type rftdscdma_band,
                                      rfc_band_config_data_enum_type config_data)
{
  void* ret_config_ptr = NULL;

  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_NUM_TDSCDMA_BANDS, RFC_REQ_DEFAULT_GET_DATA, RFC_PORT_STATE_A};
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return( ret_config_ptr );
  }

  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.band = rftdscdma_band;

  
  if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
  {
    cfg.port_state= RFC_PORT_STATE_B;
  }
  
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  if (NULL == rf_signals)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unsupported TDSCDMA band %d", rftdscdma_band);   
    return( ret_config_ptr );       
  }

  /* Retrieve requested data if exists */
  switch ( config_data )
  {
  case RFC_GPIO_DATA:
  case RFC_GRFC_DATA:
  case RFC_RF_CTRL_DATA:
    ret_config_ptr = (void *)(rf_signals);
    break;

    case RFC_CORE_DATA:
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr );
      if (NULL != device_info_ptr)
      {
        ret_config_ptr = (void *)(&(device_info_ptr->rf_asic_info[0].data[1]));
      }
    break;

  default :
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unsupported cfg_data type %d", config_data);          
    break;
  }

  //MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,"Get TDSCDMA rx config: band enum%d on device%d, port is %d ", 
  //      rftdscdma_band, device, cfg.port_state);

  return( ret_config_ptr );
}


void *rfc_tdscdma::tx_band_config_get(rfm_device_enum_type device, 
                                      rfcom_tdscdma_band_type rftdscdma_band,
                                      rfc_band_config_data_enum_type config_data)
{
  void* ret_config_ptr = NULL;

  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_TX, RFCOM_NUM_TDSCDMA_BANDS, RFC_REQ_DEFAULT_GET_DATA, RFC_PORT_STATE_A};
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_device_info_type *device_info_ptr = NULL;

  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return( ret_config_ptr );
  }

  rftdscdma_band=rf_tdscdma_core_util_get_filtered_band(rftdscdma_band);
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.band = rftdscdma_band;

  if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
  {
    cfg.port_state= RFC_PORT_STATE_B;
  }
  
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  if (NULL == rf_signals)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unsupported TDSCDMA band %d", rftdscdma_band);   
    return( ret_config_ptr );       
  }

  /* Retrieve requested data if exists */
  switch( config_data )
  {
    case RFC_GPIO_DATA:
    case RFC_GRFC_DATA:
    case RFC_RF_CTRL_DATA:
      ret_config_ptr = (void *)(rf_signals);
      break;

    case RFC_CORE_DATA:
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr );
      if (NULL != device_info_ptr)
      {
        ret_config_ptr = (void *)(&(device_info_ptr->rf_asic_info[0].data[1]));
      }

      break;

    default :
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unsupported cfg_data type %d", config_data);          
      break;
  }

  MSG_3(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED,"Get TDSCDMA tx config: band enum%d on device%d, port is %d", 
        rftdscdma_band, device, cfg.port_state);

  return( ret_config_ptr );
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the card specific meas data

   @details

*/
void *rfc_tdscdma::get_mode_config_data(rfm_device_enum_type device, 
                                        rfc_mode_config_data_enum_type config_data)
{
  void* data_config_ptr = NULL;

  switch (config_data)
  {  
    case RFC_MODE_CORE_DATA:
      data_config_ptr = (void *)&rfc_tdscdma_core_config_table;
      break;

    default:            /* Invalid mode operation */
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unknown RFC data request in get_mode_config_data", 0);
      break;
  }

  MSG_2(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED,"Get TDSCDMA mode config: config enum%d on device%d", 
        config_data, device);

  return( data_config_ptr );
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the GRFC direct write for a specific band changes.
 
*/
void rfc_tdscdma::band_config_grfc_notime(rfm_device_enum_type device, rfcom_tdscdma_band_type band,
                                          rfc_signal_type sig_type, rfc_rxtx_enum_type rx_tx, boolean set_grfc, rfc_logic_type override_logic)
{
   uint32 i = 0;
   int32 signal_num = -1;
   rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_RXTX_INVALID, RFCOM_NUM_TDSCDMA_BANDS, RFC_REQ_DEFAULT_GET_DATA};
   rfc_sig_cfg_type *rf_signals = NULL;
   rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();

   //MSG_3( MSG_SSID_RF,MSG_LEGACY_MED,
   //      "TDS band_config_grfc_notime Debug: device%d, band=%d, rx_tx=%d ", device, band, rx_tx);

   if (rfc_data == NULL)
   {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
     return;
   }

   cfg.alternate_path = 0;
   cfg.logical_device = device;
   cfg.band = band;
   cfg.rx_tx = rx_tx;

     if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
     {
       cfg.port_state= RFC_PORT_STATE_B;
     }

   rfc_data->sig_cfg_data_get(&cfg, &rf_signals );

   /* signal conversion from rfc_signal_gsm_cfg_type to rfc_signal_cfg_type */
   if ( rf_signals != NULL )
   {
     /* loop through the signal list and configure them based on their type to specified logic  */
     do
     {
       if ( i >= SIG_LIST_NOTIME_MAX_LENGTH ) /* test if the placeholder is long enough */
       {
         MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,
               "GRFC notime list overflow (device %d, band = %d, rx_tx = %d)", device, band, rx_tx);   
         /* Add assert here to catch placeholder num is not enough for signal num scenario, to avoid unexpect RF behavior */
         ASSERT(0);
       }
       else /* convert to the data list without timing information */ 
       {
           if (rf_signals[i].sig_name == RFC_SIG_LIST_END)
           {
             rf_card_sig_list_notime[i].sig_name = rf_signals[i].sig_name;
             break;
           }

           rf_card_sig_list_notime[i].sig_name 
                                    = rf_signals[i].sig_name; 
           if(set_grfc)
           {
             // set_grfc = TRUE: set GRFCs using start.logic
             rf_card_sig_list_notime[i].start.logic 
                                      = rf_signals[i].start.logic;
           }
           else
           {
             // set_grfc = FLASE: clean GRFCs using stop.logic
             rf_card_sig_list_notime[i].stop.logic 
                                      = rf_signals[i].stop.logic;
           }

           signal_num = rfc_common_get_signal_num(rf_signals[i].sig_name, sig_type);
           if(signal_num >= 0)
           {
             if((override_logic == RFC_LOW) || (override_logic == RFC_HIGH))
             {
               // Use the override logic
             rf_card_sig_list_notime[i].start.logic = override_logic;
             MSG_3(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED,"RFC TDSCDMA: Override the signal=%d (type %d) with logic %d", rf_signals[i].sig_name, sig_type, override_logic);
               }
             }

         //MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,"RFC TDSCDMA Debug: No: %d, signal_name %d, logic %d", i, rf_card_sig_list_notime[i].sig_name, rf_card_sig_list_notime[i].logic);
       }
       i++ ;

     } while ( rf_signals[i-1].sig_name != RFC_SIG_LIST_END );

     /* Configuer the specified GRFC signals to HW */
     rfc_common_config_signal( rf_card_sig_list_notime );
   }
}

int32 rfc_tdscdma::command_dispatch( rfm_device_enum_type device, int32 cmd, void *data )
{
  switch (cmd)
  {
    case RFC_TDSCDMA_SET_RX_GRFC_NO_TIME:
      {
        rfc_tdscdma_grfc_notime_control_type* tds_grfc_notime_control_ptr;  

        tds_grfc_notime_control_ptr = (rfc_tdscdma_grfc_notime_control_type*)data;
        band_config_grfc_notime ( device, tds_grfc_notime_control_ptr->band, tds_grfc_notime_control_ptr->sig_type, RFC_CONFIG_RX, TRUE,  tds_grfc_notime_control_ptr->override_logic);
      }
      break;

    case RFC_TDSCDMA_SET_TX_GRFC_NO_TIME:
      {
        rfc_tdscdma_grfc_notime_control_type* tds_grfc_notime_control_ptr;  

        tds_grfc_notime_control_ptr = (rfc_tdscdma_grfc_notime_control_type*)data;
        tds_grfc_notime_control_ptr->band = rf_tdscdma_core_util_get_filtered_band(tds_grfc_notime_control_ptr->band);

        band_config_grfc_notime ( device, tds_grfc_notime_control_ptr->band, tds_grfc_notime_control_ptr->sig_type, RFC_CONFIG_TX, TRUE, tds_grfc_notime_control_ptr->override_logic );
      }
      break;

    case RFC_TDSCDMA_CLEAN_RX_GRFC_NO_TIME:
      {
        rfc_tdscdma_grfc_notime_control_type* tds_grfc_notime_control_ptr;  

        tds_grfc_notime_control_ptr = (rfc_tdscdma_grfc_notime_control_type*)data;
        band_config_grfc_notime ( device, tds_grfc_notime_control_ptr->band, tds_grfc_notime_control_ptr->sig_type, RFC_CONFIG_RX, FALSE, tds_grfc_notime_control_ptr->override_logic );
      }
      break;

    case RFC_TDSCDMA_CLEAN_TX_GRFC_NO_TIME:
      {
        rfc_tdscdma_grfc_notime_control_type* tds_grfc_notime_control_ptr;  

        tds_grfc_notime_control_ptr = (rfc_tdscdma_grfc_notime_control_type*)data;
        tds_grfc_notime_control_ptr->band = rf_tdscdma_core_util_get_filtered_band(tds_grfc_notime_control_ptr->band);

        band_config_grfc_notime ( device, tds_grfc_notime_control_ptr->band, tds_grfc_notime_control_ptr->sig_type, RFC_CONFIG_TX, FALSE, tds_grfc_notime_control_ptr->override_logic );
      }
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"unknow RFC TDSCDMA command (cmd = %d)",cmd);    
      break;
  }

  return 1;
}

void rfc_tdscdma::sleep( rfm_device_enum_type device )
{
  rfc_cfg_params_type cfg = { RF_PATH_0,  RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_NUM_TDSCDMA_BANDS, RFC_REQ_INIT};
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return;
  }

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }


  /* set the tdscdma Rx init_table GRFC settings into default state */
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_tdd_init_default(rf_signals);

  /* set the tdscdma Tx init_table GRFC settings into default state */
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_tdd_init_default(rf_signals);

}

void rfc_tdscdma::wakeup(rfm_device_enum_type device)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_NUM_TDSCDMA_BANDS, RFC_REQ_INIT};
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return;
  }

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }

  /* Set the tdscdma rx init_table GRFC settings into init state */
  cfg.alternate_path = 0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.logical_device = device;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_tdd_config_signal(rf_signals);

  /* Set the tdscdma tx init_table GRFC settings into init state */
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_tdd_config_signal(rf_signals);

}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements VREG management for TDSCDMA mode

   @details

*/
void rfc_tdscdma::manage_vregs(rfc_power_management_info* pm_info)
{
  rfc_vreg_param vreg_param(pm_info->path, pm_info->mode);
  vreg_param.set_band(pm_info->band);
  vreg_param.set_rfchip_number(pm_info->rf_chip_num);
  vreg_param.set_rf_state(pm_info->state);

  rfc_vreg_mgr *vreg_mgr_p = rfc_vreg_mgr::get_instance();
  if (vreg_mgr_p != NULL)
  {
    vreg_mgr_p->tdscdma_manage_power(vreg_param);
  }
  else
  {
    ERR_FATAL("Cannot Create VREG_MGR for for TDSCDMA",0,0,0);
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the device object ptr for the specified logic device,
  rx or tx, band and device type.

  @details
  This function returns the device object ptr for the specified logic device
  rx or tx, band and device type.

*/
void** rfc_tdscdma::get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                      rfcom_tdscdma_band_type band, rfdevice_type_enum_type dev_type)
{
  void** dev_obj = NULL;
  uint8 antenna = 0;
  rfdevice_class *asd_tuner_mgr = NULL;


  if ( ! ( ( dev_type >= RFDEVICE_TRANSCEIVER ) && ( dev_type < RFDEVICE_TYPE_MAX_NUM ) ) )
  {
    dev_type = RFDEVICE_TYPE_MAX_NUM;
  }

  if (rfm_dev >= RFM_MAX_WAN_DEVICES || rx_tx >= RFC_CONFIG_RXTX_MAX 
      || band >= RFCOM_NUM_TDSCDMA_BANDS || dev_type >= RFDEVICE_TYPE_MAX_NUM)
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma::get_device_obj(%d, %d, %d, %d): Incorrect Parameters",
          rfm_dev, rx_tx, band, dev_type);
    return dev_obj;
  }

  dev_obj = tdscdma_rf_devices[rfm_dev][rx_tx][band][dev_type];

#ifdef FEATURE_RF_ASDIV
  /** If Asdiv Tuner Manager is present get the correct list of device objects 
      based on the ASDiv Switch position for specific Antenna */

  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  asd_tuner_mgr = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_ASD_TUNER_MANAGER, 0); 

  if ((dev_type == RFDEVICE_TUNER_MANAGER) && (asd_tuner_mgr!= NULL))
  {
    antenna = ant_params[rfm_dev][rx_tx][band].antenna_num ;
    dev_obj = (void **)(((rfcommon_asdiv_tuner_manager*)asd_tuner_mgr)->get_asdiv_device_list(antenna, dev_type, (rfdevice_class **)dev_obj));
  }
#endif /*FEATURE_RF_ASDIV*/

 return dev_obj;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns band-specific pa lut type information.

  @details
  This function returns band-specific pa lut type information.

*/
void rfc_tdscdma::get_band_pa_lut_type(rfm_device_enum_type device, rfcom_tdscdma_band_type band, uint32* rf_asic_band_pa_lut_map)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_TX, RFCOM_NUM_TDSCDMA_BANDS, RFC_REQ_DEFAULT_GET_DATA};  
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();

  if ( (device >= RFM_MAX_WAN_DEVICES) || (band >= RFCOM_NUM_TDSCDMA_BANDS) )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfc_tdscdma::get_band_pa_lut_type(device=%d, band=%d): Incorrect Parameters",
          device, band);
    return ;
  }

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFC TDSCDMA Data Object is NOT Created", 0);
    return;
  }

  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.band = band;

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }


  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr );

  if (NULL != device_info_ptr)
  {
    // This portion is RFC format specific. Need to pay attention for every PL and sanity check
    *rf_asic_band_pa_lut_map = device_info_ptr->rf_asic_info[0].data[2];

    MSG_3(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, "TxLUT type %d retrived for band %d and device %d", *rf_asic_band_pa_lut_map, band, device);
  }
  else
  {
    MSG_2(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, "Empty data for band %d and device %d", band, device);
  }
  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the rfdevices with the rfc dependent data.

  @details
  This function initializes the rfdevices with the rfc dependent data.

*/
boolean rfc_tdscdma::init_rf_devices(rfc_cfg_params_type *cfg, rfc_device_info_type *device_info_ptr)
{
  rfdevice_class *cmn_dev_obj = NULL;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  uint32 instance = 0;
  uint8 i = 0;
  rfcom_band_type_u band_u;
  //rfcmn_device_type *trx_cmn_device = NULL;
  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  uint8 trx_instance  = 0;
  uint8 pa_instance   = 0;
  uint8 asm_instance  = 0;
  uint8 papm_instance = 0; 
  uint8 tuner_instance = 0; 
  uint8 hdet_instance = 0;
  uint8 coupler_instance = 0;
  uint8 xsw_instance  = 0;
  rfdevice_trx_tdscdma_rx *rx_dev_obj = NULL;
  rfdevice_trx_tdscdma_tx *tx_dev_obj = NULL;
  uint8 ant_num = 0;
  rfdevice_class *asd_tuner_mgr = NULL;
  uint8 temp_port_state = RFC_PORT_STATE_DEFAULT;
  rfc_device_availability_enum_type device_status;
  rfdevice_trx_phy_path_enum_type phy_path= RFDEVICE_TRX_MAX_PATHS;
  /*For GRFC PA*/
  rfc_sig_cfg_type* mode_rf_signals = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();

  if (NULL == rfc_cmn)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC CMN Object is NOT Created", 0);
    return FALSE;
  }
  
  if (NULL == cfg || NULL == device_info_ptr)
  {
    /* band is not supported */
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"init_rf_devices(0x%x, 0x%x) : Bad parameters!", cfg, device_info_ptr);
    return FALSE;
  }

  if(cfg->band >= RFCOM_NUM_TDSCDMA_BANDS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid band %d!", cfg->band);
    return FALSE;  
  }

  //RF_MSG_3(RF_HIGH, "DEBUG portA/B=%d, band=%d, alt_path=%d", cfg->port_state, cfg->band, cfg->alternate_path); 

  if (cfg->port_state == RFC_PORT_STATE_A)
  {
    temp_port_state = RFC_PORT_STATE_A;

    ant_num = device_info_ptr->ant_num;

    while (i < device_info_ptr->num_devices)
    {
      dev_type = device_info_ptr->rf_asic_info[i].device_type;
      instance = device_info_ptr->rf_asic_info[i].instance;
      cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(dev_type, instance);
      phy_path = (rfdevice_trx_phy_path_enum_type) device_info_ptr->rf_asic_info[i].phy_path_num; 
     
      device_status = rfc_cmn->get_cmn_rf_device_status(dev_type, (uint8)instance);
         
      switch(dev_type)
      {
        case RFDEVICE_TRANSCEIVER:
          {
			band_u.tdscdma_band = (rfcom_tdscdma_band_type)cfg->band;
            if (cmn_dev_obj != NULL)
            {
                if ( cfg->rx_tx == RFC_CONFIG_RX) /* rx device data */
                {              
                /* Confirm we have a valid Primary or Secondary receive path */  
                if(cfg->logical_device >= RFM_DEVICE_0 && cfg->logical_device <= RFM_DEVICE_3 )
                  {
                    if (trx_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
                    {
                      if (cfg->logical_device == RFM_DEVICE_0 || cfg->logical_device == RFM_DEVICE_2)
                      {
                        phy_path = RFDEVICE_TRX_PATH_0;
                      }
                      else
                      {
                        phy_path =  RFDEVICE_TRX_PATH_1; 
                      }
                      /* Get Rx device ptr */
                      rx_dev_obj = (rfdevice_trx_tdscdma_rx *)rfdevice_cmn_get_tech_device(
                                                         (rfdevice_rxtx_common_class*)cmn_dev_obj,
                                                                                     RFM_TDSCDMA_MODE, 
                                                                     RFDEVICE_RX, 
                                                          phy_path);

                      tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][trx_instance] 
                                                                = rx_dev_obj;

                    /* Push Rx band specific data to TRX reciever */
                    rfdevice_tdscdma_rx_set_band_data( rx_dev_obj,
                                                      (rfcom_tdscdma_band_type)cfg->band,
                                                      device_info_ptr->rf_asic_info[i].data, 
                                                      RFC_ASIC_INFO_DATA_SIZE);

		      MSG_3( MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, "DEBUG rx_set_band phy_path=%d, band=%d, alt_path=%d", phy_path,cfg->band, cfg->alternate_path);

                      trx_instance++;
                    }
                  else /*if (trx_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)*/
                    {
                    RF_MSG_1(RF_FATAL, "Unsupported number of tranceiver/rx instances %d!", trx_instance); 
                    }
                  }
                else /*if(cfg->logical_device >= RFM_DEVICE_0 && cfg->logical_device <= RFM_DEVICE_3 )*/
                {
                  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Incorrect logical device %d specified!", cfg->logical_device);
                }
              } /*if ( cfg->rx_tx == RFC_CONFIG_RX)*/
    
              else if(cfg->rx_tx == RFC_CONFIG_TX) /* tx device data */
               {
                 if (trx_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
                 {
                   /* Path 0 is for Tx chain */
                   tx_dev_obj = (rfdevice_trx_tdscdma_tx *)rfdevice_cmn_get_tech_device( 
                                                   (rfdevice_rxtx_common_class*)cmn_dev_obj,
                                                                                RFM_TDSCDMA_MODE, 
                                                                    RFDEVICE_TX, 
                                                    phy_path);

                    tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][trx_instance] 
                                                             = tx_dev_obj;
                  // Temporarily add rfdevice_rxtx_common_class* type under dev_type = RFDEVICE_TRANSMITTER
                  tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TRANSMITTER][trx_instance] 
                                                           = cmn_dev_obj;

                    /* Push Tx band specific data to TRX reciever */
                    RF_MSG_2(RF_ERROR, "DEBUG tx_set_band phy_path=%d, band=%d", phy_path,cfg->band);  
                    rfdevice_tdscdma_tx_set_band_data( tx_dev_obj,
                                                       (rfcom_tdscdma_band_type)cfg->band,
                                                       device_info_ptr->rf_asic_info[i].data, 
                                                       RFC_ASIC_INFO_DATA_SIZE);
                    trx_instance++;  
                  }
                  else
                  {
                    RF_MSG_1(RF_FATAL, "Unsupported number of tranceiver/tx instances!", 0);   
                  }
              } /* end of else if(cfg->rx_tx == RFC_CONFIG_TX) */
            }/* end of if (cmn_dev_obj != NULL)*/
              else
              {
              MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma::init_rf_devices(), rftdscdma_device_list is NULL for logical dev: %d, tx/rx:%d, band:%d",
                    cfg->logical_device, cfg->rx_tx, cfg->band);
              
              /*If the device is present in RFC but has NULL physical ptr, then we return a fatal error during rfm_init */
              if (device_status == RFC_DEVICE_MISSING_FATAL)
              {
                RF_MSG_3(RF_FATAL, "Failed to map device! band = %d dev_type = %d instance = %d", 
                         cfg->band, dev_type, trx_instance);          
                return FALSE;
              }     
            }
		     if (cfg->rx_tx == RFC_CONFIG_RX)
             {
             /*call the below api to capture the transciever->phy path->modem chain(ADC) mapping to perform adc dc cal.*/
             if(!rfc_cmn->set_trx_phy_path_adc_mapping(
                                                  device_info_ptr->modem_chain,                    /*modem chain to map the ADC*/
                                                  device_info_ptr->rf_asic_info[i].phy_path_num,   /*physical path*/
                                                  device_info_ptr->rf_asic_info[i].instance,       /*Transciever device type instance*/
                                                  RFM_TDSCDMA_MODE,                                /*tech*/
                                                  band_u,                                         /*band*/
                                                  (rfdevice_class*)rx_dev_obj,                     /*rx device pointer*/
                                                  device_info_ptr->rf_asic_info[i].data,           /*port info*/
                                                  RFC_ASIC_INFO_DATA_SIZE                          /*data size*/
                                                 )
              )
              {
                 RF_MSG_3( RF_ERROR, "rfc_cmn->set_trx_phy_path_adc_mapping() failed. check rfc"
                                "modem_chain=%d,phy_path_num=%d, trx logical instance=%d  ",
                                device_info_ptr->modem_chain, 
                                device_info_ptr->rf_asic_info[i].phy_path_num,
                                device_info_ptr->rf_asic_info[i].instance
                               );
              }
             }
          }
          break;

        case RFDEVICE_ASM:
          if (cmn_dev_obj != NULL)
          {
            if (asm_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
            {
              tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][asm_instance] = cmn_dev_obj;
              asm_instance++;
            }
            else
            {
              RF_MSG_1(RF_FATAL, "Unsupported number of asm instances!", 0); 
            }

            band_u.tdscdma_band = (rfcom_tdscdma_band_type)cfg->band;
  
            if(cfg->rx_tx == RFC_CONFIG_RX)
            {
              /* Update the Rx ASM Port state with temp_port_state and Call the ASM API below */
              ((rfdevice_asm *)cmn_dev_obj)->set_rx_switch_state(RFCOM_TDSCDMA_MODE,
                                                      band_u,
                                                      temp_port_state);
              ((rfdevice_asm *)cmn_dev_obj)->set_rx_band_map(cfg->logical_device,
                                                             RFCOM_TDSCDMA_MODE, 
                                                             band_u, 
                                                             device_info_ptr->rf_asic_info[i].data, 
                                                             RFC_ASIC_INFO_DATA_SIZE
                                                             );
            }
            else if (cfg->rx_tx == RFC_CONFIG_TX)
            {
              ((rfdevice_asm *)cmn_dev_obj)->set_tx_switch_state(RFCOM_TDSCDMA_MODE,
                                                      band_u,
                                                      temp_port_state);
              ((rfdevice_asm *)cmn_dev_obj)->set_tx_band_map(cfg->logical_device,
                                                             RFCOM_TDSCDMA_MODE, 
                                                             band_u, 
                                                             device_info_ptr->rf_asic_info[i].data, 
                                                             RFC_ASIC_INFO_DATA_SIZE
                                                             );
            }
          }
          else
          {
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma::init_rf_devices(), rftdscdma_device_list is NULL for logical dev: %d, tx/rx:%d, band:%d",cfg->logical_device, cfg->rx_tx, cfg->band);
            /*If the device is present in RFC but has NULL physical ptr, then we return a fatal error during rfm_init */
            if (device_status == RFC_DEVICE_MISSING_FATAL)
            {
              RF_MSG_3(RF_FATAL, "Failed to map device! band = %d dev_type = %d instance = %d", 
                       cfg->band, dev_type, asm_instance);          
              return FALSE;
            }
          }
          break;
        
        case RFDEVICE_PA:
          if (cmn_dev_obj != NULL)
          {
            if (pa_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
            {
              tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][pa_instance] = cmn_dev_obj;
              pa_instance++; 
            } 
            else
            {
              RF_MSG_1(RF_FATAL, "Unsupported number of pa instances!", 0); 
            }

            band_u.tdscdma_band = (rfcom_tdscdma_band_type)cfg->band;
          
            ((rfdevice_pa *)cmn_dev_obj)->set_band_map(RFCOM_TDSCDMA_MODE, 
                                                       band_u, 
                                                       device_info_ptr->rf_asic_info[i].data, 
                                                       RFC_ASIC_INFO_DATA_SIZE);                                       

          }
          else
          {
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma::init_rf_devices(), rftdscdma_device_list is NULL for logical dev: %d, tx/rx:%d, band:%d",cfg->logical_device, cfg->rx_tx, cfg->band);
            /*If the device is present in RFC but has NULL physical ptr, then we return a fatal error during rfm_init */
            if (device_status == RFC_DEVICE_MISSING_FATAL)
            {
              RF_MSG_3(RF_FATAL, "Failed to map device! band = %d dev_type = %d instance = %d", 
                       cfg->band, dev_type, pa_instance);          
              return FALSE;
            }
          }
          break;

        case RFDEVICE_PAPM:
          if (cmn_dev_obj != NULL)
          {            
            if (papm_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
            {
              if(cfg->rx_tx == RFC_CONFIG_TX)
              {
                tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][papm_instance] = cmn_dev_obj;
                papm_instance++; 
              }
            }
            else
            {
              RF_MSG_1(RF_FATAL, "Unsupported number of papm instances!", 0); 
            }

            band_u.tdscdma_band = (rfcom_tdscdma_band_type)cfg->band;

            ((rfdevice_papm *)cmn_dev_obj)->set_band_map(cfg->logical_device,
                                                         RFCOM_TDSCDMA_MODE, 
                                                         band_u, 
                                                         device_info_ptr->rf_asic_info[i].data, 
                                                         RFC_ASIC_INFO_DATA_SIZE);
          }
          else
          {
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma::init_rf_devices(), rftdscdma_device_list is NULL for logical dev: %d, tx/rx:%d, band:%d",cfg->logical_device, cfg->rx_tx, cfg->band);
            /*If the device is present in RFC but has NULL physical ptr, then we return a fatal error during rfm_init */
            if (device_status == RFC_DEVICE_MISSING_FATAL)
            {
              RF_MSG_3(RF_FATAL, "Failed to map device! band = %d dev_type = %d instance = %d", 
                       cfg->band, dev_type, papm_instance);          
              return FALSE;
            }
          }
          break;

        case RFDEVICE_XSW:
           /* Confirm we were actually able to obtain our cmn_dev_obj */
           if (cmn_dev_obj == NULL)
           {
             RF_MSG_1(RF_FATAL, "Unable to get CMN RF Device Object!", 0); 
           }
           else
           {
             if (xsw_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
             {
               tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][xsw_instance] = cmn_dev_obj; 
               xsw_instance++;               
             }
             else
             {
               RF_MSG_1(RF_FATAL, "Unsupported number of XSW instances!", 0); 
             }
             
             band_u.tdscdma_band = (rfcom_tdscdma_band_type)cfg->band;
             
             if (cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1)
             {
               ((rfdevice_xsw *)cmn_dev_obj)->set_band_map_swap(RFCOM_TDSCDMA_MODE,  
                                                                band_u, 
                                                                device_info_ptr->rf_asic_info[i].data, 
                                                                RFC_ASIC_INFO_DATA_SIZE);
             }/*(cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1) */
             else if (cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1)
             {

               ((rfdevice_xsw *)cmn_dev_obj)->set_band_map_default(RFCOM_TDSCDMA_MODE, 
                                                                   band_u, 
                                                                   device_info_ptr->rf_asic_info[i].data, 
                                                                   RFC_ASIC_INFO_DATA_SIZE);
             }/* (cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1) */ 
             else
             {
               RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->req", 0);
             }
           } /* end IF cmn_dev_obj==NULL */         
           break; /* end case RFDEVICE_XSM */
           

        case RFDEVICE_TUNER:
          if (cmn_dev_obj != NULL)
          {
            if (tuner_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
            { 
              if ( (cfg->rx_tx == RFC_CONFIG_TX) || (cfg->rx_tx == RFC_CONFIG_RX) )
              {
                /*** Get Tuner Device ***/
                tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][tuner_instance] = cmn_dev_obj;
                band_u.tdscdma_band = (rfcom_tdscdma_band_type)cfg->band;
                if ((cmn_dev_obj != NULL) && (cfg->rx_tx == RFC_CONFIG_TX))
                {
                  /*** Set distortion configuration ***/
                 ((rfdevice_antenna_tuner *)cmn_dev_obj)->set_band_distortion_config_map(RFCOM_TDSCDMA_MODE, 
                                                                                         band_u,
                                                                                         device_info_ptr->rf_asic_info[i].data);
                }

                /*** Get Tuner manager ***/
                /* Grab the associated Tuner manager instance, for now there is one to one mapping between
                 a tuner device and tuner manager*/
                cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_TUNER_MANAGER, instance);

                /* Make sure there is a valid Antenna tuner manager */
                rfdevice_class *tuner_port_obj = NULL;
               
                /** As TDSCDMA doesnot support Carrier Aggregation(CA) of Tunner
                    Manager, hence all the RFM paths make requests to tuner
                    manager using port-0
               
                    Port-0 and Port-1 are synonymous from Tuner Manager point of view,
                    hence a tech can make request using any of the ports. */
               
                /*--- Get Port-0 object ---*/
                tuner_port_obj =  rfcommon_atuner_manager_factory_get_port_instance(cmn_dev_obj,
                                                                                    ATUNER_CA_PORT_0 );

                tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][tuner_instance] = tuner_port_obj;

               /*** ASD TUNER MANAGER***/
               /*** Get ASD Tuner manager ***/
               tuner_instance++;
              }
            }
          }
          else
          {
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma::init_rf_devices(), rftdscdma_device_list is NULL for logical dev: %d, tx/rx:%d, band:%d",cfg->logical_device, cfg->rx_tx, cfg->band);
           /*If the device is present in RFC but has NULL physical ptr, then we return a fatal error during rfm_init */
           if (device_status == RFC_DEVICE_MISSING_FATAL)
           {
             RF_MSG_3(RF_FATAL, "Failed to map device! band = %d dev_type = %d instance = %d", 
                      cfg->band, dev_type, tuner_instance);          
             return FALSE;
           }
          }
          break;
  

       case RFDEVICE_HDET:
       if (cmn_dev_obj != NULL)
       {
         if (hdet_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
         {
           if ( (cfg->rx_tx == RFC_CONFIG_TX) || (cfg->rx_tx == RFC_CONFIG_RX) )
           {
             tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][hdet_instance] = cmn_dev_obj;
             hdet_instance++; 
           }
         }
         else
         {
           RF_MSG_1(RF_FATAL, "Unsupported number of hdet instances!", 0); 
         }
       }
       else
       {
         MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma::init_rf_devices(), rftdscdma_device_list is NULL for logical dev: %d, tx/rx:%d, band:%d",cfg->logical_device, cfg->rx_tx, cfg->band);
         /*If the device is present in RFC but has NULL physical ptr, then we return a fatal error during rfm_init */
         if (device_status == RFC_DEVICE_MISSING_FATAL)
         {
           RF_MSG_3(RF_FATAL, "Failed to map device! band = %d dev_type = %d instance = %d", 
                    cfg->band, dev_type, hdet_instance);          
           return FALSE;
         }
       }
       break;

       case RFDEVICE_COUPLER:
       if (cmn_dev_obj != NULL)
       {
         if (coupler_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
         {
           if (cfg->rx_tx == RFC_CONFIG_TX)
           {
             tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][coupler_instance] = cmn_dev_obj;
             coupler_instance++; 
             band_u.tdscdma_band = (rfcom_tdscdma_band_type)cfg->band;
             if ((cmn_dev_obj != NULL) && (cfg->rx_tx == RFC_CONFIG_TX))
             {
               /*** Set band config map for coupler parameters ***/
               ((rfdevice_coupler *)cmn_dev_obj)->set_band_config_map(RFCOM_TDSCDMA_MODE, 
                                                                      band_u,
                                                                      device_info_ptr->rf_asic_info[i].data);
             }
           }
         }
         else
         {
           RF_MSG_1(RF_FATAL, "Unsupported number of coupler instances!", 0); 
         }
       }
       else
       {
         MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma::init_rf_devices(), rftdscdma_device_list is NULL for logical dev: %d, tx/rx:%d, band:%d",
                cfg->logical_device, cfg->rx_tx, cfg->band);
         /*If the device is present in RFC but has NULL physical ptr, then we return a fatal error during rfm_init */
         if (device_status == RFC_DEVICE_MISSING_FATAL)
         {
           RF_MSG_3(RF_FATAL, "Failed to map device! band = %d dev_type = %d instance = %d", 
                    cfg->band, dev_type, coupler_instance);          
           return FALSE;
         }
       }
       break;

        default:
              MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unkown RF device type %d", dev_type);
          break;
  
      }
      i++;
    }

   /* Final Check: If there are no PAs specified in RFC (as is the current case for GRFC PA) then append a generic GRFC PA */
   if( (cfg->rx_tx == RFC_CONFIG_TX) &&
       (pa_instance == 0) &&
       (rfc_data != NULL)
     )
   {
     /* Create grfc_pa instance if it has not been created */
     if(rfc_tdscdma_common_grfc_pa_device == NULL)
     {
       rfc_tdscdma_common_grfc_pa_device = new grfc_pa();
     }
   
     if(rfc_tdscdma_common_grfc_pa_device != NULL)
     {
       band_u.tdscdma_band = (rfcom_tdscdma_band_type)cfg->band;
   
       /* If grfc_pa device is valid then program sig info (band_map) */      
       /* get band sig data */
       rfc_data->sig_cfg_data_get(cfg, &mode_rf_signals);
       /* program band map */
       ((rfdevice_pa*)rfc_tdscdma_common_grfc_pa_device)->set_band_map( RFM_TDSCDMA_MODE, 
                                                                        band_u, 
                                                                        (int32*) mode_rf_signals, 
                                                                        0 /* Don't care */ );
   
       /* Assign grfc_pa instance to table */
       tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_PA][pa_instance] = 
         (void*)rfc_tdscdma_common_grfc_pa_device; 
     }
     else
     {
       RF_MSG_1(RF_FATAL, "Unable to create GRFC PA instance!", 0); 
       return FALSE;
     }
   } /* end if for adding GRFC PA device */

 #ifdef FEATURE_RF_ASDIV
    if (ant_num >= RFC_MAX_ANTENNA)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "Wrong ant_num=%d which is greater than Max %d!", ant_num, RFC_MAX_ANTENNA);
      return FALSE;
    }

    ant_params[cfg->logical_device][cfg->rx_tx][cfg->band].antenna_num = ant_num;
    
     /* update the rfc_asdiv_config info table with RFC attributes */
   asd_tuner_mgr = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_ASD_TUNER_MANAGER, 0);
   if ( asd_tuner_mgr != NULL )
   {
      (void)((rfcommon_asdiv_tuner_manager*)asd_tuner_mgr)->update_rfc_asdiv_config_info(cfg->req, \
                                                                                        ant_num,   \
                                                                                        dev_type,  \
                                                                                        instance);
   }
   else
   {
      RF_MSG(RF_ERROR, "ASDIV: asd_tuner_mgr returns NULL ptr");
   }

  /* If there is a Tuner device in the rf-card, we need make sure that there is at-least 
        one Tuner Instance for each Antenna Path. This is required by the Tuner design and also by the ASDiv design.
        Based on the state(Rx/Tx/Sleep) of the antenna path, other tuner devices need to be configured.
        So, We need to create a Dummy Tuner Device Object. 
     */

    /* If the tuner device is NOT present */
    if (tuner_instance == 0 ) 
    {
      rfdevice_class *tuner_port_obj = NULL;

      /* get the dummy Tuner object if needed. If there are no tuners in the rf-card, Dummy Tuner Mgr will not be created. */
      rfcommon_atuner_manager_intf *tuner_mgr_obj = (rfcommon_atuner_manager_intf *)(rfc_cmn->get_dummy_tuner_mgr(ant_num)); 
      
      if (tuner_mgr_obj != NULL)
      {
        if ( (cfg->logical_device == RFM_DEVICE_0) || (cfg->logical_device == RFM_DEVICE_1) )
        {
            tuner_port_obj =  tuner_mgr_obj->get_port_instance(ATUNER_CA_PORT_0);
        }
        else if ( (cfg->logical_device == RFM_DEVICE_2) || (cfg->logical_device == RFM_DEVICE_3) )
        {
            tuner_port_obj =  tuner_mgr_obj->get_port_instance(ATUNER_CA_PORT_1);
        }
        else
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Wrong logical_device=%d!", cfg->logical_device );
          return FALSE;
        }
        tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][tuner_instance] = tuner_port_obj;
      }
    }
 #endif /* #ifdef FEATURE_RF_ASDIV */
  }
  else if (cfg->port_state == RFC_PORT_STATE_B)
  {
    temp_port_state = RFC_PORT_STATE_B;

    while (i < device_info_ptr->num_devices)
    {
      dev_type = device_info_ptr->rf_asic_info[i].device_type;
      instance = device_info_ptr->rf_asic_info[i].instance;
      cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(dev_type, instance);

      device_status = rfc_cmn->get_cmn_rf_device_status(dev_type, (uint8)instance);
           
      switch(dev_type)
      {
      case RFDEVICE_ASM:

        if (asm_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
        {
          tdscdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][asm_instance] = cmn_dev_obj;
          asm_instance++;
        }
        else
        {
          RF_MSG_1(RF_FATAL, "Unsupported number of asm instances!", 0); 
        }

        if(cmn_dev_obj != NULL)
        {
          band_u.tdscdma_band = (rfcom_tdscdma_band_type)cfg->band;

          if(cfg->rx_tx == RFC_CONFIG_RX)
          {
            /* Update the Rx ASM Port state with temp_port_state and Call the ASM API below */
            ((rfdevice_asm *)cmn_dev_obj)->set_rx_switch_state(RFCOM_TDSCDMA_MODE,
                                                      band_u,
                                                      temp_port_state);

            ((rfdevice_asm *)cmn_dev_obj)->set_rx_band_map(cfg->logical_device,
                                                           RFCOM_TDSCDMA_MODE, 
                                                           band_u, 
                                                           device_info_ptr->rf_asic_info[i].data, 
                                                           RFC_ASIC_INFO_DATA_SIZE
                                                           );
          }
          else if (cfg->rx_tx == RFC_CONFIG_TX)
          {  
            ((rfdevice_asm *)cmn_dev_obj)->set_tx_switch_state(RFCOM_TDSCDMA_MODE,
                                                      band_u,
                                                      temp_port_state);
            ((rfdevice_asm *)cmn_dev_obj)->set_tx_band_map(cfg->logical_device,
                                                           RFCOM_TDSCDMA_MODE, 
                                                           band_u, 
                                                           device_info_ptr->rf_asic_info[i].data, 
                                                           RFC_ASIC_INFO_DATA_SIZE
                                                           );
          }
        }
        else
        {
          MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_tdscdma::init_rf_devices(), rftdscdma_device_list is NULL for logical dev: %d, tx/rx:%d, band:%d",cfg->logical_device, cfg->rx_tx, cfg->band);
          /*If the device is present in RFC but has NULL physical ptr, then we return a fatal error during rfm_init */
          if (device_status == RFC_DEVICE_MISSING_FATAL)
          {
            RF_MSG_3(RF_FATAL, "Failed to map device! band = %d dev_type = %d instance = %d", 
                     cfg->band, dev_type, asm_instance);          
            return FALSE;
          }
        }
        break;

      default:
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unkown RF device type %d", dev_type);
      break;
      }
      i++;
    } 
  }
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the rfdevices with the rfc dependent data.

  @details
  This function initializes the rfdevices with the rfc dependent data.

*/
boolean rfc_tdscdma::init_rfdevices_for_all_bands(void)
{
  uint8 band, alt_path;
  uint8 device;
  uint8 port_state;
  rfc_cfg_params_type cfg;
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();
  /* Start with true and decide based on results of device init sequences */
  boolean init_complete = TRUE;
  
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return FALSE;
  }

  //configure port_state_a by default at bootup
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  /* Loop all port_state */
  for (port_state = (uint8)RFC_PORT_STATE_A; port_state <= (uint8)RFC_PORT_STATE_B; port_state++)
  {
    cfg.port_state = (rfc_port_state_type)port_state;
    
    /* Loop all alt_path */    
    for(alt_path = 0; alt_path <= 1; alt_path++)
    {
      cfg.alternate_path = alt_path;
      
      for (device = (uint8)RFM_DEVICE_0; device < (uint8)RFM_MAX_WAN_DEVICES; device++)
      {
        cfg.logical_device = (rfm_device_enum_type)device;

        /* Push Rx info */
        cfg.rx_tx = RFC_CONFIG_RX;        

        for (band = RFCOM_BAND_TDSCDMA_B34; band < RFCOM_NUM_TDSCDMA_BANDS; band++)
        {
          cfg.band = band;    
          
          rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

          if (device_info_ptr != NULL)
          {
            init_complete &= init_rf_devices(&cfg, device_info_ptr);
          }
          else
          {
            //MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "PRx Config: device_info_ptr is NULL for Band %d", band);
          }
        } /* for (band = RFCOM_BAND_TDSCDMA_B34; band < RFCOM_NUM_TDSCDMA_BANDS; band++) */

        /* Push Tx info */
        cfg.rx_tx = RFC_CONFIG_TX;        

        for (band = RFCOM_BAND_TDSCDMA_B34; band < RFCOM_NUM_TDSCDMA_BANDS; band++)
        {
          cfg.band = band;    
          
          rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

          if (device_info_ptr != NULL)
          {
            init_complete &= init_rf_devices(&cfg, device_info_ptr);
          }
          else
          {
            //MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "PRx Config: device_info_ptr is NULL for Band %d", band);
          }
        } /* for (band = RFCOM_BAND_TDSCDMA_B34; band < RFCOM_NUM_TDSCDMA_BANDS; band++) */
      }/* for (device = RFM_DEVICE_0; device < RFM_MAX_WAN_DEVICES; device++) */
    }/* for(alt_path = 0; alt_path <= 1; alt_path++) */
  }/* for (port_state = RFC_PORT_STATE_A; port_state <= RFC_PORT_STATE_B; port_state++) */ 

  /* TBD: for ANT_SWAP device info */
  cfg.port_state= RFC_PORT_STATE_A;
  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
  for (band = RFCOM_BAND_TDSCDMA_B34; band < RFCOM_NUM_TDSCDMA_BANDS; band++)
  {
    cfg.band = band;    
    rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

    // Only try to init devices if device_info_ptr is valid
    if (NULL != device_info_ptr)
    {
      init_complete &= init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DISABLE_ANT_SWAP_1;
  for (band = RFCOM_BAND_TDSCDMA_B34; band < RFCOM_NUM_TDSCDMA_BANDS; band++)
  {
    cfg.band = band;    
    rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

    // Only try to init devices if device_info_ptr is valid
    if (NULL != device_info_ptr)
    {
      init_complete &= init_rf_devices(&cfg, device_info_ptr);
    }
  }    

  return init_complete;
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  API to update the rf port enumeration for various devices 
 
  @details
  Function is used as part of path swapping procedure. It is called when a
  suitable alternate path is selected for a particular CA or DSDS/DSDA scenario.
  The alternate path info along with the current band and logical device is used
  to determine which specific device info structure for that band should be
  used. Based on the info, rf port enumeration for various rf devices are
  updated. 
 
  @param rfm_dev
  current logical device which needs port reconfiguration
 
  @param curr_band
  current rf band involved in path swapping or port selection 
 
  @param alt_path
  preferred alternate path to use for a specific logical device & band combo
 
  @return status
  TRUE if a successful match is found in RFC AG and rf port info update is
  complete, else return FALSE
 
*/
boolean rfc_tdscdma::get_updated_rf_port
(
  rfm_device_enum_type rfm_dev, 
  rf_card_band_type curr_band, 
  uint8 alt_path
)
{
  rfc_cfg_params_type cfg;
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();

  /* Check if rfc_data pointer is NULL */
  if (rfc_data == NULL)
  {
    RF_MSG(RF_ERROR, "RFC TDSCDMA Data Object is NOT Created");
    return FALSE;
  }

  /* Set configuration pointer */
  cfg.logical_device = rfm_dev;
  cfg.rx_tx          = RFC_CONFIG_RX;
  cfg.req            = RFC_REQ_DEFAULT_GET_DATA;
  cfg.band           = rf_tdscdma_core_util_convert_band_rfc_to_rfcom(curr_band); 
  cfg.alternate_path = alt_path; 

  /* Get the column specific to a band, logical device & alt path */
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
#if 0
  MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "Debug ALT_PATH (device %d, band = %d, alt_path = %d) got device_info_ptr =0x%x", 
        cfg.logical_device,
        cfg.band,
        cfg.alternate_path,
        device_info_ptr); 
#endif
  /* Update the band port enumeration for all devices in that logical path */
  cfg.port_state = RFC_PORT_STATE_A;
  init_rf_devices(&cfg, device_info_ptr);

  MSG_4(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH,
        "Debug ALT_PATH (device %d, band = %d, alt_path = %d, portA/B = %d) updated", 
        cfg.logical_device,
        cfg.band,
        cfg.alternate_path,
        cfg.port_state);  

  cfg.port_state = RFC_PORT_STATE_B;
  init_rf_devices(&cfg, device_info_ptr);

  MSG_4(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH,
        "Debug ALT_PATH (device %d, band = %d, alt_path = %d, portA/B = %d) updated", 
        cfg.logical_device,
        cfg.band,
        cfg.alternate_path,
        cfg.port_state);      

  return TRUE; 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function obtains GRFC signal information and configures them

  @details
  This function obtains GRFC signal information of switches and configures them to setup
  feedback path for XPT calibration

*/
boolean rfc_tdscdma::setup_fb_path(rfm_device_enum_type device_fb, rfcom_tdscdma_band_type band, boolean xpt_enable)
{
  rfc_cfg_params_type cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  rfc_tdscdma_data *rfc_tdscdma_data_ptr = NULL;

  cfg.alternate_path = 0;    
  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device_fb;
  cfg.req = xpt_enable?RFC_REQ_ENABLE_XPT_CAPTURE:RFC_REQ_DISABLE_XPT_CAPTURE;

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }

  rfc_tdscdma_data_ptr = rfc_tdscdma_data::get_instance();

  if (rfc_tdscdma_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return(FALSE);
  }    

  /* TODO -- (kb) Verify the autogen file is populated with correct FB GRFCs on Bryce card.*/
  rfc_tdscdma_data_ptr->sig_cfg_data_get(&cfg, &signal_list_ptr);

  if (signal_list_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA signal_list_ptr is Null", 0);
    return(FALSE);
  }      

  rfc_common_tdd_config_signal(signal_list_ptr);
 
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function obtains device information from RFC and configures them

  @details
  This function obtains feedback device information from RFC and configures for XPT calibration

*/
boolean rfc_tdscdma::setup_fb_device
(
   rfm_device_enum_type device, 
   rfm_device_enum_type device_fb,
   rfcom_tdscdma_band_type band, 
   uint16 tx_chan, 
   boolean xpt_enable
)
{
  rfc_cfg_params_type cfg;
  rfc_intf *rfc_cmn = NULL;
  rfdevice_class *trx_device = NULL;
  rfdevice_rxtx_common_class* trx_cmn_device = NULL;  
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_tdscdma_data *rfc_tdscdma_data_ptr = NULL;
  //rftdscdma_device_tx_type *tx_device = NULL;  
  //uint32 temp_instance;
  boolean set_path=TRUE;
  rfc_cmn_properties_type *rfc_cmn_properties_ptr = NULL;
  
  
  cfg.alternate_path = 0;

  if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
  {
    cfg.port_state= RFC_PORT_STATE_B;
  }

  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  rfc_tdscdma_data_ptr = rfc_tdscdma_data::get_instance();

  if (rfc_tdscdma_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return(FALSE);
  }  
  // TODO -- (kb) Need to verify the device config info for bryce card after its bring up.
  rfc_tdscdma_data_ptr->devices_cfg_data_get(&cfg, &device_info_ptr);

  if (device_info_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA device_info_ptr is Null", 0);
    return(FALSE);
  }    
    
  //temp_instance =device_info_ptr->rf_asic_info[0].instance;

  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device_fb;
  cfg.req = xpt_enable? RFC_REQ_ENABLE_XPT_CAPTURE : RFC_REQ_DISABLE_XPT_CAPTURE;

  // TODO -- (kb) Need to verify the device config info for bryce card after its bring up.
  rfc_tdscdma_data_ptr->devices_cfg_data_get(&cfg, &device_info_ptr);

  if (device_info_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA device_info_ptr is Null", 0);
    return(FALSE);
  }      
  
  // Get common device instance.
  rfc_cmn = rfc_intf::get_instance();  
  trx_device= rfc_cmn->get_cmn_rf_device_object(RFDEVICE_TRANSCEIVER, device_info_ptr->rf_asic_info[0].instance);
  if( trx_device != NULL )
  {
    trx_cmn_device = (rfdevice_rxtx_common_class *)trx_device;
  }
  else
  {
    RF_MSG_1(RF_ERROR, "trx_cmn_device not found %d", 0);   
    return(FALSE);
  }
      
#if 0 //Anand to confirm if the following sectin can be deleted.
  if (temp_instance !=device_info_ptr->rf_asic_info[0].instance)
  {  
    /* extract the tdscdma device object from the cmn_device object and store it */
    tx_device = (rftdscdma_device_tx_type *)rfdevice_cmn_get_tech_instance(trx_cmn_device,
                                                                           RFM_TDSCDMA_MODE, 
                                                                           RFDEVICE_TX_CHAIN); 
  
    /* TODO -- (kb) Not needed. To be removed*/
    /*rfdevice_tdscdma_tx_tune_default_port((rfm_device_enum_type)((uint16)device_fb-1), tx_device, RFM_TDSCDMA_MODE, (rfcom_tdscdma_band_type)band, 
                                                tx_chan, RFDEVICE_DPD_DISABLED, RFDEVICE_EXECUTE_IMMEDIATE, NULL); */
  
  }
#endif
  /* get the feedback path attenuation state from rfc   */
  set_path &= rfc_get_cmn_device_properties(&rfc_cmn_properties_ptr);

  if(rfc_cmn_properties_ptr != NULL)
  {
    /* Turn on feedback path */
    set_path &= rfdevice_cmn_set_et_path(trx_cmn_device,
                                         RFM_TDSCDMA_MODE, 
                                         (rfm_fb_attenuation_state_type)rfc_cmn_properties_ptr->rfc_attenuation_state,
                                         xpt_enable);
    MSG_1( MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, "Set et path to attenuation state %d", rfc_cmn_properties_ptr->rfc_attenuation_state);
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfc_cmn_properties pointer is NULL", 0 ); 
    return(FALSE);
  }

  return (set_path);
}  

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function generates the AsDiv script for the requested device
  and band.

  @param position
  RF device for which to generate script
 
  @param cb_data
  Call back data to pass in additional info for extendability

  @param asdiv_script_ptr
  Output - The asdiv script
 
  @return
  True on success and FALSE on failure
*/

#ifdef FEATURE_RF_ASDIV  
boolean rfc_tdscdma::generate_asdiv_xsw_script
( 
  uint32 position,
  rf_buffer_intf *asdiv_script_ptr,
  void *cb_data
)
{
  boolean status = TRUE;
  rfc_cfg_params_type band_path_cfg;
  rfc_device_info_type* device_info_ptr = NULL;
  rf_tdscdma_asdiv_asm_script_data *xsw_script_data;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();
  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  /*Device type used to extract tech specific object*/
  rfdevice_type_enum_type cmn_dev_type = RFDEVICE_TYPE_INVALID;
  /*Device instance used to extract tech specific object*/
  uint32 instance = 0;
  /* Pointer to device class to hold common object */
  rfdevice_class *device_obj = NULL;
  boolean immediate;
  uint8 i= 0;
  rfdevice_xsw_config_type xsw_config;

  if ( rfc_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_tdscdma::generate_asdiv_xsw_script: "
                      "rfc_data ptr is null !!");
    return FALSE;
  }

  /* Get the ASM script data */
  xsw_script_data = (rf_tdscdma_asdiv_asm_script_data*)cb_data;

  /* NULL pointer check */
  if ( xsw_script_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_tdscdma_asdiv_create_asm_script: NULL callback data" );
    return FALSE;
  } /* if ( asm_script_data == NULL ) */

  /* Populate params */
  immediate = xsw_script_data->immediate;

  band_path_cfg.alternate_path = 0;
  band_path_cfg.logical_device = xsw_script_data->device; /* Not required*/
  band_path_cfg.band = xsw_script_data->band;
  band_path_cfg.rx_tx=RFC_CONFIG_RX;

  xsw_config.band.tdscdma_band = xsw_script_data->band;
  xsw_config.mode = RFCOM_TDSCDMA_MODE;

  if ((rfcommon_asdiv_position_type) position == 1) 
  {
    band_path_cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
    xsw_config.xsmode = RFDEVICE_XSW_PRX_DRX_PATH_SWAP;
  }
  else
  {
    band_path_cfg.req = RFC_REQ_DISABLE_ANT_SWAP_1;
    xsw_config.xsmode = RFDEVICE_XSW_PRX_DRX_PATH_DEFAULT;
  }

   MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfc_tdscdma::generate_asdiv_xsw_script: "
                     "position %d", position);

   /* Get device info pointer */
   rfc_data->devices_cfg_data_get(&band_path_cfg, &device_info_ptr );

   MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfc_tdscdma::generate_asdiv_xsw_script "
                     "device_info_ptr %d", &device_info_ptr);

   if ( device_info_ptr != NULL )
   {      
     /* loop through the device info ptr to find the index of each device object*/    
     for (i=0;i < device_info_ptr->num_devices;i++)
     {
       /*Get device type and device instance to retrieve device object*/
       cmn_dev_type = device_info_ptr->rf_asic_info[i].device_type;
       switch (cmn_dev_type) 
       {           
         case RFDEVICE_XSW:
           instance = device_info_ptr->rf_asic_info[i].instance;
           device_obj = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_XSW,(uint8)instance);

           if( device_obj != NULL )
           {
             if (immediate == 0) 
              {
                status = ((rfdevice_xsw *)device_obj)->set_cross_switch_config(&xsw_config,asdiv_script_ptr,(rf_device_execution_type)(!immediate),0);
                status = ((rfdevice_xsw *)device_obj)->trigger(asdiv_script_ptr,(rf_device_execution_type)(!immediate));
              }
            else
              { 
                status = ((rfdevice_xsw *)device_obj)->set_cross_switch_config(&xsw_config,NULL,(rf_device_execution_type)(!immediate),0);
                status = ((rfdevice_xsw *)device_obj)->trigger(NULL,(rf_device_execution_type)(!immediate));
              }

           }
           else
           {
             status = FALSE ;
           }
          break;

          default:
            break;    
         } 
     }/*for (i < device_info_ptr->num_devices)*/

   } 
   else
   {
     status = FALSE ; 
   }

  return status;

} /* xsw device script */
#endif /*#define FEATURE_RF_ASDIV */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band info. for the specified rx channel by querying RFC.
 
   @param
   *split_band: pointer to boolean indicating whether it was a split band
   rx_chan: DL channel number
   *rfc_band: band for which data is desired

   @details

*/
void rfc_tdscdma::get_rx_band(boolean *split_band, uint16 rx_chan, 
                               rfcom_tdscdma_band_type *rfc_band)
{
  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();
  boolean band_split_ret_val = FALSE;
  int i = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = (int)*rfc_band;
  cfg.rx_tx = RFC_CONFIG_RX;

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }


  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
  if ( (band_split_ret_val == TRUE) && (signal_list_ptr != NULL) )
  {
    *split_band = TRUE;
    /* Loop through channels to find true band */
      while ((unsigned int)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
      {
        if (((uint32)rx_chan >= signal_list_ptr[0].band_info[i].start_chan) && ((uint32)rx_chan <= signal_list_ptr[0].band_info[i].stop_chan))
        {
          *rfc_band = (rfcom_tdscdma_band_type)(signal_list_ptr[0].band_info[i].band);
          break;
        }
        i++;
      }    
  } 
  else
  {
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, "Band split return value %d", band_split_ret_val);
    if (signal_list_ptr == NULL)
    {
      MSG(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, "Null signal list pointer returned from RFC");
    }
  }
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the tx band info. for the specified tx channel by querying RFC.
 
   @param
   *split_band: pointer to boolean indicating whether it was a split band
   tx_chan: UL channel number
   *rfc_band: band for which data is desired

   @details

*/
void rfc_tdscdma::get_tx_band(boolean *split_band, uint16 tx_chan, 
                               rfcom_tdscdma_band_type *rfc_band)
{
  
  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();
  boolean band_split_ret_val = FALSE;
  int i = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = (int)*rfc_band;
  cfg.rx_tx = RFC_CONFIG_TX;

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }


  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
  
  if ( (band_split_ret_val == TRUE) && (signal_list_ptr != NULL) )
  {
     *split_band = TRUE;
     /* Loop through channels to find true band */
     while ((unsigned int)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
     {
       if (((uint32)tx_chan >= signal_list_ptr[0].band_info[i].start_chan) && ((uint32)tx_chan <= signal_list_ptr[0].band_info[i].stop_chan))
       {
         *rfc_band = (rfcom_tdscdma_band_type)(signal_list_ptr[0].band_info[i].band);
         break;
       }
       i++;
     }
   } 
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the filtered/unfiltered band info. for the specified tx channel by querying RFC.
 
   @param
   *split_band: pointer to boolean indicating whether it was a split band
   tx_chan: UL channel number
   *rfc_band: band for which data is desired

   @details

*/
void rfc_tdscdma::get_filtered_band(rfcom_tdscdma_band_type band, rfc_coex_filter_status_type filter_status, 
                               rfcom_tdscdma_band_type *rfc_band)
{
  
  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();
  boolean band_split_ret_val = FALSE;
  int i = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = band;
  cfg.rx_tx = RFC_CONFIG_TX;

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }


  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFC TDSCDMA band before querying RFC=%d", cfg.band);

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
  
  if ( (band_split_ret_val == TRUE) && (signal_list_ptr != NULL) )
  { 
     /* Loop through channels to find true band */
     while ((unsigned int)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
     {
      if(signal_list_ptr[0].band_info[i].status != RFC_FILTER_CFG_NOT_APP)
      {
         if (signal_list_ptr[0].band_info[i].status == filter_status)
         {
            *rfc_band = (rfcom_tdscdma_band_type)(signal_list_ptr[0].band_info[i].band);

             MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFC TDSCDMA band =%d", ((rfcom_tdscdma_band_type)(signal_list_ptr[0].band_info[i].band)));
         break;
         }
       }
       i++;
     }
   } 
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the tx band info. for the specified tx channel by querying RFC.
 
   @param
   split_band: band for which low and high channels are desired
   *low_earfcn: Pointer to low chan
   *high_earfcn: Pointer to high chan

   @details

*/
void rfc_tdscdma::get_tx_hi_lo_earfcn(rfcom_tdscdma_band_type split_band, 
                                     uint16 *low_earfcn,
                                     uint16 *high_earfcn)
{
  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();
  boolean band_split_ret_val = FALSE;
  int i = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = split_band;
  cfg.rx_tx = RFC_CONFIG_TX;

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
  /* Debug only */
  MSG_1(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED, "band split return val is %d", band_split_ret_val);
  if ((band_split_ret_val == TRUE) && (signal_list_ptr != NULL))
  {
   /* Loop through channels to find true band */
    while ((unsigned int)(signal_list_ptr[0].band_info[i].band) != RFC_INVALID_PARAM)
    {
      if ((uint8)(signal_list_ptr[0].band_info[i].band) == split_band)
      {
        *low_earfcn = signal_list_ptr[0].band_info[i].start_chan;
        *high_earfcn = signal_list_ptr[0].band_info[i].stop_chan;
        /* Debug only */
        MSG(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED, " Found split band earfcns");
        break;
      }
      i++;
    }
  }
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   Indicates whether device initialization has been completed for the rf card.
 
   @param

   @details

*/

/*!
  @brief Indicates whether device initialization has been completed
  for the rf card.

  @param

  @details
  return device init complete flag 

*/
boolean rfc_tdscdma::get_device_init_complete(void)
{
  return device_init_complete_status;
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the asd config for the specified band.
 
   @param
   
   @details

*/
void *rfc_tdscdma::get_asd_xsw_config(rfm_device_enum_type device, rfcom_tdscdma_band_type band, int position)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type          *rf_asd_signals = NULL;
  rfc_tdscdma_data *rfc_data = rfc_tdscdma_data::get_instance();
  rfdevice_xsw_config_type xsw_config;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC TDSCDMA Data Object is NOT Created", 0);
    return rf_asd_signals;
  }

  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.band = band;
  
  xsw_config.band.tdscdma_band=band;
  xsw_config.mode= RFCOM_TDSCDMA_MODE;

    if (rf_tdscdma_mc_state.port_state == RF_TDSCDMA_PORT_STATE_B)
    {
      cfg.port_state= RFC_PORT_STATE_B;
    }


   if ( position == 1 ) 
  {
     cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
     xsw_config.xsmode=RFDEVICE_XSW_PRX_DRX_PATH_SWAP;
  }
  else
  {
      cfg.req = RFC_REQ_DISABLE_ANT_SWAP_1;
      xsw_config.xsmode=RFDEVICE_XSW_PRX_DRX_PATH_DEFAULT;
  }

  rfc_data->sig_cfg_data_get(&cfg, &rf_asd_signals);

  return( (void*)rf_asd_signals);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function is to check if GRFC PA existed.

   @details
   This function is to check if GRFC PA existed by checking if GRFC PA related pointed is NULL or not

*/
boolean rfc_tdscdma::grfc_pa_existed(void)
{
  return(rfc_tdscdma_common_grfc_pa_device != NULL);
} /* rfc_tdscdma::grfc_pa_existed */

