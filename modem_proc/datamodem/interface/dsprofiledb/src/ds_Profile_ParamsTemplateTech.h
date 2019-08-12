#ifndef DS_PROFILE_PARAMS_TEMPLATE_TECH_H
#define DS_PROFILE_PARAMS_TEMPLATE_TECH_H
/*===========================================================================
  @file ds_Profile_ParamsTemplateTech.h

  This file defines ParamsTemplateTech template for Technology Specific Params 
  classes. The purpose of this template is to define and implement common code 
  for all ParamsXXXTech classes common functionality. 
  This enables tech classes to have minimum amount of code and reduce copy-pasting 
  the same code between them. 
  
  The class is a template which has 2 arguments :
  CPPclass - represents Technology Specifc Params class from which this template 
  will derive (i.e Params3GPPTech)

  CStruct - c structure that stores tech specific params for this technology
  (i.e ds_profile_db_3gpp_tech_params_type)

  CPPClass should have the following :
  MAP_START_RANGE, MAP_END_RANGE - the range of identifiers for the technology
  List of functions implementing various parts of tech specific functionality 
  (see all places with CppClass:: below)


                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-05-15 am Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_profile_db.h"
#include "ds_Profile_Params.h"
#include "ds_Profile_ParamsTech.h"
#include "ds_Profile_Log.h"

using namespace ds::Profile;
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
  namespace Profile
  {
    template <class CppClass, typename CStruct>
    class ParamsTemplateTech: public CppClass
    {
      public:

        DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(CppClass::memory_pool_id);

        static ParamsTech* 
          CreateInstance(const ParamsGeneral* paramsGen,
                         const void* params = NULL);

        static ds_profile_db_result_type 
          ValidateCParams(const void* params, 
                          const ds_profile_db_gen_params_type* gen_params);

        static void GetIdentsRange(ds_profile_db_ident_type* start, 
                                   ds_profile_db_ident_type* end);

        static ds_profile_db_result_type Init();
        
        static void SetValidationReqd(bool validation_reqd);

        static void SetValidationFunction(validate_fn_type tech_validation_func);

        
      protected:
           
        virtual Params* Clone() const;        

      private:    

        virtual int IdentToIndex(ds_profile_db_ident_type ident) const;

        virtual const void* GetCParamsPtr() const;
        virtual Params::ParamInfo* const
          GetMapParamsPtr() const;

        virtual validate_fn_type GetValidateFn() const;
        virtual Params::set_fn_type GetSpecialSetFn() const;
        virtual Params::get_fn_type GetSpecialGetFn() const;
        virtual Params::compare_fn_type GetCompareFn() const;

        virtual size_t GetCStructSize() const;

        ParamsTemplateTech(const ParamsTemplateTech& paramsTT);

        ParamsTemplateTech(const ParamsGeneral* paramsGen,
                           const CStruct* params = NULL);        
        CStruct params;
        static size_t params_size;
        static bool   is_validation_reqd;

        static Params::ParamInfo 
          mapParamsInfo[CppClass::MAP_END_RANGE - CppClass::MAP_START_RANGE + 1];
        
        friend class Profile;

    }; /* class ParamsTemplateTech */
  } /* namespace Profile */
} /* namespace ds */

/*===========================================================================

                    INTERNAL GLOBAL STRUCTURES INIT

===========================================================================*/


/*===========================================================================

                    STATIC MEMBER INIT

===========================================================================*/

template <class CppClass, typename CStruct>
Params::ParamInfo ParamsTemplateTech<CppClass, CStruct>::
mapParamsInfo[CppClass::MAP_END_RANGE - CppClass::MAP_START_RANGE + 1] = {0};

template <class CppClass, typename CStruct>
size_t ParamsTemplateTech<CppClass, CStruct>::params_size = sizeof(CStruct);

template <class CppClass, typename CStruct>
bool ParamsTemplateTech<CppClass, CStruct>::is_validation_reqd = true;
/*===========================================================================

                    MEMBER FUNCTIONS

===========================================================================*/
template <class CppClass, typename CStruct>
ds_profile_db_result_type ParamsTemplateTech<CppClass, CStruct>::
Init()
{
  return CppClass::Init(mapParamsInfo);
}

template <class CppClass, typename CStruct>
ParamsTech* ParamsTemplateTech<CppClass, CStruct>::
CreateInstance(const ParamsGeneral* paramsGen,
               const void* params /* = NULL */)
{
  if (!paramsGen) {
    return NULL;
  }

  if (is_validation_reqd)
  {
    if (params && 
        (DB_RESULT_SUCCESS != ValidateCParams(params,
                                              CppClass::GetGeneralCParamsPtr(paramsGen)))) 
    {
      return NULL;    
    }
  }
  else
  {
    PROFILE_DB_LOG_MED_1("Validation not required %d", is_validation_reqd);
  }
  ParamsTech* paramsTech = 
    new ParamsTemplateTech<CppClass, CStruct>(paramsGen, (CStruct*)params);
  if (!paramsTech) {
    return NULL;
  }

  if (DB_RESULT_SUCCESS != paramsTech->InitInstance()) {
    delete paramsTech;
    paramsTech = NULL;
    return NULL;
  }

  return paramsTech;
}

template <class CppClass, typename CStruct>
ParamsTemplateTech<CppClass, CStruct>::
ParamsTemplateTech(const ParamsGeneral* paramsGen,
                   const CStruct* _params /* = NULL */):
                   CppClass(paramsGen)
{
  if (_params) {
    params = *_params;
  } else {
    CppClass::GetDefaultCParams(&params);
  }
}


template <class CppClass, typename CStruct>
ParamsTemplateTech<CppClass, CStruct>::
ParamsTemplateTech(const ParamsTemplateTech<CppClass, CStruct>& paramsTT):CppClass(paramsTT.paramsGeneral), params(paramsTT.params)
{}


template <class CppClass, typename CStruct>
Params* ParamsTemplateTech<CppClass, CStruct>::
Clone() const
{
  ParamsTemplateTech<CppClass, CStruct>* paramsTech = 
    new ParamsTemplateTech<CppClass, CStruct>(*this);

  if (!paramsTech) {
    return NULL;
  }

  if (DB_RESULT_SUCCESS != paramsTech->InitInstance(this->changed_idents_list)) {
    delete paramsTech;
    paramsTech = NULL;
    return NULL;
  }

  return paramsTech;
}


template <class CppClass, typename CStruct>
ds_profile_db_result_type ParamsTemplateTech<CppClass, CStruct>::
ValidateCParams(const void* params, const ds_profile_db_gen_params_type* gen_params)
{
  ds_profile_db_result_type res;

  PROFILE_DB_LOG_MED_1("ValidateCParams(): for params 0x%x", params);

  if (!params) {
    return DB_RESULT_ERR_INVAL;
  }

  /* perform general profile checks not related to a specific parameter */
  if (DB_RESULT_SUCCESS != 
    (res = CppClass::ValidateGeneral((const CStruct*)params))) {
      return res;
  } 

  return CppClass::ValidateCParams(CppClass::validate_fn, 
                                   CppClass::MAP_START_RANGE,
                                   mapParamsInfo, 
                                   DB_ARRAY_NUM_ELEM(mapParamsInfo),
                                   params,
                                   gen_params);
}

template <class CppClass, typename CStruct>
const void* ParamsTemplateTech<CppClass, CStruct>::
GetCParamsPtr() const 
{
  return &params;
} 

template <class CppClass, typename CStruct>
Params::ParamInfo* const
ParamsTemplateTech<CppClass, CStruct>::
GetMapParamsPtr() const
{
  return mapParamsInfo;
}

template <class CppClass, typename CStruct>
validate_fn_type ParamsTemplateTech<CppClass, CStruct>::
GetValidateFn() const
{
  return CppClass::validate_fn;
}

template <class CppClass, typename CStruct>
Params::set_fn_type ParamsTemplateTech<CppClass, CStruct>::
GetSpecialSetFn() const
{
  return &(CppClass::SpecialSet);
}

template <class CppClass, typename CStruct>
Params::get_fn_type ParamsTemplateTech<CppClass, CStruct>::
GetSpecialGetFn() const
{
  return &(CppClass::SpecialGet);
}

template <class CppClass, typename CStruct>
Params::compare_fn_type ParamsTemplateTech<CppClass, CStruct>::
GetCompareFn() const
{
  return &(CppClass::CompareIdent);
}

template <class CppClass, typename CStruct>
size_t ParamsTemplateTech<CppClass, CStruct>::
GetCStructSize() const
{
  return params_size;
}


template <class CppClass, typename CStruct>
void ParamsTemplateTech<CppClass, CStruct>::
GetIdentsRange(
  ds_profile_db_ident_type* start,                          
  ds_profile_db_ident_type* end)
{
  if (!start || !end) {
    return;
  }

  *start = CppClass::MAP_START_RANGE;
  *end   = CppClass::MAP_END_RANGE;
}

template <class CppClass, typename CStruct>
int ParamsTemplateTech<CppClass, CStruct>::
IdentToIndex(ds_profile_db_ident_type ident) const
{
  if (CppClass::MAP_START_RANGE <= ident &&
      CppClass::MAP_END_RANGE   >= ident) {
    return ident - CppClass::MAP_START_RANGE;
  }

  return -1;
}

template <class CppClass, typename CStruct>
void ParamsTemplateTech<CppClass, CStruct>::
SetValidationReqd(bool validation_reqd)
{
  is_validation_reqd = validation_reqd;
}

template <class CppClass, typename CStruct>
void ParamsTemplateTech<CppClass, CStruct>::
SetValidationFunction(validate_fn_type tech_validate_func) 
{
  CppClass::validate_fn = tech_validate_func;
}

#endif /* DS_PROFILE_PARAMS_TEMPLATE_TECH_H */
