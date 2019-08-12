#ifndef RFDEVICE_LTE_ASM_INTF_H
#define RFDEVICE_LTE_ASM_INTF_H
/*!
   @file
   rfdevice_lte_asm_intf.h

   @brief

*/

/*===========================================================================

Copyright (c) 2012 - 14 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_lte_asm_intf.h#1 $

when         who   what, where, why
--------   ---   ------------------------------------------------------------------- 
03/19/15   al    KW error fix - Add array length as an input parameter
09/16/14   sn    remove unused API
05/05/14   sn    ASM class based API
04/07/14   svi   Added cleanup of ASM during Band Switching
09/20/13   kab   Pass proper CA Mode flag to ASM interface
08/13/13   kg     Added new API for rfdevice_lte_asm_disable_rx
07/18/13   pv     Added support for separate TDD Tx and Rx ASM scripts.
07/10/13   sbm   non trigger script support.
07/08/13   yzw   Add ASM support for FTM command FTM_GET_RFFE_DEVICE_INFO
06/09/13   vs    Port A/B related changes for asm
04/02/13   sn    Cleanup ASM interface
11/20/12   gvn   Featurize LTE for Triton
10/31/12   vss   Change parameters in set band port mappings to pass device instance,
                 instead of rfm device
08/07/12   vss     Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"
#include "rflte_msg.h"
#include "rfdevice_asm_intf.h"
#ifdef FEATURE_LTE
#include "rfdevice_lte_type_defs.h"
#endif /*FEATURE_LTE*/
#ifdef __cplusplus
extern "C" {
#endif
#ifdef FEATURE_LTE


  /*===========================================================================
                           Data Definitions
  ===========================================================================*/

  /*===========================================================================
                           Prototypes
  ===========================================================================*/
  /* ----------------------------------------------------------------------- */
  /* function prototypes */


  boolean rfdevice_lte_asm_enable_rx
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    uint32 carrier_idx,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  boolean rfdevice_lte_asm_enable_rx_no_trigger
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    uint32 carrier_idx,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  boolean rfdevice_lte_asm_enable_tx
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    uint32 carrier_idx,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  boolean rfdevice_lte_asm_enable_tx_no_trigger
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    uint32 carrier_idx,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  boolean rfdevice_lte_asm_init
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    void *data,
    rfdevice_lte_script_data_type* script_data_ptr
  );

  boolean rfdevice_lte_asm_sleep_rx
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  boolean rfdevice_lte_asm_wakeup_rx
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  boolean rfdevice_lte_asm_sleep_tx
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  boolean rfdevice_lte_asm_wakeup_tx
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  uint16 rfdevice_lte_asm_get_script_size
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_lte_asm_set_switch_state
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    uint8 tx_switch_state,
    uint8 rx_switch_state,
    rfa_rf_lte_rx_tx_chain_type rx_tx_chain
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_lte_asm_get_switch_state
  (
    rfm_device_enum_type device,
    rfcom_lte_band_type rf_band,
    uint8 *switch_state
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_lte_asm_get_device_info
  (
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    uint16 *mfg_id,
    uint16 *prd_id,
    uint16 *rev_id,
	uint16 array_len
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_lte_asm_disable_rx
  (
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_lte_asm_disable_rx_no_trigger
  (
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_lte_asm_tx_trigger
  (
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_lte_asm_rx_trigger
  (
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_lte_asm_disable_tx_no_trigger
  (
    rfm_device_enum_type rfm_device,
    rfcom_lte_band_type rf_band,
    rfdevice_lte_script_data_type* script_data_ptr,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/

#endif /*FEATURE_LTE*/

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_PA_INTF_H */


