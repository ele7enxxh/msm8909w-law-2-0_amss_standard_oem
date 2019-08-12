#ifndef MMGSDIUTIL_H
#define MMGSDIUTIL_H
/*===========================================================================


           M M G S D I   U T I L I T Y   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdiutil.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
08/02/16   gm      Support for CDMALess Device
07/08/16   av      Send SELECT ISDR to determine if card is eUICC or not
05/24/16   ar      Review of macros used by MMGSDI
05/23/16   dd      Skip Search Req for PB files
05/12/16   tkl     F3 log prints cleanup
05/02/16   sp      Reduce F3 log prints
05/02/16   nr      Don't reject activation req during ecc call for AP centric
04/27/16   vdc     Block select AID only for ARA-M AID
04/05/16   vdc     Send act resp and session changed evt at same time
10/26/15   nr      Disable MMGSDI PB cache for PBM module
10/16/15   ar      Adding MCCs in list which have default MNC len as 3 digits
09/30/15   ar      Fake the fourth byte of EF AD for buggy card
09/09/15   stv     Pack imsi_m activation status into subscription_ready_evt
09/07/15   av      Migrate to mcfg EFS APIs
07/09/15   ar      Validate PRL at diff steps if SD task not ready
06/24/15   vr      Replace GetCurrentThreadId with rex_self
05/15/15   lxu     Fix crash of refresh on EF GBABP
05/13/15   stv     Include uim_msg.h
04/17/15   nr      Phonebook record read optimization
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
03/24/15   kk      Subs prov EFS error handling
12/09/14   stv     Fix for subscription switch from SIM to USIM
11/10/14   yt      Return status words of original PIN OP
11/06/14   hh      Call GSTK NV API to read GSTK features in NV 65674
10/23/14   yt      Updates to session and app management
10/08/14   tl      Add check that globals not accessed without acquiring mutex
09/29/14   tkl     Added support for persistent cache
09/09/14   kk      Enhancements to NV handling
09/27/14   ar      Add support for multiple slots in BT-SAP
08/27/14   yt      Support for restricted SIM access via MMGSDI session APIs
08/11/14   bcho    Optimize srv req by using cached srv table in client context
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/28/14   vv      Remove unused function
07/28/14   am      Copy client_spec_files file list pointer also during REFRESH
07/15/14   yt      Support for NV refresh
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/30/14   vv      Remove Perso code
06/18/14   av      Fix out of bound access in mmgsdi_util_find_app_pin_index
05/27/14   av      Don't allow access if UST/CST can't be read(pin not verified)
05/15/14   yt      Move path/enum conversion utilities to off-target code
04/22/14   yt      Add mmgsdi_util_is_isim_aid()
04/22/14   av      Fix double free when handling icc pin operation response
03/28/14   kb      Update function assert_on_crit_sect_owner
03/26/14   tl      SIM Lock secondary revisions
03/25/14   yt      Handle refresh of deactivated app
03/25/14   vdc     Added support to verify PIN after PIN is changed
03/18/14   tl      Introduce new SIM Lock feature
03/04/14   yt      Check slot id of provisioning session during recovery
02/25/14   yt      Store parsed ATR in mmgsdi_data_slot_ptr
02/25/14   hh      Purge temporary mobile identities during UICC Reset Refresh
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/24/14   ar      Removed wrapper function mmgsdi_util_alloc()
02/10/14   ar      Added MMGSDI_RETURN_IF_NULL macros to reduce F3s
02/10/14   df      Add utility to check client has non-prov open
01/28/14   kb      Disable function assert_on_crit_sect_owner
01/28/14   vdc     Optimize read trans req to read from cache before SIM init
12/05/13   tkl     SFI Support
01/11/14   df      Fix off-target compilation
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/19/13   ar      Featurization for mmgsdi_malloc() and mmgsdi_free()
12/18/13   yt      Skip app termination as part of session deactivation
12/05/13   ar      Added new macro MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/04/13   spo     Do not set signals when MMGSDI task is already stopped
12/01/13   df      Add prov session type to session id util
11/21/13   vv      Halt 3gpp subscription if 3gpp2 perso fails
11/08/13   yt      Reduce size of MMGSDI tables
10/28/13   kb      Add offtarget support to check mutex is owned or not
10/21/13   yt      Add error checking for command APDUs
10/17/13   at      Support for MMGSDI Cancel APIs
10/16/13   df      Removing unused functions
10/04/13   rp      Added macros for pointer NULL check
10/03/13   yt      Multi-SIM support for OnChip
10/01/13   vdc     Remove mapping of CSIM/RUIM apps to 1X PRI for off target
09/24/13   yt      Remove function mmgsdi_util_is_gsdi_task_context()
08/30/13   yt      Remove usage of rand() to generate client and session IDs
08/26/13   am      Fix MMGSDI Sanity timer expiry processing
08/20/13   spo     Automatic Subscription provisioning enhancements in UIM
08/14/13   av      Optimize read transparent req by reading from cache directly
08/08/13   vdc     Remove mapping of CSIM/RUIM apps on slot 2/3 to 1X PRI
07/18/13   yt      Reduce redundant traces
07/09/13   vdc     Fix and code optimization for get available apps
06/28/13   bcho    APIs added to return plmn info from SE13 table.
06/28/13   vdc     When closing non-prov sessions, compare App types & AIDs
06/24/13   vdc     Added support for triple SIM
06/18/13   vv      Fix copying the entire path
06/14/13   vdc     Added utility function to get app index from the app
05/21/13   vv      Added support for session closed cause
05/21/13   av      Assume security attributes per spec if ARR read is skipped
05/20/13   av      Halt 3gpp2 subscription of 3gpp perso fails
05/20/13   vv      Added support to check if a logical channel can be re-used
05/18/13   tl      Correct print in memscpy
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
05/06/13   spo     Modified CPHS variables to array of pointers
04/16/13   tl      Rename diag_create_item_file to mmgsdi_diag_create_item_file
04/15/13   tl      Remove obsoleted featurization
03/13/13   kb      Add utility function to map an input Session ID to MMGSDI
                   Session ID
03/04/13   vv      Allow EFs outside ADF to be accessed before app. selection
01/08/13   vv      Fix to retrieve recovery data for each slot supported
01/07/13   vv      Check the return value when queuing a slot specific command
12/30/12   kk      Reworked AsyncTP for faulty RUIM cards having no GSM info
12/12/12   vv      Added support to populate the user data with current task
12/05/12   vv      Added support for parallel processing in MMGSDI
11/12/12   bcho    Added function to get length of MNC in IMSI
10/21/12   av      Update conf file only if necessary
10/01/12   vs      Added utility for context check including logic for SGLTE
09/28/12   abg     Updated ERR to MSG_ERROR
09/25/12   av      Skip reading EF-ARR for internal selects
09/19/12   vv      Reserve logical channel for CSIM app
09/05/12   av      Handling of VPE response from the card
09/03/12   bcho    mmgsdi_util_clear_opl_pnn_caches() function deleted
07/30/12   yt      Check if app is shareable across channels
07/25/12   bcho    mmgsdi_util_session_close_cleanup_and_notify prototype added
07/20/12   tl      Support more application and logical channels
05/23/12   vv      Added support for file update registration notification
04/16/12   tl      Added support for client name traces
03/29/12   shr     Aded support for session open with MF,
                   enhancements to file access request methods,
                   support for non-prov. app selection using zero length and
                   partial AID, support for Send APDU extension API
03/28/12   bcho    Function added to replace rex_sleep() with sleep mechanism
                   based on non-deferrable timer
03/12/12   vv      Fixed klocwork errors
03/05/12   bcho    MACRO added to call ERR_FATAL if mmgsdi globals are accessed
                   without acquiring critical section in gsdi task/non task
                   context
03/01/12   bcho    Function prototype comment block updated
02/23/12   av      Cleaned up remaining always ON features
02/23/12   bcho    Macros added for critical section acquisition and release
                   and its logging
01/17/12   kk      Do not allow external cache updates for specific EFs when
                   cache is empty
01/05/12   shr     Removed F3 logging from within critical sections
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   kk      Legacy GSDI removal updates
12/21/11   bcho    Changes for new API to open multiple sessions
                   in a single request
12/21/11   bcho    Added support for Get Operator Name API
10/28/11   yt      Added new files for cache-related utility functions
10/19/11   js      Support to select ADF before PIN2 operation
10/13/11   shr     Adding support for full recovery
08/09/11   tkl     Support for TMSI PLMN check
08/01/11   vs      Updates for key ref based PIN operations
05/26/11   ms      Support for diag nv item is necessary only on target
05/17/11   kk      Updates for Thread safety
05/17/11   shr     Updates for Thread safety
05/05/11   ms      Adding Support for diag nv item
04/26/11   yt      Replaced references to TMC heap with Modem heap
03/22/11   ea      moved mmgsdi_util_determine_app_type to mmgsdiutil module
04/01/11   yt      Verify validity of path for file access
03/11/11   ms      Clear the app capabilities in session deactivation
02/18/11   yt      Check if CSIM is provisioned before starting Refresh voting
02/03/11   ms      Updated F3 messages for Client ID
01/31/11   ssr     Added file access check
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
10/28/10   kk      Fixed header file inclusion related to modem mem heap
10/27/10   shr     If file security attributes indicates ARR record number
                   as 0, do not send read request to the card
10/22/10   js      Make mmgsdi do malloc from modem_heap
10/21/10   shr     Cleanup of Onchip USIM support
09/14/10   kk      Add critical section around attributes cache access
08/12/10   ssr     Fixed nvruim 1x primary session for slot2
08/10/10   adp     When we check the lookup table, check for the exact length
                   of the path.
07/21/10   adp     Check for uim file in the look up table first. Adding
                   mmgsdi_util_check_lookup_table API
07/16/10   ssr     Update fucntion header comment
07/09/10   tkl     Fixed enum mapping between mmgsdi_perso_feature_enum_type
                   & gsdi_perso_enum_type
07/02/10   ssr     Fixed refresh for ICC card which have both GSM and RUIM
06/10/10   shr     Allow close of Card Sessions
06/01/10   js      ZI memory reduction fixes
05/25/10   shr     Clean-up file access operations on Card Sessions
05/17/10   nmb     Updated mmgsdi_util_is_pin_valid to account for pin length
05/12/10   shr     File access operations need to be allowed on Card Sessions
                   irrespective of the provisioning status
05/04/10   shr     Removed temporary Feature Flags
05/04/10   shr     Clear File cache, Record cache and File attributes cache
                   when deactivating a provisioning application
05/03/10   shr     Added Dual Slot support for REFRESH RESET and REFRESH
                   RESET_AUTO
04/28/10   js      Temp fix to allocate memory from modem heap always
04/27/10   shr     Added Hiddenkey support
04/20/10   shr     Default provisioning is decided only on the basis of
                   NV_UIM_SELECT_DEFAULT_USIM_APP_I status. If a
                   Session is already active, do not attempt activation
04/13/10   ssr     Fixed get ruim capability
03/29/10   shr     Switch/deactivate provisioning clean up and redesign
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
02/19/10   shr     Clear File cache, Record cache and File attributes cache
                   while provisioning a new application
02/17/10   shr     Fixed Session activate/deactivate handling
02/16/10   kk      Removed "is default app" check for read/write cache
01/15/10   shr     Fixed incorrect access of freed memory locations
12/21/09   rn      Added a new prototype - mmgsdi_util_is_df_under_mf
12/21/09   kk      Fixed Compilation error
12/18/09   nb      Moved Dual Slot Changes
12/16/09   nb      Updated macros to fix klockwork errors
12/08/09   shr     Read EF-DIR and cache app data before sending
                   TP request to GSTK
12/07/09   js      Fixed Lint Errors
12/03/09   shr     Cache read process enhancement
10/07/09   mib     Removed dependency between UIM table and mmgsdi enum
10/08/09   kp      Replacing FEATURE_MODEM_HEAP with
                   FEATURE_UIM_ZI_MEMORY_REDUCTION
09/26/09   kp      ZI memory reduction changes
07/27/09   kk      Fixed Lint Error - Boolean within 'if' always evaluates
                   to False
07/21/09   shr     Fixed compilation warnings
07/08/09   shr     Fixed Lint warnings
04/12/09   js      Explicit cast to void* for ptr argument in mem_free and malloc
03/26/09   mib     Integration of support for ONCHIP USIM
03/19/09   js      Support for Enhanced NAA Refresh by file path
03/17/09   kk      Fixed lint error, "506: Constant value Boolean"
11/19/08   js      KW fix for potential null ptr dereferencing
10/07/08   js      Lint Fixes
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/25/08   js      Fixed compilation warning
08/15/08   js      Klockwork fix for MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR
07/17/08   kk      Added support for PKCS 15
06/12/08   srr     Support of MMGSDI Search functionality
06/23/08   sp      Add additional parameter to functions mmgsdi_util_reset_app_data()
                   mmgsdi_util_reset
06/04/08   tml     Added sw to cnf response header and return
                   MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED when malloc macro failed
03/07/08   sun     Added new function to calculate ber length
11/15/07   sun     Added support for NAA refresh
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/01/07   sun     Fixed Compile Warning
04/26/07   sp      Added function to get path from enum
03/26/07   sun     Check if additional files are being read during refresh
                   fcn
03/19/07   sun     Fixed Lint Error
03/05/07   nk      Added PIN Validation function
03/02/07   sun     Added Check for file attributes during read/write
01/1/07    sp      Fixed isues with ThinUI build.
01/11/07   sk      Changed return type for mmgsdi_util_access_path_lookup_table()
01/08/07   sun     Get the AppId from the Client ID for All Pin Operations
12/19/06   tml     Add support for path transparent access
12/11/06   tml     Ensure proper event notification for late registered clients
10/26/06   tml     Get Key ref from client ID
10/25/06   tml     Added function to check if the client is trying to access
                   selected application
10/17/06   sun     Do not wait for Card Selection if NV item is set
09/06/06   tml     Added refresh AID notification support
08/25/06   jar     Added support for FEATURE_MMGSDI_CARD_ERROR_INFO
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/18/06   sun     Added orig_pin_op to mmgsdi_util_build_pin_req
08/07/06   sun     Divert GSDI PIN calls to MMGSDI
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/29/06   sun     Lint Fixes
06/12/06   tml     Externalize functions for session clean-up
05/23/06   tml     lint
05/16/06   tml     Added utility to see if client reg table is free or nots
04/14/06   tml     Added utility function to initialize cache
04/13/06   jar     Added Support for onchip sim
04/05/06   tml     Add NO FILE ENUM and Card Removed support
02/09/06   tml     Added utility functions for conversion and lint fix
02/14/06   nk      Merged for ARR caching and by path changes
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
09/29/05   tml     Client Deregistration support
08/29/05   jar     Created a Memory allocation routine to help reduce the code
                   size by switching from the MACROS
08/26/05   sun     Added support for BT SAP
08/17/05   pv      added mmgsdi_util_print_data_to_diag function to print
                   data to the diag interface
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
06/13/05   tml     changed Msg Error to Msg Medium
02/24/05   sk      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/


#include "uim_variation.h"
#include "uim.h"
#include "mmgsdi.h"
#include "uim_pbm.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#else
#include "uim_msg.h"
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define MMGSDIUTIL_BYTE_SHIFT                      0x08
#define MMGSDIUTIL_MF_HIGH_BYTE                    0x3F
#define MMGSDIUTIL_WORD_SIZE                       4
#define MMGSDI_PERSO_RETURN_DATA_SIZE              0x250
#define MMGSDIUTIL_MNC_2_DIGITS                    0x02
#define MMGSDIUTIL_MNC_3_DIGITS                    0x03
#define MMGSDIUTIL_IMSI_DIGIT_1_OFFSET             0x01
#define MMGSDIUTIL_IMSI_DIGIT_2_OFFSET             0x02
#define MMGSDIUTIL_IMSI_LOW_DIGIT_MASK             0x0F
#define MMGSDIUTIL_MNC_LEN_BYTE                    0x03


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

typedef mmgsdi_service_enum_type mmgsdi_util_find_svr_enum_func_type(
  mmgsdi_file_enum_type file,
  mmgsdi_app_enum_type  app_type);

/* ===========================================================================
   STRUCTURE:      MMGSDI_UIM_FILE_ENUM_TABLE_TYPE

   DESCRIPTION:
     This structure contains the matching pair of a mmgsdi_file_enum_type and
     uim_items_enum_type.
     It also has the mapping that indicates which service type from the
     service table should govern the access of this file.
     If there is multiple possible service for the same file enum, a function
     pointer is provided to enable resolution
===========================================================================*/
typedef PACKED struct PACKED_POST
{
  mmgsdi_file_enum_type                mmgsdi_file;
  uim_items_enum_type                  uim_file;
  mmgsdi_service_enum_type             service_type;
} mmgsdi_uim_file_enum_table_type;

/* Enum type for corresponding DIAG service enabled. */
typedef enum {
  NO_DIAG_ENABLED       = 0x0, /* NV item is there but No Diag support */
  GSDIDIAG_ENABLED      = 0x1, /* Only GSDI Diag is supported */
  UIMDIAG_ENABLED       = 0x2, /* Only UIM Diag is supported */
  GSDI_UIM_DIAG_ENABLED = 0x3, /* GSDI & UIM both Diags are supported */
  DIAG_ERROR            = 0x4  /* No NV item support */
 }mmgsdi_diag_support_e;

/* Enum Type for different MMGSDI/GSDI queue types */
typedef enum {
  GSDI_HIGH_PRI_Q = 0x0,
  GSDI_CMD_Q      = 0x1,
  MMGSDI_CMD_Q    = 0x2,
  MMGSDI_RSP_Q    = 0x3,
  MMGSDI_NO_Q_SVC = 0x7FFFFFFF
} mmgsdi_q_svc_enum_type;

/* ==========================================================================
TYPE ENUM:    MMGSDI_CHNL_MGT_ID_ENUM_TYPE

DESCRIPTION:  Used internally to distinguish the different request types in
              general way when using general functions.
============================================================================*/
typedef enum {
  MMGSDI_CHNL_MGT_ID_NONE,
  MMGSDI_CHNL_MGT_ID_SEND_DATA
} mmgsdi_chnl_mgt_id_enum_type;

/* ==========================================================================
TYPE ENUM:      MMGSDIUTIL_CRIT_SECT_ENUM_TYPE

DESCRIPTION:    This enum denotes types of critical sections
============================================================================*/
typedef enum {
  MMGSDIUTIL_CRIT_SECT_NONE            = 0x0,
  MMGSDIUTIL_CRIT_SECT_FILE_CACHE,
  MMGSDIUTIL_CRIT_SECT_CLIENT_APP_DATA,
  MMGSDIUTIL_CRIT_SECT_TASK_SYNC,
  MMGSDIUTIL_CRIT_SECT_SE13_CACHE,
  MMGSDIUTIL_CRIT_SECT_TASK_STATE
}mmgsdiutil_crit_sect_enum_type;


/*=============================================================================

                          MMGSDI MACROS
               MACRO Definitions used through MMGSDI

=============================================================================*/

/* ===========================================================================
   MACRO:       MMGSDIUTIL_IS_PBM_CLIENT
   DESCRIPTION: Determines whether the client registered at index in
                client_id_reg_table is PBM or not
   INPUT:       index: client id reg table index
    ===========================================================================*/
#define MMGSDIUTIL_IS_PBM_CLIENT(index)                                  \
  (strncmp(PBM_CLIENT_NAME,                                              \
           mmgsdi_client_id_reg_table[index]->client_name,               \
           sizeof(mmgsdi_client_id_reg_table[index]->client_name)) == 0)

/* ===========================================================================
   MACRO:       MMGSDIUTIL_DEBUG_MSG_ERROR_X
   DESCRIPTION: When MMGSDIUTIL_DEBUG_MSG_ERROR is defined, an ERROR_FATAL
                is called so that the Errors can be caught and further
                analyzed.
                These Errors are not logged to NV.
   ===========================================================================*/
#define MMGSDIUTIL_DEBUG_MSG_ERROR_3(str_ptr,a,b,c)        \
  UIM_MSG_ERR_3("MMGSDI: (" str_ptr ")",(a),(b),(c))
#define MMGSDIUTIL_DEBUG_MSG_ERROR_2(str_ptr,a,b)          \
  UIM_MSG_ERR_2("MMGSDI: (" str_ptr ")",(a),(b))
#define MMGSDIUTIL_DEBUG_MSG_ERROR_1(str_ptr,a)            \
  UIM_MSG_ERR_1("MMGSDI: (" str_ptr ")",(a))
#define MMGSDIUTIL_DEBUG_MSG_ERROR_0(str_ptr)              \
  UIM_MSG_ERR_0("MMGSDI: (" str_ptr ")")

/* ===========================================================================-
   MACRO:       MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG
   DESCRIPTION: When FEATURE_UIM_MODEM_HELP_DEBUG is defined, the macro
                logs the error during memory allocation.
   ===========================================================================*/
#ifdef FEATURE_UIM_MODEM_HELP_DEBUG
  #define MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_3(str_ptr,a,b,c)      \
    UIM_MSG_HIGH_3("MMGSDI: (" str_ptr ")",(a),(b),(c))
  #define MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_2(str_ptr,a,b)        \
    UIM_MSG_HIGH_2("MMGSDI: (" str_ptr ")",(a),(b))
  #define MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_1(str_ptr,a)          \
    UIM_MSG_HIGH_1("MMGSDI: (" str_ptr ")",(a))
  #define MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_0(str_ptr)            \
    UIM_MSG_HIGH_0("MMGSDI: (" str_ptr ")")
#else
  #define MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_3(str_ptr,a,b,c)
  #define MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_2(str_ptr,a,b)
  #define MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_1(str_ptr,a)
  #define MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_0(str_ptr)
#endif /* FEATURE_UIM_MODEM_HELP_DEBUG */

/* ===========================================================================
   MACRO:       MMGSDI_MALLOC
   DESCRIPTION: Use mmgsdi_malloc() function only for Off-target
   ===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  #define MMGSDI_MALLOC(size) modem_mem_calloc(1, size, MODEM_MEM_CLIENT_UIM)
#endif

/* ===========================================================================
   MACRO:       MMGSDI_FREE
   DESCRIPTION: Use mmgsdi_free() function only for Off-target
   ===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  #define MMGSDI_FREE(p_mem) modem_mem_free((void*)(p_mem), MODEM_MEM_CLIENT_UIM)
#endif

/* ===========================================================================
   MACRO:       MMGSDIUTIL_TMC_MEM_FREE
   DESCRIPTION: Will Free the TMC Memory and peform the necessary checks for
                NULL.  It will also assign the Pointer to NULL.
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_TMC_MEM_FREE) */
#define MMGSDIUTIL_TMC_MEM_FREE(p_mem)                                 \
  if ( (p_mem) != NULL )                                               \
  {                                                                    \
    MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_1("Freeing memory 0x%x",           \
                                     (p_mem));                         \
    MMGSDI_FREE((p_mem));                                              \
    (p_mem) = NULL;                                                    \
  }                                                                    \
  else                                                                 \
  {                                                                    \
    MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_0("Attempt to free NULL PTR");     \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_TMC_MEM_FREE_NULL_OK
   DESCRIPTION: Will Free the TMC Memory and peform the necessary checks for
                NULL.  It will not Error or print out a message if the
                pointer is NULL.
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_TMC_MEM_FREE_NULL_OK) */
#define MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(p_mem)               \
  if ( (p_mem) != NULL )                                     \
  {                                                          \
    MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_1("Freeing memory 0x%x", \
                                     (p_mem));               \
    MMGSDI_FREE((p_mem));                                    \
    (p_mem) = NULL;                                          \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_NULL
   DESCRIPTION: Will check the parameter provided and return
                MMGSDI_INCORRECT_PARAMS if the pointer is NULL
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_RETURN_IF_NULL) */
#define MMGSDIUTIL_RETURN_IF_NULL(p_var)                             \
  if ( (p_var) == NULL  )                                            \
  {                                                                  \
    return MMGSDI_INCORRECT_PARAMS;                                  \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_STATUS_IF_NULL
   DESCRIPTION: Will check the parameter provided and return
                the given mmgsdi_status.
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_RETURN_STATUS_IF_NULL) */
#define MMGSDIUTIL_RETURN_STATUS_IF_NULL(p_var,mmgsdi_status)        \
  if ( (p_var) == NULL  )                                            \
  {                                                                  \
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("Attempt to use NULL POINTER");     \
    return mmgsdi_status;                                            \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_NULL_2
   DESCRIPTION: Will check the parameters provided and return
                MMGSDI_INCORRECT_PARAMS if any of the pointer is NULL
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_RETURN_IF_NULL_2) */
#define MMGSDIUTIL_RETURN_IF_NULL_2(p_var1, p_var2)                      \
{                                                                        \
  uint8 is_null = 0;                                                     \
  if ((p_var1) == NULL)                                                  \
  {                                                                      \
    is_null = 1;                                                         \
  }                                                                      \
  else if ((p_var2) == NULL)                                             \
  {                                                                      \
    is_null = 2;                                                         \
  }                                                                      \
  if (is_null > 0)                                                       \
  {                                                                      \
    return MMGSDI_INCORRECT_PARAMS;                                      \
  }                                                                      \
}

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_2
   DESCRIPTION: Will check the parameters provided and return
                if any of the pointer is NULL
   INPUT:       p_var1   : pointer to be checked
                p_var2   : pointer to be checked
   ===========================================================================*/
#define MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_2(p_var1, p_var2)        \
{                                                                        \
  uint8 is_null = 0;                                                     \
  if ((p_var1) == NULL)                                                  \
  {                                                                      \
    is_null = 1;                                                         \
  }                                                                      \
  else if ((p_var2) == NULL)                                             \
  {                                                                      \
    is_null = 2;                                                         \
  }                                                                      \
  if (is_null > 0)                                                       \
  {                                                                      \
    return;                                                              \
  }                                                                      \
}

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_NULL_3
   DESCRIPTION: Will check the parameters provided and return
                MMGSDI_INCORRECT_PARAMS if any of the pointer is NULL
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_RETURN_IF_NULL_3) */
#define MMGSDIUTIL_RETURN_IF_NULL_3(p_var1, p_var2, p_var3)          \
{                                                                    \
  uint8 is_null = 0;                                                 \
  if ((p_var1) == NULL)                                              \
  {                                                                  \
    is_null = 1;                                                     \
  }                                                                  \
  else if ((p_var2) == NULL)                                         \
  {                                                                  \
    is_null = 2;                                                     \
  }                                                                  \
  else if ((p_var3) == NULL)                                         \
  {                                                                  \
    is_null = 3;                                                     \
  }                                                                  \
  if (is_null > 0)                                                   \
  {                                                                  \
    return MMGSDI_INCORRECT_PARAMS;                                  \
  }                                                                  \
}

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_3
   DESCRIPTION: Will check the parameters provided and return
                if any of the pointer is NULL
   INPUT:       p_var1   : pointer to be checked
                p_var2   : pointer to be checked
                p_var3   : pointer to be checked
   ===========================================================================*/
#define MMGSDIUTIL_RETURN_WITHOUT_VALUE_IF_NULL_3(p_var1, p_var2, p_var3)    \
{                                                                            \
  uint8 is_null = 0;                                                         \
  if ((p_var1) == NULL)                                                      \
  {                                                                          \
    is_null = 1;                                                             \
  }                                                                          \
  else if ((p_var2) == NULL)                                                 \
  {                                                                          \
    is_null = 2;                                                             \
  }                                                                          \
  else if ((p_var3) == NULL)                                                 \
  {                                                                          \
    is_null = 3;                                                             \
  }                                                                          \
  if (is_null > 0)                                                           \
  {                                                                          \
    return;                                                                  \
  }                                                                          \
}

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_NULL_4
   DESCRIPTION: Will check the parameters provided and return
                MMGSDI_INCORRECT_PARAMS if any of the pointer is NULL
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_RETURN_IF_NULL_4) */
#define MMGSDIUTIL_RETURN_IF_NULL_4(p_var1, p_var2, p_var3, p_var4)  \
{                                                                    \
  uint8 is_null = 0;                                                 \
  if ((p_var1) == NULL)                                              \
  {                                                                  \
    is_null = 1;                                                     \
  }                                                                  \
  else if ((p_var2) == NULL)                                         \
  {                                                                  \
    is_null = 2;                                                     \
  }                                                                  \
  else if ((p_var3) == NULL)                                         \
  {                                                                  \
    is_null = 3;                                                     \
  }                                                                  \
  else if ((p_var4) == NULL)                                         \
  {                                                                  \
    is_null = 4;                                                     \
  }                                                                  \
  if (is_null > 0)                                                   \
  {                                                                  \
    return MMGSDI_INCORRECT_PARAMS;                                  \
  }                                                                  \
}

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_NULL_5
   DESCRIPTION: Will check the parameters provided and return
                MMGSDI_INCORRECT_PARAMS if any of the pointer is NULL
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_RETURN_IF_NULL_5) */
#define MMGSDIUTIL_RETURN_IF_NULL_5(p_var1, p_var2, p_var3, p_var4, p_var5)  \
{                                                                            \
  uint8 is_null = 0;                                                         \
  if ((p_var1) == NULL)                                                      \
  {                                                                          \
    is_null = 1;                                                             \
  }                                                                          \
  else if ((p_var2) == NULL)                                                 \
  {                                                                          \
    is_null = 2;                                                             \
  }                                                                          \
  else if ((p_var3) == NULL)                                                 \
  {                                                                          \
    is_null = 3;                                                             \
  }                                                                          \
  else if ((p_var4) == NULL)                                                 \
  {                                                                          \
    is_null = 4;                                                             \
  }                                                                          \
  else if ((p_var5) == NULL)                                                 \
  {                                                                          \
    is_null = 5;                                                             \
  }                                                                          \
  if (is_null > 0)                                                           \
  {                                                                          \
    return MMGSDI_INCORRECT_PARAMS;                                          \
  }                                                                          \
}

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_NULL_6
   DESCRIPTION: Will check the parameters provided and return
                MMGSDI_INCORRECT_PARAMS if any of the pointer is NULL
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_RETURN_IF_NULL_6) */
#define MMGSDIUTIL_RETURN_IF_NULL_6(p_var1, p_var2, p_var3, p_var4, p_var5, p_var6)  \
{                                                                                    \
  uint8 is_null = 0;                                                                 \
  if ((p_var1) == NULL)                                                              \
  {                                                                                  \
    is_null = 1;                                                                     \
  }                                                                                  \
  else if ((p_var2) == NULL)                                                         \
  {                                                                                  \
    is_null = 2;                                                                     \
  }                                                                                  \
  else if ((p_var3) == NULL)                                                         \
  {                                                                                  \
    is_null = 3;                                                                     \
  }                                                                                  \
  else if ((p_var4) == NULL)                                                         \
  {                                                                                  \
    is_null = 4;                                                                     \
  }                                                                                  \
  else if ((p_var5) == NULL)                                                         \
  {                                                                                  \
    is_null = 5;                                                                     \
  }                                                                                  \
  else if ((p_var6) == NULL)                                                         \
  {                                                                                  \
    is_null = 6;                                                                     \
  }                                                                                  \
  if (is_null > 0)                                                                   \
  {                                                                                  \
    return MMGSDI_INCORRECT_PARAMS;                                                  \
  }                                                                                  \
}

/* ===========================================================================
   MACRO:       ASSERT_OFFTARGET
   DESCRIPTION: Run ASSERT only on Off-Target code
   ===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  #define ASSERT_OFFTARGET(x)
#endif

/* ===========================================================================
   MACRO:       MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
   DESCRIPTION: Allocation of memory according to input size.
                Check if memory allocation is successful or not.
   INPUT:       p_mem   : pointer to be allocated
                ptr_size: size of the allocation
  ===========================================================================*/
#define MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(p_mem, ptr_size)                   \
  ASSERT_OFFTARGET((ptr_size) > 0);                                           \
  (p_mem) = MMGSDI_MALLOC((ptr_size));                                        \
  MMGSDIUTIL_MSG_MEMORY_DEBUG_LOG_2(                                          \
    "Memory allocation: size = 0x%x, ptr = 0x%x", (ptr_size), (p_mem));

/* ===========================================================================
   MACRO:       MMGSDI_MSG_ERROR
   DESCRIPTION: ERROR_FATAL in off target and error print in on target
   ===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  #define MMGSDI_MSG_ERROR_1(str,a)   UIM_MSG_ERR_1(str,a);
  #define MMGSDI_MSG_ERROR_2(str,a,b) UIM_MSG_ERR_2(str,a,b);
#endif

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE
   DESCRIPTION: Will return MMGSDI_INCORRECT_PARAMS if var does not fall
                within [min, max]
   ===========================================================================*/
#define MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(Var,Min,Max)                           \
  if ( ((Var) < (Min)) || ((Var) > (Max)) )                                      \
  {                                                                              \
    MMGSDIUTIL_DEBUG_MSG_ERROR_3("Value 0x%x out of range MIN: 0x%x, MAX: 0x%x", \
                               (Var),(Min),(Max));                               \
    return MMGSDI_INCORRECT_PARAMS;                                              \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_EXCEEDS
   DESCRIPTION: Will check the parameter provided and return
                MMGSDI_INCORRECT_PARAMS the var exceed the maximum allowed
   ===========================================================================*/
#define MMGSDIUTIL_RETURN_IF_EXCEEDS(Var,Max)                                  \
  if ( (Var) > (Max) )                                                         \
  {                                                                            \
    MMGSDIUTIL_DEBUG_MSG_ERROR_2("Greater than MAX: 0x%x > 0x%x, var,max",     \
                                 (Var),(Max));                                 \
    return MMGSDI_INCORRECT_PARAMS;                                            \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_NEGATIVE
   DESCRIPTION: Will check the parameter provided and return
                MMGSDI_INCORRECT_PARAMS if the var underflow
   ===========================================================================-*/
#define MMGSDIUTIL_RETURN_IF_NEGATIVE(Var)                                     \
  if ( (Var) < 0 )                                                             \
  {                                                                            \
    MMGSDIUTIL_DEBUG_MSG_ERROR_1(" Negative var: 0x%x", (Var));                \
    return MMGSDI_INCORRECT_PARAMS;                                            \
  }



/* ===========================================================================
   MACRO:       MMGSDIUTIL_SET_MIN
   DESCRIPTION: Will set var to the lesser of val1 or val2
   ===========================================================================-*/
#define MMGSDIUTIL_SET_MIN(var,val1,val2) \
  if ( (val1) < (val2) )                  \
    (var) = (val1);                       \
  else                                    \
    (var) = (val2);


/* ===========================================================================
   MACRO:       MMGSDIUTIL_CHK_REFRESH_FILE_PTRS
   DESCRIPTION: Validates that only one of the file enum or file path ptr
                is valid.
   ===========================================================================*/
#define MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(file_enum_ptr, file_path_ptr, status)   \
  if((file_enum_ptr != NULL) && (file_path_ptr != NULL))                         \
  {                                                                              \
    UIM_MSG_ERR_2("Both file enum & path not supported enum_p=0x%x path_p=0x%x", \
              file_enum_ptr, file_path_ptr);                                     \
    status = MMGSDI_NOT_SUPPORTED;                                               \
  }                                                                              \
  else                                                                           \
  {                                                                              \
    status = MMGSDI_SUCCESS;                                                     \
  }                                                                              \


/* ===========================================================================-
   MACRO:       MMGSDIUTIL_RETURN_ERR_IF_WORD_ALIGNED_VAR_OUT_OF_BOUNDS
   DESCRIPTION: Checks if the variable is not word aligned
   INPUT:       Param 1 - Non word aligned val, Param 2 - Word Aligned Value
   ===========================================================================-*/
#define MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(non_aligned, aligned)  \
  if(!((aligned == non_aligned) ||                                            \
      (aligned == non_aligned +                                               \
                  (MMGSDIUTIL_WORD_SIZE - non_aligned % MMGSDIUTIL_WORD_SIZE))\
      ))                                                                      \
  {                                                                           \
    return MMGSDI_ERROR;                                                      \
  }                                                                           \
                                                                         \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_MSG_CRIT_SECT_ENTER_DEBUG_LOG
   DESCRIPTION: To print debug msg for entering critical section
   INPUT:       crit_sect_type: critical section type
   ===========================================================================*/
#ifdef FEATURE_UIM_CRIT_SECT_DEBUG
  #define MMGSDIUTIL_MSG_CRIT_SECT_ENTER_DEBUG_LOG(crit_sect_type)            \
    UIM_MSG_HIGH_1("Entering critical section type: 0x%x", crit_sect_type);
#else
  #define MMGSDIUTIL_MSG_CRIT_SECT_ENTER_DEBUG_LOG(crit_sect_type)
#endif/* FEATURE_UIM_CRIT_SECT_DEBUG */

/* ===========================================================================
   MACRO:       MMGSDIUTIL_MSG_CRIT_SECT_LEAVE_DEBUG_LOG
   DESCRIPTION: To print debug msg for leaving critical section
   INPUT:       crit_sect_type: critical section type
   ===========================================================================*/
#ifdef FEATURE_UIM_CRIT_SECT_DEBUG
  #define MMGSDIUTIL_MSG_CRIT_SECT_LEAVE_DEBUG_LOG(crit_sect_type)            \
    UIM_MSG_HIGH_1("Leaving critical section type: 0x%x", crit_sect_type);
#else
  #define MMGSDIUTIL_MSG_CRIT_SECT_LEAVE_DEBUG_LOG(crit_sect_type)
#endif/* FEATURE_UIM_CRIT_SECT_DEBUG */

/* ===========================================================================
   MACRO:       MMGSDIUTIL_ENTER_CRIT_SECT
   DESCRIPTION: To enter rex critical section
   INPUT:       crit_sect_ptr:  critical section pointer
                crit_sect_type: critical section type
   ===========================================================================*/
#define MMGSDIUTIL_ENTER_CRIT_SECT(crit_sect_ptr, crit_sect_type)              \
  MMGSDIUTIL_MSG_CRIT_SECT_ENTER_DEBUG_LOG(crit_sect_type);                    \
  rex_enter_crit_sect(crit_sect_ptr);                                          \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_LEAVE_CRIT_SECT
   DESCRIPTION: To leave rex critical section
   INPUT:       crit_sect_ptr:  critical section pointer
                crit_sect_type: critical section type
   ===========================================================================*/
#define MMGSDIUTIL_LEAVE_CRIT_SECT(crit_sect_ptr, crit_sect_type)              \
  rex_leave_crit_sect(crit_sect_ptr);                                          \
  MMGSDIUTIL_MSG_CRIT_SECT_LEAVE_DEBUG_LOG(crit_sect_type);                    \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT
   DESCRIPTION: To enter rex critical section for client app data
   ===========================================================================*/
#define MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT                             \
  MMGSDIUTIL_ENTER_CRIT_SECT(mmgsdi_client_app_data_crit_sect_ptr,             \
                             MMGSDIUTIL_CRIT_SECT_CLIENT_APP_DATA);            \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT
   DESCRIPTION: To leave rex critical section for client app data
   ===========================================================================*/
#define MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT                             \
  MMGSDIUTIL_LEAVE_CRIT_SECT(mmgsdi_client_app_data_crit_sect_ptr,             \
                             MMGSDIUTIL_CRIT_SECT_CLIENT_APP_DATA);            \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_CHECK_NON_GSDI_TASK_CRIT_SECT_ACCESS
   DESCRIPTION: This macro needs to ERR_FATAL if shared global is accessed
                (read/write access) without acquiring critical section in non
                task context.
                - To be used in utilities where shared globals are read.


   INPUT:       crit_sect_ptr:    critical section pointer
                sect_type:        critical section type
   ===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  #define MMGSDIUTIL_CHECK_NON_GSDI_TASK_CRIT_SECT_ACCESS(crit_sect_ptr, crit_sect_type)
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* ===========================================================================
   MACRO:       MMGSDIUTIL_CHECK_CRIT_SECT_ACCESS
   DESCRIPTION: This macro needs to ERR_FATAL if shared global is accessed
                without acquiring critical section (irrespective of task context)
                - To be used in utilities where shared globals are being written
                into.
   INPUT:       crit_sect_ptr:    critical section pointer
                sect_type:        critical section type
   ===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else
  #define MMGSDIUTIL_CHECK_CRIT_SECT_ACCESS(crit_sect_ptr, crit_sect_type)
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* ===========================================================================
   MACRO:       MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS
   DESCRIPTION: To call ERR_FATAL if shared global protected by client app data
                critical section is accessed without acquiring critical section
                in non task context
   ===========================================================================*/
#define MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS        \
  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CRIT_SECT_ACCESS(                             \
    mmgsdi_client_app_data_crit_sect_ptr,                                      \
    MMGSDIUTIL_CRIT_SECT_CLIENT_APP_DATA);

/* ===========================================================================
   MACRO:       MMGSDIUTIL_CHECK_CLIENT_APP_DATA_CRIT_SECT_ACCESS
   DESCRIPTION: To call ERR_FATAL if shared global protected by client app data
                critical section is accessed without acquiring critical section
   ===========================================================================*/
#define MMGSDIUTIL_CHECK_CLIENT_APP_DATA_CRIT_SECT_ACCESS                      \
   MMGSDIUTIL_CHECK_CRIT_SECT_ACCESS(mmgsdi_client_app_data_crit_sect_ptr,     \
     MMGSDIUTIL_CRIT_SECT_CLIENT_APP_DATA);

/* ===========================================================================
   MACRO:       MMGSDIUTIL_CHECK_TASK_SYNC_CRIT_SECT_ACCESS
   DESCRIPTION: To call ERR_FATAL if task sync critical section is accessed
                without acquiring it
   ===========================================================================*/
#define MMGSDIUTIL_CHECK_TASK_SYNC_CRIT_SECT_ACCESS                            \
   MMGSDIUTIL_CHECK_CRIT_SECT_ACCESS(mmgsdi_task_sync_crit_sect_ptr,           \
     MMGSDIUTIL_CRIT_SECT_TASK_SYNC);

/* ===========================================================================
   MACRO:       MMGSDIUTIL_ENTER_TASK_SYNC_CRIT_SECT
   DESCRIPTION: To enter rex critical section for task synchronization
   ===========================================================================*/
#define MMGSDIUTIL_ENTER_TASK_SYNC_CRIT_SECT                                   \
  MMGSDIUTIL_ENTER_CRIT_SECT(mmgsdi_task_sync_crit_sect_ptr,                   \
                             MMGSDIUTIL_CRIT_SECT_TASK_SYNC);                  \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_LEAVE_TASK_SYNC_CRIT_SECT
   DESCRIPTION: To leave rex critical section for task synchronization
   ===========================================================================*/
#define MMGSDIUTIL_LEAVE_TASK_SYNC_CRIT_SECT                                   \
  MMGSDIUTIL_LEAVE_CRIT_SECT(mmgsdi_task_sync_crit_sect_ptr,                   \
                             MMGSDIUTIL_CRIT_SECT_TASK_SYNC);                  \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT
   DESCRIPTION: To enter rex critical section for mmgsdi task state
   ===========================================================================*/
#define MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT                                   \
  MMGSDIUTIL_ENTER_CRIT_SECT(mmgsdi_task_state_crit_sect_ptr,                   \
                             MMGSDIUTIL_CRIT_SECT_TASK_STATE);                  \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT
   DESCRIPTION: To leave rex critical section for mmgsdi task state
   ===========================================================================*/
#define MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT                                   \
  MMGSDIUTIL_LEAVE_CRIT_SECT(mmgsdi_task_state_crit_sect_ptr,                   \
                             MMGSDIUTIL_CRIT_SECT_TASK_STATE);                  \

/*---------------------------------------------------------------------------
  MACRO:       MMGSDIUTIL_SEND_CMD_CLIENT_DATA
  DESCRIPTION: To build user data for commands sent to UIM
---------------------------------------------------------------------------*/
#define MMGSDIUTIL_SEND_CMD_CLIENT_DATA(mmgsdi_task, index)                    \
                                     ((((uint32)mmgsdi_task & 0xFF) << 8) |    \
                                      (index & 0xFF))                          \

/*---------------------------------------------------------------------------
  MACRO:       MMGSDIUTIL_GET_TASK_FROM_UIMDRV_USER_DATA
  DESCRIPTION: To get task enum from user data. Task is 3rd byte in user_data
---------------------------------------------------------------------------*/
#define MMGSDIUTIL_GET_TASK_FROM_UIMDRV_USER_DATA(user_data)                   \
               (mmgsdi_task_enum_type)((user_data >> 8) & 0xFF)                \

/*---------------------------------------------------------------------------
  MACRO:       MMGSDIUTIL_GET_INDEX_FROM_UIMDRV_USER_DATA
  DESCRIPTION: To get index from user data. Index is 4th byte in user_data
---------------------------------------------------------------------------*/
#define MMGSDIUTIL_GET_INDEX_FROM_UIMDRV_USER_DATA(user_data)                  \
               (uint8)(user_data & 0xFF)                                       \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK
   DESCRIPTION: Gets the current mmgsdi task that is running
   INPUT:       curr_task: pointer to be allocated
    ===========================================================================*/
#define MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(curr_task) \
  *curr_task = mmgsdi_util_get_current_task();         \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_CHECK_CRIT_SECT_OWNER
   DESCRIPTION: This macro needs to ASSERT if mutex is owned by the task
   INPUT:       crit_sect_ptr:    critical section pointer
   ===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_REG_TABLE_INDEX

   DESCRIPTION:
     This function finds the appropriate index from the client id
     and prints the corresponding client name

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_print_client_name(
  mmgsdi_client_id_type    client_id
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_DETERMINE_PROTOCOL

   DESCRIPTION:
     This function determines which card technology (UICC or ICC)

   DEPENDENCIES:
     gsdimain must be finished initialization.

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_protocol_enum_type
     MMGSDI_NO_PROTOCOL   -- No protocol type is associated with the card.
     MMGSDI_ICC           -- ICC
     MMGSDI_UICC          -- UICC

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_protocol_enum_type mmgsdi_util_determine_protocol(
  mmgsdi_slot_id_enum_type      slot_id
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_FREE_RESPONSE_PTR

   DESCRIPTION:
     This function frees the dynamically allocated pointer in the
     mmgsdi cnf type structure

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_free_response_ptr(
  mmgsdi_cnf_type * response_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_CLIENT_REQUEST_TABLE_INDEX_FREE

   DESCRIPTION:
     This function checks if the index location is free or not

   DEPENDENCIES:
     None

   LIMITATIONS:
     The number of entry to the table is limited to the size of the
     mmgsdi task cmd buffer

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          A valid and free index is found.
     MMGSDI_ERROR:            Unable to find a free index.
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_client_request_table_index_free(
  int32    index,
  boolean *is_free_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_FREE_INDEX

   DESCRIPTION:
     This function returns the index location of the client request info table
     that is currently available

   DEPENDENCIES:
     None

   LIMITATIONS:
     The number of entry to the table is limited to the size of the
     mmgsdi task cmd buffer

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          A valid and free index is found.
     MMGSDI_ERROR:            Unable to find a free index.
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_free_index(
  int32 * index_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_POPULATE_CLIENT_REQUEST_TABLE_INFO

   DESCRIPTION:
     This function populate the info to the client request table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Assignment successful
     MMGSDI_ERROR:            Assignment fail.
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_populate_client_request_table_info(
  int32                               index,
  const mmgsdi_request_header_type  * request_hdr_ptr,
  uim_cmd_type                      * uim_cmd_ptr,
  mmgsdi_client_req_extra_info_type * extra_param_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_FREE_CLIENT_REQUEST_TABLE_INFO_INDEX

   DESCRIPTION:
     This function free the client request and uim rsp data pointers
     and set the len of the rsp data data to zero

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_free_client_request_table_index(
  int32 index
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_INFO

   DESCRIPTION:
     This function return pointers to the information in the client request
     table at the corresponding index location.  The pointers to client request
     and uim rsp data returned will be pointing to the location that the
     table information is storing
     data_len is a pointer that needs to be a valid memory location

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Successful retrieval of information
     MMGSDI_ERROR:            Unable to retrieve information
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_info(
  int32                                  index,
  mmgsdi_request_header_type            *request_hdr_ptr,
  uim_cmd_type                         **uim_cmd_pptr,
  mmgsdi_client_req_extra_info_type    **extra_param_pptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_COPY_ACCESS_TYPE

   DESCRIPTION:
     This function copy access type information from one buffer to another.
     This will dynamically allocation memory if required

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_copy_access_type(
  mmgsdi_access_type       *dest_access_ptr,
  const mmgsdi_access_type *src_access_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_USIM_SPEC_VERSION

   DESCRIPTION:
     This function checks the usim specification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_check_usim_spec_version (
  mmgsdi_access_type                 *mmgsdi_access_ptr,
  mmgsdi_usim_spec_version_enum_type  usim_spec_version
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_FILE_OK_IN_SVC_TABLE

   DESCRIPTION:
     This function based on the application available on the card,
     check if the file is allowed by the service table indication or not

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          OK to access the file.
     MMGSDI_ACCESS_DENIED:    Access conditions not satisfied for this EF.
     MMGSDI_NOT_SUPPORTED:    It is not OK to access the file (for reasons
                              other than access violation).

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_file_ok_in_svc_table (
  mmgsdi_client_id_type           client_id,
  mmgsdi_session_id_type          session_id,
  mmgsdi_app_enum_type            app_type,
  const mmgsdi_access_type       *mmgsdi_access_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_QUEUE_MMGSDI_UIM_REPORT_RSP

   DESCRIPTION:
     This function is used to queue a mmgsdi_uim_report_rsp in the event that a
     command is not sent down the the UIM (ex, error before command is sent, or
     a read that comes from cache

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
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_queue_mmgsdi_uim_report_rsp(
  int32                   index,
  mmgsdi_cmd_enum_type    mmgsdi_cmd,
  mmgsdi_return_enum_type error_status
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_POPULATE_CNF_RSP_HEADER

   DESCRIPTION:
     This function populate the confirmation header information

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_populate_cnf_rsp_header(
  const void*                         cnf_ptr,
  mmgsdi_response_header_type       * rsp_hdr_ptr,
  const mmgsdi_request_header_type  * req_hdr_ptr,
  mmgsdi_cnf_enum_type                cnf_cmd,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_SLOT

   DESCRIPTION:
     This function converts the mmgsdi slot type into uim slot type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_uim_slot(
  mmgsdi_slot_id_enum_type  mmgsdi_slot,
  uim_slot_type            *uim_slot_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_GSTK_SLOT

   DESCRIPTION:
     This function converts the mmgsdi slot type into gstk slot type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     gstk_slot_id_enum_type

   SIDE EFFECTS:

==========================================================================*/
gstk_slot_id_enum_type  mmgsdi_util_convert_gstk_slot
(
  mmgsdi_slot_id_enum_type  mmgsdi_slot
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_SEND_CMD_TO_UIM_SERVER

   DESCRIPTION:
     This function populate header information for uim_cmd_type and sends
     the command to uim module

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_cmd_to_uim_server (
  uim_cmd_type *cmd_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_SEND_CMD_TO_UIM_SERVER_SYNCH

   DESCRIPTION:
     This function populate header information for uim_cmd_type and sends
     the command to uim module and wait synchronously for the reply

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_cmd_to_uim_server_synch (
  uim_cmd_type *cmd_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_MORE_PENDING_COMMAND

   DESCRIPTION:
     This function check if there is more pending commands in MMGSDI queue.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_more_pending_command (
  boolean *more_pending_cmd_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CAL_AND_ALLOC_REQ_TYPE_LEN

   DESCRIPTION:
     This function calculates the total memory size required for the command
     type, allocate the memory for the command pointer, and properly assigning
     the pointer of member of the command type if applicable.
     This function also takes care of the word alignment for pointer assignment

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_cal_and_alloc_req_type_len(
  void               **msg_pptr,
  mmgsdi_cmd_enum_type cmd,
  int32                var_len,
  int32               *mem_size_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CAL_ALIGN_SIZE

   DESCRIPTION:
     This function calculates the aligned size

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_cal_align_size(
  int32                orig_size,
  int32               *aligned_size_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_ID_INDEX

   DESCRIPTION:
     This function checks if the client ID is valid and return the index
     location

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_id_index (
  mmgsdi_client_id_type  client_id,
  int32                 *client_index_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_AND_SESSION_INDEX

   DESCRIPTION:
     This function checks if the client ID and session ID provided are valid
     and return the index location for client ID in client registration table
     and session Id in session ID table that is part of client id reg table too

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_and_session_index (
  mmgsdi_session_id_type  session_id,
  int32                 * client_index_ptr,
  int32                 * session_index_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_FREE_SESSION_INDEX

   DESCRIPTION:
    This function finds the first free index in session_info_ptr[] in
    mmgsdi_client_id_reg_table, so that the mmgsdi can provision new
    sessions to the clients.

   DEPENDENCIES:
     None

   LIMITATIONS:
    None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
    None

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_free_session_index (
  int32                 client_index,
  int32                 * session_index_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_ALIGN_MEM

   DESCRIPTION:
     This function calculates the total memory size required for the command
     type, allocate the memory for the command pointer, and properly assigning
     the pointer of member of the command type if applicable.
     This function also takes care of the word alignment for pointer assignment

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_align_mem(
  void                *msg_ptr,
  mmgsdi_cmd_enum_type cmd
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_ALLOC_AND_POPULATE_EXTRA_PARAM

   DESCRIPTION:
     This function malloc the mmgsdi_client_req_extra_info_type pointer and
     populate the data.  If there is any error encountered in this function,
     the function wil free the allocated extra parameter pointer before
     returning error to the calling function

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_alloc_and_populate_extra_param(
  mmgsdi_cmd_enum_type                cmd_type,
  const void                         *cmd_ptr,
  mmgsdi_client_req_extra_info_type **extra_param_pptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_VALID_CLIENT_ID

   DESCRIPTION:
     This function checks if the client ID provided is in the registrating
     table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_valid_client_id (
  mmgsdi_client_id_type client_id
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UIM_REPORT

   DESCRIPTION:
     This function is called when UIM report back command response

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_uim_report (
  uim_rpt_type *report_ptr
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_CHANNEL

   DESCRIPTION:
     This function convert mmgsdi channel type to uim channel type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_uim_channel(
  mmgsdi_channel_id_type mmgsdi_channel,
  uim_channel_type      *uim_channel_ptr
);

/* ===========================================================================
     FUNCTION:      MMGSDI_UTIL_QUEUE_MMGSDI_SAP_REPORT_RSP

     DESCRIPTION:
       This function is used to queue a mmgsdi_sap_report_rsp in the event that a
       command is not sent down the the BT

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
  ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_queue_mmgsdi_sap_report_rsp(
  mmgsdi_slot_id_enum_type slot,
  int32                    index,
  mmgsdi_return_enum_type  cmd_status
);

/* ===========================================================================
     FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_INDEX

     DESCRIPTION:
       This function is used to retrieve the index of the request type from the
       client table

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
  ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_index(
  int32* index_ptr,
  mmgsdi_cmd_enum_type request_type
);

/*===========================================================================
FUNCTION MMGSDI_UTIL_PRINT_DATA_TO_DIAG

DESCRIPTION
  Generic Function used to print any data out onto diag.  The function takes
  a length and a data pointer and prints the data in hex format.  This
  function breaks each line to approximately 70 chars, so that the data
  would not be missed out when looking at the diag messages.

PARAMETERS
message: Message to be prefixed before each set of data
req_data: uint8 pointer to the data to be printed in hex
req_data_len: Length in bytes of the data that needs to be printed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_util_print_data_to_diag(
  char*        message,
  const uint8* req_data,
  uint16       req_data_len
);

/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_DATA_LEN_VALID

DESCRIPTION
  This function is used to verify the length of the data meets the following
  requirements:

  1. Data must be greater than 0
  2. Data must be less than GSDI_MAX_DATA_BLOCK_LEN

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  data_len is valid for configuration
            FALSE: data_len is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_data_len_valid(
  int32 data_len
);

/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_DATA_BUFFER_VALID

DESCRIPTION
  This function is used to verify the buffer used is valid.

  1. Buffer Pointer Can not be NULL

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  data_buffer is valid for request
            FALSE: data_buffer is not valid for request

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_data_buffer_valid(
  const byte * data_buffer
);

/*===========================================================================
FUNCTION MMGSDI_UTIL_IS_SLOT_VALID

DESCRIPTION
  This function is used to verify the slot id received in the message.
  All channel management procedures require that the correct slot
  is provided in the request.

  The only acceptable values are:
  MMGSDI_SLOT_1 -> 0x01
  MMGSDI_SLOT_2 -> 0x02
  MMGSDI_SLOT_3 -> 0x03

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  Slot is valid for configuration
            FALSE: Slot is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_util_is_slot_valid(
  mmgsdi_slot_id_enum_type slot
);

/*===========================================================================
FUNCTION MMMGSDI_UTIL_VALIDATE_ONCHIP_CONFIG

DESCRIPTION
  This function is used to ensure all components of the TLV DATA are
  properly set.  This is a validation function only.


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_validate_onchip_config(
  const   mmgsdi_onchip_sim_config_data_type * onchip_config_data_ptr,
  mmgsdi_onchip_mode_enum_type                 onchip_sim_mode
);

/* ============================================================================
  FUNCTION:      MMGSDI_UTIL_SEND_REFRESH_EVT_FOR_ONCHIP_ACT

  DESCRIPTION:
    This function is called to send out the REFRESH RESET events to clients
    before the onchip acitvation is started.

  DEPENDENCIES:
    None

  LIMITATIONS:
    Should be used only for sending out the REFRESH RESET or RESET_AUTO events

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
============================================================================*/
void mmgsdi_util_send_refresh_evt_for_onchip_act(
  mmgsdi_event_data_type   * evt_info_ptr,
  mmgsdi_slot_id_enum_type   mmgsdi_slot
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CARD_SESSION_CLEANUP

   DESCRIPTION:
     This function cleans up session information for the card session

   DEPENDENCIES:
     None

   LIMITATIONS:
    None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to session info ptr, app info ptr and channel info ptr.
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_card_session_cleanup(
  mmgsdi_session_info_type *session_info_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_PROV_SESSION_DEACTIVATE_CLEANUP_AND_NOTIFY

   DESCRIPTION:
     This function cleans up session information of all sessions linked
     to the provisioning application being deactivated and notifies the
     sessions

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to prov indices.
==========================================================================*/
void mmgsdi_util_prov_session_deactivate_cleanup_and_notify(
  mmgsdi_session_id_type session_id
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_SESSION_CLOSE_CLEANUP_AND_NOTIFY

   DESCRIPTION:
     This function cleans up the session and notifies the clients.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
==========================================================================*/
void mmgsdi_util_session_close_cleanup_and_notify(
  int32 client_index,
  int32 session_index
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CLOSE_ALL_SESSIONS_CLEANUP_AND_NOTIFY

   DESCRIPTION:
     This function goes through all the entries in the client id reg table,
     perform cleanup and notify the corresponding client

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_close_all_sessions_cleanup_and_notify(
  mmgsdi_slot_id_enum_type slot_id);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_IS_DEFAULT_CHANNEL_IN_USE_UICC_APPS

   DESCRIPTION:
     This function goes through all the entries in the client id reg table,
     and verifies if there are any UICC apps open on the slot specified which are
     on the default channel

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/

boolean mmgsdi_util_check_is_default_channel_in_use_uicc_apps(
  mmgsdi_slot_id_enum_type slot_id);

/*===========================================================================
FUNCTION MMMGSDI_UTIL_IS_ARAM_AID

DESCRIPTION
  This function is used to find out if the input AID is an ARA-M AID or not.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_aram_aid(
  const mmgsdi_aid_type *aid_ptr,
  boolean               *is_aram_aid_ptr);

/*===========================================================================
FUNCTION MMMGSDI_UTIL_CMP_AID

DESCRIPTION
  This function is used to compare two app ids.
  It also provides facility to ignore the APP check for ICC cards.

  Note:
  - For UICC applications, if the length of the passed in AIDs doesn't match,
    an error is returned.
  - If lengths of both AIDs is zero, then Success is returned only if the
     app type of the 2 AIDs match and is valid.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS
                            MMGSDI_ERROR

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_cmp_aid(
  const mmgsdi_aid_type *aid1_ptr,
  const mmgsdi_aid_type *aid2_ptr,
  boolean                ignore_icc_aid
);

/*===========================================================================
FUNCTION MMMGSDI_UTIL_GET_USIM_APPS

DESCRIPTION
  This function is used to get the number of usim apps.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
uint32 mmgsdi_util_get_usim_apps(mmgsdi_slot_id_enum_type slot);

/*===========================================================================
FUNCTION MMMGSDI_UTIL_BUILD_PIN_REQ

DESCRIPTION
  This function builds the PIN request for VERFIFY & UNBLOCK PIN operations.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_pin_req(
  const mmgsdi_request_header_type  *req_header_ptr,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_pin_enum_type               orig_pin_id,
  mmgsdi_pin_operation_enum_type     pin_op,
  mmgsdi_pin_operation_enum_type     orig_pin_op,
  mmgsdi_protocol_enum_type          protocol,
  const mmgsdi_pin_extra_info_type  *pin_data_ptr,
  const mmgsdi_sw_status_type       *orig_sw_ptr
);

/*===========================================================================
FUNCTION MMMGSDI_UTIL_BUILD_GET_FILE_ATTR_REQ

DESCRIPTION
  This function builds a get file attribute request and sends it to mmgsdi.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_get_file_attr_req(
  const mmgsdi_request_header_type  *req_header_ptr,
  mmgsdi_access_type                 access_type,
  mmgsdi_pin_extra_info_type        *pin_info_ptr
);

/*===========================================================================
FUNCTION MMMGSDI_UTIL_GET_KEY_REF

DESCRIPTION
  This function gets the chv based on the PIN ID and protocol
DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_retrun_enum_type
SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_util_get_key_ref(
  mmgsdi_client_id_type     client_id,
  mmgsdi_pin_enum_type      pin,
  uim_chv_type             *chv
);

/* ===========================================================================
FUNCTION MMGSDI_UTIL_INTERNAL_CB

DESCRIPTION
  This function, defined as mmgsdi_callback_type, will handle the internal
  commands generated by MMGSDI

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void mmgsdi_util_internal_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_UIM_CHV

DESCRIPTION
  This function converts mmgsdi key reference values to UIM CHV type

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  uim_chv_type

SIDE EFFECTS
  NONE
==========================================================================*/
uim_chv_type mmgsdi_util_convert_to_uim_chv(
   mmgsdi_key_ref_values_enum_type key_ref
);

/* ===========================================================================
FUNCTION MMGSDI_UTIL_FIND_APP_PIN_INDEX

DESCRIPTION
  This function finds the index into one of the foolowing tables -
  1. mmgsdi_app_pin1_table
  2. mmgsdi_app_pin2_table
  3. mmgsdi_app_univ_pin_table

  It first finds if the key reference already exists in the table,
  If  not, then it looks for the first empty slot.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
 mmgsdi_return_enum_type mmgsdi_util_find_app_pin_index(
   const mmgsdi_app_pin_info_type  **app_pin_table_pptr,
   uint8                             app_pin_table_size,
   mmgsdi_key_ref_values_enum_type   key_ref,
   uint32                           *index
);

/* ===========================================================================
FUNCTION MMGSDI_UTIL_RESET_APP_DATA

DESCRIPTION
  This function resets all the app related data including PIN

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

CRITICAL SECTIONS:
  The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
  to data slot ptr, app info ptr and pin info ptr.
===========================================================================*/
void mmgsdi_util_reset_app_data(
  boolean                  reselect_app,
  mmgsdi_slot_id_enum_type slot_id
);


/* ===========================================================================
FUNCTION MMGSDI_UTIL_RESET_APP_DATA_INIT

DESCRIPTION
  This function resets all the app related data including PIN
  This util do not have critical section protection and used only
  during task initialization.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

CRITICAL SECTIONS:
  The calling function must enter mmgsdi_client_app_data_crit_sect_ptr crit
  sect before invoking this util function. Protection for data slot ptr,
  app info ptr and pin info ptr.
===========================================================================*/
void mmgsdi_util_reset_app_data_init(
  boolean                  reselect_app,
  mmgsdi_slot_id_enum_type slot_id
);


/* ===========================================================================
FUNCTION MMGSDI_UTIL_SET_PIN_INFO

DESCRIPTION
  This function sets the pin info from the src pin info
DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void mmgsdi_util_set_pin_info(
  mmgsdi_pin_info_type           *dest,
  const mmgsdi_app_pin_info_type *src
);

/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_CB

DESCRIPTION
  This function, defined as mmgsdi_callback_type, will handle the internal
  commands  generated by MMGSDI

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
==========================================================================*/
void mmgsdi_util_gsdi_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_STORE_PIN

DESCRIPTION
  This function, will store the pin for the PIN ID.

DEPENDENCIES
  None

LIMITATIONS
  The function should be called to store only valid PIN IDs.

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
==========================================================================*/
void mmgsdi_util_store_pin(
  mmgsdi_int_app_info_type      *sel_app_ptr,
  mmgsdi_pin_enum_type           pin_id,
  mmgsdi_static_data_type        pin_data
);

/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_FIND_AID_INDEX

   DESCRIPTION:
     This function returns an index location to the mmgsdi_slot_data_type
     if a matching AID is found.

     Note:
     - if the length of the AIDs doesn't match,
    it is not considered a match.
     - If lengths of both AIDs is zero, then it is considered a match only if
       the app type of the AID in the app table is valid.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     uint32: index location of the AID
             MMGSDI_MAX_EXT_APP_INFO if not found

   SIDE EFFECTS:
     None
============================================================================*/
uint32 mmgsdi_util_find_aid_index(
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_aid_type          aid
);

/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_IS_VALID_AID_INDEX

   DESCRIPTION:
     This function returns whether the AID index is valid or not

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     TRUE if valid index
     FALSE if invalid index

   SIDE EFFECTS:
     None
============================================================================*/
boolean mmgsdi_util_is_valid_aid_index(
  uint32 aid_index
);


/*===========================================================================
FUNCTION MMMGSDI_UTIL_CAN_PROCEED_TO_POST_PIN1

DESCRIPTION
  This function checks the pin1 and univ pin status and the replacement
  condition to determine if post pin1 procedure is allowed or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Can proceed with Post PIN1 process
  FALSE: Cannot proceed with Post PIN1 process

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_util_can_proceed_to_post_pin1(
  const mmgsdi_int_app_info_type *sel_app_ptr
);

/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_ACTIVATE_FIRST_APP

   DESCRIPTION:
     This function Selects the first SIM/USIM or/and CSIM/RUIM app from the list of apps
     available

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
============================================================================*/
void mmgsdi_util_activate_first_app(
  mmgsdi_slot_id_enum_type slot
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_RESET_PIN_STATUS

   DESCRIPTION:
     This function resets the pin status

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
==========================================================================*/
void mmgsdi_util_reset_pin_status(
  mmgsdi_int_app_info_type *sel_app_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_APP_PIN_CLEANUP

   DESCRIPTION:
     This function verifies if no other App maps to the same PIN1, and if not cleans-up the PIN1 status.
     Also cleans-up the PIN2 status.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_app_pin_cleanup(
  mmgsdi_int_app_info_type *sel_app_ptr,
  mmgsdi_slot_id_enum_type  slot_id
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_SEL_APP_PIN_CACHED

   DESCRIPTION:
     This function checks to see if selected app's pin is cached

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

   SIDE EFFECTS:

==========================================================================*/
boolean mmgsdi_util_is_sel_app_pin_cached(void);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_FILE_ATTR

   DESCRIPTION:
     This function checks the file attributes

   DEPENDENCIES:
     gsdi_ef_access_conditions_check

   LIMITATIONS:
     Checks are Limited to Read and write only, for all others, it will return
     SUCCESS

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     File Read/Write will be blocked if the security conditions have not
     been met.

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_check_file_attr(
  mmgsdi_client_id_type           client_id,
  mmgsdi_session_id_type          session_id,
  mmgsdi_slot_id_enum_type        slot,
  const mmgsdi_int_app_info_type *prov_app_info_ptr,
  const mmgsdi_access_type       *access_ptr,
  mmgsdi_cmd_enum_type            cmd_type,
  boolean                         skip_uicc_arr
);

/*===========================================================================
   FUNCTION mmgsdi_util_is_pin_valid

   DESCRIPTION:
     This function will check if the pin meets at least the minimum length
     requirement and has valid data

   PARAMS:
    pin_ptr:          Will contain the code of the PIN.
    pin_len:          Will contain the length of the PIN.

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
      boolean:  TRUE  PIN is valid
                FALSE PIN is not valid

   SIDE EFFECTS:

 ===========================================================================*/
boolean mmgsdi_util_is_pin_valid(
  const uint8    *pin_ptr,
  mmgsdi_len_type pin_len
);

/*===========================================================================
   FUNCTION mmgsdi_util_is_hiddenkey_from_card_valid

   DESCRIPTION:
     This function checks if the Hidden Key data retrieved from
     the card is valid

   PARAMS:
    hiddenkey_data_ptr: Contains the Hidden Key length and data
                        retrieved from the card

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
      boolean:  TRUE  Hidden Key data is valid
                FALSE Hidden Key data is not valid

   SIDE EFFECTS:
     None
 ===========================================================================*/
boolean mmgsdi_util_is_hiddenkey_from_card_valid(
  const mmgsdi_data_type * hkey_data_ptr);

/*===========================================================================
   FUNCTION mmgsdi_util_is_hiddenkey_valid

   DESCRIPTION:
     This function checks if the Hidden Key data is valid
     (This is the ASCII format input Hiddenkey provided by clients)

   PARAMS:
    hkey_data_ptr: Contains the Hidden Key length and data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
      boolean:  TRUE  Hidden Key data is valid
                FALSE Hidden Key data is not valid

   SIDE EFFECTS:
     None
 ===========================================================================*/
boolean mmgsdi_util_is_hiddenkey_valid(
  const mmgsdi_data_type * hkey_data_ptr);

/*===========================================================================
   FUNCTION mmgsdi_util_convert_hiddenkey

   DESCRIPTION:
     This function converts the ASCII Hiddenkey input to the card format

   PARAMS:
    hiddenkey_data_ptr: Contains the Hidden Key length and data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_hiddenkey(
  const mmgsdi_static_data_type * hkey_ptr,
  mmgsdi_static_data_type       * hkey_card_ptr);

/*===========================================================================
   FUNCTION mmgsdi_util_verify_hiddenkey

   DESCRIPTION:
     This function verifies the ASCII Hiddenkey input against
     the Hiddenkey data from the card

   PARAMS:
    hiddenkey_data_ptr: Contains the Hidden Key length and data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_verify_hiddenkey(
  mmgsdi_session_id_type          session_id,
  const mmgsdi_static_data_type * hkey_ptr);

/*===========================================================================
   FUNCTION MMGSDI_UTIL_CMP_AID_GET_SESSION_PTR

   DESCRIPTION:
     This function verifies if the Session correspoding to the Session
     and Client index passed points to the same app that has been passed in.
     If so the Session ptr for the Session is returned.
     Function also provides facility to ignore the APP check for ICC card.

   PARAMS:

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
mmgsdi_session_info_type*  mmgsdi_util_cmp_aid_get_session_ptr(
  uint8                     client_index,
  uint8                     session_index,
  const mmgsdi_aid_type   * aid_ptr,
  mmgsdi_slot_id_enum_type  slot_id,
  boolean                   ignore_icc_aid
);

/*===========================================================================
   FUNCTION mmgsdi_util_get_ber_length

   DESCRIPTION:
     This function returns the ber length and sets the prefix for a ber '
     TLV data.
   PARAMS:
     orig_ber_len : as calcualted by the mmgsdi
     orig_data_len: as given by the client
     ber_data_ptr:  Array that will contain the BER Length TLV

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
uint32 mmgsdi_util_get_ber_length(
  uint32 orig_ber_len,
  uint32 orig_data_len,
  uint8  *ber_data_ptr
);

/*===========================================================================
   FUNCTION mmgsdi_util_print_file_information

   DESCRIPTION:
     This function print file information.
   PARAMS:
     file_access: File Information
     is_error_msg: boolean variable to print message as MSG_ERROR.

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
     None
 ===========================================================================*/
void mmgsdi_util_print_file_information (
  mmgsdi_access_type file_access,
  boolean is_error_msg
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_REFRESH_FILE_PATHS

DESCRIPTION
  This function shall accept byte buffer that has list of file paths.
  Each file path begins from MF, hence byte value 0x 3F shall be used
  as a delimiter. The individual file paths shall be stored in the
  array of mmgsdi_path_type. For example
  Input buffer: 0x02 0x3F 0x00 0x2F 0x02 0x3F 0x00 0x7F 0x10 0x6F 0xD2
                 |
                First byte is number of files
  Should give output -
  file_path_pptr[0].path_buf = {0x3F 0x00 0x2F 0x02}
  file_path_pptr[0].path_len = 2
  file_path_pptr[1].path_buf = {0x3F 0x00 0x7F 0x10 0x6F 0xD2}
  file_path_pptr[1].path_len = 3
  num_files = 2

DEPENDENCIES
  None

LIMITATIONS
  The function shall only consider the number of files as mentioned in the
  first byte of the input data_ptr. If the actual buffer has more files than
  the number mentioned in the first byte, the logic in the function will
  ignore the rest of the files.

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  The calling function should free the memory allocated to file_path_pptr.
----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_util_convert_to_refresh_file_paths(
  uint32                 data_len,
  const uint8            *data_ptr,
  mmgsdi_path_type       **file_path_pptr,
  uint8                  *num_files_ptr
);

/*===========================================================================
  FUNCTION mmgsdi_util_malloc_and_copy_file_list_ptr

  DESCRIPTION:
    This function shall allocate memory for the destination file list ptr
    and will copy file list ptr from source

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
    MMGSDI_SUCCESS                     : Successful file list copy operation
    MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED : Malloc Error
    MMGSDI_INCORRECT_PARAMS            : If input parameters are out of range

  SIDE EFFECTS:
    The allocated memory must be freed after use
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_malloc_and_copy_file_list_ptr(
  mmgsdi_file_enum_type    **dst_file_list_ptr,
  mmgsdi_file_enum_type    *src_file_list_ptr,
  uint32                   num_files
);

/*===========================================================================
  FUNCTION mmgsdi_util_malloc_and_copy_file_path_array

  DESCRIPTION:
    This function shall allocate memory for the destination file path
    array and will copy file paths from source array

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
    MMGSDI_SUCCESS                     => Successful file copy operation
    MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED => Malloc Error
    MMGSDI_ERROR                       => For all other failure scenarios


  SIDE EFFECTS:
    The calling function should free the allocated memory
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_malloc_and_copy_file_path_array(
  mmgsdi_path_type           **dst_file_path_pptr,
  mmgsdi_path_type           *src_file_path_ptr,
  uint32                     num_files
);

/*===========================================================================
FUNCTION MMMGSDI_UTIL_GET_CSIM_APPS

DESCRIPTION
  This function is used to get the number of CSIM apps.

DEPENDENCIES
  None

RETURN VALUE
  uint32: Number of CSIM Apps present

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 mmgsdi_util_get_csim_apps(mmgsdi_slot_id_enum_type slot);

/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_CARD_ICC

DESCRIPTION
  This function determines if the card is ICC or UICC

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  MMGSDI_SUCCESS: If the card type can be determined
  MMGSDI_ERROR:   If the card type cannot be determined

SIDE EFFECTS
  None
============================================================================*/
extern mmgsdi_return_enum_type mmgsdi_util_is_card_icc(
  const mmgsdi_slot_data_type *slot_data_ptr,
  boolean                     *is_icc_ptr);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_PROV_SESSION_INFO

DESCRIPTION
  This function takes the session_id and returns the prov_index and the slot
  mapping to that session_id

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  MMGSDI_SUCCESS: If the session_id passed in can resolved in a provisioning
                  session entry
  MMGSDI_ERROR:   If the session_id passed in cannot resolved in a provisioning
                  session entry

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_prov_session_info(
  mmgsdi_session_id_type         session_id,
  int32                         *prov_index_ptr,
  mmgsdi_slot_id_enum_type      *slot_ptr,
  mmgsdi_int_app_info_type     **int_app_info_pptr);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_SESSION_APP_INFO

DESCRIPTION
  This function takes the session_id and returns the slot and the app info
  associated with that session_id

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  MMGSDI_SUCCESS: If the session_id passed in can resolved in a provisioning
                  session entry
  MMGSDI_ERROR:   If the session_id passed in cannot resolved in a provisioning
                  session entry

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_session_app_info(
  mmgsdi_session_id_type         session_id,
  mmgsdi_slot_id_enum_type      *slot_ptr,
  mmgsdi_channel_id_type        *channel_id_ptr,
  mmgsdi_int_app_info_type     **int_app_info_pptr,
  mmgsdi_channel_info_type     **channel_info_pptr,
  mmgsdi_session_info_type     **session_info_pptr,
  uint32                        *app_index_ptr);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_SLOT_INDEX

DESCRIPTION
  This function bases on the MMGSDI slot id passed in, populates the slot index
  for the slot, app, pin & upin tables

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  MMGSDI_SUCCESS if a valid slot index is available else return  MMGSDI_ERROR

SIDE EFFECTS
  None
============================================================================*/

mmgsdi_return_enum_type mmgsdi_util_get_slot_index(
  mmgsdi_slot_id_enum_type  mmgsdi_slot,
  uint8                    *slot_index);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_SLOT_DATA_PTR

DESCRIPTION
  This function bases on the MMGSDI slot id passed in, return the pointer to the
  slot_data pointer

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  slot specific pointer or NULL if it is
  invalid slot id

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_slot_data_type* mmgsdi_util_get_slot_data_ptr(
  mmgsdi_slot_id_enum_type mmgsdi_slot);

/*===========================================================================
FUNCTION: MMGSDI_UTIL_GET_SESSION_TYPE

DESCRIPTION
  Based on the session id, this function will return the session type, and
  the aid info

DEPENDENCIES
  none

RETURN VALUE
  Success/failure

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_session_type(
  mmgsdi_session_id_type         session_id,
  mmgsdi_session_type_enum_type *session_type_ptr,
  mmgsdi_aid_type               *aid_ptr);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_FREE_CHANNEL_INDEX

   DESCRIPTION:
    This function finds the first free index in mmgsdi_channel_info_table_ptr
    so that a new channel can be provisioned at the free index

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_free_channel_index (
  int32                 * channel_index_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_MATCH_CHANNEL_INDEX

   DESCRIPTION:
    This function determines if the channel id provided is already allocated
    and indexed in the channel info table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_match_channel_index (
  const mmgsdi_slot_id_enum_type  slot,
  const mmgsdi_channel_id_type    channel_id,
  int32                          *channel_index_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_SESSION_ID_VALID

   DESCRIPTION:
      This function from input session id verifies if session id has been
      in use and also in case if the session id is mapped to a provisioning
      application then it tells the status of the provision app if activated
      or not.

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_session_id_status_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_session_id_status_enum_type  mmgsdi_util_is_session_id_valid (
  mmgsdi_session_id_type session_id
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_APP_PIN_INFO_TBL_PTR

DESCRIPTION
  This function bases on the MMGSDI slot id passed in, return the pointer to
  the mmgsdi_app_pin_info_table

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  slot specific pointer in mmgsdi_app_pin_table_ptr
  or NULL if it is invalid slot id

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_int_app_info_type** mmgsdi_util_get_app_pin_info_tbl_ptr(
  mmgsdi_slot_id_enum_type mmgsdi_slot);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_APP_PIN1_INFO_TBL_PTR

DESCRIPTION
  This function bases on the MMGSDI slot id passed in, return the pointer to
  the mmgsdi_app1_pin_info_table. It also returns the number of elements in the
  table as out parameter

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
   slot specific pointer in mmgsdi_app1_pin_info_table
   or NULL if it is invalid slot id

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_app_pin_info_type** mmgsdi_util_get_app_pin1_info_tbl_ptr(
  uint8                       *pin1_info_tbl_size_ptr,
  mmgsdi_slot_id_enum_type     mmgsdi_slot);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_UNIV_PIN_TBL_PTR

DESCRIPTION
  This function bases on the MMGSDI slot id passed in, return the pointer
  to the mmgsdi_app_univ_pin_table_ptr

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  univ pin tbl  pointer in mmgsdi_app_univ_pin_table_ptr or NULL
  if it is invalid slot id

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_app_pin_info_type* mmgsdi_util_get_univ_pin_tbl_ptr(
  mmgsdi_slot_id_enum_type mmgsdi_slot);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_APP_INDEX_FOR_APP

DESCRIPTION
  This function returns the app index of the application passed.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
   MMGSDI_SUCCESS if app is found and app_index
   MMGSDI_NOT_FOUND if app is not found

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_app_index_for_app(
  const mmgsdi_aid_type          *aid_ptr,
  const mmgsdi_slot_id_enum_type  slot_id,
  uint8                    *app_index_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_SESSION_AND_CHANNEL_INFO

   DESCRIPTION:
      Takes session ID as input and returns matching session info ptr and channel
      ptr for the session Id.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_session_and_channel_info(
  mmgsdi_session_id_type       session_id,
  mmgsdi_session_info_type     **session_info_pptr,
  mmgsdi_channel_info_type     **channel_info_pptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_PIN_CACHED

   DESCRIPTION:
     This function checks to see if selected app's pin is cached

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

   SIDE EFFECTS:

==========================================================================*/
boolean mmgsdi_util_is_pin_cached(
  const mmgsdi_int_app_info_type *app_info_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CH_ZERO_APP

   DESCRIPTION:
     This function checks if the there is a Session open for GW and a channel
     assigned to it or a session open for CSIM and a channel assigned to it
     If any one of them is assigned channel 0, it will return app information
     and session id for that session.

   DEPENDENCIES:
     Requires a Channel 0 to be opened before this function is called

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_ch_zero_app(
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_int_app_info_type **app_info_pptr,
  mmgsdi_session_id_type    *session_id_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_SESSION_INFO_FROM_APP

   DESCRIPTION:
     This function will return info on first matching session that is mapped
     to the given application

   DEPENDENCIES:
     None

   LIMITATIONS:
     Function returns on "first" match

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_session_info_from_app(
  const mmgsdi_aid_type           *app_data_ptr,
  mmgsdi_slot_id_enum_type         slot_id,
  mmgsdi_session_info_type       **session_info_pptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_ID

   DESCRIPTION:
     This function will take session id as input and it shall return reference
     to client id

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_id(
  mmgsdi_session_id_type      session_id,
  mmgsdi_client_id_type     * client_id_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_PROV_SESSION

DESCRIPTION
  This function takes the session_type as input and returns whether
  the Session is a provisioning session or not.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean:  TRUE : Provisioning session
            FALSE: Non-provisioning session

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_prov_session(
  mmgsdi_session_type_enum_type session_type
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_NON_PROV_SESSION

DESCRIPTION
  This function takes the session_type as input and returns whether
  the Session is a non-provisioning session or not.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean:  TRUE : Non-provisioning session
            FALSE: Other sessions

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_non_prov_session(
  mmgsdi_session_type_enum_type session_type
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_CARD_SESSION

DESCRIPTION
  This function takes the session_type as input and returns whether
  the session is a card session or not.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean:  TRUE : Card sessions
            FALSE: Other sessions

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_card_session(
  mmgsdi_session_type_enum_type session_type
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_VALID_ACCESS

DESCRIPTION
  This function takes the session_type and MMGSDI File enum and determines
  if the file access is valid for the session type

DEPENDENCIES
  None

LIMITATIONS
  Supports only Provisioning sessions

RETURN VALUE
  boolean:  TRUE : Valid access
            FALSE: Invalid access

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_valid_access(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_file_enum_type         mmgsdi_file
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_MEID_AVAILABILITY

DESCRIPTION
  This function will check the MEID availability of given session.

DEPENDENCIES
  None

LIMITATIONS
  Supports only Provisioning sessions

RETURN VALUE
  boolean:  TRUE : When APP and ME both have MEID capabilty
            FALSE: When either ME or APP does not have MEID support

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_get_meid_availability(
  mmgsdi_session_id_type       session_id
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_DISCARD_CB

   DESCRIPTION:
     This function discards the callback from MMGSDI

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
==========================================================================*/
void mmgsdi_util_discard_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_ALLOC_COPY_UIM_RSP

   DESCRIPTION:
     This function will allocate memory and copy the UIM response info

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type  mmgsdi_util_alloc_copy_uim_rsp(
  mmgsdi_uim_report_rsp_type       **output_uim_rsp_pptr,
  const mmgsdi_uim_report_rsp_type  *input_uim_rsp_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_PROV_APP

   DESCRIPTION:
     This function verifies if the requested application is activated on
     one of the provisioning Sessions

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_prov_app(
  const mmgsdi_aid_type         *aid_ptr,
  mmgsdi_slot_id_enum_type       slot_id,
  boolean                       *is_prov_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_APP_ACTIVATION_IN_PROGRESS

DESCRIPTION
  This function checks if the app activation of the requested app is in
  progress

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_app_activation_in_progress(
  mmgsdi_int_app_info_type *int_app_info_ptr,
  boolean                  *act_in_progress);


/* ==========================================================================
  FUNCTION MMGSDI_UTIL_UICC_HIDDENKEY_EF_WRITE_CB

  DESCRIPTION
    This function will handle Hiddenkey write CNF geneted by MMGSDI

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
=============================================================================*/
void mmgsdi_util_uicc_hidddenkey_ef_write_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

/* ==========================================================================
  FUNCTION MMGSDI_UTIL_GET_UIM_PROTOCOL

  DESCRIPTION
    This function returns the UIM card protocol for the slot

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    uim_protocol_type

  SIDE EFFECTS
    None
=============================================================================*/
uim_protocol_type mmgsdi_util_get_uim_protocol(
  mmgsdi_slot_id_enum_type slot_id
);

/* ============================================================================
   FUNCTION:      mmgsdi_util_check_file_access_data

   DESCRIPTION:
     This function check the file access parameter for given session id.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_SUCCESS: when file access data is valid.
     MMGSDI_INCORRECT_PARAMS: when file access data is not valid.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_check_file_access_data(
  mmgsdi_session_id_type         session_id,
  mmgsdi_access_type             file_access,
  mmgsdi_protocol_enum_type      protocol);

/* ============================================================================
FUNCTION mmgsdi_util_check_activation_status

DESCRIPTION
  This function checks the activation status of the app.
  For CSIM/RUIM: Activation status would be ACTIVATED if IMSI_M is programmed.
  For USIM/SIM:  Activation status is not applicable to USIM/SIM.
  For ISIM:      Activation status is not applicable to ISIM.
  As mentioned above, currently activation status is applicable only to
  CSIM/RUIM. This function can be expanded in the future for USIM/SIM/ISIM if
  any procedure for determining USIM/SIM/ISIM activation status are defined by
  carriers/specs.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_app_act_status_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_app_act_status_enum_type mmgsdi_util_check_activation_status(
  mmgsdi_session_id_type session_id
);

/* ============================================================================
   FUNCTION:      mmgsdi_util_clear_app_capabilities

   DESCRIPTION:
     This function clears the app capabilities.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_SUCCESS: When all the app apabilities are successfully cleared.
     MMGSDI_INCORRECT_PARAMS: when app_info_ptr comes as NULL.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_clear_app_capabilities(
  mmgsdi_app_capabilities_type      *app_capabilities
);

/* ============================================================================
FUNCTION mmgsdi_util_is_path_valid

DESCRIPTION
  This function performs sanity check on path parameters passed for file
  access.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean
    TRUE         File path parameters are valid
    FALSE        File path parameters are NOT valid

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_path_valid(
  const mmgsdi_path_type * path_ptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_UTIL_DETERMINE_APP_TYPE_FROM_AID

  DESCRIPTION:
    This function determines the app type based on the Application ID (AID).

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_app_enum_type
    MMGSDI_APP_USIM: USIM application type
    MMGSDI_APP_CSIM: CSIM application type
    MMGSDI_APP_UNKNOWN:   Unknown App type

  SIDE EFFECTS:
    None
  ===========================================================================*/
mmgsdi_app_enum_type mmgsdi_util_determine_app_type_from_aid(
  const mmgsdi_static_data_type * aid_ptr
);

/* ============================================================================
FUNCTION mmgsdi_util_is_client_id_valid_in_cmd

DESCRIPTION
  This function checks if the input Client ID to an MMGSDI command
  is valid or not.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS: when client id is non-zero.
     MMGSDI_INCORRECT_PARAMS: when client id is zero.

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_client_id_valid_in_cmd(
  mmgsdi_client_id_type client_id,
  mmgsdi_cmd_enum_type  cmd_enum
);

/* ============================================================================
FUNCTION mmgsdi_util_is_session_id_valid_in_cmd

DESCRIPTION
  This function checks if the input Session ID to an MMGSDI command
  is valid or not.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS: when client id is non zero.
     MMGSDI_INCORRECT_PARAMS: when client id is zero

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_session_id_valid_in_cmd(
  mmgsdi_session_id_type session_id,
  mmgsdi_cmd_enum_type   cmd_enum
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_TMSI_PLMN_CHECK

   DESCRIPTION:
     This function validates the TMSI PLMN to protect against man-in-the-middle
     attacks. The validation is done only when persolization is active.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          TMSI PLMN being validated
     MMGSDI_ERROR:            Failed to validate TMSI PLMN

   SIDE EFFECTS:
     TMSI is set to 0xFFFFFFFF if TMSI PLMN failed the check.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_tmsi_plmn_check(
  mmgsdi_session_id_type               session_id,
  mmgsdi_data_type                   * loci_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_APP_EONS_CLEANUP

   DESCRIPTION:
     This function cleans APP EONS data
   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:

==========================================================================*/
void mmgsdi_util_app_eons_cleanup(
  mmgsdi_int_app_info_type *sel_app_ptr);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_FROM_UIM_ERROR_CONDITION

   DESCRIPTION:
     This function converts the UIM card error conditon into MMGSDI
     card error condition

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_card_err_info_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_card_err_info_enum_type mmgsdi_util_convert_from_uim_error_condition(
  uim_status_type error_condition
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_FROM_UIM_SLOT

   DESCRIPTION:
     This function converts the UIM slot type to MMGSDI slot type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_from_uim_slot(
  uim_slot_type             uim_slot,
  mmgsdi_slot_id_enum_type *mmgsdi_slot_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_FROM_UIM_PROTOCOL

   DESCRIPTION:
     This function converts the UIM protocol type to MMGSDI protocol type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_from_uim_protocol(
  uim_protocol_type          uim_protocol,
  mmgsdi_protocol_enum_type *mmgsdi_protocol_ptr
);

/*===========================================================================
FUNCTION MMGSDI_UTIL_PRE_INIT_GSDI

DESCRIPTION
  This function is called to put the MMGSDI into a state where it is ready
  to accept the LINK ESTABLISHED Message from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void mmgsdi_util_preinit_mmgsdi(
  mmgsdi_session_id_type        session_id,
  mmgsdi_app_enum_type          app_type,
  mmgsdi_refresh_mode_enum_type refresh_mode,
  mmgsdi_slot_id_enum_type      slot,
  boolean                       reselect_app
);

/*===========================================================================
FUNCTION MMGSDI_UTIL_PARSE_ATR

DESCRIPTION
   This function will parse the ATR.  Parsing will focus on the Card Service
   Data and logical channel support which is are optional bytes in the ATR.
   The Card Services is introduced in the TLV Tag of 0x31 and the logical
   channel support in 0x73.

   If the ATR does not contain Card Indicator field the parsed_atr_ptr will
   remain unchanged and the available logical channels will retain the default
   value (4) to maintain legacy behavior.

DEPENDENCIES
   None

RETURN VALUE
   mmgsdi_return_enum_type
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_parse_atr(
  mmgsdi_slot_id_enum_type  slot
);

/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_SELECT_ADF_BEFORE_PIN2_OP

   DESCRIPTION:
     This function sends SELECT for correct ADF before performing PIN2
     operation.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Currently we only store FCP and thus PIN2 for ADFs. This utility function
     works for PIN2 operations on ADFs.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          SELECT operation was successful
     MMGSDI_ERROR:            SELECT operation was successful

   SIDE EFFECTS:
     None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_select_adf_before_pin2_op(
  mmgsdi_session_id_type   session_id
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_FIND_EF_IN_POWER_UP_CACHE_LIST

   DESCRIPTION:
     This function finds if the EF is one of the EF that is cached as part of
     mmgsdi power up

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     boolean
     TRUE:  requested EF is cached as part post pin process
     FALSE  requested EF is not cached as part of post pin process

   SIDE EFFECTS:
     None

==========================================================================*/
boolean mmgsdi_util_find_ef_in_power_up_cache_list(
  mmgsdi_file_enum_type ef
);

/*===========================================================================
FUNCTION MMGSDI_UTIL_TIMED_SLEEP

DESCRIPTION
  This function is a wrapper around rex_timed_wait() to replace rex_sleep()
  with a non-deferrable timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_util_timed_sleep(unsigned long milisec);

/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CHANNEL_ID_FROM_CLA

   DESCRIPTION:
     This function extracts the channel ID from the class byte of the APDU.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_SUCCESS: Successfully extracted Channel ID from CLA.
     MMGSDI_INCORRECT_PARAMS: Incorrect input parameters.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_channel_id_from_cla(
  uint8                     cla,
  mmgsdi_slot_id_enum_type  slot_id,
  uint8                    *channel_id_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GENERATE_AND_ADD_SESSION_TO_CLIENT

   DESCRIPTION:
     This function generates a unique Session ID and saves the data in the
     Client ID registration table

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     Pointer to the newly added Session Info if successful, NULL otherwise

   SIDE EFFECTS:
     None

CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to client ID reg. table and session info ptr.
==========================================================================*/
mmgsdi_session_info_type* mmgsdi_util_generate_and_add_session_to_client (
  mmgsdi_client_id_type         client_id,
  mmgsdi_session_type_enum_type session_type
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_STOP_REFRESH

   DESCRIPTION:
     This function stops on-going refresh on a session

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_SUCCESS: if refresh is stopped

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_stop_refresh (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_CHANNEL_SHARED_WITH_PROV_SESSION

   DESCRIPTION:
     This function finds that any provisioning session is also opened requested
     channel or not.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     Boolean: TRUE if channel is shared with prov session, else FALSE

   SIDE EFFECTS:
     None
==========================================================================*/
boolean mmgsdi_util_is_channel_shared_with_prov_session (
  mmgsdi_channel_id_type   channel_id,
  mmgsdi_slot_id_enum_type slot_id
);

/* ============================================================================
  FUNCTION:      MMGSDI_UTIL_CONVERT_PATH_TO_ENUM

  DESCRIPTION:
    This function is a utility to find the corresponding file enum from path

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_file_enum_type
    The corresponding file enum

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_file_enum_type mmgsdi_util_convert_path_to_enum(
  mmgsdi_session_id_type   session_id,
  mmgsdi_path_type         file_path
);

/* ============================================================================
  FUNCTION:      MMGSDI_UTIL_IS_ADF_SHAREABLE

  DESCRIPTION:
    This function determines if the ADF corresponding to an AID is shareable
    between different logical channels.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    boolean

  SIDE EFFECTS:
    None
=============================================================================*/
boolean mmgsdi_util_is_adf_shareable(
  const mmgsdi_data_type   * aid_ptr,
  mmgsdi_slot_id_enum_type   slot_id
);

/*===========================================================================
FUNCTION MMGSDI_UTIL_RESET_GSTK_LOC_ENV_RSP_COUNT

DESCRIPTION
  This function resets the gstk_loc_env_rsp_count on the passed slot id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets global slot_data_ptr
===========================================================================*/
void mmgsdi_util_reset_gstk_loc_env_rsp_count(
  mmgsdi_slot_id_enum_type
);

/*===========================================================================
FUNCTION MMGSDI_UTIL_RESERVE_LOGICAL_CHANNEL_FOR_CSIM_APP

DESCRIPTION
  This function determines if a logical channel needs to be reserved
  for a CSIM app. It is TRUE, if all of the below are TRUE
  1. The default provisioning option is auto i.e. MMGSDI is performing the
     app provisioning.
  2. Both the USIM and CSIM apps are present on the card
  3. The CSIM app is not selected yet

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_util_reserve_logical_channel_for_csim_app(
  mmgsdi_slot_id_enum_type slot_id
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_NUM_OF_MNC_DIGITS

   DESCRIPTION:
     This function returns length of MNC in the IMSI which is store in 4th
     byte of EF-AD.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Successful retrieval of number of mnc digits
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_num_of_mnc_digits(
  mmgsdi_data_type   imsi_data,
  mmgsdi_data_type   ad_data,
  uint8            * num_mnc_digits_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_CURRENT_TASK

DESCRIPTION
  This function returns the MMGSDI task that is currently running

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_task_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_task_enum_type mmgsdi_util_get_current_task(
  void
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_TASK_FROM_SLOT

DESCRIPTION
  This function returns the MMGSDI task that is currently running

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_task_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_task_enum_type mmgsdi_util_get_task_from_slot(
  mmgsdi_slot_id_enum_type slot_id
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_SLOT_FROM_TASK

DESCRIPTION
  This function returns the MMGSDI slot corresponding to the task

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_slot_id_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_slot_id_enum_type mmgsdi_util_get_slot_from_task(
  mmgsdi_task_enum_type mmgsdi_task
);


/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_SLOT_FOR_TASK_SPECIFIC_CMD

DESCRIPTION
  This function gets the slot_id to which the command is being requested
  to.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_slot_for_cmd(
  const mmgsdi_task_cmd_type     *task_cmd_ptr,
  mmgsdi_slot_id_enum_type       *slot_id_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_SLOT_DATA_PTR_FOR_TASK

DESCRIPTION
  This function, based on the MMGSDI task passed in, return the pointer to the
  slot_data global

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_slot_data1 or mmgsdi_slot_data2 pointer address

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_slot_data_type* mmgsdi_util_get_slot_data_ptr_for_task(
  mmgsdi_task_enum_type mmgsdi_task
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_SET_TP_STATE

   DESCRIPTION:
     This function sets the app tp state and slot tp state

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_set_tp_states(
  mmgsdi_slot_data_type     * slot_data_ptr,
  mmgsdi_int_app_info_type  * app_info_ptr_gw,
  mmgsdi_int_app_info_type  * app_info_ptr_1x
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_IS_TASK_CONTEXT

DESCRIPTION
  This function checks if the current task is an UIM task

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_uim_task_context(
  void
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_MAP_TO_MMGSDI_SESSION

DESCRIPTION
  This function takes the input Session ID and maps it to the MMGSDI Session ID
  corresponding to the session type of the input

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  MMGSDI_SUCCESS: If the Session ID passed in can be mapped to a MMGSDI
                  Session ID
  MMGSDI_ERROR:   If the Session ID passed in cannot be mapped to a MMGSDI
                  Session ID

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_map_to_mmgsdi_session(
  mmgsdi_session_id_type  session_id,
  mmgsdi_session_id_type *mmgsdi_session_id_ptr
);

/*===========================================================================
FUNCTION: MMGSDI_UTIL_GET_CPHS_INFO_PTR

DESCRIPTION
  Based on the session type, this function will return the cphs info ptr

DEPENDENCIES
  none

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_cphs_info_ptr(
  mmgsdi_session_type_enum_type         session_type,
  mmgsdi_cphs_int_info_type            **cphs_int_info_pptr
);


/*===========================================================================

FUNCTION MMGSDI_MEMSCPY

DESCRIPTION
  mmgsdi_memscpy calls memscpy to copy the contents from source buffer to
  dest buffer. memscpy compares the dest size with the number of bytes to be
  copied and returns the minimum of both.
  Error message is printed if the copied size is less than the
  number of bytes to copy.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static inline void mmgsdi_memscpy(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t copy_size
)
{
  size_t copied_size;
  copied_size = memscpy(dst, dst_size, src, copy_size);
  if(copied_size < copy_size)
  {
    MMGSDI_MSG_ERROR_2("memscpy truncated from 0x%x to 0x%x",
                       copy_size, copied_size);
  }
} /* mmgsdi_memscpy */


/*===========================================================================

FUNCTION MMGSDI_MEMSMOVE

DESCRIPTION
  mmgsdi_memsmove calls memsmove to move the contents from source buffer to
  dest buffer. memsmove compares the dest size with the number of bytes to be
  moved and returns the minimum of both.
  Error message is printed if the moved size is less than the
  number of bytes to move.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static inline void mmgsdi_memsmove(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t copy_size
)
{
  size_t copied_size;
  copied_size = memsmove(dst, dst_size, src, copy_size);
  if(copied_size < copy_size)
  {
    MMGSDI_MSG_ERROR_2("memsmove truncated from 0x%x to 0x%x",
                       copy_size, copied_size);
  }
} /* mmgsdi_memsmove */

/* ============================================================================
FUNCTION:      MMGSDI_UTIL_CHECK_3GPP2_APP_IN_READY_STATE

DESCRIPTION:
  This function scans through all the apps of the 3GPP2 technology type
  (3GPP or 3GPP2) on the slot and if any app is found in READY state,
  the function mentions to the caller that the waiting 3GPP app can continue
  post pin1 opeations.

  This is controllable via an EFS item/feature flag

PARAMETERS:
  IN:
  mmgsdi_slot:      MMGSDI slot

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
========================================================================== */
mmgsdi_return_enum_type mmgsdi_util_check_3gpp2_app_in_ready_state (
  mmgsdi_slot_id_enum_type     mmgsdi_slot,
  boolean                    * app_in_ready_state_ptr
);

/* ============================================================================
FUNCTION:      MMGSDI_UTIL_UNBLOCK_APPS_FOR_POST_PERSO

DESCRIPTION:
  This function scans through all the apps of the given technology type
  (3GPP or 3GPP2) on the slot and if any app is found in PERSO VERIFIED state
  and resume thier initialization.

PARAMETERS:
  IN:
  mmgsdi_slot:      MMGSDI slot
  mmgsdi_3g_type:   3G technology type

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
========================================================================== */
void mmgsdi_util_unblock_apps_for_post_perso (
  mmgsdi_slot_id_enum_type     mmgsdi_slot,
  mmgsdi_technology_enum_type  mmgsdi_3g_type

);

/* ============================================================================
FUNCTION:      mmgsdi_util_check_perso_verified

DESCRIPTION:
  This function scans through all the apps of the requested technology type
  (3GPP or 3GPP2) on the slot and if any app is found in PERSO_VERIFIED state,
  the function mentions to the caller that the waiting apps (3GPP2 or 3GPP)
  perso state can be updated (assuming the waiting app perso check has already
  succeeded).
  This function is called once a waiting app perso check is successful.

  Waiting apps perso state won't progress if :
  1. No app of the type requested, reaches READY_FOR_PERSO state ever.

  If no USIM/SIM is present on the card, CSIM/RUIM perso state can progress.
  If no CSIM/RUIM is present on the card, USIM/SIM perso state cannot progress.

PARAMETERS:
  IN:
  mmgsdi_slot:      MMGSDI slot
  mmgsdi_3g_type:   Technology of type, 3GPP or 3GPP2

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  TRUE   - If no USIM/SIM is found OR
         - If any app of requested technology type is in
           PERSO_VERIFIED state, so that the waiting app perso state
           can be updated.
  FALSE  - If otherwise

SIDE EFFECTS:
  None
========================================================================== */
boolean mmgsdi_util_check_perso_verified (
  mmgsdi_slot_id_enum_type     mmgsdi_slot,
  mmgsdi_technology_enum_type  mmgsdi_3g_type
);

/* ============================================================================
FUNCTION:      mmgsdi_util_check_perso_verified_for_waiting_apps

DESCRIPTION:
  This function scans through all of the waiting apps of the requested
  technology type (3GPP2 or 3GPP) on the slot to find any waiting apps that are
  stuck in READY_FOR_PERSO state but have succesfully completed perso checks
  (that is the perso_state for the app is MMGSDI_PERSO_ENG_INIT_COMPLETED_S).
  If any such waiting apps are found, the function mentions to the caller that
  the waiting apps perso state can be updated for those apps to PERSO_VERIFIED.
  This function is called when an app (3GPP or 3GPP2) perso is
  successful.

  Since this function won't be called until one of the apps' perso
  check is successful, the waiting apps perso state won't progress if :
  1. no app ever reaches READY_FOR_PERSO state OR
  2. perso check doesn't succeeds for the app.

PARAMETERS:
  IN:
  mmgsdi_slot:      MMGSDI slot
  mmgsdi_3g_type:   Technology of type, 3GPP or 3GPP2
  OUT:
  session_id_ptr:   Array of session_ids for retreiving session ids corresponding
                    to the waiting app READY_FOR_PERSO state that have successfully
                    completed the perso checks
  IN/OUT:
  num_session_ids_ptr: Number of valid session ids corresponding to waiting apps

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  TRUE   - If the waiting apps perso state can be updated
  FALSE  - If otherwise

SIDE EFFECTS:
  None
========================================================================== */
boolean mmgsdi_util_check_perso_verified_for_waiting_apps (
  mmgsdi_slot_id_enum_type      mmgsdi_slot,
  mmgsdi_technology_enum_type   mmgsdi_3g_type,
  mmgsdi_session_id_type       *session_id_ptr,
  uint8                        *num_session_ids
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_UPDATE_SESSION_CLOSED_CAUSE

DESCRIPTION
  This is an utility function which updates the session closed cause in the
  app info, to which the session id provided is associated.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
void mmgsdi_util_update_session_closed_cause(
  mmgsdi_session_id_type                session_id,
  mmgsdi_session_closed_cause_enum_type session_closed_cause
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_UPDATE_SESSION_CLOSED_CAUSE_FOR_SLOT

DESCRIPTION
  This is an utility function which updates session closed cause for all the
  sessions on a slot, which have an associated app info.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
void mmgsdi_util_update_session_closed_cause_for_slot(
  mmgsdi_slot_id_enum_type              slot_id,
  mmgsdi_session_closed_cause_enum_type cause
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_ALL_AVAILABLE_APPS

DESCRIPTION
  This is an utility function which retrieves the applications that
  are available on all cards.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  MMGSDI_SUCCESS: If all the available apps are retreieved
  MMGSDI_ERROR: If global slot data is NULL for all cards

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_all_available_apps(
  mmgsdi_available_apps_type *available_apps_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_MMGSDI_PROV_SESSION_ID_FROM_SLOT

DESCRIPTION
  This is an utility function which retrieves the first available mmgsdi
  session id in mmgsdi_client_id_reg_table corresponding to slot and
  technology type.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_session_id_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_session_id_type mmgsdi_util_get_mmgsdi_prov_session_id_from_slot(
  mmgsdi_slot_id_enum_type     slot_id,
  mmgsdi_technology_enum_type  mmgsdi_3g_type
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_MMGSDI_SESSION_ID_FROM_TYPE

DESCRIPTION
  This is an utility function which retrieves the mmgsdi session id
  corresponding to session type.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_mmgsdi_session_id_from_type(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_session_id_type         *session_id_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_MMGSDI_SESSION_ID_FROM_PROVISIONING_TYPE

DESCRIPTION
  This is an utility function which checks session type is provisioning and
  then retrieves its correspondning mmgsdi session id.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_mmgsdi_session_id_from_provisioning_type(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_session_id_type         *session_id_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_AID_VALID_IN_SLOT

DESCRIPTION
  This is an utility function which vecifies if the incoming aid is valid for
  corresponding slot.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_aid_valid_in_slot(
  mmgsdi_slot_data_type        * slot_data_ptr,
  mmgsdi_app_enum_type           app_type,
  mmgsdi_static_data_type        app_data
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_APDU_SANITY_CHECK

DESCRIPTION
  This utility function performs sanity checks on streaming APDUs to ensure
  they are correctly formed.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_apdu_sanity_check(
  const mmgsdi_send_apdu_data_type      * apdu_data_ptr
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_IS_CHANNEL_AVAILABLE_FOR_REUSE_UICC

   DESCRIPTION:
     This function loops through the channel info table and checks if the
     given UICC app is active in the channel info table. Returns the channel
     id and the channel info index. It returns FALSE for zero AID length.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     boolean

   SIDE EFFECTS:
     None
==========================================================================*/
boolean mmgsdi_util_check_is_channel_available_for_reuse_uicc(
  mmgsdi_slot_id_enum_type   slot_id,
  const mmgsdi_static_data_type    * aid_data_ptr,
  uint8                    * channel_id_ptr,
  uint8                    * channel_info_index_ptr
);

/* ============================================================================
FUNCTION MMGSDI_CHECK_CLIENT_HAS_NON_PROV_OPEN

DESCRIPTION
  This utility function checks if the client has any non-prov sessions open.

DEPENDENCIES
  None

LIMITATIONS
  Caller must have acquired the CLIENT_APP_DATA_CRIT_SECT mutex.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

============================================================================*/
mmgsdi_return_enum_type mmgsdi_check_client_has_non_prov_open(
  mmgsdi_client_id_type  client_id,
  boolean               *client_has_non_prov_open
);

/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_IMSI_PLMN_CUST_PLMN

   DESCRIPTION:
     This function checks whether IMSI_M PLMN matches with specific customer
     PLMN

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     boolean

   SIDE EFFECTS:
     None
==========================================================================*/
boolean mmgsdi_util_is_imsi_plmn_cust_plmn(
  mmgsdi_slot_id_enum_type slot_id
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_PURGE_TEMP_IDENTITIES_IN_PROGRESS

DESCRIPTION
  This utility function checks if temporary identities are beging purged

DEPENDENCIES
  None

LIMITATIONS
  Caller must have acquired the CLIENT_APP_DATA_CRIT_SECT mutex.

RETURN VALUE
  TRUE:   if the temporary identities are being purged
  FALSE:  otherwise

SIDE EFFECTS
  None

============================================================================*/
boolean mmgsdi_util_is_purge_temp_identities_in_progress(
  mmgsdi_slot_id_enum_type  slot_id
);

/* =============================================================================
  FUNCTION:      MMGSDI_UTIL_GET_SESSION_ID_FOR_PROV_APP

  DESCRIPTION:
    This function finds an MMGSDI provisioning session ID linked to the app.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_prov_session_id_for_app(
  mmgsdi_slot_id_enum_type           slot_id,
  const mmgsdi_aid_type            * app_data_ptr,
  mmgsdi_session_id_type           * session_id_ptr
);

/* =============================================================================
  FUNCTION:      MMGSDI_UTIL_CHECK_APP_TERMINATION_SKIPPED

  DESCRIPTION:
    This function checks if termination of the application was previously
    skipped on a channel but the channel was kept open.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    boolean

  SIDE EFFECTS:
    None
=============================================================================*/
boolean mmgsdi_util_check_app_termination_skipped(
  mmgsdi_slot_id_enum_type           slot_id,
  const mmgsdi_aid_type            * app_data_ptr,
  mmgsdi_channel_id_type           * channel_id_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_SLOT_ID

DESCRIPTION
  This function bases on the slot index passed in, populates the MMGSDI slot id

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  MMGSDI_SUCCESS if a valid slot id is available else return MMGSDI_ERROR

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_slot_id(
  uint8                      slot_index,
  mmgsdi_slot_id_enum_type * slot_id
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_IS_USIM_AID()

  DESCRIPTION
    Checks if the AID belongs to an USIM or not

  PARAMETERS
    aid_ptr       : value of the AID

  RETURN VALUE
    TRUE for USIM. FALSE in all other cases

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mmgsdi_util_is_usim_aid(
  const uint8 *aid_ptr,
  uint8        aid_len
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_IS_CSIM_AID()

  DESCRIPTION
    Checks if the AID belongs to an CSIM or not

  PARAMETERS
    aid_ptr       : value of the AID

  RETURN VALUE
    TRUE for CSIM. FALSE in all other cases

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mmgsdi_util_is_csim_aid(
  const uint8 *aid_ptr,
  uint8        aid_len
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_IS_ISIM_AID()

  DESCRIPTION
    Checks if the AID belongs to an ISIM or not

  PARAMETERS
    aid_ptr       : value of the AID

  RETURN VALUE
    TRUE for ISIM. FALSE in all other cases

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mmgsdi_util_is_isim_aid(
   const uint8 *aid_ptr,
   uint8        aid_len
);

/* =============================================================================
  FUNCTION:      MMGSDI_UTIL_FIND_GW_PROV_SESSION_FOR_SLOT

  DESCRIPTION:
    This function finds an MMGSDI GW prov session ID active on the slot.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_find_gw_prov_session_for_slot(
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_session_id_type           * session_id_ptr,
  mmgsdi_app_enum_type             * app_type_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_CONVERT_TO_IMSI_DATA_TYPE()

  DESCRIPTION
    Convert IMSI from mmgsdi_data_type to mmgsdi_imsi_data_type

  PARAMETERS
    data_ptr
    imsi_data_ptr

  RETURN VALUE
    mmgsdi_return_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_to_imsi_data_type
(
  const mmgsdi_data_type *imsi_data_ptr,
  mmgsdi_imsi_data_type  *imsi_digit_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_RESET_APP_INFO

DESCRIPTION
  This function resets the app info that is linked to the channel at the
  given index.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

CRITICAL SECTIONS:
  The function does not have critical section protection and the caller
  is expected to use critical sections.
==========================================================================*/
void mmgsdi_util_reset_app_info(
  uint8 channel_info_index
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_RESET_UNUSED_CHANNEL_APP_DATA

DESCRIPTION
  This function resets the app data and frees the corresponding channel info
  table entry for the given slot and channel id combination if app
  termination was previously skipped on that channel.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

CRITICAL SECTIONS:
  The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
  to data slot ptr, app info ptr and pin info ptr.
==========================================================================*/
void mmgsdi_util_reset_unused_channel_app_data(
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_channel_id_type   channel_id
);

/* =============================================================================
  FUNCTION:      MMGSDI_UTIL_GET_CHANNEL_APP

  DESCRIPTION:
    This function retrieves the app_info_ptr by matching the slot_id and
    channel_id in mmgsdi_channel_info_ptr_table.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    boolean

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_channel_app(
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_channel_id_type             channel_id,
  mmgsdi_int_app_info_type        ** app_info_pptr
);

/* =============================================================================
  FUNCTION:      MMGSDI_UTIL_GET_CACHE_INDEX_FROM_SESSION

  DESCRIPTION:
    This function retrieves provisioning index from the session type passed to it.
    Provisioning cache index is returned for a provisioning session type.
    MMGSDI_MAX_PROV_APPS for any session type other than provisioning.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    uint8

  SIDE EFFECTS:
    None
=============================================================================*/
uint8 mmgsdi_util_get_prov_index_from_session
(
  mmgsdi_session_type_enum_type session_type
);

/* =============================================================================
  FUNCTION:      MMGSDI_UTIL_GET_SESSION_TYPE_FROM_AS_ID

  DESCRIPTION:
    This function determines GW & 1x session types from the AS ID

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_session_type_from_as_id
(
  sys_modem_as_id_e_type          as_id,
  mmgsdi_session_type_enum_type  *mmgsdi_gw_session_type_ptr,
  mmgsdi_session_type_enum_type  *mmgsdi_1x_session_type_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_GET_PAIR_SESSION_TYPE

DESCRIPTION
  This function determines the paired session_type for the session
  whose activation request is recieved.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_pair_session_type(
  mmgsdi_session_type_enum_type  src_session_type,
  mmgsdi_session_type_enum_type *cors_src_session_type_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_GET_TECH_AND_ATTR_FROM_SESSION_TYPE

DESCRIPTION
  This function determines the technology and the attribute
  of the requested session

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_tech_and_attr_from_session_type(
  mmgsdi_session_type_enum_type  session_type,
  mmgsdi_technology_enum_type   *technology_ptr,
  mmgsdi_attribute_enum_type    *attribute_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_GET_SESSION_TYPE_FROM_TECH_AND_ATTR

DESCRIPTION
  This function determines the session from technology and attribute
  passed.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_session_type_from_tech_and_attr(
  mmgsdi_technology_enum_type    technology,
  mmgsdi_attribute_enum_type     attribute,
  mmgsdi_session_type_enum_type *session_type_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_GET_NON_PROV_SESSION_TYPE_FROM_SLOT

DESCRIPTION
  This function determines the non-prov session type from slot.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_non_prov_session_type_from_slot(
  mmgsdi_slot_id_enum_type       slot_id,
  mmgsdi_session_type_enum_type *session_type_ptr
);

/* ==========================================================================
FUNCTION MMGSDI_UTIL_IS_EF_UNDER_PHONEBOOK

DESCRIPTION
  This function determines if the EF is under PHONEBOOK Directory(5F3A) or
  Telecom ADN file(3F00 7F10 6F3A).

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if EF is under PHONEBOOK else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_util_is_ef_under_phonebook(
  const mmgsdi_access_type  *access_ptr
);

/* =========================================================================
FUNCTION MMGSDI_UTIL_IS_EF_PBR

DESCRIPTION
  This function determines if the EF is EF-PBR (0x4F30) or not.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  Boolean: TRUE if EF is EF-PBR else FALSE

SIDE EFFECTS
  NONE
==========================================================================*/
boolean mmgsdi_util_is_ef_pbr(
  const mmgsdi_access_type *access_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_1X_PROV_SESSION

DESCRIPTION
  This function takes the session_type as input and returns whether
  the Session is a 1X provisioning session or not.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean:  TRUE : 1X provisioning session
            FALSE: Not a 1X provisioning session

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_1x_prov_session(
  mmgsdi_session_type_enum_type session_type);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_RAED_CACHE_FILE

DESCRIPTION
  Function to read EFs from cache

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  Caller need take care of freeing file_data_ptr->data_ptr
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_read_cache_file(
  mmgsdi_session_id_type         session_id,
  const mmgsdi_access_type     * file_access_ptr,
  mmgsdi_data_type             * file_data_ptr
);

/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_MCFG_REFRESH_INDEX()

  DESCRIPTION
    Returns the MCFG refresh index based on the slot id.

  PARAMETERS
    slot id
    mcfg_slot_index_ptr

  RETURN VALUE
    mmgsdi_return_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_mcfg_refresh_index(
  mmgsdi_slot_id_enum_type   slot_id,
  mcfg_refresh_index_type   *mcfg_slot_index_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_CREATE_CHANNEL_INFO_TABLE_ENTRY

DESCRIPTION
  This function returns the channel_index in the channel info table ptr
  corresponding to the channel_id. If no match for channel_id is found in
  the channel info table ptr, a new entry is created and channel_index of
  that new entry is returned.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_create_channel_info_table_entry(
  mmgsdi_cmd_enum_type               orig_request_type,
  mmgsdi_slot_id_enum_type           slot_id,
  boolean                            is_icc,
  mmgsdi_channel_id_type             channel_id,
  int32                              *channel_index_ptr
);

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_ESN_INFO

DESCRIPTION
  This function will get ESN related information of given session.

DEPENDENCIES
  None

LIMITATIONS
  Supports only Provisioning sessions

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_esn_info(
  mmgsdi_session_id_type       session_id,
  mmgsdi_esn_meid_info_type   *esn_meid_info_ptr
);
#endif /* MMGSDIUTIL_H */
