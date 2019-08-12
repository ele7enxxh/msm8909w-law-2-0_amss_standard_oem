#ifndef DS_PROFILE_PROFILE_H
#define DS_PROFILE_PROFILE_H
/*===========================================================================
  @file ds_Profile_Profile.h

  This file defines Profile class which is responsible for all the operations
  on the local copy of the Profile (alloc, get/set param, etc)
  Each profile is a container for several Param subcomponents which implement
  the functionality for a distinct block of parameters. 
  Profile always has ParamsGeneral and at least one technology specific Params
  components. Additional Params technology components will be added whenever
  user tries to set a param belonging to this technology

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

#include "ds_profile_db.h"
#include "ds_Profile_ParamsTech.h"
#include "ds_Profile_ParamsTemplateTech.h"
#include "ds_Profile_ParamsGeneral.h"
#include "ds_Profile_ParamsCommonTech.h"
#include "ds_Profile_MemManager.h"
#include "ds_Profile_Params.h"
#include "ds_Profile_Params3GPP2Tech.h"
#include "ds_Profile_ParamsCommonTech.h"
#include "ds_Profile_Log.h"
#include "ds_Profile_Mutex.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
  namespace Profile
  {
    class Profile
    {
      public:

        DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(PS_MEM_PROFILE_DB_PROFILE_TYPE);
    
        static ds_profile_db_result_type Init();

        static Profile* CreateInstance(
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_1);

        static Profile* CreateInstance(
          ds_profile_db_tech_type                   tech_type,
          void*                                     tech_params = NULL,
          ds_profile_db_gen_params_type*            gen_params = NULL);

        static Profile* CreateInstanceWithoutValidation(
          ds_profile_db_tech_type                   tech_type,
          void*                                     tech_params = NULL,
          ds_profile_db_gen_params_type*            gen_params = NULL);

        static Profile* CreateInstance(const Profile* profile);

        virtual ~Profile();

        ds_profile_db_result_type AddSupportedTech(
          ds_profile_db_tech_type tech,
          void* tech_params = NULL);

        ds_profile_db_result_type AddSupportedTechWithoutValidation(
          ds_profile_db_tech_type tech,
          void* tech_params = NULL);


        ds_profile_db_result_type GetTechParams(
          ds_profile_db_tech_type                   tech_type,
          void*                                     tech_params = NULL,
          ds_profile_db_gen_params_type*            gen_params = NULL) const;

        ds_profile_db_result_type SetParam(
          ds_profile_db_ident_type   ident,
          const ds_profile_db_info_type *info,
          boolean is_set = true);

        ds_profile_db_result_type ResetParam(ds_profile_db_ident_type ident,boolean mark_invalid);

        ds_profile_db_result_type GetParam (
          ds_profile_db_ident_type   ident,
          ds_profile_db_info_type    *info) const;

        bool CompareParam (
          ds_profile_db_ident_type         ident,
          const ds_profile_db_info_type    *info) const;

        ds_profile_db_profile_num_type GetNum(void) const;        
      
        bool IsPersistent(void) const;

        ds_profile_db_supported_tech_mask GetSupportedTechMask(void) const;

        ds_profile_db_subs_type GetSubcriptionId(void) const;

        ds_profile_db_result_type GetChangedIdents (
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_ident_type** idents = NULL,
          uint16* idents_count = NULL,
          ds_profile_db_ident_type** idents_gen = NULL,
          uint16* idents_count_gen  = NULL
        ) const;

        ds_profile_db_result_type GetParamInUse (
          ds_profile_db_ident_type   ident,
          ds_profile_db_info_type    *info,
          ds_profile_db_tech_type    tech,
          boolean                    is_tech_param_set
          );

        static size_t GetCStructSize(ds_profile_db_tech_type tech);        
        static size_t GetMaxCStructSize();

        static ds_profile_db_result_type 
          GetDefaultCParams(ds_profile_db_tech_type tech, void* c_params);

        static ds_profile_db_result_type ValidateTech(ds_profile_db_tech_type tech_type);
        static ds_profile_db_result_type ValidateTechMask(ds_profile_db_supported_tech_mask tech_mask);
        static ds_profile_db_result_type ValidateSupportedTechMask(ds_profile_db_supported_tech_mask
                                                                    tech_mask);
        static ds_profile_db_result_type ValidateProfileNum(ds_profile_db_profile_num_type num);
        static ds_profile_db_result_type ValidateSubscriptionId(ds_profile_db_subs_type subs);
        static ds_profile_db_subs_type   GetDefaultSubscriptionId();
        static ds_profile_db_result_type ValidateProfileNumPerTech(ds_profile_db_profile_num_type num,
                                                                   ds_profile_db_supported_tech_mask tech_mask);
        ds_profile_db_result_type SetSubscriptionId(ds_profile_db_subs_type subsciption_id, bool track = true);
        static void SetValidationFunction();

        void SetReadTime(uint32 set_time);
        uint32 GetReadTime() const;


      protected:        
            
      private:

        Profile(ParamsGeneral* _paramsGeneral);

        Profile* Clone() const;        
       
        /* do not define, this is to prevent copying the object */
        Profile& operator=(const Profile&);
        Profile(const Profile&);

        int IdentToTech(ds_profile_db_ident_type ident) const;

        ds_profile_db_result_type GetFieldInfo (
          ds_profile_db_ident_type ident,
          unsigned int* offset, 
          uint16* len,
          void** buf) const;

        ds_profile_db_result_type GetParamsFromIdent(
          ds_profile_db_ident_type ident,
          Params** params) const;

        void SetPersistency(bool persistent);

        

        void SetNumber(ds_profile_db_profile_num_type profile_number);

        virtual const void* GetCParamsPtr(ds_profile_db_tech_type tech) const; 
            
        ds_profile_db_result_type GetChangedIdentsInternal (
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_ident_type** idents,
          uint16* idents_count
        ) const;


        ds_profile_db_result_type GetChangedIdentsInternal (  
          ds_profile_db_ident_type** idents,
          uint16* idents_count
        ) const;

        void ResetParamTracking();

        template <class TechClass>
        static ds_profile_db_result_type InitTech(ds_profile_db_tech_type tech);
        
        Params* paramsArray[DB_TECH_MAX];
        ParamsGeneral* paramsGeneral;
        uint32  profile_read_time;
                
        /* File needs to access private functions to efficiently implement read/write */
        friend class File;

        /* DBMgr needs to set up params which are not publicly modifiable */ 
        friend class DBMgr;

        friend class CacheMgr;
    }; /* class Profile */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_PROFILE_H */
