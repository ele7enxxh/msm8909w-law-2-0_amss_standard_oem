/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   R E F R E S H   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the processing of all refresh related commands

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2007 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_refresh.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
06/17/16   vdc     Added support for SPN based SIMLOCK feature
06/09/16   ar       Send FDN event source along with FDN event
06/09/16   dd       Refresh of MMGSDI_TELECOM_ADN in UICC mode
05/24/16   ar       Review of macros used by MMGSDI
05/22/16   vdc      Remove F3 messages for memory allocation failure
05/16/16   ar       F3 reduction
05/02/16   sp       Reduce F3 log prints
11/06/15   nr       Skip AID compare for card session in case of FCN REFRESH
11/16/15   nr       Updating fdn status in FCN handling based on ADN activation status
11/06/15   stv      Always send no refresh enforcement to clients
09/25/15   vr       Added support for ERA-GLONASS
09/07/15   ar       Check file path pointer for NULL after memory allocation
06/25/15   ar       Don't allocate memory if the data_len is 0
06/18/15   bcho     Close channel if session is activated on another slot
05/29/15   nr       Fixing memoory leak during card powerdown and task stop
05/27/15   nr       Fix to send refresh event if clients vote false for NAA FCN
05/22/15   nr       Do not add App Reset if 3G session reset is in retry table
05/18/15   nr       Fix to distiguish reads on different apps in single slot
05/15/15   lxu      Fix crash of refresh on EF GBABP
04/30/15   lxu      Support refresh on EF GBABP
04/17/15   nr       Phonebook record read optimization
03/30/15   ar       Support for cross-mapping in multisim_auto_provisioning NV
03/25/15   ar       Ability to process app deact while activation is in progress
01/23/15   nr       updating refresh state correctly
12/09/14   stv      Fix for subscription switch from SIM to USIM
11/24/14   av       Remove KW error to avoid dereferencing null slot_data_ptr
11/23/14   vdc      Init globals when MMGSDI state is NOT_INIT for REFRESH RESET
11/14/14   hh       Fix compilation warnings
11/11/14   vdc      Process REFRESH RESET cmd when MMGSDI state is not init
11/06/14   hh       Call GSTK NV API to read GSTK features in NV 65674
11/03/14   ar       Fix for KW issues
10/31/14   yt       Send END FAILED event for APP_RESET_AUTO (fix merge error)
10/31/14   nr       Skip accessing GSM SPN at all after REFRESH in case of UICC mode
10/23/14   yt       Updates to session and app management
10/10/14   bcho     Send refresh event with END FAILED stage for APP_RESET_AUTO
09/29/14   tkl      Added support for persistent cache
09/09/14   kk       Enhancements in NV handling
09/29/14   ar       Allow powerup if hotswap disable and protocol is unknown
09/22/14   ar       Correct F3 msg for mmgsdi_refresh_start_app_reset_auto()
09/11/14   hh       Call rex_undef_timer() when timer is no longer needed
09/04/14   ar       Remove NVRUIM cache
08/14/14   tl       Add support for silent PIN verification
08/11/14   bcho     Optimize srv req by using cached srv table in client context
08/06/14   ar       Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/28/14   vv       Remove unused function
07/28/14   am       Copy client_spec_files file list pointer also during REFRESH
07/15/14   yt       Support for NV refresh
07/14/14   tl       Remove EF-ECC from call-stack file list
07/08/14   vv       Fix for compiler warnings
06/30/14   vv       Remove Perso code
06/12/14   av       Cache mandatory EFs used by NVRUIM for critical 1x NV reads
06/09/14   df       Wait complete from all sessions before ending non-prov refresh
06/03/14   tl       Remove additional parameter variables
05/30/14   hh       Do not return error if refresh_files.file_list_ptr is NULL
05/27/14   av       Don't allow access if UST/CST can't be read(pin not verified)
05/22/14   hh       Copy refresh info to mmgsdi_refresh_data_info for retries
05/08/14   hh       Correctly handle act_or_switch that results app deactivation
04/18/14   hh       Do not purge EPSLOCI if EPS_MMI is not available in UST
04/01/14   av       NVRUIM headers cleanup
04/01/14   bcho     Do not process Refresh FCN for SIM/RUIM files in UICC mode
03/25/14   yt       Handle refresh of deactivated app
03/18/14   tl       Introduce new SIM Lock feature
03/12/14   yt       Inlcude USIM ACC in the list of call stack files
03/10/14   tl       Remove check for refresh info when activating a prov app
                    as app activation does not trigger refresh.
03/03/14   hh       Purge XXLOCI files by marking certain bytes invalid
02/26/14   ar       Send event to all clients if mode is MMGSDI_REFRESH_NAA_FCN
02/25/14   hh       Purge temporary mobile identities during UICC Reset Refresh
02/25/14   bcho     Added support to store OPLMN List in EFS and cache it
02/20/14   ak       Fix compile errors due to strict compiler on 9x35
02/10/14   ar       Replace session checks with utility functions
12/05/13   tkl      SFI Support and decouple uim file enums
01/14/14   bcho     Change Refresh 3G Session Reset to INIT FCN per ETSI spec
01/11/14   df       Fix off-target compilation error
01/10/14   df       Use v2.0 diag macros
01/09/14   ar       Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/16/13   tl       Prevent initalizing the cache with partial files
12/11/13   df       Remove unused memory paging feature
11/27/13   tl       Introduce refresh command queue
11/22/13   vv       Update REFRESH status as success if CSG feature is disabled
11/18/13   vv       Added support for parallel REFRESH
11/05/13   df       Fix LLVM compiler warning
10/31/13   vv       Fix checking REFRESH status in refresh retry cb
10/27/13   df       Convert use of resp_not_rcvd to uim_rsp_status
10/24/13   av       Fix processing of END_FAILED refresh evt for DEACT and FCN
10/18/13   yt       Clear refresh retry queue for deactivated application
10/10/13   av       Block refresh when onchip is enabled
10/04/13   bcho     F3 message reduction
10/04/13   tl       Leave task crit section before sleep during parallel refresh
09/30/13   tl       Remove delay during refresh
09/26/13   am       Allowing processing of REFRESH GSM OPL file in USIM mode
09/23/13   spo      Fixed clearing of refresh entry in refresh retry table
09/04/13   vv       Update USIM_SPN cache after it is deleted
09/04/13   bcho     Return SUCCESS even if clients vote FALSE for OK TO REFRESH
09/03/13   tl       Reduce On-chip SIM activation delay between stages
08/30/13   tl       Convert channel info table from static to dynamic allocation
07/29/13   am       Added support to determine CSG support dynamically
07/24/13   av       Block writing to the files being refreshed for INIT+FFCN
07/15/13   vs       EONS updates to support single baseband SGLTE
06/24/13   vdc      Added support for triple SIM
06/21/13   bcho     Logic modified to change Refresh mode to INIT/INIT FCN
06/14/13   spo      Fixed size of destination in memscpy
05/28/13   spo      Replaced instances of memcpy with memscpy
05/21/13   vv       Added support for session closed cause
05/17/13   shr      Handle scenario where ongoing Session Provisioning
                    operation is interrupted by a CARD_ERROR on the slot
05/16/13   vdc      Replace memcpy with safer version memscpy
05/15/13   bho      Implementation of REFRESH 3G SESS RESET as in 3GPP TS 31.111
05/14/13   vdc      Memory optimizations for slot, app, pin, upin tables
05/01/13   bcho     For ICC card provide seek direction to search pattern in EF
04/17/13   tl       Replace Featurization with NV
04/16/13   tl       Clean up of radio access features from MMGSDI code
04/15/13   tl       Remove obsoleted featurization
04/04/13   bcho     Deactivate provisioning session if APP RESET ACTIVATE fails
04/02/13   kb       Start REFRESH when no clients are registered for voting
01/28/13   vv       Fix compiler warnings
01/20/13   vv       Added support Refresh FCN support for CSG EFs
12/26/12   av       Do not allow writing to the file that is being refreshed
12/05/12   av       Add LOCI,PS_LOCI & AD as part of USIM callstack files list
10/21/12   av       Added support for new ECC event
10/15/12   bcho     Send EONS event after EONS files refresh only in full srv
10/09/12   tl       Fixed valid session_index check for session_info_ptr array
10/03/12   bcho     Check status of EF-OPL/EF-PNN in svc table before caching
10/01/12   abg      Removed Problem determining protocol type F3 message
09/25/12   av       Skip reading EF-ARR for internal selects
09/03/12   bcho     Support for OPL-PNN records raw caching on Refresh FCN
08/16/12   at       Support for new API mmgsdi_session_register_all_for_refresh
08/08/12   vv       Clear USIM_SPN cache also in case card incorrectly
                    refreshes GSM_SPN file
07/26/12   tl       Fix num_file_paths update and increase type size for for loop
07/25/12   bcho     Set channel status to NONE while starting app reset process
07/19/12   tl       Support more application and logical channels
04/26/12   av       Added missing files that are used by DS to the list of
                    call-stack files
04/20/12   av       Enabled detouring of nv/efs calls to offtarget framework
04/16/12   tl       Added traces displaying client name
03/29/12   shr      Added support for non-prov. app selection using zero
                    length and partial AID
03/28/12   bcho     rex_sleep() is replaced with sleep mechanism based on
                    non-deferrable timer
01/30/12   shr      Maintain flag to indicate last successful OTASP Commit,
                    use GSTK NV item (65674) to determine if the OTASP IMSI_M
                    enhancement is enabled or not
01/30/12   bcho     Refresh handling for EONS files modified
01/25/12   kk       Fixed KW error
12/23/11   shr      Removed featurization for ALWAYS ON features
12/21/11   bcho     Legacy GSDI removal updates
12/21/11   kk       Added USIM ECC caching support
12/21/11   bcho     Added support for Get Operator Name API
11/29/11   ssr      Used UIM API to check the hotswap capability of given slot
11/09/11   nb       Handle Timer expiry in OK_TO_PROCEED state
10/28/11   yt       Added new files for cache-related utility functions
10/27/11   shr      Fix featurization of USIM look-up table utility
                    invocation
10/13/11   nb       Support for new Refresh stage
10/04/11   nb       Fix for compilation error
10/03/11   nb       Check slot information as well when checking for steering
                    responses
09/16/11   av       Replace feature #ifdefs with an NVITEM
09/07/11   ssr      Fix to disable the protocol check when HOTSWAP is enable
09/06/11   shr      Do not attempt a Card Reset as part of EFRESH RESET/Card
                    power-up handling, if the REFRESH has been stopped
08/19/11   vs       Reject INIT based REFRESH if PIN is blocked or unverified
08/01/11   vs       Added support for session open with select response
07/22/11   vv       Fixed handling the NULL session event callback
07/18/11   nmb      Fallback to RUIM in case of CSIM missing mandatory files
05/17/11   shr      Updates for Thread safety
02/24/11   nmb      Return additional information for refresh voting failure
02/23/11   nmb      Refresh Retry enhancements
02/18/11   yt       Corrected trace for mode change from INIT+FCN to INIT
02/18/11   yt       Check if CSIM is provisioned before starting Refresh voting
02/18/11   ea       Fix compiler warnings
01/21/11   nmb      Added refresh retry support
10/01/11   shr      Need to send correct response status for REFRESH FCN
                    requested on an inactive app
12/22/10   yt       Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/14/10   js       Fix for init refresh failure for non-prov app
11/09/10   kk       Fixed resetting of fil_path_len to 0 after a refresh
                    event is sent to first client.
11/05/10   yt       Klocwork fixes
10/20/10   yt       KW fixes
08/18/10   js       Fix refresh registration for legacy clients with non-prov
                    sessions
10/13/10   nb       Fix for steering of roaming
10/06/10   shr      Fixed building of REFRESH file lists while handling
                    REFRESH FCN and REFRESH INIT FCN
10/06/10   shr      Fixed calculation of number of duplicates when handling
                    additional REFRESH registrations by path
09/15/10   nmb      Allocate mmgsdi_client_id_reg_table dynamically
08/30/10   shr      Perso clean up for Sessions
08/24/10   shr      Do not send out multiple CARD_ERROR events for a slot
08/18/10   shr      Igonre COMPLETE INIT indications from clients other
                    than MMGSDI for REFRESH RESET and RESET_AUTO
08/12/10   ssr      Fixed nvruim 1x primary session for slot2
08/10/10   adp      When we check the lookup table, check for the exact length
                    of the path.
07/21/10   adp      Check for uim file in the look up table first
07/09/10   ssr      Added CDMA refresh feature for CDMA refresh  files
07/02/10   ssr      Fixed refresh for ICC card which have both GSM and RUIM
06/29/10   shr      Card Sessions need to get FCN notification for non call
                    stack files in the FCN list of a REFRESH INIT FCN
06/28/10   shr      Added support for Hidden key event
06/24/10   yt       Klocwork fixes
06/10/10   shr      Need to add Sleep before requeuing REFRESH/Card power-up
                    requests
06/06/10   kk       Fixed uim get enum from path util function
06/01/10   shr      Send STATUS command indicating app termination to card
                    before initiating card RESET
06/01/10   js       ZI memory reduction fixes
05/26/10   yt       Lint fixes
05/13/10   adp      Fixing compiler warnings
05/10/10   kk       Fixed start of complete init timer until MMGSDI completes
05/06/10   vs       Fixed inifinite loop in FCN Refresh
05/06/10   adp      Reduced stack usage moving large buffers into heap
05/03/10   shr      Added Dual Slot support for REFRESH RESET and REFRESH
                    RESET_AUTO
04/23/10   shr      All files in the cache must be in the REFRESH list
04/23/10   kk       Fixed End Success event handling for INIT_FCN mode
04/22/10   js       Populate app type for refresh request
04/06/10   shr      Clean up REFRESH RESET and REFRESH RESET_AUTO
04/02/10   js       Populate app type for refresh request
03/30/10   kk       Fixed client based refresh registeration
03/29/10   shr      Switch/deactivate provisioning clean up and redesign
03/26/10   js       Fixed null ptr checks
03/09/10   shr      Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
02/09/10   vs       Fixed refresh to accomodate perso event
01/08/10   shr      Clear NVRUIM cache only if REFRESH is for the 1X Primary
                    provisioning session
01/12/10   nb       Fixed Compilation warning
01/05/10   mib      Fixed return value when no client is registered
12/18/09   nb       Moved Dual Slot Changes
12/04/09   rn       Added support for card reset during normal power down
12/03/09   shr      Cache read process enhancement
10/28/09   nb       Release 7 Enhanced Netwrok Selection
10/22/09   mib      Print of refresh callback address in logs
10/21/09   js       Fixed issue for no card error evt for card power up
                    request under no sim condition
10/12/09   kp       Moving Read-Write varibles to Heap
10/12/09   nb       Started the refresh timer when MMGSDI has finished its
                    initialisation for refresh modes requiring INIT
09/26/09   kp       ZI memory reduction changes
08/13/09   js       Clean up app pin info for mmgsdi card pup/pdown request
08/10/09   shr      Fixed REFRESH FCN issue - erroneous removal
                    of files from client list
08/10/09   shr      Send correct response status for FCN request on inactive app
08/05/09   js       Clean up app pin info for mmgsdi card pup/pdown request
07/27/09   kk       Fixed Lint Error - Boolean within 'if' always evaluates
                    to False
06/02/09   kk       Fixed compilation error - inclusion of nvruimi_v.h
04/29/09   js       Added prototype for static function, fixed compiler warnings
03/30/09   xz       Change mmgsdi_refresh_rsp_send_cnf_to_gstk() return type
03/19/09   js       Support for Enhanced NAA Refresh by file path
02/23/09   mib      Changed behaviour for 3G Session Reset to avoid
                    terminating the application. Added original mode in events
02/16/09   kk       INIT_FFCN mode sent to clients, to enable re-read all the
                    cached items that are not part of re-init
02/12/09   kk       Add files to refresh file list for re-reading during FCN
02/09/09   ssr      Clear Refresh timer as soon all refresh responses are received
01/28/09   nb       Fixed Klockwork Error
12/05/08   jar      Fixed a remaining compilation error.
12/02/08   ssr      Removed Compilation Error
12/01/08   ssr      Handled error condition for some MMGSDI Functions
11/18/08   kk       Added extra logs for better logging of refresh events
09/22/08   js       Fixed Lint Errors
08/28/08   sun      If command fails in the original state, then return original
                    error mmgsdi status to client
08/14/08   js       Fixed compiler warnings for unused variable
06/23/08   sp       Update the gsdi_util_preinit_gsdi function argument
                    in mmgsdi_process_refresh_evt.
06/07/08   sun      Updated additional file flag even if the reads were
                    happening via old GSDI
05/26/08   kk       Fixed compilation warning
05/23/08   kk       Fixed Data Assertion case during Refresh - FCN
04/08/08   nk       Added CDMA Refresh Support
03/24/08   sun      Fixed memory overwrite
03/19/08   nk       Fixed Klocwork issues
03/17/08   sun      Fixed the order of refresh - first handle the request type
                    and then handle the state
02/28/08   sun      New state is only relevant for Reset Mode, do not set it
                    for FCN.
02/26/08   sun      Fixed warning
02/21/08   sun      Read CPHS files on Refresh
02/13/08   kk       Removed an incorrect check for a NULL ptr in function
                    mmgsdi_refresh_req_process_dereg
01/29/08   sun      Handle REFRESH_AUTO same as a REFRESH_RESET mode
01/25/08   tml      Fixed null pointer access in case of onchip
01/11/08   jar      Handle MMGSDI_REFRESH_ORIG_PUP_REQ in mmgsdi_refresh_main
                    for MMGSDI_REFRESH_READY_STATE
12/11/07   js       Fixed RAT FCN support
12/10/07   sun      Fixed sending of events, fixed FCN list check
12/07/07   sun      Fixed Memcpy
12/05/07   sun      Mainlined mmgsdi_session_build_status_terminate so that
                    it canbe used for 2G reset as well
12/04/07   sun      Fixed compile error
12/02/07   sun      Fixed compile error
11/12/07   sun      Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "mmgsdi_icc.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_refresh_rsp.h"
#include "mmgsdi_session.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_cphs.h"
#include "nvruim_p.h"
#include "mmgsdisessionlib_v.h"
#include "mmgsdi_session.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_gen.h"
#include "fs_lib.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdicache.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_csg.h"
#include "mmgsdi_eons_opl_pnn.h"
#include "mmgsdi_file.h"
#include "mmgsdi_builtin_plmn.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_pb_cache.h"
#include "mmgsdi_silent_pin.h"
#include "uim_common_efs.h"
#include "gstk_nv_p.h"
#include "mmgsdi_multi_profile.h"
#ifdef FEATURE_SIMLOCK
#include "mmgsdi_simlock.h"
#endif /*FEATURE_SIMLOCK*/

#ifdef FEATURE_UIM_PERSISTENT_CACHE
#include "mmgsdi_persistent_cache.h"
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define MMGSDI_REFRESH_TIMER_COUNT_SHIFT         16
#define MMGSDI_REFRESH_TIMER_EXPIRY_SLOT_ID_MASK 0xFFFF
#define MMGSDI_REFRESH_MIN_PATH_LEN              2

/* ----------------------------------------------------------------------------
  LOCI file Location Update Status
-----------------------------------------------------------------------------*/
#define MMGSDI_LOCI_UPDATED                      0x00
#define MMGSDI_LOCI_NOT_UPDATED                  0x01

static mmgsdi_return_enum_type mmgsdi_refresh_process_fcn_hiddenkey(
  const mmgsdi_event_data_type *event_ptr,
  mmgsdi_file_enum_type         file);

static void mmgsdi_refresh_auto_reset_timer_expiry (
  unsigned long ptr
);

static mmgsdi_return_enum_type mmgsdi_refresh_handle_refresh_start_steering(
  const mmgsdi_event_data_type  * event_ptr
);

static mmgsdi_return_enum_type mmgsdi_refresh_build_node_list(
  const mmgsdi_session_info_type               *session_ptr,
  const mmgsdi_internal_refresh_file_list_type *refresh_files_ptr
);

static void mmgsdi_refresh_retry_handle_req(
  void
);

static mmgsdi_return_enum_type mmgsdi_refresh_retry_consolidate_table(
  uint8 slot_index
);

static void mmgsdi_refresh_retry_clear_table_index(
  uint32 retry_index
);

static mmgsdi_return_enum_type mmgsdi_refresh_retry_add_entry(
  uint8 slot_index
);

static void mmgsdi_refresh_retry_clear_fcn_entry(
  uint32  retry_index,
  uint32 *retries_ptr
);

static mmgsdi_return_enum_type mmgsdi_refresh_retry_get_index(
  mmgsdi_refresh_mode_enum_type  refresh_mode,
  uint32                        *retry_index_ptr
);

static mmgsdi_return_enum_type mmgsdi_refresh_cache_opl_pnn(
  mmgsdi_session_id_type  session_id,
  mmgsdi_client_id_type   client_id,
  mmgsdi_file_enum_type   file_enum);

static void mmgsdi_internal_refresh_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

static mmgsdi_return_enum_type mmgsdi_refresh_process_init_fcn(
  mmgsdi_slot_id_enum_type             slot_id,
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr
);

static void mmgsdi_refresh_check_and_purge_temp_identities(
  mmgsdi_slot_id_enum_type    slot_id,
  mmgsdi_protocol_enum_type   protocol
);

static mmgsdi_refresh_retry_table_entry_type *mmgsdi_refresh_retry_table[MMGSDI_REFRESH_RETRY_TABLE_SIZE];
static int mmgsdi_refresh_retry_table_index = 0;

/*=========================================================================
  Refresh Specific Utility Functions
===========================================================================*/

/*===========================================================================
FUNCTION MMGSDI_REFRESH_UPDATE_CARD_STATE

DESCRIPTION
  This function updates the card state specifically during REFRESH
  RESET/RESET AUTO handling

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_refresh_update_card_state(
  mmgsdi_slot_id_enum_type slot_id)
{
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return;
  }

 /* --------------------------------------------------------------------------
    If card state is already in NO_CARD, do not update card state.
    This is to ensure that if the card reset fails (leading to an error
    indication from UIM), this is not propagated to MMGSDI clients.

    But if card state is CARD_INSERTED when the card reset is attempted,
    then any reset failure should be indicated to clients through a CARD_ERROR
    event. Moving card state to NOT_INIT ensures this will happen.
    ------------------------------------------------------------------------*/
  if(slot_data_ptr->mmgsdi_state == MMGSDI_STATE_CARD_INSERTED)
  {
    slot_data_ptr->mmgsdi_state = MMGSDI_STATE_NOT_INIT;
  }
} /* mmgsdi_refresh_update_card_state */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_PROCESS_FCN_HIDDENKEY

  DESCRIPTION
    This function will handle the Refresh FCN of USIM/CSIM Hiddenkey

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    Hiddenkey Event will be sent to clients
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_fcn_hiddenkey(
  const mmgsdi_event_data_type *event_ptr,
  mmgsdi_file_enum_type         file)
{
  mmgsdi_int_app_info_type       *app_info_ptr  = NULL;
  mmgsdi_evt_session_notify_type  notify_type;
  mmgsdi_data_type                hkey_data;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL(event_ptr);

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));
  memset(&hkey_data, 0x00, sizeof(mmgsdi_data_type));

  /* Get the app related to the session */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(event_ptr->session_id,
                                                    NULL,
                                                    &notify_type.slot_id,
                                                    &app_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Could not get Prov app info");
    return MMGSDI_SUCCESS;
  }

  /* Recache Hiddenkey data */
  mmgsdi_status = mmgsdi_card_init_cache_binary(
                    event_ptr->session_id,
                    event_ptr->data.refresh.aid.app_type,
                    event_ptr->data.refresh.slot,
                    file,
                    &hkey_data);

  /* Clear the Hiddenkey status */
  app_info_ptr->hiddenkey.status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Verify if the Hiddenkey data is valid */
    if(mmgsdi_util_is_hiddenkey_from_card_valid(&hkey_data))
    {
      app_info_ptr->hiddenkey.status = MMGSDI_PIN_ENABLED_NOT_VERIFIED;
    }
    else
    {
      app_info_ptr->hiddenkey.status = MMGSDI_PIN_DISABLED;
    }
  }
  else if(mmgsdi_status == MMGSDI_NOT_FOUND)
  {
    UIM_MSG_HIGH_1("Optional Hiddenkey not found: status 0x%x", mmgsdi_status);
    app_info_ptr->hiddenkey.status = MMGSDI_PIN_NOT_FOUND;
  }

  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.session_id  = event_ptr->session_id;

  /* Send out the Hiddenkey event */
  mmgsdi_evt_build_and_send_pin(notify_type,
                                MMGSDI_HIDDENKEY,
                                app_info_ptr->hiddenkey.status,
                                (const mmgsdi_int_app_info_type*)app_info_ptr);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hkey_data.data_ptr);

  return mmgsdi_status;
} /* mmgsdi_refresh_process_fcn_hiddenkey */


/* ==========================================================================
  FUNCTION MMGSDI_REFRESH_QUEUE_SELECT_AID_REQ

  DESCRIPTION
    This function queue SELCT AID request when internal refresh call back
    function recevies cnf for CARD_STATUS_REQ and APP_RESET_DEACTIVATE_REQ

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_queue_select_aid_req(
  mmgsdi_aid_type          app_id,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  mmgsdi_card_refresh_extra_info_type  extra_info;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_client_id_type                client_id       = MMGSDI_CLIENT_ID_ZERO;
  mmgsdi_session_id_type               session_id      = MMGSDI_SESSION_ID_ZERO;
  mmgsdi_slot_id_enum_type             slot_id         = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_client_data_type              client_data     = 0;
  uint8                                slot_index      = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_0("mmgsdi_refresh_queue_select_aid_req");

  MMGSDIUTIL_RETURN_IF_NULL(cnf_ptr);

  memset(&extra_info, 0x00, sizeof(mmgsdi_card_refresh_extra_info_type));

  switch(cnf)
  {
    case MMGSDI_CARD_STATUS_CNF:
      client_id = cnf_ptr->status_cnf.response_header.client_id;
      session_id = cnf_ptr->status_cnf.response_header.session_id;
      slot_id = cnf_ptr->status_cnf.response_header.slot_id;
      client_data = cnf_ptr->status_cnf.response_header.client_data;
      break;
    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_CNF:
      client_id = cnf_ptr->session_app_reset_deactivate_cnf.response_header.client_id;
      session_id = cnf_ptr->session_app_reset_deactivate_cnf.response_header.session_id;
      slot_id = cnf_ptr->session_app_reset_deactivate_cnf.response_header.slot_id;
      client_data = cnf_ptr->session_app_reset_deactivate_cnf.response_header.client_data;
      break;
    default:
      return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  extra_info.mode     = mmgsdi_refresh_info_ptr[slot_index]->mode;
  extra_info.send_evt = TRUE;
  extra_info.stage    = MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED;

  (void)mmgsdi_refresh_send_evt(&extra_info,
                                &mmgsdi_refresh_info_ptr[slot_index]->request_header);

  /* Activate the application */
  mmgsdi_status = mmgsdi_session_build_select_aid(client_id,
                    session_id,
                    slot_id,
                    MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ,
                    MMGSDI_SESSION_SELECT_AID_REQ,
                    app_id.aid,
                    FALSE,
                    MMGSDI_SELECT_RSP_FCP,
                    mmgsdi_internal_refresh_cb,
                    client_data);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Refresh: Unable to build APP_RESET_ACTIVATE command 0x%x",
                  mmgsdi_status);
  }
  return mmgsdi_status;
}/* mmgsdi_refresh_queue_select_aid_req */


/* ==========================================================================
  FUNCTION MMGSDI_INTERNAL_REFRESH_CB

  DESCRIPTION
    This function, defined as mmgsdi_callback_type, will handle
    MMGSDI_DEACTIVATE CNF and MMGSDI_ACTIVATE_CNF cnfs generated by MMGSDI

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          - Command processed,
    MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
    MMGSDI_ERROR            - Other unknown failure

  SIDE EFFECTS
    None
=============================================================================*/
static void mmgsdi_internal_refresh_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_card_refresh_extra_info_type  extra_info;
  mmgsdi_evt_session_notify_type       notify_type;
  mmgsdi_int_app_info_type           * app_info_ptr    = NULL;
  mmgsdi_slot_id_enum_type             slot            = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                                slot_index      = MMGSDI_SLOT_1_INDEX;

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));
  memset(&extra_info, 0x00, sizeof(mmgsdi_card_refresh_extra_info_type));

  if (cnf_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_3("mmgsdi_internal_refresh_cb, slot: 0x%x, cnf_type: 0x%x, status: 0x%x",
                 cnf_ptr->response_header.slot_id, cnf, status);

  mmgsdi_status = mmgsdi_util_get_slot_index(
                        cnf_ptr->response_header.slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    UIM_MSG_ERR_1("Invalid slot index, 0x%x", slot_index);
    return;
  }

  if(mmgsdi_refresh_info_ptr[slot_index] == NULL)
  {
    UIM_MSG_ERR_0("Refresh:  Invalid REFRESH data, nothing can be done");
    return;
  }

  extra_info.mode = mmgsdi_refresh_info_ptr[slot_index]->mode;

  if (status != MMGSDI_SUCCESS)
  {
    /*Build the Confirmation only when we get to one of these stages*/

    /* If app activation failed */
    if(cnf == MMGSDI_SESSION_APP_RESET_ACTIVATE_CNF)
    {
      /* Post deactivate session request with internal_deact_req as TRUE */
      (void)mmgsdi_session_create_and_queue_internal_deactivate_req(
              cnf_ptr->session_app_reset_activate_cnf.response_header.session_id);
    }
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;

    /* Send END_FAILED to the clients */
    extra_info.send_evt = TRUE;
    extra_info.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    (void)mmgsdi_refresh_send_evt(&extra_info,
                                  &mmgsdi_refresh_info_ptr[slot_index]->request_header);
    (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
            &mmgsdi_refresh_info_ptr[slot_index]->request_header);
    return;
  }

  switch(cnf)
  {
    case MMGSDI_CARD_STATUS_CNF:

      /*  CARD STATUS CNF only to be received for 3G Session Reset refresh mode */
      if(mmgsdi_refresh_info_ptr[slot_index]->mode != MMGSDI_REFRESH_3G_SESSION_RESET)
      {
        UIM_MSG_ERR_1("CARD STATUS CNF received for invalid Refresh Mode = 0x%x",
                      mmgsdi_refresh_info_ptr[slot_index]->mode);
        break;
      }

      /* If refresh mode is 3G session reset, select the app */
      status = mmgsdi_util_get_session_app_info(
                 cnf_ptr->status_cnf.response_header.session_id,
                 &slot,
                 NULL,
                 &app_info_ptr,
                 NULL,
                 NULL,
                 NULL);

      if((status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
      {
        UIM_MSG_ERR_1("Invalid Prov. session - Session ID=0x%x",
                      cnf_ptr->status_cnf.response_header.session_id);
         break;
      }

      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

      /* Cleanup PIN info connected to the app */
      mmgsdi_util_app_pin_cleanup(app_info_ptr,
                                  slot);

      /* Reset app and Perso states */
      if(app_info_ptr->app_state > MMGSDI_APP_STATE_UNKNOWN)
      {
        app_info_ptr->app_state =
          MMGSDI_APP_STATE_DETECTED;
      }

      /* Send session changed event with app selected as FALSE */
      app_info_ptr->app_selected = FALSE;
      app_info_ptr->perso_state = MMGSDI_SIMLOCK_NOT_VERIFIED;

      (void)mmgsdi_util_clear_app_capabilities(&app_info_ptr->app_capabilities);

      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

      notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
      notify_type.slot_id     = slot;

      mmgsdi_evt_build_and_send_session_changed(
        notify_type, FALSE, (const mmgsdi_int_app_info_type*)app_info_ptr);

      mmgsdi_status = mmgsdi_refresh_queue_select_aid_req(app_info_ptr->app_data,
                                                          cnf,
                                                          cnf_ptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        /*Build the Confirmation only when we get to one of these stages*/
        mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;

        /* Send END_FAILED to the clients */
        extra_info.send_evt = TRUE;
        extra_info.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
        (void)mmgsdi_refresh_send_evt(&extra_info,
                                      &mmgsdi_refresh_info_ptr[slot_index]->request_header);
        (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
                &mmgsdi_refresh_info_ptr[slot_index]->request_header);
      }
      break;

    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_CNF:
      mmgsdi_status = mmgsdi_refresh_queue_select_aid_req(
                        cnf_ptr->session_app_reset_deactivate_cnf.app_id,
                        cnf,
                        cnf_ptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        /*Build the Confirmation only when we get to one of these stages*/
        mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;

        /* Send END_FAILED to the clients */
        extra_info.send_evt  = TRUE;
        extra_info.stage     = MMGSDI_REFRESH_STAGE_END_FAILED;
        (void)mmgsdi_refresh_send_evt(&extra_info,
                                      &mmgsdi_refresh_info_ptr[slot_index]->request_header);
        (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
                &mmgsdi_refresh_info_ptr[slot_index]->request_header);
      }
      break;

    case MMGSDI_SESSION_APP_RESET_ACTIVATE_CNF:
      extra_info.send_evt = TRUE;
      extra_info.stage    =
          MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED;

      (void) mmgsdi_refresh_send_evt(&extra_info,
                                     &mmgsdi_refresh_info_ptr[slot_index]->request_header);

      extra_info.send_evt = TRUE;
      extra_info.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_info.app_type = MMGSDI_REFRESH_START_NON_PROV_APP;

      (void)mmgsdi_refresh_send_evt(&extra_info,
                                    &mmgsdi_refresh_info_ptr[slot_index]->request_header);
      /* pending for client to finish initialization before sending TR */
      break;
    default:
      break;
  }
}/*mmgsdi_internal_refresh_cb*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RESET_NOTIFY_INIT

  DESCRIPTION
    This function resets all notify_init flags of all clients
    related to app that is being refreshed

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void mmgsdi_refresh_reset_notify_init(uint8 slot_index)
{
  mmgsdi_refresh_node_type     *node_ptr      = NULL;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_info_type     *session_ptr   = NULL;

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return;
  }
  if(mmgsdi_refresh_info_ptr[slot_index] == NULL)
  {
    /*Nothing to do*/
    UIM_MSG_ERR_0("No Refresh in progress, cannot reset init flag");
    return;
  }

  node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;

  while(node_ptr != NULL)
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                                    node_ptr->session_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &session_ptr,
                                    NULL);

    if(mmgsdi_status     == MMGSDI_SUCCESS &&
       session_ptr      != NULL)
    {
      session_ptr->init_complete = FALSE;
    }
    node_ptr = node_ptr->next_ptr;
  }
}/*mmgsdi_refresh_reset_notify_init*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_MALLOC_AND_COPY_FILE_LIST

  DESCRIPTION
    This function mallocs the file list and then copies the file list to the
    global mmgsdi_refresh_info_ptr

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS                     -   In case of successful execution
    MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED -   If there is error in heap memory
                                           allocation
    MMGSDI_ERROR                       -   For all other error cases

  SIDE EFFECTS
    None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_malloc_and_copy_file_list(
  mmgsdi_internal_refresh_file_list_type  refresh_files,
  uint8                                   slot_index)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  /*----------------------------------------------------------------
  Allocate memory for the File List
  ----------------------------------------------------------------*/
  if(refresh_files.file_list_ptr != NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.file_list_ptr,
      (sizeof(mmgsdi_file_enum_type) * refresh_files.num_files));

    if(mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.file_list_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    /*----------------------------------------------------------------
      Copy the Files over
    ----------------------------------------------------------------*/
    mmgsdi_memscpy(mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.file_list_ptr,
      refresh_files.num_files * sizeof(mmgsdi_file_enum_type),
      refresh_files.file_list_ptr,
      refresh_files.num_files *sizeof(mmgsdi_file_enum_type));

    mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_files =
      refresh_files.num_files;
  }
  else
  {
    mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_files = 0;
  }

  if(refresh_files.file_path_ptr != NULL)
  {
    /* Copy file path array */
    mmgsdi_status =
      mmgsdi_util_malloc_and_copy_file_path_array(
        &(mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.file_path_ptr),
        refresh_files.file_path_ptr,
        refresh_files.num_file_paths);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_file_paths = 0;
      return mmgsdi_status;
    }
    else
    {
      mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_file_paths =
        refresh_files.num_file_paths;
    }
  }
  else
  {
    mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_file_paths = 0;
  }

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_malloc_and_copy_file_list*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_AUTO_RESET_TIMER_EXPIRY

  DESCRIPTION
    This function builds a response if an external module that MMGSDI is
    dependent on does not respond. This is only applicable to refresh
    requests generated by mmgsdi itself rather than an actual proactive
    command

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
static void mmgsdi_refresh_auto_reset_timer_expiry (unsigned long slot)
{
  mmgsdi_slot_id_enum_type      slot_id = (mmgsdi_slot_id_enum_type)slot;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type       req           = {{0}};

  UIM_MSG_HIGH_1("mmgsdi_refresh_auto_reset_timer_expiry on slot: 0x%x", slot_id);

  if(mmgsdi_generic_data_ptr == NULL)
  {
    return;
  }

  req.refresh_req_type = MMGSDI_REFRESH_ORIG_REFRESH_REQ;
  memset(&req.refresh.card_req.aid, 0x00, sizeof(mmgsdi_aid_type));
  memset(&req.refresh.card_req.refresh_files,
          0x00,
          sizeof(mmgsdi_data_type));

  req.refresh.card_req.refresh_mode = MMGSDI_REFRESH_RESET_AUTO;
  req.refresh.card_req.num_files    = 0;
  req.refresh.card_req.source       = MMGSDI_REFRESH_SOURCE_MMGSDI;

  mmgsdi_status = mmgsdi_client_build_refresh_req(
                    mmgsdi_generic_data_ptr->client_id,
                    slot_id,
                    &req,
                    FALSE,
                    mmgsdi_util_internal_cb,
                    0,
                    FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Refresh:  Cannot build new request");
  }
}/*mmgsdi_refresh_auto_reset_timer_expiry*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_TIMER_EXPIRY

  DESCRIPTION
    This function builds a command to MMGSDI indicating Timer expiry if an
    external module that MMGSDI is dependent on does not respond

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_refresh_timer_expiry (unsigned long timer_count_slot)
{
  mmgsdi_slot_id_enum_type      slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;
  uint16                        timer_count   = 0;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type       req           = {{0}};

  UIM_MSG_HIGH_0("FUNCTION mmgsdi_refresh_timer_expiry");

  if(mmgsdi_generic_data_ptr == NULL)
  {
    return;
  }

  /* Get the slot ID and Timer count corresponding to the
     current timer expiry */
  slot_id = (mmgsdi_slot_id_enum_type)(timer_count_slot &
                                       MMGSDI_REFRESH_TIMER_EXPIRY_SLOT_ID_MASK);

  timer_count = (uint16)(timer_count_slot >> MMGSDI_REFRESH_TIMER_COUNT_SHIFT);

  memset(&req, 0x00, sizeof(mmgsdi_refresh_req_type));

  /* Update the REFRESH request */
  req.refresh_req_type = MMGSDI_REFRESH_ORIG_TIMER_EXPIRY_REQ;
  req.refresh.timer_count = timer_count;

  /* Build a timer expiry request to MMGSDI */
  mmgsdi_status = mmgsdi_client_build_refresh_req(
                    mmgsdi_generic_data_ptr->client_id,
                    slot_id,
                    &req,
                    FALSE,
                    mmgsdi_util_internal_cb,
                    0,
                    FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Refresh: Cannot build Timer expiry request");
  }
}/*mmgsdi_refresh_timer_expiry*/


#ifndef FEATURE_UIM_TEST_FRAMEWORK
/*===========================================================================
  FUNCTION MMGSDI_REFRESH_START_TIMER

  DESCRIPTION
    This function starts the timer based on the stage

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
static void mmgsdi_refresh_start_timer(
  mmgsdi_slot_data_type *slot_ptr,
  uint8                  slot_index)
{
  rex_timer_cnt_type timer            = 0;
  uint32             timer_count_slot = 0;

  if(slot_ptr == NULL)
  {
    return;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return;
  }

  /*Start timer and Wait for Processes to Respond*/
  (void)rex_undef_timer(&mmgsdi_refresh_timer[slot_index]);

  if(mmgsdi_refresh_info_ptr[slot_index] == NULL)
  {
    UIM_MSG_ERR_0("No Refresh in progress, cannot start timer");
    return;
  }

  /* Increment the per slot timer count */
  slot_ptr->refresh_timer_count++;

  /* Build the timer count. slot composite value to passed to
     the timer define API */
  timer_count_slot = ((uint32)slot_ptr->refresh_timer_count <<
                      MMGSDI_REFRESH_TIMER_COUNT_SHIFT) |
                     ((uint32)mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id &
                      MMGSDI_REFRESH_TIMER_EXPIRY_SLOT_ID_MASK);

  rex_def_timer_ex(&mmgsdi_refresh_timer[slot_index],
                   mmgsdi_refresh_timer_expiry,
                   timer_count_slot);

  switch(slot_ptr->refresh_state)
  {
    case MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE:
      timer =  MMGSDI_REFRESH_OK_TO_INIT_TIMER;
      break;
    case MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE:
      timer = MMGSDI_REFRESH_OK_TO_FCN_TIMER;
      break;
    case MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE:
      timer = MMGSDI_REFRESH_INIT_COMPLETE_TIMER;
      break;
    case MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE:
      timer = MMGSDI_REFRESH_FCN_COMPLETE_TIMER;
      break;
    case MMGSDI_REFRESH_READY_STATE:
      /*Nothing to do*/
      return;
    case MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE:
      timer = MMGSDI_REFRESH_OK_TO_PROCEED_WITH_REFRESH_TIMER;
      break;
    default:
      UIM_MSG_ERR_1("Refresh:  Invalid State 0x%x", slot_ptr->refresh_state);
      return;
  }

  (void)rex_set_timer(&mmgsdi_refresh_timer[slot_index], timer);
}/*mmgsdi_refresh_start_timer*/
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_SEND_EVT_TO_SPECIFIC_SESSIONS

  DESCRIPTION:
    This function will build the refresh event specific to every session
    and send the event to that session.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The funtion was successful in getting the file
                             attributes of a particular file.
    MMGSDI_ERROR:            Could not send event to client(s)
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_send_evt_to_specific_sessions(
  const mmgsdi_card_refresh_extra_info_type *extra_info_ptr,
  const mmgsdi_request_header_type          *request_hdr_ptr)
{
  mmgsdi_refresh_node_type *node_ptr      = NULL;
  mmgsdi_event_data_type   *evt_info_ptr  = NULL;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  uint32                    j,index       = 0;
  int32                     i             = 0;
  uint8                     slot_index    = MMGSDI_SLOT_1_INDEX;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL_2(extra_info_ptr,
                              request_hdr_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(request_hdr_ptr->slot_id, &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_refresh_info_ptr[slot_index],
                              mmgsdi_refresh_info_ptr[slot_index]->node_ptr);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));

  if(evt_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
  while(node_ptr)
  {
    /*The Stage could be different for eg. for NAA_INIT_FCN,
      some nodes may not need to do OK_TO_FCN*/
    if(node_ptr->rsp_received == FALSE &&
       node_ptr->stage == extra_info_ptr->stage)
    {
      mmgsdi_memscpy(&evt_info_ptr->data.refresh.aid,
                     sizeof(evt_info_ptr->data.refresh.aid),
                     &mmgsdi_refresh_info_ptr[slot_index]->aid,
                     sizeof(mmgsdi_aid_type));

      mmgsdi_memscpy(&evt_info_ptr->data.refresh.app_info.app_data,
                     sizeof(evt_info_ptr->data.refresh.app_info.app_data),
                     &mmgsdi_refresh_info_ptr[slot_index]->aid,
                     sizeof(mmgsdi_aid_type));

      evt_info_ptr->evt                                                    = MMGSDI_REFRESH_EVT;
      evt_info_ptr->data.refresh.mode                                      = extra_info_ptr->mode;
      evt_info_ptr->data.refresh.orig_mode                                 = mmgsdi_refresh_info_ptr[slot_index]->orig_mode;
      evt_info_ptr->data.refresh.slot                                      = request_hdr_ptr->slot_id;
      evt_info_ptr->data.refresh.stage                                     = node_ptr->stage;
      evt_info_ptr->data.refresh.aid.app_type                              = node_ptr->app_type;
      evt_info_ptr->data.refresh.app_info.app_data.app_type                = node_ptr->app_type;
      evt_info_ptr->data.refresh.source                                    = mmgsdi_refresh_info_ptr[slot_index]->source;
      evt_info_ptr->data.refresh.enforcement_policy.is_enforcement_present = FALSE;

      /* MMGSDI_REFRESH_APP_RESET_AUTO is an inernal mode, client voting
         to verify if its ok to deactivate provisioning is done
         by faking an APP_RESET indication */
      if(evt_info_ptr->data.refresh.mode == MMGSDI_REFRESH_APP_RESET_AUTO)
      {
        UIM_MSG_HIGH_0("Refresh: Changing internal APP_RESET_AUTO mode to NAA_APP_RESET");
        evt_info_ptr->data.refresh.mode      = MMGSDI_REFRESH_NAA_APP_RESET;
        evt_info_ptr->data.refresh.orig_mode = MMGSDI_REFRESH_NAA_APP_RESET;
      }

      /* if it is a NAA FCN event, either of the file list or file path should be present,
         else go to the next node */
      if(evt_info_ptr->data.refresh.mode == MMGSDI_REFRESH_NAA_FCN &&
         node_ptr->refresh_files.num_files == 0 &&
         node_ptr->refresh_files.num_file_paths == 0)
      {
        node_ptr = node_ptr->next_ptr;
        continue;
      }

      UIM_MSG_HIGH_3("Refresh: Sending Event MMGSDI_REFRESH_EVT mode= 0x%x, stage = 0x%x, slot = 0x%x",
                     evt_info_ptr->data.refresh.mode,
                     evt_info_ptr->data.refresh.stage,
                     evt_info_ptr->data.refresh.slot);

     /* It is possible to have nodes which may not have any files hence
        check if the node has > 0 files and then check for
        file enum and path pointers */
      if((node_ptr->refresh_files.num_files > 0) ||
         (node_ptr->refresh_files.num_file_paths > 0))
      {
        MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
          node_ptr->refresh_files.file_list_ptr,
          node_ptr->refresh_files.file_path_ptr,
          mmgsdi_status);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("Refresh: Client 0x%x has incompatible file path/enum list",
                        node_ptr->session_id);
          /* Free event info ptr */
          MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
          return mmgsdi_status;
        }
      }

      if(node_ptr->refresh_files.file_list_ptr != NULL)
      {
        evt_info_ptr->data.refresh.refresh_files.file_list_ptr =
          node_ptr->refresh_files.file_list_ptr;
        evt_info_ptr->data.refresh.refresh_files.num_files =
          node_ptr->refresh_files.num_files;
        evt_info_ptr->data.refresh.refresh_files.file_path_len = 0;
        evt_info_ptr->data.refresh.refresh_files.file_path_ptr = NULL;
      }

      if(node_ptr->refresh_files.file_path_ptr != NULL)
      {
        index = 0;
        evt_info_ptr->data.refresh.refresh_files.file_list_ptr = NULL;
        evt_info_ptr->data.refresh.refresh_files.num_files = 0;

        /* convert to single array structure of path */
        for (j = 0; j < node_ptr->refresh_files.num_file_paths; j++)
        {
          evt_info_ptr->data.refresh.refresh_files.file_path_len +=
            int32touint32(node_ptr->refresh_files.file_path_ptr[j].path_len)
            * sizeof(uint16);
        }

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
                  evt_info_ptr->data.refresh.refresh_files.file_path_ptr,
          evt_info_ptr->data.refresh.refresh_files.file_path_len);

        if(evt_info_ptr->data.refresh.refresh_files.file_path_ptr == NULL)
        {
          /* Free event info ptr */
          MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }

        for (j = 0; j < node_ptr->refresh_files.num_file_paths; j++)
        {
          for (i = 0;
               i < node_ptr->refresh_files.file_path_ptr[j].path_len;
               i++)
          {
            if ((index + 1) >= evt_info_ptr->data.refresh.refresh_files.file_path_len)
            {
              UIM_MSG_ERR_0("Trying to access out of allocated memory for file_path");
              mmgsdi_evt_free_data(evt_info_ptr);
              /* Free event info ptr */
              MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
              return MMGSDI_ERROR;
            }

            evt_info_ptr->data.refresh.refresh_files.file_path_ptr[index++] =
              node_ptr->refresh_files.file_path_ptr[j].path_buf[i] >> 8;
            evt_info_ptr->data.refresh.refresh_files.file_path_ptr[index++] =
              uint16touint8(node_ptr->refresh_files.file_path_ptr[j].path_buf[i] & 0x00FF);
          }
        }
      }

      mmgsdi_evt_notify_specific_session_id(node_ptr->session_id, evt_info_ptr);

      evt_info_ptr->data.refresh.refresh_files.file_path_len = 0;
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        evt_info_ptr->data.refresh.refresh_files.file_path_ptr);
    }
    node_ptr = node_ptr->next_ptr;
  }
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_send_evt_to_specific_sessions*/


/* ============================================================================
  FUNCTION:      MMGSDI_REFRESH_SEND_EVT_TO_ALL

  DESCRIPTION:
    This function will send events to all clients that are interested in the
    current application.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The funtion was successful in getting the file
                             attributes of a particular file.
    MMGSDI_ERROR:            Could not send event to clients
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_send_evt_to_all(
  const mmgsdi_card_refresh_extra_info_type *extra_info_ptr,
  const mmgsdi_request_header_type          *request_hdr_ptr)
{
  mmgsdi_event_data_type   *evt_info_ptr     = NULL;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_ERROR;
  uint8                     i                = 0;
  uint8                     j                = 0;
  mmgsdi_session_info_type *session_ptr      = NULL;
  mmgsdi_len_type           num_sessions     = 0;
  mmgsdi_slot_id_enum_type  slot             = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                     slot_index       = MMGSDI_SLOT_1_INDEX;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL_2(extra_info_ptr, request_hdr_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(request_hdr_ptr->slot_id, &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));

  if(evt_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_memscpy(&evt_info_ptr->data.refresh.aid,
                 sizeof(evt_info_ptr->data.refresh.aid),
                 &mmgsdi_refresh_info_ptr[slot_index]->aid,
                 sizeof(mmgsdi_aid_type));

  mmgsdi_memscpy(&evt_info_ptr->data.refresh.app_info.app_data,
                 sizeof(evt_info_ptr->data.refresh.app_info.app_data),
                 &mmgsdi_refresh_info_ptr[slot_index]->aid,
                 sizeof(mmgsdi_aid_type));

  if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_STEERING_OF_ROAMING)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      evt_info_ptr->data.refresh.plmn_list.data_ptr,
      mmgsdi_refresh_info_ptr[slot_index]->steering_mode_data.data_len);

    if(evt_info_ptr->data.refresh.plmn_list.data_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
      return MMGSDI_ERROR;
    }

    mmgsdi_memscpy(evt_info_ptr->data.refresh.plmn_list.data_ptr,
      mmgsdi_refresh_info_ptr[slot_index]->steering_mode_data.data_len,
      mmgsdi_refresh_info_ptr[slot_index]->steering_mode_data.data_ptr,
      mmgsdi_refresh_info_ptr[slot_index]->steering_mode_data.data_len);

    evt_info_ptr->data.refresh.plmn_list.data_len =
                  mmgsdi_refresh_info_ptr[slot_index]->steering_mode_data.data_len;
  }

  evt_info_ptr->evt                                                    = MMGSDI_REFRESH_EVT;
  evt_info_ptr->data.refresh.mode                                      = mmgsdi_refresh_info_ptr[slot_index]->mode;
  evt_info_ptr->data.refresh.orig_mode                                 = mmgsdi_refresh_info_ptr[slot_index]->orig_mode;
  evt_info_ptr->data.refresh.slot                                      = request_hdr_ptr->slot_id;
  evt_info_ptr->data.refresh.stage                                     = extra_info_ptr->stage;
  evt_info_ptr->data.refresh.source                                    = mmgsdi_refresh_info_ptr[slot_index]->source;
  evt_info_ptr->data.refresh.enforcement_policy.is_enforcement_present = FALSE;

  UIM_MSG_HIGH_3("Refresh: Sending Event MMGSDI_REFRESH_EVT mode= 0x%x, stage = 0x%x, slot = 0x%x",
                 mmgsdi_refresh_info_ptr[slot_index]->mode,
                 extra_info_ptr->stage,
                 evt_info_ptr->data.refresh.slot);

  for(i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if(mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[i]->num_session;

    for(j = 0; (num_sessions > 0) && (j < MMGSDI_MAX_SESSION_INFO); j++)
    {
      session_ptr =
        mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

      if(session_ptr == NULL)
      {
        /*Invalid Session, Continue to next*/
        continue;
      }
      num_sessions--;

      if(mmgsdi_refresh_info_ptr[slot_index]->mode != MMGSDI_REFRESH_RESET &&
         mmgsdi_refresh_info_ptr[slot_index]->mode != MMGSDI_REFRESH_RESET_AUTO)
      {

        session_ptr = mmgsdi_util_cmp_aid_get_session_ptr(
                        i,
                        j,
                        &mmgsdi_refresh_info_ptr[slot_index]->aid,
                        mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                        TRUE);

        if(session_ptr == NULL)
        {
          /*No need to send the event to this app*/
          continue;
        }
        if((mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_STEERING_OF_ROAMING) &&
           (!session_ptr->steering_of_roaming))
        {
          /* Not interested in steering */
          continue;
        }
        if(mmgsdi_util_is_card_session(session_ptr->session_type))
        {
          /*Card Sessions need not be sent REFRESH notifications*/
          continue;
        }
        if((session_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
           (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] == NULL) ||
           (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr == NULL))
        {
          /*No need to send the event to this app*/
          continue;
        }
        /* If the same app is open on multiple channels, we shall be performing
           APP_RESET/3G_SESSION_RESET on only one channel */
        if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_APP_RESET ||
           mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_3G_SESSION_RESET)
        {
          if((extra_info_ptr->stage == MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED) ||
             (extra_info_ptr->stage == MMGSDI_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED))
          {
            if(mmgsdi_util_is_prov_session(session_ptr->session_type))
            {
              continue;
            }
          }
          else if(extra_info_ptr->stage == MMGSDI_REFRESH_STAGE_START)
          {
            if(mmgsdi_util_is_prov_session(session_ptr->session_type))
            {
              if(extra_info_ptr->app_type == MMGSDI_REFRESH_START_NON_PROV_APP)
              {
                continue;
              }
            }
            else
            {
              if(extra_info_ptr->app_type == MMGSDI_REFRESH_START_PROV_APP)
              {
                continue;
              }
            }
          }
        }
        /* fetch the APP type from current APP */
        evt_info_ptr->data.refresh.aid.app_type =
          mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data.app_type;
        evt_info_ptr->data.refresh.app_info.app_data.app_type =
          mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data.app_type;

        if((evt_info_ptr->data.refresh.mode == MMGSDI_REFRESH_STEERING_OF_ROAMING) &&
           (evt_info_ptr->data.refresh.stage == MMGSDI_REFRESH_STAGE_START))
        {
          session_ptr->steering_complete_rcvd = FALSE;
        }
        else if ((evt_info_ptr->data.refresh.mode == MMGSDI_REFRESH_STEERING_OF_ROAMING) &&
                 ((evt_info_ptr->data.refresh.stage == MMGSDI_REFRESH_STAGE_END_SUCCESS) ||
                  (evt_info_ptr->data.refresh.stage == MMGSDI_REFRESH_STAGE_END_FAILED)))
        {
          session_ptr->steering_complete_rcvd = TRUE;
        }

        /* MMGSDI_REFRESH_APP_RESET_AUTO is an inernal mode which is set while
           processing deactivate provisioning request. Clients are notified for
           Refresh event by changing it to APP_RESET. Note that Client Voting
           to verify if they are ok to deactivate provisioning session is also
           preformed by faking Refresh mode as APP_RESET */
        if(evt_info_ptr->data.refresh.mode == MMGSDI_REFRESH_APP_RESET_AUTO)
        {
          UIM_MSG_HIGH_0("Refresh: Changing internal APP_RESET_AUTO mode to NAA_APP_RESET");
          evt_info_ptr->data.refresh.mode      = MMGSDI_REFRESH_NAA_APP_RESET;
          evt_info_ptr->data.refresh.orig_mode = MMGSDI_REFRESH_NAA_APP_RESET;
        }
        mmgsdi_evt_notify_specific_session_id(session_ptr->session_id, evt_info_ptr);
      }
      else
      {
        mmgsdi_status = mmgsdi_util_get_session_app_info(
                          session_ptr->session_id,
                          &slot,
                          NULL,
                          NULL,
                          NULL,
                          NULL,
                          NULL);

        if(mmgsdi_status != MMGSDI_SUCCESS ||
           slot != mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id)
        {
          continue;
        }
        /* fetch the AID info from current APP */
        if((session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
           (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL) &&
           (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr != NULL))
        {
          mmgsdi_memscpy(&evt_info_ptr->data.refresh.aid,
            sizeof(evt_info_ptr->data.refresh.aid),
            &mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data,
            sizeof(mmgsdi_aid_type));

          mmgsdi_memscpy(&evt_info_ptr->data.refresh.app_info.app_data,
            sizeof(evt_info_ptr->data.refresh.app_info.app_data),
            &mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data,
            sizeof(mmgsdi_aid_type));
        }

        mmgsdi_evt_notify_specific_session_id(session_ptr->session_id,
                                              evt_info_ptr);
      }
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr->data.refresh.plmn_list.data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(evt_info_ptr);
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_send_evt_to_all*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_IS_CALL_STACK_FILE

  DESCRIPTION
    This function will be used to check if the file list provided requires
    the MMGSDI Task to take down the call stack.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    boolean:  TRUE:  The callstack needs to be taken down.
              FALSE: The callstack does not need to be taken down.

  SIDE EFFECTS
    NONE
===========================================================================*/
static boolean mmgsdi_refresh_is_call_stack_file(
  mmgsdi_file_enum_type     file)
{
  switch(file)
  {
     /* GSM Related Files which require call stack to be taken down and then
        brought back up. No need to take down call stack for EF SPN */
    case MMGSDI_GSM_AD:
    case MMGSDI_GSM_AAEM:
    case MMGSDI_GSM_ACC:
    case MMGSDI_GSM_BCCH:
    case MMGSDI_GSM_EMLPP:
    case MMGSDI_GSM_IMSI:
    case MMGSDI_GSM_KC:
    case MMGSDI_GSM_KCGPRS:
    case MMGSDI_GSM_LOCI:
    case MMGSDI_GSM_LOCIGPRS:
    case MMGSDI_GSM_LP:
    case MMGSDI_GSM_PHASE:
    case MMGSDI_GSM_SST:

    /* USIM Related Files which require call stack to be taken down and then
       brought back up. No need to take down call stack for EF SPN */
    case MMGSDI_USIM_AAEM:
    case MMGSDI_USIM_EMLPP:
    case MMGSDI_USIM_EST:
    case MMGSDI_USIM_IMSI:
    case MMGSDI_USIM_KC:
    case MMGSDI_USIM_KEYS:
    case MMGSDI_USIM_KEYSPS:
    case MMGSDI_USIM_UST:
    case MMGSDI_USIM_LOCI:
    case MMGSDI_USIM_PSLOCI:
    case MMGSDI_USIM_EPSLOCI:
    case MMGSDI_USIM_AD:
    case MMGSDI_USIM_ACC:

    /* CDMA Related Files which require call stack to
     ** be taken down and then brought back up */
    case MMGSDI_CDMA_ACCOLC:
    case MMGSDI_CDMA_IMSI_M:
    case MMGSDI_CDMA_IMSI_T:
    case MMGSDI_CDMA_TMSI:
    case MMGSDI_CDMA_NAM_LOCK:
    case MMGSDI_CDMA_MSISDN:
    case MMGSDI_CDMA_CC:
    case MMGSDI_CDMA_SF_EUIM_ID:
    case MMGSDI_CDMA_PREF_LANG:
    case MMGSDI_CDMA_SERVICE_PREF:
    case MMGSDI_CDMA_PRL:
    case MMGSDI_CDMA_RUIM_ID:
    case MMGSDI_CDMA_OTAPA_SPC_ENABLE:
    case MMGSDI_CDMA_SCI:
    case MMGSDI_CDMA_ANALOG_HOME_SID:
    case MMGSDI_CDMA_HOME_SID_NID:
    case MMGSDI_CDMA_ZONE_BASED_REGN_IND:
    case MMGSDI_CDMA_SYS_REGN_IND:
    case MMGSDI_CDMA_DIST_BASED_REGN_IND:
    case MMGSDI_CDMA_CALL_TERM_MODE_PREF:
    case MMGSDI_CDMA_SVC_TABLE:
    case MMGSDI_CDMA_OTASP_OTAPA_FEATURES:
    case MMGSDI_CDMA_UIM_ID_USAGE_IND:
    case MMGSDI_CDMA_SPC_STATUS:
    case MMGSDI_CDMA_3GPD_SIPPAPSS:
    case MMGSDI_CDMA_3GPD_SIPUPP:
    case MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND:
    case MMGSDI_CDMA_3GPD_MIPFLAGS:
    case MMGSDI_CDMA_3GPD_TCPCONFIG:
    case MMGSDI_CDMA_3GPD_MIPUPP:
    case MMGSDI_CDMA_3GPD_3GPDOPM:
    case MMGSDI_CDMA_3GPD_MIPSP:

    /* CSIM FILES */
    case MMGSDI_CSIM_ACCOLC:
    case MMGSDI_CSIM_IMSI_M:
    case MMGSDI_CSIM_IMSI_T:
    case MMGSDI_CSIM_TMSI:
    case MMGSDI_CSIM_NAM_LOCK:
    case MMGSDI_CSIM_MSISDN:
    case MMGSDI_CSIM_CALL_COUNT:
    case MMGSDI_CSIM_SF_EUIM_ID:
    case MMGSDI_CSIM_SERVICE_PREF:
    case MMGSDI_CSIM_PRL:
    case MMGSDI_CSIM_RUIM_ID:
    case MMGSDI_CSIM_OTAPA_SPC_ENABLE:
    case MMGSDI_CSIM_SSCI:
    case MMGSDI_CSIM_ANALOG_HOME_SID:
    case MMGSDI_CSIM_CDMA_HOME_SID_NID:
    case MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND:
    case MMGSDI_CSIM_CDMA_SYS_REGN_IND:
    case MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND:
    case MMGSDI_CSIM_CALL_TERM_MODE_PREF:
    case MMGSDI_CSIM_CST:
    case MMGSDI_CSIM_OTASP_OTAPA_FEATURES:
    case MMGSDI_CSIM_USGIND:
    case MMGSDI_CSIM_SPC_STATUS:
    case MMGSDI_CSIM_3GPD_SIPPAPSS:
    case MMGSDI_CSIM_3GPD_SIPUPP:
    case MMGSDI_CSIM_ANALOG_LOCN_AND_REGN_IND:
    case MMGSDI_CSIM_3GPD_MIPSP:
    case MMGSDI_CSIM_3GPD_3GPDOPM:
    case MMGSDI_CSIM_3GPD_TCPCONFIG:
    case MMGSDI_CSIM_3GPD_MIPUPP:
      return TRUE;

    default:
      return FALSE;
  } /* switch */
} /* mmgsdi_refresh_is_call_stack_file */


/* ==========================================================================
  FUNCTION:      MMGSDI_REFRESH_CHANGE_MODE_IF_REQUIRED

  DESCRIPTION:
    This function checks the Refresh FCN to see if it involves the provisioning
    app call stack. If so, the refresh mode is modified to an appropriate mode.

  DEPENDENCIES:
    None

  LIMITATIONS:
    Mode change is only allowed for provisioning app.
    Currently called for Refresh - FCN check ONLY

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          - Command processed,
    MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
    MMGSDI_ERROR            - Other unknown failure

  SIDE EFFECTS:
    None
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_change_mode_if_required(
  const mmgsdi_refresh_card_req_type           *req_ptr,
  uint8                                         slot_index,
  mmgsdi_refresh_mode_enum_type                *refresh_mode_ptr,
  const mmgsdi_internal_refresh_file_list_type *refresh_file_list_ptr)
{
  uint32                    i                           = 0;
  boolean                   call_stack_file_present     = FALSE;
  boolean                   non_call_stack_file_present = FALSE;
  boolean                   is_prov                     = FALSE;
  mmgsdi_return_enum_type   mmgsdi_status               = MMGSDI_SUCCESS;

  /*----------------------------------------------------------------
    Check if it is provisioning app - if it is ,
    then check if the files is on the call stack, if it is
    then change the mode to init
    ----------------------------------------------------------------*/
  /* 1. Check parameters */
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL_5(req_ptr, mmgsdi_refresh_info_ptr[slot_index],
                              refresh_mode_ptr,
                              refresh_file_list_ptr,
                              refresh_file_list_ptr->file_list_ptr);

  if( refresh_file_list_ptr->num_files == 0)
  {
    UIM_MSG_HIGH_0("Refresh: Incorrect file related parameters for Refresh FCN");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* 2. Check if the app is provisioning app */
  mmgsdi_status = mmgsdi_util_is_prov_app(&mmgsdi_refresh_info_ptr[slot_index]->aid,
                                          mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                                          &is_prov);
  if(mmgsdi_status != MMGSDI_SUCCESS || is_prov == FALSE)
  {
    UIM_MSG_HIGH_0("Refresh: Not a provisioning app. Mode change not required.");
    return MMGSDI_SUCCESS;
  }

  /* Check if the file list has files that belong to the call stack and those that
     do not belong to the call stack to get the mode */
  for(i=0;i<refresh_file_list_ptr->num_files;i++)
  {
    if(!mmgsdi_refresh_is_call_stack_file(
        refresh_file_list_ptr->file_list_ptr[i]))
    {
      non_call_stack_file_present = TRUE;
    }
    else
    {
      call_stack_file_present = TRUE;
    }
  }

  if( non_call_stack_file_present && call_stack_file_present)
  {
    *refresh_mode_ptr = MMGSDI_REFRESH_NAA_INIT_FCN;
  }
  else if(call_stack_file_present)
  {
    *refresh_mode_ptr = MMGSDI_REFRESH_NAA_INIT;
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_change_mode_if_required*/


/* ==========================================================================
  FUNCTION MMGSDI_REFRESH_SEND_EVT

  DESCRIPTION
    This function, will send events to all nodes or all clients depending on
    the refresh stage and the refresh mode

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
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_send_evt(
  mmgsdi_card_refresh_extra_info_type *extra_info_ptr,
  const mmgsdi_request_header_type    *request_hdr_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(extra_info_ptr, request_hdr_ptr);

  if(extra_info_ptr->send_evt)
  {
    uint8 slot_index = MMGSDI_SLOT_1_INDEX;

    mmgsdi_status = mmgsdi_util_get_slot_index(request_hdr_ptr->slot_id, &slot_index);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      extra_info_ptr->stage = MMGSDI_REFRESH_STAGE_END_FAILED;
      return MMGSDI_ERROR;
    }
    if((slot_index != MMGSDI_SLOT_1_INDEX) &&
       (slot_index != MMGSDI_SLOT_2_INDEX) &&
       (slot_index != MMGSDI_SLOT_3_INDEX))
    {
      extra_info_ptr->stage = MMGSDI_REFRESH_STAGE_END_FAILED;
      return MMGSDI_ERROR;
    }

    MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

    if(extra_info_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT ||
       extra_info_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN  ||
       extra_info_ptr->mode  == MMGSDI_REFRESH_NAA_FCN                   ||
       extra_info_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH)
    {
      mmgsdi_status = mmgsdi_refresh_send_evt_to_specific_sessions(
                      extra_info_ptr,
                      request_hdr_ptr);
    }
    else
    {
      mmgsdi_status = mmgsdi_refresh_send_evt_to_all(extra_info_ptr,
                                                     request_hdr_ptr);
    }

    if(mmgsdi_status == MMGSDI_ERROR)
    {
      extra_info_ptr->stage = MMGSDI_REFRESH_STAGE_END_FAILED;
    }
  }
  return mmgsdi_status;
}/*mmgsdi_refresh_send_evt*/


/*=============================================================================
FUNCTION        MMGSDI_CLIENT_REFRESH_COMPLETE

DESCRIPTION
  This function allows the client to inform MMGSDI when it has
  finished REFRESH handling

PARAMETERS
  client_id:        Client ID of the caller
  pass_fail:        Boolean indicating whether client was able to
                    successfully read all the files that were refreshed
                    This could also be applicable to init cases, where the
                    client would inform MMGSDI that it was able to perform
                    its init properly
  response_cb_ptr:  Callback to this command
  client_ref:       User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Client ID is required

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_client_refresh_complete (
   mmgsdi_client_id_type              client_id,
   mmgsdi_slot_id_enum_type           card_slot,
   boolean                            pass_fail,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type req           = {{0}};

  req.refresh_req_type  = MMGSDI_REFRESH_ORIG_COMPLETE_REQ;
  req.refresh.pass_fail = pass_fail;

  mmgsdi_status = mmgsdi_client_build_refresh_req(client_id,
                                                  card_slot,
                                                  &req,
                                                  FALSE,
                                                  response_cb_ptr,
                                                  client_ref,
                                                  FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Command Queuing Failed 0x%x, mmgsdi_client_refresh_complete",
                  mmgsdi_status);
  }
  return mmgsdi_status;
}/*mmgsdi_client_refresh_complete*/



/*===========================================================================
  FUNCTION MMGSDI_REFRESH_SEND_COMPLETE

  DESCRIPTION:
    This function will send refresh complete command to itself

  DEPENDENCIES:
     None

  LIMITATIONS:
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 ===========================================================================*/
void mmgsdi_refresh_send_complete(
  mmgsdi_client_id_type    client_id,
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot,
  boolean                  pass_fail
)
{
  mmgsdi_slot_data_type        *slot_ptr       = NULL;
  mmgsdi_return_enum_type       mmgsdi_status  = MMGSDI_ERROR;
  uint8                         slot_index     = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_2("FUNCTION mmgsdi_refresh_send_complete , status = 0x%x, slot = 0x%x",
                 pass_fail, slot);

  mmgsdi_status = mmgsdi_util_get_slot_index(slot, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return;
  }
  if(mmgsdi_refresh_info_ptr[slot_index] == NULL ||
     mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id != slot)
  {
    /*No Refresh on Progress, return*/
    UIM_MSG_ERR_0("Refresh: No Refresh is in progress, no event to send");
    return;
  }

  slot_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_ptr == NULL )
  {
    return;
  }

  if( mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET ||
      mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET_AUTO)
  {
    mmgsdi_status = mmgsdi_client_refresh_complete(client_id,
                                                   slot,
                                                   pass_fail,
                                                   mmgsdi_util_internal_cb,
                                                   0);
    /* Updating slot specific refresh state only after successful queueing
       of MMGSDI_REFRESH_ORIG_COMPLETE_REQ */
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      slot_ptr->refresh_state = MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE;
    }
  }
  else
  {
    mmgsdi_status = mmgsdi_session_refresh_complete(session_id,
                                                    pass_fail,
                                                    mmgsdi_util_internal_cb,
                                                    0);
  }
  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    UIM_MSG_ERR_1("Unable to send Refresh Complete 0x%x", mmgsdi_status);
  }

}/*mmgsdi_refresh_send_complete*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_PROCESS_EVT

  DESCRIPTION
    This function will handle the Refresh event and set the global
    mmgsdi_data_slot1/mmgsdi_data_slot2 if required.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    Events will be sent to clients and client will in turn send
    new commands to MMGSDI that will move the overall refresh
    state
===========================================================================*/
void mmgsdi_refresh_process_evt(
  const mmgsdi_event_data_type *event_ptr)
{
  mmgsdi_slot_data_type    *slot_data_ptr        = NULL;
  mmgsdi_return_enum_type   mmgsdi_status        = MMGSDI_ERROR;
  uint32                    i,j                  = 0;
  boolean                   file_1x_found        = FALSE;
  boolean                   pass_fail            = FALSE;
  mmgsdi_refresh_node_type *node_ptr             = NULL;
  mmgsdi_file_enum_type     mmgsdi_file          = MMGSDI_NO_FILE_ENUM;
  mmgsdi_int_app_info_type *int_app_info_ptr     = NULL;
  boolean                   eons_ef_refreshed    = FALSE;
  boolean                   opl_pnn_ef_refreshed = FALSE;
  boolean                   csg_ef_refreshed     = FALSE;
  uint8                     slot_index           = MMGSDI_SLOT_1_INDEX;
#ifdef FEATURE_UIM_PERSISTENT_CACHE
  mmgsdi_session_info_type *session_info_ptr     = NULL;
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

  if (event_ptr == NULL)
  {
    UIM_MSG_ERR_0("Refresh: event_ptr is NULL");
    return;
  }

  UIM_MSG_HIGH_5("FUNCTION mmgsdi_refresh_process_evt, event: 0x%x, Refresh MODE 0x%x, Refresh STAGE: 0x%x, Orig MODE 0x%x, SLOT 0x%x",
                 event_ptr->evt,
                 event_ptr->data.refresh.mode,
                 event_ptr->data.refresh.stage,
                 event_ptr->data.refresh.orig_mode,
                 event_ptr->data.refresh.slot);

  if(event_ptr->evt != MMGSDI_REFRESH_EVT)
  {
    return;
  }

  slot_data_ptr=
    mmgsdi_util_get_slot_data_ptr(event_ptr->data.refresh.slot);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(event_ptr->data.refresh.slot,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return;
  }

  if(mmgsdi_refresh_info_ptr[slot_index] == NULL)
  {
    UIM_MSG_ERR_0("Refresh: mmgsdi_refresh_info_ptr is NULL");
    return;
  }

  switch(event_ptr->data.refresh.stage)
  {
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
      (void)mmgsdi_session_ok_to_refresh(event_ptr->session_id,
                                         TRUE,
                                         mmgsdi_util_internal_cb,
                                         0);
      break;
    case MMGSDI_REFRESH_STAGE_START:
#ifdef FEATURE_UIM_PERSISTENT_CACHE
      /* Get the slot and app info related to the session */
      mmgsdi_status = mmgsdi_util_get_session_app_info(event_ptr->session_id,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       &session_info_ptr,
                                                       NULL);

      if((mmgsdi_status == MMGSDI_SUCCESS) &&
         (session_info_ptr != NULL))
      {
        /* update persistent cache */
        if(event_ptr->data.refresh.mode == MMGSDI_REFRESH_RESET ||
           event_ptr->data.refresh.mode == MMGSDI_REFRESH_RESET_AUTO)
        {
          (void)mmgsdi_persistent_cache_deactivate_profile(event_ptr->data.refresh.slot,
                                                           session_info_ptr->session_type);
        }
        else if(event_ptr->data.refresh.mode == MMGSDI_REFRESH_NAA_FCN)
        {
          /* refresh files will be updated in presistent case after cache write */
        }
        else
        {
          (void)mmgsdi_persistent_cache_delete(event_ptr->data.refresh.slot,
                                               session_info_ptr->session_type,
                                               TRUE);
        }
      }
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

      switch(event_ptr->data.refresh.mode)
      {
        case MMGSDI_REFRESH_NAA_APP_RESET:
        case MMGSDI_REFRESH_3G_SESSION_RESET:
          /*Set the rsp field for all other nodes also to Complete*/
          node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
          while(node_ptr)
          {
            if(node_ptr->client_id != mmgsdi_generic_data_ptr->client_id)
            {
              node_ptr->rsp_received = TRUE;
            }
            node_ptr = node_ptr->next_ptr;
          }

          /* Update the session closed cause. The session changed event
             broadcasts the cause */
          mmgsdi_util_update_session_closed_cause(event_ptr->session_id,
                                                  MMGSDI_SESSION_CLOSED_CAUSE_REFRESH);

          mmgsdi_util_preinit_mmgsdi(event_ptr->session_id,
                                     event_ptr->data.refresh.aid.app_type,
                                     event_ptr->data.refresh.mode,
                                     event_ptr->data.refresh.slot,
                                     FALSE);
          break;

        case MMGSDI_REFRESH_RESET:
        case MMGSDI_REFRESH_RESET_AUTO:
          /*Set the rsp field for all nodes to Complete*/
          node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
          while(node_ptr)
          {
            node_ptr->rsp_received = TRUE;
            node_ptr = node_ptr->next_ptr;
          }
          /* Update the session closed cause. The session changed event
             broadcasts the cause */
          mmgsdi_util_update_session_closed_cause(event_ptr->session_id,
                                                  MMGSDI_SESSION_CLOSED_CAUSE_REFRESH);
          break;

        case MMGSDI_REFRESH_NAA_INIT:
        case MMGSDI_REFRESH_NAA_INIT_FCN:
        case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
          /*Set the rsp field for all other nodes also to Complete*/
          node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
          while(node_ptr)
          {
            if(node_ptr->client_id != mmgsdi_generic_data_ptr->client_id)
            {
              node_ptr->rsp_received = TRUE;
            }
            node_ptr = node_ptr->next_ptr;
          }
          mmgsdi_util_preinit_mmgsdi(event_ptr->session_id,
                                     event_ptr->data.refresh.aid.app_type,
                                     event_ptr->data.refresh.mode,
                                     event_ptr->data.refresh.slot,
                                     FALSE);
          /* !!!!app_info_ptr->app_state update here */

          mmgsdi_status = mmgsdi_util_get_prov_session_info(event_ptr->session_id,
                                                            NULL,
                                                            NULL,
                                                            &int_app_info_ptr);

          if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
          {
            UIM_MSG_ERR_2("Error obtaining Prov Session Info Status 0x%x Session ID 0x%x",
                          mmgsdi_status, event_ptr->session_id);
            return;
          }

          /* If we have not yet reached the PIN verified state, do not triger
                   Perso phase. It shall get triggered when PIN is verified */
          if (int_app_info_ptr->app_state > MMGSDI_APP_STATE_PIN_EVT_SENT)
          {
            int_app_info_ptr->app_state = MMGSDI_APP_STATE_READY_FOR_PERSO;

            (void)mmgsdi_card_init_build_internal_pup_req(
                    event_ptr->session_id,
                    MMGSDI_SLOT_AUTOMATIC,
                    MMGSDI_PIN1_EVT,
                    MMGSDI_STATE_CARD_INSERTED,
                    MMGSDI_APP_STATE_READY_FOR_PERSO);
          }
          break;

        case MMGSDI_REFRESH_NAA_FCN:
        {
          mmgsdi_uim_ef_item_category_enum_type EF_category = ROOT;
          uim_protocol_type                     protocol    = UIM_NO_SUCH_MODE;

          if (event_ptr->data.refresh.refresh_files.num_files > 0 &&
              event_ptr->data.refresh.refresh_files.file_list_ptr == NULL)
          {
            UIM_MSG_ERR_0("Refresh: file list ptr is NULL");
            return;
          }

          mmgsdi_status = MMGSDI_SUCCESS;

          protocol = mmgsdi_util_get_uim_protocol(event_ptr->data.refresh.slot);

          /*Need to selective clear cache*/
          for(i=0;i<event_ptr->data.refresh.refresh_files.num_files;i++)
          {
            EF_category = MMGSDIFILE_GET_CATEGORY_FROM_FILE(
                            event_ptr->data.refresh.refresh_files.file_list_ptr[i]);

            /* If protocol is ICC and file in refresh list is in USIM/CSIM ADF
               or protocol is UICC and file in refresh list is in GSM/CDMA DF,
               don't try to cache the file. Exempt GSM SPN file when app type
               is UICC; we have seperate handling for it */
            if(!((event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                  MMGSDI_GSM_SPN) &&
                 (event_ptr->data.refresh.aid.app_type == MMGSDI_APP_USIM)) &&
               (((protocol == UIM_ICC) && ((EF_category == USIM) || (EF_category == CSIM)))||
                ((protocol == UIM_UICC) && ((EF_category == GSM) || (EF_category == CDMA)))))
            {
              continue;
            }

            for(j=0;j<MMGSDI_REFRESH_GW_REG_FILES;j++)
            {
              if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                 mmgsdi_refresh_gw_reg_file_list_ptr[j])
              {
                break;
              }
            }
            if(j == MMGSDI_REFRESH_GW_REG_FILES)
            {
              file_1x_found = FALSE;
              for(j=0;j<MMGSDI_REFRESH_1X_REG_FILES;j++)
              {
                if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                   mmgsdi_refresh_1x_reg_file_list_ptr[j])
                {
                  file_1x_found = TRUE;
                  break;
                }
              }
              /*Skip reading this file*/
              if(file_1x_found == FALSE)
              {
                continue;
              }
            }
            /* Clear the item in the cache */
            (void)mmgsdi_cache_delete_item(event_ptr->session_id,
                                           1,
                                           &event_ptr->data.refresh.refresh_files.file_list_ptr[i]);

             /* In case the card incorrectly refreshes GSM_SPN, though it is
               operating in the 3G mode, clear the cache for USIM_SPN. This
               enables clients to read the SPN from the card */
            if((event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_GSM_SPN) &&
               (event_ptr->data.refresh.aid.app_type == MMGSDI_APP_USIM))
            {
               UIM_MSG_HIGH_0("Clearing cache for USIM SPN");
               mmgsdi_file = MMGSDI_USIM_SPN;

               (void)mmgsdi_cache_delete_item(event_ptr->session_id,
                                              1,
                                              &mmgsdi_file);
               eons_ef_refreshed = TRUE;

                /* Update the cache */
               mmgsdi_status = mmgsdi_card_init_cache_binary(
                  event_ptr->session_id,
                  event_ptr->data.refresh.aid.app_type,
                  event_ptr->data.refresh.slot,
                  MMGSDI_USIM_SPN,
                  NULL);

#ifdef FEATURE_SIMLOCK
                 if(mmgsdi_simlock_check_category_status(
                      event_ptr->data.refresh.slot,
                      SIMLOCK_CATEGORY_3GPP_SPN))
                 {
                   (void)mmgsdi_simlock_build_and_run_algo(FALSE);
                 }
#endif /*FEATURE_SIMLOCK*/

            }
            else if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                    MMGSDI_USIM_CPHSI ||
                    event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                    MMGSDI_GSM_CPHSI)
            {
              mmgsdi_cphs_read_cphs_efs_info_type  read_cphs_efs;
              read_cphs_efs.read_cphsi     = TRUE;
              read_cphs_efs.read_csp       = FALSE;
              read_cphs_efs.read_ons       = FALSE;
              read_cphs_efs.read_ons_short = FALSE;

              mmgsdi_status = mmgsdi_proc_cphs_post_pin1_init(
                event_ptr->session_id,
                event_ptr->data.refresh.slot,
                event_ptr->data.refresh.aid.app_type,
                read_cphs_efs);
            }
            else if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_USIM_CSP ||
               event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_GSM_CSP)
            {
              mmgsdi_cphs_read_cphs_efs_info_type  read_cphs_efs;
              read_cphs_efs.read_cphsi     = FALSE;
              read_cphs_efs.read_csp       = TRUE;
              read_cphs_efs.read_ons       = FALSE;
              read_cphs_efs.read_ons_short = FALSE;

              mmgsdi_status = mmgsdi_proc_cphs_post_pin1_init(
                event_ptr->session_id,
                event_ptr->data.refresh.slot,
                event_ptr->data.refresh.aid.app_type,
                read_cphs_efs);
            }
            else if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_ECC)
            {
                mmgsdi_status = mmgsdi_cache_init_usim_ecc(
                                            event_ptr->session_id,
                                            event_ptr->data.refresh.slot);
            }
            else if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_USIM_ONS||
               event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_GSM_ONS)
            {
              mmgsdi_cphs_read_cphs_efs_info_type  read_cphs_efs;
              read_cphs_efs.read_cphsi     = FALSE;
              read_cphs_efs.read_csp       = FALSE;
              read_cphs_efs.read_ons       = TRUE;
              read_cphs_efs.read_ons_short = FALSE;

              mmgsdi_status = mmgsdi_proc_cphs_post_pin1_init(
                event_ptr->session_id,
                event_ptr->data.refresh.slot,
                event_ptr->data.refresh.aid.app_type,
                read_cphs_efs);

              mmgsdi_status = mmgsdi_util_get_prov_session_info(event_ptr->session_id,
                                                                NULL,
                                                                NULL,
                                                                &int_app_info_ptr);

              if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
              {
                UIM_MSG_ERR_2("Error obtaining Prov Session Info Status 0x%x Session ID 0x%x",
                              mmgsdi_status, event_ptr->session_id);
                return;
              }

              mmgsdi_eons_send_event(event_ptr->session_id,
                                     MMGSDI_EONS_TRIGGER_REFRESH_FCN,
                                     csg_ef_refreshed);
            }
            else if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_USIM_ONS_SHORT||
               event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
               MMGSDI_GSM_ONS_SHORT)
            {
              mmgsdi_cphs_read_cphs_efs_info_type  read_cphs_efs;
              read_cphs_efs.read_cphsi     = FALSE;
              read_cphs_efs.read_csp       = FALSE;
              read_cphs_efs.read_ons       = FALSE;
              read_cphs_efs.read_ons_short = TRUE;

              mmgsdi_status = mmgsdi_proc_cphs_post_pin1_init(
                event_ptr->session_id,
                event_ptr->data.refresh.slot,
                event_ptr->data.refresh.aid.app_type,
                read_cphs_efs);

              mmgsdi_status = mmgsdi_util_get_prov_session_info(event_ptr->session_id,
                                                                NULL,
                                                                NULL,
                                                                &int_app_info_ptr);

              if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
              {
                UIM_MSG_ERR_2("Error obtaining Prov Session Info Status 0x%x Session ID 0x%x",
                              mmgsdi_status, event_ptr->session_id);
                return;
              }

              mmgsdi_eons_send_event(event_ptr->session_id,
                                     MMGSDI_EONS_TRIGGER_REFRESH_FCN,
                                     csg_ef_refreshed);
            }
            else if (event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_HIDDENKEY ||
                event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_CSIM_HIDDEN_KEY)
            {
              mmgsdi_status = mmgsdi_refresh_process_fcn_hiddenkey(
                                event_ptr,
                                event_ptr->data.refresh.refresh_files.file_list_ptr[i]);
            }
            else if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_GSM_SPN ||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_GSM_SPDI||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_GSM_OPL||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_GSM_PNN||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_SPN ||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_SPDI ||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_OPL ||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_PNN ||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_CDMA_HOME_SVC_PVDR_NAME ||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_CSIM_SPN)
            {
              eons_ef_refreshed = TRUE;
              if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_GSM_OPL||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_GSM_PNN||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_OPL ||
                       event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_PNN)
              {
                opl_pnn_ef_refreshed = TRUE;

                mmgsdi_status = mmgsdi_refresh_cache_opl_pnn(event_ptr->session_id,
                                  event_ptr->client_id,
                                  event_ptr->data.refresh.refresh_files.file_list_ptr[i]);
              }
              else
              {
                mmgsdi_status = mmgsdi_card_init_cache_binary(
                  event_ptr->session_id,
                  event_ptr->data.refresh.aid.app_type,
                  event_ptr->data.refresh.slot,
                  event_ptr->data.refresh.refresh_files.file_list_ptr[i],
                  NULL);
              }

  #ifdef FEATURE_SIMLOCK
              if((event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                    MMGSDI_GSM_SPN ||
                  event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                    MMGSDI_USIM_SPN) &&
                 mmgsdi_simlock_check_category_status(
                   event_ptr->data.refresh.slot,
                   SIMLOCK_CATEGORY_3GPP_SPN))
              {
                (void)mmgsdi_simlock_build_and_run_algo(FALSE);
              }
#endif /*FEATURE_SIMLOCK*/
            }
            else if (event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                     MMGSDI_USIM_ACM ||
                     event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                     MMGSDI_GSM_ACM  ||
                     event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                     MMGSDI_USIM_EPSNSC ||
                     event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                     MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND ||
                     event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                     MMGSDI_CDMA_ZONE_BASED_REGN_IND)
            {
              mmgsdi_status = mmgsdi_card_init_cache_record(
                                event_ptr->session_id,
                                event_ptr->data.refresh.aid.app_type,
                                event_ptr->data.refresh.slot,
                                event_ptr->data.refresh.refresh_files.file_list_ptr[i],
                                NULL);
            }
            else if (event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                     MMGSDI_CSIM_ECC ||
                     event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                     MMGSDI_CDMA_ECC ||
                     event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                     MMGSDI_GSM_ECC)
            {
              mmgsdi_evt_session_notify_type   notify_type;

              memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

              mmgsdi_status = mmgsdi_card_init_cache_binary(
                                event_ptr->session_id,
                                event_ptr->data.refresh.aid.app_type,
                                event_ptr->data.refresh.slot,
                                event_ptr->data.refresh.refresh_files.file_list_ptr[i],
                                NULL);
              /* If ECC EF is refreshed, send out the ECC event */
              notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
              notify_type.slot_id     = event_ptr->data.ecc.slot;
              notify_type.session_id  = event_ptr->session_id;
              mmgsdi_evt_build_and_send_ecc(notify_type);
            }
            else if(event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_ACSGL ||
                event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_CSGT  ||
                event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_HNBN  ||
                event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_OCSGL  ||
                event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_OCSGT  ||
                event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                MMGSDI_USIM_OHNBN)
            {
              mmgsdi_nv_context_type efs_index = MMGSDI_NV_CONTEXT_INVALID;

              if((mmgsdi_util_get_efs_item_index(
                    event_ptr->session_id, &efs_index) == MMGSDI_SUCCESS) &&
                 (mmgsdi_nv_get_external_feature_status(
                          MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG,
                          efs_index) == MMGSDI_FEATURE_ENABLED))
              {
                eons_ef_refreshed = TRUE;
                csg_ef_refreshed  = TRUE;
                mmgsdi_status = mmgsdi_csg_refresh_csgl_cache(
                                     event_ptr->session_id,
                                     event_ptr->client_id,
                                     event_ptr->data.refresh.refresh_files.file_list_ptr[i]);
              }
              else
              {
                /* If the CSG feature is disabled, return success */
                mmgsdi_status = MMGSDI_SUCCESS;
              }
            }
            else if ((((event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                        MMGSDI_USIM_OPLMNWACT) &&
                       (event_ptr->data.refresh.aid.app_type == MMGSDI_APP_USIM)) ||
                      ((event_ptr->data.refresh.refresh_files.file_list_ptr[i] ==
                        MMGSDI_GSM_OPLMNWACT) &&
                       (event_ptr->data.refresh.aid.app_type == MMGSDI_APP_SIM))) &&
                      (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_OPLMN_LIST_CONFIGURABLE_OTA, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
                      MMGSDI_FEATURE_ENABLED) &&
                     (mmgsdi_util_is_imsi_plmn_cust_plmn(event_ptr->data.refresh.slot)))
            {
              /* Check refresh is happening on EF-OPLMNwACT and builtin PLMN list
                 is present in EFS. If yes, read EFS data and update cache */
              int32  file_size = 0;
              uint8 *file_buffer_ptr = NULL;

              mmgsdi_status = mmgsdi_get_builtin_plmn_list(&file_size, &file_buffer_ptr);
              if(mmgsdi_status == MMGSDI_SUCCESS && file_size > 0 &&
                 file_buffer_ptr != NULL)
              {
                mmgsdi_data_type   data;
                mmgsdi_access_type access;

                data.data_len = file_size;
                data.data_ptr = file_buffer_ptr;
                access.access_method  = MMGSDI_EF_ENUM_ACCESS;
                access.file.file_enum = event_ptr->data.refresh.refresh_files.file_list_ptr[i];

                (void)mmgsdi_cache_write(event_ptr->session_id,
                                         &access,
                                         data,
                                         0,
                                         MMGSDI_DATA_FROM_CARD_COMPLETE);
              }
              else
              {
                mmgsdi_status = mmgsdi_card_init_cache_binary(
                                  event_ptr->session_id,
                                  event_ptr->data.refresh.aid.app_type,
                                  event_ptr->data.refresh.slot,
                                  event_ptr->data.refresh.refresh_files.file_list_ptr[i],
                                  NULL);
              }
              MMGSDIUTIL_TMC_MEM_FREE(file_buffer_ptr);
            }
            else if (event_ptr->data.refresh.refresh_files.file_list_ptr[i] == MMGSDI_TELECOM_ADN)
            {
              if(slot_data_ptr->protocol == MMGSDI_ICC)
              {
                /* Updating FDN status based ADN actrivation status for ICC */
                mmgsdi_status = mmgsdi_icc_update_fdn_status(event_ptr->session_id,
                                                               event_ptr->data.refresh.slot,
                                                               MMGSDI_FDN_SOURCE_REFRESH);
              }
              else
              {
                /* There are UICC cards in the field that update the ED-ADN under
                   TELECOM, even if 31.102 clause 4.7 indicates that a 3G ME shall
                   not access this file. The information is accessible for a 3G ME
                   in EFADN under DFPHONEBOOK. So, in order to avoid failures with
                   the REFRESH, marking mmgsdi_status as success */
                mmgsdi_status = MMGSDI_SUCCESS;
              }
            }
            else
            {
              mmgsdi_status = mmgsdi_card_init_cache_binary(
                                event_ptr->session_id,
                                event_ptr->data.refresh.aid.app_type,
                                event_ptr->data.refresh.slot,
                                event_ptr->data.refresh.refresh_files.file_list_ptr[i],
                                NULL);
            }
            if(mmgsdi_status != MMGSDI_SUCCESS &&
               mmgsdi_status != MMGSDI_NOT_SUPPORTED)
            {
              /*No need to read any further*/
              break;
            }
          }

          if(opl_pnn_ef_refreshed == TRUE)
          {
            /*send EONS cache ready event */
            mmgsdi_eons_send_cache_ready_event(event_ptr->session_id);
          }

          if(eons_ef_refreshed == TRUE)
          {
            mmgsdi_status = mmgsdi_util_get_prov_session_info(event_ptr->session_id,
                                                              NULL,
                                                              NULL,
                                                              &int_app_info_ptr);

            if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
            {
              UIM_MSG_ERR_2("Error obtaining Prov Session Info Status 0x%x Session ID 0x%x",
                            mmgsdi_status, event_ptr->session_id);
              return;
            }

            /*send EONS event only when full service is available */
            mmgsdi_eons_send_event(event_ptr->session_id,
                                   MMGSDI_EONS_TRIGGER_REFRESH_FCN,
                                   csg_ef_refreshed);

            mmgsdi_status = MMGSDI_SUCCESS;
          }

          if(mmgsdi_status == MMGSDI_SUCCESS ||
             mmgsdi_status == MMGSDI_NOT_SUPPORTED)
          {
            pass_fail = TRUE;
          }
          mmgsdi_refresh_send_complete(0,
                                       event_ptr->session_id,
                                       event_ptr->data.refresh.slot,
                                       pass_fail);
          break;
        }

        case MMGSDI_REFRESH_STEERING_OF_ROAMING:
          mmgsdi_status = mmgsdi_refresh_handle_refresh_start_steering(
                                                              event_ptr);
          break;

        default:
          break;
      }
      break;
    default:
      break;
  }
}/*mmgsdi_refresh_process_evt*/


/*===========================================================================
  FUNCTION mmgsdi_refresh_register

  DESCRIPTION:
    This function will let MMGSDI register with itself for refresh fcn

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
   None

  SIDE EFFECTS
    None
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_register(
  mmgsdi_session_id_type session_id,
  boolean                register_for_steering)
{
  mmgsdi_refresh_file_list_type refresh_files = {0};

  if((session_id == mmgsdi_generic_data_ptr->pri_gw_session_id) ||
     (session_id == mmgsdi_generic_data_ptr->sec_gw_session_id) ||
     (session_id == mmgsdi_generic_data_ptr->ter_gw_session_id))
  {
    refresh_files.num_files     = MMGSDI_REFRESH_GW_REG_FILES;
    refresh_files.file_list_ptr = mmgsdi_refresh_gw_reg_file_list_ptr;
  }
  else if((session_id == mmgsdi_generic_data_ptr->pri_1x_session_id) ||
          (session_id == mmgsdi_generic_data_ptr->sec_1x_session_id) ||
          (session_id == mmgsdi_generic_data_ptr->ter_1x_session_id))
  {
    refresh_files.num_files     = MMGSDI_REFRESH_1X_REG_FILES;
    refresh_files.file_list_ptr = mmgsdi_refresh_1x_reg_file_list_ptr;
  }

  if(register_for_steering)
  {
    (void)mmgsdi_session_register_for_refresh_with_steering(
                                      session_id,
                                      refresh_files,
                                      TRUE,
                                      mmgsdi_util_internal_cb,
                                      0);
  }
  else
  {
    (void)mmgsdi_session_register_for_refresh(session_id,
                                              refresh_files,
                                              TRUE,
                                              mmgsdi_util_internal_cb,
                                              0);
  }

  return MMGSDI_SUCCESS;
} /*mmgsdi_refresh_register*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_add_node

  DESCRIPTION:
    This function will add node to the link list

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    Only clients added to the node ptr will get refresh
    events
=============================================================================*/
static void mmgsdi_refresh_add_node(
  mmgsdi_session_id_type                         session_id,
  const mmgsdi_internal_refresh_file_list_type  *refresh_file_list_ptr,
  const boolean                                  notify_pdown,
  mmgsdi_refresh_stage_enum_type                 stage )
{
  mmgsdi_refresh_node_type      *node_ptr      = NULL;
  mmgsdi_refresh_node_type      *temp_ptr      = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_aid_type                session_aid;
  mmgsdi_slot_id_enum_type       slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                          slot_index    = MMGSDI_SLOT_1_INDEX;

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id, &slot_id, NULL,
                                                   NULL, NULL, NULL, NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    UIM_MSG_ERR_0("Refresh: Cannot retrieve slot");
    return;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return;
  }

  if(mmgsdi_refresh_info_ptr[slot_index] == NULL)
  {
    UIM_MSG_ERR_0("Refresh: Cannot add node to null");
    return;
  }

  /*Create a new node*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(node_ptr,
     sizeof(mmgsdi_refresh_node_type));

  if(node_ptr == NULL)
  {
    return;
  }

  (void)memset(&session_aid, 0x00, sizeof(mmgsdi_aid_type));

  /* copy the file list */
  if(refresh_file_list_ptr)
  {
    MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
      refresh_file_list_ptr->file_list_ptr,
      refresh_file_list_ptr->file_path_ptr,
      mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Refresh: Client 0x%x has incompatible file path/enum list",
                    node_ptr->session_id);
      MMGSDIUTIL_TMC_MEM_FREE(node_ptr);
      return;
    }

    if((refresh_file_list_ptr->num_files > 0) &&
       (refresh_file_list_ptr->file_list_ptr))
    {
      /* Malloc and copy file list ptr */
      mmgsdi_status = mmgsdi_util_malloc_and_copy_file_list_ptr(
                        &(node_ptr->refresh_files.file_list_ptr),
                        refresh_file_list_ptr->file_list_ptr,
                        refresh_file_list_ptr->num_files);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE(node_ptr);
        return;
      }

      node_ptr->refresh_files.num_files      = refresh_file_list_ptr->num_files;
      node_ptr->refresh_files.num_file_paths = 0;
    }
    if((refresh_file_list_ptr->num_file_paths > 0) &&
       (refresh_file_list_ptr->file_path_ptr))
    {
      node_ptr->refresh_files.num_file_paths =
        refresh_file_list_ptr->num_file_paths;

      node_ptr->refresh_files.num_files = 0;
      /* Malloc and copy file paths */
      mmgsdi_status = mmgsdi_util_malloc_and_copy_file_path_array(
          &(node_ptr->refresh_files.file_path_ptr),
          refresh_file_list_ptr->file_path_ptr,
          refresh_file_list_ptr->num_file_paths);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE(node_ptr);
        return;
      }
    }
  }
  node_ptr->session_id   = session_id;
  node_ptr->session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;
  node_ptr->next_ptr     = NULL;
  node_ptr->stage        = stage;
  /* We move to WAIT_FOR_CLIENT_CFM state only from WAIT_FOR_OK_TO_INIT state */
  if(stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT)
  {
    node_ptr->pdown_cnf    = notify_pdown;
  }
  else
  {
    node_ptr->pdown_cnf    = FALSE;
  }

  (void) mmgsdi_util_get_client_id(session_id,
                                   &node_ptr->client_id);

  (void) mmgsdi_util_get_session_type(session_id,
                                      &node_ptr->session_type,
                                      &session_aid);
  node_ptr->app_type     = session_aid.app_type;

  /* Once rsp_received==TRUE for every node then we move onto the next stage.
     Therefore, if rsp_received is set to TRUE here then as soon as the first
     response is received we move onto the next stage.
     However, in the following cases we want to wait for a response from all
     nodes before moving and therefore rsp_received=FALSE. The last condition
     ensures for non-prov we wait for a response from all nodes. */
  if(stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT           ||
     stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN            ||
     stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH ||
     (!mmgsdi_util_is_prov_session(node_ptr->session_type) &&
     stage == MMGSDI_REFRESH_STAGE_START))
  {
    node_ptr->rsp_received = FALSE;
  }
  else
  {
    /* No need to wait for any responses, move to next stage once
       one response is recieved */
    node_ptr->rsp_received = TRUE;
  }

  temp_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;

  if(temp_ptr == NULL)
  {
    mmgsdi_refresh_info_ptr[slot_index]->node_ptr = node_ptr;
    return;
  }

  while(temp_ptr->next_ptr)
  {
    temp_ptr= temp_ptr->next_ptr;
  }
  temp_ptr->next_ptr = node_ptr;
}/* mmgsdi_refresh_add_node */


/*=============================================================================
  FUNCTION: mmgsdi_refresh_malloc_and_convert_file_path

  DESCRIPTION:
    Mallocs the file path and Converts the File path into a UIM item and
    then to MMGSDI enum

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_malloc_and_convert_file_path(
  const mmgsdi_refresh_card_req_type           *req_ptr,
  uint8                                         slot_index,
  mmgsdi_internal_refresh_file_list_type       *refresh_files_ptr
)
{
  mmgsdi_file_enum_type       * mmgsdi_files_ptr      = NULL;
  mmgsdi_path_type            * mmgsdi_file_path_ptr  = NULL;
  mmgsdi_return_enum_type       mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_app_enum_type          mmgsdi_app_type       = MMGSDI_APP_NONE;
  uint8                         i                     = 0 ;
  uint8                         num_files             = 0;

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL_3(req_ptr, refresh_files_ptr, mmgsdi_refresh_info_ptr[slot_index]);

  /* Allocate buffes for UIM items */
   MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_files_ptr,
     ((MMGSDI_MAX_FILE_CHANGE_NOTIFICATIONS)*sizeof(mmgsdi_file_enum_type)));
  if (mmgsdi_files_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* Convert file data into file path array */
  mmgsdi_status = mmgsdi_util_convert_to_refresh_file_paths(
                    int32touint32(req_ptr->refresh_files.data_len),
                    req_ptr->refresh_files.data_ptr,
                    &mmgsdi_file_path_ptr,
                    &num_files);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_file_path_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_files_ptr);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(refresh_files_ptr->file_list_ptr,
    (sizeof(mmgsdi_file_enum_type)*num_files));

  if(refresh_files_ptr->file_list_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_file_path_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_files_ptr);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_malloc_and_copy_file_path_array(
    &(refresh_files_ptr->file_path_ptr),
    mmgsdi_file_path_ptr,
    num_files);

  /* Free the memory allocated in mmgsdi_util_convert_to_refresh_file_paths */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_file_path_ptr);


  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files_ptr->file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_files_ptr);
    return mmgsdi_status;
  }

  /* There could be refresh happened with ISIM app and ISIM file list */
  if((mmgsdi_refresh_info_ptr[slot_index]->aid.app_type == MMGSDI_APP_USIM) ||
     (mmgsdi_refresh_info_ptr[slot_index]->aid.app_type == MMGSDI_APP_CSIM) ||
     ((mmgsdi_refresh_info_ptr[slot_index]->aid.app_type == MMGSDI_APP_UNKNOWN) &&
        mmgsdi_util_is_isim_aid(mmgsdi_refresh_info_ptr[slot_index]->aid.aid.data_ptr,
                                mmgsdi_refresh_info_ptr[slot_index]->aid.aid.data_len)))
  {
    mmgsdi_app_type = mmgsdi_refresh_info_ptr[slot_index]->aid.app_type;
  }

  for(i=0; i<num_files && i<MMGSDI_MAX_FILE_CHANGE_NOTIFICATIONS;i++)
  {
    mmgsdi_status = mmgsdi_file_get_file_from_path(mmgsdi_app_type,
                                                   refresh_files_ptr->file_path_ptr[i],
                                                   &mmgsdi_files_ptr[i]);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Refresh: Could not convert from path to uim file");
      refresh_files_ptr->file_list_ptr[i] = MMGSDI_NO_FILE_ENUM;
    }
    else
    {
      refresh_files_ptr->file_list_ptr[i] = mmgsdi_files_ptr[i];
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_files_ptr);
  /* This will indicate the number of file enums */
  refresh_files_ptr->num_files = i;
  refresh_files_ptr->num_file_paths = num_files;
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_malloc_and_convert_file_path*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_CHECK_PATH_FOR_SESSION

  DESCRIPTION
    This function checks if the specified path belongs to the session type
    that the client registered on.


  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_check_path_for_session(
  const mmgsdi_session_info_type          * session_ptr,
  const mmgsdi_path_type                  * files_path_ptr
)
{
  uint16                   path_buf_df_gsm[MMGSDI_REFRESH_MIN_PATH_LEN]  =
                               {0x3F00, 0x7F20};
  uint16                   path_buf_df_cdma[MMGSDI_REFRESH_MIN_PATH_LEN] =
                               {0x3F00, 0x7F25};
  uint16                   path_buf_adf[MMGSDI_REFRESH_MIN_PATH_LEN]  =
                               {0x3F00, 0x7FFF};
  uint16                   path_buf_mf        = 0x3F00;
  uint32                   path_len           = 0;
  mmgsdi_app_enum_type     app_type           = MMGSDI_APP_NONE;
  mmgsdi_return_enum_type  mmgsdi_status      = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL_2(session_ptr, files_path_ptr);

  if((session_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
     (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] == NULL) ||
     (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr == NULL))
  {
    UIM_MSG_ERR_1("Invalid channel info: channel_index: 0x%x",
                  session_ptr->channel_info_index);
    return MMGSDI_ERROR;
  }

  app_type = mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data.app_type;
  path_len = int32touint32(files_path_ptr->path_len);

  if((path_len < MMGSDI_REFRESH_MIN_PATH_LEN) || (path_len > MMGSDI_MAX_PATH_LEN))
  {
    /*Skip this client - client has no interest in this file or
      client has not registered for any files*/
    UIM_MSG_ERR_1("Invalid path length: 0x%x", path_len);
    return MMGSDI_ERROR;
  }

  switch(session_ptr->session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_GW_PROV_TER_SESSION:
      if (((app_type == MMGSDI_APP_SIM) &&
           (memcmp(files_path_ptr->path_buf,
                   path_buf_df_gsm,
                   MIN(path_len,MMGSDI_REFRESH_MIN_PATH_LEN) * sizeof(uint16)) == 0)) ||
          ((app_type == MMGSDI_APP_USIM) &&
           (memcmp(files_path_ptr->path_buf,
                   path_buf_adf,
                   MIN(path_len,MMGSDI_REFRESH_MIN_PATH_LEN) * sizeof(uint16)) == 0)))
      {
        mmgsdi_status = MMGSDI_SUCCESS;
      }
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
    case MMGSDI_1X_PROV_SEC_SESSION:
    case MMGSDI_1X_PROV_TER_SESSION:
      if (((app_type == MMGSDI_APP_RUIM) &&
           (memcmp(files_path_ptr->path_buf,
                   path_buf_df_cdma,
                   MIN(path_len,MMGSDI_REFRESH_MIN_PATH_LEN) * sizeof(uint16)) == 0)) ||
          ((app_type == MMGSDI_APP_CSIM) &&
           (memcmp(files_path_ptr->path_buf,
                   path_buf_adf,
                   MIN(path_len,MMGSDI_REFRESH_MIN_PATH_LEN) * sizeof(uint16)) == 0)))
      {
        mmgsdi_status = MMGSDI_SUCCESS;
      }
      break;

    case MMGSDI_CARD_SESSION_SLOT_1:
    case MMGSDI_CARD_SESSION_SLOT_2:
    case MMGSDI_CARD_SESSION_SLOT_3:
      /* For a card session type, only check path */
      if (memcmp(files_path_ptr->path_buf,
                 &path_buf_mf,
                 MIN(path_len,MMGSDI_REFRESH_MIN_PATH_LEN)) == 0)
      {
        mmgsdi_status = MMGSDI_SUCCESS;
      }
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_1:
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      /* For a non-prov session type, only check path */
      if (memcmp(files_path_ptr->path_buf,
                 path_buf_adf,
                 MIN(path_len,MMGSDI_REFRESH_MIN_PATH_LEN) * sizeof(uint16)) == 0)
      {
        mmgsdi_status = MMGSDI_SUCCESS;
      }
      break;

    default:
      UIM_MSG_ERR_1("Invalid session type: 0x%x", session_ptr->session_type);
      break;
  }

  return mmgsdi_status;
} /* mmgsdi_refresh_check_path_for_session */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_REMOVE_DUPLICATE_PATHS

  DESCRIPTION
    This function checks for duplicates in the both the files lists passed.
    the final concatenated list is updated in the client specific files
    pointer passed. This should affect only clients who have registered
    using both the reg-all & by-path options. Duplicate can mainly arise
    because of way files are associated to each session type in case of
    reg-all clients.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    Builds a success/failure response to the caller

  SIDE EFFECTS
    None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_remove_duplicate_paths(
  mmgsdi_internal_refresh_file_list_type           * client_spc_files_ptr,
  const mmgsdi_internal_refresh_file_list_type     * files_by_path_ptr,
  const mmgsdi_internal_refresh_file_list_type     * reg_all_path_ptr
)
{
  uint8                                          i,j,k            = 0;
  uint32                                         total_num_files  = 0;
  uint32                                         path_len         = 0;
  mmgsdi_return_enum_type                        mmgsdi_status    = MMGSDI_ERROR;
  const mmgsdi_internal_refresh_file_list_type * max_filelist_ptr = NULL;
  const mmgsdi_internal_refresh_file_list_type * min_filelist_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_3(client_spc_files_ptr, files_by_path_ptr,
                              reg_all_path_ptr);

  total_num_files = files_by_path_ptr->num_file_paths +
                      reg_all_path_ptr->num_file_paths;

  if (files_by_path_ptr->num_file_paths >= reg_all_path_ptr->num_file_paths)
  {
    max_filelist_ptr = files_by_path_ptr;
    min_filelist_ptr = reg_all_path_ptr;
  }
  else
  {
    max_filelist_ptr = reg_all_path_ptr;
    min_filelist_ptr = files_by_path_ptr;
  }

  /* Interate through the list to find duplicates & get a final count */
  for (i = 0; i < min_filelist_ptr->num_file_paths; i++)
  {
    for (j = 0; j < max_filelist_ptr->num_file_paths; j++)
    {
      if (min_filelist_ptr->file_path_ptr[i].path_len ==
          max_filelist_ptr->file_path_ptr[j].path_len)
      {
        path_len = int32touint32(max_filelist_ptr->file_path_ptr[j].path_len);
        if (!memcmp(min_filelist_ptr->file_path_ptr[i].path_buf,
                    max_filelist_ptr->file_path_ptr[j].path_buf,
                    MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
        {
          total_num_files--;
          break;
        }
      }
    }
  }

  if(total_num_files == 0)
  {
    /* This should not happen at this point, just return */
    return mmgsdi_status;
  }

  /* Allocate memory for the final file list. Note: file_path_ptr allocated
     here will be used by individual nodes and freed at the end of refresh*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    client_spc_files_ptr->file_path_ptr,
    (sizeof(mmgsdi_path_type)* total_num_files));

  if(client_spc_files_ptr->file_path_ptr == NULL)
  {
    /* mmgsdi_refresh_info_ptr will be cleaned up later */
    UIM_MSG_ERR_0("Refresh: Malloc Error for Refresh File Path Ptr");
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Set file enum list pointer to null */
  client_spc_files_ptr->file_list_ptr = NULL;
  client_spc_files_ptr->num_files     = 0;

  /* Init file path buffer */
  memset(client_spc_files_ptr->file_path_ptr, 0x00,
         sizeof(mmgsdi_path_type)* total_num_files);

  /* Copy the max list first */
  k = 0;
  for (i = 0; i < max_filelist_ptr->num_file_paths; i++)
  {
    if(k < total_num_files)
    {
      path_len = int32touint32(max_filelist_ptr->file_path_ptr[i].path_len);
      /* Copy file path */
      client_spc_files_ptr->file_path_ptr[k].path_len = path_len;
      mmgsdi_memscpy(client_spc_files_ptr->file_path_ptr[k].path_buf,
                     sizeof(client_spc_files_ptr->file_path_ptr[k].path_buf),
                     max_filelist_ptr->file_path_ptr[i].path_buf,
                     path_len * sizeof(uint16));
      k++;
    }
  }

  /* Then compare with min list & add if needed */
  for (i = 0; i < min_filelist_ptr->num_file_paths; i++)
  {
    boolean found = FALSE;
    path_len = int32touint32(min_filelist_ptr->file_path_ptr[i].path_len);

    for (j = 0; j < max_filelist_ptr->num_file_paths; j++)
    {
      if (min_filelist_ptr->file_path_ptr[i].path_len ==
          max_filelist_ptr->file_path_ptr[j].path_len)
      {
        if (!memcmp(min_filelist_ptr->file_path_ptr[i].path_buf,
                    max_filelist_ptr->file_path_ptr[j].path_buf,
                    MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
        {
          /* Found a duplicate */
          found = TRUE;
          break;
        }
      }
    }

    if (!found)
    {
      /* Add it to the end of max list */
      if(k < total_num_files)
      {
        client_spc_files_ptr->file_path_ptr[k].path_len = path_len;
        mmgsdi_memscpy(client_spc_files_ptr->file_path_ptr[k].path_buf,
                       sizeof(client_spc_files_ptr->file_path_ptr[k].path_buf),
                       min_filelist_ptr->file_path_ptr[i].path_buf,
                       path_len * sizeof(uint16));
        k++;
      }
    }
  }

  /* Update the number of files copied so far */
  client_spc_files_ptr->num_file_paths = k;

  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_remove_duplicate_paths */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_BUILD_NODE_LIST

  DESCRIPTION
    This function builds a node if there is a match between files in FCN
    list and the files that the clients have request for

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_build_node_list(
  const mmgsdi_session_info_type               *session_ptr,
  const mmgsdi_internal_refresh_file_list_type *refresh_files_ptr
)
{
  boolean                        is_reg_by_path           = FALSE;
  boolean                        is_reg_all               = FALSE;
  uint8                          num_files                = 0;
  uint8                          i                        = 0;
  uint8                          j                        = 0;
  uint32                         k                        = 0;
  uint32                         path_len                 = 0;
  mmgsdi_return_enum_type        mmgsdi_status            = MMGSDI_ERROR;

  mmgsdi_internal_refresh_file_list_type  client_spc_refresh_files = {0};
  mmgsdi_internal_refresh_file_list_type  refresh_files_reg_all    = {0};
  mmgsdi_internal_refresh_file_list_type  refresh_files_by_path    = {0};

  MMGSDIUTIL_RETURN_IF_NULL_2(session_ptr, refresh_files_ptr);

  /* Check if client registered using file path or enums */
  is_reg_by_path = session_ptr->refresh_data.reg_by_path;

  /* Check if client registered for all files */
  is_reg_all = session_ptr->refresh_data.reg_all_files;

  /* Find the total number of files possbile for all reg types */
  if (is_reg_all)
  {
    MMGSDIUTIL_RETURN_IF_NULL(refresh_files_ptr->file_path_ptr);

    /* Find out how many files are of interest to the client out of all the
       files that have been refreshed */
    for(j = 0; j < refresh_files_ptr->num_file_paths ; j++)
    {
      if(refresh_files_ptr->file_path_ptr[j].path_len == 0)
      {
        UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                      j);
        return MMGSDI_ERROR;
      }

      /* Compare the path depending upon the sesion type */
      if (mmgsdi_refresh_check_path_for_session(
            session_ptr,
            &refresh_files_ptr->file_path_ptr[j]) == MMGSDI_SUCCESS)
      {
        num_files++;
      }
    } /*end for j*/
  }

  if(!is_reg_by_path)
  {
    MMGSDIUTIL_RETURN_IF_NULL(refresh_files_ptr->file_list_ptr);

    /*Find out how many files are in intersection set*/
    if(session_ptr->refresh_data.refresh_files.num_files > 0)
    {
      for(j=0;j<refresh_files_ptr->num_files;j++)
      {
        for(k=0;k<session_ptr->refresh_data.refresh_files.num_files;k++)
        {
          if(refresh_files_ptr->file_list_ptr[j] ==
             session_ptr->refresh_data.refresh_files.file_list_ptr[k])
          {
            num_files++;
            break;
          }
        }/*end for k*/
      }/*end for j*/
    }
  }
  else /* client has registered by file path */
  {
    MMGSDIUTIL_RETURN_IF_NULL(refresh_files_ptr->file_path_ptr);

    /* Find out how many files are in intersection set. This will help us in
      determining how much memory should we allocate for file buffer */
    if(session_ptr->refresh_data.refresh_files.num_file_paths > 0)
    {
      for(j = 0; j < refresh_files_ptr->num_file_paths ; j++)
      {
        if(refresh_files_ptr->file_path_ptr[j].path_len == 0)
        {
          UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                        j);
          return MMGSDI_ERROR;
        }

        for(k=0;k<session_ptr->refresh_data.refresh_files.num_file_paths;k++)
        {
          /* compare path length */
          if(refresh_files_ptr->file_path_ptr[j].path_len ==
             session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_len)
          {
            /* compare path buffers */
            path_len = int32touint32(refresh_files_ptr->file_path_ptr[j].path_len);
            if(!memcmp(refresh_files_ptr->file_path_ptr[j].path_buf,
                session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_buf,
                MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16))
              )
            {
              num_files++;
              break;
            }
          }
        }/*end for k*/
      }/*end for j*/
    }
  }

  if(num_files == 0)
  {
    /*Skip this client - client has no interest in this file or
      client has not registered for any files*/
    return MMGSDI_SUCCESS;
  }

  /* Find the relevant files for reg all type */
  if (is_reg_all)
  {
    /*Allocate memory for the file list*/
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      refresh_files_reg_all.file_path_ptr,
      (sizeof(mmgsdi_path_type)* num_files));

    if(refresh_files_reg_all.file_path_ptr == NULL)
    {
      /*mmgsdi_refresh_info_ptr will be cleaned up later*/
      return MMGSDI_ERROR;
    }

    /* Set file enum list pointer to null */
    refresh_files_reg_all.file_list_ptr = NULL;

    /* mem init file path buffer to zero */
    memset(refresh_files_reg_all.file_path_ptr, 0x00,
      sizeof(mmgsdi_path_type)* num_files);

    i = 0;
    /*Copy the relevant files over now*/
    for(j = 0;j < refresh_files_ptr->num_file_paths; j++)
    {
      if(refresh_files_ptr->file_path_ptr[j].path_len == 0)
      {
        UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                      j);
        /* Free previous memory allocations */
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files_reg_all.file_path_ptr);
        return MMGSDI_ERROR;
      }

      /* For all files case, check path depending upon the session type */
      if (mmgsdi_refresh_check_path_for_session(
            session_ptr,
            &refresh_files_ptr->file_path_ptr[j]) == MMGSDI_SUCCESS)
      {
        if(i < num_files)
        {
          path_len = int32touint32(refresh_files_ptr->file_path_ptr[j].path_len);

          /* Copy file path */
          refresh_files_reg_all.file_path_ptr[i].path_len =
            refresh_files_ptr->file_path_ptr[j].path_len;
          mmgsdi_memscpy(refresh_files_reg_all.file_path_ptr[i].path_buf,
                         sizeof(refresh_files_reg_all.file_path_ptr[i].path_buf),
                         refresh_files_ptr->file_path_ptr[j].path_buf,
                         path_len * sizeof(uint16));
          i++;
        }
      }
    }
    refresh_files_reg_all.num_file_paths = i;
  }

  if(!is_reg_by_path)
  {
    /*Allocate memory for the file list*/
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      client_spc_refresh_files.file_list_ptr,
      (sizeof(mmgsdi_file_enum_type)* num_files));

    if(client_spc_refresh_files.file_list_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    /* set file path pointer to null */
    client_spc_refresh_files.file_path_ptr = NULL;

    i = 0;
    /*Copy the files over now*/
    for(j=0;j<refresh_files_ptr->num_files;j++)
    {
      for(k=0;k<session_ptr->refresh_data.refresh_files.num_files;k++)
      {
        if(refresh_files_ptr->file_list_ptr[j] ==
           session_ptr->refresh_data.refresh_files.file_list_ptr[k])
        {
          if( i < num_files )
          {
            client_spc_refresh_files.file_list_ptr[i] =
              refresh_files_ptr->file_list_ptr[j];
            i++;
          }
          break;
        }
      }/*end for*/
    }/*end for*/
    client_spc_refresh_files.num_file_paths = 0;
    client_spc_refresh_files.num_files      = num_files;
  } /* if(!is_reg_by_path) */
  else /* client registered by file path */
  {
    /*Allocate memory for the file list*/
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      refresh_files_by_path.file_path_ptr,
      (sizeof(mmgsdi_path_type)* num_files));

    if(refresh_files_by_path.file_path_ptr == NULL)
    {
      /* Free previous memory allocations, if any */
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files_reg_all.file_path_ptr);

      return MMGSDI_ERROR;
    }

    /* Set file enum list pointer to null */
    refresh_files_by_path.file_list_ptr = NULL;

    /* mem init file path buffer to zero */
    memset(refresh_files_by_path.file_path_ptr, 0x00,
      sizeof(mmgsdi_path_type)* num_files);

    i = 0;
    /*Copy the files over now*/
    for(j = 0;j < refresh_files_ptr->num_file_paths; j++)
    {
      if(refresh_files_ptr->file_path_ptr[j].path_len == 0)
      {
        UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                      j);
        /* Free previous memory allocations */
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files_by_path.file_path_ptr);

        /* Free previous memory allocations, if any */
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files_reg_all.file_path_ptr);

        return MMGSDI_ERROR;
      }

      for(k=0;k<session_ptr->refresh_data.refresh_files.num_file_paths;k++)
      {
        /* compare path length */
        if(refresh_files_ptr->file_path_ptr[j].path_len ==
           session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_len)
        {
          /* compare path buffers */
          path_len = int32touint32(refresh_files_ptr->file_path_ptr[j].path_len);
          if(!memcmp(
              refresh_files_ptr->file_path_ptr[j].path_buf,
              session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_buf,
              MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
          {
            if(i < num_files)
            {
              /* Copy file path */
              refresh_files_by_path.file_path_ptr[i].path_len =
                refresh_files_ptr->file_path_ptr[j].path_len;
              mmgsdi_memscpy(refresh_files_by_path.file_path_ptr[i].path_buf,
                             sizeof(refresh_files_by_path.file_path_ptr[i].path_buf),
                             refresh_files_ptr->file_path_ptr[j].path_buf,
                             path_len* sizeof(uint16));
              i++;
            }
            break;
          }
        }
      }/* end for k */
    }/* end for j */
    refresh_files_by_path.num_file_paths = i;
    refresh_files_by_path.num_files      = 0;
  } /* end of else is_reg_by_path */

  /* If both reg all & reg by path are enabled for the client, we need to make sure
     that we dont send duplicate files in the entire list */
  if (is_reg_all || is_reg_by_path)
  {
    mmgsdi_status = mmgsdi_refresh_remove_duplicate_paths(&client_spc_refresh_files,
                                                          &refresh_files_by_path,
                                                          &refresh_files_reg_all);

    /* Free temporary memory allocation */
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files_by_path.file_path_ptr);

    /* Free temporary memory allocation, if any */
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files_reg_all.file_path_ptr);

    /* Proceed with adding the node only on success */
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  /*Add Node to Linked List*/
  mmgsdi_refresh_add_node(session_ptr->session_id,
                          &client_spc_refresh_files,
                          FALSE,
                          MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_spc_refresh_files.file_list_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_spc_refresh_files.file_path_ptr);

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_build_node_list*/


/*=============================================================================
  FUNCTION: mmgsdi_refresh_wait_for_session_init

  DESCRIPTION:
    Determines if we need to wait for Provisioning session initialization
    to complete before the terminal response is sent to the card

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    boolean : Wait for Client Init

  SIDE EFFECTS:
    Could potentially delay init process
=============================================================================*/
static boolean mmgsdi_refresh_wait_for_session_init(
  const mmgsdi_client_req_extra_info_type *extra_param_ptr,
  uint8                                    slot_index)
{
  mmgsdi_session_info_type *session_ptr  = NULL;
  uint8                     i            = 0;
  uint8                     j            = 0;
  mmgsdi_len_type           num_sessions = 0;

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return FALSE;
  }
  if(extra_param_ptr == NULL ||
     mmgsdi_refresh_info_ptr[slot_index] == NULL)
  {
    UIM_MSG_ERR_0("Parameters are NULL");
    return FALSE;
  }

  if(extra_param_ptr->refresh_data.mode == MMGSDI_REFRESH_NAA_FCN ||
     extra_param_ptr->refresh_data.mode == MMGSDI_REFRESH_RESET   ||
     extra_param_ptr->refresh_data.mode == MMGSDI_REFRESH_RESET_AUTO)
  {
    return FALSE;
  }

  for(i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if(mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[i]->num_session;

    for(j = 0; (num_sessions > 0) && (j < MMGSDI_MAX_SESSION_INFO); j++)
    {
      session_ptr =
        mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

      if(session_ptr == NULL)
      {
        /*Invalid Session, Continue to next*/
        continue;
      }
      num_sessions--;

      session_ptr = mmgsdi_util_cmp_aid_get_session_ptr(
                      i,
                      j,
                      &mmgsdi_refresh_info_ptr[slot_index]->aid,
                      mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                      TRUE);

      if(session_ptr == NULL)
      {
        continue;
      }

      /* Init complete makes sense only for Provisioning sessions */
      if((mmgsdi_util_is_prov_session(session_ptr->session_type)) &&
         (session_ptr->notify_init))
      {
        if(!session_ptr->init_complete)
        {
          UIM_MSG_HIGH_2("Refresh: init not completed yet for Client ID = 0x%x, Session ID = 0x%x",
                         ((mmgsdi_client_id_reg_table[i]->client_id>>32)&0xFFFFFFFF),
                         (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_id & 0xFFFFFFFF));
          mmgsdi_util_print_client_name(mmgsdi_client_id_reg_table[i]->client_id);
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}/*mmgsdi_refresh_wait_for_session_init*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_STATE_RESET

  DESCRIPTION:
    Starts the reset process when mode is  NAA_APP_RESET or 3G_SESSION_RESET
    1. If mode is NAA_APP_RESET, send CARD STATUS request to card, on reception
       of response, deactivate the app and then activate again.
    2. If mode is 3G Session reset, send CARD STATUS req to card, on reception
       of response, skip deactivation and activate the app.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_start_reset(
  mmgsdi_slot_data_type             *slot_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr,
  uint8                              slot_index)
{
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_channel_info_type *channel_info_ptr = NULL;
  mmgsdi_cmd_enum_type      orig_req         = MMGSDI_CARD_STATUS_REQ;

  UIM_MSG_HIGH_1("FUNCTION mmgsdi_refresh_start_reset for slot_index: 0x%x",
                 slot_index);

  MMGSDIUTIL_RETURN_IF_NULL_2(extra_param_ptr, slot_ptr);

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_CAN_NOT_REFRESH;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_refresh_info_ptr[slot_index],
                              mmgsdi_refresh_info_ptr[slot_index]->node_ptr);

  /*Get channel info ptr*/
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    mmgsdi_refresh_info_ptr[slot_index]->node_ptr->session_id,
                    NULL,
                    &channel_info_ptr);

  /*Set channel state to READY if the current state is READY_WITH_STREAMING.
    Channel state was set to READY_WITH_STREAMIG when APDU streaming was done,
    During refresh we set it back to READY so that if recovery happens after
    refresh channel should be recovered*/

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status == MMGSDI_SUCCESS && channel_info_ptr != NULL &&
     channel_info_ptr->channel_state == MMGSDI_CHANNEL_STATE_APDU_STREAMING_DONE)
  {
    channel_info_ptr->channel_state = MMGSDI_CHANNEL_STATE_READY;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_APP_RESET)
  {
    orig_req = MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ;
  }

  mmgsdi_status = mmgsdi_session_build_status_terminate(
                    mmgsdi_refresh_info_ptr[slot_index]->node_ptr->session_id,
                    mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                    orig_req,
                    mmgsdi_internal_refresh_cb,
                    0);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Refresh: Could not deactivate default app");
    extra_param_ptr->refresh_data.send_evt = TRUE;
    extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
    slot_ptr->refresh_state             = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_CAN_NOT_REFRESH;
  }
  else
  {
    /* default provisioning app is needed, MMGSDI will deactivate app */
    extra_param_ptr->refresh_data.send_evt = TRUE;
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
    extra_param_ptr->refresh_data.app_type = MMGSDI_REFRESH_START_PROV_APP;
  }

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_start_reset*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_START_APP_RESET_AUTO

  DESCRIPTION:
    Starts the app deactivate process

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_start_app_reset_auto(
  mmgsdi_slot_data_type             *slot_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr,
  uint8                              slot_index)
{
  mmgsdi_session_deact_req_type      msg;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_int_app_info_type          *app_info_ptr    = NULL;

  UIM_MSG_HIGH_1("FUNCTION mmgsdi_refresh_start_app_reset_auto for slot_index: 0x%x",
                 slot_index);

  MMGSDIUTIL_RETURN_IF_NULL_2(extra_param_ptr, slot_ptr);

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  memset(&msg, 0x00, sizeof(mmgsdi_session_deact_req_type));

  /* Retrieve the original switch/deactivate request */
  mmgsdi_memscpy(&msg.request_header,
                 sizeof(msg.request_header),
                 &mmgsdi_refresh_info_ptr[slot_index]->request_header,
                 sizeof(mmgsdi_request_header_type));

  /* First do a deactivate */
  msg.request_header.request_type = MMGSDI_SESSION_DEACTIVATE_REQ;
  msg.request_header.orig_request_type = MMGSDI_SESSION_DEACTIVATE_REQ;

  /* Lets reset the deactivation_in_progres flag so that any activation 
     request comnig after this deactivation is processed, is handled
     without any issue by MMGSDI. We deliberately do not wait until after
     sending out the SESSION_CHANGED_EVT to avoid race conditions */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                      msg.request_header.session_id,
                      NULL,
                      NULL,
                      &app_info_ptr);
  if((mmgsdi_status == MMGSDI_SUCCESS) && (app_info_ptr != NULL))
  {
    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT
    app_info_ptr->deactivation_in_progress = FALSE;
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT
  }

  mmgsdi_status = mmgsdi_session_deactivate_start(&msg);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Refresh: Could not deactivate app");
    extra_param_ptr->refresh_data.send_evt = FALSE;
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return mmgsdi_status;
  }
  else
  {
    extra_param_ptr->refresh_data.send_evt = FALSE;
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;

    /* Update the session closed cause. The session changed event
       broadcasts the cause */
    mmgsdi_util_update_session_closed_cause(msg.request_header.session_id,
                                            MMGSDI_SESSION_CLOSED_CAUSE_CLIENT_REQUEST);
  }

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_start_app_reset_auto*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_START_FULL_RESET

  DESCRIPTION:
    Starts the reset process -
    1. Close all sessions
    2. Send start event

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_start_full_reset(
  mmgsdi_slot_data_type               *slot_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr,
  mmgsdi_refresh_mode_enum_type        refresh_mode,
  boolean                              switch_protocol,
  uint8                                slot_index)
{
  mmgsdi_return_enum_type        mmgsdi_status         = MMGSDI_ERROR;
  uim_protocol_type              uim_protocol          = UIM_NO_SUCH_MODE;
  boolean                        card_reset_req        = FALSE;

  UIM_MSG_HIGH_1("FUNCTION mmgsdi_refresh_start_full_reset for slot_index: 0x%x",
                 slot_index);

  MMGSDIUTIL_RETURN_IF_NULL_2(slot_ptr, extra_param_ptr);

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  /* Get the card protocol */
  uim_protocol = mmgsdi_util_get_uim_protocol(
                   mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id);

  /* If the card is an UICC and there is a valid prov. app open on the card,
     need to send STATUS command (with P1 ='02') */
  if((uim_protocol == UIM_UICC) && (mmgsdi_refresh_info_ptr[slot_index]->node_ptr != NULL))
  {
      mmgsdi_status = mmgsdi_session_build_status_terminate(
                        mmgsdi_refresh_info_ptr[slot_index]->node_ptr->session_id,
                        mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                        MMGSDI_CARD_RESET_REQ,
                        mmgsdi_internal_refresh_cb,
                        0);
  }
  else
  {
    /* Update protocol to UICC if we have BAD protocol in globals or if the
       refresh is REFRESH_RESET_AUTO (means power_up).
       NOTE: UIMDRV will implemnt logic to power_up the card corresponding
       to protocol in NV 896 irrespective of protocol given by MMGSDI */
    if (uim_protocol == UIM_NO_SUCH_MODE || refresh_mode == MMGSDI_REFRESH_RESET_AUTO)
    {
      uim_protocol = UIM_UICC;
    }

    if(switch_protocol)
    {
      UIM_MSG_HIGH_0("Attempting to switch protocol during Refresh");

      switch(uim_protocol)
      {
        case UIM_UICC:
          uim_protocol = UIM_ICC;
          break;
        case UIM_ICC:
          /* Fall Through. Only switch for UICC cards */
        default:
          UIM_MSG_ERR_1("Switching on illegal protocol: 0x%x", uim_protocol);
          break;
      }
    }
    /* Build Reset command message */
    mmgsdi_status = mmgsdi_gen_build_card_reset(
                      mmgsdi_refresh_info_ptr[slot_index]->request_header.client_id,
                      mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                      MMGSDI_CARD_RESET_REQ,
                      uim_protocol,
                      MMGSDI_RESET_CAUSE_REFRESH,
                      mmgsdi_internal_refresh_cb,
                      0);

    card_reset_req = TRUE;
  }

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Could not build a Card Reset command");
    extra_param_ptr->refresh_data.send_evt = TRUE;
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_CAN_NOT_REFRESH;
  }

  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
  extra_param_ptr->refresh_data.send_evt = TRUE;

  /*Send the REFRESH START event first*/
  (void)mmgsdi_refresh_send_evt(&extra_param_ptr->refresh_data,
                                &mmgsdi_refresh_info_ptr[slot_index]->request_header);

  extra_param_ptr->refresh_data.send_evt = FALSE;

  /* Reset Session and application data for the slot */
  if(card_reset_req)
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

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_start_full_reset*/


/* =============================================================================
  FUNCTION:      MMGSDI_CLEAR_REFRESH_DATA_INFO

  DESCRIPTION:
    This function will clear the global mmgsdi_refresh_data_info

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    Uses critical section
=============================================================================*/
void mmgsdi_clear_refresh_data_info(
  void
)
{
  /* Protect access to global mmgsdi_refresh_data_info */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_refresh_data_info.refresh_file_list.file_path_ptr);
  memset(&mmgsdi_refresh_data_info, 0x00, sizeof(mmgsdi_refresh_data_info_type));
  /* Reset the refresh mode to MMGSDI_REFRESH_MODE_ENUM_MAX */
  mmgsdi_refresh_data_info.refresh_mode = MMGSDI_REFRESH_MODE_ENUM_MAX;

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
} /* mmgsdi_clear_refresh_data_info */


/* =============================================================================
  FUNCTION:      MMGSDI_COPY_INFO_TO_REFRESH_DATA_INFO

  DESCRIPTION:
    This function will copy all the files related data to mmgsdi_refresh_data_info
    if the refresh mode is either FCN or INIT_FCN and the number of files is
    more than zero. Additionally it also copies the refresh mode, the related
    aid and the slot information.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    Uses critical section
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_copy_info_to_refresh_data_info(
  const mmgsdi_refresh_req_type                 *refresh_req_ptr,
  mmgsdi_slot_id_enum_type                       slot_id)
{
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type  *ch_zero_app_ptr = NULL;

  if(refresh_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("NULL Pointer");
    return MMGSDI_ERROR;
  }

  /* Clear any prev data if it is still around */
  mmgsdi_clear_refresh_data_info();

  /* Protect access to global mmgsdi_refresh_data_info_ptr */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    /* Store the refresh mode */
    mmgsdi_refresh_data_info.refresh_mode = refresh_req_ptr->refresh.card_req.refresh_mode;
    /* Store the slot id */
    mmgsdi_refresh_data_info.slot_id = slot_id;

    /* If the REFRESH mode is RESET then we dont need an aid and associate
       the default channel */
    if(refresh_req_ptr->refresh.card_req.refresh_mode != MMGSDI_REFRESH_RESET)
    {
      /* Store the AID info. When the card does not send AID in the REFRESH,
         get the app that's on the default channel (0) */
      if(refresh_req_ptr->refresh.card_req.aid.aid.data_len == 0)
      {
        mmgsdi_status = mmgsdi_util_get_channel_app(slot_id,
                                                    MMGSDI_DEFAULT_CHANNEL,
                                                    &ch_zero_app_ptr);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          break;
        }
        /* Populate global with app data so we know whether it is SIM or RUIM */
        mmgsdi_memscpy(&mmgsdi_refresh_data_info.app_data,
                       sizeof(mmgsdi_refresh_data_info.app_data),
                       &ch_zero_app_ptr->app_data,
                       sizeof(mmgsdi_aid_type));
      }
      else
      {
        /* Populate global with aid passed into the refresh req */
        mmgsdi_memscpy(&mmgsdi_refresh_data_info.app_data,
                       sizeof(mmgsdi_refresh_data_info.app_data),
                       &refresh_req_ptr->refresh.card_req.aid,
                       sizeof(mmgsdi_aid_type));
      }
    }
    if((refresh_req_ptr->refresh.card_req.refresh_mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
        refresh_req_ptr->refresh.card_req.refresh_mode == MMGSDI_REFRESH_NAA_FCN ||
        refresh_req_ptr->refresh.card_req.refresh_mode == MMGSDI_REFRESH_3G_SESSION_RESET) &&
        refresh_req_ptr->refresh.card_req.refresh_files.data_len > 0)
    {
      /* Convert file data into file path array and populate the global */
      mmgsdi_status = mmgsdi_util_convert_to_refresh_file_paths(
                      int32touint32(refresh_req_ptr->refresh.card_req.refresh_files.data_len),
                      refresh_req_ptr->refresh.card_req.refresh_files.data_ptr,
                      &mmgsdi_refresh_data_info.refresh_file_list.file_path_ptr,
                      (uint8*)&mmgsdi_refresh_data_info.refresh_file_list.num_file_paths);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
    }
  } while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  return mmgsdi_status;
} /* mmgsdi_copy_info_to_refresh_data_info */


/* ============================================================================
  FUNCTION:      MMGSDI_IS_FILE_IN_REFRESH_DATA_INFO_LIST

  DESCRIPTION:
    This function is a utility to find out if a file is present in the
    global mmgsdi_refresh_data_info (that contains the files that have
    been requested to be refreshed as part of FCN or INIT_FCN refresh types).
    The idea is that if the file is present in the global for FCN or INIT_FCN
    refresh modes, the WRITE on that file will be blocked.
    As a special case, if the refresh currently underway is INIT_FFCN, the
    function returns TRUE regardless of the EF. This will ensure that when a
    INIT_FFCN is in progress, WRITE to ALL EFs is blocked for that app.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    boolean
    FALSE:          File is not present in the refresh files list
    TRUE:           File is present in refresh list or ERROR

  SIDE EFFECTS:
    Uses critical section
=============================================================================*/
boolean mmgsdi_is_file_in_refresh_data_info_list(
  mmgsdi_session_id_type    session_id,
  mmgsdi_access_type        file_access
)
{
  mmgsdi_slot_id_enum_type           slot_id         = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type          *app_info_ptr    = NULL;
  uint32                             i               = 0;
  mmgsdi_path_type                   file_path;
  boolean                            is_present      = FALSE;

  memset(&file_path, 0x00, sizeof(mmgsdi_path_type));

  /* Protect access to Client ID Reg.Table and mmgsdi_refresh_data_info_ptr */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    /* In case no refresh is in progress, return */
    if(mmgsdi_refresh_data_info.refresh_mode == MMGSDI_REFRESH_MODE_ENUM_MAX)
    {
      break;
    }

    if(MMGSDI_SUCCESS != mmgsdi_util_get_session_app_info(session_id,
                                                          &slot_id,
                                                          NULL,
                                                          &app_info_ptr,
                                                          NULL, NULL, NULL))
    {
      is_present = TRUE;
      break;
    }

    /* If the slot corresponding to the session and that of refresh req are
       different, return */
    if(slot_id != mmgsdi_refresh_data_info.slot_id)
    {
      break;
    }

    /* If aid mismatch, return */
    if(MMGSDI_SUCCESS != mmgsdi_util_cmp_aid(&app_info_ptr->app_data,
                                             &mmgsdi_refresh_data_info.app_data,
                                             FALSE))
    {
      break;
    }

    /* The slot, aid and other info is good, so move to the next steps in
       search procedure by comparing the data in the files */

    /* When a INIT+FFCN refresh is underway, return TRUE so that the writes to
       ALL the EFs can be blocked */
    if(mmgsdi_refresh_data_info.refresh_mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN)
    {
      is_present = TRUE;
      break;
    }

    /* If there's no file in the refresh files list, return */
    if(mmgsdi_refresh_data_info.refresh_file_list.num_file_paths == 0)
    {
      break;
    }

    if(file_access.access_method == MMGSDI_EF_ENUM_ACCESS)
    {
      if(MMGSDI_SUCCESS != mmgsdi_file_get_path_from_file(file_access.file.file_enum,
                                                          &file_path))
      {
        is_present = TRUE;
        break;
      }
    }
    else if(file_access.access_method == MMGSDI_BY_PATH_ACCESS &&
            file_access.file.path_type.path_len > 0 &&
            file_access.file.path_type.path_len <= MMGSDI_MAX_PATH_LEN)
    {
      file_path = file_access.file.path_type;
    }
    else
    {
      break;
    }

    /* At this point, file_path contains the file paths (either by conversion
       from BY_ENUM access OR by BY_PATH access) */
    if(file_path.path_len == 0)
    {
      break;
    }

    for(i = 0; i < mmgsdi_refresh_data_info.refresh_file_list.num_file_paths ; i++)
    {
      if((mmgsdi_refresh_data_info.refresh_file_list.file_path_ptr[i].path_len == 0) ||
         (file_path.path_len !=
          mmgsdi_refresh_data_info.refresh_file_list.file_path_ptr[i].path_len))
      {
        continue;
      }
      if(!memcmp(mmgsdi_refresh_data_info.refresh_file_list.file_path_ptr[i].path_buf,
                 file_path.path_buf,
                 MIN(file_path.path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
      {
        is_present = TRUE;
        break;
      }
    }
  } while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  return is_present;
} /* mmgsdi_is_file_in_refresh_data_info_list */


/* =============================================================================
  FUNCTION:      MMGSDI_COPY_TO_REFRESH_PTR

  DESCRIPTION:
    This function will copy all data to mmgsi_refresh_info_ptr

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
 static mmgsdi_return_enum_type mmgsdi_copy_to_refresh_ptr(
  const   mmgsdi_request_header_type    *req_header_ptr,
  const   mmgsdi_refresh_card_req_type  *req_ptr,
  uint32                                 aid_index)
{
  mmgsdi_int_app_info_type *sel_app_ptr   = NULL;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_data_type    *slot_data_ptr = NULL;
  uint8                     slot_index    = MMGSDI_SLOT_1_INDEX;

  if(req_header_ptr == NULL ||
     req_ptr        == NULL)
  {
    UIM_MSG_ERR_0("NULL Pointer");
    return MMGSDI_ERROR;
  }

  /*Free any prev data if it is still around*/
  mmgsdi_status = mmgsdi_util_get_slot_index(req_header_ptr->slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  /* If previous data exists, free memory allocated for pointer member as well */
  if(mmgsdi_refresh_info_ptr[slot_index])
  {
    mmgsdi_refresh_free_refresh_info_type(&mmgsdi_refresh_info_ptr[slot_index]);
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_refresh_info_ptr[slot_index],
                                     sizeof(mmgsdi_refresh_info_type));
  if (mmgsdi_refresh_info_ptr[slot_index] == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  mmgsdi_memscpy(&mmgsdi_refresh_info_ptr[slot_index]->request_header,
                 sizeof(mmgsdi_refresh_info_ptr[slot_index]->request_header),
                 req_header_ptr,
                 sizeof(mmgsdi_request_header_type));

  mmgsdi_refresh_info_ptr[slot_index]->mode        = req_ptr->refresh_mode;
  mmgsdi_refresh_info_ptr[slot_index]->orig_mode   = req_ptr->refresh_mode;
  mmgsdi_refresh_info_ptr[slot_index]->retry_count = req_ptr->retry_info.retry_count;
  mmgsdi_refresh_info_ptr[slot_index]->source      = req_ptr->source;

  if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_STEERING_OF_ROAMING)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_refresh_info_ptr[slot_index]->steering_mode_data.data_ptr,
      req_ptr->plmn_list.data_len);
    if (mmgsdi_refresh_info_ptr[slot_index]->steering_mode_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy(mmgsdi_refresh_info_ptr[slot_index]->steering_mode_data.data_ptr,
                   req_ptr->plmn_list.data_len,
                   req_ptr->plmn_list.data_ptr,
                   req_ptr->plmn_list.data_len);

    mmgsdi_refresh_info_ptr[slot_index]->steering_mode_data.data_len =
                                         req_ptr->plmn_list.data_len;
  }
  else
  {
    if(req_ptr->refresh_files.data_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        mmgsdi_refresh_info_ptr[slot_index]->refresh_retry_files.data_ptr,
        req_ptr->refresh_files.data_len);
      if (mmgsdi_refresh_info_ptr[slot_index]->refresh_retry_files.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_memscpy(mmgsdi_refresh_info_ptr[slot_index]->refresh_retry_files.data_ptr,
                     req_ptr->refresh_files.data_len,
                     req_ptr->refresh_files.data_ptr,
                     req_ptr->refresh_files.data_len);
    }
    mmgsdi_refresh_info_ptr[slot_index]->refresh_retry_files.data_len =
                                         req_ptr->refresh_files.data_len;
  }

  if(req_ptr->aid.aid.data_len == 0)
  {
    mmgsdi_status = mmgsdi_util_get_ch_zero_app(req_header_ptr->slot_id,
                                                &sel_app_ptr,
                                                NULL);

    if((mmgsdi_status == MMGSDI_SUCCESS) && (sel_app_ptr != NULL))
    {
      mmgsdi_memscpy(&mmgsdi_refresh_info_ptr[slot_index]->aid,
                     sizeof(mmgsdi_refresh_info_ptr[slot_index]->aid),
                     &sel_app_ptr->app_data,
                     sizeof(mmgsdi_aid_type));

      return MMGSDI_SUCCESS;
    }
  }

  mmgsdi_memscpy(&mmgsdi_refresh_info_ptr[slot_index]->aid,
                 sizeof(mmgsdi_refresh_info_ptr[slot_index]->aid),
                 &req_ptr->aid,
                 sizeof(mmgsdi_aid_type));

  if(req_ptr->aid.aid.data_len != 0)
  {
    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_header_ptr->slot_id);

    if((slot_data_ptr == NULL) ||
       (slot_data_ptr->app_info_pptr == NULL))
    {
      UIM_MSG_ERR_0("Incorrect slot/app data ptr ");
      return MMGSDI_CAN_NOT_REFRESH;
    }

    /* Application type is not populated in request data by GSTK
       and app_type is needed as mandatory param in init cache functions.
       Hence we need to update app type here.*/
    if((aid_index < MMGSDI_MAX_EXT_APP_INFO) &&
       (slot_data_ptr->app_info_pptr[aid_index] != NULL))
    {
      mmgsdi_refresh_info_ptr[slot_index]->aid.app_type =
        slot_data_ptr->app_info_pptr[aid_index]->app_data.app_type;
    }
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_copy_to_refresh_ptr*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_RESET_CARD

  DESCRIPTION:
    This function will check to see if refresh was issued for Channel 0 of the card.
    If it was then it will create the linked list with nodes of all clients that
    have voted for init on any session so that all of them can determine if
    it is a good time to do a reset.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          It is default app and the linked list has been
                             created.
    MMGSDI_ERROR:            It is not the default app.

  SIDE EFFECTS:
    All clients regardless of the aid in the refresh command will be asked if
    it is ok to init.
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_reset_card(
  const mmgsdi_refresh_card_req_type    *req_ptr,
  boolean                               *wait_for_rsp_ptr,
  boolean                               *wait_for_pdown_cfm_ptr,
  uint8                                  slot_index)
{
  int32                     client_index     = 0;
  int32                     session_index    = 0;
  mmgsdi_len_type           num_sessions     = 0;
  mmgsdi_session_info_type *session_ptr      = NULL;
  mmgsdi_slot_id_enum_type  slot             = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_1("FUNCTION mmgsdi_refresh_reset_card for slot_index: 0x%x",
                 slot_index);

  MMGSDIUTIL_RETURN_IF_NULL_3(req_ptr, wait_for_rsp_ptr, wait_for_pdown_cfm_ptr);

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  for (client_index = 0; client_index < MMGSDI_MAX_CLIENT_INFO; client_index++)
  {
    if (mmgsdi_client_id_reg_table[client_index] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[client_index]->num_session;
    /*Find out all clients connected that have any session open */
    for (session_index = 0;
         ((num_sessions > 0) && session_index < MMGSDI_MAX_SESSION_INFO);
         session_index++)
    {
      session_ptr = mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

      if (session_ptr == NULL)
      {
        continue;
      }

      num_sessions--;

      /* Without session event cb ptr clients will not get any refresh events.
         MMGSDI will be stuck waiting for responses and resulting in time out */
      if(session_ptr->session_evt_cb_ptr == NULL)
      {
        continue;
      }

      mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_ptr->session_id,
                        &slot,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL);

      if(mmgsdi_status != MMGSDI_SUCCESS ||
         slot != mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id)
      {
        continue;
      }

      if (session_ptr->refresh_data.vote_for_init)
      {
        *wait_for_rsp_ptr = TRUE;
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                NULL,
                                session_ptr->refresh_data.proceed_refresh,
                                MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT);
      }
      else if(session_ptr->refresh_data.proceed_refresh)
      {
        *wait_for_pdown_cfm_ptr = TRUE;
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                NULL,
                                TRUE,
                                MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH);
      }
    }
  }
  mmgsdi_refresh_info_ptr[slot_index]->mode = req_ptr->refresh_mode;
  return MMGSDI_SUCCESS;
} /*mmgsdi_refresh_reset_card*/


/*===========================================================================
   FUNCTION MMGSDI_REFRESH_CARD_GET_SESSION_PTR

   DESCRIPTION:
     This function returns session_ptr only for the card session.

   PARAMS:

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    mmgsdi_session_info_type *
 ===========================================================================*/
static mmgsdi_session_info_type*  mmgsdi_refresh_card_get_session_ptr(
  uint8                     client_index,
  uint8                     session_index,
  mmgsdi_slot_id_enum_type  slot_id
)
{
  mmgsdi_session_info_type  *session_ptr   = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  if (client_index >= MMGSDI_MAX_CLIENT_INFO ||
      mmgsdi_client_id_reg_table[client_index] == NULL ||
      session_index >= MMGSDI_MAX_SESSION_INFO ||
      !mmgsdi_util_is_slot_valid(slot_id))
  {
    /* Invalid Client index or session_index or slot_id */
    return NULL;
  }

  session_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if ((session_ptr != NULL) &&
      (session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
      (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL))
  {
    if((mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->slot_id == slot_id) &&
       (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr != NULL) &&
       (mmgsdi_util_is_card_session(session_ptr->session_type)))
    {
      return session_ptr;
    }
  }
  return NULL;
}/* mmgsdi_refresh_card_get_session_ptr */


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_MALLOC_AND_CALC_CLIENT_SPEC_FILES

  DESCRIPTION:
    Calculates client specific refresh file list

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_FOUND:        When session can not be located in the session
                             info table with given client index and aid.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_malloc_and_calc_client_spc_files(
  const mmgsdi_refresh_card_req_type              *req_ptr,
  mmgsdi_session_info_type                       **session_pptr,
  const mmgsdi_internal_refresh_file_list_type    *refresh_files_ptr,
  mmgsdi_internal_refresh_file_list_type          *client_spc_refresh_files_ptr,
  mmgsdi_refresh_stage_enum_type                  *stage_ptr,
  uint8                                            client_index,
  uint8                                            session_index,
  boolean                                         *need_to_send_fcn_flag_ptr,
  uint8                                            slot_index
)
{
  boolean                       is_reg_by_path           = FALSE;
  uint32                        num_files                = 0;
  uint8                         i                        = 0;
  uint8                         j                        = 0;
  uint32                        k                        = 0;
  mmgsdi_session_info_type      *session_ptr             = NULL;
  uint32                        path_len                 = 0;

  /* Check for null and input parameters */
  MMGSDIUTIL_RETURN_IF_NULL_6(req_ptr, session_pptr,
                              refresh_files_ptr,
                              client_spc_refresh_files_ptr,
                              stage_ptr,
                              need_to_send_fcn_flag_ptr);

  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  client_spc_refresh_files_ptr->num_files = 0;
  client_spc_refresh_files_ptr->num_file_paths = 0;

  session_ptr = mmgsdi_util_cmp_aid_get_session_ptr(
                  client_index,
                  session_index,
                  &mmgsdi_refresh_info_ptr[slot_index]->aid,
                  mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                  TRUE);
  if(session_ptr == NULL)
  {
    /* Skipping AID comparision on card session for REFRESH NAA FCN and
       NAA INIT FCN modes */
    session_ptr = mmgsdi_refresh_card_get_session_ptr(
                          client_index,
                          session_index,
                          mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id);
  }

  if((session_ptr == NULL) ||
     (session_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
     (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] == NULL) ||
     (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr == NULL))
  {
    return MMGSDI_NOT_FOUND;
  }

  /* check if client registered using file path or enums */
  is_reg_by_path = session_ptr->refresh_data.reg_by_path;

  if(!is_reg_by_path)
  {
    if(session_ptr->refresh_data.refresh_files.num_files > 0)
    {
      for(j=0;j<refresh_files_ptr->num_files;j++)
      {
        /* check if the file is not a stack file */
        if(!mmgsdi_refresh_is_call_stack_file(
              refresh_files_ptr->file_list_ptr[j]))
        {
          for(k=0;k<session_ptr->refresh_data.refresh_files.num_files;k++)
          {
            if(refresh_files_ptr->file_list_ptr[j] ==
               session_ptr->refresh_data.refresh_files.file_list_ptr[k])
            {
              num_files++;
              break;
            }
          }/*end for k*/
        }
      }/*end for j*/
    }
  } /* if (!is_reg_by_path) */
  else /* client has registered by file path */
  {
    if (refresh_files_ptr->num_file_paths != refresh_files_ptr->num_files)
    {
      UIM_MSG_ERR_2("refresh req path num_file 0x%x != enum num_file 0x%x",
                    refresh_files_ptr->num_file_paths,
                    refresh_files_ptr->num_files);
      return MMGSDI_ERROR;
    }
    /* Find out how many files are in intersection set. This will help us in
       determining how much memory should we allocate for file buffer */
    if(session_ptr->refresh_data.refresh_files.num_file_paths > 0)
    {
      for(j=0; j < refresh_files_ptr->num_file_paths; j++)
      {
        if(refresh_files_ptr->file_path_ptr[j].path_len == 0)
        {
          UIM_MSG_ERR_1("Zero path_len at file_path_ptr[0x%x].path_len", j);
          return MMGSDI_ERROR;
        }
        /* check if the file is not a stack file */
        /* refresh_files_ptr will contain the same number of files in the file_list_ptr
           and file_path_ptr and they are 1 to 1 mapping to each other.  Hence, we
           can check if the file is a call stack file by looking at the corresponding
           index location in the file_list_ptr */
        if(!mmgsdi_refresh_is_call_stack_file(
              refresh_files_ptr->file_list_ptr[j]))
        {
          for(k=0;k<session_ptr->refresh_data.refresh_files.num_file_paths;k++)
          {
            /* compare path length */
            if(refresh_files_ptr->file_path_ptr[j].path_len ==
               session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_len)
            {
              path_len = int32touint32(refresh_files_ptr->file_path_ptr[j].path_len);
              /* compare path buffers */
              if(!memcmp(
                  refresh_files_ptr->file_path_ptr[j].path_buf,
                  session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_buf,
                  MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
              {
                num_files++;
                break;
              }
            }
          }/*end for k*/
        } /*if the file is not a call stack file */
      }/*end for j*/
    }
  } /* is_reg_by_path */

  if(num_files > 0)
  {
    if(!is_reg_by_path)
    {
      /*Allocate memory for the file list*/
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        client_spc_refresh_files_ptr->file_list_ptr,
        (sizeof(mmgsdi_file_enum_type)* num_files));

      if(client_spc_refresh_files_ptr->file_list_ptr == NULL)
      {
        UIM_MSG_ERR_0("Refresh: Malloc Error for Refresh File List Ptr");
        return MMGSDI_ERROR;
      }

      /* Set file path pointer to null */
      client_spc_refresh_files_ptr->file_path_ptr = NULL;

      i = 0;
      for(j=0;j<refresh_files_ptr->num_files;j++)
      {
        /* check if the file is not a stack file */
        if(!mmgsdi_refresh_is_call_stack_file(
            refresh_files_ptr->file_list_ptr[j]))
        {
          for(k=0;k<session_ptr->refresh_data.refresh_files.num_files;k++)
          {
            if(refresh_files_ptr->file_list_ptr[j] ==
               session_ptr->refresh_data.refresh_files.file_list_ptr[k])
            {
              if( i < num_files )
              {
                client_spc_refresh_files_ptr->file_list_ptr[i] =
                  refresh_files_ptr->file_list_ptr[j];
                i++;
              }
              break;
            }
          }/*end for k*/
        }
      }/*end for j*/
      client_spc_refresh_files_ptr->num_files       = num_files;
      client_spc_refresh_files_ptr->num_file_paths  = 0;
    } /* if (!is_reg_by_path) */
    else
    {
      /*Allocate memory for the file path list*/
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        client_spc_refresh_files_ptr->file_path_ptr,
        (sizeof(mmgsdi_path_type)* num_files));

      if(client_spc_refresh_files_ptr->file_path_ptr == NULL)
      {
        /* mmgsdi_refresh_info_ptr will be cleaned up later*/
        UIM_MSG_ERR_0("Refresh: Malloc Error for Refresh File Path Ptr");
        return MMGSDI_ERROR;
      }

      /* Set file enum list pointer to null */
      client_spc_refresh_files_ptr->file_list_ptr = NULL;

      i = 0;
      /*Copy the files over now*/
      for(j = 0; j < refresh_files_ptr->num_file_paths; j++)
      {
        if(refresh_files_ptr->file_path_ptr[j].path_len == 0)
        {
          UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len",
                        j);
          MMGSDIUTIL_TMC_MEM_FREE(client_spc_refresh_files_ptr->file_path_ptr);
          return MMGSDI_ERROR;
        }
        /* check if the file is not a stack file */
        /* refresh_files_ptr will contain the same number of files in the file_list_ptr
           and file_path_ptr and they are 1 to 1 mapping to each other.  Hence, we
           can check if the file is a call stack file by looking at the corresponding
           index location in the file_list_ptr */
        if(!mmgsdi_refresh_is_call_stack_file(
              refresh_files_ptr->file_list_ptr[j]))
        {
          for(k=0;k<session_ptr->refresh_data.refresh_files.num_file_paths;k++)
          {
            /* compare path length */
            if(refresh_files_ptr->file_path_ptr[j].path_len ==
               session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_len)
            {
              path_len = int32touint32(refresh_files_ptr->file_path_ptr[j].path_len);

              /* compare path buffers */
              if(!memcmp(
                  refresh_files_ptr->file_path_ptr[j].path_buf,
                  session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_buf,
                  MIN(path_len,MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
              {
                if(i < num_files)
                {
                  /* Copy file path */
                  client_spc_refresh_files_ptr->file_path_ptr[i].path_len =
                    refresh_files_ptr->file_path_ptr[j].path_len;
                  mmgsdi_memscpy(client_spc_refresh_files_ptr->file_path_ptr[i].path_buf,
                    sizeof(client_spc_refresh_files_ptr->file_path_ptr[i].path_buf),
                    refresh_files_ptr->file_path_ptr[j].path_buf,
                    MIN(path_len, MMGSDI_MAX_PATH_LEN)* sizeof(uint16));

                  i++;
                }
                break;
              }
            }
          }/*end for k*/
        }
      }/*end for j*/
      client_spc_refresh_files_ptr->num_files       = 0;
      client_spc_refresh_files_ptr->num_file_paths  = num_files;
    } /* end of else is_reg_by_path */

    *stage_ptr = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;

    if (client_index >= MMGSDI_MAX_CLIENT_INFO)
    {
      UIM_MSG_ERR_1("Invalid client_index: 0x%x", client_index);
      return MMGSDI_ERROR;
    }

    if(mmgsdi_client_id_reg_table[client_index] != NULL)
    {
      if(mmgsdi_client_id_reg_table[client_index]->client_id !=
       mmgsdi_generic_data_ptr->client_id)
      {
        *need_to_send_fcn_flag_ptr |=TRUE;
      }
    }
    else
    {
      return MMGSDI_NOT_FOUND;
    }
  } /* if num_files > 0 */
  else if(mmgsdi_util_is_card_session(session_ptr->session_type))
  {
    /* Card Sessions need not be sent non FCN REFRESH notifications */
    return MMGSDI_NOT_FOUND;
  }
  *session_pptr = session_ptr;
  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_malloc_and_calc_client_spc_files */


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_PROCESS_RESET

  DESCRIPTION:
    A Refresh Reset Command has been received
       * Build Request based on all clients that are interested in this app
       * If no client is interested, then complete refresh without sending any
         events
       * Only clients that have a session open with the current app will
         receive any events and the session will also only close for only
         those clients

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_reset(
  mmgsdi_slot_id_enum_type             slot_id,
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr,
  mmgsdi_refresh_mode_enum_type        mode)
{
  uint8                                   i                          = 0;
  uint8                                   j                          = 0;
  uint32                                  k                          = 0;
  mmgsdi_session_info_type               *session_ptr                = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status              = MMGSDI_ERROR;
  boolean                                 wait_for_rsp               = FALSE;
  mmgsdi_len_type                         num_sessions               = 0;
  boolean                                 wait_for_ok_to_proceed_cfm = FALSE;
  boolean                                 need_to_send_fcn_flag      = FALSE;
  boolean                                 wait_for_fcn_rsp           = FALSE;
  mmgsdi_internal_refresh_file_list_type  refresh_files              = {0};
  mmgsdi_internal_refresh_file_list_type  client_spc_refresh_files   = {0};
  uint8                                   slot_index                 = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_1("FUNCTION mmgsdi_refresh_process_reset for slot: 0x%x", slot_id);

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, extra_param_ptr, req_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  /* REFRESH request does not indicate an AID and no application is currently
     selected on the default logical channel */
  if(mmgsdi_refresh_info_ptr[slot_index]->aid.aid.data_len == 0)
  {
    UIM_MSG_HIGH_0("Refresh - No active app on default logical channel");
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;

    return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
  }

  /* Convert 3G Session Reset to INIT FCN if it requires to follow ETSI spec */
  if(mode == MMGSDI_REFRESH_3G_SESSION_RESET &&
     mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_REFRESH_3G_SESSION_RESET_PER_ETSI_SPEC, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
       MMGSDI_FEATURE_ENABLED)
  {
    mmgsdi_status = mmgsdi_refresh_process_init_fcn(slot_id,
                                                    slot_ptr,
                                                    req_ptr,
                                                    extra_param_ptr);
    return mmgsdi_status;
  }

  /*----------------------------------------------------------------
    Convert to MMGSDI File Enums
  ----------------------------------------------------------------*/
  /* Extract file list from refresh request for 3G Session Reset */
  if(req_ptr->refresh_files.data_len > 0 && mode == MMGSDI_REFRESH_3G_SESSION_RESET)
  {
    boolean non_call_stack_file_present = FALSE;

    if(mmgsdi_refresh_malloc_and_convert_file_path(req_ptr, slot_index, &refresh_files)
       != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }

    if(refresh_files.file_list_ptr == NULL || refresh_files.file_path_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_path_ptr);
      UIM_MSG_ERR_0("Refresh file ptr is NULL");
      return MMGSDI_INCORRECT_PARAMS;
    }

    for(k = 0; k < refresh_files.num_files; k++)
    {
      if(!mmgsdi_refresh_is_call_stack_file(
          refresh_files.file_list_ptr[k]))
      {
        non_call_stack_file_present = TRUE;
        break;
      }
    }

    if(!non_call_stack_file_present)
    {
      refresh_files.num_files = 0;
      refresh_files.num_file_paths = 0;
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_path_ptr);
    }
    else
    {
      mmgsdi_status = mmgsdi_refresh_malloc_and_copy_file_list(refresh_files, slot_index);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
        return mmgsdi_status;
      }
    }
  }

  /*----------------------------------------------------------------
  Build Linked List with all clients that have registered for ok to init
  ----------------------------------------------------------------*/
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init = TRUE;
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.proceed_refresh = TRUE;

  for(i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    mmgsdi_refresh_stage_enum_type stage = MMGSDI_REFRESH_STAGE_START;

    if(mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[i]->num_session;

    for(j = 0; (num_sessions > 0) && (j < MMGSDI_MAX_SESSION_INFO); j++)
    {
      session_ptr =
        mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

      if(session_ptr == NULL)
      {
        /*Invalid Session, Continue to next*/
        continue;
      }
      num_sessions--;

      session_ptr = mmgsdi_util_cmp_aid_get_session_ptr(
                      i,
                      j,
                      &mmgsdi_refresh_info_ptr[slot_index]->aid,
                      mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                      FALSE);

      if(session_ptr == NULL)
      {
        /*This client is not interested in this app*/
        continue;
      }

      /* Without session event cb ptr clients will not get any refresh events.
         MMGSDI will be stuck waiting for responses and resulting in time out */
      if(session_ptr->session_evt_cb_ptr == NULL)
      {
        continue;
      }

      stage = MMGSDI_REFRESH_STAGE_START;

      /* Calculate client specific refresh files only when refresh mode is 3G
         session reset and we have FCN list with it*/
      if(refresh_files.file_list_ptr &&
         mode == MMGSDI_REFRESH_3G_SESSION_RESET)
      {
        mmgsdi_status = mmgsdi_refresh_malloc_and_calc_client_spc_files(
                          req_ptr,
                          &session_ptr,
                          &refresh_files,
                          &client_spc_refresh_files,
                          &stage,
                          i,
                          j,
                          &need_to_send_fcn_flag,
                          slot_index);
        if(mmgsdi_status == MMGSDI_NOT_FOUND)
        {
          continue;
        }
        else if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          /* Clean up refresh files */
          MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
          MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
          return mmgsdi_status;
        }
      }

      if(session_ptr->refresh_data.vote_for_init &&
         !mmgsdi_util_is_card_session(session_ptr->session_type))
      {
        wait_for_rsp = TRUE;
        /*Add Node to Linked List*/
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                &client_spc_refresh_files,
                                session_ptr->refresh_data.proceed_refresh,
                                MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT);
      }
      else if(session_ptr->refresh_data.proceed_refresh &&
              !mmgsdi_util_is_card_session(session_ptr->session_type))
      {
        wait_for_ok_to_proceed_cfm = TRUE;
        /*Add Node to Linked List*/
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                &client_spc_refresh_files,
                                TRUE,
                                MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH);
      }
      else if( stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN )
      {
        wait_for_fcn_rsp = TRUE;
        /*Add Node to Linked List*/
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                &client_spc_refresh_files,
                                FALSE,
                                stage);
      }
      else
      {
        if(!mmgsdi_util_is_card_session(session_ptr->session_type))
        {
          mmgsdi_refresh_add_node(session_ptr->session_id,
                                  &client_spc_refresh_files,
                                  FALSE,
                                  MMGSDI_REFRESH_STAGE_START);
        }
      }

      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_spc_refresh_files.file_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_spc_refresh_files.file_path_ptr);
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_list_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_path_ptr);

  if(mmgsdi_refresh_info_ptr[slot_index]->node_ptr == NULL)
  {
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;

    if(req_ptr->aid.aid.data_len == 0)
    {
      UIM_MSG_HIGH_0("Refresh Success");
      return MMGSDI_REFRESH_SUCCESS;
    }
    else
    {
      UIM_MSG_HIGH_0("Refresh - AID not Active");
      return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
    }
  }

  if(need_to_send_fcn_flag == FALSE)
  {
    mmgsdi_refresh_node_type *temp_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
    wait_for_fcn_rsp = FALSE;

    /* We need not do OK_TO_FCN voting here.Hence change the stage of nodes
       with MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN */

    while(temp_ptr != NULL)
    {
      if(temp_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN )
      {
        temp_ptr->stage = MMGSDI_REFRESH_STAGE_START;
      }
      temp_ptr= temp_ptr->next_ptr;
    }
    mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_files = 0;
    mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_file_paths = 0;
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.file_path_ptr);
  }

  UIM_MSG_HIGH_3("Refresh - Need to wait for OK_TO_INIT: 0x%x, OK_TO_PROCEED: 0x%x, OK_TO_FCN: 0x%x",
                 wait_for_rsp, wait_for_ok_to_proceed_cfm, wait_for_fcn_rsp);
  if(wait_for_rsp)
  {
    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE;
  }
  else if(wait_for_ok_to_proceed_cfm)
  {
    /*At least we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE;
  }
  else if(wait_for_fcn_rsp)
  {
    /*Wait for client to respond*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE;
  }
  else
  {
    mmgsdi_status = mmgsdi_refresh_start_reset(slot_ptr,
                                               extra_param_ptr,
                                               slot_index);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Refresh: Could not start reset, status 0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
    }
    slot_ptr->refresh_state  = MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
  }

  return MMGSDI_REFRESH_IN_PROGRESS;
}/*mmgsdi_refresh_process_reset*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_PROCESS_APP_RESET_AUTO

  DESCRIPTION:
    A Refresh Reset Auto Command has been received
       * This is used to vote with all clients before triggerring an app
          deactivate/app switch
       * Build ok_to_init request based on all clients that have registered
         for this
       * If no client has registered for init voting, then start app
         deactivate without sending any events

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_app_reset_auto(
  mmgsdi_slot_id_enum_type             slot_id,
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr)
{
  uint8                              i         = 0;
  uint8                              j         = 0;
  mmgsdi_session_info_type *session_ptr        = NULL;
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_len_type           num_sessions       = 0;
  boolean                   vote_for_init      = FALSE;
  boolean                   vote_for_pdown_cfm = FALSE;
  uint8                     slot_index         = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_0("FUNCTION mmgsdi_refresh_process_app_reset_auto");

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  /*----------------------------------------------------------------
  Build Linked List with all clients that have registered for ok to init
  ----------------------------------------------------------------*/
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init = TRUE;
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.proceed_refresh = TRUE;

  for(i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if(mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[i]->num_session;

    for(j = 0; (num_sessions > 0) && (j < MMGSDI_MAX_SESSION_INFO); j++)
    {
      session_ptr =
        mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

      if(session_ptr == NULL)
      {
        /*Invalid Session, Continue to next*/
        continue;
      }
      num_sessions--;

      session_ptr = mmgsdi_util_cmp_aid_get_session_ptr(
                      i,
                      j,
                      &mmgsdi_refresh_info_ptr[slot_index]->aid,
                      mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                      FALSE);

      if(session_ptr == NULL)
      {
        /*This client is not interested in this app*/
        continue;
      }

      /* Without session event cb ptr clients will not get any refresh events.
         MMGSDI will be stuck waiting for responses and resulting in time out */
      if(session_ptr->session_evt_cb_ptr == NULL)
      {
        continue;
      }

      if(mmgsdi_util_is_card_session(session_ptr->session_type))
      {
        /*Card Sessions need not be sent REFRESH notifications*/
        continue;
      }

      if(session_ptr->refresh_data.vote_for_init)
      {
        vote_for_init = TRUE;
        /*Add Node to Linked List*/
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                NULL,
                                session_ptr->refresh_data.proceed_refresh,
                                MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT);
      }
      else if(session_ptr->refresh_data.proceed_refresh)
      {

        vote_for_pdown_cfm = TRUE;
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                NULL,
                                session_ptr->refresh_data.proceed_refresh,
                                MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH);
      }
    }
  }

  if(mmgsdi_refresh_info_ptr[slot_index]->node_ptr == NULL)
  {
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
    if(req_ptr->aid.aid.data_len == 0)
    {
      UIM_MSG_HIGH_0("Refresh Success");
      return MMGSDI_REFRESH_SUCCESS;
    }
    else
    {
      UIM_MSG_HIGH_0("Refresh - No AID Active");
      return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
    }
  }

  if(vote_for_init)
  {
    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE;
  }
  else if(vote_for_pdown_cfm)
  {
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE;
  }
  else
  {
    mmgsdi_status = mmgsdi_refresh_start_app_reset_auto(slot_ptr,
                                                        extra_param_ptr,
                                                        slot_index);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Refresh: Could not start reset auto, status 0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
    }
    /* This is to ensure that we do not start an INIT_COMPLETE timer */
    slot_ptr->refresh_state = MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE;
  }

  return MMGSDI_REFRESH_IN_PROGRESS;
}/*mmgsdi_refresh_process_app_reset_auto*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_PROCESS_FULL_RESET

  DESCRIPTION:
    A Refresh Reset Command has been received
       * Purge XXLOCI files if refresh mode is MMGSDI_REFRESH_RESET and
         MMGSDI_FEATURE_PURGE_TEMP_IDENTITIES is enabled
       * Build Request based on all clients on all apps that have voted for
         init
       * All sessions will be terminated as part of this refresh command

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_full_reset(
  mmgsdi_cmd_enum_type                 orig_request_type,
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr,
  mmgsdi_slot_id_enum_type             slot_id)
{
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_ERROR;
  boolean                   wait_for_rsp       = FALSE;
  boolean                   wait_for_pdown_cfm = FALSE;
  uint8                     slot_index         = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_1("FUNCTION mmgsdi_refresh_process_full_reset, refresh_mode=0x%x",
                 mmgsdi_refresh_data_info.refresh_mode);

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  /* Purge XXLOCI files if needed */
  if(MMGSDI_REFRESH_RESET == mmgsdi_refresh_data_info.refresh_mode)
  {
    mmgsdi_refresh_check_and_purge_temp_identities(slot_id, slot_ptr->protocol);
  }

  /*----------------------------------------------------------------
  Build Linked List with all clients that have registered for ok to init
  ----------------------------------------------------------------*/
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init = TRUE;
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.proceed_refresh = TRUE;

  if (orig_request_type != MMGSDI_CARD_PUP_REQ)
  {
    mmgsdi_status = mmgsdi_refresh_reset_card(req_ptr, &wait_for_rsp, &wait_for_pdown_cfm, slot_index);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Refresh: Refresh reset failed");
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
      slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
      return MMGSDI_CAN_NOT_REFRESH;
    }
  }
  else
  {
    /* The card pup request was initiated by client, we need not wait */
    wait_for_rsp = FALSE;
    wait_for_pdown_cfm = FALSE;
  }

  if(wait_for_rsp)
  {
    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE;
  }
  else if(wait_for_pdown_cfm)
  {
    /* We need to wait for client to confirm Card Power Down*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE;
  }
  else
  {
    mmgsdi_status = mmgsdi_refresh_start_full_reset(slot_ptr,
                                                    extra_param_ptr,
                                                    req_ptr->refresh_mode,
                                                    req_ptr->switch_protocol,
                                                    slot_index);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Refresh: Could not start reset, status 0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
    }
    slot_ptr->refresh_state  = MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
  }

  return MMGSDI_REFRESH_IN_PROGRESS;
}/*mmgsdi_refresh_process_full_reset*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_PROCESS_INIT

  DESCRIPTION:
    A Refresh INIT/Reset Command has been received
       * Build Request based on all clients that are interested in this app
       * If no client is interested, then send ok to init event and set stage

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_init(
  mmgsdi_slot_id_enum_type             slot_id,
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr,
  mmgsdi_refresh_mode_enum_type        mode)
{
  uint8                     i                  = 0;
  uint8                     j                  = 0;
  mmgsdi_session_info_type *session_ptr        = NULL;
  boolean                   wait_for_rsp       = FALSE;
  mmgsdi_len_type           num_sessions       = 0;
  boolean                   skip_voting        = FALSE;
  boolean                   wait_for_pdown_cfm = FALSE;
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_SUCCESS;
  uint8                     slot_index         = MMGSDI_SLOT_1_INDEX;
  gstk_slot_id_enum_type    gstk_slot          = GSTK_SLOT_ID_MAX;

  UIM_MSG_HIGH_0("FUNCTION mmgsdi_refresh_process_init");

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid slot id, 0x%x", slot_id);
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    UIM_MSG_ERR_1("Invalid slot index, 0x%x", slot_index);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

/*----------------------------------------------------------------
  Build Linked List with all clients that have registered for ok to init
  ----------------------------------------------------------------*/
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init = TRUE;
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.proceed_refresh = TRUE;
  mmgsdi_refresh_info_ptr[slot_index]->mode = mode;

  for(i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if(mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[i]->num_session;

    for(j = 0; (num_sessions > 0) && (j < MMGSDI_MAX_SESSION_INFO); j++)
    {
      session_ptr =
        mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

      if(session_ptr == NULL)
      {
        /*Invalid Session, Continue to next*/
        continue;
      }
      num_sessions--;

      session_ptr = mmgsdi_util_cmp_aid_get_session_ptr(
                      i,
                      j,
                      &mmgsdi_refresh_info_ptr[slot_index]->aid,
                      mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                      TRUE);

      if(session_ptr == NULL)
      {
        /*This client is not interested in this app*/
        continue;
      }

      /* Without session event cb ptr clients will not get any refresh events.
         MMGSDI will be stuck waiting for responses and resulting in time out */
      if(session_ptr->session_evt_cb_ptr == NULL)
      {
        continue;
      }

      if(mmgsdi_util_is_card_session(session_ptr->session_type))
      {
        /*Card Sessions need not be sent REFRESH INIT notifications*/
        continue;
      }

      if((session_ptr->session_type == MMGSDI_1X_PROV_PRI_SESSION ||
          session_ptr->session_type == MMGSDI_1X_PROV_SEC_SESSION ||
          session_ptr->session_type == MMGSDI_1X_PROV_TER_SESSION)             &&
         mmgsdi_refresh_info_ptr[slot_index]->aid.app_type == MMGSDI_APP_CSIM  &&
         (!slot_ptr->imsi_m_programmed || slot_ptr->otasp_commit_done))
      {
        gstk_slot = mmgsdi_util_convert_gstk_slot(slot_id);
        if(gstk_slot != GSTK_SLOT_ID_MAX &&
           gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH, gstk_slot))
        {
          skip_voting = TRUE;
          if(slot_ptr->otasp_commit_done)
          {
            slot_ptr->otasp_commit_done = FALSE;
            UIM_MSG_HIGH_0("Skipping vote for Refresh for CSIM app after first OTASP commit");
          }
          if(!slot_ptr->imsi_m_programmed)
          {
            UIM_MSG_HIGH_0("Skipping vote for Refresh for CSIM app since IMSI_M is not programmed");
          }
        }
      }

      if(session_ptr->refresh_data.vote_for_init && !skip_voting)
      {
        wait_for_rsp = TRUE;
        /*Add Node to Linked List*/
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                NULL,
                                session_ptr->refresh_data.proceed_refresh,
                                MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT);
      }
      else if(session_ptr->refresh_data.proceed_refresh)
      {
        wait_for_pdown_cfm = TRUE;
        /*Add Node to Linked List*/
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                NULL,
                                session_ptr->refresh_data.proceed_refresh,
                                MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH);
      }
      else
      {
        mmgsdi_refresh_add_node(session_ptr->session_id,
                                NULL,
                                FALSE,
                                MMGSDI_REFRESH_STAGE_START);
      }
    }
  }

  if(mmgsdi_refresh_info_ptr[slot_index]->node_ptr == NULL)
  {
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;

    if(req_ptr->aid.aid.data_len == 0)
    {
      UIM_MSG_HIGH_0("Refresh Success");
      return MMGSDI_REFRESH_SUCCESS;
    }
    else
    {
      UIM_MSG_HIGH_0("Refresh - No AID Active");
      return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
    }
  }

  UIM_MSG_HIGH_2("Refresh: Need to wait for OK_TO_INIT: 0x%x, OK_TO_PROCEED: 0x%x",
                 wait_for_rsp, wait_for_pdown_cfm);
  if(wait_for_rsp)
  {
    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    =
      MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;

    extra_param_ptr->refresh_data.send_evt = TRUE;

    slot_ptr->refresh_state                =
      MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE;
  }
  else if(wait_for_pdown_cfm)
  {
    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    =
      MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH;

    extra_param_ptr->refresh_data.send_evt = TRUE;

    slot_ptr->refresh_state                =
      MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE;
  }
  else
  {
    UIM_MSG_HIGH_0("Refresh - No wait for OK_TO_INIT required, staring refresh");

    /*No client has voted for init*/
    extra_param_ptr->refresh_data.stage    =
      MMGSDI_REFRESH_STAGE_START;

    extra_param_ptr->refresh_data.send_evt = TRUE;

    slot_ptr->refresh_state                =
      MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
  }

  return MMGSDI_REFRESH_IN_PROGRESS;
}/*mmgsdi_refresh_process_init*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_PROCESS_INIT_FCN

  DESCRIPTION:
    A Refresh INIT FCN command has been received

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_init_fcn(
  mmgsdi_slot_id_enum_type             slot_id,
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr)
{
  mmgsdi_session_info_type                *session_ptr             = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status            = MMGSDI_ERROR;
  mmgsdi_internal_refresh_file_list_type  refresh_files            = {0};
  mmgsdi_internal_refresh_file_list_type  client_spc_refresh_files = {0};
  uint8                                   client_index             = 0;
  uint8                                   session_index            = 0;
  mmgsdi_refresh_stage_enum_type          stage                    = MMGSDI_REFRESH_STAGE_START;
  mmgsdi_refresh_node_type               *temp_ptr                 = NULL;
  boolean                                 wait_for_rsp             = FALSE;
  boolean                                 wait_for_pdown_cfm       = FALSE;
  boolean                                 wait_for_fcn_rsp         = FALSE;
  boolean                                 need_to_send_fcn_flag    = FALSE;
  mmgsdi_refresh_mode_enum_type           modified_refresh_mode    = MMGSDI_REFRESH_MODE_ENUM_MAX;
  mmgsdi_len_type                         num_sessions             = 0;
  uint8                                   slot_index               = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_0("FUNCTION mmgsdi_refresh_process_init_fcn");

  /* Parameter check */
  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid slot id, 0x%x", slot_id);
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    UIM_MSG_ERR_1("Invalid slot index, 0x%x", slot_index);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  /*----------------------------------------------------------------
    Convert to MMGSDI File Enums
  ----------------------------------------------------------------*/
  /* Extract file list from refresh request */
  if(mmgsdi_refresh_malloc_and_convert_file_path(req_ptr, slot_index, &refresh_files)
     != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(refresh_files.file_list_ptr == NULL || refresh_files.file_path_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_path_ptr);

    UIM_MSG_ERR_0("Refresh file ptr is NULL");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /*----------------------------------------------------------------
    Check if it is provisioning app - if it is ,
    then check if the FCN files are on the call stack, if it is
    then change the mode to init if not leave it as init+fcn
    ----------------------------------------------------------------*/
  modified_refresh_mode = req_ptr->refresh_mode;
  mmgsdi_status = mmgsdi_refresh_change_mode_if_required(req_ptr,
                                                         slot_index,
                                                         &modified_refresh_mode,
                                                         &refresh_files);

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     modified_refresh_mode == MMGSDI_REFRESH_NAA_INIT)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);

    UIM_MSG_HIGH_0("Refresh: Mode changed from INIT+FCN to INIT");
    mmgsdi_status = mmgsdi_refresh_process_init(slot_id,
                                                slot_ptr,
                                                req_ptr,
                                                extra_param_ptr,
                                                modified_refresh_mode);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_refresh_malloc_and_copy_file_list(refresh_files, slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
    return mmgsdi_status;
  }
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init = TRUE;
  mmgsdi_refresh_info_ptr[slot_index]->refresh_data.proceed_refresh = TRUE;

  /* Mode here may be different from the original refresh request
     mode req_ptr->refresh_mode */
  mmgsdi_refresh_info_ptr[slot_index]->mode = MMGSDI_REFRESH_NAA_INIT_FCN;

  for(client_index = 0; client_index < MMGSDI_MAX_CLIENT_INFO; client_index++)
  {
    if(mmgsdi_client_id_reg_table[client_index] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[client_index]->num_session;

    for(session_index = 0; (num_sessions > 0) &&
        (session_index < MMGSDI_MAX_SESSION_INFO); session_index++)
    {
      session_ptr =
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

      if(session_ptr == NULL)
      {
        /*Invalid Session, Continue to next*/
        continue;
      }
      num_sessions--;

      /* Without session event cb ptr clients will not get any refresh events.
         MMGSDI will be stuck waiting for responses and resulting in time out */
      if(session_ptr->session_evt_cb_ptr == NULL)
      {
        continue;
      }

      stage = MMGSDI_REFRESH_STAGE_START;
      mmgsdi_status = mmgsdi_refresh_malloc_and_calc_client_spc_files(
                        req_ptr,
                        &session_ptr,
                        &refresh_files,
                        &client_spc_refresh_files,
                        &stage,
                        client_index,
                        session_index,
                        &need_to_send_fcn_flag,
                        slot_index);
      if(mmgsdi_status == MMGSDI_NOT_FOUND)
      {
        continue;
      }
      else if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        /* Clean up refresh files */
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
        return mmgsdi_status;
      }
      /* Vote for init if the Session has regsitered for the same, and is
         not a card Session */
      if((session_ptr->refresh_data.vote_for_init) &&
         !mmgsdi_util_is_card_session(session_ptr->session_type))
      {
        wait_for_rsp = TRUE;
        stage = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;

        /*Add Node to Linked List*/
        mmgsdi_refresh_add_node(session_ptr->session_id,
                               &client_spc_refresh_files,
                               session_ptr->refresh_data.proceed_refresh,
                               stage);

      }
      else if((session_ptr->refresh_data.proceed_refresh) &&
              !mmgsdi_util_is_card_session(session_ptr->session_type))
      {
        wait_for_pdown_cfm = TRUE;
        stage = MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH;

        /*Add Node to Linked List*/
        mmgsdi_refresh_add_node(session_ptr->session_id,
                               &client_spc_refresh_files,
                               session_ptr->refresh_data.proceed_refresh,
                               stage);
      }
      else if( stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN )
      {
        wait_for_fcn_rsp = TRUE;
        /*Add Node to Linked List*/
        mmgsdi_refresh_add_node(session_ptr->session_id,
                               &client_spc_refresh_files,
                               FALSE,
                               stage);
      }
      else
      {
        if(!mmgsdi_util_is_card_session(session_ptr->session_type))
        {
            /*Add Node to Linked List*/
            mmgsdi_refresh_add_node(session_ptr->session_id,
                                   &client_spc_refresh_files,
                                   FALSE,
                                   MMGSDI_REFRESH_STAGE_START);
        }
      }

      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_spc_refresh_files.file_path_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_spc_refresh_files.file_list_ptr);
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);

  if(need_to_send_fcn_flag == FALSE)
  {
    wait_for_fcn_rsp = FALSE;
    temp_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;

    /* We need not do OK_TO_FCN voting here.Hence change the stage of nodes
       with MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN */

    while(temp_ptr != NULL)
    {
      if(temp_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN )
      {
        temp_ptr->stage = MMGSDI_REFRESH_STAGE_START;
      }
      temp_ptr= temp_ptr->next_ptr;
    }
    mmgsdi_refresh_info_ptr[slot_index]->mode = MMGSDI_REFRESH_NAA_INIT;
  }

  if(mmgsdi_refresh_info_ptr[slot_index]->node_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Refresh: No Node interested in FCN file");

    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;

    if(req_ptr->aid.aid.data_len == 0)
    {
      UIM_MSG_HIGH_0("Refresh Success");
      return MMGSDI_REFRESH_SUCCESS;
    }
    else
    {
      UIM_MSG_HIGH_0("Refresh - No AID Active");
      return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
    }
  }

  UIM_MSG_HIGH_3("Refresh: Need to wait for OK_TO_INIT: 0x%x, OK_TO_PROCEED: 0x%x, OK_TO_FCN: 0x%x",
                 wait_for_rsp, wait_for_pdown_cfm, wait_for_fcn_rsp);
  if(wait_for_rsp)
  {
    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    =
      MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;

    extra_param_ptr->refresh_data.send_evt = TRUE;

    slot_ptr->refresh_state                =
      MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE;
  }
  else if(wait_for_pdown_cfm)
  {
    /*Atleast we need to wait for client to respond*/
    extra_param_ptr->refresh_data.stage    =
      MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH;

    extra_param_ptr->refresh_data.send_evt = TRUE;

    slot_ptr->refresh_state                =
      MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE;
  }
  else if(wait_for_fcn_rsp)
  {
    /*Wait for client to respond*/
    extra_param_ptr->refresh_data.stage    =
      MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;

    extra_param_ptr->refresh_data.send_evt = TRUE;

    slot_ptr->refresh_state                =
      MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE;
  }
  else
  {
    UIM_MSG_HIGH_0("Refresh - No wait for OK_TO_INIT required, staring refresh");

    /*No client has voted for init*/
    extra_param_ptr->refresh_data.stage    =
      MMGSDI_REFRESH_STAGE_START;

    extra_param_ptr->refresh_data.send_evt = TRUE;

    slot_ptr->refresh_state                =
      MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
  }

  return MMGSDI_REFRESH_IN_PROGRESS;

}/*mmgsdi_refresh_process_init_fcn*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_PROCESS_FCN

  DESCRIPTION:
    A Refresh FCN Command has been received
    1.  If the mode is FCN and it is prov app and it is a call stack file,
        then change the mode to INIT
    2.  Build the Linked list for Clients that will be involved in this refresh
        mode
    3.  Transition into one of the following states –
       •    WAIT_FOR_OK_TO_FCN_STATE:
           a.   If it is FCN node and there are clients interested in files in
                FCN
           b.   Send OK_TO_FCN event to relevant clients
       •    READY_STATE:
           Success Case  - If no clients are interested in any files listed in
                           FCN, clean up, send TR

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
  =============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_fcn(
  mmgsdi_slot_id_enum_type             slot_id,
  mmgsdi_slot_data_type               *slot_ptr,
  const mmgsdi_refresh_card_req_type  *req_ptr,
  mmgsdi_client_req_extra_info_type   *extra_param_ptr)
{
  mmgsdi_session_info_type               *session_ptr           = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_refresh_mode_enum_type           modified_refresh_mode = MMGSDI_REFRESH_MODE_ENUM_MAX;
  uint8                                   client_index          = 0;
  uint8                                   session_index         = 0;
  mmgsdi_len_type                         num_sessions          = 0;
  mmgsdi_len_type                         num_active_sessions   = 0;
  mmgsdi_internal_refresh_file_list_type  refresh_files         = {0};
  uint8                                   slot_index            = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_1("FUNCTION mmgsdi_refresh_process_fcn for slot: 0x%x", slot_id);

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    UIM_MSG_ERR_1("Invalid slot index, 0x%x", slot_index);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  /*----------------------------------------------------------------
    Convert to MMGSDI File Enums
  ----------------------------------------------------------------*/
  /* Extract file list from refresh request */
  if(mmgsdi_refresh_malloc_and_convert_file_path(req_ptr, slot_index, &refresh_files)
     != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(refresh_files.file_list_ptr == NULL || refresh_files.file_path_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(refresh_files.file_path_ptr);

    UIM_MSG_ERR_0("Refresh file ptr is NULL");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /*----------------------------------------------------------------
    Check if it is provisioning app - if it is ,
    then check if the files is on the call stack, if it is
    then change the mode to init or init+fcn
    ----------------------------------------------------------------*/
  modified_refresh_mode = req_ptr->refresh_mode;
  mmgsdi_status = mmgsdi_refresh_change_mode_if_required(req_ptr,
                                                         slot_index,
                                                         &modified_refresh_mode,
                                                         &refresh_files);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
    return mmgsdi_status;
  }

  if(modified_refresh_mode != req_ptr->refresh_mode )
  {
    UIM_MSG_HIGH_2("Refresh: Refresh mode has changed orig_mode:0x%x modified_mode 0x%x ",
                   req_ptr->refresh_mode, modified_refresh_mode);
    if(modified_refresh_mode == MMGSDI_REFRESH_NAA_INIT_FCN)
    {
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
      mmgsdi_status = mmgsdi_refresh_process_init_fcn(slot_id,
                                                      slot_ptr,
                                                      req_ptr,
                                                      extra_param_ptr);
      return mmgsdi_status;
    }
    else if(modified_refresh_mode == MMGSDI_REFRESH_NAA_INIT)
    {
      (void)mmgsdi_refresh_malloc_and_copy_file_list(refresh_files, slot_index);
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
      mmgsdi_status = mmgsdi_refresh_process_init(slot_id,
                                                  slot_ptr,
                                                  req_ptr,
                                                  extra_param_ptr,
                                                  modified_refresh_mode);
      return mmgsdi_status;
    }
    else
    {
      UIM_MSG_HIGH_1("Refresh: Bad refresh mode at this point 0x%x",
                     modified_refresh_mode);
    }
  }

  /* At this point, the refresh mode is still FCN only */
  mmgsdi_status = mmgsdi_refresh_malloc_and_copy_file_list(refresh_files, slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
    return mmgsdi_status;
  }

  /*--------------------------------------------------------------------
  Build List of Clients interested in this file
  ----------------------------------------------------------------------*/
  for(client_index = 0; client_index < MMGSDI_MAX_CLIENT_INFO; client_index++)
  {
    if (mmgsdi_client_id_reg_table[client_index] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[client_index]->num_session;

    for (session_index = 0; (num_sessions > 0) &&
         (session_index < MMGSDI_MAX_SESSION_INFO); session_index++)
    {
      session_ptr =
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

      if(session_ptr == NULL)
      {
        /*Invalid Session, Continue to next*/
        continue;
      }
      num_sessions--;

      session_ptr = mmgsdi_util_cmp_aid_get_session_ptr(
                      client_index,
                      session_index,
                      &mmgsdi_refresh_info_ptr[slot_index]->aid,
                      mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id,
                      TRUE);

      if(session_ptr == NULL)
      {
        /* Skipping AID comparision on card session for REFRESH NAA FCN and
           NAA INIT FCN modes */
        session_ptr = mmgsdi_refresh_card_get_session_ptr(
                          client_index,
                          session_index,
                          mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id);
        if(session_ptr == NULL)
        {
          /*Invalid Session, Continue to next*/
          continue;
        }
      }

      /* Without session event cb ptr clients will not get any refresh events.
         MMGSDI will be stuck waiting for responses and resulting in time out */
      if(session_ptr->session_evt_cb_ptr == NULL)
      {
        continue;
      }

      /* Count of number of active sessions corresponding to the
         application on which the FCN is being processed */
      num_active_sessions++;

      mmgsdi_status = mmgsdi_refresh_build_node_list(session_ptr,
                                                     &refresh_files);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);
        return mmgsdi_status;
      }
    }
  }/*end for*/

  MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_list_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(refresh_files.file_path_ptr);

  if(mmgsdi_refresh_info_ptr[slot_index]->node_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Refresh: No Node interested in FCN file");
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = FALSE;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;

    /* If a valid AID (i.e. application present on the card) is provided
       in the REFRESH request, but the application is currently inactive,
       return a specific Success status */
    if((req_ptr->aid.aid.data_len != 0) && (num_active_sessions == 0))
    {
      UIM_MSG_HIGH_0("Refresh - No AID Active");
      return MMGSDI_REFRESH_SUCCESS_NO_AID_ACTIVE;
    }
    /* In cases where:
       - AID is not provided in the REFRESH request (ICC cards/UICC cards
         where no AID is indicated in the REFRESH request)
       - AID is present and active on the card, but no client is
         interested in the files being REFRESHed,
       Return a generic Success status */
    else
    {
      UIM_MSG_HIGH_0("Refresh Success");
      return MMGSDI_REFRESH_SUCCESS;
    }
  }

  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
  extra_param_ptr->refresh_data.send_evt = TRUE;
  slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE;


  return MMGSDI_REFRESH_IN_PROGRESS;
}/*mmgsdi_refresh_process_fcn*/


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_PROCESS_STEERING

  DESCRIPTION:
    A Refresh Command with mode STEERING OF ROAMING has been received
      Check for the required data
      Update refresh controlling data

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_REFRESH_IN_PROGRESS: Command processing was successfully started.
    MMGSDI_INCORRECT_PARAMS   : The parameters supplied to the API are not
                                within appropriate ranges.

  SIDE EFFECTS:
    None
  =============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_steering(
  mmgsdi_slot_id_enum_type              slot_id,
  mmgsdi_slot_data_type               * slot_ptr,
  const mmgsdi_refresh_card_req_type  * req_ptr,
  mmgsdi_client_req_extra_info_type   * extra_param_ptr)
{

  UIM_MSG_HIGH_0("FUNCTION mmgsdi_refresh_process_steering");

  MMGSDIUTIL_RETURN_IF_NULL_4(slot_ptr,
                              req_ptr,
                              extra_param_ptr,
                              req_ptr->plmn_list.data_ptr);

  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
  extra_param_ptr->refresh_data.send_evt = TRUE;
  slot_ptr->refresh_state                = MMGSDI_REFRESH_STEERING_IN_PROGRESS_STATE;

  return MMGSDI_REFRESH_IN_PROGRESS;
} /* mmgsdi_refresh_process_steering */


/*=============================================================================
     Processing Refresh Requests
 =============================================================================*/
/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_REQ_PROCESS_REG_BY_PATH

  DESCRIPTION:
    This function will process the registrations sent by the client using file
    paths

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_reg_by_path(
  const mmgsdi_refresh_reg_req_type           *reg_req_ptr,
  mmgsdi_session_info_type                    *session_ptr,
  boolean                                     is_reg_before
)
{
  uint8                     i                = 0;
  mmgsdi_path_type          *file_path_ptr   = NULL;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_ERROR;
  uint32                    path_len         = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(reg_req_ptr, session_ptr);

  file_path_ptr = session_ptr->refresh_data.refresh_files.file_path_ptr;

  if((file_path_ptr == NULL) && (is_reg_before == FALSE) &&
     (reg_req_ptr->refresh_files.file_path_ptr != NULL ))
  {
    /* Client registers for the first time using file paths */
    session_ptr->refresh_data.reg_by_path = TRUE;
    mmgsdi_status = mmgsdi_util_malloc_and_copy_file_path_array(
                      &file_path_ptr,
                      reg_req_ptr->refresh_files.file_path_ptr,
                      reg_req_ptr->refresh_files.num_file_paths);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }

    session_ptr->refresh_data.refresh_files.file_path_ptr = file_path_ptr;

    /*This is the first time that the client is registering with
      MMGSDI for Refresh*/
    session_ptr->refresh_data.refresh_files.num_file_paths =
      reg_req_ptr->refresh_files.num_file_paths;
    session_ptr->refresh_data.refresh_files.num_files = 0;

  } /* file_path_ptr == NULL is_reg_before == FALSE
        && req.file_path_ptr != NULL */
  else if((file_path_ptr != NULL) && (is_reg_before) &&
    (reg_req_ptr->refresh_files.file_path_ptr != NULL))
  {

    /* Client tries to modify existing registration using file paths */
    /*Need to append to the list*/
    uint32 num_dup_files    = 0;
    uint32 total_files      = session_ptr->refresh_data.refresh_files.num_file_paths;
    uint32 orig_total_files = session_ptr->refresh_data.refresh_files.num_file_paths;
    uint32 j                = 0,k=0;
    boolean dup             = FALSE;

    /*Find out the actual number of files*/
    for(j=0;j<reg_req_ptr->refresh_files.num_file_paths;j++)
    {
      if(reg_req_ptr->refresh_files.file_path_ptr[j].path_len == 0)
      {
        UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len", j);
        return MMGSDI_ERROR;
      }

      for(k=0;k<session_ptr->refresh_data.refresh_files.num_file_paths;k++)
      {
        if(reg_req_ptr->refresh_files.file_path_ptr[j].path_len ==
           session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_len)
        {
          path_len =
            int32touint32(reg_req_ptr->refresh_files.file_path_ptr[j].path_len);
          if(!memcmp(
              reg_req_ptr->refresh_files.file_path_ptr[j].path_buf,
              session_ptr->refresh_data.refresh_files.file_path_ptr[k].path_buf,
              MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
          {
            num_dup_files++;
            break;
          }
        }
      }
    }

    session_ptr->refresh_data.refresh_files.num_file_paths +=
      reg_req_ptr->refresh_files.num_file_paths - num_dup_files;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      file_path_ptr,
      (sizeof(mmgsdi_path_type) * session_ptr->refresh_data.refresh_files.num_file_paths));

    if(file_path_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    for(j = 0; j < total_files; j++)
    {
      if(session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len
          == 0)
      {
        UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len", j);
        /* Free previous memory allocation */
        MMGSDIUTIL_TMC_MEM_FREE(file_path_ptr);
        return MMGSDI_ERROR;
      }
      path_len =
        int32touint32(session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len);
      mmgsdi_memscpy(file_path_ptr[j].path_buf,
        sizeof(file_path_ptr[j].path_buf),
        session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_buf,
        path_len* sizeof(uint16));

      file_path_ptr[j].path_len =
        session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_len;
    }

    /*Now Merge the two lists.
      Take the new list, loop through the old list and check any duplicate files
      If any duplicate is found, do not copy*/
    for(i=0;i<reg_req_ptr->refresh_files.num_file_paths;i++)
    {
      dup = FALSE;

      if(reg_req_ptr->refresh_files.file_path_ptr[i].path_len == 0)
      {
        UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len", i);
        /* Free previous memory allocation */
        MMGSDIUTIL_TMC_MEM_FREE(file_path_ptr);
        return MMGSDI_ERROR;
      }
      for(j = 0; j < orig_total_files; j++)
      {
        if(file_path_ptr[j].path_len ==
            reg_req_ptr->refresh_files.file_path_ptr[i].path_len)
        {
          path_len =
            int32touint32(reg_req_ptr->refresh_files.file_path_ptr[i].path_len);
          if(!memcmp(
              session_ptr->refresh_data.refresh_files.file_path_ptr[j].path_buf,
              reg_req_ptr->refresh_files.file_path_ptr[i].path_buf,
              MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
          {
            dup = TRUE;
            break;
          }
        }
      }

      /* The non duplicate files will be added to the end of the list */
      if(!dup && (total_files < session_ptr->refresh_data.refresh_files.num_file_paths))
      {
        path_len =
          int32touint32(reg_req_ptr->refresh_files.file_path_ptr[i].path_len);
        file_path_ptr[total_files].path_len =
          reg_req_ptr->refresh_files.file_path_ptr[i].path_len;
        mmgsdi_memscpy(file_path_ptr[total_files].path_buf,
          sizeof(file_path_ptr[total_files].path_buf),
          reg_req_ptr->refresh_files.file_path_ptr[i].path_buf,
          path_len * sizeof(uint16));
        total_files++;
      }
    }
    MMGSDIUTIL_TMC_MEM_FREE(session_ptr->refresh_data.refresh_files.file_path_ptr);
    session_ptr->refresh_data.refresh_files.file_path_ptr = file_path_ptr;
    session_ptr->refresh_data.refresh_files.num_file_paths = total_files;

  } /* file_path_ptr != NULL && is_reg_before == FALSE &&
        req.file_path_ptr != NULL */
  /* Set number of file enums to zero */
  session_ptr->refresh_data.refresh_files.num_files = 0;
  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_req_process_reg_by_path */


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_REQ_PROCESS_REG_BY_LIST

  DESCRIPTION:
    This function will process the registrations sent by the client using file
    enums

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_reg_by_list(
  const mmgsdi_refresh_reg_req_type           *reg_req_ptr,
  mmgsdi_session_info_type                    *session_ptr,
  boolean                                      is_reg_before
)
{
  mmgsdi_file_enum_type  *file_list_ptr       = NULL;
  mmgsdi_file_enum_type  *valid_file_list_ptr = NULL;
  boolean                 is_prov_session     = FALSE;
  uint32                  num_valid_files     = 0;
  uint32                  num_invalid_files   = 0;
  uint32                  total_files         = 0;
  uint32                  orig_total_files    = 0;
  uint32                  i                   = 0,j=0;
  boolean                 invalid             = FALSE;

  MMGSDIUTIL_RETURN_IF_NULL_2(reg_req_ptr, session_ptr);

  file_list_ptr = session_ptr->refresh_data.refresh_files.file_list_ptr;

  /* Check if the current session is a provisioning one */
  is_prov_session = mmgsdi_util_is_prov_session(session_ptr->session_type);

  /* Registering for the first time using file enums */
  if((file_list_ptr == NULL) && (is_reg_before == FALSE) &&
     (reg_req_ptr->refresh_files.file_list_ptr != NULL))
  {
    session_ptr->refresh_data.reg_by_path = FALSE;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      file_list_ptr,
      (sizeof(mmgsdi_file_enum_type)*reg_req_ptr->refresh_files.num_files));

    if(file_list_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    /* If the session is a provisioning one, verify if the files
       being registered are valid for the session type. Remove
       invalid files from the list */
    if(is_prov_session)
    {
      for(i=0;i<reg_req_ptr->refresh_files.num_files;i++)
      {
        if(mmgsdi_file_is_valid_access(session_ptr->session_type,
            reg_req_ptr->refresh_files.file_list_ptr[i]))
        {
          file_list_ptr[num_valid_files++] =
            reg_req_ptr->refresh_files.file_list_ptr[i];
        }
      }
      /* None of the requested files are valid for the session */
      if(num_valid_files == 0)
      {
        MMGSDIUTIL_TMC_MEM_FREE(file_list_ptr);
      }
      /* Some of the files requested are valid */
      else if(num_valid_files < reg_req_ptr->refresh_files.num_files)
      {
        /* Allocate memory for the valid files */
       MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(valid_file_list_ptr,
         (sizeof(mmgsdi_file_enum_type) * num_valid_files));

        if(valid_file_list_ptr == NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE(file_list_ptr);
          return MMGSDI_ERROR;
        }

        mmgsdi_memscpy(valid_file_list_ptr,
                       sizeof(mmgsdi_file_enum_type) * num_valid_files,
                       file_list_ptr,
                       sizeof(mmgsdi_file_enum_type) * num_valid_files);

        session_ptr->refresh_data.refresh_files.file_list_ptr = valid_file_list_ptr;
        session_ptr->refresh_data.refresh_files.num_files = num_valid_files;

        MMGSDIUTIL_TMC_MEM_FREE(file_list_ptr);
      }
      /* All requested files are valid for the session */
      else
      {
        session_ptr->refresh_data.refresh_files.file_list_ptr = file_list_ptr;
        session_ptr->refresh_data.refresh_files.num_files =
          reg_req_ptr->refresh_files.num_files;
      }
    }
    /* Not a provisioning session, regsiter all files requested */
    else
    {
      mmgsdi_memscpy(file_list_ptr,
        sizeof(mmgsdi_file_enum_type)* reg_req_ptr->refresh_files.num_files,
        reg_req_ptr->refresh_files.file_list_ptr,
        sizeof(mmgsdi_file_enum_type) * reg_req_ptr->refresh_files.num_files);

      session_ptr->refresh_data.refresh_files.file_list_ptr = file_list_ptr;
      session_ptr->refresh_data.refresh_files.num_files =
        reg_req_ptr->refresh_files.num_files;
    }
  }/* file_list_ptr == NULL && is_reg_before == FALSE &&
      req.file_list_ptr != NULL */

  /* Modify the exisiting registration using file enums */
  else if((file_list_ptr != NULL) && (is_reg_before) &&
          (reg_req_ptr->refresh_files.file_list_ptr != NULL))
  {
    total_files      = session_ptr->refresh_data.refresh_files.num_files;
    orig_total_files = session_ptr->refresh_data.refresh_files.num_files;

    /*Find out the number of valid files*/
    for(i=0;i<reg_req_ptr->refresh_files.num_files;i++)
    {
      invalid = FALSE;

      /* If the session is a provisioning one, count the files
         number of files that are valid for the session type */
      if(is_prov_session)
      {
        if(!mmgsdi_file_is_valid_access(session_ptr->session_type,
            reg_req_ptr->refresh_files.file_list_ptr[i]))
        {
          invalid = TRUE;
          num_invalid_files++;
        }
      }
      /* Check for any duplicates in the request */
      if(!invalid)
      {
        for(j=0;j<session_ptr->refresh_data.refresh_files.num_files;j++)
        {
          if(session_ptr->refresh_data.refresh_files.file_list_ptr[j] ==
             reg_req_ptr->refresh_files.file_list_ptr[i])
          {
            num_invalid_files++;
            break;
          }
        }
      }
    }

    session_ptr->refresh_data.refresh_files.num_files +=
      reg_req_ptr->refresh_files.num_files - num_invalid_files;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      file_list_ptr,
      (sizeof(mmgsdi_file_enum_type) * session_ptr->refresh_data.refresh_files.num_files));

    if(file_list_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    mmgsdi_memscpy(file_list_ptr,
      sizeof(mmgsdi_file_enum_type) * session_ptr->refresh_data.refresh_files.num_files,
      session_ptr->refresh_data.refresh_files.file_list_ptr,
      sizeof(mmgsdi_file_enum_type)*total_files);

    /*---------------------------------------------------------------------------
      Now Merge the two lists.
      Take the new list, loop through the old list and check any duplicate
      or invalid files. If any duplicate/invalid file is found, do not copy.
    ---------------------------------------------------------------------------*/
    for(i=0;i<reg_req_ptr->refresh_files.num_files;i++)
    {
      invalid = FALSE;
      if(is_prov_session)
      {
        if(!mmgsdi_file_is_valid_access(session_ptr->session_type,
            reg_req_ptr->refresh_files.file_list_ptr[i]))
        {
          invalid = TRUE;
        }
      }
      if(!invalid)
      {
        for(j=0;j<orig_total_files;j++)
        {
          if(file_list_ptr[j] ==
               reg_req_ptr->refresh_files.file_list_ptr[i])
          {
            invalid = TRUE;
            break;
          }
        }
      }
      if(!invalid && (total_files <
          session_ptr->refresh_data.refresh_files.num_files))
      {
        file_list_ptr[total_files++] =
          reg_req_ptr->refresh_files.file_list_ptr[i];
      }
    }
    MMGSDIUTIL_TMC_MEM_FREE(
      session_ptr->refresh_data.refresh_files.file_list_ptr);
    session_ptr->refresh_data.refresh_files.file_list_ptr = file_list_ptr;
    session_ptr->refresh_data.refresh_files.num_files     = total_files;

  }/* file_list_ptr != NULL && is_reg_before == TRUE &&
      req.file_list_ptr != NULL */

  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_req_process_reg_by_list */


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_REQ_PROCESS_REG

  DESCRIPTION:
    This function will process the registrations sent by the client.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does
                             not have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_reg(
  mmgsdi_refresh_req_type           *req_ptr)
{
  mmgsdi_return_enum_type        mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_info_type      *session_ptr      = NULL;
  mmgsdi_file_enum_type         *file_list_ptr    = NULL;
  mmgsdi_path_type              *file_path_ptr    = NULL;
  mmgsdi_refresh_reg_req_type   *reg_req_ptr      = NULL;
  boolean                        is_reg_by_path    = FALSE;
  boolean                        is_reg_before     = FALSE;
  int32                          client_index      = 0;
  int32                          session_index     = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  reg_req_ptr = &req_ptr->refresh.reg_req;

  /*Get the Session Pointer*/
  mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                    req_ptr->request_header.session_id,
                    &client_index,
                    &session_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Refresh: Bad Client ID");
    return MMGSDI_ERROR;
  }

  /* mmgsdi_util_get_client_and_session_index success guarantees valid dereferences
     into mmgsdi_client_id_reg_table and session_info_ptr */
  session_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if(session_ptr == NULL)
  {
    UIM_MSG_ERR_1("NULL session_ptr, session_id=0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

  /*Allow client registration only if it has registered a valid event callback*/
  if(session_ptr->session_evt_cb_ptr == NULL)
  {
    UIM_MSG_ERR_1("Refresh: NULL session event callback, session_id=0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

  /* ---------------------------------------------------------------------
    1. Client should register or deregister either using file enum or file
        path only
    2. At least file path list or file enum list should hold
       one or more files
    3. If either file path ptr or file enum ptr is not null then client
       has registered before
    4. If client registered earlier by file enums and is
       now trying to register by path or vice versa then
       return error
    5. a. File Path PRovided - let Client Register by File path
       OR
       b. File List Provided - let Client Register with File List
       c. Re-Register with File List
    6. Clients registering for steering of roaming should have a GW session
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
    reg_req_ptr->refresh_files.file_list_ptr,
    reg_req_ptr->refresh_files.file_path_ptr,
    mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Refresh: Session 0x%x has incompatible file path/enum list",
                  req_ptr->request_header.session_id);
    return mmgsdi_status;
  }

  file_list_ptr = session_ptr->refresh_data.refresh_files.file_list_ptr;
  file_path_ptr = session_ptr->refresh_data.refresh_files.file_path_ptr;

  if(reg_req_ptr->steering_of_roaming)
  {
    if((session_ptr->session_type == MMGSDI_GW_PROV_PRI_SESSION) ||
       (session_ptr->session_type == MMGSDI_GW_PROV_SEC_SESSION) ||
       (session_ptr->session_type == MMGSDI_GW_PROV_TER_SESSION))
    {
      session_ptr->steering_of_roaming = TRUE;
      session_ptr->steering_complete_rcvd = TRUE;
    }
    else
    {
      UIM_MSG_ERR_1("Invalid session type for refresh mode steering: SessionType = 0x%x",
                    session_ptr->session_type);
      return MMGSDI_ERROR;
    }
  }

  /*---------------------------------------------------------------------------
    Step #2 Registration can happen only via one method
  ---------------------------------------------------------------------------*/
  if(((reg_req_ptr->refresh_files.num_files != 0) ||
      (reg_req_ptr->refresh_files.num_file_paths != 0)) &&
     ((reg_req_ptr->refresh_files.file_list_ptr!= NULL) ||
      (reg_req_ptr->refresh_files.file_path_ptr!= NULL)))
  {
   /*---------------------------------------------------------------------------
     Step #3 Has Client Registered Before?
    ---------------------------------------------------------------------------*/
    if((file_list_ptr != NULL) || (file_path_ptr != NULL))
    {
      is_reg_before = TRUE;
      is_reg_by_path = session_ptr->refresh_data.reg_by_path;
    }
    /*---------------------------------------------------------------------------
       Step #4 Check with previous Registrations
      ---------------------------------------------------------------------------*/
    if(((reg_req_ptr->refresh_files.file_list_ptr != NULL) &&
        (is_reg_by_path) && (is_reg_before))       ||
       ((reg_req_ptr->refresh_files.file_path_ptr != NULL) &&
        (is_reg_by_path == FALSE) && (is_reg_before)))
    {
      UIM_MSG_ERR_1("Refresh: Changing reg mode not allowed, curr mode - reg_by_path=0x%x",
                    is_reg_by_path);
      return MMGSDI_ERROR;
    }
    /*---------------------------------------------------------------------------
       Step #5a Let Client Register by Path
      ---------------------------------------------------------------------------*/
    if(reg_req_ptr->refresh_files.file_path_ptr != NULL)
    {
      mmgsdi_status = mmgsdi_refresh_req_process_reg_by_path(
                        reg_req_ptr,
                        session_ptr,
                        is_reg_before);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Refresh: proc_reg_by_path failed, status=0x%x",
                      mmgsdi_status);
        return mmgsdi_status;
      }
    }

    /*---------------------------------------------------------------------------
       Step #5b Let Client Register by Enum
       Step #5c Client is trying to modify existing registration using file enums
      ---------------------------------------------------------------------------*/
    if(reg_req_ptr->refresh_files.file_list_ptr != NULL)
    {
      mmgsdi_status = mmgsdi_refresh_req_process_reg_by_list(
                        reg_req_ptr,
                        session_ptr,
                        is_reg_before);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
    }
  }/*if(req_ptr->refresh_files.num_files != 0) &&
     (req_ptr->refresh_files.file_list_ptr != NULL) ||
     (req_ptr->refresh_files.file_path_ptr != NULL) */

  session_ptr->refresh_data.vote_for_init   = reg_req_ptr->vote_for_init;
  session_ptr->refresh_data.proceed_refresh = reg_req_ptr->ok_to_proceed;
  session_ptr->refresh_data.is_reg = TRUE;
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_req_process_reg*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_REQ_PROCESS_REG_ALL

  DESCRIPTION:
    This function will process the registrations sent by the client for all
    files.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does
                             not have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_reg_all(
  mmgsdi_refresh_req_type           *req_ptr)
{
  mmgsdi_return_enum_type        mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_info_type      *session_ptr      = NULL;
  mmgsdi_refresh_reg_req_type   *reg_req_ptr      = NULL;
  int32                          client_index      = 0;
  int32                          session_index     = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  reg_req_ptr = &req_ptr->refresh.reg_req;

  /* Get the Session Pointer */
  mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                    req_ptr->request_header.session_id,
                    &client_index,
                    &session_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Refresh: Bad Client ID");
    return MMGSDI_ERROR;
  }

  /* mmgsdi_util_get_client_and_session_index success guarantees valid dereferences
     into mmgsdi_client_id_reg_table and session_info_ptr */
  session_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if(session_ptr == NULL)
  {
    UIM_MSG_ERR_1("NULL session_ptr, session_id=0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

  /* Allow client registration only if it has registered a valid event callback */
  if(session_ptr->session_evt_cb_ptr == NULL)
  {
    UIM_MSG_ERR_1("Refresh: NULL session event callback, session_id=0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

  /* ---------------------------------------------------------------------
    1. Client registers using reg_all_files option
    2. If client already registered earlier then return error
    3. Clients registering for steering of roaming should have a GW session
  ---------------------------------------------------------------------------*/

  /* Proceed with client registration only if client hasn't done before,
     otherwise just ignore */
  if(session_ptr->refresh_data.reg_all_files)
  {
    UIM_MSG_ERR_1("Refresh: Already registered for all files, session_id=0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_SUCCESS;
  }

  /* Update client's parameters */
  session_ptr->refresh_data.reg_all_files   = TRUE;
  session_ptr->refresh_data.is_reg          = TRUE;
  session_ptr->refresh_data.vote_for_init   = reg_req_ptr->vote_for_init;

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_req_process_reg_all*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_REQ_PROCESS_DEREG

  DESCRIPTION:
    This function will registration refresh requests

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_dereg(
  mmgsdi_refresh_req_type           *req_ptr)
{
  mmgsdi_return_enum_type      mmgsdi_status            = MMGSDI_ERROR;
  mmgsdi_session_info_type    *session_ptr              = NULL;
  mmgsdi_channel_info_type    *channel_ptr              = NULL;
  mmgsdi_file_enum_type       *file_list_ptr            = NULL;
  mmgsdi_path_type            *file_path_ptr            = NULL;
  mmgsdi_refresh_reg_req_type *dereg_req_ptr            = NULL;
  boolean                      is_reg_by_path           = FALSE;
  uint32                       path_len                 = 0;

  UIM_MSG_HIGH_0("MMGSDI_REFRESH_PROCESS_DEREG");

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                   - Cannot be NULL
    2. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  dereg_req_ptr = &req_ptr->refresh.reg_req;

  /* Only file enum ptr or file path ptr should be valid ptr */
  MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
    dereg_req_ptr->refresh_files.file_list_ptr,
    dereg_req_ptr->refresh_files.file_path_ptr,
    mmgsdi_status);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Refresh: Session 0x%x has incompatible file path/enum list",
                  req_ptr->request_header.session_id);
    return mmgsdi_status;
  }

  /*Get the Session Pointer*/
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                   req_ptr->request_header.session_id,
                   &session_ptr,
                   &channel_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS  || session_ptr == NULL)
  {
    return mmgsdi_status;
  }
  is_reg_by_path = session_ptr->refresh_data.reg_by_path;

  /*---------------------------------------------------------
   If client sends file enum for dereg and is registered by path
     or vice versa then return an error
   ----------------------------------------------------------*/
  if(((dereg_req_ptr->refresh_files.file_list_ptr != NULL) &&
      (is_reg_by_path)) ||
     ((dereg_req_ptr->refresh_files.file_path_ptr != NULL) &&
      (!is_reg_by_path)))
  {
    UIM_MSG_ERR_0("Refresh: Enum/path mode mismatch compared to reg");
    return MMGSDI_ERROR;
  }

  /*-------------------------------------------------------------
    Client is currently registered using file enums
   -------------------------------------------------------------*/
  if(dereg_req_ptr->refresh_files.num_files != 0 &&
     dereg_req_ptr->refresh_files.file_list_ptr != NULL)
  {
    file_list_ptr = session_ptr->refresh_data.refresh_files.file_list_ptr;

    if(file_list_ptr != NULL)
    {
      /*Need to remove files from the list*/
      uint32 total_files   = 0;
      uint32 i=0, j=0, k=0;
      boolean file_found =FALSE;

      /*Count the total number of common files*/
      for(i=0;i<session_ptr->refresh_data.refresh_files.num_files;i++)
      {
        for(j=0;j<dereg_req_ptr->refresh_files.num_files;j++)
        {
          if(session_ptr->refresh_data.refresh_files.file_list_ptr[i] ==
             dereg_req_ptr->refresh_files.file_list_ptr[j])
          {
            total_files++;
            break;
          }
        }
      }

      /*This is now the new total number of files that the client
        is interested in*/
      total_files =
        session_ptr->refresh_data.refresh_files.num_files - total_files;

      if(total_files == 0)
      {
        /*All files dergistered*/
        MMGSDIUTIL_TMC_MEM_FREE(
          session_ptr->refresh_data.refresh_files.file_list_ptr);
        session_ptr->refresh_data.refresh_files.num_files = 0;
        session_ptr->steering_of_roaming    = FALSE;
        session_ptr->steering_complete_rcvd = FALSE;
      }
      else
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(file_list_ptr,
          (sizeof(mmgsdi_file_enum_type) * total_files));

        if(file_list_ptr == NULL)
        {
          return MMGSDI_ERROR;
        }

        /*Now add only those files that are not in the new list but are in the
          original list*/
        for(i=0;i<session_ptr->refresh_data.refresh_files.num_files;i++)
        {
          file_found = FALSE;
          for(j=0;j<dereg_req_ptr->refresh_files.num_files;j++)
          {
            if(session_ptr->refresh_data.refresh_files.file_list_ptr[i] ==
               dereg_req_ptr->refresh_files.file_list_ptr[j])
            {
              file_found = TRUE;
              break;
            }
          }
          if(!file_found && k<total_files)
          {
            file_list_ptr[k++] =
              session_ptr->refresh_data.refresh_files.file_list_ptr[i];
          }
        }

        MMGSDIUTIL_TMC_MEM_FREE(session_ptr->refresh_data.refresh_files.file_list_ptr);
        session_ptr->refresh_data.refresh_files.file_list_ptr   = file_list_ptr;
        session_ptr->refresh_data.refresh_files.num_files       = total_files;
      }/*total_files > 0*/

    }/*file_list_ptr != NULL*/
  }/*if(req_ptr->refresh_files.num_files != 0 &&
     req_ptr->refresh_files.file_list_ptr != NULL)*/

  /*---------------------------------------------------------------------------
    Process File path Request
   --------------------------------------------------------------------------*/
  if(dereg_req_ptr->refresh_files.num_file_paths != 0 &&
     dereg_req_ptr->refresh_files.file_path_ptr != NULL)
  {
    file_path_ptr = session_ptr->refresh_data.refresh_files.file_path_ptr;

    if(file_path_ptr != NULL)
    {
      /*Need to remove files from the list*/
      uint32 total_files   = 0;
      uint32 i=0, j=0, k=0;
      boolean file_found =FALSE;

      /*Count the total number of common files*/
      for(i=0;i<session_ptr->refresh_data.refresh_files.num_file_paths;i++)
      {
        if(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len
           == 0)
        {
          UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len", j);
          return MMGSDI_ERROR;
        }
        for(j=0;j<dereg_req_ptr->refresh_files.num_file_paths;j++)
        {
          if(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len ==
             dereg_req_ptr->refresh_files.file_path_ptr[j].path_len)
          {
            path_len =
              int32touint32(dereg_req_ptr->refresh_files.file_path_ptr[j].path_len);
            if(!memcmp(
                  session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_buf,
                  dereg_req_ptr->refresh_files.file_path_ptr[j].path_buf,
                  MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
            {
              total_files++;
              break;
            }
          }
        }
      }

      /*This is now the new total number of files that the client
        is interested in*/
      total_files =
        session_ptr->refresh_data.refresh_files.num_file_paths - total_files;

      if(total_files == 0)
      {
        /*All files dergistered*/
        MMGSDIUTIL_TMC_MEM_FREE(
          session_ptr->refresh_data.refresh_files.file_path_ptr);
        session_ptr->refresh_data.refresh_files.num_file_paths = 0;
        session_ptr->steering_of_roaming    = FALSE;
        session_ptr->steering_complete_rcvd = FALSE;
      }
      else
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(file_path_ptr,
          (sizeof(mmgsdi_path_type) * total_files));

        if(file_path_ptr == NULL)
        {
          return MMGSDI_ERROR;
        }

        /*Now add only those files that are not in the new list but are in the
          original list*/
        for(i=0;i<session_ptr->refresh_data.refresh_files.num_file_paths;i++)
        {
          file_found = FALSE;

          if(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len
             == 0)
          {
            UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len", i);
            MMGSDIUTIL_TMC_MEM_FREE(file_path_ptr);
            return MMGSDI_ERROR;
          }

          for(j=0;j<dereg_req_ptr->refresh_files.num_file_paths;j++)
          {
            if(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len ==
               dereg_req_ptr->refresh_files.file_path_ptr[j].path_len)
            {

              path_len =
                int32touint32(dereg_req_ptr->refresh_files.file_path_ptr[j].path_len);
              if(!memcmp(
                  session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_buf,
                  dereg_req_ptr->refresh_files.file_path_ptr[j].path_buf,
                  MIN(path_len, MMGSDI_MAX_PATH_LEN) * sizeof(uint16)))
              {
                file_found = TRUE;
                break;
              }
            }
          }
          if(!file_found && k<total_files)
          {
            /* Copy matching file */
            path_len =
              int32touint32(session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len);
            file_path_ptr[k].path_len =
              session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_len;
            mmgsdi_memscpy(file_path_ptr[k].path_buf,
              sizeof(file_path_ptr[k].path_buf),
              session_ptr->refresh_data.refresh_files.file_path_ptr[i].path_buf,
              path_len * sizeof(uint16));
            k++;
          }
        }

        MMGSDIUTIL_TMC_MEM_FREE(
          session_ptr->refresh_data.refresh_files.file_path_ptr);
        session_ptr->refresh_data.refresh_files.file_path_ptr   = file_path_ptr;
        session_ptr->refresh_data.refresh_files.num_file_paths  = total_files;
      }/*total_files > 0*/

    }/*file_list_ptr != NULL*/
  }/*if(req_ptr->refresh_files.num_files != 0 &&
     req_ptr->refresh_files.file_path_ptr != NULL)*/

  /*------------------------------------------------------------
    Change Voting only if client is specifically deregistering
    ------------------------------------------------------------*/
  if(dereg_req_ptr->vote_for_init)
    session_ptr->refresh_data.vote_for_init = FALSE;

  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_req_process_dereg*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_REQ_PROCESS_DEREG_ALL

  DESCRIPTION:
    This function will process the deregistrations sent by the client for all
    files

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_dereg_all(
  mmgsdi_refresh_req_type           *req_ptr)
{
  mmgsdi_return_enum_type      mmgsdi_status            = MMGSDI_ERROR;
  mmgsdi_session_info_type    *session_ptr              = NULL;
  mmgsdi_channel_info_type    *channel_ptr              = NULL;
  mmgsdi_refresh_reg_req_type *dereg_req_ptr            = NULL;

  UIM_MSG_HIGH_0("MMGSDI_REFRESH_PROCESS_DEREG_ALL");

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                   - Cannot be NULL
    2. verify params are set properly
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  dereg_req_ptr = &req_ptr->refresh.reg_req;

  /*Get the Session Pointer*/
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                   req_ptr->request_header.session_id,
                   &session_ptr,
                   &channel_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS  || session_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /* Allow client deregistration only if client has registered */
  if(!session_ptr->refresh_data.reg_all_files)
  {
    UIM_MSG_ERR_1("Refresh: Not registered for all files, session_id=0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_ERROR;
  }

  /* Update client parameters */
  session_ptr->refresh_data.reg_all_files = FALSE;

  /*------------------------------------------------------------
    Change Voting only if client is specifically deregistering
    ------------------------------------------------------------*/
  if(dereg_req_ptr->vote_for_init)
  {
    session_ptr->refresh_data.vote_for_init = FALSE;
  }

  return mmgsdi_status;
}/*mmgsdi_refresh_req_process_dereg_all*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_COMPLETE_INIT

  DESCRIPTION:
    This state will perform the following operations:
    1.  Wait for all responses to be received
    2.  Wait for PBM and WMS also to complete if it is a prov app
    3.  If all have been received or any of them has resulted in error
        then transition to next state
        • READY_STATE:
          a. Send END_SUCCESS/END_FAILED Event to all relevant clients
          b. Error Case - If any client has voted not to init,
              i. clean up
              ii.Send TR
          c.Success Case
              i. Clean up
              ii.Send TR
        •WAIT_FOR_FCN_COMPLETE_STATE
          a.If FCN >0
             i. Send START event to relevant clients

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_complete_init(
  mmgsdi_slot_data_type             *slot_ptr,
  const mmgsdi_refresh_req_type     *req_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr)
{
  mmgsdi_refresh_node_type *node_ptr           = NULL;
  boolean                   wait_for_rsp       = FALSE;
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_SUCCESS;
  uint8                     slot_index         = MMGSDI_SLOT_1_INDEX;

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  UIM_MSG_HIGH_1("FUNCTION mmgsdi_refresh_process_complete_init on slot: 0x%x",
                 req_ptr->request_header.slot_id);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  UIM_MSG_HIGH_2("REFRESH: COMPLETE INIT response received from session id = 0x%x, with status = 0x%x",
                 (req_ptr->request_header.session_id&0xFFFFFFFF),
                 req_ptr->refresh.pass_fail);

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  if(req_ptr->request_header.client_id == mmgsdi_generic_data_ptr->client_id)
  {
    /* This init complete is from MMGSDI, now start the timer for init complete
     * from other clients. This will ensure that refresh does not fails if for
     * some reason MMGSDI took time to complete its initialisation.
     */
    if(slot_ptr->refresh_state == MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE)
    {
      mmgsdi_refresh_start_timer(slot_ptr, slot_index);
    }
  }
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  extra_param_ptr->refresh_data.stage       = MMGSDI_REFRESH_STAGE_START;
  extra_param_ptr->refresh_data.send_evt    = FALSE;

  /* For card RESET modes, COMPLETE INIT is only expected from MMGSDI
     (after card is RESET and CARD_INSERTED event is sent out). Ignore
     COMPLETE INIT responses from other clients */
  if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET ||
     mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET_AUTO)
  {
    if(req_ptr->request_header.client_id != mmgsdi_generic_data_ptr->client_id)
    {
      mmgsdi_util_print_client_name(req_ptr->request_header.client_id);
    UIM_MSG_HIGH_2("Unexpected COMPLETE INIT response received from client id low = 0x%x high = 0x%x",
                   (req_ptr->request_header.client_id&0xFFFFFFFF),
                   (req_ptr->request_header.client_id >> 32 & 0xFFFFFFFF));
      return MMGSDI_SUCCESS;
    }
  }

  node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
  while(node_ptr)
  {
    if(node_ptr->session_id   == req_ptr->request_header.session_id &&
       node_ptr->rsp_received == FALSE)
    {
      node_ptr->rsp_received = TRUE;
      mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init &=
        req_ptr->refresh.pass_fail;
    }

    if(!mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init)
    {
      /*Break from loop - no need to wait for other responses*/
      break;
    }

    if(node_ptr->rsp_received == FALSE &&
       node_ptr->stage == MMGSDI_REFRESH_STAGE_START)
    {
      /*All responses have not been received, keep waiting*/
      wait_for_rsp = TRUE;
    }
    node_ptr = node_ptr->next_ptr;
  }

  if(!mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init)
  {
    UIM_MSG_HIGH_0("REFRESH Failed, received failure for refresh complete");

    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage                 = MMGSDI_REFRESH_STAGE_END_FAILED;
    extra_param_ptr->refresh_data.send_evt              = TRUE;
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;
    slot_ptr->refresh_state                             = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_CAN_NOT_REFRESH;
  }

  /*Either all the responses have been received or
    Someone could not do the init successfully*/
  if(!wait_for_rsp &&
     !mmgsdi_refresh_wait_for_session_init(extra_param_ptr, slot_index))
  {
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer[slot_index]);

    UIM_MSG_HIGH_0("REFRESH END SUCCESS, sending evt MMGSDI_REFRESH_SUCCESS");

    extra_param_ptr->refresh_data.stage                 = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt              = TRUE;
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_REFRESH_SUCCESS;
    slot_ptr->refresh_state                             = MMGSDI_REFRESH_READY_STATE;

    /*Send the End SUCCESS event first*/
    (void)mmgsdi_refresh_send_evt(&extra_param_ptr->refresh_data,
                                  &req_ptr->request_header);

     extra_param_ptr->refresh_data.send_evt = FALSE;

    /*All responses have been received, continue with refresh*/
    if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
       (mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_3G_SESSION_RESET &&
        (mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_files ||
         mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_file_paths)))
    {
      /*Start the FCN*/
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.mode     = MMGSDI_REFRESH_NAA_FCN;
      slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE;

      node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
      while(node_ptr)
      {
        /*Reset the response received flag only for nodes that have unread files*/
        if(((node_ptr->refresh_files.file_list_ptr != NULL) ||
            (node_ptr->refresh_files.file_path_ptr != NULL)) &&
           ((node_ptr->refresh_files.num_files > 0) ||
            ((node_ptr->refresh_files.num_file_paths > 0))))
        {
          node_ptr->rsp_received = FALSE;
        }
        node_ptr->stage        = MMGSDI_REFRESH_STAGE_START;
        node_ptr               = node_ptr->next_ptr;
      }
    }
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_process_complete_init*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_COMPLETE_STEERING

  DESCRIPTION:
    This state will perform the following operations:
    1. Wait for all responses to be received
    2. If all have been received or any of them has resulted in error then transition to next state

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_complete_steering(
  mmgsdi_slot_data_type             * slot_ptr,
  const mmgsdi_refresh_req_type     * req_ptr,
  mmgsdi_client_req_extra_info_type * extra_param_ptr)
{
  boolean                   wait_for_response  = FALSE;
  int                       index              = 0;
  int                       session_index      = 0;
  mmgsdi_aid_type         * aid_ptr            = NULL;
  uint8                     channel_info_index = MMGSDI_MAX_CHANNEL_INFO;
  uint8                     slot_index         = MMGSDI_SLOT_1_INDEX;
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  UIM_MSG_HIGH_3("REFRESH COMPLETE Steering response received from client id_low = 0x%x, id_high = 0x%x, with status = 0x%x",
                 (req_ptr->request_header.client_id&0xFFFFFFFF),
                 (req_ptr->request_header.client_id>>32&0xFFFFFFFF),
                 req_ptr->refresh.pass_fail);
  mmgsdi_util_print_client_name(req_ptr->request_header.client_id);

  extra_param_ptr->refresh_data.stage     = MMGSDI_REFRESH_STAGE_START;
  extra_param_ptr->refresh_data.send_evt  = FALSE;

  for(index=0; index < MMGSDI_MAX_CLIENT_INFO; index++)
  {
    if(mmgsdi_client_id_reg_table[index] == NULL)
    {
      continue;
    }

    for(session_index = 0; session_index < MMGSDI_MAX_SESSION_INFO; session_index++)
    {
      if(mmgsdi_client_id_reg_table[index]->session_info_ptr[session_index] == NULL)
      {
        continue;
      }
      if(mmgsdi_client_id_reg_table[index]->session_info_ptr[session_index]->steering_of_roaming &&
         (req_ptr->request_header.session_id == mmgsdi_client_id_reg_table[index]->session_info_ptr[session_index]->session_id))
      {
        mmgsdi_client_id_reg_table[index]->session_info_ptr[session_index]->steering_complete_rcvd = TRUE;
        break;
      }
    }
  }

  /* Refresh Failed */
  if(req_ptr->refresh.pass_fail == FALSE)
  {
    /* No Need to wait for any other client - Pack Up*/
    extra_param_ptr->refresh_data.stage     = MMGSDI_REFRESH_STAGE_END_FAILED;
    extra_param_ptr->refresh_data.send_evt  = TRUE;
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;
    slot_ptr->refresh_state                 = MMGSDI_REFRESH_READY_STATE;
    /* No need to set steering_complete_rvcd to FALSE, we will do it
     * before sending out the event for next steering comamnd.
     */
    return MMGSDI_SUCCESS;
  }

  /* All the responses have been received */
  for(index=0; index < MMGSDI_MAX_CLIENT_INFO; index++)
  {
    if(mmgsdi_client_id_reg_table[index] == NULL)
    {
      continue;
    }

    for(session_index = 0; session_index < MMGSDI_MAX_SESSION_INFO; session_index++)
    {
      if(mmgsdi_client_id_reg_table[index]->session_info_ptr[session_index] == NULL)
      {
        continue;
      }
      channel_info_index = mmgsdi_client_id_reg_table[index]->session_info_ptr[session_index]->channel_info_index;
      if((mmgsdi_client_id_reg_table[index]->session_info_ptr[session_index]->steering_of_roaming == TRUE) &&
         (mmgsdi_client_id_reg_table[index]->session_info_ptr[session_index]->steering_complete_rcvd == FALSE) &&
         (channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[channel_info_index] != NULL) &&
         (mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr != NULL))
      {
        aid_ptr =
          &mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->app_data;

        if((mmgsdi_util_cmp_aid(aid_ptr, &mmgsdi_refresh_info_ptr[slot_index]->aid, TRUE) == MMGSDI_SUCCESS) &&
           (mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id ==
              mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id))
        {
          wait_for_response = TRUE;
          break;
        }
      }
    }
  }

  if(wait_for_response == FALSE)
  {
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer[slot_index]);
    UIM_MSG_HIGH_0("All clients have responded, completed refresh");

    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt = TRUE;
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_REFRESH_SUCCESS;
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_process_complete_steering */


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_COMPLETE_FCN

  DESCRIPTION:
    This state will perform the following operations:
    1. Wait for all responses to be received
    2. If all have been received or any of them has resulted in error then transition to next state
       • READY_STATE:
          a.Send END_SUCCESS/END_FAILED to all relevant clients
          b. Error Case - If any client has voted not to init
             i.  clean up
             ii. Send TR
          c.Success Case –
             i. Clean up
             ii.Send TR

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_complete_fcn(
  mmgsdi_slot_data_type             *slot_ptr,
  const mmgsdi_refresh_req_type     *req_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr)
{
  mmgsdi_refresh_node_type *node_ptr      = NULL;
  boolean                   wait_for_rsp  = FALSE;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  uint8                     slot_index    = MMGSDI_SLOT_1_INDEX;

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  UIM_MSG_HIGH_2("REFRESH: COMPLETE FCN response received from session id = 0x%x, with status = 0x%x",
                 (req_ptr->request_header.session_id&0xFFFFFFFF),
                 req_ptr->refresh.pass_fail);

  extra_param_ptr->refresh_data.stage       = MMGSDI_REFRESH_STAGE_START;
  extra_param_ptr->refresh_data.send_evt    = FALSE;

  node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
  while(node_ptr)
  {
    if(node_ptr->session_id   == req_ptr->request_header.session_id &&
       node_ptr->rsp_received == FALSE)
    {
      UIM_MSG_HIGH_2("Session Id 0x%x responded with pass =0x%x",
                     node_ptr->session_id, req_ptr->refresh.pass_fail);
      node_ptr->rsp_received = TRUE;

      if(!req_ptr->refresh.pass_fail)
      {
        /*Break from loop , no need to look any further*/
        break;
      }
    }
    if(node_ptr->rsp_received == FALSE &&
       node_ptr->stage == MMGSDI_REFRESH_STAGE_START)
    {
      /*All responses have not been received, keep waiting*/
      wait_for_rsp = TRUE;
    }
    node_ptr = node_ptr->next_ptr;
  }

  /* All the responses have been received */
  if(!wait_for_rsp)
  {
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer[slot_index]);
    UIM_MSG_HIGH_0("All clients have responded, completed refresh");
    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage                 = MMGSDI_REFRESH_STAGE_END_SUCCESS;
    extra_param_ptr->refresh_data.send_evt              = TRUE;
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_REFRESH_SUCCESS;
    slot_ptr->refresh_state                             = MMGSDI_REFRESH_READY_STATE;
    node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
    while(node_ptr)
    {
      node_ptr->rsp_received = FALSE;
      node_ptr->stage = extra_param_ptr->refresh_data.stage;
      node_ptr = node_ptr->next_ptr;
    }
    /* If mode is INIT_FCN, this REFRESH event should go to only the
       FCN nodes */
    if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
       mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_3G_SESSION_RESET)
    {
      extra_param_ptr->refresh_data.mode = MMGSDI_REFRESH_NAA_FCN;
    }
  }

  if(!req_ptr->refresh.pass_fail)
  {
    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage                 = MMGSDI_REFRESH_STAGE_END_FAILED;
    extra_param_ptr->refresh_data.send_evt              = TRUE;
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;
    node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
    while(node_ptr)
    {
      node_ptr->rsp_received = FALSE;
      node_ptr->stage = extra_param_ptr->refresh_data.stage;
      node_ptr = node_ptr->next_ptr;
    }
    slot_ptr->refresh_state                = MMGSDI_REFRESH_READY_STATE;
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_process_complete_fcn*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_OK_INIT_RSP

  DESCRIPTION:
    This function will process the clients response to ok_to_init.

    This state will perform the following operations:
    1. If all have been received or any of them has resulted in error then transition to next state
       •WAIT_FOR_OK_TO_FCN_STATE:
           a. If it is FCN node and there are clients interested in files in FCN
           b. Send OK_TO_FCN event to relevant clients
        •READY_STATE:
           a.Error Case - If any client has voted not to init, clean up , send TR
        •WAIT_FOR_INIT_COMPLETE_STATE:
           a.If all clients have responded with OK
              i.  Send START Event to all relevant clients
              ii. Send command to GSDI to start init if it is prov app
              iii.Transition to this state
        •   APP_DEACTIVATE_STATE
           a.If the mode is APP_RESET or 3G Session reset and it is not default app

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_ok_init_rsp(
  mmgsdi_slot_data_type              *slot_ptr,
  const mmgsdi_refresh_req_type      *req_ptr,
  mmgsdi_client_req_extra_info_type  *extra_param_ptr)
{
  mmgsdi_refresh_node_type *node_ptr           = NULL;
  boolean                   wait_for_rsp       = FALSE;
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_ERROR;
  boolean                   wait_for_pdown_cfm = FALSE;
  boolean                   wait_for_fcn_cfm   = FALSE;
  uint8                     slot_index         = MMGSDI_SLOT_1_INDEX;

  MMGSDIUTIL_RETURN_IF_NULL_3(req_ptr, slot_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_refresh_info_ptr[slot_index],
                              mmgsdi_refresh_info_ptr[slot_index]->node_ptr);

  UIM_MSG_HIGH_2("REFRESH: OK_TO_INIT response received from session_id = 0x%x, with status = 0x%x",
                 (req_ptr->request_header.session_id&0xFFFFFFFF),
                 req_ptr->refresh.ok_req);

  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT;

  /*Check if all the responses have been received*/
  node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
  while( node_ptr != NULL)
  {
    /*Handle only OK_TO_INIT CASES, some nodes maybe OK_TO_INIT and some
      maybe OK_TO_FCN in case the mode is NAA_INIT_FCN*/
    if(node_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT)
    {
      if(node_ptr->session_id    == req_ptr->request_header.session_id &&
         node_ptr->rsp_received == FALSE)
      {
        if(node_ptr->pdown_cnf)
        {
          node_ptr->rsp_received = TRUE;
          node_ptr->stage        = MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH;
        }
        else
        {
          node_ptr->rsp_received = TRUE;
          node_ptr->stage        = MMGSDI_REFRESH_STAGE_START;
        }

        if((mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
            mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_3G_SESSION_RESET) &&
           !node_ptr->pdown_cnf                                         &&
           ((node_ptr->refresh_files.file_list_ptr != NULL) ||
           (node_ptr->refresh_files.file_path_ptr != NULL)))
        {
          node_ptr->stage = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
        }
        mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init &=
           req_ptr->refresh.ok_req;

        if(!mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init)
        {
          UIM_MSG_HIGH_0("REFRESH: OK_TO_INIT failure response received");

          /*Break, no need to look any further, we have a failure already*/
          break;
        }
      }
      if(node_ptr->rsp_received == FALSE)
      {
        /*All responses have not been received, keep waiting*/
        wait_for_rsp = TRUE;
      }
    }
    node_ptr = node_ptr->next_ptr;
  }

  if(!mmgsdi_refresh_info_ptr[slot_index]->refresh_data.vote_for_init)
  {
    mmgsdi_status = MMGSDI_REFRESH_LATER;

    if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET_AUTO)
    {
      /* SMS may be in progress, start timer for the command to be rebuilt */
      rex_def_timer_ex(&mmgsdi_refresh_auto_trigger_timer[slot_index],
                       mmgsdi_refresh_auto_reset_timer_expiry,
                       (uint32)mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id);

      (void)rex_set_timer(&mmgsdi_refresh_auto_trigger_timer[slot_index], MMGSDI_AUTO_REFRESH_TIMER);
    }

    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    extra_param_ptr->refresh_data.send_evt = TRUE;

    /* Reset auto is an internal REFRESH mode, no need to add it in retry table */
    if(mmgsdi_refresh_info_ptr[slot_index]->mode != MMGSDI_REFRESH_APP_RESET_AUTO)
    {
      if(mmgsdi_refresh_retry_settings_ptr[slot_index] != NULL)
      {
        mmgsdi_refresh_retry_print_table();
        if(MMGSDI_SUCCESS == mmgsdi_refresh_retry_add_entry(slot_index))
        {
          mmgsdi_refresh_retry_print_table();
        }
        else
        {
          mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;
        }
      }
    }
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = mmgsdi_status;
    slot_ptr->refresh_state                 = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_SUCCESS;
  }

  if(!wait_for_rsp)
  {
    /*------------------------------------------------------------------------
     •  WAIT_FOR_OK_TO_FCN_STATE:
       a.    If it is FCN node and there are clients interested in files in FCN
       b.   Send OK_TO_FCN event to relevant clients
    ------------------------------------------------------------------------*/
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer[slot_index]);
    /*Check if all the responses have been received*/
    node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
    /* Loop through the entire node list to chk is any node is gas registered for
       proceed_with_refresh notification */
    while( node_ptr != NULL)
    {
      if(node_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH)
      {
        wait_for_pdown_cfm     = TRUE;
        node_ptr->rsp_received = FALSE;
      }
      else if(node_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
      {
        wait_for_fcn_cfm       = TRUE;
        node_ptr->rsp_received = FALSE;
      }
      node_ptr = node_ptr->next_ptr;
    }

    if(wait_for_pdown_cfm)
    {
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.stage    =
        MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH;
      slot_ptr->refresh_state                =
        MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE;
    }
    else if(wait_for_fcn_cfm)
    {
      /*Wait for ok_to_fcn*/
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.stage    =
        MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
      slot_ptr->refresh_state                =
        MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE;
    }
    else
    {
      if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT_FCN)
      {
        /*Wait for ok_to_fcn*/
        extra_param_ptr->refresh_data.send_evt = TRUE;
        extra_param_ptr->refresh_data.stage    =
          MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
        slot_ptr->refresh_state                =
          MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE;
      }
      else if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET ||
              mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET_AUTO)
      {
        mmgsdi_status = mmgsdi_refresh_start_full_reset(slot_ptr,
                                                        extra_param_ptr,
                                                        mmgsdi_refresh_info_ptr[slot_index]->mode,
                                                        FALSE,
                                                        slot_index);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("Refresh: Could not start reset, status 0x%x",
                        mmgsdi_status);
          return mmgsdi_status;
        }
        slot_ptr->refresh_state =
          MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
      }
      else if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_APP_RESET ||
              mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_3G_SESSION_RESET)
      {
        mmgsdi_status = mmgsdi_refresh_start_reset(slot_ptr,
                                                   extra_param_ptr,
                                                   slot_index);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("Refresh: Could not start reset, status 0x%x",
                        mmgsdi_status);
          return mmgsdi_status;
        }
        slot_ptr->refresh_state =
          MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
      }/*mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_APP_RESET*/
      else if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_APP_RESET_AUTO)
      {
        mmgsdi_status = mmgsdi_refresh_start_app_reset_auto(slot_ptr,
                                                            extra_param_ptr,
                                                            slot_index);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("Refresh: Could not start reset auto , status 0x%x",
                        mmgsdi_status);
          return mmgsdi_status;
        }
        /* This is to ensure that we do not start an INIT_COMPLETE timer */
        slot_ptr->refresh_state =
          MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE;
      }/*mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_APP_RESET_AUTO*/
      else
      {
        /*----------------------------------------------------------------------
         •    WAIT_FOR_INIT_COMPLETE_STATE:
             a.   If all clients have responded with OK
                i.    Send START Event to all relevant clients
                ii.   Transition to this state
         ------------------------------------------------------------------------*/
        extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
        extra_param_ptr->refresh_data.send_evt = TRUE;
        slot_ptr->refresh_state                =
          MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
      }

      if(mmgsdi_refresh_info_ptr[slot_index]->mode != MMGSDI_REFRESH_RESET &&
         mmgsdi_refresh_info_ptr[slot_index]->mode != MMGSDI_REFRESH_RESET_AUTO)
      {
        /*Walk through all nodes and change the rsp received to FALSE*/
        node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
        while(node_ptr)
        {
          node_ptr->rsp_received = FALSE;
          node_ptr               = node_ptr->next_ptr;
        }
      }
    }
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_process_ok_init_rsp*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_OK_FCN_RSP

  DESCRIPTION:
    This function will process the clients response to ok_to_fcn.

    This state will perform the following operations:
    1. If all have been received or any of them has resulted in error then
       transition to next state
       •WAIT_FOR_INIT_COMPLETE_STATE
           a.If the mode is INIT/RESET(default)/INIT_FCN/INIT_FFCN/3G
             Session Reset(default)
           b.Send START event to relevant clients
           c.Send command to GSDI to start init if it is provisioning
             application.
        •WAIT_FOR_FCN_COMPLETE_STATE:
           a.If all clients have responded with OK and mode is FCN
              i. Send START Event to all relevant clients
              ii.Transition to this state
        •READY_STATE:
           a.Error Case - If any client has voted not to init
              i. clean up
              ii.send TR

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_ok_fcn_rsp(
  mmgsdi_slot_data_type             *slot_ptr,
  const mmgsdi_refresh_req_type     *req_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr)
{
  boolean                       ok_for_fcn     = TRUE;
  boolean                       wait_for_rsp   = FALSE;
  mmgsdi_refresh_node_type     *node_ptr       = NULL;
  mmgsdi_return_enum_type       mmgsdi_status  = MMGSDI_SUCCESS;
  uint8                         slot_index     = MMGSDI_SLOT_1_INDEX;

  /* Parameter Check */
  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_refresh_info_ptr[slot_index],
                              mmgsdi_refresh_info_ptr[slot_index]->node_ptr);

  UIM_MSG_HIGH_2("REFRESH: OK_TO_FCN response received from session id = 0x%x with status = 0x%x",
                 (req_ptr->request_header.session_id&0xFFFFFFFF),
                 req_ptr->refresh.ok_req);

  extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;

  /*Check if all the responses have been received*/
  node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
  while( node_ptr != NULL)
  {
    if(node_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
    {
      if(node_ptr->session_id   == req_ptr->request_header.session_id &&
         node_ptr->rsp_received == FALSE)
      {
        node_ptr->rsp_received = TRUE;
      }
      if(node_ptr->rsp_received &&
         req_ptr->refresh.ok_req == FALSE)
      {
        ok_for_fcn = FALSE;
        break;
      }
      if(node_ptr->rsp_received == FALSE)
      {
        /*All responses have not been received, keep waiting*/
        wait_for_rsp = TRUE;
      }
    }
    node_ptr = node_ptr->next_ptr;
  }

  /* Client rejected the fcn */
  if(!ok_for_fcn)
  {
    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage     = MMGSDI_REFRESH_STAGE_END_FAILED;
    extra_param_ptr->refresh_data.send_evt  = TRUE;
    slot_ptr->refresh_state                 = MMGSDI_REFRESH_READY_STATE;
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_REFRESH_LATER;

    /*Walk through all nodes and change the rsp received to FALSE*/
    node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
    while(node_ptr)
    {
      node_ptr->rsp_received = FALSE;
      node_ptr->stage        = MMGSDI_REFRESH_STAGE_END_FAILED;
      node_ptr               = node_ptr->next_ptr;
    }

    if(mmgsdi_refresh_retry_settings_ptr[slot_index] != NULL)
    {
      mmgsdi_refresh_retry_print_table();
      if(MMGSDI_SUCCESS == mmgsdi_refresh_retry_add_entry(slot_index))
      {
        mmgsdi_refresh_retry_print_table();
      }
      else
      {
        mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;
      }
    }
    return MMGSDI_SUCCESS;
  }

  if(!wait_for_rsp)
  {
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer[slot_index]);

    /*Walk through all nodes and change the rsp received to FALSE*/
    node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
    while(node_ptr)
    {
      node_ptr->rsp_received = FALSE;
      node_ptr->stage        = MMGSDI_REFRESH_STAGE_START;
      node_ptr               = node_ptr->next_ptr;
    }
    if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT_FCN)
    {
      /*----------------------------------------------------------------------
       •    WAIT_FOR_INIT_COMPLETE_STATE:
           a.   If all clients have responded with OK
              i.    Send START Event to all relevant clients
              ii.   Transition to this state
      ------------------------------------------------------------------------*/
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.send_evt = TRUE;
      slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
    }
    else if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_3G_SESSION_RESET)
    {
      mmgsdi_status = mmgsdi_refresh_start_reset(slot_ptr,
                                                 extra_param_ptr,
                                                 slot_index);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Refresh: Could not start reset, status 0x%x",
                      mmgsdi_status);
        return mmgsdi_status;
      }
      slot_ptr->refresh_state =
        MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
    }
    else
    {
      /*----------------------------------------------------------------------
       •    MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE:
           a.   If all clients have responded with OK
              i.    Send START Event to all relevant clients
              ii.   Transition to this state
       ------------------------------------------------------------------------*/
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.send_evt = TRUE;
      slot_ptr->refresh_state                = MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE;

    } /* else (mode != MMGSDI_REFRESH_NAA_INIT_FCN) */
  } /* if(!wait_for_rsp) */
  return MMGSDI_SUCCESS;
}/*mmgsdi_refresh_process_ok_fcn_rsp*/


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_TIMER_EXPIRY

  DESCRIPTION:
    This function processes the REFRESH timer expiry.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_timer_expiry(
  mmgsdi_slot_data_type             *slot_ptr,
  const mmgsdi_refresh_req_type     *req_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  uint8                    slot_index    = MMGSDI_SLOT_1_INDEX;

  UIM_MSG_HIGH_0("FUNCTION mmgsdi_refresh_process_timer_expiry");

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_refresh_info_ptr[slot_index]);

  /* If the timer expiry is corresponding to the ongoing REFRESH */
  if((req_ptr->request_header.slot_id ==
      mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id) &&
     (req_ptr->refresh.timer_count ==
      slot_ptr->refresh_timer_count))
  {
    UIM_MSG_HIGH_1("Timer expired in state 0x%x", slot_ptr->refresh_state);

    /* Halt ongoing REFRESH */
    extra_param_ptr->refresh_data.stage     = MMGSDI_REFRESH_STAGE_END_FAILED;
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;
    slot_ptr->refresh_state                 = MMGSDI_REFRESH_READY_STATE;

    /* For NAA_FCN refresh, update the stage to END_FAILED and rsp_recieved as
       FALSE for all the FCN nodes (i.e, all the nodes in mmgsdi_refresh_info_ptr)
       because in NAA_FCN mode refresh failed event should be sent to all nodes
       irrespective of the response recieved  */
    if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_FCN)
    {
      mmgsdi_refresh_node_type *node_ptr = NULL;

      node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
      while(node_ptr)
      {
        node_ptr->stage = MMGSDI_REFRESH_STAGE_END_FAILED;
        node_ptr->rsp_received = FALSE;
        node_ptr = node_ptr->next_ptr;
      }
      extra_param_ptr->refresh_data.send_evt = TRUE;
    }
    /* For all other modes, send END_FAILED event */
    else
    {
      extra_param_ptr->refresh_data.send_evt = TRUE;
    }
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_refresh_process_timer_expiry */


/*=============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_OK_TO_PROCEED_RSP

  DESCRIPTION:
    This function will process the clients response to OK_TO_PROCEED CNF.

    This state will perform the following operations:
    1. If all the responses have been received or any of them has resulted in error then
       transition to next state
        •START_STATE:
           a.If all clients have responded with OK
              i. Send START Event to all relevant clients
              ii.Transition to this state
        •READY_STATE:
           a.Error Case - If any client has voted not to proceed with Card Power Down
              i. clean up
              ii.send TR

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_process_ok_to_proceed_rsp(
  mmgsdi_slot_data_type              *slot_ptr,
  const mmgsdi_refresh_req_type      *req_ptr,
  mmgsdi_client_req_extra_info_type  *extra_param_ptr)
{
  mmgsdi_refresh_node_type      *node_ptr      = NULL;
  boolean                        wait_for_rsp  = FALSE;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  uint8                          slot_index    = MMGSDI_SLOT_1_INDEX;

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_refresh_info_ptr[slot_index],
                              mmgsdi_refresh_info_ptr[slot_index]->node_ptr);

  UIM_MSG_HIGH_2("REFRESH: OK_TO_PROCEED response received from session_id = 0x%x, with status = 0x%x",
                 (req_ptr->request_header.session_id&0xFFFFFFFF),
                 req_ptr->refresh.notify_pdown);

  extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH;

  /*Check if all the responses have been received*/
  node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
  while( node_ptr != NULL)
  {
    if(node_ptr->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_PROCEED_WITH_REFRESH)
    {
      if(node_ptr->session_id   == req_ptr->request_header.session_id &&
         node_ptr->rsp_received == FALSE)
      {
        node_ptr->rsp_received = TRUE;
        node_ptr->stage        = MMGSDI_REFRESH_STAGE_START;

        if((mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
            mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_3G_SESSION_RESET) &&
           ((node_ptr->refresh_files.file_list_ptr != NULL) ||
           (node_ptr->refresh_files.file_path_ptr != NULL)))
        {
          node_ptr->stage = MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
        }

        mmgsdi_refresh_info_ptr[slot_index]->refresh_data.proceed_refresh &=
           req_ptr->refresh.notify_pdown;

        if(!mmgsdi_refresh_info_ptr[slot_index]->refresh_data.proceed_refresh)
        {
          UIM_MSG_HIGH_0("REFRESH: Proceed PDown failure response received");

          /*Break, no need to look any further, we have a failure already*/
          break;
        }
      }

      if(node_ptr->rsp_received == FALSE)
      {
        /*All responses have not been received, keep waiting*/
        wait_for_rsp = TRUE;
      }
    }
    node_ptr = node_ptr->next_ptr;
  }

  if(!mmgsdi_refresh_info_ptr[slot_index]->refresh_data.proceed_refresh)
  {
    mmgsdi_status = MMGSDI_REFRESH_LATER;
    /*No Need to wait for any other client*/
    extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_END_FAILED;
    /* Reset auto is an internal REFRESH mode, no need to send out this event */
    extra_param_ptr->refresh_data.send_evt = TRUE;
    if(mmgsdi_refresh_retry_settings_ptr[slot_index] != NULL)
    {
      mmgsdi_refresh_retry_print_table();
      if(MMGSDI_SUCCESS == mmgsdi_refresh_retry_add_entry(slot_index))
      {
        mmgsdi_refresh_retry_print_table();
      }
      else
      {
        mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;
      }
    }

    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = mmgsdi_status;
    slot_ptr->refresh_state                 = MMGSDI_REFRESH_READY_STATE;
    return MMGSDI_SUCCESS;
  }

  if(!wait_for_rsp)
  {
    /*------------------------------------------------------------------------
     •  WAIT_FOR_OK_TO_FCN_STATE:
       a.    If it is FCN node and there are clients interested in files in FCN
       b.   Send OK_TO_FCN event to relevant clients
    ------------------------------------------------------------------------*/
    /* Clear mmgsdi refresh timer as the refresh state is going to change */
    (void)rex_clr_timer(&mmgsdi_refresh_timer[slot_index]);

    if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_INIT_FCN)
    {
      /*Wait for ok_to_fcn*/
      extra_param_ptr->refresh_data.send_evt = TRUE;
      extra_param_ptr->refresh_data.stage    =
        MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
      slot_ptr->refresh_state                =
        MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE;
    }
    else if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET ||
            mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET_AUTO)
    {
      mmgsdi_status = mmgsdi_refresh_start_full_reset(slot_ptr,
                                                      extra_param_ptr,
                                                      mmgsdi_refresh_info_ptr[slot_index]->mode,
                                                      FALSE,
                                                      slot_index);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Refresh: Could not start reset, status 0x%x",
                      mmgsdi_status);
        return mmgsdi_status;
      }
      slot_ptr->refresh_state =
        MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
    }
    else if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_3G_SESSION_RESET)
    {
      if(mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_files ||
         mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_file_paths)
      {
        /*Wait for ok_to_fcn*/
        extra_param_ptr->refresh_data.send_evt = TRUE;
        extra_param_ptr->refresh_data.stage    =
          MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN;
        slot_ptr->refresh_state                =
          MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE;
      }
      else
      {
        mmgsdi_status = mmgsdi_refresh_start_reset(slot_ptr,
                                                 extra_param_ptr,
                                                 slot_index);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_1("Refresh: Could not start reset, status 0x%x",
                        mmgsdi_status);
          return mmgsdi_status;
        }
        slot_ptr->refresh_state =
          MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
      }
    }
    else if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_NAA_APP_RESET)
    {
      mmgsdi_status = mmgsdi_refresh_start_reset(slot_ptr,
                                                 extra_param_ptr,
                                                 slot_index);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Refresh: Could not start reset, status 0x%x",
                      mmgsdi_status);
        return mmgsdi_status;
      }
      slot_ptr->refresh_state =
        MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
    }/*mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_NAA_APP_RESET*/
    else if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_APP_RESET_AUTO)
    {
      mmgsdi_status = mmgsdi_refresh_start_app_reset_auto(slot_ptr,
                                                          extra_param_ptr,
                                                          slot_index);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Refresh: Could not start reset auto , status 0x%x",
                      mmgsdi_status);
        return mmgsdi_status;
      }
      /* This is to ensure that we do not start an INIT_COMPLETE timer */
      slot_ptr->refresh_state =
        MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE;
    }/*mmgsdi_refresh_info_ptr->mode == MMGSDI_REFRESH_APP_RESET_AUTO*/
    else
    {
      /*----------------------------------------------------------------------
       •    WAIT_FOR_INIT_COMPLETE_STATE:
           a.   If all clients have responded with OK
              i.    Send START Event to all relevant clients
              ii.   Transition to this state
       ------------------------------------------------------------------------*/
      extra_param_ptr->refresh_data.stage    = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.send_evt = TRUE;
      slot_ptr->refresh_state                =
        MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE;
    }

    if(mmgsdi_refresh_info_ptr[slot_index]->mode != MMGSDI_REFRESH_RESET &&
       mmgsdi_refresh_info_ptr[slot_index]->mode != MMGSDI_REFRESH_RESET_AUTO)
    {
      /*Walk through all nodes and change the rsp received to FALSE*/
      node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
      while(node_ptr)
      {
        node_ptr->rsp_received = FALSE;
        node_ptr               = node_ptr->next_ptr;
      }
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_process_ok_to_proceed_rsp */


/*=============================================================================
  FUNCTION: MMGSDI_REFRESH_REQ_PROCESS_START

  DESCRIPTION:
    A Refresh Command has been received

    This state will perform the following operations:
    1.  Figure out if it is prov app
    2.  Figure what mode it is
    3.  If the mode is FCN and it is prov app and it is a call stack file, the
        change the mode to INIT
    4.  Build the Linked list for Clients that will be involved in this refresh mode
    5.  Transition into one of the following states –
       •    WAIT_FOR_OK_TO_INIT_STATE:
           a.   INIT/RESET/3G SESSION RESET/INIT_FCN will change the state to this state
           b.   Send OK_TO_INIT events to relevant clients
       •    WAIT_FOR_OK_TO_FCN_STATE:
           a.   If it is FCN node and there are clients interested in files in FCN
           b.   Send OK_TO_FCN event to relevant clients
       •    READY_STATE:
           i.   Error Case - If it is unknown mode/ app is not active, clean up Send TR
           ii.  Success Case  - If Refresh is complete and no one is interested in this
                mode. , clean up, send TR

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_req_process_start(
  mmgsdi_slot_data_type             *slot_ptr,
  const mmgsdi_refresh_req_type     *req_ptr,
  mmgsdi_client_req_extra_info_type *extra_param_ptr
 )
{
  mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_SUCCESS;
  uint32                     aid_index      = 0;
  uint8                      slot_index     = MMGSDI_SLOT_1_INDEX;

  MMGSDIUTIL_RETURN_IF_NULL_3(slot_ptr, req_ptr, extra_param_ptr);

  UIM_MSG_HIGH_2("FUNCTION mmgsdi_refresh_req_process_start, slot: 0x%x, REFRESH req_type: 0x%x",
                 req_ptr->request_header.slot_id, req_ptr->refresh_req_type);

  /* check if the aid is in the EF-DIR */
  if(req_ptr->refresh.card_req.aid.aid.data_len != 0 )
  {
    aid_index = mmgsdi_util_find_aid_index(req_ptr->request_header.slot_id,
                                           req_ptr->refresh.card_req.aid);

    if (!mmgsdi_util_is_valid_aid_index(aid_index))
    {
      UIM_MSG_ERR_0("Refresh: Refresh for app not in the EF-DIR, Refresh failed");
      extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
      slot_ptr->refresh_state             = MMGSDI_REFRESH_READY_STATE;
      return MMGSDI_REFRESH_FAIL_INVALID_AID;
    }
  }

  if(mmgsdi_copy_to_refresh_ptr(&req_ptr->request_header,
                                &req_ptr->refresh.card_req,
                                aid_index) != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Refresh:  Could not copy to refresh ptr");
    return MMGSDI_CAN_NOT_REFRESH;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_CAN_NOT_REFRESH;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return MMGSDI_CAN_NOT_REFRESH;
  }

  /* Consolidate the refresh retry table when we start a new refresh */
  if(mmgsdi_refresh_retry_settings_ptr[slot_index] != NULL)
  {
    mmgsdi_refresh_retry_print_table();
    mmgsdi_status = mmgsdi_refresh_retry_consolidate_table(slot_index);
    mmgsdi_refresh_retry_print_table();

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Refresh Retry: Could not consolodate Refresh Retry Table");
    }
  }

  /*----------------------------------------------------
     1. Figure out what mode it is
  ------------------------------------------------------*/
  switch(req_ptr->refresh.card_req.refresh_mode)
  {
    case MMGSDI_REFRESH_RESET:
    case MMGSDI_REFRESH_RESET_AUTO:
      mmgsdi_status = mmgsdi_refresh_process_full_reset(req_ptr->request_header.orig_request_type,
                                                        slot_ptr,
                                                        &req_ptr->refresh.card_req,
                                                        extra_param_ptr,
                                                        req_ptr->request_header.slot_id);
      break;
    case MMGSDI_REFRESH_NAA_APP_RESET:
    case MMGSDI_REFRESH_3G_SESSION_RESET:
      mmgsdi_status = mmgsdi_refresh_process_reset(req_ptr->request_header.slot_id,
                                                   slot_ptr,
                                                   &req_ptr->refresh.card_req,
                                                   extra_param_ptr,
                                                   req_ptr->refresh.card_req.refresh_mode);
      break;
    case MMGSDI_REFRESH_APP_RESET_AUTO:
      mmgsdi_status = mmgsdi_refresh_process_app_reset_auto(req_ptr->request_header.slot_id,
                                                            slot_ptr,
                                                            &req_ptr->refresh.card_req,
                                                            extra_param_ptr);
      break;
    case MMGSDI_REFRESH_NAA_INIT:
    case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
      mmgsdi_status = mmgsdi_refresh_process_init(req_ptr->request_header.slot_id,
                                                  slot_ptr,
                                                  &req_ptr->refresh.card_req,
                                                  extra_param_ptr,
                                                  req_ptr->refresh.card_req.refresh_mode);
      break;
    case MMGSDI_REFRESH_NAA_INIT_FCN:
      mmgsdi_status = mmgsdi_refresh_process_init_fcn(req_ptr->request_header.slot_id,
                                                      slot_ptr,
                                                      &req_ptr->refresh.card_req,
                                                      extra_param_ptr);
      break;
    case MMGSDI_REFRESH_NAA_FCN:
      mmgsdi_status = mmgsdi_refresh_process_fcn(req_ptr->request_header.slot_id,
                                                 slot_ptr,
                                                 &req_ptr->refresh.card_req,
                                                 extra_param_ptr);
      break;
    case MMGSDI_REFRESH_STEERING_OF_ROAMING:
      mmgsdi_status = mmgsdi_refresh_process_steering(req_ptr->request_header.slot_id,
                                                      slot_ptr,
                                                      &req_ptr->refresh.card_req,
                                                      extra_param_ptr);
      break;
    default:
      UIM_MSG_ERR_1("Refresh: Invalid Mode 0x%x",
                    req_ptr->refresh.card_req.refresh_mode);
      return MMGSDI_ERROR;
  }

  if(mmgsdi_refresh_info_ptr[slot_index] != NULL)
  {
    mmgsdi_refresh_info_ptr[slot_index]->refresh_status = mmgsdi_status;
    extra_param_ptr->refresh_data.mode                  = mmgsdi_refresh_info_ptr[slot_index]->mode;

    if((mmgsdi_status == MMGSDI_REFRESH_IN_PROGRESS) &&
       (mmgsdi_refresh_info_ptr[slot_index]->mode != MMGSDI_REFRESH_APP_RESET_AUTO))
    {
      /*Reset all Notify inits since we are about to start refresh now*/
      mmgsdi_refresh_reset_notify_init(slot_index);
    }
  }
  if(mmgsdi_status != MMGSDI_REFRESH_IN_PROGRESS &&
     mmgsdi_status != MMGSDI_REFRESH_SUCCESS)
  {
    extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
  }
  return mmgsdi_status;
} /* mmgsdi_refresh_req_process_start */


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_PROCESS_INIT_PIN_CHECK

  DESCRIPTION:
    This function checks PIN state to determine if INIT based REFRESH can be
    performed or should be halted

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
static void mmgsdi_refresh_process_init_pin_check (
  mmgsdi_client_req_extra_info_type  * extra_param_ptr,
  uint8                                slot_index,
  mmgsdi_return_enum_type            * mmgsdi_status_ptr
)
{
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return;
  }

  if ((extra_param_ptr == NULL) || (mmgsdi_status_ptr == NULL) ||
      (mmgsdi_refresh_info_ptr[slot_index] == NULL))
  {
    return;
  }

  if((extra_param_ptr->refresh_data.mode == MMGSDI_REFRESH_NAA_INIT ||
     extra_param_ptr->refresh_data.mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
     extra_param_ptr->refresh_data.mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN) &&
     (mmgsdi_refresh_info_ptr[slot_index]->node_ptr != NULL &&
      mmgsdi_util_is_prov_session(mmgsdi_refresh_info_ptr[slot_index]->node_ptr->session_type)))
  {
    mmgsdi_int_app_info_type     *int_app_info_ptr = NULL;
    mmgsdi_return_enum_type       get_prov_status  = MMGSDI_SUCCESS;

    get_prov_status = mmgsdi_util_get_prov_session_info(mmgsdi_refresh_info_ptr[slot_index]->node_ptr->session_id,
                                                        NULL,
                                                        NULL,
                                                        &int_app_info_ptr);

    if ((get_prov_status != MMGSDI_SUCCESS) ||
        (int_app_info_ptr == NULL) ||
        (int_app_info_ptr != NULL &&
         int_app_info_ptr->app_state <= MMGSDI_APP_STATE_PIN_EVT_SENT) ||
        (int_app_info_ptr != NULL &&
        !mmgsdi_util_can_proceed_to_post_pin1(int_app_info_ptr)))
    {
      UIM_MSG_ERR_0("Unable to proceed with post PIN1 so rejecting REFRESH");

      /* If REFRESH has not started yet avoid any REFRESH events */
      if(extra_param_ptr->refresh_data.send_evt &&
         extra_param_ptr->refresh_data.refresh_req_type == MMGSDI_REFRESH_ORIG_REFRESH_REQ)
      {
        extra_param_ptr->refresh_data.send_evt = FALSE;
      }
      else
      {
        /* REFRESH is in progress and PIN status is in inappropriate state.
           Abort REFRESH and notify all clients about the End failure */
        extra_param_ptr->refresh_data.send_evt = TRUE;
      }

      extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
      mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;
      *mmgsdi_status_ptr = MMGSDI_CAN_NOT_REFRESH;
    }
  }
} /* mmgsdi_refresh_process_init_pin_check */


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_TERMINATE_APP_AND_CLOSE_CHANNEL

  DESCRIPTION:
    This function terminates the app and closes the corresponding channel (if
    not channel zero.) It is called when a card refresh is received on an app
    which was previously deactivated but the app termination was skipped and
    the channel was kept open.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
=============================================================================*/
static void mmgsdi_refresh_terminate_app_and_close_channel(
  mmgsdi_slot_id_enum_type           slot_id,
  const mmgsdi_aid_type            * app_data_ptr,
  mmgsdi_channel_id_type             channel_id)
{
  mmgsdi_return_enum_type         mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_return_enum_type         report_status     = MMGSDI_ERROR;
  mmgsdi_session_id_type          session_id        = MMGSDI_SESSION_ID_ZERO;
  mmgsdi_cnf_type               * cnf_ptr           = NULL;
  mmgsdi_get_file_attr_req_type   get_file_attr_msg = {{0}};
  mmgsdi_request_header_type      req_header        = {0};

  if(app_data_ptr == NULL)
  {
    return;
  }

  /* Send a STATUS command synchronously to indicate to the card that
     the app is going to be terminated in the modem. */
  mmgsdi_status = mmgsdi_session_send_status_before_deactivate(
                    session_id,
                    slot_id,
                    app_data_ptr);

  req_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  req_header.session_id        = session_id;
  req_header.slot_id           = slot_id;
  req_header.orig_request_type = MMGSDI_REFRESH_REQ;

  get_file_attr_msg.access.access_method = MMGSDI_BY_APP_ID_ACCESS;
  get_file_attr_msg.access.file.app_id.data_len = app_data_ptr->aid.data_len;

  if((app_data_ptr->aid.data_len <= MMGSDI_MAX_AID_LEN) &&
     (app_data_ptr->aid.data_len > 0))
  {
    mmgsdi_memscpy((void *)get_file_attr_msg.access.file.app_id.data_ptr,
                   sizeof(get_file_attr_msg.access.file.app_id.data_ptr),
                   (void *)app_data_ptr->aid.data_ptr,
                   int32touint32(app_data_ptr->aid.data_len));
  }

  UIM_MSG_HIGH_1("Sending down UICC SELECT AID command to MMGSDI UIM: aid len 0x%x",
                 app_data_ptr->aid.data_len);

  /* This is a deactivate request */
  get_file_attr_msg.activate_aid = FALSE;

  mmgsdi_memscpy((void *)&get_file_attr_msg.request_header,
                 sizeof(mmgsdi_request_header_type),
                 (void *)&req_header,
                 sizeof(mmgsdi_request_header_type));

  mmgsdi_status =  mmgsdi_uim_uicc_select (&get_file_attr_msg,
                                           TRUE,
                                           (mmgsdi_get_file_attr_cnf_type **)&cnf_ptr,
                                           TRUE);
  if(cnf_ptr != NULL)
  {
    report_status = cnf_ptr->get_file_attr_cnf.response_header.mmgsdi_status;
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type *) cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  }

  if(mmgsdi_status != MMGSDI_SUCCESS || report_status != MMGSDI_SUCCESS)
  {
    return;
  }

  if(channel_id != MMGSDI_DEFAULT_CHANNEL)
  {
    mmgsdi_status = mmgsdi_uim_uicc_close_channel(&req_header,
                                                  NULL,
                                                  TRUE,
                                                  (mmgsdi_session_deactivate_cnf_type **)&cnf_ptr,
                                                  app_data_ptr,
                                                  MMGSDI_MAX_CHANNELS);
    if(cnf_ptr != NULL)
    {
      report_status = cnf_ptr->session_deactivate_cnf.response_header.mmgsdi_status;
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type *) cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
    }

    if(mmgsdi_status != MMGSDI_SUCCESS || report_status != MMGSDI_SUCCESS)
    {
      return;
    }
  }

  /* Free channel info data of the channel where the app was terminated
     and channel was closed. */
  mmgsdi_util_reset_unused_channel_app_data(slot_id, channel_id);

  UIM_MSG_HIGH_1("mmgsdi_refresh_terminate_app_and_close_channel status: 0x%x",
                 mmgsdi_status);
} /* mmgsdi_refresh_terminate_app_and_close_channel */


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_MAIN

  DESCRIPTION:
    This function handles all refresh requests  - it is also the entry into the
    refresh state machine

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_main(
  mmgsdi_refresh_req_type *req_ptr)
{
  mmgsdi_return_enum_type              mmgsdi_status                 = MMGSDI_ERROR;
  mmgsdi_client_req_extra_info_type   *extra_param_ptr               = NULL;
  int32                                index                         = 0;
  mmgsdi_refresh_state_enum_type       orig_state                    = MMGSDI_REFRESH_READY_STATE;
  mmgsdi_slot_data_type               *slot_data_ptr                 = NULL;
  uint8                                slot_index                    = MMGSDI_SLOT_1_INDEX;
  mmgsdi_channel_id_type               channel_id                    = UIM_MAX_CHANNELS;
  uim_protocol_type                    uim_protocol                  = UIM_NO_SUCH_MODE;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(
    req_ptr->request_header.slot_id);

  if(slot_data_ptr == NULL &&
     (req_ptr->refresh_req_type != MMGSDI_REFRESH_ORIG_REG_REQ &&
      req_ptr->refresh_req_type != MMGSDI_REFRESH_ORIG_DEREG_REQ &&
      req_ptr->refresh_req_type != MMGSDI_REFRESH_ORIG_REG_ALL_REQ &&
      req_ptr->refresh_req_type != MMGSDI_REFRESH_ORIG_DEREG_ALL_REQ))
  {
    UIM_MSG_ERR_2("Slot 0x%x is incorrect for Refresh Req Type 0x%x",
                  req_ptr->request_header.slot_id, req_ptr->refresh_req_type);
    return MMGSDI_ERROR;
  }

  if(slot_data_ptr != NULL)
  {
    mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id, &slot_index);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }
    if((slot_index != MMGSDI_SLOT_1_INDEX) &&
       (slot_index != MMGSDI_SLOT_2_INDEX) &&
       (slot_index != MMGSDI_SLOT_3_INDEX))
    {
      return MMGSDI_ERROR;
    }
  }

  /* When the MMGSDI state is not initialized and we receive REFERH event (this
     generally happens when we try to switch the profile at the boot up) with
     mode as RESET then we skip the voting from the clients as we did not send
     the CARD_INSERTED event. We build the card reset command, initialize the
     globals and clean up the sessions. */
  if(req_ptr->refresh.card_req.refresh_mode == MMGSDI_REFRESH_RESET)
  {
#ifdef FEATURE_UIM_MULTI_PROFILE
    if(mmgsdi_profile_info_ptr != NULL &&
       mmgsdi_profile_info_ptr->is_emergency_refresh &&
       req_ptr->refresh.card_req.source == MMGSDI_REFRESH_SOURCE_SIM)
    {
      req_ptr->refresh.card_req.source = MMGSDI_REFRESH_SOURCE_ECALL;
    }
#endif /* FEATURE_UIM_MULTI_PROFILE */

    if(slot_data_ptr != NULL &&
     slot_data_ptr->mmgsdi_state == MMGSDI_STATE_NOT_INIT)
    {
      uim_protocol = mmgsdi_util_get_uim_protocol(req_ptr->request_header.slot_id);
      if(uim_protocol != UIM_NO_SUCH_MODE)
      {
        mmgsdi_status = mmgsdi_gen_build_card_reset(
                          mmgsdi_generic_data_ptr->client_id,
                          req_ptr->request_header.slot_id,
                          MMGSDI_CARD_RESET_REQ,
                          uim_protocol,
                          MMGSDI_RESET_CAUSE_GENERAL,
                          mmgsdi_util_internal_cb,
                          0);
  
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          mmgsdi_util_preinit_mmgsdi(0,
                                     MMGSDI_APP_NONE,
                                     req_ptr->refresh.card_req.refresh_mode,
                                     req_ptr->request_header.slot_id,
                                     FALSE);
        }
        else
        {
          UIM_MSG_ERR_0("Could not build a Card Reset command");
          mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;
        }
      }
      else
      {
        UIM_MSG_ERR_0("Invalid protocol while building Card Reset command");
        mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;
      }
      return mmgsdi_status;
    }
  }

  /* Allocate and populate extra parameter */
  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                   MMGSDI_REFRESH_REQ,
                   (void*)req_ptr,
                   &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
  {
    UIM_MSG_ERR_0("Refresh: Could not allocate memory");
    return mmgsdi_status;
  }

  /*Fill the client Table*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    NULL,
                    extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  extra_param_ptr->refresh_data.refresh_req_type =
    req_ptr->refresh_req_type;

  if(mmgsdi_refresh_info_ptr[slot_index] != NULL)
  {
    /* If a REFRESH is in progress and a new orig REFRESH request arrives, requeue
       it onto MMGSDI refresh command queue */
    if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_REFRESH_REQ)
    {
      (void)mmgsdi_client_build_refresh_req(req_ptr->request_header.client_id,
                                            req_ptr->request_header.slot_id,
                                            req_ptr,
                                            FALSE,
                                            req_ptr->request_header.response_cb,
                                            req_ptr->request_header.client_data,
                                            TRUE);

      /* This will ensure that in the REFRESH response handling, we
         will not send out any confirmations */
      extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_START;
      extra_param_ptr->refresh_data.mode = req_ptr->refresh.card_req.refresh_mode;

      mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                       index, MMGSDI_REFRESH_REQ,
                       mmgsdi_status);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        mmgsdi_util_free_client_request_table_index(index);
      }
      return mmgsdi_status;
    }
    extra_param_ptr->refresh_data.mode = mmgsdi_refresh_info_ptr[slot_index]->mode;
  }

  if(slot_data_ptr != NULL)
  {
    orig_state = slot_data_ptr->refresh_state;
  }

  UIM_MSG_HIGH_4("Refresh: Refresh State is 0x%x, Req Type is 0x%x, retry_req is 0x%x for slot 0x%x",
                 orig_state,
                 req_ptr->refresh_req_type,
                 req_ptr->refresh.retry_req,
                 req_ptr->request_header.slot_id);

  /*Reset the mmgsdi_status to ERROR*/
  mmgsdi_status = MMGSDI_ERROR;

  switch (req_ptr->refresh_req_type)
  {
    case MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ:
      if(req_ptr->refresh.retry_req)
      {
        mmgsdi_refresh_retry_handle_req();
        break;
      }
      /* If it is not a retry_req fall through */
    case MMGSDI_REFRESH_ORIG_REFRESH_REQ:
      switch(slot_data_ptr->refresh_state)
      {
       /*--------------------------------------------------
         Ready to receive Refresh Commands from the card
       ---------------------------------------------------*/
        case MMGSDI_REFRESH_READY_STATE:
          if(slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
          {
            UIM_MSG_ERR_1("Refresh req type 0x%x not allowed since ONCHIP is activated",
                          req_ptr->refresh_req_type);
            extra_param_ptr->refresh_data.mode = req_ptr->refresh.card_req.refresh_mode;
            extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
            extra_param_ptr->refresh_data.send_evt = FALSE;

            mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;
            break;
          }

          /* Check if the refresh is for an app that was earlier deactivated
             in MMGSDI without closing the channel to the card. No need to
             check this for modes RESET (not app-specific) and APP_RESET_AUTO
             (not card-generated.) */
          if((req_ptr->refresh.card_req.refresh_mode != MMGSDI_REFRESH_RESET) &&
             (req_ptr->refresh.card_req.refresh_mode != MMGSDI_REFRESH_APP_RESET_AUTO) &&
             (mmgsdi_util_check_app_termination_skipped(req_ptr->request_header.slot_id,
                                                        &mmgsdi_refresh_data_info.app_data,
                                                        &channel_id)))
          {
            if(req_ptr->refresh.card_req.refresh_mode == MMGSDI_REFRESH_NAA_FCN)
            {
              /* Return SUCCESS for FCN refresh as the device does not have
                 any file cached for the deactivated app and hence does not
                 need to refresh the files.*/
              extra_param_ptr->refresh_data.mode = req_ptr->refresh.card_req.refresh_mode;
              extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_SUCCESS;
              extra_param_ptr->refresh_data.send_evt = FALSE;
              mmgsdi_status = MMGSDI_REFRESH_SUCCESS;
            }
            else
            {
              /* Return failure for other refresh modes as MMGSDI cannot
                 perform refresh procedures correctly on a deactivated app.
                 Also, queue a command to terminate the app in the card. */
              extra_param_ptr->refresh_data.mode = req_ptr->refresh.card_req.refresh_mode;
              extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
              extra_param_ptr->refresh_data.send_evt = FALSE;
              mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;

              mmgsdi_refresh_terminate_app_and_close_channel(req_ptr->request_header.slot_id,
                                                             &mmgsdi_refresh_data_info.app_data,
                                                             channel_id);
            }
            UIM_MSG_HIGH_3("App is not active on slot 0x%x; returning 0x%x for refresh mode 0x%x",
                           req_ptr->request_header.slot_id,
                           mmgsdi_status,
                           req_ptr->refresh.card_req.refresh_mode);
            break;
          }

          mmgsdi_status = mmgsdi_refresh_req_process_start(
                            slot_data_ptr,
                            req_ptr,
                            extra_param_ptr);
          break;
        default:
          /*Refresh Request came in a state when there was a refresh possibly in
            progress, send out an error. Error will be sent to the originating
            client in mmgsdi_refresg_rsp.
            No events need to be sent*/
          UIM_MSG_ERR_0("New Refresh Request in Bad State");

          extra_param_ptr->refresh_data.mode =
            req_ptr->refresh.card_req.refresh_mode;
          extra_param_ptr->refresh_data.stage =
            MMGSDI_REFRESH_STAGE_END_FAILED;

          mmgsdi_status = MMGSDI_CAN_NOT_REFRESH;
          break;
      }
      break;

     case MMGSDI_REFRESH_ORIG_PUP_REQ:
       if(mmgsdi_refresh_info_ptr[slot_index] != NULL)
       {
         /*A refresh may have been in progress and a new refresh command is sent
           First Clean up*/
         if(mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id ==
            req_ptr->request_header.slot_id)
         {
           extra_param_ptr->refresh_data.stage = MMGSDI_REFRESH_STAGE_END_FAILED;
           extra_param_ptr->refresh_data.send_evt = TRUE;

           (void)mmgsdi_refresh_send_evt(&extra_param_ptr->refresh_data,
                                         &req_ptr->request_header);

           UIM_MSG_ERR_0("CARD PUP received, hence stopping the current REFRESH");

           /* For REFRESH RESET no TR should be sent out to the card */
           if(mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET)
           {
             mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_REFRESH_SUCCESS;
           }
           else
           {
             mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;
           }

           (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
                   &mmgsdi_refresh_info_ptr[slot_index]->request_header);
         }
         /* If the card power-up request recieived is not on the same slot
            as the REFRESH in progress, start the card power-up */
       }

       /*Now restart the command*/
       mmgsdi_status = mmgsdi_refresh_req_process_start(
                        slot_data_ptr,
                        req_ptr,
                        extra_param_ptr);
       break;

     case MMGSDI_REFRESH_ORIG_REG_REQ:
       mmgsdi_status = mmgsdi_refresh_req_process_reg(
                       req_ptr);
       break;

     case MMGSDI_REFRESH_ORIG_DEREG_REQ:
       mmgsdi_status = mmgsdi_refresh_req_process_dereg(
                       req_ptr);
       break;

     case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
       switch(slot_data_ptr->refresh_state)
       {
         case MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE:
           /*--------------------------------------------------------
            Wait for OK_TO_INIT /OK_TO_FCN Responses from all relevant
            clients.  Only mmgsdi_session_ok_to_refresh can be handled
            in this state.
           ---------------------------------------------------------*/
           mmgsdi_status = mmgsdi_refresh_process_ok_init_rsp(
                             slot_data_ptr,
                             req_ptr,
                             extra_param_ptr);
           break;

         case MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE:
           mmgsdi_status = mmgsdi_refresh_process_ok_fcn_rsp(
                           slot_data_ptr,
                           req_ptr,
                           extra_param_ptr);
           break;

         default:
           UIM_MSG_ERR_1("Orig Ok to Refresh received in bad state 0x%x",
                         slot_data_ptr->refresh_state);
           break;
       }
       break;

     case MMGSDI_REFRESH_ORIG_PROCEED_WITH_REFRESH_REQ:
       switch(slot_data_ptr->refresh_state)
       {
         case MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE:
           mmgsdi_status = mmgsdi_refresh_process_ok_to_proceed_rsp(
                             slot_data_ptr,
                             req_ptr,
                             extra_param_ptr);
           break;
         default:
           UIM_MSG_ERR_1("Orig Ok to pdown received in bad state 0x%x",
                         slot_data_ptr->refresh_state);
           break;
       }
       break;

     case MMGSDI_REFRESH_ORIG_COMPLETE_REQ:
       switch(slot_data_ptr->refresh_state)
       {
         /*------------------------------------------------------------
          Wait for Refresh Complete commands from clients who are
          involved in the INIT Process or the FCN Process
          mmgsdi_refresh_complete can be handled in this state:
         ------------------------------------------------------------*/
         case MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE:
         case MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE:
           mmgsdi_status = mmgsdi_refresh_process_complete_init(
                           slot_data_ptr,
                           req_ptr,
                           extra_param_ptr);
           break;
         case MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE:
           mmgsdi_status = mmgsdi_refresh_process_complete_fcn(
                              slot_data_ptr,
                             req_ptr,
                             extra_param_ptr);
           break;
         case MMGSDI_REFRESH_STEERING_IN_PROGRESS_STATE:
           mmgsdi_status = mmgsdi_refresh_process_complete_steering(
                             slot_data_ptr,
                             req_ptr,
                             extra_param_ptr);
           break;
         default:
           UIM_MSG_ERR_1("Orig Complete received in bad state 0x%x",
                         slot_data_ptr->refresh_state);
           break;

       }
       break;

    case MMGSDI_REFRESH_ORIG_TIMER_EXPIRY_REQ:
      switch(slot_data_ptr->refresh_state)
      {
        case MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE:
        case MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE:
        case MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE:
        case MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE:
        case MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE:
        case MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE:
          mmgsdi_status = mmgsdi_refresh_process_timer_expiry(
                            slot_data_ptr,
                            req_ptr,
                            extra_param_ptr);
          break;
        default:
          UIM_MSG_ERR_1("Timer Expiry received in bad state 0x%x",
                        slot_data_ptr->refresh_state);
          break;
      }
      break;

    case MMGSDI_REFRESH_ORIG_REG_ALL_REQ:
      mmgsdi_status = mmgsdi_refresh_req_process_reg_all(
                        req_ptr);
      break;

    case MMGSDI_REFRESH_ORIG_DEREG_ALL_REQ:
      mmgsdi_status = mmgsdi_refresh_req_process_dereg_all(
                        req_ptr);
      break;

     default:
       UIM_MSG_ERR_1("Refresh: Bad Request, cannot process anything 0x%x",
                     req_ptr->refresh_req_type);
       break;
  }

  /* Abort INIT REFRESH processing if PIN state is not appropriate */
  if(req_ptr->refresh_req_type != MMGSDI_REFRESH_ORIG_REG_REQ &&
     req_ptr->refresh_req_type != MMGSDI_REFRESH_ORIG_DEREG_REQ &&
     req_ptr->refresh_req_type != MMGSDI_REFRESH_ORIG_REG_ALL_REQ &&
     req_ptr->refresh_req_type != MMGSDI_REFRESH_ORIG_DEREG_ALL_REQ)
  {
    mmgsdi_refresh_process_init_pin_check(extra_param_ptr, slot_index, &mmgsdi_status);
  }

  /* Do not clear mmgsdi_refresh_data_info if this is a retry request
     (MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ). For refresh retry, refresh related
     info is just copied to mmgsdi_refresh_data_info when
     mmgsdi_refresh_retry_handle_req() is called, which will be used when
     mmgsdi_refresh_main() is called next time when the actual request is being
     processed.
   */
  if(req_ptr->refresh_req_type != MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ)
  {
    if(extra_param_ptr->refresh_data.stage == MMGSDI_REFRESH_STAGE_END_FAILED ||
       extra_param_ptr->refresh_data.stage == MMGSDI_REFRESH_STAGE_START)
    {
      mmgsdi_clear_refresh_data_info();
    }
  }

  /* Freeing temporary PB cache as part of REFRESH at the following REFRESH STAGES
     STAGE_START (or) STAGE_END_SUCCESS. The reason for adding STAGE_END_SUCCESS is
     there will be no STAGE_START in case of no clients interested in the file that
     currently being refreshed.*/
  if(extra_param_ptr->refresh_data.stage == MMGSDI_REFRESH_STAGE_START ||
     (req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_COMPLETE_REQ &&
      extra_param_ptr->refresh_data.stage == MMGSDI_REFRESH_STAGE_END_SUCCESS))
  {
    /* Free the refreshed file cache only on PB files */
    if(extra_param_ptr->refresh_data.mode == MMGSDI_REFRESH_NAA_FCN)
    {
      mmgsdi_pb_free_fcn_cache(slot_index,
                               &mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files,
                               req_ptr->request_header.session_id);
    }
    else
    {
      /* Delete entire cache for UICC RESET as it is a complete card reset. 
         For other modes --> Delete cache specific to session for which refresh is currently ongoing */
      mmgsdi_pb_free_cache_data(req_ptr->request_header.slot_id,
		                 (extra_param_ptr->refresh_data.mode != MMGSDI_REFRESH_RESET) ? req_ptr->request_header.session_id : MMGSDI_INVALID_SESSION_ID,
		                 (extra_param_ptr->refresh_data.mode != MMGSDI_REFRESH_RESET) ? FALSE : TRUE);
    }
  }

  (void)mmgsdi_refresh_send_evt(&extra_param_ptr->refresh_data,
                                &req_ptr->request_header);

  if((slot_data_ptr != NULL) &&
     (orig_state != slot_data_ptr->refresh_state))
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /* Defer the starting of timer to the point where MMGSDI has finished its
     * initialization. This will ensure that refresh does not fails if for some
     * reason MMGSDI took time in initialization.
     * In case it is a non Prov app refresh, start the timer here, because
     * mmgsdi is not going to call refresh complete.
     * In case of refresh reset or refresh reset auto mode, start the timer so
     * that we donot wait indefinitely for RESET response from UIMDrv
     */
    if((slot_data_ptr->refresh_state !=
                             MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE) ||

       (mmgsdi_refresh_info_ptr[slot_index] != NULL &&
        (mmgsdi_refresh_info_ptr[slot_index]->node_ptr != NULL) &&
         !mmgsdi_util_is_prov_session(mmgsdi_refresh_info_ptr[slot_index]->node_ptr->session_type)) ||

       (mmgsdi_refresh_info_ptr[slot_index] != NULL &&
        (mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET ||
         mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET_AUTO)))
    {
      mmgsdi_refresh_start_timer(slot_data_ptr, slot_index);
    }
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  }

  if(mmgsdi_status == MMGSDI_SUCCESS ||
     mmgsdi_status == MMGSDI_REFRESH_SUCCESS ||
     mmgsdi_status == MMGSDI_REFRESH_IN_PROGRESS)
  {
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                   index, MMGSDI_REFRESH_REQ,
                   mmgsdi_status);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
}/*mmgsdi_refresh_main*/


/* ==========================================================================
  FUNCTION MMGSDI_INTERNAL_REFRESH_APP_RESET_AUTO_CB

  DESCRIPTION
    This function, defined as mmgsdi_callback_type, will handle
    app switch/deactivate related CNF geneted by MMGSDI

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
=============================================================================*/
void mmgsdi_internal_refresh_app_reset_auto_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  int32                              client_index    = 0;
  mmgsdi_request_header_type         req_header      = {0};
  uim_cmd_type                      *uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr = NULL;
  mmgsdi_sw_status_type              status_word     = {0};
  mmgsdi_cnf_type                   *act_cnf_ptr     = NULL;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  /* Slot on which the confirmation is sent for */
  uint8                              slot_index_cnf  = MMGSDI_SLOT_1_INDEX;
  /* Slot on which original request is sent for */
  uint8                              slot_index_req  = MMGSDI_SLOT_1_INDEX;
  mmgsdi_int_app_info_type          *app_info_ptr    = NULL;

  if (cnf_ptr == NULL)
  {
    return;
  }

  /* Obtain the index */
  client_index = (int32)cnf_ptr->response_header.client_data;

  UIM_MSG_HIGH_3("mmgsdi_internal_refresh_app_reset_auto_cb, status: 0x%x, cnf: 0x%x, client_index: 0x%x",
                 status, cnf, client_index);

  /* Retrieve the original Session switch/deactivate request */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                    client_index,
                    &req_header,
                    &uim_cmd_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Refresh app reset auto: Could not extract the client info - 0x%x",
                  mmgsdi_status);
    return;
  }

  /* When we reach here as part of app deactivation, in success case, normally
     we would have sent SESSION_CHANGED_EVT to all the pertinent sessions and
     corresponding app info would be reset as part of release_app_info and in that
     case app_info_ptr would be null already when we reach here. However, if we
     reached here because the app-deactivation/refresh_reset_auto failed, session_id
     might still be valid and we need to ensure to mark that the app is no longer
     being deactivated */
  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                      req_header.session_id,
                      NULL,
                      NULL,
                      &app_info_ptr);
  if((mmgsdi_status == MMGSDI_SUCCESS) && (app_info_ptr != NULL))
  {
    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT
    app_info_ptr->deactivation_in_progress = FALSE;
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT
  }

  /* Get index for the slot on which refresh (i.e. app deactivation) happened */
  mmgsdi_status = mmgsdi_util_get_slot_index(cnf_ptr->response_header.slot_id,
                                             &slot_index_cnf);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Refresh app reset auto: invalid slot id, 0x%x",
                  req_header.slot_id);
    return;
  }
  if((slot_index_cnf != MMGSDI_SLOT_1_INDEX) &&
     (slot_index_cnf != MMGSDI_SLOT_2_INDEX) &&
     (slot_index_cnf != MMGSDI_SLOT_3_INDEX))
  {
    UIM_MSG_ERR_1("Invalid slot_index_cnf, 0x%x", slot_index_cnf);
    return;
  }

  /* If a confirmation has already been sent for this command as part of
     CARD_ERROR handling, free only the client_req_table index. */
  if(mmgsdi_client_req_table_info_ptr[client_index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED)
  {
    UIM_MSG_HIGH_2("Free client_req_table index 0x%x for cmd 0x%x",
                   client_index,
                   mmgsdi_client_req_table_info_ptr[client_index]->request_type);

    mmgsdi_util_free_client_request_table_index(client_index);

    /* Cleanup the REFRESH globals */
    mmgsdi_refresh_rsp_clean_up(slot_index_cnf);

    return;
  }

  switch(cnf)
  {
    case MMGSDI_REFRESH_CNF:
      /* Either a client responded with ok_to_init as FALSE or there was
         some generic error while handling the REFRESH */
      if(status != MMGSDI_SUCCESS)
      {
        /* Queue the confirmation */
        mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                          &req_header,
                          NULL,
                          NULL,
                          FALSE,
                          status_word);
      }
      break;

    case MMGSDI_SESSION_DEACTIVATE_CNF:
      if(mmgsdi_refresh_info_ptr[slot_index_cnf] == NULL)
      {
        UIM_MSG_ERR_1("Refresh app reset auto: Deactivate cnf, REFRESH ptr[0x%x] is NULL",
                      slot_index_cnf);
        return;
      }
      if(req_header.orig_request_type == MMGSDI_SESSION_DEACTIVATE_REQ)
      {
        mmgsdi_session_cnf_add_info_type session_cnf_info;

        mmgsdi_memscpy((void*)&session_cnf_info.app_info,
                        sizeof(session_cnf_info.app_info),
                        (void*)&cnf_ptr->session_deactivate_cnf.app_id,
                        sizeof(mmgsdi_aid_type));

        /* Queue the confirmation */
        mmgsdi_status = mmgsdi_cnf_build_and_queue(status,
                                                   &req_header,
                                                   NULL,
                                                   (void*)&session_cnf_info,
                                                   FALSE,
                                                   status_word);

      }
      else if(req_header.orig_request_type == MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ)
      {
        if(status != MMGSDI_SUCCESS)
        {
          /* Queue the confirmation */
          mmgsdi_status = mmgsdi_cnf_build_and_queue(status,
                                                     &req_header,
                                                     NULL,
                                                     NULL,
                                                     FALSE,
                                                     status_word);
        }
        else
        {
          if(extra_param_ptr == NULL)
          {
            UIM_MSG_ERR_0("Refresh app reset auto: Could not get the extra_param");

            /* Queue the confirmation */
            mmgsdi_status = mmgsdi_cnf_build_and_queue(MMGSDI_ERROR,
                                                       &req_header,
                                                       NULL,
                                                       NULL,
                                                       FALSE,
                                                       status_word);
          }
          /* Start the app activation */
          else
          {
            mmgsdi_session_act_or_switch_prov_req_type msg;

            /* Create the app activate request */
            mmgsdi_memscpy(&msg.request_header,
                           sizeof(msg.request_header),
                           &req_header,
                           sizeof(mmgsdi_request_header_type));
            msg.request_header.request_type = MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ;
            msg.request_header.response_cb  = mmgsdi_internal_refresh_app_reset_auto_cb;
            msg.request_header.client_data  = (mmgsdi_client_data_type)client_index;

            msg.app_id_data.data_len = extra_param_ptr->open_data.aid.aid.data_len;
            mmgsdi_memscpy((void*)msg.app_id_data.data_ptr,
                           sizeof(msg.app_id_data.data_ptr),
                           (void*)extra_param_ptr->open_data.aid.aid.data_ptr,
                           msg.app_id_data.data_len);

            /* Start app activation: (deactivation: slot x, activation: slot y)
               1. If the app deactivation and activation happens on the same slot (x==y)
                  call mmgsdi_session_uicc_open_channel_chk() directly
               2. Otherwise (x!=y), need to check whether the same app is already active
                  or being activated on slot y, call
                  mmgsdi_session_activate_provisioning_start() to go through the check
             */
            if(cnf_ptr->response_header.slot_id == req_header.slot_id)
            {
              mmgsdi_status = mmgsdi_session_uicc_open_channel_chk(&msg);
            }
            else
            {
              mmgsdi_status = mmgsdi_session_pre_prov_activation_start(&msg);
            }

            if(mmgsdi_status == MMGSDI_SUCCESS)
            {
              UIM_MSG_HIGH_1("Refresh app reset auto: slot 0x%x prov. app activation successfully started",
                             req_header.slot_id);

              /* If the deactivated app is on slot x (slot_index_cnf) and the app
                 being activated is on a slot y (slot_index_req), where x!=y,
                 move refresh info from slot x to slot y and clean up refresh
                 related info on slot x

                 Note:
                 Clean up is needed even if the refresh info is not copied to
                 mmgsdi_refresh_info_ptr[slot_index_req] since when
                 MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF is received,
                 cnf_ptr->response_header.slot_id will be slot y, i.e. slot_index_req
               */
              if(cnf_ptr->response_header.slot_id != req_header.slot_id)
              {
                mmgsdi_status = mmgsdi_util_get_slot_index(req_header.slot_id,
                                                           &slot_index_req);
                if((MMGSDI_SUCCESS == mmgsdi_status)            &&
                   /* The following checks may not be needed since the conditions
                      already checked in mmgsdi_session_activate_provisioning_start() */
                   ((slot_index_req == MMGSDI_SLOT_1_INDEX) ||
                    (slot_index_req == MMGSDI_SLOT_2_INDEX) ||
                    (slot_index_req == MMGSDI_SLOT_3_INDEX))    &&
                   (NULL == mmgsdi_refresh_info_ptr[slot_index_req]))
                {
                  /* Copy contents from mmgsdi_refresh_info_ptr[slot_index_cnf] to
                     mmgsdi_refresh_info_ptr[slot_index_req]
                     Since the contents of mmgsdi_refresh_info_ptr[slot_index_cnf]
                     will be freed later during refresh cleanup, make a shallow copy
                     to avoid unnecessary memory allocation and freeing.

                     Note:
                       Make sure set pointer type members to NULL before calling
                       mmgsdi_refresh_rsp_clean_up() on slot_index_cnf
                     and
                       If any of the following happens, better to make a deep copy
                       rather than adding more code here to avoid accidentally
                       introducing bugs since all the memory will be freed by
                       mmgsdi_refresh_rsp_clean_up() eventually.
                       1. mmgsdi_refresh_rsp_clean_up() needs to access pointer type
                          contents during cleanup
                       2. New pointer type member is introduced to mmgsdi_refresh_info_type
                   */

                  /* Allocate memory and copy data */
                  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_refresh_info_ptr[slot_index_req],
                                                     sizeof(mmgsdi_refresh_info_type));
                  if(NULL != mmgsdi_refresh_info_ptr[slot_index_req])
                  {
                    /* Shallow copy, this copies everything */
                    *mmgsdi_refresh_info_ptr[slot_index_req] = *mmgsdi_refresh_info_ptr[slot_index_cnf];

                    /* Set pointer type members in mmgsdi_refresh_info_ptr[slot_index_cnf] to NULL */
                    memset(&mmgsdi_refresh_info_ptr[slot_index_cnf]->refresh_data.refresh_files,
                           0x00,
                           sizeof(mmgsdi_refresh_info_ptr[slot_index_cnf]->refresh_data.refresh_files));
                    memset(&mmgsdi_refresh_info_ptr[slot_index_cnf]->steering_mode_data,
                           0x00,
                           sizeof(mmgsdi_refresh_info_ptr[slot_index_cnf]->steering_mode_data));
                    memset(&mmgsdi_refresh_info_ptr[slot_index_cnf]->refresh_retry_files,
                           0x00,
                           sizeof(mmgsdi_refresh_info_ptr[slot_index_cnf]->refresh_retry_files));
                    mmgsdi_refresh_info_ptr[slot_index_cnf]->node_ptr = NULL;
                  }
                }

                /* Take caution when accessing mmgsdi_refresh_info_ptr[slot_index_cnf]
                   since pointer type members have been set NULL after shallow copy */
                mmgsdi_refresh_rsp_clean_up(slot_index_cnf);
              }
              return;
            }
            else
            {
              /* Queue the confirmation */
              mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                                         &req_header,
                                                         NULL,
                                                         NULL,
                                                         FALSE,
                                                         status_word);
            }
          }
        }
      }
      break;

    case MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF:
      if(NULL == mmgsdi_refresh_info_ptr[slot_index_cnf])
      {
        UIM_MSG_ERR_1("Refresh app reset auto: switch cnf, REFRESH ptr[0x%x] is NULL",
                      slot_index_cnf);
        return;
      }
      if((MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ == req_header.orig_request_type) &&
         req_header.response_cb)
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(act_cnf_ptr,sizeof(mmgsdi_cnf_type));

        if(act_cnf_ptr)
        {
          /* Call the original client CB here. This is being done instead of
             queueing a confirmation because the client CB needs to be called
             before the SESSION_CHANGED event is sent out irrespective of
             whether the original request lead to an activation or switch.

             Since mmgsdi_session_act_or_switch_prov_cnf_type has no pointer
             type members, use mmgsdi_memscpy() directly to copy cnf info
           */
          mmgsdi_memscpy((void*)act_cnf_ptr,
                         sizeof(mmgsdi_cnf_type),
                         (const void*)cnf_ptr,
                         sizeof(mmgsdi_cnf_type));
          act_cnf_ptr->response_header.client_data = req_header.client_data;
          (req_header.response_cb)(status, cnf, act_cnf_ptr);
          MMGSDIUTIL_TMC_MEM_FREE(act_cnf_ptr);
        }
      }
      break;

    default:
      break;
  }

  mmgsdi_util_free_client_request_table_index(client_index);

  /* Cleanup the REFRESH globals since switch/deact. provisioning is complete.
     In case of ACT_OR_SWITCH from slot x to slot y, to avoid clearing the
     refresh request that is queued on slot y before deactivation on slot x
     is complete, free mmgsdi_refresh_info_ptr[slot_index_cnf] only if
     the index to the mmgsdi_client_req_table_info_ptr[] matches client_index.
  */
  if((MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF != cnf) ||
     (mmgsdi_refresh_info_ptr[slot_index_cnf]->request_header.client_data == client_index))
  {
    mmgsdi_refresh_rsp_clean_up(slot_index_cnf);
  }
}/*mmgsdi_internal_refresh_app_reset_auto_cb*/


/*===========================================================================
  FUNCTION MMGSDI_UTIL_CHECK_CLIENT_FCN_LIST

  DESCRIPTION:
    This function checks the fcn list for any additional files
    being read during a fcn refresh

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    Additional File read flag is set if any files other than the ones
    listed in FCN command are read from the card
 ===========================================================================*/
void mmgsdi_util_check_client_fcn_list(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    mmgsdi_file
)
{
  uint8                                i                = 0;
  boolean                              file_found       = FALSE;
  mmgsdi_session_info_type           * session_ptr      = NULL;
  mmgsdi_channel_info_type           * channel_ptr      = NULL;
  mmgsdi_return_enum_type              mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type             slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  uint8                                slot_index       = MMGSDI_SLOT_1_INDEX;

  if(mmgsdi_file == MMGSDI_NO_FILE_ENUM ||
     mmgsdi_file == MMGSDI_MAX_FILE_ENUM )
  {
    UIM_MSG_ERR_0("Refresh:  FCN Check cannot be performed, filename invalid");
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (slot_id == MMGSDI_MAX_SLOT_ID_ENUM))
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }
  if((slot_index != MMGSDI_SLOT_1_INDEX) &&
     (slot_index != MMGSDI_SLOT_2_INDEX) &&
     (slot_index != MMGSDI_SLOT_3_INDEX))
  {
    return;
  }

  if((mmgsdi_refresh_info_ptr[slot_index]) &&
     (mmgsdi_refresh_info_ptr[slot_index]->node_ptr) &&
     (mmgsdi_refresh_info_ptr[slot_index]->orig_mode == MMGSDI_REFRESH_NAA_FCN) &&
     (mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_files > 0) &&
     (mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.file_list_ptr) &&
     (mmgsdi_refresh_info_ptr[slot_index]->refresh_status == MMGSDI_REFRESH_IN_PROGRESS))
  {
    mmgsdi_status = mmgsdi_util_get_session_and_channel_info(session_id,
                                                            &session_ptr,
                                                            &channel_ptr);

    if((mmgsdi_status != MMGSDI_SUCCESS) || (session_ptr == NULL) ||
       (channel_ptr == NULL) || (channel_ptr->app_info_ptr == NULL))
    {
       UIM_MSG_ERR_1("Invalid Session Id 0x0x", session_id);
    }
    else
    {
      if(channel_ptr->slot_id != mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id)
      {
        return;
      }
      for(i=0;i<mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.num_files;i++)
      {
        if(mmgsdi_file !=
             mmgsdi_refresh_info_ptr[slot_index]->refresh_data.refresh_files.file_list_ptr[i])
        {
          continue;
        }

        mmgsdi_status = mmgsdi_util_cmp_aid(&channel_ptr->app_info_ptr->app_data,
                                            &mmgsdi_refresh_info_ptr[slot_index]->aid,
                                            TRUE);

        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          /*File is being read by some Session with the same AID */
          file_found = TRUE;
          break;
        }
      }/* End of for */
    }

    if(file_found == FALSE)
    {
      /*The information is only required in the head node. Ignore the others*/
      mmgsdi_refresh_info_ptr[slot_index]->additional_files_read = TRUE;
    }
  }
}/*mmgsdi_util_check_client_fcn_list*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_HANDLE_REFRESH_START_STEERING

  DESCRIPTION:
    This function will replace the highest priority entries in MMGSDIs cached
    "Operator Controlled PLMN Selector with Access Technology" list with the
    list provided in the REFRESH command.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    OPLMwAcT list in cache will be updated.
 ===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_handle_refresh_start_steering(
  const mmgsdi_event_data_type  * event_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  boolean                 pass_fail     = TRUE;
  mmgsdi_access_type      access_info;

  if(event_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Event Info Pointer");
    return MMGSDI_ERROR;
  }

  memset(&access_info, 0x00, sizeof(mmgsdi_access_type));
  access_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  access_info.file.file_enum = MMGSDI_USIM_OPLMNWACT;

  /* At this point EF OPLMNwACT is already cached and refresh start
     streering reads the entire file so mark that complete file is
     coming from the card. */
  mmgsdi_status = mmgsdi_cache_write(event_ptr->session_id,
                                     &access_info,
                                     event_ptr->data.refresh.plmn_list,
                                     0x00,
                                     MMGSDI_DATA_FROM_CARD_COMPLETE);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    pass_fail = FALSE;
  }

  mmgsdi_status = mmgsdi_session_refresh_complete(event_ptr->session_id,
                                                  pass_fail,
                                                  mmgsdi_util_internal_cb,
                                                  0);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to send Refresh Complete");
  }

  return mmgsdi_status;
} /* mmgsdi_refresh_handle_refresh_start_steering */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_CONSOLIDATE_TABLE

  DESCRIPTION:
    This function will remove lower priority and duplicate refresh reqs from
    the refresh retry table.

  DEPENDENCIES:
    The refresh_info_ptr must be populated

  LIMITATIONS:
    None

  RETURN VALUE
    MMGSDI_INCORRECT_PARAMS   : Invalid params
    MMGSDI_SUCCESS            : Otherwise

  SIDE EFFECTS:
    None
 ===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_retry_consolidate_table(
  uint8 slot_index
)
{
  mmgsdi_refresh_retry_table_entry_type *table_entry_ptr  = NULL;
  uint32                                 retry_index  = 0;
  mmgsdi_refresh_mode_enum_type          refresh_mode = MMGSDI_REFRESH_MODE_ENUM_MAX;

  UIM_MSG_HIGH_0("Refresh Retry: Consolidate table");
  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX],
                              mmgsdi_refresh_retry_settings_ptr[slot_index]);

  refresh_mode = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->mode;

  if(mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->request_header.slot_id != MMGSDI_SLOT_1)
  {
    /* Currently implemented only for slot1 don't retry refreshes on
       other slots*/
    return MMGSDI_SUCCESS;
  }

  /* If there is a refresh reset all other retry attempts can
     be removed as they will occur as part of the card reset */
  table_entry_ptr = mmgsdi_refresh_retry_table[MMGSDI_REFRESH_RETRY_RESET];
  if(table_entry_ptr != NULL)
  {
    if(refresh_mode == MMGSDI_REFRESH_RESET || refresh_mode == MMGSDI_REFRESH_RESET_AUTO)
    {
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count = table_entry_ptr->card_req.retry_info.retry_count;
      mmgsdi_refresh_retry_clear_table(MMGSDI_SLOT_1_INDEX);
    }
    return MMGSDI_SUCCESS;
  }

  if(refresh_mode == MMGSDI_REFRESH_RESET || refresh_mode == MMGSDI_REFRESH_RESET_AUTO)
  {
    mmgsdi_refresh_retry_clear_table(MMGSDI_SLOT_1_INDEX);
    return MMGSDI_SUCCESS;
  }

  if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_STEERING_OF_ROAMING,
                                                      &retry_index))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(refresh_mode == MMGSDI_REFRESH_STEERING_OF_ROAMING)
  {
    /* Try any new steering of roaming requests but first remove
       existing retry requests from the retry table */
    if(mmgsdi_refresh_retry_table[retry_index] != NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_refresh_retry_table[retry_index]);
    }
    return MMGSDI_SUCCESS;
  }

  if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_3G_SESSION_RESET,
                                                      &retry_index))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  table_entry_ptr = mmgsdi_refresh_retry_table[retry_index];
  if(table_entry_ptr != NULL)
  {
    if(refresh_mode == MMGSDI_REFRESH_3G_SESSION_RESET)
    {
      mmgsdi_refresh_retry_clear_fcn_entry(retry_index,
                                           &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count);

    }
    return MMGSDI_SUCCESS;
  }

  if(refresh_mode == MMGSDI_REFRESH_3G_SESSION_RESET)
  {
    /* For a 3G Session Reset clear other APP_RESET, INIT_FFCN, FCN, INIT_FCN
        and INIT refreshses from the retry table for the app being refreshed */
    mmgsdi_refresh_retry_clear_table_index(retry_index - 1);/*APP_RESET*/
    mmgsdi_refresh_retry_clear_table_index(retry_index - 2);/*NAA_INIT_FULL_FCN*/
    mmgsdi_refresh_retry_clear_table_index(retry_index - 3);/*NAA_FCN*/
    mmgsdi_refresh_retry_clear_table_index(retry_index - 4);/*NAA_INIT_FCN*/
    mmgsdi_refresh_retry_clear_table_index(retry_index - 5);/*NAA_INIT*/
    return MMGSDI_SUCCESS;
  }

  if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_NAA_APP_RESET,
                                                      &retry_index))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  table_entry_ptr = mmgsdi_refresh_retry_table[retry_index];
  if(table_entry_ptr != NULL)
  {
    if(refresh_mode == MMGSDI_REFRESH_NAA_APP_RESET || refresh_mode == MMGSDI_REFRESH_APP_RESET_AUTO)
    {
      /* Remove the matching retry entry from the table */
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count = table_entry_ptr->card_req.retry_info.retry_count;
      mmgsdi_refresh_retry_clear_table_index(retry_index);/*NAA_APP_RESET*/
    }
    return MMGSDI_SUCCESS;
  }

  if(refresh_mode == MMGSDI_REFRESH_NAA_APP_RESET ||
     refresh_mode == MMGSDI_REFRESH_APP_RESET_AUTO)
  {
    /* For a reset app clear the other INIT_FFCN, FCN, INIT_FCN and INIT
       refreshses from the retry table for the app being refreshed */
    mmgsdi_refresh_retry_clear_table_index(retry_index - 1);/*NAA_INIT_FULL_FCN*/
    mmgsdi_refresh_retry_clear_table_index(retry_index - 2);/*NAA_FCN*/
    mmgsdi_refresh_retry_clear_table_index(retry_index - 3);/*NAA_INIT_FCN*/
    mmgsdi_refresh_retry_clear_table_index(retry_index - 4);/*NAA_INIT*/
    return MMGSDI_SUCCESS;
  }

  if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_NAA_INIT_FULL_FCN,
                                                      &retry_index))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  table_entry_ptr = mmgsdi_refresh_retry_table[retry_index];
  if(table_entry_ptr != NULL)
  {
    if(refresh_mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN)
    {
      /* Remove the matching retry entry from the table */
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count = table_entry_ptr->card_req.retry_info.retry_count;
      mmgsdi_refresh_retry_clear_table_index(retry_index);
    }
    return MMGSDI_SUCCESS;
  }

  if(refresh_mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN)
  {
    /* For a INIT_FFCN refreshes clear the other FCN, INIT_FCN and INIT
       refreshses from the retry table for the app being refreshed */
    mmgsdi_refresh_retry_clear_table_index(retry_index - 2); /* NAA_FCN */
    mmgsdi_refresh_retry_clear_table_index(retry_index - 3); /* NAA_INIT_FCN */
    mmgsdi_refresh_retry_clear_table_index(retry_index - 4); /* NAA_INIT */
    return MMGSDI_SUCCESS;
  }

  if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_NAA_INIT,
                                                      &retry_index))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(refresh_mode == MMGSDI_REFRESH_NAA_INIT)
  {
    table_entry_ptr = mmgsdi_refresh_retry_table[retry_index];
    if(table_entry_ptr != NULL)
    {
      /* Remove the matching retry entry from the table */
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count = table_entry_ptr->card_req.retry_info.retry_count;
      mmgsdi_refresh_retry_clear_table_index(retry_index);
      return MMGSDI_SUCCESS;
    }
  }

  if(refresh_mode == MMGSDI_REFRESH_NAA_INIT_FCN)
  {
    /*  For INIT_FCN clear the other INIT and matching FCN refresh
        requests from the retry table for the app being refreshed */
    mmgsdi_refresh_retry_clear_table_index(retry_index);
    mmgsdi_refresh_retry_clear_fcn_entry(retry_index + 1,
                                         &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count);
    return MMGSDI_SUCCESS;
  }
  if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_NAA_FCN,
                                                      &retry_index))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(refresh_mode == MMGSDI_REFRESH_NAA_FCN)
  {
    /*  Check the table for the current FCN. If it exists in
        in the table remove it and increment the retry count
        of the FCN refresh */
    mmgsdi_refresh_retry_clear_fcn_entry(retry_index,
                                         &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count);
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_retry_consolidate_table */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_CLEAR_TABLE

  DESCRIPTION:
    This function will remove all requests from the Retry Table

  DEPENDENCIES:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
void mmgsdi_refresh_retry_clear_table(
  uint8 slot_index
)
{
  int i = 0;

  if(mmgsdi_refresh_retry_settings_ptr[slot_index] == NULL)
  {
    return;
  }

  for(i = 0; i < MMGSDI_REFRESH_RETRY_TABLE_SIZE; i++)
  {
    mmgsdi_refresh_retry_clear_table_index(i);
  }
} /* mmgsdi_refresh_retry_clear_table */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_CLEAR_TABLE_INDEX

  DESCRIPTION:
    This function will remove all requests from the Retry Table Entry

  DEPENDENCIES:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
static void mmgsdi_refresh_retry_clear_table_index(
  uint32 retry_index
)
{
  mmgsdi_refresh_retry_table_entry_type *entry_ptr = NULL;

  if(retry_index >= MMGSDI_REFRESH_RETRY_TABLE_SIZE)
  {
    UIM_MSG_ERR_0("Refresh: index out of bounds to clear retry table entry");
    return;
  }

  while(mmgsdi_refresh_retry_table[retry_index] != NULL)
  {
    entry_ptr = mmgsdi_refresh_retry_table[retry_index];
    mmgsdi_refresh_retry_table[retry_index] = entry_ptr->next;
    MMGSDIUTIL_TMC_MEM_FREE(entry_ptr);
  }
} /* mmgsdi_refresh_retry_clear_table_index */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_CLEAR_FCN_ENTRY

  DESCRIPTION:
    This function will remove a duplicate FCN from the Retry Table

  DEPENDENCIES:
    None

  LIMITATIONS:
    Only searches and removes one FCN.  There should not be more than one.

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
static void mmgsdi_refresh_retry_clear_fcn_entry(
  uint32  fcn_index,
  uint32 *retries_ptr
)
{
  mmgsdi_refresh_retry_table_entry_type *clear_entry_ptr = NULL;
  mmgsdi_refresh_retry_table_entry_type *last_entry_ptr  = NULL;

  if(fcn_index >= MMGSDI_REFRESH_RETRY_TABLE_SIZE || retries_ptr == NULL)
  {
    UIM_MSG_ERR_0("Refresh Retry: Improper params while clearing fcn");
    return;
  }

  if(mmgsdi_refresh_retry_table[fcn_index] != NULL)
  {
    /* Compare the current fcn refresh to the first
       fcn in the table by comparing their file lists */
    if((mmgsdi_refresh_retry_table[fcn_index]->card_req.refresh_files.data_len ==
        mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len) &&
        (memcmp(mmgsdi_refresh_retry_table[fcn_index]->card_req.refresh_files.data_ptr,
           mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_ptr,
           mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len) == 0 ))
    {
      /* Clear the matching FCN from the retry table */
      *retries_ptr = mmgsdi_refresh_retry_table[fcn_index]->card_req.retry_info.retry_count;
      clear_entry_ptr                       = mmgsdi_refresh_retry_table[fcn_index];
      mmgsdi_refresh_retry_table[fcn_index] = mmgsdi_refresh_retry_table[fcn_index]->next;
      MMGSDIUTIL_TMC_MEM_FREE(clear_entry_ptr);
      return;
    }
    else
    {
      last_entry_ptr  = mmgsdi_refresh_retry_table[fcn_index];
      clear_entry_ptr = mmgsdi_refresh_retry_table[fcn_index]->next;
    }
  }
  /* Compare the current fcn refresh the rest of the
     fcns in the table by comparing their file lists */
  while(clear_entry_ptr != NULL)
  {
    if((clear_entry_ptr->card_req.refresh_files.data_len ==
       mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len) &&
       (memcmp(clear_entry_ptr->card_req.refresh_files.data_ptr,
          mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_ptr,
          mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len) == 0))
    {
      /* Clear the matching FCN from the retry table */
      last_entry_ptr->next = clear_entry_ptr->next;
      *retries_ptr = clear_entry_ptr->card_req.retry_info.retry_count;
      MMGSDIUTIL_TMC_MEM_FREE(clear_entry_ptr);
      return;
    }
    else
    {
      last_entry_ptr  = clear_entry_ptr;
      clear_entry_ptr = clear_entry_ptr->next;
    }
  }

  UIM_MSG_HIGH_0("FCN refresh not currently in retry table");
} /* mmgsdi_refresh_retry_clear_fcn_entry */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_ADD_ENTRY

  DESCRIPTION:
    This function will add a failed refresh to the Retry Table

  DEPENDENCIES:
    A refresh is in progress and consolidate table has been called

  LIMITATIONS:
    None

  RETURN VALUE
    MMGSDI_SUCCESS : Retry table was updated appropriately
    MMGSDI_ERROR   : Otherwise

  SIDE EFFECTS:
    None
 ===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_retry_add_entry(
  uint8 slot_index
)
{
  mmgsdi_refresh_retry_table_entry_type  *new_table_entry_ptr  = NULL;
  mmgsdi_refresh_retry_table_entry_type  *table_entry_ptr      = NULL;
  mmgsdi_refresh_mode_enum_type           refresh_mode         = MMGSDI_REFRESH_MODE_ENUM_MAX;
  uint32                                  retry_index          = 0;
  uint32                                  fcn_node_count       = 0;
  uint32                                  table_entry_size     = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX],
                              mmgsdi_refresh_retry_settings_ptr[slot_index]);

  UIM_MSG_HIGH_0("Refresh Retry: Add Entry");
  /* This function will be called only while a refresh is
     being processed and only after a call to consolidate.
     So the current retry will not already exist in the table */
  refresh_mode = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->orig_mode;

  if(mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->request_header.slot_id != MMGSDI_SLOT_1 ||
     refresh_mode == MMGSDI_REFRESH_RESET_AUTO ||
     refresh_mode == MMGSDI_REFRESH_APP_RESET_AUTO)
  {
    return MMGSDI_SUCCESS;
  }

  if(mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count >= mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_limit)
  {
    UIM_MSG_HIGH_2("Refresh Retry: Not adding refresh to retry table. retry_count:0x%x retry_limit:0x%x",
                   mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count,
                   mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_limit);
    return MMGSDI_ERROR;
  }

  /* If a Refresh Reset is already in the retry table we don't need to add the
     current refresh to the retry as it will be taken care of by the reset */
  if(mmgsdi_refresh_retry_table[MMGSDI_REFRESH_RETRY_RESET] != NULL)
  {
    if(refresh_mode == MMGSDI_REFRESH_RESET)
    {
      return MMGSDI_ERROR;
    }
    return MMGSDI_SUCCESS;
  }

  /* Voted down Refresh Reset - Populate the Refresh Reset entry in the
     retry table and start the timer to retry the refresh later */
  if(refresh_mode == MMGSDI_REFRESH_RESET)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      new_table_entry_ptr,
      sizeof(mmgsdi_refresh_retry_table_entry_type));

    if(new_table_entry_ptr == NULL)
    {
      UIM_MSG_ERR_0("Refresh: Could not malloc for new Retry Table entry");
      return MMGSDI_ERROR;
    }

    new_table_entry_ptr->card_req.refresh_mode = refresh_mode;

    mmgsdi_memscpy(&new_table_entry_ptr->card_req.aid,
                   sizeof(new_table_entry_ptr->card_req.aid),
                   &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid,
                   sizeof(mmgsdi_aid_type));

    new_table_entry_ptr->next = NULL;
    new_table_entry_ptr->card_req.retry_info.retry_count  = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count;
    new_table_entry_ptr->card_req.retry_info.retry_rsp_cb = mmgsdi_refresh_retry_cb;

    mmgsdi_refresh_retry_table[MMGSDI_REFRESH_RETRY_RESET] = new_table_entry_ptr;

    /*Start retry timer and go one with request */
    (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);

    rex_def_timer_ex(&mmgsdi_refresh_retry_timer,
                     mmgsdi_refresh_retry_timer_expiry,
                     MMGSDI_SLOT_1);

    (void)rex_set_timer(&mmgsdi_refresh_retry_timer,
                        mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);

    return MMGSDI_SUCCESS;
  }

  if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_STEERING_OF_ROAMING,
                                                      &retry_index))
  {
    return MMGSDI_ERROR;
  }

  /* Voted down Steering of Roaming request - add the refresh to
     the retry table including copying the plmn list. Start the
     timer to refresh later. */
  if(refresh_mode == MMGSDI_REFRESH_STEERING_OF_ROAMING)
  {
    if(mmgsdi_refresh_retry_table[retry_index] != NULL)
    {
      UIM_MSG_ERR_0("Refresh Retry Steering of roaming already in retry table");
      return MMGSDI_ERROR;
    }
    mmgsdi_util_cal_align_size(sizeof(mmgsdi_refresh_retry_table_entry_type), (int32*)&table_entry_size);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_table_entry_ptr,
      (table_entry_size + mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->steering_mode_data.data_len));

    if(new_table_entry_ptr == NULL)
    {
      UIM_MSG_ERR_0("Refresh: Could not malloc for new Retry Table entry");
      return MMGSDI_ERROR;
    }
    new_table_entry_ptr->card_req.plmn_list.data_ptr = (uint8*)new_table_entry_ptr + table_entry_size;
    new_table_entry_ptr->card_req.refresh_mode       = refresh_mode;

    mmgsdi_memscpy(&new_table_entry_ptr->card_req.aid,
                   sizeof(new_table_entry_ptr->card_req.aid),
                   &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid,
                   sizeof(mmgsdi_aid_type));

    mmgsdi_memscpy(new_table_entry_ptr->card_req.plmn_list.data_ptr,
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->steering_mode_data.data_len,
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->steering_mode_data.data_ptr,
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->steering_mode_data.data_len);
    new_table_entry_ptr->card_req.plmn_list.data_len =
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->steering_mode_data.data_len;

    new_table_entry_ptr->card_req.retry_info.retry_count  = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count;
    new_table_entry_ptr->card_req.retry_info.retry_rsp_cb = mmgsdi_refresh_retry_cb;
    mmgsdi_refresh_retry_table[retry_index] = new_table_entry_ptr;
    (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);

    rex_def_timer_ex(&mmgsdi_refresh_retry_timer,
                     mmgsdi_refresh_retry_timer_expiry,
                     MMGSDI_SLOT_1);

    (void)rex_set_timer(&mmgsdi_refresh_retry_timer, mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);
    return MMGSDI_SUCCESS;
  }

  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_REFRESH_3G_SESSION_RESET_PER_ETSI_SPEC, 
                                  MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_DISABLED)
  {
    /* Voted down 3G_SESSION_RESET - Add the refresh to the retry table
       including the filelist and restart the timer to retry later */
    if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_3G_SESSION_RESET,
                                                        &retry_index))
    {
      return MMGSDI_ERROR;
    }

    /* If there is an 3G session Reset pending in the retry table it will take care of APP_RESET,
       INIT, INIT+FFCN, INIT+FCN and FCN refreshes. So return to avoid adding them to the 
       refresh retry table */
    if(mmgsdi_refresh_retry_table[retry_index] != NULL)
    {
      if(refresh_mode == MMGSDI_REFRESH_3G_SESSION_RESET)
      {
        return MMGSDI_ERROR;
      }
      return MMGSDI_SUCCESS;
    }

    if(refresh_mode == MMGSDI_REFRESH_3G_SESSION_RESET)
    {
      mmgsdi_util_cal_align_size(sizeof(mmgsdi_refresh_retry_table_entry_type), (int32*)&table_entry_size);
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_table_entry_ptr,
        (table_entry_size + mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len));

      if(new_table_entry_ptr == NULL)
      {
        UIM_MSG_ERR_0("Refresh: Could not malloc for new Retry Table entry");
        return MMGSDI_ERROR;
      }
      new_table_entry_ptr->card_req.refresh_files.data_ptr = (uint8*)new_table_entry_ptr + table_entry_size;
      new_table_entry_ptr->card_req.refresh_mode           = refresh_mode;

      mmgsdi_memscpy(&new_table_entry_ptr->card_req.aid,
                     sizeof(new_table_entry_ptr->card_req.aid),
                     &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid,
                     sizeof(mmgsdi_aid_type));

      mmgsdi_memscpy(new_table_entry_ptr->card_req.refresh_files.data_ptr,
                     mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len,
                     mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_ptr,
                     mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len);
      new_table_entry_ptr->card_req.refresh_files.data_len =
        mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len;

      new_table_entry_ptr->card_req.retry_info.retry_count  = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count;
      new_table_entry_ptr->card_req.retry_info.retry_rsp_cb = mmgsdi_refresh_retry_cb;
      new_table_entry_ptr->next                             = NULL;

      (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);

      if(mmgsdi_refresh_retry_table[retry_index] == NULL)
      {
        mmgsdi_refresh_retry_table[retry_index] = new_table_entry_ptr;
      }
      else
      {
        /* If there are existing refreshes in this table entry's queue
           count to make sure we have not exceeded the maximum queue size
           and then add the new refresh retry table entry at the end. */
        fcn_node_count  = 1;
        table_entry_ptr = mmgsdi_refresh_retry_table[retry_index];
        while(table_entry_ptr->next != NULL)
        {
          fcn_node_count++;
          table_entry_ptr = table_entry_ptr->next;
        }

        if(fcn_node_count >= mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size)
        {
          UIM_MSG_ERR_0("Refresh Retry: Cannot queue retry - fcn queue is at maximum length");
          MMGSDIUTIL_TMC_MEM_FREE(new_table_entry_ptr);
        }
        else
        {
          table_entry_ptr->next = new_table_entry_ptr;
          table_entry_ptr->next->next = NULL;
        }
      }

      rex_def_timer_ex(&mmgsdi_refresh_retry_timer,
                       mmgsdi_refresh_retry_timer_expiry,
                       MMGSDI_SLOT_1);

      (void)rex_set_timer(&mmgsdi_refresh_retry_timer, mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);

      if (new_table_entry_ptr == NULL)
      {
        return MMGSDI_ERROR;
      }
      else
      {
        return MMGSDI_SUCCESS;
      }
    }
  }

  if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_NAA_APP_RESET,
                                                      &retry_index))
  {
    return MMGSDI_ERROR;
  }

  /* If there is an App Reset pending in the retry table it
     will take care of INIT, INIT+FFCN, INIT+FCN and FCN refreshes
     so return to avoid adding them to the refresh retry table */
  if(mmgsdi_refresh_retry_table[retry_index] != NULL)
  {
    if(refresh_mode == MMGSDI_REFRESH_NAA_APP_RESET)
    {
      return MMGSDI_ERROR;
    }
    return MMGSDI_SUCCESS;
  }

  /* Voted down App Reset - add the refresh to the retry
     table and restart the timer to try later */
  if(refresh_mode == MMGSDI_REFRESH_NAA_APP_RESET)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_table_entry_ptr,
      sizeof(mmgsdi_refresh_retry_table_entry_type));

    if(new_table_entry_ptr == NULL)
    {
      UIM_MSG_ERR_0("Refresh: Could not malloc for new Retry Table entry");
      return MMGSDI_ERROR;
    }

    new_table_entry_ptr->card_req.refresh_mode = refresh_mode;

    mmgsdi_memscpy(&new_table_entry_ptr->card_req.aid,
                     sizeof(new_table_entry_ptr->card_req.aid),
                     &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid,
                     sizeof(mmgsdi_aid_type));

    new_table_entry_ptr->next = NULL;
    new_table_entry_ptr->card_req.retry_info.retry_count  = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count;
    new_table_entry_ptr->card_req.retry_info.retry_rsp_cb = mmgsdi_refresh_retry_cb;

    mmgsdi_refresh_retry_table[retry_index] = new_table_entry_ptr;

    (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);

    rex_def_timer_ex(&mmgsdi_refresh_retry_timer,
                     mmgsdi_refresh_retry_timer_expiry,
                     MMGSDI_SLOT_1);

    (void)rex_set_timer(&mmgsdi_refresh_retry_timer, mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);

    return MMGSDI_SUCCESS;
  }

  if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_NAA_INIT_FULL_FCN,
                                                      &retry_index))
  {
    return MMGSDI_ERROR;
  }

  /* If there is an INIT+FFCN pending in the retry table it
     will take care of INIT, INIT+FCN and FCN refreshes so
     return to avoid adding them to the refresh retry table */
  if(mmgsdi_refresh_retry_table[retry_index] != NULL)
  {
    if(refresh_mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN)
    {
      return MMGSDI_ERROR;
    }
    return MMGSDI_SUCCESS;
  }

  /* Voted down INIT+FFCN - add the refresh to the retry
     table and restart the timer to try later */
  if(refresh_mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_table_entry_ptr,
      sizeof(mmgsdi_refresh_retry_table_entry_type));

    if(new_table_entry_ptr == NULL)
    {
      UIM_MSG_ERR_0("Refresh: Could not malloc for new Retry Table entry");
      return MMGSDI_ERROR;
    }

    new_table_entry_ptr->card_req.refresh_mode = refresh_mode;

    mmgsdi_memscpy(&new_table_entry_ptr->card_req.aid,
                   sizeof(new_table_entry_ptr->card_req.aid),
                   &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid,
                   sizeof(mmgsdi_aid_type));

    new_table_entry_ptr->next = NULL;
    new_table_entry_ptr->card_req.retry_info.retry_count  = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count;
    new_table_entry_ptr->card_req.retry_info.retry_rsp_cb = mmgsdi_refresh_retry_cb;

    mmgsdi_refresh_retry_table[retry_index] = new_table_entry_ptr;

    /*Start retry timer and go one with request */
    (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);

    rex_def_timer_ex(&mmgsdi_refresh_retry_timer,
                     mmgsdi_refresh_retry_timer_expiry,
                     MMGSDI_SLOT_1);

    (void)rex_set_timer(&mmgsdi_refresh_retry_timer, mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);

    return MMGSDI_SUCCESS;
  }

  /* Voted down INIT+FCN - Add the refresh to the retry table
     including the filelist and restart the timer to retry later */
  if(refresh_mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
     (refresh_mode == MMGSDI_REFRESH_3G_SESSION_RESET &&
      mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_REFRESH_3G_SESSION_RESET_PER_ETSI_SPEC, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
        MMGSDI_FEATURE_ENABLED))
  {
    if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_NAA_INIT_FCN,
                                                        &retry_index))
    {
      return MMGSDI_ERROR;
    }

    mmgsdi_util_cal_align_size(sizeof(mmgsdi_refresh_retry_table_entry_type), (int32*)&table_entry_size);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_table_entry_ptr,
      (table_entry_size + mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len));

    if(new_table_entry_ptr == NULL)
    {
      UIM_MSG_ERR_0("Refresh: Could not malloc for new Retry Table entry");
      return MMGSDI_ERROR;
    }
    new_table_entry_ptr->card_req.refresh_files.data_ptr = (uint8*)new_table_entry_ptr + table_entry_size;
    new_table_entry_ptr->card_req.refresh_mode           = refresh_mode;

    mmgsdi_memscpy(&new_table_entry_ptr->card_req.aid,
                   sizeof(new_table_entry_ptr->card_req.aid),
                   &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid,
                   sizeof(mmgsdi_aid_type));

    mmgsdi_memscpy(new_table_entry_ptr->card_req.refresh_files.data_ptr,
                   mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len,
                   mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_ptr,
                   mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len);
    new_table_entry_ptr->card_req.refresh_files.data_len =
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len;

    new_table_entry_ptr->card_req.retry_info.retry_count  = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count;
    new_table_entry_ptr->card_req.retry_info.retry_rsp_cb = mmgsdi_refresh_retry_cb;
    new_table_entry_ptr->next                             = NULL;

    (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);

    if(mmgsdi_refresh_retry_table[retry_index] == NULL)
    {
      mmgsdi_refresh_retry_table[retry_index] = new_table_entry_ptr;
    }
    else
    {
      /* If there are existing refreshes in this table entry's queue
         count to make sure we have not exceeded the maximum queue size
         and then add the new refresh retry table entry at the end. */
      fcn_node_count = 1;
      table_entry_ptr    = mmgsdi_refresh_retry_table[retry_index];
      while(table_entry_ptr->next != NULL)
      {
        fcn_node_count++;
        table_entry_ptr = table_entry_ptr->next;
      }

      if(fcn_node_count >= mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size)
      {
        UIM_MSG_ERR_0("Refresh Retry: Cannot queue retry - fcn queue is at maximum length");
        MMGSDIUTIL_TMC_MEM_FREE(new_table_entry_ptr);
      }
      else
      {
        table_entry_ptr->next = new_table_entry_ptr;
        table_entry_ptr->next->next = NULL;
      }
    }

    rex_def_timer_ex(&mmgsdi_refresh_retry_timer,
                     mmgsdi_refresh_retry_timer_expiry,
                     MMGSDI_SLOT_1);

    (void)rex_set_timer(&mmgsdi_refresh_retry_timer, mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);

    if (new_table_entry_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }
    else
    {
      return MMGSDI_SUCCESS;
    }
  }

  /* Voted down FCN - Add the refresh to the retry table
     including the filelist and restart the timer to retry later */
  if(refresh_mode == MMGSDI_REFRESH_NAA_FCN)
  {
    if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_NAA_FCN,
                                                        &retry_index))
    {
      return MMGSDI_ERROR;
    }
    mmgsdi_util_cal_align_size(sizeof(mmgsdi_refresh_retry_table_entry_type), (int32*)&table_entry_size);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_table_entry_ptr,
      (table_entry_size + mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len));

    if(new_table_entry_ptr == NULL)
    {
      UIM_MSG_ERR_0("Refresh: Could not malloc for new Retry Table entry");
      return MMGSDI_ERROR;
    }
    new_table_entry_ptr->card_req.refresh_files.data_ptr = (uint8*)new_table_entry_ptr + table_entry_size;
    new_table_entry_ptr->card_req.refresh_mode           = refresh_mode;

    mmgsdi_memscpy(&new_table_entry_ptr->card_req.aid,
                   sizeof(new_table_entry_ptr->card_req.aid),
                   &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid,
                   sizeof(mmgsdi_aid_type));

    mmgsdi_memscpy(new_table_entry_ptr->card_req.refresh_files.data_ptr,
                   mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len,
                   mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_ptr,
                   mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len);
    new_table_entry_ptr->card_req.refresh_files.data_len =
      mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->refresh_retry_files.data_len;

    new_table_entry_ptr->card_req.retry_info.retry_count  = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count;
    new_table_entry_ptr->card_req.retry_info.retry_rsp_cb = mmgsdi_refresh_retry_cb;
    new_table_entry_ptr->next                             = NULL;

    (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);

    if(mmgsdi_refresh_retry_table[retry_index] == NULL)
    {
      mmgsdi_refresh_retry_table[retry_index] = new_table_entry_ptr;
    }
    else
    {
      /* If there are existing refreshes in this table entry's queue
         count to make sure we have not exceeded the maximum queue size
         and then add the new refresh retry table entry at the end. */
      fcn_node_count  = 1;
      table_entry_ptr = mmgsdi_refresh_retry_table[retry_index];
      while(table_entry_ptr->next != NULL)
      {
        fcn_node_count++;
        table_entry_ptr = table_entry_ptr->next;
      }

      if(fcn_node_count >= mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size)
      {
        UIM_MSG_ERR_0("Refresh Retry: Cannot queue retry - fcn queue is at maximum length");
        MMGSDIUTIL_TMC_MEM_FREE(new_table_entry_ptr);
        return MMGSDI_ERROR;
      }
      else
      {
        table_entry_ptr->next = new_table_entry_ptr;
      }
    }

    rex_def_timer_ex(&mmgsdi_refresh_retry_timer,
                     mmgsdi_refresh_retry_timer_expiry,
                     MMGSDI_SLOT_1);

    (void)rex_set_timer(&mmgsdi_refresh_retry_timer, mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);

    if(new_table_entry_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }
    else
    {
      return MMGSDI_SUCCESS;
    }
  }

  /* Voted down INIT - Add the refresh to the retry table
     and restart the timer to retry later */
  if(refresh_mode == MMGSDI_REFRESH_NAA_INIT)
  {
    if(MMGSDI_SUCCESS != mmgsdi_refresh_retry_get_index(MMGSDI_REFRESH_NAA_INIT,
                                                        &retry_index))
    {
      return MMGSDI_ERROR;
    }
    if(mmgsdi_refresh_retry_table[retry_index] != NULL)
    {
      return MMGSDI_ERROR;
    }
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(new_table_entry_ptr,
      sizeof(mmgsdi_refresh_retry_table_entry_type));

    if(new_table_entry_ptr == NULL)
    {
      UIM_MSG_ERR_0("Refresh: Could not malloc for new Retry Table entry");
      return MMGSDI_ERROR;
    }

    new_table_entry_ptr->card_req.refresh_mode = refresh_mode;

    mmgsdi_memscpy(&new_table_entry_ptr->card_req.aid,
                   sizeof(new_table_entry_ptr->card_req.aid),
                   &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid,
                   sizeof(mmgsdi_aid_type));

    new_table_entry_ptr->next = NULL;
    new_table_entry_ptr->card_req.retry_info.retry_count  = mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->retry_count;
    new_table_entry_ptr->card_req.retry_info.retry_rsp_cb = mmgsdi_refresh_retry_cb;

    mmgsdi_refresh_retry_table[retry_index] = new_table_entry_ptr;

    (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);

    rex_def_timer_ex(&mmgsdi_refresh_retry_timer,
                     mmgsdi_refresh_retry_timer_expiry,
                     MMGSDI_SLOT_1);

    (void)rex_set_timer(&mmgsdi_refresh_retry_timer, mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);

    return MMGSDI_SUCCESS;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_retry_add_entry */


/*===========================================================================
FUNCTION MMGSDI_REFRESH_RETRY_CB

DESCRIPTION
  The default callback for refresh retries

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  If the retry was successful, queue the next retry req from the refresh
  retry table into the mmgsdi_cmd_q
===========================================================================*/
void mmgsdi_refresh_retry_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  uint32                                 i           = 0;
  mmgsdi_refresh_req_type                req         = {{0}};
  mmgsdi_refresh_retry_table_entry_type *table_entry = NULL;
  uint8                                  slot_index  = 0;

  if((mmgsdi_util_get_slot_index(cnf_ptr->response_header.slot_id,&slot_index) == MMGSDI_SUCCESS) &&
      mmgsdi_refresh_retry_settings_ptr[slot_index] == NULL)
  {
    UIM_MSG_ERR_0("Refresh Retry: NULL global data ptr in retry cb");
    return;
  }

  UIM_MSG_HIGH_1("refresh_retry_cb, status: 0x%x", mmgsdi_status);

  if(mmgsdi_status == MMGSDI_REFRESH_LATER)
  {
    UIM_MSG_HIGH_0("Refresh Retry callback. Refresh voted down");
    return;
  }
  if((mmgsdi_status != MMGSDI_REFRESH_SUCCESS) &&
     (mmgsdi_status != MMGSDI_REFRESH_ADDITIONAL_FILES_READ))
  {
    /* Refresh failed.  Restart retry timer to check retry table */
    UIM_MSG_ERR_1("Refresh Retry callback - Refresh error. mmgsdi_status:0x%x",
                  mmgsdi_status);
    (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);

    rex_def_timer_ex(&mmgsdi_refresh_retry_timer,
                     mmgsdi_refresh_retry_timer_expiry,
                     MMGSDI_SLOT_1);

    (void)rex_set_timer(&mmgsdi_refresh_retry_timer, mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);
    return;
  }

  if(mmgsdi_refresh_retry_table_index >= MMGSDI_REFRESH_RETRY_TABLE_SIZE)
  {
    UIM_MSG_ERR_0("Refresh Retry retry index out of bounds.");
    mmgsdi_refresh_retry_table_index = 0;
  }

  i = mmgsdi_refresh_retry_table_index;
  /* Go through the entire table once looking for a retry entry. Start after the
     last entry retried - stop when we find an entry or return to the start */
  do
  {
    if(mmgsdi_refresh_retry_table[i] != NULL)
    {
      mmgsdi_memscpy(&req.refresh.card_req,
                     sizeof(req.refresh.card_req),
                     &mmgsdi_refresh_retry_table[i]->card_req,
                     sizeof(mmgsdi_refresh_card_req_type));

      req.refresh_req_type                        = MMGSDI_REFRESH_ORIG_REFRESH_REQ;
      table_entry                                 = mmgsdi_refresh_retry_table[i];
      req.refresh.card_req.retry_info.retry_count = table_entry->card_req.retry_info.retry_count++;
      mmgsdi_refresh_retry_table[i]               = mmgsdi_refresh_retry_table[i]->next;
      req.refresh.card_req.source                 = MMGSDI_REFRESH_SOURCE_MMGSDI;

      (void)mmgsdi_client_build_refresh_req(
                        mmgsdi_generic_data_ptr->client_id,
                        cnf_ptr->response_header.slot_id,
                        &req,
                        FALSE,
                        req.refresh.card_req.retry_info.retry_rsp_cb,
                        0,
                        FALSE);
      MMGSDIUTIL_TMC_MEM_FREE(table_entry);

      UIM_MSG_HIGH_1("Refresh Retry success. Next retry command found at index: %d", i);

      /* Increment the index to start searching from next time */
      i++;
      if(i == MMGSDI_REFRESH_RETRY_TABLE_SIZE)
      {
        mmgsdi_refresh_retry_table_index = 0;
      }
      else
      {
        mmgsdi_refresh_retry_table_index = i;
      }
      return;
    }
    i++;
    if(i == MMGSDI_REFRESH_RETRY_TABLE_SIZE)
    {
      i = 0;
    }
  }while (i != mmgsdi_refresh_retry_table_index);

  UIM_MSG_HIGH_0("Refresh Retry success. Retry table is now empty");
} /* mmgsdi_refresh_retry_cb */


/*===========================================================================
FUNCTION MMGSDI_REFRESH_RETRY_PRINT_TABLE

DESCRIPTION
  Prints each populated entry in the refresh retry table

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_refresh_retry_print_table(
  void
)
{
  int i = 0;
  mmgsdi_refresh_retry_table_entry_type *next_entry = NULL;

  if(mmgsdi_refresh_retry_settings_ptr[MMGSDI_SLOT_1_INDEX] == NULL)
  {
    UIM_MSG_ERR_0("Refresh Retry: NULL global data ptr in retry cb");
    return;
  }

  UIM_MSG_HIGH_0("Refresh Retry - mmgsdi_refresh_retry_table:");

  for(i = 0; i < MMGSDI_REFRESH_RETRY_TABLE_SIZE; i++)
  {
    next_entry = mmgsdi_refresh_retry_table[i];

    while(next_entry != NULL)
    {
      UIM_MSG_HIGH_2("Refresh Retry type: 0x%x, for app: 0x%x",
                     next_entry->card_req.refresh_mode,
                     next_entry->card_req.aid.app_type);
      next_entry = next_entry->next;
    }
  }
} /* mmgsdi_refresh_retry_print_table */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_HANDLE_REQ

  DESCRIPTION
    This function is called to handle the refresh retry timer expiring
    to queue the next retry from the refresh retry table

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    The global refresh retry index is incremented
===========================================================================*/
static void mmgsdi_refresh_retry_handle_req(
  void
)
{
  uint32                                 i             = 0;
  mmgsdi_refresh_req_type                req          = {{0}};
  mmgsdi_refresh_retry_table_entry_type *table_entry  = NULL;
  mmgsdi_slot_id_enum_type               slot_id_enum = MMGSDI_SLOT_1;

  if(mmgsdi_refresh_retry_table_index >= MMGSDI_REFRESH_RETRY_TABLE_SIZE)
  {
    UIM_MSG_ERR_0("Refresh Retry retry index out of bounds.");
    mmgsdi_refresh_retry_table_index = 0;
  }

  i = mmgsdi_refresh_retry_table_index;

  /* Go through the entire table once looking for a retry entry. Start after the
     last entry retried - stop when we find an entry or return to the start */
  do
  {
    if(mmgsdi_refresh_retry_table[i] != NULL)
    {
      mmgsdi_memscpy(&req.refresh.card_req,
                     sizeof(req.refresh.card_req),
                     &mmgsdi_refresh_retry_table[i]->card_req,
                     sizeof(mmgsdi_refresh_card_req_type));

      req.refresh_req_type                        = MMGSDI_REFRESH_ORIG_REFRESH_REQ;
      table_entry                                 = mmgsdi_refresh_retry_table[i];
      req.refresh.card_req.retry_info.retry_count = table_entry->card_req.retry_info.retry_count;
      mmgsdi_refresh_retry_table[i]               = mmgsdi_refresh_retry_table[i]->next;
      req.refresh.card_req.retry_info.retry_count++;
      req.refresh.card_req.source                 = MMGSDI_REFRESH_SOURCE_MMGSDI;

      /* Copy refresh info into mmgsdi_refresh_data_info */
      if(MMGSDI_SUCCESS != mmgsdi_copy_info_to_refresh_data_info(&req, slot_id_enum))
      {
        UIM_MSG_ERR_0("Fail to copy info into mmgsdi_refresh_data_info");
        return;
      }

      (void)mmgsdi_client_build_refresh_req(
                        mmgsdi_generic_data_ptr->client_id,
                        slot_id_enum,
                        &req,
                        FALSE,
                        req.refresh.card_req.retry_info.retry_rsp_cb,
                        0,
                        FALSE);
      MMGSDIUTIL_TMC_MEM_FREE(table_entry);

      UIM_MSG_HIGH_1("Refresh Retry Timer expired queued command from index %d", i);

      /* increment the index to start searching from next time */
      i++;
      if(i == MMGSDI_REFRESH_RETRY_TABLE_SIZE)
      {
        mmgsdi_refresh_retry_table_index = 0;
      }
      else
      {
        mmgsdi_refresh_retry_table_index = i;
      }
      return;
    }
    i++;
    if(i == MMGSDI_REFRESH_RETRY_TABLE_SIZE)
    {
      if(mmgsdi_refresh_retry_table_index == 0)
      {
        UIM_MSG_HIGH_0("Refresh Retry Timer expired with no retry in table");
        return;
      }
      else
      {
        i = 0;
      }
    }
  }while (i != mmgsdi_refresh_retry_table_index);

  UIM_MSG_HIGH_0("Refresh Retry Timer expired with no retry in table");
} /* mmgsdi_refresh_retry_handle_req */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_TIMER_EXPIRY

  DESCRIPTION
    This function is called when the refresh retry timer expires to
    send a request to mmgsdi to handle the retry

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_refresh_retry_timer_expiry(
  unsigned long slot_id
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type       req           = {{0}};

  UIM_MSG_HIGH_0("Refresh Retry mmgsdi_refresh_retry_timer_expiry");

  if(mmgsdi_generic_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("mmgsdi_generic_data_ptr is NULL");
    return;
  }

  if(slot_id != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_0("Refresh Retry only supported for SLOT1");
    return;
  }

  req.refresh_req_type  = MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ;
  req.refresh.retry_req = TRUE;

  mmgsdi_status = mmgsdi_client_build_refresh_req(
                    mmgsdi_generic_data_ptr->client_id,
                    MMGSDI_SLOT_1,
                    &req,
                    FALSE,
                    mmgsdi_util_internal_cb,
                    0,
                    FALSE);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Refresh Retry: Cannot build new request");
  }
} /* mmgsdi_refresh_retry_timer_expiry */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_GET_INDEX

  DESCRIPTION
    This function returns the index in the refresh retry table for the
    given app and refresh mode

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    MMGSDI_ERROR   : invalid app_type or refresh_mode
    MMGSDI_SUCCESS : retry_index set successfully AND retry_index_ptr
                     points to valid index for the refresh_retry table

  SIDE EFFECTS
    None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_retry_get_index(
  mmgsdi_refresh_mode_enum_type refresh_mode,
  uint32                       *retry_index_ptr
)
{
  int i = 0;
  int first_app_index = MMGSDI_REFRESH_RETRY_PROV_APPS;

  MMGSDIUTIL_RETURN_IF_NULL_2(retry_index_ptr, mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]);

  switch(refresh_mode)
  {
    case MMGSDI_REFRESH_RESET:
      *retry_index_ptr = MMGSDI_REFRESH_RETRY_RESET;
      return MMGSDI_SUCCESS;
    case MMGSDI_REFRESH_NAA_INIT:
      *retry_index_ptr = MMGSDI_REFRESH_NAA_INIT;
      break;
    case MMGSDI_REFRESH_NAA_INIT_FCN:
      *retry_index_ptr = MMGSDI_REFRESH_NAA_INIT_FCN;
      break;
    case MMGSDI_REFRESH_NAA_FCN:
      *retry_index_ptr = MMGSDI_REFRESH_NAA_FCN;
      break;
    case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
      *retry_index_ptr = MMGSDI_REFRESH_NAA_INIT_FULL_FCN;
      break;
    case MMGSDI_REFRESH_NAA_APP_RESET:
      *retry_index_ptr = MMGSDI_REFRESH_NAA_APP_RESET;
      break;
    case MMGSDI_REFRESH_3G_SESSION_RESET:
      *retry_index_ptr = MMGSDI_REFRESH_3G_SESSION_RESET;
      break;
    case MMGSDI_REFRESH_STEERING_OF_ROAMING:
      *retry_index_ptr = MMGSDI_REFRESH_STEERING_OF_ROAMING;
      break;
    default:
      UIM_MSG_ERR_1("Refresh Retry: Could not get index for refresh mode: 0x%x",
                    refresh_mode);
      return MMGSDI_INCORRECT_PARAMS;
  }

  switch(mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid.app_type)
  {
    case MMGSDI_APP_SIM:
      /* for APP_SIM the index the refresh mode from above */
      break;
    case MMGSDI_APP_RUIM:
      *retry_index_ptr += MMGSDI_REFRESH_RETRY_MODES;
      break;
    case MMGSDI_APP_USIM:
      *retry_index_ptr += 2 * MMGSDI_REFRESH_RETRY_MODES;
      break;
    case MMGSDI_APP_CSIM:
      *retry_index_ptr += 3 * MMGSDI_REFRESH_RETRY_MODES;
      break;
    default:
      for(i = (MMGSDI_REFRESH_RETRY_PROV_APPS * MMGSDI_REFRESH_RETRY_MODES);
          i < MMGSDI_REFRESH_RETRY_TABLE_SIZE; i++)
      {
        if(mmgsdi_refresh_retry_table[i] != NULL)
        {
          if(mmgsdi_util_cmp_aid(&mmgsdi_refresh_retry_table[i]->card_req.aid,
                                 &mmgsdi_refresh_info_ptr[MMGSDI_SLOT_1_INDEX]->aid,
                                 FALSE) == MMGSDI_SUCCESS)
          {
              *retry_index_ptr = *retry_index_ptr + i - (i % MMGSDI_REFRESH_RETRY_MODES);
              return MMGSDI_SUCCESS;
          }
          else
          {
            /* This is another refresh request for an app with a pending
               retry in the table.  Use this retry_table[app] index */
            if(first_app_index == (i / MMGSDI_REFRESH_RETRY_MODES))
            {
              first_app_index++;
            } /* if i has reached the next app */
          } /* if the aids don't match */
        } /* mmgsdi_refresh_retry_table[i] != NULL*/
      } /* for */

      *retry_index_ptr += first_app_index * MMGSDI_REFRESH_RETRY_MODES;
      /* The retry table has refreshes from five nonprov apps
         and this app is different from all five of them */
      if(*retry_index_ptr >= MMGSDI_REFRESH_RETRY_TABLE_SIZE)
      {
        UIM_MSG_ERR_0("Refresh Retry: Retry table is full with different apps");
        return MMGSDI_ERROR;
      }
    }
    return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_retry_get_index */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_INIT

  DESCRIPTION
    This function initializes the refresh retry settings from NV

  DEPENDENCIES
    None

  RETURN VALUE
    MMGSDI_ERROR   : If the global mmgsdi_refresh_settings_ptr has
                     already been initialized
    MMGSDI_SUCCESS : mmgsdi_refresh_settings_ptr is populated with
                     refresh retry info from NV
  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_retry_init(
  uint8 slot_index
)
{
  uim_common_efs_context_type  efs_context;

  efs_context = mmgsdi_util_get_common_efs_item_context(slot_index);
  if (efs_context == UIM_COMMON_EFS_CONTEXT_MAX)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(mmgsdi_refresh_retry_settings_ptr[slot_index] != NULL)
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_refresh_retry_settings_ptr[slot_index],
                                     sizeof(mmgsdi_refresh_retry_settings_type));

  if(mmgsdi_refresh_retry_settings_ptr[slot_index] == NULL)
  {
    UIM_MSG_ERR_0("Refresh Retry: Could not malloc for retry settings");
    return MMGSDI_ERROR;
  }

  /* Get NV values from efs */
  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_REFRESH_RETRY,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)mmgsdi_refresh_retry_settings_ptr[slot_index],
                          sizeof(mmgsdi_refresh_retry_settings_type)) != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_0("Refresh Retry init EFS read returned error");
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_refresh_retry_settings_ptr[slot_index]);
    return MMGSDI_ERROR;
  }

  if (!mmgsdi_refresh_retry_settings_ptr[slot_index]->flags)
  {
    UIM_MSG_ERR_0("Refresh Retry not enabled in NV");
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_refresh_retry_settings_ptr[slot_index]);
    return MMGSDI_SUCCESS;
  }

  if(mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer < MMMGSI_REFRESH_RETRY_TIMER)
  {
    mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer = MMMGSI_REFRESH_RETRY_TIMER;
  }

  if(mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size > MMMGSI_REFRESH_RETRY_MAX_QUEUE_SIZE ||
     mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size == 0)
  {
    mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size = MMMGSI_REFRESH_RETRY_MAX_QUEUE_SIZE;
  }

  if(mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_limit == 0)
  {
    mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_limit = MMGSDI_REFRESH_RETRY_MAX_RETRIES;
  }

  UIM_MSG_HIGH_3("Refresh Retry active: max_retries: 0x%x max_queue_size: 0x%x retry_timer: 0x%x",
                 mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_limit,
                 mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size,
                 mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);

  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_retry_init */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_UPDATE_RETRY_SETTINGS

  DESCRIPTION
    This function re-reads the refresh retry settings from NV

  DEPENDENCIES
    None

  RETURN VALUE
    MMGSDI_ERROR   : If the global mmgsdi_refresh_settings_ptr has
                     already been initialized
    MMGSDI_SUCCESS : mmgsdi_refresh_settings_ptr is populated with
                     refresh retry info from NV
  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_update_retry_settings(
  uint8 slot_index
)
{
  uim_common_efs_context_type  efs_context   = UIM_COMMON_EFS_CONTEXT_MAX;

  efs_context = mmgsdi_util_get_common_efs_item_context(slot_index);
  if (efs_context == UIM_COMMON_EFS_CONTEXT_MAX)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* free existing retry settings */
  if(mmgsdi_refresh_retry_settings_ptr[slot_index] != NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_refresh_retry_settings_ptr[slot_index]);
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_refresh_retry_settings_ptr[slot_index],
                                         sizeof(mmgsdi_refresh_retry_settings_type));

  if(mmgsdi_refresh_retry_settings_ptr[slot_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Get NV values from efs */
  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_REFRESH_RETRY,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)mmgsdi_refresh_retry_settings_ptr[slot_index],
                          sizeof(mmgsdi_refresh_retry_settings_type)) != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_0("Could not read refresh retry settings");
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_refresh_retry_settings_ptr[slot_index]);
    return MMGSDI_ERROR;
  }

  if (!mmgsdi_refresh_retry_settings_ptr[slot_index]->flags)
  {
    UIM_MSG_ERR_0("Refresh Retry not enabled in NV");
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_refresh_retry_settings_ptr[slot_index]);
    return MMGSDI_SUCCESS;
  }

  if(mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer < MMMGSI_REFRESH_RETRY_TIMER)
  {
    mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer = MMMGSI_REFRESH_RETRY_TIMER;
  }

  if(mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size > MMMGSI_REFRESH_RETRY_MAX_QUEUE_SIZE ||
     mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size == 0)
  {
    mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size = MMMGSI_REFRESH_RETRY_MAX_QUEUE_SIZE;
  }

  if(mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_limit == 0)
  {
    mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_limit = MMGSDI_REFRESH_RETRY_MAX_RETRIES;
  }

  UIM_MSG_HIGH_3("Refresh Retry active: max_retries: 0x%x max_queue_size: 0x%x retry_timer: 0x%x",
                 mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_limit,
                 mmgsdi_refresh_retry_settings_ptr[slot_index]->max_queue_size,
                 mmgsdi_refresh_retry_settings_ptr[slot_index]->retry_timer);

  return MMGSDI_SUCCESS;
} /* mmgsdi_refresh_update_retry_settings */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_CACHE_OPL_PNN

  DESCRIPTION
    This function delete the OPL-PNN cache and re-cache all the reocrds

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    MMGSDI_RETURN_ENUM_TYPE

  SIDE EFFECTS

===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_refresh_cache_opl_pnn(
  mmgsdi_session_id_type session_id,
  mmgsdi_client_id_type  client_id,
  mmgsdi_file_enum_type  file_enum)
{
  mmgsdi_slot_id_enum_type            slot_id               = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_session_info_type           *session_info_ptr      = NULL;
  mmgsdi_session_type_enum_type       session_type          = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_get_file_attr_req_type      *get_file_attr_req_ptr = NULL;
  mmgsdi_get_file_attr_cnf_type      *get_file_attr_cnf_ptr = NULL;
  mmgsdi_search_req_type             *search_req_ptr        = NULL;
  mmgsdi_search_cnf_type             *search_cnf_ptr        = NULL;
  mmgsdi_read_req_type               *read_req_ptr          = NULL;
  mmgsdi_read_cnf_type               *read_cnf_ptr          = NULL;
  byte                                i                     = 0;
  uint32                              num_rec               = 0;
  uint32                              num_of_invalid_rec    = 0;
  uint32                              len_of_rec            = 0;
  mmgsdi_rec_num_type                 valid_rec_number      = 0;
  mmgsdi_rec_num_type                 next_valid_rec_num    = 0;
  mmgsdi_len_type                     next_valid_rec_len    = 0;
  mmgsdi_int_app_info_type           *app_info_ptr          = NULL;
  mmgsdi_access_type                  access;
  mmgsdi_data_type                    data;

  UIM_MSG_HIGH_3("eons refresh file enum: 0x%x client id: 0x%x session id: 0x%x ",
                 file_enum, client_id, session_id);

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    &slot_id,
                    NULL,
                    &app_info_ptr,
                    NULL,
                    &session_info_ptr,
                    NULL);


  if(mmgsdi_status != MMGSDI_SUCCESS ||
     session_info_ptr == NULL ||
     app_info_ptr == NULL ||
     (session_info_ptr->session_type != MMGSDI_GW_PROV_PRI_SESSION &&
      session_info_ptr->session_type != MMGSDI_GW_PROV_SEC_SESSION &&
      session_info_ptr->session_type != MMGSDI_GW_PROV_TER_SESSION))
  {
    return MMGSDI_ERROR;
  }

  if(file_enum == MMGSDI_GSM_OPL &&
     app_info_ptr->app_data.app_type == MMGSDI_APP_USIM)
  {
    UIM_MSG_HIGH_0("Converting GSM OPL into USIM OPL");
    file_enum = MMGSDI_USIM_OPL;
  }

  session_type = session_info_ptr->session_type;

  UIM_MSG_HIGH_1("session type: 0x%x", session_type);

  mmgsdi_status = mmgsdi_eons_opl_pnn_delete_file_cache(session_id,
                                                        file_enum);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file_enum;

  /* Check status of EF in cached service table */
  mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
                    client_id,
                    session_id,
                    app_info_ptr->app_data.app_type,
                    &access );

  /*if file access is not allowed in svc table, set cache status as NOT FOUND*/
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    num_rec = 0;
    len_of_rec = 0;

    mmgsdi_status = mmgsdi_eons_opl_pnn_update_file_attr(session_id,
                                          file_enum,
                                          num_rec,
                                          len_of_rec,
                                          MMGSDI_ERROR);

    UIM_MSG_HIGH_1("update file attr return status: 0x%x", mmgsdi_status);

    memset(&data, 0x00, sizeof(mmgsdi_data_type));

    mmgsdi_status = mmgsdi_eons_opl_pnn_update_valid_record_info(
                      session_id,
                      file_enum,
                      data,
                      MMGSDI_ERROR);

    UIM_MSG_HIGH_1("update valid record info return status: 0x%x",
                   mmgsdi_status);

    return mmgsdi_status;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(get_file_attr_req_ptr,
    sizeof(mmgsdi_get_file_attr_req_type));
  if (get_file_attr_req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* Determine Number of Records and Len in OPL/PNN */
  get_file_attr_req_ptr->request_header.client_id         = client_id;
  get_file_attr_req_ptr->request_header.session_id        = session_id;
  get_file_attr_req_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_req_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_req_ptr->request_header.request_len       = sizeof(mmgsdi_get_file_attr_req_type);
  get_file_attr_req_ptr->request_header.payload_len       = 0;
  get_file_attr_req_ptr->request_header.slot_id           = slot_id;
  get_file_attr_req_ptr->request_header.client_data       = 0;
  get_file_attr_req_ptr->request_header.response_cb       = NULL;
  get_file_attr_req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
  get_file_attr_req_ptr->access.file.file_enum            = file_enum;
  get_file_attr_req_ptr->int_client_data                  = 0;
  get_file_attr_req_ptr->activate_aid                     = TRUE;
  /* Since we do not care about the sec attributes, skip the check */
  get_file_attr_req_ptr->skip_uicc_arr                    = TRUE;

  switch(file_enum)
  {
    case MMGSDI_USIM_OPL:
    case MMGSDI_USIM_PNN:
      mmgsdi_status = mmgsdi_uim_uicc_select(
                        get_file_attr_req_ptr,
                        TRUE,
                        &get_file_attr_cnf_ptr,
                        TRUE);
      break;
    case MMGSDI_GSM_OPL:
    case MMGSDI_GSM_PNN:
      mmgsdi_status = mmgsdi_uim_icc_select(
                        get_file_attr_req_ptr,
                        TRUE,
                        &get_file_attr_cnf_ptr);
      break;
    default:
      break;
  }

  MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_req_ptr);

  if(get_file_attr_cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Mem_Malloc Cnf ptr failed");
    return MMGSDI_ERROR;
  }

  if(get_file_attr_cnf_ptr->file_attrib.file_type != MMGSDI_LINEAR_FIXED_FILE)
  {
    UIM_MSG_ERR_0("file type is not linear fixed file");

    /* free the content of the cnf before freeing the cnf_ptr itself */
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)get_file_attr_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_cnf_ptr);
    return MMGSDI_ERROR;
  }


  if(mmgsdi_status == MMGSDI_SUCCESS &&
     get_file_attr_cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS)
  {
    num_rec = get_file_attr_cnf_ptr->file_attrib.file_info.linear_fixed_file.num_of_rec;
    len_of_rec = get_file_attr_cnf_ptr->file_attrib.file_info.linear_fixed_file.rec_len;
  }

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = get_file_attr_cnf_ptr->response_header.mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_eons_opl_pnn_update_file_attr(session_id,
                                                       file_enum,
                                                       num_rec,
                                                       len_of_rec,
                                                       mmgsdi_status);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("update file attr failed status: 0x%x", mmgsdi_status);
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)get_file_attr_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_cnf_ptr);
    return mmgsdi_status;
  }

  /* free the content of the cnf before freeing the cnf_ptr itself */
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)get_file_attr_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(get_file_attr_cnf_ptr);

  if (num_rec > 0 && len_of_rec > 0)
  {
    /*search invaid pattern in file*/
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(search_req_ptr,
      sizeof(mmgsdi_search_req_type));

    /* memory allocation failed for search req ptr */
    if (search_req_ptr == NULL)
    {
      UIM_MSG_ERR_0("Malloc failed for search_req_ptr");
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    /* Determine record numbers with invalid pattern (empty record)  in OPL/PNN */
    search_req_ptr->request_header.client_id         = client_id;
    search_req_ptr->request_header.session_id        = session_id;
    search_req_ptr->request_header.request_type      = MMGSDI_SEARCH_REQ;
    search_req_ptr->request_header.orig_request_type = MMGSDI_SEARCH_REQ;
    search_req_ptr->request_header.request_len       = sizeof(mmgsdi_search_req_type);
    search_req_ptr->request_header.payload_len       = 0;
    search_req_ptr->request_header.slot_id           = slot_id;
    search_req_ptr->request_header.client_data       = 0;
    search_req_ptr->request_header.response_cb       = NULL;
    search_req_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
    search_req_ptr->access.file.file_enum            = file_enum;

    if(file_enum == MMGSDI_USIM_OPL ||
       file_enum == MMGSDI_USIM_PNN)
    {
      search_req_ptr->search_type =  MMGSDI_UICC_SIMPLE_SEARCH;
    }
    else
    {
      search_req_ptr->search_type =  MMGSDI_ICC_SEARCH;
    }

    search_req_ptr->rec_num = 1;
    search_req_ptr->search_direction = MMGSDI_SEARCH_FORWARD_FROM_REC_NUM;
    search_req_ptr->seek_direction = MMGSDI_SEEK_BEGINNING_FORWARDS;
    search_req_ptr->enhanced_search_offset.offset_data = 0;
    search_req_ptr->enhanced_search_offset.offset_type = MMGSDI_SEARCH_OPTION_NONE;

    search_req_ptr->data.data_len = len_of_rec;
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(search_req_ptr->data.data_ptr,
                                       search_req_ptr->data.data_len);
    if(search_req_ptr->data.data_ptr != NULL)
    {
      memset(search_req_ptr->data.data_ptr, 0xFF, search_req_ptr->data.data_len);
    }
    else
    {
      UIM_MSG_ERR_2("Memory allocation failed for search data len 0x%x status: 0x%x",
                    search_req_ptr->data.data_len, mmgsdi_status);
      /* free the content of the cnf before freeing the cnf_ptr itself */
      MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    switch(file_enum)
    {
      case MMGSDI_USIM_OPL:
      case MMGSDI_USIM_PNN:
        mmgsdi_status = mmgsdi_uim_uicc_search_record(
                          search_req_ptr,
                          TRUE,
                          &search_cnf_ptr);
        break;
      case MMGSDI_GSM_OPL:
      case MMGSDI_GSM_PNN:
        mmgsdi_status = mmgsdi_uim_icc_search_record(
                          search_req_ptr,
                          TRUE,
                          &search_cnf_ptr);
        break;
      default:
        break;
    }

    MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr->data.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(search_req_ptr);

    if(search_cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Mem_Malloc Cnf ptr failed");
      return MMGSDI_ERROR;
    }

    if(search_cnf_ptr->access.access_method != MMGSDI_EF_ENUM_ACCESS ||
       search_cnf_ptr->access.file.file_enum != file_enum)
    {
      UIM_MSG_ERR_0("Search failed: access method or file enum does not match");

      /* free the content of the cnf before freeing the cnf_ptr itself */
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)search_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(search_cnf_ptr);
      return MMGSDI_ERROR;
    }
    num_of_invalid_rec = search_cnf_ptr->searched_record_nums.data_len;
    UIM_MSG_HIGH_2("Number of invalid record in file 0x%x : 0x%x",
                   file_enum, num_of_invalid_rec);

    mmgsdi_status = mmgsdi_eons_opl_pnn_update_valid_record_info(
                      session_id,
                      file_enum,
                      search_cnf_ptr->searched_record_nums,
                      search_cnf_ptr->response_header.mmgsdi_status);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("update valid record info failed status: 0x%x",
                    mmgsdi_status);

      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)search_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE(search_cnf_ptr);
      return mmgsdi_status;
    }
  }
  else
  {
    memset(&data, 0x00, sizeof(mmgsdi_data_type));
    mmgsdi_status = mmgsdi_eons_opl_pnn_update_valid_record_info(
                      session_id,
                      file_enum,
                      data,
                      MMGSDI_ERROR);
  }

  mmgsdi_status = mmgsdi_eons_opl_pnn_get_first_uncached_valid_record_number(
                    session_id,
                    file_enum,
                    &next_valid_rec_num,
                    &next_valid_rec_len);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     num_rec == 0 ||
     next_valid_rec_len == 0)
  {
    UIM_MSG_ERR_1("get first uncached valid record number failed status: 0x%x",
                  mmgsdi_status);

    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)search_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(search_cnf_ptr);
    return mmgsdi_status;
  }
  valid_rec_number = next_valid_rec_num;

  /* free the content of the cnf before freeing the cnf_ptr itself */
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)search_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(search_cnf_ptr);

  /* Increase cache ref count*/
  mmgsdi_status = mmgsdi_eons_opl_pnn_increase_and_get_ref_cnt(
                    session_id,
                    file_enum,
                    NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS )
  {
    UIM_MSG_ERR_1("opl pnn increase and get ref count failed status: 0x%x",
                  mmgsdi_status);
    return mmgsdi_status;
  }

  /* No variable len to include */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
    sizeof(mmgsdi_read_req_type));

  /* memory allocation failed for read req ptr */
  if (read_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("Malloc failed for read_req_ptr");
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  do
  {
    read_req_ptr->request_header.client_id         = client_id;
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
    read_req_ptr->access.file.file_enum  = file_enum;
    read_req_ptr->file_type              = MMGSDI_LINEAR_FIXED_FILE;
    read_req_ptr->rec_num                = valid_rec_number;
    read_req_ptr->data_len               = 0;/*Entire Record*/

    UIM_MSG_HIGH_1("Reading record number 0x%x", valid_rec_number);
    switch(file_enum)
    {
      case MMGSDI_USIM_OPL:
      case MMGSDI_USIM_PNN:
        mmgsdi_status = mmgsdi_uim_uicc_read_record(
                          read_req_ptr,
                          TRUE,
                          &read_cnf_ptr);
        break;
      case MMGSDI_GSM_OPL:
      case MMGSDI_GSM_PNN:
        mmgsdi_status = mmgsdi_uim_icc_read_record(
                          read_req_ptr,
                          TRUE,
                          &read_cnf_ptr);
        break;
      default:
        break;
    }

    if(read_cnf_ptr == NULL)
    {
      UIM_MSG_ERR_1("Read failed for record 0x%x", read_req_ptr->rec_num);

      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_ptr);
      /* free the content of the cnf before freeing the cnf_ptr itself */
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);
      return MMGSDI_ERROR;
    }

    UIM_MSG_HIGH_1("Reading  0x%x record success, updating  cache", i );

    /*update cache records*/
    next_valid_rec_num = 0;
    next_valid_rec_len = 0;

    mmgsdi_status = mmgsdi_eons_opl_pnn_get_next_uncached_valid_record_number(
                      session_id,
                      file_enum,
                      valid_rec_number,
                      &next_valid_rec_num,
                      &next_valid_rec_len);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Get next valid opl pnn record number failed status: 0x%x",
                    mmgsdi_status);

      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_ptr);
      /* free the content of the cnf before freeing the cnf_ptr itself */
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);
      return mmgsdi_status;
    }

    /* free the content of the cnf before freeing the cnf_ptr itself */
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)((void*)read_cnf_ptr));
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);
    valid_rec_number = next_valid_rec_num;
  }
  while(valid_rec_number);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_req_ptr);

  return mmgsdi_status;
}/*mmgsdi_refresh_cache_opl_pnn*/


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_RETRY_CLEAR_APP_ENTRIES

  DESCRIPTION:
    This function will remove all requests from the Retry Table Entry
    corresponding to the given app. Called upon deactivation of app.

  DEPENDENCIES:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
void mmgsdi_refresh_retry_clear_app_entries(
  mmgsdi_app_enum_type    app_type
)
{
  uint32                                 i           = 0;
  uint32                                 retry_index = 0;
  mmgsdi_refresh_retry_table_entry_type *entry_ptr   = NULL;

  switch(app_type)
  {
    case MMGSDI_APP_SIM:
      retry_index = 0;
      break;
    case MMGSDI_APP_RUIM:
      retry_index = MMGSDI_REFRESH_RETRY_MODES;
      break;
    case MMGSDI_APP_USIM:
      retry_index = 2 * MMGSDI_REFRESH_RETRY_MODES;
      break;
    case MMGSDI_APP_CSIM:
      retry_index = 3 * MMGSDI_REFRESH_RETRY_MODES;
      break;
    default:
      return;
  }

  /* Clear entries corresponding to the app from retry table except for
     Refresh Reset since this function is called only on app deactivation. */
  for(i = 1; i < MMGSDI_REFRESH_RETRY_MODES; i++)
  {
    while(mmgsdi_refresh_retry_table[retry_index + i] != NULL)
    {
      entry_ptr = mmgsdi_refresh_retry_table[retry_index + i];
      mmgsdi_refresh_retry_table[retry_index + i] = entry_ptr->next;
      MMGSDIUTIL_TMC_MEM_FREE(entry_ptr);
    }
  }
} /* mmgsdi_refresh_retry_clear_app_entries */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_PURGE_TEMP_IDENTITIES

  DESCRIPTION:
    This function purges a given XXLOCI file as follows:
      EF_LOCI bytes:
          Byte 1-4 (TMSI) – 0xFF
          Byte 5-7 (LAI)  – No Change
          Byte 8 (LAI)    – 0xFF
          Byte 9 (LAI)    – 0xFE
          Byte 10 (RFU)   – No Change
          Byte 11         – Not Updated 0x01
      EF_PSLOCI bytes:
          Byte 1-7 (PTMSI) - 0xFF
          Byte 8-10 (RAI)  - No Change
          Byte 11 (RAI)    – 0xFF
          Byte 12 (RAI)    – 0xFE
          Byte 13 (RAI)    – 0xFF
          Byte 14          – Not Updated 0x01
      EF_EPSLOCI bytes:
          Byte 1–12 (GUTI) - 0xFF
          Byte 13-15 (TAI) - No Change
          Byte 16 (TAI)    - 0xFF
          Byte 17 (TAI)    - 0xFE
          Byte 18          - Not Updated 0x01

  DEPENDENCIES:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
static void mmgsdi_refresh_purge_temp_identities(
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_session_id_type     session_id,
  mmgsdi_file_enum_type      file_enum,
  mmgsdi_protocol_enum_type  protocol
)
{
  mmgsdi_data_type                   read_write_data;
  mmgsdi_access_type                 access;
  uint8                              data_buf[MMGSDI_EPSLOCI_LEN];
  mmgsdi_write_req_type             *req_ptr       = NULL;
  mmgsdi_write_cnf_type             *cnf_ptr       = NULL;
  mmgsdi_return_enum_type            mmgsdi_status = MMGSDI_ERROR;

  UIM_MSG_MED_2("Purge temp identities: file_enum=0x%x, protocol=0x%x",
                file_enum, protocol);

  if((file_enum != MMGSDI_GSM_LOCI)    &&
     (file_enum != MMGSDI_USIM_LOCI)   &&
     (file_enum != MMGSDI_USIM_PSLOCI) &&
     (file_enum != MMGSDI_USIM_EPSLOCI))
  {
    return;
  }

  /* Read file from cache:
     Assumption: files are already cached at this point */
  memset(data_buf, 0x00, sizeof(data_buf));
  memset(&read_write_data, 0x00, sizeof(read_write_data));
  read_write_data.data_ptr = data_buf;
  switch(file_enum)
  {
    case MMGSDI_GSM_LOCI:
    case MMGSDI_USIM_LOCI:
      read_write_data.data_len = MMGSDI_LOCI_LEN;
      break;
    case MMGSDI_USIM_PSLOCI:
      read_write_data.data_len = MMGSDI_PSLOCI_LEN;
      break;
    case MMGSDI_USIM_EPSLOCI:
      read_write_data.data_len = MMGSDI_EPSLOCI_LEN;
      break;
    default:
      break;
  }

  memset(&access, 0x00, sizeof(access));
  access.access_method     = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum    = file_enum;

  if (MMGSDI_SUCCESS != mmgsdi_cache_read(session_id,
                                          &access,
                                          &read_write_data,
                                          0,
                                          NULL))
  {
    /* At the point of UICC RESET, the files should have already been cached.
       If for any reason, the cached is not available, we still want to go ahead
       overwrite the bytes as specified below for each XXLOCI file to make sure
       NAS will not use the data
       */
    UIM_MSG_HIGH_1("Cache read file_num 0x%x fail", file_enum);
  }

  /* Set corresponding bytes in EF_XXLOCI file */
  switch(file_enum)
  {
    case MMGSDI_GSM_LOCI:
    case MMGSDI_USIM_LOCI:
      /* Set EF_LOCI bytes:
          Byte 1-4 (TMSI) – 0xFF
          Byte 5-7 (LAI)  – No Change
          Byte 8 (LAI)    – 0xFF
          Byte 9 (LAI)    – 0xFE
          Byte 10 (RFU)   – No Change
          Byte 11         – Not Updated 0x01
       */
      memset(read_write_data.data_ptr, 0xFF, 4);
      read_write_data.data_ptr[7]  = 0xFF;
      read_write_data.data_ptr[8]  = 0xFE;
      read_write_data.data_ptr[10] = MMGSDI_LOCI_NOT_UPDATED;
      break;

    case MMGSDI_USIM_PSLOCI:
      /* Set EF_PSLOCI bytes:
          Byte 1-7 (PTMSI) - 0xFF
          Byte 8-10 (RAI)  - No Change
          Byte 11 (RAI)    – 0xFF
          Byte 12 (RAI)    – 0xFE
          Byte 13 (RAI)    – 0xFF
          Byte 14          – Not Updated 0x01
       */
      memset(read_write_data.data_ptr, 0xFF, 7);
      read_write_data.data_ptr[10] = 0xFF;
      read_write_data.data_ptr[11] = 0xFE;
      read_write_data.data_ptr[12] = 0xFF;
      read_write_data.data_ptr[13] = MMGSDI_LOCI_NOT_UPDATED;
      break;

    case MMGSDI_USIM_EPSLOCI:
      /* Set EF_EPSLOCI bytes:
          Byte 1–12 (GUTI) - 0xFF
          Byte 13-15 (TAI) - No Change
          Byte 16 (TAI)    - 0xFF
          Byte 17 (TAI)    - 0xFE
          Byte 18          - Not Updated 0x01
      */
      memset(read_write_data.data_ptr, 0xFF, 12);
      read_write_data.data_ptr[15] = 0xFF;
      read_write_data.data_ptr[16] = 0xFE;
      read_write_data.data_ptr[17] = MMGSDI_LOCI_NOT_UPDATED;
      break;

    default:
      break;
  }

  /* Allocate memory for mmgsdi_write_req_type */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr, sizeof(*req_ptr));
  if(!req_ptr)
  {
    UIM_MSG_ERR_0("Memory allocation fail");
    return;
  }

  /* Write the new data to EF_XXLOCI file:
     Call mmgsdi_uim_xicc_write_transparent() directly instead of
     mmgsdi_common_write() since mmgsdi_common_write() does not write to
     XXLOCI files if there is an ongoing purge to prevent any write request
     to XXLOCI files that is already queued but not processed yet at the time
     of purge from overwriting the XXLOCI files that are just purged.
   */
  req_ptr->request_header.session_id         = session_id;
  req_ptr->request_header.slot_id            = slot_id;
  req_ptr->request_header.request_type       = MMGSDI_WRITE_REQ;
  req_ptr->request_header.orig_request_type  = MMGSDI_WRITE_REQ;
  req_ptr->access                            = access;
  req_ptr->file_type                         = MMGSDI_TRANSPARENT_FILE;
  req_ptr->data                              = read_write_data;
  if(MMGSDI_UICC == protocol)
  {
    mmgsdi_status = mmgsdi_uim_uicc_write_transparent(req_ptr,
                                                      TRUE,
                                                      &cnf_ptr);
  }
  else if(MMGSDI_ICC == protocol)
  {
    mmgsdi_status = mmgsdi_uim_icc_write_transparent(req_ptr, TRUE, &cnf_ptr);
  }

  UIM_MSG_MED_1("mmgsdi_refresh_purge_temp_identities: mmgsdi_status=0x%x",
                 mmgsdi_status);

  if(cnf_ptr)
  {
    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
  }
  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
}/* mmgsdi_refresh_purge_temp_identities */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_CHECK_AND_PURGE_TEMP_IDENTITIES

  DESCRIPTION:
    This function sets Location Status byte in XXLOCI files to Not Updated
    if MMGSDI_NV_PURGE_TEMP_IDENTITIES_SUPPORT_I is enabled

  DEPENDENCIES:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
static void mmgsdi_refresh_check_and_purge_temp_identities(
  mmgsdi_slot_id_enum_type    refresh_slot_id,
  mmgsdi_protocol_enum_type   protocol
)
{
  mmgsdi_slot_id_enum_type    session_id_slot = MMGSDI_SLOT_1;
  mmgsdi_session_id_type      session_ids[3]  = {MMGSDI_SESSION_ID_ZERO};
  mmgsdi_client_id_type       client_id       = 0;
  boolean                     eps_mmi_avail   = FALSE;
  uint8                       i               = 0;
  mmgsdi_nv_context_type      nv_context;

  /* Do nothing if
     mmgsdi_generic_data_ptr is NULL
     or
     MMGSDI_FEATURE_PURGE_TEMP_IDENTITIES is disabled */
  nv_context = mmgsdi_util_get_efs_item_index_for_slot(refresh_slot_id);

  if(!mmgsdi_generic_data_ptr ||
     (MMGSDI_FEATURE_DISABLED ==
      mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_PURGE_TEMP_IDENTITIES, nv_context)))
  {
    return;
  }

  UIM_MSG_HIGH_2("Purge temp identities: refresh_slot_id=0x%x, protocol=0x%x",
                 refresh_slot_id, protocol);

  /* Retrieve session id and client id */
  session_ids[0] = mmgsdi_generic_data_ptr->pri_gw_session_id;
  session_ids[1] = mmgsdi_generic_data_ptr->sec_gw_session_id;
  session_ids[2] = mmgsdi_generic_data_ptr->ter_gw_session_id;
  client_id      = mmgsdi_generic_data_ptr->client_id;

  for(i = 0; i < (sizeof(session_ids) / sizeof(session_ids[0])); i++)
  {
    session_id_slot = MMGSDI_SLOT_1;
    if (MMGSDI_SUCCESS != mmgsdi_util_get_prov_session_info(session_ids[i],
                                                            NULL,
                                                            &session_id_slot,
                                                            NULL))
    {
      continue;
    }

    if(refresh_slot_id != session_id_slot)
    {
      continue;
    }

    /* Found a correct gw_session, purge EF_XXLOCI files */
    if(MMGSDI_ICC == protocol)
    {
      /* Purge EF_LOCI */
      mmgsdi_refresh_purge_temp_identities(refresh_slot_id,
                                           session_ids[i],
                                           MMGSDI_GSM_LOCI,
                                           protocol);
    }
    else if(MMGSDI_UICC == protocol)
    {
      /* Purge EF_LOCI */
      mmgsdi_refresh_purge_temp_identities(refresh_slot_id,
                                           session_ids[i],
                                           MMGSDI_USIM_LOCI,
                                           protocol);

      /* Purge EF_PSLOCI */
      mmgsdi_refresh_purge_temp_identities(refresh_slot_id,
                                           session_ids[i],
                                           MMGSDI_USIM_PSLOCI,
                                           protocol);

      /* Purge EF_EPSLOCI if EPS MMI service is available */
      if((MMGSDI_SUCCESS == mmgsdi_uicc_chk_srv_available(client_id,
                                                          session_ids[i],
                                                          MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                          MMGSDI_USIM_SRV_EPS_MMI,
                                                          &eps_mmi_avail))
         && eps_mmi_avail)
      {
        mmgsdi_refresh_purge_temp_identities(refresh_slot_id,
                                             session_ids[i],
                                             MMGSDI_USIM_EPSLOCI,
                                             protocol);
      }
    }
  }
}/* mmgsdi_refresh_check_and_purge_temp_identities */


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_FREE_REFRESH_INFO_TYPE

  DESCRIPTION:
    This function frees memory allocated for mmgsdi_refresh_info_type
    and set the pointer to NULL

  DEPENDENCIES:
    None

  RETURN VALUE
    None

  SIDE EFFECTS:
    None
 ===========================================================================*/
void mmgsdi_refresh_free_refresh_info_type(
  mmgsdi_refresh_info_type  **refresh_info_pptr
)
{
  mmgsdi_refresh_node_type  *node_ptr = NULL;

  if(refresh_info_pptr && (*refresh_info_pptr))
  {
    /* Free node_ptr */
    while((*refresh_info_pptr)->node_ptr)
    {
      node_ptr = (*refresh_info_pptr)->node_ptr;
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(node_ptr->refresh_files.file_list_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(node_ptr->refresh_files.file_path_ptr);
      (*refresh_info_pptr)->node_ptr = node_ptr->next_ptr;
      MMGSDIUTIL_TMC_MEM_FREE(node_ptr);
    }

    /* Free refresh_data */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      (*refresh_info_pptr)->refresh_data.refresh_files.file_list_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      (*refresh_info_pptr)->refresh_data.refresh_files.file_path_ptr);

    /* Free steering_mode_data */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK((*refresh_info_pptr)->steering_mode_data.data_ptr);

    /* Free refresh_retry_files */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK((*refresh_info_pptr)->refresh_retry_files.data_ptr);

    /* Free memory allocated for struct mmgsdi_refresh_info_type itself */
    MMGSDIUTIL_TMC_MEM_FREE(*refresh_info_pptr);
  }
}/* mmgsdi_refresh_free_refresh_info_type */

