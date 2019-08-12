#ifndef FTM_UNIFIED_QUERY_INTERFACE_H
#define FTM_UNIFIED_QUERY_INTERFACE_H
/*!
  @file
  ftm_unified_query_interface.h

  @brief
  Common framework to perform FTM Unified Query 
*/

/*======================================================================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

======================================================================================================================*/

/*======================================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rffactory/intf/ftm_unified_query_interface.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
06/07/15   jfc     Initial Release

======================================================================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef T_WINNT
#error code not present
#endif

/*====================================================================================================================*/
/*!
  @addtogroup FTM_TEST_TOOLS_CID
  @{
*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! Enumeration to indicate the type of commands available for FTM Unified Query command */
typedef enum
{
  FTM_UNIFIED_QUERY_CMD_UNASSIGNED = 0, /*!< 0 : Unassigned command */

  FTM_UNIFIED_QUERY_CMD_QUERY_RECOMMENDED_RADIO_ALLOCATION = 1, /*!< 1 : RF Radio Allocation command */

  FTM_UNIFIED_QUERY_CMD_NUM  /*!< Max : Defines maximum number of command IDs */

} ftm_unified_query_command_enum_type;

/*! @} */ /* FTM_TEST_TOOLS_CID */


#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FTM_UNIFIED_QUERY_INTERFACE_H */

