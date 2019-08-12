#ifndef QFE2340V3P0_ASM_H
#define QFE2340V3P0_ASM_H
/*!
   @file
   qfe2340v3p0_asm.h

   @brief
   QFE2340 ASM device driver header file
*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2340/api/qfe2340v3p0_asm.h#1 $

when       who    what, where, why
--------   ---   ---------------------------------------------------------------
09/16/14   sn     ASM set band RFM Device support
08/18/14   sn     ASM/PA sequence for reg 1
05/12/14   vv     Physical Device support
05/05/14   sn     ASM class based API
10/05/12   tks    Added support for asm enable, disable, enable multi carrier
                  & disable multi carrier functions.
09/12/12   sn     Added function is_band_valid
09/06/12   shb    Added set_rx/tx_band_map
07/20/12   sn     Added time_ref_flag
07/12/12   sn     Removed command_dispatch function
06/21/12   sn     Initial version

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
#include "rfdevice_asm.h"
#include "rfdevice_asm_intf.h"
#include "qfe2340v3p0_typedef_ag.h"
#include "qfe2340v3p0_typedef.h"
#include "rf_buffer_intf.h"
#include "rfc_common.h"
#include "rfdevice_physical_device.h"

  class qfe2340v3p0_asm: public rfdevice_asm
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

    boolean disable
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *buff_obj_ptr,
      rf_device_execution_type dev_action,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );


    boolean enable_rx(rfdevice_asm_cfg *asm_cfg_p,
                      rf_buffer_intf *buff_obj_ptr,
                      rf_device_execution_type dev_action,
                      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

    boolean enable_tx(rfdevice_asm_cfg *asm_cfg_p,
                      rf_buffer_intf *buff_obj_ptr,
                      rf_device_execution_type dev_action,
                      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

    void init ();

    boolean sleep(rf_buffer_intf *buff_obj_ptr,
                  rf_device_execution_type dev_action,
                  int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

    boolean wakeup(rf_buffer_intf *buff_obj_ptr,
                   rf_device_execution_type dev_action,
                   int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

    boolean disable_rx(rfdevice_asm_cfg *asm_cfg_p,
                       rf_buffer_intf *script_buffer,
                       rf_device_execution_type execution_type,
                       int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

    boolean disable_tx(rfdevice_asm_cfg *asm_cfg_p,
                       rf_buffer_intf *script_buffer,
                       rf_device_execution_type execution_type,
                       int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

    uint16 get_script_size();

    boolean set_tx_band_map(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode,
	                        rfcom_band_type_u band,
                            int32 *data_ptr,uint8 size);

    boolean set_rx_band_map(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode,
                            rfcom_band_type_u band, int32 *data_ptr,
                            uint8 size);	
							
    virtual boolean self_test(uint32 mfg_id, uint32 prd_id, uint32 rev_id);

    virtual ~qfe2340v3p0_asm();

    boolean trigger
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *script_buffer,
      rf_device_execution_type execution_type,
      int16 script_timing
    );

    qfe2340v3p0_asm(void);

    /* Overloaded constructor to push the device information */
    qfe2340v3p0_asm(rfc_device_cfg_info_type *device_info);

    /* Overloaded constructor to push the device information */
    qfe2340v3p0_asm(rfdevice_physical_device *phy_dev_obj_ptr,rfc_phy_device_info_type *phy_device_info, rfc_logical_device_info_type *logical_device_info);

  protected:

    boolean set_rx_band_port_mapping(rfcom_mode_enum_type mode,
                                     rfcom_band_type_u band,
                                     uint16 data);

    boolean set_tx_band_port_mapping(rfcom_mode_enum_type mode,
                                     rfcom_band_type_u band,
                                     uint16 data);
    /*physical device pointer to store parent physical device object*/
    rfdevice_physical_device* parent_physical_device_ptr;

    qfe2340v3p0_ltefdd_asm_port_data_type ltefdd_rx_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

    qfe2340v3p0_ltefdd_asm_port_data_type ltefdd_tx_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

    qfe2340v3p0_ltetdd_asm_port_data_type ltetdd_rx_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

    qfe2340v3p0_ltetdd_asm_port_data_type ltetdd_tx_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

    qfe2340v3p0_tdscdma_asm_port_data_type tdscdma_rx_band_port_mapping[RFCOM_NUM_TDSCDMA_BANDS] ;

    qfe2340v3p0_tdscdma_asm_port_data_type tdscdma_tx_band_port_mapping[RFCOM_NUM_TDSCDMA_BANDS] ;

    /*! RFFE Default Unique Slave ID */
    uint8 default_usid;

    uint8* asm_data;

    int16 trigger_timing;

    boolean write_trigger;

    qfe2340v3p0_common_driver_settings_type asm_params;

    boolean asm_rx_first;
  };

#ifdef __cplusplus
}
#endif

#endif
