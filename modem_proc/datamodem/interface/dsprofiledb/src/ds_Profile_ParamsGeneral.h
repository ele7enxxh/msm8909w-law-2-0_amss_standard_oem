#ifndef DS_PROFILE_PARAMS_GENERAL_H
#define DS_PROFILE_PARAMS_GENERAL_H
/*===========================================================================
  @file ds_Profile_ParamsGeneral.h

  This file defines ParamsGeneral class which implements General Params 
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
#include "ds_Profile_MemManager.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class ParamsGeneral: public Params
    {
      public:

        DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(PS_MEM_PROFILE_DB_PARAMS_GENERAL_TYPE);

        static ds_profile_db_result_type Init();

        static ParamsGeneral* 
          CreateInstance(ds_profile_db_tech_type tech,
                         void* params = NULL);

        static ds_profile_db_result_type ValidateCParams(void *params);

        static void GetDefaultCParams(ds_profile_db_gen_params_type* params);

        /* various validation methods */
        static ds_profile_db_result_type ValidateTech(ds_profile_db_tech_type tech_type);
        static ds_profile_db_result_type ValidateTechMask(ds_profile_db_supported_tech_mask tech_mask);
        static ds_profile_db_result_type ValidateProfileNum(ds_profile_db_profile_num_type num);
        static ds_profile_db_result_type ValidateSupportedTechMask(ds_profile_db_supported_tech_mask
                                                                     tech_mask);
        static ds_profile_db_result_type ValidateSubscriptionId(ds_profile_db_subs_type subs);
        static ds_profile_db_result_type ValidateProfileNumPerTech(ds_profile_db_profile_num_type num,
                                                                   ds_profile_db_supported_tech_mask tech_mask);

        ds_profile_db_supported_tech_mask GetSupportedTechMask() const;
        ds_profile_db_profile_num_type GetNum() const;
        
        bool IsPersistent() const;
        
      protected:

        virtual ParamsGeneral* Clone() const;
  
      private:

        ParamsGeneral(const ParamsGeneral& paramsGeneral);
        ParamsGeneral(ds_profile_db_tech_type tech);
        ParamsGeneral(const ds_profile_db_gen_params_type* gen_params);

        virtual bool IsParamReadOnly( ds_profile_db_ident_type ident ) const;

        virtual int IdentToIndex(ds_profile_db_ident_type ident) const;

        virtual const void* GetCParamsPtr() const;

        virtual Params::ParamInfo* const
                      GetMapParamsPtr() const;

        virtual validate_fn_type         GetValidateFn() const;
        virtual Params::set_fn_type      GetSpecialSetFn() const;
        virtual Params::get_fn_type      GetSpecialGetFn() const;
        virtual Params::compare_fn_type  GetCompareFn() const;

        virtual size_t GetCStructSize() const;

        void SetPersistency(bool persistent);
        ds_profile_db_result_type SetSubscriptionId( ds_profile_db_subs_type subsciption_id, bool track );
        ds_profile_db_result_type SetIPV6Delegation( boolean ipv6_delegation, bool track);
        ds_profile_db_result_type SetCLATEnable( boolean clat_enabled, bool track);
        void SetNum(ds_profile_db_profile_num_type profile_number);
       
        void AddSupportedTech(ds_profile_db_tech_type tech);

        ds_profile_db_gen_params_type params;

        static const ds_profile_db_ident_type MAP_START_RANGE = GENERAL_PROFILE_PARAM_MIN;
        static const ds_profile_db_ident_type MAP_END_RANGE   = GENERAL_PROFILE_PARAM_MAX;

        static Params::ParamInfo 
          mapParamsInfo[MAP_END_RANGE - MAP_START_RANGE + 1];

        static ds_profile_db_subs_type GetDefaultSubscriptionId();
        ds_profile_db_subs_type GetSubcriptionId();
        boolean GetIPV6Delegation();
        boolean GetCLATEnable();

        /* to access class private methods */
        friend class ParamsTech;
        friend class Profile;         
    }; /* class ParamsGeneral */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_PARAMS_GENERAL_H */


