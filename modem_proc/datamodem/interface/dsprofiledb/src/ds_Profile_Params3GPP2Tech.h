#ifndef DS_PROFILE_PARAMS_3GPP2_TECH_H
#define DS_PROFILE_PARAMS_3GPP2_TECH_H
/*===========================================================================
  @file ds_Profile_Params3GPP2Tech.h

  This file defines Params3GPP2Tech class which implements 3GPP2 Technology Params 
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
    class Params3GPP2Tech: public ParamsCommonTech
    {
      public:

        static ds_profile_db_result_type Init(Params::ParamInfo* mapParamsInfo);
   
        static void GetDefaultCParams(void* params);

        virtual ~Params3GPP2Tech() = 0;
        
      protected:

        static ds_profile_db_result_type ValidateGeneral(const ds_profile_db_3gpp2_tech_params_type* profile);
        Params3GPP2Tech(const ParamsGeneral* paramsGen):ParamsCommonTech(paramsGen){};

        static const ds_profile_db_ident_type MAP_START_RANGE = TECH_3GPP2_PROFILE_PARAM_MIN;
        static const ds_profile_db_ident_type MAP_END_RANGE   = TECH_3GPP2_PROFILE_PARAM_MAX;

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

        /* for 3GPP2 this needs to be overridden, so that params that fail validation because
           they do not belong to profile sub-type (KDDI / OMH / etc) are ignored */
        static ds_profile_db_result_type ValidateCParams(
          validate_fn_type                     validation_fn,
          ds_profile_db_ident_type             start_range,
          ParamInfo*                           mapParamsInfo,
          unsigned int                         map_number_of_elements, 
          const void*                          params,
          const ds_profile_db_gen_params_type* gen_params = NULL);

        static validate_fn_type validate_fn;
      
        static const ps_mem_pool_enum_type memory_pool_id = 
          PS_MEM_PROFILE_DB_PARAMS_3GPP2_TYPE;
     
    }; /* class Params3GPP2Tech */
  } /* namespace Profile */
} /* namespace ds */
inline ds::Profile::Params3GPP2Tech::~Params3GPP2Tech(){}

#define Params3GPP2TechConcrete ParamsTemplateTech<Params3GPP2Tech, ds_profile_db_3gpp2_tech_params_type>

#endif /* DS_PROFILE_PARAMS_3GPP2_TECH_H */
