/*!
   @file
   rfc_wcdma.h

   @brief
   This file contains all the WCDMA mode specific RFC class definition

*/

/*===========================================================================

Copyright (c) 2011-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfc_wcdma.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
09/04/15   ak      Changes to put the GRFCs to init state in DRX sleep
06/16/14   tks     Added support for get updated port config 
04/17/14   ac      asd bringup with rfc changes and device script
03/20/14   ac      asd changes from dime
07/26/13   vs      Add RF interface to query DBDC bands supported
07/15/13   aro/kr  Added initial support for WCDMA ASDiv
07/09/13   kcj     Check that device init succeeded during rfwcdma_mc_init.
04/22/13   sma     Reduce RF memory consumption for WCDMA 
03/12/13   sn      XPT dual WTR fix for feedback path
11/26/12   tks     Modified the get_rf_device_obj function to return double pointer  
10/30/12   sr      changes create tech rfc object if the tech is supported by 
                   the specific rf-card.
09/18/12   ac      rfc to update parameter to device
08/08/12   ac      fix for the pure virtual function
07/27/12   sr      renamed rfdevice related enum types.
07/28/12   tks     Fixed compile errors  
07/19/12   shb     Added init() function in rfc_wcdma base class
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type.
05/18/12   sr      RFC-EFS code clean-up changes.
04/24/12   ac      Added support for rfc binary data
04/04/12   swb     Add multi-band support for HSPA APT 
03/01/01   dw      Add support for HSPA APT
11/03/11   ac      tx_disable
11/03/11   av      Add path parameter for the rx_band and tx_band variables.
10/31/11   ac      rfc wakeup
10/05/11   ac/sr   rfc sleep
09/12/11   sr      RFC cleanup.
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
04/22/11   aak    Changing rfc_path_state to rfm_path_state
04/11/11   dw     Change to support new power management for Nikel
03/23/11   dw      RFC class update
03/14/11   dw      Wrap _cplusplus around the class definition
01/27/11   dw      WCDMA RFC cleanup
01/07/11   dw      Initial Version 
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "rfa.h"
#include "rfc_card.h"
#include "rfc_card_wcdma.h"
#include "rfc_class.h"
#include "rfcommon_msg.h"
#include "rfcommon_asdiv_manager.h"
#include "rfwcdma_core_asdiv.h"
class rfc_wcdma : public rfa
{
public:
  static rfc_wcdma * get_instance(void);

  static rfc_wcdma * create_instance(void);

  virtual void config_mode(rfm_device_enum_type device);

  virtual void *mode_config_get(rfm_device_enum_type device, rfc_mode_config_data_enum_type config_data);

  virtual void rx_band_config(rfm_device_enum_type device, rfcom_wcdma_band_type band);

  virtual void update_rx_band_config(rfm_device_enum_type device, rfcom_wcdma_band_type band);

  virtual void *rx_band_config_get(rfm_device_enum_type device, rfcom_wcdma_band_type band,
                                   rfc_band_config_data_enum_type config_data);

  virtual void tx_band_config(rfm_device_enum_type device, rfcom_wcdma_band_type band);

  virtual void *tx_band_config_get(rfm_device_enum_type device, rfcom_wcdma_band_type band,
                                   rfc_band_config_data_enum_type config_data);

  virtual int32 command_dispatch( rfm_device_enum_type device, rfcs_wcdma_cmd_type cmd, void *data );

  virtual int32 sleep(rfm_device_enum_type device);
  virtual int32 wakeup(rfm_device_enum_type device);

  virtual int32 tx_disable(rfm_device_enum_type device,rfcom_wcdma_band_type band);

  void **get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                          rfcom_wcdma_band_type band, rfdevice_type_enum_type dev_type);

  virtual void *get_asd_xsw_config(rfm_device_enum_type device, rfcom_wcdma_band_type band,int position);

  #ifdef FEATURE_RF_ASDIV  
  virtual boolean generate_asdiv_xsw_script
  ( 
    uint32 position,
    rf_buffer_intf *asdiv_script_ptr,
    void *cb_data
  );
  #endif
											 
  virtual boolean setup_fb_path(rfm_device_enum_type device_fb, rfcom_wcdma_band_type band, boolean et_enable);												 
  
  virtual boolean setup_fb_device(rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_wcdma_band_type band, uint16 tx_chan);
  
  virtual void manage_vregs(rfc_power_management_info* pm_info);

  virtual void *get_dbdc_band_support(void);

  virtual ~rfc_wcdma();

  boolean get_device_init_complete(void);

  boolean get_updated_rf_port(rfm_device_enum_type rfm_dev, rf_card_band_type curr_band, uint8 alt_path);

protected:
  // Protected constructor for singleton implemenation
  rfc_wcdma(void);

  static rfc_wcdma *rfc_wcdma_ptr;

  static rfc_wcdma_core_config_type  rfc_wcdma_core_config_table;

  static rfc_wcdma_hspa_apt_config_type rfc_wcdma_hspa_config_data[RFCOM_NUM_WCDMA_BANDS];

  rfcom_wcdma_band_type rx_band[RFM_DEVICE_4];
  rfcom_wcdma_band_type tx_band[RFM_DEVICE_4];

private:
  boolean init_rf_devices(rfc_cfg_params_type *cfg, rfc_device_info_type *device_info_ptr);
  boolean init_rfdevices_for_all_bands(void);

  void *wcdma_rf_devices[RFM_DEVICE_4][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_WCDMA_BANDS][RFDEVICE_TYPE_MAX_NUM][RFC_MAX_DEVICE_INSTANCES_PER_BAND];

  /* Flag indicating if device initialization was successful */
  boolean device_init_complete;
  

#ifdef FEATURE_RF_ASDIV
  rfc_antenna_parameter_type ant_params[RFM_MAX_WAN_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_WCDMA_BANDS];
#endif 
  
};
#ifdef __cplusplus
}
#endif
