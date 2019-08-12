#ifndef DS_PROFILE_TECH_MGR_H
#define DS_PROFILE_TECH_MGR_H
/*===========================================================================
  @file ds_Profile_TechMgr.h

  This file defines Technology Manager abstract class which is responsible for 
  defining policies related to a particular technology (i.e allocating a
  profile number within the valid range, set/get default profile, etc)

                Copyright (c) 2015 Qualcomm Technologies Incorporated.
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
#include "ds_3gpp_hdlr.h"
#include "ds707_pdn_context.h"
#include "ds707_data_session_profile.h"
#include "ds_profile_db_internal.h"


/*===========================================================================

                     COMMON DEFINES

===========================================================================*/
/* the amount of various profile types are defined here
   
   Important !
   EPC profiles considered to be profiles which are both 3GPP
   and 3GPP2, meaning that EPC profile takes 1 count for 3GPP technology 
   and 1 count for 3GPP2 technology. That means, that for example there can be
   only DS_PROFILE_DB_MAX_PERS_PROFILE_COUNT / 2 persistent EPC profiles in the
   system

*/
#define DS_PROFILE_DB_3GPP2_MAX_KDDI_PROFILE_MAX DATA_SESSION_JCDMA_NO_VALID_PROFILE

#define DS_PROFILE_DB_3GPP_MAX_NON_PERS_PROFILE_COUNT   (DS_UMTS_MAX_PDP_TRANSIENT_PROFILE_NUM)
#define DS_PROFILE_DB_3GPP2_MAX_NON_PERS_PROFILE_COUNT  (DS707_MAX_PDN_INSTANCES + \
                                                         DS_PROFILE_DB_3GPP2_MAX_KDDI_PROFILE_MAX + \
                                                         DATA_SESSION_NVRUIM_MAX_PROFILES)


#define DS_PROFILE_DB_MAX_PROFILE_COUNT   (DS_PROFILE_DB_MAX_PERS_PROFILE_COUNT + \
                                           DS_PROFILE_DB_3GPP_MAX_NON_PERS_PROFILE_COUNT   + \
                                           DS_PROFILE_DB_3GPP2_MAX_NON_PERS_PROFILE_COUNT)

#define DS_PROFILE_3GPP_PROFILE_COUNT     (DS_PROFILE_DB_3GPP_MAX_NON_PERS_PROFILE_COUNT + \
                                           DS_PROFILE_DB_MAX_PERS_PROFILE_COUNT)

static const int DB_3GPP_CREATE_RANGE_MIN = 1;
static const int DB_3GPP_CREATE_RANGE_MAX = 24;
static const int DB_3GPP2_DEFAULT_PROFILE = 0;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class TechMgr
    {
      public:

        TechMgr(int non_pers_count, ds_profile_db_tech_type _tech):
          tech(_tech)
        {
          for (int i = 0; i < DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX; i++)
          {
            non_pers_profile_count[i] = non_pers_count;
            memset(&number_pool_3gpp[i], 0, sizeof(uint8)*DS_PROFILE_3GPP_PROFILE_COUNT);
          }
        }

        virtual ~TechMgr();

        virtual bool CanReserveProfileOutsideOfAllocationRange(
          ds_profile_db_profile_num_type num, 
          bool during_init = false,
          bool *overwrite = NULL,
          bool is_epc = false) const = 0;

        virtual bool CanDeleteProfile(
          ds_profile_db_profile_num_type num,
          boolean enable_deleting_default_profile = false) const = 0;

        virtual bool CanWriteProfile(
          ds_profile_db_profile_num_type num) const = 0;

        virtual bool ShouldBeTreatedAsNonPersistent(
          ds_profile_db_profile_num_type num) const = 0;

        virtual bool AllowToCreateFileOnFirstModify(
          ds_profile_db_profile_num_type num) const = 0;

        virtual ds_profile_db_result_type GetParamsFromCache(
          ds_profile_db_profile_num_type num,
          ds_profile_db_subs_type subs,
          void* tech_params, 
          ds_profile_db_gen_params_type* gen_params) = 0;

        virtual int GetCount(bool is_persistent,
                             ds_profile_db_subs_type subs) const;
        virtual int DecCount(bool is_persistent,
                             ds_profile_db_subs_type subs);
        virtual int IncCount(bool is_persistent,
                             ds_profile_db_subs_type subs);

        virtual ds_profile_db_result_type GetDefaultProfileNum(
          ds_profile_db_profile_num_type * num,
          ds_profile_db_family_type family, 
          ds_profile_db_subs_type subs) const = 0; 

        virtual ds_profile_db_result_type SetDefaultProfileNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_family_type family, 
          ds_profile_db_subs_type subs) const = 0;

        virtual ds_profile_db_result_type GetDefaultProfileNum(
          ds_profile_db_profile_num_type * num,
          ds_profile_db_family_type family) const = 0; 

        virtual ds_profile_db_result_type SetDefaultProfileNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_family_type family) const = 0;

        virtual void GetProfilesRange(
          ds_profile_db_profile_num_type* min_num,
          ds_profile_db_profile_num_type * max_num ) const = 0;

        virtual ds_profile_db_result_type GetIsEpcParamSet(
          ds_profile_db_profile_num_type num,
          ds_profile_db_ident_type       ident,
          boolean *is_tech_param_set,
          ds_profile_db_subs_type subs) const = 0;

        virtual void ResetNonPersistentCount(
          ds_profile_db_subs_type subs) = 0;

        virtual void SetTechTable(tech_func_table_type *techTbl) = 0;

        virtual ds_profile_db_result_type AllocateNum(
         ds_profile_db_profile_num_type *num,
         ds_profile_db_subs_type subs,
         ds_profile_db_supported_tech_mask tech_mask);

        virtual ds_profile_db_result_type CheckAndReserveNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_subs_type subs,
          bool overwrite);

        virtual ds_profile_db_result_type FreeNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask mask,
          ds_profile_db_subs_type subs);

        static void ResetPersistentProfileCount();

    
        /* minimum and maximum range for profiles with allocated number
           (which is profiles created with unspecified profile number) 
        */
        static const int ALLOC_RANGE_MIN = 100;
        static const int ALLOC_RANGE_MAX = ALLOC_RANGE_MIN + DS_PROFILE_DB_MAX_PROFILE_COUNT;

      protected:
        int non_pers_profile_count[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX];
        uint8 number_pool_3gpp[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX][DS_PROFILE_3GPP_PROFILE_COUNT + 1];
                
    private:
      static int pers_profile_count; /* this is shared between all managers */

      ds_profile_db_tech_type tech;
    }; /* class TechMgr */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_TECH_MGR_H */
