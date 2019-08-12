/*===========================================================================
  @file ds_Profile_MutexModem.cpp

  This file implements concrete class for mutex used in ds profile db on modem
  
                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-06-30 am Created module  

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

    
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Profile_MutexModem.h"

using namespace ds::Profile;
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

MutexModem* MutexModem::CreateInstance(void)
{
  MutexModem* m = new MutexModem();
  if (!m)
  {
    return NULL;
  }

  rex_init_crit_sect(&(m->mutex));

  m->inited = true;

  return m;
}


int MutexModem::Lock(void)
{
  rex_enter_crit_sect(&(mutex));

  return 0;
}

int MutexModem::Release(void)
{
  rex_leave_crit_sect(&(mutex));

  return 0;
}

int MutexModem::TryLock(void)
{
  return rex_try_enter_crit_sect(&(mutex));
}

MutexModem::~MutexModem(void)
{
  if (!inited)
{
    return;
  }

#if !defined(TEST_FRAMEWORK) || !defined(FEATURE_QUBE)
    rex_del_crit_sect( &mutex );
#endif

  inited = false;
}