/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


        M M G S D I   C A C H E - R E L A T E D   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains all the utility functions for accessing MMGSDI
  cache.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2011 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdicache.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
06/19/16   bcho    F3 frequency reduction by conditional logic
05/24/16   ar      Review of macros used by MMGSDI
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/03/16   tkl     F3 log prints cleanup
05/04/16   lm      Add EF SMSCAP to RUIM and CSIM cache list
01/29/16   bcho    Delete Slot, App and EF cache properly when PSM init fails
01/20/16   bcho    Remove compiler warnings
01/07/16   kv      UIM PSM handling updates
01/06/16   bcho    Don't delete attr cache if not re-cached during NAA Refresh
01/06/16   bcho    EF-ECC cache creation from PSM SFS data
11/25/15   ks      Fix compiler warnings
10/12/15   vdc     Add support for potential silent file change notification
09/30/15   ar      Fake the fourth byte of EF AD for buggy card
08/10/15   vdc     Retry read request for non spec compliant cards
07/14/15   ar      Cache complete data for req's with partial read/write data
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
03/24/15   kk      Subs prov EFS error handling
12/09/14   stv     Fix for subscription switch from SIM to USIM
12/02/14   tkl     Added check to persistent cache write
09/29/14   tkl     Added support for persistent cache
09/09/14   kk      Enhancements in NV handling
09/30/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
09/11/14   bcho    Dont cache attributes for files under DF 7F50
09/04/14   ar      Remove NVRUIM cache
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/30/14   ar      Revert FR 20469 changes to align with NAS check-in dates
07/25/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
06/19/14   av      Fix compiler warning
06/16/14   av      Delete common attributes cache at card error
06/12/14   av      Cache mandatory EFs used by NVRUIM for critical 1x NV reads
06/03/14   tl      Remove additional parameter variables
05/11/14   ar      Added NULL check before dereferencing the pointer
04/17/14   av      Enhance MMGSDI attributes cache
04/06/14   am      Added mmgsdi heap alloc clean up logic
04/03/14   am      Fix compile error on SBM due to assert_on_crit_sect_owner
03/04/14   df      Add NULL check to mmgsdi_cache_alloc_and_populate_file_attr_cache
02/24/14   ar      Removed wrapper function mmgsdi_util_alloc()
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
12/16/13   tkl     decouple caching with uim item type
01/15/14   ar      Cached CDMA_RUIM_ID in mmgsdi cache
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/18/13   tl      Prevent initalizing the cache with partial files
12/16/13   vdc     Cache the ECC file contents while initialization
12/11/13   df      Remove unused memory paging feature
12/09/13   vdc     Initialize the total length of ECC entry in the cache table
10/28/13   kb      Add offtarget support to check mutex is owned or not
10/16/13   df      Removing unused functions
10/04/13   vdc     F3 message reduction
10/01/13   vv      Do not perform READ, if EF size or rec length is zero
09/24/13   yt      Remove function mmgsdi_util_is_gsdi_task_context()
09/17/13   vdc     Convert GW / 1X prov cache pointers into array of pointers
08/06/13   vv      Fixed the check that determines if a record exists in cache
06/25/13   av      Add SF EUIMID to 1x cache list
06/24/13   vdc     Added support for triple SIM
06/20/13   at      Update ECC cache init status to MMGSDI_CACHE_INIT only
                   after succesfully reading at least 1 record.
05/28/13   spo     Replaced instances of memcpy with memscpy
05/21/13   av      Assume security attributes per spec if ARR read is skipped
05/20/13   vv      Fixed assignment of incorrect cache table pointer
05/18/13   tl      Fixed dest size param of memscpy in mmgsdi_cache_write_item()
05/17/13   bcho    Don't free ECC cache pointer when any read record fails
05/16/13   vv      Add EF-NASCONFIG to cache
05/16/13   vdc     Replace memcpy with safer version memscpy
05/06/13   spo     Allocate memory for CPHS variables during runtime
04/24/13   vv      Reset CSG eons info in the app. info
04/17/13   tl      Replace Featurization with NV
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/04/13   spo     Porting back TMSI_PLMN check in mmgsdi_cache_write()
02/19/13   av      Caching support for GID1,GID2 and few 1x files
02/04/13   vv      Added mmgsdi_session_read_prl support for RUIM card
10/21/12   av      Added support for new ECC event
10/19/12   spo     Do not update ECC cache when data buffer is not allocated
10/15/12   bcho    CPHS files cache cleared on session activation/deactivation
10/13/12   abg     Add utility to delete the contents of the common cache
10/01/12   vs      Added utility for context check including logic for SGLTE
09/28/12   abg     Fixed for reading EF-ECC record after geting read error status
09/28/12   abg     Updated ERR to MSG_ERROR
09/25/12   av      Skip reading EF-ARR for internal selects
09/05/12   av      Fixed null pointer reference
09/04/12   av      Don't send READ req if get_file_attr returns file_not_found
09/03/12   bcho    OPL-PNN file cache cleared on session activation
03/16/12   yt      Modify references to gsdi_task_tcb
03/05/12   bcho    File cache critical section protection added in
                   mmgsdi_cache_init_usim_ecc()
02/23/12   av      Clean up remaining always ON features
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
01/17/12   kk      Do not allow external cache updates for specific EFs when
                   cache is empty
01/05/12   shr     Removed F3 logging from within critical sections
12/21/11   kk      Legacy GSDI removal updates
12/21/11   kk      Added USIM ECC caching support
10/28/11   yt      Initial revision

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "mmgsdicache.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi.h"
#include "uim.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_csg.h"
#include "mmgsdi_file.h"
#include "mmgsdi_nv.h"

#ifdef FEATURE_UIM_PERSISTENT_CACHE
#include "mmgsdi_persistent_cache.h"
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

#include "bit.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Critical section to protect accesses to MMGSDI file/attributes caches */
static rex_crit_sect_type  mmgsdi_cache_crit_sect;

/* ------------------------------------------------------------------------
   STRUCTURE: MMGSDI_PROV_APPS_CACHE_INFO_TYPE

   DESCRIPTION:
      This structure holds the cache table pointer and the cache table size
      for a given provisioning application
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_cache_element_type  *cache_table_ptr;
  uint16                      cache_table_size;
} mmgsdi_prov_apps_cache_info_type;

/* ===========================================================================
 MACRO:       MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT
 DESCRIPTION: To enter rex critical section for file cache
 ===========================================================================*/
#define MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT                                 \
  MMGSDIUTIL_ENTER_CRIT_SECT(&mmgsdi_cache_crit_sect,                          \
                             MMGSDIUTIL_CRIT_SECT_FILE_CACHE);                 \

/* ===========================================================================
 MACRO:       MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT
 DESCRIPTION: To leave rex critical section for file cache
 ===========================================================================*/
#define MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT                                 \
  MMGSDIUTIL_LEAVE_CRIT_SECT(&mmgsdi_cache_crit_sect,                          \
                             MMGSDIUTIL_CRIT_SECT_FILE_CACHE);                 \

/* GW/1X prov apps cache pointers*/
static mmgsdi_prov_apps_cache_info_type mmgsdi_prov_apps_cache_ptr[MMGSDI_MAX_PROV_APPS];

static mmgsdi_record_cache_node_list_type mmgsdi_pri_gw_record_cache_list_node;

static mmgsdi_record_cache_node_list_type mmgsdi_pri_1x_record_cache_list_node;

static mmgsdi_record_cache_node_list_type mmgsdi_record_sec_gw_cache_list_node;

static mmgsdi_record_cache_node_list_type mmgsdi_record_sec_1x_cache_list_node;

static mmgsdi_record_cache_node_list_type mmgsdi_record_ter_gw_cache_list_node;

static mmgsdi_record_cache_node_list_type mmgsdi_record_ter_1x_cache_list_node;

/* slot1 Common cache */
static mmgsdi_cache_element_type mmgsdi_common_cache_slot1[MMGSDI_NUM_FILES_IN_COMMON_CACHE] =
{
  {MMGSDI_CACHE_NOT_INIT, 0, NULL, FALSE}, /* ICCID */
  {MMGSDI_CACHE_NOT_INIT, 0, NULL, FALSE} /* ELP */
};

/* Slot2 Common cache */
static mmgsdi_cache_element_type mmgsdi_common_cache_slot2[MMGSDI_NUM_FILES_IN_COMMON_CACHE] =
{
  {MMGSDI_CACHE_NOT_INIT, 0, NULL, FALSE}, /* ICCID */
  {MMGSDI_CACHE_NOT_INIT, 0, NULL, FALSE} /* ELP */
};

/* Slot3 Common cache */
static mmgsdi_cache_element_type mmgsdi_common_cache_slot3[MMGSDI_NUM_FILES_IN_COMMON_CACHE] =
{
  {MMGSDI_CACHE_NOT_INIT, 0, NULL, FALSE}, /* ICCID */
  {MMGSDI_CACHE_NOT_INIT, 0, NULL, FALSE} /* ELP */
};


mmgsdi_attr_cache_type *mmgsdi_slot1_common_cache_tbl_ptr  = NULL;
mmgsdi_attr_cache_type *mmgsdi_pri_gw_cache_tbl_ptr        = NULL;
mmgsdi_attr_cache_type *mmgsdi_pri_1x_cache_tbl_ptr        = NULL;

mmgsdi_attr_cache_type *mmgsdi_slot2_common_cache_tbl_ptr  = NULL;
mmgsdi_attr_cache_type *mmgsdi_sec_gw_cache_tbl_ptr        = NULL;
mmgsdi_attr_cache_type *mmgsdi_sec_1x_cache_tbl_ptr        = NULL;

mmgsdi_attr_cache_type *mmgsdi_slot3_common_cache_tbl_ptr  = NULL;
mmgsdi_attr_cache_type *mmgsdi_ter_gw_cache_tbl_ptr        = NULL;
mmgsdi_attr_cache_type *mmgsdi_ter_1x_cache_tbl_ptr        = NULL;

/*===========================================================================
                          IMPORTANT NOTE
=============================================================================
When adding a new file to the MMGSDI cache lists, the following
needs to be done:
a. Add the file to the MMGSDI REFRESH list.
b. Verify if the file needs to be cached during post-pin init.
c. Add an entry into mmgsdi_util_default_file_sec_attr_table.
d. If the file is record based, the file needs to be re-cached
   during REFRESH FCN handling using mmgsdi_init_cache_record()
   (in mmgsdi_refresh_process_evt()).
e. If the file is record based, in mmgsdi_session_write_cache(),
   the WRITE request should be issued using mmgsdi_session_write_record().
===========================================================================*/
/* SIM-CACHE INITIAL CONTENTS */
/* Required for GSM and RUIM with GSM access */
static mmgsdi_file_enum_type mmgsdi_sim_cache_uim_enums[MMGSDI_NUM_FILES_IN_SIM_CACHE] =
{
  MMGSDI_GSM_IMSI,     MMGSDI_GSM_KC,       MMGSDI_GSM_HPLMN, MMGSDI_GSM_ACM_MAX, MMGSDI_GSM_SST,
  MMGSDI_GSM_ACM,      MMGSDI_GSM_SPN,      MMGSDI_GSM_PUCT,  MMGSDI_GSM_BCCH,    MMGSDI_GSM_ACC,
  MMGSDI_GSM_FPLMN,    MMGSDI_GSM_LOCI,     MMGSDI_GSM_EMLPP, MMGSDI_GSM_AAEM,    MMGSDI_GSM_KCGPRS,
  MMGSDI_GSM_LOCIGPRS, MMGSDI_GSM_PHASE,    MMGSDI_GSM_AD,    MMGSDI_GSM_PLMN,    MMGSDI_GSM_PLMNWACT,
  MMGSDI_GSM_OPLMNWACT,MMGSDI_GSM_HPLMNACT, MMGSDI_GSM_CBMID, MMGSDI_GSM_LP,      MMGSDI_GSM_ECC,
  MMGSDI_GSM_SPDI,     MMGSDI_GSM_GID1,     MMGSDI_GSM_GID2,  MMGSDI_SIM_7F66_PROP1_SPT_TABLE,
  MMGSDI_SIM_7F66_PROP1_ACT_HPLMN
};

#ifdef FEATURE_MMGSDI_3GPP
/* USIM CACHE ELEMENTS */
static mmgsdi_file_enum_type mmgsdi_usim_cache_uim_enums[MMGSDI_NUM_FILES_IN_USIM_CACHE] =
{
  MMGSDI_USIM_IMSI,      MMGSDI_USIM_KEYS,      MMGSDI_USIM_KEYSPS, MMGSDI_USIM_HPLMN,    MMGSDI_USIM_ACM_MAX,
  MMGSDI_USIM_UST,       MMGSDI_USIM_EST,       MMGSDI_USIM_ACM,    MMGSDI_USIM_SPN,      MMGSDI_USIM_PUCT,
  MMGSDI_USIM_ACC,       MMGSDI_USIM_LOCI,      MMGSDI_USIM_PSLOCI, MMGSDI_USIM_EMLPP,    MMGSDI_USIM_AAEM,
  MMGSDI_USIM_START_HFN, MMGSDI_USIM_THRESHOLD, MMGSDI_USIM_KC,     MMGSDI_USIM_PLMNWACT, MMGSDI_USIM_OPLMNWACT,
  MMGSDI_USIM_HPLMNWACT, MMGSDI_USIM_RPLMNACT,  MMGSDI_USIM_FPLMN,  MMGSDI_USIM_LI,       MMGSDI_USIM_KCGPRS,
  MMGSDI_USIM_AD,        MMGSDI_USIM_CBMID,     MMGSDI_USIM_EHPLMN,   MMGSDI_USIM_EHPLMNPI, MMGSDI_USIM_LRPLMNSI,
  MMGSDI_USIM_EPSLOCI,   MMGSDI_USIM_EPSNSC,    MMGSDI_USIM_HIDDENKEY,MMGSDI_USIM_SPDI,     MMGSDI_USIM_ECC,
  MMGSDI_USIM_GID1,      MMGSDI_USIM_GID2,      MMGSDI_USIM_NASCONFIG,MMGSDI_USIM_7F66_PROP1_RAT,
  MMGSDI_USIM_7F66_PROP1_SPT_TABLE, MMGSDI_USIM_7F66_PROP1_ACT_HPLMN
};
#endif /* FEATURE_MMGSDI_3GPP */

#ifdef FEATURE_MMGSDI_3GPP2
/* RUIM CACHE ELEMENTS */
static mmgsdi_file_enum_type mmgsdi_ruim_cache_uim_enums[MMGSDI_NUM_FILES_IN_RUIM_CACHE] =
{
  MMGSDI_CDMA_IMSI_M,              MMGSDI_CDMA_IMSI_T,        MMGSDI_CDMA_SVC_TABLE,        MMGSDI_CDMA_PREF_LANG,
  MMGSDI_CDMA_HOME_SVC_PVDR_NAME,  MMGSDI_CDMA_ECC,           MMGSDI_CDMA_PRL,              MMGSDI_CDMA_EPRL,
  MMGSDI_CDMA_3GPD_MIPUPP,         MMGSDI_CDMA_3GPD_SIPUPP,   MMGSDI_CDMA_HRPDUPP,          MMGSDI_CDMA_SF_EUIM_ID,
  MMGSDI_CDMA_SERVICE_PREF,        MMGSDI_CDMA_NAM_LOCK,      MMGSDI_CDMA_HOME_SID_NID,     MMGSDI_CDMA_ZONE_BASED_REGN_IND,
  MMGSDI_CDMA_DIST_BASED_REGN_IND, MMGSDI_CDMA_SYS_REGN_IND,  MMGSDI_CDMA_OTAPA_SPC_ENABLE, MMGSDI_CDMA_RUIM_ID,
  MMGSDI_CDMA_ACCOLC,              MMGSDI_CDMA_3GPD_MIPSP,    MMGSDI_CDMA_3GPD_MIPFLAGS,    MMGSDI_CDMA_CALL_TERM_MODE_PREF,
  MMGSDI_CDMA_3GPD_3GPDOPM,        MMGSDI_CDMA_3GPD_SIPPAPSS, MMGSDI_CDMA_SMS_CAP
};

/* CSIM Cache Elements */
static mmgsdi_file_enum_type mmgsdi_csim_cache_uim_enums[MMGSDI_NUM_FILES_IN_CSIM_CACHE] =
{
  MMGSDI_CSIM_IMSI_M,            MMGSDI_CSIM_IMSI_T,                   MMGSDI_CSIM_CST,                      MMGSDI_CSIM_LANG_IND,
  MMGSDI_CSIM_EST,               MMGSDI_CSIM_AD,                       MMGSDI_CSIM_RUIM_ID,                  MMGSDI_CSIM_SERVICE_PREF,
  MMGSDI_CSIM_ACCOLC,            MMGSDI_CSIM_PRL,                      MMGSDI_CSIM_EPRL,                     MMGSDI_CSIM_MAXIMUM_PRL,
  MMGSDI_CSIM_HIDDEN_KEY,        MMGSDI_CSIM_SPN,                      MMGSDI_CSIM_ECC,                      MMGSDI_CSIM_3GPD_MIPUPP,
  MMGSDI_CSIM_3GPD_SIPUPP,       MMGSDI_CSIM_HRPDUPP,                  MMGSDI_CSIM_SF_EUIM_ID,               MMGSDI_CSIM_NAM_LOCK,
  MMGSDI_CSIM_CDMA_HOME_SID_NID, MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND, MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND, MMGSDI_CSIM_CDMA_SYS_REGN_IND,
  MMGSDI_CSIM_OTAPA_SPC_ENABLE,  MMGSDI_CSIM_3GPD_MIPSP,               MMGSDI_CSIM_3GPD_MIPFLAGS,            MMGSDI_CSIM_CALL_TERM_MODE_PREF,
  MMGSDI_CSIM_3GPD_3GPDOPM,      MMGSDI_CSIM_3GPD_SIPPAPSS,            MMGSDI_CSIM_SMS_CAP
};
#endif /* FEATURE_MMGSDI_3GPP2 */

static mmgsdi_file_enum_type mmgsdi_common_cache_uim_enums[MMGSDI_NUM_FILES_IN_COMMON_CACHE] =
{
  MMGSDI_ICCID, MMGSDI_ELP, MMGSDI_DIR
};


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_GET_PROV_CACHE_PTRS

   DESCRIPTION:
     This function return provisioning app cache pointer based on the session
     type and the app type.
     It also returns the cache size and the corresponding cache item pointer

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

CRITICAL SECTIONS:
     The calling function must enter mmgsdi_cache_crit_sect crit
     sect before invoking this util function. Protection for cache pointers.
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cache_get_prov_cache_ptrs(
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_app_enum_type           app_type,
  mmgsdi_cache_element_type    **cache_table_pptr,
  int32                         *cache_size_ptr,
  mmgsdi_file_enum_type        **cache_item_pptr)
{
  uint8  prov_session_index  = MMGSDI_MAX_PROV_APPS;

  MMGSDIUTIL_RETURN_IF_NULL_3(cache_table_pptr, cache_item_pptr, cache_size_ptr);

  prov_session_index = mmgsdi_util_get_prov_index_from_session(session_type);
  /* session_type is not provisioning return error */
  if (prov_session_index >= MMGSDI_MAX_PROV_APPS)
  {
    return MMGSDI_ERROR;
  }

  *cache_table_pptr = mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_ptr;
  *cache_size_ptr   = mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_size;

  if(*cache_table_pptr == NULL || *cache_size_ptr  == 0 )
  {
    UIM_MSG_ERR_2("Cache table not assigned yet for session_type 0x%x cache size 0x%x",
                  session_type,*cache_size_ptr );
    return MMGSDI_ERROR;
  }

  switch (app_type)
  {
    /* Required for GSM and RUIM with GSM access */
    case MMGSDI_APP_SIM:
      *cache_item_pptr = mmgsdi_sim_cache_uim_enums;
      break;

#ifdef FEATURE_MMGSDI_3GPP
    case MMGSDI_APP_USIM:
      *cache_item_pptr = mmgsdi_usim_cache_uim_enums;
      break;
#endif /* FEATURE_MMGSDI_3GPP */

#ifdef FEATURE_MMGSDI_3GPP2
    case MMGSDI_APP_RUIM:
      *cache_item_pptr = mmgsdi_ruim_cache_uim_enums;
      break;
    case MMGSDI_APP_CSIM:
      *cache_item_pptr = mmgsdi_csim_cache_uim_enums;
      break;
#endif /* FEATURE_MMGSDI_3GPP2 */

    default:
      UIM_MSG_ERR_1("Invalid app_type for cache access 0x%x", app_type);
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_cache_get_prov_cache_ptrs */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_GET_GET_COMMON_CACHE_PTR

   DESCRIPTION:
     This function, based on the session_type, determines what slot is it
     associated with and thereby determines the common cache pointer

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Sucessful handling
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cache_get_common_cache_ptr(
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_cache_element_type    **cache_table_pptr)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type   slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_info_type  *session_info_ptr = NULL;

  switch(session_type)
  {
    case MMGSDI_NON_PROV_SESSION_SLOT_1:
    case MMGSDI_CARD_SESSION_SLOT_1:
      *cache_table_pptr = mmgsdi_common_cache_slot1;
      return MMGSDI_SUCCESS;

    case MMGSDI_NON_PROV_SESSION_SLOT_2:
    case MMGSDI_CARD_SESSION_SLOT_2:
      *cache_table_pptr = mmgsdi_common_cache_slot2;
      return MMGSDI_SUCCESS;

    case MMGSDI_NON_PROV_SESSION_SLOT_3:
    case MMGSDI_CARD_SESSION_SLOT_3:
      *cache_table_pptr = mmgsdi_common_cache_slot3;
      return MMGSDI_SUCCESS;

    default:
      break;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                               mmgsdi_generic_data_ptr->pri_gw_session_id,
                               &slot_id,NULL,NULL,NULL,&session_info_ptr,NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL ||
     session_info_ptr->session_type != session_type)
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                                 mmgsdi_generic_data_ptr->sec_gw_session_id,
                                 &slot_id,NULL,NULL,NULL,&session_info_ptr,NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS ||
       session_info_ptr == NULL ||
       session_info_ptr->session_type != session_type)
    {
      mmgsdi_status = mmgsdi_util_get_session_app_info(
                                   mmgsdi_generic_data_ptr->ter_gw_session_id,
                                   &slot_id,NULL,NULL,NULL,&session_info_ptr,NULL);
      if(mmgsdi_status != MMGSDI_SUCCESS ||
         session_info_ptr == NULL ||
         session_info_ptr->session_type != session_type)
      {
        mmgsdi_status = mmgsdi_util_get_session_app_info(
                                     mmgsdi_generic_data_ptr->pri_1x_session_id,
                                     &slot_id,NULL,NULL,NULL,&session_info_ptr,NULL);
        if(mmgsdi_status != MMGSDI_SUCCESS ||
           session_info_ptr == NULL ||
           session_info_ptr->session_type != session_type)
        {
          mmgsdi_status = mmgsdi_util_get_session_app_info(
                                       mmgsdi_generic_data_ptr->sec_1x_session_id,
                                       &slot_id,NULL,NULL,NULL,&session_info_ptr,NULL);
          if(mmgsdi_status != MMGSDI_SUCCESS ||
             session_info_ptr == NULL ||
             session_info_ptr->session_type != session_type)
          {
            mmgsdi_status = mmgsdi_util_get_session_app_info(
                                         mmgsdi_generic_data_ptr->ter_1x_session_id,
                                         &slot_id,NULL,NULL,NULL,&session_info_ptr,NULL);
            if(mmgsdi_status != MMGSDI_SUCCESS ||
               session_info_ptr == NULL ||
               session_info_ptr->session_type != session_type)
            {
              UIM_MSG_ERR_1("Invalid session type - 0x%x, passed to look for common cache ptr",
                            session_type);
              return MMGSDI_ERROR;
            }
          }
        }
      }
    }
  }

  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      *cache_table_pptr = mmgsdi_common_cache_slot1;
      mmgsdi_status =  MMGSDI_SUCCESS;
      break;

    case MMGSDI_SLOT_2:
      *cache_table_pptr = mmgsdi_common_cache_slot2;
      mmgsdi_status =  MMGSDI_SUCCESS;
      break;

    case MMGSDI_SLOT_3:
      *cache_table_pptr = mmgsdi_common_cache_slot3;
      mmgsdi_status =  MMGSDI_SUCCESS;
      break;

    default:
      UIM_MSG_ERR_1("Invalid session type - 0x%x, passed to look for common cache ptr",
                    session_type);
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }

  return mmgsdi_status;
}/*mmgsdi_cache_get_common_cache_ptr*/


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_GET_CACHE_PTRS_AND_INDEX

   DESCRIPTION:
     This function maps the passed in file enum to either the common cache or
     it calls the mmgsdi_cache_get_prov_cache_ptrs to get the
     provisioning cache pointer if the file passed in is not part of the
     common cache.
     It returns the cache pointer, cache size, the corresponding cache item
     pointer as well as the index location of the file in that caches

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

CRITICAL SECTIONS:
     The calling function must enter mmgsdi_cache_crit_sect crit
     sect before invoking this util function. Protection for cache pointers.
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cache_get_cache_ptrs_and_index(
  mmgsdi_file_enum_type          mmgsdi_file,
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_app_enum_type           app_type,
  mmgsdi_cache_element_type    **cache_table_pptr,
  int32                         *cache_size_ptr,
  int32                         *file_index_ptr)
{
  mmgsdi_return_enum_type mmgsdi_status   = MMGSDI_SUCCESS;
  int32                   file_cnt        = 0;
  mmgsdi_file_enum_type  *cache_item_ptr  = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_3(cache_table_pptr, cache_size_ptr, file_index_ptr);

  /* Determine if the file is in common cache or not */
  for(file_cnt = 0; file_cnt < MMGSDI_NUM_FILES_IN_COMMON_CACHE; file_cnt++)
  {
    if (mmgsdi_common_cache_uim_enums[file_cnt] == mmgsdi_file)
    {
      mmgsdi_status = mmgsdi_cache_get_common_cache_ptr(session_type,cache_table_pptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return MMGSDI_ERROR;
      }
      *cache_size_ptr  = MMGSDI_NUM_FILES_IN_COMMON_CACHE;
      *file_index_ptr  = file_cnt;
      return MMGSDI_SUCCESS;
    }
  }

  mmgsdi_status = mmgsdi_cache_get_prov_cache_ptrs(session_type,
                                                  app_type,
                                                  cache_table_pptr,
                                                  cache_size_ptr,
                                                  &cache_item_ptr);
  if ( mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(*cache_table_pptr == NULL)
  {
    UIM_MSG_ERR_1("Cache table not assigned yet for session_type 0x%x",
                  session_type);
    return MMGSDI_ERROR;
  }

  for(file_cnt = 0; file_cnt < *cache_size_ptr; file_cnt++)
  {
    if (cache_item_ptr[file_cnt] == mmgsdi_file)
    {
      *file_index_ptr  = file_cnt;
      return MMGSDI_SUCCESS;
    }
  }

  UIM_MSG_ERR_1("Cache not found for file 0x%x", mmgsdi_file);
  return MMGSDI_NOT_FOUND;
} /* mmgsdi_cache_get_cache_ptrs_and_index */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_GET_PROV_RECORD_CACHE_PTR

   DESCRIPTION:
     This function return provisioning app record cache pointer based on the session
     type.

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cache_get_prov_record_cache_ptr(
  mmgsdi_session_type_enum_type        session_type,
  mmgsdi_record_cache_node_list_type **record_cache_pptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(record_cache_pptr);

  /* Search for technology specific record caches */
  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *record_cache_pptr = &mmgsdi_pri_gw_record_cache_list_node;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION:
      *record_cache_pptr = &mmgsdi_pri_1x_record_cache_list_node;
       break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      *record_cache_pptr = &mmgsdi_record_sec_gw_cache_list_node;
      break;
    case MMGSDI_1X_PROV_SEC_SESSION:
      *record_cache_pptr = &mmgsdi_record_sec_1x_cache_list_node;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      *record_cache_pptr = &mmgsdi_record_ter_gw_cache_list_node;
      break;
    case MMGSDI_1X_PROV_TER_SESSION:
      *record_cache_pptr = &mmgsdi_record_ter_1x_cache_list_node;
      break;
    default:
      UIM_MSG_ERR_1("Invalid session_type for record cache access 0x%x",
                    session_type);
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cache_get_prov_record_cache_ptr */


#ifdef FEATURE_UIM_PERSISTENT_CACHE
/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_UPDATE_PERSISTENT_CACHE

   DESCRIPTION:
     This function update the persistent cache with cache data

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cache_update_persistent_cache(
  mmgsdi_session_id_type        session_id,
  const mmgsdi_access_type     *file_access_ptr,
  mmgsdi_data_from_enum_type    data_fr_card
)
{
  mmgsdi_slot_id_enum_type    slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_data_type            cache_data;
  uint32                      data_len      = 0;

  if(file_access_ptr == NULL ||
     file_access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS ||
     data_fr_card == MMGSDI_DATA_FROM_CLIENT)
  {
    UIM_MSG_HIGH_0("Invalid file_access_ptr or data from client");
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    NULL);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    UIM_MSG_HIGH_1("Failed to get slot_id: mmgsdi_status = 0x%x", mmgsdi_status);
    return mmgsdi_status;
  }

  /* for MMGSDI_DATA_FROM_CARD_INCOMPLETE, if persistent cache for the
     EF is not initialized (ie. not found), then skip the update. */
  memset(&cache_data, 0x00, sizeof(mmgsdi_data_type));
  if(data_fr_card == MMGSDI_DATA_FROM_CARD_INCOMPLETE)
  {
    mmgsdi_status = mmgsdi_persistent_cache_read(slot_id,
                                                 file_access_ptr->file.file_enum,
                                                 0,
                                                 &cache_data.data_len,
                                                 &cache_data.data_ptr);

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_data.data_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_HIGH_1("data from card incomplete for file 0x%x",
                     file_access_ptr->file.file_enum);
      return mmgsdi_status;
    }
  }

  /* update persistent cache */
  /* retrieve data from cache to write into persistent cache */
  memset(&cache_data, 0x00, sizeof(mmgsdi_data_type));
  mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                        file_access_ptr,
                                        &cache_data.data_len,
                                        0,
                                        NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("failed to read cache len for enum 0x%x",
                   file_access_ptr->file.file_enum);
    return mmgsdi_status;
  }

  if(cache_data.data_len <= 0)
  {
    return MMGSDI_ERROR;
  }

  data_len = int32touint32(cache_data.data_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_data.data_ptr, data_len);
  if (cache_data.data_ptr == NULL)
  {
    /* Unable to allocate memory */
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_cache_read(session_id,
                             file_access_ptr,
                             &cache_data,
                             0,
                             NULL);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* write to persistent cache */
    mmgsdi_status = mmgsdi_persistent_cache_write(slot_id,
                                                  file_access_ptr->file.file_enum,
                                                  0,
                                                  cache_data.data_len,
                                                  (const uint8*)cache_data.data_ptr);
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_data.data_ptr);
  return mmgsdi_status;
} /* mmgsdi_cache_update_persistent_cache */
#endif /* FEATURE_UIM_PERSISTENT_CACHE */


/*===========================================================================
FUNCTION MMGSDI_CACHE_INIT

DESCRIPTION
  Function called to initialize global variables for MMGSDI cache.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
void mmgsdi_cache_init(
  void
)
{
  rex_init_crit_sect(&mmgsdi_cache_crit_sect);

  memset(&mmgsdi_pri_gw_record_cache_list_node, 0x00, sizeof(mmgsdi_record_cache_node_list_type));
  memset(&mmgsdi_pri_1x_record_cache_list_node, 0x00, sizeof(mmgsdi_record_cache_node_list_type));
  memset(&mmgsdi_record_sec_gw_cache_list_node, 0x00, sizeof(mmgsdi_record_cache_node_list_type));
  memset(&mmgsdi_record_sec_1x_cache_list_node, 0x00, sizeof(mmgsdi_record_cache_node_list_type));
  memset(&mmgsdi_record_ter_gw_cache_list_node, 0x00, sizeof(mmgsdi_record_cache_node_list_type));
  memset(&mmgsdi_record_ter_1x_cache_list_node, 0x00, sizeof(mmgsdi_record_cache_node_list_type));
} /* mmgsdi_cache_init */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_READ_ITEM_LEN

   DESCRIPTION:
     This function return the cache item len

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read_item_len(
  mmgsdi_file_enum_type         mmgsdi_file,
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_len_type               data_offset,
  mmgsdi_len_type              *total_len_ptr,
  mmgsdi_cache_init_enum_type  *cache_state_ptr
)
{
  /*-----------------------------------------------------------------------------
    Initialize variables
  ------------------------------------------------------------------------------*/
  int32                      file_index      = 0;
  int32                      cache_size      = 0;
  mmgsdi_cache_element_type *cache_table_ptr = NULL;
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_SUCCESS;

  if(mmgsdi_file != MMGSDI_USIM_UST)
  {
    UIM_MSG_HIGH_3("mmgsdi_cache_read_item_len file 0x%x, session_type 0x%x, app_type 0x%x",
                   mmgsdi_file, session_type, app_type);
  }

  if(cache_state_ptr != NULL)
  {
    *cache_state_ptr = MMGSDI_CACHE_MAX_ENUM;
  }

  MMGSDIUTIL_RETURN_IF_NULL(total_len_ptr);

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_cache_get_cache_ptrs_and_index(mmgsdi_file,
                                                          session_type,
                                                          app_type,
                                                          &cache_table_ptr,
                                                          &cache_size,
                                                          &file_index);
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (cache_table_ptr == NULL))
    {
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    if ( cache_table_ptr[file_index].init == MMGSDI_CACHE_NOT_INIT )
    {
      UIM_MSG_ERR_1("Cache not initialized 0x%x", mmgsdi_file);
      mmgsdi_status = MMGSDI_NOT_INIT;
      break;
    }
    else if( cache_table_ptr[file_index].init == MMGSDI_CACHE_NOT_FOUND )
    {
      UIM_MSG_HIGH_1("Cache status indicates that file not found on card 0x%x",
                     mmgsdi_file);

      if(cache_state_ptr != NULL)
      {
        *cache_state_ptr = MMGSDI_CACHE_NOT_FOUND;
      }
      mmgsdi_status = MMGSDI_NOT_FOUND;
      break;
    }

    if (cache_table_ptr[file_index].total_len <= data_offset)
    {
      UIM_MSG_ERR_2("Data offset 0x%x same or greater than total len 0x%x",
                    data_offset, cache_table_ptr[file_index].total_len);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }
    *total_len_ptr = cache_table_ptr[file_index].total_len - data_offset;
  }while(0);

  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cache_read_item_len */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_READ_LEN

   DESCRIPTION:
     This function return the total len - offset of the cache item

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, it will return the data
     from the first cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read_len(
  mmgsdi_session_id_type         session_id,
  const mmgsdi_access_type      *file_access_ptr,
  mmgsdi_len_type               *data_len_ptr,
  mmgsdi_len_type                offset,
  mmgsdi_cache_init_enum_type   *cache_state_ptr
)
{
  mmgsdi_return_enum_type       mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_session_type_enum_type session_type   = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_aid_type               aid_type;

  memset(&aid_type, 0x00, sizeof(mmgsdi_aid_type));

  if(cache_state_ptr != NULL)
  {
    *cache_state_ptr = MMGSDI_CACHE_MAX_ENUM;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(file_access_ptr, data_len_ptr);

  if(file_access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS)
  {

    UIM_MSG_ERR_1("Skip Cache: not by EF ENUM ACCESS: 0x%x",
                  file_access_ptr->access_method);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               &aid_type);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  return mmgsdi_cache_read_item_len(file_access_ptr->file.file_enum,
                                    aid_type.app_type,
                                    session_type,
                                    offset,
                                    data_len_ptr,
                                    cache_state_ptr);
} /* mmgsdi_cache_read_len */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_WRITE_ITEM

   DESCRIPTION:
     This function writes to the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_write_item(
  mmgsdi_file_enum_type         mmgsdi_file,
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_len_type               data_len,
  mmgsdi_len_type               data_offset,
  const uint8 *                 data_ptr,
  mmgsdi_data_from_enum_type    data_fr_card
)
{
  /*-----------------------------------------------------------------------------
    Initialize variables
  ------------------------------------------------------------------------------*/
  int32                      file_index                 = 0;
  mmgsdi_return_enum_type    mmgsdi_status              = MMGSDI_SUCCESS;
  int32                      cache_size                 = 0;
  mmgsdi_cache_element_type *cache_table_ptr            = NULL;
  boolean                    support_extended_fplmn_icc = FALSE;

  UIM_MSG_HIGH_3("mmgsdi_cache_write_item file 0x%x, session_type 0x%x, app_type 0x%x",
                 mmgsdi_file, session_type, app_type);

  if (data_offset < 0)
  {
    UIM_MSG_ERR_1("Invalid Data Offset %d < 0", data_offset);
    return MMGSDI_ERROR;
  }

  /* Check Extended FPLMN NV status when file is GSM_FPLMN. If ENABLED, write
     complete length in cache. Return error if the req lenth is not equal to
     FPLMN cache size and greater than card size i.e MMGSDI_GSM_FPLMN_SIZE */
  if (mmgsdi_file == MMGSDI_GSM_FPLMN &&
      mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SUPPORT_EXTENDED_FPLMN_ICC,
        MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
  {
    UIM_MSG_MED_1("Support Extended FPLMN with data_len 0x%x", data_len);
    if(data_len > MMGSDI_GSM_FPLMN_SIZE &&
       data_len != MMGSDI_EXTENDED_ICC_FPLMN_CACHE_LEN)
    {
      return MMGSDI_INCORRECT_PARAMS;
    }
    support_extended_fplmn_icc = TRUE;
  }

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_cache_get_cache_ptrs_and_index(mmgsdi_file,
                                                          session_type,
                                                          app_type,
                                                          &cache_table_ptr,
                                                          &cache_size,
                                                          &file_index);
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (cache_table_ptr == NULL) ||
        (file_index >= cache_size))
    {
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    if(data_fr_card == MMGSDI_DATA_NOT_FOUND)
    {
      cache_table_ptr[file_index].init = MMGSDI_CACHE_NOT_FOUND;
      mmgsdi_status = MMGSDI_SUCCESS;
      break;
    }

    if (cache_table_ptr[file_index].init == MMGSDI_CACHE_NOT_FOUND)
    {
      UIM_MSG_HIGH_1("Cache status indicates file not found on card 0x%x",
                     mmgsdi_file);
      mmgsdi_status = MMGSDI_NOT_FOUND;
      break;
    }

    if (data_ptr == NULL)
    {
      MMGSDIUTIL_DEBUG_MSG_ERROR_0("Attempt to use NULL POINTER");
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }

    /* ECC caching will be done at USIM ECC initialization */
    if (mmgsdi_file == MMGSDI_USIM_ECC)
    {
      mmgsdi_status = MMGSDI_SUCCESS;
      break;
    }

    /*-----------------------------------------------------------------------------
      Handle the special case of updating the data len in cache during init when
      GSDI is caching information.  During init, GSDI caches whole records
      partial records or offsets of records are not cached during init
    -----------------------------------------------------------------------------*/

    if (file_index < cache_size)
    {
      /* Need to first update the Length of the file */
      /* Is this the first time reading the file ?     */
      if (cache_table_ptr[file_index].init == MMGSDI_CACHE_NOT_INIT)
      {
        /* Do not allow external Write Cache request for an item
           which is in our post pin cache list when cache is empty. */
        /* Also ensure the cache is initalized with the full size
           of the file by check if the card is init complete. */
        if((data_fr_card == MMGSDI_DATA_FROM_CLIENT &&
            mmgsdi_util_find_ef_in_power_up_cache_list(mmgsdi_file)) ||
            data_fr_card == MMGSDI_DATA_FROM_CARD_INCOMPLETE)
        {
          UIM_MSG_HIGH_2("Unable to initalize cache for file: 0x%x, data from card: 0x%x",
                         mmgsdi_file, data_fr_card);
          mmgsdi_status = MMGSDI_ERROR;
          break;
        }

        /* OK to update to length returned */
        /* Need to avoid updating to length 0 */
        if ( data_len != 0 )
        {
          /* If file is FPLMN with extended support, allocate complete chunk
             of 60 Bytes and memset it with 0xFF to make the content invalid */
          if (support_extended_fplmn_icc)
          {
            MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
                              cache_table_ptr[file_index].data_ptr,
                              MMGSDI_EXTENDED_ICC_FPLMN_CACHE_LEN);

            if(cache_table_ptr[file_index].data_ptr == NULL)
            {
              mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
              break;
            }
            cache_table_ptr[file_index].total_len = MMGSDI_EXTENDED_ICC_FPLMN_CACHE_LEN;
            memset(cache_table_ptr[file_index].data_ptr, 0xFF,
                   MMGSDI_EXTENDED_ICC_FPLMN_CACHE_LEN);
          }
          else
          {
            MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
                              cache_table_ptr[file_index].data_ptr, data_len);

            if(cache_table_ptr[file_index].data_ptr == NULL)
            {
              mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
              break;
            }
            cache_table_ptr[file_index].total_len = data_len;
          }
        } /* endif - data len is not 0 */
      } /* cache file ! init */
    } /* init not completed, update the cache length */

    /*-----------------------------------------------------------------------------
      Handle case where requested length is longer than the length of the record
    -----------------------------------------------------------------------------*/
    if (data_offset + data_len > cache_table_ptr[file_index].total_len)
    {
      UIM_MSG_ERR_3("Data offset 0x%x + data len 0x%x > file size 0x%x",
                    data_offset,
                    data_len,
                    cache_table_ptr[file_index].total_len);
      if (cache_table_ptr[file_index].init == MMGSDI_CACHE_NOT_INIT)
      {
        MMGSDIUTIL_TMC_MEM_FREE(cache_table_ptr[file_index].data_ptr);
        cache_table_ptr[file_index].total_len = 0;
      }
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    } /* end if - requested data is too long */

    /*-----------------------------------------------------------------------------
      Proceed with cache write, Data length of 0 indicates the max number of bytes
      to be written
    -----------------------------------------------------------------------------*/
    if (data_len == 0)
    {
      if (cache_table_ptr[file_index].total_len <= data_offset)
      {
        UIM_MSG_ERR_2("Data offset 0x%x same or greater than total len 0x%x",
                      data_offset, cache_table_ptr[file_index].total_len);
        if (cache_table_ptr[file_index].init == MMGSDI_CACHE_NOT_INIT)
        {
          MMGSDIUTIL_TMC_MEM_FREE(cache_table_ptr[file_index].data_ptr);
          cache_table_ptr[file_index].total_len = 0;
        }
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }
      data_len = cache_table_ptr[file_index].total_len - data_offset;
    } /* Write max data length */

    if (data_len < 0)
    {
      UIM_MSG_ERR_1("Invalid Data Len requested 0x%x < 0", data_len);
      if (cache_table_ptr[file_index].init == MMGSDI_CACHE_NOT_INIT)
      {
        MMGSDIUTIL_TMC_MEM_FREE(cache_table_ptr[file_index].data_ptr);
        cache_table_ptr[file_index].total_len = 0;
      }
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Copy contents to be written into cache */
    mmgsdi_memscpy((cache_table_ptr[file_index].data_ptr) + data_offset,
                   int32touint32(cache_table_ptr[file_index].total_len) - data_offset,
                   data_ptr,
                   int32touint32(data_len));
    cache_table_ptr[file_index].init = MMGSDI_CACHE_INIT;

   /* Update whether the cache data is in synch with card or not,
      In the New arch, util write is called only after a process to the card
       It does not have the write to cache and then queue the command */
    switch (data_fr_card)
    {
      case MMGSDI_DATA_FROM_CARD_INCOMPLETE:
      case MMGSDI_DATA_FROM_CARD_COMPLETE:
        cache_table_ptr[file_index].in_synch = TRUE;
        break;
      case MMGSDI_DATA_FROM_CLIENT:
      default:
        cache_table_ptr[file_index].in_synch = FALSE;
        break;
    }
  }while(0);

  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cache_write_item */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE_SELECTIVE_ITEMS

   DESCRIPTION:
     This function deletes all the cache items except listed in the input
     parameters

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_selective_items(
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type,
  const mmgsdi_file_enum_type  *files_not_to_delete_ptr,
  uint16                        num_files
)
{
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_cache_element_type *cache_table_ptr = NULL;
  int32                      cache_data_len  = 0;
  mmgsdi_file_enum_type     *cache_item_ptr  = NULL;
  int32                      i               = 0;
  int32                      j               = 0;
  boolean                    match           = FALSE;

  UIM_MSG_HIGH_2("mmgsdi_cache_delete_selective_cache_items sesson_type 0x%x, app_type 0x%x",
                 session_type, app_type);

  if(num_files > 0)
  {
    MMGSDIUTIL_RETURN_IF_NULL(files_not_to_delete_ptr);
  }

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_cache_get_prov_cache_ptrs(session_type,
                                                     app_type,
                                                     &cache_table_ptr,
                                                     &cache_data_len,
                                                     &cache_item_ptr);
    if ((mmgsdi_status != MMGSDI_SUCCESS)
        || (cache_table_ptr == NULL)
        || (cache_item_ptr  == NULL))
    {
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    for(i = 0; i < cache_data_len; i++)
    {
      match = FALSE;
      for (j = 0; j < num_files; j++)
      {
        if(cache_item_ptr[i] == files_not_to_delete_ptr[j])
        {
          /* item matches the file that should not be deleted,
             skip to the next file */
          match = TRUE;
          continue;
        }
      }
      if(!match)
      {
        cache_table_ptr[i].init = MMGSDI_CACHE_NOT_INIT;
        cache_table_ptr[i].in_synch = FALSE;
        cache_table_ptr[i].total_len = 0;
        MMGSDIUTIL_TMC_MEM_FREE(cache_table_ptr[i].data_ptr);
      }
    }
  }while(0);

  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cache_delete_selective_items */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_CREATE_PROV_FILE_CACHE_CLEAR_CACHES

   DESCRIPTION:
     If the function is invoked due to a Session activation, this function
     creates the provisioning file cache by allocating the
     exact number of memory space required based on the corresponding
     session type and app type.
     If the provisioning file cache already exists, the file cache is
     cleared and allocated memory freed.
     This function also clears the File attributes cache and the record cache.

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_create_prov_file_cache_clear_caches(
  mmgsdi_session_id_type        session_id,
  boolean                       session_activate
)
{
  mmgsdi_return_enum_type       mmgsdi_status            = MMGSDI_SUCCESS;
  mmgsdi_session_type_enum_type session_type             = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_aid_type               aid_type;
  uint16                        cache_size               = 0;
  mmgsdi_cache_element_type   **cache_table_pptr         = NULL;
  mmgsdi_cphs_int_info_type   **mmgsdi_gw_cphs_info_pptr = NULL;
  uint8                         prov_session_index       = MMGSDI_MAX_PROV_APPS;

  memset(&aid_type, 0x00, sizeof(mmgsdi_aid_type));

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               &aid_type);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  UIM_MSG_HIGH_1("mmgsdi_cache_create_prov_file_cache_clear_caches for session_type 0x%x",
                 session_type);
  prov_session_index = mmgsdi_util_get_prov_index_from_session(session_type);
  if (prov_session_index >= MMGSDI_MAX_PROV_APPS)
  {
    /*Not a provisioing session type for given session_id */
    return MMGSDI_ERROR;
  }

  /* Pick the appropriate prov cache ptr */
  cache_table_pptr = &mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_ptr;

  /* Search for technology specific caches to pick the cphs info*/
  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      mmgsdi_gw_cphs_info_pptr = &mmgsdi_cphs_info_ptr[MMGSDI_PRI_CPHS_INFO_INDEX];
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      mmgsdi_gw_cphs_info_pptr = &mmgsdi_cphs_info_ptr[MMGSDI_SEC_CPHS_INFO_INDEX];
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      mmgsdi_gw_cphs_info_pptr = &mmgsdi_cphs_info_ptr[MMGSDI_TER_CPHS_INFO_INDEX];
      break;
    default:
      mmgsdi_gw_cphs_info_pptr = NULL;
      break;
  }

  switch (aid_type.app_type)
  {
    /* Required for GSM and RUIM with GSM access */
    case MMGSDI_APP_SIM:
      cache_size = MMGSDI_NUM_FILES_IN_SIM_CACHE;
      break;

#ifdef FEATURE_MMGSDI_3GPP
    case MMGSDI_APP_USIM:
      cache_size = MMGSDI_NUM_FILES_IN_USIM_CACHE;
      break;
#endif /* FEATURE_MMGSDI_3GPP */

#ifdef FEATURE_MMGSDI_3GPP2
    case MMGSDI_APP_RUIM:
      cache_size = MMGSDI_NUM_FILES_IN_RUIM_CACHE;
      break;
    case MMGSDI_APP_CSIM:
      cache_size = MMGSDI_NUM_FILES_IN_CSIM_CACHE;
      break;
#endif /* FEATURE_MMGSDI_3GPP2 */

    default:
      UIM_MSG_ERR_1("Invalid app_type for cache access 0x%x", aid_type.app_type);
      return MMGSDI_ERROR;
  }

  /* Clear the File attribute cache */
  (void)mmgsdi_cache_delete_prov_file_attr_cache(session_type, NULL, 0);

  /* Clear the Record Cache */
  if((aid_type.app_type == MMGSDI_APP_USIM) ||
     (aid_type.app_type == MMGSDI_APP_CSIM))
  {
    mmgsdi_cache_delete_record_selective_items(session_id, NULL, 0);
  }

  (void)mmgsdi_eons_opl_pnn_delete_cache_all(session_id);

  /* Reset the CSG EONS info. The related EFs are read after
     subscription is published */
  if(aid_type.app_type == MMGSDI_APP_USIM)
  {
    mmgsdi_csg_reset_eons_flags_in_app_info(session_id);
  }

  /* Allocate memory for cphs info if session is getting activated */
  if(session_activate)
  {
    if(mmgsdi_gw_cphs_info_pptr != NULL && *mmgsdi_gw_cphs_info_pptr == NULL)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*mmgsdi_gw_cphs_info_pptr,
                                         sizeof(mmgsdi_cphs_int_info_type));
    }
  }
  else
  {
    if(mmgsdi_gw_cphs_info_pptr != NULL && *mmgsdi_gw_cphs_info_pptr != NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        (*mmgsdi_gw_cphs_info_pptr)->cphs_ons_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        (*mmgsdi_gw_cphs_info_pptr)->cphs_ons_short_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(*mmgsdi_gw_cphs_info_pptr);
    }
  }

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  if(*cache_table_pptr != NULL)
  {
    /* Clear the file cache */
    (void)mmgsdi_cache_delete(aid_type.app_type, session_type);

    /* Free the allocated memory */
    MMGSDIUTIL_TMC_MEM_FREE(*cache_table_pptr);
    /* Update the cache size back once freed - Reset of this should be after mmgsdi_cache_delete as the previous value is used inside it*/
    mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_size = 0;
  }

  /* If the Session is geting activated, allocate memory for the
     file cache table */
  if(session_activate)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      *cache_table_pptr,
      (sizeof(mmgsdi_cache_element_type) * (uint32)cache_size));
    if ((*cache_table_pptr) == NULL)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    else
    {
      /* Update the allocated cache size in the session cache size global */
      mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_size = cache_size;
    }
  }

  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cache_create_prov_file_cache_clear_caches */


/*===========================================================================
FUNCTION MMGSDI_CACHE_DELETE_FILE_ATTR_CACHE_ITEM

DESCRIPTION:
  This function clears specific file attributes cache item

DEPENDENCIES:
  None

LIMITATIONS:
   None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mmgsdi_cache_delete_file_attr_cache_item(
  mmgsdi_attr_cache_element_type ** attr_cache_element_pptr)
{
  if((*attr_cache_element_pptr) != NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      (*attr_cache_element_pptr)->read.protection_pin_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      (*attr_cache_element_pptr)->write.protection_pin_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      (*attr_cache_element_pptr)->increase.protection_pin_ptr);
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK((*attr_cache_element_pptr));
} /* mmgsdi_cache_delete_file_attr_cache_item */


/*===========================================================================
FUNCTION MMGSDI_CACHE_RESET_FILE_ATTR_CACHE_ITEM

DESCRIPTION:
  This function resets specific file attributes cache item (without deleting
  the item from the corresponding cache list)

DEPENDENCIES:
  None

LIMITATIONS:
   None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void mmgsdi_cache_reset_file_attr_cache_item(
  mmgsdi_attr_cache_element_type ** attr_cache_element_pptr)
{
  if((*attr_cache_element_pptr) == NULL)
  {
    return;
  }

  (*attr_cache_element_pptr)->read.protection_method  = MMGSDI_ALWAYS_ALLOWED;
  (*attr_cache_element_pptr)->read.num_protection_pin = 0;
  (*attr_cache_element_pptr)->write.protection_method  = MMGSDI_ALWAYS_ALLOWED;
  (*attr_cache_element_pptr)->write.num_protection_pin = 0;
  (*attr_cache_element_pptr)->increase.protection_method  = MMGSDI_ALWAYS_ALLOWED;
  (*attr_cache_element_pptr)->increase.num_protection_pin = 0;
  (*attr_cache_element_pptr)->cyclic_increase_allowed = FALSE;

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    (*attr_cache_element_pptr)->read.protection_pin_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    (*attr_cache_element_pptr)->write.protection_pin_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    (*attr_cache_element_pptr)->increase.protection_pin_ptr);

  (*attr_cache_element_pptr)->file_type   = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
  (*attr_cache_element_pptr)->file_size   = MMGSDI_INVALID_FILE_SIZE;
  (*attr_cache_element_pptr)->num_of_rec  = MMGSDI_INVALID_REC_LEN;
  (*attr_cache_element_pptr)->rec_len     = MMGSDI_INVALID_REC_LEN;
  (*attr_cache_element_pptr)->sfi         = UIM_INVALID_SFI;
} /* mmgsdi_cache_reset_file_attr_cache_item */


/*===========================================================================
FUNCTION:    MMGSDI_CACHE_DELETE_ATTR_SELECTIVE_ITEMS

DESCRIPTION:
  This function clears file attributes cache except listed in the input
  parameters' "Not to delete" list

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None

CRITICAL SECTIONS:
  This function acquires task state critical section while accessing cache
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cache_delete_attr_selective_items
(
  mmgsdi_attr_cache_type       *attr_cache_table_ptr,
  const mmgsdi_file_enum_type  *files_not_to_delete_ptr,
  uint16                        num_files
)
{
  mmgsdi_attr_cache_element_type *curr_attr_cache_element_ptr = NULL;
  mmgsdi_attr_cache_element_type *next_attr_element_ptr       = NULL;
  mmgsdi_attr_cache_element_type *prev_attr_element_ptr       = NULL;
  int32                           index                       = 0;
  boolean                         match                       = FALSE;

  MMGSDIUTIL_RETURN_IF_NULL(attr_cache_table_ptr);

  if(num_files > 0)
  {
    MMGSDIUTIL_RETURN_IF_NULL(files_not_to_delete_ptr);
  }

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  curr_attr_cache_element_ptr = attr_cache_table_ptr->item_ptr;

  /* Delete file attribute cache elements of the files which are not part of
     not_to_delete list */
  while(curr_attr_cache_element_ptr != NULL)
  {
    next_attr_element_ptr = curr_attr_cache_element_ptr->next_ptr;

    match = FALSE;
    for (index = 0; index < num_files; index++)
    {
      if((curr_attr_cache_element_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS) &&
         (curr_attr_cache_element_ptr->access.file.file_enum == files_not_to_delete_ptr[index]))
      {
        /* item matches the file that should not be deleted,
           skip to the next file */
        match = TRUE;
        break;
      }
    }

    if(!match)
    {
      /* Relink the list of cache elements when deleting a file's attribute
         from cache */
      if(prev_attr_element_ptr != NULL)
      {
        prev_attr_element_ptr->next_ptr = next_attr_element_ptr;
      }
      else
      {
        attr_cache_table_ptr->item_ptr = next_attr_element_ptr;
      }
      mmgsdi_cache_delete_file_attr_cache_item(&curr_attr_cache_element_ptr);
    }
    else
    {
      prev_attr_element_ptr = curr_attr_cache_element_ptr;
    }
    curr_attr_cache_element_ptr = next_attr_element_ptr;
  }
  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return MMGSDI_SUCCESS;
} /* mmgsdi_cache_delete_attr_selective_items */


/*===========================================================================
FUNCTION MMGSDI_CACHE_DELETE_COMMON_FILE_ATTR_CACHE

DESCRIPTION:
  This function clears the common files attributes cache

DEPENDENCIES:
  None

LIMITATIONS:
   None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None

CRITICAL SECTIONS:
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cache_delete_common_file_attr_cache(
  mmgsdi_slot_id_enum_type slot_id)
{
  mmgsdi_return_enum_type  mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_attr_cache_type  *attr_cache_tbl_ptr = NULL;

  UIM_MSG_HIGH_0("mmgsdi_cache_delete_common_file_attr_cache");

  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      attr_cache_tbl_ptr = mmgsdi_slot1_common_cache_tbl_ptr;
      break;
    case MMGSDI_SLOT_2:
      attr_cache_tbl_ptr = mmgsdi_slot2_common_cache_tbl_ptr;
      break;
    case MMGSDI_SLOT_3:
      attr_cache_tbl_ptr = mmgsdi_slot3_common_cache_tbl_ptr;
      break;
    default:
      UIM_MSG_ERR_1("Invalid MMGSDI slot 0x%x", slot_id);
      return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_cache_delete_attr_selective_items(
                    attr_cache_tbl_ptr, NULL, 0);
  return mmgsdi_status;
} /* mmgsdi_cache_delete_common_file_attr_cache */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE_COMMON_FILE_CACHE

   DESCRIPTION:
     This function deletes all the files in the common file cache and common
     attributes cache for the slot

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_common_file_cache(
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_cache_element_type *cache_table_ptr = NULL;
  uint32                      i              = 0;

  UIM_MSG_HIGH_1("mmgsdi_cache_delete_common_file_cache slot_id 0x%x", slot_id);

  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      cache_table_ptr = mmgsdi_common_cache_slot1;
      break;

    case MMGSDI_SLOT_2:
      cache_table_ptr = mmgsdi_common_cache_slot2;
      break;

    case MMGSDI_SLOT_3:
      cache_table_ptr = mmgsdi_common_cache_slot3;
      break;

    default:
      return MMGSDI_ERROR;
  }


  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  for(i = 0; i < MMGSDI_NUM_FILES_IN_COMMON_CACHE; i++)
  {
    cache_table_ptr[i].init = MMGSDI_CACHE_NOT_INIT;
    cache_table_ptr[i].in_synch = FALSE;
    cache_table_ptr[i].total_len = 0;
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_table_ptr[i].data_ptr);
  }

  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  /* Common file cache has been deleted. Now proceed with deleting common
     attributes cache */
  return mmgsdi_cache_delete_common_file_attr_cache(slot_id);
} /* mmgsdi_cache_delete_common_file_cache */


/*===========================================================================
FUNCTION MMGSDI_CACHE_DELETE_PROV_FILE_ATTR_CACHE

DESCRIPTION:
  This function clears the provisioning session specific file attributes cache
  except listed in the input parameters' "Not to delete" list

DEPENDENCIES:
  None

LIMITATIONS:
   None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None

CRITICAL SECTIONS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_prov_file_attr_cache(
  mmgsdi_session_type_enum_type session_type,
  const mmgsdi_file_enum_type  *files_not_to_delete_ptr,
  uint16                        num_files
)
{
  mmgsdi_return_enum_type  mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_attr_cache_type  *attr_cache_table_ptr = NULL;

  UIM_MSG_HIGH_1("mmgsdi_cache_delete_prov_file_attr_cache: session_type=0x%x",
                 session_type);

  switch(session_type)
  {
    case MMGSDI_1X_PROV_PRI_SESSION:
      attr_cache_table_ptr = mmgsdi_pri_1x_cache_tbl_ptr;
      break;
    case MMGSDI_1X_PROV_SEC_SESSION:
      attr_cache_table_ptr = mmgsdi_sec_1x_cache_tbl_ptr;
      break;
    case MMGSDI_1X_PROV_TER_SESSION:
      attr_cache_table_ptr = mmgsdi_ter_1x_cache_tbl_ptr;
      break;
    case MMGSDI_GW_PROV_PRI_SESSION:
      attr_cache_table_ptr = mmgsdi_pri_gw_cache_tbl_ptr;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      attr_cache_table_ptr = mmgsdi_sec_gw_cache_tbl_ptr;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      attr_cache_table_ptr = mmgsdi_ter_gw_cache_tbl_ptr;
      break;
    default:
      UIM_MSG_ERR_1("Invalid session_type 0x%x", session_type);
      return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_cache_delete_attr_selective_items(
                    attr_cache_table_ptr, files_not_to_delete_ptr, num_files);
  return mmgsdi_status;
}/* mmgsdi_cache_delete_prov_file_attr_cache */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELTE_ITEM

   DESCRIPTION:
     This function delete specific file from cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_item(
  mmgsdi_session_id_type        session_id,
  uint16                        num_files,
  const mmgsdi_file_enum_type  *mmgsdi_file_ptr
)
{
  mmgsdi_return_enum_type       mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_session_type_enum_type session_type   = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_aid_type               aid_type;
  int32                         i              = 0;
  int32                         file_index     = 0;
  int32                         cache_size      = 0;
  mmgsdi_cache_element_type    *cache_table_ptr = NULL;

  UIM_MSG_HIGH_0("mmgsdi_cache_delete_item");

  memset(&aid_type, 0x00, sizeof(mmgsdi_aid_type));

  if(num_files > 0)
  {
    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_file_ptr);
  }
  else
  {
    /* no file to delete */
    return MMGSDI_SUCCESS;
  }

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               &aid_type);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  for(i = 0; i < num_files; i++)
  {
    mmgsdi_status = mmgsdi_cache_get_cache_ptrs_and_index(mmgsdi_file_ptr[i],
                                                         session_type,
                                                         aid_type.app_type,
                                                         &cache_table_ptr,
                                                         &cache_size,
                                                         &file_index);
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (cache_table_ptr == NULL) ||
        (file_index >= cache_size))
    {
      /* cannot find the file, return error */
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    cache_table_ptr[file_index].init = MMGSDI_CACHE_NOT_INIT;
    cache_table_ptr[file_index].in_synch = FALSE;
    cache_table_ptr[file_index].total_len = 0;
    MMGSDIUTIL_TMC_MEM_FREE(cache_table_ptr[file_index].data_ptr);
  }
  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cache_delete_item */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE

   DESCRIPTION:
     This function deletes the cache item

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete(
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type
)
{
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_cache_element_type *cache_table_ptr = NULL;
  int32                      cache_data_len  = 0;
  mmgsdi_file_enum_type     *cache_item_ptr  = NULL;
  int32                      i               = 0;

  UIM_MSG_HIGH_2("mmgsdi_cache_delete session_type 0x%x, app_type 0x%x",
                 session_type, app_type);

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_cache_get_prov_cache_ptrs(session_type,
                                                     app_type,
                                                     &cache_table_ptr,
                                                     &cache_data_len,
                                                     &cache_item_ptr);
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (cache_table_ptr == NULL))
    {
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    for(i = 0; i < cache_data_len; i++)
    {
      cache_table_ptr[i].init = MMGSDI_CACHE_NOT_INIT;
      cache_table_ptr[i].in_synch = FALSE;
      cache_table_ptr[i].total_len = 0;
      MMGSDIUTIL_TMC_MEM_FREE(cache_table_ptr[i].data_ptr);
    }
  }while(0);

  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cache_delete */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_READ_ITEM

   DESCRIPTION:
     This function reads to the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read_item (
  mmgsdi_file_enum_type          mmgsdi_file,
  mmgsdi_app_enum_type           app_type,
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_len_type                requested_data_len,
  mmgsdi_len_type                data_offset,
  mmgsdi_len_type *              actual_data_len_ptr,
  uint8  *                       data_ptr,
  mmgsdi_cache_init_enum_type   *cache_state_ptr
)
{
  /*-----------------------------------------------------------------------------
    Initialize variables
  ------------------------------------------------------------------------------*/
  int32                      file_index      = 0;
  int32                      cache_size      = 0;
  mmgsdi_cache_element_type *cache_table_ptr = NULL;
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_SUCCESS;

  if(mmgsdi_file != MMGSDI_USIM_UST)
  {
    UIM_MSG_HIGH_3("mmgsdi_cache_read_item file 0x%x, session_type 0x%x, app_type 0x%x",
                   mmgsdi_file, session_type, app_type);
  }

  if(cache_state_ptr != NULL)
  {
    *cache_state_ptr = MMGSDI_CACHE_MAX_ENUM;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(actual_data_len_ptr, data_ptr);

  if (data_offset < 0)
  {
    UIM_MSG_ERR_1("Invalid Data Offset %d < 0", data_offset);
    return MMGSDI_ERROR;
  }

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_cache_get_cache_ptrs_and_index(mmgsdi_file,
                                                         session_type,
                                                         app_type,
                                                         &cache_table_ptr,
                                                         &cache_size,
                                                         &file_index);
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (cache_table_ptr == NULL))
    {
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    /*-----------------------------------------------------------------------------
      Handle Cache Not Init
    ------------------------------------------------------------------------------*/
    if (cache_table_ptr[file_index].init == MMGSDI_CACHE_NOT_INIT)
    {
      UIM_MSG_ERR_1("Cache not initialized 0x%x", mmgsdi_file);
      mmgsdi_status = MMGSDI_NOT_INIT;
      break;
    } /* Cache Not Init */
    /* File not found on card */
    else if( cache_table_ptr[file_index].init == MMGSDI_CACHE_NOT_FOUND )
    {
      UIM_MSG_HIGH_1("Cache status indicates file not found on card 0x%x",
                     mmgsdi_file);

      if(cache_state_ptr != NULL)
      {
        *cache_state_ptr = MMGSDI_CACHE_NOT_FOUND;
      }
      mmgsdi_status = MMGSDI_NOT_FOUND;
      break;
    }

    /*-----------------------------------------------------------------------------
      Handle Incorrect Paramters
    -----------------------------------------------------------------------------*/
    if (data_offset+requested_data_len > cache_table_ptr[file_index].total_len)
    {
      UIM_MSG_ERR_3("Requested data len 0x%x + Offset 0x%x > cache file size 0x%x",
                    requested_data_len,
                    data_offset,
                    cache_table_ptr[file_index].total_len);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }  /* Incorrect Paramters */

    /*-----------------------------------------------------------------------------
      Proceed with Read
    ------------------------------------------------------------------------------*/
    if (requested_data_len == 0)
    {
      if (cache_table_ptr[file_index].total_len <= data_offset)
      {
        UIM_MSG_ERR_2("Data offset 0x%x same or greater than total len 0x%x",
                      data_offset, cache_table_ptr[file_index].total_len);
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }
      /* When requested data_len is 0, it means that whole file should
      be returned */
      *actual_data_len_ptr = cache_table_ptr[file_index].total_len - data_offset;
    } /* end if - requested_data_len is 0 */
    else
    {
      *actual_data_len_ptr = requested_data_len;
    } /* end else - requested data len is not 0 */

    if (*actual_data_len_ptr < 0)
    {
      UIM_MSG_ERR_1("Invalid Data Len requested %d < 0", *actual_data_len_ptr);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Copy from cache */
    mmgsdi_memscpy(data_ptr,
                   int32touint32(*actual_data_len_ptr),
                   (cache_table_ptr[file_index].data_ptr) + data_offset,
                   int32touint32(*actual_data_len_ptr));
  }while(0);

  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cache_read_item */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_WRITE

   DESCRIPTION:
     This function converts the mmgsdi file enum type to uim enum type.
     And write data to different caches based on the applications available
     on the card by calling the corresding util write cache for each
     technology

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, all the caches will be
     updated

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_write(
  mmgsdi_session_id_type        session_id,
  const mmgsdi_access_type     *file_access_ptr,
  mmgsdi_data_type              data,
  mmgsdi_len_type               offset,
  mmgsdi_data_from_enum_type    data_fr_card
)
{
  mmgsdi_return_enum_type       mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_session_type_enum_type session_type   = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_aid_type               aid_type;

  memset(&aid_type, 0x00, sizeof(mmgsdi_aid_type));

  MMGSDIUTIL_RETURN_IF_NULL(file_access_ptr);

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               &aid_type);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(file_access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS)
  {

    UIM_MSG_ERR_1("Skip Cache: not by EF ENUM ACCESS: 0x%x",
                  file_access_ptr->access_method);
    return MMGSDI_ERROR;
  }

  /* Do TMSI PLMN check for EF-LOCI before writing to cache */
  if(file_access_ptr->file.file_enum == MMGSDI_USIM_LOCI ||
     file_access_ptr->file.file_enum == MMGSDI_GSM_LOCI)
  {
    (void)mmgsdi_util_tmsi_plmn_check(session_id,
                                      &data);
  }

  mmgsdi_status = mmgsdi_cache_write_item(file_access_ptr->file.file_enum,
                                          aid_type.app_type,
                                          session_type,
                                          data.data_len,
                                          offset,
                                          data.data_ptr,
                                          data_fr_card);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

#ifdef FEATURE_UIM_PERSISTENT_CACHE
  /* update presistent cache when data from card is completed. To handle the case
     for offset > 0, entire data will need to be read from cache and then write to
     persistent cache.

     for MMGSDI_DATA_FROM_CLIENT doesn't trigger a persistent cache update as cache will
     be updated first, then queue a request to card, thus persistent cache will be written
     here after writing to the card; for MMGSDI_DATA_FROM_CARD_INCOMPLETE, if persistent
     cache is not found for the file, then skip the update.
  */

  (void)mmgsdi_cache_update_persistent_cache(session_id,
                                             file_access_ptr,
                                             data_fr_card);
#endif /* FEATURE_UIM_PERSISTENT_CACHE*/

  return mmgsdi_status;
} /* mmgsdi_cache_write */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_READ

   DESCRIPTION:
     This function converts the mmgsdi file enum type to uim enum type.
     And read data to different caches based on the applications available
     on the card by calling the corresding util read cache for each
     technology

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, it will return the data
     from the first cache

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read(
  mmgsdi_session_id_type        session_id,
  const mmgsdi_access_type     *file_access_ptr,
  mmgsdi_data_type             *data_ptr,
  mmgsdi_len_type               offset,
  mmgsdi_cache_init_enum_type  *cache_state_ptr
)
{
  mmgsdi_return_enum_type       mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_session_type_enum_type session_type   = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_aid_type               aid_type;

  memset(&aid_type, 0x00, sizeof(mmgsdi_aid_type));

  if(cache_state_ptr != NULL)
  {
    *cache_state_ptr = MMGSDI_CACHE_MAX_ENUM;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(file_access_ptr, data_ptr);

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               &aid_type);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(file_access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS)
  {

    UIM_MSG_ERR_1("Skip Cache: not by EF ENUM ACCESS: 0x%x",
                  file_access_ptr->access_method);
    return MMGSDI_ERROR;
  }

  return mmgsdi_cache_read_item(file_access_ptr->file.file_enum,
                                aid_type.app_type,
                                session_type,
                                data_ptr->data_len,
                                offset,
                                &data_ptr->data_len,
                                data_ptr->data_ptr,
                                cache_state_ptr);
} /* mmgsdi_cache_read */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_GET_RECORD_LEN

   DESCRIPTION:
     This function will determine if the file and record are cached and return
     the record len in the pointer provided

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_get_record_len(
  mmgsdi_session_id_type session_id,
  mmgsdi_access_type     access,
  mmgsdi_len_type        rec_num,
  mmgsdi_len_type       *rec_len_p
)
{
  mmgsdi_record_cache_node_list_type * record_cache_ptr = NULL;
  mmgsdi_record_cache_node_type      * current_node_p   = NULL;
  mmgsdi_file_enum_type                EF_name          = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_session_type_enum_type        session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cache_get_prov_record_cache_ptr(session_type,
                                                        &record_cache_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    EF_name = access.file.file_enum;
  }
  else
  {
    UIM_MSG_HIGH_0("Access other than enum_access not supported in cache");
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     Validate the input parameters and set the return value to a default
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(rec_len_p);
  *rec_len_p = 0;

  /* --------------------------------------------------------------------------
     check to see if mmgsdi_record_cache_list_node has been set with a
     valid Record entry
     ------------------------------------------------------------------------*/
  if ( record_cache_ptr->head_p == NULL )
  {
    UIM_MSG_HIGH_0("Records Entries have not been cahced");
    return MMGSDI_NOT_FOUND;
  }

  current_node_p = record_cache_ptr->head_p;

  /* --------------------------------------------------------------------------
    current_node_p can not be null because mmgsdi_record_cache_list_node
    is a global variable.  Can start checking with Head.
    -------------------------------------------------------------------------*/
  do
  {
    if (( current_node_p->record_data.EF_name == EF_name ) &&
        ( current_node_p->record_data.rec_num == rec_num ))
    {
      *rec_len_p = current_node_p->record_data.record_data.data_len;
      UIM_MSG_HIGH_1("Cache record found: REC LEN = 0x%x", *rec_len_p);
      return MMGSDI_SUCCESS;
    }

    /* Match is not made...go ahead and assign the next pointer
    ** to the current pointer so that it can be checked.
    */
    current_node_p = current_node_p->next_p;

  } while (current_node_p != NULL  );

  return MMGSDI_NOT_FOUND;
}/* mmgsdi_cache_get_record_len */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_ADD_RECORD

   DESCRIPTION:
     This function checks if the data requested to be added already exist in
     the cache or not and if not, it will add the data to the cache

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_add_record(
  mmgsdi_session_id_type  session_id,
  mmgsdi_access_type      access,
  mmgsdi_len_type         rec_num,
  const mmgsdi_data_type *data_ptr)
{
  mmgsdi_record_cache_node_list_type * record_cache_ptr  = NULL;
  mmgsdi_record_cache_node_type      * new_node_p        = NULL;
  mmgsdi_return_enum_type              mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_file_enum_type                EF_name           = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_session_info_type           * session_info_ptr  = NULL;
  mmgsdi_slot_id_enum_type             slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_data_type                     cache_data;

  memset(&cache_data, 0, sizeof(mmgsdi_data_type));

  /* Get the slot related to the session */
  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS ||
      session_info_ptr == NULL ||
      slot_id == MMGSDI_MAX_SLOT_ID_ENUM)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cache_get_prov_record_cache_ptr(session_info_ptr->session_type,
                                                         &record_cache_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Validate input parameters:
     1)  data_ptr can not be NULL
     2)  data_ptr->data_len must not exceed 255 Bytes (apdu limiation)
     3) Initialize any variables that need initialization
     ------------------------------------------------------------------------*/
  if(access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    EF_name = access.file.file_enum;
  }
  else
  {
    UIM_MSG_HIGH_0("Access other than enum_access not supported in cache");
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(data_ptr);

  if ((data_ptr->data_len > MMGSDI_MAX_REC_LEN) || (data_ptr->data_len <=0))
  {
    UIM_MSG_ERR_2("Unexpected Length : 0x%x with max value : 0x%x",
                  data_ptr->data_len, MMGSDI_MAX_REC_LEN);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     find if the data is already in the list.
     No need to cache if it is already cached.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cache_get_record(session_id,
                                          access,
                                          rec_num,
                                          NULL);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("Already has the data, no need to add, 0x%x, 0x%x",
                   EF_name, rec_num);
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Allocate memory for a node to be linked in to the list
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_node_p, sizeof(mmgsdi_record_cache_node_type))

  if (new_node_p == NULL )
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    new_node_p->record_data.record_data.data_ptr,
    data_ptr->data_len)

  if (new_node_p->record_data.record_data.data_ptr == NULL )
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(new_node_p);
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* --------------------------------------------------------------------------
     Set the Node Pointer information
     ------------------------------------------------------------------------*/
  new_node_p->next_p  = NULL;
  new_node_p->record_data.EF_name = EF_name;
  new_node_p->record_data.rec_num = rec_num;
  new_node_p->record_data.record_data.data_len = data_ptr->data_len;
  mmgsdi_memscpy(new_node_p->record_data.record_data.data_ptr,
                 int32touint32(data_ptr->data_len),
                 data_ptr->data_ptr,
                 int32touint32(data_ptr->data_len));

  /* --------------------------------------------------------------------------
     First element
     ----------------------------------------------------------------------- */
  if ( record_cache_ptr->head_p == NULL )
  {
    record_cache_ptr->head_p = new_node_p;
    record_cache_ptr->tail_p = new_node_p;
  }
  else
  {
    /* --------------------------------------------------------------------------
       From this point on...running because this is not the first time
       an entry is being made into this list.  start by locating the tail
       of the list.  The new node will be linked to the tail to become the new
       tail.
       ----------------------------------------------------------------------- */
    /* --------------------------------------------------------------------------
       Now link the new_node_p to the tail of the list.
       ------------------------------------------------------------------------*/
    record_cache_ptr->tail_p->next_p = new_node_p;
    record_cache_ptr->tail_p = new_node_p;
  }

#ifdef FEATURE_UIM_PERSISTENT_CACHE
  /* persistent cache will be created from mmgsdi_csg_build_persistent_cache
     after all CSG files are read to the cache. This is to make sure persistent
	 cache has all the records in case there's a refresh before all CSG files
	 and records are read.
	 do not update CSG files if it is not yet in persistent cache */
  if(EF_name == MMGSDI_USIM_ACSGL || EF_name == MMGSDI_USIM_CSGT  ||
     EF_name == MMGSDI_USIM_HNBN  || EF_name == MMGSDI_USIM_OCSGL ||
     EF_name == MMGSDI_USIM_OCSGT || EF_name == MMGSDI_USIM_OHNBN)
  {
    mmgsdi_status = mmgsdi_persistent_cache_read(slot_id,
                                                 EF_name,
                                                 int32touint16(rec_num),
                                                 &cache_data.data_len,
                                                 &cache_data.data_ptr);

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_data.data_ptr);
  }

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    (void)mmgsdi_persistent_cache_write(slot_id,
                                        EF_name,
                                        int32touint16(rec_num),
                                        new_node_p->record_data.record_data.data_len,
                                        (const uint8 *)new_node_p->record_data.record_data.data_ptr);
  }
#endif /* FEATURE_UIM_PERSISTENT_CACHE*/
  return MMGSDI_SUCCESS;
}/* mmgsdi_cache_add_record */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_GET_RECORD

   DESCRIPTION:
     This function get the data from the record cache if a match is found.
     If the input data_ptr is NULL, the function checks if the record
     exists in cache.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_get_record(
  mmgsdi_session_id_type session_id,
  mmgsdi_access_type     access,
  mmgsdi_len_type        rec_num,
  mmgsdi_data_type      *data_ptr)
{
  mmgsdi_record_cache_node_list_type * record_cache_ptr = NULL;
  mmgsdi_record_cache_node_type      * current_node_p   = NULL;
  mmgsdi_file_enum_type                EF_name          = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_session_type_enum_type        session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cache_get_prov_record_cache_ptr(session_type,
                                                        &record_cache_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    EF_name = access.file.file_enum;
  }
  else
  {
    access.file.file_enum = (mmgsdi_file_enum_type)UIM_EF_BY_PATH;
    return MMGSDI_ERROR;
  }

  current_node_p = record_cache_ptr->head_p;

  if ( current_node_p == NULL )
  {
    UIM_MSG_ERR_0("MMGSDI_NOT_FOUND: Record Cache not populated");
    return MMGSDI_NOT_FOUND;
  }

  do
  {
    if (( current_node_p->record_data.EF_name == EF_name ) &&
        ( current_node_p->record_data.rec_num == rec_num ))
    {
      /* The data_ptr can be NULL if the caller is interested
         only in knowing if the record exists in cache */
      if(data_ptr == NULL)
      {
        return MMGSDI_SUCCESS;
      }

      /* Get the data, if the data len passed in is greater than the
        cache data, return the cached data and update the data len to
        reflect the actual data being copied, else copy the requested data len*/
      if (data_ptr->data_len > current_node_p->record_data.record_data.data_len)
      {
        data_ptr->data_len = current_node_p->record_data.record_data.data_len;
      }
      if(data_ptr->data_ptr != NULL)
      {
        mmgsdi_memscpy(data_ptr->data_ptr,
                int32touint32(data_ptr->data_len),
                current_node_p->record_data.record_data.data_ptr,
                int32touint32(data_ptr->data_len));
        return MMGSDI_SUCCESS;
      }
    }

    current_node_p = current_node_p->next_p;
  } while ( current_node_p != NULL  );

  return MMGSDI_NOT_FOUND;
}/* mmgsdi_cache_get_record */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE_RECORD

   DESCRIPTION:
     This function delete the record cache requested by the caller

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_delete_record(
  mmgsdi_session_id_type session_id,
  mmgsdi_access_type     access,
  mmgsdi_len_type        rec_num)
{
  mmgsdi_record_cache_node_list_type * record_cache_ptr = NULL;
  mmgsdi_record_cache_node_type      * next_node_p      = NULL;
  mmgsdi_record_cache_node_type      * current_node_p   = NULL;
  mmgsdi_file_enum_type                EF_name          = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_session_type_enum_type        session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cache_get_prov_record_cache_ptr(session_type,
                                                        &record_cache_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    EF_name = access.file.file_enum;
  }
  else
  {
    UIM_MSG_HIGH_0("Access other than enum_access not supported in cache");
    return MMGSDI_ERROR;
  }

  current_node_p = record_cache_ptr->head_p;

  if ( current_node_p == NULL )
  {
    /* this case is to cover the first node when nothing has been added yet */
    UIM_MSG_ERR_2("0x%x, 0x%x not in record cache for deletion",
                  EF_name, rec_num);
    return MMGSDI_NOT_FOUND;
  }

  next_node_p = current_node_p->next_p;

  if (next_node_p == NULL)
  {
    /* only 1 element in the list */
    if (( current_node_p->record_data.EF_name == EF_name ) &&
        ( current_node_p->record_data.rec_num == rec_num ))
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(current_node_p->record_data.record_data.data_ptr);
      current_node_p->record_data.record_data.data_len = 0;
      current_node_p->next_p = NULL;
      MMGSDIUTIL_TMC_MEM_FREE(current_node_p);
      record_cache_ptr->head_p = NULL;
      record_cache_ptr->tail_p = NULL;
      return MMGSDI_SUCCESS;
    }
    return MMGSDI_NOT_FOUND;
  }
  while ( next_node_p != NULL )
  {
    if (( next_node_p->record_data.EF_name == EF_name ) &&
        ( next_node_p->record_data.rec_num == rec_num ))
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(next_node_p->record_data.record_data.data_ptr);
      next_node_p->record_data.record_data.data_len = 0;
      current_node_p->next_p = next_node_p->next_p;
      if (current_node_p->next_p == NULL) {
        record_cache_ptr->tail_p = current_node_p;
      }
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(next_node_p);
      return MMGSDI_SUCCESS;
    }
    current_node_p = current_node_p->next_p;
    next_node_p = next_node_p->next_p;
  }
  return MMGSDI_NOT_FOUND;
}/* mmgsdi_cache_delete_record */


/* ==========================================================================
   FUNCTION:      MMGSDI_CACHE_DELETE_RECORD_SELECTIVE_ITEMS

   DESCRIPTION:
     This function delete the entire record cache except listed in the input
     parameters' "Not to delete" list

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_cache_delete_record_selective_items(
  mmgsdi_session_id_type        session_id,
  const mmgsdi_file_enum_type  *files_not_to_delete_ptr,
  uint16                        num_files
)
{
  mmgsdi_record_cache_node_list_type * record_cache_ptr = NULL;
  mmgsdi_record_cache_node_type      * current_node_p   = NULL;
  mmgsdi_record_cache_node_type      * next_node_p      = NULL;
  mmgsdi_record_cache_node_type      * prev_node_p      = NULL;
  mmgsdi_session_type_enum_type        session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;
  int32                                index            = 0;
  boolean                              match            = FALSE;

  if(num_files > 0 && files_not_to_delete_ptr == NULL)
  {
    num_files = 0;
  }

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               NULL);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  mmgsdi_status = mmgsdi_cache_get_prov_record_cache_ptr(session_type,
                                                        &record_cache_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  current_node_p = record_cache_ptr->head_p;

  if ( current_node_p == NULL )
  {
    /* this case is to cover the first node when nothing has been added yet */
    UIM_MSG_HIGH_0("nothing to be delete");
    return;
  }

  while ( current_node_p != NULL )
  {
    next_node_p = current_node_p->next_p;

    match = FALSE;
    for (index = 0; index < num_files; index++)
    {
      if(current_node_p->record_data.EF_name == files_not_to_delete_ptr[index])
      {
        /* item matches the file that should not be deleted,
           skip to the next file */
        match = TRUE;
        break;
      }
    }

    if(!match)
    {
      /* Relink the list of cache elements when deleting a file's attribute
         from cache */
      if(prev_node_p != NULL)
      {
        prev_node_p->next_p = next_node_p;
      }
      else
      {
        record_cache_ptr->head_p = next_node_p;
      }

      /* Update tail if we are removing tail node */
      if(next_node_p == NULL)
      {
        record_cache_ptr->tail_p = prev_node_p;
      }

      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(current_node_p->record_data.record_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(current_node_p);
    }
    else
    {
      prev_node_p = current_node_p;
    }
    current_node_p = next_node_p;
  }
}/* mmgsdi_cache_delete_record_selective_items */


/* ==========================================================================
FUNCTION:      MMGSDI_CACHE_GET_ATTR_CACHE

DESCRIPTION:
  This function gets the appropriate attributes cache and locates the file
  (enum or path) in the cache.

DEPENDENCIES
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:  if cache was found for the file enum/path, return valid
                     attr_element_cache_pptr and item_index_ptr and
                     attr_cache_tbl_pptr are valid
    MMGSDI_ERROR:    no matching cache at all
    MMGSDI_NOT_INIT: cache was found but the file enum/path is not in the cache
                     yet, only attr_cache_tbl_pptr is valid

SIDE EFFECTS:
  None

CRITICAL SECTIONS:
   The calling function must enter mmgsdi_cache_crit_sect crit
   sect before invoking this util function. Protection for attributes cache.
==========================================================================*/
static mmgsdi_return_enum_type  mmgsdi_cache_get_attr_cache(
  mmgsdi_session_id_type             session_id,
  const mmgsdi_access_type          *access_ptr,
  mmgsdi_slot_id_enum_type           slot,
  mmgsdi_attr_cache_element_type   **attr_element_cache_pptr,
  mmgsdi_attr_cache_type           **attr_cache_tbl_pptr)
{
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_session_info_type          *session_info_ptr    = NULL;
  mmgsdi_int_app_info_type          *app_info_ptr        = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_3(attr_element_cache_pptr, attr_cache_tbl_pptr, access_ptr);

  if((access_ptr->access_method != MMGSDI_BY_PATH_ACCESS &&
      access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS) ||
     (access_ptr->access_method == MMGSDI_BY_PATH_ACCESS &&
      access_ptr->file.path_type.path_len == 0))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Check if file is common file, if so go to common cache,
     i.e., MF, TELECOM, ORANGESIM, CINGULARSIM, DCS1800, HZI */
  if (mmgsdi_file_is_ef_under_mf(access_ptr) ||
      mmgsdi_file_is_ef_under_df_in_mf(access_ptr))
  {
    /* check if we should be comparing with the MF attr cache instead */
    switch(slot)
    {
      case MMGSDI_SLOT_1:
        *attr_cache_tbl_pptr = mmgsdi_slot1_common_cache_tbl_ptr;
        break;
      case MMGSDI_SLOT_2:
        *attr_cache_tbl_pptr = mmgsdi_slot2_common_cache_tbl_ptr;
        break;
      case MMGSDI_SLOT_3:
        *attr_cache_tbl_pptr = mmgsdi_slot3_common_cache_tbl_ptr;
        break;
      default:
        UIM_MSG_ERR_1("Invalid MMGSDI slot 0x%x", slot);
        return MMGSDI_ERROR;
    }
  }
  else
  {
    /* We need to determine the app_type and session_type from session_id */
    mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                     NULL,
                                                     NULL,
                                                     &app_info_ptr,
                                                     NULL,
                                                     &session_info_ptr,
                                                     NULL);
    if((mmgsdi_status != MMGSDI_SUCCESS) || !app_info_ptr || !session_info_ptr)
    {
      return MMGSDI_ERROR;
    }

    switch(app_info_ptr->app_data.app_type)
    {
      case MMGSDI_APP_CSIM:
      case MMGSDI_APP_RUIM:
        switch(session_info_ptr->session_type)
        {
          case MMGSDI_1X_PROV_PRI_SESSION:
            *attr_cache_tbl_pptr = mmgsdi_pri_1x_cache_tbl_ptr;
            break;
          case MMGSDI_1X_PROV_SEC_SESSION:
            *attr_cache_tbl_pptr = mmgsdi_sec_1x_cache_tbl_ptr;
            break;
          case MMGSDI_1X_PROV_TER_SESSION:
            *attr_cache_tbl_pptr = mmgsdi_ter_1x_cache_tbl_ptr;
            break;
          default:
            UIM_MSG_ERR_2("Invalid session_type 0x%x for APP_RUIM/APP_CSIM: 0x%x",
                          session_info_ptr->session_type, app_info_ptr->app_data.app_type);
            return MMGSDI_ERROR;
        }
        break;

      case MMGSDI_APP_SIM:
      case MMGSDI_APP_USIM:
        switch(session_info_ptr->session_type)
        {
          case MMGSDI_GW_PROV_PRI_SESSION:
            *attr_cache_tbl_pptr = mmgsdi_pri_gw_cache_tbl_ptr;
            break;
          case MMGSDI_GW_PROV_SEC_SESSION:
            *attr_cache_tbl_pptr = mmgsdi_sec_gw_cache_tbl_ptr;
            break;
          case MMGSDI_GW_PROV_TER_SESSION:
            *attr_cache_tbl_pptr = mmgsdi_ter_gw_cache_tbl_ptr;
            break;
          default:
            UIM_MSG_ERR_2("Invalid session_type 0x%x for APP_SIM/APP_USIM: 0x%x",
                          session_info_ptr->session_type, app_info_ptr->app_data.app_type);
            return MMGSDI_ERROR;
        }
        break;

      default:
        UIM_MSG_ERR_1("Invalid app type 0x%x", app_info_ptr->app_data.app_type);
        return MMGSDI_ERROR;
    }
  }

  if(*attr_cache_tbl_pptr == NULL)
  {
    UIM_MSG_ERR_0("Null *attr_cache_tbl_pptr");
    return MMGSDI_ERROR;
  }

  *attr_element_cache_pptr = (*attr_cache_tbl_pptr)->item_ptr;
  while(*attr_element_cache_pptr != NULL)
  {
    if(FALSE != mmgsdi_file_compare_access_type(
                  &(*attr_element_cache_pptr)->access,
                  access_ptr))
    {
      return MMGSDI_SUCCESS;
    }
    *attr_element_cache_pptr = (*attr_element_cache_pptr)->next_ptr;
  }

  return MMGSDI_NOT_INIT;
} /* mmgsdi_cache_get_attr_cache */


/* ==========================================================================
FUNCTION:      MMGSDI_CACHE_CREATE_FILE_ATTR_CACHE_ELEMENT

DESCRIPTION:
  This function allocates memory for a file attribute cache element and
  assigns the item with the passed in parameter

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:  If the element is created successfully
    MMGSDI_ERROR:    If the memory allocation failed

SIDE EFFECTS:

==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cache_create_file_attr_cache_element(
  const mmgsdi_access_type        *access_ptr,
  mmgsdi_attr_cache_type          *cache_attr_tbl_ptr,
  mmgsdi_attr_cache_element_type **cache_attr_element_pptr)
{
  mmgsdi_attr_cache_element_type *new_attr_element_ptr = NULL;
  mmgsdi_attr_cache_element_type *tmp_attr_element_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_2(cache_attr_tbl_ptr, access_ptr);

  if((access_ptr->access_method != MMGSDI_BY_PATH_ACCESS &&
      access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS) ||
     (access_ptr->access_method == MMGSDI_BY_PATH_ACCESS &&
      access_ptr->file.path_type.path_len == 0))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_attr_element_ptr,
    sizeof(mmgsdi_attr_cache_element_type));
  if (new_attr_element_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  if (cache_attr_element_pptr)
  {
    *cache_attr_element_pptr = new_attr_element_ptr;
  }

  if (mmgsdi_util_copy_access_type(&new_attr_element_ptr->access,
                                   access_ptr) != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(new_attr_element_ptr);
    return MMGSDI_ERROR;
  }

  if(cache_attr_tbl_ptr->item_ptr == NULL)
  {
    /* first element in the table */
    cache_attr_tbl_ptr->item_ptr = new_attr_element_ptr;
  }
  else
  {
    tmp_attr_element_ptr = cache_attr_tbl_ptr->item_ptr;
    while(tmp_attr_element_ptr->next_ptr != NULL)
    {
      tmp_attr_element_ptr = tmp_attr_element_ptr->next_ptr;
    }
    tmp_attr_element_ptr->next_ptr = new_attr_element_ptr;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_cache_create_file_attr_cache_element */


/*===========================================================================
FUNCTION MMGSDI_CACHE_ALLOC_AND_POPULATE_FILE_ATTR_CACHE

INPUT
  mmgsdi_file_security_type  security_status_input
  mmgsdi_file_security_type *security_status_output_ptr

OUTPUT
  void

DESCRIPTION

RETURN VALUE
  void

CRITICAL SECTIONS:
   The calling function must enter mmgsdi_cache_crit_sect crit
   sect when security_status_output_ptr is from attributes cache
   before invoking this util function. Protection for attributes cache.
===========================================================================*/
void mmgsdi_cache_alloc_and_populate_file_attr_cache(
  mmgsdi_file_security_type  security_status_input,
  mmgsdi_file_security_type *security_status_output_ptr)
{
  uint32                  pin_data_size = 0;

  if(security_status_output_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null security_status_output_ptr");
    return;
  }

  security_status_output_ptr->protection_method  = MMGSDI_ALWAYS_ALLOWED;
  security_status_output_ptr->num_protection_pin = 0;
  security_status_output_ptr->protection_pin_ptr = NULL;

  if(security_status_input.num_protection_pin > 0)
  {
    if(security_status_input.protection_pin_ptr == NULL)
    {
      /* If num_protection_pin indicates there is at least
         one pin then protection_pin_ptr cannot be NULL */
      UIM_MSG_ERR_0("Null security_status_input.protection_pin_ptr with non-zero num_protection_pin");
      return;
    }
    pin_data_size = int32touint32(security_status_input.num_protection_pin)*
                    sizeof(mmgsdi_pin_enum_type);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      security_status_output_ptr->protection_pin_ptr,
      pin_data_size);
    if(security_status_output_ptr->protection_pin_ptr == NULL)
    {
      return;
    }

    mmgsdi_memscpy((void*)security_status_output_ptr->protection_pin_ptr,
                   pin_data_size,
                   (void*)security_status_input.protection_pin_ptr,
                   pin_data_size);
  }
  security_status_output_ptr->num_protection_pin =
    security_status_input.num_protection_pin;
  security_status_output_ptr->protection_method =
    security_status_input.protection_method;
} /* mmgsdi_cache_alloc_and_populate_file_attr_cache */


/*===========================================================================
FUNCTION MMGSDI_CACHE_POPULATE_ATTR_CACHE_IF_NEEDED

DESCRIPTION
  Checks for file attributes cache, if found cache ptr is returned. If not
  found and if it is a file in the cache list. Get File attributes is perfomed
  on that EF. Get File attributes is performed only if it is a internal call

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS
    MMGSDI_ERROR

SIDE EFFECTS
  If the calling function expects the attributes data, then *cache_attr_pptr
  will hold the attributes and the calling function should take care of freeing
  this ptr along with its member ptrs.

CRITICAL SECTIONS:
  The function uses mmgsdi_cache_crit_sect to protect accesses to
  the File attributes cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_populate_attr_cache_if_needed(
  mmgsdi_client_id_type            client_id,
  mmgsdi_session_id_type           session_id,
  mmgsdi_slot_id_enum_type         slot,
  boolean                          skip_uicc_arr,
  boolean                          is_select_mandatory,
  const mmgsdi_access_type        *access_ptr,
  mmgsdi_attr_cache_element_type **cache_attr_pptr,
  mmgsdi_sw_status_enum_type      *sw_present_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type     *select_ptr          = NULL;
  mmgsdi_get_file_attr_cnf_type     *cnf_ptr             = NULL;
  mmgsdi_file_security_access_type  *security_access_ptr = NULL;
  boolean                           *increase_ptr        = NULL;
  boolean                            is_gsdi_task        = TRUE;
  mmgsdi_file_size_type              file_size           = 0;
  mmgsdi_len_type                    rec_len             = 0;
  mmgsdi_rec_num_type                num_of_rec          = 0;
  mmgsdi_file_structure_enum_type    file_type           = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
  mmgsdi_file_sfi_table_type         file_info;
  uint8                              sfi                 = MMGSDI_INVALID_SFI;
  mmgsdi_protocol_enum_type          protocol            = MMGSDI_NO_PROTOCOL;

  MMGSDIUTIL_RETURN_IF_NULL(access_ptr);

  if (sw_present_ptr)
  {
    *sw_present_ptr = MMGSDI_STATUS_WORD_INVALID;
  }

  if(access_ptr->access_method != MMGSDI_BY_PATH_ACCESS &&
     access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS)
  {
    UIM_MSG_ERR_0("Cannot access cache");
    /* Return SUCCESS since the check for file attr did not fail at
      this point*/
    return MMGSDI_SUCCESS;
  }

  memset(&file_info, 0x00, sizeof(file_info));

  protocol = mmgsdi_util_determine_protocol(slot);

  /* Commands can be sent to UIM in sync mode only in GSDI context */
  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    is_gsdi_task = FALSE;
  }

  if(is_select_mandatory && !is_gsdi_task)
  {
    /* The explicit request to perform a SELECT can only come from gsdi
       task context */
    return MMGSDI_ERROR;
  }

  /* Check if the attributes are already cached */
  if(!is_select_mandatory)
  {
    mmgsdi_status = mmgsdi_cache_attr_read(session_id,
                                           slot,
                                           access_ptr,
                                           cache_attr_pptr);

    if((mmgsdi_status != MMGSDI_NOT_INIT) || !is_gsdi_task)
    {
      /* File may have been found in the attributes cache or there may be some
         error other than MMGSDI_NOT_INIT */
      return mmgsdi_status;
    }
  }

   /* File attribtues are not cached yet. Lets try to fetch the attributes
     1. First try searching the static tables for the attributes IF:
        a). access type is EF_ENUM AND
        b). caller did not mandate an explicit SELECT
     2. If not found in static table or if the caller mandated an explicit
        SELECT, get attributes by performing an explicit SELECT to the card */

  if(access_ptr->access_method == MMGSDI_EF_ENUM_ACCESS &&
     !is_select_mandatory)
  {
    /* If SFI is enabled, see if the hardcoded tables have file/rec size and
       security attributes related info */
    mmgsdi_status = mmgsdi_file_get_file_info_from_table(access_ptr->file.file_enum, &file_info);
  }
  if(MMGSDI_SUCCESS == mmgsdi_status &&
     file_info.size != MMGSDI_INVALID_FILE_SIZE &&
     mmgsdi_file_is_sfi_supported(slot))
  {
    /* We got the file/rec size info from the static SFI table. Now lets
       get the security attributes from the static table as well and
       update the update the attributes cache with all this info. */
    mmgsdi_file_attributes_type   file_attr;

    memset(&file_attr, 0x00, sizeof(file_attr));
    file_attr.file_type = file_info.file_type;
    file_type = file_info.file_type;
    sfi = file_info.sfi;

    mmgsdi_status = mmgsdi_file_set_default_sec_attr_per_spec(
                      &file_attr,
                      access_ptr->file.file_enum);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }

    switch(file_type)
    {
      case MMGSDI_LINEAR_FIXED_FILE:
        security_access_ptr = &file_attr.file_info.linear_fixed_file.file_security;
        rec_len = file_info.size;
        /* There is no way for our static SFI tables to have num_of_recs.
           For that reason num_of_rec is zero here for the files for which
           found a match in the SFI table otherwise */
        num_of_rec = 0;
        file_size = num_of_rec * rec_len;
        break;
      case MMGSDI_TRANSPARENT_FILE:
        security_access_ptr = &file_attr.file_info.transparent_file.file_security;
        file_size = file_info.size;
        break;
      case MMGSDI_CYCLIC_FILE:
        security_access_ptr = &file_attr.file_info.cyclic_file.file_security;
        increase_ptr = &file_attr.file_info.cyclic_file.increase_allowed;
        rec_len = file_info.size;
        num_of_rec = 0;
        file_size = num_of_rec * rec_len;
        break;
      default:
        UIM_MSG_ERR_1("Invalid file_type 0x%x",
                      file_attr.file_type);
        return MMGSDI_ERROR;
    }

    /* Write the attributes fetched from the static tables to the file attributes cache */
    mmgsdi_status = mmgsdi_cache_attr_write(session_id,
                                            slot,
                                            access_ptr,
                                            file_type,
                                            file_size,
                                            rec_len,
                                            num_of_rec,
                                            sfi,
                                            security_access_ptr,
                                            increase_ptr);
  }
  else
  {
    /* Will have to send the SELECT to the card to get the attributes */
    UIM_MSG_HIGH_0("Unknown Security Attr, Get File Attribute will be performed");

    /* Build the file attribute command to populate the attribute info */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
      sizeof(mmgsdi_get_file_attr_req_type));
    if (select_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    select_ptr->request_header.client_id         = client_id;
    select_ptr->request_header.session_id        = session_id;
    select_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
    select_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
    select_ptr->request_header.request_len       =
      uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
    select_ptr->request_header.payload_len        =
      select_ptr->request_header.request_len -
      uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
    select_ptr->request_header.slot_id           = slot;
    select_ptr->request_header.client_data       = 0;
    select_ptr->request_header.response_cb       = NULL;
    select_ptr->activate_aid                     = TRUE;
    /* Skip reading EF-ARR for internal calls */
    select_ptr->skip_uicc_arr                    = skip_uicc_arr;

    mmgsdi_memscpy(&select_ptr->access,
            sizeof(mmgsdi_access_type),
            access_ptr,
            sizeof(mmgsdi_access_type));

    switch(protocol)
    {
      case MMGSDI_ICC:
        mmgsdi_status = mmgsdi_uim_icc_select (select_ptr,
                                               TRUE,
                                               &cnf_ptr);
        break;
      case MMGSDI_UICC:
        mmgsdi_status = mmgsdi_uim_uicc_select (select_ptr,
                                                TRUE,
                                                &cnf_ptr,
                                                TRUE);
        break;
      default:
        UIM_MSG_HIGH_1("Invalid protocol 0x%x for file attr cache but allow continue",
                       protocol);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_ptr);
        return MMGSDI_SUCCESS;
    }

    if (cnf_ptr != NULL)
    {
      /* If mmgsdi_status is SUCCESS, it means the response_header is
         correctly populated with UIMDRV response. In that case, we can
         rely on SW present in cnf */
      if (sw_present_ptr != NULL &&
          mmgsdi_status == MMGSDI_SUCCESS)
      {
        if (cnf_ptr->response_header.status_word.present)
        {
          *sw_present_ptr = MMGSDI_STATUS_WORD_PRESENT;
        }
        else
        {
          *sw_present_ptr = MMGSDI_STATUS_WORD_ABSENT;
        }
      }
      mmgsdi_status = cnf_ptr->response_header.mmgsdi_status;
    }

    /* The file attributes would already have been cached while handling the
       CNF for this SELECT request... so no need to cache them here again */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_ptr);

    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  }

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Read out the attributes into the out-param */
    mmgsdi_status = mmgsdi_cache_attr_read(session_id,
                                           slot,
                                           access_ptr,
                                           cache_attr_pptr);
  }

  return mmgsdi_status;
} /* mmgsdi_cache_populate_attr_cache_if_needed */


/*===========================================================================
FUNCTION MMGSDI_CACHE_ATTR_READ

DESCRIPTION
  Checks for file attributes cache, if found cache ptr is returned in the
  function argument. In the SUCCESS case, *cache_attr_pptr will hold the
  attributes and the calling function should take care of freeing this ptr
  along with its member ptrs.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS
    MMGSDI_ERROR

SIDE EFFECTS
  None

CRITICAL SECTIONS:
  The function uses mmgsdi_cache_crit_sect to protect accesses to
  the File attributes cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_attr_read(
  mmgsdi_session_id_type             session_id,
  mmgsdi_slot_id_enum_type           slot,
  const mmgsdi_access_type          *access_ptr,
  mmgsdi_attr_cache_element_type   **cache_attr_pptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_attr_cache_type            *cache_attr_tbl_ptr  = NULL;
  mmgsdi_attr_cache_element_type    *new_cache_attr_ptr  = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_2(access_ptr, cache_attr_pptr);

  if(access_ptr->access_method != MMGSDI_BY_PATH_ACCESS &&
     access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Enter critical Section */
  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_cache_get_attr_cache(session_id,
                                                access_ptr,
                                                slot,
                                                &new_cache_attr_ptr,
                                                &cache_attr_tbl_ptr);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* got the cache already */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        (*cache_attr_pptr),
        sizeof(mmgsdi_attr_cache_element_type));
      if(*cache_attr_pptr == NULL)
      {
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }

      if(*cache_attr_pptr != NULL)
      {
        (void) mmgsdi_util_copy_access_type(&(*cache_attr_pptr)->access,
                                            &new_cache_attr_ptr->access);
        (*cache_attr_pptr)->cyclic_increase_allowed =
                          new_cache_attr_ptr->cyclic_increase_allowed;

        mmgsdi_cache_alloc_and_populate_file_attr_cache(new_cache_attr_ptr->read,
                                                   &((*cache_attr_pptr)->read));
        mmgsdi_cache_alloc_and_populate_file_attr_cache(new_cache_attr_ptr->write,
                                                  &((*cache_attr_pptr)->write));
        mmgsdi_cache_alloc_and_populate_file_attr_cache(new_cache_attr_ptr->increase,
                                                &((*cache_attr_pptr)->increase));
        (*cache_attr_pptr)->file_type  = new_cache_attr_ptr->file_type;
        (*cache_attr_pptr)->file_size  = new_cache_attr_ptr->file_size;
        (*cache_attr_pptr)->num_of_rec = new_cache_attr_ptr->num_of_rec;
        (*cache_attr_pptr)->rec_len    = new_cache_attr_ptr->rec_len;
        (*cache_attr_pptr)->sfi        = new_cache_attr_ptr->sfi;
      }
    }
  } while (0);

  /* Leave critical Section */
  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cache_attr_read */


/*===========================================================================
FUNCTION MMGSDI_CACHE_ATTR_WRITE

DESCRIPTION
  Checks for file attributes cache for a file. If the attributes are not
  cached already, it writes the attribtues to the attribtues cache

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS
    MMGSDI_ERROR

SIDE EFFECTS
  None

CRITICAL SECTIONS:
  The function uses mmgsdi_cache_crit_sect to protect accesses to
  the File attributes cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_attr_write(
  mmgsdi_session_id_type                   session_id,
  mmgsdi_slot_id_enum_type                 slot,
  const mmgsdi_access_type                *access_ptr,
  mmgsdi_file_structure_enum_type          file_type,
  mmgsdi_file_size_type                    file_size,
  mmgsdi_len_type                          rec_len,
  mmgsdi_rec_num_type                      num_of_rec,
  uint8                                    sfi,
  const mmgsdi_file_security_access_type * security_access_ptr,
  const boolean                          * increase_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_attr_cache_type            *cache_attr_tbl_ptr  = NULL;
  mmgsdi_attr_cache_element_type    *new_cache_attr_ptr  = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_2(access_ptr,
                              security_access_ptr);

  if(access_ptr->access_method != MMGSDI_BY_PATH_ACCESS &&
     access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Enter critical Section */
  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  do
  {
    /* Retrieve the appropriate cache ptr */
    mmgsdi_status = mmgsdi_cache_get_attr_cache(session_id,
                                                access_ptr,
                                                slot,
                                                &new_cache_attr_ptr,
                                                &cache_attr_tbl_ptr);

    if(mmgsdi_status == MMGSDI_SUCCESS && new_cache_attr_ptr != NULL)
    {
      /* Looks like attributes are already cached for this file.
         lets reset the existing attributes */
      mmgsdi_cache_reset_file_attr_cache_item(&new_cache_attr_ptr);
    }
    else if(mmgsdi_status == MMGSDI_NOT_INIT)
    {
      /* This is the first time we are caching attributes for this file */
      mmgsdi_status = mmgsdi_cache_create_file_attr_cache_element(access_ptr,
                                                                  cache_attr_tbl_ptr,
                                                                  &new_cache_attr_ptr);
      if((mmgsdi_status != MMGSDI_SUCCESS) || (new_cache_attr_ptr == NULL))
      {
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }
    }
    else
    {
      break;
    }

    /* Start populating the cache with new attributes */
    mmgsdi_cache_alloc_and_populate_file_attr_cache(security_access_ptr->read,
                                                   &(new_cache_attr_ptr->read));
    mmgsdi_cache_alloc_and_populate_file_attr_cache(security_access_ptr->write,
                                                   &(new_cache_attr_ptr->write));
    mmgsdi_cache_alloc_and_populate_file_attr_cache(security_access_ptr->increase,
                                                   &(new_cache_attr_ptr->increase));
    if(increase_ptr)
    {
      new_cache_attr_ptr->cyclic_increase_allowed = *increase_ptr;
    }
    /* else increase is not supported, create_file_attr_cache_element
       should have already set the element to 0 during intialization;
       therefore, there is no need to update it here again to FALSE */

    new_cache_attr_ptr->file_type   = file_type;
    new_cache_attr_ptr->file_size   = file_size;
    new_cache_attr_ptr->num_of_rec  = num_of_rec;
    new_cache_attr_ptr->rec_len     = rec_len;
    new_cache_attr_ptr->sfi         = sfi;

  } while (0);

  /* Leave critical Section */
  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cache_attr_write */


/*===========================================================================
FUNCTION MMGSDI_CACHE_INIT_USIM_ECC

DESCRIPTION
  This function determines the number of ECC records and caches all the records
  upto a maximum of MMGSDI_MAX_ECC_RECORDS (255) records.
  An ECC event is also sent out.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_init_usim_ecc(
  mmgsdi_session_id_type       session_id,
  mmgsdi_slot_id_enum_type     slot_id
)
{
#ifdef FEATURE_MMGSDI_3GPP
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type      *get_file_attr_ptr    = NULL;
  mmgsdi_get_file_attr_cnf_type      *cnf_ptr              = NULL;
  mmgsdi_read_req_type               *read_req_ptr         = NULL;
  mmgsdi_read_cnf_type               *read_cnf_ptr         = NULL;
  mmgsdi_session_type_enum_type       session_type         = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_cache_element_type          *cache_table_ptr      = NULL;
  uint16                              file_cnt             = 0;
  uint8                               rec_len              = 0;
  mmgsdi_rec_num_type                 num_of_recs          = 0;
  byte                                i                    = 0;
  mmgsdi_evt_session_notify_type      notify_type;
  uint8                               prov_session_index   = MMGSDI_MAX_PROV_APPS;
  uint16                              prov_cache_size      = 0;

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  /* Based on the session type, determine the cache ptr for caching ECC */
  do
  {
    mmgsdi_status = mmgsdi_util_get_session_type(session_id,&session_type,NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    if(session_type != MMGSDI_GW_PROV_PRI_SESSION &&
       session_type != MMGSDI_GW_PROV_SEC_SESSION &&
       session_type != MMGSDI_GW_PROV_TER_SESSION)
    {
      UIM_MSG_ERR_1("Invalid session_type for cache access 0x%x", session_type);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    prov_session_index = mmgsdi_util_get_prov_index_from_session(session_type);

    if(prov_session_index >= MMGSDI_MAX_PROV_APPS)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    cache_table_ptr    = mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_ptr;
    prov_cache_size    = mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_size;

    if( cache_table_ptr == NULL || prov_cache_size == 0 )
    {
      UIM_MSG_ERR_1("Prov app cache is not allocated for this session 0x%x",session_type);
      break;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(get_file_attr_ptr,
      sizeof(mmgsdi_get_file_attr_req_type));
    if(get_file_attr_ptr == NULL)
    {
      break;
    }
    /* USIM's ecc is linear record */
    /* Determine Number of Records and Len in ECC */
    get_file_attr_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
    get_file_attr_ptr->request_header.session_id        = session_id;
    get_file_attr_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
    get_file_attr_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
    get_file_attr_ptr->request_header.request_len       = sizeof(mmgsdi_get_file_attr_req_type);
    get_file_attr_ptr->request_header.payload_len       = 0;
    get_file_attr_ptr->request_header.slot_id           = slot_id;
    get_file_attr_ptr->request_header.client_data       = 0;
    get_file_attr_ptr->request_header.response_cb       = NULL;
    get_file_attr_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
    get_file_attr_ptr->access.file.file_enum            = MMGSDI_USIM_ECC;
    get_file_attr_ptr->int_client_data                  = 0;
    get_file_attr_ptr->activate_aid                     = TRUE;
    /* Do not read the EF-ARR for internal calls */
    get_file_attr_ptr->skip_uicc_arr                    = TRUE;

    mmgsdi_status = mmgsdi_uim_uicc_select(get_file_attr_ptr,
                                           TRUE,
                                           &cnf_ptr,
                                           TRUE);
    MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_ptr);

    if(cnf_ptr == NULL)
    {
      UIM_MSG_HIGH_0("Mem_Malloc Cnf ptr failed");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(mmgsdi_status != MMGSDI_SUCCESS ||
       cnf_ptr->file_attrib.file_type != MMGSDI_LINEAR_FIXED_FILE ||
       cnf_ptr->file_attrib.file_info.linear_fixed_file.num_of_rec == 0)
    {
      UIM_MSG_ERR_0("Select EF ECC failed or no Record Found");

      /* free the content of the cnf before freeing the cnf_ptr itself */
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }
    num_of_recs = cnf_ptr->file_attrib.file_info.linear_fixed_file.num_of_rec;
    rec_len = (uint8)cnf_ptr->file_attrib.file_info.linear_fixed_file.rec_len;

    /* free the content of the cnf before freeing the cnf_ptr itself */
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);

    if ( num_of_recs > MMGSDI_MAX_ECC_RECORDS )
    {
      /* Set to Max Number of Records Supported */
      UIM_MSG_HIGH_1("ECC # or Record 0x%x exceed MMGSDI cache", num_of_recs);
      num_of_recs = MMGSDI_MAX_ECC_RECORDS;
    }

    MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

    /* file_cnt iterates through 1 and writes through 2
       1. mmgsdi_usim_cache_uim_enums whose size is MMGSDI_NUM_FILES_IN_USIM_CACHE
       2. cache_table_ptr which is mmgsdi_prov_apps_cache_ptr[i].cache_table_ptr whose allocation
          is done during session activation for prov_cache_size
       It is important to check if file_cnt is not crossing above 2 buffer's boundary */
    for(file_cnt = 0; file_cnt < MIN(prov_cache_size,MMGSDI_NUM_FILES_IN_USIM_CACHE); file_cnt++)
    {
      if (mmgsdi_usim_cache_uim_enums[file_cnt] == MMGSDI_USIM_ECC)
      {
        cache_table_ptr[file_cnt].init = MMGSDI_CACHE_NOT_INIT;
        cache_table_ptr[file_cnt].no_of_records = 0;
        cache_table_ptr[file_cnt].total_len = 0;

        /* Free the allocated memory if any first and then allocate buffer
           of size rec_len*no_of_recs. All the records of ecc are cached in
           single large buffer and are accessed using rec_len as offset*/
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_table_ptr[file_cnt].data_ptr);
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_table_ptr[file_cnt].data_ptr,
                                           (rec_len * num_of_recs));
        break;
      }
    }

    MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

    if(file_cnt == prov_cache_size || file_cnt == MMGSDI_NUM_FILES_IN_USIM_CACHE)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(cache_table_ptr[file_cnt].data_ptr == NULL)
    {
      break;
    }
    /* No variable len to include */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
                                       sizeof(mmgsdi_read_req_type));

    /* No need to align because var len is 0 */
    if (read_req_ptr == NULL)
    {
      UIM_MSG_ERR_0("Malloc failed for read_req_ptr");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    for(i=1; i<=num_of_recs; i++)
    {
      read_req_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
      read_req_ptr->request_header.session_id        = session_id;
      read_req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
      read_req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
      read_req_ptr->request_header.request_len       = sizeof(mmgsdi_read_req_type);
      read_req_ptr->request_header.payload_len       = read_req_ptr->request_header.request_len -
                                                         uint32toint32(sizeof(mmgsdi_read_req_type));
      read_req_ptr->request_header.slot_id           = slot_id;
      read_req_ptr->request_header.client_data       = 0;
      read_req_ptr->request_header.response_cb       = NULL;

      read_req_ptr->access.access_method   = MMGSDI_EF_ENUM_ACCESS;
      read_req_ptr->access.file.file_enum  = MMGSDI_USIM_ECC;
      read_req_ptr->file_type              = MMGSDI_LINEAR_FIXED_FILE;
      read_req_ptr->rec_num                = i;
      read_req_ptr->data_len               = 0;/*Entire Record*/

      UIM_MSG_HIGH_1("Reading MMGSDI_USIM_ECC EF-ECC rec 0x%x", i);

      mmgsdi_status = mmgsdi_uim_uicc_read_record(read_req_ptr, TRUE, &read_cnf_ptr);

      if(read_cnf_ptr == NULL)
      {
        UIM_MSG_HIGH_0("Cnf ptr malloc failed");

        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_ptr);
        if(cache_table_ptr[file_cnt].no_of_records == 0)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_table_ptr[file_cnt].data_ptr);
          cache_table_ptr[file_cnt].init = MMGSDI_CACHE_NOT_FOUND;
        }
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }

      if((mmgsdi_status != MMGSDI_SUCCESS) ||
         (read_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS))
      {
        UIM_MSG_ERR_1("Read EF ECC failed for record 0x%x",
                    read_req_ptr->rec_num);

        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_ptr);
        /* free the content of the cnf before freeing the cnf_ptr itself */
        mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);
        if(cache_table_ptr[file_cnt].no_of_records == 0)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_table_ptr[file_cnt].data_ptr);
          cache_table_ptr[file_cnt].init = MMGSDI_CACHE_NOT_FOUND;
        }
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }

      if (cache_table_ptr[file_cnt].data_ptr == NULL)
      {
        return MMGSDI_ERROR;
      }

      /* Cache the contents of ECC */
      if((read_cnf_ptr->read_data.data_ptr != NULL) &&
         (read_cnf_ptr->read_data.data_len == rec_len) &&
         ((cache_table_ptr[file_cnt].total_len + int32touint32(read_cnf_ptr->read_data.data_len)) <= (rec_len * num_of_recs)))
      {
        mmgsdi_memscpy((cache_table_ptr[file_cnt].data_ptr) + cache_table_ptr[file_cnt].total_len,
                       (rec_len * num_of_recs) - cache_table_ptr[file_cnt].total_len,
                       read_cnf_ptr->read_data.data_ptr,
                       int32touint32(read_cnf_ptr->read_data.data_len));

        cache_table_ptr[file_cnt].no_of_records++;
        cache_table_ptr[file_cnt].total_len =
                         cache_table_ptr[file_cnt].total_len + read_cnf_ptr->read_data.data_len;
        cache_table_ptr[file_cnt].in_synch = TRUE;
      }

      /* Cache the first record read so that it can be used later for
         getting ECC record length when sending out the ECC event */
      if(i == 1)
      {
        mmgsdi_access_type          access;

        memset(&access, 0x00, sizeof(mmgsdi_access_type));

        access.file.file_enum = MMGSDI_USIM_ECC;
        access.access_method = MMGSDI_EF_ENUM_ACCESS;
        if(mmgsdi_cache_add_record(session_id,
                                   access,
                                   i,
                                   &read_cnf_ptr->read_data) != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("Unable to cache USIM ECC record no: 0x%x", i);
        }
      }

      /* free the content of the cnf before freeing the cnf_ptr itself */
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);
    }

    /* Set the cache to Init if at least 1 record is read */
    if (mmgsdi_status == MMGSDI_SUCCESS ||
        cache_table_ptr[file_cnt].no_of_records > 0)
    {
      cache_table_ptr[file_cnt].init = MMGSDI_CACHE_INIT;
    }
  } while(0);

  /* Send out the ECC event(even if the ECC read failed) */
  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = slot_id;
  notify_type.session_id  = session_id;
  mmgsdi_evt_build_and_send_ecc(notify_type);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_ptr);
  return MMGSDI_SUCCESS;
#else
  (void)session_id;
  (void)slot_id;
  return MMGSDI_ERROR;
#endif /* FEATURE_MMGSDI_3GPP */
}/*mmgsdi_cache_init_usim_ecc*/


/*===========================================================================
FUNCTION MMGSDI_CACHE_READ_USIM_ECC

DESCRIPTION
 This function copies ECC data from cache if available

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_read_usim_ecc(
  const mmgsdi_read_req_type   *req_ptr,
  mmgsdi_data_type             *temp_data_holder_ptr
)
{
  mmgsdi_session_type_enum_type session_type    = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_cache_element_type    *cache_table_ptr = NULL;
  uint16                        file_cnt           = 0;
  uint8                        *temp_cache_ptr  = NULL;
  mmgsdi_len_type               temp_record_len = 0;
  mmgsdi_return_enum_type       mmgsdi_status   = MMGSDI_ERROR;
  uint32                        temp_data_len   = 0;
  uint8                         prov_session_index = MMGSDI_MAX_PROV_APPS;
  uint16                        prov_cache_size    = 0;

  /* handle the case of USIM ECC cache reading */
  mmgsdi_status = mmgsdi_util_get_session_type(req_ptr->request_header.session_id,
                   &session_type,NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  prov_session_index = mmgsdi_util_get_prov_index_from_session(session_type);
  if (prov_session_index >= MMGSDI_MAX_PROV_APPS)
  {
    /* Session Type associated with given session id is not provisioning */
    return MMGSDI_ERROR;
  }

  cache_table_ptr   = mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_ptr;
  prov_cache_size   = mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_size;

  if (cache_table_ptr == NULL || prov_cache_size == 0)
  {
    UIM_MSG_LOW_2("Cache table not yet initialized for session type 0x%x cache_size 0x%x",
                  session_type,prov_cache_size);
    return MMGSDI_ERROR;
  }

  for(file_cnt = 0; file_cnt < MIN(prov_cache_size,MMGSDI_NUM_FILES_IN_USIM_CACHE) ; file_cnt++)
  {
    if (mmgsdi_usim_cache_uim_enums[file_cnt] == MMGSDI_USIM_ECC)
    {
      break;
    }
  }

  if(file_cnt == prov_cache_size || file_cnt == MMGSDI_NUM_FILES_IN_USIM_CACHE)
  {
    UIM_MSG_ERR_2("Could not find USIM ECC in USIM cache tables session type 0x%x cache_size 0x%x",
                  session_type,prov_cache_size);
    return MMGSDI_ERROR;
  }

   /* check if the requsted rec is cached already.
     If not chached, send the request to card */
  if(req_ptr->rec_num <= cache_table_ptr[file_cnt].no_of_records)
  {
    /* calculate the actual record len */
    temp_record_len =
    cache_table_ptr[file_cnt].total_len/cache_table_ptr[file_cnt].no_of_records;
    /* set the length of the data to be retrieved */
    if(req_ptr->data_len == 0)
    {
      /* if len = 0, set to the whole record length */
      temp_data_holder_ptr->data_len = temp_record_len;
    }
    else
    {
      temp_data_holder_ptr->data_len = req_ptr->data_len;
    }
    temp_data_len = int32touint32(temp_data_holder_ptr->data_len);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(temp_data_holder_ptr->data_ptr,
                                       temp_data_len);
    if (temp_data_holder_ptr->data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    /* offset the cache buffer to the corresponding record */
    temp_cache_ptr = cache_table_ptr[file_cnt].data_ptr + (temp_record_len * (req_ptr->rec_num-1));

    /* copy the data from the cache */
    mmgsdi_memscpy((void *)temp_data_holder_ptr->data_ptr,temp_data_len, temp_cache_ptr,temp_data_len);

    /* set the status to SUCCESS as cache read is good */
    return MMGSDI_SUCCESS;
  }
  else
  {
    return MMGSDI_ERROR;
  }
}/* mmgsdi_cache_usim_ecc */


/*===========================================================================
FUNCTION MMGSDI_CACHE_GET_ECC_FILE_DATA

DESCRIPTION
 This function copies whole of the ECC data from cache if available.
 The function allocates memory for the ecc data. It is caller's
 responsibility to free the data ptr.
 For RUIM/CSIM/GSM ECC files, the record length returned in ecc_rec_len_ptr
 is zero.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_get_ecc_file_data(
  mmgsdi_session_id_type         session_id,
  mmgsdi_file_enum_type          file_enum,
  mmgsdi_data_type               *ecc_data_ptr,
  mmgsdi_len_type                *ecc_rec_len_ptr
)
{
  mmgsdi_return_enum_type       mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_access_type            access;

  if(ecc_data_ptr == NULL ||
     ecc_rec_len_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  *ecc_rec_len_ptr = 0;

  memset(&access, 0x00, sizeof(mmgsdi_access_type));

  access.file.file_enum = file_enum;
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;

  /* Get length of the first ECC record that was populated during
     mmgsdi_cache_init_usim_ecc */
  mmgsdi_status = mmgsdi_cache_get_record_len(session_id,
                                              access,
                                              1,
                                              ecc_rec_len_ptr);

  if(file_enum == MMGSDI_USIM_ECC &&
   mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Get length of the whole ecc file data */
  mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                        &access,
                                        &ecc_data_ptr->data_len,
                                        0,
                                        NULL);

  /* Allocate the data ptr and read data from ecc cache */
  if(mmgsdi_status == MMGSDI_SUCCESS &&
     ecc_data_ptr->data_len != 0)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(ecc_data_ptr->data_ptr,
                                       ecc_data_ptr->data_len);
    if (ecc_data_ptr->data_ptr != NULL)
    {
      mmgsdi_status = mmgsdi_cache_read(session_id,
                                        &access,
                                        ecc_data_ptr,
                                        0,
                                        NULL);
    }
    else
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  return mmgsdi_status;
} /* mmgsdi_cache_get_ecc_file_data */


/*===========================================================================
FUNCTION MMGSDI_CACHE_DELETE_PKCS15_ATTR_CACHE

DESCRIPTION
 This function deletes all file attributes cached under the PKCS15 DF 7F50.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_cache_delete_pkcs15_attr_cache (
  mmgsdi_slot_id_enum_type  slot_id
)
{
  mmgsdi_attr_cache_type         * attr_cache_tbl_ptr     = NULL;
  mmgsdi_attr_cache_element_type * attr_cache_element_ptr = NULL;
  mmgsdi_attr_cache_element_type * next_attr_element_ptr  = NULL;
  mmgsdi_attr_cache_element_type * prev_attr_element_ptr  = NULL;

  UIM_MSG_LOW_0("mmgsdi_cache_delete_pkcs15_cache");

  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      attr_cache_tbl_ptr = mmgsdi_slot1_common_cache_tbl_ptr;
      break;
    case MMGSDI_SLOT_2:
      attr_cache_tbl_ptr = mmgsdi_slot2_common_cache_tbl_ptr;
      break;
    case MMGSDI_SLOT_3:
      attr_cache_tbl_ptr = mmgsdi_slot3_common_cache_tbl_ptr;
      break;
    default:
      UIM_MSG_ERR_1("Invalid MMGSDI slot 0x%x", slot_id);
      return;
  }

  if (attr_cache_tbl_ptr == NULL)
  {
    return;
  }

  attr_cache_element_ptr = attr_cache_tbl_ptr->item_ptr;

  /* Delete all the File attribute cache elements */
  while(attr_cache_element_ptr != NULL)
  {
    next_attr_element_ptr = attr_cache_element_ptr->next_ptr;

    if(attr_cache_element_ptr->access.access_method == MMGSDI_BY_PATH_ACCESS &&
       attr_cache_element_ptr->access.file.path_type.path_len > 1 &&
       attr_cache_element_ptr->access.file.path_type.path_buf[1] == 0x7F50)
    {
      /* relink the list of cache elements when deleting a PKCS15 file from cache */
      if(prev_attr_element_ptr != NULL)
      {
        prev_attr_element_ptr->next_ptr = next_attr_element_ptr;
      }
      else
      {
        attr_cache_tbl_ptr->item_ptr = next_attr_element_ptr;
      }

      mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_element_ptr);
    }
    else
    {
      prev_attr_element_ptr = attr_cache_element_ptr;
    }

    attr_cache_element_ptr = next_attr_element_ptr;
  }
} /* mmgsdi_cache_delete_pkcs15_attr_cache */


/*===========================================================================
FUNCTION MMGSDI_CACHE_CREATE_USIM_ECC_CACHE_PSM_MODE

DESCRIPTION
  To create USIM ECC cache in PSM mode

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cache_create_usim_ecc_cache_psm_mode
(
  mmgsdi_session_type_enum_type    session_type,
  uint8                            num_of_rec,
  const mmgsdi_data_type          *file_data_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_SUCCESS;
  int32                      cache_size      = 0;
  int32                      file_index      = 0;
  mmgsdi_cache_element_type *cache_table_ptr = NULL;

  if(file_data_ptr == NULL || file_data_ptr ->data_ptr == NULL ||
     file_data_ptr->data_len == 0 || num_of_rec == 0)
  {
    return MMGSDI_ERROR;
  }

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_cache_get_cache_ptrs_and_index(MMGSDI_USIM_ECC,
                                                          session_type,
                                                          MMGSDI_APP_USIM,
                                                          &cache_table_ptr,
                                                          &cache_size,
                                                          &file_index);
    if ((mmgsdi_status != MMGSDI_SUCCESS) || (cache_table_ptr == NULL) ||
        (file_index >= cache_size))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    cache_table_ptr[file_index].init = MMGSDI_CACHE_INIT;
    cache_table_ptr[file_index].no_of_records = num_of_rec;
    cache_table_ptr[file_index].total_len = file_data_ptr->data_len;

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cache_table_ptr[file_index].data_ptr);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_table_ptr[file_index].data_ptr,
                                       cache_table_ptr[file_index].total_len);

    if(cache_table_ptr[file_index].data_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }
    mmgsdi_memscpy(cache_table_ptr[file_index].data_ptr,
                   cache_table_ptr[file_index].total_len,
                   file_data_ptr->data_ptr,
                   file_data_ptr->data_len);
  }
  while(0);
  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;

  return mmgsdi_status;
}/* mmgsdi_cache_create_usim_ecc_cache_psm_mode */


/*==========================================================================
   FUNCTION      MMGSDI_CACHE_CLEAR_PROV_TYPE_CACHE_ALLOC

   DESCRIPTION:
     Clear transparent and record file cache and file attribute cache
     for provisioning applications

     The following functions internally use
     mmgsdi_cache_crit_sect to protect access to the File cache
     - mmgsdi_cache_delete_prov_file_attr_cache()
     - mmgsdi_cache_delete()

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_cache_crit_sect to protect accesses to
     the File cache
=========================================================================*/
void mmgsdi_cache_clear_prov_type_cache_alloc
(
  mmgsdi_session_id_type        session_id
)
{
  mmgsdi_return_enum_type        mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_session_type_enum_type  session_type        = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_aid_type                aid_type;
  mmgsdi_cache_element_type     *cache_table_ptr     = NULL;
  int32                          cache_size          = 0;
  mmgsdi_file_enum_type         *cache_item_ptr      = NULL;
  uint8                          prov_session_index  = MMGSDI_MAX_PROV_APPS;

  memset(&aid_type, 0x00, sizeof(mmgsdi_aid_type));

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               &aid_type);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Invalid session_type");
    return;
  }

  MMGSDICACHE_ENTER_FILE_CACHE_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_cache_get_prov_cache_ptrs(session_type,
                                                     aid_type.app_type,
                                                     &cache_table_ptr,
                                                     &cache_size,
                                                     &cache_item_ptr);

    if((mmgsdi_status != MMGSDI_SUCCESS) ||
       (cache_table_ptr == NULL)         ||
       (cache_item_ptr  == NULL))
    {
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    UIM_MSG_LOW_1("Clearing prov type cache allocs for SESSION_TYPE: 0x%x", session_type);

    /* Clear the Record Cache */
    if((aid_type.app_type == MMGSDI_APP_USIM) ||
       (aid_type.app_type == MMGSDI_APP_CSIM))
    {
      mmgsdi_cache_delete_record_selective_items(session_id, NULL, 0);
    }

    if(aid_type.app_type == MMGSDI_APP_USIM)
    {
      /* Clear the EONS_OPL_PNN Cache */
      (void)mmgsdi_eons_opl_pnn_delete_cache_all(session_id);
    }

    /* Clear the File attribute cache */
    (void)mmgsdi_cache_delete_prov_file_attr_cache(session_type, NULL, 0);

    if(cache_table_ptr != NULL)
    {
      /* Clear the file cache */
      (void)mmgsdi_cache_delete(aid_type.app_type, session_type);

      /* Free the allocated memory */
      prov_session_index = mmgsdi_util_get_prov_index_from_session(session_type);
      if (prov_session_index < MMGSDI_MAX_PROV_APPS)
      {
        MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_prov_apps_cache_ptr[prov_session_index].cache_table_ptr);
      }
    }
  }while(0);

  MMGSDICACHE_LEAVE_FILE_CACHE_CRIT_SECT;
} /* mmgsdi_cache_clear_prov_type_cache_alloc */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
