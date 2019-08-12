/*===========================================================================
  @file ds_Profile_PlatformModem.cpp

  This file implements concrete class for all platform dependent functionality 
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

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Profile_PlatformModem.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_FileEFS.h"
#include "ds_Profile_FileSFS.h"
#include "ds_Profile_CallbackMgrModem.h"
#include "ds_Profile_MutexModem.h"
#include "ds_Profile_MemManager.h"


using namespace ds::Profile;
/*===========================================================================

                     GLOBAL DECLARATIONS

===========================================================================*/
static bool inited = false;
static PlatformModem platformModem;

Platform* Platform::theInstance = &platformModem;
/*===========================================================================

                     MEMBER FUNCTIONS

===========================================================================*/
ds_profile_db_result_type PlatformModem::Init(void)
{
  inited = false;
  ds_profile_db_result_type res;

  if (inited)
  {
    return DB_RESULT_ERR_INVAL;
  } 

  if (DB_RESULT_SUCCESS != (res = Platform::Init()))
  {
    return res;
  }

  /* TODO implement platform dependent behavior in Profile */  
  if (DB_RESULT_SUCCESS != (res = Profile::Init()))
  {
    return res;
  }

  if (DB_RESULT_SUCCESS != (res = CallbackMgrModem::Init()))
  {
    return res;
  }

  inited = true;

  return DB_RESULT_SUCCESS;
}


void PlatformModem::Deinit(void)
{
  CallbackMgrModem::Deinit();
  inited = false;
}


File* PlatformModem::GetFile(ds_profile_db_profile_num_type num, ds_profile_db_subs_type subs, File::FlagType flag)
{
  File* file = NULL ;

  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return NULL;
  }

  if (is_sfs[subs])
  {
    file = new FileSFS(num, subs, flag);
  }
  else
  {
    file = new FileEFS(num, subs, flag);
  }
  if (!file || !file->IsOpen())
  {
    if (file)
    {
      delete file;
      file = NULL;
    }
    return NULL;
  }

  return file;
}

File* PlatformModem::GetFile(ds_profile_db_subs_type subs)
{

  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return NULL;
  }

  if (is_sfs[subs])
  {
    return new FileSFS();
  }
  else
  {
    return new FileEFS();  
  }
}

CallbackMgr* PlatformModem::GetCallbackMgrSingleton(void)
{
  return CallbackMgrModem::Instance();
}

Mutex* PlatformModem::GetMutex(void)
{
  return MutexModem::CreateInstance();
}

void PlatformModem::SetIsSFS(boolean is_sfs, ds_profile_db_subs_type subs)
{
  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return;
  }
  this->is_sfs[subs] = is_sfs;
}
