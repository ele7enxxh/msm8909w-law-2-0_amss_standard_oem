/*===========================================================================
  @file ds_Profile_Profile.cpp

  This file implements Profile abstract class which is responsible for all the operations
  on the local copy of the Profile (alloc, get/set param, etc)

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
  2013-03-02 am Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Profile_ParamsGeneral.h"
#include "ds_Profile_Log.h"
#include "ds_profile_db.h"
#include "ds_Profile_TechMgr.h"

extern "C"
{
#include "ds3gprofile_validator.h"
#include "ds_3gpp_profile_api.h"
#include "ps_sys.h"
}

#include <stringl/stringl.h>

using namespace ds::Profile;

#define DB_FILL_PARAM_MAP_GENERAL_INIT() DB_FILL_PARAM_MAP_INIT(ds_profile_db_gen_params_type);

/*===========================================================================

                    STATIC MEMBER INIT

===========================================================================*/
Params::ParamInfo 
  ParamsGeneral::mapParamsInfo[MAP_END_RANGE - MAP_START_RANGE + 1] = {0};
/*===========================================================================

                     INTERNAL HELPER FUNCTIONS

===========================================================================*/

/*===========================================================================

                     INTERNAL VALIDATION FUNCTIONS

===========================================================================*/
static ds_profile_db_result_type 
ValidateSubscriptionIdInternal(const ds_profile_db_info_type*  info,
                               const void*                     profile_params,
                               ...)
{
  ds_profile_db_subs_type subscription_id;
  if (!info->len)
  {
    return DB_RESULT_ERR_LEN_INVALID;
  }

  memset(&subscription_id, 0, sizeof(subscription_id));
  memscpy(&subscription_id, sizeof(subscription_id), info->buf, info->len);

  ParamsGeneral::ValidateSubscriptionId(subscription_id);

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type 
ValidateGeneral(const ds_profile_db_gen_params_type* profile) 
{
  ds_profile_db_result_type res = ParamsGeneral::ValidateTechMask(profile->tech_mask);
  if (DB_RESULT_SUCCESS != res)
  {
    return res;
  }

  res = ParamsGeneral::ValidateProfileNum(profile->profile_number);
  if (DB_RESULT_SUCCESS != res)
  {
    return res;
  }

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type 
ValidateIdent(
  ds_profile_db_ident_type        ident, 
  const ds_profile_db_info_type*  info,
  const void*                     profile_params,
  ...)
{
  switch (ident)
  {
    case(GENERAL_PROFILE_PARAM_SUBS_ID):
      return ValidateSubscriptionIdInternal(info, profile_params);
    case(GENERAL_PROFILE_PARAM_APN_NAME):
      if (info != NULL && 
            (ds3g_profile_validate_apn_name((char*)(info->buf), info->len) == TRUE))
      {
        return DB_RESULT_SUCCESS;   
      }
      else
      {
        return DB_RESULT_FAIL;
      }
    default:
      return DB_RESULT_SUCCESS;    
  }
}
/*===========================================================================

                     INTERNAL SPECIAL SET FUNCTIONS

===========================================================================*/

/*===========================================================================

                     INTERNAL SPECIAL GET FUNCTIONS

===========================================================================*/
static ds_profile_db_result_type SpecialGetProfileName(
  ds_profile_db_info_type* info,
  const void*              params)
{
  ds_profile_db_gen_params_type* gen_params = 
    (ds_profile_db_gen_params_type*) params;

  info->len = strnlen(gen_params->profile_name, sizeof(gen_params->profile_name));
  
  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGetApnName(
  ds_profile_db_info_type*       info,
  const void*                    params)
{
  ds_profile_db_gen_params_type* profile =
    (ds_profile_db_gen_params_type*)params;

  info->len = strnlen(profile->apn_name, sizeof(profile->apn_name));

  return DB_RESULT_SUCCESS;
}

static ds_profile_db_result_type SpecialGet(
  ds_profile_db_ident_type ident, 
  ds_profile_db_info_type* info,
  const void*              params)
{  
  switch (ident)
  {    
    case(GENERAL_PROFILE_PARAM_PROFILE_NAME):
      return SpecialGetProfileName(info, params); 
    case(GENERAL_PROFILE_PARAM_APN_NAME):
      return SpecialGetApnName(info, params);
    default:
      return DB_RESULT_SUCCESS;;
  }
}
/*===========================================================================

                     INTERNAL COMPARE FUNCTIONS

===========================================================================*/
static bool CompareIdent(
  ds_profile_db_ident_type ident, 
  const ds_profile_db_info_type* info1,
  const ds_profile_db_info_type* info2)
{
  switch (ident)
  {
    case(GENERAL_PROFILE_PARAM_APN_NAME):
      return (strncasecmp((const char*)info1->buf, (const char*)info2->buf, info1->len) == 0) ? true : false;
    default:
      return (memcmp(info1->buf, info2->buf, info1->len) == 0) ? true : false;
  }
}

/*===========================================================================

                    MEMBER FUNCTIONS

===========================================================================*/
ds_profile_db_result_type ParamsGeneral::Init(void)
{
  DB_FILL_PARAM_MAP_GENERAL_INIT();

  DB_FILL_PARAM_MAP(GENERAL_PROFILE_PARAM_PROFILE_NAME,
                    profile_name);

  DB_FILL_PARAM_MAP(GENERAL_PROFILE_PARAM_APN_NAME,
                    apn_name);

  /* read only param */
  DB_FILL_PARAM_MAP(GENERAL_PROFILE_PARAM_SUBS_ID,
                    subscription_id);

  DB_FILL_PARAM_MAP(GENERAL_PROFILE_PARAM_IPV6_DELEGATION,
	            ipv6_delegation);

  DB_FILL_PARAM_MAP(GENERAL_PROFILE_PARAM_CLAT_ENABLE_FLAG,
                    clat_enabled);

  return DB_RESULT_SUCCESS;
}

ParamsGeneral* ParamsGeneral::
CreateInstance(ds_profile_db_tech_type tech,
               void* params /* = NULL */)
{
  ParamsGeneral* paramsGeneral = NULL;

  if (DB_RESULT_SUCCESS != ValidateTech(tech))
  {
    return NULL;
  }

  if (params && DB_RESULT_SUCCESS != ValidateCParams(params))
  {
    return NULL;    
  }

  if (params)
  {
    paramsGeneral = new ParamsGeneral((ds_profile_db_gen_params_type*)params);
  } 
  else
  {
    paramsGeneral = new ParamsGeneral(tech);
  }

  if (!paramsGeneral)
  {
    return NULL;
  }
  if (DB_RESULT_SUCCESS != paramsGeneral->InitInstance())
  {
    delete paramsGeneral;
    return NULL;
  }
  
  return paramsGeneral;
}

ParamsGeneral::ParamsGeneral(const ParamsGeneral& paramsGeneral):params(paramsGeneral.params) 
{}

ParamsGeneral::ParamsGeneral(const ds_profile_db_gen_params_type* gen_params):params(*gen_params) 
{}

ParamsGeneral::ParamsGeneral(ds_profile_db_tech_type tech)
{  
  GetDefaultCParams(&params);
  params.tech_mask = DB_TECH_TO_TECHMASK(tech);
}

ParamsGeneral* ParamsGeneral::Clone(void) const
{
  ParamsGeneral* paramsGeneral = 
    new ParamsGeneral(*this);

  if (!paramsGeneral)
  {
    return NULL;
  }

  if (DB_RESULT_SUCCESS != paramsGeneral->InitInstance(changed_idents_list))
  {
    delete paramsGeneral;
    return NULL;
  }

  return paramsGeneral;
}

void ParamsGeneral::GetDefaultCParams(ds_profile_db_gen_params_type* params)
{
  if (!params)
  {
    return;
  }

  ds_profile_db_gen_params_type* gen_params = (ds_profile_db_gen_params_type*)params;

  memset(params, 0, sizeof(ds_profile_db_gen_params_type));
 
  gen_params->profile_number = DB_UNSPECIFIED_PROFILE_NUM;
  gen_params->persistent = true;
  gen_params->subscription_id = GetDefaultSubscriptionId();
  gen_params->ipv6_delegation = false;
  gen_params->clat_enabled = false;
}

ds_profile_db_result_type ParamsGeneral::
ValidateCParams(void* params)
{
  ds_profile_db_result_type res;

  PROFILE_DB_LOG_MED_1("ValidateCParams(): for params 0x%x", params);

  if (!params)
  {
    return DB_RESULT_ERR_INVAL;
  }

  /* perform general profile checks not related to a specific parameter */
  if (DB_RESULT_SUCCESS != 
        (res = ValidateGeneral((const ds_profile_db_gen_params_type *)params)))
  {
      return res;
  } 
  
  return Params::ValidateCParams(&ValidateIdent, MAP_START_RANGE, mapParamsInfo, DB_ARRAY_NUM_ELEM(mapParamsInfo), params);
}

ds_profile_db_supported_tech_mask ParamsGeneral::GetSupportedTechMask(void) const
{
  return params.tech_mask;
}

void ParamsGeneral::AddSupportedTech(ds_profile_db_tech_type tech)
{
  params.tech_mask |= DB_TECH_TO_TECHMASK(tech);
}

const void* ParamsGeneral::GetCParamsPtr(void) const
{
  return &params;
} 

Params::ParamInfo* const
ParamsGeneral::GetMapParamsPtr(void) const
{
  return mapParamsInfo;
}

size_t ParamsGeneral::GetCStructSize(void) const
{
  return sizeof(params);
}

ds_profile_db_result_type ParamsGeneral::
ValidateTech(ds_profile_db_tech_type tech_type) 
{
  if (tech_type >= DB_TECH_MAX)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateTech(): invalid tech %d", tech_type);
    return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type ParamsGeneral::
ValidateTechMask(ds_profile_db_supported_tech_mask tech_mask)
{
   if (!tech_mask || tech_mask > DB_MAX_SUPPORTED_TECHMASK)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateTechMask(): invalid tech_mask %d", tech_mask);
    return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
  }
  return DB_RESULT_SUCCESS;
}
  
ds_profile_db_result_type ParamsGeneral::
ValidateSupportedTechMask(ds_profile_db_supported_tech_mask tech_mask)
{
  ds_profile_db_tech_type tech  = 
    ds_profile_db_tech_mask_to_tech(tech_mask);

  if (tech == DB_TECH_3GPP || tech == DB_TECH_3GPP2)
  {
    if(ds_profile_db_get_tech_tbl(tech))
    {
      return DB_RESULT_SUCCESS;
    }
    else
    {
      PROFILE_DB_LOG_ERROR_1("ValidateTechMask(): Tech type not supported %d", tech_mask);
      return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
    }
  }
  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type ParamsGeneral::ValidateProfileNumPerTech(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask)
{
  switch (tech_mask)
  {
    case DB_TECH_TO_TECHMASK(DB_TECH_3GPP):
      if (!(num >= DB_3GPP_CREATE_RANGE_MIN && num <= DB_3GPP_CREATE_RANGE_MAX))
      {
        return DB_RESULT_ERR_INVAL_PROFILE_NUM;
      }
      break;
    case DB_TECH_TO_TECHMASK(DB_TECH_3GPP2):
      if ((!(num >= TechMgr::ALLOC_RANGE_MIN &&
             num <= TechMgr::ALLOC_RANGE_MAX)) &&
          (num != DB_3GPP2_DEFAULT_PROFILE))
      {
        return DB_RESULT_ERR_INVAL_PROFILE_NUM;
      }
      break; 
    case EPC_TECH_MASK:
      if (!(num >= TechMgr::ALLOC_RANGE_MIN && 
            num <= TechMgr::ALLOC_RANGE_MAX))
      {
        return DB_RESULT_ERR_INVAL_PROFILE_NUM;
      }
      break;
    default:
      return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
      break;
  }
  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type ParamsGeneral::
ValidateProfileNum(ds_profile_db_profile_num_type num)
{
  //Validation of number is done during Allocation of Number in TechMgr
  return DB_RESULT_SUCCESS;
}

ds_profile_db_profile_num_type ParamsGeneral::GetNum(void) const
{
  return params.profile_number;
}

bool ParamsGeneral::IsPersistent(void) const
{
  //  to avoid warning C4800: conversion between boolean to bool
  return ((params.persistent) != 0); 
}

void ParamsGeneral::SetNum(ds_profile_db_profile_num_type profile_number) 
{
  params.profile_number = profile_number;
}

ds_profile_db_result_type ParamsGeneral::ValidateSubscriptionId( ds_profile_db_subs_type subs )
{  
  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return DB_RESULT_ERR_INVAL_SUBS_ID;
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_subs_type ParamsGeneral::GetDefaultSubscriptionId(void)
{ 
  return (ds_profile_db_subs_type)ps_sys_get_default_data_subscription();	
}

void ParamsGeneral::SetPersistency(bool persistent)
{
  params.persistent = persistent;
}


ds_profile_db_result_type ParamsGeneral::SetSubscriptionId( ds_profile_db_subs_type subsciption_id, bool track)
{
  ds_profile_db_result_type res;

  if (track)
  {
    if (DB_RESULT_SUCCESS != (res = TrackParamChange(GENERAL_PROFILE_PARAM_SUBS_ID)))
    {
      return res;
    }
  }

  params.subscription_id = subsciption_id;

  return DB_RESULT_SUCCESS;
}


ds_profile_db_result_type ParamsGeneral::SetIPV6Delegation( boolean ipv6_delegation, bool track)
{
  ds_profile_db_result_type res;

  if (track)
  {
    if (DB_RESULT_SUCCESS != (res = TrackParamChange(GENERAL_PROFILE_PARAM_IPV6_DELEGATION)))
    {
      return res;
    }
  }

  params.ipv6_delegation = ipv6_delegation;

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type ParamsGeneral::SetCLATEnable( boolean clat_enabled, bool track)
{
  ds_profile_db_result_type res;

  if (track)
  {
    if (DB_RESULT_SUCCESS != (res = TrackParamChange(GENERAL_PROFILE_PARAM_CLAT_ENABLE_FLAG)))
    {
      return res;
    }
  }

  params.clat_enabled = clat_enabled;

  return DB_RESULT_SUCCESS;
}

int ParamsGeneral::IdentToIndex(ds_profile_db_ident_type ident) const 
{
  if (MAP_START_RANGE <= ident &&
      MAP_END_RANGE   >= ident)
  {
    return ident - MAP_START_RANGE;
  }

  return -1;
}

validate_fn_type ParamsGeneral::GetValidateFn(void) const
{
  return &ValidateIdent;
}

Params::set_fn_type ParamsGeneral::GetSpecialSetFn(void) const
{
  return NULL;
}

Params::get_fn_type ParamsGeneral::GetSpecialGetFn(void) const
{
  return &SpecialGet;
}

Params::compare_fn_type ParamsGeneral::GetCompareFn(void) const
{
  return &CompareIdent;
}

bool ParamsGeneral::IsParamReadOnly( ds_profile_db_ident_type ident ) const
{
  switch (ident)
  {
    case (GENERAL_PROFILE_PARAM_SUBS_ID):
      return true;
    default:
  return false;
}
}

ds_profile_db_subs_type ParamsGeneral::GetSubcriptionId(void)
{
  return params.subscription_id;
}

boolean ParamsGeneral::GetIPV6Delegation(void)
{
  return params.ipv6_delegation;
}

boolean ParamsGeneral::GetCLATEnable(void)
{
  return params.clat_enabled;
}