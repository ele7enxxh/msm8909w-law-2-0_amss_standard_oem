/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   U T I L I T Y   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the utility functions needed in the MMGSDI module

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdiutil.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/02/16   rp      Set is_status_same flag for set_service_status request
10/25/16   dd      Added support for auto reg SMS when ESN is changed
08/02/16   gm      Support for CDMALess Device
08/01/16   ar      Manage channel open gracefully if EF-DIR not present
07/08/16   av      Send SELECT ISDR to determine if card is eUICC or not
07/08/16   rp      F3 reduction
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
06/19/16   bcho    F3 frequency reduction by conditional logic
06/07/16   ar      Removing string prints from APDU and AUTH
05/31/16   lm      Added support to detect multi profile card based on EFs
05/30/16   lm      Determine number of profiles based on card response
05/24/16   ar      Review of macros used by MMGSDI
05/23/16   dd      Skip Search Req for PB files
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/16/16   tkl     F3 log prints cleanup
05/13/16   lm      Remove the extra debug loggings
05/11/16   sp      Reduce F3 logging
05/04/16   sp      Return correct status while validating card session
05/03/16   sp      Reduce F3 log prints
05/02/16   sp      Adding ONCHIP support for KCGPRS files
05/02/16   nr      Don't reject activation req during ecc call for AP centric
04/27/16   vdc     Block select AID only for ARA-M AID
04/11/16   av      Introduce LPA support
04/05/16   vdc     Send act resp and session changed evt at same time
03/26/16   av      Clear ATR related globals at card error/link est
03/14/16   bcho    Override access for auth related GSM files
01/28/16   sp      Don't update the card error cause in RESET AUTO mode
01/06/16   bcho    Don't delete attr cache if not re-cached during NAA Refresh
11/19/15   sp      Reduce F3 logging
10/26/15   av      Adding test MCCs in list which have default MNC as 3 digits
10/16/15   ar      Don't allow reSELECT on AID that failed atleast once earlier
10/16/15   ar      Adding MCCs in list which have default MNC len as 3 digits
10/12/15   vdc     Add support for potential silent file change notification
09/30/15   ar      Fake the fourth byte of EF AD for buggy card
09/25/15   vr      Added support for ERA-GLONASS
09/09/15   av      Pack imsi_m activation status into subscription_ready_evt
09/07/15   av      Migrate to mcfg EFS APIs
08/10/15   vdc     Retry read request for non spec compliant cards
07/27/15   rp      Enhance tlv validation in mmgsdi_util_validate_onchip_config
07/09/15   ar      Validate PRL at diff steps if SD task not ready
06/04/15   lxu     Fix logical channel exhausted before CSIM activation
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
05/15/15   lxu     Fix crash of refresh on EF GBABP
04/17/15   nr      Phonebook record read optimization
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
03/25/15   ar      Ability to process app deact while activation is in progress
03/24/15   kk      Subs prov EFS error handling
12/19/14   bcho    Cleanup session info even if app is not selected yet
12/15/14   kk      Fix MCFG UIM data sent for nv refresh
12/09/14   stv     Fix for subscription switch from SIM to USIM
11/24/14   vv      Send simlock run algo request if pre-perso caching is done
11/14/14   hh      Fix compilation warnings
11/10/14   yt      Return status words of original PIN OP
11/06/14   hh      Call GSTK NV API to read GSTK features in NV 65674
11/05/14   stv     Added traces for status words returned by uim drv in stream apdu
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
10/23/14   yt      Updates to session and app management
10/14/14   am      Compile warnings elimination
10/13/14   ar      Skip CSIM/RUIM App auto-activation when RTRE is NV_ONLY
09/29/14   tkl     Added support for persistent cache
09/09/14   kk      Enhancements to NV handling
09/27/14   ar      Add support for multiple slots in BT-SAP
09/18/14   bcho    Handle QMI UIM SIG as if processed by non-MMGSDI task
09/14/14   hh      Use mmgsdi_file_enum_type for KC and KCGPRS files
09/11/14   hh      Call rex_undef_timer() when timer is no longer needed
08/27/14   yt      Support for restricted SIM access via MMGSDI session APIs
08/27/14   tl      Introduce network related SIM Lock slot policies
08/21/14   am      Add NULL check for pointer dereference
08/14/14   tl      Add support for silent PIN verification
08/11/14   vv      Fixed compiler warnings
08/11/14   bcho    Optimize srv req by using cached srv table in client context
08/08/14   am      Allocate memory only when ref ptr data is greater than 0
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
08/06/14   bcho    Process MMGSDI_EONS_SS_EVENT_REQ in main GSDI task
07/30/14   vv      Added support to indicate app is initialized in recovery
07/28/14   am      Copy client_spec_files file list pointer also during REFRESH
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/15/14   yt      Support for NV refresh
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/30/14   vv      Remove Perso code
06/18/14   av      Fix out of bound access in mmgsdi_util_find_app_pin_index
06/16/14   av      Delete common attributes cache at card error
06/12/14   av      Cache mandatory EFs used by NVRUIM for critical 1x NV reads
06/10/14   tl      Introduce EF 3GPDUPPExt
06/03/14   tl      Remove additional parameter variables
06/02/14   bcho    Block access to CDMA EPRL file
05/28/14   av      Process GET_OPERATOR_NAME in main GSDI task
05/27/14   av      Don't allow access if UST/CST can't be read(pin not verified)
05/15/14   yt      Move path/enum conversion utilities to off-target code
05/12/14   vv      Fix compiler errors
05/12/14   bcho    NULL check added before dereferencing extra_param_ptr
04/29/14   vv      Fixed issue in getting session id for app logic
04/29/14   nmb     Re-enable AT+CRSM after directory maintenance changes
04/28/14   av      Return ACCESS_DENIED when access fails sec check(via EFARR)
04/22/14   yt      Add mmgsdi_util_is_isim_aid()
04/22/14   av      Fix double free when handling icc pin operation response
04/17/14   av      Enhance MMGSDI attributes cache
04/15/14   ar      Skip sending CNF if there is CARD_ERROR while processing
04/09/14   tl      Remove client id from notify info
04/07/14   av      Replace hardcoded tcbs with rex_self where possible
03/28/14   yt      Ensure usage of correct slot id
03/27/14   df      Check receiving task active in mmgsdi_uim_report_synch
03/26/14   tl      SIM Lock secondary revisions
03/25/14   yt      Handle refresh of deactivated app
03/25/14   vdc     Added support to verify PIN after PIN is changed
03/24/14   ar      Remove MMGSDI_SESSION_ENABLE/DISABLE_FDN_REQ
03/18/14   tl      Introduce new SIM Lock feature
03/11/14   bcho    Don't block 3GPP app activation if 3GPP2 app is not present
03/04/14   yt      Check slot id of provisioning session during recovery
02/25/14   yt      Support for selection mode in SELECT AID request
02/25/14   hh      Purge temporary mobile identities during UICC Reset Refresh
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/24/14   ar      Removed wrapper function mmgsdi_util_alloc()
02/24/14   ar      Replace session checks in switch with utility functions
02/19/14   am      Added logic for clearing timers and queues
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
02/10/14   ar      Return MMGSDI_ERROR in get_all_available_apps() if no card
02/10/14   ar      Replace session checks with utility functions
02/10/14   df      Add util check if any non-prov session open
01/28/14   vdc     Optimize read trans req to read from cache before SIM init
12/05/13   tkl     SFI Support
01/22/14   yt      Remove duplicate code
01/22/14   vdc     Populate Get conf. pointer header in case of directory file
01/16/14   df      Add new sim phase enumeration
01/15/14   ar      Cached MMGSDI_CDMA_RUIM_ID in mmgsdi cache
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
01/07/14   ar      Corrected the iterator max value
12/18/13   yt      Skip app termination as part of session deactivation
12/11/13   vv      Added support for the EF-PSISMSC in DF TELECOM
12/11/13   df      Remove unused memory paging feature
12/05/13   ar      Replaced all mmgsdi_malloc direct calls with macro
12/04/13   spo     Do not set signals when MMGSDI task is already stopped
12/01/13   df      Add prov session type to session id util
11/21/13   vv      Halt 3gpp subscription if 3gpp2 perso fails
11/18/13   vv      Added support for parallel REFRESH
11/13/13   yt      Reduce size of MMGSDI tables
11/11/13   df      Fix LLVM compiler error
11/07/13   tl      Remove TO-DO
11/05/13   tkl     Add IMSI check for TDS test SIM
10/27/13   df      Convert use of resp_not_rcvd to uim_rsp_status
10/25/13   df      Remove mutex aquire added in error
10/24/13   av      Fix processing of END_FAILED refresh evt for DEACT and FCN
10/22/13   am      Handling of STOP SIG during SYNCH command processing
10/21/13   yt      Add error checking for command APDUs
10/21/13   df      Ignore sanity timer timeout during sim busy
10/17/13   at      Support for MMGSDI Cancel APIs
10/16/13   df      Removing functions(some unused, some gsdi legacy conversion)
10/08/13   tl      Additional conditions for updating EF RPM VI during initalization
10/07/13   vdc     Correct debug message while retrieving session app info
10/04/13   rp      F3 message reduction
10/03/13   yt      Multi-SIM support for OnChip
10/03/13   yt      Remove duplicate code
10/01/13   av      Reject power-down/up with new error code in ONCHIP mode
10/01/13   vv      Added support for the EF-PWS
10/01/13   vdc     Remove mapping of CSIM/RUIM apps to 1X PRI for off target
09/26/13   av      Enable OnChip on bootup based on EFS item
09/24/13   yt      Remove function mmgsdi_util_is_gsdi_task_context()
09/23/13   vr      qtf-crm MOB migration
09/23/13   yt      Handle error in command queuing to UIMDRV
09/23/13   df      Support for SIM busy
09/05/13   yt      Remove usage of rand() to generate client and session IDs
09/03/13   yt      Replace MMGSDIUTIL_MAXVAL_INT32 with MMGSDI_INT32_MAX
09/03/13   tl      Reduce On-chip SIM activation delay between stages
09/01/13   av      Populate client id when preparing immediate read response
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/29/13   av      Dont return to caller if read cache API returns NOT_FOUND
08/28/13   yt      Deprecate VIRTUIM/VIRTSAT module
08/26/13   am      Fix MMGSDI Sanity timer expiry processing
08/20/13   spo     Automatic Subscription provisioning enhancements in UIM
08/14/13   av      Optimize read transparent req by reading from cache directly
08/08/13   vdc     Remove mapping of CSIM/RUIM apps on slot 2/3 to 1X PRI
08/08/13   vdc     Remove mapping of CSIM/RUIM apps on slot 2/3 to 1X PRI
08/07/13   tl      Add CDMA AKA Authenticate functionality
08/05/13   yt      Block commands to UIM if On-Chip is activated
07/15/13   vs      EONS updates to support single baseband SGLTE
07/15/13   ssr     Fixed 62XX status byte for APP selection through
                   QMI_UIM_LOGICAL_CHANNE
07/15/13   av      Introduction of new NULL_BYTES card error type
07/09/13   vdc     Fix and code optimization for get available apps
07/09/13   vdc     Fix for get all PIN status not working for other than slot1
07/03/13   df      Remove potential NULL pointer de-references
07/01/13   at      Support for additional info in EONS APIs
06/28/13   bcho    APIs added to return plmn info from SE13 table.
06/28/13   vdc     When closing non-prov sessions, compare App types & AIDs
06/27/13   spo     Added support for files under DF TELECOM
06/25/13   av      Add SF EUIMID to 1x cache list
06/24/13   vdc     Added support for triple SIM
06/20/13   av      Update READ security attrib of USIM_RAT as ALLOWED(per spec)
06/18/13   vv      If EHPLMN srv is not present, allow access to ActingHPLMN
06/18/13   vv      Fix copying the entire path
06/14/13   vdc     Added utility function to get app index from the app
06/12/13   yt      Fix compiler warnings
05/28/13   spo     Replaced instances of memcpy with memscpy
05/21/13   vv      Added support for session closed cause
05/21/13   av      Assume security attributes per spec if ARR read is skipped
05/20/13   av      Halt 3gpp2 subscription of 3gpp perso fails
05/20/13   vv      Added support to check if a logical channel can be re-used
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
05/13/13   at      Fix for integer overflow while parsing refresh file list
05/10/13   at      Handle QMI Modem task SIG during sync commands to UIM Driver
05/10/13   vv      Reject access to ActingHPLMN if EHPLMN srv is present
05/09/13   vv      Added perso state check to evaluate app activation status
05/07/13   yt      Do not return AID index for SIM/RUIM app type
05/06/13   spo     Reduce memory used by client request table
05/06/13   spo     Modified CPHS variables to array of pointers
04/29/13   at      Handle QMI UIM SIG during sync commands to UIM Driver
04/26/13   kb      Check of session type to 1X secondary in verifying hidden key
04/25/13   vv      Process Card Slot Session commands in slot specific tasks
04/24/13   vv      Reset CSG eons info in the app. info
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
04/11/13   vv      Re-design handling of VPE response from the card
04/04/13   bcho    Deacticate provisioning session if GET_ALL_PIN_STATUS fails
04/01/13   spo     Do not send session events before session open CNF is sent
03/31/13   tl      Fix compiler warning and remove redundancy
03/21/13   av      Fix logical channel reserving logic to handle multiple CSIMs
03/13/13   kb      Add utility function to map an input Session ID to MMGSDI
                   Session ID
03/08/13   vv      Allow EFs outside ADF to be accessed before app. selection
03/08/13   tl      Allow response block len of size 0 for Configuration Request
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
02/25/13   vv      Added support to save AID received in SELECT response
01/23/13   vv      Added EF-NASCONFIG to post pin1 cache list
01/20/13   vv      Added support to update ACSGL record
01/20/13   vv      Added EONS support for the CSG
01/14/13   vv      Added ERR_FATAL in case task is undetermined
01/08/13   vv      Fix to retrieve recovery data for each slot supported
01/07/13   vv      Check the return value when queuing a slot specific command
12/30/12   kk      Fixed async TP for few other scenarios
12/30/12   kk      Reworked AsyncTP for faulty RUIM cards having no GSM info
12/17/12   tl      Check EST dependent services bits for EST availability
12/15/12   bcho    Set select rsp type to RSP NO DATA in card session cleanup
12/12/12   vv      Added support to populate the user data with current task
12/12/12   av      Fixed compiler critical warnings
12/11/12   tl      Added max length check for responses with variable data size
12/05/12   vv      Added support for parallel processing in MMGSDI
11/12/12   bcho    Added function to get length of MNC in IMSI
10/21/12   av      Update conf file only if necessary
10/21/12   av      Allow cyclic increase by default
10/15/12   bcho    Handle SS event/get operator name API call in No Card and/or
                   Limited Service scenario
10/13/12   abg     Clear common file cache when resetting slot data
10/11/12   spo     Removed unnecessary F3 messages
10/09/12   tl      Change static data length copy to dynamic data length
10/03/12   abg     Removed Queuing related F3 message
10/01/12   vs      Added utility for context check including logic for SGLTE
10/01/12   abg     Removed Problem determining protocol type F3 message
09/28/12   abg     Updated ERR to MSG_ERROR
09/25/12   av      Skip reading EF-ARR for internal selects
09/25/12   abg     Fixed Hidden key file reading for CSIM and Updated check
                   on app_info_ptr while handling Hidden key confirmation
09/20/12   bcho    Added support for mmgsdi_get_SE13_plmn_names API
09/19/12   vv      Reserve logical channel for CSIM app
09/11/12   av      Handling of VPE response from the card
09/11/12   bcho    Support for ignore display condition param in
                   MMGSDI_SESSION_GET_OPERATOR_NAME_REQ handling
09/03/12   bcho    OPL-PNN cache freed in mmgsdi_util_preinit_mmgsdi()
07/30/12   yt      Check if app is shareable across channels
07/25/12   bcho    Add support for RECOVERY_IND_REQ
07/19/12   tl      Support more application and logical channels
07/13/12   vv      Added support for the CSG EFs
07/13/12   vv      Prevent multiple selects for invalid uim file
07/13/12   vv      Added support to identify apps not in EF-DIR
06/07/12   bcho    Memory alignment done for MMGSDI_EONS_SS_EVENT_REQ
05/23/12   vv      Added support for file update registration notification
05/22/12   shr     Add support for variable length Send APDU response
04/30/12   shr     Do not clear ATR when resetting slot info
04/20/12   av      Enabled detouring of nv/efs calls to offtarget framework
04/16/12   tl      Added support for client name traces
04/10/12   nmb     Revert multiple select fix
04/02/12   tl      Match entire RID and 3G App Code
04/02/12   bcho    In mmgsdi_util_alloc_and_populate_extra_param() allocate memory to
                   plmn list pointer only when data length > 0
03/29/12   shr     Added support for Session open with MF,
                   enhancements to file access request methods,
                   support for non-prov. app selection using zero length and
                   partial AID, support for Send APDU extension API
03/28/12   bcho    Function added to replace rex_sleep() with sleep mechanism
                   based on non-deferrable timer
03/27/12   nmb     Prevent multiple selects for invalid uim file
03/21/12   yt      Add request type for session_open_with_select_rsp
03/21/12   bcho    MMGSDI_APP_STATE_READY_NO_SUB_EVT replaced with
                   MMGSDI_APP_STATE_EXT_PERSO_FAILED
03/16/12   yt      Modify references for gsdi_task_tcb
03/12/12   vv      Fixed klocwork errors
03/05/12   bcho    Macro used to check critical section acquisition
03/01/12   bcho    Unused function mmgsdi_util_get_channel_index removed
02/23/12   av      Cleaned up remaining always ON features
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
02/22/12   vv      Send PIN events in case of a decrease in PIN retries count
01/30/12   bcho    Handling of MMGSDI_EONS_SS_EVENT_REQ added
                   mmgsdi_util_app_eons_cleanup() and
                   mmgsdi_util_clear_opl_pnn_caches() modified
01/17/12   kk      Do not allow external cache updates for specific EFs when
                   cache is empty
01/16/12   nmb     Removal of ALWAYS ON FEATURE_AUTH
01/09/12   at      Check for input length in mmgsdi_util_cal_align_size
12/23/11   shr     Removed featurization for ALWAYS ON features
12/22/11   bcho    To handle UIM_TECHNICAL_PROBLEM_S in
                   mmgsdi_util_convert_from_uim_error_condition()
12/21/11   kk      Legacy GSDI removal updates
12/21/11   kk      Added USIM ECC caching support
12/21/11   bcho    Changes for new API to open multiple sessions
                   in a single request
12/21/11   bcho    Added support for Get Operator Name API
11/15/11   nmb     RPM Updates
10/31/11   hn      Remove featurization of code based on feature TEST_FRAMEWORK
10/28/11   yt      Added new files for cache-related utility functions
10/18/11   shr     Keep card sessions mapped to a provisioning application
                   on channel 0 when atleast one such application is active
10/19/11   js      Support to select ADF before PIN2 operation
10/13/11   shr     Adding support for full recovery
09/23/11   nb      Async TP enhancement
09/16/11   av      Replace feature #ifdefs with an NVITEM
09/13/11   av      Fixes for KW warnings
09/12/11   vv      Support to deselect app upon error in activation
08/18/11   av      Fixes for MMGSDI Klocwork CRITICAL warnings
08/11/11   nmb     Expanded Logical Channels
08/09/11   tkl     Support for TMSI PLMN check
08/05/11   vv      Fixed assignment of missing pin info type members
08/01/11   vs      Updates for key ref based PIN operations
08/01/11   vs      Added support for session open with select response
07/22/11   vv      Fixed assignment of incorrect cache table pointer
06/21/11   kk      ThreadSafe updates from review
05/26/11   tkl     Override access for special USIM ARR
05/26/11   ms      Support for diag nv item is necessary only on target
05/20/11   yt      Added support for INCREASE request
05/17/11   kk      Updates for Thread safety
05/17/11   shr     Updates for Thread safety
05/04/11   ms      Send the PERSO_EVT to the late clients in perso state
                   as WAIT_FOR_DEPERSO_S and INIT_ERROR_S
05/11/11   nb      Support for Async TP
05/05/11   ms      Adding Support for diag nv item
04/29/11   nk      Fix KW Errors
04/26/11   vs      Support for subscription ok request
04/21/11   nmb     Added RPM Files for USIM EF path lookups
04/14/11   yt      Updated logic for security access condition check for EFs
04/05/11   ea      Added mmgsdi_util_determine_app_type_from_aid()
04/01/11   yt      Verify validity of path for file access
03/31/11   yt      Fixed compilation warning
03/16/11   kk      Allow access to card, if UPIN status is not available
03/11/11   ms      Clear the app capabilities in session deactivation
02/21/11   ms      Implementation of Session based caching on App capabilities
02/18/11   yt      Check if CSIM is provisioned before starting Refresh voting
02/11/11   ssr     Clear RUIM and GSDI cache after successful OTASP commit
02/03/11   ms      Updated F3 messages for Client ID
02/01/11   ssr     Add extra_param_ptr NULL check for pin request
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
01/31/11   ssr     Added file access check
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/18/10   yt      Fixed data length check utility
11/05/10   yt      Klocwork fixes
11/03/10   ms      Changes for enablement of C+G
10/29/10   tkl     Returned original mmgsdi_err_status in generic rsp
10/27/10   shr     Fixed alignment of switch statement
10/27/10   shr     If file security attributes indicates ARR record number
                   as 0, do not send read request to the card
10/25/10   shr     Fixed input path/AID length check when copying file
                   access information
10/21/10   shr     Cleanup of Onchip USIM support
10/07/10   shr     File path of depth greater than 5 file IDs needs
                   to be rejected when handling REFRESH file registration
                   by path/REFESH FCN
10/12/10   yt      Updated type when populating extra_param for PKCS15 request
10/12/10   yt      Dynamic allocation for mmgsdi_client_req_table_info_ptr
10/05/10   nmb     Removed extra bytes from task_cmd buffer
09/29/10   yt      Enabling support for INCREASE
09/27/10   vs      Send sync cmd to UIM only when in GSDI context
09/21/10   nmb     Fix mmgsdi_uim_file_enum_table service mappings
09/15/10   nmb     Allocate mmgsdi_client_id_reg_table dynamically
09/14/10   kk      Add critical section around attributes cache access
09/13/10   yt      Populate data_len field of extra_param for read requests
09/13/10   shr     Stop queueing and processing commands once TASK_STOP
                   is received
09/08/10   nmb     mmgsdi_task_cmd_type memory reduction
09/07/10   nmb     Fix to check service 85 before EPS EF reads
09/06/10   nb      Generate FDN event when FDN status changes
08/27/10   adp     Fixing Compiler Warning
08/23/10   yb      Reverting back the check added for MF identifier for 3F00
                   and remvoing data_len field from mmgsdi_read_extra_info_type
                   as it leads to authentication error.
08/18/10   yt      Fixed Klocwork errors
08/12/10   ssr     Fixed nvruim 1x primary session for slot2
08/10/10   adp     When we check the lookup table, check for the exact length
                   of the path.
08/09/10   adp     When mmgsdi_util_determine_uim_items_enum fails, return
                   the appropriate mgsdi_status
07/28/10   yt      Check MF address before sending path to UIM and added
                   data_len field to mmgsdi_read_extra_info_type
07/21/10   adp     Adding mmgsdi_util_check_lookup_table function
07/14/10   js      Fixed compiler warnings
07/09/10   tkl     Fixed enum mapping between mmgsdi_perso_feature_enum_type
                   & gsdi_perso_enum_type
07/02/10   nmb     Updates for JCDMA CSIM support
07/02/10   ssr     Fixed refresh for ICC card which have both GSM and RUIM
06/25/10   shr     Clean-up of Onchip support
06/10/10   shr     Allow close of Card Sessions
06/10/10   vs      Added file path check before sending down select
06/09/10   vs      Moved USIM sub to default channel
06/06/10   kk      Fixed uim get enum from path util function
06/02/10   ssr     Fixed Lint errors
06/01/10   js      ZI memory reduction fixes
05/31/10   ssr     Fixed get all PIN status where APPS does not have PIN2
05/25/10   shr     Clean-up file access operations on Card Sessions
05/17/10   nmb     Added pin length to mmgsdi_util_is_pin_valid calls
05/13/10   ssr     Fixed compilation error
05/12/10   ssr     Fixed automatic slot support for 1x apps
05/12/10   shr     File access operations need to be allowed on Card Sessions
                   irrespective of the provisioning status
05/05/10   shr     Need to mark Hiddenkey as invalid if there any
                   non 'F' digits after an 'F' is encountered
05/04/10   shr     Removed temporary Feature Flags
05/04/10   shr     Clear File cache, Record cache and File attributes cache
                   when deactivating a provisioning application
05/03/10   shr     Added Dual Slot support for REFRESH RESET and REFRESH
                   RESET_AUTO
04/27/10   shr     Added Hiddenkey support
04/23/10   yt      Fixed KW errors
04/20/10   shr     Default provisioning is decided only on the basis of
                   NV_UIM_SELECT_DEFAULT_USIM_APP_I status. If a
                   Session is already active, do not attempt activation
04/13/10   ssr     Fixed get ruim capability
04/16/10   yt      Fixed compilation warnings
04/06/10   shr     Clean up REFRESH RESET and REFRESH RESET_AUTO
04/07/10   mib     Added support for KEYS in onchip configuration
04/07/10   nb      SAP Enhancements
03/29/10   shr     Switch/deactivate provisioning clean up and redesign
03/23/10   kp      Revert the KW changes
03/18/10   kp      Fix for the KW errors
03/12/10   kk      Fixed session release handling
03/12/10   adp     Fixing KW errors
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
03/02/10   jk      Added MMSS files
03/02/10   kk      Fixed get all available apps command handling
02/19/10   shr     Clear File cache, Record cache and File attributes cache
                   while provisioning a new application
02/17/10   shr     Fixed Session activate/deactivate handling
02/16/10   kk      Removed "is default app" check for read/write cache
02/16/10   kk      Added support for slot specific common cache
02/09/10   vs      Fixed session clean up logic
02/09/10   mib     Fixed refresh list parsing when number of files is wrong
02/08/10   ssr     Fixed MS Key Request for Parameter P or G
02/05/10   ssr     Fixed get session pointer
02/01/10   jk      EPRL Support
01/25/10   shr     Fixed incorrect Session cleanup
01/15/10   shr     Fixed incorrect access of freed memory locations
01/06/10   shr     Fixed Session Close handling
12/18/09   nb      Moved Dual Slot Changes
12/16/09   rn      DF enum access support for mmgsdi_util_access_path_lookup_table
12/16/09   rm      Workaround for a recovery issue
12/14/09   ssr     Removed MMGSDI_SELECT_AID_REQ
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
12/10/09   nb      Fixed compilation errors
12/07/09   js      Fixed Lint Errors
12/07/09   js      Added support for MMGSDI_CSIM_ARR file
12/07/09   mib     Added RPLMNACT in onchip configuration
12/04/09   kp      Replacing mmgsdi_data_slot2 with mmgsdi_data_slot2_ptr
12/03/09   shr     Cache read process enhancement
12/02/09   js      Fixed session open issue for non-prov type sessions with
                   exclusive channel
12/02/09   js      Fixes for RUIM power up
10/28/09   mib     Added support for additional files
10/28/09   nb      Release 7 Enhanced Network Selection
10/21/09   kk      Fixed klocwork error - Possible Memory leak
10/16/09   js      Fixed cache write logic while allocating memory to
                   additional data ptr
10/12/09   kp      Moving Read-Write varibles to Heap
10/07/09   mib     Removed dependency between UIM table and mmgsdi enum
09/26/09   kp      ZI memory reduction changes
09/16/09   js      Fixed KW Errors
09/08/09   rn      Fixed klocwork errors
09/02/09   kk      Fixed Lint Err Boolean within 'if' always evaluates to False
08/20/09   mib     Added support for additional files in DF-HNB and DF-WLAN
07/27/09   kk      Fixed Lint Error - Boolean within 'if' always evaluates
                   to False
07/21/09   shr     Fixed compilation warnings
07/20/09   mib     Added support for additional LTE files and EF-UST extension
                   Replaced flag TEST_FRAMEWORK with FEATURE_UIM_TEST_FRAMEWORK
07/08/09   shr     Fixed Lint warnings
07/06/09   shr     Fixed Lint warnings
07/03/09   shr     Fixed Lint warnings for shift left of signed quantity
07/02/09   ssr     Fixed error freeing perso_get_data menory in
                   PERSO_GET_FEATURE_IND case
06/25/09   shr     Fixed Lint warnings
06/23/09   rn      Fixed Lint errors
05/05/09   js      Fixed warnings
04/29/09   js      Added prototype for static functions, fixed compiler warning
04/17/09   mib     client id not passed in events to applications
04/17/09   vs      Fix to allow clients to receive all streaming APDU responses
04/06/09   mib     Added support for onchip USIM authenticate
03/26/09   mib     Integration of support for ONCHIP USIM
03/23/09   sun     Added support for virtuim in QTF
03/19/09   js      Support for Enhanced NAA Refresh by file path
03/18/09   yb      Fixed Lint Errors for using free variable
03/10/09   adp     Lint Error 58: Bad type
02/24/09   nb      Perso Command support in MMGSDI
01/27/09   kk      Fixed featurization, which is causing compilation warning
01/21/09   kk      Added support for converting file path to mmgsdi file enum
                   and featurize ef access condition check.
01/20/09   vs      Fixed KW error
12/17/08   nb      Common Modem Interface
11/19/08   js      Moved definition of MMGSDIUTIL_WORD_SIZE to mmgsdiutil.h
11/13/08   nk      Added MSG in mmgsdi_util_pkcs15_free_client_table_and_build_cnf
                   to fix compiler Warning
11/05/08   tml     Fixed get session id utility function issue
10/22/08   kk      Extra check to handle invalid file path str tag in PKCS15
                   and fixed some lint errors
10/06/08   jk      Allow GSM SIM access in 1X only
10/07/08   kk      Fixed handling of invalid tags and missing tags in create
                   pkcs15 table
09/30/08   kk      Fixed incorrect boundry check on EF-DIR data during
                   PKCS15 table creation
09/24/08   js      Klockwork fix for potential null ptr dereferencing
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/20/08   jk      Support for Generate Key API and parsing of RUN CAVE
08/06/08   tml     Fixed Klocwork issues
08/08/08   vs      Fixed compile errors related to featurization
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. MAX path length in MMGSDI is set to 5.
07/10/08   sp      Add file type to extra param in read
07/10/08   sp      Add file type to extra param in read
07/17/08   sun     Fixed Parsing of response for Run Cave
07/17/08   kk      Added support for PKCS 15
07/16/08   jk      Fixed Compile Warnings
07/07/08   sp      Remove message
06/12/08   srr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/23/08   sp      Don't clean up the global that was populated as a result
                   of EF-DIR aid read during the reselection of an app.
06/21/08   kk      Added PKCS15 Support
06/04/08   tml     Added sw to cnf header and added additional debug message
04/30/08   kk      Added support for MMS EFs
04/07/08   jar     Fixed Compile Error introduced by Changelist 634222
03/28/08   sun     Added support for OMA BCAST
03/20/08   nk      Added OMH EF Support
03/03/08   nk      Updated call to mmgsdi_uim_icc_select
02/28/08   tml     Fixed MKEY parsing response parsing issue
02/26/08   jk      Allow POWER DOWN command to go onto UIM even when repeated reset
                   is happening
01/10/08   nk      Fixed Lint Error
01/02/08   jk      Insert checks and mechanism to withdraw command from uim_cmd_q
                   before deallocating command buffer
12/10/07   sun     Verify PIN Internally when SAP is Disconnected
12/06/07   sun     Removed app type comparison while comparing aids
11/19/07   js      Radio Access Technology Balancing support
11/15/07   sun     Added support for NAA refresh
11/08/07   vs      Added OMH feature support
10/02/07   tml     Added proprietary app session support
09/27/07   tml     Lint fixes
09/10/07   tml     Add MFLO supports
08/15/07   jk      Changes to support MMGSDI DATA Authentication
08/14/07   sun     Fixed Buff Offset Calculation
07/16/07   sp      Fixed the size of memcpy in
                   mmgsdi_util_get_file_path_from_enum
07/02/07   sun     Added Support for ODD Ins Class
06/28/07   sun     Removed checks for uim_cmd_ptr
06/26/07   sp      Fix compiler error
06/25/07   sp      Convert static variables to pointers to avoid stack overflow
06/18/07   sun     UIM CMD Ptr should not be null - added error messages
06/05/07   nk      Fixed jcdma queueing of mmgsdi uim report rsp
05/31/07   sp      Fixed possible data abort.
05/23/07   nk      Added default value for esn_me_change_flag
05/21/07   jk      Check for sanity timer expiry
05/22/07   sun     Free the mgs ptr allocated in gsdilib.c
05/21/07   wli     support JCDMA RUIM feature
05/18/07   sp      Do not check the no of apps present in icc protocol
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
04/26/07   sp      Added functioon to get path from enum
04/20/07   sun     Added additional cases for MMGSDI-GSDI Status conversion
04/17/07   sp      Featurize for LTK
04/12/07   sun     Featurized Phonebook
04/11/09   sp      Fixed potential buffer overflow
04/06/07   sun     Send information about all the apps in the CARD_INIT_
                   COMPLETED event
04/04/07   sun     Added Flag to check if there are actually any files being
                   refreshed
03/28/07   sun     Check if additional files are being read during refresh
                   fcn
03/28/07   tml     Fixed compilation error
03/26/07   tml     Add Onchip memory alignment
03/19/07   sun     Fixed Lint Error
03/02/07   sun     Update App PIN with correct PS_DO information relevant to
                   that app only
03/05/07   nk      Add PIN validation function
03/02/07   sun     Added function to check for file attributes
02/21/07   sun     Removed Global PIN from GSDI
02/27/07   jk      Added featurization for Memory Reduction
02/09/07   sk      Added support for 1000 phonebook entries.
02/01/07   tml     Fixed mmgsdi state for SIM_INIT refresh and null pointer
                   check
02/01/07   jk      Added check for null pointer
01/24/07   sun     Do not set mmgsdi cache based on gsdi cache. Let MMGSDI set
                   the cache after all the MMGSDI PINs have been cached.
01/15/07   sp      Fix error with ThinUI builds.
01/11/07   sk      Return MMGSDI_NOT_FOUND if file is not init in path lookup
                   table.
01/15/07   sun     Return the number of unblock retries if the command is
                   to unblock the pin
01/11/07   sun     Fixed mapping of pin status
01/11/07   sun     Fixed Compilation
01/08/07   sun     Get the AppId from the Client ID for All Pin Operations
12/19/06   tml     Add support for path transparent access, send pin events
                   at select_aid state if possible
12/27/06   sp      Back out pin event changes.
12/13/06   tml     Do not check against empty aid info from the global
                   app_info_ptr
12/12/06   tml     Added indication whether the get_file_attribute will result
                   in activation or termination of app
12/11/06   tml     Ensure proper event notification for late registered clients
11/14/06   sp      Fixed lint error
10/26/06   tml     Get Key ref from client ID
11/07/06   sun     Fixed Lint Error
11/06/06   sun     Removed Featurization
10/25/06   tml     Added initial support for Universal PIN replacement support
10/25/06   tml     Added function to check if the client is trying to access
                   selected application
10/17/06   sun     Do not wait for Card Selection if NV item is set
09/06/06   tml     Added support for refresh AID notification
08/25/06   jar     Added support for FEATURE_MMGSDI_CARD_ERROR_INFO
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/20/06   jar     Added support for FEATURE_MMGSDI_CARD_ERROR_INFO
08/18/06   sun     Send the original PIN Operation request when building a
                   new pin request
08/16/06   jar     Modifed mmgsdi_uim_file_enum_table to include the mapping
                   of MMGSDI_TELECOM_ANRC and MMGSDI_TELECOM_ANRC1.
08/07/06   tml     Added ACL support
08/07/06   sun     Divert GSDI Pin calls to MMGSDI
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/29/06   sun     Lint Fixes
06/12/06   sun     Set mmgsdi_sap_state  whenever mmgsdi_state is updated
06/21/06   pv      Do not send commands to UIM when UIM has already notified
                   an error.
06/15/06   tml     Fixed compilation warning
06/14/06   tml     Fixed compilation issue
06/12/06   pv      Add code to handle MMGSDI_SESSION_CLOSE_EVT and
                   call mmgsdi_recover_after_reset while not in recovery,
                   call uim_recovery_cmd to send commands to uim while in
                   recovery mode.  Use MMGSDI_SYNCH_UIM_REPORT_SIG instead of
                   GSDI_UIM_REPORT_SIG.
06/12/06   tml     lint
05/23/06   tml     lint and BDN, rehablitate and invalidate support
05/16/06   tml     Added utility to see if client reg table is free or not
05/09/06   tml     Fix memory overwrite and access potential  null pointer
05/01/06   tml     Ensure client ID will not be zero
04/14/06   tml     Added utility function to initialize cache data
                   and set init flag
04/13/06   sun     FOR SAP, Allow UIM FAIL responses also to go BT Client
04/13/06   jar     Added ONCHIP SIM Support
04/05/06   tml     Add NO FILE ENUM and Card Removed support
03/21/06   tml     added cfis and missing service table support and lint
03/03/06   tml     Added support for CACHE_SEL
02/09/06   tml     Added utility functions for conversion and lint fix
02/16/06   sp      Support for EF_EHPLMN
02/14/06   nk      Merged for ARR caching and by path changes
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/28/05   tml     PNN and OPL support
11/14/05    pv     change signature for functions that needs lookup
                   to return mmgsdi_return_enum_type and accept an
                   output parameter.
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header and cache read/write
11/03/05   sun     Added support for Refresh
10/28/05   sun     Added SAP Report Rsp case for memory alignment
10/25/05   tml     Added ISIM File supports
10/18/05   tml     Added in_synch info to the cache
09/29/05   tml     Client dereg supports
08/30/05   jar     Added utility functions and support for sending Card Status
                   Information.
08/26/05   sun     Added support for BT SAP
08/25/05   pv      Support API to power down and power up SIM/USIM Card
08/17/05   pv      added mmgsdi_util_print_data_to_diag function to print
                   data to the diag interface
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/18/05   jk      Fixed Compile Issues for 1x Targets
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version
02/26/05   sst     Added MMGSDIBT support

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi.h"
#include "mmgsdilib.h"
#include "mmgsdicache.h"
#include "uim.h"
#include "mmgsdi_session.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_card_init.h"
#include "fs_public.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_eons_nitz.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_refresh_rsp.h"
#include "mmgsdi_csg.h"
#include "mmgsdisessionlib_v.h"
#include "mmgsdi_uim_common.h"
#include "mmgsdi_file.h"
#include "mmgsdi_silent_pin.h"
#include "mmgsdi_multi_profile.h"
#ifdef FEATURE_SIMLOCK
#include "mmgsdi_simlock.h"
#endif /* FEATURE_SIMLOCK */
#include "mmgsdi_ss_event.h"

#include "bit.h"

#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
#include "qmi_sim_internal.h"
#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define MMGSDIUTIL_IMSI_MNC_POS               6
#define MMGSDIUTIL_IMSI_MCC_FIRST_BYTE_POS    8
#define MMGSDIUTIL_IMSI_MCC_SEC_BYTE_POS      9
#define MMGSDIUTIL_IMSI_MCC_CUST_DEFAULT    359   /* 460 */
#define MMGSDIUTIL_IMSI_MNC_CUST_DEFAULT     92   /* 03 */

/*===========================================================================
         S T A T I C    F U N C T I O N   P R O T O T Y P E S
============================================================================*/
static void mmgsdi_util_find_first_ruim_csim_app(
  mmgsdi_slot_id_enum_type  slot,
  boolean                  *app_1x_found_ptr,
  mmgsdi_data_type         *app_1x_data_ptr);

static void mmgsdi_util_find_first_sim_usim_app(
  mmgsdi_slot_id_enum_type  slot,
  boolean                  *app_gw_found_ptr,
  mmgsdi_data_type         *app_gw_data_ptr);

static mmgsdi_return_enum_type mmgsdi_util_icc_update_card_session_info(
  mmgsdi_app_enum_type     app_type,
  mmgsdi_slot_id_enum_type slot_id);

static void mmgsdi_util_reset_pin2_hiddenkey_status(
  mmgsdi_int_app_info_type *sel_app_ptr);

static boolean mmgsdi_util_check_est_available(
  mmgsdi_client_id_type           client_id,
  mmgsdi_session_id_type          session_id,
  mmgsdi_file_enum_type           file_name);


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_PRINT_CLIENT_NAME

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
)
{
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_SUCCESS;
  int32                     client_name_index  = MMGSDI_MAX_CLIENT_INFO;

  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id,
                                                  &client_name_index);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (client_name_index >= MMGSDI_MAX_CLIENT_INFO) ||
     (client_name_index < 0))
  {
    return;
  }

  if(mmgsdi_client_id_reg_table[client_name_index] != NULL)
  {
    if (mmgsdi_client_id_reg_table[client_name_index]->client_name[0] != '\0')
    {
    MSG_SPRINTF_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "client name = %s",
                     mmgsdi_client_id_reg_table[client_name_index]->client_name);
    }
  }
} /* mmgsdi_util_print_client_name */


/* ==========================================================================
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
==========================================================================*/
mmgsdi_protocol_enum_type mmgsdi_util_determine_protocol(
  mmgsdi_slot_id_enum_type      slot_id
)
{
  mmgsdi_slot_data_type        *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_NO_PROTOCOL;
  }

  /* Check if the protocol is valid */
  if((slot_data_ptr->protocol == MMGSDI_ICC) ||
     (slot_data_ptr->protocol == MMGSDI_UICC))
  {
    return slot_data_ptr->protocol;
  }

  return MMGSDI_NO_PROTOCOL;
}/* mmgsdi_util_determine_protocol */


/* ==========================================================================
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

==========================================================================*/
void mmgsdi_util_free_response_ptr(mmgsdi_cnf_type * response_ptr)
{
  mmgsdi_cnf_enum_type              cnf_type = MMGSDI_MAX_CNF_ENUM;
  mmgsdi_file_security_access_type *security_access_ptr = NULL;
  if (response_ptr != NULL)
  {
    cnf_type = response_ptr->response_header.response_type;
    switch(cnf_type)
    {
      case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      case MMGSDI_CLIENT_ID_AND_EVT_DEREG_CNF:
      case MMGSDI_SESSION_OPEN_CNF:
      case MMGSDI_SESSION_OPEN_WITH_MF_CNF:
      case MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF:
      case MMGSDI_SESSION_CLOSE_CNF:
      case MMGSDI_SESSION_DEACTIVATE_CNF:
      case MMGSDI_SESSION_APP_RESET_DEACTIVATE_CNF:
      case MMGSDI_SESSION_APP_RESET_ACTIVATE_CNF:
      case MMGSDI_SESSION_REGISTER_FILE_UPDATE_CNF:
      case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_CNF:
      case MMGSDI_WRITE_CNF:
      case MMGSDI_SET_BUILTIN_PLMN_LIST_CNF:
      case MMGSDI_GET_SIM_PROFILE_CNF:
      case MMGSDI_SET_SIM_PROFILE_CNF:
        break;

      case MMGSDI_READ_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->read_cnf.read_data.data_ptr);
        break;

      case MMGSDI_INCREASE_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->increase_cnf.increased_data.data_ptr);
        break;

      case MMGSDI_GET_FILE_ATTR_CNF:
        switch(response_ptr->get_file_attr_cnf.file_attrib.file_type)
        {
          case MMGSDI_LINEAR_FIXED_FILE:
            security_access_ptr =
              &response_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security;
            break;

          case MMGSDI_CYCLIC_FILE:
            security_access_ptr =
              &response_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security;
            break;

          case MMGSDI_TRANSPARENT_FILE:
            security_access_ptr =
              &response_ptr->get_file_attr_cnf.file_attrib.file_info.transparent_file.file_security;
            break;

          case MMGSDI_MASTER_FILE:
          case MMGSDI_DEDICATED_FILE:
          case MMGSDI_MAX_FILE_STRUCTURE_ENUM:
            break;
        }
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->get_file_attr_cnf.raw_file_attr_data.data_ptr);
        if (security_access_ptr == NULL)
          return;
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          security_access_ptr->read.protection_pin_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          security_access_ptr->write.protection_pin_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          security_access_ptr->increase.protection_pin_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          security_access_ptr->invalidate_deactivate.protection_pin_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          security_access_ptr->rehabilitate_activate.protection_pin_ptr);
        break;

      case MMGSDI_ISIM_AUTH_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->isim_auth_cnf.auts.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->isim_auth_cnf.ck.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->isim_auth_cnf.ik.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->isim_auth_cnf.res.data_ptr);
        break;

      case MMGSDI_SAP_CONNECT_CNF:
        break;
      case MMGSDI_SAP_DISCONNECT_CNF:
        break;
      case MMGSDI_SAP_GET_ATR_CNF:
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            response_ptr->sap_get_atr_cnf.atr_data.data_ptr);
        break;
      case MMGSDI_SEND_APDU_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->send_apdu_cnf.apdu_data.data_ptr);
        break;
      case MMGSDI_SEND_APDU_EXT_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->send_apdu_ext_cnf.apdu_data.data_ptr);
        break;
      case MMGSDI_SAP_SEND_APDU_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->sap_send_apdu_cnf.apdu_data.data_ptr);
        break;
      case MMGSDI_CARD_STATUS_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->status_cnf.status_data.data_ptr);
        break;
      case MMGSDI_REFRESH_CNF:
      case MMGSDI_CARD_RESET_CNF:
        break;
      case MMGSDI_SAP_POWER_OFF_CNF:
        break;
      case MMGSDI_COMPUTE_IP_AUTH_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->compute_ip_cnf.cmpt_ip_response_data.data_ptr);
        break;
      case MMGSDI_USIM_AUTH_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->usim_auth_cnf.auth_response.data_ptr);
        break;
      case MMGSDI_CDMA_AKA_CNF:
        if (response_ptr->cdma_aka_cnf.sync_fail_tag == MMGSDI_CDMA_AKA_VALID_SEQ_NUM)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            response_ptr->cdma_aka_cnf.aka_data.aka_keys.res.data_ptr);
        }
        break;
      case MMGSDI_GENERATE_KEY_VPM_CNF:
#ifdef FEATURE_MMGSDI_3GPP2
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->generate_key_vpm_cnf.octet_data.data_ptr);
#endif /* FEATURE_MMGSDI_3GPP2 */
        break;
      case MMGSDI_RUN_CAVE_CNF:
      case MMGSDI_SAP_RESET_CNF:
      case MMGSDI_SAP_POWER_ON_CNF:
      case MMGSDI_CARD_PDOWN_CNF:
      case MMGSDI_CARD_PUP_CNF:
      case MMGSDI_ACTIVATE_ONCHIP_SIM_CNF:
      case MMGSDI_SESSION_ENABLE_FDN_CNF:
      case MMGSDI_SESSION_DISABLE_FDN_CNF:
      case MMGSDI_REHABILITATE_CNF:
      case MMGSDI_INVALIDATE_CNF:
      case MMGSDI_PIN_OPERATION_CNF:
      case MMGSDI_GET_ALL_PIN_STATUS_CNF:
      case MMGSDI_SRV_AVAILABLE_CNF:
      case MMGSDI_JCDMA_GET_CARD_INFO_CNF:
        break;
      case MMGSDI_GET_ATR_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->get_atr_cnf.atr_data.data_ptr);
        break;
      case MMGSDI_BCAST_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->bcast_cnf.bcast_response.data_ptr);
        break;

      case MMGSDI_SEARCH_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(response_ptr->search_cnf.searched_record_nums.data_ptr);
        break;

      case MMGSDI_SESSION_RUN_GSM_ALGO_CNF:
      case MMGSDI_SESSION_GET_INFO_CNF:
      case MMGSDI_SESSION_STORE_ESN_CNF:
      case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CNF:
      case MMGSDI_SESSION_ENABLE_SERVICE_CNF:
      case MMGSDI_SESSION_DISABLE_SERVICE_CNF:
      case MMGSDI_GET_ALL_AVAILABLE_APPS_CNF:
      case MMGSDI_SESSION_GET_CPHS_INFO_CNF:
      case MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF:
      case MMGSDI_SESSION_SSD_UPDATE_CNF:
      case MMGSDI_SESSION_SSD_CONFIRM_CNF:
      case MMGSDI_SESSION_BS_CHAL_CNF:
      case MMGSDI_SESSION_OTASP_MS_KEY_CNF:
      case MMGSDI_SESSION_OTASP_COMMIT_CNF:
      case MMGSDI_SESSION_OTASP_OTAPA_CNF:
      case MMGSDI_SESSION_READ_PRL_CNF:
      case MMGSDI_SESSION_SUBSCRIPTION_OK_CNF:
      case MMGSDI_SESSION_PERSO_CNF:
        break;

      case MMGSDI_SESSION_OTASP_KEY_GEN_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->session_otasp_key_gen_cnf.ms_result.data_ptr);
        break;

      case MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->session_otasp_sspr_config_cnf.block_data.data_ptr);
        break;

      case MMGSDI_SESSION_OTASP_BLOCK_CNF:
        if( MMGSDI_OTASP_CONFIG ==
            response_ptr->session_otasp_block_cnf.block_op_type)
        {
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
            response_ptr->session_otasp_block_cnf.block_data.data_ptr);
        }
        break;

      case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->session_open_with_sel_rsp_cnf.select_rsp.data_ptr);
        break;

      case MMGSDI_SESSION_OPEN_EXT_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          response_ptr->session_open_ext_cnf.session_info_ptr);
        break;

      case MMGSDI_SESSION_GET_OPERATOR_NAME_CNF:
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(response_ptr->session_get_operator_name_cnf.spn.eons_data.data_ptr);
        if(response_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr != NULL)
        {
          uint32 loop_count = 0;
          for(loop_count = 0;
              loop_count < response_ptr->session_get_operator_name_cnf.plmn_list.num_of_plmn_ids;
              loop_count++)
          {
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
              response_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_ptr);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
              response_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_data.data_ptr);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
              response_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr[loop_count].plmn_additional_info.info_data.data_ptr);
          }
          MMGSDIUTIL_TMC_MEM_FREE(response_ptr->session_get_operator_name_cnf.plmn_list.plmn_info_ptr);
        }
        break;
      }

      case MMGSDI_GET_SE13_PLMN_NAMES_CNF:
      {
        if(response_ptr->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr != NULL)
        {
          uint32 loop_count = 0;
          for(loop_count = 0;
              loop_count < response_ptr->get_se13_plmn_names_cnf.plmn_list.num_of_plmn_ids;
              loop_count++)
          {
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
              response_ptr->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_ptr);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
              response_ptr->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_data.data_ptr);
          }
          MMGSDIUTIL_TMC_MEM_FREE(response_ptr->get_se13_plmn_names_cnf.plmn_list.plmn_info_ptr);
        }
        break;
      }

      case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_CNF:
      {
        if(response_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr != NULL)
        {
          uint32 loop_count = 0;
          for(loop_count = 0;
              loop_count < response_ptr->get_se13_plmn_info_by_name_cnf.num_of_plmn_ids;
              loop_count++)
          {
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
              response_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr[loop_count].plmn_long_name.data_ptr);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
              response_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr[loop_count].plmn_short_name.data_ptr);
          }
          MMGSDIUTIL_TMC_MEM_FREE(response_ptr->get_se13_plmn_info_by_name_cnf.se13_plmn_info_ptr);
        }
        break;
      }

      case MMGSDI_SESSION_SELECT_AID_CNF:
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(response_ptr->session_select_aid_cnf.select_rsp.data_ptr);
        break;

      case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_CNF:
      case MMGSDI_CONTINUE_AFTER_NV_REFRESH_CNF:
      case MMGSDI_HANDLE_NV_REFRESH_CNF:
      case MMGSDI_SAP_CARD_READER_STATUS_CNF:
      case MMGSDI_RESTRICTED_SIM_ACCESS_CNF:
      case MMGSDI_MANAGE_CHANNEL_CNF:
      case MMGSDI_MAX_CNF_ENUM:
        break;
      /* Note - There is no default case here and it is done on purpose since
         we need to handle all values for mmgsdi_cnf_enum_type. Unhandled
         values will get flagged by lint error */
    }
  }
} /* mmgsdi_util_free_response_ptr */


/* ==========================================================================
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

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_client_request_table_index_free(
  int32    index,
  boolean *is_free_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  if (is_free_ptr == NULL)
  {
    return mmgsdi_status;
  }

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_REQ_INFO))
  {
    return mmgsdi_status;
  }

  *is_free_ptr = FALSE;
  mmgsdi_status = MMGSDI_SUCCESS;
  if (mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    *is_free_ptr = TRUE;
  }

  return mmgsdi_status;
} /* mmgsdi_util_is_client_request_table_index_free */


/* ==========================================================================
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

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_free_index(
  int32 * index_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  int32                   i             = 0;
  boolean                 is_free       = TRUE;

  MMGSDIUTIL_RETURN_IF_NULL(index_ptr);

  for (i = 0; i < MMGSDI_MAX_CLIENT_REQ_INFO; i++)
  {
    mmgsdi_status = mmgsdi_util_is_client_request_table_index_free(i, &is_free);
    if (( mmgsdi_status == MMGSDI_SUCCESS ) && ( is_free == TRUE ))
    {
      *index_ptr = i;
      break;
    }
  }
  if (i == MMGSDI_MAX_CLIENT_REQ_INFO)
  {
    UIM_MSG_ERR_0("Unable to get free client index");
  }
  return mmgsdi_status;
} /* mmgsdi_util_get_client_request_table_free_index */


/* ==========================================================================
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

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_populate_client_request_table_info(
  int32                               index,
  const mmgsdi_request_header_type  * request_hdr_ptr,
  uim_cmd_type                      * uim_cmd_ptr,
  mmgsdi_client_req_extra_info_type * extra_param_ptr
)
{
  mmgsdi_task_enum_type   mmgsdi_task   = MMGSDI_TASK_MAX;

  MMGSDIUTIL_RETURN_IF_NULL(request_hdr_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_REQ_INFO - 1);

  mmgsdi_task = mmgsdi_util_get_current_task();
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* Note: The only non-MMGSDI task usage of this function could be from
       MMGSDI cancel API in client context. Hence, instead of returning error,
       make sure that the API call entered the task sync critical section */
    MMGSDIUTIL_CHECK_TASK_SYNC_CRIT_SECT_ACCESS;
  }

  if(mmgsdi_client_req_table_info_ptr[index] != NULL)
  {
    UIM_MSG_ERR_0("client_req_table index is not free");
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_client_req_table_info_ptr[index],
                                     sizeof(mmgsdi_uim_client_req_user_data_type));

  if(mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_client_req_table_info_ptr[index]->uim_cmd_ptr        = uim_cmd_ptr;
  mmgsdi_client_req_table_info_ptr[index]->extra_param_ptr    = extra_param_ptr;
  mmgsdi_client_req_table_info_ptr[index]->client_data        = request_hdr_ptr->client_data;
  mmgsdi_client_req_table_info_ptr[index]->client_id          = request_hdr_ptr->client_id;
  mmgsdi_client_req_table_info_ptr[index]->session_id         = request_hdr_ptr->session_id;
  mmgsdi_client_req_table_info_ptr[index]->request_type       = request_hdr_ptr->request_type;
  mmgsdi_client_req_table_info_ptr[index]->orig_request_type  = request_hdr_ptr->orig_request_type;
  mmgsdi_client_req_table_info_ptr[index]->response_cb        = request_hdr_ptr->response_cb;
  mmgsdi_client_req_table_info_ptr[index]->slot_id            = request_hdr_ptr->slot_id;
  mmgsdi_client_req_table_info_ptr[index]->task_id            = mmgsdi_task;
  mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status     = MMGSDI_UIM_RSP_INBOUND;

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_populate_client_request_table_info */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_FREE_CLIENT_REQUEST_TABLE_INDEX

   DESCRIPTION:
     This function free the client request and uim rsp data pointers
     and set the len of the rsp data data to zero

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_free_client_request_table_index(
  int32 index
)
{
  mmgsdi_uim_client_req_user_data_type *client_data_ptr = NULL;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_REQ_INFO) ||
      (mmgsdi_client_req_table_info_ptr[index] == NULL))
  {
    UIM_MSG_ERR_2("Index 0x%x >= 0x%x or not populated in MMGSDI client req table",
                index, MMGSDI_MAX_CLIENT_REQ_INFO);
    return;
  }

  client_data_ptr = mmgsdi_client_req_table_info_ptr[index];
  /* Synch call will require to free this pointer in the calling function */

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_data_ptr->uim_cmd_ptr);

  switch (client_data_ptr->request_type)
  {
  case MMGSDI_WRITE_REQ:
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      client_data_ptr->extra_param_ptr->write_data.data.data_ptr);
    break;
  case MMGSDI_INCREASE_REQ:
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      client_data_ptr->extra_param_ptr->increase_data.data.data_ptr);
    break;
  case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
  case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
  case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
  case MMGSDI_READ_REQ:
  case MMGSDI_REHABILITATE_REQ:
  case MMGSDI_INVALIDATE_REQ:
  case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
  case MMGSDI_PIN_OPERATION_REQ:
  case MMGSDI_GET_ALL_PIN_STATUS_REQ:
  case MMGSDI_MANAGE_CHANNEL_REQ:
    break;
  case MMGSDI_GET_FILE_ATTR_REQ:
    if(client_data_ptr->extra_param_ptr->get_file_attr_data.int_client_data != 0)
    {
      mmgsdi_pin_extra_info_type* pin_extra_info_ptr;
      pin_extra_info_ptr = (mmgsdi_pin_extra_info_type*)client_data_ptr->extra_param_ptr->get_file_attr_data.int_client_data;
      MMGSDIUTIL_TMC_MEM_FREE(pin_extra_info_ptr);
    }
    break;
  case MMGSDI_SESSION_SELECT_AID_REQ:
  case MMGSDI_UIM_REPORT_RSP:
  case MMGSDI_CLIENT_ID_REG_RSP:
  case MMGSDI_SESSION_OPEN_REQ:
  case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
  case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
  case MMGSDI_SESSION_CLOSE_REQ:
  case MMGSDI_SESSION_DEACTIVATE_REQ:
  case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
  case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
  case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
  case MMGSDI_ISIM_AUTH_REQ:
  case MMGSDI_SRV_AVAILABLE_REQ:
  case MMGSDI_USIM_AUTH_REQ:
  case MMGSDI_COMPUTE_IP_AUTH_REQ:
  case MMGSDI_RUN_CAVE_REQ:
  case MMGSDI_SESSION_CDMA_AKA_REQ:
  case MMGSDI_GENERATE_KEY_VPM_REQ:
  case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
  case MMGSDI_CNF:
  case MMGSDI_BCAST_REQ:
  case MMGSDI_SAP_CONNECT_REQ:
  case MMGSDI_SAP_DISCONNECT_REQ:
  case MMGSDI_SAP_GET_ATR_REQ:
  case MMGSDI_SAP_POWER_ON_REQ:
  case MMGSDI_SAP_POWER_OFF_REQ:
    break;
  case MMGSDI_SAP_SEND_APDU_REQ:
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        client_data_ptr->extra_param_ptr->
          sap_send_apdu_data.data.data_ptr);
    break;
  case MMGSDI_SAP_RESET_REQ:
  case MMGSDI_SAP_REPORT_RSP:
  case MMGSDI_CARD_STATUS_REQ:
  case MMGSDI_REFRESH_REQ:
  case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
  case MMGSDI_CARD_PDOWN_REQ:
  case MMGSDI_CARD_PUP_REQ:
  case MMGSDI_CARD_RESET_REQ:
  case MMGSDI_SEND_APDU_REQ:
  case MMGSDI_SEND_APDU_EXT_REQ:
  case MMGSDI_GET_ATR_REQ:
    break;
  case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      client_data_ptr->extra_param_ptr->session_get_eons_data.plmn_id_list.plmn_list_ptr);
    break;
  case MMGSDI_EONS_SS_EVENT_REQ:
    mmgsdi_eons_free_nitz_data_ptrs(
       &(client_data_ptr->extra_param_ptr->ss_event_data.ss_info.first_stack_ss.nitz_info));
    mmgsdi_eons_free_nitz_data_ptrs(
       &(client_data_ptr->extra_param_ptr->ss_event_data.ss_info.sec_stack_ss.nitz_info));
    break;
  case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
      client_data_ptr->extra_param_ptr->get_se13_plmn_names_data.plmn_id_list.plmn_list_ptr);
    break;
  case  MMGSDI_SEARCH_REQ:
    break;
  case MMGSDI_SESSION_GET_INFO_REQ:
  case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
  case MMGSDI_SESSION_STORE_ESN_REQ:
  case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ:
  case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
  case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
  case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
  case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
  case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
  case MMGSDI_SESSION_SSD_UPDATE_REQ:
  case MMGSDI_SESSION_SSD_CONFIRM_REQ:
  case MMGSDI_SESSION_BS_CHAL_REQ:
  case MMGSDI_SESSION_OTASP_MS_KEY_REQ:
  case MMGSDI_SESSION_OTASP_KEY_GEN_REQ:
  case MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ:
  case MMGSDI_SESSION_OTASP_COMMIT_REQ:
  case MMGSDI_SESSION_OTASP_OTAPA_REQ:
  case MMGSDI_SESSION_OTASP_BLOCK_REQ:
  case MMGSDI_SESSION_READ_PRL_REQ:
  case MMGSDI_SESSION_SUB_OK_REQ:
  case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
  case MMGSDI_NOTIFY_LINK_EST_REQ:
  case MMGSDI_NOTIFY_CARD_ERROR_REQ:
  case MMGSDI_NOTIFY_SIM_BUSY_REQ:
  case MMGSDI_INTERNAL_PUP_INIT_REQ:
  case MMGSDI_SESSION_OPEN_EXT_REQ:
  case MMGSDI_RECOVERY_IND_REQ:
  case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
  case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
  case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ:
  case MMGSDI_SET_BUILTIN_PLMN_LIST_REQ:
  case MMGSDI_SIMLOCK_RSP:
  case MMGSDI_SIMLOCK_EVT:
  case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
  case MMGSDI_HANDLE_NV_REFRESH_REQ:
  case MMGSDI_FULL_SERVICE_REQ:
  case MMGSDI_SAP_CARD_READER_STATUS_REQ:
  case MMGSDI_GET_SIM_PROFILE_REQ:
  case MMGSDI_SET_SIM_PROFILE_REQ:
  case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
  case MMGSDI_MAX_CMD_ENUM:
    break;
  /* Note - There is no default case here and it is done on purpose since we
     need to handle all values for mmgsdi_cmd_enum_type. Unhandled values
     will get flagged by lint error */
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_data_ptr->extra_param_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(client_data_ptr);
  mmgsdi_client_req_table_info_ptr[index] = NULL;

} /* mmgsdi_util_free_client_request_table_index */


/* ==========================================================================
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

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_info(
  int32                                  index,
  mmgsdi_request_header_type            *request_hdr_ptr,
  uim_cmd_type                         **uim_cmd_pptr,
  mmgsdi_client_req_extra_info_type    **extra_param_pptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_3(request_hdr_ptr, uim_cmd_pptr, extra_param_pptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_REQ_INFO - 1);

  if(mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    UIM_MSG_ERR_1("Index 0x%x not populated in MMGSDI client req table", index);
    return MMGSDI_ERROR;
  }

  request_hdr_ptr->client_data       = mmgsdi_client_req_table_info_ptr[index]->client_data;
  request_hdr_ptr->client_id         = mmgsdi_client_req_table_info_ptr[index]->client_id;
  request_hdr_ptr->session_id        = mmgsdi_client_req_table_info_ptr[index]->session_id;
  request_hdr_ptr->orig_request_type = mmgsdi_client_req_table_info_ptr[index]->orig_request_type;
  request_hdr_ptr->request_type      = mmgsdi_client_req_table_info_ptr[index]->request_type;
  request_hdr_ptr->response_cb       = mmgsdi_client_req_table_info_ptr[index]->response_cb;
  request_hdr_ptr->slot_id           = mmgsdi_client_req_table_info_ptr[index]->slot_id;

  *uim_cmd_pptr        = mmgsdi_client_req_table_info_ptr[index]->uim_cmd_ptr;
  *extra_param_pptr    = mmgsdi_client_req_table_info_ptr[index]->extra_param_ptr;

  return mmgsdi_status;
} /* mmgsdi_util_get_client_request_table_info */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_INDEX

   DESCRIPTION:
     Based on the request type, the client table is searched through to get
     a valid index.
   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Successful retrieval of information
     MMGSDI_ERROR:            Unable to retrieve information
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_index
(
  int32* index_ptr,
  mmgsdi_cmd_enum_type request_type
)
{
  int i = 0;

  MMGSDIUTIL_RETURN_IF_NULL(index_ptr);

  for(i = 0; i < MMGSDI_MAX_CLIENT_REQ_INFO; i++)
  {
    if(mmgsdi_client_req_table_info_ptr[i] == NULL)
    {
      continue;
    }
    if(mmgsdi_client_req_table_info_ptr[i]->orig_request_type == request_type &&
      mmgsdi_client_req_table_info_ptr[i]->request_type       == request_type)
    {
      *index_ptr = i;
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_ERROR;
} /* mmgsdi_util_get_client_request_table_index */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_COPY_ACCESS_TYPE

   DESCRIPTION:
     This function copy access type information from one buffer to another.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_copy_access_type(
  mmgsdi_access_type       *dest_access_ptr,
  const mmgsdi_access_type *src_access_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL_2(dest_access_ptr, src_access_ptr);

  if (src_access_ptr->access_method == MMGSDI_MAX_ACCESS_METHOD_ENUM)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  mmgsdi_memscpy(dest_access_ptr, sizeof(mmgsdi_access_type),
                 src_access_ptr, sizeof(mmgsdi_access_type));

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_copy_access_type */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_USIM_SPEC_VERSION

   DESCRIPTION:
     This function checks the usim specification

   DEPENDENCIES:
     None

   LIMITATIONS:
     Used for UICC only

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None

==========================================================================*/
void mmgsdi_util_check_usim_spec_version (
  mmgsdi_access_type                 *mmgsdi_access_ptr,
  mmgsdi_usim_spec_version_enum_type  usim_spec_version
)
{

  if(mmgsdi_access_ptr == NULL)
  {
    return;
  }

  if(mmgsdi_access_ptr->access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    switch (mmgsdi_access_ptr->file.file_enum)
    {
      case MMGSDI_USIM_PLMNWACT:
        if (usim_spec_version == MMGSDI_SPEC_VERSION_R99_2000_07)
        {
          mmgsdi_access_ptr->file.file_enum = MMGSDI_USIM_UPLMNSEL;
        }
        break;

      case MMGSDI_USIM_OPLMNWACT:
        if (usim_spec_version == MMGSDI_SPEC_VERSION_R99_2000_07)
        {
          mmgsdi_access_ptr->file.file_enum = MMGSDI_USIM_OPLMNSEL;
        }
        break;

      default:
        break;
    }
  }
} /* mmgsdi_util_check_usim_spec_version */


/*===========================================================================
FUNCTION MMGSDI_UTIL_CHECK_EST_AVAILABLE

DESCRIPTION
  Check the availablity of files (USIM: FDN, BDN, ACL; CSIM: FDN) to
  determine if EF-EST is present

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  EFest available
            FALSE: EFest not available
===========================================================================*/
static boolean mmgsdi_util_check_est_available (
  mmgsdi_client_id_type           client_id,
  mmgsdi_session_id_type          session_id,
  mmgsdi_file_enum_type           file_name
)
{
  mmgsdi_return_enum_type      mmgsdi_status     = MMGSDI_SUCCESS;
  boolean                      est_srv_available = FALSE;

  if (file_name == MMGSDI_USIM_EST)
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                  session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_USIM_SRV_EST,
                                                  &est_srv_available);
    if (est_srv_available == FALSE &&
        mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                    session_id,
                                                    MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                    MMGSDI_USIM_SRV_FDN,
                                                    &est_srv_available);
      if (est_srv_available == FALSE &&
          mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                      session_id,
                                                      MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                      MMGSDI_USIM_SRV_BDN,
                                                      &est_srv_available);
        if (est_srv_available == FALSE &&
            mmgsdi_status == MMGSDI_SUCCESS)
        {
          mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                        session_id,
                                                        MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                        MMGSDI_USIM_SRV_ACL,
                                                        &est_srv_available);
        }
      }
    }
  }
  else if (file_name == MMGSDI_CSIM_EST)
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                  session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_CSIM_SRV_EST,
                                                  &est_srv_available);
    if (est_srv_available == FALSE &&
        mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                    session_id,
                                                    MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                    MMGSDI_CSIM_SRV_FDN,
                                                    &est_srv_available);
    }
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Service check of EF-EST failed, let the command go to the card */
    return TRUE;
  }

  return est_srv_available;
} /* mmgsdi_util_check_est_available */


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
)
{
  boolean                  file_ok_in_svc_table = FALSE;
  mmgsdi_return_enum_type  mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_service_enum_type service_type         = MMGSDI_NONE;
  mmgsdi_slot_id_enum_type slot_id              = MMGSDI_SLOT_1;

  if(mmgsdi_access_ptr == NULL ||
     mmgsdi_access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS)
  {
    /* Allow the command to the card */
    return MMGSDI_SUCCESS;
  }

  mmgsdi_status = mmgsdi_file_get_srv_from_file(
    app_type,
    mmgsdi_access_ptr->file.file_enum,
    &service_type);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* can't find in the table mapping, allow the command to go to the
       card */
    return MMGSDI_SUCCESS;
  }

  if(mmgsdi_access_ptr->file.file_enum == MMGSDI_USIM_EST ||
     mmgsdi_access_ptr->file.file_enum == MMGSDI_CSIM_EST)
  {
    if(!mmgsdi_util_check_est_available(client_id,
                                        session_id,
                                        mmgsdi_access_ptr->file.file_enum))
    {
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
    }
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
    session_id, &slot_id, NULL, NULL, NULL, NULL, NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_SUCCESS;
  }

  /* EF-EHPLMN has priority over the proprietary ActingHPLMN.
     Check if EHPLMN is present */
  if((mmgsdi_access_ptr->file.file_enum == MMGSDI_USIM_7F66_PROP1_ACT_HPLMN) &&
     (mmgsdi_nv_is_ens_enabled(slot_id)))
  {
    service_type = MMGSDI_USIM_SRV_EHPLMN;
  }

  /* Block RUIM EPRL access if feature is enabled */
  if(mmgsdi_access_ptr->file.file_enum == MMGSDI_CDMA_EPRL &&
     mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_BLOCK_RUIM_EPRL_ACCESS, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  if(service_type == MMGSDI_NONE)
  {
    /* No service check associated with the file enum, allow the command to
       go to the card */
    return MMGSDI_SUCCESS;
  }

  switch(app_type)
  {
    case MMGSDI_APP_USIM:
    case MMGSDI_APP_CSIM:
      mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                    session_id,
                                                    MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                    service_type,
                                                    &file_ok_in_svc_table);
      break;

    case MMGSDI_APP_SIM:
    case MMGSDI_APP_RUIM:
      mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                                   MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                   service_type,
                                                   &file_ok_in_svc_table);
      break;

    default:
      /* Do not perform any service table checking on unknown Apps, let
         the command goes to the card */
      UIM_MSG_ERR_1("Invalid app type 0x%x", app_type);
      return MMGSDI_SUCCESS;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    if (mmgsdi_status == MMGSDI_ACCESS_DENIED)
    {
      /* If the read of service table EF fails because pin is not verified yet,
         we should try to be restrictive and treat it as if the service bit is
         not enabled in the service table and hence restrict the access */
      UIM_MSG_ERR_0("Access to service table is denied");
      return MMGSDI_ACCESS_DENIED;
    }
    /* Service check failed, may be because of some memory allocation issues or
       maybe beacuse of some other problem accessing the the service table EF
       (other than failed access condition), let the command go to the card */
    return MMGSDI_SUCCESS;
  }

  /* If EHPLMN is present, reject access to ActingHPLMN. If not present, allow
     access to ActingHPLMN */
  if(mmgsdi_access_ptr->file.file_enum == MMGSDI_USIM_7F66_PROP1_ACT_HPLMN)
  {
    if(file_ok_in_svc_table)
    {
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
    }
    else
    {
      mmgsdi_status = MMGSDI_SUCCESS;
    }
    return mmgsdi_status;
  }

  if(!file_ok_in_svc_table)
  {
    if((mmgsdi_access_ptr->file.file_enum != MMGSDI_CSIM_3GPD_ME3GPDOPC) &&
       (mmgsdi_access_ptr->file.file_enum != MMGSDI_CSIM_3GPD_3GPDOPM))
    {
      return MMGSDI_NOT_SUPPORTED;
    }
  }

  /* Check the additional services as needed */
  if(mmgsdi_access_ptr->file.file_enum == MMGSDI_USIM_MMSUCP)
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                  session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_USIM_SRV_MMS_USR_P,
                                                  &file_ok_in_svc_table);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      file_ok_in_svc_table = TRUE;
    }
  }
  else if(mmgsdi_access_ptr->file.file_enum == MMGSDI_CSIM_MMS_UCP)
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                  session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_CSIM_SRV_MMS_USR_P,
                                                  &file_ok_in_svc_table);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      file_ok_in_svc_table = TRUE;
    }
  }
  else if((mmgsdi_access_ptr->file.file_enum == MMGSDI_CSIM_3GPD_ME3GPDOPC) ||
          (mmgsdi_access_ptr->file.file_enum == MMGSDI_CSIM_3GPD_3GPDOPM))
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                  session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_CSIM_SRV_3GPD_MIP,
                                                  &file_ok_in_svc_table);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      file_ok_in_svc_table = TRUE;
    }
  }
  else if((mmgsdi_access_ptr->file.file_enum == MMGSDI_CSIM_3GPD_TCPCONFIG) ||
          (mmgsdi_access_ptr->file.file_enum == MMGSDI_CSIM_3GPD_DGC) ||
          (mmgsdi_access_ptr->file.file_enum == MMGSDI_CSIM_3GPD_UPPEXT))
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                  session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_CSIM_SRV_3GPD_SIP,
                                                  &file_ok_in_svc_table);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      file_ok_in_svc_table = TRUE;
    }
    else
    {
      if(!file_ok_in_svc_table)
      {
        mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                      session_id,
                                                      MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                      MMGSDI_CSIM_SRV_3GPD_MIP,
                                                      &file_ok_in_svc_table);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          file_ok_in_svc_table = TRUE;
        }
      }
    }
  }
  else if(mmgsdi_access_ptr->file.file_enum == MMGSDI_TELECOM_PSISMSC)
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                  session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_USIM_SRV_SMIP,
                                                  &file_ok_in_svc_table);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      file_ok_in_svc_table = TRUE;
    }
  }
  else if(mmgsdi_access_ptr->file.file_enum == MMGSDI_CDMA_3GPD_UPPEXT)
  {
    mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                                 MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                 MMGSDI_CDMA_SRV_3GPD_SIP,
                                                 &file_ok_in_svc_table);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      file_ok_in_svc_table = TRUE;
    }
    else
    {
      if(!file_ok_in_svc_table)
      {
        mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                                     MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                     MMGSDI_CDMA_SRV_3GPD_MIP,
                                                     &file_ok_in_svc_table);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          file_ok_in_svc_table = TRUE;
        }
      }
    }
  }

  return file_ok_in_svc_table ? MMGSDI_SUCCESS : MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_util_is_file_ok_in_svc_table*/


/* ==========================================================================
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
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_queue_mmgsdi_uim_report_rsp(
  int32                   index,
  mmgsdi_cmd_enum_type    mmgsdi_cmd_enum,
  mmgsdi_return_enum_type mmgsdi_report_status
  )
{
  mmgsdi_uim_report_rsp_type      * msg_ptr       = NULL;
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
  int32                             total_mem_len = 0;
  uint32                            task_cmd_len  = 0;
  mmgsdi_task_enum_type             mmgsdi_task   = MMGSDI_TASK_MAX;

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No Need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.uim_report_rsp;

  /*-----------------------------------------------------------------------
   Populating mmgsdi_uim_report_rsp_type header
   1) Client ID = 0
   2) Request TYPE
   3) Slot ID: Set the slot to automatic since the info is inside the user
               data
   4) Client Data Pointer: The previous request
   5) Response CallBack
   6) Payload len and request len will be populated after the content
      has been populated
   -----------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                   = MMGSDI_UIM_REPORT_RSP;
  msg_ptr->request_header.client_id            = 0;
  msg_ptr->request_header.request_type         = MMGSDI_UIM_REPORT_RSP;
  msg_ptr->request_header.orig_request_type    = MMGSDI_UIM_REPORT_RSP;
  msg_ptr->request_header.slot_id              = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.client_data          = 0;
  msg_ptr->request_header.response_cb          = NULL;
  msg_ptr->mmgsdi_error_status                 = mmgsdi_report_status;

  /*-----------------------------------------------------------------------
  Populating mmgsdi_uim_report_rsp_type content
  - the command was successful
  - report code
  - status words
  - command transacted or not
  - response data: index into mmgsdi client req table
  -----------------------------------------------------------------------*/
  msg_ptr->is_uim_success =
   ((mmgsdi_report_status == MMGSDI_SUCCESS)?TRUE:FALSE);
  if (msg_ptr->is_uim_success)
  {
    msg_ptr->status_word.sw1 = GSDI_SIM_SW1_NORM_END;
    msg_ptr->status_word.sw2 = 0x00;
  }
  else
  {
    msg_ptr->status_word.sw1 = 0xFF;
    msg_ptr->status_word.sw2 = 0xFF;
  }

  msg_ptr->uim_rpt_code = (int)UIM_NO_SUCH_REPORT_R;

  switch(mmgsdi_cmd_enum)
  {
    case MMGSDI_READ_REQ:
    case MMGSDI_WRITE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_ACCESS_R;
      break;
    case MMGSDI_INCREASE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_INC_CMD_R;
      break;
    case MMGSDI_GET_FILE_ATTR_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_SELECT_R;
      break;
    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
    case MMGSDI_SESSION_CLOSE_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_MANAGE_CHANNEL_R;
      break;
    case MMGSDI_SESSION_DEACTIVATE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_STATUS_R;
      break;
    case MMGSDI_ISIM_AUTH_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_ISIM_AUTHENTICATE_R;
      break;
    case MMGSDI_SEND_APDU_REQ:
    case MMGSDI_SEND_APDU_EXT_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_STREAM_ISO7816_APDU_R;
      break;
    case MMGSDI_SAP_CONNECT_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RESET_SWITCH_UIM_PASSIVE_R;
      break;
    case MMGSDI_SAP_GET_ATR_REQ:
      break;
    case MMGSDI_SAP_SEND_APDU_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_STREAM_ISO7816_APDU_R;
      break;
    case MMGSDI_SAP_POWER_ON_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_POWER_UP_UIM_PASSIVE_R;
      break;
    case MMGSDI_SAP_POWER_OFF_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_POWER_DOWN_R;
      break;
    case MMGSDI_SAP_RESET_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RESET_R;
      break;
    case MMGSDI_CARD_PUP_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_POWER_UP_UIM_PASSIVE_R;
      break;
    case MMGSDI_CARD_PDOWN_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_POWER_DOWN_R;
      break;
    case MMGSDI_CARD_STATUS_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_STATUS_R;
      break;
    case MMGSDI_CARD_RESET_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RESET_R;
      break;
    case MMGSDI_REFRESH_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RESET_R;
      break;
    case MMGSDI_REHABILITATE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_REHABILITATE_R;
      break;
    case MMGSDI_INVALIDATE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_INVALIDATE_R;
      break;
    case MMGSDI_PIN_OPERATION_REQ:
    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
    case MMGSDI_SRV_AVAILABLE_REQ:
    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
      break;
    case MMGSDI_USIM_AUTH_REQ:
    case MMGSDI_BCAST_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_AUTHENTICATE_R;
      break;
    case MMGSDI_COMPUTE_IP_AUTH_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_COMPUTE_IP_AUTH_R;
      break;
    case MMGSDI_RUN_CAVE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RUN_CAVE_R;
      break;
    case MMGSDI_SESSION_CDMA_AKA_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_AKA_AUTH_R;
      break;
    case MMGSDI_GENERATE_KEY_VPM_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_GENERATE_KEYS_R;
      break;
    case MMGSDI_GET_ATR_REQ:
      /* No UIM Command associate with this */
      break;
    case MMGSDI_SEARCH_REQ:
      break;
    case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_RUN_GSM_ALGO_R;
      break;
    case MMGSDI_SESSION_GET_INFO_REQ:
    case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ:
    case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
    case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
    case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
    case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
    case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
    case MMGSDI_SESSION_READ_PRL_REQ:
    case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
    case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
    case MMGSDI_SESSION_OPEN_EXT_REQ:
    case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
    case MMGSDI_EONS_SS_EVENT_REQ:
    case MMGSDI_FULL_SERVICE_REQ:
    case MMGSDI_RECOVERY_IND_REQ:
    case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
    case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ:
    case MMGSDI_SET_BUILTIN_PLMN_LIST_REQ:
    case MMGSDI_GET_SIM_PROFILE_REQ:
    case MMGSDI_SET_SIM_PROFILE_REQ:
    case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
      break;
    case MMGSDI_SESSION_STORE_ESN_REQ:
      /* always default to MEID ME R*/
      msg_ptr->uim_rpt_code = (int)UIM_STORE_ESN_MEID_ME_R;
      break;
    case MMGSDI_SESSION_OTASP_OTAPA_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_OTAPA_REQ_R;
      break;
    case MMGSDI_SESSION_SSD_UPDATE_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_SSD_UPDATE_R;
      break;
    case MMGSDI_SESSION_SSD_CONFIRM_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_CONFIRM_SSD_R;
      break;
    case MMGSDI_SESSION_BS_CHAL_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_BS_CHAL_R;
      break;
    case MMGSDI_SESSION_OTASP_MS_KEY_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_MS_KEY_REQ_R;
      break;
    case MMGSDI_SESSION_OTASP_KEY_GEN_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_KEY_GEN_REQ_R;
      break;
    case MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_SSPR_CONFIG_REQ_R;
      break;
    case MMGSDI_SESSION_OTASP_COMMIT_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_COMMIT_R;
      break;
    case MMGSDI_SESSION_OTASP_BLOCK_REQ:
      /* Block op type is unknown here */
      break;
    case MMGSDI_SAP_DISCONNECT_REQ:
      break;

    case MMGSDI_SESSION_SUB_OK_REQ:
      break;

    case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
      break;

    case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
      break;

    case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
      break;

    case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
      break;

    case MMGSDI_HANDLE_NV_REFRESH_REQ:
      break;

    case MMGSDI_MANAGE_CHANNEL_REQ:
      msg_ptr->uim_rpt_code = (int)UIM_MANAGE_CHANNEL_R;
      break;

    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
    case MMGSDI_NOTIFY_LINK_EST_REQ:
    case MMGSDI_NOTIFY_CARD_ERROR_REQ:
    case MMGSDI_NOTIFY_SIM_BUSY_REQ:
    case MMGSDI_INTERNAL_PUP_INIT_REQ:
    case MMGSDI_SAP_REPORT_RSP:
    case MMGSDI_UIM_REPORT_RSP:
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_SIMLOCK_RSP:
    case MMGSDI_SIMLOCK_EVT:
    case MMGSDI_CNF:
    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
    case MMGSDI_MAX_CMD_ENUM:
      /* Error, should not be handled by this function,
         client related function => client_id_report
         sap related function => sap_report */
    /* Note - There is no default case here and it is done on purpose since we
       need to handle all values for mmgsdi_cmd_enum_type. Unhandled values
       will get flagged by lint error */
      UIM_MSG_ERR_2("cmd 0x%x should NOT be handled by mmgsdi_util_queue_mmgsdi_uim_report_rsp; "
                    "Command Queuing Fail 0x%x",
                    mmgsdi_cmd_enum, mmgsdi_status);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
      return MMGSDI_ERROR;
  }/* end switch - request type */

  msg_ptr->cmd_transacted  = FALSE;  /* This should be false? */
  msg_ptr->rsp_data_index  = index;
  /*-------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
    -------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi response queue */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case MMGSDI task is undetermined,
       queue to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }
  mmgsdi_status = mmgsdi_cmd_ext(task_cmd_ptr, mmgsdi_task);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("In mmgsdi_util_queue_mmgsdi_uim_report_rsp(), Queue of MMGSDI_UIM_REPORT_RSP, cmd: 0x%x, status 0x%x",
                   mmgsdi_cmd_enum, mmgsdi_status);
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
  }

  return mmgsdi_status;
} /* mmgsdi_util_queue_mmgsdi_uim_report_rsp */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_QUEUE_MMGSDI_SAP_REPORT_RSP

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
  ==========================================================================*/
  mmgsdi_return_enum_type mmgsdi_util_queue_mmgsdi_sap_report_rsp(
    mmgsdi_slot_id_enum_type slot,
    int32                    index,
    mmgsdi_return_enum_type  cmd_status
 )
{
  mmgsdi_task_cmd_type            * task_cmd_ptr  = NULL;
  mmgsdi_generic_rsp_type         * msg_ptr = NULL;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
  int32                             total_mem_len = 0;
  uint32                            task_cmd_len  = 0;
  mmgsdi_task_enum_type             mmgsdi_task   = MMGSDI_TASK_MAX;

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
   /*---------------------------------------------------------------------------
    - No Need to Align pointers of the structures members
  ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.sap_report_rsp;

  /*-----------------------------------------------------------------------
    Populating mmgsdi_sap_report_rsp_type header
    1) Client ID = 0
    2) Request TYPE
    3) Slot ID: Set the slot to automatic since the info is inside the user
                data
    4) Client Data Pointer: The previous request
    5) Response CallBack
    6) Payload len and request len will be populated after the content
      has been populated
    -----------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SAP_REPORT_RSP;
  msg_ptr->request_header.client_id         = 0;
  msg_ptr->request_header.request_type      = MMGSDI_SAP_REPORT_RSP;
  msg_ptr->request_header.orig_request_type = MMGSDI_SAP_REPORT_RSP;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = NULL;
  /*-----------------------------------------------------------------------
  Populating mmgsdi_uim_report_rsp_type content
  - the command was successful
  - report code
  - status words
  - command transacted or not
  - response data: index into mmgsdi client req table
  -----------------------------------------------------------------------*/
  msg_ptr->status = cmd_status;  /* Software Error */

  msg_ptr->rsp_data_index  = index;
  /*-------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
    -------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi response queue */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  /* Task is not determined. So check the slot */
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    mmgsdi_task = mmgsdi_util_get_task_from_slot(slot);
    if(mmgsdi_task == MMGSDI_TASK_MAX)
    {
      /* Slot_id is not set. So queue to main task */
      mmgsdi_task = MMGSDI_TASK_MAIN;
    }
  }
  mmgsdi_status = mmgsdi_cmd_ext(task_cmd_ptr, mmgsdi_task);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
  }
  return mmgsdi_status;
} /* mmgsdi_util_queue_mmgsdi_sap_report_rsp */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_POPULATE_CNF_RSP_HEADER

   DESCRIPTION:
     This function populate the confirmation header information

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_populate_cnf_rsp_header(
  const void*                         cnf_ptr,
  mmgsdi_response_header_type       * rsp_hdr_ptr,
  const mmgsdi_request_header_type  * req_hdr_ptr,
  mmgsdi_cnf_enum_type                cnf_cmd,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
)
{
  int32                              len = 0;
  mmgsdi_file_security_access_type  *file_security_access_ptr = NULL;
  if ((rsp_hdr_ptr == NULL) || (req_hdr_ptr == NULL))
    return;

  rsp_hdr_ptr->client_data  = req_hdr_ptr->client_data;
  rsp_hdr_ptr->client_id    = req_hdr_ptr->client_id;
  rsp_hdr_ptr->response_len = 0;

  switch(cnf_cmd)
  {
  case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_client_id_and_evt_reg_cnf_type));
    break;

  case MMGSDI_CLIENT_ID_AND_EVT_DEREG_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_client_id_and_evt_dereg_cnf_type));
    break;

  case MMGSDI_GET_ALL_AVAILABLE_APPS_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_get_all_available_apps_cnf_type));
    break;

  case MMGSDI_ACTIVATE_ONCHIP_SIM_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_act_onchip_sim_cnf_type));
    break;

  case MMGSDI_READ_CNF:
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Can't assign rsp_hdr_ptr->response_len: Read Cnf null");
      return;
    }
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_read_cnf_type)) +
      ((mmgsdi_read_cnf_type*)cnf_ptr)->read_data.data_len;
    break;

  case MMGSDI_WRITE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_write_cnf_type));
    break;

  case MMGSDI_INCREASE_CNF:
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Can't assign rsp_hdr_ptr->response_len: Increase Cnf null");
      return;
    }
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_increase_cnf_type)) +
      ((mmgsdi_increase_cnf_type*)cnf_ptr)->increased_data.data_len;
    break;

  case MMGSDI_GET_FILE_ATTR_CNF:
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Can't assign rsp_hdr_ptr->response_len: Get File Attr Cnf null");
      return;
    }
    switch (((mmgsdi_get_file_attr_cnf_type*)cnf_ptr)->file_attrib.file_type)
    {
      case MMGSDI_LINEAR_FIXED_FILE:
        file_security_access_ptr =
         &((mmgsdi_get_file_attr_cnf_type*)cnf_ptr)->file_attrib.file_info.linear_fixed_file.file_security;
        break;

      case MMGSDI_CYCLIC_FILE:
        file_security_access_ptr =
         &((mmgsdi_get_file_attr_cnf_type*)cnf_ptr)->file_attrib.file_info.cyclic_file.file_security;
        break;

      case MMGSDI_TRANSPARENT_FILE:
        file_security_access_ptr =
          &((mmgsdi_get_file_attr_cnf_type*)cnf_ptr)->file_attrib.file_info.transparent_file.file_security;
        break;

      case MMGSDI_MASTER_FILE:
      case MMGSDI_DEDICATED_FILE:
      case MMGSDI_MAX_FILE_STRUCTURE_ENUM:
        break;
    }
    if (file_security_access_ptr != NULL)
    {
      len = file_security_access_ptr->read.num_protection_pin *
        uint32toint32(sizeof(mmgsdi_pin_enum_type));
      len += file_security_access_ptr->write.num_protection_pin *
        uint32toint32(sizeof(mmgsdi_pin_enum_type));
      len += file_security_access_ptr->increase.num_protection_pin *
        uint32toint32(sizeof(mmgsdi_pin_enum_type));
      len += file_security_access_ptr->invalidate_deactivate.num_protection_pin *
        uint32toint32(sizeof(mmgsdi_pin_enum_type));
      len += file_security_access_ptr->rehabilitate_activate.num_protection_pin *
        uint32toint32(sizeof(mmgsdi_pin_enum_type));
    }
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_get_file_attr_cnf_type)) + len;
    break;

  case MMGSDI_JCDMA_GET_CARD_INFO_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_jcdma_get_card_info_cnf_type));
    break;

  case MMGSDI_SESSION_OPEN_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_open_cnf_type));
    break;

  case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_open_with_sel_rsp_cnf_type));
    break;

  case MMGSDI_SESSION_OPEN_EXT_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_open_ext_cnf_type));
    break;

  case MMGSDI_SESSION_OPEN_WITH_MF_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_open_with_mf_cnf_type));
    break;

  case MMGSDI_SESSION_CLOSE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_close_cnf_type));
    break;

  case MMGSDI_SESSION_DEACTIVATE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_deactivate_cnf_type));
    break;

  case MMGSDI_SESSION_APP_RESET_DEACTIVATE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_app_reset_deactivate_cnf_type));
    break;

  case MMGSDI_SESSION_APP_RESET_ACTIVATE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_app_reset_activate_cnf_type));
    break;

  case MMGSDI_SESSION_SELECT_AID_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_session_select_aid_cnf_type));
    break;

  case MMGSDI_ISIM_AUTH_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_isim_auth_cnf_type));
    break;

  case MMGSDI_SAP_CONNECT_CNF:
      rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_sap_connect_cnf_type));
    break;

  case MMGSDI_SAP_DISCONNECT_CNF:
      rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_sap_disconnect_cnf_type));
    break;

  case MMGSDI_SAP_GET_ATR_CNF:
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Can't assign rsp_hdr_ptr->response_len: Sap Get ATR Cnf null");
      return;
    }
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_sap_get_atr_cnf_type)) +
        ((mmgsdi_sap_get_atr_cnf_type*)cnf_ptr)->atr_data.data_len;
    break;

  case MMGSDI_SEND_APDU_CNF:
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Can't assign rsp_hdr_ptr->response_len: Send APDU Cnf null");
      return;
    }
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_send_apdu_cnf_type)) +
        ((mmgsdi_send_apdu_cnf_type*)cnf_ptr)->apdu_data.data_len;
    break;

  case MMGSDI_SEND_APDU_EXT_CNF:
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Can't assign rsp_hdr_ptr->response_len: Send APDU EXT Cnf null");
      return;
    }
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_send_apdu_ext_cnf_type)) +
        ((mmgsdi_send_apdu_ext_cnf_type*)cnf_ptr)->apdu_data.data_len;
    break;

  case MMGSDI_SAP_SEND_APDU_CNF:
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Can't assign rsp_hdr_ptr->response_len: SAP Send APDU Cnf null");
      return;
    }
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_sap_send_apdu_cnf_type)) +
        ((mmgsdi_sap_send_apdu_cnf_type*)cnf_ptr)->apdu_data.data_len;
    break;

  case MMGSDI_SAP_POWER_ON_CNF:
      rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_sap_power_on_cnf_type));
    break;

  case MMGSDI_SAP_POWER_OFF_CNF:
      rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_sap_power_off_cnf_type));
    break;

  case MMGSDI_SAP_RESET_CNF:
      rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_sap_reset_cnf_type));
    break;

  case MMGSDI_CARD_PUP_CNF:
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_card_pup_cnf_type));
    break;

  case MMGSDI_CARD_PDOWN_CNF:
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_card_pdown_cnf_type));
    break;

  case MMGSDI_CARD_STATUS_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_status_cnf_type));
    break;

  case MMGSDI_REHABILITATE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_rehabilitate_cnf_type));
    break;

  case MMGSDI_INVALIDATE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_invalidate_cnf_type));
    break;

  case MMGSDI_REFRESH_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_refresh_cnf_type));
    break;

  case MMGSDI_PIN_OPERATION_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_pin_operation_cnf_type));
    break;

  case MMGSDI_GET_ALL_PIN_STATUS_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_get_all_pin_status_cnf_type));
    break;

  case MMGSDI_SRV_AVAILABLE_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_srv_available_cnf_type));
    break;

  case MMGSDI_USIM_AUTH_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_usim_auth_cnf_type));
    break;

  case MMGSDI_COMPUTE_IP_AUTH_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_compute_ip_auth_cnf_type)) +
        ((mmgsdi_compute_ip_auth_cnf_type*)cnf_ptr)->cmpt_ip_response_data.data_len;
    break;

  case MMGSDI_RUN_CAVE_CNF:
#ifdef FEATURE_MMGSDI_3GPP2
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_run_cave_cnf_type));
#endif /* FEATURE_MMGSDI_3GPP2 */
    break;

  case MMGSDI_CDMA_AKA_CNF:
#ifdef FEATURE_MMGSDI_3GPP2
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_cdma_aka_cnf_type));
    if (((mmgsdi_cdma_aka_cnf_type*)cnf_ptr)->sync_fail_tag == MMGSDI_CDMA_AKA_VALID_SEQ_NUM)
    {
      rsp_hdr_ptr->response_len +=
        ((mmgsdi_cdma_aka_cnf_type*)cnf_ptr)->aka_data.aka_keys.res.data_len;
    }
#endif /* FEATURE_MMGSDI_3GPP2 */
    break;

  case MMGSDI_GENERATE_KEY_VPM_CNF:
#ifdef FEATURE_MMGSDI_3GPP2
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_generate_key_vpm_cnf_type)) +
        ((mmgsdi_generate_key_vpm_cnf_type*)cnf_ptr)->octet_data.data_len;
#endif /* FEATURE_MMGSDI_3GPP2 */
    break;

  case MMGSDI_GET_ATR_CNF:
    rsp_hdr_ptr->response_len =
        uint32toint32(sizeof(mmgsdi_get_atr_cnf_type)) +
        ((mmgsdi_get_atr_cnf_type*)cnf_ptr)->atr_data.data_len;
    break;

  case MMGSDI_CARD_RESET_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_card_reset_cnf_type));
    break;

  case MMGSDI_BCAST_CNF:
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_bcast_cnf_type));
    break;

  case MMGSDI_SEARCH_CNF:
    if (cnf_ptr == NULL)
    {
      UIM_MSG_ERR_0("Can't assign rsp_hdr_ptr->response_len: Search Cnf null");
      return;
    }
    rsp_hdr_ptr->response_len = uint32toint32(sizeof(mmgsdi_search_cnf_type)) +
      ((mmgsdi_search_cnf_type*)cnf_ptr)->searched_record_nums.data_len;
    break;

  case MMGSDI_SESSION_RUN_GSM_ALGO_CNF:
    rsp_hdr_ptr->response_len =
                 uint32toint32(sizeof(mmgsdi_session_run_gsm_algo_cnf_type));
    break;

  case MMGSDI_SESSION_GET_INFO_CNF:
    rsp_hdr_ptr->response_len =
                     uint32toint32(sizeof(mmgsdi_session_get_info_cnf_type));
    break;

  case MMGSDI_SESSION_READ_PRL_CNF:
    rsp_hdr_ptr->response_len =
                     uint32toint32(sizeof(mmgsdi_session_read_prl_cnf_type));
    break;

  case MMGSDI_SESSION_STORE_ESN_CNF:
    rsp_hdr_ptr->response_len =
                    uint32toint32(sizeof(mmgsdi_session_store_esn_cnf_type));
    break;

  case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CNF:
    rsp_hdr_ptr->response_len =
       uint32toint32(sizeof(mmgsdi_session_prov_app_init_complete_cnf_type));
    break;

  case MMGSDI_SESSION_ENABLE_SERVICE_CNF:
    rsp_hdr_ptr->response_len =
               uint32toint32(sizeof(mmgsdi_session_enable_service_cnf_type));
    break;

  case MMGSDI_SESSION_DISABLE_SERVICE_CNF:
    rsp_hdr_ptr->response_len =
              uint32toint32(sizeof(mmgsdi_session_disable_service_cnf_type));
    break;

  case MMGSDI_SESSION_SSD_UPDATE_CNF:
    rsp_hdr_ptr->response_len =
              uint32toint32(sizeof(mmgsdi_session_ssd_update_cnf_type));
    break;

  case MMGSDI_SESSION_SSD_CONFIRM_CNF:
    rsp_hdr_ptr->response_len =
              uint32toint32(sizeof(mmgsdi_session_ssd_confirm_cnf_type));
    break;

  case MMGSDI_SESSION_BS_CHAL_CNF:
    rsp_hdr_ptr->response_len =
              uint32toint32(sizeof(mmgsdi_session_bs_chal_cnf_type));
    break;

  case MMGSDI_SESSION_OTASP_MS_KEY_CNF:
    rsp_hdr_ptr->response_len =
              uint32toint32(sizeof(mmgsdi_session_otasp_ms_key_cnf_type));
    break;

  case MMGSDI_SESSION_OTASP_KEY_GEN_CNF:
    rsp_hdr_ptr->response_len =
              uint32toint32(sizeof(mmgsdi_session_otasp_key_gen_cnf_type));
    break;

  case MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF:
    rsp_hdr_ptr->response_len =
          uint32toint32(sizeof(mmgsdi_session_otasp_sspr_config_cnf_type));
    break;

  case MMGSDI_SESSION_OTASP_COMMIT_CNF:
    rsp_hdr_ptr->response_len =
              uint32toint32(sizeof(mmgsdi_session_otasp_commit_cnf_type));
    break;

  case MMGSDI_SESSION_OTASP_OTAPA_CNF:
    rsp_hdr_ptr->response_len =
              uint32toint32(sizeof(mmgsdi_session_otasp_otapa_cnf_type));
    break;

  case MMGSDI_SESSION_OTASP_BLOCK_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_session_otasp_block_cnf_type)) +
      ((mmgsdi_session_otasp_block_cnf_type*)cnf_ptr)->block_data.data_len;
    break;

  case MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_session_act_or_switch_prov_cnf_type));
    break;

  case MMGSDI_SESSION_ENABLE_FDN_CNF:
  case MMGSDI_SESSION_DISABLE_FDN_CNF:
    return;

  case MMGSDI_SESSION_SUBSCRIPTION_OK_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_session_subscription_ok_cnf_type));
    break;

  case MMGSDI_SESSION_GET_OPERATOR_NAME_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_session_get_operator_name_cnf_type));
    break;

  case MMGSDI_GET_SE13_PLMN_NAMES_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_get_se13_plmn_names_cnf_type));
    break;

  case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_get_se13_plmn_info_by_name_cnf_type));
    break;

  case MMGSDI_SESSION_REGISTER_FILE_UPDATE_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_session_reg_file_update_cnf_type));
    break;

  case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_session_update_acsgl_record_cnf_type));
    break;

  case MMGSDI_SET_BUILTIN_PLMN_LIST_CNF:
    rsp_hdr_ptr->response_len =
      uint32toint32(sizeof(mmgsdi_set_builtin_plmn_list_cnf_type));
    break;

  case MMGSDI_SESSION_GET_CPHS_INFO_CNF:
  case MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF:
  case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_CNF:
  case MMGSDI_CONTINUE_AFTER_NV_REFRESH_CNF:
  case MMGSDI_HANDLE_NV_REFRESH_CNF:
  case MMGSDI_SESSION_PERSO_CNF:
  case MMGSDI_SAP_CARD_READER_STATUS_CNF:
  case MMGSDI_RESTRICTED_SIM_ACCESS_CNF:
  case MMGSDI_MANAGE_CHANNEL_CNF:
  case MMGSDI_SET_SIM_PROFILE_CNF:
  case MMGSDI_GET_SIM_PROFILE_CNF:
  case MMGSDI_MAX_CNF_ENUM:
    return;
  /* Note - There is no default case here and it is done on purpose since
     we need to handle all values for mmgsdi_cnf_enum_type. Unhandled
     values will get flagged by lint error */
  }

  rsp_hdr_ptr->payload_len = rsp_hdr_ptr->response_len -
    uint32toint32(sizeof(mmgsdi_response_header_type));
  rsp_hdr_ptr->response_type          = cnf_cmd;
  rsp_hdr_ptr->slot_id                = req_hdr_ptr->slot_id;
  rsp_hdr_ptr->client_data            = req_hdr_ptr->client_data;
  rsp_hdr_ptr->status_word.present    = sw_present;
  rsp_hdr_ptr->status_word.sw1        = status_word.sw1;
  rsp_hdr_ptr->status_word.sw2        = status_word.sw2;
} /* mmgsdi_util_populate_cnf_rsp_header */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_SLOT

   DESCRIPTION:
     This function converts the mmgsdi slot type into uim slot type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_uim_slot(
  mmgsdi_slot_id_enum_type  mmgsdi_slot,
  uim_slot_type            *uim_slot_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(uim_slot_ptr);
  switch(mmgsdi_slot)
  {
    case MMGSDI_SLOT_1:
      *uim_slot_ptr = UIM_SLOT_1;
      break;
    case MMGSDI_SLOT_2:
      *uim_slot_ptr = UIM_SLOT_2;
      break;
    case MMGSDI_SLOT_3:
      *uim_slot_ptr = UIM_SLOT_3;
      break;
    case MMGSDI_SLOT_AUTOMATIC:
      *uim_slot_ptr = UIM_SLOT_AUTOMATIC;
      break;
    default:
      *uim_slot_ptr = UIM_SLOT_NONE;
      MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid mmgsdi slot id 0x%x", mmgsdi_slot);
      break;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_convert_uim_slot */


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
)
{
  gstk_slot_id_enum_type  gstk_slot = GSTK_SLOT_ID_MAX;

  switch(mmgsdi_slot)
  {
    case MMGSDI_SLOT_1:
      gstk_slot = GSTK_SLOT_1;
      break;
    case MMGSDI_SLOT_2:
      gstk_slot = GSTK_SLOT_2;
      break;
    case MMGSDI_SLOT_3:
      gstk_slot = GSTK_SLOT_3;
      break;
    case MMGSDI_SLOT_AUTOMATIC:
    default:
      MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid mmgsdi slot id 0x%x", mmgsdi_slot);
      break;
  }
  return gstk_slot;
}/* mmgsdi_util_convert_gstk_slot */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_CHANNEL

   DESCRIPTION:
     This function converts the mmgsdi channel type into uim channel type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_uim_channel(
  mmgsdi_channel_id_type  mmgsdi_channel,
  uim_channel_type       *uim_channel_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(uim_channel_ptr);
  if(mmgsdi_channel < UIM_MAX_CHANNELS)
  {
    *uim_channel_ptr = (uim_channel_type)mmgsdi_channel;
    return MMGSDI_SUCCESS;
  }

  MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid mmgsdi channel id 0x%x",
                               mmgsdi_channel);
  return MMGSDI_ERROR;
}/* mmgsdi_util_convert_uim_channel */


/* ==========================================================================
   FUNCTION:      MMGSDI_BUILD_UIM_RSP

   DESCRIPTION:
     This function will copy info from the uim report type and put it into the
     MMGSDI command queue and set the signal for RESPONSE processing.

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:

==========================================================================*/
static mmgsdi_return_enum_type  mmgsdi_build_uim_rsp (
  mmgsdi_uim_report_rsp_type **msg_pptr,
  const uim_rpt_type          *report_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_SUCCESS;
  uint32                            rsp_data_len  = 0;
  uint8                             index         = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(report_ptr, msg_pptr);

  /* Memory allocation for msg_pptr and check for allocation */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*msg_pptr,
    sizeof(mmgsdi_uim_report_rsp_type));
  if ((*msg_pptr) == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    Populating mmgsdi_uim_report_rsp_type header
      1) Client ID = 0
      2) Request TYPE
      3) Slot ID: Set the slot to automatic since the info is inside the user
                  data
      4) Client Data Pointer: The previous request
      5) Response CallBack
      6) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  (*msg_pptr)->request_header.client_id          = 0;
  (*msg_pptr)->request_header.request_type       = MMGSDI_UIM_REPORT_RSP;
  (*msg_pptr)->request_header.orig_request_type  = MMGSDI_UIM_REPORT_RSP;
  (*msg_pptr)->request_header.slot_id            = MMGSDI_SLOT_1;
  (*msg_pptr)->request_header.client_data        = 0;
  (*msg_pptr)->request_header.response_cb        = NULL;
  (*msg_pptr)->rsp_data.data_len                 = 0;
  (*msg_pptr)->rsp_data.data_ptr                 = NULL;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_uim_report_rsp_type content
    - the command was successed or failed
    - report code
    - status words
    - command transacted or not
    - response data: index into mmgsdi client req table
  ---------------------------------------------------------------------------*/
  if (report_ptr->rpt_status == UIM_PASS)
  {
    (*msg_pptr)->is_uim_success = TRUE;
    switch (report_ptr->rpt_type)
    {
    case UIM_SELECT_R:
      (*msg_pptr)->rsp_data.data_len              =
         report_ptr->rpt.select.num_data_bytes;
      if(report_ptr->rpt.select.num_data_bytes > 0)
      {
        /* memory allocation */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
          report_ptr->rpt.select.num_data_bytes);

        if ((*msg_pptr)->rsp_data.data_ptr)
        {
          mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                         report_ptr->rpt.select.num_data_bytes,
                         report_ptr->rpt.select.data,
                         report_ptr->rpt.select.num_data_bytes);
        }
      }
      (*msg_pptr)->get_resp_status_word.sw1 = (uint8)report_ptr->rpt.select.get_resp_sw1;
      (*msg_pptr)->get_resp_status_word.sw2 = (uint8)report_ptr->rpt.select.get_resp_sw2;
      break;

    case UIM_READ_BIN_CMD_R:
    case UIM_READ_REC_CMD_R:
    case UIM_INC_CMD_R:
      (*msg_pptr)->rsp_data.data_len =
         report_ptr->rpt.read.num_bytes_read;
      if (report_ptr->rpt.read.num_bytes_read > 0)
      {
        /* memory allocation */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
           report_ptr->rpt.read.num_bytes_read);

        if ((*msg_pptr)->rsp_data.data_ptr)
        {
          mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                         report_ptr->rpt.read.num_bytes_read,
                         report_ptr->rpt.read.data,
                         report_ptr->rpt.read.num_bytes_read);
        }
      }
      break;

    case UIM_STATUS_R:
      (*msg_pptr)->rsp_data.data_len =
         report_ptr->rpt.status.num_bytes;
      if (report_ptr->rpt.status.num_bytes > 0)
      {
        /* memory allocation */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
           report_ptr->rpt.status.num_bytes);

        if ((*msg_pptr)->rsp_data.data_ptr)
        {
          mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                         report_ptr->rpt.status.num_bytes,
                         report_ptr->rpt.status.data,
                         report_ptr->rpt.status.num_bytes);
        }
      }
      break;

    case UIM_MANAGE_CHANNEL_R:
      (*msg_pptr)->rsp_data.data_len = 1;
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr, 1);

      if ((*msg_pptr)->rsp_data.data_ptr) {
        (*msg_pptr)->rsp_data.data_ptr[0] = report_ptr->rpt.channel.channel_num;
      }
      break;
    case UIM_COMPUTE_IP_AUTH_R:
      (*msg_pptr)->rsp_data.data_len = report_ptr->rpt.compute_ip_auth.data_len;
      if ((*msg_pptr)->rsp_data.data_len > 0)
      {
        /* memory allocation */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                           (*msg_pptr)->rsp_data.data_len);

        if ((*msg_pptr)->rsp_data.data_ptr)
        {
          mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                         (*msg_pptr)->rsp_data.data_len,
                         report_ptr->rpt.compute_ip_auth.data,
                         report_ptr->rpt.compute_ip_auth.data_len);
        }
      }
      break;

    case UIM_RUN_CAVE_R:
#ifdef FEATURE_MMGSDI_3GPP2
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         sizeof(dword));
      if((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len = 0;
      }
      else if ((*msg_pptr)->rsp_data.data_ptr)
      {
        (*msg_pptr)->rsp_data.data_len = sizeof(dword);
        /*Pack the dword into uint8*/
        b_packd(report_ptr->rpt.run_cave.auth,
               (*msg_pptr)->rsp_data.data_ptr, 0,
               sizeof(dword)*8);
      }
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    case UIM_AKA_AUTH_R:
#ifdef FEATURE_MMGSDI_3GPP2
      if (report_ptr->rpt.aka_auth.sync_tag == MMGSDI_CDMA_AKA_VALID_SEQ_NUM)
      {
        /* CDMA AKA AUTH RESP response
           sync tag          1 byte
           cipher key        16 bytes
           integrity key     16 bytes
           result length     1 byte
           result            x bytes*/
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                           sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.cipher_key) +
                                           sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.integrity_key) +
                                           report_ptr->rpt.aka_auth.aka_data.aka_keys.res_length +
                                           MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN +
                                           MMGSDI_CDMA_AKA_AUTH_RES_LEN_SIZE);
        if ((*msg_pptr)->rsp_data.data_ptr == NULL)
        {
          mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
          (*msg_pptr)->rsp_data.data_len       = 0;
          (*msg_pptr)->is_uim_success          = FALSE;
          (*msg_pptr)->mmgsdi_error_status     = mmgsdi_status;
          break;
        }

        (*msg_pptr)->rsp_data.data_ptr[0]    = report_ptr->rpt.aka_auth.sync_tag;

        memscpy((byte *)((*msg_pptr)->rsp_data.data_ptr +
                MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN),
                sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.cipher_key),
                report_ptr->rpt.aka_auth.aka_data.aka_keys.cipher_key,
                sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.cipher_key));

        memscpy((byte *)((*msg_pptr)->rsp_data.data_ptr +
                MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN       +
                sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.cipher_key)),
                sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.integrity_key),
                report_ptr->rpt.aka_auth.aka_data.aka_keys.integrity_key,
                sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.integrity_key));

        (*msg_pptr)->rsp_data.data_ptr[MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN        +
                  sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.cipher_key) +
                  sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.integrity_key)]
                  = (byte)report_ptr->rpt.aka_auth.aka_data.aka_keys.res_length;
        memscpy((byte *)((*msg_pptr)->rsp_data.data_ptr                          +
                MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN                                +
                sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.cipher_key)    +
                sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.integrity_key) +
                MMGSDI_CDMA_AKA_AUTH_RES_LEN_SIZE),
                report_ptr->rpt.aka_auth.aka_data.aka_keys.res_length,
                report_ptr->rpt.aka_auth.aka_data.aka_keys.res_data,
                report_ptr->rpt.aka_auth.aka_data.aka_keys.res_length);

        (*msg_pptr)->rsp_data.data_len = MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN         +
                  sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.cipher_key)    +
                  sizeof(report_ptr->rpt.aka_auth.aka_data.aka_keys.integrity_key) +
                  MMGSDI_CDMA_AKA_AUTH_RES_LEN_SIZE                                +
                  report_ptr->rpt.aka_auth.aka_data.aka_keys.res_length;
      }
      else if (report_ptr->rpt.aka_auth.sync_tag == MMGSDI_CDMA_AKA_INVALID_SEQ_NUM)
      {
        /* CDMA AKA AUTH RESP with invalid sequence number
           sync tag      1 byte
           AUTS data     14 bytes  */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                           sizeof(report_ptr->rpt.aka_auth.aka_data.auts) +
                                           MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN);
        if ((*msg_pptr)->rsp_data.data_ptr == NULL)
        {
          mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
          (*msg_pptr)->rsp_data.data_len       = 0;
          (*msg_pptr)->is_uim_success          = FALSE;
          (*msg_pptr)->mmgsdi_error_status     = mmgsdi_status;
          break;
        }

        (*msg_pptr)->rsp_data.data_ptr[0]    = report_ptr->rpt.aka_auth.sync_tag;

        memscpy((byte *)((*msg_pptr)->rsp_data.data_ptr +
                MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN),
                sizeof(report_ptr->rpt.aka_auth.aka_data.auts),
                report_ptr->rpt.aka_auth.aka_data.auts,
                sizeof(report_ptr->rpt.aka_auth.aka_data.auts));

        (*msg_pptr)->rsp_data.data_len = MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN +
                  sizeof(report_ptr->rpt.aka_auth.aka_data.auts);
      }
      else
      {
        (*msg_pptr)->rsp_data.data_len       = 0;
        (*msg_pptr)->is_uim_success          = FALSE;
        (*msg_pptr)->mmgsdi_error_status     = MMGSDI_INCORRECT_PARAMS;
      }
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    case UIM_GENERATE_KEYS_R:
#ifdef FEATURE_MMGSDI_3GPP2
      (*msg_pptr)->rsp_data.data_len = UIM_MAX_CHARS;
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         UIM_MAX_CHARS);

      if((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len = 0;
      }
      else if ((*msg_pptr)->rsp_data.data_ptr)
      {
        mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                       UIM_MAX_CHARS,
                       report_ptr->rpt.generate_keys.data,
                       UIM_MAX_CHARS);
      }
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    case UIM_SEEK_R:
      (*msg_pptr)->rsp_data.data_len = report_ptr->rpt.seek.num_records;
      if(report_ptr->rpt.seek.num_records > 0)
      {
        /* memory allocation */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
          report_ptr->rpt.seek.num_records);

        if ((*msg_pptr)->rsp_data.data_ptr)
        {
          mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                         report_ptr->rpt.seek.num_records,
                         report_ptr->rpt.seek.data,
                         report_ptr->rpt.seek.num_records);
        }
      }
      break;
    case UIM_RUN_GSM_ALGO_R:
      (*msg_pptr)->rsp_data.data_len = report_ptr->rpt.run_gsm.data_length;
      if (report_ptr->rpt.run_gsm.data_length > 0)
      {
        /* memory allocation */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
          report_ptr->rpt.run_gsm.data_length);
        if ((*msg_pptr)->rsp_data.data_ptr == NULL)
        {
          mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
          (*msg_pptr)->rsp_data.data_len = 0;
          (*msg_pptr)->is_uim_success = FALSE;
          (*msg_pptr)->mmgsdi_error_status = mmgsdi_status;
        }
        else
        {
          mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                         report_ptr->rpt.run_gsm.data_length,
                         report_ptr->rpt.run_gsm.data,
                         report_ptr->rpt.run_gsm.data_length);
        }
      }
      break;

    case UIM_STORE_ESN_MEID_ME_R:
    case UIM_STORE_ESN_ME_R:
      /* 1 byte of usage indicator */
      (*msg_pptr)->rsp_data.data_len =
        sizeof(report_ptr->rpt.store_esn.usage_ind_conf);
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len = 0;
        (*msg_pptr)->is_uim_success = FALSE;
        (*msg_pptr)->mmgsdi_error_status = mmgsdi_status;
      }
      else
      {
        mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                       (*msg_pptr)->rsp_data.data_len,
                       &report_ptr->rpt.store_esn.usage_ind_conf,
                       sizeof(report_ptr->rpt.store_esn.usage_ind_conf));
      }
      break;

    case UIM_BS_CHAL_R:
      (*msg_pptr)->rsp_data.data_len         =  sizeof(report_ptr->rpt.bs_chal);
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len       =  0;
        (*msg_pptr)->is_uim_success          =  FALSE;
        (*msg_pptr)->mmgsdi_error_status     =  mmgsdi_status;
      }
      else
      {
        /* BS CHAL response
           randbs         4 bytes  */
        b_packd( report_ptr->rpt.bs_chal.randbs,
                 ((*msg_pptr)->rsp_data.data_ptr),
                 0,
                 sizeof(dword)*8);
        (*msg_pptr)->rsp_data.data_len       = MMGSDI_BS_CHAL_RSP_FIX_LEN;
      }
      break;

    case UIM_MS_KEY_REQ_R:
      (*msg_pptr)->rsp_data.data_len         = sizeof(report_ptr->rpt.ms_key);
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len       = 0;
        (*msg_pptr)->is_uim_success          = FALSE;
        (*msg_pptr)->mmgsdi_error_status     = mmgsdi_status;
      }
      else
      {
        /* OTAPA MS KEY response
           Result Code        1 byte */
        (*msg_pptr)->rsp_data.data_ptr[0]    = report_ptr->rpt.ms_key.result;
        (*msg_pptr)->rsp_data.data_len       = MMGSDI_OTASP_MS_KEY_RSP_FIX_LEN;
      }
      break;

    case UIM_KEY_GEN_REQ_R:
      (*msg_pptr)->rsp_data.data_len         = sizeof(report_ptr->rpt.key_gen);
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len       = 0;
        (*msg_pptr)->is_uim_success          = FALSE;
        (*msg_pptr)->mmgsdi_error_status     = mmgsdi_status;
      }
      else
      {
        if(report_ptr->rpt.key_gen.ms_result_len >
           sizeof(report_ptr->rpt.key_gen.ms_result))
        {
          (*msg_pptr)->rsp_data.data_len       = 0;
          (*msg_pptr)->is_uim_success          = FALSE;
          (*msg_pptr)->mmgsdi_error_status     = MMGSDI_INCORRECT_PARAMS;
        }
        else
        {
          /* OTAPA KEY GEN response
             Result Code        1 byte
             MS Result length   1 byte
             MS Result data     x bytes  */
          (*msg_pptr)->rsp_data.data_ptr[0]    = report_ptr->rpt.key_gen.result;
          (*msg_pptr)->rsp_data.data_ptr[1]    = report_ptr->rpt.key_gen.ms_result_len;
          mmgsdi_memscpy(
            (byte *)((*msg_pptr)->rsp_data.data_ptr +
                                              MMGSDI_OTASP_KEY_GEN_RSP_FIX_LEN),
            (*msg_pptr)->rsp_data.data_len - MMGSDI_OTASP_KEY_GEN_RSP_FIX_LEN,
            report_ptr->rpt.key_gen.ms_result,
            report_ptr->rpt.key_gen.ms_result_len);

          (*msg_pptr)->rsp_data.data_len       = MMGSDI_OTASP_KEY_GEN_RSP_FIX_LEN +
                                                report_ptr->rpt.key_gen.ms_result_len ;
        }
      }
      break;

    case UIM_SSPR_CONFIG_REQ_R :
      (*msg_pptr)->rsp_data.data_len         = sizeof(report_ptr->rpt.sspr_config);
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len       = 0;
        (*msg_pptr)->is_uim_success          = FALSE;
        (*msg_pptr)->mmgsdi_error_status     = mmgsdi_status;
      }
      else
      {
        if(report_ptr->rpt.sspr_config.block_length >
           sizeof(report_ptr->rpt.sspr_config.data))
        {
          (*msg_pptr)->rsp_data.data_len       = 0;
          (*msg_pptr)->is_uim_success          = FALSE;
          (*msg_pptr)->mmgsdi_error_status     = MMGSDI_INCORRECT_PARAMS;
        }
        else
        {
          /* OTASP SSPR Config response
             Block ID         1 byte
             Result Code      1 byte
             Block Length     1 byte
             Param Data       X bytes   */
          (*msg_pptr)->rsp_data.data_ptr[0]    = report_ptr->rpt.sspr_config.block_id;
          (*msg_pptr)->rsp_data.data_ptr[1]    = report_ptr->rpt.sspr_config.result;
          (*msg_pptr)->rsp_data.data_ptr[2]    = report_ptr->rpt.sspr_config.block_length;
          mmgsdi_memscpy(
            (byte *)((*msg_pptr)->rsp_data.data_ptr +
                                         MMGSDI_OTASP_SSPR_CONFIG_RSP_FIX_LEN),
            (*msg_pptr)->rsp_data.data_len - MMGSDI_OTASP_SSPR_CONFIG_RSP_FIX_LEN,
            report_ptr->rpt.sspr_config.data,
            report_ptr->rpt.sspr_config.block_length);

          (*msg_pptr)->rsp_data.data_len       = MMGSDI_OTASP_SSPR_CONFIG_RSP_FIX_LEN +
                                                report_ptr->rpt.sspr_config.block_length;
        }
      }
      break;

    case UIM_OTAPA_REQ_R:
      (*msg_pptr)->rsp_data.data_len = sizeof(report_ptr->rpt.otapa);
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len = 0;
        (*msg_pptr)->is_uim_success = FALSE;
        (*msg_pptr)->mmgsdi_error_status = mmgsdi_status;
      }
      else
      {
        /* OTAPA response
           Result Code        1 byte
           NAM_LOCK Indicator 1 byte
           RAND OTAPA         4 bytes  */
        (*msg_pptr)->rsp_data.data_ptr[0] =report_ptr->rpt.otapa.result;
        (*msg_pptr)->rsp_data.data_ptr[1] =report_ptr->rpt.otapa.nam_lock_ind;
        b_packd(report_ptr->rpt.otapa.rand_otapa,
                ((*msg_pptr)->rsp_data.data_ptr+2),
                0,
                sizeof(dword)*8);
        (*msg_pptr)->rsp_data.data_len = MMGSDI_OTASP_OTAPA_RSP_FIX_LEN;
      }
      break;

    case UIM_VALIDATE_R:
      (*msg_pptr)->rsp_data.data_len       = sizeof(report_ptr->rpt.validate);

      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len     = 0;
        (*msg_pptr)->is_uim_success        = FALSE;
        (*msg_pptr)->mmgsdi_error_status   = mmgsdi_status;
      }
      else
      {
        /* UIM Validate Response
           Block ID    1 byte
           Result Code 1 byte      */
        (*msg_pptr)->rsp_data.data_ptr[0]  = report_ptr->rpt.validate.block_id;
        (*msg_pptr)->rsp_data.data_ptr[1]  = report_ptr->rpt.validate.result;
        (*msg_pptr)->rsp_data.data_len     = MMGSDI_OTASP_VALIDATE_RSP_FIX_LEN;
      }
      break;

    case UIM_DOWNLOAD_REQ_R:
      (*msg_pptr)->rsp_data.data_len       = sizeof(report_ptr->rpt.download);
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len     = 0;
        (*msg_pptr)->is_uim_success        = FALSE;
        (*msg_pptr)->mmgsdi_error_status   = mmgsdi_status;
      }
      else
      {
      /* UIM Download Response
         Block ID         1 byte
         Result Code      1 byte        */
        (*msg_pptr)->rsp_data.data_ptr[0]  = report_ptr->rpt.download.block_id;
        (*msg_pptr)->rsp_data.data_ptr[1]  = report_ptr->rpt.download.result;
        (*msg_pptr)->rsp_data.data_len     = MMGSDI_OTASP_DOWNLOAD_RSP_FIX_LEN;
      }
      break;

    case UIM_CONFIG_REQ_R:
      (*msg_pptr)->rsp_data.data_len    = sizeof(report_ptr->rpt.configuration);
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len   = 0;
        (*msg_pptr)->is_uim_success      = FALSE;
        (*msg_pptr)->mmgsdi_error_status = mmgsdi_status;
      }
      else
      {
        if(report_ptr->rpt.configuration.block_length >
           sizeof(report_ptr->rpt.configuration.data))
        {
          (*msg_pptr)->rsp_data.data_len       = 0;
          (*msg_pptr)->is_uim_success          = FALSE;
          (*msg_pptr)->mmgsdi_error_status     = MMGSDI_INCORRECT_PARAMS;
        }
        else
        {
          /* UIM Config Response
             Block ID         1 byte
             Block Length     1 byte
             Result Code      1 byte
             Block parm Data  depend on the block length.
          */
          (*msg_pptr)->rsp_data.data_ptr[0]  =
                                     report_ptr->rpt.configuration.block_id;
          (*msg_pptr)->rsp_data.data_ptr[1]  =
                                     report_ptr->rpt.configuration.block_length;
          (*msg_pptr)->rsp_data.data_ptr[2]  =
                                     report_ptr->rpt.configuration.result;
          mmgsdi_memscpy((byte *)((*msg_pptr)->rsp_data.data_ptr +
                                     MMGSDI_OTASP_CONFIG_RSP_FIX_LEN),
            (*msg_pptr)->rsp_data.data_len - MMGSDI_OTASP_CONFIG_RSP_FIX_LEN,
            report_ptr->rpt.configuration.data,
            report_ptr->rpt.configuration.block_length);
          (*msg_pptr)->rsp_data.data_len     = MMGSDI_OTASP_CONFIG_RSP_FIX_LEN +
                                     report_ptr->rpt.configuration.block_length;
        }
      }
      break;

    case UIM_SSPR_DOWNLOAD_REQ_R:
      (*msg_pptr)->rsp_data.data_len       =
                                       sizeof(report_ptr->rpt.sspr_download);
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len     = 0;
        (*msg_pptr)->is_uim_success        = FALSE;
        (*msg_pptr)->mmgsdi_error_status   = mmgsdi_status;
      }
      else
      {
        /* UIM SSPR Download Response
           Block   ID       1 byte
           Result  Code     1 byte
           Segment Offset   2 bytes
           Segment Size     1 byte
        */
        (*msg_pptr)->rsp_data.data_ptr[0]  =
                                   report_ptr->rpt.sspr_download.block_id;
        (*msg_pptr)->rsp_data.data_ptr[1]  =
                                   report_ptr->rpt.sspr_download.result;
        b_packw( report_ptr->rpt.sspr_download.segment_offset,
                 ((*msg_pptr)->rsp_data.data_ptr + 2),
                 0,
                 sizeof(word)*8);
        (*msg_pptr)->rsp_data.data_ptr[4]  =
                                   report_ptr->rpt.sspr_download.segment_size;
        (*msg_pptr)->rsp_data.data_len     =
                                   MMGSDI_OTASP_SSPR_DOWNLOAD_RSP_FIX_LEN;
      }
      break;

    case UIM_COMMIT_R:
      (*msg_pptr)->rsp_data.data_len         = MMGSDI_OTASP_COMMIT_RSP_FIX_LEN;
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);
      if ((*msg_pptr)->rsp_data.data_ptr == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        (*msg_pptr)->rsp_data.data_len       = 0;
        (*msg_pptr)->is_uim_success          = FALSE;
      }
      else
      {
        /* OTAPA COMMIT response
           Result Code        1 byte */
        (*msg_pptr)->rsp_data.data_ptr[0]    = report_ptr->rpt.commit.result;
      }
      break;


    default:
      UIM_MSG_HIGH_1("Other UIM Response 0x%x", report_ptr->rpt_type);
      break;
    }
  }

  /* No matter success or failure, the card might return data back */
  if (report_ptr->rpt_type == UIM_ISIM_AUTHENTICATE_R)
  {
    (*msg_pptr)->rsp_data.data_len              =
      report_ptr->rpt.isim_autn.data_length;
    if (report_ptr->rpt.isim_autn.data_length > 0)
    {
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
        report_ptr->rpt.isim_autn.data_length);
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
      if ((*msg_pptr)->rsp_data.data_ptr)
      {
        mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                       report_ptr->rpt.isim_autn.data_length,
                       report_ptr->rpt.isim_autn.data,
                       report_ptr->rpt.isim_autn.data_length);
      }
    }
  }

    /* No matter success or failure, the card might return data back */
  if (report_ptr->rpt_type == UIM_AUTHENTICATE_R)
  {
    (*msg_pptr)->rsp_data.data_len = report_ptr->rpt.autn.data_length;
    if (report_ptr->rpt.autn.data_length > 0)
    {
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
        report_ptr->rpt.autn.data_length);

      if((*msg_pptr)->rsp_data.data_ptr != NULL)
      {
        mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                       report_ptr->rpt.autn.data_length,
                       report_ptr->rpt.autn.data,
                       report_ptr->rpt.autn.data_length);
      }
      else
      {
        (*msg_pptr)->rsp_data.data_len = 0;
      }
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
    }
  }

  else if (report_ptr->rpt_type == UIM_AUTHENTICATE_ODD_INS_R)
  {
    (*msg_pptr)->rsp_data.data_len = report_ptr->rpt.autn_odd_ins.len;
    if (report_ptr->rpt.autn_odd_ins.len > 0)
    {
      uint8  buff_offset = 0;
      uint32 i           = 0;

      /*Is it a Response for MIKEY*/
      if(report_ptr->rpt.autn_odd_ins.data[0] == MMGSDI_MIKEY_RESPONSE ||
         report_ptr->rpt.autn_odd_ins.data[0] == MMGSDI_MBMS_BER_TLV_TAG)
      {
        uint8 mikey_len_length  = 1;
        buff_offset = 1; /*Skip the Tag*/

        /*Length of the TLV Object */
        mikey_len_length = (report_ptr->rpt.autn_odd_ins.data[1] & 0x80) == 0x80 ?
                      (report_ptr->rpt.autn_odd_ins.data[1] & 0x0F) : 0;
        buff_offset += (report_ptr->rpt.autn_odd_ins.data[1] & 0x80) == 0x80 ?
                       1 : 0;

        (*msg_pptr)->rsp_data.data_len =
              report_ptr->rpt.autn_odd_ins.data[buff_offset];

        for (i = 1; i < mikey_len_length; i++)
        {
           (*msg_pptr)->rsp_data.data_len =
              uint32toint32(int32touint32((*msg_pptr)->rsp_data.data_len) << 8);

           buff_offset++;

           (*msg_pptr)->rsp_data.data_len +=
              report_ptr->rpt.autn_odd_ins.data[buff_offset];

        }

      }
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);

      buff_offset++;

      if((*msg_pptr)->rsp_data.data_ptr != NULL)
      {
        mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                       int32touint32((*msg_pptr)->rsp_data.data_len),
                       report_ptr->rpt.autn_odd_ins.data+buff_offset,
                       int32touint32((*msg_pptr)->rsp_data.data_len));
      }
      else
      {
        (*msg_pptr)->rsp_data.data_len = 0;
      }
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
    }
  }
  else if (report_ptr->rpt_type == UIM_BCAST_R)
  {
    (*msg_pptr)->rsp_data.data_len = report_ptr->rpt.bcast.len;
    if (report_ptr->rpt.bcast.len > 0)
    {
      uint8   buff_offset = 0;
      uint32  i           = 0;

      /*Is it a Response for BCAST IN BER_TLV format*/
      if(report_ptr->rpt.bcast.data[0] == MMGSDI_MBMS_BER_TLV_TAG)
      {
        uint8 bcast_len_length  = 1;
        buff_offset = 1; /*Skip the Tag*/

        /*Length of the TLV Object */
        bcast_len_length = (report_ptr->rpt.bcast.data[1] & 0x80) == 0x80 ?
                      (report_ptr->rpt.bcast.data[1] & 0x0F) : 0;
        buff_offset += (report_ptr->rpt.bcast.data[1] & 0x80) == 0x80 ?
                       1 : 0;

        (*msg_pptr)->rsp_data.data_len =
              report_ptr->rpt.bcast.data[buff_offset];

        for (i = 1; i < bcast_len_length; i++)
        {
           (*msg_pptr)->rsp_data.data_len =
              uint32toint32(int32touint32((*msg_pptr)->rsp_data.data_len) << 8);
            buff_offset++;
           (*msg_pptr)->rsp_data.data_len +=
              report_ptr->rpt.bcast.data[buff_offset];

        }
      }
      /* memory allocation */
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                         (*msg_pptr)->rsp_data.data_len);

      if((*msg_pptr)->rsp_data.data_ptr != NULL)
      {
        mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
                       int32touint32((*msg_pptr)->rsp_data.data_len),
                       report_ptr->rpt.bcast.data+buff_offset,
                       int32touint32((*msg_pptr)->rsp_data.data_len));
      }
      else
      {
        (*msg_pptr)->rsp_data.data_len = 0;
      }
      /* reset mmgsdi status */
      mmgsdi_status = MMGSDI_SUCCESS;
    }
  }

  if(report_ptr->rpt_type == UIM_STREAM_ISO7816_APDU_R)
  {
    UIM_MSG_HIGH_6(" Status 0x%x APDU command status words 0x%x 0x%x; Implicit status words 0x%x 0x%x; cmd_transacted 0x%x",
                   report_ptr->rpt_status,
                   report_ptr->sw1,
                   report_ptr->sw2,
                   report_ptr->rpt.stream_iso7816_apdu.get_resp_sw1,
                   report_ptr->rpt.stream_iso7816_apdu.get_resp_sw2,
                   report_ptr->cmd_transacted);

    if(report_ptr->rpt_status == UIM_PASS)
    {
      if (report_ptr->cmd_transacted)
      {
        uint16 stream_apdu_rsp_len = 0;
        uint16 apdu_data_len_to_copy = 0;

         /*
         * Calculate the size of the data buffer that we need.
         */
        if(report_ptr->rpt.stream_iso7816_apdu.data_ptr != NULL)
        {
          stream_apdu_rsp_len =
            report_ptr->rpt.stream_iso7816_apdu.num_data_bytes;
        }

        /* Ensure that there is no integer overflow when calculating  the total
           response length. Truncate length appropriately */
        if (stream_apdu_rsp_len >
            0xFFFF - MMGSDI_GEN_SW_LEN)
        {
          stream_apdu_rsp_len = 0xFFFF - MMGSDI_GEN_SW_LEN;
        }
        apdu_data_len_to_copy = stream_apdu_rsp_len;

        stream_apdu_rsp_len = stream_apdu_rsp_len + MMGSDI_GEN_SW_LEN;

        UIM_MSG_HIGH_2("Stream APDU UIM Response length: 0x%x, MMGSDI data buffer length: 0x%x",
                   report_ptr->rpt.stream_iso7816_apdu.num_data_bytes,
                   stream_apdu_rsp_len);

        /*
         * Memory allocation.
         */
        (*msg_pptr)->rsp_data.data_len = 0;

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                               stream_apdu_rsp_len);

        if ((*msg_pptr)->rsp_data.data_ptr != NULL)
        {
          (*msg_pptr)->rsp_data.data_len = stream_apdu_rsp_len;

          mmgsdi_status = MMGSDI_SUCCESS;
          /*
           * With the addition of FEATURE_MMGSDI_ATCSIM Support, there is
           * now a need to set two additional members of the confirmation.
           * Because of the nature of this function, the data is appended
           * appropriately to the data_p to be stripped out before sending
           * it back in the gsdi_cnf.
           */
          if(report_ptr->rpt.stream_iso7816_apdu.data_ptr != NULL)
          {
            mmgsdi_memscpy((*msg_pptr)->rsp_data.data_ptr,
              stream_apdu_rsp_len,
              report_ptr->rpt.stream_iso7816_apdu.data_ptr,
              apdu_data_len_to_copy);
          }

          /*
           * Now append to the data_p accordingly.
           */
          (*msg_pptr)->rsp_data.data_ptr[stream_apdu_rsp_len-4] =
            (*msg_pptr)->status_word.sw1 = (uint8)report_ptr->sw1;
          (*msg_pptr)->rsp_data.data_ptr[stream_apdu_rsp_len-3] =
            (*msg_pptr)->status_word.sw2 = (uint8)report_ptr->sw2;
          (*msg_pptr)->rsp_data.data_ptr[stream_apdu_rsp_len-2] =
            (byte)report_ptr->rpt.stream_iso7816_apdu.get_resp_sw1;
          (*msg_pptr)->rsp_data.data_ptr[stream_apdu_rsp_len-1] =
            (byte)report_ptr->rpt.stream_iso7816_apdu.get_resp_sw2;
        }
        else
        {
          mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
          (*msg_pptr)->is_uim_success = FALSE;
        }
      }
      else
      {
        (*msg_pptr)->is_uim_success = FALSE;
      }
    }
    else
    {
      if (report_ptr->cmd_transacted)
      {
         /* As long as the transaction was successful, even if the command did
            not pass, return the response APDU status words back to the
            streaming APDU client */
        (*msg_pptr)->rsp_data.data_len = 4;

        rsp_data_len = int32touint32((*msg_pptr)->rsp_data.data_len);

        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*msg_pptr)->rsp_data.data_ptr,
                                           rsp_data_len);

        if ((*msg_pptr)->rsp_data.data_ptr != NULL)
        {
          mmgsdi_status = MMGSDI_SUCCESS;
          (*msg_pptr)->is_uim_success = TRUE;

          (*msg_pptr)->rsp_data.data_ptr[(*msg_pptr)->rsp_data.data_len-4] =
              (*msg_pptr)->status_word.sw1 = (uint8)report_ptr->sw1;
          (*msg_pptr)->rsp_data.data_ptr[(*msg_pptr)->rsp_data.data_len-3] =
              (*msg_pptr)->status_word.sw2 = (uint8)report_ptr->sw2;
          (*msg_pptr)->rsp_data.data_ptr[(*msg_pptr)->rsp_data.data_len-2] =
              (byte)report_ptr->rpt.stream_iso7816_apdu.get_resp_sw1;
          (*msg_pptr)->rsp_data.data_ptr[(*msg_pptr)->rsp_data.data_len-1] =
              (byte)report_ptr->rpt.stream_iso7816_apdu.get_resp_sw2;
        }
        else
        {
          mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
          (*msg_pptr)->rsp_data.data_len = 0;
          (*msg_pptr)->is_uim_success    = FALSE;
        }
      }
      else
      {
        (*msg_pptr)->is_uim_success = FALSE;
      }
    }
  }

  index = MMGSDIUTIL_GET_INDEX_FROM_UIMDRV_USER_DATA(report_ptr->user_data);

  (*msg_pptr)->uim_rpt_code    = (int32)report_ptr->rpt_type;
  (*msg_pptr)->status_word.sw1 = (uint8)report_ptr->sw1;
  (*msg_pptr)->status_word.sw2 = (uint8)report_ptr->sw2;
  (*msg_pptr)->cmd_transacted  = report_ptr->cmd_transacted;
  (*msg_pptr)->rsp_data_index  = (int32)index;

  /*---------------------------------------------------------------------------
    Calculate the payload len and request len
    QUESTION: better way to encapsulate it in the request amd payload len
  ---------------------------------------------------------------------------*/
  (*msg_pptr)->request_header.request_len = uint32toint32(sizeof(mmgsdi_uim_report_rsp_type));
  (*msg_pptr)->request_header.payload_len = (*msg_pptr)->request_header.request_len
                                            - uint32toint32(sizeof(mmgsdi_request_header_type));

  return mmgsdi_status;
} /* mmgsdi_build_uim_rsp */


/* ==========================================================================
   FUNCTION:      MMGSDI_UIM_REPORT

   DESCRIPTION:
     This function is called when UIM report back command response

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_uim_report (
  uim_rpt_type *report_ptr
)
{
  mmgsdi_uim_report_rsp_type *msg_ptr       = NULL;
  mmgsdi_task_cmd_type       *task_cmd_ptr  = NULL;
  int32                       total_mem_len = 0;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_SUCCESS;
  uint32                      task_cmd_len  = 0;
  mmgsdi_task_enum_type       mmgsdi_task   = MMGSDI_TASK_MAX;
  uint8                       index         = 0;

  if (report_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  /* External task. So get the task from user_data */
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    mmgsdi_task = MMGSDIUTIL_GET_TASK_FROM_UIMDRV_USER_DATA(report_ptr->user_data);
    if((mmgsdi_task != MMGSDI_TASK_MAIN)  &&
       (mmgsdi_task != MMGSDI_TASK_SLOT1) &&
       (mmgsdi_task != MMGSDI_TASK_SLOT2) &&
       (mmgsdi_task != MMGSDI_TASK_SLOT3))
    {
      /* This is unreachable code in normal circumstances, if reached, it indicates
         data corruption by the UIM. ERR_FATAL in this case */
      ERR_FATAL("Invalid task in mmgsdi_uim_report user_data", 0, 0, 0);
    }
  }

  index = MMGSDIUTIL_GET_INDEX_FROM_UIMDRV_USER_DATA(report_ptr->user_data);

  if (index >= MMGSDI_MAX_CLIENT_REQ_INFO)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Unknown index in user data 0x%x", index);
    return;
  }
  mmgsdi_status = mmgsdi_build_uim_rsp(&msg_ptr, report_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    if (msg_ptr != NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK((msg_ptr->rsp_data.data_ptr));
    }
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(msg_ptr);
    UIM_MSG_ERR_1("build uim rsp fail 0x%x or msg_ptr null, No rsp sent back to client",
    mmgsdi_status);
    return;
  }
  total_mem_len = uint32toint32(sizeof(mmgsdi_task_cmd_type)) + msg_ptr->rsp_data.data_len;
  task_cmd_len  = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);

  if (task_cmd_ptr == NULL)
  {
    /* Free data */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK((msg_ptr->rsp_data.data_ptr));
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(msg_ptr);
    return;
  }
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.uim_report_rsp, MMGSDI_UIM_REPORT_RSP);

  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_UIM_REPORT_RSP;
  mmgsdi_memscpy(&task_cmd_ptr->cmd.cmd.uim_report_rsp.request_header,
                 sizeof(task_cmd_ptr->cmd.cmd.uim_report_rsp.request_header),
                 &msg_ptr->request_header,
                 sizeof(mmgsdi_request_header_type));
  task_cmd_ptr->cmd.cmd.uim_report_rsp.is_uim_success = msg_ptr->is_uim_success;
  task_cmd_ptr->cmd.cmd.uim_report_rsp.uim_rpt_code = msg_ptr->uim_rpt_code;
  mmgsdi_memscpy(&task_cmd_ptr->cmd.cmd.uim_report_rsp.status_word,
                 sizeof(task_cmd_ptr->cmd.cmd.uim_report_rsp.status_word),
                 &msg_ptr->status_word,
                 sizeof(mmgsdi_sw_status_type));
  mmgsdi_memscpy(&task_cmd_ptr->cmd.cmd.uim_report_rsp.get_resp_status_word,
                 sizeof(task_cmd_ptr->cmd.cmd.uim_report_rsp.get_resp_status_word),
                 &msg_ptr->get_resp_status_word,
                 sizeof(mmgsdi_sw_status_type));
  task_cmd_ptr->cmd.cmd.uim_report_rsp.cmd_transacted = msg_ptr->cmd_transacted;
  task_cmd_ptr->cmd.cmd.uim_report_rsp.rsp_data_index = msg_ptr->rsp_data_index;

  if ((msg_ptr->rsp_data.data_len > 0) && (msg_ptr->rsp_data.data_ptr != NULL))
  {
    mmgsdi_memscpy(task_cmd_ptr->cmd.cmd.uim_report_rsp.rsp_data.data_ptr,
                   int32touint32(msg_ptr->rsp_data.data_len),
                   msg_ptr->rsp_data.data_ptr,
                   int32touint32(msg_ptr->rsp_data.data_len));
    task_cmd_ptr->cmd.cmd.uim_report_rsp.rsp_data.data_len = msg_ptr->rsp_data.data_len;
  }

  /* Free temp data */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK((msg_ptr->rsp_data.data_ptr));
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(msg_ptr);

  task_cmd_ptr->cmd.cmd.uim_report_rsp.request_header.request_len = total_mem_len;
  task_cmd_ptr->cmd.cmd.uim_report_rsp.request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi command queue */
  if ( mmgsdi_cmd_ext(task_cmd_ptr, mmgsdi_task) != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_2("In mmgsdi_uim_report(), Queue of MMGSDI command: MMGSDI_UIM_REPORT_RSP status 0x%x,"
                 " mmgsdi_task 0x%x", mmgsdi_status, mmgsdi_task);
} /* mmgsdi_uim_report */


/* ==========================================================================
   FUNCTION:      MMGSDI_UIM_REPORT_SYNCH

   DESCRIPTION:
     This function is called when UIM report back command response

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
static void mmgsdi_uim_report_synch (
   uim_rpt_type *report_ptr
)
{
  mmgsdi_task_enum_type    mmgsdi_task    = MMGSDI_TASK_MAX;
  uint8                    index          = 0;
  mmgsdi_task_state_enum_type task_state  = MMGSDI_TASK_INACTIVE;

  if (report_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);

  /* External task. So get the task from user_data */
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    mmgsdi_task = MMGSDIUTIL_GET_TASK_FROM_UIMDRV_USER_DATA(report_ptr->user_data);
    if((mmgsdi_task != MMGSDI_TASK_MAIN)  &&
       (mmgsdi_task != MMGSDI_TASK_SLOT1) &&
       (mmgsdi_task != MMGSDI_TASK_SLOT2) &&
       (mmgsdi_task != MMGSDI_TASK_SLOT3))
    {
      /* This is unreachable code in normal circumstances, if reached, it indicates
         data corruption by the UIM. ERR_FATAL in this case */
      ERR_FATAL("Invalid task in mmgsdi_uim_report_synch user_data", 0, 0, 0);
    }
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  task_state = mmgsdi_task_state[mmgsdi_task];
  if (task_state != MMGSDI_TASK_ACTIVE)
  {
    UIM_MSG_ERR_1("MMGSDI TASK 0x%x STOPPED...CAN'T PROCESS COMMAND ",
                  mmgsdi_task);
  }
  else
  {
  index = MMGSDIUTIL_GET_INDEX_FROM_UIMDRV_USER_DATA(report_ptr->user_data);

  if (index < MMGSDI_MAX_CLIENT_INFO)
  {
    if (mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
    {
      UIM_MSG_HIGH_0("Internal SYNCH UIM RPT PTR is not NULL");
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
    }
    (void) mmgsdi_build_uim_rsp(&mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], report_ptr);
  }
  else
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Unknown index, 0x%x in user data", index);
  }

  UIM_MSG_HIGH_1("In mmgsdi_uim_report_synch(), mmgsdi task 0x%x", mmgsdi_task);

  switch (mmgsdi_task)
  {
    case MMGSDI_TASK_SLOT1:
      (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_SYNCH_UIM_REPORT_SIG);
      break;

    case MMGSDI_TASK_SLOT2:
      (void) rex_set_sigs(UIM_GSDI_SLOT2_TCB, MMGSDI_SYNCH_UIM_REPORT_SIG);
      break;

  case MMGSDI_TASK_SLOT3:
      (void) rex_set_sigs(UIM_GSDI_SLOT3_TCB, MMGSDI_SYNCH_UIM_REPORT_SIG);
      break;

    case MMGSDI_TASK_MAIN:
    default:
      (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_SYNCH_UIM_REPORT_SIG);
      break;
  }
  }
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;
} /* mmgsdi_uim_report_synch */ /*lint !e818 */


/* ==========================================================================
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

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_cmd_to_uim_server_synch (
    uim_cmd_type *cmd_ptr
)
{
  uim_rpt_type               *uim_report_ptr  = NULL;
  rex_sigs_type               sigs_rcvd       = 0x00;
  mmgsdi_return_enum_type     mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_task_enum_type       mmgsdi_task     = MMGSDI_TASK_MAX;
  mmgsdi_slot_data_type      *slot_data_ptr   = NULL;
  uint32                      client_data     = 0;
  uim_return_type             uim_status      = UIM_GENERIC_ERROR;
  mmgsdi_task_state_enum_type task_state      = MMGSDI_TASK_INACTIVE;
  /* List of sigs all gsdi tasks will wait for...later on if we figure out
     that this function is intered for gsdi main task we will add some gsdi
     main task specific sigs to the wait sigs list as well */
  rex_sigs_type               sigs            = MMGSDI_TASK_STOP_SIG                  |
                                                MMGSDI_SYNCH_UIM_REPORT_SIG           |
                                                MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG;

  /* Get current task info */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if (mmgsdi_task == MMGSDI_TASK_MAX)
  {
    UIM_MSG_ERR_0("mmgsdi_send_cmd_to_uim_server_synch(), MMGSDI Task ID could not be determined");
    return MMGSDI_ERROR;
  }

  /* These signals are supposed to be set only on the main GSDI task. So
     for a robust design, include them in the wait sig list ONLY for the
     main GSDI task so that the tasks other than main task do not process
     them if they accidently receive these signals (maybe because of some
     sig clashes) */
  if (mmgsdi_task == MMGSDI_TASK_MAIN)
  {
    sigs =                           sigs                                  |
                                     MMGSDI_RCEVT_QMI_MODEM_TASK_READY_SIG |
                                     MMGSDI_QMI_UIM_SIG;
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  task_state = mmgsdi_task_state[mmgsdi_task];
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  if (task_state != MMGSDI_TASK_ACTIVE)
  {
    UIM_MSG_ERR_1("MMGSDI TASK 0x%x STOPPED...CAN'T PROCESS COMMAND ",
                  mmgsdi_task);
    return MMGSDI_ERROR;
  }

  /* Commands can be sent to UIM in sync mode only in GSDI context */
  if(mmgsdi_util_get_current_task() == MMGSDI_TASK_MAX)
  {
    UIM_MSG_HIGH_0("mmgsdi_send_cmd_to_uim_server_synch() in non MMGSDI context");
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr_for_task(mmgsdi_task);
  if (slot_data_ptr != NULL &&
      slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
  {
    /* Do not send command to UIM if OnChip SIM is activated */
    return MMGSDI_RESTRICTED_IN_ONCHIP_MODE;
  }

  MMGSDIUTIL_RETURN_IF_NULL(cmd_ptr);

  UIM_MSG_HIGH_1("MMGSDI SEND CMD TO UIM IN MMGSDI TASK 0x%x", mmgsdi_task);

  /* In addition to the index, the current mmgsdi_task is
     saved in user_data. This is done so that the response from
     UIM is queued to the appropriate mmgsdi task */
  client_data = MMGSDIUTIL_SEND_CMD_CLIENT_DATA(mmgsdi_task,
                                                cmd_ptr->hdr.user_data);

  /* Update the user_data with mmgsdi_task enum */
  cmd_ptr->hdr.user_data          = client_data;
  cmd_ptr->hdr.cmd_hdr.task_ptr   = NULL;
  cmd_ptr->hdr.cmd_hdr.sigs       = 0;
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  (void)q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);

  cmd_ptr->hdr.options      = UIM_OPTION_ALWAYS_RPT;
  cmd_ptr->hdr.rpt_function = mmgsdi_uim_report_synch;

  (void)rex_clr_sigs(rex_self(), MMGSDI_SYNCH_UIM_REPORT_SIG);
  (void)rex_clr_sigs(rex_self(), MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

  /* Initialize the uim_report to not success so that it can be used in
     the failure conditions */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_report_ptr, sizeof(uim_rpt_type));
  if (uim_report_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  uim_report_ptr->rpt_status = UIM_FAIL;
  uim_report_ptr->rpt_type   = UIM_NO_SUCH_REPORT_R;
  uim_report_ptr->sw1        = SW1_NO_STATUS;  /* 0x00 */
  uim_report_ptr->sw2        = SW2_NORMAL_END; /* 0x00 */
  uim_report_ptr->user_data  = cmd_ptr->hdr.user_data;
  uim_report_ptr->cmd_transacted = FALSE;

  if(slot_data_ptr != NULL)
  {
    /* Set the 2 Minute Sanity Timer for each SIM Access */
   (void)rex_set_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task],
                       slot_data_ptr->mmgsdi_uim_sanity_timer_value);
  }
  else
  {
    /* Set the default timer for each SIM Access */
    (void)rex_set_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task],
                        MMGSDI_UIM_SANITY_TIMER_VALUE);
  }

  uim_status = uim_cmd(cmd_ptr);
  if (uim_status != UIM_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to queue command to UIM; status = 0x%x", uim_status);
    (void)rex_clr_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task]);
    mmgsdi_status =
       mmgsdi_build_uim_rsp(&mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], uim_report_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);
    return mmgsdi_status;
  }

  MMGSDIUTIL_LEAVE_TASK_SYNC_CRIT_SECT;

  do
  {
    /* We need to wait for the signals. However, until the UIM Report
       & UIM Sanity Timer signals are set this function cannot proceed. Also
       assumption is UIM guarantees a response even in case of CARD_ERROR */
    sigs_rcvd = mmgsdi_wait(sigs);

    UIM_MSG_HIGH_1("OUT OF WAIT FOR UIM: SIGS 0x%x", sigs_rcvd);

    /* Handle QMI Modem task signal */
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    if (sigs_rcvd & sigs & MMGSDI_RCEVT_QMI_MODEM_TASK_READY_SIG)
    {
      mmgsdi_handle_qmi_modem_task_ready();
    }
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

    /* Handle QMI_UIM signal */
    if (sigs_rcvd & sigs & MMGSDI_QMI_UIM_SIG)
    {
      mmgsdi_handle_qmi_uim_sig();
    }

    /* Handle Sanity Timer expire signal */
    if (sigs_rcvd & sigs & MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG)
    {
      boolean sim_busy = FALSE;

      MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

      /* Check for sim busy */
      slot_data_ptr = mmgsdi_util_get_slot_data_ptr_for_task(mmgsdi_task);
      if (slot_data_ptr != NULL)
      {
         sim_busy = slot_data_ptr->sim_busy;
      }

      MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

      if (sim_busy)
      {
        /* Ignore sanity timer timeout if SIM is busy. */
        UIM_MSG_HIGH_0("Sanity timer expired during sim busy");
        (void)rex_clr_sigs(rex_self(), MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG);
        /* For security, start a new timer (can use MMGSDI_UIM_SANITY_TIMER_VALUE
           since calculations made elsewhere to find mmgsdi_uim_sanity_timer_value
           don't apply in sim busy situations */
        (void)rex_set_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task],
                            MMGSDI_UIM_SANITY_TIMER_VALUE);
      }
      else
      {
        break;
      }
    }
  } while ( (sigs_rcvd &
             (MMGSDI_SYNCH_UIM_REPORT_SIG |
             MMGSDI_TASK_STOP_SIG )) == 0 );

  MMGSDIUTIL_ENTER_TASK_SYNC_CRIT_SECT;

  /* Continue with normal processing of the Signals
  */
  if ( sigs_rcvd & sigs & MMGSDI_SYNCH_UIM_REPORT_SIG )
  {
    (void)rex_clr_sigs(rex_self(), MMGSDI_SYNCH_UIM_REPORT_SIG);
    (void)rex_clr_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task]);
  }

  /* If the Task Stop Sig is detected, stop sending
  ** the UIM Task commands and just return ERROR.
  */
  else if ( sigs_rcvd & sigs & MMGSDI_TASK_STOP_SIG )
  {
    MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
    mmgsdi_task_state[mmgsdi_task] = MMGSDI_TASK_STOPPED;
    MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

    /* The SIG is not cleared as it is handled in the forever loop */
    UIM_MSG_ERR_1("MMGSDI_TASK_STOP_SIG received, in MMGSDI TASK 0x%x",
                  mmgsdi_task);
    MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);

    return MMGSDI_ERROR;
  }
  else if ( sigs_rcvd & sigs & MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG )
  {
    /* Do no clear the SIG let the GSDI Task Main Wait Function
    ** handle it.
    */
    UIM_MSG_ERR_1("GSDI - UIM SANITY TIMER EXPIRED, IN MMGSDI TASK 0x%x",
                  mmgsdi_task);
    mmgsdi_status =
       mmgsdi_build_uim_rsp(&mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task], uim_report_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);

    return mmgsdi_status;
  }

  /* free the memory used */
  MMGSDIUTIL_TMC_MEM_FREE(uim_report_ptr);
  return MMGSDI_SUCCESS;

} /* mmgsdi_send_cmd_to_uim_server_synch */


/* ==========================================================================
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

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_cmd_to_uim_server(
  uim_cmd_type *cmd_ptr
)
{
  mmgsdi_task_enum_type       mmgsdi_task     = MMGSDI_TASK_MAX;
  mmgsdi_slot_data_type      *slot_data_ptr   = NULL;
  uint32                      client_data     = 0;
  uim_return_type             uim_status      = UIM_GENERIC_ERROR;
  mmgsdi_task_state_enum_type task_state      = MMGSDI_TASK_INACTIVE;

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if (mmgsdi_task == MMGSDI_TASK_MAX)
  {
    UIM_MSG_ERR_0("mmgsdi_send_cmd_to_uim_server(), MMGSDI Task ID could not be determined");
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  task_state = mmgsdi_task_state[mmgsdi_task];
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  if (task_state != MMGSDI_TASK_ACTIVE)
  {
    UIM_MSG_ERR_1("MMGSDI TASK 0x%x STOPPED...CAN'T PROCESS COMMAND ",
                  mmgsdi_task);
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr_for_task(mmgsdi_task);
  if (slot_data_ptr != NULL &&
      slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
  {
    /* Do not send command to UIM if OnChip SIM is activated */
    return MMGSDI_RESTRICTED_IN_ONCHIP_MODE;
  }

  MMGSDIUTIL_RETURN_IF_NULL(cmd_ptr);

  UIM_MSG_HIGH_1("MMGSDI SEND CMD TO UIM IN MMGSDI TASK 0x%x", mmgsdi_task);

  /* Store the Client req. table info index corresponding to the
     command queued to UIM and update flag to indicate command is
     in progress with UIM */
  mmgsdi_uim_cmd[mmgsdi_task].index  = uint32toint32(cmd_ptr->hdr.user_data);
  mmgsdi_uim_cmd[mmgsdi_task].cmd_in_progress = TRUE;

  /* In addition to the index, the current mmgsdi_task is
     saved in user_data. This is done so that the response from
     UIM is queued to the appropriate mmgsdi task */
  client_data = MMGSDIUTIL_SEND_CMD_CLIENT_DATA(mmgsdi_task,
                                                cmd_ptr->hdr.user_data);

  /* Update the user_data with mmgsdi_task enum */
  cmd_ptr->hdr.user_data          = client_data;
  cmd_ptr->hdr.cmd_hdr.task_ptr   = NULL;
  cmd_ptr->hdr.cmd_hdr.sigs       = 0;
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  (void)q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);
  cmd_ptr->hdr.options      = UIM_OPTION_ALWAYS_RPT;
  cmd_ptr->hdr.rpt_function = mmgsdi_uim_report;

  if (mmgsdi_task == MMGSDI_TASK_MAIN)
  {
    (void)rex_clr_sigs(UIM_GSDI_TCB, MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG);
  }
  else
  {
    (void)rex_clr_sigs(rex_self(), MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG);
  }

  if(slot_data_ptr != NULL)
  {
    /* Set the 2 Minute Sanity Timer for each SIM Access */
   (void)rex_set_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task],
                       slot_data_ptr->mmgsdi_uim_sanity_timer_value);
  }
  else
  {
    /* Set the default timer for each SIM Access */
    (void)rex_set_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task],
                        MMGSDI_UIM_SANITY_TIMER_VALUE);
  }

  uim_status = uim_cmd(cmd_ptr);
  if (uim_status != UIM_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to queue command to UIM; status = 0x%x", uim_status);
    (void)rex_clr_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task]);
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_send_cmd_to_uim_server */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_MORE_PENDING_COMMAND

   DESCRIPTION:
     This function check if there is more pending commands in MMGSDI queue.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_more_pending_command(
  boolean *more_pending_cmd_ptr
)
{
  int i = 0;

  MMGSDIUTIL_RETURN_IF_NULL(more_pending_cmd_ptr);
  *more_pending_cmd_ptr = FALSE;

  for (i = 0; i < MMGSDI_MAX_CLIENT_REQ_INFO; i++)
  {
    if(mmgsdi_client_req_table_info_ptr[i] == NULL)
    {
      continue;
    }

    *more_pending_cmd_ptr = TRUE;
    return MMGSDI_SUCCESS;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_more_pending_command */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_ID_INDEX

   DESCRIPTION:
     This function checks if the client ID is valid and return the index
     location

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_id_index (
  mmgsdi_client_id_type  client_id,
  int32                 *client_index_ptr
)
{
  mmgsdi_return_enum_type mmgsdi_status         = MMGSDI_ERROR;
  int                     i                     = MMGSDI_MAX_CLIENT_INFO;
  uint32                  input_client_id_shift = 0;
  uint32                  table_client_id_shift = 0;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  MMGSDIUTIL_RETURN_IF_NULL(client_index_ptr);

  if ((0 == client_id) || (0 == (client_id >> MMGSDI_CLIENT_ID_SHIFT)) )
  {
    UIM_MSG_ERR_2("Client_id_low = 0x%x,Client_id_high = 0x%x not registered",
                  client_id&0xFFFFFFFF,
                  client_id >> MMGSDI_CLIENT_ID_SHIFT & 0xFFFFFFFF);
    return mmgsdi_status;
  }

  input_client_id_shift = uint64touint32((client_id >> MMGSDI_CLIENT_ID_SHIFT));
  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    table_client_id_shift =
      uint64touint32((mmgsdi_client_id_reg_table[i]->client_id >> MMGSDI_CLIENT_ID_SHIFT));
    if (table_client_id_shift == input_client_id_shift)
    {
      mmgsdi_status = MMGSDI_SUCCESS;
      *client_index_ptr = i;
      break;
    }
  }
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("Client_id_low = 0x%x,Client_id_high = 0x%x not registered",
                  client_id&0xFFFFFFFF,
                  client_id >> MMGSDI_CLIENT_ID_SHIFT & 0xFFFFFFFF);
  }
  return mmgsdi_status;
}/* mmgsdi_util_get_client_id_index */


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
     None
==========================================================================*/
mmgsdi_session_id_status_enum_type  mmgsdi_util_is_session_id_valid (
  mmgsdi_session_id_type session_id
)
{
  mmgsdi_session_id_status_enum_type session_status   = MMGSDI_SESSION_ID_UNASSIGNED;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  int32                              client_index     = 0;
  int32                              session_index    = 0;
  mmgsdi_session_info_type          *session_info_ptr = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                    session_id,
                    &client_index,
                    &session_index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return session_status;
  }

  /* mmgsdi_util_get_client_and_session_index success guarantees valid dereferences
     into mmgsdi_client_id_reg_table and session_info_ptr */
  session_info_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if(session_info_ptr == NULL)
  {
    UIM_MSG_ERR_1("NULL session_info_ptr, session_id=0x%x", session_id);
    return session_status;
  }

  /* If the session type is card session or there is a valid channel for the corresponding
  session info then set the session status as assigned and active. */
  if(mmgsdi_util_is_card_session(session_info_ptr->session_type))
  {
    session_status = MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE;
  }
  else if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL))
  {
    session_status = MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE;
  }
  else if(mmgsdi_util_is_prov_session(session_info_ptr->session_type))
  {
    session_status = MMGSDI_SESSION_ID_ASSIGNED_NOT_ACTIVE;
  }
  return session_status;
} /* mmgsdi_util_is_session_id_valid */


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
  int32                   client_index,
  int32                 * session_index_ptr
)
{
  byte  i = 0;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  MMGSDIUTIL_RETURN_IF_NULL(session_index_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(client_index,0,MMGSDI_MAX_CLIENT_INFO-1);

  *session_index_ptr = MMGSDI_MAX_SESSION_INFO;

  if (mmgsdi_client_id_reg_table[client_index] == NULL)
  {
    UIM_MSG_ERR_1("Invalid client index: 0x%x", client_index);
    return MMGSDI_ERROR;
  }

  for (i=0; i<MMGSDI_MAX_SESSION_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[client_index]->session_info_ptr[i] == NULL)
    {
      *session_index_ptr = i;
      return MMGSDI_SUCCESS;
    }
  }
  UIM_MSG_ERR_1("Unable to get free session index for client index 0x%x",
                client_index);
  return MMGSDI_ERROR;
} /* mmgsdi_util_get_free_session_index */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_FREE_CHANNEL_INDEX

   DESCRIPTION:
    This function finds the first free mmgsdi_channel_info_ptr_table entry
    so that a new channel can be provisioned at the free index

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_free_channel_index (
  int32                 * channel_index_ptr
)
{
  byte   i = 0;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  MMGSDIUTIL_RETURN_IF_NULL(channel_index_ptr);

  *channel_index_ptr = MMGSDI_MAX_CHANNEL_INFO;

  for (i = 0; i < MMGSDI_MAX_CHANNEL_INFO; i++)
  {
    if(mmgsdi_channel_info_ptr_table[i] == NULL)
    {
      *channel_index_ptr = i;
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_ERROR;
} /* mmgsdi_util_get_free_channel_index */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_MATCH_CHANNEL_INDEX

   DESCRIPTION:
    This function determines if the channel id provided is already allocated
    and indexed in the channel info table

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_match_channel_index (
  const mmgsdi_slot_id_enum_type  slot,
  const mmgsdi_channel_id_type    channel_id,
  int32                          *channel_index_ptr
)
{
  byte   i = 0;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  MMGSDIUTIL_RETURN_IF_NULL(channel_index_ptr);

  *channel_index_ptr = MMGSDI_MAX_CHANNEL_INFO;

  for (i = 0; i < MMGSDI_MAX_CHANNEL_INFO; i++)
  {
    if((mmgsdi_channel_info_ptr_table[i] != NULL) &&
       (mmgsdi_channel_info_ptr_table[i]->slot_id == slot) &&
       (mmgsdi_channel_info_ptr_table[i]->channel_id == channel_id))
    {
      *channel_index_ptr = i;
      return MMGSDI_SUCCESS;
    }
  }
  return MMGSDI_ERROR;
} /* mmgsdi_util_match_channel_index */


/* ==========================================================================
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

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_valid_client_id (
  mmgsdi_client_id_type client_id
)
{
  int32                   client_index  = 0;

  return mmgsdi_util_get_client_id_index(client_id,
                                         &client_index);
}/* mmgsdi_util_is_valid_client_id */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CAL_ALIGN_SIZE

   DESCRIPTION:
     This function calculates the aligned size

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_cal_align_size(
  int32                orig_size,
  int32               *aligned_size_ptr
)
{
  int32      mod = 0;
  if ((aligned_size_ptr == NULL) ||
      (orig_size <= 0) ||
      (orig_size > MMGSDI_INT32_MAX - MMGSDIUTIL_WORD_SIZE + 1))
  {
    return;
  }
  mod = orig_size % MMGSDIUTIL_WORD_SIZE;
  if (mod == 0)
  {
    *aligned_size_ptr = orig_size;
  }
  else
  {
    *aligned_size_ptr = orig_size + (MMGSDIUTIL_WORD_SIZE - mod);
  }
}/* mmgsdi_util_cal_align_size */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CAL_AND_ALLOC

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

==========================================================================*/
static void mmgsdi_util_align_ptr(
  void                *base_msg_ptr,
  void               **new_msg_pptr,
  int32                start_offset,
  int32                base_struct_size,
  int32               *end_offset_ptr)
{
  int32          aligned_var_size = 0;

  if (base_msg_ptr == NULL)
    return;
  if (end_offset_ptr == NULL)
    return;
  if (new_msg_pptr == NULL)
    return;

  mmgsdi_util_cal_align_size(base_struct_size,
                             &aligned_var_size);
  *new_msg_pptr = (uint8*)base_msg_ptr + start_offset + aligned_var_size;
  *end_offset_ptr = start_offset + aligned_var_size;
}/* mmgsdi_util_align_ptr */


/* ==========================================================================
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

==========================================================================*/
void mmgsdi_util_align_mem(
  void                *msg_ptr,
  mmgsdi_cmd_enum_type cmd
)
{
  int32   end_offset_ptr = 0;
  int32   start_offset   = 0;

  switch (cmd)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
    case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
    case MMGSDI_SESSION_OPEN_EXT_REQ:
    case MMGSDI_SESSION_CLOSE_REQ:
    case MMGSDI_SESSION_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_SAP_REPORT_RSP:
    case MMGSDI_CARD_STATUS_REQ:
    case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
    case MMGSDI_READ_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_GET_FILE_ATTR_REQ:
    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
    case MMGSDI_GET_ATR_REQ:
    case MMGSDI_SAP_CONNECT_REQ:
    case MMGSDI_SAP_DISCONNECT_REQ:
    case MMGSDI_SAP_GET_ATR_REQ:
    case MMGSDI_SAP_POWER_ON_REQ:
    case MMGSDI_SAP_POWER_OFF_REQ:
    case MMGSDI_SAP_RESET_REQ:
    case MMGSDI_CARD_PDOWN_REQ:
    case MMGSDI_CARD_PUP_REQ:
    case MMGSDI_CARD_RESET_REQ:
    case MMGSDI_REHABILITATE_REQ:
    case MMGSDI_INVALIDATE_REQ:
    case MMGSDI_PIN_OPERATION_REQ:
    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
    case MMGSDI_SRV_AVAILABLE_REQ:
    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
    case MMGSDI_RUN_CAVE_REQ:
    case MMGSDI_GENERATE_KEY_VPM_REQ:
    case MMGSDI_SESSION_GET_INFO_REQ:
    case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
    case MMGSDI_SESSION_STORE_ESN_REQ:
    case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
    case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
    case MMGSDI_SESSION_SSD_UPDATE_REQ:
    case MMGSDI_SESSION_SSD_CONFIRM_REQ:
    case MMGSDI_SESSION_BS_CHAL_REQ:
    case MMGSDI_SESSION_OTASP_OTAPA_REQ:
    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
    case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
    case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
    case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ:
    case MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ:
    case MMGSDI_SESSION_OTASP_COMMIT_REQ:
    case MMGSDI_SESSION_READ_PRL_REQ:
    case MMGSDI_SESSION_SUB_OK_REQ:
    case MMGSDI_NOTIFY_LINK_EST_REQ:
    case MMGSDI_NOTIFY_CARD_ERROR_REQ:
    case MMGSDI_NOTIFY_SIM_BUSY_REQ:
    case MMGSDI_INTERNAL_PUP_INIT_REQ:
    case MMGSDI_SIMLOCK_RSP:
    case MMGSDI_SIMLOCK_EVT:
    case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
    case MMGSDI_HANDLE_NV_REFRESH_REQ:
    case MMGSDI_FULL_SERVICE_REQ:
    case MMGSDI_MANAGE_CHANNEL_REQ:
    case MMGSDI_GET_SIM_PROFILE_REQ:
    case MMGSDI_SET_SIM_PROFILE_REQ:
    case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
      break;

    case MMGSDI_REFRESH_REQ:
      if (msg_ptr != NULL)
      {
        if(((mmgsdi_refresh_req_type*)(msg_ptr))->refresh_req_type ==
            MMGSDI_REFRESH_ORIG_REFRESH_REQ )
        {
          if(((mmgsdi_refresh_req_type*)(msg_ptr))->refresh.card_req.refresh_mode ==
                 MMGSDI_REFRESH_STEERING_OF_ROAMING)
          {
            mmgsdi_util_align_ptr(msg_ptr,
              (void**)&((mmgsdi_refresh_req_type*)(msg_ptr))->refresh.card_req.plmn_list.data_ptr,
              0,
              sizeof(mmgsdi_refresh_req_type),
              &end_offset_ptr);
          }
          else
          {
            mmgsdi_util_align_ptr(msg_ptr,
              (void**)&((mmgsdi_refresh_req_type*)(msg_ptr))->refresh.card_req.refresh_files.data_ptr,
              0,
              sizeof(mmgsdi_refresh_req_type),
              &end_offset_ptr);
          }
        }
        else if(((mmgsdi_refresh_req_type*)(msg_ptr))->refresh_req_type ==
            MMGSDI_REFRESH_ORIG_REG_REQ ||
            ((mmgsdi_refresh_req_type*)(msg_ptr))->refresh_req_type ==
            MMGSDI_REFRESH_ORIG_DEREG_REQ)
        {
         /* We shall align both file_list and file path ptr here, however
            depending the type of reg/dereg request (by enum/ path) we
            shall set one of the ptrs to null in calling function */

          mmgsdi_util_align_ptr(msg_ptr,
             (void**)&((mmgsdi_refresh_req_type*)(msg_ptr))->refresh.reg_req.refresh_files.file_list_ptr,
             0,
             sizeof(mmgsdi_refresh_req_type),
             &end_offset_ptr);
          mmgsdi_util_align_ptr(msg_ptr,
            (void**)&((mmgsdi_refresh_req_type*)(msg_ptr))->refresh.reg_req.refresh_files.file_path_ptr,
            0,
            sizeof(mmgsdi_refresh_req_type),
            &end_offset_ptr);
        }
      }
      break;
    case MMGSDI_WRITE_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_write_req_type*)(msg_ptr))->data.data_ptr,
        0,
        sizeof(mmgsdi_write_req_type),
        &end_offset_ptr);
      }
      break;
    case MMGSDI_INCREASE_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_increase_req_type*)(msg_ptr))->data.data_ptr,
        0,
        sizeof(mmgsdi_increase_req_type),
        &end_offset_ptr);
      }
      break;
    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_activate_onchip_sim_req_type*)(msg_ptr))->onchip_sim.data_ptr,
        0,
        sizeof(mmgsdi_activate_onchip_sim_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_SEND_APDU_REQ:
    case MMGSDI_SEND_APDU_EXT_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_send_apdu_req_type*)(msg_ptr))->data.data_ptr,
          0,
          sizeof(mmgsdi_send_apdu_req_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_UIM_REPORT_RSP:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_uim_report_rsp_type*)(msg_ptr))->rsp_data.data_ptr,
        0,
        sizeof(mmgsdi_uim_report_rsp_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_ISIM_AUTH_REQ:
    case MMGSDI_USIM_AUTH_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_auth_req_type*)(msg_ptr))->auth_req.data_ptr,
        0,
        sizeof(mmgsdi_auth_req_type),
        &end_offset_ptr);
      }
      break;
    case MMGSDI_COMPUTE_IP_AUTH_REQ:
      if (msg_ptr != NULL)
      {
        switch(((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.operation_type)
        {
          case MMGSDI_CMPT_IP_HRPD_CHAP:
            mmgsdi_util_align_ptr(msg_ptr,
             (void**)&((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.hrpd_chap_data.chap_challenge_ptr,
             0,
             sizeof(mmgsdi_compute_ip_req_type),
             &end_offset_ptr);
            break;
          case MMGSDI_CMPT_IP_MIP_MN_AAA:
            mmgsdi_util_align_ptr(msg_ptr,
             (void**)&((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.mn_aaa_data.challenge_ptr,
             0,
             sizeof(mmgsdi_compute_ip_req_type),
             &end_offset_ptr);
            break;
          case MMGSDI_CMPT_IP_MIP_MN_HA:
            mmgsdi_util_align_ptr(msg_ptr,
             (void**)&((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.mn_ha_data.registration_data_ptr,
             0,
             sizeof(mmgsdi_compute_ip_req_type),
             &end_offset_ptr);
            break;
          case MMGSDI_CMPT_IP_MIP_RRQ:
            mmgsdi_util_align_ptr(msg_ptr,
             (void**)&((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.rrq_data.rrq_data_ptr,
             0,
             sizeof(mmgsdi_compute_ip_req_type),
             &end_offset_ptr);
            break;
          case MMGSDI_CMPT_IP_SIP_CHAP:
            /* chap_challenge is an array of type uint8 and length
               MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH */
            mmgsdi_util_align_ptr(msg_ptr,
             (void**)((mmgsdi_compute_ip_req_type*)(msg_ptr))->compute_ip_data.data.sip_chap_data.chap_challenge,
             0,
             sizeof(mmgsdi_compute_ip_req_type),
             &end_offset_ptr);
            break;
          case MMGSDI_CMPT_IP_MAX_VALUE:
            break;
          /* Note - There is no default case here and it is done on purpose since
             we need to handle all values for mmgsdi_compute_ip_operation_enum_type.
             Unhandled values will get flagged by lint error */
        }
      }
      break;

    case MMGSDI_CNF:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_task_cmd_data_type*)(msg_ptr))->cmd.mmgsdi_cnf_ptr,
          0,
          sizeof(mmgsdi_task_cmd_data_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_SAP_SEND_APDU_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_sap_send_apdu_req_type*)(msg_ptr))->data.data_ptr,
          0,
          sizeof(mmgsdi_sap_send_apdu_req_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_SESSION_CDMA_AKA_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_cdma_aka_req_type*)(msg_ptr))->autn.data_ptr,
        0,
        sizeof(mmgsdi_cdma_aka_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_session_open_req_type*)(msg_ptr))->non_prov_app_data.app_id_data.data_ptr,
          0,
          sizeof(mmgsdi_session_open_req_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_BCAST_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_bcast_req_type*)(msg_ptr))->bcast_req.data_ptr,
        0,
        sizeof(mmgsdi_bcast_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_SEARCH_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_search_req_type*)(msg_ptr))->data.data_ptr,
          0,
          sizeof(mmgsdi_search_req_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_session_run_gsm_algo_req_type*)(msg_ptr))->rand_val.data_ptr,
          0,
          sizeof(mmgsdi_session_run_gsm_algo_req_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_SESSION_OTASP_MS_KEY_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_session_otasp_ms_key_req_type *)(msg_ptr))->randseed.data_ptr,
          start_offset,
          sizeof(mmgsdi_session_otasp_ms_key_req_type),
          &end_offset_ptr);
        start_offset +=
          ((mmgsdi_session_otasp_ms_key_req_type *)(msg_ptr))->randseed.data_len;

        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_session_otasp_ms_key_req_type *)(msg_ptr))->param_p.data_ptr,
          start_offset,
          sizeof(mmgsdi_session_otasp_ms_key_req_type),
          &end_offset_ptr);
        start_offset +=
          ((mmgsdi_session_otasp_ms_key_req_type *)(msg_ptr))->param_p.data_len;

        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_session_otasp_ms_key_req_type *)(msg_ptr))->param_g.data_ptr,
          start_offset,
          sizeof(mmgsdi_session_otasp_ms_key_req_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_SESSION_OTASP_KEY_GEN_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_session_otasp_key_gen_req_type *)(msg_ptr))->bs_result.data_ptr,
          0,
          sizeof(mmgsdi_session_otasp_key_gen_req_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_SESSION_OTASP_BLOCK_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_session_otasp_block_req_type *)(msg_ptr))->block_data.data_ptr,
          0,
          sizeof(mmgsdi_session_otasp_block_req_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_session_get_eons_req_type*)(msg_ptr))->plmn_id_list.plmn_list_ptr,
        0,
        sizeof(mmgsdi_session_get_eons_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_EONS_SS_EVENT_REQ:
      if (msg_ptr != NULL)
      {
        int32 name_len = 0;

        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_eons_ss_event_req_type*)(msg_ptr))->ss_info.first_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_ptr,
        0,
        sizeof(mmgsdi_eons_ss_event_req_type),
        &end_offset_ptr);

        mmgsdi_util_cal_align_size(
          ((mmgsdi_eons_ss_event_req_type*)(msg_ptr))->ss_info.first_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_len,
           &name_len);

        start_offset += name_len;

        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_eons_ss_event_req_type*)(msg_ptr))->ss_info.first_stack_ss.nitz_info.short_name.plmn_name.eons_data.data_ptr,
        start_offset,
        sizeof(mmgsdi_eons_ss_event_req_type),
        &end_offset_ptr);

        name_len      = 0;
        mmgsdi_util_cal_align_size(
          ((mmgsdi_eons_ss_event_req_type*)(msg_ptr))->ss_info.first_stack_ss.nitz_info.short_name.plmn_name.eons_data.data_len,
           &name_len);

        start_offset += name_len;

        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_eons_ss_event_req_type*)(msg_ptr))->ss_info.sec_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_ptr,
        start_offset,
        sizeof(mmgsdi_eons_ss_event_req_type),
        &end_offset_ptr);

        name_len      = 0;
        mmgsdi_util_cal_align_size(
          ((mmgsdi_eons_ss_event_req_type*)(msg_ptr))->ss_info.sec_stack_ss.nitz_info.long_name.plmn_name.eons_data.data_len,
          &start_offset);

        start_offset += name_len;

        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_eons_ss_event_req_type*)(msg_ptr))->ss_info.sec_stack_ss.nitz_info.short_name.plmn_name.eons_data.data_ptr,
        start_offset,
        sizeof(mmgsdi_eons_ss_event_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_session_register_file_update_req_type*)(msg_ptr))->file_update_list.file_list_ptr,
        0,
        sizeof(mmgsdi_session_register_file_update_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_RECOVERY_IND_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
          (void**)&((mmgsdi_recovery_ind_req_type *)(msg_ptr))->session_id_ptr,
          0,
          sizeof(mmgsdi_recovery_ind_req_type),
          &end_offset_ptr);
      }
      break;

    case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
      break;

    case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_get_se13_plmn_names_req_type*)(msg_ptr))->plmn_id_list.plmn_list_ptr,
        0,
        sizeof(mmgsdi_get_se13_plmn_names_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_get_se13_plmn_info_by_name_req_type*)(msg_ptr))->plmn_long_name.data_ptr,
        0,
        sizeof(mmgsdi_get_se13_plmn_info_by_name_req_type),
        &end_offset_ptr);
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_get_se13_plmn_info_by_name_req_type*)(msg_ptr))->plmn_short_name.data_ptr,
        ((mmgsdi_get_se13_plmn_info_by_name_req_type*)(msg_ptr))->plmn_long_name.data_len,
        sizeof(mmgsdi_get_se13_plmn_info_by_name_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_session_update_acsgl_record_req_type*)(msg_ptr))->hnb_name.data_ptr,
        0,
        sizeof(mmgsdi_session_update_acsgl_record_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_SET_BUILTIN_PLMN_LIST_REQ:
      if (msg_ptr != NULL)
      {
        mmgsdi_util_align_ptr(msg_ptr,
        (void**)&((mmgsdi_set_builtin_plmn_list_req_type *)(msg_ptr))->builtin_plmn_list.plmn_list_ptr,
        0,
        sizeof(mmgsdi_set_builtin_plmn_list_req_type),
        &end_offset_ptr);
      }
      break;

    case MMGSDI_MAX_CMD_ENUM:
      UIM_MSG_ERR_0("Invalid Cmd Type");
      return;
    /* Note - There is no default case here and it is done on purpose since we
       need to handle all values for mmgsdi_cmd_enum_type. Unhandled values
       will get flagged by lint error */
  }
}/* mmgsdi_util_align_mem */


/* ==========================================================================
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

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_alloc_and_populate_extra_param(
  mmgsdi_cmd_enum_type                cmd_type,
  const void                         *cmd_ptr,
  mmgsdi_client_req_extra_info_type **extra_param_pptr)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uint32                  data_len      = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(cmd_ptr, extra_param_pptr);

  switch(cmd_type)
  {
  case MMGSDI_READ_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_read_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_pptr)->read_data.access,
      &((mmgsdi_read_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
      return mmgsdi_status;
    }
    (*extra_param_pptr)->read_data.rec_num =
      ((mmgsdi_read_req_type*)cmd_ptr)->rec_num;
    (*extra_param_pptr)->read_data.data_len =
      ((mmgsdi_read_req_type*)cmd_ptr)->data_len;
    (*extra_param_pptr)->read_data.offset =
      ((mmgsdi_read_req_type*)cmd_ptr)->offset;
    (*extra_param_pptr)->read_data.file_type =
      ((mmgsdi_read_req_type*)cmd_ptr)->file_type;
    (*extra_param_pptr)->read_data.select_before_read =
      ((mmgsdi_read_req_type*)cmd_ptr)->select_before_read;
    break;

  case MMGSDI_WRITE_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_write_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_pptr)->write_data.access,
      &((mmgsdi_write_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
      return mmgsdi_status;
    }

    data_len = int32touint32(((mmgsdi_write_req_type*)cmd_ptr)->data.data_len);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      (*extra_param_pptr)->write_data.data.data_ptr,
      data_len);
    if ((*extra_param_pptr)->write_data.data.data_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy((*extra_param_pptr)->write_data.data.data_ptr,
                   data_len,
                   ((mmgsdi_write_req_type*)cmd_ptr)->data.data_ptr,
                   data_len);

    (*extra_param_pptr)->write_data.rec_num =
      ((mmgsdi_write_req_type*)cmd_ptr)->rec_num;
    (*extra_param_pptr)->write_data.offset =
      ((mmgsdi_write_req_type*)cmd_ptr)->offset;
    (*extra_param_pptr)->write_data.file_type =
      ((mmgsdi_write_req_type*)cmd_ptr)->file_type;
    (*extra_param_pptr)->write_data.data.data_len =
      ((mmgsdi_write_req_type*)cmd_ptr)->data.data_len;

    (*extra_param_pptr)->write_data.int_client_data =
       ((mmgsdi_write_req_type*)cmd_ptr)->int_client_data;
    break;

  case MMGSDI_INCREASE_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_increase_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_pptr)->increase_data.access,
      &((mmgsdi_increase_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
      return mmgsdi_status;
    }

    data_len = int32touint32(((mmgsdi_increase_req_type*)cmd_ptr)->data.data_len);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      (*extra_param_pptr)->increase_data.data.data_ptr,
      data_len);
    if ((*extra_param_pptr)->increase_data.data.data_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy((*extra_param_pptr)->increase_data.data.data_ptr,
                   data_len,
                   ((mmgsdi_increase_req_type*)cmd_ptr)->data.data_ptr,
                   data_len);

    (*extra_param_pptr)->increase_data.data.data_len =
      ((mmgsdi_increase_req_type*)cmd_ptr)->data.data_len;
    break;

  case MMGSDI_GET_FILE_ATTR_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_get_file_info_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->get_file_attr_data.int_client_data =
      ((mmgsdi_get_file_attr_req_type*)cmd_ptr)->int_client_data;
    (*extra_param_pptr)->get_file_attr_data.select_rsp_requested =
      ((mmgsdi_get_file_attr_req_type*)cmd_ptr)->select_rsp_requested;
    (*extra_param_pptr)->get_file_attr_data.select_rsp_type =
      ((mmgsdi_get_file_attr_req_type*)cmd_ptr)->select_rsp_type;
    (*extra_param_pptr)->get_file_attr_data.skip_uicc_arr =
      ((mmgsdi_get_file_attr_req_type*)cmd_ptr)->skip_uicc_arr;

    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_pptr)->get_file_attr_data.access,
      &((mmgsdi_get_file_attr_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
    }
    break;

  case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
  case MMGSDI_SESSION_OPEN_REQ:
  case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_open_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy(&(*extra_param_pptr)->open_data.aid,
                   sizeof((*extra_param_pptr)->open_data.aid),
                   &((mmgsdi_session_open_extra_info_type*)cmd_ptr)->aid,
                   sizeof(mmgsdi_aid_type));
    (*extra_param_pptr)->open_data.exclusive_channel =
      ((mmgsdi_session_open_extra_info_type*)cmd_ptr)->exclusive_channel;
    (*extra_param_pptr)->open_data.select_rsp_requested =
      ((mmgsdi_session_open_extra_info_type*)cmd_ptr)->select_rsp_requested;
    (*extra_param_pptr)->open_data.select_rsp_type =
      ((mmgsdi_session_open_extra_info_type*)cmd_ptr)->select_rsp_type;
    break;

  case MMGSDI_SESSION_DEACTIVATE_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_deactivate_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->deactivate_data.deactivate_info = 0;
    break;

  case MMGSDI_SESSION_CLOSE_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_close_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy(&(*extra_param_pptr)->close_data.request_header,
                   sizeof((*extra_param_pptr)->close_data.request_header),
                   &((mmgsdi_session_close_req_type*)cmd_ptr)->request_header,
                   sizeof(mmgsdi_request_header_type));
    (*extra_param_pptr)->close_data.is_dummy = FALSE;
    break;

  case MMGSDI_SEND_APDU_REQ:
  case MMGSDI_SEND_APDU_EXT_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_send_apdu_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    data_len =
      int32touint32(((mmgsdi_send_apdu_req_type*)cmd_ptr)->data.data_len);

    if (data_len > 0)
    {
      (*extra_param_pptr)->send_apdu_data.cla =
        ((mmgsdi_send_apdu_req_type*)cmd_ptr)->data.data_ptr[0];
    }

    if (data_len > 1)
    {
      (*extra_param_pptr)->send_apdu_data.ins =
        ((mmgsdi_send_apdu_req_type*)cmd_ptr)->data.data_ptr[1];
    }

    (*extra_param_pptr)->send_apdu_data.channel_id =
      ((mmgsdi_send_apdu_req_type*)cmd_ptr)->channel_id;
    (*extra_param_pptr)->send_apdu_data.apdu_option =
      ((mmgsdi_send_apdu_req_type*)cmd_ptr)->apdu_option;
    break;

  case MMGSDI_SAP_DISCONNECT_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_sap_disconnect_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->sap_disconnect_data.disconnect_mode =
      ((mmgsdi_sap_disconnect_req_type*)cmd_ptr)->disconnect_mode;
    (*extra_param_pptr)->sap_disconnect_data.slot_id =
      ((mmgsdi_sap_disconnect_req_type*)cmd_ptr)->request_header.slot_id;
    break;

  case MMGSDI_SAP_CONNECT_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_sap_connect_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->sap_connect_data.slot_id =
      ((mmgsdi_sap_connect_req_type*)cmd_ptr)->request_header.slot_id;
    break;

  case MMGSDI_SAP_POWER_ON_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_sap_power_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->sap_power_data.slot_id =
      ((mmgsdi_sap_power_on_req_type*)cmd_ptr)->request_header.slot_id;
    break;

  case MMGSDI_SAP_POWER_OFF_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
          sizeof(mmgsdi_sap_power_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->sap_power_data.slot_id =
      ((mmgsdi_sap_power_off_req_type*)cmd_ptr)->request_header.slot_id;
    break;

  case MMGSDI_SAP_SEND_APDU_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_sap_send_apdu_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    data_len = int32touint32(((mmgsdi_sap_send_apdu_req_type*)cmd_ptr)->data.data_len);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      (*extra_param_pptr)->sap_send_apdu_data.data.data_ptr,
      data_len);
    if ((*extra_param_pptr)->sap_send_apdu_data.data.data_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy((*extra_param_pptr)->sap_send_apdu_data.data.data_ptr,
                   data_len,
                   ((mmgsdi_sap_send_apdu_req_type*)cmd_ptr)->data.data_ptr,
                   data_len);
    (*extra_param_pptr)->sap_send_apdu_data.data.data_len =
      ((mmgsdi_sap_send_apdu_req_type*)cmd_ptr)->data.data_len;
    (*extra_param_pptr)->sap_send_apdu_data.channel_id =
      ((mmgsdi_sap_send_apdu_req_type*)cmd_ptr)->channel_id;
    (*extra_param_pptr)->sap_send_apdu_data.slot_id =
      ((mmgsdi_sap_send_apdu_req_type*)cmd_ptr)->request_header.slot_id;
    break;

  case MMGSDI_CARD_PDOWN_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_card_pdown_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->pdown_data.option =
                      ((mmgsdi_card_pdown_req_type*)cmd_ptr)->option;
    break;
  case MMGSDI_CARD_PUP_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_card_pup_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->pup_data.option =
                       ((mmgsdi_card_pup_req_type*)cmd_ptr)->option;
    break;

  case MMGSDI_REFRESH_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_card_refresh_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->refresh_data.stage = MMGSDI_REFRESH_STAGE_ENUM_MAX;
    break;

  case MMGSDI_REHABILITATE_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_rehabilitate_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_pptr)->rehab_data.access,
      &((mmgsdi_set_file_status_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(*extra_param_pptr);
      return mmgsdi_status;
    }
    break;

  case MMGSDI_INVALIDATE_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_invalidate_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_status = mmgsdi_util_copy_access_type(
      &(*extra_param_pptr)->invalidate_data.access,
      &((mmgsdi_set_file_status_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE(*extra_param_pptr);
      return mmgsdi_status;
    }
    break;

  case MMGSDI_PIN_OPERATION_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_pin_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    /*PIN_ID*/
    (*extra_param_pptr)->pin_data.pin_info.pin_id = ((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_id;
    (*extra_param_pptr)->pin_data.orig_pin_id     = ((mmgsdi_pin_operation_req_type*)cmd_ptr)->orig_pin_id;

    if(((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_id != MMGSDI_HIDDENKEY)
    {
      /*The retry_type is filled in the only if all the data associated with the req has length of 0,
        At the external API, len = 0 is rejected, so this would allow MMGSDI to "internally" use this
        as a way to determine retry num process or not*/
      if(((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_len != 0)
      {
        (*extra_param_pptr)->pin_data.retry_type = MMGSDI_RETRY_NONE;

        /* If the request is for PIN enable/disable/verify, store the PIN data */
        if((((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op == MMGSDI_PIN_OP_ENABLE) ||
           (((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op == MMGSDI_PIN_OP_DISABLE) ||
           (((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op == MMGSDI_PIN_OP_VERIFY) ||
           (((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op == MMGSDI_PIN_OP_DISABLE_AND_REPLACE))
        {
          if((((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_len >= MMGSDI_PIN_MIN_LEN) &&
             (((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_len <= MMGSDI_PIN_MAX_LEN))
          {
            (*extra_param_pptr)->pin_data.pin_data.data_len =
              ((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_len;
            mmgsdi_memscpy((void*)(*extra_param_pptr)->pin_data.pin_data.data_ptr,
              sizeof((*extra_param_pptr)->pin_data.pin_data.data_ptr),
              (const void*)((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_ptr,
              int32touint32(((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_len));
          }
        }
        /* If the request is for PIN change/unblock, store the new PIN data */
        else if((((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op == MMGSDI_PIN_OP_CHANGE) ||
                (((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op == MMGSDI_PIN_OP_UNBLOCK))
        {
          if((((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_len >= MMGSDI_PIN_MIN_LEN) &&
             (((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_len <= MMGSDI_PIN_MAX_LEN))
          {
            (*extra_param_pptr)->pin_data.pin_data.data_len =
              ((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_len;
            mmgsdi_memscpy((void*)(*extra_param_pptr)->pin_data.pin_data.data_ptr,
              sizeof((*extra_param_pptr)->pin_data.pin_data.data_ptr),
              (const void*)((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_ptr,
              int32touint32(((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_len));

            /* Cache the new PIN data into extra parameter pointer for verification of PIN after changing it */
            if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_VERIFY_PIN_AFTER_CHANGE, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
            {
              (*extra_param_pptr)->pin_data.pin_data_copy.data_len =
                ((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_len;
              mmgsdi_memscpy((void*)(*extra_param_pptr)->pin_data.pin_data_copy.data_ptr,
                sizeof((*extra_param_pptr)->pin_data.pin_data_copy.data_ptr),
                (const void*)((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_ptr,
                int32touint32(((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_len));
            }
          }
        }
      }
      /* Cache the user entered PIN data into extra parameter pointer from the request for verification
         of PIN after changing it */
      else if((mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_VERIFY_PIN_AFTER_CHANGE, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED) &&
              (((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op == MMGSDI_PIN_OP_CHANGE) &&
              (((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_copy.data_len >= MMGSDI_PIN_MIN_LEN) &&
              (((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_copy.data_len <= MMGSDI_PIN_MAX_LEN))
      {
        (*extra_param_pptr)->pin_data.pin_data_copy.data_len =
          ((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_copy.data_len;
        mmgsdi_memscpy((void*)(*extra_param_pptr)->pin_data.pin_data_copy.data_ptr,
          sizeof((*extra_param_pptr)->pin_data.pin_data_copy.data_ptr),
          (const void*)((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_copy.data_ptr,
          int32touint32(((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_copy.data_len));
      }
    }
    /* Hiddenkey request */
    else
    {
      /* If the request is for Hiddenkey enable, store the Hiddenkey data */
      if(((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op == MMGSDI_PIN_OP_ENABLE)
      {
        (*extra_param_pptr)->pin_data.pin_data.data_len =
          ((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_len;
        mmgsdi_memscpy((void*)(*extra_param_pptr)->pin_data.pin_data.data_ptr,
          sizeof((*extra_param_pptr)->pin_data.pin_data.data_ptr),
          (const void*)((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_ptr,
          int32touint32(((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_puk.data_len));
      }
      /* If the request is for Hiddenkey change, store the new Hiddenkey data */
      else if(((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op == MMGSDI_PIN_OP_CHANGE)
      {
        (*extra_param_pptr)->pin_data.pin_data.data_len =
          ((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_len;
        mmgsdi_memscpy((void*)(*extra_param_pptr)->pin_data.pin_data.data_ptr,
          sizeof((*extra_param_pptr)->pin_data.pin_data.data_ptr),
          (const void*)((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_ptr,
          int32touint32(((mmgsdi_pin_operation_req_type*)cmd_ptr)->new_pin.data_len));
      }
    }
    /*PIN_OP*/
    (*extra_param_pptr)->pin_data.orig_pin_op = ((mmgsdi_pin_operation_req_type*)cmd_ptr)->pin_op;

    /*Status words of the original command */
    (*extra_param_pptr)->pin_data.orig_sw.sw1 = ((mmgsdi_pin_operation_req_type*)cmd_ptr)->orig_sw.sw1;
    (*extra_param_pptr)->pin_data.orig_sw.sw2 = ((mmgsdi_pin_operation_req_type*)cmd_ptr)->orig_sw.sw2;
    break;

  case MMGSDI_SRV_AVAILABLE_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_srv_available_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    break;

  case MMGSDI_SEARCH_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_search_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_status = mmgsdi_util_copy_access_type(
                                  &(*extra_param_pptr)->search_data.access,
                                  &((mmgsdi_search_req_type*)cmd_ptr)->access);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
      return mmgsdi_status;
    }
    break;

  case MMGSDI_SESSION_GET_INFO_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_get_info_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    mmgsdi_memscpy(&(*extra_param_pptr)->session_get_info_data,
                   sizeof(mmgsdi_session_get_info_extra_info_type),
                   ((mmgsdi_session_get_info_extra_info_type*)cmd_ptr),
                   sizeof(mmgsdi_session_get_info_extra_info_type));
    break;

  case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_enable_service_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy(&(*extra_param_pptr)->enable_service_data,
                   sizeof(mmgsdi_session_enable_service_extra_info_type),
                   ((mmgsdi_session_enable_service_extra_info_type*)cmd_ptr),
                   sizeof(mmgsdi_session_enable_service_extra_info_type));
    break;

  case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_enable_service_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy(&(*extra_param_pptr)->enable_service_data,
                   sizeof(mmgsdi_session_enable_service_extra_info_type),
                   ((mmgsdi_session_enable_service_extra_info_type*)cmd_ptr),
                   sizeof(mmgsdi_session_enable_service_extra_info_type));
    break;

  case MMGSDI_SESSION_OTASP_BLOCK_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_otasp_block_rsp_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->session_otasp_block_data.otasp_block_op_type =
               ((mmgsdi_session_otasp_block_req_type*)cmd_ptr)->block_op_type;
    break;
  case MMGSDI_SESSION_READ_PRL_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_read_prl_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy(&(*extra_param_pptr)->session_read_prl_data,
                   sizeof(mmgsdi_session_read_prl_extra_info_type),
                   ((mmgsdi_session_read_prl_extra_info_type*)cmd_ptr),
                   sizeof(mmgsdi_session_read_prl_extra_info_type));
    break;

  case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_get_eons_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->session_get_eons_data.ignore_display_condition =
               ((mmgsdi_session_get_eons_req_type*)cmd_ptr)->ignore_display_condition;

    (*extra_param_pptr)->session_get_eons_data.plmn_id_list.num_of_plmn_ids =
               ((mmgsdi_session_get_eons_req_type*)cmd_ptr)->plmn_id_list.num_of_plmn_ids;

    data_len =
        (*extra_param_pptr)->session_get_eons_data.plmn_id_list.num_of_plmn_ids *
        sizeof(mmgsdi_plmn_id_type);

    if(data_len != 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        (*extra_param_pptr)->session_get_eons_data.plmn_id_list.plmn_list_ptr,
        data_len);
      if ((*extra_param_pptr)->session_get_eons_data.plmn_id_list.plmn_list_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_memscpy((*extra_param_pptr)->session_get_eons_data.plmn_id_list.plmn_list_ptr,
        data_len,
        ((mmgsdi_session_get_eons_req_type*)cmd_ptr)->plmn_id_list.plmn_list_ptr,
        data_len);
    }
    break;

  case MMGSDI_EONS_SS_EVENT_REQ:
  {
    mmgsdi_eons_ss_data_type * ss_info_ext_prm_ptr = NULL;
    mmgsdi_eons_ss_data_type * ss_info_req_ptr     = NULL;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_ss_event_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    ss_info_ext_prm_ptr = &(*extra_param_pptr)->ss_event_data.ss_info;
    ss_info_req_ptr     = &((mmgsdi_eons_ss_event_req_type*)cmd_ptr)->ss_info;

    ss_info_ext_prm_ptr->session_type = ss_info_req_ptr->session_type;

    if (ss_info_req_ptr->first_stack_ss.is_valid)
    {
      mmgsdi_status = mmgsdi_eons_copy_per_stack_ss_info(
                        &(ss_info_req_ptr->first_stack_ss),
                        &(ss_info_ext_prm_ptr->first_stack_ss),
                        TRUE);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
        return mmgsdi_status;
      }
    }

    if (ss_info_req_ptr->sec_stack_ss.is_valid)
    {
      mmgsdi_status = mmgsdi_eons_copy_per_stack_ss_info(
                        &(ss_info_req_ptr->sec_stack_ss),
                        &(ss_info_ext_prm_ptr->sec_stack_ss),
                        TRUE);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
        return mmgsdi_status;
      }
    }
    break;
  }

  case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_get_se13_plmn_names_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    (*extra_param_pptr)->get_se13_plmn_names_data.plmn_id_list.num_of_plmn_ids =
      ((mmgsdi_get_se13_plmn_names_req_type*)cmd_ptr)->plmn_id_list.num_of_plmn_ids;

    data_len =
      (*extra_param_pptr)->get_se13_plmn_names_data.plmn_id_list.num_of_plmn_ids *
      sizeof(mmgsdi_plmn_id_type);

    if(data_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        (*extra_param_pptr)->get_se13_plmn_names_data.plmn_id_list.plmn_list_ptr,
        data_len);
      if ((*extra_param_pptr)->get_se13_plmn_names_data.plmn_id_list.plmn_list_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(*extra_param_pptr);
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_memscpy((*extra_param_pptr)->get_se13_plmn_names_data.plmn_id_list.plmn_list_ptr,
        data_len,
        ((mmgsdi_get_se13_plmn_names_req_type*)cmd_ptr)->plmn_id_list.plmn_list_ptr,
        data_len);
    }
    break;

  case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_session_update_acsgl_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    (*extra_param_pptr)->update_acsgl_data.update_type =
               ((mmgsdi_session_update_acsgl_record_req_type*)cmd_ptr)->update_req_type;

    (*extra_param_pptr)->update_acsgl_data.plmn_csg_id.csg_id =
               ((mmgsdi_session_update_acsgl_record_req_type*)cmd_ptr)->plmn_csg_id.csg_id;

    mmgsdi_memscpy((*extra_param_pptr)->update_acsgl_data.plmn_csg_id.plmn_id_val,
                   sizeof((*extra_param_pptr)->update_acsgl_data.plmn_csg_id.plmn_id_val),
                   ((mmgsdi_session_update_acsgl_record_req_type*)cmd_ptr)->plmn_csg_id.plmn_id_val,
                   MMGSDI_PLMN_ID_SIZE);
    break;

  case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_gstk_loc_env_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    break;

  case MMGSDI_CARD_STATUS_REQ:
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*extra_param_pptr,
      sizeof(mmgsdi_app_status_extra_info_type));
    if ((*extra_param_pptr) == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    (*extra_param_pptr)->app_status_data.app_status =
                       ((mmgsdi_status_req_type*)cmd_ptr)->me_app_status;
    break;

  case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
  case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
  case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
  case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
  case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
  case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
  case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
  case MMGSDI_SESSION_SELECT_AID_REQ:
  case MMGSDI_ISIM_AUTH_REQ:
  case MMGSDI_CLIENT_ID_REG_RSP:
  case MMGSDI_UIM_REPORT_RSP:
  case MMGSDI_CNF:
  case MMGSDI_SAP_REPORT_RSP:
  case MMGSDI_USIM_AUTH_REQ:
  case MMGSDI_COMPUTE_IP_AUTH_REQ:
  case MMGSDI_RUN_CAVE_REQ:
  case MMGSDI_SESSION_CDMA_AKA_REQ:
  case MMGSDI_GENERATE_KEY_VPM_REQ:
  case MMGSDI_BCAST_REQ:
  case MMGSDI_SESSION_OPEN_EXT_REQ:
  case MMGSDI_GET_ALL_PIN_STATUS_REQ:
  case MMGSDI_GET_ATR_REQ:
  case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
  case MMGSDI_SAP_RESET_REQ:
  case MMGSDI_CARD_RESET_REQ:
  case MMGSDI_SAP_GET_ATR_REQ:
  case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
  case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
  case MMGSDI_SESSION_STORE_ESN_REQ:
  case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ:
  case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
  case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
  case MMGSDI_SESSION_SSD_UPDATE_REQ:
  case MMGSDI_SESSION_SSD_CONFIRM_REQ:
  case MMGSDI_SESSION_BS_CHAL_REQ:
  case MMGSDI_SESSION_OTASP_MS_KEY_REQ:
  case MMGSDI_SESSION_OTASP_KEY_GEN_REQ:
  case MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ:
  case MMGSDI_SESSION_OTASP_COMMIT_REQ:
  case MMGSDI_SESSION_OTASP_OTAPA_REQ:
  case MMGSDI_SESSION_SUB_OK_REQ:
  case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
  case MMGSDI_NOTIFY_LINK_EST_REQ:
  case MMGSDI_NOTIFY_CARD_ERROR_REQ:
  case MMGSDI_NOTIFY_SIM_BUSY_REQ:
  case MMGSDI_INTERNAL_PUP_INIT_REQ:
  case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
  case MMGSDI_RECOVERY_IND_REQ:
  case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ:
  case MMGSDI_SET_BUILTIN_PLMN_LIST_REQ:
  case MMGSDI_SIMLOCK_RSP:
  case MMGSDI_SIMLOCK_EVT:
  case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
  case MMGSDI_HANDLE_NV_REFRESH_REQ:
  case MMGSDI_FULL_SERVICE_REQ:
  case MMGSDI_SAP_CARD_READER_STATUS_REQ:
  case MMGSDI_MANAGE_CHANNEL_REQ:
  case MMGSDI_GET_SIM_PROFILE_REQ:
  case MMGSDI_SET_SIM_PROFILE_REQ:
  case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
  case MMGSDI_MAX_CMD_ENUM:
    break;
  /* Note - There is no default case here and it is done on purpose since we
    need to handle all values for mmgsdi_cmd_enum_type. Unhandled values
    will get flagged by lint error */
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_alloc_and_populate_extra_param */


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
)
{
  boolean result = FALSE;

  switch ( slot )
  {
    case MMGSDI_SLOT_1:
    case MMGSDI_SLOT_2:
    case MMGSDI_SLOT_3:
      result = TRUE;
      break;
    default:
      MMGSDI_DEBUG_MSG_ERROR_1("Invalid Slot: %x", slot);
      break;
  }
  return result;
} /* mmgsdi_util_is_slot_valid */


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
)
{
  if ( data_len <= 0 )
    return FALSE;

  /* --------------------------------------------------------------------------
     Allow the maximum for Send and Receive.  Allow the Send portion
     to regulate the length further and trim it down to
     ------------------------------------------------------------------------*/
  if ( data_len >= MMGSDI_MAX_CHANNEL_DATA_RSP_LEN )
    return FALSE;

  return TRUE;
} /* mmgsdi_chnl_mgt_is_data_len_valid*/


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
)
{
  if ( data_buffer == NULL )
    return FALSE;

  return TRUE;
} /* mmgsdi_chnl_mgt_is_data_buffer_valid */


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
  uint16       req_data_len)
{
  char ascii[17] = "0123456789ABCDEF";
  /*
   * For converting decimal to ascii. */

  char buffer[50];
  /*
   * Holds the ascii string representation of the data. */

  uint32 buffer_index = 0;
  /*
   * The last valid index in the buffer, will be set to '\0'. */

  int index;

  /*For traversing the data. */
  for (index = 0; index < req_data_len; index = index + 1) {
    /*
     * If the buffer is full flush it.
     */
    if ((buffer_index+3) >= sizeof(buffer)) {
      buffer[buffer_index] = '\0';
        MSG_SPRINTF_3(
          MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "%s(0x%x) %s ...\n", message, req_data_len, buffer);
      buffer_index = 0;
    }
    /*
     * Append HEX representation of the byte to the buffer.
     */
    if (req_data != NULL){
      buffer[buffer_index++] = ascii[req_data[index] / 16];
      buffer[buffer_index++] = ascii[req_data[index] % 16];
      buffer[buffer_index++] = ' ';
    }
  }
  /*
   * Flush the remainder of the buffer.
   */
  buffer[buffer_index] = '\0';
  MSG_SPRINTF_3(
    MSG_SSID_DFLT, MSG_LEGACY_HIGH,
    "%s(0x%x) %s\n", message, req_data_len, buffer);
} /* mmgsdi_util_print_data_to_diag */


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
)
{
  uint16     index        = 0;
  uint16     len          = 0;
  uint16     tlv_len      = 0;
  uint16     offset       = 0;
  uint8      TAG_SIZE     = 1;
  uint8      LEN_SIZE     = 1;
  uint8 * tlv_data_ptr = NULL;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(onchip_config_data_ptr);
  if ( onchip_config_data_ptr->data_len == 0 )
  {
    /* A Zero Length is allowed for the purpose of
    ** getting to an idle screen but the call stacks
    ** will be stuck in limited service
    */
    return MMGSDI_SUCCESS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(onchip_config_data_ptr->data_ptr);

  if (onchip_config_data_ptr->data_len < MMGSDI_ONCHIP_TLV_PAYLOAD ||
      onchip_config_data_ptr->data_ptr[0] != MMGSDI_ACT_ONCHIP_TLV_START_TAG)
  {
    /* onchip data(TLV) must atleast have a master tag and two bytes for
       length of the value field of onchip data */
    return MMGSDI_ERROR;
  }

  /* byte 2 and 3 contain the length of the master TLV */
  tlv_len = onchip_config_data_ptr->data_ptr[1] + (onchip_config_data_ptr->data_ptr[2] * 0x100);
  tlv_data_ptr = onchip_config_data_ptr->data_ptr + MMGSDI_ONCHIP_TLV_PAYLOAD;
  index = MMGSDI_ONCHIP_TLV_PAYLOAD;

  while (index < onchip_config_data_ptr->data_len )
  {
    /* Initialize the offset to 0 */
    offset = 0;

    switch ( *tlv_data_ptr )
    {
      case MMGSDI_ACT_ONCHIP_CK_TAG:
      case MMGSDI_ACT_ONCHIP_IK_TAG:
      case MMGSDI_ACT_ONCHIP_EPSLOCI_TAG:
      case MMGSDI_ACT_ONCHIP_HPLMNWACT_TAG:
      case MMGSDI_ACT_ONCHIP_PLMNWACT_TAG:
      case MMGSDI_ACT_ONCHIP_UST_TAG:
      case MMGSDI_ACT_ONCHIP_EST_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ALGO_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_RES_LEN_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_MILENAGE_OP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_MILENAGE_SEQ_TAG:
      case MMGSDI_ACT_ONCHIP_EPSNSC_TAG:
      case MMGSDI_ACT_ONCHIP_KEYSPS_TAG:
      case MMGSDI_ACT_ONCHIP_KEYS_TAG:
      case MMGSDI_ACT_ONCHIP_RPLMNACT_TAG:
        /* These tags are valid only in case of usim */
        if (onchip_sim_mode != MMGSDI_ONCHIP_MODE_USIM)
        {
          MMGSDI_DEBUG_MSG_ERROR_1("Invalid tag for onchip SIM: 0x%x",
                                   *tlv_data_ptr);
          return MMGSDI_ERROR;
        }
        /* continue without breaking because the checks below
           apply to both SIM and USIM tags */
        //lint -fallthrough

      case MMGSDI_ACT_ONCHIP_AUTH_KEY_TAG:
      case MMGSDI_ACT_ONCHIP_IMSI_TAG:
      case MMGSDI_ACT_ONCHIP_LOCI_TAG:
      case MMGSDI_ACT_ONCHIP_KC_TAG:
      case MMGSDI_ACT_ONCHIP_PLMN_TAG:
      case MMGSDI_ACT_ONCHIP_SST_TAG:
      case MMGSDI_ACT_ONCHIP_ACC_TAG:
      case MMGSDI_ACT_ONCHIP_FPLMN_TAG:
      case MMGSDI_ACT_ONCHIP_PHASE_TAG:
      case MMGSDI_ACT_ONCHIP_AD_TAG:
      case MMGSDI_ACT_ONCHIP_PLMNSEL_TAG:
      case MMGSDI_ACT_ONCHIP_ICCID_TAG:
      case MMGSDI_ACT_ONCHIP_KCGPRS_TAG:
        /* Validate the number of bytes remainging are valid
        ** in the TLV.  -1 for the purpose of converting
        ** entire data length to respective index
        */
        if ( index >= ( onchip_config_data_ptr->data_len - LEN_SIZE ) )
        {
          MMGSDI_DEBUG_MSG_ERROR_0("Invalid TLV Format...No Length");
          return MMGSDI_ERROR;
        }

        /* move the index to the 1 Byte Length past the Tag*/
        tlv_data_ptr++;

        offset = *tlv_data_ptr + LEN_SIZE;

        /* Move the tlv_data_ptr to the next Tag */
        tlv_data_ptr += offset;

        /* Now add the Length to the Offset */
        offset += TAG_SIZE;

        /* Update the Length accordingly as well */
        len += offset;

        index += offset;
        break;

      default:
        MMGSDI_DEBUG_MSG_ERROR_1("Unhandled Tag: 0x%x",
                                 ((*tlv_data_ptr - offset) - TAG_SIZE));
        return MMGSDI_ERROR;
    }
  }

  /* --------------------------------------------------------------------------
     Check to ensure the TLV Len is equal to the SUM of all Tags and Data
     contained within this config tlv
     ------------------------------------------------------------------------*/
  if ((tlv_len != len) ||
      (tlv_len + MMGSDI_ONCHIP_TLV_PAYLOAD != onchip_config_data_ptr->data_len))
  {
    MMGSDI_DEBUG_MSG_ERROR_3("BAD TLV:  TLV LEN=0x%x, LEN=0x%x, TOTAL LEN=0x%x",
                             tlv_len, len, onchip_config_data_ptr->data_len);
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_validate_onchip_config */


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
)
{
  mmgsdi_session_info_type       *session_info_ptr = NULL;
  mmgsdi_slot_id_enum_type        slot             = MMGSDI_MAX_SLOT_ID_ENUM;
  uint32                          i                = 0;
  uint32                          j                = 0;
  mmgsdi_len_type                 num_sessions     = 0;
  mmgsdi_return_enum_type         mmgsdi_status    = MMGSDI_ERROR;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  if(evt_info_ptr == NULL)
  {
    return;
  }

  for(i=0; i<MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if(mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    num_sessions     = mmgsdi_client_id_reg_table[i]->num_session;

    /* Scan through all open sessions and send out the REFRESH event to
       valid sessions on slot 1*/
    for(j=0; (num_sessions > 0) && (j < MMGSDI_MAX_SESSION_INFO); j++)
    {
      session_info_ptr =
        mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

      if(session_info_ptr == NULL)
      {
        /*Invalid Session, Continue to next*/
        continue;
      }
      num_sessions--;

      mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_info_ptr->session_id,
                        &slot,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL);

      if(mmgsdi_status != MMGSDI_SUCCESS || slot != mmgsdi_slot)
      {
        continue;
      }
      /* Fetch the app type from current app */
      if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL))
      {
        evt_info_ptr->data.refresh.aid.app_type =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.app_type;
        evt_info_ptr->data.refresh.app_info.app_data.app_type =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.app_type;
      }
      mmgsdi_evt_notify_specific_session_id(session_info_ptr->session_id,
                                            evt_info_ptr);
    }
  }
} /* mmgsdi_util_send_refresh_evt_for_onchip_act */


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
)
{
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(session_info_ptr);

  if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
     (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
  {
    UIM_MSG_ERR_1("Channel info entry is invalid for session_id=0x%x",
                  session_info_ptr->session_id);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
  if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections > 1)
  {
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections--;
  }
  else if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections == 1)
  {
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections--;

    /* Reset channel and app data only if the channel was actually closed. */
    if (!mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_termination_skipped)
    {
      slot_data_ptr =
        mmgsdi_util_get_slot_data_ptr(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id);
      if(slot_data_ptr != NULL)
      {
        slot_data_ptr->available_logical_channels++;
      }

      if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL)
      {
        mmgsdi_util_reset_app_info(session_info_ptr->channel_info_index);
      }

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]);
    }
    else
    {
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->selective_act_after_app_term_skipped =
            FALSE;
      if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL)
      {
        if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state >
             MMGSDI_APP_STATE_UNKNOWN)
        {
            mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state =
              MMGSDI_APP_STATE_DETECTED;
        }

        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->perso_state =
            MMGSDI_SIMLOCK_NOT_VERIFIED;
        mmgsdi_util_app_eons_cleanup(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr);
      }
    }
  }
  session_info_ptr->channel_info_index = MMGSDI_MAX_CHANNEL_INFO;

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_card_session_cleanup */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_SESSION_CLOSE_CLEANUP_AND_NOTIFY

   DESCRIPTION:
     This function calls corresponding function cleanup session information,
     and notify client based on the corresponding client_index and session_index
     into the mmgsdi_client_id_reg_table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:
     The Session information for the indices will be cleanup, which include
     freeing up any relevant information for the session

==========================================================================*/
void mmgsdi_util_session_close_cleanup_and_notify(
  int32 client_index,
  int32 session_index
)
{
  mmgsdi_event_data_type         event_notify_data;
  mmgsdi_return_enum_type        mmgsdi_status          = MMGSDI_SUCCESS;
  mmgsdi_session_info_type      *session_info_ptr       = NULL;
  mmgsdi_evt_session_notify_type event_notify_info;
  /* Card Sessions should not be closed while cleaning Sessions due to
     Card errors etc. Card Sessions need to be closed only when an explicit
     request is made.  Having req_type as MMGSDI_SESSION_DEACTIVATE_REQ
     ensures that Card Sessions do not get closed when
     mmgsdi_session_release_info() is invoked. */
  mmgsdi_cmd_enum_type           req_type               = MMGSDI_SESSION_DEACTIVATE_REQ;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  memset(&event_notify_data, 0x00, sizeof(mmgsdi_event_data_type));
  memset(&event_notify_info, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  /* Range check for client index and session index */
  if((client_index < 0) || (client_index >= MMGSDI_MAX_CLIENT_INFO) ||
     (session_index < 0) || (session_index >= MMGSDI_MAX_SESSION_INFO))
  {
    UIM_MSG_ERR_2("Invalid param client_index=0x%x, session_index=0x%x",
                  client_index, session_index);
    return;
  }

  if(mmgsdi_client_id_reg_table[client_index] == NULL)
  {
    UIM_MSG_ERR_1("Invalid client index: 0x%x", client_index);
    return;
  }

  memset(&event_notify_data, 0x00, sizeof(mmgsdi_event_data_type));
  event_notify_data.evt = MMGSDI_MAX_EVT_ENUM;

  session_info_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if(session_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Ptr sesion_info_ptr");
    return;
  }
  else if((session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
  {
    UIM_MSG_ERR_0("Invalid channel info entry");
    return;
  }

  /* build the session close event for non-prov applications only */
  /* Notify the specific client only */
  if (mmgsdi_util_is_prov_session(session_info_ptr->session_type))
  {
    uint8 prov_index = MMGSDI_MAX_PROV_APPS;

    req_type = MMGSDI_SESSION_DEACTIVATE_REQ;

    prov_index = mmgsdi_util_get_prov_index_from_session(session_info_ptr->session_type);

    /* If there is card error/recovery/deactivation request then send error
       confirmation for pending activation request only. Session changed event
       with session deactivated is not sent because session changed with session
       activated was not sent earlier.*/
    if(prov_index < MMGSDI_MAX_PROV_APPS &&
       mmgsdi_generic_data_ptr != NULL &&
       mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index] != NULL &&
       mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index]->client_req_cb != NULL)
    {
      (mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index]->client_req_cb)
        (MMGSDI_ERROR,
         MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF,
         &mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index]->mmgsdi_cnf);

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index]);
    }
    else if(session_info_ptr->session_evt_cb_ptr != NULL)
    {
      if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr
           != NULL)
      {
        UIM_MSG_HIGH_1("Sending Session changed evt - 0x%x",
                       session_info_ptr->session_id);
        event_notify_info.session_id = session_info_ptr->session_id;
        event_notify_info.slot_id =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id;
        event_notify_info.notify_type = MMGSDI_EVT_NOTIFY_SINGLE_SESSION;
        mmgsdi_evt_build_and_send_session_changed(
          event_notify_info,
          FALSE,
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr);
      }
      else
      {
        UIM_MSG_HIGH_1("Null app_info_ptr for session: 0x%x",
                       session_info_ptr->session_id);
      }
    }
    else
    {
      UIM_MSG_HIGH_1("Unable to send session changed evt, null event cb ptr- session id = 0x%x",
                     session_info_ptr->session_id);
    }
  }
  else if (mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
  {
    req_type = MMGSDI_SESSION_CLOSE_REQ;
    if(session_info_ptr->session_evt_cb_ptr != NULL)
    {
      UIM_MSG_HIGH_1("Closing non prov session and sending out Session close evt - 0x%x",
                     session_info_ptr->session_id);
      mmgsdi_status = mmgsdi_evt_build_session_close(
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id,
        session_info_ptr->session_id,
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr,
        &event_notify_data);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Unable to build event data to notify client - session id = 0x%x",
                      session_info_ptr->session_id);
      }
      else if(session_info_ptr->session_evt_cb_ptr != NULL)
      {
        UIM_MSG_HIGH_1("Non prov session closed cause 0x%x",
                       event_notify_data.data.session_close.cause);

        /* Notify the specific client only */
        session_info_ptr->session_evt_cb_ptr(&event_notify_data);
      }
      mmgsdi_evt_free_data(&event_notify_data);
    }
    else
    {
      UIM_MSG_HIGH_1("Unable to send session closed evt, null event cb ptr - session id = 0x%x",
                     session_info_ptr->session_id);
    }
  }

  /* Perform cleanup for the session */
  mmgsdi_status = mmgsdi_session_release_info(session_info_ptr,
                                              req_type);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("Unable to cleanup for table[0x%x][0x%x]",
                  client_index, session_index);
  }
}/* mmgsdi_util_session_close_cleanup_and_notify */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_ICC_UPDATE_CARD_SESSION_INFO

   DESCRIPTION:
     This function maps Card Sessions with a provisioning session
     active on Channel 0 of the requested slot.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Can be used only for ICC applications.

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
     The function uses mmgsdi_client_app_data_crit_sect_ptr to protect
     accesses to the Channel Info table.
==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_util_icc_update_card_session_info(
  mmgsdi_app_enum_type     app_type,
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_session_id_type    ch_zero_session_id = 0;
  mmgsdi_session_info_type *session_info_ptr   = NULL;
  uint32                    i                  = 0;
  uint32                    j                  = 0;
  mmgsdi_return_enum_type   mmgsdi_status      = MMGSDI_SUCCESS;
  uint8                     card_channel_index = MMGSDI_MAX_CHANNEL_INFO;

  /* If not an ICC application, return */
  if((app_type != MMGSDI_APP_SIM) &&
     (app_type != MMGSDI_APP_RUIM))
  {
    UIM_MSG_HIGH_1("Card Session info update not supported for app: 0x%x",
                   app_type);

    return MMGSDI_SUCCESS;
  }

  /* Get the Session ID corresponding to any active provisioning application
    on channel 0 for the requested slot */
  mmgsdi_status = mmgsdi_util_get_ch_zero_app(slot_id,
                                              NULL,
                                              &ch_zero_session_id);

  /* Return if no provisioning application is active on channel 0 */
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_SUCCESS;
  }

  /* Get the info corresponding to the Session */
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    ch_zero_session_id,
                    &session_info_ptr,
                    NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (session_info_ptr == NULL) ||
     (session_info_ptr->channel_info_index >= MMGSDI_MAX_CHANNEL_INFO) ||
     (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] == NULL))
  {
    UIM_MSG_ERR_1("Session/Channel info not available for Session: 0x%x",
                  ch_zero_session_id);

    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* Update card Sessions which are open on the same slot to map
     to the channel info of the channel 0 provisioning application */
  for(i=0; i<MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    for(j=0; j<MMGSDI_MAX_SESSION_INFO; j++)
    {
      if (mmgsdi_client_id_reg_table[i]->session_info_ptr[j] == NULL)
      {
        continue;
      }

      card_channel_index = mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index;
      if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
         (card_channel_index != session_info_ptr->channel_info_index))
      {
        if(((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id
               == MMGSDI_SLOT_1) &&
            (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
             MMGSDI_CARD_SESSION_SLOT_1))
                                          ||
           ((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id
               == MMGSDI_SLOT_2) &&
            (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
             MMGSDI_CARD_SESSION_SLOT_2))
                                          ||
           ((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id
               == MMGSDI_SLOT_3) &&
            (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
             MMGSDI_CARD_SESSION_SLOT_3)))
        {
          UIM_MSG_HIGH_1("Channel mapping updated for session_id=0x%x",
                         mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_id);

          if((card_channel_index < MMGSDI_MAX_CHANNEL_INFO) &&
             (mmgsdi_channel_info_ptr_table[card_channel_index] != NULL) &&
             (mmgsdi_channel_info_ptr_table[card_channel_index]->num_connections > 0))
          {
            mmgsdi_channel_info_ptr_table[card_channel_index]->num_connections--;
          }

          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections++;
          mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index =
            session_info_ptr->channel_info_index;
        }
      }
    } /* for j<MAX_SESSION_INFO */
  } /* for i<MAX_CLIENT_INFO */

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_icc_update_card_session_info */


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
)
{
  mmgsdi_int_app_info_type * app_info_ptr     = NULL;
  mmgsdi_session_info_type * session_info_ptr = NULL;
  mmgsdi_channel_info_type * channel_info_ptr = NULL;
  mmgsdi_session_info_type * curr_session_ptr = NULL;
  int32                      client_index     = 0;
  int32                      session_index    = 0;
  mmgsdi_slot_data_type    * slot_data_ptr    = NULL;
  mmgsdi_slot_id_enum_type   slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_app_enum_type       app_type         = MMGSDI_APP_UNKNOWN;
  uint8                      sess_prov_index  = MMGSDI_MAX_PROV_APPS;

  /* Get app info of the application being deactivated */
  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id, &slot_id, NULL, &app_info_ptr, &channel_info_ptr,
                    &session_info_ptr, NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL) ||
     (session_info_ptr == NULL) || (channel_info_ptr == NULL))
  {
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  sess_prov_index = mmgsdi_util_get_prov_index_from_session(session_info_ptr->session_type);
  if(sess_prov_index == MMGSDI_MAX_PROV_APPS)
  {
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
    return;
  }

  slot_data_ptr->prov_app_indices[sess_prov_index] = 0;

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  /* Get the app type */
  app_type = app_info_ptr->app_data.app_type;

  /* Scan through all open sessions to identify the ones linked to the prov. app
        being deactivated */
  for (client_index = 0; client_index < MMGSDI_MAX_CLIENT_INFO; client_index++)
  {
    if (mmgsdi_client_id_reg_table[client_index] == NULL)
    {
      continue;
    }

    for (session_index = 0; session_index < MMGSDI_MAX_SESSION_INFO; session_index++)
    {
      curr_session_ptr =
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

      if ((curr_session_ptr != NULL) &&
          (curr_session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
          (mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index] != NULL) &&
          (mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->app_info_ptr != NULL))
      {
        if (slot_id != mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->slot_id)
        {
          continue;
        }

        if (mmgsdi_util_cmp_aid(
              &mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->app_info_ptr->app_data,
              &app_info_ptr->app_data, FALSE) == MMGSDI_SUCCESS)
        {
          if (mmgsdi_util_is_prov_session(curr_session_ptr->session_type))
          {
            /* For prov. sessions, Session info shall be cleaned-up and
               SESSION_CHANGED event shall be sent out */
            mmgsdi_util_session_close_cleanup_and_notify(
              client_index, session_index);
          }
          else if (mmgsdi_util_is_non_prov_session(curr_session_ptr->session_type))
          {
            /* For non-prov. sessions, the session shall be closed and
               SESSION_CLOSED event shall be sent out */
            if(mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->channel_id
                 == channel_info_ptr->channel_id)
            {
              mmgsdi_util_session_close_cleanup_and_notify(
                client_index, session_index);
            }
          }
          else if (mmgsdi_util_is_card_session(curr_session_ptr->session_type))
          {
            /* Card sessions shall only be cleaned-up (not closed) and
               no events shall be sent out */
            mmgsdi_status = mmgsdi_util_card_session_cleanup(curr_session_ptr);
          }
        }
      }
    }
  }
  /* If a card session is mapped to the provisioning application deactivated,
     then the card session will also be delinked from the application.
     On ICC cards, it is possible that another provisioning
     application (SIM/RUIM) is still active on channel 0.
     Attempt to map card sessions to any provisioning session that is still
     active on channel 0 */
  (void)mmgsdi_util_icc_update_card_session_info(app_type, slot_id);
} /* mmgsdi_util_prov_session_deactivate_cleanup_and_notify */


/* ==========================================================================
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

==========================================================================*/
void mmgsdi_util_close_all_sessions_cleanup_and_notify(
  mmgsdi_slot_id_enum_type slot_id)
{
  int32                       client_index        = 0;
  int32                       session_index       = 0;
  int32                       num_session_done    = 0;
  int32                       num_sessions        = 0;
  mmgsdi_session_info_type  * session_info_ptr    = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  for (client_index = 0; client_index < MMGSDI_MAX_CLIENT_INFO; client_index++)
  {
    if (mmgsdi_client_id_reg_table[client_index] == NULL)
    {
      continue;
    }

    num_session_done = 0;
    num_sessions = mmgsdi_client_id_reg_table[client_index]->num_session;

    /* loop until either
       1) we reach the max sessions supported or
       2) we already cleanup and notify all the open sessions */
    for (session_index = 0;
         ((session_index < MMGSDI_MAX_SESSION_INFO) &&
         (num_session_done != num_sessions)
         );
         session_index++)
    {
      session_info_ptr =
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];
      if ((session_info_ptr != NULL) &&
          (session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == slot_id))
      {
        /* the channel is opened */
        mmgsdi_util_session_close_cleanup_and_notify(client_index, session_index);
        num_session_done++;
      }
    }
  }

  /* Free channel info data of any channels on this slot which are not
     linked with any session and where app termination was skipped earlier
     (achived by passing UIM_MAX_CHANNELS to this function). */
  mmgsdi_util_reset_unused_channel_app_data(slot_id, UIM_MAX_CHANNELS);
}/* mmgsdi_util_close_all_sessions_cleanup_and_notify */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_IS_DEFAULT_CHANNEL_IN_USE_UICC_APPS

   DESCRIPTION:
     This function goes through all the entries in the channel info table and
     checks if the default channel is being used by a UICC app.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     boolean

   SIDE EFFECTS:
     None
==========================================================================*/
boolean mmgsdi_util_check_is_default_channel_in_use_uicc_apps(
  mmgsdi_slot_id_enum_type slot_id)
{
  mmgsdi_channel_info_type * curr_channel_ptr          = NULL;
  uint8                      channel_index             = 0;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  /* Scan through entries of channel info table to check if the default channel
     has USIM or CSIM active */
  for (channel_index = 0; channel_index < MMGSDI_MAX_CHANNEL_INFO; channel_index++)
  {
    if (mmgsdi_channel_info_ptr_table[channel_index] == NULL)
    {
      continue;
    }

    curr_channel_ptr = mmgsdi_channel_info_ptr_table[channel_index];

    if (curr_channel_ptr->slot_id != slot_id ||
        curr_channel_ptr->channel_id != MMGSDI_DEFAULT_CHANNEL ||
        curr_channel_ptr->app_info_ptr == NULL)
    {
      continue;
    }

    if (curr_channel_ptr->app_info_ptr->app_data.app_type == MMGSDI_APP_USIM ||
        curr_channel_ptr->app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM)
    {
      return TRUE;
    }
  }
  return FALSE;
} /* mmgsdi_util_check_is_default_channel_in_use_uicc_apps */


/*===========================================================================
FUNCTION MMGSDI_UTIL_GET_USIM_APPS

DESCRIPTION
  This function is used to get the number of usim apps.

DEPENDENCIES
  None

RETURN VALUE
  uint32: Number of USIM Apps

SIDE EFFECTS
  None
===========================================================================*/
uint32 mmgsdi_util_get_usim_apps(mmgsdi_slot_id_enum_type slot)
{
  uint32 num_apps                      = 0;
#ifdef FEATURE_MMGSDI_3GPP
  uint32 i                             = 0;
  mmgsdi_slot_data_type *data_slot_ptr = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  data_slot_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(data_slot_ptr == NULL || data_slot_ptr->app_info_pptr == NULL)
  {
      return num_apps;
  }

  for(i=0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
  {
    if(data_slot_ptr->app_info_pptr[i] == NULL)
    {
      continue;
    }
    if(data_slot_ptr->app_info_pptr[i]->app_data.app_type == MMGSDI_APP_USIM &&
       data_slot_ptr->app_info_pptr[i]->app_data.aid.data_len > 0)
    {
      num_apps++;
    }
  }
#endif /* FEATURE_MMGSDI_3GPP */
  return num_apps;
} /* mmgsdi_util_get_usim_apps */


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
  boolean               *is_aram_aid_ptr)
{
  mmgsdi_static_data_type         aram_aid =
    {9, {0xA0, 0x00, 0x00, 0x01, 0x51, 0x41, 0x43, 0x4C, 0x00}};

  MMGSDIUTIL_RETURN_IF_NULL_2(aid_ptr, is_aram_aid_ptr);

  *is_aram_aid_ptr = FALSE;

  if(aid_ptr->aid.data_len < aram_aid.data_len)
  {
    return MMGSDI_SUCCESS;
  }

  if(memcmp((void*)aid_ptr->aid.data_ptr,
            (void*)aram_aid.data_ptr,
            aram_aid.data_len) == 0)
  {
    *is_aram_aid_ptr = TRUE;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_util_is_aram_aid */


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
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_cmp_aid(
  const mmgsdi_aid_type *aid1_ptr,
  const mmgsdi_aid_type *aid2_ptr,
  boolean                ignore_icc_aid)
{
  MMGSDIUTIL_RETURN_IF_NULL_2(aid1_ptr, aid2_ptr);

  /* For SIM / RUIM app only compare APP type  */
  if((aid1_ptr->app_type == MMGSDI_APP_SIM) ||
     (aid1_ptr->app_type == MMGSDI_APP_RUIM)||
     (aid2_ptr->app_type == MMGSDI_APP_SIM) ||
     (aid2_ptr->app_type == MMGSDI_APP_RUIM))
  {
    if (ignore_icc_aid == TRUE)
    {
      /* Ignoring APP type comparison for ICC card */
      return MMGSDI_SUCCESS;
    }

    if(aid1_ptr->app_type == aid2_ptr->app_type)
    {
      return MMGSDI_SUCCESS;
    }
    else
    {
      return MMGSDI_ERROR;
    }
  }

  if((aid1_ptr->aid.data_len > UIM_AID_MAX_NAME_LENGTH) ||
     (aid1_ptr->aid.data_len < 0))
  {
    UIM_MSG_ERR_1("AID1 len 0x%x exceed UIM_AID_MAX_NAME_LENGTH",
                  aid1_ptr->aid.data_len);
    return MMGSDI_ERROR;
  }

  if((aid2_ptr->aid.data_len > UIM_AID_MAX_NAME_LENGTH) ||
     (aid2_ptr->aid.data_len < 0))
  {
    UIM_MSG_ERR_1("AID2 len 0x%x exceed UIM_AID_MAX_NAME_LENGTH",
                  aid2_ptr->aid.data_len);
    return MMGSDI_ERROR;
  }

  /* If AID lengths do not match, return error */
  if(aid1_ptr->aid.data_len != aid2_ptr->aid.data_len)
  {
    return MMGSDI_ERROR;
  }

  /* If both AIDs have 0 length, return success only if both AIDs correspond
     to a valid application */
  if(aid1_ptr->aid.data_len == 0)
  {
    if((aid1_ptr->app_type == aid2_ptr->app_type) &&
       (aid1_ptr->app_type != MMGSDI_APP_NONE))
    {
      return MMGSDI_SUCCESS;
    }
    UIM_MSG_ERR_2("App type mismatch/invalid, AID1 app type: 0x%x, AID2 app type 0x%x",
                  aid1_ptr->app_type, aid1_ptr->app_type);
    return MMGSDI_ERROR;
  }

  if(memcmp((void*)aid1_ptr->aid.data_ptr,
            (void*)aid2_ptr->aid.data_ptr,
            aid1_ptr->aid.data_len) == 0)
  {
    return MMGSDI_SUCCESS;
  }
  return MMGSDI_ERROR;
}/* mmgsdi_util_cmp_aid */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_INTERNAL_CB

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
----------------------------------------------------------------------------*/
void mmgsdi_util_internal_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_int_app_info_type          *app_info_ptr         = NULL;
  mmgsdi_request_header_type         req_header;
  uim_cmd_type                      *uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr      = NULL;
  mmgsdi_callback_cnf_type          *callback_cnf_ptr     = NULL;
  mmgsdi_slot_id_enum_type           slot_id              = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_id_type             session_id           = 0;
  mmgsdi_session_id_status_enum_type session_status       = MMGSDI_SESSION_ID_UNASSIGNED;
  int32                              client_index         = 0;
  mmgsdi_nv_context_type             sub_nv_context       = MMGSDI_NV_CONTEXT_INVALID;

  UIM_MSG_HIGH_2("MMGSDI_INTERNAL_CB: Rev'd Cnf=0x%x, Status=0x%x", cnf, status);

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  if (cnf == MMGSDI_MAX_CNF_ENUM)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("Invalid Cnf Enum");
    return;
  }

  if (cnf_ptr == NULL)
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("Null Confirmation Ptr");
    return;
  }

  (void)mmgsdi_util_get_session_app_info(
          cnf_ptr->response_header.session_id,
          &slot_id,
          NULL,
          &app_info_ptr,
          NULL,
          NULL,
          NULL);
  switch(cnf)
  {
    case MMGSDI_GET_ALL_PIN_STATUS_CNF:
      if(status != MMGSDI_SUCCESS &&
         status != MMGSDI_CODE_BLOCKED &&
         status != MMGSDI_CODE_PERM_BLOCKED)
      {
        UIM_MSG_ERR_0("GET PIN STATUS FAILED");

        /* End any ongoing REFRESH on the Session */
       (void)mmgsdi_card_init_end_refresh_in_progress(
               cnf_ptr->response_header.session_id, slot_id);

        /* Update the session closure cause. The session changed event
           broadcasts the cause */
        mmgsdi_util_update_session_closed_cause(cnf_ptr->response_header.session_id,
                                                MMGSDI_SESSION_CLOSED_CAUSE_PIN_STATUS_FAIL);

        /* Post deactivate session request with internal_deact_req as TRUE */
        (void)mmgsdi_session_create_and_queue_internal_deactivate_req(
                cnf_ptr->response_header.session_id);
      }
      else
      {
        /* In the case that device is activated due to a Recovery failure, we should
           verify the PIN silently in order to provide a better user experience.*/
        if (mmgsdi_silent_pin_is_cached(slot_id)                       &&
            cnf_ptr->get_all_pin_status_cnf.pin1.status == MMGSDI_PIN_ENABLED_NOT_VERIFIED)
        {
          if(mmgsdi_silent_pin_verify_pin(cnf_ptr->response_header.session_id,
                                          app_info_ptr,
                                          slot_id))
        {
            /* If PIN is verified silently wait for verify PIN response before
               sending events to clients */
            break;
          }
        }

        /* In the case that silent PIN verification is not performed and we
           proceed make sure the silent PIN cache is cleared as the data may
           become stale.*/
        mmgsdi_silent_pin_purge_info(slot_id);

          /*Send the PIN Event*/
          mmgsdi_cnf_generate_get_all_pin_status_evt(
            cnf_ptr->response_header.session_id);
        }
      break;

    case MMGSDI_SESSION_ENABLE_SERVICE_CNF:
      client_index = (int32)cnf_ptr->response_header.client_data;

      mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                  client_index,
                                  &req_header,
                                  &uim_cmd_ptr,
                                  &extra_param_ptr);
      if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
      {
        mmgsdi_util_free_client_request_table_index(client_index);
        break;
      }

      /* If a confirmation has already been sent for this command as part of
         CARD_ERROR handling, free only the client_req_table index. */
      if((client_index >= 0) && (client_index < MMGSDI_MAX_CLIENT_REQ_INFO) &&
         (mmgsdi_client_req_table_info_ptr[client_index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED))
      {
        UIM_MSG_HIGH_2("Free client_req_table index 0x%x for cmd 0x%x",
                       client_index,
                       mmgsdi_client_req_table_info_ptr[client_index]->request_type);

        mmgsdi_util_free_client_request_table_index(client_index);
        break;
      }

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(callback_cnf_ptr, sizeof(mmgsdi_callback_cnf_type));
      if(callback_cnf_ptr == NULL)
      {
        mmgsdi_util_free_client_request_table_index(client_index);
        break;
      }

      mmgsdi_memscpy(&callback_cnf_ptr->mmgsdi_cnf, sizeof(mmgsdi_cnf_type), cnf_ptr,sizeof(mmgsdi_cnf_type));

      callback_cnf_ptr->mmgsdi_cnf.response_header.client_data =
                           extra_param_ptr->enable_service_data.client_data;
      callback_cnf_ptr->mmgsdi_cnf.session_enable_service_cnf.service_type =
                           extra_param_ptr->enable_service_data.service_type;
      callback_cnf_ptr->mmgsdi_cnf.session_enable_service_cnf.is_status_same =
                           extra_param_ptr->enable_service_data.is_status_same;

      /* Check if app_info_ptr is NULL before dereferencing it */
      if(callback_cnf_ptr->mmgsdi_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS &&
         app_info_ptr != NULL)
      {
        switch(extra_param_ptr->enable_service_data.service_type)
        {
          case MMGSDI_USIM_SRV_BDN:
          case MMGSDI_GSM_SRV_BDN:
            app_info_ptr->app_capabilities.bdn_state = MMGSDI_SRV_ENABLE;
            break;
          case MMGSDI_USIM_SRV_ACL:
            app_info_ptr->app_capabilities.acl_state = MMGSDI_SRV_ENABLE;
            break;
          case MMGSDI_USIM_SRV_FDN:
          case MMGSDI_CSIM_SRV_FDN:
          case MMGSDI_GSM_SRV_FDN:
          case MMGSDI_CDMA_SRV_FDN:
            app_info_ptr->app_capabilities.fdn_state = MMGSDI_SRV_ENABLE;
            break;
          default:
            break;
        }
      }

      callback_cnf_ptr->mmgsdi_cnf.response_header.response_type = MMGSDI_SESSION_ENABLE_SERVICE_CNF;
      callback_cnf_ptr->client_req_cb = extra_param_ptr->enable_service_data.orig_callback;

      mmgsdi_cnf_generate_rsp_and_evt(callback_cnf_ptr);

      mmgsdi_util_free_client_request_table_index(client_index);

      MMGSDIUTIL_TMC_MEM_FREE(callback_cnf_ptr);
      break;

    case MMGSDI_SESSION_DISABLE_SERVICE_CNF:
      client_index = (int32)cnf_ptr->response_header.client_data;

      mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                     client_index,
                                     &req_header,
                                     &uim_cmd_ptr,
                                     &extra_param_ptr);
      if (mmgsdi_status != MMGSDI_SUCCESS || extra_param_ptr == NULL)
      {
        mmgsdi_util_free_client_request_table_index(client_index);
        break;
      }

      /* If a confirmation has already been sent for this command as part of
         CARD_ERROR handling, free only the client_req_table index. */
      if((client_index >= 0) && (client_index < MMGSDI_MAX_CLIENT_REQ_INFO) &&
         (mmgsdi_client_req_table_info_ptr[client_index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED))
      {
        UIM_MSG_HIGH_2("Free client_req_table index 0x%x for cmd 0x%x",
                       client_index,
                       mmgsdi_client_req_table_info_ptr[client_index]->request_type);

        mmgsdi_util_free_client_request_table_index(client_index);
        break;
      }

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(callback_cnf_ptr, sizeof(mmgsdi_callback_cnf_type));
      if(callback_cnf_ptr == NULL)
      {
        mmgsdi_util_free_client_request_table_index(client_index);
        break;
      }

      mmgsdi_memscpy(&callback_cnf_ptr->mmgsdi_cnf,
                     sizeof(mmgsdi_cnf_type),
                     cnf_ptr,
                     sizeof(mmgsdi_cnf_type));

      callback_cnf_ptr->mmgsdi_cnf.response_header.client_data =
        extra_param_ptr->enable_service_data.client_data;
      callback_cnf_ptr->mmgsdi_cnf.session_enable_service_cnf.service_type =
        extra_param_ptr->enable_service_data.service_type;
      callback_cnf_ptr->mmgsdi_cnf.session_enable_service_cnf.is_status_same =
                      extra_param_ptr->enable_service_data.is_status_same;

      if(callback_cnf_ptr->mmgsdi_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS &&
         app_info_ptr != NULL)
      {
        switch(extra_param_ptr->enable_service_data.service_type)
        {
          case MMGSDI_USIM_SRV_BDN:
          case MMGSDI_GSM_SRV_BDN:
            app_info_ptr->app_capabilities.bdn_state = MMGSDI_SRV_DISABLE;
            break;
          case MMGSDI_USIM_SRV_ACL:
            app_info_ptr->app_capabilities.acl_state = MMGSDI_SRV_DISABLE;
            break;
          case MMGSDI_USIM_SRV_FDN:
          case MMGSDI_CSIM_SRV_FDN:
          case MMGSDI_GSM_SRV_FDN:
          case MMGSDI_CDMA_SRV_FDN:
            app_info_ptr->app_capabilities.fdn_state = MMGSDI_SRV_DISABLE;
            break;
          default:
            break;
        }
      }

      callback_cnf_ptr->mmgsdi_cnf.response_header.response_type = MMGSDI_SESSION_DISABLE_SERVICE_CNF;
      callback_cnf_ptr->client_req_cb = extra_param_ptr->enable_service_data.orig_callback;

      mmgsdi_cnf_generate_rsp_and_evt(callback_cnf_ptr);

      mmgsdi_util_free_client_request_table_index(client_index);

      MMGSDIUTIL_TMC_MEM_FREE(callback_cnf_ptr);
      break;

    case MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING_CNF:
      /* For multimode targets check the client data to see if a RUIM/CSIM app
         needs to be activated */
      if (cnf_ptr->response_header.client_data)
      {
        boolean                   app_1x_found   = FALSE;
        mmgsdi_data_type          app_1x_data;
        nv_stat_enum_type         result         = NV_STAT_ENUM_MAX;
        mmgsdi_nv_item_cache_type mmgsdi_nv_data = {0};

        memset(&app_1x_data, 0x00, sizeof(mmgsdi_data_type));

        /* Get RTRE configuration and if RTRE is NV_ONLY then don't
           auto-activate CSIM/RUIM apps as it is NV based not Card based */
        result = mmgsdi_nv_get_item(NV_RTRE_CONFIG_I, &mmgsdi_nv_data, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

        if((result == NV_DONE_S) &&
           (mmgsdi_nv_data.item_value.rtre_config != NV_RTRE_CONFIG_NV_ONLY))
        {
          mmgsdi_util_find_first_ruim_csim_app(cnf_ptr->response_header.slot_id,
                                               &app_1x_found,&app_1x_data);
        }

        if(app_1x_found == FALSE)
        {
          break;
        }

        switch (cnf_ptr->response_header.slot_id)
        {
          case MMGSDI_SLOT_1:
            session_id = mmgsdi_generic_data_ptr->pri_1x_session_id;
            break;
          case MMGSDI_SLOT_2:
            session_id = mmgsdi_generic_data_ptr->sec_1x_session_id;
            break;
          case MMGSDI_SLOT_3:
            session_id = mmgsdi_generic_data_ptr->ter_1x_session_id;
            break;
          default:
            return;
        }

        /*Activate 1x session only when CDMALess NV is disbaled */
        if((mmgsdi_util_get_efs_item_index(session_id, &sub_nv_context) == MMGSDI_SUCCESS) &&
           (mmgsdi_nv_get_external_feature_status(
              MMGSDI_EXT_FEAT_CDMA_LESS_DEVICE, sub_nv_context) == MMGSDI_FEATURE_ENABLED))
        {
          UIM_MSG_HIGH_2("CDMALess NV is enabled for 1x subscription context 0x%x slot 0x%x; not activating",
                          sub_nv_context, cnf_ptr->response_header.slot_id);
          break;
        }

        /* If the provisioning session is already active, do not attempt
           an activation */
        session_status = mmgsdi_util_is_session_id_valid(session_id);

        if (session_status == MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
        {
          UIM_MSG_HIGH_1("mmgsdi_util_internal_cb: Input session ID in use 0x%x",
                         session_id);
          break;
        }

        UIM_MSG_HIGH_0("Activating 1x app after SIM/USIM activtion attempt");
        if (mmgsdi_session_activate_or_switch_provisioning(
              session_id,
              cnf_ptr->response_header.slot_id,
              app_1x_data,
              mmgsdi_util_internal_cb,
              0) != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_0("1x (CSIM/RUIM) app act or switch failed");
        }
      }
      break;

    case MMGSDI_PIN_OPERATION_CNF:
      if (cnf_ptr->pin_operation_cnf.pin_op == MMGSDI_PIN_OP_VERIFY)
      {
        /* PIN event will be sent to MMGSDI clients once the verify PIN CNF
           is processed so no need to send it here. */

        UIM_MSG_HIGH_1("Verify PIN callback received, status: 0x%x",
                       cnf_ptr->pin_operation_cnf.pin_info.status);

        if(cnf_ptr->pin_operation_cnf.pin_info.status != MMGSDI_SUCCESS &&
           mmgsdi_silent_pin_is_cached(cnf_ptr->response_header.slot_id))
        {
          mmgsdi_silent_pin_purge_info(cnf_ptr->response_header.slot_id);
        }
      }
      break;

#ifdef FEATURE_UIM_MULTI_PROFILE
    case MMGSDI_SET_SIM_PROFILE_CNF:
      {
        boolean  send_card_inserted   = FALSE;

        /* We send card inserted event if the switch of profile has no effect. The
           card returns no effect when the requested profile is current active
           profile in the card */
        if(cnf_ptr->set_sim_profile_cnf.response_header.mmgsdi_status == MMGSDI_MULTI_PROFILE_NO_EFFECT)
        {
          send_card_inserted = TRUE;
        }
        /* If the profile switch is successful and regular profile is not yet
           detected then queue set sim profile request with profile as regular */
        else if(cnf_ptr->set_sim_profile_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS &&
              mmgsdi_profile_info_ptr->reg_profile_detected == FALSE)
        {
          mmgsdi_status = mmgsdi_set_sim_profile(mmgsdi_generic_data_ptr->client_id,
                                                 cnf_ptr->set_sim_profile_cnf.response_header.slot_id,
                                                 MMGSDI_PROFILE_1,
                                                 mmgsdi_util_internal_cb,
                                                 0);
          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
            UIM_MSG_ERR_1("Could not switch profile, status: 0x%x", mmgsdi_status);
            mmgsdi_multi_profile_update_to_single_profile_info();
            send_card_inserted = TRUE;
          }
        }
        else
        {
          send_card_inserted = FALSE;
        }

        /* We send card inserted event
           1) If the switch of profile is not success (other case which we expect to return is NO_EFFECT)
           2) If queuing of set sim profile command is not successful. */
        if(send_card_inserted)
        {
          UIM_MSG_HIGH_1("MMGSDI_CARD_INSERTED after profile detection, SLOT 0x%x ",
                         cnf_ptr->set_sim_profile_cnf.response_header.slot_id);
          mmgsdi_evt_build_and_send_card_inserted(
            TRUE,
            (mmgsdi_client_id_type)0,
            cnf_ptr->set_sim_profile_cnf.response_header.slot_id);
        }
      }
      break;

    case MMGSDI_SESSION_OPEN_CNF:
      {
        boolean  send_card_inserted   = FALSE;

        /* For multi profiles capable cards, if profile info global is present
           and AID matches then store the dedicated channel id in profile info global */
        if(cnf_ptr->session_open_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS &&
           mmgsdi_profile_info_ptr != NULL &&
           cnf_ptr->session_open_cnf.app_info.aid.data_len == mmgsdi_profile_info_ptr->aid.data_len &&
           (0 == memcmp(cnf_ptr->session_open_cnf.app_info.aid.data_ptr,
                   mmgsdi_profile_info_ptr->aid.data_ptr,
                   mmgsdi_profile_info_ptr->aid.data_len)))
        {
          mmgsdi_profile_info_ptr->dedicated_channel_id = cnf_ptr->session_open_cnf.channel_id;

          /* Multi profile AID is available in the card. Assume number of profile to 0x02 */
          mmgsdi_profile_info_ptr->num_profile          = 0x02;

          /* If regular profile is not detected then send the set sim
             profile command to the card. Otherwise, send card inserted event */
          if(mmgsdi_profile_info_ptr->reg_profile_detected == FALSE)
          {
            /* Mark get active profile flag as TRUE if there is a provision for retriving
               current active profile from card */
            if(mmgsdi_profile_info_ptr->get_active_profile_apdu.data_len > 0 &&
               mmgsdi_profile_info_ptr->get_active_profile_apdu.data_ptr != NULL)
            {
              mmgsdi_profile_info_ptr->get_active_profile = TRUE;
            }

            mmgsdi_status = mmgsdi_set_sim_profile(mmgsdi_generic_data_ptr->client_id,
                                                   cnf_ptr->session_open_cnf.slot_id,
                                                   MMGSDI_PROFILE_1,
                                                   mmgsdi_util_internal_cb,
                                                   0);
            if(mmgsdi_status != MMGSDI_SUCCESS)
            {
              send_card_inserted = TRUE;
              mmgsdi_multi_profile_update_to_single_profile_info();
            }
          }
          else
          {
            send_card_inserted = TRUE;
            mmgsdi_multi_profile_update_to_single_profile_info();
          }
        }
        else
        {
          mmgsdi_multi_profile_update_to_single_profile_info();
          send_card_inserted = TRUE;
        }

        if(send_card_inserted)
        {
          UIM_MSG_HIGH_1("MMGSDI_CARD_INSERTED due to failure of session open, SLOT 0x%x ",
                         cnf_ptr->session_open_cnf.slot_id);
          mmgsdi_evt_build_and_send_card_inserted(
            TRUE,
            (mmgsdi_client_id_type)0,
            cnf_ptr->session_open_cnf.slot_id);
        }

        break;
      }
#endif /* FEATURE_UIM_MULTI_PROFILE */

    case MMGSDI_SESSION_DEACTIVATE_CNF:
    /* During Onchip auto-activation procedures, we use this internal callback */
    case MMGSDI_ACTIVATE_ONCHIP_SIM_CNF:
    default:
      break;
  }
} /* mmgsdi_util_internal_cb */


/*===========================================================================
FUNCTION MMGSDI_UTIL_BUILD_PIN_REQ

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
  const mmgsdi_sw_status_type       *orig_sw_ptr)
{
  mmgsdi_pin_operation_req_type *pin_req_ptr   = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_num_retry_enum_type     retry_type    = MMGSDI_RETRY_NONE;

  /* No variable len to include */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(pin_req_ptr,
    sizeof(mmgsdi_pin_operation_req_type));
  if (pin_req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  pin_req_ptr->request_header.client_id         = req_header_ptr->client_id;
  pin_req_ptr->request_header.session_id        = req_header_ptr->session_id;
  pin_req_ptr->request_header.request_type      = MMGSDI_PIN_OPERATION_REQ;
  pin_req_ptr->request_header.orig_request_type = req_header_ptr->orig_request_type;
  pin_req_ptr->request_header.request_len       = sizeof(mmgsdi_pin_operation_req_type);
  pin_req_ptr->request_header.payload_len       = 0;
  pin_req_ptr->request_header.slot_id           = req_header_ptr->slot_id;
  pin_req_ptr->request_header.client_data       = req_header_ptr->client_data;
  pin_req_ptr->request_header.response_cb       = req_header_ptr->response_cb;

  pin_req_ptr->pin_id                       = pin_id;
  pin_req_ptr->new_pin.data_len             = 0;
  memset(pin_req_ptr->new_pin.data_ptr, 0x00, MMGSDI_MAX_AID_LEN);
  pin_req_ptr->pin_op                       = orig_pin_op;
  pin_req_ptr->orig_pin_id                  = orig_pin_id;
  if (orig_sw_ptr != NULL)
  {
    pin_req_ptr->orig_sw = *orig_sw_ptr;
  }

  /* Store the user entered PIN from extra param info if VERIFY_PIN_AFTER_CHANGE feature is enabled.
     This is required to hold the new PIN data entered by the User to verify the PIN after it has changed */
  if((mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_VERIFY_PIN_AFTER_CHANGE, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED) &&
     (pin_data_ptr != NULL))
  {
    pin_req_ptr->pin_copy.data_len = pin_data_ptr->pin_data_copy.data_len;
    mmgsdi_memscpy(pin_req_ptr->pin_copy.data_ptr,
                   sizeof(pin_req_ptr->pin_copy.data_ptr),
                   pin_data_ptr->pin_data_copy.data_ptr,
                   sizeof(pin_data_ptr->pin_data_copy.data_ptr));
  }

  if(pin_op == MMGSDI_PIN_OP_UNBLOCK)
  {
    mmgsdi_status = mmgsdi_uim_common_unblock_pin(pin_req_ptr,
                                                  MMGSDI_GET_NUM_UNBLOCK_RETRIES,
                                                  protocol);
  }
  else if (pin_op == MMGSDI_PIN_OP_VERIFY)
  {
    /* Fill the PIN1 data which was cached earlier for verification of PIN1 to verify the PIN1.
       This is required to support the behavior of buggy card which requires PIN1 to be verified after it has changed */
    if((mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_VERIFY_PIN_AFTER_CHANGE, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED) &&
       (pin_data_ptr != NULL) &&
       (pin_data_ptr->retry_type == MMGSDI_GET_NUM_RETRIES))
    {
      pin_req_ptr->pin_puk.data_len = pin_data_ptr->pin_data_copy.data_len;
      mmgsdi_memscpy(pin_req_ptr->pin_puk.data_ptr,
                     sizeof(pin_req_ptr->pin_puk.data_ptr),
                     pin_data_ptr->pin_data_copy.data_ptr,
                     sizeof(pin_data_ptr->pin_data_copy.data_ptr));

      retry_type = MMGSDI_INTERNAL_VERIFY_AFTER_PIN_CHANGE;
    }
    else
    {
      retry_type = MMGSDI_GET_NUM_RETRIES;
    }

    mmgsdi_status = mmgsdi_uim_common_verify_pin(pin_req_ptr,
                                                 retry_type,
                                                 protocol);
  }

  MMGSDIUTIL_TMC_MEM_FREE(pin_req_ptr);
  return mmgsdi_status;
} /* mmgsdi_util_build_pin_req */


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
  mmgsdi_pin_extra_info_type        *pin_info_ptr)
{
  mmgsdi_get_file_attr_req_type       *req_ptr               = NULL;
  mmgsdi_return_enum_type              mmgsdi_status         = MMGSDI_SUCCESS;

  if (NULL == req_header_ptr)
  {
    if(pin_info_ptr)
    {
      MMGSDIUTIL_TMC_MEM_FREE(pin_info_ptr);
    }
    return MMGSDI_ERROR;
  }
    /* No variable len to include */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr,
    sizeof(mmgsdi_get_file_attr_req_type));
  if (req_ptr == NULL)
  {
    if(pin_info_ptr)
    {
      MMGSDIUTIL_TMC_MEM_FREE(pin_info_ptr);
    }
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  req_ptr->request_header.client_id         = req_header_ptr->client_id;
  req_ptr->request_header.session_id        = req_header_ptr->session_id;
  req_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  req_ptr->request_header.orig_request_type = req_header_ptr->orig_request_type;
  req_ptr->request_header.request_len       = sizeof(mmgsdi_get_file_attr_req_type);
  req_ptr->request_header.payload_len       = 0;
  req_ptr->request_header.slot_id           = req_header_ptr->slot_id;
  req_ptr->request_header.client_data       = req_header_ptr->client_data;
  req_ptr->request_header.response_cb       = req_header_ptr->response_cb;
  req_ptr->int_client_data                  = (mmgsdi_client_data_type) pin_info_ptr;
  req_ptr->activate_aid                     = TRUE;
  mmgsdi_memscpy(&req_ptr->access,
                 sizeof(req_ptr->access),
                 &access_type,
                 sizeof(mmgsdi_access_type));
  mmgsdi_status = mmgsdi_uim_icc_select(req_ptr, FALSE, NULL);
  /* Incase of error, do not free pin_info_ptr because mmgsdi_uim_icc_select()
     guarantees to free the memory pointed to by pin_info_ptr incase of error */
  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
  return mmgsdi_status;
} /* mmgsdi_util_build_get_file_attr_req */


/*===========================================================================
FUNCTION MMGSDI_UTIL_CAN_PROCEED_TO_POST_PIN1

DESCRIPTION
  This function checks the pin1 and univ pin status and the replacement
  condition to determine if post pin1 procedure is allowed or not

DEPENDENCIES
  None
LIMITATION
  Supports only MMGSDI_SLOT1 now.

RETURN VALUE
  TRUE:  Can proceed with Post PIN1 process
  FALSE: Cannot proceed with Post PIN1 process

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_util_can_proceed_to_post_pin1(
  const mmgsdi_int_app_info_type *sel_app_ptr)
{
  mmgsdi_app_pin_info_type  *pin1_ptr      = NULL;
  mmgsdi_app_pin_info_type  *univ_pin_ptr  = NULL;

  if(sel_app_ptr == NULL)
  {
    UIM_MSG_ERR_0("sel_app_ptr is NULL, Cannnot proceed with post pin1");
    return FALSE;
  }

  pin1_ptr = sel_app_ptr->pin1_ptr;
  univ_pin_ptr = sel_app_ptr->universal_pin_ptr;

  if (pin1_ptr == NULL)
  {
    UIM_MSG_ERR_0("PIN 1 ptr NULL, Cannnot proceed with post pin1");
    return FALSE;
  }

  switch(pin1_ptr->status)
  {
    case MMGSDI_PIN_BLOCKED:
    case MMGSDI_PIN_PERM_BLOCKED:
    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
    case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
      UIM_MSG_HIGH_1("PIN1 Status 0x%x (Enabled!Verified, Blked, Perm Blk, Not init), Cannot proceed with post pin1",
                     pin1_ptr->status);
      return FALSE;

    case MMGSDI_PIN_ENABLED_VERIFIED:
    case MMGSDI_PIN_UNBLOCKED:
    case MMGSDI_PIN_CHANGED:
      UIM_MSG_HIGH_1("PIN1 Status 0x%x (Enable&Verified, Changed or Unblocked), Can proceed with post pin1",
                     pin1_ptr->status);
      return TRUE;

    case MMGSDI_PIN_DISABLED:
      if (pin1_ptr->pin_replacement != MMGSDI_PIN_REPLACED_BY_UNIVERSAL)
      {
        UIM_MSG_HIGH_0("PIN1 Disabled No Replacement, Can proceed with post pin1");
        return TRUE;
      }
      /* If replaced by Universal PIN, check if Universal PIN is verified,
         if not, then can't proceed with post pin1 */
      if(univ_pin_ptr == NULL)
      {
        UIM_MSG_ERR_0("PIN1 Disabled W Replacement, Null Univ PIN ptr. Cannot proceed with post pin1");
        return FALSE;
      }
      switch(univ_pin_ptr->status)
      {
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        case MMGSDI_PIN_BLOCKED:
        case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
          UIM_MSG_HIGH_1("PIN1 Disabled W Replacement, UnivPin 0x%x (Enabled!Verified, Blked, Not Init), Cannot proceed with post pin1",
                         univ_pin_ptr->status);
          return FALSE;

        default:
          UIM_MSG_HIGH_1("PIN1 Disabled W Replacement, UnivPin Verified/Disable 0x%x, Can proceed with post pin1",
                         univ_pin_ptr->status);
          return TRUE;
      }
    case MMGSDI_PIN_RETRY_COUNT_DECREASED:
    case MMGSDI_PIN_NOT_FOUND:
      break;
  }
  UIM_MSG_ERR_1("Unknown pin1 status 0x%x, Cannot proceed with post pin1",
                pin1_ptr->status);
  return FALSE;
} /* mmgsdi_util_can_proceed_to_post_pin1 */


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
   mmgsdi_key_ref_values_enum_type key_ref)
{
  switch(key_ref)
  {
    case MMGSDI_PIN_APPL_1:
      return UIM_PIN1_APP1;
    case MMGSDI_PIN_APPL_2:
      return UIM_PIN1_APP2;
    case MMGSDI_PIN_APPL_3:
      return UIM_PIN1_APP3;
    case MMGSDI_PIN_APPL_4:
      return UIM_PIN1_APP4;
    case MMGSDI_PIN_APPL_5:
      return UIM_PIN1_APP5;
    case MMGSDI_PIN_APPL_6:
      return UIM_PIN1_APP6;
    case MMGSDI_PIN_APPL_7:
      return UIM_PIN1_APP7;
    case MMGSDI_PIN_APPL_8:
      return UIM_PIN1_APP8;
    case MMGSDI_SEC_PIN_APPL_1:
      return UIM_PIN2_APP1;
    case MMGSDI_SEC_PIN_APPL_2:
      return UIM_PIN2_APP2;
    case MMGSDI_SEC_PIN_APPL_3:
      return UIM_PIN2_APP3;
    case MMGSDI_SEC_PIN_APPL_4:
      return UIM_PIN2_APP4;
    case MMGSDI_SEC_PIN_APPL_5:
      return UIM_PIN2_APP5;
    case MMGSDI_SEC_PIN_APPL_6:
      return UIM_PIN2_APP6;
    case MMGSDI_SEC_PIN_APPL_7:
      return UIM_PIN2_APP7;
    case MMGSDI_SEC_PIN_APPL_8:
      return UIM_PIN2_APP8;
    case MMGSDI_UNIVERSAL_PIN_KEY:
      return UIM_UNIVERSAL_PIN;
    case MMGSDI_CHV1_KEY:
      return UIM_CHV1;
    case MMGSDI_CHV2_KEY:
      return UIM_CHV2;
    default:
      return UIM_PIN1_APP1;
  }
}/* mmgsdi_util_convert_to_uim_chv */


/*===========================================================================
FUNCTION MMGSDI_UTIL_GET_KEY_REF

DESCRIPTION
  This function gets the key reference based on the PIN ID and protocol
DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_retrun_enum_type
SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_util_get_key_ref(
  mmgsdi_session_id_type    session_id,
  mmgsdi_pin_enum_type      pin,
  uim_chv_type             *chv)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type   *app_info_ptr      = NULL;
  mmgsdi_channel_info_type   *channel_info_ptr  = NULL;

  /* Base on the session ID, retrieve the prov index and slot */
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                     session_id,
                     NULL,
                     &channel_info_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS)
        ||(channel_info_ptr == NULL)
        ||(channel_info_ptr->app_info_ptr == NULL))
  {
    return mmgsdi_status;
  }
  app_info_ptr = channel_info_ptr->app_info_ptr;

  switch(pin)
  {
    case MMGSDI_PIN1:
      if(app_info_ptr->pin1_ptr){
        *chv = mmgsdi_util_convert_to_uim_chv(app_info_ptr->pin1_ptr->key_ref);
        break;
      }
      UIM_MSG_HIGH_0("ERROR - PIN1 Not set yet");
      return MMGSDI_ERROR;

    case MMGSDI_PIN2:
      if (app_info_ptr->pin2.status == MMGSDI_PIN_STATUS_NOT_INITIALIZED)
      {
        UIM_MSG_HIGH_0("ERROR - PIN2 Not set yet");
        return MMGSDI_ERROR;
      }
      *chv = mmgsdi_util_convert_to_uim_chv(app_info_ptr->pin2.key_ref);
      break;

    case MMGSDI_UNIVERSAL_PIN:
      if(app_info_ptr->universal_pin_ptr){
        *chv =mmgsdi_util_convert_to_uim_chv(app_info_ptr->universal_pin_ptr->key_ref);
        break;
      }
      UIM_MSG_HIGH_0("ERROR - UNIVERSAL PIN Not set yet");
      return MMGSDI_ERROR;

    default:
      UIM_MSG_HIGH_1("ERROR - Invalid PIN 0x%x", pin);
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_get_key_ref */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_RESET_APP_DATA_INIT

DESCRIPTION
  This function resets all the app related data including PIN
  This util does not have critical section protection.

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
==========================================================================*/
void mmgsdi_util_reset_app_data_init(
  boolean                  reselect_app,
  mmgsdi_slot_id_enum_type slot_id
)
{
  int                                    i             = 0;
  mmgsdi_slot_data_type                 *slot_data_ptr = NULL;
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_SUCCESS;
  uint8                                  slot_index    = MMGSDI_SLOT_1_INDEX;
  mmgsdi_aid_select_failed_node_type    *temp_ptr      = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr != NULL)
  {
    /* Reset Data Slot Info*/
    slot_data_ptr->card_apps_available           = 0;
    slot_data_ptr->sim_phase                     = MMGSDI_SIM_PHASE_NOT_INIT;
    slot_data_ptr->esn_me_change_flag            = MMGSDI_ESN_ME_UNKNOWN;
    slot_data_ptr->mmgsdi_tp_state               = MMGSDI_TP_STATE_NOT_DONE;
    slot_data_ptr->onchip_sim_data.state         = MMGSDI_ONCHIP_SIM_NOT_VALID;
    slot_data_ptr->nv_refresh_in_progress        = FALSE;
    slot_data_ptr->is_euicc                      = FALSE;

    /* Clear File rehabilitation info */
    slot_data_ptr->rehab_info.imsi_rehabilitated = FALSE;
    slot_data_ptr->rehab_info.loci_rehabilitated = FALSE;
#ifdef FEATURE_MMGSDI_3GPP2
    memset(slot_data_ptr->rehab_info.cdma_fdn_files_rehab,
           0x00,
           sizeof(slot_data_ptr->rehab_info.cdma_fdn_files_rehab));
#endif /* FEATURE_MMGSDI_3GPP2 */

    slot_data_ptr->available_logical_channels        = MMGSDI_LEGACY_LOGICAL_CHANNELS;
    slot_data_ptr->parsed_atr.total_logical_channels = MMGSDI_LEGACY_LOGICAL_CHANNELS;
    slot_data_ptr->sfi_supported                     = FALSE;
    memset(&slot_data_ptr->card_atr, 0x00, sizeof(slot_data_ptr->card_atr));
    memset(&slot_data_ptr->hist_char, 0x00, sizeof(slot_data_ptr->hist_char));

    /* Clear prov. app indices */
    memset(slot_data_ptr->prov_app_indices,
           0x00,
           sizeof(slot_data_ptr->prov_app_indices));

    /* reset the app info only if it is not for reselecting the app */
    if(!reselect_app)
    {
      if(slot_data_ptr->app_info_pptr != NULL)
      {
        for(i =0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
        {
          if(slot_data_ptr->app_info_pptr[i] != NULL)
          {
            MMGSDIUTIL_TMC_MEM_FREE(slot_data_ptr->app_info_pptr[i]);
          }
        }
      }
    }

    /* Free memory for linked list containing AIDs failing SELECT */
    while (slot_data_ptr->aid_select_failed_ptr)
    {
      temp_ptr = slot_data_ptr->aid_select_failed_ptr->next_ptr;
      MMGSDIUTIL_TMC_MEM_FREE(slot_data_ptr->aid_select_failed_ptr)
      slot_data_ptr->aid_select_failed_ptr = temp_ptr;
    }
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  /* Free Pin Info */
  for(i =0; i < MMGSDI_MAX_PIN1_INFO; i++)
  {
    if(mmgsdi_app_pin1_table_ptr[slot_index][i] != NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_app_pin1_table_ptr[slot_index][i]);
    }
  }

  /* Free UPIN Info*/
  if(mmgsdi_app_univ_pin_table_ptr[slot_index]!= NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_app_univ_pin_table_ptr[slot_index]);
  }
 } /* mmgsdi_util_reset_app_data_init */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_RESET_APP_DATA

DESCRIPTION
  This function resets all the app related data including PIN
  along with critical section protection

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
void mmgsdi_util_reset_app_data(
  boolean                  reselect_app,
  mmgsdi_slot_id_enum_type slot_id
)
{
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
  mmgsdi_util_reset_app_data_init(reselect_app,slot_id);
  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
} /* mmgsdi_util_reset_app_data */


/* ==========================================================================
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
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_find_app_pin_index(
  const mmgsdi_app_pin_info_type  **app_pin_table_pptr,
  uint8                             app_pin_table_size,
  mmgsdi_key_ref_values_enum_type   key_ref,
  uint32                           *index)
{
  uint32         i                = 0;

  if(app_pin_table_pptr != NULL)
  {
    for(i = 0; i < app_pin_table_size; i++)
    {
      if(app_pin_table_pptr[i] == NULL)
      {
        continue;
      }
      if(app_pin_table_pptr[i]->key_ref  == key_ref)
      {
        *index = i;
        return MMGSDI_SUCCESS;
      }
    }
  }

  UIM_MSG_ERR_0("No Slot is empty and key ref does not already exist");
  return MMGSDI_ERROR;

}/* mmgsdi_util_find_app_pin_index */


/* ==========================================================================
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
==========================================================================*/
void mmgsdi_util_set_pin_info(
  mmgsdi_pin_info_type           *dest,
  const mmgsdi_app_pin_info_type *src)
{
  if(dest == NULL || src == NULL)
    return;

  dest->num_retries = src->num_retries;
  dest->num_unblock_retries = src->num_unblock_retries;
  dest->pin_replacement = src->pin_replacement;
  dest->pin_id = src->pin_id;
  dest->status = src->status;
  dest->valid_num_retries = src->valid_num_retries;
  dest->valid_num_unblock_retries = src->valid_num_unblock_retries;
}/* mmgsdi_util_set_pin_info */


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
----------------------------------------------------------------------------*/
void mmgsdi_util_store_pin(
  mmgsdi_int_app_info_type      *sel_app_ptr,
  mmgsdi_pin_enum_type           pin_id,
  mmgsdi_static_data_type        pin_data)
{
  mmgsdi_app_pin_info_type    *pin_info_ptr         = NULL;

  if(sel_app_ptr == NULL )
  {
    UIM_MSG_ERR_0("Cannot Store PIN");
    return;
  }

  if (pin_id == MMGSDI_PIN1)
  {
    pin_info_ptr = sel_app_ptr->pin1_ptr;
  }
  else if (pin_id == MMGSDI_PIN2)
  {
    pin_info_ptr = &sel_app_ptr->pin2;
  }
  else if (pin_id == MMGSDI_UNIVERSAL_PIN)
  {
    pin_info_ptr = sel_app_ptr->universal_pin_ptr;
  }
  else
  {
    UIM_MSG_ERR_1("Incorrect PIN ID = 0x%x", pin_id);
    return;
  }

  if(pin_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cannot Store PIN");
    return;
  }

 /* Store PIN after validity check */
  if ((pin_data.data_len >= MMGSDI_PIN_MIN_LEN) &&
      (pin_data.data_len <= MMGSDI_PIN_MAX_LEN))
  {
    mmgsdi_memscpy(pin_info_ptr->pin_data.data_ptr,
                   sizeof(pin_info_ptr->pin_data.data_ptr),
                   pin_data.data_ptr,
                   int32touint32(pin_data.data_len));


    pin_info_ptr->pin_data.data_len = pin_data.data_len;

    pin_info_ptr->valid_pin = FALSE;
  }
} /* mmgsdi_util_store_pin */


/* ============================================================================
  FUNCTION:      MMGSDI_UTIL_FIND_AID_INDEX

  DESCRIPTION:
    This function returns an index location to the mmgsdi_slot_data_type
    if a matching AID is found.

    Note:
    - If the length of the AIDs doesn't match, it is not considered
      a match (i.e. partial AID matching is not done).
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
)
{
  uint32                 index         = 0;
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_MAX_EXT_APP_INFO;
  }

  if(slot_data_ptr->app_info_pptr == NULL)
  {
    UIM_MSG_ERR_0("App Info Ptr Null");
    return MMGSDI_MAX_EXT_APP_INFO;
  }

  for (index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index++)
  {
    if(slot_data_ptr->app_info_pptr[index] == NULL)
    {
      continue;
    }
    /* Move to the next index if AID length is zero and app type is
       SIM/RUIM/NONE since AID is not applicable to these app types. */
    if ((slot_data_ptr->app_info_pptr[index]->app_data.aid.data_len == 0) &&
        ((slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_NONE) ||
         (slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_SIM) ||
         (slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_RUIM)))
    {
      continue;
    }

    /* If AID lengths do not match, skip to next index */
    if(aid.aid.data_len !=
       slot_data_ptr->app_info_pptr[index]->app_data.aid.data_len)
    {
      continue;
    }

    if(!memcmp(slot_data_ptr->app_info_pptr[index]->app_data.aid.data_ptr,
               aid.aid.data_ptr, int32touint32(aid.aid.data_len)))
    {
      /* AID equal */
      return index;
    }
  }
  UIM_MSG_ERR_0("Unable to find AID from the table");
  return index;
}/* mmgsdi_util_find_aid_index */


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
)
{
  if (aid_index >= MMGSDI_MAX_EXT_APP_INFO)
  {
    UIM_MSG_ERR_1("Invalid AID Index location 0x%x", aid_index);
    return FALSE;
  }
  return TRUE;
} /* mmgsdi_util_is_valid_aid_index */


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_FIND_FIRST_RUIM_CSIM_APP

   DESCRIPTION:
     This function finds the first RUIM/CSIM app from the list of apps
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
static void mmgsdi_util_find_first_ruim_csim_app(
  mmgsdi_slot_id_enum_type  mmgsdi_slot,
  boolean                  *app_1x_found_ptr,
  mmgsdi_data_type         *app_1x_data_ptr)
{
#ifdef FEATURE_MMGSDI_3GPP2
  byte                     index          = 0;
  mmgsdi_slot_data_type   *slot_data_ptr  = NULL;

  UIM_MSG_HIGH_0("mmgsdi_util_find_first_ruim_csim_app");

  if(app_1x_found_ptr == NULL || app_1x_data_ptr == NULL)
  {
    UIM_MSG_ERR_2("NULL pointer passed: app_1x_found_ptr: 0x%x , app_1x_data_ptr: 0x%x ",
                  app_1x_found_ptr, app_1x_data_ptr);
    return;
  }

  memset(app_1x_data_ptr, 0x00, sizeof(mmgsdi_data_type));
  *app_1x_found_ptr = FALSE;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(mmgsdi_slot);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  if(slot_data_ptr->app_info_pptr == NULL)
  {
    UIM_MSG_ERR_0("NULL pointer passed: slot_data_ptr->app_info_pptr");
    return;
  }

  /* Select the first RUIM/CSIM app */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index++)
  {
    if((slot_data_ptr->app_info_pptr[index] == NULL) ||
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_RUIM &&
        slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_CSIM))
    {
      continue;
    }

   if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_RUIM)
   {
      *app_1x_found_ptr = TRUE;
      return;
   }

   if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_CSIM)
    {
      *app_1x_found_ptr = TRUE;
      app_1x_data_ptr->data_len =
         slot_data_ptr->app_info_pptr[index]->app_data.aid.data_len;
      app_1x_data_ptr->data_ptr =
         slot_data_ptr->app_info_pptr[index]->app_data.aid.data_ptr;
      return;
    }
  }
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_util_find_first_ruim_csim_app */


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_FIND_FIRST_SIM_USIM_APP

   DESCRIPTION:
     This function finds the first SIM/USIM app from the list of apps
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
static void mmgsdi_util_find_first_sim_usim_app(
  mmgsdi_slot_id_enum_type  mmgsdi_slot,
  boolean                  *app_gw_found_ptr,
  mmgsdi_data_type         *app_gw_data_ptr)
{
#ifdef FEATURE_MMGSDI_3GPP
  byte                     index          = 0;
  mmgsdi_slot_data_type   *slot_data_ptr  = NULL;

  UIM_MSG_HIGH_0("mmgsdi_util_find_first_sim_usim_app");

  if(app_gw_found_ptr == NULL || app_gw_data_ptr == NULL)
  {
    UIM_MSG_ERR_2("NULL pointer passed: app_gw_found_ptr: 0x%x , app_gw_data_ptr: 0x%x ",
                  app_gw_found_ptr, app_gw_data_ptr);
    return;
  }

  memset(app_gw_data_ptr, 0x00, sizeof(mmgsdi_data_type));
  *app_gw_found_ptr = FALSE;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(mmgsdi_slot);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  if(slot_data_ptr->app_info_pptr == NULL)
  {
    UIM_MSG_ERR_0("NULL pointer passed: slot_data_ptr->app_info_pptr");
    return;
  }

  /* Select the first USIM app */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index++)
  {
    if((slot_data_ptr->app_info_pptr[index] == NULL) ||
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_USIM &&
        slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_SIM))
    {
      continue;
    }

    if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_SIM)
    {
      *app_gw_found_ptr = TRUE;
      return;
    }

    if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_USIM)
    {
      *app_gw_found_ptr = TRUE;
      app_gw_data_ptr->data_len =
         slot_data_ptr->app_info_pptr[index]->app_data.aid.data_len;
      app_gw_data_ptr->data_ptr =
         slot_data_ptr->app_info_pptr[index]->app_data.aid.data_ptr;
      return;
    }
  }
#else
  (void)mmgsdi_slot;
  (void)app_gw_found_ptr;
  (void)app_gw_data_ptr;
#endif /* FEATURE_MMGSDI_3GPP */
} /* mmgsdi_util_find_first_sim_usim_app */


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_ACTIVATE_FIRST_APP

   DESCRIPTION:
     This function Selects the first SIM/USIM or/and RUIM/CSIM app from the list of apps
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
  mmgsdi_slot_id_enum_type slot)
{
  mmgsdi_data_type                   activation_app_data;
  mmgsdi_data_type                   app_1x_data;
  mmgsdi_data_type                   app_gw_data;
  boolean                            app_1x_found       = FALSE;
  boolean                            app_gw_found       = FALSE;
  mmgsdi_session_id_type             session_id         = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_id_type             session_id_1x      = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_id_status_enum_type session_status     = MMGSDI_SESSION_ID_UNASSIGNED;
  nv_stat_enum_type                  result             = NV_STAT_ENUM_MAX;
  mmgsdi_nv_item_cache_type          mmgsdi_nv_data     = {0};
  mmgsdi_nv_context_type             sub_nv_context     = MMGSDI_NV_CONTEXT_INVALID;

  UIM_MSG_LOW_0("mmgsdi_util_select_first_app");

  memset(&activation_app_data, 0x00, sizeof(mmgsdi_data_type));
  memset(&app_1x_data, 0x00, sizeof(mmgsdi_data_type));
  memset(&app_gw_data, 0x00, sizeof(mmgsdi_data_type));

  mmgsdi_util_find_first_sim_usim_app(slot, &app_gw_found, &app_gw_data);

  if (app_gw_found)
  {
    switch (slot)
    {
      case MMGSDI_SLOT_1:
        session_id = mmgsdi_generic_data_ptr->pri_gw_session_id;
        break;
      case MMGSDI_SLOT_2:
        session_id = mmgsdi_generic_data_ptr->sec_gw_session_id;
        break;
      case MMGSDI_SLOT_3:
        session_id = mmgsdi_generic_data_ptr->ter_gw_session_id;
        break;
      default:
        return;
    }
    activation_app_data.data_len = app_gw_data.data_len;
    activation_app_data.data_ptr = app_gw_data.data_ptr;

    /* If the provisioning session is already active, do not attempt
       an activation */
    session_status = mmgsdi_util_is_session_id_valid(session_id);

    if(session_status == MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
    {
      UIM_MSG_HIGH_1("mmgsdi_util_select_first_usim_csim_app: Input session ID in use 0x%x",
                     session_id);
    }
  }

  /* Get RTRE configuration and if RTRE is NV_ONLY then don't auto-activate
     CSIM/RUIM apps as it is NV based not Card based */
  result = mmgsdi_nv_get_item(NV_RTRE_CONFIG_I, &mmgsdi_nv_data, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  if((result == NV_DONE_S) &&
     (mmgsdi_nv_data.item_value.rtre_config != NV_RTRE_CONFIG_NV_ONLY))
  {
    switch (slot)
    {
      case MMGSDI_SLOT_1:
        session_id_1x = mmgsdi_generic_data_ptr->pri_1x_session_id;
        break;
      case MMGSDI_SLOT_2:
        session_id_1x = mmgsdi_generic_data_ptr->sec_1x_session_id;
        break;
      case MMGSDI_SLOT_3:
        session_id_1x = mmgsdi_generic_data_ptr->ter_1x_session_id;
        break;
      default:
        return;
    }
    
    /*Activate 1x session only when CDMALess NV is disbaled */
    if((mmgsdi_util_get_efs_item_index(session_id_1x, &sub_nv_context) == MMGSDI_SUCCESS) &&
       (mmgsdi_nv_get_external_feature_status(
          MMGSDI_EXT_FEAT_CDMA_LESS_DEVICE, sub_nv_context) == MMGSDI_FEATURE_DISABLED))
    {
      mmgsdi_util_find_first_ruim_csim_app(slot, &app_1x_found, &app_1x_data);
    }
  }

  if ((!app_gw_found) || (session_status == MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE))
  {
    app_gw_found = FALSE;

    if (app_1x_found)
    {
      session_id = session_id_1x;
      activation_app_data.data_len = app_1x_data.data_len;
      activation_app_data.data_ptr = app_1x_data.data_ptr;

      /* If the provisioning session is already active, do not attempt
         an activation */
      session_status = mmgsdi_util_is_session_id_valid(session_id);

      if (session_status == MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
      {
        UIM_MSG_HIGH_1("mmgsdi_util_select_first_usim_csim_app: Input session ID in use 0x%x",
                       session_id);
        return;
      }
    }
  }

  if (app_1x_found || app_gw_found)
  {
    /* For a GW only target or on a multimode target if only a USIM app is
       found then only the USIM app will be activated as:
        * csim_app_found is FALSE and usim_app_found is TRUE
        * activation_app_data contains USIM app info
        * session id contains primary GW session id

       For a 1x only target or on a multimode target if only a CSIM app is
       found then only the CSIM app will be activated as:
        * usim_app_found is FALSE and csim_app_found is TRUE
        * activation_app_data contains CSIM app info
        * session id contains primary 1x session id

       For a multimode target if both a CSIM app and USIM app are
       found then:
        * usim_app_found and csim_app_found are TRUE
        * activation_app_data contains USIM app info
        * session id contains primary GW session id
       Hence the USIM app will be activated first and in the internal callback
       when the activation is complete the CSIM app will be activated next */
    (void)mmgsdi_session_activate_or_switch_provisioning(session_id,slot,
            activation_app_data,mmgsdi_util_internal_cb,
            (mmgsdi_client_data_type)(app_1x_found && app_gw_found));
  }
  else
  {
    UIM_MSG_ERR_0("Improper call to app selection logic. No app found");
  }
} /* mmgsdi_util_activate_first_app */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_RESET_PIN2_HIDDENKEY_STATUS

   DESCRIPTION:
     This function resets the PIN2 and Hiddenkey status

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
static void mmgsdi_util_reset_pin2_hiddenkey_status(
  mmgsdi_int_app_info_type *sel_app_ptr)
{
  if(sel_app_ptr == NULL)
  {
    return;
  }

  sel_app_ptr->pin2.num_retries = 0;
  sel_app_ptr->pin2.num_unblock_retries = 0;
  sel_app_ptr->pin2.status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
  sel_app_ptr->pin2.cached = FALSE;
  sel_app_ptr->pin2.valid_pin = FALSE;
  sel_app_ptr->pin2.pin_data.data_len = 0;

  sel_app_ptr->hiddenkey.status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
}/*mmgsdi_util_reset_pin2_hiddenkey_status*/


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_RESET_PIN_STATUS

   DESCRIPTION:
     This function resets the pin status

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_reset_pin_status(
  mmgsdi_int_app_info_type *sel_app_ptr)
{
  if(sel_app_ptr == NULL)
  {
    UIM_MSG_ERR_0("mmgsdi_util_reset_pin_status, sel_app_ptr is NULL");
    return;
  }

  if(sel_app_ptr->pin1_ptr)
  {
    sel_app_ptr->pin1_ptr->num_retries = 0;
    sel_app_ptr->pin1_ptr->num_unblock_retries = 0;
    sel_app_ptr->pin1_ptr->status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
    sel_app_ptr->pin1_ptr->cached = FALSE;
    sel_app_ptr->pin1_ptr->valid_pin = FALSE;
    sel_app_ptr->pin1_ptr->pin_data.data_len = 0;
  }

  mmgsdi_util_reset_pin2_hiddenkey_status(sel_app_ptr);
}/*mmgsdi_util_reset_pin_status*/


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_APP_PIN_CLEANUP

   DESCRIPTION:
     This function verifies if no other App maps to the same PIN1, and if not cleans-up the PIN1 status.
     Also cleans-up the PIN2 status.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None

CRITICAL SECTIONS:
     The calling function must enter mmgsdi_client_app_data_crit_sect_ptr crit
     sect before invoking this util function. Protection for app info table.
==========================================================================*/
void mmgsdi_util_app_pin_cleanup(
  mmgsdi_int_app_info_type *sel_app_ptr,
  mmgsdi_slot_id_enum_type  slot_id)
{
  mmgsdi_int_app_info_type **app_info_table_pptr = NULL;
  int                        app_index           = 0;
  int                        count               = 0;

  if(sel_app_ptr == NULL)
  {
    UIM_MSG_ERR_0("mmgsdi_util_app_pin_cleanup, sel_app_ptr is NULL");
    return;
  }

  if((sel_app_ptr->app_data.app_type != MMGSDI_APP_SIM) &&
     (sel_app_ptr->app_data.app_type != MMGSDI_APP_RUIM))
  {
    if(sel_app_ptr->pin1_ptr != NULL)
    {
      app_info_table_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(
                             slot_id);

      if(app_info_table_pptr == NULL)
      {
        UIM_MSG_ERR_0("mmgsdi_util_app_pin_cleanup, app_info_table_pptr is NULL");
        return;
      }

      for(app_index = 0; app_index < MMGSDI_MAX_EXT_APP_INFO; app_index++)
      {
        if(app_info_table_pptr[app_index] == NULL)
        {
          continue;
        }
        /* Match key_ref of given app with all valid apps on the slot */
        if((app_info_table_pptr[app_index]->pin1_ptr != NULL) &&
           (app_info_table_pptr[app_index]->pin1_ptr->key_ref ==
            sel_app_ptr->pin1_ptr->key_ref))
        {
          count++;

          if(count > 1)
          {
            break;
          }
        }
      }
    }
    /* If there is more than one app that shares the current PIN1,
       no need to clear PIN1 info */
    if(count > 1)
    {
      mmgsdi_util_reset_pin2_hiddenkey_status(sel_app_ptr);
    }
    else
    {
      mmgsdi_util_reset_pin_status(sel_app_ptr);
    }
  }

  sel_app_ptr->pin1_ptr = NULL;
  sel_app_ptr->universal_pin_ptr = NULL;
}/* mmgsdi_util_app_pin_cleanup */


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
)
{
  if(app_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("No App Selected yet!");
    return FALSE;
  }

  /*PIN1 ,PIN2 are mandatory PINS*/
  if(app_info_ptr->pin1_ptr!= NULL &&
     app_info_ptr->pin1_ptr->cached == TRUE &&
     app_info_ptr->pin2.cached  == TRUE )
  {
    /*Universal PIN may or may not be present, if present,
    then check if it is cached*/
    if( app_info_ptr->universal_pin_ptr!= NULL &&
        app_info_ptr->universal_pin_ptr->cached == FALSE)
    {
      UIM_MSG_HIGH_0("Universal PIN not Cached yet");
      return FALSE;
    }
    return TRUE;
  }

  UIM_MSG_HIGH_0("PINs not cached yet!");
  return FALSE;
}/*mmgsdi_util_is_pin_cached*/


/* ==========================================================================
FUNCTION:      MMGSDI_UTIL_CHECK_ACCESS_SECURITY_CONDITION

DESCRIPTION:
  This function checks the security access condition against the global
  PIN1, PIN2, UPIN etc information

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:       If the file access is allowed
    MMGSDI_ACCESS_DENIED: If the file access is restricted because of
                          security check
    MMGSDI_ERROR:         If invalid protection method is passed in as
                          argument or if any other generic error

SIDE EFFECTS:

==========================================================================*/
static mmgsdi_return_enum_type mmgsdi_util_check_access_security_condition(
  const mmgsdi_int_app_info_type  * prov_app_info_ptr,
  const mmgsdi_file_security_type * security_status_ptr)
{
  boolean                     can_proceed      = TRUE;
  boolean                     prev_can_proceed = TRUE;
  int32                       i                = 0;

  if ((security_status_ptr == NULL) || (prov_app_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("Null security_status_ptr 0x%x or prov_app_info_ptr 0x%x",
                  security_status_ptr, prov_app_info_ptr);
    return MMGSDI_ERROR;
  }

  if (security_status_ptr->protection_method != MMGSDI_ALWAYS_ALLOWED)
  {
    UIM_MSG_HIGH_1("Security Access Method 0x%x",
                   security_status_ptr->protection_method);
  }

  switch(security_status_ptr->protection_method)
  {
    case MMGSDI_ALWAYS_ALLOWED:
      return MMGSDI_SUCCESS;

    case MMGSDI_NEVER_ALLOWED:
      return MMGSDI_ACCESS_DENIED;

    case MMGSDI_AND_ALLOWED:
    case MMGSDI_OR_ALLOWED:
    case MMGSDI_SINGLE_ALLOWED:
      for(i=0; i < security_status_ptr->num_protection_pin; i++)
      {
        if(security_status_ptr->protection_pin_ptr[i] == MMGSDI_PIN1)
        {
          if (prov_app_info_ptr->pin1_ptr)
          {
            if (prov_app_info_ptr->pin1_ptr->status == MMGSDI_PIN_DISABLED ||
                prov_app_info_ptr->pin1_ptr->status == MMGSDI_PIN_ENABLED_VERIFIED)
            {
              can_proceed = TRUE;
            }
            else
            {
              UIM_MSG_HIGH_1("PIN1 (status: 0x%x) needed",
                             prov_app_info_ptr->pin1_ptr->status);
              can_proceed = FALSE;
            }
          }
          else
          {
            UIM_MSG_HIGH_0("PIN1 Null");
            can_proceed = FALSE;
          }
        }
        else if(security_status_ptr->protection_pin_ptr[i] == MMGSDI_PIN2)
        {
          if (prov_app_info_ptr->pin2.status == MMGSDI_PIN_DISABLED ||
              prov_app_info_ptr->pin2.status == MMGSDI_PIN_ENABLED_VERIFIED)
          {
            can_proceed = TRUE;
          }
          else
          {
            UIM_MSG_HIGH_1("APP1 PIN2 (status: 0x%x) needed",
                           prov_app_info_ptr->pin2.status);
            can_proceed = FALSE;
          }
        }
        else if(security_status_ptr->protection_pin_ptr[i] == MMGSDI_UNIVERSAL_PIN)
        {
          if (prov_app_info_ptr->universal_pin_ptr)
          {
            if(prov_app_info_ptr->universal_pin_ptr->status == MMGSDI_PIN_DISABLED ||
               prov_app_info_ptr->universal_pin_ptr->status == MMGSDI_PIN_ENABLED_VERIFIED)
            {
              can_proceed = TRUE;
            }
            else
            {
              UIM_MSG_HIGH_1("UPIN (status: 0x%x) needed",
                             prov_app_info_ptr->universal_pin_ptr->status);
              can_proceed = FALSE;
            }
          }
          else
          {
            /* Typically command is not allowed further, but there are cases
               where ADF does not have UPIN status and EFs under this ADF
               point to ARR record which uses UPIN status, to satisfy such
               cards, allow commands down to card to continue. */
            UIM_MSG_HIGH_0("UPIN Null: But still allow the command down to card");
            can_proceed = TRUE;
          }
        }
        else
        {
          UIM_MSG_HIGH_1("Non PIN1/Pin2/UPIN security condition 0x%x",
                         security_status_ptr->protection_pin_ptr[i]);
          can_proceed = FALSE;
        }

        if((security_status_ptr->protection_method == MMGSDI_AND_ALLOWED) ||
           (security_status_ptr->protection_method == MMGSDI_SINGLE_ALLOWED) )
        {
          if(!can_proceed)
          {
            return MMGSDI_ACCESS_DENIED;
          }
        }
        else if(security_status_ptr->protection_method == MMGSDI_OR_ALLOWED)
        {
          if(i == 0)
          {
            prev_can_proceed = can_proceed;
          }
          else
          {
            prev_can_proceed =  prev_can_proceed || can_proceed;
          }
        }
      }
      break;

    default:
      return MMGSDI_ERROR;
  }

  if(prev_can_proceed)
  {
    return MMGSDI_SUCCESS;
  }

  UIM_MSG_ERR_0("OR_OP failed");
  return MMGSDI_ACCESS_DENIED;
} /* mmgsdi_util_check_access_security_condition */


/* ==========================================================================
FUNCTION:      MMGSDI_UTIL_EF_ACCESS_CONDITIONS_CHECK

DESCRIPTION:
  This function checks if the EF can be accessed based on the security
  and the current pin condition

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
static mmgsdi_return_enum_type mmgsdi_util_ef_access_conditions_check(
  const mmgsdi_int_app_info_type        *prov_app_info_ptr,
  const mmgsdi_attr_cache_element_type  *attr_cache_ptr,
  mmgsdi_cmd_enum_type                   cmd_type
)
{
  mmgsdi_file_security_type  *security_status_ptr  = NULL;
  mmgsdi_return_enum_type     mmgsdi_status        = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(prov_app_info_ptr, attr_cache_ptr);

  /* -------------------------------------------------------------------------
     Switch on the msg_id and check the respective access PIN Status
     indication.
     -----------------------------------------------------------------------*/
  switch(cmd_type)
  {
  case MMGSDI_READ_REQ:
  case MMGSDI_SEARCH_REQ:
    security_status_ptr = (mmgsdi_file_security_type*)&(attr_cache_ptr->read);
    break;

  case MMGSDI_WRITE_REQ:
    security_status_ptr = (mmgsdi_file_security_type*)&(attr_cache_ptr->write);
    break;

  case MMGSDI_INCREASE_REQ:
    security_status_ptr = (mmgsdi_file_security_type*)&(attr_cache_ptr->increase);
    if (attr_cache_ptr->cyclic_increase_allowed != TRUE)
    {
      UIM_MSG_ERR_0("NOT ALLOWED INCREASE");
      return MMGSDI_ERROR;
    }
    break;

  default:
    UIM_MSG_ERR_1("UNKNOWN REQUEST 0x%x", cmd_type);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_check_access_security_condition(prov_app_info_ptr,
                                                     security_status_ptr);

  /* override access for EF with configuration issue in commercial card
     with special permissions to update Kc and KcGPRS */
  if (mmgsdi_status != MMGSDI_SUCCESS &&
      (cmd_type == MMGSDI_READ_REQ || cmd_type == MMGSDI_WRITE_REQ) &&
      (attr_cache_ptr->access.file.file_enum == MMGSDI_USIM_KC ||
       attr_cache_ptr->access.file.file_enum == MMGSDI_USIM_KCGPRS ||
       attr_cache_ptr->access.file.file_enum == MMGSDI_GSM_KC ||
       attr_cache_ptr->access.file.file_enum == MMGSDI_GSM_KCGPRS) &&
      mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_SPECIAL_USIM_ARR, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
  {
    UIM_MSG_HIGH_2("override cmd_type - 0x%x access for file: 0x%x",
                   cmd_type, attr_cache_ptr->access.file.file_enum);
    mmgsdi_status = MMGSDI_SUCCESS;
  }

  return mmgsdi_status;
} /* mmgsdi_util_ef_access_conditions_check */


/*===========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_FILE_ATTR

   DESCRIPTION:
     This function checks the file attributes

   DEPENDENCIES:
     None

   LIMITATIONS:
     If the file is not part of cache list, we return SUCCESS.

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     File Read/Write will be blocked if the security conditions have not
     been met.

CRITICAL SECTIONS:
     The calling function must enter mmgsdi_cache_crit_sect_ptr crit
     sect before invoking this util function. Protection for attributes cache
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_check_file_attr(
  mmgsdi_client_id_type           client_id,
  mmgsdi_session_id_type          session_id,
  mmgsdi_slot_id_enum_type        slot,
  const mmgsdi_int_app_info_type *prov_app_info_ptr,
  const mmgsdi_access_type       *access_ptr,
  mmgsdi_cmd_enum_type            cmd_type,
  boolean                         skip_uicc_arr
)
{
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_attr_cache_element_type *attr_cache_ptr  = NULL;

  /* file attr cache is for prov app only, hence we need to have a valid
     prov app pointer */
  MMGSDIUTIL_RETURN_IF_NULL_2(prov_app_info_ptr, access_ptr);

  switch(cmd_type)
  {
    case MMGSDI_READ_REQ:
    case MMGSDI_SEARCH_REQ:
    case MMGSDI_WRITE_REQ:
    case MMGSDI_INCREASE_REQ:
      break;

    default:
      return MMGSDI_SUCCESS;
  }

  if(access_ptr->access_method != MMGSDI_EF_ENUM_ACCESS)
  {
    return MMGSDI_SUCCESS;
  }
  mmgsdi_status = mmgsdi_cache_populate_attr_cache_if_needed(
    client_id,
    session_id,
    slot,
    skip_uicc_arr,
    FALSE,
    access_ptr,
    &attr_cache_ptr,
    NULL);
  if (mmgsdi_status == MMGSDI_NOT_INIT)
  {
    /* If the status is still MMGSDI_NOT_INIT it means this file is not present
       in the cachable files list. Hence the status is reset to success and
       caller is can proceed with sending the access command to the card */
    return MMGSDI_SUCCESS;
  }

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_ef_access_conditions_check(prov_app_info_ptr,
                                                         attr_cache_ptr,
                                                         cmd_type);

  mmgsdi_cache_delete_file_attr_cache_item(&attr_cache_ptr);
  return mmgsdi_status;
}/*mmgsdi_util_check_file_attr*/


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
)
{
  int32 pin_index = 0;

  if (NULL == pin_ptr)
  {
     UIM_MSG_ERR_0("PIN pointer is NULL");
     return FALSE;
  }

  if(pin_len < MMGSDI_PIN_MIN_LEN  || pin_len > MMGSDI_PIN_MAX_LEN)
  {
    UIM_MSG_ERR_0("PIN length is invalid");
    return FALSE;
  }

  /* check that the pin is populated with valid digits */
  for (pin_index = 0; pin_index < pin_len; pin_index++)
  {
    if ((pin_ptr[pin_index] < '0' ||
         pin_ptr[pin_index] > '9'))
    {
       UIM_MSG_ERR_1("PIN is of invalid length: 0x%x bytes", pin_index);
       return FALSE;
    }
  }
  return TRUE;
} /* mmgsdi_util_is_pin_valid */


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
  const mmgsdi_data_type * hkey_data_ptr)
{
  int32   hkey_index        = 0;
  uint8   hkey_val          = 0;
  boolean hkey_unused_digit = FALSE;

  if ((NULL == hkey_data_ptr) || (NULL == hkey_data_ptr->data_ptr))
  {
     UIM_MSG_ERR_0("Hiddenkey pointer is NULL");
     return FALSE;
  }

  /* Verify that the Hidden Key data length is valid */
  if(hkey_data_ptr->data_len != MMGSDI_HIDDENKEY_LEN)
  {
    UIM_MSG_ERR_0("Hiddenkey data length is invalid");
    return FALSE;
  }

  /* Verify that the Hidden Key data is valid */
  for (hkey_index = 0; hkey_index < MMGSDI_HIDDENKEY_LEN; hkey_index++)
  {
    hkey_val = (hkey_data_ptr->data_ptr[hkey_index] & 0xF0) >> 4;

    /* The minimum number of digits for Hiddenkey is 4, and
       these shall all be valid BCD digits */
    if(hkey_index < MMGSDI_HIDDENKEY_MIN_VALID_LEN)
    {
      if(hkey_val > 0x9)
      {
        UIM_MSG_ERR_0("Hiddenkey data from card is invalid");
        return FALSE;
      }
    }
    /* Beyond the minimum number (4 digits), once an 'F' is encountered (which
       indicates that it is an unused digit), the remaining digits
       shall also be 'F'
       Example: Hiddenkey value of 1234F8FF needs to be marked as invalid.*/
    else
    {
      if(hkey_val == 0xF)
      {
        hkey_unused_digit = TRUE;
      }
      else if((hkey_val > 0x9) || (hkey_unused_digit))
      {
        UIM_MSG_ERR_0("Hiddenkey data from card is invalid");
        return FALSE;
      }
    }

    hkey_val = hkey_data_ptr->data_ptr[hkey_index] & 0x0F;

    if(hkey_index < MMGSDI_HIDDENKEY_MIN_VALID_LEN)
    {
      if(hkey_val > 0x9)
      {
        UIM_MSG_ERR_0("Hiddenkey data from card is invalid");
        return FALSE;
      }
    }
    else
    {
      if(hkey_val == 0xF)
      {
        hkey_unused_digit = TRUE;
      }
      else if((hkey_val > 0x9) || (hkey_unused_digit))
      {
        UIM_MSG_ERR_0("Hiddenkey data from card is invalid");
        return FALSE;
      }
    }
  }
  return TRUE;
} /* mmgsdi_util_is_hiddenkey_from_card_valid */


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
  const mmgsdi_data_type * hkey_data_ptr)
{
  int32 hkey_index = 0;

  if ((NULL == hkey_data_ptr) || (NULL == hkey_data_ptr->data_ptr))
  {
     UIM_MSG_ERR_0("Hiddenkey pointer is NULL");
     return FALSE;
  }

  /* Verify that the Hidden Key data length is valid */
  if(hkey_data_ptr->data_len < MMGSDI_PIN_MIN_LEN ||
     hkey_data_ptr->data_len > MMGSDI_PIN_MAX_LEN)
  {
    UIM_MSG_ERR_0("Hiddenkey data length is invalid");
    return FALSE;
  }

  /* Verify that the Hidden Key data is valid */
  for (hkey_index = 0; hkey_index < hkey_data_ptr->data_len; hkey_index++)
  {
    if ((hkey_data_ptr->data_ptr[hkey_index] < '0') ||
        (hkey_data_ptr->data_ptr[hkey_index] > '9'))
    {
       UIM_MSG_ERR_0("Hiddenkey data is invalid");
       return FALSE;
    }
  }
  return TRUE;
} /* mmgsdi_util_is_hiddenkey_valid */


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
  mmgsdi_static_data_type       * hkey_card_ptr)
{
  mmgsdi_len_type         len = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(hkey_ptr, hkey_card_ptr);

  /* Since we have checks in the PIN APIs for valid length
     and data range, we do not have any checks here */

  /* Hiddenkey file size is fixed */
  hkey_card_ptr->data_len = MMGSDI_HIDDENKEY_LEN;

  memset((void*)hkey_card_ptr->data_ptr, 0xFF,
         int32touint32(hkey_card_ptr->data_len));

  /* Convert the Hiddenkey data in ASCII to the card format */
  for(len = 0; len < (hkey_ptr->data_len - 1); (len = len + 2))
  {
    hkey_card_ptr->data_ptr[len/2] = ((hkey_ptr->data_ptr[len] - '0') << 0x04) |
                                      (hkey_ptr->data_ptr[len + 1] - '0');
  }

  if(hkey_ptr->data_len % 2)
  {
    hkey_card_ptr->data_ptr[len/2] = ((hkey_ptr->data_ptr[len] - '0') << 0x04) |
                                      0xF;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_convert_hiddenkey */


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
  const mmgsdi_static_data_type * hkey_ptr)
{
  mmgsdi_static_data_type hkey_card;
  mmgsdi_data_type        hkey_cache;
  mmgsdi_access_type      file_access;
  uint32                  data_len      = 0;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_session_type_enum_type session_type;

  MMGSDIUTIL_RETURN_IF_NULL(hkey_ptr);

  /* Convert input Hiddenkey data in ASCII to card format */
  mmgsdi_status = mmgsdi_util_convert_hiddenkey(hkey_ptr, &hkey_card);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Could not verify current Hidden Key data");
    return MMGSDI_ERROR;
  }

  memset((void*)&hkey_cache, 0x00, sizeof(mmgsdi_data_type));
  memset((void*)&file_access, 0x00, sizeof(mmgsdi_access_type));

  /* Read EFHiddenkey from cache */
  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;

  mmgsdi_status = mmgsdi_util_get_session_type(session_id, &session_type, NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid session id 0x%x", session_id);
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
    UIM_MSG_ERR_1("Invalid Session type 0x%x", session_type);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                             &file_access,
                                             &hkey_cache.data_len,
                                             0,
                                             NULL);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    data_len = int32touint32(hkey_cache.data_len);
    if (data_len == MMGSDI_HIDDENKEY_LEN)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(hkey_cache.data_ptr,
                                         data_len);
      if (hkey_cache.data_ptr != NULL)
      {
        mmgsdi_status = mmgsdi_cache_read(session_id,
                                               &file_access,
                                               &hkey_cache,
                                               0,
                                               NULL);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          /* In case of error, free memory */
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hkey_cache.data_ptr);
          UIM_MSG_ERR_0("Could not read Hidden Key from cache");
          return MMGSDI_ERROR;
        }
      }
      else
      {
        UIM_MSG_ERR_0("Could not allocate memory to read Hiddenkey data from cache");
        return MMGSDI_ERROR;
      }
    }
    else
    {
      UIM_MSG_ERR_0("Incorrect file size of Hiddenkey in cache");
      return MMGSDI_ERROR;
    }
  }
  else
  {
    UIM_MSG_ERR_0("Could not read Hidden key file size from cache");
    return MMGSDI_ERROR;
  }

  /* Compare input Hiddenkey with the card data */
  if(memcmp((const void*)hkey_card.data_ptr, (const void*)hkey_cache.data_ptr,
            MMGSDI_HIDDENKEY_LEN))
  {
    UIM_MSG_ERR_0("The Hiddenkey value provided by client for verification does not match the card data");
    mmgsdi_status = MMGSDI_ERROR;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(hkey_cache.data_ptr);
  return mmgsdi_status;
} /* mmgsdi_util_verify_hiddenkey */


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
)
{
  mmgsdi_return_enum_type    mmgsdi_status;
  mmgsdi_session_info_type  *session_ptr  = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  if (client_index >= MMGSDI_MAX_CLIENT_INFO ||
      mmgsdi_client_id_reg_table[client_index] == NULL ||
      aid_ptr == NULL)
  {
    /*Invalid Client index or Invalid aid_ptr*/
    return NULL;
  }

  session_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if ((session_ptr != NULL) &&
      (session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
      (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index] != NULL))
  {
    if(mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->slot_id == slot_id)
    {
      if (mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr != NULL)
      {
        mmgsdi_status = mmgsdi_util_cmp_aid(
          &mmgsdi_channel_info_ptr_table[session_ptr->channel_info_index]->app_info_ptr->app_data,
          aid_ptr, ignore_icc_aid);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          return session_ptr;
        }
      }
    }
  }
  return NULL;
}/* mmgsdi_util_cmp_aid_get_session_ptr */


/*===========================================================================
   FUNCTION MMGSDI_UTIL_GET_BER_LENGTH

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
uint32 mmgsdi_util_get_ber_length(uint32 orig_ber_len,
                           uint32 orig_data_len,
                           uint8  *ber_data_ptr)
{

  uint32 ber_len      = orig_ber_len;
  uint8  buff_offset  = 0;
  uint32 new_length   = 0;


  if(ber_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("ber_data_ptr is NULL");
    return 0;
  }
  /*---------------------------------------------------------------
    Tag for BER TLV according to 102 331 Section 11.3 -
    First byte      Following bytes            Encoded length value
    1. '00' to '7F'      none                     0     to 127
    2. '81'             '80' to 'FF'              128   to 255
    3. '82'             '01 00' to 'FF FF'        256   to 65 535
    4. '83'             '01 00 00' to 'FF FF FF'  65536 to 16777215
    ---------------------------------------------------------------*/

  /*The length could be > 255,
  Find out how many bytes will the length field occupy*/
  while(ber_len > 0)
  {
    (new_length)++;
    ber_len = ber_len >>8;
  }

  /*Reset the length to original value*/
  ber_len     = orig_ber_len;

  /*If the length is greater than MAX Data, then there is a tag for it,
    as case #2, case #3, case #4 above*/
  if(orig_data_len > MMGSDI_MBMS_MAX_AUTH_DATA)
  {
    ber_data_ptr[0] = uint32touint8(MMGSDI_LENGTH_TAG | new_length);
    buff_offset = uint32touint8(new_length);
    (new_length)++;

    /*It is a uint32, data is placed in Big Endian Format,
      Create the rest of the ber_data*/
    while(buff_offset > 0)
    {
      ber_data_ptr[buff_offset--] = uint32touint8(ber_len);
      ber_len = ber_len >>8;
    }
  }
  else
  {
    /*case #1*/
    ber_data_ptr[0] = uint32touint8(ber_len);
  }
  return new_length;
}/*mmgsdi_util_get_ber_length*/


/*===========================================================================
   FUNCTION MMGSDI_UTIL_PRINT_FILE_INFORMATION

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
)
{
  switch (file_access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      UIM_MSG_HIGH_2("For ef_enum 0x%x, is_error_msg 0x%x",
                    file_access.file.file_enum,
            is_error_msg);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      if(is_error_msg)
      {
        uint32 mf_child_path;
        switch(file_access.file.path_type.path_len)
        {
          case 1:
           /* Printing 2 byte File ID information : MF ID*/
            UIM_MSG_ERR_1("For File path 0x%x",
                          file_access.file.path_type.path_buf[0x00]);
            break;

          case 2:
            /* Printing MF and MF Child (EF/DF) File ID information*/
            UIM_MSG_ERR_2("For File path 0x%x%x",
                          file_access.file.path_type.path_buf[0x00],
                          file_access.file.path_type.path_buf[0x01]);
            break;

          case 3:
            /* Printing MF, MF Child DF and Child DF/EF File ID information*/
            UIM_MSG_ERR_3("For File path 0x%x%x%x",
                          file_access.file.path_type.path_buf[0x00],
                          file_access.file.path_type.path_buf[0x01],
                          file_access.file.path_type.path_buf[0x02]);
            break;

          case 4:
            /* Printing MF, MF Child DF, DF CHILD DF and DF CHILD EF */
            /* Reading First and secound byte of the path buf
               and update the mf_child_path */
            mf_child_path = file_access.file.path_type.path_buf[0x00];
            mf_child_path <<=0x16;
            mf_child_path |= file_access.file.path_type.path_buf[0x01];
            UIM_MSG_ERR_3("For File path 0x%x%x%x",
                        mf_child_path,
                        file_access.file.path_type.path_buf[0x02],
                        file_access.file.path_type.path_buf[0x03]);
            break;

          default:
            UIM_MSG_ERR_1("Invalid Path length 0x%x",
                        file_access.file.path_type.path_len );
            break;
        }/*end of switch switch(file_access.file.path_type.path_len)*/
      }/*end of if is_error_msg */
      else
      {
        uint32 mf_child_path;
        switch(file_access.file.path_type.path_len)
        {
          case 1:
          /* Printing 2 byte File ID information : MF ID*/
          UIM_MSG_HIGH_1("For File path 0x%x",
                         file_access.file.path_type.path_buf[0x00]);
          break;

          case 2:
          /* Printing MF and MF Child (EF/DF) File ID information*/
            UIM_MSG_HIGH_2("For File path 0x%x%x",
                           file_access.file.path_type.path_buf[0x00],
                           file_access.file.path_type.path_buf[0x01]);
            break;

          case 3:
          /* Printing MF, MF Child DF and Child DF/EF File ID information*/
            UIM_MSG_HIGH_3("For File path 0x%x%x%x",
                           file_access.file.path_type.path_buf[0x00],
                           file_access.file.path_type.path_buf[0x01],
                           file_access.file.path_type.path_buf[0x02]);
            break;

          case 4:
          /* Printing MF, MF Child DF, DF CHILD DF and DF CHILD EF */
          /* Reading First and secound byte of the path buf
             and update the mf_child_path */
            mf_child_path = file_access.file.path_type.path_buf[0x00];
            mf_child_path <<=0x16;
              mf_child_path |= file_access.file.path_type.path_buf[0x00];
            UIM_MSG_ERR_3("For File path 0x%x%x%x",
                          mf_child_path,
                          file_access.file.path_type.path_buf[0x02],
                          file_access.file.path_type.path_buf[0x03]);
            break;

          default:
            UIM_MSG_ERR_1("Invalid Path length 0x%x",
                          file_access.file.path_type.path_len );
            break;
        }/*end of switch switch(file_access.file.path_type.path_len)*/
      }
      break;
    default:
      UIM_MSG_ERR_1("Invalid File Access Method 0x%x ",
                    file_access.access_method);
      break;
  }/*end of switch (file_access.access_method)*/
  /* Do Nothing and return*/
}/*end of mmgsdi_util_print_file_information*/


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
 ----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_util_malloc_and_copy_file_list_ptr(
  mmgsdi_file_enum_type    **dst_file_list_pptr,
  mmgsdi_file_enum_type    *src_file_list_ptr,
  uint32                   num_files
)
{
  if((src_file_list_ptr == NULL) || (num_files == 0) ||
      (dst_file_list_pptr == NULL))
  {
    UIM_MSG_ERR_3("Bad Param copy file list - src_file_list_ptr=0x%x num_files=0x%x dst_file_list_pptr=0x%x",
                  src_file_list_ptr, num_files, dst_file_list_pptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Allocate memory for file list data */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*dst_file_list_pptr,
                                     (sizeof(mmgsdi_file_enum_type) * num_files));
  if ((*dst_file_list_pptr) == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Copy the files over */
  mmgsdi_memscpy(*dst_file_list_pptr,
                 num_files * sizeof(mmgsdi_file_enum_type),
                 src_file_list_ptr,
                 num_files * sizeof(mmgsdi_file_enum_type));

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_malloc_and_copy_file_list_ptr */


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
    MMGSDI_SUCCESS                     : Successful file copy operation
    MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED : Malloc Error
    MMGSDI_INCORRECT_PARAMS            : If input parameters are out of range
    MMGSDI_ERROR                       : For all other failure scenarios


  SIDE EFFECTS:
    The calling function should free the allocated memory
 ----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_util_malloc_and_copy_file_path_array(
  mmgsdi_path_type           **dst_file_path_pptr,
  mmgsdi_path_type           *src_file_path_ptr,
  uint32                     num_files
)
{
  uint32                   i                   = 0;
  mmgsdi_return_enum_type  mmgsdi_status       = MMGSDI_SUCCESS;
  mmgsdi_path_type         *dst_file_path_ptr  = NULL;
  uint32                   path_len            = 0;

  if((src_file_path_ptr == NULL) || (num_files == 0) ||
      (dst_file_path_pptr == NULL))
  {
    UIM_MSG_ERR_3("Bad Param copy file path - src_file_p=0x%x num_files=0x%x dst_file_path_pptr=0x%x",
                  src_file_path_ptr, num_files, dst_file_path_pptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Allocate memory for file path data */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(dst_file_path_ptr,
    (num_files * sizeof(mmgsdi_path_type)));
  if (dst_file_path_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* Perform a deep copy for the file path array */
  for(i = 0; i < num_files; i++)
  {
    if(src_file_path_ptr[i].path_len == 0)
    {
      UIM_MSG_ERR_1("Refresh:Zero path_len at src_file_path_ptr[0x%x].path_len", i);
      /* Free previous memory allocation */
      MMGSDIUTIL_TMC_MEM_FREE(dst_file_path_ptr);
      return MMGSDI_ERROR;
    }
    path_len = int32touint32(src_file_path_ptr[i].path_len);
    mmgsdi_memscpy(dst_file_path_ptr[i].path_buf,
      sizeof(dst_file_path_ptr[i].path_buf),
      src_file_path_ptr[i].path_buf,
      path_len * sizeof(uint16));

    dst_file_path_ptr[i].path_len = src_file_path_ptr[i].path_len;
  }

  *dst_file_path_pptr = dst_file_path_ptr;

  return mmgsdi_status;
} /* mmgsdi_util_malloc_and_copy_file_path_array */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_REFRESH_FILE_PATHS

DESCRIPTION
  This function shall accept byte buffer that has list of file paths.
  Each file path begins from MF, hence byte value 0x 3F shall be used
  as a delimiter. The individual file paths shall be stored in the
  array of mmgsdi_path_type. The first byte shall contain the number
  of files
  For example
  Input buffer: 0x02 0x3F 0x00 0x2F 0x02 0x3F 0x00 0x7F 0x10 0x6F 0xD2
                 |
                First byte is number of files
  Should give output -
  *file_path_pptr[0].path_buf={0x3F 0x00 0x2F 0x02 0x00 0x00 0x00 0x00 0x00 0x00}
  *file_path_pptr[0].path_len = 2
  *file_path_pptr[1].path_buf={0x3F 0x00 0x7F 0x10 0x6F 0xD2 0x00 0x00 0x00 0x00}
  *file_path_pptr[1].path_len = 3
  num_files = 2

DEPENDENCIES
  NONE

LIMITATIONS
  The function shall only consider the number of files as mentioned in the
  first byte of the input data_ptr. If the actual buffer has more files than
  the number mentioned in the first byte, the logic in the function will
  ignore the rest of the files.


RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED - Malloc Error
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  Calling function should free memory allocated to file_path_pptr
----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_util_convert_to_refresh_file_paths(
  uint32                  data_len,
  const uint8            *data_ptr,
  mmgsdi_path_type       **file_path_pptr,
  uint8                  *num_files_ptr
)
{
  uint8                   file_num                    = 0;
  uint32                  index                       = 0;
  uint8                   path_index                  = 0;
  mmgsdi_path_type        *file_path_ptr              = NULL;
  uint16                  path[MMGSDI_MAX_PATH_LEN];


  /* Check for input params */
  MMGSDIUTIL_RETURN_IF_NULL_3(num_files_ptr, data_ptr, file_path_pptr);

  if(data_len == 0)
  {
    UIM_MSG_ERR_1("Bad Input Params data_len=0x%x", data_len);
    return MMGSDI_INCORRECT_PARAMS;
  }

  memset(path, 0x00,(sizeof(uint16)*(MMGSDI_MAX_PATH_LEN)));

  /* First byte contains number of files in the file list per spec
     102 221 section 8.18. This information comes from the card */

  *num_files_ptr = data_ptr[index++];
  data_len = data_len - 1;

  /* allocate memory to file_path_array */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(file_path_ptr,
    (*num_files_ptr) * sizeof(mmgsdi_path_type));

  if(file_path_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  while (( index < data_len ) && (file_num < *num_files_ptr))
  {
    path_index = 0;

    /* Parse for each file path */
    do
    {
      /* If input file path for a file exceeds 5 file ID depth, treat it
         as invalid and return */
      if(path_index >= MMGSDI_MAX_PATH_LEN)
      {
        UIM_MSG_ERR_0("mmgsdi_util_convert_to_refresh_file_paths: Invalid input file path");
        MMGSDIUTIL_TMC_MEM_FREE(file_path_ptr);
        return MMGSDI_ERROR;
      }

      path[path_index] = data_ptr[index++] << MMGSDIUTIL_BYTE_SHIFT;
      path[path_index] = path[path_index] | data_ptr[index++];
      path_index++;
    }while ((index < data_len) && (data_ptr[index] != MMGSDIUTIL_MF_HIGH_BYTE));

    /* Path index was incremented by 1 in the last iteration of do-while loop,
       hence we have path_len = path index here */
    file_path_ptr[file_num].path_len = path_index;

    /* Copy path buffer */
    mmgsdi_memscpy(file_path_ptr[file_num].path_buf,
                   sizeof(file_path_ptr[file_num].path_buf),
                   path,
                  (path_index) * sizeof(uint16));
    file_num++;
  } /* while (index < data_len ) */

  /* Compare file_num and num_files. The case where *num_files_ptr > file_num
     is an invalid REFRESH command from the card. This happens on some
     commercial cards, so we need to handle this correctly and adjust the
     value of *num_files_ptr, as if a valid number was present in the
     REFRESH payload.
     The opposite case *num_files_ptr < file_num cannot happen, because
     it's forced in the loop condition to avoid buffer overflow */
  if(*num_files_ptr > file_num)
  {
    UIM_MSG_HIGH_2("Num of refresh files mismatch *num_files_p=0x%x file_num=0x%x",
               *num_files_ptr, file_num);
    *num_files_ptr = file_num;
  }
  *file_path_pptr = file_path_ptr;
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_convert_to_refresh_file_paths */


/*===========================================================================
FUNCTION MMGSDI_UTIL_GET_CSIM_APPS

DESCRIPTION
  This function is used to get the number of CSIM apps.

DEPENDENCIES
  None

RETURN VALUE
  uint32: Number of CSIM Apps

SIDE EFFECTS
  None
===========================================================================*/
uint32 mmgsdi_util_get_csim_apps(mmgsdi_slot_id_enum_type slot)
{
  uint32 num_apps = 0;
#ifdef FEATURE_MMGSDI_3GPP2
  uint32 i        = 0;
  mmgsdi_slot_data_type *data_slot_ptr = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  data_slot_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(data_slot_ptr == NULL)
  {
    return num_apps;
  }

  if(data_slot_ptr->app_info_pptr == NULL)
  {
    UIM_MSG_ERR_0("NULL pointer passed: data_slot_ptr->app_info_pptr");
    return num_apps;
  }

  for(i = 0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
  {
    if(data_slot_ptr->app_info_pptr[i] == NULL)
    {
      continue;
    }
    if(data_slot_ptr->app_info_pptr[i]->app_data.app_type == MMGSDI_APP_CSIM &&
       data_slot_ptr->app_info_pptr[i]->app_data.aid.data_len > 0)
    {
      num_apps++;
    }
  }
#endif /* FEATURE_MMGSDI_3GPP2 */
  return num_apps;
} /* mmgsdi_util_get_csim_apps */


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
mmgsdi_return_enum_type mmgsdi_util_is_card_icc(
  const mmgsdi_slot_data_type *slot_data_ptr,
  boolean                     *is_icc_ptr)
{
  int32   i                = 0;
  boolean unknown_uicc_app = FALSE;

  MMGSDIUTIL_RETURN_IF_NULL_2(slot_data_ptr, is_icc_ptr);

  if(slot_data_ptr->app_info_pptr == NULL)
  {
    UIM_MSG_ERR_0("mmgsdi_util_is_card_icc, app info pointer is NULL");
    return MMGSDI_ERROR;
  }

  for (i = 0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
  {
    if(slot_data_ptr->app_info_pptr[i] == NULL)
    {
      continue;
    }
    if ((slot_data_ptr->app_info_pptr[i]->app_data.app_type == MMGSDI_APP_SIM) ||
        (slot_data_ptr->app_info_pptr[i]->app_data.app_type == MMGSDI_APP_RUIM))
    {
      /* found SIM or RUIM App, since they cannot co-exist with UICC applications,
         this must be a ICC card */
      *is_icc_ptr = TRUE;
      return MMGSDI_SUCCESS;
    }
    else if ((slot_data_ptr->app_info_pptr[i]->app_data.app_type == MMGSDI_APP_USIM) ||
             (slot_data_ptr->app_info_pptr[i]->app_data.app_type == MMGSDI_APP_CSIM))
    {
      /* found USIM or CSIM App, since they cannot co-exist with ICC "applications",
         this must be a UICC card */
      *is_icc_ptr = FALSE;
      return MMGSDI_SUCCESS;
    }
    else if (slot_data_ptr->app_info_pptr[i]->app_data.app_type == MMGSDI_APP_UNKNOWN)
    {
      unknown_uicc_app = TRUE;
    }
  }

  /* No known GW/1X application but there is unknown uicc app, must be UICC card.
     Also, if the card is eUICC there is possibility that EF-DIR is missing */
  if (unknown_uicc_app || slot_data_ptr->is_euicc)
  {
    *is_icc_ptr = FALSE;
    return MMGSDI_SUCCESS;
  }
  /* no known GW or 1X application or unknown app, return error for now */
  UIM_MSG_ERR_0("No known GW/1X application nor unknown UICC app");
  return MMGSDI_ERROR;
} /* mmgsdi_util_is_card_icc */


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
  mmgsdi_int_app_info_type     **int_app_info_pptr)
{
  mmgsdi_slot_data_type        *slot_data_ptr     = NULL;
  uint32                        app_index         = MMGSDI_MAX_EXT_APP_INFO;
  mmgsdi_session_info_type     *session_info_ptr  = NULL;
  mmgsdi_channel_info_type     *channel_info_ptr  = NULL;
  mmgsdi_return_enum_type       mmgsdi_status     = MMGSDI_ERROR;
  uint8                         prov_index        = MMGSDI_MAX_PROV_APPS;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    session_id,
                    &session_info_ptr,
                    &channel_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL) ||
     (channel_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  prov_index = mmgsdi_util_get_prov_index_from_session(session_info_ptr->session_type);
  if(prov_index == MMGSDI_MAX_PROV_APPS)
  {
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(channel_info_ptr->slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  app_index = slot_data_ptr->prov_app_indices[prov_index];
  if ((app_index & MMGSDI_PROV_INDEX_VALIDITY_MASK) == 0)
  {
    UIM_MSG_ERR_1("Provisioning Index 0x%x is not set", prov_index);
    return MMGSDI_ERROR;
  }

  app_index = app_index & MMGSDI_PROV_INDEX_MASK;

  if(slot_data_ptr->app_info_pptr == NULL)
  {
    UIM_MSG_ERR_1("Provisioning app ptr is NULL for mmgsdi slot 0x%x",
                  channel_info_ptr->slot_id);
    return MMGSDI_ERROR;
  }

  if(int_app_info_pptr)
  {
    *int_app_info_pptr = slot_data_ptr->app_info_pptr[app_index];
  }

  if(prov_index_ptr)
  {
    *prov_index_ptr = prov_index;
  }

  if(slot_ptr)
  {
    *slot_ptr = channel_info_ptr->slot_id;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_prov_session_info */


/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_SESSION_APP_INFO

DESCRIPTION
  This function takes the session_id and returns the slot and the app info
  associated with that session_id

DEPENDENCIES
  None

LIMITATIONS
  This function MUST NOT be called by passing in mmgsdi channel, session
  global data

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
  uint32                        *app_index_ptr)
{
  mmgsdi_session_info_type     *session_info_ptr    = NULL;
  mmgsdi_channel_info_type     *channel_info_ptr    = NULL;
  byte                          i                   = 0;
  mmgsdi_return_enum_type       mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_int_app_info_type    **app_info_table_pptr = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  /* initialized to invalid value for passed in parameter */
  if(slot_ptr)
  {
    *slot_ptr = MMGSDI_MAX_SLOT_ID_ENUM;
  }
  if(channel_id_ptr)
  {
    *channel_id_ptr = 0;
  }
  if(int_app_info_pptr)
  {
    *int_app_info_pptr = NULL;
  }
  if(channel_info_pptr)
  {
    *channel_info_pptr = NULL;
  }
  if(session_info_pptr)
  {
    *session_info_pptr = NULL;
  }
  if(app_index_ptr)
  {
    *app_index_ptr = MMGSDI_MAX_EXT_APP_INFO;
  }

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    session_id,
                    &session_info_ptr,
                    &channel_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS) ||  (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_2("Null Ptr, session_info_ptr=0x%x, session_id=0x%x",
                  session_info_ptr, session_id);
    return MMGSDI_ERROR;
  }

  if (channel_info_pptr)
  {
    /* It is possible that channel_info_ptr is still not assigned when
       this function is called and that should not be an error condition
       esp if some clients only care to get the session_info_type.
       Hence, only error out if client explicity asks for
       channel_info_pptr and we have a NULL channel_info_ptr */
    if(channel_info_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }
    *channel_info_pptr = channel_info_ptr;
  }
  if (session_info_pptr)
  {
    *session_info_pptr = session_info_ptr;
  }
  if (channel_id_ptr)
  {
    if(channel_info_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }
    *channel_id_ptr = channel_info_ptr->channel_id;
  }
  if(slot_ptr)
  {
    switch(session_info_ptr->session_type)
    {
      case MMGSDI_GW_PROV_PRI_SESSION:
      case MMGSDI_GW_PROV_SEC_SESSION:
      case MMGSDI_GW_PROV_TER_SESSION:
      case MMGSDI_1X_PROV_PRI_SESSION:
      case MMGSDI_1X_PROV_SEC_SESSION:
      case MMGSDI_1X_PROV_TER_SESSION:
        if (channel_info_ptr == NULL)
        {
          return MMGSDI_ERROR;
        }
        *slot_ptr = channel_info_ptr->slot_id;
        break;

      case MMGSDI_NON_PROV_SESSION_SLOT_1:
      case MMGSDI_CARD_SESSION_SLOT_1:
        if (channel_info_ptr != NULL)
        {
          *slot_ptr = channel_info_ptr->slot_id;
        }
        else
        {
          *slot_ptr = MMGSDI_SLOT_1;
        }
        break;

      case MMGSDI_NON_PROV_SESSION_SLOT_2:
      case MMGSDI_CARD_SESSION_SLOT_2:
        if (channel_info_ptr != NULL)
        {
          *slot_ptr = channel_info_ptr->slot_id;
        }
        else
        {
          *slot_ptr = MMGSDI_SLOT_2;
        }
        break;

      case MMGSDI_NON_PROV_SESSION_SLOT_3:
      case MMGSDI_CARD_SESSION_SLOT_3:
        if (channel_info_ptr != NULL)
        {
          *slot_ptr = channel_info_ptr->slot_id;
        }
        else
        {
          *slot_ptr = MMGSDI_SLOT_3;
        }
        break;

      default:
        UIM_MSG_ERR_1("Invalid session type 0x%x",
                      session_info_ptr->session_type);
        return MMGSDI_ERROR;
    }
  }

  if ((app_index_ptr) || (int_app_info_pptr))
  {
    if (channel_info_ptr == NULL)
    {
      UIM_MSG_ERR_1("App_index or App info ptr is requested but null channel_info_ptr, session_id=0x%x",
                    session_id);
      return MMGSDI_ERROR;
    }
  }
  else
  {
    /* No need to continue further since client is not interested in
       app index or app info ptr */
    return MMGSDI_SUCCESS;
  }

  app_info_table_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(
                          channel_info_ptr->slot_id);

  if(app_info_table_pptr == NULL)
  {
    UIM_MSG_ERR_0("app info pointer is NULL");
    return MMGSDI_ERROR;
  }

  for(i = 0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
  {
    if(app_info_table_pptr[i] == NULL)
    {
      continue;
    }
    /* Match application info in channel data with that contained in global
       mmgsdi_app_pin_table_ptr[][] */
    if(channel_info_ptr->app_info_ptr != NULL &&
       mmgsdi_util_cmp_aid(&channel_info_ptr->app_info_ptr->app_data,
                           &app_info_table_pptr[i]->app_data,
                           FALSE) == MMGSDI_SUCCESS)
    {
      if(app_index_ptr)
      {
        *app_index_ptr = i;
      }
      if (int_app_info_pptr)
      {
        *int_app_info_pptr = app_info_table_pptr[i];
      }
      return MMGSDI_SUCCESS;
    }
  }
  UIM_MSG_ERR_1("No matching app found for session_id=0x%x", session_id);
  return MMGSDI_ERROR;
} /* mmgsdi_util_get_session_app_info */


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
  uint8                    *slot_index)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  switch(mmgsdi_slot)
  {
    case MMGSDI_SLOT_1:
      *slot_index = MMGSDI_SLOT_1_INDEX;
      break;
    case MMGSDI_SLOT_2:
      *slot_index = MMGSDI_SLOT_2_INDEX;
      break;
    case MMGSDI_SLOT_3:
      *slot_index = MMGSDI_SLOT_3_INDEX;
      break;
    default:
      mmgsdi_status = MMGSDI_ERROR;
      UIM_MSG_ERR_1("Could not retrieve slot index for slot id 0x%x",
                    mmgsdi_slot);
      break;
  }
  return mmgsdi_status;
} /* mmgsdi_util_get_slot_data_ptr*/


/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_SLOT_DATA_PTR

DESCRIPTION
  This function bases on the MMGSDI slot id passed in, return the pointer to the
  mmgsdi_data_slot_ptr

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  slot pointer in mmgsdi_slot_data_ptr[] or NULL if it is invalid slot id

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_slot_data_type* mmgsdi_util_get_slot_data_ptr(
  mmgsdi_slot_id_enum_type mmgsdi_slot)
{
  mmgsdi_slot_data_type *mmgsdi_slot_data_ptr = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  switch(mmgsdi_slot)
  {
    case MMGSDI_SLOT_1:
      mmgsdi_slot_data_ptr = mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX];
      break;
    case MMGSDI_SLOT_2:
      mmgsdi_slot_data_ptr = mmgsdi_data_slot_ptr[MMGSDI_SLOT_2_INDEX];
      break;
    case MMGSDI_SLOT_3:
      mmgsdi_slot_data_ptr = mmgsdi_data_slot_ptr[MMGSDI_SLOT_3_INDEX];
      break;
    default:
      UIM_MSG_ERR_1("Invalid mmgsdi slot 0x%x", mmgsdi_slot);
      return NULL;
  }

  if(mmgsdi_slot_data_ptr == NULL)
  {
    UIM_MSG_ERR_1("slot_data_ptr is NULL for slot: 0x%x", mmgsdi_slot);
  }

  return mmgsdi_slot_data_ptr;
} /* mmgsdi_util_get_slot_data_ptr*/


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
  univ pin tbl  pointer in mmgsdi_app_univ_pin_table_ptr[] or NULL
  if it is invalid slot id

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_app_pin_info_type* mmgsdi_util_get_univ_pin_tbl_ptr(
  mmgsdi_slot_id_enum_type mmgsdi_slot)
{
  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  switch(mmgsdi_slot)
  {
    case MMGSDI_SLOT_1:
      return mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX];
    case MMGSDI_SLOT_2:
      return mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_2_INDEX];
    case MMGSDI_SLOT_3:
      return mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_3_INDEX];
    default:
      UIM_MSG_ERR_1("GET APP UNIV PIN TBL PTR - Invalid mmgsdi slot 0x%x",
                    mmgsdi_slot);
      return NULL;
  }
} /* mmgsdi_util_get_univ_pin_tbl_ptr*/


/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_APP_PIN_INFO_TBL_PPTR

DESCRIPTION
  This function bases on the MMGSDI slot id passed in, return the pointer to
  the mmgsdi_app_pin_info_table

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
   slot specific pointer in mmgsdi_app_pin_table_ptr pointer address
   or NULL if it is invalid slot id

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_int_app_info_type** mmgsdi_util_get_app_pin_info_tbl_ptr(
  mmgsdi_slot_id_enum_type mmgsdi_slot)
{
  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  switch(mmgsdi_slot)
  {
    case MMGSDI_SLOT_1:
      return mmgsdi_app_pin_table_ptr[MMGSDI_SLOT_1_INDEX];
    case MMGSDI_SLOT_2:
      return mmgsdi_app_pin_table_ptr[MMGSDI_SLOT_2_INDEX];
    case MMGSDI_SLOT_3:
      return mmgsdi_app_pin_table_ptr[MMGSDI_SLOT_3_INDEX];
    default:
      UIM_MSG_ERR_1("GET APP PIN INFO TBL PTR - Invalid mmgsdi slot 0x%x",
                    mmgsdi_slot);
      return NULL;
  }
} /* mmgsdi_util_get_app_pin_info_tbl_ptr */


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
   slot specific pointer in mmgsdi_app1_pin_info_table pointer address
   or NULL if it is invalid slot id

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_app_pin_info_type** mmgsdi_util_get_app_pin1_info_tbl_ptr(
  uint8                       *pin1_info_tbl_size_ptr,
  mmgsdi_slot_id_enum_type     mmgsdi_slot)
{
  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  *pin1_info_tbl_size_ptr =
    sizeof(mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX])/sizeof(mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][0]);

  switch(mmgsdi_slot)
  {
    case MMGSDI_SLOT_1:
      return mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX];
    case MMGSDI_SLOT_2:
      return mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_2_INDEX];
    case MMGSDI_SLOT_3:
      return mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_3_INDEX];
    default:
      *pin1_info_tbl_size_ptr = 0;
      UIM_MSG_ERR_1("GET APP PIN1 INFO TBL PTR - Invalid mmgsdi slot 0x%x",
                    mmgsdi_slot);
      return NULL;
  }
} /* mmgsdi_util_get_app_pin1_info_tbl_ptr */


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
  uint8                          *app_index_ptr)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  uint8                   app_index     = 0;
  uint8                   slot_index    = MMGSDI_SLOT_1_INDEX;

  if(aid_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(app_index_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(slot_id, &slot_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* find the app index corresponding to the app */
  if(mmgsdi_app_pin_table_ptr[slot_index] != NULL)
  {
    for(app_index = 0; app_index < MMGSDI_MAX_EXT_APP_INFO; app_index++)
    {
      if(mmgsdi_app_pin_table_ptr[slot_index][app_index] == NULL)
      {
        continue;
      }
      if(mmgsdi_util_cmp_aid(aid_ptr,
                             &(mmgsdi_app_pin_table_ptr[slot_index][app_index]->app_data),
                             FALSE) == MMGSDI_SUCCESS)
      {
        *app_index_ptr = app_index;
        return MMGSDI_SUCCESS;
      }
    }
  }
  return MMGSDI_NOT_FOUND;

  *app_index_ptr = app_index;
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_app_index_from_app */


/*===========================================================================
FUNCTION: MMGSDI_UTIL_GET_SESSION_TYPE

DESCRIPTION
  Based on the session id, this function will return the session type, and
  the aid info

DEPENDENCIES
  none

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_session_type(
  mmgsdi_session_id_type         session_id,
  mmgsdi_session_type_enum_type *session_type_ptr,
  mmgsdi_aid_type               *aid_ptr)
{
  mmgsdi_session_info_type     *session_info_ptr = 0;
  mmgsdi_channel_info_type     *channel_info_ptr = 0;

  MMGSDIUTIL_RETURN_IF_NULL(session_type_ptr);

  if (MMGSDI_SUCCESS !=
      mmgsdi_util_get_session_and_channel_info(session_id,&session_info_ptr,
      &channel_info_ptr) || (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_1("No matching session type found session_id=0x%x", session_id);
    return MMGSDI_ERROR;
  }

  *session_type_ptr = session_info_ptr->session_type;

  if ((aid_ptr != NULL) && (channel_info_ptr != NULL) &&
      (channel_info_ptr->app_info_ptr != NULL))
  {
    mmgsdi_memscpy(aid_ptr,
           sizeof(mmgsdi_aid_type),
           &channel_info_ptr->app_info_ptr->app_data,
           sizeof(mmgsdi_aid_type));
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_session_type */


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
)
{
  mmgsdi_return_enum_type     mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_session_info_type    *session_info_ptr = NULL;
  int32                       client_index      = 0;
  int32                       session_index     = 0;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                    session_id,
                    &client_index,
                    &session_index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* mmgsdi_util_get_client_and_session_index success guarantees valid dereferences
     into mmgsdi_client_id_reg_table and session_info_ptr */
  session_info_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  if(session_info_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null Ptr session_ptr=0x%x for session_id 0x%x",
                  session_info_ptr, session_id);
    return MMGSDI_ERROR;
  }
  if(session_info_pptr != NULL)
  {
    *session_info_pptr = session_info_ptr;
  }

  /* It is possible that the channel info pptr is not yet assigned when this
     function is queried by caller and that is not an error condition */
  if(channel_info_pptr != NULL &&
     session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO)
  {
    *channel_info_pptr = mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index];
  }
  return mmgsdi_status;
} /* mmgsdi_util_get_session_and_channel_info */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_AND_SESSION_INDEX

   DESCRIPTION:
     This function checks if the session ID provided is valid
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
)
{
  int32                  i             = MMGSDI_MAX_CLIENT_INFO;
  int32                  j             = MMGSDI_MAX_SESSION_INFO;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(client_index_ptr, session_index_ptr);

  if(session_id == 0)
  {
    return MMGSDI_ERROR;
  }

  /* Parse through client ID table for a matching session Id */
  for(i =  0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if(mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    for(j = 0; j < MMGSDI_MAX_SESSION_INFO; j++)
    {
      if(mmgsdi_client_id_reg_table[i]->session_info_ptr[j] != NULL)
      {
        if(mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_id
           == session_id)
        {
          *client_index_ptr    = i;
          *session_index_ptr   = j;
          return MMGSDI_SUCCESS;
        }
      }
    }
  }
  UIM_MSG_ERR_1("No matching session_id=0x%x found in client_reg_table",
                session_id);
  return MMGSDI_ERROR;
} /* mmgsdi_util_get_client_and_session_index */


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
)
{
  mmgsdi_return_enum_type    mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_session_info_type  *session_info_ptr = NULL;
  mmgsdi_channel_info_type  *channel_info_ptr = NULL;
  mmgsdi_session_id_type     session_id       = 0;

  if(app_info_pptr != NULL)
  {
    *app_info_pptr = NULL;
  }
  if(session_id_ptr != NULL)
  {
    *session_id_ptr = 0;
  }

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    mmgsdi_generic_data_ptr->pri_gw_session_id,
                    &session_info_ptr,
                    &channel_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS)   ||
     (channel_info_ptr == NULL)          ||
     (channel_info_ptr->channel_id != 0) ||
     ((slot_id != MMGSDI_SLOT_AUTOMATIC) &&
      (channel_info_ptr->slot_id != slot_id)))
  {
    mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                                    mmgsdi_generic_data_ptr->pri_1x_session_id,
                                    &session_info_ptr,
                                    &channel_info_ptr);

    if((mmgsdi_status != MMGSDI_SUCCESS)   ||
       (channel_info_ptr == NULL)          ||
       (channel_info_ptr->channel_id != 0) ||
       ((slot_id != MMGSDI_SLOT_AUTOMATIC) &&
        (channel_info_ptr->slot_id != slot_id)))
    {
      mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                                    mmgsdi_generic_data_ptr->sec_gw_session_id,
                                    &session_info_ptr,
                                    &channel_info_ptr);

      if((mmgsdi_status != MMGSDI_SUCCESS)   ||
         (channel_info_ptr == NULL)          ||
         (channel_info_ptr->channel_id != 0) ||
         ((MMGSDI_SLOT_AUTOMATIC != slot_id) &&
          (channel_info_ptr->slot_id != slot_id)))
      {
        mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                                    mmgsdi_generic_data_ptr->sec_1x_session_id,
                                    &session_info_ptr,
                                    &channel_info_ptr);

        if((mmgsdi_status != MMGSDI_SUCCESS)   ||
           (channel_info_ptr == NULL)          ||
           (channel_info_ptr->channel_id != 0) ||
           ((MMGSDI_SLOT_AUTOMATIC != slot_id) &&
            (channel_info_ptr->slot_id != slot_id)))
        {
          mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                                      mmgsdi_generic_data_ptr->ter_gw_session_id,
                                      &session_info_ptr,
                                      &channel_info_ptr);

          if((mmgsdi_status != MMGSDI_SUCCESS)   ||
             (channel_info_ptr == NULL)          ||
             (channel_info_ptr->channel_id != 0) ||
             ((MMGSDI_SLOT_AUTOMATIC != slot_id) &&
              (channel_info_ptr->slot_id != slot_id)))
          {
            mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                                        mmgsdi_generic_data_ptr->ter_1x_session_id,
                                        &session_info_ptr,
                                        &channel_info_ptr);

            if((mmgsdi_status != MMGSDI_SUCCESS)   ||
               (channel_info_ptr == NULL)          ||
               (channel_info_ptr->channel_id != 0) ||
               ((MMGSDI_SLOT_AUTOMATIC != slot_id) &&
                (channel_info_ptr->slot_id != slot_id)))
            {
              UIM_MSG_ERR_1("No Session on Channel 0 for slot 0x%x", slot_id);
              return MMGSDI_ERROR;
            }
            else
            {
              session_id = mmgsdi_generic_data_ptr->ter_1x_session_id;
            }
          }
          else
          {
            session_id = mmgsdi_generic_data_ptr->ter_gw_session_id;
          }
        }
        else
        {
          session_id = mmgsdi_generic_data_ptr->sec_1x_session_id;
        }
      }
      else
      {
        session_id = mmgsdi_generic_data_ptr->sec_gw_session_id;
      }
    }
    else
    {
      session_id = mmgsdi_generic_data_ptr->pri_1x_session_id;
    }
  }
  else
  {
    session_id = mmgsdi_generic_data_ptr->pri_gw_session_id;
  }

  if(app_info_pptr != NULL)
  {
    *app_info_pptr = channel_info_ptr->app_info_ptr;
  }
  if(session_id_ptr != NULL)
  {
    *session_id_ptr = session_id;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_get_ch_zero_app */


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
)
{
  int32                           j = 0,k           = 0;
  mmgsdi_session_info_type      * temp_session_ptr  = NULL;
  mmgsdi_return_enum_type         mmgsdi_status     = MMGSDI_SUCCESS;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(app_data_ptr, session_info_pptr);

  /* Get Session Id for the app */
  for(j=0; j<MMGSDI_MAX_CLIENT_INFO; j++)
  {
    if(mmgsdi_client_id_reg_table[j] == NULL)
    {
      continue;
    }

    for(k=0; k<MMGSDI_MAX_SESSION_INFO; k++)
    {
      temp_session_ptr =
        mmgsdi_client_id_reg_table[j]->session_info_ptr[k];
      if((temp_session_ptr != NULL) &&
         (temp_session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[temp_session_ptr->channel_info_index] != NULL) &&
         (mmgsdi_channel_info_ptr_table[temp_session_ptr->channel_info_index]->app_info_ptr != NULL) &&
         (mmgsdi_channel_info_ptr_table[temp_session_ptr->channel_info_index]->slot_id == slot_id))
      {
        mmgsdi_status = mmgsdi_util_cmp_aid(
          &mmgsdi_channel_info_ptr_table[temp_session_ptr->channel_info_index]->app_info_ptr->app_data,
          app_data_ptr, FALSE);

        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          *session_info_pptr = temp_session_ptr;
          return MMGSDI_SUCCESS;
        }
      }
    }
  }
  UIM_MSG_ERR_0("No session mapped to for given app");
  return MMGSDI_ERROR;
} /* mmgsdi_util_get_session_info_from_app */


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
)
{
  int32                   i = 0, j = 0;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  MMGSDIUTIL_RETURN_IF_NULL(client_id_ptr);

  for(i=0; i<MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if(mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    for(j=0; j<MMGSDI_MAX_SESSION_INFO; j++)
    {
      if((mmgsdi_client_id_reg_table[i]->session_info_ptr[j] != NULL) &&
         (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_id == session_id))
      {
        *client_id_ptr = mmgsdi_client_id_reg_table[i]->client_id;
        return MMGSDI_SUCCESS;
      }
    }
  }
  UIM_MSG_ERR_1("No matching client found session_id=0x%x", session_id);
  return MMGSDI_ERROR;
} /* mmgsdi_util_get_client_id */


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
)
{
  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_1X_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_1X_PROV_SEC_SESSION:
    case MMGSDI_GW_PROV_TER_SESSION:
    case MMGSDI_1X_PROV_TER_SESSION:
      return TRUE;
    default:
      break;
  }
  return FALSE;
} /* mmgsdi_util_is_prov_session */


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
)
{
  switch(session_type)
  {
    case MMGSDI_NON_PROV_SESSION_SLOT_1:
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      return TRUE;
    default:
      break;
  }
  return FALSE;
} /* mmgsdi_util_is_non_prov_session */


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
  boolean:  TRUE : Card session
            FALSE: Other sessions

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_card_session(
  mmgsdi_session_type_enum_type session_type
)
{
  switch(session_type)
  {
    case MMGSDI_CARD_SESSION_SLOT_1:
    case MMGSDI_CARD_SESSION_SLOT_2:
    case MMGSDI_CARD_SESSION_SLOT_3:
      return TRUE;
    default:
      break;
  }
  return FALSE;
} /* mmgsdi_util_is_card_session */

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
)
{
  mmgsdi_meid_data_type    meid_nv_data;
  boolean                  card_meid_avail   = FALSE;

  memset((byte*)(&meid_nv_data), 0x00, sizeof(mmgsdi_meid_data_type));
  meid_nv_data = mmgsdi_get_meid();

  /* check if service MEID is supported and activated */
  if( MMGSDI_SUCCESS !=  mmgsdi_icc_chk_srv_available(session_id,
                                                      MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                      MMGSDI_CDMA_SRV_MEID,
                                                      &card_meid_avail))
  {
    card_meid_avail = FALSE;
  }
  if( meid_nv_data.meid_available && card_meid_avail)
  {
    return TRUE;
  }
  return FALSE;
} /* mmgsdi_util_get_meid_availability */


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
)
{
  (void)cnf_ptr;

  switch(cnf)
  {
    case MMGSDI_WRITE_CNF:
      if(status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("MMGSDI_DISCARD_CB: File write failed, Status=0x%x",
                      status);
      }
      break;
    default:
      break;
  }
} /* mmgsdi_util_discard_cb */


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
)
{
  MMGSDIUTIL_RETURN_IF_NULL_2(input_uim_rsp_ptr, output_uim_rsp_pptr);

  /* Memory allocation for output_uim_rsp_pptr and check for allocation */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*output_uim_rsp_pptr,
    sizeof(mmgsdi_uim_report_rsp_type));
  if ((*output_uim_rsp_pptr) == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /* Copy the UIM response */
  mmgsdi_memscpy((void*)*output_uim_rsp_pptr,
                 sizeof(mmgsdi_uim_report_rsp_type),
                 (void*)input_uim_rsp_ptr,
                 sizeof(mmgsdi_uim_report_rsp_type));

  (*output_uim_rsp_pptr)->rsp_data.data_ptr = NULL;
  (*output_uim_rsp_pptr)->rsp_data.data_len = 0;

  /* Copy UIM response data */
  if((input_uim_rsp_ptr->rsp_data.data_ptr != NULL) &&
     (input_uim_rsp_ptr->rsp_data.data_len > 0))
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE((*output_uim_rsp_pptr)->rsp_data.data_ptr,
      input_uim_rsp_ptr->rsp_data.data_len);

    if((*output_uim_rsp_pptr)->rsp_data.data_ptr != NULL)
    {
      (*output_uim_rsp_pptr)->rsp_data.data_len =
        input_uim_rsp_ptr->rsp_data.data_len;

      mmgsdi_memscpy((void*)(*output_uim_rsp_pptr)->rsp_data.data_ptr,
                     int32touint32((*output_uim_rsp_pptr)->rsp_data.data_len),
                     (void*)input_uim_rsp_ptr->rsp_data.data_ptr,
                     int32touint32((*output_uim_rsp_pptr)->rsp_data.data_len));
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_alloc_copy_uim_rsp */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_PROV_APP

   DESCRIPTION:
     This function verifies if the requested application is activated on one
     of the provisioning Sessions.

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
)
{
  mmgsdi_session_info_type * curr_session_ptr = NULL;
  int32                      client_index     = 0;
  int32                      session_index    = 0;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(aid_ptr, is_prov_ptr);

  if (mmgsdi_client_id_reg_table[client_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  *is_prov_ptr = FALSE;

  /* Scan through all open sessions of MMGSDI to identify if it matches the
     application passed in (All provisioning apps shall be linked to MMGSDI sessions,
     hence no need to check for other client Sessions) */
  for (session_index = 0; session_index < MMGSDI_MAX_SESSION_INFO; session_index++)
  {
    curr_session_ptr =
      mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];
    if ((curr_session_ptr != NULL) &&
        (curr_session_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
        (mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index] != NULL) &&
        (mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->app_info_ptr != NULL))
    {
      if (slot_id != mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->slot_id)
      {
        continue;
      }

      if (mmgsdi_util_is_prov_session(curr_session_ptr->session_type))
      {
        if (mmgsdi_util_cmp_aid(
              &mmgsdi_channel_info_ptr_table[curr_session_ptr->channel_info_index]->app_info_ptr->app_data,
              aid_ptr, FALSE) == MMGSDI_SUCCESS)
        {
          /* The application is a provisioning one */
          *is_prov_ptr = TRUE;
          return MMGSDI_SUCCESS;
        }
      }
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_is_prov_app */


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
  boolean                  *act_in_progress)
{
  MMGSDIUTIL_RETURN_IF_NULL_2(int_app_info_ptr, act_in_progress);

  *act_in_progress = FALSE;

  if(int_app_info_ptr->app_selected == TRUE)
  {
    switch(int_app_info_ptr->app_state)
    {
      case MMGSDI_APP_STATE_UNKNOWN:
      case MMGSDI_APP_STATE_DETECTED:
      case MMGSDI_APP_STATE_READY:
      case MMGSDI_APP_STATE_DEACTIVATED:
      case MMGSDI_APP_STATE_ILLEGAL:
      case MMGSDI_APP_STATE_EXT_PERSO_FAILED:
        break;
      case MMGSDI_APP_STATE_ACTIVATED:
      case MMGSDI_APP_STATE_PERSO_VERIFIED:
      case MMGSDI_APP_STATE_WAIT_FOR_SUB_OK:
        *act_in_progress = TRUE;
        break;
      case MMGSDI_APP_STATE_READY_FOR_PERSO:
        if (int_app_info_ptr->perso_state != MMGSDI_SIMLOCK_UNLOCK_REQUIRED)
        {
          *act_in_progress = TRUE;
        }
        break;
      case MMGSDI_APP_STATE_PIN_EVT_SENT:
        if(mmgsdi_util_can_proceed_to_post_pin1(
             (const mmgsdi_int_app_info_type*)int_app_info_ptr))
        {
          *act_in_progress = TRUE;
        }
        break;
      default:
        break;
    }
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_is_app_activation_in_progress */


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
  const mmgsdi_cnf_type   *cnf_ptr)
{
  int32                              client_index    = 0;
  mmgsdi_request_header_type         req_header      = {0};
  uim_cmd_type                      *uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr = NULL;
  mmgsdi_sw_status_type              status_word     = {0};
  mmgsdi_int_app_info_type          *app_info_ptr    = NULL;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_0("mmgsdi_util_uicc_hidddenkey_ef_write_cb");

  if (cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("mmgsdi_util_uicc_hidddenkey_ef_write_cb: Cnf Ptr is NULL");
    return;
  }

  /* Obtain the index */
  client_index = (int32)cnf_ptr->response_header.client_data;

  /* Retrieve the original Hiddenkey request */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                    client_index,
                    &req_header,
                    &uim_cmd_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Hiddenkey EF Write CB: Could not extract the client info - 0x%x",
                  mmgsdi_status);
    return;
  }
  /* If a confirmation has already been sent for this command as part of
     CARD_ERROR handling, free only the client_req_table index. */
  if((client_index >= 0) && (client_index < MMGSDI_MAX_CLIENT_REQ_INFO) &&
     (mmgsdi_client_req_table_info_ptr[client_index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED))
  {
    UIM_MSG_HIGH_2("Free client_req_table index 0x%x for cmd 0x%x",
                   client_index,
                   mmgsdi_client_req_table_info_ptr[client_index]->request_type);

    mmgsdi_util_free_client_request_table_index(client_index);
    return;
  }

  switch(cnf)
  {
    case MMGSDI_WRITE_CNF:
      if(status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Hiddenkey EF Write CB: Hiddenkey write did not succeed, status 0x%x",
                      status);
        status = MMGSDI_ERROR;
      }
      else
      {
        /* Get the app related to the session */
        mmgsdi_status = mmgsdi_util_get_prov_session_info(req_header.session_id,
                                                          NULL,
                                                          NULL,
                                                          &app_info_ptr);

        if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
        {
          UIM_MSG_ERR_0("Hiddenkey EF Write CB: Could not retrieve the prov app info");
          status = MMGSDI_ERROR;
        }
        else
        {
          if (extra_param_ptr != NULL)
          {
             /* If the request is for Hiddenkey disable */
            if(extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_DISABLE)
            {
              app_info_ptr->hiddenkey.status = MMGSDI_PIN_DISABLED;
            }
            /* The request is for enable/change Hiddenkey */
            else if((extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_ENABLE) ||
                    (extra_param_ptr->pin_data.orig_pin_op == MMGSDI_PIN_OP_CHANGE))
            {
              app_info_ptr->hiddenkey.status = MMGSDI_PIN_ENABLED_VERIFIED;
            }
            mmgsdi_memscpy((void*)&extra_param_ptr->pin_data.pin_info,
                           sizeof(mmgsdi_app_pin_info_type),
                           (const void*)&app_info_ptr->hiddenkey,
                           sizeof(mmgsdi_app_pin_info_type));
          }
        }
      }

      /* Queue the confirmation */
      mmgsdi_status = mmgsdi_cnf_build_and_queue(status,
                                                 &req_header,
                                                 extra_param_ptr,
                                                 NULL,
                                                 FALSE,
                                                 status_word);
      break;
    default:
      UIM_MSG_ERR_1("Hiddenkey EF Write CB: Incorrect cnf type 0x%x", cnf);
      return;
  }
  mmgsdi_util_free_client_request_table_index(client_index);
} /* mmgsdi_util_uicc_hidddenkey_ef_write_cb */


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
  mmgsdi_slot_id_enum_type slot_id)
{
  mmgsdi_protocol_enum_type protocol_type = MMGSDI_MAX_PROTOCOL_ENUM;
  uim_protocol_type         uim_protocol  = UIM_NO_SUCH_MODE;

  /* Get the MMGSDI protocol */
  protocol_type = mmgsdi_util_determine_protocol(slot_id);

  /* Convert to UIM protocol */
  switch(protocol_type)
  {
    case MMGSDI_ICC:
      uim_protocol = UIM_ICC;
      break;
    case MMGSDI_UICC:
      uim_protocol = UIM_UICC;
      break;
    default:
      break;
  }
  return uim_protocol;
} /* mmgsdi_util_get_uim_protocol */


/* ============================================================================
   FUNCTION:      mmgsdi_util_check_file_access_data

   DESCRIPTION:
     This function check the file access parameter for given session type.

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
  mmgsdi_protocol_enum_type      protocol)
{
  mmgsdi_uim_ef_item_category_enum_type EF_category      = ROOT;
  mmgsdi_df_enum_type                   df_enum          = MMGSDI_MAX_DF_ENUM;
  mmgsdi_session_info_type            * session_info_ptr = NULL;
  mmgsdi_return_enum_type               mmgsdi_status    = MMGSDI_SUCCESS;

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    session_id,&session_info_ptr,NULL);

  if ((session_info_ptr == NULL) || (mmgsdi_status != MMGSDI_SUCCESS))
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_0("Error finding session and channel info");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* EF ENUM */
  if (file_access.access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    EF_category = MMGSDIFILE_GET_CATEGORY_FROM_FILE(file_access.file.file_enum);
    if (mmgsdi_util_is_prov_session(session_info_ptr->session_type))
    {
      if(((MMGSDI_ICC == protocol)  && ((EF_category == USIM) || (EF_category == CSIM))) ||
         ((MMGSDI_UICC == protocol) && ((EF_category == GSM)  || (EF_category == CDMA))))
      {
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      }
    }
    else if (mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
    {
      if(MMGSDI_ICC == protocol)
      {
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      }
    }
    else if (mmgsdi_util_is_card_session(session_info_ptr->session_type))
    {
      if((EF_category == USIM) ||(EF_category == CSIM))
      {
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      }
    }
    else
    {
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    }
  }
  else if ((file_access.access_method == MMGSDI_BY_PATH_ACCESS) || /* By Path */
           (file_access.access_method == MMGSDI_DF_ENUM_ACCESS))   /* DF Enum */
  {
    if (file_access.access_method == MMGSDI_BY_PATH_ACCESS)
    {
      if((file_access.file.path_type.path_len <= 0) ||
         (file_access.file.path_type.path_buf[0] != (uint16)MMGSDI_MF_DIR))
      {
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      }
      else
      {
        df_enum = (mmgsdi_df_enum_type)file_access.file.path_type.path_buf[1];
      }
    }
    else
    {
      df_enum = file_access.file.df_enum;
    }

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      switch(df_enum)
      {
        case MMGSDI_MF_DIR:
          if (file_access.access_method == MMGSDI_BY_PATH_ACCESS)
          {
            mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
          }
          break;

        case MMGSDI_RUIM_DIR:
        case MMGSDI_GSM_DIR:
          if((MMGSDI_UICC == protocol) ||
             mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
          {
            mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
          }
          break;

        default:
          break;
      } /*end of switch*/
    }
  }
  else if (file_access.access_method == MMGSDI_BY_APP_ID_ACCESS)
  {
    if(MMGSDI_ICC == protocol)
    {
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
    }
  }
  else  /* Other access method*/
  {
    mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_6("Failed to check file access data: mmgsdi_status 0x%x"
                   "session type 0x%x, protocol 0x%x, file access method 0x%x, "
                   "df enum 0x%x, EF category 0x%x",
                   mmgsdi_status,
                   session_info_ptr->session_type,
                   protocol,
                   file_access.access_method,
                   df_enum,
                   EF_category);
  }

  return mmgsdi_status;
}/* mmgsdi_util_check_file_access_data */


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
  mmgsdi_session_id_type session_id)
{
  mmgsdi_access_type          access;
  mmgsdi_data_type            imsi_m_data;
  mmgsdi_cache_init_enum_type cache_state   = MMGSDI_CACHE_MAX_ENUM ;
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_ERROR;
  boolean                     result        = FALSE;
  mmgsdi_int_app_info_type   *app_info_ptr  = NULL;

  memset(&imsi_m_data, 0x00, sizeof(mmgsdi_data_type));

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
    return MMGSDI_ACT_STATUS_UNKNOWN;
  }
  switch (app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_CSIM:
      access.file.file_enum = MMGSDI_CSIM_IMSI_M;
      break;
    case MMGSDI_APP_RUIM:
      access.file.file_enum = MMGSDI_CDMA_IMSI_M;
      break;
    default:
      return MMGSDI_ACT_STATUS_NOT_APPLICABLE;
  }
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
         imsi_m_data.data_ptr,
         MMGSDI_CSIM_IMSI_M_LENGTH);

  if (imsi_m_data.data_ptr == NULL)
  {
    return MMGSDI_ACT_STATUS_UNKNOWN;
  }

  imsi_m_data.data_len = MMGSDI_CSIM_IMSI_M_LENGTH;
  mmgsdi_status = mmgsdi_cache_read(session_id,
                                         &access,
                                         &imsi_m_data,
                                         0,
                                         &cache_state);

  /* Check if the read succeeded. */
  if(mmgsdi_status == MMGSDI_SUCCESS && cache_state != MMGSDI_CACHE_NOT_FOUND)
  {
    /* Check the 8th bit of byte 8 of the IMSI_M EF to determine if IMSI_M
       is programmed or not */
    if( (imsi_m_data.data_ptr[7] & 0x80) == 0)
    {
      result = MMGSDI_ACT_STATUS_NOT_ACTIVATED;
    }
    else
    {
      result = MMGSDI_ACT_STATUS_ACTIVATED;
    }
  }
  else
  {
    UIM_MSG_ERR_1("Failed to read CSIM/RUIM IMSI_M from cache, cache_state: 0x%x",
                  cache_state);
    result = MMGSDI_ACT_STATUS_UNKNOWN;
  }

  MMGSDIUTIL_TMC_MEM_FREE(imsi_m_data.data_ptr);
  return result;
} /* mmgsdi_util_check_activation_status */


/* ============================================================================
   FUNCTION:      mmgsdi_util_clear_app_capabilities

   DESCRIPTION:
     This function clears the app capabilities.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_SUCCESS: When all the appc apabilities are successfully cleared.
     MMGSDI_INCORRECT_PARAMS: when app_info_ptr comes as NULL.

   SIDE EFFECTS:
     None

CRITICAL SECTIONS:
     The calling function must enter mmgsdi_client_app_data_crit_sect_ptr crit
     sect before invoking this util function. Protection for app info table.
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_clear_app_capabilities(
  mmgsdi_app_capabilities_type      *app_capabilities
)
{
  MMGSDIUTIL_RETURN_IF_NULL(app_capabilities);

  app_capabilities->fdn_state                                   = MMGSDI_SRV_NOT_INIT;
  app_capabilities->bdn_state                                   = MMGSDI_SRV_NOT_INIT;
  app_capabilities->cc_state                                    = MMGSDI_SRV_NOT_INIT;
  app_capabilities->acl_state                                   = MMGSDI_SRV_NOT_INIT;
  app_capabilities->imsi_invalidated                            = FALSE;
  app_capabilities->all_srv_init                                = FALSE;
  app_capabilities->unknown_capabilities                        = FALSE;

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_clear_app_capabilities */


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
  const mmgsdi_path_type * path_ptr)
{
  int32 i = 0;

  if(path_ptr == NULL)
  {
    return FALSE;
  }

  if(path_ptr->path_len > 0 && path_ptr->path_len <= MMGSDI_MAX_PATH_LEN)
  {
    if(path_ptr->path_buf[0] != (uint16)MMGSDI_MF_DIR)
    {
      UIM_MSG_ERR_1("Invalid MF address: 0x%x", path_ptr->path_buf[0]);
      return FALSE;
    }

    for(i = 1; i < path_ptr->path_len; i++)
    {
      if((path_ptr->path_buf[i] == 0) || (path_ptr->path_buf[i] == 0xFFFF))
      {
        UIM_MSG_ERR_2("Invalid DF/EF ID at path index: 0x%x, path_len: 0x%x",
                      i, path_ptr->path_len);
        return FALSE;
      }
    }
  }
  else
  {
    UIM_MSG_ERR_1("Invalid path length: 0x%x", path_ptr->path_len);
    return FALSE;
  }

  return TRUE;
} /* mmgsdi_util_is_path_valid */


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
  const mmgsdi_static_data_type * aid_ptr)
{
  mmgsdi_app_enum_type app_type = MMGSDI_APP_UNKNOWN;

  /* AID[0] to AID[6] are used to check the app type, so make sure AID[0] to AID[6]
     is valid by checking the AID length first */
  if(aid_ptr != NULL && aid_ptr->data_len >=7)
  {
    if (mmgsdi_util_is_usim_aid(aid_ptr->data_ptr, aid_ptr->data_len))
    {
      app_type = MMGSDI_APP_USIM;
    }
    else if (mmgsdi_util_is_csim_aid(aid_ptr->data_ptr, aid_ptr->data_len))
    {
      app_type = MMGSDI_APP_CSIM;
    }
  }
  return app_type;
}/* mmgsdi_util_determine_app_type_from_aid */


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
     MMGSDI_SUCCESS: when client id is non zero.
     MMGSDI_INCORRECT_PARAMS: when client id is zero

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_client_id_valid_in_cmd(
  mmgsdi_client_id_type client_id,
  mmgsdi_cmd_enum_type  cmd_enum
)
{
  if(client_id == MMGSDI_CLIENT_ID_ZERO)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_util_is_client_id_valid_in_cmd*/


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
)
{
  if((session_id == MMGSDI_SESSION_ID_ZERO) ||
     (session_id == MMGSDI_INVALID_SESSION_ID))
  {
    UIM_MSG_ERR_2("Cmd: 0x%x, Session id is Invalid: 0x%x",
                  cmd_enum, session_id);
    return MMGSDI_INCORRECT_PARAMS;
  }
  return MMGSDI_SUCCESS;
}/*mmgsdi_util_is_session_id_valid_in_cmd */


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
)
{
  mmgsdi_return_enum_type             mmgsdi_status         = MMGSDI_SUCCESS;
  mmgsdi_data_type                    imsi_buf;
  mmgsdi_data_type                    ad_buf;
  uint16                              imsi_mcc              = 0;
  uint16                              imsi_mnc              = 0;
  uint16                              tmsi_mcc              = 0;
  uint16                              tmsi_mnc              = 0;
  int32                               i                     = 0;
  /* Old SIM cards had only 2 digits MNC and EF-AD with 3 bytes only.
     Default to 2 digits to keep backward compatibility */
  uint8                               imsi_num_mnc_digits   = MMGSDIUTIL_MNC_2_DIGITS;
  mmgsdi_file_enum_type               file_name             = MMGSDI_MAX_FILE_ENUM;
  mmgsdi_channel_info_type          * channel_info_ptr      = NULL;
  boolean                             perso_active          = FALSE;
#ifdef FEATURE_SIMLOCK
  uint8                               slot_index            = 0;
#endif /* FEATURE_SIMLOCK */

  UIM_MSG_HIGH_0("mmgsdi_util_tmsi_plmn_check");
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_TMSI_PLMN_CHECK, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_DISABLED)
  {
    return mmgsdi_status;
  }

  memset(&imsi_buf, 0x00, sizeof(mmgsdi_data_type));
  memset(&ad_buf, 0x00, sizeof(mmgsdi_data_type));

  /* retrieve slot id and app type from session id*/
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                             session_id,
                             NULL,
                             &channel_info_ptr);

  if((mmgsdi_status != MMGSDI_SUCCESS)
       ||(channel_info_ptr == NULL)
       ||(channel_info_ptr->app_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

#ifdef FEATURE_SIMLOCK
  mmgsdi_status = mmgsdi_util_get_slot_index(channel_info_ptr->slot_id,
                                             &slot_index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to determine slot index");
    return MMGSDI_ERROR;
  }

  for(i = 0; i < SIMLOCK_CATEGORY_COUNT_MAX; i++)
  {
    if(mmgsdi_simlock_status_table[slot_index][i].category_status)
    {
      /* continue TMSI PLMN check as if perso is enabled. */
      perso_active = TRUE;
      break;
    }
  }
#endif /* FEATURE_SIMLOCK */

  UIM_MSG_HIGH_1("mmgsdi_util_tmsi_plmn_check: perso_active 0x%x", perso_active);

  /* Compare IMSI PLMN with TMSI PLMN only if SIM LOCK is active */
  if(!perso_active)
  {
    UIM_MSG_HIGH_0("perso inactive, skip TMSI PLMN check");
    return MMGSDI_SUCCESS;
  }

  /* --------------------------------------------------------------------------
   Read the Administrative Data Elementary File
  ------------------------------------------------------------------------*/
  if(channel_info_ptr->app_info_ptr->app_data.app_type == MMGSDI_APP_USIM)
  {
    file_name = MMGSDI_USIM_AD;
  }
  else
  {
    file_name = MMGSDI_GSM_AD;
  }

  mmgsdi_status = mmgsdi_card_init_cache_binary(
                    session_id,
                    channel_info_ptr->app_info_ptr->app_data.app_type,
                    channel_info_ptr->slot_id,
                    file_name,
                    &ad_buf);

  if ( mmgsdi_status != MMGSDI_SUCCESS ||
       ad_buf.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(ad_buf.data_ptr);
    MMGSDI_DEBUG_MSG_ERROR_1("read AD failed or not supported 0x%x",
                             mmgsdi_status);
    return MMGSDI_ERROR;
  }

  /* get MCC MNC from IMSI */
  if(channel_info_ptr->app_info_ptr->app_data.app_type == MMGSDI_APP_USIM)
  {
    file_name = MMGSDI_USIM_IMSI;
  }
  else
  {
    file_name = MMGSDI_GSM_IMSI;
  }
  mmgsdi_status = mmgsdi_card_init_cache_binary(
                    session_id,
                    channel_info_ptr->app_info_ptr->app_data.app_type,
                    channel_info_ptr->slot_id,
                    file_name,
                    &imsi_buf);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     imsi_buf.data_ptr == NULL ||
     imsi_buf.data_len != 9 )
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(imsi_buf.data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(ad_buf.data_ptr);
    MMGSDI_DEBUG_MSG_ERROR_1("TMSI PLMN CHECK: IMSI read failed 0x%x",
                             mmgsdi_status);
    return MMGSDI_ERROR;
  }

  /* Extract MCC from IMSI */
  imsi_mcc  = (uint16)((imsi_buf.data_ptr[1] >> 4) & 0x0F) * 100;
  imsi_mcc += (uint16)((imsi_buf.data_ptr[2]) & 0x0F) * 10;
  imsi_mcc += (uint16)((imsi_buf.data_ptr[2] >> 4) & 0x0F);

  (void)mmgsdi_util_get_num_of_mnc_digits(imsi_buf, ad_buf, &imsi_num_mnc_digits);

  MMGSDIUTIL_TMC_MEM_FREE(ad_buf.data_ptr);

  /* Extract MNC from IMSI */
  imsi_mnc  = (uint16)((imsi_buf.data_ptr[3]) & 0x0F) * 10;
  imsi_mnc += (uint16)((imsi_buf.data_ptr[3] >> 4) & 0x0F);
  if(imsi_num_mnc_digits == 3)
  {
    imsi_mnc  = imsi_mnc * 10;
    imsi_mnc += (uint16)((imsi_buf.data_ptr[4]) & 0x0F);
  }

  /* Extract MCC MNC from LOCI*/
  if(loci_ptr->data_ptr == NULL ||
     loci_ptr->data_len != 11 )
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(imsi_buf.data_ptr);
    MMGSDI_DEBUG_MSG_ERROR_1("TMSI PLMN CHECK: IMSI read failed 0x%x",
                             mmgsdi_status);
    return MMGSDI_ERROR;
  }
  /* format for TMSI_PLMN:
   B4       B5       B6
   MCC2MCC1 MNC3MCC3 MNC2MNC1 */
  tmsi_mcc  = (uint16)((loci_ptr->data_ptr[4]) & 0x0F) * 100;
  tmsi_mcc += (uint16)((loci_ptr->data_ptr[4] >> 4) & 0xF) * 10;
  tmsi_mcc += (uint16)((loci_ptr->data_ptr[5]) & 0x0F);
  tmsi_mnc  = (uint16)((loci_ptr->data_ptr[6]) & 0x0F) * 10;
  tmsi_mnc += (uint16)((loci_ptr->data_ptr[6] >> 4) & 0xF);
  if(imsi_num_mnc_digits == 3)
  {
    tmsi_mnc  = tmsi_mnc * 10;
    tmsi_mnc += (uint16)((loci_ptr->data_ptr[5] >> 4) & 0x0F);
  }

  UIM_MSG_HIGH_2("mmgsdi_util_tmsi_plmn_check: imsi mcc 0x%x, mnc 0x%x",
                 imsi_mcc, imsi_mnc);
  UIM_MSG_HIGH_2("mmgsdi_util_tmsi_plmn_check: tmsi mcc 0x%x, mnc 0x%x",
                 tmsi_mcc, tmsi_mnc);
  if(imsi_mcc != tmsi_mcc || imsi_mnc != tmsi_mnc)
  {
    /* invalidate TMSI and set first 4 bytes of LOCI to 0xFFFFFFFF */
    for(i=0; i<4; i++)
    {
      loci_ptr->data_ptr[i] = loci_ptr->data_ptr[i] | 0xFF;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE(imsi_buf.data_ptr);
  return mmgsdi_status;
}/* mmgsdi_util_tmsi_plmn_check */

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
  mmgsdi_int_app_info_type *sel_app_ptr)
{
  if(sel_app_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(sel_app_ptr->eons_info.spn.eons_data.data_ptr);
  memset(&(sel_app_ptr->eons_info.spn), 0x0, sizeof(mmgsdi_eons_name_type));

  sel_app_ptr->eons_info.lang_ind = 0x0;

  sel_app_ptr->eons_info.display_spn = FALSE;
  sel_app_ptr->eons_info.spn_display_bit =
    MMGSDI_EONS_SPN_DISPLAY_BIT_NOT_AVAILABLE;

  sel_app_ptr->eons_info.rplmn_display_bit =
    MMGSDI_EONS_RPLMN_DISPLAY_BIT_NOT_AVAILABLE;

  mmgsdi_eons_free_nitz_data_ptrs(
     &(sel_app_ptr->eons_info.ss_info.first_stack_ss.nitz_info));

  mmgsdi_eons_free_nitz_data_ptrs(
     &(sel_app_ptr->eons_info.ss_info.sec_stack_ss.nitz_info));

  memset(&(sel_app_ptr->eons_info.ss_info),
         0x0,
         sizeof(mmgsdi_eons_ss_data_type));

  mmgsdi_eons_free_rplmn_info_ptrs(
    &(sel_app_ptr->eons_info.rplmn_info.first_stack_rplmn));

  mmgsdi_eons_free_rplmn_info_ptrs(
    &(sel_app_ptr->eons_info.rplmn_info.sec_stack_rplmn));

  memset(&(sel_app_ptr->eons_info.rplmn_info),
         0x0,
         sizeof(mmgsdi_eons_rplmn_info_type));
}/* mmgsdi_util_app_eons_cleanup */


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
)
{
  mmgsdi_card_err_info_enum_type mmgsdi_error_condition = MMGSDI_CARD_ERR_MAX;

  switch(error_condition)
  {
    case UIM_POLL_ERR_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_POLL_ERROR;
      break;
    case UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_NO_ATR_RCVD_AT_MAX_VOLT;
      break;
    case UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_NO_ATR_RCVD_AFTER_RESET;
      break;
    case UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_CORRUPT_ATR_RCVD_MAX_TIMES;
      break;
    case UIM_PPS_TIMED_OUT_MAX_TIMES_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_PPS_TIMED_OUT_MAX_TIMES;
      break;
    case UIM_VOLTAGE_MISMATCH_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_VOLT_MISMATCH;
      break;
    case UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_INT_CMD_TIMED_OUT_AFTER_PPS;
      break;
    case UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_INT_CMD_ERR_EXCEED_MAX_NUMS;
      break;
    case UIM_MAXED_PARITY_ERROR_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_MAXED_PARITY_ERROR;
      break;
    case UIM_MAXED_RX_BREAK_ERROR_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_MAXED_RX_BREAK_ERR;
      break;
    case UIM_MAXED_OVERRUN_ERROR_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_MAXED_OVERRUN_ERR;
      break;
    case UIM_TRANSACTION_TIMER_EXPIRED_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_TRANS_TIMER_EXP;
      break;
    case UIM_POWER_DOWN_CMD_NOTIFICATION_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY;
      break;
    case UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_INT_ERR_IN_PASSIVE_MODE;
      break;
    case UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_TIMED_OUT_IN_PASSIVE_MODE;
      break;
    case UIM_MAX_PARITY_IN_PASSIVE_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_MAX_PARITY_IN_PASSIVE;
      break;
    case UIM_MAX_RXBRK_IN_PASSIVE_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_MAX_RXBRK_IN_PASSIVE;
      break;
    case UIM_MAX_OVERRUN_IN_PASSIVE_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_MAX_OVERRUN_IN_PASSIVE;
      break;
    case UIM_CARD_REMOVED_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_CARD_REMOVED;
      break;
    case UIM_TECHNICAL_PROBLEM_S:
      mmgsdi_error_condition =  MMGSDI_CARD_ERR_TECHNICAL_PROBLEM;
      break;
    case UIM_NULL_BYTE_ERR_S:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_NULL_BYTES;
      break;
    case UIM_ERR_S:
    default:
      mmgsdi_error_condition = MMGSDI_CARD_ERR_UNKNOWN_ERROR;
      break;
  }

  UIM_MSG_HIGH_2("In mmgsdi_util_convert_from_uim_error_condition, UIM error: 0x%x, MMGSDI error: 0x%x",
                 error_condition, mmgsdi_error_condition);

  return mmgsdi_error_condition;
}/* mmgsdi_util_convert_from_uim_error_condition */


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
  mmgsdi_slot_id_enum_type *mmgsdi_slot_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_slot_ptr);

  switch(uim_slot)
  {
    case UIM_SLOT_1:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_1;
      break;

    case UIM_SLOT_2:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_2;
      break;

    case UIM_SLOT_3:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_3;
      break;

    case UIM_SLOT_AUTOMATIC:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_AUTOMATIC;
      break;

    default:
      UIM_MSG_ERR_1("Invalid uim slot id 0x%x", uim_slot);

      *mmgsdi_slot_ptr = MMGSDI_MAX_SLOT_ID_ENUM;
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_convert_from_uim_slot */


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
  mmgsdi_protocol_enum_type *mmgsdi_protocol_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_protocol_ptr);

  switch(uim_protocol)
  {
    case UIM_ICC:
      *mmgsdi_protocol_ptr = MMGSDI_ICC;
      break;

    case UIM_UICC:
      *mmgsdi_protocol_ptr = MMGSDI_UICC;
      break;

    case UIM_NO_SUCH_MODE:
      *mmgsdi_protocol_ptr = MMGSDI_NO_PROTOCOL;
      break;

    default:
      UIM_MSG_ERR_1("Invalid uim protocol 0x%x", uim_protocol);

      *mmgsdi_protocol_ptr = MMGSDI_MAX_PROTOCOL_ENUM;
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_convert_from_uim_protocol */


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
)
{
  mmgsdi_file_enum_type         items_not_deleted[5];
  uint16                        num_items             = 0;
  mmgsdi_slot_data_type        *slot_data_ptr         = NULL;
  boolean                       is_icc                = FALSE;
  mmgsdi_int_app_info_type     *app_info_ptr          = NULL;
  mmgsdi_return_enum_type       mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type session_type          = MMGSDI_MAX_SESSION_TYPE_ENUM;

  UIM_MSG_HIGH_3("In mmgsdi_util_preinit_mmgsdi, app_type: 0x%x, mode: 0x%x, slot: 0x%x",
                 app_type, refresh_mode, slot);

  memset(items_not_deleted, 0x00, sizeof(mmgsdi_file_enum_type) * 5);

  /* Get the slot data corresponding to the slot */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  switch (refresh_mode)
  {
    case MMGSDI_REFRESH_RESET:
    case MMGSDI_REFRESH_RESET_AUTO:

      /* Reset the card error condition only if the refresh mode is RESET.
         For RESET_AUTO, error cause helps to determine whether CARD_ERROR
         event needs to send to clients or not.*/
      if (refresh_mode == MMGSDI_REFRESH_RESET)
      {
        slot_data_ptr->card_error_condition = MMGSDI_CARD_ERR_UNKNOWN_ERROR;
      }

      /* Clean-up the session and application data corresponding to the slot */
      mmgsdi_util_close_all_sessions_cleanup_and_notify(slot);
      mmgsdi_util_reset_app_data(reselect_app, slot);

      /* Clear common file cache */
      (void)mmgsdi_cache_delete_common_file_cache(slot);
      break;

    case MMGSDI_REFRESH_NAA_INIT:
    case MMGSDI_REFRESH_NAA_INIT_FCN:
    case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
    case MMGSDI_REFRESH_NAA_APP_RESET:
    case MMGSDI_REFRESH_3G_SESSION_RESET:
      /* Check if the card is an ICC card */
      if (mmgsdi_util_is_card_icc(slot_data_ptr, &is_icc) != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("failed to get protocol of slot 0x%x!", slot);
        return;
      }

      /* Get the app infofor the prov. session */
      mmgsdi_status =  mmgsdi_util_get_prov_session_info(session_id,
                                                         NULL,
                                                         NULL,
                                                         &app_info_ptr);

      if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
      {
        UIM_MSG_ERR_0("Nothing to do, app_info_ptr is NULL");
        return;
      }

      /* Set the app_capabilities to Default values */
      (void)mmgsdi_util_clear_app_capabilities(&app_info_ptr->app_capabilities);

      /* Set Default values to Personalization info in app_info_ptr,
         to be used for Late Clients */
      app_info_ptr->perso_info.feature             = MMGSDI_MAX_PERSO_FEATURE_ENUM;
      app_info_ptr->perso_info.num_retries         = 0;
      app_info_ptr->perso_info.num_unblock_retries = 0;

      /* Reset the TP state for ICC cards */
      if (is_icc)
      {
        slot_data_ptr->mmgsdi_tp_state = MMGSDI_TP_STATE_NOT_DONE;
        app_info_ptr->app_tp_state     = MMGSDI_TP_STATE_NOT_DONE;
      }

      mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                                   &session_type,
                                                   NULL);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Failed to map session type from session id 0x%x",
                      session_id);
        return;
      }

      /* Clear CPHS info */
      if(session_type == MMGSDI_GW_PROV_PRI_SESSION &&
         mmgsdi_cphs_info_ptr[MMGSDI_PRI_CPHS_INFO_INDEX] != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_cphs_info_ptr[MMGSDI_PRI_CPHS_INFO_INDEX]->cphs_ons_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_cphs_info_ptr[MMGSDI_PRI_CPHS_INFO_INDEX]->cphs_ons_short_ptr);
        memset(mmgsdi_cphs_info_ptr[MMGSDI_PRI_CPHS_INFO_INDEX],
               0x00, sizeof(mmgsdi_cphs_int_info_type));
      }
      else if(session_type == MMGSDI_GW_PROV_SEC_SESSION &&
              mmgsdi_cphs_info_ptr[MMGSDI_SEC_CPHS_INFO_INDEX] != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_cphs_info_ptr[MMGSDI_SEC_CPHS_INFO_INDEX]->cphs_ons_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_cphs_info_ptr[MMGSDI_SEC_CPHS_INFO_INDEX]->cphs_ons_short_ptr);
        memset(mmgsdi_cphs_info_ptr[MMGSDI_SEC_CPHS_INFO_INDEX],
               0x00, sizeof(mmgsdi_cphs_int_info_type));
      }
      else if(session_type == MMGSDI_GW_PROV_TER_SESSION &&
              mmgsdi_cphs_info_ptr[MMGSDI_TER_CPHS_INFO_INDEX] != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_cphs_info_ptr[MMGSDI_TER_CPHS_INFO_INDEX]->cphs_ons_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          mmgsdi_cphs_info_ptr[MMGSDI_TER_CPHS_INFO_INDEX]->cphs_ons_short_ptr);
        memset(mmgsdi_cphs_info_ptr[MMGSDI_TER_CPHS_INFO_INDEX],
               0x00, sizeof(mmgsdi_cphs_int_info_type));
      }

#ifdef FEATURE_SIMLOCK
      /* Reset Personalization state */
      app_info_ptr->perso_state    = MMGSDI_SIMLOCK_NOT_VERIFIED;
#endif  /* FEATURE_SIMLOCK */

      mmgsdi_util_app_eons_cleanup(app_info_ptr);
      /* Invalidate the File cache, attributes cache and EF-ARR record cache */
      switch (app_info_ptr->app_data.app_type)
      {
        case MMGSDI_APP_USIM:
          (void)mmgsdi_eons_opl_pnn_delete_cache_all(session_id);

          /* Reset the CSG EONS info. The related EFs are read after
             subscription is published */
          mmgsdi_csg_reset_eons_flags_in_app_info(session_id);

          if(refresh_mode == MMGSDI_REFRESH_NAA_APP_RESET)
          {
            num_items = 0;
          }
          else
          {
            /* Don't want to clear out entire Cache, should selectivly clear cache.
               Clearing all files except the following */
            items_not_deleted[0] = MMGSDI_USIM_ECC;
            items_not_deleted[1] = MMGSDI_USIM_LI;
            num_items = 2;
          }

          mmgsdi_cache_delete_record_selective_items(session_id,
                                                     items_not_deleted,
                                                     num_items);

          (void)mmgsdi_cache_delete_prov_file_attr_cache(session_type,
                                                         items_not_deleted,
                                                         num_items);

          (void)mmgsdi_cache_delete_selective_items(MMGSDI_APP_USIM,
                                                    session_type,
                                                    items_not_deleted,
                                                    num_items);

          /* Reset the USIM spec. version */
          app_info_ptr->usim_spec_version = MMGSDI_VERSION_UNKNOWN;

          /* Reset the GPRS Anite MCC indication */
          app_info_ptr->gprs_anite_mcc = FALSE;

          /* Reset the TDS Test SIM indication */
          app_info_ptr->tds_test_imsi = FALSE;

          /* Reset the HZI version */
          app_info_ptr->hzi_version = MMGSDI_UHZI_NONE;
          break;

        case MMGSDI_APP_SIM:
          (void)mmgsdi_eons_opl_pnn_delete_cache_all(session_id);

          /* Don't want to clear out entire Cache, should selectivly clear cache.
             Clearing all files except the following */
          items_not_deleted[0] = MMGSDI_GSM_LP;
          num_items = 1;

          (void)mmgsdi_cache_delete_prov_file_attr_cache(session_type,
                                                         items_not_deleted,
                                                         num_items);

          (void)mmgsdi_cache_delete_selective_items(MMGSDI_APP_SIM,
                                                    session_type,
                                                    items_not_deleted,
                                                    num_items);

          /* Clear flags indicating IMSI/LOCI rehabilitation status */
          slot_data_ptr->rehab_info.imsi_rehabilitated = FALSE;
          slot_data_ptr->rehab_info.imsi_rehabilitated = FALSE;

          /* Reset the GPRS Anite MCC indication */
          app_info_ptr->gprs_anite_mcc = FALSE;

          /* Reset the TDS Test SIM indication */
          app_info_ptr->tds_test_imsi = FALSE;

          /* Reset the HZI version */
          app_info_ptr->hzi_version = MMGSDI_UHZI_NONE;
          break;

       case MMGSDI_APP_RUIM:
          /* Don't want to clear out entire Cache, should selectivly clear cache.
             Clearing all files except the following */
          items_not_deleted[0] = MMGSDI_CDMA_PREF_LANG;
          num_items = 1;

          (void)mmgsdi_cache_delete_prov_file_attr_cache(session_type,
                                                         items_not_deleted,
                                                         num_items);

          (void)mmgsdi_cache_delete_selective_items(MMGSDI_APP_RUIM,
                                                    session_type,
                                                    items_not_deleted,
                                                    num_items);

#ifdef FEATURE_MMGSDI_3GPP2
          /* Clear flags indicating IMSI_M/IMSI_T/TMSI rehabilitation status */
          memset(slot_data_ptr->rehab_info.cdma_fdn_files_rehab,
                 0x00,
                 sizeof(slot_data_ptr->rehab_info.cdma_fdn_files_rehab));
#endif /* FEATURE_MMGSDI_3GPP2 */
          break;

        case MMGSDI_APP_CSIM:
          mmgsdi_cache_delete_record_selective_items(session_id, NULL, 0);

          if(refresh_mode == MMGSDI_REFRESH_NAA_APP_RESET)
          {
            num_items = 0;
          }
          else
          {
            /* Don't want to clear out entire Cache, should selectivly clear cache.
               Clearing all files except the following */
            items_not_deleted[0] = MMGSDI_CSIM_LANG_IND;
            num_items = 1;
          }

          (void)mmgsdi_cache_delete_prov_file_attr_cache(session_type,
                                                         items_not_deleted,
                                                         num_items);

          (void)mmgsdi_cache_delete_selective_items(MMGSDI_APP_CSIM,
                                                    session_type,
                                                    items_not_deleted,
                                                    num_items);
          break;

        default:
          UIM_MSG_ERR_1("Unhandled app type 0x%x",
                        app_info_ptr->app_data.app_type);
          break;
      }
      break;

    case MMGSDI_REFRESH_NAA_FCN:
      /* Do Nothing */
      break;

    default:
      UIM_MSG_ERR_1("Unknown Refresh mode: 0x%x", refresh_mode);
      break;
  }
}/* mmgsdi_util_preinit_mmgsdi */


/*===========================================================================
FUNCTION MMGSDI_UTIL_PARSE_ATR

DESCRIPTION
   This function will parse the ATR.  Parsing will focus on the Card Service
   Data and logical channel support which is are optional bytes in the ATR.
   The Card Services is introduced in the TLV Tag of 0x31 and the logical
   channel support in 0x73.

   If the ATR does not contain Card Indicator field the parsed_atr will
   remain unchanged and the available logical channels will retain the default
   value (4) to maintain legacy behavior.

DEPENDENCIES
   None

RETURN VALUE
   mmgsdi_return_enum_type
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_parse_atr(
  mmgsdi_slot_id_enum_type  slot
)
{
  byte                        index              = 0;
  byte                        mask_length_nibble = 0x0F;
  byte                        length             = 0;
  mmgsdi_hist_char_info_type *hist_ptr           = NULL;
  mmgsdi_slot_data_type      *slot_data_ptr      = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  hist_ptr = &slot_data_ptr->hist_char;

  if(hist_ptr->hist_char_length > MMGSDI_MAX_HIST_CHAR_LEN)
  {
    UIM_MSG_ERR_1("MMGSDI historical data len: 0x%x is greater than maximum",
                  hist_ptr->hist_char_length);
    return MMGSDI_ERROR;
  }

  /* A T R   T L V  from ISO7816-4                          */
  /* 1: Card Indicator 0x80 (MANDATORY)                     */
  /* 2: Country/Issuer Indicator 0x1Y or 0x2Y  (OPTIONAL)   */
  /* 3: Card Services Data 0x31 (OPTIONAL)                  */
  /* 4: Card Capabilities 0x73  (OPTIONAL)                  */

  if (hist_ptr->hist_char_data[index] == MMGSDI_CARD_INDICATOR_TLVS)
  {
    /* ATR Is In TLV Format */

    /* Increment the Index to the 2nd Byte */
    index++;
  }
  else
  {
    return MMGSDI_ERROR;
  }

  while(index < hist_ptr->hist_char_length)
  {
    /* Either Optional Tag 0x1Y or 0x2Y can be Next */
    /* Skip the Appropriate Number of Bytes         */
    if (((hist_ptr->hist_char_data[index] >> 4) ==
          MMGSDI_COUNTRY_NAT_DATA) ||
         ((hist_ptr->hist_char_data[index] >> 4) ==
          MMGSDI_ISSUER_ID_NUM))
    {
      /* What is the Length of Bytes to Skip */
      length = hist_ptr->hist_char_data[index] & mask_length_nibble;

      if (length >= (hist_ptr->hist_char_length - index))
      {
        /* Error:  Length can't be greater than 15 - current index */
        /* Stop Parsing                                            */
        break;
      }
      else
      {
        /* Increment the index to skip Y Bytes */
        index = index + length;
      }
    }

    /* Optional Tag 0x31 can be Next */
    /* Get relevant Information      */
    if ((index < hist_ptr->hist_char_length) &&
        (index < MMGSDI_MAX_HIST_CHAR_LEN) &&
        (hist_ptr->hist_char_data[index] == MMGSDI_CARD_SERVICES_DATA))
    {
      slot_data_ptr->parsed_atr.card_services_data_present = TRUE;

      /* Increment the Index To the Next Byte */
      index++;

      if (index < hist_ptr->hist_char_length)
      {
        /* Is Select by full DF Name Supported */
        if ((hist_ptr->hist_char_data[index] & MMGSDI_AID_BY_FULL_DF_MASK) ==
            MMGSDI_AID_BY_FULL_DF_MASK )
        {
          slot_data_ptr->parsed_atr.select_by_full_df_name = TRUE;
        }
        else
        {
          slot_data_ptr->parsed_atr.select_by_full_df_name = FALSE;
        }

        /* Is Select by Partial DF Name Supported */
        if ((hist_ptr->hist_char_data[index] & MMGSDI_AID_BY_PART_DF_MASK) ==
            MMGSDI_AID_BY_PART_DF_MASK)
        {
          slot_data_ptr->parsed_atr.select_by_part_df_name = TRUE;
        }
        else
        {
          slot_data_ptr->parsed_atr.select_by_part_df_name = FALSE;
        }

        /* Is the AID in EF-DIR */
        if ((hist_ptr->hist_char_data[index] & MMGSDI_AID_IN_EF_DIR_MASK) ==
            MMGSDI_AID_IN_EF_DIR_MASK)
        {
          slot_data_ptr->parsed_atr.aid_available_in_dir = TRUE;
        }
        else
        {
          slot_data_ptr->parsed_atr.aid_available_in_dir = FALSE;
        }

        /* Is the AID in EF-ATR */
        if ((hist_ptr->hist_char_data[index] & MMGSDI_AID_IN_EF_ATR_MASK) ==
            MMGSDI_AID_IN_EF_ATR_MASK)
        {
          slot_data_ptr->parsed_atr.aid_available_in_atr = TRUE;
        }
        else
        {
          slot_data_ptr->parsed_atr.aid_available_in_atr = FALSE;
        }

      } /* if index < max hist bytes */
    } /* CARD_SERVICES_DATA */

    if ((index < hist_ptr->hist_char_length) &&
        (index < MMGSDI_MAX_HIST_CHAR_LEN) &&
        (hist_ptr->hist_char_data[index] == MMGSDI_CARD_CAPABILITIES))
    {
      /* SFI supported or not is found in 6th bit of byte 1 */
      index += 1;
      if (index < hist_ptr->hist_char_length &&
          index < MMGSDI_MAX_HIST_CHAR_LEN &&
          (hist_ptr->hist_char_data[index] & MMGSDI_SFI_SUPPORT_ATR_MASK) ==
            MMGSDI_SFI_SUPPORT_ATR_MASK)
      {
        slot_data_ptr->sfi_supported = TRUE;
      }
      else
      {
        slot_data_ptr->sfi_supported = FALSE;
      }

      /* Logical channels supported by card is found in last
         three bits of third byte of GSDI_CARD_CAPABILITES
         8 7 6 5 4 3 2 1
                   | | |__T
                   | |____Z
                   |______Y

         Supported logical channels = 4Y + 2Z + T + 1     */

      index += 2;
      if (index < hist_ptr->hist_char_length &&
          index < MMGSDI_MAX_HIST_CHAR_LEN)
      {
        slot_data_ptr->available_logical_channels = 1 +
          (hist_ptr->hist_char_data[index] & MMGSDI_LOGICAL_CHAN_LEN_MASK);

        UIM_MSG_HIGH_1("0x%x logical channels have been identified by the ATR",
                       slot_data_ptr->available_logical_channels);

        /* Y = Z = T = 1 means eight or more logical channels
           set the available logical channels to the MAX and
           rely on the card to deny open channel requests */
        if(slot_data_ptr->available_logical_channels == MMGSDI_LOGICAL_CHAN_MAX)
        {
          slot_data_ptr->available_logical_channels = UIM_MAX_CHANNELS;
        }
        /*  For cards specifying fewer channels than legacy behavior
            assume out of spec card and default to legacy behavior. */
        else if(slot_data_ptr->available_logical_channels < MMGSDI_LEGACY_LOGICAL_CHANNELS)
        {
          slot_data_ptr->available_logical_channels = MMGSDI_LEGACY_LOGICAL_CHANNELS;
        }
        /* Update the total logical channel info */
        slot_data_ptr->parsed_atr.total_logical_channels =
          slot_data_ptr->available_logical_channels;

        UIM_MSG_HIGH_1("0x%x logical channels will be used",
                       slot_data_ptr->available_logical_channels);
      }
    } /* CARD_CAPABILITIES */
    index++;
  } /* While */

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_parse_atr */


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
)
{
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type      *select_ptr    = NULL;
  mmgsdi_get_file_attr_cnf_type      *cnf_ptr       = NULL;
  mmgsdi_int_app_info_type           *sel_app_ptr   = NULL;
  mmgsdi_slot_id_enum_type            slot          = MMGSDI_MAX_SLOT_ID_ENUM;

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot,
                                                   NULL,
                                                   &sel_app_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (sel_app_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if((sel_app_ptr->app_data.app_type != MMGSDI_APP_USIM) &&
     (sel_app_ptr->app_data.app_type != MMGSDI_APP_CSIM))
  {
    UIM_MSG_ERR_1("Invalid app type 0x%x", sel_app_ptr->app_data.app_type);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
    sizeof(mmgsdi_get_file_attr_req_type));
  if (select_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  select_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
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
  select_ptr->access.access_method             = MMGSDI_BY_APP_ID_ACCESS;
  /* Since we do not care about the sec attributes, skip the check */
  select_ptr->skip_uicc_arr                    = TRUE;

  mmgsdi_memscpy(&select_ptr->access.file.app_id,
                 sizeof(select_ptr->access.file.app_id),
                 &sel_app_ptr->app_data.aid,
                 sizeof(mmgsdi_static_data_type));

  select_ptr->activate_aid                     = TRUE;


  mmgsdi_status = mmgsdi_uim_uicc_select (select_ptr,
                                          TRUE,
                                          &cnf_ptr,
                                          TRUE);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_ptr);

  if (cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cnf Ptr Null after SELECT ADF");
    return MMGSDI_ERROR;
  }

  if( (mmgsdi_status != MMGSDI_SUCCESS) ||
      (cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_0("Select ADF before PIN2 verification failed");
    mmgsdi_status = MMGSDI_ERROR;
  }

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  return mmgsdi_status;
} /* mmgsdi_init_select_adf_before_pin2_op */

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
)
{
  switch(ef)
  {
    case MMGSDI_ICCID:
    case MMGSDI_ELP:
    case MMGSDI_DIR:

    /* USIM EFs */
    case MMGSDI_USIM_LI:
    case MMGSDI_USIM_ECC:
    case MMGSDI_USIM_NETPAR:
    case MMGSDI_USIM_RPLMNACT:
    case MMGSDI_USIM_AD:
    case MMGSDI_USIM_IMSI:
    case MMGSDI_USIM_ACC:
    case MMGSDI_USIM_HPLMN:
    case MMGSDI_USIM_HPLMNWACT:
    case MMGSDI_USIM_PLMNWACT:
    case MMGSDI_USIM_OPLMNWACT:
    case MMGSDI_USIM_LOCI:
    case MMGSDI_USIM_PSLOCI:
    case MMGSDI_USIM_EPSLOCI:
    case MMGSDI_USIM_KEYS:
    case MMGSDI_USIM_KEYSPS:
    case MMGSDI_USIM_EPSNSC:
    case MMGSDI_USIM_FPLMN:
    case MMGSDI_USIM_START_HFN:
    case MMGSDI_USIM_THRESHOLD:
    case MMGSDI_USIM_CBMID:
    case MMGSDI_USIM_KC:
    case MMGSDI_USIM_KCGPRS:
    case MMGSDI_USIM_ACM:
    case MMGSDI_USIM_ACM_MAX:
    case MMGSDI_USIM_EHPLMN:
    case MMGSDI_USIM_EST:
    case MMGSDI_USIM_EHPLMNPI:
    case MMGSDI_USIM_LRPLMNSI:
    case MMGSDI_USIM_HIDDENKEY:
    case MMGSDI_USIM_SPN:
    case MMGSDI_USIM_SPDI:
    case MMGSDI_USIM_7F66_PROP1_ACT_HPLMN:
    case MMGSDI_USIM_7F66_PROP1_RAT:
    case MMGSDI_USIM_7F66_PROP1_SPT_TABLE:
    case MMGSDI_USIM_NASCONFIG:

    /* GSM EFs */
    case MMGSDI_GSM_LP:
    case MMGSDI_GSM_ECC:
    case MMGSDI_GSM_PHASE:
    case MMGSDI_GSM_HPLMN:
    case MMGSDI_GSM_AD:
    case MMGSDI_GSM_IMSI:
    case MMGSDI_GSM_ACC:
    case MMGSDI_GSM_LOCI:
    case MMGSDI_GSM_KC:
    case MMGSDI_GSM_FPLMN:
    case MMGSDI_GSM_PLMN:
    case MMGSDI_GSM_HPLMNACT:
    case MMGSDI_GSM_PLMNWACT:
    case MMGSDI_GSM_OPLMNWACT:
    case MMGSDI_GSM_LOCIGPRS:
    case MMGSDI_GSM_KCGPRS:
    case MMGSDI_GSM_CBMID:
    case MMGSDI_GSM_ACM:
    case MMGSDI_GSM_ACM_MAX:
    case MMGSDI_GSM_SPN:
    case MMGSDI_GSM_SPDI:

    case MMGSDI_SIM_7F66_PROP1_ACT_HPLMN:
    case MMGSDI_SIM_7F66_PROP1_SPT_TABLE:

    /* CDMA EFs */
    case MMGSDI_CDMA_IMSI_M:
    case MMGSDI_CDMA_IMSI_T:
    case MMGSDI_CDMA_HOME_SVC_PVDR_NAME:
    case MMGSDI_CDMA_ECC:
    case MMGSDI_CDMA_PREF_LANG:
    case MMGSDI_CDMA_SERVICE_PREF:
    case MMGSDI_CDMA_NAM_LOCK:
    case MMGSDI_CDMA_ZONE_BASED_REGN_IND:
    case MMGSDI_CDMA_DIST_BASED_REGN_IND:
    case MMGSDI_CDMA_SYS_REGN_IND:
    case MMGSDI_CDMA_OTAPA_SPC_ENABLE:
    case MMGSDI_CDMA_RUIM_ID:
    case MMGSDI_CDMA_ACCOLC:

    /* CSIM EFs */
    case MMGSDI_CSIM_ECC:
    case MMGSDI_CSIM_LANG_IND:
    case MMGSDI_CSIM_CST:
    case MMGSDI_CSIM_EST:
    case MMGSDI_CSIM_AD:
    case MMGSDI_CSIM_RUIM_ID:
    case MMGSDI_CSIM_SERVICE_PREF:
    case MMGSDI_CSIM_IMSI_M:
    case MMGSDI_CSIM_IMSI_T:
    case MMGSDI_CSIM_ACCOLC:
    case MMGSDI_CSIM_PRL:
    case MMGSDI_CSIM_EPRL:
    case MMGSDI_CSIM_MAXIMUM_PRL:
    case MMGSDI_CSIM_HIDDEN_KEY:
    case MMGSDI_CSIM_SPN:
    case MMGSDI_CSIM_NAM_LOCK:
    case MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND:
    case MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND:
    case MMGSDI_CSIM_CDMA_SYS_REGN_IND:
    case MMGSDI_CSIM_OTAPA_SPC_ENABLE:

      return TRUE;

    default:
      return FALSE;
  }
}/* mmgsdi_util_find_ef_in_power_up_cache_list*/


/*===========================================================================
FUNCTION MMGSDI_UTIL_TIMED_SLEEP

DESCRIPTION
  This function is a wrapper around rex_timed_wait() to replace rex_sleep()
  with a non-deferrable timer.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void mmgsdi_util_timed_sleep(
  unsigned long milisec)
{
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  mmgsdi_task_enum_type  mmgsdi_task = MMGSDI_TASK_MAX;

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the current task is undetermined,
       return */
    return;
  }
  (void)rex_timed_wait(MMGSDI_TIMED_SLEEP_TIMER_SIG,
                       &mmgsdi_timed_sleep_timer[mmgsdi_task], milisec);
  (void)rex_clr_sigs(rex_self(), MMGSDI_TIMED_SLEEP_TIMER_SIG);
#else
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* mmgsdi_util_timed_sleep */


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
)
{
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(channel_id_ptr);

  /* Get the slot data */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* If only standard logical channels are supported, logical
     channel no. is available in lower 2 bits of the CLA */
  if(slot_data_ptr->parsed_atr.total_logical_channels <=
     MMGSDI_LEGACY_LOGICAL_CHANNELS)
  {
    *channel_id_ptr = cla & MMGSDI_CLA_STD_LOGICAL_CHANNEL_MASK;
  }
  else
  {
    /* When card supports extended logical channels, CLA bit b7 is used
       to determine if logical channel number in the CLA is an extended
       logical channel or standard logical channel */
    if(cla & MMGSDI_CLA_B7_MASK)
    {
      *channel_id_ptr = (cla & MMGSDI_CLA_EXT_LOGICAL_CHANNEL_MASK) +
                        MMGSDI_FIRST_EXT_LOGICAL_CHANNEL;
    }
    else
    {
      *channel_id_ptr = cla & MMGSDI_CLA_STD_LOGICAL_CHANNEL_MASK;
    }
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_channel_id_from_cla */


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
)
{
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;
  int32                     client_index  = 0;
  int32                     session_index = 0;
  mmgsdi_session_info_type *session_ptr   = NULL;

  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id,
                                                  &client_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("mmgsdi_util_generate_and_add_session_to_client, Invalid client id low = 0x%x high = 0x%x",
                  (client_id & 0xFFFFFFFF), (client_id >> 32 & 0xFFFFFFFF));
    return NULL;
  }

  /* mmgsdi_util_get_client_id_index success guarantees valid
     client_id_reg_table entry */
  if (mmgsdi_client_id_reg_table[client_index]->num_session >=
      MMGSDI_MAX_SESSION_INFO)
  {
    UIM_MSG_ERR_2("Reached max sessions supported for client id low = 0x%x high = 0x%x",
                  (client_id & 0xFFFFFFFF), (client_id >> 32 & 0xFFFFFFFF));
    return NULL;
  }

  /* Find free session index in the session_info_ptr table and
     allocate memory */
  mmgsdi_status = mmgsdi_util_get_free_session_index(
                    client_index,&session_index);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("No free index available in session_info_ptr[]");
    return NULL;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    /* mmgsdi_util_get_free_session_index success guarantees valid entry
       in client_id_reg_table */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index],
      sizeof(mmgsdi_session_info_type));

    if(mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index] == NULL)
    {
      break;
    }

    session_ptr =
      mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];
    if(session_ptr == NULL)
    {
      UIM_MSG_ERR_2("Session Ptr available in session_info_ptr[]is NULL for client id low = 0x%x high = 0x%x",
                    (client_id & 0xFFFFFFFF), (client_id >> 32 & 0xFFFFFFFF));
      break;
    }

    /* Increment Session count for the client */
    mmgsdi_client_id_reg_table[client_index]->num_session++;

    /* Assign complement of session_ptr as session id to avoid exposing
       pointers. Use least significant 32 bits to maintain legacy behavior. */
    session_ptr->session_id = (~((uint32)session_ptr)) & 0xFFFFFFFF;

    /* Set session open CNF sent as FALSE */
    session_ptr->is_session_open_cnf_sent = FALSE;

    /*Store the Session type */
    session_ptr->session_type = session_type;
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
  return session_ptr;
}/* mmgsdi_util_generate_and_add_session_to_client */


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
)
{
  mmgsdi_refresh_node_type            *node_ptr        = NULL;
  mmgsdi_card_refresh_extra_info_type  extra_info;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  uint8                                slot_index      = MMGSDI_SLOT_1_INDEX;

  memset(&extra_info, 0x00, sizeof(mmgsdi_card_refresh_extra_info_type));

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
  if((mmgsdi_refresh_info_ptr[slot_index] != NULL) &&
     (mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id == slot_id))
  {
    node_ptr = mmgsdi_refresh_info_ptr[slot_index]->node_ptr;
    while(node_ptr)
    {
      if(node_ptr->session_id == session_id)
      {
        /* Send out REFRESH END_FAILED event to all clients */
        extra_info.mode      = mmgsdi_refresh_info_ptr[slot_index]->mode;
        extra_info.send_evt  = TRUE;
        extra_info.stage     = MMGSDI_REFRESH_STAGE_END_FAILED;
        mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;

        (void)mmgsdi_refresh_send_evt(&extra_info,
                                      &mmgsdi_refresh_info_ptr[slot_index]->request_header);

        /* send confirmation to GSTK */
        mmgsdi_status = mmgsdi_refresh_rsp_send_cnf_to_gstk(
                          &mmgsdi_refresh_info_ptr[slot_index]->request_header);
        break;
      }
      node_ptr = node_ptr->next_ptr;
    }
  }
  return mmgsdi_status;
}/* mmgsdi_util_stop_refresh */


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_CHANNEL_SHARED_WITH_PROV_SESSION

   DESCRIPTION:
     This function finds that any provisioning session is opened on requested
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
)
{
  boolean                   return_value      = FALSE;
  int32                     session_cnt       = 0;
  int32                     num_sessions     = 0;
  mmgsdi_session_info_type *session_info_ptr = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  /*Check client id reg table at index 0; at index 0 it has mmgsdi client which
    opens all provisioning sessions*/
  if(mmgsdi_client_id_reg_table[0] == NULL)
  {
    return return_value;
  }

  num_sessions = mmgsdi_client_id_reg_table[0]->num_session;

  for(session_cnt = 0; (num_sessions > 0) && (session_cnt < MMGSDI_MAX_SESSION_INFO); session_cnt++)
  {
    session_info_ptr =
      mmgsdi_client_id_reg_table[0]->session_info_ptr[session_cnt];

    if(session_info_ptr == NULL)
    {
      /*Invalid Session, Continue to next*/
      continue;
    }
    num_sessions--;

    session_info_ptr =
      mmgsdi_client_id_reg_table[0]->session_info_ptr[session_cnt];

    if((session_info_ptr != NULL) &&
       (mmgsdi_util_is_prov_session(session_info_ptr->session_type)) &&
       (session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id == channel_id) &&
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == slot_id))
    {
      return_value = TRUE;
      UIM_MSG_HIGH_2("Channel id 0x%x is shared with prov session type 0x%x",
                     channel_id, session_info_ptr->session_type);
      break;
    }
  }
  return return_value;
}/* mmgsdi_util_is_channel_shared_with_prov_session */


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
)
{
  mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type   *sel_app_ptr   = NULL;
  mmgsdi_file_enum_type       file_enum     = MMGSDI_NO_FILE_ENUM;

  if ((file_path.path_len <= 1) ||
      (file_path.path_len > MMGSDI_MAX_PATH_LEN))
  {
    return file_enum;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_id,
                        NULL,NULL,
                        &sel_app_ptr,NULL,NULL,NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (sel_app_ptr == NULL))
  {
    UIM_MSG_ERR_1("Invalid session id 0x%x", session_id);
    return file_enum;
  }

  mmgsdi_status = mmgsdi_file_get_file_from_path(sel_app_ptr->app_data.app_type,
                                                 file_path,
                                                 &file_enum);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("uim enum not found");
    return file_enum;
  }

  return file_enum;
} /* mmgsdi_util_convert_path_to_enum */


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
boolean mmgsdi_util_is_adf_shareable
(
  const mmgsdi_data_type   * aid_ptr,
  mmgsdi_slot_id_enum_type   slot_id
)
{
  uint8                      i = 0;
  mmgsdi_int_app_info_type **app_info_tbl_pptr;

  MMGSDIUTIL_RETURN_IF_NULL(aid_ptr);

  app_info_tbl_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(slot_id);

  /* Check for valid app_info_tbl_pptr */
  if(app_info_tbl_pptr == NULL)
  {
    UIM_MSG_ERR_0("app_info_tbl_pptr is NULL");
    return FALSE;
  }

  for(i = 0; i < MMGSDI_MAX_APP_INFO; i++)
  {
    if(app_info_tbl_pptr[i] == NULL)
    {
      continue;
    }
    if(app_info_tbl_pptr[i]->app_selected &&
       app_info_tbl_pptr[i]->app_data.aid.data_len == aid_ptr->data_len &&
       app_info_tbl_pptr[i]->adf_attributes.info_valid &&
       (memcmp(app_info_tbl_pptr[i]->app_data.aid.data_ptr,
               aid_ptr->data_ptr,
               aid_ptr->data_len) == 0))
    {
      UIM_MSG_HIGH_1("adf_shareable: 0x%x",
                 app_info_tbl_pptr[i]->adf_attributes.adf_shareable);
      return app_info_tbl_pptr[i]->adf_attributes.adf_shareable;
    }
  }

  /* If ADF attributes are not available, assume that the ADF is shareable
     by default so that MMGSDI sends MANAGE CHANNEL command to UIM */
  return TRUE;
} /* mmgsdi_util_is_adf_shareable */


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
  mmgsdi_slot_id_enum_type slot_id
)
{
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return;
  }

  slot_data_ptr->gstk_loc_env_rsp_count = 0;
} /* mmgsdi_util_reset_gstk_loc_env_rsp_count */


/*===========================================================================
FUNCTION MMGSDI_UTIL_RESERVE_LOGICAL_CHANNEL_FOR_CSIM_APP

DESCRIPTION
  This function determines if a logical channel needs to be reserved
  for a CSIM app. It is TRUE, if all of the below are TRUE
  1. Both the USIM and CSIM apps are present on the card
  2(i).  If the UICC has only one CSIM app, the CSIM app is not selected yet
         (that is, it is in DETECTED state).
   (ii). Or, if the UICC has multiple CSIM apps, NONE of the CSIM apps is
         selected yet (that is, all the CSIM apps are in DETECTED state)

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_util_reserve_logical_channel_for_csim_app(
  mmgsdi_slot_id_enum_type slot_id
)
{
  uint8                      index              = 0;
  mmgsdi_slot_data_type     *slot_data_ptr      = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return FALSE;
  }

  /* If device supports only one slot and NV 73732 is enabled for primary
     subscription, don't reserve a channel */
  if(mmgsdi_generic_data_ptr->num_slots == 1 &&
     (mmgsdi_nv_get_external_feature_status(
        MMGSDI_EXT_FEAT_CDMA_LESS_DEVICE, MMGSDI_NV_CONTEXT_PRIMARY) == MMGSDI_FEATURE_ENABLED))
  {
    return FALSE;
  }
  
  /* If device supports two slots and NV 73732 is enabled for both primary
     and secondary subscription, don't reserve a channel */
  if(mmgsdi_generic_data_ptr->num_slots == 2 &&
     (mmgsdi_nv_get_external_feature_status(
        MMGSDI_EXT_FEAT_CDMA_LESS_DEVICE, MMGSDI_NV_CONTEXT_PRIMARY) == MMGSDI_FEATURE_ENABLED) &&
     (mmgsdi_nv_get_external_feature_status(
        MMGSDI_EXT_FEAT_CDMA_LESS_DEVICE, MMGSDI_NV_CONTEXT_SECONDARY) == MMGSDI_FEATURE_ENABLED))
  {
    return FALSE;
  }

  /* Check if both USIM and CSIM apps are present */
  if((mmgsdi_util_get_usim_apps(slot_id)) &&
     (mmgsdi_util_get_csim_apps(slot_id)))
  {
    /* Check the state of CSIM apps. If there is one CSIM app on the UICC and
       the app is NOT in DETECTED state, do not reserve logical channel.
       Incase the UICC has multiple CSIM apps, if even one CSIM app is found
       that is NOT in DETECTED state, that is if you find out even one CSIM app
       that is in ACTIVATED, PERSO_READY, READY or any other states, no need
       to reserve the logical channel. If in future we add a new state in
       between DETECTED and ACTIVATED, this logic will need to be revisited. */
    for(index = 0; index < MMGSDI_MAX_APP_INFO; index++)
    {
      if(slot_data_ptr->app_info_pptr[index] == NULL)
      {
        continue;
      }
      if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_CSIM &&
         slot_data_ptr->app_info_pptr[index]->app_state > MMGSDI_APP_STATE_DETECTED)
      {
        return FALSE;
      }
    }
    UIM_MSG_HIGH_0("Logical channel reserved for CSIM App");
    return TRUE;
  }
  return FALSE;
} /* mmgsdi_util_reserve_logical_channel_for_csim_app */


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
)
{
  uint16                i                     = 0;
  uint16                mcc_value             = 0;
  /* In addition to North American networks, there are other PLMNs that
     also use MNC with 3 digits. */
  uint16                mnc_3digit_mcc_list[] = {302, 310, 311, 312, 313, 314,
                                                 315, 316, 334, 348, 405, 344,
                                                 365, 722, 342, 350, 346, 732,
                                                 366, 750, 352, 358, 354, 708,
                                                 338, 356, 360, 374, 376};
  mmgsdi_imsi_data_type imsi                  = {{0}};

  MMGSDIUTIL_RETURN_IF_NULL_2(imsi_data.data_ptr, num_mnc_digits_ptr);

  /* Old SIM cards had only 2 digits MNC and EF-AD with 3 bytes only.
     Default to 2 digits to keep backward compatibility */
  *num_mnc_digits_ptr = MMGSDIUTIL_MNC_2_DIGITS;

  if(ad_data.data_ptr == NULL)
  {
    UIM_MSG_HIGH_0("EF-AD ptr is NULL, use default 2 digit MNC" );
    return MMGSDI_SUCCESS;
  }

  /* 3GPP 23.122 Annex 'A'
     For PCS1900 for North America, regulations mandate that a 3-digit MNC
     shall be used; however during a transition period, a 2 digit MNC may
     be broadcast by the Network and, in this case, the 3rd digit of the
     SIM is stored as 0 (this is the 0 suffix rule).
     For these specific country codes, MNC length shall default to 3. */
  mcc_value  = (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_1_OFFSET] >> 4) &
                MMGSDIUTIL_IMSI_LOW_DIGIT_MASK) * 100;
  mcc_value += (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_2_OFFSET]) &
                MMGSDIUTIL_IMSI_LOW_DIGIT_MASK) * 10;
  mcc_value += (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_2_OFFSET] >> 4) &
                MMGSDIUTIL_IMSI_LOW_DIGIT_MASK);
  for (i = 0; i < sizeof(mnc_3digit_mcc_list) / sizeof(uint16); i++)
  {
    if (mcc_value == mnc_3digit_mcc_list[i])
    {
      *num_mnc_digits_ptr = MMGSDIUTIL_MNC_3_DIGITS;
      break;
    }
  }

  /* GCF test scripts expect a 3-digit MNC... EF-AD in 2G test specs is 3 bytes
     long, so for the 2G GCF testing, we will always end up here. That is, we
     will always end up setting/faking the 4th byte of EF-AD that is returned
     to the client. Since GCF test scripts expect "3" as MNC length in the EF-AD
     for both 2G and 3G tests, we should make sure to create/set the byte4 of
     EF-AD as "3" always for the test IMSIs.
     NOTE that if the EF-AD on the card already is atleast 4 bytes long, the
     MNC-length indicated in byte4 of EF-AD is the one that is used, as seen
     below */
  if (MMGSDI_SUCCESS == mmgsdi_util_convert_to_imsi_data_type(&imsi_data, &imsi) &&
      mmgsdi_card_init_is_imsi_test_imsi(imsi))
  {
    *num_mnc_digits_ptr = MMGSDIUTIL_MNC_3_DIGITS;
  }

  if (ad_data.data_len > MMGSDIUTIL_MNC_LEN_BYTE && ad_data.data_ptr != NULL)
  {
    if ( ((ad_data.data_ptr[MMGSDIUTIL_MNC_LEN_BYTE] & 0x0F) == MMGSDIUTIL_MNC_2_DIGITS ) ||
         ((ad_data.data_ptr[MMGSDIUTIL_MNC_LEN_BYTE] & 0x0F) == MMGSDIUTIL_MNC_3_DIGITS ) )
    {
      *num_mnc_digits_ptr = ad_data.data_ptr[MMGSDIUTIL_MNC_LEN_BYTE] & 0x0F;
    }
    else
    {
      UIM_MSG_ERR_1("EF-AD has invalid MNC length: 0x%x",
                    ad_data.data_ptr[MMGSDIUTIL_MNC_LEN_BYTE]);
    }
  }

  UIM_MSG_HIGH_1("Number of MNC digits: 0x%x", *num_mnc_digits_ptr );
  return MMGSDI_SUCCESS;
}/* mmgsdi_util_get_num_of_mnc_digits */


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
)
{
  rex_tcb_type *  mmgsdi_current_task = NULL;

#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
  /* In case of SGLTE if the call is occuring in GSDI context but as a result
     of a request that came over QMI from QSC the function returns FALSE to
     mimic behavior of a true client access and to also avoid a deadlock with
     UIM driver in situations such as recovery */
  if (qmi_sim_internal_active_mmgsdi_sync_call())
  {
    return MMGSDI_TASK_MAX;
  }
#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE */

  mmgsdi_current_task = rex_self();

  /* If MAIN TASK is processing qmi uim sig, it is required to consider it
     as non mmgsdi task so that card access can be prevented from QMI UIM */
  if(UIM_GSDI_TCB == mmgsdi_current_task &&
     !mmgsdi_is_qmi_uim_processing(mmgsdi_current_task))
  {
    return MMGSDI_TASK_MAIN;
  }
  else if(UIM_GSDI_SLOT1_TCB == mmgsdi_current_task)
  {
    return MMGSDI_TASK_SLOT1;
  }
  else if(UIM_GSDI_SLOT2_TCB == mmgsdi_current_task)
  {
    return MMGSDI_TASK_SLOT2;
  }
  else if(UIM_GSDI_SLOT3_TCB == mmgsdi_current_task)
  {
    return MMGSDI_TASK_SLOT3;
  }

  return MMGSDI_TASK_MAX;
} /* mmgsdi_util_get_current_task */


/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_TASK_FROM_SLOT

DESCRIPTION
  This function returns the MMGSDI task corresponding to a slot

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_task_enum_type

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_task_enum_type mmgsdi_util_get_task_from_slot(
  mmgsdi_slot_id_enum_type slot_id
)
{
  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      return MMGSDI_TASK_SLOT1;

    case MMGSDI_SLOT_2:
      return MMGSDI_TASK_SLOT2;

    case MMGSDI_SLOT_3:
      return MMGSDI_TASK_SLOT3;

     default:
       break;
  }

  return MMGSDI_TASK_MAX;
} /* mmgsdi_util_get_task_from_slot */


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
)
{
  switch(mmgsdi_task)
  {
    case MMGSDI_TASK_SLOT1:
      return MMGSDI_SLOT_1;

    case MMGSDI_TASK_SLOT2:
      return MMGSDI_SLOT_2;

    case MMGSDI_TASK_SLOT3:
      return MMGSDI_SLOT_3;

     default:
       break;
  }
  return MMGSDI_MAX_SLOT_ID_ENUM;
} /* mmgsdi_util_get_slot_from_task */


/*===========================================================================
  FUNCTION MMGSDI_UTIL_GET_SLOT_FOR_CMD

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
)
{
  mmgsdi_session_id_status_enum_type session_id_status = MMGSDI_SESSION_ID_UNASSIGNED;
  mmgsdi_session_type_enum_type      session_type      = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_SUCCESS;

  if((task_cmd_ptr == NULL) ||
     (slot_id_ptr  == NULL))
  {
    UIM_MSG_ERR_0("mmgsdi_util_get_slot_for_cmd, input parameters error");
    return MMGSDI_ERROR;
  }

  switch(task_cmd_ptr->cmd.cmd_enum)
  {
    /* Commands to be handled in the main task */
    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
    case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
    case MMGSDI_SESSION_OPEN_EXT_REQ:
    case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
    case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
    case MMGSDI_EONS_SS_EVENT_REQ:
    case MMGSDI_FULL_SERVICE_REQ:
    case MMGSDI_SESSION_GET_INFO_REQ:
    /* sim busy sent to main, ensures processing even if slot task is stuck */
    case MMGSDI_NOTIFY_SIM_BUSY_REQ:
    case MMGSDI_HANDLE_NV_REFRESH_REQ:
    case MMGSDI_GET_SIM_PROFILE_REQ:
      *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
      return MMGSDI_SUCCESS;

    /* Session specific commands, to be redirected to slot specific tasks */
    case MMGSDI_READ_REQ:
    case MMGSDI_WRITE_REQ:
    case MMGSDI_INCREASE_REQ:
    case MMGSDI_GET_FILE_ATTR_REQ:
    case MMGSDI_SESSION_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_ISIM_AUTH_REQ:
    case MMGSDI_REHABILITATE_REQ:
    case MMGSDI_INVALIDATE_REQ:
    case MMGSDI_PIN_OPERATION_REQ:
    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
    case MMGSDI_SRV_AVAILABLE_REQ:
    case MMGSDI_USIM_AUTH_REQ:
    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
    case MMGSDI_COMPUTE_IP_AUTH_REQ:
    case MMGSDI_RUN_CAVE_REQ:
    case MMGSDI_SESSION_CDMA_AKA_REQ:
    case MMGSDI_GENERATE_KEY_VPM_REQ:
    case MMGSDI_BCAST_REQ:
    case MMGSDI_SEARCH_REQ:
    case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
    case MMGSDI_SESSION_STORE_ESN_REQ:
    case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
    case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
    case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
    case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
    case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ:
    case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
    case MMGSDI_SESSION_SSD_UPDATE_REQ:
    case MMGSDI_SESSION_SSD_CONFIRM_REQ:
    case MMGSDI_SESSION_BS_CHAL_REQ:
    case MMGSDI_SESSION_OTASP_MS_KEY_REQ:
    case MMGSDI_SESSION_OTASP_KEY_GEN_REQ:
    case MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ:
    case MMGSDI_SESSION_OTASP_COMMIT_REQ:
    case MMGSDI_SESSION_OTASP_OTAPA_REQ:
    case MMGSDI_SESSION_OTASP_BLOCK_REQ:
    case MMGSDI_SESSION_READ_PRL_REQ:
    case MMGSDI_CARD_STATUS_REQ:
    case MMGSDI_SESSION_SUB_OK_REQ:
    case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
    case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
      mmgsdi_status = mmgsdi_util_get_session_app_info(
                        task_cmd_ptr->cmd.cmd.read_req.request_header.session_id,
                        slot_id_ptr,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_REFRESH_REQ:
      if (task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type     ==
          MMGSDI_REFRESH_ORIG_REFRESH_REQ                             ||
          task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type     ==
          MMGSDI_REFRESH_ORIG_PUP_REQ                                 ||
          task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type     ==
          MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ                        ||
          task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type     ==
          MMGSDI_REFRESH_ORIG_TIMER_EXPIRY_REQ)
      {
        *slot_id_ptr = task_cmd_ptr->cmd.cmd.request_header.slot_id;
      }
      else if (task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type ==
          MMGSDI_REFRESH_ORIG_REG_REQ                                 ||
          task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type      ==
          MMGSDI_REFRESH_ORIG_DEREG_REQ                               ||
          task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type      ==
          MMGSDI_REFRESH_ORIG_REG_ALL_REQ                             ||
          task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type      ==
          MMGSDI_REFRESH_ORIG_DEREG_ALL_REQ)
      {
        *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
      }
      else
      {
        session_id_status = mmgsdi_util_is_session_id_valid(
                                      task_cmd_ptr->cmd.cmd.request_header.session_id);
        if(session_id_status != MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
        {
          *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
          return MMGSDI_SUCCESS;
        }
        mmgsdi_status = mmgsdi_util_get_session_app_info(
                          task_cmd_ptr->cmd.cmd.request_header.session_id,
                          slot_id_ptr,
                          NULL,
                          NULL,
                          NULL,
                          NULL,
                          NULL);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
        }
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
      session_id_status = mmgsdi_util_is_session_id_valid(
                                    task_cmd_ptr->cmd.cmd.read_req.request_header.session_id);
      if(session_id_status == MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
      {
        mmgsdi_status = mmgsdi_util_get_session_app_info(
                          task_cmd_ptr->cmd.cmd.read_req.request_header.session_id,
                          slot_id_ptr,
                          NULL,
                          NULL,
                          NULL,
                          NULL,
                          NULL);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
        }
      }
      else
      {
        *slot_id_ptr = task_cmd_ptr->cmd.cmd.read_req.request_header.slot_id;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_SESSION_OPEN_REQ:
      if(task_cmd_ptr->cmd.cmd.session_open_req.session_type == MMGSDI_NON_PROV_SESSION_SLOT_1)
      {
        *slot_id_ptr = MMGSDI_SLOT_1;
      }
      else if(task_cmd_ptr->cmd.cmd.session_open_req.session_type == MMGSDI_NON_PROV_SESSION_SLOT_2)
      {
        *slot_id_ptr = MMGSDI_SLOT_2;
      }
      else if(task_cmd_ptr->cmd.cmd.session_open_req.session_type == MMGSDI_NON_PROV_SESSION_SLOT_3)
      {
        *slot_id_ptr = MMGSDI_SLOT_3;
      }
      else
      {
        *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
      }
      return MMGSDI_SUCCESS;

    case MMGSDI_SESSION_CLOSE_REQ:
      session_id_status = mmgsdi_util_is_session_id_valid(
                                    task_cmd_ptr->cmd.cmd.read_req.request_header.session_id);
      if(session_id_status != MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
      {
        *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
        return MMGSDI_SUCCESS;
      }
      (void)mmgsdi_util_get_session_type(task_cmd_ptr->cmd.cmd.read_req.request_header.session_id,
                                         &session_type, NULL);
      if(mmgsdi_util_is_non_prov_session(session_type))
      {
        mmgsdi_status = mmgsdi_util_get_session_app_info(
                          task_cmd_ptr->cmd.cmd.read_req.request_header.session_id,
                          slot_id_ptr,
                          NULL,
                          NULL,
                          NULL,
                          NULL,
                          NULL);
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
        }
      }
      else
      {
        *slot_id_ptr = MMGSDI_SLOT_AUTOMATIC;
      }
      return MMGSDI_SUCCESS;

    /* Slot specific commands, to be redirected to slot specific tasks */
    case MMGSDI_INTERNAL_PUP_INIT_REQ:
    case MMGSDI_CARD_PUP_REQ:
    case MMGSDI_CARD_PDOWN_REQ:
    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
    case MMGSDI_NOTIFY_LINK_EST_REQ:
    case MMGSDI_NOTIFY_CARD_ERROR_REQ:
    case MMGSDI_GET_ATR_REQ:
    case MMGSDI_SEND_APDU_REQ:
    case MMGSDI_SEND_APDU_EXT_REQ:
    case MMGSDI_SAP_CONNECT_REQ:
    case MMGSDI_SAP_DISCONNECT_REQ:
    case MMGSDI_SAP_POWER_ON_REQ:
    case MMGSDI_SAP_POWER_OFF_REQ:
    case MMGSDI_SAP_RESET_REQ:
    case MMGSDI_SAP_GET_ATR_REQ:
    case MMGSDI_SAP_SEND_APDU_REQ:
    case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
    case MMGSDI_RECOVERY_IND_REQ:
    case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
    case MMGSDI_CARD_RESET_REQ:
    case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
    case MMGSDI_MANAGE_CHANNEL_REQ:
    case MMGSDI_SET_SIM_PROFILE_REQ:
    case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
      *slot_id_ptr = task_cmd_ptr->cmd.cmd.request_header.slot_id;
      return MMGSDI_SUCCESS;

    /* These commands should not be queued to the MMGSDI command queues */
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_UIM_REPORT_RSP:
    case MMGSDI_SAP_REPORT_RSP:
    case MMGSDI_CNF:
    case MMGSDI_MAX_CMD_ENUM:
      return MMGSDI_ERROR;

    default:
      break;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_slot_for_cmd */


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
)
{

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  switch(mmgsdi_task)
  {
    case MMGSDI_TASK_SLOT1:
      return mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX];

    case MMGSDI_TASK_SLOT2:
      return mmgsdi_data_slot_ptr[MMGSDI_SLOT_2_INDEX];

    case MMGSDI_TASK_SLOT3:
      return mmgsdi_data_slot_ptr[MMGSDI_SLOT_3_INDEX];

    case MMGSDI_TASK_MAIN:
    default:
      UIM_MSG_ERR_1("Unexpected task 0x%x", mmgsdi_task);
      return NULL;
  }
} /* mmgsdi_util_get_slot_data_ptr_for_task*/


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_SET_TP_STATE

   DESCRIPTION:
     This function sets the app tp state and slot tp state.
     Three important globals for this function are:
       1. Slot TP state - This is global TP state for a slot
       2. App TP state  - This is app specific TP state in case there are more than one
                          provisioning app on card and can initiate TP.
       3. TP ref count  - This is passed while sending a TP DL request to GSTK and is
                          retruned by GSTK upon completion of TP DL. The two values,
                          the one with MMGSDI and one returned by GSTK, shall match else
                          we have an out of order TP response.

   Following cases to be considered as of now:
   1. RUIM card with GSM application needing Terminal Profile Download -
      a. GSM application initiates a TP DL request to GSTK as part of post pin1 init
         with ref count set to 1. Various globals are as -
         Slot TP State     - IN PROGRESS OK
         GSM App TP state  - IN PROGRESS OK
         RUIM App TP state - NOT DONE
         TP ref count      - 0x1

         Now we can have two cases:
         Case I - We start RUIM post pin1.
           i. In this case since TP is already in progress
              we do not initiate another TP DL request. Various globals are as -
              Slot TP State     - IN PROGRESS OK
              GSM App TP state  - IN PROGRESS OK
              RUIM App TP state - NOT DONE
              TP ref count      - 0x1
          ii. We receive response for TP from GSTK. Since slot TP state is in IN PROGRESS OK
              we activate both GSM and RUIM application. Various globals are as -
              Slot TP State     - DONE
              GSM App TP state  - DONE
              RUIM App TP state - DONE
              TP ref count      - 0x1

         Case II - We get TP response before we start RUIM post pin1.
           i. In this case we only activate GSM app based on app state. Various globals are as -
              Slot TP State     - DONE
              GSM App TP state  - DONE
              RUIM App TP state - DONE
              TP ref count      - 0x1
          ii. Later when RUIM post pin1 starts we see that TP is already done for this slot and
              straight away proceed with perso procedure with doing TP.Various globals are as -
              Slot TP State     - DONE
              GSM App TP state  - DONE
              RUIM App TP state - DONE
              TP ref count      - 0x1

   2. RUIM card with GSM application not needing Terminal Profile Download -
      a. GSM application initiates a TP DL request to GSTK as part of post pin1 init
         with ref count set to 1. Various globals are as -
         Slot TP State     - IN PROGRESS NOT OK
         GSM App TP state  - IN PROGRESS NOT OK
         RUIM App TP state - NOT DONE
         TP ref count      - 0x1

         Now we can have two cases:
         Case I - We start RUIM post pin1.
           i. In this case since TP is already in progress but with NOT OK state so
              we do initiate another TP DL request. Various globals are as -
              Slot TP State     - IN PROGRESS OK
              GSM App TP state  - IN PROGRESS NOT OK
              RUIM App TP state - IN PROGRESS OK
              TP ref count      - 0x2
          ii. We receive response of TP from GSTK for GSM TP. Since MMGSDIs TP ref count
              value is 0x2 and value returned by GSTK is 0x1, we ignore this TP response.
              Various globals are as -
              Slot TP State     - IN PROGRESS OK
              GSM App TP state  - IN PROGRESS NOT OK
              RUIM App TP state - IN PROGRESS OK
              TP ref count      - 0x2
         iii. Later we receive response of TP from GSTK for RUIM TP. Now MMGSDIs TO ref
              count matches the value returned by GSTK and we activate both the apps.
              Various globals are as -
              Slot TP State     - DONE
              GSM App TP state  - NOT NEEDED
              RUIM App TP state - DONE
              TP ref count      - 0x2

         Case II - We get GSM TPs response before we start RUIM post pin1.
           i. Since slot TP state is IN PROGRESS NOT OK and GSM app TP state is also
              IN PROGRESS NOT OK, we only activate GSM app as we know there will be TP
              request from RUIM for sure. Various globals are as -
              Slot TP State     - NOT DONE
              GSM App TP state  - NOT NEEDED
              RUIM App TP state - NOT DONE
              TP ref count      - 0x1
          ii. Later when RUIM post pin1 starts we see that TP is not done for this slot and
              proceed with TP DL procedure for RUIM app on same slot.Various globals are as -
              Slot TP State     - IN PROGRESS OK
              GSM App TP state  - NOT NEEDED
              RUIM App TP state - IN PROGRESS OK
              TP ref count      - 0x2
         iii. When we receive TP response for request initiated by RUIM we activate only RUIM app
              looking at app state. GSM app will be in APP STATE PERSO VERIFIED.
              Various globals are as -
              Slot TP State     - DONE
              GSM App TP state  - NOT NEEDED
              RUIM App TP state - DONE
              TP ref count      - 0x2

   3. RUIM card with no GSM application -
      a. RUIM application on card requires a TP and initiates a TP DL request to GSTK as
         part of post pin1 init with ref count set to 1. Various globals are as -
         Slot TP State     - IN PROGRESS OK
         GSM App TP state  - NOT DONE
         RUIM App TP state - IN PROGRESS OK
         TP ref count      - 0x1

         i. We receive response for TP from GSTK. Since slot TP state is in IN PROGRESS OK
            we activate RUIM application. We will not have a valid app to any GW session and
            will ignore them for GW activation.
            Various globals are as -
            Slot TP State     - DONE
            GSM App TP state  - NOT DONE
            RUIM App TP state - DONE
            TP ref count      - 0x1

      b. RUIM application on card does not requires a TP. We initiates a TP DL request to GSTK
         with TP needed as FALSE and with ref count set to 1. Various globals are as -
         Slot TP State     - IN PROGRESS NOT OK
         GSM App TP state  - NOT DONE
         RUIM App TP state - IN PROGRESS NOT OK
         TP ref count      - 0x1

         i. We receive response for TP from GSTK. Since slot TP state is in IN PROGRESS NOT OK
            we activate RUIM application. We will not have a valid app to any GW session and
            will ignore them for GW activation.
            Various globals are as -
            Slot TP State     - DONE
            GSM App TP state  - NOT DONE
            RUIM App TP state - NOT NEEDED
            TP ref count      - 0x1

   4. GSM only card -
      a. GSM application on card requires a TP and initiates a TP DL request to GSTK as
         part of post pin1 init with ref count set to 1. Various globals are as -
         Slot TP State     - IN PROGRESS OK
         GSM App TP state  - IN PROGRESS OK
         RUIM App TP state - NOT DONE
         TP ref count      - 0x1

        i. We receive response for TP from GSTK. Since slot TP state is in IN PROGRESS OK
           we activate GSM application. We will not have a valid app to any 1X session and
           will ignore them for 1X activation.
           Various globals are as -
           Slot TP State     - DONE
           GSM App TP state  - NOT DONE
           RUIM App TP state - DONE
           TP ref count      - 0x1

      b. GSM application on card does not requires a TP. We initiates a TP DL request to GSTK
         with TP needed as FALSE and with ref count set to 1. Various globals are as -
         Slot TP State     - IN PROGRESS NOT OK
         GSM App TP state  - IN PROGRESS NOT OK
         RUIM App TP state - NOT DONE
         TP ref count      - 0x1

         i. We receive response for TP from GSTK. Since slot TP state is in IN PROGRESS NOT OK
            we activate GSM application. We will not have a valid app to any 1X session and
            will ignore them for 1X activation.
            Various globals are as -
            Slot TP State     - DONE
            GSM App TP state  - NOT NEEDED
            RUIM App TP state - NOT DONE
            TP ref count      - 0x1

   5. UICC card -
      a. We will not even be here as this function is only for ICC cards as of now.

   NOTE: We will always send a TP DL request to GSTK, whether card supports STK or not.
         This is for GSTK to set its state accordingly.

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
)
{
  /* Please read full description above it is quite complicated */
  UIM_MSG_HIGH_0("mmgsdi_util_set_tp_states");

  MMGSDIUTIL_RETURN_IF_NULL(slot_data_ptr);

  switch(slot_data_ptr->mmgsdi_tp_state)
  {
    case MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK:
      slot_data_ptr->mmgsdi_tp_state = MMGSDI_TP_STATE_NOT_DONE;
      if(app_info_ptr_gw != NULL)
      {
        if(app_info_ptr_gw->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)
        {
          /* TP was initiated by GSM and was not needed. RUIM will again initate a TP request */
          app_info_ptr_gw->app_tp_state  = MMGSDI_TP_STATE_NOT_NEEDED;
        }
      }

      if(app_info_ptr_1x != NULL)
      {
        if(app_info_ptr_1x->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)
        {
          /* TP was initiated by GSM and was not needed. RUIM will again initate a TP request */
          app_info_ptr_1x->app_tp_state  = MMGSDI_TP_STATE_NOT_NEEDED;
        }
      }
      break;

    case MMGSDI_TP_STATE_IN_PROGRESS_OK:
      slot_data_ptr->mmgsdi_tp_state = MMGSDI_TP_STATE_DONE;
      if(app_info_ptr_gw != NULL)
      {
        if(app_info_ptr_gw->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)
        {
          /* TP was also initiated by GSM and was not processed based on ref count.
             This TP request is for RUIM */
          app_info_ptr_gw->app_tp_state  = MMGSDI_TP_STATE_NOT_NEEDED;
        }
        else if(app_info_ptr_gw->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_OK)
        {
          /* All Good */
          app_info_ptr_gw->app_tp_state  = MMGSDI_TP_STATE_DONE;
        }
      }

      if(app_info_ptr_1x != NULL)
      {
        if(app_info_ptr_1x->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)
        {
          /* TP was also initiated by RUIM and was not processed based on ref count.
             This TP request is for GSM */
          app_info_ptr_1x->app_tp_state  = MMGSDI_TP_STATE_NOT_NEEDED;
        }
        else if(app_info_ptr_1x->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_OK)
        {
          app_info_ptr_1x->app_tp_state  = MMGSDI_TP_STATE_DONE;
        }
      }
      break;

    default:
      UIM_MSG_ERR_1("Unexpecetd TP state 0x%x", slot_data_ptr->mmgsdi_tp_state);
      return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_1("Slot tp state after TP RSP processing 0x%x",
             slot_data_ptr->mmgsdi_tp_state);
  if(app_info_ptr_1x != NULL)
  {
    UIM_MSG_HIGH_1("1X tp state after TP RSP processing 0x%x",
               app_info_ptr_1x->app_tp_state);
  }

  if(app_info_ptr_gw != NULL)
  {
    UIM_MSG_HIGH_1("GW tp state after TP RSP processing 0x%x",
               app_info_ptr_gw->app_tp_state);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_set_tp_states */


/*===========================================================================
  FUNCTION MMGSDI_UTIL_IS_UIM_TASK_CONTEXT

DESCRIPTION
  This function checks if the current task is an UIM task

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - If the current task is UIM task
  FALSE  - If otherwise

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_is_uim_task_context(
  void
)
{
#ifndef FEATURE_UIM_USES_TASK_HANDLE
  rex_tcb_type * uim_current_task = NULL;
#endif /* !FEATURE_UIM_USES_TASK_HANDLE */

#ifndef FEATURE_UIM_USES_TASK_HANDLE
  uim_current_task = rex_self();

  if(uim_current_task == &uim_tcb)
  {
    return TRUE;
  }
  else if(uim_current_task == &uim_2_tcb)
  {
    return TRUE;
  }
  else if(uim_current_task == &uim_3_tcb)
  {
    return TRUE;
  }

#else
  if(TASK_HANDLE(uim) == rex_self())
  {
    return TRUE;
  }
  else if(TASK_HANDLE(uim_2) == rex_self())
  {
    return TRUE;
  }
  else if(TASK_HANDLE(uim_3) == rex_self())
  {
    return TRUE;
  }

#endif /* !FEATURE_UIM_USES_TASK_HANDLE */

  return FALSE;
} /* mmgsdi_util_is_uim_task_context */


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
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type session_type  = MMGSDI_MAX_SESSION_TYPE_ENUM;

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_session_id_ptr);

  mmgsdi_status = mmgsdi_util_get_session_type(session_id,
                                               &session_type,
                                               NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Could not map Session ID: 0x%x to a session type",
                  session_id);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_mmgsdi_session_id_from_type(
                    session_type,
                    mmgsdi_session_id_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unable to get Session ID for session type 0x%x",
                  session_type);
    return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_map_to_mmgsdi_session */


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
  mmgsdi_cphs_int_info_type            **cphs_int_info_pptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(cphs_int_info_pptr);

  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *cphs_int_info_pptr = mmgsdi_cphs_info_ptr[MMGSDI_PRI_CPHS_INFO_INDEX];
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      *cphs_int_info_pptr = mmgsdi_cphs_info_ptr[MMGSDI_SEC_CPHS_INFO_INDEX];
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      *cphs_int_info_pptr = mmgsdi_cphs_info_ptr[MMGSDI_TER_CPHS_INFO_INDEX];
      break;
    default:
      UIM_MSG_ERR_1("Invalid session_type 0x%x in retrieving CPHS info ptr",
                    session_type);
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_cphs_info_ptr */


/* ============================================================================
FUNCTION:      MMGSDI_UTIL_CHECK_3GPP2_APP_IN_READY_STATE

DESCRIPTION:
  This function scans through all the apps of the 3GPP2 technology type
  on the slot and if any app is found in READY state, the function mentions to
  the caller that the waiting 3GPP app can continue post pin1 opeations. If
  there is no 3GPP2 app available on slot in auto provisioning scenario or
  no 3GPP2 app is being activated in manual provisioning scenario, caller
  is informed to continue post pin1 opeations of waiting 3GPP app.

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
)
{
  uint8                   index                 = 0;
  uint8                   num_apps_detected     = 0;
  mmgsdi_slot_data_type  *slot_data_ptr         = NULL;
  boolean                 mmgsdi_int_select     = FALSE;
  mmgsdi_nv_item_cache_type mmgsdi_nv_data      = {0};
  nv_stat_enum_type         nv_status           = NV_FAIL_S;

  MMGSDIUTIL_RETURN_IF_NULL(app_in_ready_state_ptr);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(mmgsdi_slot);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  *app_in_ready_state_ptr = FALSE;
  mmgsdi_nv_data.item_value.uim_select_default_usim_app = FALSE;

  /* Find out if MMGSDI needs to do provisioning or not */
  nv_status = mmgsdi_nv_get_item(NV_UIM_SELECT_DEFAULT_USIM_APP_I,
                                 &mmgsdi_nv_data, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  UIM_MSG_MED_2("NV_UIM_SELECT_DEFAULT_USIM_APP_I Read: Status: 0x%x, Select Default App.: 0x%x",
                 nv_status,  mmgsdi_nv_data.item_value.uim_select_default_usim_app);

  if ((nv_status != NV_DONE_S) ||
      ( mmgsdi_nv_data.item_value.uim_select_default_usim_app == TRUE))
  {
    mmgsdi_int_select = TRUE;
  }

  /* Scan through the apps list to find if any CSIMs/RUIMs
     in ILLEGAL or READY state */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index ++)
  {
    if(slot_data_ptr->app_info_pptr[index] == NULL)
    {
      continue;
    }

    if((slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_CSIM) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_RUIM))
    {
      continue;
    }

    if(slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_READY ||
       slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_ILLEGAL)
    {
      *app_in_ready_state_ptr = TRUE;
      return MMGSDI_SUCCESS;
    }
    /* If manual provisioning is enabled and app_state is DETECTED i.e. app
       activation is not triggered by UI, there is no need to wait for CSIM/RUIM
       to reach READY state */
    else if (mmgsdi_int_select == FALSE)
    {
      if(slot_data_ptr->app_info_pptr[index]->app_state > MMGSDI_APP_STATE_DETECTED)
      {
       /* App is some other state */
        num_apps_detected ++;
      }
    }
    else
    {
      /* App is some other state */
      num_apps_detected ++;
    }
  }

  UIM_MSG_HIGH_1("Number of 3GPP2 apps not in READY state: 0x%x",
                 num_apps_detected);

  /* If there is no 3GPP2 app present on card, set app_in_ready_state as TRUE
     to not block 3GPP app intialization */
  if(num_apps_detected == 0)
  {
    *app_in_ready_state_ptr = TRUE;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_util_check_3gpp2_app_in_ready_state */


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

)
{
  mmgsdi_session_id_type session_id[MMGSDI_MAX_EXT_APP_INFO];
  uint8                  num_session_ids        = MMGSDI_MAX_EXT_APP_INFO;
  uint8                  index                  = 0;

  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index ++)
  {
    session_id[index] = MMGSDI_INVALID_SESSION_ID;
  }

  if(mmgsdi_util_check_perso_verified_for_waiting_apps(mmgsdi_slot,
                                                       mmgsdi_3g_type,
                                                       session_id,
                                                       &num_session_ids) != FALSE)
  {
    for(index = 0;
        index < num_session_ids &&
        index < sizeof(session_id)/sizeof(mmgsdi_session_id_type);
        index ++)
    {
      UIM_MSG_HIGH_1("Unblocking App with 0x%x tech post perso procedures",
                     mmgsdi_3g_type );

      /* Trigger post perso procedures */
      (void)mmgsdi_card_init_build_internal_pup_req(session_id[index],
                                                    mmgsdi_slot,
                                                    MMGSDI_PIN1_EVT,
                                                    MMGSDI_STATE_CARD_INSERTED,
                                                    MMGSDI_APP_STATE_PERSO_VERIFIED);
    }
  }
}/* mmgsdi_util_unblock_apps_for_post_perso */


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

  This is controllable via an EFS item/feature flag

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
)
{
  uint8                  index                = 0;
  uint8                  num_apps_detected    = 0;
  mmgsdi_slot_data_type *slot_data_ptr        = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(mmgsdi_slot);

  if(slot_data_ptr == NULL)
  {
    return FALSE;
  }

  /* Scan through the apps list to find if any USIMs/SIMs
     in PERSO_VERIFIED, ILLEGAL or READY state */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index ++)
  {
    if(slot_data_ptr->app_info_pptr[index] == NULL)
    {
      continue;
    }

    if((mmgsdi_3g_type == MMGSDI_TECH_3GPP2) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_CSIM) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_RUIM))
    {
      continue;
    }

    if((mmgsdi_3g_type == MMGSDI_TECH_3GPP) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_USIM) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_SIM))
    {
      continue;
    }

    if(slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_PERSO_VERIFIED ||
       slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_READY ||
       slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_ILLEGAL)
    {
      return TRUE;
    }
    else
    {
      /* App is some other state */
      num_apps_detected ++;
    }
  }

  UIM_MSG_HIGH_1("No 0x%x app in PERSO_VERIFIED state", mmgsdi_3g_type);

  /* In 3GPP case, even if there are no 3GPP apps found, we return TRUE
     In 3GPP2 case, if there are no 3GPP2 apps found, we return FALSE. This
     ensures that cards with no 3GPP2 app support are not activated */
  if((num_apps_detected == 0) &&
     (mmgsdi_3g_type == MMGSDI_TECH_3GPP))
  {
    /* No GW apps found on the card */
    return TRUE;
  }

  return FALSE;
} /* mmgsdi_util_check_perso_verified */


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

  This is controllable via an EFS item/feature flag

PARAMETERS:
  IN:
  mmgsdi_slot:         MMGSDI slot
  mmgsdi_3g_type:      Technology of type, 3GPP or 3GPP2
  OUT:
  session_id_ptr:      Array of session_ids for retreiving session ids corresponding
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
  mmgsdi_slot_id_enum_type     mmgsdi_slot,
  mmgsdi_technology_enum_type  mmgsdi_3g_type,
  mmgsdi_session_id_type      *session_id_ptr,
  uint8                       *num_session_ids_ptr
)
{
  uint8                     index                 = 0;
  mmgsdi_slot_data_type    *slot_data_ptr         = NULL;
  mmgsdi_session_info_type *session_info_ptr      = NULL;
  mmgsdi_return_enum_type   mmgsdi_status         = MMGSDI_ERROR;
  uint8                     num_valid_session_ids = 0;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(mmgsdi_slot);

  if(slot_data_ptr == NULL)
  {
    return FALSE;
  }

  if(session_id_ptr == NULL ||
     num_session_ids_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid parameters");
    return FALSE;
  }

  /* Scan through the apps list to find out if a requested app (3gpp or 3gpp2)
     is in a PERSO_VERIFIED state and is waiting on the other (3gpp2 or 3gpp)
     app state in order to start post perso. */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index ++)
  {
    if(slot_data_ptr->app_info_pptr[index] == NULL)
    {
      continue;
    }

    if((mmgsdi_3g_type == MMGSDI_TECH_3GPP) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_USIM) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_SIM))
    {
      continue;
    }

    if((mmgsdi_3g_type == MMGSDI_TECH_3GPP2) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_CSIM) &&
       (slot_data_ptr->app_info_pptr[index]->app_data.app_type != MMGSDI_APP_RUIM))
    {
      continue;
    }

    if(slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_PERSO_VERIFIED)
    {
      /* Get the session_id for the requested (3gpp or 3gpp2) app */
      mmgsdi_status = mmgsdi_util_get_session_info_from_app(&(slot_data_ptr->app_info_pptr[index]->app_data),
                                                            mmgsdi_slot,
                                                            &session_info_ptr);
      if(mmgsdi_status == MMGSDI_SUCCESS &&
         session_info_ptr != NULL)
      {
        if(mmgsdi_util_is_session_id_valid(session_info_ptr->session_id) ==
                MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE &&
           num_valid_session_ids < *num_session_ids_ptr)
        {
          *(session_id_ptr + num_valid_session_ids) = session_info_ptr->session_id;
          num_valid_session_ids ++;
        }
      }
      session_info_ptr = NULL;
    }
  }

  if(num_valid_session_ids > 0)
  {
    /* Found atleast one requested app in perso_verified state */
    *num_session_ids_ptr = num_valid_session_ids;
    return TRUE;
  }

  return FALSE;
} /* mmgsdi_util_check_perso_verified_for_waiting_apps */


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
  mmgsdi_session_id_type                 session_id,
  mmgsdi_session_closed_cause_enum_type  session_closed_cause)
{
  mmgsdi_return_enum_type                 mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type              * app_info_ptr  = NULL;

  /* Get app info of the application */
  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id, NULL, NULL,
                                                   &app_info_ptr, NULL,
                                                   NULL, NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
  {
    UIM_MSG_ERR_1("Could not retrieve app info for the session_id=0x%x",
                  session_id);
    return;
  }

  app_info_ptr->session_closed_cause = session_closed_cause;
} /* mmgsdi_util_update_session_closed_cause */


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
)
{
  int32                       client_index        = 0;
  int32                       session_index       = 0;
  int32                       num_sessions        = 0;
  mmgsdi_session_info_type  * session_info_ptr    = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  for (client_index = 0; client_index < MMGSDI_MAX_CLIENT_INFO; client_index++)
  {
    if (mmgsdi_client_id_reg_table[client_index] == NULL)
    {
      continue;
    }

    num_sessions = mmgsdi_client_id_reg_table[client_index]->num_session;

    /* loop until either
       1) we reach the max sessions supported or
       2) we already updated all the open sessions */
    for (session_index = 0;
         ((num_sessions > 0) &&
         (session_index < MMGSDI_MAX_SESSION_INFO));
         session_index++)
    {
      session_info_ptr =
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];
      if (session_info_ptr == NULL)
      {
        continue;
      }

      num_sessions--;

      if ((session_info_ptr != NULL) &&
          (session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == slot_id) &&
          (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL))
      {
        /* Update the cause */
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->session_closed_cause
          = cause;
      }
    }
  }
} /* mmgsdi_util_update_session_closed_cause_for_slot */


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
  MMGSDI_ERROR: If slot data is NULL for all cards

SIDE EFFECTS
  None
============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_all_available_apps(
  mmgsdi_available_apps_type *available_apps_ptr
)
{
  mmgsdi_return_enum_type   mmgsdi_status     = MMGSDI_NOT_INIT;
  mmgsdi_slot_data_type    *slot1_data_ptr    = NULL;
  mmgsdi_slot_data_type    *slot2_data_ptr    = NULL;
  mmgsdi_slot_data_type    *slot3_data_ptr    = NULL;
  mmgsdi_slot_data_type    *slot_data_ptr     = NULL;
  mmgsdi_slot_id_enum_type  slot_id           = MMGSDI_SLOT_1;
  mmgsdi_app_info_type     *card_app_info_ptr = NULL;
  int32                    *card_num_apps_ptr = NULL;
  uint32                    app_count         = 0;
  uint32                    prov_app_count    = 0;
  uint32                    card_app_count    = 0;

  MMGSDIUTIL_RETURN_IF_NULL(available_apps_ptr);
  memset(available_apps_ptr, 0x00, sizeof(mmgsdi_available_apps_type));

  slot1_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);
  slot2_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_2);
  slot3_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_3);

  if((slot1_data_ptr == NULL) &&
     (slot2_data_ptr == NULL) &&
     (slot3_data_ptr == NULL))
  {
    UIM_MSG_ERR_0("Either too early in init or error in both slots");
    return MMGSDI_ERROR;
  }

  if ((slot1_data_ptr != NULL) &&
      (slot1_data_ptr->mmgsdi_state == MMGSDI_STATE_CARD_INSERTED))
  {
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else if ((slot2_data_ptr != NULL) &&
           (slot2_data_ptr->mmgsdi_state == MMGSDI_STATE_CARD_INSERTED))
  {
    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else if ((slot3_data_ptr != NULL) &&
           (slot3_data_ptr->mmgsdi_state == MMGSDI_STATE_CARD_INSERTED))
  {
    mmgsdi_status = MMGSDI_SUCCESS;
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Neither slot has a card that is inserted/detected");
    return mmgsdi_status;
  }

  /* Initialize number of apps to 0 */
  available_apps_ptr->card1_num_app = 0;
  available_apps_ptr->card2_num_app = 0;
  available_apps_ptr->card3_num_app = 0;

  /* Extract all Slots App info */
  for(slot_id = MMGSDI_SLOT_1; slot_id < MMGSDI_SLOT_AUTOMATIC; slot_id++)
  {
    card_app_info_ptr = NULL;
    card_num_apps_ptr = NULL;

    switch(slot_id)
    {
      case MMGSDI_SLOT_1:
        slot_data_ptr = mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX];
        card_app_info_ptr = available_apps_ptr->card1_app_info;
        card_num_apps_ptr = &(available_apps_ptr->card1_num_app);
        break;
      case MMGSDI_SLOT_2:
        slot_data_ptr = mmgsdi_data_slot_ptr[MMGSDI_SLOT_2_INDEX];
        card_app_info_ptr = available_apps_ptr->card2_app_info;
        card_num_apps_ptr = &(available_apps_ptr->card2_num_app);
        break;
      case MMGSDI_SLOT_3:
        slot_data_ptr = mmgsdi_data_slot_ptr[MMGSDI_SLOT_3_INDEX];
        card_app_info_ptr = available_apps_ptr->card3_app_info;
        card_num_apps_ptr = &(available_apps_ptr->card3_num_app);
        break;
      default:
        break;
    }

    if ((slot_data_ptr != NULL) &&
        (slot_data_ptr->app_info_pptr != NULL) &&
        (slot_data_ptr->mmgsdi_state == MMGSDI_STATE_CARD_INSERTED) &&
        (card_app_info_ptr != NULL) &&
        (card_num_apps_ptr != NULL))
    {
      card_app_count = 0;
      for (app_count = 0; app_count < MMGSDI_MAX_APP_INFO; app_count++)
      {
        if((slot_data_ptr->app_info_pptr[app_count] == NULL) ||
           (slot_data_ptr->app_info_pptr[app_count]->app_data.app_type == MMGSDI_APP_NONE) ||
           (slot_data_ptr->app_info_pptr[app_count]->app_not_in_efdir))
        {
          continue;
        }

        /* Increase the App count for Slot */
        (*card_num_apps_ptr)++;
        card_app_info_ptr[card_app_count].cached = FALSE;
        card_app_info_ptr[card_app_count].slot = slot_id;

        if(slot_data_ptr->app_info_pptr[app_count]->pin1_ptr != NULL &&
           slot_data_ptr->app_info_pptr[app_count]->pin1_ptr->cached)
        {
          /* PIN 1 INFO */
          card_app_info_ptr[card_app_count].pin1.pin_id =
                 slot_data_ptr->app_info_pptr[app_count]->pin1_ptr->pin_id;
          card_app_info_ptr[card_app_count].pin1.status =
                 slot_data_ptr->app_info_pptr[app_count]->pin1_ptr->status;
          card_app_info_ptr[card_app_count].pin1.pin_replacement =
                 slot_data_ptr->app_info_pptr[app_count]->pin1_ptr->pin_replacement;
          card_app_info_ptr[card_app_count].pin1.num_retries =
                 slot_data_ptr->app_info_pptr[app_count]->pin1_ptr->num_retries;
          card_app_info_ptr[card_app_count].pin1.num_unblock_retries =
                 slot_data_ptr->app_info_pptr[app_count]->pin1_ptr->num_unblock_retries;
          card_app_info_ptr[card_app_count].pin1.valid_num_retries =
                 slot_data_ptr->app_info_pptr[app_count]->pin1_ptr->valid_num_retries;
          card_app_info_ptr[card_app_count].pin1.valid_num_unblock_retries =
                 slot_data_ptr->app_info_pptr[app_count]->pin1_ptr->valid_num_unblock_retries;
        }
        else
        {
          card_app_info_ptr[card_app_count].pin1.pin_id = MMGSDI_MAX_PIN_ENUM;
        }

        /* PIN 2 INFO */
        if(slot_data_ptr->app_info_pptr[app_count]->pin2.cached)
        {
          card_app_info_ptr[card_app_count].pin2.pin_id =
                slot_data_ptr->app_info_pptr[app_count]->pin2.pin_id;
          card_app_info_ptr[card_app_count].pin2.status =
                slot_data_ptr->app_info_pptr[app_count]->pin2.status;
          card_app_info_ptr[card_app_count].pin2.pin_replacement =
                slot_data_ptr->app_info_pptr[app_count]->pin2.pin_replacement;
          card_app_info_ptr[card_app_count].pin2.num_retries =
                slot_data_ptr->app_info_pptr[app_count]->pin2.num_retries;
          card_app_info_ptr[card_app_count].pin2.num_unblock_retries =
                slot_data_ptr->app_info_pptr[app_count]->pin2.num_unblock_retries;
          card_app_info_ptr[card_app_count].pin2.valid_num_retries =
                slot_data_ptr->app_info_pptr[app_count]->pin2.valid_num_retries;
          card_app_info_ptr[card_app_count].pin2.valid_num_unblock_retries =
                slot_data_ptr->app_info_pptr[app_count]->pin2.valid_num_unblock_retries;
        }
        else
        {
          card_app_info_ptr[card_app_count].pin2.pin_id = MMGSDI_MAX_PIN_ENUM;
        }

        if(slot_data_ptr->app_info_pptr[app_count]->universal_pin_ptr != NULL &&
           slot_data_ptr->app_info_pptr[app_count]->universal_pin_ptr->cached)
        {
          /* Universal PIN INFO */
          card_app_info_ptr[card_app_count].universal_pin.pin_id =
                slot_data_ptr->app_info_pptr[app_count]->universal_pin_ptr->pin_id;
          card_app_info_ptr[card_app_count].universal_pin.status =
                slot_data_ptr->app_info_pptr[app_count]->universal_pin_ptr->status;
          card_app_info_ptr[card_app_count].universal_pin.pin_replacement =
                slot_data_ptr->app_info_pptr[app_count]->universal_pin_ptr->pin_replacement;
          card_app_info_ptr[card_app_count].universal_pin.num_retries =
                slot_data_ptr->app_info_pptr[app_count]->universal_pin_ptr->num_retries;
          card_app_info_ptr[card_app_count].universal_pin.num_unblock_retries =
                slot_data_ptr->app_info_pptr[app_count]->universal_pin_ptr->num_unblock_retries;
          card_app_info_ptr[card_app_count].universal_pin.valid_num_retries =
                slot_data_ptr->app_info_pptr[app_count]->universal_pin_ptr->valid_num_retries;
          card_app_info_ptr[card_app_count].universal_pin.valid_num_unblock_retries =
                slot_data_ptr->app_info_pptr[app_count]->universal_pin_ptr->valid_num_unblock_retries;
        }
        else
        {
          card_app_info_ptr[card_app_count].universal_pin.pin_id = MMGSDI_MAX_PIN_ENUM;
        }

        /* HIDDENKEY INFO */
        if(slot_data_ptr->app_info_pptr[app_count]->hiddenkey.status !=
             MMGSDI_PIN_STATUS_NOT_INITIALIZED)
        {
          card_app_info_ptr[card_app_count].hiddenkey.pin_id =
                slot_data_ptr->app_info_pptr[app_count]->hiddenkey.pin_id;
          card_app_info_ptr[card_app_count].hiddenkey.status =
                slot_data_ptr->app_info_pptr[app_count]->hiddenkey.status;
          card_app_info_ptr[card_app_count].hiddenkey.pin_replacement =
                slot_data_ptr->app_info_pptr[app_count]->hiddenkey.pin_replacement;
          card_app_info_ptr[card_app_count].hiddenkey.num_retries =
                slot_data_ptr->app_info_pptr[app_count]->hiddenkey.num_retries;
          card_app_info_ptr[card_app_count].hiddenkey.num_unblock_retries =
                slot_data_ptr->app_info_pptr[app_count]->hiddenkey.num_unblock_retries;
          card_app_info_ptr[card_app_count].hiddenkey.valid_num_retries =
                slot_data_ptr->app_info_pptr[app_count]->hiddenkey.valid_num_retries;
          card_app_info_ptr[card_app_count].hiddenkey.valid_num_unblock_retries =
                slot_data_ptr->app_info_pptr[app_count]->hiddenkey.valid_num_unblock_retries;
        }
        else
        {
          card_app_info_ptr[card_app_count].hiddenkey.pin_id = MMGSDI_MAX_PIN_ENUM;
        }

        /* DIR Index */
        card_app_info_ptr[card_app_count].dir_index =
              slot_data_ptr->app_info_pptr[app_count]->dir_index;

        /* Check to see if this App is a provisioning app */
        for(prov_app_count = 0; prov_app_count < MMGSDI_MAX_PROV_APPS; prov_app_count++)
        {
          if((app_count | MMGSDI_PROV_INDEX_VALIDITY_MASK) == slot_data_ptr->prov_app_indices[prov_app_count])
          {
            card_app_info_ptr[card_app_count].prov_app = TRUE;
            card_app_info_ptr[card_app_count].cached = TRUE;
            break;
          }
        }
        /* Indicate the App type */
        card_app_info_ptr[card_app_count].app_data.app_type =
                slot_data_ptr->app_info_pptr[app_count]->app_data.app_type;

        /* Copy the App AID */
        mmgsdi_memscpy(&(card_app_info_ptr[card_app_count].app_data.aid),
          sizeof(card_app_info_ptr[card_app_count].app_data.aid),
          &(slot_data_ptr->app_info_pptr[app_count]->app_data.aid),
          sizeof(mmgsdi_static_data_type));

        /* Copy the App Label */
        mmgsdi_memscpy(&(card_app_info_ptr[card_app_count].app_data.label),
          sizeof(card_app_info_ptr[card_app_count].app_data.label),
          &(slot_data_ptr->app_info_pptr[app_count]->app_data.label),
          sizeof(mmgsdi_static_data_type));
        card_app_count++;

      }/* Extract apps information on Card*/
    }/*slot_data_ptr->app_info_pptr != NULL*/
  }/* End of for loop*/

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_all_available_apps */


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
)
{
  uint8                      index            = 0;
  mmgsdi_session_id_type     session_id       = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_info_type * session_info_ptr = NULL;

  /* Check index 0 is valid in client id reg table, it is occupied by MMGSDI */
  if(mmgsdi_client_id_reg_table[0] == NULL)
  {
    return session_id;
  }

  for(index = 0; index < mmgsdi_client_id_reg_table[0]->num_session; index++)
  {
    session_info_ptr = mmgsdi_client_id_reg_table[0]->session_info_ptr[index];

    /* check slot id in channel info corresponding to session id */
    if ((session_info_ptr != NULL) &&
        (session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
        (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
        (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL) &&
        (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == slot_id))
    {
      if(mmgsdi_3g_type == MMGSDI_TECH_3GPP)
      {
        if(session_info_ptr->session_type == MMGSDI_GW_PROV_PRI_SESSION ||
           session_info_ptr->session_type == MMGSDI_GW_PROV_SEC_SESSION||
           session_info_ptr->session_type == MMGSDI_GW_PROV_TER_SESSION)
        {
          session_id = session_info_ptr->session_id;
          break;
        }
      }
      else if(mmgsdi_3g_type == MMGSDI_TECH_3GPP2)
      {
        if(session_info_ptr->session_type == MMGSDI_1X_PROV_PRI_SESSION ||
           session_info_ptr->session_type == MMGSDI_1X_PROV_SEC_SESSION ||
           session_info_ptr->session_type == MMGSDI_1X_PROV_TER_SESSION)
        {
          session_id = session_info_ptr->session_id;
          break;
        }
      }
    }
  }
  return session_id;
} /* mmgsdi_util_get_mmgsdi_prov_session_id_from_slot */


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
)
{
  MMGSDIUTIL_RETURN_IF_NULL(session_id_ptr);

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *session_id_ptr = mmgsdi_generic_data_ptr->pri_gw_session_id;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      *session_id_ptr = mmgsdi_generic_data_ptr->sec_gw_session_id;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      *session_id_ptr = mmgsdi_generic_data_ptr->ter_gw_session_id;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION:
      *session_id_ptr = mmgsdi_generic_data_ptr->pri_1x_session_id;
      break;
    case MMGSDI_1X_PROV_SEC_SESSION:
      *session_id_ptr = mmgsdi_generic_data_ptr->sec_1x_session_id;
      break;
    case MMGSDI_1X_PROV_TER_SESSION:
      *session_id_ptr = mmgsdi_generic_data_ptr->ter_1x_session_id;
      break;
    case MMGSDI_CARD_SESSION_SLOT_1:
      *session_id_ptr = mmgsdi_generic_data_ptr->
                          slot_session_id[MMGSDI_SLOT_1_INDEX];
      break;
    case MMGSDI_CARD_SESSION_SLOT_2:
      *session_id_ptr = mmgsdi_generic_data_ptr->
                          slot_session_id[MMGSDI_SLOT_2_INDEX];
      break;
    case MMGSDI_CARD_SESSION_SLOT_3:
      *session_id_ptr = mmgsdi_generic_data_ptr->
                          slot_session_id[MMGSDI_SLOT_3_INDEX];
      break;
    default:
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_mmgsdi_session_id_from_type */


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
)
{
  MMGSDIUTIL_RETURN_IF_NULL(session_id_ptr);

  if (mmgsdi_util_is_prov_session(session_type))
  {
    return mmgsdi_util_get_mmgsdi_session_id_from_type(session_type,
                                                       session_id_ptr);
  }

  /* session type must have been non-prov */
  return MMGSDI_ERROR;
} /* mmgsdi_util_get_mmgsdi_session_id_from_provisioning_type */


/* ============================================================================
FUNCTION MMGSDI_UTIL_IS_AID_VALID_IN_SLOT

DESCRIPTION
  This is an utility function which verifies if the incoming aid is valid for
  corresponding slot. This is valid only for aids that are present in EF DIR

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
  mmgsdi_slot_data_type        *slot_data_ptr,
  mmgsdi_app_enum_type          app_type,
  mmgsdi_static_data_type       app_data
)
{
  uint32                        app_count =  0;

  for (app_count = 0; app_count < MMGSDI_MAX_APP_INFO; app_count++)
  {
    if((slot_data_ptr->app_info_pptr[app_count] == NULL) ||
       (slot_data_ptr->app_info_pptr[app_count]->app_data.app_type ==
        MMGSDI_APP_NONE) ||
       (slot_data_ptr->app_info_pptr[app_count]->app_not_in_efdir))
    {
      continue;
    }
    if(slot_data_ptr->app_info_pptr[app_count]->app_data.app_type != app_type)
    {
      continue;
    }
    if(slot_data_ptr->app_info_pptr[app_count]->app_data.aid.data_len !=
       app_data.data_len)
    {
      continue;
    }
    if(memcmp(slot_data_ptr->app_info_pptr[app_count]->app_data.aid.data_ptr,
              app_data.data_ptr,
              app_data.data_len) == 0)
    {
      return TRUE;
    }
  }
  return FALSE;
}/* mmgsdi_util_is_aid_valid_in_slot */


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
)
{
  MMGSDIUTIL_RETURN_IF_NULL(apdu_data_ptr);

  /* APDU DATA Len must be within a certain range */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(apdu_data_ptr->data_len,
                                    MMGSDI_MIN_APDU_LEN,
                                    MMGSDI_MAX_CHANNEL_DATA_CMD_LEN);

  /* Currently UIM software only supports short Lc and Le in command APDUs.
     Return error if extended Lc/Le or mismatched data length are found in
     the APDU */
  if (apdu_data_ptr->data_len > UIM_CASE2_7816_APDU_SIZE)
  {
    if (apdu_data_ptr->data_ptr[UIM_CASE3_4_7816_LC_OFFSET] == 0)
    {
      return MMGSDI_INCORRECT_PARAMS;
    }
    if ((apdu_data_ptr->data_ptr[UIM_CASE3_4_7816_LC_OFFSET] !=
         apdu_data_ptr->data_len - (UIM_CASE3_4_7816_LC_OFFSET + 1)) &&
        ((apdu_data_ptr->data_len - (UIM_CASE3_4_7816_LC_OFFSET + 1)) -
          apdu_data_ptr->data_ptr[UIM_CASE3_4_7816_LC_OFFSET] != 1))
    {
      UIM_MSG_ERR_0("Malformed APDU");
      return MMGSDI_INCORRECT_PARAMS;
    }
  }

  /* Per ISO 7816-4 Section 5.1, return error for invalid CLA byte (0xFF) or
     INS byte (0x6X or 0x9X) */
  if ((apdu_data_ptr->data_ptr[UIM_7816_APDU_CLASS_OFFSET] == 0xFF) ||
      ((apdu_data_ptr->data_ptr[UIM_7816_APDU_INSTRN_OFFSET] & 0xF0) == 0x60) ||
      ((apdu_data_ptr->data_ptr[UIM_7816_APDU_INSTRN_OFFSET] & 0xF0) == 0x90))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* If the APDU is a Select Command, it must meet certain length
  ** Requirements
  */
  if ((apdu_data_ptr->data_ptr[UIM_7816_APDU_INSTRN_OFFSET] == SELECT) &&
      (apdu_data_ptr->data_ptr[UIM_7816_APDU_P1_OFFSET] == UIM_7816_APDU_MIN_SIZE) &&
     ((apdu_data_ptr->data_len < MMGSDI_SELECT_APDU_MIN_LEN) ||
      (apdu_data_ptr->data_ptr[MMGSDI_SELECT_APDU_LEN_OFFSET] < MMGSDI_SELECT_APDU_MIN_LEN)))
  {
    UIM_MSG_ERR_0("Invalid length for Select Command");
    return MMGSDI_INCORRECT_PARAMS;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_apdu_sanity_check */


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
  uint8                    * channel_info_index_ptr)
{
  uint8                      channel_index    = 0;
  mmgsdi_channel_info_type * curr_channel_ptr = NULL;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  if((aid_data_ptr == NULL) ||
     (aid_data_ptr->data_len <= 0) ||
     (aid_data_ptr->data_len > MMGSDI_MAX_AID_LEN))
  {
    return FALSE;
  }

  for (channel_index = 0; channel_index < MMGSDI_MAX_CHANNEL_INFO; channel_index++)
  {
    if (mmgsdi_channel_info_ptr_table[channel_index] == NULL)
    {
      continue;
    }

    curr_channel_ptr = mmgsdi_channel_info_ptr_table[channel_index];

    if ((curr_channel_ptr->slot_id != slot_id) ||
        (curr_channel_ptr->app_info_ptr == NULL))
    {
      continue;
    }

    if(curr_channel_ptr->exclusive_channel)
    {
      continue;
    }

    if ((curr_channel_ptr->app_info_ptr->app_data.aid.data_len != aid_data_ptr->data_len) ||
        (memcmp((void *)(curr_channel_ptr->app_info_ptr->app_data.aid.data_ptr),
                (void *)(aid_data_ptr->data_ptr), aid_data_ptr->data_len)) )
    {
      continue;
    }

    if (channel_id_ptr)
    {
      *channel_id_ptr = curr_channel_ptr->channel_id;
    }
    if (channel_info_index_ptr)
    {
      *channel_info_index_ptr = channel_index;
    }

    return TRUE;
  }

  return FALSE;
} /* mmgsdi_util_check_is_channel_available_for_reuse_uicc */


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
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  int32                   num_sessions  = 0;
  uint8                   i             = 0;
  int32                   index         = 0;

  MMGSDIUTIL_CHECK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  MMGSDIUTIL_RETURN_IF_NULL(client_has_non_prov_open);

  *client_has_non_prov_open = FALSE;

  mmgsdi_status = mmgsdi_util_get_client_id_index(client_id, &index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  num_sessions = mmgsdi_client_id_reg_table[index]->num_session;

  /* Check the session type of every session this client has open  */
  for (i = 0; (num_sessions > 0) && (i < MMGSDI_MAX_SESSION_INFO) ; i++)
  {
    if (mmgsdi_client_id_reg_table[index]->session_info_ptr[i] == NULL)
    {
      continue;
    }
    num_sessions--;

    if(mmgsdi_util_is_non_prov_session(
         mmgsdi_client_id_reg_table[index]->session_info_ptr[i]->session_type))
    {
      /* There is at least one non-prov session open */
      *client_has_non_prov_open = TRUE;
      break;
    }
  }

  /* No non-prov sessions were found */
  return MMGSDI_SUCCESS;
} /* mmgsdi_check_client_has_non_prov_open */


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
  mmgsdi_slot_id_enum_type slot_id)
{
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_id_type    session_id       = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_int_app_info_type *int_app_info_ptr = NULL;
  mmgsdi_data_type          imsi_data;
  mmgsdi_access_type        imsi_file_access;
  uint16                    imsi_mcc = 0;
  uint16                    imsi_mnc = 0;
  uint8                     imsi_buffer[MMGSDI_CDMA_IMSI_LEN] = {0};

  session_id = mmgsdi_util_get_mmgsdi_prov_session_id_from_slot(
                 slot_id,
                 MMGSDI_TECH_3GPP2);

  if(session_id == MMGSDI_INVALID_SESSION_ID)
  {
    UIM_MSG_ERR_1("No active 1x session available on slot: 0x%x", slot_id);
    return FALSE;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &int_app_info_ptr,
                    NULL,
                    NULL,
                    NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     int_app_info_ptr == NULL)
  {
    return FALSE;
  }

  memset(&imsi_data, 0x00, sizeof(mmgsdi_data_type));
  memset(&imsi_file_access, 0x00, sizeof(mmgsdi_access_type));

  /*Read EF-IMSI from cache*/
  imsi_file_access.access_method = MMGSDI_EF_ENUM_ACCESS;

  if(int_app_info_ptr->app_data.app_type == MMGSDI_APP_RUIM)
  {
    imsi_file_access.file.file_enum = MMGSDI_CDMA_IMSI_M;
  }
  else
  {
    imsi_file_access.file.file_enum = MMGSDI_CSIM_IMSI_M;
  }

  imsi_data.data_len = MMGSDI_CDMA_IMSI_LEN;
  imsi_data.data_ptr = imsi_buffer;

  mmgsdi_status = mmgsdi_cache_read(
                    session_id,
                    &imsi_file_access,
                    &imsi_data,
                    0,
                    NULL);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (imsi_data.data_len != MMGSDI_CDMA_IMSI_LEN))
  {
    return FALSE;
  }

  imsi_mcc = imsi_data.data_ptr[MMGSDIUTIL_IMSI_MCC_SEC_BYTE_POS] & 0x03;
  imsi_mcc = imsi_mcc << 8;
  imsi_mcc = imsi_mcc | imsi_data.data_ptr[MMGSDIUTIL_IMSI_MCC_FIRST_BYTE_POS];

  imsi_mnc = imsi_data.data_ptr[MMGSDIUTIL_IMSI_MNC_POS];

  if(imsi_mcc == MMGSDIUTIL_IMSI_MCC_CUST_DEFAULT &&
     imsi_mnc == MMGSDIUTIL_IMSI_MNC_CUST_DEFAULT)
  {
    UIM_MSG_HIGH_1("IMSI M PLMN matches with customer PLMN on slot: 0x%x", slot_id);
    return TRUE;
  }

  return FALSE;
}/* mmgsdi_util_is_imsi_plmn_cust_plmn */


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
)
{
  mmgsdi_nv_context_type nv_context;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  nv_context = mmgsdi_util_get_efs_item_index_for_slot(slot_id);

  /* Return TRUE if all of the followings are true:
     1. MMGSDI_FEATURE_PURGE_TEMP_IDENTITIES is enabled
     2. A MMGSDI_REFRESH_RESET is in progess on the given slot
     */
  if ((slot_id                == mmgsdi_refresh_data_info.slot_id)      &&
      (MMGSDI_REFRESH_RESET   == mmgsdi_refresh_data_info.refresh_mode) &&
      (MMGSDI_FEATURE_ENABLED ==
       mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_PURGE_TEMP_IDENTITIES, nv_context)))
  {
    return TRUE;
  }

  return FALSE;
}/* mmgsdi_util_is_purge_temp_identities_in_progress */


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
  mmgsdi_session_id_type           * session_id_ptr)
{
  mmgsdi_return_enum_type    mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_channel_info_type  *channel_info_ptr     = NULL;
  uint8                      i                    = 0;
  mmgsdi_session_id_type     mmgsdi_session_ids[] =
                               {mmgsdi_generic_data_ptr->pri_gw_session_id,
                                mmgsdi_generic_data_ptr->sec_gw_session_id,
                                mmgsdi_generic_data_ptr->ter_gw_session_id,
                                mmgsdi_generic_data_ptr->pri_1x_session_id,
                                mmgsdi_generic_data_ptr->sec_1x_session_id,
                                mmgsdi_generic_data_ptr->ter_1x_session_id};

  MMGSDIUTIL_RETURN_IF_NULL_2(session_id_ptr, app_data_ptr);

  if (slot_id == MMGSDI_SLOT_AUTOMATIC)
  {
    return MMGSDI_ERROR;
  }

  for(i = 0; i < sizeof(mmgsdi_session_ids)/sizeof(mmgsdi_session_ids[0]); i++)
  {
    mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                        mmgsdi_session_ids[i],
                        NULL,
                        &channel_info_ptr);

    if((mmgsdi_status == MMGSDI_SUCCESS) &&
       (channel_info_ptr != NULL) &&
       (channel_info_ptr->slot_id == slot_id))
    {
      if(channel_info_ptr->app_info_ptr != NULL)
      {
        if (app_data_ptr->app_type == MMGSDI_APP_SIM ||
            app_data_ptr->app_type == MMGSDI_APP_RUIM)
        {
          if (channel_info_ptr->app_info_ptr->app_data.app_type ==
                app_data_ptr->app_type)
          {
            *session_id_ptr = mmgsdi_session_ids[i];
             return MMGSDI_SUCCESS;
          }
        }
        else if (app_data_ptr->app_type == MMGSDI_APP_USIM ||
                 app_data_ptr->app_type == MMGSDI_APP_CSIM)
        {
          if ((channel_info_ptr->app_info_ptr->app_data.app_type ==
                 app_data_ptr->app_type)               &&
              (channel_info_ptr->app_info_ptr->app_data.aid.data_len ==
                 app_data_ptr->aid.data_len) &&
              (app_data_ptr->aid.data_len > 0) &&
              (app_data_ptr->aid.data_len <= MMGSDI_MAX_AID_LEN) &&
              (memcmp(channel_info_ptr->app_info_ptr->app_data.aid.data_ptr,
                      app_data_ptr->aid.data_ptr,
                      app_data_ptr->aid.data_len) == 0))
          {
            *session_id_ptr = mmgsdi_session_ids[i];
            return MMGSDI_SUCCESS;
          }
        }
      }
    }
  }

  return MMGSDI_ERROR;
} /* mmgsdi_util_get_prov_session_id_for_app */


/* =============================================================================
  FUNCTION:      MMGSDI_UTIL_CHECK_APP_TERMINATION_SKIPPED

  DESCRIPTION:
    This function checks if termination of the application was previously
    skipped on a channel but the channel was kept open and returns the
    corresponding channel id.

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
  mmgsdi_channel_id_type           * channel_id_ptr)
{
  uint8                   i             = 0;

  if(app_data_ptr == NULL)
  {
    return FALSE;
  }

  for(i = 0; i < MMGSDI_MAX_CHANNEL_INFO; i++)
  {
    if(mmgsdi_channel_info_ptr_table[i] == NULL ||
       slot_id != mmgsdi_channel_info_ptr_table[i]->slot_id)
    {
      continue;
    }

    if(mmgsdi_channel_info_ptr_table[i]->app_info_ptr == NULL)
    {
      continue;
    }

    if((app_data_ptr->aid.data_len !=
         mmgsdi_channel_info_ptr_table[i]->app_info_ptr->app_data.aid.data_len) ||
       (app_data_ptr->aid.data_len < 0) ||
       (app_data_ptr->aid.data_len > MMGSDI_MAX_AID_LEN))
    {
      continue;
    }

    /* Return TRUE if the app is found in the channel info table with the
       app_termination_skipped flag set -- meaning that the session for that
       app was deactivated but the channel was kept open. */
    if(memcmp(mmgsdi_channel_info_ptr_table[i]->app_info_ptr->app_data.aid.data_ptr,
               app_data_ptr->aid.data_ptr,
               app_data_ptr->aid.data_len) != 0)
    {
      continue;
    }

    if(mmgsdi_channel_info_ptr_table[i]->app_termination_skipped)
    {
      if (channel_id_ptr)
      {
        *channel_id_ptr = mmgsdi_channel_info_ptr_table[i]->channel_id;
      }
      return TRUE;
    }
    return FALSE;
  }

  return FALSE;
} /* mmgsdi_util_check_app_termination_skipped */


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
  mmgsdi_slot_id_enum_type * slot_id)
{
  MMGSDIUTIL_RETURN_IF_NULL(slot_id);

  switch(slot_index)
  {
    case MMGSDI_SLOT_1_INDEX:
      *slot_id = MMGSDI_SLOT_1;
      break;
    case MMGSDI_SLOT_2_INDEX:
      *slot_id = MMGSDI_SLOT_2;
      break;
    case MMGSDI_SLOT_3_INDEX:
      *slot_id = MMGSDI_SLOT_3;
      break;
    default:
      UIM_MSG_ERR_1("Could not retrieve slot id for slot index 0x%x", slot_index);
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_slot_id */


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
)
{
  const uint8 usim_aid_value[] = { MMGSDI_3GPP_APP_ID_RID_BYTE_1,
                                   MMGSDI_3GPP_APP_ID_RID_BYTE_2,
                                   MMGSDI_3GPP_APP_ID_RID_BYTE_3,
                                   MMGSDI_3GPP_APP_ID_RID_BYTE_4,
                                   MMGSDI_3GPP_APP_ID_RID_BYTE_5,
                                   MMGSDI_USIM_APP_CODE_BYTE_1,
                                   MMGSDI_USIM_APP_CODE_BYTE_2 };
  if (aid_ptr == NULL)
  {
    return FALSE;
  }

  if (aid_len >= sizeof(usim_aid_value))
  {
    if (memcmp(aid_ptr, usim_aid_value, sizeof(usim_aid_value)) == 0)
    {
      return TRUE;
    }
  }

  return FALSE;
} /* mmgsdi_util_is_usim_aid */


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
)
{
  const uint8 csim_aid_value[] = { MMGSDI_3GPP2_APP_ID_RID_BYTE_1,
                                   MMGSDI_3GPP2_APP_ID_RID_BYTE_2,
                                   MMGSDI_3GPP2_APP_ID_RID_BYTE_3,
                                   MMGSDI_3GPP2_APP_ID_RID_BYTE_4,
                                   MMGSDI_3GPP2_APP_ID_RID_BYTE_5,
                                   MMGSDI_CSIM_APP_CODE_BYTE_1,
                                   MMGSDI_CSIM_APP_CODE_BYTE_2 };
  if (aid_ptr == NULL)
  {
    return FALSE;
  }

  if (aid_len >= sizeof(csim_aid_value))
  {
    if (memcmp(aid_ptr, csim_aid_value, sizeof(csim_aid_value)) == 0)
    {
      return TRUE;
    }
  }

  return FALSE;
} /* mmgsdi_util_is_csim_aid */


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
)
{
  const uint8 isim_aid_value[] = { MMGSDI_3GPP_APP_ID_RID_BYTE_1,
                                   MMGSDI_3GPP_APP_ID_RID_BYTE_2,
                                   MMGSDI_3GPP_APP_ID_RID_BYTE_3,
                                   MMGSDI_3GPP_APP_ID_RID_BYTE_4,
                                   MMGSDI_3GPP_APP_ID_RID_BYTE_5,
                                   MMGSDI_ISIM_APP_CODE_BYTE_1,
                                   MMGSDI_ISIM_APP_CODE_BYTE_2 };
  if (aid_ptr == NULL)
  {
    return FALSE;
  }

  if (aid_len >= sizeof(isim_aid_value))
  {
    if (memcmp(aid_ptr, isim_aid_value, sizeof(isim_aid_value)) == 0)
    {
      return TRUE;
    }
  }

  return FALSE;
} /* mmgsdi_util_is_isim_aid */


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
)
{
  mmgsdi_return_enum_type    mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_channel_info_type  *channel_info_ptr     = NULL;
  uint8                      i                    = 0;
  mmgsdi_session_id_type     mmgsdi_session_ids[] =
                               {mmgsdi_generic_data_ptr->pri_gw_session_id,
                                mmgsdi_generic_data_ptr->sec_gw_session_id,
                                mmgsdi_generic_data_ptr->ter_gw_session_id};

  MMGSDIUTIL_RETURN_IF_NULL_2(session_id_ptr, app_type_ptr);

  if (mmgsdi_util_is_slot_valid(slot_id) == FALSE)
  {
    return MMGSDI_ERROR;
  }

  for(i = 0; i < sizeof(mmgsdi_session_ids)/sizeof(mmgsdi_session_ids[0]); i++)
  {
    mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                        mmgsdi_session_ids[i],
                        NULL,
                        &channel_info_ptr);

    if((mmgsdi_status == MMGSDI_SUCCESS) &&
       (channel_info_ptr != NULL) &&
       (channel_info_ptr->slot_id == slot_id) &&
       (channel_info_ptr->app_info_ptr != NULL))
    {
      if (channel_info_ptr->app_info_ptr->app_data.app_type == MMGSDI_APP_SIM ||
          channel_info_ptr->app_info_ptr->app_data.app_type == MMGSDI_APP_USIM)
      {
        *session_id_ptr = mmgsdi_session_ids[i];
        *app_type_ptr = channel_info_ptr->app_info_ptr->app_data.app_type;
        return MMGSDI_SUCCESS;
      }
    }
  }

  return MMGSDI_ERROR;
} /* mmgsdi_util_find_gw_prov_session_for_slot */


/*===========================================================================
  FUNCTION MMGSDI_UTIL_CONVERT_TO_IMSI_DATA_TYPE()

  DESCRIPTION
    Convert IMSI from mmgsdi_data_type to mmgsdi_imsi_data_type

  PARAMETERS
    imsi_data_ptr  : value of imis in mmgsdi_data_type format
    imsi_digit_ptr : value of imsi in a mmgsdi_imsi_data_type digit format

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
)
{
  uint8                 i     = 0;

  if((imsi_data_ptr == NULL) ||
     (imsi_digit_ptr == NULL) ||
     (imsi_data_ptr->data_ptr == NULL) ||
   (((imsi_data_ptr->data_len - 1) * 2) - 1 > sizeof(imsi_digit_ptr->d)))
  {
    UIM_MSG_ERR_0("Invalid imsi_data_ptr");
    return MMGSDI_ERROR;
  }

  memset(&imsi_digit_ptr->d[0], 0xF, sizeof(imsi_digit_ptr->d));

  /* -----------------------------------------------------------
    Parse MCC from the IMSI:
    B0     B1    B2   B3   B4    B5    B6     B7      B8
    LEN  D1PB   D3D2 D5D4 D7D6  D9D8 D11D10 D13D12  D15D14
  -------------------------------------------------------------*/
  for(i = 0; (i < (imsi_data_ptr->data_len - 2)); i++)
  {
    imsi_digit_ptr->d[i*2] = imsi_data_ptr->data_ptr[i+1] & MMGSDI_IMSI_HIGH_BYTE_MASK;
    imsi_digit_ptr->d[i*2] >>= 4;
    imsi_digit_ptr->d[i*2 + 1] = imsi_data_ptr->data_ptr[i+2] & MMGSDI_IMSI_LOW_BYTE_MASK;
  }
  imsi_digit_ptr->d[i * 2] =
    imsi_data_ptr->data_ptr[(imsi_data_ptr->data_len) - 1] & MMGSDI_IMSI_HIGH_BYTE_MASK;
  imsi_digit_ptr->d[i * 2] >>= 4;

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_convert_to_imsi_data_type */


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
)
{
  if (channel_info_index >= MMGSDI_MAX_CHANNEL_INFO ||
      mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr == NULL)
  {
    return;
  }

  if(mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->app_state >
       MMGSDI_APP_STATE_UNKNOWN)
  {
    mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->app_state =
      MMGSDI_APP_STATE_DETECTED;
  }

  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->perso_state =
      MMGSDI_SIMLOCK_NOT_VERIFIED;

  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->usim_spec_version =
    MMGSDI_VERSION_UNKNOWN;

  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->gprs_anite_mcc = FALSE;
  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->tds_test_imsi = FALSE;

  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->hzi_version = MMGSDI_UHZI_NONE;

  /* Cleanup PIN info connected to the app */
  mmgsdi_util_app_pin_cleanup(mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr,
                              mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id);
  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->app_selected = FALSE;
  (void)mmgsdi_util_clear_app_capabilities(
          &mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->app_capabilities);
  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->perso_info.feature             =
    MMGSDI_MAX_PERSO_FEATURE_ENUM;
  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->perso_info.num_retries         = 0;
  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->perso_info.num_unblock_retries = 0;
  if (mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->app_not_in_efdir)
  {
    memset(&mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->app_data,
           0x00, sizeof(mmgsdi_aid_type));
  }

  memset(&mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->aid_in_fcp,
          0x00, sizeof(mmgsdi_static_data_type));
  /* EONS Info Cleanup */
  mmgsdi_util_app_eons_cleanup(mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr);
  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->status_app_is_initialized = FALSE;
  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->pre_perso_files_cached    = FALSE;

  /* App has been deactivated, so set the deactivation_in_progress to FALSE */
  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr->deactivation_in_progress  = FALSE;

  mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr = NULL;
} /* mmgsdi_util_reset_app_info */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_RESET_UNUSED_CHANNEL_APP_DATA

DESCRIPTION
  This function resets the app data and frees the corresponding channel info
  table entry for the given slot and channel id combination if app
  termination was previously skipped on that channel.
  If an invalid channel id is passed, all the channel entries on the current
  slot where app termination was skipped are cleared.

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
)
{
  uint8 i = 0;

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
  for (i = 0; i < MMGSDI_MAX_CHANNEL_INFO; i++)
  {
    if (mmgsdi_channel_info_ptr_table[i] == NULL ||
        mmgsdi_channel_info_ptr_table[i]->slot_id != slot_id ||
        (channel_id < UIM_MAX_CHANNELS &&
         mmgsdi_channel_info_ptr_table[i]->channel_id != channel_id) ||
        mmgsdi_channel_info_ptr_table[i]->num_connections != 0 ||
        !mmgsdi_channel_info_ptr_table[i]->app_termination_skipped)
    {
      continue;
    }

    if (mmgsdi_channel_info_ptr_table[i]->app_info_ptr != NULL)
    {
      mmgsdi_util_reset_app_info(i);
    }

    /* Free the channel info table entry */
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_channel_info_ptr_table[i]);
  }
  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
} /* mmgsdi_util_reset_unused_channel_app_data */


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
    mmgsdi_return_type

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_channel_app(
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_channel_id_type             channel_id,
  mmgsdi_int_app_info_type        ** app_info_pptr)
{
  uint8 i = 0;

  if(app_info_pptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  for (i = 0; i < MMGSDI_MAX_CHANNEL_INFO; i++)
  {
    if(mmgsdi_channel_info_ptr_table[i] == NULL ||
       mmgsdi_channel_info_ptr_table[i]->slot_id != slot_id ||
       mmgsdi_channel_info_ptr_table[i]->channel_id != channel_id)
    {
      continue;
    }

    if (mmgsdi_channel_info_ptr_table[i]->app_info_ptr == NULL)
    {
      break;
    }

    *app_info_pptr = mmgsdi_channel_info_ptr_table[i]->app_info_ptr;

    return MMGSDI_SUCCESS;
  }

  return MMGSDI_ERROR;
} /* mmgsdi_util_get_channel_app */


/* =============================================================================
  FUNCTION:      MMGSDI_UTIL_GET_PROV_INDEX_FROM_SESSION

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
)
{
  uint8 session_index = MMGSDI_MAX_PROV_APPS;

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      session_index = MMGSDI_PRI_GW_PROV_APP_SEL_INDEX;
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      session_index = MMGSDI_PRI_1X_PROV_APP_SEL_INDEX;
      break;

    case MMGSDI_GW_PROV_SEC_SESSION:
      session_index = MMGSDI_SEC_GW_PROV_APP_SEL_INDEX;
      break;

    case MMGSDI_1X_PROV_SEC_SESSION:
      session_index = MMGSDI_SEC_1X_PROV_APP_SEL_INDEX;
      break;

    case MMGSDI_GW_PROV_TER_SESSION:
      session_index = MMGSDI_TER_GW_PROV_APP_SEL_INDEX;
      break;

    case MMGSDI_1X_PROV_TER_SESSION:
      session_index = MMGSDI_TER_1X_PROV_APP_SEL_INDEX;
      break;

    default:
      UIM_MSG_ERR_1("Invalid  provisioning session_type 0x%x", session_type);
      break;
  }

  return session_index;
} /* mmgsdi_util_get_prov_index_from_session */


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
)
{
  if(access_ptr == NULL)
  {
    return FALSE;
  }

  if(access_ptr->access_method == MMGSDI_BY_PATH_ACCESS)
  {
    if((access_ptr->file.path_type.path_len == 4) &&
       (access_ptr->file.path_type.path_buf[0x00]  == GSDI_MF_PATH_USED) &&
       ((access_ptr->file.path_type.path_buf[0x01] == GSDI_TELECOM_PATH_USED) ||
       (access_ptr->file.path_type.path_buf[0x01]  == GSDI_ADF_PATH_USED)) &&
       (access_ptr->file.path_type.path_buf[0x02]  == GSDI_PHONEBOOK_DF_PATH_USED) &&
       (access_ptr->file.path_type.path_buf[0x03]  == GSDI_PBR_EF_PATH_USED))
    {
      return TRUE;
    }
  }

  return FALSE;
} /* mmgsdi_util_is_ef_pbr */


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
  mmgsdi_session_type_enum_type *cors_src_session_ptr
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  uint8                         iterator_attr = 0;
  mmgsdi_session_type_enum_type session_pair[MMGSDI_NUM_ATTRIBUTES][2] =
    {
      {MMGSDI_GW_PROV_PRI_SESSION, MMGSDI_1X_PROV_PRI_SESSION},
      {MMGSDI_GW_PROV_SEC_SESSION, MMGSDI_1X_PROV_SEC_SESSION},
      {MMGSDI_GW_PROV_TER_SESSION, MMGSDI_1X_PROV_TER_SESSION}
    };

  MMGSDIUTIL_RETURN_IF_NULL(cors_src_session_ptr);

  for (iterator_attr = 0;iterator_attr < MMGSDI_NUM_ATTRIBUTES; iterator_attr++)
  {
    if (session_pair[iterator_attr][0] == src_session_type)
    {
      *cors_src_session_ptr = session_pair[iterator_attr][1];
      mmgsdi_status = MMGSDI_SUCCESS;
      break;
    }
    else if (session_pair[iterator_attr][1] == src_session_type)
    {
      *cors_src_session_ptr = session_pair[iterator_attr][0];
      mmgsdi_status = MMGSDI_SUCCESS;
      break;
    }
  }
  return mmgsdi_status;
} /* mmgsdi_util_get_pair_session_type */


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
)
{
  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_gw_session_type_ptr, mmgsdi_1x_session_type_ptr);

  switch(as_id)
  {
    case SYS_MODEM_AS_ID_1:
      *mmgsdi_gw_session_type_ptr = MMGSDI_GW_PROV_PRI_SESSION;
      *mmgsdi_1x_session_type_ptr = MMGSDI_1X_PROV_PRI_SESSION;
      break;

    case SYS_MODEM_AS_ID_2:
      *mmgsdi_gw_session_type_ptr = MMGSDI_GW_PROV_SEC_SESSION;
      *mmgsdi_1x_session_type_ptr = MMGSDI_1X_PROV_SEC_SESSION;
      break;

    case SYS_MODEM_AS_ID_3:
      *mmgsdi_gw_session_type_ptr = MMGSDI_GW_PROV_TER_SESSION;
      *mmgsdi_1x_session_type_ptr = MMGSDI_1X_PROV_TER_SESSION;
      break;

    default:
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_session_type_from_as_id */


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
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(technology_ptr, attribute_ptr);

  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *technology_ptr = MMGSDI_TECH_3GPP;
      *attribute_ptr  = MMGSDI_ATTR_PRIMARY;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      *technology_ptr = MMGSDI_TECH_3GPP;
      *attribute_ptr  = MMGSDI_ATTR_SECONDARY;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      *technology_ptr = MMGSDI_TECH_3GPP;
      *attribute_ptr  = MMGSDI_ATTR_TERTIARY;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION:
      *technology_ptr = MMGSDI_TECH_3GPP2;
      *attribute_ptr  = MMGSDI_ATTR_PRIMARY;
      break;
    case MMGSDI_1X_PROV_SEC_SESSION:
      *technology_ptr = MMGSDI_TECH_3GPP2;
      *attribute_ptr  = MMGSDI_ATTR_SECONDARY;
      break;
    case MMGSDI_1X_PROV_TER_SESSION:
      *technology_ptr = MMGSDI_TECH_3GPP2;
      *attribute_ptr  = MMGSDI_ATTR_TERTIARY;
      break;
    default:
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }
  return mmgsdi_status;
}/* mmgsdi_util_get_tech_and_attr_from_session_type */


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
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(session_type_ptr);

  switch (technology)
  {
    case MMGSDI_TECH_3GPP:
      switch (attribute)
      {
        case MMGSDI_ATTR_PRIMARY:
          *session_type_ptr = MMGSDI_GW_PROV_PRI_SESSION;
          break;
        case MMGSDI_ATTR_SECONDARY:
          *session_type_ptr = MMGSDI_GW_PROV_SEC_SESSION;
          break;
        case MMGSDI_ATTR_TERTIARY:
          *session_type_ptr = MMGSDI_GW_PROV_TER_SESSION;
          break;
        default:
          mmgsdi_status = MMGSDI_ERROR;
          break;
      }
      break;
    case MMGSDI_TECH_3GPP2:
      switch (attribute)
      {
        case MMGSDI_ATTR_PRIMARY:
          *session_type_ptr = MMGSDI_1X_PROV_PRI_SESSION;
          break;
        case MMGSDI_ATTR_SECONDARY:
          *session_type_ptr = MMGSDI_1X_PROV_SEC_SESSION;
          break;
        case MMGSDI_ATTR_TERTIARY:
          *session_type_ptr = MMGSDI_1X_PROV_TER_SESSION;
          break;
        default:
          mmgsdi_status = MMGSDI_ERROR;
          break;
      }
      break;
    default:
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }
  return mmgsdi_status;
}/* mmgsdi_util_get_session_type_from_tech_and_attr */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_GET_NON_PROV_SESSION_TYPE_FROM_SLOT

DESCRIPTION
    This function determines the non-prov session type from slot

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
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(session_type_ptr);

  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      *session_type_ptr = MMGSDI_NON_PROV_SESSION_SLOT_1;
      break;
    case MMGSDI_SLOT_2:
      *session_type_ptr = MMGSDI_NON_PROV_SESSION_SLOT_2;
      break;
    case MMGSDI_SLOT_3:
      *session_type_ptr = MMGSDI_NON_PROV_SESSION_SLOT_3;
      break;
    default:
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
  }

  return mmgsdi_status;
}/* mmgsdi_util_get_non_prov_session_type_from_slot */


/* =========================================================================
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
  const mmgsdi_access_type *access_ptr
)
{
  if(access_ptr == NULL)
  {
    return FALSE;
  }

  if(access_ptr->access_method == MMGSDI_BY_PATH_ACCESS)
  {
    if((access_ptr->file.path_type.path_len == 4) &&
       (access_ptr->file.path_type.path_buf[0x00]  == GSDI_MF_PATH_USED) &&
       ((access_ptr->file.path_type.path_buf[0x01] == GSDI_TELECOM_PATH_USED) ||
       (access_ptr->file.path_type.path_buf[0x01]  == GSDI_ADF_PATH_USED)) &&
       (access_ptr->file.path_type.path_buf[0x02]  == GSDI_PHONEBOOK_DF_PATH_USED))
    {
      return TRUE;
    }
    /* Check if it is 3F00 7F10 6F3A */
    else if((access_ptr->file.path_type.path_len == 3) &&
            (access_ptr->file.path_type.path_buf[0x00]  == GSDI_MF_PATH_USED) &&
            (access_ptr->file.path_type.path_buf[0x01] == GSDI_TELECOM_PATH_USED) &&
            (access_ptr->file.path_type.path_buf[0x02]  == GSDI_ADN_EF_PATH_USED))
    {
      return TRUE;
    }
  }

  return FALSE;
} /* mmgsdi_util_is_ef_under_phonebook */


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
  mmgsdi_session_type_enum_type session_type
)
{
  switch(session_type)
  {
    case MMGSDI_1X_PROV_PRI_SESSION:
    case MMGSDI_1X_PROV_SEC_SESSION:
    case MMGSDI_1X_PROV_TER_SESSION:
      return TRUE;
    default:
      break;
  }
  return FALSE;
} /* mmgsdi_util_is_1x_prov_session */

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
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;

  if (file_access_ptr == NULL || file_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_cache_read_len(
                    session_id,
                    file_access_ptr,
                    &(file_data_ptr->data_len),
                    0,
                    NULL);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    if (file_data_ptr->data_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(file_data_ptr->data_ptr,
                                         file_data_ptr->data_len);

      if(file_data_ptr->data_ptr != NULL)
      {
        mmgsdi_status = mmgsdi_cache_read(
                          session_id,
                          file_access_ptr,
                          file_data_ptr,
                          0,
                          NULL);
      }
      else
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
    else
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
  }
  return mmgsdi_status;
} /* mmgsdi_util_read_cache_file */


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
)
{
  if(mcfg_slot_index_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  switch(slot_id)
  {
    case MMGSDI_SLOT_1:
      *mcfg_slot_index_ptr = MCFG_REFRESH_INDEX_0;
      break;

    case MMGSDI_SLOT_2:
      *mcfg_slot_index_ptr = MCFG_REFRESH_INDEX_1;
      break;

    case MMGSDI_SLOT_3:
      *mcfg_slot_index_ptr = MCFG_REFRESH_INDEX_2;
      break;

    default:
      return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_mcfg_refresh_index */


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
)
{
  mmgsdi_return_enum_type            mmgsdi_status        = MMGSDI_SUCCESS;
  int32                              channel_index        = MMGSDI_MAX_CHANNEL_INFO;
  mmgsdi_slot_data_type              *slot_data_ptr       = NULL;

  if(!channel_index_ptr)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_match_channel_index(slot_id,
                                                  channel_id,
                                                  &channel_index);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    /* If no entry is found then create an entry in the channel table */
    if(mmgsdi_status != MMGSDI_SUCCESS || is_icc)
    {
      mmgsdi_status = mmgsdi_util_get_free_channel_index(&channel_index);
      if((mmgsdi_status != MMGSDI_SUCCESS) ||
         (channel_index >= MMGSDI_MAX_CHANNEL_INFO) ||
         (mmgsdi_channel_info_ptr_table[channel_index] != NULL))
      {
        UIM_MSG_ERR_0("Error in allocating entry in mmgsdi_channel_info_ptr_table");
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_channel_info_ptr_table[channel_index],
                                         sizeof(mmgsdi_channel_info_type));

      if (mmgsdi_channel_info_ptr_table[channel_index] == NULL)
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        break;
      }
      /* Wipe off any data that exists at the index in the table */
      memset(mmgsdi_channel_info_ptr_table[channel_index], 0x00,
             sizeof(mmgsdi_channel_info_type));

      mmgsdi_channel_info_ptr_table[channel_index]->channel_id =
        channel_id;

      mmgsdi_channel_info_ptr_table[channel_index]->slot_id =
        slot_id;

      slot_data_ptr->available_logical_channels--;

      UIM_MSG_HIGH_1("Logical channel succesfully opened. 0x%x logical channels now available",
                     slot_data_ptr->available_logical_channels);
    }

    if(channel_index >= MMGSDI_MAX_CHANNEL_INFO ||
       mmgsdi_channel_info_ptr_table[channel_index] == NULL)
    {
      UIM_MSG_ERR_0("Invalid Channel Index");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    mmgsdi_channel_info_ptr_table[channel_index]->num_connections++;
    if((orig_request_type == MMGSDI_SESSION_OPEN_WITH_MF_REQ) ||
       (orig_request_type == MMGSDI_SESSION_OPEN_REQ) ||
       (orig_request_type == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ))
    {
      /* Set state of channel */
      mmgsdi_channel_info_ptr_table[channel_index]->channel_state = MMGSDI_CHANNEL_STATE_NON_PROV_ACT_IN_PROG;
    }
  }
  while (0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    *channel_index_ptr = channel_index;
  }

  return mmgsdi_status;
} /* mmgsdi_util_create_channel_info_table_entry */


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
)
{
  mmgsdi_meid_data_type      meid_nv_data;
  boolean                    card_meid_avail = FALSE;
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type  *app_info_ptr    = NULL;
  nv_stat_enum_type          nv_read_status  = NV_FAIL_S;
  mmgsdi_nv_item_cache_type  mmgsdi_nv_data  = {0};

  MMGSDIUTIL_RETURN_IF_NULL(esn_meid_info_ptr);

  memset(&meid_nv_data, 0x00, sizeof(mmgsdi_meid_data_type));

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    session_id,
                    NULL,
                    NULL,
                    &app_info_ptr,
                    NULL,
                    NULL,
                    NULL);
  if(mmgsdi_status != MMGSDI_SUCCESS ||
     app_info_ptr  == NULL)
  {
    return MMGSDI_ERROR;
  }

  switch(app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_RUIM:
      /* check if service 9 is supported and activated */
      mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                                   MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                   MMGSDI_CDMA_SRV_MEID,
                                                   &card_meid_avail);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        /* reset card_meid_avail to FALSE */
        card_meid_avail = FALSE;
      }
      break;

    case MMGSDI_APP_CSIM:
      /* There is no service bit for MEID for CSIM, assign card capability
         as TRUE */
      card_meid_avail = TRUE;
      break;

    default:
      return MMGSDI_ERROR;
  }

  /* Attempt to read MEID, if successful and CST n9 is activated then
     store_esn command should be sent with MEID.  If CST n8 is on and ME
     provisioned with MEID then usage indicator bit2 should also be
     interperted. */
  meid_nv_data = mmgsdi_get_meid();

  if(card_meid_avail && meid_nv_data.meid_available)
  {
    esn_meid_info_ptr->esn_usage  = GSDI_RUIM_STORE_MEID_USE_EUIM_ID;
    esn_meid_info_ptr->esn_length = GSDI_RUIM_STORE_MEID_LENGTH;
    esn_meid_info_ptr->meid_available = TRUE;
    qw_equ(esn_meid_info_ptr->meid, meid_nv_data.meid);
  }
  else
  {
    esn_meid_info_ptr->esn_usage  = GSDI_RUIM_STORE_ESN_USE_UIM_ID;
    esn_meid_info_ptr->esn_length = GSDI_RUIM_STORE_ESN_LENGTH;
    esn_meid_info_ptr->meid_available = FALSE;

    /* Get the HW ESN from NV */
    nv_read_status = mmgsdi_nv_get_item(NV_ESN_ME_I,
                                        &mmgsdi_nv_data,
                                        MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
    if (nv_read_status != NV_DONE_S)
    {
      UIM_MSG_ERR_1("NV Read ESN Failed with status: 0x%x", nv_read_status);
      return MMGSDI_ERROR;
    }
    mmgsdi_memscpy(&esn_meid_info_ptr->esn,
                   sizeof(esn_meid_info_ptr->esn),
                   &mmgsdi_nv_data.item_value.esn,
                   sizeof(esn_meid_info_ptr->esn));
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_util_get_esn_info */
