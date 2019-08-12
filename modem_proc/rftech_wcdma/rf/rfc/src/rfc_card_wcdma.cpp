/*!
   @file
   rfc_card_wcdma.cpp

   @brief
   This file contains all the WCDMA mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2008-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/rfc/src/rfc_card_wcdma.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
09/03/14   rsr     Changes to return TX LUT index when querying TX LUT info.
04/17/14   ac      asd bringup with rfc changes and device script
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech 
07/17/13   aro     Replaced with antenna position enum
07/26/13   vs      Add RF interface to query DBDC bands supported
07/15/13   aro/kr  Added initial support for WCDMA ASDiv
07/09/13   kcj     Check that device init succeeded during rfwcdma_mc_init.
06/02/13   JJ     Change message level to high and remove redundant code
06/02/13   JJ     Get chip ID from RFC in function rfc_wcdma_manage_vregs
03/12/13   sn     XPT dual WTR fix for feedback path
11/26/12   tks    Changed get_device api to return a list of objects
11/02/12   dw     Offtarget warning fix
10/11/12   dw     SVVZ bringup updates
09/18/12   ac     rfc to take device as a parameter
08/12/12   vbh    Added rfc_wcdma_get_device()
07/29/12   tks    Fixed compile errors  
07/17/12   shb    Added rfc_wcdma_init()
05/07/12   dw     Move rfc common wakeup/sleep out of manage vreg
02/28/12   sr      made changes to put all the GPIOs into low power mode during sleep.
11/05/11   aak    Enable NPA voting from RF SW for 9x15 
11/03/11   ac     wakeup,tx_disable
10/31/11   ac     rfc sleep
10/05/11   ac/sr  rfc sleep
09/26/11   tks    Fixed KW Warning. 
09/06/11   dw     APT cleanup
08/25/11   aak    Enable RF NPA voting only for MSM8960 
08/23/11   ac     rfc cleanup
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
04/22/11   aak    Changing rfc_path_state to rfm_path_state
04/11/11   dw     Change to support new power management for Nikel
04/06/11   ad     Port APT changes from MDM9k
03/23/11   dw     Use get_instance() for WCDMA
01/27/11   dw     WCDMA RFC cleanup
01/10/11   dw     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfc_card.h"
#include "rfc_card_wcdma.h"
#include "rfcommon_core_error.h"
#include "rfcommon_core.h"
#include "rfwcdma_core.h"
#include "rfm_path_types.h"
#include "rfcommon_msg.h"
}

#include "rfc_wcdma.h"
#include "rfc_wcdma_data.h"

rfc_power_management_info rfwcdma_pm_info;
/* ----------------------------------------------------------------------- */
/*!
  @brief
  Sets the specified path to the technology mode of operation.

  @details
  This funtion will call the card specific version of the set mode API.

  @param
  path RF signal path

*/
void rfc_wcdma_mode_config( rfm_device_enum_type device )
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->config_mode(device);

  return;
} /* rfc_wcdma_mode_config() */

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function configures GPIO/GRFC settings along
  with any other parameter data structures for a particular band for Rx chain.

  @details

  @param
  path request Rx path
  band RFC band

*/
void rfc_wcdma_rx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band
)
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->rx_band_config(device, band);

}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function returns top level ptr to the current Rx band configuration.

  @details
  The function will return Rx configuration such as GPIO/GRFC settings along
  with any other parameter data structures for a particular band.  The void*
  allows for casting for usage under various band types.

  Dependency that rfc_set_band() be called before using this function.

  @param
  path request Rx path
  band RFC band
  config_data type of data to obtain

  @return
  void * ptr of the current Rx band configuration ptr.

*/
void *rfc_wcdma_get_rx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rfc_band_config_data_enum_type config_data
)
{
  void *data_config_ptr = NULL;
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return (data_config_ptr);
  }

  data_config_ptr = rfc_obj->rx_band_config_get(device, band, config_data);

  return (data_config_ptr);
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function gets mode configuration data 

  @details
  
  @param
  

 @return
  
*/
void *rfc_wcdma_get_mode_config_data
(
  rfm_device_enum_type device,
  rfc_mode_config_data_enum_type config_data
)
{
  void *data_config_ptr = NULL;
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return (data_config_ptr);
  }

  data_config_ptr = rfc_obj->mode_config_get(device, config_data);

  return (data_config_ptr);
}
/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function configures GPIO/GRFC settings along
  with any other parameter data structures for a particular band for Tx chain.

  @details

  @param
  

*/
void rfc_wcdma_tx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band
)
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->tx_band_config(device, band);
}
/* ----------------------------------------------------------------------- */
/*!
  @brief
  

  @details
  
  @param 
  
  @return
  
*/
void *rfc_wcdma_get_tx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rfc_band_config_data_enum_type config_data
)
{
  void *data_config_ptr = NULL;
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return (data_config_ptr);
  }

  data_config_ptr = rfc_obj->tx_band_config_get(device, band, config_data);

  return (data_config_ptr);
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  

  @details
  

  @param 
  
  @return
  

*/
int32 rfc_wcdma_command_dispatch
( 
  rfm_device_enum_type device, 
  rfcs_wcdma_cmd_type cmd, 
  void *data
) 
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return 0;
  }

  rfc_obj->command_dispatch(device, cmd, data);

  return 1;

}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  

  @details
  

  @param
  
  @return
  

*/
int32 rfc_wcdma_sleep
( 
  rfm_device_enum_type device
) 
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return 0;
  }

  rfc_obj->sleep(device);

  return 1;

}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Get Tx band configuration from RFC.

  @details
  
  @param band 

  
  @return
  
*/
int32 rfc_wcdma_wakeup(rfm_device_enum_type device)
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"wcdma RFC Obj is NOT created. ",0);
    return 0;
  }
  rfc_obj->wakeup(device);

  return 1;
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Get Tx band configuration from RFC.

  @details
  
  @param 
  
  @return
  

*/
int32 rfc_wcdma_tx_disable(rfm_device_enum_type device,rfcom_wcdma_band_type band)
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"wcdma RFC Obj is NOT created. ",0);
    return 0;
  }
  rfc_obj->tx_disable(device,band);

  return 1;
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  

  @details
  
  @param 
  @return
  

*/
void rfc_wcdma_manage_vregs
( 
  rfm_device_enum_type device, 
  rfcom_wcdma_band_type band, 
  rfm_path_state state
)
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();
  rf_path_enum_type path = rfcommon_core_device_to_path(device);
  rfc_cfg_params_type cfg;
  rfc_device_info_type *device_info_ptr;
  rfc_rf_chip_number wtr_chip_num = RF_CHIP_0;


  /* RFC OBJ null pointer check */
  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
	return;
  }

  cfg.alternate_path = 0;
  cfg.band = band;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_RX;
  /* Get RFC information */
  if (rfc_wcdma_data_get_device_info(&cfg, &device_info_ptr) == TRUE)
  {
     wtr_chip_num = (rfc_rf_chip_number)(device_info_ptr->rf_asic_info[0].instance);
     MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"rfc_wcdma_manage_vregs:RFC WCDMA chip_num %d",wtr_chip_num);
  }
  else
  {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfc_wcdma_manage_vregs:RFC WCDMA chip_num to default: %d",wtr_chip_num);
  }

   /*rflte_pm_info structure is filled out here so all cards version can reference */
  rfwcdma_pm_info.mode = RFM_IMT_MODE;
  rfwcdma_pm_info.band = band;
  rfwcdma_pm_info.path = path;
  rfwcdma_pm_info.state = state;
  rfwcdma_pm_info.rf_chip_num = wtr_chip_num;  

  rfc_obj->manage_vregs(&rfwcdma_pm_info);

  return;
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Create a RFC device 

  @details
  The function will return a pointer to the created RFC device  
  
  @param rfm_device

  @param rx_tx 

  @param band

  @param dev_type
  
  @return
  void * ptr of the created RF device oject 

*/
void** rfc_wcdma_get_device
(
  rfm_device_enum_type rfm_device, 
  rfc_rxtx_enum_type rx_tx,
  rfcom_wcdma_band_type band, 
  rfdevice_type_enum_type dev_type
)
{
   rfc_wcdma *rfc_wcdma_p = rfc_wcdma::get_instance();

  if(rfc_wcdma_p == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, 
        "rfc_wcdma_get_device:RFC WCDMA object not initialized");
    return NULL;
  }

  return (rfc_wcdma_p->get_rf_device_obj(rfm_device, rx_tx, band, dev_type ));
}

rfdevice_rxtx_common_class * rfc_wcdma_get_common_rxtx_device
(
  rfm_device_enum_type device, 
  rfcom_wcdma_band_type band, 
  uint32 alt_path
)
{
  rfc_device_info_type *device_info_ptr = NULL;
  /*Device type used to extract tech specific object*/
  rfdevice_type_enum_type dev_type = RFDEVICE_TYPE_INVALID;
  /*Device instance used to extract tech specific object*/
  uint32 instance = 0;
  rfc_cfg_params_type cfg;
  

  cfg.alternate_path = alt_path;
  cfg.band = band;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.logical_device = device;
  cfg.rx_tx = RFC_CONFIG_TX;
  cfg.path = rfcommon_core_device_to_path(device);

  /* Get RFC information */
  if (rfc_wcdma_data_get_device_info(&cfg, &device_info_ptr) == TRUE)
  {
     instance = device_info_ptr->rf_asic_info[0].instance;
     /*Retrieve device object*/
     return rfc_common_get_cmn_device_object((uint8)instance);
  }
  else
  {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," Could not get device info for LTE band %d",band);
     return NULL;
  }
}

void rfc_wcdma_get_grfc_info_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rfc_sig_cfg_type* rfc_signal,
  rfc_signal_type rfc_signal_type
)
{
  uint8 i = 0;
  uint8 j = 0;

  rfc_sig_cfg_type *rfc_tx_grfc_tbl_ptr = NULL;

  rfc_tx_grfc_tbl_ptr = (rfc_sig_cfg_type *)rfc_wcdma_get_tx_band_config(device,band,RFC_GRFC_DATA);

  if (rfc_tx_grfc_tbl_ptr == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, 
        "rfc_wcdma_get_grfc_info_band_config:GRFC table ptr is NULL");
    return;
  }
  /* loop through the signal list and configure them based on their type to specified logic  */
  while ((rfc_tx_grfc_tbl_ptr[i].sig_name != RFC_SIG_LIST_END)&&(rfc_signal[j].sig_name != RFC_SIG_LIST_END))
  {
    if(i >= RFC_MAX_NUM_OF_GRFC)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "GRFC script exceed MAX count. count: %d, MAX: %d",i,RFC_MAX_NUM_OF_GRFC);
      break;
    }
    if (rfc_common_get_signal_type(rfc_tx_grfc_tbl_ptr[i].sig_name)==rfc_signal_type)
    {
      rfc_signal[j].sig_name = rfc_tx_grfc_tbl_ptr[i].sig_name;
      rfc_signal[j].start.logic = rfc_tx_grfc_tbl_ptr[i].start.logic;
      j++;
    }
    i++;
  }
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  calls RFC function to setup feedback path
  
  @details
  
  @param 
  
  @return
  
*/

boolean rfc_wcdma_setup_fb_path (rfm_device_enum_type device_fb, rfcom_wcdma_band_type band, boolean et_enable)
{
  boolean api_status = TRUE ;
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();
	
  api_status = rfc_obj->setup_fb_path(device_fb, band, et_enable);
	
  return(api_status);
	
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  calls RFC function to configure feedback device
  
  @details
  
  @param 
  
  @return
  
*/

boolean rfc_wcdma_setup_fb_device (rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_wcdma_band_type band, uint16 tx_chan)
{
  boolean api_status = TRUE ;
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();
	
  api_status = rfc_obj->setup_fb_device(device, device_fb, band, tx_chan);
	
  return(api_status);
	
}
/*!
  @brief
  This function generates the AsDiv GRFCs script for the requested device
  and band.

  @param position
  RF device for which to generate GRFC script
 
  @param cb_data
  Call back data to pass in additional info for extendability

  @param asdiv_script_ptr
  Output - The GRFC script
 
  @return
  True on success and FALSE on failure
*/

#ifdef FEATURE_RF_ASDIV  
boolean
rfc_wcdma_get_asdiv_xsw_script
( 
  uint32 position,
  rf_buffer_intf *asdiv_script_ptr,
  void* cb_data
)
{
  boolean status;
  rfc_wcdma *rfc_wcdma_obj = rfc_wcdma::get_instance();


  if ( asdiv_script_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "generate_asdiv_xsw_script : scrpit ptr is "
                      "null"); 
    return FALSE;
  }

  if ( rfc_wcdma_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"wcdma RFC Obj is NOT created. ",0);
    return FALSE ;
  }

  status = rfc_wcdma_obj->generate_asdiv_xsw_script((int)position ,asdiv_script_ptr,
                                                             cb_data
                                                             );


  if( status == FALSE )
  {
    RF_MSG( RF_ERROR, "rfc_wcdma_get_asdiv_xsw_script : "
                      "script generation failed");
  }

  return status;
} /* rfc_wcdma_get_asdiv_xsw_script */
#endif

void* 
rfc_wcdma_get_asd_xsw_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint32 position
)
{
  rfc_sig_cfg_type *rfc_asd_sig_info = NULL;
  rfc_wcdma *rfc_wcdma_obj = rfc_wcdma::get_instance();

  if ( rfc_wcdma_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"wcdma RFC Obj is NOT created. ",0);
    return rfc_asd_sig_info ;
  }

  rfc_asd_sig_info = 
    (rfc_sig_cfg_type *) rfc_wcdma_obj->get_asd_xsw_config( device,
                                                             band,
                                                             (int)position );


  return rfc_asd_sig_info;

}


/* ----------------------------------------------------------------------- */
/*!
  @brief
  Returns whether device initialization has been completed.
  
  @details
 
  @return TRUE if device init is complete, FALSE otherwise
  
*/

boolean rfc_wcdma_get_device_init_complete(void)
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return FALSE;
  }

  return (rfc_obj->get_device_init_complete());
}
/* ----------------------------------------------------------------------- */
/*!
  @brief
  Calls RFC function to get the DBDC band combinations supported
  
  @details
  This would query the RF card and get the DBDC band combinations supported by the card.
  
  @param None
  
  @return
   Pointer to the structure containing the DBDC band combinations
  
*/

void *rfc_wcdma_get_dbdc_band_support (void)
{
  void *data_config_ptr = NULL;
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return FALSE;
  }
	
  data_config_ptr = rfc_obj->get_dbdc_band_support();

  return (data_config_ptr);
	
}


/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function updates the rx band config for the device passed

  @details

  @param
  device 
  band RFC band

*/
void rfc_wcdma_update_rx_band_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band
)
{
  rfc_wcdma *rfc_obj = rfc_wcdma::get_instance();

  if ( rfc_obj == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"WCDMA RFC Obj is NOT created. ",0);
    return;
  }

  rfc_obj->update_rx_band_config(device, band);

}