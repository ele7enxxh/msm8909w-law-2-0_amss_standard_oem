#ifndef RF_CDMA_LNA_H
#define RF_CDMA_LNA_H

/*!
  @file
  rf_cdma_lna.h

  @details
  This file exports the definitions and declartions pertaining to Rx LNA.
*/

/*==============================================================================

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_lna.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/03/11   sty     Added rf_cdma_check_rise_fall_values()
07/21/11   cd      - Added gain offset parameter for OLTA switchpoint support
                   - Added APIs to query timer hysteresis data
06/17/11   cd      Provided get APIs to retrieve switchpoint rise/fall data
06/02/11   cd      Modified switchpoint configuration
03/02/11   aro     Re-structured the RxAGC Param type
02/17/11   aro     Moved LNA Gain Step definition to RFM
01/31/11   aro     [1] Deleted unused functions
                   [2] Added RxAGC Min Macro
12/29/10   aro     Renamed definitions
12/14/10   aro     Removed common LNA override function
12/13/10   aro     Added function to set LNA State
12/13/10   aro     Added LNA Config Static Initializer
12/09/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#include "rfm_gain_state_types.h"
#include "rfm_cdma_power_mode_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! This macro will provide the calculation for the AGC MIN value that 
corresponds to the given LNA offset value.  The value return can be loaded 
directly into the RX_AGC_VALUE_MIN register corresponding to the LNA offset. */
#define RF_CDMA_GET_RX_AGC_MIN_VAL( lna_offset ) \
  (int8)((MIN((lna_offset - 510)/4, 0) + 0x7F) & 0x7F )

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of LNA Switchpoint Configuration Mode
*/
typedef enum
{
  RF_CDMA_LNA_SWITCH_PT_MODE_OLTA,
  /*!< OLTA Algorithm based LNA Switchpoints */

  RF_CDMA_LNA_SWITCH_PT_MODE_FIXED,
  /*!< Fixed LNA Switchpoints */

  RF_CDMA_LNA_SWITCH_PT_MODE_NONE
  /*!< No LNA Switchpoints */

} rf_cdma_lna_sp_mode_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of LNA Switchpoint Active Status

  @details
  LNA switchpoints can be set to active or inactive for a given linearity 
  state.
*/
typedef enum
{
  RF_CDMA_LNA_SWITCH_PT_INACTIVE,
  /*!< LNA Switchpoints are inactive */

  RF_CDMA_LNA_SWITCH_PT_ACTIVE,
  /*!< LNA Switchpoints are active */

} rf_cdma_lna_sp_active_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold LNA Switch Point Configuration
*/
typedef struct
{

  rf_cdma_lna_sp_mode_type sp_mode;
  /*!< LNA Switchpoint configuration mode */

  union
  {
    struct rf_cdma_lna_switch_pt_fixed_config
    {
      int16 fall; /*!< Lna Fall Value */
      int16 rise; /*!< Lna Rise Value */
    } fixed;

    struct rf_cdma_lna_switch_pt_olta_config
    {
      int16 hysteresis; /*!< Lna Hysterisis */
      int16 backoff; /*!< Lna Backoff */
    } olta;

  } sp_data; /*!< LNA Switchpoint Config Data */

  int16 post_fall_hold;
  /*!< Post-fall Hold Timer for time hysteresis */

  int16 post_rise_hold;
  /*!< Post-rise Hold Timer for time hysteresis */

} rf_cdma_lna_sp_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold LNA Linearity Info
*/
typedef struct
{
  rf_cdma_lna_sp_active_type lin_mode_active;
  /*!< Active/inactive state for current linearity */

  rf_cdma_lna_sp_info_type sp_info[RFM_LNA_GAIN_STEP_NUM];
  /*!< LNA Swutch Point Info */

} rf_cdma_lna_lin_sp_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold LNA switchpoint Info for all Linearity Modes
*/
typedef struct
{

  rf_cdma_lna_lin_sp_info_type lin_info[RFM_CDMA_POWER_MODE_MAX];
  /*!< LNA Linearity Info */

} rf_cdma_lna_info_type;

/*----------------------------------------------------------------------------*/
int16
rf_cdma_lna_get_rise_swpt
(
  const rf_cdma_lna_info_type* lna_info,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_step_type gain_step,
  int16 gain_offset_val
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_lna_get_fall_swpt
(
  const rf_cdma_lna_info_type* lna_info,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_step_type gain_step,
  int16 gain_offset_val
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_lna_get_post_rise_hold
(
  const rf_cdma_lna_info_type* lna_info,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_step_type gain_step
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_lna_get_post_fall_hold
(
  const rf_cdma_lna_info_type* lna_info,
  rfm_cdma_power_mode_type power_mode,
  rfm_lna_gain_step_type gain_step
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_check_rise_fall_values
(
  int16 rise_values[RFM_LNA_GAIN_STEP_NUM],
  int16 fall_values[RFM_LNA_GAIN_STEP_NUM]
);

#ifdef __cplusplus
}
#endif

#endif  /* RF_CDMA_LNA_H */

