#ifndef RFM_GAIN_STATE_TYPES_H
#define RFM_GAIN_STATE_TYPES_H

/*!
  @file
  rfm_gain_state_types.h

  @details
  This file exports the definitions and declartions related to gain states.
*/

/*==============================================================================

  Copyright (c) 2010 - 2013  Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_gain_state_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/05/14   cd      Updated number of gain ranges for 8 PA state support
04/23/14   cd      Support for 8 PA state functionality
05/20/13   kai     Added new feedback path attenuation state enum
05/15/13   sty     Fixed typos
09/20/12   zhw     PA Range Enum Max Limit
06/14/11   aro     PA State / PA Range Enum update
06/13/11   aro     Documentation Update
03/15/11   aro     Added PA Gain Step enumeration
03/02/11   aro     Re-structured the RxAGC Param type
02/22/11   sty     Renamed rfm_pa_r1_r0_type to rfm_pa_range_type
02/17/11   sty     Added rfm_pa_r1_r0_type
02/17/11   aro     Moved LNA Gain Step definition to RFM
02/09/11   aro     Added PA Gain State enums
12/29/10   aro     Added Number of Gain State Enum
12/06/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Valid LNA gain states for CDMA. INVALID_RFM_LNA_GAIN_STATE refers to
  invalid LNA gain state.
*/
typedef enum
{

  RFM_LNA_GAIN_STATE_0, /*!< G0, maximum gain */

  RFM_LNA_GAIN_STATE_1, /*!< G1 */

  RFM_LNA_GAIN_STATE_2, /*!< G2 */

  RFM_LNA_GAIN_STATE_3, /*!< G3 */

  RFM_LNA_GAIN_STATE_4, /*!< G4, minimum gain */

  RFM_LNA_GAIN_STATE_NUM, /*!< Number of LNA Gain States */

  RFM_LNA_INVALID_GAIN_STATE /*!< Invalid LNA Gain State */

} rfm_lna_gain_state_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of LNA Gain Steps
*/
typedef enum
{
  RFM_LNA_GAIN_STEP_0_TO_1,
  /*!< Gain step from Gain state 0 to 1 */

  RFM_LNA_GAIN_STEP_1_TO_2,
  /*!< Gain step from Gain state 1 to 2 */

  RFM_LNA_GAIN_STEP_2_TO_3,
  /*!< Gain step from Gain state 2 to 3 */

  RFM_LNA_GAIN_STEP_3_TO_4,
  /*!< Gain step from Gain state 3to 4 */

  RFM_LNA_GAIN_STEP_NUM

  /*!< Maximum number of Gain steps */

} rfm_lna_gain_step_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Valid PA gain states. RFM_PA_GAIN_STATE_NUM refers to
  invalid PA gain state.
*/
typedef enum
{

  RFM_PA_GAIN_STATE_0 = 0, /*!< PA Gain State 0, minimum gain */

  RFM_PA_GAIN_STATE_1 = 1, /*!< PA Gain State 1 */

  RFM_PA_GAIN_STATE_2 = 2, /*!< PA Gain State 2 */

  RFM_PA_GAIN_STATE_3 = 3, /*!< PA Gain State 3 */

  RFM_PA_GAIN_STATE_4 = 4, /*!< PA Gain State 4 */

  RFM_PA_GAIN_STATE_5 = 5, /*!< PA Gain State 5 */

  RFM_PA_GAIN_STATE_6 = 6, /*!< PA Gain State 6 */

  RFM_PA_GAIN_STATE_7 = 7, /*!< PA Gain State 7 */

  RFM_PA_GAIN_STATE_NUM /*!< Number of PA Gain States */

} rfm_pa_gain_state_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Valid PA gain Steps
*/
typedef enum
{

  RFM_PA_GAIN_STEP_0_TO_1, /*!< PA Gain Step 0 to 1 */

  RFM_PA_GAIN_STEP_1_TO_2, /*!< PA Gain Step 1 to 2 */

  RFM_PA_GAIN_STEP_2_TO_3, /*!< PA Gain Step 2 to 3 */

  RFM_PA_GAIN_STEP_3_TO_4, /*!< PA Gain Step 3 to 4 */

  RFM_PA_GAIN_STEP_4_TO_5, /*!< PA Gain Step 4 to 5 */

  RFM_PA_GAIN_STEP_5_TO_6, /*!< PA Gain Step 5 to 6 */

  RFM_PA_GAIN_STEP_6_TO_7, /*!< PA Gain Step 6 to 7 */

  RFM_PA_GAIN_STEP_NUM /*!< Number of PA Gain Steps */

} rfm_pa_gain_step_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This enum represents the states (High/Low) that the PA R1 and R0 hardware 
  lines may take
*/
typedef enum
{
  RFM_PA_R1_R0_0_0 = 0, /*!< R1 and R0 lines low */

  RFM_PA_R1_R0_0_1 = 1, /*!< R1 line Low and R0 line High  */

  RFM_PA_R1_R0_1_0 = 2, /*!< R1 line High and R0 line Low */

  RFM_PA_R1_R0_1_1 = 3, /*!< R1 line High and R0 line High */

  RFM_PA_RANGE_4 = 4, /*!< PA Gain Range 4 */

  RFM_PA_RANGE_5 = 5, /*!< PA Gain Range 5 */

  RFM_PA_RANGE_6 = 6, /*!< PA Gain Range 6 */

  RFM_PA_RANGE_7 = 7, /*!< PA Gain Range 7 */

  RFM_PA_GAIN_RANGE_NUM

} rfm_pa_range_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enumeration of feedback path attuenuation states
*/
typedef enum
{
  RFM_FB_ATTENUATION_STATE_LOW,
  RFM_FB_ATTENUATION_STATE_MID,
  RFM_FB_ATTENUATION_STATE_HIGH,
  RFM_FB_ATTENUATION_STATE_INVALID
} rfm_fb_attenuation_state_type;

#ifdef __cplusplus
}
#endif

#endif  /* RFM_GAIN_STATE_TYPES_H */

