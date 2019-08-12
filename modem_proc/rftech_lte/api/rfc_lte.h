#ifndef RFC_LTE_H
#define RFC_LTE_H

/*!
   @file
   rfc_lte.h

   @brief
   This file contains the LTE RFC class definition
   The LTE RFC class encapsulates the actual RF card that is used when in LTE mode.
   The LTE RFC class also provides all the exposed interfaces to external modules
   The LTE RFC class is a singleton implementation

*/

/*===========================================================================

Copyright (c) 2010 - 2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rfc_lte.h#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
11/26/15   lhb     FR 31436: API to provide the HW band limitation to LTE RRC
01/14/14   dw      Ported : GRFC Alt Path Support
12/15/14   ars     Adding check_intra_ca_feasible() API to fix issues with intra CA on split bands. 
12/05/14   dw      Changes to support split bands based on split specified in RFC.
10/20/14   ndb     Remove the Deprecated  API's 
08/15/14   st      NS07 Port Update Support
08/01/14   ndb     Added support for extended EARFCN for LTE
07/10/14   jf      TM8 + SRS switching for TDD 
06/27/14   gvn     Modify algorithm for split-intraband support 
07/02/14   jf      AsDiv support 
06/11/14   bar     Added disable_fb_device() API to support IQ capture 
05/15/14   gvn     Add Api for getting overlapping split bands
04/23/14   tks     Support for dynamic path selection
04/11/14   bsh     Split Band Support
03/27/14   svi     [ASDIV] asdiv_tuner to support multiple RF configurations
01/09/14   svi     Split Band Support
04/25/13   gvn     Support for Split bands for LTE 
04/10/13   kab     Added support for Tx Antenna Swap. 
03/20/13   kab     Decreased memory allocated to lte_rf_devices 
03/12/13   sn      XPT dual WTR fix for feedback path
11/15/12   adk     Backed out Qtuner code
09/14/12   jf      Qtuner LTE support (initial)
10/31/12   vss     Change to get device API for returning a list of objects
10/30/12   sr      changes create tech rfc object if the tech is supported by 
                   the specific rf-card.
08/16/12   pl      Update RFC configuration strucutre to reflect new changes in RFC 
07/27/12   sr      renamed rfdevice related enum types.
07/18/12   sr      added get_device api to return the rf device obj ptr
                   based on logical_path, rx_tx, band and device_type.
07/12/12   sr      push band-port mapping info to devices 
06/01/12   aca     Fix to clean up Tx signals when disable tx is called 
05/24/12   jf      Move the manage_vregs() to base-class from rf-card specific files 
05/18/12   sr      RFC-EFS code clean-up changes.
05/07/12   jf      Added support for rfc binary data
04/24/12   cd      Fixed LTE exit mode to restore init settings for both Tx and Rx band GRFC/ANT_SELs
03/01/12   jf    Added LPM status support
12/05/11   tnt     Add mode_sleep() 
06/29/11   pl      Added path and band information in mode_exit()
06/23/11   pl      added mode_exit() interface
06/23/11   pl      Rename config_mode and get_config_mode_data 
04/13/11   aak     Revert name to rfc_lte_manage_vregs()
04/13/11   aak     Change rfc_lte_manage_vregs() to rfc_lte_manage_power()
01/13/11   pl      Use rfcom_lte_band_type instead of rf_card_band_type
12/15/10   pl     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
#include "rfc_card.h"
#ifdef __cplusplus
} /* extern "C" */
#endif


#include "rfdevice_class.h"
#include "rfa.h"
#include "rfc_class.h"
#include "rflte_mc.h"

/*===========================================================================
                           Class Definition
===========================================================================*/
#ifdef __cplusplus
class rfc_lte_intf : public rfa
{
public:
  // -------- LTE RFC API ------
  // Pure Virtual interfaces for Interface class
  virtual void init(void);
  
  virtual void tx_disable(rfm_device_enum_type device,rfcom_lte_band_type band);

  virtual void mode_sleep(rfm_device_enum_type device, rfcom_lte_band_type band, uint8 alt_path);

  virtual void mode_exit(rfm_device_enum_type device, rfcom_lte_band_type rx_band, rfcom_lte_band_type tx_band, uint8 alt_path);

  virtual void mode_config(rfm_device_enum_type device);

  virtual void* mode_config_get(rfm_device_enum_type device, rfc_mode_config_data_enum_type config_data);

  virtual void rx_band_config( rfm_device_enum_type device, rfcom_lte_band_type band, uint8 alt_path);

  virtual void *rx_band_config_get(rfm_device_enum_type device, rfcom_lte_band_type band,
                                   rfc_band_config_data_enum_type config_data, uint8 alt_path);

  virtual void tx_band_config( rfm_device_enum_type device, rfcom_lte_band_type band);

  virtual void *tx_band_config_get(rfm_device_enum_type device, rfcom_lte_band_type band,
                                   rfc_band_config_data_enum_type config_data);

  virtual int32 command_dispatch( rfm_device_enum_type device, int32 cmd, void *data);

  virtual void manage_vregs(rfc_power_management_info* pm_info);
  
  virtual boolean setup_fb_path(rfm_device_enum_type device_fb, rfcom_lte_band_type band, boolean et_enable);												 
  
  virtual boolean setup_fb_device(rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_lte_band_type band, uint32 tx_chan);  

  virtual boolean disable_fb_device(rfm_device_enum_type device, rfm_device_enum_type device_fb,rfcom_lte_band_type band, uint32 tx_chan);

  void** get_rf_device_obj( rfm_device_enum_type rfm_dev, 
                            rfc_rxtx_enum_type rx_tx,
                            rfcom_lte_band_type band, 
                            rfdevice_type_enum_type dev_type );

  void get_rx_band( boolean *split_band, 
                    rfcom_lte_earfcn_type rx_chan, 
                   rfcom_lte_band_type *rfc_band);
  void get_tx_band( boolean *split_band, 
                    rfcom_lte_earfcn_type tx_chan, 
                    rfcom_lte_band_type *rfc_band,
                    boolean filtered_band );
  void get_tx_hi_lo_earfcn(rfcom_lte_band_type split_band,
                           rfcom_lte_earfcn_type*low_earfcn,
                           rfcom_lte_earfcn_type *high_earfcn);

  virtual rfm_device_enum_type get_mapped_device_for_scell 
    ( rfcom_lte_band_type pcell_band,
      rfcom_lte_band_type scell_band, 
      rfm_device_enum_type scell_rfm_device );

  virtual void reset_grfc_band_config( rfm_device_enum_type device, 
                                       rfcom_lte_band_type band,
                                       uint8 alt_path);

  boolean get_updated_rf_port( rfm_device_enum_type rfm_dev, 
                               rf_card_band_type curr_band, 
                               uint8 alt_path );

  boolean get_delta_dev( rfm_device_enum_type rfm_dev, 
                         rf_card_band_type curr_band, 
                         uint8 prev_alt_path, uint8 next_alt_path ); 
  
  /* These new APIs are for querying band based on additional params */
  void get_rx_band_v2(boolean *split_band, rfcom_device_enum_type device, rfcom_lte_earfcn_type rx_chan, rfc_lte_bw_type curr_bw, 
                      uint8 path_index_override, rfcom_lte_band_type *rfc_band);

  void get_tx_band_v2(boolean *split_band, rfcom_lte_earfcn_type tx_chan, rfc_lte_bw_type curr_bw,
                      uint8 path_index_override, rfcom_lte_band_type *rfc_band, boolean filtered_band);

   void get_rx_hi_lo_earfcn(rfcom_lte_band_type split_band, rfcom_lte_earfcn_type *low_earfcn, rfcom_lte_earfcn_type *high_earfcn);


void get_split_rx_bands(boolean *split_band, 
                        rfcom_lte_earfcn_type rx_chan, 
                        rfcom_lte_band_type *rfc_band1, 
                        rfcom_lte_band_type *rfc_band2);
void get_rx_split_band_channel_rasters(rfcom_lte_band_type split_band, 
                                       rfcom_lte_earfcn_type *low_earfcn,
                                       rfcom_lte_earfcn_type *high_earfcn,
                                       rfcom_lte_band_type *band_list,
                                       uint8 *num_of_splits);

boolean check_intra_ca_feasible(rflte_mc_carrier_info_rfc_type *pcc_info, 
                                rflte_mc_carrier_info_rfc_type *scc_info,
                                rfcom_lte_band_type *intra_ca_band);
boolean rfc_get_lte_properties (rfc_lte_properties_type **ptr);

  virtual void *get_asd_xsw_config(rfm_device_enum_type device, rfcom_lte_band_type band,int position);
  virtual boolean generate_srs_xsw_script
  ( 
    uint32 position,
    rf_buffer_intf *srs_xsw_script_ptr,
    rfm_device_enum_type device,
    rfcom_lte_band_type band,
    boolean immediate
  );

  boolean get_ns07_ports
  (
	rfm_device_enum_type rfm_dev, 
	rfcom_lte_band_type rf_band  
  );

  uint64 get_split_bands(rfcom_lte_band_type band);
  
  #ifdef FEATURE_RF_ASDIV  
  virtual boolean generate_asdiv_xsw_script
  ( 
    uint32 position,
    rf_buffer_intf *asdiv_script_ptr,
    void *cb_data
  );
  #endif
  // ----------------------------------

  // Singleton GetInstance
  static rfc_lte_intf *Get_Instance(void);

  // Singleton create instance
  static rfc_lte_intf *create_instance(void);

  // Destructor
  virtual ~rfc_lte_intf();

protected:
  // Singleton instance pointer
  // This pointer is to be initialized by the getInstance method of the child
  // class that implements the virtual interfaces of RFC LTE class
  static rfc_lte_intf *RFC_LTE_p;

  // Add LPM status for All bands in LTE, initialized LMP status for all band to False
  boolean band_lpm_enable_status[RFCOM_NUM_LTE_BANDS];

  // Constructor
  rfc_lte_intf();

  void init_rfdevices(void);

  rfcom_lte_band_type rx_band[RFM_MAX_DEVICES];
  rfcom_lte_band_type tx_band[RFM_MAX_DEVICES];

private:
  void init_rf_devices(rfc_cfg_params_type *cfg, rfc_device_info_type *device_info_ptr);
  void init_rfdevices_for_all_bands(void);

  void *lte_rf_devices[RFM_MAX_WAN_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_LTE_BANDS][RFDEVICE_TYPE_MAX_NUM][RFC_MAX_DEVICE_INSTANCES_PER_BAND];

  rfc_antenna_parameter_type ant_params[RFM_MAX_WAN_DEVICES][RFC_CONFIG_RXTX_MAX][RFCOM_NUM_LTE_BANDS];
    
  // NONE
};
#endif /* __cplusplus */

#endif /* RFC_LTE_H */

