/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y


GENERAL DESCRIPTION

  This source file contains the library functions that are exposed to GSTK
  client as well as internal GSTK clients.

FUNCTIONS
  gstk_slot_client_init_ext
    GSTK client calls this function to initialize with GSTK module
    Client call to this function will be an asynchronous call ie
    a command will be queued into GSTK queue to process the client id
    request. Upon client id being available, client callback will be
    invoked to report the client id.

  gstk_client_toolkit_cmd_reg
    Event registration and de-registration for raw format OR for
    GSTK enhanced registration support

  gstk_send_terminal_response
    Send terminal response library function

  gstk_send_terminal_response_ext
    Send terminal response library function (Allow more than
    GSTK_MAX_ADDITIONAL_INFO bytes of additional info to be sent)

  gstk_send_raw_terminal_response
    Send raw terminal response library function

  gstk_send_envelope_evt_download_command
    Send event download envelope command library function

  gstk_send_envelope_loc_info_command
    Send Location Information envelope command library function

  gstk_send_envelope_menu_sel_command
    Send menu selection envelope command library function

  gstk_send_envelope_sms_pp_dl_command
    Send SMS PP download envelope command library function

  gstk_slot_send_envelope_sms_pp_dl_command
    Send SMS PP download envelope command library function

  gstk_update_profile_download_value
    Send terminal profile command library function

  gstk_map_proactive_req_to_cnf
    This function mapped the passed in command req type to the corresponding
    response type

  gstk_parse_ton_npi
    This function parse the ton and npi byte into gstk enum types

  gstk_send_proactive_cmd
    Allows UIM Server to put proactive command onto GSTK command queue

  gstk_ascii_to_gstk_address_type
    This function converts ASCII digits to gstk_address_type

  gstk_address_to_ascii
    This function converts gstk_address_type to ASCII digits

  gstk_send_envelope_cell_broadcast_dl_command
    Send Cell Broadcast download envelope command library function

  gstk_slot_send_envelope_cell_broadcast_dl_command
    Send Cell Broadcast download envelope command library function

  gstk_is_cell_broadcast_dl_supported
    Utility function to check if cell broadcast download is supported.

  gstk_send_raw_envelope_command
    This function is used to send raw envelope command down to SIM/USIM/RUIM.

  gstk_io_ctrl
    General interface to set/get GSTK data. The I/O operations currently
    supported is to get current access technology

  gstk_send_icon_display_rsp
    This function notifies GSTK whether the UI displayed the icon

  gstk_send_user_cnf_alpha_and_icon_disp_rsp
    This function notifies GSTK whether the UI confirmed the call
    and displayed the icon

  gstk_slot_send_envelope_cc_command_ext
    Send call control envelope command library function based on the slot,
    rat and location information provided

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstklib.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/17/16   vr      F3 reduction
05/06/16   gs      F3 logging improvements
05/04/16   gm      Removed the logic of setting CDMA as rat when both LTE and 
                   CDMA are active
09/25/15   vr      Check pending proactive cmd MMGSDI FETCH allowed is set
05/21/15   lm      UT framework changes
05/12/15   dy      Enabling Toolkit-initiated GBA TP bit (byte 25 bit 5)
04/27/15   vr      Non UI and non NW proactive command parallel processing
01/09/15   gm      Update slot according to client_id
10/08/14   bd      Changes to control eHRPD loc info feature with NV
10/08/14   shr     Comments clean-up
10/08/14   gm      Remove GSTK registration with WMS if FEATURE_ESTK is enabled
09/25/14   gm      Added support to send failure indication for OTASP
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info
09/23/14   gs      Make gstk_num_of_instances as part of gstk_shared_data_type
09/09/14   vr      Reduce stack usage
09/04/14   dy      Fix KW errors
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/29/14   vr      Offtarget support for mmgsdi fetch_allowed
07/23/14   vr      Cleanup GSTK_CLIENT_INIT_EXT function
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/21/14   vr      Set client id for attach control request
07/21/14   sw      Display alpha related changes
07/11/14   shr     Add Support for NV Refresh
06/12/14   gm      Updating client id in MO_SMS control request
06/02/14   dy      Fixing incorrect cell_id being copied for CC and
                   MO SMS control
05/31/14   vr      Checking all the session types for as_id to slot mapping
05/20/14   dy      Fixed handling of apps_enabled in mmgsdi_session_changed
05/20/14   sw      KW errors on DI.3.0 promoted
05/19/14   dy      Fix mapping of ds sub id to slot id
05/08/14   gm      Updating gstk_instances_ptr to GSTK context
05/06/14   gs      GSTK Featurization enhancements and clean up
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/17/14   gm      Added support for IMS communication control
03/03/14   shr     ESTK BIP clean-up and enhancements
02/20/14   gm      Remove FEATURE_CCBS
02/19/14   gs      Fixed infinite IMSI refresh issue
01/31/14   vr      Allow the fetch command based on the fetch status from MMGSDI
01/30/14   vr      SIM initiated call control handling
01/19/14   bd      Fixed incorrect TR for PLI NMR UTRAN
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Updated GSTK_NULL_CHECK_PTR macro
12/26/13   vr      Fix for single SIM targets, as_id to slot, slot to as_id
12/18/13   hn      Fixed memory leak in gstk_io_ctrl()
12/16/13   bd      Added support for new CM SS event handling
12/11/13   df      Remove unused memory paging feature
12/11/13   bd      Fixed a crash when MM sends MM INDICATION for SGLTE
12/06/13   bd      Changes in GSTK to support MM IDLE events for SGLTE+G
12/02/13   gm      Remove Lint error
11/12/13   sw      Removed FEATURE_GSTK_TEST_AUTOMATION code
11/13/13   shr     Added support for BIP over WLAN
10/10/13   gm      Marked cmd_ref_id to get command table index
10/04/13   sw      Reduced F3 messages
09/20/13   shr     LTE multi-SIM support
09/13/13   gm      Support for Recovery and hot-swap
08/22/13   vr      Klocwork fixes
08/06/13   vr      Centralized Call Control changes
07/26/13   hh      Support for E-UTRAN Inter-frequency measurements
07/23/13   sw      Added support in gstk_io_ctrl for sharing DS RAT information
07/22/13   hn      Support MM Indications
07/08/13   sw      Not allowing Proactive commands at same time from different slots
07/01/13   hh      Added support for passing RAT through MO SMS CTRL API
07/01/13   hn      Support SGLTE dual MMs
06/24/13   xz      Support IMS Registration event download
06/20/13   vr      Triton TSTS changes
06/19/13   hh      Send error response to clients when env ref table is full
05/23/13   hh      Fix KW errors
05/13/13   vr      gstk_proactive_cmd_in_progress set to flase, for null
                   procactive command bytes.
05/13/13   vr      Update slot id for Cell Broadcast download envelope
05/01/13   vr      Fix command processing logic in GSTK
04/29/13   gm      Removed unused GSTK function
04/26/13   gm      Not allowing fetch before TP download
04/23/13   gm      Remove unused function
04/16/13   gm      Return FALSE if gstk-state is not equal to GSTK_IDLE_S
04/09/13   vr      Use safer versions of memcpy() and memmove()
04/02/13   av      Move UIM tasks to group4 of RCINIT
03/09/13   gm      Not allowing proactive command before TP
03/29/13   bd      Allow ATTACH CTRL to queue when env retry is in progress
03/21/13   ak      Allow uimdrv to notify GSTK when an error is returned
                   for a FETCH cmd
02/21/13   hn      Enhancement of Loc Status and VPE retry
02/11/13   bd      Support for (e)HRPD access tech
01/17/13   hn      Support multiple sessions per subscription
11/29/12   bd      Clean up of redundant members from client table
09/30/12   vs      Notify remote TAL client of recovery
09/20/12   yt      Manage concurrency between BIP and OTASP
09/04/12   hk      Fix security issue in raw TR
08/27/12   hk      Fix to pass correct CC length
08/03/12   bd      Fixed crash issue on MOB
06/05/12   sg      Return Err for Attach Ctrl queue if env retry in progress
05/24/12   dd      Fix NW Rej envelope
05/22/12   nb      Convert setting and resetting of gstk_proactive_cmd_in_progress
                   flag to MACRO
04/25/12   bd      Clean up FEATURE_CAT_REL6 which was moved to NV
03/09/12   bd      Enhancements for location information in CC
03/16/12   yt      Modify references to gstk_tcb
03/13/12   dd      Check sms tpdu pointer for NULL
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/13/12   nb      Fix for compilation warning
01/03/12   nb      Support to retrieve terminal profile
12/20/11   nb      GSDI cleanup
12/05/11   xz      Fix issue of null ptr dereference
12/05/11   xz      Fix issue of invalid data length
12/02/11   dd      Fix security issues
12/02/11   dd      Fix security issues
10/21/11   sg      Check if mode change has CDMA RAT in normal service
10/20/11   sg      Access Tech for IO CTRL when LTE and CDMA are active
10/13/11   nb      Full recovery support
09/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
08/24/11   xz      Fix compiler warnings and LINT/KW errors
08/03/11   xz      Fix compiler warning
07/26/11   js      Change to support AcT event processing in TAL
07/06/11   xz      Fix crash issue of GSTK_IO_GET_CURR_LOC_STATUS
05/25/11   xz      Set slot ID to GSTK_SLOT_1 for ESTK_MENU_SEL_REQ_CMD cmd
05/24/11   nb      Fix for compilation error
05/18/11   nb      Thread Safe - Release critical section when exiting
                   gstk_client_init()
05/18/11   bd      Thread safe changes - protect client table
05/18/11   nb      Removed Usage of gstk_util_save_env_cmd_info()
05/18/11   bd      Removed gstk_util_cmd_details_ref_table_free_slot() from GSTK APIs
05/18/11   bd      Moved access to pending_responses to GSTK task context
05/17/11   sg      Queue access tech to TAL
05/04/11   dd      Fix a crash in SMS PP DL
05/03/11   dd      Fix deadlock between GSTK/UIM
04/25/11   bd      Fixed issue of proactive commands not being fetched
                   after REFRESH RESET
04/28/11   bd      Featurized inclusion of ds_cap.h using FEATURE_NAS
04/26/11   dd      Fix compilation error
04/24/11   dd      SCWS support
04/22/11   bd      Fixed accessing array out of bounds
04/13/11   ea      Add proactive cmd enable / disable support
04/05/11   xz      Changes to support VPE ATTACH CTRL
04/01/11   bd      Fixed a log statement
03/21/11   bd      Fixed a memory leak
03/16/11   nb      Fixed menu selection for Dual STK
02/24/11   js      Fixed compilation error
02/22/11   js      Fix for async processing of TAL commands
02/18/11   yt      Fixed Klocwork error
02/03/11   dd      User conf with icon for BIP command support
02/02/11   dd      Combine Access Tech envelopes in TAL
02/02/11   sg      1)Delay first fetch by 15 seconds
                   2)Avoid memset by 0 overwrite TAL CB
02/01/11   dd      Fix the TAC for LTE
02/01/11   ssr     Fixed CCP parsing
01/03/11   xz      Fix the issue of wrong ECI
12/30/10   ssr     Dynamically allocate command buffer
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/16/10   nb      Added Definition for gstk_send_envelope_menu_sel_command_ext
                   and updated handling of menu selection response
12/10/10   sg      Port support of IO ctrl to get current loc status
11/25/10   ssr     Fixed command detail parsing
11/09/10   bd      Fixed dual standby issues for network related commands
11/04/10   bd      Fixed log statement
11/01/10   js      Fixed GSTK Memory Leak
10/18/10   js      Fixed compiler warnings
10/05/10   xz      KW Fixes
10/01/10   dd      Change the slot in gstk_send_user_cnf_alpha_rsp
09/13/10   xz      Add support of SETUP MENU in raw APDU for DS/DS
08/26/10   xz      Add support of EPS Call Control
08/23/10   vs      Corrected type of tech_support_mask
08/19/10   js      Update gstk_tal_client_registration()
08/18/10   dd      Fix KW error
08/18/10   dd      Fixing critical lint error
08/14/10   js      Input param check for gstk_tal_send_cmd
08/08/10   js      GSTK should support backward compatibility for slot specific
                   client registration on fusion
07/28/10   sg      TAL support for envelope commands
07/28/10   xz      Read ENS NV during initialization
07/27/10   js      TAL fixes for sending TR
07/26/10   xz      LINT fixes
07/26/10   nb      Updated gstk_uim_fetch_allow to allow FETCH after ENS init
07/24/10   js      TAL command processing fixes
07/16/10   xz      Fix the issue of queuing too many cmd in MMGSDI to read
                   service table
07/15/10   sg      TAL API updates
07/15/10   dd      Fix sending of raw alpha to clients
07/14/10   sg      Fix RPC Linker error
07/07/10   sg      Client registration for TAL
07/09/10   xz      Don't filter events at API level
07/07/10   xz      Added API gstk_is_service_enabled_ext
06/30/10   dd      Icon display confirmation
06/30/10   xz      Fix TP DL issue
06/29/10   nb      Merged changes for DS DS
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
06/01/10   js      ZI memory reduction fixes
05/11/10   xz      1) Add support of network rejection event
                   2) Add support of GSTK_IO_SEND_EMM_REGISTERED_EVENT
05/05/10   bd      Fixed issue of not sending MO SMS CONTROL envelope to slot 2
04/27/10   xz      Add support of EMM-IDLE event
04/09/10   dd      Allow only 1 UTK session at a time
04/07/10   yt      Fixed compilation warnings
04/07/10   xz      Fix the issue of delay in FETCH after envelope rsp
03/29/10   dd      Remove the check for the GSTK_SLOT_AUTO in menu selection env
03/29/10   bd      Fixed slot issue in sms-pp dl
03/22/10   bd      Fixed gstk_slot_client_init_ext to add GSTK_WAKEUP_TYPE
03/19/10   dd      Add GSTK_SLOT_AUTO as valid slot during client init
03/18/10   bd      Fixed gstk_get_card_event_data for cmd_ref_id and client type
03/15/10   xz      Lint Fix
03/12/10   sg      DS fixes for slot id
03/05/10   xz      DS/DS NMR Changes
03/03/10   bd      Support for caching proactive commands
02/19/10   bd      Ported the fix for issue of gstk_profile_received not being updated
                   when client sends TP
02/15/10   nb      Moved Dual Slot Changes
01/25/10   xz      Fix the issue of not able to parse CCP
01/14/10   sg      Added gstk_io_ctrl api
01/12/09   sg      Added command for BIP support
11/18/09   bd      Support for delaying sending of CC envelope to card
10/01/09   kp      Fixed compilation error
09/26/09   kp      ZI memory reduction changes
08/26/09   xz      Featurize CCP with FEATURE_NAS
08/25/09   sg      Changed argument type of mask in gstk_client_reg to uint64
07/29/09   xz      Add gstk_util_parse_ccp()
05/18/09   dd      Only allow one GSTK Menu Selection message at a time
04/28/09   gg      Calling gsdi_lib_is_service_available() in
                   gstk_is_sms_pp_supported() to check status of service
04/15/09   xz      Support client id clean up of modem restart
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id
                   when parse client TP and simplify TP download procedure
03/11/09   yb      Fixed Warnings for using 'free' as variable name
03/02/09   xz      Add client_id when send raw terminal response
02/27/09   gg      Fixing SimToolkit Black screen problem
01/16/09   xz      1) Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
                   2) Fix issue of unable to send terminal response with addi info
11/13/08   xz      Fix issue of unable to de-reg
11/13/08   xz      Fix klockwork error
11/03/08   sk      Featurized ESTK based on external interface support
10/13/08   yb      Fixed documentation
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/16/08   sk      Fixed lint error
09/14/08   sk      Fixed lint errors
09/08/08   sk      Added support for FEATURE_ESTK
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/22/08   xz      Change malloc() to gstk_malloc()
08/21/08   sk      Added support for enhanced GSTK registration that includes
                   support for multiple clients to perform terminal profile
                   download and support for clients to register for a subset of
                   proactive command in GSTK or RAW format.
08/14/08   sk      Fixed warnings.
08/08/08   xz      Add support for raw format of registration, proactive
                   command, terminal response and evelope command/response
04/07/08   sk      Fix for PLI UTRAN NMR - Limited Service
03/17/08   sk      Fixed klockwork errors
02/06/08   sk      Support for PLI UTRAN NMR
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
11/20/07   tml     Added support to allow delay in sending TR until client
                   responded back for certain internal command that was originally
                   handled by GSTK
06/18/07   sp      Back out changes in gstk_slot_is_cc_required()
05/25/07   sp      Update the check to see if service is available and also
                   activated for call control.
04/27/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
02/01/07   sk      Reverted changes to return FALSE for gprs call control
01/31/07   sk      Fixed lint error
01/29/07   sk      Return FALSE for check for gprs call control
11/08/06   tml     Added function to return the current toolkit slot
10/23/06   sk      Fixed lint errors.
09/18/06   sk      Fixed lint errors.
08/21/06   sk      Added featurization for GPRS call control
05/23/06   tml     GPRS Call Control support
06/01/06   sk      Added support for channel status and data available
                   registrations.
05/03/06   tml     lint
01/18/05   sk      Added check to avoid Terminal Response for a
                   REFRESH SIM Reset proactive command.
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/23/04  sk/tml   Fixed compilation warning
09/20/04   tml     Added 8 bit to 7 bit decoding function
08/19/04   tml     Added cell broadcast support
07/22/04   tml     Added gstk_send_terminal_response_ext API such that
                   greater than GSTK_MAX_ADDITIONAL_INFO can be passed down
                   to the card
05/26/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/16/04   tml     linted
03/01/04   tml     Added provide local info lang support
12/02/03   tml     call control and mo sms control fixes
11/05/03   tml     Fixed call control issue
09/26/03   tml     Put cmd back to free queue if send terminal response fails
09/19/03   att     Added return values
09/13/03   tml     Added ascii to gstk_address_type and vice versa conversaton
                   utility functions
                           Changed slot_id_type to slot_id_enum_type
09/04/03   tml     Dual slot support
09/03/03   tml     Fixed envelope command table free issue
07/08/03   tml     Allowed at least 1 address in the MO SMS control to be
                   of zero length
05/16/03   tml     linted
04/21/03   tml     Added is sms pp support library function and NPI support and
                   cdma sms support
03/06/03   tml     Updated enum names, added gstk_is_cc_required and
                   gstk_is_mo_sms_ctrl_required
02/26/03   tml     Changed functions to take gstk_evt_cb_funct_type instead of
                   pointer to gstk_evt_cb_funct_type
02/13/03   tml     Added/Changed DM messages
02/12/03   tml     Added gstk_map_proactive_req_to_cnf
02/11/03   tml     Ensure that GSTK is the only that is allowed to send
                   terminal response for More Time proactive command
02/07/03   tml     Initial Version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include <stringl/stringl.h>
#ifdef FEATURE_GSTK_FUSION_REMOTE
#include "gstk_exp_fusion.h"
#define GSTK_EXP_H
#else
#include "gstk_exp.h"
#endif /*FEATURE_GSTK_FUSION_REMOTE*/
#include "gstk.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"
#include "gstk_envelope_rsp.h"
#ifdef FEATURE_GSTK_TAL
#include "gstk_tal.h"
#endif /* FEATURE_GSTK_TAL */
#include "string.h"
#include "err.h"
#include "uim_msg.h"
#ifdef FEATURE_NAS
#include "ds_cap.h"
#endif /* FEATURE_NAS */
#ifdef FEATURE_ESTK
#include "estk.h"
#endif /* FEATURE_ESTK */

/*===========================================================================

                     DEFINES

===========================================================================*/
/* access identity */
#define ACCESS_OCTET_ID                      0

/* layer 1 identity */
#define LAYER_1_OCTET_ID                     1

/* layer 2 identity */
#define LAYER_2_OCTET_ID                            2

/*connection element*/
#define TRANSPARENT_DATA                     0

/* modem type */
#define NO_MODEM                             0
#define AUTO_TYPE_1                          8
#define RESERVED_OMT                         3

/* Facsimile group 3 */
#define ITC_FAX_G3                           3

/* Fixed Network User Rate */
#define FNUR_57600                            0x0C

/*===========================================================================
FUNCTION GSTK_SLOT_CLIENT_INIT_EXT

DESCRIPTION
  To request a GSTK client ID for a specified (U)SIM slot.

PARAMETERS
  client_type: [Input]        Indicates the client type of the calling client.
                              Valid values are:
                                GSTK_DS_TYPE
                                GSTK_UI_TYPE
                                GSTK_ESTK_TYPE
  response_cb: [Input]        client callback that GSTK uses to return
                              client id
  client_ref:  [Input/Output] client reference data


DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS             - if processing of client_init is successful
    GSTK_INVALID_CLIENT_TYPE - if client type is invalid
    GSTK_BAD_PARAM           - if client callback is NULL
    GSTK_MEMORY_ERROR        - if GSTK queue is FULL
    GSTK_UNSUPPORTED_COMMAND - if enhanced multiple client support is
                               not turned on.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_client_init_ext (
  gstk_slot_id_enum_type         slot,
  gstk_client_type               client_type,
  gstk_client_init_cb_func_type  response_cb,
  gstk_client_ref_data_type      client_ref
)
{
  gstk_task_cmd_type      *task_cmd_ptr  = NULL;

  UIM_MSG_HIGH_0("GSTK Slot client init ext");
  switch(client_type)
  {
  case GSTK_DS_TYPE:
  case GSTK_UI_TYPE:
  case GSTK_TEST_TYPE:
  case GSTK_ESTK_TYPE:
  case GSTK_NO_TYPE:
  case GSTK_WAKEUP_TYPE:
  case GSTK_IMS_TYPE:
    break;
  default:
    UIM_MSG_ERR_1("Invalid GSTK client type 0x%x", client_type);
    return GSTK_INVALID_CLIENT_TYPE;
  }

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return GSTK_BAD_PARAM;
  }

  /* Check if client callback is NULL */
  if (response_cb == NULL)
  {
    UIM_MSG_ERR_0("Client callback is NULL");
    return GSTK_BAD_PARAM;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr == NULL )
  {
    return GSTK_MEMORY_ERROR;
  }
  /* build the client id registration command */
  task_cmd_ptr->cmd.client_id_reg.message_header.command_group =
    GSTK_CLIENT_REG_REQ_CMD;
  task_cmd_ptr->cmd.client_id_reg.message_header.command_id    =
    int32touint32((int32)GSTK_CLIENT_ID_REG_REQ);
  task_cmd_ptr->cmd.client_id_reg.message_header.sim_slot_id   = slot;
  task_cmd_ptr->cmd.client_id_reg.message_header.user_data     = client_ref;
  task_cmd_ptr->cmd.client_id_reg.client_type                  = client_type;
  task_cmd_ptr->cmd.client_id_reg.client_cb_ptr                = response_cb;
  gstk_task_put_cmd_buf(task_cmd_ptr);

  return GSTK_SUCCESS;
} /* gstk_slot_client_init_ext */


/*===========================================================================
FUNCTION GSTK_CLIENT_TOOLKIT_CMD_REG

DESCRIPTION
  This is a new registration API that clients can use to register with GSTK for
  specific proactive commands. This API differs from "gstk_client_reg_ext" in
  the following aspects.
  a. the client has the option of specifying hich part (UI, MODEM or ALL) of the
     proactive command it would like to process.
  b. The client can also specify if it wishes to recive the proactive command notification
  in the GSTK format or in the RAW APDU format.
  c. Client can provide a command callback that GSTK would use to report the status
     of the registration even before the proactive commands come in from the card.
  NOTE: A single client should register with the same callback for any proactive
        command registration irrespective of the format and the functionality
        type.

  The following functionalities are similar as in the API "gstk_client_reg_ext".
  a. the client should provide a event callback that GSTK would use to send
     notification of the proactive commands as and when they occur.
  b. Based on the evt_reg_bitmask and the class bitmask, gstk will only send
     event/information to the client who has registered for that event.
  c. The cb function should be aware of its total processing time, since the cb
     will be running in gstk context.
  d. Most of the proactive commands allow only one client registration, this is
  to prevent multiple terminal responses sent by various clients for the same
  single proactive command.  Exception to this is MORE TIME and END OF
  PROACTIVE COMMAND commands, where even without client registration for these
  specific events, GSTK will broadcast them to all its clients.
  NOTE: New Caller(s) that would be calling in for registering for proactive
        commands are recommended to use the this API which is backward compatible
        in functionality to the API "gstk_client_reg_ext".

PARAMETERS

  client_id         [Input] - Client id
  user_data         [Input] - Client user data
  data_format_type  [Input] - Data format
  num               [Input] - Number of commands being registered for
  reg_list_ptr      [Input] - pointer to registration list
  client_cmd_cb     [Input] - Immediate callback invoked to reflect status of registration
  client_evt_cb     [Input] - Callback to receive command in the raw / gstk format

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_CLIENT_NOT_REGISTERED - if the client id is not recognized
    GSTK_BAD_PARAM             - bad input parameters provided
    GSTK_NULL_INPUT_PARAM      - null input pointer provided
    GSTK_SUCCESS               - command was queued successfully to GSTK

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been
  called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_client_toolkit_cmd_reg(
  gstk_client_id_type                         client_id,
  gstk_client_ref_data_type                   client_data,
  gstk_client_pro_cmd_data_format_enum_type   data_format_type,
  uint32                                      num,
  const gstk_toolkit_cmd_reg_info_type        *reg_list_ptr,
  gstk_reg_rsp_cmd_cb_func_type               client_cmd_cb,
  gstk_toolkit_evt_cb_func_type               client_evt_cb
)
{
  gstk_task_cmd_type      *task_cmd_ptr  = NULL;

  /* check for valid client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  /* check for valid data format type */
  if (!gstk_util_is_valid_data_format_req(data_format_type))
  {
    UIM_MSG_ERR_1("Invalid data format type 0x%x", data_format_type);
    return GSTK_BAD_PARAM;
  }
  /* check for number of registration */
  if (num > GSTK_TOTAL_ELEMENTS_IN_CLASS_ARRAY)
  {
    UIM_MSG_ERR_1("Too many registrations 0x%x", num);
    return GSTK_BAD_PARAM;
  }

  /* check for null pointer */
  if (num > 0)
  {
    if (!reg_list_ptr)
    {
      UIM_MSG_ERR_0("reg_list_ptr is NULL");
      return GSTK_NULL_INPUT_PARAM;
    }
    if (client_evt_cb ==  NULL)
    {
      UIM_MSG_ERR_0("Client Pro Cmd Cb not provided");
      return GSTK_NULL_INPUT_PARAM;
    }
  }/* if (num > 0) */
  /* check for validity of client immediate callback */
  if (client_cmd_cb == NULL)
  {
    UIM_MSG_ERR_0("Client Command Callback not provided");
    return GSTK_BAD_PARAM;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr == NULL )
  {
    return GSTK_MEMORY_ERROR;
  }
  /* build the terminal profile command */
  task_cmd_ptr->cmd.client_evt_reg.message_header.sim_slot_id =
    gstk_util_get_sim_slot_id(client_id);
  task_cmd_ptr->cmd.client_evt_reg.message_header.command_group =
    GSTK_CLIENT_REG_REQ_CMD;
  task_cmd_ptr->cmd.client_evt_reg.message_header.command_id =
    int32touint32((int32)GSTK_CLIENT_PRO_CMD_REG_REQ);
  task_cmd_ptr->cmd.client_evt_reg.message_header.user_data = client_data;
  task_cmd_ptr->cmd.client_evt_reg.client_id                = client_id;
  task_cmd_ptr->cmd.client_evt_reg.client_cmd_cb            = client_cmd_cb;
  task_cmd_ptr->cmd.client_evt_reg.client_evt_cb            = client_evt_cb;
  task_cmd_ptr->cmd.client_evt_reg.num_of_cmds              = num;
  task_cmd_ptr->cmd.client_evt_reg.data_format_type         = data_format_type;

  if (num > 0)
  {
    task_cmd_ptr->cmd.client_evt_reg.reg_info_ptr =
      (gstk_toolkit_cmd_reg_info_type*)GSTK_CALLOC(sizeof(gstk_toolkit_cmd_reg_info_type) * num);
    if (!task_cmd_ptr->cmd.client_evt_reg.reg_info_ptr)
    {
      /* put the queue back to the free queue */
      gstk_task_free_cmd_buf(task_cmd_ptr);
      return GSTK_MEMORY_ERROR;
    }
    (void)memscpy( task_cmd_ptr->cmd.client_evt_reg.reg_info_ptr,
            sizeof(gstk_toolkit_cmd_reg_info_type) * num,
            reg_list_ptr,
            sizeof(gstk_toolkit_cmd_reg_info_type) * num );
  }/* if (num > 0) */

  gstk_task_put_cmd_buf(task_cmd_ptr);
  return GSTK_SUCCESS;
} /* gstk_client_toolkit_cmd_reg */

/*===========================================================================
FUNCTION GSTK_SEND_TERMINAL_RESPONSE_EXT

DESCRIPTION
  This function is served as a utility function to the gstk client, so that
  the client can call this function in sending a terminal response to
  a proactive command.
  In this function, GSTK, will copy all the information in the parameter
  list into gstk_cmd type, and assign the correct length, cmd_detail_reference
  to the gstk_exp_hdr_type, and signal GSTK_TERMINAL_RESPONSE, so that when GSTK
  starts processing this command from its command queue, it knows what
  command it is, the corresponding command details to be used based on the
  ref_id.
  This function differs from gstk_Send_terminal_response in that it takes
  gstk_additional_info_ptr_type instead of gstk_additional_info_type.
  i.e., when client is required to pass in more than GSTK_MAX_ADDITIONAL_INFO
  byte of additional information, they can use this utility function

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                             value returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
  command_number: [Input] This is the same value as that was being passed to
                          the client in the corresponding proactive command
                          request
  terminal_response_enum: [Input] Specifies the _CNF for the response to
                                  the proactive cmd that the client wants to
                                  notify SIM/USIM
  general_result: [Input] Specifies the result of the proactive command
                          operation
  additional_result: [Input] Pointer specifies the optional additional result
                             for the proactive command, the length field in
                             the gstk_additonal_info_ptr_type specifies number of
                             additional_info_ptr in the structure
  other_info: [Input] A union pointer of various extra parameters for the
                      various proactive command response, please refer to
                      gstk_terminal_rsp_param_type for details

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_EXTRA_PARAM_MISSING,
    GSTK_EXTRA_PARAM_NOT_REQUIRED,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_terminal_response_ext (
        gstk_client_id_type                     client_id,
        uint32                                  user_data,
        uint32                                  cmd_detail_ref_id,
        uint8                                   command_number,
        gstk_cmd_enum_type                      terminal_response_enum,
        gstk_general_result_enum_type           general_result,
        gstk_additional_info_ptr_type          *additional_result,
        gstk_terminal_rsp_extra_param_type     *other_info )
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
    gstk_task_cmd_type      *task_cmd   = NULL;
    uint32                  index       = GSTK_CMD_REF_TABLE_INDEX(cmd_detail_ref_id);

    UIM_MSG_HIGH_3("Client send terminal rsp, ref_id:0x%x, command #:0x%x, enum:0x%x",
                   cmd_detail_ref_id, command_number, terminal_response_enum);

    gstk_status = gstk_util_compare_ref_id(cmd_detail_ref_id);
    if(gstk_status != GSTK_SUCCESS)
    {
      return gstk_status;
    }

    /* validate client id */
    if (!gstk_util_is_valid_client_id(client_id))
    {
        UIM_MSG_ERR_1("Invalid Client ID 0x%x", client_id);
        return GSTK_CLIENT_NOT_REGISTERED;
    }

    if (index >= GSTK_MAX_PROACTIVE_COMMAND)
    {
      return GSTK_PARAM_EXCEED_BOUNDARY;
    }
    /* validate command details/command num and command resp type */
    if (command_details_ref_table_ptr[index].free_slot) {
      /* no need to send terminal response down since already done so */
      UIM_MSG_ERR_2("Free slot for TR: 0x%x, expected TR: 0x%x",
                    terminal_response_enum,
                    command_details_ref_table_ptr[index].expected_cmd_rsp_type);
      if ((command_details_ref_table_ptr[index].expected_cmd_rsp_type
             == GSTK_MORE_TIME_CNF) ||
          (terminal_response_enum == GSTK_MORE_TIME_CNF)) {
        return GSTK_SUCCESS;
      }
      else {
        return GSTK_BAD_PARAM;
      }
    }
    else {
      if (command_details_ref_table_ptr[index].expected_cmd_rsp_type == GSTK_MORE_TIME_CNF) {
        if (!gstk_util_is_client_id_internal(client_id))
        {
         /* no need to send terminal response down since gstk will do so */
         return GSTK_SUCCESS;
        }
      }
      if ((command_details_ref_table_ptr[index].expected_cmd_rsp_type ==
            terminal_response_enum) &&
          (command_details_ref_table_ptr[index].command_details.command_number ==
            command_number)) { /* command details info okay */

        /* Check for general result */
        switch(general_result) {
        case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
        case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
          if (command_details_ref_table_ptr[index].partial_comprehension) {
            general_result = GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION;
          }
          break;
        case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
        case GSTK_PCMD_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ:
        case GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM:
        case GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE:
        case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
        case GSTK_REFRESH_PERFORMED_BUT_USIM_NOT_ACTIVE:
        default:
          break;
        }

        /* get command buf */
        task_cmd = gstk_task_get_cmd_buf();
        if ( task_cmd != NULL ) { /* successfully obtained command buffer */
            /* build the terminal response */
            if (other_info != NULL)
            {
            UIM_MSG_HIGH_1("Other_info is 0x%x", other_info->present);
            }

            task_cmd->cmd.general_cmd.message_header.sim_slot_id =
              gstk_util_get_sim_slot_id(client_id);
            gstk_status = gstk_populate_terminal_response_cnf(
                                            task_cmd,
                                            terminal_response_enum,
                                            general_result,
                                            additional_result,
                                            other_info,
                                            command_number,
                                            cmd_detail_ref_id,
                                            user_data);
            if (gstk_status == GSTK_SUCCESS) {
              /* set GSTK_CMD_Q_SIG and put on GSTK queue */
              gstk_task_put_cmd_buf(task_cmd);
            }
            else {
              /* put the queue back to the free queue */
              gstk_task_free_cmd_buf(task_cmd);
              UIM_MSG_ERR_1("Populate Terminal Response Error, 0x%x",
                            gstk_status);
            }
        }
        else {
            // build error message to GSTK?????????????
            gstk_status = GSTK_MEMORY_ERROR;
        }
      }
      else { /* command details info not okay */
            if(other_info == NULL)
            {
                UIM_MSG_ERR_0("other_info ERR:NULL");
                return GSTK_BAD_PARAM;
            }
#ifdef FEATURE_ESTK
        if (estk_is_in_legacy_mode() &&
            (terminal_response_enum == GSTK_DISPLAY_TEXT_CNF ||
             terminal_response_enum == GSTK_GET_INKEY_CNF))
        {
          UIM_MSG_HIGH_1("estk is in legacy mode 0x%x!",
                         terminal_response_enum);
          if (terminal_response_enum == GSTK_GET_INKEY_CNF)
          {
            if (other_info->extra_param.get_inkey_extra_param.user_get_inkey_input.text
                == NULL)
            {
              UIM_MSG_ERR_0("Null Ptr!");
              return GSTK_BAD_PARAM;
            }
            UIM_MSG_HIGH_3("GET INKEY TR: 0x%x, 0x%x, 0x%x",
                           other_info->present,
                           other_info->extra_param.get_inkey_extra_param.user_get_inkey_input.length,
                           other_info->extra_param.get_inkey_extra_param.user_get_inkey_input.text[0]);
            if (general_result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY &&
                other_info->present &&
                other_info->extra_param.get_inkey_extra_param.user_get_inkey_input.length == 1 &&
                other_info->extra_param.get_inkey_extra_param.user_get_inkey_input.text[0] == 0x01)
            {
              if (gstk_send_user_cnf_alpha_and_icon_disp_rsp(
                    client_id,
                    cmd_detail_ref_id,
                    TRUE,
                    0) != GSTK_SUCCESS)
              {
                UIM_MSG_ERR_0("failed to send user cnf alpha rsp!");
                return GSTK_ERROR;
              }
            }
            else
            {
              if (gstk_send_user_cnf_alpha_and_icon_disp_rsp(
                    client_id,
                    cmd_detail_ref_id,
                    FALSE,
                    0) != GSTK_SUCCESS)
              {
                UIM_MSG_ERR_0("failed to send user cnf alpha rsp!");
                return GSTK_ERROR;
              }
            }
          }
          return GSTK_SUCCESS;
        }
        else
#endif /*FEATURE_ESTK*/
        {
          UIM_MSG_ERR_2("Command Details info Error, exp command #: 0x%x, exp command type: 0x%x",
                        command_details_ref_table_ptr[index].command_details.command_number,
                        command_details_ref_table_ptr[index].expected_cmd_rsp_type);
          UIM_MSG_ERR_2("Command Details info recv: command #: 0x%x, term rsp type: 0x%x",
                        command_number, terminal_response_enum);
          return GSTK_BAD_PARAM;
        }
      }
    }
    return gstk_status;
}/*lint !e818 *other_info *additional_info suppression for externalized function */
/*gstk_send_terminal_response_ext*/

/*===========================================================================
FUNCTION GSTK_SEND_TERMINAL_RESPONSE

DESCRIPTION
  This function is served as a utility function to the gstk client, so that
  the client can call this function in sending a terminal response to
  a proactive command.
  In this function, GSTK, will copy all the information in the parameter
  list into gstk_cmd type, and assign the correct length, cmd_detail_reference
  to the gstk_exp_hdr_type, and signal GSTK_TERMINAL_RESPONSE, so that when GSTK
  starts processing this command from its command queue, it knows what
  command it is, the corresponding command details to be used based on the
  ref_id.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                             value returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
  command_number: [Input] This is the same value as that was being passed to
                          the client in the corresponding proactive command
                          request
  terminal_response_enum: [Input] Specifies the _CNF for the response to
                                  the proactive cmd that the client wants to
                                  notify SIM/USIM
  general_result: [Input] Specifies the result of the proactive command
                          operation
  additional_result: [Input] Pointer specifies the optional additional result
                             for the proactive command, the length field in
                             the gstk_additonal_info_type specifies number of
                             additional_info in the structure
  other_info: [Input] A union pointer of various extra parameters for the
                      various proactive command response, please refer to
                      gstk_terminal_rsp_param_type for details

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_EXTRA_PARAM_MISSING,
    GSTK_EXTRA_PARAM_NOT_REQUIRED,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_terminal_response (
        gstk_client_id_type                     client_id,
        uint32                                  user_data,
        uint32                                  cmd_detail_ref_id,
        uint8                                   command_number,
        gstk_cmd_enum_type                      terminal_response_enum,
        gstk_general_result_enum_type           general_result,
        const gstk_additional_info_type         *additional_result,
        gstk_terminal_rsp_extra_param_type      *other_info )
{
    gstk_status_enum_type          gstk_status = GSTK_SUCCESS;
    gstk_additional_info_ptr_type  additional_ptr_result;

    /* initialize additional into ptr data */
    additional_ptr_result.length = 0;
    additional_ptr_result.additional_info_ptr = NULL;
    if ((additional_result != NULL) && (additional_result->length > 0)) {
      if (additional_result->length <= GSTK_MAX_ADDITIONAL_INFO)
      {
        additional_ptr_result.additional_info_ptr = GSTK_CALLOC(
                                                    additional_result->length);
        if (additional_ptr_result.additional_info_ptr == NULL) {
          return GSTK_MEMORY_ERROR;
        }
        additional_ptr_result.length = additional_result->length;
        (void)memscpy(additional_ptr_result.additional_info_ptr,
               additional_result->length,
               additional_result->additional_info,
               additional_result->length);
      }
      else
      {
        UIM_MSG_ERR_2("additional_result->length (0x%x) is too long (>= 0x%x)!",
                      additional_result->length, GSTK_MAX_ADDITIONAL_INFO);
        return GSTK_BAD_PARAM;
      }
    }

    if (other_info != NULL)
    {
      UIM_MSG_HIGH_1("Other_info is 0x%x", other_info->present);
    }
    gstk_status = gstk_send_terminal_response_ext(client_id,
                                    user_data,
                                    cmd_detail_ref_id,
                                    command_number,
                                    terminal_response_enum,
                                    general_result,
                                    &additional_ptr_result,
                                    other_info);

    /* free the info ptr if not null */
    if (additional_ptr_result.additional_info_ptr != NULL) {
      gstk_free(additional_ptr_result.additional_info_ptr);
      additional_ptr_result.additional_info_ptr = NULL;
    }

    return gstk_status;

} /*gstk_send_terminal_response*/

/*===========================================================================
FUNCTION gstk_send_raw_terminal_response

DESCRIPTION
  This function is served as a utility function to the gstk client, so that
  the client can call this function in sending a raw terminal response to
  a proactive command.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
 term_rsp: [Input] The raw terminal response that the client wants to send

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,               - Terminal response is sent successfully
    GSTK_CLIENT_NOT_REGISTERED, - The client ID passed in is invalid
    GSTK_BAD_PARAM,             - One of parameters passed in is invalid
    GSTK_MEMORY_ERROR           - Can't allocate memory

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_raw_terminal_response (
  gstk_client_id_type                     client_id,
  uint32                                  user_data,
  uint32                                  cmd_detail_ref_id,
  gstk_generic_data_type                  term_resp)
{
  gstk_status_enum_type   gstk_status             = GSTK_SUCCESS;
  gstk_task_cmd_type      *task_cmd_ptr           = NULL;
  uint8                   *gstk_term_resp_buff_ptr= NULL;
  uint32                  index                   = GSTK_CMD_REF_TABLE_INDEX(cmd_detail_ref_id);

  UIM_MSG_HIGH_2("Client send raw terminal rsp, client_id:0x%x, cmd_detail_ref_id #:0x%x",
                 client_id, cmd_detail_ref_id);

  gstk_status = gstk_util_compare_ref_id(cmd_detail_ref_id);
  if(gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* validate client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    /* it is not gstk calling this function either */
      UIM_MSG_ERR_1("Invalid Client ID 0x%x", client_id);
      return GSTK_CLIENT_NOT_REGISTERED;
    }

  /* raw terminal response data shoule be less than 255 bytes */
  if (term_resp.data_len > GSTK_MAX_DATA_SIZE)
  {
    UIM_MSG_ERR_1("Raw Terminal Response Data is too big (>%d bytes)!",
                  GSTK_MAX_DATA_SIZE);
    return GSTK_BAD_PARAM;
  }

  if (index >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_details_ref_id (0x%x) is invalid!", cmd_detail_ref_id);
    return GSTK_BAD_PARAM;
  }

  if (command_details_ref_table_ptr[index].free_slot)
  {
    UIM_MSG_ERR_1("Invalid cmd_detail_ref_id 0x%x in gstk_send_raw_terminal_response()!",
                  cmd_detail_ref_id);
    return GSTK_BAD_PARAM;
  }

  if (term_resp.data_len <= 0 || term_resp.data_buffer_ptr == NULL)
  {
    UIM_MSG_ERR_2("Empty term resp passed in! data_len: %d, data_buffer_ptr: 0x%x",
                  term_resp.data_len, term_resp.data_buffer_ptr);
    return GSTK_BAD_PARAM;
  }

  /* check if data len is at least bigger than GSTK_CMD_TYPE_OFFSET before accessing it */
  if (term_resp.data_len <= GSTK_CMD_TYPE_OFFSET)
  {
    UIM_MSG_ERR_2("data_len (0x%x) is smaller than cmd type offset (0x%x)!",
                  term_resp.data_len, GSTK_CMD_TYPE_OFFSET);
    return GSTK_BAD_PARAM;
  }

  /* check data len is bigger than cmd details length */
  if (term_resp.data_len <=
      term_resp.data_buffer_ptr[GSTK_CMD_DETAILS_LEN_OFFSET])
  {
    UIM_MSG_ERR_2("data_len (0x%x) is smaller than length of cmd details (0x%x)!",
                  term_resp.data_len,
                  term_resp.data_buffer_ptr[GSTK_CMD_DETAILS_LEN_OFFSET]);
    return GSTK_BAD_PARAM;
  }

  /* check command number */
  if (command_details_ref_table_ptr[index].command_details.command_number
      != term_resp.data_buffer_ptr[GSTK_CMD_NUMBER_OFFSET])
  {
    UIM_MSG_ERR_2("Cmd num in raw term rsp is unexpected! expecting: 0x%x, passed in 0x%x",
                  command_details_ref_table_ptr[index].command_details.command_number,
                  term_resp.data_buffer_ptr[GSTK_CMD_NUMBER_OFFSET]);
    return GSTK_BAD_PARAM;
  }

  /* check expected type of command */
  if (command_details_ref_table_ptr[index].command_details.type_of_command
      != term_resp.data_buffer_ptr[GSTK_CMD_TYPE_OFFSET])
  {
    UIM_MSG_ERR_2("Command type in raw terminal response is unexpected! expecting 0x%x, passed in 0x%x",
                  command_details_ref_table_ptr[index].command_details.type_of_command,
                  term_resp.data_buffer_ptr[GSTK_CMD_TYPE_OFFSET]);
    return GSTK_BAD_PARAM;
  }

  /* allocate terminal response buffer used in GSTK */
  gstk_term_resp_buff_ptr = (uint8 *)GSTK_CALLOC(int32touint32(term_resp.data_len));
  if (gstk_term_resp_buff_ptr == NULL) {
    return GSTK_MEMORY_ERROR;
  }
  else {  /* copy client terminal response data to GSTK buffer */
    (void)gstk_memcpy(gstk_term_resp_buff_ptr,
                      term_resp.data_buffer_ptr,
                      int32touint32(term_resp.data_len),
                      int32touint32(term_resp.data_len),
                      int32touint32(term_resp.data_len));
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr != NULL ) { /* successfully obtained command buffer */
    /* populate command buffer */
    task_cmd_ptr->cmd.client_raw_term_resp.message_header.command_group =
      GSTK_TERMINAL_RSP;
    task_cmd_ptr->cmd.client_raw_term_resp.message_header.command_id    =
      int32touint32((int32)GSTK_RAW_TERMIMAL_RESPONSE);
    task_cmd_ptr->cmd.client_raw_term_resp.cmd_details_ref_id           =
      cmd_detail_ref_id;
    task_cmd_ptr->cmd.client_raw_term_resp.message_header.user_data     =
      user_data;
    task_cmd_ptr->cmd.client_raw_term_resp.client_id                    =
      client_id;
    task_cmd_ptr->cmd.client_raw_term_resp.raw_term_resp.data_len       =
      term_resp.data_len;
    task_cmd_ptr->cmd.client_raw_term_resp.raw_term_resp.data_buffer_ptr=
      gstk_term_resp_buff_ptr;
    task_cmd_ptr->cmd.client_raw_term_resp.message_header.sim_slot_id   =
      gstk_util_get_sim_slot_id(client_id);

    /* put command into queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else { /* command details info not okay */
    gstk_free(gstk_term_resp_buff_ptr);
    UIM_MSG_ERR_0("No GSTK Cmd Buf for Raw Terminal Resp Info! client_id: 0x%x, cmd_detail_ref_id: 0x%x");
    gstk_status = GSTK_MEMORY_ERROR;
  }

  return gstk_status;
} /*gstk_send_raw_terminal_response*/

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_LOC_INFO_COMMAND

DESCRIPTION

  This function sends the location information envelope command to the USIM
  This function does not require client ID because it is intended to be called
  by internal modules.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_slot_client_init_ext()
  loc_info:  [Input] Location information consisting of mcc and mnc
  user_data: [Input] User data that client wants GSTK to keep track of

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_general_envelope_rsp_enum_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_loc_info_command (
  gstk_client_id_type     client_id,
  const uint8           * loc_info_ptr,
  uint32                  user_data)
{
  gstk_task_cmd_type    * task_cmd_ptr = NULL;
  gstk_slot_id_enum_type  slot         = GSTK_SLOT_ID_MAX;

  UIM_MSG_HIGH_0("GSTK client location information");
  if(NULL == loc_info_ptr)
  {
    UIM_MSG_ERR_0("NULL Input Pointer");
    return GSTK_NULL_INPUT_PARAM;
  }

  /* validate client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    /* it is not gstk calling this function either */
    UIM_MSG_ERR_1("Client 0x%x Not Registered", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }
  slot = gstk_util_get_sim_slot_id(client_id);
  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("Invalid Slot 0x%x", slot);
    return GSTK_BAD_PARAM;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr == NULL )
  {
    return GSTK_MEMORY_ERROR;
  }

  task_cmd_ptr->cmd.location_information_envelope_ind.message_header.client_id     =
    client_id;
  task_cmd_ptr->cmd.location_information_envelope_ind.message_header.sim_slot_id   =
    slot;
  task_cmd_ptr->cmd.location_information_envelope_ind.message_header.command_group =
    GSTK_ENVELOPE_CMD;
  task_cmd_ptr->cmd.location_information_envelope_ind.message_header.command_id    =
    (int)GSTK_LOCATION_INFORMATION_IND;
  task_cmd_ptr->cmd.location_information_envelope_ind.message_header.user_data     =
    user_data;

  (void)memscpy(task_cmd_ptr->cmd.location_information_envelope_ind.mcc_mnc,
         sizeof(task_cmd_ptr->cmd.location_information_envelope_ind.mcc_mnc),
         loc_info_ptr,
         GSTK_MAX_PLMN_LEN);

  /* set GSTK_CMD_Q_SIG and put on GSTK queue */
  gstk_task_put_cmd_buf_to_head(task_cmd_ptr);

  return GSTK_SUCCESS;
}/*gstk_send_envelope_loc_info_command*/

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_EVT_DOWNLOAD_COMMAND

DESCRIPTION

  This function sends the event download envelope command to the SIM/USIM
  Client is required to send the evt_list as the parameter to this function
  The gstk_evt_dl_ind_type is a struct where the first element is an enum
  type to indicate what events are being sent to the SIM/USIM in this
  command.
  Based on the enum value, client is required to fill in the specific
  information related to that event indication.
  This function does not require client ID because it is intended to be called
  by internal modules.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  evt_list: [Input] Pointer to the event download info

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_general_envelope_rsp_enum_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_evt_download_command (
  gstk_client_id_type         client_id,
  uint32                      user_data,
  gstk_evt_dl_ind_type      * evt_list
)
{
  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type    * task_cmd    = NULL;
  gstk_slot_id_enum_type  slot        = gstk_util_get_sim_slot_id(client_id);

  UIM_MSG_HIGH_0("GSTK client send evt dl");

  /* validate client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    /* it is not gstk calling this function either */
    UIM_MSG_ERR_1("Client 0x%x Not Registered", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  if (evt_list == NULL)
  {
    UIM_MSG_ERR_0("Null Input Pointer");
    return GSTK_ERROR;
  }

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd != NULL)
  {
    /* successfully obtained command buffer - build the evt download envelope command*/
    task_cmd->cmd.general_cmd.message_header.client_id   = client_id;
  task_cmd->cmd.general_cmd.message_header.sim_slot_id = slot;
    gstk_status = gstk_util_build_evt_dl_ind(task_cmd, user_data, evt_list);
    if (gstk_status == GSTK_SUCCESS)
    {
      /* set GSTK_CMD_Q_SIG and put on GSTK queue */
      if ((gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
             slot) == TRUE)
          &&
          evt_list->evt_type == GSTK_LOCATION_STATUS_EVT_IND)
      {
        gstk_task_put_cmd_buf_to_head(task_cmd);
      }
      else
      {
        gstk_task_put_cmd_buf(task_cmd);
      }
    }
    else
    {
      UIM_MSG_ERR_3("Unable to build EVT DL for 0x%x, status=0x%x, slot = 0x%x",
                  evt_list->evt_type, gstk_status, slot);
      /* put the queue back to the free queue */
      gstk_task_free_cmd_buf(task_cmd);
    }
  }
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }
  return gstk_status;
} /*lint !e818 *evt_list suppression for externalized function */
/* gstk_send_envelope_evt_download_command */


/*===========================================================================
FUNCTION GSTK_SLOT_SEND_ENVELOPE_MO_SMS_CTRL_COMMAND_EXT

DESCRIPTION

  This function is used to send MO SMS control to a specific SIM/USIM slot
  within the gstk_mo_sms_ctrl_ind_type structure, user needs to
  populate the RP_Destination_Address, TP_Destination_Address

PARAMETERS
  slot: [Input] The SIM slot ID
  user_data: [Input] User data that client wants GSTK to keep track of
  mo_sms_control: [Input] The MO SMS control info
  gstk_mo_sms_ctrl_cb: [Input] Function pointer to which GSTK will send
                               the card response data corresponding to the
                               SMS MO Control command

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_envelope_mo_sms_ctrl_command_ext (
  gstk_slot_id_enum_type                slot,
  uint32                                user_data,
  const gstk_mo_sms_ctrl_ind_ext_type   *mo_sms_control_data_ptr,
  gstk_evt_cb_funct_type                gstk_mo_sms_ctrl_cb
)
{
  gstk_task_cmd_type    *task_cmd = NULL;

  UIM_MSG_HIGH_3("gstk_slot_send_envelope_mo_sms_ctrl_command_ext: "
                 "slot=0x%x, "
                 "mo_sms_control_data_ptr=%p, "
                 "gstk_mo_sms_ctrl_cb=%p",
                 slot, mo_sms_control_data_ptr, gstk_mo_sms_ctrl_cb);

  /* validate slot ID*/
  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return GSTK_BAD_PARAM;
  }

  if(gstk_instances_ptr[(uint32)slot-1] == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* validate call back function */
  if (gstk_mo_sms_ctrl_cb == NULL || mo_sms_control_data_ptr == NULL)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  UIM_MSG_HIGH_3("gstk_slot_send_envelope_mo_sms_ctrl_command_ext: "
                 "mo_sms_control_data_ptr->rat=0x%x, "
                 "mo_sms_control_data_ptr->rp_dest_address.length=0x%x, "
                 "mo_sms_control_data_ptr->tp_dest_address.length=0x%x",
                 mo_sms_control_data_ptr->rat,
                 mo_sms_control_data_ptr->rp_dest_address.length,
                 mo_sms_control_data_ptr->tp_dest_address.length);

  /* check for mandatory data, both TP and RP are needed */
  if((!mo_sms_control_data_ptr->rp_dest_address.length) ||
     (!mo_sms_control_data_ptr->tp_dest_address.length))
  {
    return GSTK_BAD_PARAM;
  }

  switch (mo_sms_control_data_ptr->rat)
  {
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
  case GSTK_ACCESS_TECH_GSM:
  case GSTK_ACCESS_TECH_UTRAN:
  case GSTK_ACCESS_TECH_LTE:
    gstk_util_log_gwl_info(&(mo_sms_control_data_ptr->loc_info.gwl_info));
    break;
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE */
  case GSTK_ACCESS_TECH_CDMA:
    gstk_util_log_cdma_info(&(mo_sms_control_data_ptr->loc_info.cdma_info));
    break;
  case GSTK_ACCESS_TECH_HRPD:
  case GSTK_ACCESS_TECH_EHRPD:
    if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, slot) == FALSE)
    {
      return GSTK_BAD_PARAM;
    }
    break;
  case GSTK_ACCESS_TECH_WLAN:
    break;
  case GSTK_ACCESS_NONE:
    /*RAT input is mandatory*/
    return GSTK_BAD_PARAM;
  default:
    break;
  }

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if ( task_cmd != NULL )
  {
    /* successfully obtained command buffer - build the envelope command */
    task_cmd->cmd.mo_sms_ctrl_envelope_ind.
      message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.mo_sms_ctrl_envelope_ind.
      message_header.command_id    = (int)GSTK_MO_SMS_CTRL_IND;
    task_cmd->cmd.mo_sms_ctrl_envelope_ind.
      message_header.user_data     = user_data;
    task_cmd->cmd.mo_sms_ctrl_envelope_ind.message_header.sim_slot_id   = slot;
    task_cmd->cmd.mo_sms_ctrl_envelope_ind.
      message_header.client_id     = gstk_instances_ptr[(uint32)slot-1]->client_id;
    task_cmd->cmd.mo_sms_ctrl_envelope_ind.client_cb = gstk_mo_sms_ctrl_cb;

    /* Static members only, copy data from mo_sms_control_data_ptr */
    task_cmd->cmd.mo_sms_ctrl_envelope_ind.mo_sms_ctrl_data =
      *mo_sms_control_data_ptr;

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_slot_send_envelope_mo_sms_ctrl_command_ext */


/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_MENU_SEL_COMMAND

DESCRIPTION

  This function sends the menu selection envelope command down to SIM/USIM.
  Clients need to provide the item identifier for the user selected item.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the value
                     returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  item_chosen: [Input] User selected item.
                       Valid value [0x01, 0xFF],
                       0x00 is used to indicate a null item identifier
  help_requested: [Input] Optional data field in the envelope command:
                          TRUE -> help is requested
                          FALSE -> help is not requested and should not
                                   be sent down to the SIM/USIM

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_general_envelope_rsp_enum_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_menu_sel_command (
        gstk_client_id_type            client_id,
        uint32                         user_data,
        uint8                          item_chosen,
        boolean                        help_requested )
{
  return gstk_send_envelope_menu_sel_command_ext(client_id,
                                                 user_data,
                                                 item_chosen,
                                                 help_requested,
                                                 NULL);
} /* gstk_send_envelope_menu_sel_command */

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_MENU_SEL_COMMAND_EXT

DESCRIPTION

  This function sends the menu selection envelope command down to SIM/USIM.
  Clients need to provide the item identifier for the user selected item.

PARAMETERS
  client_id     : [Input] Client id of the calling client. This should be the value
                          returned from the gstk_slot_client_init_ext()
  user_data     : [Input] User data that client wants GSTK to keep track of
  item_chosen   : [Input] User selected item.
                          Valid value [0x01, 0xFF],
                          0x00 is used to indicate a null item identifier
  help_requested: [Input] Optional data field in the envelope command:
                          TRUE -> help is requested
                          FALSE -> help is not requested and should not
                                   be sent down to the SIM/USIM
  gstk_raw_env_cb:[Input] Callback function

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  GSTK_MENU_IND_RSP in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_menu_sel_command_ext (
        gstk_client_id_type            client_id,
        uint32                         user_data,
        uint8                          item_chosen,
        boolean                        help_requested,
        gstk_evt_cb_funct_type         gstk_raw_env_cb)
{
  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type    * task_cmd    = NULL;
  uint32                  i           = 0;
#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
 estk_cmd_type            *estk_cmd_ptr = NULL;
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/

  UIM_MSG_HIGH_2("GSTK client send menu selection extension Extension, 0x%x 0x%x",
                 item_chosen, help_requested);

  /* validate client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    /* it is not gstk calling this function either */
    UIM_MSG_ERR_1("Client 0x%x not registered", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  /* check if another menu selection message is already in progress */
  for (i = 0; i <= GSTK_MAX_PROACTIVE_COMMAND; i++)
  {
    if ((gstk_shared_data.envelope_cmd_ref_table[i].is_free == FALSE) &&
        (gstk_shared_data.envelope_cmd_ref_table[i].expected_cmd_rsp_type == GSTK_MENU_IND_RSP))
    {
      UIM_MSG_ERR_1("GSTK Menu Selection message already in progress. Client ID: 0x%x",
                    client_id);
      return GSTK_ERROR;
    }
  }

#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
  if (gstk_shared_data.gstk_client_table[client_id - 1].client_type !=
      GSTK_ESTK_TYPE)
  {
    if(GSTK_IDLE_S_RECOVERY(GSTK_SLOT_1))
    {
      UIM_MSG_ERR_0("gstk_send_envelope_menu_sel_command_ext: GSTK_IDLE_S_RECOVERY");
      return GSTK_ERROR;
    }

    estk_cmd_ptr = estk_get_cmd_buf();
    if (estk_cmd_ptr != NULL)
    {
      estk_cmd_ptr->hdr.cli_user_data  = user_data;
      estk_cmd_ptr->hdr.cmd            = ESTK_MENU_SEL_REQ_CMD;
      estk_cmd_ptr->hdr.cmd_ref_id     = 0;     /*doesn't matter*/
      estk_cmd_ptr->hdr.command_number = 0;     /*doesn't matter*/
      estk_cmd_ptr->hdr.sim_slot_id = GSTK_SLOT_1; /* doesn't matter */
      estk_cmd_ptr->cmd_data.menu_sel_req.chosen_item_id = item_chosen;
      estk_cmd_ptr->cmd_data.menu_sel_req.request_help   = help_requested;
      estk_cmd_ptr->estk_raw_evt_cb                      = gstk_raw_env_cb;
      estk_put_cmd(estk_cmd_ptr);
      return GSTK_SUCCESS;
    }
    else
    {
      UIM_MSG_ERR_0("failed to get estk cmd buf!");
      return GSTK_ERROR;
    }
  }
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd != NULL)
  {
    /* successfully obtained command buffer - build the envelope command */
    task_cmd->cmd.menu_selection_envelope_ind.message_header.client_id     = client_id;
    task_cmd->cmd.menu_selection_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.menu_selection_envelope_ind.message_header.command_id    = (int)GSTK_MENU_SELECTION_IND;
    task_cmd->cmd.menu_selection_envelope_ind.message_header.sim_slot_id   = gstk_util_get_sim_slot_id(client_id);
    task_cmd->cmd.menu_selection_envelope_ind.message_header.user_data     = user_data;
    task_cmd->cmd.menu_selection_envelope_ind.chosen_item_id               = item_chosen;
    task_cmd->cmd.menu_selection_envelope_ind.request_help                 = help_requested;
    task_cmd->cmd.menu_selection_envelope_ind.client_cb                    = gstk_raw_env_cb;

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  return gstk_status;
} /* gstk_send_envelope_menu_sel_command_ext */

/*===========================================================================
FUNCTION GSTK_SEND_RAW_ENVELOPE_COMMAND

DESCRIPTION

  This function is used to send raw envelope command down to SIM/USIM/RUIM.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the value
                     returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  env_cmd:   [Input] The envelope command data to be sent to SIM/USIM
  env_cmd_cb:[Input] Callback function

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,               - The envelopse command is sent successfully
    GSTK_MEMORY_ERROR,          - Can't allocate memory or envelope command information buffer
    GSTK_BAD_PARAM,             - The parameter passed in is invalid
    GSTK_CLIENT_NOT_REGISTERED  - The client is not registered

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_raw_envelope_rsp_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_raw_envelope_command (
  gstk_client_id_type          client_id,
  uint32                       user_data,
  gstk_generic_data_type       env_cmd,
  gstk_evt_cb_funct_type       gstk_raw_env_cb)
{
  gstk_status_enum_type   gstk_status  = GSTK_SUCCESS;
  gstk_task_cmd_type    * task_cmd_ptr = NULL;
#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
  boolean                 help_request = FALSE;
  uint8                   item_chosen  = 0;
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/

  UIM_MSG_HIGH_1("GSTK client send raw envelope of len 0x%x",
                 env_cmd.data_len);

  /* validate client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    /* it is not gstk calling this function either */
    UIM_MSG_ERR_1("Client 0x%x not registered", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  if (env_cmd.data_len <= 0)
  {
    UIM_MSG_ERR_1("invalid env cmd data len! 0x%x", env_cmd.data_len);
    return GSTK_BAD_PARAM;
  }

  if (env_cmd.data_buffer_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Ptr!");
    return GSTK_BAD_PARAM;
  }

#ifdef FEATURE_ESTK_SETUP_MENU_INTERFACE
  if (env_cmd.data_buffer_ptr[0] == 0xD3)
  {
    if (env_cmd.data_len < 9)
    {
      UIM_MSG_ERR_1("invalid len of env cmd: 0x%x", env_cmd.data_len);
      return GSTK_BAD_PARAM;
    }
    else
    {
      item_chosen = env_cmd.data_buffer_ptr[8];
      if (env_cmd.data_len > 9)
      {
        help_request = TRUE;
      }
    }

    /* menu selection envelope cmd */
    gstk_status = gstk_send_envelope_menu_sel_command_ext(
                    client_id,
                    user_data,
                    item_chosen,
                    help_request,
                    gstk_raw_env_cb);
    return gstk_status;
  }
#endif /*FEATURE_ESTK_SETUP_MENU_INTERFACE*/

  /* get command buf */
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr != NULL )
  {
    /* successfully obtained command buffer - build the envelope command */
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.client_id     = client_id;
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.sim_slot_id   = gstk_util_get_sim_slot_id(client_id);
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.command_id    = (int)GSTK_RAW_ENVELOPE_IND;
    task_cmd_ptr->cmd.raw_envelope_ind.message_header.user_data     = user_data;
    task_cmd_ptr->cmd.raw_envelope_ind.client_cb                    = gstk_raw_env_cb;

    task_cmd_ptr->cmd.raw_envelope_ind.env_len      = int32touint32(env_cmd.data_len);
    task_cmd_ptr->cmd.raw_envelope_ind.env_data_ptr = GSTK_CALLOC(int32touint32(env_cmd.data_len));
    if (task_cmd_ptr->cmd.raw_envelope_ind.env_data_ptr == NULL)
    {
      gstk_task_free_cmd_buf(task_cmd_ptr);
      return GSTK_MEMORY_ERROR;
    }

    (void)gstk_memcpy(task_cmd_ptr->cmd.raw_envelope_ind.env_data_ptr,
                      env_cmd.data_buffer_ptr,
                      int32touint32(env_cmd.data_len),
                      task_cmd_ptr->cmd.raw_envelope_ind.env_len,
                      int32touint32(env_cmd.data_len));

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }

  return gstk_status;
} /* gstk_send_raw_envelope_command */

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_SMS_PP_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a sms pp download command to the
  SIM/USIM.

PARAMETERS
  user_data: [Input] User data that client wants GSTK to keep track of
  is_cdma_sms_pp: [Input] Indicates whether it is CDMA or non CDMA SMS
                          PP download
  gstk_address_type: [Input] Optional according to standard specification.
                             user can use the length field to indicate
                             whether there is an address or not
                             (length = 0 => GSTK should not look at the
                             address parameter in constructing the envelope
                             command).
                             When the length != 0, the address field should
                             hold the RP_Originating_Address of the Service
                             Center
  sms_tpdu: [Input] Pointer indicating the SMS TPDU for the PP download
                    command
  gstk_sms_pp_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             SMS PP download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_sms_pp_dl_command (
    uint32                         user_data,
    boolean                        is_cdma_sms_pp,
    const gstk_address_type       *address,
    const gstk_sms_tpdu_type      *sms_tpdu,
    gstk_evt_cb_funct_type         gstk_sms_pp_dl_cb
)
{
  return gstk_slot_send_envelope_sms_pp_dl_command(
           GSTK_SLOT_1,
           user_data,
           is_cdma_sms_pp,
           address,
           sms_tpdu,
           gstk_sms_pp_dl_cb);
}

/*===========================================================================
FUNCTION GSTK_SLOT_SEND_ENVELOPE_SMS_PP_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a sms pp download command to a
  specific SIM/USIM slot

PARAMETERS
  slot: [Input] The SIM slot ID
  user_data: [Input] User data that client wants GSTK to keep track of
  is_cdma_sms_pp: [Input] Indicates whether it is CDMA or non CDMA SMS
                          PP download
  gstk_address_type: [Input] Optional according to standard specification.
                             user can use the length field to indicate
                             whether there is an address or not
                             (length = 0 => GSTK should not look at the
                             address parameter in constructing the envelope
                             command).
                             When the length != 0, the address field should
                             hold the RP_Originating_Address of the Service
                             Center
  sms_tpdu: [Input] Pointer indicating the SMS TPDU for the PP download
                    command
  gstk_sms_pp_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             SMS PP download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_envelope_sms_pp_dl_command (
  gstk_slot_id_enum_type          slot,
  uint32                          user_data,
  boolean                         is_cdma_sms_pp,
  const gstk_address_type       * address_ptr,
  const gstk_sms_tpdu_type      * sms_tpdu_ptr,
  gstk_evt_cb_funct_type          gstk_sms_pp_dl_cb
)
{
  gstk_task_cmd_type      * task_cmd = NULL;

  UIM_MSG_HIGH_0("GSTK client sms pp dl");

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return GSTK_BAD_PARAM;
  }

  if(gstk_instances_ptr[(uint32)slot-1] == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* validate call back function and Input Pointers. Address is optional */
  if (gstk_sms_pp_dl_cb == NULL || sms_tpdu_ptr == NULL || sms_tpdu_ptr->tpdu == NULL)
  {
    UIM_MSG_ERR_0("Null Input Param");
    UIM_MSG_ERR_3("gstk_sms_pp_dl_cb is 0x%x, sms_tpdu is 0x%x, sms_tpdu->tpdu is 0x%x",
                  gstk_sms_pp_dl_cb,
                  sms_tpdu_ptr,
                  (sms_tpdu_ptr != NULL)?sms_tpdu_ptr->tpdu:0);
    return GSTK_NULL_INPUT_PARAM;
  }

  if (sms_tpdu_ptr->length == 0)
  {
    /* mandatory param */
    UIM_MSG_ERR_0("SMS TPDU len = 0");
    return GSTK_BAD_PARAM;
  }

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd != NULL)
  { /* successfully obtained command buffer - build the envelope command */
    task_cmd->cmd.sms_pp_envelope_ind.message_header.client_id     = gstk_instances_ptr[(uint32)slot-1]->client_id;
    task_cmd->cmd.sms_pp_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.sms_pp_envelope_ind.message_header.command_id    = (int)GSTK_SMS_PP_DOWNLOAD_IND;
    task_cmd->cmd.sms_pp_envelope_ind.message_header.user_data     = user_data;
    task_cmd->cmd.sms_pp_envelope_ind.message_header.sim_slot_id   = slot;
    task_cmd->cmd.sms_pp_envelope_ind.is_cdma_sms_pp               = is_cdma_sms_pp;
    task_cmd->cmd.sms_pp_envelope_ind.client_cb                    = gstk_sms_pp_dl_cb;
    /* Copy TPDU Data */
    task_cmd->cmd.sms_pp_envelope_ind.tpdu.length = sms_tpdu_ptr->length;
    task_cmd->cmd.sms_pp_envelope_ind.tpdu.tpdu   = GSTK_CALLOC(sms_tpdu_ptr->length);
    if (task_cmd->cmd.sms_pp_envelope_ind.tpdu.tpdu == NULL)
    {
      gstk_task_free_cmd_buf(task_cmd);
      return GSTK_MEMORY_ERROR;
    }

    (void)memscpy(task_cmd->cmd.sms_pp_envelope_ind.tpdu.tpdu, sms_tpdu_ptr->length,
                  sms_tpdu_ptr->tpdu, sms_tpdu_ptr->length) ;
    /* optional address */
    task_cmd->cmd.sms_pp_envelope_ind.address.length = 0;

    if(address_ptr != NULL)
    {
      if(address_ptr->length != 0)
      {
        task_cmd->cmd.sms_pp_envelope_ind.address.length = address_ptr->length;
        task_cmd->cmd.sms_pp_envelope_ind.address.TON    = address_ptr->TON;
        task_cmd->cmd.sms_pp_envelope_ind.address.NPI    = address_ptr->NPI;
        (void)memscpy(task_cmd->cmd.sms_pp_envelope_ind.address.address,
               sizeof(task_cmd->cmd.sms_pp_envelope_ind.address.address),
               address_ptr->address, address_ptr->length);
      }
    }

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }

  return GSTK_SUCCESS;
} /*gstk_slot_send_envelope_sms_pp_dl_command*/


/*===========================================================================
FUNCTION GSTK_UPDATE_PROFILE_DOWNLOAD_VALUE

DESCRIPTION

  This function allows clients to update the terminal profile before gstk
  sends it down to gsdi and uim server.
  GSTK will send the profile download events to the card after it receives
  the first profile download update value from one of its client.
  GSTK will ignore the subsequence profile download updates.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  num_updates: [Input] Specifies the number of gstk_profile_dl_support_type
                       that the clients are specifying in this function
  value_list: [Input] Pointer to list of new updates

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_PROFILE_VALUE_NOT_ALLOWED,
    GSTK_PROFILE_ALREADY_SENT,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_FILE_ACCESS_FAIL,
    GSTK_NOT_INIT

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_update_profile_download_value (
    gstk_client_id_type                    client_id,
    uint32                                 user_data,
    uint8                                  num_updates,
    const gstk_profile_dl_support_type    *value_list )
{
    gstk_status_enum_type            gstk_status = GSTK_SUCCESS;
    int                              i           = 0;
    gstk_profile_dl_support_ext_type *tp_ext_ptr = NULL;

    UIM_MSG_HIGH_0("GSTK client profile download");

    if (value_list == NULL) {
        UIM_MSG_ERR_0("Null input param");
        gstk_status = GSTK_NULL_INPUT_PARAM;
        return gstk_status;
    }

    tp_ext_ptr = GSTK_CALLOC(
                   num_updates * sizeof(gstk_profile_dl_support_ext_type));
    if (tp_ext_ptr)
    {
      for (i = 0; i < num_updates; ++ i)
      {
        tp_ext_ptr[i].card_app_type = (uint8)GSTK_APP_NONE;
        tp_ext_ptr[i].profile_dl_bitmask = value_list[i].profile_dl_bitmask;
        tp_ext_ptr[i].value = value_list[i].value;
      }
      gstk_status = gstk_update_profile_download_value_ext(
                      client_id,
                      user_data,
                      num_updates,
                      tp_ext_ptr);
      gstk_free(tp_ext_ptr);
    }
    else
    {
      UIM_MSG_ERR_0("Failed to allocate tp_ext_ptr!");
      gstk_status = GSTK_MEMORY_ERROR;
    }

    return gstk_status;
} /*gstk_update_profile_download_value */


/*===========================================================================
FUNCTION GSTK_UPDATE_PROFILE_DOWNLOAD_VALUE_EXT

DESCRIPTION
  This function allows clients to update the terminal profile before gstk
  sends it down to gsdi and uim server.
  GSTK will send the profile download events to the card after it receives
  the first profile download update value from one of its client.
  GSTK will ignore the subsequence profile download updates.
  This function differs from function GSTK_UPDATE_PROFILE_DOWNLOAD_VALUE()
  as the former supports differentiating sat Vs usat terminal profile while the
  latter does not.

PARAMETERS
  client_id:     [Input] Client id of the calling client.  This should be the
                         value returned from the gstk_slot_client_init_ext()
  user_data:     [Input] User data that client wants GSTK to keep track of
  num_updates:   [Input] Specifies the number of gstk_profile_dl_support_type
                         that the clients are specifying in this function
  value_list_ptr:[Input] Pointer to list of new updates

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_PROFILE_VALUE_NOT_ALLOWED,
    GSTK_PROFILE_ALREADY_SENT,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_FILE_ACCESS_FAIL,
    GSTK_NOT_INIT

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_update_profile_download_value_ext (
    gstk_client_id_type                        client_id,
    uint32                                     user_data,
    uint8                                      num_updates,
    const gstk_profile_dl_support_ext_type    *value_list_ptr )
{
    gstk_status_enum_type    gstk_status   = GSTK_SUCCESS;
    gstk_task_cmd_type      *task_cmd_ptr  = NULL;
    int32                    i             = 0;
    uint32                   j             = 0;
    gstk_slot_id_enum_type   slot          = gstk_util_get_sim_slot_id(client_id);
    gstk_nv_items_data_type  nv_data;

    if (value_list_ptr == NULL) {
        UIM_MSG_ERR_0("gstk_update_profile_download_value_ext: Null input param");
        gstk_status = GSTK_NULL_INPUT_PARAM;
        return gstk_status;
    }

    if (!GSTK_IS_VALID_SLOT_ID(slot))
    {
      UIM_MSG_ERR_1("gstk_update_profile_download_value_ext: invalid slot 0x%x",
                    slot);
      return GSTK_ERROR;
    }

    /* Retrieve CAT Version */
    gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                        slot,
                        &nv_data);

    /* for each terminal profile data, we will copy 4 bytes to
       gstk_task_cmd_type.terminal_profile_cmd.data buffer */
    i = num_updates * 4;
    if (i > GSTK_MAX_TP_DATA_SIZE)
    {
      UIM_MSG_ERR_1("TP data is too long (0x%x)!", i);
      return GSTK_INVALID_LENGTH;
    }

    if (gstk_util_is_valid_client_id(client_id)) {
      /* get command buf */
      task_cmd_ptr = gstk_task_get_cmd_buf();
      if ( task_cmd_ptr != NULL ) { /* successfully obtained command buffer */
           /* build the terminal profile command */
           task_cmd_ptr->cmd.terminal_profile_cmd.message_header.command_group = GSTK_TERMINAL_PROFILE_CMD;
           task_cmd_ptr->cmd.terminal_profile_cmd.message_header.command_id = 0; /* doesn't matter */
           task_cmd_ptr->cmd.terminal_profile_cmd.message_header.user_data = user_data;
           task_cmd_ptr->cmd.terminal_profile_cmd.message_header.client_id = client_id;
         task_cmd_ptr->cmd.terminal_profile_cmd.message_header.sim_slot_id = slot;

           for (i = 0, j = 0; i < num_updates; i++)
           {
             if ((j + 3) >= GSTK_MAX_TP_DATA_SIZE)
             {
               UIM_MSG_ERR_0("terminal_profile_cmd.data is too small!");
               return GSTK_ERROR;
             }
             task_cmd_ptr->cmd.terminal_profile_cmd.data[j++] =
                 (uint8)value_list_ptr[i].card_app_type;
             task_cmd_ptr->cmd.terminal_profile_cmd.data[j++] =
                 (uint8)((value_list_ptr[i].profile_dl_bitmask & 0xFF00) >> 0x0008);
             task_cmd_ptr->cmd.terminal_profile_cmd.data[j++] =
                 (uint8)(value_list_ptr[i].profile_dl_bitmask & 0x00FF);
             if (j < GSTK_MAX_TP_DATA_SIZE)
             {
               if (gstk_tp_value_needs_to_be_disabled(value_list_ptr[i].profile_dl_bitmask,
                                                      nv_data.cat_version) == TRUE)
               {
                 task_cmd_ptr->cmd.terminal_profile_cmd.data[j++] = 0x00;
               }
               else
               {
                 task_cmd_ptr->cmd.terminal_profile_cmd.data[j++] =
                     value_list_ptr[i].value;
               }
             }
           }
           task_cmd_ptr->cmd.terminal_profile_cmd.length = int32touint32(j);
           /* put command back to command queue */
           gstk_task_put_cmd_buf(task_cmd_ptr);
      }
      else {
           gstk_status = GSTK_MEMORY_ERROR;
      }
    }
    else {
        UIM_MSG_ERR_1("Client 0x%x not registered", client_id);
        gstk_status = GSTK_CLIENT_NOT_REGISTERED;
    }
    return gstk_status;
} /*gstk_update_profile_download_value_ext */


/*===========================================================================
FUNCTION gstk_map_proactive_req_to_cnf

DESCRIPTION
  This function mapped the passed in command req type to the corresponding
  response type

PARAMETERS
  request_cmd: [Input] Original proactive command REQ to be mapped
  expected_cmd_rsp: [Input/Output] Pointer to response command type (CNF)
                                   to be populated

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type:
      GSTK_SUCCESS
      GSTK_UNSUPPORTED_COMMAND
      GSTK_INVALID_COMMAND

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_map_proactive_req_to_cnf(
  gstk_cmd_enum_type    request_cmd,
  gstk_cmd_enum_type  * expected_cmd_rsp)
{

  UIM_MSG_HIGH_0("client map req to cnf");

  if (expected_cmd_rsp == NULL)
  {
    UIM_MSG_ERR_0("Null Input Param");
    return GSTK_NULL_INPUT_PARAM;
  }

  switch(request_cmd)
  {
    case GSTK_DISPLAY_TEXT_REQ:
        *expected_cmd_rsp = GSTK_DISPLAY_TEXT_CNF;
        break;
    case GSTK_GET_INKEY_REQ:
        *expected_cmd_rsp = GSTK_GET_INKEY_CNF;
        break;
    case GSTK_GET_INPUT_REQ:
        *expected_cmd_rsp = GSTK_GET_INPUT_CNF;
        break;
    case GSTK_LAUNCH_BROWSER_REQ:
        *expected_cmd_rsp = GSTK_LAUNCH_BROWSER_CNF;
        break;
    case GSTK_MORE_TIME_REQ:
        *expected_cmd_rsp = GSTK_MORE_TIME_CNF;
        break;
    case GSTK_PLAY_TONE_REQ:
        *expected_cmd_rsp = GSTK_PLAY_TONE_CNF;
        break;
    case GSTK_POLL_INTERVAL_REQ:
        *expected_cmd_rsp = GSTK_POLL_INTERVAL_CNF;
        break;
    case GSTK_PROVIDE_LOCAL_INFO_REQ:
        *expected_cmd_rsp = GSTK_PROVIDE_LOCAL_INFO_CNF;
        break;
    case GSTK_PROVIDE_LANG_INFO_REQ:
        *expected_cmd_rsp = GSTK_PROVIDE_LANG_INFO_CNF;
        break;
    case GSTK_REFRESH_REQ:
        *expected_cmd_rsp = GSTK_REFRESH_CNF;
        break;
    case GSTK_RUN_AT_CMDS_REQ:
        *expected_cmd_rsp = GSTK_RUN_AT_CMDS_CNF;
        break;
    case GSTK_SELECT_ITEM_REQ:
        *expected_cmd_rsp = GSTK_SELECT_ITEM_CNF;
        break;
    case GSTK_SEND_SMS_REQ:
        *expected_cmd_rsp = GSTK_SEND_SMS_CNF;
        break;
    case GSTK_SEND_SS_REQ:
        *expected_cmd_rsp = GSTK_SEND_SS_CNF;
        break;
    case GSTK_SEND_USSD_REQ:
        *expected_cmd_rsp = GSTK_SEND_USSD_CNF;
        break;
    case GSTK_SETUP_CALL_REQ:
        *expected_cmd_rsp = GSTK_SETUP_CALL_CNF;
        break;
    case GSTK_SETUP_EVENT_LIST_REQ:
        *expected_cmd_rsp = GSTK_SETUP_EVENT_LIST_CNF;
        break;
    case GSTK_SETUP_MENU_REQ:
        *expected_cmd_rsp = GSTK_SETUP_MENU_CNF;
        break;
    case GSTK_SETUP_IDLE_TEXT_REQ:
        *expected_cmd_rsp = GSTK_SETUP_IDLE_TEXT_CNF;
        break;
    case GSTK_SEND_DTMF_REQ:
        *expected_cmd_rsp = GSTK_SEND_DTMF_CNF;
        break;
    case GSTK_LANG_NOTIFICATION_REQ:
        *expected_cmd_rsp = GSTK_LANG_NOTIFICATION_CNF;
        break;
    default:
        UIM_MSG_ERR_1("Unknown GSTK REQ Type 0x%x", request_cmd);
        *expected_cmd_rsp = GSTK_END_PROACTIVE_CMD_REQ;
        return GSTK_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_map_proactive_cmd_cnf_rsp*/


/*===========================================================================
FUNCTION gstk_parse_ton_npi

DESCRIPTION
  This function parse the ton and npi byte into gstk enum types

PARAMETERS
  TON_NPI: [Input] byte that contains TON and NPI info
  TON: [Input/Output] Pointer to TON
  NPI: [Input/Output] Pointer to NPI


DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type:
      GSTK_SUCCESS
      GSTK_NULL_INPUT_PARAM

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parse_ton_npi(
  uint8                TON_NPI,
  gstk_ton_enum_type * TON,
  gstk_npi_enum_type * NPI
)
{

  if (TON == NULL || NPI == NULL)
  {
    UIM_MSG_ERR_0("Null Input Para");
    return GSTK_NULL_INPUT_PARAM;
  }

  *TON = (gstk_ton_enum_type)((TON_NPI & GSTK_TON_MASK_VALUE) >> GSTK_TON_SHIFT_VALUE);
  *NPI = (gstk_npi_enum_type)(TON_NPI & GSTK_NPI_MASK_VALUE);
  UIM_MSG_HIGH_2("*TON=0x%x, *NPI=0x%x", *TON, *NPI);
  switch(*TON)
  {
  case GSTK_UNKNOWN_TON:
    *TON = GSTK_UNKNOWN_TON;
    break;
  case GSTK_INTERNATIONAL_NUM:
    *TON = GSTK_INTERNATIONAL_NUM;
    break;
  case GSTK_NATIONAL_NUM:
    *TON = GSTK_NATIONAL_NUM;
    break;
  case GSTK_NETWORK_SPECIFIC_NUM:
    *TON = GSTK_NETWORK_SPECIFIC_NUM;
    break;
  default:
    *TON = GSTK_RFU_TON;
    break;
  }


  switch(*NPI)
  {
  case GSTK_UNKNOWN_NPI:
   *NPI = GSTK_UNKNOWN_NPI;
   break;
  case GSTK_ISDN_TELEPHONY_NPI:
   *NPI = GSTK_ISDN_TELEPHONY_NPI;
   break;
  case GSTK_DATA_NPI:
   *NPI = GSTK_DATA_NPI;
   break;
  case GSTK_TELEX_NPI:
   *NPI = GSTK_TELEX_NPI;
   break;
  case GSTK_PRIVATE_NPI:
   *NPI = GSTK_PRIVATE_NPI;
   break;
  case GSTK_EXT_RESERVED_NPI:
   *NPI = GSTK_EXT_RESERVED_NPI;
   break;
  case GSTK_RFU_NPI:
  default:
   *NPI = GSTK_RFU_NPI;
   break;
  }
  return GSTK_SUCCESS;
}


/*              for internal module use                   */

/*===========================================================================
FUNCTION gstk_send_proactive_cmd

DESCRIPTION

  This function allows clients to put a proactive cmd request before parsing
  onto GSTK command queue
  queue.

PARAMETERS
  data_length: [Input] Length of the data
  data: [Input] Pointer to the data buffer that contains the Proactive cmd.
                If data == NULL, indicate that the current proactive command in
                progress is cancelled

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_proactive_cmd(
  uim_slot_type   slot,
  uint32          data_length,
  const uint8   * data
)
{
  gstk_status_enum_type     gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type      * task_cmd    = NULL;
  uint32                    cmd_bit     = 5;

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    gstk_shared_data.fetch_is_in_progress = FALSE;
    return GSTK_BAD_PARAM;
  }
  else if (data != NULL)
  {
    if (data_length > GSTK_MAX_DATA_SIZE || data_length < 9)
    {
      UIM_MSG_ERR_1("Data Len 0x%x > GSTK_MAX_DATA_SIZE", data_length);
      gstk_status = GSTK_PARAM_EXCEED_BOUNDARY;
    }
    else
    {
      UIM_MSG_HIGH_1("UI or NW proactive session in progress 0x%x",
                      gstk_shared_data.ui_nw_proactive_session_slot_in_progress);

      if (!gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_PCMD_SELECTIVE_PARALLEL_PROCESSING))
      {
        /* Check the command for parallel processing */
        if(data[1] == GSTK_2_BYTE_LENGTH_FIRST_VALUE)
        {
          cmd_bit++;
        }
        switch(data[cmd_bit])
        {
          case GSTK_CMD_STK_SET_UP_MENU:
          case GSTK_CMD_STK_SET_UP_EVENT_LIST:
          case GSTK_CMD_STK_MORE_TIME:
          case GSTK_CMD_STK_POLL_INTERVAL:
          case GSTK_CMD_STK_POLLING_OFF:
          case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
          case GSTK_CMD_STK_TIMER_MANAGEMENT:
          case GSTK_CMD_STK_LANG_NOTIFICATION:
          case GSTK_CMD_STK_ACTIVATE:
          case GSTK_CMD_STK_END_OF_PROACTIVE_SES:
            break;
          case GSTK_CMD_STK_REFRESH:
            if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA, GSTK_SLOT_ID_MAX) == FALSE)
            {
              if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND, 
                                            GSTK_SLOT_ID_MAX) == TRUE)
              {
                UIM_MSG_HIGH_0("Alpha will send to UI even in case of no alpha and null alpha");
                gstk_shared_data.ui_nw_proactive_session_slot_in_progress = (gstk_slot_id_enum_type)slot;
              }
              else
              {
                /* Cmd qualifier bit + Device Identities TLV + Tag*/
                cmd_bit += 1 + 4 + 1;
                /* Check for alpha tag */
                while((cmd_bit < data_length) && ((data[cmd_bit] & 0x7F) !=  GSTK_ALPHA_IDENTIFIER_TAG))
                {
                  /* Not alpha tag */
                  if(cmd_bit < (data_length - 1))
                  {
                    /* Increment length bytes */
                    cmd_bit += gstk_find_length_of_length_value(&data[cmd_bit + 1]);

                    if(cmd_bit < data_length)
                    {
                      /* length + Tag */
                      cmd_bit += data[cmd_bit] + 1;
                    }
                  }
                }
                if(cmd_bit >= data_length)
                {
                  UIM_MSG_HIGH_0("Refresh command with out alpha");
                }
                else
                {
                  if((cmd_bit < (data_length - 1)) && (data[cmd_bit + 1] != 0))
                  {
                    gstk_shared_data.ui_nw_proactive_session_slot_in_progress = (gstk_slot_id_enum_type)slot;
                  }
                }
              }
            }
            break;
          default:
            gstk_shared_data.ui_nw_proactive_session_slot_in_progress = (gstk_slot_id_enum_type)slot;
            break;
        }
      }
      else
      {
        gstk_shared_data.ui_nw_proactive_session_slot_in_progress = (gstk_slot_id_enum_type)slot;
      }
      /* get the command from buffer */
      task_cmd = gstk_task_get_cmd_buf();
      if (task_cmd != NULL)
      {
        /* Build the gstk_cmd */
        task_cmd->cmd.proactive_cmd.message_header.command_group = GSTK_PROACTIVE_CMD;
        task_cmd->cmd.proactive_cmd.message_header.command_id    = 0; /* doesn't matter */
        task_cmd->cmd.proactive_cmd.message_header.user_data     = 0;  /* doesn't matter */
        task_cmd->cmd.proactive_cmd.message_header.sim_slot_id =
          (gstk_slot_id_enum_type)slot;

        task_cmd->cmd.proactive_cmd.length = data_length;
        (void)memscpy(task_cmd->cmd.proactive_cmd.data,
               sizeof(task_cmd->cmd.proactive_cmd.data),
               data, data_length);
        /* put command back to command queue */
        gstk_task_put_cmd_buf(task_cmd);
      }
      else
      {
        gstk_status = GSTK_ERROR;
      }
    }
  }
  else
  {
    /* NULL command data indicates that there is no longer a command in progress */
    /* This can happen if there an error is returned in response to FETCH */
    gstk_status = GSTK_BAD_PARAM;
  }

  if(gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("Proactive Cmd ERR, %x", gstk_status);
    GSTK_SET_PROACTIVE_CMD_IN_PROGRESS_FLAG(
            gstk_instances_ptr[slot - 1]->gstk_proactive_cmd_in_progress, FALSE);
    if(gstk_shared_data.ui_nw_proactive_session_slot_in_progress == slot)
    {
      gstk_shared_data.ui_nw_proactive_session_slot_in_progress = GSTK_SLOT_ID_MAX;
    }
  }

  gstk_shared_data.fetch_is_in_progress = 0;
  if(gstk_shared_data.ui_nw_proactive_session_slot_in_progress != slot)
  {
    /* No need to check other slots, in case of non ui/NW proactive command in a
       UI/NW based proactive session. 0xFF != slot in case of non UI/NW session */
    gstk_util_check_slot_with_fetch_rejected();
  }
  return gstk_status;
} /* gstk_send_proactive_cmd */

/*===========================================================================
FUNCTION gstk_address_to_ascii

DESCRIPTION
  This function converts the gstk address foramt to ASCII value

PARAMETERS
  gstk_addr: [Input] Pointer to input address
  ascii_addr: [Input/Output] Pointer to output address
  ascii_len; [Input] Length of the ascii_addr buffer, to ensure the size of
                     the ascii_addr will not exceed the information that is
                     being convertedh of the ascii_addr buffer;


DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type:
      GSTK_SUCCESS
      GSTK_NULL_INPUT_PARAM

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_address_to_ascii(
  const gstk_address_type *gstk_addr,
  uint8                   *ascii_addr,
  uint8                    ascii_len)
{
  int i = 0;
  uint8 addr_len = 0;

  if (gstk_addr == NULL || ascii_addr == NULL)
  {
    UIM_MSG_ERR_0("Null Input Param");
    return GSTK_NULL_INPUT_PARAM;
  }
  if (ascii_len < gstk_addr->length)
  {
    UIM_MSG_ERR_2("ascii len 0x%x < gstk addr len 0x%x",
                  ascii_len, gstk_addr->length);
    return GSTK_BAD_PARAM;
  }

  memset(ascii_addr, 0x00, ascii_len);
  if (gstk_addr->length > GSTK_MAX_ADDRESS_LEN) {
    UIM_MSG_ERR_0("gstk_addr->length exceeds GSTK_MAX_ADDRESS_LEN");
    addr_len = GSTK_MAX_ADDRESS_LEN;
  }
  else {
    addr_len = gstk_addr->length;
  }
  for (i=0; i< addr_len; i++) {
    switch (gstk_addr->address[i])
    {
      case 0x0A:
        ascii_addr[i] = 0x2A;
        break;
      case 0x0B:
        ascii_addr[i] = 0x23;
        break;
      default:
        ascii_addr[i] = gstk_addr->address[i] + 0x30;
        break;
    }
  }
  return GSTK_SUCCESS;
} /* gstk_address_to_ascii */

/*===========================================================================
FUNCTION gstk_ascii_to_gstk_address_type

DESCRIPTION
  This function converts the ASCII value to gstk address format

PARAMETERS
  ascii_addr: [Input] Pointer to input address
  ascii_len: [Input] Input address length
  gstk_addr: [Input/Output] Pointer to output address


DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type:
      GSTK_SUCCESS
      GSTK_NULL_INPUT_PARAM

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_ascii_to_gstk_address_type(
  const uint8       *ascii_addr,
  uint8              ascii_len,
  gstk_address_type *gstk_addr)
{
  int i = 0;
  if (gstk_addr == NULL || ascii_addr == NULL)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  if (ascii_len > GSTK_MAX_ADDRESS_LEN) {
    UIM_MSG_ERR_1("ascii len 0x%x > GSTK_MAX_ADDRESS_LEN", ascii_len);
    return GSTK_BAD_PARAM;
  }
  memset(gstk_addr->address, 0x00, GSTK_MAX_ADDRESS_LEN);

  for (i = 0; i < ascii_len; i++)
  {
    switch (ascii_addr[i]) {
    case 0x2A:
      gstk_addr->address[i] = 0x0A;
      break;
    case 0x23:
      gstk_addr->address[i] = 0x0B;
      break;
    default:
      gstk_addr->address[i] = ascii_addr[i] - 0x30;
      break;
    }
  }
  gstk_addr->length = ascii_len;
  return GSTK_SUCCESS;

} /* gstk_ascii_to_gstk_address_type */

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_CELL_BROADCAST_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a cell broadcast download command to the
  SIM/USIM.

PARAMETERS
  user_data: [Input] User data that client wants GSTK to keep track of
  cb_page: [Input] Pointer indicating the Cell Broadcast page for the download
                    command
  gstk_cb_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             Cell Broadcast download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called.
  Client is responsible to pad the page to length 88 (31.111)

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_cell_broadcast_dl_command (
  uint32                     user_data,
  const gstk_cb_page_type   *cb_page_ptr,
  gstk_evt_cb_funct_type     gstk_cb_dl_cb
)
{
  return gstk_slot_send_envelope_cell_broadcast_dl_command(
           GSTK_SLOT_1,
           user_data,
           cb_page_ptr,
           gstk_cb_dl_cb);
}

/*===========================================================================
FUNCTION GSTK_SLOT_SEND_ENVELOPE_CELL_BROADCAST_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a cell broadcast download command to
  a specific SIM/USIM slot

PARAMETERS
  slot: [Input] The SIM slot ID
  user_data: [Input] User data that client wants GSTK to keep track of
  cb_page: [Input] Pointer indicating the Cell Broadcast page for the download
                    command
  gstk_cb_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             Cell Broadcast download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called.
  Client is responsible to pad the page to length 88 (31.111)

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_envelope_cell_broadcast_dl_command (
  gstk_slot_id_enum_type      slot,
  uint32                      user_data,
  const gstk_cb_page_type   * cb_page_ptr,
  gstk_evt_cb_funct_type      gstk_cb_dl_cb
)
{
  gstk_task_cmd_type    * task_cmd = NULL;

  UIM_MSG_HIGH_0("GSTK client cell broadcast dl");

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return GSTK_BAD_PARAM;
  }

  if(gstk_instances_ptr[(uint32)slot-1] == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* validate call back function */
  if(gstk_cb_dl_cb == NULL || cb_page_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null Input Param: gstk_cb_dl_cb = 0x%x cb_page_ptr = 0x%x",
                  gstk_cb_dl_cb, cb_page_ptr);
    return GSTK_NULL_INPUT_PARAM;
  }

  /* If padding is required, it is required to be done by the caller */
  if(cb_page_ptr->length != GSTK_CELL_BROADCAST_PAGE_LEN)
  {
    /* mandatory param */
    UIM_MSG_ERR_1("Cell Broadcast len = 0x%x", cb_page_ptr->length);
    return GSTK_BAD_PARAM;
  }

  if(cb_page_ptr->cb_page_data == NULL)
  {
    /* null page data */
    UIM_MSG_ERR_0("Cell Broadcast data NULL");
    return GSTK_BAD_PARAM;
  }

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd != NULL)
  {
    /* successfully obtained command buffer - build the envelope command */
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.sim_slot_id   = slot;
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.client_id     = gstk_instances_ptr[(uint32)slot-1]->client_id;
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.command_id    = (int)GSTK_CELL_BROADCAST_DOWNLOAD_IND;
    task_cmd->cmd.cell_broadcast_envelope_ind.message_header.user_data     = user_data;
    task_cmd->cmd.cell_broadcast_envelope_ind.client_cb                    = gstk_cb_dl_cb;

    /* Cell Broadcast Page */
    task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.length       = cb_page_ptr->length;
    task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.cb_page_data = GSTK_CALLOC(cb_page_ptr->length);
    if(task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.cb_page_data == NULL)
    {
      gstk_task_free_cmd_buf(task_cmd);
      return GSTK_MEMORY_ERROR;
    }
    (void)memscpy(task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.cb_page_data,
           cb_page_ptr->length,
           cb_page_ptr->cb_page_data,
           cb_page_ptr->length) ;

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }

  return GSTK_SUCCESS;
} /*gstk_slot_send_envelope_cell_broadcast_dl_command*/

/*===========================================================================
FUNCTION   gstk_queue_rrc_utran_nmr_rsp

DESCRIPTION
  Library function called by RRC to post an NMR response to GSTK for network
  measurement results for utran.

PARAMETERS
  status:              result status from RRC
  length:              Length of data returned
  nmr_info_ptr:        UTRAN NMR info returned from RRC
  data_ptr:            GSTK reference data returned back by RRC
  nmr_utran_qualifier: type of NMR

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  Max length of bytes sent by RRC should <= 0xFF

SIDE EFFECTS
  Will queue a UTRAN NMR response to GSTK

SEE ALSO
  None
===========================================================================*/
void gstk_queue_rrc_utran_nmr_rsp_ext(
  gstk_utran_status_enum_type        status,
  uint32                             length,
  const uint8                        *nmr_info_ptr,
  const void                         *data_ptr,
  gstk_nmr_utran_qualifier_enum_type nmr_utran_qualifier)
{
  gstk_task_cmd_type      * task_cmd_ptr = NULL;
  uint32                  ref_id         = 0;
  gstk_slot_id_enum_type  slot           = GSTK_SLOT_1;

  UIM_MSG_HIGH_3("gstk_queue_rrc_utran_nmr_rsp_ext:"
                 "status=0x%x, "
                 "length=0x%x, "
                 "nmr_utran_qualifier=0x%x",
                 status, length, nmr_utran_qualifier);

  /* do not check for nmr_info_ptr to be NULL, as RRC may return a NULL ptr for
     error case . In such a case GSTK needs to return an appropriate Terminal
     Response **Unable to process command**
  */

  if (data_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Data ptr nmr_info_ptr");
    return;
  }
  ref_id = *((uint32 *)data_ptr);
  if(GSTK_CMD_REF_TABLE_INDEX(ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_2("invalid ref id 0x%x CMD Index 0x%x", ref_id, GSTK_CMD_REF_TABLE_INDEX(ref_id));
    return;
  }
  slot = command_details_ref_table[GSTK_CMD_REF_TABLE_INDEX(ref_id)].sim_slot_id;
  UIM_MSG_HIGH_2("slot = 0x%x, ref_id = 0x%x", slot, ref_id);

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return;
  }

  GSTK_RETURN_IF_NULL_PTR(gstk_instances_ptr[(uint32)slot - 1]);

  /* get the command from buffer */
  task_cmd_ptr = gstk_task_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return;
  }

  switch(status)
  {
  case GSTK_UTRAN_NMR_RESULT_SUCCESS:
    if ( length == 0 && nmr_info_ptr == NULL )
    {
      UIM_MSG_HIGH_0("RRC returned Success for NMR with no data");
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result =
        GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 1;
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr =
        GSTK_CALLOC(sizeof(uint8));
      if (task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr
         == NULL)
      {
        /* put the queue back to the free queue */
        gstk_task_free_cmd_buf(task_cmd_ptr);
        return;
      }
      *(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr) =
        (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    }
    else
    {
      if(gstk_instances_ptr[(uint32)slot - 1]->gstk_next_curr_location_status == GSTK_LIMITED_SERVICE)
      {
        task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result =
          GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE;
      }
      else
      {
        task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result =
          GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      }
    }
    break;

  case GSTK_UTRAN_NMR_RESULT_FAIL_UE_ON_GERAN:
  case GSTK_UTRAN_NMR_RESULT_FAIL_UE_OOS:
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result =
      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 1;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr =
      GSTK_CALLOC(sizeof(uint8));
    if (task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr
       == NULL)
    {
      /* put the queue back to the free queue */
      gstk_task_free_cmd_buf(task_cmd_ptr);
      return;
    }
    *(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr) =
    (uint8)GSTK_NO_SERVICE_AVAILABLE;
    break;

  case GSTK_UTRAN_NMR_RESULT_FAIL_OTHER:
  case GSTK_UTRAN_NMR_RESULT_FAIL_ASID_MISMATCH:
  default:
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result =
      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 1;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr =
      GSTK_CALLOC(sizeof(uint8));
    if (task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr
       == NULL)
    {
      /* put the queue back to the free queue */
      gstk_task_free_cmd_buf(task_cmd_ptr);
      return;
    }
    *(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr) =
    (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    break;
  }

  /* Build the gstk_cmd */
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.command_group =
    GSTK_TERMINAL_RSP;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.command_id    =
    (uint32) GSTK_PROVIDE_LOCAL_INFO_CNF;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length =
    length;
  if ( length > 0 && nmr_info_ptr!=NULL )
  {
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length =
       GSTK_MIN(length, 0xFF);
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr =
      GSTK_CALLOC((sizeof(uint8)) *
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length);
      if (!task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr)
      {
        /* free the info ptr if not null */
        if (task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL)
        {
          gstk_free(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr);
          task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
        }
        /* put the queue back to the free queue */
        gstk_task_free_cmd_buf(task_cmd_ptr);
        return;
      }
      memset(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr,
             0x00,
             task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length);
      (void)memscpy(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr,
             task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length,
             nmr_info_ptr,
             task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length);
  }
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type     =
    GSTK_NMR_UTRAN_INFO;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.
    nmr_utran.nmr_utran_qualifier = nmr_utran_qualifier;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.user_data     = 0;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.sim_slot_id   = slot;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.cmd_details_ref_id           = ref_id;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_number               =
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(ref_id)].command_details.command_number;

  /* put command back to command queue */
  gstk_task_put_cmd_buf(task_cmd_ptr);

}/* gstk_queue_rrc_utran_nmr_rsp_ext */

/*===========================================================================
FUNCTION   gstk_queue_rrc_utran_nmr_rsp

DESCRIPTION
  Library function called by RRC to post an NMR response to GSTK for network
  measurement results for utran.

PARAMETERS
  status:       result status from RRC
  length:       Length of data returned
  nmr_info_ptr: UTRAN NMR info returned from RRC
  data_ptr:     GSTK reference data returned back by RRC

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  Max length of bytes sent by RRC should <= 0xFF

SIDE EFFECTS
  Will queue a UTRAN NMR response to GSTK

SEE ALSO
  None
===========================================================================*/
void gstk_queue_rrc_utran_nmr_rsp(
  gstk_utran_status_enum_type status,
  uint32                      length,
  const uint8               * nmr_info_ptr,
  const void                * data_ptr)
{
  UIM_MSG_HIGH_0(" IN gstk_queue_rrc_utran_nmr_rsp ");

  if (!data_ptr)
  {
    UIM_MSG_ERR_0("Null Data ptr nmr_info_ptr");
    return;
  }

  /* Use GSTK_NMR_NONE_MEAS for now */
  gstk_queue_rrc_utran_nmr_rsp_ext(status,
                                   length,
                                   nmr_info_ptr,
                                   data_ptr,
                                   GSTK_NMR_NONE_MEAS);
}/* gstk_queue_rrc_utran_nmr_rsp */

/*===========================================================================
FUNCTION   gstk_uim_fetch_allow

DESCRIPTION
  Library function called by UIM to check if UIM should do fetch or not
  depending on the following two condtions:
  1. whether the next command in gstk queue BIP Envelope or not
  2. whether there is a BIP Envelope that has been sent to UIM from gstk
     for which gstk has not received a response yet.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_uim_fetch_allow(
  gstk_slot_id_enum_type slot
)
{
  boolean ret_flag = FALSE;

#ifdef FEATURE_GSTK_TAL
  UIM_MSG_HIGH_1("TAL: tal_is_fetch_ok is %d", tal_is_fetch_ok);
  if(tal_is_fetch_ok == FALSE)
  {
    return tal_is_fetch_ok;
  }
#endif /*FEATURE_GSTK_TAL */

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return FALSE;
  }

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)slot - 1], FALSE);

  if(gstk_instances_ptr[(uint8)slot - 1]->gstk_sub_state == GSTK_RECOVERY_SUB_S)
  {
    UIM_MSG_HIGH_1("Not allowing Fetch as recovery is in progress for slot id 0x%x",
                   slot);
    gstk_instances_ptr[(uint8)slot - 1]->is_fetch_ignored = TRUE;
    return FALSE;
  }

  if(gstk_instances_ptr[(uint8)slot - 1]->is_tp_rsp_received != TRUE)
  {
    UIM_MSG_HIGH_1("Not allowing Fetch as TP download is not complete for slot id 0x%x",
                   slot);
    gstk_instances_ptr[(uint8)slot - 1]->is_fetch_ignored = TRUE;
    return FALSE;
  }

  if (gstk_shared_data.fetch_is_in_progress != 0 ||
      (gstk_shared_data.ui_nw_proactive_session_slot_in_progress != GSTK_SLOT_ID_MAX &&
       gstk_shared_data.ui_nw_proactive_session_slot_in_progress != slot))
  {
      UIM_MSG_HIGH_3("Don't allow slot 0x%x to fetch (PCMD on slot 0x%xor fetch command on slot 0x%x) is in progress !",
                     slot, gstk_shared_data.ui_nw_proactive_session_slot_in_progress,
                     gstk_shared_data.fetch_is_in_progress);
      gstk_instances_ptr[(uint8)slot - 1]->is_fetch_ignored = TRUE;
      return FALSE;
  }

  /* Wait until CM task is ready because we initialize ESTK (a GSTK client)
     only after CM is ready. QMI is the other GSTK client we wait for to come
     up in order to start allowing fetch. If we don't wait for these tasks/clients
     to come up, and card sends a proactive command for these clients, we
     might run into issues. */
  if (!((gstk_shared_data.ready_tasks & (00000001 << (uint8) GSTK_CM_TASK)) &&
        (gstk_shared_data.ready_tasks & (00000001 << (uint8) GSTK_QMI_MODEM_TASK))))
  {
    UIM_MSG_HIGH_0("Fetch not allowed because all the GSTK clients are not up yet!");
    gstk_instances_ptr[(uint8)slot - 1]->is_fetch_ignored = TRUE;
    return FALSE;
  }

  /* Entering gstk critical section */
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  if(!gstk_is_env_cmd_to_be_skipped())
  {
    if(gstk_instances_ptr[(uint8)slot - 1]->gstk_bip_env_in_progress)
    {
      UIM_MSG_HIGH_1("Not allowing Fetch as BIP env in progress 0x%x!",
                     gstk_instances_ptr[(uint8)slot - 1]->gstk_bip_env_in_progress);
    }
    /* Check proactive command is on same slot */
    else if(gstk_instances_ptr[(uint8)slot - 1]->gstk_proactive_cmd_in_progress)
    {
      UIM_MSG_HIGH_1("Not allowing Fetch as proactive command is already in progress 0x%x",
                     slot);
    }
    else
    {
      UIM_MSG_HIGH_1("MMGSDI FETCH allow Status is 0x%x",
               gstk_shared_data.fetch_allowed[(uint8)slot - 1]);
      ret_flag = gstk_shared_data.fetch_allowed[(uint8)slot - 1];
      GSTK_SET_PROACTIVE_CMD_IN_PROGRESS_FLAG(
        gstk_instances_ptr[(uint8)slot - 1]->gstk_proactive_cmd_in_progress, ret_flag);
    }
  }
  else
  {
    UIM_MSG_HIGH_0("Not allowing Fetch as First command in gstk queue is BIP ENV");
  }
  if(ret_flag)
  {
    gstk_shared_data.fetch_is_in_progress = slot;
  }
  gstk_instances_ptr[(uint8)slot - 1] ->is_fetch_ignored = !ret_flag;
  /* Leaving gstk critical section */
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  return ret_flag;
} /* gstk_uim_fetch_allow */

/* ============================================================================
  FUNCTION:      GSTK_CLIENT_ID_FORCE_CLEANUP

  DESCRIPTION:
    This function performs cleanup for client id when RPC has indicated that
    the application processor client(s) has/have been terminated.

  PARAMETERS:
    response_cb: Input  Callback that maps to the client id which enables
                        GSTK to perform corresponding client id
                        deregistration on the client's behalf.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    Will result in the client id(s) that were linked to the corresponding
    response_cb during the gstk_slot_client_init_ext() to be deregistered.
========================================================================== */
void gstk_client_id_force_cleanup(
  gstk_client_init_cb_func_type response_cb_ptr
)
{
  uint8 gstk_insts = 0;
  if (!response_cb_ptr)
  {
    UIM_MSG_ERR_0("response_cb_ptr:NULL!");
    return;
  }

  for(gstk_insts = 0; gstk_insts < gstk_shared_data.gstk_num_of_instances; gstk_insts++)
  {
    if (gstk_slot_client_init_ext(gstk_insts + 1, GSTK_NO_TYPE, response_cb_ptr, 0)
          != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("Failed to dereg client id of cb 0x%x", response_cb_ptr);
    }
  }
} /* gstk_client_id_force_cleanup */

/*============================================================================
  FUNCTION:      GSTK_UTIL_PARSE_CCP

  DESCRIPTION:
    This function parses Capability Configuration Parameter TLV into
    cm_bearer_capability_T that can be used by Call Manager

  PARAMETERS:
    ccp_ptr:        [Input]  The pointer to CCP data
    bearer_cap_ptr: [Output] The pointer to cm_bearer_capability_T
    len:            [Input]  The size of cm_bearer_capability_T

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    gstk_status_enum_type:
      GSTK_SUCCESS             - if processing of client_init is successful
      GSTK_BAD_PARAM           - if client callback is NULL
      GSTK_ERROR               - fail to parse CCP data

  SIDE EFFECTS:
    None

  SEE ALSO:
    Definition of structure cm_bearer_capability_T in mn_cm_exp.h file
========================================================================== */
gstk_status_enum_type gstk_util_parse_ccp(
  gstk_cap_config_type      *ccp_ptr,
  uint8                     *bearer_cap_ptr,
  uint32                    len
)
{
#ifdef FEATURE_NAS
  uint8                   index      = 0;
  uint8                   ccp_len    = 0;
  uint8                   *data_ptr  = NULL;
  cm_bearer_capability_T  *gw_bc_ptr = (cm_bearer_capability_T *)bearer_cap_ptr;
  int                     cnt        = 0;
   #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
   #ifdef FEATURE_GSM_AMR_WB
  int                     max       = 6;
  gsm_speech_ver_e_type   *spch_ver[6];
  #else
  int                     max       = 5;
  gsm_speech_ver_e_type   *spch_ver[5];
  #endif /* FEATURE_GSM_AMR_WB */
  #else
  #ifdef FEATURE_GSM_AMR_WB
  int                     max       = 4;
  gsm_speech_ver_e_type   *spch_ver[4];
  #else
  int                     max       = 3;
  gsm_speech_ver_e_type   *spch_ver[3];
  #endif /* FEATURE_GSM_AMR_WB */
  #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
  gstk_slot_id_enum_type  slot      = gstk_shared_data.ui_nw_proactive_session_slot_in_progress;

  if (ccp_ptr == NULL || bearer_cap_ptr == NULL)
  {
    UIM_MSG_ERR_2("NULL ptr: 0x%x, 0x%x.", ccp_ptr, bearer_cap_ptr);
    return GSTK_BAD_PARAM;
  }

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("Invalid slot id: 0x%x", slot);
    return GSTK_BAD_PARAM;
  }

  /* dump CCP data */
  gstk_util_dump_byte_array("CCP", ccp_ptr->ccp, ccp_ptr->length);

  data_ptr = ccp_ptr->ccp;

  /* first byte (OCTET 2 in 04.08) is the length of CCP */
  ccp_len = ccp_ptr->ccp[0];
  if (ccp_ptr->length != (ccp_len + 1))
  {
    UIM_MSG_ERR_2("TLV Len (0x%x) doesn't match with CCP Len (0x%x)!",
                  len, ccp_len);
    return GSTK_ERROR;
  }
  /* byte 11 to byte 14 should be all 0xFF, ignore them */
  ccp_len = (ccp_len > 10) ? 10 : ccp_len;
  data_ptr++;
  index++;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)slot - 1], GSTK_BAD_PARAM);

  if (gstk_instances_ptr[(uint8)slot - 1]->apps_enabled & GSTK_APP_SIM_BMSK ||
      gstk_instances_ptr[(uint8)slot - 1]->apps_enabled & GSTK_APP_USIM_BMSK)
  {
    gw_bc_ptr->present = TRUE;

    /* The ccp data starts at 3rd octect of bearer capability defined in 04.08 */
    /*  OCTET 3  */
    gw_bc_ptr->radio_channel_requirement = *data_ptr;
    gw_bc_ptr->radio_channel_requirement &= 0x60;
    gw_bc_ptr->radio_channel_requirement >>= 5;

    gw_bc_ptr->coding_standard = *data_ptr;
    gw_bc_ptr->coding_standard &=0x10;
    gw_bc_ptr->coding_standard >>= 4;

    gw_bc_ptr->transfer_mode = *data_ptr;
    gw_bc_ptr->transfer_mode &= 0x08;
    gw_bc_ptr->transfer_mode >>=3;

    gw_bc_ptr->information_transfer_capability = (byte)(*data_ptr & 0x07);

    data_ptr++;
    index++;

    /* OCTET 3a, 3b, ... */
#ifdef FEATURE_GSM_AMR_WB
    spch_ver[0] = &gw_bc_ptr->gsm_speech_ver_pref_0;
    spch_ver[1] = &gw_bc_ptr->gsm_speech_ver_pref_1;
    spch_ver[2] = &gw_bc_ptr->gsm_speech_ver_pref_2;
    spch_ver[3] = &gw_bc_ptr->gsm_speech_ver_pref_3;

    #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
    spch_ver[4] = &gw_bc_ptr->gsm_speech_ver_pref_4;
    spch_ver[5] = &gw_bc_ptr->gsm_speech_ver_pref_5;
    #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
#else
    spch_ver[0] = &gw_bc_ptr->gsm_speech_ver_pref_1;
    spch_ver[1] = &gw_bc_ptr->gsm_speech_ver_pref_2;
    spch_ver[2] = &gw_bc_ptr->gsm_speech_ver_pref_3;

    #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
    spch_ver[3] = &gw_bc_ptr->gsm_speech_ver_pref_4;
    spch_ver[4] = &gw_bc_ptr->gsm_speech_ver_pref_5;
    #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
#endif

    for (cnt=0; (*(data_ptr-1)&0x80)==0x00 && cnt<max; cnt++)
    {
      *spch_ver[cnt] = (gsm_speech_ver_e_type)(*data_ptr & 0x0f);
      data_ptr++;
      index++;
    }

    /* consume any left speech version indications */
    /*  --> won't do this because it might crash */

    if (index <= ccp_len)
    {
      /*  THERE IS MORE DATA - OCTET 4  */
      gw_bc_ptr->compression = (byte) ((*data_ptr & 0x40) >> 6);
      gw_bc_ptr->structure = (byte)((*data_ptr & 0x30) >> 4);
      gw_bc_ptr->duplex_mode = (byte)((*data_ptr & 0x08) >> 3);
      gw_bc_ptr->configuration = (byte)((*data_ptr & 0x04) >>2);
      gw_bc_ptr->NIRR = (byte)((*data_ptr & 0x02) >> 1);
      gw_bc_ptr->establishment = (byte)(*data_ptr & 0x01);

      data_ptr++;
      index++;

      if (index <= ccp_len)
      {
        /*  STILL MORE DATA - OCTET 5  */
        gw_bc_ptr->access_id = (byte) ((*data_ptr & 0x60) >> 5);

        if (gw_bc_ptr->access_id != ACCESS_OCTET_ID)
        {
          UIM_MSG_ERR_1("PROTOCOL ERROR: Unexpected value for access_id = %d\n",
                        gw_bc_ptr->access_id);
          return GSTK_ERROR;
        }

        gw_bc_ptr->rate_adaption = (byte)((*data_ptr & 0x18) >> 3);
        gw_bc_ptr->signalling_access_protocol = (byte)(*data_ptr & 0x07);

        if ((*data_ptr & 0x80) == 0) /* 5a */
        {
          data_ptr++;
          index++;

          if (index > ccp_len)
          {
            UIM_MSG_ERR_2("CCP out of boundary (0x%x > 0x%x)!",
                          index, ccp_len);
            return GSTK_ERROR;
          }

          gw_bc_ptr->other_ITC = (byte)((*data_ptr & 0x60) >> 5);
          gw_bc_ptr->other_rate_adaption = (byte)((*data_ptr & 0x18) >> 3);

          if (((*data_ptr & 0x80) == 0))  /* 5b */
          {
            data_ptr++;
            index++;

            if (index > ccp_len)
            {
              UIM_MSG_ERR_2("CCP out of boundary (0x%x > 0x%x)!",
                            index, ccp_len);
              return GSTK_ERROR;
            }

            gw_bc_ptr->rate_adaption_header = (byte)((*data_ptr & 0x40) >> 6);
            gw_bc_ptr->multiple_frame_establishment = (byte)((*data_ptr & 0x20) >> 5);
            gw_bc_ptr->mode_of_operation = (byte)((*data_ptr & 0x10) >> 4);
            gw_bc_ptr->logical_link_identifier_negotiation = (byte)((*data_ptr & 0x08) >> 3);
            gw_bc_ptr->assignor_assignee = (byte)((*data_ptr & 0x04) >> 2);
            gw_bc_ptr->inband_outband_negotiation = (byte)((*data_ptr & 0x02) >> 1); /* Bit 2*/
          }
        }

        data_ptr++;
        index++;

        if (index <= ccp_len)
        {
          /*  STILL MORE DATA - OCTET 6  */

          gw_bc_ptr->layer_1_id = (byte) ((*data_ptr & 0x60) >> 5);

          if (gw_bc_ptr->layer_1_id != LAYER_1_OCTET_ID)
          {
            UIM_MSG_ERR_1("PROTOCOL ERROR: Unexpected value for layer_1_id = %d\n",
                          gw_bc_ptr->layer_1_id);
            return GSTK_ERROR;
          }

          gw_bc_ptr->user_information_layer_1_protocol =
            (byte)((*data_ptr & 0x1e) >> 1);
          gw_bc_ptr->sync_async = (byte)(*data_ptr & 0x01);

          if ((*data_ptr & 0x80) == 0)
          {

            /* More data - Octet 6a */
            data_ptr++;
            index++;

            if (index > ccp_len)
            {
              UIM_MSG_ERR_2("CCP out of boundary (0x%x > 0x%x)!",
                            index, ccp_len);
              return GSTK_ERROR;
            }

            gw_bc_ptr->number_of_data_bits =
              (byte)((*data_ptr & 0x10) >> 4);
            gw_bc_ptr->number_of_stop_bits =
              (byte)((*data_ptr & 0x40) >> 6);
            gw_bc_ptr->user_rate =
              (byte)(*data_ptr & 0x0f);
            gw_bc_ptr->negotiation =
              (byte)((*data_ptr & 0x20) >> 5);

            if ((*data_ptr & 0x80) == 0)
            {
              /* More data - Octet 6b */
              data_ptr++;
              index++;

              if (index > ccp_len)
              {
                UIM_MSG_ERR_2("CCP out of boundary (0x%x > 0x%x)!",
                              index, ccp_len);
                return GSTK_ERROR;
              }

              gw_bc_ptr->parity =
                (byte)(*data_ptr & 0x07);
              gw_bc_ptr->intermediate_rate =
                (byte)((*data_ptr & 0x60) >> 5);
              gw_bc_ptr->NIC_on_Tx =
                (byte)((*data_ptr & 0x10) >> 4);
              gw_bc_ptr->NIC_on_Rx =
                (byte)((*data_ptr & 0x08) >> 3);

              if ((*data_ptr & 0x80) == 0)
              {
                /* more data - Octet 6c */
                data_ptr++;
                index++;

                if (index > ccp_len)
                {
                  UIM_MSG_ERR_2("CCP out of boundary (0x%x > 0x%x)!",
                                index, ccp_len);
                  return GSTK_ERROR;
                }

                gw_bc_ptr->modem_type =
                  (byte)(*data_ptr & 0x1f);
                gw_bc_ptr->connection_element =
                  (byte)((*data_ptr & 0x60) >> 5);

                if ( (gw_bc_ptr->connection_element == TRANSPARENT_DATA)
                    && (gw_bc_ptr->modem_type == AUTO_TYPE_1) )
                {
                  UIM_MSG_ERR_0("PROTOCOL ERROR: modem_type indicates autobauding_type_1 when connection_element indicates transparent\n");
                  return GSTK_ERROR;
                }

                if ( (gw_bc_ptr->information_transfer_capability == ITC_FAX_G3)
                    && (gw_bc_ptr->modem_type != NO_MODEM) )
                {
                  UIM_MSG_ERR_0("PROTOCOL ERROR: When info_transfer_capability indicates fax_g3, modem_type should be no_modem\n");
                  return GSTK_ERROR;
                }

                /* These added for Phase2+ elements */
                if ((*data_ptr & 0x80) == 0)
                {
                  /* more data - Octet 6d */
                  data_ptr++;
                  index++;

                  if (index > ccp_len)
                  {
                    UIM_MSG_ERR_2("CCP out of boundary (0x%x > 0x%x)!",
                                  index, ccp_len);
                    return GSTK_ERROR;
                  }

                  gw_bc_ptr->fixed_network_user_rate =
                    (byte)(*data_ptr & 0x1f);
                  gw_bc_ptr->other_modem_type =
                    (byte)((*data_ptr & 0x60) >> 5);

                  if (gw_bc_ptr->other_modem_type == RESERVED_OMT)
                  {
                    UIM_MSG_ERR_0("PROTOCOL ERROR: Received a reserved value of other_modem_type\n");
                    return GSTK_ERROR;
                  }

                  if (gw_bc_ptr->fixed_network_user_rate > FNUR_57600)
                  {
                    UIM_MSG_ERR_0("PROTOCOL ERROR: Received a reserved value of fixed_network_user_rate\n");
                    return GSTK_ERROR;
                  }

                  if ((*data_ptr & 0x80) == 0)
                  {
                    /* more data - Octet 6e */
                    data_ptr++;
                    index++;

                    if (index > ccp_len)
                    {
                      UIM_MSG_ERR_2("CCP out of boundary (0x%x > 0x%x)!",
                                    index, ccp_len);
                      return GSTK_ERROR;
                    }

                    gw_bc_ptr->maximum_number_of_trafic_channels =
                      (byte)(*data_ptr & 0x07);
                    gw_bc_ptr->acceptable_channel_codings =
                      (byte)((*data_ptr & 0x78) >> 3);

                    /* Note:
                       In the network to MS direction,
                       max_number_of_trafic_channels &
                       acceptable_channel_codings are spare
                    */

                    if ((*data_ptr & 0x80) == 0)
                    {
                      /* more data - Octet 6f */
                      data_ptr++;
                      index++;

                      if (index > ccp_len)
                      {
                        UIM_MSG_ERR_2("CCP out of boundary (0x%x > 0x%x)!",
                                      index, ccp_len);
                        return GSTK_ERROR;
                      }

                      gw_bc_ptr->wanted_air_interface_user_rate =
                        (byte)(*data_ptr & 0x0f);
                      gw_bc_ptr->user_initiated_modification_indication =
                        (byte)((*data_ptr & 0x70) >> 4);

                      if ((*data_ptr & 0x80) == 0)
                      {
                        /* more data - Octet 6f */
                        data_ptr++;
                        index++;

                        if (index > ccp_len)
                        {
                          UIM_MSG_ERR_2("CCP out of boundary (0x%x > 0x%x)!",
                                        index, ccp_len);
                          return GSTK_ERROR;
                        }

                        gw_bc_ptr->acceptable_channel_coding_extended  =
                          (byte)((*data_ptr & 0xf0) >> 4);
                        /* Bits 4-3 */
                        gw_bc_ptr->asymmetry_indication =
                          (byte)((*data_ptr & 0x0C) >> 2);
                      }
                    }
                  }
                }
              }
            }
          }

          data_ptr++;
          index++;

          if (index <= ccp_len)
          {
            /*  STILL MORE DATA - OCTET 7  */
            gw_bc_ptr->user_information_layer_2_protocol =
              (byte)(*data_ptr & 0x1f);
            gw_bc_ptr->layer_2_id =
              (byte) ((*data_ptr & 0x60) >> 5);

            if (gw_bc_ptr->layer_2_id != LAYER_2_OCTET_ID)
            {
              UIM_MSG_ERR_1("PROTOCOL ERROR: Unexpected value for layer_2_id = %d\n",
                            gw_bc_ptr->layer_2_id);
              return GSTK_ERROR;
            }

          }  /* end of if (index... for octet 7 */
        }  /* end of if (index... for octet 6  */
      }  /* end of if (index... for octet 5  */
    }  /* end of if (index... for octet 4  */
  }
  else
  {
    UIM_MSG_ERR_1("Can't parse CCP with current apps enabled 0x%x",
                  gstk_instances_ptr[(uint8)slot - 1]->apps_enabled);
    return GSTK_ERROR;
  }

  return GSTK_SUCCESS;
#else
  (void)ccp_ptr;
  (void)bearer_cap_ptr;
  (void)len;

  UIM_MSG_ERR_0("CCP is not support!");
  return GSTK_ERROR;
#endif /* FEATURE_NAS */
} /* gstk_util_parse_ccp */

/*============================================================================
  FUNCTION:      GSTK_IO_CTRL

  DESCRIPTION:
    General interface to set/get GSTK data. The I/O operations currently
    supported are:
    1) GSTK_IO_GET_CLIENT_TABLE_NEW_CLIENT_CB: return event callback function
       registered by a client. param_in_ptr points to the client ID and
       param_out_ptr points to pointer of the callback function of
       gstk_toolkit_evt_cb_func_type type.
    2) GSTK_IO_GET_CLIENT_TABLE_SLOT_ID: get the slot ID that the client is
       associated with. param_in_ptr points to the client ID and param_out_ptr
       points to slot ID returned.
    3) GSTK_IO_GET_CLIENT_TABLE_USER_DATA: get user data of a client.
       param_in_ptr points to client ID and param_out_ptr points to the user
       data of uint32 type
    4) GSTK_IO_GET_CMD_RAW_APDU: get raw APUD of a proactive command referred
       by command details reference ID. param_in_ptr points to the command
       details reference ID and param_out_ptr points to gstk_generic_data_type
       variable where the raw APDU to be stored. The caller function is
       responsible of free memory pointed by data_buffer_ptr in
       gstk_generic_data_type.
    5) GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT: call the function
       gstk_util_find_client_with_registered_event() to find the client that
       registered with a specific event. param_in_ptr points to a variable of
       gstk_io_find_client_with_registered_event_in_param_type and param_out_ptr
       points to the client ID returned.
    6) GSTK_IO_SET_CURR_SESSION_UI_RELATED: set the value of
       gstk_is_curr_session_ui_related of a GSTK instance (slot). param_in_ptr
       points to a variable of gstk_io_set_curr_session_ui_related_in_param_type
       and param_out_ptr points to some dummy data (can't be NULL even though
       nothing is returned)
    7) GSTK_IO_CALL_CLIENT_EVT_CB: invoke gstk_util_call_client_evt_cb().
       param_in_ptr points to a variable of gstk_io_call_client_evt_cb_param_type
       and param_out_ptr points to dummy data (can't be NULL)
    8) GSTK_IO_MODEM_AS_ID_TO_SLOT_ID: convert modem active subscription ID to
       GSTK slot ID. The modem active subscription ID is defined as eumeration
       date type sys_modem_as_id_e_type in sys_modem.h file. param_in_ptr points
       to modem active subscription ID, param_out_ptr points to GSTK slot ID
       returned.
    9) GSTK_IO_SLOT_ID_TO_MODEM_AS_ID: convert GSTK slot ID to modem active
       subscription ID. The modem active subscription ID is defined as eumeration
       date type sys_modem_as_id_e_type in sys_modem.h file. param_in_ptr points
       to GSTK slot ID, param_out_ptr points to modem active subscription ID
       returned.
    10)GSTK_IO_DS_SUBSCRIPTION_ID_TO_SLOT_ID: Convert DS subscription ID to
       slot ID.
    11)GSTK_IO_GET_CURR_ACCESS_TECH: read evt_cb_type of
       gstk_client_table. param_in_ptr points to client ID and param_out_ptr
       points to a variable of gstk_access_technology_type.
    12)GSTK_IO_SEND_MM_IDLE_EVENT: Send MM IDLE event from NAS to GSTK
    13)GSTK_IO_SEND_EMM_REGISTERED_EVENT: Send MM IDLE from LTE-NAS to GSTK
    14)GSTK_IO_SEND_MM_INDICATION: Send MM indication from NAS to GSTK
    15)GSTK_IO_SEND_NW_REJ_EVENT: Send NW Reject event from NAS to GSTK
    16)GSTK_IO_GET_CURR_LOC_STATUS: Get the location status for the slot
       from the GSTK cache
    17)GSTK_IO_GET_TERMINAL_PROFILE_DATA: retrieve the cached TP info
    18)GSTK_IO_MANAGE_BIP_OTASP_CONCURRENCY: Manage BIP/OTASP concurrency
    19)GSTK_IO_GET_DS_SYSTEM_RAT_INFO: Get preferred network and the RAT details
       obtained from DS for the slot
    20)GSTK_IO_GET_WLAN_AVAILABILITY: Get availability of WLAN network
    21)GSTK_IO_SIM_INITIATED_MODIFIED_CC_RESP: Copy the call control response
       sent to the client in case of SIM initiated call control response
       modified to another call type
    22)GSTK_IO_GET_ACTIVE_DATA_SUB_SLOT_ID: get the slot ID corresponding to the
       active DATA subscription

  PARAMETERS:
    io_cmd:       [Input]   The type of I/O command
    param_in_ptr: [Input]   The pointer to the input paramter
    param_out_ptr:[Output]  The pointer to the output paramter

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    gstk_status_enum_type:
      GSTK_SUCCESS             - if processing of client_init is successful
      GSTK_BAD_PARAM           - if parameter is NULL

  SIDE EFFECTS:
    None

  SEE ALSO:
    Definition of structure cm_bearer_capability_T in mn_cm_exp.h file
========================================================================== */
gstk_status_enum_type gstk_io_ctrl(
  gstk_io_ctrl_enum_type    io_cmd,
  const void               *param_in_ptr,
  void                     *param_out_ptr
)
{
  gstk_client_id_type                                     *client_id_ptr       = NULL;
  uint32                                                  len                  = 0;
  gstk_io_find_client_with_registered_event_in_param_type *find_cli_reg_ptr    = NULL;
  gstk_io_set_curr_session_ui_related_in_param_type       *set_ui_sess_ptr     = NULL;
  gstk_slot_id_enum_type                                  slot                 = GSTK_SLOT_ID_MAX;
  gstk_io_call_client_evt_cb_param_type                   *call_cli_evt_cb_ptr = NULL;
  gstk_generic_data_type                                  *raw_cmd_ptr         = NULL;
  uint32                                                  cmd_details_ref_id   = GSTK_CMD_DETAILS_REF_TABLE_FULL;
  gstk_io_send_mm_idle_evt_type                           *mm_idle_evt_ptr     = NULL;
  gstk_io_send_mm_ind_param_type                          *mm_ind_ptr          = NULL;
  gstk_task_cmd_type                                      *gstk_cmd_p          = NULL;
  gstk_io_send_nw_rej_evt_type                            *nw_rej_evt_ptr      = NULL;
  gstk_cc_rsp_type                                        *cc_rsp              = NULL;
  uint32                                                  i                    = 0;
  uint32                                                  j                    = 0;
  uint32                                                  k                    = 0;
  sys_modem_as_id_e_type                                  as_id                = SYS_MODEM_AS_ID_NONE;
  ds_sys_subscription_enum_type                           sub_id               = DS_SYS_DEFAULT_SUBS;
  wms_msg_event_info_s_type                              *info_ptr             = NULL;

  if (param_in_ptr == NULL || param_out_ptr == NULL)
  {
    UIM_MSG_ERR_2("NULL PTR: 0x%x, 0x%x", param_in_ptr, param_out_ptr);
    return GSTK_BAD_PARAM;
  }

  /* Avoid logging for io_cmds that are not helpful */ 
  if((io_cmd != GSTK_IO_MODEM_AS_ID_TO_SLOT_ID) &&
     (io_cmd != GSTK_IO_SLOT_ID_TO_MODEM_AS_ID) &&
     (io_cmd != GSTK_IO_DS_SUBSCRIPTION_ID_TO_SLOT_ID) &&
     (io_cmd != GSTK_IO_GET_CURR_ACCESS_TECH) &&
     (io_cmd != GSTK_IO_GET_CURR_LOC_STATUS) &&
     (io_cmd != GSTK_IO_GET_DS_SYSTEM_RAT_INFO) &&
     (io_cmd != GSTK_IO_GET_WLAN_AVAILABILITY) &&
     (io_cmd != GSTK_IO_GET_ACTIVE_DATA_SUB_SLOT_ID))
  {
    UIM_MSG_HIGH_1("in gstk_io_ctrl() with cmd 0x%x", io_cmd);
  }

  switch(io_cmd)
  {
  case GSTK_IO_GET_CLIENT_TABLE_NEW_CLIENT_CB:
    client_id_ptr = (gstk_client_id_type *)param_in_ptr;
    len = sizeof(gstk_toolkit_evt_cb_func_type);
    (void)gstk_memcpy(param_out_ptr,
                      &gstk_shared_data.gstk_client_table[*client_id_ptr-1].client_cb,
                      len,
                      len,
                      len);
    break;
  case GSTK_IO_GET_CLIENT_TABLE_USER_DATA:
    client_id_ptr = (gstk_client_id_type *)param_in_ptr;
    len = sizeof(uint32);
    (void)gstk_memcpy(param_out_ptr,
                      &gstk_shared_data.gstk_client_table[*client_id_ptr-1].user_data,
                      len,
                      len,
                      len);
    break;
  case GSTK_IO_GET_CMD_RAW_APDU:
    cmd_details_ref_id = *((uint32 *)param_in_ptr);
    raw_cmd_ptr = (gstk_generic_data_type *)param_out_ptr;
    len = command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].raw_cmd.data_len;
    raw_cmd_ptr->data_len = (int32)len;
    raw_cmd_ptr->data_buffer_ptr = GSTK_CALLOC(len);
    (void)gstk_memcpy(raw_cmd_ptr->data_buffer_ptr,
                      command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_details_ref_id)].raw_cmd.data_buffer_ptr,
                      len,
                      len,
                      len);
    break;
  case GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT:
    find_cli_reg_ptr =
      (gstk_io_find_client_with_registered_event_in_param_type *)param_in_ptr;
    return gstk_util_find_client_with_registered_event(
             (gstk_client_id_type *)param_out_ptr,
             find_cli_reg_ptr->cmd_type,
             find_cli_reg_ptr->format_type,
             find_cli_reg_ptr->cli_func_type);
  case GSTK_IO_SET_CURR_SESSION_UI_RELATED:
    set_ui_sess_ptr =
      (gstk_io_set_curr_session_ui_related_in_param_type *)param_in_ptr;
    if (!GSTK_IS_VALID_SLOT_ID(set_ui_sess_ptr->slot))
    {
      UIM_MSG_ERR_1("invalid slot id 0x%x!", set_ui_sess_ptr->slot);
      return GSTK_ERROR;
    }

    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)set_ui_sess_ptr->slot - 1], GSTK_ERROR);

    gstk_instances_ptr[(uint32)set_ui_sess_ptr->slot - 1]->gstk_is_curr_session_ui_related =
      set_ui_sess_ptr->is_curr_session_ui_related;
    break;
  case GSTK_IO_GET_CLIENT_TABLE_SLOT_ID:
    client_id_ptr = (gstk_client_id_type *)param_in_ptr;
    slot = gstk_util_get_sim_slot_id(*client_id_ptr);
    len = sizeof(gstk_slot_id_enum_type);
    (void)gstk_memcpy(param_out_ptr,
                      &slot,
                      len,
                      len,
                      len);
    break;
  case GSTK_IO_CALL_CLIENT_EVT_CB:
    call_cli_evt_cb_ptr = (gstk_io_call_client_evt_cb_param_type *)param_in_ptr;
    gstk_util_call_client_evt_cb(
      call_cli_evt_cb_ptr->client_id,
      call_cli_evt_cb_ptr->gstk_req_ptr);
    break;
  case GSTK_IO_MODEM_AS_ID_TO_SLOT_ID:
    if(gstk_shared_data.gstk_num_of_instances == 1)
    {
      slot = GSTK_SLOT_1;
      (void)gstk_memcpy(param_out_ptr,
                        &slot,
                        sizeof(slot),
                        sizeof(slot),
                        sizeof(slot));
      break;
    }
    as_id = *((sys_modem_as_id_e_type *)param_in_ptr);
    (void)gstk_util_validate_slot_sessions();

    for (i = 0; i < GSTK_MODEM_AS_INFO_SIZE; ++i)
    {
      if (gstk_shared_data.modem_as_info[i].as_id == as_id)
      {
        for (j = 0; j < GSTK_MMGSDI_SESS_INFO_SIZE; ++j)
        {
          /* Check if this subscription has matching session type */
          for (k = 0; k < GSTK_AS_MAX_SESSION; k++)
          {
            if (gstk_shared_data.mmgsdi_sess_info[j].sess_type ==
              gstk_shared_data.modem_as_info[i].sess_types[k])
            {
              slot = gstk_shared_data.mmgsdi_sess_info[j].slot;
              if (GSTK_IS_VALID_SLOT_ID(slot))
              {
                UIM_MSG_HIGH_2("found slot 0x%x for as_id 0x%x!", slot, as_id);
                len = sizeof(slot);
                (void)gstk_memcpy(param_out_ptr,
                                  &slot,
                                  len,
                                  len,
                                  len);
                return GSTK_SUCCESS;
              }

              UIM_MSG_ERR_1("slot info (0x%x) unavailable! check next type", slot);
              break;
            } /*if( mmgsdi_sess_info[j].sess_type == modem_as_info[i].sess_types ) */
          } /* for (k = 0; k < GSTK_AS_MAX_SESSION; k++) */
        } /* for (j = 0; j < GSTK_MMGSDI_SESS_INFO_SIZE; ++j) */
        UIM_MSG_ERR_0("invalid AS sess types!");
        for (k = 0; k < GSTK_AS_MAX_SESSION; k++)
        {
          UIM_MSG_ERR_2("invalid sess_types[0x%x]: 0x%x!",
                        k,
                        gstk_shared_data.modem_as_info[i].sess_types[k]);
        }
        return GSTK_ERROR;
      } /* if (gstk_shared_data.modem_as_info[i].as_id == as_id) */
    } /* for (i = 0; i < GSTK_MODEM_AS_INFO_SIZE; ++i) */
    UIM_MSG_ERR_2("invalid as_id 0x%x!, num_of_slots 0x%x",
                  as_id, gstk_shared_data.gstk_num_of_instances);
    return GSTK_ERROR;
  case GSTK_IO_SLOT_ID_TO_MODEM_AS_ID:
    if(gstk_shared_data.gstk_num_of_instances == 1)
    {
      as_id = SYS_MODEM_AS_ID_1;
      (void)gstk_memcpy(param_out_ptr,
                        &as_id,
                        sizeof(sys_modem_as_id_e_type),
                        sizeof(sys_modem_as_id_e_type),
                        sizeof(sys_modem_as_id_e_type));
      break;
    }
    slot = *((gstk_slot_id_enum_type *)param_in_ptr);
    if (!GSTK_IS_VALID_SLOT_ID(slot))
    {
      UIM_MSG_ERR_1("invalid slot 0x%x!", slot);
      return GSTK_ERROR;
    }

    (void)gstk_util_validate_slot_sessions();

    for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i)
    {
      if (gstk_shared_data.mmgsdi_sess_info[i].slot == slot)
      {
        for (j = 0; j < GSTK_MODEM_AS_INFO_SIZE; ++j)
        {
          /* Check if this subscription has matching session type */
          for (k = 0; k < GSTK_AS_MAX_SESSION; k++)
          {
            if (gstk_shared_data.modem_as_info[j].sess_types[k] ==
                gstk_shared_data.mmgsdi_sess_info[i].sess_type)
            {
              as_id = gstk_shared_data.modem_as_info[j].as_id;
              if (as_id > SYS_MODEM_AS_ID_NONE &&
                  as_id < SYS_MODEM_AS_ID_NO_CHANGE)
              {
                UIM_MSG_HIGH_2("found as_id 0x%x for slot 0x%x!", as_id, slot);
                len = sizeof(sys_modem_as_id_e_type);
                (void)gstk_memcpy(param_out_ptr,
                                  &as_id,
                                  len,
                                  len,
                                  len);
                return GSTK_SUCCESS;
              }

              UIM_MSG_ERR_1("as_id info unavailable! 0x%x, check next type", as_id);
              break;
            } /* if(modem_as_info[j].sess_types == mmgsdi_sess_info[i].sess_type) */
          } /* for (k = 0; k < GSTK_AS_MAX_SESSION; k++) */
        } /* for (j = 0; j < GSTK_MODEM_AS_INFO_SIZE; ++j) */
        UIM_MSG_ERR_1("invalid MMGSDI sess type 0x%x",
                      gstk_shared_data.mmgsdi_sess_info[i].sess_type);
        return GSTK_ERROR;
      } /* if (gstk_shared_data.mmgsdi_sess_info[i].slot == slot) */
    } /* for (i = 0; i < GSTK_MMGSDI_SESS_INFO_SIZE; ++i) */
    UIM_MSG_ERR_2("as_id unavailable for slot 0x%x!, num_of_slots 0x%x",
                  slot, gstk_shared_data.gstk_num_of_instances);
    return GSTK_ERROR;
  case GSTK_IO_DS_SUBSCRIPTION_ID_TO_SLOT_ID:
    if(gstk_shared_data.gstk_num_of_instances == 1)
    {
      slot = GSTK_SLOT_1;
      (void)gstk_memcpy(param_out_ptr,
                        &slot,
                        sizeof(slot),
                        sizeof(slot),
                        sizeof(slot));
      break;
    }
    /*
      ds sub id maps to as_id 1 to 1, but it does not map to
      slot id 1 to 1. Therefore, first map ds sub id to as_id,
      then use existing gstk_io_ctrl to map as_id to slot id
    */

    sub_id = *((ds_sys_subscription_enum_type *)param_in_ptr);

    switch (sub_id)
    {
    case DS_SYS_PRIMARY_SUBS:
      as_id = SYS_MODEM_AS_ID_1;
      break;
    case DS_SYS_SECONDARY_SUBS:
      as_id = SYS_MODEM_AS_ID_2;
      break;
    case DS_SYS_TERTIARY_SUBS:
      as_id = SYS_MODEM_AS_ID_3;
      break;
    default:
      UIM_MSG_ERR_1("Error: DS sub id is 0x%x, can't use to get as_id",
                    sub_id);
      return GSTK_ERROR;
    }

    if (gstk_io_ctrl(GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
                     &as_id,
                     &slot) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("Could not translate as_id 0x%x to slot_id",
                    as_id);
      return GSTK_ERROR;
    }

    if (GSTK_IS_VALID_SLOT_ID(slot))
    {
      UIM_MSG_HIGH_2("found slot 0x%x for as_id 0x%x!", slot, as_id);
      len = sizeof(slot);
      (void)gstk_memcpy(param_out_ptr,
                        &slot,
                        len,
                        len,
                        len);
      return GSTK_SUCCESS;
    }
    else
    {
      UIM_MSG_ERR_2("Invalid slot id 0x%x!, num_of_slots 0x%x",
                    slot, gstk_shared_data.gstk_num_of_instances);
      return GSTK_ERROR;
    }
    break;
  case GSTK_IO_GET_CURR_ACCESS_TECH:
    len = sizeof(gstk_access_technology_type);
    slot = (*(gstk_slot_id_enum_type *)(param_in_ptr));
    if (!GSTK_IS_VALID_SLOT_ID(slot))
    {
      UIM_MSG_ERR_1("Invalid slot id 0x%x!", slot);
      return GSTK_ERROR;
    }
    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)slot - 1], GSTK_ERROR);

    UIM_MSG_HIGH_2("Slot is 0x%x , latest available rat is 0x%x",
                   slot,
                   gstk_instances_ptr[(uint8)slot - 1]->gstk_next_curr_rat);
    
    (void)gstk_memcpy(param_out_ptr,
                      &gstk_instances_ptr[(uint8)slot - 1]->gstk_next_curr_rat,
                      len,
                      len,
                      len);
    break;
  case GSTK_IO_SEND_MM_IDLE_EVENT:
  case GSTK_IO_SEND_EMM_REGISTERED_EVENT:
    mm_idle_evt_ptr = (gstk_io_send_mm_idle_evt_type *)param_in_ptr;
    len = sizeof(gstk_io_send_mm_idle_evt_type);
    if (mm_idle_evt_ptr->cell_id.cell_len > GSTK_MAX_CELL_ID_LEN)
    {
      UIM_MSG_ERR_1("Invalid cell len 0x%x!",
                    mm_idle_evt_ptr->cell_id.cell_len);
      return GSTK_MEMORY_ERROR;
    }

    gstk_cmd_p = gstk_task_get_cmd_buf();
    if (gstk_cmd_p != NULL)
    {
      gstk_cmd_p->cmd.mm_idle_state.message_header.command_group =
        GSTK_MM_IDLE_CMD;
      gstk_cmd_p->cmd.mm_idle_state.message_header.command_id =
        GSTK_MM_IDLE_FROM_NAS;
      gstk_cmd_p->cmd.mm_idle_state.stack_id = mm_idle_evt_ptr->stack_id;
      /* as_id is subscription ID for DSDS/TSTS */
      if (gstk_io_ctrl(GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
                       &mm_idle_evt_ptr->as_id,
                       &slot) == GSTK_SUCCESS)
      {
        gstk_cmd_p->cmd.mm_idle_state.message_header.sim_slot_id = slot;
      }
      else
      {
        UIM_MSG_ERR_1("can't get slot for as_id 0x%x!",
                      mm_idle_evt_ptr->as_id);
        gstk_task_free_cmd_buf(gstk_cmd_p);
        return GSTK_ERROR;
      }

      (void)gstk_memcpy(gstk_cmd_p->cmd.mm_idle_state.mcc_mnc,
                        mm_idle_evt_ptr->mcc_mnc,
                        GSTK_MAX_PLMN_LEN,
                        GSTK_MAX_PLMN_LEN,
                        GSTK_MAX_PLMN_LEN);
      if(mm_idle_evt_ptr->rat == (uint8)SYS_RAT_LTE_RADIO_ACCESS)
      {
        gstk_cmd_p->cmd.mm_idle_state.lac[0] = mm_idle_evt_ptr->lac[1];
        gstk_cmd_p->cmd.mm_idle_state.lac[1] = mm_idle_evt_ptr->lac[0];
      }
      else
      {
        (void)gstk_memcpy(gstk_cmd_p->cmd.mm_idle_state.lac,
                          mm_idle_evt_ptr->lac,
                          GSTK_MAX_LAC_LEN,
                          GSTK_MAX_LAC_LEN,
                          GSTK_MAX_LAC_LEN);
      }
      gstk_cmd_p->cmd.mm_idle_state.location_state =
        mm_idle_evt_ptr->location_state;
      gstk_cmd_p->cmd.mm_idle_state.rat =
        mm_idle_evt_ptr->rat;
      if(mm_idle_evt_ptr->rat == (uint8)SYS_RAT_LTE_RADIO_ACCESS)
      {
        gstk_cmd_p->cmd.mm_idle_state.cell_id.cell_id[0] =
          ((mm_idle_evt_ptr->cell_id.cell_id[3] & 0x0F) << 4) |
          ((mm_idle_evt_ptr->cell_id.cell_id[2] & 0xF0) >> 4);
        gstk_cmd_p->cmd.mm_idle_state.cell_id.cell_id[1] =
          ((mm_idle_evt_ptr->cell_id.cell_id[2] & 0x0F) << 4) |
          ((mm_idle_evt_ptr->cell_id.cell_id[1] & 0xF0) >> 4);
        gstk_cmd_p->cmd.mm_idle_state.cell_id.cell_id[2] =
          ((mm_idle_evt_ptr->cell_id.cell_id[1] & 0x0F) << 4) |
          ((mm_idle_evt_ptr->cell_id.cell_id[0] & 0xF0) >> 4);
        gstk_cmd_p->cmd.mm_idle_state.cell_id.cell_id[3] =
          (mm_idle_evt_ptr->cell_id.cell_id[0] << 4) | 0x0F;
      }
      else
      {
        if(gstk_memcpy(gstk_cmd_p->cmd.mm_idle_state.cell_id.cell_id,
                       mm_idle_evt_ptr->cell_id.cell_id,
                       mm_idle_evt_ptr->cell_id.cell_len,
                       GSTK_MAX_CELL_ID_LEN,
                       GSTK_MAX_CELL_ID_LEN) <
           (size_t)mm_idle_evt_ptr->cell_id.cell_len)
        {
          gstk_task_free_cmd_buf(gstk_cmd_p);
          return GSTK_MEMORY_ERROR;
        }
      }
      gstk_cmd_p->cmd.mm_idle_state.cell_id.cell_len =
        mm_idle_evt_ptr->cell_id.cell_len;
      gstk_cmd(gstk_cmd_p);
    }
    else
    {
      return GSTK_MEMORY_ERROR;
    }
    break;
  case GSTK_IO_SEND_MM_INDICATION:
    mm_ind_ptr = (gstk_io_send_mm_ind_param_type *)param_in_ptr;

    gstk_cmd_p = gstk_task_get_cmd_buf();

    if (gstk_cmd_p == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }

    gstk_cmd_p->cmd.mm_idle_state.message_header.command_group =
      GSTK_MM_IDLE_CMD;

    gstk_cmd_p->cmd.mm_idle_state.message_header.command_id =
      GSTK_MM_INDICATION;

    gstk_cmd_p->cmd.mm_idle_state.message_header.user_data =
      mm_ind_ptr->type;

    gstk_cmd_p->cmd.mm_idle_state.stack_id =
      mm_ind_ptr->stack_id;

      /* as_id is subscription ID for DSDS/TSTS */
    if ( gstk_io_ctrl(GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
                     &mm_ind_ptr->as_id,
                     &slot) == GSTK_SUCCESS)
    {
      gstk_cmd_p->cmd.mm_idle_state.message_header.sim_slot_id = slot;
    }
    else
    {
      UIM_MSG_ERR_1("can't get slot for as_id 0x%x!", mm_ind_ptr->as_id);
      gstk_task_free_cmd_buf(gstk_cmd_p);
      return GSTK_ERROR;
    }
    UIM_MSG_HIGH_3("GSTK_MM_INDICATION stack_id: 0x%x as_id: 0x%x, slot: 0x%x",
                   mm_ind_ptr->stack_id, mm_ind_ptr->as_id, slot);
    gstk_cmd(gstk_cmd_p);
    break;

  case GSTK_IO_SEND_NW_REJ_EVENT:
    gstk_cmd_p = gstk_task_get_cmd_buf();
    if (gstk_cmd_p == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }

    gstk_cmd_p->cmd.nw_rej_ind.message_header.client_id     = gstk_instances_ptr[0]->client_id;
    gstk_cmd_p->cmd.nw_rej_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    gstk_cmd_p->cmd.nw_rej_ind.message_header.command_id    = (uint32)GSTK_NW_REJ_EVT_IND;
    gstk_cmd_p->cmd.nw_rej_ind.message_header.user_data     = cmd_details_ref_id;
    nw_rej_evt_ptr = (gstk_io_send_nw_rej_evt_type *)param_in_ptr;
    len = sizeof(gstk_nw_rej_evt_type);
    (void)gstk_memcpy(&gstk_cmd_p->cmd.nw_rej_ind.nw_rej_evt,
                      nw_rej_evt_ptr,
                      len,
                      len,
                      len);
    if(gstk_cmd_p->cmd.nw_rej_ind.nw_rej_evt.rat == (uint8)SYS_RAT_LTE_RADIO_ACCESS)
    {
      gstk_cmd_p->cmd.nw_rej_ind.nw_rej_evt.lac[0] = nw_rej_evt_ptr->lac[1];
      gstk_cmd_p->cmd.nw_rej_ind.nw_rej_evt.lac[1] = nw_rej_evt_ptr->lac[0];
    }

    /* Get Slot corresponding to AS_ID */
    if (gstk_io_ctrl(GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
                     &nw_rej_evt_ptr->as_id,
                     &slot) == GSTK_SUCCESS)
    {
      gstk_cmd_p->cmd.nw_rej_ind.message_header.sim_slot_id = slot;
    }
    else
    {
      UIM_MSG_ERR_1("can't get slot for as_id 0x%x!", nw_rej_evt_ptr->as_id);
      gstk_task_free_cmd_buf(gstk_cmd_p);
      return GSTK_ERROR;
    }
    gstk_cmd(gstk_cmd_p);
    break;
  case GSTK_IO_GET_CURR_LOC_STATUS:
    len = sizeof(gstk_location_status_enum_type);
    slot = (*(gstk_slot_id_enum_type *)(param_in_ptr));
    if (!GSTK_IS_VALID_SLOT_ID(slot))
    {
      UIM_MSG_ERR_1("Invalid slot id 0x%x!", slot);
      return GSTK_ERROR;
    }
    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)slot - 1], GSTK_ERROR);
    (void)gstk_memcpy(param_out_ptr,
                      &gstk_instances_ptr[(uint8)slot - 1]->gstk_next_curr_location_status,
                      len,
                      len,
                      len);
    break;
  case GSTK_IO_GET_TERMINAL_PROFILE_DATA:
    len = (uint32)GSTK_TERMINAL_PROFILE_SIZE;
    slot = (*(gstk_slot_id_enum_type *)(param_in_ptr));
    if (!GSTK_IS_VALID_SLOT_ID(slot))
    {
      UIM_MSG_ERR_1("Invalid slot id 0x%x!", slot);
      return GSTK_ERROR;
    }
    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)slot - 1], GSTK_ERROR);
    if(gstk_instances_ptr[(uint8)slot - 1]->gstk_num_of_tp_dl_received <
       gstk_shared_data.num_of_expected_tp)
    {
      return GSTK_INSUFFICIENT_INFO;
    }

    (void)gstk_memcpy(param_out_ptr,
                      gstk_instances_ptr[(uint8)slot - 1]->gstk_terminal_profile_cache,
                      len,
                      len,
                      len);
    break;

  case GSTK_IO_MANAGE_BIP_OTASP_CONCURRENCY:
    slot = (*(gstk_slot_id_enum_type *)(param_in_ptr));
    if (!GSTK_IS_VALID_SLOT_ID(slot))
    {
      UIM_MSG_ERR_1("Invalid slot id 0x%x!", slot);
      (*(boolean *)param_out_ptr) = TRUE;
      return GSTK_ERROR;
    }

    rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)slot - 1], GSTK_ERROR);
    if(gstk_instances_ptr[(uint8)slot-1]->bip_blocked)
    {
      gstk_instances_ptr[(uint8)slot-1]->bip_blocked = FALSE;
      (*(boolean *)param_out_ptr) = TRUE;
    }
    else if(!gstk_instances_ptr[(uint8)slot-1]->bip_blocked &&
            gstk_instances_ptr[(uint8)slot-1]->otasp_blocked)
    {
      (*(boolean *)param_out_ptr) = FALSE;
    }
    else if (!gstk_instances_ptr[(uint8)slot-1]->bip_blocked &&
             !gstk_instances_ptr[(uint8)slot-1]->otasp_blocked)
    {
      gstk_instances_ptr[(uint8)slot-1]->bip_blocked = TRUE;
      (*(boolean *)param_out_ptr) = TRUE;
    }
    rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
    break;

  case GSTK_IO_GET_DS_SYSTEM_RAT_INFO:
    slot = (*(gstk_slot_id_enum_type *)(param_in_ptr));
    if (!GSTK_IS_VALID_SLOT_ID(slot))
    {
      UIM_MSG_ERR_1("Invalid slot id 0x%x!", slot);
      (*(boolean *)param_out_ptr) = TRUE;
      return GSTK_ERROR;
    }
    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)slot - 1], GSTK_ERROR);
    *(ds_sys_system_status_ex_type**)param_out_ptr = &gstk_instances_ptr[(uint8)slot-1]->ds_sys_status_info;
    break;

  case GSTK_IO_GET_WLAN_AVAILABILITY:
    (*(boolean *)param_out_ptr) = gstk_shared_data.wlan_info.is_wlan_available;
    break;

  case GSTK_IO_SIM_INITIATED_MODIFIED_CC_RESP:
    cc_rsp = (gstk_cc_rsp_type *)param_in_ptr;
    memset(&gstk_shared_data.cc_req_action, 0x00, sizeof(gstk_cc_req_action_type));
    (void)gstk_memcpy(&gstk_shared_data.cc_req_action,
                      &cc_rsp->cc_req_action,
                      sizeof(gstk_cc_req_action_type),
                      sizeof(gstk_cc_req_action_type),
                      sizeof(gstk_cc_req_action_type));
    if(gstk_shared_data.cc_req_action.call_type == GSTK_USSD)
    {
      uint8 *ussd_string = NULL;
      ussd_string = (uint8 *)GSTK_CALLOC(
                gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.length);
      if(ussd_string == NULL)
      {
        memset(&gstk_shared_data.cc_req_action, 0x00, sizeof(gstk_cc_req_action_type));
        return GSTK_ERROR;
      }
      (void)gstk_memcpy(ussd_string,
                        gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text,
                        gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.length,
                        gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.length,
                        gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.length);

       gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text = ussd_string;
    }
    if(gstk_shared_data.cc_req_action.alpha.text != NULL)
    {
      uint8 *text_string = NULL;
      text_string = (uint8 *)GSTK_CALLOC(gstk_shared_data.cc_req_action.alpha.length);
      if(text_string == NULL)
      {
        if((gstk_shared_data.cc_req_action.call_type == GSTK_USSD) &&
           (gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text != NULL))
        {
          gstk_free(gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text);
          gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text = NULL;
        }
        memset(&gstk_shared_data.cc_req_action, 0x00, sizeof(gstk_cc_req_action_type));
        return GSTK_MEMORY_ERROR;
      }
      (void)gstk_memcpy(text_string,
                        gstk_shared_data.cc_req_action.alpha.text,
                        gstk_shared_data.cc_req_action.alpha.length,
                        gstk_shared_data.cc_req_action.alpha.length,
                        gstk_shared_data.cc_req_action.alpha.length);
      gstk_shared_data.cc_req_action.alpha.text = text_string;
    }
    break;

  case GSTK_IO_GET_ACTIVE_DATA_SUB_SLOT_ID:
    slot = GSTK_SLOT_1;
    if (gstk_io_ctrl(GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
                     &gstk_shared_data.active_data_subs,
                     &slot) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("Failed to retreive slot info for active DATA SUB: 0x%x",
                    gstk_shared_data.active_data_subs);
      return GSTK_ERROR;
    }
    if (GSTK_IS_VALID_SLOT_ID(slot))
    {
      UIM_MSG_HIGH_2("Found slot: 0x%x for active DATA SUB: 0x%x",
                     slot, gstk_shared_data.active_data_subs);
      len = sizeof(slot);
      (void)gstk_memcpy(param_out_ptr,
                        &slot,
                        len,
                        len,
                        len);
      return GSTK_SUCCESS;
    }
    else
    {
      UIM_MSG_ERR_2("Invalid slot id: 0x%x, Number of slots: 0x%x",
                    slot, gstk_shared_data.gstk_num_of_instances);
      return GSTK_ERROR;
    }
    break;
    
  case GSTK_IO_SEND_WMS_MT_MESSAGE_ERROR_EVENT:
    info_ptr = (wms_msg_event_info_s_type *)param_in_ptr;
    if (gstk_io_ctrl(
          GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
          &info_ptr->status_info.as_id,
          &slot) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_1("failed to get slot for as_id 0x%x!",
                    info_ptr->status_info.as_id);
      return GSTK_ERROR;
    }
    
    gstk_cmd_p = gstk_task_get_cmd_buf();
    
    if (gstk_cmd_p != NULL)
    {
      /* Build GSTK Command */
      gstk_cmd_p->cmd.wms_evt_ind.message_header.command_group = GSTK_WMS_EVT_IND;
      gstk_cmd_p->cmd.wms_evt_ind.message_header.sim_slot_id = slot;
      gstk_cmd_p->cmd.wms_evt_ind.wms_event = GSTK_WMS_MSG_EVENT_MT_MESSAGE_ERROR;
      gstk_task_put_cmd_buf(gstk_cmd_p);
    }
    break;

  default:
    UIM_MSG_ERR_1("Unknown IO cmd 0x%x!", io_cmd);
    return GSTK_ERROR;
  }
  return GSTK_SUCCESS;
} /*gstk_io_ctrl*/

/*===========================================================================
FUNCTION gstk_get_card_event_data

DESCRIPTION
  Library function to get cached proactive command

PARAMETER
  client_id  [Input]        : Indicates the client id that client got during it's
                              registration with GSTK
  cmd_ref_id [Input]        : Index to command_details_ref_table
  event_ptr  [Input/Output] : Pointer to client's memory where proactive command
                              details should be copied
                              data_format_type should be filled in by client.
                              Client should fill in the format type and allocated memory
                              for copying command.

DEPENDENCIES
  None

RETURN VALUE
  GSTK_SUCCESS - When a command is copied without errors
  GSTK_ERROR   - Otherwise

COMMENTS
  1. It's client's responsibility to free the member fields in the event_ptr after
     calling this function.
  2. If client doesn't call this function to retrieve the command, due to some
     erroneous situation, then the internal pointers in event_ptr which gets shallow
     copied will get freed while freeing the respective slot in command_details_ref_table_ptr

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_get_card_event_data(
  gstk_client_id_type                   client_id, /* for future use */
  uint32                                cmd_ref_id,
  gstk_client_pro_cmd_reg_data_type*    event_ptr)
{
  gstk_client_type client_type;
  uint32           index          = GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id);

  if(gstk_util_compare_ref_id(cmd_ref_id) != GSTK_SUCCESS)
  {
    return GSTK_BAD_PARAM;
  }

  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("Client not registered 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  /* Get client type from client id */
  client_type = gstk_shared_data.gstk_client_table[client_id-1].client_type;

  if (client_type ==  GSTK_WAKEUP_TYPE &&
      event_ptr   !=  NULL &&
      index  <   GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_HIGH_1("gstk_get_card_event_data - cmd_ref_id = %d", cmd_ref_id);
    if (GSTK_RAW_FORMAT == event_ptr->data_format_type)
    {
      if (event_ptr->pro_cmd_reg_data.raw_req_rsp_ptr == NULL)
      {
        UIM_MSG_ERR_0("NULL ptr!");
        return GSTK_BAD_PARAM;
      }
      (void)memscpy (&event_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload,
              sizeof (gstk_generic_data_type),
              &command_details_ref_table_ptr[index].raw_cmd,
              sizeof (gstk_generic_data_type));
      return GSTK_SUCCESS;
    }
    if (GSTK_GSTK_FORMAT == event_ptr->data_format_type &&
        NULL != command_details_ref_table_ptr[index].cmd_ptr)
    {
      /* Copy(shallow) data */
      (void)memscpy((void*)event_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr,
             sizeof(gstk_cmd_from_card_type),
             (void*)command_details_ref_table_ptr[index].cmd_ptr,
             sizeof(gstk_cmd_from_card_type));
      gstk_free (command_details_ref_table_ptr[index].cmd_ptr);
      command_details_ref_table_ptr[index].cmd_ptr = NULL;
      return GSTK_SUCCESS;
    }
  }
  UIM_MSG_ERR_2("gstk_get_card_event_data ERROR, client id = 0x%x, client type = 0x%x",
                client_id, client_type);
  return GSTK_ERROR;
} /* gstk_get_card_event_data */

/*===========================================================================
FUNCTION   gstk_send_icon_display_rsp

DESCRIPTION
  Library function called by client to say if the icon was displayed.

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback
  icon_disp :  yes / no response for whether the icon was displayed

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_icon_display_rsp(
  gstk_client_id_type  client_id,
  uint32               gstk_user_data,
  boolean              icon_disp
)
{
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  /* Check for validity of cmd_ref_id */
  if (GSTK_CMD_REF_TABLE_INDEX(gstk_user_data) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("client ref id not within range 0x%x", gstk_user_data);
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_1(" IN gstk_send_icon_display_rsp: Icon_disp is 0x%x",
                 icon_disp);
  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id               = gstk_user_data;
  task_cmd_ptr->hdr.cli_user_data            = 0;
  task_cmd_ptr->hdr.cmd                      = ESTK_ICON_DISPLAY_CONF_CMD;
  task_cmd_ptr->hdr.sim_slot_id              = gstk_util_get_sim_slot_id(client_id);
  task_cmd_ptr->result                       = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.user_rsp.icon_disp  = icon_disp;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)gstk_user_data;
  (void)icon_disp;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_send_icon_display_rsp */

/*===========================================================================
FUNCTION   gstk_send_user_cnf_alpha_and_icon_disp_rsp

DESCRIPTION
  Library function called by client to vote for YES or NO as a response for the
  user confirmation alpha sent by ESTK for Set up Call or Open Channel command.

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback
               reporting the set up call (display func). The client needs
               to send back this reference id when sending the user response
               as to whether the call should be set up or not.
  yes_no_rsp : yes / no response for setting up the call
  icon_disp  : Whether icon was displayed

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_user_cnf_alpha_and_icon_disp_rsp(
  gstk_client_id_type  client_id,
  uint32               gstk_user_data,
  boolean              yes_no_rsp,
  boolean              icon_disp
)
{
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  /* Check for validity of cmd_ref_id */
  if (GSTK_CMD_REF_TABLE_INDEX(gstk_user_data) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("client ref id not within range 0x%x", gstk_user_data);
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_2(" IN gstk_send_user_cnf_alpha_and_icon_disp_rsp: Usr cnf=0x%x, icon disp=0x%x",
                 yes_no_rsp, icon_disp);

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id               = gstk_user_data;
  task_cmd_ptr->hdr.cli_user_data            = 0;
  task_cmd_ptr->hdr.cmd                      = ESTK_GENERAL_USER_CNF_CMD;
  task_cmd_ptr->hdr.sim_slot_id              = gstk_util_get_sim_slot_id(client_id);
  task_cmd_ptr->result                       = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.user_rsp.yes_no_rsp = yes_no_rsp;
  task_cmd_ptr->cmd_data.user_rsp.icon_disp  = icon_disp;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
 #else /* FEATURE_ESTK */
  (void)client_id;
  (void)gstk_user_data;
  (void)yes_no_rsp;
  (void)icon_disp;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_send_user_cnf_alpha_and_icon_disp_rsp*/

/*===========================================================================
FUNCTION gstk_tal_client_registration

DESCRIPTION
  This function is used by clients to register call backs for event
  notifications

PARAMETERS
 client_type -       This describes whether the client resides on local or
                     remote processor
 gstk_tal_cb -       This callback is used by TAL to inform the clients
                     about UIM driver response or to send proactive commands
 tech_support_mask - Clients use this bit mask to inform TAL about supported
                     technologies/proactive commands.
DEPENDENCIES
  None

RETURN VALUE
 gstk_status_enum_type
 GSTK_SUCCESS:            The command was queued onto the GSTK Queue.
 GSTK_BAD_PARAM:          Invalid Input Param
 GSTK_MEMORY_ERROR:       Error while allocating memory on heap
 GSTK_ERROR               Generic Error/API not supported

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_tal_client_registration(
  gstk_tal_client_type                  client_type,
  gstk_tal_reg_cb_type                  gstk_tal_cb,
  uint32                                tech_support_mask)
{
#ifdef FEATURE_GSTK_TAL
  gstk_task_cmd_type            *task_cmd_ptr    = NULL;

  UIM_MSG_HIGH_1("TAL: gstk_tal_client_registration - client_type 0x%x",
                 client_type);
  (void)tech_support_mask; /* temporarily not available */
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_tal_cb, GSTK_ERROR);

  if (client_type != GSTK_TAL_LOCAL_CLIENT &&
      client_type != GSTK_TAL_REMOTE_CLIENT)
  {
    UIM_MSG_ERR_0("TAL:Invalid client type");
    return GSTK_BAD_PARAM;
  }

  /* get the command from buffer */
  task_cmd_ptr = gstk_task_get_cmd_buf();

  if (task_cmd_ptr)
  {
    /* Build GSTK Command */
    task_cmd_ptr->cmd.tal_cmd.message_header.command_group = GSTK_TAL_CMD;
    task_cmd_ptr->cmd.tal_cmd.message_header.sim_slot_id   = GSTK_SLOT_1;
    task_cmd_ptr->cmd.tal_cmd.client_type                  = client_type;
    task_cmd_ptr->cmd.tal_cmd.uim_cmd.cmd_type             = GSTK_CLIENT_REG_REQ_CMD;
    task_cmd_ptr->cmd.tal_cmd.gstk_tal_cb                  = gstk_tal_cb;
    /* put command back to command queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }
  return GSTK_SUCCESS;
#else
#ifdef FEATURE_GSTK_FUSION_REMOTE
  return gstk_fusion_tal_client_registration(client_type,
                                             gstk_tal_cb,
                                             tech_support_mask);
#else
  (void)client_type;
  (void)gstk_tal_cb;
  (void)tech_support_mask;
  UIM_MSG_ERR_0("API not supported for build");
  return GSTK_ERROR;
#endif /*FEATURE_GSTK_FUSION_REMOTE*/
#endif /* FEATURE_GSTK_TAL */
} /*gstk_tal_client_registration*/

/* ----------------------------------------------------------------------------
   FUNCTION:      GSTK_TAL_SEND_CMD

   DESCRIPTION:
     This function is used by GSTK local and remote to send terminal response,
     terminal profile and envelope commands to TAL. TAL sends this command to
     UIM driver and then notifies the caller about the result

     Params:
     tal_cmd     -> contains TAL command data to be sent to UIM driver
     client_type -> contains the client Local or Remote that sent the cmd

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     gstk_status_enum_type
     GSTK_SUCCESS:          The cmd was successfully queued to GSTK command buffer
     GSTK_MEMORY_ERROR      Error in allocating memory
     GSTK_BAD_PARAM         Erroneous Input Parameters
     GSTK_ERROR             Generic Error/API not supported

   SIDE EFFECTS:
-------------------------------------------------------------------------------*/
gstk_status_enum_type gstk_tal_send_cmd(
  gstk_tal_client_type             client_type,
  gstk_tal_uim_cmd_type           *tal_cmd)
{
#ifdef FEATURE_GSTK_TAL
  gstk_task_cmd_type            *task_cmd_ptr    = NULL;
  UIM_MSG_HIGH_0("TAL: gstk_tal_send_cmd");

  GSTK_RETURN_ERROR_IF_NULL_PTR(tal_cmd, GSTK_ERROR);
  if((client_type != GSTK_TAL_LOCAL_CLIENT) &&
     (client_type != GSTK_TAL_REMOTE_CLIENT))
  {
    UIM_MSG_ERR_3("TAL: gstk_tal_send_cmd incorrect client_type 0x%x tal_cmd 0x%x, num_bytes 0x%x",
                  client_type, tal_cmd->cmd_type, tal_cmd->num_bytes);
    return GSTK_BAD_PARAM;
  }

  /* get the command from buffer */
  task_cmd_ptr = gstk_task_get_cmd_buf();

  if (task_cmd_ptr)
  {
    /* Build GSTK Command */
    task_cmd_ptr->cmd.tal_cmd.message_header.command_group = GSTK_TAL_CMD;
    task_cmd_ptr->cmd.tal_cmd.client_type                  = client_type;
    task_cmd_ptr->cmd.tal_cmd.uim_cmd.cmd_type             = tal_cmd->cmd_type;
    task_cmd_ptr->cmd.tal_cmd.uim_cmd.user_data            = tal_cmd->user_data;
    task_cmd_ptr->cmd.tal_cmd.uim_cmd.num_bytes            = tal_cmd->num_bytes;
    task_cmd_ptr->cmd.tal_cmd.uim_cmd.slot                 = tal_cmd->slot;
    if (task_cmd_ptr->cmd.tal_cmd.uim_cmd.num_bytes > 0)
    {
      task_cmd_ptr->cmd.tal_cmd.uim_cmd.data_ptr = GSTK_CALLOC(
        task_cmd_ptr->cmd.tal_cmd.uim_cmd.num_bytes);
      if(task_cmd_ptr->cmd.tal_cmd.uim_cmd.data_ptr)
      {
        (void)memscpy(task_cmd_ptr->cmd.tal_cmd.uim_cmd.data_ptr,
               task_cmd_ptr->cmd.tal_cmd.uim_cmd.num_bytes,
               tal_cmd->data_ptr,
               tal_cmd->num_bytes);
      }
      else
      {
        gstk_task_free_cmd_buf(task_cmd_ptr);
        return GSTK_MEMORY_ERROR;
      }
    }
    /* put command to GSTK command queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }
  return GSTK_SUCCESS;
#else
#ifdef FEATURE_GSTK_FUSION_REMOTE
  return gstk_fusion_tal_send_cmd(client_type,tal_cmd);
#else
  (void)client_type;
  (void)tal_cmd;
  UIM_MSG_ERR_0("API not supported for build");
  return GSTK_ERROR;
#endif /* FEATURE_GSTK_FUSION_REMOTE */
#endif /*  FEATURE_GSTK_TAL */
} /*gstk_tal_send_cmd*/

/* ----------------------------------------------------------------------------
   FUNCTION:      GSTK_TAL_NOTIFY_RAT

   DESCRIPTION:
     This function is used by GSTK local and remote to notify TAL about
     current RAT
     Params:
     rat = Subscription to the mentioned Radio Access Technology is available

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     gstk_status_enum_type
     GSTK_SUCCESS:          The cmd was successfully sent by TAL to UIM
     GSTK_ERROR:            The cmd was not sent by TAL to UIM

   SIDE EFFECTS:
-------------------------------------------------------------------------------*/
gstk_status_enum_type gstk_tal_notify_rat(
  gstk_tal_client_type             client_type,
  gstk_access_technology_type      rat)
{
#ifdef FEATURE_GSTK_TAL
  gstk_task_cmd_type            *task_cmd_ptr    = NULL;

  UIM_MSG_HIGH_2("TAL: Client is 0x%x and rat is 0x%x", client_type, rat);

  if(client_type >= (gstk_tal_client_type)GSTK_TAL_MAX_CLIENTS)
  {
    return GSTK_ERROR;
  }

  if ((rat != GSTK_ACCESS_TECH_GSM) &&
      (rat != GSTK_ACCESS_TECH_UTRAN) &&
      (rat != GSTK_ACCESS_TECH_CDMA) &&
      (rat != GSTK_ACCESS_TECH_HRPD) &&
      (rat != GSTK_ACCESS_TECH_EHRPD) &&
      (rat != GSTK_ACCESS_TECH_LTE) &&
      (rat != GSTK_ACCESS_TECH_LTE) &&
      (rat != GSTK_ACCESS_NONE))
  {
      UIM_MSG_ERR_1("Unknown Access Technology type 0x%x", rat);
      return GSTK_ERROR;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr)
  {
    /* Build GSTK Command */
    task_cmd_ptr->cmd.tal_cmd.message_header.command_group = GSTK_TAL_CMD;
    task_cmd_ptr->cmd.tal_cmd.message_header.sim_slot_id   = GSTK_SLOT_1;
    task_cmd_ptr->cmd.tal_cmd.client_type                  = client_type;
    task_cmd_ptr->cmd.tal_cmd.uim_cmd.cmd_type             = GSTK_GET_CURR_ACCESS_TECH_CMD;
    task_cmd_ptr->cmd.tal_cmd.uim_cmd.user_data            = rat;
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }
  return GSTK_SUCCESS;
#else
#ifdef FEATURE_GSTK_FUSION_REMOTE
  return gstk_fusion_tal_notify_rat(client_type, rat);
#else
  (void)client_type;
  (void)rat;
  UIM_MSG_ERR_0("API not supported for build");
  return GSTK_ERROR;
#endif /* FEATURE_GSTK_FUSION_REMOTE */
#endif /* FEATURE_GSTK_TAL */
} /*gstk_tal_notify_rat*/

/*============================================================================
  FUNCTION:      GSTK_GSDI_SET_FETCH_STATUS

  DESCRIPTION:
    API used by GSDI to set status for fetch allowed or not

  PARAMETERS:
    slot_id: [Input]  Slot
    status:  [Input]  FETCH Status
                      TRUE - FETCH Allowed
                      FALSE - FETCH Not Allowed

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None

  SEE ALSO:

========================================================================== */
void gstk_gsdi_set_fetch_status(
  gstk_slot_id_enum_type       slot_id,
  boolean                      status
)
{
  if (!GSTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("invalid slot id: 0x%x", slot_id);
    return;
  }

  UIM_MSG_HIGH_2("gstk_gsdi_set_fetch_status: Slot = 0x%x, status = 0x%x",
                 slot_id, status);

  if((((uint8)slot_id -1) >= 0) && (((uint8)slot_id - 1) < gstk_shared_data.gstk_num_of_instances)
     && (((uint8)slot_id - 1) < GSTK_MAX_NUM_OF_INSTANCES))
  {
    gstk_shared_data.fetch_allowed[(uint8)slot_id - 1] = status;
  }

#ifdef FEATURE_UIM_MULTI_PROFILE
  if ((gstk_shared_data.ready_tasks & (00000001 << (uint8) GSTK_CM_TASK)) &&
      (gstk_shared_data.ready_tasks & (00000001 << (uint8) GSTK_QMI_MODEM_TASK)) &&
      gstk_shared_data.fetch_allowed[(uint8)slot_id - 1] &&
      gstk_instances_ptr[(uint8)slot_id - 1]->is_fetch_ignored)
  {
    /* call uimdrv API to check pending proactive command */
    (void)uim_check_pending_proactive_command((uim_slot_type)slot_id);
  }
#endif /* FEATURE_UIM_MULTI_PROFILE */
} /* gstk_gsdi_set_fetch_status */

/*============================================================================
  FUNCTION:      gstk_send_attach_ctrl_req

  DESCRIPTION:
    API used query USIM if or not attach to PLMN is allowed

    Once the function is invoked, the calling task is expected to call
    rex_wait(sig_to_wait). Once result is available, GSTK will set sig_to_wait
    signal the caller task and the result will be returned by calling
    gstk_get_attach_ctrl_res().

  PARAMETERS:
    task_tcb: [Input]  the caller task TCB
    sig_to_wait: [Input]  the signal will be set
    mcc_mnc_p: [Input] the pointer to a 3-byte array that store MCC/MNC

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    gstk_status_enum_type
      GSTK_SUCCESS - succeed in queuing the request
      GSTK_ERROR - failed to queue the request


  SIDE EFFECTS:
    None

  SEE ALSO:
    None
========================================================================== */
gstk_status_enum_type gstk_send_attach_ctrl_req(
  void   *task_tcb,
  uint32 sig_to_wait,
  uint8  *mcc_mnc_p
)
{
  gstk_slot_id_enum_type slot          = GSTK_SLOT_1;
  gstk_task_cmd_type     *task_cmd_ptr = NULL;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[0], GSTK_ERROR);

  if (mcc_mnc_p == NULL || task_tcb == NULL)
  {
    UIM_MSG_ERR_2("null ptr: 0x%x 0x%x", mcc_mnc_p, task_tcb);
    return GSTK_BAD_PARAM;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr == NULL)
  {
    return GSTK_ERROR;
  }
  else
  {
    task_cmd_ptr->cmd.attach_ctrl_ind.message_header.sim_slot_id =
      slot;
    task_cmd_ptr->cmd.attach_ctrl_ind.message_header.command_group =
      GSTK_ENVELOPE_CMD;
    task_cmd_ptr->cmd.attach_ctrl_ind.message_header.command_id =
      (int)GSTK_LOCATION_INFORMATION_IND;
    /* set it to invalid value to indicate the cmd is to query loc info
       envelope cmd status */
    task_cmd_ptr->cmd.attach_ctrl_ind.message_header.user_data =
      GSTK_MAX_PROACTIVE_COMMAND;
    task_cmd_ptr->cmd.attach_ctrl_ind.message_header.client_id = 
      gstk_instances_ptr[0]->client_id;
    task_cmd_ptr->cmd.attach_ctrl_ind.attach_ctrl.task_tcb =
      task_tcb;
    task_cmd_ptr->cmd.attach_ctrl_ind.attach_ctrl.sig_to_wait =
      sig_to_wait;
    (void)gstk_memcpy(task_cmd_ptr->cmd.attach_ctrl_ind.attach_ctrl.mcc_mnc,
                      mcc_mnc_p,
                      GSTK_MAX_PLMN_LEN,
                      GSTK_MAX_PLMN_LEN,
                      GSTK_MAX_PLMN_LEN);
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }

  return GSTK_SUCCESS;
} /*gstk_send_attach_ctrl_req*/

/*============================================================================
  FUNCTION:      gstk_get_attach_ctrl_res

  DESCRIPTION:
    API used to retrieve the result of attach control request sent early

  PARAMETERS:
    None

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    gstk_status_enum_type
      GSTK_SUCCESS - succeed in queuing the request
      GSTK_ERROR - failed to queue the request


  SIDE EFFECTS:
    None

  SEE ALSO:
    None
========================================================================== */
gstk_attach_ctrl_result_enum_type gstk_get_attach_ctrl_res()
{
  gstk_attach_ctrl_result_enum_type result = GSTK_ATTACH_CTRL_INVALID;

  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  result = gstk_instances_ptr[0]->attach_ctrl_res;
  gstk_instances_ptr[0]->attach_ctrl_res = GSTK_ATTACH_CTRL_INVALID;
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);

  return result;
} /*gstk_get_attach_ctrl_res*/

/*===========================================================================
FUNCTION   gstk_scws_ch_status_req

DESCRIPTION
  Library function called by client for SCWS Ch status change.

PARAMETERS
  client_id  : client id
  ch id     :  Channel id
  ch status :  Channel status

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_ch_status_req(
  gstk_client_id_type           client_id,
  uint32                        ch_id,
  gstk_scws_ch_state_enum_type  ch_status
)
{
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type sim_slot_id    = GSTK_SLOT_AUTO;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  sim_slot_id = gstk_util_get_sim_slot_id(client_id);

  if(!GSTK_IS_VALID_SLOT_ID(sim_slot_id))
  {
     return GSTK_ERROR;
  }

  UIM_MSG_HIGH_3("gstk_scws_ch_status_req: ch_id=0x%x, ch_status=0x%x, RECOVERY=0x%x",
                 ch_id, ch_status, GSTK_IDLE_S_RECOVERY(sim_slot_id));

  if (GSTK_IDLE_S_RECOVERY(sim_slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = 0; /* Not used */
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_CH_STATUS_REQ_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = gstk_util_get_sim_slot_id(client_id);
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_ch_status_req.ch_id     = ch_id;
  task_cmd_ptr->cmd_data.scws_ch_status_req.ch_status = ch_status;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)ch_id;
  (void)ch_status;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_scws_ch_status_req*/

/*===========================================================================
FUNCTION   gstk_scws_data_avail_req_type

DESCRIPTION
  Library function called by client to say if data is available on SCWS socket.

PARAMETERS
  client_id      : client id
  ch id          : Channel status
  data           : Data from the scws socket
  data_remaining : How much data is left
DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_data_avail_req_type(
  gstk_client_id_type      client_id,
  uint32                   ch_id,
  gstk_generic_data_type   data,
  uint32                   data_remaining

)
{
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type sim_slot_id    = GSTK_SLOT_AUTO;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  if (data.data_len <= 0)
  {
    UIM_MSG_ERR_1("invalid data len! 0x%x", data.data_len);
    return GSTK_BAD_PARAM;
  }

  if (data.data_buffer_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Ptr!");
    return GSTK_BAD_PARAM;
  }

  sim_slot_id = gstk_util_get_sim_slot_id(client_id);

  if(!GSTK_IS_VALID_SLOT_ID(sim_slot_id))
  {
     return GSTK_ERROR;
  }

  UIM_MSG_HIGH_1("gstk_scws_data_avail_req: GSTK_IDLE_S_RECOVERY=0x%x",
                 GSTK_IDLE_S_RECOVERY(sim_slot_id));

  if (GSTK_IDLE_S_RECOVERY(sim_slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = 0; /* Not used */
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_DATA_AVAIL_REQ_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = gstk_util_get_sim_slot_id(client_id);
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_data_avail_req.ch_id    = ch_id;
  task_cmd_ptr->cmd_data.scws_data_avail_req.data_payload.len = (uint32)data.data_len;
  task_cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr = (uint8*)GSTK_CALLOC((dword)data.data_len);
  (void)gstk_memcpy((void*)task_cmd_ptr->cmd_data.scws_data_avail_req.data_payload.data_ptr,
                    (void*)data.data_buffer_ptr,
                    (size_t)data.data_len,
                    (size_t)data.data_len,
                    (size_t)data.data_len);
  /* ESTK will free the pointer after copying the data to the receive buffer */

  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)ch_id;
  (void)data;
  (void)data_remaining;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* estk_scws_data_avail_req_type */

/*===========================================================================
FUNCTION   gstk_scws_open_ch_rsp

DESCRIPTION
  Library function called by client to report SCWS Open Ch response.

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback
  ch id      : Channel id
  ch status  :  Channel status

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_open_ch_rsp(
  gstk_client_id_type           client_id,
  uint32                        cmd_ref_id,
  uint32                        ch_id,
  gstk_scws_ch_state_enum_type  ch_status
)
{
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type    slot_id       = GSTK_SLOT_1;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  slot_id = gstk_util_get_sim_slot_id(client_id);

  UIM_MSG_HIGH_3("gstk_scws_open_ch_rsp: ch_id=0x%x, ch_status=0x%x, RECOVERY=0x%x",
                 ch_id,
                 ch_status,
                 GSTK_IDLE_S_RECOVERY(slot_id));

  if (GSTK_IDLE_S_RECOVERY(slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = cmd_ref_id;
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_OPEN_CH_RSP_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = slot_id;
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_open_ch_rsp.ch_id       = ch_id;
  task_cmd_ptr->cmd_data.scws_open_ch_rsp.ch_status   = ch_status;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)cmd_ref_id;
  (void)ch_id;
  (void)ch_status;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_scws_open_ch_rsp*/

/*===========================================================================
FUNCTION   gstk_scws_close_ch_rsp

DESCRIPTION
  Library function called by client to report SCWS Close Channel response.

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback
  ch id      : Channel id
  ch status  :  Channel status

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_close_ch_rsp(
  gstk_client_id_type           client_id,
  uint32                        cmd_ref_id,
  uint32                        ch_id,
  gstk_scws_ch_state_enum_type  ch_status
)
{
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type    slot_id       = GSTK_SLOT_1;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  slot_id = gstk_util_get_sim_slot_id(client_id);

  UIM_MSG_HIGH_3("gstk_scws_close_ch_rsp: ch_id=0x%x, ch_status=0x%x, RECOVERY=0x%x",
                 ch_id,
                 ch_status,
                 GSTK_IDLE_S_RECOVERY(slot_id));

  if (GSTK_IDLE_S_RECOVERY(slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = cmd_ref_id;
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_CLOSE_CH_RSP_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = slot_id;
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_close_ch_rsp.ch_id      = ch_id;
  task_cmd_ptr->cmd_data.scws_close_ch_rsp.ch_status  = ch_status;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)cmd_ref_id;
  (void)ch_id;
  (void)ch_status;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
#endif /* FEATURE_ESTK */
}/* gstk_scws_close_ch_rsp*/

/*===========================================================================
FUNCTION   gstk_scws_send_data_rsp

DESCRIPTION
  Library function called by client to report SCWS Send Data respose.

PARAMETERS
  client_id     : client id
  cmd_ref_id    : This is the gstk command reference id in the
                  gstk header information sent to the client in the callback
  ch id         : Channel status
  write_success : Whether the write was a success

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type  gstk_scws_send_data_rsp(
  gstk_client_id_type      client_id,
  uint32                   cmd_ref_id,
  uint32                   ch_id,
  boolean                  write_success
)
{
#ifdef FEATURE_ESTK
  estk_cmd_type          *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type    slot_id       = GSTK_SLOT_1;

  /* Check for validity of client id */
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("GSTK_CLIENT_NOT_REGISTERED 0x%x", client_id);
    return GSTK_CLIENT_NOT_REGISTERED;
  }

  slot_id = gstk_util_get_sim_slot_id(client_id);

  UIM_MSG_HIGH_3("gstk_scws_send_data_rsp: ch_id=0x%x, write_success=0x%x, RECOVERY=0x%x",
                 ch_id,
                 write_success,
                 GSTK_IDLE_S_RECOVERY(slot_id));

  if (GSTK_IDLE_S_RECOVERY(slot_id))
  {
    return GSTK_ERROR;
  }

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf(); /* memset is done in this function */
  if (task_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }
  task_cmd_ptr->hdr.cmd_ref_id                        = cmd_ref_id;
  task_cmd_ptr->hdr.cli_user_data                     = 0;
  task_cmd_ptr->hdr.cmd                               = ESTK_SCWS_SEND_DATA_RSP_CMD;
  task_cmd_ptr->hdr.sim_slot_id                       = slot_id;
  task_cmd_ptr->result                                = ESTK_SUCCESS;
  task_cmd_ptr->cmd_data.scws_send_data_rsp.ch_id      = ch_id;
  task_cmd_ptr->cmd_data.scws_send_data_rsp.is_success  = write_success;
  /* Ignore return value for estk_put_cmd() as the function returns error
  ** for a null task_cmd_ptr which has already been checked for in this function
  */
  estk_put_cmd(task_cmd_ptr);
  return GSTK_SUCCESS;
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)cmd_ref_id;
  (void)ch_id;
  (void)write_success;
  UIM_MSG_ERR_0("FEATURE_ESTK not defined");
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_scws_send_data_rsp*/

/* ----------------------------------------------------------------------------
   FUNCTION:      GSTK_TAL_NOTIFY_LS

   DESCRIPTION:
     This function is used by GSTK local and remote to notify TAL about
     current Location Status
     Params:
     ls = Current location status value

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     gstk_status_enum_type
     GSTK_SUCCESS:          The cmd was successfully queued to GSTK Queue
     GSTK_ERROR:            There was an error queueing command to GSTK.

   SIDE EFFECTS:
-------------------------------------------------------------------------------*/
gstk_status_enum_type gstk_tal_notify_ls(
  gstk_tal_client_type             client_type,
  gstk_location_status_enum_type   ls)
{
#ifdef FEATURE_GSTK_TAL
  gstk_task_cmd_type            *task_cmd_ptr    = NULL;

  UIM_MSG_HIGH_2("TAL: Client is 0x%x and LS is 0x%x", client_type, ls);

  if(client_type >= (gstk_tal_client_type)GSTK_TAL_MAX_CLIENTS)
  {
    return GSTK_ERROR;
  }

  if ((ls != GSTK_NORMAL_SERVICE) &&
      (ls != GSTK_LIMITED_SERVICE) &&
      (ls != GSTK_NO_SERVICE))
  {
      UIM_MSG_ERR_1("Unknown Location Status Value 0x%x", ls);
      return GSTK_ERROR;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr)
  {
    /* Build GSTK Command */
    task_cmd_ptr->cmd.tal_cmd.message_header.command_group = GSTK_TAL_CMD;
    task_cmd_ptr->cmd.tal_cmd.message_header.sim_slot_id   = GSTK_SLOT_1;
    task_cmd_ptr->cmd.tal_cmd.client_type                  = client_type;
    task_cmd_ptr->cmd.tal_cmd.uim_cmd.cmd_type             = GSTK_TAL_SEND_LS_CMD;
    task_cmd_ptr->cmd.tal_cmd.uim_cmd.user_data            = (uint32)ls;
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }
  return GSTK_SUCCESS;
#else
#ifdef FEATURE_GSTK_FUSION_REMOTE
  return gstk_fusion_tal_notify_ls(client_type, ls);
#else
  (void)client_type;
  (void)ls;
  UIM_MSG_ERR_0("API not supported for build");
  return GSTK_ERROR;
#endif /* FEATURE_GSTK_FUSION_REMOTE */
#endif /* FEATURE_GSTK_TAL */
} /* gstk_tal_notify_ls */

/*===========================================================================
FUNCTION GSTK_SLOT_SEND_TERMINAL_PROFILE_DL_COMMAND

DESCRIPTION
  MMGSDI calls this function to inform GSTK to perform terminal profile download procedure for
  specified SIM/USIM slot

PARAMETERS
  slot         : [Input] The SIM slot ID
  ok_to_dl     : Ok to perform TP download or not
  user_data    : [Input] User data that client wants GSTK to keep track of
  gstk_tp_dl_cb: [Input] Function pointer to which GSTK will send the card
                 response data corresponding to the Cell Broadcast download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_terminal_profile_dl_command (
  gstk_slot_id_enum_type      slot,
  boolean                     ok_to_dl,
  uint32                      user_data,
  gstk_evt_cb_funct_type      gstk_tp_dl_cb
)
{
  gstk_task_cmd_type    * task_cmd = NULL;

  UIM_MSG_HIGH_1("gstk_slot_send_terminal_profile_dl_command for slot 0x%x",
                 slot);
  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return GSTK_BAD_PARAM;
  }
  
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)slot - 1], GSTK_BAD_PARAM);
  
  if(gstk_instances_ptr[(uint32)slot-1] == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* validate call back function */
  if(gstk_tp_dl_cb == NULL)
  {
    UIM_MSG_ERR_0("Null Input Param:");
    return GSTK_NULL_INPUT_PARAM;
  }

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd != NULL)
  {
    /* successfully obtained command buffer - build the envelope command */
    task_cmd->cmd.mmgsdi_evt.message_header.sim_slot_id   = (gstk_slot_id_enum_type)slot;
    task_cmd->cmd.mmgsdi_evt.message_header.client_id     = gstk_instances_ptr[(uint32)slot-1]->client_id;
    task_cmd->cmd.mmgsdi_evt.message_header.command_group = GSTK_MMGSDI_CMD;
    task_cmd->cmd.mmgsdi_evt.message_header.command_id    = (int)GSTK_MMGSDI_TERMINAL_PROFILE_DL_EVT;
    task_cmd->cmd.mmgsdi_evt.message_header.user_data     = user_data;
    task_cmd->cmd.mmgsdi_evt.evt.terminal_profile.ok_dl   = ok_to_dl;
    task_cmd->cmd.mmgsdi_evt.client_cb                    = gstk_tp_dl_cb;

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }
  return GSTK_SUCCESS;
} /* gstk_slot_send_terminal_profile_dl_command */

/*===========================================================================
FUNCTION gstk_send_recovery_cmd

DESCRIPTION
  This function is called by UIM to send the Recovery command to GSTK

DEPENDENCIES
  None

RETURN VALUE
  Void
===========================================================================*/
gstk_status_enum_type gstk_send_recovery_cmd(
  uim_slot_type                   slot,
  uimdrv_recovery_stage_enum_type stage
)

  {
  gstk_status_enum_type  result  = GSTK_SUCCESS;
  gstk_task_cmd_type    *cmd_ptr = NULL;

  UIM_MSG_HIGH_2("gstk_send_recovery_cmd for slot 0x%x, with recovery stage: 0x%x",
                 slot, stage);

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("Invalid Slot received in gstk_send_recovery_cmd: 0x%x",
                  slot);
    return GSTK_BAD_PARAM;
  }

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_instances_ptr[(uint8)slot - 1], GSTK_BAD_PARAM);

  if(stage == UIMDRV_RECOVERY_STAGE_START)
  {
    cmd_ptr = gstk_task_get_cmd_buf();
    if(cmd_ptr != NULL)
    {
      if(gstk_instances_ptr[(uint8)slot - 1]->gstk_sub_state == GSTK_RECOVERY_SUB_S)
      {
        gstk_util_find_and_delete_recovery_end_cmd();
      }
      cmd_ptr->cmd.general_cmd.message_header.sim_slot_id   = (gstk_slot_id_enum_type)slot;
      cmd_ptr->cmd.general_cmd.message_header.command_group = GSTK_RECOVERY_CMD;
      cmd_ptr->cmd.general_cmd.message_header.command_id    = GSTK_RECOVERY_START_CMD;
      gstk_task_put_cmd_priority_buf(cmd_ptr);
    }
    else
    {
      result = GSTK_MEMORY_ERROR;
    }
  }
  else if(stage == UIMDRV_RECOVERY_STAGE_END)
  {
    cmd_ptr = gstk_task_get_cmd_buf();
    if(cmd_ptr != NULL)
    {
      cmd_ptr->cmd.general_cmd.message_header.sim_slot_id   = (gstk_slot_id_enum_type)slot;
      cmd_ptr->cmd.general_cmd.message_header.command_group = GSTK_RECOVERY_CMD;
      cmd_ptr->cmd.general_cmd.message_header.command_id    = GSTK_RECOVERY_END_CMD;
      gstk_task_put_cmd_buf(cmd_ptr);
    }
    else
    {
      result = GSTK_MEMORY_ERROR;
    }
  }
  else
  {
    result = GSTK_BAD_PARAM;
  }
  return result;
} /* gstk_send_recovery_cmd */

/*===========================================================================
FUNCTION GSTK_SLOT_SEND_ENVELOPE_CC_COMMAND_EXT

DESCRIPTION

  This function sends the Call Control envelope command to SIM/USIM based on
  the slot, radio access technology and location information provided.
  Clients need to supply the gstk_cc_ind_type_ext as the parameter which has
  information on radio access technology and location information.
  The gstk_address_string is required based on whether the call is
  voice call, SS or USSD.
  There are three optional data fields: subaddress, ccp1 and ccp2 for voice
  call related Call Control envelope command
  This function does not require client ID because it is intended to be called
  by internal modules.

PARAMETERS
  slot:         [Input] Indicates if the client wants to send CC envelope to
                        Slot 1 or Slot 2
  user_data:    [Input] User data that client wants GSTK to keep track of
  cc_ind_param: [Input] Pointer to the call control envelope command info
  gstk_cc_cb:   [Input] Function pointer to which GSTK will send
                        the card response data corresponding to the
                        Call Control command

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_envelope_cc_command_ext (
  gstk_slot_id_enum_type       slot,
  uint32                       user_data,
  const gstk_cc_ind_type_ext*  cc_ind_param,
  gstk_evt_cb_funct_type       gstk_cc_cb
)
{
  gstk_task_cmd_type*     task_cmd = NULL;
  uint8                   i        = 0;

  UIM_MSG_HIGH_1("In gstk_slot_send_envelope_cc_command_ext, slot = 0x%x",
                 slot);

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x", slot);
    return GSTK_BAD_PARAM;
  }

  if(gstk_instances_ptr[(uint32)slot-1] == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* validate call back function */
  if (gstk_cc_cb == NULL || cc_ind_param == NULL)
  {
    UIM_MSG_ERR_0("Null Input Param");
    return GSTK_NULL_INPUT_PARAM;
  }

  /* check for mandatory data */
  switch(cc_ind_param->call_enum_type)
  {
    case GSTK_VOICE:
      if (cc_ind_param->gstk_address_string.voice.address.length == 0)
      {
        UIM_MSG_ERR_0("address len == 0");
        return GSTK_BAD_PARAM;
      }
      break;
    case GSTK_USSD:
      if (cc_ind_param->gstk_address_string.ussd_string.length == 0)
      {
        UIM_MSG_ERR_0("ussd len == 0");
        return GSTK_BAD_PARAM;
      }
      break;
    case GSTK_SS:
      if (cc_ind_param->gstk_address_string.ss_string.length == 0)
      {
        UIM_MSG_ERR_0("ss len == 0");
        return GSTK_BAD_PARAM;
      }
      break;
    case GSTK_PDP_CONTEXT_ACT:
      if (cc_ind_param->gstk_address_string.pdp_context.length <= 0)
      {
        UIM_MSG_ERR_0("Invalid pdp context len");
        return GSTK_BAD_PARAM;
      }
      break;
    case GSTK_EPS_PDN_CONN_ACT:
      if (cc_ind_param->gstk_address_string.pdp_context.length <= 0)
      {
        UIM_MSG_ERR_0("Invalid pdn conn act len");
        return GSTK_BAD_PARAM;
      }
      break;
    case GSTK_IMS_CALL:
      if (cc_ind_param->gstk_address_string.ims_call.length <= 0)
      {
        UIM_MSG_ERR_0("Invalid ims uri len");
        return GSTK_BAD_PARAM;
      }
      break;
    default:
      UIM_MSG_ERR_1("Unknown call enum type 0x%x",
                    cc_ind_param->call_enum_type);
      return GSTK_BAD_PARAM;
  } /* switch(cc_ind_param->call_enum_type) */

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd != NULL)
  {
    task_cmd->cmd.cc_envelope_ind.message_header.client_id     = gstk_instances_ptr[(uint32)slot-1]->client_id;
    task_cmd->cmd.cc_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.cc_envelope_ind.message_header.command_id    = (int)GSTK_CC_IND;
    task_cmd->cmd.cc_envelope_ind.message_header.sim_slot_id   = slot;
    task_cmd->cmd.cc_envelope_ind.message_header.user_data     = user_data;
    task_cmd->cmd.cc_envelope_ind.cc_data.call_enum_type       = cc_ind_param->call_enum_type;
    task_cmd->cmd.cc_envelope_ind.client_cb                    = gstk_cc_cb;
    task_cmd->cmd.cc_envelope_ind.cc_data.rat                  = cc_ind_param->rat;

    UIM_MSG_HIGH_1("RAT passed by client: 0x%x", cc_ind_param->rat);

    /* Copy location information depending on RAT */
    switch (cc_ind_param->rat)
    {
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
      case GSTK_ACCESS_TECH_GSM:
      case GSTK_ACCESS_TECH_UTRAN:
      case GSTK_ACCESS_TECH_LTE:
        task_cmd->cmd.cc_envelope_ind.cc_data.loc_info.gwl_info =
            cc_ind_param->loc_info.gwl_info;
        for (i = 0; i < GSTK_MAX_PLMN_LEN; i++)
        {
          UIM_MSG_HIGH_2("Location information passed by client: mcc_and_mnc [%d]: 0x%x",
                         i, cc_ind_param->loc_info.gwl_info.mcc_and_mnc[i]);
        }
        for (i = 0; i < GSTK_MAX_LAC_LEN; i++)
        {
          UIM_MSG_HIGH_2("loc_area_code [%d]: 0x%x",
                         i, cc_ind_param->loc_info.gwl_info.loc_area_code[i]);
        }
        UIM_MSG_HIGH_1("cell_id_len = %d",
                       cc_ind_param->loc_info.gwl_info.cell_id_len);
        for (i = 0; i < GSTK_MAX_CELL_ID_LEN; i++)
        {
          UIM_MSG_HIGH_2("cell_id [%d]: 0x%x",
                         i, cc_ind_param->loc_info.gwl_info.cell_id[i]);
        }

        if (cc_ind_param->loc_info.gwl_info.cell_id_len > GSTK_MAX_CELL_ID_LEN ||
            cc_ind_param->loc_info.gwl_info.cell_id_len <= 0)
        {
          UIM_MSG_HIGH_1("invalid cell_id_len 0x%x, set cell_id_len to 0",
                         cc_ind_param->loc_info.gwl_info.cell_id_len);
          task_cmd->cmd.cc_envelope_ind.cc_data.loc_info.gwl_info.cell_id_len = 0;
        }
        break;
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA) || defined(FEATURE_LTE)*/
      case GSTK_ACCESS_TECH_HRPD:
      case GSTK_ACCESS_TECH_EHRPD:
        if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, slot) == FALSE)
        {
          return GSTK_BAD_PARAM;
        }
        /* Use location info cache when RAT is HDR */
        UIM_MSG_HIGH_0("Client passes HDR RAT for CC");
        break;
      case GSTK_ACCESS_TECH_CDMA:
#ifdef FEATURE_CDMA
        task_cmd->cmd.cc_envelope_ind.cc_data.loc_info.cdma_info =
            cc_ind_param->loc_info.cdma_info;
        UIM_MSG_HIGH_3("Location information passed by client: MCC: 0x%x, IMSI_11_12: 0x%x, SID: 0x%x",
                       cc_ind_param->loc_info.cdma_info.mcc,
                       cc_ind_param->loc_info.cdma_info.imsi_11_12,
                       cc_ind_param->loc_info.cdma_info.sid);
        UIM_MSG_HIGH_3("NID: 0x%x, BASE ID: 0x%x, BASE LAT: 0x%x",
                       cc_ind_param->loc_info.cdma_info.nid,
                       cc_ind_param->loc_info.cdma_info.base_id,
                       cc_ind_param->loc_info.cdma_info.base_lat);
        UIM_MSG_HIGH_1 ("BASE LONG: 0x%x",
                        cc_ind_param->loc_info.cdma_info.base_long);
#else
        UIM_MSG_ERR_0("FEATURE_CDMA is not enabled");
        gstk_task_free_cmd_buf(task_cmd);
        return GSTK_BAD_PARAM;
#endif /* FEATURE_CDMA */
        break;
      case GSTK_ACCESS_TECH_WLAN:
        /*For WLAN RAT location information is optional */
        break;
      case GSTK_ACCESS_NONE:
        UIM_MSG_ERR_0("Invalid value for RAT");
        gstk_task_free_cmd_buf(task_cmd);
        return GSTK_BAD_PARAM;

      default:
        UIM_MSG_ERR_0("Invalid value for RAT");
        gstk_task_free_cmd_buf(task_cmd);
        return GSTK_BAD_PARAM;
    } /* switch (cc_ind_param->rat) */

    switch(cc_ind_param->call_enum_type)
    {
      case GSTK_VOICE:
        if(gstk_memcpy(
             task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.address.address,
             cc_ind_param->gstk_address_string.voice.address.address,
             cc_ind_param->gstk_address_string.voice.address.length,
             GSTK_MAX_ADDRESS_LEN,
             GSTK_MAX_ADDRESS_LEN) <
           cc_ind_param->gstk_address_string.voice.address.length)
        {
          gstk_task_free_cmd_buf(task_cmd);
          return GSTK_MEMORY_ERROR;
        }
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.address.TON =
          cc_ind_param->gstk_address_string.voice.address.TON;
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.address.NPI =
          cc_ind_param->gstk_address_string.voice.address.NPI;
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.address.length =
          cc_ind_param->gstk_address_string.voice.address.length;
        /* initialize optional ccp1/ccp2 and subaddress */
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp1.length = 0;
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp2.length = 0;
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.subaddress.length = 0;
        /*optional ccp1 */
        if (cc_ind_param->gstk_address_string.voice.ccp1.length > 0)
        {
          if(cc_ind_param->gstk_address_string.voice.ccp1.length > GSTK_MAX_CCP_LEN)
          {
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp1.length =
               GSTK_MAX_CCP_LEN;
          }
          else
          {
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp1.length =
            cc_ind_param->gstk_address_string.voice.ccp1.length;
          }
          if(gstk_memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp1.ccp,
                         cc_ind_param->gstk_address_string.voice.ccp1.ccp,
                         cc_ind_param->gstk_address_string.voice.ccp1.length,
                         GSTK_MAX_CCP_LEN,
                         GSTK_MAX_CCP_LEN) <
             cc_ind_param->gstk_address_string.voice.ccp1.length)
          {
            gstk_task_free_cmd_buf(task_cmd);
            return GSTK_MEMORY_ERROR;
          }
        }

        /*optional ccp2 */
        if (cc_ind_param->gstk_address_string.voice.ccp2.length > 0)
        {
          if(cc_ind_param->gstk_address_string.voice.ccp1.length > GSTK_MAX_CCP_LEN)
          {
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp2.length =
               GSTK_MAX_CCP_LEN;
          }
          else
          {
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp2.length =
            cc_ind_param->gstk_address_string.voice.ccp2.length;
          }
          if(gstk_memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp2.ccp,
                         cc_ind_param->gstk_address_string.voice.ccp2.ccp,
                         cc_ind_param->gstk_address_string.voice.ccp2.length,
                         GSTK_MAX_CCP_LEN,
                         GSTK_MAX_CCP_LEN) <
             cc_ind_param->gstk_address_string.voice.ccp2.length)
          {
            gstk_task_free_cmd_buf(task_cmd);
            return GSTK_MEMORY_ERROR;
          }
        }

        /*optional subaddress */
        if (cc_ind_param->gstk_address_string.voice.subaddress.length > 0)
        {
          task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.subaddress.length =
            cc_ind_param->gstk_address_string.voice.subaddress.length;
          if(gstk_memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.subaddress.subaddress,
                         cc_ind_param->gstk_address_string.voice.subaddress.subaddress,
                         cc_ind_param->gstk_address_string.voice.subaddress.length,
                         GSTK_MAX_SUBADDR_LEN,
                         GSTK_MAX_SUBADDR_LEN) <
             cc_ind_param->gstk_address_string.voice.subaddress.length)
          {
            gstk_task_free_cmd_buf(task_cmd);
            return GSTK_MEMORY_ERROR;
          }
        }
        break;

      case GSTK_USSD:
      {
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text =
          GSTK_CALLOC(cc_ind_param->gstk_address_string.ussd_string.length);
        if (task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text == NULL)
        {
          /* memory_error - put the queue back to the free queue */
          gstk_task_free_cmd_buf(task_cmd);
          return GSTK_MEMORY_ERROR;
        }
        (void)memscpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text,
               cc_ind_param->gstk_address_string.ussd_string.length,
               cc_ind_param->gstk_address_string.ussd_string.text,
               cc_ind_param->gstk_address_string.ussd_string.length);
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.dcs =
          cc_ind_param->gstk_address_string.ussd_string.dcs;
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.length =
          cc_ind_param->gstk_address_string.ussd_string.length;
      }
      break;

      case GSTK_SS:
      {
        if(gstk_memcpy(
           task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ss_string.address,
           cc_ind_param->gstk_address_string.ss_string.address,
           cc_ind_param->gstk_address_string.ss_string.length,
           GSTK_MAX_ADDRESS_LEN,
           GSTK_MAX_ADDRESS_LEN) <
           cc_ind_param->gstk_address_string.ss_string.length)
        {
          gstk_task_free_cmd_buf(task_cmd);
          return GSTK_MEMORY_ERROR;
        }
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ss_string.TON =
          cc_ind_param->gstk_address_string.ss_string.TON;
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ss_string.NPI =
          cc_ind_param->gstk_address_string.ss_string.NPI;
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ss_string.length =
          cc_ind_param->gstk_address_string.ss_string.length;
      }
      break;

      case GSTK_PDP_CONTEXT_ACT:
      {
        if(gstk_memcpy(
             task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.pdp_context.pdp_act_param,
             cc_ind_param->gstk_address_string.pdp_context.pdp_act_param,
             int32touint32(cc_ind_param->gstk_address_string.pdp_context.length),
             GSTK_MAX_PDP_ACT_PARAM_SIZE,
             GSTK_MAX_PDP_ACT_PARAM_SIZE) <
           int32touint32(cc_ind_param->gstk_address_string.pdp_context.length))
        {
          gstk_task_free_cmd_buf(task_cmd);
          return GSTK_MEMORY_ERROR;
        }
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.pdp_context.length =
          cc_ind_param->gstk_address_string.pdp_context.length;
      }
      break;

      case GSTK_EPS_PDN_CONN_ACT:
      {
        if(gstk_memcpy(
             task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.eps_conn_act.eps_act_param,
             cc_ind_param->gstk_address_string.eps_conn_act.eps_act_param,
             int32touint32(cc_ind_param->gstk_address_string.pdp_context.length),
             GSTK_MAX_PDP_ACT_PARAM_SIZE,
             GSTK_MAX_PDP_ACT_PARAM_SIZE) <
           int32touint32(cc_ind_param->gstk_address_string.pdp_context.length))
        {
          gstk_task_free_cmd_buf(task_cmd);
          return GSTK_MEMORY_ERROR;
        }
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.eps_conn_act.length =
          cc_ind_param->gstk_address_string.eps_conn_act.length;
      }
      break;

      case GSTK_IMS_CALL:
      {
        if(gstk_memcpy(
             task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ims_call.ims_uri,
             cc_ind_param->gstk_address_string.ims_call.ims_uri,
             int32touint32(cc_ind_param->gstk_address_string.ims_call.length),
             GSTK_MAX_IMS_CALL_PARAM_SIZE,
             GSTK_MAX_IMS_CALL_PARAM_SIZE) <
           int32touint32(cc_ind_param->gstk_address_string.ims_call.length))
        {
          gstk_task_free_cmd_buf(task_cmd);
          return GSTK_MEMORY_ERROR;
        }
        task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ims_call.length =
          cc_ind_param->gstk_address_string.ims_call.length;
      }
      break;

      default:
        UIM_MSG_ERR_1("Unhandled Call Type:%d", cc_ind_param->call_enum_type);
        break;
    }
    /* set the slot */
    task_cmd->cmd.cc_envelope_ind.slot              = slot;
    task_cmd->cmd.cc_envelope_ind.slot_info_present = TRUE;
    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }
  return GSTK_SUCCESS;
} /* gstk_slot_send_envelope_cc_command_ext */

/*===========================================================================
FUNCTION gstk_otasp_status_reg

DESCRIPTION

  This function is used to register for OTASP status

PARAMETERS
  user_data:       [Input] User data that client wants GSTK to keep track of
  client_reg_cb:   [Input] registration status callback
  otasp_status_cb: [Input] Callback function

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,               - The envelopse command is sent successfully
    GSTK_MEMORY_ERROR,          - Can't allocate memory or envelope command information buffer

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_otasp_status_reg (
  gstk_client_ref_data_type                 user_data,
  gstk_client_otasp_status_reg_cb_func_type client_reg_cb,
  gstk_client_otasp_status_cb_func_type     otasp_status_cb
)
{
  gstk_task_cmd_type            *task_cmd_ptr    = NULL;

  UIM_MSG_HIGH_0("gstk_otasp_status_reg");

  GSTK_RETURN_ERROR_IF_NULL_PTR(otasp_status_cb, GSTK_ERROR);
  GSTK_RETURN_ERROR_IF_NULL_PTR(client_reg_cb, GSTK_ERROR);
  
  /* get the command from buffer */
  task_cmd_ptr = gstk_task_get_cmd_buf();

  if (task_cmd_ptr)
  {
    /* Build GSTK Command */
    task_cmd_ptr->cmd.otasp_reg_cmd.message_header.command_group   = GSTK_OTASP_REG_CMD;
    task_cmd_ptr->cmd.otasp_reg_cmd.user_data                      = user_data;
    task_cmd_ptr->cmd.otasp_reg_cmd.gstk_otasp_status_cb           = otasp_status_cb;
    task_cmd_ptr->cmd.otasp_reg_cmd.gstk_otasp_reg_cb              = client_reg_cb;    
    /* put command back to command queue */
    gstk_task_put_cmd_buf(task_cmd_ptr);
  }
  else
  {
    return GSTK_MEMORY_ERROR;
  }
  return GSTK_SUCCESS;  
}
