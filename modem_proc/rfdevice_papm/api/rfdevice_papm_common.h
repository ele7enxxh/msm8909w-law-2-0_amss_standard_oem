#ifndef RFDEVICE_PAPM_COMMON_H
#define RFDEVICE_PAPM_COMMON_H
/*!
   @file
   rfdevice_papm_common.h

   @brief
   This file contains definition & prototypes for generic 3rd party pa devices 

*/
/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_papm/api/rfdevice_papm_common.h#1 $
$Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/19/14   vv     rfdevice_reg_dump() API  for debug
10/23/14   vv      Added QFE3100 support
10/21/14   px      Added RFM Device support
10/09/14   px      Added EFS support for all papm device specific scripts
05/08/14   vv      Physical device interface support for BOLT
01/20/14   vv      Increasing PAPM max register size
11/15/13   vv      Deleting device information from derived class
10/30/13   sb      Added support for delay after standby
8/13/13    vs      clean up of prepare buffer code
6/13/13    vbh     Added explicit trigger for the PAPM device
05/31/13   sr      auto-bypass disable support for APT mode.
04/26/13   vs      Updated the changes for reading HW part number
03/23/13   sr      changes to support script timing.
03/22/13   sr      changes to PAPM to use quto-gen settings.
03/22/13   sr      changes to PAPM to use quto-gen settings.
01/17/13   shb     Added support to put QPoet in Bypass mode
11/02/12   bn      APT related changes
08/27/12   bn      Storing Device type and ID in the base class variables
08/10/12   bn      Adding PA VBATT LDO programming and last write register
07/26/12   bn      Initial release

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "rfdevice_papm.h"
#include "rf_buffer_intf.h"
#include "rf_rffe_common.h"
#include "rfdevice_papm_types.h"
#include "rfdevice_papm_data.h"
#include "rfdevice_physical_device.h"

#define RFDEVICE_PAPM_MAX_REG_SIZE 30
#define MAX_PATH_SIZE 100

class rfdevice_papm_common:public rfdevice_papm
{
public:
 
   /* Constructor*/
  rfdevice_papm_common(rfdevice_physical_device *phy_dev_obj_ptr,
                       rfc_phy_device_info_type *phy_device_info, 
					   rfc_logical_device_info_type *logical_device_info
					   ); 
 
  void init(); 
   
  boolean enable(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band, 
                 rfdevice_papm_bw_enum_type bw, rf_buffer_intf *script_buffer, 
                 rf_device_execution_type execution_type, int16 script_timing = 0);

  boolean disable(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rf_buffer_intf *script_buffer, 
                  rf_device_execution_type exec_type, int16 script_timing = 0); 

  boolean set_mode_bias(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band, rfdevice_papm_mode_type xpt_mode,
                        uint16 bias, rf_buffer_intf *script_buffer, rf_device_execution_type execution_type, 
                        int16 script_timing = 0); 

  boolean xpt_linearizer_settings(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, 
								  rfdevice_papm_mode_type xpt_mode, uint16 bias, uint8 *settings);
  
  boolean set_band_map(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, 
                       rfcom_band_type_u band, int32 *data_ptr, uint8 size); 

  boolean init_status_get(void); 

  boolean trigger(rf_buffer_intf *script_buffer, rfm_device_enum_type rfm_device, 
	              rf_device_execution_type dev_action, int16 script_timing);

  boolean get_et_voltage_min_max(rfdevice_papm_misc_info* ET_voltage_info_p);
#ifdef FEATURE_RFA_DEVICE_DEBUG
  #error code not present
#endif /* FEATURE_RFA_DEVICE_DEBUG */
  virtual ~rfdevice_papm_common();

  void reset(uint8 usid);
    
protected:

  rfdevice_physical_device* parent_physical_device_ptr;

  /*! pointer to the PA device reg settings data object */
  rfdevice_papm_data *papm_data_ptr;

  rfdevice_papm_misc_info ET_voltage_info;

  boolean init_status; /* PA device initialization status */
  
  uint8 rffe_bus;  /*! RFFE channel/bus information */    
  uint8 default_usid; /*! RFFE Default Unique Slave ID */
  uint8 assigned_usid;  /*! RFFE Programmable Unique Slave ID*/  
  uint8 gsid;  /*! RFFE Group ID */

  uint16 hw_rev_reg; /*! Stores HW revision register address*/
  uint8 hw_revision;    /*! HW revision*/

  uint16 hw_part_reg; /*! Stores HW part register address*/
  uint8 hw_part;       /*! HW part number*/

  uint16 bias;  /*! PAPM bias value and flag */
  rfcom_band_type_u curr_band;

  boolean trigger_write_flag;
  int16 non_trigger_script_timing;
  boolean standby_delay_flag;

  /*! PA band_port info for each tech/band/rfm_device */
  uint8 cdma_band_port[RFM_MAX_DEVICES][RFM_CDMA_MAX_BAND];
  uint8 wcdma_band_port[RFM_MAX_DEVICES][RFCOM_NUM_WCDMA_BANDS];
  uint8 lte_band_port[RFM_MAX_DEVICES][RFCOM_NUM_LTE_BANDS];
  uint8 gsm_band_port[RFM_MAX_DEVICES][RFCOM_NUM_GSM_BANDS];
  uint8 tdscdma_band_port[RFM_MAX_DEVICES][RFCOM_NUM_TDSCDMA_BANDS];

  rf_rffe_data_type rffe_script[RFDEVICE_PAPM_MAX_REG_SIZE];

  boolean auto_bypass_disable;

  uint8* efs_data_ptr;
  rfdevice_reg_settings_type* efs_settings_bw_init;
  rfdevice_reg_settings_type* efs_settings_bypass;
  rfdevice_reg_settings_type* efs_settings_apt;
  rfdevice_reg_settings_type* efs_settings_et;
  rfdevice_reg_settings_type* efs_settings_cal_state;

  boolean efs_settings_data_get(rfdevice_papm_cfg_params_type *cfg, rfdevice_reg_settings_type *settings);
  uint8* get_efs_papm_data(void);
  void get_efs_papm_settings(void);
  boolean is_matching_label(uint8* label1, char const* label2);
#ifdef FEATURE_RFA_DEVICE_DEBUG
  #error code not present
#endif /* FEATURE_RFA_DEVICE_DEBUG */                                                 
private: 
  uint8 calculate_apt_vreg_val(uint16 bias);
  void reset_rffe_script(void);
  uint8 get_port(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band);

  boolean prepare_buffer(rfdevice_reg_settings_type *settings, 
						             rf_device_execution_type execution_type, 
						             int16 timing, uint8 *num_trans);

  boolean process_cmd_sequence(uint8 usid, rfdevice_papm_cmd_seq_type *cmd_seq, uint8 *reg_read);

  boolean config(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, 
                 rfcom_band_type_u band, rfdevice_papm_data_req_enum_type papm_state, 
                 rfdevice_papm_bw_enum_type papm_bw, uint16 bias, 
                 rf_device_execution_type execution_type, 
                 rf_buffer_intf *script_buffer, int16 script_timing = 0);

  uint8 get_hw_device_rev(void);
  uint8 get_hw_device_part(void);
  void print_device_info(void);
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*RFDEVICE_PAPM_COMMON_H*/

