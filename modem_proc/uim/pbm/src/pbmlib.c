/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER LIBRARY

GENERAL DESCRIPTION
  This file contains the APIs exported by PBM service which is executed as a
  library within the PBM client's task.

  Copyright (c) 2002 - 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmlib.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/16   nr      NAL China emergency requirement
05/25/16   sp      F3 improvements
05/24/16   nr      Logging improvements
05/23/16   lm      Adding equivalent check for compressed field_type for PBM_FT_PHONE
05/05/16   dd      Enhance write algo to check across all pb_sets for USIM files
11/17/15   stv     Logging improvements
08/05/15   nr      Operator specifc NV ECC numbers configuration
11/14/14   NR      Fixing medium and low level compiler warnings
08/27/14   stv     Return card error correctly when queried for by clients 
08/23/14   NR      Fixing medium and low level compiler warnings
07/07/14   NR      Use v2.0 diag macros
07/07/14   NR      Fixing medium and low level compiler warnings
06/26/14   stv     NV wrapping for Special FDN check 
05/29/14   stv     NV wrapping for 3gpp spec rel.11 changes
05/05/14   stv     reset loc_number_len to actual length for special FDN handling
05/05/14   stv     Added Special FDN handling - Russian numbers
05/01/14   NR      FR 19033: Reduce Code Duplication pbm
03/20/14   stv     Pass correct source buffer length during memscpy in pbm_session_record_read
03/07/14   NR      Fix to check input text length with the sim text length in 
                   write record request to send a proper error
02/16/14   NR      PBM/QMI_PBM KW Critical warnings
02/08/14   NR      PBM has to check for ECC in all subscriptions present in one slot 
                   corresponding to session passed by CM
01/12/14   NR      TMC header deprecation changes
12/19/13   NR      Rel 11 requirement to prefer OTA ECC cat over USIM ECC cat 
10/04/13   stv     F3 message reduction 
10/03/13  nalavala memcpy banned api replacement with memscpy
09/22/10   krishna Fix for 256471
05/10/10   krishna provided the clients Session based field info
03/18/10   krishna Resolved the compilation errors in WM7
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/04/09   krishna changes for CR 167785
04/01/09   kcp     fixed defect in pbm_is_init_completed
04/01/09   kcp     fixed CR146463  MS does not display alpha tags for FDN entries
09/01/09   kcp     fixed compilation errors with VU MSMSHARED_PBM.01.01.23
12/26/08   kcp     added changes for ASUS CR 165600
10/01/08   sg      In pbm_clear_phonebook() shift out all records to clear a
                   circular phonebook
02/13/08   clm     pbm_record_validate() now takes into account a leading '+' digit
                   when determining whether a field of type PBM_FT_PHONE exceeds max length.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/07/07   clm     Add PBM_FIELD_TECH_TYPE and PBM_FIELD_FIELDID_PRIMARY_PHONE
11/01/07   clm     Fix CR130077.  Make it possible to add empty records to
                   circular caches.  This makes EFS and SIM circular record
                   files similar in behavior.
10/24/07   clm     Fix CR125200.  Record id's from external apps should be
                   unaware of special circular cache indexing.
08/08/07   cvs     Fix abort when accessing sim PB's before SIM_INIT_COMPLETED
06/20/07   cvs     Fix invalid ptr access
05/02/07   cvs     Fix emergency category prioritization
03/16/07   cvs     Add support for error category
01/05/07   cvs     Increase allowed numbers of fields
10/10/06   cvs     Move messages to PBM's diag msg ID
08/02/06   cvs     Header file cleanup for WINCE
08/02/06   cvs     Fix problem with prior checkin
07/27/06   slw     Fix menu items displayed when adding FDNs
07/24/06   cvs     Fixes for unequal size multi pb set support
07/11/06   slw     Fix for fields being erased
06/19/06   aps     Fix for fields being written as PBM_FT_STRING
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
04/05/06   cvs     Emergency category priority fixes
03/24/06   cvs     %x -> 0x%x in log messages
03/23/06   cvs     Lint fixes.
03/21/06   cvs     Add function to get extended phonebook info
03/10/06   cvs     Lint fixes
03/09/05   cvs     Fix clearing SIM call history
03/08/06   cvs     Fixes related to emergency categories
03/06/06   cvs     Add new fields
02/23/06   cvs     rework memory allocation macros
02/22/06   cvs     fix pbm_find_name()
02/21/06   cvs     pbm_write_lock() should always call callback if SUCCESS is returned
02/14/06   cvs     Fix num fields in emergency group
01/19/06   cvs     Add pbm_emergency_number_cat(), cleanup emergency nums
01/09/06   cvs     Lint fixes
11/16/05   cvs     Lint fixes
11/15/05   cvs     Fix pbm_find_name()
11/10/05   cvs     Add network ECC numbers
10/18/05   cvs     Add new fields
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
09/27/05   cvs     Move size parameters to their own file.
                   Mainline USE_BEST_FIT_MEMORY_MANAGER.  Change field sizes.
09/01/05   cvs     Fix memory overwrites in pbm_enum_rec_init()
08/30/05   cvs     Revert enum_rec_init() behavior of caching names.  Increase field size.
08/26/05   cvs     Fix lint warning
08/26/05   AT      Added new Emergency Group for searches.
08/24/05   cvs     More optimizations of enum_rec_init(), also fix some cases where
                   we didn't release critical sections in error legs.
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/19/05   cvs     Change error handling on unique IDs (more tolerant)
08/12/05   AT      Added UCS2 support for strings.
08/02/05   cvs     Fix memory leak in pbm_write()
08/01/05   cvs     rename field
07/29/05   AT      Improved some messages.
07/29/05   cvs     Fix deadlock issue by entering pbm_write_lock_crit_sect
                   before entering pbm_crit_sect.
07/27/05   AT      Adding check for maximum record size when presented
                   with a record.
07/08/05   cvs     Add new fields
07/07/05   cvs     Add error messages
06/30/05   AT      Made pbm_enum_rec_init check if the search is legal.
06/28/05   cvs     clean up critical section in error case.
06/23/05   cvs     Speed up searching, using qsort() for results
06/03/05   cvs     Multiproc cleanup, add metacomments
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
04/15/05   cvs     Fix returned values in error cases
04/12/05   AT      Allow PBM_DEFAULT when using legacy functions
                   pbm_file_info and pbm_find_name.
                   Also, only generate a USIM sync value for ADN files.
                   Various lint fixes.
04/08/05   cvs     Add fields
04/01/05   cvs     Fix infinite loop when registering the second notification.
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/28/05   AT      Updated ECC requirements.
03/22/05   PA      Do not update unique id for deletion/update.
03/08/05   PA      Support for PBM notifications
02/08/05   PA      Support for read lock, non-blocking write-lock,
                   support for greater than enumeration.
02/03/05   PA      Free fields in record write if phonebook id is not valid.
01/18/05   PA      Moved checks in record write affected by asynchronous
                   processing to a critical section.
                   Fixed lint errors.
01/12/05   PA      PBM USIM Support
01/10/05   PA      Added support for distribution list
12/18/04   SUN     Get the cache from the record id in pbm_insert_in_sorted_order
12/16/04   SUN     Added a new Addr field
12/15/04   SUN     Get Fields in sorted order
12/08/04   PA      Continue over NULL records in pbm_find_number
12/08/04   PA      Fix memcpy parameters in get field info
12/08/04   PA      Allow write of complex fields.
12/03/04   PA      Record validation and field info APIs
11/18/04   PA      Support for complex fields and write lock.
===========================================================================*/
#include "customer.h"
#include "uim_msg.h"
#include "pbm.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "pbmcache.h"
#include "pbmuim.h"
#include "pbmgsdi.h"
#include "pbmefs.h"
#include "pbmtask_i.h"
#include "stdlib.h"
#include "pbm_conversion.h"
#include "stringl/stringl.h"

/* <EJECT> */
/*===========================================================================
                         DEFINITIONS
===========================================================================*/
static boolean pbm_find_ecc_cat_in_rec(
  pbm_record_id_type   rec_id,
  uint8               *ecc_category_ptr,
  pbm_field_id_e_type *found_field_ptr,
  pbm_ecc_mode_type   *emergency_mode,
  uint8               *srv_status_flag_ptr
);

static boolean pbm_number_search_in_cache(pbm_phonebook_type pb_id,
                                const byte *number,
                                int loc_number_len,
                                pbm_record_s_type * record,
                                PBM_FIND_CB_FUNC proc_func);

/* Save session information in PBM for each slot or subscription (1X/GW) */
SESSION_INFO pbm_session_info[PBM_SESSION_MAX];

// Maximum number of distribution lists a contact can be associated with.
#define PBM_DLISTS_MAX 50

/*===========================================================================
                     Declare data for DS to register with PBM
===========================================================================*/
PBM_EVENT_FUNC ds_ready_fn = NULL;

/*===========================================================================
    Declare static globals for pbm_enum_rec_init() and pbm_enum_next_rec()
===========================================================================*/

#define PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE 0
#define PBM_ENUM_REC_INIT_HANDLE_IN_USE     1
#define PBM_MAX_SUB_FIELDS_IN_COMPLEX 3
static int g_search_enumerator = 0;
static pbm_record_id_type *g_search_results = NULL;
static int g_search_length = 0;
static int g_search_enumerator_ext[PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT] = {0};
static pbm_record_id_type *g_search_results_ext[PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT] = {NULL};
static int g_search_length_ext[PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT] = {0};
static int g_pbm_session_rec_init_handles[PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT] = {0};

//this will store the info abt the init status of EFS and SIMs
pbm_init_info_s_type pbm_init_info = {0};

/*===========================================================================
                              STATIC DATA
===========================================================================*/
static pbm_device_type pbm_default_pb_ids[] = { PBM_EFS, PBM_SDL, PBM_ADN,
PBM_FDN,PBM_SDN
};
static pbm_device_type pbm_ecc_pb_ids[] = { PBM_ECC, PBM_ECC_OTHER};
/* Note: When new fields are added to this table, update PBM_MAX_NUM_FIELDS
 * if required */

  //represent adn & fdn for all sessions
pbm_g_other_field_info_s_type g_sim_field_info[MAX_UIM_APPS][4] =
{
   {
/* ADN Group */
{
   12,
   {
      /* Field ID,   Max No of items, Max length,         Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,            1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_PHONE_GENERIC,   1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_INDEX,           1, PBM_WORD_SIZE,      PBM_FT_WORD,     FALSE, TRUE},
      { PBM_FIELD_NUMBER_TYPE,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, TRUE},
      { PBM_FIELD_NUMBER_PLAN,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     TRUE,  TRUE},
      { PBM_FIELD_GSM_TONNPI,      1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, FALSE},
      { PBM_FIELD_ADDITIONAL_NUMBER, 0, PBM_NUM_SIZE,     PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_GRPNAME_ID,      0, PBM_WORD_SIZE,      PBM_FT_WORD,     TRUE,  TRUE},
      { PBM_FIELD_OTHERNAME,       0, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_EMAIL,           0, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_HIDDEN,          0, 1                 , PBM_FT_BOOLEAN,  TRUE,  TRUE},
      { PBM_FIELD_COMPLEX,         6, PBM_MAX_FIELD_SIZE, PBM_FT_COMPLEX,  FALSE, FALSE},
   }
},
    /* FDN Group */
{
   7,
   {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,            1, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,    TRUE,  TRUE},
      { PBM_FIELD_PHONE_GENERIC,   1, PBM_NUM_SIZE,        PBM_FT_PHONE,   TRUE,  TRUE},
      { PBM_FIELD_INDEX,           1, PBM_WORD_SIZE,       PBM_FT_WORD,    FALSE, TRUE},
      { PBM_FIELD_NUMBER_TYPE,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, TRUE},
      { PBM_FIELD_NUMBER_PLAN,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     TRUE,  TRUE},
      { PBM_FIELD_GSM_TONNPI,      1, PBM_BYTE_SIZE,       PBM_FT_BYTE,    FALSE, FALSE},
      { PBM_FIELD_COMPLEX,         6, PBM_MAX_FIELD_SIZE,  PBM_FT_COMPLEX, FALSE, FALSE},
   }
      },
            /* GRPNAME group */
      {
         1,
         {
            /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
            { PBM_FIELD_NAME,      0, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,    TRUE,  TRUE},
         }
    },
            /* AAS group */
      {
         1,
         {
            /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
            { PBM_FIELD_NAME,      0, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,    TRUE,  TRUE},
         }
    }
  }
};
//represent efs
pbm_g_efs_field_info_s_type g_efs_field_info =
{
   PBM_MAX_NUM_FIELDS,
   {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,                    1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_PHONE_GENERIC,          15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_PHONE_WORK,             15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_PHONE_HOME,             15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_PHONE_FAX,              15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_PHONE_VOICE,            15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_PHONE_CELL,             15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_PHONE_PAGER,            15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_PHONE_OTHER,            15, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_EMAIL,                   2, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_URL,                     2, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_ADDRESS,                 2, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_NOTES,                   1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_ORG,                     1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_JOBTITLE,                1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_LASTNAME,                1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_FIRSTNAME,               1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_OTHERNAME,               1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_NAMEPREFIX,              1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_NAMESUFFIX,              1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_PHOTO,                   1, PBM_MAX_FIELD_SIZE, PBM_FT_BINARY,   TRUE,  TRUE},
      { PBM_FIELD_PHOTOURL,                1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_POBOX,                   1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_STREET,                  1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_ADDREXTRA,               1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_CITY,                    1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_STATE,                   1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_COUNTRY,                 1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_ZIPCODE,                 1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_UID,                     1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_PUBLICKEY,               1, PBM_TEXT_SIZE_BYTES,PBM_FT_BINARY,   TRUE,  TRUE},
      { PBM_FIELD_PUBLICKEYSTRING,         1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_GSM_TONNPI,              1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, FALSE},
      { PBM_FIELD_INDEX,                   1, PBM_WORD_SIZE,      PBM_FT_WORD,     FALSE, TRUE},
      { PBM_FIELD_SPEED_DIAL,              1, PBM_WORD_SIZE,      PBM_FT_WORD,     TRUE,  TRUE},
      { PBM_FIELD_SIP_URL,                 1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_DIAL_STRING,             1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_PHONE_VIDEOCALLERID,     1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_PRIVATE_EMAIL1,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_PRIVATE_EMAIL2,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_EMAIL1,         1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_EMAIL2,         1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_URL,            1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_PHONE_BUSINESS_FAX,      1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_STREET,         1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_CITY,           1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_STATE,          1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_COUNTRY,        1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_POSTALCODE,     1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_NOTES,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_RINGTONE_AUDIO,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_RINGTONE_VIDEO,          1, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BIRTHDAY,                1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_EVENT_UID,               1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_DEFAULT_PHONE,           1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_DEFAULT_EMAIL,           1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_DEFAULT_ADDRESS,         1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_DEFAULT_URL,             1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_DEFAULT_FAX,             1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_DISTRIBUTION_LIST,       1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_COMPLEX,                62, PBM_MAX_FIELD_SIZE, PBM_FT_COMPLEX,  FALSE, FALSE},
      { PBM_FIELD_PRIVATEPHONE_VOICETAG,   1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_BUSINESSPHONE_VOICETAG,  1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_PRIVATEMOBILE_VOICETAG,  1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_BUSINESSMOBILE_VOICETAG, 1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_VIDEOTELEPHONY,          1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_USEDCOUNT,               1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_BCARD,                   1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_GROUP,                   1, PBM_DWORD_SIZE,     PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_DLISTS,      PBM_DLISTS_MAX, PBM_DWORD_SIZE,    PBM_FT_DWORD,    TRUE,  TRUE},
      { PBM_FIELD_PH_UID,                  1, PBM_DDWORD_SIZE,    PBM_FT_BINARY,   TRUE,  TRUE},
      { PBM_FIELD_PRIVATE_ADDITIONALINFO,  1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_BUSINESS_ADDITIONALINFO, 1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_PHONE_VIDEOTELEPHONY,    1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_PREFER_VT,               1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     TRUE,  TRUE},
      { PBM_FIELD_APP_DEFINED_BOOLEAN_FIELD_ATTRIBUTE, 62, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,TRUE,  TRUE},
      { PBM_FIELD_APP_DEFINED_STRING_FIELD_ATTRIBUTE,  62, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,TRUE,  TRUE},
      { PBM_FIELD_APP_DEFINED_BOOLEAN_RECORD_ATTRIBUTE,62, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,TRUE,  TRUE},
      { PBM_FIELD_APP_DEFINED_STRING_RECORD_ATTRIBUTE, 62, PBM_MAX_FIELD_SIZE, PBM_FT_UCS2,TRUE,  TRUE},
      { PBM_FIELD_FIELDID_PRIMARY_PHONE, 1, PBM_WORD_SIZE, PBM_FT_WORD, TRUE,  TRUE},
   }
};
//represent other groups like ecc, call-hist ,sdl
pbm_g_other_field_info_s_type g_rest_field_info[NUM_GROUPS-5] =
{
/* SIM Group */
{
   7,
   {
      /* Field ID,   Max No of items, Max length,         Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,            1, PBM_TEXT_SIZE_BYTES,PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_PHONE_GENERIC,   1, PBM_NUM_SIZE,       PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_INDEX,           1, PBM_WORD_SIZE,      PBM_FT_WORD,     FALSE, TRUE},
      { PBM_FIELD_NUMBER_TYPE,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, TRUE},
      { PBM_FIELD_NUMBER_PLAN,     1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     TRUE,  TRUE},
      { PBM_FIELD_GSM_TONNPI,      1, PBM_BYTE_SIZE,      PBM_FT_BYTE,     FALSE, FALSE},
      { PBM_FIELD_COMPLEX,         6, PBM_MAX_FIELD_SIZE, PBM_FT_COMPLEX,  FALSE, FALSE},
   }
},

/* SDL Group */
{
   7,
   {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,            1, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,    TRUE,  TRUE},
      { PBM_FIELD_PHONE_OTHER,     1, PBM_NUM_SIZE,        PBM_FT_PHONE,   TRUE,  TRUE},
      { PBM_FIELD_PHONE_GENERIC,   1, PBM_NUM_SIZE,        PBM_FT_PHONE,   TRUE,  TRUE},
      { PBM_FIELD_INDEX,           1, PBM_WORD_SIZE,       PBM_FT_WORD,    FALSE, TRUE},
      { PBM_FIELD_VOICETAG,        1, PBM_DWORD_SIZE,      PBM_FT_DWORD,   TRUE,  TRUE},
      { PBM_FIELD_GSM_TONNPI,      1, PBM_BYTE_SIZE,       PBM_FT_BYTE,    FALSE, FALSE},
      { PBM_FIELD_COMPLEX,         6, PBM_MAX_FIELD_SIZE,  PBM_FT_COMPLEX, FALSE, FALSE},
   }
},
/* Call History Group */
{
   12,
   {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,            1, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,   TRUE,  TRUE},
      { PBM_FIELD_PHONE_GENERIC,   1, PBM_NUM_SIZE,        PBM_FT_PHONE,  TRUE,  TRUE},
      { PBM_FIELD_NUMBER_TYPE,     1, PBM_BYTE_SIZE,       PBM_FT_BYTE,   FALSE, TRUE},
      { PBM_FIELD_NUMBER_PLAN,     1, PBM_BYTE_SIZE,       PBM_FT_BYTE,   TRUE,  TRUE},
      { PBM_FIELD_GSM_TONNPI,      1, PBM_BYTE_SIZE,       PBM_FT_BYTE,   FALSE, FALSE},
      { PBM_FIELD_CALL_TYPE,       1, PBM_WORD_SIZE,       PBM_FT_WORD,   FALSE, TRUE},
      { PBM_FIELD_DATE,            1, PBM_DWORD_SIZE,      PBM_FT_DWORD,  FALSE, TRUE},
      { PBM_FIELD_DURATION,        1, PBM_DWORD_SIZE,      PBM_FT_DWORD,  FALSE, TRUE},
      { PBM_FIELD_COMPLEX,         8, PBM_MAX_FIELD_SIZE,  PBM_FT_COMPLEX,FALSE, FALSE},
      { PBM_FIELD_CALL_METHOD,     1, PBM_WORD_SIZE,       PBM_FT_WORD,   FALSE, TRUE},
      { PBM_FIELD_AOC,             1, PBM_MAX_FIELD_SIZE,  PBM_FT_BINARY, FALSE, TRUE},
      { PBM_FIELD_TECH_TYPE,       1, PBM_WORD_SIZE,       PBM_FT_WORD,   TRUE,  TRUE},
   }
},

/* Emergency Group */
{
   6,
   {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,                 1, PBM_TEXT_SIZE_BYTES, PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_HARDCODED_ECC,        1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_NV_ECC,               1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_SIM_ECC,             1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_NETWORK_ECC,          1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_ECC_SERVICE_CATEGORY, 1, PBM_BYTE_SIZE,       PBM_FT_BYTE,     FALSE, TRUE}
   }
},

/* Hardcoded and NV Emergency Group */
{
   3,
   {
      /* Field ID,   Max No of items, Max length,       Field type, Search any cat, search this cat */
      { PBM_FIELD_NAME,                 1, NV_MAX_LTRS,         PBM_FT_UCS2,     TRUE,  TRUE},
      { PBM_FIELD_HARDCODED_ECC,        1, PBM_NUM_SIZE,        PBM_FT_PHONE,    TRUE,  TRUE},
      { PBM_FIELD_NV_ECC,               1, NV_MAX_DIAL_DIGITS,  PBM_FT_PHONE,    TRUE,  TRUE}
   }
},

};


#define PBM_MAX_REQUIRED_FIELDS 3
//below array should follow the same order as the diff PB Groups
static const int g_required_fields[NUM_GROUPS][PBM_MAX_REQUIRED_FIELDS] = {
{ 0, 0, 0},
{ PBM_FIELD_NAME, 0, 0},
{ 0, 0, 0},
{ 0, 0, 0},
{ 0, 0, 0},
{ 0, 0, 0},
{ 0, 0, 0},
{ 0, 0, 0},
{ 0, 0, 0}
};

/*===========================================================================*
 *              PHONE BOOK MANAGER RECORD BASED API                          *
 *                                                                           *
 * These functions supply the API for the record based requests.             *
 *===========================================================================*/


/*===========================================================================
FUNCTION PBM_SESSION_CALCULATE_FIELDS_SIZE_FROM_ID

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure
  based on the record ID.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_session_calculate_fields_size_from_id( pbm_record_id_type rec_id )
{
    PB_CACHE *cache;
    int size = 0,num_fields_uncompressed=0;
    uint16 location;
    pbm_return_type ret;
    pbm_phonebook_type pb_id = {PBM_GPB};
    pbm_session_enum_type session_type = PBM_SESSION_MAX;
    uint8 i;
    pbm_field_s_type *uncompressed_data_buf=NULL;

    pb_id = pbm_session_record_id_to_phonebook_type(rec_id);
    location = pbm_session_record_id_to_location(rec_id);

    rex_enter_crit_sect(&pbm_crit_sect);

    if(pb_id.device_type == PBM_ECC)
    {
        boolean loc_found = FALSE;
        for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids) && loc_found == FALSE; ++i)
        {
        /* case for all EFs in pbm_default_pb_ids */
            switch (pbm_ecc_pb_ids[i])
            {
                case PBM_ECC_OTHER:
                /* check the number of records for EFs in EFS */
                    if ( NULL != (cache = pbm_device_id_to_cache(session_type, PBM_ECC_OTHER)))
                    {
                        if (cache->status == PBM_STATUS_READY)
                        {
                            if(location <= cache->num_of_records)
                            {
                                pb_id.device_type = PBM_ECC_OTHER;
                                loc_found = TRUE;
                            }
                            else
                            {
                                location -= cache->num_of_records;
                            }
                        }
                    }
                    break;
            case PBM_ECC:
            default:
            /* Follow app preference for traversing all EFs present under ADF DIR */
                session_type = pbm_record_id_to_session_type (rec_id);
                {
                    /* check the number of records for EFs in EFS */
                    if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[i])))
                    {
                        if (cache->status == PBM_STATUS_READY)
                        {
                            if(location <= cache->num_of_records)
                            {
                                pb_id.device_type = PBM_ECC;
                                pb_id.prov_type =	pbm_session_type_to_provision_type(session_type);
                                pb_id.pb_category =  pbm_session_type_to_pb_category(session_type);
                                loc_found = TRUE;
                            }
                            else
                            {
                                location -= cache->num_of_records;
                            }

                        }
                    }
                }
                break;
            }
        }

    }


   do
   {
      if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
      {
        ret = PBM_ERROR;
        break;
      }

      if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
          ret != PBM_ERROR_SYNC)
      {
         break;
      }


      if (location > cache->num_of_records)
      {
         UIM_MSG_ERR_2("Location %d is higher than length of cache %d.",
                     location, cache->num_of_records);
         ret = PBM_ERROR_INDEX_INVALID;
         break;
      }

      if (cache->circular == TRUE)
      {
         //For circular buffers we need to so some special translation for location
         location = PBM_CIRC_CACHE_NTHLOC(cache, location);
      }

      if (location == 0)
      {
         UIM_MSG_ERR_0("Location cannot be zero.");
         ret = PBM_ERROR_NOT_FOUND;
         break;
      }

      if ((cache->pb_cache_array == NULL) ||
          (cache->pb_cache_array[location] == NULL) ||
          (cache->pb_cache_array[location]->fields == NULL) ||
          (cache->pb_cache_array[location]->num_fields == 0))
      {
         ret = PBM_ERROR_NOT_FOUND;
         break;
      }

      if(PBM_SUCCESS != pbm_uncompress_fields(cache->pb_cache_array[location]->fields,
                                                   cache->pb_cache_array[location]->num_fields,
                                                   &uncompressed_data_buf,
                                                   &num_fields_uncompressed,
                                                   FALSE))
      {
        UIM_MSG_ERR_0("Can't copy fields");
        ret = PBM_ERROR;
      }
   } while (0); /*lint !e717 */

   rex_leave_crit_sect(&pbm_crit_sect);


   if (ret == PBM_SUCCESS)
   {
     size = pbm_calculate_size_from_fields(uncompressed_data_buf, num_fields_uncompressed);
   }

   PBM_MEM_FREEIF(uncompressed_data_buf);
   return size;
}

/*===========================================================================
 FUNCTION PBM_SESSION_CALCULATE_UNDECODED_FIELDS_SIZE_FROM_ID

 DESCRIPTION
   Calculates how much memory is needed for a copy of the fields structure
   based on the record ID.

 DEPENDENCIES

 SIDE EFFECTS
===========================================================================*/
int pbm_session_calculate_undecoded_fields_size_from_id( pbm_record_id_type rec_id )
{
   PB_CACHE                     *cache;
   int                          size = 0,num_fields_uncompressed=0;
   uint16                       location;
   pbm_return_type              ret;
   pbm_phonebook_type           pb_id = {PBM_GPB};
   pbm_session_enum_type        session_type = PBM_SESSION_MAX;
   uint8                        i;
   pbm_field_s_type             *uncompressed_data_buf=NULL;

   pb_id = pbm_session_record_id_to_phonebook_type(rec_id);
   location = pbm_session_record_id_to_location(rec_id);

   rex_enter_crit_sect(&pbm_crit_sect);
   if(pb_id.device_type == PBM_ECC)
   {
     boolean loc_found = FALSE;
     for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids) && loc_found == FALSE; ++i)
     {
       /* case for all EFs in pbm_default_pb_ids */
       switch (pbm_ecc_pb_ids[i])
       {
         case PBM_ECC_OTHER:
           /* check the number of records for EFs in EFS */
           if ( NULL != (cache = pbm_device_id_to_cache(session_type, PBM_ECC_OTHER)))
           {
             if (cache->status == PBM_STATUS_READY)
             {
               if(location <= cache->num_of_records)
               {
                 pb_id.device_type = PBM_ECC_OTHER;
                 loc_found = TRUE;
               }
               else
               {
                 location -= cache->num_of_records;
               }
             }
           }
           break;
         case PBM_ECC:
         default:
           /* Follow app preference for traversing all EFs present under ADF DIR */
           session_type = pbm_record_id_to_session_type (rec_id);
           {
             /* check the number of records for EFs in EFS */
             if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[i])))
             {
               if (cache->status == PBM_STATUS_READY)
               {
                 if(location <= cache->num_of_records)
                 {
                   pb_id.device_type = PBM_ECC;
                   pb_id.prov_type   = pbm_session_type_to_provision_type(session_type);
                   pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
                   loc_found         = TRUE;
                 }
                 else
                 {
                   location -= cache->num_of_records;
                 }
               }
             }
           }
           break;
         }
       }
     }

     do
     {
       if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
       {
         ret = PBM_ERROR;
         break;
       }

       if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS && ret != PBM_ERROR_SYNC)
       {
         break;
       }

       if (location > cache->num_of_records)
       {
         UIM_MSG_ERR_2("Location %d is higher than length of cache %d.",
                      location, cache->num_of_records);
         ret = PBM_ERROR_INDEX_INVALID;
         break;
       }

       if (cache->circular == TRUE)
       {
         //For circular buffers we need to so some special translation for location
         location = PBM_CIRC_CACHE_NTHLOC(cache, location);
       }

       if (location == 0)
       {
         UIM_MSG_ERR_0("Location cannot be zero.");
         ret = PBM_ERROR_NOT_FOUND;
         break;
       }

       if ((cache->pb_cache_array == NULL) ||
           (cache->pb_cache_array[location] == NULL) ||
           (cache->pb_cache_array[location]->fields == NULL) ||
           (cache->pb_cache_array[location]->num_fields == 0))
       {
         ret = PBM_ERROR_NOT_FOUND;
         break;
       }

       if(PBM_SUCCESS != pbm_uncompress_fields(cache->pb_cache_array[location]->fields,
                                               cache->pb_cache_array[location]->num_fields,
                                               &uncompressed_data_buf,
                                               &num_fields_uncompressed,
                                               TRUE))
       {
         UIM_MSG_ERR_0("Can't copy fields");
         ret = PBM_ERROR;
       }
     } while (0); /*lint !e717 */

     rex_leave_crit_sect(&pbm_crit_sect);

     if (ret == PBM_SUCCESS)
     {
       size = pbm_calculate_size_from_fields(uncompressed_data_buf, num_fields_uncompressed);
     }

     PBM_MEM_FREEIF(uncompressed_data_buf);
     return size;
}

/*===========================================================================
FUNCTION PBM_SESSION_RECORD_READ

DESCRIPTION
Searches the cache appropriate to the given phonebook and retrieves
information contained in a specified record, returning success/failure
indication.  Does not delete the record from the address book.  If no
record is found, PBM_ERROR_NOT_FOUND is returned.

The layout of data_buf when it is returned is that it starts with
*num_fields_ptr number of pbm_field_s_type structures.  After those structures,
is the data for those fields.  pbm_get_field_data_pointer() can get pointers
to the actual field data.


DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_record_read(
  pbm_record_id_type            rec_id,
  uint16                       *category_ptr,
  int                          *num_fields_ptr,
  uint8                        *data_buf,
  uint32                        data_buf_size
)
{
  PB_CACHE                     *cache                 = NULL;
  uint16                        location              = 0;
  pbm_return_type               ret                   = PBM_ERROR;
  pbm_phonebook_type            pb_id                 = {PBM_GPB};
  pbm_session_enum_type         session_type          = PBM_SESSION_MAX;
  uint8                         i                     = 0;
  pbm_field_s_type             *uncompressed_data_buf = NULL;
  uint32                        src_buf_size          = 0;

  PBM_CHECK_PTR_RET(num_fields_ptr, PBM_ERROR);

  *num_fields_ptr = 0;

  pb_id = pbm_session_record_id_to_phonebook_type(rec_id);
  location = pbm_session_record_id_to_location(rec_id);

  rex_enter_crit_sect(&pbm_crit_sect);

  if(pb_id.device_type == PBM_ECC)
  {
    boolean loc_found = FALSE;

    for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids) && loc_found == FALSE; ++i)
    {
      /* case for all EFs in pbm_default_pb_ids */
      switch (pbm_ecc_pb_ids[i])
      {
        case PBM_ECC_OTHER:
          /* check the number of records for EFs in EFS */
          if ( NULL != (cache = pbm_device_id_to_cache(session_type, PBM_ECC_OTHER)))
          {
            if (cache->status == PBM_STATUS_READY)
            {
              if(location <= cache->num_of_records)
              {
                pb_id.device_type = PBM_ECC_OTHER;
                loc_found = TRUE;
              }
              else
              {
                location -= cache->num_of_records;
              }
            }
          }
          break;

        case PBM_ECC:
        default:
          /* Follow app preference for traversing all EFs present under ADF DIR */
          session_type = pbm_record_id_to_session_type (rec_id);
          /* check the number of records for EFs in EFS */
          if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[i])))
          {
            if (cache->status == PBM_STATUS_READY)
            {
              if(location <= cache->num_of_records)
              {
                pb_id.device_type = PBM_ECC;
                pb_id.prov_type =  pbm_session_type_to_provision_type(session_type);
                pb_id.pb_category =  pbm_session_type_to_pb_category(session_type);
                loc_found = TRUE;
              }
              else
              {
                location -= cache->num_of_records;
              }
            }
          }
          break;
        }
      }
    }

    do
    {
      if((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
      {
        ret = PBM_ERROR;
        break;
      }

      if((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
          ret != PBM_ERROR_SYNC)
      {
        break;
      }

      if(location > cache->num_of_records)
      {
        UIM_MSG_ERR_0("Location is higher than length of cache.");
        ret = PBM_ERROR_INDEX_INVALID;
        break;
      }

      if(cache->circular == TRUE)
      {
        /* For circular buffers we need to so some special translation for location */
        location = PBM_CIRC_CACHE_NTHLOC(cache, location);
      }

      if(location == 0)
      {
        UIM_MSG_ERR_0("Location cannot be zero.");
        ret = PBM_ERROR_NOT_FOUND;
        break;
      }

      if((cache->pb_cache_array == NULL) ||
         (cache->pb_cache_array[location] == NULL) ||
         (cache->pb_cache_array[location]->fields == NULL) ||
         (cache->pb_cache_array[location]->num_fields == 0))
      {
        ret = PBM_ERROR_NOT_FOUND;
        break;
      }

      if(category_ptr)
      {
        *category_ptr   = cache->pb_cache_array[location]->category;
      }

      if(num_fields_ptr)
      {
        *num_fields_ptr = cache->pb_cache_array[location]->num_fields;
      }

      if(PBM_SUCCESS != pbm_uncompress_fields((const pbm_field_s_type *)
                                               cache->pb_cache_array[location]->fields,
                                               cache->pb_cache_array[location]->num_fields,
                                               &uncompressed_data_buf,
                                               num_fields_ptr,
                                               FALSE))
      {
        UIM_MSG_ERR_0("Can't copy fields");
        ret = PBM_ERROR;
        break;
      }

      src_buf_size = pbm_calculate_size_from_fields(uncompressed_data_buf, *num_fields_ptr);
      memscpy(data_buf, data_buf_size, uncompressed_data_buf, src_buf_size);
      PBM_MEM_FREEIF(uncompressed_data_buf);
      ret = PBM_SUCCESS;
      if(pb_id.device_type != PBM_ECC && pb_id.device_type != PBM_ECC_OTHER)
      {
        UIM_MSG_HIGH_5("PBM: Attempt to read non ECC compress record 0x%x from slot 0x%x, device type 0x%x, pb category 0x%x compress found 0x%x fields",
                        rec_id,
                        pb_id.slot_id,
                        pb_id.device_type,
                        pb_id.pb_category,
                        cache->pb_cache_array[location]->num_fields);
    }
    } while (0);
    rex_leave_crit_sect(&pbm_crit_sect);
    return ret;
}


/*===========================================================================
FUNCTION PBM_SESSION_UNDECODED_RECORD_READ

DESCRIPTION
Searches the cache appropriate to the given phonebook and retrieves
information contained in a specified record, returning success/failure
indication.  Does not delete the record from the address book.  If no
record is found, PBM_ERROR_NOT_FOUND is returned.

The layout of data_buf when it is returned is that it starts with
*num_fields_ptr number of pbm_field_s_type structures.  After those structures,
is the data for those fields.  pbm_get_field_data_pointer() can get pointers
to the actual field data.

This has the text fields like NAME, EMAIL,SNE in undecoded format i.e. same as that stored in SIM card 
The number fields will be still converted from BCD to ASCII


DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_record_read_undecoded(
  pbm_record_id_type            rec_id,
  uint16                       *category_ptr,
  int                          *num_fields_ptr,
  uint8                        *data_buf,
  uint32                        data_buf_size)
{
  PB_CACHE                           *cache                 = NULL;
  uint16                              location              = 0;
  pbm_return_type                     ret                   = PBM_ERROR;
  pbm_phonebook_type                  pb_id                 = {PBM_GPB};
  pbm_session_enum_type               session_type          = PBM_SESSION_MAX;
  uint8                               i                     = 0;
  pbm_field_s_type                   *uncompressed_data_buf = NULL;

  PBM_CHECK_PTR_RET(num_fields_ptr, PBM_ERROR);

  *num_fields_ptr = 0;

  pb_id = pbm_session_record_id_to_phonebook_type(rec_id);
  location = pbm_session_record_id_to_location(rec_id);
  
  UIM_MSG_HIGH_1("PBM: Attempt to read record undecoded record 0x%x", rec_id);

  rex_enter_crit_sect(&pbm_crit_sect);

  if(pb_id.device_type == PBM_ECC)
  {
    boolean loc_found = FALSE;

    for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids) && loc_found == FALSE; ++i)
    {
      /* case for all EFs in pbm_default_pb_ids */
      switch (pbm_ecc_pb_ids[i])
      {
        case PBM_ECC_OTHER:
          /* check the number of records for EFs in EFS */
          if( NULL != (cache = pbm_device_id_to_cache(session_type, PBM_ECC_OTHER)))
          {
            if(cache->status == PBM_STATUS_READY)
            {
              if(location <= cache->num_of_records)
              {
                pb_id.device_type = PBM_ECC_OTHER;
                loc_found = TRUE;
              }
              else
              {
                location -= cache->num_of_records;
              }
            }
          }
          break;

        case PBM_ECC:
        default:
          /* Follow app preference for traversing all EFs present under ADF DIR */
          session_type = pbm_record_id_to_session_type (rec_id);
          /* check the number of records for EFs in EFS */
          if(NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[i])))
          {
            if(cache->status == PBM_STATUS_READY)
            {
              if(location <= cache->num_of_records)
              {
                pb_id.device_type = PBM_ECC;
                pb_id.prov_type =  pbm_session_type_to_provision_type(session_type);
                pb_id.pb_category =  pbm_session_type_to_pb_category(session_type);
                loc_found = TRUE;
              }
              else
              {
                location -= cache->num_of_records;
              }
            }
          }
          break;
        }
      }
    }

    do
    {
      if((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
      {
        ret = PBM_ERROR;
        break;
      }

      if((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
          ret != PBM_ERROR_SYNC)
      {
        break;
      }

      if(location > cache->num_of_records)
      {
        UIM_MSG_ERR_0("Location is higher than length of cache.");
        ret = PBM_ERROR_INDEX_INVALID;
        break;
      }

      if (cache->circular == TRUE)
      {
        /* For circular buffers we need to so some special translation for location */
        location = PBM_CIRC_CACHE_NTHLOC(cache, location);
      }

      if(location == 0)
      {
        UIM_MSG_ERR_0("Location cannot be zero.");
        ret = PBM_ERROR_NOT_FOUND;
        break;
      }

      if((cache->pb_cache_array == NULL) ||
         (cache->pb_cache_array[location] == NULL) ||
         (cache->pb_cache_array[location]->fields == NULL) ||
         (cache->pb_cache_array[location]->num_fields == 0))
      {
        ret = PBM_ERROR_NOT_FOUND;
        break;
      }

      if(category_ptr)
      {
        *category_ptr   = cache->pb_cache_array[location]->category;
      }

      if (num_fields_ptr)
      {
        *num_fields_ptr = cache->pb_cache_array[location]->num_fields;
      }

      if(PBM_SUCCESS != pbm_uncompress_fields((const pbm_field_s_type *)
                                               cache->pb_cache_array[location]->fields,
                                               cache->pb_cache_array[location]->num_fields,
                                               &uncompressed_data_buf,
                                               num_fields_ptr,
                                               TRUE))
      {
        UIM_MSG_ERR_0("Can't copy fields");
        ret = PBM_ERROR;
        break;
      }

      memscpy(data_buf, data_buf_size, uncompressed_data_buf, data_buf_size);
      PBM_MEM_FREEIF(uncompressed_data_buf);
      ret = PBM_SUCCESS;
    } while (0);

    rex_leave_crit_sect(&pbm_crit_sect);
    return ret;
} /* pbm_session_record_read_undecoded */

/*===========================================================================
FUNCTION PBM_SESSION_RECORD_WRITE

DESCRIPTION
Add/updates/deletes a record in the address book (both EFS and SIM).  This is
the new interface that supports many different fields per contact.

To add a record:
1)  Set rec_id = 0xFFFF (PBM_SESSION_GET_FIRST_AVAILABLE) (and rec_id_ptr=&rec_id).
2)  Set items_ptr equal to an array of fields for the new record
3)  Set item_count equal to the length of above array
    To update a record, do the same as when adding a record (above), except set
    rec_id to the record ID of the record to be updated.  Note that the fields
    given in items_list will replace all fields in the current record, i.e. the
    current fields will be deleted and replaced, not added to.

To delete a record:
1)  Set rec_id to the record ID of the record to be deleted.
2)  Set data_buf = NULL.
3)  Other fields are not used.

When making an asynchronous call, a success return value indicates that
the command has been successfully queued. The actual result of the operation
is given in the first argument to the callback function proc_func(). The
resulting record id is returned synchronously within rec_id_ptr and along
with the proc_func callback.

The layout of data_buf when it is returned is that it starts with
*num_fields_ptr number of pbm_field_s_type structures.  After those structures,
is the data for those fields.  pbm_get_field_data_pointer() can get pointers
to the actual field data.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_record_write( pbm_phonebook_type      pb_id,
                                  pbm_record_id_type     *rec_id_ptr,
                                  uint16                  cat,
                                  int                     num_fields,
                                  const uint8            *data_buf,
                                  int                     data_buf_size,
                                  PBM_WRITE_CB_FUNC       proc_func,
                                  void                   *user_data)
{
   pbm_write_record_s_type *write_cmd = NULL;
   PB_CACHE                *cache = pbm_pb_id_to_cache(pb_id);
   pbm_return_type          ret;
   pbm_record_id_type       rec_id;
   pbm_addr_cache_s_type   *prev_rec = NULL;
   pbm_addr_cache_s_type   *record = NULL;
   uint16                   unique_id;
   boolean                  deletion;
   boolean                  new = FALSE;
   int                      location;
   uint8                   *curr_index = NULL;
   uint8                   *prev_index = NULL;
   uint64                   ph_unique_id;
   pbm_field_s_type        *my_data_buf = NULL;
   uint8 l_j = 0;
   pbm_session_enum_type session_type = pbm_mapping_to_session(pb_id.pb_category,
                                                               (pbm_slot_type)pb_id.slot_id,
                                                               pb_id.prov_type);

   if (!cache)
   {
      UIM_MSG_ERR_1("Invalid phonebook id %d", pb_id.device_type);
      return PBM_ERROR;
   }

   PBM_CHECK_PTR_RET(rec_id_ptr, PBM_ERROR);

   UIM_MSG_HIGH_2("PBM: Attempt to write %d fields to record 0x%x",
                num_fields, *rec_id_ptr);


   rex_enter_crit_sect(&pbm_crit_sect);

   ret = pbm_session_record_validate(pb_id, *rec_id_ptr,
                                (pbm_cat_e_type)cat, (uint8*)data_buf,
                                data_buf_size, num_fields);

   rex_leave_crit_sect(&pbm_crit_sect);
   if ( ret != PBM_SUCCESS )
   {
    UIM_MSG_ERR_1(" Record validation failed during write %d", ret);
     return ret;
   }

   /* make a copy of the data_buf.  data_buf belogs to the caller, and
    * my_data_buf belongs to PBM.  If data_buf is NULL, this is probably a
    * deletion.  my_data_buf is already NULL, and that's handled later */
   if (data_buf != NULL && num_fields != 0)
   {
     pbm_field_s_type *new_fields_ptr;
       //if atleast 1 simple anr field , then convert it
       //into complex anr before adding the record to cache
     ret = pbm_convert_to_anr_ext((const pbm_field_s_type *)data_buf,
                                                  num_fields,&new_fields_ptr);
       if (PBM_SUCCESS != ret)
       {
         UIM_MSG_ERR_1("Error %d while copying fields", ret);
         PBM_MEM_FREEIF(my_data_buf);
         return PBM_ERROR;
       }
     ret = pbm_compress_fields(new_fields_ptr,
                                 num_fields,
                                 &my_data_buf);
     if (PBM_SUCCESS != ret)
      {
       UIM_MSG_ERR_1("Error %d while copying fields", ret);
       PBM_MEM_FREEIF(new_fields_ptr);
       PBM_MEM_FREEIF(my_data_buf);
       return PBM_ERROR;
   }
     PBM_MEM_FREEIF(new_fields_ptr);
   }

   do
   {
      rex_enter_crit_sect(&pbm_crit_sect);

      /* Deletion is not allowed on circular pb's. Best we can do is
         write empty records in to shift out all old records. */
      deletion = (cache->circular)?(FALSE):(num_fields == 0 ? TRUE : FALSE);

      /* Find location of the entry within cache */
      if (*rec_id_ptr == PBM_SESSION_GET_FIRST_AVAILABLE)
      {
         /* Give me the first available slot */
         if ((location = pbm_get_first_available(cache, deletion)) == 0)
         {
            UIM_MSG_HIGH_2("Could not find first available for cache %d %d.",
                         cache->num_of_records, cache->records_in_use);
            ret = PBM_ERROR_MEM_FULL;
            break;
         }
         else if(pb_id.device_type == PBM_ECC_OTHER && location > PBM_NV_EMERGENCY_NUMBERS)
         {
            UIM_MSG_HIGH_2("NV ECC cache full %d %d.",
                        cache->num_of_records, cache->records_in_use);
           ret = PBM_ERROR_MEM_FULL;
           break;
         }
         new = TRUE;
         rec_id = pbm_session_location_to_record_id(pb_id, location);
      } else
      {
         if (cache->circular == TRUE)
         {
            /* Cannot to write to anything but the PBM_SESSION_GET_FIRST_AVAILABLE location
               in a circular file */
            ret = PBM_ERROR_NOT_SUPPORTED;
            break;
         }
         location = pbm_session_record_id_to_location(*rec_id_ptr);
         if(pb_id.device_type == PBM_ECC_OTHER && location > PBM_NV_EMERGENCY_NUMBERS)
         {
           UIM_MSG_HIGH_1("NV ECC PBM_ERROR_INDEX_INVALID %d.",location);
           ret = PBM_ERROR_INDEX_INVALID;
           break;
         }

         rec_id = *rec_id_ptr;
      }

      UIM_MSG_HIGH_2("PBM: pbm_record_write writing %d fields to record 0x%x",
                   num_fields, rec_id);

      if (location > cache->num_of_records)
      {
         UIM_MSG_ERR_0("Write: index is outside cache.");
         ret = PBM_ERROR_INDEX_INVALID;
         break;
      }

      /* If it is deletion and record is not present, return error */
      if (deletion &&
          (!cache->pb_cache_array || !cache->pb_cache_array[location] ||
           !cache->pb_cache_array[location]->num_fields))
      {
         UIM_MSG_ERR_2("Write: Attempt to delete empty record pb_id %d, location %d",
                     pb_id.device_type, location);
         ret = PBM_ERROR_NOT_FOUND;
         break;
      }

      /* Allocate index array if USIM fields are present in previous or current
         record and fill it. There's a race condition here.
         If you write 2 records back to back that both require allocating USIM fields,
         the second one will get the same copy of the IAP, and will overwrite the first record's fields */
      if (!pbm_allocate_index(rec_id, my_data_buf, num_fields, &prev_index, &curr_index) ||
          (curr_index && !pbm_get_usim_index(rec_id, my_data_buf, num_fields, curr_index)) ||
          (prev_index && !pbm_copy_usim_index(rec_id, prev_index)))
      {
        pbm_pb_set_type      pb_set                    = PBM_USIM_PB_INVALID;
        pbm_pb_set_type      input_pb_set              = PBM_USIM_PB_INVALID;
        uint16               records_per_pb_set        = 0;
        uint16               records_in_current_pb_set = 0;
        boolean              found_free_entry          = FALSE;

        /* No need to search across all pb_sets if the
           record_id is other than PBM_SESSION_GET_FIRST_AVAILABLE */
        if (*rec_id_ptr != PBM_SESSION_GET_FIRST_AVAILABLE)
        {
          ret = PBM_ERROR_MEM_FULL;
          break;
        }

        input_pb_set       = pbm_record_id_to_pb_set(rec_id);
        /* Calculating the number of records to calucate
           the correct location to be start for searching USIM availabilty */
        for(pb_set = PBM_USIM_PB_ONE; pb_set <= input_pb_set; pb_set++)
        {
          records_per_pb_set += cache->records_in_pb_set[pb_set];
        }

        /* This means no space for USIM files in current pb_set.
           So we should try out remianign pb_sets if any before we error out to the user */
        for(pb_set = input_pb_set + 1; pb_set < PBM_MAX_NUM_PB_SETS; pb_set++)
        {
          records_in_current_pb_set = records_per_pb_set + cache->records_in_pb_set[pb_set];
          for(location = records_per_pb_set + 1;
              location <= records_in_current_pb_set && location <= cache->num_of_records;
              location++)
          {
            rec_id = pbm_session_location_to_record_id(pb_id, location);

            /* If this is location is a hidden location,
               then we should not treat it as a empty location */
            if(pbm_is_rec_hidden(rec_id) || !cache->pb_cache_array ||
               (cache->pb_cache_array[location] &&
                cache->pb_cache_array[location]->num_fields))
            {
              continue;
            }

            /* Allocate index array if USIM fields are present in previous or current
               record and fill it. There's a race condition here.
               If you write 2 records back to back that both require allocating USIM fields,
               the second one will get the same copy of the IAP, and will overwrite the first record's fields */
            if (!pbm_allocate_index(rec_id, my_data_buf, num_fields, &prev_index, &curr_index) ||
                (curr_index && !pbm_get_usim_index(rec_id, my_data_buf, num_fields, curr_index)) ||
                (prev_index && !pbm_copy_usim_index(rec_id, prev_index)))
            {
              /* Moving to next pb_set to check availabilty */
              break;
            }

            /* Found a valid space for USIM in this pb_set */
            found_free_entry = TRUE;
            break;
          }

          if(found_free_entry)
          {
            UIM_MSG_HIGH_2("PBM: pbm_record_write writing 0x%x fields to record 0x%x", num_fields, rec_id);

            /* If we have reached means we would found some space for writing cotnacts with USIM fields too */
            break;
          }

          /* Incrementing record location to move to start of the next pb_set */
          records_per_pb_set += cache->records_in_pb_set[pb_set];
        }

        /* No space found for USIM files across all pb_sets present on the card */
        if(pb_set == PBM_MAX_NUM_PB_SETS)
        {
          UIM_MSG_ERR_2("No space available for USIM fields record 0x%x pb_set 0x%x", rec_id, pb_set);
          ret = PBM_ERROR_MEM_FULL;
          break;
        }
      }
      //trying to write a hidden contact
      //whereas the support OR memory not present
      for(l_j = 0;l_j < num_fields; l_j++)
      {
        if(my_data_buf!=NULL
           && my_data_buf[l_j].field_id == PBM_FIELD_HIDDEN
           && pbm_get_field_data_ptr(my_data_buf,l_j) != NULL
           && *((byte *) pbm_get_field_data_ptr(my_data_buf,l_j)) == TRUE
           && pbc_cache[session_type].num_of_records < location)
        {
          ret = PBM_ERROR_MEM_FULL;
          break;
        }
      }
      if(ret == PBM_ERROR_MEM_FULL)
      {
	 //break the outer do while
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

      /* Save previous record to commit list if required */
      ret = pbm_list_backup(rec_id);
      if (ret != PBM_SUCCESS)
      {
         UIM_MSG_ERR_2("Unable to save previous entry (rec_id: %d) to commit list %d",
                     rec_id, ret);
         break;
      }

      /* Get a reference to previous record */
      prev_rec = pbm_cache_record_get(rec_id);
      if (!prev_rec)
      {
         UIM_MSG_ERR_1("Unexpected NULL previous record 0x%x", rec_id);
         ret = PBM_ERROR_MEM_FULL;
         break;
      }

      /* Allocate new cache record */
      record = pbm_cache_record_new();
      if (!record)
      {
         UIM_MSG_ERR_0("Unable to allocate new record");
         ret = PBM_ERROR_MEM_FULL;
         break;
      }

      /* Generate unique id */
      if (pbm_action_requires_sim(pb_id))
      {
         if (pb_id.device_type == PBM_ADN)
         {
            if (prev_rec->num_fields) /* deletion or update */
            {
               (void) pbm_uim_generate_sync_values(cache, &write_cmd->sync, FALSE);
               unique_id = prev_rec->unique_id;
               /* we didn't generate a new UID, so we don't care about wrap around */
            } else
            {
               unique_id = pbm_uim_generate_sync_values(cache, &write_cmd->sync, TRUE);

               if (!unique_id)
               {
                  /* Do not allow unique id to wrap around */
                  UIM_MSG_ERR_0("Unable to allocate SIM unique id");

                  /* This could return an error as well, but we don't use SIM UID */
                  /* anywhere, so let's just keep going */
                  /* This really shouldn't happen, as we regerate Sim UIDs when we */
                  /* detect rollover inside pbm_uim_generate_sync_values() */
               }
            }
         } else
         {
            unique_id = 0xFFFF;
         }
      } else
      {
         unique_id = pbm_efs_generate_unique_id();

         if (!unique_id)
         {
            /* Do not allow unique id to wrap around */
            UIM_MSG_ERR_0("Unable to allocate PBM unique id");
            ret = PBM_ERROR;
            break;
         }
      }

      /* Update cache using the new entry */
      if (deletion)
      {
         pbm_cache_record_fill(record, rec_id, unique_id, cat, 0, 0);

         /* Delete record in cache */
         ret = pbm_cache_delete(cache, rec_id);
         PBM_CHECK_ERR_BREAK(ret,"PBM cache delete failed with error");

         cache->pb_cache_array[location] = record; /* Dummy record */
      } else
      {
         /* If adding a new record to the EFS phone-book, update the field
          * list to append a new phone unique ID field. The validation check
          * performed earlier already checked that the field did not exist. */
         if (pb_id.device_type == PBM_EFS)
         {
            uint16 new_num_fields = (uint16)num_fields;

            if (new)
            {
               ret = pbm_get_next_phone_unique_id(&ph_unique_id);
               PBM_CHECK_ERR_BREAK(ret, "Get next unique ID failed");

               ret = pbm_append_phone_uid_field(ph_unique_id, &my_data_buf, &new_num_fields);
               PBM_CHECK_ERR_BREAK(ret, "Assign phone uid failed");

               num_fields = (int)new_num_fields;
            } else
            {
               if (!pbm_find_phone_uid_field(my_data_buf, num_fields, NULL))
               {
                  /* Find current PH_UID field value from cache, and append it to
                   * the update field list. */
                  ret = pbm_get_phone_unique_id_from_record(prev_rec, &ph_unique_id);
                  if (ret != PBM_SUCCESS)
                  {
                     UIM_MSG_ERR_2("Failed with error %d in locating phone uid for existing record %d",
                                 ret, rec_id);

                     /* This is a pathological situation since the PBM EFS initialization
                      * should have assigned unique IDs to all old entries. */
                     ret = pbm_get_next_phone_unique_id(&ph_unique_id);
                     PBM_CHECK_ERR_BREAK(ret, "Get next unique ID failed");
                  }

                  ret = pbm_append_phone_uid_field(ph_unique_id, &my_data_buf, &new_num_fields);
                  PBM_CHECK_ERR_BREAK(ret, "Assign phone uid failed");

                  num_fields = (int)new_num_fields;
               }
            }
         }
           /* Add record to cache, existing record is overwritten */
           pbm_cache_record_fill(record, rec_id, unique_id, cat,
                                 (uint16) num_fields, my_data_buf);

           ret = pbm_cache_add(cache, record, new);
           if (ret != PBM_SUCCESS)
           {
              UIM_MSG_ERR_1("Cache add failed with error %d.", ret);
              break;
           }

      }

      /* Allocate index array if USIM fields are present in previous or current
       * record and fill it */
      ret = pbm_master_usim_cache_update(rec_id, prev_index, curr_index);
      if (PBM_SUCCESS != ret)
      {
         (void) pbm_cache_delete(cache, rec_id);
         UIM_MSG_ERR_2("Failed with error %d to update master usim cache for %d", ret, rec_id);
         ret = PBM_ERROR;
         break;
      }

      rex_leave_crit_sect(&pbm_crit_sect);

      /* Send PBM command */
      write_cmd->cmd_type   = PBM_CMD_WRITE;
      write_cmd->rec_id     = rec_id;
      write_cmd->prev_rec   = prev_rec;
      write_cmd->proc_func  = proc_func;
      write_cmd->user_data  = user_data;

      write_cmd->curr_rec        = pbm_cache_record_get(rec_id);
      write_cmd->curr_usim_index = curr_index;
      write_cmd->prev_usim_index = prev_index;
      pbm_client_cmd_q_put(PBM_CMD_PTR(write_cmd));

      if (cache->circular == TRUE)
      {
         //Go back to hiding cache location details for circular cache types
         location = PBM_CIRC_CACHE_LOCTOINDEX(cache, location);
         rec_id = pbm_session_location_to_record_id(pb_id, location);
      }
      *rec_id_ptr = rec_id;

      ret = PBM_SUCCESS;

   } while (0); /*lint !e717 */

   /* Error Handling */
   if (ret != PBM_SUCCESS)
   {
      if (my_data_buf)
         pbm_free_fields_internal(&my_data_buf, num_fields);
      pbm_free_index(prev_index, curr_index);
      if (write_cmd)
         pbm_cmd_free(PBM_CMD_PTR(write_cmd));
      if (prev_rec)
         (void) pbm_cache_record_free(prev_rec);
      if (record)
         (void) pbm_cache_record_free(record);
	  rex_leave_crit_sect(&pbm_crit_sect);
   }
   return ret;
}


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_REC_INIT

DESCRIPTION
Initializes searches of the address book for specific records

Does search to set up for subsequent calls to pbm_enum_next_rec(), which
iterates through the records that match the given criteria.  Search results
are stored in static variables and are overwritten by the next call to
pbm_rec_init().

Searching rules are as follows:
1)  Only records with the given category are matched.  If PBM_CAT_NONE is
    specified, then all categories are matched.
2)  Only records that contain at least one instance of a field with the
    given field ID are matched.  If PBM_FIELD_NONE is specified, then
    this criterion is ignored.
3)  Only records with data in the specified field that "matches" the data
    specified by data_ptr are matched.  If data_ptr is NULL, then this
    criterion is ignored.

  What "matches" means depends on the setting of flags:
          Default (0x0) is case insensitive, starts with, string match
          with FIELD_NONE.
          Bit 0:    0   - PBM_CASE_INSENSITIVE
          Bit 0:    1   - PBM_CASE_SENSITIVE
          Bit 1-2:  0   - PBM_SEARCHTYPE_STARTSWITH
          Bit 1-2:  1   - PBM_SEARCHTYPE_EXACT
          Bit 1-2:  10  - PBM_SEARCHTYPE_CONTAINS
          Bit 3:    0   - PBM_DATATYPE_STR
          Bit 3:    1   - PBM_DATATYPE_BYTE
          Bit 4:    0   - PBM_FIELDID_NONE
          Bit 4:    1   - PBM_FIELDID_PHONE (Any type of phone)
          Bit 5:    0   - PBM_CALLHISTORY_NONE
          Bit 5:    1   - PBM_CALLHISTORY_ALL (incoming, outgoing, missed)
          Bit 6:    0   - PBM_ENUMERATE_ALL
          Bit 6:    1   - PBM_ENUMERATE_AVAILABLE
          Bit 7:    0   - PBM_MATCHING_NORMAL
          Bit 7:    1   - PBM_MATCHING_GREATER (Not supported with SEARCHTYPE_CONTAINS).
          Bit 8:    0   - PBM_SORT_LASTNAME_FIRST
          Bit 8:    1   - PBM_SORT_LASTNAME_SECOND

  Example:  pbm_rec_init(PBM_EFS, PBM_CAT_NONE, PBM_FIELD_NONE, "joh", 3,
                         PBM_CASE_INSENSITIVE | PBM_SEARCHTYPE_STARTSWITH)
            would match on the names "John Smith" and "Johann Bach"

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_rec_init(pbm_phonebook_type pb_id,
                                  uint16 category,
                                  pbm_field_id_e_type field_id,
                                  const uint8 *data_ptr,
                                  uint16 data_size,
                                  uint32 flags)
{
   pbm_return_type            ret = PBM_SUCCESS;
   PB_CACHE                   *cache;
   pbm_session_enum_type      session_type = PBM_SESSION_DEFAULT;
   pbm_record_id_type         *temp_results = NULL; /* Will point to array of record IDs */
   int                        result_index;
   int                        size = 0;
   int                        num_caches = 0;
   int                        caches_used = 0;
   int                        i;
   int                        cidx;
   pbm_cat_e_type             my_cat = (pbm_cat_e_type)category;
   pbm_internal_field_info    *field_info_ptr = NULL;


   boolean avail_flag = (((flags & PBM_ENUMERATE_MASK) ==
                          PBM_ENUMERATE_AVAILABLE) ? TRUE : FALSE);

   PB_CACHE *pb_cache_list[PBM_RESERVED] = {NULL,}; /* Cache list. */
   pbm_device_type *pb_ids = NULL;                  /* Phonebook list */

   PB_CACHE *pb_cache = NULL;    /* Used when list is one big. */
   pbm_device_type ch_pb_ids[] = {PBM_DIALED, PBM_RCVD, PBM_MISS};

   /* First make sure the category is valid for the search. */
   if (pb_id.device_type != PBM_ECC_OTHER &&
          my_cat != PBM_CAT_NONE && pb_id.device_type != pbm_cat_to_phonebook(my_cat))
   {
      UIM_MSG_ERR_2("Category %d and phonebook %d does not match",
                  my_cat, pb_id.device_type);
      return PBM_ERROR_CAT_NOT_SUPPORTED;
   }

   PBM_MEM_ALLOC( field_info_ptr,sizeof(pbm_internal_field_info));
   if (NULL == field_info_ptr)
   {
   
     UIM_MSG_ERR_0("malloc failed for field_info_ptr");
     return PBM_ERROR;
   }

   if(pbm_get_internal_field_info(pb_id,field_info_ptr) == PBM_SUCCESS)
   {
     /* Now if they are searching for a field, make sure they can. */
     if (field_id != PBM_FIELD_NONE)
     {
        for (i = 0; i < field_info_ptr->num_fields; i++)
        {
           if (field_id == field_info_ptr->info[i].field_id)
           {
              if (((my_cat == PBM_CAT_NONE) && !field_info_ptr->info[i].field_search) ||
                  ((my_cat != PBM_CAT_NONE) && !field_info_ptr->info[i].field_cat_search))
              {
                 PBM_MEM_FREEIF (field_info_ptr);
                 return PBM_ERROR_NOT_SUPPORTED;
              }
              break;
           }
        }
     }
   }
   else
   {
     PBM_MEM_FREEIF (field_info_ptr);
     return PBM_ERROR;
   }

   PBM_MEM_FREEIF (field_info_ptr);
   rex_enter_crit_sect(&pbm_crit_sect);
   PBM_MEM_FREEIF(g_search_results);

   /* When all call history phonebooks are enumerated, pb_id
    * could be PBM_DEFAULT */
   if (pb_id.device_type == PBM_DEFAULT)
   {
      if ((flags & PBM_CALLHISTORY_MASK) == PBM_CALLHISTORY_ALL)
      {
         pb_ids = ch_pb_ids;
         num_caches = ARR_SIZE(ch_pb_ids);
      } else
      {
         pb_ids = pbm_default_pb_ids;
         num_caches = ARR_SIZE(pbm_default_pb_ids);
      }
      /* Check if the caches are ready.
       * For PBM_DEFAULT, no SIM and no SIM file should not be returned as an
       * error */
      for (cidx = 0; cidx < num_caches; ++cidx)
      {/* case for all EFs in pbm_default_pb_ids */
         switch (pb_ids[cidx])
         {
            case PBM_MISS:
            case PBM_RCVD:
            case PBM_DIALED:
            case PBM_EFS:
            case PBM_SDL:
               //session_type is optional since for computing above phonebook's caches it is not required
               if ( NULL != (cache = pbm_device_id_to_cache(session_type, pb_ids[cidx])))
               {/* check the number of records for files in EFS */
                  if (cache->status == PBM_STATUS_READY)
                  {
                     pb_cache_list[caches_used++] = cache;
                  } else
                  {
                     UIM_MSG_ERR_2("Cache for PB %d not available %d", (pb_ids?pb_ids[cidx]:pb_id.device_type), ret);
                     if (!avail_flag &&
                         ret != PBM_ERROR_NO_SIM && ret != PBM_ERROR_NOT_AVAILABLE && ret != PBM_ERROR_RESTRICTED)
                     {
                        goto END_FUNC_ENUM_REC_INIT;
                     }
                  }
               }
               break;
            case PBM_ADN:
            case PBM_SDN:
            case PBM_FDN:
            default:
               /* Follow app preference for traversing all EFs present under ADF DIR */
               for (session_type = PBM_SESSION_DEFAULT; session_type < PBM_SESSION_MAX; session_type++)
               {
                  if ( NULL != (cache = pbm_device_id_to_cache(session_type, pb_ids[cidx])))
                  {/* check the number of records for files in EFS */
                     if (cache->status == PBM_STATUS_READY)
                     {
                        pb_cache_list[caches_used++] = cache;
                     }
                     else
                     {
                        UIM_MSG_ERR_2("Cache for PB %d not available %d", ((pb_ids)?pb_ids[cidx]:pb_id.device_type), ret);
                        //(ret != PBM_ERROR_RESTRICTED)check is to skip if particular cache have restricted access CR146463
                        //ex - some SIMs may restrict access to ADN records when FDN is disabled
                        if (!avail_flag &&
                            ret != PBM_ERROR_NO_SIM && ret != PBM_ERROR_NOT_AVAILABLE && ret != PBM_ERROR_RESTRICTED)
                        {
                           goto END_FUNC_ENUM_REC_INIT;
                        }
                     }
                  }
               }
               break;
         }
      }
   } /*  if (pb_id.device_type == PBM_DEFAULT)... */
   //process all the ECC caches
   else if (pb_id.device_type == PBM_ECC)
   {
      num_caches = ARR_SIZE(pbm_ecc_pb_ids);
      for (cidx = 0; cidx < num_caches; ++cidx)
      {
         switch (pbm_ecc_pb_ids[cidx])
         {
            case PBM_ECC_OTHER:
               //session_type is optional since for computing above phonebook's caches it is not required
               if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[cidx])))
               {/* check the number of records for files in EFS */
                  if (cache->status == PBM_STATUS_READY)
                     pb_cache_list[caches_used++] = cache;
               }
               break;
            case PBM_ECC:
            default:
               /* Follow app preference for traversing all EFs present under ADF DIR */
               session_type = pbm_mapping_to_session(pb_id.pb_category,(pbm_slot_type) pb_id.slot_id, pb_id.prov_type);
               {
                  if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[cidx])))
                  {/* check the number of records for files in EFS */
                     if (cache->status == PBM_STATUS_READY)
                        pb_cache_list[caches_used++] = cache;
                  }
               }
               break;
         }
      }
   } else
   {
      /* Check if the cache is ready */
      if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS)
      {
         goto END_FUNC_ENUM_REC_INIT;
      }

      pb_cache_list[0] = pbm_pb_id_to_cache(pb_id);
      caches_used = 1;
   }


   /* Allocate the maximum number of possible results. */
   for (cidx = 0; cidx < caches_used; ++cidx)
   {
      size += pb_cache_list[cidx]->records_in_use;
   }

   if (size == 0)
   {
      g_search_length = 0;
      g_search_enumerator = 0;
      ret = PBM_SUCCESS;
      goto END_FUNC_ENUM_REC_INIT;

   }

   /* Allocate srch results array*/
   PBM_MEM_ALLOC(temp_results,(uint32)size * sizeof(pbm_record_id_type));

   if (temp_results == NULL)
   {
      g_search_length = 0;
      g_search_enumerator = 0;
      ret = PBM_ERROR;
      goto END_FUNC_ENUM_REC_INIT;
   }

   result_index = 0;

   /* All call history phonebooks */
   /* pb_id could be PBM_DEFAULT or any one of the call history phonebooks */
   for (cidx = 0; cidx < caches_used; ++cidx)
   {
      pb_cache = pb_cache_list[cidx];

      if (!pb_cache)
      {
         UIM_MSG_ERR_1("Cache was NULL for index %d", cidx);
         continue;
      }

      for ( i = 1; i <= pb_cache->num_of_records; i++ )
      {
         int index;

         if (pb_cache->circular)
            index = PBM_CIRC_CACHE_NTHLOC(pb_cache, i);
         else
            index = i;

         if ((pb_cache->pb_cache_array) && (pb_cache->pb_cache_array[index]))
         {
            if (match_found(my_cat, field_id, data_ptr, data_size, flags,
                            pb_cache->pb_cache_array[index]))
            {
               if (result_index >= size)
               {
                  UIM_MSG_ERR_2("Allocated %d records, index %d", size, result_index);
                  break;
               }
               if (pb_cache->circular)
               {
                  //Hide the location shifting details for circular files from outside apps.
                  temp_results[result_index++] = pbm_session_location_to_record_id(pb_cache->pb_id, i);
               } else
               {
                  temp_results[result_index++] = pb_cache->pb_cache_array[index]->record_id;
               }
            }
         }
      }
   }
   /* Allocate g_search_results[] and copy results in*/
   do
   {
      if (result_index == 0)
      {
         g_search_results = NULL;
         g_search_length = 0;
      } else
      {
         PBM_MEM_ALLOC(g_search_results, (uint32)result_index * sizeof(g_search_results[0]));
         PBM_CHECK_PTR_BREAK(g_search_results, ret, PBM_ERROR_MEM_FULL);

         /* g_search_results is a 0-based array*/
         for (i=0; i<result_index; i++)
         {
            g_search_results[i] = temp_results[i];
         }

         g_search_length = (int) result_index;
      }
   } while ( 0 ); /*lint !e717 */

   /* clean up the temp results */
   PBM_MEM_FREEIF(temp_results);
   /* Initialize enumerator*/
   g_search_enumerator = 0;
   ret = PBM_SUCCESS;
   END_FUNC_ENUM_REC_INIT:
   rex_leave_crit_sect(&pbm_crit_sect);
   return ret;

}


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_NEXT_REC_ID

DESCRIPTION
Retrieves the next record ID that matches the search criteria
specified in the most recent call to pbm_enum_rec_init().

Only one search at a time is supported.  This can allow situations where
one application has called pbm_enum_rec_init() and is in the process of
calling pbm_enum_next_rec_id() a few times.  Then another application calls
pbm_enum_rec_init() and changes the search criteria and resets the search
before the first application is done.

When the end of the enumeration is reached, the index is not reset to the
beginning of the enumeration.  All subsequent calls to this function must
continue to return PBM_EOF.  The caller must call pbm_enum_rec_init() to
reinitialize the search criteria.

DEPENDENCIES
pbm_enum_rec_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_next_rec_id(pbm_record_id_type *rec_id_ptr)
{
   pbm_return_type ret;
   PB_CACHE *cache;
   uint16 location;
   pbm_phonebook_type pb_id = {PBM_GPB};

   PBM_CHECK_PTR_RET(rec_id_ptr, PBM_ERROR);

   rex_enter_crit_sect(&pbm_crit_sect);
   /* Empty list?*/
   if (!g_search_results)
   {
      *rec_id_ptr = PBM_RECID_NULL;
      ret = PBM_ERROR_NOT_FOUND;
      goto END_FUNC_ENUM_NEXT_REC_ID;
   }

   /* Skip over blank entries*/
   /* (Blank because they have been deleted from the cache)*/
   while (g_search_enumerator < g_search_length)
   {
      *rec_id_ptr = g_search_results[g_search_enumerator];

      /* see if the entry is blank */
      pb_id = pbm_session_record_id_to_phonebook_type(*rec_id_ptr);

      if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
      {
         *rec_id_ptr = PBM_RECID_NULL;
         ret = PBM_ERROR_NOT_FOUND;
         goto END_FUNC_ENUM_NEXT_REC_ID;
      }
      do
      {
         if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
             ret != PBM_ERROR_SYNC)
         {
            break;
         }
         location = pbm_session_record_id_to_location(*rec_id_ptr);

         if (location > cache->num_of_records)
         {
            UIM_MSG_ERR_0("Location is higher than length of cache.");
            ret = PBM_ERROR_INDEX_INVALID;
            break;
         }
         if (cache->circular == TRUE)
         {
            //For circular buffers we need to so some special translation for location
            location = PBM_CIRC_CACHE_NTHLOC(cache, location);
         }
         if (location == 0)
         {
            UIM_MSG_ERR_0("Location cannot be zero.");
            ret = PBM_ERROR_NOT_FOUND;
            break;
         }
         if ((cache->pb_cache_array == NULL) ||
             (cache->pb_cache_array[location] == NULL) ||
             (cache->pb_cache_array[location]->fields == NULL) ||
             (cache->pb_cache_array[location]->num_fields == 0))
         {
            ret = PBM_ERROR_NOT_FOUND;
            break;
         }
      } while (0);
      if (ret == PBM_SUCCESS)
      {
         break;
      }
      g_search_enumerator++;
   }
   if (g_search_enumerator >= g_search_length)
   {
      *rec_id_ptr = PBM_RECID_NULL;
      ret = PBM_EOF;
      goto END_FUNC_ENUM_NEXT_REC_ID;
   }
   g_search_enumerator++;
   ret = PBM_SUCCESS;
   //Area of improvement:  Double check our search result, and make sure it
   //still matches the search criteria (or probably do it in record write?)
   END_FUNC_ENUM_NEXT_REC_ID:
   rex_leave_crit_sect(&pbm_crit_sect);
   return ret;
}


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_REC_INIT_EXT

DESCRIPTION
Initializes searches of the address book for specific records

Does search to set up for subsequent calls to pbm_session_enum_next_rec_ext(), which
iterates through the records that match the given criteria.

Searching rules are as follows:
1)  Only records with the given category are matched.  If PBM_CAT_NONE is
    specified, then all categories are matched.
2)  Only records that contain at least one instance of a field with the
    given field ID are matched.  If PBM_FIELD_NONE is specified, then
    this criterion is ignored.
3)  Only records with data in the specified field that "matches" the data
    specified by data_ptr are matched.  If data_ptr is NULL, then this
    criterion is ignored.

  What "matches" means depends on the setting of flags:
          Default (0x0) is case insensitive, starts with, string match
          with FIELD_NONE.
          Bit 0:    0   - PBM_CASE_INSENSITIVE
          Bit 0:    1   - PBM_CASE_SENSITIVE
          Bit 1-2:  0   - PBM_SEARCHTYPE_STARTSWITH
          Bit 1-2:  1   - PBM_SEARCHTYPE_EXACT
          Bit 1-2:  10  - PBM_SEARCHTYPE_CONTAINS
          Bit 3:    0   - PBM_DATATYPE_STR
          Bit 3:    1   - PBM_DATATYPE_BYTE
          Bit 4:    0   - PBM_FIELDID_NONE
          Bit 4:    1   - PBM_FIELDID_PHONE (Any type of phone)
          Bit 5:    0   - PBM_CALLHISTORY_NONE
          Bit 5:    1   - PBM_CALLHISTORY_ALL (incoming, outgoing, missed)
          Bit 6:    0   - PBM_ENUMERATE_ALL
          Bit 6:    1   - PBM_ENUMERATE_AVAILABLE
          Bit 7:    0   - PBM_MATCHING_NORMAL
          Bit 7:    1   - PBM_MATCHING_GREATER (Not supported with SEARCHTYPE_CONTAINS).
          Bit 8:    0   - PBM_SORT_LASTNAME_FIRST
          Bit 8:    1   - PBM_SORT_LASTNAME_SECOND

  Example:  pbm_session_enum_rec_init_ext(pb_id, PBM_CAT_NONE, PBM_FIELD_NONE, "joh", 3,
                         PBM_CASE_INSENSITIVE | PBM_SEARCHTYPE_STARTSWITH, &handle)
            would match on the names "John Smith" and "Johann Bach"

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_rec_init_ext(pbm_phonebook_type pb_id,
                                  uint16 category,
                                  pbm_field_id_e_type field_id,
                                  const uint8 *data_ptr,
                                  uint16 data_size,
                                  uint32 flags,
                                  uint8 *handle)
{
   pbm_return_type              ret = PBM_SUCCESS;
   PB_CACHE                     *cache;
   pbm_session_enum_type        session_type = PBM_SESSION_DEFAULT;
   pbm_record_id_type           *temp_results = NULL; /* Will point to array of record IDs */
   int                          result_index;
   int                          size = 0;
   int                          num_caches = 0;
   int                          caches_used = 0;
   int                          i, handle_iterator;
   int                          cidx;
   pbm_cat_e_type               my_cat = (pbm_cat_e_type)category;
   pbm_internal_field_info      *field_info_ptr = NULL;

   boolean avail_flag = (((flags & PBM_ENUMERATE_MASK) ==
                          PBM_ENUMERATE_AVAILABLE) ? TRUE : FALSE);

   PB_CACHE *pb_cache_list[PBM_RESERVED] = {NULL,}; /* Cache list. */
   pbm_device_type *pb_ids = NULL;                  /* Phonebook list */

   PB_CACHE *pb_cache = NULL;    /* Used when list is one big. */
   pbm_device_type ch_pb_ids[] = {PBM_DIALED, PBM_RCVD, PBM_MISS};

   /* First make sure the category is valid for the search. */
   if (pb_id.device_type != PBM_ECC_OTHER &&
          my_cat != PBM_CAT_NONE && pb_id.device_type != pbm_cat_to_phonebook(my_cat))
   {
      UIM_MSG_ERR_2("Category %d and phonebook %d does not match",
                  my_cat, pb_id.device_type);
      return PBM_ERROR_CAT_NOT_SUPPORTED;
   }

   PBM_MEM_ALLOC( field_info_ptr,sizeof(pbm_internal_field_info));
   if (NULL == field_info_ptr)
   {
     UIM_MSG_ERR_0("malloc failed for field_info_ptr");
     return PBM_ERROR;
   }

   if(pbm_get_internal_field_info(pb_id,field_info_ptr) == PBM_SUCCESS)
   {
     /* Now if they are searching for a field, make sure they can. */
     if (field_id != PBM_FIELD_NONE)
     {
        for (i = 0; i < field_info_ptr->num_fields; i++)
        {
           if (field_id == field_info_ptr->info[i].field_id)
           {
              if (((my_cat == PBM_CAT_NONE) && !field_info_ptr->info[i].field_search) ||
                  ((my_cat != PBM_CAT_NONE) && !field_info_ptr->info[i].field_cat_search))
              {
                 PBM_MEM_FREEIF (field_info_ptr);
                 return PBM_ERROR_NOT_SUPPORTED;
              }
              break;
           }
        }
     }
   }
   else
   {
      PBM_MEM_FREEIF (field_info_ptr);
      return PBM_ERROR;
   }

   PBM_MEM_FREEIF (field_info_ptr);
   /* select the handle for the current client */
   rex_enter_crit_sect(&pbm_crit_sect);
   for(handle_iterator = 0; handle_iterator < PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT; handle_iterator++)
   {
      if(g_pbm_session_rec_init_handles[handle_iterator] == PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE)
      {
         /* this handle has to be used by clients while calling the enum_next_rec_id */
         *handle = handle_iterator;
         g_pbm_session_rec_init_handles[handle_iterator] = PBM_ENUM_REC_INIT_HANDLE_IN_USE;
         break;
      }
   }
   if(handle_iterator == PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT )
   {
      /* return error in this case as there is no free handle */
      *handle = 0xff;
      ret = PBM_ERROR;
      goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
   }
   PBM_MEM_FREEIF(g_search_results_ext[handle_iterator]);

   /* When all call history phonebooks are enumerated, pb_id
    * could be PBM_DEFAULT */
   if (pb_id.device_type == PBM_DEFAULT)
   {
      if ((flags & PBM_CALLHISTORY_MASK) == PBM_CALLHISTORY_ALL)
      {
         pb_ids = ch_pb_ids;
         num_caches = ARR_SIZE(ch_pb_ids);
      } else
      {
         pb_ids = pbm_default_pb_ids;
         num_caches = ARR_SIZE(pbm_default_pb_ids);
      }
      /* Check if the caches are ready.
       * For PBM_DEFAULT, no SIM and no SIM file should not be returned as an
       * error */
      for (cidx = 0; cidx < num_caches; ++cidx)
      {/* case for all EFs in pbm_default_pb_ids */
         switch (pb_ids[cidx])
         {
            case PBM_MISS:
            case PBM_RCVD:
            case PBM_DIALED:
            case PBM_EFS:
            case PBM_SDL:
               //session_type is optional since for computing above phonebook's caches it is not required
               if ( NULL != (cache = pbm_device_id_to_cache(session_type, pb_ids[cidx])))
               {/* check the number of records for files in EFS */
                  if (cache->status == PBM_STATUS_READY)
                  {
                     pb_cache_list[caches_used++] = cache;
                  } else
                  {
                     UIM_MSG_ERR_2("Cache for PB %d not available %d", (pb_ids?pb_ids[cidx]:pb_id.device_type), ret);
                     if (!avail_flag &&
                         ret != PBM_ERROR_NO_SIM && ret != PBM_ERROR_NOT_AVAILABLE && ret != PBM_ERROR_RESTRICTED)
                     {
                        g_pbm_session_rec_init_handles[handle_iterator]=PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE; /* as error is returned handle is not in use */
                        *handle = 0xff;
                        goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
                     }
                  }
               }
               break;
            case PBM_ADN:
            case PBM_SDN:
            case PBM_FDN:
            default:
               /* Follow app preference for traversing all EFs present under ADF DIR */
               for (session_type = PBM_SESSION_DEFAULT; session_type < PBM_SESSION_MAX; session_type++)
               {
                  if ( NULL != (cache = pbm_device_id_to_cache(session_type, pb_ids[cidx])))
                  {/* check the number of records for files in EFS */
                     if (cache->status == PBM_STATUS_READY)
                     {
                        pb_cache_list[caches_used++] = cache;
                     }
                     else
                     {
                        UIM_MSG_ERR_2("Cache for PB %d not available %d", ((pb_ids)?pb_ids[cidx]:pb_id.device_type), ret);
                        //(ret != PBM_ERROR_RESTRICTED)check is to skip if particular cache have restricted access CR146463
                        //ex - some SIMs may restrict access to ADN records when FDN is disabled
                        if (!avail_flag &&
                            ret != PBM_ERROR_NO_SIM && ret != PBM_ERROR_NOT_AVAILABLE && ret != PBM_ERROR_RESTRICTED)
                        {
                           g_pbm_session_rec_init_handles[handle_iterator]=PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE; /* as error is returned handle is not in use */
                           *handle = 0xff;
                           goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
                        }
                     }
                  }
               }
               break;
         }
      }
   } /*  if (pb_id.device_type == PBM_DEFAULT)... */
   //process all the ECC caches
   else if (pb_id.device_type == PBM_ECC)
   {
      num_caches = ARR_SIZE(pbm_ecc_pb_ids);
      for (cidx = 0; cidx < num_caches; ++cidx)
      {
         switch (pbm_ecc_pb_ids[cidx])
         {
            case PBM_ECC_OTHER:
               //session_type is optional since for computing above phonebook's caches it is not required
               if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[cidx])))
               {/* check the number of records for files in EFS */
                  if (cache->status == PBM_STATUS_READY)
                     pb_cache_list[caches_used++] = cache;
               }
               break;
            case PBM_ECC:
            default:
               /* Follow app preference for traversing all EFs present under ADF DIR */
               session_type = pbm_mapping_to_session(pb_id.pb_category,(pbm_slot_type) pb_id.slot_id, pb_id.prov_type);
               {
                  if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[cidx])))
                  {/* check the number of records for files in EFS */
                     if (cache->status == PBM_STATUS_READY)
                        pb_cache_list[caches_used++] = cache;
                  }
               }
               break;
         }
      }
   } else
   {
      /* Check if the cache is ready */
      if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS)
      {
         g_pbm_session_rec_init_handles[handle_iterator]=PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE; /* as error is returned handle is not in use */
         *handle = 0xff;
         goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
      }

      pb_cache_list[0] = pbm_pb_id_to_cache(pb_id);
      caches_used = 1;
   }


   /* Allocate the maximum number of possible results. */
   for (cidx = 0; cidx < caches_used; ++cidx)
   {
      size += pb_cache_list[cidx]->records_in_use;
   }

   if (size == 0)
   {
      /* as we are returning success here in this condition handle is still in use */
      g_search_length_ext[handle_iterator] = 0;
      g_search_enumerator_ext[handle_iterator] = 0;
      g_pbm_session_rec_init_handles[handle_iterator] = PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE;
      UIM_MSG_HIGH_1("PBM pbm_session_enum_rec_init_ext handle returned is %d, number of records matched = 0", handle_iterator);
      ret = PBM_ERROR_NOT_FOUND;
      goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
   }

   /* Allocate srch results array*/
   PBM_MEM_ALLOC(temp_results,(uint32)size * sizeof(pbm_record_id_type));

   if (temp_results == NULL)
   {
      g_pbm_session_rec_init_handles[handle_iterator] = PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE; /* in case of error returned handle should be free */
      g_search_length_ext[handle_iterator] = 0;
      g_search_enumerator_ext[handle_iterator] = 0;
      UIM_MSG_HIGH_1("PBM pbm_session_enum_rec_init_ext handle returned is %d, number of records matched = 0, temp_results not allocated memory",
          handle_iterator);
      ret = PBM_ERROR;
      goto END_FUNC_ENUM_REC_INIT_LEAVE_CS;
   }

   result_index = 0;

   /* All call history phonebooks */
   /* pb_id could be PBM_DEFAULT or any one of the call history phonebooks */
   for (cidx = 0; cidx < caches_used; ++cidx)
   {
      pb_cache = pb_cache_list[cidx];

      if (!pb_cache)
      {
         UIM_MSG_ERR_1("Cache was NULL for index %d", cidx);
         continue;
      }

      for ( i = 1; i <= pb_cache->num_of_records; i++ )
      {
         int index;

         if (pb_cache->circular)
            index = PBM_CIRC_CACHE_NTHLOC(pb_cache, i);
         else
            index = i;

         if ( (pb_cache->pb_cache_array) && (pb_cache->pb_cache_array[index]) )
         {
            if (match_found(my_cat, field_id, data_ptr, data_size, flags,
                            pb_cache->pb_cache_array[index]))
            {
               if (result_index >= size)
               {
                  UIM_MSG_ERR_2("Allocated %d records, index %d", size, result_index);
                  break;
               }
               if (pb_cache->circular)
               {
                  //Hide the location shifting details for circular files from outside apps.
                  temp_results[result_index++] = pbm_session_location_to_record_id(pb_cache->pb_id, i);
               } else
               {
                  temp_results[result_index++] = pb_cache->pb_cache_array[index]->record_id;
               }
            }
         }
      }
   }
   /* Allocate g_search_results[] and copy results in*/
   do
   {
      if (result_index == 0)
      {
         g_search_results_ext[handle_iterator] = NULL;
         g_search_length_ext[handle_iterator] = 0;
      } else
      {
         PBM_MEM_ALLOC(g_search_results_ext[handle_iterator], (uint32)result_index * sizeof(g_search_results_ext[0][0]));
         PBM_CHECK_PTR_BREAK(g_search_results_ext[handle_iterator], ret, PBM_ERROR_MEM_FULL);

         /* g_search_results is a 0-based array*/
         for (i=0; i<result_index; i++)
         {
            g_search_results_ext[handle_iterator][i] = temp_results[i];
         }

         g_search_length_ext[handle_iterator] = (int) result_index;
      }
   } while ( 0 ); /*lint !e717 */

   /* clean up the temp results */
   PBM_MEM_FREEIF(temp_results);

   /* Initialize enumerator for this client handle*/
   g_search_enumerator_ext[handle_iterator] = 0;
   ret = PBM_SUCCESS;
   UIM_MSG_HIGH_2("PBM pbm_session_enum_rec_init_ext handle returned is %d, number of records matched = %d",
       handle_iterator , g_search_length_ext[handle_iterator]);
END_FUNC_ENUM_REC_INIT_LEAVE_CS:
   rex_leave_crit_sect(&pbm_crit_sect);
   return ret;
}


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_REC_INIT_EXT_FREE_HANDLE

DESCRIPTION
  To be called by clients in order to free the handle that has been allocated to
  the client via pbm_session_enum_rec_init_ext.

  This API is to be used in case all the records are not read via
  pbm_session_enum_next_rec_id_ext. If all the records are read then handle
  gets free automatically.
  Handle wil be marked not in use and memory allocated to array containing the
  record ids will be deallocated.

DEPENDENCIES
pbm_session_enum_rec_init_ext() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_rec_init_ext_free_handle(
  uint8 handle
)
{
  if (handle >= PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT)
  {
    return PBM_ERROR ;
  }

  rex_enter_crit_sect(&pbm_crit_sect);

  PBM_MEM_FREEIF(g_search_results_ext[handle]);
  g_pbm_session_rec_init_handles[handle] = PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE;

  rex_leave_crit_sect(&pbm_crit_sect);

  UIM_MSG_HIGH_1("PBM pbm_session_enum_rec_init_ext_free_handle called for handle 0x%x", handle);

  return PBM_SUCCESS;
} /* pbm_session_enum_rec_init_ext_free_handle */


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_NEXT_REC_ID_EXT

DESCRIPTION
  Retrieves the next record ID that matches the search criteria
  for the client handle returned to client via pbm_session_enum_rec_init_ext() .

  When the end of the enumeration is reached, the index is not reset to the
  beginning of the enumeration.Once all the records are read this API will return
  PBM_EOF Client should not call this API after recieving PBM_EOF as it may return
  stale data after that. The caller must call pbm_session_enum_rec_init_ext() to
  reinitialize the search criteria...

DEPENDENCIES
pbm_session_enum_rec_init_ext() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_next_rec_id_ext(
  pbm_record_id_type *rec_id_ptr,
  uint8               handle
)
{
  if (handle >= PBM_MAX_NUM_CLIENTS_FOR_ENUM_REC_INIT)
  {
    return PBM_ERROR;
  }
  else
  {
    pbm_return_type              ret      = PBM_ERROR;
    PB_CACHE                    *cache    = NULL;
    uint16                       location = 0;
    pbm_phonebook_type           pb_id    = {PBM_GPB};

    PBM_CHECK_PTR_RET(rec_id_ptr, PBM_ERROR);

    rex_enter_crit_sect(&pbm_crit_sect);

    if (g_search_enumerator_ext[handle] >= g_search_length_ext[handle])
    {
      /* once we are here all the reords re being read.the handle must be freed by clients explicitly */
      *rec_id_ptr = PBM_RECID_NULL;
      UIM_MSG_HIGH_1("PBM pbm_session_enum_next_rec_id_ext called for handle %d, all records already read.", handle);
      ret = PBM_EOF;
      goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
    }

    if (g_pbm_session_rec_init_handles[handle] == PBM_ENUM_REC_INIT_HANDLE_NOT_IN_USE)
    {
      /* in case if the handle has been freed explicitly by the client */
      *rec_id_ptr = PBM_RECID_NULL;
      UIM_MSG_HIGH_1("PBM pbm_session_enum_next_rec_id_ext called for handle %d, handle not in use returning PBM_EOF.", handle);
      ret = PBM_EOF;
      goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
    }

    /* Empty list?*/
    if (!g_search_results_ext[handle])  /* need to have return PBM_EOF check before this statement */
    {
      *rec_id_ptr = PBM_RECID_NULL;
      ret = PBM_ERROR_NOT_FOUND;
      goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
    }

    /* Skip over blank entries*/
    /* (Blank because they have been deleted from the cache)*/
    while (g_search_enumerator_ext[handle] < g_search_length_ext[handle])
    {
      *rec_id_ptr = g_search_results_ext[handle][g_search_enumerator_ext[handle]];

      /* see if the entry is blank */
      pb_id = pbm_session_record_id_to_phonebook_type(*rec_id_ptr);

      if((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
      {
        *rec_id_ptr = PBM_RECID_NULL;
        ret = PBM_ERROR_NOT_FOUND;
        goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
      }

      do
      {
        if((ret = check_pb_ready(pb_id)) != PBM_SUCCESS &&
            ret != PBM_ERROR_SYNC)
        {
          break;
        }

        location = pbm_session_record_id_to_location(*rec_id_ptr);
        if (location > cache->num_of_records)
        {
          UIM_MSG_ERR_0("Location is higher than length of cache.");
          ret = PBM_ERROR_INDEX_INVALID;
          break;
        }

        if (cache->circular == TRUE)
        {
          /* For circular buffers we need to so some special translation for location */
          location = PBM_CIRC_CACHE_NTHLOC(cache, location);
        }
        if (location == 0)
        {
          UIM_MSG_ERR_0("Location cannot be zero.");
          ret = PBM_ERROR_NOT_FOUND;
          break;
        }
        if((cache->pb_cache_array == NULL) ||
           (cache->pb_cache_array[location] == NULL) ||
           (cache->pb_cache_array[location]->fields == NULL) ||
           (cache->pb_cache_array[location]->num_fields == 0))
        {
          ret = PBM_ERROR_NOT_FOUND;
          break;
        }
      } while (0);
      if (ret == PBM_SUCCESS)
      {
        break;
      }
      g_search_enumerator_ext[handle]++;
    }
    if (g_search_enumerator_ext[handle] == g_search_length_ext[handle])  /* for case when ret != success for all the traversed records  or to return the PBM_EOF for very first time*/
    {
      g_search_enumerator_ext[handle]++;
      *rec_id_ptr = PBM_RECID_NULL;
      ret = PBM_EOF;
      goto END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS;
    }
    g_search_enumerator_ext[handle]++;
    ret = PBM_SUCCESS;

    END_FUNC_ENUM_NEXT_REC_ID_LEAVE_CS:
    rex_leave_crit_sect(&pbm_crit_sect);
    return ret;
  }
}/* pbm_session_enum_next_rec_id_ext */


/*===========================================================================
FUNCTION PBM_SESSION_CLEAR_PHONEBOOK

DESCRIPTION
  Clears all entries within a given phonebook

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_clear_phonebook(pbm_phonebook_type pb_id)
{
   int i;
   pbm_record_id_type rec_id;
   pbm_return_type ret = PBM_SUCCESS;
   pbm_return_type write_retval = PBM_SUCCESS;
   PB_CACHE *cache;

   if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
   {
      UIM_MSG_ERR_1("Invalid phonebook %d", pb_id.device_type);
      return PBM_ERROR;
   }
   rex_enter_crit_sect(&pbm_crit_sect);
   if ((cache->status == PBM_STATUS_READY) && (cache->records_in_use > 0))
   {
      if (cache->circular)
      {
         /* Need to shift in empty records to clear these pb's */
         for (i = cache->num_of_records; i >= 1; i--)
         {
            rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;
            write_retval = pbm_session_record_write(pb_id, &rec_id, 0, 0, NULL, 0,
                                            NULL, NULL);
            if (write_retval != PBM_SUCCESS)
            {
               UIM_MSG_ERR_2("Failure to delete 0x%x : %d", rec_id, write_retval);
               ret = PBM_ERROR;
            }
         }
      } else
      {
         for (i = 1; i <= cache->num_of_records; i++)
         {
            if (!cache->pb_cache_array[i])
               continue;
            rec_id = pbm_session_location_to_record_id(pb_id, i);
            write_retval = pbm_session_record_write(pb_id, &rec_id, 0, 0, NULL, 0,
                                            NULL, NULL);
            if (write_retval != PBM_SUCCESS)
            {
               UIM_MSG_ERR_2("Failure to delete 0x%x:%d", rec_id, write_retval);
               ret = PBM_ERROR;
            }
         }
      }
   }
   rex_leave_crit_sect(&pbm_crit_sect);
   return ret;
}



/*===========================================================================*
 *              PHONE BOOK MANAGER LEGACY BASED API                          *
 *                                                                           *
 * These functions supply the API for the legacy based requests.             *
 *===========================================================================*/

static pbm_return_type pbm_get_fileinfo_internal(pbm_phonebook_type pb_id,
                                                 pbm_extended_fileinfo_s_type *info)
{
   PB_CACHE *cache;
   uint32 i;
   pbm_session_enum_type session_type = PBM_SESSION_DEFAULT;

   int j;

   PBM_CHECK_PTR_RET(info, PBM_ERROR);
   rex_enter_crit_sect(&pbm_crit_sect);

   memset(info,0,sizeof(pbm_extended_fileinfo_s_type));


   if (pb_id.device_type == PBM_DEFAULT)
   {
      for (i = 0; i < ARR_SIZE(pbm_default_pb_ids); ++i)
      {
         /* case for all EFs in pbm_default_pb_ids */
         switch (pbm_default_pb_ids[i])
         {
            case PBM_EFS:
            case PBM_SDL:
               /* check the number of records for EFs in EFS */
               //session_type is optional since for PBM_ECC_OTHER cache it is not required
               if ( NULL == (cache = pbm_device_id_to_cache(session_type, pbm_default_pb_ids[i])))
               {
                  rex_leave_crit_sect(&pbm_crit_sect);
                  return PBM_ERROR;
               }
               if (cache->status == PBM_STATUS_READY)
               {
                  info->records_used +=  cache->records_in_use;
                  info->number_of_records +=  cache->num_of_records;
                  info->text_len = MAX(cache->text_length[0], info->text_len);
                  info->number_len = PBM_MAX_NUM_LENGTH;  /* EFS max num len is always longest here */
               }
               break;

            case PBM_ADN:
            case PBM_FDN:
            case PBM_SDN:
            default:
               /* Follow app preference for traversing all EFs present under ADF DIR */
               for (session_type = PBM_SESSION_DEFAULT; session_type < PBM_SESSION_MAX; session_type++)
               {
                  /* check the number of records for EFs in EFS */
                  if ( NULL == (cache = pbm_device_id_to_cache(session_type, pbm_default_pb_ids[i])))
                  {
                     rex_leave_crit_sect(&pbm_crit_sect);
                     return PBM_ERROR;
                  }
                  if (cache->status == PBM_STATUS_READY)
                  {
                     info->records_used +=  cache->records_in_use;
                     info->number_of_records +=  cache->num_of_records;
                     info->text_len = MAX(cache->text_length[0], info->text_len);
                     info->number_len = PBM_MAX_NUM_LENGTH;  /* EFS max num len is always longest here */
                  }
               }
               break;
         }
      }
   }
   //process all the ECC caches
   else if (pb_id.device_type == PBM_ECC)
   {
      for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids); ++i)
      {
         /* case for all EFs in pbm_default_pb_ids */
         switch (pbm_ecc_pb_ids[i])
         {
            case PBM_ECC_OTHER:
               /* check the number of records for EFs in EFS */
               if ( NULL != (cache = pbm_device_id_to_cache(session_type, PBM_ECC_OTHER)))
               {
                  if (cache->status == PBM_STATUS_READY)
                  {
                     info->records_used +=  cache->records_in_use;
                     info->number_of_records +=  cache->num_of_records;
                     info->text_len = MAX(cache->text_length[0], info->text_len);
                     info->number_len = PBM_MAX_NUM_LENGTH;  /* EFS max num len is always longest here */
                  }
               }
               break;
            case PBM_ECC:
            default:
               /* Follow app preference for traversing all EFs present under ADF DIR */
               session_type = pbm_mapping_to_session (pb_id.pb_category,(pbm_slot_type) pb_id.slot_id, pb_id.prov_type);
               {
                  /* check the number of records for EFs in EFS */
                  if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[i])))
                  {
                     if (cache->status == PBM_STATUS_READY)
                     {
                        info->records_used +=  cache->records_in_use;
                        info->number_of_records +=  cache->num_of_records;
                        info->text_len = MAX(cache->text_length[0], info->text_len);
                        info->number_len = PBM_MAX_NUM_LENGTH;  /* EFS max num len is always longest here */
                     }
                  }
               }
               break;
         }
      }
   } else
   {
      if ( NULL == (cache = pbm_pb_id_to_cache(pb_id)))
      {
         rex_leave_crit_sect(&pbm_crit_sect);
         return PBM_ERROR;
      }
      if (cache->status != PBM_STATUS_READY)
      {
         rex_leave_crit_sect(&pbm_crit_sect);
         return check_pb_ready(pb_id);
      }
      session_type = pbm_mapping_to_session (pb_id.pb_category,(pbm_slot_type) pb_id.slot_id, pb_id.prov_type);
      info->number_of_records =  cache->num_of_records;
      info->text_len          = cache->text_length[0];
      info->records_used      = cache->records_in_use;


      if (pbm_action_requires_sim(pb_id))
      {
         /* assume extension records not available */
         info->number_len = PBM_MAX_NUM_LENGTH_NO_EXT_RECORD;

         /* see if we have extension records available */
         if (cache->extension_cache != NULL)
         {
            /* Now search through the extension records to see if there is
             * any available.
             */
            for (j = cache->extension_cache->num_of_rec; j > 0; j--)
            {
               if (cache->extension_cache->in_use_array[j].index == 0)
               {
                  /* there's at least one available */
                  info->number_len = PBM_MAX_NUM_LENGTH;
                  break;
               }
            }
         }
         /* add the write protection related status data here */
         /* by default the write status should be send as allowed in case when srite status is NULL and not available to PBM */
         info->pb_write_status.write_status = PBM_WRITE_STATUS_ALLOWED;
         if(cache->write.num_protection_pin != 0) /* some PIN protection is in place */
         {
           /* re initialize write status with not allowed */
           info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_ALLOWED;
           		    info->pb_write_status.pin_protection_mask = cache->write.pin_protection_mask;
                  info->pb_write_status.protection_method = cache->write.protection_method;

                  /* write status would depend upon the pin protection mask and pin's verification status */
                  /* search the bits one by one and then check if the PIN corresponding to that is verified or not */

                  if(info->pb_write_status.protection_method == MMGSDI_ALWAYS_ALLOWED)
                  {
                    info->pb_write_status.write_status = PBM_WRITE_STATUS_ALLOWED;
                  }
                  else if(info->pb_write_status.protection_method == MMGSDI_NEVER_ALLOWED)
                  {
                    info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_ALLOWED;
                  }
                  else if(info->pb_write_status.protection_method == MMGSDI_AND_ALLOWED)
                  {
                    uint32 flag = 0; /* flag to check if all the PINs are verified, in the end of for loop,
                                     this flag and pin_protection_mask should match for success case */
                    /* check all pins and their status even if one is not verified mark status as not allowed */
                    				for(i=0; i<(uint32)(PBM_BITS_IN_PIN_PROTECTION_MASK); i++)
                            {
                              if(PBM_IS_BIT_SET(cache->write.pin_protection_mask, i))
                              {
                                switch(i)
                                {
                                case PBM_PIN_PIN1:
                                  if(pbm_pin1_verified.value & PBM_SESSIONID_TO_SESSION_MASK(session_type))
                                  {
                                    flag |= (1 << PBM_PIN_PIN1);
                                  }
                                  break;
                                case PBM_PIN_PIN2:
                                  if(pbm_pin2_verified.value & PBM_SESSIONID_TO_SESSION_MASK(session_type))

                                  {
                                    flag |= (1 << PBM_PIN_PIN2);
                                  }
                                  break;
                                case PBM_PIN_ADM:
                                  /* ADM protected PB can not be written upon */
                                  info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_ALLOWED;
                                  break;
                                default:
                                  UIM_MSG_ERR_0("Wrong PIN type being checked");
                                  info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_KNOWN;
                                  break;
                                }
                                /* here we wont break from for loop, since we need to check all PINs, except for ADM case */
                                if(info->pb_write_status.write_status == PBM_WRITE_STATUS_NOT_KNOWN)
                                {
                                  break;
                                }
                              }
                            }
                            /* here check the status of the flag, if flag matches the pin_protection_mask, all the PINs are verified */
                            if(info->pb_write_status.write_status != PBM_WRITE_STATUS_NOT_KNOWN)
                            {
                              if(flag == cache->write.pin_protection_mask)

                              {
                                info->pb_write_status.write_status = PBM_WRITE_STATUS_ALLOWED;
                              }
                              else
                              {
                                info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_ALLOWED;
                              }
                            }
                  }
                  else if(info->pb_write_status.protection_method == MMGSDI_OR_ALLOWED)
                  {
                    uint32 flag = 0; /* flag to check if any of the PINs in mask is verified, in the end of for loop,
                                     and operation of flag and pin_protection_mask would decide the write status */
                    /* check all pins and their status even if one is verified mark status as allowed */
                    for(i=0; i<(uint32)(PBM_BITS_IN_PIN_PROTECTION_MASK); i++)
                    {
                      if(PBM_IS_BIT_SET(cache->write.pin_protection_mask, i))
                      {
                        switch(i)
                        {
                        case PBM_PIN_PIN1:
                          if(pbm_pin1_verified.value & PBM_SESSIONID_TO_SESSION_MASK(session_type))
                          {
                            flag |= (1 << PBM_PIN_PIN1);
                          }
                          break;
                        case PBM_PIN_PIN2:
                          if(pbm_pin2_verified.value & PBM_SESSIONID_TO_SESSION_MASK(session_type))
                          {
                            flag |= (1 << PBM_PIN_PIN2);
                          }
                          break;
                        case PBM_PIN_ADM:
                          /* since only ADM PIN protected phonebook can never be written upon
                          no need to update flag here, as one of pin1 or pin2 is required */
                          break;
                        default:
                          UIM_MSG_ERR_0("Wrong PIN type being checked");
                          info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_KNOWN;
                          break;
                        }
                        /* here we wont break from for loop, since we need to check all PINs, except for ADM case */
                        if(info->pb_write_status.write_status == PBM_WRITE_STATUS_NOT_KNOWN)
                        {
                          break;
                        }
                      }
                    }
                    /* here check the and of flag and pin_protection_mask, even if one of the bit matches then phonebook can be written upon*/
                    if(info->pb_write_status.write_status != PBM_WRITE_STATUS_NOT_KNOWN)
                    {
                      if(flag)
                      {
                        info->pb_write_status.write_status = PBM_WRITE_STATUS_ALLOWED;
                      }
                      else
                      {
                        info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_ALLOWED;
                      }
                    }
                  }
                  else if(info->pb_write_status.protection_method == MMGSDI_SINGLE_ALLOWED)
                  {
                    /* find out which PIN is being used and then check its status */
                    for(i=0; i < (uint32)(PBM_BITS_IN_PIN_PROTECTION_MASK); i++)
                    {
                      if(PBM_IS_BIT_SET(cache->write.pin_protection_mask, i))
                      {
                        switch(i)
                        {
                        case PBM_PIN_PIN1:
                          if(pbm_pin1_verified.value & PBM_SESSIONID_TO_SESSION_MASK(session_type))
                          {
                            info->pb_write_status.write_status = PBM_WRITE_STATUS_ALLOWED;
                          }
                          else
                          {
                            info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_ALLOWED;
                          }
                          break;
                        case PBM_PIN_PIN2:

                          if(pbm_pin2_verified.value & PBM_SESSIONID_TO_SESSION_MASK(session_type))
                          {
                            info->pb_write_status.write_status = PBM_WRITE_STATUS_ALLOWED;
                          }
                          else
                          {
                            info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_ALLOWED;
                          }
                          break;
                        case PBM_PIN_ADM:
                          /* ADM can never be modified */
                          info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_ALLOWED;
                          break;
                        default:
                          UIM_MSG_ERR_0("Wrong PIN type being checked");
                          info->pb_write_status.write_status = PBM_WRITE_STATUS_NOT_KNOWN;
                          break;
                        }
                        break; /* break from for loop */
                      }
                    }
                  }
                  //info->pb_write_status.write_status = PBM_WRITE_STATUS_ALLOWED;
     }
      } else
      {
         /* EFS phonebook */
         info->number_len = PBM_MAX_NUM_LENGTH;  /* EFS max num len */
      }
   }

   rex_leave_crit_sect(&pbm_crit_sect);
   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_GET_PHONEBOOK_USIM_FIELDS_CAPIBILITIES

DESCRIPTION
  It will return the USIM field capibilities like mapping type of file, number of records,
  records_in_use  and record length.

  The requested data is returned in the pointer information synchronously.

DEPENDENCIES
  ADN cache should be ready.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_phonebook_usim_fields_capibilities (pbm_phonebook_type pb_id,
                                               pbm_all_usim_field_capabilities_s_type *info)
{
   PB_CACHE                                 *cache;
   PB_MASTER_USIM_CACHE                     *m_usim_cache = NULL;
   PB_USIM_CACHE                            *usim_cache = NULL;
   uint32                                   i;
   int                                      j, no_usim_fields = 0;
   pbm_return_type                          ret = PBM_SUCCESS;

   PBM_CHECK_PTR_RET(info, PBM_ERROR);
   if(pb_id.device_type != PBM_ADN)
   {
      /* return error */
	  return PBM_ERROR;
   }
   memset(info,0,sizeof(pbm_all_usim_field_capabilities_s_type)); /* memset passed strucure to 0 */
   rex_enter_crit_sect(&pbm_crit_sect); /* since caches are being accessed use critical section approach */
   if ( NULL == (cache = pbm_pb_id_to_cache(pb_id)))
   {
     ret = PBM_ERROR ;
     goto  END_FUNC;
   }
   if (cache->status != PBM_STATUS_READY)
   {
     ret =  check_pb_ready(pb_id);
     goto  END_FUNC;
   }
   info->total_pbsets = 0;
   /* fill the passed info structure here */
   for(i=0; i < PBM_MAX_NUM_PB_SETS; i++)
   {
     if(cache->records_in_pb_set[i] != 0)
     {
       info->total_pbsets++;
     }
     no_usim_fields = 0; /* re init for next pb set */
     m_usim_cache = cache->m_usim_cache[i];
     if(m_usim_cache)
     {
       /* fill the information present for various USIM fields present in this PB set */
       if(m_usim_cache->usim_cache_array)
       {
         for(j=1; j <= m_usim_cache->num_caches; j++)
         {
           usim_cache = m_usim_cache->usim_cache_array[j];
           if(!usim_cache)
           {
             continue;
           }
           else
           {
             info->pbm_all_usim_capibilities[i][no_usim_fields].field_id = pbm_file_id_to_field_id(usim_cache->file_id);
             info->pbm_all_usim_capibilities[i][no_usim_fields].mapping_type = usim_cache->mapping_type;
             info->pbm_all_usim_capibilities[i][no_usim_fields].num_of_records = usim_cache->num_of_records;
             info->pbm_all_usim_capibilities[i][no_usim_fields].records_in_use = usim_cache->records_in_use;
             info->pbm_all_usim_capibilities[i][no_usim_fields].record_length = usim_cache->record_length;
             no_usim_fields++;
           }
         }
       }
       else
       {
         continue; /* go to next PB set */
       }
     }
   }
   END_FUNC: 
   rex_leave_crit_sect(&pbm_crit_sect);
   return ret;
}

/*===========================================================================
FUNCTION PBM_GET_ADN_PB_SET_CAPIBILITIES

DESCRIPTION
  It will return the ADN capabilities per phonebook set 

  The requested data is returned in the pointer information synchronously.

DEPENDENCIES
  ADN cache should be ready.

SIDE EFFECTS
===========================================================================*/
pbm_return_type  pbm_get_adn_pbset_capibilities (
  pbm_phonebook_type                pb_id,
  pbm_adn_capabilites_pbset_s_type *info
 )
{
   PB_CACHE                   *cache        = NULL;
   uint32                      i            = 0;
   uint32                      index        = 0;
   uint32                      start        = 0;
   pbm_return_type             ret          = PBM_SUCCESS;
   pbm_session_enum_type       session_type = PBM_SESSION_DEFAULT;

   PBM_CHECK_PTR_RET(info, PBM_ERROR);

   if(pb_id.device_type != PBM_ADN)
   {
     return PBM_ERROR;
   }

   session_type = pbm_mapping_to_session(pb_id.pb_category,
                                         (pbm_slot_type)pb_id.slot_id,
                                         pb_id.prov_type);
   PBM_VALIDATE_SESSION_RET(session_type,PBM_ERROR);

   memset(info, 0, sizeof(pbm_adn_capabilites_pbset_s_type));

   rex_enter_crit_sect(&pbm_crit_sect);

   if(NULL == (cache = pbm_pb_id_to_cache(pb_id)))
   {
     ret = PBM_ERROR;
     goto  END_FUNC;
   }

   /* check the cache status if it is ready */
   if (cache->status != PBM_STATUS_READY)
   {
     ret = check_pb_ready(pb_id);
     goto  END_FUNC;
   }

   for(i = 0; i < PBM_MAX_NUM_PB_SETS; i++)
   {
     if(cache->records_in_pb_set[i] != 0)
     {
       pbm_g_other_field_info_s_type    *field_info = NULL;
       uint8                             k          = 0;

       info->num_pb_sets++;
       info->adn_caps[i].max_adn_records = cache->records_in_pb_set[i];
       info->adn_caps[i].max_text_len = cache->text_length[i];
       field_info = &g_sim_field_info[session_type][0];

       for(k = 0; k < field_info->num_fields; k++)
       {
         if(field_info->info[k].field_type == PBM_FT_PHONE &&
            field_info->info[k].field_id == PBM_FIELD_PHONE_GENERIC)
         {
           info->adn_caps[i].max_num_len = field_info->info[k].max_length ;
           break;
         }
       }

       for(index = start+1; index <= (start + cache->records_in_pb_set[i]); index++ )
       {
         if(cache->pb_cache_array != NULL &&
            cache->pb_cache_array[index] != NULL &&
            cache->pb_cache_array[index]->num_fields != 0)
         {
           /* valid record which is in use currently */
           info->adn_caps[i].used_adn_records++;
         }
       }

       UIM_MSG_HIGH_3("max_adn_records 0x%x text_length 0x%x used_adn_records 0x%x",
                       info->adn_caps[i].max_adn_records,
                       info->adn_caps[i].max_text_len,
                       info->adn_caps[i].used_adn_records);

       start += cache->records_in_pb_set[i];
     }
   }

   END_FUNC:
   rex_leave_crit_sect(&pbm_crit_sect);
   return ret;
} /* pbm_get_adn_pbset_capibilities */


/*===========================================================================
FUNCTION PBM_SESSION_EXTENDED_FILE_INFO

DESCRIPTION
  Through GSDI will return the total number of records, the number of records
  in use, the text length, and the max phone number length for the specified phonebook.

  Data returned in info.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_extended_file_info (pbm_phonebook_type pb_id,
                                        pbm_extended_fileinfo_s_type *info)
{
   pbm_return_type ret;

   PBM_CHECK_PTR_RET(info, PBM_ERROR);

   ret = pbm_get_fileinfo_internal(pb_id,info);

   return ret;
}


/*===========================================================================
FUNCTION PBM_SESSION_EXTENDED_FILE_INFO_ASYNC

DESCRIPTION
  Through GSDI will return the total number of records, the number of records
  in use, the text length, and the max phone number length for the specified phonebook.
  However this API should be used only by modem clients in dual processors
  since the callback is in the same API

  Data returned in info.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_extended_file_info_async(pbm_phonebook_type pb_id,
                                        PBM_SESSION_EXTENDED_FILE_INFO_CB_FUNC proc_func)
{

   pbm_return_type ret = PBM_ERROR;
   pbm_extended_fileinfo_s_type info;

   PBM_CHECK_PTR_RET(proc_func, PBM_ERROR);

   ret = pbm_get_fileinfo_internal(pb_id,&info);
   (* proc_func)(ret,
				 pb_id,
				 &info);
   return ret;
}


/*===========================================================================
FUNCTION PBM_SESSION_GET_NUM_RECS

DESCRIPTION
Gets the total number of records in the given phone book.

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
uint16 pbm_session_get_num_recs(pbm_phonebook_type pb_id)
{
   uint16 count = 0;
   PB_CACHE *cache;
   unsigned int i;
   pbm_session_enum_type session_type = PBM_SESSION_DEFAULT;

   rex_enter_crit_sect(&pbm_crit_sect);
   if (pb_id.device_type == PBM_DEFAULT)
   {
      for (i = 0; i < ARR_SIZE(pbm_default_pb_ids); ++i)
      {
         /* case for all EFs in pbm_default_pb_ids */
         switch (pbm_default_pb_ids[i])
         {
            case PBM_EFS:
            case PBM_SDL:
               /* check the number of records for EFs in EFS */
               if ( NULL !=
                    (cache = pbm_device_id_to_cache(session_type, pbm_default_pb_ids[i])))
                  if (cache->status == PBM_STATUS_READY)
                     count += (uint16) cache->records_in_use;
               break;

            case PBM_ADN:
            case PBM_FDN:
            case PBM_SDN:
            default:
               /* Follow app preference for traversing all EFs present under ADF DIR */
               for (session_type = PBM_SESSION_DEFAULT; session_type < PBM_SESSION_MAX; session_type++)
               {
                  if ((cache = pbm_device_id_to_cache(session_type, pbm_default_pb_ids[i])) != NULL)
                     if (cache->status == PBM_STATUS_READY)
                        count += (uint16) cache->records_in_use;
               }
               break;
         }
      }
   }
   //process all the ECC caches
   else if (pb_id.device_type == PBM_ECC)
   {
      for (i = 0; i < ARR_SIZE(pbm_ecc_pb_ids); ++i)
      {
         /* case for all EFs in pbm_default_pb_ids */
         switch (pbm_ecc_pb_ids[i])
         {
            case PBM_ECC_OTHER:
               /* check the number of records for EFs in EFS */
               if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[i])))
                  if (cache->status == PBM_STATUS_READY)
                     count += (uint16) cache->records_in_use;
               break;

            case PBM_ECC:
            default:
               /* Follow app preference for traversing all EFs present under ADF DIR */
               session_type = pbm_mapping_to_session (pb_id.pb_category,(pbm_slot_type) pb_id.slot_id, pb_id.prov_type);
               {
                  if ((cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[i])) != NULL)
                     if (cache->status == PBM_STATUS_READY)
                        count += (uint16) cache->records_in_use;
               }
               break;
         }
      }
   } else
   {
      if ((cache = pbm_pb_id_to_cache(pb_id)) != NULL)
         if (cache->status == PBM_STATUS_READY)
            count = (uint16) cache->records_in_use;
   }

   rex_leave_crit_sect(&pbm_crit_sect);
   return count;
}


/*===========================================================================
FUNCTION PBM_REG_EVENT_CB

DESCRIPTION
  Allows the user to initialize the ds_ready_fn to be called during init.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_reg_event_cb ( PBM_EVENT_FUNC user_fn )
{
   PBM_CHECK_PTR_RET(user_fn, PBM_ERROR);
   ds_ready_fn = user_fn;
   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_SESSION_FIND_LOCATION

DESCRIPTION
  This function will call the function defined in proc_func for each matching
  occurance of string "number".  If no match is found PBM_EOF is returned
  otherwise each matching record is returned, until EOF is reached.

  Data returned in proc_func.  The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback.  If clients want
  to use the data after the callback terminates, they need to make a copy.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_find_location (pbm_phonebook_type pb_id,
                                   int indx,
                                   pbm_record_s_type * data)
{
   PB_CACHE *cache;
   int index = indx;
   pbm_return_type ret = PBM_ERROR;

   if (index  < 0 )
      return ret ;

   if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
      return PBM_ERROR;
   rex_enter_crit_sect(&pbm_crit_sect);
   do
   {
      if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS)
      {
         break;
      }

      /* The Phonebook cache has no entries or they asked for one beyond the
       * range of this phone book, so fail. */
      if ((cache->records_in_use == 0) || (cache->num_of_records < index))
      {
         ret = PBM_ERROR_NOT_FOUND;
         break;
      }

      if (cache->circular)
         index = PBM_CIRC_CACHE_NTHLOC(cache, indx);

      if ((cache->pb_cache_array[index] != NULL) &&
          (cache->pb_cache_array[index]->fields != NULL) &&
          (cache->pb_cache_array[index]->num_fields > 0))
      {
         if (data)
         {
            pbm_build_record_from_fields(cache->pb_cache_array[index]->fields,
                                         cache->pb_cache_array[index]->num_fields,
                                         indx, data);
            ret = PBM_SUCCESS;
         }
      }
      else
      {
         ret = PBM_ERROR_NOT_FOUND;
      }

   } while (0);
   rex_leave_crit_sect(&pbm_crit_sect);
      return ret;
               }



/*===========================================================================
FUNCTION PBM_SESSION_FIND_NUMBER

DESCRIPTION
  This function will call the function defined in proc_func for each matching
  occurance of string "number".  If no match is found PBM_EOF is returned
  otherwise each matching record is returned, until EOF is reached.

  Data returned in proc_func.  The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback.  If clients want
  to use the data after the callback terminates, they need to make a copy.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_find_number (pbm_phonebook_type pb_id,
                                 const byte *number, int loc_number_len,
                                 pbm_record_s_type * record,
                                 PBM_FIND_CB_FUNC proc_func)
{
   PB_CACHE *cache;
   int j;
   pbm_return_type ret_val = PBM_ERROR_NOT_FOUND;
   pbm_return_type pb_ready_ret_val = PBM_ERROR_NOT_FOUND;
   int records_in_use = 0;
   pbm_session_enum_type session_type = PBM_SESSION_DEFAULT;

   UIM_MSG_HIGH_4("PBM pbm_session_find_number called for pb_category ox%x, prov_type 0x%x, slot_id 0x%x device_type 0x%x",
  	           pb_id.pb_category , pb_id.prov_type, pb_id.slot_id, pb_id.device_type);
   PBM_CHECK_PTR_RET(number, PBM_ERROR);

   rex_enter_crit_sect(&pbm_crit_sect);
   if(pb_id.device_type == PBM_FDN)
   {
     session_type = pbm_mapping_to_session(pb_id.pb_category,(pbm_slot_type) pb_id.slot_id, pb_id.prov_type);
     if (USE_GSM_SUPPORT(session_type) || (USE_CDMA_SUPPORT(session_type)))
     {
       pb_id.prov_type = PBM_PROVISION_NONE;
       pb_id.pb_category = PBM_GPB;
       pb_id.slot_id = pbm_session_info[session_type].slot_id;
     }
   }
   //if the pb_id is PBM_ECC then we should process the two caches PBM_ECC,&PBM_ECC_OTHER
   if (pb_id.device_type == PBM_ECC)
   {
      uint8 l_pb_ids_size = 0;
      pbm_phonebook_type temp_pb_id = {PBM_GPB};
      session_type = PBM_SESSION_DEFAULT;
      l_pb_ids_size = ARR_SIZE(pbm_ecc_pb_ids);
      for (j = 0; j < l_pb_ids_size; j++)
      {    /* Case for all EFs in pbm_default_pb_ids*/
         switch (pbm_ecc_pb_ids[j])
         {
            case PBM_ECC_OTHER:
               /* check the number of records for EFs in EFS */
               if (NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[j])))
                  records_in_use += cache->records_in_use;
               break;

            case PBM_ECC:
            default:
               /* Follow app preference for traversing all EFs present under ADF DIR */
               session_type = pbm_mapping_to_session (pb_id.pb_category,(pbm_slot_type) pb_id.slot_id, pb_id.prov_type);
               {
                  /* check the number of records for EFs in EFS */
                  if ( NULL != (cache = pbm_device_id_to_cache(session_type, pbm_ecc_pb_ids[j])))
                     records_in_use += cache->records_in_use;
               }
               break;
         }
      }

      /* The Phonebook cache has no entries so fail. */
      if (records_in_use == 0)
      {
         ret_val = PBM_ERROR_NOT_FOUND;
		 goto END_FUNC_SESSION_FIND_NUMBER;
      }

      for (j = 0; j < l_pb_ids_size; j++)
      {
         if (PBM_ECC == pbm_ecc_pb_ids[j])
         {
               if(TRUE == pbm_number_search_in_cache (pb_id, number, loc_number_len, record, proc_func))
               {
                 ret_val = PBM_SUCCESS;
				 goto END_FUNC_SESSION_FIND_NUMBER;
               }

         } else /* PBM_ECC_OTHER */
         {
            temp_pb_id = pbm_device_id_to_pb_id(session_type, pbm_ecc_pb_ids[j]);
            if(TRUE == pbm_number_search_in_cache(temp_pb_id, number, loc_number_len, record, proc_func))
            {
              ret_val = PBM_SUCCESS;
			  goto END_FUNC_SESSION_FIND_NUMBER;
            }
         }
      }
   }

   else
   {
      if ( ( pb_ready_ret_val = check_pb_ready(pb_id) ) != PBM_SUCCESS)
      {
         ret_val = pb_ready_ret_val;
         goto END_FUNC_SESSION_FIND_NUMBER;
      }
      if ( NULL != (cache = pbm_pb_id_to_cache(pb_id)))
      records_in_use += cache->records_in_use;

      /* The Phonebook cache has no entries so fail. */
      if (records_in_use == 0)
      {
         ret_val = PBM_ERROR_NOT_FOUND;
		 goto END_FUNC_SESSION_FIND_NUMBER;
      }

      if(TRUE == pbm_number_search_in_cache(pb_id, number, loc_number_len, record, proc_func))
      {
        UIM_MSG_HIGH_0("number found");
        ret_val = PBM_SUCCESS;
		goto END_FUNC_SESSION_FIND_NUMBER;
      }
   }

END_FUNC_SESSION_FIND_NUMBER:
   rex_leave_crit_sect(&pbm_crit_sect);
   return ret_val;
}

/*===========================================================================
FUNCTION PBM_SESSION_GET_FIELD_INFO_COUNT

DESCRIPTION
  This function returns the number of PBM fields supported by the specified
  PBM phonebook or category.

PROTOTYPE:
  pbm_return_type pbm_get_field_info_count(pbm_phonebook_type pb_id, pbm_cat_e_type cat,
                                           int *num)
PARAMETERS:
  pb_id       [in]: Phonebook device.
  cat         [in]: Category for which information is requested.
  num        [out]: Number of fields supported by this category.

RETURN VALUE:
  PBM_SUCCESS         : If successful
  PBM_ERROR_NOT_READY : If requested phonebook is not ready
  PBM_ERROR           : Otherwise

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_get_field_info_count(pbm_phonebook_type pb_id, pbm_cat_e_type cat,
                                         int *num)
{
   pbm_internal_field_info *field_info = NULL;

   PBM_CHECK_PTR_RET(num, PBM_ERROR);

   if (pb_id.device_type != PBM_ECC_OTHER &&
             cat != PBM_CAT_NONE && pb_id.device_type != pbm_cat_to_phonebook(cat))
   {
      UIM_MSG_ERR_2("Category %d and phonebook %d does not match", cat, pb_id.device_type);
      return PBM_ERROR;
   }

   PBM_MEM_ALLOC(field_info,sizeof(pbm_internal_field_info));
   if(field_info == NULL)
   {
      UIM_MSG_ERR_0("field_info alloc failed ");
      return PBM_ERROR;
   }

   if(pbm_get_internal_field_info(pb_id,field_info) == PBM_SUCCESS)
   {
     *num = field_info->num_fields;
   }
   else
   {
      PBM_MEM_FREEIF(field_info);
      return PBM_ERROR;
   }

   PBM_MEM_FREEIF(field_info);
   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_SESSION_GET_FIELD_INFO

DESCRIPTION
  This function returns field information associated with each PBM field
  supported by the specified category

PROTOTYPE:
  pbm_return_type pbm_get_field_info(pbm_phonebook_type pb_id, pbm_cat_e_type cat,
                                   pbm_field_info_s_type *pf, int size)
PARAMETERS:
  pb_id       [in]: The phonebook device for which information is requested
  cat         [in]: Category for which information is requested.
  pf         [out]: Array of field info which is filled up by PBM
  size        [in]: Size of pf array in bytes.

RETURN VALUE:
  PBM_SUCCESS         : If successful
  PBM_ERROR_NOT_READY : If requested phonebook is not ready
  PBM_ERROR           : Otherwise

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_get_field_info(pbm_phonebook_type pb_id, pbm_cat_e_type cat,
                                   pbm_field_info_s_type *pf, int size)
{
   pbm_internal_field_info *field_info = NULL;
   pbm_return_type          ret_val    = PBM_SUCCESS;

   PBM_CHECK_PTR_RET(pf, PBM_ERROR);

  do
  {
    if(size <= 0)
    {
      UIM_MSG_HIGH_1("Invalid Size %d",size);
      ret_val = PBM_ERROR;
      break;
    }

    if (pb_id.device_type != PBM_ECC_OTHER &&
        cat != PBM_CAT_NONE && pb_id.device_type != pbm_cat_to_phonebook(cat))
    {
      UIM_MSG_ERR_2("Category %d and phonebook %d does not match", cat, pb_id.device_type);
      ret_val = PBM_ERROR;
      break;
    }

    PBM_MEM_ALLOC(field_info,sizeof(pbm_internal_field_info));
    if(field_info == NULL)
    {
      UIM_MSG_ERR_0("field_info alloc failed ");
      ret_val = PBM_ERROR;
      break;
    }

    if(pbm_get_internal_field_info(pb_id,field_info) == PBM_SUCCESS)
    {
      memscpy(pf, size, (void *)field_info->info,
            (uint16) (field_info->num_fields*(uint16)sizeof(pbm_field_info_s_type)));
    }
    else
    {
      ret_val = PBM_ERROR;
      break;
    }
  }while(0);

   PBM_MEM_FREEIF(field_info);
   return ret_val;
}


/*===========================================================================
FUNCTION pbm_compare_field_types

DESCRIPTION
  This function compares two field types to determine if they are
  the same or not.  If PBM accepts a UCS2 string, it can also accept
  a PBM8 string.

PROTOTYPE:
  boolean pbm_compare_field_types(pbm_field_type_e_type type_provided,
                                  pbm_field_type_e_type type_accepted)

PARAMETERS:
  type_provided [in]: The type of the record being checked.
  type_accepted [in]: The type PBM will accepts.

RETURN VALUE:
  TRUE:  The types are the same or the accepted type is UCS2 and
         the provided type is either UCS2 or STRING.
  FALSE: There are differences.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static boolean pbm_compare_field_types(
  pbm_field_type_e_type type_provided,
  pbm_field_type_e_type type_accepted)
{
  switch (type_accepted)
  {
    case PBM_FT_UCS2:
      return((type_provided == PBM_FT_STRING) ||
             (type_provided == PBM_FT_UCS2)||
             (type_provided == PBM_FT_UCS2_COMPRESSED));

    case PBM_FT_PHONE:
      return((type_provided == PBM_FT_BCD) ||
             (type_provided == PBM_FT_PHONE));

    default:
      break;
  }

  return(type_provided == type_accepted);
} /* pbm_compare_field_types */


static pbm_return_type pbm_validate_ph_uid_in_cache(
    pbm_field_s_type    *ptyped_fields,
    pbm_record_id_type  pbm_cache_rec_id,
    int                 i
    )
{
  int                   j;
  pbm_addr_cache_s_type *record;
  pbm_return_type       ret = PBM_SUCCESS;

  record = pbm_cache_record_get(pbm_cache_rec_id);
  PBM_CHECK_PTR_RET(record, PBM_ERROR);
  /* Locate unique ID field in cache record. */
  for (j = 0; j < record->num_fields; j++)
  {
    if (record->fields[j].field_id == PBM_FIELD_PH_UID)
    {
      void *data_ptr1;
      void *data_ptr2;
      /* Found, compare against the update field. */
      if (ptyped_fields[i].data_len != record->fields[j].data_len)
      {
        ret = PBM_ERROR_UNIQUE_IDS_CHANGED;
      }
      else
      {
        data_ptr1 = pbm_get_field_data_ptr(ptyped_fields,i);
        data_ptr2 = pbm_get_field_data_ptr(record->fields,j);
        if (!data_ptr1 || !data_ptr2)
        {
          UIM_MSG_ERR_2("Null ptr 0x%x 0x%x", data_ptr1, data_ptr2);
          ret = PBM_ERROR;
        }
        else if (memcmp(data_ptr1, data_ptr2, ptyped_fields[i].data_len) != 0)
        {
          ret = PBM_ERROR_UNIQUE_IDS_CHANGED;
        }
      }
      break;
    }
  }
  if ( (ret == PBM_SUCCESS) && (j == record->num_fields) ) /* if ret contains some error then this step is not required */
  {
    UIM_MSG_ERR_1("Failed locating PH_UID field for cache record %d",
      pbm_cache_rec_id);
  }
  (void) pbm_cache_record_free(record);
  return ret;
}


static pbm_return_type pbm_validate_ph_uid(
    pbm_phonebook_type    pb_id,
    pbm_field_s_type      *ptyped_fields,
    pbm_record_id_type    pbm_cache_rec_id,
    int                   num_fields
    )
{
  int i;
  pbm_return_type ret = PBM_SUCCESS;
  /* Only for EFS phone book. */
  if (pb_id.device_type != PBM_EFS)
  {
    return PBM_SUCCESS;
  }
  /* Locate PH_UID field in update field list. */
  for (i = 0; i < num_fields; i++)
  {
    if (ptyped_fields[i].field_id == PBM_FIELD_PH_UID)
    {
      break;
    }
  }
  /* If adding a new record... */
  if (pbm_cache_rec_id == PBM_SESSION_GET_FIRST_AVAILABLE)
  {
    /* Not permitted to have the PH_UID field set. */
    if (i != num_fields)
    {
      UIM_MSG_ERR_2("Have PH_UID field in GET_FIRST_AVAILABLE %d %d",
        i, num_fields);
      return PBM_ERROR_INVALID_REC;
    }
    return PBM_SUCCESS;
  }

  /* If updating an existing record... */
  if (i == num_fields)

  {
    /* Did not find the PH_UID field. */
    return PBM_SUCCESS; /* All OK so far, done with these checks... */
  }
  /* Check that the PH_UID field matches what is in cache. */
  rex_enter_crit_sect(&pbm_crit_sect);
  ret = pbm_validate_ph_uid_in_cache(ptyped_fields, pbm_cache_rec_id, i);
  rex_leave_crit_sect(&pbm_crit_sect);
  return ret;
}

/*===========================================================================
FUNCTION PBM_SESSION_RECORD_VALIDATE

DESCRIPTION
  This function checks whether a given record is valid.

  The checks include the following.
    - Check whether fields that are required are included. For ADN or FDN phonebooks
      there should be a non-NULL number string. For other phonebooks there should be
      a non-NULL number OR non-NULL name string.
    - Check whether the maximum number of a particular field or the maximum size of
      a particular field or the maximum size of the entire record is exceeded.
    - Check whether all the fields are supported for this phonebook and whether the
      types of fields match the field id.

PROTOTYPE:
  pbm_return_type pbm_session_record_validate(pbm_phonebook_type pb_id, uint16 rec_id,
                                    pbm_cat_e_type cat,
                                    uint8 *pfields, int num_fields);
PARAMETERS:
  pb_id       [in]: Phonebook device
  rec_id      [in]: Record id.
  cat         [in]: Category of the record
  pfields     [in]: buffer with Array of fields containing data (same struct as
                    pbm_record_read() and pbm_session_record_write()
  num_fields  [in]: Number of fields.

RETURN VALUE:
  PBM_ERROR_INDEX_INVALID   : If the record contains an INDEX field and if it is
                              invalid or the record id is not valid.
  PBM_ERROR_TEXT_TOO_LONG   : If name is too long.
  PBM_ERROR_NUMBER_TOO_LONG : If number is too long.
  PBM_ERROR_NOT_READY       : If the phone book requested has not been initialized.
  PBM_ERROR_NO_SIM          : If the corresponding UIM is not present.
  PBM_ERROR_NOT_AVAILABLE   : If the corresponding UIM file is not available
  PBM_ERROR_RESTRICTED      : If UIM file is not available due to restriction
  PBM_ERROR_INVALID_REC     : If the record is not valid for this phonebook due
                              to other reasons.
  PBM_SUCCESS               : If record is valid


DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_record_validate(pbm_phonebook_type pb_id,
                                    pbm_record_id_type rec_id,
                                    pbm_cat_e_type cat,
                                    uint8 *data_buf,
                                    int data_buf_size,
                                    int num_fields)
{
   int group = INVALID_GROUP,x;
   int i;
   int j;
   int counts[PBM_MAX_NUM_FIELDS];
   boolean record_required_fields[PBM_MAX_REQUIRED_FIELDS],num_plus=FALSE,ton_not_int=FALSE,ton_not_int_complex=FALSE,num_plus_complex=FALSE;// num_plus is flag to check the + sign in number  and ton_not_int is flag to check TON is not International
   pbm_phonebook_type old_pb_id = {PBM_GPB};
   pbm_field_s_type *ptyped_fields = (pbm_field_s_type *)data_buf;  //lint !e826 unusual cast
   pbm_record_id_type pbm_cache_rec_id = rec_id;
   uint8 *start_data_field_ptr,*end_data_field_ptr;// for validating pbm_buffer
   void *complex_field,*sub_field_data_ptr,*ton_data_ptr;
   //complex_field  -  to get the field data pointer of the complex field
   //ton_data_ptr - to get the field data ptr of TON
   char *num_ptr;// pointing to the first char of the number
   uint32 anr_sub_fields_size[PBM_MAX_SUB_FIELDS_IN_COMPLEX];// PBM_MAX_SUB_FIELDS_IN_COMPLEX = 3
   uint32 complex_sizes=0,num_comp_fields,s_f_cnt=0;
   pbm_field_s_type *sub_fields_ptr;
   pbm_session_enum_type session_type;
   pbm_return_type ret = PBM_SUCCESS;
   pbm_internal_field_info *field_info_ptr = NULL;

   if ((num_fields > PBM_MAX_NUM_FIELDS) || (num_fields < 0))
   {
      UIM_MSG_ERR_1("Num_fileds are %d",num_fields);
      return PBM_ERROR;
   }

   /* Parameter check */
   ret = pbm_record_write_check(pb_id,rec_id, cat,(pbm_field_s_type *) data_buf,
                                   num_fields);
   PBM_CHECK_ERR_RET(ret, "Parameter check failed");

   if (rec_id != PBM_SESSION_GET_FIRST_AVAILABLE)
   {
      PB_CACHE *pb_cache = NULL;
      pb_cache = pbm_pb_id_to_cache(pb_id);
		  PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);
      if (pb_cache->circular == TRUE)
      {
         //Get the true cache rec_id for circular caches
         uint16 location = pbm_session_record_id_to_location(rec_id);
         location = PBM_CIRC_CACHE_NTHLOC(pb_cache, location);
         pbm_cache_rec_id = pbm_session_location_to_record_id(pb_id,location);
      }
   }
   memset(counts, 0, sizeof(counts));
   /* Check if pfields is empty */
   if (num_fields)
   {
      int calculated_size;
      PBM_CHECK_PTR_RET(ptyped_fields,PBM_ERROR_INVALID_REC);
      calculated_size = pbm_calculate_size_from_fields((pbm_field_s_type*)data_buf,num_fields);
      if ((data_buf_size < calculated_size) || (calculated_size > PBM_MAX_RECORD_SIZE))
      {
         if (calculated_size > PBM_MAX_RECORD_SIZE)
         {
            UIM_MSG_ERR_3("Invalid Record (have %d fields, calculated size %d beyond max needed %d)",
                        num_fields, calculated_size, PBM_MAX_RECORD_SIZE );
         } else
         {
            UIM_MSG_ERR_3("Invalid Record (have %d fields, data given %d needed %d)",
                        num_fields, data_buf_size, calculated_size );
         }
         return PBM_ERROR_INVALID_REC;
      }
      else if (calculated_size > 1024)
      {
         UIM_MSG_HIGH_1("Record of unusual size.  %d", calculated_size);
      }
   } else
   {
      /* Deletion */
      if (ptyped_fields)
      {
         UIM_MSG_ERR_0("Invalid Record (deletion, non-null data)");
	         ret = PBM_ERROR_INVALID_REC;
	      }
		  else
	      {
             ret = PBM_SUCCESS;
		  }
		  return ret;
   }

   if ((cat == PBM_CAT_ERROR) || (cat != PBM_CAT_NONE && pb_id.device_type != pbm_cat_to_phonebook(cat)))
   {
      UIM_MSG_ERR_2("Category %d and phonebook %d does not match", cat, pb_id.device_type);
      return PBM_ERROR_INVALID_REC;
   }

   group = pbm_pb_id_to_fieldgroup(pb_id);
   if (group == INVALID_GROUP)
	   	{
      return PBM_ERROR_NOT_SUPPORTED;
	   	}

   // If there are no errors, check the category/record ID pair.
   if (pbm_cache_rec_id != PBM_SESSION_GET_FIRST_AVAILABLE)
   {
      old_pb_id = pbm_session_record_id_to_phonebook_type(pbm_cache_rec_id);

      if (pb_id.device_type != old_pb_id.device_type)
      {
         UIM_MSG_ERR_3("Can not move record %d from one phonebook %d to another %d",
                     pbm_cache_rec_id, old_pb_id.device_type, pb_id.device_type);
         return PBM_ERROR_INVALID_REC;
      }
   }

for (x = 0; x < num_fields; x++)
{

 // validating the data-buffer
 start_data_field_ptr = (uint8 *)pbm_get_field_data_ptr(ptyped_fields,x);
 end_data_field_ptr = start_data_field_ptr + ptyped_fields[x].data_len;
 if( !( (start_data_field_ptr > data_buf) && (start_data_field_ptr < (data_buf+data_buf_size)) && (end_data_field_ptr > data_buf) && (end_data_field_ptr <= (data_buf+data_buf_size))))
  {
     UIM_MSG_ERR_0("Corrupt data_buffer");
     return PBM_ERROR;
  }


// If field is complex then extract the number from it and check for + sign and TON
 if ( ptyped_fields[x].field_id == PBM_FIELD_COMPLEX )
   {
  complex_field =(pbm_complex_field_s_type *)pbm_get_field_data_ptr(ptyped_fields,x) ;
  PBM_CHECK_PTR_RET(complex_field,PBM_ERROR);   // field-id is present but no data
  num_comp_fields = pbm_complex_fill_sizes(anr_sub_fields_size,PBM_MAX_SUB_FIELDS_IN_COMPLEX,complex_field) ;
  if ( (num_comp_fields == 0 ) ||  (!pbm_allocate_fields((uint8 **)&sub_fields_ptr,&complex_sizes,num_comp_fields,(const uint32 *)anr_sub_fields_size  ) ))
      {
       UIM_MSG_ERR_0("No sub fields found in complex field");//print PBM error message
       return PBM_ERROR ;
      }
  if ( pbm_get_fields_from_complex(complex_field,sub_fields_ptr,(int*) &num_comp_fields) == PBM_SUCCESS)
  {
      num_plus_complex=FALSE;
      ton_not_int_complex=FALSE;
      for(s_f_cnt = 0; s_f_cnt < num_comp_fields; s_f_cnt++)
      {
        sub_field_data_ptr = pbm_get_field_data_ptr(sub_fields_ptr,s_f_cnt);
        if(sub_field_data_ptr == NULL)
         {
           pbm_free_fields((uint8 **)&sub_fields_ptr,num_comp_fields);
           return PBM_ERROR;
         }
        if ( sub_fields_ptr[s_f_cnt].field_id == PBM_FIELD_ADDITIONAL_NUMBER )
        {
           num_ptr =(char *)pbm_get_field_data_ptr(sub_fields_ptr,s_f_cnt);
           if ( num_ptr == NULL )
           {
               UIM_MSG_ERR_0("Unexpected NULL pointer");
               pbm_free_fields((uint8 **)&sub_fields_ptr ,num_comp_fields);
               return PBM_ERROR;
      }
           else if ( *num_ptr == '+' )
      {
               num_plus_complex = TRUE; // flag to track + sign in number )
         }
      }

        if (sub_fields_ptr[s_f_cnt].field_id == PBM_FIELD_GSM_TONNPI )
        {
           ton_data_ptr = pbm_get_field_data_ptr(sub_fields_ptr,s_f_cnt);
           if ( ton_data_ptr == NULL )
           {
               UIM_MSG_ERR_0("Unexpected NULL pointer");
               pbm_free_fields((uint8 **)&sub_fields_ptr ,num_comp_fields);
               return PBM_ERROR;
           }
           else if (( ( *((byte *)ton_data_ptr) & CM_TON_MASK )== CM_TON_NATIONAL ) || ( (*((byte *)ton_data_ptr) & CM_TON_MASK )==CM_TON_NETWORK_SPECIFIC ))
           {
               ton_not_int_complex = TRUE ;
           }

        }

        if ( num_plus_complex && ton_not_int_complex )
        {
          pbm_free_fields((uint8 **)&sub_fields_ptr ,num_comp_fields);
          return PBM_ERROR;
        }
      }//end of for - loop with index as 's_f_cnt'

  }

            }

  //if field is generic phone number then check for + sign and TON
  if ( ptyped_fields[x].field_id == PBM_FIELD_PHONE_GENERIC )
            {
          num_ptr =(char *)pbm_get_field_data_ptr(ptyped_fields,x);
          PBM_CHECK_PTR_RET(num_ptr,PBM_ERROR);
          if(*num_ptr == '+' )
              num_plus = TRUE; // flag to track + sign in number
            }
  if ( ptyped_fields[x].field_id == PBM_FIELD_GSM_TONNPI )
      {
          ton_data_ptr = pbm_get_field_data_ptr(ptyped_fields,x);
          PBM_CHECK_PTR_RET(ton_data_ptr,PBM_ERROR);
          if ( ( *((byte *)ton_data_ptr) & CM_TON_MASK )== CM_TON_NATIONAL  || (( *((byte *)ton_data_ptr) & CM_TON_MASK )==CM_TON_NETWORK_SPECIFIC ))
               ton_not_int = TRUE;
         }

  if ( num_plus && ton_not_int)
      {
               return PBM_ERROR;
      }


      }

   /* Validate phone unique ID field as follows -
     * If adding a new record: check that the PH_UID field does not exist.
     * If updating an existing record: check that either PH_UID field does not exist,
     * or that it PH_UID exists and matches exactly what is already in the cache.
     *
     * Note that later checks will verify that the PH_UID field exists only once.
     */

   ret = pbm_validate_ph_uid(pb_id, ptyped_fields, pbm_cache_rec_id, num_fields);
   if(ret != PBM_SUCCESS)
   {
     return ret;
   }

   /* If there are still no errors, check that the field can exist
    * for this category and that there are no missing required fields.
    */

   /* Build the list of required fields. */
   for (j = 0; j < PBM_MAX_REQUIRED_FIELDS; j++)
   {
      if (g_required_fields[group][j] != 0)
         record_required_fields[j] = TRUE;
      else
         record_required_fields[j] = FALSE;
   }
   session_type = pbm_mapping_to_session(pb_id.pb_category, (pbm_slot_type) pb_id.slot_id,
                                          pb_id.prov_type);

   PBM_MEM_ALLOC( field_info_ptr,sizeof(pbm_internal_field_info));
   if (NULL == field_info_ptr)
   {
     UIM_MSG_ERR_0("malloc failed for field_info_ptr");
     return PBM_ERROR;
   }

   do
   {
     /* For each field... */

     for (i = 0; i < num_fields; i++)
     {
       rex_enter_crit_sect(&pbm_crit_sect);
       if(!(pbm_hidden_key_verified & PBM_SESSIONID_TO_SESSION_MASK(session_type)))
       {
         if( (rec_id != PBM_SESSION_GET_FIRST_AVAILABLE) && ( pbm_is_rec_hidden(rec_id) ) )
         /* only while editing the contact we need to check if rec_id is hidden or not */
         {
           //return error if an attempt is made to overwrite hidden record,while hidden key is not verified
           ret = PBM_ERROR_RESTRICTED;
         }

         //shouldnt allow writing hidden records when key is not verified
         if(ptyped_fields[i].field_id == PBM_FIELD_HIDDEN &&
	              *((byte *) pbm_get_field_data_ptr(ptyped_fields,i)) == TRUE)
         {
           //return error if an attempt is made to write hidden record,while hidden key is not verified
           ret = PBM_ERROR_RESTRICTED;
         }
       }

       rex_leave_crit_sect(&pbm_crit_sect);
       if(ret != PBM_SUCCESS)
       {
         PBM_MEM_FREEIF (field_info_ptr);
         return ret;
       }

       /* Check to see if this field is a required field.  If so,
       * mark it as no longer required since we have one. */
       for (j = 0; j < PBM_MAX_REQUIRED_FIELDS; j++)
       {
         if (g_required_fields[group][j] == ptyped_fields[i].field_id)
         {
            record_required_fields[j] = FALSE;
            break;
         }
       }

       if(pbm_get_internal_field_info(pb_id,field_info_ptr) != PBM_SUCCESS)
       {
         PBM_MEM_FREEIF (field_info_ptr);
         return PBM_ERROR;
       }
      //if the request is to update ,
      //and the existing record is already using a ext record
      //we need to consider it as free record since this is
      //an overwrite operation
      else if(pbm_action_requires_sim(pb_id))
      {
        uint16          location;
        pbm_file_type  filename = PBM_FILE_NONE;
        PB_CACHE        *pb_cache = NULL;

        pb_cache = pbm_pb_id_to_cache(pb_id);
        if (NULL == pb_cache)
        {
           UIM_MSG_ERR_0("Unexpected NULL pointer");
           PBM_MEM_FREEIF (field_info_ptr);
           return PBM_ERROR;
        }

        location = pbm_session_record_id_to_location(rec_id);
        filename = pbm_pb_record_index_to_filename(pb_cache,location);
        if(pbm_find_extension_record(pb_cache->extension_cache,location,
                                       filename))
        {
          for(j = 0;j < field_info_ptr->num_fields; j++)
          {
            if(field_info_ptr->info[j].field_type == PBM_FT_PHONE)
            {
              field_info_ptr->info[j].max_length = 2 + PBM_MAX_NUM_LENGTH;
            }
          }
        }
      }

      /* Now check to make sure the field type and the number
       * of occurrances match what we need. */
      for (j = 0; j < field_info_ptr->num_fields; j++)
      {
         if(ptyped_fields[i].field_id == PBM_FIELD_HARDCODED_ECC &&
		 	                     pb_id.device_type == PBM_ECC_OTHER)
         {
           PBM_MEM_FREEIF (field_info_ptr);
           return PBM_ERROR_NOT_SUPPORTED;
         }
         if (ptyped_fields[i].field_id == field_info_ptr->info[j].field_id)
         {
            counts[j]++;
            if ((!pbm_compare_field_types(ptyped_fields[i].field_type,
                                      field_info_ptr->info[j].field_type)) ||
                (((field_info_ptr->info[j].num_items > 0) &&
                  (counts[j] > field_info_ptr->info[j].num_items))))
            {
               UIM_MSG_ERR_3("Bad Field Value: ID=0x%X Type=0x%x Count=%d", ptyped_fields[i].field_id,
                           ptyped_fields[i].field_type, counts[j] );
               UIM_MSG_ERR_2("for record %d, phonebook %d", pbm_cache_rec_id, pb_id.device_type);
               PBM_MEM_FREEIF (field_info_ptr);
               return PBM_ERROR_INVALID_REC;
            }

          /* for EFS write PBM do not convert the names encodign it writes in EFS as it is hence no check required */
            if (ptyped_fields[i].field_type== PBM_FT_UCS2) /* all the fields with UCS2 field type have to go with this procedure of checking
                                                                                                            if they can be acoomodated in ASCII only */
            {
               /* check with utility if the name can be converted to pure ascii */
               sim_coding_mode_enum_type out_mode = SIM_CODING_GSM7;
               uint16 out_base, out_converts;
               /* Subract 2 from max_length as PBM includes 2 bytes for UCS2 null char */
               uint16 out_len = field_info_ptr->info[j].max_length - 2;
               void *data_ptr = pbm_get_field_data_ptr(ptyped_fields,i);
               uint16 ucs_str_len = pbmutils_wstrlen((uint16*)data_ptr);

               pbmconvert_parse_input(data_ptr, ucs_str_len, &out_mode, &out_len, &out_base,
                          &out_converts, pbmconvert_return_input_string_first_ucs2_char, FALSE);
               if (out_converts != ucs_str_len)
               {
                 UIM_MSG_ERR_2("Did not convert all characters expected %d converted %d",
                                ucs_str_len, out_converts);
                 PBM_MEM_FREEIF (field_info_ptr);
                 return PBM_ERROR_TEXT_TOO_LONG;
               }
            }
            else
            if (ptyped_fields[i].data_len > field_info_ptr->info[j].max_length)
            {
               if (ptyped_fields[i].field_type == PBM_FT_PHONE)
               {
                 ret = PBM_ERROR_NUMBER_TOO_LONG;
               }
               else
               {
                 ret = PBM_ERROR_TEXT_TOO_LONG;
               }

               PBM_MEM_FREEIF (field_info_ptr);
               return ret;
            }

            /* A phone number can only be size PBM_NUM_SIZE if
               '+' dial character is the first digit*/
            if ((ptyped_fields[i].field_type == PBM_FT_PHONE) &&
                (ptyped_fields[i].data_len == field_info_ptr->info[j].max_length) &&
                (((char*)(pbm_get_field_data_ptr(ptyped_fields, i)))[0] != '+'))
            {
               PBM_MEM_FREEIF (field_info_ptr);
               return PBM_ERROR_NUMBER_TOO_LONG;
            }
            if (PBM_FIELD_COMPLEX == ptyped_fields[i].field_id)
            {
              pbm_field_s_type *sfield;
              pbm_complex_result_s_type *result = NULL;
              pbm_return_type ret_val;
              boolean anr_found = FALSE;
              ret_val = pbm_enum_complex_init(&ptyped_fields[i], NULL, &result);
              if (ret_val != PBM_SUCCESS)
              {
                 UIM_MSG_ERR_1("Failed complex field enumeration %d",ret_val);
                 PBM_MEM_FREEIF (field_info_ptr);
                 return PBM_FIELD_NONE;
              }
              while ((sfield = pbm_enum_complex_next(result)) != NULL)
              {

                if(sfield->field_id == PBM_FIELD_ADDITIONAL_NUMBER)
                {
                  anr_found = TRUE;
                  if (sfield->data_len > field_info_ptr->info[j].max_length)
                  {
                     if (ptyped_fields[i].field_type == PBM_FT_PHONE)
                     {
                       pbm_enum_complex_free(result);
                       PBM_MEM_FREEIF (field_info_ptr);
                       return PBM_ERROR_NUMBER_TOO_LONG;
                     }
                  }
                }
              } /* for each subfield */

              pbm_enum_complex_free(result);
              if(anr_found == FALSE)
              {
                UIM_MSG_ERR_3("Bad Field ID: ID=0x%X for record %d, phonebook %d",
                            ptyped_fields[i].field_id, pbm_cache_rec_id, pb_id.device_type);
                PBM_MEM_FREEIF (field_info_ptr);
                return PBM_ERROR_INVALID_REC;
              }

            }
            break;
         }
      }

      if (j == field_info_ptr->num_fields)
      {
         UIM_MSG_ERR_3("Bad Field ID: ID=0x%X for record %d, phonebook %d",
                     ptyped_fields[i].field_id, pbm_cache_rec_id, pb_id.device_type);
         PBM_MEM_FREEIF (field_info_ptr);
         return PBM_ERROR_INVALID_REC;
      }

      if (ptyped_fields[i].field_id == PBM_FIELD_INDEX)
      {
         uint16 index;
         void *data_ptr = pbm_get_field_data_ptr(ptyped_fields,i);
         if (NULL == data_ptr)
         {
            UIM_MSG_ERR_0("Unexpected NULL pointer");
            PBM_MEM_FREEIF (field_info_ptr);
            return PBM_ERROR;
         }
         index = *(uint16 *)(data_ptr);
         if (index != pbm_session_record_id_to_location(pbm_cache_rec_id))
         {
            UIM_MSG_ERR_3("Index within record %d does not match record id %d, phonebook %d",
                        index, pbm_cache_rec_id, pb_id.device_type);
            PBM_MEM_FREEIF (field_info_ptr);
            return PBM_ERROR_INDEX_INVALID;
         }
      }
   } /* for each field */

   for (j = 0; j < PBM_MAX_REQUIRED_FIELDS; j++)
   {
      if (record_required_fields[j])
      {
         UIM_MSG_ERR_2("Missing required field ID 0x%x, phonebook %d",
                     field_info_ptr->info[j].field_id, pb_id.device_type);
         PBM_MEM_FREEIF (field_info_ptr);
         return PBM_ERROR_INVALID_REC;
      }
   }
   }while(FALSE);

   PBM_MEM_FREEIF (field_info_ptr);
   return ret;
}

/*===========================================================================
FUNCTION PBM_NOTIFY_REGISTER

DESCRIPTION
  This function is used to register a callback function that is used to notify
  PBM events. The user data given as input will be passed on when the
  notification callback is called.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_notify_unregister
===========================================================================*/
pbm_return_type pbm_notify_register(PBM_NOTIFY_FUNC func, void *user_data)
{
   pbm_notify_node_s_type *node;
   pbm_notify_node_s_type *curr;

   PBM_CHECK_PTR_RET(func, PBM_ERROR);

   /* Allocate notification node */
   PBM_MEM_ALLOC(node, sizeof(pbm_notify_node_s_type));
   PBM_CHECK_PTR_RET(node, PBM_ERROR);

   UIM_MSG_HIGH_2("pbm_notify_register called for function %p user data:0x%x", func, user_data);
   rex_enter_crit_sect(&pbm_crit_sect);

   /* Return error if func/user_data pair is already registered */
   curr = pbm_notify_list;
   while (curr)
   {
      if ((curr->notify_func == func) && (curr->user_data == user_data))
      {
         UIM_MSG_ERR_2("Callback %p:0x%x already registered", func, user_data);
         PBM_MEM_FREEIF(node);
         rex_leave_crit_sect(&pbm_crit_sect);
         return PBM_ERROR;
      }
      curr = curr->next;
   }

   /* Add node to the head of notification list */
   node->next = pbm_notify_list;
   node->notify_func = func;
   node->user_data = user_data;

   pbm_notify_list = node;

   rex_leave_crit_sect(&pbm_crit_sect);

   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_NOTIFY_UNREGISTER

DESCRIPTION
  This function is used to cancel the notification function registered with PBM.

  The function pointer and user data provided as input should match the
  registered function pointer and user data.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_notify_register
===========================================================================*/
pbm_return_type pbm_notify_unregister(PBM_NOTIFY_FUNC func, void *user_data)
{
   pbm_notify_node_s_type *curr;
   pbm_notify_node_s_type *prev;
   pbm_return_type ret = PBM_ERROR;

   PBM_CHECK_PTR_RET(func, PBM_ERROR);
   UIM_MSG_HIGH_2("pbm_notify_unregister called for function %p user data:0x%x", func, user_data);
   rex_enter_crit_sect(&pbm_crit_sect);

   if (!pbm_notify_list)
   {
      UIM_MSG_ERR_0("Notification list is empty");
      rex_leave_crit_sect(&pbm_crit_sect);
      return PBM_ERROR;
   }

   prev = NULL;
   curr = pbm_notify_list;

   while (curr)
   {
      if ((curr->notify_func == func) && (curr->user_data == user_data))
      {
         /* Found node, delete it from the list */
         if (prev)
            prev->next = curr->next;
         else
            pbm_notify_list = curr->next;

         PBM_MEM_FREEIF(curr);
         ret = PBM_SUCCESS;
         break;
      }
      prev = curr;
      curr = curr->next;
   }

   rex_leave_crit_sect(&pbm_crit_sect);

   if (ret != PBM_SUCCESS)
   {
      UIM_MSG_ERR_3("error %d in finding notification function %p:0x%x",
                  ret, func, user_data);
   }

   return ret;
}


pbm_session_enum_type  pbmif_mmgsdi_session_to_pbm_session(mmgsdi_session_type_enum_type mmgsdi_session)
{
  pbm_session_enum_type l_pbm_session = PBM_SESSION_GPB_1;
  switch(mmgsdi_session)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      l_pbm_session = PBM_SESSION_LPB_GW;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION:
      l_pbm_session = PBM_SESSION_LPB_1X;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      l_pbm_session = PBM_SESSION_LPB_GW_SECONDARY;
      break;
    case MMGSDI_1X_PROV_SEC_SESSION:
      l_pbm_session = PBM_SESSION_LPB_1X_SECONDARY;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      l_pbm_session = PBM_SESSION_LPB_GW_TERTIARY;
      break;
    case MMGSDI_1X_PROV_TER_SESSION:
      l_pbm_session = PBM_SESSION_LPB_1X_TERTIARY;
      break;
    case MMGSDI_CARD_SESSION_SLOT_1:
      l_pbm_session = PBM_SESSION_GPB_1;
      break;
    case MMGSDI_CARD_SESSION_SLOT_2:
      l_pbm_session = PBM_SESSION_GPB_2;
      break;
    case MMGSDI_CARD_SESSION_SLOT_3:
      l_pbm_session = PBM_SESSION_GPB_3;
      break;
    default:
      l_pbm_session = PBM_SESSION_MAX;
      break;

  }
  return l_pbm_session;
}


/*===========================================================================
FUNCTION PBM_SESSION_EMERGENCY_NUMBER_CAT_EXT

DESCRIPTION
  This function searches the ECC phonebook for the number just passed in
  to determine if the current number is an Emergency number.

  If the found is found in gw only  then found_in_gw is marked TRUE and relavent category
  if associated with the number will be filled in ecc_category_ptr_gw

  If the found is found in 1x only  then found_in_1x is marked TRUE and relavent category
  if associated with the number will be filled in ecc_category_ptr_1x

  If the found is found in both 1x and gw  then found_in_gw and found_in_1x are marked TRUE
  and relavent category if associated with the number will be filled in ecc_category_ptr_gw
  and ecc_category_ptr_1x

  Length does not include the trailing NULL.

DEPENDENCIES


RETURN VALUE
  pbm_return_type - PBM_SUCCESS

SIDE EFFECTS
  none
===========================================================================*/
pbm_return_type pbm_session_emergency_number_cat_ext(
  pbm_session_enum_type        p_session_type,
  const byte                  *num,
  byte                         len,
  boolean                     *found_in_gw,
  boolean                     *found_in_1x,
  uint8                       *ecc_category_ptr_gw,
  uint8                       *ecc_category_ptr_1x
)
{
  pbm_session_enum_type                 l_session_type                = PBM_SESSION_MAX;
  pbm_record_id_type                    rec_id                        = PBM_INVALID_REC_ID;
  pbm_phonebook_type                    pb_id                         = {PBM_GPB};
  pbm_return_type                       ret_val                       = PBM_ERROR;
  boolean                               is_emergency_num              = FALSE;
  /* field found in the current record */
  pbm_field_id_e_type                   found_field                   = PBM_FIELDID_NONE;
  uint32                                best_match_index_gw;
  /* index into priority array of best category so far */
  uint32                                best_match_index_1x;
  uint32                                i                             = 0;
  /* category found in the current record */
  uint8                                 found_category                = 0;
  uint8                                 handle                        = 0;
  boolean                               found_on_slot[PBM_NUM_SLOTS]  = {FALSE};

  /* Rel 11 specs say to prefer categories in network over sim, and SIM over others
   * NV aren't spec'd, so they go to the bottom */
  pbm_field_id_e_type ecc_order_preference[] =
  {
    PBM_FIELD_NETWORK_ECC,
    PBM_FIELD_SIM_ECC,
    PBM_FIELD_HARDCODED_ECC,
    PBM_FIELD_NV_ECC
  };

  /* specs say to prefer categories in SIM over network, and network over others
   * NV aren't spec'd, so they go to the bottom */
  pbm_field_id_e_type ecc_order_preference_pre_rel11[] =
  {
    PBM_FIELD_SIM_ECC,
    PBM_FIELD_NETWORK_ECC,
    PBM_FIELD_HARDCODED_ECC,
    PBM_FIELD_NV_ECC
  };
  byte my_ph_num[PBM_MAX_NUM_LENGTH+1]; //plus null char

  PBM_CHECK_PTR5_RET(num, ecc_category_ptr_gw, ecc_category_ptr_1x, found_in_gw, found_in_1x, PBM_ERROR);

  UIM_MSG_HIGH_8("Checking num as emergency.  Len: %d.  Num: 0x%x 0x%x 0x%x 0x%x 0x%x pbm_3gpp_rel_ver 0x%x p_session_type 0x%x",
                  len, (len>0?num[0]:-1), (len>1?num[1]:-1), (len>2?num[2]:-1), (len>3?num[3]:-1), (len>4?num[4]:-1),
                  pbm_3gpp_rel_ver, p_session_type);
  *ecc_category_ptr_gw = 0;
  *ecc_category_ptr_1x = 0;
  *found_in_gw = FALSE;
  *found_in_1x = FALSE;
  if ( pbm_3gpp_rel_ver < LTE_3GPP_REL11  )
  {
    memscpy( ecc_order_preference , ARR_SIZE(ecc_order_preference) ,
             ecc_order_preference_pre_rel11 , ARR_SIZE(ecc_order_preference_pre_rel11)  );
  }
  best_match_index_gw = ARR_SIZE(ecc_order_preference);
  best_match_index_1x = ARR_SIZE(ecc_order_preference);

  if (len == 0 || len > PBM_MAX_NUM_LENGTH)
  {
    return PBM_SUCCESS;
  }

  memset(my_ph_num,0,sizeof(my_ph_num));
  memscpy(my_ph_num, sizeof(my_ph_num), num, len);

  for(l_session_type = PBM_SESSION_GPB_1;l_session_type < PBM_SESSION_MAX;l_session_type++)
  {
    if(PBM_LPB != pbm_session_type_to_pb_category(l_session_type))
    {
      continue;
    }

    /* only check in RATs which belong to the requested slot.*/
    if(pbm_session_info[l_session_type].slot_id != pbm_session_info[p_session_type].slot_id )
    {
      continue;
    }

    pb_id = pbm_uim_device_type_to_phonebook_type((pbm_uim_app_enum_type)l_session_type,PBM_ECC);

    /* If no valid slot_id associated with provisioning sessions means 
       either card is in detected state or in card error state */
    if(!(pbm_session_info[p_session_type].slot_id >= PBM_SLOT_1 &&
        pbm_session_info[p_session_type].slot_id < PBM_NUM_SLOTS))
    {
      pbm_slot_type            slot           = PBM_SLOT_1;
      pbm_session_enum_type    card_session   = PBM_SESSION_DEFAULT;

      for(slot = PBM_SLOT_1; slot < PBM_NUM_SLOTS; slot++)
      {
        if(!found_on_slot[slot] && pbm_is_card_present_in_slot(slot))
        {
          found_on_slot[slot] = TRUE;
          break;
        }
      }

      /*
       * 1. Check first NV per slot cache content to see if the dialing string is emergency or not
       * 2. check Hard code cache if the number is not found in NV per sub
       */
      if(slot != PBM_NUM_SLOTS)
      {
        /* get session_index for GPB for same (slot) first */
        card_session = pbm_pb_type_and_slot_to_session_type(slot, PBM_GPB);
        pb_id        = pbm_uim_device_type_to_phonebook_type((pbm_uim_app_enum_type)card_session,
                                                              PBM_ECC);
      }
    }

    /* Try the numbers in ECC phonebook. */
    ret_val = pbm_session_enum_rec_init_ext(pb_id, (uint16)PBM_CAT_ECC, PBM_FIELD_NONE,
                                            my_ph_num, (uint16)strlen((char*)my_ph_num)+1,
                                            PBM_SEARCHTYPE_EXACT | PBM_DATATYPE_STR |
                                            PBM_FIELDID_PHONE | PBM_STRTYPE_PBM8, &handle);
    if (PBM_SUCCESS == ret_val)
    {
      while (PBM_SUCCESS == pbm_session_enum_next_rec_id_ext(&rec_id, handle))
      {
        pbm_ecc_mode_type emergency_mode     = NOT_EMERGENCY;
        boolean           srv_category_found = FALSE;
        uint8             srv_status_flag    = 0;

        srv_category_found  = pbm_find_ecc_cat_in_rec (rec_id, 
                                                       &found_category,
                                                       &found_field,
                                                       &emergency_mode,
                                                       &srv_status_flag);
        UIM_MSG_HIGH_3("roam_status 0x%x srv_status 0x%x srv_status_flag 0x%x",
                        pbm_session_info[l_session_type].roam_status,
                        pbm_session_info[l_session_type].srv_status,
                        srv_status_flag);

        /* Resolve as normal if UE is in full service on home PLMN and srv_status_flag
	   associated to emergency number is set */
        if(pbm_session_info[l_session_type].roam_status == SYS_ROAM_STATUS_OFF && 
           pbm_session_info[l_session_type].srv_status  == SYS_SRV_STATUS_SRV &&
           srv_status_flag)
        {
          pbm_session_enum_rec_init_ext_free_handle(handle);
          return PBM_SUCCESS;
        }

        /* we have a match */
        is_emergency_num = TRUE;

        /* see if we can find a category */
        if (srv_category_found)
        {
          /* we found a category, Now, see if it's better than any category we may have. */
          for (i=0; i<ARR_SIZE(ecc_order_preference); i++)
          {
            if (ecc_order_preference[i] == found_field)
            {
              if (i < best_match_index_gw &&
                  (emergency_mode == EMERGENCY_GW || emergency_mode == EMERGENCY_GW_1X))
              {
                best_match_index_gw = i;
                *ecc_category_ptr_gw = found_category;
                UIM_MSG_HIGH_2("Picking GW emergency category 0x%x based on field 0x%x",
                               *ecc_category_ptr_gw, found_field);
              }
              else if (i < best_match_index_1x &&
                       (emergency_mode == EMERGENCY_1X ||
                        emergency_mode == EMERGENCY_GW_1X))
              {
                best_match_index_1x = i;
                *ecc_category_ptr_1x = found_category;
                UIM_MSG_HIGH_2("Picking 1X emergency category 0x%x based on field 0x%x",
                                *ecc_category_ptr_1x, found_field);
              }
            }
          }
        }

        UIM_MSG_HIGH_4("best_match_index_gw 0x%x best_match_index_1x 0x%x i 0x%x rec_id 0x%x",
                         best_match_index_gw, best_match_index_1x, i, rec_id);
        if (emergency_mode == EMERGENCY_1X ||
            emergency_mode == EMERGENCY_GW_1X)
        {
          *found_in_1x = TRUE;
        }
        if (emergency_mode == EMERGENCY_GW ||
            emergency_mode == EMERGENCY_GW_1X)
        {
          *found_in_gw = TRUE;
        }
      }
      pbm_session_enum_rec_init_ext_free_handle(handle); /* free the handle */
    }
    else
    {
      UIM_MSG_ERR_1("pbm_session_enum_rec_init failed %d", ret_val);
    }
  }
  UIM_MSG_HIGH_6("Emergency number status %d gw cat %d 1X cat %d found_in_gw 0x%x found_in_1x 0x%x session 0x%x",
                  is_emergency_num, *ecc_category_ptr_gw, *ecc_category_ptr_1x,
                  *found_in_gw, *found_in_1x, p_session_type);
  return PBM_SUCCESS;
} /* pbm_session_emergency_number_cat_ext */


/*===========================================================================
FUNCTION PBM_SESSION_EMERGENCY_NUMBER_CAT

DESCRIPTION
  This function searches the ECC phonebook for the number just passed in
  to determine if the current number is an Emergency number.  If it is, the
  category is returned in ecc_category_ptr.  If the number has no category, it is
  set to 0.

  Length does not include the trailing NULL.

DEPENDENCIES
  uipbm.h
  nv.h

RETURN VALUE
  Boolean

SIDE EFFECTS
  none
===========================================================================*/
boolean pbm_session_emergency_number_cat(pbm_session_enum_type sess_type,const byte *num, byte len, uint8 *ecc_category_ptr)
{
   pbm_record_id_type rec_id;
   pbm_phonebook_type pb_id = {PBM_GPB};
   pbm_return_type ret_val;
   boolean is_emergency_num = FALSE;
   pbm_field_id_e_type found_field;  //field found in the current record
   uint32 best_match_index;          //index into priority array of best category so far
   uint32 i;
   uint8 found_category, handle;             //category found in the current record

   /* specs say to prefer categories in SIM over network, and network over others */
   /* NV aren't spec'd, so they go to the bottom */
   pbm_field_id_e_type ecc_order_preference[] =
   {
      PBM_FIELD_SIM_ECC,
      PBM_FIELD_NETWORK_ECC,
      PBM_FIELD_HARDCODED_ECC,
      PBM_FIELD_NV_ECC
   };

   byte my_ph_num[PBM_MAX_NUM_LENGTH+1]; //plus null char

   PBM_CHECK_PTR2_RET(num, ecc_category_ptr, FALSE);

   UIM_MSG_HIGH_6("Checking num as emergency.  Len: %d.  Num: 0x%x 0x%x 0x%x 0x%x 0x%x",
                   len, (len>0?num[0]:-1), (len>1?num[1]:-1), (len>2?num[2]:-1), (len>3?num[3]:-1), (len>4?num[4]:-1));

   if (len == 0 || len > PBM_MAX_NUM_LENGTH)
   {
      return FALSE;
   }

   memset(my_ph_num,0,sizeof(my_ph_num));
   memscpy(my_ph_num, PBM_MAX_NUM_LENGTH, num, len);

   pb_id = pbm_uim_device_type_to_phonebook_type((pbm_uim_app_enum_type)sess_type,PBM_ECC);

   /* Try the numbers in ECC phonebook. */
   ret_val = pbm_session_enum_rec_init_ext(pb_id, (uint16)PBM_CAT_ECC, PBM_FIELD_NONE,
                               my_ph_num, (uint16)strlen((char*)my_ph_num)+1,
                               PBM_SEARCHTYPE_EXACT | PBM_DATATYPE_STR |
                               PBM_FIELDID_PHONE | PBM_STRTYPE_PBM8, &handle);
   *ecc_category_ptr = 0;
   best_match_index = ARR_SIZE(ecc_order_preference);
   if (PBM_SUCCESS == ret_val)
   {
      while (PBM_SUCCESS == pbm_session_enum_next_rec_id_ext(&rec_id, handle))
      {
        pbm_ecc_mode_type emergency_mode	   = NOT_EMERGENCY;
        boolean           srv_category_found       = FALSE;
        uint8             srv_status_flag          = 0;

        srv_category_found  = pbm_find_ecc_cat_in_rec (rec_id, 
                                                       &found_category,
                                                       &found_field,
                                                       &emergency_mode,
                                                       &srv_status_flag);
        /* Resolve as normal if UE is in full service and srv_status_flag
           associated to emergency number is set */
        if(pbm_session_info[sess_type].roam_status == SYS_ROAM_STATUS_OFF &&
           pbm_session_info[sess_type].srv_status  == SYS_SRV_STATUS_SRV &&
           srv_status_flag)
        {
          pbm_session_enum_rec_init_ext_free_handle(handle);
          return PBM_SUCCESS;
        }

         /* we have a match */
         is_emergency_num = TRUE;

        /* see if we can find a category */
        if (srv_category_found)
         {
            //we found a category
            //Now, see if it's better than any category we may have.
            for (i=0; i<ARR_SIZE(ecc_order_preference); i++)
            {
               if (ecc_order_preference[i] == found_field)
               {
                  if (i < best_match_index)
                  {
                     best_match_index = i;
                     *ecc_category_ptr = found_category;
                     UIM_MSG_HIGH_3("Picking emergency category 0x%x based on field 0x%x record 0x%x",
                                  *ecc_category_ptr, found_field, rec_id);
                  }
               }
            }
         }
      }
	  pbm_session_enum_rec_init_ext_free_handle(handle);
   } else
   {
      UIM_MSG_ERR_1("pbm_session_enum_rec_init failed %d", ret_val);
   }

   UIM_MSG_HIGH_2("Emergency number status %d cat %d",is_emergency_num,*ecc_category_ptr);

   return is_emergency_num;
}


/*===========================================================================
FUNCTION PBM_FIND_ECC_CAT_IN_REC

DESCRIPTION
  This function searches a given record id, assumed to be in the ECC
  phonebook for the ecc category.

  If it finds the field, it returns TRUE, and fills in ecc_category_ptr and
  found_field_ptr.

DEPENDENCIES

RETURN VALUE
  Boolean

SIDE EFFECTS
  none
===========================================================================*/
boolean pbm_find_ecc_cat_in_rec(
  pbm_record_id_type	 rec_id,
  uint8                 *ecc_category_ptr,
  pbm_field_id_e_type   *found_field_ptr,
  pbm_ecc_mode_type 	*emergency_mode,
  uint8                 *srv_status_flag_ptr
)
{
  uint8                  i           = 0;
  pbm_return_type        pbm_ret_val = PBM_ERROR;
  boolean                ret         = FALSE;
  pbm_field_s_type      *fields_buf  = NULL;
  uint32                 fields_size = 0;
  uint16                 category    = 0;
  int                    item_count  = 0;
  void                  *data_ptr    = NULL;

  PBM_CHECK_PTR4_RET(ecc_category_ptr, found_field_ptr, emergency_mode, srv_status_flag_ptr, FALSE);

  /* See if we can find a category */
  fields_size = (uint32)pbm_session_calculate_fields_size_from_id(rec_id);
  if (fields_size == 0)
  {
    UIM_MSG_ERR_1("invalid record passed to pbm_find_ecc_cat_in_rec 0x%x", rec_id);
    return FALSE;
  }

  PBM_MEM_ALLOC(fields_buf,fields_size);
  PBM_CHECK_PTR_RET(fields_buf, FALSE);

  pbm_ret_val = pbm_session_record_read(rec_id, &category, &item_count,
                                        (uint8*)fields_buf, fields_size);
  if (pbm_ret_val != PBM_SUCCESS)
  {
    UIM_MSG_ERR_1("pbm_record_read() failed with error 0x%x.", pbm_ret_val);
    PBM_MEM_FREEIF(fields_buf);
    return ret;
  }

  /* see if we can find the ecc cat field */
  for (i = 0; i < item_count; i++)
  {
    switch(fields_buf[i].field_id)
    {
      case PBM_FIELD_ECC_SERVICE_CATEGORY:
        data_ptr = pbm_get_field_data_ptr(fields_buf,i);
        PBM_CHECK_PTR_BREAK(data_ptr, ret, FALSE);

        *ecc_category_ptr = *(uint8*)data_ptr;
        ret = TRUE;
        break;

      case PBM_FIELD_EMERGENCY_MODE:
        data_ptr = pbm_get_field_data_ptr(fields_buf,i);
        PBM_CHECK_PTR_BREAK(data_ptr, ret, FALSE);

        *emergency_mode = *(uint8*)data_ptr;
        break;

      case PBM_FIELD_NETWORK_ECC:
      case PBM_FIELD_HARDCODED_ECC:
      case PBM_FIELD_NV_ECC:
      case PBM_FIELD_SIM_ECC:
        *found_field_ptr = fields_buf[i].field_id;
        break;

      /* See if we can find the service status flag field */
      case PBM_FIELD_SRV_STATUS_FLAG:
        data_ptr = pbm_get_field_data_ptr(fields_buf, i);
        PBM_CHECK_PTR_BREAK(data_ptr, ret, FALSE);

        *srv_status_flag_ptr = *(uint8*)data_ptr;
        break;

      default:
        break;
    }
  }

  UIM_MSG_HIGH_4("Found ECC cat 0x%x with field type 0x%x emergency_mode 0x%x srv_status_flag 0x%x",
                  *ecc_category_ptr, *found_field_ptr, *emergency_mode, *srv_status_flag_ptr);
  PBM_MEM_FREEIF(fields_buf);
  return ret;
}/* pbm_find_ecc_cat_in_rec */


/*===========================================================================
FUNCTION PBM_FDN_NUMBER_MATCH

DESCRIPTION
  The rules for FDN is that all the SIM digits must match, but
  extra digits are allowed at the end.
  The following examples show a SIM string, a dialed String,
  and what the function should return.
  { "12345",       "12345",        TRUE },
  { "12345",       "22345",        FALSE },
  { "12345",       "123456",       TRUE },
  { "12345",       "1234",         FALSE },
  { "12345H1234",  "12345",        TRUE },
  { "123456H1234", "12345",        FALSE },
  { "123456H1234", "123456",       TRUE },
  { "12345T1234",  "1234561234",   TRUE },
  { "12345T1234",  "12345612345",  TRUE },
  { "123456T1234", "12345",        FALSE },
  { "12345?1234",  "12345",        FALSE },
  { "123456?1234", "12345",        FALSE },
  { "123456?1234", "12345671234",  TRUE },
  { "123456?1234", "123456712345", TRUE },
  { "123456?1234", "1234567123",   FALSE },
  { "??????????",  "123456789",    FALSE },
  { "??????????",  "1234567890",   TRUE },
  { "??????????",  "12345678901",  TRUE },
  { "?????H????",  "12345678901",  TRUE },
  { "?????H????",  "12345678901",  TRUE },
  { "?????T????",  "12345",        TRUE },
  { "?????T????",  "1234",         FALSE },

DEPENDENCIES
  None.

SIDE EFFECTS
===========================================================================*/
boolean pbm_fdn_number_match(const byte *sim_num,    byte sim_num_len,
                             const byte *dialed_num, byte dialed_num_len)
{
   return pbm_number_match_int(sim_num, sim_num_len, dialed_num, dialed_num_len, TRUE);
}


/*===========================================================================
FUNCTION PBM_NUMBER_MATCH

DESCRIPTION
  The rules for non-FDN is that the digits in the SIM must match.  No
  extra characters are allow.  The following examples show a SIM string,
  a dialed String, and what the function should return.
  { "12345",       "12345",        TRUE },
  { "12345",       "22345",        FALSE },
  { "12345",       "123456",       FALSE },
  { "12345",       "1234",         FALSE },
  { "12345H1234",  "12345",        TRUE },
  { "123456H1234", "12345",        FALSE },
  { "123456H1234", "123456",       TRUE },
  { "12345T1234",  "1234561234",   FALSE },
  { "12345T1234",  "12345612345",  FALSE },
  { "123456T1234", "12345",        FALSE },
  { "12345?1234",  "12345",        FALSE },
  { "123456?1234", "12345",        FALSE },
  { "123456?1234", "12345671234",  TRUE },
  { "123456?1234", "123456712345", FALSE },
  { "123456?1234", "1234567123",   FALSE },
  { "??????????",  "123456789",    FALSE },
  { "??????????",  "1234567890",   TRUE },
  { "??????????",  "12345678901",  FALSE },
  { "?????H????",  "12345678901",  FALSE },
  { "?????H????",  "12345678901",  FALSE },
  { "?????T????",  "12345",        TRUE },
  { "?????T????",  "1234",         FALSE },
  { "911",         "911",          TRUE },
  { "911",         "9111",         FALSE },

DEPENDENCIES
  None.

SIDE EFFECTS
===========================================================================*/
boolean pbm_number_match(const byte *sim_num,    byte sim_num_len,
                         const byte *dialed_num, byte dialed_num_len)
{
   return pbm_number_match_int(sim_num, sim_num_len, dialed_num, dialed_num_len, FALSE);
}


/*===========================================================================
FUNCTION PBM_IS_INIT_COMPLETED

DESCRIPTION
will let the client know whether the init process of PBM is comlpeted or not


DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_init_completed()
{
   boolean init_complete = FALSE;
   uint8 i;

   UIM_MSG_HIGH_0("pbm_is_init_completed");

   for (i = 0; i < (uint8)MAX_UIM_APPS; i++)
   {
      //check for the initialisation of card inserted sessions only
      if ((pbm_session_activated & PBM_SESSIONID_TO_SESSION_MASK(i)) != 0)
      {
         /* Also check if session is active or not */
         if ((TRUE == pbm_init_info.pbm_session_init_done[i]) ||
             (FALSE == pbm_session_info[i].isActive))
         {
            init_complete = TRUE;
         } else
         {
            //return as soon as you find one SESSION that is not initialised
            init_complete = FALSE;
            break;
         }
      }
   }
   return init_complete;
}

/*===========================================================================
FUNCTION PBM_NUMBER_SEARCH_IN_CACHE

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static boolean pbm_number_search_in_cache (pbm_phonebook_type pb_id,
                                 const byte *number,
                                 int loc_number_len,
                                 pbm_record_s_type * record,
                                 PBM_FIND_CB_FUNC proc_func)
{
   pbm_addr_cache_s_type *record_ptr;
   PB_CACHE *cache;
   boolean (*match_fn)(const byte *, byte, const byte *, byte);

   pbm_field_s_type      *field_ptr;
   pbm_field_s_type      *sfield;
   int i, j,k;
   int index;
   pbm_return_type ret_val;
   boolean queried_number_has_plus = FALSE, cache_number_contains_ton = FALSE;
   boolean valid_ss_string = FALSE, cache_number_contains_plus = FALSE;
   boolean match_numbers = FALSE;
   uint8 offset_for_cache_number=0;
   byte local_number[PBM_MAX_NUM_LENGTH + 2];
   int pos_plus = 0xff,loc_number_len_actual = 0;
   byte cached_num[2 + PBM_MAX_NUM_LENGTH];
   uint16 cached_num_data_len = 0;
   uint8 *local_number_modified = NULL;
   boolean match_fn_return = FALSE;



   cache = pbm_pb_id_to_cache(pb_id);
   PBM_CHECK_PTR_RET(cache, FALSE);  /* KW error */
   memset(local_number,0,PBM_MAX_NUM_LENGTH + 2);
   if (pb_id.device_type == PBM_FDN)
   {
      match_fn = pbm_fdn_number_match;
   } else
   {
      match_fn = pbm_number_match;
   }
   if ( loc_number_len >= PBM_MAX_NUM_LENGTH + 1 )
   {
      return FALSE;
   }
   memscpy(local_number, PBM_MAX_NUM_LENGTH + 1, number, loc_number_len);
   /* scan here the passed number to find if it contains the "+" sign */
   if(strstr((char *)local_number,"+"))
   {
      if(!pbm_util_get_ss_ton_status_of_queried_number(&valid_ss_string,
               &queried_number_has_plus, (int*)&loc_number_len, local_number))
               /* this API will fill the desired flags accordingly */
      {
         UIM_MSG_HIGH_2("check failed invalid ton match %d %d ",valid_ss_string,queried_number_has_plus);
         return FALSE;
      }
   }
   else /* + sign not present in the number queried */
   {
      queried_number_has_plus = FALSE; /* since + sign is not present, match with numbers whose TON is not set */
   }
   loc_number_len_actual = loc_number_len ; 
   for (i = 1; i <= cache->num_of_records; i++)
   {
      cache_number_contains_ton = FALSE;
      if ((record_ptr = cache->pb_cache_array[i]) != NULL)
      {
         field_ptr = record_ptr->fields;
         for (j = 0; j < record_ptr->num_fields; j++)
         {
            pbm_complex_result_s_type *result = NULL;
            ret_val = pbm_enum_complex_init(&field_ptr[j], NULL, &result);
            if (ret_val != PBM_SUCCESS)
            {
               UIM_MSG_ERR_2("Failed complex field enumeration %d for %d",
                           ret_val, record_ptr->record_id);
               return FALSE;
            }
            while ((sfield = pbm_enum_complex_next(result)) != NULL)
            {
               cache_number_contains_ton = pbm_util_get_cache_number_status_of_ton(sfield);
               if(cache_number_contains_ton == TRUE)
               {
                break;
            }
            }
            pbm_enum_complex_free(result);
            result = NULL;
            if(cache_number_contains_ton == TRUE)
            {
             break;
         }
         }
         for (j = 0; j < record_ptr->num_fields; j++)
         {
            pbm_complex_result_s_type *result = NULL;
            local_number_modified = NULL;
            loc_number_len = loc_number_len_actual ;
            offset_for_cache_number = 0; /* resetting the offset */
            ret_val = pbm_enum_complex_init(&field_ptr[j], NULL, &result);
            if (ret_val != PBM_SUCCESS)
            {
               UIM_MSG_ERR_2("Failed complex field enumeration %d for %d",
                           ret_val, record_ptr->record_id);
               return FALSE;
            }
            while ((sfield = pbm_enum_complex_next(result)) != NULL)
            {
               cache_number_contains_plus = FALSE; /* reset the flags */
               match_numbers = FALSE;
               if (sfield->field_type == PBM_FT_PHONE || sfield->field_type == PBM_FT_BCD)
               {
                  /* check if number contains the + sign */
                  char *number_ptr;
                  char number[2 + PBM_MAX_NUM_LENGTH];
		  memset(number,0xFF,sizeof(number));
                  if (sfield->field_type == PBM_FT_BCD)
                  {
                    byte *bcd_num_ptr =   (byte*)pbm_get_field_data_ptr(sfield,0);
                    if (!pbm_bcd_to_ascii(&bcd_num_ptr[2],
                                          bcd_num_ptr[0]-1,
                                          bcd_num_ptr[1],
                                          (byte *) number))
                    {
                       UIM_MSG_ERR_2("SIM ERROR record %d %d ,could not be converted to ascii",
                                       ((byte*)pbm_get_field_data_ptr(sfield,0))[0],
                                       ((byte*)pbm_get_field_data_ptr(sfield,0))[1]);
                       continue;
                    }
                    number_ptr = number;
                  }
                  else
                  {
                     number_ptr = (char*)pbm_get_field_data_ptr(sfield,0);
                  }

                  if (number_ptr[0] == '+')
                  {
                     cache_number_contains_plus = TRUE;
                     offset_for_cache_number = 1; /* since cache number contains the + sign,
                                 while matching pbm cache number + 1 is to be send */
                  }

                  cached_num_data_len = strlen(number_ptr);
                 
                  if ( PBM_FEATURE_ENABLED == pbm_nv_get_feature_status(PBM_FEATURE_FDN_SPECIAL_HANDLING) ) 
                  {
			/*I.e. 
			      +7 999 925 190 0228
			       8 888 925 190 0228
			      So
			      1. if need to compare 2 numbers, then check if they start with 8 or +7
			      2. if starts with 8 or +7, then the numbers should be matched by last 10 digits
				 Otherthan last 10 digits are ignored.
		       */
			if( ( ( (cache_number_contains_plus == TRUE) && (number_ptr[1] == '7') && (cached_num_data_len >= 12)  ) &&
				      ( (queried_number_has_plus == FALSE) && (local_number[0] == '8') && (loc_number_len >= 11)       ) )  ||                                           
				      (( (cache_number_contains_plus == FALSE) && (number_ptr[0] == '8') && (cached_num_data_len >= 11) )  &&
				      ( (queried_number_has_plus == TRUE) && (local_number[0] == '7') && (loc_number_len >= 11)        ) ))
		       {
			 /* offset for cache number should be calculated based on '+' and also to offset digits more than 10 */
			 offset_for_cache_number = (offset_for_cache_number+1) + (cached_num_data_len - offset_for_cache_number - 11);
			 local_number_modified = local_number + 1 + (loc_number_len - 11);
			 loc_number_len = 10;
			 match_numbers = TRUE;
		       }/* special fdn check */
                       UIM_MSG_HIGH_6(" PBM Special FDN handling - match 0x%x, queried plus 0x%x, cache plus0x%x cache first 0x%x, cache second  0x%x, queried first 0x%x",
                                        match_numbers, queried_number_has_plus, cache_number_contains_plus, number_ptr[0], number_ptr[1], local_number[0]);
                   }

                  if (match_numbers != TRUE)
                  {
                  match_numbers = pbm_util_query_to_match_numbers(sfield->field_id,queried_number_has_plus
                        ,cache_number_contains_ton,cache_number_contains_plus,valid_ss_string);
                  }

                  UIM_MSG_HIGH_3("pbm_number_search_in_cache %d %d %d",
                                  queried_number_has_plus,cache_number_contains_ton,
                                  cache_number_contains_plus);
                  if (match_numbers)
                  {
                      memscpy(cached_num,sizeof(cached_num),number_ptr,cached_num_data_len);
                      if(number_ptr[0] == '+')
                      {
                        //below code will take care to match the following
                        //dialed num "**21*+821080101000#"
                        //cached num "+**21*821080101000#"

                        //Also it make sure that the following wont match
                        //dialed num "**21*821080101000#"
                        //cached num "+**21*821080101000#"

                        pos_plus = 0;
                        if (pbm_util_is_valid_sups_string(number_ptr+1,
                            strlen(number_ptr+1), &pos_plus, FALSE)   /* if the number is valid SS string */
                            )
                        {

                           if (pbm_util_is_valid_sups_string(number_ptr+1,
                               strlen(number_ptr+1), &pos_plus, TRUE)   /* if the number is valid SS string */
                               )
                           {
                                /* pos_plus will contain the position for the plus sign */
                                /* need to remove + sign from the first position and insert it into the right position */
                                /*pbm_construct_number_with_plus_sign_at_pos(return_data->number, strlen(return_data->number),pos_plus);*/

                                for (k=0;k<pos_plus;k++)
                                {
                                   cached_num[k] = cached_num[k+1];
                                }
                                cached_num[k] = '+';
                                offset_for_cache_number -= 1;
                           }

                        }

                      }

                      if(local_number_modified != NULL)
                      {
                        match_fn_return = (*match_fn)((byte *) cached_num + offset_for_cache_number,
                                          (byte) cached_num_data_len - offset_for_cache_number,
                                          (byte *) local_number_modified,
                                          (byte) loc_number_len);
                      }
                      else
                      {
                        match_fn_return = (*match_fn)((byte *) cached_num + offset_for_cache_number,
                                (byte) cached_num_data_len - offset_for_cache_number,
                                (byte *) local_number,
                                          (byte) loc_number_len);
                      }

                      if (match_fn_return)
                      {
                          index = pbm_session_record_id_to_location(record_ptr->record_id);
                          if (cache->circular)
                             index = PBM_CIRC_CACHE_LOCTOINDEX(cache, index);

                          if (record != NULL)
                          {
                             /* This builds a record from the fields, however, if there are
                             * multiple numbers in the cache, the number returned in record is
                             * the highest number field.  This will not affect current code,
                             * and this is for the legacy UI, so this caveot should not be
                             * an issue.
                             */
                             pbm_build_record_from_fields(field_ptr, record_ptr->num_fields,
                                                          index, record);
                          }
                          /* Clients expect asynchrounous call to have memory of its own.
                          * The input record pointer can be NULL */
                          if (proc_func != NULL)
                          {
                             pbm_record_s_type *async_record;
                             PBM_MEM_ALLOC(async_record, sizeof(pbm_record_s_type));
                             if (!async_record)
                             {
                                (*proc_func) (PBM_ERROR, NULL);
                             }
                             else
                             {
                                /* This builds a record from the fields, however, if there are
                                * multiple numbers in the cache, the number returned in record is
                                * the highest number field.  This will not affect current code,
                                * and this is for the legacy UI, so this caveot should not be
                                * an issue.
                                */
                                pbm_build_record_from_fields(field_ptr, record_ptr->num_fields,
                                                             index, async_record);

                                (*proc_func) (PBM_SUCCESS, async_record);
                                PBM_MEM_FREEIF(async_record);
                             }
                          }
                          pbm_enum_complex_free(result);
                          return TRUE;
                      }/* if matching phone number */
                  }/* if + sign inconsistency is there in the cache and passed number */
               } /* phone field type found */
            } /* for each subfield */
            pbm_enum_complex_free(result);
         } /* for each field */
      } /* if record not NULL */
   } /* for each record */

   UIM_MSG_HIGH_0("returning false pbm_number_search_in_cache ");
   return FALSE;
}

/*===========================================================================
FUNCTION PBM_ENUM_REC_INIT

DESCRIPTION
Initializes searches of the address book for specific records

Does search to set up for subsequent calls to pbm_enum_next_rec(), which
iterates through the records that match the given criteria.  Search results
are stored in static variables and are overwritten by the next call to
pbm_rec_init().

Searching rules are as follows:
1)  Only records with the given category are matched.  If PBM_CAT_NONE is
    specified, then all categories are matched.
2)  Only records that contain at least one instance of a field with the
    given field ID are matched.  If PBM_FIELD_NONE is specified, then
    this criterion is ignored.
3)  Only records with data in the specified field that "matches" the data
    specified by data_ptr are matched.  If data_ptr is NULL, then this
    criterion is ignored.

  What "matches" means depends on the setting of flags:
          Default (0x0) is case insensitive, starts with, string match
          with FIELD_NONE.
          Bit 0:    0   - PBM_CASE_INSENSITIVE
          Bit 0:    1   - PBM_CASE_SENSITIVE
          Bit 1-2:  0   - PBM_SEARCHTYPE_STARTSWITH
          Bit 1-2:  1   - PBM_SEARCHTYPE_EXACT
          Bit 1-2:  10  - PBM_SEARCHTYPE_CONTAINS
          Bit 3:    0   - PBM_DATATYPE_STR
          Bit 3:    1   - PBM_DATATYPE_BYTE
          Bit 4:    0   - PBM_FIELDID_NONE
          Bit 4:    1   - PBM_FIELDID_PHONE (Any type of phone)
          Bit 5:    0   - PBM_CALLHISTORY_NONE
          Bit 5:    1   - PBM_CALLHISTORY_ALL (incoming, outgoing, missed)
          Bit 6:    0   - PBM_ENUMERATE_ALL
          Bit 6:    1   - PBM_ENUMERATE_AVAILABLE
          Bit 7:    0   - PBM_MATCHING_NORMAL
          Bit 7:    1   - PBM_MATCHING_GREATER (Not supported with SEARCHTYPE_CONTAINS).
          Bit 8:    0   - PBM_SORT_LASTNAME_FIRST
          Bit 8:    1   - PBM_SORT_LASTNAME_SECOND

  Example:  pbm_rec_init(PBM_EFS, PBM_CAT_NONE, PBM_FIELD_NONE, "joh", 3,
                         PBM_CASE_INSENSITIVE | PBM_SEARCHTYPE_STARTSWITH)
            would match on the names "John Smith" and "Johann Bach"

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_rec_init(pbm_device_type device_type,
                               uint16 category,
                               pbm_field_id_e_type field_id,
                               const uint8 *data_ptr,
                               uint16 data_size,
                               uint32 flags)
{

  pbm_return_type ret = PBM_SUCCESS;
  pbm_phonebook_type pb_id = {PBM_GPB};
  uint16 cat = category;
    if((ret = pbm_sim_device_type_to_phonebook_type(device_type,&pb_id)) == PBM_SUCCESS)
    {
      cat = (uint16)pbm_legacy_cat_to_session_cat((pbm_cat_e_type)cat);
      ret = pbm_session_enum_rec_init(pb_id,
                               cat,
                               field_id,
                               data_ptr,
                               data_size,
                               flags);
    }
    return ret;
}

pbm_return_type pbm_enum_next_rec_id(uint16 *rec_id_ptr)
{

  pbm_return_type ret = PBM_SUCCESS;

    pbm_record_id_type session_rec_id;
  ret = pbm_session_enum_next_rec_id(&session_rec_id);
  //convert 32 bit rec_id to old 16 bit rec_id
  *rec_id_ptr = (uint16)session_rec_id;
  return ret;
}

pbm_return_type pbm_find_location (pbm_device_type device_type,
                                   int indx,
                                   pbm_record_s_type * data,
                                   PBM_FIND_CB_FUNC proc_func)
{
    pbm_return_type ret = PBM_SUCCESS;
    pbm_phonebook_type pb_id = {PBM_GPB};

    if((ret = pbm_sim_device_type_to_phonebook_type(device_type,&pb_id)) == PBM_SUCCESS)
    {
        ret = pbm_session_find_location (pb_id,
                           indx,
                           data);
    }
    if(proc_func != NULL)
        (*proc_func) (ret, data);
    return ret;
}


pbm_return_type pbm_find_number (pbm_device_type device_type,
                              const byte *number, int loc_number_len,
                              pbm_record_s_type * record,
                              PBM_FIND_CB_FUNC proc_func)
{
    pbm_return_type    ret   = PBM_SUCCESS;
    pbm_phonebook_type pb_id = {PBM_GPB};
    if((ret = pbm_sim_device_type_to_phonebook_type(device_type,&pb_id)) == PBM_SUCCESS)
    {
      ret = pbm_session_find_number (pb_id,
                              number, loc_number_len,
                              record,
                              proc_func);
    }
    UIM_MSG_HIGH_2("PBM pbm_find_number returned 0x%x device_type 0x%x", ret, device_type);
    return ret;
}

uint16 pbm_get_num_recs(pbm_device_type device_type)
{
    uint16 count = 0;
    pbm_phonebook_type pb_id = {PBM_GPB};
    if(pbm_sim_device_type_to_phonebook_type(device_type,&pb_id) == PBM_SUCCESS)
    {
      count = pbm_session_get_num_recs(pb_id);
    }
    return count;
}

int pbm_calculate_fields_size_from_id( uint16 rec_id )
{
    pbm_record_id_type new_rec_id;

    if(pbm_old_recid_to_new_recid(rec_id,&new_rec_id) == PBM_SUCCESS)
    {
      return pbm_session_calculate_fields_size_from_id(new_rec_id);
    }
    return 0;
}

pbm_return_type pbm_record_read(uint16            rec_id,
                                uint16           *category_ptr,
                                int              *num_fields_ptr,
                                uint8            *data_buf,
                                uint32            data_buf_size)
{
    pbm_phonebook_type pb_id;
    pbm_return_type ret = PBM_SUCCESS;
    pbm_record_id_type new_rec_id;

    if((ret = pbm_old_recid_to_new_recid(rec_id,&new_rec_id)) == PBM_SUCCESS)
    {
      ret = pbm_session_record_read(new_rec_id,
                                        category_ptr,
                                        num_fields_ptr,
                                        data_buf,
                                        data_buf_size);
      pb_id = pbm_session_record_id_to_phonebook_type(new_rec_id);
      if(category_ptr != NULL)
      {
        *category_ptr = (uint16)pbm_session_cat_to_legacy_cat(pb_id,(pbm_cat_e_type)*category_ptr);
      }

    }
    return ret;
}

/*===========================================================================
FUNCTION PBM_EXTENDED_FILE_INFO

DESCRIPTION
  Through GSDI will return the total number of records, the number of records
  in use, the text length, and the max phone number length for the specified phonebook.

  Data returned in proc_func.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_extended_file_info (pbm_device_type device_type,
                                        PBM_EXTENDED_FILE_INFO_CB_FUNC proc_func)
{
    pbm_extended_fileinfo_s_type info;
    pbm_return_type ret;
    pbm_phonebook_type pb_id;

    PBM_CHECK_PTR_RET(proc_func, PBM_ERROR);


    if((ret = pbm_sim_device_type_to_phonebook_type(device_type,&pb_id)) == PBM_SUCCESS)
    {

        ret = pbm_get_fileinfo_internal(pb_id, &info);

        (* proc_func)(ret,
                    device_type,
                    &info);
    }

    return ret;
}



pbm_return_type pbm_record_write( pbm_device_type         device_type,
                                  uint16                 *rec_id_ptr,
                                  uint16                  cat,
                                  int                     num_fields,
                                  const uint8            *data_buf,
                                  int                     data_buf_size,
                                  PBM_WRITE_CB_FUNC       proc_func,
                                  void                   *user_data)
{
    pbm_return_type ret = PBM_SUCCESS;
    pbm_record_id_type new_rec_id;
    pbm_phonebook_type pb_id = {PBM_GPB};
    uint16 category = cat;

    //new record case
    if(*rec_id_ptr == PBM_GET_FIRST_AVAILABLE)
    {
        new_rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;
    }
    //update case
    else
    {
        ret = pbm_old_recid_to_new_recid(*rec_id_ptr,&new_rec_id);
    }
    if(ret  == PBM_SUCCESS)
    {
        ret = pbm_sim_device_type_to_phonebook_type(device_type,&pb_id);
    }
    if(ret  == PBM_SUCCESS)
    {
        category =(uint16)pbm_legacy_cat_to_session_cat((pbm_cat_e_type)category);
        ret = pbm_session_record_write( pb_id,
                                  &new_rec_id,
                                  category,
                                  num_fields,
                                  data_buf,
                                  data_buf_size,
                                  proc_func,
                                  user_data);
        (void)pbm_new_recid_to_old_recid(rec_id_ptr,new_rec_id);

    }
    return ret;
}

pbm_return_type pbm_record_validate(pbm_device_type device_type,
                                    uint16 rec_id,
                                    pbm_cat_e_type cat,
                                    uint8 *data_buf,
                                    int data_buf_size,
                                    int num_fields)
{
    pbm_return_type ret = PBM_SUCCESS;
    pbm_record_id_type new_rec_id;
    pbm_phonebook_type pb_id = {PBM_GPB};

    //new record case
    if(rec_id == PBM_GET_FIRST_AVAILABLE)
    {
    new_rec_id = PBM_SESSION_GET_FIRST_AVAILABLE;
    }
    //update case
    else
    {
    ret = pbm_old_recid_to_new_recid(rec_id,&new_rec_id);
    }
    if(ret  == PBM_SUCCESS)
    {
    ret = pbm_sim_device_type_to_phonebook_type(device_type,&pb_id);
    }
    if(ret  == PBM_SUCCESS)
    {
      cat=pbm_legacy_cat_to_session_cat(cat);
      ret = pbm_session_record_validate(pb_id,
                                    new_rec_id,
                                    cat,
                                    data_buf,
                                    data_buf_size,
                                    num_fields);
    }
    return ret;
}

pbm_return_type pbm_clear_phonebook(pbm_device_type device_type)
{
    pbm_return_type ret = PBM_SUCCESS;
    pbm_phonebook_type pb_id = {PBM_GPB};
    if((ret = pbm_sim_device_type_to_phonebook_type(device_type,&pb_id)) == PBM_SUCCESS)
    {
      ret = pbm_session_clear_phonebook(pb_id);
    }
    return ret;
}

/*===========================================================================
  FUNCTION pbm_get_slot_info()

  DESCRIPTION
    Get the slot info for the given PBM session
    
  PARAMETERS
    pbm_session_enum_type pbm_session
    
  RETURN VALUE

    mmgsdi_slot_id_enum_type
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint16  pbm_get_slot_info (pbm_session_enum_type pbm_session)
{
  uint16     mmgsdi_slot_id;

  rex_enter_crit_sect(&pbm_crit_sect);
  mmgsdi_slot_id   = PBM_PBM_SLOT_TO_MMGSDI_SLOT(pbm_session_info[pbm_session].slot_id);
  rex_leave_crit_sect(&pbm_crit_sect);
  return mmgsdi_slot_id;
}

/*===========================================================================
  FUNCTION pbm_is_card_present_in_slot()
  DESCRIPTION
    Checks if card is present in the given slot or not
  PARAMETERS
    Slot
  RETURN VALUE
    boolean 
    - TRUE if card is present in that slot 
    - FALSE if card is not present in that slot 
  DEPENDENCIES
    None
  SIDE EFFECTS
    None
===========================================================================*/
boolean pbm_is_card_present_in_slot(uint8 slot)
{
  boolean is_present = FALSE ; 

  PBM_VALIDATE_SLOT_RET(slot,is_present);
  rex_enter_crit_sect(&pbm_crit_sect);
  is_present =   pbm_card_status[slot-1];
  rex_leave_crit_sect(&pbm_crit_sect);
  return is_present;
}
