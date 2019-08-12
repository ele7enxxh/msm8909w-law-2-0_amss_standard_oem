/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I   L I B R A R Y


GENERAL DESCRIPTION

  This file contains Library function that MMGSDI client can call to access
  various SIM/USIM/RUIM functions

  mmgsdi_read_transparent
    Read data from a Transparent file

  mmgsdi_read_record
    Read data from a record in a Linear Fixed or Cyclic file

  mmgsdi_write_transparent
    Write data into a Transparent file

  mmgsdi_write_record
    Write data into a record Linear Fixed or Cyclic file

  mmgsdi_get_file_attr
    Get File Attribute information for the files

  mmgsdi_send_card_status
    Send a Status Command to the card for Detection Purposes

  mmgsdi_verify_pin
    Verify the PIN with the card

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdilib.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
06/07/16   ar      Removing string prints from APDU and AUTH
05/24/16   bcho    EONS F3 reduction
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/03/16   tkl     F3 log prints cleanup
04/11/16   av      Introduce LPA support
11/26/15   sp      write back EF-TST data to card from cache after recovery
10/26/15   nr      Disable MMGSDI PB cache for PBM module
10/12/15   vdc     Add support for potential silent file change notification
09/25/15   vr      Added support for ERA-GLONASS
06/25/15   hh      Initialze .do_not_recover as TRUE for all channels
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
05/14/15   ar      Correcting the check of incoming SAP APDU data length
03/25/15   stv     API to block card status events
12/15/14   kk      Fix MCFG UIM data sent for nv refresh
11/14/14   hh      Fix compilation warnings
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
10/29/14   am      Handle MMGSDI_NV_PREF_MODE_NOT_FROM_RUIM_I as device specific
10/10/14   vdc     Expose NV for CDMA select before reading ADN under TELECOM
09/09/14   kk      Enhancements to NV handling
09/30/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
09/27/14   ar      Add support for multiple slots in BT-SAP
09/22/14   ar      Correct F3 msg for mmgsdi_card_pdown()
08/27/14   yt      Support for restricted SIM access via MMGSDI session APIs
08/14/14   tl      Add support for silent PIN verification
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/31/14   am      Corrected use of UIM Legacy Macros
07/30/14   vv      Added support to indicate app is initialized in recovery
07/30/14   ar      Revert FR 20469 changes to align with NAS check-in dates
07/25/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/15/14   yt      Support for NV refresh
06/05/14   at      Critical KW P1 fix
03/04/14   yt      Check slot id of provisioning session during recovery
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/19/14   am      Added logic for clearing timers and queues
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
02/10/14   df      Update comments for client dereg API
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/24/13   spo     Fix to handle sap requests only on slot 1
12/04/13   spo     Do not set signals when MMGSDI task is already stopped
11/27/13   tl      Introduce refresh command queue
10/30/13   am      Acquire APP crit sect prior to slot data ptr access
10/21/13   yt      Add error checking for command APDUs
10/17/13   at      Support for MMGSDI Cancel APIs
10/16/13   tl      Remove MMGSDI_FEATURE_SAP
10/08/13   ar      Support to select CDMA before TP by default
10/07/13   av      Enter crit sect in card_pdown API before accessing global
10/04/13   vdc     F3 message reduction
10/04/13   vv      Added API to return se13 table release date and source
10/04/13   vv      Added NV support for ENS sub-features
10/03/13   yt      Multi-SIM support for OnChip
10/03/13   yt      Remove duplicate code
10/01/13   av      Reject power-down/up with new error code in ONCHIP mode
09/27/13   am      Prevent potential overflow of var_len
09/23/13   df      Support for SIM busy
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/20/13   ar      Fix to get client task name.
07/09/13   vdc     Fix and code optimization for get available apps
06/28/13   bcho    APIs added to return plmn info from SE13 table.
06/24/13   vdc     Added support for triple SIM
06/12/13   vdc     Fix compilation warning & critical section problem
05/31/13   js      Avoid not releasing critical section.
05/31/13   spo     Fixed logic for get_all_available_apps()
05/17/13   hn      Support for Alpha for Refresh
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
04/25/13   vv      Remove the legacy hotswap function
04/24/13   yt      Add NV_GPRS_ANITE_GCF_I to MMGSDI NV cache
04/17/13   tl      Replace Featurization with NV
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
04/11/13   vv      Re-design handling of VPE response from the card
03/05/13   tl      Use internal client id for activate OnChip when invalid
03/04/13   av      Retrieve ICCID from link_established and cache it
02/20/13   at      Added NV_FTM_MODE_I while fetching the cached items
02/14/13   tl      Move QMI UIM to MMGSDI context from QMI modem context
02/12/13   vv      Reject on chip activate request for slots other than slot-1
01/28/13   vv      Fix compiler warnings
01/08/13   vv      Fix to retrieve recovery data for each slot supported
01/03/13   abg     Added MMGSDI_QMI_GET_NV_ITEM_FROM_CACHE API
12/26/12   av      Do not allow writing to file that is being refreshed
12/15/12   bcho    Use select rsp type in recovery which is used during initial
                   app activation
12/05/12   vv      Support for parallel processing in MMGSDI
10/03/12   abg     Removed Queuing related F3 message
10/01/12   abg     Removed Problem determining protocol type F3 message
09/20/12   bcho    Added support for mmgsdi_get_SE13_plmn_names API
09/05/12   av      Handling of VPE response from the card
08/29/12   at      Support for new API mmgsdi_sap_connect_ext
08/24/12   abg     Added card slot validity check in Card Power Down API
08/10/12   av      Removed RPC support along with RPC style comments
08/07/12   tl      Added definition of mmgsdi_get_protocol
07/25/12   bcho    Add support for recovery of non prov sessions
07/13/12   vv      Added support to identify apps not in EF-DIR
04/27/12   tl      Reverted support for client name for rcinit framework
04/16/12   tl      Added support for client name traces
03/29/12   shr     Added support for non-prov. app selection using zero
                   length and partial AID, support for Send APDU extension API,
                   remove UIM TCB check in Notify Link Est API - API can be
                   invoked from either UIM or Virtuim
03/16/12   yt      Modify references to uim_tcb
03/07/12   bcho    NULL check removed for mmgsdi_channel_info_table_ptr
03/02/12   vv      Added esn_me_change_flag support on slot2
03/01/12   bcho    Function definition comment block updated
02/27/12   shr     Added support for Send APDU extension API
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
01/14/12   bcho    Check for negative len added in activate onchip sim/usim APIs.
                   Changes in mmgsdi_get_all_available_apps_sync to provide pin
                   info only when it is cached
01/05/12   shr     Removed F3 logging from within critical sections
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   bcho    Legacy GSDI removal updates
11/30/11   vv      Set valid_num_retries and valid_num_unblock_retries values
11/15/11   nmb     RPM Updates
10/13/11   shr     Adding support for full recovery
09/23/11   nb      Async TP enhancement
09/16/11   av      Replace feature #ifdefs with an NVITEM
08/18/11   av      Fixes for MMGSDI Klocwork CRITICAL warnings
05/17/11   shr     Updates for Thread safety
05/11/11   nb      Support for Async TP
04/28/11   nk      Fixed compiler warning mmgsdi_client_id_reg_table will
                   always be true
04/11/11   vs      Updated APDU max length check
04/05/11   ea      moved mmgsdi_util_determine_app_type to mmgsdiutil module
02/11/11   ea      Added check to see if slot is present before sending pup/pdown.
02/03/11   ms      Updated F3 messages for Client ID
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
01/12/11   yt      Fixed compiler warnings
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
10/26/10   ms      Fixed compilation issues
10/19/10   js      Fixed session management open for event callback
10/05/10   nmb     Removed extra bytes from task_cmd buffer
09/29/10   ssr     Added support of both client id and session id for refresh
09/15/10   nmb     Allocate mmgsdi_client_id_reg_table dynamically
08/18/10   mib     Fixed Lint warnings
07/08/10   js      Fixed to compiler warnings
07/02/10   ssr     Fixed refresh for ICC card which have both GSM and RUIM
06/28/10   rm      Fixes to remove INTLOCKs
06/24/10   yt      Need to get free index before accessing mmgsdilib_callback_table
05/05/10   nmb     Fixed ICC client callback for BDN service changes
04/27/10   shr     Added Hiddenkey support
04/13/10   ssr     Fixed get ruim capability
04/06/10   shr     Clean up REFRESH RESET and REFRESH RESET_AUTO
04/07/10   yt      Fixed compilation warnings
04/07/10   nb      SAP Enhancements
03/31/10   yt      Added FEATURE_MMGSDI_OP_MODE_SLAVE to support Fusion APIs
03/30/10   kk      Fixed client based refresh registeration
03/12/10   adp     Fixing KW errors
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
02/17/10   shr     Fixed Session activate/deactivate handling
02/09/10   vs      Fixed available apps logic
01/16/10   vs      Fixed crash caused by failed refresh registration
01/13/10   nb      Added definition for mmgsdi_perso_get_indicators.
01/06/10   mib     Fixed possible race condition in mmgsdilib
01/06/10   shr     Fixed Session Close handling
01/06/10   kk      Fixed Get all available cnf building
01/04/10   nb      Removed definition for session based APIs
12/18/09   nb      Moved Dual Slot Changes
12/16/09   nb      Lint Fixes
12/14/09   shr     Fixed Perso handling and APIs
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
11/02/09   nb      Mergerd Sesson Based APIs
10/28/09   nb      Release 7 Enhanced Network Selection
09/26/09   kp      ZI memory reduction changes
09/08/09   rn      Fixed klocwork errors
07/28/09   mib     Added mmgsdi_perso_lock_down_and_enable_limited_access
07/28/09   shr     Fixed Perso set data crash
07/27/09   kk      Fixed Lint Error - Boolean within 'if' always evaluates
                   to False
07/20/09   mib     Fixed lint warning due to mmgsdi_perso_operation not used
06/11/09   sun     Added missing feature
05/15/09   nb      Added support for mmgsdi_perso_get_unblock_dck_retries command
05/06/09   nb      Removed unused parameter from
                   mmgsdi_session_perso_get_dck_retries api
04/29/09   js      Fixed compiler warning
03/26/09   mib     Integration of support for ONCHIP USIM
03/19/09   js      Support for Enhanced NAA Refresh by file path
02/24/09   nb      Perso Command Support in MMGSDI
01/21/09   kk      Updated length check for read_record API
12/17/08   nb      Common Modem Interface
11/19/08   js      Fixed lint errors
09/25/08   js      Fixed Klockwork errors
09/22/08   js      Removed MFLO featurization in mmgsdi_send_apdu
07/20/08   jk      Support for Generate Key API
07/30/08   SSR     Update search for blocking the search directions belonging
                   to Record Pointer.
07/17/08   kk      Added support for PKCS 15
06/23/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/21/08   kk      Added PKCS15 Support
03/03/08   sun     Added support for OMA Bcast
01/10/08   nk      Fixed lint error, memcpy of esn to qw_equ of esn
11/15/07   sun     Added support for NAA refresh
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO support
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
08/14/07   sun     Added MUK_DEL Context
07/02/07   sun     Added Comments for usage
06/01/07   sun     Fixed Featurization for SAP
05/22/07   nk      Added support for ESN ME change and removed featurization
                   for uniform rpc support
05/17/07   wli     Added support for JCDMA RUIM
05/02/07   sun     Added support for Service Available and USIM Auth
04/03/07   sp      Send status command before termuinating an app.
03/26/07   tml     Fixed lint error
03/19/07   sun     Fixed Lint Error
03/07/07   sun     Added support for Disable with Replacement
03/06/07   nk      Added PIN length check for minimum acceptable length
01/13/07   tml     Add path support for Select
12/19/06   tml     Add support for path transparent and record access.  Add
                   support for cyclic and linear fixed write
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app.  Allow session
                   management to use the mmgsdi_session_build_select_aid api
08/25/06   sun     Fixed slot for Refresh
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     lint
05/23/06   tml     BDN support
04/15/06   tml     Do not copy if length of onchip data is 0
04/13/06   jar     Added ONCHIP SIM Support
01/29/06   tml     Moved mmgsdi_sim_refresh to mmgsdi.c
01/17/05   tml     MMGSDI cleanup
01/04/06   tml     Updated enum name for SAP and Card powerup and down
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
11/03/05   sun     Added support for Refresh Command
10/28/05   sun     Fixed Orig Request Types
10/26/05   pv      Allow reads and writes of more than 512 bytes
10/20/05   sun     Added support for BT Deregisteration
09/29/05   tml     Greater than 512 byte Cache Read and client dereg supports
08/30/05   jar     Set the client id before sending to GSDI in Card Status API
08/30/05   jar     Added support for gsdi_send_card_status()
08/27/05   tml     Compilation fix
08/26/05   sun     Added support for BT SAP
08/25/05   pv      API to support CARD PDOWN and PUP
07/26/05   sst     Added MMGSDIBT support
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
02/24/05   sk      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "intconv.h"
#include "mmgsdilib.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_session.h"
#include "uim.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_refresh.h"
#include "mmgsdilib_p.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_se13.h"
#include "mmgsdi_file.h"
#include "mmgsdi_crsm.h"
#include "mmgsdi_caller_context.h"
#include "qmi_uim.h"
#include "mmgsdi_manage_channel.h"
#include "mmgsdi_multi_profile.h"
#include "mmgsdicache.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/
/*===========================================================================
FUNCTION MMGSDI_SAP_CONNECT_CONNECTION

DESCRIPTION
  This function attempts to perform a SAP connect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  condition:       Condition to check for active call status.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achieved by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_connect_operation(
    mmgsdi_client_id_type                 client_id,
    mmgsdi_slot_id_enum_type              card_slot,
    mmgsdi_sap_mode_enum_type             sap_mode,
    mmgsdi_sap_conn_condition_enum_type   condition,
    mmgsdi_callback_type                  response_cb_ptr,
    mmgsdi_client_data_type               client_ref)
{
  mmgsdi_task_cmd_type        *task_cmd_ptr  = NULL;
  mmgsdi_sap_connect_req_type* msg_ptr       = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;
  int32                        total_mem_len = 0;
  uint32                       task_cmd_len  = 0;

  /* Input Parameters Check.
   */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SAP_CONNECT_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_connect, unsupported mode 0x%x", (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_connect, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_connect_req;

  /*
   * Populating mmgsdi_sap_connect_req_type header.
   */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_CONNECT_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_CONNECT_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_CONNECT_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*
   * Populating condition value
   */
  msg_ptr->condition = condition;

  /*
   * Populating mmgsdi_sap_connect_req_type content.
   */
  /*msg_ptr->request_header.sap_mode = sap_mode;*/
  /*
   * Calculate the payload len and request len.
   */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
         - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_CONNECT_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_connect_operation */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_CONNECT

DESCRIPTION
  This function attempts to perform a SAP connect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_connect(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  return mmgsdi_sap_connect_operation(
           client_id,
           card_slot,
           sap_mode,
           MMGSDI_SAP_CONN_COND_BLOCK_VOICE_OR_DATA,
           response_cb_ptr,
           client_ref);
} /* mmgsdi_sap_connect */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_CONNECT_EXT

DESCRIPTION
  This function attempts to perform a SAP connect. It also provides an option
  for the client to specify a condition for the SAP connection request.
  SAP connection request with proceed based on this condition.

PARAMS:
  client_id:           Client Id of the caller
  card_slot:           Physical slot for multiple slot targets.
  sap_mode:            Client/Server.
  condition:           Condtion to check for active call status.
  response_cb_ptr:     Response callback.
  client_ref:          User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_connect_ext(
    mmgsdi_client_id_type                 client_id,
    mmgsdi_slot_id_enum_type              card_slot,
    mmgsdi_sap_mode_enum_type             sap_mode,
    mmgsdi_sap_conn_condition_enum_type   condition,
    mmgsdi_callback_type                  response_cb_ptr,
    mmgsdi_client_data_type               client_ref)
{
  return mmgsdi_sap_connect_operation(
           client_id,
           card_slot,
           sap_mode,
           condition,
           response_cb_ptr,
           client_ref);
} /* mmgsdi_sap_connect_ext */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_DISCONNECT

DESCRIPTION
  This function attempts to perform a SAP disconnect.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  disconnect_mode: Graceful/Immediate.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_disconnect(
    mmgsdi_client_id_type               client_id,
    mmgsdi_slot_id_enum_type            card_slot,
    mmgsdi_sap_mode_enum_type           sap_mode,
    mmgsdi_disconnect_mode_enum_type    disconnect_mode,
    mmgsdi_callback_type                response_cb_ptr,
    mmgsdi_client_data_type             client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_sap_disconnect_req_type* msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;
  mmgsdi_event_data_type          event_data;

  /* Input Parameters Check.
   */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SAP_DISCONNECT_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_disconnect, not supported on slot 0x%x",
                  card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  memset(&event_data, 0, sizeof(mmgsdi_event_data_type));

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_disconnect_req;

  /*
   * Populating mmgsdi_sap_disconnect_req_type header.
   */
  task_cmd_ptr->cmd.cmd_enum               = MMGSDI_SAP_DISCONNECT_REQ;
  msg_ptr->request_header.client_id        = client_id;
  msg_ptr->request_header.request_type     = MMGSDI_SAP_DISCONNECT_REQ;
  msg_ptr->request_header.orig_request_type= MMGSDI_SAP_DISCONNECT_REQ;
  msg_ptr->request_header.slot_id          = card_slot;
  msg_ptr->request_header.client_data      = client_ref;
  msg_ptr->request_header.response_cb      = response_cb_ptr;
  /*
   * Populating mmgsdi_sap_disconnect_req_type content.
   */
  msg_ptr->disconnect_mode = disconnect_mode;
  msg_ptr->sap_mode        = sap_mode;
  /*
   * Calculate the payload len and request len.
   */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
         - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_DISCONNECT_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_disconnect */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_sap_get_atr_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;

  /*
   * The applications available on the card. */

  /* Input Parameters Check.
   */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SAP_GET_ATR_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_get_atr, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_get_atr_req;

  /*
   * Populate header.
   */
  task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_SAP_GET_ATR_REQ;
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.request_type       = MMGSDI_SAP_GET_ATR_REQ;
  msg_ptr->request_header.orig_request_type  = MMGSDI_SAP_GET_ATR_REQ;
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
         - uint32toint32(sizeof(mmgsdi_request_header_type));
  /*
   * Put on mmgsdi command queue.
   */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_GET_ATR_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_sap_get_atr */


/*===========================================================================
FUNCTION MMGSDI_SAP_SEND_APDU

DESCRIPTION
  This function will send an APDU to the SAP Client.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  channel_id:      The channel id.
  apdu_data:       The APDU to send to the card.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_send_apdu(
  mmgsdi_client_id_type          client_id,
  mmgsdi_slot_id_enum_type       card_slot,
  int32                          channel_id,
  mmgsdi_sap_send_apdu_data_type apdu_data,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr = NULL;
  mmgsdi_sap_send_apdu_req_type * msg_ptr      = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  int32                          total_mem_len = 0;
  int32                          temp_mem      = 0;
  uint32                         task_cmd_len  = 0;

  MMGSDIUTIL_RETURN_IF_NULL(apdu_data.data_ptr);

  /* Input Parameters Check.
   */
  if ( channel_id < 0 )
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if (apdu_data.data_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if ((apdu_data.data_len < MMGSDI_MIN_APDU_LEN) ||
           (apdu_data.data_len > MMGSDI_MAX_CHANNEL_DATA_CMD_LEN))
  {
    MMGSDI_DEBUG_MSG_ERROR_1("Bad data size (%u)", apdu_data.data_len);
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if ((apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] == SELECT) &&
             (apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET] == UIM_7816_APDU_MIN_SIZE) &&
             ((apdu_data.data_len < MMGSDI_SELECT_APDU_MIN_LEN) ||
              (apdu_data.data_ptr[MMGSDI_SELECT_APDU_LEN_OFFSET] < MMGSDI_SELECT_APDU_MIN_LEN)))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  else if (apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] == GET_RESPONSE)
  {
    if (apdu_data.data_len != 5)
    {
      MMGSDI_DEBUG_MSG_ERROR_2("Bad data size (req: %u, exp: %u)",
                               apdu_data.data_len, 5);
      return MMGSDI_INCORRECT_PARAMS;
    }
    else if ((apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET] != 0X00) ||
               (apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET] != 0X00))
    {
      MMGSDI_DEBUG_MSG_ERROR_2("GSDIBT bad p1(%u) or p2(%u)",
                               apdu_data.data_ptr[UIM_7816_APDU_P1_OFFSET],
                               apdu_data.data_ptr[UIM_7816_APDU_P2_OFFSET]);
      return MMGSDI_INCORRECT_PARAMS;
    }
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SAP_SEND_APDU_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_send_apdu, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + apdu_data.data_len;

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.sap_send_apdu_req, MMGSDI_SAP_SEND_APDU_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_send_apdu_req;

  /* Populate header.
    */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_SEND_APDU_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_SEND_APDU_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_SEND_APDU_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  /*
    * Populate the payload.
    */
  msg_ptr->channel_id    = int32touint8(channel_id);
  msg_ptr->data.data_len = apdu_data.data_len;
  mmgsdi_memscpy(msg_ptr->data.data_ptr,
                 int32touint32(msg_ptr->data.data_len),
                 apdu_data.data_ptr,
                 int32touint32(msg_ptr->data.data_len));
  /*
    * Calculate the payload len and request len.
    */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /*
  * Put on mmgsdi command queue.
  */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SEND_APDU_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_sap_send_apdu */


/*===========================================================================
FUNCTION MMGSDI_SAP_POWER_ON

DESCRIPTION
  This function attempts to perform a SAP power on

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_power_on(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  mmgsdi_task_cmd_type         * task_cmd_ptr = NULL;
  mmgsdi_sap_power_on_req_type* msg_ptr       = NULL;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  int32                         total_mem_len = 0;
  uint32                        task_cmd_len  = 0;

  /* Input Parameters Check.
   */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SAP_POWER_ON_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_power_on, unsupported mode 0x%x", (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_power_on, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_power_on_req;

  /*
   * Populating mmgsdi_sap_power_on_req_type header.
   */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_POWER_ON_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_POWER_ON_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_POWER_ON_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  /*
   * Populating mmgsdi_sap_power_on_req_type content.
   */
  /*msg_ptr->request_header.sap_mode = sap_mode;*/
  /*
   * Calculate the payload len and request len.
   */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_POWER_ON_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_power_on */


/*===========================================================================
FUNCTION MMGSDI_SAP_POWER_OFF

DESCRIPTION
  This function attempts to perform a SAP power off

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_power_off(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_sap_power_off_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;

  /* Input Parameters Check.
   */
  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_power_off, unsupported mode 0x%x", (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SAP_POWER_OFF_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_power_off, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_power_off_req;

  /*
   * Populating mmgsdi_sap_power_off_req_type header.
   */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_POWER_OFF_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_POWER_OFF_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_POWER_OFF_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  /*
   * Populating mmgsdi_sap_power_off_req_type content.
   */
  /*msg_ptr->request_header.sap_mode = sap_mode;*/
  /*
   * Calculate the payload len and request len.
   */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_POWER_OFF_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_power_off */


/*===========================================================================
FUNCTION MMGSDI_SAP_CARD_READER_STATUS

DESCRIPTION
  This function attempts to perform a SAP card reader status

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_card_reader_status(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  mmgsdi_task_cmd_type                  * task_cmd_ptr  = NULL;
  mmgsdi_sap_card_reader_status_req_type* msg_ptr = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status    = MMGSDI_ERROR;
  int32                                   total_mem_len = 0;
  uint32                                  task_cmd_len  = 0;

  /* Input Parameters Check.
   */
  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_card_reader_status, unsupported mode 0x%x",
                  (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SAP_CARD_READER_STATUS_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_card_reader_status, not supported on slot 0x%x",
                  card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_card_reader_status_req;

  /*
   * Populating mmgsdi_sap_card_reader_status_req_type header.
   */
  task_cmd_ptr->cmd.cmd_enum                  = MMGSDI_SAP_CARD_READER_STATUS_REQ;
  msg_ptr->request_header.client_id           = client_id;
  msg_ptr->request_header.request_type        = MMGSDI_SAP_CARD_READER_STATUS_REQ;
  msg_ptr->request_header.orig_request_type   = MMGSDI_SAP_CARD_READER_STATUS_REQ;
  msg_ptr->request_header.slot_id             = card_slot;
  msg_ptr->request_header.client_data         = client_ref;
  msg_ptr->request_header.response_cb         = response_cb_ptr;
  /*
   * Populating mmgsdi_sap_card_reader_status_req_type content.
   */
  /*
   * Calculate the payload len and request len.
   */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  mmgsdi_status = mmgsdi_caller_context_sap_card_reader_status_and_notify_client(task_cmd_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  return mmgsdi_status;
} /* mmgsdi_sap_card_reader_status */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SAP_RESET

DESCRIPTION
  This function attempts to perform a SAP RESET

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  sap_mode:        Client/Server.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_reset(
    mmgsdi_client_id_type     client_id,
    mmgsdi_slot_id_enum_type  card_slot,
    mmgsdi_sap_mode_enum_type sap_mode,
    mmgsdi_callback_type      response_cb_ptr,
    mmgsdi_client_data_type   client_ref)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_sap_reset_req_type     * msg_ptr     = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;

  /* Input Parameters Check.
   */
  if (sap_mode != MMGSDIBT_SERVER)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_reset, unsupported mode 0x%x", (int)sap_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SAP_RESET_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(card_slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("mmgsdi_sap_reset, not supported on slot 0x%x", card_slot);
    return MMGSDI_NOT_SUPPORTED;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_reset_req;
  /*
   * Populating mmgsdi_sap_reset_req_type header.
   */
  task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_SAP_RESET_REQ;
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.request_type       = MMGSDI_SAP_RESET_REQ;
  msg_ptr->request_header.orig_request_type  = MMGSDI_SAP_RESET_REQ;
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;

  msg_ptr->request_header.request_len = total_mem_len;
    msg_ptr->request_header.payload_len = total_mem_len
                                          - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SAP_RESET_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_sap_reset */ /*lint !e715*/


/*===========================================================================
FUNCTION:      MMGSDI_CARD_PUP

DESCRIPTION:
  This function is used to perform a power up to the SIM/USIM card.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client Id of the caller
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_callback_type    response_cb_ptr: Input.  Call back function when the
                                                   command execution is done.
  mmgsdi_card_pup_options_enum_type pup_option: Input.  Option as to whether or not
                                                   to notify sim removal event.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                   completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_pup(
    mmgsdi_client_id_type             client_id,
    mmgsdi_slot_id_enum_type          card_slot,
    mmgsdi_callback_type              response_cb_ptr,
    mmgsdi_card_pup_options_enum_type pup_option,
    mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr  = NULL;
  mmgsdi_card_pup_req_type*       msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_SUCCESS;
  int32                           total_mem_len = 0;
  uint32                          task_cmd_len  = 0;
  mmgsdi_slot_data_type         * slot_data_ptr = NULL;

  /* Input Parameters Check.
   */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_CARD_PUP_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(pup_option,
                                    MMGSDI_CARD_POWER_UP_SWITCH_UIM_PASSIVE,
                                    MMGSDI_CARD_POWER_UP_INITIAL_PUP);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(card_slot);

    /* Ignore pup if card is not present */
    if(slot_data_ptr != NULL &&
       slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED &&
       mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_CARD_SLOT_CHECK, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
    {
      UIM_MSG_HIGH_0("No card present, ignoring power up.");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(slot_data_ptr != NULL &&
       slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
    {
      UIM_MSG_ERR_0("Command not allowed since ONCHIP is activated");
      mmgsdi_status = MMGSDI_RESTRICTED_IN_ONCHIP_MODE;
    }
  } while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(MMGSDI_SUCCESS != mmgsdi_status)
  {
    return mmgsdi_status;
  }

  if (pup_option == MMGSDI_CARD_POWER_UP_INITIAL_PUP)
  {
    mmgsdi_refresh_req_type      refresh_req;

    memset(&refresh_req,0,sizeof(mmgsdi_refresh_req_type));

    refresh_req.refresh_req_type = MMGSDI_REFRESH_ORIG_PUP_REQ;
    refresh_req.refresh.card_req.refresh_mode = MMGSDI_REFRESH_RESET_AUTO;
    refresh_req.refresh.card_req.source       = MMGSDI_REFRESH_SOURCE_MMGSDI;

    /* when a card power up initail pup option is provided, a power
       up command is triggered via a refresh RESET command */
    mmgsdi_status = mmgsdi_client_build_refresh_req( client_id,
                                                     card_slot,
                                                     &refresh_req,
                                                     FALSE,
                                                     response_cb_ptr,
                                                     client_ref,
                                                     FALSE);

    if ((mmgsdi_status != MMGSDI_SUCCESS) &&
        (mmgsdi_status != MMGSDI_REFRESH_IN_PROGRESS))
    {
      MMGSDI_DEBUG_MSG_ERROR_1("UIM Did not accept reset(%u)", mmgsdi_status);
    }
  }
  else
  {
    total_mem_len = sizeof(mmgsdi_task_cmd_type);

    task_cmd_len = int32touint32(total_mem_len);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
    if (task_cmd_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    /*---------------------------------------------------------------------------
      - No need to align pointers
      ---------------------------------------------------------------------------*/
    msg_ptr = &task_cmd_ptr->cmd.cmd.card_pup_req;

    /*
    * Populating mmgsdi_card_pup_req_type header.
    */
    task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_CARD_PUP_REQ;
    msg_ptr->request_header.client_id          = client_id;
    msg_ptr->request_header.orig_request_type  = MMGSDI_CARD_PUP_REQ;
    msg_ptr->request_header.request_type       = MMGSDI_CARD_PUP_REQ;
    msg_ptr->request_header.slot_id            = card_slot;
    msg_ptr->request_header.client_data        = client_ref;
    msg_ptr->request_header.response_cb        = response_cb_ptr;

    /*
    * Populating mmgsdi_card_pup_req_type content.
    */
    msg_ptr->option = pup_option;

    /*
    * Calculate the payload len and request len.
    */
    msg_ptr->request_header.request_len = total_mem_len;
    msg_ptr->request_header.payload_len = total_mem_len
                                          - uint32toint32(sizeof(mmgsdi_request_header_type));

    /* Put on mmgsdi command queue */
    mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
      MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    }

    UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_CARD_PUP_REQ 0x%x",
                   mmgsdi_status);
  }

  return mmgsdi_status;
} /* mmgsdi_card_pup */


/*===========================================================================
FUNCTION:      MMGSDI_CARD_PDOWN

DESCRIPTION:
  This function is used to perform a power down to the SIM/USIM card.

  Description of the parameters:
  mmgsdi_client_id_type    client_id:      Input.  Client Id of the caller
  mmgsdi_slot_id_enum_type card_slot:      Input.  Physical slot for multiple
                                                   slot targets.
  mmgsdi_callback_type    response_cb_ptr: Input.  Call back function when the
                                                   command execution is done.
  mmgsdi_card_pup_options_enum_type pdown_option: Input.Option as to whether or not
                                                   to notify sim insertion.
  mmgsdi_client_data_type  client_ref:     Input.  User Data returned upon
                                                   completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_card_pdown(
    mmgsdi_client_id_type                   client_id,
    mmgsdi_slot_id_enum_type                card_slot,
    mmgsdi_callback_type                    response_cb_ptr,
    mmgsdi_card_pdown_options_enum_type     pdown_option,
    mmgsdi_client_data_type                 client_ref
)
{
  mmgsdi_task_cmd_type         * task_cmd_ptr  = NULL;
  mmgsdi_card_pdown_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;
  int32                          total_mem_len = 0;
  uint32                         task_cmd_len  = 0;
  mmgsdi_slot_data_type        * slot_data_ptr = NULL;

  if(mmgsdi_util_is_slot_valid(card_slot) == FALSE)
  {
    UIM_MSG_ERR_1(" Invalid Card Slot 0x%x in mmgsdi_card_pdown", card_slot);
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_CARD_PDOWN_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(pdown_option,
                                    MMGSDI_CARD_POWER_DOWN_ONLY,
                                    MMGSDI_CARD_POWER_DOWN_NOTIFY_GSDI);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(card_slot);

    /* Ignore pdown if card is not present */
    if(slot_data_ptr != NULL &&
       slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED &&
       mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_CARD_SLOT_CHECK, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
    {
      UIM_MSG_HIGH_0("No card present, ignoring power down.");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(slot_data_ptr != NULL &&
       slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
    {
      UIM_MSG_ERR_0("Command not allowed since ONCHIP is activated");
      mmgsdi_status = MMGSDI_RESTRICTED_IN_ONCHIP_MODE;
    }
  } while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(MMGSDI_SUCCESS != mmgsdi_status)
  {
    return mmgsdi_status;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.card_pdown_req;

  /*
   * Populating mmgsdi_card_pdown_req_type header.
   */
  task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_CARD_PDOWN_REQ;
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.orig_request_type  = MMGSDI_CARD_PDOWN_REQ;
  msg_ptr->request_header.request_type       = MMGSDI_CARD_PDOWN_REQ;
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;
  /*
   * Populating mmgsdi_card_pdown_req_type content.
   */

  msg_ptr->option = pdown_option;

  /*
   * Calculate the payload len and request len.
   */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_CARD_PDOWN_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_card_pdown */


/*===========================================================================
FUNCTION MMGSDI_ACTIVATE_ONCHIP

DESCRIPTION
  This function will build a MMGSDI_ACTIVATE_ONCHIP_SIM_REQ and queue it to
  the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets. Only slot-1
                     is supported
  onchip_sim_config: TLV Data Len and TLV Data.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  Note: If client fails to provide a client id, meaning the client_id
        passed is 0, then an internal client id will be used.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Access to a real card will not be possible without a power cycle
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_activate_onchip (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_onchip_sim_config_data_type onchip_sim_config,
  mmgsdi_onchip_mode_enum_type       onchip_mode,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                 * task_cmd_ptr  = NULL;
  mmgsdi_activate_onchip_sim_req_type  * msg_ptr       = NULL;
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_ERROR;
  int32                                  total_mem_len = 0;
  uint32                                 task_cmd_len  = 0;
  int32                                  temp_mem      = 0;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_ACTIVATE_ONCHIP_SIM_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS && mmgsdi_generic_data_ptr != NULL)
  {
    client_id = mmgsdi_generic_data_ptr->client_id;
  }

  if (mmgsdi_util_is_slot_valid(card_slot) == FALSE)
  {
    MMGSDI_DEBUG_MSG_ERROR_1("In mmgsdi_activate_onchip_sim, Slot ID: 0x%x not supported",
                             card_slot);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if ( onchip_sim_config.data_len > MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE ||
       onchip_sim_config.data_len < 0  )
  {
    MMGSDI_DEBUG_MSG_ERROR_1("CONFIG DATA LEN INVALID 0x%x",
                             onchip_sim_config.data_len);
    return MMGSDI_ERROR;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ( onchip_sim_config.data_len > 0 )
  {
    /* Check is only valid if the Len != 0 */
    MMGSDIUTIL_RETURN_IF_NULL(onchip_sim_config.data_ptr);
  }
  /* --------------------------------------------------------------------------
     Allocate 1 large block of memory for the task_cmd_ptr and for the
     onchip config data.
     NOTE:  Side effect of the following total len is that it may not
            match exactly as a result of mmgsdi_task_cmd_data_type being
            a union. Will be addressed by using the correct type size
            rather than sizeof mmgsdi_task_cmd_data_type
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + onchip_sim_config.data_len;

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.act_onchip_sim_req, MMGSDI_ACTIVATE_ONCHIP_SIM_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.act_onchip_sim_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_ACTIVATE_ONCHIP_SIM_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_ACTIVATE_ONCHIP_SIM_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_ACTIVATE_ONCHIP_SIM_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*-----------------------------------------------------------------------------
    Calculate the payload len and request len
    ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len -
                                       uint32toint32(sizeof(mmgsdi_request_header_type));

  msg_ptr->onchip_sim.data_len = onchip_sim_config.data_len;
  msg_ptr->onchip_sim_mode     = onchip_mode;

  if ( onchip_sim_config.data_len > 0 )
  {
    /* --------------------------------------------------------------------------
       Set the Onchip configuration Data to point to the end of the task
       command pointer block allocated.  Then copy the configuration Data
       ------------------------------------------------------------------------*/
    mmgsdi_memscpy(msg_ptr->onchip_sim.data_ptr,
                   int32touint32(onchip_sim_config.data_len),
                   onchip_sim_config.data_ptr,
                   int32touint32(onchip_sim_config.data_len));
  }

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr
    ** has not been put onto the command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_2("Queue of MMGSDI command: MMGSDI_ACTIVATE_ONCHIP_SIM_REQ status=0x%x, onchip mode=0x%x",
                 mmgsdi_status, onchip_mode);

  return mmgsdi_status;
} /* mmgsdi_activate_onchip */


/*===========================================================================
FUNCTION MMGSDI_ACTIVATE_ONCHIP_SIM

DESCRIPTION
  This function will build a MMGSDI_ACTIVATE_ONCHIP_SIM_REQ and queue it to
  the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets. Only slot-1
                     is supported
  onchip_sim_config: TLV Data Len and TLV Data.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  Note: If client fails to provide a client id, meaning the client_id
        passed is 0, then an internal client id will be used.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Access to a real card will not be possible without a power cycle
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_activate_onchip_sim (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_onchip_sim_config_data_type onchip_sim_config,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  return mmgsdi_activate_onchip(client_id,
                                card_slot,
                                onchip_sim_config,
                                MMGSDI_ONCHIP_MODE_SIM,
                                response_cb_ptr,
                                client_ref);
} /* mmgsdi_activate_onchip_sim */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_ACTIVATE_ONCHIP_USIM

DESCRIPTION
  This function will build a MMGSDI_ACTIVATE_ONCHIP_SIM_REQ and queue it to
  the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets. Only slot-1
                     is supported
  onchip_sim_config: TLV Data Len and TLV Data.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  Note: If client fails to provide a client id, meaning the client_id
        passed is 0, then an internal client id will be used.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Access to a real card will not be possible without a power cycle
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_activate_onchip_usim (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_onchip_sim_config_data_type onchip_sim_config,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  return mmgsdi_activate_onchip(client_id,
                                card_slot,
                                onchip_sim_config,
                                MMGSDI_ONCHIP_MODE_USIM,
                                response_cb_ptr,
                                client_ref);
} /* mmgsdi_activate_onchip_usim */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_GET_ESN_ME_CHANGE_FLAG

DESCRIPTION
  This function will return the esn ME change flag obtained during post pin1
  init

PARAMS:
  none

DEPENDENCIES:

LIMITATIONS:

RETURN VALUE:
   mmgsdi_esn_me_change_enum_type:
     MMGSDI_ESN_ME_UNKNOWN
     MMGSDI_ESN_ME_NO_CHANGE
     MMGSDI_ESN_ME_CHANGE
     MMGSDI_ESN_ME_UPDATE_PENDING
     MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED

SIDE EFFECTS:
===========================================================================*/
mmgsdi_esn_me_change_enum_type mmgsdi_get_esn_me_change_flag (
  mmgsdi_slot_id_enum_type slot)
{
#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_slot_data_type          *slot_data_ptr   = NULL;
  mmgsdi_esn_me_change_enum_type  esn_me_cng_flag = MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED;

  /* Protect access to slot data table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr != NULL)
  {
  esn_me_cng_flag = slot_data_ptr->esn_me_change_flag;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return esn_me_cng_flag;
#endif /* FEATURE_MMGSDI_3GPP2 */
  (void)slot;
  return MMGSDI_ESN_ME_CHANGE_NOT_SUPPORTED;
}/* mmgsdi_get_esn_me_change_flag */ /*lint !e715*/


/* ============================================================================
FUNCTION MMGSDI_GET_ATR

DESCRIPTION:
  This is the API used to retrieve the ATR Information which pertains to the
  card(s) currently inserted in the ME.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  option:          Additional optional parameters for this API. Currently none
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Archived by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
  ========================================================================== */
mmgsdi_return_enum_type mmgsdi_get_atr(
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_option_type       option,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_task_cmd_type      * task_cmd_ptr  = NULL;
  mmgsdi_get_atr_req_type   * msg_ptr       = NULL;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_ERROR;
  int32                       total_mem_len = 0;
  uint32                      task_cmd_len  = 0;

  /* -------------------------------------------------------------------------
     Validate the Input Parameters
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_GET_ATR_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(card_slot, MMGSDI_SLOT_1, MMGSDI_SLOT_3);
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len  = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* --------------------------------------------------------------------------
    No need to Align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.get_atr_req;

  /*
   * Populate header.
   */
  task_cmd_ptr->cmd.cmd_enum                 = MMGSDI_GET_ATR_REQ;
  msg_ptr->request_header.client_id          = client_id;
  msg_ptr->request_header.request_type       = MMGSDI_GET_ATR_REQ;
  msg_ptr->request_header.orig_request_type  = MMGSDI_GET_ATR_REQ;
  msg_ptr->request_header.slot_id            = card_slot;
  msg_ptr->request_header.client_data        = client_ref;
  msg_ptr->request_header.response_cb        = response_cb_ptr;

  /* --------------------------------------------------------------------------
     Calculate the payload len and request len.
     ----------------------------------------------------------------------- */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
         - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the
    ** command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_ATR_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_get_atr */ /*lint !e715*/


/*===========================================================================
FUNCTION MMGSDI_SEND_APDU_OPERATION

DESCRIPTION
  This function shall populate and queue the send APDU request to MMGSDI.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  apdu_data:       The APDU to send to the card.
  apdu_option:     Specify the type of response required
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.
  req_type:        Type of Send APDU request.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achieved by calling mmgsdi_client_id_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_send_apdu_operation(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_send_apdu_data_type         apdu_data,
  mmgsdi_send_apdu_options_enum_type apdu_option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref,
  mmgsdi_cmd_enum_type               req_type)
{
  mmgsdi_task_cmd_type      * task_cmd_ptr  = NULL;
  mmgsdi_send_apdu_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_ERROR;
  int32                       total_mem_len = 0;
  int32                       temp_mem      = 0;
  uint32                      task_cmd_len  = 0;
  boolean                     reject_apdu   = FALSE;

  UIM_MSG_HIGH_2("mmgsdi_send_apdu_operation, Req: 0x%x, Option: 0x%x",
                 req_type, apdu_option);

  /* -------------------------------------------------------------------------
     Validate the Input Parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(card_slot, MMGSDI_SLOT_1, MMGSDI_SLOT_3);

  /* APDU DATA Must not be NULL */
  MMGSDIUTIL_RETURN_IF_NULL_2(response_cb_ptr, apdu_data.data_ptr);

  mmgsdi_status = mmgsdi_util_apdu_sanity_check(&apdu_data);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    req_type);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Process interindustry (Bit b8 set to 0) MANAGE CHANNEL APDUs as MMGSDI request
     In case of failure and the APDU should not be rejected, continue as APDU streaming
   */
  if(apdu_data.data_len >= UIM_CASE1_7816_APDU_SIZE &&
     (apdu_data.data_ptr[UIM_7816_APDU_CLASS_OFFSET] & 0x80) == 0 &&
     (uim_instrns_enum_type)apdu_data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] == MANAGE_CHANNEL)
  {
    mmgsdi_status = mmgsdi_apdu_manage_channel(client_id,
                                               card_slot,
                                               apdu_option,
                                               apdu_data,
                                               client_ref,
                                               response_cb_ptr,
                                               req_type,
                                               &reject_apdu);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      return MMGSDI_SUCCESS;
    }

    /* In case of failure, continue with APDU streaming unless
       this APDU should be rejected */
    if (reject_apdu)
    {
      return MMGSDI_ACCESS_DENIED;
    }
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + apdu_data.data_len;

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if(task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.send_apdu_req, req_type);

  msg_ptr = &task_cmd_ptr->cmd.cmd.send_apdu_req;

  /*---------------------------------------------------------------------------
    Populate header.
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = req_type;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = req_type;
  msg_ptr->request_header.orig_request_type = req_type;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  /*---------------------------------------------------------------------------
     Populate the payload.
  ---------------------------------------------------------------------------*/
  msg_ptr->data.data_len = apdu_data.data_len;
  mmgsdi_memscpy(msg_ptr->data.data_ptr,
                 int32touint32(msg_ptr->data.data_len),
                 apdu_data.data_ptr,
                 int32touint32(msg_ptr->data.data_len));
  msg_ptr->apdu_option = apdu_option;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /*---------------------------------------------------------------------------
    Put on mmgsdi command queue.
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_2("Queue of MMGSDI command: MMGSDI_SEND_APDU_REQ 0x%x status 0x%x",
                 req_type, mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_send_apdu_operation */


/*===========================================================================
FUNCTION MMGSDI_SEND_APDU

DESCRIPTION
  This function will send an APDU to the card.  This will cause MMGSDI
  to generate a get response as needed for the client to handle.  If the
  Get Response does not occur when the client is notified, then the data
  considered part of the get response is discarded.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  apdu_data:       The APDU to send to the card.
  option:          Additional optional parameters for this API. Currently none
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achieved by calling mmgsdi_client_id_and_evt_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_apdu(
  mmgsdi_client_id_type      client_id,
  mmgsdi_slot_id_enum_type   card_slot,
  mmgsdi_send_apdu_data_type apdu_data,
  mmgsdi_option_type         option,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref)
{
  (void) option;

  return mmgsdi_send_apdu_operation(
           client_id,
           card_slot,
           apdu_data,
           MMGSDI_SEND_APDU_RETURN_IMPLICIT_STATUS_WORDS,
           response_cb_ptr,
           client_ref,
           MMGSDI_SEND_APDU_REQ);
} /* mmgsdi_send_apdu */


/*===========================================================================
FUNCTION MMGSDI_SEND_APDU_EXT

DESCRIPTION
  Sends an APDU command to the card inserted in the specified slot.

  Provides option to request for intermediate responses (i.e. SW1/SW2 of
  0x61/0xXX for UICC cards and 0x9F/0xXX for ICC cards) or the final
  card response.

PARAMS:
  client_id:       Client Id of the caller
  card_slot:       Physical slot for multiple slot targets.
  apdu_data:       The APDU to send to the card.
  apdu_option:     Specify the type of response required
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achieved by calling mmgsdi_client_id_and_evt_reg().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_apdu_ext(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_send_apdu_data_type         apdu_data,
  mmgsdi_send_apdu_options_enum_type apdu_option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  return mmgsdi_send_apdu_operation(
           client_id,
           card_slot,
           apdu_data,
           apdu_option,
           response_cb_ptr,
           client_ref,
           MMGSDI_SEND_APDU_EXT_REQ);
} /* mmgsdi_send_apdu_ext */


/*===========================================================================
   FUNCTION MMGSDI_NAA_REFRESH

   DESCRIPTION
     This API is used by the Generic SIM Toolkit Task to request that a REFRESH
     be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 7 USIM
     Refresh Modes (as defined by the enum mmgsdi_refresh_mode_enum_type).

     MMGSDI_REFRESH_NAA_INIT_FULL_FCN      (SIM/USIM)
     MMGSDI_REFRESH_FCN                    (SIM/USIM)
     MMGSDI_REFRESH_NAA_INIT_FCN           (SIM/USIM)
     MMGSDI_REFRESH_NAA_INIT               (SIM/USIM)
     MMGSDI_REFRESH_RESET                  (SIM/USIM)
     MMGSDI_REFRESH_NAA_APP_RESET          (USIM)
     MMGSDI_REFRESH_3G_SESSION_RESET       (USIM)

   PARAMETERS:
     client_id:          Client ID of the caller
     card_slot:          Slot on which refresh has to be performed
     aid:                Application ID (only applicable to 3G)
     refresh_mode:       Refresh Mode as sent by the card
     refresh_files:      Refresh Files if any (optional)
     num_files:          Number of refresh files
     alpha_string_ptr:   Alpha string, if Alpha is present
     response_cb_ptr:    Callback for MMGSDI to call  upon completion of the
                         registration request.
     client_ref_ptr:     User Data returned to the user upon completion of
                         the command.
   DEPENDENCIES
     None

   LIMITATIONS:
     None

   RETURN VALUE
     mmgsdi_return_enum_type

   SIDE EFFECTS
     Depending on the Mode requested, various events are generated as a result
     of the REFRESH Peformed.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_naa_refresh(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_aid_type                    aid,
  mmgsdi_refresh_mode_enum_type      refresh_mode,
  mmgsdi_data_type                   refresh_data,
  uint8                              num_files,
  mmgsdi_alpha_string_type           *alpha_string_ptr,
  mmgsdi_option_type                 option,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_refresh_source_enum_type refresh_source = MMGSDI_REFRESH_SOURCE_SIM;

#ifdef FEATURE_UIM_MULTI_PROFILE
  if(mmgsdi_profile_info_ptr != NULL &&
     mmgsdi_profile_info_ptr->is_emergency_refresh)
  {
    refresh_source = MMGSDI_REFRESH_SOURCE_ECALL;
    mmgsdi_profile_info_ptr->is_emergency_refresh = FALSE;
  }
#endif /* FEATURE_UIM_MULTI_PROFILE */

  return mmgsdi_naa_refresh_ext(client_id,
                                card_slot,
                                aid,
                                refresh_mode,
                                refresh_data,
                                num_files,
                                alpha_string_ptr,
                                option,
                                refresh_source,
                                response_cb_ptr,
                                client_ref);
}/*mmgsdi_naa_refresh*/ /*lint !e715*/


/*=========================================================================
FUNCTION:      MMGSDI_CLIENT_ID_AND_EVT_REG

DESCRIPTION:
  This function allows client registration for new event callback that
  will contain additional info based on specific events being sent.
  The response to this request will return client ID to the calling task.

PARAMETERS:
evt_cb_ptr:      Pointer to callback for event notifications.
                 If NULL is passed, no event callback will be made

response_cb_ptr: Callback for MMGSDI to call upon completion of the
                 registration request.

client_ref_ptr:  User Data returned to the user upon completion of the
                 command.

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
  MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                           because the max number of commands are already queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to
  notify any client of any events.
  ===================================================================== */
mmgsdi_return_enum_type  mmgsdi_client_id_and_evt_reg (
  mmgsdi_evt_callback_type   *evt_cb_ptr,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  return mmgsdi_client_id_and_evt_reg_ext(evt_cb_ptr,
                                          response_cb_ptr,
                                          client_ref,
                                          NULL,
                                          0);
} /* mmgsdi_client_id_and_evt_reg */


/*=========================================================================
FUNCTION:      MMGSDI_CLIENT_ID_AND_EVT_REG_EXT

DESCRIPTION:
  This function allows client registration for new event callback that
  will contain additional info based on specific events being sent.
  The response to this request will return client ID to the calling task.

PARAMETERS:
evt_cb_ptr:      Pointer to callback for event notifications.
                 If NULL is passed, no event callback will be made
response_cb_ptr: Callback for MMGSDI to call upon completion of the
                 registration request.
client_ref_ptr:  User Data returned to the user upon completion of the
                 command.
client_name_ptr: Name of the client.
client_name_len: Length of the client name above.

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
  MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                           because the max number of commands are already queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to
  notify any client of any events.
  ===================================================================== */
mmgsdi_return_enum_type  mmgsdi_client_id_and_evt_reg_ext (
  mmgsdi_evt_callback_type   *evt_cb_ptr,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref,
  const char*                client_name_ptr,
  uint8                      client_name_len
)
{
  mmgsdi_task_cmd_type                  *task_cmd_ptr   = NULL;
  mmgsdi_client_id_reg_req_type         *msg_ptr        = NULL;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_ERROR;
  int32                                 total_mem_len   = 0;
  uint32                                task_cmd_len    = 0;
  char                                  rex_task_name[REX_TASK_NAME_LEN + 1];

 /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* No Need to Align Pointer inside client ID and Evt Reg structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.client_id_reg_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_usim_auth_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Client Data Pointer
      5) Response CallBack
      6) Event Callback
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CLIENT_ID_AND_EVT_REG_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_CLIENT_ID_AND_EVT_REG_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CLIENT_ID_AND_EVT_REG_REQ;
  msg_ptr->request_header.client_id         = 0;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  (void)rex_get_task_name(rex_self(), rex_task_name, sizeof(rex_task_name),NULL);
  strlcpy(msg_ptr->client_task_name, rex_task_name, sizeof(msg_ptr->client_task_name));

  if (client_name_ptr &&
      client_name_len <= MMGSDI_TASK_NAME_LEN)
  {
    strlcpy(msg_ptr->client_name, client_name_ptr, client_name_len);
  }
  else
  {
    /* If the client did not provide any name, copy task_name as both client_name
       and task_name */
    strlcpy(msg_ptr->client_name, rex_task_name, sizeof(msg_ptr->client_name));
  }

  /* Populate command specific information */
  msg_ptr->evt_cb_ptr                       = evt_cb_ptr;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
     onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_CLIENT_ID_AND_EVT_REG_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_client_id_and_evt_reg_ext */


/* ============================================================================
   FUNCTION       MMGSDI_GET_ALL_AVAILABLE_APPS

   DESCRIPTION
     This function will build a request to the MMGSDI Task to request the
     information of all available applications from the UICC/ICC Card(s).

   PARAMETERS:
   client_id:         Client ID of the requesting client.
   response_cb_ptr:   Response callback.
   client_ref:        User Data returned upon completion of this cmd.

   DEPENDENCIES:
     A valid Client ID is required, which can be achieved by calling
     mmgsdi_client_id_reg().

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already queued.

   SIDE EFFECTS:

========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_all_available_apps (
  mmgsdi_client_id_type              client_id,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                 *task_cmd_ptr   = NULL;
  mmgsdi_available_apps_req_type       *msg_ptr        = NULL;
  mmgsdi_return_enum_type               mmgsdi_status  = MMGSDI_ERROR;
  int32                                 total_mem_len  = 0;
  uint32                                task_cmd_len   = 0;

 /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client id check
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_GET_ALL_AVAILABLE_APPS_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* No need to align pointer inside available apps request structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.available_apps_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_available_apps_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Client Data Pointer
      5) Response CallBack
      6) Event Callback
      7) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GET_ALL_AVAILABLE_APPS_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_GET_ALL_AVAILABLE_APPS_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GET_ALL_AVAILABLE_APPS_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.session_id        = MMGSDI_INVALID_SESSION_ID;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
     onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_ALL_AVAILABLE_APPS_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_get_all_available_apps */


/* ============================================================================
   FUNCTION       MMGSDI_GET_ALL_AVAILABLE_APPS_SYNC

   DESCRIPTION
     This function will build a request to the MMGSDI Task for the information
     of all available applications from the UICC/ICC Card(s).
     The client calling this function expects an instant response.

     This function uses critical sections to protect accesses to
     MMGSDI global data. Hence, it is advisable to pet the dog
     before invoking the function.

   PARAMETERS:
     client_id:         Client ID of the requesting client.
     available_apps_ptr:Buffer to store the information of all available
                        applications on the UICC/ICC Card(s).

   DEPENDENCIES:
     A valid Client ID is required, which can be achieved by calling
     mmgsdi_client_id_reg().

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_INIT:         Called before CARD_INSERTED on either slot
   SIDE EFFECTS:

===============================================================================*/
mmgsdi_return_enum_type mmgsdi_get_all_available_apps_sync (
  mmgsdi_client_id_type              client_id,
  mmgsdi_available_apps_type        *available_apps_ptr
)
{
  mmgsdi_return_enum_type  mmgsdi_status   = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_0("In mmgsdi_get_all_available_apps_sync");

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) NULL check for available_apps_ptr
    2) Client ID check
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(available_apps_ptr);

  /* Protect access to the CLient ID reg. table/App table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_util_get_all_available_apps(available_apps_ptr);
  }
  else
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_get_all_available_apps_sync */


/*===========================================================================
   FUNCTION:      MMGSDI_CLIENT_ID_AND_EVT_DEREG

   DESCRIPTION:
     This function allows MMGSDI clients to deregister from MMGSDI. If
     successful, the client will no longer receive MMGSDI events or be able
     to send commands to MMGSDI.

     If the client still has card or provisioning sessions opened then these
     will be closed. Non-provisioning sessions must be closed before calling
     this API. If non-provisioning sessions are still open then the command
     will not be executed and the command callback will return MMGSDI_ERROR.

   PARAMETERS:
   mmgsdi_client_id_type      client_id:    Input.  Client ID

   mmgsdi_callback_type response_cb_ptr:    Input.  Callback for MMGSDI to call
                                                    upon completion of the
                                                    deregistration request.

   mmgsdi_client_data_type client_ref_ptr:  Input.  User Data returned to the user
                                                    upon completion of the command.

   DEPENDENCIES:
     A valid Client ID is required, which can be achieved by calling
     mmgsdi_client_id_reg()

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            The command could not be generated and queued.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_client_id_and_evt_dereg (
  mmgsdi_client_id_type      client_id,
  mmgsdi_callback_type       response_cb_ptr,
  mmgsdi_client_data_type    client_ref
)
{
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_client_id_dereg_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_ERROR;
  int32                             total_mem_len = 0;
  uint32                            task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (NULL == response_cb_ptr)
  {
    /* if client does not provide the call back then use mmgsdi internal call back */
    response_cb_ptr = mmgsdi_util_discard_cb;
  }
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* No Need to Align Pointer inside client ID Dereg structure */
  msg_ptr = &task_cmd_ptr->cmd.cmd.client_id_dereg_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_client_id_and_evt_dereg_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Client Data Pointer
      5) Response CallBack
      6) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_client_id_and_evt_dereg */


/*===========================================================================
   FUNCTION:      MMGSDI_GET_RECOVERY_DATA_SYNC

   DESCRIPTION:
     Returns all information needed for recovering a slot which has been Reset.
     On UICC card, channel on which streaming is done and only non prov session
     is opened will not be recovered. On ICC card, channel other than default
     channel will not be recovered.

   PARAMETERS:
   uim_recovery_data_type recovery_data_ptr:  Output.  Pointer to the MMGSDI
                                                       recovery data type.
                                                       The result is stored
                                                       in this pointer.

   uim_slot_type          slot:               Input.   Slot for which data
                                                       is needed.
   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task can invoke this API

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Request was successfully handled.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_ERROR:            Generic error.

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_recovery_data_sync(
  uim_recovery_data_type *recovery_data_ptr,
  uim_slot_type           uim_slot_id
)
{
  mmgsdi_slot_id_enum_type       slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_protocol_enum_type      protocol         = MMGSDI_MAX_PROTOCOL_ENUM;
  uint8                          channel_index    = 0;
  uint8                          i                = 0;
  uint8                          channel_id       = 0;
  uint8                          rehab_data_index = 0;
  mmgsdi_int_app_info_type      *app_info_ptr     = NULL;
  boolean                        upin_info_done   = FALSE;
  mmgsdi_rehab_info_type        *rehab_info_ptr   = NULL;
  mmgsdi_slot_data_type         *slot_data_ptr    = NULL;
  mmgsdi_return_enum_type        mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_channel_state_enum_type channel_state    = MMGSDI_CHANNEL_STATE_NONE;
  mmgsdi_access_type             access;

  UIM_MSG_HIGH_0("In mmgsdi_get_recovery_data_sync");

  MMGSDIUTIL_RETURN_IF_NULL(recovery_data_ptr);
  memset(&access, 0x00, sizeof(mmgsdi_access_type));

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  /* Secret info like PIN data can be exposed only to known and trusted clients.
     Hence, the API is allowed to be called only by UIM */
  if(mmgsdi_util_is_uim_task_context() == FALSE)
  {
    UIM_MSG_ERR_0("mmgsdi_get_recovery_data_sync() invoked in non-UIM context");
    return MMGSDI_ERROR;
  }
#endif /* !FEATURE_UIM_TEST_FRAMEWORK*/

  /* Get slot specific data */
  switch (uim_slot_id)
  {
    case UIM_SLOT_1:
      slot_id = MMGSDI_SLOT_1;
      break;

    case UIM_SLOT_2:
      slot_id = MMGSDI_SLOT_2;
      break;

    case UIM_SLOT_3:
      slot_id = MMGSDI_SLOT_3;
      break;

    default:
      UIM_MSG_ERR_1("Incorrect UIM Slot 0x%x", uim_slot_id);
      return MMGSDI_INCORRECT_PARAMS;
  }

  /* Protect access to Channel/App/PIN info globals */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* Reset GSTK loc envelope (VPE) response count */
  mmgsdi_util_reset_gstk_loc_env_rsp_count(slot_id);

  /* Get the slot data */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    return MMGSDI_INCORRECT_PARAMS;
  }

  UIM_MSG_HIGH_1("Recovery, Slot: 0x%x", slot_id);

  /* Get the rehab. info for the slot */
  rehab_info_ptr = &slot_data_ptr->rehab_info;

  /* Initialize uim_recovery_data_type */
  memset(recovery_data_ptr, 0x00, sizeof(uim_recovery_data_type));
  for (i = 0;
       i < (sizeof(recovery_data_ptr->app_data) / sizeof(recovery_data_ptr->app_data[0]));
       i++)
  {
    recovery_data_ptr->app_data[i].do_not_recover = TRUE;
  }

  /* Fill the slot ID into the recovery info */
  recovery_data_ptr->slot = uim_slot_id;

  /* Determine protocol for the slot in recovery */
  protocol = mmgsdi_util_determine_protocol(slot_id);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_1("Recovery, Card Protocol: 0x%x", protocol);

  /* Fill the card protocol into the recovery info */
  if(protocol == MMGSDI_UICC)
  {
    recovery_data_ptr->protocol = UIM_UICC;
  }
  else if(protocol == MMGSDI_ICC)
  {
    recovery_data_ptr->protocol = UIM_ICC;
  }

  /* If card protocol is ICC, update the recovery data to indicate
     the files that need rehabilitation */
  if(protocol == MMGSDI_ICC)
  {
#ifdef FEATURE_MMGSDI_3GPP
    /* IMSI rehabilitation Data */
    if(rehab_info_ptr->imsi_rehabilitated)
    {
      UIM_MSG_HIGH_0("Recovery, IMSI Rehab required");

      recovery_data_ptr->rehab_data[rehab_data_index].rehab_reqd = TRUE;
      access.access_method = MMGSDI_EF_ENUM_ACCESS;
      access.file.file_enum = MMGSDI_GSM_IMSI;
      (void)mmgsdi_file_set_uim_path(&access, &recovery_data_ptr->rehab_data[rehab_data_index].path);
      rehab_data_index++;
    }

    /* LOCI rehabilitation Data */
    if(rehab_info_ptr->loci_rehabilitated)
    {
      UIM_MSG_HIGH_0("Recovery, LOCI Rehab required");

      recovery_data_ptr->rehab_data[rehab_data_index].rehab_reqd = TRUE;
      access.access_method = MMGSDI_EF_ENUM_ACCESS;
      access.file.file_enum = MMGSDI_GSM_LOCI;
      (void)mmgsdi_file_set_uim_path(&access, &recovery_data_ptr->rehab_data[rehab_data_index].path);
      rehab_data_index++;
    }
#endif /* FEATURE_MMGSDI_3GPP */

#ifdef FEATURE_MMGSDI_3GPP2
    /* IMSI_M rehabilitation Data */
    if(rehab_info_ptr->cdma_fdn_files_rehab[0])
    {
      UIM_MSG_HIGH_0("Recovery, IMSI_M Rehab required");

      recovery_data_ptr->rehab_data[rehab_data_index].rehab_reqd = TRUE;
      access.access_method = MMGSDI_EF_ENUM_ACCESS;
      access.file.file_enum = MMGSDI_CDMA_IMSI_M;
      (void)mmgsdi_file_set_uim_path(&access, &recovery_data_ptr->rehab_data[rehab_data_index].path);
      rehab_data_index++;
    }

    /* IMSI_T rehabilitation Data */
    if(rehab_info_ptr->cdma_fdn_files_rehab[1])
    {
      UIM_MSG_HIGH_0("Recovery, IMSI_T Rehab required");

      recovery_data_ptr->rehab_data[rehab_data_index].rehab_reqd = TRUE;
      access.access_method = MMGSDI_EF_ENUM_ACCESS;
      access.file.file_enum = MMGSDI_CDMA_IMSI_T;
      (void)mmgsdi_file_set_uim_path(&access, &recovery_data_ptr->rehab_data[rehab_data_index].path);
      rehab_data_index++;
    }

    /* TMSI rehabilitation Data */
    if(rehab_info_ptr->cdma_fdn_files_rehab[2])
    {
      UIM_MSG_HIGH_0("Recovery, TMSI Rehab required");

      recovery_data_ptr->rehab_data[rehab_data_index].rehab_reqd = TRUE;
      access.access_method = MMGSDI_EF_ENUM_ACCESS;
      access.file.file_enum = MMGSDI_CDMA_TMSI;
      (void)mmgsdi_file_set_uim_path(&access, &recovery_data_ptr->rehab_data[rehab_data_index].path);
      rehab_data_index++;
    }
#endif /* FEATURE_MMGSDI_3GPP2 */
  }

   /* Update recovery data with channel, App and PIN info */
  for(channel_index = 0;
      ((channel_index < MMGSDI_MAX_CHANNEL_INFO) &&
       (recovery_data_ptr->num_channels <
        (uim_channel_type)UIM_MAX_CHANNELS));
      channel_index++)
  {
    if (mmgsdi_channel_info_ptr_table[channel_index] == NULL)
    {
      continue;
    }
    /*On ICC cards only provisioning sessions shall be recovered (which are
      always open on the default channel) and all non-prov. sessions shall not
      be recovered*/
    if(protocol == MMGSDI_ICC)
    {
      if(recovery_data_ptr->num_channels == 1)
      {
        break;
      }
      if(mmgsdi_channel_info_ptr_table[channel_index]->channel_id !=
         MMGSDI_DEFAULT_CHANNEL)
      {
        UIM_MSG_HIGH_1("Channel id 0x%x will not be recovered",
                       mmgsdi_channel_info_ptr_table[channel_index]->channel_id);
        continue;
      }
    }

    /* If channel is open on the slot being recovered.. */
    if(mmgsdi_channel_info_ptr_table[channel_index]->slot_id == slot_id)
    {
      /* Convert the channel ID to UIM format */
      mmgsdi_status = mmgsdi_util_convert_uim_channel(
                        mmgsdi_channel_info_ptr_table[channel_index]->channel_id,
                        &channel_id);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        continue;
      }

      if(recovery_data_ptr->app_data[channel_id].is_ch_valid)
      {
        UIM_MSG_ERR_1("Index: 0x%x already contains valid data", channel_id);
        continue;
      }

      UIM_MSG_HIGH_1("Recovery, Channel ID: 0x%x", channel_id);

      /* Store the channel ID of the app to be recovered */
      recovery_data_ptr->app_data[channel_id].channel = channel_id;

      /* Increment count indicating number of opened channels */
      recovery_data_ptr->num_channels++;

      /* Indicate that the channel/app info contained is valid */
      recovery_data_ptr->app_data[channel_id].is_ch_valid = TRUE;

      /*Channels which are opened with only non-prov. sessions and where APDU
        streaming has been done will not be recovered. All other channels will
        be recovered*/
      if(mmgsdi_util_is_channel_shared_with_prov_session(
           mmgsdi_channel_info_ptr_table[channel_index]->channel_id, slot_id) == FALSE)
      {
        channel_state = mmgsdi_channel_info_ptr_table[channel_index]->channel_state;
        /*MMGSDI does not know the state of the App where APDU streaming has
          been done so it can not restore the App to the same state, thus
          recovery of associated channel will not be done*/
        switch(channel_state)
        {
          case MMGSDI_CHANNEL_STATE_NONE:
          case MMGSDI_CHANNEL_STATE_NON_PROV_ACT_IN_PROG:
          case MMGSDI_CHANNEL_STATE_NON_PROV_DEACT_IN_PROG:
          case MMGSDI_CHANNEL_STATE_READY:
            recovery_data_ptr->app_data[channel_id].do_not_recover = FALSE;
            break;
          case MMGSDI_CHANNEL_STATE_APDU_STREAMING_DONE:
          default:
            recovery_data_ptr->app_data[channel_id].do_not_recover = TRUE;
            UIM_MSG_HIGH_1("Do not recover channel id 0x%x as APDU streaming is done",
                           channel_id);
            continue;
        }
      }
      else
      {
        recovery_data_ptr->app_data[channel_id].do_not_recover = FALSE;
      }

      /* Get the app. info of the application on the channel to be recovered */
      app_info_ptr = mmgsdi_channel_info_ptr_table[channel_index]->app_info_ptr;

       /* If no application is active on the channel, there is no
          more info to be recovered. Skip to next channel table entry */
      if(app_info_ptr == NULL)
      {
        UIM_MSG_HIGH_1("No Application active on channel 0x%x",
                       recovery_data_ptr->app_data[channel_id].channel);

        continue;
      }

      /*If app is USIM OR SIM, Fill EF-TST data in recovery data*/
      if(app_info_ptr->app_data.app_type == MMGSDI_APP_SIM || 
         app_info_ptr->app_data.app_type == MMGSDI_APP_USIM)
      {
        mmgsdi_session_id_type  session_id  = MMGSDI_INVALID_SESSION_ID;

        mmgsdi_status = mmgsdi_util_get_prov_session_id_for_app(
                          slot_id,
                          &(app_info_ptr->app_data),
                          &session_id);

        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          mmgsdi_len_type tst_datalen = 0;

          access.access_method  = MMGSDI_EF_ENUM_ACCESS;
          if(app_info_ptr->app_data.app_type == MMGSDI_APP_USIM)
          {
            access.file.file_enum = MMGSDI_USIM_7F66_PROP1_SPT_TABLE;
          }
          else
          {
            access.file.file_enum = MMGSDI_SIM_7F66_PROP1_SPT_TABLE;
          }
          mmgsdi_status = mmgsdi_cache_read_len(
                            session_id,
                            &access,
                            &tst_datalen,
                            0,
                            NULL);

          if(mmgsdi_status == MMGSDI_SUCCESS && tst_datalen > 0)
          {
            mmgsdi_data_type mmgsdi_tst_data;

            if(tst_datalen > UIM_EF_TST_MAX_LEN)
            {
              tst_datalen = UIM_EF_TST_MAX_LEN;
            }
            memset(&mmgsdi_tst_data, 0x00, sizeof(mmgsdi_data_type));
            mmgsdi_tst_data.data_len = tst_datalen;
            mmgsdi_tst_data.data_ptr = recovery_data_ptr->app_data[channel_id].ef_tst_data.data;

              mmgsdi_status = mmgsdi_cache_read(
                                session_id,
                                &access,
                                &mmgsdi_tst_data,
                                0,
                                NULL);
            if(mmgsdi_status == MMGSDI_SUCCESS)
            {
              recovery_data_ptr->app_data[channel_id].ef_tst_data.length = (uint8)tst_datalen;
            }
          }
        }
      }

      /* If card prtocol is UICC, retrieve AID and UPIN data */
      if(protocol == MMGSDI_UICC)
      {
         /* Invalid AID, skip to next channel table entry */
        if ((app_info_ptr->app_data.aid.data_len > UIM_AID_MAX_NAME_LENGTH) ||
            (app_info_ptr->app_data.aid.data_len < 0))
        {
          UIM_MSG_ERR_1("Invalid AID on channel 0x%x",
                        recovery_data_ptr->app_data[channel_id].channel);
          recovery_data_ptr->app_data[channel_id].is_aid_valid = FALSE;
          continue;
        }

        UIM_MSG_HIGH_2("Recovery, App.activation needed on Channel: 0x%x, select rsp type: 0x%x",
                       channel_id,
                       mmgsdi_channel_info_ptr_table[channel_index]->select_rsp_type);

        recovery_data_ptr->app_data[channel_id].is_aid_valid = TRUE;

        if((app_info_ptr->app_data.app_type == MMGSDI_APP_USIM) ||
           (app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM))
        {
          /* In case of USIM/CSIM, the status that the app is initialized
             is sent by the MMGSDI.
             In this case, we check both the app state and the
             status_app_is_initialized value. This is because, in case the
             recovery happens after the CARD_STATUS request is queued but
             before it is processed, we do not want to indicate to drivers
             to send the CARD_STATUS request */
          if ((app_info_ptr->app_state == MMGSDI_APP_STATE_READY) &&
              (app_info_ptr->status_app_is_initialized))
          {
            recovery_data_ptr->app_data[channel_id].send_status = TRUE;
          }
        }
        else if (app_info_ptr->app_data.app_type == MMGSDI_APP_UNKNOWN)
        {
          /* In this case (e.g. ISIM app), the status that the app is initialized
             is sent by the client. The status_app_is_initialized indicates if
             a client has done so */
          if(app_info_ptr->status_app_is_initialized)
          {
            recovery_data_ptr->app_data[channel_id].send_status = TRUE;
          }
        }

        UIM_MSG_MED_1("UIM DRV send status 0x%x",
                 recovery_data_ptr->app_data[channel_id].send_status);

        /* Update AID data in the recovery info */
        if(app_info_ptr->app_data.aid.data_len > 0)
        {
          mmgsdi_memscpy(recovery_data_ptr->app_data[channel_id].aid.data,
            sizeof(recovery_data_ptr->app_data[channel_id].aid.data),
            app_info_ptr->app_data.aid.data_ptr,
            int32touint32(app_info_ptr->app_data.aid.data_len));

          recovery_data_ptr->app_data[channel_id].aid.aid_length =
            int32touint8(app_info_ptr->app_data.aid.data_len);
        }

        /*Update select response type*/
        switch(mmgsdi_channel_info_ptr_table[channel_index]->select_rsp_type)
        {
          case MMGSDI_SELECT_RSP_NO_DATA:
            recovery_data_ptr->app_data[channel_id].session_control =
              UIM_ACTIVATION_WITH_NO_DATA;
            break;
          case MMGSDI_SELECT_RSP_FMD:
            recovery_data_ptr->app_data[channel_id].session_control =
              UIM_ACTIVATION_WITH_FMD;
            break;
          case MMGSDI_SELECT_RSP_FCI:
            recovery_data_ptr->app_data[channel_id].session_control =
              UIM_ACTIVATION_WITH_FCI;
            break;
          case MMGSDI_SELECT_RSP_FCI_WITH_INTERFACES:
            recovery_data_ptr->app_data[channel_id].session_control =
              UIM_ACTIVATION_WITH_FCI_INTERFACES;
            break;
          case MMGSDI_SELECT_RSP_FCP:
          default:
            recovery_data_ptr->app_data[channel_id].session_control =
              UIM_ACTIVATION_OR_RESET;
            break;
        }

        /* Update UPIN data in the recovery info if available */
        if((!upin_info_done) &&
           (app_info_ptr->universal_pin_ptr != NULL) &&
           (app_info_ptr->universal_pin_ptr->status == MMGSDI_PIN_ENABLED_VERIFIED) &&
           (app_info_ptr->universal_pin_ptr->pin_data.data_len != 0))
        {
          UIM_MSG_HIGH_0("Recovery, UPIN verification needed");

          /* Update Key Ref. */
          recovery_data_ptr->upin_data.chv =
            mmgsdi_util_convert_to_uim_chv(app_info_ptr->universal_pin_ptr->key_ref);

           /* Copy PIN data */
          mmgsdi_memscpy(recovery_data_ptr->upin_data.chv_digits,
            sizeof(recovery_data_ptr->upin_data.chv_digits),
            app_info_ptr->universal_pin_ptr->pin_data.data_ptr,
            int32touint32(app_info_ptr->universal_pin_ptr->pin_data.data_len));

          upin_info_done = TRUE;
        }
      }

      /* Update PIN1 data in the recovery info if available */
      if((app_info_ptr->pin1_ptr != NULL) &&
         (app_info_ptr->pin1_ptr->status == MMGSDI_PIN_ENABLED_VERIFIED) &&
         (app_info_ptr->pin1_ptr->pin_data.data_len != 0))
      {
        uim_chv_type temp_chv       = UIM_CHV_MAX;
        uint8        temp_ch_index  = 0;
        boolean      found          = FALSE;

        /* Retrieve Key ref. */
        temp_chv =
          mmgsdi_util_convert_to_uim_chv(app_info_ptr->pin1_ptr->key_ref);

        /* Using Key ref., verify if the current PIN1 data has already
           been updated in the recovery data for another application
           (occurs when application share the same PIN1 Key ref.).
           In this case, do not update PIN data again, since a
           a PIN Key ref. needs to be verified only once */
        for(temp_ch_index = 0;
            temp_ch_index < (uim_channel_type)UIM_MAX_CHANNELS;
            temp_ch_index++)
        {
          if((recovery_data_ptr->app_data[temp_ch_index].is_ch_valid) &&
             (recovery_data_ptr->app_data[temp_ch_index].pin1_data.chv == temp_chv))
          {
            found = TRUE;
            break;
          }
        }

        /* If current PIN1 key. ref. data is not present in recovery info,
           update the same */
        if(!found)
        {
          UIM_MSG_HIGH_1("Recovery, PIN1 verification needed, Key Ref.: 0x%x",
                         temp_chv);

          /* Update Key Ref. */
          recovery_data_ptr->app_data[channel_id].pin1_data.chv = temp_chv;

          /* Copy PIN data */
          mmgsdi_memscpy(recovery_data_ptr->app_data[channel_id].pin1_data.chv_digits,
            sizeof(recovery_data_ptr->app_data[channel_id].pin1_data.chv_digits),
            app_info_ptr->pin1_ptr->pin_data.data_ptr,
            int32touint32(app_info_ptr->pin1_ptr->pin_data.data_len));
        }
      }

      /* Update PIN2 data in the recovery info if available */
      if((app_info_ptr->pin2.status == MMGSDI_PIN_ENABLED_VERIFIED) &&
         (app_info_ptr->pin2.pin_data.data_len != 0))
      {
        UIM_MSG_HIGH_1("Recovery, PIN2 verification needed, Key Ref.: 0x%x",
                       app_info_ptr->pin2.key_ref);

        /* Update Key Ref. */
        recovery_data_ptr->app_data[channel_id].pin2_data.chv =
          mmgsdi_util_convert_to_uim_chv(app_info_ptr->pin2.key_ref);

        /* Copy PIN data */
        mmgsdi_memscpy(recovery_data_ptr->app_data[channel_id].pin2_data.chv_digits,
          sizeof(recovery_data_ptr->app_data[channel_id].pin2_data.chv_digits),
          app_info_ptr->pin2.pin_data.data_ptr,
          int32touint32(app_info_ptr->pin2.pin_data.data_len));
      }
    }
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  UIM_MSG_HIGH_1("Recovery, Num. channels: 0x%x", recovery_data_ptr->num_channels);

  (void)mmgsdi_build_recovery_ind_req(slot_id, protocol);

  return MMGSDI_SUCCESS;
} /* mmgsdi_get_recovery_data_sync */


/*============================================================================
   FUNCTION:      MMGSDI_NOTIFY_SIM_BUSY

   DESCRIPTION:
     Called by UIM Drivers to inform MMGSDI that the SIM card has been busy
     for a long period while processing a command.  UIM drivers is currently
     unable to send/receive APDUs but expects normal operation to resume.
     MMGSDI must inform its clients so they can take appropiate action to
     cope until normal sim card operations resume.

   PARAMETERS:
     bool          sim_busy :  True  => sim is currently busy
                               False => normal operations resumed
     uim_slot_type slot     :  Affected slot

   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task should invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

       MMGSDI_SUCCESS:          The command structure was properly generated
                                and queued onto the MMGSDI Command Queue.
       MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                                within appropriate ranges.
       MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                                because the max number of commands are already
                                queued.
       MMGSDI_ERROR:            Generic error.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_notify_sim_busy(
  boolean        sim_busy,
  uim_slot_type  slot
)
{
  mmgsdi_task_cmd_type                * task_cmd_ptr  = NULL;
  mmgsdi_notify_sim_busy_req_type     * msg_ptr       = NULL;
  int32                                 total_mem_len = 0;
  uint32                                task_cmd_len  = 0;
  mmgsdi_slot_id_enum_type              slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_return_enum_type               mmgsdi_status = MMGSDI_ERROR;

  UIM_MSG_HIGH_2("In mmgsdi_notify_sim_busy, Slot: 0x%x, SIM Busy: 0x%x",
                 slot, sim_busy);

  /* Validate the slot */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(slot,
                                    UIM_SLOT_1,
                                    UIM_SLOT_3);

  /* Convert slot type from UIM Driver type to MMGSDI type */
  mmgsdi_status = mmgsdi_util_convert_from_uim_slot(slot, &slot_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Allocate memory for the MMGSDI Task Command */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len  = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* No need to Align pointers of the structures members */
  msg_ptr = &task_cmd_ptr->cmd.cmd.sim_busy_req;

  /*---------------------------------------------------------------------------
    Populating card_error_req header
      1) Client ID
      2) Request type
      3) Slot ID
      4) Client Data
      5) Response CallBack
      6) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_NOTIFY_SIM_BUSY_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.request_type      = MMGSDI_NOTIFY_SIM_BUSY_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_NOTIFY_SIM_BUSY_REQ;
  msg_ptr->request_header.slot_id           = slot_id;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = mmgsdi_util_internal_cb;

  /*---------------------------------------------------------------------------
    Populating the mmgsdi_notify_sim_busy_req_type content

  ---------------------------------------------------------------------------*/
  msg_ptr->sim_busy = sim_busy;

  /* --------------------------------------------------------------------------
     Calculate the payload len and request len.
     ----------------------------------------------------------------------- */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
         - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Always put Sim Busy on mmgsdi command queue for the main task.
     This ensures it will be processed even if the slot specific task is stuck.
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_cmd_ext(task_cmd_ptr, MMGSDI_TASK_MAIN);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the
    ** command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_NOTIFY_SIM_BUSY_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_notify_sim_busy */


/*============================================================================
   FUNCTION:      MMGSDI_NOTIFY_LINK_EST_SLOT

   DESCRIPTION:
     Notifies MMGSDI of card detection and provides all information about
     the card on a slot.
     The API shall be called by UIM after a card has been detected during
     inital power-up, due to a hotswap or as a result of a card RESET command.

   PARAMETERS:
   uim_link_established_data_type link_est_data:  Input info on the detected
                                                  card on a slot
   uim_slot_type:                                 slot_id

   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task can invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
   MMGSDI_ERROR:            Generic error.


   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_notify_link_est_slot(
  uim_slot_link_established_data_type link_est_data,
  uim_slot_type                       slot_id
)
{
  mmgsdi_task_cmd_type              * task_cmd_ptr   = NULL;
  mmgsdi_notify_link_est_req_type   * msg_ptr        = NULL;
  int32                               total_mem_len  = 0;
  uint32                              task_cmd_len   = 0;
  mmgsdi_return_enum_type             mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type            mmgsdi_slot_id = MMGSDI_SLOT_AUTOMATIC;
  mmgsdi_task_enum_type               mmgsdi_task    = MMGSDI_TASK_MAX;

  UIM_MSG_HIGH_1("In mmgsdi_notify_link_est, slot 0x%x", slot_id);

  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len  = int32touint32(total_mem_len);

  mmgsdi_status = mmgsdi_util_convert_from_uim_slot(slot_id, &mmgsdi_slot_id);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* In this case, link_est is processed in main task */
    UIM_MSG_HIGH_1("Unexpected slot, 0x%x", mmgsdi_slot_id);
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* --------------------------------------------------------------------------
    No need to Align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.link_est_req;

  /*---------------------------------------------------------------------------
    Populating card_error_req header
      1) Client ID
      2) Request type
      3) Client Data
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_NOTIFY_LINK_EST_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.request_type      = MMGSDI_NOTIFY_LINK_EST_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_NOTIFY_LINK_EST_REQ;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = mmgsdi_util_internal_cb;
  msg_ptr->request_header.slot_id           = mmgsdi_slot_id;

  /*---------------------------------------------------------------------------
    Populating link_est_req content
  ---------------------------------------------------------------------------*/
  /* Indicate that card is present on slot */
  msg_ptr->link_est_data.card_present = TRUE;

  /* Copy DF-CDMA presence status */
  msg_ptr->link_est_data.cdma_df_present = link_est_data.cdma_df_present;

  /* Copy DF-GSM presence status */
  msg_ptr->link_est_data.gsm_df_present = link_est_data.gsm_df_present;

  /* Find out if its an euicc or not (based on the ATR) */
  msg_ptr->link_est_data.is_euicc = link_est_data.is_euicc;

  /* Convert UIM protocol type to MMGSDI protocol tpye and copy */
  (void)mmgsdi_util_convert_from_uim_protocol(
          link_est_data.protocol,
          &msg_ptr->link_est_data.protocol);

  /* Copy ATR data */
  if(link_est_data.atr_buf.num_bytes <= MMGSDI_MAX_ATR_LEN)
  {
    msg_ptr->link_est_data.atr.atr_length = link_est_data.atr_buf.num_bytes;
  }
  else
  {
    msg_ptr->link_est_data.atr.atr_length = MMGSDI_MAX_ATR_LEN;
  }

  mmgsdi_memscpy(msg_ptr->link_est_data.atr.atr_data,
                 sizeof(msg_ptr->link_est_data.atr.atr_data),
                 link_est_data.atr_buf.data,
                 msg_ptr->link_est_data.atr.atr_length);

  /* Copy History characters */
  if(link_est_data.hist_char_buf.length <=
     MMGSDI_MAX_HIST_CHAR_LEN)
  {
    msg_ptr->link_est_data.hist_char.hist_char_length =
      link_est_data.hist_char_buf.length;
  }
  else
  {
    msg_ptr->link_est_data.hist_char.hist_char_length =
      MMGSDI_MAX_HIST_CHAR_LEN;
  }

  mmgsdi_memscpy(msg_ptr->link_est_data.hist_char.hist_char_data,
                 sizeof(msg_ptr->link_est_data.hist_char.hist_char_data),
                 link_est_data.hist_char_buf.data,
                 msg_ptr->link_est_data.hist_char.hist_char_length);

  /* Copy Work waiting time value */
  msg_ptr->link_est_data.work_waiting_time =
    link_est_data.work_waiting_time;

  /* Copy ICCID data */
  if(link_est_data.iccid_buf.num_bytes <=
     MMGSDI_ICCID_LEN)
  {
    msg_ptr->link_est_data.iccid.length =
      link_est_data.iccid_buf.num_bytes;
  }
  else
  {
    msg_ptr->link_est_data.iccid.length =
      MMGSDI_ICCID_LEN;
  }

  if(msg_ptr->link_est_data.iccid.length > 0)
  {
    mmgsdi_memscpy(msg_ptr->link_est_data.iccid.data,
                   sizeof(msg_ptr->link_est_data.iccid.data),
                   link_est_data.iccid_buf.data,
                   msg_ptr->link_est_data.iccid.length);
  }

  /* --------------------------------------------------------------------------
     Calculate the payload len and request len.
     ----------------------------------------------------------------------- */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
         - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_task = mmgsdi_util_get_task_from_slot(mmgsdi_slot_id);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the task is undetermined,
       default to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }

  mmgsdi_status = mmgsdi_cmd_ext(task_cmd_ptr, mmgsdi_task);
  UIM_MSG_HIGH_1("Queue of MMGSDI_NOTIFY_LINK_EST_REQ command, status: 0x%x",
                 mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the
    ** command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }
  return mmgsdi_status;
} /* mmgsdi_notify_link_est_slot */


/*============================================================================
   FUNCTION:      MMGSDI_NOTIFY_LINK_EST

   DESCRIPTION:
     Notifies MMGSDI of card detection and provides all information about
     the Cards.
     The API shall be called by UIM after cards have been detected during
     inital power-up, due to a hotswap or as a result of a card RESET command.

   PARAMETERS:
   uim_link_established_data_type link_est_data:  Input.  Info on the detected
                                                          cards.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task can invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
   MMGSDI_ERROR:            Generic error.


   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_notify_link_est(
  uim_link_established_data_type link_est_data
)
{
  uint8                               slot_index    = 0;
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_ERROR;


  /*---------------------------------------------------------------------------
    Populating link_est_req content
  ---------------------------------------------------------------------------*/
  for(slot_index = 0; slot_index < UIM_MAX_NUM_SLOTS; slot_index++)
  {
    if(link_est_data.slot_data[slot_index].slot_status != UIM_SLOT_PRESENT)
    {
      continue;
    }

    mmgsdi_status = mmgsdi_notify_link_est_slot(link_est_data.slot_data[slot_index],
                                                (uim_slot_type)(slot_index + 1));
  }

  return mmgsdi_status;
} /* mmgsdi_notify_link_est */


/*============================================================================
   FUNCTION:      MMGSDI_NOTIFY_CARD_ERROR

   DESCRIPTION:
     Notifies MMGSDI of any Card Error and the cause.

   PARAMETERS:
   uim_status_type error_condition:  Input.  Card Error cause.

   uim_slot_type   slot:             Input.  Card slot on which error
                                             has occurred.

   boolean         temporary_error:  Input.  Temporary or permanent
                                             Card error

   DEPENDENCIES:
     None

   LIMITATIONS:
     Only UIM task can invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
   MMGSDI_ERROR:            Generic error.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_notify_card_error(
  uim_status_type error_condition,
  uim_slot_type   slot,
  boolean         temporary_error
)
{
  mmgsdi_task_cmd_type                * task_cmd_ptr  = NULL;
  mmgsdi_notify_card_error_req_type   * msg_ptr       = NULL;
  int32                                 total_mem_len = 0;
  uint32                                task_cmd_len  = 0;
  mmgsdi_slot_id_enum_type              slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_return_enum_type               mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_task_enum_type                 mmgsdi_task   = MMGSDI_TASK_MAX;

  UIM_MSG_HIGH_2("In mmgsdi_notify_card_error, Slot: 0x%x, Error: 0x%x",
                 slot, error_condition);

  /* --------------------------------------------------------------------------
     Validate slot ID
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(slot,
                                    UIM_SLOT_1,
                                    UIM_SLOT_3);

  /* --------------------------------------------------------------------------
     Convert UIM slot type to MMGSDI slot type
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_convert_from_uim_slot(slot, &slot_id);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len  = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* --------------------------------------------------------------------------
    No need to Align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.card_error_req;

  /*---------------------------------------------------------------------------
    Populating card_error_req header
      1) Client ID
      2) Request type
      3) Slot ID
      4) Client Data
      5) Response CallBack
      6) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_NOTIFY_CARD_ERROR_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.request_type      = MMGSDI_NOTIFY_CARD_ERROR_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_NOTIFY_CARD_ERROR_REQ;
  msg_ptr->request_header.slot_id           = slot_id;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = mmgsdi_util_internal_cb;

  /*---------------------------------------------------------------------------
    Populating the mmgsdi_notify_card_error_req_type content
    - error_condition
  ---------------------------------------------------------------------------*/
  msg_ptr->error_condition = mmgsdi_util_convert_from_uim_error_condition(
                               error_condition);
  msg_ptr->temporary_error = temporary_error;

  /* --------------------------------------------------------------------------
     Calculate the payload len and request len.
     ----------------------------------------------------------------------- */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
         - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_task = mmgsdi_util_get_task_from_slot(slot_id);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the slot specific task is undetermined
       default to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }
  mmgsdi_status = mmgsdi_cmd_ext(task_cmd_ptr, mmgsdi_task);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the
    ** command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_NOTIFY_CARD_ERROR_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_notify_card_error */


/*================================================================
FUNCTION  MMGSDI_RPM_HAS_ICCID_CHANGED

DESCRIPTION:
  This function is called to determine if the ICCID has changed
  since the previous card inserted

  INPUT PARAMETERS:
    mmgsdi_slot: Slot to check
    status_ptr : out param for whether the ICCID has changed or not


DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          status is:
                             TRUE : if the ICCID has changed
                             FALSE: if the ICCID is the same
                                    as the last card inserted
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.

SIDE EFFECTS:
  status_ptr is updated
===============================================================*/
mmgsdi_return_enum_type mmgsdi_rpm_has_iccid_changed
(
  mmgsdi_slot_id_enum_type            mmgsdi_slot,
  boolean                             *status_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(status_ptr);

  switch(mmgsdi_slot)
  {
    case MMGSDI_SLOT_1:
    case MMGSDI_SLOT_2:
    case MMGSDI_SLOT_3:
      *status_ptr = mmgsdi_nv_get_rpm_iccid_has_changed(mmgsdi_slot);
      return MMGSDI_SUCCESS;
    default:
      break;
  }

  return MMGSDI_INCORRECT_PARAMS;
} /* mmgsdi_rpm_has_iccid_changed */


/* ============================================================================
   FUNCTION       MMGSDI_GET_PROTOCOL

   DESCRIPTION
     This function will request protocol for the slot.
     The client calling this function expects an instant response.

   PARAMETERS:
     client_id:         Client ID of the requesting client.
     protocol_ptr:      Buffer to store the protocol information.

   DEPENDENCIES:
     A valid Client ID is required, which can be achieved by calling
     mmgsdi_client_id_and_evt_reg().

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_INIT:         Called before CARD_INSERTED on either slot

   SIDE EFFECTS:
     None
===============================================================================*/
mmgsdi_return_enum_type mmgsdi_get_protocol (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot,
  mmgsdi_protocol_enum_type        * protocol_ptr
)
{
  mmgsdi_slot_data_type            *slot_data_ptr   = NULL;

  (void) client_id;

  /*---------------------------------------------------------------------------
      Input Parameters Check
      1) NULL check for available_apps_ptr
    ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(protocol_ptr);

  /* Protect access to the CLient ID reg. table/App table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr == NULL)
  {
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    return MMGSDI_ERROR;
  }

  *protocol_ptr = mmgsdi_util_determine_protocol(slot);

  if(*protocol_ptr == MMGSDI_NO_PROTOCOL ||
     slot_data_ptr->mmgsdi_state == MMGSDI_STATE_NO_CARD)
  {
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    UIM_MSG_ERR_2("protocol 0x%x, mmgsdi_state 0x%x",
                  *protocol_ptr, slot_data_ptr->mmgsdi_state);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return MMGSDI_SUCCESS;
} /* mmgsdi_get_protocol */


/*================================================================
FUNCTION  MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED

DESCRIPTION:
  This function is called by gstk to indicate that a response
  to VPE has been received from the card with status 90 xx.
  If card has responded to VPE with an IMSI switch, an internal
  refresh will be triggered by MMGSDI if MMGSDI has a stale value
  of IMSI in its cache.
  Caller should not expect any response from the API as such.

  INPUT PARAMETERS:
    slot_id:          Slot where the card is question exists
    location_status:  Location status
    mcc:              MCC
    mnc:              MNC


DEPENDENCIES:
  None

LIMITATIONS:
  The request will go through ONLY IF this is the first time GSTK
  called this API since card inserted, that is, if the API is
  called by GSTK in response to the FIRST VPE response (90 xx)
  from the card.

RETURN VALUE:
  None

SIDE EFFECTS:
  MMGSDI's global count for the VPEs is incremented everytime the
  function is called.
  INIT_FFCN Refresh is triggered if IMSI change is reported by the
  card.
===============================================================*/
void mmgsdi_gstk_loc_env_rsp_received
(
  mmgsdi_slot_id_enum_type            slot_id,
  mmgsdi_location_status_enum_type    loc_status,
  uint16                              mcc,
  uint16                              mnc,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
  mmgsdi_task_cmd_type                       *task_cmd_ptr   = NULL;
  mmgsdi_gstk_loc_env_rsp_received_req_type  *msg_ptr        = NULL;
  int32                                       total_mem_len  = 0;
  uint32                                      task_cmd_len   = 0;
  mmgsdi_return_enum_type                     mmgsdi_status  = MMGSDI_ERROR;

  /* loc_status, mcc and mnc are reserved for future use */
  (void) loc_status;
  (void) mcc;
  (void) mnc;

 /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.gstk_loc_env_rsp_received_req;

  /* Populating mmgsdi_gstk_loc_env_rsp_received_req_type header
      1) Client ID
      2) Request TYPE
      3) Slot ID
      4) Client Data Pointer
      5) Response CallBack
      6) Event Callback
      7) Payload len and request len will be populated after the content
         has been populated */

  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.session_id        = 0;
  msg_ptr->request_header.slot_id           = slot_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Calculate the payload len and request len */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
    total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ status 0x%x",
                 mmgsdi_status);

} /* mmgsdi_gstk_loc_env_rsp_received */


/*====================================================================
FUNCTION        MMGSDI_GET_SE13_PLMN_NAMES

DESCRIPTION
  This API is used to get plmn names by directly accessing SE13 table for
  given set of PLMN Ids.

  Upon successful completion of this function, MMGSDI sends a confirmation to
  client with PLMN name list.

PARAMETERS
  mmgsdi_client_id_type    client_id:           Client ID
  mmgsdi_plmnId_list_type  plmn_id_list:        list of PLMN id
  mmgsdi_callback_type     response_cb_ptr:     Response callback
  mmgsdi_client_data_type  client_ref:          User Data returned upon
                                                completion of this cmd.

DEPENDENCIES
  The Client ID specified must be valid.

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not queued to the MMGSDI Task
                             because the max number of commands are already queued.

SIDE EFFECTS
  None
====================================================================== */
mmgsdi_return_enum_type mmgsdi_get_se13_plmn_names(
  mmgsdi_client_id_type              client_id,
  mmgsdi_plmn_id_list_type           plmn_id_list,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                *task_cmd_ptr   = NULL;
  mmgsdi_get_se13_plmn_names_req_type *msg_ptr        = NULL;
  mmgsdi_return_enum_type              mmgsdi_status  = MMGSDI_SUCCESS;
  int32                                var_len        = 0;
  int32                                total_mem_len  = 0;
  int32                                temp_mem       = 0;
  uint32                               task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) plmn list ptr != NULL checl
    4) num of plmn ids > 0 Check
    5) word aligned variable check
    6) Memory allocation for task_cmd_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_GET_SE13_PLMN_NAMES_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /*return INCORRECT PARAM error when API is called with
    Number of plmn ids 0 or plmn list ptr is NULL*/
  if(plmn_id_list.plmn_list_ptr == NULL || plmn_id_list.num_of_plmn_ids == 0)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len = plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len,&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len,temp_mem);

  total_mem_len += temp_mem;

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.get_se13_plmn_names_req,
                         MMGSDI_GET_SE13_PLMN_NAMES_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.get_se13_plmn_names_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_get_se13_plmn_names_req_type header
      1) Client ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GET_SE13_PLMN_NAMES_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_GET_SE13_PLMN_NAMES_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GET_SE13_PLMN_NAMES_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->plmn_id_list.num_of_plmn_ids = plmn_id_list.num_of_plmn_ids;
  mmgsdi_memscpy(msg_ptr->plmn_id_list.plmn_list_ptr,
                 plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type),
                 plmn_id_list.plmn_list_ptr,
                 plmn_id_list.num_of_plmn_ids * sizeof(mmgsdi_plmn_id_type));

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_SE13_PLMN_NAMES_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_get_se13_plmn_names */


/*====================================================================
FUNCTION MMGSDI_GET_SE13_PLMN_INFO_BY_NAME

DESCRIPTION
  This API is used to search PLMN long name/short name in SE13 table.

  Upon successful completion of this function, MMGSDI sends a confirmation to
  client with MCC, MNC, RAT type, PLMN long name and PLMN short name.

PARAMETERS
  mmgsdi_client_id_type    client_id:           Client ID
  mmgsdi_data_type         plmn_long_name:      plmn long name ptr and length
  mmgsdi_data_type         plmn_short_name:     plmn short name ptr and length
  mmgsdi_rat_enum_type     rat_type             RAT type, if valid RAT type is
                                                passed MMGSDI returns entries
                                                matching the RAT.
  mmgsdi_callback_type     response_cb_ptr:     Response callback
  mmgsdi_client_data_type  client_ref:          User Data returned upon
                                                completion of this cmd.

DEPENDENCIES
  The Client ID specified must be valid.
  PLMN name string must be in Default 7 bit coded alphabet encoding.

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not queued to the MMGSDI Task
                             because the max number of commands are already queued.

SIDE EFFECTS
  None
====================================================================== */
mmgsdi_return_enum_type mmgsdi_get_se13_plmn_info_by_name(
  mmgsdi_client_id_type              client_id,
  mmgsdi_data_type                   plmn_long_name,
  mmgsdi_data_type                   plmn_short_name,
  mmgsdi_rat_enum_type               rat_type,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                       *task_cmd_ptr   = NULL;
  mmgsdi_get_se13_plmn_info_by_name_req_type *msg_ptr        = NULL;
  mmgsdi_return_enum_type                     mmgsdi_status  = MMGSDI_SUCCESS;
  int32                                       var_len        = 0;
  int32                                       total_mem_len  = 0;
  int32                                       temp_mem       = 0;
  uint32                                      task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) plmn long name validity check
    4) plmn short name validity Check
    5) word aligned variable check
    6) Memory allocation for task_cmd_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(rat_type,
                                    MMGSDI_RAT_NONE,
                                    MMGSDI_RAT_TDS);

  /*return INCORRECT PARAM error when API is called with
    Number of plmn ids 0 or plmn list ptr is NULL*/
  if((plmn_long_name.data_ptr != NULL &&
       (plmn_long_name.data_len <= 0 ||
        plmn_long_name.data_len > MMGSDI_EONS_MAX_PLMN_NAME_LEN)) ||
     (plmn_long_name.data_len > 0 && plmn_long_name.data_ptr == NULL) ||
     (plmn_short_name.data_ptr != NULL &&
       (plmn_short_name.data_len <= 0 ||
        plmn_short_name.data_len > MMGSDI_EONS_MAX_PLMN_NAME_LEN)) ||
     (plmn_short_name.data_len > 0 && plmn_short_name.data_ptr == NULL) ||
     (plmn_long_name.data_ptr == NULL && plmn_short_name.data_ptr == NULL))
  {
    UIM_MSG_ERR_3("Invalid params: long name len = 0x%x long name ptr = 0x%x short name ptr = 0x%x",
                  plmn_long_name.data_len,
                  plmn_long_name.data_ptr,
                  plmn_short_name.data_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len = plmn_long_name.data_len + plmn_short_name.data_len;

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len,&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len,temp_mem);

  total_mem_len += temp_mem;

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.get_se13_plmn_info_by_name_req,
                         MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.get_se13_plmn_info_by_name_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_get_se13_plmn_info_by_name_req_type header
      1) Client ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->rat_type = rat_type;
  msg_ptr->plmn_long_name.data_len = plmn_long_name.data_len;
  msg_ptr->plmn_short_name.data_len = plmn_short_name.data_len;

  if(plmn_long_name.data_len > 0)
  {
    mmgsdi_memscpy(msg_ptr->plmn_long_name.data_ptr,
                   plmn_long_name.data_len,
                   plmn_long_name.data_ptr,
                   plmn_long_name.data_len);
  }

  if(plmn_short_name.data_len > 0)
  {
    mmgsdi_memscpy(msg_ptr->plmn_short_name.data_ptr,
                   plmn_short_name.data_len,
                   plmn_short_name.data_ptr,
                   plmn_short_name.data_len);
  }

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_get_se13_plmn_info_by_name */


/* ============================================================================
FUNCTION MMGSDI_GET_SE13_NETWORK_INFO_BY_INDEX_SYNC

DESCRIPTION
  This function will return PLMN info retrieved from SE13 table at given index.

PARAMETERS:
 client_id:             Client ID of the requesting client.
 Index:                 table index
 se13_network_info_ptr: out param, se13 network info ptr

DEPENDENCIES:
  A valid Client ID is required, which can be achieved by calling
  mmgsdi_client_id_and_evt_reg()

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS:          successful retrieval of PLMN name
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           with in appropriate ranges.
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_get_se13_network_info_by_index_sync(
  mmgsdi_client_id_type            client_id,
  uint16                           index,
  mmgsdi_se13_network_info_type  * se13_network_info_ptr
)
{
  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) NULL check for se13_network_info_ptr
    2) Client ID check
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(se13_network_info_ptr);
  memset(se13_network_info_ptr, 0x00, sizeof(mmgsdi_se13_network_info_type));

  /* Checks if the input Client ID is valid or not */
  if (client_id == MMGSDI_CLIENT_ID_ZERO)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  return mmgsdi_eons_get_se13_plmn_info_by_index(index,
                                                 se13_network_info_ptr);
} /* mmgsdi_get_se13_network_info_by_index_sync */


/*===========================================================================
FUNCTION MMGSDI_GET_CACHED_NV_ITEM

DESCRIPTION
  This function provides the cached value of the NV item cached in MMGSDI

DEPENDENCIES
  None

PARAMETERS
  data_ptr  : Contains the NV Item enum to be read and shall hold the NV item
  value when the API call returns

LIMITATIONS
  API returns valid information only if MMGSDI has already read and cached the requested
  NV item

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_ERROR   : On error to get value of NV item cached in MMGSDI
  MMGSDI_SUCCESS : On successful retrival of NV from cache

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_cached_nv_item (
  mmgsdi_cached_nv_item_data    *nv_item_data_ptr)
{
  nv_items_enum_type  item_enum;
  mmgsdi_nv_item_cache_type mmgsdi_nv_data = {0};
  nv_stat_enum_type   nv_status;

  item_enum = NV_ITEMS_ENUM_MAX;
  if(nv_item_data_ptr == NULL)
  {
    UIM_MSG_HIGH_0(" nv_item_data_ptr is NULL");
    return MMGSDI_INCORRECT_PARAMS;
  }

  switch (nv_item_data_ptr->nv_item_enum)
  {
    case MMGSDI_NV_UIM_SELECT_DEFAULT_USIM_APP_I :
      item_enum = NV_UIM_SELECT_DEFAULT_USIM_APP_I;
      break;

    case MMGSDI_NV_FTM_MODE_I :
      item_enum = NV_FTM_MODE_I;
      break;

    case MMGSDI_NV_PREF_MODE_NOT_FROM_RUIM_I:
      if (mmgsdi_nv_get_feature_status(
            MMGSDI_FEATURE_PREF_MODE_NOT_FROM_RUIM, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.pref_mode_not_from_ruim = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.pref_mode_not_from_ruim = FALSE;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_NV_CDMA_DF_SELECT_BEFORE_TP_I :
      if (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_CDMA_DF_SELECT_BEFORE_TP, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
          MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.cdma_df_select_before_tp = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.cdma_df_select_before_tp = FALSE;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_NV_EXPLICIT_SELECT_DFCDMA_BEFORE_TELECOM_ADN_READ_I:
      if (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_EXPLICIT_SELECT_DFCDMA_BEFORE_TELECOM_ADN_READ,
                                       MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
          MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.cdma_df_select_before_telecom_adn = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.cdma_df_select_before_telecom_adn = FALSE;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_NV_GPRS_ANITE_GCF_I:
      item_enum = NV_GPRS_ANITE_GCF_I;
      break;

    case MMGSDI_NV_ENS_ENABLED_I:
      item_enum = NV_ENS_ENABLED_I;
      break;

    case MMGSDI_NV_SCAN_CONTROL_APPLICATION_SUPPORT_I:
      if (mmgsdi_nv_get_ens_sub_feature_status(MMGSDI_NV_SCAN_CONTROL_APPLICATION_SUPPORT_I) ==
          MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.scan_control_application_support = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.scan_control_application_support = FALSE;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_NV_NETWORK_SELECTION_MENU_CONTROL_APPLICATION_SUPPORT_I:
      if (mmgsdi_nv_get_ens_sub_feature_status(MMGSDI_NV_NETWORK_SELECTION_MENU_CONTROL_APPLICATION_SUPPORT_I) ==
          MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.nw_selection_menu_control_application_support = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.nw_selection_menu_control_application_support = FALSE;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_NV_ACTING_HPLMN_RESCAN_FEATURE_SUPPORT_I:
      if (mmgsdi_nv_get_ens_sub_feature_status(MMGSDI_NV_ACTING_HPLMN_RESCAN_FEATURE_SUPPORT_I) ==
          MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.acting_hplmn_rescan_feature_support = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.acting_hplmn_rescan_feature_support = FALSE;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_NV_RAT_BALANCING_SUPPORT_I:
      if (mmgsdi_nv_get_ens_sub_feature_status(MMGSDI_NV_RAT_BALANCING_SUPPORT_I) ==
          MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.uicc_rat_balancing_support = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.uicc_rat_balancing_support = FALSE;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_NV_SUPPORT_EXTENDED_FPLMN_ICC_I:
      if (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SUPPORT_EXTENDED_FPLMN_ICC, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
          MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.support_extended_fplmn_icc = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.support_extended_fplmn_icc = FALSE;
      }
      return MMGSDI_SUCCESS;

    default:
      UIM_MSG_HIGH_1("NV item not supported 0x%x", nv_item_data_ptr->nv_item_enum);
      return MMGSDI_ERROR;
  }

  if (mmgsdi_nv_is_nv_item_cached(item_enum, MMGSDI_NV_CONTEXT_PRIMARY))
  {
    nv_status = mmgsdi_nv_get_nv_item_from_cache(item_enum, &mmgsdi_nv_data,
                MMGSDI_NV_CONTEXT_PRIMARY);
    if (nv_status == NV_DONE_S)
    {
      switch (item_enum)
      {
        case NV_UIM_SELECT_DEFAULT_USIM_APP_I :
          nv_item_data_ptr->nv_item_data.uim_select_default_usim_app =
            mmgsdi_nv_data.item_value.uim_select_default_usim_app;
          break;

        case NV_FTM_MODE_I:
          nv_item_data_ptr->nv_item_data.ftm_mode =
            mmgsdi_nv_data.item_value.ftm_mode;
          break;

        case NV_GPRS_ANITE_GCF_I :
          nv_item_data_ptr->nv_item_data.gprs_anite_gcf =
            mmgsdi_nv_data.item_value.gprs_anite_gcf;
          break;

        case NV_ENS_ENABLED_I:
          nv_item_data_ptr->nv_item_data.ens_enabled =
            mmgsdi_nv_data.item_value.ens_enabled;
          break;

        default:
          return MMGSDI_ERROR;
      }

      return MMGSDI_SUCCESS;
    }
  }
  UIM_MSG_HIGH_1(" NV item not cached 0x%x", item_enum);
  return MMGSDI_ERROR;
} /*mmgsdi_get_cached_nv_item */


/*================================================================
FUNCTION  MMGSDI_SET_QMI_UIM_SIG

DESCRIPTION:
  Sets the MMGSDI_QMI_CAT_SIG to begin processing of QMI CAT.

INPUT PARAMETERS:
  None

DEPENDENCIES:
  mmgsdi_task must be finished initialization.

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
void mmgsdi_set_qmi_uim_sig(void)
{
  mmgsdi_task_state_enum_type     task_state = MMGSDI_TASK_INACTIVE;

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  task_state = mmgsdi_task_state[MMGSDI_TASK_MAIN];
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  if(task_state == MMGSDI_TASK_ACTIVE)
  {
    (void)rex_set_sigs(UIM_GSDI_TCB, MMGSDI_QMI_UIM_SIG);
  }
} /* mmgsdi_set_qmi_uim_sig */


/*===========================================================================
FUNCTION:  MMGSDI_GET_SE13_TABLE_RELEASE_INFO

DESCRIPTION:
  This function will return SE13 release date and source of the table.

INPUT PARAMETERS:
  se13_table_details_info_ptr se13 release info ptr

DEPENDENCIES:
  mmgsdi_task must be finished initialization.

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_SUCCESS            : successful retrieval of SE13 release info
  MMGSDI_INCORRECT_PARAMS   : on null pointer as parameter
  MMGSDI_ERROR              : failure to retrieve SE13 release info

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_se13_table_release_info(
  mmgsdi_se13_table_details_info * se13_table_details_info_ptr)
{

  if(se13_table_details_info_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(mmgsdi_eons_get_se13_table_details_info(se13_table_details_info_ptr))
  {
    return MMGSDI_SUCCESS;
  }
  else
  {
    UIM_MSG_ERR_0("Failed to retrieve SE13 table details");
  }

  return MMGSDI_ERROR;
} /* mmgsdi_get_se13_table_release_info */


/*===========================================================================
FUNCTION:  MMGSDI_CANCEL_REQUESTS

DESCRIPTION:
  This API is used to cancel all the pending requests for the particular
  client ID.

INPUT PARAMETERS:
  client_id: Client ID whose requests need to be cancelled.

DEPENDENCIES:
  The client must have a valid client ID.

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_SUCCESS          -- The Command was properly processed.
  MMGSDI_INCORRECT_PARAMS -- The parameters supplied to the API are not within
                             appropriate ranges.

SIDE EFFECTS:
  None
====================================================================== */
mmgsdi_return_enum_type mmgsdi_cancel_requests(
  mmgsdi_client_id_type              client_id
)
{
  mmgsdi_cancel_info_type  cancel_info;
  mmgsdi_return_enum_type  mmgsdi_status  = MMGSDI_SUCCESS;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
  ---------------------------------------------------------------------------*/
  if (client_id == MMGSDI_CLIENT_ID_ZERO)
  {
    UIM_MSG_ERR_0("Client id is zero");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Update cancel info */
  memset(&cancel_info, 0x00, sizeof(mmgsdi_cancel_info_type));
  cancel_info.cancel_type = MMGSDI_CANCEL_USING_CLIENT_ID;;
  cancel_info.id.client_id = client_id;

  /* We need to enter this critical section because we want the MMGSDI task
     command queue manipulation to be done in the client API context */
  MMGSDIUTIL_ENTER_TASK_SYNC_CRIT_SECT;
  mmgsdi_status = mmgsdi_process_cancel_req(cancel_info);
  MMGSDIUTIL_LEAVE_TASK_SYNC_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_cancel_requests */


/* ==========================================================================
FUNCTION:       MMGSDI_SET_BUILTIN_PLMN_LIST
========================================================================== */
/**
  This function is used to set builtin PLMN list which is received from network.
  MMGSI stores this list in EFS and gives it precedence over EF-OPLMNwACT.

  @param[in] client_id:               Client ID
  @param[in] builtin_plmn_list        list of builtin PLMNs
  @param[in] response_cb_ptr          Response callback
  @param[in] client_ref               User Data returned upon
                                      completion of this cmd.

  @return
  MMGSDI_SUCCESS          -- The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS.-- The parameters supplied to the API are not
                             within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL.  -- The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type  mmgsdi_set_builtin_plmn_list (
  mmgsdi_client_id_type                  client_id,
  mmgsdi_builtin_plmn_list_type          builtin_plmn_list,
  mmgsdi_callback_type                   response_cb_ptr,
  mmgsdi_client_data_type                client_ref
)
{
  mmgsdi_task_cmd_type                  *task_cmd_ptr   = NULL;
  mmgsdi_set_builtin_plmn_list_req_type *msg_ptr        = NULL;
  mmgsdi_return_enum_type                mmgsdi_status  = MMGSDI_SUCCESS;
  int32                                  var_len        = 0;
  int32                                  total_mem_len  = 0;
  int32                                  temp_mem       = 0;
  uint32                                 task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) plmn list ptr != NULL checl
    4) num of plmn ids > 0 Check
    5) word aligned variable check
    6) Memory allocation for task_cmd_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SET_BUILTIN_PLMN_LIST_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* Return INCORRECT PARAM error when API is called with
     Number of plmn ids 0 or plmn list ptr is NULL. Also note that
     Number of plmn ids 0 and plmn list ptr as NULL is a valid input */
  if((builtin_plmn_list.plmn_list_ptr == NULL && builtin_plmn_list.num_of_plmn_ids != 0) ||
     (builtin_plmn_list.plmn_list_ptr != NULL && builtin_plmn_list.num_of_plmn_ids == 0))
  {
    UIM_MSG_ERR_2("Invalid params: Number of plmn ids = 0x%x plmn list ptr = 0x%x",
                  builtin_plmn_list.num_of_plmn_ids, builtin_plmn_list.plmn_list_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len = builtin_plmn_list.num_of_plmn_ids * sizeof(mmgsdi_builtin_plmn_id_type);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(var_len,&temp_mem);

  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(var_len,temp_mem);

  total_mem_len += temp_mem;

  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.set_builtin_plmn_list_req ,
                         MMGSDI_SET_BUILTIN_PLMN_LIST_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.set_builtin_plmn_list_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_get_se13_plmn_names_req_type header
      1) Client ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SET_BUILTIN_PLMN_LIST_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SET_BUILTIN_PLMN_LIST_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SET_BUILTIN_PLMN_LIST_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->builtin_plmn_list.num_of_plmn_ids = builtin_plmn_list.num_of_plmn_ids;
  if (builtin_plmn_list.plmn_list_ptr)
  {
    mmgsdi_memscpy(msg_ptr->builtin_plmn_list.plmn_list_ptr,
                   builtin_plmn_list.num_of_plmn_ids * sizeof(mmgsdi_builtin_plmn_id_type),
                   builtin_plmn_list.plmn_list_ptr,
                   builtin_plmn_list.num_of_plmn_ids * sizeof(mmgsdi_builtin_plmn_id_type));
  }

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SET_BUILTIN_PLMN_LIST_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_set_builtin_plmn_list */


/* ===========================================================================
   FUNCTION: MMGSDI_GET_CACHED_NV_ITEM_EXT
=========================================================================== */
/**
  This function provides the cached value of the slot/subs-based NV item
  cached in MMGSDI

  @param[in] nv_item_data_ptr  Contains the NV Item enum to be read and
                               shall hold the NV item value when the API
                               call returns
  @param[in] nv_index          Index of the item in cache based on slot or
                               session type

  @dependencies
  None

  @limitations
  API returns valid information only if MMGSDI has already read and
  cached the requested NV item

  @return
  MMGSDI_ERROR   : On error to get from cache
  MMGSDI_SUCCESS : On successful retrival of NV from cache

  @sideeffects
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_cached_nv_item_ext (
  mmgsdi_cached_nv_item_data    *nv_item_data_ptr,
  uint8                          nv_index)
{
  mmgsdi_slot_id_enum_type  slot_id;
  mmgsdi_nv_context_type    nv_context;

  MMGSDIUTIL_RETURN_IF_NULL(nv_item_data_ptr);

  if (nv_index >= MMGSDI_MAX_NUM_SLOTS ||
      mmgsdi_util_get_slot_id(nv_index,&slot_id) != MMGSDI_SUCCESS)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot_id);

  switch (nv_item_data_ptr->nv_item_enum)
  {
    case MMGSDI_NV_UIM_JCDMA_MODE_I:
      if (mmgsdi_nv_get_jcdma_service_status(slot_id) == MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.jcdma_mode = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.jcdma_mode = FALSE;
      }
      break;
    case MMGSDI_NV_UIM_GSM_DCS_1800_I:
      if (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_UIM_GSM_DCS_1800, nv_context) ==
          MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.uim_gsm_dcs_1800 = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.uim_gsm_dcs_1800 = FALSE;
      }
      break;
    case MMGSDI_NV_RUIM_ALLOW_ESN_FAIL_I:
      if (mmgsdi_nv_get_feature_status(
            MMGSDI_FEATURE_RUIM_ALLOW_ESN_FAIL, nv_context) == MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.ruim_allow_esn_fail = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.ruim_allow_esn_fail = FALSE;
      }
      break;
    case MMGSDI_NV_RUIM_ZERO_ESN_FAIL_I:
      if (mmgsdi_nv_get_feature_status(
            MMGSDI_FEATURE_RUIM_ZERO_ESN_FAIL, nv_context) == MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.ruim_zero_esn_fail = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.ruim_zero_esn_fail = FALSE;
      }
      break;
    case MMGSDI_NV_UIM_RUIM_SUPPORT_SCI_I:
      if (mmgsdi_nv_get_feature_status(
            MMGSDI_FEATURE_UIM_RUIM_SUPPORT_SCI, nv_context) == MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.uim_ruim_support_sci = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.uim_ruim_support_sci = FALSE;
      }
      break;
    case MMGSDI_NV_UIM_AN_HRPD_FALLBACK_I:
      if (mmgsdi_nv_get_feature_status(
            MMGSDI_FEATURE_UIM_AN_HRPD_FALLBACK, nv_context) == MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.uim_an_hrpd_fallback = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.uim_an_hrpd_fallback = FALSE;
      }
      break;
    case MMGSDI_NV_UIM_MISCONFIG_RUIM_N5_WORKAROUND_I:
      if (mmgsdi_nv_get_feature_status(
            MMGSDI_FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND, nv_context) == MMGSDI_FEATURE_ENABLED)
      {
        nv_item_data_ptr->nv_item_data.uim_misconfig_ruim_n5_workaround = TRUE;
      }
      else
      {
        nv_item_data_ptr->nv_item_data.uim_misconfig_ruim_n5_workaround = FALSE;
      }
      break;
    default:
      UIM_MSG_ERR_1("MMGSDI NV/EFS enum 0x%x not cached", nv_item_data_ptr->nv_item_enum);
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_get_cached_nv_item_ext */


/*===============================================================
FUNCTION:       MMGSDI_CONTINUE_AFTER_NV_REFRESH
============================================================== */
/**
This function is used to trigger MMGSDI to continue initialization after NV refresh is done.

  @param[in] slot_id:      Slot on which the NV refresh is complete
  @param[in] nv_context:   Context where NV refresh is complete
  @param[in] reference_id: Reference Identifier
  @return
  VOID

  @dependencies
  None
*/
void mmgsdi_continue_after_nv_refresh (
  mmgsdi_slot_id_enum_type  slot_id,
  mmgsdi_nv_context_type    nv_context,
  uint32                    reference_id
)
{
  mmgsdi_task_cmd_type                      *task_cmd_ptr   = NULL;
  mmgsdi_continue_after_nv_refresh_req_type *msg_ptr        = NULL;
  int32                                      total_mem_len  = 0;
  uint32                                     task_cmd_len   = 0;
  mmgsdi_return_enum_type                    mmgsdi_status  = MMGSDI_ERROR;

  (void) reference_id;

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                     task_cmd_len);
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.continue_after_nv_refresh_req;

  /* Populating mmgsdi_continue_after_nv_refresh header
     1) Client ID
     2) Request TYPE
     3) Slot ID
     4) Client Data Pointer
     5) Response CallBack
     6) Event Callback
     7) Payload len and request len will be populated after the content
        has been populated */

  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.session_id        = 0;
  msg_ptr->request_header.slot_id           = slot_id;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = NULL;
  msg_ptr->nv_context                       = nv_context;
  msg_ptr->reference_id                     = reference_id;

  /* Calculate the payload len and request len */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
   total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_3("Queue of MMGSDI command: MMGSDI_CONTINUE_AFTER_NV_REFRESH status 0x%x, nv context 0x%x, slot 0x%x",
                mmgsdi_status,nv_context,slot_id);
}/*mmgsdi_continue_after_nv_refresh*/


/* ==========================================================================
FUNCTION:       MMGSDI_RESTRICTED_SIM_ACCESS
========================================================================== */
/**
  This function provides restricted access to the SIM card via certain
  commands. The client passes parameters of the command APDU which are then
  used by MMGSDI by to queue an equivalent MMGSDI request. The asynchronous
  response is returned in APDU format.

  @param[in] client_id:               Client ID
  @param[in] card_slot                Slot on which to send the command
  @param[in] command                  Instruction
  @param[in] apdu_params              P1, P2, P3 parameters for command APDU
  @param[in] data                     Command data (if applicable)
  @param[in] file_id_and_path         Path to file including the file id
                                      path_len = 0 if not applicable
                                      path_len = 1 if only file_id present
  @param[in] response_cb_ptr          Response callback
  @param[in] client_ref               User Data returned upon
                                      completion of this cmd.

  @return
  MMGSDI_SUCCESS          -- The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
  MMGSDI_INCORRECT_PARAMS.-- The parameters supplied to the API are not
                             within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL.  -- The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

  @dependencies
  The client must have a valid client ID.
*/
mmgsdi_return_enum_type  mmgsdi_restricted_sim_access (
  mmgsdi_client_id_type                  client_id,
  mmgsdi_slot_id_enum_type               card_slot,
  uint8                                  command,
  mmgsdi_apdu_params_type                apdu_params,
  mmgsdi_data_type                       data,
  mmgsdi_path_type                       file_id_and_path,
  mmgsdi_callback_type                   response_cb_ptr,
  mmgsdi_client_data_type                client_ref
)
{
  mmgsdi_return_enum_type                  mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_session_id_type                   session_id           = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_app_enum_type                     app_type             = MMGSDI_APP_NONE;
  mmgsdi_restricted_sim_access_data_type * crsm_client_data_ptr = NULL;
  mmgsdi_access_type                       file_access;

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if (client_id == MMGSDI_CLIENT_ID_ZERO)
  {
    UIM_MSG_ERR_0("Invalid client ID");
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_crsm_find_session_for_access(card_slot,
                                                      file_id_and_path,
                                                      command,
                                                      &session_id,
                                                      &app_type);

  if (mmgsdi_status != MMGSDI_SUCCESS || session_id == MMGSDI_INVALID_SESSION_ID)
  {
    UIM_MSG_ERR_1("Unable to find MMGSDI session for sending AT+CRSM command 0x%x",
                  command);
    return MMGSDI_ERROR;
  }

  memset(&file_access, 0x00, sizeof(file_access));

  if (command != STATUS)
  {
    mmgsdi_status = mmgsdi_crsm_calculate_file_access(app_type,
                                                      file_id_and_path,
                                                      &file_access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /* Store client data on heap so that the response from MMGSDI can be
     returned back to the client later */
  mmgsdi_status = mmgsdi_crsm_copy_restricted_sim_access_client_data(client_id,
                                                                     card_slot,
                                                                     command,
                                                                     apdu_params,
                                                                     response_cb_ptr,
                                                                     client_ref,
                                                                     &crsm_client_data_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS || crsm_client_data_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /* Map the +CRSM command to an MMGSDI request and queue to MMGSDI. */
  mmgsdi_status = mmgsdi_crsm_queue_restricted_sim_access_cmd(session_id,
                                                              file_access,
                                                              command,
                                                              apdu_params,
                                                              data,
                                                              crsm_client_data_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(crsm_client_data_ptr);
  }

  UIM_MSG_HIGH_2("Queue of MMGSDI req for AT+CRSM command 0x%x: status 0x%x",
                 command, mmgsdi_status);
  return mmgsdi_status;
} /* mmgsdi_restricted_sim_access */


/*============================================================================
   FUNCTION:      MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE

   DESCRIPTION:
     In some specific cards available in the field when BIP session or SMS PP
     download is triggered, there is a potential for certain files to be update
     without any indication to MMGSDI, so this function will notify MMGSDI when
     any BIP session or SMS PP download starts, so that MMGSDI can perform the
     appropriate action. This will potentially save on redundant SELECTs when
     accessing the EFs.

   PARAMETERS:
     client_id:         Client ID of the requesting client.
     slot:              Slot on which potential file change occurs. 

   DEPENDENCIES:
     None

   LIMITATIONS:
     Only GSTK task should invoke this API.

   RETURN VALUE:
     mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_ERROR:            Generic error.

   SIDE EFFECTS:
     None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_start_potential_silent_file_change(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot
)
{
  mmgsdi_task_cmd_type                      *task_cmd_ptr   = NULL;
  mmgsdi_start_silent_file_change_req_type  *msg_ptr        = NULL;
  int32                                      total_mem_len  = 0;
  mmgsdi_return_enum_type                    mmgsdi_status  = MMGSDI_ERROR;

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Validate the slot */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(slot,
                                    MMGSDI_SLOT_1,
                                    MMGSDI_SLOT_3);

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                     int32touint32(total_mem_len));
  if(task_cmd_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.start_silent_file_change_req;

  /* Populating mmgsdi_map_sessions_to_slots header */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.session_id        = 0;
  msg_ptr->request_header.slot_id           = slot;

  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_start_potential_silent_file_change */


/*===========================================================================
  FUNCTION:  MMGSDI_BLOCK_CARD_STATUS_EVENTS
=========================================================================== */
/**
  This API is used to block QMI UIM from sending card status events.

  @param[in] client_id:           Client ID

  @dependencies
  The client must have a valid client ID.

  @limitations
  None

  @return
  MMGSDI_SUCCESS          -- The Command was properly processed.
  MMGSDI_INCORRECT_PARAMS -- The parameters supplied to the API are not within
                             appropriate ranges.

  @sideeffects
  None
====================================================================== */
mmgsdi_return_enum_type mmgsdi_block_card_status_events(
  mmgsdi_client_id_type              client_id
)
{
  if (client_id == MMGSDI_CLIENT_ID_ZERO)
  {
    UIM_MSG_ERR_0("Client id is zero");
    return MMGSDI_INCORRECT_PARAMS;
  }

  (void)qmi_uim_block_card_status_indications();

  return MMGSDI_SUCCESS;
} /* mmgsdi_block_card_status_events */


/*================================================================
FUNCTION  MMGSDI_SET_SIM_PROFILE

DESCRIPTION:
  This function is called to to switch the profile to the given profile ID.

  INPUT PARAMETERS:
    client_id:           Client ID of the caller.
    slot:                Specific slot
    activate_profile_id: Profile ID to be activated.
    response_cb_ptr:     Pointer to the response callback.
    client_ref:          User data returned upon completion of this command.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:

  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because 
                             the maximum number of commands are already queued.

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_set_sim_profile(
  mmgsdi_client_id_type           client_id,
  mmgsdi_slot_id_enum_type        slot,
  mmgsdi_profile_id_enum_type     activate_profile_id,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
)
{
#ifdef FEATURE_UIM_MULTI_PROFILE
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_set_sim_profile_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_ERROR;
  int32                             total_mem_len = 0;
  uint32                            task_cmd_len  = 0;

  UIM_MSG_HIGH_1("SET SIM PROFILE for profile id: 0x%x", activate_profile_id);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function check
    2) Client ID
    3) Validate slot id
    4) Validate input profile id
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_SET_SIM_PROFILE_REQ);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid Client ID %0x%x",client_id);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Currently, we support profile 1 (regular) and profile 2 (emergency) only */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(activate_profile_id,
                                    MMGSDI_PROFILE_1,
                                    MMGSDI_PROFILE_2);

  if(slot != MMGSDI_SLOT_1 || mmgsdi_profile_info_ptr == NULL)
  {
    UIM_MSG_ERR_1("SET SIM PROFILE API is not supported on slot:0x%x", slot);
    return MMGSDI_MULTI_PROFILE_NOT_SUPPORTED;
  }

  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* --------------------------------------------------------------------------
    No need to align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.set_sim_profile_req;

  /* --------------------------------------------------------------------------
     Populating mmgsdi_set_profile_req_type header.
     ----------------------------------------------------------------------- */
  task_cmd_ptr->cmd.cmd_enum                  = MMGSDI_SET_SIM_PROFILE_REQ;
  msg_ptr->request_header.client_id           = client_id;
  msg_ptr->request_header.request_type        = MMGSDI_SET_SIM_PROFILE_REQ;
  msg_ptr->request_header.orig_request_type   = MMGSDI_SET_SIM_PROFILE_REQ;
  msg_ptr->request_header.slot_id             = slot;
  msg_ptr->request_header.client_data         = client_ref;
  msg_ptr->request_header.response_cb         = response_cb_ptr;

  /* Populate profile that needs to be activated */
  msg_ptr->activate_profile_id                = activate_profile_id;

  /* --------------------------------------------------------------------------
     Calculate the payload len and request len.
     ----------------------------------------------------------------------- */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
    - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SET_SIM_PROFILE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;

#else
  (void)client_id;
  (void)slot;
  (void)activate_profile_id;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_MULTI_PROFILE_NOT_SUPPORTED;

#endif /* FEATURE_UIM_MULTI_PROFILE */
} /* mmgsdi_set_sim_profile */


/*================================================================
FUNCTION  MMGSDI_GET_SIM_PROFILE

DESCRIPTION:
  This function is called to get the profile data from the EFS.

  INPUT PARAMETERS:
    client_id:       Client ID of the caller.
    slot:            Specific slot
    response_cb_ptr: Pointer to the response callback.
    client_ref:      User data returned upon completion of this command.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:

  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because 
                             the maximum number of commands are already queued.

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_get_sim_profile(
  mmgsdi_client_id_type           client_id,
  mmgsdi_slot_id_enum_type        slot,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
)
{
#ifdef FEATURE_UIM_MULTI_PROFILE
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_get_sim_profile_req_type * msg_ptr       = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_ERROR;
  int32                             total_mem_len = 0;
  uint32                            task_cmd_len  = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function check
    2) Client ID
    3) Validate slot id
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_GET_SIM_PROFILE_REQ);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid Client ID %0x%x",client_id);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(slot != MMGSDI_SLOT_1 || mmgsdi_profile_info_ptr == NULL)
  {
    UIM_MSG_ERR_1("GET SIM PROFILE API is not supported on slot : 0x%x ", slot);
    return MMGSDI_MULTI_PROFILE_NOT_SUPPORTED;
  }

  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  task_cmd_len = sizeof(q_link_type) + int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* --------------------------------------------------------------------------
    No need to align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.get_sim_profile_req;

  /* --------------------------------------------------------------------------
     Populating mmgsdi_set_profile_req_type header.
     ----------------------------------------------------------------------- */
  task_cmd_ptr->cmd.cmd_enum                  = MMGSDI_GET_SIM_PROFILE_REQ;
  msg_ptr->request_header.client_id           = client_id;
  msg_ptr->request_header.request_type        = MMGSDI_GET_SIM_PROFILE_REQ;
  msg_ptr->request_header.orig_request_type   = MMGSDI_GET_SIM_PROFILE_REQ;
  msg_ptr->request_header.slot_id             = slot;
  msg_ptr->request_header.client_data         = client_ref;
  msg_ptr->request_header.response_cb         = response_cb_ptr;

  /* --------------------------------------------------------------------------
     Calculate the payload len and request len.
     ----------------------------------------------------------------------- */
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
    - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_SIM_PROFILE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;

#else
  (void)client_id;
  (void)slot;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_MULTI_PROFILE_NOT_SUPPORTED;
#endif /* FEATURE_UIM_MULTI_PROFILE */
} /* mmgsdi_get_sim_profile */
