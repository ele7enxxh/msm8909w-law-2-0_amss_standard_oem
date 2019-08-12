/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/common/src/uim_common_efs.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
08/18/16   vdc     Added support for simlock single lock
08/02/16   gm      Support for CDMALess Device
07/20/16   shr     Support for NV based Toolkit disablement
07/04/16   ar      EONS selective logging
05/23/16   dd      Enhancing the voltage supply indication to QMI feature
04/11/16   av      Introduce LPA support
01/25/16   ssr     Delay local card power up in Modem SSR in BTSAP mode
12/25/15   bcho    Send Card and session events based on SFS data in PSM mode
11/05/15   ks      Rel12 polling enhancement
09/25/15   vr      Added support for handling multiple profiles enabled cards
09/23/15   bcho    Use MCFG Context enum instead of typecast
09/08/15   ll      UIMDRV migrates to UIM COMMON EFS APIs for all EFS items
09/07/15   bcho    Migrate to mcfg EFS wrappers
08/05/15   nr      Operator specifc NV ECC numbers configuration
07/03/15   tkl     Added support for custom fields
04/30/15   vdc     Added support for APN for bootstrapping
04/30/15   lxu     Use gba_url and gba_port in nv if they are valid
04/30/15   vdc     Added support for GBA
04/30/15   nr      Enhancements in handling of GCF anite flag update
04/27/15   vr      Non UI and non NW proactive command parallel processing
12/19/14   gm      Added support to read envelope retry data from NV
12/10/14   vr      Disable CC env during SEND/RECEIVE DATA based on NV config
11/19/14   av      Fix logic of setting ANITE_NV when card contains CSIM app
11/10/14   akv     Support for user to customize LDO settings
10/31/14   gm      Update qmi_cat_raw_unsupported_features in conf file
10/14/14   tl      Fix compiler warnings
10/14/14   am      Compile warnings elimination
10/13/14   hh      Add support to get Voice Domain Preference
10/12/14   hh      Add support to get Voice Domain Preference
09/09/14   kk      Enhancements to NV handling
09/02/14   yt      Fix expected return value from efs_put()/efs_write()
08/11/14   bd      Log efs_errno for efs_get and efs_put failure
07/25/14   av      Auto onchip activation for multi slots
07/15/14   yt      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "uim_common_efs.h"
#include "uim_msg.h"
#include "fs_public.h"
#include "fs_lib.h"
#include "fs_diag_access.h"
#include <stringl/stringl.h>
#include "modem_mem.h"
#include "mmgsdilib_common.h"
#include "uim_v.h"
#include "uim_p.h"
#include "qmi_uim.h"
#include "qmi_cat.h"
#include "mmgsdilib.h"
#include "mcfg_fs.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* Maximum number of contexts currently supported in EFS */
#define UIM_COMMON_EFS_MAX_NUM_CONTEXTS  3

/*=============================================================================

                          UIM COMMON FUNCTIONS
                     Definitions used throughout UIM

=============================================================================*/

/*=============================================================================
FUNCTION:      UIM_COMMON_CONVERT_TO_MCFG_CONTEXT

DESCRIPTION:
  This function will convert uim common context type to mcfg context type.

PARAMETERS
  uim_common_context: UIM COMMON context type, passed in by the callers to API

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  mcfg_fs_sub_id_e_type - MCFG CONTEXT Enum

SIDE EFFECTS:
  None
=============================================================================*/
mcfg_fs_sub_id_e_type uim_common_convert_to_mcfg_context(
  uim_common_efs_context_type  uim_common_context
)
{
  switch (uim_common_context)
  {
    case UIM_COMMON_EFS_DEVICE:
      return MCFG_FS_SUBID_NONE;
    case UIM_COMMON_EFS_CONTEXT_0:
      return MCFG_FS_SUBID_0;
    case UIM_COMMON_EFS_CONTEXT_1:
      return MCFG_FS_SUBID_1;
    case UIM_COMMON_EFS_CONTEXT_2:
      return MCFG_FS_SUBID_2;
    case UIM_COMMON_EFS_CONTEXT_MAX:
    default:
      break;
  }

  return MCFG_FS_SUBID_BIG;
} /* uim_common_get_mcfg_context */


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_GET_PATH_FROM_ENUM

DESCRIPTION:
  This function will return the path of an EFS file based on the enum passed.

PARAMETERS
  path_buf:        Buffer to hold the path
  path_buf_len:    Size of the path buffer
  efs_item:        The enum corresponding to the EFS item/file to be read

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
static uim_common_efs_status_enum_type uim_common_efs_get_path_from_enum(
  char                         * path_buf,
  uint32                         path_buf_len,
  uim_common_efs_item_type       efs_item
)
{
  char    * efs_path           = NULL;
  size_t    efs_path_len       = 0;
  size_t    copied_size        = 0;

  if(path_buf == NULL || path_buf_len == 0)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  switch(efs_item)
  {
    case UIM_COMMON_EFS_MMGSDI_REFRESH_RETRY:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_REFRESH_RETRY;
      break;
    case UIM_COMMON_EFS_MMGSDI_REFRESH_VOTE_OK:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_REFRESH_VOTE_OK;
      break;
    case UIM_COMMON_EFS_MMGSDI_FEATURE_STATUS_LIST:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_FEATURE_STATUS_LIST;
      break;
    case UIM_COMMON_EFS_MMGSDI_ENS_SLOT_ENABLED:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_ENS_SLOT_ENABLED;
      break;
    case UIM_COMMON_EFS_MMGSDI_MULTI_CONTEXT_CONF:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_MULTI_CONTEXT_CONF;
      break;
    case UIM_COMMON_EFS_MMGSDI_ONCHIP_SUPPORT:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_SUPPORT;
      break;
    case UIM_COMMON_EFS_MMGSDI_DIAG_SUPPORT:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_DIAG_SUPPORT;
      break;
    case UIM_COMMON_EFS_MMGSDI_HALT_SUBSCRIPTION:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_HALT_SUB;
      break;
    case UIM_COMMON_EFS_MMGSDI_ENCRYPT_SUB_OK:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_ENCRYPT_SUB;
      break;
    case UIM_COMMON_EFS_MMGSDI_CSIM_DEVICE_MODEL:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_CSIM_DEVICE_MODEL;
      break;
    case UIM_COMMON_EFS_MMGSDI_ONCHIP_CONFIG_SLOT1:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT1;
      break;
    case UIM_COMMON_EFS_MMGSDI_ONCHIP_CONFIG_SLOT2:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT2;
      break;
    case UIM_COMMON_EFS_MMGSDI_ONCHIP_CONFIG_SLOT3:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT3;
      break;
    case UIM_COMMON_EFS_MMGSDI_ENS_SUB_FEATURES_STATUS_LIST:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_ENS_SUB_FEATURES_STATUS_LIST_FILE;
      break;
    case UIM_COMMON_EFS_MMGSDI_BUILT_IN_PLMN_LIST:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_BUILTIN_PLMN_LIST_FILE;
      break;
    case UIM_COMMON_EFS_MMGSDI_AUTO_ACTIVATE_MULTI_SLOTS_FILE:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_AUTO_ACTIVATE_MULTI_SLOTS_FILE;
      break;
    case UIM_COMMON_EFS_MMGSDI_GCF_GSTK_FILE:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_GCF_GSTK_FILE;
      break;
     case UIM_COMMON_EFS_MMGSDI_MULTI_PROFILE_FILE:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_MULTI_PROFILE_FILE;
      break;
    case UIM_COMMON_EFS_MMGSDI_GPRS_GCF_ANITE_CONTROL:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_GPRS_GCF_ANITE_CONTROL;
      break;
    case UIM_COMMON_EFS_MMGSDI_SELECTIVE_LOGGING:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_SELECTIVE_LOGGING;
      break;
    case UIM_COMMON_EFS_GSTK_ME_TP_CLIENTS:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_ME_TP_CLIENTS;
      break;
    case UIM_COMMON_EFS_GSTK_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA;
      break;
    case UIM_COMMON_EFS_GSTK_ME_PROACTIVE_FEATURE_ENABLE_CFG:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_ME_PROACTIVE_FEATURE_ENABLE_CFG;
      break;
    case UIM_COMMON_EFS_GSTK_ME_TAL_CONF:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_ME_TAL_CONF;
      break;
    case UIM_COMMON_EFS_GSTK_ME_REDIAL_CFG:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_ME_REDIAL_CFG;
      break;
    case UIM_COMMON_EFS_GSTK_ME_QMI_CAT_SVC_INIT:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_ME_QMI_CAT_SVC_INIT;
      break;
    case UIM_COMMON_EFS_GSTK_ME_FEATURE_BMASK:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_ME_FEATURE_BMASK;
      break;
    case UIM_COMMON_EFS_GSTK_SLOT_TOOLKIT_DISABLED:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SLOT_TOOLKIT_DISABLED;
      break;
    case UIM_COMMON_EFS_GSTK_SLOT_CAT_VERSION:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SLOT_CAT_VERSION;
      break;
    case UIM_COMMON_EFS_GSTK_SLOT_RAT_PRIORITY_CONFIG:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SLOT_RAT_PRIORITY_CONFIG;
      break;
    case UIM_COMMON_EFS_GSTK_SLOT_BIP_APN_MAPPING_TABLE:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SLOT_BIP_APN_MAPPING_TABLE;
      break;
    case UIM_COMMON_EFS_GSTK_SLOT_DEFAULT_APN:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SLOT_DEFAULT_APN;
      break;
    case UIM_COMMON_EFS_GSTK_SLOT_FEATURE_BMASK:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SLOT_FEATURE_BMASK;
      break;
    case UIM_COMMON_EFS_GSTK_SLOT_FEATURE_BMASK_2:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SLOT_FEATURE_BMASK_2;
      break;
    case UIM_COMMON_EFS_GSTK_SUB_BIP_OTASP_CONCURRENCY:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SUB_BIP_OTASP_CONCURRENCY;
      break;
    case UIM_COMMON_EFS_GSTK_SUB_ATTACH_CTRL_TIMER:
      efs_path = UIM_COMMON_EFS_PATH_GSTK_SUB_ATTACH_CTRL_TIMER;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_CAT_MODE:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_MODE;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_CAT_DEFAULT_LANG:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_DEFAULT_LANG;
      break;
    case UIM_COMMON_EFS_NON_UIM_CSG_SUPPORT_CONFIG:
      efs_path = UIM_COMMON_EFS_PATH_NON_UIM_CSG_SUPPORT_CONFIG;
      break;
    case UIM_COMMON_EFS_NON_UIM_IMS_ENABLE:
      efs_path = UIM_COMMON_EFS_PATH_NON_UIM_IMS_ENABLE;
      break;
    case UIM_COMMON_EFS_NON_UIM_RPM_INFO:
      efs_path = UIM_COMMON_EFS_PATH_NON_UIM_RPM_INFO;
      break;
    case UIM_COMMON_EFS_UIMDRV_CONF:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_CONF;
      break;
    case UIM_COMMON_EFS_UIMDRV_JCDMA_MODE:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_JCDMA_MODE;
      break;
    case UIM_COMMON_EFS_NON_UIM_VOICE_DOMAIN_PREF:
      efs_path = UIM_COMMON_EFS_PATH_NON_UIM_VOICE_DOMAIN_PREF;
      break;
    case UIM_COMMON_EFS_NON_UIM_OPERATOR_NAME:
      efs_path = UIM_COMMON_EFS_PATH_NON_UIM_OPERATOR_NAME;
      break;
    case UIM_COMMON_EFS_UIMDRV_PDOWN_NV_EF:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_PDOWN_NV;
      break;
    case UIM_COMMON_EFS_UIMDRV_LDO_SETTINGS:
      efs_path = UIM_COMMON_EFS_PATH_HW_LDO_CONFIG;
      break;
    case UIM_COMMON_EFS_UIMDRV_FEATURES_STATUS_LIST:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_FEATURES_STATUS_LIST;
      break;
    case UIM_COMMON_EFS_UIMDRV_HOTSWAP_DEBOUNCE_CONFIG:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_HOTSWAP_DEBOUNCE_CONFIG;
      break;
    case UIM_COMMON_EFS_UIMDRV_ACTIVE_SLOT:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_ACTIVE_SLOT;
      break;
    case UIM_COMMON_EFS_UIMDRV_BUSY_RESPONSE :
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_BUSY_RESPONSE;
      break;
    case UIM_COMMON_EFS_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_BUSY_RESPONSE_SIMULATE_NULL;
      break;
    case UIM_COMMON_EFS_UIMDRV_REMOTE_COMMAND_RESP_TIMER:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_REMOTE_COMMAND_RESP_TIMER;
      break;
    case UIM_COMMON_EFS_UIMDRV_SUPPLY_VOLTAGE_IND_ACK_DELAY_TIMER:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_SUPPLY_VOLTAGE_IND_ACK_DELAY_TIMER;
      break;
    case UIM_COMMON_EFS_UIMDRV_FEATURE_SUPPORT_HOTSWAP:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_FEATURE_SUPPORT_HOTSWAP;
      break;
    case UIM_COMMON_EFS_UIMDRV_HW_CONFIG:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_HW_CONFIG;
      break;
    case UIM_COMMON_EFS_UIMDRV_DISABLE_POLLING:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_DISABLE_POLLING;
      break;
    case UIM_COMMON_EFS_UIMDRV_EXTENDED_RECOVERY_FILE:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_EXTENDED_RECOVERY_FILE;
      break;
    case UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE:
      efs_path = UIM_COMMON_EFS_PATH_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE;
      break;
    case UIM_COMMON_EFS_LAST_MODE_AND_CONFIG_DELAY_FOR_REMOTE_CONNECT:
      efs_path = UIM_COMMON_EFS_PATH_LAST_MODE_AND_CONFIG_DELAY_FOR_REMOTE_CONNECT;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_TP:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_CONFIG_TP;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_CAT_DISPLAY_ALPHA_FILE:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_DISPLAY_ALPHA_FILE;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_CAT_BLOCK_SMS_PP_ENV:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_BLOCK_SMS_PP_ENV;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_CAT_RAW_UNSUPPORTED_FEATURES:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_CAT_RAW_UNSUPPORTED_FEATURES;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SAP_SEC_RESTR:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_SAP_SEC_RESTR;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_AUTH_SEC_RESTR:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_AUTH_SEC_RESTR;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_RESTR:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_RESTR;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SILENT_PIN1_FILE:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_SILENT_PIN1_FILE;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_AID_LIST:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_AID_LIST;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_CLOSE_CHANNEL_SYNC:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_CLOSE_CHANNEL_SYNC;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SILENT_RECOVERY:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_CONFIG_SILENT_RECOVERY;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_GBA_FQDN_WHITE_LIST:
      efs_path = QMI_UIM_CONFIG_GBA_FQDN_WHITE_LIST;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_GBA_FQDN_BLACK_LIST:
      efs_path = QMI_UIM_CONFIG_GBA_FQDN_BLACK_LIST;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_SIMLOCK_STATUS_WAITING_FOR_VALID_CARD:
      efs_path = UIM_COMMON_EFS_PATH_UIMQMI_QMI_UIM_SIMLOCK_STATUS_WAITING_FOR_VALID_CARD;
      break;
    case UIM_COMMON_EFS_GBA_BOOTSTRAP_BSF_ADDRESS:
      efs_path = UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_BSF_ADDRESS;
      break;
    case UIM_COMMON_EFS_GBA_BOOTSTRAP_APN:
      efs_path = UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_APN;
      break;
    case UIM_COMMON_EFS_GBA_BOOTSTRAP_CUSTOM_REALM:
      efs_path = UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_CUSTOM_REALM;
      break;
    case UIM_COMMON_EFS_GBA_BOOTSTRAP_CUSTOM_USER_AGENT_TMPI:
      efs_path = UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_CUSTOM_USER_AGENT_TMPI;
      break;
    case UIM_COMMON_EFS_PBM_NV_ECC_LIST:
      efs_path = UIM_COMMON_EFS_PATH_PBM_NV_ECC_LIST;
      break;
    case UIM_COMMON_EFS_PBM_NV_HARDCODED_ECC_CONFIG:
      efs_path = UIM_COMMON_EFS_PATH_PBM_NV_HARDCODED_ECC_CONFIG;
      break;
    case UIM_COMMON_EFS_PBM_NV_HARDCODED_ECC_LIST:
      efs_path = UIM_COMMON_EFS_PATH_PBM_NV_HARDCODED_ECC_LIST;
      break;
    case UIM_COMMON_EFS_PBM_NV_ECC_LIST_PER_SUB:
      efs_path = UIM_COMMON_EFS_PATH_PBM_NV_ECC_LIST_PER_SUB;
      break;
    case UIM_COMMON_EFS_PBM_SIM_ECC_AIRPLANE:
      efs_path = UIM_COMMON_EFS_PATH_PBM_SIM_ECC_AIRPLANE;
      break;
    case UIM_COMMON_EFS_PBM_NV_ECC_SOURCE_CONFIG:
      efs_path = UIM_COMMON_EFS_PATH_PBM_NV_ECC_SOURCE_CONFIG;
      break;
    case UIM_COMMON_EFS_PBM_EFS_SUPPORT_DISABLE:
      efs_path = UIM_COMMON_EFS_PATH_PBM_EFS_SUPPORT_DISABLE;
      break;
    case UIM_COMMON_EFS_PBM_FEATURES_LIST:
      efs_path = UIM_COMMON_EFS_PATH_PBM_FEATURES_LIST;
      break;
    case UIM_COMMON_EFS_NON_UIM_LTE_3GPP_RELEASE_VER:
      efs_path = UIM_COMMON_EFS_PATH_NON_UIM_LTE_3GPP_RELEASE_VER;
      break;
    case UIM_COMMON_EFS_GBA_BOOTSTRAP_HTTP_HEADER_IMEI:
      efs_path = UIM_COMMON_EFS_PATH_GBA_BOOTSTRAP_HTTP_HEADER_IMEI;
      break;
    case UIM_COMMON_EFS_PBM_CONF:
      efs_path = UIM_COMMON_EFS_PATH_PBM_CONF;
      break;
    case UIM_COMMON_EFS_MMGSDI_PSM:
      efs_path = UIM_COMMON_EFS_PATH_MMGSDI_PSM;
      break;
    case UIM_COMMON_EFS_NON_UIM_PSM_MODE:
      efs_path = UIM_COMMON_EFS_PATH_NON_UIM_PSM_MODE;
      break;
    case UIM_COMMON_EFS_LPA_SUPPORT:
      efs_path = UIM_COMMON_EFS_PATH_LPA_SUPPORT;
      break;
    case UIM_COMMON_EFS_NON_UIM_CDMA_LESS_DEVICE:
      efs_path = UIM_COMMON_EFS_PATH_NON_UIM_CDMA_LESS_DEVICE;
      break;
    default:
      return UIM_COMMON_EFS_ERROR;
  }

  efs_path_len = strlen(efs_path);
  copied_size = memscpy(path_buf, path_buf_len, efs_path, efs_path_len);
  if(copied_size < efs_path_len)
  {
    UIM_MSG_ERR_2("memscpy truncated from 0x%x to 0x%x",
                  efs_path_len, copied_size);
    return UIM_COMMON_EFS_ERROR;
  }

  return UIM_COMMON_EFS_SUCCESS;
} /* uim_common_efs_get_path_from_enum */


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_GET_PATH_FROM_ENUM_FOR_DIR

DESCRIPTION:
  This function will return the path of an EFS file based on the enum passed.

PARAMETERS
  path_buf:        Buffer to hold the path
  path_buf_len:    Size of the path buffer
  efs_item:        The enum corresponding to the EFS dir to be created

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
static uim_common_efs_status_enum_type uim_common_efs_get_path_from_enum_for_dir(
  char                         * path_buf,
  uint32                         path_buf_len,
  uim_common_dir_item_type       dir_item
)
{
  char    * efs_dir_path       = NULL;
  size_t    efs_path_len       = 0;
  size_t    copied_size        = 0;

  if(path_buf == NULL || path_buf_len == 0)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  switch(dir_item)
  {
    case UIM_COMMON_EFS_MODEM_CONFIG_DIR1:
      efs_dir_path = UIM_COMMON_EFS_PATH_MODEM_CONFIG_DIR1;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_CONFIG_DIR2:
      efs_dir_path = UIM_COMMON_EFS_PATH_MODEM_QMI_CONFIG_DIR2;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_DIR3:
      efs_dir_path = UIM_COMMON_EFS_PATH_MODEM_QMI_CAT_CONFIG_DIR3;
      break;
    case UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_DIR4:
      efs_dir_path = UIM_COMMON_EFS_PATH_MODEM_QMI_UIM_CONFIG_DIR4;
      break;
    case UIM_COMMON_EFS_CONF_DIR1:
      efs_dir_path = UIM_COMMON_EFS_PATH_CONF_DIR1;
      break;
    case UIM_COMMON_EFS_CONF_DIR2:
      efs_dir_path = UIM_COMMON_EFS_PATH_CONF_DIR2;
      break;
    case UIM_COMMON_EFS_CONF_DIR3:
      efs_dir_path = UIM_COMMON_EFS_PATH_CONF_DIR3;
      break;
    default:
      return UIM_COMMON_EFS_ERROR;
  }

  efs_path_len = strlen(efs_dir_path);
  copied_size = memscpy(path_buf, path_buf_len, efs_dir_path, efs_path_len);
  if(copied_size < efs_path_len)
  {
    UIM_MSG_ERR_2("memscpy truncated from 0x%x to 0x%x",
                  efs_path_len, copied_size);
    return UIM_COMMON_EFS_ERROR;
  }
  return UIM_COMMON_EFS_SUCCESS;
}/*uim_common_efs_get_path_from_enum_for_dir*/


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_CREATE_DIR

DESCRIPTION:
  This function will create dir.

PARAMETERS
  efs_item: The enum corresponding to the EFS item/file to be read
  mode:      Permissions for the directory

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_create_dir(
  uim_common_dir_item_type  dir_item,
  uint16                    mode
)
{
  char            path_buf[256];

  memset(path_buf, 0x00, sizeof(path_buf));
  if(uim_common_efs_get_path_from_enum_for_dir(path_buf,
                                       sizeof(path_buf),
                                       dir_item) != UIM_COMMON_EFS_SUCCESS)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  if(mcfg_mkdir(path_buf, mode, MCFG_FS_TYPE_EFS) != 0)
  {
    UIM_MSG_ERR_2("efs_errno: 0x%x item 0x%x", efs_errno, dir_item);
    return UIM_COMMON_EFS_ERROR;
  }

  return UIM_COMMON_EFS_SUCCESS;
}/*uim_common_efs_create_dir*/


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_GET_FILE_SIZE

DESCRIPTION:
  This function will retrieve the size of the EFS file using efs_stat().

PARAMETERS
  efs_item:        The enum corresponding to the EFS item/file to be read
  efs_context:     Context for the EFS item
  file_size_ptr:   Output variable to return file size

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_get_file_size(
  uim_common_efs_item_type       efs_item,
  uim_common_efs_context_type    efs_context,
  uint32                       * file_size_ptr
)
{
  char            path_buf[256];
  struct fs_stat  file_stats;

  if(file_size_ptr == NULL)
  {
    return UIM_COMMON_EFS_ERROR;
  }
  memset(path_buf, 0x00, sizeof(path_buf));
  memset(&file_stats, 0x00, sizeof(file_stats));

  if(uim_common_efs_get_path_from_enum(path_buf,
                                       sizeof(path_buf),
                                       efs_item) != UIM_COMMON_EFS_SUCCESS)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  if(mcfg_fs_stat(path_buf, &file_stats,
     MCFG_FS_TYPE_EFS,
     uim_common_convert_to_mcfg_context(efs_context)) != MCFG_FS_STATUS_OK)
  {
    UIM_MSG_ERR_2("efs_errno: 0x%x item 0x%x", efs_errno, efs_item);
    return UIM_COMMON_EFS_ERROR;
  }

  *file_size_ptr = (uint32)file_stats.st_size;

  return UIM_COMMON_EFS_SUCCESS;
} /* uim_common_efs_get_file_size */


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_GET_DIR_STAT

DESCRIPTION:
  This function will retrieve the stats of the Dir using efs_stat().

PARAMETERS
  dir_item:        The enum corresponding to the DIR to be accessed
  file_size_ptr:   Output variable to return file size

DEPENDENCIES:
  None

LIMITATIONS:
  None.

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_get_dir_stat(
  uim_common_dir_item_type       dir_item,
  uint32                       * file_size_ptr
)
{
  char             path_buf[256];
  struct fs_stat   file_stats;

  if(file_size_ptr == NULL)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  memset(&file_stats, 0x00, sizeof(file_stats));

  memset(path_buf, 0x00, sizeof(path_buf));
  if(uim_common_efs_get_path_from_enum_for_dir(path_buf,
                                       sizeof(path_buf),
                                       dir_item) != UIM_COMMON_EFS_SUCCESS)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  if(mcfg_fs_stat(path_buf,
                 &file_stats,
                  MCFG_FS_TYPE_EFS,
                  MCFG_FS_SUBID_NONE ) != MCFG_FS_STATUS_OK)
  {
    UIM_MSG_ERR_2("efs_errno: 0x%x dir item 0x%x", efs_errno, dir_item);
    return UIM_COMMON_EFS_ERROR;
  }

  *file_size_ptr = (uint32)file_stats.st_size;

  return UIM_COMMON_EFS_SUCCESS;
} /* uim_common_efs_get_dir_stat */


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_READ

DESCRIPTION:
  This function will read the EFS file using efs_read() or efs_get()
  depending on the type of file.
  If file type is UIM_EFS_REGULAR_FILE_TYPE, the file is read using efs_read().
  This file type is used to read files which may need to be read partially.
  For UIM_EFS_ITEM_FILE_TYPE file type, the entire file is read at once
  using efs_get(). This file type is used to read the 'special item' files
  of small size (<= FS_ITEM_MAX).

PARAMETERS
  efs_item:        The enum corresponding to the EFS item/file to be read
  file_type:       Type of EFS file to be accessed
  context:         Index of the EFS item for multi-slot settings
  data_ptr:        Buffer to hold the data to be read (to be allocated
                   by the caller)
  data_len:        Size of the buffer

DEPENDENCIES:
  The caller of this API should know the file size and pass the data_len
  parameter accordingly.

LIMITATIONS:
  None

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_read(
  uim_common_efs_item_type       efs_item,
  uim_common_efs_file_type       file_type,
  uim_common_efs_context_type    context,
  uint8                        * data_ptr,
  uint32                         data_len
)
{
  char     path_buf[256];
  int      fd           = 0;

  if(data_ptr == NULL || data_len == 0)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  memset(path_buf, 0x00, sizeof(path_buf));

  if(uim_common_efs_get_path_from_enum(path_buf,
                                       sizeof(path_buf),
                                       efs_item) != UIM_COMMON_EFS_SUCCESS)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  /* Use efs_get() to read item file */
  if(file_type == UIM_COMMON_EFS_ITEM_FILE_TYPE)
  {
    if(mcfg_fs_read(path_buf,
                    (void *)data_ptr,
                    data_len,
                    MCFG_FS_TYPE_EFS,
                    uim_common_convert_to_mcfg_context(context)) == MCFG_FS_STATUS_OK)
    {
      return UIM_COMMON_EFS_SUCCESS;
    }
    else
    {
      UIM_MSG_ERR_2("efs_errno: 0x%x item 0x%x", efs_errno, efs_item);
      return UIM_COMMON_EFS_ERROR;
    }
  }

  /* For regular file, open and read the file using efs_read()*/
  fd = mcfg_fopen(path_buf,
                  MCFG_FS_O_RDONLY,
                  MCFG_FS_ALLPERMS,
                  MCFG_FS_TYPE_EFS,
                  uim_common_convert_to_mcfg_context(context));
  if(fd < 0)
  {
    UIM_MSG_ERR_1("efs_errno: 0x%x", efs_errno);
    return UIM_COMMON_EFS_ERROR;
  }

  if(mcfg_fread(fd, (void *)data_ptr, data_len, MCFG_FS_TYPE_EFS) != data_len)
  {
    UIM_MSG_ERR_1("efs_errno: 0x%x", efs_errno);
    (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
    return UIM_COMMON_EFS_ERROR;
  }

  (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);

  return UIM_COMMON_EFS_SUCCESS;
} /* uim_common_efs_read */


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_WRITE

DESCRIPTION:
  This function will write the data to the EFS using efs_write() or efs_put()
  depending on the type of file.
  If file type is UIM_EFS_REGULAR_FILE_TYPE, the data is written to a regular
  EFS file using efs_write(). These type of files can be updated partially.
  For UIM_EFS_ITEM_FILE_TYPE file type, the entire file is overwritten using
  efs_put(). This file type is used to write to 'special item' files
  of small size (<= FS_ITEM_MAX).

PARAMETERS
  efs_item:        The enum corresponding to the EFS item/file
  file_type:       Type of EFS file to be accessed
  context:         Index of the EFS item for multi-slot settings
  data_ptr:        Buffer with the data to be written
  data_len:        Size of the buffer

DEPENDENCIES:
  The caller of this API should know the file size and pass the data_len
  parameter accordingly.

LIMITATIONS:
  None

RETURN VALUE:
  uim_common_efs_status_enum_type

  UIM_COMMON_EFS_SUCCESS:          The EFS operation was successful.
  UIM_COMMON_EFS_ERROR:            The EFS operation was not successful.

SIDE EFFECTS:
  None
=============================================================================*/
uim_common_efs_status_enum_type uim_common_efs_write(
  uim_common_efs_item_type       efs_item,
  uim_common_efs_file_type       file_type,
  uim_common_efs_context_type    context,
  const char                   * data_ptr,
  uint32                         data_len
)
{
  char     path_buf[256];
  int      fd = 0;

  if(data_ptr == NULL || data_len == 0)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  memset(path_buf, 0x00, sizeof(path_buf));

  if(uim_common_efs_get_path_from_enum(path_buf,
                                       sizeof(path_buf),
                                       efs_item) != UIM_COMMON_EFS_SUCCESS)
  {
    return UIM_COMMON_EFS_ERROR;
  }

  /* Use efs_put() to update an item file */
  if(file_type == UIM_COMMON_EFS_ITEM_FILE_TYPE)
  {
    if(mcfg_fs_write(path_buf, (void *)data_ptr, data_len,
         MCFG_FS_O_CREAT|MCFG_FS_O_RDWR|MCFG_FS_O_AUTODIR|MCFG_FS_O_TRUNC,
         0777, MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context)) == MCFG_FS_STATUS_OK)
    {
      return UIM_COMMON_EFS_SUCCESS;
    }
    else
    {
      UIM_MSG_ERR_1("efs_errno: 0x%x", efs_errno);
      return UIM_COMMON_EFS_ERROR;
    }
  }

  /* For regular file, open and write to the file using efs_write() */
  fd = mcfg_fopen(path_buf, MCFG_FS_O_WRONLY|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC,
         MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, uim_common_convert_to_mcfg_context(context));
  if(fd < 0)
  {
    UIM_MSG_ERR_1("efs_errno: 0x%x", efs_errno);
    return UIM_COMMON_EFS_ERROR;
  }

  if(mcfg_fwrite(fd, (void *)data_ptr, data_len, MCFG_FS_TYPE_EFS) != data_len)
  {
    UIM_MSG_ERR_1("efs_errno: 0x%x", efs_errno);
    (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);
    return UIM_COMMON_EFS_ERROR;
  }

  (void)mcfg_fclose(fd, MCFG_FS_TYPE_EFS);

  return UIM_COMMON_EFS_SUCCESS;
} /* uim_common_efs_write */


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_COMPARE_AND_UPDATE_CONF_FILE

DESCRIPTION:
  This function will create config file for EFS items that are accessed in
  multiple contexts.

PARAMETERS
  None

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
=============================================================================*/
static void uim_common_efs_compare_and_update_conf_file
(
  const char *                buf_ptr,
  uint16                      buf_len
)
{
  uim_common_efs_status_enum_type  status        = UIM_COMMON_EFS_ERROR;
  uint8                           *file_data_ptr = NULL;
  uint32                           file_size     = 0;

  if(buf_ptr == NULL || buf_len == 0)
  {
    return;
  }

  status = uim_common_efs_get_file_size(UIM_COMMON_EFS_MMGSDI_MULTI_CONTEXT_CONF,
                                        UIM_COMMON_EFS_DEVICE,
                                        &file_size);
  if(file_size == buf_len)
  {
    file_data_ptr = (uint8 *)modem_mem_calloc(1, file_size, MODEM_MEM_CLIENT_UIM);
    if(file_data_ptr == NULL)
    {
      return;
    }

    status = uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_MULTI_CONTEXT_CONF,
                                 UIM_COMMON_EFS_REGULAR_FILE_TYPE,
                                 UIM_COMMON_EFS_DEVICE,
                                 file_data_ptr,
                                 file_size);
    if(status == UIM_COMMON_EFS_SUCCESS)
    {
      if(memcmp(buf_ptr, file_data_ptr, file_size) == 0)
      {
        modem_mem_free((void *)file_data_ptr, MODEM_MEM_CLIENT_UIM);
        return;
      }
    }
  }

  status = uim_common_efs_write(UIM_COMMON_EFS_MMGSDI_MULTI_CONTEXT_CONF,
                                UIM_COMMON_EFS_REGULAR_FILE_TYPE,
                                UIM_COMMON_EFS_DEVICE,
                                buf_ptr,
                                buf_len);
  if(status != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_0("Unabled to update mmgsdi_multi_context.conf");
  }

  modem_mem_free((void *)file_data_ptr, MODEM_MEM_CLIENT_UIM);
} /* uim_common_efs_compare_and_update_conf_file */


/*=============================================================================
FUNCTION:      UIM_COMMON_EFS_CREATE_CONF_FILE

DESCRIPTION:
  This function will create config file for EFS items that are accessed in
  multiple contexts.

PARAMETERS
  None

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
=============================================================================*/
void uim_common_efs_create_conf_file
(
  void
)
{
  uim_common_efs_context_type efs_context        = UIM_COMMON_EFS_CONTEXT_0;
  uint16                      buffer_len         = 0;
  char                       *buffer_ptr         = NULL;
  uint16                      buffer_index       = 0;
  uint8                       num_item_files     = 0;
  uint8                       i                  = 0;
  char                       *context_suffix     = "_Subscription01";
  struct fs_stat              conf_stat;
  char                        file_path_buf[256];
  size_t                      copied_size        = 0;
  size_t                      context_suffix_len = 0;

  uim_common_conf_item_list_type    item_files[] =
  {
    {UIM_COMMON_EFS_MMGSDI_REFRESH_RETRY,                      TRUE},
    {UIM_COMMON_EFS_MMGSDI_REFRESH_VOTE_OK,                    TRUE},
    {UIM_COMMON_EFS_MMGSDI_FEATURE_STATUS_LIST,                TRUE},
    {UIM_COMMON_EFS_MMGSDI_ENS_SLOT_ENABLED,                   TRUE},
    {UIM_COMMON_EFS_MMGSDI_ONCHIP_SUPPORT,                     TRUE},
#ifdef FEATURE_GSTK
    {UIM_COMMON_EFS_GSTK_SLOT_CAT_VERSION,                     TRUE},
    {UIM_COMMON_EFS_GSTK_SLOT_RAT_PRIORITY_CONFIG,             TRUE},
    {UIM_COMMON_EFS_GSTK_SLOT_FEATURE_BMASK,                   TRUE},
    {UIM_COMMON_EFS_GSTK_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA,      TRUE},
    {UIM_COMMON_EFS_GSTK_SUB_BIP_OTASP_CONCURRENCY,            TRUE},
    {UIM_COMMON_EFS_GSTK_SUB_ATTACH_CTRL_TIMER,                TRUE},
    {UIM_COMMON_EFS_GSTK_SLOT_FEATURE_BMASK_2,                 TRUE},
#endif /* FEATURE_GSTK */
#ifdef FEATURE_ESTK
    {UIM_COMMON_EFS_GSTK_SLOT_BIP_APN_MAPPING_TABLE,           TRUE},
    {UIM_COMMON_EFS_GSTK_SLOT_DEFAULT_APN,                     TRUE},
#endif /* FEATURE_ESTK */
    /* PBM EFs */
    {UIM_COMMON_EFS_LAST_MODE_AND_CONFIG_DELAY_FOR_REMOTE_CONNECT, TRUE},
    {UIM_COMMON_EFS_PBM_NV_ECC_LIST_PER_SUB,                   TRUE},
    {UIM_COMMON_EFS_UIMDRV_JCDMA_MODE,                         TRUE},
    {UIM_COMMON_EFS_UIMDRV_PDOWN_NV_EF,                        TRUE},
    {UIM_COMMON_EFS_UIMDRV_LDO_SETTINGS,                       TRUE},
    {UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE,    FALSE},

    {UIM_COMMON_EFS_MMGSDI_DIAG_SUPPORT,                       FALSE},
    {UIM_COMMON_EFS_MMGSDI_HALT_SUBSCRIPTION,                  FALSE},
    {UIM_COMMON_EFS_MMGSDI_ENCRYPT_SUB_OK,                     FALSE},
    {UIM_COMMON_EFS_MMGSDI_CSIM_DEVICE_MODEL,                  FALSE},
    {UIM_COMMON_EFS_MMGSDI_ONCHIP_CONFIG_SLOT1,                FALSE},
    {UIM_COMMON_EFS_MMGSDI_ONCHIP_CONFIG_SLOT2,                FALSE},
    {UIM_COMMON_EFS_MMGSDI_ONCHIP_CONFIG_SLOT3,                FALSE},
    {UIM_COMMON_EFS_MMGSDI_ENS_SUB_FEATURES_STATUS_LIST,       FALSE},
    {UIM_COMMON_EFS_MMGSDI_BUILT_IN_PLMN_LIST,                 FALSE},
    {UIM_COMMON_EFS_MMGSDI_AUTO_ACTIVATE_MULTI_SLOTS_FILE,     FALSE},
    {UIM_COMMON_EFS_MMGSDI_GCF_GSTK_FILE,                      FALSE},
    {UIM_COMMON_EFS_MMGSDI_MULTI_PROFILE_FILE,                 FALSE},
    {UIM_COMMON_EFS_MMGSDI_GPRS_GCF_ANITE_CONTROL,             FALSE},
    {UIM_COMMON_EFS_MMGSDI_PSM,                                FALSE},

    /* GSTK EFS items */
#ifdef FEATURE_GSTK
    {UIM_COMMON_EFS_GSTK_ME_TP_CLIENTS,                        FALSE},
    {UIM_COMMON_EFS_GSTK_ME_REDIAL_CFG,                        FALSE},
    {UIM_COMMON_EFS_GSTK_ME_PROACTIVE_FEATURE_ENABLE_CFG,      FALSE},
    {UIM_COMMON_EFS_GSTK_ME_TAL_CONF,                          FALSE},
    {UIM_COMMON_EFS_GSTK_ME_FEATURE_BMASK,                     FALSE},
    {UIM_COMMON_EFS_GSTK_SLOT_TOOLKIT_DISABLED,                TRUE},
#endif /* FEATURE_GSTK */
    /* QMI_CAT EFS items */
    {UIM_COMMON_EFS_UIMQMI_QMI_CAT_MODE,                       FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_TP,                  FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_CAT_DEFAULT_LANG,               FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_CAT_DISPLAY_ALPHA_FILE,         FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_CAT_BLOCK_SMS_PP_ENV,           FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_CAT_RAW_UNSUPPORTED_FEATURES,   FALSE},

    /* QMI_UIM EFS items */
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SAP_SEC_RESTR,       FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_AUTH_SEC_RESTR,      FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_RESTR,      FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SILENT_PIN1_FILE,    FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_APDU_SEC_AID_LIST,   FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_CLOSE_CHANNEL_SYNC,  FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_SILENT_RECOVERY,     FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_SIMLOCK_STATUS_WAITING_FOR_VALID_CARD, FALSE},

    /* GBA EFS items */
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_GBA_FQDN_WHITE_LIST, FALSE},
    {UIM_COMMON_EFS_UIMQMI_QMI_UIM_CONFIG_GBA_FQDN_BLACK_LIST, FALSE},
    {UIM_COMMON_EFS_GBA_BOOTSTRAP_BSF_ADDRESS,                 FALSE},
    {UIM_COMMON_EFS_GBA_BOOTSTRAP_APN,                         FALSE},
    {UIM_COMMON_EFS_GBA_BOOTSTRAP_CUSTOM_REALM,                FALSE},
    {UIM_COMMON_EFS_GBA_BOOTSTRAP_CUSTOM_USER_AGENT_TMPI,      FALSE},
    {UIM_COMMON_EFS_GBA_BOOTSTRAP_HTTP_HEADER_IMEI,            FALSE},

    /* LPA EFS items */
    {UIM_COMMON_EFS_LPA_SUPPORT,                               FALSE}
  };

  /* Create directory, if needed */
  memset(&conf_stat, 0, sizeof(conf_stat));
  if (mcfg_fs_stat(UIM_COMMON_EFS_PATH_CONF_DIR1, &conf_stat, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE) == MCFG_FS_STATUS_EFS_ERR )
  {
    (void)mcfg_mkdir(UIM_COMMON_EFS_PATH_CONF_DIR1, 0777, MCFG_FS_TYPE_EFS);
    (void)mcfg_mkdir(UIM_COMMON_EFS_PATH_CONF_DIR2, 0777, MCFG_FS_TYPE_EFS);
    (void)mcfg_mkdir(UIM_COMMON_EFS_PATH_CONF_DIR3, 0777, MCFG_FS_TYPE_EFS);
  }
  else if (mcfg_fs_stat(UIM_COMMON_EFS_PATH_CONF_DIR2, &conf_stat, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE) == MCFG_FS_STATUS_EFS_ERR )
  {
    (void)mcfg_mkdir(UIM_COMMON_EFS_PATH_CONF_DIR2, 0777, MCFG_FS_TYPE_EFS);
    (void)mcfg_mkdir(UIM_COMMON_EFS_PATH_CONF_DIR3, 0777, MCFG_FS_TYPE_EFS);
  }
  else if (mcfg_fs_stat(UIM_COMMON_EFS_PATH_CONF_DIR3, &conf_stat, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE) == MCFG_FS_STATUS_EFS_ERR )
  {
    (void)mcfg_mkdir(UIM_COMMON_EFS_PATH_CONF_DIR3, 0777, MCFG_FS_TYPE_EFS);
  }

  num_item_files = sizeof(item_files)/sizeof(item_files[0]);
  for(i = 0; i < num_item_files; i++)
  {
    memset(file_path_buf, 0x00, sizeof(file_path_buf));
    if(uim_common_efs_get_path_from_enum(file_path_buf,
                                         sizeof(file_path_buf),
                                         item_files[i].file) != UIM_COMMON_EFS_SUCCESS)
    {
      UIM_MSG_ERR_1( "Unable to get path for file 0x%x", item_files[i].file);
      return;
    }

    if(item_files[i].is_multi_context)
    {
      /* Each file path will be written once for each EFS context.
         One new line after each file path.
         Length of suffix ("_Subscription01" or "_Subscription02" which is added
         to the file path) needs to be added for item files belonging to contexts 1 and 2. */
      buffer_len += UIM_COMMON_EFS_MAX_NUM_CONTEXTS * strlen(file_path_buf) +
                    UIM_COMMON_EFS_MAX_NUM_CONTEXTS * sizeof(char) +
                    (UIM_COMMON_EFS_MAX_NUM_CONTEXTS - 1) * strlen(context_suffix);
    }
    else
    {
      buffer_len += strlen(file_path_buf) + sizeof(char);
    }
  }

  buffer_ptr = (char *)modem_mem_calloc(1, buffer_len, MODEM_MEM_CLIENT_UIM);
  if(buffer_ptr == NULL)
  {
    UIM_MSG_ERR_0( "Memory allocation failed for conf files buffer!");
    return;
  }

  for(efs_context = UIM_COMMON_EFS_CONTEXT_0;
      efs_context <= UIM_COMMON_EFS_CONTEXT_2;
      efs_context++ )
  {
    context_suffix = NULL;
    for(i = 0; i < num_item_files; i++)
    {
      uint16 len_to_write = 0;
      if((item_files[i].is_multi_context) || (efs_context == UIM_COMMON_EFS_CONTEXT_0))
      {
        memset(file_path_buf, 0x00, sizeof(file_path_buf));
        if(uim_common_efs_get_path_from_enum(file_path_buf,
                                             sizeof(file_path_buf),
                                             item_files[i].file) != UIM_COMMON_EFS_SUCCESS)
        {
          UIM_MSG_ERR_1( "Unable to get path for file 0x%x", item_files[i].file);
          modem_mem_free((void *)buffer_ptr, MODEM_MEM_CLIENT_UIM);
          return;
        }

        switch (efs_context)
        {
          case UIM_COMMON_EFS_CONTEXT_1:
            context_suffix = "_Subscription01";
            break;
          case UIM_COMMON_EFS_CONTEXT_2:
            context_suffix = "_Subscription02";
            break;
          case UIM_COMMON_EFS_CONTEXT_0:
            /* suffix not needed for context 0 */
          default:
            /* Assume context 0 for invalid value */
            break;
        }

        if(context_suffix != NULL)
        {
          context_suffix_len = strlen(context_suffix);
          copied_size = memscpy(file_path_buf + strlen(file_path_buf),
                                sizeof(file_path_buf) - strlen(file_path_buf),
                                context_suffix,
                                context_suffix_len);
          if(copied_size < context_suffix_len)
          {
            UIM_MSG_ERR_2("memscpy truncated from 0x%x to 0x%x",
                          context_suffix_len, copied_size);
            return;
          }
        }

        len_to_write = strlen(file_path_buf) + sizeof(char);

        /* Check if buffer_ptr has enough space left */
        if (len_to_write > (buffer_len - buffer_index))
        {
          UIM_MSG_ERR_1("Insufficient buffer space for item_files[0x%x] !", i);
          modem_mem_free((void *)buffer_ptr, MODEM_MEM_CLIENT_UIM);
          return;
        }

        memscpy((void*)&buffer_ptr[buffer_index],
                buffer_len - buffer_index,
                (void*)file_path_buf,
                strlen(file_path_buf));

        buffer_index += strlen(file_path_buf);
        buffer_ptr[buffer_index++] = '\n';
      }
    }
  }

  uim_common_efs_compare_and_update_conf_file(buffer_ptr, buffer_len);
  modem_mem_free((void *)buffer_ptr, MODEM_MEM_CLIENT_UIM);
} /* uim_common_efs_create_conf_file */
