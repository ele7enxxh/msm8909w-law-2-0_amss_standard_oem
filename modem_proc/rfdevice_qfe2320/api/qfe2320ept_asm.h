#ifndef QFE2320EPT_ASM_H
#define QFE2320EPT_ASM_H
/*!
   @file
   qfe2320ept_asm.h

   @brief
   QFE2320 ASM device driver header file
*/

/*===========================================================================

Copyright (c) 2012 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qfe2320/api/qfe2320ept_asm.h#1 $

when       who    what, where, why
--------   ---   ---------------------------------------------------------------
09/16/14   sn     ASM set band RFM Device support
05/12/14   vv     Physical Device support
05/05/14   sn     ASM class based API
03/20/14   gy     Added QFE2320 EPT support
10/31/13   sn     Added API to support IRAT for QFE2320
07/09/13   sbo    Added support for TDS and LTETDD
04/10/13   sn     Cleanup ASM interface
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
#include "qfe2320ept_typedef_ag.h"
#include "qfe2320ept_typedef.h"
#include "rf_buffer_intf.h"
#include "rfc_common.h"
#include "rfdevice_physical_device.h"

  class qfe2320ept_asm: public rfdevice_asm
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

    void init ();


    boolean disable_rx
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *script_buffer,
      rf_device_execution_type execution_type,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    boolean disable_tx
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *script_buffer,
      rf_device_execution_type execution_type,
      int16 script_timing = RFCOM_INVALID_TIMING_OFFSET
    );

    uint16 get_script_size();

    boolean trigger
    (
      rfdevice_asm_cfg *asm_cfg_p,
      rf_buffer_intf *script_buffer,
      rf_device_execution_type execution_type,
      int16 script_timing
    );

    boolean set_rx_band_map
    (
	  rfm_device_enum_type rfm_device,
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band,
      int32 *data_ptr,
      uint8 size
    );

    boolean set_tx_band_map
    (
	  rfm_device_enum_type rfm_device,
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band,
      int32 *data_ptr,
      uint8 size
    );
	
    virtual boolean self_test
    (
      uint32 mfg_id,
      uint32 prd_id,
      uint32 rev_id
    );

    virtual ~qfe2320ept_asm();

    /* Overloaded constructor to push the device information */
    qfe2320ept_asm(rfc_device_cfg_info_type *device_info, uint8 rev_id = 0);

    /* Overloaded constructor to push the device information */
    qfe2320ept_asm(rfdevice_physical_device *phy_dev_obj_ptr,rfc_phy_device_info_type *phy_device_info, rfc_logical_device_info_type *logical_device_info); 

  protected:

    /*Physical device object ptr to store its parent physical device object*/
    rfdevice_physical_device* parent_physical_device_ptr;

    /*
      boolean write_script(rf_buffer_intf *buff_obj_ptr,
                           const qfe2320ept_ccs_data_type* reg_tbl,
                           const uint8* data_tbl, uint8 size_tbl,
                           int16 timing, boolean time_ref_flag);

      boolean write_to_hw(rf_buffer_intf *buff_obj_ptr,
                          const qfe2320ept_ccs_data_type* reg_tbl,
                          const uint8* data_tbl, uint8 size_tbl);

      boolean is_band_valid(rfcom_mode_enum_type mode,
                            uint8 band_enum, boolean is_tx);
    */

    boolean set_rx_band_port_mapping(rfcom_mode_enum_type mode,
                                     rfcom_band_type_u band,
                                     uint16 data);

    boolean set_tx_band_port_mapping(rfcom_mode_enum_type mode,
                                     rfcom_band_type_u band,
                                     uint16 data);

    qfe2320ept_wcdma_asm_port_data_type wcdma_rx_band_port_mapping[RFCOM_NUM_WCDMA_BANDS];

    qfe2320ept_gsm_asm_port_data_type gsm_rx_band_port_mapping[RFCOM_NUM_GSM_BANDS];

    qfe2320ept_cdma_asm_port_data_type cdma_rx_band_port_mapping[RFM_CDMA_MAX_BAND];

    qfe2320ept_ltefdd_asm_port_data_type ltefdd_rx_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

    qfe2320ept_ltetdd_asm_port_data_type ltetdd_rx_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

    qfe2320ept_tdscdma_asm_port_data_type tdscdma_rx_band_port_mapping[RFCOM_NUM_TDSCDMA_BANDS];

    qfe2320ept_wcdma_asm_port_data_type wcdma_tx_band_port_mapping[RFCOM_NUM_WCDMA_BANDS];

    qfe2320ept_gsm_asm_port_data_type gsm_tx_band_port_mapping[RFCOM_NUM_GSM_BANDS] ;

    qfe2320ept_cdma_asm_port_data_type cdma_tx_band_port_mapping[RFM_CDMA_MAX_BAND];

    qfe2320ept_ltefdd_asm_port_data_type ltefdd_tx_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

    qfe2320ept_ltetdd_asm_port_data_type ltetdd_tx_band_port_mapping[RFCOM_NUM_LTE_BANDS] ;

    qfe2320ept_tdscdma_asm_port_data_type tdscdma_tx_band_port_mapping[RFCOM_NUM_TDSCDMA_BANDS];


    /*! RFFE Default Unique Slave ID */
    uint8 default_usid;

    uint8* asm_data;

    qfe2320ept_common_driver_settings_type asm_params;

    rfcom_mode_enum_type curr_mode;

    boolean common_rx
    (
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band
    );

    boolean common_tx
    (
      rfcom_mode_enum_type mode,
      rfcom_band_type_u band
    );
  };

#ifdef __cplusplus
}
#endif

#endif
