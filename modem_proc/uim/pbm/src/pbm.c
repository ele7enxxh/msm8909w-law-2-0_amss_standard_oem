/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER FUNCTIONS

GENERAL DESCRIPTION
  This file contains the PBM write and init functions.

  Copyright (c) 2002 - 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/16   nr      NAL China emergency requirement
06/27/16   nr      112 as emergency in all RATs
05/25/16   sp      F3 improvements
05/24/16   sp      Logging improvements
05/11/16   sp      Compilation Fix
05/11/16   sp      Logging improvements
08/26/15   nr      pbm is not reading both NV67221 and NV0057 when NV70242 to 0
03/03/15   stv     Migrate some efs access to uim_common_efs API
08/05/15   nr      Operator specifc NV ECC numbers configuration
07/20/15   nr      Enter & leave critical section inside pbm_ecc_set_hardcoded_eccs()
01/19/15   kr      Fixed Compilation warnings
10/08/14   stv     Reduce redundant indications to AP for OTA ECC update through CM SS event
09/15/14   stv     Ignore the failure of reading /nv/item_files/pbm/pbm_nv_ecc_config
09/12/14   stv     Feature to maintain sim absent hardcoded ecc list till pin is verified
08/05/14   NR      Removal of unused feature flags
07/29/14   am      Correctly direct/redirect atomic_ops APIs across MOBs
07/25/14   am      Added stubs for RFM APIs to fix linker errors
07/07/14   NR      Use v2.0 diag macros
06/16/14   NR      AAS sim field info is not getting copied during boot up
                   for all sessions except card session.
06/06/14   stv     Checking emergency mode and service category as well while building
                   hardcoded ecc cache
05/29/14   stv     NV wrapping for 3gpp spec rel.11 changes
05/16/14   NR      Enabling or Disabling ADN based on NV configurations
05/01/14   NR      FR 19033: Reduce Code Duplication pbm
02/25/14   stv     CM SS event handling changes for SGLTE
02/10/14   stv     Potential de-referencing of NULL pointer without checking for it
01/09/14   kk      Fix compilation error for SBM MOB
01/09/14   NR      potential memory leak issues in PBM funcions
01/07/13   NR      Pointer Null check is missing for the pointer ecc_cache returned
                   from pbm_device_id_to_cache () in function pbm_ecc_clear_all_id ()
12/19/13   NR      PBM: LLVM compiler warnings
09/17/10   krishna Clear NW ECC no's when the corresponding emergency list len is 0
05/10/10   krishna provided the clients Session based field info
03/18/10   krishna Used pbmtask_i.h instead if pbmtask.h as part of CMI activity
03/18/10   krishna Resolved the compilation errors in WM7
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/04/09   krishna changes for CR 167785
12/29/08   pvphani Adding Null pointer check in pbm_list_recover function. Fix for CR: 164527
03/04/08   clm     Add logic to revert records containing USIM only fields upon
                   failure to add, delete, or update.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
01/16/07   cvs     Add event for phonebooks becoming ready
12/06/06   cvs     Fix double read of files after full card refresh
12/06/06   cvs     Remove banned APIs
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
07/12/06   cvs     Fix compile warnings
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
04/10/06   cvs     add debug feature to ignore WMS init
03/24/06   cvs     %x -> 0x%x in log messages
02/28/06   cvs     Add support for MBDN
02/23/06   cvs     rework memory allocation macros
01/19/06   cvs     Emergency number cleanup, fix bug in network emerg nums
01/09/06   cvs     Lint fixes
01/04/06   cvs     Fix compile error without GSDI
12/14/05   cvs     Better log messages
12/06/05   cvs     Wait to init sim phonebooks until after WMS
11/15/05   cvs     Lint fixes
11/10/05   cvs     Add network ECC numbers
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
08/04/05   cvs     Check for committed list growth
08/01/05   cvs     Code review comments
07/29/05   AT      Added additional range checking.
07/06/05   cvs     Add 999 to emergency list if CPHS is enabled.
06/14/05   cvs     Fix GCF failures related to emergency numbers
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/23/05   cvs     Fix off-by-one error in loading ECC list
04/29/05   cvs     Fix Error Handling for USIM failures
03/31/05   AT      Created feature FEATURE_CPHS_UK to hardcode the
                   emergency number 999 in all cases.  This feature
                   causes a GCF test case to fail, but meets the CPHS
                   requirement.  Note these requirements are in conflict.
                   Also added featurization for non_MMGSDI builds.
03/28/05   AT      Updated ECC requirements.
03/08/05   PA      Support for PBM notifications
01/20/05   PA      Added support for read lock, non-blocking write lock and
                   support for MSISDN.
01/24/05   PA      Changed NV_EMERGENCY_NUMBER_START to
                   PBM_NV_EMERGENCY_NUMBER_START
01/20/05   PA      Add only emergency numbers to ecc_cache.
01/12/05   PA      PBM USIM Support
12/09/04   PA      Set cache status to ready after reading NV items
12/03/04   PA      Fix category used in mirroring
12/02/04   PA      Add reference when new record in added to committed list.
11/19/04   PA      Support non-GSDI builds
11/18/04   PA      Support for write lock
09/29/04   PA      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm.h"
#include "uim_msg.h"
#include "pbm_common_types.h"
#include "pbmefs.h"
#include "pbm_conversion.h"
#include "pbmtask_i.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "pbmuim.h"
#include "pbmgsdi.h"
#include "pbm_nv_refresh.h"
#include "cm.h"
#ifdef FEATURE_DUAL_SIM
#include "cm_dualsim.h"
#endif
#include "atomic_ops.h"
#include "stringl/stringl.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
                              MACROS
===========================================================================*/

/*--------------------------------------------------------
  To store list of status of pbm features
----------------------------------------------------------*/
pbm_features_status_list_type pbm_features_status_list;

/*===========================================================================
FUNCTION PBM_NV_FEATURES_STATUS_LIST_UPDATE_VERSION

DESCRIPTION
  Function used to validate the PBM Features Status NVITEM's version.
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
static void pbm_nv_features_status_list_update_version(void);

/*===========================================================================
FUNCTION PBM_NV_INIT_FEATURES_STATUS_NV_LOOKUP

DESCRIPTION
  Function used to get the status of PBM features as defined by the NVITEM
  "PBM_FEATURES_STATUS_LIST" (71535).
  The global, "features_status_list", used to hold the NVITEM, is expandable,
  yet the size is fixed. See the struct definition for more detailed doc on
  that. The global struct has a "version" item that is updated everytime we
  add new items to this global struct (starting with version 0 in the first
  implementation)
  The pbm code has full control over version of NVITEM. Taking an example,
  if in future, in version 2 of this struct(that has 10 items), the delta is 3
  items (from version 0 that had 7 items), and this build is loaded onto the
  device that already had the items set using version 0 of this NVITEM, then,
  on boot-up after loading the build, pbm will validate the version it read
  from NV. If it is not 2, it will update it to 2 and write it back to NV. At
  this point, however, the value of features/items 8 through 10 will be set to
  their default values by pbm code.

DEPENDENCIES
  Minimum QXDM version required to be able to set this NVITEM (71535) is
  QXDM 3.14.757

RETURN VALUE
  None

SIDE EFFECTS
  "version" item in this NVITEM is writable through QXDM (no QXDM support to
  hide it or grey it out) or QCN. However if what is written explicitly doesn't
  match what's in the build, pbm will change it to what's in the build and
  it could be annoying for the QXDM user to see it change to something else
  (when the user reads the NVITEM next time).
===========================================================================*/
static void pbm_nv_init_features_status_nv_lookup(void);

/*===========================================================================
FUNCTION PBM_NV_WRITE_DEFAULT_VALUE_OF_DELTA_FEATURES

DESCRIPTION
  Function used to write the default values of PBM Features (controlled by
  PBM_Feature_Status_list NVITEM) that are delta between the two
  versions of Feature_Status_List to global features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_nv_write_default_value_of_delta_features(
  uint8 version_in_nv,
  uint8 version_in_build
);

/*===========================================================================
FUNCTION PBM_NV_WRITE_DEFAULT_VALUE_OF_ALL_FEATURES

DESCRIPTION
  Function used to write the default values of all the PBM Features
  (controlled by PBM_Feature_Status_list NVITEM), defined currently, to
  global pbm_features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_nv_write_default_value_of_all_features(void);

/*===========================================================================
FUNCTION PBM_NV_FEATURE_WRITE_DEFAULT_VALUE

DESCRIPTION
  Function used to write the default value of MMGSDI Feature (controlled by
  PBM_Feature_Status_list NVITEM) to global pbm_features_status_list.

DEPENDENCIES

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
static pbm_return_type pbm_nv_feature_write_default_value(
  pbm_nv_features_enum_type nv_feature);

/*===========================================================================
FUNCTION PBM_IS_OTA_UPDATE_REQUIRED

DESCRIPTION
 This function is to check if the OTA ECC cache need to be updated based on
 the cache content and latest CM SS ECC data

RETURN VALUE
 boolean
 TRUE - Requires an update and hence need to re-build the cache and send
        ecc ready event to client
 FALSE - Doesnt need an update - probably the CM SS data and cache data
         are identical

PARAMETERS

SIDE EFFECTS

===========================================================================*/
boolean  pbm_is_ota_ecc_update_required(PB_CACHE *ecc_cache,cm_mm_msim_ss_info_s_type *ii_info_ptr);

/*-------------------------------------------------------------------
  List of total number of PBM features present in NVITEM in each version
  of feature_status_list. Whenever a newer version of the NVITEM is exposed,
  size of the array grows by 1...So, if 8 features/items were added in VERSION0
  and 2 in VERSION1, the array should have 2 items listing the number of total
  features at each version.. {8,10} in this case...
---------------------------------------------------------------------*/
uint8  pbm_features_total_till_version[] =
  {PBM_FEATURES_STATUS_LIST_VERSION0_COUNT,
   PBM_FEATURES_STATUS_LIST_VERSION1_COUNT};

/*===========================================================================
                       Declare data for the ECC.
===========================================================================*/
uint32 ecc_has_entries = 0;
uint32 pbm_wms_init_finished = 0;

/* This variable tracks the 3gpp REL version in the build by reading LTE NV Item */
lte_3gpp_release_ver_e  pbm_3gpp_rel_ver  = LTE_3GPP_REL11 ;

/* This variable is read from NV pbm_hardcode_ecc_config (NV 69736 ) to decide the
     source of hardcoded ecc list ( i.e. either from code or from NV 69737)*/
boolean                 pbm_hardcode_ecc_config = FALSE ;

rex_crit_sect_type pbm_crit_sect;

/* PBM Write Lock */
pbm_lock_s_type g_lock = {NULL, PBM_LOCK_NOT_INIT, FALSE};

/* List of notification functions registered with PBM */
pbm_notify_node_s_type *pbm_notify_list = NULL;

/* This list contains the last known good record for each pending write. This is
 * used in bringing the cache to a sane state when a write operation fails. If a
 * record does not exist in this list, the existing cache entry is implied to be
 * the last committed record.
 * */
pbm_addr_node_s_type *pbm_committed_list = NULL;
/* CM stuff */

static cm_client_id_type pbm_cm_client_id = (cm_client_id_type) -1;


static pbm_return_type pbm_ecc_init(void);

static pbm_return_type pbm_cm_init(void);

static pbm_return_type pbm_recover_usim_record(const pbm_write_record_s_type
                                                *write);

/*===========================================================================*
 *              PHONE BOOK MANAGER INITIALIZATION FUNCTIONS                  *
 *                                                                           *
 * These functions will initialize the phone book managers phone books       *
 *===========================================================================*/


/*===========================================================================
FUNCTION PBM_BUILD_ECC_RECORD

DESCRIPTION
  This function takes a name, number, and category and builds the appropriate
  record for PBM_ECC.  It then adds this record to the cache.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_build_ecc_record(
  pbm_field_id_e_type    field_id,
  uint32                 data_length,
  const char            *ecc_string,
  uint32                 name_length,
  const uint16          *name_string,
  boolean                usim_ecc_serv_cat_valid,
  uint8                  usim_ecc_serv_cat,
  pbm_session_enum_type  session_type,
  pbm_ecc_mode_type      emergency_mode,
  uint8                  srv_status_flag
)
{
   pbm_phonebook_type     pb_id            = {PBM_GPB};
   uint32                 number_of_fields = 0;
   uint32                 field_length[5]  = {0};
   int                    index            = 0;
   pbm_addr_cache_s_type *record           = NULL;
   PB_CACHE              *ecc_cache        = NULL;
   uint16                 location;

   switch (field_id)
   {
      case PBM_FIELD_HARDCODED_ECC:
      case PBM_FIELD_NV_ECC:
         pb_id.device_type = PBM_ECC_OTHER;
         ecc_cache = pbm_pb_id_to_cache (pb_id);
         break;
      case PBM_FIELD_NETWORK_ECC:
        if(PBM_LPB == pbm_session_type_to_pb_category(session_type))
        {
         ecc_cache = pbm_file_id_to_cache(session_type, PBM_FILE_ECC);
        }
        else //the else case would be applicable for NV only builds
        {
         pb_id.device_type = PBM_ECC_OTHER;
         ecc_cache = pbm_pb_id_to_cache (pb_id);
        }
        break;
      case PBM_FIELD_SIM_ECC:
         ecc_cache = pbm_file_id_to_cache(session_type, PBM_FILE_ECC);
         break;
      default:
         UIM_MSG_HIGH_1("invalid ecc category 0x%x", field_id);
         return PBM_ERROR;
   }

   PBM_CHECK_PTR2_RET(ecc_cache, ecc_string, PBM_ERROR);

   if ( usim_ecc_serv_cat == 0xFF )
   {
     usim_ecc_serv_cat = PBM_EMERGENCY_SERVICE_CAT_DEFAULT ;
   }
   else if ( usim_ecc_serv_cat > PBM_EMERGENCY_SERVICE_CAT_VALID_MASK )
   {
     usim_ecc_serv_cat &= PBM_EMERGENCY_SERVICE_CAT_VALID_MASK;
   }

  if (data_length > 0)
  {
    uint32 ecc_first_half  = (((data_length>0?ecc_string[0]:-1) << 16 & 0x00FF0000) |
                              ((data_length>1?ecc_string[1]:-1) <<  8 & 0x0000FF00) |
                              ((data_length>2?ecc_string[2]:-1)       & 0x000000FF));
    uint32 ecc_second_half = (((data_length>3?ecc_string[3]:-1) << 16 & 0x00FF0000) |
                              ((data_length>4?ecc_string[4]:-1) <<  8 & 0x0000FF00) |
                              ((data_length>5?ecc_string[5]:-1)       & 0x000000FF));

    UIM_MSG_HIGH_7("Adding ECC num, type 0x%x len 0x%x emergency_mode 0x%x, cat 0x%x valid 0x%x, num 0x%x 0x%x", 
                   field_id,
                   data_length, 
                   emergency_mode,
                   usim_ecc_serv_cat,
                   usim_ecc_serv_cat_valid,
                   ecc_first_half,
                   ecc_second_half);

      field_length[number_of_fields++] =  data_length;
      if (name_length > 0)
      {
         field_length[number_of_fields++] =  name_length;
      }
      if (usim_ecc_serv_cat_valid)
      {
         field_length[number_of_fields++] = 1;
      }
      if(emergency_mode != NOT_EMERGENCY)
      {
         field_length[number_of_fields++] = 1;
      }
      if(srv_status_flag)
      {
        field_length[number_of_fields++] = 1;
      }

      // Next format the number for the ECC cache indicating that it is from
      // the SESSION. If we ever need to figure out which application it came
      // from, that would be a different the field ID for now, each SESSION
      // has a unique ID.
      record = pbm_cache_record_new();
      PBM_CHECK_PTR_RET(record, PBM_ERROR_MEM_FULL);
      if (!pbm_allocate_fields_internal(&record->fields, number_of_fields,
                                        field_length))
    {
      PBM_MEM_FREEIF(record);
      return PBM_ERROR;
    }
    record->unique_id = 0;

      //hardcoded no's should skip the first PBM_NV_EMERGENCY_NUMBERS locations
      //since they are used by NV
      if(field_id == PBM_FIELD_HARDCODED_ECC)
      {
        uint8 loc_index = 0;

        for (loc_index = PBM_NV_EMERGENCY_NUMBERS+1; loc_index <= ecc_cache->num_of_records; loc_index++)
        {
           if ((ecc_cache->pb_cache_array[loc_index] == NULL) || (ecc_cache->pb_cache_array[loc_index]->num_fields == 0))
              break;
        }

        if (loc_index > ecc_cache->num_of_records)  /* No empty slot was found*/
        {
          pbm_free_fields_internal(&record->fields, 1);
          PBM_MEM_FREEIF(record);
          return PBM_ERROR_MEM_FULL;
        }
        location = loc_index;
      }
      /* Allocate the location.  Return an error if it is full. */
      else if (0 == (location = pbm_get_first_available(ecc_cache, FALSE)))
      {
         pbm_free_fields_internal(&record->fields, 1);
         PBM_MEM_FREEIF(record);
         return PBM_ERROR_MEM_FULL;
      }

      switch (field_id)
      {
         case PBM_FIELD_HARDCODED_ECC:
         case PBM_FIELD_NV_ECC:
            pb_id.device_type = PBM_ECC_OTHER;
            record->record_id = pbm_session_location_to_record_id(pb_id, location);
            break;
         case PBM_FIELD_NETWORK_ECC:
         case PBM_FIELD_SIM_ECC:
            pb_id = pbm_device_type_to_phonebook_type(session_type, PBM_ECC);
            record->record_id = pbm_session_location_to_record_id(pb_id, location);
            break;
         default:
            UIM_MSG_HIGH_1("invalid ecc category 0x%x", field_id);
            return PBM_ERROR;
      }

      record->category = (int) PBM_CAT_ECC;
      record->num_fields = (uint16) number_of_fields;
      index = 0;
      pbm_fill_field(&record->fields[index++], field_id,
                     (pbm_field_type_e_type) PBM_FT_PHONE,
                     (byte *) ecc_string, (uint16) data_length);

      if (name_length > 0)
      {
         pbm_fill_field(&record->fields[index++], PBM_FIELD_NAME,
                        (pbm_field_type_e_type) PBM_FT_UCS2,
                        (byte *) name_string, (uint16) name_length);
      }

      if (usim_ecc_serv_cat_valid)
      {
         pbm_fill_field(&record->fields[index++],
                        PBM_FIELD_ECC_SERVICE_CATEGORY,
                        (pbm_field_type_e_type) PBM_FT_BYTE,
                        (byte *) &usim_ecc_serv_cat, 1);
      }
      if(emergency_mode != NOT_EMERGENCY)
      {
        pbm_fill_field(&record->fields[index++],
                       PBM_FIELD_EMERGENCY_MODE,
                       (pbm_field_type_e_type) PBM_FT_BYTE,
                       (byte *) &emergency_mode, 1);
      }
      if(srv_status_flag)
      {
        pbm_fill_field(&record->fields[index++],
                       PBM_FIELD_SRV_STATUS_FLAG,
                       (pbm_field_type_e_type) PBM_FT_BYTE,
                       (byte *) & srv_status_flag, 1);
      }

      // Finally, add the number to the cache.
      if (PBM_SUCCESS != pbm_cache_add(ecc_cache, record, TRUE))
      {
         pbm_free_fields_internal(&record->fields, 1);
         PBM_MEM_FREEIF(record);
         return PBM_ERROR;
      }
   }
   return PBM_SUCCESS;
} /* pbm_build_ecc_record */


/*===========================================================================
FUNCTION PBM_ECC_SET_HARDCODED_ECCS

DESCRIPTION
  This function is called at initialization and whenever a SIM is
  inserted or removed.  If uim_present indicates no SIM is present,
  then the longer hardcoded list of emergency numbers is used.  If
  the SIM is inserted, then the shorter list is used.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_ecc_set_hardcoded_eccs (
  boolean   uim_present
)
{
  pbm_phonebook_type                 pb_id = {PBM_GPB};
  typedef struct pbm_ecc_tbl
  {
    char                       *dial_string;
    uint32                      string_length;
    boolean                     already_in_pbm;
    uint8                       service_cat_len; // currently this is only one byte
    pbm_service_category_val    service_cat_val;
    pbm_ecc_mode_type           emergency_mode;
  } PBM_ECC_TBL;

  uint8                              i                        = 0;
  uint8                              j                        = 0;
  uint8                              list_size                = 0;
  char                             **num                      = NULL;
  pbm_hardcoded_ecc_s_type          *pbm_nv_hardcoded_ecc_ptr = NULL;
  pbm_return_type                    ret_val                  = PBM_SUCCESS;
  PBM_ECC_TBL                       *nv_hardcoded_list        = NULL;
  struct pbm_ecc_tbl                *current_list             = NULL;
  PB_CACHE                          *ecc_cache                = NULL;
  boolean                            add_entry_to_list = FALSE ;
  pbm_feature_status_enum_type       ecc_hardcode_sim_absent_till_pin_lock = PBM_FEATURE_DISABLED;
  uim_common_efs_status_enum_type    efs_status               = UIM_COMMON_EFS_ERROR;

  PBM_ECC_TBL hardcoded_with_uim[] = {
    { "911",  4, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW_1X}, /* 911 is ALWAYS an emergency number */
    { "112",  4, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW_1X},
#if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
    { "*911", 5, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_1X},    /* These are specific to CDMA */
    { "#911", 5, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_1X},
#endif
  };

  PBM_ECC_TBL hardcoded_with_no_uim[] = {
    { "911",  4, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW_1X},
    { "112",  4, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW_1X},
#if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
    { "*911", 5, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_1X},
    { "#911", 5, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_1X},
#endif
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
    { "000",  4, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW},
    { "08",   3, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW},
    { "110",  4, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW},
    { "999",  4, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW},
    { "118",  4, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW},
    { "119",  4, FALSE,PBM_EMERGENCY_SERVICE_CAT_LEN,PBM_EMERGENCY_SERVICE_CAT_DEFAULT,EMERGENCY_GW},
#endif
  };

  ecc_hardcode_sim_absent_till_pin_lock = pbm_nv_get_feature_status(PBM_FEATURE_ECC_HARDCODE_SIM_ABSENT_LIST_TILL_PIN_LOCK);
  UIM_MSG_HIGH_2("uim_present 0x%x pbm_hardcode_ecc_config 0x%x",
                  uim_present, pbm_hardcode_ecc_config);

  if ( !pbm_hardcode_ecc_config )
  {
    /* Select the table of ECCs which is used when no SIM is inserted. */
    if (uim_present)
    {
      current_list = hardcoded_with_uim;
      list_size = ARR_SIZE(hardcoded_with_uim);
    }
    else
    {
      current_list = hardcoded_with_no_uim;
      list_size = ARR_SIZE(hardcoded_with_no_uim);
    }
  }
  else
  {
    PBM_MEM_ALLOC(nv_hardcoded_list,PBM_HARDCODED_NV_EMERGENCY_NUMBERS*sizeof(PBM_ECC_TBL));
    if( !nv_hardcoded_list )
    {
      return ;
    }
    PBM_MEM_ALLOC(pbm_nv_hardcoded_ecc_ptr,PBM_HARDCODED_NV_EMERGENCY_NUMBERS*sizeof(pbm_hardcoded_ecc_s_type));
    if( !pbm_nv_hardcoded_ecc_ptr )
    {
      PBM_MEM_FREEIF(nv_hardcoded_list);
      return ;
    }
    memset((byte *)nv_hardcoded_list, 0,PBM_HARDCODED_NV_EMERGENCY_NUMBERS*sizeof(PBM_ECC_TBL));
    memset((byte *)pbm_nv_hardcoded_ecc_ptr, 0,PBM_HARDCODED_NV_EMERGENCY_NUMBERS*sizeof(pbm_hardcoded_ecc_s_type));

       efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_NV_HARDCODED_ECC_LIST,
                                        UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                        UIM_COMMON_EFS_DEVICE,
                                        (uint8 *)pbm_nv_hardcoded_ecc_ptr,
                                        (uint32)PBM_HARDCODED_NV_EMERGENCY_NUMBERS*sizeof(pbm_hardcoded_ecc_s_type));

       if (efs_status != UIM_COMMON_EFS_SUCCESS )
       {
         UIM_MSG_ERR_1("PBM Hardcoded ECC read from NV failed %d",efs_status);
         ret_val = PBM_ERROR;
       }


    PBM_MEM_ALLOC(num,PBM_HARDCODED_NV_EMERGENCY_NUMBERS*sizeof(char *));
    if( !num )
    {
      PBM_MEM_FREEIF(nv_hardcoded_list);
      PBM_MEM_FREEIF(pbm_nv_hardcoded_ecc_ptr);
      return ;
    }

    memset((byte *)num, 0,PBM_HARDCODED_NV_EMERGENCY_NUMBERS*sizeof(char *));
    for ( i=0; i < PBM_HARDCODED_NV_EMERGENCY_NUMBERS&&ret_val==PBM_SUCCESS; i++ )
    {
      add_entry_to_list   =  FALSE ;
      if( pbm_nv_hardcoded_ecc_ptr[i].num_digits == 0 || 
          pbm_nv_hardcoded_ecc_ptr[i].num_digits > PBM_ECC_MAX_LEN)
      {
        continue ;
      }

      if( pbm_nv_hardcoded_ecc_ptr[i].category_length > PBM_EMERGENCY_SERVICE_CAT_LEN )
      {
        pbm_nv_hardcoded_ecc_ptr[i].category_length = PBM_EMERGENCY_SERVICE_CAT_LEN ;
      }

      if ( pbm_nv_hardcoded_ecc_ptr[i].category_length )
      {
        pbm_nv_hardcoded_ecc_ptr[i].emergency_category  &= PBM_EMERGENCY_SERVICE_CAT_VALID_MASK ;
      }

      switch ( pbm_nv_hardcoded_ecc_ptr[i].hardcoded_type )
      {
        case PBM_HARDCODE_ALWAYS :
          add_entry_to_list = TRUE ;
        break;

        case PBM_HARDCODE_CARD_ABSENT :
          add_entry_to_list = ( !uim_present  || ( uim_present && ecc_hardcode_sim_absent_till_pin_lock && !( pbm_pin1_verified.value & PBM_MASK_CARD_SESSION_PIN_VALUES) ) );
        break;

        case PBM_HARDCODE_CARD_PRESENT :
          add_entry_to_list = uim_present ;
        break;

        default :
          add_entry_to_list = FALSE ;
        break;
      }
      UIM_MSG_HIGH_2("add_entry_to_list 0x%x and hardcode type 0x%x",
                      add_entry_to_list, pbm_nv_hardcoded_ecc_ptr[i].hardcoded_type);

      if ( add_entry_to_list )
      {
        PBM_MEM_ALLOC(num[i], pbm_nv_hardcoded_ecc_ptr[i].num_digits + 1);
        if( !num[i] )
        {
          PBM_MEM_FREEIF(pbm_nv_hardcoded_ecc_ptr);
          PBM_MEM_FREEIF(nv_hardcoded_list);
          PBM_MEM_FREEIF(num);
          return ;
        }
        else
        {
          UIM_MSG_HIGH_2("allocated %d for num[%d]",pbm_nv_hardcoded_ecc_ptr[i].num_digits + 1 , i);
          memset((byte *)num[i], 0,pbm_nv_hardcoded_ecc_ptr[i].num_digits + 1);
        }

        for (j = 0; j < pbm_nv_hardcoded_ecc_ptr[i].num_digits; j++)
        {
          num[i][j] = pbm_nv_hardcoded_ecc_ptr[i].digits[j];
          UIM_MSG_HIGH_3("num[%d][%d] is %d ",i,j , pbm_nv_hardcoded_ecc_ptr[i].digits[j]);
        }
        num[i][j] = '\0';

        nv_hardcoded_list[i].dial_string = num[i] ;
        nv_hardcoded_list[i].string_length = pbm_nv_hardcoded_ecc_ptr[i].num_digits + 1;
        nv_hardcoded_list[i].already_in_pbm = FALSE ;
        nv_hardcoded_list[i].service_cat_len = pbm_nv_hardcoded_ecc_ptr[i].category_length;
        if( pbm_nv_hardcoded_ecc_ptr[i].category_length )
        {
          nv_hardcoded_list[i].service_cat_val = pbm_nv_hardcoded_ecc_ptr[i].emergency_category ;
        }
        nv_hardcoded_list[i].emergency_mode = (pbm_ecc_mode_type)(pbm_nv_hardcoded_ecc_ptr[i].emergency_mode) ;
        UIM_MSG_HIGH_3(" PBM hardcode 0x%x string_length 0x%x emergency_mode 0x%x",
                         pbm_nv_hardcoded_ecc_ptr[i].hardcoded_type,
                         nv_hardcoded_list[i].string_length,
                         nv_hardcoded_list[i].emergency_mode);
      }
    }

    PBM_MEM_FREEIF(pbm_nv_hardcoded_ecc_ptr);
    current_list = nv_hardcoded_list;
    list_size = PBM_HARDCODED_NV_EMERGENCY_NUMBERS ;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  /* Search through the ECC cache for hardcoded entries */
  pb_id.device_type = PBM_ECC_OTHER;
  ecc_cache = pbm_pb_id_to_cache(pb_id);
  if(ecc_cache == NULL)
  {
    rex_leave_crit_sect(&pbm_crit_sect);
    return;
  }

  for (i = 1; i <= ecc_cache->num_of_records; i++)
  {
    if (ecc_cache->pb_cache_array && ecc_cache->pb_cache_array[i])
    {
      if (match_found(PBM_CAT_NONE, PBM_FIELD_HARDCODED_ECC, NULL, 0, 0,
                       ecc_cache->pb_cache_array[i]))
      {
        /* We found an entry that is a hardcoded ECC
             If the entry is not in the current list, remove it. */
        for (j = 0; j < list_size; j++)
        {
          if ( (current_list[j].string_length > 0) &&
               (match_found(PBM_CAT_NONE, PBM_FIELD_HARDCODED_ECC,
                             current_list[j].dial_string,
                             (uint16) current_list[j].string_length,
                             PBM_SEARCHTYPE_EXACT,
                             ecc_cache->pb_cache_array[i])) &&
               (match_found(PBM_CAT_NONE, PBM_FIELD_EMERGENCY_MODE,
                             &current_list[j].emergency_mode,
                             (uint16) sizeof(current_list[j].emergency_mode),
                             PBM_SEARCHTYPE_EXACT,
                             ecc_cache->pb_cache_array[i])) &&
               (current_list[j].service_cat_len > 0) &&
               (match_found(PBM_CAT_NONE, PBM_FIELD_ECC_SERVICE_CATEGORY,
                             &current_list[j].service_cat_val,
                             (uint16) current_list[j].service_cat_len,
                             PBM_SEARCHTYPE_EXACT,
                             ecc_cache->pb_cache_array[i])))
          {
            current_list[j].already_in_pbm = TRUE;
            break;
          }
        }
        if (j == list_size)
        {
          /* The entry was not in the current list, so remove it. */
          ret_val = pbm_cache_delete(ecc_cache, ecc_cache->pb_cache_array[i]->record_id);
          if (PBM_SUCCESS != ret_val)
          {
            UIM_MSG_ERR_2("Error 0x%x while removing record 0x%x from ECC cache",
                           ret_val, ecc_cache->pb_cache_array[i]->record_id);
          }
        }
      }
    }
  }

  /* If any entry is not in the current list, add it. */
  for (i = 0; i < list_size; i++)
  {
    if (FALSE == current_list[i].already_in_pbm)
    {
      /* Use dummy session for HARDCODED ECC/ EFS records */
      ret_val = pbm_build_ecc_record(PBM_FIELD_HARDCODED_ECC,
                                      current_list[i].string_length,
                                      current_list[i].dial_string,
                                      0, NULL, ((current_list[i].service_cat_val) ? TRUE : FALSE),
                                      current_list[i].service_cat_val,
                                      PBM_SESSION_DEFAULT,
                                     current_list[i].emergency_mode, 0);
      if (PBM_SUCCESS != ret_val)
      {
        UIM_MSG_ERR_1("Error 0x%x in building hardcoded record.", ret_val);
      }
    }
  }

  rex_leave_crit_sect(&pbm_crit_sect);

  if ( pbm_hardcode_ecc_config )
  {
    PBM_MEM_FREEIF(nv_hardcoded_list);
    for ( i = 0;i < PBM_HARDCODED_NV_EMERGENCY_NUMBERS ; i ++ )
    {
      if ( num[i])
      {
        PBM_MEM_FREEIF(num[i]);
      }
    }
    PBM_MEM_FREEIF(num);
  }
} /* pbm_ecc_set_hardcoded_eccs */


/*===========================================================================
FUNCTION PBM_ECC_CLEAR_ALL_ID

DESCRIPTION
  This function is called at initialization and whenever a SIM is
  removed.  It removes all entries in the emergency_cache that
  have the ID of the given field.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_ecc_clear_all_id(
  pbm_session_enum_type      session_type,
  pbm_field_id_e_type        field_id
)
{
  PB_CACHE      *ecc_cache = NULL;
  int            i         = 0;
  boolean        uses_sim  = FALSE;

  UIM_MSG_HIGH_3("pbm_ecc_clear_all_id ecc_has_entries 0x%x field_id 0x%x session_type 0x%x",
                  ecc_has_entries, field_id, session_type);
  switch (field_id)
  {
    case PBM_FIELD_SIM_ECC:
      uses_sim = TRUE;
    break;

    case PBM_FIELD_NETWORK_ECC:
      uses_sim = FALSE;
    break;

    default:
    return;
  }

  /* If this ECC was not initialized, nothing to do. */
  if (!uses_sim ||
      (0 != (ecc_has_entries & PBM_SESSIONID_TO_SESSION_MASK(session_type))))
  {
    /* Search through the ECC cache for hardcoded entries */
    ecc_cache = pbm_device_id_to_cache(session_type, PBM_ECC);
    PBM_CHECK_PTR_RET(ecc_cache, VOID);

    for (i = 1; i <= ecc_cache->num_of_records; i++)
    {
      if (ecc_cache->pb_cache_array[i])
      {
        if (match_found(PBM_CAT_NONE, field_id, NULL, 0, 0,
                        ecc_cache->pb_cache_array[i]))
        {
          pbm_return_type ret_val    = PBM_SUCCESS;

          ret_val = pbm_cache_delete(ecc_cache,ecc_cache->pb_cache_array[i]->record_id);
          if (PBM_SUCCESS != ret_val)
          {
            UIM_MSG_ERR_3("Error %d in deleting ECC entry %d from ECC cache 0x%x",
                           ret_val, ecc_cache->pb_cache_array[i]->record_id, ecc_cache);
          }
        }
      }
    }
  }

  if (uses_sim)
  {
    /* Clear out the flag now. */
    PBM_SESSION_RESET(ecc_has_entries, session_type);
  }
  return;
}/* pbm_ecc_clear_all_id */


/*===========================================================================
FUNCTION PBM_INIT_ECC_NV

DESCRIPTION
  This function is called during boot.  It reads the ECC list as stored
  in NV.

  The idea is to always allow Emergency Calls through by keeping a list
  of valid Emergency Call Codes (ECC's) in NV in order for the ME to
  be able to call an emergency call even if no SIM is installed.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_init_ecc_nv (
void
)
{
  int                              i               = 0;
  int                              j               = 0;
  uint16                           size            = 0;
  uint16                           name[PBM_TEXT_SIZE_CHARS];
  pbm_phonebook_type               pb_id           = {PBM_GPB};
  PB_CACHE                        *cache           = NULL;
  pbm_return_type                  ret_val         = PBM_SUCCESS;
  uim_common_efs_status_enum_type  efs_status      = UIM_COMMON_EFS_ERROR;
  pbm_nv_ecc_type                 *nv_ecc_item     = NULL;

  /* Add all the hardcoded numbers. */
  pbm_ecc_set_hardcoded_eccs(FALSE);

  pb_id.device_type = PBM_ECC_OTHER;
  cache = pbm_pb_id_to_cache(pb_id);
  PBM_CHECK_PTR_RET(cache, PBM_ERROR);

  PBM_MEM_ALLOC(nv_ecc_item,
                PBM_NV_EMERGENCY_NUMBERS*sizeof(pbm_nv_ecc_type));
  PBM_CHECK_PTR_RET(nv_ecc_item, PBM_ERROR);
  memset ((byte *)nv_ecc_item,
           0,
           PBM_NV_EMERGENCY_NUMBERS*sizeof(pbm_nv_ecc_type));

  /* Read the NV ECC numbers from 67221 item  */
  efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_NV_ECC_LIST,
                                   UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                   UIM_COMMON_EFS_DEVICE,
                                   (uint8 *)nv_ecc_item,
                                  (uint32)PBM_NV_EMERGENCY_NUMBERS*sizeof(pbm_nv_ecc_type));

  if (efs_status != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_1("NV read failed.status=0x%x",efs_status);
    ret_val = PBM_ERROR;
  }

  /* Read in the numbers from NV. */
  for(i=0; i<PBM_NV_EMERGENCY_NUMBERS && ret_val==PBM_SUCCESS; i++)
  {
    if (nv_ecc_item[i].num_digits > PBM_ECC_MAX_LEN)
    {
      nv_ecc_item[i].num_digits = PBM_ECC_MAX_LEN;
    }

    if (nv_ecc_item[i].num_digits > 0 && nv_ecc_item[i].status == NV_EMERGENCY_SD_NUM)
    {
      /* Need to add a NULL to the end of dial_nv. */
      char *dial_string = NULL;

      PBM_MEM_ALLOC(dial_string, nv_ecc_item[i].num_digits + 1);
      if(dial_string == NULL)
      {
        PBM_MEM_FREEIF(nv_ecc_item);
        return PBM_ERROR_MEM_FULL;
      }

      for (j = 0; j < nv_ecc_item[i].num_digits; j++)
      {
        dial_string[j] = nv_ecc_item[i].digits[j];
      }

      /* Add NULL at the end of the number */
      dial_string[j] = '\0';
      size = (uint16) strlen((char *) nv_ecc_item[i].letters);

      if (size > PBM_TEXT_SIZE_CHARS)
      {
        size = PBM_TEXT_SIZE_CHARS;
      }
      if (!pbmconvert_pbm8_to_ucs2((uint8 *) nv_ecc_item[i].letters, size,
                                    name, ARR_SIZE(name)))
      {
        UIM_MSG_ERR_1("Could not convert all %d octets of the name",size);
      }

      if (PBM_SUCCESS != pbm_build_ecc_record(PBM_FIELD_NV_ECC,
                                              nv_ecc_item[i].num_digits + 1,
                                              dial_string,
                                              (pbmutils_wstrlen(name) + 1)* sizeof(uint16),
                                               name,
                                               FALSE, 0,
                                               PBM_SESSION_DEFAULT,
                                               nv_ecc_item[i].emergency_mode, 0))
      {
        UIM_MSG_ERR_0("Could not build ECC record");
      }

      PBM_MEM_FREEIF(dial_string);
    }
  }

  PBM_MEM_FREEIF(nv_ecc_item);
  PBM_SET_PB_CACHE_READY(cache);
  return ret_val;
}/* pbm_init_ecc_nv */


/*===========================================================================
FUNCTION PBM_INIT_CREATE_CONF_FILE

DESCRIPTION
  Function used to create a .conf file in EFS if not present. the .conf file
  is used to specify the PBM NV items that can be backed up in a QCN file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_init_create_conf_file(void)
{
  char           *buffer_ptr       = NULL;
  uint16          buffer_len       = 0;
  uint16          buffer_index     = 0;
  uint8           file_index       = 0;
  uint8           num_config_files = 0;
  uint32                           file_size        = 0;
  uim_common_efs_status_enum_type  efs_status       = UIM_COMMON_EFS_ERROR;
  char           *conf_files[] =
  {
    UIM_COMMON_EFS_PATH_PBM_NV_ECC_LIST,
    UIM_COMMON_EFS_PATH_PBM_NV_HARDCODED_ECC_CONFIG,
    UIM_COMMON_EFS_PATH_PBM_NV_HARDCODED_ECC_LIST,
    UIM_COMMON_EFS_PATH_PBM_SIM_ECC_AIRPLANE,
    UIM_COMMON_EFS_PATH_PBM_EFS_SUPPORT_DISABLE,
    UIM_COMMON_EFS_PATH_PBM_FEATURES_LIST,
    UIM_COMMON_EFS_PATH_PBM_NV_ECC_LIST_PER_SUB
  };

  do
  {
    efs_status = uim_common_efs_get_dir_stat(UIM_COMMON_EFS_CONF_DIR1,
                                             &file_size);
    if (efs_status == UIM_COMMON_EFS_ERROR)
    {
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR1,(uint16)0777);
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR2,(uint16)0777);
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR3,(uint16)0777);
       break;
    }

    efs_status = uim_common_efs_get_dir_stat(UIM_COMMON_EFS_CONF_DIR2,
                                              &file_size);

    if (efs_status == UIM_COMMON_EFS_ERROR)
    {
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR2,(uint16)0777);
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR3,(uint16)0777);
       break;
    }

    efs_status = uim_common_efs_get_dir_stat(UIM_COMMON_EFS_CONF_DIR3,
                                             &file_size);

    if (efs_status == UIM_COMMON_EFS_ERROR)
    {
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_CONF_DIR3,(uint16)0777);
    }
  }while(0);

  /* Calculate total length needed to write, note on total size needed:
     (num_config_files * (string len of each file + '\n')) */
  num_config_files = sizeof(conf_files)/sizeof(conf_files[0]);
  for (file_index = 0; file_index < num_config_files; file_index++)
  {
    buffer_len += strlen(conf_files[file_index]) + sizeof(char);
  }


  PBM_MEM_ALLOC(buffer_ptr,buffer_len);
  if (buffer_ptr == NULL)
  {
    return;
  }

  memset(buffer_ptr, 0, buffer_len);

  /* If created first time, write the items */
  for (file_index = 0; file_index < num_config_files; file_index++)
  {
    uint16 len_wrote = 0;
    uint16 len_to_write = strlen(conf_files[file_index]);

    /* Content of the file. Note: there should not be spaces before the names! */
    len_wrote  = memscpy((void*)&buffer_ptr[buffer_index],
                                    buffer_len - buffer_index,
                                   (void*)conf_files[file_index],
                                  len_to_write);
    buffer_index += len_wrote;

    /* Check ensures space for '\n' */
    if ( len_wrote != len_to_write || buffer_index >= buffer_len)
    {
      UIM_MSG_ERR_1( "Insufficient buffer space for conf_files[%d] !",file_index);
      PBM_MEM_FREEIF(buffer_ptr);
      return;
    }

    buffer_ptr[buffer_index++] = '\n';
  }
  pbm_compare_and_update_file(UIM_COMMON_EFS_PBM_CONF, buffer_ptr, (uint32)buffer_len);

  PBM_MEM_FREEIF(buffer_ptr);
} /* pbm_init_create_conf_file */


/*===========================================================================
FUNCTION PBM_NV_GET_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of PBM features listed in enum
  pbm_nv_features_enum_type as defined by the NVITEM 71535 -
  "PBM_FEATURES_STATUS_LIST".

DEPENDENCIES
  Minimum QXDM version required to be able to read/write this NVITEM is
  latest QXDM(or any version later than 3.14.757)

RETURN VALUE
  pbm_feature_status_enum

SIDE EFFECTS
  None
===========================================================================*/
pbm_feature_status_enum_type pbm_nv_get_feature_status(
  pbm_nv_features_enum_type  nv_feature)
{
  pbm_feature_status_enum_type ret_value = PBM_FEATURE_DISABLED;

  switch(nv_feature)
  {
    case PBM_FEATURE_FDN_SPECIAL_HANDLING:
      ret_value = pbm_features_status_list.fdn_special_handling;
      break;
    case PBM_FEATURE_ECC_HARDCODE_SIM_ABSENT_LIST_TILL_PIN_LOCK :
      ret_value = pbm_features_status_list.pbm_ecc_hardcode_sim_absent_list_till_pin_lock;
      break;
    default:
      return ret_value;
  }

  UIM_MSG_HIGH_2("Status of feature 0x%x is 0x%x", nv_feature, ret_value);
  return ret_value;
} /* pbm_nv_get_feature_status */

/*===========================================================================
FUNCTION PBM_NV_FEATURE_WRITE_DEFAULT_VALUE

DESCRIPTION
  Function used to write the default value of MMGSDI Feature (controlled by
  PBM_Feature_Status_list NVITEM) to global pbm_features_status_list.

DEPENDENCIES

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
static pbm_return_type pbm_nv_feature_write_default_value(
  pbm_nv_features_enum_type nv_feature)
{
  switch(nv_feature)
  {
    case PBM_FEATURE_FDN_SPECIAL_HANDLING:
      pbm_features_status_list.fdn_special_handling = PBM_FEATURE_DISABLED;
      break;
    case PBM_FEATURE_ADN_CACHING:
    {
      uint8 slot_id;
      for(slot_id = 0;slot_id < PBM_MAX_SLOTS;slot_id++)
      {
        pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].
                                   gpb_caching_status    = PBM_FEATURE_ENABLED;
        pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].
                                   lpb_1x_caching_status = PBM_FEATURE_ENABLED;
        pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].
                                   lpb_gw_caching_status = PBM_FEATURE_ENABLED;
      }
    }
      break;
    case PBM_FEATURE_ECC_HARDCODE_SIM_ABSENT_LIST_TILL_PIN_LOCK :
      pbm_features_status_list.pbm_ecc_hardcode_sim_absent_list_till_pin_lock = PBM_FEATURE_DISABLED;
    break;

    default:
      return PBM_ERROR;
  }
  return PBM_SUCCESS;
} /* pbm_nv_feature_write_default_value */

/*===========================================================================
FUNCTION PBM_NV_WRITE_DEFAULT_VALUE_OF_ALL_FEATURES

DESCRIPTION
  Function used to write the default values of all the PBM Features
  (controlled by PBM_Feature_Status_list NVITEM), defined currently, to
  global pbm_features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_nv_write_default_value_of_all_features(void)
{
  uint8 pbm_feature;
  uint8 num_features_in_build;

  num_features_in_build = pbm_features_total_till_version[PBM_FEATURES_STATUS_LIST_CURRENT_VERSION];

  memset(&pbm_features_status_list, 0x00, sizeof(pbm_features_status_list));

  /* Update the Version in features_status_list to the current build version */
  pbm_features_status_list.version = PBM_FEATURES_STATUS_LIST_CURRENT_VERSION;

  /* Update features_status_list with default values */
  for(pbm_feature = 0; pbm_feature < num_features_in_build; pbm_feature++)
  {
    (void)pbm_nv_feature_write_default_value((pbm_nv_features_enum_type )pbm_feature);
  }
} /* pbm_nv_write_default_value_of_all_features */

/*===========================================================================
FUNCTION PBM_NV_WRITE_DEFAULT_VALUE_OF_DELTA_FEATURES

DESCRIPTION
  Function used to write the default values of PBM Features (controlled by
  PBM_Feature_Status_list NVITEM) that are delta between the two
  versions of Feature_Status_List to global features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_nv_write_default_value_of_delta_features(
  uint8 version_in_nv,
  uint8 version_in_build
)
{
  uint8 pbm_feature;
  uint8 num_features_in_nv;
  uint8 num_features_in_build;

  if((version_in_nv >= sizeof(pbm_features_total_till_version)/sizeof(uint8)) ||
     (version_in_build >= sizeof(pbm_features_total_till_version)/sizeof(uint8)))
  {
    UIM_MSG_ERR_0("Array out-of-bounds while writing default values for PBM features");
    return;
  }

  num_features_in_nv    = pbm_features_total_till_version[version_in_nv];
  num_features_in_build = pbm_features_total_till_version[version_in_build];

  /* Since pbm_features_total_till_version array is indexed by the
     feature_status_list *version*, it's value at version version_in_nv is
     essentially the first feature of the delta list...no need to bail out in
     case writing fails... */
  for(pbm_feature = num_features_in_nv; pbm_feature < num_features_in_build; pbm_feature++)
  {
    (void)pbm_nv_feature_write_default_value((pbm_nv_features_enum_type )pbm_feature);
  }
} /* pbm_nv_write_default_value_of_all_features */

/*===========================================================================
FUNCTION PBM_NV_FEATURES_STATUS_LIST_UPDATE_VERSION

DESCRIPTION
  Function used to validate the PBM Features Status NVITEM's version.
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
static void pbm_nv_features_status_list_update_version(void)
{
  uim_common_efs_status_enum_type   efs_status  = UIM_COMMON_EFS_ERROR;

  UIM_MSG_HIGH_2("PBM Features Status NV ITEM version - in NV: 0x%x, in build: 0x%x",
                 pbm_features_status_list.version,
                 PBM_FEATURES_STATUS_LIST_CURRENT_VERSION);

  if (pbm_features_status_list.version < PBM_FEATURES_STATUS_LIST_CURRENT_VERSION)
  {
    /* If the features_status_list version in NV is less than current
       pbm_features_status_list version in our code, meaning new features have been
       added since the version in NV till the current version, we write back
       the default value of all those delta features to the NV. */
    pbm_nv_write_default_value_of_delta_features(
                                                 pbm_features_status_list.version,
                                                 PBM_FEATURES_STATUS_LIST_CURRENT_VERSION
                                                );

    pbm_features_status_list.version = PBM_FEATURES_STATUS_LIST_CURRENT_VERSION;

    efs_status = uim_common_efs_write(UIM_COMMON_EFS_PBM_FEATURES_LIST,
                                      UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                      UIM_COMMON_EFS_DEVICE,
                                     (const char *)&pbm_features_status_list,
                                     (uint32)sizeof(pbm_features_status_list));

    if (efs_status != UIM_COMMON_EFS_SUCCESS)
    {
      UIM_MSG_ERR_1("PBM Features Status NV ITEM version update failed!, error 0x%x",
                    efs_status);
    }
  }
} /* pbm_nv_features_status_list_update_version */


/*===========================================================================
FUNCTION PBM_NV_INIT_FEATURES_STATUS_NV_LOOKUP

DESCRIPTION
  Function used to get the status of PBM features as defined by the NVITEM
  "PBM_FEATURES_STATUS_LIST" (71535).
  The global, "features_status_list", used to hold the NVITEM, is expandable,
  yet the size is fixed. See the struct definition for more detailed doc on
  that. The global struct has a "version" item that is updated everytime we
  add new items to this global struct (starting with version 0 in the first
  implementation)
  The pbm code has full control over version of NVITEM. Taking an example,
  if in future, in version 2 of this struct(that has 10 items), the delta is 3
  items (from version 0 that had 7 items), and this build is loaded onto the
  device that already had the items set using version 0 of this NVITEM, then,
  on boot-up after loading the build, pbm will validate the version it read
  from NV. If it is not 2, it will update it to 2 and write it back to NV. At
  this point, however, the value of features/items 8 through 10 will be set to
  their default values by pbm code.

DEPENDENCIES
  Minimum QXDM version required to be able to set this NVITEM (71535) is
  QXDM 3.12.997.

RETURN VALUE
  None

SIDE EFFECTS
  "version" item in this NVITEM is writable through QXDM (no QXDM support to
  hide it or grey it out) or QCN. However if what is written explicitly doesn't
  match what's in the build, pbm will change it to what's in the build and
  it could be annoying for the QXDM user to see it change to something else
  (when the user reads the NVITEM next time).
===========================================================================*/
static void pbm_nv_init_features_status_nv_lookup(void)
{
  uim_common_efs_status_enum_type efs_status = UIM_COMMON_EFS_ERROR;

  /* Start off with the global struct initialised to 0 */
  memset(&pbm_features_status_list, 0x00, sizeof(pbm_features_status_list));

  efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_FEATURES_LIST,
                                   UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                   UIM_COMMON_EFS_DEVICE,
                                   (uint8 *)&pbm_features_status_list,
                                   (uint32)sizeof(pbm_features_status_list));

  UIM_MSG_HIGH_1("pbm_nv_init_features_status_nv_lookup Features NV read status 0x%x", efs_status);

  if (efs_status == UIM_COMMON_EFS_SUCCESS)
  {
    pbm_nv_features_status_list_update_version();
  }
  else
  {
    /* Updating global pbm_features_status_list with default value of all the
       defined features */
    pbm_nv_write_default_value_of_all_features();

    /* Write features' status to the EFS, erasing previous contents,
    create if not present */
    efs_status = uim_common_efs_write(UIM_COMMON_EFS_PBM_FEATURES_LIST,
                                      UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                      UIM_COMMON_EFS_DEVICE,
                                      (const char *)&pbm_features_status_list,
                                      (uint32)sizeof(pbm_features_status_list));

    if(efs_status != UIM_COMMON_EFS_SUCCESS)
    {
       UIM_MSG_LOW_1("Write to pbm_features_list default failed 0x%x", efs_status);
    }
  }
} /* pbm_nv_init_features_status_nv_lookup */


/*===========================================================================
FUNCTION PBM_INIT

DESCRIPTION
  This function is called during PBM task startup after the task related
  initialization is done.

  - Initialize Caches
  - Register functions with MMGSDI
  - Initialize EFS based phonebooks
  - Initialize Emergency Call Codes
  - MCFG Registration 

  Note that UIM based phonebook is read after SIM initialization is complete.

DEPENDENCIES
  pbm_task_init has been called before.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_init(
  void
)
{
   pbm_return_type        ret_val      = PBM_ERROR;
   uint8                  i            = 0;

   /* Intilizing the global last updated TxID to zero */
   pbm_free_txid();

   pbm_init_info.pbm_efs_init_done = FALSE;
   for (i = 0; i < (int) MAX_UIM_APPS; i++)
   {
      pbm_init_info.pbm_session_init_done[i] = FALSE;
   }
   for (i = 1; i < (int) MAX_UIM_APPS; i++)
   {
      memscpy(g_sim_field_info[i],sizeof(g_sim_field_info[0][0])*4,
              g_sim_field_info[i-1],sizeof(g_sim_field_info[0][0])*4);
   }
   /* pbm usim path lookup table initialization */
   for (i = 1; i < (int) MAX_UIM_APPS; i++)
   {
      memscpy(pbm_usim_path_lookup_table[i],
              sizeof(pbm_usim_path_lookup_table[0][0])*PBM_NUM_FILES_IN_LOOKUP_TABLE,
              pbm_usim_path_lookup_table[i-1],
              sizeof(pbm_usim_path_lookup_table[0][0])*PBM_NUM_FILES_IN_LOOKUP_TABLE);
   }
   pbm_init_create_conf_file();

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   /* MCFG Registration */
   pbm_nv_mcfg_refresh_register();
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

   /* initializing the seek data */
   memset(pbm_seek_data, 0, sizeof(pbm_seek_data));

   /* initializing atomic variables */
   atomic_init(&pbm_pin1_verified, (atomic_plain_word_t) 0 );
   atomic_init(&pbm_pin2_verified, (atomic_plain_word_t) 0);

   /* Initialize caches to zero */
   ret_val = pbm_cache_init();
   PBM_CHECK_ERR_RET(ret_val, "Cache init failed");

   /* Initialize GSDI
   * UIM is initialized based on events we receive from GSDI */
   ret_val = pbm_mmgsdi_init();
   PBM_CHECK_ERR(ret_val, "GSDI init failed");

   /* Initialize EFS based phonebook */
   ret_val = pbm_efs_init();
   pbm_init_info.pbm_efs_init_done = TRUE;
   PBM_CHECK_ERR(ret_val, "EFS init failed");

   /* Initialize Emergency Call Codes */
   ret_val = pbm_ecc_init();
   PBM_CHECK_ERR(ret_val, "ECC init failed");

   /* Initialize CM client */
   ret_val = pbm_cm_init();
   PBM_CHECK_ERR(ret_val, "PBM CM init failed");

   /* Get the pbm features' status via NV look up */
   pbm_nv_init_features_status_nv_lookup();

   return PBM_SUCCESS;
} /* pbm_init */


/*===========================================================================
FUNCTION pbm_compare_and_update_file

DESCRIPTION
  This function opens the file, compares its contents against the passed
  buffer and if the two are different, empties the file and writes back
  the buffer data to the file. It also writes the buffer data to the file
  if file couldn't be read.

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
void pbm_compare_and_update_file
(
  uim_common_efs_item_type   efs_item,
  const char *               buf_ptr,
  uint32                     buf_len
)
{
  boolean                          write_conf      = TRUE;
  uint8                           *file_data_ptr   = NULL;
  uint32                           file_size       = 0;
  uim_common_efs_status_enum_type  efs_status      = UIM_COMMON_EFS_ERROR;
  uim_common_efs_file_type         efs_file_type   = UIM_COMMON_EFS_ITEM_FILE_TYPE;


  if(buf_ptr == NULL  || buf_len == 0)
  {
    return;
  }

  efs_status = uim_common_efs_get_file_size(efs_item,
                                            UIM_COMMON_EFS_DEVICE,
                                            &file_size);

  if( efs_status == UIM_COMMON_EFS_SUCCESS &&
      file_size == buf_len)
  {

    /* If the file size is non-zero, read data from file to compare against the
       buffer and write back to the file only if the data is different. If file
       read fails, still write the buffer to the file  */
    PBM_MEM_ALLOC(file_data_ptr,file_size);

    PBM_CHECK_PTR_RET(file_data_ptr,VOID);

    /* Check to just have this exapandable for ITEM EFS files as well */
    if( efs_item == UIM_COMMON_EFS_PBM_CONF )
    {
      efs_file_type = UIM_COMMON_EFS_REGULAR_FILE_TYPE;
    }

    efs_status = uim_common_efs_read(efs_item,
                                     efs_file_type,
                                     UIM_COMMON_EFS_DEVICE,
                                     file_data_ptr,
                                     file_size);

    if(efs_status == UIM_COMMON_EFS_SUCCESS)
      {
      /*Read is success - Compare the data */
        if(memcmp(file_data_ptr, buf_ptr, buf_len) == 0)
        {
          write_conf = FALSE;
        }
      }
      PBM_MEM_FREEIF(file_data_ptr);
    }
  else
  {
    UIM_MSG_MED_1("Fail in retrving file size or mis-matched"
                  "file size for PBM CONF status 0x%x",efs_status);
  }

  if(write_conf)
  {
     /* We will be here if
        a. File doesnt exist
        b. Data in file is not matching with current content  */
    efs_status = uim_common_efs_write(efs_item,
                                      efs_file_type,
                                      UIM_COMMON_EFS_DEVICE,
                                     (const char *)buf_ptr,
                                     (uint32)buf_len);

    if( UIM_COMMON_EFS_SUCCESS != efs_status )
    {
      UIM_MSG_MED_1("Write to PBM CONF failed 0x%x",efs_status);
    }
  }
} /* pbm_compare_and_update_file */



/*===========================================================================
FUNCTION PBM_ECC_INIT

DESCRIPTION
  Initialize Emergency Call Codes.

  The idea is to always allow Emergency Calls through by keeping a list
  of valid Emergency Call Codes (ECC's) in NV in order for the ME to
  be able to call an emergency call even if no SIM is installed.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_ecc_init(
  void
)
{
  pbm_phonebook_type                 pb_id        = {PBM_GPB};
  pbm_return_type                    ret_val      = PBM_SUCCESS;
  uim_common_efs_status_enum_type    efs_status   = UIM_COMMON_EFS_ERROR;
  uint8                              slot_index   = 0;

  pb_id.device_type = PBM_ECC_OTHER;

  /* Initialize Emergency Call Codes from NV */
  ret_val = pbm_efs_cache_structure_init(&ecc_other_cache,
                                         pb_id,
                                         PBM_MAX_ECC_NUMBERS,
                                         FALSE);
  if (PBM_SUCCESS != ret_val)
  {
    UIM_MSG_ERR_1("Error %d initializing emergency numbers", ret_val);
  }

  efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_NV_HARDCODED_ECC_CONFIG,
                                   UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                   UIM_COMMON_EFS_DEVICE,
                                   &pbm_hardcode_ecc_config,
                                   (uint32)sizeof(boolean));

  if (efs_status != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_1("PBM NV read failed during reading Hardcoded Config file 0x%x",efs_status);
  }

  /* Intilize ECC list per slot */
  for(slot_index = PBM_SLOT_1_INDEX; slot_index <= PBM_SLOT_3_INDEX; slot_index++)
  {
    (void)pbm_init_nv_ecc_per_slot(slot_index);
  }

  ret_val = pbm_init_ecc_nv();
  if (PBM_SUCCESS != ret_val)
  {
    UIM_MSG_ERR_1("Error during pbm_init_ecc_nv 0x%x", ret_val);
  }

  efs_status = uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_LTE_3GPP_RELEASE_VER,
                                   UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                   UIM_COMMON_EFS_DEVICE,
                                   (uint8 *)&pbm_3gpp_rel_ver,
                                   (uint32)sizeof(lte_3gpp_release_ver_e));

  if (efs_status != UIM_COMMON_EFS_SUCCESS)
  {
    pbm_3gpp_rel_ver  = LTE_3GPP_REL11 ;
  }
  UIM_MSG_HIGH_2("pbm_rel_ver_read_from_nv status 0x%x value 0x%x", efs_status, pbm_3gpp_rel_ver);

  return PBM_SUCCESS;
}/* pbm_ecc_init */


/*===========================================================================
FUNCTION PBM_INT_RECORD_WRITE

DESCRIPTION
  This function performs the actual write of the record to UIM or EFS.
  It is always called from within PBM Task. The address record is already
  added to cache from within the library function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_int_record_write (pbm_write_record_s_type *write)
{
   pbm_phonebook_type pb_id = {PBM_GPB};
   pbm_device_type device_type;
   pbm_record_id_type rec_id;

   pbm_return_type ret_val = PBM_ERROR ;
   pbm_addr_cache_s_type *record;
   int fields_size;
   int index;
   pbm_device_type outgoing_call_mirror[] = { PBM_LND, PBM_OCI};
   pbm_device_type incoming_call_mirror[] = { PBM_ICI};
   pbm_device_type *mirror = NULL;
   int mirror_size = 0;
   PB_CACHE *cache;

   PBM_CHECK_PTR_RET(write, PBM_ERROR);
   rec_id = write->rec_id;
   device_type = (pbm_device_type) pbm_record_id_to_device_type(rec_id);
   pb_id = pbm_session_record_id_to_phonebook_type(rec_id);

   /* This can happen when GSDI sends a refresh */
   if (PBM_SUCCESS != (ret_val = check_pb_ready(pb_id)))
   {
      pbm_write_notify(write, ret_val);
      pbm_write_cmd_free(write);
      return ret_val;
   }

   /* Mirror callhistory entry to SIM if it is not a deletion.
   * Because sizes may be different between the SIM and
   * EFS versions, deletions in one file do not necessarily mean
   * a record will be deleted from the mirror file.  Thus, if the
   * SIM file must be deleted, that needs to be done seperately.
   */
   record = write->curr_rec;
   if ( record != NULL )
   {
      if (record->num_fields)
   {
      switch (device_type)
      {
         case PBM_DIALED:
            mirror = outgoing_call_mirror;
            mirror_size = ARR_SIZE(outgoing_call_mirror);
            break;
         case PBM_RCVD:
         case PBM_MISS:
            mirror = incoming_call_mirror;
            mirror_size = ARR_SIZE(incoming_call_mirror);
            break;
         default:
            mirror = NULL;
            mirror_size = 0;
            break;
      } /*  switch(pb_id)... */

      if (NULL != mirror)
      {
         for (index = 0; index < mirror_size; index++)
         {
            pbm_phonebook_type mirror_pb_id = pb_id;
            mirror_pb_id.device_type = mirror[index];
            cache = pbm_pb_id_to_cache(mirror_pb_id);
            if (!cache)
               continue;

            if (0 < cache->num_of_records)
            {
               rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;
               /* Write the fields to SIM cache too.
               * pbm_record_write() makes a copy */
               fields_size = pbm_calculate_size_from_fields(record->fields,
                                                            record->num_fields);

               ret_val = pbm_session_record_write (mirror_pb_id, &rec_id,
                                                   (uint16) PBM_CAT_NONE,
                                                   (uint32) record->num_fields,
                                                   (uint8*) record->fields,
                                                   fields_size, NULL, NULL);
               if (PBM_SUCCESS != ret_val)
               {
                  UIM_MSG_ERR_2("Mirroring Call hist to Session %d file failed with error %d",
                              mirror[index], ret_val);
               }
            }
         }
      }
   }

   /* Now do actual write */
   switch (pb_id.device_type)
   {
      case PBM_DIALED:
      case PBM_RCVD:
      case PBM_MISS:
      case PBM_EFS:
      case PBM_SDL:
         ret_val = pbm_record_write_to_efs(write);
         break;

      case PBM_ADN:
      case PBM_SDN:
      case PBM_FDN:
      case PBM_LND:
      case PBM_MSISDN:
      case PBM_MBN:
      case PBM_GRPNAME:
      case PBM_AAS:
      case PBM_OCI:
      case PBM_ICI:
      case PBM_MBDN:
         ret_val = pbm_record_write_to_sim(write);
         break;

      default:
         UIM_MSG_ERR_1("Unrecognized phonebook for write, %d.",
                     pb_id.device_type);
         ret_val = PBM_ERROR;
         break;
   } /* end switch */
   }
   if (ret_val != PBM_SUCCESS)
   {
      pbm_write_notify(write, ret_val);
      pbm_write_cmd_free(write);
   }
   return ret_val;
}


/*===========================================================================
FUNCTION PBM_WRITE_CMD_FREE

DESCRIPTION
  This function frees any references to cache records and frees
  the command structure itself. It also signals the PBM task to go
  ahead and process the next command.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_cmd_free(pbm_write_record_s_type *write)
{
   PBM_CHECK_PTR_RET(write, VOID);

   if (write->prev_rec)
      (void) pbm_cache_record_free(write->prev_rec);

   if (write->curr_rec)
      (void) pbm_cache_record_free(write->curr_rec);

   PBM_MEM_FREEIF(write->curr_usim_index);
   PBM_MEM_FREEIF(write->prev_usim_index);

   pbm_cmd_free(PBM_CMD_PTR(write));
   (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
}


/*===========================================================================
FUNCTION PBM_WRITE_NOTIFY

DESCRIPTION
  This function sends the asynchronous result of a PBM write operation.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_notify (const pbm_write_record_s_type *write,
                       pbm_return_type ret_val)
{
   pbm_notify_event_e_type event;
   pbm_writecb_data_s_type cb_data;
   pbm_session_event_data_u_type evt_data = {0};
   pbm_phonebook_type pb_id;
   PB_CACHE *cache = NULL;


   PBM_CHECK_PTR_RET(write, VOID);

   /* notify async callback passed in with write cmd */
   cb_data.ret = ret_val;
   //since this is the old rec_id we would need just 16 LSBs
   (void)pbm_new_recid_to_old_recid(&cb_data.rec_id,write->rec_id);
   //copy the new 32 bit rec_id
   cb_data.sess_rec_id = write->rec_id;
   cb_data.user_data = write->user_data;

   if (write->proc_func)
      (*write->proc_func)(&cb_data);

   /* Send notifications using registered functions */
   if (PBM_SUCCESS == ret_val)
   {
      /* Check # fields to find if there is a record.
      * If both current and previous records are present, this is an update.
      * If current record is present and previous record is not present,
      * this is an add operation. Otherwise this is a deletion.
      */
      if (write->curr_rec && write->curr_rec->num_fields)
      {
         if (write->prev_rec && write->prev_rec->num_fields)
            event = PBM_EVENT_REC_UPDATE;
         else
            event = PBM_EVENT_REC_ADD;
      } else
      {
         event = PBM_EVENT_REC_DELETE;
      }
   } else
   {
      event = PBM_EVENT_REC_FAILED;
   }
   evt_data.rec_id = write->rec_id;
   pb_id = pbm_session_record_id_to_phonebook_type(write->rec_id);
   cache = pbm_pb_id_to_cache(pb_id);
   PBM_CHECK_PTR_RET(cache,VOID);

   if ( !( (cache->status == PBM_STATUS_INIT_SYNC) && (pb_id.device_type == PBM_ADN)))
   {
   pbm_add_notify(event, evt_data);
}
   else
   {
      UIM_MSG_HIGH_1("Event not notified for rec_id 0x%x",write->rec_id);
   }
}


/*===========================================================================
FUNCTION PBM_LIST_BACKUP

DESCRIPTION
  This function makes sure that there is a valid entry in the committed list for
  this record id. If there are no pending writes on this record, the current
  cache entry is saved to committed list. If the previous entry is a NULL, we
  replace it with a dummy entry.

DEPENDENCIES
  Cache should be initialized before. Record id should be valid.
  Should be called from within pbm_crit_sect.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_list_backup(pbm_record_id_type rec_id)
{
   PB_CACHE *cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
   uint16 location = pbm_record_id_to_location(rec_id);
   pbm_addr_node_s_type *curr_node = NULL;
   pbm_addr_node_s_type *prev_node = NULL;
   pbm_addr_node_s_type *node = NULL;
   pbm_addr_cache_s_type *dummy = NULL;
   int committed_list_size = 0;

   PBM_CHECK_PTR_RET(cache, PBM_ERROR);

   if (PBM_SESSION_GET_FIRST_AVAILABLE == rec_id)
      return PBM_ERROR;

   /* Search for a node with given record id or go till the end */
   curr_node = pbm_committed_list;
   while (curr_node)
   {
      if (pbm_compare_rec_ids(curr_node->rec_id , rec_id))
         return PBM_SUCCESS; /* We are done */
      prev_node = curr_node;
      curr_node = curr_node->next;
   }

   /* Allocate a new node */
   PBM_MEM_ALLOC(node, sizeof(pbm_addr_node_s_type));
   PBM_CHECK_PTR_RET(node, PBM_ERROR_MEM_FULL);

  /* If entry is NULL, add a dummy entry in cache for future comparison */
  if (!cache->pb_cache_array[location])
  {
    dummy = pbm_cache_record_new();
    if (!dummy)
    {
      PBM_MEM_FREEIF(node);
      return PBM_ERROR_MEM_FULL;
    }
    pbm_cache_record_fill(dummy, rec_id, 0, 0, 0, NULL);
    cache->pb_cache_array[location] = dummy;
  }

   node->rec_id = rec_id;
   node->record = pbm_cache_record_get(rec_id);
   node->next = NULL;

   /* Add node to committed list (end of list) */
   if (prev_node)
      prev_node->next = node;
   else
      pbm_committed_list = node;

   /* check committed list size */
   node = pbm_committed_list;
   committed_list_size = 0;
   while (node != NULL)
   {
      committed_list_size++;
      node = node->next;
   }
   if (committed_list_size > 10)
   {
      UIM_MSG_ERR_1("Committed list unusual size %d", committed_list_size);
   }
   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_LIST_UPDATE

DESCRIPTION
  This function updates the PBM committed list after a successful write.

  If the cache entry is same as current write record, there are no other
  pending writes. So we delete the corresponding node from within PBM
  committed list. A dummy record wthin cache is also relaplced with the
  original NULL entry.

  If cache entry is different, we update the corresponding node within
  committed list with the latest record.

DEPENDENCIES
  No critical section dependencies.

SIDE EFFECTS
===========================================================================*/
void pbm_list_update(const pbm_write_record_s_type *write)
{
   pbm_addr_cache_s_type *cache_rec;
   PB_CACHE *cache;
   pbm_addr_node_s_type *curr_node = NULL;
   pbm_addr_node_s_type *prev_node = NULL;
   boolean found = FALSE;
   uint16 location;

   PBM_CHECK_PTR_RET(write, VOID);

   cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(write->rec_id));
   PBM_CHECK_PTR_RET(cache, VOID);

   location = pbm_record_id_to_location(write->rec_id);
   if (location > cache->num_of_records)
   {
      UIM_MSG_HIGH_3("Location for record ID out of bounds %d of %d",
                   write->rec_id, location, cache->num_of_records);
      return;
   }

   rex_enter_crit_sect(&pbm_crit_sect);
   cache_rec = cache->pb_cache_array[location];

   if (!cache_rec)
   {
      UIM_MSG_HIGH_1("NULL cache entry found for record id %d",
                   write->rec_id);
      rex_leave_crit_sect(&pbm_crit_sect);
      return;
   }

   /* Search for the record id within committed list */
   PBM_LIST_SEARCH(write->rec_id, prev_node, curr_node, found);
   if (!found || !curr_node)
   {
      UIM_MSG_HIGH_2("Can't find entry in committed list for rec 0x%x, node 0x%x",
                   write->rec_id, curr_node);
      rex_leave_crit_sect(&pbm_crit_sect);
      return;
   }

   if (cache_rec == write->curr_rec)
   {
      /* There are no more pending writes.
      * Remove node from committed list. */
      (void) pbm_cache_record_free(curr_node->record);
      PBM_LIST_REMOVE(prev_node, curr_node);

      /* Remove dummy record */
      if (!cache_rec->num_fields)
      {
         (void) pbm_cache_record_free(cache_rec);
         cache->pb_cache_array[location] = NULL;
      }
   } else
   {
      /* Update node with new record */
      (void) pbm_cache_record_free(curr_node->record);
      curr_node->record = write->curr_rec;
      curr_node->record->nref++;
   }
   rex_leave_crit_sect(&pbm_crit_sect);
}


/*===========================================================================
FUNCTION PBM_LIST_RECOVER

DESCRIPTION
   This function is called to recover a cache entry in response to a write error.
   The entry is obtained from the PBM committed list.

   If cache entry is same as current write record, there are no other pending
   writes. In this case we have to do the recovery operation. In the normal case
   we take the record from the corresponding node in commit list and add it to
   the cache.

   If the current operation was partially successful ie deletion was successful
   and add failed, we delete the current cache entry. This can happen only
   for an Update operation on EFS or while adding USIM entries.

   In both cases above, we no longer need the node in commiitted list after this
   and hence the node is removed. Also, if the entry in cache is a dummy record,
   it is converted to a NULL.

DEPENDENCIES
   None. No need to be in critical section to call this function.

SIDE EFFECTS
===========================================================================*/
void pbm_list_recover (const pbm_write_record_s_type *write,
                       boolean partial_success)
{
   PB_CACHE *cache;
   pbm_addr_cache_s_type *cache_rec;
   pbm_addr_node_s_type *curr_node = NULL;
   pbm_addr_node_s_type *prev_node = NULL;
   uint16 location;
   boolean found;
   pbm_return_type ret;

  PBM_CHECK_PTR_RET(write, VOID);

   cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(write->rec_id));
   PBM_CHECK_PTR2_RET(cache, cache->pb_cache_array, VOID);

   location = pbm_record_id_to_location(write->rec_id);
   rex_enter_crit_sect(&pbm_crit_sect);

   cache_rec = cache->pb_cache_array[location];
   if (!cache_rec)
   {
      UIM_MSG_HIGH_1("NULL cache entry found for record %d", write->rec_id);
      rex_leave_crit_sect(&pbm_crit_sect);
      return;
   }

   /* make sure there isn't another write to the same record pending */
   if (write->curr_rec == cache_rec)
   {
      /* Search for the record id within committed list */
      PBM_LIST_SEARCH(write->rec_id, prev_node, curr_node, found);
      if (!found || !curr_node)
      {
         UIM_MSG_HIGH_2("Can't find entry in committed list- rec 0x%x, node 0x%x",
                      write->rec_id, curr_node);
         rex_leave_crit_sect(&pbm_crit_sect);
         return;
      }

      if (partial_success)
      {
         if ((write->curr_rec && pbm_is_usim_required(write->rec_id,
                                                      write->curr_rec->fields,
                                                      write->curr_rec->num_fields)) ||
             (write->prev_rec && pbm_is_usim_required(write->rec_id,
                                                      write->prev_rec->fields,
                                                      write->prev_rec->num_fields)))
         {
            /*Recover USIM record when Add, Delete or Update operation failed*/
            ret = pbm_recover_usim_record(write);
            PBM_CHECK_ERR(ret, "USIM Record Recovery after error failed");
         } else
         {
            /* Deletion was successful and Add failed for EFS entries.
            * We ignore the commit list in this case. */
            ret = pbm_cache_delete(cache, write->rec_id);
            PBM_CHECK_ERR(ret, "Recovery after partial update failed");
         }
      } else
      {
         /* Normal recovery case when Add, Delete or Update operation failed.
         * In this case we add entry from commit list to the cache. */
         ret = pbm_cache_add(cache, curr_node->record, FALSE);
         PBM_CHECK_ERR(ret, "Recovery after error failed");
         curr_node->record->nref++;
      }

      /* we are done with the node in committed list, so it can be removed */
      (void) pbm_cache_record_free(curr_node->record);
      PBM_LIST_REMOVE(prev_node, curr_node);

      /* Remove dummy record, if present */
      if (cache->pb_cache_array[location] &&
          !cache->pb_cache_array[location]->num_fields)
      {
         (void) pbm_cache_record_free(cache->pb_cache_array[location]);
         cache->pb_cache_array[location] = NULL;
      }
   }
   rex_leave_crit_sect(&pbm_crit_sect);
}

/*===========================================================================
FUNCTION PBM_RECOVER_USIM_RECORD

DESCRIPTION
   Restores pb cache to previous entry.  Creates a write command for restoring
   data written to SESSION.  Backup list mechanism is not used, so if this write
   command request fails, then we give up on trying to fix it.

DEPENDENCIES
   Should be called only from PBM task and from pbm_list_recover.  Only used when
   failure adding, deleting, or updating fails on a current or previous record
   entry that contains a USIM field.

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_recover_usim_record (const
                                                pbm_write_record_s_type *write)
{
   pbm_write_record_s_type *write_cmd = NULL;
   pbm_phonebook_type pb_id;
   PB_CACHE                *cache = NULL;
   pbm_return_type          ret;
   pbm_addr_cache_s_type   *prev_rec = NULL;
   pbm_addr_cache_s_type   *record   = NULL;
   uint16                   unique_id;
   uint8                   *curr_index = NULL;
   uint8                   *prev_index = NULL;

   PBM_CHECK_PTR2_RET(write, write->prev_rec, PBM_ERROR);
   pb_id = pbm_session_record_id_to_phonebook_type(write->rec_id);
   cache = pbm_pb_id_to_cache(pb_id);

   if (!cache)
   {
      UIM_MSG_ERR_1("Invalid phonebook id %d", pb_id.device_type);
      return PBM_ERROR;
   }

   do
   {
      /* Allocate index array if USIM fields are present in previous or current
      * record and fill it.
      * There's a race condition here.  If you write 2 records back to back
      * that both require allocating USIM fields, the second one will get the
      * same copy of the IAP, and will overwrite the first record's fields */
      if (!pbm_allocate_index(write->rec_id, write->prev_rec->fields,
                              write->prev_rec->num_fields, &prev_index, &curr_index) ||
          (curr_index && !pbm_get_usim_index(write->rec_id,
                                             write->prev_rec->fields, write->prev_rec->num_fields, curr_index))
          || (prev_index && !pbm_copy_usim_index(write->rec_id, prev_index)))
      {
         UIM_MSG_ERR_3("No space available for USIM fields record %d %d %d",
                     write->rec_id, prev_index, curr_index);
         ret = PBM_ERROR_MEM_FULL;
         break;
      }

      /* Allocate PBM write command */
      write_cmd = (pbm_write_record_s_type *)
                  pbm_cmd_alloc(sizeof(pbm_write_record_s_type));
      if (!write_cmd)
      {
         UIM_MSG_ERR_0("Unable to allocate PBM command");
         ret = PBM_ERROR_MEM_FULL;
         break;
      }

      /* Get a reference to failed current record.
      This will now be our previous record entry for clean up purposes*/
      prev_rec = pbm_cache_record_get(write->rec_id);
      if (!prev_rec)
      {
         UIM_MSG_ERR_1("Unexpected NULL previous rec 0x%x", write->rec_id);
         ret = PBM_ERROR;
         break;
      }

      /* Use our old record as our restoring record */
      record = write->prev_rec;
      if (!record)
      {
         UIM_MSG_ERR_0("Unable to allocate PBM command");
         ret = PBM_ERROR;
         break;
      }

      if (pb_id.device_type == PBM_ADN)
      {
         if (prev_rec->num_fields) /* deletion or update */
         {
            (void) pbm_uim_generate_sync_values(cache, &write_cmd->sync, FALSE);
            unique_id = prev_rec->unique_id;
            /* we didn't generate new UID, so we don't care about wrap around */
         } else
         {
            unique_id = pbm_uim_generate_sync_values(cache,
                                                     &write_cmd->sync, TRUE);
            if (!unique_id)
            {
               /* Do not allow unique id to wrap around */
               UIM_MSG_ERR_0("Unable to allocate SIM unique id");
               /* This could return an error as well, but we don't use SIM UID
                * anywhere, so let's just keep going
                * This really shouldn't happen, as we regerate Sim UIDs when we
                * detect rollover inside pbm_uim_generate_sync_values() */
            }
         }
      } else
      {
         unique_id = 0xFFFF;
      }
      record->unique_id = unique_id;

      /* Restore prevoius record into pbm cache */
      ret = pbm_cache_add(cache, record, FALSE);
      if (ret != PBM_SUCCESS)
      {
         UIM_MSG_ERR_1("Cache add failed with error %d.", ret);
         break;
      }

      /* pbm_cache_add doesn't do this, so we need to do this here. */
      record->nref++;

      /* Allocate index array if USIM fields are present in previous or current
      * record and fill it */
      if (pbm_master_usim_cache_update(write->rec_id, prev_index, curr_index)
          != PBM_SUCCESS)
      {
         (void) pbm_cache_delete(cache, write->rec_id);
         UIM_MSG_ERR_1("Unable to update master usim cache for %d",
                     write->rec_id);
         ret = PBM_ERROR;
         break;
      }

      /* Build and Send restoring PBM command */
      write_cmd->cmd_type   = PBM_CMD_WRITE;
      write_cmd->rec_id     = write->rec_id;
      write_cmd->prev_rec   = prev_rec;
      write_cmd->proc_func  = NULL;
      write_cmd->user_data  = NULL;
      write_cmd->curr_rec        = pbm_cache_record_get(write->rec_id);
      write_cmd->curr_usim_index = curr_index;
      write_cmd->prev_usim_index = prev_index;
      pbm_client_cmd_q_put(PBM_CMD_PTR(write_cmd));

      ret = PBM_SUCCESS;
   } while (0);

   /* Error Handling */
   if (ret != PBM_SUCCESS)
   {
      pbm_free_index(prev_index, curr_index);
      if (write_cmd)
         pbm_cmd_free(PBM_CMD_PTR(write_cmd));
      if (prev_rec)
         (void) pbm_cache_record_free(prev_rec);
      if (record)
         (void) pbm_cache_record_free(record);
   }
   return ret;
}

/*===========================================================================
FUNCTION PBM_CM_INIT

DESCRIPTION
   Initialize CM Client, for network emergency numbers.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cm_init (
  void
)
{
   cm_client_status_e_type cm_status;

   cm_status = cm_client_init(CM_CLIENT_TYPE_PBM, &pbm_cm_client_id);
   if (cm_status != CM_CLIENT_OK)
   {
     UIM_MSG_ERR_2("cm_client_init failed, 0x%x 0x%x",cm_status, pbm_cm_client_id);
     return PBM_ERROR;
   }

  cm_status = cm_mm_client_ss_reg_msim(pbm_cm_client_id,
                                       pbm_cm_ss_event_cb,
  	                               CM_CLIENT_EVENT_REG,
  	                               CM_SS_EVENT_EMERG_NUM_LIST,
  	                               CM_SS_EVENT_EMERG_NUM_LIST,
  	                               NULL,
                                       SYS_MODEM_AS_ID_MASK_ANY);

   if (CM_CLIENT_OK != cm_status)
   {
    UIM_MSG_ERR_2("cm_mm_client_ss_reg_msim failed for emerg_num_list 0x%x 0x%x",
                   cm_status, pbm_cm_client_id);
    return PBM_ERROR;
  }

  cm_status = cm_mm_client_ss_reg_msim(pbm_cm_client_id,
                                       pbm_cm_ss_event_cb,
                                       CM_CLIENT_EVENT_REG,
                                       CM_SS_EVENT_SRV_CHANGED,
                                       CM_SS_EVENT_SRV_CHANGED,
                                       NULL,
                                       SYS_MODEM_AS_ID_MASK_ANY);
  
  if (CM_CLIENT_OK != cm_status)
  {
    UIM_MSG_ERR_2("cm_mm_client_ss_reg_msim failed for srv_changed 0x%x 0x%x",
                   cm_status, pbm_cm_client_id);
    return PBM_ERROR;
  }

   cm_status = cm_client_act(pbm_cm_client_id);
   if (CM_CLIENT_OK != cm_status)
   {
     UIM_MSG_ERR_2("cm_client_act failed 0x%x 0x%x", cm_status, pbm_cm_client_id);
     return PBM_ERROR;
   }
   return PBM_SUCCESS;
}/* pbm_cm_init */


/*===========================================================================
FUNCTION pbm_handle_cm_ss_evt

DESCRIPTION
  This function is called when we get an SS event and it will only look for
  the follwoing events.

  1. CM_SS_EVENT_EMERG_NUM_LIST event to add numbers to the ECC cache.
  2. CM_SS_EVENT_SRV_CHANGED event to update UE srv status.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_handle_cm_ss_evt (
  pbm_cm_ss_evt_type* cm_ss_evt_ptr
)
{
  uint8                           stack_idx        = 0;
  uint8                           j                = 0;
  PB_CACHE                       *ecc_cache        = NULL;
  pbm_return_type                 ret_val          = PBM_SUCCESS;
  pbm_session_enum_type           sess_type        = PBM_SESSION_DEFAULT;
  cm_mm_msim_ss_info_s_type      *ii_info_ptr	   = NULL;

  PBM_CHECK_PTR_RET(cm_ss_evt_ptr, VOID);

  rex_enter_crit_sect(&pbm_crit_sect);

  switch (cm_ss_evt_ptr->ss_event)
  {
    case CM_SS_EVENT_EMERG_NUM_LIST:
    {
      boolean         rebuild_ecc_cache = TRUE;

      ii_info_ptr   = cm_ss_evt_ptr->ii_info_ptr;

      PBM_CHECK_PTR_RET(ii_info_ptr, VOID);
      sess_type = pbm_mapping_as_id_to_pbm_session(ii_info_ptr->asubs_id);
      UIM_MSG_HIGH_2("PBM received CM SS event for sess_type 0x%x as_id 0x%x ",
                      sess_type, ii_info_ptr->asubs_id);

      ecc_cache = pbm_file_id_to_cache(sess_type, PBM_FILE_ECC);
      PBM_CHECK_PTR2_RET(ecc_cache, ecc_cache->pb_cache_array, VOID);

      rebuild_ecc_cache = pbm_is_ota_ecc_update_required(ecc_cache,ii_info_ptr) ;

      if ( sess_type != PBM_SESSION_DEFAULT && rebuild_ecc_cache )
      {
        pbm_ecc_clear_all_id(sess_type, PBM_FIELD_NETWORK_ECC);
        for (stack_idx = 0; stack_idx < ii_info_ptr->number_of_stacks && stack_idx < CM_NO_STACKS; stack_idx++)
        {
           UIM_MSG_HIGH_2("PBM CM SS stack 0x%x list_len 0x%x ",
                           stack_idx, ii_info_ptr->stack_info[stack_idx].emerg_num_list.num_list_len);
           for ( j = 0 ; j < ii_info_ptr->stack_info[stack_idx].emerg_num_list.num_list_len && j < CM_MAX_EMERGENCY_NUM_COUNT; j ++ )
           {
             char   emerg_num[sizeof(ii_info_ptr->stack_info[stack_idx].emerg_num_list.num_list[j].num.buf)+1];
             uint16 emerg_num_len = ii_info_ptr->stack_info[stack_idx].emerg_num_list.num_list[j].num.len;

             if (emerg_num_len == 0 ||
                 (emerg_num_len >= sizeof(emerg_num)))
             {
               UIM_MSG_ERR_4("Emergency number length is 0x%x for item 0x%x of 0x%x stack of len 0x%x",
                             emerg_num_len,
                             j,
                             stack_idx,
                             ii_info_ptr->stack_info[stack_idx].emerg_num_list.num_list_len);
               continue;
             }

             memset(emerg_num, 0, sizeof(emerg_num));
                    (void)memscpy(emerg_num,sizeof(emerg_num),
                    (char*)ii_info_ptr->stack_info[stack_idx].emerg_num_list.num_list[j].num.buf,
                    emerg_num_len);

             ret_val = pbm_build_ecc_record(PBM_FIELD_NETWORK_ECC,
                                            emerg_num_len + 1, /*Add 1 for NULL*/
                                            emerg_num, 0, NULL, TRUE,
                                            (uint8)ii_info_ptr->stack_info[stack_idx].emerg_num_list.num_list[j].num_type,
                                            sess_type,
                                            EMERGENCY_GW,
                                            0);
             if (PBM_SUCCESS != ret_val)
             {
               UIM_MSG_ERR_3("Error 0x%x in building network ECC record 0x%x of 0x%x stack", ret_val, stack_idx, j);
               break;
             }
           }
         }

         ecc_cache = pbm_file_id_to_cache(sess_type, PBM_FILE_ECC);
         PBM_SET_PB_CACHE_READY(ecc_cache);
       }
     }
     break;

    case CM_SS_EVENT_SRV_CHANGED:
    {
      ii_info_ptr = cm_ss_evt_ptr->ii_info_ptr;
      PBM_CHECK_PTR_RET(ii_info_ptr, VOID);

      for (stack_idx = 0; stack_idx < ii_info_ptr->number_of_stacks && stack_idx < CM_NO_STACKS; stack_idx++)
      {
        switch(ii_info_ptr->asubs_id)
        {
          case SYS_MODEM_AS_ID_1:
            if(PBM_SS_INFO_CHANGED(ii_info_ptr->stack_info[0].changed_fields) ||
               (stack_idx == 1 &&
		(ii_info_ptr->stack_info[1].is_operational &&
                 PBM_SS_INFO_CHANGED(ii_info_ptr->stack_info[1].changed_fields))))
            {
              UIM_MSG_HIGH_3("PBM GW Service Status: 0x%x 1X srv status 0x%x roam_status: 0x%x",
                             pbm_session_info[PBM_SESSION_LPB_GW].srv_status,
                             pbm_session_info[PBM_SESSION_LPB_1X].srv_status,
                             ii_info_ptr->stack_info[stack_idx].roam_status);
              if(PBM_3GPP_SYS_MODE(ii_info_ptr->stack_info[stack_idx].sys_mode))
              {
                pbm_session_info[PBM_SESSION_LPB_GW].srv_status  = ii_info_ptr->stack_info[stack_idx].srv_status;
                pbm_session_info[PBM_SESSION_LPB_GW].roam_status = ii_info_ptr->stack_info[stack_idx].roam_status;
              }
              else if(PBM_3GPP2_SYS_MODE(ii_info_ptr->stack_info[stack_idx].sys_mode))
              {
                pbm_session_info[PBM_SESSION_LPB_1X].srv_status  = ii_info_ptr->stack_info[stack_idx].srv_status;
                pbm_session_info[PBM_SESSION_LPB_1X].roam_status = ii_info_ptr->stack_info[stack_idx].roam_status;
              }
              else
              {
                pbm_session_info[PBM_SESSION_LPB_GW].srv_status  = ii_info_ptr->stack_info[stack_idx].srv_status;
                pbm_session_info[PBM_SESSION_LPB_1X].srv_status  = ii_info_ptr->stack_info[stack_idx].srv_status;
                pbm_session_info[PBM_SESSION_LPB_GW].roam_status = ii_info_ptr->stack_info[stack_idx].roam_status;
                pbm_session_info[PBM_SESSION_LPB_1X].roam_status = ii_info_ptr->stack_info[stack_idx].roam_status;
              }
            }

            /* Presently only one configuration is supported where a 2nd stack related
               events is processed for the primary subscription. It is the single
               baseband SGLTE. Since only G can be present on the second stack we check
               for sys mode to be G and only process in that case */
            if (stack_idx == 1 &&
                !ii_info_ptr->stack_info[1].is_operational &&
		ii_info_ptr->stack_info[0].srv_status != SYS_SRV_STATUS_SRV)
            {
              UIM_MSG_HIGH_2("Updating PBM GW Service Status on secondary stack: 0x%x roam_status 0x%x",
                              pbm_session_info[PBM_SESSION_LPB_GW].srv_status,
                              ii_info_ptr->stack_info[1].roam_status);
              pbm_session_info[PBM_SESSION_LPB_GW].srv_status  = ii_info_ptr->stack_info[1].srv_status;
              pbm_session_info[PBM_SESSION_LPB_GW].roam_status = ii_info_ptr->stack_info[1].roam_status;
            }
            break;

          case SYS_MODEM_AS_ID_2:
            UIM_MSG_HIGH_3("PBM GW Service Status: 0x%x 1X srv status 0x%x roam_status: 0x%x",
                            pbm_session_info[PBM_SESSION_LPB_GW_SECONDARY].srv_status,
                            pbm_session_info[PBM_SESSION_LPB_1X_SECONDARY].srv_status,
                            ii_info_ptr->stack_info[stack_idx].roam_status);
            if(PBM_3GPP_SYS_MODE(ii_info_ptr->stack_info[stack_idx].sys_mode))
            {
              pbm_session_info[PBM_SESSION_LPB_GW_SECONDARY].srv_status  = ii_info_ptr->stack_info[stack_idx].srv_status;
              pbm_session_info[PBM_SESSION_LPB_GW_SECONDARY].roam_status = ii_info_ptr->stack_info[stack_idx].roam_status;
            }
            else if(PBM_3GPP2_SYS_MODE(ii_info_ptr->stack_info[stack_idx].sys_mode))
            {
              pbm_session_info[PBM_SESSION_LPB_1X_SECONDARY].srv_status  = ii_info_ptr->stack_info[stack_idx].srv_status;
              pbm_session_info[PBM_SESSION_LPB_1X_SECONDARY].roam_status = ii_info_ptr->stack_info[stack_idx].roam_status;
            }
            else
            {
              pbm_session_info[PBM_SESSION_LPB_GW_SECONDARY].srv_status  = ii_info_ptr->stack_info[stack_idx].srv_status;
              pbm_session_info[PBM_SESSION_LPB_1X_SECONDARY].srv_status  = ii_info_ptr->stack_info[stack_idx].srv_status;
              pbm_session_info[PBM_SESSION_LPB_GW_SECONDARY].roam_status = ii_info_ptr->stack_info[stack_idx].roam_status;
              pbm_session_info[PBM_SESSION_LPB_1X_SECONDARY].roam_status = ii_info_ptr->stack_info[stack_idx].roam_status;
            }
            break;

          case SYS_MODEM_AS_ID_3:
            UIM_MSG_HIGH_2("PBM GW Service Status: 0x%x roam_status: 0x%x",
                            pbm_session_info[PBM_SESSION_LPB_GW_TERTIARY].srv_status,
                            ii_info_ptr->stack_info[stack_idx].roam_status);
            pbm_session_info[PBM_SESSION_LPB_GW_TERTIARY].srv_status  = ii_info_ptr->stack_info[stack_idx].srv_status;
            pbm_session_info[PBM_SESSION_LPB_GW_TERTIARY].roam_status = ii_info_ptr->stack_info[stack_idx].roam_status;
            break;

      default:
         break;
   }
}
    }
    break;

    default:
      break;
  }

  rex_leave_crit_sect(&pbm_crit_sect);
}/* pbm_handle_cm_ss_evt */


/*===========================================================================
FUNCTION pbm_wms_cfg_cb

DESCRIPTION
WMS callback.  Used to start reading from SIM.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_wms_cfg_cb(
pbm_wms_resp_s_type* wms_resp_ptr
)
{
  pbm_session_enum_type pbm_session = PBM_SESSION_GPB_1;

  pbm_session = pbmif_mmgsdi_session_to_pbm_session(wms_resp_ptr->info_ptr.session_type );
  UIM_MSG_HIGH_7("PBM Receieved WMS session 0x%x TxId 0x%x PBM Last TxId 0x%x pbm_session 0x%x pbm_wms_init_finished 0x%x pbm_session_subs_ready 0x%x pbr_status 0x%x",
        wms_resp_ptr->info_ptr.session_type,
        wms_resp_ptr->info_ptr.t_id,
        pbm_session_info[pbm_session].last_txid,
        pbm_session,
        pbm_wms_init_finished,
        pbm_session_subs_ready,
        pbm_session_info[pbm_session].in_use_adn_pb);
  if(pbm_session != PBM_SESSION_MAX &&
     pbm_session_info[pbm_session].last_txid == wms_resp_ptr->info_ptr.t_id)
  {
    PBM_SESSION_SET(pbm_wms_init_finished,pbm_session);
    if(pbm_session_subs_ready & PBM_SESSIONID_TO_SESSION_MASK(pbm_session))
    {
      if ( pbm_session_info[pbm_session].in_use_adn_pb != PBM_PB_NOT_KNOWN )
      {
        pbm_make_fileinfo_request(pbm_session,0);
      }
    }
  }
}/* pbm_wms_cfg_cb */

/*===========================================================================
FUNCTION PBM_POPULATE_ECC_OTHER_IF_PIN_STATUS_CHANGE

DESCRIPTION
  This function takes effect when ecc_hardcode_sim_pin_lock_feature is enabled
  along with NV 69736 pbm_hardcode_ecc_config

  This will return TRUE if hardcode cache is re-built and FALSE if it is NOT re-built

PARAMETERS

SIDE EFFECTS

===========================================================================*/
boolean  pbm_populate_ecc_other_if_pin1_status_change(atomic_word_t pin1_prev_status)
{
  boolean                   ecc_sim_pin_lock_feature = ( pbm_nv_get_feature_status(PBM_FEATURE_ECC_HARDCODE_SIM_ABSENT_LIST_TILL_PIN_LOCK) ) ? TRUE : FALSE ;
  boolean                   ret_val = FALSE ;

  UIM_MSG_HIGH_0("In pbm_populate_ecc_other_if_pin1_status_change");
  if( pbm_hardcode_ecc_config && ecc_sim_pin_lock_feature )
  {
     boolean prev_pin_state = ( pin1_prev_status.value & PBM_MASK_CARD_SESSION_PIN_VALUES ) ? TRUE : FALSE ;
     boolean updated_pin_state = ( pbm_pin1_verified.value & PBM_MASK_CARD_SESSION_PIN_VALUES ) ? TRUE : FALSE ;
     UIM_MSG_HIGH_2("Masked prev pin 0x%x and current pin 0x%x",(pin1_prev_status.value & PBM_MASK_CARD_SESSION_PIN_VALUES) ,
                                                                (pbm_pin1_verified.value & PBM_MASK_CARD_SESSION_PIN_VALUES));
     if ( prev_pin_state != updated_pin_state )
     {
       pbm_ecc_set_hardcoded_eccs(TRUE);
       ret_val = TRUE ;
     }
  }
  else
  {
      UIM_MSG_MED_1("pbm_hardcode_ecc_config %d ",pbm_hardcode_ecc_config);
  }

  return ret_val;

}
/*===========================================================================
FUNCTION PBM_IS_OTA_UPDATE_REQUIRED

DESCRIPTION
 This function is to check if the OTA ECC cache need to be updated based on
 the cache content and latest CM SS ECC data

 TRUE - Requires an update and hence need to re-build the cache and send
        ecc ready event to client
 FALSE - Doesnt need an update - probably the CM SS data and cache data
         are identical

PARAMETERS

SIDE EFFECTS

===========================================================================*/
boolean  pbm_is_ota_ecc_update_required(PB_CACHE *ecc_cache,cm_mm_msim_ss_info_s_type *ii_info_ptr)
{
    int rec_in_stack = 0 ;
    int cache_rec_iter = 0 ; /* cache record iterator */
    int stack_index    = 0 ; /* stack index to iterate over the stacks in ii_info_ptr  */
    uint8  num_of_ota_records_in_cache = 0 ;
    uint8  num_of_ota_records_received = 0 ;

    /* Calculate the number of ecc entries received in CM SS event */
    for ( stack_index = 0 ; stack_index < ii_info_ptr->number_of_stacks && stack_index < CM_NO_STACKS; stack_index++)
    {
      num_of_ota_records_received += ii_info_ptr->stack_info[stack_index].emerg_num_list.num_list_len ;
    }

    /* Calculate the Network ECC records for this session in cache */
    for(cache_rec_iter = 0 ; cache_rec_iter < ecc_cache->num_of_records ; cache_rec_iter++ )
    {
      if ( ecc_cache->pb_cache_array[cache_rec_iter] != NULL  &&
           match_found( PBM_CAT_NONE, PBM_FIELD_NETWORK_ECC , NULL, 0, 0,
                        ecc_cache->pb_cache_array[cache_rec_iter]) )
      {
        num_of_ota_records_in_cache++;
      }
    }

    /* If there is a mismatch in received number of ecc entries from network and current list -
       then proceed to build the cache */

    if( num_of_ota_records_in_cache != num_of_ota_records_received )
    {
       UIM_MSG_HIGH_2("Mis match in Network ECC list in cache (0x%x) and received cm ss event (0x%x) - Rebuild ecc cache",
                       num_of_ota_records_in_cache,num_of_ota_records_received);
       return TRUE ;
    }

    UIM_MSG_HIGH_1("Number of ecc entries match in cache and cm ss list 0x%x",num_of_ota_records_in_cache);
    for( cache_rec_iter = 0 ; cache_rec_iter < ecc_cache->num_of_records ; cache_rec_iter++ )
    {
      if ( ecc_cache->pb_cache_array[cache_rec_iter] != NULL &&
           match_found( PBM_CAT_NONE, PBM_FIELD_NETWORK_ECC , NULL, 0, 0,
                        ecc_cache->pb_cache_array[cache_rec_iter] ) )
      {
         /* Found a NETWORK ECC entry in the cache - check this if this is present in the current cm ss ecc list */
         boolean number_match = FALSE ;
         for (stack_index = 0; stack_index < ii_info_ptr->number_of_stacks && stack_index < CM_NO_STACKS; stack_index++)
         {
            for ( rec_in_stack = 0 ;
                  rec_in_stack < ii_info_ptr->stack_info[stack_index].emerg_num_list.num_list_len && rec_in_stack < CM_MAX_EMERGENCY_NUM_COUNT ;
                  rec_in_stack++ )
            {
                 char     emerg_num[sizeof(ii_info_ptr->stack_info[stack_index].emerg_num_list.num_list[rec_in_stack].num.buf)+1];
                 uint16   emerg_num_len = ii_info_ptr->stack_info[stack_index].emerg_num_list.num_list[rec_in_stack].num.len;
                 uint8    cnt;

                 if (emerg_num_len == 0)
                 {
                    UIM_MSG_ERR_3("Emergency number length is 0 for item %d of %d stack of len %d",
                                  rec_in_stack ,stack_index,ii_info_ptr->stack_info[stack_index].emerg_num_list.num_list_len);
                    continue;
                 }

                 if (emerg_num_len >= sizeof(emerg_num))
                 {
                    UIM_MSG_ERR_2("ECC num len from cm is too large, allow %d, reported %d",
                                   sizeof(emerg_num)-1, emerg_num_len);
                    emerg_num_len = sizeof(emerg_num) - 1;
                 }

                 memset(emerg_num, 0, sizeof(emerg_num));
                 (void)memscpy(emerg_num,
                               sizeof(emerg_num),
                               (char*)ii_info_ptr->stack_info[stack_index].emerg_num_list.num_list[rec_in_stack].num.buf,
                               emerg_num_len);

                 /* Check the number and emergency service category in the cache entry matches with current entry in cm ss list */
                 if( (match_found(PBM_CAT_NONE,
                                  PBM_FIELD_NETWORK_ECC,
                                  emerg_num,
                                  (uint16)emerg_num_len+1 ,
                                  PBM_SEARCHTYPE_EXACT,
                                  ecc_cache->pb_cache_array[cache_rec_iter])))
                 {
                    UIM_MSG_HIGH_3("Number matched in cm ss and cache for index in cache 0x%x stack 0x%x index in stack list 0x%x",cache_rec_iter,stack_index,rec_in_stack);
                    for( cnt = 0 ; cnt < ecc_cache->pb_cache_array[cache_rec_iter]->num_fields ; cnt++ )
                    {
                       if( ecc_cache->pb_cache_array[cache_rec_iter]->fields && ecc_cache->pb_cache_array[cache_rec_iter]->fields[cnt].field_id == PBM_FIELD_ECC_SERVICE_CATEGORY )
                       {
                          uint8 *ecc_category = (uint8 *)pbm_get_field_data_ptr(ecc_cache->pb_cache_array[cache_rec_iter]->fields , cnt);
                          if( ecc_category != NULL )
                          {
                             if( *ecc_category == ii_info_ptr->stack_info[stack_index].emerg_num_list.num_list[rec_in_stack].num_type )
                             {
                                number_match = TRUE ;
                             }
                             UIM_MSG_HIGH_2("Service category in cm ss 0x%x and cache record 0x%x ",ii_info_ptr->stack_info[stack_index].emerg_num_list.num_list[rec_in_stack].num_type , *ecc_category );
                             break;
                          }/* ecc_category != NULL */
                       }
                    }/* for cnt=0 */
                 }

                 if( number_match == TRUE )
                   break;
            } /* for rec_in_stack=0 */

           if( number_match == TRUE )
             break;
         }

         if( number_match == FALSE )
         {
            UIM_MSG_HIGH_1("Number NOT found in the cache for record in cache 0x%x - Rebuild ecc cache",cache_rec_iter);
            return TRUE ;
         }
       }
    }/* for cache_rec_iter=0 */

    UIM_MSG_HIGH_0("OTA ECC cache update is not required for this CM SS event");
    return FALSE ;
}



