#ifndef MMGSDI_H
#define MMGSDI_H
/*===========================================================================


            M M G S D I   D E F I N I T I O N S

                      A N D   F U N C T I O N S


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
05/24/16   ar      Review of macros used by MMGSDI
05/23/16   dd      Skip Search Req for PB files
05/04/16   lm      Add EF SMSCAP to RUIM and CSIM cache list
05/02/16   sp      Adding ONCHIP support for KCGPRS files
04/14/16   tkl     Add upper bound length check in read/write APIs 
04/05/16   vdc     Send act resp and session changed evt at same time
03/29/16   av      Preserve SAP ATR when cleaning up globals during SAP connect
12/25/15   bcho    Send Card and session events based on SFS data in PSM mode
11/16/15   nr      Adding MMGSDI_TELECOM_ADN to refresh reg file list
10/26/15   nr      Disable MMGSDI PB cache for PBM module
10/16/15   ar      Don't allow reSELECT on AID that failed atleast once earlier
10/12/15   vdc     Add support for potential silent file change notification
09/25/15   vr      Added support for ERA-GLONASS
08/10/15   av      Retry read request for non spec compliant cards
07/22/15   vdc     Synchronize main task with slot tasks at boot up
07/14/15   ar      Cache complete data for req's with partial read/write data
07/09/15   ar      Validate PRL at diff steps if SD task not ready
06/18/15   bcho    Close channel if session is activated on another slot
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
03/25/15   ar      Ability to process app deact while activation is in progress
03/24/15   kk      Subs prov EFS error handling
01/30/15   vdc     Add file size tag workaround for select response
01/12/15   ar      Refresh timer increased to 150 for PS_DETACH & STOP_MODE req
12/15/14   kk      Fix MCFG UIM data sent for nv refresh
11/24/14   vv      Send simlock run algo request if pre-perso caching is done
11/10/14   yt      Return status words of original PIN OP
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
10/18/14   bcho    Refresh timer increased to accommodate MMOC timer of 72 sec
10/14/14   av      Remove unnecessary local array variable
10/02/14   tkl     Added support for persistent cache
09/30/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
09/27/14   ar      Add support for multiple slots in BT-SAP
09/18/14   av      Remove unused defines from code
09/18/14   bcho    Handle QMI UIM SIG as if processed by non-MMGSDI task
09/11/14   hh      Call rex_undef_timer() when timer is no longer needed
09/04/14   ar      Remove NVRUIM cache
08/27/14   tl      Introduce network related SIM Lock slot policies
08/19/14   hh      Moved 1x_upp related functions to mmgsdi_1x_upp.c
08/14/14   tl      Add support for silent PIN verification
08/11/14   bcho    Optimize srv req by using cached srv table in client context
08/08/14   ar      Queue SESSION_DEACTIVATE_REQ to high priority queue
07/31/14   hh      Make mmgsdi_cm_init() available outside mmgsdi.c
07/30/14   vv      Added support to indicate app is initialized in recovery
07/30/14   vv      Remove unused data types
07/30/14   ar      Revert FR 20469 changes to align with NAS check-in dates
07/25/14   av      Auto onchip activation for multi slots
07/07/14   ar      Support to store more FPLMNs in cache for MMGSDI_GSM_FPLMN
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/15/14   yt      Support for NV refresh
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/30/14   vv      Remove Perso code
06/14/14   kk      Fixed channel data handling after app termination is skipped
06/12/14   av      Cache mandatory EFs used by NVRUIM for critical 1x NV reads
06/03/14   tl      Remove additional parameter variables
05/13/14   yt      Cache EF-PRL/EF-EPRL based on the actual PRL size
04/22/14   yt      Add macros for ISIM apps
04/17/14   av      Enhance MMGSDI attributes cache
04/08/14   ar      Skip sending event to clients if service status is same
03/27/14   yt      Add processing for slot 3
03/26/14   tl      SIM Lock secondary revisions
03/25/14   vdc     Added support to verify PIN after PIN is changed
03/24/14   ar      Remove MMGSDI_SESSION_ENABLE/DISABLE_FDN_REQ
03/18/14   tl      Introduce new SIM Lock feature
03/13/14   ar      Update correct PRL_VERSION and PRL_REVISION
02/25/14   yt      Support for selection mode in SELECT AID request
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/19/14   am      Added logic for clearing timers and queues
01/30/14   vdc     Optimize read trans req to read from cache before SIM init
12/05/13   tkl     SFI Support and decouple uim file enums
01/29/14   vdc     Added support for PLMN name source
01/16/14   df      Add new sim phase enumeration
01/15/14   ar      Cached MMGSDI_CDMA_RUIM_ID in mmgsdi cache
01/10/14   df      Use v2.0 diag macros
12/18/13   yt      Skip app termination as part of session deactivation
12/16/13   tl      Prevent initalizing the cache with partial files
12/09/13   vdc     Send RTRE configuration to NV asynchronously
12/04/13   spo     Do not set signals when MMGSDI task is already stopped
12/02/13   vdc     Added tag 0x25 for proprietary info TLV in FCP response
11/27/13   tl      Introduce refresh command queue
11/22/13   bcho    Use roaming status received in CM SS event in EONS algorithm
11/21/13   vv      Halt 3gpp subscription if 3gpp2 perso fails
11/18/13   vv      Added support for parallel REFRESH
11/08/13   yt      Reduce size of MMGSDI tables
11/07/13   tl      Remove TO-DO
11/05/13   tkl     Add IMSI check for TDS test SIM
10/27/13   df      Convert rsp_not_rcvd bool to uim_rsp_status enum
10/21/13   yt      Parse 92 XX status words when UIM report status is PASS
10/17/13   at      Support for MMGSDI Cancel APIs
10/16/13   tl      Remove MMGSDI_FEATURE_SAP
10/04/13   rp      Added macros for NULL cheks
10/03/13   yt      Remove duplicate code
10/01/13   vv      Do not perform READ, if EF size or rec length is zero
09/26/13   av      Enable OnChip on bootup based on EFS item
09/23/13   vr      qtf-crm MOB migration
09/23/13   df      Support for SIM busy
09/05/13   yt      Remove usage of secapi_get_random retry from perso
08/30/13   yt      Add check for integer overflow
08/30/13   av      Support for Chinese language PLMN names
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/26/13   am      Fix MMGSDI Sanity timer expiry processing
08/20/13   spo     Automatic Subscription provisioning enhancements in UIM
08/09/13   kk      Deprecating uimcallctrl and moving task init functions
08/07/13   tl      Add CDMA AKA Authenticate functionality
07/26/13   av      Move MMGSDI_MAX_APP_ID_LEN to publich header for uimqmi use
07/24/13   av      Block writing to the files being refreshed for INIT+FFCN
07/15/13   vs      EONS updates to support single baseband SGLTE
07/15/13   ssr     Fixed 62XX status byte for APP selection through
                   QMI_UIM_LOGICAL_CHANNE
06/28/13   bcho    APIs added to return plmn info from SE13 table.
06/27/13   yt      New signal for card status command queue
06/25/13   yt      Add flag to ignore app state to auth/run_gsm_algo req types
06/25/13   av      Add SF EUIMID to 1x cache list
06/24/13   vdc     Added support for triple SIM
05/31/13   av      Fix algorithm for reading the right EF ARR
05/30/13   akv     Heart beat mechanism for Watchdog
05/21/13   vv      Added support for session closed cause
05/16/13   vv      Add EF-NASCONFIG to cache
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
05/10/13   at      Handle QMI Modem task SIG during sync commands to UIM Driver
05/10/13   bcho    Added support to provide rplmn-spn display bit and network
                   info in get operator name API cnf and in EONS event
05/08/13   at      Supporting 62XX & 63XX warnings in APDU transactions
05/06/13   spo     Reduce memory used by client request table
05/06/13   spo     Modified CPHS variables to array of pointers
05/06/13   spo     Memory optimization for get response Cache
04/29/13   at      Handle QMI UIM SIG during sync commands to UIM Driver
04/26/13   yt      Fix compilation for non-CDMA builds
04/25/13   vdc     Added constant GSDI_ADN_EF_PATH_USED
04/17/13   tl      Replace Featurization with NV
04/16/13   bcho    Add EONS global info type structure
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
04/12/13   av      Move UIM tasks to group4 of RCINIT
04/12/13   spo     Do not send events to clients before Evt Reg CNF is sent
04/11/13   vv      Re-design handling of VPE response from the card
04/04/13   bcho    Param added to indicate internal deactivate request
04/01/13   spo     Do not send session events before session open CNF is sent
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
03/04/13   av      Retrieve ICCID from link_established and cache it
02/26/13   vv      Added support to save AID received in SELECT response
02/26/13   at      Added signal for OBDAPP
02/19/13   av      Caching support for GID1,GID2 and few 1x files
02/14/13   tl      Added MMGSDI_QMI_UIM_SIG comment to track signal
02/04/13   vv      Added mmgsdi_session_read_prl support for RUIM card
01/23/13   vv      Update EF-TST to indicate CSG support
01/20/13   vv      Introduced new UCS2_PACKED encoding scheme
01/20/13   vv      Added support for updating EF-ACSGL record
01/20/13   vv      Added EONS support for the CSG
01/08/13   vv      Fix to retrieve recovery data for each slot supported
12/30/12   kk      Reworked AsyncTP for faulty RUIM cards having no GSM info
12/26/12   av      Do not allow writing to the file that is being refreshed
12/12/12   shr     Modified GSDI TCB names w.r.t. Triple SIM support
12/15/12   bcho    New param for select rsp type added in channel info
12/12/12   av      Use SIGREX APIs for registering for task stop with RCINIT
12/05/12   vv      Added support for parallel processing in MMGSDI
10/15/12   yt      Add field in client_req_table for problematic commands
10/15/12   bcho    Handle SS event/get operator name API call in No Card and/or
                   Limited Service scenario
09/28/12   yt      Update user profile parameters after successful OTASP Commit
09/28/12   abg     Removed MMGSDI_DEBUG_ERR
09/25/12   av      Skip reading EF-ARR for internal selects
09/25/12   tl      Move Update EF-RPM from CM context to MMGSDI
09/20/12   bcho    Added support for mmgsdi_get_SE13_plmn_names API
09/11/12   av      Handling of VPE response from the card
09/11/12   bcho    Ignore display condition enum type param added in
                   mmgsdi_session_get_eons_req_type and in
                   mmgsdi_session_get_eons_extra_info_type
09/03/12   bcho    value of MMGSDI_REFRESH_GW_REG_FILES macro modified
                   Structures added to carry record ef data and info
08/29/12   at      Support for new API mmgsdi_sap_connect_ext
08/16/12   at      Support for new API mmgsdi_session_register_all_for_refresh
08/16/12   tl      Add support to close channel without app termination
07/30/12   yt      New structure for storing parsed SELECT response
07/25/12   bcho    Add support for RECOVERY_IND_REQ
07/25/12   tl      Fixed otasp ms key for a_key_p_rev greater than 2
07/20/12   tl      Increase number of applications in mmgsdi context to 20
07/20/12   tl      Increase MMGSDI_CMD_QUEUE_MAX_SIZE from 80 to 100
07/13/12   tl      Track clients that overflow queue
07/13/12   vv      Added support to identify apps not in EF-DIR
06/08/12   nb      Backing out last change
05/25/12   nb      Reworked AsyncTP for faulty RUIM cards having no GSM info
05/23/12   vv      Added support for the file update notification registration
05/14/12   tkl     Add QMI_SIM_INTERNAL service
04/26/12   av      Added missing files to refresh_1x_reg_file array that are
                   required by DS
04/16/12   tl      Added support for client name traces
04/02/12   tl      Added support to match entire RID with 3G App Code
03/30/12   av      Replaced feature flags with NVITEM
03/29/12   shr     Added support for Session Open with MF,
                   support for Send APDU extension API
03/28/12   bcho    Changes to replace rex_sleep calls with non-deferrable timer
03/20/12   yt      Add request type for session_open_with_select_rsp
03/16/12   yt      Modify references to gsdi_task_tcb
03/02/12   av      Removed feature FEATURE_MMGSDI_NV_ME_CONFIG
03/01/12   kk      Added support for app state extraction in get_session_info
02/18/12   av      Cleaned up remaining ALWAYS ON/OFF features
01/30/12   shr     Maintain flag to indicate last successful OTASP Commit
01/30/12   bcho    MMGSDI_EONS_SS_EVENT_REQ request processed
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   bcho    Legacy GSDI removal updates
12/21/11   kk      Added USIM ECC caching support
12/21/11   bcho    Changes for new API to open multiple sessions
                   in a single request
12/21/11   bcho    Added support for Get Operator Name API
12/07/11   ssr     extern mmgsdi_init_rtre_config function
11/11/11   kk      GSDI Removal - moved macro to api level header file
10/28/11   yt      Added new files for cache-related utility functions
10/13/11   shr     Adding support for full recovery
10/13/11   nb      Added support for new refresh stage
08/23/11   nb      Async TP enhancement
09/06/11   shr     Do not attempt a Card Reset as part of EFRESH RESET/Card
                   power-up handling, if the REFRESH has been stopped
08/30/11   js      Check for path len in parsing refresh files
08/11/11   nmb     Expanded Logical Channels
08/01/11   vs      Updates for key ref based PIN operations
08/01/11   vs      Added support for session open with select response
07/07/11   yt      Added macro for ICCID length
05/23/11   yt      Added support for INCREASE request
05/23/11   kk      Added registration of uimcallctrl in other card err cases
05/17/11   shr     Updates for Thread safety
05/04/11   ms      Send the PERSO_EVT to the late clients in perso state
                   as WAIT_FOR_DEPERSO_S and INIT_ERROR_S
05/11/11   nb      Support for Async TP
04/27/11   yt      Included private GSDI header
04/26/11   vs      Update for subscription ok req
04/22/11   vs      Support for subscription ok request
04/07/11   ms      Implementation of 2nd Slot Special Behavior for perso
                   by Locking only one of the SIM slots.
02/23/11   nmb     Refresh Retry enhancements
02/21/11   ms      Implementation of Session based caching on App capabilities
02/18/11   yt      Check if CSIM is provisioned before starting Refresh voting
02/11/11   ssr     Clear RUIM and GSDI cache after successful OTASP commit
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
01/21/11   nmb     Added refresh retry support
11/02/10   vs      Update to process NVRUIM cmds via MMGSDI high priority q
10/12/10   yt      Dynamic allocation for mmgsdi_client_req_table_info_ptr
09/29/10   yt      Enabling support for INCREASE
09/17/10   yk      Added support dynamic registration on WDOG.
09/15/10   nmb     Allocate mmgsdi_client_id_reg_table dynamically
09/08/10   nmb     mmgsdi_task_cmd_type memory reduction
09/02/10   shr     Clean up Client ID dereg and Session close handling
08/30/10   shr     Perso clean up for Sessions
08/18/10   mib     Fixed Lint warnings
07/28/10   yt      Added data_len field to mmgsdi_read_extra_info_type
07/23/10   js      Increased MMGSDI queue size to 85
07/02/10   nmb     Updates for JCDMA CSIM support
07/02/10   ssr     Fixed refresh for ICC card which have both GSM and RUIM
06/28/10   shr     Introduced high priority MMGSDI queue
06/28/10   shr     Added support for Hidden key event
06/19/10   js      Increase MMGSDI queue size to 60
06/10/10   vs      Added HPLMN and SPT to Refresh list
06/02/10   shr     Increased MMGSDI command queue size to 40
06/01/10   js      ZI memory reduction fixes
05/19/10   nb      Added prototype for mmgsdi_set_ens_support_flag
04/27/10   shr     Added Hiddenkey support
04/23/10   shr     All files in the cache must be in the REFRESH list
04/06/10   shr     Clean up REFRESH RESET and REFRESH RESET_AUTO
04/07/10   mib     Added support for KEYS in onchip configuration
04/07/10   nb      SAP Update
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
03/01/10   js      Added caching support for LTE specific files
02/19/10   shr     Session get info API implementation
02/17/10   shr     Fixed Session activate/deactivate handling
02/01/10   jk      EPRL Support
01/13/10   nb      Added declaration for mmgsdi_session_perso_operation
01/12/10   nb      Update for Release 7 Enhanced Network Selection
01/06/10   shr     Fixed Session Close handling
12/18/09   nb      Moved Dual Slot Changes
12/16/09   rn      Defined a new DF path lookup table type and its constants
12/14/09   ssr     Removed MMGSDI_SELECT_AID_REQ
12/10/09   nb      Fixed compilation errors
12/08/09   js      Added support for MMGSDI_CSIM_ARR file
12/07/09   mib     Added RPLMNACT in onchip configuration
12/04/09   kp      Replacing mmgsdi_data_slot2 with mmgsdi_data_slot2_ptr
12/03/09   shr     Cache read process enhancement
12/02/09   js      Fixed session open issue for non-prov type sessions with
                   exclusive channel
10/28/09   nb      Release 7 Enhanced Network Selection
10/12/09   kp      Moving Read-Write varibles to Heap
09/26/09   kp      ZI memory reduction changes
07/28/09   mib     Added mmgsdi_perso_lock_down_and_enable_limited_access
07/20/09   mib     Added support for XOR and milenage onchip authentication
                   Added support for additional LTE files and EF-UST extension
04/06/09   mib     Added support for onchip USIM authenticate
03/26/09   mib     Integration of support for ONCHIP USIM
03/19/09   js      Support for Enhanced NAA Refresh by file path
03/12/09   js      Support for reporting invalid number of PIN retries
03/02/09   sun     Added private header file
02/24/09   nb      Perso Command Support in MMGSDI
02/12/09   kk      Add files to refresh file list for re-reading during FCN
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
10/29/08   tml     Modem restart client id dereg clean up
10/14/08   sun     Increased the refresh timers
10/07/08   kk      Fixed handling of invalid tags and missing tags in create
                   pkcs15 table
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/20/08   jk      Support for Generate Key API
07/22/08   js      Renamed MMGSDI_MAX_PATH_LEN to MMGSDI_NUM_ARR_FILES
07/20/08   sp      Add file type to mmgsdi_read_extra_info_type
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/21/08   kk      Added PKCS15 Support
04/08/08   nk      Added CDMA Refresh Support
03/28/08   sun     Added support for OMA Bcast
03/28/08   sk      Fixed warnings
03/13/08   nk      Added CDMA FDN Support
02/21/08   sun     Added CPHS file for FCN registration
01/30/08   nk      Added #defines for JCDMA Default Values
01/29/08   sun     Added new state for refresh.
12/10/07   sun     Verify PIN Internally when SAP is Disconnected
11/19/07   js      Added Radio Access Technology Balancing support
11/15/07   sun     Added support for NAA refresh
11/08/07   vs/nk   Added EUIMID feature support
10/02/07   tml     Added proprietary app session support
09/10/07   tml     Added MFLO Supports
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
07/02/07   sun     Added new #defines for MBMS
05/22/07   nk      Added esn_me_change_flag to mmgsdi_slot_data_type
05/17/07   wli     Added support for JCDMA RUIM
05/07/07   sp      Added mmgsdi_malloc and mmgsdi_free
05/02/07   sun     Added support for Srv Available and USIM Auth APIs
04/26/07   sp      Add new defines for ARR files
04/17/07   sun     Set up a timer if external module other than UIM does not
                   respond
02/21/07   sun     Added cached flag to each of the pins
02/27/07   jk      Reduce MAX_CLIENT_INFO for Memory Reduction
01/17/07   sp      Add MMGSDI_STATE_PIN_EVT_SENT
                   to mmgsdi states
01/12/07   sun     Determine the SEID and then look for the Record Number
                   for the ARR File while parsing security attributes
01/11/07   sun     Save the PIN1 Report Status. Need to be used while sending
                   the confirmation.
12/27/06   sp      Back out pin event changes.
12/12/06   tml     Added member activate to get_file_attr_req_type
11/06/06   sp      Added new mmgsdi state MMGSDI_STATE_PIN_EVT_SENT
10/25/06   tml     Added initial support for Universal PIN replacement support
10/02/06   sp      Added me support for fdn, bdn and acl to mmgsdi data
09/08/06   sun     Added new #defines.
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
08/07/06   sun     Added pin and valid_pin flag to mmgsdi_app_pin_info
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   sun     Added new Sap State
05/23/06   tml     BDN and rehabilitate and invalidate support
04/15/06   tml     Moved onchip data to slot specific global structure
04/13/06   jar     Added ONCHIP SIM Support
03/14/06   sun     Added refresh option to mmgsdi_refresh_req_type
02/14/06   nk      Merged for ARR caching and by path changes
01/29/06   tml     Moved mmgsdi_sim_refresh to mmgsdi.h
01/04/06   tml     Updated enum name for SAP and Card powerup and down
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/08/05   pv      rename mmgsdi_check_card_status - mmgsdi_3G_ready_status_ind
11/03/05   tml     Fixed header
11/03/05   sun     Added support for refresh
10/28/05   sun     Fixed SAP Disconnect
10/20/05   sun     Added support for BT Deregisteration
09/29/05   tml     Client Deregistration support
08/30/05   jar     Added support to send/receive Status Commands
08/25/05   pv      Added support for powering down and powerering up SIM/USIM
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/26/05   sst     Added MMGSDIBT support
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
06/15/05   tml     Removed data pointer for holding read response
02/24/05   sk      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "mmgsdisessionlib.h"
#include "mmgsdilib_v.h"
#include "uim_p.h"
#include "cm.h"
#include "gstk_exp.h"
#include "task.h"
#include "simlock_common.h"
#include "simlock_modem_p.h"
#include "dog_hb_rex.h"
#include "rcinit.h"
#include "mcfg_refresh.h"

#ifndef FEATURE_UIM_USES_TASK_HANDLE
#define UIM_GSDI_TCB           &gsdi_task_tcb
#define UIM_GSDI_SLOT1_TCB     &gsdi_slot_1_task_tcb
#define UIM_GSDI_SLOT2_TCB     &gsdi_slot_2_task_tcb
#define UIM_GSDI_SLOT3_TCB     &gsdi_slot_3_task_tcb
#else
/* Need this as we are referring to UIM task handle */
TASK_EXTERN(uim);
TASK_EXTERN(uim_2);
TASK_EXTERN(uim_3);
#define UIM_GSDI_TCB       rcinit_lookup_rextask("gsdi")
#define UIM_GSDI_SLOT1_TCB rcinit_lookup_rextask("mmgsdi_1")
#define UIM_GSDI_SLOT2_TCB rcinit_lookup_rextask("mmgsdi_2")
#define UIM_GSDI_SLOT3_TCB rcinit_lookup_rextask("mmgsdi_3")
#endif /* !FEATURE_UIM_USES_TASK_HANDLE */

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define MMGSDI_TASK_CMD_Q_SIG                 0x00000001
#define MMGSDI_TASK_RSP_Q_SIG                 0x00000002
#define MMGSDI_SYNCH_UIM_REPORT_SIG           0x00000004
#define MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG    0x00000008
#define MMGSDI_RPT_TIMER_SIG                  0x00000010
#define MMGSDI_NV_READ_SIG                    0x00000020
#define MMGSDI_NV_WRITE_SIG                   0x00000040
#define MMGSDI_TIMED_SLEEP_TIMER_SIG          0x00000080
#define MMGSDI_QCSI_QMI_SIM_INTERNAL_SIG      0x00000100
#define MMGSDI_OBDAPP_SIG                     0x00000200
#define MMGSDI_QMI_UIM_SIG                    0x00000400
#define MMGSDI_RCEVT_CM_TASK_READY_SIG        0x00000800 /* RCINIT sets this when CM task
                                                            is ready */
#define MMGSDI_TASK_STOP_SIG                  0x00001000
#define MMGSDI_CLASH_WITH_TASK_OFFLINE_SIG    0x00002000 /* RESERVED SIGNAL */
#define MMGSDI_CLASH_WITH_TASK_STOP_SIG       0x00004000 /* RESERVED SIGNAL */
#define MMGSDI_CLASH_WITH_TASK_START_SIG      0x00008000 /* RESERVED SIGNAL */
#define MMGSDI_RCEVT_QMI_MODEM_TASK_READY_SIG 0x00010000 /* RCINIT sets this when QMI modem
                                                            task is ready */
#define MMGSDI_CARD_STATUS_CMD_Q_SIG          0x00020000
#define MMGSDI_REFRESH_CMD_Q_SIG              0x00040000
#define MMGSDI_SIMLOCK_MSG_SIG                0x00080000
#define MMGSDI_MAIN_TASK_READY_SIG            0x00100000 /* Main task sets this to unblock
                                                            slot tasks when it is ready */
#define MMGSDI_ECALL_STATUS_SIG               0x00200000
#define MMGSDI_CLASH_WITH_ONCRPC_REPLY_SIG    0x00400000 /* RESERVED SIGNAL */
#define MMGSDI_SYNC_WAIT_SIG                  0x00800000
#define MMGSDI_FREE_SIG_2                     0x01000000 /* FREE SIGNAL - RFU */

/*******ADDITIONAL RESERVED SIGNALS**************/
#define MMGSDI_CLASH_WITH_CAMERA_SIG          0x02000000 /* RESERVED SIGNAL */
#define MMGSDI_CLASH_WITH_GRP_ACK_SIG         0x04000000 /* RESERVED SIGNAL */
#define MMGSDI_CLASH_WITH_SEC_OP_COMPLETE_SIG 0x08000000 /* RESERVED SIGNAL */
#define MMGSDI_CLASH_WITH_VS_OP_COMPLETE_SIG  0x10000000 /* RESERVED SIGNAL */
#define MMGSDI_CLASH_WITH_FS_OP_COMPLETE_SIG  0x20000000 /* RESERVED SIGNAL */
#define MMGSDI_CLASH_WITH_REX_SLEEP_SIG       0x40000000 /* RESERVED SIGNAL */
/************************************************/

#define MMGSDI_DEFAULT_CHANNEL   0
#define MMGSDI_MAX_CHANNELS      UIM_MAX_CHANNELS

#define MMGSDI_MAX_CLIENT_INFO   50

/* (Number of tasks + 1 for main task) * (Number of commands that can be in progress per task) */
#define MMGSDI_MAX_CLIENT_REQ_INFO   ((MMGSDI_MAX_NUM_SLOTS + 1) * 4)

#define MMGSDI_CLIENT_ID_SHIFT             32
#define MMGSDI_SESSION_ID_MASK             0x00000000FFFFFFFF

/* defines an upper bound length for checking in APIs  */
#define MMGSDI_MAX_DATA_SIZE               0xFFFF

#define MMGSDI_MAX_REC_NUM                 0xFF
#define MMGSDI_MAX_REC_LEN                 0xFF

#define MMGSDI_HIGH_PRI_CMD_QUEUE_MAX_SIZE 10
#define MMGSDI_REFRESH_CMD_QUEUE_MAX_SIZE  20

#define MMGSDI_CMD_QUEUE_MAX_SIZE          100

#define MMGSDI_LEGACY_LOGICAL_CHANNELS     4  /* Usual number of logical channels-
                                                              mmgsdi legacy behavior */

/* Define the number of slots supported */
#define MMGSDI_MAX_NUM_SLOTS          UIM_MAX_NUM_SLOTS

#define MMGSDI_MAX_CHANNEL_INFO       (UIM_MAX_CHANNELS * MMGSDI_MAX_NUM_SLOTS)  /* maximum channels * number of slots */

#define MMGSDI_MAX_SESSION_INFO       MMGSDI_MAX_CHANNEL_INFO  /* Limit a client's sessions to total
                                                               number of logical channels */

#define MMGSDI_APP_TEMPLATE_TAG  0x61
#define MMGSDI_APP_ID_TAG        0x4F

#define MMGSDI_3GPP_APP_ID_RID_BYTE_1    0xA0
#define MMGSDI_3GPP_APP_ID_RID_BYTE_2    0x00
#define MMGSDI_3GPP_APP_ID_RID_BYTE_3    0x00
#define MMGSDI_3GPP_APP_ID_RID_BYTE_4    0x00
#define MMGSDI_3GPP_APP_ID_RID_BYTE_5    0x87
#define MMGSDI_3GPP2_APP_ID_RID_BYTE_1   0xA0
#define MMGSDI_3GPP2_APP_ID_RID_BYTE_2   0x00
#define MMGSDI_3GPP2_APP_ID_RID_BYTE_3   0x00
#define MMGSDI_3GPP2_APP_ID_RID_BYTE_4   0x03
#define MMGSDI_3GPP2_APP_ID_RID_BYTE_5   0x43
#define MMGSDI_USIM_APP_CODE_BYTE_1      0x10
#define MMGSDI_USIM_APP_CODE_BYTE_2      0x02
#define MMGSDI_CSIM_APP_CODE_BYTE_1      0x10
#define MMGSDI_CSIM_APP_CODE_BYTE_2      0x02
#define MMGSDI_ISIM_APP_CODE_BYTE_1      0x10
#define MMGSDI_ISIM_APP_CODE_BYTE_2      0x04

#define MMGSDI_UICC_APP_LABEL_TAG        0x50
#define MMGSDI_UICC_APP_LABEL_MAX_LENGTH 32
#define MMGSDI_CSIM_IMSI_M_LENGTH        0x0A

#define MMGSDI_TLV_TAG_LEN    0x01
#define MMGSDI_TLV_LEN_LEN    0x01

#define MMGSDI_SW2_CMD_X_TRYS_LEFT        0xC0
#define MMGSDI_MASK_FOR_HIGH_BYTE         0xF0
#define MMGSDI_MASK_FOR_LOW_BYTE          0x0F

#define MMGSDI_NUM_ARR_FILES              5
#define MMGSDI_ARR_UNDER_MF               0x2F06
#define MMGSDI_ARR_FIRST_DF_LEVEL         0x6F06

/*-----------------------------------------------------
   Size of Get Response cache table

   Assumption is that only one client shall stream
   APDUs per channel (except channel 0 - QMI UIM and
   ATCoP can stream APDUs).

   Hence cache size = (max. no. of logical channels +
                       1 - to accomodate additional client
                           on channel 0)*
                      max. no. of slots
-----------------------------------------------------*/
#define MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES   ((UIM_MAX_CHANNELS + 1) * MMGSDI_MAX_NUM_SLOTS)

#define MMGSDI_MAX_PROV_APPS 6

#define MMGSDI_PRI_GW_PROV_APP_SEL_INDEX 0
#define MMGSDI_PRI_1X_PROV_APP_SEL_INDEX 1
#define MMGSDI_SEC_GW_PROV_APP_SEL_INDEX 2
#define MMGSDI_SEC_1X_PROV_APP_SEL_INDEX 3
#define MMGSDI_TER_GW_PROV_APP_SEL_INDEX 4
#define MMGSDI_TER_1X_PROV_APP_SEL_INDEX 5

#define MMGSDI_PROV_INDEX_VALIDITY_MASK 0x8000
#define MMGSDI_PROV_INDEX_MASK          0x7FFF

#define MMGSDI_PRI_CPHS_INFO_INDEX 0
#define MMGSDI_SEC_CPHS_INFO_INDEX 1
#define MMGSDI_TER_CPHS_INFO_INDEX 2

#define MMGSDI_PRI_PRL_INDEX 0
#define MMGSDI_SEC_PRL_INDEX 1
#define MMGSDI_TER_PRL_INDEX 2

/* Maximum number of apps supported per slot in MMGSDI
   for Automatic Subscription provisioning */
#define MMGSDI_MAX_SUPPORTED_AUTO_PROV_APPS 2

/*-----------------------------------------------------
   ETSI 102 221, Section 10.1.1
   Various masks and constants to extract logical channel
   info from CLA byte of the APDU
-----------------------------------------------------*/
/* Mask to extract the logical channel number from CLA for
   a standard logical channel */
#define MMGSDI_CLA_STD_LOGICAL_CHANNEL_MASK 0x03

/* Mask to extract the logical channel number from CLA for
   an extended logical channel */
#define MMGSDI_CLA_EXT_LOGICAL_CHANNEL_MASK 0x0F

/* Mask to determine if the logical channel in the CLA
   is a standard or extended logical channel */
#define MMGSDI_CLA_B7_MASK                  0x40

/* First logical channel value in the extended
   logical channel format */
#define MMGSDI_FIRST_EXT_LOGICAL_CHANNEL    0x04

/* CLA value expected for ICC cards on channel 0 */
#define MMGSDI_CLA_VALUE_ICC_CH_0           0xA0

/* Masks to identify provisioning applications */
#define MMGSDI_GSM_APP_MASK     0x01
#define MMGSDI_USIM_APP_MASK    0x02
#define MMGSDI_RUIM_APP_MASK    0x04
#define MMGSDI_DCS1800_APP_MASK 0x08
#define MMGSDI_CSIM_APP_MASK    0x10

/* Constants corresponding to History bytes contents
   (per ISO/IEC 7816-4) */
#define MMGSDI_CARD_INDICATOR_TLVS   0x80
#define MMGSDI_COUNTRY_NAT_DATA      0x01
#define MMGSDI_ISSUER_ID_NUM         0x02
#define MMGSDI_CARD_SERVICES_DATA    0x31
#define MMGSDI_AID_BY_FULL_DF_MASK   0x80
#define MMGSDI_AID_BY_PART_DF_MASK   0x40
#define MMGSDI_AID_IN_EF_DIR_MASK    0x20
#define MMGSDI_AID_IN_EF_ATR_MASK    0x10
#define MMGSDI_LOGICAL_CHAN_LEN_MASK 0x07
#define MMGSDI_SFI_SUPPORT_ATR_MASK  0x04
#define MMGSDI_CARD_CAPABILITIES     0x73
#define MMGSDI_LOGICAL_CHAN_MAX      0x08

/*-----------------------------------------------------
ETSI 102.221, Table 9.2
SEID
'00' The global application PIN is disabled AND (the
     Universal PIN is either disabled or enabled
     and used or not known)
'01' The current global application PIN is enabled OR
     the Universal PIN is enabled but it is not to be
     used for verification AND the global application
     PIN is disabled
-----------------------------------------------------*/
#define MMGSDI_SEID_00                    0x00
#define MMGSDI_SEID_01                    0x01

/* Offset for Select Len */
#define MMGSDI_SELECT_APDU_LEN_OFFSET     0x04
/* Min Len for Select APDU */
#define MMGSDI_SELECT_APDU_MIN_LEN        0x05

/* ----------------------------------------------------------------------------
   DEFINE:      MMGSDI_ONCHIP_SIM_AUTH_KEY_LEN

   DESCRIPTION: Defines the maximum length allowed for the ONCHIP SIM
                Authentication Key
  ---------------------------------------------------------------------------*/
#define MMGSDI_ONCHIP_SIM_AUTH_KEY_LEN  16
#define MMGSDI_ONCHIP_USIM_AUTH_KEY_LEN 16

/* FDN, BDN, ACL support masks */
#define  MMGSDI_FDN_SUPPORTED_MASK      0x0001
#define  MMGSDI_BDN_SUPPORTED_MASK      0x0002
#define  MMGSDI_ACL_SUPPORTED_MASK      0x0004

/*  Bits set for fdn, bdn and acl */
#define  MMGSDI_DEFAULT_ME_CONFIG_VALUE 0x7

/* Minimum length of an APDU Len */
#define MMGSDI_MIN_APDU_LEN             0x4

/*Default Value for No of PIN Retries*/
#define MMGSDI_DEFAULT_PIN_RETRIES 3

/* Define the max. ATR length */
#define MMGSDI_MAX_ATR_LEN                   UIM_MAX_ATR_CHARS

/* Define the max. History Characters length */
#define MMGSDI_MAX_HIST_CHAR_LEN             UIM_MAX_HIST_CHARS

/*--------------------------------------------------------------------------
                             File cache sizes
--------------------------------------------------------------------------*/
#define MMGSDI_NUM_FILES_IN_SIM_CACHE        30

#define MMGSDI_NUM_FILES_IN_USIM_CACHE       41

#ifdef FEATURE_MMGSDI_3GPP2
#define MMGSDI_NUM_FILES_IN_RUIM_CACHE       27
#define MMGSDI_NUM_FILES_IN_CSIM_CACHE       31
#endif /* FEATURE_MMGSDI_3GPP2 */

#define MMGSDI_NUM_FILES_IN_COMMON_CACHE     3

/*--------------------------------------------------------------------------
                             MMGSDI Task Timer Values
--------------------------------------------------------------------------*/
#define MMGSDI_UIM_SANITY_TIMER_VALUE        500000 /* 8 Minutes and 20 seconds */

#define MMGSDI_UIM_SANITY_ONCHIP_TIMER_VALUE 120000 /* 2 Minutes */

#define MMGSDI_AUTO_REFRESH_TIMER            2000

#define MMGSDI_REFRESH_OK_TO_INIT_TIMER      10000

#define MMGSDI_REFRESH_OK_TO_FCN_TIMER       10000

#define MMGSDI_REFRESH_INIT_COMPLETE_TIMER   120000

#define MMGSDI_REFRESH_FCN_COMPLETE_TIMER    120000

#define MMGSDI_REFRESH_OK_TO_PROCEED_WITH_REFRESH_TIMER 150000

#define MMGSDI_ICC_SRV_MASK 0x03 /*2 consecutive bits make 1 service*/

/* Type of locks */
#define MMGSDI_JCDMA_LINE_LOCK                    0
#define MMGSDI_JCDMA_NW_LOCK                      1
#define MMGSDI_JCDMA_SP_LOCK                      2
#define MMGSDI_JCDMA_UG_LOCK                      3
/* The value read from EF to see if the card is test card */
#define MMGSDI_JCDMA_UIM_TEST_VERSION             1
#define MMGSDI_JCDMA_UIM_LOCK_IND              0x01
#define MMGSDI_JCDMA_UIM_IMSI_PROGRAMMED       0x80
#define MMGSDI_JCDMA_UIM_IMSI_STATUS           0xF7
#define MMGSDI_JCDMA_UIMID_DEFAULT       0xFFFFFFFF
#define MMGSDI_JCDMA_UIM_SP_UG_DEFAULT         0xFF
#define MMGSDI_JCDMA_8BITS                        8
#define MMGSDI_JCDMA_RUIMID_DEFAULT            0xFF
#define MMGSDI_JCDMA_IMSI_MCC1_DEFAULT          440
#define MMGSDI_JCDMA_IMSI_MCC2_DEFAULT          441
#define MMGSDI_JCDMA_SP_ID_DEFAULT                0
#define MMGSDI_JCDMA_UG_ID_DEFAULT                0
#define MMGSDI_JCDMA_INDICATOR_DEFAULT            1

#ifdef FEATURE_MMGSDI_3GPP2
#define MMGSDI_JCDMA_ESN_ME_BIT                0x01
#define MMGSDI_CDMA_FDN_FILES                     3
#endif /* FEATURE_MMGSDI_3GPP2 */

#define MMGSDI_GBA_BS_TAG                      0xDD
#define MMGSDI_GBA_NAF_DERIVATION_TAG          0xDE
#define MMGSDI_MBMS_MAX_AUTH_DATA              0x7F
#define MMGSDI_MIKEY_RESPONSE                  0x53
#define MMGSDI_MBMS_BER_TLV_TAG                0x73
#define MMGSDI_BCAST_TAG                       0xAE
#define MMGSDI_BCAST_MODE_TAG                  0x90
#define MMGSDI_LENGTH_TAG                      0x80

#define MMGSDI_REFRESH_1X_REG_FILES              74
#define MMGSDI_REFRESH_GW_REG_FILES              87

/* fix length of OTASP Response  */
#define MMGSDI_BS_CHAL_RSP_FIX_LEN             0x04
#define MMGSDI_OTASP_KEY_GEN_RSP_FIX_LEN       0x02
#define MMGSDI_OTASP_MS_KEY_RSP_FIX_LEN        0x01
#define MMGSDI_OTASP_SSPR_CONFIG_RSP_FIX_LEN   0x03
#define MMGSDI_OTASP_OTAPA_RSP_FIX_LEN         0x06
#define MMGSDI_OTASP_VALIDATE_RSP_FIX_LEN      0x02
#define MMGSDI_OTASP_DOWNLOAD_RSP_FIX_LEN      0x02
#define MMGSDI_OTASP_CONFIG_RSP_FIX_LEN        0x03
#define MMGSDI_OTASP_SSPR_DOWNLOAD_RSP_FIX_LEN 0x05
#define MMGSDI_OTASP_COMMIT_RSP_FIX_LEN        0x01

/* fix length of AKA AUTH Response*/
#define MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN      0x01
#define MMGSDI_CDMA_AKA_AUTH_RES_LEN_SIZE      0x01

#define MMGSDI_OTASP_COMMIT_RSP_SUCCESS        0x00

typedef uint8  mmgsdi_channel_id_type;

typedef rex_tcb_type mmgsdi_rex_tcb_type;

/* Minimum valid EF path length */
#define MMGSDI_MIN_EF_PATH_LEN                 0x04

/*Minimum number of valid bytes in EFHiddenkey */
#define MMGSDI_HIDDENKEY_MIN_VALID_LEN         2
/* EFHiddenkey file size */
#define MMGSDI_HIDDENKEY_LEN                   4

#define MMGSDI_MAX_OVERFLOW_SIZE               0x10

#define MMGSDI_MAX_EXT_APP_INFO                20

#define MMGSDI_MAX_PIN1_INFO                   8

/* For accessing slot at index 0 */
#define MMGSDI_SLOT_1_INDEX                    0

/* For accessing slot at index 1 */
#define MMGSDI_SLOT_2_INDEX                    1

/* For accessing slot at index 2 */
#define MMGSDI_SLOT_3_INDEX                    2

/* To check validity of bits in session_type_mask */
#define MMGSDI_SESSION_VALID_MASK             (MMGSDI_GW_PROV_PRI_SESSION_MASK | \
                                               MMGSDI_GW_PROV_SEC_SESSION_MASK | \
                                               MMGSDI_GW_PROV_TER_SESSION_MASK | \
                                               MMGSDI_1X_PROV_PRI_SESSION_MASK | \
                                               MMGSDI_1X_PROV_SEC_SESSION_MASK | \
                                               MMGSDI_1X_PROV_TER_SESSION_MASK | \
                                               MMGSDI_CARD_SESSION_SLOT_1_MASK | \
                                               MMGSDI_CARD_SESSION_SLOT_2_MASK | \
                                               MMGSDI_CARD_SESSION_SLOT_3_MASK)

/* GSM11.11 Section 9.4.1                         */
/* 3GPP TS31.101 Section 10.2.1.1                 */
/* Response Commands which are correctly Executed */
#define GSDI_SIM_SW1_NORM_END                   0x90
#define GSDI_SIM_SW2_NORM_END                   0x00
#define GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO  0x91
#define GSDI_SIM_SW1_NORM_END_W_EXTRA_INFO      0x92
#define GSDI_SIM_SW1_NORM_END_W_SW2_LENGTH      0x9F
#define GSDI_SIM_SW1_PROCEDURE_BYTE_61          0x61
#define GSDI_SIM_SW1_PROCEDURE_BYTE_6C          0x6C

/* GSM11.11 Section 9.4.2                         */
/* 3GPP TS31.101 Section 101.2.1.2                */
/* Responses to commands which are postponed      */
#define GSDI_SW1_SIM_APP_TOOLKIT_BUSY           0x93

/* GSM11.11 Section 9.4.3                         */
/* Memory Management                              */
#define GSDI_SW1_MEMORY_MANAGEMENT              0x92
#define GSDI_SW2_MEMORY_PROBLEM                 0x40

/* GSM11.11 Section 9.4.4                         */
/* Referencing Management                         */
#define GSDI_SW1_REFERENCE_MANAGEMENT           0x94
#define GSDI_SW2_NO_EF_SELECTED                 0x00
#define GSDI_SW2_OUT_OF_RANGE                   0x02
#define GSDI_SW2_FILE_OR_PATTER_ID_NOT_FOUND    0x04
#define GSDI_SW2_FILE_INCONSISTENT_WITH_CMD     0x08

/* GSM11.11 Section 9.4.5                         */
/* 3GPP TS31.101 Section 10.2.1.6                 */
/* Security Management                            */
#define GSDI_SW1_SECURITY_MANAGEMENT            0x98
#define GSDI_SW2_NO_CHV_INITIALISED             0x02
#define GSDI_SW2_VERIF_UNBLOCK_AUTH_FAIL_W_1    0x04
#define GSDI_SW2_CNTRCTN_W_CHV_STATUS           0x08
#define GSDI_SW2_CNTRCTN_W_INVALIDATION_STATUS  0x10
#define GSDI_SW2_CHV_BLOCK_UNSUCCESSFUL_UNBLOCK 0x40
#define GSDI_SW2_CHV_UNBLOCKED                  0x40
#define GSDI_SW2_INCREASED_FAIL_MAX_REACHED     0x50
#define GSDI_SW2_AUTH_ERROR_APP_SPECIFIC        0x62
#define GSDI_SW2_AUTH_ERROR_GSM_CNTXT_NOT_SUP   0x64
#define GSDI_SW2_AUTH_ERROR_KEYFRESHNESS_FAIL   0x65
#define GSDI_SW2_AUTH_ERROR_NO_MEM              0x66

/* GSM11.11 Section 9.4.6                         */
/* 3GPP TS31.101 Section 10.2.1.5                 */
/* Appliation Independent Errors                  */
#define GSDI_SW1_INCORRECT_P3                   0x67
#define GSDI_SW1_WRONG_LENGTH                   0x67   /* USIM */
#define GSDI_SW2_WRONG_LENGTH_OR_NO_DIAGS       0xFF   /* USIM */
#define GSDI_SW1_INCORRECT_P1_OR_P2             0x6B
#define GSDI_SW2_ADDRESSED_RECORD_OUT_OF_RANGE  0x02
#define GSDI_SW2_INCORRECT_P1P2_NO_EXTRA_DATA   0x00
#define GSDI_SW1_UNKNOWN_INSTRUCTION_CODE       0x6D
#define GSDI_SW1_WRONG_INSTRUCTION_CLASS        0x6E
#define GSDI_SW1_TECH_PROB_NO_DIAG_GIVEN        0x6F

/* 3GPP TS31.101 Section 10.2.1.3                 */
/* Warnings                                       */
#define GSDI_SW1_WARNINGS_PART_1                0x62
#define GSDI_SW2_NO_INFO_VOLATILE_MEM_UNCHANGED 0x00
#define GSDI_SW2_PART_OF_DATA_MAY_BE_CORRUPTED  0x81
#define GSDI_SW2_END_REACHED_BEFORE_LE_BYTES    0x82
#define GSDI_SW2_SELECTED_FILE_INVALIDATED      0x83
#define GSDI_SW2_FCI_NOT_ACCORDING_TO_SPECS     0x84
#define GSDI_SW1_WARNINGS_PART_2                0x63
#define GSDI_SW2_FILE_FILLED_UP_BY_LAST_WRITE   0x81
#define GSDI_SW2_CMD_SUCCESS_AFTER_X_TRYS       0xCF

/* 3GPP TS31.101 Section 10.2.1.4                 */
/* Execution Errors                               */
#define GSDI_SW1_EXECUTION_ERROR_1              0x64
#define GSDI_SW2_NO_INFO_VOL_MEM_NOT_CHANGED    0x00
#define GSDI_SW1_EXECUTION_ERROR_2              0x65
#define GSDI_SW2_NO_INFO_VOL_MEM_CHANGED        0x00
#define GSDI_SW2_EXECUTION_MEMORY_PROBLEM       0x81

/* 3GPP TS31.101 Section 10.2.1.5.1               */
/* Function in CLA Not Supported                  */
#define GSDI_SW1_FUNCTIONS_IN_CLA_ERROR         0x68
#define GSDI_SW2_NO_INFORMATION_GIVEN           0x00
#define GSDI_SW2_LOGICAL_CHANNEL_NOT_SUPPORTED  0x81
#define GSDI_SW2_SECURE_MSG_NOT_SUPPORTED       0x82

/* 3GPP TS31.101 Section 10.2.1.5.2               */
/* Command Not Allowed                            */
#define GSDI_SW1_COMMAND_NOT_ALLOWED            0x69
#define GSDI_SW2_NO_INFO_GIVEN                  0x00
#define GSDI_SW2_CMD_INCOMPATIBLE_W_FILE_STRUCT 0x81
#define GSDI_SW2_SECURITY_STATUS_NOT_SATISFIED  0x82
#define GSDI_SW2_AUTH_PIN_METHOD_BLOCKED        0x83
#define GSDI_SW2_REFERENCE_DATA_INVALIDATED     0x84
#define GSDI_SW2_CNDTNS_OF_USE_NOT_SATISFIED    0x85
#define GSDI_SW2_CMD_NOT_ALLOWD_NO_EF_SELECTED  0x86

/* 3GPP TS31.101 Section 10.2.1.5.3               */
/* Wrong Parameters                               */
#define GSDI_SW1_INCORRECT_PARAMETERS           0x6A
#define GSDI_SW2_IN_THE_DATA_FIELD              0x80
#define GSDI_SW2_FUNCTION_NOT_SUPPORTED         0x81
#define GSDI_SW2_FILE_NOT_FOUND                 0x82
#define GSDI_SW2_RECORD_NOT_FOUND               0x83
#define GSDI_SW2_LC_INCONSISTENT_WITH_TLV       0x85
#define GSDI_SW2_INCORRECT_PARAMS_P1_OR_P2      0x86
#define GSDI_SW2_LC_INCONSISTENT_WITH_P1_OR_P2  0x87
#define GSDI_SW2_REFERENCE_DATA_NOT_FOUND       0x88
#define GSDI_SW2_UNDEFINED_99                   0x99

/* MFLO Specification */
#define GSDI_SW2_UCAST_AUTH_KEY_NOT_INIT         0x01
#define GSDI_SW2_UCAST_DATA_BLOCK_NOT_INIT       0x02
#define GSDI_SW2_VERIFY_UCAST_FAILED             0x04
#define GSDI_SW2_LOCKED_INIT_PARAM               0x05
#define GSDI_SW2_APPLET_NOT_INIT                 0x06
#define GSDI_SW2_PUBLIC_KEY_CERT_NOT_INIT        0x07

/* uim specific */
#define GSDI_SW1_UIM_TIMEOUT                    0x00
#define GSDI_SW2_UIM_TIMEOUT                    0x00

/*Internal to GSDI*/
#define GSDI_SW1_INT_ERROR                      0xFF
#define GSDI_SW2_INT_ERROR                      0xFF

#define MMGSDI_MAX_CHANNEL_DATA_CMD_LEN 261 /* 6 Bytes of APDU Header + 255 Data */

#define MMGSDI_MAX_CHANNEL_DATA_RSP_LEN   300 /* Variable Data + Overhead          */

#define GSDI_EST_FDN_OFFSET         0
#define GSDI_EST_FDN_MASK           0x01
#define GSDI_EST_BDN_OFFSET         0
#define GSDI_EST_BDN_MASK           0x02
#define GSDI_EST_ACL_OFFSET         0
#define GSDI_EST_ACL_MASK           0x04

#define GSDI_PIN1_STATUS_OFFSET     18
#define GSDI_PUK1_STATUS_OFFSET     19
#define GSDI_PIN2_STATUS_OFFSET     20
#define GSDI_PUK2_STATUS_OFFSET     21
#define GSDI_FILE_CHARAC_OFFSET     13     /* 14th byte in dir-info */
#define GSDI_CODE_INIT_MASK         0x80   /* 1000 0000 */
#define GSDI_CHV1_DISABLED_MASK     0x80   /* 1000 0000 */

#define GSDI_RUIM_ESN_USAGE_MASK        0x01
#define GSDI_RUIM_STORE_ESN_LENGTH      0x04
#define GSDI_RUIM_STORE_ESN_USE_UIM_ID  0x10

#define GSDI_FCP_TEMPLATE_TAG            0x62
#define GSDI_FCI_TEMPLATE_TAG            0x6F
#define GSDI_FILE_DESCRIPTOR_TAG         0x82
#define GSDI_FILE_IDENTIFIER_TAG         0x83
#define GSDI_DF_NAME_AID_TAG             0x84
#define GSDI_PROPRIETARY_INFO_TAG        0xA5
#define GSDI_PROPRIETARY_INFO_BUGGY_TAG 0x25
#define GSDI_LIFE_CYCLE_INTEG_TAG        0x8A
#define GSDI_SEC_ATTR_PROP_TAG           0x86
#define GSDI_SEC_ATTR_CMPACT_TAG         0x8C
#define GSDI_SEC_ATTR_EXPAND_TAG         0xAB
#define GSDI_SEC_ATTR_REF_EXP_TAG        0x8B
#define GSDI_FILE_SIZE_TAG               0x80
#define GSDI_ALT_FILE_SIZE_TAG           0xE2
#define GSDI_TOTAL_FILE_SIZE_TAG         0x81
#define GSDI_SHORT_FILE_ID_TAG           0x88
#define GSDI_LEN_128_255_ENCODE_TAG      0x81

#define GSDI_FILE_DESC_SHAREABLE_MASK    0x40

#define GSDI_PIN_STATUS_DO_TAG      0xC6
#define GSDI_PS_DO_TAG              0x90
#define GSDI_KEY_REF_TAG            0x83

#define GSDI_AM_DO_BYTE_TAG         0x80
#define GSDI_AM_DO_INST_TAG         0x84
#define GSDI_SC_DO_OR_TAG           0xA0
#define GSDI_SC_DO_AND_TAG          0xAF
#define GSDI_SC_DO_ALWAYS_TAG       0x90
#define GSDI_SC_DO_NEVER_TAG        0x97
#define GSDI_SC_DO_KEY_REF_TAG      0xA4
#define GSDI_SC_DO_USAGE_QUAL_TAG   0x95

#define GSDI_PS_DO_USAGE_QUAL_LEN          0x01
#define GSDI_USAGE_QUAL_USER_VERIFICATON   0x08

#define SEC_INC_MSK                 0x32
#define SEC_INC_OPERATION           0x32
#define SEC_READ_MSK                0x01
#define SEC_READ_OPERATION          0x01
#define SEC_UPDATE_MSK              0x02
#define SEC_UPDATE_OPERATION        0x02
#define SEC_DEACT_MSK               0x08
#define SEC_DEACT_OPERATION         0x08
#define SEC_ACT_MSK                 0x10
#define SEC_ACT_OPERATION           0x10

#define GSDI_MF_PATH_USED                    0x3F00
#define GSDI_USIM_PATH_USED                  0xFFFF
#define GSDI_TELECOM_PATH_USED               0x7F10
#define GSDI_ADF_PATH_USED                   0x7FFF
#define GSDI_PHONEBOOK_DF_PATH_USED          0x5F3A
#define GSDI_ADN_EF_PATH_USED                0x6F3A
#define GSDI_PBR_EF_PATH_USED                0x4F30

#define GSDI_SW1_OFFSET              0
#define GSDI_SW2_OFFSET              1

#define GSDI_SPT_DATA_MIN_LEN        0x08
#define MMGSDI_RPM_VERSION_DATA_LEN  0x01      /* EF-RPM Version Implemented */
#define MMGSDI_RPM_VERSION           0x02      /* Indicates RPM Version supported */

#define GSDI_RUIM_SF_EUIMID_USAGE_MASK    0x03
#define GSDI_RUIM_STORE_MEID_LENGTH       0x07
#define GSDI_RUIM_STORE_MEID_USE_EUIM_ID  0x10
#define GSDI_RUIM_MEID_UIMID_USAGE_IND    0x01
#define GSDI_RUIM_SF_EUIMID_ESN_USAGE_IND 0x02
#define GSDI_RUIM_SF_EUIMID_USAGE_IND     0x03

/* ----------------------------------------------------------------------------
** MAX Number of Files GSDI can perform File Change Notification for
** ---------------------------------------------------------------------------*/
#define MMGSDI_MAX_FILE_CHANGE_NOTIFICATIONS 120

#define MMGSDI_TASK_NAME_LEN                11

#define MMGSDI_A_KEY_P_REV                  2

/* ----------------------------------------------------------------------------
** MAX Number of FPLMNs supported by MMGSDI cache is 20 for ICC cards
** ---------------------------------------------------------------------------*/
#define MMGSDI_MAX_EXTENDED_ICC_FPLMN_COUNT  20
#define MMGSDI_EXTENDED_ICC_FPLMN_CACHE_LEN  (MMGSDI_MAX_EXTENDED_ICC_FPLMN_COUNT * MMGSDI_PLMN_ID_SIZE)
#define MMGSDI_GSM_FPLMN_SIZE                12

/* ----------------------------------------------------------------------------
   16-bit format UCS2 encoding types
-----------------------------------------------------------------------------*/
#define MMGSDI_UCS2_ENCODING_TYPE_1         0x80
#define MMGSDI_UCS2_ENCODING_TYPE_2         0x81
#define MMGSDI_UCS2_ENCODING_TYPE_3         0x82

/* ----------------------------------------------------------------------------
   Maximum values for data types
-----------------------------------------------------------------------------*/
#define MMGSDI_UINT32_MAX                   0xFFFFFFFF
#define MMGSDI_INT32_MAX                    0x7FFFFFFF

/* Size of PRL Size field */
#define MMGSDI_SIZE_OF_PRL_SIZE             2

/* length of RAND of GSM algorithm */
#define MMGSDI_RAND_LEN                     16

/* size of IMSI digits */
#define MMGSDI_IMSI_DIGIT_LEN               15

/* Type of attributes available */
#define MMGSDI_NUM_ATTRIBUTES               (MMGSDI_MAX_PROV_APPS/2)

/*-----------------------------------------------------------------------------
   DATA TYPE:      MMGSDI_CLIENT_NAME_TYPE

   DESCRIPTION:
     Array storing the name of the Client associated with the current
     task. This name is obtained by using the be rex_self() function.
-----------------------------------------------------------------------------*/
typedef char mmgsdi_client_name_type[MMGSDI_TASK_NAME_LEN+1];

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_TASK_STATE_ENUM_TYPE

   DESCRIPTION:
     The MMGSDI task state enum type
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_TASK_INACTIVE,
  MMGSDI_TASK_ACTIVE,
  MMGSDI_TASK_STOPPED,
  MMGSDI_TASK_ENDED
} mmgsdi_task_state_enum_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_TASK_ENUM_TYPE

   DESCRIPTION:
     The MMGSDI task enum type
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_TASK_MAIN     = 0x00,
  MMGSDI_TASK_SLOT1,
  MMGSDI_TASK_SLOT2,
  MMGSDI_TASK_SLOT3,
  MMGSDI_NUM_TASKS,
  MMGSDI_TASK_MAX      = 0x7f
} mmgsdi_task_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_CMD_ENUM_TYPE

   DESCRIPTION:
     The command enum type that MMGSDI supports
-------------------------------------------------------------------------------*/
typedef enum {
  /* 0 */
  MMGSDI_CLIENT_ID_AND_EVT_REG_REQ,
  MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ,
  MMGSDI_READ_REQ,
  MMGSDI_WRITE_REQ,
  MMGSDI_INCREASE_REQ,
  MMGSDI_GET_FILE_ATTR_REQ,
  MMGSDI_SESSION_OPEN_REQ,
  MMGSDI_SESSION_CLOSE_REQ,
  MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ,
  MMGSDI_SESSION_DEACTIVATE_REQ,

  /* 10 */
  MMGSDI_SESSION_SELECT_AID_REQ,
  MMGSDI_ISIM_AUTH_REQ,
  MMGSDI_SEND_APDU_REQ,
  MMGSDI_SAP_CONNECT_REQ,
  MMGSDI_SAP_DISCONNECT_REQ,
  MMGSDI_SAP_GET_ATR_REQ,
  MMGSDI_SAP_SEND_APDU_REQ,
  MMGSDI_SAP_POWER_ON_REQ,
  MMGSDI_SAP_POWER_OFF_REQ,
  MMGSDI_SAP_CARD_READER_STATUS_REQ,

  /* 20 */
  MMGSDI_SAP_RESET_REQ,
  MMGSDI_CARD_PUP_REQ,
  MMGSDI_CARD_PDOWN_REQ,
  MMGSDI_CARD_STATUS_REQ,
  MMGSDI_REFRESH_REQ,
  MMGSDI_REHABILITATE_REQ,
  MMGSDI_INVALIDATE_REQ,
  MMGSDI_ACTIVATE_ONCHIP_SIM_REQ,
  MMGSDI_PIN_OPERATION_REQ,
  MMGSDI_GET_ALL_PIN_STATUS_REQ,

  /* 30 */
  MMGSDI_SRV_AVAILABLE_REQ,
  MMGSDI_USIM_AUTH_REQ,
  MMGSDI_JCDMA_GET_CARD_INFO_REQ,
  MMGSDI_COMPUTE_IP_AUTH_REQ,
  MMGSDI_RUN_CAVE_REQ,
  MMGSDI_GENERATE_KEY_VPM_REQ,
  MMGSDI_GET_ATR_REQ,
  MMGSDI_CARD_RESET_REQ,
  MMGSDI_BCAST_REQ,
  MMGSDI_SEARCH_REQ,

  /* 40 */
  MMGSDI_SESSION_RUN_GSM_ALGO_REQ,
  MMGSDI_SESSION_ENABLE_SERVICE_REQ,
  MMGSDI_SESSION_DISABLE_SERVICE_REQ,
  MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ,
  MMGSDI_SESSION_STORE_ESN_REQ,
  MMGSDI_SESSION_GET_INFO_REQ,
  MMGSDI_SESSION_GET_CPHS_INFO_REQ,
  MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ,
  MMGSDI_SAP_REPORT_RSP,
  MMGSDI_UIM_REPORT_RSP,

  /* 50 */
  MMGSDI_CLIENT_ID_REG_RSP,
  MMGSDI_GET_ALL_AVAILABLE_APPS_REQ,
  MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ,
  MMGSDI_SESSION_SSD_UPDATE_REQ,
  MMGSDI_SESSION_SSD_CONFIRM_REQ,
  MMGSDI_SESSION_BS_CHAL_REQ,
  MMGSDI_SESSION_OTASP_MS_KEY_REQ,
  MMGSDI_SESSION_OTASP_KEY_GEN_REQ,
  MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ,
  MMGSDI_SESSION_OTASP_COMMIT_REQ,

  /* 60 */
  MMGSDI_SESSION_OTASP_OTAPA_REQ,
  MMGSDI_SESSION_OTASP_BLOCK_REQ,
  MMGSDI_SESSION_READ_PRL_REQ,
  MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ,
  MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ,
  MMGSDI_SESSION_SUB_OK_REQ,
  MMGSDI_TP_DOWNLOAD_COMPLETE_REQ,
  MMGSDI_SESSION_OPEN_EXT_REQ,
  MMGSDI_NOTIFY_LINK_EST_REQ,
  MMGSDI_NOTIFY_CARD_ERROR_REQ,

  /* 70 */
  MMGSDI_INTERNAL_PUP_INIT_REQ,
  MMGSDI_SESSION_GET_OPERATOR_NAME_REQ,
  MMGSDI_EONS_SS_EVENT_REQ,
  MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ,
  MMGSDI_SEND_APDU_EXT_REQ,
  MMGSDI_SESSION_OPEN_WITH_MF_REQ,
  MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ,
  MMGSDI_RECOVERY_IND_REQ,
  MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ,
  MMGSDI_GET_SE13_PLMN_NAMES_REQ,

  /* 80 */
  MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ,
  MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ,
  MMGSDI_SESSION_CDMA_AKA_REQ,
  MMGSDI_NOTIFY_SIM_BUSY_REQ,
  MMGSDI_SET_BUILTIN_PLMN_LIST_REQ,
  MMGSDI_SIMLOCK_RSP,
  MMGSDI_SIMLOCK_EVT,
  MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ,
  MMGSDI_HANDLE_NV_REFRESH_REQ,
  MMGSDI_FULL_SERVICE_REQ,

  /* 90 */
  MMGSDI_MANAGE_CHANNEL_REQ,
  MMGSDI_SET_SIM_PROFILE_REQ,
  MMGSDI_GET_SIM_PROFILE_REQ,
  MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ,
  MMGSDI_CNF,

  MMGSDI_MAX_CMD_ENUM = 0x7FFFFFFF
} mmgsdi_cmd_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_DIR_ENUM_TYPE

   DESCRIPTION:
     The enum contains the directory
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_MF_LEVEL      = 0x3F00,
  MMGSDI_RUIM_LEVEL    = 0x7F25,
  MMGSDI_GSM_LEVEL     = 0x7f20,
  MMGSDI_TELECOM_LEVEL = 0x7f10,
  MMGSDI_ADF_LEVEL     = 0x7fff,
  MMGSDI_DCS1800_LEVEL = 0x7F21,
  MMGSDI_PKCS15_LEVEL  = 0x7F50,
  MMGSDI_DEFAULT_LEVEL = 0xFFFF
} mmgsdi_dir_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_CARD_STATE_ENUM_TYPE

   DESCRIPTION:
     The Card State. Please do not change the order of the states.
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_STATE_NOT_INIT = 0x00000000,
  MMGSDI_STATE_NO_CARD,
  MMGSDI_STATE_CARD_INSERTED,
  MMGSDI_STATE_ENUM_MAX = 0x7FFFFFFF
} mmgsdi_card_state_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_SIMLOCK_STATE_ENUM_TYPE

   DESCRIPTION:
     The SIMLock State
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_SIMLOCK_NOT_VERIFIED,
  MMGSDI_SIMLOCK_VERIFIED,
  MMGSDI_SIMLOCK_UNLOCK_REQUIRED,
  MMGSDI_SIMLOCK_FAILURE
} mmgsdi_simlock_state_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_PIN_ENUM_TYPE

   DESCRIPTION:
     The Card Holder Verification Values (CHVs) or Personal
     Identification Numbers (PINs) for a SIM/USIM/RUIM.
     ADM pins refer to administrative PINs
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_APP1_PIN1           = 0x00000000,
  MMGSDI_APP1_PIN2,
  MMGSDI_APP2_PIN1,
  MMGSDI_APP2_PIN2,
  MMGSDI_APP3_PIN1,
  MMGSDI_APP3_PIN2,
  MMGSDI_APP4_PIN1,
  MMGSDI_APP4_PIN2,
  MMGSDI_APP5_PIN1,
  MMGSDI_APP5_PIN2,
  MMGSDI_APP6_PIN1,
  MMGSDI_APP6_PIN2,
  MMGSDI_APP7_PIN1,
  MMGSDI_APP7_PIN2,
  MMGSDI_APP8_PIN1,
  MMGSDI_APP8_PIN2,
  MMGSDI_SESSION_UNIVERSAL_PIN,
  MMGSDI_SESSION_ADM1,
  MMGSDI_SESSION_ADM2,
  MMGSDI_SESSION_ADM3,
  MMGSDI_SESSION_ADM4,
  MMGSDI_SESSION_ADM5,
  MMGSDI_SESSION_ADM6,
  MMGSDI_SESSION_ADM7,
  MMGSDI_SESSION_ADM8,
  MMGSDI_SESSION_ADM9,
  MMGSDI_SESSION_ADM10,
  MMGSDI_SESSION_ADM11,
  MMGSDI_SESSION_ADM12,
  MMGSDI_SESSION_ADM13,
  MMGSDI_SESSION_ADM14,
  MMGSDI_SESSION_ADM15,
  MMGSDI_SESSION_MAX_PIN_ENUM = 0x7FFFFFFF
} mmgsdi_session_pin_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_CACHE_INIT_ENUM_TYPE

   DESCRIPTION:
     Identifies the current status of the cache member
     MMGSDI_CACHE_NOT_INIT: File not yet attempted to be cached
     MMGSDI_CACHE_INIT: File present in cache
     MMGSDI_CACHE_NOT_FOUND: File attempted to be cached, not present on card
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_CACHE_NOT_INIT   = 0x00000000,
  MMGSDI_CACHE_INIT,
  MMGSDI_CACHE_NOT_FOUND,
  MMGSDI_CACHE_MAX_ENUM   = 0x7FFFFFFF
} mmgsdi_cache_init_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_ACTIVATE_ONCHIP_TLV_ENUM_TYPE

   DESCRIPTION:
     The Card Holder Verification Values (CHVs) or Personal
      Identification Numbers (PINs) for a SIM/USIM/RUIM.
      ADM pins refer to administrative PINs
-------------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_ACT_ONCHIP_TLV_START_TAG          = 0x00,
  MMGSDI_ACT_ONCHIP_AUTH_KEY_TAG           = 0x01,
  MMGSDI_ACT_ONCHIP_IMSI_TAG               = 0x02,
  MMGSDI_ACT_ONCHIP_LOCI_TAG               = 0x03,
  MMGSDI_ACT_ONCHIP_KC_TAG                 = 0x04, /* For SIM only */
  MMGSDI_ACT_ONCHIP_PLMN_TAG               = 0x05, /* For SIM only */
  MMGSDI_ACT_ONCHIP_SST_TAG                = 0x06, /* For SIM only */
  MMGSDI_ACT_ONCHIP_ACC_TAG                = 0x07,
  MMGSDI_ACT_ONCHIP_FPLMN_TAG              = 0x08,
  MMGSDI_ACT_ONCHIP_PHASE_TAG              = 0x09, /* For SIM only */
  MMGSDI_ACT_ONCHIP_AD_TAG                 = 0x0A,
  MMGSDI_ACT_ONCHIP_PLMNSEL_TAG            = 0x0B, /* For SIM only */
  MMGSDI_ACT_ONCHIP_CK_TAG                 = 0x0C, /* Ciphering Key                   */
  MMGSDI_ACT_ONCHIP_IK_TAG                 = 0x0D, /* Integrity Key                   */
  /* All items below can be used for USIM only */
  MMGSDI_ACT_ONCHIP_EPSLOCI_TAG            = 0x0E, /* EPS LOCI                        */
  MMGSDI_ACT_ONCHIP_HPLMNWACT_TAG          = 0x0F, /* HPLMN selector with access tech */
  MMGSDI_ACT_ONCHIP_PLMNWACT_TAG           = 0x10, /* User Controlled PLMN Selector   */
  MMGSDI_ACT_ONCHIP_UST_TAG                = 0x11, /* USIM SERVICE TABLE              */
  MMGSDI_ACT_ONCHIP_EST_TAG                = 0x12, /* USIM ENABLED SERVICE TABLE      */
  MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG   = 0x13, /* Response for USIM authenticate 3G context  */
  MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG  = 0x14, /* Response for USIM authenticate GSM context */
  MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG = 0x15, /* Response for USIM authenticate AUTS        */
  MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG  = 0x16, /* Response for USIM authenticate error       */
  MMGSDI_ACT_ONCHIP_EPSNSC_TAG             = 0x17, /* EPS NAS Security Context        */
  MMGSDI_ACT_ONCHIP_KEYSPS_TAG             = 0x18, /* C and I keys for PS domain      */
  MMGSDI_ACT_ONCHIP_USIM_AUTH_ALGO_TAG     = 0x19, /* USIM authenticate algorithm     */
  MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG = 0x1A, /* USIM authenticate root key      */
  MMGSDI_ACT_ONCHIP_USIM_AUTH_RES_LEN_TAG  = 0x1B, /* USIM authenticate response len  */
  MMGSDI_ACT_ONCHIP_USIM_MILENAGE_OP_TAG   = 0x1C, /* Operator data for milenage      */
  MMGSDI_ACT_ONCHIP_USIM_MILENAGE_SEQ_TAG  = 0x1D, /* Initial seq number for milenage */
  MMGSDI_ACT_ONCHIP_RPLMNACT_TAG           = 0x1E, /* RPLMN */
  MMGSDI_ACT_ONCHIP_KEYS_TAG               = 0x1F, /* C and I keys */
  MMGSDI_ACT_ONCHIP_ICCID_TAG              = 0x20, /* ICCID */
  MMGSDI_ACT_ONCHIP_KCGPRS_TAG             = 0x21, /* KCGPRS file */
  MMGSDI_ACT_ONCHIP_MAX_ENUM_TAG           = 0x7F
}mmgsdi_activate_onchip_tlv_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_ONCHIP_MODE_ENUM_TYPE

   DESCRIPTION:
     Mode for the onchip activation
-------------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_ONCHIP_MODE_SIM              = 0x00,
  MMGSDI_ONCHIP_MODE_USIM,
  MMGSDI_ONCHIP_MODE_MAX_ENUM         = 0x7F
}mmgsdi_onchip_mode_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_ONCHIP_AUTH_ALGO_ENUM_TYPE

   DESCRIPTION:
     Mode for the onchip activation
-------------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_ONCHIP_AUTH_ALGORITHM_FIXED    = 0x00,
  MMGSDI_ONCHIP_AUTH_ALGORITHM_XOR,
  MMGSDI_ONCHIP_AUTH_ALGORITHM_MILENAGE,
  MMGSDI_ONCHIP_AUTH_ALGORITHM_MAX_ENUM = 0x7F
}mmgsdi_onchip_auth_algo_enum_type;

typedef enum {
    MMGSDI_KEYREF_ALWAYS = 0x00,
    MMGSDI_PIN_APPL_1 = 0x01,
    MMGSDI_PIN_APPL_2 = 0x02,
    MMGSDI_PIN_APPL_3 = 0x03,
    MMGSDI_PIN_APPL_4 = 0x04,
    MMGSDI_PIN_APPL_5 = 0x05,
    MMGSDI_PIN_APPL_6 = 0x06,
    MMGSDI_PIN_APPL_7 = 0x07,
    MMGSDI_PIN_APPL_8 = 0x08,
    MMGSDI_UNIVERSAL_PIN_KEY = 0x11,
    MMGSDI_SEC_PIN_APPL_1 = 0x81,
    MMGSDI_SEC_PIN_APPL_2 = 0x82,
    MMGSDI_SEC_PIN_APPL_3 = 0x83,
    MMGSDI_SEC_PIN_APPL_4 = 0x84,
    MMGSDI_SEC_PIN_APPL_5 = 0x85,
    MMGSDI_SEC_PIN_APPL_6 = 0x86,
    MMGSDI_SEC_PIN_APPL_7 = 0x87,
    MMGSDI_SEC_PIN_APPL_8 = 0x88,
    MMGSDI_CHV1_KEY       = 0xFD, /*Not from 102.221*/
    MMGSDI_CHV2_KEY       = 0xFE, /*Not from 102.221*/
    MMGSDI_NEVER          = 0xFF
}mmgsdi_key_ref_values_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_REFRESH_STATE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the current state of refresh
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_REFRESH_READY_STATE,
  MMGSDI_REFRESH_WAIT_FOR_OK_TO_INIT_STATE,
  MMGSDI_REFRESH_WAIT_FOR_OK_TO_FCN_STATE,
  MMGSDI_REFRESH_WAIT_FOR_OK_TO_PROCEED_WITH_REFRESH_STATE,
  MMGSDI_REFRESH_COMP_IN_PROGRESS_STATE,
  MMGSDI_REFRESH_WAIT_FOR_INIT_COMPLETE_STATE,
  MMGSDI_REFRESH_WAIT_FOR_FCN_COMPLETE_STATE,
  MMGSDI_REFRESH_STEERING_IN_PROGRESS_STATE,
  MMGSDI_REFRESH_MAX_STATE = 0x7FFFFFFF
} mmgsdi_refresh_state_enum_type;

/*===========================================================================
  ENUM:   MMGSDI_SRV_STATE_ENUM_TYPE
===========================================================================*/
/** Generic Status of services.

    This enumeration is used to maintain the state of the services:
    FDN, BDN, CC, ACL
*/
typedef enum {
  MMGSDI_SRV_NOT_INIT,  /* Service is Not Initialized. */
  MMGSDI_SRV_INIT,      /* Service is Initialized. */
  MMGSDI_SRV_ENABLE,    /* Service is Enabled */
  MMGSDI_SRV_DISABLE,   /* Service is Disabled */
  MMGSDI_MAX_FDN_STATE_ENUM = 0x7FFFFFFF
  /**< Forces an initialize size enumeration. */
 }mmgsdi_srv_state_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_TP_STATE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the current state of TP procedure
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_TP_STATE_NOT_DONE,          /* TP not done */
  MMGSDI_TP_STATE_IN_PROGRESS_OK,    /* TP procesdure is in progress with TP needed */
  MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK,/* TP procesdure is in progress with TP not needed */
  MMGSDI_TP_STATE_NOT_NEEDED,        /* TP not needed */
  MMGSDI_TP_STATE_DONE,              /* TP Completed */
  MMGSDI_MAX_TP_STATE = 0x7FFF
} mmgsdi_tp_state_enum_type;

/* ----------------------------------------------------------------------------
  ENUM:      MMGSDI_CARD_RESET_CAUSE_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the Card Reset cause
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_RESET_CAUSE_GENERAL  = 0x00000000,
                        /* Default Card reset cause */
  MMGSDI_RESET_CAUSE_REFRESH,
                        /* Card Reset triggered due to Refresh */
  MMGSDI_RESET_CAUSE_PROFILE_SWITCH,
                        /* Card Reset triggered due to profile switch */
}mmgsdi_card_reset_cause_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_PROFILE_CARD_SWITCH_ACTION_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the switch action required for the card to switch the profile
-------------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_MULTI_PROFILE_CARD_SWITCH_ACTION_UICC_RESET = 0x00,
                         /* Switch action is UICC Reset */
  MMGSDI_MULTI_PROFILE_CARD_SWITCH_ACTION_REFRESH,
                         /* Switch action is REFRESH */
  MMGSDI_MULTI_PROFILE_CARD_SWITCH_ACTION_NONE
                         /* No switch action */
}mmgsdi_profile_card_switch_action_enum_type;

/* --------------------------------------------------------------
   ENUM:      MMGSDI_CHANNEL_STATE_ENUM_TYPE

   DESCRIPTION:
     Mmgsdi logical channel state enumerator. Channel states are used during
     recovery to determine that channel is required to be recovered or not.
     If a channel is only opened with non provisioning session on UICC and
     APDU  streaming has been done, channel will not be recovered. All other
     channels will be recovered on UICC. Only Default channel will be recovered
     on ICC card.
---------------------------------------------------------------*/
typedef enum {
  MMGSDI_CHANNEL_STATE_NONE,
  MMGSDI_CHANNEL_STATE_NON_PROV_ACT_IN_PROG, /*activation is in progress*/
  MMGSDI_CHANNEL_STATE_READY, /*channel is ready to use*/
  MMGSDI_CHANNEL_STATE_APDU_STREAMING_DONE, /*streaming is done*/
  MMGSDI_CHANNEL_STATE_NON_PROV_DEACT_IN_PROG, /*deactivation is in progress*/
  MMGSDI_CHANNEL_STATE_MAX_ENUM = 0x7FFFFFFF
} mmgsdi_channel_state_enum_type;

/* ----------------------------------------------------------------------------
  ENUM:      MMGSDI_USIM_SPEC_VERSION_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the USIM spec. Version
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_VERSION_UNKNOWN = 0x00000000,
  MMGSDI_SPEC_VERSION_R99_2000_07,
  MMGSDI_SPEC_VERSION_R99_2000_09,
  MMGSDI_SPEC_VERSION_R99_2000_12,
  MMGSDI_SEPC_VERSION_MAX = 0x7FFFFFFF
}mmgsdi_usim_spec_version_enum_type;

/* ----------------------------------------------------------------------------
  ENUM:      MMGSDI_UHZI_VERSION_ENUM_TYPE

   DESCRIPTION:
     This enum indicates the Homezone Version
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_UHZI_NONE,
  MMGSDI_UHZI_V1,
  MMGSDI_UHZI_V2,
  MMGSDI_UHZI_VERSION_MAX_VALUE = 0x7FFFFFFF
}mmgsdi_uhzi_version_enum_type;

/* --------------------------------------------------------------------------
   ENUM:      MMGSDI_NUM_RETRY_ENUM_TYPE

   DESCRIPTION:
     The request to get the number of retries or the
     number of unblock retries.
---------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_RETRY_NONE,
  MMGSDI_GET_NUM_RETRIES,
  MMGSDI_GET_NUM_UNBLOCK_RETRIES,
  MMGSDI_INTERNAL_VERIFY_AFTER_PIN_CHANGE
}mmgsdi_num_retry_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_SESSION_ID_STATUS_ENUM_TYPE

   DESCRIPTION:
     This structure is used when returning the status of a particular
     Session ID
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE,
  MMGSDI_SESSION_ID_ASSIGNED_NOT_ACTIVE,
  MMGSDI_SESSION_ID_UNASSIGNED,
  MMGSDI_SESSION_ID_MAX_STATUS_TYPE = 0x7FFFFFFF
} mmgsdi_session_id_status_enum_type;

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_DATA_FROM_ENUM_TYPE

   DESCRIPTION:
     Indicates whether data to the cache is coming from the card, the client,
     or the cache is not ready. For data coming from the card, the cache can
     only be initalized for a complete file. Incomplete or partial data can be
     written to the cache after that files is initalized in the cache.
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_DATA_FROM_CARD_COMPLETE,
  MMGSDI_DATA_FROM_CARD_INCOMPLETE,
  MMGSDI_DATA_FROM_CLIENT,
  MMGSDI_DATA_NOT_FOUND,
  MMGSDI_DATA_FROM_MAX = 0x7FFFFFFF
} mmgsdi_data_from_enum_type;

typedef enum {
  ROOT     = MMGSDI_EF_UNDER_MF_START,
  CDMA     = MMGSDI_RUIM_EF_START,
  GSM      = MMGSDI_GSM_EF_START,
  TELECOM  = MMGSDI_TELECOM_EF_START,
  USIM     = MMGSDI_USIM_EF_START,
  DCS1800  = MMGSDI_DCS1800_EF_START,
 /// ORANGE   = MMGSDI_ORANGE_EF_START,
 /// CINGULAR = MMGSDI_CINGULAR_EF_START,
  HZI      = MMGSDI_HZI_EF_START,
  BY_PATH  = MMGSDI_EF_PATH_START,
  CSIM     = MMGSDI_CSIM_EF_START,
  ISIM     = MMGSDI_ISIM_EF_START
}mmgsdi_uim_ef_item_category_enum_type;


typedef enum {
  MMGSDI_SC_DO_ALWAYS_TAG  = 0x90,
  MMGSDI_SC_DO_NEVER_TAG     = 0x97,
  MMGSDI_SC_DO_KEY_REF_TAG = 0xA4
}mmgsdi_sc_do_tags_eum_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_EONS_METHOD_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the EONS method
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_EONS_METHOD_NONE,
  MMGSDI_EONS_METHOD_OPL_PNN_LOOKUP,
  MMGSDI_EONS_METHOD_CPHS_ONS_LOOKUP,
  MMGSDI_EONS_METHOD_NITZ_LOOKUP,
  MMGSDI_EONS_METHOD_STATIC_TABLE_LOOKUP,
  MMGSDI_EONS_METHOD_MNC_MCC_STR,
  MMGSDI_EONS_METHOD_CSG,
  MMGSDI_EONS_METHOD_MAX
} mmgsdi_eons_method_enum_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_TASKS_TYPE

   DESCRIPTION:
     The enum for various modem tasks that MMGSDI interacts with
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_MMGSDI_TYPE     = 0, /**< MMGDSI itself. */
  MMGSDI_GSTK_TYPE,           /**< GSTK task. */
  MMGSDI_QMI_MODEM_TYPE,      /**< QMI Modem task. */
  MMGSDI_CM_TYPE,             /**< CM task. */
  MMGSDI_RRC_TYPE,            /**< RRC task. */
  MMGSDI_TASK_TYPE_MAX   = 0x7FFFFFFF
} mmgsdi_tasks_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_CANCEL_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the type of cancel request
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_CANCEL_USING_CLIENT_ID,
  MMGSDI_CANCEL_USING_SESSION_ID
} mmgsdi_cancel_enum_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_UIM_RSP_STATUS_TYPE

   DESCRIPTION:
     Tracks if UIM has provided rsp for this client_request_table entry
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_UIM_RSP_INBOUND       = 0, /**< Rsp expected (Either cmd is being processed
                                         OR rsp is on queue, waiting to be processed */
  MMGSDI_UIM_RSP_PROCESSING,        /**< Rsp is on queue, currently being processed */
  MMGSDI_UIM_RSP_NOT_RECEIVED       /**< Rsp was not received */
} mmgsdi_uim_rsp_status_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_TECHNOLOGY_ENUM_TYPE

   DESCRIPTION:
     This enum identifies 3gpp or 3gpp2 technology
-------------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_TECH_3GPP       = 0x00, /**< Identifies 3gpp (USIM/SIM) type */
  MMGSDI_TECH_3GPP2      = 0x01, /**< Identifies 3gpp2 (CSIM/RUIM) type */
  MMGSDI_TECH_UNKNOWN    = 0x02
} mmgsdi_technology_enum_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_SIM_PHASE_ENUM_TYPE

   DESCRIPTION:
     This enum stores the phase of the sim which comes from EF-phase.
-------------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_SIM_PHASE_NOT_INIT, /**< EF-Phase not read yet */
  MMGSDI_SIM_PHASE_1,
  MMGSDI_SIM_PHASE_2,
  MMGSDI_SIM_PHASE_2_PLUS    /**< EF-Phase indicates phase 2+ or greater */
} mmgsdi_sim_phase_enum_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_CHK_SRV_ENUM_TYPE

   DESCRIPTION:
     This enum indicates if access to the card is allowed to check srv status.
-------------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_CHK_SRV_FROM_CACHE_ONLY,
  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE
}mmgsdi_chk_srv_enum_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_ATTRIBUTE_ENUM_TYPE

   DESCRIPTION:
     This enum identifies Primary, Secondary and Tertiary attributes
-------------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_ATTR_PRIMARY       = 0x00,  /**< Identifies Primary type */
  MMGSDI_ATTR_SECONDARY     = 0x01,  /**< Identifies Secondary type */
  MMGSDI_ATTR_TERTIARY      = 0x02,  /**< Identifies Tertiary type */
  MMGSDI_ATTR_UNKNOWN       = 0x03
} mmgsdi_attribute_enum_type;

/* ----------------------------------------------------------------------------

   ENUM:      MMGSDI_SW_STATUS_ENUM_TYPE

   DESCRIPTION:
     This enum tells whether SW is populated from cmd_transacted provided in
     UIMDRV response or not.
-------------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_STATUS_WORD_PRESENT   = 0x00,    /* Command is transacted by card */
  MMGSDI_STATUS_WORD_ABSENT,              /* Command is not transacted by card */
  MMGSDI_STATUS_WORD_INVALID   = 0x7F     /* cmd_transacted value not taken from UIMDRV response */
} mmgsdi_sw_status_enum_type;


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCT:      MMGSDI_APDU_DATA_GET_RSP_CACHE

   DESCRIPTION:
     This cache will maintain the "Get Response" Buffer which is normally
     done by our UIM Drivers to ensure we don't lose the Get Response Data
     and at the same time still maintain an Active representation of the
     APDU Exchange Protocol as defined in 7816-3.

   USAGE:
     This cache is only active for a Client when sending APDUs. If the client
     sends an APDU Command resulting in a GET RESPONSE, i.e. intermediate Status
     Words: 61XX/9FXX, the GET Response data is stored.
     The client must issue a Get Response APDU as the subsequent APDU to retrieve
     the data stored.  If the subsequent APDU from the client on the logical
     channel is not a GET Response APDU, the current cache will be cleared.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type     client_id;
  mmgsdi_slot_id_enum_type  slot_id;
  uint8                     channel_id;
  uint8                     implicit_sw1;
  mmgsdi_data_type          apdu;
}mmgsdi_apdu_data_get_rsp_cache_type;

typedef struct {
  mmgsdi_file_enum_type EF_name;
  mmgsdi_len_type       rec_num;
  mmgsdi_data_type      record_data;
} mmgsdi_record_cache_type;

typedef struct mmgsdi_record_cache_node {
  struct mmgsdi_record_cache_node     *next_p;
  mmgsdi_record_cache_type             record_data;
}mmgsdi_record_cache_node_type;

typedef struct {
  mmgsdi_record_cache_node_type * head_p;
  mmgsdi_record_cache_node_type * tail_p;
}mmgsdi_record_cache_node_list_type;

typedef struct {
  mmgsdi_cache_init_enum_type  init;
  mmgsdi_len_type              total_len;
  uint8                       *data_ptr;
  boolean                      in_synch;
  uint8                        no_of_records;
}mmgsdi_cache_element_type;

typedef struct {
  mmgsdi_cache_init_enum_type  init;
  mmgsdi_rec_num_type          record_number;
  uint8                       *data_ptr; /*record data*/
}mmgsdi_cache_record_data_type;

typedef struct {
  mmgsdi_cache_init_enum_type    init;
  mmgsdi_record_ef_info_type     record_ef_info;
  mmgsdi_cache_record_data_type *record_list_ptr;
  uint32                         cache_ref_cnt;
}mmgsdi_cache_record_ef_data_type;

typedef struct mmgsdi_attr_cache_element{
  mmgsdi_access_type                      access;
  mmgsdi_file_structure_enum_type         file_type;
  mmgsdi_file_security_type               read;
  mmgsdi_file_security_type               write;
  boolean                                 cyclic_increase_allowed;
  mmgsdi_file_security_type               increase;
  mmgsdi_file_size_type                   file_size;
  mmgsdi_rec_num_type                     num_of_rec;
  mmgsdi_len_type                         rec_len;
  uint8                                   sfi;  /* Short File Indicator for the file,
                                                   if supported */
  struct mmgsdi_attr_cache_element       *next_ptr;
}mmgsdi_attr_cache_element_type;

typedef struct {
  mmgsdi_attr_cache_element_type  *item_ptr;
}mmgsdi_attr_cache_type;

typedef struct{
  mmgsdi_key_ref_values_enum_type   key_ref;
  mmgsdi_pin_enum_type              pin_id;
  mmgsdi_pin_status_enum_type       status;
  mmgsdi_pin_replace_enum_type      pin_replacement;
  uint32                            num_retries;
  uint32                            num_unblock_retries;
  mmgsdi_static_data_type           pin_data;
  boolean                           valid_pin; /*the PIN Data may not be valid*/
  boolean                           cached;
  boolean                           valid_num_retries;
  boolean                           valid_num_unblock_retries;
}mmgsdi_app_pin_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ONCHIP_AUTO_ACTIVATION_DATA_TYPE

   DESCRIPTION:
     The structure contains the global data for the ONCHIP auto activation
     as read from the onchip auto activation EFS item.
-------------------------------------------------------------------------------*/
typedef struct {
  /* Whether Onchip should be auto-activated by MMGSDI or not */
  boolean                                 auto_activation_enabled;
  /* Whether for SIM or for USIM */
  mmgsdi_app_enum_type                    app_type;
} mmgsdi_onchip_auto_activation_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ONCHIP_SIM_DATA_TYPE

   DESCRIPTION:
     The structure contains the global data for the ONCHIP SIM.  Only useful
     when active.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_onchip_sim_state_enum_type        state;
  mmgsdi_app_enum_type                     app_type;
  uint32                                   sel_aid_index;
  uint8                                    auth_key[MMGSDI_ONCHIP_SIM_AUTH_KEY_LEN];
  mmgsdi_data_type                         usim_auth_3g_rsp;
  mmgsdi_data_type                         usim_auth_gsm_rsp;
  mmgsdi_data_type                         usim_auth_auts_rsp;
  uint8                                    usim_auth_error_rsp[2];
  mmgsdi_onchip_auth_algo_enum_type        usim_auth_algo;
  uint8                                    usim_auth_algo_res_length;
  mmgsdi_data_type                         usim_auth_algo_root_key;
  uint64                                   usim_auth_milenage_seq;
  mmgsdi_data_type                         usim_auth_milenage_op_data;
} mmgsdi_onchip_sim_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_APP_CAPABILITIES_TYPE

   DESCRIPTION:
     The structure contains the information of the services available
     on the card.

     fdn_state:         FDN is enabled or not
     bdn_state:         BDN is enabled or not
     cc_state:          CC is enabled or not
     acl_state:         ACL is enabled or not
     sim_phone_book:    Entries within GSM phonebook
     usim_global_phone_book:
                        Entries within USIM Global
     usim_local_phone_book:
                        Entries within USIM Local
     imsi_invalidated:  IMSI is validated or not
     all_srv_init:      Set to TRUE if all the services are enabled.
-------------------------------------------------------------------------------*/

typedef struct{
  mmgsdi_srv_state_enum_type   fdn_state;
  mmgsdi_srv_state_enum_type   bdn_state;
  mmgsdi_srv_state_enum_type   cc_state;
  mmgsdi_srv_state_enum_type   acl_state;
  boolean                      imsi_invalidated;
  boolean                      all_srv_init;
  boolean                      unknown_capabilities;
}mmgsdi_app_capabilities_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PERSO_INFO_TYPE

   DESCRIPTION:
     The structure contains the information of the Personalization
     information to be used for the Late Clients.

     feature:             Personalization Feature
     num_retries:         Number of retries Left
     num_unblock_retries: Number of unblock retries Left
-------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_perso_feature_enum_type  feature;
  uint32                          num_retries;
  uint32                          num_unblock_retries;
 }mmgsdi_perso_info_type;

typedef struct{
  boolean                         adf_shareable;
  uint32                          total_file_size;
  mmgsdi_file_status_type         adf_status;
  boolean                         info_valid;
}mmgsdi_adf_attributes_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_NITZ_DATA_TYPE

   DESCRIPTION:
     Data Structure to hold RPLMN ID, LAC, CSG info and NITZ name received from
     the latest serving system event
-----------------------------------------------------------------------------*/
typedef struct{
  boolean                long_name_available;
  boolean                short_name_available;
  mmgsdi_plmn_name_type  long_name;
  mmgsdi_plmn_name_type  short_name;
  boolean                rplmn_available;
  mmgsdi_plmn_id_type    rplmn_id;
  boolean                lac_available;
  uint16                 lac;
  sys_csg_info_s_type    csg_info;
}mmgsdi_eons_nitz_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_PER_STACK_SS_DATA_TYPE

   DESCRIPTION:
     Data Structure to hold serving system info per call stack
-----------------------------------------------------------------------------*/
typedef struct{
  boolean                        is_valid;
  mmgsdi_eons_nitz_data_type     nitz_info;
  sys_sys_mode_e_type            sys_mode;
  sys_roam_status_e_type         roam_status;
  sys_srv_status_e_type          srv_status;
}mmgsdi_eons_per_stack_ss_data_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_SS_DATA_TYPE

   DESCRIPTION:
     Data Structure to hold serving system info for all supported call stacks
-----------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_session_type_enum_type       session_type;
  mmgsdi_eons_per_stack_ss_data_type  first_stack_ss;
  mmgsdi_eons_per_stack_ss_data_type  sec_stack_ss;
}mmgsdi_eons_ss_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_PER_STACK_RPLMN_INFO_TYPE

   DESCRIPTION:
     Data Structure to hold current RPLMN ID, LAC, CSG info and name determined
     by running the EONS algo on the RPLMN ID+LAC or CSG info
-----------------------------------------------------------------------------*/
typedef struct{
  boolean                             rplmn_available;
  mmgsdi_plmn_id_type                 rplmn_id;
  boolean                             lac_available;
  uint16                              lac;
  boolean                             is_home_network; /*3GPP2: SYS ROAM STATUS is OFF
                                                         3GPP: RPLMN matches with HPLMN or SPDI list*/
  boolean                             full_srv_evt_sent;
  sys_srv_status_e_type               srv_status;
  mmgsdi_eons_method_enum_type        eons_method;
  mmgsdi_eons_name_source_enum_type   rplmn_name_source;
  boolean                             display_operator_info;
  boolean                             display_spn_info;
  boolean                             long_name_available;
  boolean                             short_name_available;
  mmgsdi_plmn_name_type               long_name;
  mmgsdi_plmn_name_type               short_name;
  sys_csg_info_s_type                 csg_info;
  mmgsdi_plmn_additional_info_type    additional_info;
  uint8                               num_additional_names;
  mmgsdi_plmn_additional_names_type   additional_names[MMGSDI_ADDITIONAL_NAMES_MAX];
}mmgsdi_eons_per_stack_rplmn_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_RPLMN_INFO_TYPE

   DESCRIPTION:
     Data Structure to hold RPLMN info for all supported call stacks
-------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_eons_per_stack_rplmn_info_type  first_stack_rplmn;
  mmgsdi_eons_per_stack_rplmn_info_type  sec_stack_rplmn;
}mmgsdi_eons_rplmn_info_type;

/*------------------------------------------------------------------------------
  STRUCTURE  MMGSDI_EONS_INFO_TYPE

  DESCRIPTION:
    Define a type that contains EONS related data
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_eons_name_type                   spn;
  boolean                                 display_spn;
  uint8                                   lang_ind;
  mmgsdi_eons_spn_display_bit_enum_type   spn_display_bit;
  mmgsdi_eons_rplmn_display_bit_enum_type rplmn_display_bit;
  mmgsdi_eons_ss_data_type                ss_info;
  mmgsdi_eons_rplmn_info_type             rplmn_info;
  boolean                                 is_get_file_attr_acsgl_done;
  mmgsdi_rec_num_type                     num_of_acsgl_rec;
  mmgsdi_len_type                         len_of_acsgl_rec;
  boolean                                 is_get_file_attr_csgt_done;
  mmgsdi_rec_num_type                     num_of_csgt_rec;
  mmgsdi_len_type                         len_of_csgt_rec;
  boolean                                 is_get_file_attr_hnbn_done;
  mmgsdi_rec_num_type                     num_of_hnbn_rec;
  mmgsdi_len_type                         len_of_hnbn_rec;
  boolean                                 is_get_file_attr_ocsgl_done;
  mmgsdi_rec_num_type                     num_of_ocsgl_rec;
  mmgsdi_len_type                         len_of_ocsgl_rec;
  boolean                                 is_get_file_attr_ocsgt_done;
  mmgsdi_rec_num_type                     num_of_ocsgt_rec;
  mmgsdi_len_type                         len_of_ocsgt_rec;
  boolean                                 is_get_file_attr_ohnbn_done;
  mmgsdi_rec_num_type                     num_of_ohnbn_rec;
  mmgsdi_len_type                         len_of_ohnbn_rec;
}mmgsdi_eons_info_type;

/*------------------------------------------------------------------------------
  STRUCTURE  MMGSDI_EONS_DISPLAY_INFO_TYPE

  DESCRIPTION:
    Define a type that contains rplmn display bit, spn display bit and
    network's home and roaming status
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_eons_spn_display_bit_enum_type   spn_display_bit;
  mmgsdi_eons_rplmn_display_bit_enum_type rplmn_display_bit;
  mmgsdi_eons_roaming_status_enum_type    roaming_status;
}mmgsdi_eons_display_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INT_APP_INFO_TYPE

   DESCRIPTION:
     The structure contains the application information on the card.
     app_selected:             Whether this is a provisoning application or not
     app_state:                Indicates the initialization state of this application.
                               Valid states for Prov Application include:
                                 MMGSDI_APP_STATE_UNKNOWN
                                 MMGSDI_APP_STATE_DETECTED
                                 MMGSDI_APP_STATE_ACTIVATED
                                 MMGSDI_APP_STATE_PIN_EVT_SENT
                                 MMGSDI_APP_STATE_PERSO_VERIFIED
                                 MMGSDI_APP_STATE_READY
                               Valid states for Non Prov Application include:
                                 MMGSDI_APP_STATE_UNKNOWN
                                 MMGSDI_APP_STATE_DETECTED
                                 MMGSDI_APP_STATE_ACTIVATED
                                 MMGSDI_APP_STATE_READY
     perso_state:               Indicates the personalization state
                                When perso state becomes
                                MMGSDI_PERSO_ENG_INIT_COMPLETED_S app state would also
                                change to MMGSDI_APP_STATE_PERSO_VERIFIED
     proprietary_app:           Whether this application is proprietary or not
     pin1_ptr:                  PIN1 information, pointing to mmgsdi_app_pin1_table
     pin2:                      PIN2 information.  Each application has its own PIN2
     universal_pin_ptr:         UPIN information, pointing to mmgsdi_app_univ_pin_table
                                when this application's PIN1 is being replaced by UPIN
     app_data:                  Application ID, Label
     perso_ability:             It contains app perso ability, It will be initialized to
                                MMGSDI_PERSO_NOT_APPLICABLE, MMGSDI_PERSO_SIM_ALLOWED, or
                                MMGSDI_PERSO_RUIM_ALLOWED, depending on the
                                application and card capability.
     hiddenkey:                 The status of the Hidden Key for the application
                                described
     dir_index:                 The record number in EF-DIR corresponding to the
                                application described
     app_capabilities:          This contains the information of the services available
                                on the card.
     perso_info:                Contains the personalization information to be used for
                                Late clients.
     usim_spec_version:         If the application is a USIM, the spec. version
                                corresponding to the application
     hzi_version:               Homezone version
     gprs_anite_mcc:            Indicates if the MCC of the application matches
                                a reserved Anite set
     tds_test_imsi:             Indicates if the IMSI of the application matches
                                a reserved TDS test SIM set
     app_not_in_efdir:          Indicates that the app is not listed in EF-DIR
     adf_attributes:            Attributes of ADF returned in SELECT response
     app_tp_state:              Application TP state
     aid_in_fcp:                AID in SELECT response
     session_closed_cause:      Indicates the session closed cause
     status_app_is_initialized: Indicates if STATUS with app initialized is sent
     service_status:            Current network status of application
     pre_perso_files_cached:    Indicates if the files in pre-perso list are cached
-------------------------------------------------------------------------------*/
typedef struct{
  boolean                               app_selected;
  mmgsdi_app_state_enum_type            app_state;
  mmgsdi_simlock_state_enum_type        perso_state;
  boolean                               proprietary_app;
  mmgsdi_app_pin_info_type             *pin1_ptr;
  mmgsdi_app_pin_info_type              pin2;
  boolean                               fcp_pin_data_absent;
  mmgsdi_app_pin_info_type             *universal_pin_ptr;
  mmgsdi_aid_type                       app_data;
  byte                                  perso_ability;
  mmgsdi_app_pin_info_type              hiddenkey;
  uint8                                 dir_index;
  mmgsdi_app_capabilities_type          app_capabilities;
  mmgsdi_perso_info_type                perso_info;
  mmgsdi_eons_info_type                 eons_info;
  mmgsdi_usim_spec_version_enum_type    usim_spec_version;
  mmgsdi_uhzi_version_enum_type         hzi_version;
  boolean                               gprs_anite_mcc;
  boolean                               tds_test_imsi;
  boolean                               app_not_in_efdir;
  mmgsdi_adf_attributes_type            adf_attributes;
  mmgsdi_tp_state_enum_type             app_tp_state;
  mmgsdi_static_data_type               aid_in_fcp;
  mmgsdi_session_closed_cause_enum_type session_closed_cause;
  boolean                               nv_refresh_in_progress;
  boolean                               status_app_is_initialized;
  simlock_app_service_status_enum_type  service_status;
  boolean                               pre_perso_files_cached;
  boolean                               deactivation_in_progress;
}mmgsdi_int_app_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ME_CAPABILITY_TYPE

   DESCRIPTION:
     The structure contains the me capabilities to support fdn, bdn & acl.
-------------------------------------------------------------------------------*/
typedef struct {
  boolean   me_supports_fdn;
  boolean   me_supports_bdn;
  boolean   me_supports_acl;
}mmgsdi_me_capability_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REHAB_INFO_TYPE

   DESCRIPTION:
     The structure contains the information of files rehabilitated on the slot.

     imsi_rehabilitated:    IMSI rehabilitated or not
    loci_rehabilitated:     LOCI rehabilitated or not
     cdma_fdn_files_rehab:  CDMA FDN files rehabilitated or not
-------------------------------------------------------------------------------*/
typedef struct {
  boolean  imsi_rehabilitated;
  boolean  loci_rehabilitated;
#ifdef FEATURE_MMGSDI_3GPP2
  boolean  cdma_fdn_files_rehab[MMGSDI_CDMA_FDN_FILES];
#endif /* FEATURE_MMGSDI_3GPP2 */
}mmgsdi_rehab_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ATR_DATA_TYPE

   DESCRIPTION:
     This structure contains the ATR information.
     atr_length: Number of bytes in ATR
     atr_data:   ATR data
-------------------------------------------------------------------------------*/
typedef struct {
  uint8 atr_length;
  uint8 atr_data[MMGSDI_MAX_ATR_LEN];
} mmgsdi_atr_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_HIST_CHAR_DATA_TYPE

   DESCRIPTION:
     This structure contains the History characters information.
     hist_char_length: Number of bytes in History characters
     hist_char_data:   History characters
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8 hist_char_length;
  uint8 hist_char_data[MMGSDI_MAX_HIST_CHAR_LEN];
} mmgsdi_hist_char_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ICCID_INFO_TYPE

   DESCRIPTION:
     This structure contains the ICCID info as received from uimdrv.
     length:    Number of ICCID bytes
     data:      ICCID
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8 length;
  uint8 data[MMGSDI_ICCID_LEN];
} mmgsdi_iccid_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_IMSI_DATA_TYPE

   DESCRIPTION:
     This structure contains MMGSDI_MAX_IMSI_DIGIT_LEN digits of IMSI
-------------------------------------------------------------------------- */
typedef struct{
  uint8 d[MMGSDI_IMSI_DIGIT_LEN];
} mmgsdi_imsi_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PARSED_ATR_TYPE

   DESCRIPTION:
     Results of Parsing ATR Data
-------------------------------------------------------------------------------*/
typedef struct {
  boolean    card_services_data_present;
  boolean    select_by_full_df_name;
  boolean    select_by_part_df_name;
  boolean    aid_available_in_dir;
  boolean    aid_available_in_atr;
  uint8      total_logical_channels;
}mmgsdi_parsed_atr_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SW_STATUS_TYPE

   DESCRIPTION:
     The structure contains the sw1 and sw2 returned
-------------------------------------------------------------------------------*/
typedef struct {
  uint8        sw1;
  uint8        sw2;
} mmgsdi_sw_status_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_AID_SELECT_FAILED_NODE_TYPE

   DESCRIPTION:
     Contains AID and SW for the apps on which SELECT failed at least once
-------------------------------------------------------------------------------*/
typedef struct mmgsdi_aid_select_failed_node {
  mmgsdi_aid_type                          app_info;
  mmgsdi_sw_status_type                    sw;
  struct mmgsdi_aid_select_failed_node    *next_ptr;
}mmgsdi_aid_select_failed_node_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CALLBACK_CNF_TYPE

   DESCRIPTION:
     Confirmation to client task command type

-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_callback_type            client_req_cb;
  mmgsdi_cnf_type                 mmgsdi_cnf;
} mmgsdi_callback_cnf_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SLOT_DATA_TYPE

   DESCRIPTION:
     The structure contains the global MMGSDI data for slot
     mmgsdi_state:        MMGSDI card state
     mmgsdi_sap_state:    SAP state
     app_info_ptr:        Pointer to application info including PIN
     prov_app_indices:    Index locations for the provisioning applications.
                          Format as follow:
                            b8 - Indicates whether the index has valid location
                                 or not.  1 indicates valid, 0 indicates invalid
                            b7 - b1 - Index location
                          prov_app_indices[MMGSDI_PRI_GW_PROV_APP_SEL_INDEX]:
                            GW Primary Provisioning
                          prov_app_indices[MMGSDI_PRI_1X_PROV_APP_SEL_INDEX]:
                            1X Primary Provsioning
                          prov_app_indices[MMGSDI_SEC_GW_PROV_APP_SEL_INDEX]:
                            GW Secondary Provisioning
                          prov_app_indices[MMGSDI_SEC_1X_PROV_APP_SEL_INDEX]:
                            1X Secondary Provsioning
     onchip_sim_data:     Onchip data
     jcdma_card_info:     Carries the JCDMA information
     esn_me_change_flag:  Indicates if ESN ME has changed or not
     refresh_state:       Indicates the current refresh state for the slot
                          Assumption: Each card can only send 1 proactive command
                          at a time, hence, even if the refresh is specific to
                          the application and not the entire card, we only need
                          one instance to track the overall refresh state for the
                          card
     dcs1800_present      Indicates if DCS 1800 DF is present on the card
     imsi_m_programmed    Indicates if CSIM IMSI_M is programmed on the card
     otasp_commit_done:   Indicates if OTASP COMMIT has been performed
     mmgsdi_tp_state:     Indicate the current TP state for concerned slot
     refresh_timer_count  Number of times REFRESH expiry timer has been started
     available_logical_channels:
                          Available channels to open to the card
     tp_ref_count:        Reference count for Terminal Profile
     rehab_info:          Rehabilitation info for files on slot
     card_error_condition:Card Error Cause
     card_apps_available: Bitmap of apps available on card
     sim_phase:           Phase of the SIM card
     card_atr:            Card ATR
     hist_char:           History characters
     protocol:            Card protocol
     gstk_loc_env_rsp_count:
                          Number of location envelope responses received as
                          reported by GSTK
     upp_update_required: At powere up with unprovisioned, this flag is set to
                          TRUE so that upon OTASP COMMIT, assuming the MDN is
                          provisioned by then, SIPUPP/MIPUPP/HRPDUPP files are
                          updated with the data from the provisioned MDN EF.
     mmgsdi_uim_sanity_timer_value:
                          Sanity timer for card requests
     mmgsdi_rpm_iccid_has_changed:
                          Indicates if the ICCID in this card power up is
                          different from the ICCID in the previous power up
     sim_busy:            Card is currently busy but expected to resume normal
                          operations.
     sfi_supported:       Whether card supports file accesses by SFI or not
     parsed_atr:          Parsed result of card ATR
     nv_refresh_in_progress:
                          Indicates if MMGSDI is waiting for rsp from MCFG after NV refresh
     gsm_df_present:      Indicates if GSM DF present in the card
     nv_refresh_reference_count
                          Reference count used for synchornizing NV Refresh trigger
     aid_select_failed_ptr:
                          List of AIDs which failed to activate (SELECT failed). This
                          is then used to ensure app reselection is not attempted.
     sap_atr:             SAP ATR in SAP connected mode
     is_euicc:            Whether the card is eUICC or not
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_card_state_enum_type           mmgsdi_state;
  mmgsdi_card_state_enum_type           mmgsdi_sap_state;
  mmgsdi_int_app_info_type            **app_info_pptr;
  uint32                                prov_app_indices[MMGSDI_MAX_PROV_APPS];
  mmgsdi_onchip_sim_data_type           onchip_sim_data;
  mmgsdi_jcdma_card_info_enum_type      jcdma_card_info;
  mmgsdi_esn_me_change_enum_type        esn_me_change_flag;
  mmgsdi_refresh_state_enum_type        refresh_state;
  boolean                               dcs1800_present;
  boolean                               imsi_m_programmed;
  boolean                               otasp_commit_done;
  mmgsdi_tp_state_enum_type             mmgsdi_tp_state;
  uint16                                refresh_timer_count;
  uint8                                 available_logical_channels;
  uint32                                tp_ref_count;
  mmgsdi_rehab_info_type                rehab_info;
  mmgsdi_card_err_info_enum_type        card_error_condition;
  uint8                                 card_apps_available;
  mmgsdi_sim_phase_enum_type            sim_phase;
  mmgsdi_atr_info_type                  card_atr;
  mmgsdi_hist_char_info_type            hist_char;
  mmgsdi_protocol_enum_type             protocol;
  uint8                                 gstk_loc_env_rsp_count;
  boolean                               upp_update_required;
  uint32                                mmgsdi_uim_sanity_timer_value;
  boolean                               mmgsdi_rpm_iccid_has_changed;
  boolean                               sim_busy;
  boolean                               sfi_supported;
  mmgsdi_parsed_atr_type                parsed_atr;
  boolean                               nv_refresh_in_progress;
  boolean                               gsm_df_present;
  uint32                                nv_refresh_reference_count;
  mmgsdi_aid_select_failed_node_type   *aid_select_failed_ptr;
  mmgsdi_atr_info_type                  sap_atr;
  boolean                               is_euicc;
} mmgsdi_slot_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GENERIC_DATA_TYPE

   DESCRIPTION:
     The structure contains general global data for MMGSDI
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type                    client_id;
  mmgsdi_session_id_type                   pri_gw_session_id;
  mmgsdi_session_id_type                   pri_1x_session_id;
  mmgsdi_session_id_type                   sec_gw_session_id;
  mmgsdi_session_id_type                   sec_1x_session_id;
  mmgsdi_session_id_type                   ter_gw_session_id;
  mmgsdi_session_id_type                   ter_1x_session_id;
  mmgsdi_session_id_type                   slot_session_id[MMGSDI_MAX_NUM_SLOTS];
  cm_client_id_type                        mmgsdi_cm_id;
  boolean                                  voice_call_active;
  boolean                                  data_call_active;
  mmgsdi_me_capability_type                me_capabilities;
  boolean                                  call_ctrl_reg_done;
  /* EONS global info, to be used when there is no card or no active
     GW_PRI session */
  mmgsdi_eons_info_type                    mmgsdi_eons_global_info;
  /* Bitmask for marking which modem tasks are ready */
  uint16                                   ready_tasks;
  /* Onchip auto activation data as read from the auto activation EFS items */
  mmgsdi_onchip_auto_activation_data_type  onchip_act_data[MMGSDI_MAX_NUM_SLOTS];
  uint8                                    num_slots;
  boolean                                  emergency_call_active[MMGSDI_MAX_PROV_APPS];
  boolean                                  ecbm_mode_active;
  /* Switch/Activate confirmation, stores the confirmation temporarily until
     MCFG NV refresh signals MMGSDI to continue initialization */
  mmgsdi_callback_cnf_type                *mmgsdi_switch_act_cnf_ptr[MMGSDI_MAX_PROV_APPS];
} mmgsdi_generic_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INTERNAL_REFRESH_FILE_LIST_TYPE

   DESCRIPTION:    The structure has the information for the files that have been
                   Refreshed or the list of files that the client is interested
                   in receiving notification for
                   From file_list_ptr and file_path_ptr only one of the pointers
                   shall point to valid memory location and other pointer shall
                   be null. Variable num_files shall be indicate for the number
                   of files enums in file_list_ptr and variable num_file_paths
                   will indicate the number of file paths in file_path_ptr.
-------------------------------------------------------------------------------*/
typedef struct {
  uint32                   num_files;
  mmgsdi_file_enum_type   *file_list_ptr;
  mmgsdi_path_type        *file_path_ptr;
  uint32                   num_file_paths;
}mmgsdi_internal_refresh_file_list_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_DATA_INFO_TYPE

   DESCRIPTION:
     The structure contains the information regarding the files the refresh has
     been requested for and the related app and slot information.
     app_data:           Application ID, Label
     slot_id:            Slot ID
     refresh_file_lit:   Files refresh to be performed on
     refresh_mode:       Refresh mode
-------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_internal_refresh_file_list_type  refresh_file_list;
  mmgsdi_aid_type                         app_data;
  mmgsdi_slot_id_enum_type                slot_id;
  mmgsdi_refresh_mode_enum_type           refresh_mode;
} mmgsdi_refresh_data_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SLOT_LINK_EST_DATA_TYPE

   DESCRIPTION:
     This structure contains the card info
     card_present:      Indicates if card is present or not
     cdma_df_present:   Indicates if CDMA DF is present
     gsm_df_present:    Indicates if GSM DF is present
     atr:               ATR data
     histr_char:        History characters
     work_waiting_time: Work waiting time
     iccid:             ICCID
     is_euicc:          EUICC or regular card
-------------------------------------------------------------------------------*/
typedef struct {
  boolean                    card_present;
  mmgsdi_protocol_enum_type  protocol;
  boolean                    cdma_df_present;
  boolean                    gsm_df_present;
  mmgsdi_atr_info_type       atr;
  mmgsdi_hist_char_info_type hist_char;
  uint32                     work_waiting_time;
  mmgsdi_iccid_info_type     iccid;
  boolean                    is_euicc;
}mmgsdi_slot_link_established_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REQUEST_HEADER_TYPE

   DESCRIPTION:
     This structure contains the Request Header Information used to direct
     the MMGSDI Task as to what type of command is contained within the payload
     of the request.
     client_id:      Client ID
     session_id:     Session ID (used for session-based commands)
     response_type:  Request Command Type
     response_len:   Length of message + header
     payload_len:    Length of the message payload: this includes the size of
                     the structure without the header as well as the size of
                     any pointer data being allocated.
                     Note: The additional data pointer being allocated is not
                     a continuous memory space
     slot_id:        Slot that the request is to be performed on
     client_data:    Client data that will be returned in the response
     response_cb:    Response Callback Pointer
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type     client_id;
  mmgsdi_session_id_type    session_id;
  mmgsdi_channel_id_type    channel_id;
  mmgsdi_cmd_enum_type      orig_request_type;
  mmgsdi_cmd_enum_type      request_type;
  mmgsdi_len_type           request_len;
  mmgsdi_len_type           payload_len;
  mmgsdi_slot_id_enum_type  slot_id;
  mmgsdi_client_data_type   client_data;
  mmgsdi_callback_type      response_cb;
} mmgsdi_request_header_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_READ_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a read request
     for the MMGSDI Task.  Based on the parameters, this
     command can be used for accessing the data for LINEAR FIXED,
     CYCLIC and TRANSPARENT files.
     request_header:     Read Request Header information
     access:             File to be read by name
     rec_num:            Record number to be accessed in case of Linear Fixed
                         or Cyclic files
     offset:             Offset to Transparent files to be accessed
     data_len:           Length of data requested to be read
     file_type:          Linear Fixed, Cyclic or Transparent file
     select_before_read: Perform a SELECT first (to get the file length)
                         and then perform a READ.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_access_type              access;
  mmgsdi_rec_num_type             rec_num;
  mmgsdi_offset_type              offset;
  mmgsdi_len_type                 data_len;
  mmgsdi_file_structure_enum_type file_type;
  boolean                         select_before_read;
} mmgsdi_read_req_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_WRITE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a write request
     for the MMGSDI Task.  Based on the parameters, this
     command can be used for accessing the data for LINEAR FIXED,
     CYCLIC or TRANSPARENT files
     request_header: Write Request Header information
     access:         File to be written by name
     rec_num:        Record number to be written in case of Linear Fixed or
                     Cyclic files
     offset:         Offset to Transparent files to be written
     file_type:      Linear Fixed, Cyclic or Transparent file
     data:           Data to be written to the file
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_access_type              access;
  mmgsdi_rec_num_type             rec_num;
  mmgsdi_offset_type              offset;
  mmgsdi_file_structure_enum_type file_type;
  mmgsdi_write_data_type          data;
  mmgsdi_client_data_type         int_client_data;
} mmgsdi_write_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INCREASE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an increase request
     for the MMGSDI Task.  Based on the parameters, this
     command can be used for accessing the data for CYCLIC files
     request_header: Increase Request Header information
     access:         File to be incremented by name
     data:           Value of increment applied to file record contents
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_access_type              access;
  mmgsdi_increase_data_type       data;
  mmgsdi_client_data_type         int_client_data;
} mmgsdi_increase_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_REG_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a client_id_reg request
     for the MMGSDI Task.
     request_header: Client ID Registration Request Header information
     evt_cb_ptr:     Event Callback function pointer that client wants to
                     receive event notification from.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
  mmgsdi_evt_callback_type    * evt_cb_ptr;
  mmgsdi_client_name_type       client_task_name;
  mmgsdi_client_name_type       client_name;
} mmgsdi_client_id_reg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_DEREG_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a client_id_dereg request
     for the MMGSDI Task.
     request_header: Client ID Deregistration Request Header information
     evt_cb_ptr:     Event Callback function pointer that client wants to
                     remove from receiving event notification from.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
  mmgsdi_evt_callback_type    * evt_cb_ptr;
} mmgsdi_client_id_dereg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a client_id_and_evt_dereg request
     for the MMGSDI Task.
     request_header: Client ID and event Deregistration Request Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
} mmgsdi_client_id_and_evt_dereg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ_TYPE

   DESCRIPTION:
     This structure is used when marking an application and related sessions as
     illegal or legal/ready
     request_header: Request Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type            request_header;
  mmgsdi_session_app_legal_status_type  legal_status;
} mmgsdi_session_manage_illegal_subscription_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_AVAILABLE_APPS_REQ_TYPE

   DESCRIPTION:
     This structure is used when obtaining all the available apps discovered
     by MMGSDI.
     request_header: Get All Available Apps Request Header information
     evt_cb_ptr:     Event Callback function pointer that client wants to
                     remove from receiving event notification from.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
} mmgsdi_available_apps_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_OPERATION_REQ_TYPE

   DESCRIPTION:    This structure is used to build a PIN Operations request
                   To the MMGSDI Task.  PIN Operations are classified as:
                       Verify PIN, Disable PIN, Enable PIN, Change PIN, Unblock
                   PIN.

   MEMBERS:
     mmgsdi_pin_operation_enum_type pin_op :  PIN Operation
     mmgsdi_pin_enum_type           pin_id :  PIN ID for operation. Applicable to
                                           :  Verify,Enable,Disable,Change,Unblock
     mmgsdi_static_data_type       pin_puk :  PIN or PUK.  Applicable to
                                           :  Verify,Enable,Disable,Change,Unblock
     mmgsdi_static_data_type       new_pin :  New PIN. Applicable only to
                                           :  Change,Unblock
     mmgsdi_static_data_type      pin_copy :  Copied PIN. Applicable only to Change
     mmgsdi_pin_eum_type       orig_pin_id :  Original PIN ID
     mmgsdi_sw_status_type         orig_sw :  Status words of the original command
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_pin_operation_enum_type  pin_op;
  mmgsdi_pin_enum_type            pin_id;
  mmgsdi_static_data_type         pin_puk;
  mmgsdi_static_data_type         new_pin;
  mmgsdi_static_data_type         pin_copy;
  mmgsdi_pin_enum_type            orig_pin_id;
  mmgsdi_sw_status_type           orig_sw;
} mmgsdi_pin_operation_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEL_AID_REQ_TYPE

   DESCRIPTION:    This structure is used to build a Select AID Request
                   To the MMGSDI Task.

   MEMBERS:
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_static_data_type         app_data;
} mmgsdi_select_aid_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_ALL_PIN_STATUS_REQ_TYPE

   DESCRIPTION:    This structure is used to build a PIN Status request
                   To the MMGSDI Task.

   MEMBERS:
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
} mmgsdi_get_all_pin_status_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_FILE_ATTR_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a get_file_attributes request
     for the MMGSDI Task.  Based on the parameters, this
     command can be used for getting file attributes for LINEAR FIXED,
     CYCLIC or TRANSPARENT files and master file and
     dedicated files
     request_header:       Get File Attributes Request Header information
     access:               File to be accessed by name
     activate_aid:         Boolean indicating whether aid should be activated
     int_client_data:      Client data to be stored
     select_rsp_requested: Boolean indication whether select rsp in needed
     select_rsp_type:      Type of select rsp needed
     skip_uicc_arr:        Whether to skip arr check for uicc cards
     aid_select_mode:      Mode of AID selection
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type     request_header;
  mmgsdi_access_type             access;
  boolean                        activate_aid;
  mmgsdi_client_data_type        int_client_data;
  boolean                        select_rsp_requested;
  mmgsdi_select_rsp_enum_type    select_rsp_type;
  boolean                        skip_uicc_arr;
  mmgsdi_select_mode_enum_type   aid_select_mode;
} mmgsdi_get_file_attr_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an mmgsdi_session_open request
     for the MMGSDI Task.
     request_header:                    Session Open Request Header information
     session_type:                      Type of session being opened
     non_prov_app_data_ptr:             Information about app if non-provisioning.
     set_notify:                        Boolean indicating whether clients should call
                                        mmgsdi_session_prov_app_init_complete() when they
                                        finish their initialization procedure for the
                                        applications. This is only applicable to the
                                        provisioning-session types.
     evt_cb_ptr:                        Event Callback function pointer that client wants to
                                        register to receive event notification.
     select_rsp_requested:              Boolean indication whether select rsp in
                                        needed
     select_rsp_type:                   Type of select rsp needed
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type     request_header;
  mmgsdi_session_type_enum_type  session_type;
  mmgsdi_non_prov_app_info_type  non_prov_app_data;
  boolean                        set_notify;
  mmgsdi_evt_callback_type    *  evt_cb_ptr;
  boolean                        select_rsp_requested;
  mmgsdi_select_rsp_enum_type    select_rsp_type;
} mmgsdi_session_open_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_EXT_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an mmgsdi_session_open_ext request
     for the MMGSDI Task.
     request_header:                    Session Open Request Header information
     session_type_mask:                 Mask that holds Type of sessions being opened
     set_notify_mask:                   Mask indicating whether clients should call
                                        mmgsdi_session_prov_app_init_complete() when they
                                        finish their initialization procedure for the
                                        applications. This is only applicable to the
                                        provisioning-session types.
     evt_cb_ptr:                        Event Callback function pointer that client wants to
                                        register to receive event notification.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type     request_header;
  uint32                         session_type_mask;
  uint32                         set_notify_mask;
  mmgsdi_evt_callback_type       *evt_cb_ptr;
} mmgsdi_session_open_ext_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_WITH_MF_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an mmgsdi_session_open_with_mf
     request for the MMGSDI Task.
     request_header:                    Session Open Request Header information
     session_type:                      Type of session being opened
     evt_cb_ptr:                        Event Callback function pointer that
                                        client wants to register to receive
                                        event notification.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type     request_header;
  mmgsdi_session_type_enum_type  session_type;
  mmgsdi_evt_callback_type    *  evt_cb_ptr;
} mmgsdi_session_open_with_mf_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_CLOSE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a session_close request
     for the MMGSDI Task.
     request_header    : Session Close Request Header information
     non_prov_terminate: Indicate whether client wants to terminate non-prov apps
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
  boolean                    non_prov_terminate;
} mmgsdi_session_close_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_STATUS_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the MMGSDI Task.
     request_header: Status Request Header information
     me_app_status  :  Status of the UICC Application maintained by the ME
     ret_data_struct:  Structure of Return Data Requested
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_status_me_app_enum_type   me_app_status;
  mmgsdi_status_ret_data_enum_type ret_data_struct;
} mmgsdi_status_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_CONNECT_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a SAP connect request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type            request_header;
  mmgsdi_sap_conn_condition_enum_type   condition;
} mmgsdi_sap_connect_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_DISCONNECT_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a SAP disconnect request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  mmgsdi_disconnect_mode_enum_type     disconnect_mode;
  mmgsdi_sap_mode_enum_type            sap_mode;
} mmgsdi_sap_disconnect_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_ATR_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a get atr request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
}mmgsdi_get_atr_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_GET_ATR_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a SAP get atr request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef mmgsdi_get_atr_req_type mmgsdi_sap_get_atr_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEND_APDU_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a SAP APDU request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type         request_header;
  uint8                              channel_id;
  mmgsdi_send_apdu_options_enum_type apdu_option;
  mmgsdi_send_apdu_data_type         data;
} mmgsdi_send_apdu_req_type;

typedef mmgsdi_send_apdu_req_type mmgsdi_sap_send_apdu_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_POWER_ON_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a SAP power on request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
} mmgsdi_sap_power_on_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_POWER_OFF_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a SAP power off request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
} mmgsdi_sap_power_off_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_CARD_READER_STATUS_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a SAP card reader status request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
}mmgsdi_sap_card_reader_status_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_RESET_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a SAP reset request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
} mmgsdi_sap_reset_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PUP_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a power up CARD request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type              request_header;
  mmgsdi_card_pup_options_enum_type       option;
}mmgsdi_card_pup_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_RESET_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a Card reset request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type        request_header;
  uim_protocol_type                 protocol;
  mmgsdi_card_reset_cause_enum_type reset_cause;
} mmgsdi_card_reset_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PDOWN_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a power down CARD request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type              request_header;
  mmgsdi_card_pdown_options_enum_type     option;
}mmgsdi_card_pdown_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SET_FILE_STATUS_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a invalidate or rehabilitate
     request for the MMGSDI Task.
     request_header: Invalidate/Rehabilitate Request Header information
     access:         File to Invalidate/Rehabilitate
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_access_type               access;
} mmgsdi_set_file_status_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SRV_AVAILABLE_REQ_TYPE

   DESCRIPTION:
     This structure is used when finding out if the service is enabled in
     CST/SST/UST
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_service_enum_type         service;
} mmgsdi_srv_available_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_AUTH_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an USIM authentication request
     for the MMGSDI Task.
     request_header:      Read Request Header information
     auth_req:            authentication request
     illegal_or_perso_ok: Process the command in illegal or perso failed state
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  mmgsdi_autn_context_enum_type        auth_context;
  mmgsdi_data_type                     auth_req;
  boolean                              illegal_or_perso_ok;
} mmgsdi_auth_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_COMPUTE_IP_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating COMPUTE IP request
     for the MMGSDI Task.
     request_header:        Request Header information
     compute_ip_data:       authentication request
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  mmgsdi_compute_ip_data_type          compute_ip_data;
} mmgsdi_compute_ip_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_RUN_CAVE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating RUN CAVE request
     for the MMGSDI Task.
     request_header:        Request Header information
     rand_type:             Random Type for RUN CAVE Operation
     rand_chal:             Random Challenge for RUN CAVE Operation
     dig_len:               Digit Length for RUN CAVE
     digits:                Digits for RUN CAVE
     process_control:       Process Control for RUN CAVE
     esn:                   ESN For Run CAVE calculation
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type            request_header;
  mmgsdi_cave_rand_type_type            rand_type;
  mmgsdi_cave_rand_chal_type            rand_chal;
  mmgsdi_cave_dig_len_type              dig_len;
  mmgsdi_cave_digits_type               digits;
  mmgsdi_cave_process_control_type      process_control;
  mmgsdi_cave_esn_type                  esn;
} mmgsdi_run_cave_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CDMA_AKA_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating AKA request
     for the MMGSDI Task.
     request_header:        Request Header information
     aka_cmd:               Specifies the AKA command
     randa:                 Random Anonymity Key
     AUTN_len:              Authentication Token length
     AUTN:                  Authentication Token
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type            request_header;
  mmgsdi_cdma_aka_cmd_enum_type         aka_cmd;
  mmgsdi_cdma_aka_randa_type            randa;
  mmgsdi_data_type                      autn;
} mmgsdi_cdma_aka_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GENERATE_KEY_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a GENERATE KEY request
     for the MMGSDI Task.
     request_header:        Request Header information
     first_octet:           First Octet for generate key Operation
     last_octet:            Last Octet for Generate Key Operation
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type            request_header;
  mmgsdi_single_vpm_octet_type          first_octet;
  mmgsdi_single_vpm_octet_type          last_octet;
} mmgsdi_generate_key_req_type;

/*--------------------------------------------------------------------------
  STRUCTURE:      MMGSDI_SEARCH_REQ_TYPE

  DESCRIPTION:
    This structure is used when generating a search or seek request
    for the MMGSDI Task.  Based on the parameters, this
    command can be used for accessing the data for LINEAR FIXED,
    CYCLIC files
    request_header    : Search Request Header information
    access            : File to be search
    search type       : ICC_SEARCH, UICC_SIMPLE and UICC_ENHANCED Search
    rec_num           : It is used for UICC SIMPLE and ENHANCED Search.
                        It will ignore for ICC_SEARCH.
                        It indicate the record number from where search will
                        start in a record file.
                        Valid Range for record number:-
                        0x00         : Current Record
                        0x01 to 0xFE : Physical Record number.

    Seek Direction    : Search direction for ICC Search.
    Search Direction  : Search Direction for UICC Search.
    offset            : It is used for UICC ENHANCED SEARCH. It indicate the
                        record offset from where, search will start in
                        a record.
                        Valid Range:
                         0x00-0xFF for MMGSDI_SEARCH_BY_CHAR type.
                         0x00–0xFE for MMGSDI_SEARCH_BY_OFFSET type.
    data              : data to search.
--------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type        request_header;
  mmgsdi_access_type                access;
  mmgsdi_search_enum_type           search_type;
  mmgsdi_rec_num_type               rec_num;
  mmgsdi_seek_direction_enum_type   seek_direction;
  mmgsdi_search_direction_enum_type search_direction;
  mmgsdi_search_offset_data_type    enhanced_search_offset;
  mmgsdi_search_data_type           data;
} mmgsdi_search_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ESN_MEID_INFO_TYPE

   DESCRIPTION:
     This structure is used to store ESN/MEID related information
     esn_usage:             ESN Usage indicator
     esn_length:            ESN Length
     esn:                   ESN value
     meid:                  MEID value
-------------------------------------------------------------------------------*/
typedef struct {
  uint8                      esn_usage;
  uint8                      esn_length;
  dword                      esn;
  qword                      meid;
  boolean                    meid_available;
}mmgsdi_esn_meid_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_STORE_ESN_REQ_TYPE

   DESCRIPTION:
     This structure is used when issuing the store ESN request to the card
     request_header:        Request Header information
     esn_meid_info:         ESN/MEID related information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
  mmgsdi_esn_meid_info_type  esn_meid_info;
}mmgsdi_session_store_esn_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_UIM_REPORT_RSP_TYPE

   DESCRIPTION:
     The structure contains the raw data return from UIM as a result of a
     request sent from MMGSDI
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;  /* Request header information   */
  boolean                     is_uim_success;  /* UIM_PASS = TRUE,
                                                  UIM_FAIL = FALSE             */
  int32                       uim_rpt_code;    /* map to uim_report_code_type  */
  mmgsdi_sw_status_type       status_word;     /* sw1 and sw2 from the Card    */
  mmgsdi_sw_status_type       get_resp_status_word;   /*   Intermediate sw     */
  boolean                     cmd_transacted;  /* whether the cmd was sent to
                                                  the card or not              */
  int32                       rsp_data_index;  /* index into
                                                  mmgsdi_client_req_table      */
  mmgsdi_data_type            rsp_data;
  mmgsdi_return_enum_type     mmgsdi_error_status;
} mmgsdi_uim_report_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_REG_RSP_TYPE

   DESCRIPTION:
     The structure contains the client reg/dereg response
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;  /* Request header information   */
  mmgsdi_return_enum_type     status;          /* Registration success/failed  */
  int32                       rsp_data_index;  /* index into
                                                  mmgsdi_client_req_table      */
} mmgsdi_client_id_reg_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GENERIC_RSP_TYPE

   DESCRIPTION:
     The structure contains the generic response information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;  /* Request header information   */
  mmgsdi_return_enum_type     status;          /* Registration success/failed  */
  int32                       rsp_data_index;  /* index into
                                                  mmgsdi_client_req_table      */
} mmgsdi_generic_rsp_type;

/* ===========================================================================
   STRUCTURE:      MMGSDI_SIMLOCK_SESSION_ID_TABLE_TYPE

   DESCRIPTION:
===========================================================================*/
typedef struct {
  mmgsdi_session_id_type   session_id[SIMLOCK_SLOT_COUNT_MAX][SIMLOCK_SUBS_COUNT_MAX];
} mmgsdi_simlock_session_id_table_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SIMLOCK_RSP_TYPE

   DESCRIPTION:
     The structure contains the SIM Lock response information
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_result_enum_type                      status;
  simlock_message_response_data_type            resp_data;
  const mmgsdi_simlock_session_id_table_type  * user_data_ptr;
} mmgsdi_simlock_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SIMLOCK_EVT_TYPE

   DESCRIPTION:
     The structure contains the SIM Lock response information
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_result_enum_type                status;
  simlock_event_data_type                 event_data;
} mmgsdi_simlock_evt_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_RETRY_INFO_TYPE

   DESCRIPTION:
     Refresh retry data for each refresh request
     retry_count       : Retries attempted so far
     retry_limit       : Max retry attempts
     retry_rsp_callback: Callback to be executed after a retry attempt

-------------------------------------------------------------------------------*/
typedef struct {
  uint32               retry_count;
  uint32               retry_limit;
  mmgsdi_callback_type retry_rsp_cb;
}mmgsdi_refresh_retry_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_RETRY_SETTINGS_TYPE

   DESCRIPTION:
     Global refresh retry data type
     flags         : Specific refresh retry settings
     retry_timer   : Time between retry attempts
     max_queue_size: The largest number of fcn retry attempts
     retry_limit   : Max number of retries before giving up

-------------------------------------------------------------------------------*/
typedef struct {
  uint32 flags;
  uint32 retry_timer;
  uint32 max_queue_size;
  uint32 retry_limit;
}mmgsdi_refresh_retry_settings_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_CARD_REQ_TYPE

   DESCRIPTION:
     This structure is used when a refresh is issued to MMGSDI
     refresh_files   : FCN
     refresh_mode    : Refresh Mode
     num_files       : Number of files in FCN
     AID             : Application ID
     plmn_list       : PLMNwAcT List
     switch_protocol : switch protocol
     retry_info      : Refresh retry info
  -------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_refresh_mode_enum_type      refresh_mode;
  mmgsdi_data_type                   refresh_files;
  uint32                             num_files;
  mmgsdi_aid_type                    aid;
  mmgsdi_data_type                   plmn_list;
  boolean                            switch_protocol;
  mmgsdi_refresh_retry_info_type     retry_info;
  mmgsdi_refresh_source_enum_type    source;
}
mmgsdi_refresh_card_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_REG_REQ_TYPE

   DESCRIPTION:
     This structure is used for refresh registration/deregistration
     refresh_files      : FCN registration
     vote_for_init      : Vote for Terminating app
     steering_of_roaming: Interested in refresh mode steering
     ok_to_proceed      : Receive notification before starting refresh
     reg_all_files      : Option to register all files
  -------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_internal_refresh_file_list_type    refresh_files;
  boolean                                   vote_for_init;
  boolean                                   steering_of_roaming;
  boolean                                   ok_to_proceed;
  boolean                                   reg_all_files;
}mmgsdi_refresh_reg_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the MMGSDI Task.
     request_header  : Status Request Header information
     refresh_req_type: Original Refresh Request Type
     card_req        : Used when Card Initiates a Refresh
     reg_req         : Refresh Registration Request
     pass_fail       : Used with Refresh_complete to notify successful refresh
     ok_req          : Used to indicate if its ok to proceed with refresh
     retry_req       : Retry Refresh
     timer_count     : Timer count corresponding to the
                       expired timer
     notify_pdown    : Notify client before card power down

  -------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type         request_header;
  mmgsdi_refresh_orig_enum_type      refresh_req_type;
  union{
    mmgsdi_refresh_card_req_type     card_req;
    mmgsdi_refresh_reg_req_type      reg_req;
    boolean                          pass_fail;
    boolean                          ok_req;
    boolean                          retry_req;
    uint16                           timer_count;
    boolean                          notify_pdown;
  }refresh;
} mmgsdi_refresh_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_BCAST_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an OMA BCAST request
     for the MMGSDI Task.
     request_header: Bcast Request Header information
     bcast_mode:     MMGSDI_BCAST_SPE_AUDIT_MODE/
                     MMGSDI_BCAST_SPE_SIG_MODE/
                     MMGSDI_BCAST_REC_AUDIT_MODE
     bcast_req:      bcast request
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  mmgsdi_bcast_mode_enum_type          bcast_mode;
  mmgsdi_data_type                     bcast_req;
} mmgsdi_bcast_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_SUB_OK_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a session_close request
     for the MMGSDI Task.
     request_header: Request Header information
     ok_for_sub:     Boolean to notify if it's ok to publish subscription
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
  boolean                    ok_for_sub;
} mmgsdi_session_sub_ok_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TP_DOWNLOAD_COMPLETE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a Terminal Profile Download Complete Request
     for the MMGSDI Task.
     request_header: Request Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
} mmgsdi_tp_dl_complete_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NOTIFY_LINK_EST_REQ_TYPE

   DESCRIPTION:
     This structure is used when notifying a Link Establishment
     (card detection) to the MMGSDI Task.
     request_header:  Request Header information
     link_est_data:   Card related data
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type             request_header;
  mmgsdi_slot_link_established_data_type link_est_data;
} mmgsdi_notify_link_est_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NOTIFY_CARD_ERROR_REQ_TYPE

   DESCRIPTION:
     This structure is used when notifying a Card Error
     to the MMGSDI Task.
     request_header:  Request Header information
     error_condition: Card error cause
     temporary_error: Is this a permanent or temperary card error
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type     request_header;
  mmgsdi_card_err_info_enum_type error_condition;
  boolean                        temporary_error;
} mmgsdi_notify_card_error_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_NOTIFY_SIM_BUSY_REQ_TYPE

   DESCRIPTION:
     This structure is used when notifying a SIM busy
     to the MMGSDI Task.
     request_header :  Request Header information
     sim_busy       :  Boolean indicating is SIM card is currently busy
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type     request_header;
  boolean                        sim_busy;
} mmgsdi_notify_sim_busy_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INTERNAL_PUP_INIT_REQ_TYPE

   DESCRIPTION:
     This structure is used when notifying a Internal Card power-up request
     to the MMGSDI Task.
     request_header:  Request Header information
     curr_card_state: Current state of the card being intialized
     curr_app_state:  Current state of the application being initalized
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;
  mmgsdi_card_state_enum_type curr_card_state;
  mmgsdi_app_state_enum_type  curr_app_state;
} mmgsdi_internal_pup_init_req_type;

/* ----------------------------------------------------------------------------
   UNION:      MMGSDI_RSP_TYPE

   DESCRIPTION:
     This union contains the response type
     uim_report_rsp:      Response from UIM
     client_id_reg_rsp:   Response for Client ID/Evt Reg and Dereg
     generic_rsp:         Generic Response
-------------------------------------------------------------------------------*/
typedef union {
  mmgsdi_uim_report_rsp_type       uim_report_rsp;
  mmgsdi_client_id_reg_rsp_type    client_id_reg_rsp;
  mmgsdi_generic_rsp_type          generic_rsp;
} mmgsdi_rsp_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_WRITE_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the write extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
  mmgsdi_len_type                  rec_num;
  mmgsdi_len_type                  offset;
  mmgsdi_file_structure_enum_type  file_type;
  mmgsdi_data_type                 data;
  mmgsdi_client_data_type          int_client_data;
} mmgsdi_write_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_READ_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the read extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
  mmgsdi_len_type                  offset;
  mmgsdi_len_type                  rec_num;
  mmgsdi_len_type                  data_len;
  mmgsdi_file_structure_enum_type  file_type;
  boolean                          select_before_read;
} mmgsdi_read_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INCREASE_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the increase extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
  mmgsdi_data_type                 data;
} mmgsdi_increase_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REHABILITATE_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the rehabilitate extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
} mmgsdi_rehabilitate_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_INVALIDATE_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the invalidate extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
} mmgsdi_invalidate_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_FILE_INFO_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the get file attribute extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
  mmgsdi_client_data_type          int_client_data;
  boolean                          select_rsp_requested;
  mmgsdi_select_rsp_enum_type      select_rsp_type;
  boolean                          skip_uicc_arr;
} mmgsdi_get_file_info_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the session open extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_aid_type                aid;
  boolean                        exclusive_channel;
  uint32                         app_pin_index;
  boolean                        select_rsp_requested;
  mmgsdi_select_rsp_enum_type    select_rsp_type;
} mmgsdi_session_open_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_DEACTIVATE_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the session deactivate extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  int32                deactivate_info;
} mmgsdi_session_deactivate_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_CLOSE_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the session close extra info
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_request_header_type  request_header;
  boolean                     is_dummy;
} mmgsdi_session_close_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_PIN_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is used to help build a PIN Operations
                   request to the MMGSDI Task.

   MEMBERS:
     mmgsdi_pin_operation_type  orig_pin_op   : PIN Operation
     mmgsdi_pin_info_type       pin_info      : PIN Information
     mmgsdi_num_retry_enum_type retry_type    : Type of Retry # being requested
     mmgsdi_static_data_type    pin_data      : PIN data
     mmgsdi_static_data_type    pin_data_copy : Copied PIN data
     mmgsdi_pin_enum_type       orig_pin_id   : Original PIN id
     mmgsdi_sw_status_type      orig_sw       : SW1/SW2 of the original req
---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_pin_operation_enum_type  orig_pin_op;
  mmgsdi_app_pin_info_type        pin_info;
  mmgsdi_num_retry_enum_type      retry_type;
  mmgsdi_return_enum_type         pin1_report_status;
  mmgsdi_static_data_type         pin_data;
  mmgsdi_static_data_type         pin_data_copy;
  mmgsdi_pin_enum_type            orig_pin_id;
  mmgsdi_sw_status_type           orig_sw;
}mmgsdi_pin_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_DISCONNECT_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the SAP disconnect extra information
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_disconnect_mode_enum_type disconnect_mode;
  mmgsdi_slot_id_enum_type         slot_id;
} mmgsdi_sap_disconnect_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_CONNECT_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the SAP connect extra information
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_slot_id_enum_type slot_id;
} mmgsdi_sap_connect_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SAP_POWER_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the SAP Power ON/OFF extra information
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_slot_id_enum_type slot_id;
} mmgsdi_sap_power_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEND_APDU_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the send APDU extra info

   MEMBERS:
     channel_id  : Channel ID
     cla         : Class byte of the APDU
     ins         : Instruction byte of the APDU
     apdu_option : Type of APDU response requested
     data        : Requested APDU (includes CLA and INS)
     slot_id     : Slot for which SAP_SEND_APDU is requested
------------------------------------------------------------------------------*/
typedef struct
{
  uint8                              channel_id;
  uint8                              cla;
  uint8                              ins;
  mmgsdi_send_apdu_options_enum_type apdu_option;
  mmgsdi_data_type                   data;
  mmgsdi_slot_id_enum_type           slot_id;
} mmgsdi_send_apdu_extra_info_type;

typedef mmgsdi_send_apdu_extra_info_type mmgsdi_sap_send_apdu_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PUP_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the card_pup extra info
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_card_pup_options_enum_type       option;
}mmgsdi_card_pup_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_PDOWN_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the card_pdown extra info
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_card_pdown_options_enum_type     option;
}mmgsdi_card_pdown_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CARD_REFRESH_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the card_pdown extra info
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_refresh_stage_enum_type        stage;
  boolean                               send_evt;
  mmgsdi_refresh_orig_enum_type         refresh_req_type;
  mmgsdi_refresh_mode_enum_type         mode;
  mmgsdi_refresh_start_app_enum_type    app_type;
}mmgsdi_card_refresh_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_REQ_EXTRA_INFO_TYPE

   DESCRIPTION:
     The union contains the extra info that is required to be stored to be
     used in building the confirmation
-------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_ACTIVATE_ONCHIP_SIM_REQ

   DESCRIPTION:
     This structure is used for the ONCHIP SIM REQ Data Type.  This command
     structure is populated for the purpose of configuring the GSDI SIM
     Cache and the Authentication Algorithm Key.

     The Authentication used is defaulted to the the XOR Alogirthm.

     request_header:       Activate ONCHIP SIM Request Header information
     onchip_sim:           Data to be written to the file
     onchip_sim.data_len:  len of TLV data
     onchip_sim.data:      TLV Datat

-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type         request_header;
  mmgsdi_onchip_sim_config_data_type onchip_sim;
  mmgsdi_onchip_mode_enum_type       onchip_sim_mode;
} mmgsdi_activate_onchip_sim_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SRV_AVAILABLE_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is used to help build a SRV AVAILABLE
                   request to the MMGSDI Task.
   MEMBERS:
     boolean       srv_available
---------------------------------------------------------------------------*/
typedef struct
{
  boolean         srv_available;
}mmgsdi_srv_available_extra_info_type;

/* --------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SEARCH_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the search extra info.
     access: File Information
---------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_access_type               access;
} mmgsdi_search_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_CNF_ADD_INFO_TYPE

   DESCRIPTION:    This structure is used to help build a various session
                   Operation confirmation additional param to the MMGSDI Task.

   MEMBERS:
     app_info             : Application detail
     select_rsp_requested : Boolean to indicate if select rsp is requested
     select_rsp_type      : Type of select response needed
     rsp_data             : Select response data
---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_aid_type                     app_info;
  boolean                             select_rsp_requested;
  mmgsdi_select_rsp_enum_type         select_rsp_type;
  mmgsdi_data_type                    rsp_data;
} mmgsdi_session_cnf_add_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OPEN_EXT_CNF_INFO_TYPE

   DESCRIPTION:    This structure is used to help build a various session
                   Operation confirmation params.

   MEMBERS:
    session_info:  Holds information of all the sessions opened.
    num_sessions:  number of sessions opened successfully.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                          num_sessions;
  mmgsdi_session_open_info_type* session_info_ptr;
} mmgsdi_session_open_ext_cnf_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_INFO_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is for extra param for session get info cmd

   MEMBERS:
     session_type      : session type
     slot_id           : slot ID
     channel_id     :    The logical channel ID that is assigned
                         to the application
     app_data          : APP identifier information
     app_state         : Application status - refer mmgsdi_app_state_enum_type
 ---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_session_type_enum_type  session_type;
  mmgsdi_slot_id_enum_type       slot_id;
  mmgsdi_channel_id_type         channel_id;
  mmgsdi_aid_type                app_data;
  mmgsdi_app_state_enum_type     app_state;
}mmgsdi_session_get_info_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_ENABLE_SERVICE_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is for extra param for session get info cmd

   MEMBERS:
     service_type      : service type requested for enable/disable
     client_data       : client data of the client
     orig_callback     : original call back of the client
 ---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_service_enum_type service_type;
  mmgsdi_client_data_type  client_data;
  mmgsdi_callback_type     orig_callback;
  boolean                  is_status_same;
}mmgsdi_session_enable_service_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_READ_PRL_DATA_TYPE

   DESCRIPTION:
     Data Structure to hold READ PRL data.
-------------------------------------------------------------------------------*/

typedef struct {
  word                             prl_version;
  dword                            size;
  boolean                          valid;
  byte                             *roaming_list_ptr;
  word                             sspr_p_rev;
} mmgsdi_read_prl_data_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_READ_PRL_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is for extra param for session read prl cmd

   MEMBERS:
     service_type      : service type requested for enable/disable
     client_data       : client data of the client
     orig_callback     : original call back of the client
 ---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_read_prl_data_type* prl_ptr;
  mmgsdi_client_data_type    client_data;
  mmgsdi_callback_type       orig_callback;
}mmgsdi_session_read_prl_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_BLOCK_RSP_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is for extra param for OTASP block cmd

   MEMBERS:
     otasp_block_op_type : OTASP block operation type
 ---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_otasp_block_op_enum_type  otasp_block_op_type;
}mmgsdi_session_otasp_block_rsp_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_EONS_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is for extra param for GET EONS cmd

   MEMBERS:
     plmn_id_list :             PLMN Id list type
     ignore_display_condition : ignore display condition enum type
 ---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_plmn_id_list_type               plmn_id_list;
  mmgsdi_eons_ignore_disp_cond_enum_type ignore_display_condition;
}mmgsdi_session_get_eons_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_SE13_PLMN_NAMES_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is for extra param for GET SE13 PLMN NAMES cmd

   MEMBERS:
     plmn_id_list : PLMN Id list type
 ---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_plmn_id_list_type plmn_id_list;
}mmgsdi_get_se13_plmn_names_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SS_EVENT_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is for extra param for EONS SS EVENT cmd

   MEMBERS:
     ss_info : SS Event's EONS data
 ---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_eons_ss_data_type ss_info;
}mmgsdi_ss_event_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_UPDATE_ACSGL_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is for extra param for UPDATE EF-ACSGL cmd

   MEMBERS:
     update_type : Type of update requested
     plmn_csg_id : CSG Id and corresponding PLMN Id
 ---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_update_acsgl_enum_type update_type;
  mmgsdi_plmn_csg_id_type       plmn_csg_id;
}mmgsdi_session_update_acsgl_extra_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GSTK_LOC_ENV_EXTRA_INFO_TYPE

   DESCRIPTION:    This structure is for extra param for loc env response

   MEMBERS:
     imsi_switched : IMSI is switched or not
 ---------------------------------------------------------------------------*/
typedef struct
{
  boolean imsi_switched;
}mmgsdi_gstk_loc_env_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_APP_STATUS_EXTRA_INFO_TYPE

   DESCRIPTION:
     The structure contains the app status extra info
------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_status_me_app_enum_type  app_status;
}mmgsdi_app_status_extra_info_type;


/* ----------------------------------------------------------------------------
   UNION:      MMGSDI_CLIENT_REQ_EXTRA_INFO_TYPE

   DESCRIPTION:
     This union contains the response type
-------------------------------------------------------------------------------*/
typedef union
{
  mmgsdi_write_extra_info_type               write_data;
  mmgsdi_read_extra_info_type                read_data;
  mmgsdi_increase_extra_info_type            increase_data;
  mmgsdi_get_file_info_extra_info_type       get_file_attr_data;
  mmgsdi_send_apdu_extra_info_type           send_apdu_data;
  mmgsdi_sap_disconnect_extra_info_type      sap_disconnect_data;
  mmgsdi_sap_connect_extra_info_type         sap_connect_data;
  mmgsdi_sap_power_extra_info_type           sap_power_data;
  mmgsdi_sap_send_apdu_extra_info_type       sap_send_apdu_data;
  mmgsdi_card_pup_extra_info_type            pup_data;
  mmgsdi_card_pdown_extra_info_type          pdown_data;
  mmgsdi_card_refresh_extra_info_type        refresh_data;
  mmgsdi_rehabilitate_extra_info_type        rehab_data;
  mmgsdi_invalidate_extra_info_type          invalidate_data;
  mmgsdi_pin_extra_info_type                 pin_data;
  mmgsdi_srv_available_extra_info_type       srv_available_data;
  mmgsdi_search_extra_info_type              search_data;
  mmgsdi_session_open_extra_info_type        open_data;
  mmgsdi_session_deactivate_extra_info_type  deactivate_data;
  mmgsdi_session_close_extra_info_type       close_data;
  mmgsdi_session_get_info_extra_info_type    session_get_info_data;
  mmgsdi_session_enable_service_extra_info_type   enable_service_data;
  mmgsdi_session_otasp_block_rsp_extra_info_type  session_otasp_block_data;
  mmgsdi_session_read_prl_extra_info_type         session_read_prl_data;
  mmgsdi_session_get_eons_extra_info_type         session_get_eons_data;
  mmgsdi_get_se13_plmn_names_extra_info_type      get_se13_plmn_names_data;
  mmgsdi_ss_event_extra_info_type                 ss_event_data;
  mmgsdi_session_update_acsgl_extra_info_type     update_acsgl_data;
  mmgsdi_gstk_loc_env_extra_info_type             gstk_loc_env_data;
  mmgsdi_app_status_extra_info_type               app_status_data;
} mmgsdi_client_req_extra_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_UIM_CLIENT_REQ_USER_DATA_TYPE

   DESCRIPTION:
     The structure will be tabled in MMGSDI where its index will be passed to
     UIM for processing.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type                   client_id;
  mmgsdi_session_id_type                  session_id;
  mmgsdi_cmd_enum_type                    orig_request_type;
  mmgsdi_cmd_enum_type                    request_type;
  mmgsdi_slot_id_enum_type                slot_id;
  mmgsdi_client_data_type                 client_data;
  mmgsdi_callback_type                    response_cb;
  uim_cmd_type                          * uim_cmd_ptr;  /* To be removed */
  mmgsdi_client_req_extra_info_type     * extra_param_ptr;
  mmgsdi_uim_rsp_status_type              uim_rsp_status;
  mmgsdi_task_enum_type                   task_id;
} mmgsdi_uim_client_req_user_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_REG_DATA_TYPE

   DESCRIPTION:
     Data to represent the refresh data that the client has registered with

    MEMBERS:
      refresh_file    : Files Client is interested in
      vote_for_init   : Client will vote for init
      reg_by_path     : is registration by path
      is_reg          : is registered
      proceed_refresh : Client interested in receiving notification before card
                        power down
      reg_all_files   : Client registers for all files

-------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_internal_refresh_file_list_type  refresh_files;
  boolean                                 vote_for_init;
  boolean                                 reg_by_path;
  boolean                                 is_reg;
  boolean                                 proceed_refresh;
  boolean                                 reg_all_files;
}mmgsdi_refresh_reg_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_PIN_INFO_TYPE

   DESCRIPTION:
     The structure contains the session pin information.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_session_pin_enum_type  id;
  mmgsdi_pin_status_enum_type   status;
} mmgsdi_session_pin_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CHANNEL_INFO_TYPE

   DESCRIPTION:
     The structure contains channel information.
     num_connections:         Indicates how many clients have opened a session
                              referencing this application/application type
     slot_id:                 Which slot is this applicable to
     channel_id:              The logical channel ID that is assigned to the
                              application by the card
     exclusive_channel:       Whether the channel is opened exlusively or can be
                              shared
     app_info_ptr:            Pointer to the application details including AID,
                              Label, PINs
     channel_state:           state of the channel e.g. activation in progress,
                              deactivation in progress, streaming is done etc.
     select_rsp_type:         response type requested as part of a SELECT
     app_termination_skipped: Whether app termination was skipped on this channel
                              during session deactivation
     selective_act_after_app_term_skipped: Indicates if selective activation is
                              required in cases where app termination was skipped
                              earlier, for eg STATUS command is not required
                              after post pin.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_len_type                num_connections;
  mmgsdi_slot_id_enum_type       slot_id;
  mmgsdi_channel_id_type         channel_id;
  boolean                        exclusive_channel;
  mmgsdi_int_app_info_type      *app_info_ptr;
  mmgsdi_channel_state_enum_type channel_state;
  mmgsdi_select_rsp_enum_type    select_rsp_type;
  boolean                        app_termination_skipped;
  boolean                        selective_act_after_app_term_skipped;
} mmgsdi_channel_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_INFO_TYPE

   DESCRIPTION:
     The structure contains session information.
     session_id:            Unique ID associated with each mmgsdi_session_open() call
     session_type:          Associate the session_id with the type of session that it
                            opens with.  Required especially before the binding with the
                            application on the card is being performed
     channel_info_index     An index into the channel info pointer table which stores
                            channel information allocated for the session.
     refresh_data:          Refresh information associated with this session_id and
                            the corresponding applications
     file_data:             File update list associated with this session_id
     session_evt_cb_ptr:    Event callback pointer for session related events
     notify_init:           Indicate if the client wants to be responsible for
                            notifying its initialization complete procedure
     init_complete:         The client has indicated that it has finished its
                            initialization procedure for this session/application
     steering_of_roaming:   Client is interested in refresh mode steering of roaming
     steering_complete_rcvd:Client has sent refresh complete for steering
     is_session_open_cnf_sent : Indicates if the session open confirmation is sent to
                                client. This is used to send session events to clients
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_session_id_type            session_id;
  mmgsdi_session_type_enum_type     session_type;
  uint8                             channel_info_index;
  mmgsdi_refresh_reg_data_type      refresh_data;
  mmgsdi_file_update_list_type      file_data;
  mmgsdi_evt_callback_type         *session_evt_cb_ptr;
  boolean                           notify_init;
  boolean                           init_complete;
  boolean                           steering_of_roaming;
  boolean                           steering_complete_rcvd;
  boolean                           is_session_open_cnf_sent;
} mmgsdi_session_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CLIENT_ID_REG_TABLE_TYPE

   DESCRIPTION:
     The structure contains client registration information
     client_id                   : Unique ID assigned to client upon mmgsdi_client_id_reg or
                                   mmgsdi_client_id_and_evt_reg
     evt_cb_ptr                  : Event callback from card related events
     num_session                 : Number of session that the session_info_ptr has
     session_info_ptr            : Array of allocated memory location for num_session of session
                                   related information
     cmd_rsp_cb_ptr              : Callback pointer used to match with client_id_reg for
                                   clean up purpose
     is_client_evt_reg_cnf_sent  : Indicates if the event registration cnf is sent to client
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_client_id_type      client_id;
  mmgsdi_client_name_type    client_task_name;
  mmgsdi_client_name_type    client_name;
  mmgsdi_evt_callback_type * evt_cb_ptr;
  mmgsdi_len_type            num_session;
  mmgsdi_session_info_type * session_info_ptr[MMGSDI_MAX_SESSION_INFO];
  mmgsdi_callback_type       cmd_rsp_cb_ptr;
  boolean                    is_client_evt_reg_cnf_sent;
} mmgsdi_client_id_reg_table_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_NODE_TYPE

   DESCRIPTION:
     Global Data populated when a refresh command is received
     client_id            : Client ID
     session_id           : Session ID
     session_type         : Session type
     app_type             : App type
     stage                : Refresh Stage for each node
     refresh_files        : Files as recived in FCN from the card
     rsp_received         : Boolean to determine if client has responded
     pdown_cnf            : Boolean to identify if client in also interested in pdown cfm.
                            Needed to transition from OK_TO_FCN to CLINET_CNF or START
     next_ptr             : Pointer to next refresh node
-------------------------------------------------------------------------------*/
typedef struct mmgsdi_refresh_node{
  mmgsdi_client_id_type                     client_id;
  mmgsdi_session_id_type                    session_id;
  mmgsdi_session_type_enum_type             session_type;
  mmgsdi_app_enum_type                      app_type;
  mmgsdi_refresh_stage_enum_type            stage;
  mmgsdi_internal_refresh_file_list_type    refresh_files;
  boolean                                   rsp_received;
  boolean                                   pdown_cnf;
  struct mmgsdi_refresh_node               *next_ptr;
}mmgsdi_refresh_node_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_REFRESH_INFO_TYPE

   DESCRIPTION:
     Global Data populated when a refresh command is received
     request_header       : header information stored from the original request
     mode                 : Refresh Mode as received from the card
     aid                  : Application ID as received from the card
     refresh_data         : Refresh data as received by the card,
                            contents may get modified based on the mode
                            and details of the command.
     additional_files_read: boolean to track if files other than FCN files
                            have been read
     steering_mode_data   : PLMNwAcT List
     node_ptr             : head to the linked list of all refresh nodes
     refresh_status       : Final refresh status that will be send to the card
     orig_mode            : Original mode as received from the card
     refresh_retry_files  : Files to add to the retry table if req is voted down
     retry_count          : Current refresh's retry attempts
     source               : Source of Refresh
-------------------------------------------------------------------------------*/
typedef struct{
  mmgsdi_request_header_type      request_header;
  mmgsdi_refresh_mode_enum_type   mode;
  mmgsdi_aid_type                 aid;
  mmgsdi_refresh_reg_data_type    refresh_data;
  boolean                         additional_files_read;
  mmgsdi_data_type                steering_mode_data;
  mmgsdi_refresh_node_type        *node_ptr;
  mmgsdi_return_enum_type         refresh_status;
  mmgsdi_refresh_mode_enum_type   orig_mode;
  mmgsdi_data_type                refresh_retry_files;
  uint32                          retry_count;
  mmgsdi_refresh_source_enum_type source;
}mmgsdi_refresh_info_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_JCDMA_GET_CARD_INFO_REQ_TYPE

   DESCRIPTION:    This structure is used to build a jcdama get card info
                   request to the MMGSDI Task.

   MEMBERS:        mmgsdi_request_header_type      request_header

---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
} mmgsdi_jcdma_get_card_info_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an mmgsdi_session_act_or_switch
     prov request for the MMGSDI Task.
     request_header:    Activate or Switch Provisioning Request Header info
     card_slot:         Slot containing the card with the provisioning app
     app_id_data:       Application ID of provisioning app
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_slot_id_enum_type        card_slot;
  mmgsdi_static_data_type         app_id_data;
} mmgsdi_session_act_or_switch_prov_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_DEACT_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an mmgsdi_session_deact
     request for the MMGSDI Task.
     request_header:      Activate or Switch Provisioning Request Header info
     internal_deact_req:  Flag indicating internal deactivate request.
     in_priority_queue:   Flag indicating internal/implicit deactivate request.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  boolean                         internal_deact_req;
  boolean                         in_priority_queue;
  boolean                         is_reason_mismatch;
} mmgsdi_session_deact_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_INFO_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating an mmgsdi_session_get_info request
     for the MMGSDI Task.

   request_header:      Header information for Session Get info request
   session_info_query:  Query type and key.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_session_info_query_type  session_info_query;
} mmgsdi_session_get_info_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ_TYPE

   DESCRIPTION:
   request for the MMGSDI Task.

  request_header: Header information for Session prov app init complete request
  init_status:    The initialization is successful.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  boolean                         init_status;
} mmgsdi_session_prov_app_init_complete_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_RUN_GSM_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a Session RUN GSM Algo request
     for the MMGSDI Task.

     request_header:       Request Header information
     rand_val:             Length and data of RAND
     illegal_or_perso_ok:  Process the command in illegal or perso failed state
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type     request_header;
  mmgsdi_data_type               rand_val;
  boolean                        illegal_or_perso_ok;
} mmgsdi_session_run_gsm_algo_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_ENABLE_SERVICE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a BDN or ACL enable request
     for the MMGSDI Task.
     request_header:  BDN or ACL Enable Request Header information
     service:         BDN or ACL Service
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
  mmgsdi_service_enum_type      service;
} mmgsdi_session_enable_service_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_DISABLE_SERVICE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a BDN or ACL disable request
     for the MMGSDI Task.
     request_header:  BDN ir ACL disable Request Header information
     service:         BDN or ACL Service
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type        request_header;
  mmgsdi_service_enum_type          service;
} mmgsdi_session_disable_service_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_READ_CACHE_FILE_SIZE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a read cache file size req type

     request_header:  Read Cache File Size Request Header information
     file_name:       The enum used to access the SIM/USIM/RUIM/CSIM
                      Elementary Files.
     file_size_ptr:   The pointer to the buffer to store the file
                      size. The memory should be allocated and
                      provided by the caller.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
  mmgsdi_file_enum_type         file_name;
  mmgsdi_len_type             * file_size_ptr;
} mmgsdi_session_read_cache_file_size_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_READ_CACHE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a read cache req type

     request_header:    Read Cache Request Header information
     file_name:         The enum used to access the SIM/USIM/RUIM/CSIM
                        Elementary Files.
     read_data:         Buffer length and pointer. Used to store the
                        data to be read. The memory should be allocated
                        and provided by the caller.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
  mmgsdi_file_enum_type         file_name;
  mmgsdi_data_type              read_data;
} mmgsdi_session_read_cache_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_WRITE_CACHE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a write cache file size req type

     request_header:    Write Cache File Size Request Header information
     file_name:         The enum used to access the SIM/USIM/RUIM/CSIM
                        Elementary Files.
     write_data:        Buffer length and pointer. Used to store the
                        data to be written.
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
  mmgsdi_file_enum_type           file_name;
  mmgsdi_data_type                write_data;
} mmgsdi_session_write_cache_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_CPHS_INFO_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a get cphs info request

     request_header:   Get CPHS info Request Header information
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
} mmgsdi_session_get_cphs_info_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a get app capabilities request

     request_header:   Get app capabilities request Header information
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type      request_header;
} mmgsdi_session_get_app_capabilities_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_OTAPA_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a OTAPA request

     request_header:   Get app capabilities request Header information
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type        request_header;
  uint8                             start_stop;        /* OTAPA Start Stop */
  dword                             randseed;          /* Random seed */
  dword                             esn;               /* ESN or Pseudo ESN */
} mmgsdi_session_otasp_otapa_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_SSD_UPDATE_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a SSD Update request

     request_header :   SSD Update request Header information
     randssd        :   56 bits from BS for SSD Generation
     process_control:   process control bits
     esn            :   32 bits ESN
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  qword                                randssd;
  uint8                                process_control;
  dword                                esn;
} mmgsdi_session_ssd_update_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_SSD_CONFIRM_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a SSD Confirm request

     request_header:   SSD Confirm request Header information
     authbs        :   Auth Signature from Base Station
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  dword                                authbs;
} mmgsdi_session_ssd_confirm_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_BS_CHAL_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a BS challenge request

     request_header:   BS CHAL request Header information
     randsseed     :   Random number generator seed
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type           request_header;
  dword                                randseed;
} mmgsdi_session_bs_chal_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_MS_KEY_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a OTASP MS Key request

     request_header:   MS Key request Header information
     randseed      :   Seed used to generate TRUE random no
     a_key_p_rev   :   A-KEY-P_REV supported by BS
     param_p       :   Param P
     param_g       :   Param G
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_data_type                 randseed;
  uint8                            a_key_p_rev;
  mmgsdi_data_type                 param_p;
  mmgsdi_data_type                 param_g;
} mmgsdi_session_otasp_ms_key_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_KEY_GEN_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a OTASP request

     request_header:   Key GEN request Header information
     bs_result     :   BS Result
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  mmgsdi_data_type                 bs_result;
} mmgsdi_session_otasp_key_gen_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ_CNF

   DESCRIPTION:
     This structure is used when preparing a OTASP SSPR Config request

     request_header:   SSPR Config request Header information
     block_id      :   block id
     req_offset    :   Requested offset
     req_max_size  :   Requested Max size in the response
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
  uint8                            block_id;
  word                             req_offset;
  uint8                            req_max_size;
} mmgsdi_session_otasp_sspr_config_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_COMMIT_REQ_CNF

   DESCRIPTION:
     This structure is used when preparing a OTASP MS Key request

     request_header:   Commit request Header information
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type       request_header;
} mmgsdi_session_otasp_commit_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_OTASP_BLOCK_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a OTASP block request

     request_header:   OTASP block request Header information
     block_op_type :   OTASP block type
     block_id      :   Block id
     block_data    :   Block data
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type        request_header;
  mmgsdi_otasp_block_op_enum_type   block_op_type;
  uint8                             block_id;
  mmgsdi_data_type                  block_data;
} mmgsdi_session_otasp_block_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_READ_PRL_REQ_TYPE

   DESCRIPTION:
     This structure is used when reading the PRL

     request_header:   READ PRL request Header information
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type        request_header;
} mmgsdi_session_read_prl_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SET_SIM_PROFILE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating the set profile request for the
     MMGSDI Task.
     request_header:                    Set SIM Profile Request Header information
     activate_profile_id:               ID of profile to activate
-------------------------------------------------------------------------------*/

typedef struct {
  mmgsdi_request_header_type     request_header;
  mmgsdi_profile_id_enum_type    activate_profile_id;
}mmgsdi_set_sim_profile_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_SIM_PROFILE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating the get sim profile request
     for the MMGSDI Task.
     request_header:                    Set SIM Profile Request Header information
-------------------------------------------------------------------------------*/

typedef struct {
  mmgsdi_request_header_type     request_header;
}mmgsdi_get_sim_profile_req_type;

/* --------------------------------------------------------------
   STRUCTURE:      MMGSDI_RECOVERY_IND_REQ_TYPE

   DESCRIPTION:
     This structure is used when determining non provisioning sessions to be
     closed and sending notification to mmgsdi task during recovery procedure.

     request_header:   Request Header information
     num_of_sessions:  number of sessions to be closed
     session_id_ptr:   list of non-prov. sessions to be closed (number of
                       sessions described in num_of_sessions)
---------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
  uint32                        num_of_sessions;
  mmgsdi_session_id_type       *session_id_ptr;
} mmgsdi_recovery_ind_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_GET_EONS_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a get eons request

     request_header:           EONS request Header information
     plmn_id_list:             List of PLMN IDs
     ignore_display_condition: ignore display condition enum type
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type             request_header;
  mmgsdi_plmn_id_list_type               plmn_id_list;
  mmgsdi_eons_ignore_disp_cond_enum_type ignore_display_condition;
} mmgsdi_session_get_eons_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_SE13_PLMN_NAMES_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a get SE13 plmn names request

     request_header:   Get SE13 plmn names request Header information
     plmn_id_list:        List of PLMN IDs
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;
  mmgsdi_plmn_id_list_type    plmn_id_list;
} mmgsdi_get_se13_plmn_names_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SET_BUILTIN_PLMN_LIST_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a set builtin plmn list request

     request_header      : Set builtin plmn list request Header information
     builtin_plmn_listt :  List of Builtin PLMN IDs
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
  mmgsdi_builtin_plmn_list_type builtin_plmn_list;
} mmgsdi_set_builtin_plmn_list_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ_TYPE

   DESCRIPTION:
     This structure is used when preparing a get SE13 plmn info by name request

     request_header  : Get SE13 plmn info by name request Header information
     plmn_long_name  : PLMN long name
     plmn_short_name : PLMN short name
     ret_type        : RAT type
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;
  mmgsdi_data_type            plmn_long_name;
  mmgsdi_data_type            plmn_short_name;
  mmgsdi_rat_enum_type        rat_type;
} mmgsdi_get_se13_plmn_info_by_name_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_EONS_SS_EVENT_REQ_TYPE

   DESCRIPTION:
     This structure is used when ss event is received

     request_header:   Header information
     ss_info:          ss event data
---------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type  request_header;
  mmgsdi_eons_ss_data_type    ss_info;
} mmgsdi_eons_ss_event_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_FULL_SERVICE_REQ_TYPE

   DESCRIPTION:
     This structure is used when CM SS event is received

     request_header:   Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type    request_header;
} mmgsdi_full_service_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the MMGSDI Task.
     request_header  : Status request header information
     reg_req         : File update registration request
  -------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type         request_header;
  mmgsdi_file_update_list_type       file_update_list;
} mmgsdi_session_register_file_update_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the MMGSDI Task.
     request_header  : Status request header information
  -------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type         request_header;
} mmgsdi_gstk_loc_env_rsp_received_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the MMGSDI Task.
     request_header  : Status request header information
     update_req_type : Type of update of CSG ID requested
     plmn_csg_id     : CSG ID to be updated and associated PLMN ID
     hnb_name        : Home Node(b) name corresponding to the CSG ID
  -------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type         request_header;
  mmgsdi_update_acsgl_enum_type      update_req_type;
  mmgsdi_plmn_csg_id_type            plmn_csg_id;
  mmgsdi_data_type                   hnb_name;
} mmgsdi_session_update_acsgl_record_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the MMGSDI Task.
     request_header  : Status request header information
     nv_context      : Nv context on which refresh is complete.
     reference_id    : Reference id of the MCFG trigger
  -------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
  mmgsdi_nv_context_type     nv_context;
  uint32                     reference_id;
} mmgsdi_continue_after_nv_refresh_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_HANDLE_NV_REFRESH_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the MMGSDI Task.
     request_header  : Status request header information
     type                  : NV Refresh type - slot or session
     slot_index          : Slot on which refresh is triggered
     sub_index          : session on which refresh is triggered
     reference_id       : MCFG reference identifier
                                - provided back in done API call
  -------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type request_header;
  mcfg_refresh_type_e_type   type;            /**< Refresh for slot or sub specific items */
  mcfg_refresh_index_type    slot_index;      /**< Slot index for this refresh */
  mcfg_refresh_index_type    sub_index;       /**< Subscription index for this refresh */
  uint32                     reference_id;    /**< Reserved. */
} mmgsdi_handle_nv_refresh_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_MANAGE_CHANNEL_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a MANAGE CHANNEL request
     for the MMGSDI Task.
------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type   request_header;
  mmgsdi_channel_id_type       op_close_channel; /* Channel to open or close */
  uim_mc_action_type           mc_action;        /* Manage Channel Open or Close */
} mmgsdi_manage_channel_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_START_SILENT_FILE_CHANGE_REQ_TYPE

   DESCRIPTION:
     This structure is used when GSTK notifies MMGSDI that a file may have changed
     without REFRESH being triggered.
     request_header :  Request Header information
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_request_header_type     request_header;
} mmgsdi_start_silent_file_change_req_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TASK_CMD_DATA_TYPE

   DESCRIPTION:
     MMGSDI command type, to be put into the mmgsdi response command queue

-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_cmd_enum_type         cmd_enum;
  union {
    /* Request */
    mmgsdi_request_header_type                      request_header;
    mmgsdi_client_id_reg_req_type                   client_id_reg_req;
    mmgsdi_client_id_dereg_req_type                 client_id_dereg_req;
    mmgsdi_client_id_and_evt_dereg_req_type         client_id_and_evt_dereg_req;
    mmgsdi_available_apps_req_type                  available_apps_req;
    mmgsdi_pin_operation_req_type                   pin_operation_req;
    mmgsdi_read_req_type                            read_req;
    mmgsdi_write_req_type                           write_req;
    mmgsdi_increase_req_type                        increase_req;
    mmgsdi_get_file_attr_req_type                   get_file_attr_req;
    mmgsdi_session_open_req_type                    session_open_req;
    mmgsdi_session_open_ext_req_type                session_open_ext_req;
    mmgsdi_session_open_with_mf_req_type            session_open_with_mf_req;
    mmgsdi_session_close_req_type                   session_close_req;
    mmgsdi_session_deact_req_type                   session_deact_req;
    mmgsdi_status_req_type                          status_req;
    mmgsdi_send_apdu_req_type                       send_apdu_req;
    mmgsdi_set_file_status_req_type                 rehab_req;
    mmgsdi_set_file_status_req_type                 invalidate_req;
    mmgsdi_sap_connect_req_type                     sap_connect_req;
    mmgsdi_sap_disconnect_req_type                  sap_disconnect_req;
    mmgsdi_sap_get_atr_req_type                     sap_get_atr_req;
    mmgsdi_sap_send_apdu_req_type                   sap_send_apdu_req;
    mmgsdi_sap_power_on_req_type                    sap_power_on_req;
    mmgsdi_sap_power_off_req_type                   sap_power_off_req;
    mmgsdi_sap_card_reader_status_req_type          sap_card_reader_status_req;
    mmgsdi_sap_reset_req_type                       sap_reset_req;
    mmgsdi_card_pup_req_type                        card_pup_req;
    mmgsdi_card_pdown_req_type                      card_pdown_req;
    mmgsdi_refresh_req_type                         refresh_req;
    mmgsdi_activate_onchip_sim_req_type             act_onchip_sim_req;
    mmgsdi_select_aid_req_type                      select_aid_req;
    mmgsdi_get_all_pin_status_req_type              pin_status_req;
    mmgsdi_srv_available_req_type                   srv_available_req;
    mmgsdi_auth_req_type                            auth_req;
    mmgsdi_compute_ip_req_type                      compute_ip_req;
    mmgsdi_run_cave_req_type                        run_cave_req;
    mmgsdi_cdma_aka_req_type                        cdma_aka_req;
    mmgsdi_generate_key_req_type                    generate_key_req;
    mmgsdi_card_reset_req_type                      card_reset_req;
    mmgsdi_get_atr_req_type                         get_atr_req;
    mmgsdi_bcast_req_type                           bcast_req_type;
    /* Response */
    mmgsdi_uim_report_rsp_type                      uim_report_rsp;
    mmgsdi_client_id_reg_rsp_type                   client_id_reg_rsp;
    mmgsdi_generic_rsp_type                         sap_report_rsp;
    mmgsdi_simlock_rsp_type                         simlock_rsp;
    mmgsdi_simlock_evt_type                         simlock_evt;
    mmgsdi_callback_cnf_type                       *mmgsdi_cnf_ptr;
    mmgsdi_jcdma_get_card_info_req_type             jcdma_card_info_req;
    mmgsdi_search_req_type                          search_req;
    mmgsdi_session_act_or_switch_prov_req_type      session_act_or_switch_prov_req;
    mmgsdi_session_get_info_req_type                session_get_info_req;
    mmgsdi_session_prov_app_init_complete_req_type  session_prov_app_init_complete_req;
    mmgsdi_session_run_gsm_algo_req_type            session_run_gsm_algo_req;
    mmgsdi_session_enable_service_req_type          session_enable_service_req;
    mmgsdi_session_disable_service_req_type         session_disable_service_req;
    mmgsdi_session_read_cache_file_size_req_type    session_read_cache_file_size_req;
    mmgsdi_session_read_cache_req_type              session_read_cache_req;
    mmgsdi_session_write_cache_req_type             session_write_cache_req;
    mmgsdi_session_store_esn_req_type               session_store_esn_req;
    mmgsdi_session_manage_illegal_subscription_req_type
                                                    session_manage_illegal_subscription_req;
    mmgsdi_session_get_cphs_info_req_type           session_get_cphs_info_req;
    mmgsdi_session_get_app_capabilities_req_type    session_get_app_cap_req;
    mmgsdi_session_ssd_update_req_type              session_ssd_update_req;
    mmgsdi_session_ssd_confirm_req_type             session_ssd_confirm_req;
    mmgsdi_session_bs_chal_req_type                 session_bs_chal_req;
    mmgsdi_session_otasp_ms_key_req_type            session_otasp_ms_key_req;
    mmgsdi_session_otasp_key_gen_req_type           session_otasp_key_gen_req;
    mmgsdi_session_otasp_sspr_config_req_type       session_otasp_sspr_config_req;
    mmgsdi_session_otasp_commit_req_type            session_otasp_commit_req;
    mmgsdi_session_otasp_otapa_req_type             session_otasp_otapa_req;
    mmgsdi_session_otasp_block_req_type             session_otasp_block_req;
    mmgsdi_session_read_prl_req_type                session_read_prl_req;
    mmgsdi_session_sub_ok_req_type                  session_sub_ok_req;
    mmgsdi_tp_dl_complete_req_type                  tp_dl_complete_req;
    mmgsdi_session_get_eons_req_type                session_get_eons_req;
    mmgsdi_eons_ss_event_req_type                   eons_ss_event_req;
    mmgsdi_full_service_req_type                    simlock_ss_event_req;
    mmgsdi_notify_link_est_req_type                 link_est_req;
    mmgsdi_notify_card_error_req_type               card_error_req;
    mmgsdi_notify_sim_busy_req_type                 sim_busy_req;
    mmgsdi_internal_pup_init_req_type               internal_pup_init_req;
    mmgsdi_session_register_file_update_req_type    file_update_req;
    mmgsdi_recovery_ind_req_type                    recovery_ind_req;
    mmgsdi_gstk_loc_env_rsp_received_req_type       gstk_loc_env_rsp_received_req;
    mmgsdi_get_se13_plmn_names_req_type             get_se13_plmn_names_req;
    mmgsdi_session_update_acsgl_record_req_type     update_acsgl_record_req;
    mmgsdi_get_se13_plmn_info_by_name_req_type      get_se13_plmn_info_by_name_req;
    mmgsdi_set_builtin_plmn_list_req_type           set_builtin_plmn_list_req;
    mmgsdi_continue_after_nv_refresh_req_type       continue_after_nv_refresh_req;
    mmgsdi_handle_nv_refresh_req_type               mcfg_refresh_data;
    mmgsdi_manage_channel_req_type                  manage_channel_req;
    mmgsdi_set_sim_profile_req_type                 set_sim_profile_req;
    mmgsdi_get_sim_profile_req_type                 get_sim_profile_req;
    mmgsdi_start_silent_file_change_req_type        start_silent_file_change_req;
  } cmd;
} mmgsdi_task_cmd_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_TASK_CMD_DATA_TYPE

   DESCRIPTION:
     The common command header for all commands received by MMGSDI task
-------------------------------------------------------------------------------*/
typedef struct {
  q_link_type                 link;
  mmgsdi_task_cmd_data_type   cmd;
}mmgsdi_task_cmd_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_SESSION_INT_DATA_TYPE

   DESCRIPTION:
     Data Structure to hold session application data between intermediate
     session management requests
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_aid_type               orig_aid;
  mmgsdi_aid_type               new_aid;
  mmgsdi_request_header_type    req_header;
}mmgsdi_session_int_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_MEID_DATA_TYPE

   DESCRIPTION:
     Data Structure to hold MEID value.
-------------------------------------------------------------------------------*/

typedef struct {
  qword                      meid;
  boolean                    meid_available;
} mmgsdi_meid_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_UIM_CMD_DATA_TYPE

   DESCRIPTION:
     Data Structure to info on command sent to UIM.
-------------------------------------------------------------------------------*/

typedef struct {
  boolean                    cmd_in_progress;
  int32                      index;
} mmgsdi_uim_cmd_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_CPHS_INT_INFO_TYPE

   DESCRIPTION:
      CPHS capability information, CPHS services status, and CPHS customer
    service profile status, cphs ONS length and data ptr
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_cphs_info_type      cphs_info; /**< CPHS info. */
  mmgsdi_len_type            cphs_ons_len;
  uint8                    * cphs_ons_ptr;
  mmgsdi_len_type            cphs_ons_short_len;
  uint8                    * cphs_ons_short_ptr;
} mmgsdi_cphs_int_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_CANCEL_INFO_TYPE

   DESCRIPTION:
      Used for information needed in cancel APIs
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_cancel_enum_type    cancel_type;
  union {
    mmgsdi_client_id_type    client_id;
    mmgsdi_session_id_type   session_id;
  } id;
} mmgsdi_cancel_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_1X_PRL

   DESCRIPTION:
      Used for storing information related to PRL
-------------------------------------------------------------------------------*/
typedef struct {
  boolean                    prl_validated;
  mmgsdi_read_prl_data_type* prl_data_ptr;
} mmgsdi_1x_prl_type;

/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/

/* --------------------------------------------------------------------------
   MACRO:       MMGSDI_DEBUG_MSG_ERR
   DESCRIPTION: When FEATURE_MMGSDI_DEBUG_ERRORS is define, an ERROR_FATA
                is called so that the Errors can be caught and further
                analyzed.
                These Errors are not logged to NV.
   -------------------------------------------------------------------------*/
#define MMGSDI_DEBUG_MSG_ERROR_0(str)        UIM_MSG_ERR_0("MMGSDI: " str)
#define MMGSDI_DEBUG_MSG_ERROR_1(str,a)      UIM_MSG_ERR_1("MMGSDI: " str,a)
#define MMGSDI_DEBUG_MSG_ERROR_2(str,a,b)    UIM_MSG_ERR_2("MMGSDI: " str,a,b)
#define MMGSDI_DEBUG_MSG_ERROR_3(str,a,b,c)  UIM_MSG_ERR_3("MMGSDI: " str,a,b,c)

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_CMD

   DESCRIPTION:
     This function allows client to put command into the MMGSDI command queue.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_cmd ( mmgsdi_task_cmd_type * task_cmd_ptr  );

/*===========================================================================
   FUNCTION:      MMGSDI_CMD_EXT

   DESCRIPTION:
     This function allows client to put command into the MMGSDI command queue
     specific to a MMGSDI task

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                              because the max number of commands are already
                              queued.

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_cmd_ext(
  mmgsdi_task_cmd_type * task_cmd_ptr,
  mmgsdi_task_enum_type  mmgsdi_task
);

  /*  GSDI Dog variables */
extern dog_report_type gsdi_dog_rpt_var[MMGSDI_NUM_TASKS]; /* Initial Safety */
extern uint32          gsdi_dog_rpt_time_var[MMGSDI_NUM_TASKS]; /* Initial Safety */

extern mmgsdi_uim_cmd_data_type mmgsdi_uim_cmd[MMGSDI_NUM_TASKS];

extern boolean         mmgsdi_sap_connected[MMGSDI_MAX_NUM_SLOTS];

extern mmgsdi_refresh_data_info_type         mmgsdi_refresh_data_info;

extern mmgsdi_client_id_reg_table_type      *mmgsdi_client_id_reg_table[MMGSDI_MAX_CLIENT_INFO];

extern mmgsdi_uim_client_req_user_data_type *mmgsdi_client_req_table_info_ptr[MMGSDI_MAX_CLIENT_REQ_INFO];

extern mmgsdi_slot_data_type                *mmgsdi_data_slot_ptr[MMGSDI_MAX_NUM_SLOTS];

extern mmgsdi_generic_data_type             *mmgsdi_generic_data_ptr;

extern mmgsdi_int_app_info_type             *mmgsdi_app_pin_table_ptr[MMGSDI_MAX_NUM_SLOTS][MMGSDI_MAX_EXT_APP_INFO];

extern mmgsdi_app_pin_info_type             *mmgsdi_app_pin1_table_ptr[MMGSDI_MAX_NUM_SLOTS][MMGSDI_MAX_PIN1_INFO];

extern mmgsdi_app_pin_info_type             *mmgsdi_app_univ_pin_table_ptr[MMGSDI_MAX_NUM_SLOTS];

extern mmgsdi_refresh_info_type             *mmgsdi_refresh_info_ptr[MMGSDI_MAX_NUM_SLOTS];

extern mmgsdi_refresh_retry_settings_type   *mmgsdi_refresh_retry_settings_ptr[MMGSDI_MAX_NUM_SLOTS];

/* The list of files that we have refreshed for*/
extern mmgsdi_file_enum_type                *mmgsdi_refresh_gw_reg_file_list_ptr;

extern mmgsdi_file_enum_type                *mmgsdi_refresh_1x_reg_file_list_ptr;

extern mmgsdi_channel_info_type             *mmgsdi_channel_info_ptr_table[MMGSDI_MAX_CHANNEL_INFO];

/* Pointers to 1X PRL DATA */
extern mmgsdi_1x_prl_type                    mmgsdi_1x_prl[MMGSDI_MAX_NUM_SLOTS];

extern mmgsdi_cnf_type                      *mmgsdibt_pipe_apdu_ptr[MMGSDI_MAX_NUM_SLOTS];

extern rex_timer_type                        mmgsdi_refresh_timer[MMGSDI_MAX_NUM_SLOTS];

extern rex_timer_type                        mmgsdi_refresh_retry_timer;

extern rex_timer_type                        mmgsdi_refresh_auto_trigger_timer[MMGSDI_MAX_NUM_SLOTS];

extern rex_timer_type                        mmgsdi_uim_sanity_timer[MMGSDI_NUM_TASKS];

extern rex_timer_type                        mmgsdi_timed_sleep_timer[MMGSDI_NUM_TASKS];

/* Array of queues to hold card status commands for mmgsdi tasks */
extern q_type                                mmgsdi_card_status_cmd_q[MMGSDI_NUM_TASKS];

/* Array of queues to hold high priority commands for mmgsdi tasks */
extern q_type                                mmgsdi_high_pri_cmd_q[MMGSDI_NUM_TASKS];

/* Array of queues to hold internal commands for mmgsdi tasks */
extern q_type                                mmgsdi_internal_cmd_q[MMGSDI_NUM_TASKS];

/* Array of queues to hold commands for mmgsdi tasks */
extern q_type                                mmgsdi_task_cmd_q[MMGSDI_NUM_TASKS];

/* Array of queues to hold responses for mmgsdi tasks */
extern q_type                                mmgsdi_task_rsp_q[MMGSDI_NUM_TASKS];

/* Queue to hold refresh commands for the main mmgsdi task */
extern q_type                                mmgsdi_refresh_cmd_q;

#ifdef FEATURE_SIMLOCK
/* Queue to hold simlock responses for the main mmgsdi task */
extern q_type                                mmgsdi_simlock_rsp_q;
#endif /* FEATURE_SIMLOCK */

/* Queue to hold NV responses*/
extern q_type                                mmgsdi_nv_q;

/* Critical section to be used to protect accesses to Client, Session,
   Channel, Application, PIN and Slot info globals */
extern rex_crit_sect_type                   *mmgsdi_client_app_data_crit_sect_ptr;

extern rex_crit_sect_type                   *mmgsdi_task_sync_crit_sect_ptr;

extern rex_crit_sect_type                   *mmgsdi_task_state_crit_sect_ptr;

/* ----------------------------------------------------------------------------
   MMGSDI_APDU_GET_RSP_TABLE

   DESCRITPION:
     Table will hold the Get response caches. The get response caches will
     be managed by the client ID and the channel ID for which the GET
     Response was generated on.

   USAGE:
     This table will use accessor functions to add and delete entries:
     mmgsdi_gen_get_rsp_table_retrieve() - Retrieve APDU Get Response
     mmgsdi_gen_get_rsp_table_add()      - Cache the APDU Get Response
     mmgsdi_gen_get_rsp_table_flush()    - Remove all APDU Get Responses

   This table should never be accessed directly.
   ------------------------------------------------------------------------- */
extern mmgsdi_apdu_data_get_rsp_cache_type   *mmgsdi_apdu_get_rsp_table_ptr[MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES];


extern mmgsdi_uim_report_rsp_type * mmgsdi_internal_synch_uim_rpt_rsp[MMGSDI_NUM_TASKS];

extern mmgsdi_cphs_int_info_type * mmgsdi_cphs_info_ptr[MMGSDI_MAX_NUM_SLOTS];

extern mmgsdi_task_state_enum_type mmgsdi_task_state[MMGSDI_NUM_TASKS];

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_CLIENT_BUILD_REFRESH_REQ

DESCRIPTION
  This API is used by the Generic SIM Toolkit Task to request that a REFRESH
  be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 7 USIM
  Refresh Modes (as defined by the enum gsdi_refresh_modes_enum_type).

  MMGSDI_REFRESH_NAA_INIT_FULL_FCN      (SIM/USIM)
  MMGSDI_REFRESH_FCN                    (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT_FCN           (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT               (SIM/USIM)
  MMGSDI_REFRESH_RESET                  (SIM/USIM)
  MMGSDI_REFRESH_NAA_APP_RESET          (USIM)
  MMGSDI_REFRESH_3G_SESSION_RESET       (USIM)

  This API is used for Client based REFRESH requests.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type.

SIDE EFFECTS
  Depending on the Mode requested, various events are generated as a result
  of the REFRESH Peformed.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_client_build_refresh_req(
  mmgsdi_client_id_type           client_id,
  mmgsdi_slot_id_enum_type        card_slot,
  const mmgsdi_refresh_req_type * req_ptr,
  boolean                         register_for_steering,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref,
  boolean                         requeue_flag
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_SESSION_BUILD_REFRESH_REQ

DESCRIPTION
  This API is used by the Generic SIM Toolkit Task to request that a REFRESH
  be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 7 USIM
  Refresh Modes (as defined by the enum gsdi_refresh_modes_enum_type).

  MMGSDI_REFRESH_NAA_INIT_FULL_FCN      (SIM/USIM)
  MMGSDI_REFRESH_FCN                    (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT_FCN           (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT               (SIM/USIM)
  MMGSDI_REFRESH_RESET                  (SIM/USIM)
  MMGSDI_REFRESH_NAA_APP_RESET          (USIM)
  MMGSDI_REFRESH_3G_SESSION_RESET       (USIM)

  This API is used for Session based REFRESH requests.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type.

SIDE EFFECTS
  Depending on the Mode requested, various events are generated as a result
  of the REFRESH Peformed.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_build_refresh_req(
  mmgsdi_session_id_type          session_id,
  const mmgsdi_refresh_req_type * req_ptr,
  boolean                         register_for_steering,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
);

/*===========================================================================
   FUNCTION MMGSDI_NAA_REFRESH_EXT

   DESCRIPTION
     This API is used by the Generic SIM Toolkit Task to request that a REFRESH
     be performed on the Mobile with refresh source.  There are 5 GSM SIM
     Refresh Modes and 7 USIM Refresh Modes (as defined by the enum
     mmgsdi_refresh_mode_enum_type).

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
     option:             refresh option
     source:             refresh source
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
mmgsdi_return_enum_type mmgsdi_naa_refresh_ext(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_aid_type                    aid,
  mmgsdi_refresh_mode_enum_type      refresh_mode,
  mmgsdi_data_type                   refresh_data,
  uint8                              num_files,
  mmgsdi_alpha_string_type           *alpha_string_ptr,
  mmgsdi_option_type                 option,
  mmgsdi_refresh_source_enum_type    source,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);

/*===========================================================================
FUNCTION MMGSDI_WAIT

DESCRIPTION
  Main wait routine for the GSDI task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals). The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type mmgsdi_wait(
 rex_sigs_type requested_mask
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_EVT_CB

DESCRIPTION
  Internal Event Callback function registered during mmgsdi_init so that
  MMGSDI can receive event notification

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  State variables may be updated as needed
===========================================================================*/
void mmgsdi_evt_cb (
  const mmgsdi_event_data_type *event_ptr
);

/* ----------------------------------------------------------------------------
FUNCTION MMGSDI_GET_ME_CONFIG_PARAM_FROM_NV

DESCRIPTION
This function gets the me capabilities for fdn, bdn and acl.
Reads the nv_item and sets the global data. If the nv_item is not enabled, then
the function writes the defaul value to the nv. If the nv_item could not be
read, a failure is return.

DEPENDENCIES
  None

RETURN VALUE
 mmgsdi_return_enum_type
 MMGSDI_SUCCESS : Upon successful read of nv item
 MMGSDI_ERROR   : Upon a nv read/write failure

SIDE EFFECTS
 If nv operation is not succesful, this function returns error and can put
 the me to no card.
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_get_me_config_params_from_nv(void);

/*===========================================================================
FUNCTION MMGSDI_STATE_UPDATE_CARD_ERROR_STATE

DESCRIPTION
  This function updates the card state and the app states when the CARD_ERROR
  event is received

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_state_update_card_error_state(
  mmgsdi_slot_id_enum_type slot_id);

/*===========================================================================

FUNCTION MMGSDI_MALLOC

DESCRIPTION
  mmgsdi_malloc calls mem_malloc to request memory from the tmc_heap or tmc_small_heap
  depending on whether or not the request greater than 64KB if
  FEATURE_MMGSDI_USE_SMALL_HEAP is defined and FEATURE_LOW_MEMORY_USAGE is not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * mmgsdi_malloc(
  dword size
);

/*===========================================================================

FUNCTION MMGSDI_FREE

DESCRIPTION
  The mmgsdi_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void mmgsdi_free(
  void * ptr
);

/*===========================================================================
FUNCTION MMGSDI_OPEN_BASE_SESSIONS

DESCRIPTION
  Function to add an entry for session id during mmgsdi_init.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_open_base_sessions(
  void
);

/*===========================================================================

FUNCTION MMGSDI_PARSE_REFRESH_FILE_LIST

DESCRIPTION
  This function parses the refresh files from refresh registration request to the FORMAT reuired by mmgsdi.

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_parse_refresh_file_list(
  mmgsdi_refresh_file_list_type  refresh_files,
  mmgsdi_refresh_req_type      * req_ptr
);

/*===========================================================================
FUNCTION MMGSDI_GET_MEID

DESCRIPTION
  It returns cached MEID data.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_meid_data_type

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_meid_data_type mmgsdi_get_meid(void);

/*===========================================================================

FUNCTION MMGSDI_CACHE_MEID_DATA

DESCRIPTION
  Caching MEID data from NV memory.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_cache_meid_data(void);

/*===========================================================================
FUNCTION MMGSDI_SET_ENS_SUPPORT_FLAG

DESCRIPTION
  Check to see if HPPLMN Scan feature is supported.
  Set the support flag to 1
  Scan Control feature is indicated by the first byte in  EF-Terminal Support Table
  Network Selection menu is indicated by the second byte in the body of the EF-Terminal Support Table
  Acting HPLMN Rescan feature is indicated by the third byte in the body of the EF-Terminal Support Table

  PARAMS:
    session_id      : Client Id of the caller
    slot            : Card Slot
    app_type        : Application Type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_set_ens_support_flag(
  mmgsdi_session_id_type     session_id,
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_app_enum_type       app_type
);

/*===========================================================================
FUNCTION MMGSDI_STATE_UPDATE_APP_STATE_FOR_PROV_APP

DESCRIPTION
  This function finds the appropriate provisioning application pointer and
  update its app state to the appropriate state

DEPENDENCIES
  This function should be called only for provisioning related state changes
  with a valid provisioning session id

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_state_update_app_state_for_prov_app(
  mmgsdi_session_id_type      session_id,
  mmgsdi_app_state_enum_type  constraint_state,
  mmgsdi_app_state_enum_type  new_state,
  mmgsdi_app_state_enum_type *curr_state_ptr
);

/*===========================================================================
FUNCTION mmgsdi_gstk_terminal_profile_cb

DESCRIPTION
  Callback function called by GSTK to indicate completion of TP download.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_gstk_terminal_profile_cb
(
  gstk_cmd_from_card_type * gstk_tp_response_ptr
);

/*===========================================================================
FUNCTION MMGSDI_BUILD_RECOVERY_IND_REQ

DESCRIPTION
  This function builds RECOVERY IND command and queues it to MMGSDI

DEPENDENCIES
  None

LIMITATIONS:
  None

RETURN VALUE
  mmgsdi_return_enum_type
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_build_recovery_ind_req(
  mmgsdi_slot_id_enum_type  slot_id,
  mmgsdi_protocol_enum_type protocol
);

/*===========================================================================
FUNCTION MMGSDI_IS_PERSO_COMPLETED

DESCRIPTION
  Simply indicates whether or not the GSDI Personalization Initialization
  procedures have been completed.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_is_perso_completed(
  mmgsdi_session_id_type
);

/*===========================================================================
FUNCTION MMGSDI_IS_ENS_ENABLED_IN_NV

DESCRIPTION
  This function checks if ens is enabled in NV.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_is_ens_enabled_in_nv(void);

/*================================================================
FUNCTION  MMGSDI_FREE_CLIENT_REQ_DATA_FOR_PENDING_COMMANDS

DESCRIPTION:
  This function is called at LINK_EST to check if there
  are any pending commands that did not receive response from UIM.
  If there are such commands, the corresponding client_req_table
  entry is freed. An error cnf is supposed to have been sent
  for such commands at CARD_ERROR

  INPUT PARAMETERS:
    slot_id

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
void mmgsdi_free_client_req_data_for_pending_commands
(
  mmgsdi_slot_id_enum_type   slot_id
);

/*================================================================
FUNCTION  MMGSDI_QUEUE_ERROR_CNF_FOR_PENDING_COMMANDS

DESCRIPTION:
  This function is called at card error event to check if there
  are any pending commands that did not receive response from UIM.

  INPUT PARAMETERS:
    slot_id

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
void mmgsdi_queue_error_cnf_for_pending_commands
(
  mmgsdi_slot_id_enum_type   slot_id
);

/*===========================================================================
FUNCTION MMGSDI_HANDLE_QMI_UIM_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process the signal
  from QMI UIM.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_handle_qmi_uim_sig
(
  void
);

#ifndef FEATURE_UIM_TEST_FRAMEWORK
/*===========================================================================
FUNCTION MMGSDI_HANDLE_QMI_MODEM_TASK_READY

DESCRIPTION
  Function called to handle the task-ready notification from RCEVT for
  QMI MODEM task. This notification is received when QMI MODEM task calls
  rcevt_signal_name(). This function should only be called once during
  task initialization.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
void mmgsdi_handle_qmi_modem_task_ready
(
  void
);
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================
FUNCTION MMGSDI_ADD_CLIENT_ID_ENTRY

DESCRIPTION
  Function to find a valid free spot in the client ID registration table
  and create a client ID in the table.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None.

CRITICAL SECTIONS:
  The calling function must enter mmgsdi_client_app_data_crit_sect_ptr crit
  sect before invoking this function.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_add_client_id_entry(
  mmgsdi_client_id_type       *client_id_out_ptr,
  mmgsdi_callback_type         cmd_rsp_cb_ptr,
  mmgsdi_client_name_type      client_name);

/*===========================================================================
FUNCTION MMGSDI_PROCESS_CANCEL_REQ

DESCRIPTION
  This function is used to purge any pending repuests that the client wants
  MMGSDI to cancel. The 2 APIs that could lead to this are:
  mmgsdi_cancel_requests: Cancels all pending requests for the client
  mmgsdi_session_cancel_requests: Cancels only the specific session id
  related requests for the client.

  As a result of the cancel request, an error response is sent to each of the
  pending requests that were cleaned up.

DEPENDENCIES
 None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_cancel_req
(
  mmgsdi_cancel_info_type   cancel_info
);

/*===========================================================================
FUNCTION mmgsdi_present_in_preinit_file_access_list

DESCRIPTION
  This function, is called from mmgsdi_process_command and will be invoked
  during an attempt to access EFs during SIM initialization.
  This function is important in limiting access to only certain files during
  the initialization process. At present the files include EF DIR, EF ELP
  EF GSM LP and EF CDMA LP.

DEPENDENCIES
  None.

LIMITATIONS
  None.

RETURN VALUE
  boolean
  TRUE          - If file is found,
  FALSE         - If file is not found

SIDE EFFECTS
  NONE
===========================================================================*/
boolean mmgsdi_present_in_preinit_file_access_list
(
  const mmgsdi_task_cmd_data_type *cmd_ptr
);

/*===========================================================================
FUNCTION MMGSDI_CM_INIT

DESCRIPTION
  Function called to register with CM

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
void mmgsdi_cm_init
(
  void
);

/*===========================================================================
FUNCTION MMGSDI_BUILD_QUEUE_STATIC_CNF

DESCRIPTION
  This function, builds a default confirmation response in case of an error
  function.

DEPENDENCIES


LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_build_and_queue_static_cnf(
  int32                    index,
  mmgsdi_return_enum_type  status
);

/*=======================================================================
FUNCTION  MMGSDI_IS_QMI_UIM_PROCESSING

DESCRIPTION:
  This function is called to check whether MMGSDI MAIN TASK is processing
  QMI UIM SIG.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  Boolean

SIDE EFFECTS:
  None
=======================================================================*/
boolean mmgsdi_is_qmi_uim_processing
(
  const rex_tcb_type *task_tcb_ptr
);

#endif /* MMGSDI_H */
