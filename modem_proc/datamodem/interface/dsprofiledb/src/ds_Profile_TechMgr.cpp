/*===========================================================================
  @file ds_Profile_TechMgr.cpp

  This file implements Technology Manager abstract class which is responsible for 
  providing functionality related to a particular technology (i.e allocating a
  profile number within the valid range, set/get default profile, etc)

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-05-20 am Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Profile_TechMgr.h"
#include "ds_Profile_Log.h"

using namespace ds::Profile;
/*===========================================================================

                     STATIC MEMBER INITIALIZATIONS

===========================================================================*/
int TechMgr::pers_profile_count = DS_PROFILE_DB_MAX_PERS_PROFILE_COUNT;
/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
TechMgr::~TechMgr(void)
{
  pers_profile_count = DS_PROFILE_DB_MAX_PERS_PROFILE_COUNT;
}

int TechMgr::GetCount(bool is_persistent,
                      ds_profile_db_subs_type subs) const
{
  if (is_persistent)
  {
    return pers_profile_count;
  } 
  else
  {
    if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
        DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
    {
      PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
      return -1;
    }
    return non_pers_profile_count[subs];
  }
}

int TechMgr::DecCount(bool is_persistent,
                      ds_profile_db_subs_type subs)
{
  int* tmp_ptr;
  if (is_persistent)
  {
    tmp_ptr = &pers_profile_count;
  } 
  else
  {
    if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
        DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
    {
      PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
      return -1;
    }
    tmp_ptr = &non_pers_profile_count[subs];
  }

  if (*tmp_ptr > 0)
  {
    --(*tmp_ptr);
    PROFILE_DB_LOG_MED_3("DecCount(): new count for profiles with persistency = %d"
                         " tech = %d is %d",
                         is_persistent, tech, *tmp_ptr);  
  }

  return (*tmp_ptr);
}

int TechMgr::IncCount(bool is_persistent,
                      ds_profile_db_subs_type subs)
{
  int* tmp_ptr;
  if (is_persistent)
  {
    tmp_ptr = &pers_profile_count;
  } 
  else
  {
    if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
        DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
    {
      PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
      return -1;
    }
    tmp_ptr = &non_pers_profile_count[subs];
  }

  if (*tmp_ptr >= 0) //TODO :: Get max count
  {
    ++(*tmp_ptr);
    PROFILE_DB_LOG_MED_3("IncCount(): new count for profiles with persistency = %d"
                         " tech = %d is %d",
                         is_persistent, tech, *tmp_ptr);  
  }

  return (*tmp_ptr);
}

void TechMgr::ResetPersistentProfileCount()
{
  pers_profile_count = DS_PROFILE_DB_MAX_PERS_PROFILE_COUNT;
}

ds_profile_db_result_type TechMgr::AllocateNum(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_subs_type subs,
  ds_profile_db_supported_tech_mask tech_mask)
{
  PROFILE_DB_LOG_MED_1("Use old mechanism of numbering for other technologies %d", tech_mask);
  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type TechMgr::CheckAndReserveNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_subs_type subs,
  bool overwrite)
{
  PROFILE_DB_LOG_MED_1("Use old mechanism of checking for this num %d", num);
  return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
}

ds_profile_db_result_type TechMgr::FreeNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask mask,
  ds_profile_db_subs_type subs)
{
  PROFILE_DB_LOG_MED_1("Use old mechanism of checking for this num %d", num);
  return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
}