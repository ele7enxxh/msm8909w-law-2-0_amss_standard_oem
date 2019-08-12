#ifndef DS_PROFILE_DB_PLATFORM_H
#define DS_PROFILE_DB_PLATFORM_H
/*===========================================================================
  @file ds_Profile_Platform.h

  This file defines abstract class for all platform dependent functionality 
  in ds profile db

  THERE SHOULD BE ONLY ONE FILE DERIVING CLASS WITH CONCRETE IMPLEMENTATION PER
  PLATFORM
  
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
#include "ds_Profile_CallbackMgr.h"
#include "ds_Profile_Mutex.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class Platform
    {
      public:

        static Platform* Instance();

        /* invoke this method from the derived class, it has a platform independent implementation */
        virtual ds_profile_db_result_type Init() = 0;   

        virtual void Deinit() = 0;   
        virtual File* GetFile(ds_profile_db_profile_num_type num, ds_profile_db_subs_type subs, File::FlagType flag) = 0;
        virtual File* GetFile(ds_profile_db_subs_type subs)  = 0;
        virtual CallbackMgr* GetCallbackMgrSingleton() = 0;

        /* failOnDoubleLock - if true, this mutex will fail to be locked if already locked */
        virtual Mutex* GetMutex() = 0;

        virtual void SetIsSFS(boolean is_sfs, ds_profile_db_subs_type subs) = 0;

      protected:
        /* should be inited in derived classes */
        static Platform* theInstance;  
        Platform() {};

      private:            

        /* do not define, this is to prevent copying the object */
        Platform(const Platform&);
        Platform& operator=(const Platform&);

    }; /* class Platform */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_DB_PLATFORM_H */
