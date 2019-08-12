#ifndef RFC_CLASS_H
#define RFC_CLASS_H
/*!
  @file
  rfc.h

  @brief
  This file contains the class definition of the RFC container base class.
  The RFC container base class defines the card-specific but tech-indenpendent
  attributes and behviors.

  Each specific RF card is to implement the interfaces provided in this contains
  class

*/

/*===========================================================================

Copyright (c) 2011-15 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/rfc/inc/rfc_class.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/03/15   vv      Added rfc support for ADC VCM Cal feature
02/24/15   am      Change IRAT alt path lookup implementation
10/10/14   tks     Updated interface to use optimized path selection table
09/29/14   Saul    New alt path selection APIs.
09/02/14   vv      Added device cfg info to physical device array
08/13/14   tks     Modified get alt path table function param list 
08/12/14   aak     Move PAM calls for LTE light sleep to the vreg manager
08/12/14   sb      Set XO drive strength based on the WTRs present in an RFC
07/14/14   vrb     Support for Alternate Path Selection Table
07/09/14   aak     Changes for API to control MPP for DAC Vref
06/16/14   vv      Updated rfc_intf with logical devices size 
06/09/14   vv      Enable physical device creation for QFE devices with new physical/logical device config structures
05/02/14   spa     Propagate CDMA RFC failure up to RFM init
05/01/14   Saul    Increase max device instances to 17.
04/23/14   tks     Support for dynamic path selection
03/17/14   kg      Port from Dime 
02/11/14   sr      Re-design asdiv_tuner to support multiple RF configurations
01/16/14   ndb     Added Daughter Card support(NON_FATAL_DEVICES_SUPPORT).
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
06/29/13   hm     Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
06/07/13   vs      Updated changes for third party tuner and increase #tuner to 5
05/17/13   kai     Add new api to get cmn device properties
04/08/13   bmg     Add support for keeping an array of physical devices to
                   represent chips in the card.
03/05/13   sr      Added self_test support.
02/21/13   sr      Added support to return WTR device for the tx dac.
01/31/13   pv      Support Dime interface requirements + CA for Device Manager.
01/16/13   sr      Increased the max devices to 15.
11/19/12   aki     Added more cleanup code to fix OFT memory leaks
10/29/12   sbm     maximum 5 device instances are allowed.
09/24/12   sr      Made changes to remove hand-coded RFC files.
07/27/12   sr      renamed rfdevice related enum types.
07/18/12   sr      Changes to create devices through device factory.
07/09/12   aak     Move Quiet mode API from device to RFC
06/19/12   vss     Changes to use device factory
05/23/12   sr      Moved the generic code to rfc base-class.
05/18/12   sr      Made chanegs to derive all the rfc classes from rfa class.
04/18/12   swb     Override new operator to allocate memory from MODEM RFA
03/20/12   sr      Added support for rfc binary data
01/16/12   pv      RF Common Core Device Manager Implementation.
09/27/11   aro     Added interface to load int device calibration data
09/19/11   aro     Added common RFC interface to perform target specific
                   RC Tuner calibration
09/14/11   aro     Added interface to query the common logical device
                   parameter
08/25/11   aak     Changes to enable NPA voting from RFSW for resources needed
08/15/11   aak     New RFC interface for rfc_get_awake_resource_request()
08/05/11   sr/plim Old RFC cleanup.
06/30/11   cd      Added class function to query card specific device
                   configuration information
06/07/11   shb     Created rfc_init_status flag as private member of rfc_intf
                   base class. Added accessor functions: protected method
                   set_init_status() and public method get_init_status()
02/03/11   pl      Adding a member function to allow gpio configuration of
                   GPIO.
01/17/11   pl      init version
============================================================================*/

/* Max RF devices per device type per rf card*/
#define RFC_MAX_DEVICE_INSTANCES 17
#define RFC_MAX_DEVICE_INSTANCES_PER_BAND 5

#define RFC_ENABLE_RFFE_SCAN_EFS_DUMP true

#include "rfcom.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
system_clock_enum rfc_card_get_sys_clk_type(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef __cplusplus

#include "rfa.h"

extern "C" {
#include "DDITlmm.h"
}

#include "rfc_card.h"
#include "rfc_common.h"
#include "txlm_intf.h"
#include "rxlm_intf.h"
#include "rfdevice_class.h"
#include "rfdevice_physical_device.h"


/*A new structure to track the physical device status*/
typedef struct
{
 rfdevice_physical_device* device_obj;
 rfc_phy_device_info_type* phy_dev_cfg;
 rfc_device_availability_enum_type device_status;
}rfc_physical_device_struct_type;

/*!To store the trx to adc & phy_path mapping*/
typedef struct
{
 uint32 trx_logical_instance;
 uint32 phy_path;
 uint32 modem_chain;
 rfdevice_rx_vco_path_enum_type vco_path;
}trx_phy_path_adc_mapping_type;

/*A new structure to track the logical device status*/
typedef struct
{
 rfdevice_class* device_obj;
 rfc_device_availability_enum_type device_status;
}rfc_logical_device_struct_type;


/* Definition of RFC Common base class */
class rfc_intf : public rfa
{
public:
  // Singleton constructor
  rfc_intf(rf_hw_type rfhw, system_clock_enum sysclk);

  // RFC Container API
  system_clock_enum get_sys_clk_type(void);

  // Public accessor function to get value rfc_card_init_success
  boolean get_init_status(void);

  /* API to query the base class for the dev_param info*/
  const rfc_common_logical_device_params_type*
                          get_logical_dev_param(rfm_device_enum_type device);

  // Abstract API
  virtual void command_dispatch(rfm_path_enum_type path, rfcs_cmd_type cmd, void *data );

  // TODO: To be moved to tech specific interfaces
  virtual void manage_vregs(rfm_path_enum_type path, rfc_vregs_id_type vreg_id, rfc_vregs_cmd_type vreg_cmd);

  // API to provide RFM layer with card specific device configuration information, used by TRM
  virtual boolean get_devices_configuration(rfm_devices_configuration_type *dev_cfg);

  // API to provide RFM layer with power mgmt data to be used by MCPM
  virtual void get_awake_resource_request(rfm_resource_info *dev_cfg);

  // API to set Quiet mode from RF
  virtual void set_quiet_mode(rfm_mode_enum_type mode, boolean on_off);

  // API to turn on vregs in autocal mode
  virtual void common_enable_autocal_vregs(boolean on_off);

  // API to turn on DAC Vref MPP
  virtual void common_enable_mpp_dac_vref(boolean on_off);

  // API for light sleep vregs
  virtual void lte_light_sleep_vregs(void);

  // API for light sleep wakeup vregs
  virtual void lte_light_sleep_wakeup_vregs(void);

  virtual const boolean do_tuner_device_calibration_top(void);

  virtual const boolean do_internal_device_calibration_top(void);

  virtual const boolean get_rfc_data (void);

  virtual const boolean do_internal_device_calibration(uint8 instance_num);

  virtual const boolean load_internal_device_calibration(void);

  virtual boolean get_cmn_device_properties(rfc_cmn_properties_type **ptr);

  virtual rfc_alt_path_sel_type* get_alt_path_selection_tbl(uint32 *tbl_size);
  virtual rfc_alt_path_sel_type* get_irat_alt_path_selection_tbl(uint32 *tbl_size, uint32 *num_band_groups);

  virtual boolean update_port_mapping(rfm_device_enum_type rfm_dev, 
                                      rfcom_mode_enum_type curr_mode, 
                                      rf_card_band_type curr_band, 
                                      uint8 alt_path); 

  virtual boolean get_delta_dev( rfm_device_enum_type rfm_dev,
                                 rfcom_mode_enum_type curr_mode,
                                 rf_card_band_type curr_band, 
                                 uint8 prev_alt_path,
                                 uint8 next_alt_path ); 

  rfdevice_class *get_cmn_rf_device_object(rfdevice_type_enum_type dev_type, 
                                           uint8 instance);

  rfc_device_availability_enum_type get_cmn_rf_device_status(rfdevice_type_enum_type dev_type, 
                                                             uint8 instance);

  rfdevice_class *get_txdac_trx_device(txlm_dac_type txdac);

  boolean do_self_test(rfc_self_test_info_type *self_test_info);

  rfdevice_class *get_dummy_tuner_mgr(uint32 antenna);

  boolean get_asdiv_config_info(int32 config_num, rfc_asdiv_config_info_type **config_info);

  rfc_xo_settings_type* get_rfc_xo_drive_strength_info();

  boolean set_trx_phy_path_adc_mapping(       
                                            uint32 rx_modem_chain,       /*modem chain to map the ADC*/
                                            uint32 phy_path_num,         /*physical path*/
                                            uint32 trx_logical_instance, /* Transciever device type instance*/
                                            rfcom_mode_enum_type mode,  
                                            rfcom_band_type_u band,
                                            rfdevice_class* trx_device,
                                            int32* data_array,
                                            uint8 array_size
                                        );

  boolean get_trx_phy_path_adc_mapping(       
                                            uint32 rx_modem_chain,        /*modem chain to map the ADC*/
                                            uint32 *phy_path_num,         /*physical path*/
                                            uint32 *trx_logical_instance, /* Transciever device type instance*/
                                            rfdevice_rx_vco_path_enum_type *vco_path
                                         );

  boolean get_valid_adc_list(       
                              uint32* adc_list  
                             );


  boolean rfc_rffe_bus_scan(void);

  /* =============================================== */
  // Singleton Get_Instance()
  static rfc_intf *get_instance(void);

  // Destructor
  virtual ~rfc_intf();

  uint8 num_trx;

protected:

  boolean create_cmn_rf_devices(void);

  void destroy_cmn_rf_devices(void);

  boolean create_asdiv_tuner_mgr( void );

  // Protected accessor function to set rfc_card_init_success
  void set_init_status(boolean status);

  void create_lte_rfc(void);

  void create_gsm_rfc(void);

  void create_tdscdma_rfc(void);

  void create_wcdma_rfc(void);

  boolean create_cdma_rfc(void);

  void create_gnss_rfc(void);

  rfc_physical_device_struct_type* find_associated_physical_device(rfdevice_type_enum_type type,
                                                                   uint32 instance);

  bool get_device_self_cal_path(int device_index, char* buffer, int size);

  /*! Structure containing the Logical Device Parameter mapping to Physical RF Devices */
  rfc_common_logical_device_params_type dev_param[RFM_MAX_DEVICES];

  // Singletom reference
  static rfc_intf *rfc_p;

  // Contains the HW ID of the RF card
  rf_hw_type                           rfc_card_id;

  rfc_logical_device_struct_type cmn_rf_devices[RFDEVICE_TYPE_MAX_NUM][RFC_MAX_DEVICE_INSTANCES];
  int phys_devices_count;
  int logical_devices_count;
  rfc_physical_device_struct_type* phys_devices_array;
  rfdevice_class* txdac_trx_device[TXLM_DAC_NUM];
  rfdevice_class* dummy_tuner_mgr[RFC_MAX_ANTENNA];
  trx_phy_path_adc_mapping_type trx_phy_path_adc_mapping[RXLM_CHAIN_MAX];

private:

  // Contains init status of RF card
  boolean                              rfc_card_init_success;

  // Indicates the clocking mechanism used for the RF card
  system_clock_enum                    sys_clk;

  rfc_xo_settings_type                 rf_xo_clk_info[RF_XO_MAX_CLKS];

};
#endif /* __cplusplus */
#endif /* RFC_H */

