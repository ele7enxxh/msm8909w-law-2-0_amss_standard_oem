#ifndef RFCOMMON_CORE_UTILS_H
#define RFCOMMON_CORE_UTILS_H
/*!
  @file
  rfcommon_core_utils.h

  @brief
  This file contains all type declarations for the RFCOMMON Utilities module.
*/

/*=============================================================================
Copyright (c) 2013 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core_utils.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/27/15   zhh    FBRX ILPC optimize
10/08/14   ka      Add common api to return device pointer based on tech and band 
10/02/14   ck      Added an utility api to convert rfcom band type to sys band type 
07/22/14   tks     Added api to convert sys band to rfcom band
05/29/14   pl      Add conversion utility from tech bands to rfc_bands
04/05/13   gvn     FTM IRAT Timing packet support 
02/08/13   cri     Initial version of common core utilities

============================================================================*/

#include "rfcom.h"
#include "rfm_device_types.h"
#include "ftm_common_log.h"
#include "rfdevice_cmn_intf.h"

#ifdef __cplusplus
   extern "C"{
#endif

#define MAX_TECH_STRING_SIZE  10

/*----------------------------------------------------------------------------*/
uint8 rfcommon_core_get_tx_chain_from_device( rfm_device_enum_type device );

/*----------------------------------------------------------------------------*/
boolean rfcommon_core_get_tech_str( rfm_mode_enum_type mode , char str[10] );
/*----------------------------------------------------------------------------*/

void rfcommon_core_log_irat_timing_data ( uint16 num_trx_trans, uint16 num_non_trx_trans, uint16 estimated_exe_time );
/*----------------------------------------------------------------------------*/
rf_card_band_type rfcommon_core_convert_wcdma_band_to_rfc_band
(
  rfcom_wcdma_band_type band
);

/*----------------------------------------------------------------------------*/
rf_card_band_type rfcommon_core_convert_gsm_band_to_rfc_band
(
  rfcom_gsm_band_type band
);

/*----------------------------------------------------------------------------*/
rf_card_band_type rfcommon_core_convert_tds_band_to_rfc_band
(
  rfcom_tdscdma_band_type band
);

/*----------------------------------------------------------------------------*/
int rfcommon_core_convert_sys_band_to_rfcom_band
(
  sys_band_class_e_type band
); 

sys_band_class_e_type rfcommon_core_convert_rfcom_band_to_sys_band(rfm_mode_enum_type tech, rfcom_band_type_u band);

/*----------------------------------------------------------------------------*/
rfdevice_rxtx_common_class* rfcommon_core_get_tx_device_ptr
(
  rfm_device_enum_type device, 
  rfm_mode_enum_type tech, 
  rfcom_band_type_u band
);

/*----------------------------------------------------------------------------*/
uint32
rfcommon_core_get_wtr_instance_from_device
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rfm_mode_enum_type current_mode
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_CORE_UTILS_H */
