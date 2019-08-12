#ifndef RFM_PATH_TYPES_H
#define RFM_PATH_TYPES_H

/*!
  @file
  rfm_path_types.h

  @details
  This file exports the definitions of types that are used in the RFM
  interface relating to RF Path parameters.
*/

/*==============================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_path_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/17/11   aak     Moving rfm_path_state to rfm_mode_types.h
04/22/11   aak     Enum for possible path states
12/13/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif


/*!
  @brief
  A listing of all potential RF path in the system.
*/
typedef enum
{ 
  RFM_PATH_0, /*!< RF Path 0 */
  RFM_PATH_1, /*!< RF Path 1 */
  RFM_PATH_2, /*!< RF Path 2 */
  RFM_PATH_3, /*!< RF Path 3 */
  RFM_PATH_NUM, /*!< Total number of RF Paths */
  RFM_PATH_MAX=RFM_PATH_NUM /*!< Max number of RF Paths */
} rfm_path_enum_type;


#ifdef __cplusplus
}
#endif

#endif /* RFM_PATH_TYPES_H */

