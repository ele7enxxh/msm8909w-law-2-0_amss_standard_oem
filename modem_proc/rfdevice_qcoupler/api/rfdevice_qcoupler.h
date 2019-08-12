#ifndef RFDEVICE_QCOUPLER_H
#define RFDEVICE_QCOUPLER_H


/*===========================================================================

Copyright (c) 2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qcoupler/api/rfdevice_qcoupler.h#1 $
  $DateTime: 2016/12/14 01:58:12 $
  $Author: mplcsds1 $

when       who   what, where, why
--------   ---   --------------------------------------------------------------- 
11/12/15   dbc   Fix indices for RFC device info fields 
10/21/15   dbc   Added RFDEVICE_QCOUPLER_RFC_PORT_NUM_INDEX macro 
09/24/14   px    Added EFS support for all QCoupler device specific scripts
08/21/14   vv    Added support to decouple qcoupler factory to chip specific drivers 
                 Clean up unused device constructor
05/12/14   vv    Physical device support
04/08/14   yb    Coupler driver integration with RFC and AG files
                 Added tech band information to config API
02/24/14   yb    Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#define RFDEVICE_QCOUPLER_MFG_ID_GET(x)   (((x) >> 22) & 0x3FF)
#define RFDEVICE_QCOUPLER_PRD_ID_GET(x)   (((x) >> 14) & 0xFF)
#define RFDEVICE_QCOUPLER_PORT_NUM_GET(x) ((x) & 0x7F)
#define RFDEVICE_QCOUPLER_DIR_GET_DEFAULT(x)      (((x) >> 7) & 0x7F)
#define RFDEVICE_QCOUPLER_RFC_PORT_NUM_INDEX  0
#define RFDEVICE_QCOUPLER_RFC_FWD_ATTN_INDEX  1
#define RFDEVICE_QCOUPLER_RFC_REV_ATTN_INDEX  2
#define RFDEVICE_QCOUPLER_RFC_DEFAULT_FWD_DIRECTION_INDEX  3

#ifdef __cplusplus
extern "C" {
#endif

#include "rfc_common.h"
#include "rfdevice_qcoupler_typedef.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_coupler.h"

#define MAX_PATH_SIZE 100

class rfdevice_qcoupler: public rfdevice_coupler
{
public:

  rfdevice_qcoupler(void);
  rfdevice_qcoupler(
                    rfdevice_physical_device* phy_obj_ptr,
                    rfc_phy_device_info_type *phy_device_info, 
                    rfc_logical_device_info_type *logical_device_info,
                    rfdevice_qcoupler_settings_type *qcoupler_settings,
                    rfdevice_qcoupler_func_tbl_type *qcoupler_fn_ptrs
                    );   virtual ~rfdevice_qcoupler();

  boolean config_coupler
  (
    rfdevice_coupler_tech_cfg_type tech_cfg,
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

  void init(void);

  virtual boolean init 
  (
    int32* rfc_data
  );

  boolean sleep
  (
    int32* rfc_data,
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

  boolean wakeup
  (
    int32* rfc_data,
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

   boolean trigger
  (
    int32* rfc_data,
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action, 
    int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
  );

  boolean set_band_config_map
  (
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    int32 *data_ptr
  );

  
protected:

  /* physical device pointer to store the parent physical device object  during physical device creation */
  rfdevice_physical_device* rfdevice_phy_obj_ptr; 

  rfdevice_qcoupler *rfdevice_qcoupler_p;
  
  rfc_device_cfg_info_type qcoupler_device_info;
  
  rfdevice_qcoupler_settings_type qcoupler_settings;

  rfdevice_qcoupler_ag_cfg_type qcoupler_device_cfg;

  rfdevice_qcoupler_func_tbl_type qcoupler_fn_ptrs;

  rfdevice_qcoupler_settings_debug_type qcoupler_debug_struct;

  rfdevice_qcoupler_rfc_cfg_band_map_type qcoupler_rfc_config_mapping;

  rfdevice_qcoupler_cfg_type qcoupler_cfg;

  /* List of API's used by the driver and not exposed */

  boolean config_coupler_default (void);

  boolean rfdevice_qcoupler_process_ag_table
  (
    rfdevice_qcoupler* qcoupler_device,
    rf_buffer_intf *rf_buff,
    const uint16* reg_table,
    const int16* data_table,
    uint8 write_size,
    rf_device_execution_type dev_action,
    int16 script_timing,
    uint8 port_index = 0
  );
  
  boolean rfdevice_qcoupler_validate
  (
    rf_buffer_intf *buff_obj_ptr, 
    rf_device_execution_type dev_action,
    int16 script_timing   
  );

  boolean rfdevice_qcoupler_construct_driver_ag 
  ( 
    rfc_device_cfg_info_type* cfg,  
    rfdevice_qcoupler* qcoupler_device 
  );

  boolean rfdevice_qcoupler_update_cfg
  (
    rfdevice_qcoupler* qcoupler_device,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_coupling_direction_type direction
  );

  boolean rfdevice_qcoupler_get_band_config_map
  (
    rfdevice_qcoupler* qcoupler_device,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    rfdevice_qcoupler_rfc_cfg_type **rfc_cfg
  );

  private:
    uint8* efs_data_ptr;
    rfdevice_qcoupler_efs_settings_type* efs_settings;
    rfdevice_qcoupler_efs_settings_type* efs_settings_fwd;
    rfdevice_qcoupler_efs_settings_type* efs_settings_rev;
    rfdevice_qcoupler_efs_settings_type* efs_settings_open;
    rfdevice_qcoupler_efs_settings_type* efs_settings_internal;

    boolean trigger_internal
    (
      int32* rfc_data,
      rf_buffer_intf *buff_obj_ptr, 
      rf_device_execution_type dev_action, 
      int16 script_timing
    );

    boolean config_coupler_internal
    (
      rfdevice_coupler_tech_cfg_type tech_cfg,
      rf_buffer_intf *buff_obj_ptr, 
      rf_device_execution_type dev_action, 
      int16 script_timing
    );

    boolean set_band_config_map_internal
    (
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band,
      int32 *data_ptr
    );

    boolean wakeup_internal
    (
      int32* rfc_data,
      rf_buffer_intf *buff_obj_ptr, 
      rf_device_execution_type dev_action, 
      int16 script_timing
    );

    boolean sleep_internal
    (
      int32* rfc_data,
      rf_buffer_intf *buff_obj_ptr, 
      rf_device_execution_type dev_action, 
      int16 script_timing
    );

    void get_efs_qcoupler_data(void);
    rfdevice_qcoupler_efs_settings_type* get_efs_qcoupler_settings(char const* script_label);
};

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_QCOUPLER_H */
