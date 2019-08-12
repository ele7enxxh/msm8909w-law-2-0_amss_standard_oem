#ifndef IXLOCK_H
#define IXLOCK_H
/**
   @file ixlock.h

   @brief 
   The generic lock module.

   This module provides a wrapper layer that routes calls to INTLOCK either
   on ARM L4 or native REX or Hexagon L4.

   Externalized Functions:
   IxLock,
   IxUnlock.

   Initialization and sequencing requirements:
   None.
*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                            H E A D E R   F O R
                          I X I P C  M O D U L E

Copyright (c) 2006-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                         EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/ixlock.h#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
01/14/11     leo     (Tech Pubs) Edited Doxygen comments and markup.
12/11/10     nk      Doxygenated.
05/10/10     yh      IxLockInit is added in header file.
10/31/06     qxu     Initial Revision.

===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE

===========================================================================*/

/*
* Include files
*/
#include "rex.h"
#include "comdef.h"

/** @addtogroup SMECOMUTIL_IPC 
    @{
*/

/**
   Placeholder for a generic lock type attribute.
*/
typedef enum
{
LOCK_TIME_CRITICAL_ONLY,    /**< Lock time critical section.  */ 
LOCK_TASK_ONLY,             /**< Lock task.                   */  
LOCK_ISR_ONLY,              /**< Lock between thread and ISR. */ 
LOCK_TASK_TIME_CRITICAL,    /**< Lock between threads only.   */
LOCK_ISR_TIME_CRITICAL      /**< Use REX critical sections.   */
} IxLockEnumType;


/**
   @brief
   Placeholder for a generic lock mutex variable.
*/
typedef struct
{
  IxLockEnumType  e_LockType;     /**< Mutex type: One of three types.       */ 
rex_crit_sect_type  z_CritSect; /**< REX-based critical section structure. */
} strLockInfo;

/**
   Abstraction call that locks a critical section or interrupts,
   depending on the lock type being set of type IxLockEnumType.
   Supports REX, L4, and BLAST.

   @param[in] p_Lock Pointer to the lock information.

   @return 
   None.

   @dependencies
   None.
*/
void IxLock(strLockInfo *p_Lock);

/**
   Abstraction call that unlocks a critical section or interrupts.
   Supports REX, L4, and BLAST.

   @param[in] p_Lock Pointer to the lock information.

   @return 
   None.

   @dependencies
   None.
*/
void IxUnlock(strLockInfo *p_Lock);

/**
  Abstraction call that intializes a critical section or interrupts.
   Supports REX, L4, and BLAST.

   @param[in] p_Lock Pointer to the lock information.
   @param[in] e_Type Placeholder that defines the type of lock.

   @return 
   TRUE -- Section is intialized.\n
   FALSE -- Section is not initialized.

   @dependencies
   None.
*/
boolean IxLockInit(strLockInfo *p_Lock, IxLockEnumType e_Type);

/** @} */ /* end_addtogroup SMECOMUTIL_IPC */

#endif /* IXLOCK_H */
