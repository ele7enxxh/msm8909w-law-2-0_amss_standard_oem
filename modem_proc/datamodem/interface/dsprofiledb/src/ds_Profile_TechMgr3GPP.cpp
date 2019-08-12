/*===========================================================================
  @file ds_Profile_TechMgr3GPP.cpp

  This file implements 3GPP Technology Manager class which is responsible for 
  providing functionality related to a 3GPP technology (i.e allocating a
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
#include "ds_Profile_TechMgr3GPP.h"
#include "ds_Profile_TechMgr.h"
#include "ds_Profile_Log.h"

extern "C"
{
#include "ds_3gpp_profile_api.h"
}

using namespace ds::Profile;
/*===========================================================================

                     GLOBAL DECLARATIONS

===========================================================================*/
static const int DB_3GPP_MIN_RANGE = 0;
/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
TechMgr3GPP::TechMgr3GPP(void) : TechMgr(DS_PROFILE_DB_3GPP_MAX_NON_PERS_PROFILE_COUNT,
                                   DB_TECH_3GPP) 
{
 memset(&tech3GPP, 0, sizeof(tech_func_table_type));
}


bool TechMgr3GPP::CanReserveProfileOutsideOfAllocationRange(
  ds_profile_db_profile_num_type num, 
  bool during_init /* = false */,
  bool *overwrite /* = NULL */,
  bool is_epc /* = false*/) const
{
  if (during_init)
{
    if (num >= DB_3GPP_CREATE_RANGE_MIN && num <= DB_3GPP_CREATE_RANGE_MAX)
    {
    return true;
  }
  return false;
  } 
  else
  {
    PROFILE_DB_LOG_HIGH_3("3GPP: Can ReserveOutside of Allocation func num %d, during_init %d, is_epc %d", 
                          num, during_init, is_epc);
    if (num >= DB_3GPP_CREATE_RANGE_MIN && num <= DB_3GPP_CREATE_RANGE_MAX)
    {
      if (is_epc)
      {
        return false;
      } 
      else
      {
        if (overwrite)
        {
          *overwrite = true;
        }
        return true;
      }
    }
    else if (num >= TechMgr::ALLOC_RANGE_MIN && num <= TechMgr::ALLOC_RANGE_MAX)
    {
      if (is_epc)
    {
      return true;
    }
      else
      {
        return false;
      }
    }
    return false;
  }
}

bool TechMgr3GPP::CanDeleteProfile(ds_profile_db_profile_num_type num,
                                   boolean enable_deleting_default_profile) const
{
  ds_profile_db_profile_num_type default_num;
  ds_profile_db_result_type res;

  if (enable_deleting_default_profile)
  {
    return true;
  }

  /* iterate over all default profiles and make sure that num is not one of them*/
  for (int i = DB_EMBEDDED_PROFILE_FAMILY; i < DB_MAX_PROFILE_FAMILY; i++)
  {
    if (DB_RESULT_SUCCESS != 
          (res = GetDefaultProfileNum(&default_num, (ds_profile_db_family_type)i)))
    {
      PROFILE_DB_LOG_ERROR_1("CanDeleteProfile(): could not get default profile num, error %d, returning false", res);
      return false;
    }

    if (num == default_num)
    {
      PROFILE_DB_LOG_ERROR_1("CanDeleteProfile(): can't delete default profile for family %d", i);
      return false;
    }
      

    for (int j = DB_PROFILE_ACTIVE_SUBSCRIPTION_1; 
         j < DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX; j++)
    {
      if (DB_RESULT_SUCCESS != 
            (res = GetDefaultProfileNum(&default_num, (ds_profile_db_family_type)i, (ds_profile_db_subs_type)j)))
      {
        PROFILE_DB_LOG_ERROR_1("CanDeleteProfile(): could not get default profile num, error %d, returning false", res);
        return false;
      }

      if (num == default_num)
      {
        PROFILE_DB_LOG_ERROR_2("CanDeleteProfile(): can't delete default profile for family %d, subs %d", i, j);
        return false;
      }
    }
  }
  
  return true;
}

ds_profile_db_result_type TechMgr3GPP::GetParamsFromCache(
  ds_profile_db_profile_num_type num,
  ds_profile_db_subs_type subs,
  void* tech_params, 
  ds_profile_db_gen_params_type* gen_params)
{
  if (tech3GPP.get_profile)
  {
    return tech3GPP.get_profile(num, tech_params, gen_params,  subs);
  }
  else
  {
    memset(tech_params, 0 , sizeof(ds_profile_db_3gpp_tech_params_type));
    return DB_RESULT_SUCCESS;
  }
}


ds_profile_db_result_type TechMgr3GPP::GetDefaultProfileNum(
  ds_profile_db_profile_num_type * num,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs) const
{
  if (tech3GPP.get_default_profile_num_per_subs)
  {
    return tech3GPP.get_default_profile_num_per_subs(num, family, subs);
  }
  else
  {
    return DB_RESULT_ERR_INVAL_PROFILE_FAMILY;
  }
}

ds_profile_db_result_type TechMgr3GPP::SetDefaultProfileNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs) const
{
  if (tech3GPP.set_default_profile_num_per_subs)
  {
    return tech3GPP.set_default_profile_num_per_subs(num, family, subs);
  }
  else
  {
    return DB_RESULT_ERR_INVAL_PROFILE_FAMILY;
  }
}

ds_profile_db_result_type TechMgr3GPP::GetDefaultProfileNum(
  ds_profile_db_profile_num_type * num,
  ds_profile_db_family_type family) const
{
  ds_profile_db_subs_type subs = (ds_profile_db_subs_type)ps_sys_get_default_data_subscription();
  if (tech3GPP.get_default_profile_num_per_subs)
  {
    return tech3GPP.get_default_profile_num_per_subs(num, family, subs);
  }
  else
  {
    return DB_RESULT_ERR_INVAL_PROFILE_FAMILY;
  }
}

ds_profile_db_result_type TechMgr3GPP::SetDefaultProfileNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type family) const
{
  ds_profile_db_subs_type subs = (ds_profile_db_subs_type)ps_sys_get_default_data_subscription();
  if (tech3GPP.set_default_profile_num_per_subs)
  {
    return tech3GPP.set_default_profile_num_per_subs(num, family, subs);
  }
  else
  {
    return DB_RESULT_ERR_INVAL_PROFILE_FAMILY;
  }
}

void TechMgr3GPP::GetProfilesRange(
  ds_profile_db_profile_num_type* min_num,
  ds_profile_db_profile_num_type* max_num ) const
{
  *min_num = DB_3GPP_CREATE_RANGE_MIN;
  *max_num = ALLOC_RANGE_MAX;
}

bool TechMgr3GPP::CanWriteProfile(
  ds_profile_db_profile_num_type num) const
{
  return true;
}

bool TechMgr3GPP::ShouldBeTreatedAsNonPersistent(
  ds_profile_db_profile_num_type num) const
{
  return false;
}

bool TechMgr3GPP::AllowToCreateFileOnFirstModify(
  ds_profile_db_profile_num_type num) const
{
  return false;
}

ds_profile_db_result_type TechMgr3GPP::GetIsEpcParamSet(
   ds_profile_db_profile_num_type num,
   ds_profile_db_ident_type       ident,
  boolean *is_tech_param_set,
  ds_profile_db_subs_type subs) const
{
  if (tech3GPP.get_is_tech_param_set)
  {
    return tech3GPP.get_is_tech_param_set(num, subs, ident, is_tech_param_set);
  }
  else
  {
    *is_tech_param_set = false;
    return DB_RESULT_SUCCESS;
  }
}

void TechMgr3GPP::ResetNonPersistentCount(
  ds_profile_db_subs_type subs)
{
  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return;
  }

  non_pers_profile_count[subs] =  DS_PROFILE_DB_3GPP_MAX_NON_PERS_PROFILE_COUNT;
}

void TechMgr3GPP::SetTechTable(tech_func_table_type *techTbl)
{
  if (techTbl)
  {
    tech3GPP = *techTbl;
  }
}

ds_profile_db_result_type TechMgr3GPP:: AllocateNum(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_subs_type subs,
  ds_profile_db_supported_tech_mask tech_mask)
{
  uint8 index = 0;
  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return DB_RESULT_ERR_INVAL_SUBS_ID;
  }
  for (index = 1; index <= DB_3GPP_CREATE_RANGE_MAX; index ++)
  {
    if (!number_pool_3gpp[subs][index])
    {
	    number_pool_3gpp[subs][index] = TRUE;
      *num = (ds_profile_db_profile_num_type)index;
      return DB_RESULT_SUCCESS;
    }
  }

  return DB_RESULT_ERR_OUT_OF_PROFILES;
}

ds_profile_db_result_type TechMgr3GPP::CheckAndReserveNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_subs_type subs,
  bool overwrite)
{
  PROFILE_DB_LOG_MED_1("Use new mechanism %d", num);
  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return DB_RESULT_ERR_INVAL_SUBS_ID;
  }

  if (!(num >= DB_3GPP_CREATE_RANGE_MIN && num <= DB_3GPP_CREATE_RANGE_MAX))
  {
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }


  if (overwrite)
  {
    number_pool_3gpp[subs][num] = true;
    return DB_RESULT_SUCCESS;
  }

  if (!number_pool_3gpp[subs][num])
  {
    number_pool_3gpp[subs][num] = true;
    return DB_RESULT_SUCCESS;
  } 
  else
  {
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  return DB_RESULT_SUCCESS; 
}

ds_profile_db_result_type TechMgr3GPP::FreeNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask mask,
  ds_profile_db_subs_type subs)
{
  PROFILE_DB_LOG_MED_1("Using new Mechanism %d", num);
  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return DB_RESULT_ERR_INVAL_SUBS_ID;
  }

  if (!(num >= DB_3GPP_CREATE_RANGE_MIN && num <= DB_3GPP_CREATE_RANGE_MAX))
  {
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  /* if the number was never taken, return an error */
  if (!number_pool_3gpp[subs][num])
  {
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }
  number_pool_3gpp[subs][num] = false; 
  return DB_RESULT_SUCCESS;
}
