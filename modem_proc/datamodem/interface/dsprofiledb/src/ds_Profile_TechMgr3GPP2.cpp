/*===========================================================================
  @file ds_Profile_TechMgr3GPP2.cpp

  This file implements 3GPP2 Technology Manager class which is responsible for 
  providing functionality related to a 3GPP2 technology (i.e allocating a
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
#include "ds_Profile_TechMgr3GPP2.h"
#include "ds_Profile_TechMgr.h"
#include "ds_Profile_Log.h"

extern "C"
{
#include "ds_profile_3gpp2_mgr.h"
}

using namespace ds::Profile;
/*===========================================================================

                     GLOBAL DECLARATIONS

===========================================================================*/
static const int DB_3GPP2_CREATE_DURING_INIT_RANGE_MIN = 0;
static const int DB_3GPP2_CREATE_DURING_INIT_RANGE_MAX = 0;

/*===========================================================================

                     INTERNAL FUNCTIONS

===========================================================================*/
bool TechMgr3GPP2::KDDI_or_OMH(
  ds_profile_db_profile_num_type num) const
{

  ds_profile_db_profile_num_type min_num = 0, max_num = 0;  
  ds_profile_db_result_type res;


  /* KDDI */
  if (tech3GPP2.get_profiles_range)
  {
    res = tech3GPP2.get_profiles_range(DB_3GPP2_PROFILE_KDDI,
                                       &min_num, &max_num);
  

    if (DB_RESULT_SUCCESS == res)
    {
      if (num >= min_num && num <= max_num)
      {
        return true;
      }
    }
  }

  /* OMH */
  if (tech3GPP2.get_profiles_range)
  {
    res = tech3GPP2.get_profiles_range(DB_3GPP2_PROFILE_OMH,
                                       &min_num, &max_num);

    if (DB_RESULT_SUCCESS == res)
    {
      if (num >= min_num && num <= max_num)
      {
        return true;
      }
    }
  }
  return false;
}





/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
TechMgr3GPP2::TechMgr3GPP2(void) : TechMgr(DS_PROFILE_DB_3GPP2_MAX_NON_PERS_PROFILE_COUNT,
                                     DB_TECH_3GPP2) 
{
  memset(&tech3GPP2, 0, sizeof(tech_func_table_type));
  InitNumber(DB_PROFILE_ACTIVE_SUBSCRIPTION_1);
  
}


//TODO: Check if we can have a default dummy number to support EPC
void TechMgr3GPP2::InitNumber(ds_profile_db_subs_type subs)
{
  ds_profile_db_profile_num_type min_num = 0, max_num = 0;  
  ds_profile_db_result_type res;
  tech_func_table_type* ds_profile_3gpp2_tech_tbl = NULL;
/*-----------------------------------------------------------------------------------*/
  non_pers_profile_count[subs] = DS_PROFILE_DB_3GPP2_MAX_NON_PERS_PROFILE_COUNT;
  ds_profile_3gpp2_tech_tbl = ds_profile_db_get_tech_tbl(DB_TECH_3GPP2);
  if (subs == DB_PROFILE_ACTIVE_SUBSCRIPTION_1)
  {
    if (ds_profile_3gpp2_tech_tbl)
    {
      /* now calculate the real amount */
      if (ds_profile_3gpp2_tech_tbl->get_profiles_range)
      {
        /* first subtract the amount of MAX KDDI profiles on DEFAULT SUBS ONLY */
        non_pers_profile_count[DB_PROFILE_ACTIVE_SUBSCRIPTION_1] -= 
          DS_PROFILE_DB_3GPP2_MAX_KDDI_PROFILE_MAX;
        res = ds_profile_3gpp2_tech_tbl->get_profiles_range(DB_3GPP2_PROFILE_KDDI,
                                                            &min_num, &max_num);


        if (DB_RESULT_SUCCESS == res)
        {
          non_pers_profile_count[DB_PROFILE_ACTIVE_SUBSCRIPTION_1] += max_num - min_num;
        }
      }
      else
      {
        non_pers_profile_count[DB_PROFILE_ACTIVE_SUBSCRIPTION_1] += max_num - min_num;
      }
    }
  }
}

bool TechMgr3GPP2::CanReserveProfileOutsideOfAllocationRange(
  ds_profile_db_profile_num_type num,
  bool during_init /*= false */,
  bool *overwrite /*= NULL */,
  bool is_epc /*= false*/ ) const
{
  if (during_init)
  {
    if (KDDI_or_OMH(num))
    {
      return true;
    }

    if (num == DB_3GPP2_CREATE_DURING_INIT_RANGE_MIN)
    {
      return true;
    }
    return false;
  } 
  else
  {
    PROFILE_DB_LOG_HIGH_3("3GPP2: Can ReserveOutside of Allocation func num %d, during_init %d, is_epc %d", 
	                      num, during_init, is_epc);
    if (KDDI_or_OMH(num))
    {
      return false;
    } 
    else if (num >= TechMgr::ALLOC_RANGE_MIN && num <= TechMgr::ALLOC_RANGE_MAX)
    {
      return true;
    }
    return false;
  }
}

/* enable_deleting_default_profile is currently ignored for this TECH */
bool TechMgr3GPP2::CanDeleteProfile(ds_profile_db_profile_num_type num,
                                    boolean enable_deleting_default_profile) const
{
  if (KDDI_or_OMH(num))
  {
    return false;
  }

  if (enable_deleting_default_profile)
  {
    return true;
  }
  
  bool res = (DB_3GPP2_DEFAULT_PROFILE != num);

  if (!res)
  {
    PROFILE_DB_LOG_ERROR_1("CanDeleteProfile(): can't for this num %d", num);
  }

  return res;
}

ds_profile_db_result_type TechMgr3GPP2::GetParamsFromCache(
  ds_profile_db_profile_num_type num,
  ds_profile_db_subs_type subs,
  void* tech_params, 
  ds_profile_db_gen_params_type* gen_params)
{
  if (tech3GPP2.get_profile)
  {
    return tech3GPP2.get_profile(num, (ds_profile_db_3gpp2_tech_params_type *)tech_params, gen_params, subs);
  }
  else
  {
    if (tech_params)
    {
      memset(tech_params, 0, sizeof(ds_profile_db_3gpp2_tech_params_type));
    }

    if (gen_params)
    {
      memset(gen_params, 0, sizeof(ds_profile_db_gen_params_type));
    }
    return DB_RESULT_SUCCESS;
  }
}

ds_profile_db_result_type TechMgr3GPP2::GetDefaultProfileNum(
  ds_profile_db_profile_num_type * num,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs) const
{
  if (tech3GPP2.get_default_profile_num_per_subs)
  {
    return tech3GPP2.get_default_profile_num_per_subs(num, family, subs);
  }
  return DB_RESULT_ERR_INVAL_PROFILE_FAMILY;
}

ds_profile_db_result_type TechMgr3GPP2::SetDefaultProfileNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs) const
{
  if (tech3GPP2.set_default_profile_num_per_subs)
  {
    return tech3GPP2.set_default_profile_num_per_subs(num, family, subs);
  }
  return DB_RESULT_ERR_INVAL_PROFILE_FAMILY;
}

ds_profile_db_result_type TechMgr3GPP2::GetDefaultProfileNum(
  ds_profile_db_profile_num_type * num,
  ds_profile_db_family_type family) const
{
  if (tech3GPP2.get_default_profile_num)
  {
    return tech3GPP2.get_default_profile_num(num, family);
  }
  else
  {
  return DB_RESULT_ERR_INVAL_PROFILE_FAMILY;
  }

}

ds_profile_db_result_type TechMgr3GPP2::SetDefaultProfileNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_family_type family) const
{
  if (tech3GPP2.set_default_profile_num)
  {
    return tech3GPP2.set_default_profile_num(num, family);
  }
  else
  {
  return DB_RESULT_ERR_INVAL_PROFILE_FAMILY;
  }
}

void TechMgr3GPP2::GetProfilesRange(
  ds_profile_db_profile_num_type* min_num,
  ds_profile_db_profile_num_type * max_num ) const
{
  *min_num = DB_3GPP2_CREATE_DURING_INIT_RANGE_MIN;
  *max_num = ALLOC_RANGE_MAX;
}

bool TechMgr3GPP2::CanWriteProfile(
  ds_profile_db_profile_num_type num) const
{
  return (!KDDI_or_OMH(num));
}

bool TechMgr3GPP2::ShouldBeTreatedAsNonPersistent(
  ds_profile_db_profile_num_type num) const
{
  return KDDI_or_OMH(num);
}

bool TechMgr3GPP2::AllowToCreateFileOnFirstModify(
  ds_profile_db_profile_num_type num) const
{
  return (DB_3GPP2_DEFAULT_PROFILE == num)? true : false;
}

ds_profile_db_result_type TechMgr3GPP2::GetIsEpcParamSet(
   ds_profile_db_profile_num_type num,
   ds_profile_db_ident_type   ident,
  boolean *is_tech_param_set,
  ds_profile_db_subs_type subs) const
{
  if (tech3GPP2.get_is_tech_param_set)
  {
    tech3GPP2.get_is_tech_param_set(num, subs, ident, is_tech_param_set);
  return DB_RESULT_SUCCESS;
  }
  else
  {
  return DB_RESULT_ERR_INVAL_PROFILE_FAMILY;
  }
}

void TechMgr3GPP2::ResetNonPersistentCount(
  ds_profile_db_subs_type subs)
{

  InitNumber(subs);
  return;
}

void TechMgr3GPP2::SetTechTable(tech_func_table_type *techTbl)
{
  if (techTbl)
  {
    tech3GPP2 = *techTbl;
  }
}
