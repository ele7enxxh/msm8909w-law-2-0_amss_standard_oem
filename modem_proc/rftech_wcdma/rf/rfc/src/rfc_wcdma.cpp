/*!
   @file
   rfc_wcdma.cpp

   @brief
   This file contains all the WCDMA mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2008 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/rfc/src/rfc_wcdma.cpp#1 $

when       who     what, where, why
--------------------------------------------------------------------------------
05/26/16   sub     RF F3 cleanup
09/04/15   ak      Changes to put the GRFCs to init state in DRX sleep
04/17/15   vs      Reverting Un-paired (dummy) tuner support.
04/13/15   dyc     Un-paired (dummy) tuner support.
03/05/15   vv      API support for rfc to capture modem chain to trx and VCO path mapping 
12/04/14   ag     Update the ASDIV table/antenna params only if the req is RFC_REQ_ENABLE_ANT_SWAP_1
                   or RFC_REQ_DISABLE_ANT_SWAP_1
11/18/14   dyc     Support updating rfc_asdiv_config_info table from RFC info
11/11/14   ac      xsw inverse logic fix for device 
09/07/14   ak      ASDIV : Switching the Antenna position correctly in wakeup based on the last position before sleep.
10/23/14   ac      tuner support for DBDC
10/21/14   px      Added RFM Device support for PAPM
09/16/14   sn      ASM set band RFM Device support
09/15/14   vbh     [1] Added device as a parameter to support Tx sharing
                   [2] Removed hardcoding on device 0
                   [3] Initialize RFC for both Tx devices
06/16/14   tks     Added support for get updated port config 
04/17/14   ac      asd bringup with rfc changes and device script
04/09/14   zhh     WCDMA related Array boundary check
04/08/14   yb      Added support for coupler set band configuration
03/25/14   yb      Added support for Tuner Linearizer
03/20/12   ac      asd changes from dime 
01/16/14   ndb     Added Daughter Card support(NON_FATAL_DEVICES_SUPPORT).
12/11/13   tks     Updated tx set band data to enable RFC based LUT map control
12/10/dw   dw      Fix warning and remove bringup hack
11/20/13   rp      Changes to support class based implementation for Common 
                   device driver.
10/31/13   shb     Converted TRX common device interface to C++
10/3/13    kr      Add to query asdiv tuner is present/not.
09/23/13   vbh     Warning fixes 
09/16/13   hm      hooking up WCDMA C++ device driver interface for WTR
07/17/13   aro     Explicit comparision
07/26/13   vs      Add RF interface to query DBDC bands supported
07/15/13   aro/kr  Added initial support for WCDMA ASDiv
07/09/13   kcj     Check that device init succeeded during rfwcdma_mc_init.
05/20/13   kai     Get the feedback path attenuation state from rfc
05/07/13   rmb     Fix OFT Error.
05/06/13   vb      Support for Tuner manager factory
05/02/13   vb      KW Fix: Check for Tuner Manager validity
05/02/13   vb      Tuner CA changes 
05/01/13   rmb     Added enable/disable QTF Tuner support if Antenna Tuner object is present in RFC.
04/22/13   sma     Reduce RF memory consumption for WCDMA
04/23/13   ka      Initial DBDC support
04/19/13   adk     Update parameters that get RFDEVICE_TUNER_MANAGER object
04/11/13   kai     Match apt_config_enum with WFW SMPS mode enum
03/22/13   sr      changes to PAPM device interface.
03/12/13   sn      XPT dual WTR fix for feedback path
03/08/13   kcj     KW and off-target warning fixes
03/05/13   vrb     Replace RFC accessor param init_req boolean by a req enum
03/05/13   kcj     Fix KW warnings 
02/28/13   kcj     Throw FATAL error if a required device is missing.
02/14/13   ac      tdet feature cleanup
02/09/13   vb      KW fix
02/07/13   vb      Add support for Tuner manager
02/06/13   dw      Add inclusion of rfcommon_msg.h
01/27/12   gh      Add support for GRFC PA
01/07/13   tks     Fix incorrect port enum being pushed to the second device 
11/30/12   adk     Dime RFC updates to get tuner and hdet device instances
11/26/12   tks     Added support for multiple instance of rfdevices
11/26/12   ac      dual carrier support for the bands in rf cards
11/16/12    vb     Added TUNER device support
11/14/12   dw      Add Dime APT support
10/30/12   sr      changes create tech rfc object if the tech is supported by 
                   the specific rf-card.
10/11/12    dw     SVVZ bringup updates
09/28/12   swb     Add support for RFC virtual PA mapping
09/27/12   vbh     Added NULL ptr check before calling any ASM device APIs
09/18/12   ac      rfc to take device as parameter
09/05/12   vbh     Fix on target warnings 
08/26/12   tks     Add support to push band port information to pa device  
08/19/12   tks     Add support to push band port information to asm device  
08/17/12  ac/tks   Add command dispatch support to check for APT status 
08/10/12   tks     Add support for pushing band port information to transceiver 
                   device 
08/08/12   ac      rfc crash fix 
08/03/12   sr      initialized the rf_devices[] ptrs 
07/27/12   sr      renamed rfdevice related enum types
07/28/12   tks     Fixed compile errors  
07/19/12   shb     Implemented rfc_wcdma::init() to push band port to device
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function 
                   ptr table
05/18/12   sr      RFC-EFS code clean-up changes
04/24/12   ac      Added few more bands for rfc binary data support
04/24/12   ac      Added support for rfc binary data
04/17/12   gh      Add support for TX_GRFC_ENGINE_DATA in mode_config_get
04/04/12   swb     Add multi-band support for HSPA APT
03/01/01   dw      Add support for HSPA APT
01/17/12   kg      rfc support for wcdma with qpa
01/12/12   vb      Tx init signals configured as part of RFC tx_band_config
01/12/12   vb      Remove configuring of Tx signals as part of RFC sleep
11/22/11   vb      Bug fix in programming RFC signals during WCDMA Wakeup
11/09/11   ac      code cleanup, tx_disable
11/08/11   ac      fix for tx_disable 
11/03/11   ac      tx_disable
11/03/11   av      Add path for the rx_band and tx_band variables.
10/31/11   ac      rfc wakeup
10/05/11   sr/ac   rfc sleep
09/12/11   sr      RFC cleanup.
03/23/11   dw      Use get_instance() for WCDMA and add destructor.
01/10/11   dw      Initial Version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfc_card.h"
#include "rfc_wcdma.h"
#include "rfc_common.h"
#include "rfc_wcdma_data.h"
#include "rfc_class.h"
#include "rfdevice_rxtx_common_class.h"
#include "rfdevice_wcdma_intf.h"
#include "rfdevice_asm.h" 
#include "rfc_vreg_mgr.h"
#include "rfdevice_pa.h"
#include "rfdevice_papm.h"
#include "rfc_common_grfc_pa.h"
#include "rfcommon_msg.h"
#include "rfcommon_atuner_manager_factory.h"
#include "rfdevice_coupler.h"
#include "rfdevice_xsw.h"
extern "C"
{
  #include "rfcommon_core.h"
  #include "rfwcdma_core.h"
}

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_tuner_manager.h"
#include "rfcommon_atuner_manager_intf.h"
#endif

/* To Enable/Disable QTF Tuner support */
#ifdef FEATURE_RF_HAS_QTUNER
extern "C"
{
 boolean enable_qtf_tuner_support = FALSE;
}
#endif
/* ----------------------------------------------------------------------- */
/*!
  @brief
  RF Card WCDMA CORE configuration table.

  @details
  Contains the static values for the CORE parameters for WCDMA mode of operation.
  The structure and type is specific to this RF card.
  {warmup_time, tune_time, pwr_on_wakeup, cgagc_settling_time}
  This structure shouldn't be card specific. only the data.. needs change
*/
rfc_wcdma_core_config_type  rfc_wcdma::rfc_wcdma_core_config_table = {500,200,TRUE,200};

rfc_wcdma_hspa_apt_config_type rfc_wcdma::rfc_wcdma_hspa_config_data[RFCOM_NUM_WCDMA_BANDS] = 
  {
    {FALSE,0,0,0},
    {FALSE,0,0,0},
    {FALSE,0,0,0},
    {FALSE,0,0,0},
    {FALSE,0,0,0},
    {FALSE,0,0,0},
    {FALSE,0,0,0},
    {FALSE,0,0,0},
    {FALSE,0,0,0}
  };

rfc_wcdma *rfc_wcdma::rfc_wcdma_ptr = (rfc_wcdma *)NULL;

/* Pointer to device class to hold common object */
static rfdevice_class *rfc_wcdma_common_grfc_pa_device = NULL;

rfc_wcdma * rfc_wcdma::get_instance(void)
{
  return rfc_wcdma_ptr;
}

// create singleton object
rfc_wcdma * rfc_wcdma::create_instance(void)
{
  if (rfc_wcdma_ptr == NULL )
  {
    rfc_wcdma_ptr = (rfc_wcdma *)new rfc_wcdma();
  }
  return rfc_wcdma_ptr;
}

/*!
  @brief Indicates whether device initialization has been completed
  for the rf card.

  @details

  @return Device init complete flag 

*/
boolean rfc_wcdma::get_device_init_complete(void)
{
  return device_init_complete;
}

// Constructor 
rfc_wcdma::rfc_wcdma()
{
  uint8 log_dev      = 0;
  uint8 rx_tx        = 0;
  uint8 band         = 0;
  uint8 dev_type     = 0;
  uint8 dev_instance = 0;

  /* Initialize the device init flag */
  device_init_complete = FALSE;

  /* Initialize WCDMA rf_devices */ 
  for (log_dev = 0; log_dev < RFM_DEVICE_4; log_dev++)
  {
    for(rx_tx = 0; rx_tx < RFC_CONFIG_RXTX_MAX; rx_tx++)
    {
      for (band = 0; band < RFCOM_NUM_WCDMA_BANDS; band++)
      {
        for(dev_type = 0; dev_type < RFDEVICE_TYPE_MAX_NUM; dev_type++)
        {
          for (dev_instance = 0; dev_instance <RFC_MAX_DEVICE_INSTANCES_PER_BAND; dev_instance++)
          {
            wcdma_rf_devices[log_dev][rx_tx][band][dev_type][dev_instance] = NULL;
          }
        }
      }
    }
  }

  /*  Push band port information for device, pa & asm */
  device_init_complete = init_rfdevices_for_all_bands();
}

// Destructor
rfc_wcdma::~rfc_wcdma()
{
  rfc_wcdma_ptr = NULL;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the common WCDMA RFC configuration.

   @details

*/

void rfc_wcdma::config_mode( rfm_device_enum_type device )
{

  rfc_sig_cfg_type *rf_signals = NULL;
  /* Init configuration parameters for WCDMA mode*/
  rfc_cfg_params_type cfg = { RF_PATH_0,  RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_NUM_WCDMA_BANDS, RFC_REQ_INIT};
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return;
  }
  if ( (device >= RFM_DEVICE_4) || ( device == RFM_ALL_DEVICES ))
  {
  RF_MSG(RF_ERROR,"wrong device index");
  return;
  }
  /* initialize the RFC rx and tx band information */
  rx_band[device] = RFCOM_NUM_WCDMA_BANDS;
  tx_band[device] = RFCOM_NUM_WCDMA_BANDS;
  cfg.alternate_path = 0;
  cfg.logical_device = device;

  MSG_1(MSG_SSID_RF, MSG_LEGACY_LOW,"RFC WCDMA Get Data called on %d",cfg.logical_device );

  /*Initialize all the GRFC/GPIOs/RF Signals to the init value*/
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_config_signal(rf_signals);

  /* Is there mode dependent config ?*/
  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Select WCDMA mode device%d", device);

} 

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function implements the WCDMA Rx band configuration for RFC.
*/
void rfc_wcdma::rx_band_config(rfm_device_enum_type device, rfcom_wcdma_band_type curr_band)
{
  rfc_cfg_params_type cfg;
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  
#ifdef FEATURE_RF_ASDIV
    rfcommon_asdiv_position_type position = 0;
#endif /*FEATURE_RF_ASDIV*/


  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return;
  }

  if ( (device >= RFM_DEVICE_4) || ( device == RFM_ALL_DEVICES ))
  {
  RF_MSG(RF_ERROR,"wrong device index");
  return ;
  }
  /*Update the common config for accessor function*/
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  /*Before configuring the new band, need to move back the GRFC/GPIOs configured for previous band
  to their default state*/
  if(rx_band[device] != curr_band)
  {
    cfg.band = (int)rx_band[device];
    rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
    rfc_common_init_default(rf_signals);
  }
  
  /*Cleanup for previous band done, now start configuration for the new band*/
  rf_signals = NULL;
  cfg.band = (int)curr_band;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_config_signal(rf_signals);

  #ifdef FEATURE_RF_ASDIV
  /* Configure ASDiv switch position */
  rf_signals = NULL;
  position = rfcommon_asdiv_get_current_position(device);

  if(position == 0)
  {
    cfg.req = RFC_REQ_DISABLE_ANT_SWAP_1;
  }
  else
  {
    cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
  }

  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_config_signal(rf_signals);
#endif /*FEATURE_RF_ASDIV*/

  /*Update the curr band*/
  rx_band[device] = curr_band;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band configuration info.

   @details

*/
void *rfc_wcdma::rx_band_config_get(rfm_device_enum_type device, rfcom_wcdma_band_type curr_band,
                                    rfc_band_config_data_enum_type config_data)
{
  void *data_config_ptr = NULL;
  rfc_band_config_data_enum_type data = config_data;  
  static rfcom_wcdma_band_type rfc_wcdma_rx_prev_band = RFCOM_BAND_INVALID;
  rfc_cfg_params_type cfg;
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return data_config_ptr;
  }

  /*Update the common config for accessor function*/
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.band = (int)curr_band;

  switch (data)
  {
    case RFC_GPIO_DATA:
    case RFC_GRFC_DATA:
    case RFC_RF_CTRL_DATA:
      /*Extract the GPIO/GRFC info from RFC AG*/
      rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
      data_config_ptr = rf_signals;    
      break;

    case RFC_CORE_DATA: 
      /*Extract the RTR Rx band port mapping from RFC AG*/
      // Dime RFC changes : should query device to get the port info.
      // rfc_ag->wcdma_accessor_fp(&cfg, &rf_signals);
      // data_config_ptr = &(rf_signals->rf_asic_info.rf_asic_port_id);
      break;

    default:            /* Invalid mode operation */
      MSG_3(MSG_SSID_RF, MSG_LEGACY_FATAL,"Unknown RFC data request (%d, %d, %d)", device, curr_band, config_data);
      break;
  }

  /*Update the curr band*/
  rfc_wcdma_rx_prev_band = curr_band;

//  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFC WCDMA:rx_band_config_get curr_band: %d", rfc_wcdma_rx_prev_band);

  return( data_config_ptr );
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the GPIO/GRFCs/RF Cntrl lines for Tx chain

   @details
*/
void rfc_wcdma::tx_band_config(rfm_device_enum_type device, rfcom_wcdma_band_type curr_band)
{
  rfc_sig_cfg_type *rf_signals = NULL;

  /* Init configuration parameters for init tx GRFCs */
  rfc_cfg_params_type cfg = {RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_TX, RFCOM_NUM_WCDMA_BANDS, RFC_REQ_INIT};
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return;
  }
  if ( (device >= RFM_DEVICE_4) || ( device == RFM_ALL_DEVICES ))
  {
  RF_MSG(RF_ERROR,"wrong device index");
  return;
  }
  /* Configure the init tx GRFCs */
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.req = RFC_REQ_INIT;

  rfc_data->sig_cfg_data_get(&cfg, &rf_signals );
  rfc_common_config_signal ( rf_signals);

  /*Update the common config for accessor function*/
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  /*Before configuring the new band, need to move back the GRFC/GPIOs configured for previous band
  to their default state*/
  rf_signals = NULL;
  if(tx_band[device] != curr_band)
  {
    cfg.band = (int)tx_band[device];
    rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
    rfc_common_init_default(rf_signals);
  }
  
  /*Cleanup for previous band done, now start configuration for the new band*/
  rf_signals = NULL;
  cfg.band = (int)curr_band;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_config_signal(rf_signals);

  /*Update the curr band*/
  tx_band[device] = curr_band;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the GPIO/GRFCs/RF Cntrl lines for Tx chain

   @details

*/
void *rfc_wcdma::tx_band_config_get(rfm_device_enum_type device, rfcom_wcdma_band_type curr_band,
                                    rfc_band_config_data_enum_type config_data)
{
  void *data_config_ptr = NULL;
  rfc_band_config_data_enum_type data = config_data;
  static rfcom_wcdma_band_type rfc_wcdma_tx_prev_band = RFCOM_BAND_INVALID;
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_cfg_params_type cfg;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return data_config_ptr;
  }

  /*Update the common config for accessor function*/
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.band = (int)curr_band;

  switch (data)
  {
    case RFC_GPIO_DATA:
    case RFC_GRFC_DATA:
    case RFC_RF_CTRL_DATA:
      /*Extract the GPIO/GRFC info from RFC AG*/
      rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
      data_config_ptr = rf_signals;   
    break;

    case RFC_CORE_DATA: 
      /*Extract the RTR Tx band port mapping from RFC AG*/
      // Dime RFC changes : Should query the device to get the port info.
      //rfc_ag->wcdma_accessor_fp(&cfg, &rf_signals);     
      //data_config_ptr = &(rf_signals->rf_asic_info.rf_asic_port_id);
      break;

    default:            
      /* Invalid mode operation */
      MSG_3(MSG_SSID_RF, MSG_LEGACY_FATAL,"Unknown RFC data request (%d, %d, %d)", device, curr_band, config_data);
      break;
  }

  /*Update the curr band*/
  rfc_wcdma_tx_prev_band = curr_band;
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFC WCDMA:tx_band_config_get curr_band: %d", rfc_wcdma_tx_prev_band);

  return( data_config_ptr );
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the GPIO/GRFCs/RF Cntrl lines for the mode

   @details

*/
void *rfc_wcdma::mode_config_get(rfm_device_enum_type device, rfc_mode_config_data_enum_type config_data)
{
  void* data_config_ptr = NULL;
  rfc_sig_cfg_type *rf_signals = NULL;

  /* Init configuration parameters for WCDMA mode and for Rx config*/
  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_NUM_WCDMA_BANDS, RFC_REQ_INIT};
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return data_config_ptr;
  }

  cfg.alternate_path = 0;
  cfg.logical_device = device; 

  switch (config_data)
  {
    case RFC_MODE_GPIO_DATA:
    case RFC_MODE_GRFC_DATA: 
    case RFC_MODE_RX_GRFC_ENGINE_DATA:

      /*! @NOTE: Return init signals for WCDMA for Rx only to meet IRAT needs */
      rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
      data_config_ptr = rf_signals;
      break;

    case RFC_MODE_CORE_DATA:
      data_config_ptr = (void *)&rfc_wcdma_core_config_table;
      break;

    case RFC_MODE_TX_GRFC_ENGINE_DATA:
      cfg.rx_tx = RFC_CONFIG_TX;
      cfg.req = RFC_REQ_INIT;
      rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
      data_config_ptr = rf_signals;
      break;

    default:            /* Invalid mode operation */
      MSG_2(MSG_SSID_RF, MSG_LEGACY_FATAL,"Unknown RFC data request (%d, %d)",device, config_data);
      break;
  }

  return( data_config_ptr );
}

int32 rfc_wcdma::sleep( rfm_device_enum_type device)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_NUM_WCDMA_BANDS, RFC_REQ_INIT};
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return 0;
  }
  if ( (device >= RFM_DEVICE_4) || ( device == RFM_ALL_DEVICES ))
  {
  RF_MSG(RF_ERROR,"wrong device index");
  return 0;
  }
  /* set the wcdma rx init_table GRFC settings into default state */
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_init_default(rf_signals);

  /* set the wcdma rx band_table GRFC settings into default state */
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.band = (int)rx_band[device];
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_init_default(rf_signals);

  //reset the rx and tx bands 
  rx_band[device] = RFCOM_NUM_WCDMA_BANDS;

  return 1;
}

int32 rfc_wcdma::tx_disable( rfm_device_enum_type device,rfcom_wcdma_band_type curr_band)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_TX, RFCOM_NUM_WCDMA_BANDS, RFC_REQ_DEFAULT_GET_DATA};
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return 0;
  }
  if ( (device >= RFM_DEVICE_4) || ( device == RFM_ALL_DEVICES ))
  {
  RF_MSG(RF_ERROR,"wrong device index");
  return 0;
  }
  /* set the previous band GRFC settings into default state */
  cfg.alternate_path = 0;
  cfg.band = curr_band;
  cfg.logical_device= device; 

  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_init_default(rf_signals);
  rf_signals = NULL;

  /* set the wcdma tx init_table GRFC settings into default state */
  cfg.req = RFC_REQ_INIT;
  rf_signals = NULL;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_init_default(rf_signals);
  
  //reset the tx bands 
  tx_band[device] = RFCOM_NUM_WCDMA_BANDS;
  return 1;
}

void rfc_wcdma::manage_vregs(rfc_power_management_info* pm_info)
{
  rfc_vreg_param vreg_param(pm_info->path, pm_info->mode);
  vreg_param.set_band(pm_info->band);
  vreg_param.set_rfchip_number(pm_info->rf_chip_num);
  vreg_param.set_rf_state(pm_info->state);

  rfc_vreg_mgr *vreg_mgr_p = rfc_vreg_mgr::get_instance();
  if (vreg_mgr_p != NULL)
  {
    vreg_mgr_p->wcdma_manage_power(vreg_param);
  }
  else
  {
    ERR_FATAL("Cannot Create VREG_MGR for  N6600 NA card",0,0,0);
  }

}

int32 rfc_wcdma::wakeup(rfm_device_enum_type device)
{
  rfc_cfg_params_type cfg = { RF_PATH_0, RFM_DEVICE_0, RFC_CONFIG_RX, RFCOM_NUM_WCDMA_BANDS, RFC_REQ_INIT};
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return 0;
  }

  /* Set the wcdma rx init_table GRFC settings into default state */
  cfg.alternate_path = 0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.logical_device = device; 

  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_config_signal(rf_signals);

  /* Set the wcdma tx init_table GRFC settings into default state */
  cfg.rx_tx = RFC_CONFIG_TX;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_config_signal(rf_signals);

  return 1;
}

int32 rfc_wcdma::command_dispatch( rfm_device_enum_type device, rfcs_wcdma_cmd_type cmd, void *data )
{
  switch (cmd)
  {

   case RFC_WCDMA_IS_DUAL_CARRIER_SUPPORTED:
        ((rfc_wcdma_dc_supported_type*)data)->dc_is_supported = TRUE;
        break;  

   case RFC_WCDMA_GET_CORE_CONFIG:
      *((rfc_wcdma_core_config_type *)data)  = rfc_wcdma_core_config_table;
      break;    
            
    case RFC_WCDMA_IS_LP_TX_LUT_SUPPORTED:
      *((boolean*)data) = FALSE;
        break;
                   
    case RFC_WCDMA_GET_APT_CONFIG:
      /* QPOET2 default bias needs a headroom of 300mV ~ 400mV, hence 
         reducing the default APT bias from 3700mV to 3200mV, as per
         recommendation from Tom Marra(RF Systems) */
      ((rfc_wcdma_apt_supported_type*)data)->apt_config =  RFC_WCDMA_XPT_QPOET_SUPPORTED;
      ((rfc_wcdma_apt_supported_type*)data)->default_smps_val = 3200;
      break;
                   
    case RFC_WCDMA_IS_QPA_SUPPORTED:
      *((boolean*)data) = FALSE;
        break;
                   
     case RFC_WCDMA_GET_HSPA_APT_CONFIG:
      if (((rfc_wcdma_hspa_apt_config_band_type *)data)->band >= RFCOM_NUM_WCDMA_BANDS)
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"RFC WCDMA Get HSPA APT config: Invalid band");
        break;
      }
      if ( ((rfc_wcdma_hspa_apt_config_band_type *)data)->band < RFCOM_NUM_WCDMA_BANDS )
      {
        (((rfc_wcdma_hspa_apt_config_band_type *)data)->rfc_wcdma_hspa_config_data) =
           rfc_wcdma_hspa_config_data[((rfc_wcdma_hspa_apt_config_band_type *)data)->band];
      } else
      {
       RF_MSG(RF_ERROR,"resource script returned was NULL");
      }
    break;

    default:
      MSG_2(MSG_SSID_RF, MSG_LEGACY_FATAL,"Unknown RFC cmd (%d, %d)",device, cmd);
      break;
  }

  return 1;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the device object ptr for the specified logical_path,
  rx or tx, band and device type.

  @details
  This function returns the device object ptr for the specified logical_path,
  rx or tx, band and device type.

*/
void **rfc_wcdma::get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                      rfcom_wcdma_band_type band, rfdevice_type_enum_type dev_type)
{
  void **dev_obj = NULL;

  #ifdef FEATURE_RF_ASDIV
  uint8 antenna = 0;
  rfdevice_class *asd_tuner_mgr = NULL;
  #endif

  if (rfm_dev >= RFM_DEVICE_4 || rx_tx >= RFC_CONFIG_RXTX_MAX 
      || band >= RFCOM_NUM_WCDMA_BANDS || dev_type >= RFDEVICE_TYPE_MAX_NUM)
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_wcdma::get_device_obj(%d, %d, %d, %d): Incorrect Parameters",
          rfm_dev, rx_tx, band, dev_type);
    return dev_obj;
  }

  dev_obj = wcdma_rf_devices[rfm_dev][rx_tx][band][dev_type];

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
  This function initializes the rfdevices with the rfc dependent data.

  @details
  This function initializes the rfdevices with the rfc dependent data.

  @return TRUE if all devices were initialized, FALSE otherwise
   
*/
boolean rfc_wcdma::init_rf_devices(rfc_cfg_params_type *cfg, rfc_device_info_type *device_info_ptr)
{
  rfdevice_class *cmn_dev_obj = NULL;
  rfcom_band_type_u rf_band; 
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  uint32 instance = 0;
  uint8 i = 0;
  rfc_intf *rfc_cmn   = rfc_intf::get_instance();
  bool is_dev_supported = true;
  rfdevice_trx_phy_path_enum_type phy_path = RFDEVICE_TRX_PATH_0;

 #ifdef FEATURE_RF_ASDIV
  rfdevice_class *asd_tuner_mgr = NULL;
  uint8 tuner_dev_instance = 0;
  uint8 ant_num = 0;
 #endif

  /* Track the number of instances of each device type */
  uint8 dev_inst_count[RFDEVICE_TYPE_MAX_NUM];

  rfc_sig_cfg_type* mode_rf_signals = NULL;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  rfdevice_trx_wcdma_rx *rx_dev_obj = NULL;
  rfdevice_trx_wcdma_tx *tx_dev_obj = NULL;

  /* Validate input parameters */
  if (NULL == cfg || NULL == device_info_ptr)
  {
    RF_MSG_2(RF_FATAL, "init_rf_devices(0x%x, 0x%x) : Bad parameters!", cfg, device_info_ptr);
    return FALSE;
  }

  if(((cfg->logical_device < RFM_DEVICE_0) || (cfg->logical_device >= RFM_DEVICE_4)) ||
     ((cfg->band < RFCOM_BAND_IMT) || (cfg->band >= RFCOM_NUM_WCDMA_BANDS)))
  {
    RF_MSG_2(RF_ERROR, "init_rf_devices : invalid device %d or band %d",
                        cfg->logical_device, cfg->band);
    return FALSE;
  }

  /* Determine if the requested band is supported on the specified */
  /* logical device before proceeding. */
  if (!rfwcdma_core_is_band_supported((rfcom_wcdma_band_type)cfg->band, rfcommon_core_device_to_path(cfg->logical_device)))
  {
     RF_MSG_2(RF_MED, "Band %d not supported on log_dev %d. Skipping device mapping.", cfg->band, cfg->logical_device);
     return TRUE;
  }

  if (NULL == rfc_cmn)
  {
    RF_MSG_1(RF_FATAL,"RFC CMN Object is NOT Created", 0);
    return FALSE;
  }

  /* Initialize the device counts for this configuration */
  for (unsigned type=0;type<RFDEVICE_TYPE_MAX_NUM;type++)
  {
    dev_inst_count[type] = 0;
  }

  rf_band.wcdma_band = (rfcom_wcdma_band_type)cfg->band; 


#ifdef FEATURE_RF_ASDIV
  ant_num = device_info_ptr->ant_num;
#endif

  /* Attempt to map all devices for this configuration. */
  while (i < device_info_ptr->num_devices)
  {
    dev_type = device_info_ptr->rf_asic_info[i].device_type;
    instance = device_info_ptr->rf_asic_info[i].instance;
    phy_path = (rfdevice_trx_phy_path_enum_type) device_info_ptr->rf_asic_info[i].phy_path_num;
    cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(dev_type, (uint8)instance);
    rfc_device_availability_enum_type device_status  = rfc_cmn->get_cmn_rf_device_status(dev_type, (uint8)instance);
    
    /* Default the current device to being supported. */
    is_dev_supported = true;
            
    /* Validate that the dev_type retrieved was valid. */    
    if (dev_type<0 || dev_type>=RFDEVICE_TYPE_MAX_NUM)
    {
      is_dev_supported = false;
      RF_MSG_1(RF_FATAL, "Invalid device type!", 0); 
    }
    /* Check whether the desired instance is valid. If not we'll */
    /* return FALSE here as something has gone very wrong. */
    else if (dev_inst_count[dev_type] >= RFC_MAX_DEVICE_INSTANCES_PER_BAND)
    {
      RF_MSG_1(RF_FATAL, "Unsupported number of device instances of type %d!", dev_type);
      return FALSE;
    }
    else
    {
      switch(dev_type)
      {
        case RFDEVICE_TRANSCEIVER:
          /* Confirm we were actually able to obtain our cmn_dev_obj */
          if (cmn_dev_obj == NULL)
          {
             RF_MSG_1(RF_FATAL, "Unable to get CMN RF Device Object!", 0); 
          }
          else
          {
              if (RFC_CONFIG_RX == cfg->rx_tx) /* rx device data */
              {
                /* Confirm we have a valid Primary or Secondary receive path */
                if (cfg->logical_device >= RFM_DEVICE_0 && cfg->logical_device <= RFM_DEVICE_3)
                {
                  rx_dev_obj = (rfdevice_trx_wcdma_rx *)((rfdevice_rxtx_common_class*)cmn_dev_obj)->get_tech_instance(RFM_IMT_MODE, RFDEVICE_RX, phy_path);

                  wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] = 
                      rx_dev_obj;
                  rfdevice_wcdma_rx_set_band_data( cfg->logical_device,
                                                   rx_dev_obj,
                                                   (rfcom_wcdma_band_type)cfg->band,
                                                   device_info_ptr->rf_asic_info[i].data,
                                                   RFC_ASIC_INFO_DATA_SIZE);
                }
                /*! Invalid path configuration */
                else
                {
                  RF_MSG_1(RF_ERROR, "Incorrect logical device specified!", 0); 
                }
              }/* !RFC_CONFIG_RX */
              else if (RFC_CONFIG_TX == cfg->rx_tx) /* tx device data */
              {
                tx_dev_obj = (rfdevice_trx_wcdma_tx *)((rfdevice_rxtx_common_class*)cmn_dev_obj)->get_tech_instance(RFM_IMT_MODE, RFDEVICE_TX, phy_path);
                wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] = tx_dev_obj;

                rfdevice_wcdma_tx_set_band_data(cfg->logical_device,tx_dev_obj,
                                                (rfcom_wcdma_band_type)cfg->band,
                                                device_info_ptr->rf_asic_info[i].data,
                                                RFC_ASIC_INFO_DATA_SIZE,
                                                RFDEVICE_PA_LUT_MAPPING_VALID);

              }/* !RFC_CONFIG_TX */
              else
              {
                RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->rx_tx", 0); 
              }
          } /* end IF cmn_dev_obj==NULL */

		  if (cfg->rx_tx == RFC_CONFIG_RX)
          {
            /*call the below api to capture the transciever->phy path->modem chain(ADC) mapping to perform adc dc cal.*/
            if(!rfc_cmn->set_trx_phy_path_adc_mapping(
                                                  device_info_ptr->modem_chain,                    /*modem chain to map the ADC*/
                                                  device_info_ptr->rf_asic_info[i].phy_path_num,   /*physical path*/
                                                  device_info_ptr->rf_asic_info[i].instance,       /*Transciever device type instance*/
                                                  RFM_IMT_MODE,                                    /*tech*/
                                                  rf_band,                                         /*band*/
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

          break; /* end case RFDEVICE_TRANSCEIVER */
    
        case RFDEVICE_PA: 
          /* Confirm we were actually able to obtain our cmn_dev_obj */
          if (cmn_dev_obj == NULL)
          {
             RF_MSG_1(RF_FATAL, "Unable to get CMN RF Device Object!", 0); 
          }
          else
          {
            if(cfg->rx_tx == RFC_CONFIG_TX) 
            {
              wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] = cmn_dev_obj; 
            
              ((rfdevice_pa *)cmn_dev_obj)->set_band_map(RFM_IMT_MODE, rf_band, 
                                                         device_info_ptr->rf_asic_info[i].data, 
                                                         RFC_ASIC_INFO_DATA_SIZE);
            }/* !RFC_CONFIG_TX */
            else
            {
              RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->rx_tx", 0); 
            }
          } /* end IF cmn_dev_obj==NULL */
          break; /* end case RFDEVICE_PA */
    
         case RFDEVICE_ASM:
           /* Confirm we were actually able to obtain our cmn_dev_obj */
           if (cmn_dev_obj == NULL)
           {
              RF_MSG_1(RF_FATAL, "Unable to get CMN RF Device Object!", 0); 
           }
           else
           {
             wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] = cmn_dev_obj; 
             
             if (cfg->rx_tx == RFC_CONFIG_RX)
             {
               ((rfdevice_asm *)cmn_dev_obj)->set_rx_band_map(cfg->logical_device, RFM_IMT_MODE, rf_band, 
                                                              device_info_ptr->rf_asic_info[i].data, 
                                                              RFC_ASIC_INFO_DATA_SIZE);
             }/* !RFC_CONFIG_RX */
             else if (cfg->rx_tx == RFC_CONFIG_TX)
             {
                 ((rfdevice_asm *)cmn_dev_obj)->set_tx_band_map(cfg->logical_device, RFM_IMT_MODE, rf_band, 
                                                                device_info_ptr->rf_asic_info[i].data, 
                                                                RFC_ASIC_INFO_DATA_SIZE);
             }/* !RFC_CONFIG_TX */ 
             else
             {
               RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->rx_tx", 0);
             }
           } /* end IF cmn_dev_obj==NULL */         
           break; /* end case RFDEVICE_ASM */
    
        case RFDEVICE_PAPM:
          if(cfg->rx_tx == RFC_CONFIG_TX)
          {
            wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] = cmn_dev_obj;
            if (cmn_dev_obj != NULL)
            {
              ((rfdevice_papm *)cmn_dev_obj)->set_band_map(cfg->logical_device,
                                                           RFM_IMT_MODE, rf_band, 
                                                           device_info_ptr->rf_asic_info[i].data, 
                                                           RFC_ASIC_INFO_DATA_SIZE);
            }

          }
          else
          {
            RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->rx_tx", 0);
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
             wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] = cmn_dev_obj; 
             
             if (cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1)
             {
               ((rfdevice_xsw *)cmn_dev_obj)->set_band_map_swap(RFM_IMT_MODE, rf_band, 
                                                              device_info_ptr->rf_asic_info[i].data, 
                                                              RFC_ASIC_INFO_DATA_SIZE);
             }/* !RFC_CONFIG_RX */
             else if (cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1)
             {

                ((rfdevice_xsw *)cmn_dev_obj)->set_band_map_default(RFM_IMT_MODE, rf_band, 
                                                                device_info_ptr->rf_asic_info[i].data, 
                                                                RFC_ASIC_INFO_DATA_SIZE);
             }/* !RFC_CONFIG_TX */ 
             else
             {
               RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->req", 0);
             }
           } /* end IF cmn_dev_obj==NULL */         
           break; /* end case RFDEVICE_ASM */
    
    
#ifdef FEATURE_RF_HAS_QTUNER
        case RFDEVICE_TUNER:

          if ( (cfg->rx_tx == RFC_CONFIG_TX) || (cfg->rx_tx == RFC_CONFIG_RX) )
          {
            /*** Get Tuner Device ***/
            wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] = cmn_dev_obj;

            if ((cmn_dev_obj != NULL) && (cfg->rx_tx == RFC_CONFIG_TX))
            {
              /*** Set distortion configuration ***/
             ((rfdevice_antenna_tuner *)cmn_dev_obj)->set_band_distortion_config_map(RFM_IMT_MODE, rf_band, 
                                                                    device_info_ptr->rf_asic_info[i].data);
            }

            /*** Get Tuner manager ***/
            /*Grab the associated Tuner manager instance, for now there is one to one mapping between
            a tuner device and tuner manager*/
            cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_TUNER_MANAGER, (uint8)instance);

           /* Make sure there is a valid Antenna tuner manager */
            if (cmn_dev_obj != NULL)
            {
              rfdevice_class *tune_port_obj = NULL;
             
              /** As WCDMA does not support Carrier Aggregation(CA) of Tuner
                  Manager, hence all the RFM paths make requests to tuner
                  manager using port-0
             
                  Port-0 and Port-1 are synonymous from Tuner Manager point of view,
                  hence a tech can make request using any of the ports. */
             
              if ( (cfg->logical_device == RFM_DEVICE_0) || (cfg->logical_device == RFM_DEVICE_1) )
               {             
              /*--- Get Port-0 object ---*/
              tune_port_obj =  rfcommon_atuner_manager_factory_get_port_instance ( cmn_dev_obj,
                                                                                   ATUNER_CA_PORT_0 );
             
              wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][dev_inst_count[dev_type]] = tune_port_obj;
             
              }

              if ( (cfg->logical_device == RFM_DEVICE_2) || (cfg->logical_device == RFM_DEVICE_3) )
              {  
                              /*--- Get Port-1 object ---*/
                tune_port_obj =  rfcommon_atuner_manager_factory_get_port_instance ( cmn_dev_obj,
                                                                                   ATUNER_CA_PORT_1 );
             
                wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][dev_inst_count[dev_type]] = tune_port_obj;
             
              }

              /* After obtaining the Common RF Device Object for Antenna Tuner, 
                 enable QTF Tuner support for all the bands */
              enable_qtf_tuner_support = TRUE;
                                            
                      #ifdef FEATURE_RF_ASDIV
              tuner_dev_instance++ ;
              #endif      /*ASDIV TUNER MANAGER*/

            }
          }
          else
          {
            RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->rx_tx", 0);
          }

          break;
#endif
        case RFDEVICE_COUPLER:

          if(cfg->rx_tx == RFC_CONFIG_TX)
          {
            /*** Get Tuner Device ***/
            wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] = cmn_dev_obj;

            if ((cmn_dev_obj != NULL))
            {
              /*** Set distortion configuration ***/
             ((rfdevice_coupler *)cmn_dev_obj)->set_band_config_map(RFM_IMT_MODE, rf_band, 
                                                                    device_info_ptr->rf_asic_info[i].data);
            }

          }
          else
          {
            RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->rx_tx", 0);
          }

          break;

        case RFDEVICE_HDET:
          if ( (cfg->rx_tx == RFC_CONFIG_TX) || (cfg->rx_tx == RFC_CONFIG_RX) )
          {
            wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] = cmn_dev_obj;
          }
          else
          {
            RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->rx_tx", 0);
          }

          break;
    
        default:
          RF_MSG_1(RF_FATAL, "Unsupported device type %d!", dev_type);
          /* We'll use this later so that we don't throw a fatal error for this */
          /* device request. */
          is_dev_supported = false;
          break;
      } /* end switch(dev_type) */
    } /* end IF device instances */

    /* Check if the device is supported before checking for device mapping success. */
    if (is_dev_supported)
    {
      /* Check if we were successful in assigning the requested device. */
      /* If not, ASSERT here so that the error is caught early. */
      if(wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][dev_inst_count[dev_type]] != NULL)
      {
        /* Device mapping was successful, increment the count. */
        dev_inst_count[dev_type]++;
      }
      /* Return FALSE if a supported device was requested but couldn't be mapped. */
      else
      {

        if (device_status == RFC_DEVICE_MISSING_FATAL)
        {
         RF_MSG_3(RF_FATAL, "Failed to map device! band=%d dev_type=%d instance=%d", cfg->band, dev_type, instance);
         return FALSE;
      }
    }
   }

    i++;
  } /* end while for going through all devices listed */


  /* Final Check: If there are no PAs specified in RFC (as is the current
     case for GRFC PA) then append a generic GRFC PA
  */
  if( (cfg->rx_tx == RFC_CONFIG_TX) &&
      (dev_inst_count[RFDEVICE_PA] == 0) &&
      (rfc_data != NULL)
    )
  {
    /* Create grfc_pa instance if it has not been created */
    if(rfc_wcdma_common_grfc_pa_device == NULL)
    {
      rfc_wcdma_common_grfc_pa_device = new grfc_pa();
    }

    if(rfc_wcdma_common_grfc_pa_device != NULL)
    {
      /* If grfc_pa device is valid then program sig info (band_map) */      
      /* get band sig data */
      rfc_data->sig_cfg_data_get(cfg, &mode_rf_signals);
      /* program band map */
      ((rfdevice_pa*)rfc_wcdma_common_grfc_pa_device)->set_band_map( RFM_IMT_MODE, 
                                                                     rf_band, 
                                                                     (int32*) mode_rf_signals, 
                                                                     0 /* Don't care */ );

      /* Assign grfc_pa instance to table */
      wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_PA][dev_inst_count[RFDEVICE_PA]] = 
        (void*)rfc_wcdma_common_grfc_pa_device; 
    }
    else
    {
      /* Return FALSE if we failed to create the PA instance. */
      RF_MSG_1(RF_FATAL,"Unable to create GRFC PA instance!", 0);
      return FALSE;
    }
  } /* end if for adding GRFC PA device */


#ifdef FEATURE_RF_ASDIV
 if ( (cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1) || (cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1) )
 {
  ant_params[cfg->logical_device][RFC_CONFIG_RX][cfg->band].antenna_num = ant_num ;

  ant_params[cfg->logical_device][RFC_CONFIG_TX][cfg->band].antenna_num = ant_num ;

 
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

   if (tuner_dev_instance == 0 ) 
   {
     rfdevice_class *tuner_port_obj = NULL;

     /* get the dummy Tuner object if needed. If there are no tuners in the rf-card, Dummy Tuner Mgr will not be created. */
     rfcommon_atuner_manager_intf *tuner_mgr_obj = (rfcommon_atuner_manager_intf *)(rfc_cmn->get_dummy_tuner_mgr(ant_num)); 

     if (tuner_mgr_obj != NULL)
     {
       if ( (cfg->logical_device == RFM_DEVICE_0) || (cfg->logical_device == RFM_DEVICE_1) )
       {
         tuner_port_obj =  tuner_mgr_obj->get_port_instance( ATUNER_CA_PORT_0 );
       }
       if ( (cfg->logical_device == RFM_DEVICE_2) || (cfg->logical_device == RFM_DEVICE_3) )
       {
         tuner_port_obj =  tuner_mgr_obj->get_port_instance( ATUNER_CA_PORT_1 );
       }
       wcdma_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][tuner_dev_instance] = tuner_port_obj;
     }
   }  
 }
#endif
  /* If we got this far then our device init was successful */
  return TRUE;

} /* rfc_wcdma::init_rf_devices */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the rfdevices with the rfc dependent data.

  @details
  This function initializes the rfdevices with the rfc dependent data.

  @return TRUE if device init succeeded on all specified bands, FALSE otherwise
 
*/
boolean rfc_wcdma::init_rfdevices_for_all_bands(void)
{
  uint8 band;
  rfc_cfg_params_type cfg;
  rfc_device_info_type *device_info_ptr;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  /* Start with true, AND in results of device init sequences */
  boolean init_complete = TRUE;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return FALSE;
  }

  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  for ( band = RFCOM_BAND_IMT; band < RFCOM_NUM_WCDMA_BANDS; band++ )
  {
    cfg.band = band;    
    rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

    // Only try to init devices if device_info_ptr is valid
    if (NULL != device_info_ptr)
    {
      init_complete &= init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_1;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  for ( band = RFCOM_BAND_IMT; band < RFCOM_NUM_WCDMA_BANDS; band++ )
  {
    cfg.band = band;
    rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

    // Only try to init devices if device_info_ptr is valid
    if (NULL != device_info_ptr)
    {
      init_complete &= init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_2;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  for ( band = RFCOM_BAND_IMT; band < RFCOM_NUM_WCDMA_BANDS; band++ )
  {
    cfg.band = band;
    rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

    // Only try to init devices if device_info_ptr is valid
    if (NULL != device_info_ptr)
    {
      init_complete &= init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_3;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  for ( band = RFCOM_BAND_IMT; band < RFCOM_NUM_WCDMA_BANDS; band++ )
  {
    cfg.band = band;
    rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

    // Only try to init devices if device_info_ptr is valid
    if (NULL != device_info_ptr)
    {
      init_complete &= init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  for ( band = RFCOM_BAND_IMT; band < RFCOM_NUM_WCDMA_BANDS; band++ )
  {
    cfg.band = band;
    rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

    // Only try to init devices if device_info_ptr is valid
    if (NULL != device_info_ptr)
    {
      init_complete &= init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_2;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  for ( band = RFCOM_BAND_IMT; band < RFCOM_NUM_WCDMA_BANDS; band++ )
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
  cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
  for ( band = RFCOM_BAND_IMT; band < RFCOM_NUM_WCDMA_BANDS; band++ )
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
  for ( band = RFCOM_BAND_IMT; band < RFCOM_NUM_WCDMA_BANDS; band++ )
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

boolean rfc_wcdma::get_updated_rf_port
(
  rfm_device_enum_type rfm_dev, 
  rf_card_band_type curr_band, 
  uint8 alt_path
)
{
  rfc_cfg_params_type cfg;
  rfc_device_info_type *device_info_ptr;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();

  /* Check if rfc_data pointer is NULL */
  if (rfc_data == NULL)
  {
    RF_MSG(RF_ERROR, "RFC WCDMA Data Object is NOT Created");
    return FALSE;
  }

  /* Set configuration pointer */
  cfg.alternate_path = 0;
  cfg.logical_device = rfm_dev;
  cfg.rx_tx          = RFC_CONFIG_RX;
  cfg.req            = RFC_REQ_DEFAULT_GET_DATA;
  cfg.band           = rfwcdma_core_convert_band_rf_card_to_rfcom(curr_band); 
  cfg.alternate_path = alt_path; 

  /* Get the column specific to a band, logical device & alt path */
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);

  /* Update the band port enumeration for all devices in that logical path */
  init_rf_devices(&cfg, device_info_ptr);

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
boolean rfc_wcdma::setup_fb_path(rfm_device_enum_type device_fb, rfcom_wcdma_band_type band, boolean et_enable)
{
  rfc_cfg_params_type cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  rfc_wcdma_data *rfc_wcdma_data_ptr = NULL;

  cfg.alternate_path = 0;    
  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device_fb;
  cfg.req = et_enable?RFC_REQ_ENABLE_XPT_CAPTURE:RFC_REQ_DISABLE_XPT_CAPTURE;

  rfc_wcdma_data_ptr = rfc_wcdma_data::get_instance();

  if (rfc_wcdma_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return(FALSE);
  }    

  rfc_wcdma_data_ptr->sig_cfg_data_get(&cfg, &signal_list_ptr);

  if (signal_list_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA signal_list_ptr is Null", 0);
    return(FALSE);
  }      

  rfc_common_config_signal(signal_list_ptr);
 
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function obtains device information from RFC and configures them

  @details
  This function obtains feedback device information from RFC and configures for XPT calibration

*/
boolean rfc_wcdma::setup_fb_device(rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_wcdma_band_type band, uint16 tx_chan)
{
  rfc_cfg_params_type cfg;
  rfc_intf *rfc_cmn = NULL;
  rfdevice_class *trx_device = NULL;
  rfdevice_rxtx_common_class* trx_cmn_device = NULL;  
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_wcdma_data *rfc_wcdma_data_ptr = NULL;
  rfdevice_trx_wcdma_tx *tx_device = NULL;  
  uint32 temp_instance;
  boolean set_path=TRUE;
  rfc_cmn_properties_type *rfc_cmn_properties_ptr = NULL;
  
  cfg.alternate_path = 0;
  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  rfc_wcdma_data_ptr = rfc_wcdma_data::get_instance();

  if (rfc_wcdma_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return(FALSE);
  }  
  
  rfc_wcdma_data_ptr->devices_cfg_data_get(&cfg, &device_info_ptr);

  if (device_info_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA device_info_ptr is Null", 0);
    return(FALSE);
  }    
    
  temp_instance =device_info_ptr->rf_asic_info[0].instance;

  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device_fb;
  cfg.req = RFC_REQ_ENABLE_XPT_CAPTURE;

  rfc_wcdma_data_ptr->devices_cfg_data_get(&cfg, &device_info_ptr);

  if (device_info_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA device_info_ptr is Null", 0);
    return(FALSE);
  }      
  
  rfc_cmn = rfc_intf::get_instance();  
  trx_device= rfc_cmn->get_cmn_rf_device_object(RFDEVICE_TRANSCEIVER, device_info_ptr->rf_asic_info[0].instance);
  if( trx_device != NULL )
  {
      /* extract the wcdma device object from the cmn_device object and store it */ 
      trx_cmn_device = (rfdevice_rxtx_common_class *)trx_device;
  }
  else
  {
    RF_MSG_1(RF_ERROR, "trx_cmn_device not found %d", 0);   
    return(FALSE);
  }
      
      
  if (temp_instance !=device_info_ptr->rf_asic_info[0].instance)
  {  
    tx_device = (rfdevice_trx_wcdma_tx *)((rfdevice_rxtx_common_class *)trx_device)->get_tech_instance( RFM_IMT_MODE,
                                                                                RFDEVICE_TX,
                                                                                RFDEVICE_TRX_PATH_0 );

    rfdevice_wcdma_tx_tune_default_port((rfm_device_enum_type)((uint16)device_fb-1), tx_device, RFM_IMT_MODE, (rfcom_wcdma_band_type)band, 
                                                               tx_chan, RFDEVICE_DPD_DISABLED, RFDEVICE_EXECUTE_IMMEDIATE, NULL);
  
  }
    
  /* get the feedback path attenuation state from rfc   */
  set_path &= rfc_get_cmn_device_properties(&rfc_cmn_properties_ptr);

  /* Turn on feedback path */
  if(rfc_cmn_properties_ptr != NULL)
  {
    set_path &= rfdevice_cmn_set_et_path(trx_cmn_device,
                                         RFM_IMT_MODE, 
                                         (rfm_fb_attenuation_state_type)rfc_cmn_properties_ptr->rfc_attenuation_state,
                                         TRUE);
    RF_MSG_1(RF_HIGH, "Set et path to attenuation state %d", rfc_cmn_properties_ptr->rfc_attenuation_state);          
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
boolean rfc_wcdma::generate_asdiv_xsw_script
( 
  uint32 position,
  rf_buffer_intf *asdiv_script_ptr,
  void *cb_data
)
{
  boolean status = TRUE;
  rfc_cfg_params_type band_path_cfg;
  rfc_device_info_type* device_info_ptr = NULL;
  rfwcdma_asdiv_asm_script_data *xsw_script_data;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();
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
    RF_MSG( RF_ERROR, "rfc_wcdma::generate_asdiv_xsw_script: "
                      "rfc_data ptr is null !!");
    return FALSE;
  }

  /* Get the ASM script data */
  xsw_script_data = (rfwcdma_asdiv_asm_script_data*)cb_data;

  /* NULL pointer check */
  if ( xsw_script_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfwcdma_asdiv_create_asm_script: NULL callback data" );
    return FALSE;
  } /* if ( asm_script_data == NULL ) */

  /* Populate params */
  immediate = xsw_script_data->immediate;

  band_path_cfg.alternate_path = 0;
  band_path_cfg.logical_device = xsw_script_data->device; /* Not required*/
  band_path_cfg.band = xsw_script_data->band;
  band_path_cfg.rx_tx=RFC_CONFIG_RX;

  xsw_config.band.wcdma_band = xsw_script_data->band;
  xsw_config.mode = RFM_IMT_MODE;

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

  // MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfc_wcdma::generate_asdiv_xsw_script: "
  //                   "position %d", position);

   /* Get device info pointer */
   rfc_data->devices_cfg_data_get(&band_path_cfg, &device_info_ptr );

   //MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfc_wcdma::generate_asdiv_xsw_script "
   //                "device_info_ptr %d", &device_info_ptr);

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
                status = ((rfdevice_xsw *)device_obj)->set_cross_switch_config(&xsw_config,asdiv_script_ptr,(rf_device_execution_type)(!immediate),0);
                status = ((rfdevice_xsw *)device_obj)->trigger(asdiv_script_ptr,(rf_device_execution_type)(!immediate));
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

void *rfc_wcdma::get_asd_xsw_config(rfm_device_enum_type device, rfcom_wcdma_band_type band, int position)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type          *rf_asd_signals = NULL;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();
  rfdevice_xsw_config_type xsw_config;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return rf_asd_signals;
  }

  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.band = band;

  xsw_config.band.wcdma_band=band;
  xsw_config.mode= RFM_IMT_MODE;


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

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get DBDC band combinations supported

  @details
  This will have to return the number of DBDC combinations supported on the RFC and those band combos.
  For RFCs which does not support any DBDC bands , by deafult it will be zero.

  @params None

  @return
  Pointer to the structure containing the DBDC band combinations
 
*/
void *rfc_wcdma::get_dbdc_band_support(void)
{
  rfc_wcdma_data *rfc_wcdma_data_ptr = NULL;
  void *dbdc_list_ptr = NULL ;
  rfc_wcdma_properties_type *band_list_ptr;

  rfc_wcdma_data_ptr = rfc_wcdma_data::get_instance();

  if (rfc_wcdma_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC WCDMA Data Object is NOT Created", 0);
    return(FALSE);
  }    

  rfc_wcdma_data_ptr->get_wcdma_properties(&band_list_ptr);
  dbdc_list_ptr = band_list_ptr;

  if (band_list_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"DBDC: RFC WCDMA DBDC band combo ptr is Null", 0);
    return(FALSE);
  }      
 
  return(dbdc_list_ptr);
  
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function updates the WCDMA Rx band configuration for RFC.
*/
void rfc_wcdma::update_rx_band_config(rfm_device_enum_type device, rfcom_wcdma_band_type curr_band)
{
  rfc_cfg_params_type cfg;
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_wcdma_data *rfc_data = rfc_wcdma_data::get_instance();


 if(device >= RFM_DEVICE_4) 
 {
   MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"update_rx_band_config: Invalid device %d for band %d", device, curr_band); 
   return;
 }

  /*Update the curr band*/
  rx_band[device] = curr_band;

  MSG_2(MSG_SSID_RF, MSG_LEGACY_LOW,"Updating the band config device %d band %d", device, curr_band); 
}
