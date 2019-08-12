/******************************************************************************
  @file    ds_profile_db.c
  @brief   DS PROFILES DATABASE API implementation

  DESCRIPTION
  This file contains implementation of DS PROFILES DATABASE API.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/13   am      Created the module. First version of the file.
===========================================================================*/

/*---------------------------------------------------------------------------
                           INCLUDE FILES
---------------------------------------------------------------------------*/
#include <stdlib.h>
#include "ds_profile_db.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_DBMgr.h"
#include "ps_system_heap.h"
#include "ds_profile_db_internal.h"
#include <stringl/stringl.h>


using namespace ds::Profile;

#ifdef FEATURE_USER_CONFIGURE_EFS
  static void *heapMemoryForFile = NULL;
  static ds_profile_db_deleted_profile_entry* delete_profile_list_head_ptr = NULL;
  static uint8 profile_delete_count = 16;
#endif

/*Table to store the function pointers from Mode handlers*/
static tech_func_table_type tech_func_table[DB_TECH_MAX];

static ds_profile_db_common_params_map_type ds_profile_db_common_to_tech_params_tbl[] =
{
  {COMMON_TECH_PROFILE_PARAM_APN_DISABLE_FLAG, TECH_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG, TECH_3GPP2_PROFILE_PARAM_APN_ENABLED},
  {COMMON_TECH_PROFILE_PARAM_APN_CLASS, TECH_3GPP_PROFILE_PARAM_APN_CLASS, TECH_3GPP2_PROFILE_PARAM_APN_CLASS},
  {COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY, TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY, TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY},
  {COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY, TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY, TECH_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY},
  {COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY, TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY, TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY},
  {COMMON_TECH_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY, TECH_3GPP_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY, TECH_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY},
  {COMMON_TECH_PROFILE_PARAM_LINGER_PARAMS, TECH_3GPP_PROFILE_PARAM_LINGER_PARAMS, TECH_3GPP2_PROFILE_PARAM_LINGER_PARAMS},
  {COMMON_TECH_PROFILE_PARAM_PCSCF_REQ_FLAG, TECH_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG, TECH_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED},
  {COMMON_TECH_PROFILE_PARAM_AUTH_TYPE, TECH_3GPP_PROFILE_PARAM_AUTH_TYPE, TECH_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL},
  {COMMON_TECH_PROFILE_PARAM_AUTH_USERNAME, TECH_3GPP_PROFILE_PARAM_AUTH_USERNAME, TECH_3GPP2_PROFILE_PARAM_USER_ID},
  {COMMON_TECH_PROFILE_PARAM_AUTH_PASSWORD, TECH_3GPP_PROFILE_PARAM_AUTH_PASSWORD, TECH_3GPP2_PROFILE_PARAM_AUTH_PASSWORD},
  {COMMON_TECH_PROFILE_PARAM_OPERATOR_RESERVED_PCO, TECH_3GPP_PROFILE_PARAM_OPERATOR_RESERVED_PCO, TECH_3GPP2_PROFILE_PARAM_OP_PCO_ID},
  {COMMON_TECH_PROFILE_PARAM_MCC, TECH_3GPP_PROFILE_PARAM_MCC, TECH_3GPP2_PROFILE_PARAM_MCC},
  {COMMON_TECH_PROFILE_PARAM_MNC, TECH_3GPP_PROFILE_PARAM_MNC, TECH_3GPP2_PROFILE_PARAM_MNC},
  {COMMON_TECH_PROFILE_PARAM_USER_APP_DATA, TECH_3GPP_PROFILE_PARAM_USER_APP_DATA, TECH_3GPP2_PROFILE_PARAM_USER_APP_DATA},
  {COMMON_TECH_PROFILE_PARAM_PDP_TYPE, TECH_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE, TECH_3GPP2_PROFILE_PARAM_PDN_TYPE},
  {COMMON_TECH_PROFILE_PARAM_DNS_DHCP_REQ_FLAG, TECH_3GPP_PROFILE_PARAM_DNS_DHCP_REQ_FLAG, TECH_3GPP2_PROFILE_PARAM_DNS_DHCP_REQ_FLAG},
  {COMMON_TECH_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG, TECH_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG, TECH_3GPP2_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG}
};


/*===========================================================================
   Function implementations
===========================================================================*/
ds_profile_db_result_type ds_profile_db_init(void)
{
  return DBMgr::Init();
}

void ds_profile_db_deinit()
{
  DBMgr::Deinit();
}

ds_profile_db_profile_managed_type* ds_profile_db_alloc(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs
)
{
  return (ds_profile_db_profile_managed_type *)Profile::CreateInstance(tech_mask, subs);
}

ds_profile_db_result_type ds_profile_db_dealloc(
  ds_profile_db_profile_managed_type* profile
)
{
  if (profile)
  {
    PROFILE_DB_LOG_HIGH_1("Deleting Profile 0x%x", profile);
    delete ((Profile*)profile);
    profile = NULL;
  }
  else
  {
    PROFILE_DB_LOG_ERROR("NULL Profile pointer passed for dealloc");
    return DB_RESULT_FAIL;
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type ds_profile_db_set_param( 
  ds_profile_db_profile_managed_type *profile,
  ds_profile_db_ident_type            ident,
  const ds_profile_db_info_type       *info
)
{
  return ((Profile*)profile)->SetParam(ident, info);
}

ds_profile_db_result_type ds_profile_db_get_param( 
  ds_profile_db_profile_managed_type *profile,
  ds_profile_db_ident_type            ident,
  ds_profile_db_info_type             *info
)
{
  return ((Profile*)profile)->GetParam(ident, info);
}

ds_profile_db_result_type ds_profile_db_get_tech_params(
  const ds_profile_db_profile_managed_type* profile,
  ds_profile_db_tech_type                   tech_type,
  void*                                     tech_params,
  ds_profile_db_gen_params_type*            gen_params /* can be NULL */
)
{
  return ((Profile*)profile)->GetTechParams(tech_type, tech_params, gen_params);
}

ds_profile_db_profile_num_type ds_profile_db_get_profile_num(
  const ds_profile_db_profile_managed_type *profile
)
{
  return ((Profile*)profile)->GetNum();
}

boolean ds_profile_db_is_profile_persistent(
  const ds_profile_db_profile_managed_type *profile
)
{
  return ((Profile*)profile)->IsPersistent();
}

ds_profile_db_supported_tech_mask ds_profile_db_get_supported_tech_mask(
  const ds_profile_db_profile_managed_type *profile
)
{
  return ((Profile*)profile)->GetSupportedTechMask();
}
                               
ds_profile_db_result_type ds_profile_db_get_changed_idents(
  const ds_profile_db_profile_managed_type* profile,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_ident_type** idents, /* can be NULL */
  uint16* idents_count, /* can be NULL */
  ds_profile_db_ident_type** idents_gen, /* can be NULL */
  uint16* idents_count_gen /* can be NULL */
)
{
  return ((Profile*)profile)->GetChangedIdents(tech_mask, 
                                               idents,
                                               idents_count, 
                                               idents_gen,
                                               idents_count_gen);
}

void ds_profile_db_free_idents(ds_profile_db_ident_type* idents)
{
  if (!idents)
  {
    return;
  }

  PS_SYSTEM_HEAP_MEM_FREE (idents);
}


ds_profile_db_result_type ds_profile_db_init_creation_params(
  ds_profile_db_creation_params_type* params
)
{
  return DBMgr::InitCreationParams(params);
}

ds_profile_db_result_type ds_profile_db_create(
  const ds_profile_db_creation_params_type* params,
  ds_profile_db_profile_managed_type* profile
)
{
  return DBMgr::Instance()->CreateProfile(params, (Profile*)profile);
}

ds_profile_db_result_type ds_profile_db_delete(
  ds_profile_db_profile_num_type  num,
  ds_profile_db_supported_tech_mask tech_mask,
  boolean enable_deleting_default_profile,
  ds_profile_db_subs_type subs
)
{
  return DBMgr::Instance()->DeleteProfile(num, tech_mask, subs, enable_deleting_default_profile);
}

ds_profile_db_result_type ds_profile_db_write(
  const ds_profile_db_profile_managed_type* profile
)
{
  return DBMgr::Instance()->SetProfile((Profile*)profile);
}

ds_profile_db_result_type ds_profile_db_read(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  ds_profile_db_profile_managed_type** profile)
{
  return DBMgr::Instance()->GetProfile(num, tech_mask, subs, (Profile **)profile);
}

ds_profile_db_result_type ds_profile_db_set_default_profile_num(
  ds_profile_db_profile_num_type num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family)
{
  return DBMgr::Instance()->SetDefaultProfileNum(num, tech, family);
}

ds_profile_db_result_type ds_profile_db_get_default_profile_num(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family)
{
  return DBMgr::Instance()->GetDefaultProfileNum(num, tech, family);
}


ds_profile_db_result_type ds_profile_db_get_default_profile_num_per_subs(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs
)
{
  return DBMgr::Instance()->GetDefaultProfileNum(num, tech, family, subs);
}

ds_profile_db_result_type ds_profile_db_set_default_profile_num_per_subs(
  ds_profile_db_profile_num_type num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs
)
{
  return DBMgr::Instance()->SetDefaultProfileNum(num, tech, family, subs);
}              
ds_profile_db_result_type ds_profile_db_reset(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs)
{
  return DBMgr::Instance()->ResetProfile(num, tech_mask, subs);
}

ds_profile_db_result_type ds_profile_db_reset_profile_param(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_ident_type       ident,
  ds_profile_db_subs_type        subs,
  boolean mark_invalid)
{
  return DBMgr::Instance()->ResetProfileParam(num, tech_mask, ident, subs, mark_invalid);
}
                  

ds_profile_db_result_type ds_profile_db_get_profiles_range(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_profile_num_type *min_num,
  ds_profile_db_profile_num_type *max_num)
{
  return DBMgr::Instance()->GetProfilesRange(tech_mask, min_num, max_num);
}

ds_profile_db_result_type ds_profile_db_get_profiles(
  ds_profile_db_supported_tech_mask tech_mask,                  
  ds_profile_db_subs_type subs,
  ds_profile_db_profile_managed_type ***profiles,
  uint16* profiles_count)
{
  return DBMgr::Instance()->GetProfiles(tech_mask, 
                                        subs,
                                        (Profile***)profiles,
                                        profiles_count);
}

ds_profile_db_result_type ds_profile_db_get_profiles_with_ident(
  ds_profile_db_supported_tech_mask tech_mask, 
  ds_profile_db_ident_type ident,
  const ds_profile_db_info_type* param_info,
  ds_profile_db_subs_type subs,
  ds_profile_db_profile_managed_type ***profiles,
  uint16* profiles_count)
{
  return DBMgr::Instance()->GetProfiles(tech_mask, 
                                        ident, 
                                        param_info,                  
                                        subs,
                                        (Profile***)profiles, 
                                        profiles_count);
}

ds_profile_db_result_type ds_profile_db_free_profiles(
  ds_profile_db_profile_managed_type** profiles,
  uint16 profiles_count)
{
  ds_profile_db_result_type res;

  if (!profiles)
  {
    return DB_RESULT_SUCCESS;
  }

  for (unsigned int i = 0; i < profiles_count; i++)
  {
    if (profiles[i])
    {
      if (DB_RESULT_SUCCESS != (res = ds_profile_db_dealloc(profiles[i])))
      {
        return res;
      }      
    }
  }

  PS_SYSTEM_HEAP_MEM_FREE (profiles);

  return DB_RESULT_SUCCESS;
}


ds_profile_db_callback_id_type ds_profile_db_register_for_notifications(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,                  
  ds_profile_db_cb_type cback,
  ds_profile_db_subs_type subs,
  void *user_data)
{
  return DBMgr::Instance()->RegisterForNotifications(num, tech_mask, cback, subs, user_data);
}

ds_profile_db_result_type ds_profile_db_deregister_for_notifications(
  ds_profile_db_callback_id_type callback_id,
  ds_profile_db_subs_type        subs
)
{
  return DBMgr::Instance()->DeregisterForNotifications(callback_id, subs);
}

ds_profile_db_callback_id_type ds_profile_db_register_for_notifications_MH(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,    
  boolean treat_tech_mask_as_OR,
  ds_profile_db_cb_type cback,
  void *user_data
)
{
  ds_profile_db_subs_type subs = (ds_profile_db_subs_type)ps_sys_get_default_data_subscription();
  return DBMgr::Instance()->RegisterForNotifications(num, 
                                                     tech_mask,                                                     
                                                     cback, 
                                                     subs,
                                                     user_data,
                                                     true,
                                                     (treat_tech_mask_as_OR != 0));
}

ds_profile_db_result_type ds_profile_db_register_function_table_MH(
  tech_func_table_type *tech_table,
  ds_profile_db_supported_tech_mask tech_mask
)
{
  ds_profile_db_tech_type tech_type;
  tech_type = ds_profile_db_tech_mask_to_tech(tech_mask);
  if (Profile::ValidateTech(tech_type) != DB_RESULT_SUCCESS)
  {
    return DB_RESULT_FAIL;
  }
  if (tech_func_table[tech_type].is_registered != true)
  {
    tech_func_table[tech_type] = *tech_table;
    tech_func_table[tech_type].is_registered = true;
    return DB_RESULT_SUCCESS;
  }
  else
  {
    PROFILE_DB_LOG_ERROR_1("Tech Mask %d has already registered %d!!", tech_mask);
    return DB_RESULT_FAIL;
  }
}

tech_func_table_type* ds_profile_db_get_tech_tbl(
  ds_profile_db_tech_type tech
)
{
  if (DB_RESULT_SUCCESS != Profile::ValidateTech(tech))
  {
    return NULL;
  }
  if(tech_func_table[tech].is_registered)
  {
    return &tech_func_table[tech];
  }
  else
  {
    return NULL;
  }
}

void ds_profile_db_init_power_up_finished(void)
{
  DBMgr::Instance()->PowerUpFinished();
}
ds_profile_db_tech_type ds_profile_db_tech_mask_to_tech(
   uint16 tech_mask
)
{
  int tech = 0xFF;
  if (tech_mask == 0x01)
  {
    tech = DB_TECH_3GPP;
  } else if (tech_mask == 0x02)
  {
    tech = DB_TECH_3GPP2;
  } else if (tech_mask == 0x07)
  {
    tech = DB_TECH_COMMON;
  }
  return (ds_profile_db_tech_type)(tech);
}

ds_profile_db_tech_type ds_profile_db_get_tech_type(
  ds_profile_db_profile_num_type num,
  ds_profile_db_subs_type subs
)
{
  return DBMgr::Instance()->GetTechType(num, subs);
}

ds_profile_db_result_type ds_profile_db_get_param_in_use( 
  ds_profile_db_profile_managed_type *profile,
  ds_profile_db_ident_type            ident,
  ds_profile_db_info_type             *info,
  ds_profile_db_tech_type             tech,
  ds_profile_db_subs_type             subs
)
{
  if (ident < COMMON_TECH_PROFILE_PARAM_PDP_TYPE || ident > COMMON_TECH_PARAM_MAX)
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  return (DBMgr::Instance()->GetParamsInUse((Profile *)profile, ident, info, tech, subs));
  
}

ds_profile_db_ident_type ds_profile_db_get_tech_ident_from_common_ident (ds_profile_db_ident_type common_ident,
																		 ds_profile_db_tech_type  tech)
{
  ds_profile_db_ident_type ident = INVALID_PROFILE_PARAM;
  if (common_ident <= COMMON_TECH_PARAM_MAX && common_ident >= COMMON_TECH_PARAM_MIN)
  {
    if (tech == DB_TECH_3GPP)
    {
      ident = ds_profile_db_common_to_tech_params_tbl[COMMON_TECH_PARAM_MAX - common_ident].ds_profile_3gpp_param;
    } else
    {
      ident = ds_profile_db_common_to_tech_params_tbl[COMMON_TECH_PARAM_MAX - common_ident].ds_profile_3gpp2_param;
    }
  }
  return ident;
}

ds_profile_db_result_type ds_profile_db_get_persistence(
  ds_profile_db_profile_num_type profile_num,
  ds_profile_db_subs_type subs,
  boolean *is_persistent)
{
  return DBMgr::Instance()->GetPersistence(profile_num, subs, is_persistent);
}


#ifdef FEATURE_USER_CONFIGURE_EFS

void* ds_profile_db_get_heap_mem_for_file(void)
{
  return heapMemoryForFile;
}

void ds_profile_db_set_heap_mem_for_file(void *buf)
{
  heapMemoryForFile = buf;
}

void ds_profile_add_entry_to_delete_profile_list 
(
  ds_profile_db_deleted_profile_entry *deleted_profile
)
{
  static ds_profile_db_deleted_profile_entry *temp_delete_profile = NULL;
  static ds_profile_db_deleted_profile_entry *temp_next_profile = NULL;
  static uint32 deleted_profiles = 0;
  if (!delete_profile_list_head_ptr)
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(delete_profile_list_head_ptr, 
                             sizeof(ds_profile_db_deleted_profile_entry),
                             ds_profile_db_deleted_profile_entry*);
    if (delete_profile_list_head_ptr)
    {
      memscpy(delete_profile_list_head_ptr, sizeof(ds_profile_db_deleted_profile_entry),
              deleted_profile, sizeof(ds_profile_db_deleted_profile_entry));
      delete_profile_list_head_ptr->next = NULL;
      profile_delete_count--;
    }
  } else
  {
    if (profile_delete_count == 0)
    {
      temp_next_profile = temp_delete_profile->next;
      memscpy(temp_delete_profile, sizeof(ds_profile_db_deleted_profile_entry),
              deleted_profile, sizeof(ds_profile_db_deleted_profile_entry));
      temp_delete_profile->next = temp_next_profile;
	  temp_delete_profile = temp_delete_profile->next;
	  deleted_profiles++;
	   if (deleted_profiles == 15)
      {
        temp_delete_profile = delete_profile_list_head_ptr;
		deleted_profiles = 0;
      }
    } else
    {
      temp_delete_profile = delete_profile_list_head_ptr;
      while(temp_delete_profile->next != NULL && profile_delete_count)
      {
        temp_delete_profile = temp_delete_profile->next;
      }

      PS_SYSTEM_HEAP_MEM_ALLOC(temp_delete_profile->next, 
                               sizeof(ds_profile_db_deleted_profile_entry),
                               ds_profile_db_deleted_profile_entry*);
      if (temp_delete_profile->next)
      {
        memscpy(temp_delete_profile->next, sizeof(ds_profile_db_deleted_profile_entry),
                deleted_profile, sizeof(ds_profile_db_deleted_profile_entry));
        temp_delete_profile->next->next = NULL;
        profile_delete_count--;
        if (delete_profile_list_head_ptr->next == NULL)
        {
          delete_profile_list_head_ptr->next = temp_delete_profile;
        }
        if (profile_delete_count == 0)
        {
          temp_delete_profile = delete_profile_list_head_ptr;
        }
      }
    }
  }
}

ds_profile_db_result_type ds_profile_db_is_valid_profile(
  ds_profile_db_profile_num_type  num_type, 
  ds_profile_db_subs_type         db_subs,
  ds_profile_db_tech_type         tech_type,
  boolean                         *is_valid
)
{
  return DBMgr::Instance()->IsValidProfile(num_type, db_subs, tech_type, is_valid);
}

boolean ds_profile_db_get_epc_support_val
(
  void
)
{
  return DBMgr::Instance()->GetEPCSupportVal();
}

mcfg_fs_sub_id_e_type ds_profile_db_convert_ds_to_mcfg_subs(ds_profile_db_subs_type subs)
{
  mcfg_fs_sub_id_e_type mcfg_subs_id = MCFG_FS_SUBID_NONE;
  switch (subs)
  {
    case DB_PROFILE_ACTIVE_SUBSCRIPTION_1:
      mcfg_subs_id = MCFG_FS_SUBID_0;
      break;
    case DB_PROFILE_ACTIVE_SUBSCRIPTION_2:
      mcfg_subs_id = MCFG_FS_SUBID_1;
      break;
    case DB_PROFILE_ACTIVE_SUBSCRIPTION_3: 
      mcfg_subs_id = MCFG_FS_SUBID_2;
      break;
    default:
      break;
  }
  PROFILE_DB_LOG_MED_2("MCFG subs %d, DS subs %d", mcfg_subs_id, subs);
  return mcfg_subs_id;
}

#endif /*FEATURE_USER_CONFIGURE_EFS*/
