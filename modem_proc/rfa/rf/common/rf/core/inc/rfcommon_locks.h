#ifndef RF_CDMA_LOCKS_H
#define RF_CDMA_LOCKS_H

/*!
  @file
  rf_cdma_locks.h
 
  @details
  This file defines interfaces used to make CDMA Driver Thread Safe.
*/

/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_locks.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/30/12   bmg     Added non-blocking try function
05/02/12   aro     Interface to de-init critical section
08/11/11   aro     Added C++ protection
07/06/11   aro     Updated Lock interfaces to pass the Lock Data
04/21/11   aro     Branched file to common location
04/21/11   aro     Changed to move LOCK implementation to common
04/21/11   aro     Renamed LOCKs function to move to Common area
03/08/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rex.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Lock Data Type to be used for critical section */
typedef rex_crit_sect_type rf_lock_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_common_init_critical_section
(
  rf_lock_data_type *crit_section
);

/*----------------------------------------------------------------------------*/
boolean
rf_common_deinit_critical_section
(
  rf_lock_data_type *crit_section
);

/*----------------------------------------------------------------------------*/
void
rf_common_enter_critical_section
(
  rf_lock_data_type *crit_section
);

/*----------------------------------------------------------------------------*/
boolean
rf_common_try_enter_critical_section
(
  rf_lock_data_type *crit_section
);

/*----------------------------------------------------------------------------*/
void
rf_common_leave_critical_section
(
  rf_lock_data_type *crit_section
);

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_LOCKS_H */
