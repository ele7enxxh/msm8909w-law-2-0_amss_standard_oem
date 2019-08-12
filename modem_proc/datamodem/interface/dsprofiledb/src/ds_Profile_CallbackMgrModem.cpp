/*===========================================================================
  @file ds_Profile_CallbackMgr.cpp

  This file implements concrete class for all callback functionality 
  in ds profile db - modem side

  User callbacks are stored in 2 lists :

  1. callbacks per specific profile
     This list consists of nodes with unique profile numbers (with maximum number
     of nodes as the maximum total number of profiles). Each node has an internal 
     list of clients per this profile number. 

  2. callbacks per technology mask
     This list consists of nodes with unique tech masks (with maximum number of nodes
     as the amount of current supported technology combinations). Each node has an 
     internal list of clients per this technology mask

  
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

                     INCLUDE FILES FOR MODULE

===========================================================================*/
extern "C"
{
#include "dstask_v.h"
}

#include "ds_profile_db.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_Platform.h"
#include "ds_Profile_Log.h"
#include <stdarg.h>
#include <stringl/stringl.h>
#include "ps_system_heap.h"
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_Profile_CallbackMgrModem.h"

extern "C"
{
#include "ds_util.h"
#include "ds_profile_os.h"
#include "dstaski.h"
}


using namespace ds::Profile;
/*===========================================================================

                     LOCAL DEFINITIONS AND DATATYPES

===========================================================================*/
/* each callback id for mode handler clients will have this bit turned on */
#define MH_CALLBACK_ID_FLAG 0x80000000


/* Command types and structs */
/*****************************/

/* Callback list types and structs */
/*****************************/
/*
  There are 2, 2 dimensional lists responsible for callback clients:
  List of clients registered per specific number, 
  List of clients registered per technology mask
  
  clients_per_number:

  50 -> client1 -> client2 -> .
  |
  52 -> client3 -> .
  |
  53 -> client4 -> client5 -> .


  clients_per_number:

  3GPP -> client1 -> client2 -> .
  |
  3GPP+3GPP2+COMMON -> client3 -> .
  |
  3GPP2 -> client4 -> client5 -> .

  Thus, external list nodes contain either a particular profile number, or
  a tech mask. Each external list node also contains a pointer to the internal
  list consisting of clients per that number or tech_mask
*/


typedef enum {
  DS_PROFILE_DB_NUM_CALLBACK_LIST, 
  DS_PROFILE_DB_TECHMASK_CALLBACK_LIST,
  DS_PROFILE_DB_SUBS_CALLBACK_LIST
} ds_profile_db_callback_list_type;


/* external node of num list */
struct ds_profile_db_num_list_node_type
{
  ds_util_list_hndl_type internal_list;
  ds_profile_db_profile_num_type num;
  ds_profile_db_subs_type subs;
  
  /* internal node */
  struct internal_list_node_type
  {
    /* each node has info to which external list type does it belong, 
       used during deregistration, for faster search */
    ds_profile_db_callback_list_type node_type;
    ds_profile_db_num_list_node_type* containing_node;
    ds_profile_db_supported_tech_mask tech_mask;
    bool treat_tech_mask_as_OR;
    ds_profile_db_cb_type cback;
    ds_profile_db_callback_id_type callback_id;
    void *user_data;
  };

/* used to enable more convenient type usage */
#define ds_profile_db_internal_num_node \
  ds_profile_db_num_list_node_type::internal_list_node_type
}; 


/* external node of num list */
struct ds_profile_db_techmask_list_node_type
{
  ds_util_list_hndl_type internal_list;
  ds_profile_db_supported_tech_mask tech_mask;
  bool treat_tech_mask_as_OR;
  ds_profile_db_subs_type subs;

  /* internal node */
  struct internal_list_node_type
  {
    /* each node has info to which external list type does it belong, 
       used during deregistration, for faster search */
    ds_profile_db_callback_list_type node_type;    
    ds_profile_db_techmask_list_node_type* containing_node;
    ds_profile_db_cb_type cback;
    ds_profile_db_callback_id_type callback_id;
    void *user_data;
  };  

//struct ds_profile_db_subs_list_node_type {
//  ds_util_list_hndl_type internal_list;
//  ds_profile_db_supported_tech_mask tech_mask;
//  bool treat_tech_mask_as_OR;
//  ds_profile_db_subs_type subs;
//
//  /* internal node */
//  struct internal_list_node_type {
//    /* each node has info to which external list type does it belong,
//       used during deregistration, for faster search */
//    ds_profile_db_callback_list_type node_type;
//    ds_profile_db_techmask_list_node_type* containing_node;
//    ds_profile_db_cb_type cback;
//    ds_profile_db_callback_id_type callback_id;
//    void *user_data;
//};




/* used to enable more convenient type usage */
#define ds_profile_db_internal_techmask_node \
  ds_profile_db_techmask_list_node_type::internal_list_node_type
}; 


/* node in the list of gathered callbacks */
struct ds_profile_db_invoke_cb_node_type
{
  Profile** profiles;
  uint16 profiles_count;
  ds_profile_db_event_type event;
  ds_profile_db_cb_type    cback;
  ds_profile_db_callback_id_type callback_id;
  void* user_data;
};

/*===========================================================================

                     GLOBAL DECLARATIONS

===========================================================================*/
CallbackMgrModem* CallbackMgrModem::theInstance = NULL;
/*===========================================================================

                     LOCAL FUNCTIONS

===========================================================================*/    
static ds_profile_db_callback_list_type GetInternalNodeType(void* node)
{
  ds_profile_db_callback_list_type type = *((ds_profile_db_callback_list_type*)node);

  return type;  
}

static int per_each_external_node(void* data, va_list args)
{ 
 
  ds_util_list_hndl_type internal_list = *((ds_util_list_hndl_type*)data);
  if (!internal_list)
  {
    return 1;
  }

  ds_util_itr_rel_hndl(internal_list);

  return 0;
}

static void ReleaseInternalLists(
  ds_util_list_hndl_type list)
{
  ds_util_list_do_for_every_node(list, per_each_external_node, false, NULL);
}

/* Used to determine whether a callback should be invoked for a profile with
   particular tech_mask to the client which requested a particular tech_mask

   If treat_tech_mask_as_OR - is true, callback will be invoked if there is at
   least one shared simple technology between callback_tech_mask and profile_tech_mask
   If treat_tech_mask_as_OR - is false, callback will be invoked only if 
   profile fully supports the whole tech_mask provided by client

   relevant_tech_mask - is an output param which specifies the part of the profile
   mask which is relevant for this particular client (This can be used to query whether
   params for this part were changed)
 */

static bool IsMaskSupported(
  ds_profile_db_supported_tech_mask callback_tech_mask,
  bool treat_tech_mask_as_OR,
  ds_profile_db_supported_tech_mask profile_tech_mask,
  ds_profile_db_supported_tech_mask* relevant_profile_tech_mask)
{
  bool result = false; 

  if (treat_tech_mask_as_OR)
  {
    if (relevant_profile_tech_mask)
    {
      *relevant_profile_tech_mask = callback_tech_mask & profile_tech_mask;
    }
    result = ((callback_tech_mask & profile_tech_mask) != 0);
  } else
  {
    if (relevant_profile_tech_mask)
    {
      *relevant_profile_tech_mask = callback_tech_mask;
    }
    result = DB_IS_SUBSET_OF_TECHMASK(callback_tech_mask, profile_tech_mask);
  }

  return result;
}

/*===========================================================================

                     FUNCTIONS USED IN LIST ITERATIONS

===========================================================================*/
static int find_num_clients_list(void* data, va_list args)
{
  ds_profile_db_profile_num_type* num;  
  ds_profile_db_num_list_node_type** num_list; 

  int res = -1;
  
  num = va_arg(args, ds_profile_db_profile_num_type*);    
  num_list = va_arg(args, ds_profile_db_num_list_node_type**);

  ds_profile_db_num_list_node_type* list_node =  (ds_profile_db_num_list_node_type*)data;

  if (*num == list_node->num)
  {
    *num_list = list_node;
    res = 0;
  }

  return res;
}

static int find_exact_techmask_clients_list(void* data, va_list args)
{
  ds_profile_db_supported_tech_mask* tech_mask;  
  bool treat_tech_mask_as_OR;
  ds_profile_db_techmask_list_node_type** techmask_list;
  ds_profile_db_subs_type subs;
  
  int res = -1;
  
  tech_mask = va_arg(args, ds_profile_db_supported_tech_mask*);    
  treat_tech_mask_as_OR = (va_arg(args, int) != 0);
  subs = (ds_profile_db_subs_type)va_arg(args, int);
  techmask_list = va_arg(args, ds_profile_db_techmask_list_node_type**);

  ds_profile_db_techmask_list_node_type* list_node =  (ds_profile_db_techmask_list_node_type*)data;

  if (*tech_mask == list_node->tech_mask && 
      treat_tech_mask_as_OR == list_node->treat_tech_mask_as_OR)
  {
    *techmask_list = list_node;
    res = 0;
  }

  return res;
}

int CallbackMgrModem::per_each_node_invoke_cb(void *data, va_list args)
{
  ds_profile_db_notify_cmd_type *cmd;
  ds_util_list_hndl_type *gathered_callbacks_ptr;

  int res = -1;
  ds_profile_db_callback_id_node_type *list_node =
    (ds_profile_db_callback_id_node_type *)data;
  ds_profile_db_callback_list_type type;
  ds_profile_db_cb_type cback;
  void *user_data = NULL;
  ds_profile_db_internal_num_node *temp_node = NULL;
  ds_profile_db_internal_techmask_node *temp_tech_mask_node = NULL;

  cmd = va_arg(args, ds_profile_db_notify_cmd_type *);
  gathered_callbacks_ptr = va_arg(args, ds_util_list_hndl_type *);
  type = GetInternalNodeType((void*)list_node->callback_data);


  void *callback_id_data = list_node->callback_data;

  switch (type)
  {
    case(DS_PROFILE_DB_NUM_CALLBACK_LIST):
      temp_node = (ds_profile_db_internal_num_node *)callback_id_data;
      cback = temp_node->cback;
      user_data = temp_node->user_data;
      break;
    case(DS_PROFILE_DB_TECHMASK_CALLBACK_LIST):
      temp_tech_mask_node = (ds_profile_db_internal_techmask_node *)callback_id_data;
      cback = temp_tech_mask_node->cback;
      user_data = temp_tech_mask_node->user_data;
      break;
    default:
      return DB_RESULT_FAIL;
  }
//error handling get it from deregister
  /* post command with specific cb info to the task */

  if (DS_SUCCESS != CallbackMgrModem::Instance()->GatherCBCall((Profile **)(cmd->profiles),
                                                               cmd->profiles_count,
                                                               (ds_profile_db_event_type)(cmd->event),
                                                               cback,
                                                               list_node->callback_id,
                                                               user_data,
                                                               *gathered_callbacks_ptr))
  {
    res = -1;
    goto bail;
  }

bail:

  return res;
}

int CallbackMgrModem::per_each_node_deregister_cb(void *data, va_list args)
{
  int res = -1;
  ds_profile_db_subs_type subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_1;

  //cmd = va_arg(args, ds_profile_db_notify_cmd_type*);
  //gathered_callbacks_ptr = va_arg(args, ds_util_list_hndl_type*);
  subs = (ds_profile_db_subs_type)va_arg(args, int);
  ds_profile_db_callback_id_node_type *list_node =
    (ds_profile_db_callback_id_node_type *)data;

  res = (int)CallbackMgrModem::theInstance->DeregisterAllClientsPerSub(list_node, subs);

  return res;
}


int CallbackMgrModem::per_each_num_node_invoke_cb(void* data, va_list args)
{
  ds_profile_db_notify_cmd_type* cmd;    
  ds_profile_db_supported_tech_mask relevant_tech_mask;
  ds_util_list_hndl_type* gathered_callbacks_ptr;
  
  int res = -1;
  
  cmd = va_arg(args, ds_profile_db_notify_cmd_type*);  
  gathered_callbacks_ptr = va_arg(args, ds_util_list_hndl_type*);

  ds_profile_db_internal_num_node* list_node = 
    (ds_profile_db_internal_num_node*)data;

  if (!IsMaskSupported(list_node->tech_mask, 
                       list_node->treat_tech_mask_as_OR,
                       ((Profile*)(cmd->profiles[0]))->GetSupportedTechMask(),
                       &relevant_tech_mask))
  {
    return res;
  }
  
  /* for modify event 
      if no idents relevant to this techmask were changed, do not invoke cb, but return success */
  if (DB_MODIFY_PROFILE_EVENT == cmd->event)
  {

    /* check if the relevant technology part in profile was changed */
    uint16 idents_changed, idents_changed_gen;
    if (DB_RESULT_SUCCESS != (((Profile*)(cmd->profiles[0]))->GetChangedIdents(relevant_tech_mask, 
                                                                               NULL,
                                                                               &idents_changed,
                                                                               NULL,
                                                                                &idents_changed_gen)))
    {
      goto bail;    
    }

    if (!(idents_changed + idents_changed_gen))
    {
      res = 0;
      goto bail;
    }  
  }

  PROFILE_DB_LOG_HIGH_3("per_each_num_node_invoke_cb(): posting Invoke CB command with callback_id 0x%x, event %d, profiles count %d",
                        list_node->callback_id, cmd->event, cmd->profiles_count);
    
  /* post command with specific cb info to the task */
  if (DS_SUCCESS != CallbackMgrModem::Instance()->GatherCBCall((Profile**)(cmd->profiles),
                                                            cmd->profiles_count,
                                                            (ds_profile_db_event_type)(cmd->event),
                                                            list_node->cback,
                                                            list_node->callback_id,
                                                            list_node->user_data,
                                                               *gathered_callbacks_ptr))
  {
    res = -1;
    goto bail;
  }    
  

bail:
  
  return res;
}

int CallbackMgrModem::per_each_techmask_node_invoke_cb(void* data, va_list args)
{
  ds_profile_db_notify_cmd_type* cmd;  
  ds_util_list_hndl_type* gathered_callbacks_ptr;

  int res = -1;  

  cmd = va_arg(args, ds_profile_db_notify_cmd_type*);     
  gathered_callbacks_ptr = va_arg(args, ds_util_list_hndl_type*);

  ds_profile_db_internal_techmask_node* list_node = 
    (ds_profile_db_internal_techmask_node*)data;
          
  PROFILE_DB_LOG_HIGH_3("per_each_techmask_node_invoke_cb(): posting Invoke CB command with callback_id 0x%x, event %d, profiles count %d",
                        list_node->callback_id, cmd->event, cmd->profiles_count);

  /* post command with specific cb info to the task */
  if (DS_SUCCESS != CallbackMgrModem::Instance()->GatherCBCall((Profile**)(cmd->profiles),
                                                           cmd->profiles_count,
                                                           (ds_profile_db_event_type)(cmd->event),
                                                           list_node->cback,
                                                           list_node->callback_id,
                                                           list_node->user_data,
                                                               *gathered_callbacks_ptr))
  {
    res = -1;
    goto bail;
  }

bail:

  return res;
}

int CallbackMgrModem::per_each_supported_techmask_clients_list(void* data, va_list args)
{  
  ds_profile_db_notify_cmd_type* cmd;   
  ds_util_list_hndl_type* gathered_callbacks_ptr;

  int res = -1;
  ds_profile_db_supported_tech_mask relevant_tech_mask;

  cmd = va_arg(args, ds_profile_db_notify_cmd_type*);   
  gathered_callbacks_ptr = va_arg(args, ds_util_list_hndl_type*);
  
  ds_profile_db_techmask_list_node_type* list_node =  (ds_profile_db_techmask_list_node_type*)data;
  

  if (!IsMaskSupported(list_node->tech_mask, 
                       list_node->treat_tech_mask_as_OR,
                       ((Profile*)(cmd->profiles[0]))->GetSupportedTechMask(),
                       &relevant_tech_mask))
  {
      return res;
  }


    /* for modify event 
     if no idents relevant to this techmask were changed, do not invoke cb, but return success */
  if (DB_MODIFY_PROFILE_EVENT == cmd->event)
  {

    /* check if the relevant technology part in profile was changed */
    uint16 idents_changed, idents_changed_gen;
    if (DB_RESULT_SUCCESS != (((Profile*)(cmd->profiles[0]))->GetChangedIdents(relevant_tech_mask, 
                                                                               NULL, 
                                                                               &idents_changed,
                                                                               NULL,
                                                                                &idents_changed_gen)))
    {
      return -1;    
    }
    if (!(idents_changed + idents_changed_gen))
    {
      return 0;
    }  
  }

  ds_util_list_do_for_every_node(list_node->internal_list,
                                 &CallbackMgrModem::per_each_techmask_node_invoke_cb,
                                 false,
                                 NULL,
                                 cmd,
                                 gathered_callbacks_ptr);      
  res = 0;
        
  return res;
}

int CallbackMgrModem::per_each_invoke_cb_node_fire(void* data, va_list args)
{
  ds_profile_db_invoke_cb_node_type* node = (ds_profile_db_invoke_cb_node_type*)data;
  ds_profile_db_subs_type subs = (ds_profile_db_subs_type)va_arg(args, int);

  CallbackMgrModem::Instance()->InvokeCBInternal(node->cback, 
                                                 node->callback_id,
                                                 node->event, 
                                                 node->profiles,
                                                 node->profiles_count,
                                                 node->user_data,
                                                 subs);

  return 0;
}

/*===========================================================================

                     HANDLERS

===========================================================================*/
void CallbackMgrModem::NotifyTechMaskClients(
  ds_profile_db_notify_cmd_type* cmd, 
  ds_util_list_hndl_type clients_list,
  ds_util_list_hndl_type gathered_callbacks)
{    
  ds_util_list_do_for_every_node(clients_list, 
                                 per_each_supported_techmask_clients_list,
                                 false,
                                 NULL,
                                 cmd,
                                 &gathered_callbacks);                             
}

void CallbackMgrModem::NotifyNumClients(
  ds_profile_db_notify_cmd_type* cmd, 
  ds_util_list_hndl_type clients_list,
  ds_util_list_hndl_type gathered_callbacks)
{
  int res;
  boolean found_num;
  ds_profile_db_num_list_node_type*  num_client_list_node = NULL;
  ds_profile_db_profile_num_type num = ((Profile*)(cmd->profiles[0]))->GetNum();
  
  res = ds_util_list_do_for_every_node(clients_list, 
                                       find_num_clients_list,
                                       true,
                                       &found_num,
                                       &num,
                                       &num_client_list_node);

  if (DS_SUCCESS != res || !found_num)
  {
    return;
  }

  ds_util_list_do_for_every_node(num_client_list_node->internal_list,
                                 per_each_num_node_invoke_cb,
                                 false,
                                 NULL,
                                 cmd,
                                 &gathered_callbacks);                                                                
}

void CallbackMgrModem::NotifyAllClients(
  ds_profile_db_notify_cmd_type *cmd,
  ds_util_list_hndl_type clients_list,
  ds_util_list_hndl_type gathered_callbacks)
{
  ds_util_list_do_for_every_node(clients_list,
                                 per_each_node_invoke_cb,
                                 false,
                                 NULL,
                                 cmd,
                                 &gathered_callbacks);
}

void CallbackMgrModem::DeRegsiterAllClients(
  ds_util_list_hndl_type clients_list,
  ds_profile_db_subs_type subs)
{
  ds_util_list_do_for_every_node(clients_list,
                                 per_each_node_deregister_cb,
                                 false,
                                 NULL,
                                 subs);
}

/*===========================================================================

                     MEMBER FUNCTIONS

===========================================================================*/
CallbackMgrModem::CallbackMgrModem(void) : in_callback(false),
                                     mutex(NULL)                                     
{
  int iter = 0;
  for (iter = 0; iter < DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX; iter++)
  {
    clients_per_number[iter] = NULL;
    clients_per_techmask[iter] = NULL;
    mh_clients_per_number[iter] = NULL;
    mh_clients_per_techmask[iter] = NULL;
    valid_clients_ids[iter] = NULL;
    mh_valid_clients_ids[iter] = NULL;
  }
}


CallbackMgrModem* CallbackMgrModem::Instance(void)
{
  return theInstance;
}

ds_profile_db_result_type CallbackMgrModem::Init(void)
{  
  ds_profile_db_result_type res;

  if (theInstance)
  {
    PROFILE_DB_LOG_ERROR("Init(): was already invoked");
    return DB_RESULT_ERR_INVAL;
  } 

  theInstance = new CallbackMgrModem();
  if (!theInstance)
  {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }
  
  if (!(theInstance->mutex = Platform::Instance()->GetMutex()))
  {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }

  for (uint32 iter = 0; iter < DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX; iter++)
  {
    if (ds_util_list_get_hndl(&theInstance->valid_clients_ids[iter]) != DS_SUCCESS)
    {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }

    if (ds_util_list_get_hndl(&theInstance->mh_valid_clients_ids[iter]) != DS_SUCCESS)
    {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }

    if (ds_util_list_get_hndl(&theInstance->clients_per_number[iter]) != DS_SUCCESS)
    {
    res =  DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }

    if (ds_util_list_get_hndl(&theInstance->clients_per_techmask[iter]) != DS_SUCCESS)
    {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }

    if (ds_util_list_get_hndl(&theInstance->mh_clients_per_number[iter]) != DS_SUCCESS)
    {
    res =  DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }

    if (ds_util_list_get_hndl(&theInstance->mh_clients_per_techmask[iter]) != DS_SUCCESS)
    {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }
  }

  return DB_RESULT_SUCCESS;

bail:
  if(theInstance)
  {
    for (uint32 iter = 0; iter < DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX; iter++)
    {
      ds_util_list_rel_hndl(theInstance->valid_clients_ids[iter]);
      ds_util_list_rel_hndl(theInstance->mh_valid_clients_ids[iter]);
      ds_util_list_rel_hndl(theInstance->clients_per_number[iter]);
      ds_util_list_rel_hndl(theInstance->clients_per_techmask[iter]);
      ds_util_list_rel_hndl(theInstance->mh_clients_per_number[iter]);
      ds_util_list_rel_hndl(theInstance->mh_clients_per_techmask[iter]);
    }
  }
  if (theInstance)
  {
    delete theInstance;
  }

  theInstance = NULL;
  
  PROFILE_DB_LOG_ERROR_1("Init(): error %d occured", res);

  return res;
}

void CallbackMgrModem::Deinit(void)
{
  delete theInstance;
  theInstance = NULL; 
}



CallbackMgrModem::~CallbackMgrModem(void)
{  
  for (uint32 iter = 0; iter < DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX; iter++)
  {
    if (clients_per_number[iter])
    {
      ReleaseInternalLists(clients_per_number[iter]);
      ds_util_list_rel_hndl(clients_per_number[iter]);
      clients_per_number[iter] = NULL;
  }

    if (clients_per_techmask[iter])
    {
      ReleaseInternalLists(clients_per_techmask[iter]);
      ds_util_list_rel_hndl(clients_per_techmask[iter]);
      clients_per_techmask[iter] = NULL;
  }

    if (mh_clients_per_number[iter])
    {
      ReleaseInternalLists(mh_clients_per_number[iter]);
      ds_util_list_rel_hndl(mh_clients_per_number[iter]);
      mh_clients_per_number[iter] = NULL;
  }

    if (mh_clients_per_techmask[iter])
    {
      ReleaseInternalLists(mh_clients_per_techmask[iter]);
      ds_util_list_rel_hndl(mh_clients_per_techmask[iter]);
      mh_clients_per_techmask[iter] = NULL;
  }

    if (valid_clients_ids[iter])
    {
      ds_util_list_rel_hndl(valid_clients_ids[iter]);
      valid_clients_ids[iter] = NULL;
  }

    if (mh_valid_clients_ids[iter])
    {
      ds_util_list_rel_hndl(mh_valid_clients_ids[iter]);
      mh_valid_clients_ids[iter] = NULL;
    }
  }
  
  delete mutex;
  mutex = NULL;
}

void* CallbackMgrModem::RegisterForNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_cb_type cback,
  ds_profile_db_callback_id_type callback_id,
  void * user_data,
  ds_profile_db_subs_type subs,
  bool mode_handler, /* = false */
  bool treat_tech_mask_as_OR /* = false */)
{  
  /* try to find existing internal list of clients per given num */
  boolean found_num;
  ds_util_list_hndl_type clients = (mode_handler) ? mh_clients_per_number[0] : clients_per_number[subs];

  ds_profile_db_num_list_node_type*  num_client_list_node = NULL;
  if (DS_SUCCESS != ds_util_list_do_for_every_node(clients,
                                                   find_num_clients_list, 
                                                   true,
                                                   &found_num, 
                                                   &num,
                                                   &num_client_list_node))
  {
    return 0;
  }

  /* set internal list to be the one that was found or to a new one if wasn't */
  ds_util_list_hndl_type internal_list;
  if (found_num)
  {
    internal_list = num_client_list_node->internal_list;
  } else
  {
    if (ds_util_list_get_hndl(&internal_list) != DS_SUCCESS)
    {
      return 0;
    }
    
    /* add this internal list to external one */
    ds_profile_db_num_list_node_type num_node;
    num_node.num = num;
    num_node.internal_list = internal_list;
    void*  tmp_list_data_ptr;
    if (NULL == (tmp_list_data_ptr = ds_util_list_add_node(clients,
                                                           &num_node, 
                                                           sizeof(num_node))))
    {
      ds_util_list_rel_hndl(internal_list);
      return 0;
    }
    num_client_list_node = (ds_profile_db_num_list_node_type*)tmp_list_data_ptr;
  }

  /* now add node with all the info to internal list */
  ds_profile_db_internal_num_node info_node;

  void* list_info_node_ptr;
  
  info_node.node_type = DS_PROFILE_DB_NUM_CALLBACK_LIST;
  info_node.containing_node = num_client_list_node;
  info_node.cback = cback;
  info_node.tech_mask = tech_mask;
  info_node.treat_tech_mask_as_OR = treat_tech_mask_as_OR;
  info_node.user_data = user_data;
  info_node.callback_id = callback_id;

  list_info_node_ptr = ds_util_list_add_node(internal_list, &info_node, sizeof(info_node));

  if (NULL == list_info_node_ptr)
  {
    /* release the list if we created it */
    if (!found_num)
    {
      ds_util_list_rel_hndl(internal_list);
      return 0;
    }
  }

  return list_info_node_ptr;
}

void* CallbackMgrModem::RegisterForTechMask(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_cb_type cback,
  ds_profile_db_callback_id_type callback_id,
  void * user_data,
  ds_profile_db_subs_type subs,
  bool mode_handler /*  = false */,
  bool treat_tech_mask_as_OR /* = false */)
{  
  /* try to find existing internal list of clients per given techmask */
  boolean found_tech_mask;
  ds_profile_db_techmask_list_node_type*  techmask_client_list_node = NULL;

  ds_util_list_hndl_type clients = (mode_handler) ? mh_clients_per_techmask[0] : clients_per_techmask[subs];
  if (DS_SUCCESS != ds_util_list_do_for_every_node(clients,
                                                   find_exact_techmask_clients_list, 
                                                   true,
                                                   &found_tech_mask, 
                                                   &tech_mask,
                                                   treat_tech_mask_as_OR,
                                                   (int)subs,
                                                   &techmask_client_list_node))
  {
    return 0;
  }

  /* set internal list to be the one that was found or to a new one if wasn't */
  ds_util_list_hndl_type internal_list;
  if (found_tech_mask)
  {
    internal_list = techmask_client_list_node->internal_list;
  } else
  {
    if (ds_util_list_get_hndl(&internal_list) != DS_SUCCESS)
    {
      return 0;
    }
    
    /* add this internal list to external one */
    ds_profile_db_techmask_list_node_type techmask_node;
    techmask_node.tech_mask = tech_mask;
    techmask_node.treat_tech_mask_as_OR = treat_tech_mask_as_OR;
    techmask_node.internal_list = internal_list;
    techmask_node.subs = subs;
    void*  tmp_list_data_ptr;
    if (NULL == (tmp_list_data_ptr = ds_util_list_add_node(clients,
                                                           &techmask_node,
                                                           sizeof(techmask_node))))
    {
      ds_util_list_rel_hndl(internal_list);
      return 0;
    }
    techmask_client_list_node = (ds_profile_db_techmask_list_node_type*)tmp_list_data_ptr;
  }

  /* now add node with all the info to internal list */
  ds_profile_db_internal_techmask_node info_node;
  void* list_info_node_ptr;
  
  
  info_node.node_type = DS_PROFILE_DB_TECHMASK_CALLBACK_LIST;  
  info_node.containing_node = techmask_client_list_node;
  info_node.cback = cback;  
  info_node.user_data = user_data;
  info_node.callback_id = callback_id;

  list_info_node_ptr = ds_util_list_add_node(internal_list, &info_node, sizeof(info_node));

  if (NULL == list_info_node_ptr)
  {
    /* release the list if we created it */
    if (!found_tech_mask)
    {
      ds_util_list_rel_hndl(internal_list);
      return 0;
    }
  }

  return list_info_node_ptr;
}

ds_profile_db_callback_id_type CallbackMgrModem::Register(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask, 
  ds_profile_db_cb_type cback, 
  void *user_data,
  ds_profile_db_subs_type subs,
  bool mode_handler, /* = false */
  bool treat_tech_mask_as_OR /* = false */)
{
  static uint32 static_callback_id = 1;
  uint32 curr_callback_id;

  if (NULL == cback)
  {
    PROFILE_DB_LOG_ERROR("Register(): NULL cback function pointer, exiting");
    return DB_RESULT_ERR_INVAL;
  }

  void* callback_info = 0;
  if (0 != mutex->Lock())
  {
    return 0;
  }
  AutoReleseMutex a_m(mutex);

  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return DB_RESULT_ERR_INVAL_SUBS_ID;
  }

  /* allocate a new number in a cyclic manner */
  /* for mode handlers turn on the MST bit */
  if (MH_CALLBACK_ID_FLAG == ++static_callback_id)
  {
    static_callback_id = 1;
  }
  if (mode_handler)
  {
    curr_callback_id = static_callback_id | MH_CALLBACK_ID_FLAG;
  } else
  {
    curr_callback_id = static_callback_id;
  }
  
  if (DB_UNSPECIFIED_PROFILE_NUM == num)
  {
    callback_info = RegisterForTechMask(tech_mask, cback, curr_callback_id, user_data, subs, mode_handler, treat_tech_mask_as_OR);
  } else
  {
    callback_info = RegisterForNum(num, tech_mask, cback, curr_callback_id, user_data, subs, mode_handler, treat_tech_mask_as_OR);
  }

  if (!callback_info)
  {
    PROFILE_DB_LOG_ERROR_2("Register(): could not register for num %d, tech_mask %d", num, tech_mask);
    return 0;
  }
  
  ds_profile_db_callback_id_node_type cb_node;
  cb_node.callback_id = curr_callback_id;
  cb_node.callback_data = callback_info;

  ds_util_list_hndl_type client_ids = (mode_handler) ? mh_valid_clients_ids[0] : valid_clients_ids[subs];

  if (ds_util_list_add(client_ids, &cb_node, sizeof(cb_node)) != DS_SUCCESS)
  {
    DeregisterInternal(callback_info, mode_handler, subs);
    return 0;
  }

  return curr_callback_id;
}

ds_profile_db_result_type CallbackMgrModem::Deregister(ds_profile_db_callback_id_type callback_id,
                                                       ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type res;
  void* callback_id_node = NULL;
  void* callback_id_data = NULL;

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return DB_RESULT_ERR_INVAL_SUBS_ID;
  }

  if (!callback_id)
  {
    PROFILE_DB_LOG_ERROR_1("Deregister(): invalid callback_id 0x%x", callback_id);
    return DB_RESULT_ERR_INVAL;
  }

  ds_util_list_hndl_type client_ids = (callback_id & MH_CALLBACK_ID_FLAG) ? mh_valid_clients_ids[0] : valid_clients_ids[subs];

  callback_id_node = ds_util_list_find_node(client_ids, &callback_id, sizeof(callback_id));
  if (!callback_id_node)
  {
    PROFILE_DB_LOG_ERROR_1("Deregister(): callback_id 0x%x is not in the list of valid callbacks", callback_id);
    res = DB_RESULT_ERR_INVAL;
    goto bail;
  }

  callback_id_data = ((ds_profile_db_callback_id_node_type *)callback_id_node)->callback_data;
  if (!callback_id_data)
  {
    res = DB_RESULT_FAIL;
    goto bail;
  }

  /* the callback id is not valid */
  if (ds_util_list_remove(client_ids, &callback_id, sizeof(callback_id)) != DS_SUCCESS)
  {
    return DB_RESULT_ERR_INVAL;
  }

  res = DeregisterInternal(callback_id_data, (callback_id & MH_CALLBACK_ID_FLAG) != 0, subs);

bail:

  if (DB_RESULT_SUCCESS != res)
  {
    PROFILE_DB_LOG_ERROR_1("Deregister(): error %d occured", res);
  }
  return res;
}

ds_profile_db_result_type CallbackMgrModem::DeregisterAllClientsPerSub(ds_profile_db_callback_id_node_type *list_node,
                                                                       ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type res;
  void *callback_id_data = NULL;
  ds_profile_db_callback_id_type callback_id = 0;
  callback_id_data = list_node->callback_data;
  //ds_profile_db_callback_list_type type = GetInternalNodeType(callback_id_data);


  if (!list_node->callback_id)
  {
    PROFILE_DB_LOG_ERROR_1("Deregister(): invalid callback_id 0x%x", list_node->callback_id);
    return DB_RESULT_ERR_INVAL;
  }

  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return DB_RESULT_ERR_INVAL_SUBS_ID;
  }

  callback_id = list_node->callback_id;
  //ds_util_list_hndl_type client_ids = (callback_id & MH_CALLBACK_ID_FLAG) ? mh_valid_clients_ids[0] : valid_clients_ids[subs];

  if (!callback_id_data)
  {
    res = DB_RESULT_FAIL;
    goto bail;
  }

  res = DeregisterInternal(callback_id_data, (callback_id & MH_CALLBACK_ID_FLAG) != 0, subs);

bail:

  if (DB_RESULT_SUCCESS != res)
  {
    PROFILE_DB_LOG_ERROR_1("Deregister(): error %d occured", res);
  }

  return res;
}

/*
ds_profile_db_result_type CallbackMgrModem::Deregister(ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type res;
  void* callback_id_node = NULL;
  void* callback_id_data = NULL;

  if (0 != mutex->Lock()) {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

//if (!callback_id) {
//  PROFILE_DB_LOG_ERROR_1("Deregister(): invalid callback_id 0x%x", callback_id);
//  return DB_RESULT_ERR_INVAL;
//}

  ds_util_list_hndl_type client_ids = (callback_id & MH_CALLBACK_ID_FLAG )? mh_valid_clients_ids : valid_clients_ids;

  callback_id_node = ds_util_list_find_node(client_ids, &callback_id, sizeof(callback_id));
  if (!callback_id_node) {
    PROFILE_DB_LOG_ERROR_1("Deregister(): callback_id 0x%x is not in the list of valid callbacks", callback_id);
    res = DB_RESULT_ERR_INVAL;
    goto bail;
  }

  callback_id_data = ((ds_profile_db_callback_id_node_type*)callback_id_node)->callback_data;
  if (!callback_id_data) {
    res = DB_RESULT_FAIL;
    goto bail;
  }

 
  if (ds_util_list_remove(client_ids, &callback_id, sizeof(callback_id)) != DS_SUCCESS) {    
    return DB_RESULT_ERR_INVAL;
  }

  res = DeregisterInternal(callback_id_data, (callback_id & MH_CALLBACK_ID_FLAG) != 0, subs);

bail:

  if (DB_RESULT_SUCCESS != res) {
    PROFILE_DB_LOG_ERROR_1("Deregister(): error %d occured", res);
  }

  return res;
}
*/



ds_profile_db_result_type CallbackMgrModem::DeregisterInternal(void *callback_id_data, bool mode_handler,
                                                               ds_profile_db_subs_type subs)
{ 
  ds_profile_db_callback_list_type type = GetInternalNodeType((void*)callback_id_data);
  void* containing_node = NULL;
  void* internal_node = NULL;
  uint32 containing_node_size = 0;
  uint32 internal_node_size = 0;
  ds_util_list_hndl_type main_list = NULL;
  ds_util_list_hndl_type internal_list = NULL;

  switch (type)
  {
    case(DS_PROFILE_DB_NUM_CALLBACK_LIST):
      internal_node = (ds_profile_db_internal_num_node*)callback_id_data;
      containing_node = ((ds_profile_db_internal_num_node*)callback_id_data)->containing_node;      
      main_list = mode_handler ? mh_clients_per_number[0] : clients_per_number[subs];
      containing_node_size = sizeof(ds_profile_db_num_list_node_type);
      internal_node_size = sizeof(ds_profile_db_internal_num_node);
      break;
    case(DS_PROFILE_DB_TECHMASK_CALLBACK_LIST):
      internal_node = (ds_profile_db_internal_techmask_node*)callback_id_data;
      containing_node = ((ds_profile_db_internal_techmask_node*)callback_id_data)->containing_node;
      main_list = mode_handler ? mh_clients_per_techmask[0] : clients_per_techmask[subs];
      containing_node_size = sizeof(ds_profile_db_techmask_list_node_type);
      internal_node_size = sizeof(ds_profile_db_internal_techmask_node);
      break;
    default:
      return DB_RESULT_FAIL;
  }

  if (!containing_node || !(internal_list = *((ds_util_list_hndl_type *)containing_node)))
  {
    PROFILE_DB_LOG_ERROR_2("DeregisterInternal(): containing_node 0x%p or internal_list 0x%p are invalid", 
                            containing_node, internal_list);
    return DB_RESULT_ERR_INVAL;
  }

  if (DS_SUCCESS != ds_util_list_remove(internal_list, internal_node, internal_node_size))
  {
    return DB_RESULT_ERR_INVAL;
  }
  
  /* if the list is empty, we need to remove it */
  if (!ds_util_list_get_size(internal_list))
  {
    ds_util_list_rel_hndl(internal_list);
    ds_util_list_remove(main_list, containing_node, containing_node_size);
  }
  
  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type CallbackMgrModem::Notify(
  Profile** profiles,
  uint16 profiles_count,
  ds_profile_db_event_type event,
  ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type res; 
  Profile** profiles_copy = NULL;

  /* they will be released in NotifyHandler() */
  if (DB_RESULT_SUCCESS != (res = CopyProfiles(profiles, profiles_count, &profiles_copy)))
  {
    return res;
  }

  if (DB_RESULT_SUCCESS != (res = NotifyInternal(profiles_copy, profiles_count, event, subs)))
  {
    goto bail;
  }

  return DB_RESULT_SUCCESS;

bail:

  FreeProfiles(profiles_copy, profiles_count);

  return res;
}

ds_profile_db_result_type CallbackMgrModem::Notify(
  Profile **profiles,
  uint16 profiles_count,
  ds_profile_db_event_type event,
  ds_profile_db_subs_type subs,
  bool notify_mh_only)
{
  ds_profile_db_result_type res;
  Profile **profiles_copy = NULL;

  /* they will be released in NotifyHandler() */
  if (DB_RESULT_SUCCESS != (res = CopyProfiles(profiles, profiles_count, &profiles_copy)))
  {
    return res;
  }

  if (DB_RESULT_SUCCESS != (res = NotifyInternal(profiles_copy, profiles_count, event, subs)))
  {
    goto bail;    
  }

  return DB_RESULT_SUCCESS;

bail:

  FreeProfiles(profiles_copy, profiles_count);

  return res;
}

ds_profile_db_result_type CallbackMgrModem::Notify(
  Profile* profile,
  ds_profile_db_event_type event,
  ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type res; 
  Profile** profiles_copy = NULL;

  /* they will be released in NotifyHandler() */
  if (DB_RESULT_SUCCESS != (res = CopyProfiles(profile, &profiles_copy)))
  {
    return res;
  }

  if (DB_RESULT_SUCCESS != (res = NotifyInternal(profiles_copy, 1, event, subs)))
  {
    goto bail;    
  }

  return DB_RESULT_SUCCESS;

bail:

  FreeProfiles(profiles_copy, 1);

  return res;
}


ds_profile_db_result_type CallbackMgrModem::NotifyInternal(
  Profile** profiles,
  uint16 profiles_count,
  ds_profile_db_event_type event,
  ds_profile_db_subs_type subs)
{
  /* POWER UP event is currently not supported by this API, use NotifyOnPowerUp() instead */
  if (DB_POWER_UP_EVENT == event)
  {
    PROFILE_DB_LOG_ERROR_1("NotifyInternal(): tried to use API with POWER_UP event, currently it is not supported in this API", event)
    return DB_RESULT_ERR_INVAL_OP;
  }

  /* multiple profiles are not currently supported by this API, user NotifyOnPowerUp()
     for multiple profiles in POWER_UP event */
  if (profiles_count > 1)
  {
    PROFILE_DB_LOG_ERROR_1("NotifyInternal(): tried to use API with %d profiles, currently only 1 profile is supported", profiles_count)
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != theInstance->mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  /* in this case mutex should be released manually, prior to sending user cb
     currently done in NotifyHandler() */
  //AutoReleseMutex a_m(theInstance->mutex);

  ds_cmd_type *ds_cmd = ds_allocate_cmd_buf(sizeof(ds_profile_db_callback_cmd_type));
  if (!ds_cmd)
  {
    theInstance->mutex->Release();
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  ds_cmd->hdr.cmd_id = DS_CMD_PROFILE_DB_EXT;

  ds_profile_db_callback_cmd_type *cmd = (ds_profile_db_callback_cmd_type *)(ds_cmd->cmd_payload_ptr);

  cmd->operation_type = DS_PROFILE_DB_NOTIFY_CMD;
  cmd->notify_cmd.event = event;
  cmd->notify_cmd.profiles = (void**)profiles;
  cmd->notify_cmd.profiles_count = profiles_count;
  cmd->notify_cmd.subs = subs;


  PROFILE_DB_LOG_HIGH_3("NotifyInternal(): posting %d cmd for event %d with profoile_count %d",
                        cmd->operation_type,
                        cmd->notify_cmd.event,
                        cmd->notify_cmd.profiles_count);

  if (event != DB_REFRESH_PROFILE_EVENT)
  {
  /* first, notify MH's synchronously (reuse the command) */
  NotifyMH(&(cmd->notify_cmd));
  }

  /* now notify all the rest asynchronously */
  ds_put_cmd_ext(ds_cmd);
  if (event == DB_REFRESH_PROFILE_EVENT)
  {
    theInstance->mutex->Release();
  }
  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type CallbackMgrModem::NotifyInternal(
  Profile **profiles,
  uint16 profiles_count,
  ds_profile_db_event_type event,
  ds_profile_db_subs_type subs,
  bool notify_MH_only)
{
  /* POWER UP event is currently not supported by this API, use NotifyOnPowerUp() instead */
  if (DB_POWER_UP_EVENT == event)
  {
    PROFILE_DB_LOG_ERROR_1("NotifyInternal(): tried to use API with POWER_UP event, currently it is not supported in this API", event)
    return DB_RESULT_ERR_INVAL_OP;
  }

  /* multiple profiles are not currently supported by this API, user NotifyOnPowerUp()
     for multiple profiles in POWER_UP event */
  if (profiles_count > 1)
  {
    PROFILE_DB_LOG_ERROR_1("NotifyInternal(): tried to use API with %d profiles, currently only 1 profile is supported", profiles_count)
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != theInstance->mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  /* in this case mutex should be released manually, prior to sending user cb
     currently done in NotifyHandler() */
  //AutoReleseMutex a_m(theInstance->mutex);

  ds_profile_db_notify_cmd_type cmd;

  cmd.event = event;
  cmd.profiles = (void **)profiles;
  cmd.profiles_count = profiles_count;
  cmd.subs = subs;

  /* PROFILE_DB_LOG_HIGH_3("NotifyInternal(): posting %d cmd for event %d with profoile_count %d",
                         cmd->operation_type,
                         cmd->notify_cmd.event,
                         cmd->notify_cmd.profiles_count);
                         */

  /* first, notify MH's synchronously (reuse the command) */
  NotifyMH(&cmd);
  return DB_RESULT_SUCCESS;
}

/* Should only be invoked for MH clients */
ds_profile_db_result_type CallbackMgrModem::NotifyOnPowerUp(
  Profile** profiles_for_power_up,
  uint16 profiles_count_for_power_up,    
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_cb_type    cback,
  ds_profile_db_callback_id_type callback_id,
  ds_profile_db_event_type event,
  void* user_data,
  ds_profile_db_subs_type subs,
  bool treat_tech_mask_as_OR /*= false */)
{
  Profile** profiles = NULL;
  unsigned int profiles_count = 0;
  ds_profile_db_result_type res;

  /* we need to filter out those profiles from the array 
     of those stored during init */

  /* first we need to count the number of relevant profiles */ 
  for (unsigned int i = 0; i < profiles_count_for_power_up; i++)
  {
    /* we are looking for a specific profile */
    if (DB_UNSPECIFIED_PROFILE_NUM != num)
    {
      if (profiles_for_power_up[i]->GetNum() != num)
      {
        continue;
      }
    }    


    if (IsMaskSupported(tech_mask, treat_tech_mask_as_OR, profiles_for_power_up[i]->GetSupportedTechMask(), NULL))
    {
      profiles_count++;
    }

  }

  if (profiles_count)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(profiles, profiles_count * sizeof(Profile*), Profile** );
    if (NULL == profiles)
    {
      res = DB_RESULT_ERR_OUT_OF_MEMORY;
      goto bail; 
    }

    /* now copy the profiles */
    for (unsigned int i = 0, j = 0; i < profiles_count_for_power_up && j < profiles_count; i++)
    {
      /* we are looking for a specific profile */
      if (DB_UNSPECIFIED_PROFILE_NUM != num)
      {
        if (profiles_for_power_up[i]->GetNum() != num)
        {
          continue;
        }
      } 

      if (IsMaskSupported(tech_mask, treat_tech_mask_as_OR, profiles_for_power_up[i]->GetSupportedTechMask(), NULL))
      {
        profiles[j] = Profile::CreateInstance(profiles_for_power_up[i]);
        if (!profiles[j])
        {
          res = DB_RESULT_ERR_OUT_OF_PROFILES;
          goto bail;
        }
        j++;
      }
    }
  }

  /* for mode handlers we do enable calling DB API's from within callback
     we rely on their dignity, MH's won't abuse this ... */
  if (DB_RESULT_SUCCESS != (res = InvokeCBInternal(cback, 
                                                   callback_id,
                                                   event,
                                                   profiles,
                                                   profiles_count,
                                                   user_data,
                                                   subs)))
  {
    goto bail;
  }

  res = DB_RESULT_SUCCESS;
  /* fall through */

bail:

  if (profiles)
  {
    for (unsigned int i = 0; i < profiles_count; i++)
    {
      if (profiles[i])
      {
        delete profiles[i];
        profiles[i] = NULL;
      }
    }

    PS_SYSTEM_HEAP_MEM_FREE (profiles);
  }

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}

ds_profile_db_result_type CallbackMgrModem::GatherCBCall(
  Profile** profiles,
  uint16 profiles_count,
  ds_profile_db_event_type event,
  ds_profile_db_cb_type    cback,
  ds_profile_db_callback_id_type callback_id,
  void* user_data,
  ds_util_list_hndl_type gathered_callbacks)
{
  ds_profile_db_invoke_cb_node_type node;
  node.profiles = profiles;
  node.profiles_count = profiles_count;
  node.event = event;
  node.cback = cback;
  node.callback_id = callback_id;
  node.user_data = user_data;

  if (ds_util_list_add(gathered_callbacks, &node, sizeof(node)) != DS_SUCCESS)
  {
    PROFILE_DB_LOG_ERROR("InvokeCB(): could not add to internal list while trying to invoke callback");
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  return DB_RESULT_SUCCESS;
}

bool CallbackMgrModem::WithinCallback(void)
{
  if (!IS_IN_DS_TASK())
{
    return false;
  }

  if (in_callback)
  {
    PROFILE_DB_LOG_HIGH("WithinCallback(): Calling DB api from within Callback, this is probably an error");
  }

  return in_callback;
}

void CallbackMgrModem::CommandHandler (  
  ds_cmd_type  *cmd_ptr
)
{    
  if (0 != theInstance->mutex->Lock())
  {
    return;
  }
  /* in this case mutex should be released manually, prior to sending user cb
     currently done in NotifyHandler() */
  //AutoReleseMutex a_m(theInstance->mutex);

  ds_profile_db_callback_cmd_type * cmd_info = 
    (ds_profile_db_callback_cmd_type *)(cmd_ptr->cmd_payload_ptr);

  if (NULL == cmd_info)
  {
    PROFILE_DB_LOG_ERROR("CommandHandler(): invalid cmd");
    goto bail;
  }
  
  PROFILE_DB_LOG_HIGH_1("CommandHandler(): arrived cmd %d",
                        cmd_info->operation_type);

  switch (cmd_info->operation_type)
  {
    case(DS_PROFILE_DB_NOTIFY_CMD):
      return NotifyHandler(&(cmd_info->notify_cmd)); /* mutex is released inside */       
    default:
      PROFILE_DB_LOG_ERROR_1("CommandHandler(): invalid cmd type %d",
                             cmd_info->operation_type);
      break;
  }
  
  /* fall through */
bail:

  theInstance->mutex->Release();

  return;
}

/* sync notifications for Mode Handler clients */
void CallbackMgrModem::NotifyMH(void* cmd)
{
  ds_util_list_hndl_type gathered_callbacks;
  ds_profile_db_notify_cmd_type *notify_cmd = (ds_profile_db_notify_cmd_type *)cmd;
  if (ds_util_list_get_hndl(&gathered_callbacks) != DS_SUCCESS)
  {
    PROFILE_DB_LOG_ERROR("NotifyMH(): could not allocate memory for gathered_callbacks list");
    theInstance->mutex->Release();
    return;
  }

  NotifyNumClients((ds_profile_db_notify_cmd_type *)cmd, theInstance->mh_clients_per_number[0], gathered_callbacks);
  NotifyTechMaskClients((ds_profile_db_notify_cmd_type *)cmd, theInstance->mh_clients_per_techmask[0], gathered_callbacks);

  theInstance->mutex->Release();

  /* for mode handlers we do enable calling DB API's from within callback
     we rely on their dignity, MH's won't abuse this ... */
  theInstance->FireCBListAndRelease(gathered_callbacks,
                                    (ds_profile_db_subs_type)notify_cmd->subs);
}

/* handler for the Notify command - async notifications for regular clients */
void CallbackMgrModem::NotifyHandler(void* cmd)
{
  ds_util_list_hndl_type gathered_callbacks;
  ds_profile_db_subs_type subs;
  ds_profile_db_notify_cmd_type *notify_cmd = (ds_profile_db_notify_cmd_type *)cmd;

  if (notify_cmd->event == DB_REFRESH_PROFILE_EVENT)
  {
    return NotifyRefreshHandler(cmd);
  }
  if (ds_util_list_get_hndl(&gathered_callbacks) != DS_SUCCESS)
  {
    PROFILE_DB_LOG_ERROR("NotifyHandler(): could not allocate memory for gathered_callbacks list");
    theInstance->mutex->Release();
    return;
  }
  subs = (ds_profile_db_subs_type)(notify_cmd->subs);
  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    ds_util_list_rel_hndl(gathered_callbacks);
    theInstance->mutex->Release();
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return;
  }
  NotifyNumClients((ds_profile_db_notify_cmd_type *)cmd, theInstance->clients_per_number[subs], gathered_callbacks);
  NotifyTechMaskClients((ds_profile_db_notify_cmd_type *)cmd, theInstance->clients_per_techmask[subs], gathered_callbacks);

  
  theInstance->mutex->Release();
  
  /* we are inside DCC task, just in case you are wondering */
  theInstance->in_callback = true;
  theInstance->FireCBListAndRelease(gathered_callbacks,
                                    (ds_profile_db_subs_type)notify_cmd->subs);
  theInstance->in_callback = false;

  //now, release the profiles
  FreeProfiles((Profile**)(((ds_profile_db_notify_cmd_type*)cmd)->profiles),
               ((ds_profile_db_notify_cmd_type*)cmd)->profiles_count);
}

void CallbackMgrModem::NotifyRefreshHandler(void *cmd)
{
  ds_util_list_hndl_type gathered_callbacks;
  ds_profile_db_subs_type subs;
  ds_profile_db_notify_cmd_type *notify_cmd = (ds_profile_db_notify_cmd_type *)cmd;
  if (ds_util_list_get_hndl(&gathered_callbacks) != DS_SUCCESS)
  {
    PROFILE_DB_LOG_ERROR("NotifyHandler(): could not allocate "
                         "memory for gathered_callbacks list");
    theInstance->mutex->Release();
    return;
  }
  subs = (ds_profile_db_subs_type)(notify_cmd->subs);
  if (DB_ACTIVE_SUBSCRIPTION_NO_CHANGE > subs ||
      DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX <= subs)
  {
    ds_util_list_rel_hndl(gathered_callbacks);
    theInstance->mutex->Release();
    PROFILE_DB_LOG_ERROR_1("ValidateSubscriptionId(): invalid subs_id %d", subs);
    return;
  }
  NotifyAllClients(notify_cmd, theInstance->valid_clients_ids[subs], gathered_callbacks);
  theInstance->FireCBListAndRelease(gathered_callbacks,
                                    (ds_profile_db_subs_type)notify_cmd->subs);
  //If event is Refresh event clean up the client list. Use ds util functions to clear the list.

  DeRegsiterAllClients(theInstance->valid_clients_ids[subs], subs);
  if( ds_util_list_reset(theInstance->valid_clients_ids[subs]))
  {
    PROFILE_DB_LOG_ERROR_1("Reset of valid client 0x%x failed", 
                            theInstance->valid_clients_ids[subs]);
  }
  theInstance->mutex->Release();
  //now, release the profiles
  FreeProfiles((Profile **)(((ds_profile_db_notify_cmd_type *)cmd)->profiles),
               ((ds_profile_db_notify_cmd_type *)cmd)->profiles_count);
}


ds_profile_db_result_type CallbackMgrModem::InvokeCBInternal(
  ds_profile_db_cb_type cback,
  ds_profile_db_callback_id_type callback_id,
  ds_profile_db_event_type event,
  Profile** profiles,
  uint16 profiles_count, 
  void *user_data,
  ds_profile_db_subs_type subs)
{
  /* if the callback id is not valid, that means we unregistered, do nothing */
  ds_util_list_hndl_type client_ids = (callback_id & MH_CALLBACK_ID_FLAG) ?
    mh_valid_clients_ids[0] : valid_clients_ids[subs];

  if (!callback_id || !ds_util_list_find_node(client_ids, &callback_id, sizeof(callback_id)))
  {
    PROFILE_DB_LOG_HIGH_1("InvokeCBInternal(): Invalid callback_id 0x%x, "
                          "ignoring", callback_id);
    return DB_RESULT_ERR_INVAL_OP;
  }

  PROFILE_DB_LOG_HIGH_4("InvokeCBInternal(): Invoking user callback with id "
                        "0x%x with event %d and %d profiles and subs %d",
                        callback_id, event, profiles_count, subs);   

  Profile** profiles_copy = NULL;
  if (DB_RESULT_SUCCESS != CopyProfiles(profiles, profiles_count, &profiles_copy))
  {
    PROFILE_DB_LOG_ERROR("InvokeCBInternal(): error while making a copy of "
                         "profiles for client");
    return DB_RESULT_ERR_OUT_OF_PROFILES;
  }

  /* each client gets a separate copy of profiles, clients are responsible to release them */
  (*(cback))(event, 
             (ds_profile_db_profile_managed_type**)profiles_copy,
             profiles_count,
             user_data,
             subs);

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type CallbackMgrModem::CopyProfiles(
  Profile** profiles, 
  uint16 profiles_count, 
  Profile*** profiles_copy)
{
  ds_profile_db_result_type res;

  if (0 == profiles_count)
  {
    *profiles_copy = 0;
    return DB_RESULT_SUCCESS;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(*profiles_copy, profiles_count * sizeof(Profile*), 
                            Profile**);
  if (NULL == *profiles_copy)
  {
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  memset(*profiles_copy, 0, profiles_count * sizeof(Profile*));

  for (unsigned int i = 0; i < profiles_count; i++)
  {
    (*profiles_copy)[i] = Profile::CreateInstance(profiles[i]);
    if (!((*profiles_copy)[i]))
    {
      res = DB_RESULT_ERR_OUT_OF_PROFILES;
      goto bail;
    }    
    PROFILE_DB_LOG_HIGH_2("CallbackMgrModem::CopyProfiles profiles Copy (): profile num %d, profile subs %d", (*profiles_copy)[i]->GetNum(),
                          (*profiles_copy)[i]->GetSubcriptionId()); 
  }  

  return DB_RESULT_SUCCESS;

bail:

  FreeProfiles(*profiles_copy, profiles_count);

  return res;
}

ds_profile_db_result_type CallbackMgrModem::CopyProfiles(
  Profile* profile,                                               
  Profile*** profiles_copy)
{
  ds_profile_db_result_type res;
  
  PS_SYSTEM_HEAP_MEM_ALLOC(*profiles_copy, 1 * sizeof(Profile*), 
                            Profile**);
  if (NULL == *profiles_copy)
  {
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  memset(*profiles_copy, 0, 1 * sizeof(Profile*));

  (*profiles_copy)[0] = Profile::CreateInstance(profile);
  if (!((*profiles_copy)[0]))
  {
    res = DB_RESULT_ERR_OUT_OF_PROFILES;
    goto bail;
  }

  return DB_RESULT_SUCCESS;

bail:

  FreeProfiles(*profiles_copy, 1);

  return res;
}

void CallbackMgrModem::FreeProfiles(Profile** profiles, uint16 profiles_count) 
{
  if (!profiles || !profiles_count)
  {
    return;
  }

  for (unsigned int i = 0; i < profiles_count; i++)
  {
    if (profiles[i])
    {
      delete profiles[i];
      profiles[i] = NULL;
    }
  }

  PS_SYSTEM_HEAP_MEM_FREE (profiles);
}

void CallbackMgrModem::FireCBListAndRelease(ds_util_list_hndl_type gathered_callbacks,
                                            ds_profile_db_subs_type subs)
{
  ds_util_list_do_for_every_node(gathered_callbacks, 
                                 per_each_invoke_cb_node_fire, 
                                 false,
                                 NULL,
                                 subs);

  ds_util_list_rel_hndl(gathered_callbacks);
}

