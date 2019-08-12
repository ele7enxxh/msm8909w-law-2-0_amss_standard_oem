#ifndef RFDEVICE_TDSCDMA_ASM_INTF_H
#define RFDEVICE_TDSCDMA_ASM_INTF_H
/*!
   @file
   rfdevice_tdscdma_asm_intf.h

   @brief

*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_tdscdma_asm_intf.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------- 
03/19/15   al     KW error fix - Add array length as an input parameter to get ASM info  
12/02/14   jyu    Fixed timing error for RX-TX transition when Rx and 
                  Tx reference time are too close
10/03/14   jyu    Fixed bug for cascading ASMs
09/16/14   sn     remove unused API
07/22/14   jyu    Fixed the bug for X2T IRAT with ASM non-trigger/trigger writes
06/04/14   jyu    Added support to separate non-trigger and trigger writes
05/05/14   sn     ASM class based API
08/05/13   al     Added support to set ASM switch state
07/08/13   yzw    Add ASM support for FTM command FTM_GET_RFFE_DEVICE_INFO
05/16/13   jyu    Added API to disable rx and tx ASM
04/30/13   jyu    Added API to allow update ASM band port information
04/24/13   jyu    Added support to allow pass multiple script buffers for script
                  building
04/02/13   sn     Cleanup ASM interface
03/11/13   sar    Updated file for APQ, GNSS only target.
12/21/12   jyu    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_TDSCDMA

#include "rfcom.h"
#include "rfdevice_asm_intf.h"
#include "rfdevice_tdscdma_type_defs.h"
#include "rf_buffer_intf.h"
#ifdef __cplusplus
extern "C" {
#endif


  /*==============================================================================
                           Data Definitions
  ==============================================================================*/

  /*==============================================================================
                           Function Prototypes
  ==============================================================================*/

typedef enum
{
  TDS_ASM_NO_TRIGGER,
  TDS_ASM_TRIGGER_ONLY,
  TDS_ASM_INCLUDE_ALL,
  TDS_ASM_INCLUDE_ALL_FOR_IRAT,
  TDS_ASM_NO_ACTION
} rfdevice_tdscdma_asm_trigger_action_e;

  boolean rfdevice_tdscdma_use_asm
  (
    rfm_device_enum_type rfm_device,
    rfcom_tdscdma_band_type rf_band
  );
  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_enable_rx
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing,
    boolean use_single_script_buf,
    rf_buffer_intf** script_array,
    rfdevice_tdscdma_asm_trigger_action_e action
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_enable_tx
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing,
    boolean use_single_script_buf,
    rf_buffer_intf** script_array,
    rfdevice_tdscdma_asm_trigger_action_e action
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_disable_rx
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing,
    boolean use_single_script_buf,
    rf_buffer_intf** script_array,
    rfdevice_tdscdma_asm_trigger_action_e action,
    boolean* asm_config_needed
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_disable_tx
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing,
    boolean use_single_script_buf,
    rf_buffer_intf** script_array,
    rfdevice_tdscdma_asm_trigger_action_e action,
    boolean* asm_config_needed
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_tx_rx_control
  (
    rfm_device_enum_type pri_device,
    rfm_device_enum_type sec_device,
    rfm_device_enum_type tx_device,
    boolean div_en,
    rfcom_tdscdma_band_type rf_band,
    rf_buffer_intf* script,
    rf_device_execution_type execution_type,
    int16 pri_script_timing,
    int16 sec_script_timing,
    int16 tx_script_timing,
    boolean use_single_script_buf,
    rf_buffer_intf** script_array,
    rfdevice_tdscdma_asm_trigger_action_e action
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_rx_tx_control
  (
    rfm_device_enum_type pri_device,
    rfm_device_enum_type sec_device,
    rfm_device_enum_type tx_device,
    boolean div_en,
    rfcom_tdscdma_band_type rx_band,
    rfcom_tdscdma_band_type tx_band,
    rf_buffer_intf* script,
    rf_device_execution_type execution_type,
    int16 pri_script_timing,
    int16 sec_script_timing,
    int16 tx_script_timing,
    boolean use_single_script_buf,
    rf_buffer_intf** script_array,
    rfdevice_tdscdma_asm_trigger_action_e rx_action,
    rfdevice_tdscdma_asm_trigger_action_e tx_action
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_init
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    void *data,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_sleep_rx
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_wakeup_rx
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );
  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_sleep_tx
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_wakeup_tx
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );
  /*----------------------------------------------------------------------------*/
  uint16 rfdevice_tdscdma_asm_get_script_size
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band
  );

  /*----------------------------------------------------------------------------*/

  boolean rfdevice_tdscdma_asm_get_device_info
  (
    rfm_device_enum_type rfm_device,
    rfcom_tdscdma_band_type rf_band,
    uint16 *mfg_id,
    uint16 *prd_id,
    uint16 *rev_id,
	uint16 array_len
  );


  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_set_rx_switch_state
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    uint8 switch_state_set
  );


  /*----------------------------------------------------------------------------*/
  boolean rfdevice_tdscdma_asm_set_tx_switch_state
  (
    rfm_device_enum_type device,
    rfcom_tdscdma_band_type rf_band,
    uint8 switch_state_set
  );



#ifdef __cplusplus
}
#endif
#endif /* FEATURE_TDSCDMA */

#endif /* RFDEVICE_PA_INTF_H */



