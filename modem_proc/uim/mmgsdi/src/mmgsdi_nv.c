
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

                                    A N D

                     N O N V O L A T I L E   M E M O R Y

                              I N T E R F A C E


GENERAL DESCRIPTION

  This file contains the header information for the GSDI to NV Interface.  This
  interface will be exercised by the GSDI Task and Virtual SIM Task.

EXTERNALIZED FUNCTIONS

  mmgsdi_nv_get_item()                       - Used get items from nv

  mmgsdi_nv_put_item()                       - Used put items into nv

  mmgsdi_nv_init_features_status_nv_lookup() - Used to populate global
                                               mmgsdi_features_status frm NVITEM
                                               MMGSDI_FEATURES_STATUS_LIST

  mmgsdi_nv_get_feature_status()             - Used to fetch status of features
                                               listed in
                                               mmgsdi_feature_status_enum
                                               (linked to NV ITEM MMGSDI_
                                               FEATURES_STATUS_LIST NVITEM)

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2005,2009-2016 QUALCOMM Technologies, Inc (QTI) and
its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_nv.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/02/16   gm      Support for CDMALess Device
06/29/16   shr     Support for NV based Toolkit disablement
06/19/16   bcho    F3 frequency reduction by conditional logic
05/23/16   dd      Skip Search Req for PB files
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/12/16   lm      Remove excessive logging
05/05/16   rp      Re-cache RTRE NV on MCFG refresh
09/23/15   bcho    Use MCFG Context enum instead of typecast
09/07/15   av      Migrate to mcfg EFS APIs
08/10/15   lxu     enable MMGSDI_FEATURE_IGNORE_INIT_ERROR by default
04/29/15   hh      Differentiate CSG support for W and LTE for EF-TST
12/01/15   bcho    Fixed ver number in 67211 table
12/15/14   kk      Fixed ENS settings after NV Refresh
12/03/14   bcho    featurize support to use stored rplmn name across operators
12/01/14   av      Featurize FEATURE_OPL_PNN_LOOKUP_FOR_NON_RPLMN default
11/26/14   av      Disable CSG feature by default
11/14/14   hh      Fix compilation warnings
11/12/14   hh      Use default value of NV 70361 when read fails
11/06/14   hh      Call GSTK NV API to read GSTK features in NV 65674
10/14/14   av      Handle appropriately zero ICCID reported by uimdrv
10/14/14   tl      Fix compiler warnings
10/14/14   am      Compilation warnings elimination
10/12/14   hh      Add support to get Voice Domain Preference
10/02/14   kk      Fixed KW error
09/30/14   kk      Enhancements in NV handling
09/24/14   gm      Remove GOBI mode code
09/30/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
08/29/14   hh      Fix compliation warnings
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/30/14   ar      Revert FR 20469 changes to align with NAS check-in dates
07/28/14   av      Fix version23 of NV item 67211
07/25/14   av      Auto onchip activation for multi slots
07/25/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
07/16/14   yt      Use correct size for memset()
07/15/14   yt      Support for NV refresh
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
06/02/14   bcho    New item to decide on blocking of CDMA EPRL access
05/09/14   vv      Deprecate MMGSDI_FEATURE_UICC_RAT_INDICATOR_SUPPORT
04/06/14   am      Added mmgsdi heap alloc clean up logic
04/01/14   av      NVRUIM headers cleanup
03/25/14   vdc     Added new item for verifying PIN when PIN is changed
02/27/14   dy      Add default_apn, bip_apn_mapping_tbl, rat_priority NV item
02/25/14   hh      Purge temporary mobile identities during UICC Reset Refresh
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
01/16/14   av      New item for deciding bw card and nv for 3gpd credentials
01/10/14   bcho    Refresh 3G Session Reset as per ETSI spec under NV feature
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/24/13   am      Added NV protection to Recovery API
12/18/13   yt      Skip app termination as part of session deactivation
12/09/13   vdc     Send RTRE configuration to NV asynchronously
12/05/13   ar      Replaced all mmgsdi_malloc direct calls with macro
11/21/13   vv      Halt 3gpp subscription if 3gpp2 perso fails
10/17/13   am      Added new item for plmn comparison in the nvitem 67211
10/08/13   tl      Addition of an RPM info EFS structure type
10/04/13   bcho    F3 message reduction
10/04/13   vv      Added NV support for ENS sub-features
09/26/13   av      Enable OnChip on bootup based on EFS item
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/20/13   vv      Update EF-TST to indicate support for IMS
08/20/13   spo     Automatic Subscription provisioning enhancements in UIM
07/29/13   am      Added support to determine CSG support dynamically
07/26/13   tkl     Add NV support for enabling use of SMS-PP envelope command
06/24/13   vdc     Added support for triple SIM
05/17/13   av      Halt 3gpp2 subscription if 3gpp perso fails
05/16/13   vdc     Replace memcpy with safer version memscpy
05/15/13   bcho    Added new items to mmgsdi features status nvitem 67211
05/13/13   av      Fixed KW errors
04/25/13   vdc     Added select_dfcdma to mmgsdi feature status nvitem 67211
04/17/13   tl      Replace Featurization with NV
04/11/13   tkl     Add NV suport for SETUP Call display alpha value
04/05/13   av      Move UIM tasks to group4 of RCINIT
04/04/13   bcho    Added new items to mmgsdi features status nvitem 67211
03/27/13   spo     Added new items to mmgsdi feature status list
02/19/13   av      Merge mmgsdi,gstk,qmiuim/cat conf files to mmgsdi.conf
01/04/13   vv      Handle NV write command in main task
01/03/13   abg     Added API mmgsdi_nv_init_read_and_cache_nv_items to read NV during task-init
12/26/12   av      Do not allow writing to the file that is being refreshed
11/05/12   av      Expand features status list nvitem to control skipping
                   checking security attributes before reading EF
10/31/12   tl      CSIM conformance updating EF MECRP
10/31/12   tl      CSIM conformance updating EF MODEL
10/21/12   av      Update conf file only if necessary
10/16/12   yt      Add new items to mmgsdi feature status list
09/28/12   yt      Update user profile parameters after successful OTASP Commit
08/24/12   abg     fix default value of ignore_init_error
04/20/12   av      Enabled detouring of nv/efs calls to offtarget framework
03/08/12   av      Added new items to mmgsdi features status nvitem 67211
01/30/12   shr     Use GSTK NV item (65674) to determine if the OTASP IMSI_M
                   enhancement is enabled or not
01/13/12   at      Added NV item caching for NV_DISABLE_CM_CALL_TYPE_I
12/21/11   shr     Legacy GSDI removal updates
11/15/11   nmb     RPM Updates
10/10/11   av      Updated default values of MMGSDI Features
09/16/11   av      Replace feature #ifdefs with an NVITEM
07/12/11   at      Added NV item caching for NV_SERVICE_DOMAIN_PREF_I
06/05/11   vs      Updated NV item caching logic to include NV_ESN_ME_I
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/24/10   js      Resolve race condition between GSDI NV READ/WRITE
10/25/10   nmb     Copy NV item data to NV command buffer
10/04/10   nmb     Added NV queue to address NV deadlocks
09/08/10   nmb     mmgsdi_task_cmd_type memory reduction
07/14/10   js      Fixed compilation warnings
05/13/10   vs      Fix for NV and GSDI deadlock
10/21/09   rn      Fixed klocwork errors
09/26/09   kp      ZI memory reduction changes
07/20/09   mib     Replaced flag TEST_FRAMEWORK with FEATURE_UIM_TEST_FRAMEWORK
02/27/09   nd      Virtuim changes for QTF
03/08/06   jar     Merged in Branched SIM Lock
05/11/05   sst     Lint fixes
11/15/04   jar     Clear out GSDI NV Signal when NV Response is received
02/21/04   jar     Removed gsdi_wait_nv() since it is not being used and
                   causes compiler errors.
11/05/03   jar     Added support for Slot Preference Changes.  Switch from
                   gsdi_wait_nv() to gsdi_wait() to allow for GSDI_RPT_SIG
                   handling.
07/02/03   tml     Fixed gsdi_put_nv
12/05/02   jar     Added #include "err.h" for ERR Macros.
11/26/02   jar     Removed RTRE Code.
08/30/02 jar/ck    Added gsdi_send_rtre_command.  changed gsdi_nvi to gsdi_nv
                   _cmd.  Localized gsdi_wait_nv.  Added internal feature
                   MMGSDI_RTRE.
08/18/02   jar     Created gsdi_nv.h
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "mmgsdi_nv.h"
#include "nv.h"
#include "uim_msg.h"
#include "err.h"
#include "mmgsdiutil.h"
#include "mmgsdi_refresh.h"
#include "mmgsdisessionlib_v.h"
#include "mmgsdi.h"
#include "queue.h"
#include "fs_public.h"
#include "gstk_exp_v.h"
#include "mmgsdilib_v.h"
#include "uim_common_efs.h"
#include "mmgsdi_ruim_jcdma.h"
#include "mmgsdi_refresh.h"
#include "mmgsdilib_p.h"
#include "mcfg_fs.h"

#ifdef FEATURE_FEMTO_CSG
#include "sys_v.h"
#endif /* FEATURE_FEMTO_CSG */
#include "sys.h"

#include "qmi_uim.h"
#include "qmi_cat.h"
#ifdef FEATURE_GSTK
#include "gstk_nv_p.h"
#endif /* FEATURE_GSTK */
#ifdef FEATURE_ESTK
#include "estk.h"
#endif /* FEATURE_ESTK */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

static mmgsdi_nv_item_cache_table_type *mmgsdi_nv_item_cache_table_ptr[MMGSDI_NV_CONTEXTS_MAX];

static mmgsdi_return_enum_type mmgsdi_nv_put_nv_item_in_cache (
  nv_items_enum_type      item_enum,
  const nv_item_type     *data_ptr,
  nv_stat_enum_type       item_status,
  mmgsdi_nv_context_type  nv_context
);

/* defines for RPM ICCID change identification support */
#define MMGSDI_RPM_ICCID_FILE "/nv/item_files/modem/uim/mmgsdi/rpm_iccid"

/* GSTK file in EFS corresponding to NV 65674 (GSTK Features Status) */
#define GSTK_FEATURES_FILE    "/nv/item_files/modem/uim/gstk/feature_bmsk"

/* NAS file in EFS corresponding to NV 70361 (CSG Support Flag) */
#define NAS_CSG_SUPPORT_FILE  "/nv/item_files/modem/nas/csg_support_configuration"

/* IMS file in EFS corresponding to NV 67218 (IMS Support Flag) */
#define IMS_SUPPORT_FILE      "/nv/item_files/ims/IMS_enable"

/* RPM info file in EFS corresponding to NV 67210 (RPM INFO) */
#define RPM_INFO_FILE         "/nv/item_files/modem/mmode/rpm_info"

/*---------------------------------------------------------------------------
  To store list of status of mmgsdi features for each slot. Status of an
  operator-specific feature is stored at the index corresponding to that slot.
  Status of  ME-specific features is stored at 0-th index.
  Refer to function mmgsdi_nv_is_feature_item_slot_based() for the list of
  operator-specific features.
---------------------------------------------------------------------------*/
static mmgsdi_features_status_list_type features_status_list[MMGSDI_MAX_NUM_SLOTS];

typedef PACKED struct PACKED_POST
{
  mmgsdi_nv_features_enum_type     feature;
  uint8                            version_introduced;
  mmgsdi_feature_status_enum_type  default_value;
  boolean                          slot_based;
} mmgsdi_features_list_type;

static mmgsdi_features_list_type features_list_init_table[] =
{
  { MMGSDI_FEATURE_ABSENT_MANDATORY_FILES_INIT,                     0, MMGSDI_FEATURE_ENABLED,  TRUE  },
  { MMGSDI_FEATURE_ECC_NOT_PRESENT_SUPPORT,                         0, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_SPECIAL_USIM_ARR,                                0, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_DEPRECATED_1,                                    0, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_CDMA_DF_SELECT_BEFORE_TP,                        0, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_CHANGE_INST_CLASS,                               0, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_CARD_SLOT_CHECK,                                 0, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_TMSI_PLMN_CHECK,                                 0, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_IGNORE_INIT_ERROR,                               2, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_ALLOW_SEND_APDU_ONLY,                            2, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_PREF_MODE_NOT_FROM_RUIM,                         2, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_RUIM_ALLOW_ESN_FAIL,                             2, MMGSDI_FEATURE_DISABLED, TRUE  },
  { MMGSDI_FEATURE_RUIM_ZERO_ESN_FAIL,                              2, MMGSDI_FEATURE_ENABLED,  TRUE  },
  { MMGSDI_FEATURE_UIM_RUIM_SUPPORT_SCI,                            2, MMGSDI_FEATURE_ENABLED,  TRUE  },
  { MMGSDI_FEATURE_UIM_AN_HRPD_FALLBACK,                            2, MMGSDI_FEATURE_DISABLED, TRUE  },
  { MMGSDI_FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND,                2, MMGSDI_FEATURE_ENABLED,  TRUE  },
  { MMGSDI_FEATURE_PERSONALIZATION_SLOTS_LOCKED_ON_SLOT1,           2, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_CSIM_UPP_UPDATE_AFTER_OTASP,                     3, MMGSDI_FEATURE_DISABLED, TRUE  },
  { MMGSDI_FEATURE_ALLOW_ACCESS_BEFORE_APP_READY,                   4, MMGSDI_FEATURE_DISABLED, FALSE  },
  { MMGSDI_FEATURE_SKIP_UICC_SECURITY_ATTR_CHECK_BEFORE_READ,       5, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_BLOCK_WRITES_TO_REFRESH_FILES,                   6, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_SE13_TABLE_LOOKUP_GET_FIRST_GSMA_MATCH,          7, MMGSDI_FEATURE_DISABLED, FALSE  },
  { MMGSDI_FEATURE_NO_PERSONALIZATION_ON_ADDITIONAL_SLOTS,          8, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_MMGSDI_UNBLOCK_PIN_HANDLE_INVALID_SW,            9, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_UIM_GSM_DCS_1800,                                9, MMGSDI_FEATURE_DISABLED, TRUE  },
  { MMGSDI_FEATURE_DEPRECATED_2,                                    9, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_EXPLICIT_SELECT_DFCDMA_BEFORE_TELECOM_ADN_READ, 10, MMGSDI_FEATURE_DISABLED, FALSE },
#ifdef FEATURE_CUST_1
  { MMGSDI_FEATURE_OPL_PNN_LOOKUP_FOR_NON_RPLMN,                   11, MMGSDI_FEATURE_ENABLED,  FALSE },
#else
  { MMGSDI_FEATURE_OPL_PNN_LOOKUP_FOR_NON_RPLMN,                   11, MMGSDI_FEATURE_DISABLED, FALSE },
#endif /* FEATURE_CUST_1 */
  { MMGSDI_FEATURE_HALT_3GPP2_SUBS_IF_3GPP_PERSO_FAILS,            12, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_MULTISIM_AUTO_PROVISIONING,                     13, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_NORTH_AMERICAN_PLMN_COMPARISON,                 14, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_HALT_3GPP_SUBS_IF_3GPP2_PERSO_FAILS,            15, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_SKIP_PROV_APP_TERMINATION,                      16, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_REFRESH_3G_SESSION_RESET_PER_ETSI_SPEC,         17, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_USE_3GPD_CREDENTIALS_FROM_NV,                   18, MMGSDI_FEATURE_DISABLED, TRUE  },
  { MMGSDI_FEATURE_USE_SFI,                                        19, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_OPLMN_LIST_CONFIGURABLE_OTA,                    20, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_PURGE_TEMP_IDENTITIES,                          20, MMGSDI_FEATURE_DISABLED, TRUE  },
  { MMGSDI_FEATURE_VERIFY_PIN_AFTER_CHANGE,                        21, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_BLOCK_RUIM_EPRL_ACCESS,                         22, MMGSDI_FEATURE_DISABLED, FALSE },
  { MMGSDI_FEATURE_SUPPORT_EXTENDED_FPLMN_ICC,                     23, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_USE_STORED_RPLMN_NAME_ACROSS_OPERATORS,         24, MMGSDI_FEATURE_ENABLED,  FALSE },
  { MMGSDI_FEATURE_SKIP_SEARCH_FOR_PB_FILES,                       25, MMGSDI_FEATURE_DISABLED,  TRUE }
};

/*---------------------------------------------------------------------------
  To store list of status of external features (i.e. features outside of MMGSDI)
  for each slot/subscription.
     nas_cfg_feature_csg:         Status of CSG Feature (NAS NV 70361)
     ims_cfg_feature_ims_support: Status of IMS Support (IMS NV 67218)
     rpm_enabled: Indicates whether RPM feature is enabled
                                  (from MMODE NV 67210 )
     jcdma_service_status:        Indicates if JCDMA is enabled
     volte_using_ims:             VOLTE using IMS (CM NV 66048)
                                  The following values are considered as supported:
                                    SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY
                                    SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED
                                    SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED
-------------------------------------------------------------------------------*/
static struct {
  mmgsdi_feature_status_enum_type  nas_cfg_feature_csg;
  mmgsdi_feature_status_enum_type  ims_cfg_feature_ims_support;
  mmgsdi_feature_status_enum_type  rpm_enabled;
  mmgsdi_feature_status_enum_type  jcdma_service_status;
  mmgsdi_feature_status_enum_type  volte_using_ims;
  mmgsdi_feature_status_enum_type  cdma_less_device;
} mmgsdi_external_features_status_list[MMGSDI_MAX_NUM_SLOTS];

/*---------------------------------------------------------------------------
  To store the status of ENS feature for each slot
---------------------------------------------------------------------------*/
static boolean mmgsdi_ens_slot_enabled[MMGSDI_MAX_NUM_SLOTS];

/*---------------------------------------------------------------------------
  To store status of mmgsdi ENS sub features
---------------------------------------------------------------------------*/
static mmgsdi_ens_sub_features_status_list_type mmgsdi_ens_sub_features_status_list;

/*===========================================================================
FUNCTION MMGSDI_NV_GET_ITEM

DESCRIPTION
  Get an item from the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
nv_stat_enum_type mmgsdi_nv_get_item(
  nv_items_enum_type         item,
  mmgsdi_nv_item_cache_type *mmgsdi_nv_data_ptr,
  mmgsdi_nv_context_type     nv_context
)
{
  nv_cmd_type             *mmgsdi_nv_cmd_ptr = NULL;
  nv_stat_enum_type        nv_status         = NV_NOTALLOC_S;
  mmgsdi_task_enum_type    mmgsdi_task       = MMGSDI_TASK_MAX;
  nv_item_type            *nv_data_ptr       = NULL;

  UIM_MSG_HIGH_1("In mmgsdi_nv_get_item, item: 0x%x", item);

  /* Get current task info */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);

  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    UIM_MSG_ERR_0("Could not determine MMGSDI Task");
    return NV_BADPARM_S;
  }

  /* Device specific items are accessed from primary context,
     if the context is DEVICE SPECIFIC, convert it into PRIMARY CONTEXT */
  if(nv_context == MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
  {
    nv_context = MMGSDI_NV_CONTEXT_PRIMARY;
  }

  if (mmgsdi_nv_is_nv_item_cached(item, nv_context))
  {
    return mmgsdi_nv_get_nv_item_from_cache(item, mmgsdi_nv_data_ptr, nv_context);
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_nv_cmd_ptr, sizeof(nv_cmd_type));

  if(mmgsdi_nv_cmd_ptr == NULL)
  {
    return NV_NOTALLOC_S;
  }

  /* Notify this task when done */
  mmgsdi_nv_cmd_ptr->tcb_ptr = rex_self();
  mmgsdi_nv_cmd_ptr->sigs = MMGSDI_NV_READ_SIG;

  /* Command goes on no queue when done */
  mmgsdi_nv_cmd_ptr->done_q_ptr = NULL;

  /* Item to read */
  mmgsdi_nv_cmd_ptr->item = item;
  mmgsdi_nv_cmd_ptr->cmd = NV_READ_F;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(nv_data_ptr,
                                     sizeof(nv_item_type));

  if(nv_data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_nv_cmd_ptr);
    return NV_NOTALLOC_S;
  }

  /* Set the Data Ptr for NV Item to read */
   mmgsdi_nv_cmd_ptr->data_ptr = nv_data_ptr;

  /* Clear REX Signals */
  (void) rex_clr_sigs( rex_self(), MMGSDI_NV_READ_SIG );

  /* Send Command to NV */
  nv_cmd(mmgsdi_nv_cmd_ptr);

  /* Wait for Signal from NV that it's complete */
  (void)mmgsdi_wait(MMGSDI_NV_READ_SIG);

  /* Clear Signal */
  (void) rex_clr_sigs( rex_self(), MMGSDI_NV_READ_SIG );

  (void)mmgsdi_nv_put_nv_item_in_cache(
          item,
          nv_data_ptr,
          mmgsdi_nv_cmd_ptr->status,
          nv_context);

  nv_status = mmgsdi_nv_cmd_ptr->status;
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_nv_cmd_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(nv_data_ptr);

  /* Need to copy the recently cached item into out ptr */
  if (mmgsdi_nv_is_nv_item_cached(item, nv_context))
  {
    return mmgsdi_nv_get_nv_item_from_cache(item, mmgsdi_nv_data_ptr, nv_context);
  }

  return nv_status;
} /* mmgsdi_nv_get_item */


/*===========================================================================
FUNCTION MMGSDI_NV_PUT_ITEM

DESCRIPTION
  Put an item on to the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
nv_stat_enum_type mmgsdi_nv_put_item(
  nv_items_enum_type      item,
  const nv_item_type     *data_ptr,
  mmgsdi_nv_context_type  nv_context
)
{
  nv_cmd_type             *mmgsdi_nv_cmd_ptr  = NULL;
  mmgsdi_return_enum_type  mmgsdi_status      = MMGSDI_ERROR;
  nv_item_type            *cmd_data_ptr       = NULL;
  int32                    mmgsdi_nv_cmd_size = 0;

  UIM_MSG_HIGH_1("In mmgsdi_nv_put_item, item: %d", item);

  if(data_ptr == NULL)
  {
    return NV_BADPARM_S;
  }

  /* Device specific items are accessed from primary context,
     if the context is DEVICE SPECIFIC, convert it into PRIMARY CONTEXT */
  if(nv_context == MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
  {
    nv_context = MMGSDI_NV_CONTEXT_PRIMARY;
  }

  /* Allocate memory for the NV command */
  mmgsdi_util_cal_align_size(sizeof(nv_cmd_type), &mmgsdi_nv_cmd_size);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_nv_cmd_ptr,
                                     (mmgsdi_nv_cmd_size + sizeof(nv_item_type)));

  if(mmgsdi_nv_cmd_ptr == NULL)
  {
    return NV_NOTALLOC_S;
  }

  /* Copy data to write */
  cmd_data_ptr = (nv_item_type*)((uint8*)mmgsdi_nv_cmd_ptr + mmgsdi_nv_cmd_size);
  mmgsdi_memscpy(cmd_data_ptr,
                 sizeof(nv_item_type),
                 data_ptr,
                 sizeof(nv_item_type));

  /* Notify main MMGSDI task when done */
  mmgsdi_nv_cmd_ptr->tcb_ptr    = UIM_GSDI_TCB;
  mmgsdi_nv_cmd_ptr->sigs       = MMGSDI_NV_WRITE_SIG;
  mmgsdi_nv_cmd_ptr->done_q_ptr = &mmgsdi_nv_q;

  /* Item to write */
  mmgsdi_nv_cmd_ptr->item     = item;
  mmgsdi_nv_cmd_ptr->cmd      = NV_WRITE_F;
  mmgsdi_nv_cmd_ptr->data_ptr = cmd_data_ptr;

  /* Send Command to NV */
  nv_cmd(mmgsdi_nv_cmd_ptr);

  mmgsdi_status = mmgsdi_nv_put_nv_item_in_cache(
                    item,
                    data_ptr,
                    NV_DONE_S,
                    nv_context);

  UIM_MSG_HIGH_1("MMGSDI NV Cache write status=0x%x", mmgsdi_status);

  return NV_DONE_S;
}/* mmgsdi_nv_put_item */


/*===========================================================================
FUNCTION MMGSDI_NV_IS_NV_ITEM_CACHED

DESCRIPTION
  This function returns TRUE if the NV item is cached by MMGSDI

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_nv_is_nv_item_cached (
  nv_items_enum_type     item_enum,
  mmgsdi_nv_context_type nv_context
)
{
  uint32 i    = 0x0;
  uint32 mask = 0x1;

  if (nv_context >= MMGSDI_NV_CONTEXTS_MAX ||
      mmgsdi_nv_item_cache_table_ptr[nv_context] == NULL)
  {
    UIM_MSG_ERR_1("nv_context - 0x%x is invalid or mmgsdi_nv_item_cache_table_ptr is NULL",
                  nv_context);
    return FALSE;
  }

  for (i=0; i<MMGSDI_NV_ITEM_CACHE_SIZE; i++)
  {
    if (mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_enum == item_enum)
    {
      /* Create bit mask using the index of the item to determine if cached */
      if (mmgsdi_nv_item_cache_table_ptr[nv_context]->is_cached_mask &
          (mask << i))
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
  }

  UIM_MSG_ERR_1("Getting item: %d will cause deadlocks. Add to NV cache",
                item_enum);
  /* This function should be called only with items that are in the cache
     to see if they are currently valid before calling
     mmgsdi_nv_get_nv_item_from_cache. Return true if item is not a cached
     item so that getting the item from the cache fails. */
  return TRUE;
} /* mmgsdi_nv_is_nv_item_cached */


/*===========================================================================
FUNCTION MMGSDI_NV_GET_NV_ITEM_FROM_CACHE

DESCRIPTION
  This function provides the cached value of the NV item

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  nv_stat_enum_type

SIDE EFFECTS
  None
===========================================================================*/
nv_stat_enum_type mmgsdi_nv_get_nv_item_from_cache (
  nv_items_enum_type         item_enum,
  mmgsdi_nv_item_cache_type *mmgsdi_nv_data_ptr,
  mmgsdi_nv_context_type     nv_context
)
{
  uint32 i = 0x0;

  if (nv_context >= MMGSDI_NV_CONTEXTS_MAX ||
      mmgsdi_nv_item_cache_table_ptr[nv_context] == NULL)
  {
    UIM_MSG_ERR_1("nv_context - 0x%x is invalid or mmgsdi_nv_item_cache_table_ptr is NULL",
                  nv_context);
    return NV_NOTALLOC_S;
  }

  for (i =0; i<MMGSDI_NV_ITEM_CACHE_SIZE; i++)
  {
    if (mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_enum == item_enum)
    {
      *mmgsdi_nv_data_ptr =
        mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i];

      UIM_MSG_HIGH_1("Got NV item: %d in MMGSDI NV Cache", item_enum);
      return mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_status;
    }
  }

  UIM_MSG_ERR_1("Did not find item %d in cache", item_enum);
  return NV_NOTALLOC_S;
} /* mmgsdi_nv_get_nv_item_from_cache */


/*===========================================================================
FUNCTION MMGSDI_NV_PUT_NV_ITEM_IN_CACHE

DESCRIPTION
  This function copies the NV item to the cache

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_nv_put_nv_item_in_cache (
  nv_items_enum_type     item_enum,
  const nv_item_type    *data_ptr,
  nv_stat_enum_type      item_status,
  mmgsdi_nv_context_type nv_context
)
{
  uint32 i    = 0x0;
  uint32 mask = 0x1;

  if (nv_context >= MMGSDI_NV_CONTEXTS_MAX ||
      mmgsdi_nv_item_cache_table_ptr[nv_context] == NULL)
  {
    UIM_MSG_ERR_1("nv_context - 0x%x is invalid or mmgsdi_nv_item_cache_table_ptr is NULL",
                  nv_context);
    return MMGSDI_ERROR;
  }

  for (i=0; i<MMGSDI_NV_ITEM_CACHE_SIZE; i++)
  {
    if (mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_enum == item_enum)
    {
      mmgsdi_nv_item_cache_table_ptr[nv_context]->is_cached_mask |= (mask << i);
      mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_status =
        item_status;
      if (item_status == NV_DONE_S)
      {
        switch(item_enum)
        {
          case NV_GPRS_ANITE_GCF_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.gprs_anite_gcf =
              data_ptr->gprs_anite_gcf;
            break;

          case NV_UIM_CDMA_PREF_SLOT_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.uim_cdma_pref_slot =
              data_ptr->uim_cdma_pref_slot;
            break;

          case NV_UIM_GSM_PREF_SLOT_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.uim_gsm_pref_slot =
              data_ptr->uim_gsm_pref_slot;
            break;

          case NV_RTRE_CONFIG_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.rtre_config =
              data_ptr->rtre_config;
            break;

          case NV_MMGSDI_ME_CONFIG_PARAM_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.mmgsdi_me_config_param =
              data_ptr->mmgsdi_me_config_param;
            break;

          case NV_UIM_SELECT_DEFAULT_USIM_APP_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.uim_select_default_usim_app =
              data_ptr->uim_select_default_usim_app;
            break;

          case NV_MEID_ME_I:
            mmgsdi_memscpy(&mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.meid,
                           sizeof(mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.meid),
                           &data_ptr->meid,
                           sizeof(data_ptr->meid));
            break;

          case NV_UE_IMEI_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.ue_imei =
              data_ptr->ue_imei;
            break;

          case NV_ENS_ENABLED_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.ens_enabled =
              data_ptr->ens_enabled;
            break;

          case NV_HOMEZONE_ENABLED_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.homezone_enabled =
              data_ptr->homezone_enabled;
            break;

          case NV_FTM_MODE_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.ftm_mode =
              data_ptr->ftm_mode;
            break;

          case NV_ESN_ME_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.esn =
              data_ptr->esn;
            break;

          case NV_SCM_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.scm =
              data_ptr->scm;
            break;

          case NV_MOB_CAI_REV_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.mob_cai_rev =
              data_ptr->mob_cai_rev;
            break;

          case NV_JCDMA_UIM_LOCK_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.jcdma_uim_lock =
              data_ptr->jcdma_uim_lock;
            break;

          case NV_JCDMA_RUIM_ID_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.jcdma_ruim_id =
              data_ptr->jcdma_ruim_id;
            break;

          case NV_SERVICE_DOMAIN_PREF_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.service_domain_pref =
              data_ptr->service_domain_pref;
            break;

          case NV_DISABLE_CM_CALL_TYPE_I:
            mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_value.disable_cm_call_type =
              data_ptr->disable_cm_call_type;
            break;

          default:
            break;
        }
      }
      UIM_MSG_HIGH_1("Put NV item: %d in MMGSDI NV Cache", item_enum);
      return MMGSDI_SUCCESS;
    }
  }

  UIM_MSG_ERR_1("Using item: %d will cause deadlocks. Add to NV cache",
                item_enum);
  return MMGSDI_ERROR;
} /* mmgsdi_nv_put_nv_item_in_cache */


/*===========================================================================
FUNCTION MMGSDI_NV_ITEM_CACHE_INIT

DESCRIPTION
  Allocate memory and initialize the items in the cache

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_nv_item_cache_init (
  void
)
{
  uint8 nv_context = 0;

  for(nv_context = 0; nv_context < MMGSDI_NV_CONTEXTS_MAX; nv_context++)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_nv_item_cache_table_ptr[nv_context],
      sizeof(mmgsdi_nv_item_cache_table_type));
    if (mmgsdi_nv_item_cache_table_ptr[nv_context] == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_nv_item_cache_table_ptr[nv_context]->is_cached_mask      = 0x0;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x0].item_enum = NV_GPRS_ANITE_GCF_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x1].item_enum = NV_UIM_CDMA_PREF_SLOT_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x2].item_enum = NV_UIM_GSM_PREF_SLOT_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x3].item_enum = NV_RTRE_CONFIG_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x4].item_enum =
      NV_MMGSDI_ME_CONFIG_PARAM_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x5].item_enum =
      NV_UIM_SELECT_DEFAULT_USIM_APP_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x6].item_enum = NV_MEID_ME_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x7].item_enum = NV_UE_IMEI_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x8].item_enum = NV_ENS_ENABLED_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x9].item_enum = NV_HOMEZONE_ENABLED_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0xA].item_enum = NV_FTM_MODE_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0xB].item_enum = NV_ESN_ME_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0xC].item_enum = NV_SCM_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0xD].item_enum = NV_MOB_CAI_REV_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0xE].item_enum = NV_JCDMA_UIM_LOCK_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0xF].item_enum = NV_JCDMA_RUIM_ID_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x10].item_enum = NV_SERVICE_DOMAIN_PREF_I;
    mmgsdi_nv_item_cache_table_ptr[nv_context]->item[0x11].item_enum = NV_DISABLE_CM_CALL_TYPE_I;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_nv_item_cache_init */


/*===========================================================================
FUNCTION MMGSDI_NV_FEATURE_WRITE_DEFAULT_VALUE

DESCRIPTION
  Function used to write the default value of MMGSDI Feature (controlled by
  MMGSDI_Feature_Status_list NVITEM) to global features_status_list.

DEPENDENCIES

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_nv_feature_write_default_value(
  mmgsdi_nv_features_enum_type nv_feature,
  uint8                        index)
{
  MMGSDIUTIL_RETURN_IF_EXCEEDS(index, (MMGSDI_MAX_NUM_SLOTS - 1));

  if(nv_feature >= MMGSDI_MAX_ITEMS_IN_FEATURE_NVITEM)
  {
    return MMGSDI_ERROR;
  }

  features_status_list[index].item[nv_feature] = features_list_init_table[nv_feature].default_value;
  return MMGSDI_SUCCESS;
} /* mmgsdi_nv_feature_write_default_value */


/*===========================================================================
FUNCTION MMGSDI_NV_GET_NUM_OF_FEATURES_IN_VER

DESCRIPTION
  Function used to get no of features until a particular version.

DEPENDENCIES

RETURN VALUE
  num_features_in_ver

SIDE EFFECTS
  None
===========================================================================*/
static uint8 mmgsdi_nv_get_num_of_features_in_ver(
  uint8 version_to_count
)
{
  uint8 i                   = 0;
  uint8 no_of_entries       = sizeof(features_list_init_table)/sizeof(mmgsdi_features_list_type);
  uint8 num_features_in_ver = 0;

  for (i = 0; i < no_of_entries; i++)
  {
    if(version_to_count >= features_list_init_table[i].version_introduced)
    {
      num_features_in_ver++;
    }
    else
    {
      break;
    }
  }

  return num_features_in_ver;
}/* mmgsdi_nv_get_num_of_features_in_ver */


/*===========================================================================
FUNCTION MMGSDI_NV_WRITE_DEFAULT_VALUE_OF_DELTA_FEATURES

DESCRIPTION
  Function used to write the default values of MMGSDI Features (controlled by
  MMGSDI_Feature_Status_list NVITEM) that are delta between the two
  versions of Feature_Status_List to global features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_nv_write_default_value_of_delta_features(
  uint8 version_in_nv,
  uint8 index
)
{
  uint8 mmgsdi_feature;
  uint8 num_features_in_nv;
  uint8 num_features_in_build;
  uint8 no_of_entries         = 0;
  uint8 curr_version_in_build = 0;

  no_of_entries = sizeof(features_list_init_table)/sizeof(mmgsdi_features_list_type);

  curr_version_in_build = features_list_init_table[no_of_entries - 1].version_introduced;

  if(version_in_nv >= curr_version_in_build)
  {
    UIM_MSG_ERR_0("Array out-of-bounds while writing default values for MMGSDI features");
    return;
  }

  num_features_in_nv    = mmgsdi_nv_get_num_of_features_in_ver(version_in_nv);
  num_features_in_build = mmgsdi_nv_get_num_of_features_in_ver(curr_version_in_build);

  /* Since mmgsdi_features_total_till_version array is indexed by the
     feature_status_list *version*, it's value at version version_in_nv is
     essentially the first feature of the delta list...no need to bail out in
     case writing fails... */
  for(mmgsdi_feature = num_features_in_nv; mmgsdi_feature < num_features_in_build; mmgsdi_feature++)
  {
    (void)mmgsdi_nv_feature_write_default_value((mmgsdi_nv_features_enum_type )mmgsdi_feature,
                                                index);
  }
} /* mmgsdi_nv_write_default_value_of_delta_features */


/*===========================================================================
FUNCTION MMGSDI_NV_WRITE_DEFAULT_VALUE_OF_ALL_FEATURES

DESCRIPTION
  Function used to write the default values of all the MMGSDI Features
  (controlled by MMGSDI_Feature_Status_list NVITEM), defined currently, to
  global features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_nv_write_default_value_of_all_features(
  uint8   index)
{
  uint8 mmgsdi_feature;
  uint8 num_features_in_build;

  if (index >= MMGSDI_MAX_NUM_SLOTS)
  {
    return;
  }

  num_features_in_build = sizeof(features_list_init_table)/sizeof(mmgsdi_features_list_type);

  memset(&features_status_list[index], 0x00, sizeof(features_status_list[index]));

  /* Update the Version in features_status_list to the current build version */
  features_status_list[index].version = features_list_init_table[num_features_in_build - 1].version_introduced;

  /* Update features_status_list with default values */
  for(mmgsdi_feature = 0; mmgsdi_feature < num_features_in_build; mmgsdi_feature++)
  {
    (void)mmgsdi_nv_feature_write_default_value((mmgsdi_nv_features_enum_type )mmgsdi_feature,
                                                index);
  }
} /* mmgsdi_nv_write_default_value_of_all_features */


/*===========================================================================
FUNCTION MMGSDI_NV_FEATURES_STATUS_LIST_UPDATE_VERSION

DESCRIPTION
  Function used to validate the MMGSDI Features Status NVITEM's version.
  If what's in the NV is less than the one currently defined in the build,
  updates it to the current version defined and writes it back to the NV.
  It also writes back to the NV the default values of the delta items in
  in the list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Version gets updated in the NV if it doesn't match the one in the build. If
  the version is updated, next time when the user reads the NVITEM, a
  dropdown of items corresponding to the updated version is shown instead of
  what the user saw the first time the NV read was done (corresponding to
  the version present in the NV at that point).
===========================================================================*/
static void mmgsdi_nv_features_status_list_update_version(
  uint8                        index)
{
  uim_common_efs_context_type   efs_context           = UIM_COMMON_EFS_CONTEXT_MAX;
  uint8                         num_features_in_build = 0;

  efs_context = mmgsdi_util_get_common_efs_item_context(index);
  if (efs_context == UIM_COMMON_EFS_CONTEXT_MAX)
  {
    return;
  }

  num_features_in_build = sizeof(features_list_init_table)/sizeof(mmgsdi_features_list_type);

  UIM_MSG_HIGH_2("MMGSDI Features Status NV ITEM version - in NV: 0x%x, in build: 0x%x",
                 features_status_list[index].version,
                 features_list_init_table[num_features_in_build - 1].version_introduced);

  if (features_status_list[index].version < features_list_init_table[num_features_in_build - 1].version_introduced)
  {
    /* If the features_status_list version in NV is less than current
       features_status_list version in our code, meaning new features have been
       added since the version in NV till the current version, we write back
       the default value of all those delta features to the NV. */
    mmgsdi_nv_write_default_value_of_delta_features(
      features_status_list[index].version,
      index);

    features_status_list[index].version = features_list_init_table[num_features_in_build - 1].version_introduced;

    if (uim_common_efs_write(UIM_COMMON_EFS_MMGSDI_FEATURE_STATUS_LIST,
                             UIM_COMMON_EFS_ITEM_FILE_TYPE,
                             efs_context,
                             (const char*)&features_status_list[index],
                             sizeof(features_status_list[index])) != UIM_COMMON_EFS_SUCCESS)
    {
      UIM_MSG_ERR_0("MMGSDI Features Status NV ITEM version update failed!");
    }
  }
} /* mmgsdi_nv_features_status_list_update_version */


/*===========================================================================
FUNCTION MMGSDI_NV_INIT_FEATURES_STATUS_NV_LOOKUP

DESCRIPTION
  Function used to get the status of MMGSDI features as defined by the NVITEM
  "MMGSDI_FEATURES_STATUS_LIST" (67211).
  The global, "features_status_list", used to hold the NVITEM, is expandable,
  yet the size is fixed. See the struct definition for more detailed doc on
  that. The global struct has a "version" item that is updated everytime we
  add new items to this global struct (starting with version 0 in the first
  implementation)
  The mmgsdi code has full control over version of NVITEM. Taking an example,
  if in future, in version 2 of this struct(that has 10 items), the delta is 3
  items (from version 0 that had 7 items), and this build is loaded onto the
  device that already had the items set using version 0 of this NVITEM, then,
  on boot-up after loading the build, mmgsdi will validate the version it read
  from NV. If it is not 2, it will update it to 2 and write it back to NV. At
  this point, however, the value of features/items 8 through 10 will be set to
  their default values by mmgsdi code.

DEPENDENCIES
  Minimum QXDM version required to be able to set this NVITEM (67211) is
  QXDM 3.12.997.

RETURN VALUE
  None

SIDE EFFECTS
  "version" item in this NVITEM is writable through QXDM (no QXDM support to
  hide it or grey it out) or QCN. However if what is written explicitly doesn't
  match what's in the build, mmgsdi will change it to what's in the build and
  it could be annoying for the QXDM user to see it change to something else
  (when the user reads the NVITEM next time).
===========================================================================*/
void mmgsdi_nv_init_features_status_nv_lookup(
  uint8     index)
{
  uim_common_efs_context_type   efs_context = UIM_COMMON_EFS_CONTEXT_MAX;


  efs_context = mmgsdi_util_get_common_efs_item_context(index);
  if (efs_context == UIM_COMMON_EFS_CONTEXT_MAX)
  {
    return;
  }

  /* Start off with the global struct initialised to 0 */
  memset(&features_status_list[index],
         0x00,
         sizeof(features_status_list[index]));

  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_FEATURE_STATUS_LIST,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)&features_status_list[index],
                          sizeof(features_status_list[index])) ==
      UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_MED_1("Successfully read the MMGSDI Features Status from NV at index 0x%x", index);
    mmgsdi_nv_features_status_list_update_version(index);
  }
  else
  {
    UIM_MSG_HIGH_1("Failed to read MMGSDI Features Status from NV. Writing back default values at index 0x%x",
                   index);

    /* Updating global features_status_list with default value of all the
       defined features */
    mmgsdi_nv_write_default_value_of_all_features(index);

    /* Write features' status to the EFS, erasing previous contents,
    create if not present */
    (void)uim_common_efs_write(UIM_COMMON_EFS_MMGSDI_FEATURE_STATUS_LIST,
                               UIM_COMMON_EFS_ITEM_FILE_TYPE,
                               efs_context,
                               (const char*)&features_status_list[index],
                               sizeof(features_status_list[index]));
  }
} /* mmgsdi_nv_init_features_status_nv_lookup */


/*===========================================================================
FUNCTION MMGSDI_NV_INIT_EXTERNAL_FEATURES_SLOT_ITEMS

DESCRIPTION
  Function used to get the status of slot based external features (i.e. features
  outside of MMGSDI), stored in EFS based NV:
    NV 69687: UIM JCDMA Mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_nv_init_external_features_slot_items(
  uint8  index
)
{
  uim_common_efs_context_type  efs_context        = UIM_COMMON_EFS_CONTEXT_MAX;
  uint8                        jcdma_mode         = 0;

  if (index >= sizeof(mmgsdi_external_features_status_list)/sizeof(mmgsdi_external_features_status_list[0]))
  {
    return;
  }

  efs_context = mmgsdi_util_get_common_efs_item_context(index);
  if (UIM_COMMON_EFS_CONTEXT_MAX == efs_context)
  {
    return;
  }

  /* Read UIM JCDMA Mode (NV 69687) from EFS, Default value: DISABLED */
  mmgsdi_external_features_status_list[index].jcdma_service_status = MMGSDI_FEATURE_DISABLED;
  if (uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_JCDMA_MODE,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          &jcdma_mode,
                          sizeof(jcdma_mode)) == UIM_COMMON_EFS_SUCCESS)
  {
    if (jcdma_mode != 0)
    {
      mmgsdi_external_features_status_list[index].jcdma_service_status = MMGSDI_FEATURE_ENABLED;
      /* If enabled, initialize JCFMA info */
      (void)mmgsdi_jcdma_init(index);
    }
  }
  UIM_MSG_HIGH_2("jcdma_mode=0x%x, jcdma_service_status=0x%x",
                 jcdma_mode,
                 mmgsdi_external_features_status_list[index].jcdma_service_status);
}/* mmgsdi_nv_init_external_features_slot_items */


/*===========================================================================
FUNCTION MMGSDI_NV_INIT_EXTERNAL_FEATURES_SESSION_ITEMS

DESCRIPTION
  Function used to get the status of session based external features (i.e.
  features outside of MMGSDI), stored in EFS based NV:
    NV 70361: CSG SUPPORT CONFIGURATION
    NV 67218: IMS ENABLE
    NV 67210: RPM INFO (Need further confirmation from MM folks whether this is
              ME based or session based NV item)
    NV 66048: VOICE DOMAIN PREF

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_nv_init_external_features_session_items(
  uint8                        index
)
{
  uim_common_efs_context_type   efs_context        = UIM_COMMON_EFS_CONTEXT_MAX;
#ifdef FEATURE_FEMTO_CSG
  sys_csg_support_e_type        csg_support_status = SYS_CSG_SUPPORT_NONE;
#endif /* FEATURE_FEMTO_CSG */
  uint8                         ims_support_status = 0;
  sys_voice_domain_pref_e_type  voice_domain_pref  = SYS_VOICE_DOMAIN_PREF_NONE;
  boolean                       cdma_less_device   = FALSE;

  PACKED struct PACKED_POST
  {
    uint8                       is_rpm_enabled;
    uint8                       app_max_num_reset;
    uint8                       app_reset_counter;
    uint32                      app_reset_timer;
    uint32                      app_reset_block_leak_rate_timer;
    uint8                       average_reset_time;
    uint32                      leak_rate_timer;
  } rpm_info;

  if (index >= sizeof(mmgsdi_external_features_status_list)/sizeof(mmgsdi_external_features_status_list[0]))
  {
    return;
  }

  efs_context = mmgsdi_util_get_common_efs_item_context(index);
  if (UIM_COMMON_EFS_CONTEXT_MAX == efs_context)
  {
    return;
  }

#ifdef FEATURE_FEMTO_CSG
#ifdef FEATURE_DISABLE_CSG_BYDEFAULT
  /* In this case, default value is support disabled */
  mmgsdi_external_features_status_list[index].nas_cfg_feature_csg = MMGSDI_FEATURE_DISABLED;
#else
  /* In this case, default value is support enabled */
  mmgsdi_external_features_status_list[index].nas_cfg_feature_csg = MMGSDI_FEATURE_ENABLED;
#endif /* FEATURE_DISABLE_CSG_BYDEFAULT */

  /* Read CSG SUPPORT CONFIGURATION (NV 70361) from EFS */
  if (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_CSG_SUPPORT_CONFIG,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)&csg_support_status,
                          sizeof(csg_support_status)) ==
      UIM_COMMON_EFS_SUCCESS)
  {
    /* Retrieve and store the status of the CSG feature */
    if(csg_support_status == SYS_CSG_SUPPORT_DISABLED)
    {
      mmgsdi_external_features_status_list[index].nas_cfg_feature_csg =
        MMGSDI_FEATURE_DISABLED;
    }
    else
    {
      mmgsdi_external_features_status_list[index].nas_cfg_feature_csg =
        MMGSDI_FEATURE_ENABLED;
    }
  }
#else
  mmgsdi_external_features_status_list[index].nas_cfg_feature_csg =
    MMGSDI_FEATURE_DISABLED;
#endif /* FEATURE_FEMTO_CSG */

  /* Read IMS ENABLE (NV 67218) from EFS, Default value: DISABLED */
  mmgsdi_external_features_status_list[index].ims_cfg_feature_ims_support =
    MMGSDI_FEATURE_DISABLED;
  if (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_IMS_ENABLE,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)&ims_support_status,
                          sizeof(ims_support_status)) ==
      UIM_COMMON_EFS_SUCCESS)
  {
    /* Retrieve and store the status of the IMS support
       0 - Disabled
       1 - Enabled
       2 - Proprietary IMS support */
    if(ims_support_status != 0)
    {
      mmgsdi_external_features_status_list[index].ims_cfg_feature_ims_support =
        MMGSDI_FEATURE_ENABLED;
    }
  }

  /* Read RPM info (NV 67210) from EFS, Default value: DISABLED */
  mmgsdi_external_features_status_list[index].rpm_enabled = MMGSDI_FEATURE_DISABLED;
  if (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_RPM_INFO,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)&rpm_info,
                          sizeof(rpm_info)) ==
      UIM_COMMON_EFS_SUCCESS)
  {
    if(rpm_info.is_rpm_enabled)
    {
      mmgsdi_external_features_status_list[index].rpm_enabled =
        MMGSDI_FEATURE_ENABLED;
    }
  }

  /* Read VOICE_DOMAIN_PREF (NV 66048) from EFS, Default value: DISABLED */
  mmgsdi_external_features_status_list[index].volte_using_ims = MMGSDI_FEATURE_DISABLED;
  if (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_VOICE_DOMAIN_PREF,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)&voice_domain_pref,
                          sizeof(voice_domain_pref)) ==
      UIM_COMMON_EFS_SUCCESS)
  {
    /* Retrieve and store the status of the VOLTE USING IMS support
       Enabled if the value is one of the following:
         SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY
         SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED
         SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED
     */
    if (SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY      == voice_domain_pref ||
        SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED     == voice_domain_pref ||
        SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED == voice_domain_pref)
    {
      mmgsdi_external_features_status_list[index].volte_using_ims =
        MMGSDI_FEATURE_ENABLED;
    }
  }

  /* Read CDMALess info (NV 73732) from EFS, Default value: DISABLED */
  mmgsdi_external_features_status_list[index].cdma_less_device = MMGSDI_FEATURE_DISABLED;
  if (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_CDMA_LESS_DEVICE,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)&cdma_less_device,
                          sizeof(cdma_less_device)) ==
      UIM_COMMON_EFS_SUCCESS)
  {
    if(cdma_less_device)
    {
      mmgsdi_external_features_status_list[index].cdma_less_device =
        MMGSDI_FEATURE_ENABLED;
    }
  }
  UIM_MSG_MED_3("nas_cfg_feature_csg=0x%x, rpm_enabled=0x%x, ims_cfg_feature_ims_support=0x%x",
                mmgsdi_external_features_status_list[index].nas_cfg_feature_csg,
                mmgsdi_external_features_status_list[index].rpm_enabled,
                mmgsdi_external_features_status_list[index].ims_cfg_feature_ims_support);

  UIM_MSG_MED_2("volte_using_ims=0x%x, cdma_less_device=0x%x",
                mmgsdi_external_features_status_list[index].volte_using_ims,
                mmgsdi_external_features_status_list[index].cdma_less_device);
}/* mmgsdi_nv_init_external_features_session_items */


/*===========================================================================
FUNCTION MMGSDI_NV_INIT_EXTERNAL_FEATURES_STATUS_NV_LOOKUP

DESCRIPTION
  Function used to get the status of external features (i.e. features
  outside of MMGSDI), stored in EFS based NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_nv_init_external_features_status_nv_lookup(
  uint8     index)
{
  /* Start off with the global struct initialised to 0 */
  memset(&mmgsdi_external_features_status_list[index], 0x00,
         sizeof(mmgsdi_external_features_status_list[index]));

  /* Init external features depending on slot based NV items */
  mmgsdi_nv_init_external_features_slot_items(index);

  /* Init external features depending on session based NV items */
  mmgsdi_nv_init_external_features_session_items(index);
} /* mmgsdi_nv_init_external_features_status_nv_lookup */


/*===========================================================================
FUNCTION MMGSDI_NV_INIT_ENS_SLOT_ENABLED

DESCRIPTION
  Function used to get the status of ENS slot enabled feature stored
  in EFS for each slot.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_nv_init_ens_slot_enabled(
  uint8     slot_index)
{
  uim_common_efs_context_type  efs_context;
  uint8                        ens_enabled = 0;

  efs_context = mmgsdi_util_get_common_efs_item_context(slot_index);
  if (efs_context == UIM_COMMON_EFS_CONTEXT_MAX)
  {
    return;
  }

  /* Read the ENS Slot Enabled file from EFS */
  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_ENS_SLOT_ENABLED,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)&ens_enabled,
                          sizeof(ens_enabled)) ==
      UIM_COMMON_EFS_SUCCESS)
  {
    /* Retrieve and store the status of the ENS Slot Enabled feature */
    if(ens_enabled == 0)
    {
      mmgsdi_ens_slot_enabled[slot_index] = MMGSDI_FEATURE_DISABLED;
    }
    else
    {
      mmgsdi_ens_slot_enabled[slot_index] = MMGSDI_FEATURE_ENABLED;
    }

    UIM_MSG_MED_1("Successfully read the ENS slot enabled status 0x%x from EFS",
                  mmgsdi_ens_slot_enabled[slot_index]);
  }
  else
  {
    /* Fall back to the legacy ENS NV item if the EFS item is not present */
    if (mmgsdi_is_ens_enabled_in_nv() && (efs_context == UIM_COMMON_EFS_CONTEXT_0) )
    {
      mmgsdi_ens_slot_enabled[slot_index] = MMGSDI_FEATURE_ENABLED;
    }
    else
    {
      mmgsdi_ens_slot_enabled[slot_index] = MMGSDI_FEATURE_DISABLED;
    }
    UIM_MSG_HIGH_0("Failed to read ENS slot enabled from EFS, setting status to DISABLED");
  }
} /* mmgsdi_nv_init_ens_slot_enabled */


/*===========================================================================
FUNCTION MMGSDI_NV_IS_ENS_ENABLED

DESCRIPTION
  Function used to get the status of ENS slot enabled feature for the given
  slot

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_nv_is_ens_enabled(
  mmgsdi_slot_id_enum_type     slot)
{
  uint8 slot_index = 0;

  if (mmgsdi_util_get_slot_index(slot, &slot_index) != MMGSDI_SUCCESS)
  {
    return FALSE;
  }

  return mmgsdi_ens_slot_enabled[slot_index];
} /* mmgsdi_nv_is_ens_enabled */


/*===========================================================================
FUNCTION MMGSDI_NV_GET_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of MMGSDI features listed in enum
  mmgsdi_nv_features_enum_type  as defined by the NVITEM 67211 -
  "MMGSDI_FEATURES_STATUS_LIST".

  Certain features in this list are operator-specific (refer to
  mmgsdi_nv_is_feature_item_slot_based() for such features) which means those
  features can be disabled or enabled based on which card is present in
  that slot. For querying status of such items, the slot id needs to be
  passed as an argument to this function.
  For querying the status of all other items which are ME-specific and not
  card-dependent,  MMGSDI_SLOT_1 should be passed.

DEPENDENCIES
  Minimum QXDM version required to be able to read/write this NVITEM is
  QXDM 3.12.997.

RETURN VALUE
  mmgsdi_feature_status_enum

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_feature_status_enum_type mmgsdi_nv_get_feature_status(
  mmgsdi_nv_features_enum_type nv_feature,
  mmgsdi_nv_context_type       nv_context)
{
  mmgsdi_feature_status_enum_type ret_value = MMGSDI_FEATURE_DISABLED;

  if(nv_feature >= MMGSDI_MAX_ITEMS_IN_FEATURE_NVITEM)
  {
    return MMGSDI_FEATURE_DISABLED;
  }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/

  /* Device specific items are accessed from primary context,
        if the context is DEVICE SPECIFIC, convert it into PRIMARY CONTEXT */
  if(nv_context == MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
  {
    nv_context = MMGSDI_NV_CONTEXT_PRIMARY;
  }

  if(nv_context >= MMGSDI_MAX_NUM_SLOTS)
  {
    return MMGSDI_FEATURE_DISABLED;
  }

  ret_value = features_status_list[nv_context].item[nv_feature];

  if(features_list_init_table[nv_feature].default_value != ret_value)
  {
    UIM_MSG_HIGH_2("Status of feature 0x%x is 0x%x", nv_feature, ret_value);
  }

  return ret_value;
} /* mmgsdi_nv_get_feature_status */


/*===========================================================================
FUNCTION MMGSDI_NV_GET_EXTERNAL_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of external features listed in the enum:
  mmgsdi_nv_external_features_enum.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_feature_status_enum

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_feature_status_enum_type mmgsdi_nv_get_external_feature_status(
  mmgsdi_nv_external_features_enum nv_feature,
  mmgsdi_nv_context_type           index)
{
  mmgsdi_feature_status_enum_type ret_value = MMGSDI_FEATURE_DISABLED;

  if (index >= sizeof(mmgsdi_external_features_status_list)/sizeof(mmgsdi_external_features_status_list[0]))
  {
    return MMGSDI_FEATURE_DISABLED;
  }

  switch(nv_feature)
  {
    case MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG:
      ret_value = mmgsdi_external_features_status_list[index].nas_cfg_feature_csg;
      break;

    case MMGSDI_EXT_FEAT_IMS_CFG_FEATURE_IMS_SUPPORT:
      ret_value = mmgsdi_external_features_status_list[index].ims_cfg_feature_ims_support;
      break;

    case MMGSDI_EXT_FEAT_RPM_ENABLED:
      ret_value = mmgsdi_external_features_status_list[index].rpm_enabled;
      break;

    case MMGSDI_EXT_JCDMA_SERVICE_STATUS:
      ret_value = mmgsdi_external_features_status_list[index].jcdma_service_status;
      break;

    case MMGSDI_EXT_FEAT_VOLTE_USING_IMS:
      ret_value = mmgsdi_external_features_status_list[index].volte_using_ims;
      break;

    case MMGSDI_EXT_FEAT_CDMA_LESS_DEVICE:
      ret_value = mmgsdi_external_features_status_list[index].cdma_less_device;
      break;

    default:
      UIM_MSG_MED_1("Invalid external feature 0x%x", nv_feature);
      return ret_value;
  }

  UIM_MSG_MED_2("Status of external feature 0x%x is 0x%x", nv_feature, ret_value);

  return ret_value;
} /* mmgsdi_nv_get_external_feature_status */


/*===========================================================================
FUNCTION MMGSDI_NV_UPDATE_RPM_ICCID

DESCRIPTION
  Compare the previously stored ICCID to the current ICCID of the card.  If the
  values are different update nv with the new ICCID from the card.
  Note that if the current card has no ICCID or if the ICCID is a non-standard
  ICCID (less than 10 bytes), all zeros (10 bytes) are written to the nv (to
  indicate no/non-standard ICCID).

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  mmgsdi_rpm_iccid_has_changed_slot will be:
    TRUE : if the ICCID is different than the last card inserted in the ME OR
           if the ICCID is zero or a non-standard ICCID
    FALSE: if the ICCID is the same as the last card inserted in the ME
===========================================================================*/
void mmgsdi_nv_update_rpm_iccid (
  mmgsdi_data_type         new_rpm_iccid,
  mmgsdi_slot_id_enum_type mmgsdi_slot
)
{
  uint8                   mmgsdi_rpm_iccid[MMGSDI_MAX_NUM_SLOTS][MMGSDI_ICCID_LEN];
  mmgsdi_slot_data_type  *slot_data_ptr                        = NULL;
  uint8                   slot_index                           = MMGSDI_SLOT_1_INDEX;
  uint8                   mmgsdi_invalid_iccid[MMGSDI_ICCID_LEN];

  UIM_MSG_HIGH_0("UPDATE RPM ICCID");

  memset(mmgsdi_invalid_iccid, 0x00, sizeof(mmgsdi_invalid_iccid));
  memset(mmgsdi_rpm_iccid, 0x00, sizeof(mmgsdi_rpm_iccid));

  if((new_rpm_iccid.data_ptr == NULL) ||
     (new_rpm_iccid.data_len != MMGSDI_ICCID_LEN))
  {
     /* Incase ICCID EF is not present on the card or there is some other
       issue reading ICCID from card (or ICCID reported by card in a non-
       standard ICCID) and we reached here with a NULL iccid or non-standard
       iccid, make sure we write 0x00s (10 bytes) as ICCID to the RPM EFS.
       This is to make sure we can treat this card as a new card if
       no/non-standard ICCID is present on the card (or there are issues
       reading ICCID from the card). */
    new_rpm_iccid.data_len = sizeof(mmgsdi_invalid_iccid);
    new_rpm_iccid.data_ptr = mmgsdi_invalid_iccid;
  }

  (void) mcfg_fs_read(MMGSDI_RPM_ICCID_FILE, (void *)mmgsdi_rpm_iccid,
                      sizeof(mmgsdi_rpm_iccid), MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE);

  /* Enter crit sect to protect global */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT

  do
  {
    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(mmgsdi_slot);
    if(slot_data_ptr == NULL)
    {
      break;
    }

    if(MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(mmgsdi_slot, &slot_index))
    {
      break;
    }

    if(memcmp(mmgsdi_rpm_iccid[slot_index],
              new_rpm_iccid.data_ptr,
              MMGSDI_ICCID_LEN))
    {
      slot_data_ptr->mmgsdi_rpm_iccid_has_changed = TRUE;
      mmgsdi_memscpy(mmgsdi_rpm_iccid[slot_index],
                     sizeof(mmgsdi_rpm_iccid[slot_index]),
                     new_rpm_iccid.data_ptr,
                     MMGSDI_ICCID_LEN);

      (void) mcfg_fs_write(MMGSDI_RPM_ICCID_FILE, (void *)mmgsdi_rpm_iccid,
                           sizeof(mmgsdi_rpm_iccid), MCFG_FS_O_CREAT|MCFG_FS_O_RDWR|MCFG_FS_O_AUTODIR|MCFG_FS_O_TRUNC,
                           0777, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE);
    }
    else if(!memcmp(mmgsdi_rpm_iccid[slot_index],
                    mmgsdi_invalid_iccid,
                    MMGSDI_ICCID_LEN))
    {
      /* We reach here if EFS file already contains an invalid (all zeros) ICCID
         and current ICCID is also a non-standard one (or there is no ICCID on the
         card). In this case, since the EFS file already contains an invalid ICCID,
         no need to update the EFS file again with an invalid ICCID (all zeros) -
         simply update the global indicating that the ICCID has changed.
         Yes, this does mean that we would be indicating that the ICCID has changed
         even if the same card having no-ICCID that was inserted in the device the
         last time is re-inserted into the device... */
      slot_data_ptr->mmgsdi_rpm_iccid_has_changed = TRUE;
    }
    else
    {
      slot_data_ptr->mmgsdi_rpm_iccid_has_changed = FALSE;
    }

    if(slot_data_ptr->mmgsdi_rpm_iccid_has_changed)
    {
      UIM_MSG_HIGH_1("RPM ICCID has changed for SLOT 0x%x", mmgsdi_slot);
    }
  } while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT
} /* mmgsdi_nv_update_rpm_iccid */


/*===========================================================================
FUNCTION MMGSDI_NV_GET_RPM_ICCID_HAS_CHANGED

DESCRIPTION
  Return whether the ICCID is different than the last ICCID read from the card

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  Boolean - TRUE : if the ICCID is changed
            FALSE: if the ICCID is the same

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_nv_get_rpm_iccid_has_changed (
  mmgsdi_slot_id_enum_type mmgsdi_slot
)
{
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;
  boolean                 rpm_iccid_has_changed = FALSE;

  /* Protect access to slot data table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(mmgsdi_slot);
  if(slot_data_ptr == NULL)
  {
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT
    return FALSE;
  }
  rpm_iccid_has_changed = slot_data_ptr->mmgsdi_rpm_iccid_has_changed;

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT
  return rpm_iccid_has_changed;
} /* mmgsdi_nv_get_rpm_iccid_has_changed */


/*===========================================================================
FUNCTION MMGSDI_NV_SEND_RTRE_COMMAND_TO_NV

DESCRIPTION
  Sends RTRE configuration to NV, bypassing CM.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void mmgsdi_nv_send_rtre_command_to_nv(
  const nv_item_type *data_ptr
)
{
  nv_cmd_type             *mmgsdi_nv_cmd_ptr  = NULL;
  mmgsdi_return_enum_type  mmgsdi_status      = MMGSDI_ERROR;
  nv_item_type            *cmd_data_ptr       = NULL;
  int32                    mmgsdi_nv_cmd_size = 0;

  if(data_ptr == NULL)
  {
    return;
  }

  /* Allocate memory for the NV command */
  mmgsdi_util_cal_align_size(sizeof(nv_cmd_type), &mmgsdi_nv_cmd_size);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_nv_cmd_ptr,
                                     (mmgsdi_nv_cmd_size + sizeof(nv_item_type)));

  if(mmgsdi_nv_cmd_ptr == NULL)
  {
    return;
  }

  /* Copy data to write */
  cmd_data_ptr = (nv_item_type*)((uint8*)mmgsdi_nv_cmd_ptr + mmgsdi_nv_cmd_size);
  mmgsdi_memscpy(cmd_data_ptr,
                 sizeof(nv_item_type),
                 data_ptr,
                 sizeof(nv_item_type));

  /* Notify main MMGSDI task when done */
  mmgsdi_nv_cmd_ptr->tcb_ptr    = UIM_GSDI_TCB;
  mmgsdi_nv_cmd_ptr->sigs       = MMGSDI_NV_WRITE_SIG;
  mmgsdi_nv_cmd_ptr->done_q_ptr = &mmgsdi_nv_q;

  /* Item to write */
  mmgsdi_nv_cmd_ptr->item = NV_RTRE_CONFIG_I;
  mmgsdi_nv_cmd_ptr->cmd  = NV_RTRE_OP_CONFIG_F;
  mmgsdi_nv_cmd_ptr->data_ptr = cmd_data_ptr;

  /* Send Command to NV */
  nv_cmd(mmgsdi_nv_cmd_ptr);

  mmgsdi_status = mmgsdi_nv_put_nv_item_in_cache(
                    NV_RTRE_CONFIG_I,
                    data_ptr,
                    NV_DONE_S,
                    MMGSDI_NV_CONTEXT_PRIMARY);

  UIM_MSG_HIGH_2("RTRE CONFIG NV Status = 0x%x, Cache write status: 0x%x",
                 mmgsdi_nv_cmd_ptr->status, mmgsdi_status);
} /* mmgsdi_nv_send_rtre_command_to_nv */


/*===========================================================================
FUNCTION MMGSDI_NV_INIT_ONCHIP_AUTO_ACTIVATION_DATA

DESCRIPTION
  Reads the EFS item to figure out if the OnChip should be enabled on boot up
  or not. If it dictates that OnChip be auto activated on boot up, the EFS
  item contents are validated and are read into the MMGSDI global. This is done
  for all the instances of this EFS item (assuming the corresponding slot index
  is enabled via NV 70210.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_nv_init_onchip_auto_activation_data (
  uint8 index
)
{
  int                                      efs_data_len         = 0;
  mmgsdi_app_enum_type                     mmgsdi_app_type      = MMGSDI_APP_NONE;
  boolean                                  auto_act_enabled     = FALSE;
  uint8                                    efs_slot_index       = MMGSDI_SLOT_1_INDEX;
  uim_common_efs_context_type              efs_context          = UIM_COMMON_EFS_CONTEXT_MAX;

  struct
  {
    uint8   auto_activation_enabled;
    uint8   app_type;
    uint8   slot_id;
    uint8   rfu[13];
  } onchip_auto_act_data;

  efs_context = mmgsdi_util_get_common_efs_item_context(index);
  if (efs_context == UIM_COMMON_EFS_CONTEXT_MAX)
  {
    return;
  }

  if(mmgsdi_generic_data_ptr == NULL)
  {
    return;
  }

  memset(&onchip_auto_act_data, 0x00, sizeof(onchip_auto_act_data));

  /* Lets parse the EFS data; we copy it to our global only if it is valid */
  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_ONCHIP_SUPPORT,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)&onchip_auto_act_data,
                          sizeof(onchip_auto_act_data)) !=
        UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid onchip EFS item len:0x%x", efs_data_len);
    return;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(onchip_auto_act_data.slot_id,
                                                  &efs_slot_index) ||
     index != efs_slot_index)
  {
    return;
  }

  /* Validate the input that indicates whether onchip auto activation is
     enabled or not.  */
  switch(onchip_auto_act_data.auto_activation_enabled)
  {
    case 1:
      auto_act_enabled = TRUE;
      break;
    case 0:
    default:
      /* EFS item suggested that onchip auto-activation should not be performed.
         No need to validate rest of the contents of the EFS item */
      return;
  }

  /* Validate app type from the EFS item. OnChip currently supported only
     for SIM and USIM */
  switch(onchip_auto_act_data.app_type)
  {
    case 1:
      mmgsdi_app_type = MMGSDI_APP_SIM;
      break;
    case 3:
      mmgsdi_app_type = MMGSDI_APP_USIM;
      break;
    default:
      UIM_MSG_ERR_1("Invalid app type mentioned in OnChip EFS item: 0%x",
                    onchip_auto_act_data.app_type);
      return;
  }

  mmgsdi_generic_data_ptr->onchip_act_data[index].auto_activation_enabled =
    auto_act_enabled;
  mmgsdi_generic_data_ptr->onchip_act_data[index].app_type =
    mmgsdi_app_type;
} /* mmgsdi_nv_init_onchip_auto_activation_data */


/*===========================================================================
FUNCTION MMGSDI_NV_SET_DEFAULT_VALUE_OF_ALL_ENS_SUB_FEATURES

DESCRIPTION
  Function used to set the default values of all the ENS sub features
  (controlled by ens_sub_feature_status_list NVITEM), to the
  global mmgsdi_ens_sub_features_status_list.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_nv_set_default_value_of_all_ens_sub_features(
  void
)
{
  memset(&mmgsdi_ens_sub_features_status_list, 0x00, sizeof(mmgsdi_ens_sub_features_status_list));

  /* Update mmgsdi_ens_sub_features_status_list with default values */
  mmgsdi_ens_sub_features_status_list.scan_control_application_support = MMGSDI_FEATURE_ENABLED;
  mmgsdi_ens_sub_features_status_list.nw_selection_menu_control_application_support = MMGSDI_FEATURE_ENABLED;
  mmgsdi_ens_sub_features_status_list.acting_hplmn_rescan_feature_support = MMGSDI_FEATURE_ENABLED;
  mmgsdi_ens_sub_features_status_list.rat_balancing_support = MMGSDI_FEATURE_ENABLED;
} /* mmgsdi_nv_set_default_value_of_all_ens_sub_features */


/*===========================================================================
FUNCTION MMGSDI_NV_INIT_ENS_SUB_FEATURES_STATUS_NV_LOOKUP

DESCRIPTION
  Function used to get the status of ens sub features stored in EFS based NV.
  If it fails to read, it sets the mmgsdi_ens_sub_features_status_list members
  to a default value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_nv_init_ens_sub_features_status_nv_lookup(
  void
)
{
  /* Start off with the global struct initialised to 0 */
  memset(&mmgsdi_ens_sub_features_status_list, 0x00, sizeof(mmgsdi_ens_sub_features_status_list));

  UIM_MSG_HIGH_0("mmgsdi_nv_init_ens_sub_features_status_nv_lookup");

  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_ENS_SUB_FEATURES_STATUS_LIST,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          UIM_COMMON_EFS_DEVICE,
                          (uint8 *) &mmgsdi_ens_sub_features_status_list,
                          sizeof(mmgsdi_ens_sub_features_status_list)) != UIM_COMMON_EFS_SUCCESS)
  {
    /* Updating global mmgsdi_ens_sub_features_status_list with default values */
    mmgsdi_nv_set_default_value_of_all_ens_sub_features();
  }
} /* mmgsdi_nv_init_ens_sub_features_status_nv_lookup */


/*===========================================================================
FUNCTION MMGSDI_NV_GET_ENS_SUB_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of ENS sub features

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_feature_status_enum

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_feature_status_enum_type mmgsdi_nv_get_ens_sub_feature_status(
  mmgsdi_cached_nv_item_enum_type ens_feature
)
{
  mmgsdi_feature_status_enum_type ret_value = MMGSDI_FEATURE_DISABLED;

  switch(ens_feature)
  {
    case MMGSDI_NV_SCAN_CONTROL_APPLICATION_SUPPORT_I:
      ret_value = mmgsdi_ens_sub_features_status_list.scan_control_application_support;
      break;
    case MMGSDI_NV_NETWORK_SELECTION_MENU_CONTROL_APPLICATION_SUPPORT_I:
      ret_value = mmgsdi_ens_sub_features_status_list.nw_selection_menu_control_application_support;
      break;
    case MMGSDI_NV_ACTING_HPLMN_RESCAN_FEATURE_SUPPORT_I:
      ret_value = mmgsdi_ens_sub_features_status_list.acting_hplmn_rescan_feature_support;
      break;
    case MMGSDI_NV_RAT_BALANCING_SUPPORT_I:
      ret_value = mmgsdi_ens_sub_features_status_list.rat_balancing_support;
      break;
    default:
      UIM_MSG_HIGH_1("Invalid Feature 0x%x", ens_feature);
      return ret_value;
  }
  UIM_MSG_HIGH_2("Status of ENS sub feature 0x%x is 0x%x", ens_feature, ret_value);
  return ret_value;
} /* mmgsdi_nv_get_ens_sub_feature_status */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================
FUNCTION MMGSDI_NV_GET_JCDMA_SERVICE_STATUS

DESCRIPTION
  Function to retrieve the value of uim_jcdma_status.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_feature_status_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_feature_status_enum_type mmgsdi_nv_get_jcdma_service_status(
  mmgsdi_slot_id_enum_type slot_id)
{
  mmgsdi_nv_context_type nv_context = MMGSDI_NV_CONTEXT_INVALID;

  nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot_id);

  return mmgsdi_nv_get_external_feature_status(MMGSDI_EXT_JCDMA_SERVICE_STATUS, nv_context);
} /* mmgsdi_nv_get_jcdma_service_status*/


/*===========================================================================
FUNCTION MMGSDI_NV_IS_FEATURE_ITEM_SLOT_BASED

DESCRIPTION
  Function to determine if a feature status item is slot based.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_nv_is_feature_item_slot_based(
  mmgsdi_nv_features_enum_type  feature_status_item)
{

  if(feature_status_item >= MMGSDI_MAX_ITEMS_IN_FEATURE_NVITEM)
  {
    return FALSE;
  }

  return features_list_init_table[feature_status_item].slot_based;

} /* mmgsdi_nv_is_feature_item_slot_based */


/*===========================================================================
FUNCTION MMGSDI_NV_GET_FEATURE_STATUS_FROM_LIST

DESCRIPTION
  Function used to get the status of an MMGSDI feature from the input list

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_feature_status_enum

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_feature_status_enum_type mmgsdi_nv_get_feature_status_from_list(
  mmgsdi_nv_features_enum_type              nv_feature,
  const mmgsdi_features_status_list_type  * features_status_list_ptr)
{
  mmgsdi_feature_status_enum_type ret_value = MMGSDI_FEATURE_DISABLED;

  if (features_status_list_ptr == NULL ||
      nv_feature >= MMGSDI_MAX_ITEMS_IN_FEATURE_NVITEM)
  {
    return MMGSDI_FEATURE_DISABLED;
  }

  ret_value = features_status_list_ptr->item[nv_feature];

  return ret_value;
} /* mmgsdi_nv_get_feature_status_from_list */


/*===========================================================================
FUNCTION MMGSDI_NV_UPDATE_FEATURE_STATUS

DESCRIPTION
  Function used to update the value of MMGSDI Feature (controlled by
  MMGSDI_Feature_Status_list NVITEM) to global features_status_list.

  For operator-specific features, index corresponding to the slot id should
  be passed. For ME-specific features, index 0 should be passed.

DEPENDENCIES

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_nv_update_feature_status(
  mmgsdi_nv_features_enum_type      nv_feature,
  uint8                             index,
  mmgsdi_feature_status_enum_type   feature_status)
{
  MMGSDIUTIL_RETURN_IF_EXCEEDS(index, (MMGSDI_MAX_NUM_SLOTS - 1));

  if (nv_feature >= MMGSDI_MAX_ITEMS_IN_FEATURE_NVITEM)
  {
    return MMGSDI_ERROR;
  }

  features_status_list[index].item[nv_feature] = feature_status;

  return MMGSDI_SUCCESS;
} /* mmgsdi_nv_update_feature_status */


/*===========================================================================
FUNCTION MMGSDI_NV_REFRESH_FEATURE_STATUS_LIST

DESCRIPTION
  Function to refresh the MMGSDI feature status list for the index
  corresponding to the card slot on which refresh is occuring. It also
  restores the values of ME-specific items since they should not be part of
  the NV refresh.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_nv_refresh_feature_status_list(
  uint8    efs_index)
{
  mmgsdi_features_status_list_type   orig_features_list;
  uint8                              feature_item           = 0;
  mmgsdi_feature_status_enum_type    feature_status         = MMGSDI_FEATURE_DISABLED;
  boolean                            commit_to_efs          = FALSE;
  uint8                              num_features_in_build  = 0;

  if (efs_index >= MMGSDI_MAX_NUM_SLOTS)
  {
    return MMGSDI_INCORRECT_PARAMS;;
  }

  /* Backup the feature status EFS item at index 0 to prevent the ME-specific
     items from getting overwritten during the refresh. */
  if (efs_index == MMGSDI_SLOT_1_INDEX)
  {
    memset(&orig_features_list, 0x00, sizeof(mmgsdi_features_status_list_type));
    mmgsdi_memscpy(&orig_features_list,
                   sizeof(orig_features_list),
                   &features_status_list[efs_index],
                   sizeof(features_status_list[efs_index]));
  }

  /* Re-read the feature status list from EFS from the given index. */
  mmgsdi_nv_init_features_status_nv_lookup(efs_index);

  /* No need to restore ME-specific items (which are at index 0) if a non-zero
     index is being refreshed. */
  if (efs_index != MMGSDI_SLOT_1_INDEX)
  {
    return MMGSDI_SUCCESS;
  }

  num_features_in_build = sizeof(features_list_init_table)/sizeof(mmgsdi_features_list_type);

  for (feature_item = 0; feature_item < num_features_in_build; feature_item++)
  {
    /* Do not restore values of slot-specific items. */
    if (mmgsdi_nv_is_feature_item_slot_based((mmgsdi_nv_features_enum_type)feature_item))
    {
      continue;
    }

    /* Read original value of the item and compare it with the value post-refresh. */
    feature_status = mmgsdi_nv_get_feature_status_from_list((mmgsdi_nv_features_enum_type)feature_item,
                                                            &orig_features_list);
    if (feature_status == mmgsdi_nv_get_feature_status((mmgsdi_nv_features_enum_type)feature_item,
                                                       MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC))
    {
      continue;
    }

    if (mmgsdi_nv_update_feature_status((mmgsdi_nv_features_enum_type)feature_item,
                                        efs_index,
                                        feature_status) == MMGSDI_SUCCESS)
    {
      commit_to_efs = TRUE;
    }
  }

  if (commit_to_efs)
  {
    if (uim_common_efs_write(UIM_COMMON_EFS_MMGSDI_FEATURE_STATUS_LIST,
                             UIM_COMMON_EFS_ITEM_FILE_TYPE,
                             UIM_COMMON_EFS_CONTEXT_0,
                             (const char*)&features_status_list[efs_index],
                             sizeof(features_status_list[efs_index])) == UIM_COMMON_EFS_SUCCESS)
    {
      UIM_MSG_HIGH_0("Restored ME-specific features status after NV refresh");
    }
    else
    {
      UIM_MSG_ERR_0("Failed to restore ME-specific features status after NV refresh");
    }
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_nv_refresh_feature_status_list */


/*===========================================================================
FUNCTION MMGSDI_NV_CLEAR_NV_ITEM_CACHE

DESCRIPTION
  Function to clear the NV cache

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_nv_clear_nv_item_cache (
  nv_items_enum_type     item_enum,
  mmgsdi_nv_context_type nv_context
)
{
  uint32 i    = 0x0;
  uint32 mask = 0x1;

  if (nv_context >= MMGSDI_NV_CONTEXTS_MAX ||
      mmgsdi_nv_item_cache_table_ptr[nv_context] == NULL)
  {
    UIM_MSG_ERR_1("nv_context - 0x%x is invalid or mmgsdi_nv_item_cache_table_ptr is NULL",
                  nv_context);
    return;
  }

  UIM_MSG_LOW_2("clear NV item enum 0x%x context 0x%x", item_enum, nv_context);

  for (i=0; i<MMGSDI_NV_ITEM_CACHE_SIZE; i++)
  {
    if (mmgsdi_nv_item_cache_table_ptr[nv_context]->item[i].item_enum == item_enum)
    {
      /* Create bit mask using the index of the item to determine if cached */
      if (mmgsdi_nv_item_cache_table_ptr[nv_context]->is_cached_mask &
          (mask << i))
      {
        mmgsdi_nv_item_cache_table_ptr[nv_context]->is_cached_mask &= (~(mask << i));
        return;
      }
    }
  }

  return;
} /* mmgsdi_nv_clear_nv_item_cache */


/*===========================================================================
FUNCTION MMGSDI_NV_REFRESH_SLOT_ITEMS

DESCRIPTION
  Function to refresh the slot-specific items from NV/EFS at the index
  corresponding to the slot on which refresh is happening.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_nv_refresh_slot_items(
  mcfg_refresh_index_type    refresh_index)
{
  mmgsdi_nv_item_cache_type    mmgsdi_nv_item = {0};
  uim_common_efs_context_type  efs_context    = UIM_COMMON_EFS_CONTEXT_MAX;
  mmgsdi_nv_context_type       nv_efs_index   = MMGSDI_NV_CONTEXT_INVALID;
  uint8                        slot_index     = MMGSDI_SLOT_1_INDEX;

  switch (refresh_index)
  {
    case MCFG_REFRESH_INDEX_0:
      nv_efs_index = MMGSDI_NV_CONTEXT_PRIMARY;
      efs_context  = UIM_COMMON_EFS_CONTEXT_0;
      slot_index   = MMGSDI_SLOT_1_INDEX;
      break;
    case MCFG_REFRESH_INDEX_1:
      nv_efs_index = MMGSDI_NV_CONTEXT_SECONDARY;
      efs_context  = UIM_COMMON_EFS_CONTEXT_1;
      slot_index   = MMGSDI_SLOT_2_INDEX;
      break;
    case MCFG_REFRESH_INDEX_2:
      nv_efs_index = MMGSDI_NV_CONTEXT_TERTIARY;
      efs_context  = UIM_COMMON_EFS_CONTEXT_2;
      slot_index   = MMGSDI_SLOT_3_INDEX;
      break;
    default:
      return MMGSDI_INCORRECT_PARAMS;
  }

  /* Clear the NV cache */
  mmgsdi_nv_clear_nv_item_cache(NV_ENS_ENABLED_I, MMGSDI_NV_CONTEXT_PRIMARY);
  mmgsdi_nv_clear_nv_item_cache(NV_JCDMA_UIM_LOCK_I, nv_efs_index);
  mmgsdi_nv_clear_nv_item_cache(NV_JCDMA_RUIM_ID_I, nv_efs_index);

  mmgsdi_nv_init_ens_slot_enabled(nv_efs_index);

  (void)mmgsdi_nv_refresh_feature_status_list(nv_efs_index);

  (void)mmgsdi_refresh_update_retry_settings(nv_efs_index);

  (void)mmgsdi_nv_get_item(NV_JCDMA_UIM_LOCK_I, &mmgsdi_nv_item, nv_efs_index);
  (void)mmgsdi_nv_get_item(NV_JCDMA_RUIM_ID_I, &mmgsdi_nv_item, nv_efs_index);

  /* Init external features depending on slot based NV items */
  mmgsdi_nv_init_external_features_slot_items(slot_index);

  return MMGSDI_SUCCESS;
} /* mmgsdi_nv_refresh_slot_items */


/*===========================================================================
FUNCTION MMGSDI_NV_REFRESH_SESSION_ITEMS

DESCRIPTION
  Function to refresh the session-specific items from NV/EFS at the index
  corresponding to the slot on which refresh is happening.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_nv_refresh_session_items(
  mcfg_refresh_index_type    sub_index)
{
  mmgsdi_nv_item_cache_type     mmgsdi_nv_item = {0};
  uim_common_efs_context_type   efs_context    = UIM_COMMON_EFS_CONTEXT_0;
  mmgsdi_nv_context_type        nv_efs_index   = MMGSDI_NV_CONTEXT_INVALID;
  uint8                         slot_index     = MMGSDI_SLOT_1_INDEX;

  switch (sub_index)
  {
    case MCFG_REFRESH_INDEX_0:
      nv_efs_index = MMGSDI_NV_CONTEXT_PRIMARY;
      efs_context  = UIM_COMMON_EFS_CONTEXT_0;
      slot_index   = MMGSDI_SLOT_1_INDEX;
      break;
    case MCFG_REFRESH_INDEX_1:
      nv_efs_index = MMGSDI_NV_CONTEXT_SECONDARY;
      efs_context  = UIM_COMMON_EFS_CONTEXT_1;
      slot_index   = MMGSDI_SLOT_2_INDEX;
      break;
    case MCFG_REFRESH_INDEX_2:
      nv_efs_index = MMGSDI_NV_CONTEXT_TERTIARY;
      efs_context  = UIM_COMMON_EFS_CONTEXT_2;
      slot_index   = MMGSDI_SLOT_3_INDEX;
      break;
    default:
      return MMGSDI_INCORRECT_PARAMS;
  }

#ifdef FEATURE_3GPP_CSFB
  mmgsdi_nv_clear_nv_item_cache(NV_SERVICE_DOMAIN_PREF_I, nv_efs_index);
  mmgsdi_nv_clear_nv_item_cache(NV_DISABLE_CM_CALL_TYPE_I, nv_efs_index);

  (void)mmgsdi_nv_get_item(NV_SERVICE_DOMAIN_PREF_I, &mmgsdi_nv_item, nv_efs_index);
  (void)mmgsdi_nv_get_item(NV_DISABLE_CM_CALL_TYPE_I, &mmgsdi_nv_item, nv_efs_index);
#endif/* FEATURE_3GPP_CSFB */

  /* Clear and re cache NV 855 RTRE CONFIG if NV refresh is triggered for SUB0 only */
  if(sub_index == MCFG_REFRESH_INDEX_0)
  {
    mmgsdi_nv_item_cache_type cached_mmgsdi_nv_data    = {0};
    mmgsdi_nv_item_cache_type refreshed_mmgsdi_nv_data = {0};
    nv_stat_enum_type         result                   = NV_NOTALLOC_S;
    nv_stat_enum_type         cached_result            = NV_NOTALLOC_S;

    UIM_MSG_HIGH_1("Refresh of RTRE NV: 0x%x", sub_index);

    /* Get RTRE configuration from Cache*/
    cached_result = mmgsdi_nv_get_item(NV_RTRE_CONFIG_I, &cached_mmgsdi_nv_data,
                      MMGSDI_NV_CONTEXT_PRIMARY);
    
    mmgsdi_nv_clear_nv_item_cache(NV_RTRE_CONFIG_I, MMGSDI_NV_CONTEXT_PRIMARY);

    /* Get RTRE configuration from NV*/
    result = mmgsdi_nv_get_item(NV_RTRE_CONFIG_I, &refreshed_mmgsdi_nv_data,
               MMGSDI_NV_CONTEXT_PRIMARY);
    if(result        != NV_DONE_S ||
       cached_result != NV_DONE_S ||
       (cached_mmgsdi_nv_data.item_value.rtre_config != 
        refreshed_mmgsdi_nv_data.item_value.rtre_config))
    {
      mmgsdi_init_rtre_config();
    }
  }

  /* Init external features depending on session based NV items */
  mmgsdi_nv_init_external_features_session_items(slot_index);

  return MMGSDI_SUCCESS;
} /* mmgsdi_nv_refresh_session_items */


/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_EFS_ITEM_INDEX()

  DESCRIPTION
    Returns the index of the EFS item based on the session id.

  PARAMETERS
    session id
    efs_index_ptr

  RETURN VALUE
    mmgsdi_return_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_efs_item_index(
  mmgsdi_session_id_type    session_id,
  mmgsdi_nv_context_type  * efs_index_ptr
)
{
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type  session_type  = MMGSDI_MAX_SESSION_TYPE_ENUM;

  if (efs_index_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_1X_PROV_PRI_SESSION:
    case MMGSDI_CARD_SESSION_SLOT_1:
      *efs_index_ptr = MMGSDI_NV_CONTEXT_PRIMARY;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_1X_PROV_SEC_SESSION:
    case MMGSDI_CARD_SESSION_SLOT_2:
      *efs_index_ptr = MMGSDI_NV_CONTEXT_SECONDARY;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
    case MMGSDI_1X_PROV_TER_SESSION:
    case MMGSDI_CARD_SESSION_SLOT_3:
      *efs_index_ptr = MMGSDI_NV_CONTEXT_TERTIARY;
      break;
    default:
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_efs_item_index */


/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_EFS_ITEM_INDEX_FOR_SLOT

  DESCRIPTION
    Returns the index of the EFS item based on the slot id.

  PARAMETERS
    slot id

  RETURN VALUE
    mmgsdi_nv_context_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_nv_context_type mmgsdi_util_get_efs_item_index_for_slot(
  mmgsdi_slot_id_enum_type  slot_id
)
{
  switch (slot_id)
  {
    case MMGSDI_SLOT_1:
      return MMGSDI_NV_CONTEXT_PRIMARY;
    case MMGSDI_SLOT_2:
      return MMGSDI_NV_CONTEXT_SECONDARY;
    case MMGSDI_SLOT_3:
      return MMGSDI_NV_CONTEXT_TERTIARY;
    default:
      UIM_MSG_ERR_0("Invalid Slot");
      break;
  }

  return MMGSDI_NV_CONTEXT_INVALID;
} /* mmgsdi_util_get_efs_item_index_for_slot */


/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_COMMON_EFS_ITEM_CONTEXT

  DESCRIPTION
    Returns the index of the common EFS item context based on the index.

  PARAMETERS
    index

  RETURN VALUE
    uim_common_efs_context_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uim_common_efs_context_type mmgsdi_util_get_common_efs_item_context(
  uint8 index
)
{
  switch (index)
  {
    case 0:
      return UIM_COMMON_EFS_CONTEXT_0;
    case 1:
      return UIM_COMMON_EFS_CONTEXT_1;
    case 2:
      return UIM_COMMON_EFS_CONTEXT_2;
    default:
      return UIM_COMMON_EFS_CONTEXT_MAX;
  }
}/*mmgsdi_util_get_common_efs_item_context*/


/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_EFS_ITEM_CONTEXT()

  DESCRIPTION
    Returns the index of the EFS item based on the session id.

  PARAMETERS
    session id
    efs_context_ptr

  RETURN VALUE
    mmgsdi_return_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_efs_item_context(
  mmgsdi_session_id_type        session_id,
  uim_common_efs_context_type  *efs_context_ptr
)
{
  mmgsdi_session_type_enum_type  session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;

  MMGSDIUTIL_RETURN_IF_NULL(efs_context_ptr);

  if (mmgsdi_util_get_session_type(session_id, &session_type, NULL) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_1X_PROV_PRI_SESSION:
    case MMGSDI_CARD_SESSION_SLOT_1:
      *efs_context_ptr = UIM_COMMON_EFS_CONTEXT_0;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_1X_PROV_SEC_SESSION:
    case MMGSDI_CARD_SESSION_SLOT_2:
      *efs_context_ptr = UIM_COMMON_EFS_CONTEXT_1;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
    case MMGSDI_1X_PROV_TER_SESSION:
    case MMGSDI_CARD_SESSION_SLOT_3:
      *efs_context_ptr = UIM_COMMON_EFS_CONTEXT_2;
      break;
    default:
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_efs_item_context */


/* ============================================================================
FUNCTION MMGSDI_NV_GET_TOOLKIT_ENABLED_STATUS

DESCRIPTION
  This function returns the status Toolkit functionality
  enablement (Terminal Profile, FETCH, Envelopes)
  
PARAMETERS
  slot_id

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_feature_status_enum_type: MMGSDI_FEATURE_ENABLED : Toolkit is enabled
                                   MMGSDI_FEATURE_DISABLED: Toolkit is disabled

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_feature_status_enum_type mmgsdi_nv_get_toolkit_enabled_status(
  mmgsdi_slot_id_enum_type    slot_id
)
{
#ifdef FEATURE_GSTK
  /*---------------------------------------------------------------------------
    To store status of Toolkit Enablement for each slot (NV XXXXX).
       is_cached:  TRUE if the NV read and value is cached
       is_enabled: TRUE if Toolkit is enabled
  -------------------------------------------------------------------------------*/
  static struct {
    boolean                         is_cached;
    mmgsdi_feature_status_enum_type is_enabled;
  } mmgsdi_toolkit_enabled_status[MMGSDI_MAX_NUM_SLOTS];
  
  uint8                       slot_index       = 0;
  uim_common_efs_context_type efs_context      = UIM_COMMON_EFS_CONTEXT_MAX;
  uint8                       toolkit_disabled = 0;

  if (mmgsdi_util_get_slot_index(slot_id, &slot_index) != MMGSDI_SUCCESS)
  {
    return MMGSDI_FEATURE_DISABLED;
  }

  /* If Toolkit enablement status is cached... */
  if(!mmgsdi_toolkit_enabled_status[slot_index].is_cached)
  {  
    efs_context = mmgsdi_util_get_common_efs_item_context(slot_index);
    if (efs_context == UIM_COMMON_EFS_CONTEXT_MAX)
    {
      return MMGSDI_FEATURE_DISABLED;
    }

    /* Set the default value of Toolkit Enablement as TRUE */
    mmgsdi_toolkit_enabled_status[slot_index].is_enabled = MMGSDI_FEATURE_ENABLED;
    mmgsdi_toolkit_enabled_status[slot_index].is_cached = TRUE;

    /* Read the Toolkit Disabled file from EFS */
    if (uim_common_efs_read(UIM_COMMON_EFS_GSTK_SLOT_TOOLKIT_DISABLED,
                            UIM_COMMON_EFS_ITEM_FILE_TYPE,
                            efs_context,
                            (uint8 *)&toolkit_disabled,
                            sizeof(toolkit_disabled)) ==
        UIM_COMMON_EFS_SUCCESS)
    {
      if(toolkit_disabled)
      {
        mmgsdi_toolkit_enabled_status[slot_index].is_enabled = MMGSDI_FEATURE_DISABLED;
      }
    }
    
    UIM_MSG_LOW_2("Toolkit enabled status for slot_index 0x%x is 0x%x",
                  slot_index, mmgsdi_toolkit_enabled_status[slot_index].is_enabled);
  }
  return mmgsdi_toolkit_enabled_status[slot_index].is_enabled;
#else
  (void)slot_id;
  return MMGSDI_FEATURE_DISABLED;
#endif /* FEATURE_GSTK */
} /* mmgsdi_nv_get_toolkit_enabled_status */
