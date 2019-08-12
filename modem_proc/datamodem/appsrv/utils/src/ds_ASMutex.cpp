/*==============================================================================

                              ds_ASMutex.cpp

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
#include "ds_ASMutex.h"
namespace ASUtils = Appsrv::Utils;


ASUtils::ASMutex::ASMutex()
{
  memset(&mutex, 0, sizeof(rex_crit_sect_type));
  rex_init_crit_sect(&mutex);
}


void ASUtils::ASMutex::lock()
{
  rex_enter_crit_sect(&mutex);
}


void ASUtils::ASMutex::unlock()
{
  rex_leave_crit_sect(&mutex);
}




ASUtils::ASLockGuard::ASLockGuard(ASMutex& crit_sect)
: mutex(&crit_sect)
{
  mutex->lock();
}


ASUtils::ASLockGuard::~ASLockGuard()
{
  mutex->unlock();
}

