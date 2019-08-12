#ifndef RFDEVICE_ASM_INTF_H
#define RFDEVICE_ASM_INTF_H
/*!
   @file
   rfdevice_asm_intf.h

   @brief

*/

/*===========================================================================

Copyright (c) 2012 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_asm_intf.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
01/27/15   dbc   Added rfdevice_asm_set_asm_xsw_port_config() API 
11/19/14   vv    Added rfdevice_reg_dump() API  for debug
09/16/14   sn    ASM set band RFM Device support
06/19/14   aca   ASM therm support
05/05/14   sn    ASM class based API
09/17/13   vs    Added API to modify autogen register settings
07/08/13   yzw   Add ASM support for FTM command FTM_GET_RFFE_DEVICE_INFO
06/04/13   vs    updated the driver interface for asm port status
04/02/13   sn    Cleanup ASM interface
12/10/12   aca   ASM timing, CA support
08/10/12   tks   Added support for set_tx_band_map and set_rx_band_map
07/12/12   sn    Removed command dispatch function
06/21/12   sn    Initial version
==============================================================================*/

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#ifdef __cplusplus
extern "C" {
#endif

  /*==============================================================================
                           Data Definitions
  ==============================================================================*/

  typedef enum
  {
    RFDEVICE_ASM_GET_MAX_SCRIPT_SIZE,
    RFDEVICE_ASM_RX_BAND_PORT_MAPPING,
    RFDEVICE_ASM_TX_BAND_PORT_MAPPING,
    RFDEVICE_ASM_INVALID_CMD
  }
  rfdevice_asm_cmd_type;

  typedef struct
  {
    rfm_device_enum_type rfm_device;
    rfcom_mode_enum_type mode;
    rfcom_band_type_u band;
    rfdevice_asm_rx_tx_type asm_rx_tx;
    rfdevice_asm_cell_type asm_cell;
    rfdevice_asm_trigger_enum_type  trigger_type;
  } rfdevice_asm_cfg_type;
  /*==============================================================================
                           Prototypes
  ==============================================================================*/

  boolean rfdevice_asm_enable
  (
    rfdevice_asm_cfg_type *asm_cfg_p,
    void *device_ptr,
    void *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );

  /* -------------------------------------------------------------------------- */
  boolean rfdevice_asm_disable
  (
    rfdevice_asm_cfg_type *asm_cfg_p,
    void *device_ptr,
    void *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );

  /* -------------------------------------------------------------------------- */
  boolean rfdevice_asm_sleep
  (
    rfdevice_asm_cfg_type *asm_cfg_p,
    void *device_ptr,
    void *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );

  /* -------------------------------------------------------------------------- */
  boolean rfdevice_asm_wakeup
  (
    rfdevice_asm_cfg_type *asm_cfg_p,
    void *device_ptr,
    void *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );

  /* -------------------------------------------------------------------------- */
  boolean rfdevice_asm_trigger
  (
    rfdevice_asm_cfg_type *asm_cfg_p,
    void *device_ptr,
    void *buff_obj_ptr,
    rf_device_execution_type dev_action,
    int16 script_timing
  );

  /* -------------------------------------------------------------------------- */
  uint16 rfdevice_asm_get_script_size
  (
    void *device_ptr
  );

  /* -------------------------------------------------------------------------- */
  boolean rfdevice_asm_set_tx_band_map
  (
    void *device_ptr, rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, 
	rfcom_band_type_u band,
    int32 *data_ptr, uint8 size
  );

  /* -------------------------------------------------------------------------- */
  boolean rfdevice_asm_set_rx_band_map
  (
    void *device_ptr, rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, 
	rfcom_band_type_u band,
    int32 *data_ptr, uint8 size
  );

  /* -------------------------------------------------------------------------- */
  boolean rfdevice_asm_set_asm_xsw_port_config
  (
    void *device_ptr,
    rfm_device_enum_type rfm_device,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_asm_set_switch_state
  (
    void *device_ptr,
    uint8 switch_state
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_asm_get_switch_state
  (
    void *device_ptr,
    uint8 *switch_state
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_asm_get_device_info
  (
    void *device_ptr, uint16 *mfg_id, uint16 *prd_id, uint16 *rev_id
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_asm_set_rx_switch_state
  (
    void *device_ptr,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    uint8 switch_state_set
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_asm_set_tx_switch_state
  (
    void *device_ptr,
    rfcom_mode_enum_type mode,
    rfcom_band_type_u band,
    uint8 switch_state_set
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_asm_therm_on_off
  (
      void *device_ptr, boolean on_off
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_get_asm_temperature
  (
      void *device_ptr, int16 *temperature
  );

  boolean rfdevice_asm_modify_ag_setting
  (
    void *device_ptr,
    rfdevice_asm_mod_cfg_type *asm_reg_cfg,
    rfdevice_mod_reg_info_type *asm_reg_data
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_asm_dump_reg
  (
    void *device_ptr
  );

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_PA_INTF_H */


