#ifndef RFDEVICE_WCDMA_ASM_INTF_H
#define RFDEVICE_WCDMA_ASM_INTF_H
/*!
   @file
   rfdevice_wcdma_asm_intf.h

   @brief

*/

/*===========================================================================

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/device/rfdev_intf/inc/rfdevice_wcdma_asm_intf.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------- 
03/19/15   al     KW error fix - Add array length as an input parameter 
05/05/14   sn     ASM class based API
07/08/13   yzw    Add ASM support for FTM command FTM_GET_RFFE_DEVICE_INFO
04/02/13   sn     Cleanup ASM interface
03/11/13   sar    Updated file for APQ, GNSS only target.
11/26/12   tks    Updated asm device interface to support configuration of
                  multiple asm devices
08/12/12   vbh    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_WCDMA
#include "rfcom.h"
#include "rfdevice_asm_intf.h"
#include "rfdevice_wcdma_type_defs.h"
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

  boolean rfdevice_wcdma_asm_enable_rx
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_wcdma_asm_enable_tx
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_wcdma_asm_disable_rx
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_wcdma_asm_disable_tx
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_wcdma_asm_init
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band,
    void *data,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_wcdma_asm_sleep_rx
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_wcdma_asm_sleep_tx
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_wcdma_asm_wakeup_rx
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_wcdma_asm_wakeup_tx
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band,
    rf_buffer_intf *script,
    rf_device_execution_type execution_type,
    int16 script_timing
  );
  /*----------------------------------------------------------------------------*/
  uint16 rfdevice_wcdma_asm_get_script_size
  (
    rfm_device_enum_type device,
    rfcom_wcdma_band_type rf_band
  );

  /*----------------------------------------------------------------------------*/
  boolean rfdevice_wcdma_asm_get_device_info
  (
    rfm_device_enum_type rfm_device,
    rfcom_wcdma_band_type rf_band,
    uint16 *mfg_id,
    uint16 *prd_id,
    uint16 *rev_id,
	uint16 array_len
  );

#ifdef __cplusplus
}
#endif
#endif /* FEATURE_WCDMA */
#endif /* RFDEVICE_PA_INTF_H */



