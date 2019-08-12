
#ifndef RFLTE_CMNDEF_H
#define RFLTE_CMNDEF_H

/*!
   @file
   rflte_cmndef.h

   @brief

*/

/*===========================================================================

Copyright (c) 2010 - 2012 by QUALCOMM Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rflte_cmndef.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
09/20/12   aca     LTE debug log message mask implementation
09/17/12   aca     QPA independent path config support
09/14/12   jf      Qtuner LTE support (initial)
08/30/12   jf      Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"



/*===========================================================================
                           Data Definitions
===========================================================================*/

/* ENUM for different type of PA supported*/
  typedef enum 
  {
	LTE_GRFC_BASED,
  LTE_GRFC_BASED_W_ASM_CNFG,
	LTE_NON_GRFC_BASED,
	LTE_MAX_DEVICE_TYPE
  } rflte_device_type;

/* Structure for qpa support*/
  typedef struct
  {
     rfcom_lte_band_type band;
     rflte_device_type device_type;
     rf_path_enum_type path;
  } rfc_lte_band_device_type;

/*Structure for qtuner support */
  typedef struct
  {
     rfcom_lte_band_type band;
     boolean qtuner_is_in_use;
  } rfc_lte_band_qtuner_type;
/*----------------------------------------------------------------------------*/
typedef enum
{
  QFE_LTE_PA_ON_SETTINGS,
  QFE_LTE_PA_OFF_SETTINGS,
  QFE_LTE_PA_STATE_0_SETTINGS,
  QFE_LTE_PA_STATE_1_SETTINGS,
  QFE_LTE_PA_STATE_2_SETTINGS,
  QFE_LTE_PA_STATE_3_SETTINGS,
  QFE_LTE_PA_SETTINGS_NUM,
  QFE_LTE_PA_SETTINGS_INVALID,
} qfe_lte_pa_settings_type;

/*----------------------------------------------------------------------------*/
/*
typedef enum
{
  RF_LTE_DISABLED_DEBUG_MASK = 0x0,  
  RF_LTE_LEGACY_ENABLE_ALL_MASK = 0x1,  
  RF_LTE_TX_PLL_ERROR_CORRECTION_DEBUG_MASK = 0x2,
  RF_LTE_SMPS_PDM_INTERPOLATE_DEBUG_MASK = 0x4,
  RF_LTE_TEMP_COMP_DEBUG_MASK = 0x8,
  RF_LTE_MTPL_DEBUG_MASK = 0x10,
  RF_LTE_FREQ_COMP_DEBUG_MASK = 0x20,  
  RF_LTE_IRAT_DEBUG_MASK = 0x40
} rf_lte_debug_msg_enums;
*/

/*----------------------------------------------------------------------------*/

typedef struct
{
  uint8 ssbi_buf_size;
  qfe_lte_pa_settings_type pa_settings_type;
  rfdevice_ssbi_write_type* pa_ssbi_buf; 
  rfcom_lte_band_type lte_band;
}qfe_lte_pa_data_type;


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* RFLTE_CMNDEF_H */

