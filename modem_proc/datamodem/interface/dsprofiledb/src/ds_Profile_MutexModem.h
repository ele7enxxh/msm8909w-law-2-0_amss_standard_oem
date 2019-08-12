#ifndef DS_PROFILE_MUTEX_MODEM_H
#define DS_PROFILE_MUTEX_MODEM_H
/*===========================================================================
  @file ds_Profile_MutexModem.h

  This file defines concrete class for mutex used in ds profile db on modem
  
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
#include "ds_profile_db.h"
#include "ds_Profile_Mutex.h"
#include "ds_Profile_MemManager.h"

extern "C"
{
#include "rex.h"
}

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class MutexModem : public Mutex
    {
      public:   

        DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(PS_MEM_PROFILE_DB_MUTEX_MODEM_TYPE);

        static MutexModem* CreateInstance();

        virtual int Lock();
        virtual int TryLock();
        virtual int Release();

        virtual ~MutexModem();
        
      protected:
        
        MutexModem():inited(false){};

      private:      

        bool inited;        
        rex_crit_sect_type mutex;
    }; /* class Mutex */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_MUTEX_MODEM_H */
