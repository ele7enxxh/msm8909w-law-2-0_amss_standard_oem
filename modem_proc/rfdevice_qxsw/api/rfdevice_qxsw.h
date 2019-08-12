#ifndef RFDEVICE_QXSW_H
#define RFDEVICE_QXSW_H


/*===========================================================================

Copyright (c) 2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qxsw/api/rfdevice_qxsw.h#1 $
  $DateTime: 2016/12/13 07:58:05 $
  $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
01/15/15   dbc   Add critical section lock for all pertinent APIs
10/02/14   dbc   Implement rfdevice_asm_xsw_intf interface for tracking XSW position 
09/24/14   px    Added EFS support for all QXSW device specific scripts
09/19/14   ndb   Remove the un-used interfaces init()
08/20/14   vv    Remove unused old constructor
08/07/14   vv    Added support to decouple qxsw factory to chip specific drivers 
05/12/14   vv    Physical device support
04/01/14   dbc   Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "rfc_common.h"
#include "rfdevice_xsw.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_qxsw_typedef.h"
#include "rfdevice_asm_xsw_intf.h"


#define MAX_PATH_SIZE 100

class rfdevice_qxsw: public rfdevice_xsw
{
public:

  boolean set_cross_switch_config
  (
    rfdevice_xsw_config_type* xsconfig,
    rf_buffer_intf*           buff_obj_ptr, 
    rf_device_execution_type  dev_action, 
    int16 script_timing       = RFCOM_INVALID_TIMING_OFFSET
  );

  boolean get_cross_switch_config
  (
    rfdevice_xsw_config_type* xsconfig
  );

  boolean set_band_map_default
  (
   rfcom_mode_enum_type mode, 
   rfcom_band_type_u band, 
   int32 *data_ptr, 
   uint8 size 
  );

  boolean set_band_map_swap
  (
   rfcom_mode_enum_type mode, 
   rfcom_band_type_u band, 
   int32 *data_ptr, 
   uint8 size 
  );
 
  boolean set_srs_band_map_default
  (
   rfcom_mode_enum_type mode, 
   rfcom_band_type_u band, 
   int32 *data_ptr, 
   uint8 size 
  );

  boolean set_srs_band_map_swap
  (
   rfcom_mode_enum_type mode, 
   rfcom_band_type_u band, 
   int32 *data_ptr, 
   uint8 size 
  );

  boolean init 
  (
    int32* rfc_data
  );


  boolean trigger
  (
    rf_buffer_intf*           buff_obj_ptr, 
    rf_device_execution_type  dev_action, 
    int16 script_timing       = RFCOM_INVALID_TIMING_OFFSET
  );


  virtual ~rfdevice_qxsw();
  

 /* Overloaded constructor to push the device information */
  rfdevice_qxsw(
                  rfdevice_physical_device* phy_obj_ptr,
                  rfc_phy_device_info_type *phy_device_info, 
                  rfc_logical_device_info_type *logical_device_info,
                  rfdevice_qxsw_settings_type *qxsw_settings,
                  rfdevice_qxsw_func_tbl_type *qxsw_fn_ptrs,
                  rfdevice_asm_xsw_intf *asm_xsw_intf_ptr = NULL
               );

  rfdevice_qxsw(void);

  boolean debug(boolean enable);

  /*--------   Pure virtual functions not needed by QXSW ----------*/

  uint16 get_script_size(void);
  
  /*--------  End Pure virtual functions not needed by QXSW -------*/
  
  protected:
  
  rfdevice_qxsw *rfdevice_qxsw_p;
  
  /*To store the parent physical device object*/
  rfdevice_physical_device* parent_phy_obj_ptr;
  
  rfdevice_asm_xsw_intf* rfdevice_asm_xsw_intf_ptr;
  
  rfdevice_qxsw_settings_type qxsw_settings;
  
  rfc_device_cfg_info_type qxsw_device_info;
  
  rfdevice_qxsw_ag_cfg_type qxsw_device_cfg;
  
  rfdevice_qxsw_port_info_type default_band_port_mapping;

  rfdevice_qxsw_port_info_type swap_band_port_mapping;

  uint8 srs_swap_band_port_mapping[RFCOM_NUM_LTE_BANDS];
  uint8 srs_default_band_port_mapping[RFCOM_NUM_LTE_BANDS];

  rfdevice_qxsw_func_tbl_type qxsw_fn_ptrs;

  rfdevice_qxsw_settings_debug_type qxsw_debug_struct;

  boolean rfdevice_qxsw_get_port_settings
  (
    rfdevice_qxsw* qxsw_device,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_xsw_modes_type switch_mode,
    uint8* port_index,
    rfdevice_qxsw_port_settings_type** port_settings 
  );
  
  boolean rfdevice_qxsw_get_tech_port_info
  (
    rfdevice_qxsw* qxsw_device,
    rfcom_mode_enum_type mode, 
    rfcom_band_type_u band,
    rfdevice_xsw_modes_type switch_mode,
    uint8** port_data_ptr
  );
  
  boolean rfdevice_qxsw_process_ag_table
  (
    rfdevice_qxsw* qxsw_device,
    rf_buffer_intf *rf_buff,
    const uint16* reg_table,
    const int16* data_table,
    uint8 write_size,
    rf_device_execution_type dev_action,
    int16 script_timing,
    uint8 port_index = 0
  );
  
  
  boolean rfdevice_qxsw_write_efs(rfdevice_qxsw* qxsw_device);

  boolean rfdevice_qxsw_construct_driver_ag 
  ( 
    rfc_device_cfg_info_type* cfg,  
    rfdevice_qxsw* qxsw_device 
  );
  
  boolean rfdevice_qxsw_param_validity
  (
     rfdevice_xsw_config_type* cfg, 
     rf_buffer_intf *buff_obj_ptr, 
     rf_device_execution_type dev_action,
     int16 script_timing   
  );
  
  private:
    uint8* efs_data_ptr;
    rfdevice_qxsw_efs_settings_type* efs_settings;
    rfdevice_qxsw_efs_settings_type* efs_settings_prx_drx;
    rfdevice_qxsw_efs_settings_type* efs_settings_prx_only;
    rfdevice_qxsw_efs_settings_type* efs_settings_srs_only;
    rfdevice_qxsw_efs_settings_type* efs_settings_prx_drx_tx;

    boolean set_band_map_default_internal
    (
     rfcom_mode_enum_type mode, 
     rfcom_band_type_u band, 
     int32 *data_ptr, 
     uint8 size 
    );

    boolean set_band_map_swap_internal
    (
     rfcom_mode_enum_type mode, 
     rfcom_band_type_u band, 
     int32 *data_ptr, 
     uint8 size 
    );

    boolean set_srs_band_map_default_internal
    (
     rfcom_mode_enum_type mode, 
     rfcom_band_type_u band, 
     int32 *data_ptr, 
     uint8 size 
    );

    boolean set_srs_band_map_swap_internal
    (
     rfcom_mode_enum_type mode, 
     rfcom_band_type_u band, 
     int32 *data_ptr, 
     uint8 size 
    );

    boolean set_cross_switch_config_internal
    (
      rfdevice_xsw_config_type* xsconfig,
      rf_buffer_intf*           buff_obj_ptr, 
      rf_device_execution_type  dev_action, 
      int16 script_timing
    );

    boolean trigger_internal
    (
      rf_buffer_intf*           buff_obj_ptr, 
      rf_device_execution_type  dev_action, 
      int16 script_timing
    );

    void get_efs_qxsw_data(void);
    rfdevice_qxsw_efs_settings_type* get_efs_qxsw_settings(char const* script_label);
};

void qxsw_convert_port(uint8 input_port, uint8 ant_sel, uint8 *port_to_use);

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_QXSW_H */
