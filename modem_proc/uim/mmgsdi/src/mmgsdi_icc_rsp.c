/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   I C C   R S P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC response processing support for MMGSDI.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_icc_rsp.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/16   bcho    F3 frequency reduction by conditional logic
06/07/16   ar      Removing string prints from APDU and AUTH
05/24/16   ar      Review of macros used by MMGSDI
05/13/16   dd      Skip Search Req for PB files
05/17/16   ar      Fix read/write of cache files when cache is not_init
10/26/15   nr      Disable MMGSDI PB cache for PBM module
07/14/15   ar      Correct the caching logic of EF-PRL and EF-EPRL
07/14/15   ar      Cache complete data for req's with partial read/write data
05/13/15   stv     Handling 0x92 0x40 status word in error or failure case
04/17/15   nr      Phonebook record read optimization
01/09/15   vdc     Fix compiler warnings
01/09/15   ar      Cache file if req_len is more than file size
12/09/14   ar      Replace mmgsdi_u/icc_rsp_srv_available with common func
10/16/14   tl      Fix report status logic for SW1: 0xFF
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/28/14   vv      Remove unused function
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/18/14   av      Fix out of bound access in mmgsdi_util_find_app_pin_index
05/13/14   yt      Cache EF-PRL/EF-EPRL based on the actual PRL size
05/12/14   bcho    NULL check added before dereferencing extra_param_ptr
04/22/14   av      Fix double free when handling icc pin operation response
02/05/14   tkl     SFI Support and decouple uim file enums
01/21/14   yt      Remove duplicate code
01/16/14   df      Add new sim phase enumeration
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/16/13   tl      Prevent initalizing the cache with partial files
12/11/13   df      Remove unused memory paging feature
11/20/13   spo     Updated mmgsdi status to return success in 92 0X handling
11/06/13   tl      Remove TO-DO
10/21/13   yt      Parse 92 XX status words when UIM report status is PASS
10/04/13   rp      F3 message reduction
09/20/13   yt      Remove duplicate code
08/07/13   tl      Add CDMA AKA Authenticate functionality
06/10/13   tkl     Marked cache not found when UIM return command timeout
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
04/16/13   tl      Clean up of radio access features from MMGSDI code
02/21/13   tl      Added NULL pointer check for memory allocation and
                   clear cache for write commands when allocation fails
01/31/13   tl      Added new memory management error code
10/11/12   spo     Removed unnecessary F3 messages
10/03/12   abg     Mask the specific success status w.r.t. PIN operations -
                   MMGSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE from clients
                   and return the generic MMGSDI_SUCCESS
10/02/12   yt      Update valid_num_retries flag for PIN2
09/28/12   abg     Updated ERR to MSG_ERROR
09/03/12   bcho    mmgsdi_icc_rsp_search modified to support sync operation,
                   cache OPL-PNN records in mmgsdi_icc_rsp_read()
03/07/12   yt      Report PIN OP status based on status of subsequent SELECT
01/26/12   vv      Set sw_present field in PIN operation resp
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   kk      Legacy GSDI removal updates
10/28/11   yt      Added new files for cache-related utility functions
10/13/11   shr     Adding support for full recovery
09/28/11   tkl     Fixed write cache for TMSI PLMN check
09/06/11   shr     Do not attempt a Card Reset as part of EFRESH RESET/Card
                   power-up handling, if the REFRESH has been stopped
08/18/11   av      Fixes for MMGSDI Klocwork CRITICAL warnings
08/09/11   tkl     Support for TMSI PLMN check
05/17/11   yt      Added support for INCREASE request
02/01/11   ssr     Add extra_param_ptr NULL check for pin request
01/24/11   shr     After any successful/unsuccessful PIN operation on ICC
                   cards, retrieve the PIN status from the card
01/17/11   yt      Indicate that status word is present in Auth response
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
10/27/10   ms      Changes for enablement of C+G and
                   Fix for mmgsdi_icc_rsp_parse_pin issue
06/22/10   nb      Passon the uim_rsp_ptr->cmd_transacted field while creating
                   cnf header so that sw_present field is appropriatly populated
05/19/10   nb      Updated mmgsdi_icc_rsp_write to support sync writes
03/02/10   kk      Fixed get file attr rsp handling
12/18/09   nb      Moved Dual Slot Changes
12/07/09   js      Fixed lint errors
12/03/09   shr     Cache read process enhancement
12/02/09   js      Fixes for RUIM power up
09/26/09   kp      ZI memory reduction changes
07/27/09   kk      Fixed Lint Error - Boolean within 'if' always evaluates
                   to False
07/08/09   shr     Fixed Lint warnings
06/10/09   ssr     Fixed unblock retry counter for incorrect pin
04/29/09   js      Included header mmgsdi_icc_rsp.h
03/12/09   js      Support for reporting invalid number of PIN retries
01/14/09   vs      Fixed KW error
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/19/08   nb      Klockwork Fixes
11/05/08   ssr     Resolved compiler error for RUIM enablement
11/03/08   tml     Returned error if the get all pin status command processing
                   was erroneous and lint fixes
10/20/08   sun     Allow protocol to be sent as part of card reset
09/22/08   js      Fixed Lint Errors
09/08/08   sun     Fixed Compilation
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/26/08   jk      Fixed 1X Auth featurization for LTK Compilation
08/21/08   sun     Select should happen on RUIM DIR rather than GSM DIR for CDMA
                   protocol
08/21/08   sun     Featurized code, code review comments
08/20/08   jk      Support for Generate Key API & Compile Fixes related to PKCS15
08/20/08   kk      Calls in create_pkcs15_table made to async
07/30/08   SSR     Fixed MMGSDI status for ICC Search, for SW1 WRONG Length
07/20/08   sp      Update cache properly for read on record based files
07/15/08   jk      Fixed Compile Errors / Warnings
06/12/08   ssr     Support of MMGSDI Search functionality
06/05/08   tml     Added sw in confirmation and messages cleanup
05/23/08   vs      Fixed file status info bit decoding
03/17/08   nk      Fixed Klocwork Issues.
03/03/08   nk      Added synchronous support in mmgsdi_icc_rsp_get_file_attr
                   and mmgsdi_icc_rsp_rehabitate
02/29/08   vs      Changed pin_status to not initialized if bit is set to zero
12/11/07   sun     Build Card Reset if the original request is for CARD_RESET
11/15/07   sun     Added support for NAA refresh
11/05/07   tml     Fixed null pointer access when the file read was from
                   gsdi cache
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
04/04/07   sun     Fixed Double Freeing of pin_info_ptr
03/26/07   sun     Check if additional files are being read during refresh
                   fcn
03/20/07   sun     Select the first index of app_pin1 by default
03/19/07   sun     Fixed Lint Error
02/21/07   sun     Removed Global PIN from GSDI
01/23/07   sp      Set the cnf_status with uim report status.
01/18/07   sun     First Handle the Select Response and then check the
                   retries
08/07/06   sun     Added PIN and Valid PIN to mmgsdi_app_pin_info_type
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     lint
06/08/06   tml     fixed featurized
05/23/06   tml     BDN, Rehabilitate and invalidate supports and lint fixes
04/18/06   nk      Featurized code
04/13/06   jar     Added ONCHIP SIM Support
03/08/06   sun     Do not parse the UIM Response data for Streaming APDU
02/15/06   tml     Added data fr card info to write cache item
02/14/06   nk      Merge for: ARR caching and by path changes
12/21/05   sun     Return Proper error ISIM Authentication failure
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
11/03/05   sun     For SAP APDU, return success even file is not found
08/30/05   jar     Added support to handle a ICC Status Response
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/26/05   sst     Added MMGSDIBT support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   sk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdicache.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_file.h"
#include "mmgsdi_pb_cache.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Minimum required length of file attribute response length */
#define MMGSDI_ICC_MANDATORY_FILE_ATTR_RESP_LEN 14

/* Minimum required length of mandatory or dedicated file attribute response length */
#define MMGSDI_ICC_MANDATORY_MF_DF_FILE_ATTR_RESP_LEN 22

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_PARSE_UIM_RESPONSE

   DESCRIPTION:
     This function will parse write access to any file using the ICC protocol

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
mmgsdi_return_enum_type mmgsdi_icc_rsp_parse_uim_response(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
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
      mmgsdi_status = MMGSDI_SUCCESS;
      break;

    case GSDI_SW1_MEMORY_MANAGEMENT: /* 0x92 */
        UIM_MSG_HIGH_0("Success After Card Internal Update");
        if ((sw2 & 0xF0) == 0)  /* 0x0X */
        {
          switch (rpt_code)
          {
          case UIM_SELECT_R:
          case UIM_STATUS_R:
          case UIM_SEEK_R:
          case UIM_RUN_GSM_ALGO_R:
            /* read binary, read record is not valid */
            UIM_MSG_ERR_1("Invalid Report Code %x", rpt_code);
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
          default:
            /* command success only after X times internal retries */
            /* May need to set to GSDI SUCCESS because action did  */
            /* eventually succeed                                  */
            mmgsdi_status = MMGSDI_SUCCESS;
            break;
          }
        }
        else
        {
          /* Can't determine error but    */
          /* Error doesn't need to create */
          /* an event to the UI           */
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
        }
      break; /* GSDI_SW1_MEMORY_MANAAGEMENT 0x92 */

    default: /* other sw1 when rpt_status == PASS */
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }
  }
  else
  {
    /* UIM_FAIL */
    switch ( sw1 )
    {
    case GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO: /* 0x91 XX*/
      UIM_MSG_HIGH_0("GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO");
      mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
      break; /* GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO  0x91 XX*/

    case GSDI_SW1_SIM_APP_TOOLKIT_BUSY: /* 0x93 */
      UIM_MSG_HIGH_0("GSDI_SW1_SIM_APP_TOOLKIT_BUSY");
      switch (rpt_code)
      {
      case UIM_ENVELOPE_R:
        /* Sim is busy */
        mmgsdi_status = MMGSDI_SIM_BUSY;
        break;
      default:
        UIM_MSG_ERR_1("Invalid report code %x", rpt_code);
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
        UIM_MSG_HIGH_0("GSDI_SW2_NO_EF_SELECTED");
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
          UIM_MSG_ERR_1("Invalid report code %x", rpt_code);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_OUT_OF_RANGE: /* 0x02 */
        UIM_MSG_HIGH_0("GSDI_SW2_OUT_OF_RANGE");
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
        UIM_MSG_HIGH_0("GSDI_SW2_FILE_OR_PATTER_ID_NOT_FOUND");
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
          UIM_MSG_ERR_1("Invalid report code %x", rpt_code);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_FILE_INCONSISTENT_WITH_CMD: /* 0x08 */
        UIM_MSG_HIGH_0("GSDI_SW2_FILE_INCONSISTENT_WITH_CMD");
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
        UIM_MSG_HIGH_0("GSDI_SW2_NO_CHV_INITIALISED");
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
          UIM_MSG_ERR_1("Invalid report code 0x%x", rpt_code);
          mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
          break;
        }
        break;

      case GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1: /* 0x04 */
        UIM_MSG_HIGH_0("GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1");
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
        UIM_MSG_HIGH_0("GSDI_SW2_CNTRCTN_W_CHV_STATUS");
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
        UIM_MSG_HIGH_0("GSDI_SW2_CNTRCTN_W_INVALIDATION_STATUS");
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
        UIM_MSG_HIGH_0("GSDI_SW2_INCREASED_FAIL_MAX_REACHED");
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
        UIM_MSG_HIGH_0("GSDI_SW2_AUTH_ERROR_APP_SPECIFIC");
        switch (rpt_code)
        {
          case UIM_ISIM_AUTHENTICATE_R:
            /* Authentication failed        */
            mmgsdi_status = MMGSDI_AUTH_ERROR_INCORRECT_MAC;
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

    case GSDI_SW1_WRONG_LENGTH: /* 0x67 */
      UIM_MSG_HIGH_0("GSDI_SW1_WRONG_LENGTH");
      /* or GSDI_SW1_INCORRECT_P3 - doesn't have specific sw2 */
      mmgsdi_status = MMGSDI_INCORRECT_LENGTH;
      break; /* GSDI_SW1_WRONG_LENGTH 0x67 */

    case GSDI_SW1_INCORRECT_P1_OR_P2: /* 0x6B */
      UIM_MSG_HIGH_0("GSDI_SW1_INCORRECT_P1_OR_P2");
      /* Application Independent Errors   */
      /* Incorrect Parameters P3          */
      /* or Incorrect Parameters P1 & P2  */
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break; /* GSDI_SW1_INCORRECT_P1_OR_P2 0x6B */

    case GSDI_SW1_UNKNOWN_INSTRUCTION_CODE: /* 0x6D */
    case GSDI_SW1_WRONG_INSTRUCTION_CLASS:  /* 0x6E */
      UIM_MSG_HIGH_0("GSDI_UNKNOWN_INST_CLASS");
      /* Unknown or Wrong Instruction class  */
      mmgsdi_status = MMGSDI_UNKNOWN_INST_CLASS;
      break; /* GSDI_SW1_WRONG_INSTRUCTION_CLASS 0x6E */

    case GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN: /* 0x6F */
      UIM_MSG_HIGH_0("GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN");
      /* Application Independent Erros    */
      mmgsdi_status = MMGSDI_SIM_TECHNICAL_PROBLEMS;
      break; /* GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN 0x6F */

    case GSDI_SIM_SW1_NORM_END: /* 0x90 */
      UIM_MSG_HIGH_0("GSDI_SIM_SW1_NORM_END : UIM CMD TIME OUT");
      mmgsdi_status = MMGSDI_UIM_CMD_TIMEOUT;
      break; /* GSDI_SIM_SW1_NORM_END 0x90 */

    case GSDI_SW1_UIM_TIMEOUT: /* 0x00 */
      UIM_MSG_HIGH_0("GSDI_SW1_UIM_TIMEOUT");
      switch ( sw2 ) {
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

      case GSDI_SW1_MEMORY_MANAGEMENT: /* 0x92 */
        switch(sw2)
        {
          case GSDI_SW2_MEMORY_PROBLEM: /* 0x40 */
            UIM_MSG_HIGH_0("GSDI_SW2_MEMORY_PROBLEM");
            /* Memory Management Error - SIM Memory Problem */
            mmgsdi_status = MMGSDI_MEMORY_ERROR;
            break;

          default:
            mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
            break;
        }
        break;

    default:
      /* Unhandled Error Condition */
      mmgsdi_status = MMGSDI_ERROR_NO_EVENT_NEEDED;
      break;
    }
  } /* UIM Fail */
  return mmgsdi_status;
} /* mmgsdi_icc_rsp_parse_uim_response */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_POPULATE_ICC_FILE_STATUS_INFO

  DESCRIPTION:
    This function populates the File Status into mmgsdi file status
    type according to GSM spec 11.11

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static void mmgsdi_populate_icc_file_status_info(
  mmgsdi_slot_id_enum_type     slot,
  const uint8                * header_status_attr,
  mmgsdi_file_status_type    * file_status_ptr
)
{
  mmgsdi_slot_data_type      *slot_data_ptr = NULL;
  mmgsdi_sim_phase_enum_type  phase         = MMGSDI_SIM_PHASE_NOT_INIT;

  if((header_status_attr == NULL) || (file_status_ptr == NULL))
  {
    UIM_MSG_ERR_2("Null input params: header_status_attr 0x%x or file_status_ptr 0x%x",
                  header_status_attr, file_status_ptr);
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  phase = slot_data_ptr->sim_phase;

  if((!(slot_data_ptr->card_apps_available & MMGSDI_GSM_APP_MASK )) &&
       (slot_data_ptr->card_apps_available & MMGSDI_RUIM_APP_MASK))
  {
    phase = MMGSDI_SIM_PHASE_2;
  }

  /* Per 3GPP TS 11.11, section 9.3 if bit b1 is set to 1 (bit mask 0x01)
     the file is not invalidated. If it is set to 0 file is invalidated */
  if ((header_status_attr[11] & 0x01) == 0x01)
  {
    file_status_ptr->file_invalidated = FALSE;
  }
  else
  {
    file_status_ptr->file_invalidated = TRUE;
  }

  if((phase == MMGSDI_SIM_PHASE_2)||
     (phase == MMGSDI_SIM_PHASE_2_PLUS))
  {
    /* Per 3GPP TS 11.11, section 9.3 if bit b3 is set to 1 (bit mask 0x04)
       the file is readable and updateable when invalidated else not */
    if ((header_status_attr[11] & 0x04) == 0x04)
    {
      file_status_ptr->read_write_when_invalidated = TRUE;
    }
    else
    {
      file_status_ptr->read_write_when_invalidated = FALSE;
    }
  }
} /* mmgsdi_populate_icc_file_status_info */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_POPULATE_ICC_SECURITY_INFO

  DESCRIPTION:
    This function populates security information according to GSM spec 11.11

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static mmgsdi_return_enum_type mmgsdi_populate_icc_security_info(
  uint8                      card_access_cond,
  mmgsdi_file_security_type *protection_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_pin_enum_type    pin_id        = MMGSDI_MAX_PIN_ENUM;
  uint32                  pin_len       = 0;

  MMGSDIUTIL_RETURN_IF_NULL(protection_ptr);
  protection_ptr->num_protection_pin = 0;
  protection_ptr->protection_pin_ptr = NULL;
  protection_ptr->protection_method = MMGSDI_NEVER_ALLOWED;
  switch (card_access_cond)
  {
  case 0x00:
    protection_ptr->protection_method = MMGSDI_ALWAYS_ALLOWED;
    return mmgsdi_status;
  case 0x01:
    pin_id = MMGSDI_PIN1;
    break;
  case 0x02:
    pin_id = MMGSDI_PIN2;
    break;
  case 0x03:
    protection_ptr->protection_method = MMGSDI_NEVER_ALLOWED;
    return mmgsdi_status;
  case 0x04:
    pin_id = MMGSDI_ADM1;
    break;
  case 0x05:
    pin_id = MMGSDI_ADM2;
    break;
  case 0x06:
    pin_id = MMGSDI_ADM3;
    break;
  case 0x07:
    pin_id = MMGSDI_ADM4;
    break;
  case 0x08:
    pin_id = MMGSDI_ADM5;
    break;
  case 0x09:
    pin_id = MMGSDI_ADM6;
    break;
  case 0x0A:
    pin_id = MMGSDI_ADM7;
    break;
  case 0x0B:
    pin_id = MMGSDI_ADM8;
    break;
  case 0x0C:
    pin_id = MMGSDI_ADM9;
    break;
  case 0x0D:
    pin_id = MMGSDI_ADM10;
    break;
  case 0x0E:
    pin_id = MMGSDI_ADM11;
    break;
  case 0x0F:
  default:
    protection_ptr->protection_method = MMGSDI_NEVER_ALLOWED;
    return mmgsdi_status;
  }

  /* Single PIN is required conditions */
  protection_ptr->num_protection_pin = 1;

  pin_len = (int32touint32(protection_ptr->num_protection_pin)
             * sizeof(mmgsdi_pin_enum_type));

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((protection_ptr->protection_pin_ptr),
                                     pin_len);
  if(protection_ptr->protection_pin_ptr == NULL)
  {
    mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    protection_ptr->num_protection_pin = 0;
  }
  else
  {
    protection_ptr->protection_method = MMGSDI_SINGLE_ALLOWED;
    protection_ptr->protection_pin_ptr[0] = pin_id;
  }
  return mmgsdi_status;
} /* mmgsdi_populate_icc_security_info */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_POPULATE_ICC_SEC_ATTR_FROM_UIM_TO_MMGSDI

  DESCRIPTION:
    This function populate the Security Attribute into mmgsdi file protection
    type according to GSM spec 11.11

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static void mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi(
  uint8                            * sim_header_attr,
  mmgsdi_file_security_access_type * file_security_access_ptr
)
{
  if ((file_security_access_ptr == NULL) || (sim_header_attr == NULL))
  {
    UIM_MSG_ERR_2("Sim header attr 0x%x of File Sec Acc Pt 0x%x is null",
                  sim_header_attr, file_security_access_ptr);
    return;
  }

  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[8] / 0x10),
    &file_security_access_ptr->read);
  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[8]& 0x0F),
    &file_security_access_ptr->write);
  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[9] / 0x10),
    &file_security_access_ptr->increase);
  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[10] / 0x10),
    &file_security_access_ptr->rehabilitate_activate);
  (void)mmgsdi_populate_icc_security_info(
    (sim_header_attr[10]& 0x0F),
    &file_security_access_ptr->invalidate_deactivate);
} /* mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_RSP_EXTRACT_FILE_ATTR

  DESCRIPTION:
    This function is used to decode and map the Header information from the SIM
    format as define in GSM 11.11 as follows:

    byte 01:    rfu
    byte 02:    rfu
    byte 03:    file size MSB
    byte 04:    file size LSB
    byte 05:    file id MSB
    byte 06:    file id LSB
    byte 07:    type of file
              00 --> RFU
              01 --> MF
              02 --> DF
              04 --> EF
    byte 08:    RFU for Linear and Transparent Files
              bit7 = 1 Indicates Cyclic Increase allowed
    byte 09:    B8-B5 Read/Seek Permissions
              B4-B1 Update Permissions
    byte 10:    B8-B5 Increase Permissions
              B4-B1 RFU
    byte 11:    B8-B5 RehabilitatePermissions
              B4-B1 Invalidate Permissions
    byte 12:    File Status
              B8-B4 RFU
              B3 = 0 Not Readable/Writeable when invalidate
              B3 = 1 Readable/Writeable when invalidated
              B2 RFU
              B1 = 0 (Invalidated)
              B1 = 1 (Not Invalideated - Valid)
    byte 13:    Length of the following Information (Byte 14 to end]
    byte 14:    Structure of EF
              00 --> TRANSPARENT
              01 --> LINEAR FIXED
              03 --> CYCLIC
    byte 15:    Length of Record.  For cyclic and linear fixed, this denotes
              the length of the record.  For a transparent file it is 0x00.
    byte 16+:   (optional when defined)

    and it is populated into mmgsdi_file_attributes_type

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
void mmgsdi_icc_rsp_extract_file_attr (
  mmgsdi_slot_id_enum_type       slot,
  uint8 *                        sim_header_attr,
  mmgsdi_len_type                sim_header_data_len,
  mmgsdi_file_attributes_type  * attr_ptr
)
{
  mmgsdi_file_status_type          *file_status_ptr = NULL;
  mmgsdi_file_security_access_type *file_security_access_ptr = NULL;

  if (attr_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null attr_ptr ");
    return;
  }

  /* Check that data pointer is valid and the length meets mandatory data
     length requirement as specified by TS 11.11 */
  if (sim_header_attr == NULL ||
      sim_header_data_len < MMGSDI_ICC_MANDATORY_FILE_ATTR_RESP_LEN)
  {
    UIM_MSG_ERR_1("Invalid file attributes resp data, length: 0x%x",
                  sim_header_data_len);
    return;
  }

  memset(attr_ptr, 0x00, int32touint32(sizeof(mmgsdi_file_attributes_type)) );

  attr_ptr->file_size = ((int32)sim_header_attr[2] << 8) | (int32)sim_header_attr[3];
  attr_ptr->file_id[0]= sim_header_attr[4];
  attr_ptr->file_id[1]= sim_header_attr[5];

  switch(sim_header_attr[6])
  {
    case 1: /* MASTER FILE */
      /* For master files, the first 22 bytes are mandatory as specified by TS 11.11 */
      if (sim_header_data_len < MMGSDI_ICC_MANDATORY_MF_DF_FILE_ATTR_RESP_LEN)
      {
        /* If data length is invalid, indicate error but continue populating attribute
           parameters as to not break communication with the card */
        UIM_MSG_ERR_1("Invalid MF file attr resp data length: 0x%x",
                      sim_header_data_len);
      }
      attr_ptr->file_type = MMGSDI_MASTER_FILE;
      break;
    case 2: /* DEDICATED FILE */
      /* For dedicated files, the first 22 bytes are mandatory as specified by TS 11.11 */
      if (sim_header_data_len < MMGSDI_ICC_MANDATORY_MF_DF_FILE_ATTR_RESP_LEN)
      {
        /* If data length is invalid, indicate error but continue populating attribute
           parameters as to not break communication with the card */
        UIM_MSG_ERR_1("Invalid DF file attr resp data length: 0x%x",
                      sim_header_data_len);
      }
      attr_ptr->file_type = MMGSDI_DEDICATED_FILE;
      break;
    case 4:
      switch(sim_header_attr[13])
      {
        case 0x03: /* CYCLIC FILE */
          /* For record based files, an extra byte is required to identify
             record length as specified by TS 11.11 */
          if (sim_header_data_len < (MMGSDI_ICC_MANDATORY_FILE_ATTR_RESP_LEN + 1))
          {
            UIM_MSG_ERR_1("Invalid record based resp data length: 0x%x",
                          sim_header_data_len);
            return;
          }
          attr_ptr->file_type = MMGSDI_CYCLIC_FILE;
          attr_ptr->file_info.cyclic_file.increase_allowed =
                                            ( sim_header_attr[7] & 0x40 ) / 0x40;

          file_status_ptr = &attr_ptr->file_info.cyclic_file.file_status;
          mmgsdi_populate_icc_file_status_info(slot,
                                              sim_header_attr,
                                              file_status_ptr);

          file_security_access_ptr = &attr_ptr->file_info.cyclic_file.file_security;
          mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi(sim_header_attr,
                                                          file_security_access_ptr);

          attr_ptr->file_info.cyclic_file.rec_len =  sim_header_attr[14];
          attr_ptr->file_info.cyclic_file.num_of_rec = 0;
          if (attr_ptr->file_info.cyclic_file.rec_len > 0)
          {
            attr_ptr->file_info.cyclic_file.num_of_rec =
              attr_ptr->file_size/attr_ptr->file_info.cyclic_file.rec_len;
          }
          break;

        case 0x01: /* LINEAR FILE */
          /* For record based files, an extra byte is required to identify
             record length as specified by TS 11.11 */
          if (sim_header_data_len < (MMGSDI_ICC_MANDATORY_FILE_ATTR_RESP_LEN + 1))
          {
            UIM_MSG_ERR_1("Invalid record based resp data length: 0x%x",
                          sim_header_data_len);
            return;
          }
          attr_ptr->file_type = MMGSDI_LINEAR_FIXED_FILE;

          file_status_ptr = &attr_ptr->file_info.linear_fixed_file.file_status;
          mmgsdi_populate_icc_file_status_info(slot,
                                               sim_header_attr,
                                               file_status_ptr);

          file_security_access_ptr = &attr_ptr->file_info.linear_fixed_file.file_security;
          mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi(sim_header_attr,
                                                          file_security_access_ptr);

          attr_ptr->file_info.linear_fixed_file.rec_len = sim_header_attr[14];
          attr_ptr->file_info.linear_fixed_file.num_of_rec = 0;
          if (attr_ptr->file_info.linear_fixed_file.rec_len > 0)
          {
            attr_ptr->file_info.linear_fixed_file.num_of_rec =
              attr_ptr->file_size/attr_ptr->file_info.linear_fixed_file.rec_len;
          }
          break;

        case 0x00: /* TRANSPARENT FILE */
          attr_ptr->file_type = MMGSDI_TRANSPARENT_FILE;

          file_status_ptr = &attr_ptr->file_info.transparent_file.file_status;
          mmgsdi_populate_icc_file_status_info(slot,
                                               sim_header_attr,
                                               file_status_ptr);

          file_security_access_ptr = &attr_ptr->file_info.transparent_file.file_security;
          mmgsdi_populate_icc_sec_attr_from_uim_to_mmgsdi(sim_header_attr,
                                                          file_security_access_ptr);
          break;
        default:
          UIM_MSG_ERR_1("Unable to determine file structure 0x%x",
                        sim_header_attr[13]);
      }
      break;
    default:
      attr_ptr->file_type = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
      break;
  }
} /* mmgsdi_icc_rsp_extract_file_attr *//*lint !e715 */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_READ

   DESCRIPTION:
     This function will build the response to Read request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_read (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_read_cnf_type                 ** mmgsdi_cnf_pptr,
  boolean                                 synch
)
{
  mmgsdi_request_header_type            req_header;
  int32                                 index                 = 0;
  mmgsdi_return_enum_type               mmgsdi_status         = MMGSDI_SUCCESS;
  mmgsdi_read_cnf_type                * cnf_ptr               = NULL;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr       = NULL;
  uim_cmd_type                        * uim_cmd_ptr           = NULL;
  mmgsdi_return_enum_type               report_status         = MMGSDI_SUCCESS;
  boolean                               sw_present            = FALSE;
  mmgsdi_data_type                      rsp_data_buf;
  mmgsdi_data_type                      cnf_data_buf;
  mmgsdi_offset_type                    mmgsdi_offset         = 0;
  boolean                               is_cache_init_read    = FALSE;


  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);
  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_cnf_pptr);
  }

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
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status == MMGSDI_SUCCESS)
  {
    if (extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
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
    if((extra_param_ptr->read_data.file_type == MMGSDI_LINEAR_FIXED_FILE)&&
       (extra_param_ptr->read_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)&&
       ((extra_param_ptr->read_data.access.file.file_enum == MMGSDI_GSM_OPL)||
        (extra_param_ptr->read_data.access.file.file_enum == MMGSDI_GSM_PNN)))
    {
      mmgsdi_status = mmgsdi_eons_opl_pnn_update_record(
                        req_header.session_id,
                        extra_param_ptr->read_data.access.file.file_enum,
                        rsp_data_buf,
                        (mmgsdi_rec_num_type)extra_param_ptr->read_data.rec_num,
                        MMGSDI_CACHE_MAX_ENUM);
    }
    /* Update the cache content if the EF is under Phonebook directory.
       Add record to the file cache list */
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
    else if((extra_param_ptr->read_data.file_type == MMGSDI_TRANSPARENT_FILE) ||
        (( (extra_param_ptr->read_data.file_type == MMGSDI_LINEAR_FIXED_FILE) ||
          (extra_param_ptr->read_data.file_type == MMGSDI_CYCLIC_FILE)) &&
        (extra_param_ptr->read_data.rec_num == 1)))
    {
      mmgsdi_data_from_enum_type data_from = MMGSDI_DATA_FROM_CARD_INCOMPLETE;

      if(extra_param_ptr->read_data.data_len == 0)
      {
        data_from = MMGSDI_DATA_FROM_CARD_COMPLETE;
      }
      /* If the length of the PRL file is greater than 2, it means the response
         contains the PRL size bytes + data. Hence, mark it as
         DATA_FROM_CARD_COMPLETE so that it gets written to the cache. */
      else if (extra_param_ptr->read_data.access.file.file_enum == MMGSDI_CDMA_PRL ||
               extra_param_ptr->read_data.access.file.file_enum == MMGSDI_CDMA_EPRL)
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

      mmgsdi_status = mmgsdi_cache_write(req_header.session_id,
                                         &extra_param_ptr->read_data.access,
                                         rsp_data_buf,
                                         mmgsdi_offset,
                                         data_from);
    }
    mmgsdi_status = MMGSDI_SUCCESS;
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
           ((extra_param_ptr->read_data.access.file.file_enum == MMGSDI_GSM_OPL)||
            (extra_param_ptr->read_data.access.file.file_enum == MMGSDI_GSM_PNN)))
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
      sw_present,/* return sw1 and sw2 for read */
      uim_rsp_ptr->status_word);
  }
  else
  {
    if (mmgsdi_cnf_pptr == NULL)
    {
      UIM_MSG_ERR_0("Null cnf Ptr");
      MMGSDIUTIL_TMC_MEM_FREE(rsp_data_buf.data_ptr);
      return MMGSDI_ERROR;
    }

    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*mmgsdi_cnf_pptr),
      sizeof(mmgsdi_read_cnf_type));
    if ((*mmgsdi_cnf_pptr) == NULL)
    {
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
                                          uim_rsp_ptr->status_word);

      cnf_ptr->response_header.mmgsdi_status = report_status;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(rsp_data_buf.data_ptr);
  return mmgsdi_status;
} /* mmgsdi_icc_rsp_read */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_SET_PIN_STATUS

   DESCRIPTION:
     This function sets the pin status for the current GSM/RUIM App

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_set_pin_status should be used

   RETURN VALUE:
   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
static void mmgsdi_icc_rsp_set_pin_status(
  mmgsdi_pin_enum_type        pin_id,
  mmgsdi_int_app_info_type   *sel_app_ptr,
  byte                        pin_status_offset,
  const mmgsdi_data_type     *add_data_ptr)
{
  mmgsdi_pin_status_enum_type         pin_status   = MMGSDI_PIN_ENABLED_VERIFIED;
  boolean                             pin_verified = TRUE;
  mmgsdi_app_pin_info_type           *pin_info_ptr = NULL;

  if(sel_app_ptr == NULL || add_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("Bad Parameters");
    return;
  }

  if(pin_id == MMGSDI_PIN1)
  {
    pin_info_ptr = sel_app_ptr->pin1_ptr;
  }
  else if( pin_id == MMGSDI_PIN2)
  {
    pin_info_ptr = &sel_app_ptr->pin2;
  }
  else
  {
    UIM_MSG_ERR_1("Invalid PIN 0x%x", pin_id);
    return;
  }

  if(pin_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Bad Parameters");
    return;
  }

  if ((pin_info_ptr->status == MMGSDI_PIN_ENABLED_NOT_VERIFIED) ||
      (pin_info_ptr->status == MMGSDI_PIN_STATUS_NOT_INITIALIZED))
    pin_verified = FALSE;

  /* --------------------------------------------------------------------------
     1. Check to see if the PIN is Initialized
     1.1 If the Permanently blocked, then set the status to PERM_BLOCKED.
     1.2 If the num Retries is zero, then the pin is blocked.
     1.3 if not and the disabled mask is set, pin is disabled
     1.4.Else PIn is Enabled and not Verified
     ------------------------------------------------------------------------*/
  if ( add_data_ptr->data_ptr[pin_status_offset] & GSDI_CODE_INIT_MASK )
  {
   if ( ( pin_id == MMGSDI_PIN1 ) &&
        ( add_data_ptr->data_ptr[GSDI_FILE_CHARAC_OFFSET] &
          GSDI_CHV1_DISABLED_MASK ) )
    {
      pin_status  = MMGSDI_PIN_DISABLED;
      UIM_MSG_HIGH_1("GSM PIN %x DISABLED", ((uint32)pin_id));
    }
    else
    {
      /* Case for PIN1 if Enabled */
      if (pin_verified)
        pin_status = MMGSDI_PIN_ENABLED_VERIFIED;
      else
        pin_status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
      UIM_MSG_HIGH_1("GSM PIN %x ENABLED", ((uint32)pin_id));
    }

    if ( pin_info_ptr->num_unblock_retries == 0)
    {
      pin_status   = MMGSDI_PIN_PERM_BLOCKED;
      UIM_MSG_HIGH_1("PIN 0x%x is perm blocked", pin_id);
    }
    else if ( pin_info_ptr->num_retries == 0 )
    {
      pin_status   = MMGSDI_PIN_BLOCKED;
      UIM_MSG_HIGH_1("PIN 0x%x is blocked", pin_id);
    }

  }
  else
  {
    /* Per spec 3GPP TS 11.11, if bit 8 of the CHV byte of the SELECT command
       response is not set then the pin_status is set to not initialized */
    pin_status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
  }

  pin_info_ptr->status = pin_status;
} /* mmgsdi_icc_rsp_set_pin_status */


/*===========================================================================
FUNCTION MMGSDI_ICC_PARSE_PIN_RSP

DESCRIPTION
  This function parses the pin rsp based on the uim rsp

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_icc_rsp_parse_pin(
  mmgsdi_return_enum_type            report_status,
  const mmgsdi_pin_extra_info_type  *pin_extra_info_ptr,
  const mmgsdi_data_type            *add_data_ptr,
  mmgsdi_session_id_type             session_id,
  mmgsdi_int_app_info_type          *sel_app_ptr,
  mmgsdi_slot_id_enum_type           slot_id)
{
  mmgsdi_app_pin_info_type  *pin_info_ptr      = NULL;
  uint32                     index             = 0;
  mmgsdi_app_pin_info_type **app_pin1_tbl_pptr = NULL;
  uint8                      app_pin1_tbl_size = 0;

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  if(pin_extra_info_ptr == NULL || add_data_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null input ptr: pin_extra_info_ptr 0x%x, add_data_ptr 0x%x",
                  pin_extra_info_ptr, add_data_ptr);
    return;
  }

  app_pin1_tbl_pptr = mmgsdi_util_get_app_pin1_info_tbl_ptr(&app_pin1_tbl_size,
                                                            slot_id);
  if(app_pin1_tbl_pptr == NULL ||
     app_pin1_tbl_size == 0)
  {
    UIM_MSG_ERR_0("Could not get PIN1 table");
    return;
  }

  if (pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN1)
  {
    pin_info_ptr   = sel_app_ptr->pin1_ptr;
    if(pin_info_ptr == NULL)
    {
      /*Select the First Index by Default*/
      sel_app_ptr->pin1_ptr = app_pin1_tbl_pptr[index];
      sel_app_ptr->pin1_ptr->key_ref = MMGSDI_CHV1_KEY;
      pin_info_ptr = sel_app_ptr->pin1_ptr;

      sel_app_ptr->pin2.key_ref = MMGSDI_CHV2_KEY;
    }
  }
  else if (pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN2){
    pin_info_ptr   = &sel_app_ptr->pin2;
    pin_info_ptr->key_ref = MMGSDI_CHV2_KEY;
  }
  else
  {
    UIM_MSG_ERR_1("Invalid PIN ID 0x%x", pin_extra_info_ptr->pin_info.pin_id );
    return;
  }

  pin_info_ptr->pin_id = pin_extra_info_ptr->pin_info.pin_id;
  pin_info_ptr->valid_num_retries = TRUE;
  pin_info_ptr->valid_num_unblock_retries = TRUE;

  switch(report_status)
  {
    case MMGSDI_SUCCESS:
      if(pin_extra_info_ptr->retry_type == MMGSDI_GET_NUM_RETRIES)
      {
        /* Get number of retry left for PINs and PUKs */
        sel_app_ptr->pin1_ptr->num_retries          =
          add_data_ptr->data_ptr[GSDI_PIN1_STATUS_OFFSET] & 0x0F;
        sel_app_ptr->pin1_ptr->num_unblock_retries  =
          add_data_ptr->data_ptr[GSDI_PUK1_STATUS_OFFSET] & 0x0F;
        sel_app_ptr->pin2.num_retries               =
          add_data_ptr->data_ptr[GSDI_PIN2_STATUS_OFFSET] & 0x0F;
        sel_app_ptr->pin2.num_unblock_retries       =
          add_data_ptr->data_ptr[GSDI_PUK2_STATUS_OFFSET] & 0x0F;
        sel_app_ptr->pin2.valid_num_retries         = TRUE;
        sel_app_ptr->pin2.valid_num_unblock_retries = TRUE;

        mmgsdi_icc_rsp_set_pin_status(MMGSDI_PIN1,sel_app_ptr,GSDI_PIN1_STATUS_OFFSET,
                                      add_data_ptr);
        mmgsdi_icc_rsp_set_pin_status(MMGSDI_PIN2,sel_app_ptr,GSDI_PIN2_STATUS_OFFSET,
                                      add_data_ptr);
      }
      /*If the PIN already exists, then change the valid FLAG TO TRUE*/
      if(pin_info_ptr->pin_data.data_len != 0)
      {
         pin_info_ptr->valid_pin = TRUE;
      }
      break;

    case MMGSDI_PIN_NOT_INITIALIZED:
    case MMGSDI_INCOMPAT_PIN_STATUS:
    case MMGSDI_INCORRECT_CODE:
      break;

    case MMGSDI_CODE_BLOCKED:
      pin_info_ptr->status       = MMGSDI_PIN_BLOCKED;
      pin_info_ptr->num_retries  = 0;
      break;

    case MMGSDI_CODE_PERM_BLOCKED:
      pin_info_ptr->status               = MMGSDI_PIN_PERM_BLOCKED;
      pin_info_ptr->num_unblock_retries  = 0;
      break;

    default:
      UIM_MSG_ERR_1("Unhandled pin status 0x%x", report_status);
      pin_info_ptr->valid_num_retries = FALSE;
      pin_info_ptr->valid_num_unblock_retries = FALSE;
      return;
   } /* Parsing UIM Status */

   /*-------------------------------------------------------------------------------------------------
     If the PIN Is set to ENABLED NOT VERIFIED, and the operation was a Succeful Verification,
     Succeful Unblock, or A Successful Change then change the state to ENABLED_VERIFIED
     -------------------------------------------------------------------------------------------------*/
   if((report_status == MMGSDI_SUCCESS) && (pin_extra_info_ptr->retry_type == MMGSDI_RETRY_NONE))
   {
     if(pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN1 &&
         sel_app_ptr->pin1_ptr != NULL)
     {
       if((pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_VERIFY ) ||
         (pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_UNBLOCK ) ||
         (pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_CHANGE  ) ||
         (pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_ENABLE  ))
       {
         sel_app_ptr->pin1_ptr->status = MMGSDI_PIN_ENABLED_VERIFIED;
       }
       else if(pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_DISABLE)
       {
         sel_app_ptr->pin1_ptr->status = MMGSDI_PIN_DISABLED;
       }
     }
     else if((pin_extra_info_ptr->pin_info.pin_id == MMGSDI_PIN2) &&
             ((pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_VERIFY ) ||
              (pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_UNBLOCK)||
              (pin_extra_info_ptr->orig_pin_op == MMGSDI_PIN_OP_CHANGE )))
     {
       sel_app_ptr->pin2.status = MMGSDI_PIN_ENABLED_VERIFIED;
     }
   }
} /* mmgsdi_icc_rsp_parse_pin*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_GET_FILE_ATTR

   DESCRIPTION:
     This function will build the response to Get File Attribute request

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function

   LIMITATIONS:
     This function is limited to the use of ICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_get_file_attr(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_get_file_attr_cnf_type        ** attr_cnf_pptr,
  boolean                                 synch
)
{
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status      = MMGSDI_SUCCESS;
  int32                              index              = 0;
  mmgsdi_client_req_extra_info_type *extra_param_ptr    = NULL;
  uim_cmd_type                      *uim_cmd_ptr        = NULL;
  mmgsdi_return_enum_type            report_status      = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type          *sel_app_ptr        = NULL;
  mmgsdi_get_file_attr_cnf_type     *cnf_ptr            = NULL;
  mmgsdi_pin_extra_info_type        *pin_extra_info_ptr = NULL;

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

  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  UIM_MSG_HIGH_1("Select status 0x%x", report_status);

  if (report_status == MMGSDI_UIM_CMD_TIMEOUT &&
      req_header.orig_request_type == MMGSDI_GET_FILE_ATTR_REQ)
  {
    /* Update cache to indicate that file was not found on card */
    (void)mmgsdi_cache_write(req_header.session_id,
                             &extra_param_ptr->get_file_attr_data.access,
                             uim_rsp_ptr->rsp_data,
                             0,
                             MMGSDI_DATA_NOT_FOUND);

    UIM_MSG_HIGH_0("Marked cache to MMGSDI_CACHE_NOT_FOUND due to MMGSDI_UIM_CMD_TIMEOUT");
    return MMGSDI_UIM_CMD_TIMEOUT;
  }

  if(req_header.orig_request_type == MMGSDI_PIN_OPERATION_REQ ||
     req_header.orig_request_type == MMGSDI_GET_ALL_PIN_STATUS_REQ)
  {
    mmgsdi_status = mmgsdi_util_get_prov_session_info(
                                     req_header.session_id,
                                     NULL,NULL,
                                     &sel_app_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS || sel_app_ptr == NULL)
    {
      return mmgsdi_status;
    }

    /* Retrieve the PIN operation info */
    pin_extra_info_ptr =
      (mmgsdi_pin_extra_info_type*)extra_param_ptr->get_file_attr_data.int_client_data;

    mmgsdi_icc_rsp_parse_pin(report_status,
                             pin_extra_info_ptr,
                             (mmgsdi_data_type*)&uim_rsp_ptr->rsp_data,
                             req_header.session_id,
                             sel_app_ptr,
                             req_header.slot_id);

    if(sel_app_ptr->pin1_ptr == NULL)
    {
      UIM_MSG_ERR_0("ICC RSP - PIN1 Null");
      mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                                 &req_header,
                                                 extra_param_ptr,
                                                 (void*)&uim_rsp_ptr->rsp_data,
                                                 uim_rsp_ptr->cmd_transacted,
                                                 uim_rsp_ptr->status_word);
      return mmgsdi_status;

    }
    /*Set all flags to cached*/
    sel_app_ptr->pin1_ptr->cached = TRUE;
    sel_app_ptr->pin2.cached  = TRUE;

  }
  if (!synch)
  {
    /* For PIN operations, retrieve the original PIN operation status
       from the PIN info if the SELECT operation was successful */
    if((pin_extra_info_ptr != NULL) &&
       (req_header.orig_request_type == MMGSDI_PIN_OPERATION_REQ) &&
       (report_status == MMGSDI_SUCCESS))
    {
      report_status = pin_extra_info_ptr->pin1_report_status;
    }

    mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                        &req_header,
                                        extra_param_ptr,
                                        (void*)&uim_rsp_ptr->rsp_data,
                                        uim_rsp_ptr->cmd_transacted,
                                        uim_rsp_ptr->status_word);
  }
  else
  {
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

  UIM_MSG_HIGH_2("ICC RSP - GET_FILE_ATTR status is 0x%x for file 0x%x",
                 report_status,
                 extra_param_ptr->get_file_attr_data.access.file.file_enum);
  return mmgsdi_status;
} /* mmgsdi_icc_rsp_get_file_attr */


/* Needs to evaluate if whole file can be featurized */


/*----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_SEND_APDU

   DESCRIPTION:
     This function will build the response to SEND_APDU request

   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_rsp_send_apdu should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_rsp_send_apdu(
  const mmgsdi_uim_report_rsp_type* uim_rsp_ptr)
{
  mmgsdi_request_header_type         req_header;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  int32                              index            = 0;
  uim_cmd_type*                      uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type* extra_param_ptr  = NULL;
  mmgsdi_return_enum_type            report_status    = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_0("MMGSDI_ICC_RSP_SEND_APDU");
  /*
   * Check input pointers.
   */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);
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
                                             FALSE, /* status word not required
                                                       for send apdu */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_send_apdu*/ /*lint !e715 */


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_STATUS

   DESCRIPTION:
     This function will build the response to the Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{

  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;
  mmgsdi_return_enum_type             report_status    = MMGSDI_SUCCESS;

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
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

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

  mmgsdi_status = mmgsdi_cnf_build_and_queue(report_status,
                                             &req_header,
                                             extra_param_ptr,
                                             (void*)&uim_rsp_ptr->rsp_data,
                                             uim_rsp_ptr->cmd_transacted,
                                               /* status word required for status cmd */
                                             uim_rsp_ptr->status_word);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_status */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_REHABILITATE

   DESCRIPTION:
     This function will build the response to Rehabilitate request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_rehabilitate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_rehabitate (
  const mmgsdi_uim_report_rsp_type      *  uim_rsp_ptr,
  mmgsdi_rehabilitate_cnf_type          ** rehab_cnf_pptr,
  boolean                                  synch
)
{
  mmgsdi_request_header_type            req_header;
  int32                                 index            = 0;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr  = NULL;
  uim_cmd_type                        * uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type               report_status    = MMGSDI_SUCCESS;
  mmgsdi_rehabilitate_cnf_type        * cnf_ptr          = NULL;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if (synch)
  {
    MMGSDIUTIL_RETURN_IF_NULL(rehab_cnf_pptr);
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

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* unsuccessful processing of command */
    if (extra_param_ptr->rehab_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      UIM_MSG_HIGH_1("UIM_FAIL For MMGSDI EF %x",
                     extra_param_ptr->rehab_data.access.file.file_enum);
    }
    UIM_MSG_HIGH_3("UIM_FAIL sw1 %x sw2 %x status 0x%x",
                   uim_rsp_ptr->status_word.sw1,
                   uim_rsp_ptr->status_word.sw2,
                   report_status);
  } /* Parsing UIM Status */

  if (!synch)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(
        report_status,
        &req_header,
        extra_param_ptr,
        (void*)&uim_rsp_ptr->rsp_data,
        FALSE, /* status word not required for rehabilitate cmd */
        uim_rsp_ptr->status_word);
  }
  else
  {
    /* Obtain memory for confirmation data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*rehab_cnf_pptr),
      sizeof(mmgsdi_rehabilitate_cnf_type));
    if ((*rehab_cnf_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    cnf_ptr = *rehab_cnf_pptr;
    mmgsdi_status = mmgsdi_cnf_build_rehabilitate_data(cnf_ptr,
                                                       extra_param_ptr);
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
                                          MMGSDI_REHABILITATE_CNF,
                                          FALSE, /* sw not required for
                                                    rehabilitate cmd */
                                          uim_rsp_ptr->status_word);
      cnf_ptr->response_header.mmgsdi_status = report_status;
    } /* mmgsdi_cnf_build_rehabilitate_data status check */
  } /* synch process */


  UIM_MSG_HIGH_2("ICC RSP - REHABILITATE status is 0x%x for file 0x%x",
                 report_status, extra_param_ptr->rehab_data.access.file.file_enum);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_rehabitate */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_INVALIDATE

   DESCRIPTION:
     This function will build the response to Invalidate request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_invalidate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_invalidate (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type            req_header;
  int32                                 index            = 0;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type   * extra_param_ptr  = NULL;
  uim_cmd_type                        * uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type               report_status    = MMGSDI_SUCCESS;

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

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type,
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* unsuccessful processing of command */
    if (extra_param_ptr->invalidate_data.access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      UIM_MSG_HIGH_1("UIM_FAIL For MMGSDI EF %x",
                     extra_param_ptr->invalidate_data.access.file.file_enum);
    }
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

  UIM_MSG_HIGH_2("ICC RSP - INVALIDATE status is 0x%x for file 0x%x",
             report_status,
             extra_param_ptr->invalidate_data.access.file.file_enum);

  return mmgsdi_status;
} /* mmgsdi_icc_rsp_invalidate */


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_PIN_OPERATION

   DESCRIPTION:
     This function will build the response to the PIN Operation Command

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_rsp_pin_operation should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_pin_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{

  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                       *uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type  *extra_param_ptr       = NULL;
  mmgsdi_return_enum_type             report_status         = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type           *sel_app_ptr           = NULL;
  mmgsdi_access_type                  access_type;
  mmgsdi_pin_extra_info_type         *pin_info_ptr          = NULL;

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

  if (mmgsdi_status != MMGSDI_SUCCESS || (extra_param_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                                   req_header.session_id,
                                   NULL,NULL,
                                   &sel_app_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS || sel_app_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /*---------------------------------------------------------------------------
    1) Parse the response status returned from UIM
    2) If success:
         update record number accessed in confirmation type
  ---------------------------------------------------------------------------*/
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    /* ------------------------------------------------------------------------
       Command processing returned Status words other than 0x90 0x00
       ----------------------------------------------------------------------*/
   MMGSDIUTIL_DEBUG_MSG_ERROR_3("UIM_FAIL sw1:0x%x sw2:%x status:0x%x",
                                uim_rsp_ptr->status_word.sw1,
                                uim_rsp_ptr->status_word.sw2,
                                report_status);
  }

  /*Parse the Response from UIM and the Set the Pin Status and NUM Retries
    accordingly*/
  mmgsdi_icc_rsp_parse_pin(report_status,
                           &extra_param_ptr->pin_data,
                           (mmgsdi_data_type*)&uim_rsp_ptr->rsp_data,
                           req_header.session_id,
                           sel_app_ptr,
                           req_header.slot_id);

  /*Copy the information back to extra param data*/
  if ( extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN1 )
  {
    mmgsdi_memscpy(&extra_param_ptr->pin_data.pin_info,
                   sizeof(extra_param_ptr->pin_data.pin_info),
                   sel_app_ptr->pin1_ptr,
                   sizeof(mmgsdi_app_pin_info_type));
  }
  else if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_PIN2)
  {
    mmgsdi_memscpy(&extra_param_ptr->pin_data.pin_info,
                   sizeof(extra_param_ptr->pin_data.pin_info),
                   &sel_app_ptr->pin2,
                   sizeof(mmgsdi_app_pin_info_type));
  }

  if (report_status == MMGSDI_SUCCESS)
  {
    /* Since the PIN operation is successful, store the PIN value */
    mmgsdi_util_store_pin(sel_app_ptr,
                          extra_param_ptr->pin_data.pin_info.pin_id,
                          extra_param_ptr->pin_data.pin_data);
  }

  /*----------------------------------------------------------------------------------
   Now get the Status and the Number of Retries for the PINs by doing a Select
   on MMGSDI_GSM_DIR
   ----------------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(pin_info_ptr,
    sizeof(mmgsdi_pin_extra_info_type));
  if (pin_info_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* Store the PIN operation status (to be used to build the
     confirmation later) */
  extra_param_ptr->pin_data.pin1_report_status = report_status;

  extra_param_ptr->pin_data.retry_type         = MMGSDI_GET_NUM_RETRIES;
  mmgsdi_memscpy(pin_info_ptr,
                 sizeof(mmgsdi_pin_extra_info_type),
                 &extra_param_ptr->pin_data,
                 sizeof(mmgsdi_pin_extra_info_type));

  access_type.access_method     = MMGSDI_DF_ENUM_ACCESS;
  if(sel_app_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    access_type.file.df_enum      = MMGSDI_GSM_DIR;
  }
  else if(sel_app_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    access_type.file.df_enum      = MMGSDI_RUIM_DIR;
  }

  /*Get the Number of Retries*/
  mmgsdi_status = mmgsdi_util_build_get_file_attr_req(&req_header,
                                                      access_type,
                                                      pin_info_ptr);

  /* pin_info_ptr is already free'd when processing of get_file_attr above
     fails - so, do not free it in case of error status below */
  pin_info_ptr = NULL;

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("build get file attr req status is 0x%x", mmgsdi_status);

    if(report_status != MMGSDI_SUCCESS)
    {
      mmgsdi_status = report_status;
    }
    else
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
    mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                               &req_header,
                                               extra_param_ptr,
                                               (void*)&uim_rsp_ptr->rsp_data,
                                               uim_rsp_ptr->cmd_transacted,
                                               uim_rsp_ptr->status_word);

    UIM_MSG_HIGH_3("ICC RSP - PIN OPERATION status is 0x%x for PIN_OP 0x%x,PIN_STATUS = 0x%x",
                   report_status,
                   extra_param_ptr->pin_data.orig_pin_op,
                   extra_param_ptr->pin_data.pin_info.status);

  }
  return mmgsdi_status;
} /* mmgsdi_icc_rsp_pin_operation*/


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_PIN_STATUS

   DESCRIPTION:
     This function will build the response to the PIN Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_pin_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_pin_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type          req_header;
  int32                               index                 = 0;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr           = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr       = NULL;
  mmgsdi_pin_extra_info_type        * pin_info_ptr          = NULL;
  mmgsdi_access_type                  access_type;
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

    UIM_MSG_HIGH_1("ICC RSP - PIN STATUS  Internal failure (mmgsdi_error_status 0x%x)",
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
                                               FALSE, /* status word not required
                                                         for pin status */
                                               uim_rsp_ptr->status_word);

    return mmgsdi_status;
  }

  /* Now do a Select on MMGSM_DIR to get the number of retries and number of
     unblock retries*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(pin_info_ptr,
    sizeof(mmgsdi_pin_extra_info_type));
  if (pin_info_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*PIN ID = 1*/
  pin_info_ptr->orig_pin_op     = MMGSDI_PIN_OP_NONE;
  pin_info_ptr->pin_info.pin_id = MMGSDI_PIN1;
  pin_info_ptr->retry_type      = MMGSDI_GET_NUM_RETRIES;
  pin_info_ptr->pin_info.status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
  access_type.access_method     = MMGSDI_DF_ENUM_ACCESS;

  if(app_info_ptr->app_data.app_type == MMGSDI_APP_SIM)
  {
    access_type.file.df_enum      = MMGSDI_GSM_DIR;
  }
  else if(app_info_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    access_type.file.df_enum      = MMGSDI_RUIM_DIR;
  }
  mmgsdi_status = mmgsdi_util_build_get_file_attr_req(&req_header,
                                                      access_type,
                                                      pin_info_ptr);

  /*PIN Info Ptr would get freed in mmgsdi_util_free_client_request_table_index
    in case of error... so, no need to free it again in case of error status below*/
  pin_info_ptr = NULL;

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                               &req_header,
                                               extra_param_ptr,
                                               (void*)&uim_rsp_ptr->rsp_data,
                                               FALSE, /* status word not required
                                                         because of mmgsdi internal
                                                         failure */
                                               uim_rsp_ptr->status_word);

    if (extra_param_ptr != NULL)
    {
      UIM_MSG_HIGH_2("For PIN_OP 0x%x,PIN_STATUS = 0x%x",
                     extra_param_ptr->pin_data.orig_pin_op,
                     extra_param_ptr->pin_data.pin_info.status);
    }
  }

  return mmgsdi_status;

} /* mmgsdi_icc_rsp_pin_status*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_RUN_GSM_ALGO

   DESCRIPTION:
     This function will build the response to RUN GSM algorithm

   DEPENDENCIES:
     None

   LIMITATIONS:
     This function is limited to the use of CDMA ICC cards

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_run_gsm_algo (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr
)
{
#ifdef FEATURE_MMGSDI_3GPP
  mmgsdi_request_header_type          req_header       = {0};
  int32                               index            = 0;
  mmgsdi_return_enum_type             mmgsdi_status    = MMGSDI_SUCCESS;
  uim_cmd_type                      * uim_cmd_ptr      = NULL;
  mmgsdi_return_enum_type             report_status    = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type * extra_param_ptr  = NULL;

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

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO - 1);

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
  report_status = mmgsdi_icc_rsp_parse_uim_response(uim_rsp_ptr);

  if (report_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_3("UIM_FAIL sw1 0x%x sw2 0x%x status 0x%x",
                  uim_rsp_ptr->status_word.sw1,
                  uim_rsp_ptr->status_word.sw2,
                  report_status);
  } /* Parsing UIM Status */

  mmgsdi_status = mmgsdi_cnf_build_and_queue(
      report_status,
      &req_header,
      extra_param_ptr,
      (void*)&uim_rsp_ptr->rsp_data,
      uim_rsp_ptr->cmd_transacted,
      uim_rsp_ptr->status_word);

  return mmgsdi_status;
#else
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP */
} /* mmgsdi_icc_rsp_run_gsm_algo */

