#ifndef DS_PROFILE_DB_MGR_H
#define DS_PROFILE_DB_MGR_H
/*===========================================================================
  @file ds_Profile_DBMgr.h

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
  2013-01-16 ez Created module
  2013-04-10 am Added implementation

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
#include "ds_Profile_CacheMgr.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

using namespace ds::Profile;


namespace ds
{
  namespace Profile
  {
    class DBMgr
    {
 
      public:

        DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(PS_MEM_PROFILE_DB_DBMGR_TYPE);
      
        static DBMgr* Instance();

        static ds_profile_db_result_type Init();
        static void Deinit(void);

        void PowerUpFinished();
      
        //************************************
        // Method:      InitCreationParams
        // FullName:    ds::Profile::DBMgr::InitCreationParams
        // Description: Used to init special structure used during profile creation
        //  
        // Access:      public static 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_creation_params_type * params
        //************************************
        static ds_profile_db_result_type InitCreationParams(ds_profile_db_creation_params_type* params);

        ds_profile_db_result_type CreateProfile(
          const ds_profile_db_creation_params_type* params,
          Profile* profile);

        ds_profile_db_result_type DeleteProfile(
          ds_profile_db_profile_num_type  profile_num,
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs,
          boolean enable_deleting_default_profile = false);
#ifdef FEATURE_USER_CONFIGURE_EFS
        ds_profile_db_result_type SetProfile(
          Profile* profile,
          bool delete_on_file = false);
#else
        ds_profile_db_result_type SetProfile(
          Profile* profile);
#endif
        ds_profile_db_result_type GetProfile(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs,
          Profile** profile);

        ds_profile_db_result_type GetParamsInUse(
          Profile*                            profile,
          ds_profile_db_ident_type            ident,
          ds_profile_db_info_type             *info,
          ds_profile_db_tech_type             tech_type,
          ds_profile_db_subs_type             subs);

        //************************************
        // Method:      GetProfiles
        // FullName:    ds::Profile::DBMgr::GetProfiles
        // Description: Get all profiles with specific tech_mask
        //  
        // Access:      public 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   Profile * * * profiles
        // Parameter:   uint16 * profiles_count
        //************************************
        ds_profile_db_result_type GetProfiles(
          ds_profile_db_supported_tech_mask tech_mask,                  
          ds_profile_db_subs_type subs,                 
          Profile*** profiles,
          uint16* profiles_count);

        //************************************
        // Method:      GetProfiles
        // FullName:    ds::Profile::DBMgr::GetProfiles
        // Description: Get a;l profiles with specific tech_mask and 
        //              with particular ident contents equal the provided info
        //  
        // Access:      public 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   ds_profile_db_ident_type ident
        // Parameter:   const ds_profile_db_info_type * param_info
        // Parameter:   Profile * * * profiles
        // Parameter:   uint16 * profiles_count
        //************************************
        ds_profile_db_result_type GetProfiles(
          ds_profile_db_supported_tech_mask tech_mask,  
          ds_profile_db_ident_type ident,
          const ds_profile_db_info_type* param_info,
          ds_profile_db_subs_type subs,
          Profile ***profiles,
          uint16* profiles_count);

        ds_profile_db_result_type GetDefaultProfileNum(
          ds_profile_db_profile_num_type *num,
          ds_profile_db_tech_type tech,
          ds_profile_db_family_type family);

        ds_profile_db_result_type SetDefaultProfileNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_tech_type tech,
          ds_profile_db_family_type family);

        ds_profile_db_result_type GetDefaultProfileNum(
          ds_profile_db_profile_num_type *num,
          ds_profile_db_tech_type tech,
          ds_profile_db_family_type family,
          ds_profile_db_subs_type subs);

        ds_profile_db_result_type SetDefaultProfileNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_tech_type tech,
          ds_profile_db_family_type family,
          ds_profile_db_subs_type subs);

        void CommandHandler(
           ds_cmd_type *cmd_ptr);

        //************************************
        // Method:      ResetProfile
        // FullName:    ds::Profile::DBMgr::ResetProfile
        // Description: Resets the whole profile
        //  
        // Access:      public 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        //************************************
        ds_profile_db_result_type ResetProfile(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs);
      
        //************************************
        // Method:      ResetProfileParam
        // FullName:    ds::Profile::DBMgr::ResetProfileParam
        // Description: Resets a particular profile param
        //  
        // Access:      public 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   ds_profile_db_ident_type ident
        // Parameter:   bool mark_invalid - 
        //              if true the ident will be marked as invalid 
        //              this is only relevant for few ident's that
        //              have validity flags, i.e qos / tft in 3GPP
        //************************************
        ds_profile_db_result_type ResetProfileParam(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_ident_type       ident,
          ds_profile_db_subs_type subs,
          boolean mark_invalid);

        ds_profile_db_result_type GetProfilesRange(
          ds_profile_db_supported_tech_mask tech_mask,  
          ds_profile_db_profile_num_type *min_num,
          ds_profile_db_profile_num_type *max_num);

        //************************************
        // Method:      RegisterForNotifications
        // FullName:    ds::Profile::DBMgr::RegisterForNotifications
        // Description: See CallbackMgr interface for more info
        //  
        // Access:      public 
        // Returns:     ds_profile_db_callback_id_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   ds_profile_db_cb_type cback
        // Parameter:   void * user_data
        // Parameter:   bool mode_handler
        // Parameter:   bool treat_tech_mask_as_OR
        //************************************
        ds_profile_db_callback_id_type RegisterForNotifications(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,                  
          ds_profile_db_cb_type cback,
          ds_profile_db_subs_type subs,
          void *user_data,
          bool mode_handler = false,
          bool treat_tech_mask_as_OR  = false);

        ds_profile_db_result_type DeregisterForNotifications(
          ds_profile_db_callback_id_type callback_id,
          ds_profile_db_subs_type subs);
        
        ds_profile_db_tech_type GetTechType(
          ds_profile_db_profile_num_type num,
          ds_profile_db_subs_type subs);
        
        ds_profile_db_result_type GetPersistence(
           ds_profile_db_profile_num_type num,
           ds_profile_db_subs_type subs,
           boolean *is_peristent);
        
        ds_profile_db_result_type IsValidProfile(
          ds_profile_db_profile_num_type num_type,
          ds_profile_db_subs_type        subs,
          ds_profile_db_tech_type        tech_type,
          boolean                        *is_valid);

        TechMgr* GetTechManager(ds_profile_db_tech_type tech);
		
        //************************************
        // Method:      MarkRangeInCacheDuringInit
        // FullName:    ds::Profile::DBMgr::MarkRangeInCacheDuringInit
        // Description: Used by MarkSpecialProfilesInCache
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   ds_profile_db_profile_num_type start_range
        // Parameter:   ds_profile_db_profile_num_type end_range
        // Parameter:   bool is_persistent
        //************************************
        ds_profile_db_result_type MarkRangeInCacheDuringInit(
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_profile_num_type start_range,
          ds_profile_db_profile_num_type end_range,
          bool is_persistent,
          ds_profile_db_subs_type subs);


                //************************************
        // Method:      StoreInCacheDuringInit
        // FullName:    ds::Profile::DBMgr::StoreInCacheDuringInit
        // Description: Invoked from within OnTheFlyToCache to store
        //              the profiles created on the fly
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile * * profiles
        // Parameter:   unsigned int profiles_count
        //************************************
        ds_profile_db_result_type StoreInCacheDuringInit(Profile **profiles, unsigned int profiles_count,
                                                         ds_profile_db_subs_type subs);


        //************************************
        // Method:      MarkSpecialProfilesInCache
        // FullName:    ds::Profile::DBMgr::MarkSpecialProfilesInCache
        // Description: Marks special profiles in cache as if they are present in the system.
        //              This is done to enable treating some special profiles like KDDI, OMH, etc
        //              by mode handlers (for backward compatibility). They are marked in cache
        //              to enable modify / read operations pass through
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        //************************************
        ds_profile_db_result_type MarkSpecialProfilesInCache(
          ds_profile_db_subs_type subs);

        void ReleaseRangeInCache(
          ds_profile_db_profile_num_type start,
          ds_profile_db_profile_num_type end,
          ds_profile_db_supported_tech_mask tech_mask,
          bool is_persistent,
          ds_profile_db_subs_type subs);

        //************************************
        // Method:      FromFileToCache
        // FullName:    ds::Profile::DBMgr::FromFileToCache
        // Description: Fetches profiles from file system to Cache
        //              Used during Init
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile * * profiles
        // Parameter:   unsigned int * profiles_count
        //************************************
        ds_profile_db_result_type FromFileToCache(         
          Profile **profiles,
          unsigned int* profiles_count,
          ds_profile_db_subs_type subs);   


        //************************************
        // Method:      OnTheFlyToCache
        // FullName:    ds::Profile::DBMgr::OnTheFlyToCache
        // Description: Creates special profiles on the fly
        //              Used during Init
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile * * profiles
        // Parameter:   unsigned int * profiles_count
        //************************************
        ds_profile_db_result_type OnTheFlyToCache(         
          Profile **profiles,
          unsigned int* profiles_count,
          ds_profile_db_subs_type subs);   

        /* some profiles we shout treat as non persistent, even though their flag is set to true 
           this is because they are managed in legacy file system by MH's */
        bool ShouldBeTreatedAsNonPersistent(const Profile* profile);

        void DeviceModeCommandHandler(
           ds3gsubsmgr_event_info_type *event_info_ptr);

        bool GetEPCSupportVal(void);

      private:

        static DBMgr* theInstance;

        bool ds_profile_db_epc_support_val;


        Mutex* mutex;
        CallbackMgr* callbackMgr;

	      DBMgr();
        ~DBMgr();

        /* do not define, this is to prevent copying the object */
        DBMgr(const DBMgr&);
        DBMgr& operator=(const DBMgr&);
        
        void SetEPCSupportVal(
         uint8 epc_support_val);
        
        ds_profile_db_result_type GetProfilesInternal(
          ds_profile_db_supported_tech_mask tech_mask,                  
          ds_profile_db_ident_type ident,
          const ds_profile_db_info_type* param_info,
          ds_profile_db_subs_type subs,
          Profile ***profiles,
          uint16* profiles_count);
#ifdef FEATURE_USER_CONFIGURE_EFS
        ds_profile_db_result_type SetProfileInternal(
          Profile* profile,
          bool send_reset_event = false,
          bool delete_from_file = false
          );
#else
        ds_profile_db_result_type SetProfileInternal(
          Profile* profile,
          bool send_reset_event = false);
#endif
        /* TODO hide all the ugly number allocation logic in a separate class */
        //************************************
        // Method:      GetNumForProfile
        // FullName:    ds::Profile::DBMgr::GetNumForProfile
        // Description: Allocates a new number for the profile if the 
        //              provided profile has unspecified number or reserves
        //              the profile number in the system.
        //              Can fail if the number to reserve is not allowed
        //              for one of the tech's in profile tech_mask or 
        //              if there is no space left for the new profile in 
        //              the system
        //              If operation succeeded, provided profile will be 
        //              updated with the allocated number (or in case of 
        //              reservation it will remain intact), also the 
        //              amount of available profiles will be decreased
        //           
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   const Profile * profile
        // Parameter:   ds_profile_db_profile_num_type * new_num - output param,
        //              in case of profile reservation it will be equal to the number
        //              inside the provided profile. Can be NULL if client is not 
        //              interested in tracking the reserved / allocated number
        //              
        // Parameter:   bool overwrite - whether it is allowed to reserve a profile
        //              that is already present in the system
        // Parameter:   bool during_init - the rules for reserving profile numbers
        //              during init can be different from those during normal create
        //              operation
        //************************************
        ds_profile_db_result_type GetNumForProfile(
          const Profile* profile,
          ds_profile_db_profile_num_type* new_num = NULL, 
          ds_profile_db_subs_type subs = (ds_profile_db_subs_type)ps_sys_get_default_data_subscription(),
          bool overwrite = false,
          bool during_init = false);

        //************************************
        // Method:      GetNumForProfile
        // FullName:    ds::Profile::DBMgr::GetNumForProfile
        // Description: Same as above, but instead of providing the profile
        //              structure, user provides num, tech_mask and persistence flag
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   bool is_persistent
        // Parameter:   ds_profile_db_profile_num_type * new_num
        // Parameter:   bool overwrite
        // Parameter:   bool during_init
        //************************************
        ds_profile_db_result_type GetNumForProfile(
          ds_profile_db_profile_num_type num, 
          ds_profile_db_supported_tech_mask tech_mask,
          bool is_persistent, 
          ds_profile_db_subs_type subs, 
          ds_profile_db_profile_num_type* new_num = NULL, 
          bool overwrite = false, 
          bool during_init = false);

        //************************************
        // Method:      ReleaseNumForProfile
        // FullName:    ds::Profile::DBMgr::ReleaseNumForProfile
        // Description: Marks profile number as not used, increases
        //              the amount of available profiles in the system
        //              Can fail if profile is not allowed to be released       
        //
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   bool is_persistent
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   bool enable_deleting_default_profile
        //************************************
        ds_profile_db_result_type ReleaseNumForProfile(
          bool is_persistent,
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs,
          boolean enable_deleting_default_profile = false);


        //************************************
        // Method:      CheckAndReserveNum
        // FullName:    ds::Profile::DBMgr::CheckAndReserveNum
        // Description: Invoked from GetNumForProfile in case when the 
        //              profile number is specified 
        //              Does a couple of checks whether the profile number 
        //              can be reserved in the system.
        //              If the number is within the range that DB allocates numbers from
        //              this number will be reserved in it, so that we will not give it away
        //              for new profiles that will be created afterwards
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   bool during_init
        // Parameter:   bool overwrite
        //************************************
        ds_profile_db_result_type CheckAndReserveNum(
          ds_profile_db_profile_num_type num,                                                 
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_subs_type subs,
          bool during_init, 
          bool overwrite);

        //************************************
        // Method:      AllocateNum
        // FullName:    ds::Profile::DBMgr::AllocateNum
        // Description: Invoked from GetNumForProfile in case when the
        //              number was not specified. The number will be allocated
        //              from the pool of available free profile numbers
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type * num
        //************************************
        ds_profile_db_result_type AllocateNum(ds_profile_db_profile_num_type* num,
                                              ds_profile_db_subs_type subs,
                                              ds_profile_db_supported_tech_mask tech_mask);

        //************************************
        // Method:      FreeNum
        // FullName:    ds::Profile::DBMgr::FreeNum
        // Description: Is invoked from ReleaseNumForProfile. Release 
        //              the profile number from pool of numbers (in case
        //              it was previously allocated from there), also checks
        //              that the profile can be released
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask mask
        // Parameter:   bool enable_deleting_default_profile
        //************************************
        ds_profile_db_result_type FreeNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask mask,
          boolean enable_deleting_default_profile = false,
          ds_profile_db_subs_type subs = (ds_profile_db_subs_type)ps_sys_get_default_data_subscription());


        void ResetNumbers(
          ds_profile_db_subs_type subs);

        ds_profile_db_result_type DecProfileCount(
          bool is_persistent,
          ds_profile_db_supported_tech_mask mask,
          ds_profile_db_subs_type subs);

        ds_profile_db_result_type IncProfileCount(
          bool is_persistent,
          ds_profile_db_supported_tech_mask mask,
          ds_profile_db_subs_type subs);

        //************************************
        // Method:      NotifyClients
        // FullName:    ds::Profile::DBMgr::NotifyClients
        // Description: Notifies client with particular ever. 
        //              This is invoked each time an operation is performed
        //              on a DB with the provided profile. See CallbackMgr 
        //              interface for more details
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile * profile
        // Parameter:   ds_profile_db_event_type event
        //************************************
        ds_profile_db_result_type NotifyClients(
          Profile* profile, 
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs);

        //************************************
        // Method:      NotifyClientWithPowerUpEvent
        // FullName:    ds::Profile::DBMgr::NotifyClientWithPowerUpEvent
        // Description: Same as above, but used only for POWER UP events
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   bool treat_tech_mask_as_OR
        // Parameter:   ds_profile_db_cb_type cback
        // Parameter:   ds_profile_db_callback_id_type callback_id
        // Parameter:   void * user_data
        //************************************
        ds_profile_db_result_type NotifyClientWithPowerUpEvent(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,
          bool treat_tech_mask_as_OR, 
          ds_profile_db_cb_type cback,
          ds_profile_db_callback_id_type callback_id,
          void* user_data,
          ds_profile_db_subs_type subs,
          ds_profile_db_event_type event = DB_INVALID_EVENT,
          Profile** profiles = NULL,
          uint16 profile_count = 0);

        //************************************
        //  Method:      NotifyClientsDuringRefresh
        // FullName:    ds::Profile::DBMgr::NotifyClientsDuringRefresh
        // Description: Notifies client with particular ever. 
        //              This is invoked each time an operation is performed
        //              on a DB with the provided profile. See CallbackMgr 
        //              interface for more details
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile **profile
        // Parameter:   uint16 profile_count
        // Parameter:   ds_profile_db_event_type event
        // Parameter:   ds_profile_db_subs_type subs
        //************************************
        ds_profile_db_result_type NotifyClientsDuringRefresh(
          Profile **profiles,
          uint16 profile_count,
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs);

        //************************************
        // Method:      PowerUpSequence
        // FullName:    ds::Profile::DBMgr::PowerUpSequence
        // Description: Invoked during Init
        //  
        // Access:      private static 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        //************************************
        static ds_profile_db_result_type PowerUpSequence();

        //************************************
        // Method:      CleanupProfiles
        // FullName:    ds::Profile::DBMgr::CleanupProfiles
        // Description: Used in many places to cleanup a number of 
        //              profiles (i.e during error handling)
        //  
        // Access:      private 
        // Returns:     void
        // Qualifier:  
        // Parameter:   unsigned int profiles_count
        // Parameter:   Profile * * profiles
        // Parameter:   bool remove_from_cache
        // Parameter:   bool dealocate_numbers
        //************************************
        void CleanupProfiles(
          unsigned int profiles_count,
          Profile** profiles,
          bool  remove_from_cache = false,
          bool  dealocate_numbers = false);
        
        ds_profile_db_result_type ReadFromEFSHelper(
         ds_profile_db_subs_type subs,
         ds_profile_db_event_type event,
         uint16 *profiles_count);

        void RefreshCommandHandler(
           ds_cmd_type *cmd_ptr);

        /* tech managers using for querying various policies in various tech's (i.e create / delete policies */
        TechMgr* techMgrs[DB_TECH_MAX];
        
        bool CanDeleteProfile(ds_profile_db_profile_num_type num, 
                              ds_profile_db_supported_tech_mask tech_mask,
                              boolean enable_deleting_default_profile = false);

        bool CanWriteProfile(ds_profile_db_profile_num_type num, ds_profile_db_supported_tech_mask tech_mask);


        /* normally, it is enforced that file creation for profiles which are being modified 
           have been issued. For some profiles we want to make an exception for this rule and
           actually allow creating the file during the modification if the file does not exist */
        bool AllowToCreateFileOnFirstModify(const Profile* profile);
        
        /* Profile numbers are allocated from this pool */
        bool numbersPool[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX][DS_PROFILE_DB_MAX_PROFILE_COUNT];

        /* profiles stored internally between the Init and PowerUpFinished operations
           Used for notifying mode handlers with power up events - NotifyClientWithPowerUpEvent */
        Profile** profiles_for_power_up;
        unsigned int profiles_count_for_power_up;
        bool is_powerup_subs_device_event;
        CacheMgr  cacheMgr;
        typedef struct
        {
          ds_profile_db_profile_num_type num;
          ds_profile_db_supported_tech_mask tech_mask;
          bool treat_tech_mask_as_OR;
          ds_profile_db_cb_type cback;
          ds_profile_db_callback_id_type callback_id;
          void* user_data;
        }powerup_Mh_Cb_Id;

        powerup_Mh_Cb_Id ids[2];


    }; /* class DBMgr */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_DB_MGR_H */
