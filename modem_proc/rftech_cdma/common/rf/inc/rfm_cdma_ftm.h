#ifndef RFM_CDMA_FTM_H
#define RFM_CDMA_FTM_H

/*!
   @file
   rfm_cdma_ftm.h

   @brief
   RF Driver's CDMA interface file for FTM.
*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rfm_cdma_ftm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/30/14   zhw     Compiler warning fix
09/24/14   pk      Compiler warning fix
08/20/14   ndb     Add Tuner_ID as part of the response in 
                   "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/06/14   fh      Fixed c2k thermistor read tq pair req/release and api 
05/22/14   sty     Added rfm_cdma_ftm_set_antenna()
02/04/14   jmf     rfm_cdma_ftm_config_fb_transceiver updated with support for 
                   configuring fb gain state
04/24/13   cd      Add support for Therm read as part of Tx Meas Opcode
04/12/13   aro     Debug enable interfaces to enable calv3 debug mode
03/19/13   aro     Modularized FB config into transceiver, asm, and rxlm
03/13/13   sn      Added XPT functions
03/09/13   hdz     Added rfm_cdma_ftm_convert_hdet_to_txagc_dbm10()
02/20/13   sty     Added rfm_cdma_ftm_log_search_diversity_device() and 
                   rfm_cdma_ftm_log_search_primary_device()
02/04/13   ndb/bmg Added rfm_cdma_ftm_set_tuner_tune_code_override() 
                   implementation support 
09/27/12   zhw     Remove write_sbi/grfc_via_rfsw flag
08/07/12   hdz     Added rfm_cdma_ftm_get_therm_index_remainder()
05/29/12   zhw     Support for write GRFC via RFSW debug flag
05/19/12   zhw     Support for write SBI via RFSW debug flag
03/21/12   cd      Provide functionality to enable/disable Rx AGC logging
03/01/12   sty     Added RFM_CDMA_CONFIG_ENABLE_SSMA_DEBUG_MSGS
02/06/12   cd      Added interface to setup HDET 
01/23/12   hdz     Added API rfm_cdma_ftm_get_therm_reading()
01/16/12   aro     PA state as an argument to enable ET path function
01/16/12   aro     Added interface to query THERM read
01/13/12   aro     Added support for ET path enable for self calibration
01/05/12   sty     Added RFM_CDMA_CONFIG_ENABLE_SSMA
12/06/11   aro     Added interface to sample and average HDET reads
11/22/11   aro     Added interface to disable iC for debug purpose
10/13/11   aro     Added interface to enable and disable various debug mode for
                   various debug modules
09/21/11   aro     Added interface to report Jammer Status
08/11/11   aro     Added support to query synth status
07/26/11   aro     Added interface to query HDET settling time
07/21/11   aro     Added interfaces to enable and disable the HDET circuit
07/12/11   aro     Added support to query intelliceiver power mode
07/01/11   aro     Added Clear Power Mode override interface for FTM
06/29/11   aro     Renamed intelliceiver function name
06/14/11   aro     Added interface to perform PA Range to PA State Mapping
05/21/11   aro     Added interface to query Logical device parameter
03/07/11   aro     Changed return type for HDET Read
03/04/11   aro     [1] Added AVGA index Query function
                   [2] Added Turnaround Query function
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/09/11   aro     Added HDET Read Interface
02/02/11   aro     Added Set Power Mode Interface
02/01/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "rfc_cdma.h"
#include "rfm_gain_state_types.h"
#include "sys.h"
#include "rf_cdma_utils_freq.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Enumeration of masks pertaining to various CDMA block to configure debug 
    mode */
typedef enum
{

  RFM_CDMA_CONFIG_IC_DEBUG   = 0x1,
  /*!< Mask to indicate intelliceiver debug mode configuration */

  RFM_CDMA_CONFIG_HDET_DEBUG = 0x2,
  /*!< Mask to indicate HDET debug mode configuration */

  RFM_CDMA_CONFIG_PLIM_DEBUG = 0x4,
  /*!< Mask to indicate Power Limiter debug mode configuration */

  RFM_CDMA_CONFIG_ENABLE_SSMA = 0x8,
  /*!< Mask to indicate if SSMA is to be enabled (set to 1) or disabled 
       (set to 0) */

  RFM_CDMA_CONFIG_ENABLE_SSMA_DEBUG_MSGS = 0x10,
  /*!< Mask to indicate if SSMA debug F3s are to be enabled (set to 1) or 
       disabled (set to 0) */

  RFM_CDMA_CONFIG_RXAGC_LOG_DEBUG = 0x20,
  /*!< Mask to indicate if debug mode Rx AGC logging must be enabled */

  RFM_CDMA_CONFIG_CALV3_DEBUG = 0x100
  /*!< Mask to indicate if debug mode for calv3 is to be enabled */

} rfm_cdma_ftm_config_debug_mode_mask_type;

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_power_mode_override
(
  rfm_device_enum_type device,
  rfm_cdma_power_mode_type power_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_clear_power_mode_override
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_configure_ic_operation
(
  rfm_device_enum_type device,
  boolean disable_ic
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_get_min_max_avga_idx
(
  const rfm_device_enum_type device,
  uint8 *min_lut_idx,
  uint8 *max_lut_idx
);

/*----------------------------------------------------------------------------*/
rfm_cdma_power_mode_type
rfm_cdma_ftm_get_power_mode
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rfm_cdma_power_mode_type
rfm_cdma_ftm_get_jammer_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_get_synth_lock_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_setup_hdet
(
  rfm_device_enum_type device,
  rfdevice_cdma_hdet_power_mode_type power_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_enable_hdet
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
uint16
rfm_cdma_ftm_read_hdet
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
uint16
rfm_cdma_ftm_multi_read_hdet
(
  rfm_device_enum_type device,
  uint16 num_of_reads
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_disable_hdet
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
uint32
rfm_cdma_ftm_get_hdet_settling_time
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_get_therm_read
(
  rfm_device_enum_type device,
  uint32 *therm_read,
  const rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_get_therm_read_in_cal
(
  rfm_device_enum_type device,
  uint16 *therm_read
);

/*----------------------------------------------------------------------------*/
int
rfm_cdma_ftm_get_tc
(
  rfm_cdma_band_class_type const band
);

/*----------------------------------------------------------------------------*/
const rfc_cdma_logical_device_params_type*
rfm_cdma_ftm_get_logical_device_params
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rfm_pa_gain_state_type
rfm_cdma_map_pa_range_to_pa_state
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_pa_range_type pa_range
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_configure_debug_mode
(
  rfm_device_enum_type device,
  uint32 debug_mask,
  boolean debug_en
);

/*----------------------------------------------------------------------------*/
uint16
rfm_cdma_ftm_get_therm_reading
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
uint8
rfm_cdma_ftm_get_therm_index_remainder
(
  rfm_device_enum_type device,
  uint8 *remainder,
  uint16 *therm_raw
);

/*----------------------------------------------------------------------------*/
uint32 
rfm_cdma_ftm_set_tuner_tune_code_override
( 
  rfcom_device_enum_type device,
  uint8 override_flag,
  void *data,
  uint8* tuner_nv_ptr,
  uint8* tuner_id_ptr
);
/*----------------------------------------------------------------------------*/
rfm_device_enum_type
rfm_cdma_ftm_log_search_diversity_device
(
  rfm_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
rfm_device_enum_type
rfm_cdma_ftm_log_search_primary_device
(
  rfm_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_cdma_ftm_config_fb_asm
(
  rfm_device_enum_type device_fb,
  rfm_cdma_band_class_type band, 
  boolean et_enable
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_cdma_ftm_config_fb_transceiver
(
  rfm_device_enum_type device, 
  rfm_device_enum_type device_fb, 
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band, 
  uint32 tx_chan,
  uint8 fb_state
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_cdma_ftm_deconfig_fb_transceiver 
(
  rfm_device_enum_type device, 
  rfm_device_enum_type device_fb,
  rfm_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
int16
rfm_cdma_ftm_convert_hdet_to_txagc
(
   uint16 hdet_read,
   rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_obtain_hdet_lock
(
  rfm_device_enum_type device,
  boolean hkadc_lock
);

/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_set_antenna
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
sys_band_class_e_type 
rfm_cdma_rf_band_type_to_sys_band_type
(
  rfm_cdma_band_class_type band
);


/*----------------------------------------------------------------------------*/
boolean 
rfm_cdma_ftm_get_pa_switchpoints
( 
  rfm_device_enum_type      tx_device , 
  rfm_cdma_band_class_type  band , 
  int16                     pa_rise_swpts[] , 
  int16                     pa_fall_swpts[] , 
  int16*                    active_pa_states
) ;


#ifdef __cplusplus
}
#endif

#endif /* RFM_CDMA_FTM_H */

