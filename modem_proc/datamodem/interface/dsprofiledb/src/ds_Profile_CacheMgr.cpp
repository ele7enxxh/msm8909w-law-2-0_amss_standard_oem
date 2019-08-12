/*===========================================================================
  FILE: ds_Profile_CacheMgr.cpp

  OVERVIEW: This file provides implementation of the DBMgr class.

  DEPENDENCIES: None

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-01-16 ez Created module
  2013-04-10 am Added implementation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_profile_db.h"
#include "ds_profile_db_internal.h"
#include "ds_Profile_DBMgr.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_TechMgr.h"
#include "ds_Profile_File.h"
#include "ds_Profile_FileEFS.h"
#include "ds_Profile_TechMgr3GPP.h"
#include "ds_Profile_TechMgr3GPP2.h"
#include "ds_Profile_Platform.h"
#include "ds_Profile_Log.h"
#include "ps_system_heap.h"
#include "ds_Profile_NVMgr.h"

extern "C"
{
#include "ds_profile_3gpp2_mgr.h"
}

#include <stringl/stringl.h>

using namespace ds::Profile;




CacheMgr::CacheMgr(void)
{
  for (int i = 0; i < DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX; i++)
  {
    memset(&cache[i], 0,sizeof(cache[i]));
    q_init(&cache[i]);
  }
}

CacheMgr::~CacheMgr(void)
{
  /* release cache */
  for (int subs = 0; subs < DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX; subs++)
  {
    cache_entry_block *cache_block    = NULL;
    /*-----------------------------------------------------------*/
    while ((cache_block = (cache_entry_block*)q_get(&cache[subs])) != NULL)
    {
      if (cache_block->cache.params)
      {
        PS_SYSTEM_HEAP_MEM_FREE(cache_block->cache.params);
      }
      PS_SYSTEM_HEAP_MEM_FREE(cache_block);
    }
    q_destroy(&cache[subs]);
  }
}

CacheMgr::cache_entry* CacheMgr::GetCache(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  bool exact_tech_mask /* = true */
)
{
  cache_entry_block *cache_block = NULL;
  if (DB_RESULT_SUCCESS != Profile::ValidateProfileNum(num))
  {
    return NULL;
  }

  if (DB_RESULT_SUCCESS != Profile::ValidateSubscriptionId(subs))
  {
    return NULL;
  }
  cache_block = (cache_entry_block*)q_check(&cache[subs]);
  while (cache_block)
  {
    if (num == cache_block->cache.num &&
        (exact_tech_mask ? (tech_mask == cache_block->cache.tech_mask) :
                           (DB_IS_SUBSET_OF_TECHMASK(tech_mask, cache_block->cache.tech_mask))))
    {
      return &(cache_block->cache);
    }
    cache_block = (cache_entry_block*)q_next(&cache[subs], &(cache_block->link));
  }

  return NULL;
}

ds_profile_db_result_type CacheMgr::GetCache(cache_entry *c_e, Profile **profile)
{
  /* TODO consider disabling params check for structures received from  MH */
  TechMgr *techMgr = NULL;
  ds_profile_db_gen_params_type gen_params;
  bool first_tech = true;
  Profile *tmpProf = NULL;
  ds_profile_db_result_type res;

  void *tech_params = ps_system_heap_mem_alloc(Profile::GetMaxCStructSize());
  if (NULL == tech_params)
  {
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }
  memset(tech_params, 0, Profile::GetMaxCStructSize());
  memset(&gen_params, 0, sizeof(ds_profile_db_gen_params_type));
  /*TODO ADD support for Subs id setting here.*/
  DB_FOR_EVERY_TECH_IN_MASK_DO_START(c_e->tech_mask, tech)

  if ((techMgr = DBMgr::Instance()->GetTechManager(tech)) != NULL)
  {
    if (first_tech)
    {

      if (DB_RESULT_SUCCESS != (res = techMgr->GetParamsFromCache(c_e->num, c_e->subs, tech_params, &gen_params)))
      {
        goto bail;
      }
      
      if (!(tmpProf = Profile::CreateInstanceWithoutValidation(tech, tech_params, &gen_params)))
      {
        res = DB_RESULT_ERR_OUT_OF_PROFILES;
        goto bail;
      }

      first_tech = false;

    } 
    else
    {

      /* gen_params is ignored in this case */
      if (DB_RESULT_SUCCESS != (res = techMgr->GetParamsFromCache(c_e->num, c_e->subs, tech_params, &gen_params)))
      {
        goto bail;
      }
      if (DB_RESULT_SUCCESS != (res = tmpProf->AddSupportedTechWithoutValidation(tech, tech_params)))
      {
        goto bail;
      }

    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  if (c_e->tech_mask & DB_TECH_TO_TECHMASK(DB_TECH_COMMON))
  {
    if (!tmpProf)
    {
      res = DB_RESULT_FAIL;
      goto bail;
    }

    if (DB_RESULT_SUCCESS != (res = tmpProf->AddSupportedTech(DB_TECH_COMMON, c_e->params)))
    {
      goto bail;
    }
  }
  if (tmpProf)
  {
    *profile = tmpProf;
  }
  else
  {
    res = DB_RESULT_FAIL;
    goto bail;
  }

  if (tech_params)
  {
    PS_SYSTEM_HEAP_MEM_FREE(tech_params);
  }

  return DB_RESULT_SUCCESS;

bail:

  if (tech_params)
  {
    PS_SYSTEM_HEAP_MEM_FREE(tech_params);
  }

  if (tmpProf)
  {
    delete tmpProf;
    tmpProf = NULL;
  }

  return res;
}




/* provided a profile number, retrieve Profile from cache 
   (currently all cache except for common params part is fetched from mode handlers) */
ds_profile_db_result_type CacheMgr::GetCache(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  Profile **profile,
  bool exact_tech_mask /*= true*/)
{
  cache_entry *c_e = GetCache(num, tech_mask, subs, exact_tech_mask);
  if (!c_e)
  {
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  return GetCache(c_e, profile);
}

ds_profile_db_result_type CacheMgr::UpdateCache(const Profile *profile)
{
  ds_profile_db_subs_type subs;
  ds_profile_db_result_type res;
  cache_entry_block* cache_block = NULL;

  subs = profile->GetSubcriptionId();

  if (DB_RESULT_SUCCESS != (res = Profile::ValidateSubscriptionId(subs)))
  {
    return res;
  }
  cache_block = (cache_entry_block*)q_check(&cache[subs]);
  while (cache_block!= NULL)
  {
    if (profile->GetNum() == cache_block->cache.num &&
        profile->GetSupportedTechMask() == cache_block->cache.tech_mask)
    {
      if (cache_block->cache.tech_mask & DB_TECH_TO_TECHMASK(DB_TECH_COMMON))
      {
        if (!cache_block->cache.params)
        {
          cache_block->cache.params = (ds_profile_db_common_tech_params_type *)
            ps_system_heap_mem_alloc(sizeof(ds_profile_db_common_tech_params_type));
          if (!cache_block->cache.params)
          {
            return DB_RESULT_ERR_OUT_OF_MEMORY;
          }
        }

        ds_profile_db_result_type res = profile->GetTechParams(DB_TECH_COMMON, cache_block->cache.params);
        if (DB_RESULT_SUCCESS != res)
        {
          PS_SYSTEM_HEAP_MEM_FREE(cache_block->cache.params);
        }
        return res;

      } 
      else
      {
        if (cache_block->cache.params)
        {
          PS_SYSTEM_HEAP_MEM_FREE(cache_block->cache.params);
          cache_block->cache.params = NULL;
        }
        return DB_RESULT_SUCCESS;
      }
    } /* end if */
    cache_block = (cache_entry_block*)q_next(&cache[subs], &(cache_block->link));
  }

  return DB_RESULT_ERR_INVAL_PROFILE_NUM;
}

/* creates local cache entry in DB
   currently stores only common params and very basic profile info 
   (num, supported tech's, etc) 
   MH cache is not updated here, it is done via callbacks
 */
ds_profile_db_result_type CacheMgr::CreateCache(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  bool is_persistent,
  ds_profile_db_common_tech_params_type *common)
{
  ds_profile_db_common_tech_params_type *common_params = NULL;
  ds_profile_db_result_type res;
  cache_entry default_entry;
  cache_entry_block *cache_block = NULL;

   PS_SYSTEM_HEAP_MEM_ALLOC(cache_block, sizeof(cache_entry_block),
                           cache_entry_block*);
  if(!cache_block)
  {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    return res;
  }

  memset(cache_block, 0, sizeof(cache_entry_block));
  cache_block->cache = default_entry;
  cache_block->cache.num = num;
  cache_block->cache.tech_mask = tech_mask;
  cache_block->cache.subs = subs;
  cache_block->cache.is_persistent = is_persistent; 
  if (cache_block->cache.tech_mask & DB_TECH_TO_TECHMASK(DB_TECH_COMMON) &&
          common)
      {

        common_params = (ds_profile_db_common_tech_params_type *)
          ps_system_heap_mem_alloc(sizeof(ds_profile_db_common_tech_params_type));
        if (!common_params)
        {
          res = DB_RESULT_ERR_OUT_OF_MEMORY;
      cache_block->cache = default_entry;
          goto bail;
        }

        *common_params = *common;
    cache_block->cache.params = common_params;
      }


  (void) q_link ( cache_block, &cache_block->link );
  /* Add it to the queue of cache*/
  q_put((&cache[subs]), &(cache_block->link));
      return DB_RESULT_SUCCESS;

  /* fail through */

bail:
  if (common_params)
  {
    PS_SYSTEM_HEAP_MEM_FREE(common_params);
  }
  if (cache_block)
  {
    PS_SYSTEM_HEAP_MEM_FREE(cache_block);
  }

  return res;
}

/* creates local cache entry in DB
   currently stores only common params and very basic profile info 
   (num, supported tech's, etc) 
   MH cache is not updated here, it is done via callbacks
 */
ds_profile_db_result_type CacheMgr::CreateCache(const Profile *profile)
{
  ds_profile_db_common_tech_params_type common;
  ds_profile_db_common_tech_params_type *common_ptr = NULL;
  ds_profile_db_result_type res;
  if (profile->GetSupportedTechMask() & DB_TECH_TO_TECHMASK(DB_TECH_COMMON))
  {
    if (DB_RESULT_SUCCESS != (res = profile->GetTechParams(DB_TECH_COMMON, &common)))
    {
      return res;
    }
    common_ptr = &common;
  }

  return CreateCache(profile->GetNum(),
                     profile->GetSupportedTechMask(),
                     profile->GetSubcriptionId(),
                     !DBMgr::Instance()->ShouldBeTreatedAsNonPersistent(profile),
                     common_ptr);

}

ds_profile_db_result_type CacheMgr::UpdateOrCreateCache(
  const Profile *profile,
  bool overwrite, /* = false */
  bool *new_profile /* = NULL */)
{
  ds_profile_db_result_type res = DB_RESULT_SUCCESS;

  /* first try to update existing cache */
  if (overwrite)
  {
    res = UpdateCache(profile);
    if (DB_RESULT_SUCCESS == res)
    {
      if (new_profile)
      {
        *new_profile = false;
      }
      return DB_RESULT_SUCCESS;
    }
  } else
  {
    /* we are not allowed to overwrite the existing profile, only to create a new one */
    if (GetCache(profile->GetNum(),
                 profile->GetSupportedTechMask(),
                 profile->GetSubcriptionId()))
    {
      return DB_RESULT_ERR_INVAL_PROFILE_NUM;
    }
  }

  if (DB_RESULT_ERR_INVAL_PROFILE_NUM != res && DB_RESULT_SUCCESS != res)
  {
    return res;
  }

  if (new_profile)
  {
    *new_profile = true;
  }

  return CreateCache(profile);
}

ds_profile_db_result_type CacheMgr::RemoveCache(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs)
{
  cache_entry_block  *cache_block = NULL;
  cache_entry_block  *prev_block  = NULL;
/*-------------------------------------------------------*/
  cache_block = (cache_entry_block*)q_check(&cache[subs]);

  while (cache_block != NULL)
  {
    if (num == cache_block->cache.num && tech_mask == cache_block->cache.tech_mask)
    {
      prev_block = cache_block;
      cache_block = (cache_entry_block*)q_next(&cache[subs], &(cache_block->link));
      if (prev_block->cache.params)
      {
        PS_SYSTEM_HEAP_MEM_FREE(prev_block->cache.params);
      }
      q_delete(&cache[subs], &(prev_block->link));
      PS_SYSTEM_HEAP_MEM_FREE(prev_block);
      return DB_RESULT_SUCCESS;
    }
    else
    {
      cache_block = (cache_entry_block*)q_next(&cache[subs], &(cache_block->link));
    }
  }
  return DB_RESULT_ERR_INVAL_PROFILE_NUM;
}

uint8 CacheMgr::GetNumOfCacheEntry(ds_profile_db_supported_tech_mask tech_mask,
                                   ds_profile_db_subs_type subs)
{
  return q_cnt(&cache[subs]);
}

ds_profile_db_result_type CacheMgr::GetProfilesFromInternalCache
(
  Profile ***profiles,
  uint16 *count,
  uint16 tech_mask,
  ds_profile_db_ident_type ident,
  const ds_profile_db_info_type *param_info,
  ds_profile_db_subs_type subs
)
{
  Profile *tmpProf = NULL;
  ds_profile_db_result_type res = DB_RESULT_SUCCESS;
  cache_entry_block *cache_block = NULL;
  ds_profile_db_profile_num_type omh_min_num = 0;
  ds_profile_db_profile_num_type omh_max_num = 0;
  uint32 num_omh_profiles = 0;
/*----------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
  res = ds_profile_3gpp2_mgr_get_profiles_range(DB_3GPP2_PROFILE_OMH,
                                                &omh_min_num,
                                                &omh_max_num);

  if (DB_RESULT_SUCCESS != res)
  {
    PROFILE_DB_LOG_ERROR("Error in getting OMH profile range!!!");
  }

  num_omh_profiles = ds_profile_3gpp2_mgr_get_num_valid_omh_profiles();
#endif /*FEATURE_DATA_IS707*/
  cache_block = (cache_entry_block *)q_check(&cache[subs]);
  while(cache_block != NULL)
  {
    tmpProf = NULL;
    if (DB_UNSPECIFIED_PROFILE_NUM != cache_block->cache.num)
    {
      if (tech_mask == cache_block->cache.tech_mask)
      {
        if (!num_omh_profiles &&
           (cache_block->cache.num >= omh_min_num 
            && cache_block->cache.num <= omh_max_num))
        {
          cache_block = (cache_entry_block *)q_next(&cache[subs], &(cache_block->link));
          continue;
        }
        res = GetCache(&(cache_block->cache), &(tmpProf));
        if (DB_RESULT_SUCCESS != res)
        {
          /* for OMH profiles we hold entries in DB table even though they 
             might not exist in 3GPP2 MH cache */
          if (DB_RESULT_ERR_3GPP2_OMH_PROFILE_NOT_VALID == res)
          {
			      cache_block = (cache_entry_block *)q_next(&cache[subs], &(cache_block->link));
            continue;
          } 
          else
          {
            return res;
          }
        }

        if (INVALID_PROFILE_PARAM != ident)
        {
          if (tmpProf)
          {
            if (!tmpProf->CompareParam(ident, param_info))
            {
              delete tmpProf;
              tmpProf = NULL;
	      cache_block = (cache_entry_block *)q_next(&cache[subs], &(cache_block->link));
              continue;
            }
          }
        }
        if (tmpProf)
        {
          tmpProf->SetSubscriptionId(subs);
          (*profiles)[*count] = tmpProf;
          (*count)++;
        }
        else
        {
          PROFILE_DB_LOG_ERROR("GetProfilesFromInternalCache could not get temp Profile");
        }
      }
    }
    cache_block = (cache_entry_block *)q_next(&cache[subs], &(cache_block->link));
  }

  return res;
}


ds_profile_db_result_type CacheMgr::GetPersistence(ds_profile_db_profile_num_type num,
                                                   ds_profile_db_subs_type subs,
                                                   boolean *is_persistent)
{
  TechMgr *techMgr = NULL;
  ds_profile_db_gen_params_type profile_general_params;
  bool first_tech = true;
  void *tech_params = NULL;
  ds_profile_db_result_type status = DB_RESULT_FAIL;
  cache_entry_block *cache_block = NULL;
/*---------------------------------------------------------------------------------------*/

  cache_block = (cache_entry_block *)q_check(&cache[subs]);

  while (cache_block)
  {
    if (num == cache_block->cache.num)
    {
      tech_params = ps_system_heap_mem_alloc(Profile::GetMaxCStructSize());
      if (NULL == tech_params)
      {
        return DB_RESULT_ERR_OUT_OF_MEMORY;
      }

      DB_FOR_EVERY_TECH_IN_MASK_DO_START(cache_block->cache.tech_mask, tech)
      if ((techMgr = DBMgr::Instance()->GetTechManager(tech)))
      {
        if (first_tech)
        {
          if (DB_RESULT_SUCCESS != (status =
                                    techMgr->GetParamsFromCache(num, subs,
                                                                tech_params,
                                                                &profile_general_params)))
          {
            goto bail;
          }
          first_tech = false;
          if (is_persistent)
          {
            *is_persistent = profile_general_params.persistent;
          } 
          else
          {
            goto bail;
          }
        } 
        else
        {
          break; /*add comments*/
        }
      }
      DB_FOR_EVERY_TECH_IN_MASK_DO_END
      break;
    }
    cache_block = (cache_entry_block*)q_next(&cache[subs], &(cache_block->link));
  }

bail:
  if (tech_params)
  {
    PS_SYSTEM_HEAP_MEM_FREE(tech_params);
  }
  return status;
}

ds_profile_db_tech_type CacheMgr::GetTechType
(
  ds_profile_db_profile_num_type num,
  ds_profile_db_subs_type subs
)
{
  ds_profile_db_tech_type tech = DB_TECH_INVALID;
  cache_entry_block *cache_block = NULL;
/*---------------------------------------------------------------------------------------*/

  cache_block = (cache_entry_block *)q_check(&cache[subs]);

  while (cache_block)
  {
    if (num == cache_block->cache.num)
    {
      tech = ds_profile_db_tech_mask_to_tech(cache_block->cache.tech_mask);
      break;
    }
    cache_block = (cache_entry_block*)q_next(&cache[subs], &(cache_block->link));
  }
  return tech;
}


ds_profile_db_result_type CacheMgr::IsValidProfile
(
  ds_profile_db_profile_num_type num,
  ds_profile_db_subs_type        subs,
  ds_profile_db_tech_type        tech_type,
  boolean                        *is_valid
)
{
  cache_entry_block *cache_block = NULL;
/*---------------------------------------------------------------------------------------*/

  cache_block = (cache_entry_block *)q_check(&cache[subs]);

  while (cache_block)
  {
    if (num == cache_block->cache.num &&
        tech_type == ds_profile_db_tech_mask_to_tech(cache_block->cache.tech_mask))
    {
      *is_valid = true;
       break;
    }
    cache_block = (cache_entry_block*)q_next(&cache[subs], &(cache_block->link));
  }
  return DB_RESULT_SUCCESS;
}

void CacheMgr::RemoveCacheForSubs(
  ds_profile_db_subs_type subs,
  ds_profile_db_num_tech_type *removed_entries,
  uint32 *count
)
{
  int tmp_count = 0;
  cache_entry default_entry;

  cache_entry_block *cache_block    = NULL;
  /*-----------------------------------------------------------*/
  while ((cache_block = (cache_entry_block*)q_get(&cache[subs])) != NULL)
      {
    removed_entries[tmp_count].profile_num = cache_block->cache.num;
    removed_entries[tmp_count].tech_mask = cache_block->cache.tech_mask;
    removed_entries[tmp_count].is_persistent = cache_block->cache.is_persistent; 
    if (cache_block->cache.params)
    {
      PS_SYSTEM_HEAP_MEM_FREE(cache_block->cache.params);
      cache_block->cache.params = NULL;
    }
    PS_SYSTEM_HEAP_MEM_FREE(cache_block);
      tmp_count++;
    }
  *count = tmp_count;
}
