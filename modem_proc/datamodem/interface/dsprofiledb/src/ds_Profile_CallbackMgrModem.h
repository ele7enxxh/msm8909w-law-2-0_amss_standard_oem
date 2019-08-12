#ifndef DS_PROFILE_DB_CALLBACKMGR_MODEM_H
#define DS_PROFILE_DB_CALLBACKMGR_MODEM_H
/*===========================================================================
  @file ds_Profile_CallbackMgrModem.h

  This file defines concrete class for all callback functionality 
  in ds profile db - modem side
  
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
#include "ds_profile_db_internal.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_Mutex.h"
#include "ds_Profile_MemManager.h"
#include "ds_Profile_CallbackMgr.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

using namespace ds::Profile;


namespace ds
{
  namespace Profile
  {
    class CallbackMgrModem : public CallbackMgr
    {
      public:  
        
        DS_PROFILE_DB_USE_MEMORY_MANAGEMENT(PS_MEM_PROFILE_DB_CALLBACK_MGR_MODEM_TYPE);

        static CallbackMgrModem* Instance();
  
        virtual ds_profile_db_callback_id_type Register(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,                  
          ds_profile_db_cb_type cback,
          void *user_data,
          ds_profile_db_subs_type subs,
          bool mode_handler = false,
          bool treat_tech_mask_as_OR = false);

        virtual ds_profile_db_result_type Deregister(ds_profile_db_callback_id_type callback_id,
                                                     ds_profile_db_subs_type subs);
        
        virtual ds_profile_db_result_type DeregisterAllClientsPerSub(ds_profile_db_callback_id_node_type* list_node,
                                                                     ds_profile_db_subs_type subs);
        
        virtual ds_profile_db_result_type Notify(
          Profile** profiles,
          uint16 profiles_count,
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs);
        
        virtual ds_profile_db_result_type Notify(
          Profile** profiles,
          uint16 profiles_count,
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs,
          bool notify_mh_only);

        virtual ds_profile_db_result_type Notify(
          Profile* profile,          
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs);

        virtual bool WithinCallback();                                         

      private:  

        CallbackMgrModem();
        virtual ~CallbackMgrModem();

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
          bool treat_tech_mask_as_OR = false);

        static ds_profile_db_result_type Init();   
        static void Deinit(); 
               
        static CallbackMgrModem* theInstance;

        ds_util_list_hndl_type clients_per_number[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX];
        ds_util_list_hndl_type clients_per_techmask[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX];

        ds_util_list_hndl_type mh_clients_per_number[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX];
        ds_util_list_hndl_type mh_clients_per_techmask[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX];
        
        bool in_callback;        

        /* the list of all valid clients */

        ds_util_list_hndl_type valid_clients_ids[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX];
        ds_util_list_hndl_type mh_valid_clients_ids[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX];

        Mutex* mutex;

        //************************************
        // Method:      GatherCBCall
        // FullName:    ds::Profile::CallbackMgrModem::GatherCBCall
        // Description: Adds a particular callback call to the list 
        //              of gathered callbacks
        //  
        // Access:      virtual private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   Profile * * profiles
        // Parameter:   uint16 profiles_count
        // Parameter:   ds_profile_db_event_type event
        // Parameter:   ds_profile_db_cb_type cback
        // Parameter:   ds_profile_db_callback_id_type callback_id
        // Parameter:   void * user_data
        // Parameter:   ds_util_list_hndl_type gathered_callbacks
        //************************************
        virtual ds_profile_db_result_type GatherCBCall(
          Profile** profiles,
          uint16 profiles_count,
          ds_profile_db_event_type event,
          ds_profile_db_cb_type    cback,
          ds_profile_db_callback_id_type callback_id,
          void* user_data,
          ds_util_list_hndl_type gathered_callbacks);
                       
        //************************************
        // Method:      RegisterForNum
        // FullName:    ds::Profile::CallbackMgrModem::RegisterForNum
        // Description: Register client for a particular profile number
        //  
        // Access:      private 
        // Returns:     void*
        // Qualifier:  
        // Parameter:   ds_profile_db_profile_num_type num
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   ds_profile_db_cb_type cback
        // Parameter:   ds_profile_db_callback_id_type callback_id
        // Parameter:   void * user_data
        // Parameter:   bool mode_handler
        // Parameter:   bool treat_tech_mask_as_OR
        //************************************
        void* RegisterForNum(
          ds_profile_db_profile_num_type num,
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_cb_type cback,
          ds_profile_db_callback_id_type callback_id,
          void * user_data,
          ds_profile_db_subs_type subs,
          bool mode_handler = false, 
          bool treat_tech_mask_as_OR = false);

        //************************************
        // Method:      RegisterForTechMask
        // FullName:    ds::Profile::CallbackMgrModem::RegisterForTechMask
        // Description: Register client for the whole tech mask
        //  
        // Access:      private 
        // Returns:     void*
        // Qualifier:  
        // Parameter:   ds_profile_db_supported_tech_mask tech_mask
        // Parameter:   ds_profile_db_cb_type cback
        // Parameter:   ds_profile_db_callback_id_type callback_id
        // Parameter:   void * user_data
        // Parameter:   bool mode_handler
        // Parameter:   bool treat_tech_mask_as_OR
        //************************************
        void* RegisterForTechMask(
          ds_profile_db_supported_tech_mask tech_mask,
          ds_profile_db_cb_type cback,
          ds_profile_db_callback_id_type callback_id,
          void * user_data,
          ds_profile_db_subs_type subs,
          bool mode_handler = false,
          bool treat_tech_mask_as_OR = false); 

        ds_profile_db_result_type DeregisterInternal(void* callback_id, bool mode_hanler,
                                                     ds_profile_db_subs_type subs);

        //************************************
        // Method:      InvokeCBInternal
        // FullName:    ds::Profile::CallbackMgrModem::InvokeCBInternal
        // Description: Directly invokes the user callback
        //  
        // Access:      private 
        // Returns:     ds_profile_db_result_type
        // Qualifier:  
        // Parameter:   ds_profile_db_cb_type cback
        // Parameter:   ds_profile_db_callback_id_type callback_id
        // Parameter:   ds_profile_db_event_type event
        // Parameter:   Profile * * profiles
        // Parameter:   uint16 profiles_count
        // Parameter:   void * user_data
        //************************************
        ds_profile_db_result_type InvokeCBInternal(
          ds_profile_db_cb_type cback,
          ds_profile_db_callback_id_type callback_id,
          ds_profile_db_event_type event,
          Profile** profiles,
          uint16 profiles_count, 
          void *user_data,
          ds_profile_db_subs_type subs);        

        //************************************
        // Method:      FireCBListAndRelease
        // FullName:    ds::Profile::CallbackMgrModem::FireCBListAndRelease
        // Description: Iterates over the list of gathered callbacks, 
        //              invokes each one of them and releases it
        //  
        // Access:      private 
        // Returns:     void
        // Qualifier:  
        // Parameter:   ds_util_list_hndl_type gathered_callbacks
        //************************************
        void FireCBListAndRelease(ds_util_list_hndl_type gathered_callbacks,
                                  ds_profile_db_subs_type subs);

        //************************************
        // Method:      CommandHandler
        // FullName:    ds::Profile::CallbackMgrModem::CommandHandler
        // Description: Handler for the DS task commands
        //  
        // Access:      private static 
        // Returns:     void
        // Qualifier:  
        // Parameter:   ds_cmd_type * cmd_ptr
        //************************************
        static void CommandHandler(          
          ds_cmd_type  *cmd_ptr);

        ds_profile_db_result_type NotifyInternal(
          Profile** profiles,
          uint16 profiles_count,
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs);

        ds_profile_db_result_type NotifyInternal(
          Profile** profiles,
          uint16 profiles_count,
          ds_profile_db_event_type event,
          ds_profile_db_subs_type subs,
          bool notify_MH_only);

        //************************************
        // Method:      NotifyHandler
        // FullName:    ds::Profile::CallbackMgrModem::NotifyHandler
        // Description: Handler for the Notify commands 
        //  
        // Access:      private static 
        // Returns:     void
        // Qualifier:  
        // Parameter:   void * cmd
        //************************************
        static void NotifyHandler(void* cmd);   

        static void NotifyRefreshHandler(void *cmd);


        //************************************
        // Method:      NotifyMH
        // FullName:    ds::Profile::CallbackMgrModem::NotifyMH
        // Description: Synchronous mode handler notification
        //  
        // Access:      private static 
        // Returns:     void
        // Qualifier:  
        // Parameter:   void * cmd
        //************************************
        static void NotifyMH(void* cmd);

        static void NotifyTechMaskClients(
          ds_profile_db_notify_cmd_type* cmd, 
          ds_util_list_hndl_type clients_list,
          ds_util_list_hndl_type gathered_callbacks);

        static void NotifyNumClients(
          ds_profile_db_notify_cmd_type* cmd, 
          ds_util_list_hndl_type clients_list,
          ds_util_list_hndl_type gathered_callbacks);

        static void NotifyAllClients(
           ds_profile_db_notify_cmd_type* cmd, 
           ds_util_list_hndl_type clients_list,
           ds_util_list_hndl_type gathered_callbacks);

        static void DeRegsiterAllClients(
           ds_util_list_hndl_type clients_list,
           ds_profile_db_subs_type subs);

        static void InvokeCBHandler(void* cmd);

        /* used in list iterations */
        static int per_each_node_invoke_cb(void* data, va_list args);
        static int per_each_num_node_invoke_cb(void* data, va_list args);
        static int per_each_techmask_node_invoke_cb(void* data, va_list args);
        static int per_each_supported_techmask_clients_list(void* data, va_list args); 
        static int per_each_invoke_cb_node_fire(void* data, va_list args);
        static int per_each_node_deregister_cb(void* data, va_list args);

        friend class PlatformModem;
        friend void ::ds_profile_db_cmd_hdlr (
          ds_cmd_type  *cmd_ptr);

        static ds_profile_db_result_type CopyProfiles(
          Profile** profiles, 
          uint16 profiles_count, 
          Profile*** profiles_copy);

        static ds_profile_db_result_type CopyProfiles(
          Profile* profiles,           
          Profile*** profiles_copy);

        static void FreeProfiles(Profile** profiles, uint16 profiles_count);

    }; /* class CallbackMgr */
  } /* namespace Profile */
} /* namespace ds */

#endif /* DS_PROFILE_DB_CALLBACKMGR_MODEM_H */
