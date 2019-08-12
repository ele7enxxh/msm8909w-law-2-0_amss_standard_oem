#ifndef RFC_CMN_TRANSCEIVER_HDET_ADAPTER_H
#define RFC_CMN_TRANSCEIVER_HDET_ADAPTER_H

/*!
   @file
   rfc_cmn_transceiver_hdet_adapter.h

   @brief
   Declaration of the Transceiver device to Common HDET interface adapter.
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/inc/rfc_cmn_transceiver_hdet_adapter.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/29/13   adk     Read and store temp (K-sensor) values for QFE TDET
03/21/13   ms      Tuner TDET support 
03/13/13   aca     Tuner power characterization
01/22/13   adk     Added support for TDET gated mode
12/06/12   pv      LTE support for HDET CMN class. 
12/06/12   bmg     Initial version.

==============================================================================*/

#include "rfdevice_hdet_cmn.h"
#ifdef FEATURE_CDMA1X
#include "rfc_cdma_transceiver_hdet_adapter.h"
#endif
#include "rfc_wcdma_transceiver_hdet_adapter.h"

#ifdef FEATURE_LTE
#include "rfc_lte_transceiver_hdet_adapter.h"
#endif

#ifdef FEATURE_TDSCDMA
#include "rfc_tdscdma_transceiver_hdet_adapter.h"
#endif

class rfc_cmn_transceiver_hdet_adapter : public rfdevice_hdet_cmn
{
public:
  rfc_cmn_transceiver_hdet_adapter(rfdevice_cmn_type* rfdevice_cmn_ptr);
  ~rfc_cmn_transceiver_hdet_adapter();

  virtual void* get_hdet_tech_object(rfm_mode_enum_type tech);

  virtual void init();

  virtual int16 get_k_comp(void);

  virtual boolean update_k_comp(void);

  virtual boolean set_gain_state(rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action, rfdevice_hdet_gain_state_type hdet_gain_state, uint8 hdet_pid = 0);

  virtual boolean program_k0_val(uint16 k0_val);

  virtual boolean hkadc_set_ref_path(rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action);

  virtual boolean hkadc_set_incident_path(rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action);

  virtual boolean get_ref_val(rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action, uint16* hdet_ref_pwr, boolean trigger_flag);

  virtual boolean get_k_sensor_val(uint16* k_val);

  virtual boolean update_temp_comp_param( rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action);

  virtual boolean update_temp_comp_param_cb( qfe_script_type* temp_comp_data_ptr);

  virtual boolean get_incident_val(rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action, uint16* hdet_inc_pwr, boolean trigger_flag);
  
  virtual boolean get_power(rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action, uint8 hdet_pid, uint16* hdet_inc_pwr);

  virtual boolean get_dc_offset_val(uint16* hdet_inc_dc_offset_val, uint16* hdet_ref_dc_offset_val);

  virtual boolean setup_power_read(rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action, 
    uint8 num_avg,uint16 wait_time);

  virtual boolean do_hdet_dc_cal(void);

  virtual boolean disable_hdet(rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action);

  virtual boolean auto_cal(rf_buffer_intf* buff_obj_ptr, rf_device_execution_type dev_action);


 virtual boolean get_gated_incident_val
 (
  rf_buffer_intf *buff_obj_ptr, 
  rf_device_execution_type dev_action,
  uint16 *hdet_inc_pwr,
  boolean trigger_flag,int delay,int number_of_samples
  );


private:
  #ifdef FEATURE_CDMA1X
  rfc_cdma_transceiver_hdet_adapter* cdma_adapter;
  #endif /* FEATURE_CDMA1X */
  rfc_wcdma_transceiver_hdet_adapter* wcdma_adapter;
  #ifdef FEATURE_LTE
   rfc_lte_transceiver_hdet_adapter* lte_adapter;
  #endif
  
  #ifdef FEATURE_TDSCDMA
  rfc_tdscdma_transceiver_hdet_adapter* tdscdma_adapter;
  #endif
  
  /* No copying of this object is allowed once it is created */
  rfc_cmn_transceiver_hdet_adapter(const rfc_cmn_transceiver_hdet_adapter&) {}
  rfc_cmn_transceiver_hdet_adapter& operator=(const rfc_cmn_transceiver_hdet_adapter& other) { return *this; }
};

#endif /* RFC_CMN_TRANSCEIVER_HDET_ADAPTER_H */

