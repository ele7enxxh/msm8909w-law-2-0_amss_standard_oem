/*!
  @file
  cp_mutex.h

  @brief
  Ordering of the control-plane mutexes.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/cp_mutex_order.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef _CP_MUTEX_ORDER_H_
#define _CP_MUTEX_ORDER_H_

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define CP_MUTEX_ORDER(name)  _CP_MUTEX_ORDER_##name

/*=============================================================================
  The following enum defines the order in which control-plane mutexes must
  be entered.  By following this ordering, we can prevent deadlock due to
  different threads entering mutexes in different orders.  The technique used
  is called "lock leveling"; for some references, see
  
    http://msdn.microsoft.com/en-us/magazine/cc163618.aspx#S1
    http://www.eonclash.com/Tutorials/Multithreading/MartinHarvey1.1/Ch7.html
    http://drdobbs.com/high-performance-computing/204801163?pgno=1

  The control-plane mutex tracking code assigns a unique order to each mutex,
  rather than the shared "levels" in the above references.

  In the enum below, mutexes with lower order (nearer the top of the enum) must
  be entered before mutexes with higher order (nearer the bottom of the enum).
  In general, the "closer" a mutex is to the UI the lower its order should be.
  Mutexes acquired immediately upon entry to external APIs should come nearer
  the top of this enum, those used internally should come further down, and
  those used by low-level system functions should come nearer the bottom.

  Ordering of mutexes must be done system-wide, as calling another task's API
  may lead to a mutex being acquired in that task, and it may call yet other
  tasks.  That being said, there's no need to spend a lot of time trying to
  figure out ordering until an error message in the logs indicates that there
  is a problem - at present we expect the mutex tree to be fairly flat and not
  to have many cross-task acquisitions.
=============================================================================*/

typedef enum
{

  /* Add all mutexes above this line.  The following are for unit tests only,
     and are not to be used otherwise. */
  CP_MUTEX_ORDER(MCS_UT_1),
  CP_MUTEX_ORDER(MCS_UT_2),
  CP_MUTEX_ORDER(MCS_UT_3)
} _cp_mutex_order_t;


#endif   /* #ifndef _CP_MUTEX_ORDER_H_ */
