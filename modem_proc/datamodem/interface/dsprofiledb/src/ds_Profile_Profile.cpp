/*===========================================================================
  @file ds_Profile_Profile.cpp

  This file implements Profile class which is responsible for all the operations
  on the local copy of the Profile (alloc, get/set param, etc)
  Each profile is a container for several Param subcomponents which implement
  the functionality for a distinct block of parameters. 
  Profile always has ParamsGeneral and at least one technology specific Params
  components. Additional Params technology components will be added whenever
  user tries to set a param belonging to this technology


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
  2013-04-26 am Design reworked to support EPC and complex profile types

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_profile_os.h"
#include "ds_profile_db.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_Params.h"
#include "ds_Profile_ParamsGeneral.h"
#include "ds_Profile_ParamsCommonTech.h"
#include "ds_Profile_Params3GPPTech.h"
#include "ds_Profile_Params3GPP2Tech.h"
#include "ds_Profile_ParamsTemplateTech.h"
#include "ds_Profile_Log.h"
#include <stringl/stringl.h>
#include "ps_system_heap.h"
#include <stdarg.h>

extern "C"
{
#include "ds_util.h"
}

#include <stdlib.h>

using namespace ds::Profile;
/*===========================================================================

                    INTERNAL DEFINITIONS

===========================================================================*/
typedef struct {
  ParamsTech* (*CreateInstance)
    (const ParamsGeneral* paramsGen, const void* params);

  void (*GetDefaultCParams)(void* params);

  void (*GetIdentsRange)(
    ds_profile_db_ident_type* start,                          
    ds_profile_db_ident_type* end);

  void (*SetValidationRequired)(
    bool is_validation_reqd);

  void (*SetValidationFunction)(
    validate_fn_type tech_validation_func);

  uint16 struct_size;
} TechInfo;
/*===========================================================================

                    STATIC MEMBER INIT

===========================================================================*/
static TechInfo techInfoArray[DB_TECH_MAX] = {{0}};
/*===========================================================================

                     INTERNAL FUNCTIONS

===========================================================================*/
template <class TechClass>
ds_profile_db_result_type Profile::InitTech(ds_profile_db_tech_type tech)
{
  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = TechClass::Init()))
  {
    return res;
  }

  techInfoArray[tech].CreateInstance    = &TechClass::CreateInstance;
  techInfoArray[tech].GetDefaultCParams = &TechClass::GetDefaultCParams;
  techInfoArray[tech].GetIdentsRange    = &TechClass::GetIdentsRange;
  techInfoArray[tech].struct_size       = TechClass::params_size;
  techInfoArray[tech].SetValidationRequired = &TechClass::SetValidationReqd;
  techInfoArray[tech].SetValidationFunction = &TechClass::SetValidationFunction;

  return DB_RESULT_SUCCESS;
}        

inline static TechInfo* GetTechInfo(ds_profile_db_tech_type tech)
{
  return &(techInfoArray[tech]);
}
/*===========================================================================

                    MEMBER FUNCTIONS

===========================================================================*/

ds_profile_db_result_type Profile::Init(void)
{
  PROFILE_DB_LOG_HIGH("Init(): started");

  ds_profile_db_result_type res;
  /* should be inited first */
  if (DB_RESULT_SUCCESS != (res = ParamsGeneral::Init()))
  {
    PROFILE_DB_LOG_ERROR_1("Init(): could not init ParamsGeneral class, error %d", res);
    return res;
  }

  /* should be inited second */
  if (DB_RESULT_SUCCESS != 
        (res = InitTech<ParamsCommonTechConcrete>(DB_TECH_COMMON)))
  {
    PROFILE_DB_LOG_ERROR_1("Init(): could not init ParamsCommonTechConcrete class, error %d", res);
    return res;
  }

  /* all the rest */
  if (DB_RESULT_SUCCESS != 
        (res = InitTech<Params3GPPTechConcrete>(DB_TECH_3GPP)))
  {
    PROFILE_DB_LOG_ERROR_1("Init(): could not init Params3GPPTechConcrete class, error %d", res);
    return res;
  }

  if (DB_RESULT_SUCCESS != 
        (res = InitTech<Params3GPP2TechConcrete>(DB_TECH_3GPP2)))
  {
    PROFILE_DB_LOG_ERROR_1("Init(): could not init Params3GPP2TechConcretey class, error %d", res);
    return res;
  }  
  return DB_RESULT_SUCCESS;
}

void Profile::SetValidationFunction()
{
  tech_func_table_type* tech_table_3gpp = ds_profile_db_get_tech_tbl(DB_TECH_3GPP);
  tech_func_table_type* tech_table_3gpp2 = ds_profile_db_get_tech_tbl(DB_TECH_3GPP2);
  if (tech_table_3gpp)
  {
    GetTechInfo(DB_TECH_3GPP)->SetValidationFunction(tech_table_3gpp->validation_function);
  }
  if (tech_table_3gpp2)
  {
    GetTechInfo(DB_TECH_3GPP2)->SetValidationFunction(tech_table_3gpp2->validation_function);
  }
}

Profile* Profile::CreateInstance(ds_profile_db_supported_tech_mask tech_mask,
                                 ds_profile_db_subs_type subs)
{
  PROFILE_DB_LOG_MED_1("CreateInstance(): tech_mask %d", tech_mask);

  if (DB_RESULT_SUCCESS != ParamsGeneral::ValidateTechMask(tech_mask))
  {
    return NULL;
  }

  Profile* profile = NULL;

  DB_FOR_EVERY_TECH_IN_MASK_DO_START(tech_mask, tech)
  if (!profile)
  {
    profile = CreateInstance(tech);
    if (!profile)
    {
      return NULL;
    }
  } else
  {
    if (DB_RESULT_SUCCESS != profile->AddSupportedTech(tech))
    {
      delete profile;
      profile = NULL;
      return NULL;
    }      
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END
    profile->SetSubscriptionId(subs);
  return profile;
}


Profile* Profile::CreateInstance(
  ds_profile_db_tech_type                   tech_type,
  void*                                     tech_params, /* = NULL */
  ds_profile_db_gen_params_type*            gen_params /* = NULL */)
{
  PROFILE_DB_LOG_MED_3("CreateInstance(): tech %d, tech_params 0x%p, gen_params 0x%p", 
                       tech_type, tech_params, gen_params);

  if (DB_RESULT_SUCCESS !=  ParamsGeneral::ValidateTech(tech_type))
  {
    return NULL;
  }

  ParamsGeneral* paramsGeneral = ParamsGeneral::CreateInstance(tech_type, gen_params);
  
  if (NULL == paramsGeneral)
  {
    return NULL;
  }
  
  Profile* profile = new Profile(paramsGeneral);
  if (NULL == profile)
  {
    delete paramsGeneral;
    paramsGeneral = NULL;
    return NULL;
  }

  ds_profile_db_result_type res = profile->AddSupportedTech(tech_type, tech_params);
  if (DB_RESULT_SUCCESS != res)
  {
    delete profile;
    profile = NULL;
    return NULL;
  }

  return profile;
}

Profile* Profile::CreateInstanceWithoutValidation(
  ds_profile_db_tech_type                   tech_type,
  void*                                     tech_params, /* = NULL */
  ds_profile_db_gen_params_type*            gen_params /* = NULL */)
{
  Profile* profile = NULL;
  PROFILE_DB_LOG_MED_3("CreateInstanceWithoutValidation(): tech %d, tech_params 0x%p, gen_params 0x%p", 
                       tech_type, tech_params, gen_params);
  GetTechInfo(tech_type)->SetValidationRequired(false);
  profile = CreateInstance(tech_type, tech_params, gen_params);
  GetTechInfo(tech_type)->SetValidationRequired(true);
  return profile;
}

Profile* Profile::CreateInstance(const Profile* profile)
{
  if (!profile)
  {
    return NULL;
  }

  return profile->Clone();
}

Profile::Profile(ParamsGeneral* _paramsGeneral):paramsGeneral(_paramsGeneral)
{
  memset(paramsArray, 0, sizeof(paramsArray));
}

Profile* Profile::Clone(void) const
{
  Profile* tmpProf = NULL;

  ParamsGeneral* newParamsGeneral = paramsGeneral->Clone();
  if (!newParamsGeneral)
  {
    return NULL;
  }

  tmpProf = new Profile(newParamsGeneral);
  if (!tmpProf)
  {
    delete newParamsGeneral;
    newParamsGeneral = NULL;
    return NULL;
  }

  for (int i = 0; i < DB_ARRAY_NUM_ELEM(paramsArray); i++)
  {
    if (paramsArray[i])
    {
      tmpProf->paramsArray[i] = paramsArray[i]->Clone();
      if (!(tmpProf->paramsArray[i]))
      {
        delete tmpProf;
        tmpProf = NULL;
        return NULL;
      }    
    }
  }

  return tmpProf;
}

Profile::~Profile(void)
{
  for (int tech = 0; tech < DB_TECH_MAX; tech++)
  {
    if (paramsArray[tech])
    {

      PROFILE_DB_LOG_HIGH_2("Deleting paramsArray[%d] 0x%x", tech, paramsArray[tech]);
      delete paramsArray[tech];
      paramsArray[tech] = NULL;
    }
  }
  if (paramsGeneral)
  {
    PROFILE_DB_LOG_HIGH_1("Deleting ParamsGeneral 0x%x", paramsGeneral);
    delete paramsGeneral;
    paramsGeneral = NULL;
  }
  else
  {
    PROFILE_DB_LOG_ERROR("ParamsGeneral is NULL!!!!");
  }
}

ds_profile_db_result_type Profile::
AddSupportedTech(ds_profile_db_tech_type tech_type, void* tech_params /* = NULL */) 
{
  PROFILE_DB_LOG_MED_2("AddSupportedTech(): tech %d, tech_params 0x%p", 
                        tech_type, tech_params);

  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = ParamsGeneral::ValidateTech(tech_type)))
  {
    return res;
  }

  /* we do not allow to add s technology that's already there */
  if (paramsArray[tech_type])
  {
    PROFILE_DB_LOG_ERROR_1("AddSupportedTech(): profile already has tech %d", tech_type);
    return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
  }

  ParamsTech* paramsTech = NULL;
  paramsTech = GetTechInfo(tech_type)->CreateInstance(paramsGeneral, tech_params);
  if (NULL == paramsTech)
  {
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  paramsArray[tech_type] = paramsTech;
  paramsGeneral->AddSupportedTech(tech_type);

  return DB_RESULT_SUCCESS;
}


ds_profile_db_result_type Profile::
AddSupportedTechWithoutValidation(ds_profile_db_tech_type tech_type, void* tech_params /* = NULL */) 
{
  PROFILE_DB_LOG_MED_2("AddSupportedTechWithoutValidation(): tech %d, tech_params 0x%p", 
                        tech_type, tech_params);
  ds_profile_db_result_type res = DB_RESULT_FAIL;

  GetTechInfo(tech_type)->SetValidationRequired(false);
  res = AddSupportedTech(tech_type, tech_params);
  GetTechInfo(tech_type)->SetValidationRequired(true);
  return res;
}

ds_profile_db_result_type Profile::GetTechParams(
  ds_profile_db_tech_type                   tech_type,
  void*                                     tech_params, /* = NULL */
  ds_profile_db_gen_params_type*            gen_params /* = NULL */) const
{
  PROFILE_DB_LOG_MED_3("GetTechParams(): tech %d, tech_params 0x%p, gen_params 0x%p", 
                       tech_type, tech_params, gen_params);

  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = ParamsGeneral::ValidateTech(tech_type)))
  {
    return res;
  }

  if (tech_params)
  {
    if (paramsArray[tech_type])
    {
      paramsArray[tech_type]->GetCParams(tech_params);
    } else
    {
      PROFILE_DB_LOG_ERROR_1("GetTechParams(): profile does not support tech %d", tech_type);
      return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
    }
  }

  if (gen_params)
  {
    paramsGeneral->GetCParams(gen_params);
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type Profile::SetParam(
  ds_profile_db_ident_type   ident,
  const ds_profile_db_info_type *info,
  boolean is_set)
{
  PROFILE_DB_LOG_MED_1("SetParam(): ident %d", ident);

  ds_profile_db_result_type res;
  Params* params;
  if (DB_RESULT_SUCCESS != (res = GetParamsFromIdent(ident, &params)))
  {
    return res;
  }

  if (DB_RESULT_SUCCESS != (res = params->SetParam(ident, info, is_set)))
  {
    PROFILE_DB_LOG_ERROR_1("SetParam(): error %d in Params::SetParam()", res);
    return res; 
  }
 
  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type Profile::ResetParam
( 
   ds_profile_db_ident_type ident,
   boolean mark_invalid 
)
{
  PROFILE_DB_LOG_MED_1("ResetParam(): ident %d", ident);

  Profile* tmpProf = NULL;

  ds_profile_db_result_type res;

  int tech = IdentToTech(ident);
  if (-1 == tech)
  {
    res = DB_RESULT_ERR_INVAL_IDENT;
    goto bail;
  }

  /* if param is general, than it is always supported, just pick any technology */
  if (DB_TECH_MAX == tech)
  {
    tech = DB_TECH_3GPP;
  }

  /* this technology is not supported by current profile */
  if (!paramsArray[tech])
  {
    res = DB_RESULT_ERR_INVAL_PROFILE_TYPE;
    goto bail;
  }

  /* create a default profile */
  tmpProf = CreateInstance(DB_MAX_SUPPORTED_TECHMASK);
  if (!tmpProf)
  {
    PROFILE_DB_LOG_ERROR("ResetParam(): could not allocate temporary profile");
    res = DB_RESULT_ERR_OUT_OF_PROFILES;
    goto bail;
  }

  /* get the requested param from the default profile */
  ds_profile_db_info_type info;
  unsigned int dont_care;

  /*New Ident to reset all PDP Context*/

  /* reset all the required params inside that profile */
  //TODO Reset all the params here
  //Reset PDP TYPE
  if (ident == TECH_3GPP_PROFILE_PARAM_APN_CLASS)
  {
    /* ignore error, since we know that we invoke it right */
    tmpProf->GetFieldInfo(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE, &dont_care, &(info.len), &(info.buf));
    res = SetParam(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE, &info, !mark_invalid);  
  /* fail through */

    tmpProf->GetFieldInfo(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP, &dont_care, &(info.len), &(info.buf));
    res = SetParam(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP, &info, !mark_invalid);  

    tmpProf->GetFieldInfo(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP, &dont_care, &(info.len), &(info.buf));
    res = SetParam(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP, &info, !mark_invalid);  
      
    tmpProf->GetFieldInfo(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG, &dont_care, &(info.len), &(info.buf));
    res = SetParam(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG, &info, !mark_invalid);  

    tmpProf->GetFieldInfo(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID, &dont_care, &(info.len), &(info.buf));
    res = SetParam(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID, &info, !mark_invalid);  

    tmpProf->GetFieldInfo(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4, &dont_care, &(info.len), &(info.buf));
    res = SetParam(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4, &info, !mark_invalid);  

    tmpProf->GetFieldInfo(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6, &dont_care, &(info.len), &(info.buf));
    res = SetParam(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6, &info, !mark_invalid);  

    tmpProf->GetFieldInfo(TECH_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC, &dont_care, &(info.len), &(info.buf));
    res = SetParam(TECH_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC, &info, !mark_invalid);  

    tmpProf->GetFieldInfo(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG, &dont_care, &(info.len), &(info.buf));
    res = SetParam(TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG, &info, !mark_invalid); 
    
    tmpProf->GetFieldInfo(GENERAL_PROFILE_PARAM_APN_NAME , &dont_care, &(info.len), &(info.buf));
    res = SetParam(GENERAL_PROFILE_PARAM_APN_NAME, &info, !mark_invalid); 
  } else
  {
    tmpProf->GetFieldInfo(ident , &dont_care, &(info.len), &(info.buf));
    res = SetParam(ident, &info, !mark_invalid);
  }

bail:
  
  delete tmpProf;
  tmpProf = NULL;

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
  
}

ds_profile_db_result_type Profile::GetParam (
  ds_profile_db_ident_type   ident,
  ds_profile_db_info_type    *info) const
{
  ds_profile_db_result_type res;
  Params* params;
  if (DB_RESULT_SUCCESS != (res = GetParamsFromIdent(ident, &params)))
  {
    return res;
  }

  res = params->GetParam(ident, info);

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}

ds_profile_db_result_type Profile::GetParamInUse (
  ds_profile_db_ident_type   ident,
  ds_profile_db_info_type    *info,
  ds_profile_db_tech_type    tech,
  boolean                    is_tech_param_set
)
{
  PROFILE_DB_LOG_MED_1("GetParamInUse(): ident %d", ident);

  ds_profile_db_result_type res;
  ds_profile_db_ident_type  new_ident;
  Params* params;
  
  if (!is_tech_param_set)
  {
    if (DB_RESULT_SUCCESS != (res = GetParamsFromIdent(ident, &params)))
    {
      return res;
    }
    res = params->GetParam(ident, info);
    if ((ident == COMMON_TECH_PROFILE_PARAM_PDP_TYPE) && (tech == DB_TECH_3GPP))
    {
      ds_profile_3gpp_pdp_type_enum_type pdp_type;
      switch (*(ds_profile_db_pdp_type*)info->buf)
      {
        case DB_PDP_TYPE_IPV4:
          pdp_type = DS_PROFILE_3GPP_PDP_IP;
          memscpy(info->buf, info->len, (void*)&pdp_type, sizeof(ds_profile_3gpp_pdp_type_enum_type));
          break;
        case DB_PDP_TYPE_IPV6:
          pdp_type = DS_PROFILE_3GPP_PDP_IPV6;
          memscpy(info->buf, info->len, (void*)&pdp_type, sizeof(ds_profile_3gpp_pdp_type_enum_type));
          break;
        case DB_PDP_TYPE_IPV4V6:
          pdp_type = DS_PROFILE_3GPP_PDP_IPV4V6;
          memscpy(info->buf, info->len, (void*)&pdp_type, sizeof(ds_profile_3gpp_pdp_type_enum_type));
          break;
        default:
          PROFILE_DB_LOG_ERROR("Unknown PDP type for conversion");
          break;
      }
    }

  } else
  {
    new_ident = ds_profile_db_get_tech_ident_from_common_ident(ident, tech);
    if (DB_RESULT_SUCCESS != (res = GetParamsFromIdent(new_ident, &params)))
    {
      return res;
    }
    res = params->GetParam(new_ident, info);
  }
  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res; 
}

bool Profile::CompareParam (
  ds_profile_db_ident_type         ident,
  const ds_profile_db_info_type    *info) const
{
  PROFILE_DB_LOG_MED_1("CompareParam(): ident %d", ident);

  ds_profile_db_result_type res;
  Params* params;
  if (DB_RESULT_SUCCESS != (res = GetParamsFromIdent(ident, &params)))
  {
    return false;
  }

  return params->CompareParam(ident, info);
}

static int add_ident_to_array(void* data, va_list args)
{
  ds_profile_db_ident_type* idents;  
  unsigned int *i;
  
  idents = va_arg(args, ds_profile_db_ident_type*);  
  i = va_arg(args, unsigned int*);

  idents[*i] = *((ds_profile_db_ident_type*)data);
  (*i)++;

  return 0;
}

ds_profile_db_result_type Profile::GetChangedIdentsInternal (
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_ident_type** idents,
  uint16* idents_count
) const
{
  if (!idents_count && !idents)
  {
    return DB_RESULT_SUCCESS;
  }

  ds_profile_db_result_type res;
  uint16 count_tmp = 0;
  ds_util_list_hndl_type list;

  /* first count the changed idents */
  DB_FOR_EVERY_TECH_IN_MASK_DO_START(tech_mask, tech)
  if (tech <= DB_TECH_COMMON)
  {
    if (!paramsArray[tech])
    {
      return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
    }

  if (NULL == (list = paramsArray[tech]->GetChangedIdentsList()))
  {
      return DB_RESULT_FAIL;
    }

    count_tmp += (uint16)ds_util_list_get_size(list);
    DB_FOR_EVERY_TECH_IN_MASK_DO_END

  if (idents_count)
  {
      *idents_count = count_tmp;
    }
    
    /* now get the changed idents */
  if (!idents)
  {
      return DB_RESULT_SUCCESS;
    }

    *idents = NULL;

    /* nothing to report */
  if (0 == count_tmp)
  {
      return DB_RESULT_SUCCESS;
    }


    PS_SYSTEM_HEAP_MEM_ALLOC(*idents, sizeof(ds_profile_db_ident_type) * count_tmp, 
                             ds_profile_db_ident_type*);

  if (NULL == *idents)
  {
      return DB_RESULT_ERR_OUT_OF_MEMORY;
    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_START(tech_mask, tech)
  /* no need to check non null of paramsArray[tech] and non null of list, already did above */
  if (tech < DB_TECH_MAX)
  {
    list = paramsArray[tech]->GetChangedIdentsList();
    unsigned int i = 0;
    if (DS_SUCCESS !=  ds_util_list_do_for_every_node(list,
                                                      add_ident_to_array,
                                                      false,
                                                      NULL, 
                                                      *idents, 
                                                      &i))
    {
       res = DB_RESULT_FAIL;
       goto bail;
    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  return DB_RESULT_SUCCESS;

bail:
  if (*idents)
  {
    PS_SYSTEM_HEAP_MEM_FREE(*idents);
  }

  return res;
}

ds_profile_db_result_type Profile::GetChangedIdentsInternal (  
  ds_profile_db_ident_type** idents,
  uint16* idents_count
) const
{
  if (!idents_count && !idents)
  {
    return DB_RESULT_SUCCESS;
  }

  ds_profile_db_result_type res;
  uint16 count_tmp = 0;
  ds_util_list_hndl_type list;

  /* first count the changed idents */    
  if (NULL == (list = paramsGeneral->GetChangedIdentsList()))
  {
    return DB_RESULT_FAIL;
  }

  count_tmp = (uint16)ds_util_list_get_size(list);

  if (idents_count)
  {
    *idents_count = count_tmp;
  }

  if (!idents)
  {
    return DB_RESULT_SUCCESS;
  }

  /* now get the changed idents */
  *idents = NULL;

  /* nothing to report */
  if (0 == count_tmp)
  {
    return DB_RESULT_SUCCESS;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC( *idents, sizeof(ds_profile_db_ident_type) * count_tmp,
                           ds_profile_db_ident_type*);
                                 
  if (NULL == *idents)
  {
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  unsigned int i = 0;
  if (DS_SUCCESS !=  ds_util_list_do_for_every_node(list,
                                                    add_ident_to_array,
                                                    false,
                                                    NULL, 
                                                    *idents, 
                                                    &i))
  {
     res = DB_RESULT_FAIL;
     goto bail;
  }

  return DB_RESULT_SUCCESS;

bail:
  if (*idents)
  {
    PS_SYSTEM_HEAP_MEM_FREE(*idents);
  }

  return res;
}



ds_profile_db_result_type Profile::GetChangedIdents (
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_ident_type** idents /* = NULL */,
  uint16* idents_count /* = NULL */,
  ds_profile_db_ident_type** idents_gen /* = NULL */, 
  uint16* idents_count_gen /* = NULL */
) const
{
  PROFILE_DB_LOG_MED_5("GetChangedIdents(): tech_mask %d, idents 0x%p, idents_count 0x%p,"
                       "idents_gen 0x%p, idents_count_gen 0x%p",
                       tech_mask, idents, idents_count, 
                       idents_gen, idents_count_gen);

  ds_profile_db_result_type res;

  if (DB_RESULT_SUCCESS != (res = ParamsGeneral::ValidateTechMask(tech_mask)))
  {
    return res;
  }
  
  if (DB_RESULT_SUCCESS != (res = GetChangedIdentsInternal(tech_mask, idents, idents_count)))
  {
    PROFILE_DB_LOG_ERROR_1("GetChangedIdents(): error %d while fetching idents", res);
    return res;
  }

  if (DB_RESULT_SUCCESS != (res = GetChangedIdentsInternal(idents_gen, idents_count_gen)))
  {
    PROFILE_DB_LOG_ERROR_1("GetChangedIdents(): error %d while fetching general idents", res);
    return res;
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_profile_num_type Profile::GetNum(void) const
{
  return paramsGeneral->GetNum();
}

bool Profile::IsPersistent( void ) const
{
  return paramsGeneral->IsPersistent();
}

ds_profile_db_supported_tech_mask Profile::GetSupportedTechMask( void ) const
{
  return paramsGeneral->GetSupportedTechMask();
}

ds_profile_db_result_type Profile::GetFieldInfo (
  ds_profile_db_ident_type ident,
  unsigned int* offset, 
  uint16* len,
  void** buf) const
{
  ds_profile_db_result_type res;
  Params* params;
  if (DB_RESULT_SUCCESS != (res = GetParamsFromIdent(ident, &params)))
  {
    return res;
  }

  res = params->GetFieldInfo(ident, offset, len, buf);

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}


ds_profile_db_result_type Profile::
GetParamsFromIdent(
  ds_profile_db_ident_type ident,
  Params** params) const
{
  ds_profile_db_result_type res;
  int tech = IdentToTech(ident);
  if (-1 == tech)
  {
    res = DB_RESULT_ERR_INVAL_IDENT;
    goto bail;
  }

  if (DB_TECH_MAX == tech)
  {
    *params = paramsGeneral;
    return DB_RESULT_SUCCESS;
  }

  //Check if the tech is registered
  switch (tech)
  {
    case DB_TECH_3GPP:
    case DB_TECH_3GPP2:
      if(!ds_profile_db_get_tech_tbl((ds_profile_db_tech_type)tech))
      {
        res = DB_RESULT_ERR_SET_IDENT_NOT_SUPPORTED;
        goto bail;
      }
      break;
  }

  *params = paramsArray[tech];
  if (!(*params))
  {
    res = DB_RESULT_ERR_INVAL_PROFILE_TYPE;
    goto bail;
  }

  return DB_RESULT_SUCCESS;  

bail:

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;

}

size_t Profile::GetMaxCStructSize(void)
{
  static size_t max_size = 0;

  if (max_size > 0)
  {
    return max_size;
  }

  for (int tech = 0; tech < DB_TECH_MAX; tech++)
  {
    if (GetTechInfo((ds_profile_db_tech_type)tech)->struct_size > max_size)
    {
      max_size = GetTechInfo((ds_profile_db_tech_type)tech)->struct_size;
    }
  }

  if (sizeof(ds_profile_db_gen_params_type) > max_size)
  {
    max_size = sizeof(ds_profile_db_gen_params_type);
  }

  return max_size;
}

const void* Profile::GetCParamsPtr(ds_profile_db_tech_type tech) const
{
  if (tech <= DB_TECH_COMMON)
  {  
    if (paramsArray[tech]) 
    {
      return paramsArray[tech]->GetCParamsPtr();
    }
  }
  return NULL;
}

size_t Profile::GetCStructSize(ds_profile_db_tech_type tech)
{
  if (DB_TECH_MAX == tech)
  {
    return sizeof(ds_profile_db_gen_params_type);
  }

  if (DB_RESULT_SUCCESS != ParamsGeneral::ValidateTech(tech))
  {
    return 0;  
  }

  return GetTechInfo(tech)->struct_size;
}

ds_profile_db_result_type Profile::GetDefaultCParams( ds_profile_db_tech_type tech, void* c_params )
{
  ds_profile_db_result_type res;
  if (DB_TECH_MAX == tech)
  {
    ParamsGeneral::GetDefaultCParams((ds_profile_db_gen_params_type*)c_params);
    return DB_RESULT_SUCCESS;
  }

  res = ParamsGeneral::ValidateTech(tech);
  if (DB_RESULT_SUCCESS != res)
  {
      return res;  
  }

  GetTechInfo(tech)->GetDefaultCParams(c_params);

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type Profile::ValidateProfileNum(ds_profile_db_profile_num_type num)
{
  return ParamsGeneral::ValidateProfileNum(num);
}

ds_profile_db_result_type Profile::ValidateSubscriptionId( ds_profile_db_subs_type subs )
{
  return ParamsGeneral::ValidateSubscriptionId(subs);
}

ds_profile_db_result_type Profile::ValidateTech(ds_profile_db_tech_type tech_type)
{
  return ParamsGeneral::ValidateTech(tech_type);
}

ds_profile_db_result_type Profile::ValidateTechMask(ds_profile_db_supported_tech_mask tech_mask)
{
  return ParamsGeneral::ValidateTechMask(tech_mask);
}

ds_profile_db_result_type Profile::ValidateSupportedTechMask(ds_profile_db_supported_tech_mask
                                                              tech_mask)
{
  return ParamsGeneral::ValidateSupportedTechMask(tech_mask);
}

ds_profile_db_result_type Profile::ValidateProfileNumPerTech(ds_profile_db_profile_num_type num,
                                                             ds_profile_db_supported_tech_mask tech_mask)
{
  return ParamsGeneral::ValidateProfileNumPerTech(num, tech_mask);
}

ds_profile_db_subs_type Profile::GetDefaultSubscriptionId(void)
{
  return ParamsGeneral::GetDefaultSubscriptionId();
}

void Profile::SetPersistency(bool persistent) 
{ 
  paramsGeneral->SetPersistency(persistent);
}

ds_profile_db_result_type Profile::SetSubscriptionId( ds_profile_db_subs_type subsciption_id, bool track /* true */)
{
  return paramsGeneral->SetSubscriptionId(subsciption_id, track);
}

ds_profile_db_subs_type Profile::GetSubcriptionId(void) const
{
  return paramsGeneral->GetSubcriptionId();
}

void Profile::SetNumber(ds_profile_db_profile_num_type profile_number) 
{
  paramsGeneral->SetNum(profile_number);
}


int Profile::IdentToTech(ds_profile_db_ident_type ident) const 
{
  if (GENERAL_PROFILE_PARAM_MIN <= ident &&
      GENERAL_PROFILE_PARAM_MAX >= ident)
  {
    return DB_TECH_MAX;
  }

  ds_profile_db_ident_type start_range;
  ds_profile_db_ident_type end_range;
  for (int tech = 0; tech < DB_TECH_MAX; tech++)
  {
    GetTechInfo((ds_profile_db_tech_type)tech)->GetIdentsRange(&start_range, &end_range);
    if (start_range <= ident &&
        end_range   >= ident)
    {
      return tech;
    }
  }

  return -1;
}

void Profile::ResetParamTracking(void)
{
  paramsGeneral->ResetParamTracking();

  for (int i = 0; i < DB_ARRAY_NUM_ELEM(paramsArray); i++)
  {
    if (paramsArray[i])
    {
      paramsArray[i]->ResetParamTracking();
    }
  }
}

void Profile::SetReadTime(uint32 set_time)
{
  profile_read_time = set_time;
  return;
}

uint32 Profile::GetReadTime() const
{
  return profile_read_time;
}
