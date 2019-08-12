#ifndef RFM_CDMA_POWER_MODE_TYPES_H
#define RFM_CDMA_POWER_MODE_TYPES_H

/*!
  @file
  rfm_cdma_power_mode_types.h

  @details
  This file exports the definitions and declartions pertaining to Power Modes.
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rfm_cdma_power_mode_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/06/11   aro     Added enum for invalid Power mode
06/15/11   aro     Deleted obselete power mode enums
02/01/11   aro     Initial Release.

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Available Power Modes
*/
typedef enum
{

  RFM_CDMA_POWER_MODE_NORMAL,
  /*!< Normal Power Mode */

  RFM_CDMA_POWER_MODE_1,
  /*!< Low Power Mode 1 */

  RFM_CDMA_POWER_MODE_2,
  /*!< Low Power Mode 2 */

  RFM_CDMA_POWER_MODE_MAX,
  /*!< Power Mode Max : Not used */

  RFM_CDMA_POWER_MODE_INVALID = RFM_CDMA_POWER_MODE_MAX
  /*!< Invalid Powe Mode */

} rfm_cdma_power_mode_type;

#ifdef __cplusplus
}
#endif

#endif  /* RFM_CDMA_POWER_MODE_TYPES_H */


