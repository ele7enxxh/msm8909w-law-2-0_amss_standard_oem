#ifndef FTM_COMMON_LOCKS_H
#define FTM_COMMON_LOCKS_H
/*!
  @file
  ftm_common_locks.h

  @brief
  This module contains APIs that encapsulate critical section routines
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_locks.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/18/11   sty     Initial revision

==============================================================================*/

#include "rex.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
void
ftm_common_init_critical_section
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_common_enter_critical_section
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_common_leave_critical_section
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_COMMON_LOCKS_H */
