/*
@file drmconfig.c
@brief TZPLAYREADY CONFIG.

*/
/*===========================================================================
   Copyright (c) 2011-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/tzdrm/tzcommon/config/shared/src/tzdrmconfig.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

 when       who     what, where, why
 --------   ---     ---------------------------------------------------------
04/18/14    rk     Renamed tzcommon_entry.h to app_common_svc_handler.h
04/03/14    tp     Modified by running "atyle --style=allman" command.
04/02/14    tp     Removed the use of g_prov_dir_name.
11/07/13    rk     Added flag so that OEMs can enable single time provisioning.
11/06/13    rk     Added play enabler GUID check for video protection
03/22/13    cz     Added new dir for widevine dash
04/04/13    dm     Removed extern declaration for get_internal_test_flag()
04/03/13    dm     Reverted the changes made earler to SFS prefix for internal testing purpose
03/27/13    ck     Added security_stream_checker
02/11/13    cz     Added  a new function get_aes_key_path()
02/08/13    rk     Added new flag to check proprietary opl level
01/29/13    rk     Added new message to set robustness version dynamically
01/21/12    cz     Added an audio protection enable flag
12/14/12    dm     Added changes to make SFS_PREFIX configurable for internal testing
11/16/12    rk     Added configurable path for license store
11/07/12    rk     Exposed g_robustness_ver for other libs
11/06/12    cz     Exposed g_bUse_IV_constraint for other libs
11/06/12    cz     Removed USES_PR_2_0 condition check as TZ 2.0 only uses PR 2.0
07/10/12    rk     Added changes related playready 2.0 integration.
06/15/12    rk     Added support to allow key provisioning only once.
06/07/12    rp     Added key hardcode toggle support for decrypt & domain testcases
04/25/12    rk     Change directory names for key provisioning.
12/21/11    kr     Update for CR#326083.
11/16/11    cz     Initial Version
===========================================================================*/
#include <comdef.h>
#include <string.h>
#include "app_common_svc_handler.h"
#include "qsee_comstr.h"
#include "tzdrmconfig.h"
#include "tzapps_test_config.h"

/* SFS_PREFIX MUST NOT be Changed */
#define SFS_PREFIX  L"usr_sfs:"


/*
**  The follow define is used for drmtest directory
**  Test app isn't shipped to OEMs
*/
#define USER_ETEST_DRMTEST_DIR     "/data/misc/exectest"

/*
**  The follow defines are hardcoded playready path's.
**  Will remove them once we have changed the OEM layer to just use the USER_KEY_ROOT_PATH directly
*/
#define USER_PLAYREADY_DATA_STORE          "app_ms/license.hds"
#define USER_PLAYREADY_CERT_PATH           "app_ms/"
#define USER_PLAYREADY_CERT_TEMPL_PATH     "app_ms/app_ms/"
#define USER_PLAYREADY_PRIV_PATH           "app_ms/app_ms/"
#define USER_PLAYREADY_SFS_PRT_PATH        "app_ms/"

#define FINALIZE_PROV_FLAG                 "finalize_prov_flag.data"

#define AES_KEY_FILE                       "aeskey.dat"


char g_playready_data_store[TZ_CM_MAX_NAME_LEN];
char g_playready_cert_path[TZ_CM_MAX_NAME_LEN];
char g_playready_cert_templ_path[TZ_CM_MAX_NAME_LEN];
char g_playready_priv_path[TZ_CM_MAX_NAME_LEN] ;
char g_playready_sfs_prt_path[TZ_CM_MAX_NAME_LEN];
char g_tz_log_file_name[TZ_CM_MAX_NAME_LEN];
char g_wv_keybox_lv3_file_path[TZ_CM_MAX_NAME_LEN];
char g_wv_keybox_lv1_file_path[TZ_CM_MAX_NAME_LEN];
char g_wv_device_id_file_path[TZ_CM_MAX_NAME_LEN];
char g_wv_keybox_sfs_dir[TZ_CM_MAX_NAME_LEN];
char g_wv_dash_keybox_file_path[TZ_CM_MAX_NAME_LEN];
char g_wv_dash_keybox_sfs_dir[TZ_CM_MAX_NAME_LEN];
char g_drmtest_dir[TZ_CM_MAX_NAME_LEN];
char g_prov_path[TZ_CM_MAX_NAME_LEN];
char g_single_time_prov_flag[TZ_CM_MAX_NAME_LEN];


char g_playready_aes_key_file_path[TZ_CM_MAX_NAME_LEN];


void config_set_pk_path
(
    char * pDataStore,
    char * pCert,
    char * pCertTempl,
    char * pPriv,
    uint32 lenPath
)
{
    char tempBuf[TZ_CM_MAX_NAME_LEN];
    WCHAR tempWcharBuf[TZ_CM_MAX_NAME_LEN];

    memset(pDataStore, 0, lenPath);
    memset(pCert, 0, lenPath);
    memset(pCertTempl, 0, lenPath);
    memset(pPriv, 0, lenPath);

    memset(tempBuf, 0, TZ_CM_MAX_NAME_LEN);
    memset(tempWcharBuf, 0, TZ_CM_MAX_NAME_LEN);
    memcpy(tempBuf, USER_LIC_STORE_PATH, strlen(USER_LIC_STORE_PATH));
    qsee_strlcat(tempBuf + strlen(USER_LIC_STORE_PATH), USER_PLAYREADY_DATA_STORE, TZ_CM_MAX_NAME_LEN);
    qsee_strtowstr(tempBuf, tempWcharBuf, (strlen(tempBuf)+1)*2);
    memcpy(pDataStore, tempWcharBuf, qsee_wstrlen(tempWcharBuf)*2);

    memset(tempBuf, 0, TZ_CM_MAX_NAME_LEN);
    memset(tempWcharBuf, 0, TZ_CM_MAX_NAME_LEN);
    memcpy(tempBuf, USER_KEY_ROOT_PATH, strlen(USER_KEY_ROOT_PATH));
    qsee_strlcat(tempBuf + strlen(USER_KEY_ROOT_PATH), USER_PLAYREADY_CERT_PATH, TZ_CM_MAX_NAME_LEN);
    qsee_strtowstr(tempBuf, tempWcharBuf, (strlen(tempBuf)+1)*2);
    memcpy(pCert, tempWcharBuf, qsee_wstrlen(tempWcharBuf)*2);

    memset(tempBuf, 0, TZ_CM_MAX_NAME_LEN);
    memset(tempWcharBuf, 0, TZ_CM_MAX_NAME_LEN);
    memcpy(tempBuf, USER_KEY_ROOT_PATH, strlen(USER_KEY_ROOT_PATH));
    qsee_strlcat(tempBuf + strlen(USER_KEY_ROOT_PATH), USER_PLAYREADY_CERT_TEMPL_PATH, TZ_CM_MAX_NAME_LEN);
    qsee_strtowstr(tempBuf, tempWcharBuf, (strlen(tempBuf)+1)*2);
    memcpy(pCertTempl,SFS_PREFIX, qsee_wstrlen(SFS_PREFIX)*2);
    memcpy(pCertTempl+qsee_wstrlen(SFS_PREFIX)*2, tempWcharBuf, qsee_wstrlen(tempWcharBuf)*2);

    memset(tempBuf, 0, TZ_CM_MAX_NAME_LEN);
    memset(tempWcharBuf, 0, TZ_CM_MAX_NAME_LEN);
    memcpy(tempBuf, USER_KEY_ROOT_PATH, strlen(USER_KEY_ROOT_PATH));
    qsee_strlcat(tempBuf + strlen(USER_KEY_ROOT_PATH), USER_PLAYREADY_PRIV_PATH, TZ_CM_MAX_NAME_LEN);
    qsee_strtowstr(tempBuf, tempWcharBuf, (strlen(tempBuf)+1)*2);
    memcpy(pPriv,SFS_PREFIX, qsee_wstrlen(SFS_PREFIX)*2);
    memcpy(pPriv+qsee_wstrlen(SFS_PREFIX)*2, tempWcharBuf, qsee_wstrlen(tempWcharBuf)*2);

    return;
}

void config_set_prov_path
(
    char * pProvPath,
    uint32 lenPath
)
{
    memset(pProvPath, 0, lenPath);
    memcpy(pProvPath, USER_KEY_ROOT_PATH, strlen(USER_KEY_ROOT_PATH));
    pProvPath[strlen(USER_KEY_ROOT_PATH)] = '\0';
    memset(g_playready_sfs_prt_path, 0, lenPath);
    memcpy(g_playready_sfs_prt_path, USER_PLAYREADY_SFS_PRT_PATH, strlen(USER_PLAYREADY_SFS_PRT_PATH));
    g_playready_sfs_prt_path[strlen(USER_PLAYREADY_SFS_PRT_PATH)] = '\0';
}

void config_set_log_path
(
    char * pLogPath,
    uint32 lenPath
)
{
    memset(pLogPath, 0, lenPath);
    memcpy(pLogPath, USER_TZ_LOG_FILE_NAME, strlen(USER_TZ_LOG_FILE_NAME));
    pLogPath[strlen(USER_TZ_LOG_FILE_NAME)] = '\0';

    return;
}

void config_set_widevine_path
(
    char * pWvKeyboxFilePath,
    char * pWvKeyboxFwFilePath,
    char * pWvDeviceIdFilePath,
    char * pWvKeyboxSfsDir,
    uint32 lenPath
)
{
    memset(pWvKeyboxFilePath, 0, lenPath);
    memcpy(pWvKeyboxFilePath, USER_WIDEVINE_KEYBOX_LVL3_FILE_PATH, strlen(USER_WIDEVINE_KEYBOX_LVL3_FILE_PATH));
    pWvKeyboxFilePath[strlen(USER_WIDEVINE_KEYBOX_LVL3_FILE_PATH)] = '\0';

    memset(pWvKeyboxFwFilePath, 0, lenPath);
    memcpy(pWvKeyboxFwFilePath, USER_WIDEVINE_KEYBOX_LVL1_FILE_PATH, strlen(USER_WIDEVINE_KEYBOX_LVL1_FILE_PATH));
    pWvKeyboxFwFilePath[strlen(USER_WIDEVINE_KEYBOX_LVL1_FILE_PATH)] = '\0';

    memset(pWvDeviceIdFilePath, 0, lenPath);
    memcpy(pWvDeviceIdFilePath, USER_WIDEVINE_DEVICE_ID_FILE_PATH, strlen(USER_WIDEVINE_DEVICE_ID_FILE_PATH));
    pWvDeviceIdFilePath[strlen(USER_WIDEVINE_DEVICE_ID_FILE_PATH)] = '\0';

    memset(pWvKeyboxSfsDir, 0, lenPath);
    memcpy(pWvKeyboxSfsDir, USER_WIDEVINE_KEYBOX_SFS_DIR, strlen(USER_WIDEVINE_KEYBOX_SFS_DIR));
    pWvKeyboxSfsDir[strlen(USER_WIDEVINE_KEYBOX_SFS_DIR)] = '\0';

    return;
}


void config_set_widevine_dash_path
(
    char * pWvKeyboxFilePath,
    char * pWvKeyboxSfsDir,
    uint32 lenPath
)
{
    memset(pWvKeyboxFilePath, 0, lenPath);
    memcpy(pWvKeyboxFilePath, USER_WIDEVINE_DASH_KEYBOX_PATH, strlen(USER_WIDEVINE_DASH_KEYBOX_PATH));
    pWvKeyboxFilePath[strlen(USER_WIDEVINE_DASH_KEYBOX_PATH)] = '\0';

    memset(pWvKeyboxSfsDir, 0, lenPath);
    memcpy(pWvKeyboxSfsDir, USER_WIDEVINE_DASH_KEYBOX_SFS_DIR, strlen(USER_WIDEVINE_DASH_KEYBOX_SFS_DIR));
    pWvKeyboxSfsDir[strlen(USER_WIDEVINE_DASH_KEYBOX_SFS_DIR)] = '\0';

    return;
}


void config_set_test_path
(
    char * pDrmtestDir,
    uint32 lenPath
)
{
    memset(g_drmtest_dir, 0, lenPath);
    memcpy(g_drmtest_dir, USER_ETEST_DRMTEST_DIR, strlen(USER_ETEST_DRMTEST_DIR));
    g_drmtest_dir[strlen(USER_ETEST_DRMTEST_DIR)] = '\0';

    return;
}


void config_set_aes_key_path
(
    char   *pAes_key_file,
    uint32 lenPath
)
{
    char tmpAesPath[TZ_CM_MAX_NAME_LEN] = {0};

    qsee_wstrtostr((const WCHAR *)g_playready_priv_path, tmpAesPath, TZ_CM_MAX_NAME_LEN);
    memset(pAes_key_file, 0, lenPath);
    memcpy(pAes_key_file, tmpAesPath, strlen(tmpAesPath));
    memcpy(pAes_key_file + strlen(tmpAesPath), AES_KEY_FILE, strlen(AES_KEY_FILE));

}


void config_set_prov_finalize_flag_path(char *psingle_time_prov_flag, uint32 len)
{
    memset(psingle_time_prov_flag, 0, len);
    memcpy(psingle_time_prov_flag, USER_KEY_ROOT_PATH, strlen(USER_KEY_ROOT_PATH));
    memcpy(psingle_time_prov_flag+strlen(USER_KEY_ROOT_PATH), FINALIZE_PROV_FLAG, strlen(FINALIZE_PROV_FLAG));
}

void init_tzdrm_config_path(void)
{
    config_set_pk_path(g_playready_data_store,
                       g_playready_cert_path,
                       g_playready_cert_templ_path,
                       g_playready_priv_path,
                       TZ_CM_MAX_NAME_LEN);

    config_set_prov_path(g_prov_path,
                         TZ_CM_MAX_NAME_LEN);

    config_set_log_path(g_tz_log_file_name,
                        TZ_CM_MAX_NAME_LEN);


    config_set_aes_key_path(g_playready_aes_key_file_path,
                            TZ_CM_MAX_NAME_LEN);

    config_set_widevine_path(g_wv_keybox_lv3_file_path,
                             g_wv_keybox_lv1_file_path,
                             g_wv_device_id_file_path,
                             g_wv_keybox_sfs_dir,
                             TZ_CM_MAX_NAME_LEN);

    config_set_widevine_dash_path(g_wv_dash_keybox_file_path,
                                  g_wv_dash_keybox_sfs_dir,
                                  TZ_CM_MAX_NAME_LEN);

    config_set_prov_finalize_flag_path(g_single_time_prov_flag, TZ_CM_MAX_NAME_LEN);

    config_set_test_path(g_drmtest_dir, TZ_CM_MAX_NAME_LEN);


    return;
}

boolean get_IV_constraint_flag()
{
    return g_bUse_IV_constraint;

}

boolean get_proprietary_play_enabler_video_protect()
{
    return g_bUse_proprietary_play_enabler_video_protect;
}

#ifdef OEM_VIDEO_PROTECT_TEMP
boolean get_proprietary_play_enabler_default_behaviour()
{
    return g_bUse_proprietary_play_enabler_default_subsample;
}
#endif

uint32 get_robustness_ver()
{
    return g_robustness_ver;
}

void set_robustness_ver(uint32 version)
{
    g_robustness_ver = version;
}

int32 get_aes_key_path(uint8* dst, uint32 len)
{
    int32 ret = 0;

    do
    {
        if (len < TZ_CM_MAX_NAME_LEN)
        {
            ret = -1;
            break;
        }
        memcpy(dst, g_playready_aes_key_file_path, TZ_CM_MAX_NAME_LEN);
    }
    while(0);

    return ret;
}

boolean get_audio_CP_enable_flag()
{
    return g_audio_protection_enable;
}

boolean get_proprietary_play_enabler_profile()
{
    return g_bUse_proprietary_play_enabler_profile;
}

boolean get_proprietary_opl_profile_0()
{
    return g_bUse_proprietary_opl_profile_0;
}

uint32 oem_disable_security_stream_check(uint32 application)
{
    // Do not change this code without consulting with Qualcomm.
    if(application == 1)
        return 0x00000003;
    else
        return 0;
}

// Big endian for oem_mask and oem_pattern.
int32 oem_get_IV_constraint(
    uint32    application,
    uint32    crypto_mode,
    uint8     *oem_mask,
    uint8     *oem_pattern,
    int32     *allow)
{
    // Do not change this code without consulting with Qualcomm.
    if((application == 1) && (crypto_mode == 1))
    {
        if(!oem_mask || !oem_pattern || !allow)
            return -1;
        oem_mask[0] = 0xC0;
        oem_pattern[0] = 0x40;
        *allow = 1;
        return 1;
    }
    else
        return 0;
}

boolean get_prov_constraint_flag()
{
    return g_bUse_prov_constraint;
}
