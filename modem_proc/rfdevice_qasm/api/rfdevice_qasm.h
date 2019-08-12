#ifndef RFDEVICE_QASM_H
#define RFDEVICE_QASM_H


/*===========================================================================

Copyright (c) 2013-2015 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qasm/api/rfdevice_qasm.h#1 $
  $DateTime: 2016/12/14 01:58:13 $
  $Author: mplcsds1 $

when         who   what, where, why
--------   ---   ---------------------------------------------------------------
12/23/15   dbc    Voting rework to cover more ASM sharing scenarios
01/27/15   dbc    Add set_asm_xsw_port_config declaration
11/19/14   vv     rfdevice_reg_dump() API  for debug
10/01/14   dbc    Add support for ASM<->XSW interface 
09/17/14   px     Added EFS support for all QASM device specific scripts
09/16/14   sn     ASM set band RFM Device support
08/20/14   vv     Remove unused old constructor
08/07/14   vv     Added support to decouple qasm factory to chip specific drivers
07/29/14   px     Combined trigger support when processing ag table
07/22/14   sn     Added SV voting to handle dual chain scenarios
05/08/14   vv     Physical device support
05/05/14   sn     ASM class based API
09/11/13   aca    updated get_script_size from pure virtual to virtual
08/05/13   aca    Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "rfc_common.h"
#include "rfdevice_asm.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_qasm_typedef.h"
#include "rfdevice_asm_xsw_intf.h"

#define MAX_PATH_SIZE 100

  class rfdevice_qasm: public rfdevice_asm
  {
  public:

    boolean enable
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean sleep
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean wakeup
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean set_tx_band_map(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode,
	                        rfcom_band_type_u band,
                            int32 *data_ptr,uint8 size);

    boolean set_rx_band_map(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode,
                            rfcom_band_type_u band, int32 *data_ptr,
                            uint8 size);	
    	
    boolean set_asm_xsw_port_config
    (
      rfm_device_enum_type rfm_device,
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band
    );
 
    boolean disable
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean trigger
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

#ifdef FEATURE_RFA_DEVICE_DEBUG
   #error code not present
#endif /* FEATURE_RFA_DEVICE_DEBUG */

    virtual ~rfdevice_qasm();


  /* Overloaded constructor for physical device support */
    rfdevice_qasm(
                  rfdevice_physical_device* phy_obj_ptr,
                  rfc_phy_device_info_type *phy_device_info, 
                  rfc_logical_device_info_type *logical_device_info,
                  rfdevice_qasm_settings_type *qasm_settings,
                  rfdevice_qasm_func_tbl_type *qasm_fn_ptrs,
                  rfdevice_asm_xsw_intf *asm_xsw_intf_ptr = NULL
                  ); 

    rfdevice_qasm(void);

    boolean debug(boolean enable);

    /*--------   Pure virtual functions not needed by QASM ----------*/

    uint16 get_script_size(void);

    /*--------  End Pure virtual functions not needed by QASM -------*/


  protected:

    rfdevice_qasm *rfdevice_qasm_p;

   /*To store the parent physical device object*/
   rfdevice_physical_device* rfdevice_phy_obj_ptr;

   rfdevice_asm_xsw_intf* rfdevice_asm_xsw_intf_ptr;
  
    rfdevice_qasm_settings_type qasm_settings;

    rfc_device_cfg_info_type qasm_device_info;

    rfdevice_qasm_ag_cfg_type qasm_device_cfg;

    rfdevice_qasm_port_info_type rx_port_mapping;

    rfdevice_qasm_port_info_type tx_port_mapping;

    rfdevice_qasm_port_info_type xsw_port_mapping;

    rfdevice_qasm_func_tbl_type qasm_fn_ptrs;

    rfdevice_qasm_settings_debug_type qasm_debug_struct;

    boolean enable_rx
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean enable_tx
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean disable_rx
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean disable_tx
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean rfdevice_qasm_get_port_settings
    (
      rfdevice_qasm* qasm_device,
	  rfm_device_enum_type rfm_device,
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band,
      rfdevice_qasm_port_type port_select,
      uint8* port_index,
      rfdevice_qasm_port_settings_type** tech_settings
    );

    boolean rfdevice_qasm_get_tech_port_info
    (
      rfdevice_qasm* qasm_device,
	  rfm_device_enum_type rfm_device,
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band,
      rfdevice_qasm_port_type port_select,
      uint8** port_data_ptr
    );

    boolean rfdevice_qasm_process_ag_table
    (
      rfdevice_qasm* qasm_device,
      rf_buffer_intf *rf_buff,
      const uint16* reg_table,
      const int16* data_table,
      uint8 write_size,
      rf_device_execution_type dev_action,
      int16 script_timing,
      boolean combined_trigger = FALSE,
      uint8 port_index = 0
    );

    void init(void);

    boolean rfdevice_qasm_write_efs(rfdevice_qasm* qasm_device);

    boolean rfdevice_qasm_construct_driver_ag
    (
      rfc_device_cfg_info_type* cfg,
      rfdevice_qasm* qasm_device
    );


    boolean rfdevice_qasm_param_validity
    (
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing
    );

#ifdef FEATURE_RFA_DEVICE_DEBUG	
  #error code not present
#endif /* FEATURE_RFA_DEVICE_DEBUG */

  uint64 rx_tx_vote[RFM_NUM_MODES][2];	
  uint64 sleep_wakeup_vote[RFM_NUM_MODES][2];  
  uint64 lte_ca_vote[RFM_MAX_WAN_DEVICES][RFDEVICE_ASM_RX_TX_MAX];
  uint32 sv_vote[RFM_MAX_WAN_DEVICES];
    
  private:
    uint8* efs_data_ptr;
    rfdevice_qasm_efs_settings_type* efs_settings;
    rfdevice_qasm_efs_settings_type* efs_settings_enable_tx;
    rfdevice_qasm_efs_settings_type* efs_settings_disable_tx;
    rfdevice_qasm_efs_settings_type* efs_settings_enable_rx;
    rfdevice_qasm_efs_settings_type* efs_settings_disable_rx;
    rfdevice_qasm_efs_settings_type* efs_settings_xsw_def_tx; 
    rfdevice_qasm_efs_settings_type* efs_settings_xsw_swap_tx; 
    rfdevice_qasm_efs_settings_type* efs_settings_xsw_def_rx; 
    rfdevice_qasm_efs_settings_type* efs_settings_xsw_swap_rx; 
    rfdevice_qasm_efs_settings_type* efs_settings_xsw_port; 

    void get_efs_qasm_data(void);
    rfdevice_qasm_efs_settings_type* get_efs_qasm_settings(char const* script_label);
  };

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_QASM_H */
