#ifndef DS_PROFILE_TECH_MGR_3GPP2_H
#define DS_PROFILE_TECH_MGR_3GPP2_H
/*===========================================================================
  @file ds_Profile_TechMgr3GPP2.h

  This file defines 3GPP2 Technology Manager class which is responsible for 
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
#include "ds_profile_db.h"
#include "ds_Profile_TechMgr.h"
#include "ds_Profile_MemManager.h"
#include "ds_profile_db_internal.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class TechMgr3GPP2 : public TechMgr
    {
      public:

        DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(PS_MEM_PROFILE_DB_TECH_MGR_3GPP2_TYPE);
  
        virtual bool CanReserveProfileOutsideOfAllocationRange(
          ds_profile_db_profile_num_type num,
          bool during_init = false,
          bool *overwrite = NULL,
          bool is_epc = false ) const;

        virtual bool CanDeleteProfile(
          ds_profile_db_profile_num_type num,
          boolean enable_deleting_default_profile = false) const;

        virtual bool CanWriteProfile(
          ds_profile_db_profile_num_type num) const;

        virtual bool ShouldBeTreatedAsNonPersistent(
          ds_profile_db_profile_num_type num) const;

        virtual bool AllowToCreateFileOnFirstModify(
          ds_profile_db_profile_num_type num) const;

        virtual ds_profile_db_result_type GetParamsFromCache(
          ds_profile_db_profile_num_type num,
          ds_profile_db_subs_type subs,
          void* tech_params, 
          ds_profile_db_gen_params_type* gen_params);


        virtual ds_profile_db_result_type GetDefaultProfileNum(
          ds_profile_db_profile_num_type * num,
          ds_profile_db_family_type family,
          ds_profile_db_subs_type subs) const;

        virtual ds_profile_db_result_type SetDefaultProfileNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_family_type family,
          ds_profile_db_subs_type subs) const;

        virtual ds_profile_db_result_type GetDefaultProfileNum(
          ds_profile_db_profile_num_type * num,
          ds_profile_db_family_type family) const; 

        virtual ds_profile_db_result_type SetDefaultProfileNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_family_type family) const;

        virtual void GetProfilesRange(
          ds_profile_db_profile_num_type* min_num,
          ds_profile_db_profile_num_type * max_num ) const;

        virtual ds_profile_db_result_type GetIsEpcParamSet(
          ds_profile_db_profile_num_type num,
          ds_profile_db_ident_type       ident,
          boolean *is_tech_param_set,
          ds_profile_db_subs_type subs) const;

        virtual void ResetNonPersistentCount(
          ds_profile_db_subs_type subs);

        TechMgr3GPP2();        

        void InitNumber(
          ds_profile_db_subs_type subs);

        virtual void SetTechTable(tech_func_table_type *techTbl);

        bool KDDI_or_OMH(ds_profile_db_profile_num_type num) const;

      private:
        tech_func_table_type tech3GPP2;

    }; /* class TechMgr3GPP2 */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_TECH_MGR_3GPP2_H */
