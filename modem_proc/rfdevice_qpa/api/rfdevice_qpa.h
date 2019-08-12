#ifndef RFDEVICE_QPA_H
#define RFDEVICE_QPA_H


/*===========================================================================

Copyright (c) 2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qpa/api/rfdevice_qpa.h#1 $
  $DateTime: 2016/12/13 07:59:29 $
  $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
11/19/14   vv    Added rfdevice_reg_dump() API  for debug
09/22/14   dbc   Add gain_state param to qpa_calc_icq
09/11/14   px    Added EFS support for all QPA device specific scripts
08/27/14   vv    Update temperature read from int8 to int16
08/20/14   vv    Remove unused old constructor
08/07/14   vv    Added support to decouple qpa factory to chip specific drivers 
07/18/14   vv    Added therm support
06/11/14   sn    Added Rtune and Ctune support
05/05/14   vv    Physical device creation support
09/13/13   aca   Physical device support
08/05/13   aca   Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfc_common.h"
#include "rfdevice_pa.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_qpa_typedef.h"

#define MAX_PATH_SIZE 100

class rfdevice_qpa: public rfdevice_pa
{
public:

  rfdevice_qpa(void);

  /* Overloaded constructor to push the device information */
  rfdevice_qpa(
    rfdevice_physical_device* phy_obj_ptr,
    rfc_phy_device_info_type *phy_device_info,
    rfc_logical_device_info_type *logical_device_info,
    rfdevice_qpa_settings_type *qpa_settings,
    rfdevice_qpa_func_tbl_type *qpa_fn_ptrs
  );

  virtual ~rfdevice_qpa();

  boolean config
  (
    rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

  boolean set_pa_txagc(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer, rf_device_execution_type dev_action,
                       int16 script_timing);

  boolean set_pa_on_off(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer, rf_device_execution_type dev_action,
                        int16 script_timing);

  boolean set_pa_trigger(rfdevice_pa_cfg* pa_params_p, rf_buffer_intf *script_buffer, rf_device_execution_type dev_action,
                         int16 script_timing);

  boolean get_pa_info();

  boolean sleep(rfdevice_pa_cfg* pa_params_p,
                rf_buffer_intf *buff_obj_ptr,
                rf_device_execution_type dev_action,
                int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean wakeup( rfdevice_pa_cfg* pa_params_p,
                  rf_buffer_intf *buff_obj_ptr,
                  rf_device_execution_type dev_action,
                  int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);


  boolean set_band_map(rfcom_mode_enum_type mode,
                       rfcom_band_type_u band,
                       int32 *data_ptr,
                       uint8 size);


  boolean pa_therm_on_off(boolean turn_on_off);

  boolean get_pa_temperature(int16* temperature);

  boolean debug(boolean enable);

  uint16 get_script_size(void);
#ifdef FEATURE_RFA_DEVICE_DEBUG
  #error code not present
#endif /* FEATURE_RFA_DEVICE_DEBUG */
protected:

  void init();

  rfdevice_qpa *rfdevice_qpa_p;

  /*To store the parent physical device object*/
  rfdevice_physical_device* parent_phy_obj_ptr;

  rfdevice_qpa_settings_type qpa_settings;

  rfc_device_cfg_info_type qpa_device_info;

  rfdevice_qpa_ag_cfg_type qpa_device_cfg;

  rfdevice_qpa_port_info_type port_mapping;

  rfdevice_qpa_func_tbl_type qpa_fn_ptrs;

  rfdevice_qpa_settings_debug_type qpa_debug_struct;

  boolean rfdevice_qpa_get_port_settings
  (
    rfdevice_qpa* qpa_device,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_pa_bw_enum_type bw,
    uint8* port_index,
    rfdevice_qpa_port_settings_type** tech_settings
  );

  boolean rfdevice_qpa_get_tech_port_info
  (
    rfdevice_qpa* qpa_device,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    uint8** port_data_ptr
  );

  boolean rfdevice_qpa_process_ag_table
  (
    rfdevice_qpa* qpa_device,
    rf_buffer_intf *rf_buff,
    const uint8* reg_table,
    const uint8* data_table,
    uint8 write_size,
    rf_device_execution_type dev_action,
    int16 script_timing,
    uint8 port_index = 0
  );

  boolean rfdevice_qpa_param_validity
  (
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_pa_bw_enum_type bw,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );

  boolean rfdevice_qpa_write_efs(rfdevice_qpa* qpa_device);

  boolean rfdevice_qpa_construct_driver_ag
  (
    rfc_device_cfg_info_type* cfg,
    rfdevice_qpa* qpa_device
  );

  boolean on_off
  (
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    boolean on_off,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );


  boolean set_gain_range
  (
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_pa_bw_enum_type bw,
    uint16 pa_gain_range,
    boolean mod_type,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

  boolean set_current_bias
  (
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_pa_bw_enum_type bw,
    uint8 pa_gain_range,
    uint8 pa_icq_pdm,
    boolean mod_type,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

  boolean trigger
  (
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

#ifdef FEATURE_RFA_DEVICE_DEBUG
  #error code not present
#endif /*FEATURE_RFA_DEVICE_DEBUG*/ 
  private:
  uint8 vth_code;
  uint8* efs_data_ptr;
  rfdevice_qpa_efs_settings_type* efs_settings;
  rfdevice_qpa_efs_settings_type* efs_settings_config;
  rfdevice_qpa_efs_settings_type* efs_settings_pa_on;
  rfdevice_qpa_efs_settings_type* efs_settings_pa_off;
  rfdevice_qpa_efs_settings_type* efs_settings_cal_state;
  rfdevice_qpa_efs_settings_type* efs_settings_bias_range[RFDEVICE_QPA_BIAS_RANGE_MAX + 1]; 
  rfdevice_qpa_efs_settings_type* efs_settings_gain_range[RFDEVICE_QPA_RANGE_MAX + 1];
  rfdevice_qpa_efs_settings_type* efs_settings_gain_range_gmsk[RFDEVICE_QPA_RANGE_MAX + 1];
  rfdevice_qpa_efs_settings_type* efs_settings_gain_range_8psk[RFDEVICE_QPA_RANGE_MAX + 1];

  void get_efs_qpa_data(void);
  rfdevice_qpa_efs_settings_type* get_efs_qpa_settings(char const* script_label, uint8 gain=0xFF);
};

void qpa_calc_icq(uint8 gain_state, uint8 icq_override, uint8 icq_default, uint8 *ret_val);

void qpa_calc_rtune(uint8 bus_id, uint8 slave_id);

void qpa_calc_ctune(uint8 *Ctune_idx, uint8 bus_id, uint8 slave_id);

void qpa_vth_tune(uint8 bus_id, uint8 slave_id, uint8 *vth_code);

void qpa_vth_write(uint8 bus_id, uint8 slave_id, uint8 vth_code);

void qpa_port_map(rfcom_band_type_u band, rfdevice_pa_bw_enum_type bw, rfm_mode_enum_type mode, uint8 *port_index);

void qpa_therm_setup(uint8 bus_id, uint8 slave_id);

void qpa_read_therm(uint8 bus_id, uint8 slave_id,int16* therm_degrees_C);

void qpa_therm_cleanup(uint8 bus_id, uint8 slave_id);



extern "C"
{
  boolean rfdevice_qpa_rffe_in_single
  (
    uint8 bus_id,
    uint8 slave_id,
    uint16 reg,
    uint8 *value
  );

  boolean rfdevice_qpa_rffe_out_single
  (
    uint8 bus_id,
    uint8 slave_id,
    uint16 reg,
    uint8 value
  );
}

#endif /* RFDEVICE_QPA_H */
