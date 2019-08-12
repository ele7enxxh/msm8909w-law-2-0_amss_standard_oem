#ifndef DS_PROFILE_CACHE_MGR_H
#define DS_PROFILE_CACHE_MGR_H
/*===========================================================================
  @file ds_Profile_CacheMgr.h

  This file defines DB singleton class which is responsible for dispatching 
  the calls from C API to the correct Profile Manager

                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  6/9/2014   bh  Created module
  

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_profile_db.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_TechMgr.h"
#include "ds_Profile_Mutex.h"
#include "ds_Profile_CallbackMgr.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

using namespace ds::Profile;


namespace ds
{
  namespace Profile
  {
    class CacheMgr
    {
 
      public:

        /* currently cache only has common profile params for complex profiles */
        struct cache_entry{
          cache_entry(): num(DB_UNSPECIFIED_PROFILE_NUM), tech_mask(0), subs(DB_ACTIVE_SUBSCRIPTION_NO_CHANGE), is_persistent(FALSE),  params(NULL){}
          ~cache_entry() { if (params) {free(params);} }
          ds_profile_db_profile_num_type num;
          ds_profile_db_supported_tech_mask tech_mask;
          ds_profile_db_subs_type subs;
          boolean is_persistent;
          ds_profile_db_common_tech_params_type* params;          
        }; 

        struct cache_entry_block{
          q_link_type  link;
          cache_entry  cache;
        };

        ds_profile_db_result_type UpdateCache(const Profile* profile);

        //************************************
        // Method:      UpdateOrCreateCache
        // FullName:    ds::Profile::DBMgr::UpdateOrCreateCache
        // Description: Updates existing or creates a new cache entry
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   const Profile * profile
        // Parameter:   bool overwrite
        // Parameter:   bool * new_profile - ouput param, true if new entry
        //              was created in cache. Can be NULL - ignored
        //************************************
        ds_profile_db_result_type UpdateOrCreateCache(
          const Profile* profile,
          bool overwrite = false,
          bool* new_profile = NULL);

        //************************************
        // Method:      CreateCache
        // FullName:    ds::Profile::DBMgr::CreateCache
        // Description: Creates new cache entry
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   const Profile * profile
        //************************************
        ds_profile_db_result_type CreateCache(const Profile* profile);

        //************************************
        // Method:      CreateCache
        // FullName:    ds::Profile::DBMgr::CreateCache
        // Description: Creates new cache entry
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   bool is_persistent
        // Parameter:   ds_profile_db_common_tech_params_type * common
        //************************************
        ds_profile_db_result_type CreateCache(
          ds_profile_db_profile_num_type num, 
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs,
          bool is_persistent, 
          ds_profile_db_common_tech_params_type* common);

        /* */
        //************************************
        // Method:      GetCache
        // FullName:    ds::Profile::DBMgr::GetCache
        // Description: Get lightweight cache entry stored internally in DB
        //  
        // Access:      private 
        // Returns:     cache_entry*
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   bool exact_tech_mask -  if false than any profile that 
        //              supports that requested tech_mask + additional technologies will also suit,
        //              if it is true, than only exact tech_mask will match
        //************************************
        cache_entry* GetCache(
          ds_profile_db_profile_num_type num, 
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs, 
          bool exact_tech_mask = true);


        //************************************
        // Method:      GetCache
        // FullName:    ds::Profile::DBMgr::GetCache
        // Description: Heavy operation that fetches cache from mode handlers
        //              and constructs Profile object. 
        //              !! Validates every param fetched from mode handlers !!
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   Profile * * profile - output param
        // Parameter:   bool exact_tech_mask
        //************************************
        ds_profile_db_result_type GetCache(
          ds_profile_db_profile_num_type num, 
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs,
          Profile** profile,
          bool exact_tech_mask = true);

        //************************************
        // Method:      GetCache
        // FullName:    ds::Profile::DBMgr::GetCache
        // Description: Same as above, lightweight internal cache entry 
        //              is provided instead of profile number and tech_mask
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   cache_entry * c_e
        // Parameter:   Profile * * profile
        //************************************
        ds_profile_db_result_type GetCache(cache_entry* c_e, Profile** profile);

        ds_profile_db_result_type RemoveCache(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,
           ds_profile_db_subs_type subs
        );

       void RemoveCacheForSubs(
         ds_profile_db_subs_type subs,
         ds_profile_db_num_tech_type *removed_entries,
         uint32 *count
         );

        uint8 GetNumOfCacheEntry(
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs
        );

        ds_profile_db_result_type GetProfilesFromInternalCache
        (
          Profile ***profiles, 
          uint16 *count,
          uint16 tech_mask,
          ds_profile_db_ident_type ident,
          const ds_profile_db_info_type *param_info,
          ds_profile_db_subs_type subs
        );

        ds_profile_db_result_type GetPersistence
        (
           ds_profile_db_profile_num_type num,
           ds_profile_db_subs_type subs,
           boolean *is_persistent
         );

        ds_profile_db_tech_type GetTechType
        (
          ds_profile_db_profile_num_type num,
          ds_profile_db_subs_type subs
        );

        ds_profile_db_result_type IsValidProfile
        (
          ds_profile_db_profile_num_type num,
          ds_profile_db_subs_type        subs,
          ds_profile_db_tech_type        tech_type,
          boolean                        *is_valid
        );

        CacheMgr();

        ~CacheMgr();

          /* DBMgr profile cache lightweight structure. Has a few basic profile
           properties and also common params, since they are handled by DB. 
           other tech params are stored in mode handlers. 

           Important! 
           ---------
           All profiles that DB is allowed to perform operations on should be 
           marked in this table 
         */
        q_type cache[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX];

    }; /* class CacheMgr */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_CACHE_MGR_H */

