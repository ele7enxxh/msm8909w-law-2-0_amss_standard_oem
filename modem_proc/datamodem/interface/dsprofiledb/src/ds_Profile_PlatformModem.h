#ifndef DS_PROFILE_DB_PLATFORM_MODEM_H
#define DS_PROFILE_DB_PLATFORM_MODEM_H
/*===========================================================================
  @file ds_Profile_PlatformModem.h

  This file defines concrete class for all platform dependent functionality 
  in ds profile db on the modem side
  
                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-06-18 am Created module  

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

    
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_profile_db.h"
#include "ds_Profile_File.h"
#include "ds_Profile_Platform.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class PlatformModem : public Platform
    {
      public:
        
        virtual ds_profile_db_result_type Init();   
        virtual void Deinit();
        virtual File* GetFile(ds_profile_db_profile_num_type num, ds_profile_db_subs_type subs, File::FlagType flag);
        virtual File* GetFile(ds_profile_db_subs_type subs);
        virtual CallbackMgr* GetCallbackMgrSingleton();
        virtual Mutex* GetMutex();
        virtual void   SetIsSFS(boolean is_sfs, ds_profile_db_subs_type subs);
      
      private:

        boolean is_sfs[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX];
    }; /* class PlatformModem */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_DB_PLATFORM_MODEM_H */
