/*!
  @file
  lte_common.h

  @brief
  This file contains definitions for the Common type and constants required
  by different modules within LTE Access Stratum(AS)

*/

/*=============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_common.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/20/10   wshaw   add watchdog support for LTE tasks
=============================================================================*/

#ifndef LTE_COMMON_H
#define LTE_COMMON_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/

#include <comdef.h>
#include <lte_ind_msg.h>
#include <lte_cfg.h>
#include <dog.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/


/*=============================================================================

                   EXTERNAL ENUMERATION TYPES

=============================================================================*/

/*=============================================================================

                   EXTERNAL STRUCTURE/UNION TYPES

=============================================================================*/
/*! @brief watchdog related variables are defined here
*/
typedef struct
{
  dog_report_type   wdog_handle;
  uint32            wdog_wakeup_time;
  boolean           wdog_disable;
} lte_wdog_info_s;


/*=============================================================================

                   DATA TYPES

=============================================================================*/

#endif  //LTE_COMMON_H
