#ifndef __TZDRMCONFIG_H_
#define __TZDRMCONFIG_H_
/*===========================================================================
  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/tzdrm/tzcommon/config/shared/inc/tzdrmconfig.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14    tp     Modified by running "atyle --style=allman" command.
11/07/13    rk     Added flag so that OEMs can enable single time provisioning.
11/06/13    rk     Added play enabler GUID check for video protection
06/29/13    cz     Added new dir for widevine dash
02/08/13    rk     Added new flag to check proprietary opl level.
01/21/12    cz     Added an audio protection enable flag
01/17/13    cz     Set the default g_bUse_IV_constraint to 0
12/14/12    dm     Modified the data store path for internal testing
11/23/12    rk     Changed default robustness version value to zero
11/16/12    rk     Added configurable path for license store
11/07/12    rk     Added a flag which denotes robustness version.
11/06/12    cz     Changed release description for external use
11/06/12    cz     Added one config feature flag
06/01/12    rk     Added support to allow key provisioning only once.
04/25/12    rk     Change directory names for key provisioning.
12/21/11    kr     Update for CR#326083.
10/20/11    cz     Initial Version.
===========================================================================*/
/*
** OEMs can change all following Macro defines. the sub-directory name under /<partition>/<anyname>/app_ms
** or /<partition>/<any name>/app_g shoud not be changed.
** for example USER_PLAYREADY_DATA_STORE by default is located at "/persist/data/app_ms/license.hds"
** OEMs can chanage it to "/data/app_ms/license.hds"
** OEMs can NOT change it to "/data/myplayready/license.hds"
** or "/data/myplayready/mydrmms/license.hds"
** The length of path should not be larger than 160
*/

/*
** This macro defines the root path for the keys provisioned for playready
** and generic key provisioning only.
*/
#define USER_KEY_ROOT_PATH                    "/persist/data/"

/*
** This macro defines the root path for the license.hds which gets generated
** with playready.  OEMs can chanage it to "/data/" so that license.hds is
** stored at /data/license.hds.
** Please note that this macro is applicable only for playready.
*/
#define USER_LIC_STORE_PATH                   "/data/data/"



/*
**  The follow defines are for widevine
*/
#define USER_WIDEVINE_KEYBOX_LVL3_FILE_PATH   "/persist/data/app_g/sfs/keybox_lvl3.dat"
#define USER_WIDEVINE_KEYBOX_LVL1_FILE_PATH   "/persist/data/app_g/sfs/keybox_lvl1.dat"
#define USER_WIDEVINE_DEVICE_ID_FILE_PATH     "/persist/data/app_g/sfs/device_id.dat"
#define USER_WIDEVINE_KEYBOX_SFS_DIR          "/persist/data/app_g/sfs"

/*
**  The follow defines are for widevine DASH
*/
#define USER_WIDEVINE_DASH_KEYBOX_PATH        "/persist/data/app_g/sfs/keybox_lvl1.dat"
#define USER_WIDEVINE_DASH_KEYBOX_SFS_DIR     "/persist/data/app_g/sfs"

/*
**  The follow defines are for log
*/
#define USER_TZ_LOG_FILE_NAME                 "/data/misc/playready/logs/tzdrm.log"


/*
**  customers need to config g_bUse_IV_constraint
**  if g_bUse_IV_constraint = 1, audio IV must begin with 0b01
**                               video IV must begin with 0b00
**  if g_bUse_IV_constraint = 0, there is no such a constraint
*/
static boolean g_bUse_IV_constraint = 0;

/*
**  customers need to config g_bUse_proprietary_play_enabler_video_protect
**  if g_bUse_proprietary_play_enabler_video_protect = 1, g_bUse_proprietary_play_enabler_default_subsample = 1
                audio stream must match customer specific requirements and default behaviour is "CHECK_SUBSAMPLE"
                This is the case where server side development is completely up and running.
**
**  if g_bUse_proprietary_play_enabler_video_protect = 1, g_bUse_proprietary_play_enabler_default_subsample = 0
                audio stream must match customer specific requirements and default behaviour "CHECK_NONE"
                This is the case where server side development is in intermediate stage where there can be a GUID in license
**
**  if g_bUse_proprietary_play_enabler_video_protect = 0, g_bUse_proprietary_play_enabler_default_subsample = 1
                server side development is not ready (no GUIDs in license) and default behaviour "CHECK_SUBSAMPLE"
**
**  if g_bUse_proprietary_play_enabler_video_protect = 0, g_bUse_proprietary_play_enabler_default_subsample = 0
                server side development is not ready (no GUIDs in license) and default behaviour "CHECK_NONE"
**
** NOTE: g_bUse_IV_constraint and below flags SHOULD NOT be used together.
*/
static boolean g_bUse_proprietary_play_enabler_video_protect = 0;
#ifdef OEM_VIDEO_PROTECT_TEMP
static boolean g_bUse_proprietary_play_enabler_default_subsample = 0;
#endif

/*
**  This is the robustness version value used by porting kit while generating
**  device certificates.  This value should be incremented by OEMs as part of
**  reprovisioning the device whenever there is a revocation.  If there is a
**  mismatch between this value and the robustness version used in model
**  certificates, then initialize will fail.  Hence, this value should always be
**  same as that of the value used in model certificates.
*/
static uint32 g_robustness_ver = 0;

/* This flag to control whether audio output buffer needs to be protected
** 1 is to enable audio content protection
** 0 is to disable audio content protection
*/
static boolean g_audio_protection_enable = 0;

/*
** OEMs need to set this flag to zero to avoid check for existence of
** play enabler object in license.
** set g_bUse_proprietary_play_enabler_profile = 1, to check for existence of
** play enabler object in license.
** Note: Blocking or unblocking HDCP display is based on how this flag is
**       used with g_bUse_proprietary_opl_profile_0.  Valid combinations
**       are explained in the comment section for g_bUse_proprietary_opl_profile_0.
**
** set g_bUse_proprietary_play_enabler_profile = 0, to ignore check for
** existence of play enabler object in license.
**
*/
static boolean g_bUse_proprietary_play_enabler_profile = 0;

/*
** OEMs need to define this flag to enable check for OPL >= 270.
**
** Please note that if this flag has to be enabled then g_bUse_proprietary_play_enabler_profile
** should also be enabled.  Below are the valid/invalid combinations.
**
** (1) g_bUse_proprietary_play_enabler_profile = 0, g_bUse_proprietary_opl_profile_0 = 0 (valid)
**     HDCP display is controlled with regular OPL>=301 check
** (2) g_bUse_proprietary_play_enabler_profile = 1, g_bUse_proprietary_opl_profile_0 = 0 (valid)
**     HDCP display is blocked if play enabler object is not found in license.
** (3) g_bUse_proprietary_play_enabler_profile = 0, g_bUse_proprietary_opl_profile_0 = 1 (Invalid)
**     Invalid combination.  Result is same as case (1)
** (4) g_bUse_proprietary_play_enabler_profile = 1, g_bUse_proprietary_opl_profile_0 = 1 (valid)
**     HDCP display is blocked if OPL>=270.
**
** set g_bUse_proprietary_opl_profile_0 = 1, to check for OPL >= 270 in license.
** Note: If this flag is enabled (set to one) then OPL >=270 check will act as fallback
**       check in case play enabler object for HDCP display is not found in license.
**       i.e., HDCP WFD gets blocked if play enabler object is not found and OPL >= 270.
**
** set g_bUse_proprietary_opl_profile_0 = 0, to ignore OPL 270 check
** Note: If this flag is disabled (set to zero) then HDCP display gets blocked if play
**       enabler object for HDCP display is not found in license.
*/
static boolean g_bUse_proprietary_opl_profile_0 = 0;

/*
**  customers need to config g_bUse_prov_constraint to have single time provisioning
**  feature with drm_prov_finalize().
**  if g_bUse_prov_constraint = 1, key provisioning can be done only once after calling drm_prov_finalize().
**  if g_bUse_prov_constraint = 0, there is no such constraint
*/
static boolean g_bUse_prov_constraint = 0;


#endif /* __TZDRMCONFIG_H_ */
