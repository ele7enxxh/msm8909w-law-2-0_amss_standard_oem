#ifndef PBM_H
#define PBM_H

/*===========================================================================

           P H O N E B O O K  M A N A G E R  H E A D E R  F I L E

DESCRIPTION
  This is the interface exported by phone book manager service.

 Copyright (c) 2002 - 2016 by QUALCOMM Technologies, Inc(QTI) 
 All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/16   nr      NAL China emergency requirement
03/03/15   stv     Migrate some efs access to uim_common_efs API
08/05/15   nr      Operator specifc NV ECC numbers configuration
09/12/14   stv     Feature to maintain sim absent hardcoded ecc list till pin is verified
05/29/14   stv     NV wrapping for 3gpp spec rel.11 changes
05/16/14   NR      Enabling or Disabling ADN based on NV configurations
01/10/14   NR      Operation Delete AAS Record not working
03/18/10   krishna Used pbmtask_i.h instead if pbmtask.h as part of CMI activity
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/08/08   clm     Queue up pbm notify requests to avoid calling them while in critical sections of pbm.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
08/02/06   cvs     Header file cleanup for WINCE
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
02/24/06   cvs     Reduce pbm_conversion footprint
02/23/06   cvs     rework memory allocation macros
12/06/05   cvs     Wait to init sim phonebooks until after WMS
11/15/05   cvs     Lint fixes
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
08/12/05   AT      Added UCS2 support for strings.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
03/08/05   PA      Support for PBM notifications
01/20/05   PA      Added support for read lock, multiple lock types.
01/14/05   PA      Make PBM GSTK unaware
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
12/17/04   SUN     Fixed Compilation
11/19/04   PA      Support non-GSDI builds
11/18/04   PA      Support for write lock
09/28/04   PA      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "err.h"
#include "pbm_common_types.h"
#include "pbmtask_i.h"
#include "pbmlib.h"
#include "lte.h"
#include "uim_common_efs.h"

#ifdef FEATURE_MODEM_RCINIT_PHASE2
#define PBM_TCB  TASK_HANDLE(pbm)
#else
#define PBM_TCB  &pbm_tcb
#endif 

#define PBM_MASK_CARD_SESSION_PIN_VALUES     0xFFFFFFBC   
/* The card session sepecific bits are set to 0 */ 


extern rex_crit_sect_type pbm_crit_sect;

/*---------------------------------------------------------------
  PBM_FEATURES_STATUS_LIST (71535) NVITEM related defines
-----------------------------------------------------------------*/
/* Max possible feature-elements in FEATURES_STATUS_LIST_TYPE NVITEM struct */
/* RFU should calculated based on the the actual number of bytes 
   used for defining a new feature.*/
#define PBM_MAX_ITEMS_IN_FEATURE_NVITEM           56
/* Current total # of features in FEATURES_STATUS_LIST_TYPE NVITEM struct*/
#define PBM_FEATURES_STATUS_LIST_CURRENT_COUNT    3
#define PBM_FEATURES_STATUS_LIST_CURRENT_VERSION  1

/* Total # of features at each version of NVITEM.. starting with VERSION 0 */
#define PBM_FEATURES_STATUS_LIST_VERSION0_COUNT   2
#define PBM_FEATURES_STATUS_LIST_VERSION1_COUNT   3

/*=====================================
  ENUM:   
PBM_FEATURE_STATUS_ENUM_TYPE
=======================================*/
typedef enum {
  PBM_FEATURE_DISABLED  = 0, /* Feature disabled */
  PBM_FEATURE_ENABLED   = 1  /* Feature enabled */  
} pbm_feature_status_enum_type;

/* ----------------------------------------------------
   STRUCTURE:      phonebook_caching_status_type

   DESCRIPTION:
     Data Structure to hold status of caching status 
     for Global PhoneBook and LocalPhonebook
------------------------------------------------------*/
typedef struct
{
  pbm_feature_status_enum_type gpb_caching_status;
  pbm_feature_status_enum_type lpb_gw_caching_status;
  pbm_feature_status_enum_type lpb_1x_caching_status;
}phonebook_caching_status_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      PBM_FEATURES_STATUS_LIST_TYPE

   DESCRIPTION:
     Data Structure to hold status of pbm features as it reads it from
     efs NVITEM "PBM_FEATURES_STATUS_LIST" (71535)
     * Size of this struct should remain the SAME at all times.
     * "reserved_for_future_use" array items will be hidden from QXDM users at
       all times (hence not writable).
     * "version" should always be the FIRST item.
     * "reserved_for_future_use" should always be the LAST item.
     * "version" is the key here. First implementation of this struct sets it
       to 0.
     * Everytime new pbm item(s) is/are required to be exposed to the
       users via this NVITEM,
       - the item(s) should be added in this struct just before the
         "reserved_for_future_use" item.
       - make sure to decrease the size of "reserved_for_future_use" array by
         the number of items(say, n) added to the struct. That is, increase the
         value of PBM_FEATURES_STATUS_LIST_CURRENT_COUNT by n.
       - increase the value of PBM_FEATURES_STATUS_LIST_CURRENT_VERSION by 1
       - define PBM_FEATURES_STATUS_LIST_VERSIONx_COUNT equal to
         how many ever features/items are added until now (including the ones
         we are currently adding in version x).
       - Update global pbm_features_total_till_version[] by adding an element
         PBM_FEATURES_STATUS_LIST_VERSIONx_COUNT to the array.
         pbm_features_total_till_version[]
       - in pbm_nv_feature_write_default_value(), add case(s) for default
         value of the newly added item(s)/feature(s).
       - in pbm_nv_get_feature_status(), add case(s) for new item(s).
       - add item in enum "pbm_nv_features_enum_type "
       - Notify QXDM team of the modification.
       - In the release notes, mention the new version of this NVITEM.
       - In the release notes, suggest upgrading QXDM to the version to be
         released two weeks from that date (assuming that QXDM would pick our
         updated NVITEM by then). Otherwise, the user won't be able to see and
         update(set) the new entry in the NVITEM. The PBM code would by
         default set the unset entries to their default values.
-------------------------------------------------------------------------------*/
typedef struct {
  uint8 version;
  /* version 0: NEW FEATURE ITEMS = 2; TOTAL CURRENT FEATURE ITEMS = 2 */
  pbm_feature_status_enum_type  fdn_special_handling;
  /* ADN PhoneBook caching staus for GPB and LPB per slot based */
  phonebook_caching_status_type pbm_adn_phonebook_caching_status[3];
  /* version 0: RESERVED_FOR_FUTURE_USE size is 56 - 2 = 54 */
  pbm_feature_status_enum_type  pbm_ecc_hardcode_sim_absent_list_till_pin_lock;
  /* RFU should calculated based on the the actual number of bytes 
     used for defining a new feature.*/
  pbm_feature_status_enum_type  reserve_for_future_use
   [PBM_MAX_ITEMS_IN_FEATURE_NVITEM - PBM_FEATURES_STATUS_LIST_CURRENT_COUNT];
} pbm_features_status_list_type;

/* Global to store PBM_FEATURE_LIST enabled/disabled through NV */
extern pbm_features_status_list_type pbm_features_status_list;

/* The subsequent enums and struct are for the EFS-based NV ITEM
   PBM_FEATURES_STATUS_LIST_TYPE */

/*==================================================
  ENUM:   PBM_NV_FEATURES_ENUM_TYPE
===================================================*/
typedef enum {
  PBM_FEATURE_FDN_SPECIAL_HANDLING = 0,
  PBM_FEATURE_ADN_CACHING,
  PBM_FEATURE_ECC_HARDCODE_SIM_ABSENT_LIST_TILL_PIN_LOCK
} pbm_nv_features_enum_type;

typedef struct
{
   rex_tcb_type         *lock_owner; /* Task that owns the lock, if set indecates that
                                        the lock is acquired */
   pbm_lock_type_e_type  lock_type;  /* Type of lock */
   boolean               done;       /* TRUE if async processing is done. */
} pbm_lock_s_type;

extern pbm_lock_s_type g_lock;


extern boolean            sp_ecc_enabled;

extern uint32 pbm_wms_init_finished;

extern lte_3gpp_release_ver_e  pbm_3gpp_rel_ver;

extern boolean                 pbm_hardcode_ecc_config;

typedef struct
{
   uint8  nref;
   uint16 unique_id;
   pbm_record_id_type record_id;
   uint16 category;
   uint16 num_fields;
   pbm_field_s_type *fields;
} pbm_addr_cache_s_type;
typedef struct { 
  /* Address (index) */
  uint8                                             address;
  /* flag bits */
  uint8                                             status;
  /* Number of digits */
  uint8                                             num_digits;
  /* Array of digits */
  uint8                                             digits[NV_MAX_DIAL_DIGITS];
  /* Name associated with number */
  uint8                                             letters[NV_MAX_LTRS];
  //system mode in which this is emergency
  uint8                             emergency_mode;
} pbm_nv_ecc_type;


#define PBM_HARDCODED_NV_EMERGENCY_NUMBERS    10 
#define PBM_ECC_MAX_DIAL_DIGITS_IN_NV_PER_SUB 14
#define PBM_NV_EMERGENCY_NUMBERS_PER_SUB      50
#define PBM_NV_MAX_DIAL_DIGITS                28 
#define PBM_EMERGENCY_SERVICE_CAT_LEN         1 
#define PBM_EMERGENCY_SERVICE_CAT_VALID_MASK  0x7F
#define PBM_SIM_DIALING_NUMBER_LEN            10

typedef enum
{
  NOT_HARDCODE=-1,
  PBM_HARDCODE_ALWAYS,
  PBM_HARDCODE_CARD_ABSENT,
  PBM_HARDCODE_CARD_PRESENT
}pbm_hardcode_type;

typedef enum
{
  PBM_READ_ECC_FROM_DEFAULT,
  PBM_READ_ECC_FROM_BOTH = PBM_READ_ECC_FROM_DEFAULT,
  PBM_READ_ECC_FROM_DIAL_I,
  PBM_READ_ECC_FROM_EFS_NV,
  PBM_READ_ECC_FROM_MAX = PBM_READ_ECC_FROM_EFS_NV
}pbm_read_ecc_from_enum;
/* PBM_READ_ECC_FROM_BOTH reads the NV ECC from NV_DIAL_I as well as EFS_NV item */


typedef enum
{
  PBM_EMERGENCY_SERVICE_CAT_DEFAULT= 0x00,
  PBM_EMERGENCY_SERVICE_CAT_POLICE = 0x01,
  PBM_EMERGENCY_SERVICE_CAT_AMBULANCE = 0x02,
  PBM_EMERGENCY_SERVICE_CAT_FIRE_BRIGADE = 0x04,
  PBM_EMERGENCY_SERVICE_CAT_MARINE_GUARD = 0x08 ,
  PBM_EMERGENCY_SERVICE_CAT_MOUNTAIN_RESCUE = 0x10 , 
  PBM_EMERGENCY_SERVICE_CAT_MANUAL_INIT_ECALL = 0x20 , 
  PBM_EMERGENCY_SERVICE_CAT_AUTOMATIC_INIT_ECALL = 0x40,
  PBM_EMERGENCY_SERVICE_CAT_RESERVE = 0x80,
  PBM_EMERGENCY_SERVICE_CAT_MAX = PBM_EMERGENCY_SERVICE_CAT_RESERVE
}pbm_service_category_val;


typedef struct {
  uint8           num_digits;
  uint8           digits[PBM_NV_MAX_DIAL_DIGITS];
  uint8           value; // for future use 
  uint8           category_length;
  uint8           emergency_category;
  uint8           reserve; // for future use 
  uint8           emergency_mode;
  uint8           hardcoded_type;
} pbm_hardcoded_ecc_s_type;

typedef struct pbm_nv_ecc_list_per_sub
{
  char                     *dial_string;
  uint32                    string_length;
  uint8                     service_cat_len; /* currently this is only one byte */
  pbm_service_category_val  service_cat_val;
  uint8                     srv_status_flag;
} PBM_NV_ECC_LIST_PER_SUB;

typedef struct {
  uint8           num_digits;
  uint8           digits[PBM_ECC_MAX_DIAL_DIGITS_IN_NV_PER_SUB];
  uint8           category_length;
  uint8           emergency_category;
  uint8           value;   /* Storing service status flag */
  uint8           reserve; /* for future use */
} pbm_nv_ecc_per_sub_s_type;

typedef struct pbm_addr_node_s
{
   pbm_record_id_type rec_id;
   pbm_addr_cache_s_type *record;
   struct pbm_addr_node_s *next;
} pbm_addr_node_s_type;

typedef struct
{
   boolean update_psc;
   uint16  cc;
   uint32  psc;
} pbm_write_sync_s_type;



typedef enum sim_modes
{
   SIM_CODING_GSM7 = 0x00,             /* Coding is done per GSM7 Spec. */
   SIM_CODING_PURE_UCS2 = 0x80,        /* Each character is two octets. */
   SIM_CODING_UCS2_HALF_PAGE_1 = 0x81, /* Octet 3 = Base 0hhh hhhh h000 0000 */
   SIM_CODING_UCS2_HALF_PAGE_2 = 0x82  /* Octets 3&4 specify the base. */
} sim_coding_mode_enum_type;



typedef enum
{
   PBM_WRITE_NOT_INIT,
   PBM_WRITE_DELETE_LINKS,
   PBM_WRITE_SYNC,
   PBM_WRITE_PREV_USIM,
   PBM_WRITE_MASTER,
   PBM_WRITE_CURR_USIM,
   PBM_WRITE_PBC,
   PBM_WRITE_LINKS_DELETED,
   PBM_WRITE_DONE
} pbm_write_state_e_type;

typedef struct pbm_notify_node_s
{
   PBM_NOTIFY_FUNC           notify_func;
   void                     *user_data;
   struct pbm_notify_node_s *next;
} pbm_notify_node_s_type;

extern pbm_notify_node_s_type *pbm_notify_list;
extern pbm_addr_node_s_type *pbm_committed_list;

/* l-value of pointers should not be NULL */
#define PBM_LIST_SEARCH(recid, prev_node, curr_node, found)  \
  {                                                          \
    (found) = FALSE;                                         \
    (prev_node) = NULL;                                      \
    (curr_node) = pbm_committed_list;                        \
    while ((curr_node))                                      \
    {                                                        \
      if ( pbm_compare_rec_ids((curr_node)->rec_id,(recid) ) )                   \
      {                                                      \
        (found) = TRUE;                                      \
        break;                                               \
      }                                                      \
      (prev_node) = (curr_node);                             \
      (curr_node) = (curr_node)->next;                       \
    }                                                        \
  }

#define PBM_LIST_REMOVE(prev_node, curr_node)                \
  {                                                          \
    if ((prev_node))                                         \
      (prev_node)->next = (curr_node)->next;                 \
    else                                                     \
      pbm_committed_list = (curr_node)->next;                \
    PBM_MEM_FREEIF(curr_node);                               \
  }

typedef struct
{
   PBM_DEFINE_CMD_HEADER()
   pbm_record_id_type rec_id;
   pbm_addr_cache_s_type  *curr_rec;
   pbm_addr_cache_s_type  *prev_rec;
   uint8                  *curr_usim_index;
   uint8                  *prev_usim_index;
   pbm_write_sync_s_type   sync;
   PBM_WRITE_CB_FUNC       proc_func;
   void                   *user_data;
} pbm_write_record_s_type;

/*lint -emacro(740, PBM_WRITE_CMD_PTR) */
#define PBM_WRITE_CMD_PTR(x) ((pbm_write_record_s_type *)(x))

typedef struct
{
   PBM_DEFINE_CMD_HEADER()
   pbm_lock_type_e_type lock_type;
   PBM_WRITE_CB_FUNC proc_func;
   void *user_data;
} pbm_write_lock_s_type;

typedef struct
{
   PBM_DEFINE_CMD_HEADER()
   pbm_notify_data_s_type notify_data;
   pbm_session_event_data_u_type session_notify_data;
} pbm_notify_evt_s_type;

typedef struct
{
   int     index; /* Logical index across multiple phonebook sets */
   byte bcd_number[2+(2*PBM_SIM_DIALING_NUMBER_LEN)];      /* BCD coding */
   uint8  sim_text[PBM_TEXT_SIZE_CHARS]; /* null terminated UCS2 string */
   /* Following fields added to support USIM */
   uint32  time;  /* Date and time in clock seconds */
   uint32  duration;  /* in seconds, USIM supports up to 0x00FFFFFF */
   boolean is_answered;  /* TRUE = answered, FALSE = not answered */
} pbm_num_text_record_s_type;

/* A pfsource_fn function gets the next input character as UCS2 and moves the
 * parameter pointer to the next character. */
typedef uint16  (*pfsource_fn)(const void **in_str_ptr);

/*lint -emacro(740, PBM_WRITE_LOCK_CMD_PTR) */
#define PBM_WRITE_LOCK_CMD_PTR(x) ((pbm_write_lock_s_type *)(x))

/*===========================================================================
FUNCTION PBM_INIT

DESCRIPTION
  This function is called after PIN is verified.  It performs these primary
  functions.

  1) Read ADN into memory
  2) Read EXT file attributes
  3) Set up linked list from ADN.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_init ( void );

/*===========================================================================
FUNCTION PBM_INIT_FILE_INFO

DESCRIPTION
  No longer used.  Remains in build until other APPs remove references to
  this.

DEPENDENCIES
  None.  Note used.

SIDE EFFECTS
  None.  Note used.
===========================================================================*/
void pbm_init_file_info(void);

pbm_return_type pbm_uim_del_anrid(uint32 buffer_index,uint16 l_delete_link_index,
                                       pbm_field_s_type *anr_field);

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
);

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
void pbm_ecc_clear_all_id(pbm_session_enum_type session_type, pbm_field_id_e_type field_id);

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
  boolean	uim_present
);

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
SIDE EFFECTS
  None
===========================================================================*/
void pbm_compare_and_update_file
(
  uim_common_efs_item_type   efs_item,
  const char *               buf_ptr,
  uint32                     buf_len
);


/*===========================================================================
FUNCTION PBM_WRITE_NOTIFY

DESCRIPTION
  This function sends the asynchronous result of a PBM write operation

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_notify(const pbm_write_record_s_type *write, pbm_return_type ret_val);

/*===========================================================================
FUNCTION PBM_INT_RECORD_WRITE

DESCRIPTION
  This function performs the actual write of the record to UIM or EFS.
  It is always called from within PBM Task. The address record is already
  added to cache from within the library function.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_int_record_write(pbm_write_record_s_type *write);

/*===========================================================================
FUNCTION PBM_WRITE_CMD_FREE

DESCRIPTION
  This function frees any references to cache records and frees
  the command structure itself.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_write_cmd_free(pbm_write_record_s_type *write);

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
  for an Update operation on EFS.

  In both cases above, we no longer need the node in commiitted list after this
  and hence the node is removed. Also, if the entry in cache is a dummy record,
  it is converted to a NULL.

  In case of UIM, delete and add is the same operation. In this case, this
  function should always be called with 'partial_update' set to FALSE.

DEPENDENCIES
  None. No need to be in critical section to call this function.

SIDE EFFECTS
===========================================================================*/
void pbm_list_recover(const pbm_write_record_s_type *write, boolean delete_succeed);

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

pbm_return_type pbm_list_backup(pbm_record_id_type rec_id);


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
void pbm_list_update(const pbm_write_record_s_type *write);


/*===========================================================================
FUNCTION pbmconvert_parse_input

DESCRIPTION
  Parse input string and determine the best SIM encoding to use
  IIt actually does not convert the string to desired encoding.

  in_str:  [in] Either PBM8 or UCS2 string to store.
  in_len:  [in] Length of the character array.

  out_mode:    [out]  The mode we will use to encode it.
  out_len:  [in/out]  in = Number of octets we can store.
                     out = The number of octets we will need.
  out_base:    [out]  The base if using 81 or 82 coding schemes.
  out_converts:[out] The number of characters that got converted
  src_fn:       [in]  Function that converts input to Unicode.
  gsm7_only:    [in]  True if we must encode using GSM7 alphabet.

  This function looks at the input string, and determines which of
  the encoding schemes would encode the most characters of the input
  string.  To do this, it attempts to convert it to each of the four
  different mechanisms, stopping when it hits something illegal.  If
  gsm7_only is set, we know the other 3 encodings are illegal, thus
  we encode as much as we can. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/


void pbmconvert_parse_input(const void *in_str, uint16 in_len,
                                   sim_coding_mode_enum_type *out_mode, uint16 *out_len,
                                   uint16 *out_base, uint16 *out_converts,
                                   pfsource_fn src_fn, boolean gsm7_only);

/*===========================================================================
FUNCTION pbmconvert_return_input_string_first_ucs2_char

DESCRIPTION
  This function takes the PBM8 or UCS2 string, gets the UCS2 character and
  then increment the pointer by two. It returns the first converted UCS2 
  character.

  in_ptr: [in/out] Source pointer that is moved after we get the octet.

DEPENDENCIES
  None.

RETURN VALUE
  Unicode value at in_ptr;

SIDE EFFECTS
  None.
===========================================================================*/
uint16 pbmconvert_return_input_string_first_ucs2_char(const void **in_ptr);

/*===========================================================================
FUNCTION PBM_NV_GET_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of PBM features listed in enum
  mmgsdi_nv_features_enum_type  as defined by the NVITEM 71535 -
  "PBM_FEATURES_STATUS_LIST".

DEPENDENCIES
  Minimum QXDM version required to be able to read/write this NVITEM is
  latest QXDM(or any version later than 3.14.117).

RETURN VALUE
  pbm_feature_status_enum

SIDE EFFECTS
  None
===========================================================================*/
pbm_feature_status_enum_type pbm_nv_get_feature_status(
  pbm_nv_features_enum_type  nv_feature);
/*===========================================================================
FUNCTION PBM_POPULATE_ECC_OTHER_IF_PIN_STATUS_CHANGE

DESCRIPTION
  This function takes effect when ecc_hardcode_sim_pin_lock_feature is enabled 
  along with NV 69736 pbm_hardcode_ecc_config

  This will return TRUE if hardcode cache is re-built and FALSE if it is NOT re-built

PARAMETERS

SIDE EFFECTS

===========================================================================*/
boolean  pbm_populate_ecc_other_if_pin1_status_change(atomic_word_t pin1_prev_status);

#endif /* ifndef PBM_H */
