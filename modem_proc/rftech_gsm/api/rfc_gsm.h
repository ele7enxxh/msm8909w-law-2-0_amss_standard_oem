/*!
   @file
   rfc_card_gsm.c

   @brief
   This file contains all the GSM mode specific implementations to support RFC.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfc_gsm.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
12/15/14   sml     Rewrite ASD API to reduce wake-up timeline
12/02/14   zbz     Make rfc gsm timing ptr per device 
09/24/14   hoh     Add multislot APT control feature
08/22/14   rr      Changes to support nv c3
07/31/14   tsr     GSM Alternate path support
07/30/14   cj      Added changes for tx sharing feature
07/25/14   sb      bug fixes for V1 IP2 cal functionality 
07/01/14   ch      Add Tuner manager tx_disable 
06/18/14   ch      Pass txlm_buf_index to tx_burst_device_config (for CL tuner initialisation)
06/02/14   sml     Adding necessary check condition in the case where tuners are not present 
                   and ASD switch is present to avoid un-necessary prints
05/16/14   sn      Added sleep_device_config_tx for ASM sleep 
04/29/14   sc      Manage Vregs based on subscription
04/29/14   tsr     Mainline GSM RFC config v2 apis 
04/24/14   sw      Introduce rffe-based api for ASD processing 
04/14/14   sw      Disable tuners during X2G iRAT scenarios (port from DI3)
04/01/14   sw      Port from Dime3 (Re-design of asdiv to support multiple configurations) - reintroduced
03/28/14   hoh     Revert back CL5565452 due to boot up failure
03/26/14   sw      Port from Dime3 (Re-design of asdiv to support multiple configurations)
03/20/14   sw      Introduce function to get the timing info for a signal type 
03/17/14   kg      Port from Dime 
02/11/14  as/sr    Re-design asdiv_tuner to support multiple RF configurations
03/05/14   tsr     Removed unwanted parameters to tx_band_config api
02/25/14   tsr     Update RX PLL setting time from device driver to rfgsm core rx timing struct 
02/24/14   tsr     Mainline Device driver script retrieval and Deprecate Feature D3925
02/11/14   vv      backedout the previous change
02/06/14   sg      Handle the RFC_DEVICE_MISSING_FATAL scenario for RF daughter card support
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech
09/27/13   tsr     Added support to retrieve individual device scripts 
08/28/13   sml     Extracting ASD script depending on device id through
                   call back data
08/27/13   sb      Add changes to update RX timing for GSM RXD 
08/07/13   cpm     atuner init() scripted into SBI events
08/01/13   sb      Added interface chanegs to support GSM RX diversity 
07/29/13   sml/svi ASD support for GSM tech
06/20/13   sb      Add hooks to populate Spur channel list from RFC 
06/17/13   svi     "TX_GTR_THRES" NV values interpretation fix. 
04/08/13   tc      Add timing hooks for QPOET and QTUNER 
04/02/13   sb      Fix Null check in init rf devices 
04/01/13   sb      RFC gsm cleanup 
03/27/13   sb      Added support for Enhanced APT 
03/20/13   ec      Replace deprecated RFM_MAX_DEVICES with RFM_MAX_WAN_DEVICES
02/27/13   lcl     Increase data structure to support two or more of the same
                   devices type.  For example, two or more ASMs.
02/05/13   sb      Created separate APIs to configure RF front end devices based on whether
                   they are called every TX/RX burst or only when the band is changed. 
01/3/13    av      Changes to support chain2 for GSM
12/21/12   sr      Use rx timing for rx ant switch timing. 
12/20/12   ggs     Add framework for appending rf_on grfc info from rfc  
12/19/12   sb      Optimize GSM APT 
12/11/12   sb      Add hooks for GSM APT  
11/16/12   tsr     Tx Alpha and TX SBI start delta update for DIME based on wtr1605 
10/30/12   sr      changes create tech rfc object if the tech is supported by 
                   the specific rf-card.
09/04/12   sr      added Tx_gain over-ride API.
08/10/12   sr      Made changes to add the rf_on signal to transition tx script.
08/07/12   sr      added rf front-end device config api calls 
07/27/12   sr      renamed rfdevice related enum types.
07/24/12   kb      Fixed Compiler errors
07/20/12   sr      Made changes to pass the RFM_DEVICE and rf_buffer to rfc_gsm
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type.
07/11/12   sr      changes to initialize the devices as required.
07/09/12   sr      changes to use the rfc_data object instead of rfc_ag function ptr table.
05/22/12   sr      Move the manage_vregs() to base class from rf-card files.
05/18/12   sr      RFC-EFS code clean-up changes.
04/18/12   jps/lcl PA Range and PA R0/R1 logic decoupling 
03/20/12   sr      Added support for rfc binary data
02/28/12   dej     Hooked up PA start/stop delta adjustments in FTM mode. 
                   ( Added tx_timing_adjust(band) ) 
11/04/11    sr     made changes to update ths tx timing based on 
                   pa_start(stop)_time_offset NV value.
10/28/11   vrb     Add Desense channel list for each band, for DC Spur Removal
10/11/11    sr     added rfc tx disable/enable sleep/wakeup calls to configure 
                   RFC signals for tx disable/enable  sleep/wakeup state.
10/11/11    sr     optimized code for ISR over-run issue.
08/23/09   sr      made changes to move the common functionality to base class.
                   and derived RFC classes will implement RFC specific code.
08/20/09   sr      added init_once() to update the GRFC timing based on NV..
04/28/11   rsr     Support for GSM to use NPA APIs
02/18/11   sr      Added static get_instance() to create and return the obj ptr.
01/03/11   bmg     Added virtual destructor for the base class
07/08/09   av      Cleanup and added port configuration (RTR) data  in RFC
11/21/08   dyc     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
extern "C" {
#include "rfc_card.h"
#include "rfc_card_gsm.h"

}

#include "rfa.h"
#include "rfdevice_class.h"
#include "rfc_class.h"

class rfc_gsm : public rfa
{
public:
  static rfc_gsm * get_instance(void);
  static rfc_gsm * create_instance(void);

  virtual void init_once(rfm_device_enum_type rfm_dev);
  virtual void config_mode(rfm_device_enum_type rfm_dev, rf_buffer_intf* enter_mode_buffer,uint8 alt_path);
  virtual void disable_rffe(rfm_device_enum_type rfm_dev, rf_buffer_intf *buffer, rfcom_gsm_band_type band);
  virtual void* get_mode_config_data(rfm_device_enum_type rfm_dev, 
                                     rfc_mode_config_data_enum_type config_data);
  virtual int32 command_dispatch( rfm_device_enum_type rfm_dev, int32 cmd, void *data );

  virtual void *rx_band_config_get(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                                   rfc_band_config_data_enum_type config_data);
  virtual void tx_band_config(rfm_device_enum_type rfm_dev,
                              rfcom_gsm_band_type rfgsm_band,
                              rf_buffer_intf* tx_band_script);

  virtual void rx_burst_config(rfm_device_enum_type rfm_dev,
                                  rfcom_gsm_band_type rfgsm_band,
                                  uint16 arfcn,uint8 alt_path);

  virtual void tx_burst_config(rfm_device_enum_type rfm_dev,
                               rfcom_gsm_band_type rfgsm_band,
                               uint16 arfcn);
  virtual void tx_gain_config( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                               rfgsm_modulation_type mod_type, uint8 slot_num, int32 pwr_in_dbm, 
                               uint8 pa_range, rf_buffer_intf *tx_trans_buffer, boolean bias_flag, 
                               uint16 bias, uint8 apt_mode, boolean override_start_time);

  virtual boolean ip2_cal_device_config(rfm_device_enum_type rfm_dev,
                                       rfcom_gsm_band_type band,
                                       uint16 arfcn,
                                       void *setup_script_ptr,
                                       void *cleanup_script_ptr);
  virtual boolean get_updated_rf_port( rfm_device_enum_type rfm_dev,
									   rf_card_band_type curr_band, 
                                       uint8 alt_path);

  virtual void append_rf_off_on_entries(rfm_device_enum_type rfm_dev, rf_buffer_intf *tx_trans_buffer);
  virtual boolean rf_on_grfc_ctl( rfm_device_enum_type rfm_dev ); 
  virtual boolean is_papm_multislot_ctl_supported( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfgsm_band );
  virtual void tx_gain_override(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                                rfgsm_modulation_type mod_type, uint8 pa_range, 
                                rf_buffer_intf *tx_trans_buffer, boolean bias_flag, 
                                uint16 bias, uint8 apt_mode, boolean override_start_time);

  virtual void *tx_band_config_get(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band,
                                   rfc_band_config_data_enum_type config_data);

  virtual int32 tx_disable(rfm_device_enum_type rfm_dev);
  virtual int32 tx_enable(rfm_device_enum_type rfm_dev);

  virtual int32 tx_timing_adjust(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band);

  virtual int32 sleep(rfm_device_enum_type rfm_dev);
  virtual int32 wakeup(rfm_device_enum_type rfm_dev,uint8  alt_path);
  virtual boolean populate_spur_chan_list();

  rfdevice_class *get_rf_device_obj(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,
                                    rfcom_gsm_band_type band, rfdevice_type_enum_type dev_type, uint32 instance);

  rfdevice_class** get_rf_device_list(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx,rfcom_gsm_band_type band,
                                      rfdevice_type_enum_type dev_type);

  rfcom_gsm_band_type get_supported_band(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx);

  uint32 get_rf_path(rfm_device_enum_type rfm_dev, rfc_rxtx_enum_type rx_tx, rfcom_gsm_band_type band,  uint32 instance);

  boolean update_rx_timing_params(uint32 rx_burst_time, 
								  uint16 num_sbi_before_pll, 
								  uint16 num_sbi_from_pll,
                                  uint16 rx_pll_settling_time_qs,
                                  rfm_device_enum_type rfm_dev);

  virtual void manage_vregs(rf_path_enum_type path, rfcom_gsm_band_type band, rfm_path_state state, rfm_mode_enum_type mode);
  virtual boolean get_cfg_info(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfgsm_band, rfc_rxtx_enum_type rx_tx, rfc_cfg_info_type *cfg_struct, uint32 alt_path);
  #ifdef FEATURE_RF_ASDIV
    virtual boolean append_txd_grfc(uint8 command, rf_buffer_intf *start_buffer, void *cb_data);
    virtual void append_asdiv_rffe(uint8 command, rf_buffer_intf *start_buffer, void *cb_data);
    virtual void get_timing_info( rfm_device_enum_type rfm_dev, rfcom_gsm_band_type rfgsm_band, rfdevice_type_enum_type dev_type, rfc_signal_type sig_info, int16 *start_delta);
  #endif    

  boolean get_device_init_complete(void);

  boolean initialize_rfc_timing_info(rfm_device_enum_type rfm_dev);

  virtual ~rfc_gsm();

protected:
  // Protected constructor for singleton implemenation
  rfc_gsm(void);

  // Contains the HW ID of the RF card
  static rf_hw_type rfc_hw_id;

  static rfc_gsm *rfc_gsm_ptr;

  rfc_gsm_rx_timing_info_type rfc_gsm_rx_timing_info[RFM_MAX_WAN_DEVICES];     /* rx burst timing info. */
  rfc_gsm_rx_timing_info_type rfc_gsm_rx_mon_timing_info[RFM_MAX_WAN_DEVICES]; /* monitor burst timing info. */
  rfc_gsm_tx_timing_info_type rfc_gsm_tx_timing_info[RFM_MAX_WAN_DEVICES];    /* tx burst timing info. */
  static mdsp_xo_desense_arfcn_struct rfc_gsm_desense_chan_list; /* desense channel list for all bands*/
  rfc_gsm_core_config_type  rfc_gsm_core_config_table[RFM_MAX_WAN_DEVICES];    /* rfc gsm core config data */
  static rfc_gsm_pa_gain_range_map_type rfc_gsm_pa_gain_range_map; /* pa gain range map */ 
  static rfc_gsm_supported_band_map_type rfc_gsm_supported_band_map; /*Maps a logical device with a supported band*/

private:
  void update_timing_based_on_nv(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, rfc_sig_cfg_type *rf_signals);
  void construct_tx_burst_script(rfm_device_enum_type rfm_dev);
  void update_timing_based_on_card(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, rfc_sig_cfg_type *rf_signals);
  void init_rf_devices(rfc_cfg_params_type *cfg, rfc_device_info_type *device_info_ptr);
  void init_rfdevices_for_all_bands(rfm_device_enum_type rfm_dev);
  void allocate_script_buffers(void);

  /* rf-front-end devices config APIs */
  boolean tx_band_device_config(rfm_device_enum_type rfm_dev, rfdevice_class* dev_obj, rfcom_gsm_band_type band,
                                rf_buffer_intf* band_buffer);

  boolean tx_burst_device_config(rfm_device_enum_type rfm_dev,
                                 rfdevice_class *dev_obj,
                                 rfcom_gsm_band_type band,
                                 uint16 arfcn,
                                 uint32 txlm_buf_index);

  /* rf-front-end devices config APIs */
  boolean rx_burst_device_config(rfm_device_enum_type rfm_dev,
                                    rfdevice_class *dev_obj,
                                    rfcom_gsm_band_type band,
                                    rfc_sig_cfg_type *sig_ptr,
                                    uint16 arfcn);

  boolean tx_gain_device_config(rfm_device_enum_type rfm_dev, rfdevice_class *dev_obj, rfcom_gsm_band_type band, 
                                       uint8 pa_range, rfgsm_modulation_type mod_type,
                                       rf_buffer_intf *tx_trans_buffer,uint8 slot_num, rf_buffer_intf *tx_start_buffer, 
                                       boolean bias_flag, uint16 bias, uint8 apt_mode, boolean override_start_time);

  boolean tx_disable_device_config(rfdevice_class *dev_obj, rfcom_gsm_band_type rfgsm_band, uint16 arfcn, rfm_device_enum_type rfm_dev);
  boolean tx_enable_device_config(rfdevice_class *dev_obj, rfm_device_enum_type rfm_dev);
  boolean sleep_device_config_tx(rfdevice_class *dev_obj);
  boolean sleep_device_config(rfdevice_class *dev_obj);
  boolean wakeup_device_config(rfdevice_class *dev_obj, rf_buffer_intf *wakeup_buffer, boolean pwr_off_in_sleep);

  rfdevice_class *gsm_rf_devices[RFM_MAX_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_GSM_BANDS][RFDEVICE_TYPE_MAX_NUM][RFC_MAX_DEVICE_INSTANCES_PER_BAND];


  uint32 trx_device_rf_path[RFM_MAX_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_GSM_BANDS][RFC_MAX_DEVICE_INSTANCES_PER_BAND];

  rfc_antenna_parameter_type ant_params[RFM_MAX_WAN_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_GSM_BANDS];
};

