/*!
   @file
   rfc_tdscdma.h

   @brief
   This file contains all the TDSCDMA mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2011 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rfc_tdscdma.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
11/02/14   qzh     Get PA related timing from RFC file instead of using default value for GRFC PA
07/10/14   jyu     Asd bringup with rfc changes and device script 
06/16/14   tks     Added support for get updated port config 
03/28/14   qzh     [Merge]Handle the RFC_DEVICE_MISSING_FATAL scenario for RF daughter card support on TDS
03/28/14   kb      [Merge][XPT] Add support for porper shutdown of WTR during XPT deconfig.
03/26/14  as/sr    [Merge]Re-design asdiv_tuner to support multiple RF configurations 
                   (porting per CR612404) 
09/18/13   ms      [Merge]Add api for getting filtered band 
07/26/13   ms      [Merge]AsDiv support 
05/20/13   al      [merge]Added functions to support split band
04/30/13   kb      [Merge][XPT] Support to set up feedback path and device for EPT 
03/18/13   jyu     Change to use RFM_MAX_WAN_DEVICES and 
                   RFC_MAX_DEVICE_INSTANCES_PER_BAND
12/21/12   jyu     Modified the get_rf_device_obj function to return double pointer  
10/30/12   sr      changes create tech rfc object if the tech is supported by 
                   the specific rf-card.
09/26/12   jyu     Changed to use logic device instead of logic path 
09/26/12   jyu     Added interface to return device_info  
09/14/12   jyu     Added interface to return band specific pa lut map 
08/20/12   jyu     Added support to set/clean specific GRFC (immediate execution)
08/20/12   jyu     Added support to cleanup GRFCs  
07/31/12   jyu     Fixed compilation errors 
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type.
07/17/12   sr      rfc clean-up for dime. 
04/12/12   ycl     Added wakeup/sleep.
08/15/08   zg      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfc_card.h"
#include "rfc_card_tdscdma.h"
#include "rfc_class.h"
#include "rfcommon_msg.h"
#include "rfcommon_asdiv_manager.h"
#include "rf_tdscdma_mdsp_rf_event.h"
}

class rfc_tdscdma
{
public:
  static rfc_tdscdma * get_instance(void);
  static rfc_tdscdma * create_instance(void);

  virtual void sleep(rfm_device_enum_type device);
  virtual void wakeup(rfm_device_enum_type device);
  virtual void config_mode(rfm_device_enum_type device);
  virtual void* get_mode_config_data(rfm_device_enum_type device, 
                                     rfc_mode_config_data_enum_type config_data);
  virtual int32 command_dispatch( rfm_device_enum_type device, int32 cmd, void *data );
  virtual void rx_band_config( rfm_device_enum_type device, rfcom_tdscdma_band_type band );
  virtual void *rx_band_config_get(rfm_device_enum_type device, rfcom_tdscdma_band_type band,
                                   rfc_band_config_data_enum_type config_data);
  virtual void tx_band_config( rfm_device_enum_type device, rfcom_tdscdma_band_type band );
  virtual void *tx_band_config_get(rfm_device_enum_type device, rfcom_tdscdma_band_type band,
                                   rfc_band_config_data_enum_type config_data);
  virtual void** get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                          rfcom_tdscdma_band_type band, rfdevice_type_enum_type dev_type);
  virtual void get_band_pa_lut_type(rfm_device_enum_type device, rfcom_tdscdma_band_type band, 
                            uint32* rf_asic_band_pa_lut_map);  
  virtual void manage_vregs(rfc_power_management_info* pm_info);

  virtual boolean setup_fb_path(rfm_device_enum_type device_fb, rfcom_tdscdma_band_type band, boolean xpt_enable);		
  
  virtual boolean setup_fb_device(rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_tdscdma_band_type band, uint16 tx_chan, boolean xpt_enable);
  
  virtual void *get_asd_xsw_config(rfm_device_enum_type device, rfcom_tdscdma_band_type band,int position);

#ifdef FEATURE_RF_ASDIV  
  virtual boolean generate_asdiv_xsw_script
  ( 
    uint32 position,
    rf_buffer_intf *asdiv_script_ptr,
    void *cb_data
  );
#endif


  void get_rx_band(boolean *split_band, uint16 rx_chan, rfcom_tdscdma_band_type *rfc_band);
  void get_tx_band(boolean *split_band, uint16 tx_chan, rfcom_tdscdma_band_type *rfc_band);
  void get_filtered_band(rfcom_tdscdma_band_type band, rfc_coex_filter_status_type filter_status, 
                               rfcom_tdscdma_band_type *rfc_band);
  void get_tx_hi_lo_earfcn(rfcom_tdscdma_band_type split_band, uint16 *low_earfcn, uint16 *high_earfcn);
  
  boolean get_device_init_complete(void);

  boolean get_updated_rf_port(rfm_device_enum_type rfm_dev, rf_card_band_type curr_band, uint8 alt_path);

  boolean grfc_pa_existed(void);

  virtual ~rfc_tdscdma();

protected:
  static rfc_tdscdma *rfc_tdscdma_ptr;

  // Protected constructor for singleton implemenation
  rfc_tdscdma(void);

private:

  void band_config_grfc_notime(rfm_device_enum_type device, rfcom_tdscdma_band_type band,
                               rfc_signal_type sig_type, rfc_rxtx_enum_type rx_tx, boolean set_grfc, rfc_logic_type override_logic);

  boolean init_rf_devices(rfc_cfg_params_type *cfg, rfc_device_info_type *device_info_ptr);
  boolean init_rfdevices_for_all_bands(void);

  void *tdscdma_rf_devices[RFM_MAX_WAN_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_TDSCDMA_BANDS][RFDEVICE_TYPE_MAX_NUM][RFC_MAX_DEVICE_INSTANCES_PER_BAND];

  rfc_antenna_parameter_type ant_params[RFM_MAX_WAN_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_TDSCDMA_BANDS];
  
  /* Flag indicating if device initialization was successful */
  boolean device_init_complete_status;
  
};

