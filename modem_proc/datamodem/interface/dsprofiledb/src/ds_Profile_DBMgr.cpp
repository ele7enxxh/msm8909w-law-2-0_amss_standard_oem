/*===========================================================================
  FILE: ds_Profile_DBMgr.cpp

  OVERVIEW: This file provides implementation of the DBMgr class.

  DEPENDENCIES: None

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

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_profile_db.h"
#include "ds_Profile_DBMgr.h"
#include "ds_Profile_Profile.h"
#include "ds_Profile_TechMgr.h"
#include "ds_Profile_File.h"
#include "ds_Profile_FileEFS.h"
#include "ds_Profile_TechMgr3GPP.h"
#include "ds_Profile_TechMgr3GPP2.h"
#include "ds_Profile_Platform.h"
#include "ds_Profile_Log.h"
#include "ps_system_heap.h"
#include "ds_Profile_NVMgr.h"

extern "C"
{
#include "ds3geventmgr.h"
#include "ds_profile_3gpp2_mgr.h"
#include "ds3gsubsmgr.h"
#include "ds_profile_db_internal.h"
}

#include <stringl/stringl.h>

using namespace ds::Profile;

DBMgr *DBMgr::theInstance = NULL;

/*===========================================================================

                     GLOBAL DECLARATIONS

===========================================================================*/


/*===========================================================================

                     Internal FUNCTIONS

===========================================================================*/
static ds_profile_db_result_type ValidateCreationParams(const ds_profile_db_creation_params_type *params)
{
  ds_profile_db_result_type res;
  if (DB_UNSPECIFIED_PROFILE_NUM != params->profile_num)
  {
    if (DB_RESULT_SUCCESS != (res = Profile::ValidateProfileNum(params->profile_num)))
    {
      return res;
    }
  }

  return Profile::ValidateSubscriptionId(params->subscription_id);
}

static inline bool WithinNumberPoolRange(ds_profile_db_profile_num_type num)
{
  return ((num >= TechMgr::ALLOC_RANGE_MIN) && (num < TechMgr::ALLOC_RANGE_MAX));
}
/*===========================================================================

                     MEMBER FUNCTIONS

===========================================================================*/
DBMgr* DBMgr::Instance(void)
{
  return theInstance;
}

DBMgr::DBMgr(void) : mutex(NULL), callbackMgr(NULL),
  profiles_for_power_up(NULL), profiles_count_for_power_up(0)
{
  memset(techMgrs, 0, sizeof(techMgrs));
  memset(numbersPool, false, sizeof(numbersPool));
}

static void _internal_deinit(void)
{
  DBMgr::Deinit();
}

ds_profile_db_result_type DBMgr::Init(void)
{
  PROFILE_DB_LOG_HIGH("Init(): started");
  ds_profile_db_result_type res, status;
  boolean is_sfs = false;
  NVMgr *nvMgr  = NULL;
  uint8 subs_id = 0;
  ds3gsubsmgr_client_type  ds_profile_db_subs_client_id;
  ds3geventmgr_filter_type        filter_info;
#ifdef FEATURE_USER_CONFIGURE_EFS
  void *heapForFile = NULL;
#endif
  if (theInstance)
  {
    return DB_RESULT_ERR_INVAL;
  }

  if (DB_RESULT_SUCCESS != (res = Platform::Instance()->Init()))
  {
    return res;
  }

  /*Alloc NV MGR*/
  nvMgr = new NVMgr();
  if (!nvMgr)
  {
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  NVMgr::InitEFSConfFile();

  //Read NV here and update DBMgr
  if (DB_RESULT_SUCCESS == (status = nvMgr->ReadFromNV(DS_PROFILE_DB_ENABLE_SFS,
                                                       &is_sfs,
                                                       sizeof(is_sfs),
                                                       DB_PROFILE_ACTIVE_SUBSCRIPTION_1)))
  {
    PROFILE_DB_LOG_ERROR_1("Read from NV failed for SFS with the status %d", status);
  }
  Platform::Instance()->SetIsSFS(is_sfs, DB_PROFILE_ACTIVE_SUBSCRIPTION_1);
  if (DB_RESULT_SUCCESS == (status = nvMgr->ReadFromNV(DS_PROFILE_DB_ENABLE_SFS,
                                                       &is_sfs,
                                                       sizeof(is_sfs),
                                                       DB_PROFILE_ACTIVE_SUBSCRIPTION_2)))
  {
    PROFILE_DB_LOG_ERROR_1("Read from NV failed for SFS with the status %d", status);
  }
  //Set no matter what
  Platform::Instance()->SetIsSFS(is_sfs, DB_PROFILE_ACTIVE_SUBSCRIPTION_2);
#ifdef FEATURE_USER_CONFIGURE_EFS
  PS_SYSTEM_HEAP_MEM_ALLOC(heapForFile, MAX_TLV_SIZE, void*);

  if (heapForFile == NULL)
  {
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  ds_profile_db_set_heap_mem_for_file(heapForFile);
#endif
  theInstance = new DBMgr();
  if (!theInstance)
  {
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  theInstance->callbackMgr = Platform::Instance()->GetCallbackMgrSingleton();
  if (!theInstance->callbackMgr)
  {
    res = DB_RESULT_FAIL;
    goto bail;
  }

  if (!(theInstance->mutex = Platform::Instance()->GetMutex()))
  {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }

  theInstance->techMgrs[DB_TECH_3GPP] = new TechMgr3GPP();
  if (!theInstance->techMgrs[DB_TECH_3GPP])
  {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }

  theInstance->techMgrs[DB_TECH_3GPP]->SetTechTable(ds_profile_db_get_tech_tbl(DB_TECH_3GPP));
  theInstance->techMgrs[DB_TECH_3GPP2] = new TechMgr3GPP2();
  if (!theInstance->techMgrs[DB_TECH_3GPP2])
  {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }
  theInstance->techMgrs[DB_TECH_3GPP2]->SetTechTable(ds_profile_db_get_tech_tbl(DB_TECH_3GPP2));
  Profile::SetValidationFunction();

 /*Read the efs file and store in the member variable in NV-MGR*/
  (void)nvMgr->ReadEFSFile(DB_EFS_FILE_TYPE_EPC_SUPPORT);

  /*Set the value of EPC support variable, irrespectively */
  theInstance->SetEPCSupportVal(nvMgr->GetEPCSupportVal());

  if (DB_RESULT_SUCCESS != (res = PowerUpSequence()))
  {
    goto bail;
  }

  /* should not be enabled, as it could try to use platform resources which were already
     released by other cleanup procedures */
  //(void)atexit(_internal_deinit);
  memset(&filter_info, 0, sizeof(ds3geventmgr_filter_type));
  filter_info.tech = PS_SYS_TECH_ALL;

  for(subs_id = 0; subs_id < DS3GSUBSMGR_SUBS_ID_COUNT; subs_id++)
  {
    filter_info.ps_subs_id = (ps_sys_subscription_enum_type)
       ds3gsubsmgr_subs_id_cm_to_ds((sys_modem_as_id_e_type)subs_id);
    if (ds3geventmgr_event_reg(DS3GEVENTMGR_NV_REFRESH_EV,
                               DS3GEVENTMGR_CLIENT_ID_PROFILE,
                               &filter_info,
                               ds_profile_db_ds3g_cmd_hdlr,
                               NULL ) == FALSE)
    {
      PROFILE_DB_LOG_ERROR("Registration for CARD REFRESH EV failed !");
      res = DB_RESULT_FAIL;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Register for subsmgr events
  -------------------------------------------------------------------------*/
  if (ds3gsubsmgr_event_reg(DS3GSUBSMGR_DEVICE_MODE_EV,
                            ds_profile_db_ds3g_subsmgr_event_cb,
                            NULL,
                            &ds_profile_db_subs_client_id) == FALSE)
  {
    PROFILE_DB_LOG_ERROR("Device mode ev registration failed.");
    res = DB_RESULT_FAIL;
    goto bail;
  }


  PROFILE_DB_LOG_HIGH("Init(): complete");

  return DB_RESULT_SUCCESS;

bail:

  delete theInstance;
  theInstance = NULL;

  Platform::Instance()->Deinit();

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}

ds_profile_db_result_type DBMgr::PowerUpSequence(void)
{
  Profile **profiles = NULL;
  unsigned int profiles_count = DS_PROFILE_DB_MAX_PROFILE_COUNT;
  ds_profile_db_result_type res;
  unsigned int created_on_the_fly = 0;
  ds_profile_db_subs_type subs = (ds_profile_db_subs_type)ps_sys_get_default_data_subscription();

  PS_SYSTEM_HEAP_MEM_ALLOC(profiles, profiles_count * sizeof(Profile*),
                           Profile**);
  if (NULL == profiles) {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }
  memset(profiles, 0, profiles_count * sizeof(Profile *));

  /* mark in cache special profiles (KDDI / OMH / etc) as if they exist, but do not create them */
  if (DB_RESULT_SUCCESS != (res = theInstance->MarkSpecialProfilesInCache(subs)))
  {
    goto bail;
  }

  /* read all profiles from EFS */
  if (DB_RESULT_SUCCESS != (res = theInstance->FromFileToCache(profiles, &profiles_count, subs)))
  {
    goto bail;
  }

  /* create some special profiles on the fly */
  created_on_the_fly = DS_PROFILE_DB_MAX_PROFILE_COUNT - profiles_count;
  if (DB_RESULT_SUCCESS != (res = theInstance->OnTheFlyToCache(profiles + profiles_count, &created_on_the_fly, subs)))
  {
    goto bail;
  }
  profiles_count += created_on_the_fly;

  theInstance->profiles_for_power_up = profiles;
  theInstance->profiles_count_for_power_up = profiles_count;

  PROFILE_DB_LOG_HIGH_2("PowerUpSequence(): Finished with success, read %d profiles from EFS, created in cache %d profiles on the fly",
                        profiles_count - created_on_the_fly, created_on_the_fly);

  return DB_RESULT_SUCCESS;

bail:

  /* in case of error, also clean cache and release numbers */
  theInstance->CleanupProfiles(DS_PROFILE_DB_MAX_PROFILE_COUNT, profiles, true, true);
  theInstance->profiles_for_power_up = NULL;
  theInstance->profiles_count_for_power_up = 0;

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}

ds_profile_db_result_type DBMgr::MarkSpecialProfilesInCache(ds_profile_db_subs_type subs)
{

  ds_profile_db_profile_num_type kddi_min_num = 0, kddi_max_num = 0;
  ds_profile_db_profile_num_type omh_min_num = 0, omh_max_num = 0;
  ds_profile_db_result_type res;
  tech_func_table_type* tech_table_3gpp2 = ds_profile_db_get_tech_tbl(DB_TECH_3GPP2);


  /* KDDI */
  /* on error, ignore */

  if (tech_table_3gpp2)
  {
    if (tech_table_3gpp2->get_profiles_range)
    {
      res = tech_table_3gpp2->get_profiles_range(DB_3GPP2_PROFILE_KDDI,
                                                 &kddi_min_num,
                                                 &kddi_max_num);


      if (DB_RESULT_SUCCESS == res)
      {
        if (DB_RESULT_SUCCESS != (res = MarkRangeInCacheDuringInit(DB_TECH_TO_TECHMASK(DB_TECH_3GPP2),
                                                                   kddi_min_num, kddi_max_num, false, subs)))
        {
          goto bail;
        }
      }


      /* OMH */
      /* on error, ignore */
      res = tech_table_3gpp2->get_profiles_range(DB_3GPP2_PROFILE_OMH,
                                                 &omh_min_num,
                                                 &omh_max_num);

      if (DB_RESULT_SUCCESS == res)
      {
        if (DB_RESULT_SUCCESS != (res = MarkRangeInCacheDuringInit(DB_TECH_TO_TECHMASK(DB_TECH_3GPP2),
                                                                   omh_min_num, omh_max_num, false, subs)))
        {
          goto bail;
        }
      }
      return DB_RESULT_SUCCESS;

    bail:

      ReleaseRangeInCache(kddi_min_num, kddi_max_num, DB_TECH_TO_TECHMASK(DB_TECH_3GPP2), false, subs);
      ReleaseRangeInCache(omh_min_num, omh_max_num, DB_TECH_TO_TECHMASK(DB_TECH_3GPP2), false, subs);

      return res;
    }
    return DB_RESULT_FAIL;
  }
  else
  {
    return DB_RESULT_SUCCESS;
  }
}

void DBMgr::PowerUpFinished(void)
{
  PROFILE_DB_LOG_HIGH("PowerUpFinished(): started");

  if (callbackMgr->WithinCallback())
  {
    return;
  }

  if (0 != mutex->Lock())
  {
    return;
  }
  AutoReleseMutex a_m(mutex);

  if (profiles_for_power_up)
  {
    CleanupProfiles(profiles_count_for_power_up, profiles_for_power_up);
  }

  profiles_for_power_up = NULL;
  profiles_count_for_power_up = 0;
}

void DBMgr::Deinit(void)
{
  theInstance->PowerUpFinished();

  Platform::Instance()->Deinit();

  delete theInstance;
  theInstance = NULL;
}

DBMgr::~DBMgr(void)
{
  /* release tech managers */
  TechMgr *techMgr = NULL;
  for (int tech = 0; tech < DB_TECH_MAX; tech++)
  {
    if ((techMgr = (GetTechManager((ds_profile_db_tech_type)tech))))
    {
      delete techMgr;
      techMgr = NULL;
    }
  }
  _internal_deinit();
  delete mutex;
  mutex = NULL;
}

ds_profile_db_result_type DBMgr::CreateProfile(
  const ds_profile_db_creation_params_type *params,
  Profile *profile)
{
  bool new_profile = false;
  bool num_specified = false;
  boolean enable_deleting_default_profile = true;
  if (!profile || !params)
  {
    PROFILE_DB_LOG_ERROR_2("CreateProfile(): invalid creation params 0x%p or profile 0x%p", params, profile);
    return DB_RESULT_ERR_INVAL_OP;
  }

  PROFILE_DB_LOG_MED_3("CreateProfile(): profile num %d, persistency set to %d, subs id %d",
                       params->profile_num,
                       params->persistent,
                       params->subscription_id);

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);


  ds_profile_db_result_type res = ValidateCreationParams(params);
  if (DB_RESULT_SUCCESS != res)
  {
    PROFILE_DB_LOG_ERROR("CreateProfile(): invalid creation params");
    return res;
  }

  res = Profile::ValidateSupportedTechMask(profile->GetSupportedTechMask());
  {
    if (DB_RESULT_SUCCESS != res)
    {
      return res;
    }
  }

  /* profile should have an unspecified number*/
  if (DB_UNSPECIFIED_PROFILE_NUM != profile->GetNum())
  {
    PROFILE_DB_LOG_ERROR("CreateProfile(): trying to create a profile for the second time with the same local cached copy")
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  File *file = NULL;

  ds_profile_db_profile_num_type new_num = params->profile_num;

  /* set all the creation params  */
  //  to avoid warning C4800: conversion between boolean to bool
  profile->SetPersistency((params->persistent) != 0);


  if (DB_RESULT_SUCCESS != (res = profile->SetSubscriptionId(params->subscription_id, false)))
  {
    goto bail;
  }

  if (DB_UNSPECIFIED_PROFILE_NUM != new_num)
  {
    profile->SetNumber(new_num);
    num_specified = true;
  }

  /* the checks whether the profile can be created for a specific number are done inside */
  if (DB_RESULT_SUCCESS != (res = GetNumForProfile(profile, &new_num, params->subscription_id, false)))
  {
    goto bail;
  }

  /* This is to deal with special case where we creating over the existing profile 
     (required by ATCOP */
  new_profile = true;

  if (DB_UNSPECIFIED_PROFILE_NUM == profile->GetNum())
  {
    profile->SetNumber(new_num);
    res = cacheMgr.CreateCache(profile);
  } else
  {
    /* currently we overwrite the existing profile when the profile num is specified during creation */
    res = cacheMgr.UpdateOrCreateCache(profile, true, &new_profile);
  }

  if (DB_RESULT_SUCCESS != res)
  {
    goto bail;
  }

  if (!ShouldBeTreatedAsNonPersistent(profile))
  {
    file = Platform::Instance()->GetFile(profile->GetNum(),
                                         profile->GetSubcriptionId(),
                                         File::F_CREATE_OVERWRITE);
    if (!file)
    {
      res = DB_RESULT_ERR_FILE_ACCESS;
      goto bail;
    }
#ifdef FEATURE_USER_CONFIGURE_EFS
    if (DB_RESULT_SUCCESS != (res = file->Write(profile, false, false, true)))
    {
#else
    if (DB_RESULT_SUCCESS != (res = file->Write(profile)))
    {
#endif
      goto bail;
    }
  }

  if (DB_RESULT_SUCCESS != (res = NotifyClients(profile, DB_CREATE_PROFILE_EVENT, profile->GetSubcriptionId())))
  {
    goto bail;
  }

  if (file)
  {
    delete file;
    file = NULL;
  }

  PROFILE_DB_LOG_MED_1("CreateProfile(): success, profile num %d",
                       params->profile_num);

  return DB_RESULT_SUCCESS;

bail:

  profile->SetNumber(DB_UNSPECIFIED_PROFILE_NUM);

  if (res != DB_RESULT_ERR_INVAL_PROFILE_NUM && num_specified != true)
  {
    if (DB_UNSPECIFIED_PROFILE_NUM != new_num)
    {
      if (file)
      {
        file->Delete(new_num, params->subscription_id);
        delete file;
      }
    }

    cacheMgr.RemoveCache(new_num, profile->GetSupportedTechMask(), profile->GetSubcriptionId());
    if (res != DB_RESULT_ERR_OUT_OF_PROFILES)
    {
      ReleaseNumForProfile(!ShouldBeTreatedAsNonPersistent(profile),
                           new_num,
                           profile->GetSupportedTechMask(),
                           profile->GetSubcriptionId(),
                           enable_deleting_default_profile); /* 0 - means we don't check permissions for deletion */
    }
  }

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}

ds_profile_db_result_type DBMgr::DeleteProfile(
  ds_profile_db_profile_num_type  profile_num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  boolean enable_deleting_default_profile /* = false */)
{
  bool is_persistent = false;
  char apn_name[DS_PROFILE_DB_MAX_APN_NAME_LEN + 1];
  ds_profile_db_info_type info_type = { 0 };
  ds_profile_db_deleted_profile_entry deleted_profile = { 0 };
  PROFILE_DB_LOG_MED_2("DeleteProfile(): profile_num %d , tech_mask %d", profile_num, tech_mask);

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  File *file = NULL;

  if (DB_RESULT_SUCCESS != Profile::ValidateSupportedTechMask(tech_mask))
  {
    return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
  }

  /* do a quick before you go to heavy operations of constructing a full profile from cache */
  CacheMgr::cache_entry *c_e = cacheMgr.GetCache(profile_num, tech_mask, subs);
  if (!c_e)
  {
    PROFILE_DB_LOG_ERROR_2("DeleteProfile(): trying to delete non existing profile %d with tech_mask %d",
                           profile_num, tech_mask);
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  if (!CanDeleteProfile(profile_num, c_e->tech_mask, enable_deleting_default_profile))
  {
    PROFILE_DB_LOG_ERROR_2("DeleteProfile(): deleting of profile %d with mask %d is not allowed",
                           profile_num, c_e->tech_mask);
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  Profile *profile;
  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = cacheMgr.GetCache(profile_num, tech_mask, subs, &profile)))
  {
    PROFILE_DB_LOG_ERROR_1("DeleteProfile(): could not get cache, error %d", res);
    return res;
  }
  if (profile)
  {

    if (DB_RESULT_SUCCESS != (res = ReleaseNumForProfile(!ShouldBeTreatedAsNonPersistent(profile),
                                                         profile_num,
                                                         profile->GetSupportedTechMask(),
                                                         subs,
                                                         enable_deleting_default_profile)))
    {
      goto bail;
    }
  } else
  {
    PROFILE_DB_LOG_ERROR("DeleteProfile(): Profile returned was NULL from GetCache");
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }

  /* should succeed as we checked cache validity previously */
  cacheMgr.RemoveCache(profile_num, tech_mask, subs);

  if (DB_RESULT_SUCCESS != (res = NotifyClients(profile, DB_DELETE_PROFILE_EVENT, subs)))
  {
    PROFILE_DB_LOG_ERROR_1("DeleteProfile(): could not notify clients, error %d", res);
  }

  is_persistent = !ShouldBeTreatedAsNonPersistent(profile);

  if (is_persistent)
  {
    file = Platform::Instance()->GetFile(subs);
    if (!file)
    {
      res = DB_RESULT_ERR_FILE_ACCESS;
      goto bail;
    }
    if (DB_RESULT_SUCCESS != (res = file->Delete(profile_num, subs)))
    {
      goto bail;
    }
  }
  deleted_profile.num = profile_num;
  deleted_profile.tech_mask = profile->GetSupportedTechMask();
  info_type.buf = apn_name;
  info_type.len = DS_PROFILE_DB_MAX_APN_NAME_LEN + 1;
  if (DB_RESULT_SUCCESS != profile->GetParam(GENERAL_PROFILE_PARAM_APN_NAME,
                                             &info_type))
  {
    goto bail;
  }
  memscpy(deleted_profile.apn_name, DS_PROFILE_DB_MAX_APN_NAME_LEN + 1, info_type.buf, info_type.len);

  ds_profile_add_entry_to_delete_profile_list(&deleted_profile);

  res = DB_RESULT_SUCCESS;
  /* fall through */

bail:

  if (file)
  {
    delete file;
    file = NULL;
  }

  if (profile)
  {
    delete profile;
    profile = NULL;
  }

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}
#ifdef FEATURE_USER_CONFIGURE_EFS
ds_profile_db_result_type DBMgr::SetProfile(
  Profile * profile,
  bool delete_on_file /*= false*/)
{
  ds_profile_db_result_type res = DB_RESULT_FAIL;

  if (!profile)
  {
    PROFILE_DB_LOG_ERROR("SetProfile(): NULL profile param");
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (DB_RESULT_SUCCESS != (res = Profile::ValidateSubscriptionId(profile->GetSubcriptionId())))
  {
    return res;
  }

  PROFILE_DB_LOG_MED_3("SetProfile(): profile number %d, tech_mask %d, subs %d",
                       profile->GetNum(), profile->GetSupportedTechMask(), profile->GetSubcriptionId());

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  /* profile should not have an unspecified number*/
  if (DB_UNSPECIFIED_PROFILE_NUM == profile->GetNum())
  {
    PROFILE_DB_LOG_ERROR("SetProfile(): you are trying to save a fresh locally created cached profile");
    PROFILE_DB_LOG_ERROR("SetProfile(): you should use the one obtained by GetProfile() or by CreateProfile()");
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  if (!CanWriteProfile(profile->GetNum(), profile->GetSupportedTechMask()))
  {
    PROFILE_DB_LOG_ERROR_2("SetProfile(): modifying a profile %d with mask %d, is not allowed",
                           profile->GetNum(), profile->GetSupportedTechMask());
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  return SetProfileInternal(profile, false, delete_on_file);
}

ds_profile_db_result_type DBMgr::SetProfileInternal(
  Profile * profile,
  bool send_reset_event, /* = false */
  bool delete_from_file  /*= false*/)
{
  File *file = NULL;
  File *file_3gpp2 = NULL;
  ds_profile_db_result_type res;
  bool create_3gpp2 = false;
  File::FlagType      flag = File::F_READ;

  if (!cacheMgr.GetCache(profile->GetNum(), profile->GetSupportedTechMask(), profile->GetSubcriptionId()))
  {
    PROFILE_DB_LOG_ERROR("SetProfile(): trying to modify a non-existing profile");
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  if (DB_RESULT_SUCCESS != (res = cacheMgr.UpdateCache(profile)))
  {
    PROFILE_DB_LOG_ERROR("SetProfile(): could not modify cache, even though it exists");
    goto bail;
  }

  if (DB_RESULT_SUCCESS != 
       (res = 
          Profile::ValidateSupportedTechMask(profile->GetSupportedTechMask())))
  {
    return res;
  }

  if (DB_RESULT_SUCCESS != (res = NotifyClients(profile,
                                                send_reset_event ?
                                                DB_RESET_PROFILE_EVENT :
                                                DB_MODIFY_PROFILE_EVENT,
                                                profile->GetSubcriptionId())))
  {
    PROFILE_DB_LOG_ERROR_1("SetProfile(): could not notify clients, error %d", res);
  }

  if (!ShouldBeTreatedAsNonPersistent(profile))
  {
    if (AllowToCreateFileOnFirstModify(profile))
    {
      file_3gpp2 = Platform::Instance()->GetFile(profile->GetNum(), profile->GetSubcriptionId(), flag);
      if (!file_3gpp2)
      {
        flag = File::F_WRITE_CREATE;
        create_3gpp2 = true;
        PROFILE_DB_LOG_ERROR("SetProfileInternal() Create 3GPP2 Default Profile!");
      }
      /*local variable file2 to check if 3GPP2 default profile exists already or not*/
      else
      {
        PROFILE_DB_LOG_ERROR("SetProfileInternal() Modify 3GPP2 Default Profile!");
        file_3gpp2->Close();
        delete file_3gpp2;
        file_3gpp2 = NULL;
      }
    }
    file = Platform::Instance()->GetFile(profile->GetNum(), profile->GetSubcriptionId(), flag);
    if (!file)
    {
      res = DB_RESULT_ERR_FILE_ACCESS;
      goto bail;
    }
    if (flag == File::F_READ)
    {
      if (DB_RESULT_SUCCESS != (res = file->ReadTLV()))
      {
        goto bail;
      }
      profile->SetReadTime(time_get_secs());
    } 
    else
    {
      PROFILE_DB_LOG_ERROR("SetProfileInternal() Write 3GPP2 Default Profile");
    }
    if (DB_RESULT_SUCCESS != (res = file->Open(profile->GetNum(),File::F_RW, profile->GetSubcriptionId())))
    {
      goto bail;
    }
    if (DB_RESULT_SUCCESS != (res = file->Write(profile, delete_from_file, send_reset_event, false, create_3gpp2)))
    {
      goto bail;
    }
  }

  res = DB_RESULT_SUCCESS;
  /* fall through */

bail:
  if (file)
  {
    delete file;
    file = NULL;
  }
  profile->ResetParamTracking();
  PROFILE_DB_LOG_ERROR_ON_EXIT(res);
  return res;
}

#else
ds_profile_db_result_type DBMgr::SetProfile(
  Profile * profile)
{
  if (!profile)
  {
    PROFILE_DB_LOG_ERROR("SetProfile(): NULL profile param");
    return DB_RESULT_ERR_INVAL_OP;
  }

  PROFILE_DB_LOG_MED_2("SetProfile(): profile number %d, tech_mask %d",
                       profile->GetNum(), profile->GetSupportedTechMask());

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  /* profile should not have an unspecified number*/
  if (DB_UNSPECIFIED_PROFILE_NUM == profile->GetNum())
  {
    PROFILE_DB_LOG_ERROR("SetProfile(): you are trying to save a fresh locally "
                         "created cached profile");
    PROFILE_DB_LOG_ERROR("SetProfile(): you should use the one obtained by "
                         "GetProfile() or by CreateProfile()");
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  if (!CanWriteProfile(profile->GetNum(), profile->GetSupportedTechMask()))
  {
    PROFILE_DB_LOG_ERROR_2("SetProfile(): modifying a profile %d with mask %d,"
                           "is not allowed",
                           profile->GetNum(), profile->GetSupportedTechMask());
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  if (DB_RESULT_SUCCESS != (res = Profile::ValidateSubscriptionId(profile->GetSubcriptionId())))
  {
    return res;
  }

  return SetProfileInternal(profile);
}

ds_profile_db_result_type DBMgr::SetProfileInternal(
  Profile * profile,
  bool send_reset_event /* = false */)
{
  File *file = NULL;
  ds_profile_db_result_type res;

  if (!GetCache(profile->GetNum(), profile->GetSupportedTechMask()))
  {
    PROFILE_DB_LOG_ERROR("SetProfile(): trying to modify a non-existing profile");
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  if (DB_RESULT_SUCCESS != (res = cacheMgr.UpdateCache(profile)))
  {
    PROFILE_DB_LOG_ERROR("SetProfile(): could not modify cache, even though "
                         "it exists");
    goto bail;
  }


  if (DB_RESULT_SUCCESS != (res = NotifyClients(profile,
                                                send_reset_event ?
                                                DB_RESET_PROFILE_EVENT :
                                                DB_MODIFY_PROFILE_EVENT,
                                                subs)))
  {
    PROFILE_DB_LOG_ERROR_1("SetProfile(): could not notify clients, error %d",
                           res);
  }

  if (!ShouldBeTreatedAsNonPersistent(profile))
  {
    file = Platform::Instance()->GetFile(profile->GetNum(),
                                         profile->GetSubcriptionId(),
                                         (AllowToCreateFileOnFirstModify(profile)) ?
                                         File::F_WRITE_CREATE :
                                         File::F_WRITE);
    if (!file)
    {
      res = DB_RESULT_ERR_FILE_ACCESS;
      goto bail;
    }
    if (DB_RESULT_SUCCESS != (res = file->Write(profile)))
    {
      goto bail;
    }
  }

  res = DB_RESULT_SUCCESS;
  /* fall through */

bail:
  if (file)
  {
    delete file;
    file = NULL;
  }

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}
#endif

ds_profile_db_result_type DBMgr::GetProfile(
  ds_profile_db_profile_num_type profile_num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  Profile **profile)
{
  /* TODO, in phase 2, when the whole Profile is fetched from cache, tracking list
     might be cleaned */
  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (!profile)
  {
    PROFILE_DB_LOG_ERROR("GetProfile(): invalid profile output pointer");
    return DB_RESULT_ERR_INVAL_OP;
  }

  PROFILE_DB_LOG_MED_3("GetProfile(): profile number %d, tech_mask %d, subs %d", profile_num, tech_mask, subs);

  ds_profile_db_result_type res;

  //Making change for CR631257 where get param based on EPC
  if (DB_RESULT_SUCCESS != (res = cacheMgr.GetCache(profile_num, tech_mask, subs, profile, false)))
  {
    PROFILE_DB_LOG_ERROR_1("GetProfile(): could not get cache, error %d", res);
    return res;
  }

  if (profile)
  {
    if (*profile)
    {
      PROFILE_DB_LOG_HIGH_1("Setting Subscription explicitly for GetParams from cache for profile 0x%x", *profile);
      (*profile)->SetSubscriptionId(subs, false);
    }
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type DBMgr::GetProfiles(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  Profile ***profiles,
  uint16 *profiles_count)
{
  PROFILE_DB_LOG_MED_1("GetProfiles(): tech_mask %d", tech_mask);
  ds_profile_db_info_type param_info = { 0 };
  return GetProfilesInternal(tech_mask,
                             INVALID_PROFILE_PARAM,
                             &param_info, subs, profiles, profiles_count);
}

ds_profile_db_result_type DBMgr::GetProfiles(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_ident_type ident,
  const ds_profile_db_info_type *param_info,
  ds_profile_db_subs_type subs,
  Profile ***profiles,
  uint16 *profiles_count)
{
  PROFILE_DB_LOG_MED_2("GetProfiles(): tech_mask %d, ident %d", tech_mask, ident);
  return GetProfilesInternal(tech_mask, ident, param_info,
                             subs, profiles, profiles_count);
}


ds_profile_db_result_type DBMgr::GetProfilesInternal(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_ident_type ident,
  const ds_profile_db_info_type *param_info,
  ds_profile_db_subs_type subs,
  Profile ***profiles,
  uint16 *profiles_count)
{
  ds_profile_db_result_type res;
  uint16 count = 0;

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (!profiles || !profiles_count || !param_info)
  {
    PROFILE_DB_LOG_ERROR_3("GetProfilesInternal(): invalid profiles 0x%p, profiles_count "
                           "0x%p or param_info 0x%p pointers",
                           profiles, profiles_count, param_info);
    return DB_RESULT_ERR_INVAL_OP;
  }
  *profiles_count = 0;

  res = Profile::ValidateSupportedTechMask(tech_mask);
  if (DB_RESULT_SUCCESS != res)
  {
    return res;
  }
  /* make sure the param_info is valid */
  if (INVALID_PROFILE_PARAM != ident)
  {
    if (!param_info->len || !param_info->buf)
    {
      if (!((ident == GENERAL_PROFILE_PARAM_APN_NAME) && (param_info->len == 0)))
      {
        PROFILE_DB_LOG_ERROR_2("GetProfilesInternal(): invalid param_info len %d or buf 0x%p",
                               param_info->len, param_info->buf);
        return DB_RESULT_ERR_INVAL_OP;
      }
    }
  }

  if (DB_RESULT_SUCCESS != (res = Profile::ValidateTechMask(tech_mask)))
  {
    return res;
  }

  count = cacheMgr.GetNumOfCacheEntry(tech_mask, subs);
  if (!count)
  {
    *profiles_count = 0;
    *profiles = NULL;
    return DB_RESULT_SUCCESS;
  }

  PS_SYSTEM_HEAP_MEM_ALLOC(*profiles, (count) * sizeof(Profile*), 
                           Profile**);
  if (NULL == *profiles) {
    return DB_RESULT_ERR_OUT_OF_MEMORY;
  }
  memset(*profiles, 0, count * sizeof(Profile *));

  /* iterate over cache table and get all the  profiles */
  count = 0;
  PROFILE_DB_LOG_MED_3("Getting Profiles for techmask %d, subs %d, profiles 0x%x", tech_mask, subs, profiles);
  res = cacheMgr.GetProfilesFromInternalCache(profiles, &count, tech_mask, ident, param_info, subs);
  if (DB_RESULT_SUCCESS != res)
  {
    goto bail;
  }

  if (0 == count)
  {
    *profiles_count = 0;
    PROFILE_DB_LOG_ERROR("GetProfilesFromInternalCache returned 0 count");
    goto bail;
  }
  *profiles_count = count;
  return DB_RESULT_SUCCESS;

bail:
  CleanupProfiles(count, *profiles);
  *profiles = NULL;
  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}

ds_profile_db_result_type DBMgr::ResetProfile(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type res = DB_RESULT_FAIL;
  PROFILE_DB_LOG_MED_2("ResetProfile(): profile num %d, tech_mask %d", num, tech_mask);

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (DB_RESULT_SUCCESS != (res = Profile::ValidateSupportedTechMask(tech_mask)))
  {
    return res;
  }

  /* do a quick before you go to heavy operations of constructing a full profile from cache */
  CacheMgr::cache_entry *c_e = cacheMgr.GetCache(num, tech_mask, subs);
  if (!c_e)
  {
    PROFILE_DB_LOG_ERROR_2("ResetProfile(): trying to reset non existing profile %d with tech_mask %d", num, tech_mask);
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  if (!CanWriteProfile(num, c_e->tech_mask))
  {
    PROFILE_DB_LOG_ERROR_2("ResetProfile(): resetting of profile %d with mask %d is not allowed",
                           num, c_e->tech_mask);
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  /* get an existing profile and check its tech mask */
  Profile *profile = NULL;
  bool persistency = false;

  if (DB_RESULT_SUCCESS != (res = GetProfile(num, tech_mask, subs, &profile)))
  {
    PROFILE_DB_LOG_ERROR_1("ResetProfile(): could not get profile, error %d", res);
    return res;
  }


  /* we need to keep those intact */
  if (profile)
  {
    persistency = !ShouldBeTreatedAsNonPersistent(profile);
    subs = profile->GetSubcriptionId();
    delete profile;
    profile = NULL;
  } else
  {
    PROFILE_DB_LOG_ERROR("ResetProfile():Get Profile returned NULL profile");
  }

  /* now create a new profile (a default one) with the same tech mask */
  profile = Profile::CreateInstance(tech_mask);
  if (!profile)
  {
    return DB_RESULT_ERR_OUT_OF_PROFILES;
  }

  profile->SetNumber(num);
  profile->SetPersistency(persistency);
  profile->SetSubscriptionId(subs, false);

  res = SetProfileInternal(profile, true);
  delete profile;
  profile = NULL;
  return res;
}

ds_profile_db_result_type DBMgr::ResetProfileParam(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_ident_type       ident,
  ds_profile_db_subs_type        subs,
  boolean mark_invalid)
{
  ds_profile_db_result_type res = DB_RESULT_FAIL;
  PROFILE_DB_LOG_MED_3("ResetProfileParam(): profile num %d, tech_mask %d, ident %d", num, tech_mask, ident);

  /* TODO revisit the implementation in phase 2 */
  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (DB_RESULT_SUCCESS != (res = Profile::ValidateSupportedTechMask(tech_mask)))
  {
    return res;
  }
  /* do a quick before you go to heavy operations of constructing a full profile from cache */
  CacheMgr::cache_entry *c_e = cacheMgr.GetCache(num, tech_mask, subs);
  if (!c_e)
  {
    PROFILE_DB_LOG_ERROR_1("ResetProfileParam(): trying to reset param for non existing profile %d", num);
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  if (!CanWriteProfile(num, c_e->tech_mask))
  {
    PROFILE_DB_LOG_ERROR_2("ResetProfileParam(): resetting of param in profile %d with mask %d is not allowed",
                           num, c_e->tech_mask);
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  /* get an existing profile */
  Profile *profile = NULL;
  if (DB_RESULT_SUCCESS != (res = GetProfile(num, tech_mask, subs, &profile)))
  {
    PROFILE_DB_LOG_ERROR_1("ResetProfileParam(): could not get profile, error %d", res);
    return res;
  }

  /* reset the required param inside that profile */
  if (profile)
  {
    if (DB_RESULT_SUCCESS != (res = profile->ResetParam(ident, mark_invalid)))
    {
      goto bail;
    }

#ifdef FEATURE_USER_CONFIGURE_EFS
    res = SetProfile(profile, true);
#else
    res = SetProfile(profile);
#endif
  }
  /* fall through */

bail:
  if (profile)
  {
    delete profile;
    profile = NULL;
  }

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}

ds_profile_db_result_type DBMgr::GetDefaultProfileNum(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family)
{
  PROFILE_DB_LOG_MED_2("GetDefaultProfileNum(): tech %d, family %d", tech, family);

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (!num)
  {
    PROFILE_DB_LOG_ERROR("GetDefaultProfileNum(): NULL num pointer");
    return DB_RESULT_ERR_INVAL_OP;
  }

  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = Profile::ValidateTech(tech)))
  {
    return res;
  }

  TechMgr *techMgr = GetTechManager(tech);
  if (!techMgr)
  {
    PROFILE_DB_LOG_ERROR_1("GetDefaultProfileNum(): unsupported technology %d", tech);
    return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
  }

  return techMgr->GetDefaultProfileNum(num, family);
}

ds_profile_db_result_type DBMgr::GetDefaultProfileNum(
  ds_profile_db_profile_num_type *num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs)
{
  PROFILE_DB_LOG_MED_3("GetDefaultProfileNum(): tech %d, family %d, subs %d", tech, family, subs);

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (!num)
  {
    PROFILE_DB_LOG_ERROR("GetDefaultProfileNum(): NULL num pointer");
    return DB_RESULT_ERR_INVAL_OP;
  }

  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = Profile::ValidateTech(tech)))
  {
    return res;
  }

  if (DB_RESULT_SUCCESS != (res = Profile::ValidateSubscriptionId(subs)))
  {
    return res;
  }

  TechMgr *techMgr = GetTechManager(tech);
  if (!techMgr)
  {
    PROFILE_DB_LOG_ERROR_1("GetDefaultProfileNum(): unsupported technology %d", tech);
    return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
  }

  return techMgr->GetDefaultProfileNum(num, family, subs);
}

ds_profile_db_result_type DBMgr::SetDefaultProfileNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family)
{
  PROFILE_DB_LOG_MED_3("SetDefaultProfileNum(): num %d tech %d, family %d",
                       num, tech, family);

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = Profile::ValidateTech(tech)))
  {
    return res;
  }

  TechMgr *techMgr = GetTechManager(tech);
  if (!techMgr)
  {
    PROFILE_DB_LOG_ERROR_1("SetDefaultProfileNum(): unsupported technology %d", tech);
    return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
  }

  return techMgr->SetDefaultProfileNum(num, family);
}

ds_profile_db_result_type DBMgr::SetDefaultProfileNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_tech_type tech,
  ds_profile_db_family_type family,
  ds_profile_db_subs_type subs)
{
  PROFILE_DB_LOG_MED_4("SetDefaultProfileNum(): num %d tech %d, family %d subs %d",
                       num, tech, family, subs);

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = Profile::ValidateTech(tech)))
  {
    return res;
  }

  if (DB_RESULT_SUCCESS != (res = Profile::ValidateSubscriptionId(subs)))
  {
    return res;
  }

  /* if profile num exists, make sure its subs id is equal to the one we are
     trying to set */
  Profile *profile;
  if (DB_RESULT_SUCCESS == (res = cacheMgr.GetCache(num, DB_TECH_TO_TECHMASK(tech), subs, &profile, false)))
  {
    if ((profile) && (profile->GetSubcriptionId() != subs))
    {
      PROFILE_DB_LOG_ERROR_2("SetDefaultProfileNum(): trying to set subs id %d "
                             "for existing profile with subs id %d",
                             subs, profile->GetSubcriptionId());
      return DB_RESULT_ERR_INVAL_OP;
    }
  }

  TechMgr *techMgr = GetTechManager(tech);
  if (!techMgr)
  {
    return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
  }

  return techMgr->SetDefaultProfileNum(num, family, subs);
}

ds_profile_db_result_type DBMgr::GetProfilesRange(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_profile_num_type *min_num,
  ds_profile_db_profile_num_type *max_num)
{
  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (!min_num || !max_num)
  {
    PROFILE_DB_LOG_HIGH_2("GetProfilesRange(): invalid min_num 0x%p or max_num 0x%p pointers",
                          min_num, max_num);
    return DB_RESULT_ERR_INVAL_OP;
  }

  ds_profile_db_result_type res;
  if (DB_RESULT_SUCCESS != (res = Profile::ValidateTechMask(tech_mask)))
  {
    return res;
  }

  TechMgr *techMgr = NULL;
  *min_num = 0;
  *max_num = DB_MAXIMUM_PROFILE_NUM;
  ds_profile_db_profile_num_type tmp_min_num = 0;
  ds_profile_db_profile_num_type tmp_max_num = 0;

  /* find intersection of all ranges for technologies in mask */
  DB_FOR_EVERY_TECH_IN_MASK_DO_START(tech_mask, tech)
  if ((techMgr = GetTechManager(tech)))
  {
    techMgr->GetProfilesRange(&tmp_min_num, &tmp_max_num);
  }
  if (tmp_min_num > *min_num)
  {
    *min_num = tmp_min_num;
  }
  if (tmp_max_num < *max_num)
  {
    *max_num = tmp_max_num;
  }
  if (*min_num > *max_num)
  {
    return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  return DB_RESULT_SUCCESS;
}

ds_profile_db_callback_id_type DBMgr::RegisterForNotifications(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_cb_type cback,
  ds_profile_db_subs_type subs,
  void *user_data,
  bool mode_handler /* = false */,
  bool treat_tech_mask_as_OR  /*  = false */)
{
  static int iter = 0;
  PROFILE_DB_LOG_MED_4("RegisterForNotifications(): profile num %d, tech_mask %d "
                       "mode_handler %d treat_tech_mask_as_OR %d",
                       num, tech_mask, mode_handler, treat_tech_mask_as_OR);

  if (callbackMgr->WithinCallback())
  {
    return 0;
  }

  if (0 != mutex->Lock())
  {
    return 0;
  }
  AutoReleseMutex a_m(mutex);

  ds_profile_db_result_type res;
 
  if (DB_RESULT_SUCCESS != (res = Profile::ValidateTechMask(tech_mask)))
  {
    return res;
  }
  if (DB_RESULT_SUCCESS != (res = Profile::ValidateSubscriptionId(subs)))
  {
    return res;
  }
  if (DB_RESULT_SUCCESS != (res = Profile::ValidateSupportedTechMask(tech_mask)))
  {
    return res;
  }

  if (NULL == cback)
  {
    PROFILE_DB_LOG_ERROR("RegisterForNotifications(): NULL cback function pointer, exiting");
    return DB_RESULT_ERR_INVAL;
  }

  /* registration per profile is not allowed for non-existing ones */
  CacheMgr::cache_entry *c_e = NULL;
  if (DB_UNSPECIFIED_PROFILE_NUM != num)
  {
    if (!(c_e = cacheMgr.GetCache(num, tech_mask, subs)))
    {
      PROFILE_DB_LOG_ERROR_1("RegisterForNotifications(): profile %d does not exist, "
                             "registration is not allowed", num);
      return 0;
    }

    if (!DB_IS_SUBSET_OF_TECHMASK(tech_mask, c_e->tech_mask))
    {
      PROFILE_DB_LOG_ERROR_3("RegisterForNotifications(): profile %d mask %d does not"
                             "support the mask %d you are trying to register to %d",
                             num, c_e->tech_mask, tech_mask);
      return 0;
    }
  }

  ds_profile_db_callback_id_type callback_id = callbackMgr->Register(num,
                                                                     tech_mask,
                                                                     cback,
                                                                     user_data,
                                                                     subs,
                                                                     mode_handler,
                                                                     treat_tech_mask_as_OR);
  if (!callback_id)
  {
    return 0;
  }

  if (mode_handler)
  {
    if (DB_RESULT_SUCCESS != NotifyClientWithPowerUpEvent(num, tech_mask, treat_tech_mask_as_OR,
                                                          cback, callback_id, user_data,
                                                          subs, DB_POWER_UP_EVENT))
    {
      DeregisterForNotifications(callback_id, subs);
      return 0;
    }
    ids[iter].callback_id = callback_id;
    ids[iter].cback = cback;
    ids[iter].treat_tech_mask_as_OR = treat_tech_mask_as_OR;
    ids[iter].tech_mask = tech_mask;
    ids[iter].user_data = user_data;
    ids[iter].num = num;
    iter++;
  }

  PROFILE_DB_LOG_MED_1("RegisterForNotifications(): success, callback_id is 0x%x",
                       callback_id);

  return callback_id;
}

ds_profile_db_result_type DBMgr::DeregisterForNotifications(
  ds_profile_db_callback_id_type callback_id,
  ds_profile_db_subs_type subs)
{
  PROFILE_DB_LOG_MED_2("DeregisterForNotifications(): callback_id %d, subs %d", 
                       callback_id, subs);

  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  return callbackMgr->Deregister(callback_id, subs);
}


/* private functions */
TechMgr* DBMgr::GetTechManager(ds_profile_db_tech_type tech)
{
  return techMgrs[tech];
}

ds_profile_db_result_type DBMgr::InitCreationParams(ds_profile_db_creation_params_type *params)
{
  if (!params)
  {
    return DB_RESULT_ERR_INVAL;
  }

  params->profile_num     = DB_UNSPECIFIED_PROFILE_NUM;
  params->persistent      = TRUE;
  params->subscription_id = Profile::GetDefaultSubscriptionId();

  return DB_RESULT_SUCCESS;
}

void DBMgr::ReleaseRangeInCache(
  ds_profile_db_profile_num_type start,
  ds_profile_db_profile_num_type end,
  ds_profile_db_supported_tech_mask tech_mask,
  bool is_persistent,
  ds_profile_db_subs_type subs)
{
  for (ds_profile_db_profile_num_type i = start; i <= end; i++)
  {
    cacheMgr.RemoveCache(i, tech_mask, subs);
    ReleaseNumForProfile(is_persistent,
                         i,
                         tech_mask,
                         subs);
  }
}





void DBMgr::CleanupProfiles(unsigned int profiles_count,
                            Profile **profiles,
                            bool   remove_from_cache, /* = false */
                            bool   dealocate_numbers /* = false */)
{
  if (!profiles)
  {
    return;
  }

  for (unsigned int i = 0; i < profiles_count; i++)
  {
    if (profiles[i])
    {
      if (remove_from_cache)
      {
        cacheMgr.RemoveCache(profiles[i]->GetNum(), profiles[i]->GetSupportedTechMask(),
                             profiles[i]->GetSubcriptionId());
      }

      if (dealocate_numbers)
      {
        ReleaseNumForProfile(!ShouldBeTreatedAsNonPersistent(profiles[i]),
                             profiles[i]->GetNum(),
                             profiles[i]->GetSupportedTechMask(),
                             profiles[i]->GetSubcriptionId());
      }

      delete profiles[i];
      profiles[i] = NULL;
    }
  }

  PS_SYSTEM_HEAP_MEM_FREE(profiles);

}

ds_profile_db_result_type DBMgr::OnTheFlyToCache(Profile **profiles,
                                                 unsigned int *profiles_count,
                                                 ds_profile_db_subs_type subs)
{

  if (DB_RESULT_SUCCESS == Profile::ValidateSupportedTechMask(DB_TECH_TO_TECHMASK(DB_TECH_3GPP2)))
  {
    /* create some profiles on the fly */
    /*************************************/
    /* for now it is only profile 0 for 3gpp2 */
    unsigned int create_on_the_fly_num = 1;
  
    if (create_on_the_fly_num > *profiles_count)
    {
      return DB_RESULT_ERR_OUT_OF_PROFILES;
    }
    PROFILE_DB_LOG_HIGH_1("On the Fly to cache for subs %d", subs);
    /* check that profile does not already exist in cache */
    if (cacheMgr.GetCache(0, DB_TECH_TO_TECHMASK(DB_TECH_3GPP2), subs))
    {
      PROFILE_DB_LOG_MED("OnTheFlyToCache(): profile 0 already exists");
      *profiles_count = 0;
      return DB_RESULT_SUCCESS;
    }

    Profile *profile = Profile::CreateInstance(DB_TECH_3GPP2);
    if (!profile)
    {
      return DB_RESULT_ERR_OUT_OF_PROFILES;
    }

    profile->SetSubscriptionId(subs, false);
    profile->SetPersistency(false);
    profile->SetNumber(0);
    profiles[0] = profile;
  
    *profiles_count = create_on_the_fly_num;

    ds_profile_db_result_type res = StoreInCacheDuringInit(profiles, *profiles_count, subs);
    if (DB_RESULT_SUCCESS)
    {
      goto bail;
    }
    return DB_RESULT_SUCCESS;

bail:
    if (profile)
    {
      delete profile;
      profile = NULL;
    }
    PROFILE_DB_LOG_ERROR_ON_EXIT(res);
    return res;
  }
  else
  {
    *profiles_count = 0;
    return DB_RESULT_SUCCESS;
  }

}

ds_profile_db_result_type DBMgr::StoreInCacheDuringInit(Profile **profiles, unsigned int profiles_count,
                                                        ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type res;

  for (unsigned int i = 0; i < profiles_count; i++)
  {
    if (profiles[i])
    {
      if (DB_RESULT_SUCCESS != (res = GetNumForProfile(profiles[i], NULL, subs, false, true)))
    {
      return res;
    }
    PROFILE_DB_LOG_HIGH_3("Before Create cache for profile[%d] = 0x%x for num %d", i, profiles[i],profiles[i]->GetNum());
    if (DB_RESULT_SUCCESS != (res = cacheMgr.CreateCache(profiles[i])))
    {
      return res;
    }
  }
    else
    {
      PROFILE_DB_LOG_ERROR("Profile is NULL");
    }
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type DBMgr::MarkRangeInCacheDuringInit(
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_profile_num_type start_range,
  ds_profile_db_profile_num_type end_range,
  bool is_persistent,
  ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type res;

  for (ds_profile_db_profile_num_type i = start_range; i < end_range; i++)
  {
    if (DB_RESULT_SUCCESS != (res = GetNumForProfile(i, tech_mask,
                                                     is_persistent, subs,
                                                     NULL, false, true)))
    {
      return res;
    }
    if (DB_RESULT_SUCCESS != (res = cacheMgr.CreateCache(i, tech_mask, subs, is_persistent, NULL)))
    {
      return res;
    }
  }

  return DB_RESULT_SUCCESS;
}


ds_profile_db_result_type DBMgr::FromFileToCache(Profile **profiles,
                                                 unsigned int *profiles_count,
                                                 ds_profile_db_subs_type subs)
{
  ds_profile_db_result_type res;

  /* read everything there is from EFS */
  /*************************************/
  File *file = Platform::Instance()->GetFile(subs);
  if (!file)
  {
    res = DB_RESULT_ERR_FILE_ACCESS;
    goto bail;
  }

  res = file->ReadAll(profiles, profiles_count);
  if (DB_RESULT_ERR_OUT_OF_PROFILES == res)
  {
    PROFILE_DB_LOG_HIGH_1(" More profiles read from EFS than expected . Please delete excess files res %d", res);
  }
  else if (DB_RESULT_SUCCESS != res)
  {
    goto bail;
  }

  res = StoreInCacheDuringInit(profiles, *profiles_count, subs);
  if (DB_RESULT_SUCCESS != res)
  {
    PROFILE_DB_LOG_HIGH_1("FromFileToCache(): could not store profiles "
                          "fetched from EFS to cache,"
                          "error %d, still proceeding", res);
    res = DB_RESULT_SUCCESS;
  }

  res = DB_RESULT_SUCCESS;
  /* fall through */

bail:

  if (file)
  {
    delete file;
    file = NULL;
  }

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}

ds_profile_db_result_type DBMgr::GetNumForProfile(ds_profile_db_profile_num_type num,
                                                  ds_profile_db_supported_tech_mask tech_mask,
                                                  bool is_persistent,
                                                  ds_profile_db_subs_type subs,
                                                  ds_profile_db_profile_num_type *new_num /* = NULL */,
                                                  bool overwrite, /* = false */
                                                  bool during_init /* = false */)
{
  ds_profile_db_result_type res;
  ds_profile_db_profile_num_type new_num_internal;

  if (DB_UNSPECIFIED_PROFILE_NUM == num)
  {
    res = AllocateNum(&new_num_internal, subs, tech_mask);
  } else
  {
    res = CheckAndReserveNum(num, tech_mask, subs, during_init, overwrite);
    new_num_internal = num;
  }

  if (DB_RESULT_SUCCESS != res)
  {
    return res;
  }

  /* only decrease the count if the profile is actually a new one */
  if (0 == cacheMgr.GetCache(num, tech_mask, subs))
  {
    if (DB_RESULT_SUCCESS != (res = DecProfileCount(is_persistent, tech_mask, subs)))
    {
      goto bail;
    }
  }

  if (new_num)
  {
    *new_num = new_num_internal;
  }

  return DB_RESULT_SUCCESS;

bail:

  FreeNum(new_num_internal, tech_mask, FALSE, subs);
  return res;
}


ds_profile_db_result_type DBMgr::GetNumForProfile(const Profile *profile,
                                                  ds_profile_db_profile_num_type *new_num /* = NULL */,
                                                  ds_profile_db_subs_type subs,  /*= DB_PROFILE_ACTIVE_SUBSCRIPTION_1 */
                                                  bool overwrite, /* = false */
                                                  bool during_init /* = false */)
{
  return GetNumForProfile(profile->GetNum(),
                          profile->GetSupportedTechMask(),
                          !ShouldBeTreatedAsNonPersistent(profile),
                          profile->GetSubcriptionId(),
                          new_num,
                          overwrite,
                          during_init);
}

ds_profile_db_result_type DBMgr::ReleaseNumForProfile(
  bool is_persistent,
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  boolean enable_deleting_default_profile
)
{
  ds_profile_db_result_type res = FreeNum(num, tech_mask, enable_deleting_default_profile, subs);
  if (DB_RESULT_SUCCESS != res)
  {
    return res;
  }

  IncProfileCount(is_persistent, tech_mask, subs);

  return DB_RESULT_SUCCESS;
}

/* 
  Does a couple of checks whether the profile number 
  can be reserved in the system.
  If the number is within the range that DB allocates numbers from
  this number will be reserved in it, so that we will not give it away
  for new profiles that will be created afterwards
*/
ds_profile_db_result_type DBMgr::CheckAndReserveNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  ds_profile_db_subs_type subs,
  bool during_init,
  bool overwrite)
{
  TechMgr *techMgr = NULL;
  ds_profile_db_result_type res = DB_RESULT_SUCCESS;
  bool is_epc = (tech_mask == (DB_TECH_TO_TECHMASK(DB_TECH_COMMON) |
                                 DB_TECH_TO_TECHMASK(DB_TECH_3GPP) |
                                 DB_TECH_TO_TECHMASK(DB_TECH_3GPP2))) ? true : false;


  /* two checks need to be done before the profile can be reserved 
     1. Either profile is in valid reservation range for every involved tech 
     2. Or profile is being set during init and it is within valid creation range 
   */
  DB_FOR_EVERY_TECH_IN_MASK_DO_START(tech_mask, tech)
  if ((techMgr = GetTechManager(tech)))
  {
    if (!techMgr->CanReserveProfileOutsideOfAllocationRange(num, during_init, &overwrite, is_epc))
    {
      res = DB_RESULT_ERR_INVAL_PROFILE_NUM;
      if (!during_init)
      {
        return res;
      } 
      else
      {
        break;
      }
    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  //Handle 3GPP special case here.
  if ( DB_TECH_TO_TECHMASK(DB_TECH_3GPP) == tech_mask)
  {
    if ((techMgr = GetTechManager(DB_TECH_3GPP)))
    {
      res = techMgr->CheckAndReserveNum(num, subs, overwrite);
    }
  }
  else
  {
  /* at this point, either we passed the check 1) or we are during init */
  /* update numbers pool if the number is managed by it and the place is not taken already */
  if (WithinNumberPoolRange(num))
  {
    if (overwrite)
    {
      numbersPool[subs][num - TechMgr::ALLOC_RANGE_MIN] = true;
      return DB_RESULT_SUCCESS;
    }

    if (!numbersPool[subs][num - TechMgr::ALLOC_RANGE_MIN])
    {
      numbersPool[subs][num - TechMgr::ALLOC_RANGE_MIN] = true;
      return DB_RESULT_SUCCESS;
      } 
      else
      {
        return DB_RESULT_ERR_INVAL_PROFILE_NUM;
      }
    }
  }
  return res;
}

ds_profile_db_result_type DBMgr::AllocateNum(ds_profile_db_profile_num_type *num,
                                             ds_profile_db_subs_type subs,
                                             ds_profile_db_supported_tech_mask tech_mask)
{
  static int lastUnusedIndex[DB_PROFILE_ACTIVE_SUBSCRIPTION_MAX] = { 0 };
  TechMgr *techMgr = NULL;

  //Profile numbering of 3gpp profile should start from 1 -24
  //for backward compatibility

  if (DB_TECH_TO_TECHMASK(DB_TECH_3GPP) == tech_mask)
  {
    if (NULL != (techMgr = GetTechManager(DB_TECH_3GPP)))
    {
      return (techMgr->AllocateNum(num,subs,tech_mask));
    }
    else
    {
      return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
    }
  }
  else
  {
  /* i counts the iteration, the actual index to use is lastUnusedIndex */
  for (int i = 0; i < DS_PROFILE_DB_MAX_PROFILE_COUNT;
       i++,
       lastUnusedIndex[subs] = (++lastUnusedIndex[subs]) % DS_PROFILE_DB_MAX_PROFILE_COUNT)
  {
    if (false == numbersPool[subs][lastUnusedIndex[subs]])
    {
      numbersPool[subs][lastUnusedIndex[subs]] = true;
      *num = lastUnusedIndex[subs] + TechMgr::ALLOC_RANGE_MIN;
      lastUnusedIndex[subs] = (++lastUnusedIndex[subs]) % DS_PROFILE_DB_MAX_PROFILE_COUNT;
      return DB_RESULT_SUCCESS;
    }
  }
  }
  return DB_RESULT_ERR_OUT_OF_PROFILES;
}

ds_profile_db_result_type DBMgr::FreeNum(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask mask,
  boolean enable_deleting_default_profile,
  ds_profile_db_subs_type subs)
{
  TechMgr* techMgr = NULL;

  if (!CanDeleteProfile(num, mask, enable_deleting_default_profile))
  {
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  //Profile numbering of 3gpp profile should start from 1 -24
  //for backward compatibility

  if (DB_TECH_TO_TECHMASK(DB_TECH_3GPP) == mask)
  {
    if (NULL != (techMgr = GetTechManager(DB_TECH_3GPP)))
    {
      return (techMgr->FreeNum(num, mask, subs));
    }
    else
    {
      return DB_RESULT_ERR_INVAL_PROFILE_TYPE;
    }
  }

  /* if the number was never taken, return an error */
  if (WithinNumberPoolRange(num))
  {
    if (!numbersPool[subs][num - TechMgr::ALLOC_RANGE_MIN])
    {
      return DB_RESULT_ERR_INVAL_PROFILE_NUM;
    }

    numbersPool[subs][num - TechMgr::ALLOC_RANGE_MIN] = false;
  }

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type DBMgr::DecProfileCount(
  bool is_persistent,
  ds_profile_db_supported_tech_mask mask,
  ds_profile_db_subs_type subs)
{
  TechMgr *techMgr = NULL;
  bool     firstTech = false;

  DB_FOR_EVERY_TECH_IN_MASK_DO_START(mask, tech)
  if ((techMgr = GetTechManager(tech)))
  {
    if (techMgr->GetCount(is_persistent, subs) <= 0)
    {
      return DB_RESULT_ERR_OUT_OF_PROFILES;
    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  DB_FOR_EVERY_TECH_IN_MASK_DO_START(mask, tech)
  if ((techMgr = GetTechManager(tech)))
  {
    if (!firstTech)
    {
      techMgr->DecCount(is_persistent, subs);
      if (is_persistent)
      {
        firstTech = true;
      }
    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  return DB_RESULT_SUCCESS;
}

ds_profile_db_result_type DBMgr::IncProfileCount(
  bool is_persistent,
  ds_profile_db_supported_tech_mask mask,
  ds_profile_db_subs_type subs)
{
  TechMgr *techMgr = NULL;
  bool firstTech   = false;

  DB_FOR_EVERY_TECH_IN_MASK_DO_START(mask, tech)
  if ((techMgr = GetTechManager(tech)))
  {
    if (!firstTech)
    {
      if (techMgr->IncCount(is_persistent, subs) <= 0)
      {
        return DB_RESULT_FAIL;
      }
       //If it is persistent consider EPC as single entity
      //CR772637
      if (is_persistent)
      {
        firstTech = true;
      }
    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  return DB_RESULT_SUCCESS;
}


ds_profile_db_result_type DBMgr::NotifyClients(
  Profile *profile,
  ds_profile_db_event_type event,
  ds_profile_db_subs_type subs)
{
  PROFILE_DB_LOG_MED_2("NotifyClients(): event %d, subs %d", event, subs);

  ds_profile_db_result_type res;

  if (DB_RESULT_SUCCESS != (res = callbackMgr->Notify(profile, event, subs)))
  {
    goto bail;
  }
#ifndef FEATURE_USER_CONFIGURE_EFS
  profile->ResetParamTracking();
#endif

  return DB_RESULT_SUCCESS;

bail:

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);

  return res;
}

ds_profile_db_result_type DBMgr::NotifyClientWithPowerUpEvent(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  bool treat_tech_mask_as_OR,
  ds_profile_db_cb_type cback,
  ds_profile_db_callback_id_type callback_id,
  void *user_data,
  ds_profile_db_subs_type subs,
  ds_profile_db_event_type event,
  Profile **profiles,
  uint16 profile_count)
{
  PROFILE_DB_LOG_MED_3("NotifyClientWithPowerUpEvent(): profile num %d, tech_mask %d, callback_id %d", num, tech_mask, callback_id);

  if (!profiles && profile_count == 0)
  {
    return callbackMgr->NotifyOnPowerUp(profiles_for_power_up,
                                        profiles_count_for_power_up,
                                        num,
                                        tech_mask,
                                        cback,
                                        callback_id,
                                        event,
                                        user_data,
                                        subs,
                                        treat_tech_mask_as_OR);
  } else
  {
    return callbackMgr->NotifyOnPowerUp(profiles,
                                        profile_count,
                                        num,
                                        tech_mask,
                                        cback,
                                        callback_id,
                                        event,
                                        user_data,
                                        subs,
                                        treat_tech_mask_as_OR);
  }
}

bool DBMgr::CanDeleteProfile(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask,
  boolean enable_deleting_default_profile)
{
  TechMgr *techMgr = NULL;

  DB_FOR_EVERY_TECH_IN_MASK_DO_START(tech_mask, tech)
  if ((techMgr = GetTechManager(tech)))
  {
    if (!techMgr->CanDeleteProfile(num, enable_deleting_default_profile))
    {
      return false;
    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  return true;
}

bool DBMgr::CanWriteProfile(
  ds_profile_db_profile_num_type num,
  ds_profile_db_supported_tech_mask tech_mask)
{
  TechMgr *techMgr = NULL;

  DB_FOR_EVERY_TECH_IN_MASK_DO_START(tech_mask, tech)
  if ((techMgr = GetTechManager(tech)))
  {
    if (!techMgr->CanWriteProfile(num))
    {
      return false;
    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  return true;
}

bool DBMgr::ShouldBeTreatedAsNonPersistent(
  const Profile *profile)
{
  if ((profile) && (!profile->IsPersistent()))
  {
    return true;
  }

  TechMgr *techMgr = NULL;

  if (profile)
  {
    DB_FOR_EVERY_TECH_IN_MASK_DO_START(profile->GetSupportedTechMask(), tech)
    if ((techMgr = GetTechManager(tech)))
    {
      if (!techMgr->ShouldBeTreatedAsNonPersistent(profile->GetNum()))
      {
        return false;
      }
    }
    DB_FOR_EVERY_TECH_IN_MASK_DO_END
  } else
  {
    PROFILE_DB_LOG_ERROR("Profile Ptr is NULL. Ds Profile DB is in a bad state");
  }

  return true;
}


/* normally, it is enforced that file creation for profiles which are being modified 
   have been issued. For some profiles we want to make an exception for this rule and
   actually allow creating the file during the modification if the file does not exist */
bool DBMgr::AllowToCreateFileOnFirstModify(
  const Profile *profile)
{
  TechMgr *techMgr = NULL;

  DB_FOR_EVERY_TECH_IN_MASK_DO_START(profile->GetSupportedTechMask(), tech)
  if ((techMgr = GetTechManager(tech)))
  {
    if (!techMgr->AllowToCreateFileOnFirstModify(profile->GetNum()))
    {
      return false;
    }
  }
  DB_FOR_EVERY_TECH_IN_MASK_DO_END

  return true;
}

ds_profile_db_tech_type DBMgr::GetTechType(ds_profile_db_profile_num_type num,
                                           ds_profile_db_subs_type subs)
{
  ds_profile_db_tech_type tech_type = DB_TECH_INVALID;

  /* TODO revisit the implementation in phase 2 */
  if (callbackMgr->WithinCallback())
  {
    return DB_TECH_INVALID;
  }

  if (0 != mutex->Lock())
  {
    return DB_TECH_INVALID;
  }
  AutoReleseMutex a_m(mutex);

  if (DB_RESULT_SUCCESS != Profile::ValidateProfileNum(num))
  {
    return tech_type;
  }

  tech_type = cacheMgr.GetTechType(num, subs);
  return tech_type;
}

ds_profile_db_result_type DBMgr::GetPersistence(ds_profile_db_profile_num_type num,
                                                ds_profile_db_subs_type subs,
                                                boolean *is_persistent)
{
  ds_profile_db_result_type status = DB_RESULT_FAIL;

  /* TODO revisit the implementation in phase 2 */
  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (DB_RESULT_SUCCESS != (status = Profile::ValidateProfileNum(num)))
  {
    return status;
  }

  status = cacheMgr.GetPersistence(num, subs, is_persistent);
  return status;
}

ds_profile_db_result_type DBMgr::GetParamsInUse(Profile *profile,
                                                ds_profile_db_ident_type ident,
                                                ds_profile_db_info_type  *info,
                                                ds_profile_db_tech_type  tech_type,
                                                ds_profile_db_subs_type  subs)

{
  /* TODO revisit the implementation in phase 2 */
  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_ERR_INVAL_OP;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  boolean is_tech_param_set = false;
  TechMgr *tech_mgr = GetTechManager(tech_type);
  if (tech_mgr)
  {
    tech_mgr->GetIsEpcParamSet(profile->GetNum(), ident, &is_tech_param_set, subs);
  }

  return (profile->GetParamInUse(ident, info, tech_type, is_tech_param_set));
}

void DBMgr::CommandHandler
(
  ds_cmd_type *cmd_ptr
)
{
  switch (cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_PROFILE_DB_NV_REFRESH_EXT:
      RefreshCommandHandler(cmd_ptr);
      break;
    default:
      break;
  }
}

void DBMgr::DeviceModeCommandHandler
(
  ds3gsubsmgr_event_info_type *event_info_ptr
)
{
  ds_profile_db_subs_type subs;
  ds_profile_db_result_type res = DB_RESULT_SUCCESS;
  uint16 profile_count = 0;
  /*
  1.) If multi Sim then read the profiles from the other directories 
  2.) Else dont do anything if single sim 
  */
  if (callbackMgr->WithinCallback())
  {
    res = DB_RESULT_ERR_INVAL_OP;
    return;
  }

  if (0 != mutex->Lock())
  {
    res = DB_RESULT_FAIL;
    return;
  }

  AutoReleseMutex a_m(mutex);

  PROFILE_DB_LOG_HIGH_2("DeviceModeCommand Handler current device mode %d, prev device mode %d",
                        (int)event_info_ptr->device_mode.curr, (int)event_info_ptr->device_mode.prev);

  if (((event_info_ptr->device_mode.curr == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY) ||
      (event_info_ptr->device_mode.curr == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE))
      && event_info_ptr->device_mode.prev == SYS_MODEM_DEVICE_MODE_SINGLE_SIM)
  {
    subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_2;
    res = ReadFromEFSHelper(subs, DB_POWER_UP_EVENT, &profile_count);
    if (DB_RESULT_SUCCESS != res)
    {
      goto bail;
    }
  } 
  else if ((event_info_ptr->device_mode.curr == SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY) &&
           (event_info_ptr->device_mode.prev != SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY))
  {
    //Read from subs2 and subs 3
    subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_2;
    res = ReadFromEFSHelper(subs, DB_POWER_UP_EVENT, &profile_count);
    if (DB_RESULT_SUCCESS != res)
    {
      goto bail;
    }
    subs = DB_PROFILE_ACTIVE_SUBSCRIPTION_3;
    res = ReadFromEFSHelper(subs, DB_POWER_UP_EVENT, &profile_count);
    if (DB_RESULT_SUCCESS != res)
    {
      goto bail;
    }
    profile_count = 0;
  }

bail:
  PROFILE_DB_LOG_ERROR_ON_EXIT(res);
}

void DBMgr::ResetNumbers(ds_profile_db_subs_type subs)
{
  for (int i=0; i < DB_TECH_MAX - 1; i++)
  {
    techMgrs[i]->ResetNonPersistentCount(subs);
  }
}

void DBMgr::RefreshCommandHandler
(
  ds_cmd_type *cmd_ptr
)
{
  ds_profile_db_num_tech_type removed_entries[DS_PROFILE_DB_MAX_PROFILE_COUNT];
  uint32 count = 0;
  Profile **profiles = NULL;
  ds_profile_db_result_type res;
  uint16 count_of_profiles_read = 0;
  boolean enable_default_profile_delete = TRUE;

  if (callbackMgr->WithinCallback())
  {
    res = DB_RESULT_ERR_INVAL_OP;
    return;
  }

  if (0 != mutex->Lock())
  {
    res = DB_RESULT_FAIL;
    return;
  }

  AutoReleseMutex a_m(mutex);

  ds_profile_nv_refresh_cmd_type *cmd_info = (ds_profile_nv_refresh_cmd_type *)cmd_ptr->cmd_payload_ptr;

  PROFILE_DB_LOG_HIGH_1("RefreshCommand Handler for subs %d",
                        cmd_info->subs);
  /*
  1.) Clear the cache with subs id content 
  2.) Release the numbers. 
  3.) ReRead from the subs id directory and re populate 
  4.) Notify ModeHandlers about the New Event with same payload of new Profiles 
  5.) Consider all the corner cases 
  */

  memset(removed_entries, 0, sizeof(ds_profile_db_num_tech_type)*DS_PROFILE_DB_MAX_PROFILE_COUNT);
  cacheMgr.RemoveCacheForSubs((ds_profile_db_subs_type)(cmd_info->subs), removed_entries, &count);
  for (uint32 i = 0; i < count; i++)
  {
    ReleaseNumForProfile((removed_entries[i].is_persistent == TRUE), removed_entries[i].profile_num,
                         removed_entries[i].tech_mask,
                         (ds_profile_db_subs_type)(cmd_info->subs), enable_default_profile_delete);
  }
  ResetNumbers((ds_profile_db_subs_type)(cmd_info->subs));
  res = ReadFromEFSHelper((ds_profile_db_subs_type)(cmd_info->subs),
                          DB_REFRESH_PROFILE_EVENT, &count_of_profiles_read);

//Notify All Cients with 0 Profiles but NV refresh event.
  count_of_profiles_read = 0;
  if ((res = NotifyClientsDuringRefresh(profiles, count_of_profiles_read, DB_REFRESH_PROFILE_EVENT,
                                        (ds_profile_db_subs_type)(cmd_info->subs))) != DB_RESULT_SUCCESS)
  {
    goto bail;
  }
  res = DB_RESULT_SUCCESS;
  /* fall through */
bail:
  ds3geventmgr_set_and_notify_nv_refresh_status(DS3GEVENTMGR_CLIENT_ID_PROFILE);
  PROFILE_DB_LOG_ERROR_ON_EXIT(res);
  return;
}



ds_profile_db_result_type DBMgr::NotifyClientsDuringRefresh(
  Profile **profiles,
  uint16 profile_count,
  ds_profile_db_event_type event,
  ds_profile_db_subs_type subs)
{
  PROFILE_DB_LOG_MED_1("NotifyClients(): event %d", event);

  ds_profile_db_result_type res;

  if (DB_RESULT_SUCCESS != (res = callbackMgr->Notify(profiles, profile_count,
                                                      event, subs)))
  {
    goto bail;
  }
#ifndef FEATURE_USER_CONFIGURE_EFS
  profile->ResetParamTracking();
#endif
  return DB_RESULT_SUCCESS;
bail:

  PROFILE_DB_LOG_ERROR_ON_EXIT(res);
  return res;
}


ds_profile_db_result_type DBMgr::ReadFromEFSHelper
(
  ds_profile_db_subs_type subs,
  ds_profile_db_event_type event,
  uint16 *profile_count
)
{
  ds_profile_db_result_type res;
  Profile **profiles = NULL;
  unsigned int profiles_count = DS_PROFILE_DB_MAX_PROFILE_COUNT;
  unsigned int created_on_the_fly = 0;
  int iter = 0;
  File *file = NULL;

  /* read everything there is from EFS */
  /*************************************/
  PS_SYSTEM_HEAP_MEM_ALLOC(profiles, profiles_count * sizeof(Profile *),
                            Profile**);
  if (NULL == profiles)
  {
    res = DB_RESULT_ERR_OUT_OF_MEMORY;
    goto bail;
  }
  memset(profiles, 0, profiles_count * sizeof(Profile *));

  file = Platform::Instance()->GetFile(subs);
  if (!file)
  {
    res = DB_RESULT_ERR_FILE_ACCESS;
    goto bail;
  }

  if (subs == (ds_profile_db_subs_type)DB_PROFILE_ACTIVE_SUBSCRIPTION_1)
  {
    /* mark in cache special profiles (KDDI / OMH / etc) as if they exist, but do not create them */
    if (DB_RESULT_SUCCESS != (res = theInstance->MarkSpecialProfilesInCache(subs)))
    {
      PROFILE_DB_LOG_HIGH_1("Result for mark special profiles %d", res);
      goto bail;
    }
  }

  res = file->ReadAll(profiles, &profiles_count, subs);
  if (DB_RESULT_ERR_OUT_OF_PROFILES == res)
  {
    PROFILE_DB_LOG_HIGH_1("Profiles on EFS exceed the number supported, res = %d", res);
  }
  else if (DB_RESULT_SUCCESS != res)
  {
    goto bail;
  }

  PROFILE_DB_LOG_HIGH_1("ReadAll (): returned profiles %d", profiles_count);
  res = StoreInCacheDuringInit(profiles, profiles_count, subs);
  if (DB_RESULT_SUCCESS != res)
  {
    PROFILE_DB_LOG_HIGH_1("FromFileToCache(): could not store "
                          "profiles fetched from EFS to cache,"
                          "error %d, still proceeding", res);
    res = DB_RESULT_SUCCESS;
  }

  if (subs == (ds_profile_db_subs_type)DB_PROFILE_ACTIVE_SUBSCRIPTION_1)
  {
    /* create some special profiles on the fly */
    created_on_the_fly = DS_PROFILE_DB_MAX_PROFILE_COUNT - profiles_count;
    if (DB_RESULT_SUCCESS != (res = theInstance->OnTheFlyToCache(profiles + profiles_count,
                                                                 &created_on_the_fly, subs)))
    {
      goto bail;
    }
    profiles_count += created_on_the_fly;
  }
  if (file)
  {
    delete file;
    file = NULL;
  }
  //Notify Mode Handlers with the new profiles and the event with subs id.
  for (iter = 0; iter < 2; iter++)
  {
    if ((res = NotifyClientWithPowerUpEvent(ids[iter].num,
                                            ids[iter].tech_mask,
                                            ids[iter].treat_tech_mask_as_OR,
                                            ids[iter].cback, ids[iter].callback_id,
                                            ids[iter].user_data,
                                            subs,
                                            event,
                                            profiles,
                                            profiles_count))
        != DB_RESULT_SUCCESS)
    {
      goto bail;
    }
  }
  res = DB_RESULT_SUCCESS;
  /* fall through */
bail:
  *profile_count = profiles_count;
  CleanupProfiles(profiles_count, profiles);
  if (file)
  {
    delete file;
    file = NULL;
  }
  PROFILE_DB_LOG_ERROR_ON_EXIT(res);
  return res;
}

ds_profile_db_result_type DBMgr::IsValidProfile(
  ds_profile_db_profile_num_type num,
  ds_profile_db_subs_type        subs,
  ds_profile_db_tech_type        tech_type,
  boolean                        *is_valid
)
{

  /* TODO revisit the implementation in phase 2 */
  if (callbackMgr->WithinCallback())
  {
    return DB_RESULT_FAIL;
  }

  if (0 != mutex->Lock())
  {
    return DB_RESULT_FAIL;
  }
  AutoReleseMutex a_m(mutex);

  if (DB_RESULT_SUCCESS != Profile::ValidateProfileNum(num))
  {
    return DB_RESULT_ERR_INVAL_PROFILE_NUM;
  }

  *is_valid = false;
  return cacheMgr.IsValidProfile(num, subs, tech_type, is_valid);
}

void DBMgr::SetEPCSupportVal
(
  uint8 epc_support_val
)
{
  if(theInstance)
  {
    if (epc_support_val > 1) 
    {
      theInstance->ds_profile_db_epc_support_val = false;
    }
    else
    {
      theInstance->ds_profile_db_epc_support_val = ((epc_support_val)?1:0);
    }
  }
}

bool DBMgr::GetEPCSupportVal
(
 void
)
{
  if (theInstance) 
  {
    return theInstance->ds_profile_db_epc_support_val;
  }
  /*Instance is not defined, return default value as false and log error*/
  return false;
}
