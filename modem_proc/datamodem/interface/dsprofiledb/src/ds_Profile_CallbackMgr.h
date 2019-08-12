#ifndef DS_PROFILE_DB_CALLBACKMGR_H
#define DS_PROFILE_DB_CALLBACKMGR_H
/*===========================================================================
  @file ds_Profile_CallbackMgr.h

  This file defines abstract class for all callback functionality 
  in ds profile db
  
                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-06-18 am Created module  

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

    
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_profile_db.h"
#include "ds_Profile_Profile.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Profile
  {
    class CallbackMgr
    {
      public:        
          
        //************************************
        // Method:      Register
        // FullName:    ds::Profile::CallbackMgr::Register
        // Description: Register callback function in a queue of clients.
        //              The notification is done using the Notify
        //  
        // Access:      virtual public 
        // Returns:     ds_profile_db_callback_id_type, 0 in case of error
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num - profile num to observe
        //              a specific profile, or DB_UNSPECIFIED_PROFILE_NUM to observe technology
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask - technology, or a number of 
        //              technologies to be observed (see treat_tech_mask_as_OR)
        // Parameter:   ds_profile_db_cb_type cback - user callback function to be invoked
        // Parameter:   void * user_data - user data to be provided as is during callback
        // Parameter:   bool mode_handler - whether the client is mode handler 
        // Parameter:   bool treat_tech_mask_as_OR - if true tech_mask is treated as a collection of simple 
        //              technologies, otherwise it is treated as one complex technology
        //              See Notify for more details on how the callbacks are invoked
        //              
        //************************************
        virtual ds_profile_db_callback_id_type Register(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,                  
          ds_profile_db_cb_type cback,
          void *user_data,
          ds_profile_db_subs_type subs,
          bool mode_handler = false,
          bool treat_tech_mask_as_OR = false) = 0;


        //************************************
        // Method:      Deregister
        // FullName:    ds::Profile::CallbackMgr::Deregister
        // Description: Deregister the client from the queue of clients
        //  
        // Access:      virtual public 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_callback_id_type callback_id
        //************************************
        virtual ds_profile_db_result_type Deregister(ds_profile_db_callback_id_type callback_id,
                                                     ds_profile_db_subs_type subs) = 0;

         //************************************
        // Method:      DeregisterAllClientsPerSub
        // FullName:    ds::Profile::CallbackMgr::Deregister
        // Description: Deregister the client from the queue of clients
        //  
        // Access:      virtual public 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_callback_id_type callback_id
        //************************************
        virtual ds_profile_db_result_type DeregisterAllClientsPerSub(ds_profile_db_callback_id_node_type* list_node,
                                                                     ds_profile_db_subs_type subs) = 0;

        //************************************
        // Method:      Notify
        // FullName:    ds::Profile::CallbackMgr::Notify
        // Description: Notifies all previously registered clients about the changes
        //              in a provided profile with provided event.
        //              Mode handler clients will be notified immediately, in the 
        //              context of a caller, notification for regular clients 
        //              will be posted as a command to DS task - async notification
        //              Each client will be notified if the following 2 conditions are true :
        //              1. if client has registered for a particular profile number, it should 
        //                 be equal to the provided profile's number
        //              2. profile tech_mask includes the tech_mask provided during registration
        //                 OR has at least one tech from tech_mask provided during registration -> 
        //                 if treat_tech_mask_as_OR was true during registration
        //              3. In case of CREATE / MODIFY event, profile has changes in tech's 
        //                 specified by tech_mask during registration     
        //  
        // Access:      virtual public 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile * profile
        // Parameter:   ds_profile_db_event_type event
        //************************************
        virtual ds_profile_db_result_type Notify(
          Profile* profile,          
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs) = 0;

        //************************************
        // Method:      Notify
        // FullName:    ds::Profile::CallbackMgr::Notify
        // Description: same as above, but for multiple profiles. 
        //              Currently, the number of profiles in array should be 1
        //  
        // Access:      virtual public 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile * * profiles
        // Parameter:   uint16 profiles_count
        // Parameter:   ds_profile_db_event_type event
        //************************************
        virtual ds_profile_db_result_type Notify(
          Profile** profiles,
          uint16 profiles_count,
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs) = 0;

        //************************************
        // Method:      Notify
        // FullName:    ds::Profile::CallbackMgr::Notify
        // Description: same as above, but for multiple profiles. 
        //              Currently, the number of profiles in array should be 1
        //  
        // Access:      virtual public 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile * * profiles
        // Parameter:   uint16 profiles_count
        // Parameter:   ds_profile_db_event_type event
        //************************************
        virtual ds_profile_db_result_type Notify(
          Profile** profiles,
          uint16 profiles_count,
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs,
          bool notify_mh_only) = 0;


        //************************************
        // Method:      WithinCallback
        // FullName:    ds::Profile::CallbackMgr::WithinCallback
        // Description: Returns true if is invoked from within callback 
        //              (used to protect against using profile db API's from
        //              within callback)
        //
        //  
        // Access:      virtual public 
        // Returns:     bool
        // Qualifier:  
        //************************************
        virtual bool WithinCallback() = 0;
        
      protected:
        CallbackMgr() {};
        virtual ~CallbackMgr() = 0;
        
      private:            

        /* do not define, this is to prevent copying the object */
        CallbackMgr(const CallbackMgr&);
        CallbackMgr& operator=(const CallbackMgr&);

        
        //************************************
        // Method:      NotifyOnPowerUp
        // FullName:    ds::Profile::CallbackMgr::NotifyOnPowerUp
        // Description: Immediately invokes callback with POWER UP event
        //              (without registering the client to the queue)
        //              In this case function receives the list of all profiles 
        //              for power up event and processes them to find only those 
        //              that are relevant to the specified tech_mask and num
        //              Currently, only mode handlers will be notified
        //  
        // Access:      virtual private - it is private to suggest that it should
        //              only be used by internal clients (currently mode handlers)
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile * * profiles
        // Parameter:   uint16 profiles_count
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   ds_profile_db_cb_type cback
        // Parameter:   ds_profile_db_callback_id_type callback_id
        // Parameter:   void * user_data
        // Parameter:   bool treat_tech_mask_as_OR
        //************************************
        virtual ds_profile_db_result_type NotifyOnPowerUp(
          Profile** profiles,
          uint16 profiles_count,    
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_cb_type    cback,
          ds_profile_db_callback_id_type callback_id,
          ds_profile_db_event_type event,
          void* user_data,
          ds_profile_db_subs_type subs,
          bool treat_tech_mask_as_OR = false) = 0;

        /* to access NotifyOnPowerUp */
        friend class DBMgr;

    }; /* class CallbackMgr */
  } /* namespace Profile */
} /* namespace ds */

inline ds::Profile::CallbackMgr::~CallbackMgr(){}

#endif /* DS_PROFILE_DB_CALLBACKMGR_H */
