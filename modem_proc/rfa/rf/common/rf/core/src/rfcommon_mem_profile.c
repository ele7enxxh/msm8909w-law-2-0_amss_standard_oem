/*!
   @file
   rfcommon_mem_profile.c

   @brief
   This file contains functions for memory profiling.

   @details
   The functions here only profile the memory allocated dynamically using the 
   modem_mem_alloc()/modem_mem_calloc() family of functions.
 
   @addtogroup RF_COMMON_MEM_PROFILE
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_mem_profile.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/22/12   cvd     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfcommon_mem_profile.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the current memory allocation.

  @details
  This function returns the amount of memory currently allocated in the heap
  in bytes.

  @return
  The amount of memory currently allocated in the heap in bytes.

  @todo
  Return the value of modem_mem_to_be_deprecated_debug_get_client_current_usage(cl)
*/
rfm_memusage_type
rfm_memusage_get
(
  void
)
{
  return 0;//modem_mem_to_be_deprecated_debug_get_client_current_usage(MODEM_MEM_CLIENT_RFA); 
}/* rfm_memusage_get */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the change in memory allocation.

  @details
  This function returns the change in the amount of memory allocated from the 
  time "start" was set to the current call.

  @param start
  The memory usage at the start of profiling.

  @return
  The difference in memory usage in bytes.

  @todo
  Return the value of modem_mem_to_be_deprecated_debug_get_client_current_usage(cl) - start
*/
rfm_memusage_type
rfm_memusage_get_change
(
  rfm_memusage_type start
)
{
  return 0;//modem_mem_to_be_deprecated_debug_get_client_current_usage(MODEM_MEM_CLIENT_RFA) - start;
}/* rfm_memusage_get_change */

/*! @} */
