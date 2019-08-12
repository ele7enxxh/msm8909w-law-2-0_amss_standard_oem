#ifndef RFDEVICE_QPA_4G_4G_H
#define RFDEVICE_QPA_4G_4G_H


/*===========================================================================

Copyright (c) 2014 - 2016 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qpa/api/rfdevice_qpa_4g.h#1 $
  $DateTime: 2016/12/13 07:59:29 $
  $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
05/09/16   dbc   Added GSM support for EFS gain & bias scripts
12/02/15   dbc   Added support for QPA/QASM common tab voting
12/01/15   dbc   Add 16 Bit ICQ support
02/02/15   px    Added EFS debugging support
11/07/14   sn    Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfc_common.h"
#include "rfdevice_pa.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_qpa_4g_typedef.h"

#define MAX_PATH_SIZE 100

class rfdevice_qpa_4g: public rfdevice_pa
{
public:

  rfdevice_qpa_4g(void);

  /* Overloaded constructor to push the device information */
  rfdevice_qpa_4g(
    rfdevice_physical_device* phy_obj_ptr,
    rfc_phy_device_info_type *phy_device_info,
    rfc_logical_device_info_type *logical_device_info,
    rfdevice_qpa_4g_settings_type *qpa_settings,
    rfdevice_qpa_4g_func_tbl_type *qpa_fn_ptrs
  );

  virtual ~rfdevice_qpa_4g();

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



protected:

  void init();

  rfdevice_qpa_4g *rfdevice_qpa_4g_p;

  /*To store the parent physical device object*/
  rfdevice_physical_device* parent_phy_obj_ptr;

  rfdevice_qpa_4g_settings_type qpa_settings;

  rfc_device_cfg_info_type qpa_device_info;

  rfdevice_qpa_4g_ag_cfg_type qpa_device_cfg;

  rfdevice_qpa_4g_port_info_type port_mapping;

  rfdevice_qpa_4g_func_tbl_type qpa_fn_ptrs;

  rfdevice_qpa_4g_settings_debug_type qpa_debug_struct;

  boolean rfdevice_qpa_4g_get_port_settings
  (
    rfdevice_qpa_4g* qpa_device,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_pa_bw_enum_type bw,
    uint8* port_index,
    rfdevice_qpa_4g_port_settings_type** tech_settings
  );

  boolean rfdevice_qpa_4g_get_tech_port_info
  (
    rfdevice_qpa_4g* qpa_device,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    uint8** port_data_ptr
  );

  boolean rfdevice_qpa_4g_process_ag_table
  (
    rfdevice_qpa_4g* qpa_device,
    rf_buffer_intf *rf_buff,
    const rfdevice_qpa_4g_reg_data_type* reg_table,
    const uint8* data_table,
    uint8 write_size,
    rf_device_execution_type dev_action,
    int16 script_timing
  );

    boolean rfdevice_qpa_4g_process_ag_gain_table
  (
    rfdevice_qpa_4g* qpa_device,
    rf_buffer_intf *rf_buff,
    const rfdevice_qpa_4g_gain_reg_data_type* reg_table,
    const uint8* data_table,
    uint8 write_size,
    rf_device_execution_type dev_action,
    int16 script_timing,
    uint8 gain_range,
    uint8 num_gains,
    boolean icq_flag,
    uint16 pa_icq_16		
  );
  
  boolean rfdevice_qpa_4g_param_validity
  (
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_pa_bw_enum_type bw,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );

  
  boolean rfdevice_qpa_4g_construct_driver_ag
  (
    rfc_device_cfg_info_type* cfg,
    rfdevice_qpa_4g* qpa_device
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
    uint16 pa_icq_16,
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

boolean common_init
(
  rfdevice_pa_cfg* pa_params_p,
  rf_buffer_intf *buff_obj_ptr,
  rf_device_execution_type dev_action,
  int16 script_timing
);

  // Common Tab Methods
  boolean common_power_up_init
  (
    rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );
  
  boolean common_wakeup
  (
    rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );
  
  boolean common_sleep
  (
    rfdevice_pa_cfg* pa_params_p,
    rf_buffer_intf *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );

  private:
  uint8* efs_data_ptr;
  uint8* get_efs_qpa_data(void);
  boolean get_efs_qpa_settings(void);
  rfdevice_qpa_4g_efs_settings_type* efs_qpa_config;
  rfdevice_qpa_4g_efs_settings_type* efs_qpa_cal_state;
  rfdevice_qpa_4g_efs_gain_settings_type* efs_qpa_gain_range;
  rfdevice_qpa_4g_efs_gain_settings_type* efs_qpa_bias_range;
  rfdevice_qpa_4g_efs_gain_settings_type* efs_qpa_gmsk_gain_range;
  rfdevice_qpa_4g_efs_gain_settings_type* efs_qpa_gmsk_bias_range;
  rfdevice_qpa_4g_efs_gain_settings_type* efs_qpa_8psk_gain_range;
  rfdevice_qpa_4g_efs_gain_settings_type* efs_qpa_8psk_bias_range;
  boolean is_matching_label(uint8* label1, char const* label2);
  boolean is_using_efs;
};

void rfdevice_qpa_4g_calc_icq(uint8 gain_state, uint8 icq_override, uint8 icq_default, uint8 *ret_val);

void rfdevice_qpa_4g_calc_rtune(uint8 bus_id, uint8 slave_id);

void rfdevice_qpa_4g_therm_setup(uint8 bus_id, uint8 slave_id);

void rfdevice_qpa_4g_read_therm(uint8 bus_id, uint8 slave_id,int16* therm_degrees_C);

void rfdevice_qpa_4g_therm_cleanup(uint8 bus_id, uint8 slave_id);

uint8 rfdevice_qpa_4g_convert_bw(rfdevice_pa_bw_enum_type bw,  rfcom_mode_enum_type mode);

uint8 rfdevice_qpa_4g_get_efs_data_column(uint8 port_index, rfdevice_qpa_4g_tx_bw_data_type lcl_bw);

extern "C"
{
  boolean rfdevice_qpa_4g_rffe_in_single
  (
    uint8 bus_id,
    uint8 slave_id,
    uint16 reg,
    uint8 *value
  );

  boolean rfdevice_qpa_4g_rffe_out_single
  (
    uint8 bus_id,
    uint8 slave_id,
    uint16 reg,
    uint8 value
  );
}

#endif /* RFDEVICE_QPA_4G_H */
