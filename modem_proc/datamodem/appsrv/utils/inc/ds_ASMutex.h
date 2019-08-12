/*==============================================================================

                              ds_ASMutex.h

GENERAL DESCRIPTION
  Critical section utility

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_ASMUTEX_H
#define DS_ASMUTEX_H

#include "rex.h"

namespace Appsrv
{
namespace Utils
{


/*==============================================================================
CLASS ASMutex

DESCRIPTION
  Object used to manage critical sections. Calls rex_enter_crit_sect upon
  construction and rex_leave_crit_sect upon destruction.

DEPENDENCIES
  crit_sect not NULL
==============================================================================*/
class ASMutex
{
public:
  ASMutex();

  void lock();
  void unlock();

private:
  // Remove copy constructor
  ASMutex(const ASMutex&);

  rex_crit_sect_type mutex;
};



/*==============================================================================
CLASS LockGuard

DESCRIPTION
  Object used to manage mutex object.
  Locks the mutex on construction and unlocks on destruction.

DEPENDENCIES
  None
==============================================================================*/
class ASLockGuard
{
public:
  // ASLockGuard(rex_crit_sect_type* crit_sect);
  ASLockGuard(ASMutex& crit_sect);
  ~ASLockGuard();


private:
  ASMutex* mutex;

  // Remove default, copy constructor
  ASLockGuard();
  ASLockGuard(const ASLockGuard&);
};


} /* Utils */
} /* Appsrv */

#endif /* DS_ASMUTEX_H */
