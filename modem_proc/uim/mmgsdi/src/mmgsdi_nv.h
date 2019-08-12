#ifndef MMGSDI_NV_H
#define MMGSDI_NV_H
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

mmgsdi_nv_update_rpm_iccid()                - Used to update the last used ICCID

mmgsdi_nv_get_rpm_iccid_has_changed()       - Used to determine if the ICCID has changed

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2003,2011-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_nv.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/02/16   gm      Support for CDMALess Device
07/01/16   shr     Support for NV based Toolkit disablement
05/23/16   dd      Skip Search Req for PB files
04/29/15   hh      Differentiate CSG support for W and LTE for EF-TST
12/03/14   bcho    featurize support to use stored rplmn name across operators
11/06/14   hh      Call GSTK NV API to read GSTK features in NV 65674 
10/14/14   av      Handle appropriately zero ICCID reported by uimdrv
10/12/14   hh      Add support to get Voice Domain Preference
09/09/14   kk      Enhancements in NV handling
09/30/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
07/30/14   ar      Revert FR 20469 changes to align with NAS check-in dates
07/25/14   av      Auto onchip activation for multi slots
07/25/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
07/15/14   yt      Support for NV refresh
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
06/02/14   bcho    New item to decide on blocking of CDMA EPRL access
05/09/14   vv      Deprecate MMGSDI_FEATURE_UICC_RAT_INDICATOR_SUPPORT
04/06/14   am      Added mmgsdi heap alloc clean up logic
03/28/14   ar      Enabled N.A. PLMN comparison feature by default
03/25/14   vdc     Added new item for verifying PIN when PIN is changed
02/27/14   dy      Add default_apn, bip_apn_mapping_tbl, rat_priority NV item
02/25/14   hh      Purge temporary mobile identities during UICC Reset Refresh
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
01/16/14   av      New item for deciding bw card and nv for 3gpd credentials
01/10/14   bcho    Refresh 3G Session Reset as per ETSI spec under NV feature
12/18/13   yt      Skip app termination as part of session deactivation
11/21/13   vv      Halt 3gpp subscription if 3gpp2 perso fails
10/17/13   am      Added new item for plmn comparison in the nvitem 67211
10/08/13   tl      Addition of an RPM info EFS structure type
10/08/13   ar      Support to select CDMA before TP by default
10/04/13   vv      Added NV support for ENS sub-features
09/26/13   av      Enable OnChip on bootup based on EFS item
08/29/13   vv      Update EF-TST to indicate support for IMS
08/20/13   spo     Automatic Subscription provisioning enhancements in UIM
07/29/13   am      Added support to determine CSG support dynamically
06/24/13   vdc     Added support for triple SIM
05/17/13   av      Halt 3gpp2 subscription if 3gpp perso fails
05/15/13   bcho    Added new items to mmgsdi features status nvitem 67211
04/25/13   vdc     Added select_dfcdma to mmgsdi feature status nvitem 67211
04/17/13   tl      Replace Featurization with NV
04/05/13   av      Move UIM tasks to group4 of RCINIT
04/04/13   bcho    Added new items to mmgsdi features status nvitem 67211
03/27/13   spo     Add new items to mmgsdi feature status list
01/03/12   abg     Added API mmgsdi_nv_init_read_and_cache_nv_items to read NV during task-init
12/26/12   av      Do not allow writing to the file that is being refreshed
11/05/12   av      Expand features status list nvitem to control skipping
                   checking security attributes before reading EF
10/31/12   tl      CSIM conformance updating EF MECRP
10/31/12   tl      CSIM conformance updating EF MODEL
10/21/12   av      Update conf file only if necessary
10/16/12   yt      Add new items to mmgsdi feature status list
09/28/12   yt      Update user profile parameters after successful OTASP Commit
03/08/12   av      Added new items to mmgsdi features status nvitem 67211
01/30/12   shr     Use GSTK NV item (65674) to determine if the OTASP IMSI_M
                   enhancement is enabled or not
01/13/12   at      Added NV item caching for NV_DISABLE_CM_CALL_TYPE_I
12/21/11   shr     Legacy GSDI removal updates
11/15/11   nmb     RPM Support
10/10/11   av      Updated default values of MMGSDI Features
09/16/11   av      Replace feature #ifdefs with an NVITEM
07/12/11   at      Added NV item caching for NV_SERVICE_DOMAIN_PREF_I
06/05/11   vs      Updated NV item caching logic to include NV_ESN_ME_I
10/04/10   nmb     Added NV queue to address NV deadlocks
05/13/10   vs      Fix for NV and GSDI deadlock
11/05/03   jar     Added GSDI - NV Interface function for SLOT Preference
                   Reads: gsdi_get_nv_slot_preferences().
11/26/02   jar     Removed RTRE Code
09/14/02   jar     Changed FEATURE_MMGSDI_RTRE to Internal Feature MMGSDI_RTRE.
08/30/02  jar/ck   Removed gsdi_wait_nv.  Added gsdi_send_rtre_command.
                   Added feature MMGSDI_RTRE.
08/18/02   jar     Created gsdi_nv.h
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "nv.h"
#include "mmgsdi.h"
#include "uim_common_efs.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/* The following NV items are currently read and written to by MMGSDI. If this
   changes then the cache size MMGSDI_NV_ITEM_CACHE_SIZE and the MMGSDI NV
   cache initialization routine must be updated appropriately. The items are:
   NV_GPRS_ANITE_GCF_I
   NV_UIM_CDMA_PREF_SLOT_I
   NV_UIM_GSM_PREF_SLOT_I
   NV_RTRE_CONFIG_I
   NV_MMGSDI_ME_CONFIG_PARAM_I
   NV_UIM_SELECT_DEFAULT_USIM_APP_I
   NV_MEID_ME_I
   NV_UE_IMEI_I
   NV_ENS_ENABLED_I
   NV_HOMEZONE_ENABLED_I
   NV_FTM_MODE_I
   NV_ESN_ME_I
   NV_SCM_I
   NV_MOB_CAI_REV_I
   NV_JCDMA_UIM_LOCK_I
   NV_JCDMA_RUIM_ID_I
   NV_SERVICE_DOMAIN_PREF_I
   NV_DISABLE_CM_CALL_TYPE_I */
#define MMGSDI_NV_ITEM_CACHE_SIZE       0x12

#define MMGSDI_NV_CONTEXTS_MAX 0x03

#ifndef FEATURE_UIM_TEST_FRAMEWORK
#define MMGSDI_BUILTIN_PLMN_LIST_FILE            "/nv/item_files/modem/uim/mmgsdi/builtin_plmn_list"
#define MMGSDI_NV_EFS_PATH                       "/nv/item_files/modem/uim/mmgsdi"
#else
#error code not present
#endif

/*---------------------------------------------------------------------------
  MMGSDI_FEATURES_STATUS_LIST (67211) NVITEM related defines
---------------------------------------------------------------------------*/
/* Max possible feature-elements in FEATURES_STATUS_LIST_TYPE NVITEM struct */
#define MMGSDI_MAX_ITEMS_IN_FEATURE_NVITEM           63

typedef struct {
  nv_items_enum_type  item_enum;
  union {
    boolean                                          gprs_anite_gcf;
    nv_uim_slot_type                                 uim_cdma_pref_slot;
    nv_uim_slot_type                                 uim_gsm_pref_slot;
    nv_rtre_configuration_type                       rtre_config;
    uint32                                           mmgsdi_me_config_param;
    boolean                                          uim_select_default_usim_app;
    qword                                            meid;
    nv_ue_imei_type                                  ue_imei;
    boolean                                          ens_enabled;
    boolean                                          homezone_enabled;
    byte                                             ftm_mode;
    nv_esn_type                                      esn;
    byte                                             scm;
    byte                                             mob_cai_rev;
    nv_jcdma_ruimid                                  jcdma_ruim_id;
    nv_jcdma_uim_lock_data                           jcdma_uim_lock;
    nv_srv_domain_pref_type                          service_domain_pref;
    uint64                                           disable_cm_call_type;
  }item_value;
  nv_stat_enum_type   item_status;
} mmgsdi_nv_item_cache_type;

typedef struct {
  mmgsdi_nv_item_cache_type  item[MMGSDI_NV_ITEM_CACHE_SIZE];
  uint32                     is_cached_mask;
} mmgsdi_nv_item_cache_table_type;

/* The subsequent enums and struct are for the EFS-based NV ITEM
   MMGSDI_FEATURES_STATUS_LIST_TYPE */

/*=============================================================================
  ENUM:   MMGSDI_NV_FEATURES_ENUM_TYPE
=======================================================================*/
typedef enum {
  MMGSDI_FEATURE_ABSENT_MANDATORY_FILES_INIT = 0,
  MMGSDI_FEATURE_ECC_NOT_PRESENT_SUPPORT,
  MMGSDI_FEATURE_SPECIAL_USIM_ARR,
  MMGSDI_FEATURE_DEPRECATED_1,
  MMGSDI_FEATURE_CDMA_DF_SELECT_BEFORE_TP,
  MMGSDI_FEATURE_CHANGE_INST_CLASS,
  MMGSDI_FEATURE_CARD_SLOT_CHECK,
  MMGSDI_FEATURE_TMSI_PLMN_CHECK,
  MMGSDI_FEATURE_IGNORE_INIT_ERROR,
  MMGSDI_FEATURE_ALLOW_SEND_APDU_ONLY,

  /* 10 */
  MMGSDI_FEATURE_PREF_MODE_NOT_FROM_RUIM,
  MMGSDI_FEATURE_RUIM_ALLOW_ESN_FAIL,
  MMGSDI_FEATURE_RUIM_ZERO_ESN_FAIL,
  MMGSDI_FEATURE_UIM_RUIM_SUPPORT_SCI,
  MMGSDI_FEATURE_UIM_AN_HRPD_FALLBACK,
  MMGSDI_FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND,
  MMGSDI_FEATURE_PERSONALIZATION_SLOTS_LOCKED_ON_SLOT1,
  MMGSDI_FEATURE_CSIM_UPP_UPDATE_AFTER_OTASP,
  MMGSDI_FEATURE_ALLOW_ACCESS_BEFORE_APP_READY,
  MMGSDI_FEATURE_SKIP_UICC_SECURITY_ATTR_CHECK_BEFORE_READ,

  /*20 */
  MMGSDI_FEATURE_BLOCK_WRITES_TO_REFRESH_FILES,
  MMGSDI_FEATURE_SE13_TABLE_LOOKUP_GET_FIRST_GSMA_MATCH,
  MMGSDI_FEATURE_NO_PERSONALIZATION_ON_ADDITIONAL_SLOTS,
  MMGSDI_FEATURE_MMGSDI_UNBLOCK_PIN_HANDLE_INVALID_SW,
  MMGSDI_FEATURE_UIM_GSM_DCS_1800,
  MMGSDI_FEATURE_DEPRECATED_2,
  MMGSDI_FEATURE_EXPLICIT_SELECT_DFCDMA_BEFORE_TELECOM_ADN_READ,
  MMGSDI_FEATURE_OPL_PNN_LOOKUP_FOR_NON_RPLMN,
  MMGSDI_FEATURE_HALT_3GPP2_SUBS_IF_3GPP_PERSO_FAILS,
  MMGSDI_FEATURE_MULTISIM_AUTO_PROVISIONING,

  /* 30 */
  MMGSDI_FEATURE_NORTH_AMERICAN_PLMN_COMPARISON,
  MMGSDI_FEATURE_HALT_3GPP_SUBS_IF_3GPP2_PERSO_FAILS,
  MMGSDI_FEATURE_SKIP_PROV_APP_TERMINATION,
  MMGSDI_FEATURE_REFRESH_3G_SESSION_RESET_PER_ETSI_SPEC,
  MMGSDI_FEATURE_USE_3GPD_CREDENTIALS_FROM_NV,
  MMGSDI_FEATURE_USE_SFI,
  MMGSDI_FEATURE_OPLMN_LIST_CONFIGURABLE_OTA,
  MMGSDI_FEATURE_PURGE_TEMP_IDENTITIES,
  MMGSDI_FEATURE_VERIFY_PIN_AFTER_CHANGE,
  MMGSDI_FEATURE_BLOCK_RUIM_EPRL_ACCESS,

  /*40*/
  MMGSDI_FEATURE_SUPPORT_EXTENDED_FPLMN_ICC,
  MMGSDI_FEATURE_USE_STORED_RPLMN_NAME_ACROSS_OPERATORS,
  MMGSDI_FEATURE_SKIP_SEARCH_FOR_PB_FILES
} mmgsdi_nv_features_enum_type;

/*=============================================================================
  ENUM:   MMGSDI_FEATURE_STATUS_ENUM_TYPE
=======================================================================*/
typedef enum {
  MMGSDI_FEATURE_DISABLED = 0, /* Feature disabled */
  MMGSDI_FEATURE_ENABLED  = 1  /* Feature enabled */
} mmgsdi_feature_status_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FEATURES_STATUS_LIST_TYPE

   DESCRIPTION:
     Data Structure to hold status of mmgsdi features as it reads it from
     efs NVITEM "MMGSDI_FEATURES_STATUS_LIST" (67211)
     * Size of this struct should remain the SAME at all times.
     * "reserved_for_future_use" array items will be hidden from QXDM users at
       all times (hence not writable).
     * "version" should always be the FIRST item.
     * "reserved_for_future_use" shold always be the LAST item.
     * "version" is the key here. First implementation of this struct sets it
       to 0.
     * Everytime new mmgsdi item(s) is/are required to be exposed to the
       users via this NVITEM,
       - the item(s) should be added in this struct just before the
         "reserved_for_future_use" item.
       - make sure to decrease the size of "reserved_for_future_use" array by
         the number of items(say, n) added to the struct. That is, increase the
         value of MMGSDI_FEATURES_STATUS_LIST_CURRENT_COUNT by n.
       - increase the value of MMGSDI_FEATURES_STATUS_LIST_CURRENT_VERSION by 1
       - define MMGSDI_FEATURES_STATUS_LIST_VERSIONx_COUNT equal to
         how many ever features/items are added until now (including the ones
         we are currently adding in version x).
       - Update global mmgsdi_features_total_till_version[] by adding an element
         MMGSDI_FEATURES_STATUS_LIST_VERSIONx_COUNT to the array.
         mmgsdi_features_total_till_version[]
       - in mmgsdi_nv_feature_write_default_value(), add case(s) for default
         value of the newly added item(s)/feature(s).
       - in mmgsdi_nv_get_feature_status(), add case(s) for new item(s).
       - add item in enum "mmgsdi_nv_features_enum_type "
       - Notify QXDM team of the modification.
       - In the release notes, mention the new version of this NVITEM.
       - In the release notes, suggest upgrading QXDM to the version to be
         released two weeks from that date (assuming that QXDM would pick our
         updated NVITEM by then). Otherwise, the user won't be able to see and
         update(set) the new entry in the NVITEM. The MMGSDI code would by
         default set the unset entries to their default values.
-------------------------------------------------------------------------------*/
typedef struct {
  uint8 version;
  mmgsdi_feature_status_enum_type   item[MMGSDI_MAX_ITEMS_IN_FEATURE_NVITEM];
} mmgsdi_features_status_list_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_NV_EXTERNAL_FEATURES_ENUM

   DESCRIPTION:
     Enumeration corresponding to external features
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG = 0,
  MMGSDI_EXT_FEAT_IMS_CFG_FEATURE_IMS_SUPPORT,
  MMGSDI_EXT_FEAT_RPM_ENABLED,
  MMGSDI_EXT_JCDMA_SERVICE_STATUS,
  MMGSDI_EXT_FEAT_VOLTE_USING_IMS,
  MMGSDI_EXT_FEAT_CDMA_LESS_DEVICE
} mmgsdi_nv_external_features_enum;

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
extern nv_stat_enum_type mmgsdi_nv_get_item(
  nv_items_enum_type         item,               /* which item */
  mmgsdi_nv_item_cache_type *mmgsdi_nv_data_ptr, /* pointer to space for item */
  mmgsdi_nv_context_type     nv_context          /* Conrtext of the item to access */
);

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
extern nv_stat_enum_type mmgsdi_nv_put_item(
  nv_items_enum_type      item,      /* which item */
  const nv_item_type     *data_ptr,  /* pointer to space for item */
  mmgsdi_nv_context_type  nv_context /* Conrtext of the item to access */
);

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
extern mmgsdi_return_enum_type mmgsdi_nv_item_cache_init (
  void);

/*===========================================================================
FUNCTION MMGSDI_NV_INIT_FEATURES_STATUS_NV_LOOKUP

DESCRIPTION
  Function used to get the status of MMGSDI features as defined by the NVITEM
  "MMGSDI_FEATURES_STATUS_LIST_FILE".
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
  None

RETURN VALUE
  None

SIDE EFFECTS
  "version" item in this NVITEM is writable through QXDM (no QXDM support to
  hide it or grey it out) or QCN. However if what is written explicitly doesn't
  match what's in the build, mmgsdi will change it to what's in the build and
  it could be annoying for the QXDM user to see it change to something else
  (when the user reads the NVITEM next time).
===========================================================================*/
extern void mmgsdi_nv_init_features_status_nv_lookup(
  uint8     index);

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
extern void mmgsdi_nv_init_external_features_status_nv_lookup(
  uint8     index);

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
extern mmgsdi_feature_status_enum_type mmgsdi_nv_get_external_feature_status(
  mmgsdi_nv_external_features_enum nv_feature,
  uint8                            index
);

/*===========================================================================
FUNCTION MMGSDI_NV_UPDATE_RPM_ICCID

DESCRIPTION
  Compare the previously stored ICCID to the current ICCID of the card.  If the
  values are different update nv with the new ICCID from the card.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  rpm_iccid_has_changed_slot will be:
    TRUE : if the ICCID is different than the last card inserted in the ME
    FALSE: if the ICCID is the same as the last card inserted in the ME
===========================================================================*/
void mmgsdi_nv_update_rpm_iccid (
  mmgsdi_data_type         new_rpm_iccid,
  mmgsdi_slot_id_enum_type mmgsdi_slot
);

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
);

/*===========================================================================
FUNCTION MMGSDI_NV_GET_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of MMGSDI features listed in enum
  mmgsdi_nv_features_enum_type  from the NVITEM 67211
  (MMGSDI_FEATURES_STATUS_LIST_FILE).

  Certain features in this list are operator-specific (refer to
  mmgsdi_nv_is_feature_item_slot_based() for such features) which means those
  features can be disabled or enabled based on which card is present in
  that slot. For querying status of such items, the slot id needs to be
  passed as an argument to this function.
  For querying the status of all other items which are ME-specific and not
  card-dependent,  MMGSDI_SLOT_1 should be passed.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_feature_status_enum

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_feature_status_enum_type mmgsdi_nv_get_feature_status(
  mmgsdi_nv_features_enum_type  feature_status,
  mmgsdi_nv_context_type        nv_context
);

/*===========================================================================
FUNCTION MMGSDI_NV_COMPARE_AND_UPDATE_CONF_FILE

DESCRIPTION
  This function attempts to open the file, create it if it does not exist,
  compares its contents against the passed buffer and if the two are different,
  empties the file and writes back the buffer data to the file. It also writes
  the buffer data to the file if file couldn't be read.

PARAMETERS
  file_path_ptr: File path
  buf_ptr:       Buffer to compare against the file data
  buf_len:       Buffer length

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void mmgsdi_nv_compare_and_update_conf_file(
  const char*                file_path_ptr,
  const char*                buf_ptr,
  uint16                     buf_len
);

/*===========================================================================
FUNCTION MMGSDI_NV_CREATE_CONF_FILE

DESCRIPTION
  Function used to create a .conf file in EFS if not present. the .conf file
  is used to specify UIM NV items (except UIMDRV items) that can be backed up
  in a QCN file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_nv_create_conf_file (
  void
);

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
);

/*===========================================================================
FUNCTION MMGSDI_NV_GET_ITEM_FROM_CACHE

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
);

/*===========================================================================
FUNCTION MMGSDI_NV_SEND_RTRE_COMMAND_TO_NV

DESCRIPTION
  Sends RTRE configuration to NV, bypassing CM.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void mmgsdi_nv_send_rtre_command_to_nv (
  const nv_item_type             *data_ptr
);

/*===========================================================================
FUNCTION MMGSDI_NV_INIT_ONCHIP_AUTO_ACTIVATION_DATA

DESCRIPTION
  Reads the EFS item to figure out if the OnChip should be enabled on boot up
  or not. If it dictates that OnChip be auto activated on boot up, the EFS
  item contents are validated and are read into the MMGSDI global.

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
);

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
extern void mmgsdi_nv_init_ens_sub_features_status_nv_lookup(
  void
);

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
);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

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
  uint8     slot_index);

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
  mmgsdi_slot_id_enum_type     slot);


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
  mmgsdi_slot_id_enum_type slot_id);

/*===========================================================================
FUNCTION MMGSDI_NV_REFRESH_SLOT_ITEMS

DESCRIPTION
  Function to refresh the slot-specific items from NV/EFS at the given index.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_nv_refresh_slot_items(
  mcfg_refresh_index_type    refresh_index);

/*===========================================================================
FUNCTION MMGSDI_NV_REFRESH_SESSION_ITEMS

DESCRIPTION
  Function to refresh the session-specific items from NV/EFS at the given
  index.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_nv_refresh_session_items(
  mcfg_refresh_index_type    sub_index);

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
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_EFS_ITEM_INDEX_FOR_SLOT

  DESCRIPTION
    Returns the index of the EFS item based on the slot id.

  PARAMETERS
    slot id
    efs_index_ptr

  RETURN VALUE
    mmgsdi_return_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_nv_context_type mmgsdi_util_get_efs_item_index_for_slot(
  mmgsdi_slot_id_enum_type  slot_id
);

/*===========================================================================
  FUNCTION MMGSDI_NV_CLEAR_NV_ITEM_CACHE

  DESCRIPTION
    Clears the nv cache for the item specified for that context.

  PARAMETERS
    item_enum
    nv_context

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_nv_clear_nv_item_cache (
  nv_items_enum_type     item_enum,
  mmgsdi_nv_context_type nv_context
);

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
);

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
);

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
);
#endif /* MMGSDI_NV_H */
