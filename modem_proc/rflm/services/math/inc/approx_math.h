/*!
  @file
  approx_math.h

  @brief
  Approximate math functions.
*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef APPROX_MATH_H
#define APPROX_MATH_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*! @brief Approx math result/scale pair.
*/
typedef struct
{
  uint32 result;   /*!< Result value */
  int32 scale;     /*!< Scale factor */
} uint32_result_scale_t;

#ifdef __cplusplus
}
#endif

#include "approx_uinv.h"
#include "approx_dB_to_ulin.h"
#include "approx_ulin_to_dB.h"
#include "approx_atan2.h"
#include "approx_uinvsqrt.h"


#endif /* APPROX_MATH_H */
