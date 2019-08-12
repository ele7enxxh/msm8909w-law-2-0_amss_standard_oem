#ifndef RF_COMMON_MEM_PROFILE_H
#define RF_COMMON_MEM_PROFILE_H

/*!
   @file
   rfcommon_mem_profile.h

   @brief
   This file contains structures for memory profiling.
 
   @{
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_mem_profile.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/22/12   cvd     Initial version

==============================================================================*/

#include "comdef.h"
#include "modem_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data type for memory usage

  @details
  This datatype will indicate the memory used. It is an int32 as the difference
  in usage can be negative
*/

typedef int32  rfm_memusage_type;

rfm_memusage_type
rfm_memusage_get
(
  void
);

rfm_memusage_type
rfm_memusage_get_change
(
  rfm_memusage_type start
);

#ifdef __cplusplus
}
#endif

#endif /* RF_COMMON_MEM_PROFILE_H */