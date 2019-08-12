#ifndef DS_PROFILE_PARAMS_COMMON_TECH_H
#define DS_PROFILE_PARAMS_COMMON_TECH_H
/*===========================================================================
  @file ds_Profile_ParamsCommonTech.h

  This file defines ParamsCommonTech class which implements Common Technology Params 
  subcomponent of Profile class.


                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-04-22 am Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_profile_db.h"
#include "ds_Profile_Params.h"
#include "ds_Profile_ParamsTech.h"
#include "ds_Profile_ParamsTemplateTech.h"
#include "ps_mem.h"


/*===========================================================================

                     VARIOUS PUBLIC DEFINES

===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class ParamsCommonTech: public ParamsTech
    {
      public:
    
        static ds_profile_db_result_type Init(Params::ParamInfo* mapParamsInfo);

        static void GetDefaultCParams(void* params);

        virtual ~ParamsCommonTech() = 0;

    protected:
       
        static ds_profile_db_result_type ValidateGeneral(const ds_profile_db_common_tech_params_type* profile);

        ParamsCommonTech(const ParamsGeneral* paramsGen):ParamsTech(paramsGen){};     

        static const ds_profile_db_ident_type MAP_START_RANGE = (ds_profile_db_ident_type)COMMON_TECH_PARAM_MIN;
        static const ds_profile_db_ident_type MAP_END_RANGE   = COMMON_TECH_PARAM_MAX;

        /* allow access to extending classes */
        static ds_profile_db_result_type SpecialGet(
          ds_profile_db_ident_type ident, 
          ds_profile_db_info_type* info,
          const void*              params);

        static void SpecialSet(
          ds_profile_db_ident_type       ident, 
          const ds_profile_db_info_type* info,
          void*                          params,
          boolean is_set);

        static bool CompareIdent(
          ds_profile_db_ident_type ident, 
          const ds_profile_db_info_type* info1,
          const ds_profile_db_info_type* info2);

        static validate_fn_type validate_fn;      

        static const ps_mem_pool_enum_type memory_pool_id = 
          PS_MEM_PROFILE_DB_PARAMS_COMMON_TYPE;

    }; /* class ParamsCommonTech */
  } /* namespace Profile */
} /* namespace ds */

inline ds::Profile::ParamsCommonTech::~ParamsCommonTech(){}

#define ParamsCommonTechConcrete ParamsTemplateTech<ParamsCommonTech, ds_profile_db_common_tech_params_type>

#endif /* DS_PROFILE_PARAMS_COMMON_TECH_H */
