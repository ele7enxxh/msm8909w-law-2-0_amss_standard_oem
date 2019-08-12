/*!
   @file
   rfc_lte.cpp

   @brief
   This file contains implementation the RFC LTE class
   The RFC LTE card is an abstract class that acts as an interface to
   abstract different RF cards the system may support 

   The implementation aspects of this file contains only the following:
   1. Contructor
   2. Destructor
   3. Get_Instance for the singleton

*/

/*===========================================================================

Copyright (c) 2010 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/rf/rfc/src/rfc_lte.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
06/09/16   vg      Updated the WTR->ADC mapping table for VCM cal.
11/26/15   lhb     FR 31436: API to provide the HW band limitation to LTE RRC
10/16/15   as      Revert - "Memset LTE Device pointer database before every update"
03/12/15   am      Ported : Memset LTE Device pointer database before every update
03/05/15   vv      API support for rfc to capture modem chain to trx and VCO path mapping 
01/14/15   dw      Ported : GRFC Alt Path Support
01/15/15   qzh     Reconfig Rx signal in case previous tx band share the same signal with current Rx band
12/15/14   ars     Adding check_intra_ca_feasible() API to fix issues with intra CA on split bands.
12/11/14   dw/qzh  Adding additional check to ignore invalid bands while fetching
                   split band info from RFC.
12/05/14   dw      Changes to support split bands based on split specified in RFC.
11/18/14   dyc     Support updating rfc_asdiv_config_info table from RFC info
11/03/14   ska     Adding fix for correctly picking device pointer for B29
10/23/14   cri     Initialize band support and only scan valid bands
10/21/14   px      Added RFM Device support for PAPM
10/20/14   ndb     Remove the un-used ASD interface  + Add the GRFC scripts in 
                   in SRS ASD API
10/07/14   ndb     Added 3DL CA support(Phase 2)
09/16/14   sn      ASM set band RFM Device support
08/25/14   bar     Added support for Jolokia RUMI
08/15/14   st      NS07 Port Update Support
07/31/14   ndb     Added support for extended EARFCN for LTE
07/10/14   jf      TM8 + SRS switching for TDD
07/08/14   gvn     Modify algorithm for split-intraband support
07/02/14   jf      AsDiv support
06/16/14   tks     Added alternate path support for update port mapping
06/13/14   kab     Add 3xCA support
06/11/14   bar     Added disable_fb_device() API to support IQ capture 
06/06/14   gvn     Use band passed in, instead of global variable in reset_grfc
05/15/14   gvn     Add Api for getting overlapping split bands
05/14/14   jf      Remove F3 MSG
04/23/14   tks     Support for dyanmic path selection
04/11/14   npi     Clean up common device pointer usage
04/11/14   bsh     Split Band Support
04/09/14   yb      Added support for Coupler
04/02/14   Saul    RFC. Intermediate check-in for alternate path.
03/27/14   svi     [ASDIV] asdiv_tuner to support multiple RF configurations
03/25/14   yb      Added support for Tuner Linearizer
01/09/14   svi     Split Band Support
01/07/13   cri     fix for common class implementation of et path config
12/09/13   kab     Fixed compiler warnings
11/20/13   rp      Changes to support class based implementation for Common 
                   device driver.
10/31/13   shb     Converted TRX common device interface to C++
10/23/13   cri     Enable D3925 sample capture
10/07/13   gvn     Fix error checking in reset_grfc function
09/19/13   aks     Updated functions to support LTE device driver C++ Interface
08/12/13   bsh     Separate core and util functions
07/22/13   as      Added support for RFDEVICE_ASD_TUNER_MANAGER logical device init
07/09/13    pv     Added support for separating out TDD Tx, Rx ASM scripts.
06/06/13    pv     Added support for multple TDD ASM scripts.
06/06/13   php     Remove reset GRFC to Init as MC already resets
05/20/13   kai     Get the feedback path attenuation state from rfc
05/13/13   gvn     Removed unwanted F3s
05/06/13   vb      Bug fix for assiging the Tuner port object
05/06/13   vb      Support for Tuner manager factory
05/02/13   vb      KW Fix: Check for Tuner Manager validity
05/02/13   vb      Tuner CA changes 
04/10/13   kab     Added support for Tx Antenna Swap.
03/22/13   sr      changes to PAPM device interface.
03/20/13   kab     Decreased memory allocated to lte_rf_devices
03/12/13   sn      XPT dual WTR fix for feedback path
03/05/13   vrb     Replace RFC accessor param init_req boolean by a req enum
02/15/13   aca     Tuner manager programming update
02/12/13   aca     Tuner manager support
01/28/13   sbm     Enable LPM for HDET.
01/24/13   php     Initialize HDET device object
01/15/13   gvn     Remove FEATURE_RF_HAS_QTUNER
12/21/12   pv      Fixed the bug for wrong band port enum for more than 1 transceiver chip.
12/18/12   bsh     KW errors fixed
11/29/12   gvn     QTUNER API Integration
11/15/12   adk     Backed out Qtuner code
09/14/12   jf      Qtuner LTE support (initial)
10/31/12   vss     Change to get device API for returning a list of objects,
                    made changes to populate list of rfdevices into lte_rfdevices hash table
10/30/12   sr      changes create tech rfc object if the tech is supported by 
                   the specific rf-card.
10/15/12   sbm     Set tx band map for Q-PA device.
10/02/12   sbm     Null check for asm device type.
08/29/12   pl/sbm  Initialize global rx/tx band array to invalid at constructor and exit mode
08/28/12   vss     Add support for PA and QPoet (power tracking device) 
08/22/12   pl      Fix bug on RX band config
08/21/12   vss     Add support for RFDEVICE_PA and RFDEVICE_ASM. 
08/16/12   pl      Update RFC configuration strucutre to reflect new changes in RFC 
08/10/12   vrb     Use updated interfaces to push band port information to dev
08/08/12   vss     Update set_rx_port and set_tx_port to use device instance instead of RFM device 
08/03/12   sr      initialized the rf_devices[] ptrs .
07/30/12   pl      Fix warning
07/27/12   sr      renamed rfdevice related enum types.
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type.
07/12/12   sr      push band-port mapping info to devices 
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
06/01/12   aca     Fix to clean up Tx signals when disable tx is called
05/24/12   jf      Move the manage_vregs() to base-class from rf-card specific files 
05/18/12   sr      RFC-EFS code clean-up changes.
05/07/12   jf      Added support for rfc binary data 
04/24/12   cd      Fixed LTE exit mode to restore init settings for both Tx and Rx band GRFC/ANT_SELs
03/08/12   php     Added support for TXP2 signals
03/01/12   jf    Added LPM status support
12/05/11   tnt     Add mode_sleep() api
01/03/11   bmg     Removed delete from class destructor, see comments.
12/15/10   pl      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfc_card.h"
#include "rfdevice_lte_interface.h"
#include "rflte_util.h"
#include "rflte_mc.h"
} // Extern "C"

#include "rfc_lte.h"
#include "rfc_common.h"
#include "rfc_card_lte.h"
#include "modem_mem.h"   
#include "rfc_vreg_mgr.h"
#include "rfc_lte_data.h"
#include "rfc_class.h"
#include "rfdevice_rxtx_common_class.h"
#include "rfdevice_asm.h"
#include "rfdevice_pa.h"
#include "rfdevice_papm.h"
#include "rfdevice_coupler.h"
#include "rfcommon_atuner_manager_factory.h"
#include "rfcommon_msg.h"
#include "rfcommon_fbrx_api.h"
#include "rfdevice_xsw.h"
#include "rfc_common_grfc_pa.h"

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_tuner_manager.h"
#endif
#include "rfcommon_atuner_manager_intf.h"


rfc_lte_intf *rfc_lte_intf::RFC_LTE_p = (rfc_lte_intf *)NULL;
static rfdevice_class *rfc_lte_common_grfc_pa_device = NULL;

// Singleton GetInstance method to instantiate Object
rfc_lte_intf * rfc_lte_intf::Get_Instance(void)
{
  return RFC_LTE_p;
}

// Craete the Singleton Object
rfc_lte_intf * rfc_lte_intf::create_instance(void)
{
  if (RFC_LTE_p == NULL )
  {
    RFC_LTE_p = (rfc_lte_intf *)new rfc_lte_intf();
  }
  return RFC_LTE_p;
}

// Constructor 
rfc_lte_intf::rfc_lte_intf()
{
  int8 i = 0;
  uint8 log_dev = 0;
  uint8 rx_tx = 0;
  uint8 band = 0;
  uint8 dev_type = 0;
  uint8 dev_instance = 0;


  for(i = 0; i < RFCOM_NUM_LTE_BANDS; i++)
  {
   band_lpm_enable_status[i] = FALSE;
  }
  band_lpm_enable_status[RFCOM_BAND_LTE_B13] = TRUE;

  // initialize the lte_rf_devices
  for (log_dev = 0; log_dev < RFM_MAX_WAN_DEVICES; log_dev++)
  {
    rx_band[log_dev] = RFCOM_BAND_LTE_INVALID;
    tx_band[log_dev] = RFCOM_BAND_LTE_INVALID;

    for(rx_tx = 0; rx_tx < RFC_CONFIG_RXTX_MAX; rx_tx++)
    {
      for (band = 0; band < RFCOM_NUM_LTE_BANDS; band++)
      {
        for(dev_type = 0; dev_type < RFDEVICE_TYPE_MAX_NUM; dev_type++)
        {
          for (dev_instance = 0; dev_instance <RFC_MAX_DEVICE_INSTANCES_PER_BAND; dev_instance++)
          {
          lte_rf_devices[log_dev][rx_tx][band][dev_type][dev_instance] = NULL;
          }
        }
      }
    }
  }

  // initialize rfdevices with required rfc data
  init_rfdevices_for_all_bands();

}

// Destructor
rfc_lte_intf::~rfc_lte_intf()
{

  RFC_LTE_p = NULL;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function performs one RFC initialization during power-up for LTE.

   @details

*/
void rfc_lte_intf::init(void)
{
 /* do nothing for now */
}
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function puts the RFC Tx signals in their default state.

   @details

*/

void rfc_lte_intf::tx_disable(rfm_device_enum_type device,rfcom_lte_band_type band)
{
  rfc_cfg_params_type cfg = { RF_PATH_0,  RFM_DEVICE_0, RFC_CONFIG_TX, RFCOM_NUM_LTE_BANDS, RFC_REQ_DEFAULT_GET_DATA};
  rfc_sig_cfg_type *rf_signals = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE device not valid, device %d ", device);
    return;
  }

  /* set the previous band GRFC settings into default state */
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.band = band;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_data->sig_cfg_data_get(&cfg, &rf_signals);
  rfc_common_init_default(rf_signals);

  //reset the tx bands 
  tx_band[device] = RFCOM_NUM_LTE_BANDS;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function performs the RFC sleep for LTE mode, where the RFC signals will be put their default
   state.

   @details

*/
void rfc_lte_intf::mode_sleep(rfm_device_enum_type device, rfcom_lte_band_type band, uint8 alt_path)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE device not valid, device %d ", device);
    return;
  }

  cfg.alternate_path = alt_path;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.band = band;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);
  rfc_common_init_default(signal_list_ptr);

  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.band = band;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);
  rfc_common_init_default(signal_list_ptr);

  //reset the rx and tx bands 
  rx_band[device] = RFCOM_NUM_LTE_BANDS;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function performs the RFC exit for LTE mode, where the RFC signals will be put their default
   state.

   @details

*/
void rfc_lte_intf::mode_exit(rfm_device_enum_type device, rfcom_lte_band_type rx_bnd, rfcom_lte_band_type tx_bnd, uint8 alt_path)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE device not valid, device %d ", device);
    return;
  }

  cfg.alternate_path = alt_path;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.band = rx_bnd;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);
  rfc_common_init_default(signal_list_ptr);

  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.band = tx_bnd;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);
  rfc_common_init_default(signal_list_ptr);

  rx_band[device] = RFCOM_BAND_LTE_INVALID;
  tx_band[device] = RFCOM_BAND_LTE_INVALID;
 
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configure the RFC for LTE mode of operation.

   @details

*/
void rfc_lte_intf::mode_config(rfm_device_enum_type device)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE device not valid, device %d ", device);
    return;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,"Select LTE mode device %d", device);

  /* initialize the RFC rx and tx band information */
  rx_band[device] = RFCOM_BAND_LTE_INVALID;
  tx_band[device] = RFCOM_BAND_LTE_INVALID;
  
  // Setup Rx GRFCs
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_INIT;
  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr );
  rfc_common_config_signal(signal_list_ptr);

  // Setup Tx GRFCs
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.req = RFC_REQ_INIT;
  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr );
  rfc_common_config_signal(signal_list_ptr);

}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function return the LTE mode RFC configuration info. It enables us to make the 
   accessor function pointer point to cases where we have either TX or RX config and init
   flag set to true

   @details

*/
void *rfc_lte_intf::mode_config_get (rfm_device_enum_type device, rfc_mode_config_data_enum_type config_data)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  void                      *ret_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return ret_ptr;
  }

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE device not valid, device %d ", device);
    return ret_ptr;
  }

  cfg.alternate_path = 0;

  switch (config_data)
  {
  case RFC_MODE_TX_GRFC_ENGINE_DATA:

    // Setup configuration parameters
    cfg.logical_device = device;
    cfg.rx_tx = RFC_CONFIG_TX;
    cfg.req = RFC_REQ_INIT;

    rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);          
    ret_ptr = (void *)(signal_list_ptr);
    break;

  case RFC_MODE_RX_GRFC_ENGINE_DATA:

    // Setup configuration parameters
    cfg.logical_device = device;
    cfg.rx_tx = RFC_CONFIG_RX;
    cfg.req = RFC_REQ_INIT;

    rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);          
    ret_ptr = (void *)(signal_list_ptr);
    break;


  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unsupported data type %d", config_data);          
    break;
  }

  return ret_ptr;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function handles RFC related generic commands.

   @details

*/
int32 rfc_lte_intf::command_dispatch( rfm_device_enum_type device, int32 cmd, void *data )
{
  rfcom_lte_band_type band = RFCOM_BAND_LTE_INVALID;

  if(data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," data is NULL for device (%d)", device);
    return 0;
  }

  switch(cmd)
  {
    case RFC_LTE_LPM_ENABLE_STATUS:      
      band = ((rfc_lte_band_lpm_type*) data)->band;
      if (device == RFM_DEVICE_0 && band < RFCOM_NUM_LTE_BANDS)
      {
        ((rfc_lte_band_lpm_type*) data)->lpm_band_status = band_lpm_enable_status[band];
      }
      else
      {
        MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR," invalid cmd (%d) for device (%d) and band(%d)", cmd, device, band);
      }
      break;

    default:
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
            "invalid cmd (%d) for device (%d)", cmd, device);
      break;
  }
  return 0;
}
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function resets the GRFC list for the LTE Rx for the specified band. 

   @details

*/
void rfc_lte_intf::reset_grfc_band_config( rfm_device_enum_type device, rfcom_lte_band_type band, uint8 alt_path)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"reset_grfc_band_config: Invalid device %d", device);
    return;
  }

  // Setup common configuration parameters
  cfg.alternate_path = alt_path;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  cfg.band = (int)band;
  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);
  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"reset_grfc_band_config: Setting to defaults,RX band enum %d on device %d", band, device);
  rfc_common_init_default(signal_list_ptr);
}
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the RFC for the LTE Rx for the specified band. 

   @details

*/
void rfc_lte_intf::rx_band_config( rfm_device_enum_type device, rfcom_lte_band_type band, uint8 alt_path )
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE device not valid, device %d ", device);
    return;
  }

  // Setup common configuration parameters
  cfg.alternate_path = alt_path;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  /*Cleanup for previous band done, now start configuration for the new band*/
  signal_list_ptr = NULL;
  cfg.band = (int)band;
  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);
  rfc_common_config_signal(signal_list_ptr);

  /*Update the curr band*/
  rx_band[device] = band;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the  rx band RFC configuration ino. for the specified rx band.

   @details

*/
void *rfc_lte_intf::rx_band_config_get (rfm_device_enum_type device, rfcom_lte_band_type band,
                                        rfc_band_config_data_enum_type config_data, uint8 alt_path)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type          *signal_list_ptr = NULL;
  void                      *ret_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return ret_ptr;
  }

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE device not valid, device %d ", device);
    return ret_ptr;
  }


  // Setup configuration parameters
  cfg.alternate_path = alt_path;
  cfg.logical_device = device;
  cfg.band = band;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);

  /* Retrieve requested data if exists */
  switch(config_data)
  {
    case RFC_GPIO_DATA:
    case RFC_GRFC_DATA:
    case RFC_RF_CTRL_DATA:
      ret_ptr = (void *)(signal_list_ptr);
      break;
    case RFC_CORE_DATA:
      // Dime RFC Changes : Should query the device instead of RFC to get the port info.
      // ret_ptr = (void *)(&(signal_list_ptr->rf_asic_info.rf_asic_port_id));
      break;
    default :
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unsupported data type %d", config_data);          
      break;
  }

  return (ret_ptr);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the RFC for the LTE Tx for the specified band. 

   @details

*/
void rfc_lte_intf::tx_band_config( rfm_device_enum_type device, rfcom_lte_band_type band )
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE device not valid, device %d ", device);
    return;
  }


  MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"Select LTE TX band enum %d on device %d", band, device);

  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  /*Before configuring the new band(parameter "band"), need to move back the GRFC/GPIOs configured for previous band
  (tx_band[device])to their default state*/
  if((tx_band[device] != band) && (tx_band[device] != RFCOM_BAND_LTE_INVALID))
  {
    cfg.band = (int)tx_band[device];
    rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);
    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"tx_band_config: Setting to defaults, TX band enum %d on device %d", band, device);
    rfc_common_init_default(signal_list_ptr);
    
    /* Need to reconfig Rx signal in case previous tx band share 
       the same signal with current Rx band*/
    cfg.rx_tx = RFC_CONFIG_RX;
    signal_list_ptr = NULL;
    cfg.band = (int)band;
    rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);
    rfc_common_config_signal(signal_list_ptr);

    cfg.rx_tx = RFC_CONFIG_TX;
  }

  signal_list_ptr = NULL;
  cfg.band = band;
  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);
  rfc_common_config_signal(signal_list_ptr);

  /*Update the curr band*/
  tx_band[device] = band;
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the  tx band RFC configuration ino. for the specified tx band.

   @details

*/
void *rfc_lte_intf::tx_band_config_get (rfm_device_enum_type device, rfcom_lte_band_type band,
                                        rfc_band_config_data_enum_type config_data)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  void                      *ret_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return ret_ptr;
  }

  if( (device < RFM_DEVICE_0) || (device > RFM_DEVICE_5) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE device not valid, device %d ", device);
    return ret_ptr;
  }



  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.band = band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  rfc_data->sig_cfg_data_get(&cfg, &signal_list_ptr);

  /* Retrieve requested data if exists */
  switch(config_data)
  {
    case RFC_GPIO_DATA:
    case RFC_GRFC_DATA:
    case RFC_RF_CTRL_DATA:
      ret_ptr = (void *)(signal_list_ptr);
      break;
    case RFC_CORE_DATA:
      // Dime RFC changes : should query the device to get the port info.
      // ret_ptr = (void *)(&(signal_list_ptr->rf_asic_info.rf_asic_port_id));
      break;
    default :
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unsupported data type %d", config_data);          
      break;
  }

  return (ret_ptr);
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function configures the vregs for LTE mode

   @details

*/
void rfc_lte_intf::manage_vregs(rfc_power_management_info* pm_info)
{
  rfc_vreg_param vreg_param(pm_info->path, pm_info->mode);
  vreg_param.set_band(pm_info->band);
  vreg_param.set_rfchip_number(pm_info->rf_chip_num);
  vreg_param.set_rf_state(pm_info->state);

  rfc_vreg_mgr *vreg_mgr_p = rfc_vreg_mgr::get_instance();
  if (vreg_mgr_p != NULL)
  {
    vreg_mgr_p->lte_manage_power(vreg_param);
  }
  else
  {
    ERR_FATAL("Cannot Create VREG_MGR for  N6600 NA card",0,0,0);
  }

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
void** rfc_lte_intf::get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                      rfcom_lte_band_type band, rfdevice_type_enum_type dev_type)
{
  void **dev_obj = NULL;
  uint8 antenna = 0;
  rfdevice_class *asd_tuner_mgr = NULL;

  if (rfm_dev >= RFM_MAX_WAN_DEVICES || rx_tx >= RFC_CONFIG_RXTX_MAX 
      || band >= RFCOM_NUM_LTE_BANDS || dev_type >= RFDEVICE_TYPE_MAX_NUM)
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_lte_intf::get_device_obj(%d, %d, %d, %d): Incorrect Parameters",
          rfm_dev, rx_tx, band, dev_type);
    return dev_obj;
  }

  dev_obj = lte_rf_devices[rfm_dev][rx_tx][band][dev_type];

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
#endif /* FEATURE_RF_ASDIV*/

  return dev_obj;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the rfdevices with the rfc dependent data.

  @details
  This function initializes the rfdevices with the rfc dependent data.

*/
void rfc_lte_intf::init_rf_devices(rfc_cfg_params_type *cfg, rfc_device_info_type *device_info_ptr)
{
  rfdevice_class *cmn_dev_obj = NULL;
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  uint32 instance = 0;
  uint8 i = 0;
  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  rfcom_band_type_u band_u;
  uint8 trx_dev_instance = 0;
  uint8 pa_dev_instance = 0;
  uint8 asm_dev_instance = 0;
  uint8 papm_dev_instance = 0;
  uint8 tuner_dev_instance = 0;
  uint8 hdet_dev_instance = 0;
  uint8 coupler_dev_instance = 0;
  uint8 xsw_dev_instance = 0;
  uint8 temp_port_state = RFC_PORT_STATE_DEFAULT;
  uint8 default_port_state = RFC_PORT_STATE_A;
  rfdevice_trx_lte_rx *rx_dev_obj = NULL;
  rfdevice_trx_lte_tx *tx_dev_obj = NULL;
  rfdevice_trx_phy_path_enum_type phy_path= RFDEVICE_TRX_MAX_PATHS;
  uint8 ant_num = 0;
  int num_trx_rx_device = 0;

#ifdef FEATURE_RF_ASDIV
  rfdevice_class *asd_tuner_mgr = NULL;
#endif

  rfc_sig_cfg_type* mode_rf_signals = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (NULL == rfc_cmn)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC CMN Object is NOT Created", 0);
    return;
  }

  if (NULL == cfg || NULL == device_info_ptr)
  {
    /* band is not supported */
    // MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"init_rf_devices(0x%x, 0x%x) : Bad parameters!", cfg, device_info_ptr);
    return;
  }
  if (cfg->logical_device >= RFM_MAX_WAN_DEVICES || cfg->rx_tx >= RFC_CONFIG_RXTX_MAX 
      || cfg->band >= RFCOM_NUM_LTE_BANDS)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_lte_intf::init_rf_devices: logical_dev, rx_tx, band (%d, %d, %d ): Incorrect Parameters",
          cfg->logical_device, cfg->rx_tx, cfg->band);
    return;
  }

  ant_num = device_info_ptr->ant_num;
  band_u.lte_band = (rfcom_lte_band_type)cfg->band;

  if (cfg->port_state == RFC_PORT_STATE_A)
  {
    temp_port_state = RFC_PORT_STATE_A;

  while (i < device_info_ptr->num_devices)
  {
    dev_type = device_info_ptr->rf_asic_info[i].device_type;
    instance = device_info_ptr->rf_asic_info[i].instance;
    cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(dev_type, (uint8)instance);
    if (cmn_dev_obj == NULL)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"cmd dev obj is NULL %d %d", dev_type,instance);
    i++;
      continue;
    }

    switch(dev_type)
    {
      case RFDEVICE_TRANSCEIVER:
          /* Push the band-port information to device */
        if (RFC_CONFIG_RX == cfg->rx_tx) /* rx device data */
        {
          if (trx_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
          {
            /* Read phy path from RFC */
            phy_path= 
              (rfdevice_trx_phy_path_enum_type)
              device_info_ptr->rf_asic_info[trx_dev_instance].phy_path_num;

            /* Get the RX device ptr */
            rx_dev_obj = 
              (rfdevice_trx_lte_rx *)
              ((rfdevice_rxtx_common_class*)cmn_dev_obj)\
                    -> get_tech_instance( RFM_LTE_MODE,
                                          RFDEVICE_RX, 
                                          phy_path);

            if( NULL == rx_dev_obj)
            {
              RF_MSG(RF_ERROR, "RFC LTE: Unable to get Rx device ptr!!");
            }

            if(cfg->logical_device > RFM_MAX_WAN_DEVICES)
            {
              RF_MSG(RF_ERROR, "RFC LTE: cfg->logical_device out of bound");
            }

            /* Store Rx Device ptr */
            lte_rf_devices\
              [cfg->logical_device][cfg->rx_tx]\
              [cfg->band][dev_type][trx_dev_instance] = rx_dev_obj;

            /* Call Rx set band data */
            rfdevice_lte_rx_set_band_data
            (
              rx_dev_obj,
              (rfcom_lte_band_type)cfg->band, 
              device_info_ptr->rf_asic_info[trx_dev_instance].data,
              RFC_ASIC_INFO_DATA_SIZE ,
              cfg->logical_device
            );

            trx_dev_instance++;
          } /*end of  if (trx_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND) */
          else
          {
            RF_MSG_1 ( RF_ERROR,"Trx device Prx instance overflow trx_dev_instance: %d", 
                       trx_dev_instance );
            return;
          }/* end of else if (trx_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND) */
        }/* end of if ( RFC_CONFIG_RX == cfg->rx_tx ) */

        if (RFC_CONFIG_TX == cfg->rx_tx) /* tx device data */
        {
          if (trx_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
          {
              tx_dev_obj =
              (rfdevice_trx_lte_tx *)
              ((rfdevice_rxtx_common_class*)cmn_dev_obj)\
                 -> get_tech_instance( RFM_LTE_MODE,
                                       RFDEVICE_TX, 
                                       RFDEVICE_TRX_PATH_0 );

          /* Temporarilty push Device index 0 data to the transceiver.
          This assumes index 0 in the list of devices is the transceiver device.
          This needs to be updated to find the transceiver device using RFDEVICE_TRANSCEIVER id*/
              lte_rf_devices\
                 [cfg->logical_device][cfg->rx_tx]\
                 [cfg->band][dev_type][trx_dev_instance] = tx_dev_obj;

              rfdevice_lte_tx_set_band_data
              (
                tx_dev_obj,
                                        (rfcom_lte_band_type)cfg->band, 
                                        device_info_ptr->rf_asic_info[trx_dev_instance].data,
                                        RFC_ASIC_INFO_DATA_SIZE,
                cfg->logical_device
              );
        }
          else
          {
              RF_MSG_1
              (
                 RF_ERROR,
                 "TRX Tx device instance overflow "
                 "trx_dev_instance: %d", 
                 trx_dev_instance
              );
        }
        }/* end of if (RFC_CONFIG_TX == cfg->rx_tx */

      if (cfg->rx_tx == RFC_CONFIG_RX)
      {
        num_trx_rx_device++;
        /*call the below api to capture the transciever->phy path->modem chain(ADC) mapping to perform adc dc cal.*/
        if(num_trx_rx_device == 1)
        {
        if(!rfc_cmn->set_trx_phy_path_adc_mapping(
                                                  device_info_ptr->modem_chain,                    /*modem chain to map the ADC*/
                                                  device_info_ptr->rf_asic_info[i].phy_path_num,   /*physical path*/
                                                  device_info_ptr->rf_asic_info[i].instance,       /*Transciever device type instance*/
                                                  RFM_LTE_MODE,                                    /*tech*/
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
      break; /* end case RFDEVICE_TRANSCEIVER */

      case RFDEVICE_ASM:
        if (asm_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
        {
        lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][asm_dev_instance] =\
          cmn_dev_obj;
        asm_dev_instance++;
        band_u.lte_band = (rfcom_lte_band_type)cfg->band;

        if(cfg->rx_tx == RFC_CONFIG_RX)
        {
            /* Update the Rx ASM Port state with temp_port_state and Call the ASM API below */
            ((rfdevice_asm *)cmn_dev_obj)->set_rx_switch_state(RFCOM_LTE_MODE,
                                                               band_u,
                                                               temp_port_state);

          ((rfdevice_asm *)cmn_dev_obj)->set_rx_band_map
                                         (
                                           cfg->logical_device,
                                           RFCOM_LTE_MODE, 
                                           band_u, 
                                           device_info_ptr->rf_asic_info[i].data, 
                                           RFC_ASIC_INFO_DATA_SIZE
                                         );

            /* Update the Tx ASM Port state with default_port_state and Call the ASM API below */
           ((rfdevice_asm *)cmn_dev_obj)->set_rx_switch_state(RFCOM_LTE_MODE,
                                                              band_u,
                                                              default_port_state);
        }
        else if (cfg->rx_tx == RFC_CONFIG_TX)
        {
            /* Update the Rx ASM Port state with temp_port_state and Call the ASM API below */
            ((rfdevice_asm *)cmn_dev_obj)->set_tx_switch_state(RFCOM_LTE_MODE,
                                                               band_u,
                                                               temp_port_state);
          ((rfdevice_asm *)cmn_dev_obj)->set_tx_band_map
                                         (
                                           cfg->logical_device,
                                           RFCOM_LTE_MODE, 
                                           band_u, 
                                           device_info_ptr->rf_asic_info[i].data, 
                                           RFC_ASIC_INFO_DATA_SIZE
                                         );

             /* Update the Tx ASM Port state with default_port_state and Call the ASM API below */
           ((rfdevice_asm *)cmn_dev_obj)->set_tx_switch_state(RFCOM_LTE_MODE,
                                                              band_u,
                                                              default_port_state);
        }
        }
        else
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASM device instance overflow asm_dev_instance:"
                     " %d", asm_dev_instance);
        }
        break;
        

       case RFDEVICE_PA:
        /*set band map is only for QPA. for HCPA and sattelite PA this should not affect.
        The set_band_map() is empty for rffe based PA.*/
        if(cfg->rx_tx == RFC_CONFIG_TX)
        {
        if (pa_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
        {
          lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][pa_dev_instance] =\
            cmn_dev_obj;
          pa_dev_instance++;
          band_u.lte_band = (rfcom_lte_band_type)cfg->band;
          ((rfdevice_pa *)cmn_dev_obj)->set_band_map
                                         (
                                           RFCOM_LTE_MODE, 
                                           band_u, 
                                           device_info_ptr->rf_asic_info[i].data, 
                                           RFC_ASIC_INFO_DATA_SIZE
                                         );
        }
        else
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"PA device instance overflow pa_dev_instance:"
                     " %d", pa_dev_instance);
        }
        }
        break;
      case RFDEVICE_PAPM:
        if(cfg->rx_tx == RFC_CONFIG_TX)
        {
          if (papm_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
          {
          lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][papm_dev_instance] =\
            cmn_dev_obj;  
          band_u.lte_band = (rfcom_lte_band_type)cfg->band;
          if (cmn_dev_obj != NULL)
          {
            ((rfdevice_papm *)cmn_dev_obj)->set_band_map( cfg->logical_device,
                                                          RFCOM_LTE_MODE, 
                                                          band_u, 
                                                          device_info_ptr->rf_asic_info[i].data, 
                                                          RFC_ASIC_INFO_DATA_SIZE);
          }
                                        
          papm_dev_instance++;
          }
          else
          {
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"PAPM device instance overflow papm_dev_instance:"
                       " %d", papm_dev_instance);
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
          lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][xsw_dev_instance] = cmn_dev_obj; 
          band_u.lte_band = (rfcom_lte_band_type)cfg->band;

          if (cfg->req == RFC_REQ_ENABLE_ANT_SWAP_1)
          {

            ((rfdevice_xsw *)cmn_dev_obj)->set_band_map_swap(RFCOM_LTE_MODE, band_u, 
                                                             device_info_ptr->rf_asic_info[i].data, 
                                                             RFC_ASIC_INFO_DATA_SIZE);
            ((rfdevice_xsw *)cmn_dev_obj)->set_srs_band_map_swap(RFCOM_LTE_MODE, band_u, 
                                                                 device_info_ptr->rf_asic_info[i].data, 
                                                                 RFC_ASIC_INFO_DATA_SIZE);
          }/* !RFC_CONFIG_RX */
          else if (cfg->req == RFC_REQ_DISABLE_ANT_SWAP_1)
          {

             ((rfdevice_xsw *)cmn_dev_obj)->set_band_map_default(RFCOM_LTE_MODE, band_u, 
                                                                 device_info_ptr->rf_asic_info[i].data, 
                                                                 RFC_ASIC_INFO_DATA_SIZE);
             ((rfdevice_xsw *)cmn_dev_obj)->set_srs_band_map_default(RFCOM_LTE_MODE, band_u, 
                                                                     device_info_ptr->rf_asic_info[i].data, 
                                                                     RFC_ASIC_INFO_DATA_SIZE);
          }/* !RFC_CONFIG_TX */ 
          else
          {
            RF_MSG_1(RF_ERROR, "Invalid configuration provided in cfg->req", 0);
          }
        } /* end IF cmn_dev_obj==NULL */         
        break; /* end case RFDEVICE_XSW */

      case RFDEVICE_TUNER:
        if ( (cfg->rx_tx == RFC_CONFIG_TX) || (cfg->rx_tx == RFC_CONFIG_RX) )
        {
          if (tuner_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
          {
            /*** Get Tuner Device ***/
            lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][tuner_dev_instance] = cmn_dev_obj; 
            band_u.lte_band = (rfcom_lte_band_type)cfg->band;
            if ((cmn_dev_obj != NULL) && (cfg->rx_tx == RFC_CONFIG_TX))
            {
              /*** Set distortion configuration ***/
              ((rfdevice_antenna_tuner *)cmn_dev_obj)->set_band_distortion_config_map(RFCOM_LTE_MODE, band_u, 
                 device_info_ptr->rf_asic_info[i].data);
            }

            /*** Get Tuner manager ***/
            /*Grab the associated Tuner manager instance, for now there is one to one mapping between
               a tuner device and tuner manager*/
            cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(RFDEVICE_TUNER_MANAGER, (uint8)instance);

            /* Make sure there is a valid Antenna tuner manager */
            if (cmn_dev_obj != NULL)
            {

              rfdevice_class *tuner_port_obj = NULL;
              /* For LTE Carrier Aggregation(CA), 
                 - A tuner physical device can be shared for primary paths on each of P-cell and S-cell.
                 - A tuner physical device can be shared for diversity paths on each of P-cell and S-cell.
                 - A tuner physical device will NEVER be shared for both primary and diversity paths of P-cell (or S-cell).

                 Hence Port-0 of Tuner manager can be assigned for RFM path-0 and RFM path-1 (P-cell paths)
                      and Port-1 of Tuner manager will be assigned for RFM path-2 and RFM path-3 (S-cell paths)

                 Port-0 and Port-1 are synonymous from Tuner Manager point of view, hence a tech can make request 
                 using any of the ports. */

              if ( (cfg->logical_device == RFM_DEVICE_0) || (cfg->logical_device == RFM_DEVICE_1) )
              {
                /*--- Get Port-0 object ---*/
                tuner_port_obj =  rfcommon_atuner_manager_factory_get_port_instance ( cmn_dev_obj,
                                                                                      ATUNER_CA_PORT_0 );
              
                lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][tuner_dev_instance] = tuner_port_obj;
              }

              if ( (cfg->logical_device == RFM_DEVICE_2) || (cfg->logical_device == RFM_DEVICE_3) )
              {
                /*--- Get Port-1 object ---*/
                tuner_port_obj =  rfcommon_atuner_manager_factory_get_port_instance ( cmn_dev_obj,
                                                                                      ATUNER_CA_PORT_1 );

                lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][tuner_dev_instance] = tuner_port_obj;
              }

              if ( (cfg->logical_device == RFM_DEVICE_4) || (cfg->logical_device == RFM_DEVICE_5) )
              {
                /*--- Get Port-2 object ---*/
                tuner_port_obj =  rfcommon_atuner_manager_factory_get_port_instance ( cmn_dev_obj,
                                                                                      ATUNER_CA_PORT_2 );

                lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][tuner_dev_instance] = tuner_port_obj;
              }
            }
            
            tuner_dev_instance++;
          }
          else
          {
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Tuner device instance overflow tuner_dev_instance:"
                       " %d", tuner_dev_instance);
          }
        }
        break;

       case RFDEVICE_HDET:
          if (cfg->rx_tx == RFC_CONFIG_TX)
          {
             if (hdet_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
             {
                lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][hdet_dev_instance] =\
                   cmn_dev_obj;
                hdet_dev_instance++;
             }
             else
             {
                MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "HDET device instance overflow hdet_dev_instance:"
                      " %d", hdet_dev_instance);
             }
          }

          break; 
 
      case RFDEVICE_COUPLER:
          if (cfg->rx_tx == RFC_CONFIG_TX)
          {
             if (coupler_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
             {
               lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][coupler_dev_instance] =\
                  cmn_dev_obj;
               band_u.lte_band = (rfcom_lte_band_type)cfg->band;
               if ((cmn_dev_obj != NULL) && (cfg->rx_tx == RFC_CONFIG_TX))
               {
                 /*** Set distortion configuration ***/
                 ((rfdevice_coupler *)cmn_dev_obj)->set_band_config_map(RFCOM_LTE_MODE, band_u, 
                  device_info_ptr->rf_asic_info[i].data);
               }
               coupler_dev_instance++;
             }
             else
             {
                MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "COUPLER device instance overflow coupler_dev_instance:"
                      " %d", coupler_dev_instance);
             }
          }

          break; 
      default:
        break;
    }

    i++;
  }

  /* Final Check: If there are no PAs specified in RFC (as is the current
     case for GRFC PA) then append a generic GRFC PA
  */
#ifndef T_RUMI_EMULATION
  if( (cfg->rx_tx == RFC_CONFIG_TX) 
      && (pa_dev_instance == 0)
      && (rfc_data != NULL))
  {
    /* Create grfc_pa instance if it has not been created */
    if(rfc_lte_common_grfc_pa_device == NULL)
    {
      rfc_lte_common_grfc_pa_device = new grfc_pa();
    }

    if(rfc_lte_common_grfc_pa_device != NULL)
    {
      /* If grfc_pa device is valid then program sig info (band_map) */      
      /* get band sig data */
      rfc_data->sig_cfg_data_get(cfg, &mode_rf_signals);
      /* program band map */
      ((rfdevice_pa*)rfc_lte_common_grfc_pa_device)->set_band_map( RFM_LTE_MODE, 
                                                                   band_u, 
                                                                   (int32*) mode_rf_signals, 
                                                                   0 /* Don't care */ );

      /* Assign grfc_pa instance to table */
      lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_PA][pa_dev_instance] =\
            (void*)rfc_lte_common_grfc_pa_device;
          pa_dev_instance++;
    }
    else
    {
      /* Return FALSE if we failed to create the PA instance. */
      RF_MSG_1(RF_FATAL,"Unable to create GRFC PA instance!", 0);
    }
  } /* end if for adding GRFC PA device */
#endif

#ifdef FEATURE_RF_ASDIV

  ant_params[cfg->logical_device][cfg->rx_tx][cfg->band].antenna_num = ant_num ;

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
         
  if (tuner_dev_instance == 0 ) // if the tuner device is NOT present
  {
    rfdevice_class *tuner_port_obj = NULL;

    /* get the dummy Tuner object if needed. If there are no tuners in the rf-card, Dummy Tuner Mgr will not be created.*/
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

      lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][RFDEVICE_TUNER_MANAGER][tuner_dev_instance] = tuner_port_obj;
    }
  }

#endif /* FEATURE_RF_ASDIV */

  }
  else if (cfg->port_state == RFC_PORT_STATE_B)
  {
    temp_port_state = RFC_PORT_STATE_B;

    while (i < device_info_ptr->num_devices)
    {
      dev_type = device_info_ptr->rf_asic_info[i].device_type;
      instance = device_info_ptr->rf_asic_info[i].instance;
      cmn_dev_obj = rfc_cmn->get_cmn_rf_device_object(dev_type, (uint8)instance);
      if (cmn_dev_obj == NULL)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"cmd dev obj is NULL %d %d", dev_type,instance);
        i++;
        continue;
      }

      switch(dev_type)
      {
      case RFDEVICE_ASM:
        if (asm_dev_instance < RFC_MAX_DEVICE_INSTANCES_PER_BAND)
        {
          lte_rf_devices[cfg->logical_device][cfg->rx_tx][cfg->band][dev_type][asm_dev_instance] =\
            cmn_dev_obj;
          asm_dev_instance++;
          band_u.lte_band = (rfcom_lte_band_type)cfg->band;

          if(cfg->rx_tx == RFC_CONFIG_RX)
          {
            /* Update the Rx ASM Port state with temp_port_state and Call the ASM API below */
            ((rfdevice_asm *)cmn_dev_obj)->set_rx_switch_state(RFCOM_LTE_MODE,
                                                               band_u,
                                                               temp_port_state);
            ((rfdevice_asm *)cmn_dev_obj)->set_rx_band_map
                                           (
                                             cfg->logical_device,
                                             RFCOM_LTE_MODE, 
                                             band_u, 
                                             device_info_ptr->rf_asic_info[i].data, 
                                             RFC_ASIC_INFO_DATA_SIZE
                                           );

            /* Update the Rx ASM Port state with default_port_state and Call the ASM API below */
            ((rfdevice_asm *)cmn_dev_obj)->set_rx_switch_state(RFCOM_LTE_MODE,
                                                               band_u,
                                                               default_port_state);
          }
          else if (cfg->rx_tx == RFC_CONFIG_TX)
          {
            /* Update the Tx ASM Port state with temp_port_state and Call the ASM API below */
            ((rfdevice_asm *)cmn_dev_obj)->set_tx_switch_state(RFCOM_LTE_MODE,
                                                               band_u,
                                                               temp_port_state);
            ((rfdevice_asm *)cmn_dev_obj)->set_tx_band_map
                                           (
                                             cfg->logical_device,
                                             RFCOM_LTE_MODE, 
                                             band_u, 
                                             device_info_ptr->rf_asic_info[i].data, 
                                             RFC_ASIC_INFO_DATA_SIZE
                                           );

            /* Update the Tx ASM Port state with default_port_state and Call the ASM API below */
            ((rfdevice_asm *)cmn_dev_obj)->set_tx_switch_state(RFCOM_LTE_MODE,
                                                               band_u,
                                                               default_port_state);
          }
        }
        else
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"ASM device instance overflow asm_dev_instance:"
                     " %d", asm_dev_instance);
        }
        break;

      default:
        break;
      }
      i++;
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the rfdevices with the rfc dependent data.

  @details
  This function initializes the rfdevices with the rfc dependent data.

*/
void rfc_lte_intf::init_rfdevices_for_all_bands(void)
{
  uint8 band;
  rfc_cfg_params_type       cfg;
  rfc_device_info_type *device_info_ptr;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }

  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_A; // By default we should configure Port A settings.

  for ( band = RFCOM_BAND_LTE_B1; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
    {
      cfg.band = band;    
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
      init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_1;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_A; // By default we should configure Port A settings.

  for ( band = RFCOM_BAND_LTE_B1; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
    {
      cfg.band = band;
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
      init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_A; // By default we should configure Port A settings.

  for ( band = RFCOM_BAND_LTE_B1; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
    {
      cfg.band = band;
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
      init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_2;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_A; // By default we should configure Port A settings.

  for ( band = RFCOM_BAND_LTE_B1; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
    {
      cfg.band = band;    
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
      init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_3;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_A; // By default we should configure Port A settings.

  for ( band = RFCOM_BAND_LTE_B1; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
    {
      cfg.band = band;
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
      init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_4;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_A; // By default we should configure Port A settings.

  for ( band = RFCOM_BAND_LTE_B1; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
    {
      cfg.band = band;
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
      init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.logical_device = RFM_DEVICE_5;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_A; // By default we should configure Port A settings.

  for ( band = RFCOM_BAND_LTE_B1; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
    {
      cfg.band = band;
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
      init_rf_devices(&cfg, device_info_ptr);
    }
  }

  /* Need to support Port B config for RF Device ASM only for LTE TDD bands only */
  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_B;

  for ( band = RFCOM_BAND_LTE_B33; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if (rflte_util_is_band_tdd((rfcom_lte_band_type)band) == TRUE)
    {
      if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
      {
        cfg.band = band;    
        rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
        init_rf_devices(&cfg, device_info_ptr);
      }
    }
  }

  cfg.logical_device = RFM_DEVICE_1;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_B;

  for ( band = RFCOM_BAND_LTE_B33; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if (rflte_util_is_band_tdd((rfcom_lte_band_type)band) == TRUE)
    {
      if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
      {
        cfg.band = band;
        rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
        init_rf_devices(&cfg, device_info_ptr);
      }
    }
  }

  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_B;

  for ( band = RFCOM_BAND_LTE_B33; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if (rflte_util_is_band_tdd((rfcom_lte_band_type)band) == TRUE)
    {
      if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
      {
        cfg.band = band;
        rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
        init_rf_devices(&cfg, device_info_ptr);
      }
    }
  }

  cfg.logical_device = RFM_DEVICE_2;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_B;

  for ( band = RFCOM_BAND_LTE_B33; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if (rflte_util_is_band_tdd((rfcom_lte_band_type)band) == TRUE)
    {
      if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
      {
        cfg.band = band;    
        rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
        init_rf_devices(&cfg, device_info_ptr);
      }
    }
  }

  cfg.logical_device = RFM_DEVICE_3;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_B;

  for ( band = RFCOM_BAND_LTE_B33; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if (rflte_util_is_band_tdd((rfcom_lte_band_type)band) == TRUE)
    {
      if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
      {
        cfg.band = band;
        rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
        init_rf_devices(&cfg, device_info_ptr);
      }
    }
  }

  cfg.logical_device = RFM_DEVICE_4;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_B;

  for ( band = RFCOM_BAND_LTE_B33; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if (rflte_util_is_band_tdd((rfcom_lte_band_type)band) == TRUE)
    {
      if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
      {
        cfg.band = band;
        rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
        init_rf_devices(&cfg, device_info_ptr);
      }
    }
  }
  cfg.logical_device = RFM_DEVICE_5;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_B;

  for ( band = RFCOM_BAND_LTE_B33; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if (rflte_util_is_band_tdd((rfcom_lte_band_type)band) == TRUE)
    {
      if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
      {
        cfg.band = band;
        rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
        init_rf_devices(&cfg, device_info_ptr);
      }
    }
  }

  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
  cfg.port_state = RFC_PORT_STATE_A; // By default we should configure Port A settings.

  for ( band = RFCOM_BAND_LTE_B1; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
    {
      cfg.band = band;    
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
      init_rf_devices(&cfg, device_info_ptr);
    }
  }

  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DISABLE_ANT_SWAP_1;
  cfg.port_state = RFC_PORT_STATE_A; // By default we should configure Port A settings.

  for ( band = RFCOM_BAND_LTE_B1; band < RFCOM_NUM_LTE_BANDS; band++ )
  {
    if ((rflte_nv_get_tx_band_support((rfcom_lte_band_type)band) == TRUE)||
         (band == RFCOM_BAND_LTE_B29 ))//Since B29 is Rx only, we need this condition to enable init for B29
    {
      cfg.band = band;    
      rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
      init_rf_devices(&cfg, device_info_ptr);
    }
  }

  /* NS07 Settings */

  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_NS07; // By default we should configure Port A settings.

  cfg.band = RFCOM_BAND_LTE_B13;    
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
  init_rf_devices(&cfg, device_info_ptr);


  cfg.logical_device = RFM_DEVICE_1;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_NS07; // By default we should configure Port A settings.

  
  cfg.band = RFCOM_BAND_LTE_B13;    
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
  init_rf_devices(&cfg, device_info_ptr);

  cfg.logical_device = RFM_DEVICE_0;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_NS07; // By default we should configure Port A settings.

  cfg.band = RFCOM_BAND_LTE_B13;    
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
  init_rf_devices(&cfg, device_info_ptr);

  cfg.logical_device = RFM_DEVICE_2;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_NS07; // By default we should configure Port A settings.

  cfg.band = RFCOM_BAND_LTE_B13;    
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
  init_rf_devices(&cfg, device_info_ptr);

  cfg.logical_device = RFM_DEVICE_3;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_NS07; // By default we should configure Port A settings.

  cfg.band = RFCOM_BAND_LTE_B13;    
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
  init_rf_devices(&cfg, device_info_ptr);

  cfg.logical_device = RFM_DEVICE_4;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_NS07; // By default we should configure Port A settings.

  cfg.band = RFCOM_BAND_LTE_B13;    
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
  init_rf_devices(&cfg, device_info_ptr);

  cfg.logical_device = RFM_DEVICE_5;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state = RFC_PORT_STATE_NS07; // By default we should configure Port A settings.

  cfg.band = RFCOM_BAND_LTE_B13;    
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);
  init_rf_devices(&cfg, device_info_ptr);

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function obtains GRFC signal information and configures them

  @details
  This function obtains GRFC signal information of switches and configures them to setup
  feedback path for XPT calibration

*/
boolean rfc_lte_intf::setup_fb_path(rfm_device_enum_type device_fb, rfcom_lte_band_type band, boolean et_enable)
{
  rfc_cfg_params_type cfg;
  rfc_sig_cfg_type       *signal_list_ptr = NULL;
  rfc_lte_data *rfc_lte_data_ptr = NULL;

  cfg.alternate_path = 0;    
  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device_fb;
  cfg.req = et_enable?RFC_REQ_ENABLE_XPT_CAPTURE:RFC_REQ_DISABLE_XPT_CAPTURE;

  rfc_lte_data_ptr = rfc_lte_data::get_instance();
  if (rfc_lte_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return(FALSE);
  }
  rfc_lte_data_ptr->sig_cfg_data_get(&cfg, &signal_list_ptr);

    if (signal_list_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE signal_list_ptr is NULL", 0);
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
boolean rfc_lte_intf::setup_fb_device(rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_lte_band_type band, uint32 tx_freq)
{
  rfc_cfg_params_type cfg;
  rfc_intf *rfc_cmn = NULL;
  rfdevice_class *trx_device = NULL;
  rfdevice_rxtx_common_class* trx_cmn_device = NULL;  
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_lte_data *rfc_lte_data_ptr = NULL;
  rfdevice_trx_lte_tx *tx_device = NULL;  
  rfdevice_lte_script_data_type script_data = {TRUE,NULL};  
  uint32 temp_instance;
  boolean set_path = TRUE;
  rfc_cmn_properties_type *rfc_cmn_properties_ptr = NULL;
  rfdevice_rxtx_common_class* device_ptr = NULL;
  
  cfg.alternate_path = 0;
  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  rfc_lte_data_ptr = rfc_lte_data::get_instance();

  if (rfc_lte_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: RFC LTE Data Object is NOT Created", 0);
    return(FALSE);
  }
  
  rfc_lte_data_ptr->devices_cfg_data_get(&cfg, &device_info_ptr);

    if (device_info_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: RFC LTE device_info_ptr for Tx is NULL", 0);
    return(FALSE);
  }
    
  temp_instance =device_info_ptr->rf_asic_info[0].instance;

  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device_fb;
  cfg.req = RFC_REQ_ENABLE_XPT_CAPTURE;

  rfc_lte_data_ptr->devices_cfg_data_get(&cfg, &device_info_ptr);

    if (device_info_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: RFC LTE device_info_ptr for Rx is NULL", 0);
    return(FALSE);
  }
      
  rfc_cmn = rfc_intf::get_instance();  
  trx_device= rfc_cmn->get_cmn_rf_device_object(RFDEVICE_TRANSCEIVER, device_info_ptr->rf_asic_info[0].instance);
  if( trx_device != NULL )
  {
    /* extract the lte device object from the cmn_device object and store it */ 
    trx_cmn_device = (rfdevice_rxtx_common_class *)trx_device;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: trx_cmn_device not created", 0);
    return(FALSE);
  }
      
      
  if (temp_instance !=device_info_ptr->rf_asic_info[0].instance)
  {  
    tx_device = (rfdevice_trx_lte_tx*)(((rfdevice_rxtx_common_class *)trx_device)\
                                       ->get_tech_instance
                                       (
                                                                    RFM_LTE_MODE, 
                                         RFDEVICE_TX,
                                         RFDEVICE_TRX_PATH_0
                                        )
                                       );
  
    if (NULL != tx_device)
    {
      rfdevice_lte_tx_tune_to_default_port( tx_device,
                                            band, 
                                            (uint32)tx_freq,
                                             0, 
                                            RFDEVICE_DPD_DISABLED,
                                            &script_data, 
                                            TRUE, 
                                            (rfm_device_enum_type)((uint16)device_fb-1));
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: tx device pointer is NULL!", 0 ); 
      return(FALSE);
    }
  }

  /* get the feedback path attenuation state from rfc   */
  set_path &= rfc_get_cmn_device_properties(&rfc_cmn_properties_ptr);

  if(rfc_cmn_properties_ptr != NULL)
  {
    /* Turn on feedback path */
    rfdevice_cmn_set_et_path(trx_cmn_device,
                             RFM_LTE_MODE, 
                             (rfm_fb_attenuation_state_type)rfc_cmn_properties_ptr->rfc_attenuation_state,
                             TRUE);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"setup_fb_device: Set et path to attenuation state %d", rfc_cmn_properties_ptr->rfc_attenuation_state);        
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: rfc_cmn_properties pointer is NULL", 0 ); 
    return(FALSE);
  }

  /* Get common device object for new device architecture */
  device_ptr = rfdevice_lte_tx_get_common_dev_ptr(device, band);

  if (NULL != device_ptr)
  {
    /* Set feedback path attenuation to gain state 2 and enable */
    set_path &= rfcommon_fbrx_mc_enable_rf_dev(device_ptr, 2);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: common device pointer is NULL", 0 ); 
    return(FALSE);
  }
  
  return (set_path);
}  


boolean rfc_lte_intf::disable_fb_device(rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_lte_band_type band, uint32 tx_freq)
{
  rfc_cfg_params_type cfg;
  rfc_intf *rfc_cmn = NULL;
  rfdevice_class *trx_device = NULL;
  rfdevice_rxtx_common_class* trx_cmn_device = NULL;  
  rfc_device_info_type *device_info_ptr = NULL;
  rfc_lte_data *rfc_lte_data_ptr = NULL;
  rfdevice_trx_lte_tx *tx_device = NULL;  
  rfdevice_lte_script_data_type script_data = {TRUE,NULL};  
  uint32 temp_instance;
  boolean set_path = TRUE;
  rfc_cmn_properties_type *rfc_cmn_properties_ptr = NULL;
  rfdevice_rxtx_common_class* device_ptr = NULL;
  
  cfg.alternate_path = 0;
  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;

  rfc_lte_data_ptr = rfc_lte_data::get_instance();

  if (rfc_lte_data_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: RFC LTE Data Object is NOT Created", 0);
    return(FALSE);
  }
  
  rfc_lte_data_ptr->devices_cfg_data_get(&cfg, &device_info_ptr);

    if (device_info_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: RFC LTE device_info_ptr for Tx is NULL", 0);
    return(FALSE);
  }
    
  temp_instance =device_info_ptr->rf_asic_info[0].instance;

  cfg.band = (int)band;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.logical_device = device_fb;
  cfg.req = RFC_REQ_ENABLE_XPT_CAPTURE;

  rfc_lte_data_ptr->devices_cfg_data_get(&cfg, &device_info_ptr);

    if (device_info_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: RFC LTE device_info_ptr for Rx is NULL", 0);
    return(FALSE);
  }
      
  rfc_cmn = rfc_intf::get_instance();  
  trx_device= rfc_cmn->get_cmn_rf_device_object(RFDEVICE_TRANSCEIVER, device_info_ptr->rf_asic_info[0].instance);
  if( trx_device != NULL )
  {
    /* extract the lte device object from the cmn_device object and store it */ 
    trx_cmn_device = (rfdevice_rxtx_common_class *)trx_device;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: trx_cmn_device not created", 0);
    return(FALSE);
  }
      
      
  if (temp_instance !=device_info_ptr->rf_asic_info[0].instance)
  {  
    tx_device = (rfdevice_trx_lte_tx*)(((rfdevice_rxtx_common_class *)trx_device)\
                                       ->get_tech_instance
                                       (
                                                                    RFM_LTE_MODE, 
                                         RFDEVICE_TX,
                                         RFDEVICE_TRX_PATH_0
                                        )
                                       );
  
    if (NULL != tx_device)
    {
      rfdevice_lte_tx_tune_to_default_port( tx_device,
                                            band, 
                                            (uint32)tx_freq,
                                             0, 
                                            RFDEVICE_DPD_DISABLED,
                                            &script_data, 
                                            TRUE, 
                                            (rfm_device_enum_type)((uint16)device_fb-1));
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: tx device pointer is NULL!", 0 ); 
      return(FALSE);
    }
  }

  /* Get common device object for new device architecture */
  device_ptr = rfdevice_lte_tx_get_common_dev_ptr(device, band);

  if (NULL != device_ptr)
  {
    /* Set feedback path attenuation to gain state 2 and enable */
    set_path &= rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"setup_fb_device: common device pointer is NULL", 0 ); 
    return(FALSE);
  }
  
  return (set_path);
}  

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
void rfc_lte_intf::get_rx_band(boolean *split_band, rfcom_lte_earfcn_type rx_chan, 
                               rfcom_lte_band_type *rfc_band)
{
  
  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;
  int i = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = (int)*rfc_band;
  cfg.rx_tx = RFC_CONFIG_RX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
  if ( (band_split_ret_val == TRUE) && (signal_list_ptr != NULL) )
  {
    *split_band = TRUE;
    /* Loop through channels to find true band */
      while ((unsigned int)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
      {
        if (((uint32)rx_chan >= signal_list_ptr[0].band_info[i].start_chan) && ((uint32)rx_chan <= signal_list_ptr[0].band_info[i].stop_chan))
        {
          *rfc_band = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
          break;
        }
        i++;
      }    
  } 
  else
  {
    /* Do Nothing, probably not a split band */
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
void rfc_lte_intf::get_tx_band(boolean *split_band, rfcom_lte_earfcn_type tx_chan, 
                               rfcom_lte_band_type *rfc_band,boolean filtered_band)
{
  
  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;
  int i = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = (int)*rfc_band;
  cfg.rx_tx = RFC_CONFIG_TX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
 if ( (band_split_ret_val == TRUE) && (signal_list_ptr != NULL) )
 {
   *split_band = TRUE;
   /* Loop through channels to find true band */
     while ((unsigned int)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
     {
       if (filtered_band)
       {
          if (signal_list_ptr[0].band_info[i].status == RFC_FILTERED_PATH_TRUE)
          {
             *rfc_band = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
             break;
          }
       }
       else if (((uint32)tx_chan >= signal_list_ptr[0].band_info[i].start_chan) && ((uint32)tx_chan <= signal_list_ptr[0].band_info[i].stop_chan))
       {
         *rfc_band = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
         break;
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
void rfc_lte_intf::get_tx_hi_lo_earfcn(rfcom_lte_band_type split_band, 
                                     rfcom_lte_earfcn_type *low_earfcn,
                                     rfcom_lte_earfcn_type *high_earfcn)
{

  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;
  int i = 0;
  /* These params help to detect the largest range for a given split band */
  boolean largest_range_detected = FALSE;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = split_band;
  cfg.rx_tx = RFC_CONFIG_TX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
 
  if ((TRUE == band_split_ret_val) && (NULL != signal_list_ptr))
  {
   /* Loop through channels to find true band and the largest channel range */
    while ((uint32)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
    {
      if (signal_list_ptr[0].band_info[i].band == split_band)
      {
          if (FALSE == largest_range_detected)
          {
              *low_earfcn = signal_list_ptr[0].band_info[i].start_chan;
              *high_earfcn = signal_list_ptr[0].band_info[i].stop_chan;
              largest_range_detected = TRUE;
          }
          else
          {
              if (*low_earfcn > signal_list_ptr[0].band_info[i].start_chan)
              {
                  *low_earfcn = signal_list_ptr[0].band_info[i].start_chan;
              }
              if (*high_earfcn < signal_list_ptr[0].band_info[i].stop_chan)
              {
                  *high_earfcn = signal_list_ptr[0].band_info[i].stop_chan;
              }
          }
      }
      i++;
    }
  }
}

rfm_device_enum_type rfc_lte_intf::get_mapped_device_for_scell 
(
 rfcom_lte_band_type pcell_band,
 rfcom_lte_band_type scell_band,
 rfm_device_enum_type scell_rfm_device
)
{
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  rfc_band_pair_list_type *band_pair_lists = NULL;

  rfc_data->ca_scell_log_path_swap_data_get(&band_pair_lists);
  uint8 band_pair_list_idx = 0;

  if (band_pair_lists != NULL)
  {
    while (band_pair_lists->band_pair_list[band_pair_list_idx].band1 != RFCOM_NUM_LTE_BANDS)
    {
      if (band_pair_lists->band_pair_list[band_pair_list_idx].band1 == pcell_band &&
          band_pair_lists->band_pair_list[band_pair_list_idx].band2 == scell_band)
      {
        MSG_3(MSG_SSID_RF, MSG_LEGACY_LOW,"rfc_lte_intf::get_mapped_device_for_scell() swapping "
              " devices pcell band: %d scell band: %d scell_rfm_device:%d", pcell_band, scell_band, scell_rfm_device);
        /* Dynamic SCELL Swap not supported for SCC2 right now. Need to be designed; if necessary. */
        if (scell_rfm_device == RFM_DEVICE_2) 
        {
          return (RFM_DEVICE_0);
        } 
        /* Dynamic SCELL Swap not supported for SCC2 right now. Need to be designed; if necessary. */
        else if (scell_rfm_device == RFM_DEVICE_3)
        {
          return (RFM_DEVICE_1);
        }
        else
        {
         MSG_1(MSG_SSID_RF, MSG_LEGACY_LOW,"rfc_lte_intf::get_mapped_device_for_scell() "
                                      " no swapping done scell device passed: %d", scell_rfm_device);
         return (scell_rfm_device);
        }
        break;
      }
      band_pair_list_idx++;
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_LOW,"rfc_lte_intf::get_mapped_device_for_scell() table"
                                      " not available pcell band: %d scell band: d", pcell_band, scell_band);
  }
  return (scell_rfm_device);
}

  /* These new APIs are for querying band based on additional params */
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band based on certain selection params.

   @details
   The function will return rx band part of the rfc_band pointer. 

   @parameter
   @param
   split_band: Flag specifying if the path was split or not. 
   @param 
   rx_chan: Rx channel
   @param 
   curr_bw: current system bw
   @param 
   path_index_override: Path override index to match the Tx path on the Band  
   @param 
   *rfc_band: updated rfc band.

*/
void rfc_lte_intf::get_rx_band_v2(boolean *split_band, 
                                  rfcom_device_enum_type device,
                                  rfcom_lte_earfcn_type rx_chan,
                                  rfc_lte_bw_type curr_bw,
                                  uint8 path_index_override,
                                  rfcom_lte_band_type *rfc_band)
{
  
  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;
  int i = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.logical_device = device;
  cfg.band = (int)*rfc_band;
  cfg.rx_tx = RFC_CONFIG_RX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);

  if ( (band_split_ret_val == TRUE) && (signal_list_ptr != NULL) )
  {
    *split_band = TRUE;

    /* We need to first validate the path index override param and retreive the band */
    if (path_index_override < RFC_PATH_MAX)
    {
      /* Loop through path index override to find true band */
      while ((unsigned int)signal_list_ptr[0].band_info[i].path_index != RFC_INVALID_PARAM)
      {
        if ((unsigned int)signal_list_ptr[0].band_info[i].path_index == path_index_override)
        {
          *rfc_band = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
          break;
        }
        i++;
      }
    }
    else
    {
      /* Loop through channels to find true band based on BW */
      while ((unsigned int)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
      {
        /* Match the System BW before checking the channels */
        if ((signal_list_ptr[0].band_info[i].sys_bw == curr_bw)|| 
            (signal_list_ptr[0].band_info[i].sys_bw == RFC_BW_ALL))
        {
          /* Once the BW is matched then scan for valid chan */
          if (((uint32)rx_chan >= signal_list_ptr[0].band_info[i].start_chan) && 
              ((uint32)rx_chan <= signal_list_ptr[0].band_info[i].stop_chan))
          {
            *rfc_band = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
            break;
          }
        }
        i++;
      }
    }
  } 
  else
  {
    /* Do Nothing, probably not a split band */
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Split Band, final band returned = %d ", *rfc_band);
}
/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the tx band based on certain selection params.

   @details
   The function will return tx band part of the rfc_band pointer. 

   @parameter
   @param
   split_band: Flag specifying if the path was split or not. 
   @param 
   rx_chan: tx channel
   @param 
   curr_bw: current system bw
   @param 
   path_index_override: Path override index to match the Tx path on the Band  
   @param 
   *rfc_band: updated rfc band.

*/

void rfc_lte_intf::get_tx_band_v2(boolean *split_band, 
                                  rfcom_lte_earfcn_type tx_chan,
                                  rfc_lte_bw_type curr_bw,
                                  uint8 path_index_override,
                                  rfcom_lte_band_type *rfc_band,
                                  boolean filtered_band)
{
  
  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;
  int i = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = (int)*rfc_band;
  cfg.rx_tx = RFC_CONFIG_TX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);

  if ( (band_split_ret_val == TRUE) && (signal_list_ptr != NULL) )
  {
    *split_band = TRUE;

    /* We need to first validate the path index override param and retreive the band */
    if (path_index_override < RFC_PATH_MAX)
    {
      /* Loop through path index override to find true band */
      while ((unsigned int)signal_list_ptr[0].band_info[i].path_index != RFC_INVALID_PARAM)
      {
        if ((unsigned int)signal_list_ptr[0].band_info[i].path_index == path_index_override)
        {
          *rfc_band = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
          break;
        }
        i++;
      }
    }
    else
    {
      /* Loop through channels to find true band based on BW */
      while ((unsigned int)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
      {
        /* filtered_band will only take precedence if Path override index is not valid */
        if (filtered_band)
        {
          if (signal_list_ptr[0].band_info[i].status == RFC_FILTERED_PATH_TRUE)
          {
             *rfc_band = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
             break;
          }
        }
        /* Match the System BW before checking the channels */
        else if ((signal_list_ptr[0].band_info[i].sys_bw == curr_bw)|| 
                 (signal_list_ptr[0].band_info[i].sys_bw == RFC_BW_ALL))
        {
          /* Once the BW is matched then scan for valid chan */
          if (((uint32)tx_chan >= signal_list_ptr[0].band_info[i].start_chan) && 
              ((uint32)tx_chan <= signal_list_ptr[0].band_info[i].stop_chan))
          {
            *rfc_band = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
            break;
          }
        }
        i++;
      }
    }
  } 
}

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band info. for the specified rx channel by querying RFC.
 
   @param
   split_band: band for which low and high channels are desired
   *low_earfcn: Pointer to low chan
   *high_earfcn: Pointer to high chan

   @details

*/
void rfc_lte_intf::get_rx_hi_lo_earfcn(rfcom_lte_band_type split_band, 
                                       rfcom_lte_earfcn_type *low_earfcn,
                                       rfcom_lte_earfcn_type *high_earfcn)
{

  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;

  /* These params help to detect the largest range for a given split band */
  boolean largest_range_detected = FALSE;

  int i = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = split_band;
  cfg.rx_tx = RFC_CONFIG_RX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
 
  if ((band_split_ret_val == TRUE) && (signal_list_ptr != NULL))
  {
   /* Loop through channels to find true band and the largest channel range */
    while ((uint32)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
    {
      if (signal_list_ptr[0].band_info[i].band == split_band)
      {
        if (largest_range_detected == FALSE)
        {
          *low_earfcn = signal_list_ptr[0].band_info[i].start_chan;
          *high_earfcn = signal_list_ptr[0].band_info[i].stop_chan;
          largest_range_detected = TRUE;
        }
        else
        {
          if (*low_earfcn > signal_list_ptr[0].band_info[i].start_chan)
          {
            *low_earfcn = signal_list_ptr[0].band_info[i].start_chan;
          }
          if (*high_earfcn < signal_list_ptr[0].band_info[i].stop_chan)
          {
              *high_earfcn = signal_list_ptr[0].band_info[i].stop_chan;
          }
        }
      }
      i++;
    }
  }
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
boolean rfc_lte_intf::get_updated_rf_port
(
  rfm_device_enum_type rfm_dev, 
  rf_card_band_type curr_band, 
  uint8 alt_path
)
{
  rfcom_lte_band_type  rf_band = RFCOM_BAND_LTE_INVALID; 
  rfc_cfg_params_type  cfg;
  rfc_device_info_type *device_info_ptr;
  boolean status = TRUE;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    RF_MSG_1(RF_ERROR,"RFC LTE Data Object is NOT Created", 0);
    status = FALSE;
  }

  if(status == TRUE)
  {    
  /* Convert from rf card band type to rf com band type */
  rf_band = rflte_util_convert_rfc_band_to_rfcom_band(curr_band); 

  /* Initialize the config params structure */
  cfg.logical_device = rfm_dev;
  cfg.band           = (uint8)rf_band; 
  cfg.rx_tx          = RFC_CONFIG_RX;
  cfg.req            = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state     = RFC_PORT_STATE_A;

  /* Enable when AG support is available */
  cfg.alternate_path = alt_path; 

  /* Get the column specific to a band, logical device & alt path */
    status = rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr); 
  } 

  if(status == TRUE)
  {
  /* Update the band port enumeration for all devices in that logical path */
  init_rf_devices(&cfg, device_info_ptr); 
  }

  return status; 
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  API to detect the delta devices between two alternate paths for the same
  logical device
 
  @details
  Function retrieves the devices list for a particular logical device and its
  two alternate paths. These device lists are compared and their delta devices
  are stored and return to the calling function. The information is used by
  lte tech main control to determine the reconfiguration requirements in the
  case of a Pcell/Scell swap
 
  @param rfm_dev
  current logical device for which a path update is requested
 
  @param curr_band
  current band in use
 
  @param prev_alt_path
  alternate path which is currently used by this logical device for
  the specific band
 
  @param next_alt_path
  alternate path which will be replacing the current alt path for this
  particular logical device and band combo
 
  @retval status
  return's TRUE if its able to successfully detect the delta devices between
  two alternate paths, else returns FALSE. 

*/
boolean rfc_lte_intf::get_delta_dev
(
  rfm_device_enum_type rfm_dev, 
  rf_card_band_type curr_band, 
  uint8 prev_alt_path, 
  uint8 next_alt_path 
)
{
  uint8 loop_idx  = 0; 
  uint8 delta_idx = 0; 
  rfcom_lte_band_type  rf_band = RFCOM_BAND_LTE_INVALID; 
  rfc_cfg_params_type  prev_cfg;
  rfc_cfg_params_type  next_cfg; 
  rfc_device_info_type *prev_dev_info_ptr;
  rfc_device_info_type *next_dev_info_ptr; 
  rfdevice_type_enum_type delta_dev[RFDEVICE_TYPE_MAX_NUM]; 
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    RF_MSG_1(RF_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return FALSE;
  }

  /* Convert from rf card band type to rf com band type */
  rf_band = rflte_util_convert_rfc_band_to_rfcom_band(curr_band); 

  /* Initialize the config params structure */
  prev_cfg.logical_device = rfm_dev;
  prev_cfg.band           = (uint8)rf_band; 
  prev_cfg.rx_tx          = RFC_CONFIG_RX;
  prev_cfg.req            = RFC_REQ_DEFAULT_GET_DATA;
  prev_cfg.port_state     = RFC_PORT_STATE_A;
  /* // Wait for alternate path support
  prev_cfg.alt_path       = prev_alt_path; */ 

  next_cfg.logical_device = rfm_dev;
  next_cfg.band           = (uint8)rf_band; 
  next_cfg.rx_tx          = RFC_CONFIG_RX;
  next_cfg.req            = RFC_REQ_DEFAULT_GET_DATA;
  next_cfg.port_state     = RFC_PORT_STATE_A;
  /* // Wait for alternate path support
  next_cfg.alt_path       = next_alt_path; */ 

  /* Obtain reference to the column in RFC ss matching the current band,
     logical device and alternate path */
  rfc_data->devices_cfg_data_get(&prev_cfg, &prev_dev_info_ptr); 
  rfc_data->devices_cfg_data_get(&next_cfg, &next_dev_info_ptr); 

  if (prev_dev_info_ptr == NULL || next_dev_info_ptr == NULL)
  {
    RF_MSG(RF_ERROR, "Unable to find a matching column in "
                     "RFC SS for specified alternate path"); 
    return FALSE; 
  }

  if (prev_dev_info_ptr->num_devices != next_dev_info_ptr->num_devices)
  {
    RF_MSG(RF_ERROR, "Check if the RFC columns are matching in terms of "
                     "number of devices to reconfigure"); 
    return FALSE; 
  }

  for (loop_idx = 0; loop_idx < prev_dev_info_ptr->num_devices; loop_idx++)
  {
    if ( prev_dev_info_ptr->rf_asic_info[loop_idx].device_type == 
         next_dev_info_ptr->rf_asic_info[loop_idx].device_type )
    {
      if ( prev_dev_info_ptr->rf_asic_info[loop_idx].data[1] != 
           next_dev_info_ptr->rf_asic_info[loop_idx].data[1] )
      {
        delta_dev[delta_idx++] = next_dev_info_ptr->rf_asic_info[loop_idx].device_type; 
      } 
    }
  }

  return TRUE; 
}


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
void rfc_lte_intf::get_split_rx_bands(boolean *split_band, rfcom_lte_earfcn_type rx_chan, 
                               rfcom_lte_band_type *rfc_band1, rfcom_lte_band_type *rfc_band2)
{
  
  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;
  /* split_count keeps track of number of split band matches for a given earfcn.
     This will be 2 in case a earfcn falls in the overlap region between two split bands*/
  int i = 0, split_count = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }
  /* Setup configuration parameters */
  cfg.alternate_path = 0;
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = (int)*rfc_band1;
  cfg.rx_tx = RFC_CONFIG_RX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
  if ( (band_split_ret_val == TRUE) && (signal_list_ptr != NULL) )
  {
    *split_band = TRUE;
    /* Loop through channels to find true band */
      while ((unsigned int)signal_list_ptr[0].band_info[i].band != RFC_INVALID_PARAM)
      {
        if (((uint32)rx_chan >= signal_list_ptr[0].band_info[i].start_chan) && ((uint32)rx_chan <= signal_list_ptr[0].band_info[i].stop_chan))
        {
          split_count++;
          /* split_count will be 1 for the 1st match, set both bands the same in case there is no
           * overlap
           */
          if (split_count == 1)
          {
            *rfc_band1 = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
            *rfc_band2 = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
          }
          /* split_count will be 2 for the 2nd match, in this case, there is an overlap*/
          else if (split_count == 2)
          {
            *rfc_band2 = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[i].band);
            break;
          }
        }
        i++;
      }
  } 
  else
  {
    /* Do Nothing, not a split band */
  }
}


/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function returns the rx band info. for the specified rx channel by querying RFC.
 
   @param
   Input parameter:
   split_band: band for which low and high channels are desired
   Output Parameters:
   *low_earfcn: will store list of low earfcns
   *high_earfcn: will store list of high earfcns
   *band_list: Will store list of split bands 

   @details
   Limitation: only works on RFC which is bandwidth independent. To support RFC list which lists earfcns
   by split band and bandwidth, this api may need modification

*/
void rfc_lte_intf::get_rx_split_band_channel_rasters(rfcom_lte_band_type split_band, 
                                                     rfcom_lte_earfcn_type *low_earfcn,
                                                     rfcom_lte_earfcn_type *high_earfcn,
                                                     rfcom_lte_band_type *band_list,
                                                     uint8 *num_of_splits)
{

  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;

  int split_count = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return;
  }
  // Setup configuration parameters
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band = split_band;
  cfg.rx_tx = RFC_CONFIG_RX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
 
  if ((band_split_ret_val == TRUE) && (signal_list_ptr != NULL))
  {
   /* Loop through RFC data to get earfcn range and list of split bands */
    while ((uint32)signal_list_ptr[0].band_info[split_count].band != RFC_INVALID_PARAM)
    {
      if (RFLTE_MC_MAX_RFC_SPLITS == split_count)
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RFC has more split band combos than can be handled by RFSW!! Please check RFC SS.");
        break;
      }
      if (NULL != low_earfcn)
      {
        *low_earfcn = (rfcom_lte_earfcn_type)signal_list_ptr[0].band_info[split_count].start_chan;
        low_earfcn++;
      }
      if (NULL != high_earfcn)
      {
        *high_earfcn = (rfcom_lte_earfcn_type)signal_list_ptr[0].band_info[split_count].stop_chan;
        high_earfcn++;
      }
      if (NULL != band_list)
      {
         *band_list = (rfcom_lte_band_type)(signal_list_ptr[0].band_info[split_count].band);
         band_list++;
      }
      split_count++;
    }
    if (NULL != num_of_splits)
    {
      *num_of_splits = (uint8)split_count;
    }
  }
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
boolean rfc_lte_intf::generate_asdiv_xsw_script
( 
  uint32 position,
  rf_buffer_intf *asdiv_script_ptr,
  void *cb_data
)
{
  boolean status = TRUE;
  rfc_cfg_params_type band_path_cfg;
  rfc_device_info_type* device_info_ptr = NULL;
  rflte_mc_asdiv_get_script_struct_type *xsw_script_data;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
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
    RF_MSG( RF_ERROR, "rfc_lte_intf::generate_asdiv_xsw_script: "
                      "rfc_data ptr is null !!");
    return FALSE;
  }

  /* Get the ASM script data */
  xsw_script_data = (rflte_mc_asdiv_get_script_struct_type*)cb_data;

  /* NULL pointer check */
  if ( xsw_script_data == NULL )
  {
    RF_MSG( RF_ERROR, "generate_asdiv_xsw_script: NULL callback data" );
    return FALSE;
  } /* if ( asm_script_data == NULL ) */

  /* Populate params */
  immediate = xsw_script_data->immediate;

  band_path_cfg.alternate_path = 0;
  band_path_cfg.logical_device = xsw_script_data->device; /* Not required*/
  band_path_cfg.band = xsw_script_data->band;
  band_path_cfg.rx_tx=RFC_CONFIG_RX;
  band_path_cfg.path = RF_PATH_0;
  band_path_cfg.port_state = RFC_PORT_STATE_A;

  xsw_config.band.lte_band = xsw_script_data->band;
  xsw_config.mode = RFM_LTE_MODE;

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

   /* Get device info pointer */
   rfc_data->devices_cfg_data_get(&band_path_cfg, &device_info_ptr );

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
                status = ((rfdevice_xsw *)device_obj)->set_cross_switch_config(&xsw_config,asdiv_script_ptr,(rf_device_execution_type)(immediate),0);
                status = ((rfdevice_xsw *)device_obj)->trigger(asdiv_script_ptr,(rf_device_execution_type)(immediate));
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
     MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfc_lte_intf::generate_asdiv_xsw_script "
                     "device_info_ptr is NULL ");
   }

  return status;

} /* xsw device script */
#endif /*#define FEATURE_RF_ASDIV */

void *rfc_lte_intf::get_asd_xsw_config(rfm_device_enum_type device, rfcom_lte_band_type band, int position)
{
  rfc_cfg_params_type       cfg;
  rfc_sig_cfg_type          *rf_asd_signals = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  rfdevice_xsw_config_type xsw_config;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return rf_asd_signals;
  }

  // Setup configuration parameters
  cfg.alternate_path = 0;
  cfg.logical_device = device;
  cfg.band = band;

  xsw_config.band.lte_band=band;
  xsw_config.mode= RFM_LTE_MODE;


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
  This function generates the AsDiv script for the requested device
  and band.

  @param position
  RF device for which to generate script
 
  @param xsw_script_data
  Call back data to pass in additional info for extendability

  @param srs_xsw_script_ptr
  Output - The asdiv script
 
  @return
  True on success and FALSE on failure
*/
boolean rfc_lte_intf::generate_srs_xsw_script
( 
  uint32 position,
  rf_buffer_intf *srs_xsw_script_ptr,
  rfm_device_enum_type device,
  rfcom_lte_band_type band,
  boolean immediate
)
{
  boolean status = TRUE;
  rfc_cfg_params_type band_path_cfg;
  rfc_device_info_type* device_info_ptr = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  rfc_intf *rfc_cmn = rfc_intf::get_instance();
  /*Device type used to extract tech specific object*/
  rfdevice_type_enum_type cmn_dev_type = RFDEVICE_TYPE_INVALID;
  /*Device instance used to extract tech specific object*/
  uint32 instance = 0;
  /* Pointer to device class to hold common object */
  rfdevice_class *device_obj = NULL;
  uint8 i= 0;
  rfdevice_xsw_config_type xsw_config;
  rfc_sig_cfg_type *rfc_grfc_tbl_ptr = NULL;
  int32 number_of_grfc = 0;

  if ( rfc_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfc_lte_intf::generate_srs_xsw_script: "
                      "rfc_data ptr is null !!");
    return FALSE;
  }

  /* Populate params */
  band_path_cfg.alternate_path = 0;
  band_path_cfg.path = RF_PATH_0;
  band_path_cfg.logical_device = device; /* Not required*/
  band_path_cfg.band = band;
  band_path_cfg.rx_tx=RFC_CONFIG_RX;
  band_path_cfg.port_state = RFC_PORT_STATE_A;

  xsw_config.band.lte_band = band;
  xsw_config.mode = RFM_LTE_MODE;

  if (position == 1) 
  {
    band_path_cfg.req = RFC_REQ_ENABLE_ANT_SWAP_1;
    xsw_config.xsmode = RFDEVICE_XSW_SRS_ONLY_PATH_SWAP;
  }
  else
  {
    band_path_cfg.req = RFC_REQ_DISABLE_ANT_SWAP_1;
    xsw_config.xsmode = RFDEVICE_XSW_SRS_ONLY_PATH_DEFAULT;
  }

   /* Get the list of devices */
   rfc_data->devices_cfg_data_get(&band_path_cfg, &device_info_ptr );

   /*Configure the list of devices*/
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
                status = ((rfdevice_xsw *)device_obj)->set_cross_switch_config(&xsw_config,srs_xsw_script_ptr,(rf_device_execution_type)(!immediate),0);
                status = ((rfdevice_xsw *)device_obj)->trigger(srs_xsw_script_ptr,(rf_device_execution_type)(!immediate));
              }
            else
              { 
                status = ((rfdevice_xsw *)device_obj)->set_cross_switch_config(&xsw_config,srs_xsw_script_ptr,(rf_device_execution_type)(immediate),0);
                status = ((rfdevice_xsw *)device_obj)->trigger(srs_xsw_script_ptr,(rf_device_execution_type)(immediate));
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
     MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_lte_data::generate_srs_xsw_script "
                     "device_info_ptr is NULL ");
   }

  /*Get the GRFC's Info */  

  rfc_grfc_tbl_ptr = (rfc_sig_cfg_type*)get_asd_xsw_config(device,band,position);

  if (rfc_grfc_tbl_ptr != NULL)
  {
     /*Get ANT_SEL grfc's in script buffer*/
    number_of_grfc = rfc_common_get_grfc_buffer(rfc_grfc_tbl_ptr, RFC_ANT_SEL, srs_xsw_script_ptr);
  }
  
  MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfc_lte_data::generate_srs_xsw_script"
                                     "Number of GRFC extracted for SRS switch = %d "
                                     "RFM_Device:%d, band:%d, Position:%d",
                                     device,band,position,number_of_grfc);

  return status;
}

boolean rfc_lte_intf::get_ns07_ports
(
  rfm_device_enum_type rfm_dev, 
  rfcom_lte_band_type rf_band  
)
{
  rfc_cfg_params_type  cfg;
  rfc_device_info_type *device_info_ptr;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();

  if (rfc_data == NULL)
  {
    RF_MSG_1(RF_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return FALSE;
  }
  
  /* Initialize the config params structure */
  cfg.logical_device = rfm_dev;
  cfg.band           = (uint8)rf_band; 
  cfg.rx_tx          = RFC_CONFIG_RX;
  cfg.req            = RFC_REQ_DEFAULT_GET_DATA;
  cfg.port_state     = RFC_PORT_STATE_NS07;

  /* Enable when AG support is available */
  // cfg.alternate_path = alt_path; 

  /* Get the column specific to a band, logical device & alt path */
  rfc_data->devices_cfg_data_get(&cfg, &device_info_ptr);   

  /* Update the band port enumeration for all devices in that logical path */
  init_rf_devices(&cfg, device_info_ptr); 

  return TRUE; 
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gives the split band mask for the requested full band.

  @param band
  Full Band for which the split band mask is to be generated.
 
  @return
  uint64 with bit positions corresponding to split bands set to 1. 
  If no split bands, then 0 is returned.
*/
uint64 rfc_lte_intf::get_split_bands(rfcom_lte_band_type band)
{
  rfc_cfg_params_type	cfg;
  rfc_band_split_info_type	*signal_list_ptr = NULL;
  rfc_lte_data *rfc_data     = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;
  uint64 split_band          = 0;	
  int counter                = 0;

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
   	return 0;
  }

  // Setup configuration parameters
  cfg.logical_device = RFM_DEVICE_0;
  cfg.band           = band;
  cfg.rx_tx          = RFC_CONFIG_RX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr);
  
  if ( ( TRUE == band_split_ret_val ) && ( NULL != signal_list_ptr ) )
  {
    /* Loop through channels to find split bands */
   	while ((uint32)signal_list_ptr[0].band_info[counter].band != RFC_INVALID_PARAM)
   	{
	  /*  Check to ignore the invald split bands in RFC.
              The invalid bands are added in RFC in case the main band is empty. */
	  if( ( 99999 != signal_list_ptr[0].band_info[counter].start_chan ) || 
               ( 99999 != signal_list_ptr[0].band_info[counter].stop_chan ) )
          {
	     split_band = split_band | ( ( ( uint64 ) 0x1 ) << signal_list_ptr[0].band_info[counter].band );
	  }
   	  counter++;
	 }
  }

  return split_band;
  
}/* get_split_bands(rfcom_lte_band_type band) */

/* ----------------------------------------------------------------------- */
/*!
   @brief
   This function checks whether intra-band is possible and returns the band in case intra ca is
   possible
 
   @param pcc_info
   Pointer to PCell carrier information
 
   @param scc_info
   Pointer to SCell carrier information
 
   @param intra_ca_band   
   Pointer to be populated with the intra_ca_split_band if intra is true

   @details
   This function checks whether intra-band is possible and returns the band in case intra ca is
   possible.
   The possible scenarios are:
   1. inter-band CA: should return FALSE
   2. intra CA (no split): return TRUE, populate intra_ca_band with pcc\scc band
   3. intra CA (scc in same split as pcc): return TRUE, populate intra_ca_band with pcc\scc band
   4. intra CA (scc in different split, pcc retune needed): return TRUE, populate intra_ca_band with new split
   5. split CA: (scc and pcc cannot fit in a split): return FALSE: This should be disallowed at higher layers
   6. split CA: (pcc and scc belong to different split bands eg: B40 and B41): return FALSE
*/

boolean rfc_lte_intf::check_intra_ca_feasible(rflte_mc_carrier_info_rfc_type *pcc_info, 
                                              rflte_mc_carrier_info_rfc_type *scc_info,
                                              rfcom_lte_band_type *intra_ca_band)
{

  rfc_cfg_params_type       cfg;
  rfc_band_split_info_type          *signal_list_ptr_pcc = NULL;
  rfc_band_split_info_type          *signal_list_ptr_scc = NULL;
  rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  boolean band_split_ret_val = FALSE;
  boolean debug_flag = FALSE;
  uint8 i = 0;
  uint8 j = 0;
  boolean intra_feasible = FALSE;

  int split_count = 0;

  /* Validate input parameters */
  if (NULL == pcc_info)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"check_intra_ca_feasible: pcc_info is NULL", 0);
    return FALSE;
  }

  if (NULL == scc_info)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"check_intra_ca_feasible: scc_info is NULL", 0);
    return FALSE;
  }

  if (NULL == intra_ca_band)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"check_intra_ca_feasible: intra_ca_band is NULL", 0);
    return FALSE;
  }
  /* End of Validate input parameters */

  if (rfc_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"check_intra_ca_feasible: RFC LTE Data Object is NOT Created", 0);
    return FALSE;
  }

  if ((TRUE == debug_flag))
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "Input params: PCC band %d, bw %d, Chan %d, device %d ", pcc_info->rx_band, pcc_info->rflte_sys_bw, pcc_info->rflte_chan_rx, pcc_info->device);
    MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "Input params: SCC band %d, bw %d, Chan %d, device %d ", scc_info->rx_band, scc_info->rflte_sys_bw, scc_info->rflte_chan_rx, scc_info->device);
  }

  /* Get PCC RFC info */
  cfg.logical_device = pcc_info->device;
  cfg.band = pcc_info->rx_band;
  cfg.rx_tx = RFC_CONFIG_RX;

  band_split_ret_val = rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr_pcc);

  /* Debug only */
  if (signal_list_ptr_pcc != NULL)
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "check_intra_ca_feasible: Signal ptr 1st row: PCC band %d, bw %d, start Chan %d, stop_chan %d ", 
          signal_list_ptr_pcc[0].band_info[0].band, 
          signal_list_ptr_pcc[0].band_info[0].sys_bw, 
          signal_list_ptr_pcc[0].band_info[0].start_chan, 
          signal_list_ptr_pcc[0].band_info[0].stop_chan);
  }

  /* Get SCC RFC info */

  cfg.logical_device = scc_info->device;
  cfg.band = scc_info->rx_band;
  cfg.rx_tx = RFC_CONFIG_RX;

  band_split_ret_val &= rfc_data->band_split_cfg_data_get(&cfg, &signal_list_ptr_scc);
  
  if (signal_list_ptr_scc != NULL)
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "check_intra_ca_feasible: Signal ptr 1st row: SCC band %d, bw %d, start Chan %d, stop_chan %d ", 
          signal_list_ptr_scc[0].band_info[0].band, 
          signal_list_ptr_scc[0].band_info[0].sys_bw, 
          signal_list_ptr_scc[0].band_info[0].start_chan, 
          signal_list_ptr_scc[0].band_info[0].stop_chan);
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "check_intra_ca_feasible: band_split_ret_val %d ", band_split_ret_val);

  if(band_split_ret_val == TRUE)
  {
    if ((signal_list_ptr_pcc != NULL) && (signal_list_ptr_scc != NULL))
    {
      /*if pcc bw != ALL_BW, loop on rfc_pcc_bw till row found for the 1st split */
      while (intra_feasible == FALSE)
      {
        /* Loop over PCC rfc data to find 1st split row which matches its bw */
        /* Match the System BW before checking the channels */
          while((signal_list_ptr_pcc[0].band_info[i].sys_bw != pcc_info->rflte_sys_bw) &&
                (signal_list_ptr_pcc[0].band_info[i].sys_bw != RFC_BW_ALL) )
          {
            /* Check to ensure there is no infinite loop */
            if (signal_list_ptr_pcc[0].band_info[i].sys_bw == RFC_INVALID_PARAM)
            {
              MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "check_intra_ca_feasible: Sys bw passed doesn't match the RFC rflte_sys_bw %d ", 
                    pcc_info->rflte_sys_bw);
              break;
            }
            i++;
          }

          /* Break out of Top level While loop if reached end of RFC struct */
          if (signal_list_ptr_pcc[0].band_info[i].sys_bw == RFC_INVALID_PARAM)
          {
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "check_intra_ca_feasible: Reached end of PCC RFC info without a match i %d ", i);
            break;
          }

          /* Loop over SCC rfc data to find 1st split row which matches its bw */

          while( (signal_list_ptr_scc[0].band_info[j].sys_bw != scc_info->rflte_sys_bw) &&
                 (signal_list_ptr_pcc[0].band_info[i].sys_bw != RFC_BW_ALL) )
          {
            /* Check to ensure there is no infinite loop */
            if (signal_list_ptr_scc[0].band_info[j].sys_bw == RFC_INVALID_PARAM)
            {
              MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "check_intra_ca_feasible: Sys bw passed doesn't match the RFC rflte_sys_bw %d ", 
                    scc_info->rflte_sys_bw);
              break;
            }
            j++;
          }

          /* Break out of Top level While loop if reached end of RFC struct */
          if (signal_list_ptr_scc[0].band_info[j].sys_bw == RFC_INVALID_PARAM)
          {
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "check_intra_ca_feasible: Reached end of SCC RFC info without a match j %d ", j);
            break;
          }

          if ((TRUE == debug_flag))
          {
            MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"check_intra_ca_feasible:loop variables i: %d, j: %d", i, j);
          }

          /* If pcc rx_chan fits within this split and scc rx_chan fits within this split, intra is possible*/
          if( (((uint32)pcc_info->rflte_chan_rx >= signal_list_ptr_pcc[0].band_info[i].start_chan) && 
                ((uint32)pcc_info->rflte_chan_rx <= signal_list_ptr_pcc[0].band_info[i].stop_chan))
                                                 &&
            (((uint32)scc_info->rflte_chan_rx >= signal_list_ptr_scc[0].band_info[j].start_chan) && 
                ((uint32)scc_info->rflte_chan_rx <= signal_list_ptr_scc[0].band_info[j].stop_chan)) )
          {
            intra_feasible = TRUE;
            /* Additional check to make sure both PCC and SCC matched the same split */
            if (signal_list_ptr_pcc[0].band_info[i].band == signal_list_ptr_scc[0].band_info[j].band)
            {
              *intra_ca_band = (rfcom_lte_band_type) (signal_list_ptr_pcc[0].band_info[i].band);
              MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "check_intra_ca_feasible: PCC and PCC split bands match: pcc_band %d, scc_band %d, intra_ca_band %d", 
                    signal_list_ptr_pcc[0].band_info[i].band,
                    signal_list_ptr_scc[0].band_info[j].band,
                    *intra_ca_band);
            }
            else
            {
              /* Break out of top level while: error condition */
              intra_feasible = FALSE;
              MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "check_intra_ca_feasible: PCC and SCC split bands didn't match: pcc_band %d, scc_band %d ", 
                    signal_list_ptr_pcc[0].band_info[i].band,
                    signal_list_ptr_scc[0].band_info[j].band);
              break;
            }
          }
          else
          {
            i++;
            j++;
            continue;
          }
      } /* while (intra_feasible == FALSE) */
    } /* NULL Check */
    else
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "check_intra_ca_feasible: Error retrieving data from rfc: NULL signal ptrs returned!!");
    }
  }
  else /* Non-split bands */
  {
    if (pcc_info->rx_band == scc_info->rx_band)
    {
      intra_feasible = TRUE;
      *intra_ca_band = pcc_info->rx_band;
    }
  }
  return intra_feasible;
}
/*!
  @brief
  This function get partial band range information from RFC.

  @param ptr
  point to LTE parital band range information in RFC
 
  @return
   TRUE if get partial band infomation successfully, else false 
*/

boolean rfc_lte_intf::rfc_get_lte_properties (rfc_lte_properties_type **ptr)
{
   boolean status = FALSE;
   rfc_lte_data *rfc_data = rfc_lte_data::get_instance();
  
  if (NULL == rfc_data)
  {
    RF_MSG( RF_ERROR, " RFC Common data object is NOT created : rfc_common_data == NULL!" );
    return status;
  }

  status = rfc_data->get_lte_properties(ptr);
  return status;
}
