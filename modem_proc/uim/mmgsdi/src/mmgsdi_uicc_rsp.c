/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I C C   R S P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC response processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uicc_rsp.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
06/19/16   bcho    F3 frequency reduction by conditional logic
06/07/16   ar      Removing string prints from APDU and AUTH
03/31/16   lm      Do not switch protocol when multi profile AID is not present
05/24/16   ar      Review of macros used by MMGSDI
05/23/16   dd      Skip Search Req for PB files
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/17/16   ar      Fix read/write of cache files when cache is not_init
05/12/16   dd      Fill enum data when error resp is timeout for get file attr
03/16/16   na      Reset PIN info corresponding to the non-prov app in ICC mode
01/07/16   kv      UIM PSM handling updates
10/26/15   nr      Disable MMGSDI PB cache for PBM module
09/25/15   vr      Added support for ERA-GLONASS
08/10/15   vdc     Retry read request for non spec compliant cards
07/14/15   ar      Correct the caching logic of EF-PRL and EF-EPRL
07/14/15   ar      Cache complete data for req's with partial read/write data
07/02/15   rp      Fix buffer overread while parsing security attr in the FCP
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
04/27/15   av      Fix buffer overflow when processing compact security attr
04/17/15   nr      Phonebook record read optimization
01/30/15   vdc     Add file size tag workaround for select response
01/09/15   ar      Cache file if req_len is more than file size
12/12/14   yt      Get retries only for Verify/Unblock/Enable/Disable PIN op
12/09/14   ar      Replace mmgsdi_u/icc_rsp_srv_available with common func
11/17/14   hh      Add validation for PIN header data
11/14/14   hh      Fix compilation warnings
11/03/14   yt      Return status words of original PIN operation
10/23/14   yt      Updates to session and app management
10/16/14   tl      Fix report status logic for SW1: 0xFF
10/14/14   av      Remove unnecessary local array variable
10/07/14   ar      Removing check for data coding byte and length of data
09/09/14   kk      Enhancements in NV handling
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/31/14   am      Corrected use of UIM Legacy Macros
07/30/14   vv      Added support to indicate app is initialized in recovery
07/28/14   vv      Remove unused function
07/15/14   yt      Support for NV refresh
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/26/14   ar      Parse FCP response even if invalid security attributes
06/23/14   yt      Send STATUS before app deactivation
06/18/14   av      Fix out of bound access in mmgsdi_util_find_app_pin_index
06/03/14   tl      Remove additional parameter variables
05/13/14   yt      Cache EF-PRL/EF-EPRL based on the actual PRL size
05/12/14   bcho    NULL check added before dereferencing extra_param_ptr
05/06/14   vdc     Set default FCP if PIN status is missing
04/29/14   nmb     Re-enable AT+CRSM after directory maintenance changes
04/22/14   at      Critical KW fixes
04/17/14   av      Enhance MMGSDI attributes cache
03/27/14   yt      Use correct slot id
03/25/14   vdc     Added support to verify PIN after PIN is changed
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
12/05/13   tkl     SFI Support and decouple uim file enums
01/21/14   yt      Remove duplicate code
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/16/13   tl      Prevent initalizing the cache with partial files
12/11/13   df      Remove unused memory paging feature
12/05/13   ar      Replaced all mmgsdi_malloc direct calls with macro
12/02/13   vdc     Added tag 0x25 for proprietary info TLV in FCP response
11/18/13   vv      Added support for parallel REFRESH
11/06/13   tl      Remove TO-DO
10/21/13   yt      Parse 92 XX status words when UIM report status is PASS
10/04/13   spo     F3 message reduction
09/20/13   yt      Remove duplicate code
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/07/13   tl      Add CDMA AKA Authenticate functionality
07/29/13   am      Added support to determine CSG support dynamically
06/24/13   vdc     Added support for triple SIM
06/19/13   vv      Set default FCP if PIN status is missing
06/17/13   bcho    EF-ARR parsing logic modified for protection method
06/10/13   tkl     Marked cache not found when UIM return command timeout
05/31/13   av      Fix algorithm for reading the right EF ARR
05/21/13   av      Assume security attributes per spec if ARR read is skipped
05/21/13   vdc     Fix compilation errors for memory optimization
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
05/10/13   spo     Added Authentication error support for ISIM
04/23/13   tl      Do not allow read/write for invalidated files on UICC
04/17/13   tl      Replace Featurization with NV
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
02/25/13   vv      Added support to save AID received in SELECT response
02/21/13   tl      Added NULL pointer check for memory allocation and
                   clear cache for write commands when allocation fails
02/07/13   yt      Support for activation/deactivation of EFs
01/31/13   tl      Added new memory management error code
01/20/13   vv      Added EONS support for the CSG
11/12/12   vv      Provide sec. attributes for EFs in proprietary DFs
10/11/12   spo     Removed unnecessary F3 messages
09/28/12   abg     Updated ERR to MSG_ERROR
09/25/12   av      Skip reading EF-ARR for internal selects
09/09/12   tl      Initialize internal_add_params variable
09/03/12   bcho    mmgsdi_uicc_rsp_search modified to support sync operation,
                   cache OPL-PNN records in mmgsdi_uicc_rsp_read()
07/31/12   vv      Support SELECT BY_PATH for non-prov sessions
07/30/12   yt      Parse SELECT AID response
07/17/12   tl      Increase number of applications in mmgsdi context to 20
04/09/12   yt      Allow opening of non-prov session in ICC mode
04/02/12   vv      Handle function not supported (0x6A 0x81) response
03/29/12   shr     When FCP is not returned for an app, do not set default
                   PIN values if the app already has valid PIN data
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
01/26/12   vv      Set sw_present field in PIN operation resp
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   kk      Legacy GSDI removal updates
12/21/11   kk      Added USIM ECC caching support
10/28/11   yt      Added new files for cache-related utility functions
10/13/11   nb      Adding support for full recovery
09/28/11   tkl     Fixed write cache for TMSI PLMN check
09/16/11   av      Replace feature #ifdefs with an NVITEM
09/06/11   shr     Do not attempt a Card Reset as part of EFRESH RESET/Card
                   power-up handling, if the REFRESH has been stopped
08/18/11   av      Fixes for MMGSDI Klocwork CRITICAL warnings
08/09/11   tkl     Support for TMSI PLMN check
08/01/11   vs      Updating PIN extra info for intermediate PIN ops
08/01/11   vs      Setting default PIN info without overwriting existing data
06/23/11   yt      Ignore invalid status word from card during PIN verfication
05/23/11   yt      Added support for INCREASE request
05/23/11   yt      Set default PIN status if FCP is absent in SELECT response
05/17/11   kk      Updates for Thread safety
02/21/11   ms      Fix for Get file attribute failure with ISIM
02/01/11   ssr     Add extra_param_ptr NULL check for pin request
01/17/10   yt      Indicate that status word is present in Auth response
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/06/10   yt      Fixed compilation error
11/29/10   ms      Assume default security attributes when current SE is not known
11/05/10   yt      Klocwork fixes
10/27/10   shr     If file security attributes indicates ARR record number
                   as 0, do not send read request to the card
10/14/10   yt      Updated handling of client req table entries in case of error
10/11/10   tkl     Added support for USIM card with EF ARR{2F06} under USIM ADF
09/30/10   yt      Update increase_allowed flag in cyclic EF security attributes
09/09/10   vs      Set PIN status as disabled only when PIN is not init
08/25/10   yt      Added parsing for MF id in file header for get file attr
08/24/10   shr     Do not send out multiple CARD_ERROR events for a slot
06/18/10   shr     Do not modify any other access condition if AM_DO value
                   indicates that it is for INCREASE command (value '32')
06/22/10   nb      Passon the uim_rsp_ptr->cmd_transacted field while creating
                   cnf header so that sw_present field is appropriatly populated
06/16/10   vs      Fixing memory leak in SEC ATTR extraction logic
06/10/10   shr     If PIN status is already cached, do not send PIN commands
                   to the card
06/01/10   shr     Send STATUS command indicating app termination to card
                   before initiating card RESET
05/31/10   ssr     Fixed get all PIN status where APPS does not have PIN2
04/27/10   shr     Added Hiddenkey support
03/12/10   adp     Fixing KW errors
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
02/16/10   kk      Removed "is default app" check for read/write cache
12/18/09   nb      Moved Dual Slot Changes
12/14/09   ssr     Removed MMGSDI_SELECT_AID_REQ
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
12/08/09   js      Added support for MMGSDI_CSIM_ARR file
12/04/09   kp      Replacing mmgsdi_data_slot2 with mmgsdi_data_slot2_ptr
12/03/09   shr     Cache read process enhancement
10/28/09   mib     Continue in case of security attributes decoding error
10/12/09   kp      Moving Read-Write varibles to Heap
09/26/09   kp      ZI memory reduction changes
09/18/09   js      Fixed index issue with read_arr for phone book files
                   while performing select
07/27/09   kk      Fixed Lint Error - Boolean within 'if' always evaluates
                   to False
07/20/09   shr     Correctly update valid_num_unblock_retries
03/25/09   kk      Updated, if ARR record num of SEID0 is 0, use SEID1 rec num
03/12/09   js      Support for reporting invalid number of PIN retries
02/10/09   tml     Fixed security access condition population issue with
                   expand or reference expand format
01/20/09   vs      Fixed KW error
01/12/09   sun     Fixed the index issue with read_arr for read_by_path files
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/03/08   tml     Returned error if the get all pin status command processing
                   was erroneous and lint fixes
10/20/08   sun     If WRONG_CLASS is received at APP Activation, then do  a
                   card reset and change protocol to GSM.
10/01/08   ssr     Fixed for UICC Search where search is performed successfully
                   with no response data.
09/25/08   js      Fixed Klockwork errors
09/22/08   js      Fixed Lint Errors
09/08/08   sun     Fixed Compilation
09/04/08   sp      Added Null pointer check
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/20/08   jk      Compile Fixes related to PKCS15
08/20/08   kk      Calls in create_pkcs15_table made to async
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. Max path length in MMGSDI is set to 5.
                   Renamed MMGSDI_MAX_PATH_SIZE to MMGSDI_NUM_ARR_FILES
07/30/08   SSR     Update MMGSDI Status for seek on 0x6282 card warning.
07/20/08   sp      Update the cache properly for read on record based files
07/16/08   jk      Fixed Compile Warnings
06/12/08   ssr     Support of MMGSDI Search functionality
06/04/08   tml     Added sw support to cnf and message cleanup
05/12/08   tml     Fixed null pointer access when select request is BY_PATH
03/24/08   sun     Added support for OMA Bcast
03/13/08   tml     fixed memory leak
01/10/07   nk      Fixed link error
11/30/07   sun     Added support for NAA refresh
11/19/07   tml     Fixed memory leak
09/19/07   sun     Fixed PIN Header Decoding
09/10/07   tml     Add MFLO support
08/14/07   sun     Handled new error codes
06/28/07   sun     Use Extra Param ptr instead of uim to get the read file info
06/18/07   sun     Added Checks for UIM_CMD_PTR - it should never be NULL
05/11/07   sun     If the PIN is already changed from not init, then do not
                   change the status, this maybe a new select on an already
                   selected application
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
04/26/07   sp      Added support for ARR files inthe second DF level.
04/07/07   sp      Added parsing of the life cycle byte
04/04/07   sp      Send status command before terminating an app.
04/04/07   sun     Reflect the changes for Replacement in UNIV PIN pointer
                   as well
03/26/07   sun     Check if additional files are being read during refresh
                   fcn
03/19/07   sun     Fixed Lint Error
03/07/07   sun     Added support for Disable with Replacement
03/02/07   sun     Update App PIN with correct PS_DO information relevant to
                   that app only
02/21/07   sun     Removed Global PIN from GSDI
01/31/07   sp      Removed Access_denied from mmgsdi_uicc_rsp_parse_pin_num
01/31/07   tml     Fixed ARR path issues
01/17/07   tml     Reset the mmgsdi status after trying to find the path
                   pointer
01/16/07   sk      Added check in mmgsdi_uicc_rsp_decode_ref_exp_sec_attr()
                   to see if file is init in lookup table
01/12/07   sun     Determine the SEID and then look for the Record Number
                   for the ARR File while parsing security attributes
01/11/07   sun     Handle Code Blocked and CODE_PERM_BLOCKED separately.
01/08/07   sun     Get the AppId from the Client ID for All Pin Operations
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
11/21/06   sun     Fixed Parsing of the PIN Header data.
11/14/06   tml     Allow decoding PIN header to be done on non USIM App
11/12/06   sun     Increment offset within the while loop
10/27/06   tml     Allow decoding PIN header to be done on non USIM App
10/25/06   tml     Added initial support for Universal PIN replacement support
10/25/06   tml     Update cache only if default app is being modified.
10/14/06   jar     Set the report status to MMGSDI_SUCCESS when found to
                   avoid an incorrect status to the client.
                   Fixed Abort Issue seen in Univeral PIN Operations
10/13/06   tml     Update aid length in gsdi_data global structure with the
                   current_aid member
09/07/06   sun     CHANGED ACCESS_DENIED error to blocked/perm blocked status
                   For get_all_pin_status,reset status for errors other than
                   BLOCKED/PERM_BLOCKED
09/06/06   tml     Fixed Expand Security Parsing issue
08/20/06   tml     Fix select AID issue
08/18/06   sun     For Unblock PIN request, update the num_unblock_retries.
08/14/06   sun     Set the cache flag if universal pin status fails
08/07/06   sun     Added PIN and valid_pin to app_pin_info_type
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/08/06   tml     Fixed featurization
03/08/06   sun     Do not parse the UIM Response data for Streaming APDU
02/15/06   tml     Lint fix and add data fr card boolean to the write cache
02/14/06   nk      Merged for: ARR caching and by path changes
12/21/05   sun     Return Proper error ISIM Authentication failure
12/14/05   tml     MMGSDI Cleanup
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header and compilation issue
11/03/05   tml     Fixed status word 0x63 for write and increase
09/21/05   tml     Fixed status word for pin related status word
09/15/05   tml     Fixed status word issue for 0x68 and 0x63
08/30/05   jar     Added support to handle a Status Command Response from the
                   Card
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/26/05   sst     Added MMGSDIBT suppoort
07/14/05   tml     Added original request type
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_refresh.h"
#include "uim.h"
#include "rex.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_session.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_file.h"
#include "mmgsdi_pb_cache.h"
#include "mmgsdi_multi_profile.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Max length for protection PIN */
#define MMGSDI_PIN_LEN_MAX                        10

/* Value of the mandatory data coding byte which is part of the file
   descriptor response data for the select command. reference TS 102.221 */
#define MMGSDI_FILE_DESCRIPTOR_DATA_CODING_BYTE   0x21

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_PARSE_UIM_RESPONSE

   DESCRIPTION:
     This function will parse write access to any file using the UICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_parse_uim_response(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uim_report_code_type    rpt_code      = UIM_NO_SUCH_REPORT_R;
  boolean                 is_success    = FALSE;
  int32                   sw1           = 0;
  int32                   sw2           = 0;
  mmgsdi_return_enum_type error_status  = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  rpt_code     = (uim_report_code_type)uim_rsp_ptr->uim_rpt_code;
  is_success   = uim_rsp_ptr->is_uim_success;
  sw1          = uim_rsp_ptr->status_word.sw1;
  sw2          = uim_rsp_ptr->status_word.sw2;
  error_status = uim_rsp_ptr->mmgsdi_error_status;

  /* Was the request successful */
  if (is_success == TRUE)
  {
    switch ( sw1 )
    {
      case GSDI_SIM_SW1_NORM_END: /* 0x90 */
        switch(rpt_code)
        {
          case UIM_SEEK_R:
            /* Search should contain the Response Data */
            if (0x00 == uim_rsp_ptr->rsp_data.data_len)
            {
              /* No Response Data, Return Pattern not found */
              UIM_MSG_HIGH_0("Search is run successfully with no response data");
              mmgsdi_status = MMGSDI_NOT_FOUND;
            }
            break;
  
          default:
            mmgsdi_status = MMGSDI_SUCCESS;
            break;
        } /* endof switch (rpt_code) */
        break;

      case GSDI_SIM_SW1_NORM_END_W_EXTRA_INFO: /* 0x92 */
        /* According to 102.221, in case of UICC, 92 XX status words are
           applicable only to TRANSACT DATA command which is currently not
           supported. Hence, these status words are handled as SUCCESS. */
        mmgsdi_status = MMGSDI_SUCCESS;
        break; /* GSDI_SIM_SW1_NORM_END_W_EXTRA_INFO 0x92 */

      default: /* other sw1 when rpt_status == PASS */
        mmgsdi_status = MMGSDI_ERROR;
        break;
    }
  }
  else
  {
    UIM_MSG_HIGH_3("In mmgsdi_uicc_rsp_parse_uim_response SW1: 0x%x, SW2: 0x%x, rpt_code: 0x%x",
               sw1, sw2, rpt_code);
    /* UIM_FAIL */
    switch ( sw1 )
    {
      case GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO: /* 0x91 XX*/
        mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        break; /* GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO  0x91 XX*/

      case GSDI_SW1_SIM_APP_TOOLKIT_BUSY: /* 0x93 */
        switch (rpt_code)
        {
        case UIM_ENVELOPE_R:
          /* Sim is busy */
          mmgsdi_status = MMGSDI_SIM_BUSY;
          break;
        default:
          UIM_MSG_MED_1("Invalid report code 0x%x", rpt_code);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break; /* GSDI_SW1_SIM_APP_TOOLKIT_BUSY 0x93 */

      case GSDI_SW1_REFERENCE_MANAGEMENT:  /* 0x94 */
        /* Reference Management Error, Now  */
        /* Determine the specific Error     */
        switch ( sw2 )
        {
        case GSDI_SW2_NO_EF_SELECTED: /* 0x00 */
          switch (rpt_code)
          {
          case UIM_ACCESS_R:
          case UIM_READ_BIN_CMD_R:
          case UIM_READ_REC_CMD_R:
          case UIM_WRITE_BIN_CMD_R:
          case UIM_WRITE_REC_CMD_R:
          case UIM_INC_CMD_R:
          case UIM_SEEK_R:
          case UIM_INVALIDATE_R:
          case UIM_REHABILITATE_R:
            /* No EF Selected               */
            mmgsdi_status = MMGSDI_NO_EF_SELECTED;
            break;

          default:
            UIM_MSG_MED_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        case GSDI_SW2_OUT_OF_RANGE: /* 0x02 */
          switch (rpt_code)
          {
          case UIM_ACCESS_R: /* only update record and read record */
          case UIM_READ_BIN_CMD_R:
          case UIM_READ_REC_CMD_R:
          case UIM_WRITE_BIN_CMD_R:
          case UIM_WRITE_REC_CMD_R:
          case UIM_INC_CMD_R:
            /* Out of Range or invalid      */
            /* address                      */
            mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
            break;
          default:
            UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        case GSDI_SW2_FILE_OR_PATTER_ID_NOT_FOUND: /* 0x04 */
          switch (rpt_code)
          {
          case UIM_ACCESS_R:
          case UIM_READ_BIN_CMD_R:
          case UIM_READ_REC_CMD_R:
          case UIM_WRITE_BIN_CMD_R:
          case UIM_WRITE_REC_CMD_R:
          case UIM_INC_CMD_R:
          case UIM_SELECT_R:
          case UIM_SEEK_R:
            /* File ID not found or pattern */
            /* not found                    */
            mmgsdi_status = MMGSDI_NOT_FOUND;
            break;
          default:
            UIM_MSG_MED_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        case GSDI_SW2_FILE_INCONSISTENT_WITH_CMD: /* 0x08 */
          switch (rpt_code)
          {
          case UIM_ACCESS_R:
          case UIM_READ_BIN_CMD_R:
          case UIM_READ_REC_CMD_R:
          case UIM_WRITE_BIN_CMD_R:
          case UIM_WRITE_REC_CMD_R:
          case UIM_INC_CMD_R:
          case UIM_SEEK_R:
          case UIM_RUN_GSM_ALGO_R:
            /* File is inconsistend with    */
            /* command                      */
            mmgsdi_status = MMGSDI_EF_INCONSISTENT;
            break;
          default:
            UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        default:
          /* Can't determine error but    */
          /* Error doesn't need to create */
          /* an event to the UI           */
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break; /* GSDI_SW1_REFERENCE_MANAGEMENT 0x94 */

      case GSDI_SW1_SECURITY_MANAGEMENT: /* 0x98 */
        /* Security Management Errors       */
        switch ( sw2 )
        {
        case GSDI_SW2_NO_CHV_INITIALISED: /* 0x02 */
          switch (rpt_code)
          {
          case UIM_VERIFY_CHV_R:
          case UIM_CHANGE_CHV_R:
          case UIM_DISABLE_CHV_R:
          case UIM_ENABLE_CHV_R:
          case UIM_UNBLOCK_CHV_R:
            /* No CHV Initialized           */
            mmgsdi_status = MMGSDI_PIN_NOT_INITIALIZED;
            break;
          default:
            UIM_MSG_MED_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        case GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1: /* 0x04 */
          switch (rpt_code)
          {
          case UIM_VERIFY_CHV_R:
          case UIM_CHANGE_CHV_R:
          case UIM_DISABLE_CHV_R:
          case UIM_ENABLE_CHV_R:
          case UIM_UNBLOCK_CHV_R:
            UIM_MSG_HIGH_0("GSDI_INCORRECT_CODE");
            mmgsdi_status = MMGSDI_INCORRECT_CODE;
            break;

          case UIM_ACCESS_R:
          case UIM_READ_BIN_CMD_R:
          case UIM_READ_REC_CMD_R:
          case UIM_WRITE_BIN_CMD_R:
          case UIM_WRITE_REC_CMD_R:
          case UIM_INC_CMD_R:
          case UIM_SEEK_R:
          case UIM_INVALIDATE_R:
          case UIM_REHABILITATE_R:
            UIM_MSG_HIGH_0("GSDI_ACCESS_DENIED");
            /* Access conditon not fullfilled  */
            /* or CHV Verification not success */
            /* or CHV Unblock not success      */
            /* or authentication failed        */
            mmgsdi_status = MMGSDI_ACCESS_DENIED;
            break;

          case UIM_RUN_GSM_ALGO_R:
            UIM_MSG_HIGH_0("GSDI_AUTH_FAIL");
            mmgsdi_status = MMGSDI_AUTH_FAIL;
            break;

          default:
            UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        case GSDI_SW2_CNTRCTN_W_CHV_STATUS: /* 0x08 */
          switch (rpt_code)
          {
          case UIM_VERIFY_CHV_R:
          case UIM_CHANGE_CHV_R:
          case UIM_DISABLE_CHV_R:
          case UIM_ENABLE_CHV_R:
          case UIM_UNBLOCK_CHV_R:
            /* Access conditon not fullfilled  */
            /* or CHV Verification not success */
            /* or CHV Unblock not success      */
            /* or authentication failed        */
            mmgsdi_status = MMGSDI_INCOMPAT_PIN_STATUS;
            break;
          default:
            UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        case GSDI_SW2_CNTRCTN_W_INVALIDATION_STATUS: /* 0x10 */
          switch (rpt_code)
          {
          case UIM_ACCESS_R:
          case UIM_READ_BIN_CMD_R:
          case UIM_READ_REC_CMD_R:
          case UIM_WRITE_BIN_CMD_R:
          case UIM_WRITE_REC_CMD_R:
          case UIM_INC_CMD_R:
          case UIM_SEEK_R:
          case UIM_INVALIDATE_R:
          case UIM_REHABILITATE_R:
            /* Access conditon not fullfilled  */
            /* or CHV Verification not success */
            /* or CHV Unblock not success      */
            /* or authentication failed        */
            mmgsdi_status = MMGSDI_INVALIDATION_CONTRADICTION_STATUS;
            break;
          default:
            UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        case GSDI_SW2_CHV_BLOCK_UNSUCCESSFUL_UNBLOCK: /* or GSDI_SW2_CHV_UNBLOCKED 0x40 */
          switch (rpt_code)
          {
          case UIM_VERIFY_CHV_R:
          case UIM_CHANGE_CHV_R:
          case UIM_DISABLE_CHV_R:
          case UIM_ENABLE_CHV_R:
            UIM_MSG_HIGH_0("GSDI_CODE_BLOCKED");
            mmgsdi_status = MMGSDI_CODE_BLOCKED;
            break;

          case UIM_UNBLOCK_CHV_R:
            /* Access conditon not fullfilled  */
            /* or CHV Verification not success */
            /* or CHV Unblock not success      */
            /* or authentication failed        */
            UIM_MSG_HIGH_0("GSDI_CODE_PERM_BLOCKED");
            mmgsdi_status = MMGSDI_CODE_PERM_BLOCKED;
            break;

          default:
            UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        case GSDI_SW2_INCREASED_FAIL_MAX_REACHED: /* 0x50 */
          switch (rpt_code)
          {
          case UIM_ACCESS_R: /* Increase only */
          case UIM_READ_BIN_CMD_R:
          case UIM_READ_REC_CMD_R:
          case UIM_WRITE_BIN_CMD_R:
          case UIM_WRITE_REC_CMD_R:
          case UIM_INC_CMD_R:
            /* Access conditon not fullfilled  */
            /* or CHV Verification not success */
            /* or CHV Unblock not success      */
            /* or authentication failed        */
            mmgsdi_status = MMGSDI_INCREASE_MAX_REACHED;
            break;
          default:
            UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          }
          break;

        case GSDI_SW2_AUTH_ERROR_APP_SPECIFIC: /* 0x62 */
          switch (rpt_code)
          {
            case UIM_ISIM_AUTHENTICATE_R:
            case UIM_AUTHENTICATE_R:
            case UIM_AUTHENTICATE_ODD_INS_R:
              /* Authentication failed        */
              mmgsdi_status = MMGSDI_AUTH_ERROR_INCORRECT_MAC;
              break;
            default:
              UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
              mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
              break;
          }
          break;

        case GSDI_SW2_AUTH_ERROR_GSM_CNTXT_NOT_SUP: /* 0x64 */
          switch(rpt_code)
          {
            case UIM_AUTHENTICATE_R:
            case UIM_AUTHENTICATE_ODD_INS_R:
            case UIM_ISIM_AUTHENTICATE_R:
              mmgsdi_status = MMGSDI_AUTH_ERROR_GSM_CNTXT_NOT_SUP;
              break;

            default:
              UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
              mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
              break;
          }
          break;

        case GSDI_SW2_AUTH_ERROR_KEYFRESHNESS_FAIL: /* 0x65 */
          switch(rpt_code)
          {
            case UIM_AUTHENTICATE_ODD_INS_R:
              mmgsdi_status = MMGSDI_AUTH_ERROR_KEY_FRESHNESS_FAIL;
              break;

            default:
              UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
              mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
              break;
          }
          break;

        case GSDI_SW2_AUTH_ERROR_NO_MEM: /* 0x66 */
          switch(rpt_code)
          {
            case UIM_AUTHENTICATE_ODD_INS_R:
            case UIM_BCAST_R:
              mmgsdi_status = MMGSDI_AUTH_ERROR_NO_MEM_SPACE;
              break;

            default:
              UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
              mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
              break;
          }
          break;

        default:
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break; /* GSDI_SW1_SECURITY_MANAGEMENT 0x98 */

      case GSDI_SW1_WARNINGS_PART_1: /* 0x62 */
        switch ( sw2 )
        {
        case GSDI_SW2_NO_INFO_VOLATILE_MEM_UNCHANGED: /* 0x00 */
          mmgsdi_status = MMGSDI_WARNING_NO_INFO_GIVEN;
          break;

        case GSDI_SW2_END_REACHED_BEFORE_LE_BYTES:  /* 0x82 */
          switch(rpt_code)
          {
             case UIM_SEEK_R:
             /* Search pattern not found, Reached end of file*/
               mmgsdi_status = MMGSDI_NOT_FOUND;
               break;

             default:
               mmgsdi_status = MMGSDI_INCORRECT_LENGTH;
               break;
          }/* End of switch */
          break;

        case GSDI_SW2_SELECTED_FILE_INVALIDATED:  /* 0x83 */
          mmgsdi_status = MMGSDI_WARNING_POSSIBLE_CORRUPTION;
          break;

        case GSDI_SW2_FCI_NOT_ACCORDING_TO_SPECS: /* 0x84 */
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;

        default:
          /* Unhandled Error Condition */
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break; /* GSDI_SW1_WARNINGS_PART_2 0x62*/

      case GSDI_SW1_WARNINGS_PART_2: /* 0x63 */
        switch (rpt_code)
        {
        case UIM_WRITE_BIN_CMD_R:
        case UIM_WRITE_REC_CMD_R:
        case UIM_INC_CMD_R:
          if ( (sw2 & MMGSDI_MASK_FOR_HIGH_BYTE) == MMGSDI_SW2_CMD_X_TRYS_LEFT )
          {
            /* Access conditon not fullfilled  */
            /* or CHV Verification not success */
            /* or CHV Unblock not success      */
            /* or authentication failed        */
            mmgsdi_status = MMGSDI_SUCCESS;

          }
          else
          {
            /* Unhandled Error Condition */
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          }
          break;

        case UIM_VERIFY_CHV_R:
        case UIM_CHANGE_CHV_R:
        case UIM_DISABLE_CHV_R:
        case UIM_ENABLE_CHV_R:
          if ( (sw2 & MMGSDI_MASK_FOR_HIGH_BYTE) == MMGSDI_SW2_CMD_X_TRYS_LEFT)
          {
            if ( (sw2 & MMGSDI_MASK_FOR_LOW_BYTE) == 0x00 )
            {
              mmgsdi_status = MMGSDI_CODE_BLOCKED;
            }
            else
            {
              mmgsdi_status = MMGSDI_INCORRECT_CODE;
            }
          }
          else
          {
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          }
          break;

        case UIM_UNBLOCK_CHV_R:
          if ( (sw2 & MMGSDI_MASK_FOR_HIGH_BYTE) == 0xC0)
          {
            if ( (sw2 & MMGSDI_MASK_FOR_LOW_BYTE) == 0x00 )
            {
              mmgsdi_status = MMGSDI_CODE_PERM_BLOCKED;
            }
            else
            {
              mmgsdi_status = MMGSDI_INCORRECT_CODE;
            }
          }
          else
          {
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          }
          break;

        default:
          UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW1_EXECUTION_ERROR_1: /* 0x64 */
        mmgsdi_status = MMGSDI_WARNING_POSSIBLE_CORRUPTION;
        break; /* GSDI_SW1_EXECUTION_ERROR_1 0x64 */
      case GSDI_SW1_EXECUTION_ERROR_2: /* 0x65 */
        switch ( sw2 )
        {
          case GSDI_SW2_EXECUTION_MEMORY_PROBLEM: /* 0x81 */
            /* Memory Management Error          */
            /* SIM Memory Problem           */
            mmgsdi_status = MMGSDI_MEMORY_ERROR;
            break;

          default:
            UIM_MSG_HIGH_1("Execution error sw1 0x%x", sw1);
            mmgsdi_status = MMGSDI_WARNING_POSSIBLE_CORRUPTION;
            break;
        }
        break; /* GSDI_SW1_EXECUTION_ERROR_2 0x65 */

      case GSDI_SW1_WRONG_LENGTH: /* 0x67 */
        /* or GSDI_SW1_INCORRECT_P3 - doesn't have specific sw2 */
        mmgsdi_status = MMGSDI_INCORRECT_LENGTH;
        break; /* GSDI_SW1_WRONG_LENGTH 0x67 */

      case GSDI_SW1_FUNCTIONS_IN_CLA_ERROR: /* 0x68 */
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
        break; /* GSDI_SW1_WRONG_LENGTH 0x68 */

      case GSDI_SW1_COMMAND_NOT_ALLOWED: /* 0x69 */
        switch ( sw2 )
        {
        case GSDI_SW2_NO_INFO_GIVEN: /* 0x00 */
          mmgsdi_status = MMGSDI_WARNING_NO_INFO_GIVEN;
          break;

        case GSDI_SW2_CMD_INCOMPATIBLE_W_FILE_STRUCT: /* 0x81 */
          mmgsdi_status = MMGSDI_EF_INCONSISTENT;
          break;

        case GSDI_SW2_SECURITY_STATUS_NOT_SATISFIED: /* 0x82 */
        case GSDI_SW2_REFERENCE_DATA_INVALIDATED: /* 0x84 */
        case GSDI_SW2_CNDTNS_OF_USE_NOT_SATISFIED: /* 0x85 */
        case GSDI_SW2_CMD_NOT_ALLOWD_NO_EF_SELECTED: /* 0x86 */
          UIM_MSG_HIGH_1("Access Denied sw2: 0x%x", sw2);
          mmgsdi_status = MMGSDI_ACCESS_DENIED;
          break;

        case GSDI_SW2_AUTH_PIN_METHOD_BLOCKED:  /* 0x83 */
          switch(rpt_code)
          {
            case UIM_VERIFY_CHV_R:
            case UIM_ENABLE_CHV_R:
            case UIM_DISABLE_CHV_R:
            case UIM_CHANGE_CHV_R:
              UIM_MSG_HIGH_0("GSDI_CODE_BLOCKED");
              mmgsdi_status = MMGSDI_CODE_BLOCKED;
              break;
            case UIM_UNBLOCK_CHV_R:
              UIM_MSG_HIGH_0("GSDI_CODE_PERM_BLOCKED");
              mmgsdi_status = MMGSDI_CODE_PERM_BLOCKED;
              break;
            default:
              UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
              mmgsdi_status = MMGSDI_ACCESS_DENIED;
              break;
          }
          break;

        case GSDI_SW2_UNDEFINED_99:
        if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_CHANGE_INST_CLASS, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED &&
            rpt_code == UIM_SELECT_R)
          {
            UIM_MSG_ERR_0("WRONG INSTRUCTION CLASS");
            mmgsdi_status = MMGSDI_WRONG_CLASS;
          }
          else
          {
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          }
          break;

        default:
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break; /* GSDI_SW1_COMMAND_NOT_ALLOWED 0x69 */

      case GSDI_SW1_INCORRECT_PARAMETERS: /* 0x6A */
        switch ( sw2 )
        {
        case GSDI_SW2_IN_THE_DATA_FIELD:  /* 0x80 */
        case GSDI_SW2_LC_INCONSISTENT_WITH_TLV:  /*  0x85 */
        case GSDI_SW2_INCORRECT_PARAMS_P1_OR_P2:  /*  0x86 */
        case GSDI_SW2_LC_INCONSISTENT_WITH_P1_OR_P2: /* 0x87 */
          mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
          break;
        case GSDI_SW2_REFERENCE_DATA_NOT_FOUND: /* 0x88 */
          mmgsdi_status = MMGSDI_REF_DATA_NOT_FOUND;
          break;

        case GSDI_SW2_FILE_NOT_FOUND: /* 0x82 */
        case GSDI_SW2_RECORD_NOT_FOUND:  /* 0x83 */
          mmgsdi_status = MMGSDI_NOT_FOUND;
          break;

        case GSDI_SW2_FUNCTION_NOT_SUPPORTED: /* 0x81 */
          mmgsdi_status = MMGSDI_NOT_SUPPORTED;
          break;

        default:
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break; /* GSDI_SW1_INCORRECT_PARAMETERS 0x6A*/

      case GSDI_SW1_INCORRECT_P1_OR_P2: /* 0x6B */
        /* Application Independent Errors   */
        /* Incorrect Parameters P3          */
        /* or Incorrect Parameters P1 & P2  */
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
        break; /* GSDI_SW1_INCORRECT_P1_OR_P2 0x6B */

      case GSDI_SW1_UNKNOWN_INSTRUCTION_CODE: /* 0x6D */
      case GSDI_SW1_WRONG_INSTRUCTION_CLASS:  /* 0x6E */
        /* Unknown or Wrong Instruction class  */
        mmgsdi_status = MMGSDI_UNKNOWN_INST_CLASS;
        break; /* GSDI_SW1_WRONG_INSTRUCTION_CLASS 0x6E */

      case GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN: /* 0x6F */
        /* Application Independent Erros    */
        mmgsdi_status = MMGSDI_SIM_TECHNICAL_PROBLEMS;
        break; /* GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN 0x6F */

      case GSDI_SIM_SW1_NORM_END: /* 0x90 */
        mmgsdi_status = MMGSDI_UIM_CMD_TIMEOUT;
        break; /* GSDI_SIM_SW1_NORM_END 0x90 */

      case GSDI_SW1_UIM_TIMEOUT: /* 0x00 */
        switch ( sw2 )
        {
          case GSDI_SW2_UIM_TIMEOUT: /* 0x00 */
              mmgsdi_status = MMGSDI_UIM_CMD_TIMEOUT;
            break;
          default:
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
        }
        break; /* GSDI_SW1_UIM_TIMEOUT 0x00 */

      case GSDI_SW1_INT_ERROR:
        if (error_status != MMGSDI_SUCCESS)
        {
          mmgsdi_status = error_status;
        }
        else
        {
          /* In the case an internal error occurs but the mmgsdi_error_status failed
             to be updated correctly, manually update the report status to generic error. */
          mmgsdi_status = MMGSDI_ERROR;
        }
        break;

      default:
        /* Unhandled Error Condition */
        mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        break;
    }
  } /* UIM Fail */
  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_parse_uim_response */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_GET_FILE_REC_INFO

   DESCRIPTION:
     This function populate the UICC File Header Record information

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type  mmgsdi_uicc_rsp_get_file_des(
  uint8                            file_descriptor,
  mmgsdi_file_attributes_type     *decoded_data_ptr
)
{
  int linear_file = 0x02;
  int trans_file  = 0x01;
  int cyclic_file = 0x06;
  int DF_ADF   = 0x38;

  if(!decoded_data_ptr)
  {
    return MMGSDI_ERROR;
  }

  if((file_descriptor & 0x80) == 0)
  {
    if((file_descriptor & 0x07) == linear_file)
    {
       decoded_data_ptr->file_type =  MMGSDI_LINEAR_FIXED_FILE;
    }
    if((file_descriptor & 0x07) == trans_file)
    {
      decoded_data_ptr->file_type =  MMGSDI_TRANSPARENT_FILE;
    }
    if((file_descriptor & 0x07) == cyclic_file)
    {
      decoded_data_ptr->file_type =  MMGSDI_CYCLIC_FILE;
    }
    if((file_descriptor & 0x38) == DF_ADF)
    {
      decoded_data_ptr->file_type = MMGSDI_DEDICATED_FILE;
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_uicc_rsp_get_file_des */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_GET_FILE_REC_INFO

   DESCRIPTION:
     This function populate the UICC File Header Record information

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type  mmgsdi_uicc_rsp_get_file_rec_info(
  int32                         len,
  uint8                         data_msb,
  uint8                         data_lsb,
  uint8                         num_of_rec,
  uint8                         file_type,
  mmgsdi_file_attributes_type  *decoded_data_ptr)
{
  if(!decoded_data_ptr)
  {
    return MMGSDI_ERROR;
  }
  if(len == 0x05)
  {
    /* Record length */
    if((file_type & 0x07) == 0x02)
    {
      decoded_data_ptr->file_info.linear_fixed_file.rec_len =
        ((int32)data_msb << 8) | (int32)data_lsb ;
      decoded_data_ptr->file_info.linear_fixed_file.num_of_rec = num_of_rec;
    }
    if((file_type & 0x07) == 0x06)
    {
      decoded_data_ptr->file_info.cyclic_file.rec_len =
        ((int32)data_msb << 8) | (int32)data_lsb ;
      decoded_data_ptr->file_info.cyclic_file.num_of_rec = num_of_rec;
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_uicc_rsp_get_file_rec_info */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_UICC_RSP_GET_LIFE_CYCLE_INFO

  DESCRIPTION:
    This function populate the UICC File Header Life cycle information.
    Also sets the flag read_write_when_invalidated to be in sync with
    icc respnse since some clients use it without looking for the protocol.

  DEPENDENCIES:
    This function assumes that the file type is already decoded succesfully.

  LIMITATIONS:
    This function is limited to the use of UICC

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command process is successful.
    MMGSDI_ERROR:            The Input parameter is null.

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_uicc_rsp_get_life_cycle_info(
  uint8                         life_cycle_byte,
  mmgsdi_file_attributes_type  *decoded_data_ptr
)
{

  mmgsdi_file_status_type * file_status_ptr = NULL;

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(decoded_data_ptr, MMGSDI_ERROR);

  /* based on the file type, assign the pointer */
  switch(decoded_data_ptr->file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
      file_status_ptr =
          &decoded_data_ptr->file_info.linear_fixed_file.file_status;
      break;
    case MMGSDI_CYCLIC_FILE:
      file_status_ptr =
          &decoded_data_ptr->file_info.cyclic_file.file_status;
      break;
    case MMGSDI_TRANSPARENT_FILE:
      file_status_ptr =
          &decoded_data_ptr->file_info.transparent_file.file_status;
      break;
    default:
      return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(file_status_ptr, MMGSDI_ERROR);

  /* Set if the file is activated or deactivated */
  /* From spec 102.221 11.1.1, life cycle byte has the following values *
   * of interest                                                        *
   * 000001-1 -Activate state                                           *
   * 000001-0 -Deactivate state                                         *
   *                                                                    *
   */
  if((life_cycle_byte & MMGSDI_UICC_LIFE_CYCLE_BYTE_CHECK) == 0)
  {
    /* File is activated */
    if( (life_cycle_byte & MMGSDI_UICC_LIFE_CYCLE_FILE_ACTIVATED)
          == MMGSDI_UICC_LIFE_CYCLE_FILE_ACTIVATED)
    {
      file_status_ptr->file_invalidated = FALSE;
    }
    /* File is deactivated */
    else if( (life_cycle_byte & MMGSDI_UICC_LIFE_CYCLE_FILE_DEACTIVATED)
               == MMGSDI_UICC_LIFE_CYCLE_FILE_DEACTIVATED)
    {
      file_status_ptr->file_invalidated = TRUE;
    }
    else
    {
      UIM_MSG_HIGH_0("Life_cycle_byte different than file_activated/deactivated");
    }
  }

  /* Text from ETSI 102 221, section 11.1.14:
     A deactivated file on UICC card shall no longer be available within
     the selected application for any function except for the SELECT and
     the ACTIVATE FILE functions. */
  file_status_ptr->read_write_when_invalidated = FALSE;

  return MMGSDI_SUCCESS;
}/* mmgsdi_uicc_rsp_get_life_cycle_info */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_CONVERT_TO_MMGSDI_PIN

   DESCRIPTION:
     This function checks to see if the PIN given is present in the mapping
     table

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC  as it uses the uicc pin
     mapping table.

   RETURN VALUE:
     boolean

     TRUE:         1
     FALSE:        0

   SIDE EFFECTS:
     None

-------------------------------------------------------------------------------*/
static boolean mmgsdi_uicc_rsp_convert_to_mmgsdi_pin
(
  int32                 pin,
  mmgsdi_pin_enum_type *pin_enum_ptr
)
{
  if (pin >= 0x01 && pin <= 0x08)
  {
    *pin_enum_ptr = MMGSDI_PIN1;
  }
  else if (pin >= 0x81 && pin <= 0x88)
  {
    *pin_enum_ptr = MMGSDI_PIN2;
  }
  else
  {
    switch (pin)
    {
      case 0x11:
        *pin_enum_ptr = MMGSDI_UNIVERSAL_PIN;
        break;
      case 0x0A:
        *pin_enum_ptr = MMGSDI_ADM1;
        break;
      case 0x0B:
        *pin_enum_ptr = MMGSDI_ADM2;
        break;
      case 0x0C:
        *pin_enum_ptr = MMGSDI_ADM3;
        break;
      case 0x0D:
        *pin_enum_ptr = MMGSDI_ADM4;
        break;
      case 0x0E:
        *pin_enum_ptr = MMGSDI_ADM5;
        break;
      case 0x8A:
        *pin_enum_ptr = MMGSDI_ADM6;
        break;
      case 0x8B:
        *pin_enum_ptr = MMGSDI_ADM7;
        break;
      case 0x8C:
        *pin_enum_ptr = MMGSDI_ADM8;
        break;
      case 0x8D:
        *pin_enum_ptr = MMGSDI_ADM9;
        break;
      case 0x8E:
        *pin_enum_ptr = MMGSDI_ADM10;
        break;
      default:
        *pin_enum_ptr = MMGSDI_MAX_PIN_ENUM;
        return FALSE;
    }
  }

  return TRUE;
}/* mmgsdi_uicc_rsp_convert_to_mmgsdi_pin */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_EXTRACT_EXP_SEC_ATTR

   DESCRIPTION:
     This function populate the corresponding command (read/write/activate/
     deactivate/increase etc)'s key reference into mmgsdi file security struct

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_uicc_rsp_extract_exp_sec_attr(
  mmgsdi_file_security_type           *oper_ptr,
  mmgsdi_file_security_enum_type       key_ref_value,
  const mmgsdi_file_security_type     *security_pins_ptr
  )
{
  int32 i = 0;
  boolean ret = FALSE;

  MMGSDIUTIL_RETURN_IF_NULL_3(oper_ptr,
                              oper_ptr->protection_pin_ptr,
                              security_pins_ptr);

  while(i < security_pins_ptr->num_protection_pin)
  {
      ret = mmgsdi_uicc_rsp_convert_to_mmgsdi_pin(
              (int32)security_pins_ptr->protection_pin_ptr[i],
              &(oper_ptr->protection_pin_ptr[i]));
      if(ret == FALSE)
      {
          return MMGSDI_ERROR;
      }
      i++;
  }
  oper_ptr->num_protection_pin =
              security_pins_ptr->num_protection_pin;
  if(security_pins_ptr->num_protection_pin == 1)
  {
      key_ref_value = MMGSDI_SINGLE_ALLOWED;
  }
  oper_ptr->protection_method = key_ref_value;

  return MMGSDI_SUCCESS;
} /* mmgsdi_uicc_rsp_extract_exp_sec_attr */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_EXP_SEC_ATTR

   DESCRIPTION:
     This function search through the header info for security attributes
     according to the expand encoding format

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_uicc_rsp_decode_exp_sec_attr (
  const uint8                 * attr_buffer_ptr,
  int32                         attr_length,
  mmgsdi_file_attributes_type * sec_oper_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status                 = MMGSDI_SUCCESS;
  boolean                           or_tag_present;
  boolean                           and_tag_present;
  byte                              am_do_value;
  mmgsdi_sc_do_tags_eum_type        sc_do_tag;
  mmgsdi_file_security_enum_type    key_ref_value                 = MMGSDI_NEVER_ALLOWED;
  uint16                            orand_tag_length              = 0;
  uint16                            sc_do_length;
  uint16                            offset                        = 0;
  mmgsdi_file_security_type         security_pins;
  uint8                             prev_flg                      = 0;
  uint8                             cnt                           = 0;
  uint16                            loop                          = 0;
  mmgsdi_file_security_access_type *operation_ptr                 = NULL;
  uint32                            pin_length                    = 0;
  boolean                           protection_check_done         = FALSE;
  uint16                            orand_tag_length_pos          = 0;

  MMGSDIUTIL_RETURN_IF_NULL(sec_oper_ptr);
  mmgsdi_uicc_rsp_free_pin_protection_info(sec_oper_ptr);

  switch(sec_oper_ptr->file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
      operation_ptr = &sec_oper_ptr->file_info.linear_fixed_file.file_security;
      break;
    case MMGSDI_CYCLIC_FILE:
      operation_ptr = &sec_oper_ptr->file_info.cyclic_file.file_security;
      break;
    case MMGSDI_TRANSPARENT_FILE:
      operation_ptr = &sec_oper_ptr->file_info.transparent_file.file_security;
      break;
    case MMGSDI_MASTER_FILE:
    case MMGSDI_DEDICATED_FILE:
    case MMGSDI_MAX_FILE_STRUCTURE_ENUM:
      return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(operation_ptr, attr_buffer_ptr);

  pin_length = sizeof(mmgsdi_file_security_enum_type) * MMGSDI_PIN_LEN_MAX;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(security_pins.protection_pin_ptr,
                                     pin_length);
  if (security_pins.protection_pin_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  security_pins.num_protection_pin = 0;

  while ( ( offset < attr_length ) &&
          ( attr_buffer_ptr[offset] != 0xFF ) &&
          ( attr_buffer_ptr[offset] != 0x00) )
  {
    security_pins.num_protection_pin = 0;
    cnt = 0;
    prev_flg = 0;
    orand_tag_length_pos = 0;

    if ( ( attr_buffer_ptr[offset] == GSDI_AM_DO_BYTE_TAG ) ||
          ( attr_buffer_ptr[offset] == GSDI_AM_DO_INST_TAG ) )
    {
      offset++; /* am_do_tag */
      /* Length of AM_DO tags 80 and 84 can only be 1 as per 7816-4, Section 5.4.3.2 */
      if (offset >= attr_length ||
          attr_buffer_ptr[offset] != 0x01 ||
          (attr_buffer_ptr[offset] + 2) > attr_length)
      {
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }
      offset++; /* am_do_length */
      if (offset >= attr_length)
      {
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }
      am_do_value = attr_buffer_ptr[offset++];

      /* At this point, the AM_DO tags are all good... even if the rest of the
         TLV (for SC_DOs) is messed up, that is, the SC_DOs fails the length
         checks, we do not return ERROR and instead we go ahead and associate
         MMGSDI_NEVER_ALLOWED for the various access mode bits (READ, UPDATE etc) */
      if (offset < attr_length)
      {
        /* Check if an OR or AND tag is present */
        or_tag_present = FALSE;
        and_tag_present = FALSE;
        orand_tag_length = 0;
        if ( attr_buffer_ptr[offset] == GSDI_SC_DO_OR_TAG ) {
            or_tag_present = TRUE;
            offset++;
            orand_tag_length_pos = offset;
            orand_tag_length = attr_buffer_ptr[offset++];
        } else if ( attr_buffer_ptr[offset] == GSDI_SC_DO_AND_TAG ) {
            and_tag_present = TRUE;
            offset++;
            orand_tag_length_pos = offset;
            orand_tag_length = attr_buffer_ptr[offset++];
        }

        loop = 0;

        while(loop <= orand_tag_length)
        {
          loop++;
          if ((offset + 1) >= attr_length)
          {
            mmgsdi_status = MMGSDI_ERROR;
            break;
            /* breaking from the inner while loop only */
          }
          sc_do_tag = (mmgsdi_sc_do_tags_eum_type)attr_buffer_ptr[offset++];
          sc_do_length = attr_buffer_ptr[offset++];

          switch (sc_do_tag)
          {
          case MMGSDI_SC_DO_ALWAYS_TAG:
            if(sc_do_length == 0x00)
            {
              key_ref_value = MMGSDI_ALWAYS_ALLOWED;

              /* if access mode has more than one security conditions with OR
                 relation, no need to check remaining security conditions */
              if(or_tag_present == TRUE)
              {
                protection_check_done = TRUE;
              }
            }
            break;

          case MMGSDI_SC_DO_NEVER_TAG:
            if(sc_do_length == 0x00)
            {
              key_ref_value = MMGSDI_NEVER_ALLOWED;

              /* if access mode has more than one security conditions with AND
                 relation, no need to check remaining security conditions */
              if(and_tag_present == TRUE)
              {
                protection_check_done = TRUE;
              }
            }
            break;

          case MMGSDI_SC_DO_KEY_REF_TAG:
            if(offset < attr_length &&
               attr_buffer_ptr[offset++] == GSDI_KEY_REF_TAG)
            {
              if(prev_flg == 1)
              {
                if(or_tag_present)
                {
                  key_ref_value = MMGSDI_OR_ALLOWED;
                }
                else if(and_tag_present)
                {
                  key_ref_value = MMGSDI_AND_ALLOWED;
                }
              }

              /* With below increment, we get offset for PIN data */
              offset++;

              /* Copy PIN data upto 10 because protection_pin_ptr is
                 allocated to store 10 pins only */
              if(offset < attr_length &&
                 cnt < MMGSDI_PIN_LEN_MAX)
              {
                security_pins.num_protection_pin++;
                security_pins.protection_pin_ptr[cnt] = (mmgsdi_pin_enum_type)attr_buffer_ptr[offset];
                cnt = cnt + 1;
              }

              /* With below increment, we get offset for Usage Qualifier */
              offset++;
            }

            /* Skip over Usage Qualifier if applicable*/
            offset = offset + sc_do_length - 3;

            prev_flg = 1;

            break;
          default:
            UIM_MSG_ERR_0("Unrecognized SC_DO tag");
            break;
          } /* switch */

          /* No need to check further if access is always/never allowed */
          if(protection_check_done == TRUE)
          {
            protection_check_done = FALSE;

            /* Reset protection PIN info */
            security_pins.num_protection_pin = 0;
            offset = orand_tag_length_pos + orand_tag_length;

            /* With below increment, we get offset for AM_DO_TAG of next property */
            offset++;
            break;
          }
          loop = loop + sc_do_length + 2;
        }/* while(loop <= orand_tag_length) */
      }

      /* Check for INCREASE access */
      if ((am_do_value & SEC_INC_MSK) == SEC_INC_OPERATION)
      {
        if(security_pins.num_protection_pin > 0)
        {
          pin_length = sizeof(mmgsdi_pin_enum_type)*int32touint32(security_pins.num_protection_pin);
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->increase.protection_pin_ptr);
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
            operation_ptr->increase.protection_pin_ptr,
            pin_length);
          if (operation_ptr->increase.protection_pin_ptr == NULL)
          {
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(security_pins.protection_pin_ptr);
            return MMGSDI_ERROR;
          }
          mmgsdi_status = mmgsdi_uicc_rsp_extract_exp_sec_attr(
                                    &operation_ptr->increase,
                                    key_ref_value,
                                    &security_pins);
          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            /* In case of error, default to NEVER and continue normally */
            UIM_MSG_ERR_0("Invalid increase sec attributes");
            operation_ptr->increase.protection_method = MMGSDI_NEVER_ALLOWED;
            operation_ptr->increase.num_protection_pin = 0;
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->increase.protection_pin_ptr);
          }
          else
          {
            sec_oper_ptr->file_info.cyclic_file.increase_allowed = TRUE;
          }
        }
        else
        {
          operation_ptr->increase.protection_method = key_ref_value;
        }
      }
      /* INCREASE access is indicated using AM_DO tag of '84', where as
         the other access conditions are indicated with an AM_DO tag
         of '80'. Hence INCREASE needs to be handled separately */
      else
      {
        /* Check for READ access */
        if ((am_do_value & SEC_READ_MSK) == SEC_READ_OPERATION)
        {
          if(security_pins.num_protection_pin > 0)
          {
            pin_length = sizeof(mmgsdi_pin_enum_type)*int32touint32(security_pins.num_protection_pin);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->read.protection_pin_ptr);
            MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
              operation_ptr->read.protection_pin_ptr,
              pin_length);
            if (operation_ptr->read.protection_pin_ptr == NULL)
            {
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(security_pins.protection_pin_ptr);
              return MMGSDI_ERROR;
            }
            mmgsdi_status = mmgsdi_uicc_rsp_extract_exp_sec_attr(
                                      &operation_ptr->read,
                                      key_ref_value,
                                      &security_pins);
            if (mmgsdi_status != MMGSDI_SUCCESS)
            {
              /* In case of error, default to NEVER and continue normally */
              UIM_MSG_ERR_0("Invalid read sec attributes");
              operation_ptr->read.protection_method = MMGSDI_NEVER_ALLOWED;
              operation_ptr->read.num_protection_pin = 0;
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->read.protection_pin_ptr);
            }
          }
          else
          {
            operation_ptr->read.protection_method = key_ref_value;
          }
        }
        /* Check for UPDATE access */
        if ((am_do_value & SEC_UPDATE_MSK) == SEC_UPDATE_OPERATION)
        {
          if(security_pins.num_protection_pin > 0)
          {
            pin_length = sizeof(mmgsdi_pin_enum_type)*int32touint32(security_pins.num_protection_pin);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->write.protection_pin_ptr);
            MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
              operation_ptr->write.protection_pin_ptr,
              pin_length);
            if (operation_ptr->write.protection_pin_ptr == NULL)
            {
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(security_pins.protection_pin_ptr);
              return MMGSDI_ERROR;
            }
            mmgsdi_status = mmgsdi_uicc_rsp_extract_exp_sec_attr(
                                      &operation_ptr->write,
                                      key_ref_value,
                                      &security_pins);
            if (mmgsdi_status != MMGSDI_SUCCESS)
            {
              /* In case of error, default to NEVER and continue normally */
              UIM_MSG_ERR_0("Invalid write sec attributes");
              operation_ptr->write.protection_method = MMGSDI_NEVER_ALLOWED;
              operation_ptr->write.num_protection_pin = 0;
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->write.protection_pin_ptr);
            }
          }
          else
          {
            operation_ptr->write.protection_method = key_ref_value;
          }
        }
        /* Check for DEACTIVATE access */
        if ((am_do_value & SEC_DEACT_MSK) == SEC_DEACT_OPERATION)
        {
          if(security_pins.num_protection_pin > 0)
          {
            pin_length = sizeof(mmgsdi_pin_enum_type)*int32touint32(security_pins.num_protection_pin);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->invalidate_deactivate.protection_pin_ptr);
            MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
              operation_ptr->invalidate_deactivate.protection_pin_ptr,
              pin_length);
            if (operation_ptr->invalidate_deactivate.protection_pin_ptr == NULL)
            {
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(security_pins.protection_pin_ptr);
              return MMGSDI_ERROR;
            }
            mmgsdi_status = mmgsdi_uicc_rsp_extract_exp_sec_attr(
                                      &operation_ptr->invalidate_deactivate,
                                      key_ref_value,
                                      &security_pins);
            if (mmgsdi_status != MMGSDI_SUCCESS)
            {
              /* In case of error, default to NEVER and continue normally */
              UIM_MSG_ERR_0("Invalid deactivate sec attributes");
              operation_ptr->invalidate_deactivate.protection_method = MMGSDI_NEVER_ALLOWED;
              operation_ptr->invalidate_deactivate.num_protection_pin = 0;
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->invalidate_deactivate.protection_pin_ptr);
            }
          }
          else
          {
            operation_ptr->invalidate_deactivate.protection_method = key_ref_value;
          }
        }
        /* Check for ACTIVATE access */
        if ((am_do_value & SEC_ACT_MSK) == SEC_ACT_OPERATION)
        {
          if(security_pins.num_protection_pin > 0)
          {
            pin_length = sizeof(mmgsdi_pin_enum_type)*int32touint32(security_pins.num_protection_pin);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->rehabilitate_activate.protection_pin_ptr);
            MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
              operation_ptr->rehabilitate_activate.protection_pin_ptr,
              pin_length);
            if (operation_ptr->rehabilitate_activate.protection_pin_ptr == NULL)
            {
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(security_pins.protection_pin_ptr);
              return MMGSDI_ERROR;
            }
            mmgsdi_status = mmgsdi_uicc_rsp_extract_exp_sec_attr(
                                      &operation_ptr->rehabilitate_activate,
                                      key_ref_value,
                                      &security_pins);
            if (mmgsdi_status != MMGSDI_SUCCESS)
            {
              /* In case of error, default to NEVER and continue normally */
              UIM_MSG_ERR_0("Invalid activate sec attributes");
              operation_ptr->rehabilitate_activate.protection_method = MMGSDI_NEVER_ALLOWED;
              operation_ptr->rehabilitate_activate.num_protection_pin = 0;
              MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(operation_ptr->rehabilitate_activate.protection_pin_ptr);
            }
          }
          else
          {
            operation_ptr->rehabilitate_activate.protection_method = key_ref_value;
          }
        }
      }
    }
    else
    {
      offset++;
      UIM_MSG_ERR_0("Invalid AM DO tag offset");
      mmgsdi_status = MMGSDI_ERROR;
      /* Commented out setting status to GSDI_ERROR */
      /* due to decoding issues.  Won't get         */
      /* important attributes to client due to this */
      /* gsdi_status = GSDI_ERROR;                  */
    }
  } /* while */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(security_pins.protection_pin_ptr);

  if(mmgsdi_status == MMGSDI_ERROR)
  {
    UIM_MSG_ERR_0("Could not decode Exp Sec attr");
    mmgsdi_status = MMGSDI_SUCCESS;
  }

  return mmgsdi_status;

} /* mmgsdi_uicc_rsp_decode_exp_sec_attr */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_READ_ARR()

   DESCRIPTION:
     This function reads an ARR for the reference to expanded format security
     attributes from select response.

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC.
     The path contained in path_data should not start with 3F00.

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            Read failed.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_uicc_rsp_read_arr(
  const mmgsdi_request_header_type  * orig_req_header_ptr,
  const byte                        * attr_buffer_ptr,
  mmgsdi_file_attributes_type       * decoded_attr_ptr,
  mmgsdi_path_type                    path_data,
  mmgsdi_file_enum_type               mmgsdi_file,
  byte                                EF_ARR_record_number
)
{
  mmgsdi_read_req_type                   *req_ptr             = NULL;
  mmgsdi_read_cnf_type                   *cnf_ptr             = NULL;
  mmgsdi_return_enum_type                mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_data_type                       cache_arr_buf;
  uint32                                 total_mem_len        = 0;
  mmgsdi_uim_ef_item_category_enum_type  EF_category          = ROOT;
  uint32                                 num_arr_file         = 0;
  uint16                                 arr_files[MMGSDI_NUM_ARR_FILES];
  mmgsdi_access_type                     arr_access;
  boolean                                found_arr            = FALSE;
  mmgsdi_session_info_type               *session_info_ptr    = NULL;

  memset(arr_files,0x00, sizeof(uint16 )* MMGSDI_NUM_ARR_FILES);
  memset(&arr_access, 0x00, sizeof(mmgsdi_access_type));
  memset(&cache_arr_buf, 0x00, sizeof(mmgsdi_data_type));

  MMGSDIUTIL_RETURN_IF_NULL_3(orig_req_header_ptr, attr_buffer_ptr,
                              decoded_attr_ptr);

  UIM_MSG_HIGH_2("mmgsdi_uicc_rsp_read_arr: ARR=0x%x: RecNum=0x%x",
                 attr_buffer_ptr[0], EF_ARR_record_number);

  /* Get the file id of arr */
  switch(attr_buffer_ptr[0])
  {
    case 0x2F:
      num_arr_file = 1;
      arr_files[0] = (uint16)MMGSDI_ARR_UNDER_MF;

      if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SPECIAL_USIM_ARR, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
      {
        EF_category = MMGSDIFILE_GET_CATEGORY_FROM_FILE(mmgsdi_file);
        if (EF_category == USIM)
        {
          num_arr_file = 2;
          arr_files[1] = (uint16)MMGSDI_ARR_FIRST_DF_LEVEL;
        }
      }
      break;
    case 0x6F:
      EF_category = MMGSDIFILE_GET_CATEGORY_FROM_FILE(mmgsdi_file);
      arr_files[0] = (uint16)MMGSDI_ARR_UNDER_MF;
      if(EF_category == BY_PATH)
      {
        uint8 i = 0;
        /* Max number of ARR files to be checked against */
        num_arr_file = int32touint32(path_data.path_len);
        for (i = 1; i < num_arr_file && i < MMGSDI_NUM_ARR_FILES; i++)
        {
          arr_files[i] = (uint16)MMGSDI_ARR_FIRST_DF_LEVEL;
        }
      }
      else
      {
        num_arr_file = 2;
        arr_files[1] = (uint16)MMGSDI_ARR_FIRST_DF_LEVEL;
      }
      break;
    case 0x4F:
      num_arr_file = 3;
      arr_files[0] = (uint16)MMGSDI_ARR_UNDER_MF;
      arr_files[1] = (uint16)MMGSDI_ARR_FIRST_DF_LEVEL;
      arr_files[2] = (uint16)FLIPW(attr_buffer_ptr);
      break;
    default:
      UIM_MSG_ERR_2("Wrong EF ARR ID 0X%x%x",
                    attr_buffer_ptr[0], attr_buffer_ptr[1]);
      return MMGSDI_ERROR;
  }
  /* read the file*/
  while(num_arr_file)
  {
    /* File ID was Used */
    if( arr_files[num_arr_file-1]== MMGSDI_ARR_UNDER_MF)
    {
      /* EF_ARR_file = MMGSDI_ARR; */
      arr_access.file.file_enum = MMGSDI_ARR;
      arr_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    }
    else if(arr_files[num_arr_file-1] == MMGSDI_ARR_FIRST_DF_LEVEL)
    {
      arr_access.access_method = MMGSDI_EF_ENUM_ACCESS;
      /* Find the DF that the file is located in.  UIM file enum's most significant
         bit carries such info */
      EF_category = MMGSDIFILE_GET_CATEGORY_FROM_FILE(mmgsdi_file);
      if (EF_category == USIM)
      {
        if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SPECIAL_USIM_ARR, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
        {
          if(attr_buffer_ptr[0] == 0x2F)
          {
            arr_access.access_method = MMGSDI_BY_PATH_ACCESS;
            arr_access.file.path_type.path_len = 3;
            arr_access.file.path_type.path_buf[0]= (uint16)MMGSDI_MF_LEVEL;
            arr_access.file.path_type.path_buf[1]= (uint16)MMGSDI_ADF_LEVEL;
            arr_access.file.path_type.path_buf[2]= ((attr_buffer_ptr[0] & 0xFF) << 8) + (attr_buffer_ptr[1] & 0xFF);
          }
          else
          {
          arr_access.file.file_enum = MMGSDI_USIM_ARR;
          }
        } /* mmgsdi feature special_usim_arr enabled */
        else
        {
          arr_access.file.file_enum = MMGSDI_USIM_ARR;
        } /* mmgsdi feature special_usim_arr disabled */
      }
      else if (EF_category == CSIM)
      {
        arr_access.file.file_enum = MMGSDI_CSIM_ARR;
      }
      else if (EF_category == TELECOM)
      {
        arr_access.file.file_enum = MMGSDI_TELECOM_ARR;
      }
      else if(EF_category == ISIM)
      {
        arr_access.file.file_enum = MMGSDI_ISIM_ARR;
      }
      else if( EF_category == BY_PATH)
      {
        /* In this case, path_data (passed as a parameter) will have the
           EF path info, on which a SELECT is requested. */
        mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
          orig_req_header_ptr->session_id,
          &session_info_ptr, NULL);

        if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
        {
          return mmgsdi_status;
        }
        if (path_data.path_len <= 1)
        {
          UIM_MSG_ERR_0("Unable to derive ARR Path invalid path data");
          return MMGSDI_ERROR;
        }
        switch(path_data.path_buf[0])
        {
          case GSDI_MF_PATH_USED:
            /* The UIM ARR File Must be used */
            UIM_MSG_ERR_0("Wrong ARR path for GSDI_MF_PATH_USED");
            return MMGSDI_ERROR;

          case GSDI_TELECOM_PATH_USED:
            /* The TELECOM ARR File Must be used */
            arr_access.file.file_enum = MMGSDI_TELECOM_ARR;
            break;

          case GSDI_USIM_PATH_USED:
            if(path_data.path_buf[1] ==  GSDI_PHONEBOOK_DF_PATH_USED)
            {
              /* GSDI_USIM_PATH_USED is mapped to 0xFFFF for case of accessing
                 local phonebook by path. This should apply for both CSIM/USIM
                 cases.
                 The USIM/CSIM ARR File Must be used. */
              switch(session_info_ptr->session_type)
              {
                case MMGSDI_GW_PROV_PRI_SESSION:
                case MMGSDI_GW_PROV_SEC_SESSION:
                case MMGSDI_GW_PROV_TER_SESSION:
                  arr_access.file.file_enum = MMGSDI_USIM_ARR;
                  break;
                case MMGSDI_1X_PROV_PRI_SESSION:
                case MMGSDI_1X_PROV_SEC_SESSION:
                case MMGSDI_1X_PROV_TER_SESSION:
                  arr_access.file.file_enum = MMGSDI_CSIM_ARR;
                  break;
                default:
                  UIM_MSG_ERR_1("Incorrect session type 0x%x in uicc_rsp_read_arr",
                                session_info_ptr->session_type);
                  return MMGSDI_ERROR;
              }
            }
            else
            {
              UIM_MSG_ERR_1("Calculating ARR: Invalid path starting with: 0x%x",
                            path_data.path_buf[0]);
              return MMGSDI_ERROR;
            }
            break;
          case GSDI_ADF_PATH_USED:
            UIM_MSG_HIGH_1("ARR path used: 0x%x", path_data.path_buf[0]);
            /* Check if it is an EF or a DF under the ADF */
            if(num_arr_file > 2)
            {
              /* It is not the EF */
              uint8 i = 1;
              UIM_MSG_HIGH_0("Calculating ARR for a DF under the ADF");
              arr_access.access_method = MMGSDI_BY_PATH_ACCESS;
              /* Path will change everytime for a nested DF configuration.
                 For example, if the original EF is 3f00/7fff/7f66/5f30/4f4f,
                 we should construct paths for candidate EF ARR (6f06) in this
                 order:
                 3f00/7fff/7f66/5f30/6f06
                 3f00/7fff/7f66/6f06
                 3f00/7fff/6f06
                 before searching the EF ARR at the MF level */
              memset(arr_access.file.path_type.path_buf, 0x00, sizeof(arr_access.file.path_type.path_buf));
              arr_access.file.path_type.path_len = uint32toint32(num_arr_file + 1);
              arr_access.file.path_type.path_buf[0]= (uint16)MMGSDI_MF_LEVEL;
              while (i <= num_arr_file - 1 &&
                     i <= MMGSDI_MAX_PATH_LEN - 2)
              {
                arr_access.file.path_type.path_buf[i] = path_data.path_buf[i-1];
                i++;
              }
              arr_access.file.path_type.path_buf[i]= ((attr_buffer_ptr[0] & 0xFF) << 8) + (attr_buffer_ptr[1] & 0xFF);
            }
            else if(num_arr_file == 2)
            {
              UIM_MSG_HIGH_0("Calculating ARR for an EF under the ADF");
              /* The USIM/CSIM ARR File Must be used   */
              switch(session_info_ptr->session_type)
              {
                case MMGSDI_GW_PROV_PRI_SESSION:
                case MMGSDI_GW_PROV_SEC_SESSION:
                case MMGSDI_GW_PROV_TER_SESSION:
                  arr_access.file.file_enum = MMGSDI_USIM_ARR;
                  break;
                case MMGSDI_1X_PROV_PRI_SESSION:
                case MMGSDI_1X_PROV_SEC_SESSION:
                case MMGSDI_1X_PROV_TER_SESSION:
                  arr_access.file.file_enum = MMGSDI_CSIM_ARR;
                  break;
                case MMGSDI_NON_PROV_SESSION_SLOT_1:
                case MMGSDI_NON_PROV_SESSION_SLOT_2:
                case MMGSDI_NON_PROV_SESSION_SLOT_3:
                  arr_access.access_method = MMGSDI_BY_PATH_ACCESS;
                  arr_access.file.path_type.path_len = 3;
                  arr_access.file.path_type.path_buf[0]= (uint16)MMGSDI_MF_LEVEL;
                  arr_access.file.path_type.path_buf[1]= (uint16)MMGSDI_ADF_LEVEL;
                  arr_access.file.path_type.path_buf[2]= ((attr_buffer_ptr[0] & 0xFF) << 8) + (attr_buffer_ptr[1] & 0xFF);
                  break;
                default:
                  UIM_MSG_ERR_1("Incorrect session type 0x%x in uicc_rsp_read_arr",
                                session_info_ptr->session_type);
                  return MMGSDI_ERROR;
              }
            }
            else
            {
              /* Unknown PATH Used */
              UIM_MSG_ERR_0("Error Calculating ARR for EF/DF under the ADF");
              return MMGSDI_ERROR;
            }
            break;
          default:
            UIM_MSG_ERR_0("Wrong ARR path used!");
            return MMGSDI_ERROR;
        }
      }
      else
      {
        UIM_MSG_ERR_1("wrong EF_category: 0x%x", EF_category);
        return MMGSDI_ERROR;
      }
    }
    else
    {
      /* Was the request for a Select using the
         path for a USIM File */
      if (MMGSDIFILE_GET_TECH_TYPE_FROM_FILE(mmgsdi_file) == MMGSDI_EF_PATH_START)
      {
        UIM_MSG_LOW_0("Path already exists");
      }
      else /* request for select using enum */
      {
        if (path_data.path_len != 0)
        {
          UIM_MSG_ERR_0("path_array should have been empty, instead has some invalid values");
          return MMGSDI_ERROR;
        }
        /* need to get the file path for the enum */
        memset(&path_data, 0x00, sizeof(path_data));

        mmgsdi_status = mmgsdi_file_get_path_from_file(mmgsdi_file,
                                                       &path_data);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_0("Couldn't find the path for the enum");
          return mmgsdi_status;
        }
      }

      if (path_data.path_len <= 1)
      {
        /* Invalid path pointer */
        UIM_MSG_ERR_0("Unable to derive ARR Path invalid path data");
        return MMGSDI_ERROR;
      }

      arr_access.access_method = MMGSDI_BY_PATH_ACCESS;
      arr_access.file.path_type = path_data;
    } /* arr_files[num_arr_files - 1] == 4FXX */

    if(orig_req_header_ptr == NULL)
    {
      UIM_MSG_ERR_0("orig_req_header_ptr is -> NULL");
      return MMGSDI_INCORRECT_PARAMS;
    }

    mmgsdi_status = mmgsdi_cache_get_record_len(orig_req_header_ptr->session_id,
                                                arr_access,
                                                EF_ARR_record_number,
                                                &cache_arr_buf.data_len);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      if (cache_arr_buf.data_len > 0)
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cache_arr_buf.data_ptr,
                                           cache_arr_buf.data_len);
        if (cache_arr_buf.data_ptr == NULL)
        {
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
        mmgsdi_status = mmgsdi_cache_get_record(
                           orig_req_header_ptr->session_id,
                           arr_access, EF_ARR_record_number,
                          &cache_arr_buf);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          found_arr = TRUE;
          mmgsdi_status = mmgsdi_uicc_rsp_decode_exp_sec_attr(
                            cache_arr_buf.data_ptr,
                            cache_arr_buf.data_len,
                            decoded_attr_ptr);
        }
        MMGSDIUTIL_TMC_MEM_FREE(cache_arr_buf.data_ptr);
      }
      return mmgsdi_status;
    }

    /* READ EF ARR file with record number */
    mmgsdi_util_cal_align_size(sizeof(mmgsdi_read_req_type),
                              (int32 *)&total_mem_len);

    /* No variable len to include */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr, total_mem_len);

    if(req_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    req_ptr->request_header.client_id = orig_req_header_ptr->client_id;
    req_ptr->request_header.session_id = orig_req_header_ptr->session_id;
    req_ptr->request_header.request_type = MMGSDI_READ_REQ;
    req_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
    req_ptr->request_header.request_len = sizeof(mmgsdi_read_req_type);
    req_ptr->request_header.payload_len = req_ptr->request_header.request_len -
                                            uint32toint32(sizeof(mmgsdi_read_req_type));
    req_ptr->request_header.slot_id = orig_req_header_ptr->slot_id;
    req_ptr->request_header.client_data = 0;
    req_ptr->request_header.response_cb = NULL;
    mmgsdi_memscpy(&req_ptr->access,
                   sizeof(req_ptr->access),
                   &arr_access,
                   sizeof(mmgsdi_access_type));

    req_ptr->rec_num = EF_ARR_record_number;
    req_ptr->offset = 0;
    req_ptr->file_type = MMGSDI_LINEAR_FIXED_FILE;
    if (EF_category == BY_PATH)
    {
      mmgsdi_status = mmgsdi_uim_uicc_read_record (req_ptr,
                                                   TRUE,
                                                   &cnf_ptr);
    }
    else
    {
      mmgsdi_status = mmgsdi_uim_uicc_read_record (req_ptr,
                                                   TRUE,
                                                   &cnf_ptr);
    }

    if( (mmgsdi_status == MMGSDI_SUCCESS) &&
        (cnf_ptr != NULL) &&
        (cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS))
    {
      found_arr = TRUE;
      mmgsdi_status = mmgsdi_uicc_rsp_decode_exp_sec_attr(
                        cnf_ptr->read_data.data_ptr,
                        cnf_ptr->read_data.data_len,
                        decoded_attr_ptr);

      if (mmgsdi_cache_add_record(orig_req_header_ptr->session_id,
                                  arr_access,
                                  EF_ARR_record_number,
                                  &cnf_ptr->read_data) != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Unable to cache ARR record no: 0x%x",
                    EF_ARR_record_number);
      }
    }

    /* Free the buffer if it was allocated */
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)cnf_ptr));
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);

    if(found_arr)
      break;

    num_arr_file--;
  } /* while (num_arr_file */

  return mmgsdi_status;
}/* mmgsdi_uicc_rsp_read_arr */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_REF_EXP_SEC_ATTR

   DESCRIPTION:
     This function decode the security attribute based on reference expand
     security attribute encoded scheme.  A Read to an ARR is required before
     the parsing of the data

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_uicc_rsp_decode_ref_exp_sec_attr (
  const mmgsdi_request_header_type           * orig_req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type * orig_extra_info_ptr,
  const byte                                 * attr_buffer_ptr,
  int32                                        attr_length,
  mmgsdi_file_attributes_type                * decoded_attr_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_SUCCESS;
  byte                                EF_ARR_record_number = 0;
  byte                                rec_number_offset    = 0;
  mmgsdi_file_enum_type               mmgsdi_file          = MMGSDI_BY_PATH_ENUM;
  mmgsdi_int_app_info_type           *current_app_ptr      = NULL;
  byte                                seid                 = MMGSDI_SEID_00;
  byte                                i                    = 0;
  mmgsdi_session_info_type           *session_info_ptr     = NULL;
  mmgsdi_path_type                    path_data;

  MMGSDIUTIL_RETURN_IF_NULL_3(orig_extra_info_ptr, attr_buffer_ptr, orig_req_header_ptr);

  memset(&path_data, 0x00, sizeof(path_data));

  /* Obtain the enum or path name */
  if (orig_extra_info_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    mmgsdi_file = orig_extra_info_ptr->access.file.file_enum;
  }
  else if (orig_extra_info_ptr->access.access_method == MMGSDI_BY_PATH_ACCESS)
  {
    if(orig_extra_info_ptr->access.file.path_type.path_len > MMGSDI_MAX_PATH_LEN)
    {
      UIM_MSG_ERR_1("Invalid path length: 0x%x",
                    orig_extra_info_ptr->access.file.path_type.path_len);
      return MMGSDI_ERROR;
    }

    path_data = orig_extra_info_ptr->access.file.path_type;
    if(path_data.path_buf[0] == GSDI_MF_PATH_USED &&
       path_data.path_len > 1)
    {
      /* In order to make sure the path sent into mmgsdi_uicc_rsp_read_arr()
         is consistent by_path and by_enum, always make sure tha path does
         not start with MF DIR, that is, 3F00 */
      memset(&path_data, 0x00, sizeof(path_data));
      memscpy(path_data.path_buf,
              sizeof(path_data.path_buf),
              &orig_extra_info_ptr->access.file.path_type.path_buf[1],
              (orig_extra_info_ptr->access.file.path_type.path_len - 1) * sizeof(uint16));
      path_data.path_len = orig_extra_info_ptr->access.file.path_type.path_len - 1;
    }
  }

  if(attr_length == 0x03)
  {
    /*------------------------------------------------------------------
      ETSI 102.221
      11.1.1.4.7.3 ,
      If the Tag is 8B and the Length is 3, then the following applies -
          Tag               - Byte 0
          Length            - Byte 1
          EF ARR File ID    - Byte 2 & Byte 3
          EF ARR Rec Number - Byte 4

          The Length and Tag have already been accounted for
    -------------------------------------------------------------------*/
    rec_number_offset = 2;
  }
  else if(attr_length > 0x03 &&
          (attr_length & 1) == 0)
  {
    /*------------------------------------------------------------------
      ETSI 102.221
      11.1.1.4.7.3 ,
      If the length of the data following tag '8B' is '02' + X × '02'
      the following definition applies -
          Tag               - Byte 0
          Length            - Byte 1
          EF ARR File ID    - Byte 2 & Byte 3
          SEID              - Byte X + 4
          EF ARR Rec Number - Byte X + 5

      -------------------------------------------------------------------*/
    mmgsdi_status =  mmgsdi_util_get_session_and_channel_info(
                       orig_req_header_ptr->session_id,
                       &session_info_ptr,
                       NULL);

    if((mmgsdi_status != MMGSDI_SUCCESS) ||
       (session_info_ptr == NULL) ||
       (session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL) ||
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr  == NULL) ||
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin1_ptr == NULL))
    {
      UIM_MSG_HIGH_0("Assuming default security attributes");
      (void)mmgsdi_file_set_default_sec_attr(decoded_attr_ptr);
      return MMGSDI_SUCCESS;
    }
    current_app_ptr = mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr;
    /*-------------------------------------------------------------------
       Determine the current Security Environment Identifier
     --------------------------------------------------------------------*/
    if(current_app_ptr->pin1_ptr->status != MMGSDI_PIN_DISABLED)
    {
      seid = MMGSDI_SEID_01;
    }
    else
    {
      if(current_app_ptr->universal_pin_ptr)
      {
        if(current_app_ptr->universal_pin_ptr->status != MMGSDI_PIN_DISABLED &&
           current_app_ptr->pin1_ptr->pin_replacement == MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL)
        {
          seid = MMGSDI_SEID_01;
        }
      }
    }

    /*For all other conditions, seid is 0*/
    /*-----------------------------------------------------------------
      Look for SEID in the Select Response. Skip the First two bytes
      The first two bytes is the EF ARR File ID.
      ----------------------------------------------------------------*/
    for(i = 2; i < attr_length; i += 2)
    {
      if(attr_buffer_ptr[i] == seid)
      {
        rec_number_offset = i + 1;
        break;
      }
    }

    if(rec_number_offset == 0)
    {
      UIM_MSG_HIGH_0("ARR record number not found; assuming default security attributes");
      (void)mmgsdi_file_set_default_sec_attr(decoded_attr_ptr);
      return MMGSDI_SUCCESS;
    }
  }
  else
  {
    UIM_MSG_ERR_1("Bad attr_length: 0x%x", attr_length);
    return MMGSDI_ERROR;
  }

  EF_ARR_record_number = attr_buffer_ptr[rec_number_offset];

  /* If EF_ARR record number is indicated as 0, do not send a read request
     to the card, return immediately with default security attributes */
  if(EF_ARR_record_number == 0)
  {
    UIM_MSG_HIGH_0("EF_ARR record number is 0, assuming default security attributes");
    (void)mmgsdi_file_set_default_sec_attr(decoded_attr_ptr);
    return MMGSDI_SUCCESS;
  }

  /* Skip reading EF-ARR for sec sttributes for internal select calls */
  if (orig_extra_info_ptr->skip_uicc_arr)
  {
    UIM_MSG_HIGH_0("Skipping reading EF_ARR, assuming default security attributes");
    (void)mmgsdi_file_set_default_sec_attr_per_spec(decoded_attr_ptr,
                                                    orig_extra_info_ptr->access.file.file_enum);

    return MMGSDI_SUCCESS;
  }

  mmgsdi_status = mmgsdi_uicc_rsp_read_arr(orig_req_header_ptr,
                                           attr_buffer_ptr,
                                           decoded_attr_ptr,
                                           path_data,
                                           mmgsdi_file,
                                           EF_ARR_record_number
                                           );

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_decode_ref_exp_sec_attr */ /*lint !e715 */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_CMP_CMD_SEC_ATTR

   DESCRIPTION:
     This function populates the mmgsdi file security type with
     data from the EF header information according to compact
     security attribute encoding scheme

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_uicc_rsp_decode_cmp_cmd_sec_attr(
  int32                     *num_sc_bytes_referenced_ptr,
  int32                      num_of_sc_bytes,
  const uint8               *sc_bytes_ptr,
  mmgsdi_file_security_type *command_sec_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL_3(num_sc_bytes_referenced_ptr, sc_bytes_ptr,
                              command_sec_ptr);

  if(*num_sc_bytes_referenced_ptr < num_of_sc_bytes)
  {
    if(sc_bytes_ptr[*num_sc_bytes_referenced_ptr] == 0x00)
    {
      /* ALWAYS */
      command_sec_ptr->protection_method = MMGSDI_ALWAYS_ALLOWED;
      command_sec_ptr->num_protection_pin = 0;
    }
    else if(sc_bytes_ptr[*num_sc_bytes_referenced_ptr] == 0x10)
    {
      /* User PIN verification */
      command_sec_ptr->protection_method = MMGSDI_OR_ALLOWED;
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        command_sec_ptr->protection_pin_ptr, sizeof(mmgsdi_pin_enum_type));
      if(command_sec_ptr->protection_pin_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      command_sec_ptr->protection_pin_ptr[0] = MMGSDI_PIN1;
      command_sec_ptr->num_protection_pin = 1;
    }
    else if(sc_bytes_ptr[*num_sc_bytes_referenced_ptr] == 0x90)
    {
      /* ADM verification */
      command_sec_ptr->protection_method  = MMGSDI_OR_ALLOWED;
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        command_sec_ptr->protection_pin_ptr, sizeof(mmgsdi_pin_enum_type));
      if(command_sec_ptr->protection_pin_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      command_sec_ptr->protection_pin_ptr[0] = MMGSDI_ADM1;
      command_sec_ptr->num_protection_pin = 1;
    }
    else if(sc_bytes_ptr[*num_sc_bytes_referenced_ptr] == 0xFF)
    {
      /* NEVER */
      command_sec_ptr->protection_method = MMGSDI_NEVER_ALLOWED;
      command_sec_ptr->num_protection_pin = 0;
    }
    (*num_sc_bytes_referenced_ptr)++;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_uicc_rsp_decode_cmp_cmd_sec_attr */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_CMP_SEC_ATTR

   DESCRIPTION:
     This function decodes the EF header information according to compact
     security attribute encoding scheme

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_uicc_rsp_decode_cmp_sec_attr (
  const uint8                 *attr_buffer_ptr,
  int32                        attr_length,
  mmgsdi_file_attributes_type *decoded_attr_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status           = MMGSDI_SUCCESS;
  int32                             num_of_sc_bytes         = 0;
  int32                             last_buffer_index       = 0;
  /* Following array is of size 7 because of this text in 102221 and 7816-4:
     'the number of SC bytes following, this is equal to the number of bits
     set to '1' in bits b7 to b1 in the AM byte.'. */
  uint8                             sc_bytes[7];
  int32                             num_sc_bytes_referenced = 0;
  uint8                             am_byte                 = 0;
  int32                             i                       = 0;
  mmgsdi_file_security_access_type *operation_ptr           = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_2(attr_buffer_ptr, decoded_attr_ptr);

  /* HERE <- Put some checks for length == 0 and length == 1 */
  if(attr_length > 0)
  {
    am_byte = attr_buffer_ptr[0];
  }
  else
  {
    UIM_MSG_ERR_0("Sec attribute length is zero");
    mmgsdi_status = MMGSDI_ERROR;
    return mmgsdi_status;
  }

  switch(decoded_attr_ptr->file_type)
  {
  case MMGSDI_LINEAR_FIXED_FILE:
    operation_ptr = &decoded_attr_ptr->file_info.linear_fixed_file.file_security;
    break;
  case MMGSDI_CYCLIC_FILE:
    operation_ptr = &decoded_attr_ptr->file_info.cyclic_file.file_security;
    break;
  case MMGSDI_TRANSPARENT_FILE:
    operation_ptr = &decoded_attr_ptr->file_info.transparent_file.file_security;
    break;
  default:
    return MMGSDI_ERROR;
  }

  /* Get the SC byte(s) and place in an array of bytes,
     where array[0] is last SC, array[1] is next-to-last SC, etc.
     The number of SC bytes in compact format FCP tag 8C is variable, at
     max it can be 7... this is the text from spec: 'the number of SC bytes
     following, this is equal to the number of bits set to '1' in bits b7 to b1
     in the AM byte.'.
     NOTE that In order to accomodate any non-spec compliant card that has more
     than 7 bytes for SC, we truncate the num of SC bytes to 7 instead of
     returning failure. */

  memset(sc_bytes, 0x00, sizeof(sc_bytes));

  /* How many SC bytes do we have. */
  num_of_sc_bytes = (attr_length - 1);
  if(num_of_sc_bytes > sizeof(sc_bytes))
  {
    /* This is a non-spec compliant card */
    num_of_sc_bytes = sizeof(sc_bytes);
  }

  /* Now lets place the SC bytes into an SC array */
  last_buffer_index = num_of_sc_bytes;
  for ( i=0; i < num_of_sc_bytes; i++)
  {
    sc_bytes[i] = attr_buffer_ptr[last_buffer_index--];
  }

  /* Go through the AM byte bit map and assoicate each bit set
    starting with b1 thru b8 to SC array */

  /* '00000001' - READ/SEARCH   */
  /* '00000010' - UPDATE      */
  /* '00001000' - DEACTIVATE  */
  /* '00010000' - ACTIVATE    */

  /* INCREASE is NOT specified in ISO 7816-9 AM byte definition */
  /* set INCREASE key ref to default - NEVER */
  operation_ptr->increase.protection_method = MMGSDI_NEVER_ALLOWED;
  operation_ptr->increase.num_protection_pin = 0;

  /* Initialize to NEVER */
  operation_ptr->read.protection_method = MMGSDI_NEVER_ALLOWED;
  operation_ptr->read.num_protection_pin = 0;
  operation_ptr->write.protection_method = MMGSDI_NEVER_ALLOWED;
  operation_ptr->write.num_protection_pin = 0;
  operation_ptr->invalidate_deactivate.protection_method = MMGSDI_NEVER_ALLOWED;
  operation_ptr->invalidate_deactivate.num_protection_pin = 0;
  operation_ptr->rehabilitate_activate.protection_method = MMGSDI_NEVER_ALLOWED;
  operation_ptr->rehabilitate_activate.num_protection_pin = 0;

  /* Check if READ/SEEK bit is set */
  if((am_byte & 0x01) == 0x01)
  {
    mmgsdi_status = mmgsdi_uicc_rsp_decode_cmp_cmd_sec_attr(
      &num_sc_bytes_referenced,
      num_of_sc_bytes,
      sc_bytes,
      &operation_ptr->read);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  } /* am_byte & 0x01 */

  /* Check if UPDATE bit is set */
  if((am_byte & 0x02) == 0x02)
  {
    mmgsdi_status = mmgsdi_uicc_rsp_decode_cmp_cmd_sec_attr(
      &num_sc_bytes_referenced,
      num_of_sc_bytes,
      sc_bytes,
      &operation_ptr->write);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  } /* am_byte & 0x2 */


  /* Check if DEACTIVATE bit is set */
  if((am_byte & 0x08) == 0x08)
  {
    mmgsdi_status = mmgsdi_uicc_rsp_decode_cmp_cmd_sec_attr(
      &num_sc_bytes_referenced,
      num_of_sc_bytes,
      sc_bytes,
      &operation_ptr->invalidate_deactivate);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  } /* am_byte & 0x8 */


  /* Check if ACTIVATE bit is set */
  if((am_byte & 0x10) == 0x10)
  {
    mmgsdi_status = mmgsdi_uicc_rsp_decode_cmp_cmd_sec_attr(
      &num_sc_bytes_referenced,
      num_of_sc_bytes,
      sc_bytes,
      &operation_ptr->rehabilitate_activate);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  } /* am_byte & 0x10 */
  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_decode_cmp_sec_attr */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_FREE_PIN_PROTECTION_INFO

   DESCRIPTION:
     This function populate the corresponding command (read/write/activate/
     deactivate/increase etc)'s key reference into mmgsdi file security struct

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
void mmgsdi_uicc_rsp_free_pin_protection_info(
  mmgsdi_file_attributes_type *file_attr_ptr)
{
  mmgsdi_file_security_access_type *file_security_access_ptr = NULL;

  if(NULL ==file_attr_ptr)
  {
    return;
  }

  switch (file_attr_ptr->file_type)
  {
  case MMGSDI_LINEAR_FIXED_FILE:
    file_security_access_ptr =
      &file_attr_ptr->file_info.linear_fixed_file.file_security;
    break;
  case MMGSDI_CYCLIC_FILE:
    file_security_access_ptr =
      &file_attr_ptr->file_info.cyclic_file.file_security;
    break;
  case MMGSDI_TRANSPARENT_FILE:
    file_security_access_ptr =
      &file_attr_ptr->file_info.transparent_file.file_security;
    break;
  default:
    return;
  }
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(file_security_access_ptr->increase.protection_pin_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(file_security_access_ptr->read.protection_pin_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(file_security_access_ptr->write.protection_pin_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    file_security_access_ptr->invalidate_deactivate.protection_pin_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    file_security_access_ptr->rehabilitate_activate.protection_pin_ptr);

  file_security_access_ptr->increase.num_protection_pin = 0;
  file_security_access_ptr->read.num_protection_pin = 0;
  file_security_access_ptr->write.num_protection_pin = 0;
  file_security_access_ptr->invalidate_deactivate.num_protection_pin = 0;
  file_security_access_ptr->rehabilitate_activate.num_protection_pin = 0;
} /* mmgsdi_uicc_rsp_free_pin_protection_info */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_EF_HEADER_DATA

   DESCRIPTION:
     This function decodes the EF header information into mmgsdi file attribute
     structure

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_uicc_rsp_decode_EF_header_data (
  const mmgsdi_request_header_type           * orig_req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type * orig_extra_info_ptr,
  const uint8                                * header_content_ptr,
  int32                                        header_length,
  mmgsdi_file_attributes_type                * decoded_data_ptr
)
{
  mmgsdi_return_enum_type  mmgsdi_status           = MMGSDI_SUCCESS;
  int32                    file_descriptor_offset  = 0;
  int32                    file_identifier_offset  = 0;
  int32                    proprietory_offset      = 0;
  int32                    life_cycle_offset       = 0;
  int32                    security_attr_offset    = 0;
  int32                    file_size_offset        = 0;
  int32                    total_file_size_offset  = 0;
  int32                    sfi_offset              = 0;
  uint16                   total_len               = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(header_content_ptr, decoded_data_ptr);

   /* First byte should be the FCP templage tag */
  if(header_content_ptr[0] != GSDI_FCP_TEMPLATE_TAG)
  {
    return MMGSDI_ERROR;
  }

  /* FCP template length is determined by byte 2 or 3 of the data
     as indicated in TS 102.221 */
  /* FCP template length stored in byte 2 is between 0-127 bytes,
     FCP template length stored in byte 3 is between 128-256 bytes,
     as specified by TS 102.220 */
  if(header_length >= 3 &&
     header_content_ptr[2] == GSDI_FILE_DESCRIPTOR_TAG)
  {
    /* One byte field for FCP length */
    file_descriptor_offset = 2;
    total_len = header_content_ptr[1] + 2;
  }
  else if(header_length >= 4 &&
          header_content_ptr[1] == GSDI_LEN_128_255_ENCODE_TAG &&
          header_content_ptr[3] == GSDI_FILE_DESCRIPTOR_TAG)
  {
    /* Two byte field for FCP length  */
    file_descriptor_offset = 3;
    total_len = header_content_ptr[2] + 3;
  }
  else
  {
    UIM_MSG_ERR_0("Invalid FCP Template length parameter");
    return MMGSDI_ERROR;
  }

  if (total_len > header_length)
  {
    /* There are commercial cards that send an invalid length of the TLV, even
       if TLV is correctly formatted and can be decoded */
    UIM_MSG_ERR_2("Invalid card data, Assigning header_len: 0x%x to total_len: 0x%x",
                  header_length, total_len);
    total_len = header_length;
  }

  /* GSDI_FILE_DESCRIPTOR_TAG */
  /* File Descriptor Tag (Mandatory) */
  /* Data Coding Byte to be 0x21 is not mandatory as per TS 102 221
     "The actual value shall not be checked nor used by the terminal"*/
  if(total_len > (file_descriptor_offset + 6) &&
     header_content_ptr[file_descriptor_offset] == GSDI_FILE_DESCRIPTOR_TAG &&
     (header_content_ptr[file_descriptor_offset + 1] == 2 ||
      header_content_ptr[file_descriptor_offset + 1] == 5))
  {
    /* File descriptor byte */
    mmgsdi_status = mmgsdi_uicc_rsp_get_file_des(
                      header_content_ptr[file_descriptor_offset + 2],
                      decoded_data_ptr);
    /* Data coding byte: file_descriptor_offset + 3 is not supported */

    /* Length is 2 or 5 bytes, if 5 bytes Optional record info is present */
    mmgsdi_status = mmgsdi_uicc_rsp_get_file_rec_info(
      header_content_ptr[file_descriptor_offset + 1],
      header_content_ptr[file_descriptor_offset + 4],
      header_content_ptr[file_descriptor_offset + 5],
      header_content_ptr[file_descriptor_offset + 6],
      header_content_ptr[file_descriptor_offset + 2],
      decoded_data_ptr);
  }
  else
  {
    UIM_MSG_ERR_0("Invalid File Descriptor Data");
    mmgsdi_status = MMGSDI_ERROR;
  }/* GSDI_FILE_DESCRIPTOR_TAG */
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* File Identifier Tag (Mandatory) */
  file_identifier_offset = (file_descriptor_offset + 1) +
                            header_content_ptr[file_descriptor_offset + 1] + 1;
  if(total_len > (file_identifier_offset + 3) &&
     header_content_ptr[file_identifier_offset] == GSDI_FILE_IDENTIFIER_TAG &&
     header_content_ptr[file_identifier_offset + 1] == 2)
  {
    decoded_data_ptr->file_id[0] = header_content_ptr[file_identifier_offset + 2];
    decoded_data_ptr->file_id[1] = header_content_ptr[file_identifier_offset + 3];

    /*If the file identifier is 0x3F00 (for MF) and the file type is DF, update
     the file type to MF*/
    if(decoded_data_ptr->file_id[0] == 0x3F &&
       decoded_data_ptr->file_id[1] == 0x00 &&
       decoded_data_ptr->file_type == MMGSDI_DEDICATED_FILE)
    {
      decoded_data_ptr->file_type = MMGSDI_MASTER_FILE;
    }
  }
  else
  {
    UIM_MSG_ERR_0("Invalid File Identifier Data");
    return MMGSDI_ERROR;
  } /* GSDI_FILE_IDENTIFIER_TAG */

  /* Optional proprietary info tag */
  /* Life Cycle offset */
  proprietory_offset = (file_identifier_offset + 1) +
                        header_content_ptr[file_identifier_offset + 1] + 1;

  /* The tag GSDI_PROPRIETARY_INFO_BUGGY_TAG(0x25) is added to support a
     commercial USIM card. This has 0x25 as tag instead of proprietary info tag
     in 3rd data object TLV. As this is a commercial card we allow the parsing
     of TLV to proceed further.*/
  if (total_len > (proprietory_offset + 1) &&
     ((header_content_ptr[proprietory_offset] == GSDI_PROPRIETARY_INFO_TAG) ||
      (header_content_ptr[proprietory_offset] == GSDI_PROPRIETARY_INFO_BUGGY_TAG)))
  {
    /* increment the length */
    life_cycle_offset = (proprietory_offset + 1) +
                        header_content_ptr[proprietory_offset+1] + 1;
  }
  else
  {
    life_cycle_offset = proprietory_offset;
  } /* GSDI_PROPRIETARY_INFO_TAG */

  /* Mandatory life cyclic flag */
  if (total_len > (life_cycle_offset + 2) &&
      header_content_ptr[life_cycle_offset] == GSDI_LIFE_CYCLE_INTEG_TAG &&
      header_content_ptr[life_cycle_offset + 1] == 1)
  {
    mmgsdi_status = mmgsdi_uicc_rsp_get_life_cycle_info(
                      header_content_ptr[life_cycle_offset + 2],
                      decoded_data_ptr);
  }
  else
  {
    UIM_MSG_ERR_0("Invalid Life Status Data");
    return MMGSDI_ERROR;
  } /* GSDI_LIFE_CYCLE_INTEG_TAG */

  /* Security attributes Tag (Mandatory) */
  security_attr_offset = (life_cycle_offset +1 ) +
                         header_content_ptr[life_cycle_offset + 1] + 1;

  /* Security Attribute Type */
  if(total_len > security_attr_offset &&
     header_content_ptr[security_attr_offset] == GSDI_SEC_ATTR_PROP_TAG)
  {
    /* Proprietary Security Attribute */
    /* Not supported at this time */
    mmgsdi_status = MMGSDI_ERROR;
  }
  else if(total_len > (security_attr_offset + 2) &&
          header_content_ptr[security_attr_offset] == GSDI_SEC_ATTR_CMPACT_TAG &&
          total_len > (security_attr_offset + header_content_ptr[security_attr_offset+1] + 1))
  {
    /* Compact Format Security Attribute */
    mmgsdi_status = mmgsdi_uicc_rsp_decode_cmp_sec_attr(
                            &header_content_ptr[security_attr_offset + 2],
                            header_content_ptr[security_attr_offset + 1],
                            decoded_data_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_uicc_rsp_free_pin_protection_info(decoded_data_ptr);
    }
  }
  else if(total_len > (security_attr_offset + 2) &&
          header_content_ptr[security_attr_offset] == GSDI_SEC_ATTR_EXPAND_TAG &&
          total_len > (security_attr_offset + header_content_ptr[security_attr_offset + 1] + 1))
  {
    /* Expanded Format Security Attribute */
    mmgsdi_status = mmgsdi_uicc_rsp_decode_exp_sec_attr(
                            &header_content_ptr[security_attr_offset + 2],
                            header_content_ptr[security_attr_offset + 1],
                            decoded_data_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_uicc_rsp_free_pin_protection_info(decoded_data_ptr);
    }
  }
  else if(total_len > (security_attr_offset + 2) &&
          header_content_ptr[security_attr_offset] == GSDI_SEC_ATTR_REF_EXP_TAG &&
          total_len > (security_attr_offset + header_content_ptr[security_attr_offset + 1] + 1))
  {
    /* Referenced to Expanded Format */
    mmgsdi_status = mmgsdi_uicc_rsp_decode_ref_exp_sec_attr(
                            orig_req_header_ptr,
                            orig_extra_info_ptr,
                            &header_content_ptr[security_attr_offset + 2],
                            header_content_ptr[security_attr_offset + 1],
                            decoded_data_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_uicc_rsp_free_pin_protection_info(decoded_data_ptr);
    }
  }
  else
  {
    UIM_MSG_ERR_0("Invalid Security Attribure Data");
    mmgsdi_status = MMGSDI_ERROR;
  }

  /* Proceed even if there is an error decoding the security attributes */

  /* File size Tag (Mandatory)
     Note: Certain commercial cards use alternate, non-spec complient values for the
           file size tag so also check for alternate fize size tags. */
  file_size_offset = (security_attr_offset+1) +
                     header_content_ptr[security_attr_offset + 1] + 1;
  if(total_len > (file_size_offset + 3) &&
     (header_content_ptr[file_size_offset] == GSDI_FILE_SIZE_TAG ||
      header_content_ptr[file_size_offset] == GSDI_ALT_FILE_SIZE_TAG) &&
     total_len > file_size_offset + header_content_ptr[file_size_offset + 1] + 1)
  {
    uint8  i  = 0;

    decoded_data_ptr->file_size = 0;

    for (i = 0; i < header_content_ptr[file_size_offset + 1] &&
                i < sizeof(decoded_data_ptr->file_size); i++)
    {
      decoded_data_ptr->file_size =
        (decoded_data_ptr->file_size << 8) +
        header_content_ptr[file_size_offset + 2 + i];
    }
  }
  else
  {
    UIM_MSG_ERR_0("Invalid File Size Tag");
    return MMGSDI_ERROR;
  } /* GSDI_FILE_SIZE_TAG */

  /* Optional total file size tag */
  total_file_size_offset = file_size_offset + 1 +
                           header_content_ptr[file_size_offset + 1] + 1;

  if (total_len > (total_file_size_offset + 1) &&
      (header_content_ptr[total_file_size_offset] == GSDI_TOTAL_FILE_SIZE_TAG))
  {
    /* increment the length */
    sfi_offset = total_file_size_offset + 1 +
                 header_content_ptr[total_file_size_offset+1] + 1;
  }
  else
  {
    sfi_offset = total_file_size_offset;
  } /* GSDI_TOTAL_FILE_SIZE_TAG */

  /* Optional Short File Indicator (SFI) tag */
  if (total_len > (sfi_offset + 2) &&
      header_content_ptr[sfi_offset] == GSDI_SHORT_FILE_ID_TAG)
  {
    if(header_content_ptr[sfi_offset + 1] == 0)
    {
      /* SFI not supported */
      decoded_data_ptr->sfi = MMGSDI_INVALID_SFI;
    }
    else if(header_content_ptr[sfi_offset + 1] == 1)
    {
      /* SFI is contained in the first 5 significant bits (bits b8 to b4)...
         hence, right shift by three bits so we have bits b8 to b4 */
      decoded_data_ptr->sfi = header_content_ptr[sfi_offset + 2] >> 3;
      /* Validate the SFI */
      if(decoded_data_ptr->sfi > MMGSDI_VALID_MAX_SFI)
      {
        decoded_data_ptr->sfi = MMGSDI_INVALID_SFI;
      }
    }
    else
    {
      UIM_MSG_ERR_0("Invalid SFI tag data");
      /* Being conservative here and not returning error here to cover any
         out of spec card. */
    }
  } /* GSDI_SHORT_FILE_ID_TAG */

  return MMGSDI_SUCCESS;
} /* mmgsdi_uicc_rsp_decode_EF_header_data */ /*lint !e715 */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_SET_PIN_STATUS
   DESCRIPTION:
     This function sets the pin status in the mmgsdi_data_slot1
   DEPENDENCIES:
   LIMITATIONS:
     This function is limited to the use of UICC
   RETURN VALUE:
   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static void mmgsdi_uicc_rsp_set_pin_status(mmgsdi_int_app_info_type *aid_ptr,
                                    mmgsdi_pin_enum_type             pin_type,
                                    byte                             pin_bit_value,
                                    byte                             ps_do_byte_field,
                                    byte                             usage_qualifier
)
{
  switch(pin_type)
  {
    case MMGSDI_PIN1:
      aid_ptr->pin1_ptr->pin_id  = pin_type;
      if ((pin_bit_value & ps_do_byte_field) !=0){
        /* If the PIN is already changed from not init, then do not
           change the status, this maybe a new select on an already
           selected application*/
        if(aid_ptr->pin1_ptr->status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
        {
          aid_ptr->pin1_ptr->status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
        }
      }
      else{
        if(aid_ptr->pin1_ptr->status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
        {
          aid_ptr->pin1_ptr->status = MMGSDI_PIN_DISABLED;
        }
      }
      if(aid_ptr->universal_pin_ptr)
      {
        /*PIN 1 may not be available when we were checking for replacement*/
        aid_ptr->pin1_ptr->pin_replacement = aid_ptr->universal_pin_ptr->pin_replacement;
      }

      break;
    case MMGSDI_PIN2:
      aid_ptr->pin2.pin_id  = pin_type;
      aid_ptr->pin2.pin_replacement = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;
      if ((pin_bit_value & ps_do_byte_field) !=0){
        /* If the PIN is already changed from not init, then do not
           change the status, this maybe a new select on an already
           selected application*/
        if(aid_ptr->pin2.status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
        {
          aid_ptr->pin2.status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
        }
      }
      else{
        if(aid_ptr->pin2.status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
        {
          aid_ptr->pin2.status = MMGSDI_PIN_DISABLED;
        }
      }
      break;
    case MMGSDI_UNIVERSAL_PIN:
      aid_ptr->universal_pin_ptr->pin_id  = pin_type;
      if ((pin_bit_value & ps_do_byte_field) !=0){
        /* If the PIN is already changed from not init, then do not
           change the status, this maybe a new select on an already
           selected application*/
        if(aid_ptr->universal_pin_ptr->status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
        {
          aid_ptr->universal_pin_ptr->status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
        }
      }
      else{
        if(aid_ptr->universal_pin_ptr->status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
        {
          aid_ptr->universal_pin_ptr->status = MMGSDI_PIN_DISABLED;
        }
      }

      aid_ptr->universal_pin_ptr->pin_replacement = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;

      if ((usage_qualifier & GSDI_USAGE_QUAL_USER_VERIFICATON) == GSDI_USAGE_QUAL_USER_VERIFICATON)
      {
        aid_ptr->universal_pin_ptr->pin_replacement = MMGSDI_PIN_REPLACED_BY_UNIVERSAL;
        UIM_MSG_HIGH_1("PIN1 replaced by Universal PIN, usage qualifier = 0x%x",
                       usage_qualifier);
      }

      if(aid_ptr->pin1_ptr)
      {
        aid_ptr->pin1_ptr->pin_replacement = aid_ptr->universal_pin_ptr->pin_replacement;
      }
      else
      {
        UIM_MSG_ERR_1("PIN1 ptr NULL, unable to assign pin replacement info usage qualifier = 0x%x",
                      usage_qualifier);
      }
      break;
    default:
      UIM_MSG_ERR_1("UICC RSP - SET PIN STATUS  Invalid PIN ID: 0x%x", pin_type);
      break;
  }

} /* mmgsdi_uicc_rsp_set_pin_status */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_SET_DEFAULT_FCP

   DESCRIPTION:
     This function populates the FCP information if the FCP template is missing
     from SELECT response.

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static void mmgsdi_uicc_rsp_set_default_fcp(
  mmgsdi_int_app_info_type   *app_info_ptr,
  mmgsdi_app_pin_info_type  **app_pin1_tbl_pptr,
  uint8                       app_pin1_tbl_size,
  mmgsdi_slot_id_enum_type    slot
)
{
  uint32                      pin_index         = 0;
  mmgsdi_int_app_info_type  * ch_zero_app_ptr   = NULL;

  if (app_info_ptr == NULL ||
      app_pin1_tbl_pptr == NULL)
  {
    return;
  }

  /* If the app info already has valid PIN info, then return, this may be
     a new select on an already selected application */
  if(app_info_ptr->pin1_ptr != NULL)
  {
    UIM_MSG_HIGH_0("No need to set Default FCP, PIN info already available for app");
    return;
  }

  /* Indicate that PIN data for the app was not extracted from FCP */
  app_info_ptr->fcp_pin_data_absent = TRUE;

  /* Ignoring return type since failure to get channel zero app ptr
     will result in a NULL ptr response */
  (void)mmgsdi_util_get_ch_zero_app (slot, &ch_zero_app_ptr, NULL);

  if (ch_zero_app_ptr)
  {
    /* An app has been selected on channel zero. Set the non-telecom app's
       pin status to reflect that of the channel zero app */
    UIM_MSG_HIGH_0("Setting PIN1 and PIN2 same as the default channel app");
    app_info_ptr->pin1_ptr = ch_zero_app_ptr->pin1_ptr;
    mmgsdi_memscpy((void *)&app_info_ptr->pin2,
                   sizeof(app_info_ptr->pin2),
                   (void *)&ch_zero_app_ptr->pin2,
                   sizeof(mmgsdi_app_pin_info_type));
  }
  else
  {
    /* No app has been selected on the default channel yet so setting key ref
       for PIN1 as MMGSDI_PIN_APPL_1 and PIN2 as MMGSDI_SEC_PIN_APPL_1
       respectively */
    UIM_MSG_HIGH_0("Setting PIN1 keyref PIN_APPL_1,PIN2 keyref SEC_PIN_APPL_1");
    /* Ignoring return type since app pin table has only 8 entries and
       one of them will match MMGSDI_PIN_APPL_1 key ref */
    (void)mmgsdi_util_find_app_pin_index((const mmgsdi_app_pin_info_type **)app_pin1_tbl_pptr,
                                         app_pin1_tbl_size,
                                         MMGSDI_PIN_APPL_1,
                                         &pin_index);
    app_info_ptr->pin1_ptr = app_pin1_tbl_pptr[pin_index];

    memset((void *)&app_info_ptr->pin2, 0x0, sizeof(mmgsdi_app_pin_info_type));
    app_info_ptr->pin2.pin_id  = MMGSDI_PIN2;
    app_info_ptr->pin2.key_ref = MMGSDI_SEC_PIN_APPL_1;
  }
}/* mmgsdi_uicc_rsp_set_default_fcp */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_PIN_HEADER_DATA

   DESCRIPTION:
     This function decodes the EF header information into mmgsdi file attribute
     structure

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The calling function must enter mmgsdi_client_app_data_crit_sect_ptr crit
     sect before invoking this util function. Protection for app info ptr.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_decode_pin_header_data (
  const mmgsdi_request_header_type           * orig_req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type * orig_extra_info_ptr,
  const uint8                                * header_content_ptr,
  int32                                        header_length
)
{
  mmgsdi_return_enum_type           mmgsdi_status                 = MMGSDI_SUCCESS;
  int32                             i                             = 0;
  mmgsdi_int_app_info_type         *app_info_ptr                  = NULL;
  byte                              ps_do_byte_field              = 0;
  byte                              ps_do_byte_field_lgth         = 0;
  byte                              ps_do_byte_field_value_offset = 0;
  byte                              num_of_key_params;
  uint32                            pin_index                     = 0;
  uint32                            aid_index                     = 0;
  mmgsdi_aid_type                   aid;
  byte                              usage_qualifier               = 0;
  byte                              usage_qualifier_lgth          = 0;
  boolean                           app_pin1_match                = FALSE;
  boolean                           app_pin2_match                = FALSE;
  mmgsdi_app_pin_info_type        **app_pin1_tbl_pptr             = NULL;
  uint8                             app_pin1_tbl_size             = 0;
  mmgsdi_app_pin_info_type         *app_univ_pin_tbl_ptr          = NULL;
  mmgsdi_protocol_enum_type         protocol                      = MMGSDI_NO_PROTOCOL;
  mmgsdi_key_ref_values_enum_type   orig_pin2_key_ref             = MMGSDI_SEC_PIN_APPL_1;
  uint8                             slot_index                    = MMGSDI_SLOT_1_INDEX;
  uint16                            offset                        = 0;

  MMGSDIUTIL_RETURN_IF_NULL_3(header_content_ptr, orig_extra_info_ptr,
                              orig_req_header_ptr);

  mmgsdi_memscpy(&aid.aid,
                 sizeof(aid.aid),
                 &orig_extra_info_ptr->access.file.app_id,
                 sizeof(mmgsdi_static_data_type));
  aid_index = mmgsdi_util_find_aid_index(
                orig_req_header_ptr->slot_id,
                aid);

  if (aid_index >=  MMGSDI_MAX_EXT_APP_INFO)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(orig_req_header_ptr->slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  app_pin1_tbl_pptr = mmgsdi_util_get_app_pin1_info_tbl_ptr(&app_pin1_tbl_size,
                                                            orig_req_header_ptr->slot_id);
  app_info_ptr = mmgsdi_data_slot_ptr[slot_index]->app_info_pptr[aid_index];
  app_univ_pin_tbl_ptr = mmgsdi_app_univ_pin_table_ptr[slot_index];
  protocol = mmgsdi_data_slot_ptr[slot_index]->protocol;

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(app_info_ptr, MMGSDI_ERROR);

  /* Reset PIN info corresponding to the non-prov app in ICC mode. This is
     done to avoid having incorrect PIN data in MMGSDI */
  if((orig_req_header_ptr->orig_request_type == MMGSDI_SESSION_OPEN_REQ ||
      orig_req_header_ptr->orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ ||
      orig_req_header_ptr->orig_request_type == MMGSDI_SESSION_SELECT_AID_REQ) &&
     protocol == MMGSDI_ICC)
  {
    app_info_ptr->pin1_ptr = NULL;
    memset(&app_info_ptr->pin2, 0x00, sizeof(mmgsdi_app_pin_info_type));
    return MMGSDI_SUCCESS;
  }

  orig_pin2_key_ref = app_info_ptr->pin2.key_ref;

  /* Validate header length
     The length of FCP can be expressed in 1 or 2 bytes
     According to ETSI 102.221, if the length is <= 0x7F,
     it is encoded in only 1 byte. If length is between 0x80
     and 0xFF, it is decoded in two bytes with first byte as
     0x81 and the second byte containing length  */
  if (header_length > 0 && GSDI_FCP_TEMPLATE_TAG == header_content_ptr[0])
  {
    if (header_length >= 2 && header_content_ptr[1] <= 0x7F)
    {
      /* One byte field for FCP length */
      offset = MMGSDI_TLV_TAG_LEN + MMGSDI_TLV_LEN_LEN;
    }
    else if(header_length >= 3 && header_content_ptr[1] == 0x81)
    {
      /* Two byte field for FCP length  */
      offset = MMGSDI_TLV_TAG_LEN + MMGSDI_TLV_LEN_LEN + 1;
    }
  }

  /* Look for PIN STATUS DO TAG starting after the FCP Template Tag
     and FCP Tag Length field*/
  if (offset > 0)
  {
    for(i = offset; i < (header_length-1); i++)
    {
      /*First Byte is the tag*/
      if(header_content_ptr[i] == int32touint8(GSDI_PIN_STATUS_DO_TAG))
      {
        break;
      }

      /*This is not a PIN Status tag, skip the entire TLV*/
      /*no of bytes of this TLV + Length field itself*/
      i += header_content_ptr[i+1] + MMGSDI_TLV_LEN_LEN;
    }
  }

  if(0 == offset || i >= (header_length-1))
  {
    UIM_MSG_ERR_3("Not FCP Template or no PIN STATUS TAG: setting PIN status to default values, offset=0x%x, i=0x%x, header_length=0x%x",
                  offset, i, header_length);
    mmgsdi_uicc_rsp_set_default_fcp(app_info_ptr,
                                    app_pin1_tbl_pptr,
                                    app_pin1_tbl_size,
                                    orig_req_header_ptr->slot_id);
    return MMGSDI_SUCCESS;
  }

  /* Locate the first mandatory tag parameter */
  if (i >= (header_length - 3) ||
      header_content_ptr[i+2] != int32touint8(GSDI_PS_DO_TAG))
  {
    UIM_MSG_ERR_0("No PS_DO_TAG");
    return MMGSDI_ERROR;
  }

  ps_do_byte_field_lgth = header_content_ptr[i+3];
  if (ps_do_byte_field_lgth != 0 && i >= (header_length - 4))
  {
    UIM_MSG_ERR_3("Incorrect ps_do_byte_field_lgth=0x%x, i=0x%x, header_length=0x%x",
                  ps_do_byte_field_lgth, i, header_length);
    return MMGSDI_ERROR;
  }

  /* Set offset for PS_DO */
  ps_do_byte_field_value_offset = int32touint8(i+4);

  /* Check only # of bytes specified in PIN Status Template DO TLV or header_length,
     whichever is less */
  if (i < header_length - header_content_ptr[i+1] - 2)
  {
    header_length = i + header_content_ptr[i+1] + 2;
  }

  /* Move to next tag byte after PS_DO: Len, PS_DO Tag, Len, PS_DO, Tag */
  i = ps_do_byte_field_value_offset + ps_do_byte_field_lgth;

  while (ps_do_byte_field_lgth > 0)
  {
    num_of_key_params = 0x80;
    ps_do_byte_field  = header_content_ptr[ps_do_byte_field_value_offset];

    while (i < header_length && num_of_key_params != 0)
    {
      /* PS_DO Format:  [] -> optional
         PS_DO_Tag  L  PS_DO   [USAGE_QUAL_TAG   L  USAGE_QUAL]  KEY_REF_TAG   1  KEY_REF
           0x90     w    x        0x95           1      y           0x83       1    z
           x -> Most significant bit maps to the corresponding KEY_REG TLV
           y -> 0x08 (Need user verification)
           z -> Key reference (102.221 Section 9.5 */

      if (header_content_ptr[i] == GSDI_SC_DO_USAGE_QUAL_TAG)
      {
        /* Increment to the length byte */
        if (++i >= header_length)
        {
          break;
        }
        usage_qualifier_lgth = header_content_ptr[i];

        /* skip the length field */
        if (++i >= header_length)
        {
          break;
        }

        if (usage_qualifier_lgth == GSDI_PS_DO_USAGE_QUAL_LEN)
        {
          usage_qualifier = header_content_ptr[i];
          /* skip over the usage qualifier byte */
          i++;
        }
        else
        {
          /* skip the number of bytes */
          i += usage_qualifier_lgth;
        }
      } /* usage_qual_tag */
      else if (header_content_ptr[i] == GSDI_KEY_REF_TAG)
      {
        /* Move to PIN */
        i+=2;
        if (i >= header_length)
        {
          break;
        }

        pin_index = 0;
        switch (header_content_ptr[i])
        {
          /*------------------------------------------------------------------------
            The Get Response could be 1 of the follwoing cases -
            a. Response data with PINs related to this app and other app pins as well
               -> In this case we will enforce the app to use only pins relevant to this app

            b. Response data with PINs totally unrelated to this app
               -> In this case we will select the last pin info provided

            c. Reponse Data wih only 1 PIN but not related to this app
               -> Allow this PIN to be chosen for this app

            d. Reponse Data wih only 1 PIN related to this app
               -> Happy Case!

           ------------------------------------------------------------------------*/
          case MMGSDI_PIN_APPL_1:
          case MMGSDI_PIN_APPL_2:
          case MMGSDI_PIN_APPL_3:
          case MMGSDI_PIN_APPL_4:
          case MMGSDI_PIN_APPL_5:
          case MMGSDI_PIN_APPL_6:
          case MMGSDI_PIN_APPL_7:
          case MMGSDI_PIN_APPL_8:
            mmgsdi_status = mmgsdi_util_find_app_pin_index((const mmgsdi_app_pin_info_type **)app_pin1_tbl_pptr,
                                                           app_pin1_tbl_size,
                                                           (mmgsdi_key_ref_values_enum_type)header_content_ptr[i],
                                                           &pin_index);
            if(mmgsdi_status != MMGSDI_SUCCESS)
            {
               UIM_MSG_ERR_1("Could not find PIN in this table - 0x%x",
                             header_content_ptr[i]);
               break;
            }

            if(aid_index == pin_index)/*Case a or Case d*/
            {
              app_pin1_match = TRUE;
              app_info_ptr->pin1_ptr = app_pin1_tbl_pptr[pin_index];
            }
            else /*Case a, Case b, Case c*/
            {
              if(!app_pin1_match)
              {
                UIM_MSG_HIGH_2("This PIN 0x%x is not applicable to this app 0x%x",
                               header_content_ptr[i], aid_index);

                app_info_ptr->pin1_ptr = app_pin1_tbl_pptr[pin_index];
              }
            }/*Case a will eventually be captured in the if clause*/

            if(app_info_ptr->pin1_ptr == NULL)
            {
              /*This should never happen*/
              UIM_MSG_ERR_0("PIN Ptr is still null, defaulting");
              app_info_ptr->pin1_ptr = app_pin1_tbl_pptr[pin_index];
            }

            mmgsdi_uicc_rsp_set_pin_status(app_info_ptr,
                                           MMGSDI_PIN1,
                                           num_of_key_params,
                                           ps_do_byte_field,
                                           usage_qualifier
                                          );

            num_of_key_params = (num_of_key_params >> 1);
            break;

          case MMGSDI_UNIVERSAL_PIN_KEY:
            app_info_ptr->universal_pin_ptr = app_univ_pin_tbl_ptr;
            mmgsdi_uicc_rsp_set_pin_status(app_info_ptr,
                                           MMGSDI_UNIVERSAL_PIN,
                                           num_of_key_params,
                                           ps_do_byte_field,
                                           usage_qualifier
                                          );

            num_of_key_params = (num_of_key_params >> 1);
            break;

          case MMGSDI_SEC_PIN_APPL_1:
          case MMGSDI_SEC_PIN_APPL_2:
          case MMGSDI_SEC_PIN_APPL_3:
          case MMGSDI_SEC_PIN_APPL_4:
          case MMGSDI_SEC_PIN_APPL_5:
          case MMGSDI_SEC_PIN_APPL_6:
          case MMGSDI_SEC_PIN_APPL_7:
          case MMGSDI_SEC_PIN_APPL_8:
            /*Case a or Case d*/
            if(orig_pin2_key_ref ==
                (mmgsdi_key_ref_values_enum_type)header_content_ptr[i])
            {
              app_pin2_match = TRUE;
              app_info_ptr->pin2.key_ref =
                  (mmgsdi_key_ref_values_enum_type)header_content_ptr[i];
            }
            else /*Case a, Case b, Case c*/
            {
              if(!app_pin2_match)/*Case b or Case c*/
              {
                UIM_MSG_HIGH_2("This PIN 0x%x is not applicable to this app 0x%x",
                               header_content_ptr[i], aid_index);

                app_info_ptr->pin2.key_ref =
                  (mmgsdi_key_ref_values_enum_type)header_content_ptr[i];
              }
            }/*Case a will eventually be captured in the if clause*/

            mmgsdi_uicc_rsp_set_pin_status(app_info_ptr,
                                           MMGSDI_PIN2,
                                           num_of_key_params,
                                           ps_do_byte_field,
                                           usage_qualifier
                                          );

            num_of_key_params = (num_of_key_params >> 1);
            break;

          default:
            UIM_MSG_HIGH_0("unknown pin");
            num_of_key_params = (num_of_key_params >> 1);
            break;
        } /* switch PIN */

        /* Once encounter 1 Key Reference, the usage qualifier is no longer applicable
           to the next key reference */
        usage_qualifier_lgth = 0;
        usage_qualifier      = 0;

        /* Move to next tag */
        i++;
      } /* key_ref_tag*/
      else
      {
        /* Data is corrupted, stop processing PS_DO TLV */
        return MMGSDI_ERROR;
      }
    } /* while loop on i => searching for key reference 0x83 tag */

    /* complete mapping of 1 byte of ps do value */
    ps_do_byte_field_lgth--;
    /* move to the "next" ps do value byte, i.e., 0x90, L, v1, v2, v3, */
    ps_do_byte_field_value_offset++;
  } /* while loop */

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_decode_pin_header_data */ /*lint !e715 */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_FILE_HEADER

   DESCRIPTION:
     This function decodes the header information for MF, DF and EF

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC
     Currently, this function only supports decoding for EF

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:
     None.

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
     to app info ptr updated in mmgsdi_uicc_rsp_decode_pin_header_data.
-------------------------------------------------------------------------------*/
void mmgsdi_uicc_rsp_decode_file_header (
  const mmgsdi_request_header_type           * orig_req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type * orig_extra_info_ptr,
  const uint8                                * header_content_ptr,
  int32                                        header_length,
  mmgsdi_file_attributes_type                 *gsdi_format_attr_ptr
)
{
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;

  if( NULL == gsdi_format_attr_ptr)
  {
    return;
  }
 (void) memset( gsdi_format_attr_ptr, 0x00, sizeof(mmgsdi_file_attributes_type) );

 /* Parse for PIN */
  if(orig_extra_info_ptr->access.access_method == MMGSDI_BY_APP_ID_ACCESS &&
     orig_req_header_ptr->orig_request_type == MMGSDI_GET_ALL_PIN_STATUS_REQ)
  {
    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
    mmgsdi_status = mmgsdi_uicc_rsp_decode_pin_header_data(orig_req_header_ptr,
                                                           orig_extra_info_ptr,
                                                           header_content_ptr,
                                                           header_length);
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_DEBUG_MSG_ERROR_1("Parse PIN Failed for UICC  0x%x",
                                   mmgsdi_status);
    }
    return;
  }

  /* Only Support EF Header parsing Now */
  mmgsdi_status = mmgsdi_uicc_rsp_decode_EF_header_data(orig_req_header_ptr,
                                                        orig_extra_info_ptr,
                                                        header_content_ptr,
                                                        header_length,
                                                        gsdi_format_attr_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("UICC Get File Attr Extract Fail 0x%x",
                                 mmgsdi_status);
  }
} /* mmgsdi_uicc_rsp_decode_file_header */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_GET_FILE_ATTR

   DESCRIPTION:
     This function will build the response to the get file attributes request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_get_file_attr(
  const mmgsdi_uim_report_rsp_type  * uim_rsp_ptr,
  mmgsdi_get_file_attr_cnf_type    ** attr_cnf_pptr,
  boolean                             synch
)
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_cnf_type       * cnf_ptr              = NULL;
  int32                                 index                = 0;
  uim_cmd_type                        * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr      = NULL;
  mmgsdi_return_enum_type               report_status        = MMGSDI_SUCCESS;
  boolean                               is_multi_profile_aid = FALSE;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(attr_cnf_pptr);
  }

   /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);

  UIM_MSG_HIGH_2("Select status 0x%x, cmd_transacted", report_status,
                 uim_rsp_ptr->cmd_transacted);

#ifdef FEATURE_UIM_MULTI_PROFILE
  /* Determine whether the request for select is for multi profile AID */
  if((mmgsdi_profile_info_ptr != NULL) &&
     (extra_param_ptr->get_file_attr_data.access.access_method == MMGSDI_BY_APP_ID_ACCESS) &&
     (mmgsdi_profile_info_ptr->aid.data_len > 0) &&
     (extra_param_ptr->get_file_attr_data.access.file.app_id.data_len == mmgsdi_profile_info_ptr->aid.data_len) &&
     (memcmp(extra_param_ptr->get_file_attr_data.access.file.app_id.data_ptr,
            mmgsdi_profile_info_ptr->aid.data_ptr,
            mmgsdi_profile_info_ptr->aid.data_len) == 0))
  {
    is_multi_profile_aid = TRUE;
  }
#endif /* FEATURE_UIM_MULTI_PROFILE */

    /* If cmd_transacted is TRUE and status is WRONG_CLASS, then it means card
     wants MMGSDI to switch the protocol. In that case send a RESET command
     to card. In multi profile AID case, do not switch the protocol since the
     purpose of the request is to determine whether the card is single or multi
     profile card */
  if(report_status == MMGSDI_WRONG_CLASS &&
     uim_rsp_ptr->cmd_transacted &&
     is_multi_profile_aid == FALSE)
    {
      mmgsdi_util_preinit_mmgsdi(req_header.session_id,
                                 MMGSDI_APP_NONE,
                                 MMGSDI_REFRESH_RESET,
                                 req_header.slot_id,
                                 FALSE);

      mmgsdi_status = mmgsdi_gen_build_card_reset(
                      mmgsdi_generic_data_ptr->client_id,
                      req_header.slot_id,
                      MMGSDI_CARD_RESET_REQ,
                      UIM_GSM,
                      MMGSDI_RESET_CAUSE_GENERAL,
                      mmgsdi_util_internal_cb,
                      0);
    }
    else if (report_status == MMGSDI_UIM_CMD_TIMEOUT &&
      req_header.orig_request_type == MMGSDI_GET_FILE_ATTR_REQ)
    {
      /* Update cache to indicate that file was not found on card */
      (void)mmgsdi_cache_write(req_header.session_id,
                               &extra_param_ptr->get_file_attr_data.access,
                               uim_rsp_ptr->rsp_data,
                               0,
                               MMGSDI_DATA_NOT_FOUND);

      UIM_MSG_HIGH_0("Marked cache to MMGSDI_CACHE_NOT_FOUND due to MMGSDI_UIM_CMD_TIMEOUT");
    }

  if (!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                               &req_header,
                                               extra_param_ptr,
                                               (void*)&uim_rsp_ptr->rsp_data,
                                               uim_rsp_ptr->cmd_transacted,
                                               uim_rsp_ptr->status_word);
  }
  else
  {
    if (attr_cnf_pptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*attr_cnf_pptr),
      sizeof(mmgsdi_get_file_attr_cnf_type));
    if ((*attr_cnf_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    cnf_ptr = *attr_cnf_pptr;

    mmgsdi_status = mmgsdi_cnf_build_get_file_attr_data(
      cnf_ptr,
      report_status, extra_param_ptr, &req_header, &uim_rsp_ptr->rsp_data);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Populate Header */
      /* Header population should be done AFTER data populate
        because any pointer length will be assigned in the
        individual confirmation population and used in the
        populate_cnf_rsp_header for assigning the response
        length */
      mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                      &cnf_ptr->response_header,
                                      &req_header,
                                      MMGSDI_GET_FILE_ATTR_CNF,
                                      uim_rsp_ptr->cmd_transacted,
                                      uim_rsp_ptr->status_word);

      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
  }

  UIM_MSG_HIGH_2("UICC RSP - GET FILE ATTR status is 0x%x for file 0x%x",
                 report_status,
                 extra_param_ptr->get_file_attr_data.access.file.file_enum);

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_get_file_attr */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_WRITE

   DESCRIPTION:
     This function will build the response to Write request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_write (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_write_cnf_type                ** mmgsdi_cnf_pptr,
  boolean                                 synch
)
{

  mmgsdi_request_header_type             req_header;
  int32                                  index                = 0;
  mmgsdi_return_enum_type                mmgsdi_status        = MMGSDI_SUCCESS;
  uim_cmd_type                         * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type    * extra_param_ptr      = NULL;
  mmgsdi_return_enum_type                report_status        = MMGSDI_SUCCESS;
  mmgsdi_write_cnf_type                * cnf_ptr              = NULL;
  mmgsdi_data_type                       write_data_buf;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_cnf_pptr);
  }

  memset(&write_data_buf, 0x00, sizeof(mmgsdi_data_type));

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (extra_param_ptr == NULL ||
      extra_param_ptr->write_data.data.data_ptr == NULL ||
      extra_param_ptr->write_data.data.data_len <= 0)
  {
    UIM_MSG_ERR_0("No extra write response data");
    return MMGSDI_ERROR;
  }

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
         write to cache if item is in the cache
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);

  /* copy rsp_data from uim_rsp_ptr so it can be modified by
     TMSI PLMN check during util write cache operation*/
  write_data_buf.data_len = extra_param_ptr->write_data.data.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(write_data_buf.data_ptr,
                                     write_data_buf.data_len);

  /* In case of failed memory allocation, clear cache to avoid stale data */
  if (write_data_buf.data_ptr == NULL)
  {
    if (extra_param_ptr->write_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      (void)mmgsdi_cache_delete_item(req_header.session_id,
                                     1,
                                     &extra_param_ptr->write_data.access.file.file_enum);
    }
    else
    {
      UIM_MSG_HIGH_1("failed to delete cache item with access method: %x",
                     extra_param_ptr->increase_data.access.access_method);
    }
  }
  else
  {
    mmgsdi_memscpy(write_data_buf.data_ptr,
                   int32touint32(write_data_buf.data_len),
                   extra_param_ptr->write_data.data.data_ptr,
                   int32touint32(write_data_buf.data_len));

    if (report_status == MMGSDI_SUCCESS)
    {
      /* Write to cache. If it is record based file,
         update only if it is 1st rec */
      if(!((extra_param_ptr->write_data.file_type == MMGSDI_LINEAR_FIXED_FILE) &&
           (extra_param_ptr->write_data.rec_num != 1)))
      {
        mmgsdi_data_from_enum_type data_from = MMGSDI_DATA_FROM_CARD_INCOMPLETE;

        if(extra_param_ptr->read_data.data_len == 0)
        {
          data_from = MMGSDI_DATA_FROM_CARD_COMPLETE;
        }

        (void)mmgsdi_cache_write(
                          req_header.session_id,
                          &extra_param_ptr->write_data.access,
                          write_data_buf,
                          extra_param_ptr->write_data.offset,
                          data_from);
      }
      mmgsdi_status = MMGSDI_SUCCESS;
    }
    else
    {
      /* unsuccessful processing of command */
      if (extra_param_ptr->write_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
      {
        UIM_MSG_HIGH_1("UIM_FAIL For MMGSDI EF %x",
                       extra_param_ptr->write_data.access.file.file_enum);

        if(report_status == MMGSDI_NOT_FOUND)
        {
          if(!((extra_param_ptr->write_data.file_type == MMGSDI_LINEAR_FIXED_FILE) &&
               (extra_param_ptr->write_data.rec_num != 1)))
          {
            /* Update cache to indicate that file was not found on card */
            (void)mmgsdi_cache_write(
                    req_header.session_id,
                    &extra_param_ptr->write_data.access,
                    write_data_buf,
                    extra_param_ptr->write_data.offset,
                    MMGSDI_DATA_NOT_FOUND);
          }
        }
      }
      UIM_MSG_HIGH_3("UIM_FAIL sw1 %x sw2 %x report status 0x%x",
                     uim_rsp_ptr->status_word.sw1,
                     uim_rsp_ptr->status_word.sw2,
                     report_status);
    } /* Parsing UIM Status */

    MMGSDIUTIL_TMC_MEM_FREE(write_data_buf.data_ptr);
  }

  if(!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                               &req_header,
                                               extra_param_ptr,
                                               NULL,
                                               uim_rsp_ptr->cmd_transacted,
                                               /* status word required for write */
                                               uim_rsp_ptr->status_word);
  }
  else
  {
    if (mmgsdi_cnf_pptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*mmgsdi_cnf_pptr),
      sizeof(mmgsdi_write_cnf_type));
    if ((*mmgsdi_cnf_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    cnf_ptr = *mmgsdi_cnf_pptr;

    mmgsdi_status = mmgsdi_cnf_build_write_data(cnf_ptr,
                                                report_status,
                                                extra_param_ptr);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Populate Header */
      /* Header population should be done AFTER data populate because any pointer
            * length will be assigned in the individual confirmation population and used in the
            * populate_cnf_rsp_header for assigning the response   length
            */
      mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                          &cnf_ptr->response_header,
                                          &req_header,
                                          MMGSDI_WRITE_CNF,
                                          uim_rsp_ptr->cmd_transacted,
                                          /* Status word required for read */
                                          uim_rsp_ptr->status_word);

      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
  }

  UIM_MSG_HIGH_2("UICC RSP - WRITE   status is 0x%x for file 0x%x",
                 report_status, extra_param_ptr->write_data.access.file.file_enum);

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_write */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_READ

   DESCRIPTION:
     This function will build the response to Read request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_read should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_read (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_read_cnf_type              ** mmgsdi_cnf_pptr,
  boolean                              synch
)
{

  mmgsdi_request_header_type            req_header;
  int32                                 index                 = 0;
  mmgsdi_return_enum_type               mmgsdi_status         = MMGSDI_SUCCESS;
  mmgsdi_read_cnf_type                * cnf_ptr               = NULL;
  uim_cmd_type                        * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr       = NULL;
  mmgsdi_return_enum_type               report_status         = MMGSDI_SUCCESS;
  boolean                               sw_present            = FALSE;
  mmgsdi_data_type                      rsp_data_buf;
  mmgsdi_nv_context_type                efs_index             = MMGSDI_NV_CONTEXT_INVALID;
  mmgsdi_file_sfi_table_type            file_info;
  mmgsdi_data_type                      cnf_data_buf;
  mmgsdi_offset_type                    mmgsdi_offset         = 0;
  boolean                               is_cache_init_read    = FALSE;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_cnf_pptr);
  }

  memset(&file_info, 0x00, sizeof(file_info));
  memset(&rsp_data_buf, 0x00, sizeof(mmgsdi_data_type));
  memset(&cnf_data_buf, 0x00, sizeof(mmgsdi_data_type));

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(extra_param_ptr);

  mmgsdi_offset   = extra_param_ptr->read_data.offset;

  if (uim_rsp_ptr->rsp_data.data_len > 0 &&
      uim_rsp_ptr->rsp_data.data_ptr)
  {
    /* copy rsp_data from uim_rsp_ptr so it can be modified by
       TMSI PLMN check during util write cache operation*/
    rsp_data_buf.data_len = uim_rsp_ptr->rsp_data.data_len;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(rsp_data_buf.data_ptr,
                                       rsp_data_buf.data_len);

    if (rsp_data_buf.data_ptr != NULL)
    {
      mmgsdi_memscpy(rsp_data_buf.data_ptr,
                     int32touint32(rsp_data_buf.data_len),
                     uim_rsp_ptr->rsp_data.data_ptr,
                     int32touint32(rsp_data_buf.data_len));
    }
    else
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(rsp_data_buf.data_ptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status == MMGSDI_SUCCESS)
  {
    if(extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      if ( extra_param_ptr->read_data.access.file.file_enum == MMGSDI_NO_FILE_ENUM ||
           extra_param_ptr->read_data.access.file.file_enum == MMGSDI_MAX_FILE_ENUM ||
           extra_param_ptr->read_data.access.file.file_enum == MMGSDI_EF_PATH_START)
      {
        UIM_MSG_ERR_1("Can not convert to uim enum 0x%x",
                      extra_param_ptr->read_data.access.file.file_enum);
      }
      else
      {
        /*Check the FCN List in case additional files are being read during refresh*/
        mmgsdi_util_check_client_fcn_list(req_header.session_id,
                                          extra_param_ptr->read_data.access.file.file_enum);
      }
    }

    /* write to cache. If it is record based file,
       update only if it is 1st rec */
    /* handle the case of ECC caching in a special manner
       A fixed number of ECC records (5 records) are to be cached */
    if((extra_param_ptr->read_data.file_type == MMGSDI_LINEAR_FIXED_FILE)&&
       (extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)&&
       ((extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_OPL)||
        (extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_PNN)))
    {
      mmgsdi_status = mmgsdi_eons_opl_pnn_update_record(
                        req_header.session_id,
                        extra_param_ptr->read_data.access.file.file_enum,
                        rsp_data_buf,
                        (mmgsdi_rec_num_type)extra_param_ptr->read_data.rec_num,
                        MMGSDI_CACHE_MAX_ENUM);
    }
    /* Update the cache content if the EF is under Phonebook directory.
       Add record node to the file cache list  */
    else if(mmgsdi_util_is_ef_under_phonebook(&extra_param_ptr->read_data.access) &&
            !mmgsdi_util_is_ef_pbr(&extra_param_ptr->read_data.access))
    {
      int32     client_index = 0;

      mmgsdi_status = mmgsdi_util_get_client_id_index(req_header.client_id,
                                                      &client_index);

      /* Disable MMGSDI PB cache from client PBM to avoid unnecessary additional SEARCH commands
         performed by MMGSDI completely. Also restricting usage of MMGSDI PB cache only to external
         clients (Android phonebook).*/
      if(mmgsdi_status == MMGSDI_SUCCESS && !MMGSDIUTIL_IS_PBM_CLIENT(client_index))
      {
        /* Proceed with cache of other PB records.*/
        mmgsdi_status = mmgsdi_pb_cache_records(&extra_param_ptr->read_data.access,
                                                req_header.slot_id,
                                                extra_param_ptr->read_data.rec_num,
                                                req_header.session_id);
      }
    }
    else if ((extra_param_ptr->read_data.file_type == MMGSDI_LINEAR_FIXED_FILE) &&
             ((mmgsdi_util_get_efs_item_index(
                 req_header.session_id, &efs_index) == MMGSDI_SUCCESS) &&
              (mmgsdi_nv_get_external_feature_status(
                 MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG, efs_index) == MMGSDI_FEATURE_ENABLED)) &&
        (extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_ACSGL ||
         extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_OCSGL ||
         extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_CSGT  ||
         extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_OCSGT ||
         extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_HNBN  ||
         extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_OHNBN))
    {
      (void)mmgsdi_cache_add_record(req_header.session_id,
                              extra_param_ptr->read_data.access,
                              extra_param_ptr->read_data.rec_num,
                              &rsp_data_buf);
    }
    else if( (extra_param_ptr->read_data.file_type == MMGSDI_TRANSPARENT_FILE) ||
             ((extra_param_ptr->read_data.file_type == MMGSDI_LINEAR_FIXED_FILE ||
               extra_param_ptr->read_data.file_type == MMGSDI_CYCLIC_FILE) &&
               extra_param_ptr->read_data.rec_num == 1) ||
             (extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_ECC))
    {
      mmgsdi_data_from_enum_type data_from = MMGSDI_DATA_FROM_CARD_INCOMPLETE;

      if(extra_param_ptr->read_data.data_len == 0)
      {
        data_from = MMGSDI_DATA_FROM_CARD_COMPLETE;
      }
      /* In case of a PRL file, if the length is greater than 2, it means the
         response contains the PRL size bytes + data. Hence, mark it as
         DATA_FROM_CARD_COMPLETE so that it gets written to the cache. */
      else if (extra_param_ptr->read_data.access.file.file_enum == MMGSDI_CSIM_PRL ||
               extra_param_ptr->read_data.access.file.file_enum == MMGSDI_CSIM_EPRL)
      {
        if (extra_param_ptr->read_data.data_len > MMGSDI_SIZE_OF_PRL_SIZE)
        {
          data_from = MMGSDI_DATA_FROM_CARD_COMPLETE;
        }
      }
      /* MMGSDI sends complete read request for partial read request for files
         present in cache and if the cache is not initialized then the entry is
         present in the cache list. In this case, mark the data from card as complete.*/
      else if (extra_param_ptr->read_data.file_type == MMGSDI_TRANSPARENT_FILE &&
               extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
      {
        mmgsdi_len_type             cache_data_len = 0;
        mmgsdi_cache_init_enum_type cache_state    = MMGSDI_CACHE_MAX_ENUM;

        if(MMGSDI_NOT_INIT == mmgsdi_cache_read_len(req_header.session_id,
                                                    &extra_param_ptr->read_data.access,
                                                    &cache_data_len,
                                                    0,
                                                    &cache_state))
        {
          data_from = MMGSDI_DATA_FROM_CARD_COMPLETE;

          /* When read request was processed in case of cache not_inited, 
             data_len is set to full file size and offset = 0 and extra_params
             has the partial read req data_len and offset. 
             therefore, update cache with proper offset if read request is for
             partial data and use this offset to write to cache instead of the
             offset data from extra_param */
          mmgsdi_offset = 0;
          is_cache_init_read = TRUE;
        }
      }

      (void)mmgsdi_cache_write(req_header.session_id,
                               &extra_param_ptr->read_data.access,
                               rsp_data_buf,
                               mmgsdi_offset,
                               data_from);
    }
  }
  else
  {
    /* unsuccessful processing of command */
    if (extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      if(report_status == MMGSDI_NOT_FOUND)
      {
        if((extra_param_ptr->read_data.file_type == MMGSDI_LINEAR_FIXED_FILE)&&
           (extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)&&
           ((extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_OPL)||
            (extra_param_ptr->read_data.access.file.file_enum == MMGSDI_USIM_PNN)))
        {
            mmgsdi_status = mmgsdi_eons_opl_pnn_update_record(
                              req_header.session_id,
                              extra_param_ptr->read_data.access.file.file_enum,
                              rsp_data_buf,
                              (mmgsdi_rec_num_type)extra_param_ptr->read_data.rec_num,
                              MMGSDI_CACHE_NOT_FOUND);
        }
        else if((extra_param_ptr->read_data.file_type == MMGSDI_TRANSPARENT_FILE) ||
                (((extra_param_ptr->read_data.file_type == MMGSDI_LINEAR_FIXED_FILE) ||
                  (extra_param_ptr->read_data.file_type == MMGSDI_CYCLIC_FILE)) &&
                 (extra_param_ptr->read_data.rec_num == 1)))
        {
          /* Update cache to indicate that file was not found on card */
          (void)mmgsdi_cache_write(req_header.session_id,
                                   &extra_param_ptr->read_data.access,
                                   rsp_data_buf,
                                   extra_param_ptr->read_data.offset,
                                   MMGSDI_DATA_NOT_FOUND);
        }
      }
    }
  } /* Parsing UIM Status */

  if (report_status == MMGSDI_SUCCESS)
  {
    /* if read from cache, and the operation is successful,
       the cmd_transacted will be FALSE.  if the operation is not successful,
       the command will go to uim */
    sw_present = TRUE;
  }
  else if ((report_status == MMGSDI_INCORRECT_LENGTH) &&
           !extra_param_ptr->read_data.select_before_read &&
           (extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS) &&
           (extra_param_ptr->read_data.file_type == MMGSDI_TRANSPARENT_FILE) &&
           (MMGSDI_SUCCESS == mmgsdi_file_get_file_info_from_table(
                                extra_param_ptr->read_data.access.file.file_enum,
                                &file_info)) &&
           (file_info.size != MMGSDI_INVALID_FILE_SIZE) &&
           mmgsdi_file_is_sfi_supported(req_header.slot_id))
  {
    /* MMGSDI has static tables that contain the spec-defined SFIs and
       spec-defined file lengths. If a file has an entry in that table,
       MMGSDI uses the file legnth (is client passed in zero as read
       data len) and SFI from those static tables to send a read request
       to the uimdrv. There are some non-spec compliant cards that have
       file length different than what is mentioned in the spec. For those
       files, card may return 67 00 (incorrect length). If that is the case,
       perform an explicit SELECT on the file and gets its length and retry
       the read with that length. In order to make sure the reattempt is
       performed only once (to avoid an infinite loop) we need to check
       extra_param_ptr->read_data.select_before_read before reattempting
       the read request. */
    mmgsdi_read_req_type               *read_req_ptr                = NULL;

    /* Retry the READ request with explicit SELECT. This READ will be sync
       or async depending on whether the original READ request was sync or
       async */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr, sizeof(mmgsdi_read_req_type));
    if (read_req_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(rsp_data_buf.data_ptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    /* Since command is being retried with SELECT, mark the following
       as TRUE so that MMGSDI can ensure that upon receiving
       MMGSDI_INCORRECT_LENGTH, MMGSDI does not reattempt more than
       once.. we dont want to get into a situation where if someone
       fakes the card response as 0x67 0x00 everytime for the read
       request, resulting in infinite queueing and processing of
       READ requests */
    read_req_ptr->select_before_read               = TRUE;
    read_req_ptr->request_header.client_id         = req_header.client_id;
    read_req_ptr->request_header.session_id        = req_header.session_id;
    read_req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
    read_req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
    read_req_ptr->request_header.request_len       =
      uint32toint32(sizeof(mmgsdi_read_req_type));
    read_req_ptr->request_header.payload_len       =
      read_req_ptr->request_header.request_len -
      uint32toint32(sizeof(mmgsdi_read_req_type));
    read_req_ptr->request_header.slot_id           = req_header.slot_id;
    read_req_ptr->request_header.client_data       = req_header.client_data;
    read_req_ptr->request_header.response_cb       = req_header.response_cb;
    mmgsdi_memscpy(&read_req_ptr->access, sizeof(mmgsdi_access_type),
                   &extra_param_ptr->read_data.access, sizeof(mmgsdi_access_type));
    read_req_ptr->rec_num                          = extra_param_ptr->read_data.rec_num;
    read_req_ptr->offset                           = extra_param_ptr->read_data.offset;
    read_req_ptr->file_type                        = extra_param_ptr->read_data.file_type;
    read_req_ptr->data_len                         = extra_param_ptr->read_data.data_len;

    mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr, synch, mmgsdi_cnf_pptr);

    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);

    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* A read request has been attempted again, which will take care of
         populating and sending the cnf to the client. This current failed
         read request can return now without any futher action as it is
         no longer needed. The entry for this current failed read request
         will be removed from client req table ptr as well. */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(rsp_data_buf.data_ptr);
      return MMGSDI_SUCCESS;
    }
  }
  else
  {
    sw_present = uim_rsp_ptr->cmd_transacted;
  }

  cnf_data_buf.data_len = rsp_data_buf.data_len;
  cnf_data_buf.data_ptr = rsp_data_buf.data_ptr;

  /* update cnf_data_buf if MMGSDI sends complete read request
     for partial read request for files present in cache but cache is
     not initialized*/
  if (is_cache_init_read &&
      extra_param_ptr->read_data.data_len != 0 &&
      extra_param_ptr->read_data.data_len < rsp_data_buf.data_len)
  {
    cnf_data_buf.data_len = extra_param_ptr->read_data.data_len;

    if (extra_param_ptr->read_data.offset != 0 &&
        extra_param_ptr->read_data.offset < rsp_data_buf.data_len)
    {
      cnf_data_buf.data_ptr =
           &rsp_data_buf.data_ptr[extra_param_ptr->read_data.offset];
    }
  }

  if (!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status, &req_header, extra_param_ptr,
      (void*)&cnf_data_buf,
      sw_present,  /* status word required for read */
      uim_rsp_ptr->status_word);
  }
  else
  {
    if (mmgsdi_cnf_pptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE(rsp_data_buf.data_ptr);
      return MMGSDI_ERROR;
    }

    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*mmgsdi_cnf_pptr),
      sizeof(mmgsdi_read_cnf_type));
    if ((*mmgsdi_cnf_pptr) == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE(rsp_data_buf.data_ptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    cnf_ptr = *mmgsdi_cnf_pptr;

    mmgsdi_status = mmgsdi_cnf_build_read_data(
      cnf_ptr,
      report_status,
      extra_param_ptr,
      (mmgsdi_data_type*)&cnf_data_buf);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Populate Header */
      /* Header population should be done AFTER data populate
        because any pointer length will be assigned in the
        individual confirmation population and used in the
        populate_cnf_rsp_header for assigning the response
        length */
      mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                          &cnf_ptr->response_header,
                                          &req_header,
                                          MMGSDI_READ_CNF,
                                          sw_present,
                                          /* Status word required for read */
                                          uim_rsp_ptr->status_word);

      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(rsp_data_buf.data_ptr);
  return mmgsdi_status;

} /* mmgsdi_uicc_rsp_read */


/*----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_SEND_APDU

   DESCRIPTION:
     This function will build the response to SEND_APDU request

   DEPENDENCIES:

   LIMITATIONS:
     For ICC protocol access, mmgsdi_uicc_rsp_send_apdu should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_send_apdu(
  const mmgsdi_uim_report_rsp_type* uim_rsp_ptr)
{
  mmgsdi_request_header_type         req_header;
  int32                              index            = 0;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type*                      uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_0("MMGSDI_UICC_RSP_SEND_APDU");
  /*
   * Check input pointers.
   */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(
    index, 0, MMGSDI_MAX_CLIENT_INFO-1);
  /*
   * Get the request header.
   */
  mmgsdi_status =
    mmgsdi_util_get_client_request_table_info(
      uim_rsp_ptr->rsp_data_index,
      &req_header,
      &uim_cmd_ptr,
      &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* sw not required
                                                       for send apdu */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_send_apdu*/ /*lint !e715 */


/* =============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_UPDATE_APP_STATUS

   DESCRIPTION:
     This function updates the app status if it is initialized

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
static void mmgsdi_uicc_rsp_update_app_status(
  mmgsdi_session_id_type              session_id,
  mmgsdi_status_me_app_enum_type      app_status
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type * app_info_ptr     = NULL;

  /* ignore the other status types as app info
     might not be available */
  if(app_status != MMGSDI_STATUS_APP_INITIALIZED)
  {
    return;
  }

  UIM_MSG_MED_0("mmgsdi_uicc_rsp_update_app_status");

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_id,
                        NULL,
                        NULL,
                        &app_info_ptr,
                        NULL,
                        NULL,
                        NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Unable to get session app info");
    return;
  }

  /* set the app initialized status to TRUE for any UICC
     app with a valid aid */
  if ((app_info_ptr->app_data.aid.data_len > 0) &&
      (app_info_ptr->app_data.aid.data_len <= UIM_AID_MAX_NAME_LENGTH))
  {
    app_info_ptr->status_app_is_initialized = TRUE;
  }
} /* mmgsdi_uicc_rsp_update_app_status */


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_STATUS

   DESCRIPTION:
     This function will build the response to the Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol status, mmgsdi_icc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_status_cnf_type               ** mmgsdi_cnf_pptr,
  boolean                                 synch
)
{

  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  uint8                               slot_index            = MMGSDI_SLOT_1_INDEX;
  mmgsdi_status_cnf_type            * cnf_ptr               = NULL;

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* ------------------------------------------------------------------------
       Command processing returned Status words other than 0x90 0x00
       ----------------------------------------------------------------------*/
   MMGSDIUTIL_DEBUG_MSG_ERROR_3("UIM_FAIL sw1:%x sw2:%x status:0x%x",
                                uim_rsp_ptr->status_word.sw1,
                                uim_rsp_ptr->status_word.sw2,
                                report_status);
  } /* Parsing UIM Status */

  if (synch)
  {
    if (mmgsdi_cnf_pptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*mmgsdi_cnf_pptr),
      sizeof(mmgsdi_status_cnf_type));
    if ((*mmgsdi_cnf_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    cnf_ptr = *mmgsdi_cnf_pptr;

    mmgsdi_status = mmgsdi_cnf_build_status_data(cnf_ptr,
                                                 report_status,
                                                 (void*)&uim_rsp_ptr->rsp_data);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                          &cnf_ptr->response_header,
                                          &req_header,
                                          MMGSDI_CARD_STATUS_CNF,
                                          uim_rsp_ptr->cmd_transacted,
                                          uim_rsp_ptr->status_word);

      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
    else
    {
      UIM_MSG_ERR_1("Failed to build STATUS data: status 0x%x", mmgsdi_status);
      cnf_ptr->response_header.mmgsdi_status = mmgsdi_status;
    }

    return cnf_ptr->response_header.mmgsdi_status;
  }

  /* Need to build a deactivate command if status
     command indicated app termination */
  if ( (req_header.orig_request_type == MMGSDI_SESSION_CLOSE_REQ) ||
       (req_header.orig_request_type == MMGSDI_SESSION_DEACTIVATE_REQ) ||
       (req_header.orig_request_type == MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ))
  {
    /* Build deactivate command message */
    mmgsdi_status = mmgsdi_session_build_deactivate( req_header.client_id,
                                                     req_header.session_id,
                                                     req_header.slot_id,
                                                     req_header.orig_request_type,
                                                     req_header.response_cb,
                                                     req_header.client_data);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
       return mmgsdi_status;
    }

    /* If not able to successfully build the close req, send the confirmation */
    report_status = MMGSDI_ERROR;
  } /* orig_request_type == MMGSDI_SESSION_CLOSE_REQ ||
                           MMGSDI_SESSION_DEACTIVATE_REQ */
  else if ( req_header.orig_request_type == MMGSDI_CARD_RESET_REQ)
  {
    /* Proceed with building and queuing a Reset request only if the REFRESH
       is still in progress on the slot. A Card Error for example could have
       stopped the REFRESH, and in this case, do not queue the Reset request */
    mmgsdi_status = mmgsdi_util_get_slot_index(req_header.slot_id,
                                               &slot_index);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
    if((slot_index != MMGSDI_SLOT_1_INDEX) &&
       (slot_index != MMGSDI_SLOT_2_INDEX) &&
       (slot_index != MMGSDI_SLOT_3_INDEX))
    {
      return MMGSDI_ERROR;
    }
    if((mmgsdi_refresh_info_ptr[slot_index] != NULL) &&
       (mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id ==
        req_header.slot_id))
    {
      /* Build Reset command message */
      mmgsdi_status = mmgsdi_gen_build_card_reset(req_header.client_id,
                                                  req_header.slot_id,
                                                  req_header.orig_request_type,
                                                  UIM_UICC,
                                                  MMGSDI_RESET_CAUSE_REFRESH,
                                                  req_header.response_cb,
                                                  req_header.client_data);

      /* Reset Session and application data for the slot */
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        UIM_MSG_HIGH_0("Reset Session and application data");
        mmgsdi_util_preinit_mmgsdi(0,
                                   MMGSDI_APP_NONE,
                                   mmgsdi_refresh_info_ptr[slot_index]->mode,
                                   mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                                   FALSE);

        mmgsdi_refresh_update_card_state(
          mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id);
      }
    }
    else
    {
      UIM_MSG_ERR_0("Cannot proceed with Card Reset, REFRESH no longer in progress");

      /* Return Success status without queuing a confirmation since Status command
         is complete and REFRESH is no longer in progress */
      mmgsdi_status = MMGSDI_SUCCESS;
    }
    return mmgsdi_status;
  } /* orig_request_type == MMGSDI_CARD_RESET_REQ */
   else if (req_header.orig_request_type == MMGSDI_CARD_STATUS_REQ)
  {
    if ((report_status == MMGSDI_SUCCESS) &&
        (extra_param_ptr != NULL))
    {
      mmgsdi_uicc_rsp_update_app_status(req_header.session_id,
                                        extra_param_ptr->app_status_data.app_status);
    }
  } /* orig_request_type == MMGSDI_CARD_STATUS_REQ */

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             uim_rsp_ptr->cmd_transacted,
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_status */


/*===========================================================================
FUNCTION MMMGSDI_UICC_SET_PARSE_PIN

DESCRIPTION
  This function sets the pin information based on uim status

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_uicc_rsp_parse_pin_num(
  mmgsdi_return_enum_type           report_status,
  const mmgsdi_pin_extra_info_type  *pin_extra_info_ptr,
  const mmgsdi_uim_report_rsp_type  *uim_rsp_ptr,
  mmgsdi_cmd_enum_type               orig_request_type,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_int_app_info_type          *sel_app_ptr
)
{
  mmgsdi_return_enum_type      mmgsdi_status        = report_status;
  uint32                       num_retries          = 0;
  uint32                       num_unblock_retries  = 0;
  mmgsdi_pin_status_enum_type  pin_status           = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
  mmgsdi_app_pin_info_type    *pin_info_ptr         = NULL;

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

  if (pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN1)
  {
    pin_info_ptr   = sel_app_ptr->pin1_ptr;
  }
  else if (pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN2)
  {
    pin_info_ptr   = &sel_app_ptr->pin2;
  }
  else if (pin_extra_info_ptr->pin_info.pin_id == MMGSDI_UNIVERSAL_PIN)
  {
    pin_info_ptr   = sel_app_ptr->universal_pin_ptr;
  }
  else
  {
    UIM_MSG_ERR_1("Incorrect PIN ID = %d", pin_extra_info_ptr->pin_info.pin_id);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(pin_info_ptr);

  pin_info_ptr->pin_id  = pin_extra_info_ptr->pin_info.pin_id;
  num_retries           = pin_info_ptr->num_retries;
  num_unblock_retries   = pin_info_ptr->num_unblock_retries;
  pin_status            = pin_info_ptr->status;

  switch(report_status)
  {
    case MMGSDI_SUCCESS:
      pin_info_ptr->valid_num_retries         = TRUE;
      pin_info_ptr->valid_num_unblock_retries = TRUE;
      break;

    case MMGSDI_INCORRECT_CODE:
      /*Some PIN Error has occured*/
      if(uim_rsp_ptr->status_word.sw2 & MMGSDI_MASK_FOR_LOW_BYTE)
      {
        if(pin_extra_info_ptr->retry_type == MMGSDI_GET_NUM_UNBLOCK_RETRIES ||
           (pin_extra_info_ptr->retry_type == MMGSDI_RETRY_NONE &&
            pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_UNBLOCK))
        {
          num_unblock_retries = (uim_rsp_ptr->status_word.sw2 & MMGSDI_MASK_FOR_LOW_BYTE);
          pin_info_ptr->valid_num_unblock_retries = TRUE;
        }
        else if (pin_extra_info_ptr->retry_type == MMGSDI_GET_NUM_RETRIES ||
                 pin_extra_info_ptr->retry_type == MMGSDI_RETRY_NONE)
        {
          num_retries = (uim_rsp_ptr->status_word.sw2 & MMGSDI_MASK_FOR_LOW_BYTE);
          pin_info_ptr->valid_num_retries = TRUE;
        }

        /*If this is the response to the request for Unblock Retries or
          Max Retries, then set the mmgsdi_status to success*/
        if( pin_extra_info_ptr->retry_type != MMGSDI_RETRY_NONE)
        {
          mmgsdi_status = MMGSDI_SUCCESS;
          /*If the PIN already exists, then change the valid FLAG TO TRUE*/
          if(pin_info_ptr->pin_data.data_len != 0)
          {
             pin_info_ptr->valid_pin = TRUE;
          }
        }
      }
      else /*Retries == 0*/
      {
        /*Verify,Enable,Disable,Change  all will return blocked*/
        if(pin_extra_info_ptr->retry_type == MMGSDI_RETRY_NONE)
        {
           pin_status      = MMGSDI_PIN_BLOCKED;
           num_retries     = 0;
           mmgsdi_status   = MMGSDI_CODE_BLOCKED;
           pin_info_ptr->valid_num_retries = TRUE;
        }
        else if(pin_extra_info_ptr->retry_type == MMGSDI_GET_NUM_UNBLOCK_RETRIES)
        {
           pin_status          = MMGSDI_PIN_PERM_BLOCKED;
           num_unblock_retries = 0;
           mmgsdi_status       = MMGSDI_CODE_PERM_BLOCKED;
           pin_info_ptr->valid_num_unblock_retries = TRUE;
        }
      }

      pin_info_ptr->num_retries         = num_retries;
      pin_info_ptr->num_unblock_retries = num_unblock_retries;
      pin_info_ptr->status              = pin_status;
      break;

    case MMGSDI_ACCESS_DENIED:
      pin_info_ptr->num_retries         = num_retries;
      pin_info_ptr->num_unblock_retries = num_unblock_retries;
      pin_info_ptr->status              = pin_status;
      pin_info_ptr->valid_num_retries         = TRUE;
      pin_info_ptr->valid_num_unblock_retries = TRUE;
      break;

    case MMGSDI_CODE_BLOCKED:
      pin_info_ptr->num_retries         = 0;
      pin_info_ptr->num_unblock_retries = num_unblock_retries;
      pin_info_ptr->status              = MMGSDI_PIN_BLOCKED;
      pin_info_ptr->valid_num_retries         = TRUE;
      pin_info_ptr->valid_num_unblock_retries = TRUE;
      break;

    case MMGSDI_CODE_PERM_BLOCKED:
      pin_info_ptr->num_retries         = 0;
      pin_info_ptr->num_unblock_retries = 0;
      pin_info_ptr->status              = MMGSDI_PIN_PERM_BLOCKED;
      pin_info_ptr->valid_num_retries         = TRUE;
      pin_info_ptr->valid_num_unblock_retries = TRUE;
      break;

    case MMGSDI_ERROR_NO_EVENT_NEEDED:
      pin_info_ptr->valid_num_retries         = TRUE;
      pin_info_ptr->valid_num_unblock_retries = TRUE;
      if (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_MMGSDI_UNBLOCK_PIN_HANDLE_INVALID_SW, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
          MMGSDI_FEATURE_ENABLED)
      {
        /* Ignore error if the request was for retrieving number of unblock
           retries as part of PIN verification */
        if(pin_extra_info_ptr->retry_type == MMGSDI_GET_NUM_UNBLOCK_RETRIES &&
           pin_info_ptr->status != MMGSDI_PIN_BLOCKED &&
           pin_info_ptr->status != MMGSDI_PIN_PERM_BLOCKED)
        {
          pin_info_ptr->valid_num_unblock_retries = FALSE;
          mmgsdi_status = MMGSDI_SUCCESS;
        }
      }
      else
      {
        if (orig_request_type == MMGSDI_GET_ALL_PIN_STATUS_REQ)
        {
          pin_info_ptr->num_retries = 3;
          mmgsdi_status             = MMGSDI_SUCCESS;
        }
        pin_info_ptr->valid_num_retries         = FALSE;
        pin_info_ptr->valid_num_unblock_retries = FALSE;
      }
      break;

    default:
      if (orig_request_type == MMGSDI_GET_ALL_PIN_STATUS_REQ)
      {
        pin_info_ptr->num_retries = 3;
        mmgsdi_status             = MMGSDI_SUCCESS;
      }
      pin_info_ptr->valid_num_retries         = FALSE;
      pin_info_ptr->valid_num_unblock_retries = FALSE;
      break;
  } /* Parsing UIM Status */

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_parse_pin_num */


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_PIN_OPERATION

   DESCRIPTION:
     This function will build the response to the PIN Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol status, mmgsdi_icc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_pin_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_pin_enum_type                pin_id                = MMGSDI_PIN1;
  mmgsdi_int_app_info_type          * app_info_ptr          = NULL;
  mmgsdi_channel_info_type          * channel_info_ptr      = NULL;
  mmgsdi_sw_status_type               status_words          = {0};

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL )
  {
    return MMGSDI_ERROR;
  }
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                             req_header.session_id,
                             NULL,
                             &channel_info_ptr);
  if((mmgsdi_status != MMGSDI_SUCCESS)
       ||(channel_info_ptr == NULL)
       ||(channel_info_ptr->app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }
  app_info_ptr = channel_info_ptr->app_info_ptr;

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  status_words.sw1 = uim_rsp_ptr->status_word.sw1;
  status_words.sw2 = uim_rsp_ptr->status_word.sw2;

  if (report_status != MMGSDI_SUCCESS)
  {
    /* ------------------------------------------------------------------------
       Command processing returned Status words other than 0x90 0x00
       ----------------------------------------------------------------------*/
   MMGSDIUTIL_DEBUG_MSG_ERROR_3("UIM_FAIL sw1:%x sw2:%x status:0x%x",
                                 status_words.sw1,
                                 status_words.sw2,
                                report_status);

    report_status = mmgsdi_uicc_rsp_parse_pin_num(report_status,
                                                  &extra_param_ptr->pin_data,
                                                  uim_rsp_ptr,
                                                  req_header.orig_request_type,
                                                  req_header.slot_id,
                                                  app_info_ptr);

    /* After a successful Verify/Enable/Disable/Unblock, MMGSDI sends UNBLOCK
       and VERIFY requests to the card with empty PIN to retrieve the number
       of retries. If the UNBLOCK request returns error, we still send the
       VERIFY to get the number of retries. If the VERIFY request fails,
       we return success because the original verify PIN opertation was successful.
       Skip these operations in case of GET_ALL_PIN_STATUS when orig_pin_op
       is NONE because there is no originial status to be returned. */
    if (extra_param_ptr->pin_data.orig_pin_op != MMGSDI_PIN_OP_NONE)
    {
      if (extra_param_ptr->pin_data.retry_type == MMGSDI_GET_NUM_UNBLOCK_RETRIES)
      {
        mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                                  extra_param_ptr->pin_data.pin_info.pin_id,
                                                  extra_param_ptr->pin_data.orig_pin_id,
                                                  MMGSDI_PIN_OP_VERIFY,
                                                  extra_param_ptr->pin_data.orig_pin_op,
                                                  MMGSDI_UICC,
                                                  &(extra_param_ptr->pin_data),
                                                  &(extra_param_ptr->pin_data.orig_sw));
        return mmgsdi_status;
      }
      if(extra_param_ptr->pin_data.retry_type == MMGSDI_GET_NUM_RETRIES)
      {
        /* Set report_status to return SUCCESS in the confirmation below. */
        report_status    = MMGSDI_SUCCESS;
        status_words.sw1 = extra_param_ptr->pin_data.orig_sw.sw1;
        status_words.sw2 = extra_param_ptr->pin_data.orig_sw.sw2;
      }
    }
  }

  if(report_status == MMGSDI_SUCCESS)
  {
    if(extra_param_ptr->pin_data.retry_type  == MMGSDI_RETRY_NONE)
    {
      /*If the Attempt was successful, then FOR VERIFY & ENABLE OPERATION
        Set the PIN To ENABLED_VERIFIED*/
      if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_VERIFY ||
         extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_ENABLE ||
         extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_UNBLOCK ||
         extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_CHANGE)
      {
        if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN1
           && app_info_ptr->pin1_ptr != NULL)
        {
          app_info_ptr->pin1_ptr->status          = MMGSDI_PIN_ENABLED_VERIFIED;

          /*If Enable Attempt was successful, then the pin replacement is
            cancelled*/
          if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_ENABLE)
          {
            app_info_ptr->pin1_ptr->pin_replacement =  MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;

            /*Also Reflect the changes in UNIV PIN*/
            if(app_info_ptr->universal_pin_ptr)
            {
              app_info_ptr->universal_pin_ptr->pin_replacement = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;
            }
          }
        }
        else if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN2){
          app_info_ptr->pin2.status = MMGSDI_PIN_ENABLED_VERIFIED;
        }
        else if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_UNIVERSAL_PIN &&
                app_info_ptr->universal_pin_ptr != NULL)
        {
          app_info_ptr->universal_pin_ptr->status = MMGSDI_PIN_ENABLED_VERIFIED;
        }
      }
      /*If the Attempt was successful, then FOR DISABLE OPERATION
        Set the PIN To DISABLED*/
      if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_DISABLE)
      {
        if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN1
           && app_info_ptr->pin1_ptr != NULL)
        {
          app_info_ptr->pin1_ptr->status = MMGSDI_PIN_DISABLED;
        }
        else if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN2){
          app_info_ptr->pin2.status = MMGSDI_PIN_DISABLED;
        }
        else if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_UNIVERSAL_PIN &&
                app_info_ptr->universal_pin_ptr != NULL)
        {
          app_info_ptr->universal_pin_ptr->status = MMGSDI_PIN_DISABLED;
        }
      }

      /*If the Attempt was successful, then FOR DISABLE AND REPLACE OPERATION
        Set the PIN To DISABLED*/
      if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_DISABLE_AND_REPLACE)
      {
        if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN1
           && app_info_ptr->pin1_ptr != NULL)
        {
          app_info_ptr->pin1_ptr->status          = MMGSDI_PIN_DISABLED;
          app_info_ptr->pin1_ptr->pin_replacement = MMGSDI_PIN_REPLACED_BY_UNIVERSAL;
        }
      }
      /* Since the PIN operation is successful, store the PIN value */
      mmgsdi_util_store_pin(app_info_ptr,
                            extra_param_ptr->pin_data.pin_info.pin_id,
                            extra_param_ptr->pin_data.pin_data);
    }

    /*----------------------------------------------------------------------------------
     1)If this is the first response, then the current_pin_op will be NULL In this case
     send out a Unblock Request with copied PIN data.
     2)If this is the response to the 1) then the current pin_op will be  UNBLOCK, in that
     case send out the Verify PIN Request with copied PIN data
     3)If this is the response to the 2) then the current pin_op will be  VERIFY with empty,
     in that case send out the Verify PIN Request with entered PIN data
     4)If this is the response to the 3) then the current pin_op will be VERIFY with PIN data,
     send out the confirmation
     ----------------------------------------------------------------------------------*/
    if(extra_param_ptr->pin_data.retry_type == MMGSDI_RETRY_NONE)
    {
      mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                                extra_param_ptr->pin_data.pin_info.pin_id,
                                                extra_param_ptr->pin_data.orig_pin_id,
                                                MMGSDI_PIN_OP_UNBLOCK,
                                                extra_param_ptr->pin_data.orig_pin_op,
                                                MMGSDI_UICC,
                                                &(extra_param_ptr->pin_data),
                                                &(uim_rsp_ptr->status_word));
      return mmgsdi_status;
    }
    if(extra_param_ptr->pin_data.retry_type == MMGSDI_GET_NUM_UNBLOCK_RETRIES)
    {
      mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                                extra_param_ptr->pin_data.pin_info.pin_id,
                                                extra_param_ptr->pin_data.orig_pin_id,
                                                MMGSDI_PIN_OP_VERIFY,
                                                extra_param_ptr->pin_data.orig_pin_op,
                                                MMGSDI_UICC,
                                                &(extra_param_ptr->pin_data),
                                                &(extra_param_ptr->pin_data.orig_sw));
      return mmgsdi_status;
    }

    /* Some buggy cards require PIN1 to be verified after PIN1 value is changed in order to work correctly.
       PIN1 verify command with the cached PIN1 value entered by user is issued to satisfy the card */
    if((mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_VERIFY_PIN_AFTER_CHANGE, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED) &&
       (extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_CHANGE) &&
       (extra_param_ptr->pin_data.retry_type == MMGSDI_GET_NUM_RETRIES))
    {
      mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                                extra_param_ptr->pin_data.pin_info.pin_id,
                                                extra_param_ptr->pin_data.orig_pin_id,
                                                MMGSDI_PIN_OP_VERIFY,
                                                extra_param_ptr->pin_data.orig_pin_op,
                                                MMGSDI_UICC,
                                                &(extra_param_ptr->pin_data),
                                                NULL);
      return mmgsdi_status;
    }
  }

  if ( extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN1 &&
       app_info_ptr->pin1_ptr != NULL)
  {
    mmgsdi_memscpy(&extra_param_ptr->pin_data.pin_info,
                   sizeof(extra_param_ptr->pin_data.pin_info),
                   app_info_ptr->pin1_ptr,
                   sizeof(mmgsdi_app_pin_info_type));
  }
  else if ( extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN2 )
  {
    mmgsdi_memscpy(&extra_param_ptr->pin_data.pin_info,
                   sizeof(extra_param_ptr->pin_data.pin_info),
                   &app_info_ptr->pin2,
                   sizeof(mmgsdi_app_pin_info_type));
  }
  else if ( extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_UNIVERSAL_PIN &&
            app_info_ptr->universal_pin_ptr != NULL)
  {
    mmgsdi_memscpy(&extra_param_ptr->pin_data.pin_info,
                   sizeof(extra_param_ptr->pin_data.pin_info),
                   app_info_ptr->universal_pin_ptr,
                   sizeof(mmgsdi_app_pin_info_type));
  }

  /*If the original Operation is a Get all PIn Status, then requeue the request for PIN2
    and Universal PIN*/
  if(req_header.orig_request_type == MMGSDI_GET_ALL_PIN_STATUS_REQ)
  {
    if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN1){
      if(app_info_ptr->pin1_ptr != NULL)
      {
        app_info_ptr->pin1_ptr->cached = TRUE;
      }
      extra_param_ptr->pin_data.pin1_report_status = report_status;
      pin_id = MMGSDI_PIN2;
      mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                                 pin_id,
                                                 extra_param_ptr->pin_data.orig_pin_id,
                                                 MMGSDI_PIN_OP_UNBLOCK,
                                                 extra_param_ptr->pin_data.orig_pin_op,
                                                 MMGSDI_UICC,
                                                 NULL,
                                                 NULL);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        /* Continue PIN status for PIN2 and Universal PIN */
        return mmgsdi_status;
      }
      /* PIN2 is not there, continue PIN status for universal PIN */
      app_info_ptr->pin2.cached = TRUE;
      pin_id = MMGSDI_UNIVERSAL_PIN;
      mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                                 pin_id,
                                                 extra_param_ptr->pin_data.orig_pin_id,
                                                 MMGSDI_PIN_OP_UNBLOCK,
                                                 extra_param_ptr->pin_data.orig_pin_op,
                                                 MMGSDI_UICC,
                                                 NULL,
                                                 NULL);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        /* Continue PIN status for Universal PIN */
        return mmgsdi_status;
      }
      /* UPIN is not there, Get all pin status completed*/
    }
    else if (extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN2)
    {
      app_info_ptr->pin2.cached = TRUE;
      pin_id = MMGSDI_UNIVERSAL_PIN;
      mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                                 pin_id,
                                                 extra_param_ptr->pin_data.orig_pin_id,
                                                 MMGSDI_PIN_OP_UNBLOCK,
                                                 extra_param_ptr->pin_data.orig_pin_op,
                                                 MMGSDI_UICC,
                                                 NULL,
                                                 NULL);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        /*If Universal PIN status, can be found, then return now
          else set the cached flags to TRUE and build a successful
          response*/
        return mmgsdi_status;
      }
    }

    if(app_info_ptr->universal_pin_ptr)
    {
      app_info_ptr->universal_pin_ptr->cached = TRUE;
    }

    /*Send the Report status of only Pin1. Ignore the others*/
    report_status = extra_param_ptr->pin_data.pin1_report_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             uim_rsp_ptr->cmd_transacted,
                                             status_words);

  UIM_MSG_HIGH_3("UICC RSP - PIN OPERATION   status is 0x%x for PIN_OP 0x%x, PIN_STATUS = 0x%x",
                 mmgsdi_status,
                 extra_param_ptr->pin_data.orig_pin_op,
                 extra_param_ptr->pin_data.pin_info.status);

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_pin_operation*/


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_HIDDENKEY_OPERATION

   DESCRIPTION:
     This function will build the response to the Hiddenkey Command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_hiddenkey_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header          ={0};
  int32                               index               = 0;
  mmgsdi_return_enum_type             mmgsdi_status       = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr         = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr     = NULL;
  mmgsdi_return_enum_type             report_status       = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type          * app_info_ptr        = NULL;
  int32                               client_cmd_index    = 0;
  mmgsdi_client_req_extra_info_type * new_extra_param_ptr = NULL;
  mmgsdi_session_type_enum_type       session_type        = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_static_data_type             hkey_card;
  mmgsdi_write_data_type              write_data;
  mmgsdi_access_type                  file_access;

  UIM_MSG_HIGH_0("MMGSDI_UICC_RSP_HIDDENKEY_OPERATION");

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Get the app related to the session */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                    req_header.session_id,
                    NULL,
                    NULL,
                    &app_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Could not get Prov app info");
    return MMGSDI_ERROR;
  }

  /* Obtain the Stats of the operation */
  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status == MMGSDI_SUCCESS)
  {
    /* If the requested is Hiddenkey operation is a verify */
    if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_VERIFY)
    {
      app_info_ptr->hiddenkey.status = MMGSDI_PIN_ENABLED_VERIFIED;
    }
    else
    {
      /* If the requested Hiddenkey operation is disable, create
         the Hiddenkey data to be written to the card */
      if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_DISABLE)
      {
        hkey_card.data_len = MMGSDI_HIDDENKEY_LEN;
        memset((void*)hkey_card.data_ptr, 0xFF, int32touint32(hkey_card.data_len));
      }
      /* If the requested Hiddenkey operation is an Enable or Change, convert
         the Hiddenekey data from the client to the Card format */
      else if((extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_ENABLE) ||
              (extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_CHANGE))
      {
        mmgsdi_status = mmgsdi_util_convert_hiddenkey(&extra_param_ptr->pin_data.pin_data,
                                                      &hkey_card);

        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_0("Could not convert input Hiddenkey data to the card format");
          return MMGSDI_ERROR;
        }
      }
      else
      {
        UIM_MSG_ERR_1("Invalid Hiddenkey operation: 0x%x",
                      extra_param_ptr->pin_data.orig_pin_op);
        return MMGSDI_ERROR;
      }

      mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&client_cmd_index);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Unable to find valid index, 0x%x", mmgsdi_status);
        return mmgsdi_status;
      }

      /*-------------------------------------------------------------------------
      allocate and build extra param ptr
      -------------------------------------------------------------------------*/
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_extra_param_ptr,
        sizeof(mmgsdi_pin_extra_info_type));

      if (new_extra_param_ptr == NULL)
      {
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      mmgsdi_memscpy((void*)&new_extra_param_ptr->pin_data,
                     sizeof(new_extra_param_ptr->pin_data),
                     (const void*)&extra_param_ptr->pin_data,
                     sizeof(mmgsdi_pin_extra_info_type));

      mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                        client_cmd_index,
                        &req_header,
                        NULL,
                        new_extra_param_ptr);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Unable to populate client request 0x%x", mmgsdi_status);
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(new_extra_param_ptr);
        return mmgsdi_status;
      }

      /* File to be written to */
      memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type));
      file_access.access_method = MMGSDI_EF_ENUM_ACCESS;

      mmgsdi_status = mmgsdi_util_get_session_type(req_header.session_id,
                                                   &session_type,
                                                   NULL);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_0("Could not get Session type");
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
        return MMGSDI_ERROR;
      }

      if((session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
         (session_type == MMGSDI_GW_PROV_SEC_SESSION) ||
         (session_type == MMGSDI_GW_PROV_TER_SESSION))
      {
        file_access.file.file_enum = MMGSDI_USIM_HIDDENKEY;
      }
      else if((session_type == MMGSDI_1X_PROV_PRI_SESSION) ||
              (session_type == MMGSDI_1X_PROV_SEC_SESSION) ||
              (session_type == MMGSDI_1X_PROV_TER_SESSION))
      {
        file_access.file.file_enum = MMGSDI_CSIM_HIDDEN_KEY;
      }
      else
      {
        UIM_MSG_ERR_0("Invalid Session type");
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
        return MMGSDI_ERROR;
      }

      /* Data to be written to the card */
      write_data.data_len = hkey_card.data_len;
      write_data.data_ptr = hkey_card.data_ptr;

      /* Issue a WRITE request to EFHiddenkey */
      mmgsdi_status = mmgsdi_session_write_transparent(
                        req_header.session_id,
                        file_access,
                        0,
                        write_data,
                        mmgsdi_util_uicc_hidddenkey_ef_write_cb,
                        (mmgsdi_client_data_type)client_cmd_index);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Could not post Hiddenkey Write request, status 0x%x",
                      mmgsdi_status);
        mmgsdi_util_free_client_request_table_index(client_cmd_index);
      }
      return mmgsdi_status;
    }
  }/* end of if(report_status == MMGSDI_SUCCESS)*/

  mmgsdi_memscpy((void*)&extra_param_ptr->pin_data.pin_info,
                 sizeof(extra_param_ptr->pin_data.pin_info),
                 (const void*)&app_info_ptr->hiddenkey,
                 sizeof(mmgsdi_app_pin_info_type));

  /* Queue the confirmation */
  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* sw not required for pin op */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_hiddenkey_operation */


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_PIN_STATUS

   DESCRIPTION:
     This function will build the response to the Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol status, mmgsdi_icc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_pin_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;
  mmgsdi_int_app_info_type          * app_info_ptr          = NULL;

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!uim_rsp_ptr->is_uim_success)
  {

    UIM_MSG_HIGH_1("UICC RSP - PIN STATUS  Internal failure (mmgsdi_error_status 0x%x)",
                   uim_rsp_ptr->mmgsdi_error_status);

    mmgsdi_status = mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,
                                               &req_header,
                                               extra_param_ptr,
                                               (void*)&uim_rsp_ptr->rsp_data,
                                               FALSE, /* sw not required due to
                                                         internal processing error*/
                                               uim_rsp_ptr->status_word);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_prov_session_info(req_header.session_id,
                                                    NULL,
                                                    NULL,
                                                    &app_info_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  MMGSDIUTIL_RETURN_STATUS_IF_NULL(app_info_ptr, MMGSDI_ERROR);

  if(mmgsdi_util_is_pin_cached(app_info_ptr))
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                               &req_header,
                                               extra_param_ptr,
                                               (void*)&uim_rsp_ptr->rsp_data,
                                               FALSE, /* sw not required due to
                                                         internal processing error*/
                                               uim_rsp_ptr->status_word);

    UIM_MSG_HIGH_1("UICC RSP - PIN STATUS  Internal failure (pin not cached)  status 0x%x",
                   mmgsdi_status);

    return mmgsdi_status;
  }

  if(app_info_ptr->pin1_ptr == NULL ||
     app_info_ptr->pin1_ptr->cached == FALSE)
  {
    /*---------------------------------------------------------------------------
      1) Get the Num Retries - PIN1
      2) Get the Max Num Retries - PIN1
      3) Get the Num Retries - PIN2
      4) Get the Max Num Retries - PIN2
      5) Get the Num Retries - UNIVERSAL  PIN
      6) Get the Max Num Retries - UNIVERSAL PIN
    ---------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                              MMGSDI_PIN1,
                                              MMGSDI_PIN1,
                                              MMGSDI_PIN_OP_UNBLOCK,
                                              MMGSDI_PIN_OP_NONE,
                                              MMGSDI_UICC,
                                              NULL,
                                              NULL);
    return mmgsdi_status;
  }

  if(app_info_ptr->pin2.cached == FALSE)
  {
    mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                              MMGSDI_PIN2,
                                              MMGSDI_PIN2,
                                              MMGSDI_PIN_OP_UNBLOCK,
                                              MMGSDI_PIN_OP_NONE,
                                              MMGSDI_UICC,
                                              NULL,
                                              NULL);
    return mmgsdi_status;
  }

  if(app_info_ptr->universal_pin_ptr == NULL ||
     app_info_ptr->universal_pin_ptr->cached == FALSE)
  {
    mmgsdi_status = mmgsdi_util_build_pin_req(&req_header,
                                              MMGSDI_UNIVERSAL_PIN,
                                              MMGSDI_UNIVERSAL_PIN,
                                              MMGSDI_PIN_OP_UNBLOCK,
                                              MMGSDI_PIN_OP_NONE,
                                              MMGSDI_UICC,
                                              NULL,
                                              NULL);
    return mmgsdi_status;
  }

  UIM_MSG_ERR_0("PIN Cache flags not updated properly");
  return MMGSDI_ERROR;

} /* mmgsdi_uicc_rsp_pin_status*/


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_AUTH

   DESCRIPTION:
     This function will handle the response for a  USIM Authentication
     and build a response.

   DEPENDENCIES:


   LIMITATIONS:
     For ISIM , mmgsdi_session_main_response should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_auth(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_return_enum_type               report_status   = MMGSDI_SUCCESS;
  int32                                 index           = 0;
  uim_cmd_type                        * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr = NULL;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!uim_rsp_ptr->is_uim_success)
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  if (uim_rsp_ptr->rsp_data.data_len > 0 && uim_rsp_ptr->rsp_data.data_ptr != NULL)
  {
    UIM_MSG_HIGH_1("UICC auth response first byte: 0x%x",
                   uim_rsp_ptr->rsp_data.data_ptr[0]);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             uim_rsp_ptr->cmd_transacted,
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_auth*/


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_BCAST

   DESCRIPTION:
     This function will handle the response for a  USIM Authentication
     and build a response.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_bcast(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type            req_header;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_return_enum_type               report_status   = MMGSDI_SUCCESS;
  int32                                 index           = 0;
  uim_cmd_type                        * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr = NULL;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!uim_rsp_ptr->is_uim_success)
  {
    /* mmgsdi status will be populated as a result of sw1, and sw2
       This will be passed into the build confirmation function */
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             FALSE, /* sw not required
                                                       for bcast */
                                             uim_rsp_ptr->status_word);

    return mmgsdi_status;

}/* mgsdi_uicc_rsp_bcast */


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_PARSE_SELECT_RESPONSE_ADF

   DESCRIPTION:
     This function will parse the SELECT response for an ADF

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The SELECT response was successfully parsed.
     MMGSDI_ERROR:            The SELECT response was not parsed.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_parse_select_response_adf
(
  mmgsdi_int_app_info_type              * app_info_ptr,
  const uint8                           * rsp_data_ptr,
  uint16                                  rsp_data_len,
  mmgsdi_select_rsp_enum_type             select_rsp_type
)
{
  uint16                     offset        = 0;
  uint8                      adf_mask      = 0x38;
  uint8                      file_size_len = 0;
  uint16                     fcp_len       = 0;

  if(rsp_data_ptr == NULL || app_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null pointer");
    return MMGSDI_ERROR;
  }

  app_info_ptr->adf_attributes.info_valid = FALSE;

   /* Skip parsing if the first byte is not FCP templage tag */
  if((select_rsp_type == MMGSDI_SELECT_RSP_NO_DATA ||
      select_rsp_type == MMGSDI_SELECT_RSP_FCP) &&
     rsp_data_len >= 1 &&
     rsp_data_ptr[0] == GSDI_FCP_TEMPLATE_TAG)
  {
    UIM_MSG_HIGH_0("Parsing FCP template response");
  }
  else
  {
    UIM_MSG_ERR_1("SELECT response parsing not supported for template type 0x%x",
                  rsp_data_ptr[0]);
    return MMGSDI_ERROR;
  }

  /* The length of FCP can be expressed in 1 or 2 bytes  */
  /* According to ETSI 102.221, if the length is <= 0x7F,
     it is encoded in only 1 byte. If length is between 0x80
     and 0xFF, it is decoded in two bytes with first byte as
     0x81 and the second byte containing length  */
  if(rsp_data_len >= 2 && rsp_data_ptr[1] <= 0x7F)
  {
    /* One byte field for FCP length */
    fcp_len = rsp_data_ptr[1];
    offset = 2;
  }
  else if(rsp_data_len >= 3 && rsp_data_ptr[1] == 0x81)
  {
    /* Two byte field for FCP length  */
    fcp_len = rsp_data_ptr[2];
    offset = 3;
  }
  else
  {
    UIM_MSG_ERR_0("Invalid length byte in FCP");
    return MMGSDI_ERROR;
  }

  if(fcp_len + offset > rsp_data_len)
  {
    /* Do not return if the value of length field in FCP does not
       correspond to the length of response buffer as FCP may contain
       incorrect value of length */
    UIM_MSG_ERR_1("Wrong value of FCP length: 0x%x", fcp_len);
  }

  /* File Descriptor Tag (Mandatory) */
  if((offset + 2) < rsp_data_len &&
     rsp_data_ptr[offset] == GSDI_FILE_DESCRIPTOR_TAG)
  {
    if((rsp_data_ptr[offset+1] != 2) &&
       ((rsp_data_ptr[offset+2] & adf_mask) != adf_mask))
    {
      UIM_MSG_HIGH_0("File Descriptor Byte does not indicate ADF");
      return MMGSDI_ERROR;
    }

    if((rsp_data_ptr[offset + 2] & GSDI_FILE_DESC_SHAREABLE_MASK)
        == GSDI_FILE_DESC_SHAREABLE_MASK)
    {
      app_info_ptr->adf_attributes.adf_shareable = TRUE;
      UIM_MSG_HIGH_0("App is shareable across logical channels");
    }
    else
    {
      app_info_ptr->adf_attributes.adf_shareable = FALSE;
      UIM_MSG_HIGH_0("App is NOT shareable across logical channels");
    }
  }
  else
  {
    UIM_MSG_ERR_0("Invalid File Descriptor Tag");
    return MMGSDI_ERROR;
  }/* GSDI_FILE_DESCRIPTOR_TAG */

  /* File Identifier Tag (Optional for ADF) */
  offset = offset + 1 + rsp_data_ptr[offset+1] + 1;
  if((offset + 1) < rsp_data_len &&
     rsp_data_ptr[offset] == GSDI_FILE_IDENTIFIER_TAG)
  {
    offset = offset + 1 + rsp_data_ptr[offset+1] + 1;
  } /* GSDI_FILE_IDENTIFIER_TAG */

  /* DF Name Tag (Mandatory for ADF; contains AID) */
  if((offset + 1) < rsp_data_len &&
     rsp_data_ptr[offset] == GSDI_DF_NAME_AID_TAG)
  {
    if(((offset + 2) < rsp_data_len) &&
        (rsp_data_ptr[offset+1] <= MMGSDI_MAX_AID_LEN))
    {
      app_info_ptr->aid_in_fcp.data_len = rsp_data_ptr[offset+1];
      mmgsdi_memscpy(app_info_ptr->aid_in_fcp.data_ptr,
                     sizeof(app_info_ptr->aid_in_fcp.data_ptr),
                     &rsp_data_ptr[offset+2],
                     app_info_ptr->aid_in_fcp.data_len);
    }
    offset = offset + 1 + rsp_data_ptr[offset+1] + 1;
  }
  else
  {
    UIM_MSG_ERR_0("AID not present in SELECT AID response");
    return MMGSDI_ERROR;
  } /* GSDI_DF_NAME_TAG */

  /* Optional proprietary info tag */
  /* Life Cycle offset */
  if ((offset + 1) < rsp_data_len &&
      rsp_data_ptr[offset] == GSDI_PROPRIETARY_INFO_TAG)
  {
    /* increment the length */
    offset = offset + 1 + rsp_data_ptr[offset+1] + 1;
  }

  /* Mandatory life cyclic flag */
  if ((offset + 2) < rsp_data_len &&
      rsp_data_ptr[offset] == GSDI_LIFE_CYCLE_INTEG_TAG)
  {

    if((rsp_data_ptr[offset+2] &
        MMGSDI_UICC_LIFE_CYCLE_BYTE_CHECK) == 0)
    {
      /* File is activated */
      if( (rsp_data_ptr[offset+2] &
           MMGSDI_UICC_LIFE_CYCLE_FILE_ACTIVATED)
            == MMGSDI_UICC_LIFE_CYCLE_FILE_ACTIVATED)
      {
       app_info_ptr->adf_attributes.adf_status.file_invalidated = FALSE;
      }
      /* File is deactivated */
      else if( (rsp_data_ptr[offset+2] &
                MMGSDI_UICC_LIFE_CYCLE_FILE_DEACTIVATED)
                 == MMGSDI_UICC_LIFE_CYCLE_FILE_DEACTIVATED)
      {
        app_info_ptr->adf_attributes.adf_status.file_invalidated = TRUE;
      }
      else
      {
        UIM_MSG_HIGH_0("Life_cycle_byte different than file_activated/deactivated");
      }

      app_info_ptr->adf_attributes.adf_status.read_write_when_invalidated = TRUE;
    }
    offset = offset + 1 + rsp_data_ptr[offset+1] + 1;
  }
  else
  {
    UIM_MSG_ERR_0("Invalid Life Status Tag");
    return MMGSDI_ERROR;
  }

  /* Security attributes Tag (Mandatory) */
  if(((offset + 1) < rsp_data_len) &&
     (rsp_data_ptr[offset] == GSDI_SEC_ATTR_PROP_TAG ||
      rsp_data_ptr[offset] == GSDI_SEC_ATTR_EXPAND_TAG ||
      rsp_data_ptr[offset] == GSDI_SEC_ATTR_CMPACT_TAG ||
      rsp_data_ptr[offset] == GSDI_SEC_ATTR_REF_EXP_TAG) )
  {
    /* Security attribute parsing not supported */
    offset = offset + 1 + rsp_data_ptr[offset+1] + 1;
  }
  else
  {
    UIM_MSG_ERR_0("Invalid security attributes tag");
    return MMGSDI_ERROR;
  }

  /* PIN Status template DO Tag (Mandatory) */
  if((offset + 1) < rsp_data_len &&
     rsp_data_ptr[offset] == GSDI_PIN_STATUS_DO_TAG)
  {
    /* PS_DO bytes already parsed */
    offset = offset + 1 + rsp_data_ptr[offset+1] + 1;
  }
  else
  {
    UIM_MSG_ERR_0("Invalid PIN status DO tag");

    /* Ignore invalid or missing PIN Status Template to allow apps to be
       selected and continue parsing the remaining TLVs to correctly set
       adf_attributes.info_valid */
  }

  /* Total file size tag (Optional) */
  if((offset + 1) < rsp_data_len &&
     rsp_data_ptr[offset] == GSDI_TOTAL_FILE_SIZE_TAG )
  {
    uint8 i = 0;

    file_size_len = rsp_data_ptr[offset+1];
    for(i = 0; i < file_size_len && (offset+2+i) < rsp_data_len; i++)
    {
      app_info_ptr->adf_attributes.total_file_size =
        app_info_ptr->adf_attributes.total_file_size |
        ((uint32)rsp_data_ptr[offset+2+i] << 8*(file_size_len-i-1));
    }
  } /* GSDI_TOTAL_FILE_SIZE_TAG */
  /* FCP_TEMPLATE_TAG */

  app_info_ptr->adf_attributes.info_valid = TRUE;
  UIM_MSG_HIGH_0("SELECT AID response parsed");
  return MMGSDI_SUCCESS;
} /* mmgsdi_uicc_rsp_parse_select_response_adf */


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_SET_FILE_STATUS

   DESCRIPTION:
     This function will build the response to activate/deactivate request

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_rsp_invalidate/
     mmgsdi_icc_rsp_rehabilitate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_set_file_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_cmd_enum_type                    req_type
)
{
  mmgsdi_request_header_type            req_header;
  int32                                 index            = 0;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr  = NULL;
  uim_cmd_type                        * uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type               report_status    = MMGSDI_SUCCESS;
  mmgsdi_file_enum_type                 mmgsdi_file      = MMGSDI_MAX_FILE_ENUM;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if (req_type != MMGSDI_INVALIDATE_REQ && req_type != MMGSDI_REHABILITATE_REQ)
  {
    return MMGSDI_ERROR;
  }

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (req_type == MMGSDI_INVALIDATE_REQ)
  {
    mmgsdi_file = extra_param_ptr->invalidate_data.access.file.file_enum;
  }
  else
  {
    mmgsdi_file = extra_param_ptr->rehab_data.access.file.file_enum;
  }

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* processing of command failure */
    UIM_MSG_HIGH_3("UIM_FAIL sw1 %x sw2 %x status 0x%x",
                   uim_rsp_ptr->status_word.sw1,
                   uim_rsp_ptr->status_word.sw2,
                   report_status);
  } /* Parsing UIM Status */

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      FALSE, /* status word not required for invalidate cmd */
      uim_rsp_ptr->status_word);

  UIM_MSG_HIGH_3("UICC RSP - REQ 0x%x status is 0x%x for file 0x%x",
                 req_type, report_status, mmgsdi_file);

  return mmgsdi_status;
} /* mmgsdi_uicc_rsp_set_file_status */


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_MANAGE_CHANNEL_OP_SYNC

   DESCRIPTION:
     This function will build the response to the manage Channel sync Command

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_manage_channel_op_sync (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_session_deactivate_cnf_type   ** mmgsdi_cnf_pptr,
  mmgsdi_channel_id_type                 *out_channel_id_ptr,
  uim_mc_action_type                      mc_action
)
{

  mmgsdi_request_header_type           req_header;
  int32                                index                 = 0;
  mmgsdi_return_enum_type              mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                       * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr       = NULL;
  mmgsdi_return_enum_type              report_status         = MMGSDI_SUCCESS;
  mmgsdi_session_deactivate_cnf_type * cnf_ptr               = NULL;

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* ------------------------------------------------------------------------
       Command processing returned Status words other than 0x90 0x00
       ----------------------------------------------------------------------*/
   MMGSDIUTIL_DEBUG_MSG_ERROR_3("UIM_FAIL sw1:%x sw2:%x status:0x%x",
                                uim_rsp_ptr->status_word.sw1,
                                uim_rsp_ptr->status_word.sw2,
                                report_status);
  } /* Parsing UIM Status */

  if (mmgsdi_cnf_pptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Obtain memory for confirmation data */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*mmgsdi_cnf_pptr),
    sizeof(mmgsdi_session_deactivate_cnf_type));
  if ((*mmgsdi_cnf_pptr) == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  cnf_ptr = *mmgsdi_cnf_pptr;

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_util_populate_cnf_rsp_header(cnf_ptr,
                                        &cnf_ptr->response_header,
                                        &req_header,
                                        MMGSDI_SESSION_DEACTIVATE_CNF,
                                        uim_rsp_ptr->cmd_transacted,
                                        uim_rsp_ptr->status_word);

    cnf_ptr->response_header.mmgsdi_status = report_status;
  }
  else
  {
    UIM_MSG_ERR_1("Failed to build Manage CHANNEL data: status 0x%x", mmgsdi_status);
    cnf_ptr->response_header.mmgsdi_status = mmgsdi_status;
  }

  /* Copy the channel id from rsp if the req is open channel */
  if (cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS &&
      out_channel_id_ptr != NULL &&
      mc_action == UIM_MC_OPEN &&
      uim_rsp_ptr->rsp_data.data_len > 0 &&
      uim_rsp_ptr->rsp_data.data_ptr != NULL)
  {
    *out_channel_id_ptr = uim_rsp_ptr->rsp_data.data_ptr[0];
  }

  return cnf_ptr->response_header.mmgsdi_status;
} /* mmgsdi_uicc_rsp_manage_channel_op */


/*===========================================================================
FUNCTION MMGSDI_UICC_RSP_MANAGE_CHANNEL

DESCRIPTION
  Function to handle the MANAGE CHANNEL response received from UIM.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_manage_channel(
  const mmgsdi_uim_report_rsp_type*     uim_rsp_ptr
)
{
  mmgsdi_request_header_type           req_header;
  int32                                index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uim_cmd_type                       * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr = NULL;
  mmgsdi_return_enum_type              report_status   = MMGSDI_SUCCESS;

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);
  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);
  if (report_status != MMGSDI_SUCCESS)
  {
    /* processing of command failure */
    UIM_MSG_HIGH_3("UIM_FAIL sw1 %x sw2 %x status 0x%x",
                   uim_rsp_ptr->status_word.sw1,
                   uim_rsp_ptr->status_word.sw2,
                   report_status);
  } /* Parsing UIM Status */

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             uim_rsp_ptr->cmd_transacted,
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
}/* mmgsdi_uicc_rsp_manage_channel */


#ifdef FEATURE_UIM_MULTI_PROFILE
/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_UICC_RSP_SIM_PROFILE

  DESCRIPTION:
    This function will build the response for set sim profile request

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of USIM

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_sim_profile(
  const mmgsdi_uim_report_rsp_type       *uim_rsp_ptr,
  mmgsdi_profile_id_enum_type             profile_id,
  uint8                                   apdu_index,
  uint8                                   num_apdu_count
)
{
  mmgsdi_request_header_type          req_header;
  mmgsdi_return_enum_type             mmgsdi_status    = MMGSDI_SUCCESS;
  int32                               index            = 0;
  uim_cmd_type                       *uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type             report_status    = MMGSDI_SUCCESS;
  mmgsdi_sw_status_type               status_word      = {0};
  mmgsdi_sw_status_type               same_status_word = {0};
  mmgsdi_sw_status_type               diff_status_word = {0};
  mmgsdi_client_req_extra_info_type  *extra_param_ptr  = NULL;

  UIM_MSG_HIGH_0("MMGSDI_UICC_RSP_SIM_PROFILE");

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL_2(uim_rsp_ptr, mmgsdi_profile_info_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Error in getting info from client req table info");
    return mmgsdi_status;
  }

  UIM_MSG_HIGH_2("UICC RSP - uim_rsp_sw1 = 0x%x, uim_rsp_sw2 = 0x%x", 
                 uim_rsp_ptr->status_word.sw1, uim_rsp_ptr->status_word.sw2);

  if(mmgsdi_profile_info_ptr->get_active_profile)
  {
    report_status = mmgsdi_uicc_rsp_parse_uim_response(uim_rsp_ptr);

    if(uim_rsp_ptr->rsp_data.data_len == 0)
    {
      UIM_MSG_ERR_0("Multi profile support is not available with the card");
      return MMGSDI_ERROR;
    }

    if(uim_rsp_ptr->rsp_data.data_ptr != NULL)
    {
      if(uim_rsp_ptr->rsp_data.data_ptr[MMGSDI_MULTI_PROFILE_ACTIVE_PROFILE_BYTE] ==
           MMGSDI_PROFILE_TYPE_REGULAR)
      {
        mmgsdi_profile_info_ptr->active_profile.profile_id = MMGSDI_PROFILE_1;
        mmgsdi_profile_info_ptr->active_profile.profile_type = MMGSDI_PROFILE_TYPE_REGULAR;

        /* We can make profile detected as TRUE as we got NO_EFFECT in 
           confirmation which means the current active profile in the card is REGULAR */
        mmgsdi_profile_info_ptr->reg_profile_detected = TRUE;

        report_status = MMGSDI_MULTI_PROFILE_NO_EFFECT;
      }
      else if(uim_rsp_ptr->rsp_data.data_ptr[MMGSDI_MULTI_PROFILE_ACTIVE_PROFILE_BYTE] ==
                MMGSDI_PROFILE_TYPE_EMERGENCY)
      {
        mmgsdi_profile_info_ptr->active_profile.profile_id = MMGSDI_PROFILE_2;
        mmgsdi_profile_info_ptr->active_profile.profile_type = MMGSDI_PROFILE_TYPE_EMERGENCY;
        report_status = MMGSDI_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_1("Active profile retrieved from the card is invalid: 0x%x",
                      uim_rsp_ptr->rsp_data.data_ptr[MMGSDI_MULTI_PROFILE_ACTIVE_PROFILE_BYTE]);
        return MMGSDI_ERROR;
      }
    }
  }
  else
  {
    mmgsdi_sw_status_type derived_status_word = {0};

    switch(profile_id)
    {
      case MMGSDI_PROFILE_1:
        /* Request to switch back to profile 1 (regular). So, we use switch back status word */
        same_status_word.sw1 = mmgsdi_profile_info_ptr->sw_switch_back_same_profile[apdu_index].sw1;
        same_status_word.sw2 = mmgsdi_profile_info_ptr->sw_switch_back_same_profile[apdu_index].sw2;

        diff_status_word.sw1 = mmgsdi_profile_info_ptr->sw_switch_back_diff_profile[apdu_index].sw1;
        diff_status_word.sw2 = mmgsdi_profile_info_ptr->sw_switch_back_diff_profile[apdu_index].sw2;

      break;

      case MMGSDI_PROFILE_2:
        /* Request to switch to profile 2 (emergency). So, we use switch status word */
        same_status_word.sw1 = mmgsdi_profile_info_ptr->sw_switch_same_profile[apdu_index].sw1;
        same_status_word.sw2 = mmgsdi_profile_info_ptr->sw_switch_same_profile[apdu_index].sw2;

        diff_status_word.sw1 = mmgsdi_profile_info_ptr->sw_switch_diff_profile[apdu_index].sw1;
        diff_status_word.sw2 = mmgsdi_profile_info_ptr->sw_switch_diff_profile[apdu_index].sw2;

      break;
  
      default:
        UIM_MSG_ERR_1("Invalid profile id: 0x%x", profile_id);
        return MMGSDI_ERROR;
    }

    derived_status_word.sw1 = uim_rsp_ptr->status_word.sw1;
    derived_status_word.sw2 = uim_rsp_ptr->status_word.sw2;

    /* Card returns 91 XX when there is a pending proactive command with the
       card and switch APDU command is successful. As the switch is successful,
       the response status words are converted to switched profile status word
       to report successful response to the client */
    if(uim_rsp_ptr->status_word.sw1 == GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO)
    {
      derived_status_word = diff_status_word;
    }

    if(derived_status_word.sw1 == diff_status_word.sw1 &&
       derived_status_word.sw2 == diff_status_word.sw2)
    {
      report_status = MMGSDI_SUCCESS;
    }
    else if(derived_status_word.sw1 == same_status_word.sw1 &&
            derived_status_word.sw2 == same_status_word.sw2)
    {
      report_status = MMGSDI_MULTI_PROFILE_NO_EFFECT;
    }
    else
    {
      report_status = MMGSDI_ERROR;
    }

    UIM_MSG_HIGH_1("UICC RSP - SIM Profile status is 0x%x", report_status);

    /* The following if condition will be only true when we are in middle of 
       sending the APDUs to the card for switching the profile. If the apdu 
       index is equal to number of APDU count then we have to send the card 
       reset command based on the switch action*/
    if(apdu_index < num_apdu_count - 1)
    {
      return report_status;
    }

    /* If the caller is MMGSDI then update the active profile information */
    if(req_header.client_id == mmgsdi_generic_data_ptr->client_id &&
       (report_status == MMGSDI_SUCCESS || report_status == MMGSDI_MULTI_PROFILE_NO_EFFECT))
    {
      mmgsdi_profile_info_ptr->active_profile.profile_id = MMGSDI_PROFILE_1;
      mmgsdi_profile_info_ptr->active_profile.profile_type = MMGSDI_PROFILE_TYPE_REGULAR;
      mmgsdi_profile_info_ptr->reg_profile_detected = TRUE;
    }

    status_word.sw1 = uim_rsp_ptr->status_word.sw1;
    status_word.sw2 = uim_rsp_ptr->status_word.sw2;

    /* If switching of the profile is successful then do the action required as per the configuration 
       At present, we queue the CARD_RESET request even in case of NO_ACTION case.*/
    if(report_status == MMGSDI_SUCCESS &&
       (mmgsdi_profile_info_ptr->switch_action == MMGSDI_MULTI_PROFILE_CARD_SWITCH_ACTION_UICC_RESET ||
        mmgsdi_profile_info_ptr->switch_action == MMGSDI_MULTI_PROFILE_CARD_SWITCH_ACTION_NONE))
    {
      mmgsdi_status = mmgsdi_gen_build_card_reset(req_header.client_id,
                                                  req_header.slot_id,
                                                  MMGSDI_CARD_RESET_REQ,
                                                  UIM_UICC,
                                                  MMGSDI_RESET_CAUSE_PROFILE_SWITCH,
                                                  req_header.response_cb,
                                                  req_header.client_data);
  
      /* Reset Session and application data for the slot */
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        UIM_MSG_HIGH_0("Reset Session and application data");
        mmgsdi_util_preinit_mmgsdi(0,
                                   MMGSDI_APP_NONE,
                                   MMGSDI_REFRESH_RESET,
                                   req_header.slot_id,
                                   FALSE);
  
        mmgsdi_refresh_update_card_state(req_header.slot_id);
      }
    }
  }
  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             NULL,
                                             NULL,
                                             TRUE,
                                             status_word);
  return report_status;
} /* mmgsdi_uicc_rsp_sim_profile */
#endif /* FEATURE_UIM_MULTI_PROFILE */

