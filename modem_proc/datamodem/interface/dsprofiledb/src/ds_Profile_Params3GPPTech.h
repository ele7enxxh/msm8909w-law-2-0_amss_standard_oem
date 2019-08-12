#ifndef DS_PROFILE_PARAMS_3GPP_TECH_H
#define DS_PROFILE_PARAMS_3GPP_TECH_H
/*===========================================================================
  @file ds_Profile_Params3GPPTech.h

  This file defines Params3GPPTech class which implements 3GPP Technology Params 
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
#include "ds_Profile_ParamsCommonTech.h"
#include "ds_Profile_ParamsTemplateTech.h"
#include "ps_mem.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
  namespace Profile
  {
    class Params3GPPTech: public ParamsCommonTech
    {
      public:

        static ds_profile_db_result_type Init(Params::ParamInfo* mapParamsInfo);         

        static void GetDefaultCParams(void* params);

        virtual ~Params3GPPTech() = 0;
                
      protected:          

        static ds_profile_db_result_type ValidateGeneral(const ds_profile_db_3gpp_tech_params_type* profile);
        Params3GPPTech(const ParamsGeneral* paramsGen):ParamsCommonTech(paramsGen){};

        static const ds_profile_db_ident_type MAP_START_RANGE = TECH_3GPP_PROFILE_PARAM_MIN;
        static const ds_profile_db_ident_type MAP_END_RANGE   = TECH_3GPP_PROFILE_PARAM_MAX;

        static ds_profile_db_result_type SpecialGet(
          ds_profile_db_ident_type ident, 
          ds_profile_db_info_type* info,
          const void*              params);

        static void SpecialSet(
          ds_profile_db_ident_type ident, 
          const ds_profile_db_info_type* info,
          void*              params,
          boolean is_set);

        static bool CompareIdent(
          ds_profile_db_ident_type ident, 
          const ds_profile_db_info_type* info1,
          const ds_profile_db_info_type* info2);

        static validate_fn_type validate_fn;
      
        static const ps_mem_pool_enum_type memory_pool_id = 
          PS_MEM_PROFILE_DB_PARAMS_3GPP_TYPE;

    }; /* class Params3GPPTech */
  } /* namespace Profile */
} /* namespace ds */

inline ds::Profile::Params3GPPTech::~Params3GPPTech(){}

#define Params3GPPTechConcrete ParamsTemplateTech<Params3GPPTech, ds_profile_db_3gpp_tech_params_type>

#endif /* DS_PROFILE_PARAMS_3GPP_TECH_H */
