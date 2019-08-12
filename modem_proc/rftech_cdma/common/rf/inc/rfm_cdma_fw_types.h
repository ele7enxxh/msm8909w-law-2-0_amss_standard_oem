#ifndef RFM_CDMA_FW_TYPES_H
#define RFM_CDMA_FW_TYPES_H

/*!
   @file
   rfm_cdma_fw_types.h

   @brief
   Contains FW types to be included by RF driver.
*/

/*==============================================================================

  Copyright (c) 2011 - Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rfm_cdma_fw_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/14/11   aro     Added invalid FW state
07/19/11   Saul    First Version.
==============================================================================*/ 
 
#include "comdef.h" 
 
#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Specifies CDMA Firmware States for RF
*/
typedef enum
{
  RFM_CDMA_FW_STATE_DISABLE,
  /*!< Disabled */

  RFM_CDMA_FW_STATE_ACTIVE,
  /*!< Active */

  RFM_CDMA_FW_STATE_MEAS,
  /*!< MEASUREMENT */

  RFM_CDMA_FW_STATE_INVALID
  /*!< Invalid FW state */

} rfm_cdma_fw_state_type;

#ifdef __cplusplus
}
#endif

#endif
