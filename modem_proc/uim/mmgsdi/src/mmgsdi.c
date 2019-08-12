/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

                            M A I N     T A S K


GENERAL DESCRIPTION

  This source file contains the main GSDI task along with supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/25/16   dd      Added support for auto reg SMS when ESN is changed
09/07/16   vdc     Extend full and expired service policies to any slot
08/02/16   gm      Support for CDMALess Device
07/20/16   shr     Support for NV based Toolkit disablement
07/08/16   av      Send SELECT ISDR to determine if card is eUICC or not
07/08/16   rp      F3 reduction
07/05/16   nr      Added support for 256 bands (LTE U related)
06/20/16   ar      Don't RESET card if EF-DIR req is not processed by UIMDRV
05/31/16   lm      Added support to detect multi profile card based on EFs
05/30/16   lm      Determine number of profiles based on card response
05/24/16   bcho    EONS F3 reduction
05/22/16   vdc     Remove F3 messages for memory allocation failure
05/18/16   nr      Redirect requests to NV during QCN Restore operation
05/17/16   tkl     F3 log prints cleanup
05/16/16   ar      F3 reduction
05/16/16   ar      No need to send PDOWN if EF-DIR fails for eUICC card
05/12/16   lm      Remove excessive logging
05/11/16   sp      Reduce F3 logging
05/04/16   lm      Add EF SMSCAP to RUIM and CSIM cache list
05/02/16   sp      Reduce F3 log prints
04/05/16   vdc     Send act resp and session changed evt at same time
03/26/16   av      Clear ATR related globals at card error/link est
01/07/16   kv      UIM PSM handling updates
01/06/16   bcho    Don't delete attr cache if not re-cached during NAA Refresh
01/04/16   bcho    Use q_linear_delete_new to search TP DL request in queue
12/25/15   bcho    Send Card and session events based on SFS data in PSM mode
11/16/15   nr      Adding MMGSDI_TELECOM_ADN to refresh reg file list
11/23/15   vdc     Send profile switching evt when profile has switched in card
11/05/15   ks      Rel12 polling enhancement
10/30/15   ar      Don't send cnf during flush if the rsp_status is PROCESSING
10/12/15   vdc     Add support for potential silent file change notification
09/25/15   vr      Added support for ERA-GLONASS
09/24/15   vv      Check EF-TST length before setting the data
09/18/15   ar      Make MDN related EF read before SIM Lock has been validated
09/09/15   stv     Pack imsi_m activation status into subscription_ready_evt
09/08/15   bcho    Run SIMLOCK Algo in Onchip SIM
08/27/15   vdc     Do not retry service registration if it fails once
07/22/15   vdc     Synchronize main task with slot tasks at boot up
07/20/15   nr      Block search request when deactivation in progress
07/14/15   ar      Check for uim_rsp_status before setting it to NOT_RECEIVED
07/14/15   ar      Handle sending double confirmation to client
07/09/15   ar      Validate PRL at diff steps if SD task not ready
06/29/15   hh      Return generic error for non-permission related reasons
06/25/15   ar      Fix compiler warnings
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
05/29/15   nr      Fixing memoory leak during card powerdown and task stop
05/18/15   nr      Fix to distiguish reads on different apps in single slot
05/15/15   hh      UE supports LTE only if at least one LTE band is enabled
04/29/15   hh      Differentiate CSG support for W and LTE for EF-TST
04/17/15   nr      Phonebook record read optimization
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
03/26/15   ar      Acquire APP_DATA_CRIT_SECT if non GSDI task
03/25/15   ar      Allow back to DEACT of different apps on same slot
12/09/14   ar      Replace mmgsdi_u/icc_rsp_srv_available with common func
11/15/14   ar      Reduce usage of MMGSDI_TASK_ENTER/LEAVE_CRIT_SECT
11/14/14   hh      Fix compilation warnings
11/06/14   hh      Call GSTK NV API to read GSTK features in NV 65674
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
10/12/14   hh      Set EF-TST Byte 7 based on both 67218 and 66048
09/29/14   tkl     Added support for persistent cache
09/09/14   kk      Enhancements to NV handling
09/29/14   tl      Add 3GPP2 IMSI M/T to preinit file access list
09/27/14   ar      Add support for multiple slots in BT-SAP
09/25/14   tl      Add SimLock featurization to run algo
09/21/14   ar      Add check for task_state before accessing its globals
09/18/14   bcho    Handle QMI UIM SIG as if processed by non-MMGSDI task
09/11/14   hh      Call rex_undef_timer() when timer is no longer needed
09/04/14   kk      Fix compilation error when LTE is disabled
09/04/14   ar      Remove NVRUIM cache
08/29/14   hh      Fix compliation warnings
08/28/14   tl      Split QMI initalization into separate functions
08/27/14   tl      Introduce network related SIM Lock slot policies
08/19/14   hh      Always send confirmation once updating UPP files ends
08/12/14   yt      Remove uim_rsp_report
08/08/14   ar      Queue SESSION_DEACTIVATE_REQ to high priority queue
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/31/14   hh      Set call_active as long as the call process started
07/29/14   tl      Add GID1 and GID2 to preinit file access list
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/15/14   yt      Support for NV refresh
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
07/10/14   vv      Allow FETCH for UICC cards after card_inserted event
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
07/07/14   vv      Remove Perso code
07/04/14   vdc     Added support to check whether LTE RAT is available
06/23/14   yt      Send STATUS before app deactivation
06/16/14   av      Delete common attributes cache at card error
06/11/14   vv      Fix for a possible memory leak
06/03/14   tl      Remove additional parameter variables
05/21/14   am      Extend critical section to completely queue a cmd
05/12/14   vv      Fix compiler errors
05/12/14   bcho    NULL check added before dereferencing extra_param_ptr
04/27/14   am      Do not directly access timer structure elements
04/15/14   ar      Skip sending CNF if there is CARD_ERROR while processing
04/09/14   am      Properly flush cmds in queues
04/09/14   tl      Check perso state before sending perso event
04/08/14   am      Release mutex correctly even in case of error
04/07/14   av      Replace hardcoded tcbs with rex_self where possible
04/06/14   am      Added mmgsdi heap alloc clean up logic
04/03/14   am      Fix compile error on SBM due to assert_on_crit_sect_owner
04/01/14   av      NVRUIM headers cleanup
03/28/14   kb      Update function assert_on_crit_sect_owner
03/27/14   yt      Ensure usage of correct slot id
03/26/14   tl      SIM Lock secondary revisions
03/25/14   tl      Revert changes causing queues to not correctly flush
03/24/14   ar      Remove MMGSDI_SESSION_ENABLE/DISABLE_FDN_REQ
03/19/14   am      Added leave crit sect logic
03/19/14   am      Queue error response only if MMGSDI Task is ACTIVE
03/18/14   tl      Introduce new SIM Lock feature
02/26/14   am      Set the running MMGSDI task to ACTIVE in MMGSDI context
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/19/14   am      Added logic for clearing timers and queues
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
02/13/14   ar      Replace session checks with utility functions
02/13/14   at      Fix for KW Critical errors
02/10/14   df      Fix off target compilation error
02/10/14   ar      Replace session checks with utility functions
02/10/14   df      Allow client dereg if only prov or card session open
02/04/14   am      Corrected CRIT SECT CHECK
01/31/14   df      Set GSTK fetch for non-ATT cards
01/30/14   vdc     Optimize read trans req to read from cache before SIM init
12/05/13   tkl     SFI Support and decouple uim file enums
01/29/14   df      Change legacy include of uim diag header files
01/29/14   df      Improve mmgsdi_remove_requests_in_queue_send_cnf
01/28/14   df      Use q_cnt() instead directly accessing cnt
01/21/14   yt      Remove duplicate code
01/10/14   df      Use v2.0 diag macros
01/09/14   vv      Added comment about the usage of byte 6 in EF-TST
01/09/14   ar      Remove Off Target featurization to cover functionality
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
01/07/14   at      Fix task index check when removing requests in progress with UIMDRV.
12/19/13   yt      Support for new CM SS event interface
12/19/13   ar      Featurization for mmgsdi_malloc() and mmgsdi_free()
12/11/13   df      Remove unused memory paging feature
12/05/13   ar      Replaced all mmgsdi_malloc direct calls with macro
12/04/13   spo     Do not set signals when MMGSDI task is already stopped
11/27/13   tl      Introduce refresh command queue
11/22/13   tl      Check pre-init file access list during app state wait for sub ok
11/22/13   vv      Added input parameter check for the CSG ID
11/21/13   vv      Halt 3gpp subscription if 3gpp2 perso fails
11/18/13   vv      Added support for parallel REFRESH
11/08/13   yt      Reduce size of MMGSDI tables
11/07/13   tl      Remove TO-DO
11/05/13   tkl     deregister watch dog when break from main task
11/01/13   ar      Update GSTK slot after tpdl completion response
10/28/13   kb      Add offtarget support to check mutex is owned or not
10/27/13   df      Send error_evt during sap connect rsp
10/21/13   tkl     Prevent SRV available request when PIN is not verified
10/21/13   df      Ignore sanity timer timeout during sim busy
10/18/13   vv      Removed FEATURE_MMGSDI_OP_MODE_SLAVE
10/18/13   yt      Clear refresh retry queue for deactivated application
10/17/13   at      Support for MMGSDI Cancel APIs
10/16/13   df      Removing functions(some unused, some legacy gsdi conversion)
10/16/13   tl      Remove MMGSDI_FEATURE_SAP
08/28/13   tkl     Rename FEATURE_UIM_SGLTE
10/08/13   tl      Additional conditions for updating EF RPM VI during initalization
10/07/13   vdc     Remove debug message while handling task stop signal
10/04/13   rp      F3 message reduction
10/04/13   vv      Update EF-TST bytes based on the respective NV values
10/03/13   yt      Remove duplicate code
10/03/13   vdc     Improvements in NITZ storage in EFS
10/01/13   av      Perform task-ready sig registrations before rcinit_handshake
10/01/13   vr      Move break statement of TASK_STOP_SIG under
                   FEATURE_MODEM_RCINIT flag
09/27/13   tl      Fix KW errors
09/23/13   vr      qtf-crm MOB migration
09/23/13   df      Support for SIM busy
09/10/13   am      Fixed Klocwork warnings
09/05/13   yt      Remove usage of secapi_get_random retry from perso
09/05/13   yt      Remove usage of rand() to generate client and session IDs
08/30/13   yt      Fix Klocwork errors
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/29/13   vv      Update EF-TST to indicate support for IMS
08/28/13   yt      Deprecate VIRTUIM/VIRTSAT module
08/26/13   am      Fix MMGSDI Sanity timer expiry processing
08/20/13   spo     Automatic Subscription provisioning enhancements in UIM
08/09/13   kk      Deprecating uimcallctrl and moving task init functions
08/07/13   tl      Add CDMA AKA Authenticate functionality
08/06/13   at      Init critical sections by main task sooner
07/29/13   am      Added support to determine CSG support dynamically
07/24/13   av      Block writing to the files being refreshed for INIT+FFCN
07/15/13   vs      EONS updates to support single baseband SGLTE
07/02/13   kb      Added support for memory leak detection
06/28/13   bcho    APIs added to return plmn info from SE13 table.
06/27/13   yt      New signal for card status command queue
06/27/13   spo     Added support for files under DF TELECOM
06/26/13   av      Use MMGDSI session ID when processing SUB_OK request
06/25/13   yt      Send AUTH command to card ignore_app_state flag is TRUE
06/24/13   vdc     Added support for triple SIM
06/12/13   yt      Fix compiler warning
05/30/13   akv     Heart beat mechanism for Watchdog
05/28/13   tl      Check status OTASP Commit before clearing cache
05/28/13   spo     Replaced instances of memcpy with memscpy
05/21/13   vv      Added support for session closed cause
05/21/13   yt      Fix Klocwork errors
05/17/13   av      Halt 3gpp2 subscription of 3gpp perso fails
05/16/13   vv      Add EF_NASCONFIG to cache
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
05/10/13   at      Handle QMI Modem task SIG during sync commands to UIM Driver
05/08/13   bcho    Added support to provide rplmn-spn display bit and network
                   info in get operator name API cnf and in EONS event
05/06/13   spo     Reduce memory used by client request table
05/06/13   spo     Allocate memory for CPHS variables during runtime
05/06/13   spo     Memory optimization for get response Cache
04/29/13   at      Handle QMI UIM SIG during sync commands to UIM Driver
04/25/13   yt      Fix compilation for non-CDMA builds
04/24/13   vdc     Clear refresh retry tbl when CARD_ERROR is received on slot1
04/17/13   av      Prioritize access to EFs related to acquistion on 2G cards
04/17/13   av      Prioritize acquisition related EF reads
04/17/13   tl      Replace Featurization with NV
04/16/13   bcho    Modify use of EONS global info data
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
04/12/13   av      Move UIM tasks to group4 of RCINIT
04/12/13   spo     Do not send events to clients before Evt Reg CNF is sent
04/05/13   vv      Fixed determining the task when RCINIT is enabled
04/01/13   spo     Do not send session events before session open CNF is sent
03/13/13   kb      Restart post-pin1 init. with MMGSDI Session ID once
                   SUB_OK is received
03/04/13   av      Write to NV 947 only if necessary
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
02/26/13   at      Added signal handling for OBDAPP
02/14/13   vv      Clear the sanity timer
02/14/13   tl      Move QMI UIM to MMGSDI context from QMI modem context
02/12/13   yt      Support for activation/deactivation of EFs
02/07/13   vv      EONS support for sys mode SYS_SYS_MODE_TDS
02/04/13   at      Move reading NV 4398 to mmgsdi_init_pre_startup
01/28/13   vv      Fix compiler warnings
01/23/13   vv      Update EF-TST to indicate support for CSG
01/20/13   vv      Added support for updating EF-ACSGL record
01/20/13   vv      Added EONS support for CSG
01/07/13   vv      Check the return value when queuing a slot specific command
01/07/13   yt      Update UPP files at power up if not provisioned earlier
01/04/13   vv      Handle NV write command in main task
01/04/13   tl      Fill request type parameter before write transparent
12/30/12   kk      Fixed async TP for few other scenarios
12/30/12   kk      Reworked AsyncTP for faulty RUIM cards having no GSM info
12/26/12   av      Do not allow writing to file that is being refreshed
12/12/12   vv      Added check for task before queuing the command
12/12/12   av      Use SIGREX APIs for registering for task stop with RCINIT
12/06/12   yt      Allow AUTH requests if app state is illegal or perso locked
12/05/12   vv      Added support for parallel processing in MMGSDI
11/06/12   vv      Do not initialize QMI_UIM in FTM mode
11/02/12   tl      CSIM conformance updating EF MECRP
10/21/12   av      Update conf file only if necessary
10/16/12   yt      Add files to mmgsdi_present_in_preinit_file_access_list
10/15/12   vv      Update EF-TST to indicate support for CSG
10/15/12   vv      Added support for updating EF-ACSGL record
10/15/12   vv      Added EONS support for CSG
10/15/12   yt      Queue error response for pending commands at card error
10/15/12   bcho    Handle SS event/get operator name API call in No Card and/or
                   Limited Service scenario
10/11/12   spo     Removed unnecessary F3 messages
10/11/12   bcho    Remove length check while updating rplmn name in NITZ data
10/05/12   yt      Register with RCInit framework for termination signal
10/03/12   abg     Removed Queuing related F3 message
10/01/12   yt      Write digit '0' in correct format while updating UPP files
10/01/12   tl      Moved NV item read to mmgsdi init for optimization
10/01/12   abg     Removed Problem determining protocol type F3 message
09/28/12   yt      Update user profile parameters after successful OTASP Commit
09/28/12   abg     Updated ERR to MSG_ERROR
09/25/12   tl      Move Update EF-RPM from CM context to MMGSDI
09/20/12   bcho    Added support for mmgsdi_get_SE13_plmn_names API
09/05/12   av      Handling of VPE response from the card
09/03/12   bcho    Add EF CPHS ONS and EF CPHS ONS SHORT in refresh list
                   mmgsdi_uicc_rsp_search/mmgsdi_icc_rsp_search calls modified
08/29/12   at      Support for new API mmgsdi_sap_connect_ext
08/16/12   at      Support for new API mmgsdi_session_register_all_for_refresh
08/13/12   av      Allow reading of IMSI when perso is in ready state
07/30/12   av      Fixed error handling of get_app_capabilities_req
07/30/12   tl      Track session id of cmd queue overflow
07/25/12   bcho    Add support for recovery of non prov sessions
07/19/12   tl      Support more application and logical channels
07/13/12   tl      Track clients that overflow queue
06/08/12   nb      Backing out last change
05/25/12   nb      Reworked AsyncTP for faulty RUIM cards having no GSM info
05/24/12   tkl     Delay qmi_sim_internal_init() until task is started.
05/23/12   vv      Added support for the file update notification registration
05/14/12   tkl     Add QMI_SIM_INTERNAL service
05/03/12   yt      Call mmgsdi_init_pre_startup() after RCInit handshake
05/03/12   av      Support store meid offtarget
05/02/12   yt      Refactor mmgsdi_init()
04/27/12   tl      Reverted support for client name for rcinit framework
04/26/12   av      Created nvitem for refresh_vote_ok
04/18/12   bcho    Validate session id while processing get operator name req
04/16/12   tl      Added support for client name traces
03/30/12   av      Replaced feature flags with NVITEM
03/29/12   shr     Added support for session open with MF,
                   support for Send APDU extension API
03/28/12   bcho    Changes to replace rex_sleep calls with non-deferrable timer
03/21/12   yt      Add request type for session_open_with_select_rsp
03/21/12   bcho    External personalization modified
                   1.Send perso event with DONE status if client is ok with
                     publishing subscription.
                   2.Send perso event with FAILURE status if client is not ok
                     with publishing subscription.
03/16/12   yt      Modify references to gsdi_task_tcb
03/07/12   yt      Move RCInit init function to GSDI context and send event
                   to RCInit framework after initialization
03/07/12   bcho    Compilation error removed
03/07/12   bcho    Removal of ERR_FATAL, mmgsdi_channel_info_table_ptr
                   initialized with address of mmgsdi_channel_info_table
03/02/12   av      Removed feature FEATURE_MMGSDI_NV_ME_CONFIG
03/01/12   bcho    Critical section comment block added
02/23/12   nmb     QMI QMUX Transition
02/23/12   av      Cleaned up remaining always ON features
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
02/07/12   nmb     NV Item support for SECURE TZ
01/30/12   shr     Use GSTK NV item (65674) to determine if the OTASP IMSI_M
                   enhancement is enabled or not
01/30/12   bcho    MMGSDI_EONS_SS_EVENT_REQ request processed
01/13/12   at      Added NV item read for NV_DISABLE_CM_CALL_TYPE_I
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   bcho    Legacy GSDI removal updates
12/21/11   kk      Added USIM ECC caching support
12/21/11   bcho    Changes for new API to open multiple sessions
                   in a single request
12/21/11   bcho    Added support for Get Operator Name API
12/07/11   ssr     extern mmgsdi_init_rtre_config function
11/29/11   ssr     Initialized RUIM control at card inserted event
10/28/11   yt      Added new files for cache-related utility functions
10/20/11   yt      Added support for RCInit
10/18/11   av      Removed features that are always OFF
10/13/11   shr     Adding support for full recovery
10/13/11   nb      Support for new refresh stage
09/30/11   av      Updated the pre-pin-verification-read-permitted files list
09/23/11   nb      Async TP enhancement
09/16/11   av      Replace feature #ifdefs with an NVITEM
09/12/11   vv      Support to deselect app upon error in activation
09/08/11   nb      Read EFDIR after TP
08/30/11   js      Check for path len in parsing refresh files
08/11/11   nmb     Extended Logical Channels
08/01/11   av      Slot-specific handling of JCDMA apis
07/27/11   vv      Added RAT and LTE support in TST update for 2G card
07/22/11   vv      Fixed logging appropriate dbg msg for a success or failure
07/14/11   kk      Fixed GSDI diag nv init logic
07/14/11   kk      Fixed compilation error due to wrong merge
07/12/11   at      Added NV item read for NV_SERVICE_DOMAIN_PREF_I
07/11/11   shr     Process MMGSDI_SESSION_SELECT_AID_REQ on the high priority
                   MMGSDI queue to ensure that an application activation is
                   completed before a new activation request is handled
07/07/11   yt      Added ICCID to common cache
06/21/11   kk      ThreadSafe updates from review
06/16/11   yt      Thread safety updates for INCREASE API
06/03/11   shr     Reverted featurization of FS headers
05/26/11   ms      Support for diag nv item is necessary only on target
05/23/11   yt      Added support for INCREASE request
05/23/11   kk      Added registration of uimcallctrl in other card err cases
05/17/11   kk      Updates for Thread safety
05/17/11   shr     Updates for Thread safety
05/16/11   at      Support for mmgsdi.conf file on init
05/11/11   nb      Support for Async TP
05/05/11   ms      Adding Support for diag nv item
04/26/11   vs      Update for subscription ok request
04/26/11   yt      Replaced references to TMC heap with Modem heap
04/26/11   vs      Support for subscription ok request
04/14/11   kk      Updated client force cleanup handling for session based
04/08/11   nmb     Reset uim sanity timer appropriately
03/24/11   yt      Populate cnf with data from read request in case of error
03/04/11   ssr     Fixed compilation error for UMTS only flavor
02/21/11   ms      Implementation of Session based caching on App capabilities
02/18/11   yt      Check if CSIM is provisioned before starting Refresh voting
02/11/11   ssr     Clear RUIM and GSDI cache after successful OTASP commit
02/08/11   ssr     Fixed PRL read for JCDMA case
02/03/11   ms      Updated F3 messages for Client ID
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
01/24/11   js      Handling MMGSDI_TASK_CMD_Q_SIG correctly to avoid accidental
                   reset.
01/21/11   nmb     Added refresh retry support
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/22/10   nb      DO USIM ENS specific initialisation with SESSION_CHANGED event
12/13/10   nb      Do not indicate LTE support in TST update for 2G card
12/06/10   nmb     Update EF_TST with LTE support
12/01/10   yt      Dynamically allocate buffer for static confirmations
11/30/10   yt      Including private GSTK header
11/24/10   js      Resolve race condition between GSDI NV READ/WRITE
11/11/10   shr     Allow file access requests even if the subscription
                   corresponding to the application has been marked as illegal
11/05/10   nmb     Fixed compiler warning
11/02/10   vs      Update to process NVRUIM cmds via MMGSDI high priority q
11/02/10   vs      Remove NVRUIM subscription enablement for CSIM
10/28/10   js      Send RTRE config update for CM when card error occurs.
10/28/10   js      Fix RTRE config update for CM on Fusion
10/25/10   nmb     Additional mmgsdi_nv_q check to ensure command exists in queue
10/22/10   js      Make mmgsdi do malloc from modem_heap
10/14/10   yt      Updated handling of client req table entries in case of error
10/12/10   yt      Dynamic allocation for mmgsdi_client_req_table_info_ptr
10/07/10   yt      Set correct response type for PERSO_REQ in case of error
10/07/10   ms      Fix for Lint Errors
10/05/10   nmb     Removed extra bytes from task_cmd buffer
10/04/10   nmb     Added NV queue to address NV deadlocks
09/29/10   yt      Enabling support for INCREASE
09/20/10   adp     Reject SIM/ USIM authentication request to a client if PIN1 is
                   not verified (on a PIN1 enabled card).
09/17/10   yk      Added support dynamic registration on WDOG.
09/16/10   ms      Fixes for new UT Framework
09/15/10   nmb     Allocate mmgsdi_client_id_reg_table dynamically
09/13/10   shr     Stop queueing and processing commands once TASK_STOP
                   is received
09/10/10   ps      Merge UIM Driver DSDS changes
09/08/10   nmb     mmgsdi_task_cmd_type memory reduction
09/02/10   shr     Clean up Client ID dereg and Session close handling
08/30/10   shr     Perso clean up for Sessions
08/18/10   nmb     Removed compilation warnings
08/18/10   mib     Fixed Lint warnings
08/10/10   nb      Do not Set 5th byte if EF_TST for 2G card
07/26/10   vs      Allow GW sessions when C2K with GSM access is enabled
07/17/10   js      MMGSDI should send OK_TO_TP_DL event on card error to GSTK
                   on remote FUSION target
07/14/10   js      Fixed compilation warnings
07/07/10   yb      Removing dependency on rex from apps side
07/07/10   adp     Memory optimization for mmgsdibt_sap_connect_cnf_data_ptr
07/01/10   nmb     Updates for JCDMA CSIM support
06/28/10   shr     Introduced high priority MMGSDI queue
06/28/10   shr     Added support for Hidden key event
06/25/10   yt      Fixed compiler warnings
06/25/10   shr     Clean-up of Onchip support
06/25/10   shr     Perso commands need to be allowed when no Card is inserted
06/24/10   yt      Klocwork fixes
06/22/10   ssr     Allow RUIM initialization when SIM is failed due to
                   mandatory file missing
06/22/10   js      Corrected featurization around mmgsdi_sap_init
06/19/10   js      Featurized sap init for fusion
06/19/10   vs      Illegal SIM req post-processing
06/17/10   js      Fixed NVRUIM init for fusion
06/16/10   nb      Read RAT only if ENS is enabled
06/10/10   vs      Added HPLMN and SPT to Refresh list
06/09/10   js      Fix NVRUIM init for fusion
06/08/10   nb      Fixed compiler warning
06/03/10   nb      Fixed crash in function mmgsdi_set_ens_support_flag
06/01/10   js      ZI memory reduction fixes
06/01/10   nb      Cache SPT table and ACT HPLMN only when ENS is enabled
05/25/10   shr     Clean-up file access operations on Card Sessions
05/20/10   js      MMGSDI need not register for refresh in slave mode of
                   operation
05/19/10   nb      Added SPT_TABLE and ACT_HPLMN to USIM and SIM cache list.
                   Moved gsdi_set_support_flag to mmgsdi_set_ens_support_flag
05/17/10   nb      Initialize SAP with MMGSDI only for old BT
05/14/10   js      Fix for fusion linker error
05/13/10   vs      Fix for NV and GSDI deadlock
05/04/10   shr     Clear File cache, Record cache and File attributes cache
                   when deactivating a provisioning application
05/03/10   shr     Added Dual Slot support for REFRESH RESET and REFRESH
                   RESET_AUTO
04/28/10   js      Temp fix to allocate memory from modem heap always
04/27/10   shr     Added Hiddenkey support
04/23/10   shr     All files in the cache must be in the REFRESH list
04/15/10   nb      Call mmgsdi_cm_init only when working ontarget
04/09/10   shr     Clean up REFRESH RESET and REFRESH RESET_AUTO
04/08/10   kk      Fix for Sap connection issue
04/07/10   yt      Fixed compilation warnings
04/07/10   nb      SAP Fixes
04/06/10   ssr     Fixed compilation error
03/30/10   adp     Eliminated FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC feature,
                   which was reintroduced because of a merge
03/29/10   shr     Switch/deactivate provisioning clean up and redesign
03/19/10   vs      Fixed slot specific command logic for non-prov sessions
03/12/10   adp     Fixing KW errors
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
03/05/10   yt      Fixed compilation error
03/02/10   kk      Fixed get all available apps command handling
03/01/10   js      Added caching support for LTE specific files
02/26/10   js      Added caching support for LTE specific files
02/19/10   shr     Clear File cache, Record cache and File attributes cache
                   while provisioning a new application
02/16/10   kk      Added support for slot specific common cache
02/01/10   jk      EPRL Support
01/29/10   ssr     Fixed compilation error for non RUIM build
01/28/10   ssr     MMGSDI RTRE Configuration initialization
01/18/10   nk      Added feature around inclusion of uimdiag.h
01/12/10   nb      Update for Release 7 Enhanced Network Selection
01/06/10   mib     Fixed possible race condition in mmgsdilib
01/06/10   shr     Fixed Session Close handling
01/04/10   nb      Compilation fixes
12/23/09   nb      Fixed linker error due to Dual Slot Merge
12/16/09   rn      Declared a new lookup table for DF's and their paths
12/14/09   ssr     Removed MMGSDI_SELECT_AID_REQ,
                   Update CSIM cache file list
12/11/09   kk      Enabling UIMDiag
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
12/10/09   nb      Fixed compilation errors
12/08/09   rn      Added support for send apdu in mmgsdi_is_cmd_session_specific
12/07/09   js      Lint Fixes
12/07/09   mib     IMSI can be read when device is perso locked
12/07/09   js      Added support for MMGSDI_CSIM_ARR file
12/04/09   kp      Replacing mmgsdi_data_slot2 with mmgsdi_data_slot2_ptr
12/03/09   shr     Cache read process enhancement
10/28/09   nb      Release 7 Enhanced Network Selection
10/23/09   kp      Allocating memory for the global gsdidiag pointers.
10/19/09   mib     Fixed memory corruption
10/13/09   kp      Allocating memory in mmgsdi_init for the
                   global bt pointers
10/12/09   kp      Moving Read-Write varibles to Heap
10/01/09   kp      Fixed fatal error
09/26/09   kp      ZI memory reduction changes
09/08/09   rn      Fixed klocwork errors
09/02/09   kk      Fixed Lint Err Boolean within 'if' always evaluates to False
08/28/09   shr     Handle Error scenarios in Client Reg request
09/01/09   js      Remove incorrect featurization for
                   FEATURE_HTORPC_METACOMMENTS
08/24/09   shr     Fixed compliation errors (for CDMA only)
07/20/09   mib     Replaced flag TEST_FRAMEWORK with FEATURE_UIM_TEST_FRAMEWORK
                   Added support for additional LTE files and EF-UST extension
06/30/09   sg      Featurized uim_power_control call in mmgsdi_handle_stop_sig to get qtf working
06/15/09   sg      Updated gsdi_check_perso_fs_procs be invoked for test framework
06/01/09   ssr     Merging of SIM and RUIM PERSO
05/13/09   kk      Eliminated FEATURE_MMGSDI_NO_TCB_PTR_OR_CRIT_SEC feature
05/04/09   mib     Fixed typo in mmgsdi_uicc_activate_onchip_usim name
04/27/09   js      Added prototype for static function, fixed compile warnings
03/26/09   mib     Integration of support for ONCHIP USIM
03/23/09   sun     Added support for Virtuim in QTF
03/19/09   js      Support for Enhanced NAA Refresh by file path
03/18/09   yb      Fixed Lint Errors for loss of sign from long to unsigned long
03/06/09   tml     Clean up refresh pointer when we receive a card error
03/02/09   sun     Fixed Compilation
02/25/09   kk      Define the SECRND Timer
02/24/09   nb      Perso Command support in MMGSDI
02/12/09   kk      Add files to refresh file list for re-reading during FCN
02/03/09   rm      Fix to avoid a possible dangling command pointer access by
                   the UIM task in erroneous situations (SIM interface going bad
                   due to transmission errors leading to multiple recoveries)
01/14/09   mib     Replaced mmgsdi_free with MMGSDIUTIL_TMC_MEM_FREE
01/27/09   sun     Removal of old GSDI Refresh timer
01/21/09   kk      Featurized conditional file access
01/15/09   ssr     Fixed Lint errors for RTRE changes
01/12/09   ssr     MMGSDI RTRE Configuration initialization
01/09/09   tml     ensure MMGSDI_GENERATE_KEY_VPM_REQ response will be sent
                   back to client when the protocol is UICC
01/08/09   tml     Report back to client regardless of whether a card/
                   protocol is known or not
12/18/08   sun     Removed old Perso
12/01/08   ssr     Handled error condition for some MMGSDI Functions
11/19/08   js      Fixed KW error for potential null ptr derferencing
11/18/08   js      Updated featurization for Send APDU related code
11/17/08   nk      Removed const from input in mmgsdi_add_client_id_entry to
                   fix compiler warning
11/15/08   js      Removed MFLO featurization for Send APDU related code
10/29/08   tml     Modem restart client id dereg clean up
10/06/08   jk      Allow GSM SIM access in 1X Only
09/29/08   ssr     Return MMGSDI_ACCESS_DENIED on unfulfilled access condition
09/22/08   js      Fixed Lint Errors
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/26/08   jk      Fixed Featurization for 1X Auth for LTK compilation
08/22/08   js      Klockwork fixes
08/20/08   jk      Support for Generate Key API
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/25/08   sun     Fixed Compilation
06/23/08   tml     Added featurization for BT in WM
06/23/08   sp      Update mmgsdi_util_reset_app_data()
                   with new function parameter in mmgsdi_init.
06/04/08   tml     Added status word to response header.  Messages cleanup
                   and return more appropriate error code when
                   mmgsdi_process_response is unable to process the responses.
                   return NOT SUPPORTED appropriately due to featurization
04/08/08   nk      Added CDMA Refresh Support
03/24/08   sun     Added support for OMA Bcast
03/19/08   nk      Updated call to mmgsdi_icc_rsp_get_file_attr and
                   mmgsdi_icc_rsp_rehabitate
03/14/08   sk      Reverted the change for the usage of signals during
                   sim initialization
02/22/08   tml     Queue get all pin status via library to avoid
                   simultaneous handling of 2 get all pin status at the
                   same time when there is > 1 usim app available
02/21/08   sun     Added CPHS files for FCN Registration
02/14/08   kk      Removed the Usage of signals during sim initialization
01/20/08   sun     If same command is in progress then do not set the task
                   command signal
01/11/08   jar     Don't send a generic error response for a flush queue attempt
                   when the current command has a response pending.
                   For Card Power Up, Set Refresh Orig to MMGSDI_CARD_PUP_REQ.
12/17/07   vs      Featurized function call mmgsdi_icc_run_cave()
12/10/07   sun     Verify PIN Internally when SAP is Disconnected
11/19/07   js      Radio Access Technology Balancing support
11/30/07   sun     Added support for NAA refresh
11/16/07   jk      Reorganized signal processing to fix crash
11/15/07   tml     Use regular rex function for critical sections for
                   mmgsdi_task_rsp_q to prevent gsdi task stuck when synchronous
                   commands were issued
09/23/07   sk      Put a lock around mmgsdi_task_rsp_q
09/10/07   tml     Added MFLO Supports.
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
07/12/07   sk      Fixed boolean return value and more EFs to
                   mmgsdi_present_in_preinit_file_access_list()
06/26/07   sk      Fixed compilation problem.
06/25/07   sk      Prevent certain access to card at particular gsdi states
05/21/07   jk      Do not allow reentry to mmgsdi_recover_after_reset
05/17/07   wli     Added support for JCDMA RUIM
05/13/07   sp      Fixed warning.
05/07/07   sp      Added new mmgsdi malloc/free functions.
05/02/07   sun     Added support for Service Available and USIM Auth
04/24/07   sun     Fixed Warning
04/17/07   sp      Featurize for LTK
04/17/07   sun     Set up a timer if external module other than UIM does not
                   respond
04/03/07   sun     Let the entire GET_PIN_STATUS complete before processing
                   new requests
03/19/07   sun     Fixed Lint Error
02/27/07   jk      Added featurization for Memory Reduction
02/09/07   tml     Ensure confirmation is built and sent to client
01/17/07   sp      Fix thinUI build issues.
01/17/07   pv      Use gsdi_perso_engine_notify_clients_of_perso_event instead
                   of gsdi_perso_notify_clients_of_perso_event  based on
                   the feature FEATURE_MMGSDI_PERSONALIZATION_ENGINE
12/27/06   sp      Back out pin event changes.
12/11/06   tml     Ensure proper event notification for late registered clients
11/14/06   sp      Fixed lint error
11/13/06   sun     Synch MMGSDI any call present flag with CM Info
11/06/06   sp      Proceed with post pin functions only after pin event
                   is sent
10/25/06   tml     Trigger post pin1 from SELECT_AID event only if there is
                   replacement criteria is satisfied.
10/13/06   sun     When the card is powered off, let the card pup
                   go to the card regardless of the slot.
10/02/06   sp      Function to read fdn, bdn and acl from nv
08/20/06   jar     Process the Error Signal before the response.
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/22/06   tml     Compilation fix for 1x
08/16/06   tml     Added ACL support
08/16/06   sun     Fixed Lint
08/14/06   sun     Fixed the request type for static cnfs
07/26/06   tml     Fixed compilation warning
07/26/06   sun     Added support for multi USIM App and MMGSDI PIN
07/07/06   sun     Removed Reference to mc.h
06/12/06   pv      Call mmgsdi_recover_after_reset when
                   GSDI_UIM_RESET_COMPLETE_SIG is received
06/13/06   tml     Fixed compilation issue
06/08/06   tml     Fixed featurization issue
05/23/06   tml     Lint, BDN support and GSM Invalidate and Rehabilitate support
05/19/06   pv      Do not wait for GSDI_TASK_CMD_Q_SIG while a mmgsdi command
                   is in progress.
05/22/06   tml     Fixed lint issue
05/19/06   tml     Fixed compilation warnings
05/15/06   tml     Flush pending mmgsdi command upon receiving UIM error
                   signal
05/03/06   pv      Do now allow processing a GSDI command while a MMGSDI
                   command is in progress.
                   Do not wait for MMGSDI_TASK_CMD_Q_SIG signal while mmgsdi
                   is only waiting for a response.
                   Clear the signals when no more items to look at.
04/11/06   jar     Added ONCHIP SIM Support
03/21/06   tml     lint
03/14/06   sun     Register with CM for PH_IN_USE event.
02/15/06   tml     lint fix
02/16/06   sp      Support for EF_EHPLMN
12/12/05   nk      Merge for ARR caching and by path changes
01/30/05   pv      Do not notify any other clients other than BT when a
                   failure is reported by UIM in passive mode.
01/29/06   tml     Moved mmgsdi_sim_refresh to mmgsdi.c
12/20/05   sun     Added missing commands to mmgsdi_cmd
12/14/05   tml     Moved Confirmations to mmgsdi_cnf.c
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/09/05   sun     Fixed Lint Errors
11/08/05   pv      change mmgsdi_check_card_status - mmgsdi_3G_ready_status_ind
                   and change the parameters to mmgsdi_send_card_status so as
                   to send a 3G session ready status to USIM.
11/03/05   sun     Added support for Refresh
10/28/05   sun     Fixed SAP Disconnect
10/26/05   pv      Do not print an error message when a Refresh is in progress
10/20/05   sun     Added support for BT Deregisteration
10/18/05   tml     Added in_synch info to the cache
09/29/05   tml     Client Deregistration support
08/30/05   jar     Added support to register as a client and
                   to handle status commands to / from the card
08/26/05   pv      Consider MMGSDI_REFRESH_IN_PROGRESS as success
08/26/05   sun     Added support for BT SAP
08/25/05   pv      Added support for powering down and powerering up SIM/USIM
07/26/05   sst     Added MMGSDIBT support
07/18/05   jk      Fixed Compile Issues for 1x Targets
07/14/05   tml     Added original request type
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
05/05/05   tml     initial versoin


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "rex.h"
#include "task.h"
#include <stdlib.h>
#include "uim_msg.h"
#include "err.h"
#include "queue.h"
#include "dsm.h"
#include "memheap.h"
#include "customer.h"
#include "string.h"
#include "uim.h"

#include "intconv.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"

#include "mmgsdi.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_session.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_gen_rsp.h"
#include "mmgsdi_evt.h"
#include "mmgsdilib_p.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_task_init.h"
#ifdef FEATURE_SIMLOCK
#include "mmgsdi_simlock.h"
#endif /* FEATURE_SIMLOCK */
#include "mmgsdi_ss_event.h"
#include "mmgsdi_euicc.h"

#ifdef FEATURE_MMGSDI_3GPP
  /* Interface Files */
#include "sys_cnst.h"
#include "sys_type.h"
#include "sys_stru.h"
#include "timers.h"
#endif /* FEATURE_MMGSDI_3GPP */

#ifdef FEATURE_MMGSDI_3GPP2
#include "target.h"
#include "queue.h"
#endif /* FEATURE_MMGSDI_3GPP2 */

#ifdef FEATURE_FEMTO_CSG
#include "sys_v.h"
#endif /* FEATURE_FEMTO_CSG */

#include "nvruim_p.h"

#include "mmgsdi_sap.h"
#include "mmgsdi_sap_rsp.h"

#include "mmgsdi_cnf.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_ruim_jcdma.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_refresh_rsp.h"
#include "mmgsdi_file_update.h"
#include "mmgsdi_sub_selection.h"
#include "mmgsdi_builtin_plmn.h"

#ifdef FEATURE_GSTK
#include "gstk_p.h"
#endif /* FEATURE_GSTK */
#include "gstk_nv_p.h"

#include "fs_lib.h"
#include "fs_public.h"
#include "secapi.h"
#include "policyman.h"
#include "rcevt_rex.h"
#include "rcinit.h"

#include "qmi_uim.h"
#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
#include "qmi_sim_internal.h"
#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE */

#include "mmgsdi_eons.h"
#include "mmgsdi_eons_nitz.h"
#include "mmgsdi_se13.h"
#include "mmgsdi_csg.h"
#include "obdapp_p.h"
#include "mmgsdi_common.h"
#include "mmgsdi_common_rsp.h"
#include "mmgsdi_1x_upp.h"
#include "mmgsdi_nv_refresh.h"
#include "mmgsdi_pb_cache.h"
#include "mmgsdi_multi_profile.h"
#include "mmgsdi_psm.h"

#ifdef FEATURE_UIM_PERSISTENT_CACHE
#include "mmgsdi_persistent_cache.h"
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

#ifdef FEATURE_LTE
#include "hwio_cap.h"
#endif /* FEATURE_LTE */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

static mmgsdi_return_enum_type mmgsdi_process_client_reg_req(
  mmgsdi_client_id_reg_req_type * req_ptr
);

static mmgsdi_return_enum_type mmgsdi_process_client_dereg_req(
  const mmgsdi_client_id_dereg_req_type * req_ptr
);

static mmgsdi_return_enum_type mmgsdi_process_client_reg_cnf(
  const mmgsdi_client_id_reg_rsp_type   * client_rsp_ptr
);

static mmgsdi_return_enum_type mmgsdi_process_client_dereg_cnf(
  const mmgsdi_client_id_reg_rsp_type   * client_rsp_ptr
);

static mmgsdi_return_enum_type mmgsdi_process_tp_download_complete(
  mmgsdi_tp_dl_complete_req_type * req_ptr
);

static mmgsdi_return_enum_type mmgsdi_process_recovery_ind_req(
  mmgsdi_recovery_ind_req_type* req_ptr,
  mmgsdi_protocol_enum_type     protocol
);

static mmgsdi_return_enum_type mmgsdi_process_tp_dl_complete_response(
  const mmgsdi_uim_report_rsp_type * uim_rsp_ptr
);

static mmgsdi_return_enum_type mmgsdi_build_refresh_req(
  mmgsdi_client_id_type           client_id,
  mmgsdi_session_id_type          session_id,
  mmgsdi_slot_id_enum_type        card_slot,
  const mmgsdi_refresh_req_type * req_ptr,
  boolean                         register_for_steering,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref,
  boolean                         requeue_flag
);

static mmgsdi_return_enum_type mmgsdi_process_get_operator_name(
  mmgsdi_session_get_eons_req_type * req_ptr
);

mmgsdi_record_cache_node_list_type mmgsdi_record_cache_list_node;

mmgsdi_uim_cmd_data_type mmgsdi_uim_cmd[MMGSDI_NUM_TASKS];

/* The sap is connected to the SIM. */
boolean        mmgsdi_sap_connected[MMGSDI_MAX_NUM_SLOTS] = {FALSE};

/* Array of queues to hold card status commands for mmgsdi tasks */
q_type mmgsdi_card_status_cmd_q[MMGSDI_NUM_TASKS];

/* Array of queues to hold high priority commands for mmgsdi tasks */
q_type mmgsdi_high_pri_cmd_q[MMGSDI_NUM_TASKS];

/* Array of queues to hold internal mmgsdi commands for mmgsdi tasks */
q_type mmgsdi_internal_cmd_q[MMGSDI_NUM_TASKS];

/* Arrays of queues to hold commands for mmgsdi tasks */
q_type mmgsdi_task_cmd_q[MMGSDI_NUM_TASKS];

/* Array of queues to hold responses for mmgsdi tasks */
q_type mmgsdi_task_rsp_q[MMGSDI_NUM_TASKS];

/* Queue to hold mmgsdi refresh commands */
q_type mmgsdi_refresh_cmd_q;

#ifdef FEATURE_SIMLOCK
/* Queue to hold mmgsdi simlock responses */
q_type mmgsdi_simlock_rsp_q;
#endif /* FEATURE_SIMLOCK */

/* Queue to hold NV responses*/
q_type mmgsdi_nv_q;

/* Number of commands unable to fit in queue */
uint32 overflow_num = 0;
mmgsdi_client_id_type               overflow_client_id[MMGSDI_MAX_OVERFLOW_SIZE];
mmgsdi_session_id_type              overflow_session_id[MMGSDI_MAX_OVERFLOW_SIZE];

mmgsdi_refresh_info_type           *mmgsdi_refresh_info_ptr[MMGSDI_MAX_NUM_SLOTS];

mmgsdi_refresh_retry_settings_type *mmgsdi_refresh_retry_settings_ptr[MMGSDI_MAX_NUM_SLOTS];

/* Global to store data related to PRL */
mmgsdi_1x_prl_type                  mmgsdi_1x_prl[MMGSDI_MAX_NUM_SLOTS];

#ifdef FEATURE_SIMLOCK
/* Array indicating which SIM Lock features are activated. This will be referenced
   to determine if TMSI PLMN requires validation to protect against man-in-the-middle
   attacks. */
mmgsdi_simlock_status_info_type     mmgsdi_simlock_status_table[SIMLOCK_SLOT_COUNT_MAX][SIMLOCK_CATEGORY_COUNT_MAX];
#endif /* FEATURE_SIMLOCK */

/*==============================================================================
                          MMGSDI CRITICAL SECTIONS
================================================================================
--------------------------------------
1. MMGSDI Critical Section Hierarchy
--------------------------------------
MMGSDI uses multiple Critical Sections and hence Deadlocks are a likely
scenario.
To avoid this, it has to be ensured that whenever MMGSDI acquires
multiple critical sections, it is done in the same order.
To help achieve this a MMGSDI locking hierarchy has been created
by assigning levels to MMGSDI critical sections.

Rules to be followed:
a. When MMGSDI is holding a critical section at level n, it cannot take
   a critical section at a level lower than n.
   (Ex: When holding a Level 2 critical section, a Level 1
    critical section cannot be acquired)
b. When MMGSDI is holding a mutex at level n, it cannot take
   another critical section at level n.
   What this implies is that if 2 critical sections are at the same level,
   they are assumed to be mutually exclusive to each other.If this is
   not the case, the mutex level of these critical sections needs to be
   adjusted by bumping up/bringing down their level as appropriate.

--------------------------------------------------------------------------------
CRIT.       |                    CRITICAL SECTION NAME
SECT. LEVEL |
--------------------------------------------------------------------------------
LEVEL 1     |            mmgsdi_task_sync_crit_sect_ptr
--------------------------------------------------------------------------------
LEVEL 2     |            mmgsdi_client_app_data_crit_sect_ptr
--------------------------------------------------------------------------------
LEVEL 3     |                mmgsdi_cache_crit_sect
--------------------------------------------------------------------------------

--------------------------------------
2. MMGSDI Critical Section Usage
--------------------------------------
The globals that each of the MMGSDI critical sections protect are
listed below.

--------------------------------------------------------------------------------
            CRIT. SECT.             |                    GLOBAL
--------------------------------------------------------------------------------
1. mmgsdi_cache_crit_sect           | Used to protect acceses to
                                    | file/attributes caches
                                    |
                                    | File cache globals:
                                    | a. mmgsdi_common_cache_slot1
                                    | b. mmgsdi_common_cache_slot2
                                    | c. mmgsdi_pri_gw_cache_ptr
                                    | d. mmgsdi_sec_gw_cache_ptr
                                    | e. mmgsdi_pri_1x_cache_ptr
                                    | f. mmgsdi_sec_1x_cache_ptr
                                    |
                                    | File attributes cache globals:
                                    | a. mmgsdi_slot1_common_cache_tbl_ptr
                                    | b. mmgsdi_slot2_common_cache_tbl_ptr
                                    | c. mmgsdi_pri_1x_cache_tbl_ptr
                                    | d. mmgsdi_sec_1x_cache_tbl_ptr
                                    | e. mmgsdi_pri_gw_cache_tbl_ptr
                                    | f. mmgsdi_sec_gw_cache_tbl_ptr
--------------------------------------------------------------------------------
2. mmgsdi_client_app_data_crit_     | Used to protect acceses to
   sect_ptr                         | Client/Session/Channel/App/PIN/Slot
                                    | info tables
                                    |
                                    | a. mmgsdi_client_id_reg_table_ptr
                                    | b. Session Info within the Client ID
                                    |    reg. table
                                    | c. mmgsdi_channel_info_table_ptr
                                    | d. mmgsdi_app_pin_table_ptr
                                    | e. mmgsdi_app_pin1_table_ptr
                                    | f. mmgsdi_app_univ_pin_table_ptr
                                    | g. mmgsdi_data_slot_ptr
                                    |
--------------------------------------------------------------------------------
3. mmgsdi_task_sync_crit_sect_ptr   | Used to ensure various MMGSDI tasks
                                    | are not run in parallel
--------------------------------------------------------------------------------
4. mmgsdi_task_state_crit_sect_ptr  | Used to protect access to MMGSDI task
                                    | state
==============================================================================*/
rex_crit_sect_type  mmgsdi_client_app_data_crit_sect;
rex_crit_sect_type *mmgsdi_client_app_data_crit_sect_ptr               = &mmgsdi_client_app_data_crit_sect;

rex_crit_sect_type  mmgsdi_task_sync_crit_sect;
rex_crit_sect_type *mmgsdi_task_sync_crit_sect_ptr                     = &mmgsdi_task_sync_crit_sect;

rex_crit_sect_type  mmgsdi_task_state_crit_sect;
rex_crit_sect_type *mmgsdi_task_state_crit_sect_ptr                    = &mmgsdi_task_state_crit_sect;

/* Global structure to general info for gsdi task */
mmgsdi_generic_data_type              mmgsdi_generic_data;
mmgsdi_generic_data_type             *mmgsdi_generic_data_ptr          = &mmgsdi_generic_data;

/* Array that represents if MMGSDI task is stopped or not */
mmgsdi_task_state_enum_type           mmgsdi_task_state[MMGSDI_NUM_TASKS];

/* ----------------------------------------------------------------------------
 Table to hold the list of event handlers
------------------------------------------------------------------------------*/
rex_timer_type                        mmgsdi_refresh_auto_trigger_timer[MMGSDI_MAX_NUM_SLOTS];
rex_timer_type                        mmgsdi_refresh_timer[MMGSDI_MAX_NUM_SLOTS];

/* REFRESH retry is supported on slot-1 only. This is the corresponding timer */
rex_timer_type                        mmgsdi_refresh_retry_timer;

/* MMGSDI UIM Interface Sanity Timer */
rex_timer_type                        mmgsdi_uim_sanity_timer[MMGSDI_NUM_TASKS];

/* MMGSDI timed sleep Timer */
rex_timer_type                        mmgsdi_timed_sleep_timer[MMGSDI_NUM_TASKS];

/* MMGSDI data for Slot */
mmgsdi_slot_data_type                *mmgsdi_data_slot_ptr[MMGSDI_MAX_NUM_SLOTS];

mmgsdi_cnf_type                      *mmgsdibt_pipe_apdu_ptr[MMGSDI_MAX_NUM_SLOTS] = {NULL};

mmgsdi_app_pin_info_type *mmgsdi_app_pin1_table_ptr[MMGSDI_MAX_NUM_SLOTS][MMGSDI_MAX_PIN1_INFO];

mmgsdi_app_pin_info_type *mmgsdi_app_univ_pin_table_ptr[MMGSDI_MAX_NUM_SLOTS];

mmgsdi_int_app_info_type *mmgsdi_app_pin_table_ptr[MMGSDI_MAX_NUM_SLOTS][MMGSDI_MAX_EXT_APP_INFO];

/* List of files requested by the card to be refreshed */
mmgsdi_refresh_data_info_type mmgsdi_refresh_data_info;

/* ----------------------------------------------------------------------------
   MMGSDI_APDU_GET_RSP_TABLE

   DESCRITPION:
     Table will hold the Get response caches.  The get response caches will
   be managed by the client ID and the channel ID for which the GET
   Response was generated on.

   USAGE:
     This table will use accessor functions to add and delete entries:
     mmgsdi_gen_get_rsp_table_retrieve() - Retrieve APDU Get Response
     mmgsdi_gen_get_rsp_table_add()      - Cache the APDU Get Response
     mmgsdi_gen_get_rsp_table_flush()    - Remove all APDU Get Responses

   This table should never be accessed directly.
   ------------------------------------------------------------------------- */
mmgsdi_apdu_data_get_rsp_cache_type* mmgsdi_apdu_get_rsp_table_ptr[MMGSDI_MAX_AVAIL_GET_RSP_CACHE_ENTRIES];

mmgsdi_uim_client_req_user_data_type *mmgsdi_client_req_table_info_ptr[MMGSDI_MAX_CLIENT_REQ_INFO];

/*  GSDI Dog variables */
dog_report_type gsdi_dog_rpt_var[MMGSDI_NUM_TASKS] = {0}; /* Initial Safety */

mmgsdi_channel_info_type *mmgsdi_channel_info_ptr_table[MMGSDI_MAX_CHANNEL_INFO];

mmgsdi_file_enum_type mmgsdi_refresh_gw_reg_file_list[MMGSDI_REFRESH_GW_REG_FILES] =
{
  MMGSDI_GSM_AD,        MMGSDI_GSM_AAEM,       MMGSDI_GSM_ACC,        MMGSDI_GSM_BCCH,
  MMGSDI_GSM_EMLPP,     MMGSDI_GSM_FPLMN,      MMGSDI_GSM_HPLMN,      MMGSDI_GSM_HPLMNACT,
  MMGSDI_GSM_IMSI,      MMGSDI_GSM_KC,         MMGSDI_GSM_KCGPRS,     MMGSDI_GSM_LOCI,
  MMGSDI_GSM_LOCIGPRS,  MMGSDI_GSM_LP,         MMGSDI_GSM_OPLMNWACT,  MMGSDI_GSM_PHASE,
  MMGSDI_GSM_PLMN,      MMGSDI_GSM_PLMNWACT,   MMGSDI_GSM_SPN,        MMGSDI_GSM_SST,
  MMGSDI_GSM_ACM,       MMGSDI_GSM_ACM_MAX,    MMGSDI_GSM_PUCT,       MMGSDI_GSM_CBMID,
  MMGSDI_GSM_ECC,       MMGSDI_USIM_ACM_MAX,   MMGSDI_USIM_ACM,       MMGSDI_USIM_PUCT,
  MMGSDI_USIM_ACC,      MMGSDI_USIM_LOCI,      MMGSDI_USIM_PSLOCI,    MMGSDI_USIM_LI,
  MMGSDI_USIM_KCGPRS,   MMGSDI_USIM_AD,        MMGSDI_USIM_CBMID,     MMGSDI_USIM_EHPLMN,
  MMGSDI_USIM_AAEM,     MMGSDI_USIM_EMLPP,     MMGSDI_USIM_EST,       MMGSDI_USIM_FPLMN,
  MMGSDI_USIM_HPLMN,    MMGSDI_USIM_HPLMNWACT, MMGSDI_USIM_IMSI,      MMGSDI_USIM_KC,
  MMGSDI_USIM_KEYS,     MMGSDI_USIM_KEYSPS,    MMGSDI_USIM_PLMNWACT,  MMGSDI_USIM_OPLMNWACT,
  MMGSDI_USIM_RPLMNACT, MMGSDI_USIM_SPN,       MMGSDI_USIM_START_HFN, MMGSDI_USIM_THRESHOLD,
  MMGSDI_USIM_UST,      MMGSDI_USIM_ARR,       MMGSDI_TELECOM_ARR,    MMGSDI_ARR,
  MMGSDI_GSM_CPHSI,     MMGSDI_USIM_CPHSI,     MMGSDI_GSM_CSP,        MMGSDI_USIM_CSP,
  MMGSDI_USIM_7F66_PROP1_RAT,                  MMGSDI_USIM_EHPLMNPI,  MMGSDI_USIM_LRPLMNSI,
  MMGSDI_USIM_EPSLOCI,  MMGSDI_USIM_EPSNSC,    MMGSDI_USIM_7F66_PROP1_ACT_HPLMN,
  MMGSDI_USIM_7F66_PROP1_SPT_TABLE,            MMGSDI_USIM_HIDDENKEY, MMGSDI_USIM_OHNBN,
  MMGSDI_GSM_SPDI,      MMGSDI_GSM_OPL,        MMGSDI_GSM_PNN,        MMGSDI_USIM_NASCONFIG,
  MMGSDI_USIM_SPDI,     MMGSDI_USIM_OPL,       MMGSDI_USIM_PNN,       MMGSDI_USIM_ECC,
  MMGSDI_USIM_ONS,      MMGSDI_USIM_ONS_SHORT, MMGSDI_GSM_ONS,        MMGSDI_GSM_ONS_SHORT,
  MMGSDI_USIM_ACSGL,    MMGSDI_USIM_CSGT,      MMGSDI_USIM_HNBN,      MMGSDI_USIM_OCSGL,
  MMGSDI_USIM_OCSGT,    MMGSDI_TELECOM_ADN
};
mmgsdi_file_enum_type *mmgsdi_refresh_gw_reg_file_list_ptr = mmgsdi_refresh_gw_reg_file_list;

mmgsdi_file_enum_type mmgsdi_refresh_1x_reg_file_list[MMGSDI_REFRESH_1X_REG_FILES] =
{
  MMGSDI_CDMA_ACCOLC,                          MMGSDI_CDMA_IMSI_M,
  MMGSDI_CDMA_IMSI_T,                          MMGSDI_CDMA_TMSI,
  MMGSDI_CDMA_NAM_LOCK,                        MMGSDI_CDMA_MSISDN,
  MMGSDI_CDMA_CC,                              MMGSDI_CDMA_SF_EUIM_ID,
  MMGSDI_CDMA_ECC,                             MMGSDI_CDMA_PREF_LANG,
  MMGSDI_CDMA_PRL,                             MMGSDI_CDMA_SERVICE_PREF,
  MMGSDI_CDMA_RUIM_ID,                         MMGSDI_CDMA_SCI,
  MMGSDI_CDMA_SVC_TABLE,                       MMGSDI_CDMA_OTAPA_SPC_ENABLE,
  MMGSDI_CDMA_ANALOG_HOME_SID,                 MMGSDI_CDMA_HOME_SID_NID,
  MMGSDI_CDMA_ZONE_BASED_REGN_IND,             MMGSDI_CDMA_SYS_REGN_IND,
  MMGSDI_CDMA_DIST_BASED_REGN_IND,             MMGSDI_CDMA_SPC_STATUS,
  MMGSDI_CDMA_CALL_TERM_MODE_PREF,             MMGSDI_CDMA_OTASP_OTAPA_FEATURES,
  MMGSDI_CDMA_UIM_ID_USAGE_IND,                MMGSDI_CDMA_3GPD_SIPPAPSS,
  MMGSDI_CDMA_3GPD_SIPUPP,                     MMGSDI_CDMA_3GPD_MIPUPP,
  MMGSDI_CDMA_3GPD_MIPFLAGS,                   MMGSDI_CDMA_3GPD_TCPCONFIG,
  MMGSDI_CDMA_3GPD_3GPDOPM,                    MMGSDI_CDMA_3GPD_MIPSP,
  MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND,        MMGSDI_CSIM_ECC,
  MMGSDI_CSIM_ACCOLC,                          MMGSDI_CSIM_IMSI_M,
  MMGSDI_CSIM_IMSI_T,                          MMGSDI_CSIM_TMSI,
  MMGSDI_CSIM_NAM_LOCK,                        MMGSDI_CSIM_MSISDN,
  MMGSDI_CSIM_CALL_COUNT,                      MMGSDI_CSIM_SF_EUIM_ID,
  MMGSDI_CSIM_PRL,                             MMGSDI_CSIM_SERVICE_PREF,
  MMGSDI_CSIM_RUIM_ID,                         MMGSDI_CSIM_SSCI,
  MMGSDI_CSIM_CST,                             MMGSDI_CSIM_OTAPA_SPC_ENABLE,
  MMGSDI_CSIM_ANALOG_HOME_SID,                 MMGSDI_CSIM_CDMA_HOME_SID_NID,
  MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND,        MMGSDI_CSIM_CDMA_SYS_REGN_IND,
  MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND,        MMGSDI_CSIM_SPC_STATUS,
  MMGSDI_CSIM_CALL_TERM_MODE_PREF,             MMGSDI_CSIM_OTASP_OTAPA_FEATURES,
  MMGSDI_CSIM_USGIND,                          MMGSDI_CSIM_3GPD_SIPPAPSS,
  MMGSDI_CSIM_3GPD_SIPUPP,                     MMGSDI_CSIM_3GPD_MIPUPP,
  MMGSDI_CSIM_ARR,                             MMGSDI_CSIM_EPRL,
  MMGSDI_CSIM_MAXIMUM_PRL,                     MMGSDI_CSIM_EST,
  MMGSDI_CSIM_AD,                              MMGSDI_CSIM_LANG_IND,
  MMGSDI_CSIM_HIDDEN_KEY,                      MMGSDI_CDMA_HOME_SVC_PVDR_NAME,
  MMGSDI_CSIM_SPN,                             MMGSDI_CSIM_3GPD_3GPDOPM,
  MMGSDI_CSIM_3GPD_MIPSP,                      MMGSDI_TELECOM_ADN,
  MMGSDI_CDMA_SMS_CAP,                         MMGSDI_CSIM_SMS_CAP
};
mmgsdi_file_enum_type *mmgsdi_refresh_1x_reg_file_list_ptr = mmgsdi_refresh_1x_reg_file_list;

mmgsdi_meid_data_type  mmgsdi_data_meid = {{0,0}, FALSE};

mmgsdi_client_id_reg_table_type *mmgsdi_client_id_reg_table[MMGSDI_MAX_CLIENT_INFO];

mmgsdi_cphs_int_info_type *mmgsdi_cphs_info_ptr[MMGSDI_MAX_NUM_SLOTS];

mmgsdi_uim_report_rsp_type *mmgsdi_internal_synch_uim_rpt_rsp[MMGSDI_NUM_TASKS];

RCEVT_SIGEX_SIGREX        mmgsdi_rcevt_siginfo_cm;
RCEVT_SIGEX_SIGREX        mmgsdi_rcevt_siginfo_qmi_modem;

/* To identify that QMI UIM SIG has been received and MMGSDI's Main Task is
   processing it. */
static boolean mmgsdi_process_qmi_uim_sig = FALSE;

/*===========================================================================
FUNCTION MMGSDI_IS_CMD_VALID_UPDATE_PARAMS

DESCRIPTION
  This function validates the input parameters (Client ID/Session ID)
  of a MMGSDI command.
  It also updates the input parameters (Client ID, Slot ID) in the
  command as required.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_is_cmd_valid_update_params(
  mmgsdi_task_cmd_type           * task_cmd_ptr)
{
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_client_id_type              client_id      = 0;
  mmgsdi_session_id_type             session_id     = 0;
  mmgsdi_session_id_status_enum_type session_status = MMGSDI_SESSION_ID_UNASSIGNED;


  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  client_id = task_cmd_ptr->cmd.cmd.read_req.request_header.client_id;
  session_id = task_cmd_ptr->cmd.cmd.read_req.request_header.session_id;

  switch (task_cmd_ptr->cmd.cmd_enum)
  {
    /* Commands which do not require Client ID/Session ID validation */
    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
    case MMGSDI_SESSION_GET_INFO_REQ:
      break;

    /* Client based commands */
    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_OPEN_EXT_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
    case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
    case MMGSDI_CARD_PUP_REQ:
    case MMGSDI_CARD_PDOWN_REQ:
    case MMGSDI_SAP_CONNECT_REQ:
    case MMGSDI_SAP_DISCONNECT_REQ:
    case MMGSDI_SAP_GET_ATR_REQ:
    case MMGSDI_SAP_SEND_APDU_REQ:
    case MMGSDI_SAP_POWER_ON_REQ:
    case MMGSDI_SAP_POWER_OFF_REQ:
    case MMGSDI_SAP_RESET_REQ:
    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
    case MMGSDI_GET_ATR_REQ:
    case MMGSDI_SEND_APDU_REQ:
    case MMGSDI_SEND_APDU_EXT_REQ:
    case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
    case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ:
    case MMGSDI_SET_BUILTIN_PLMN_LIST_REQ:
    /* Though following are internal commands, valid client ID is expected -
       as per design, all commands queued to MMGSDI need to be
       from valid clients */
    case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
    case MMGSDI_NOTIFY_LINK_EST_REQ:
    case MMGSDI_NOTIFY_CARD_ERROR_REQ:
    case MMGSDI_NOTIFY_SIM_BUSY_REQ:
    case MMGSDI_INTERNAL_PUP_INIT_REQ:
    case MMGSDI_MANAGE_CHANNEL_REQ:
    case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
      /* Validate input Client ID */
      mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_3("Cmd: 0x%x, Invalid Client ID: client_id_low = 0x%x, client_id_high = 0x%x",
                      task_cmd_ptr->cmd.cmd_enum,
                      client_id&0xFFFFFFFF,
                      client_id>>MMGSDI_CLIENT_ID_SHIFT & 0xFFFFFFFF);
        return MMGSDI_INCORRECT_PARAMS;
      }
      break;

    /* Session based commands */
    case MMGSDI_SESSION_CLOSE_REQ:
    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
    case MMGSDI_SESSION_DEACTIVATE_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ:
    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
    case MMGSDI_PIN_OPERATION_REQ:
    case MMGSDI_READ_REQ:
    case MMGSDI_WRITE_REQ:
    case MMGSDI_INCREASE_REQ:
    case MMGSDI_GET_FILE_ATTR_REQ:
    case MMGSDI_ISIM_AUTH_REQ:
    case MMGSDI_USIM_AUTH_REQ:
    case MMGSDI_COMPUTE_IP_AUTH_REQ:
    case MMGSDI_RUN_CAVE_REQ:
    case MMGSDI_SESSION_CDMA_AKA_REQ:
    case MMGSDI_GENERATE_KEY_VPM_REQ:
    case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
    case MMGSDI_CARD_STATUS_REQ:
    case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
    case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
    case MMGSDI_BCAST_REQ:
    case MMGSDI_SEARCH_REQ:
    case MMGSDI_SESSION_SSD_UPDATE_REQ:
    case MMGSDI_SESSION_SSD_CONFIRM_REQ:
    case MMGSDI_SESSION_BS_CHAL_REQ:
    case MMGSDI_SESSION_OTASP_MS_KEY_REQ:
    case MMGSDI_SESSION_OTASP_KEY_GEN_REQ:
    case MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ:
    case MMGSDI_SESSION_OTASP_COMMIT_REQ:
    case MMGSDI_SESSION_OTASP_OTAPA_REQ:
    case MMGSDI_SESSION_OTASP_BLOCK_REQ:
    case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
    case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
    case MMGSDI_SESSION_READ_PRL_REQ:
    case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
    case MMGSDI_SRV_AVAILABLE_REQ:
    case MMGSDI_SESSION_SUB_OK_REQ:
    case MMGSDI_EONS_SS_EVENT_REQ:
    case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
    case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
    case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
    case MMGSDI_SESSION_STORE_ESN_REQ:
      /* Validate Input Session ID */
      session_status = mmgsdi_util_is_session_id_valid(session_id);

      if((session_status != MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE) &&
         (session_status != MMGSDI_SESSION_ID_ASSIGNED_NOT_ACTIVE))
      {
        UIM_MSG_ERR_2("Cmd: 0x%x, Input session id: 0x%x is not in use",
                      task_cmd_ptr->cmd.cmd_enum, session_id);
        return MMGSDI_INCORRECT_PARAMS;
      }

      /* Update command with the Client ID corresponding to the Session */
      mmgsdi_status = mmgsdi_util_get_client_id(
                        session_id,
                        &task_cmd_ptr->cmd.cmd.read_req.request_header.client_id);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
      break;

    case MMGSDI_REFRESH_REQ:
      /* Verify if input Session ID is valid and update Client ID in command */
      mmgsdi_status = mmgsdi_util_get_client_id(
                        session_id,
                        &task_cmd_ptr->cmd.cmd.read_req.request_header.client_id);

      /* Obtain slot corresponding to Session */
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_util_get_session_app_info(
                          session_id,
                          &task_cmd_ptr->cmd.cmd.read_req.request_header.slot_id,
                          NULL,NULL,NULL,NULL,NULL);

        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          /*slot may not be assigned yet - ok to continue */
          task_cmd_ptr->cmd.cmd.read_req.request_header.slot_id = MMGSDI_SLOT_AUTOMATIC;
        }
      }
      /* If Session ID is not valid, check if REFRESH command is client based */
      else
      {
        mmgsdi_status = mmgsdi_util_is_valid_client_id(client_id);

        /* If Client ID is not valid, build and Error CNF back to client */
        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          mmgsdi_client_req_extra_info_type extra_param;
          mmgsdi_sw_status_type             status_word;


          UIM_MSG_ERR_3("Cmd: 0x%x, Invalid Client ID: client_id_low = 0x%x, client_id_high = 0x%x",
                        task_cmd_ptr->cmd.cmd_enum,
                        client_id&0xFFFFFFFF,
                        client_id>>MMGSDI_CLIENT_ID_SHIFT & 0xFFFFFFFF);

          /* Initialize locals */
          memset(&extra_param, 0x00, sizeof(extra_param));
          status_word.sw1 = 0x00;
          status_word.sw2 = 0x00;

          extra_param.refresh_data.refresh_req_type =
            task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type;

          mmgsdi_status = mmgsdi_cnf_build_and_queue(
                            MMGSDI_INCORRECT_PARAMS,
                            &task_cmd_ptr->cmd.cmd.read_req.request_header,
                            &extra_param,
                            NULL,
                            FALSE, /* sw not required for refresh */
                            status_word);

          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            UIM_MSG_ERR_0("Could not queue REFRESH CNF back to client");
          }

          return MMGSDI_INCORRECT_PARAMS;
        }
      }
      break;

    /* Internal commands - Used only by MMGSDI */
    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
    case MMGSDI_REHABILITATE_REQ:
    case MMGSDI_INVALIDATE_REQ:
    case MMGSDI_CARD_RESET_REQ:
      break;

    default:
      break;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_is_cmd_valid_update_params */


/*===========================================================================
FUNCTION MMGSDI_IS_CMD_SESSION_SPECIFIC

DESCRIPTION
  This function determines whether the request is session specific or not.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean mmgsdi_is_cmd_session_specific(
  mmgsdi_cmd_enum_type       cmd)
{
  switch (cmd)
  {
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
    case MMGSDI_SAP_REPORT_RSP:
    case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
    case MMGSDI_RECOVERY_IND_REQ:
    case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
    case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
    case MMGSDI_MANAGE_CHANNEL_REQ:
    case MMGSDI_SET_SIM_PROFILE_REQ:
      return FALSE;
    default:
      return TRUE;
  }
} /* mmgsdi_is_cmd_session_specific */


/*===========================================================================
FUNCTION MMGSDI_IS_CMD_SLOT_SPECIFIC

DESCRIPTION
  This function determines whether the request is slot specific or non slot
  related

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean mmgsdi_is_cmd_slot_specific(
  mmgsdi_cmd_enum_type       cmd)
{
  switch (cmd)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
    case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
    case MMGSDI_SAP_CONNECT_REQ:
    case MMGSDI_SAP_DISCONNECT_REQ:
    case MMGSDI_SAP_POWER_ON_REQ:
    case MMGSDI_SAP_POWER_OFF_REQ:
    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
    case MMGSDI_SAP_RESET_REQ:
    case MMGSDI_CARD_PUP_REQ:
    case MMGSDI_CARD_PDOWN_REQ:
    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
    /*Refresh Request needs to be slot unspecific for cases such as when
      refresh is sent as a result of card power up*/
    case MMGSDI_REFRESH_REQ:
    case MMGSDI_CARD_RESET_REQ:
    case MMGSDI_UIM_REPORT_RSP:
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_SESSION_GET_INFO_REQ:
    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_OPEN_EXT_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
    case MMGSDI_SESSION_CLOSE_REQ:
    case MMGSDI_SESSION_SUB_OK_REQ:
    case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
    case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ:
    case MMGSDI_SET_BUILTIN_PLMN_LIST_REQ:
    /* Process GET_OPERATOR_NAME_REQ and EONS_SS_EVENT_REQ also when there is
       no card in slot*/
    case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
    case MMGSDI_EONS_SS_EVENT_REQ:
    case MMGSDI_FULL_SERVICE_REQ:
    case MMGSDI_NOTIFY_LINK_EST_REQ:
    case MMGSDI_NOTIFY_CARD_ERROR_REQ:
    case MMGSDI_NOTIFY_SIM_BUSY_REQ:
    case MMGSDI_INTERNAL_PUP_INIT_REQ:
    case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
    case MMGSDI_SIMLOCK_RSP:
    case MMGSDI_SIMLOCK_EVT:
    case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
    case MMGSDI_HANDLE_NV_REFRESH_REQ:
    case MMGSDI_GET_SIM_PROFILE_REQ:
    case MMGSDI_CNF:
    case MMGSDI_MAX_CMD_ENUM:
      return FALSE;

    case MMGSDI_READ_REQ:
    case MMGSDI_WRITE_REQ:
    case MMGSDI_INCREASE_REQ:
    case MMGSDI_GET_FILE_ATTR_REQ:
    case MMGSDI_SESSION_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_ISIM_AUTH_REQ:
    case MMGSDI_SEND_APDU_REQ:
    case MMGSDI_SEND_APDU_EXT_REQ:
    case MMGSDI_SAP_GET_ATR_REQ:
    case MMGSDI_SAP_SEND_APDU_REQ:
    case MMGSDI_SAP_REPORT_RSP:
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
    case MMGSDI_GET_ATR_REQ:
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
    case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
    case MMGSDI_RECOVERY_IND_REQ:
    case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
    case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
    case MMGSDI_MANAGE_CHANNEL_REQ:
    case MMGSDI_SET_SIM_PROFILE_REQ:
    case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
    /* Note - There is no default case here and it is done on purpose since we
       need to handle all values for mmgsdi_cmd_enum_type. Unhandled values
       will get flagged by lint error */
      if((cmd == MMGSDI_SEND_APDU_REQ ||
          cmd == MMGSDI_SEND_APDU_EXT_REQ ||
          cmd == MMGSDI_MANAGE_CHANNEL_REQ)
         && mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_ALLOW_SEND_APDU_ONLY, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) == MMGSDI_FEATURE_ENABLED)
      {
        UIM_MSG_HIGH_1("Proceed with send APDU 0x%x command irrespective of card state",
                       cmd);
        return FALSE;
      }
      return TRUE;
  }
  UIM_MSG_ERR_1("In mmgsdi_is_cmd_slot_specific, unknown command: 0x%x", cmd);
  return FALSE;
} /* mmgsdi_is_cmd_slot_specific */


/*===========================================================================
FUNCTION MMGSDI_IS_SLOT_READY_FOR_CMD

DESCRIPTION
  This function determines whether the request can proceed to the UIM
  module or not.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean mmgsdi_is_slot_ready_for_cmd(
  const mmgsdi_task_cmd_data_type *cmd_data_ptr,
  mmgsdi_session_id_type           session_id,
  mmgsdi_slot_id_enum_type         slot,
  boolean                         *conditional_flag_ptr)
{
  mmgsdi_slot_data_type     *slot_data_ptr     = NULL;
  boolean                    proceed_to_card   = TRUE;
  mmgsdi_int_app_info_type  *app_info_ptr      = NULL;
  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_session_info_type  *session_info_ptr  = NULL;

  /* conditional_flag_ptr is used to indicate if certain types of access to
     EFs on the card are allowed when 'SIM init completed' is still pending.
  */
  if((cmd_data_ptr == NULL) || (conditional_flag_ptr == NULL))
  {
    UIM_MSG_ERR_2("Bad input param, cmd_data_ptr: 0x%x, conditional_flag_ptr: 0x%x",
                  cmd_data_ptr, conditional_flag_ptr);
    return FALSE;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

  if(slot_data_ptr == NULL)
  {
    proceed_to_card = FALSE;
  }
  else
  {
    if (slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED)
    {
      UIM_MSG_HIGH_1("In mmgsdi_is_slot_ready_for_cmd, mmgsdi_state: 0x%x",
                     slot_data_ptr->mmgsdi_state);
    }

    switch (slot_data_ptr->mmgsdi_state)
    {
      case MMGSDI_STATE_CARD_INSERTED:
        switch(cmd_data_ptr->cmd_enum)
        {
          case MMGSDI_READ_REQ:
          case MMGSDI_WRITE_REQ:
          case MMGSDI_GET_FILE_ATTR_REQ:
          case MMGSDI_SEARCH_REQ:
            mmgsdi_status = mmgsdi_util_get_session_app_info(
              session_id,
              NULL,
              NULL,
              &app_info_ptr,
              NULL,
              &session_info_ptr,
              NULL);
            if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL) ||
                 (session_info_ptr == NULL))
            {
              proceed_to_card = TRUE;
              break;
            }

      /* Blocking search request to process de-activation quickly triggered
               from external clients */
            if(app_info_ptr->deactivation_in_progress &&
               cmd_data_ptr->cmd_enum == MMGSDI_SEARCH_REQ)
            {
              UIM_MSG_HIGH_1("deactivation_in_progress, Blocking comand 0x%x", cmd_data_ptr->cmd_enum);
              return FALSE;
            }

            switch(app_info_ptr->app_state)
            {
              case MMGSDI_APP_STATE_UNKNOWN:
              case MMGSDI_APP_STATE_DETECTED:
              case MMGSDI_APP_STATE_ACTIVATED:
              case MMGSDI_APP_STATE_PIN_EVT_SENT:
              case MMGSDI_APP_STATE_READY_FOR_PERSO:
              case MMGSDI_APP_STATE_PERSO_VERIFIED:
              case MMGSDI_APP_STATE_WAIT_FOR_SUB_OK:
              case MMGSDI_APP_STATE_DEACTIVATED:
              case MMGSDI_APP_STATE_ENUM_MAX:
                /* Set the conditional flag for sessions to prov apps */
                if ((session_info_ptr != NULL) &&
                    (mmgsdi_util_is_prov_session(session_info_ptr->session_type)))
                {
                  *conditional_flag_ptr = TRUE;
                }
                proceed_to_card = TRUE;
                break;

              case MMGSDI_APP_STATE_READY:
              case MMGSDI_APP_STATE_EXT_PERSO_FAILED:
              case MMGSDI_APP_STATE_ILLEGAL:
                proceed_to_card = TRUE;
                break;
            }
            break;
/* Spec 11.11  Section: 8.16 RUN GSM ALGORITHM and
   31.102 5.1.1.2 USIM initialisation.
   AUTH requests need to be allowed in illegal or perso locked
   states so that NAS can pass AUTH commands from network. This
   ensures that UE can be authenticated even in illegal/locked state. */
          case MMGSDI_USIM_AUTH_REQ:
          case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
            mmgsdi_status = mmgsdi_util_get_session_app_info(
                              session_id,
                              NULL,
                              NULL,
                              &app_info_ptr,
                              NULL,
                              NULL,
                              NULL);

            if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
            {
              proceed_to_card = FALSE;
              break;
            }
            switch(app_info_ptr->app_state)
            {
              case MMGSDI_APP_STATE_READY:
                proceed_to_card = TRUE;
                break;
              case MMGSDI_APP_STATE_PERSO_VERIFIED:
              case MMGSDI_APP_STATE_READY_FOR_PERSO:
              case MMGSDI_APP_STATE_ILLEGAL:
              case MMGSDI_APP_STATE_WAIT_FOR_SUB_OK:
              case MMGSDI_APP_STATE_EXT_PERSO_FAILED:
                if ((cmd_data_ptr->cmd_enum == MMGSDI_USIM_AUTH_REQ &&
                     cmd_data_ptr->cmd.auth_req.illegal_or_perso_ok) ||
                    (cmd_data_ptr->cmd_enum == MMGSDI_SESSION_RUN_GSM_ALGO_REQ &&
                     cmd_data_ptr->cmd.session_run_gsm_algo_req.illegal_or_perso_ok))
                {
                  proceed_to_card = TRUE;
                }
                else
                {
                  UIM_MSG_HIGH_0("App not ready for authentication");
                  proceed_to_card = FALSE;
                }
                break;
              default:
                UIM_MSG_HIGH_0("USIM App initialization not done or Failure Authenticating the SIM to a GSM Network, hence return error");
                proceed_to_card = FALSE;
                break;
            }
            break;
          case MMGSDI_EONS_SS_EVENT_REQ:
            mmgsdi_status = mmgsdi_util_get_session_app_info(
                              session_id,
                              NULL,
                              NULL,
                              &app_info_ptr,
                              NULL,
                              NULL,
                              NULL);

            if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
            {
              proceed_to_card = FALSE;
              break;
            }

            if(app_info_ptr->app_state == MMGSDI_APP_STATE_READY ||
               app_info_ptr->app_state == MMGSDI_APP_STATE_ILLEGAL)
            {
              proceed_to_card = TRUE;
            }
            else
            {
              proceed_to_card = FALSE;
            }
            break;
          case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
            if((slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)  ||
               (slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_OK))
            {
              proceed_to_card = TRUE;
            }
            else
            {
              proceed_to_card = FALSE;
            }
            break;
          case MMGSDI_SRV_AVAILABLE_REQ:
            mmgsdi_status = mmgsdi_util_get_session_app_info(
                              session_id,
                              NULL,
                              NULL,
                              &app_info_ptr,
                              NULL,
                              NULL,
                              NULL);

            if((mmgsdi_status != MMGSDI_SUCCESS) || (app_info_ptr == NULL))
            {
              proceed_to_card = FALSE;
              break;
            }

            if(app_info_ptr->app_state == MMGSDI_APP_STATE_PERSO_VERIFIED ||
               app_info_ptr->app_state == MMGSDI_APP_STATE_READY ||
               app_info_ptr->app_state == MMGSDI_APP_STATE_ILLEGAL ||
               app_info_ptr->app_state == MMGSDI_APP_STATE_WAIT_FOR_SUB_OK)
            {
              proceed_to_card = TRUE;
            }
            else
            {
              proceed_to_card = FALSE;
            }
            break;
          default:
            proceed_to_card = TRUE;
            break;
        }
        break;
      case MMGSDI_STATE_NOT_INIT:
        /* When MMGSDI state is NOT_INIT make proceed to card as TRUE if
           1) When the request is TP_DOWNLOAD and TP_STATE is in progress
           2) When multi profile EFS is present and request is selection of
              AID or set sim profile */
        if((MMGSDI_TP_DOWNLOAD_COMPLETE_REQ == cmd_data_ptr->cmd_enum) &&
           ((slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_OK)  ||
            (slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)))
        {
          proceed_to_card = TRUE;
        }
#ifdef FEATURE_UIM_MULTI_PROFILE
        else if((mmgsdi_profile_info_ptr != NULL) &&
            ((MMGSDI_SESSION_SELECT_AID_REQ == cmd_data_ptr->cmd_enum) ||
                 (MMGSDI_SET_SIM_PROFILE_REQ == cmd_data_ptr->cmd_enum)))
        {
          proceed_to_card = TRUE;
        }
#endif /* FEATURE_UIM_MULTI_PROFILE */
        else
        {
          proceed_to_card = FALSE;
        }
        break;
      case MMGSDI_STATE_NO_CARD:
         /* Check if SAP connected, if so allow SAP commands to proceed */
         if((slot_data_ptr->card_error_condition == MMGSDI_CARD_ERR_SAP_CONNECTED)&&
            ((cmd_data_ptr->cmd_enum == MMGSDI_SAP_GET_ATR_REQ)||
             (cmd_data_ptr->cmd_enum == MMGSDI_SAP_SEND_APDU_REQ)||
             (cmd_data_ptr->cmd_enum == MMGSDI_SAP_DISCONNECT_REQ)||
             (cmd_data_ptr->cmd_enum == MMGSDI_SAP_POWER_ON_REQ)||
             (cmd_data_ptr->cmd_enum == MMGSDI_SAP_POWER_OFF_REQ)||
             (cmd_data_ptr->cmd_enum == MMGSDI_SAP_RESET_REQ)))
         {
           proceed_to_card = TRUE;
           break;
         }
         else
         {
           proceed_to_card = FALSE;
         }
         break;
      case MMGSDI_STATE_ENUM_MAX:
        proceed_to_card = FALSE;
        break;
    }
  }
  return proceed_to_card;
} /* mmgsdi_is_slot_ready_for_cmd */


/*===========================================================================
   FUNCTION:      MMGSDI_BUILD_AND_QUEUE_CLIENT_REG_RSP

   DESCRIPTION:
     This function will build and queue the client ID, Event registration
     and deregistration response

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_build_and_queue_client_reg_rsp(
  int32                               index,
  mmgsdi_return_enum_type             client_reg_status,
  const mmgsdi_request_header_type  * req_header_ptr
)
{
  mmgsdi_task_cmd_type       *task_cmd_ptr =  NULL;
  mmgsdi_request_header_type rsp_header    = {0};
  uint32                     task_cmd_len  =  0;
  mmgsdi_task_enum_type      mmgsdi_task   = MMGSDI_TASK_MAX;

  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    POPULATE THE RESPONSE MESSAGE PAYLOAD
    1. client_id         - Client ID Obtained
    2. request_type      - Request Command Type
    3. orig_request_type - original request type
    3. request_len       - Length of message + header
    4. payload_len       - Length of the message payload
    5. slot_id           - Slot that the request is to be performed on
    6. client_data       - Pointer to Client Data
    7. response_cb       - Response Callback Pointer
  ---------------------------------------------------------------------------*/
  rsp_header = task_cmd_ptr->cmd.cmd.client_id_reg_rsp.request_header;

  rsp_header.client_id                  = req_header_ptr->client_id;
  rsp_header.request_type               = req_header_ptr->request_type;
  rsp_header.orig_request_type          = req_header_ptr->orig_request_type;
  rsp_header.slot_id                    = req_header_ptr->slot_id;
  rsp_header.client_data                = req_header_ptr->client_data;
  rsp_header.response_cb                = req_header_ptr->response_cb;
  rsp_header.request_len                = uint32toint32(sizeof(mmgsdi_task_cmd_type));
  rsp_header.payload_len                = rsp_header.request_len -
                                            uint32toint32(sizeof(mmgsdi_request_header_type));

  task_cmd_ptr->cmd.cmd_enum = MMGSDI_CLIENT_ID_REG_RSP;
  task_cmd_ptr->cmd.cmd.client_id_reg_rsp.rsp_data_index = index;
  task_cmd_ptr->cmd.cmd.client_id_reg_rsp.status = client_reg_status;
  (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

  /* Put the message on the queue */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the current task is undetermined
       queue to main task */
    q_put(&mmgsdi_task_rsp_q[MMGSDI_TASK_MAIN], &task_cmd_ptr->link);

    /* Set the command queue signal for */
    /* receive task                     */
    (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_TASK_RSP_Q_SIG);
    return MMGSDI_SUCCESS;
  }
  q_put(&mmgsdi_task_rsp_q[mmgsdi_task], &task_cmd_ptr->link);

  /* Set the command queue signal for */
  /* receive task                     */
  (void) rex_set_sigs(rex_self(), MMGSDI_TASK_RSP_Q_SIG);
  return MMGSDI_SUCCESS;
}/* mmgsdi_build_and_queue_client_reg_rsp */


/*===========================================================================
FUNCTION MMGSDI_BUILD_AND_QUEUE_GENERIC_ERR_RSP

DESCRIPTION
  This function build the generic error response in case a command is not
  able to be sent to MMGSDI/UIM for further processing.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_build_and_queue_generic_err_rsp(
  mmgsdi_slot_id_enum_type slot,
  int32                    index,
  mmgsdi_cmd_enum_type     cmd,
  mmgsdi_return_enum_type  mmgsdi_error_status)
{
  mmgsdi_request_header_type request_header;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_SUCCESS;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_REQ_INFO))
  {
    UIM_MSG_ERR_1("Invalid index 0x%x", index);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    UIM_MSG_ERR_1("Index 0x%x not populated in MMGSDI client req table", index);
    return MMGSDI_ERROR;
  }

  switch (cmd)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
      /* flush client ID rsp */
      request_header.client_id         = mmgsdi_client_req_table_info_ptr[index]->client_id;
      request_header.request_type      = mmgsdi_client_req_table_info_ptr[index]->request_type;
      request_header.orig_request_type = mmgsdi_client_req_table_info_ptr[index]->orig_request_type;
      request_header.slot_id           = slot;
      request_header.client_data       = mmgsdi_client_req_table_info_ptr[index]->client_data;
      request_header.response_cb       = mmgsdi_client_req_table_info_ptr[index]->response_cb;

      mmgsdi_status = mmgsdi_build_and_queue_client_reg_rsp(index, MMGSDI_ERROR, &request_header);
      break;

    case MMGSDI_SAP_DISCONNECT_REQ:
      mmgsdi_status = mmgsdi_util_queue_mmgsdi_sap_report_rsp(slot, index,
                                                              MMGSDI_ERROR);
      break;

    case MMGSDI_UIM_REPORT_RSP:
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_CNF:
    case MMGSDI_MAX_CMD_ENUM:
      return MMGSDI_ERROR;

    default:
      /* clear the pending command first */
      mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(index,
                                                              cmd,
                                                              mmgsdi_error_status);
      break;
  }
  return mmgsdi_status;
} /* mmgsdi_build_and_queue_generic_err_rsp */


/*===========================================================================
FUNCTION MMGSDI_QUEUE_GENERIC_ERR_RSP_AND_INDEX

DESCRIPTION
  This function get a free index from the client reg table and
  based on the command type build and queue a generic error response
  message to MMGSDI.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_queue_generic_err_rsp_and_index(
  const mmgsdi_task_cmd_data_type      * task_cmd_data_ptr,
  mmgsdi_return_enum_type                mmgsdi_error_status)
{
  int32                               index           = 0;
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_client_req_extra_info_type * extra_param_ptr = NULL;

  if (task_cmd_data_ptr == NULL)
  {
    return mmgsdi_status;
  }

  /* Check if a generic error response needs to be queued for the command */
  switch (task_cmd_data_ptr->cmd_enum)
  {
    /* No response needs to be queued for these commands even if the
       command handling fails */
    case MMGSDI_NOTIFY_LINK_EST_REQ:
    case MMGSDI_NOTIFY_CARD_ERROR_REQ:
    case MMGSDI_NOTIFY_SIM_BUSY_REQ:
    case MMGSDI_INTERNAL_PUP_INIT_REQ:
    case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
      UIM_MSG_HIGH_2("No generic error response queued for command: 0x%x, status: 0x%x",
                     task_cmd_data_ptr->cmd_enum, mmgsdi_error_status);
      return MMGSDI_SUCCESS;

    default:
      break;
  }

  /*-------------------------------------------------------------------------
  build response message and queue onto mmgsdi response q
  mmgsdi_process_response is expecting to have a client request table
  index to free
  -------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /*-------------------------------------------------------------------------
    allocate and build extra param ptr
    -------------------------------------------------------------------------*/
    /* Evaluate if this need to be done or not */
    mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                      task_cmd_data_ptr->cmd_enum,
                      &task_cmd_data_ptr->cmd,
                      &extra_param_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Unable to populate extra param, status:  0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                      index,
                      &task_cmd_data_ptr->cmd.client_id_reg_req.request_header,
                      NULL,
                      extra_param_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Release the client table */
      mmgsdi_util_free_client_request_table_index(index);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(extra_param_ptr);
    }
    else
    {
      mmgsdi_status = mmgsdi_build_and_queue_generic_err_rsp(
           task_cmd_data_ptr->cmd.client_id_reg_req.request_header.slot_id,
           index,
           task_cmd_data_ptr->cmd_enum,
           mmgsdi_error_status);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Queue to MMGSDI UIM Report Rsp Unsuccessful, status: 0x%x",
                      mmgsdi_status);
        /* Release the client table */
        mmgsdi_util_free_client_request_table_index(index);
      }
    }
  }
  return mmgsdi_status;
} /*mmgsdi_queue_generic_err_rsp_and_index*/


/*===========================================================================
FUNCTION MMGSDI_PROCESS_REHABILITATE_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate rehabilitate library
  functions.

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
static mmgsdi_return_enum_type mmgsdi_process_rehabilitate_cmd (
  const mmgsdi_set_file_status_req_type *rehabilitate_req_ptr,
  mmgsdi_protocol_enum_type              protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(rehabilitate_req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  if(protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uicc_set_file_status(rehabilitate_req_ptr,
                                                MMGSDI_REHABILITATE_REQ);
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_set_file_status(rehabilitate_req_ptr,
                                               MMGSDI_REHABILITATE_REQ);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
} /* mmgsdi_process_rehabilitate_cmd */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_PIN_OP_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate PIN Op functions.

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
static mmgsdi_return_enum_type mmgsdi_process_pin_op_cmd (
  const mmgsdi_pin_operation_req_type *pin_op_req_ptr,
  mmgsdi_protocol_enum_type            protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(pin_op_req_ptr);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  /* If request is for a Hiddenkey Operation */
  if(pin_op_req_ptr->pin_id == MMGSDI_HIDDENKEY)
  {
    /* Hiddenkey is supported only on UICC */
    if(protocol == MMGSDI_UICC)
    {
      mmgsdi_status = mmgsdi_uicc_hiddenkey_operation(pin_op_req_ptr);
    } /* protocol is MMGSDI_UICC */
    else
    {
      UIM_MSG_ERR_0("Hiddenkey operation not supported for non-UICC protocols");
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
    } /* protocol is MMGSDI_ICC */
  }
  else
  {
    mmgsdi_status = mmgsdi_common_pin_operation(pin_op_req_ptr, protocol);
  }
  return mmgsdi_status;
} /* mmgsdi_process_pin_op_cmd */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_GET_ALL_PIN_STATUS_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate Get All Pin Status
  functions.

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
static mmgsdi_return_enum_type mmgsdi_process_get_all_pin_status_cmd (
  const mmgsdi_get_all_pin_status_req_type *get_all_pin_status_req_ptr,
  mmgsdi_protocol_enum_type                 protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(get_all_pin_status_req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  if(protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uicc_pin_status(get_all_pin_status_req_ptr);
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_pin_status(get_all_pin_status_req_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
} /* mmgsdi_process_get_all_pin_status_cmd */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_REHABILITATE_RSP

DESCRIPTION
  This function, called in mgmsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate rehabilitate library
  functions.

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
static mmgsdi_return_enum_type mmgsdi_process_rehabilitate_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr,
  mmgsdi_protocol_enum_type         protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;
  int32                      index              = 0;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  index = uim_rsp_ptr->rsp_data_index;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_REQ_INFO)  ||
      mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    UIM_MSG_ERR_1("Invalid index 0x%x", index);
    return MMGSDI_ERROR;
  }

  if(protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uicc_rsp_set_file_status(uim_rsp_ptr,
                                                    MMGSDI_REHABILITATE_REQ);
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_rsp_rehabitate(uim_rsp_ptr, NULL, FALSE);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
} /* mmgsdi_process_rehabilitate_rsp */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_INVALIDATE_CMD

DESCRIPTION
  This function, called in mgmsdi_process_command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate invalidate library
  functions.

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
static mmgsdi_return_enum_type mmgsdi_process_invalidate_cmd (
  const mmgsdi_set_file_status_req_type *invalidate_req_ptr,
  mmgsdi_protocol_enum_type              protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(invalidate_req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  if(protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uicc_set_file_status(invalidate_req_ptr,
                                                MMGSDI_INVALIDATE_REQ);
  } /* protocol is MMGSDI_UICC */
  else
  {
      mmgsdi_status = mmgsdi_icc_set_file_status(invalidate_req_ptr,
                                                 MMGSDI_INVALIDATE_REQ);
  } /* protocol is MMGSDI_ICC */

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("Failed to process invalidate cmd, Protocol: 0x%x, Slot: 0x%x",
                   protocol, invalidate_req_ptr->request_header.slot_id);
  }

  return mmgsdi_status;
} /* mmgsdi_process_invalidate_cmd */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_INVALIDATE_RSP

DESCRIPTION
  This function, called in mgmsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate invalidate library
  functions.

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
static mmgsdi_return_enum_type mmgsdi_process_invalidate_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr,
  mmgsdi_protocol_enum_type         protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;
  int32                      index              = 0;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  index = uim_rsp_ptr->rsp_data_index;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_REQ_INFO) ||
      protocol == MMGSDI_NO_PROTOCOL)
  {
    UIM_MSG_ERR_2("Invalid index 0x%x or protocol 0x%x", index, protocol);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    UIM_MSG_ERR_1("Index 0x%x not populated in MMGSDI client req table", index);
    return MMGSDI_ERROR;
  }

  if(protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uicc_rsp_set_file_status(uim_rsp_ptr,
                                                    MMGSDI_INVALIDATE_REQ);
  } /* protocol is MMGSDI_UICC */
  else
  {
    mmgsdi_status = mmgsdi_icc_rsp_invalidate(uim_rsp_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
} /* mmgsdi_process_invalidate_rsp */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_PIN_OP_RSP

DESCRIPTION
  This function, called in mmgsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate pin operation
  functions.

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
static mmgsdi_return_enum_type mmgsdi_process_pin_op_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr,
  mmgsdi_protocol_enum_type         protocol
)
{
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_SUCCESS;
  int32                               index           = 0;
  mmgsdi_request_header_type          req_header;
  uim_cmd_type                      * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type * extra_param_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  index = uim_rsp_ptr->rsp_data_index;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_REQ_INFO) ||
      protocol == MMGSDI_NO_PROTOCOL)
  {
    UIM_MSG_ERR_2("Invalid index 0x%x or protocol 0x%x", index, protocol);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    UIM_MSG_ERR_1("Index 0x%x not populated in MMGSDI client req table", index);
    return MMGSDI_ERROR;
  }

  /* Retrieve the PIN ID from the Client Table */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* extra_param should never be NULL here */
  if(extra_param_ptr == NULL)
  {
    UIM_MSG_ERR_0("extra_param_ptr NULL in Hiddenkey response");
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_3("Processing Response for MMGSDI_PIN_OPERATION_REQ, Protocol: 0x%x, Slot: 0x%x, PIN ID: 0x%x",
                 protocol,
                 mmgsdi_client_req_table_info_ptr[index]->slot_id,
                 extra_param_ptr->pin_data.pin_info.pin_id);

  /* If operation is for Hiddenkey */
  if(extra_param_ptr->pin_data.pin_info.pin_id == MMGSDI_HIDDENKEY)
  {
    return mmgsdi_uicc_rsp_hiddenkey_operation(uim_rsp_ptr);
  }

  if(protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uicc_rsp_pin_operation(uim_rsp_ptr);
  } /* protocol is MMGSDI_UICC */
  else
  {
      mmgsdi_status = mmgsdi_icc_rsp_pin_operation(uim_rsp_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


/*===========================================================================
FUNCTION MMGSDI_PROCESS_GET_ALL_PIN_STATUS_RSP

DESCRIPTION
  This function, called in mmgsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate get all pin status
  functions.

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
static mmgsdi_return_enum_type mmgsdi_process_get_all_pin_status_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr,
  mmgsdi_protocol_enum_type         protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;
  int32                      index              = 0;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  index = uim_rsp_ptr->rsp_data_index;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_REQ_INFO) ||
       protocol == MMGSDI_NO_PROTOCOL)
  {
    UIM_MSG_ERR_2("Invalid index 0x%x or protocol 0x%x", index, protocol);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    UIM_MSG_ERR_1("Index 0x%x not populated in MMGSDI client req table", index);
    return MMGSDI_ERROR;
  }

  if(protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uicc_rsp_pin_status(uim_rsp_ptr);
  } /* protocol is MMGSDI_UICC */
  else
  {
      mmgsdi_status = mmgsdi_icc_rsp_pin_status(uim_rsp_ptr);
  } /* protocol is MMGSDI_ICC */
  return mmgsdi_status;
}


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
boolean mmgsdi_present_in_preinit_file_access_list(
    const mmgsdi_task_cmd_data_type *cmd_ptr)
{
  mmgsdi_file_enum_type mmgsdi_filename = MMGSDI_MAX_FILE_ENUM;

  if(cmd_ptr == NULL)
  {
    return FALSE;
  }

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_ALLOW_ACCESS_BEFORE_APP_READY, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
     MMGSDI_FEATURE_ENABLED)
  {
    UIM_MSG_HIGH_0("Allowing access to file");
    return TRUE;
  }

  switch(cmd_ptr->cmd_enum)
  {
    case MMGSDI_READ_REQ:
      switch (cmd_ptr->cmd.read_req.access.access_method)
      {
        case MMGSDI_EF_ENUM_ACCESS:
          mmgsdi_filename = cmd_ptr->cmd.read_req.access.file.file_enum;
          break;
        case MMGSDI_DF_ENUM_ACCESS:
        case MMGSDI_BY_PATH_ACCESS:
        case MMGSDI_BY_APP_ID_ACCESS:
        case MMGSDI_MAX_ACCESS_METHOD_ENUM:
          UIM_MSG_HIGH_1("access_method currently not supported 0x%x",
                         cmd_ptr->cmd.read_req.access.access_method);
          break;
        default:
          break;
      }/* End of switch (cmd_ptr->cmd.read_req.access.access_method) */
      break;

    case MMGSDI_WRITE_REQ:
      switch (cmd_ptr->cmd.write_req.access.access_method)
      {
        case MMGSDI_EF_ENUM_ACCESS:
          mmgsdi_filename = cmd_ptr->cmd.write_req.access.file.file_enum;
          break;
        case MMGSDI_DF_ENUM_ACCESS:
        case MMGSDI_BY_PATH_ACCESS:
        case MMGSDI_BY_APP_ID_ACCESS:
        case MMGSDI_MAX_ACCESS_METHOD_ENUM:
          UIM_MSG_HIGH_1("access_method currently not supported 0x%x",
                         cmd_ptr->cmd.write_req.access.access_method);
          break;
        default:
          break;
      }/* switch (cmd_ptr->cmd.write_req.access.access_method) */
      break;

     case MMGSDI_GET_FILE_ATTR_REQ:
      switch (cmd_ptr->cmd.get_file_attr_req.access.access_method)
      {
        case MMGSDI_EF_ENUM_ACCESS:
          mmgsdi_filename = cmd_ptr->cmd.get_file_attr_req.access.file.file_enum;
          break;
        case MMGSDI_DF_ENUM_ACCESS:
        case MMGSDI_BY_PATH_ACCESS:
        case MMGSDI_BY_APP_ID_ACCESS:
        case MMGSDI_MAX_ACCESS_METHOD_ENUM:
          UIM_MSG_HIGH_1("access_method currently not supported 0x%x",
                         cmd_ptr->cmd.get_file_attr_req.access.access_method);
          break;
        default:
          break;
      }/* End of switch (cmd_ptr->cmd.get_file_attr_req.access.access_method) */
      break;

    case MMGSDI_SEARCH_REQ:
      switch (cmd_ptr->cmd.search_req.access.access_method)
      {
        case MMGSDI_EF_ENUM_ACCESS:
          mmgsdi_filename = cmd_ptr->cmd.search_req.access.file.file_enum;
          break;
        case MMGSDI_DF_ENUM_ACCESS:
        case MMGSDI_BY_PATH_ACCESS:
        case MMGSDI_BY_APP_ID_ACCESS:
        case MMGSDI_MAX_ACCESS_METHOD_ENUM:
          UIM_MSG_HIGH_1("access_method currently not supported 0x%x",
                         cmd_ptr->cmd.search_req.access.access_method);
          break;

        default:
          break;
      }/* End of switch (cmd_ptr->cmd.read_req.access.access_method) */
      break;

    default:
      break;
  }/* End of switch(cmd_ptr->cmd.cmd_enum) */

  switch(mmgsdi_filename)
  {
    /* We need to allow IMSI read when phone is in SimLock state,
     * but we let the command go thru only if IMSI is already cached.
     *  This should be the normal case, because IMSI was already read
     *  by perso engine.
     */
    case MMGSDI_GSM_IMSI:
    case MMGSDI_USIM_IMSI:
    case MMGSDI_CDMA_IMSI_M:
    case MMGSDI_CDMA_IMSI_T:
    case MMGSDI_CSIM_IMSI_M:
    case MMGSDI_CSIM_IMSI_T:
    {
      mmgsdi_return_enum_type       mmgsdi_status     = MMGSDI_SUCCESS;
      mmgsdi_int_app_info_type    * int_app_info_ptr  = NULL;

      if (cmd_ptr->cmd_enum == MMGSDI_READ_REQ)
      {
        mmgsdi_status = mmgsdi_util_get_prov_session_info(
                               cmd_ptr->cmd.read_req.request_header.session_id,
                               NULL,
                               NULL,
                               &int_app_info_ptr);
      }
      if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
      {
        /* Invalid session_id*/
        return FALSE;
      }

      if(int_app_info_ptr->perso_state == MMGSDI_SIMLOCK_UNLOCK_REQUIRED ||
         int_app_info_ptr->perso_state == MMGSDI_SIMLOCK_NOT_VERIFIED)
      {
        mmgsdi_access_type     file_access;
        mmgsdi_len_type        file_len = 0;

        memset(&file_access, 0x00, sizeof(mmgsdi_access_type));
        file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
        file_access.file.file_enum = mmgsdi_filename;

        mmgsdi_status = mmgsdi_cache_read_len(
            cmd_ptr->cmd.read_req.request_header.session_id,
            &file_access,
            &file_len,
            0,
            NULL);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          return TRUE;
        }
      }
      else if(int_app_info_ptr->perso_state == MMGSDI_SIMLOCK_VERIFIED)
      {
        return TRUE;
      }
      break;
    }
    case MMGSDI_DIR:
    case MMGSDI_ICCID:
    case MMGSDI_ELP:
    case MMGSDI_ARR:
    case MMGSDI_GSM_ECC:
    case MMGSDI_GSM_LP:
    case MMGSDI_GSM_AD:
    case MMGSDI_GSM_PHASE:
    case MMGSDI_GSM_SPN:
    case MMGSDI_GSM_PNN:
    case MMGSDI_GSM_OPL:
    case MMGSDI_GSM_KC:
    case MMGSDI_GSM_KCGPRS:
    case MMGSDI_GSM_LOCI:
    case MMGSDI_GSM_LOCIGPRS:
    case MMGSDI_GSM_ACC:
    case MMGSDI_GSM_SST:
    case MMGSDI_GSM_GID1:
    case MMGSDI_GSM_GID2:
    case MMGSDI_USIM_ECC:
    case MMGSDI_USIM_AD:
    case MMGSDI_USIM_ARR:
    case MMGSDI_USIM_LI:
    case MMGSDI_USIM_SPN:
    case MMGSDI_USIM_PNN:
    case MMGSDI_USIM_OPL:
    case MMGSDI_USIM_SPNI:
    case MMGSDI_USIM_PNNI:
    case MMGSDI_USIM_EPSNSC:
    case MMGSDI_USIM_EPSLOCI:
    case MMGSDI_USIM_START_HFN:
    case MMGSDI_USIM_THRESHOLD:
    case MMGSDI_USIM_KC:
    case MMGSDI_USIM_KCGPRS:
    case MMGSDI_USIM_KEYS:
    case MMGSDI_USIM_KEYSPS:
    case MMGSDI_USIM_LOCI:
    case MMGSDI_USIM_PSLOCI:
    case MMGSDI_USIM_ACC:
    case MMGSDI_USIM_UST:
    case MMGSDI_USIM_GID1:
    case MMGSDI_USIM_GID2:
    /* make MDN related EF read before SIM Lock has been validated */
    case MMGSDI_USIM_MSISDN:
    case MMGSDI_CDMA_ECC:
    case MMGSDI_CDMA_PREF_LANG:
    case MMGSDI_CDMA_RUIM_ID:
    case MMGSDI_CDMA_ESN_ME:
    case MMGSDI_CDMA_RUIM_PHASE:
    case MMGSDI_CDMA_HOME_SVC_PVDR_NAME:
    case MMGSDI_CDMA_ADM_DATA:
    case MMGSDI_CDMA_SF_EUIM_ID:
    case MMGSDI_CDMA_GID1:
    case MMGSDI_CDMA_GID2:
    case MMGSDI_CSIM_ECC:
    case MMGSDI_CSIM_LANG_IND:
    case MMGSDI_CSIM_RUIM_ID:
    case MMGSDI_CSIM_ESN_ME:
    case MMGSDI_CSIM_SPN:
    case MMGSDI_CSIM_AD:
    case MMGSDI_CSIM_SF_EUIM_ID:
    case MMGSDI_CSIM_ROOT_CERT:
    case MMGSDI_CSIM_ARR:
    case MMGSDI_CSIM_GID1:
    case MMGSDI_CSIM_GID2:
    /* make MDN related EF read before SIM Lock has been validated */
    case MMGSDI_CSIM_MSISDN:
    case MMGSDI_TELECOM_ARR:
    case MMGSDI_TELECOM_ICE_DN:
    case MMGSDI_TELECOM_ICE_FF:
    case MMGSDI_TELECOM_ICE_GRAPHICS:
    /* make MDN related EF read before SIM Lock has been validated */
    case MMGSDI_TELECOM_MSISDN:

      UIM_MSG_HIGH_1("Return TRUE for 0x%x", mmgsdi_filename);
      return TRUE;
    default:
      break;
  }
  return FALSE;
}/* mmgsdi_present_in_preinit_file_access_list */


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_GET_APP_CAPABILITIES_REQ

   DESCRIPTION:
     This function handles get app capabilities request and extracts the
     corresponding API based on the APP type.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_SUCCESS: Successful handling of the request
     MMGSDI_ERROR  : Invalid request parameters

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_get_app_capabilities_req(
  mmgsdi_session_get_app_capabilities_req_type * req_ptr
)
{
  mmgsdi_int_app_info_type  *app_info_ptr = NULL;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_get_prov_session_info(
                            req_ptr->request_header.session_id,
                            NULL,NULL,&app_info_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("Invalid session info");
    return MMGSDI_ERROR;
  }

  switch (app_info_ptr->app_data.app_type)
  {
    case MMGSDI_APP_SIM:
      mmgsdi_status = mmgsdi_icc_get_sim_capabilities(req_ptr->request_header.session_id,
                                                      req_ptr->request_header.slot_id);
      break;

    case MMGSDI_APP_RUIM:
      mmgsdi_status = mmgsdi_icc_get_ruim_capabilities(req_ptr->request_header.session_id,
                                                      req_ptr->request_header.slot_id);
      break;

    case MMGSDI_APP_USIM:
      mmgsdi_status = mmgsdi_uicc_get_capabilities(req_ptr->request_header.session_id,
                                                   req_ptr->request_header.slot_id,
                                                   MMGSDI_APP_USIM);
      break;

    case MMGSDI_APP_CSIM:
      mmgsdi_status = mmgsdi_uicc_get_capabilities(req_ptr->request_header.session_id,
                                                   req_ptr->request_header.slot_id,
                                                   MMGSDI_APP_CSIM);
      break;

    default:
      UIM_MSG_ERR_0("Invalid session info");
      return MMGSDI_ERROR;
  }

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi_process_get_app_capabilities_req failed, status: 0x%x",
                  mmgsdi_status);
    return mmgsdi_status;
  }

  return mmgsdi_gen_get_app_capabilities(req_ptr);
} /* mmgsdi_process_get_app_capabilities_req */


/*===========================================================================
FUNCTION MMGSDI_STATE_SUB_OK_PERSO_UPDATE

DESCRIPTION
  Function to update the state or global info as needed when external perso
  sub_ok is received

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void mmgsdi_state_sub_ok_perso_update(
  mmgsdi_session_id_type        session_id,
  mmgsdi_perso_status_enum_type perso_status)
{
  mmgsdi_int_app_info_type       *int_app_info_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status          = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type        slot_id                = MMGSDI_SLOT_AUTOMATIC;
  mmgsdi_evt_session_notify_type  notify_type;

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    &int_app_info_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    /* Invalid session_id*/
    return;
  }

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = slot_id;

  /* Only if the app state is WAIT_FOR_SUB_OK, trigger the state transition */
  if(int_app_info_ptr->app_state != MMGSDI_APP_STATE_WAIT_FOR_SUB_OK)
  {
    return;
  }

  /* Update to reflect the new state if we were in the WAIT_FOR_SUB_OK state
     before */
  if(perso_status == MMGSDI_PERSO_STATUS_GENERIC_FAILURE)
  {
    mmgsdi_state_update_app_state_for_prov_app(session_id,
                                               MMGSDI_APP_STATE_WAIT_FOR_SUB_OK,
                                               MMGSDI_APP_STATE_EXT_PERSO_FAILED,
                                               NULL);
  }
  /* if external perso succeeded */
  else if(perso_status == MMGSDI_PERSO_STATUS_DONE)
  {
    /* Move the app state to PERSO_VERIFIED */
    mmgsdi_state_update_app_state_for_prov_app(session_id,
                                               MMGSDI_APP_STATE_WAIT_FOR_SUB_OK,
                                               MMGSDI_APP_STATE_PERSO_VERIFIED,
                                               NULL);

    if(mmgsdi_nv_get_feature_status(
       MMGSDI_FEATURE_HALT_3GPP2_SUBS_IF_3GPP_PERSO_FAILS, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
         MMGSDI_FEATURE_ENABLED)
    {
      switch(int_app_info_ptr->app_data.app_type)
      {
        case MMGSDI_APP_CSIM:
        case MMGSDI_APP_RUIM:
          /* Find out if the 1x app post perso can be performed */
          if(mmgsdi_util_check_perso_verified(slot_id, MMGSDI_TECH_3GPP) == FALSE)
          {
            /* Do not perform CSIM/RUIM post perso update. Once perso check
               for USIM succeeds, CSIM/RUIM post perso update will also be
               triggered */
            UIM_MSG_HIGH_0("Defer updating CSIM/RUIM perso state until USIM/SIM is perso verified");
            /* Build and send the perso event with DONE status */
            mmgsdi_evt_build_and_send_perso(
              notify_type,
              (const mmgsdi_int_app_info_type*)int_app_info_ptr,
              perso_status,
              MMGSDI_MAX_PERSO_FEATURE_ENUM,
              0,
              0);
            return;
          }
          /* Continue with CSIM/RUIM perso state update */
          UIM_MSG_HIGH_1("Continue with perso update for app 0x%x",
                         int_app_info_ptr->app_data.app_type);
          break;

        case MMGSDI_APP_USIM:
        case MMGSDI_APP_SIM:
          /* Find out if there are any CSIM/RUIM apps in PERSO_VERIFIED
             that are awaiting to perform post perso operations upon
             successful perso check of USIM/GSM */
          mmgsdi_util_unblock_apps_for_post_perso(slot_id, MMGSDI_TECH_3GPP2);

          /* Continue with USIM/SIM post perso procedures */
          break;

        default:
          UIM_MSG_HIGH_1("Invalid app type 0x%x",
                         int_app_info_ptr->app_data.app_type);
          break;
      } /* end switch */
    }

    if(mmgsdi_nv_get_feature_status(
       MMGSDI_FEATURE_HALT_3GPP_SUBS_IF_3GPP2_PERSO_FAILS, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC) ==
         MMGSDI_FEATURE_ENABLED)
    {
      /* Halt 3GPP subs until 3GPP2 perso is verified on the slot-1. But, for
         the other slots, only if the below features are disabled */
      if((slot_id == MMGSDI_SLOT_1) ||
         ((mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_NO_PERSONALIZATION_ON_ADDITIONAL_SLOTS, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
                      == MMGSDI_FEATURE_DISABLED) &&
          (mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_PERSONALIZATION_SLOTS_LOCKED_ON_SLOT1, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
                      == MMGSDI_FEATURE_DISABLED)))
      {
        switch(int_app_info_ptr->app_data.app_type)
        {
          case MMGSDI_APP_CSIM:
          case MMGSDI_APP_RUIM:
            /* Find out if there are any USIM/SIM apps in PERSO_VERIFIED
               that are waiting to perform post perso operations upon
               successful perso check of CSIM/RUIM */
            mmgsdi_util_unblock_apps_for_post_perso(slot_id, MMGSDI_TECH_3GPP);

            /* Continue with CSIM/RUIM perso state update */
            UIM_MSG_HIGH_1("Continue with perso update for app 0x%x",
                           int_app_info_ptr->app_data.app_type);
            break;

          case MMGSDI_APP_USIM:
          case MMGSDI_APP_SIM:
            /* Find out if the gw app post perso can be performed */
            if(mmgsdi_util_check_perso_verified(slot_id, MMGSDI_TECH_3GPP2) == FALSE)
            {
              /* Do not perform USIM/SIM post perso update. Once perso check
                 for CSIM/RUIM succeeds, USIM/SIM post perso update will also be
                 triggered */
              UIM_MSG_HIGH_0("Defer updating USIM/SIM perso state until CSIM/RUIM is perso verified");
              /* Build and send the perso event with DONE status */
              mmgsdi_evt_build_and_send_perso(
                notify_type,
                (const mmgsdi_int_app_info_type*)int_app_info_ptr,
                perso_status,
                MMGSDI_MAX_PERSO_FEATURE_ENUM,
                0,
                0);
              return;
            }
            /* Continue with USIM/SIM perso state update */
            UIM_MSG_HIGH_1("Continue with perso update for app 0x%x",
                           int_app_info_ptr->app_data.app_type);
            break;

          default:
            UIM_MSG_HIGH_1("Invalid app type 0x%x",
                           int_app_info_ptr->app_data.app_type);
            break;
        } /* end switch */
      }
    }

    (void)mmgsdi_card_init_build_internal_pup_req(session_id,
                                                  slot_id,
                                                  MMGSDI_PIN1_EVT,
                                                  MMGSDI_STATE_CARD_INSERTED,
                                                  MMGSDI_APP_STATE_PERSO_VERIFIED);
  }
  else
  {
    return;
  }

  /* Build and send the perso event with DONE status */
  mmgsdi_evt_build_and_send_perso(
    notify_type,
    (const mmgsdi_int_app_info_type*)int_app_info_ptr,
    perso_status,
    MMGSDI_MAX_PERSO_FEATURE_ENUM,
    0,
    0);
} /* mmgsdi_state_sub_ok_perso_update */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_SUB_OK_REQ

DESCRIPTION
  This function, called in mmgsdi_process_command.
  It will determine if a subscription that is initialized successfully will
  get published to the rest of the modem.

DEPENDENCIES
  None

LIMITATIONS
  This command is only supported on provisioning subscriptions

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_NOT_SUPPORTED    - Subscription state doesn't support this command
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_sub_ok_req (
  const mmgsdi_session_sub_ok_req_type *sub_ok_req_ptr)
{
  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_return_enum_type    req_status        = MMGSDI_ERROR;
  mmgsdi_int_app_info_type  *int_app_info_ptr  = NULL;
  mmgsdi_slot_id_enum_type   slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                      index             = 0;

  req_status = mmgsdi_util_get_prov_session_info(
                 sub_ok_req_ptr->request_header.session_id,
                 NULL, &slot_id, &int_app_info_ptr);

  if ((req_status == MMGSDI_SUCCESS) && (int_app_info_ptr != NULL))
  {
    UIM_MSG_HIGH_1("Identified prov app for SUB OK request.App type=0x%x",
                   int_app_info_ptr->app_data.app_type);
    if (int_app_info_ptr->app_state == MMGSDI_APP_STATE_WAIT_FOR_SUB_OK)
    {
      mmgsdi_perso_status_enum_type  perso_status      = MMGSDI_PERSO_STATUS_NONE;
      mmgsdi_session_id_type         mmgsdi_session_id = 0;

      /* Map input Session ID to MMGSDI Session ID. MMGSDI Session ID needs to
         be used in MMGSDI power-up sequence to ensure any ongoing REFRESH which
         triggered the power-up sequence is correctly completed */
      mmgsdi_status = mmgsdi_util_map_to_mmgsdi_session(
                        sub_ok_req_ptr->request_header.session_id,
                        &mmgsdi_session_id);

      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        if (sub_ok_req_ptr->ok_for_sub == TRUE)
        {
          UIM_MSG_HIGH_0("In mmgsdi_process_sub_ok_req, perso done");
          perso_status = MMGSDI_PERSO_STATUS_DONE;
        }
        else
        {
          UIM_MSG_HIGH_0("In mmgsdi_process_sub_ok_req, perso failed");
          perso_status = MMGSDI_PERSO_STATUS_GENERIC_FAILURE;
        }

        mmgsdi_state_sub_ok_perso_update(mmgsdi_session_id,
                                         perso_status);
      }
      else
      {
        UIM_MSG_ERR_1("Received invalid Session ID: 0x%x in SUB OK req",
                      sub_ok_req_ptr->request_header.session_id);
        req_status = MMGSDI_INCORRECT_PARAMS;
      }
    }
    else
    {
      UIM_MSG_ERR_1("Received SUB OK req in unsupported state=0x%x",
                    int_app_info_ptr->app_state);
      req_status = MMGSDI_NOT_SUPPORTED;
    }
  }
  else
  {
    req_status = MMGSDI_ERROR;
    if (int_app_info_ptr != NULL)
    {
      UIM_MSG_ERR_2("SUB OK req with session id=0x%x,app type=0x%x failed",
                  sub_ok_req_ptr->request_header.session_id,
                  int_app_info_ptr->app_data.app_type);
    }
    else
    {
      UIM_MSG_ERR_1("SUB OK req with session id=0x%x failed",
                    sub_ok_req_ptr->request_header.session_id);
    }
  }

  /* Get an index from the table used to store client information */
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("SUB OK req:Failed to get free client index,status=0x%x",
                  mmgsdi_status);
    return mmgsdi_status;
  }

  /* Store client information for later use when the response is processed */
  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                                              index,
                                              &sub_ok_req_ptr->request_header,
                                              NULL,
                                              NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("SUB OK req: Failed to populate client info,status=0x%x",
                  mmgsdi_status);
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  /* Send out the response */
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, sub_ok_req_ptr->request_header.request_type,
                    req_status);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_process_sub_ok_req */


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
  mmgsdi_return_enum_type  status)
{
  mmgsdi_return_enum_type                mmgsdi_status        = MMGSDI_ERROR;
  uim_cmd_type                         * uim_cmd_ptr          = NULL;
  mmgsdi_client_req_extra_info_type    * extra_param_ptr      = NULL;
  mmgsdi_task_cmd_type                 * mmgsdi_task_buf_ptr  = NULL;
  int32                                  mem_size             = 0;
  uint32                                 task_cmd_len         = 0;
  mmgsdi_task_enum_type                  mmgsdi_task          = MMGSDI_TASK_MAX;
  mmgsdi_request_header_type             req_header;

  UIM_MSG_ERR_1("Building Cnf from Static Task Cmd Buf 0x%x", status);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  memset(&req_header, 0x00, sizeof(mmgsdi_request_header_type));

  mmgsdi_util_cal_align_size(sizeof(mmgsdi_task_cmd_type),
                               &mem_size);

  task_cmd_len = sizeof(mmgsdi_callback_cnf_type) + int32touint32(mem_size);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_task_buf_ptr, task_cmd_len);

  if(mmgsdi_task_buf_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(
                                index,
                                &req_header,
                                &uim_cmd_ptr,
                                &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_task_buf_ptr);
    return mmgsdi_status;
  }

  mmgsdi_util_align_mem(&mmgsdi_task_buf_ptr->cmd,
                         MMGSDI_CNF);

  if (mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_task_buf_ptr);
    return MMGSDI_ERROR;
  }

  memset(mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr, 0x00, sizeof(mmgsdi_callback_cnf_type));

  mmgsdi_task_buf_ptr->cmd.cmd_enum = MMGSDI_CNF;

  mmgsdi_cnf_map_req_to_cnf_enum_type(
    req_header.orig_request_type,
    &mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.response_type);

  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->client_req_cb                            = req_header.response_cb;
  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.client_data   = req_header.client_data;
  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.client_id     = req_header.client_id;
  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.session_id    = req_header.session_id;
  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.slot_id       = req_header.slot_id;
  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.mmgsdi_status = status;

  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.status_word.present = FALSE;
  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.status_word.sw1     = 0x00;
  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.status_word.sw2     = 0x00;

  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.write_cnf.response_header.response_len =
                     uint32toint32(sizeof(mmgsdi_response_header_type));

  if(req_header.orig_request_type == MMGSDI_PIN_OPERATION_REQ)
  {
    if(extra_param_ptr != NULL)
    {
      mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.pin_operation_cnf.pin_op =
              extra_param_ptr->pin_data.orig_pin_op;
    }
  }
  else if(req_header.orig_request_type == MMGSDI_READ_REQ)
  {
    if(extra_param_ptr != NULL)
    {
      mmgsdi_memscpy(&mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.read_cnf.access,
                     sizeof(mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.read_cnf.access),
                     &extra_param_ptr->read_data.access,
                     sizeof(mmgsdi_access_type));
      mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.read_cnf.accessed_rec_num =
        extra_param_ptr->read_data.rec_num;
      mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.read_cnf.accessed_offset =
        extra_param_ptr->read_data.offset;
    }
  }

  mmgsdi_task_buf_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.payload_len = 0;
  (void)q_link(mmgsdi_task_buf_ptr, &mmgsdi_task_buf_ptr->link);

  /* Put the message on the queue */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case current task is not determined,
       queue to main task */
    q_put(&mmgsdi_task_rsp_q[MMGSDI_TASK_MAIN], &mmgsdi_task_buf_ptr->link);

    /* Set the command queue signal for */
    /* receive task                     */
    (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_TASK_RSP_Q_SIG);
    return MMGSDI_SUCCESS;
  }
  q_put(&mmgsdi_task_rsp_q[mmgsdi_task], &mmgsdi_task_buf_ptr->link);

  /* Set the command queue signal for */
  /* receive task                     */
  (void) rex_set_sigs(rex_self(), MMGSDI_TASK_RSP_Q_SIG);
  return MMGSDI_SUCCESS;
} /* mmgsdi_build_and_queue_static_cnf */


/*===========================================================================
FUNCTION MMGSDI_REMOVE_REQUESTS_IN_QUEUE_SEND_CNF

DESCRIPTION
  This function is used to remove the requests from the passed queue pointer.
  The check for determining if the command needs to be removed or not is based
  on the incoming command enum type:
  If command is cancel req - all requests with the client id is removed,
  If command is session cancel - all requests with session id is removed.
  The command's callback is invoked in such cases & the command is then deleted
  from the queue.

DEPENDENCIES
 None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_remove_requests_in_queue_send_cnf
(
  q_type                    * queue_ptr,
  mmgsdi_cancel_info_type     cancel_info
)
{
  q_link_type           link;
  mmgsdi_task_cmd_type *command_ptr      = NULL;
  mmgsdi_task_cmd_type *next_command_ptr = NULL;
  mmgsdi_task_enum_type mmgsdi_task      = MMGSDI_TASK_MAX;

  MMGSDIUTIL_RETURN_IF_NULL(queue_ptr);

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if (mmgsdi_task > MMGSDI_TASK_MAX)
  {
    return MMGSDI_ERROR;
  }

  /* Loop through links in the command queue and check if the
     request needs to be removed */
  command_ptr = (mmgsdi_task_cmd_type*)q_check(queue_ptr);
  while (command_ptr != NULL)
  {
    boolean                     remove_from_queue     = FALSE;
    mmgsdi_return_enum_type     mmgsdi_status         = MMGSDI_SUCCESS;
    mmgsdi_client_id_type       current_cmd_client_id = 0;
    mmgsdi_task_state_enum_type task_state            = MMGSDI_TASK_INACTIVE;

    link = command_ptr->link;

    /* Check if this is the client we are interested in */
    switch (cancel_info.cancel_type)
    {
      case MMGSDI_CANCEL_USING_CLIENT_ID:
        /* For session based commands, the client_id may not have been saved to the
           command_ptr. Therefore when cancelling by client_id we check the cmd's
           client ID for a match, and also convert the cmd's session ID to a client
           ID to check it too. */

        if (MMGSDI_SUCCESS == mmgsdi_util_get_client_id(
                                command_ptr->cmd.cmd.read_req.request_header.session_id,
                                &current_cmd_client_id))
        {
          /* Current cmd's session ID was successfully converted to a valid
             client ID, check if this command should be removed */
          if (cancel_info.id.client_id == current_cmd_client_id)
          {
            remove_from_queue = TRUE;
          }
        }

        if (cancel_info.id.client_id ==
              command_ptr->cmd.cmd.read_req.request_header.client_id)
        {
          remove_from_queue = TRUE;
        }
        break;
      case MMGSDI_CANCEL_USING_SESSION_ID:
        if (cancel_info.id.session_id ==
              command_ptr->cmd.cmd.read_req.request_header.session_id)
        {
          remove_from_queue = TRUE;
        }
        break;
      default:
        /* Should not happen */
        return MMGSDI_ERROR;
    }

    /* get pointer to the next command now, as this command may removed from the queue
       and the memory, including the link ptrs, freed */
    next_command_ptr = (mmgsdi_task_cmd_type*)q_next(queue_ptr, &link);

    /* If required, pop this from queue & send error response */
    if (remove_from_queue)
    {
      /* Queue error CNF before popping cmd from queue as follows */
      if (mmgsdi_task < MMGSDI_NUM_TASKS)
      {
        MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
        task_state = mmgsdi_task_state[mmgsdi_task];
        MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;
      }
      else
      {
        /* If we are not in MMGSDI task, simulate as if we are
           so that error CNF is correctly queued */
        task_state = MMGSDI_TASK_ACTIVE;
      }

      if(task_state == MMGSDI_TASK_ACTIVE)
      {
        /* Return error to the client for the particular request */
        mmgsdi_status = mmgsdi_queue_generic_err_rsp_and_index(&command_ptr->cmd,
                                                               MMGSDI_SIM_BUSY);
        UIM_MSG_HIGH_2("Error response queuing status 0x%x for not-responded-to cmd 0x%x",
                       mmgsdi_status, command_ptr->cmd.cmd_enum);

        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          return MMGSDI_ERROR;
        }
      }

      /* Delete command from queue */
      if (q_delete_ext(queue_ptr, &command_ptr->link))
      {
        MMGSDIUTIL_TMC_MEM_FREE(command_ptr);
      }
      else
      {
        /* If we fail to delete from queue, ignore & try with the next one */
        UIM_MSG_ERR_1("Cannot delete from queue, command_ptr: 0x%x",
                      command_ptr);
      }
    }

    /* Move to the next link in the queue */
    command_ptr = next_command_ptr;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_remove_requests_in_queue_send_cnf */


/*===========================================================================
FUNCTION MMGSDI_REMOVE_REQUESTS_IN_PROGRESS_SEND_CNF

DESCRIPTION
  This function is used to remove the in-progress requests from the global
  client request table info.The check for determining if the command needs
  to be removed or not is based on the incoming command enum type:
  If command is cancel req - all requests with the client id is removed,
  If command is session cancel - all requests with session id is removed.
  The in-progess requests are removed in 2 steps:
  1. The matching in-progress requests are marked for no response & an error
     response is sent now
  2. When the response from UIMDRV comes back, the corresponing entry is
     removed from the client request table

DEPENDENCIES
 None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_remove_requests_in_progress_send_cnf
(
  mmgsdi_cancel_info_type     cancel_info
)
{
  uint16                   index_task            = 0;
  uint16                   index_client_req      = 0;
  boolean                  remove_req            = FALSE;
  boolean                  rsp_found             = FALSE;
  mmgsdi_return_enum_type  mmgsdi_status         = MMGSDI_ERROR;
  mmgsdi_client_id_type    current_cmd_client_id = 0;

  for(index_client_req = 0; index_client_req < MMGSDI_MAX_CLIENT_REQ_INFO; index_client_req++)
  {
    remove_req = FALSE;
    rsp_found  = FALSE;

    /* If rsp_status is already MMGSDI_UIM_RSP_NOT_RECEIVED, it means an error
       cnf is already sent for this. If rsp_status is MMGSDI_UIM_RSP_PROCESSING
       it means this is ongoing response handling. MMGSDI RSP handling will
       send the cnf */
    if (mmgsdi_client_req_table_info_ptr[index_client_req] == NULL ||
        mmgsdi_client_req_table_info_ptr[index_client_req]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED ||
        mmgsdi_client_req_table_info_ptr[index_client_req]->uim_rsp_status == MMGSDI_UIM_RSP_PROCESSING)
    {
      continue;
    }

    /* Check if this is the client we are interested in */
    switch (cancel_info.cancel_type)
    {
      case MMGSDI_CANCEL_USING_CLIENT_ID:
        /* For session based commands, the client_id may not have been saved to
           the client_req_table. Therefore when cancelling by client_id we check
           the client_req_table client ID for a match, and also convert the
           current index's session ID to a client ID to check it too. */
        if (MMGSDI_SUCCESS == mmgsdi_util_get_client_id(
                                mmgsdi_client_req_table_info_ptr[index_client_req]->session_id,
                                &current_cmd_client_id))
        {
          /* Current client_table_req's session ID was successfully converted to a valid
             client ID, check if this command should be removed */
          if (cancel_info.id.client_id == current_cmd_client_id)
          {
            remove_req = TRUE;
          }
        }

        if (cancel_info.id.client_id ==
              mmgsdi_client_req_table_info_ptr[index_client_req]->client_id)
        {
          remove_req = TRUE;
        }
        break;
      case MMGSDI_CANCEL_USING_SESSION_ID:
        if (cancel_info.id.session_id ==
              mmgsdi_client_req_table_info_ptr[index_client_req]->session_id)
        {
          remove_req = TRUE;
        }
        break;
      default:
        /* Should not happen */
        return MMGSDI_ERROR;
    }

    if (!remove_req)
    {
      continue;
    }

    MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

    /* Loop through each task's response queue to check the index */
    for (index_task = 0; (index_task < MMGSDI_NUM_TASKS) && (!rsp_found); index_task++)
    {
      mmgsdi_task_cmd_type  * rsp_q_ptr = NULL;

      if (mmgsdi_task_state[index_task] == MMGSDI_TASK_INACTIVE)
      {
        continue;
      }
      rsp_q_ptr = (mmgsdi_task_cmd_type *)q_check(&mmgsdi_task_rsp_q[index_task]);
      while(rsp_q_ptr != NULL)
      {
        if(rsp_q_ptr->cmd.cmd_enum == MMGSDI_UIM_REPORT_RSP &&
           rsp_q_ptr->cmd.cmd.uim_report_rsp.rsp_data_index == index_client_req)
        {
          /* Response is already queued by UIM. Nothing to do here. */
          mmgsdi_client_req_table_info_ptr[index_client_req]->uim_rsp_status = MMGSDI_UIM_RSP_INBOUND;
          rsp_found = TRUE;
          break;
        }
        rsp_q_ptr = (mmgsdi_task_cmd_type *)q_next(&mmgsdi_task_rsp_q[index_task], &rsp_q_ptr->link);
      }
    } /* for loop index_task */

    MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

    /* If response is already queued by UIM, move to the next request in list */
    if (rsp_found)
    {
      continue;
    }

    /* If no response was found in response queues for this particular request,
       send an error response to client. Note that setting makes sure that client
       request table is cleaned when the response eventually comes from UIMDRV   */
    mmgsdi_client_req_table_info_ptr[index_client_req]->uim_rsp_status = MMGSDI_UIM_RSP_NOT_RECEIVED;

    mmgsdi_status = mmgsdi_build_and_queue_static_cnf(index_client_req, MMGSDI_ERROR);
    UIM_MSG_HIGH_2("Error response queuing status 0x%x for not-responded-to cmd 0x%x",
                   mmgsdi_status,
                   mmgsdi_client_req_table_info_ptr[index_client_req]->request_type);
  } /* for loop index_client_req */

  return MMGSDI_SUCCESS;
} /* mmgsdi_remove_requests_in_progress_send_cnf */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_CANCEL_REQ

DESCRIPTION
  This function is used to purge any pending requests that the client wants
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
)
{
  uint8                   index_task    = 0;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  /* Loop through each of the tasks & check for requests in the queue */
  for (index_task = 0; index_task < MMGSDI_NUM_TASKS; index_task++)
  {
    MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
    if (mmgsdi_task_state[index_task] == MMGSDI_TASK_INACTIVE)
    {
      MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;
      continue;
    }
    MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

    /* Check High priority queue first */
    mmgsdi_status = mmgsdi_remove_requests_in_queue_send_cnf(
                      &mmgsdi_high_pri_cmd_q[index_task],
                      cancel_info);

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Next check regular queue */
      mmgsdi_status = mmgsdi_remove_requests_in_queue_send_cnf(
                        &mmgsdi_task_cmd_q[index_task],
                        cancel_info);
    }

    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* Also mark the command in progress with UIMDRV so that error response can
     be sent now, but client req table entry is cleaned upon UIMDRV's response */
  mmgsdi_status = mmgsdi_remove_requests_in_progress_send_cnf(
                    cancel_info);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return MMGSDI_SUCCESS;
}/* mmgsdi_process_cancel_req */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_COMMAND

DESCRIPTION
  This function, called in gsdimain, is called to process an mmgsdi command.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate library function.

DEPENDENCIES
  gsdimain must be finished initialization.

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
static mmgsdi_return_enum_type mmgsdi_process_command(
  mmgsdi_task_cmd_type           * task_cmd_ptr
)
{
  /*-----------------------------------------------------------------------------
  Initialize variables
  -----------------------------------------------------------------------------*/
  mmgsdi_return_enum_type             mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_protocol_enum_type           protocol           = MMGSDI_NO_PROTOCOL;
  boolean                             proceed_processing = TRUE;
  boolean                             cond_processing    = FALSE;
  mmgsdi_slot_id_enum_type            slot_id            = MMGSDI_SLOT_AUTOMATIC;
  mmgsdi_task_enum_type               mmgsdi_task        = MMGSDI_TASK_MAX;
  mmgsdi_int_app_info_type          * app_info_ptr       = NULL;

  /*----------------------------------------------------------------------------
  Validate the parameters provided
  ----------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  /* Check if the command in main task needs to be queued to slot specific
     task */
  if(MMGSDI_TASK_MAIN == mmgsdi_util_get_current_task())
  {
    mmgsdi_status = mmgsdi_util_get_slot_for_cmd(task_cmd_ptr, &slot_id);

    /* If valid slot ID cannot be determined, return an error CNF
       to the client */
    if(mmgsdi_status == MMGSDI_ERROR)
    {
      return mmgsdi_queue_generic_err_rsp_and_index(
               &task_cmd_ptr->cmd,
               MMGSDI_INCORRECT_PARAMS);
    }
    else if((mmgsdi_status == MMGSDI_SUCCESS) &&
            (slot_id != MMGSDI_SLOT_AUTOMATIC))
    {
      mmgsdi_task = mmgsdi_util_get_task_from_slot(slot_id);

      /* Queue the command */
      mmgsdi_status = mmgsdi_cmd_ext(task_cmd_ptr, mmgsdi_task);
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        return MMGSDI_CMD_QUEUED_TO_SLOT_TASK;
      }
      else
      {
        /* In this case i.e. command not successfully queued to slot specific
           task's queue, return an error. This indicates something is wrong
           and so do not continue in main task */

        UIM_MSG_ERR_2("Command could not be queued to slot specific task: 0x%x, status: 0x%x",
                      mmgsdi_task, mmgsdi_status);

        return mmgsdi_queue_generic_err_rsp_and_index(
                 &task_cmd_ptr->cmd,
                 MMGSDI_INCORRECT_PARAMS);
      }
    }
  }

  UIM_MSG_HIGH_3("MMGSDI_PROCESS_COMMAND: 0x%x, task 0x%x, slot 0x%x",
                 task_cmd_ptr->cmd.cmd_enum,
                 mmgsdi_util_get_current_task(),
                 task_cmd_ptr->cmd.cmd.request_header.slot_id);

  /* Validate the command and update input parameters */
  mmgsdi_status = mmgsdi_is_cmd_valid_update_params(task_cmd_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* For invalid REFRESH req., CNF has already been queued */
    if(task_cmd_ptr->cmd.cmd_enum != MMGSDI_REFRESH_REQ)
    {
      return mmgsdi_queue_generic_err_rsp_and_index(
               &task_cmd_ptr->cmd,
               mmgsdi_status);
    }
    else
    {
      return MMGSDI_SUCCESS;
    }
  }

  if (mmgsdi_is_cmd_slot_specific(task_cmd_ptr->cmd.cmd_enum))
  {
    if(mmgsdi_is_cmd_session_specific(task_cmd_ptr->cmd.cmd_enum))
    {
      mmgsdi_status = mmgsdi_util_get_session_app_info(
        task_cmd_ptr->cmd.cmd.read_req.request_header.session_id,
        &task_cmd_ptr->cmd.cmd.read_req.request_header.slot_id,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }
    else
    {
      mmgsdi_status = MMGSDI_SUCCESS;
    }

    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      if (mmgsdi_is_slot_ready_for_cmd(
          &task_cmd_ptr->cmd,
          task_cmd_ptr->cmd.cmd.read_req.request_header.session_id,
          task_cmd_ptr->cmd.cmd.client_id_reg_req.request_header.slot_id,
          &cond_processing))
      {
        proceed_processing = TRUE;
        if(cond_processing == TRUE)
        {
          if(mmgsdi_present_in_preinit_file_access_list(
             &task_cmd_ptr->cmd) == FALSE)
          {
            UIM_MSG_HIGH_0("proceed_processing FALSE");
            proceed_processing = FALSE;
          }
        }/* if(cond_processing == TRUE) */
      }
      else
      {
        proceed_processing = FALSE;
      }
    }
    else
    {
      proceed_processing = FALSE;
    }
  }
  else
  {
    proceed_processing = TRUE;
  }

  if (!proceed_processing)
  {

    UIM_MSG_ERR_1("Unable to proceed with command 0x%x, Use static cnf buffer to build cnf notification",
                  task_cmd_ptr->cmd.cmd_enum);

    /* For MMGSDI_MANAGE_CHANNEL_REQ, return generic error when slot is not ready */
    if (MMGSDI_MANAGE_CHANNEL_REQ == task_cmd_ptr->cmd.cmd_enum)
    {
      mmgsdi_status = mmgsdi_queue_generic_err_rsp_and_index(
                        &task_cmd_ptr->cmd,
                        MMGSDI_ERROR);
    }
    else
    {
      mmgsdi_status = mmgsdi_queue_generic_err_rsp_and_index(
                        &task_cmd_ptr->cmd,
                        MMGSDI_ACCESS_DENIED);
    }

    return mmgsdi_status;
  }

  protocol = mmgsdi_util_determine_protocol(task_cmd_ptr->cmd.cmd.read_req.request_header.slot_id);

  /*----------------------------------------------------------------------------
  Dispatch commmand to proper technology/command handling function.
  -----------------------------------------------------------------------------*/
  switch(task_cmd_ptr->cmd.cmd_enum)
  {
    case MMGSDI_READ_REQ:
      if(protocol == MMGSDI_UICC)
      {
        /* can this be featurized */
        mmgsdi_status = mmgsdi_uicc_read(&task_cmd_ptr->cmd.cmd.read_req);
      } /* protocol is MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_read(&task_cmd_ptr->cmd.cmd.read_req);
      } /* protocol is MMGSDI_ICC */
      break; /* end case - MMGSDI_READ_REQ */

    case MMGSDI_WRITE_REQ:
      mmgsdi_status = mmgsdi_common_write(&task_cmd_ptr->cmd.cmd.write_req, protocol);
      break; /* end case - MMGSDI_WRITE_REQ */

    case MMGSDI_INCREASE_REQ:
      mmgsdi_status = mmgsdi_common_increase(&task_cmd_ptr->cmd.cmd.increase_req, protocol);
      break; /* end case - MMGSDI_INCREASE_REQ */

    case MMGSDI_GET_FILE_ATTR_REQ:
      mmgsdi_status = mmgsdi_common_get_file_attr(
                            &task_cmd_ptr->cmd.cmd.get_file_attr_req,
                            protocol);
      break; /* end case - MMGSDI_GET_FILE_ATTRIBUTES */

    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
      mmgsdi_status = mmgsdi_process_client_reg_req(
                        &task_cmd_ptr->cmd.cmd.client_id_reg_req);
      break;

    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
      mmgsdi_status = mmgsdi_process_client_dereg_req(
                       &task_cmd_ptr->cmd.cmd.client_id_dereg_req);
      break;

    case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
      mmgsdi_status = mmgsdi_gen_available_apps_req(
                        &task_cmd_ptr->cmd.cmd.available_apps_req);
      break;

    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_OPEN_EXT_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
    case MMGSDI_SESSION_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_SESSION_CLOSE_REQ:
    case MMGSDI_ISIM_AUTH_REQ:
      mmgsdi_status = mmgsdi_session_main(&task_cmd_ptr->cmd);
      break;

    case MMGSDI_CARD_PUP_REQ:
      mmgsdi_status = mmgsdi_gen_process_card_pup(&task_cmd_ptr->cmd.cmd.card_pup_req);
      break; /* end case - MMGSDI_CARD_PUP_REQ */

    case MMGSDI_CARD_PDOWN_REQ:
      mmgsdi_status = mmgsdi_gen_process_card_pdown(&task_cmd_ptr->cmd.cmd.card_pdown_req);
      break; /* end case - MMGSDI_CARD_PDOWN_REQ */

    case MMGSDI_CARD_RESET_REQ:
      mmgsdi_status = mmgsdi_gen_process_card_reset(&task_cmd_ptr->cmd.cmd.card_reset_req);
      break; /* end case - MMGSDI_CARD_RESET_REQ */

    case MMGSDI_SAP_CONNECT_REQ:
      mmgsdi_status = mmgsdi_sap_sap_connect(&task_cmd_ptr->cmd.cmd.sap_connect_req);
    break;

    case MMGSDI_SAP_DISCONNECT_REQ:
      mmgsdi_status = mmgsdi_sap_sap_disconnect(&task_cmd_ptr->cmd.cmd.sap_disconnect_req);
    break;

    case MMGSDI_SAP_GET_ATR_REQ:
      mmgsdi_status = mmgsdi_gen_process_get_atr(
        MMGSDI_SAP_GET_ATR_REQ,
        (const mmgsdi_get_atr_req_type*)&task_cmd_ptr->cmd.cmd.sap_get_atr_req);
      break;  /* end case - MMGSDI_SAP_GET_ATR_REQ */

    case MMGSDI_SAP_SEND_APDU_REQ:
      mmgsdi_status = mmgsdi_sap_sap_send_apdu(&task_cmd_ptr->cmd.cmd.sap_send_apdu_req, protocol);
      break; /* end case - MMGSDI_SAP_SEND_APDU_REQ */

    case MMGSDI_SAP_POWER_ON_REQ:
      mmgsdi_status = mmgsdi_sap_sap_power_on(&task_cmd_ptr->cmd.cmd.sap_power_on_req);
      break; /* end case - MMGSDI_SAP_POWER_ON_REQ */

    case MMGSDI_SAP_POWER_OFF_REQ:
      mmgsdi_status = mmgsdi_sap_sap_power_off(&task_cmd_ptr->cmd.cmd.sap_power_off_req);
      break; /* end case - MMGSDI_SAP_POWER_OFF_REQ */

    case MMGSDI_SAP_RESET_REQ:
      mmgsdi_status = mmgsdi_sap_sap_reset(&task_cmd_ptr->cmd.cmd.sap_reset_req);
      break; /* end case - MMGSDI_SAP_RESET_REQ */

    case MMGSDI_CARD_STATUS_REQ:
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if(protocol == MMGSDI_UICC)
      {
        mmgsdi_status = mmgsdi_uicc_status(&task_cmd_ptr->cmd.cmd.status_req);
      } /* protocol is MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_status(&task_cmd_ptr->cmd.cmd.status_req);
      } /* protocol is MMGSDI_ICC */
      break; /* end case - MMGSDI_CARD_STATUS_REQ */

    case MMGSDI_REFRESH_REQ:
      mmgsdi_status = mmgsdi_refresh_main(&task_cmd_ptr->cmd.cmd.refresh_req);
      break; /* end case - MMGSDI_REFRESH_REQ */

    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
      mmgsdi_status = mmgsdi_common_activate_onchip(&task_cmd_ptr->cmd.cmd.act_onchip_sim_req,
                                                    task_cmd_ptr->cmd.cmd.act_onchip_sim_req.onchip_sim_mode);
      break;

    case MMGSDI_REHABILITATE_REQ:
      mmgsdi_status = mmgsdi_process_rehabilitate_cmd(&task_cmd_ptr->cmd.cmd.rehab_req, protocol);
      break; /* end case - MMGSDI_REHABILITATE_REQ */

    case MMGSDI_INVALIDATE_REQ:
      mmgsdi_status = mmgsdi_process_invalidate_cmd(&task_cmd_ptr->cmd.cmd.invalidate_req, protocol);
      break; /* end case - MMGSDI_INVALIDATE_REQ */

    case MMGSDI_SEND_APDU_REQ:
    case MMGSDI_SEND_APDU_EXT_REQ:
      mmgsdi_status = mmgsdi_gen_process_send_apdu(&task_cmd_ptr->cmd.cmd.send_apdu_req, protocol);
      break;

    case MMGSDI_PIN_OPERATION_REQ:
      mmgsdi_status = mmgsdi_process_pin_op_cmd(&task_cmd_ptr->cmd.cmd.pin_operation_req, protocol);
      break; /* end case - MMGSDI_PIN_OPERATION_REQ */

    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
      mmgsdi_status = mmgsdi_process_get_all_pin_status_cmd(&task_cmd_ptr->cmd.cmd.pin_status_req, protocol);
      break; /* end case - MMGSDI_PIN_OPERATION_REQ */

    case MMGSDI_SRV_AVAILABLE_REQ:
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if(protocol == MMGSDI_UICC)
      {
        mmgsdi_status = mmgsdi_uicc_is_service_available(&task_cmd_ptr->cmd.cmd.srv_available_req);
      } /* protocol is MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_is_service_available(&task_cmd_ptr->cmd.cmd.srv_available_req);
      } /* protocol is MMGSDI_ICC */
      break; /* end case - MMGSDI_SRV_AVAILABLE_REQ */

    case MMGSDI_USIM_AUTH_REQ:
#ifdef FEATURE_MMGSDI_3GPP
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if(protocol == MMGSDI_UICC)
      {
        mmgsdi_status = mmgsdi_uicc_auth(&task_cmd_ptr->cmd.cmd.auth_req);
      }
      else
      {
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP */
      break; /* end case - MMGSDI_USIM_AUTH_REQ */

    case MMGSDI_RUN_CAVE_REQ:
      mmgsdi_status = mmgsdi_common_run_cave(&task_cmd_ptr->cmd.cmd.run_cave_req,
                                             protocol);
      break; /* end case - MMGSDI_RUN_CAVE_REQ */

    case MMGSDI_SESSION_CDMA_AKA_REQ:
      mmgsdi_status = mmgsdi_common_cdma_aka_auth(
                        &task_cmd_ptr->cmd.cmd.cdma_aka_req, protocol);
      break; /* end case - MMGSDI_SESSION_CDMA_AKA_REQ */

    case MMGSDI_GENERATE_KEY_VPM_REQ:
      mmgsdi_status = mmgsdi_common_generate_key_vpm(
                        &task_cmd_ptr->cmd.cmd.generate_key_req, protocol);
      break; /* end case - MMGSDI_GENERATE_KEY_VPM_REQ */

    case MMGSDI_COMPUTE_IP_AUTH_REQ:
      mmgsdi_status = mmgsdi_common_compute_ip_auth(
                        &task_cmd_ptr->cmd.cmd.compute_ip_req, protocol);
      break; /* end case - MMGSDI_COMPUTE_IP_AUTH_REQ */

    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
      mmgsdi_status = mmgsdi_util_get_slot_for_cmd(task_cmd_ptr, &slot_id);
      if (MMGSDI_FEATURE_ENABLED == mmgsdi_nv_get_jcdma_service_status(slot_id))
      {
        mmgsdi_status = mmgsdi_process_jcdma_card_get_info_cmd(
                                   &task_cmd_ptr->cmd.cmd.jcdma_card_info_req);
      }
      else
      {
        UIM_MSG_HIGH_0("JCDMA Service is disabled on ME");
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
      break; /* end case - MMGSDI_JCDMA_GET_CARD_INFO_REQ */

    case MMGSDI_GET_ATR_REQ:
      mmgsdi_status = mmgsdi_gen_process_get_atr(MMGSDI_GET_ATR_REQ, &task_cmd_ptr->cmd.cmd.get_atr_req);
      break; /* end case - MMGSDI_GET_ATR_REQ */

    case MMGSDI_BCAST_REQ:
#ifdef FEATURE_MMGSDI_3GPP
      if(protocol == MMGSDI_UICC)
      {
        mmgsdi_status = mmgsdi_uicc_bcast(&task_cmd_ptr->cmd.cmd.bcast_req_type);
      }
      else
      {
        UIM_MSG_ERR_0("Incorrect Protocol for BCAST AUTH");
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP */
      break; /* end case - MMGSDI_BCAST_REQ */

    case MMGSDI_SEARCH_REQ:
      if (protocol != MMGSDI_UICC)
      {
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
        break;
      }

      mmgsdi_status = mmgsdi_common_search(&task_cmd_ptr->cmd.cmd.search_req, protocol);
      break;

    case MMGSDI_SESSION_GET_INFO_REQ:
      mmgsdi_status = mmgsdi_gen_session_get_info(&task_cmd_ptr->cmd.cmd.session_get_info_req);
      break;

    case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
#ifdef FEATURE_MMGSDI_3GPP
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if (protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_icc_run_gsm_algo(&task_cmd_ptr->cmd.cmd.session_run_gsm_algo_req);
      }
      else
      {
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP */
      break; /* end case - MMGSDI_SESSION_RUN_GSM_ALGO_REQ */

    case MMGSDI_SESSION_STORE_ESN_REQ:
#ifdef FEATURE_MMGSDI_3GPP2
      mmgsdi_status = mmgsdi_card_init_store_esn(&task_cmd_ptr->cmd.cmd.session_store_esn_req);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if (protocol == MMGSDI_UICC)
      {
        mmgsdi_status = mmgsdi_uicc_process_enable_or_disable_srv(
                          task_cmd_ptr->cmd.cmd.session_enable_service_req.request_header,
                          task_cmd_ptr->cmd.cmd.session_enable_service_req.service,
                          MMGSDI_SESSION_ENABLE_SERVICE_REQ);
      }
      else
      {
        mmgsdi_status = mmgsdi_icc_process_enable_service(
                          &task_cmd_ptr->cmd.cmd.session_enable_service_req);
      }
      break;

    case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
      mmgsdi_status = mmgsdi_gen_session_manage_illegal_subscription(
                            &task_cmd_ptr->cmd.cmd.session_manage_illegal_subscription_req);
      break;

    case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if (protocol == MMGSDI_UICC)
      {
        mmgsdi_status = mmgsdi_uicc_process_enable_or_disable_srv(
                          task_cmd_ptr->cmd.cmd.session_disable_service_req.request_header,
                          task_cmd_ptr->cmd.cmd.session_disable_service_req.service,
                          MMGSDI_SESSION_DISABLE_SERVICE_REQ);
      }
      else
      {
        mmgsdi_status = mmgsdi_icc_process_disable_service(
                          &task_cmd_ptr->cmd.cmd.session_disable_service_req);
      }
      break;

    case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ:
      mmgsdi_status = mmgsdi_gen_prov_app_init_complete(
                    &task_cmd_ptr->cmd.cmd.session_prov_app_init_complete_req);
      break;

    case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
      mmgsdi_status = mmgsdi_gen_get_cphs_info(
                    &task_cmd_ptr->cmd.cmd.session_get_cphs_info_req);
      break;

    case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
      mmgsdi_status = mmgsdi_process_get_app_capabilities_req(
                      &task_cmd_ptr->cmd.cmd.session_get_app_cap_req);
      break;

    case MMGSDI_SESSION_SSD_UPDATE_REQ:
      mmgsdi_status = mmgsdi_common_ssd_update(
                        &task_cmd_ptr->cmd.cmd.session_ssd_update_req, protocol);
      break;

    case MMGSDI_SESSION_SSD_CONFIRM_REQ:
      mmgsdi_status = mmgsdi_common_ssd_confirm(
                        &task_cmd_ptr->cmd.cmd.session_ssd_confirm_req, protocol);
      break;

    case MMGSDI_SESSION_BS_CHAL_REQ:
      mmgsdi_status = mmgsdi_common_bs_chal(
                        &task_cmd_ptr->cmd.cmd.session_bs_chal_req, protocol);
      break;

    case MMGSDI_SESSION_OTASP_MS_KEY_REQ:
      mmgsdi_status = mmgsdi_common_otasp_ms_key(
                        &task_cmd_ptr->cmd.cmd.session_otasp_ms_key_req, protocol);
      break;

    case MMGSDI_SESSION_OTASP_KEY_GEN_REQ:
      mmgsdi_status = mmgsdi_common_otasp_key_gen(
                        &task_cmd_ptr->cmd.cmd.session_otasp_key_gen_req, protocol);
      break;

    case MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ:
      mmgsdi_status = mmgsdi_common_otasp_sspr_config(
                        &task_cmd_ptr->cmd.cmd.session_otasp_sspr_config_req, protocol);
      break;

    case MMGSDI_SESSION_OTASP_COMMIT_REQ:
      mmgsdi_status = mmgsdi_common_otasp_commit(
                        &task_cmd_ptr->cmd.cmd.session_otasp_commit_req, protocol);
      break;

    case MMGSDI_SESSION_OTASP_OTAPA_REQ:
      mmgsdi_status = mmgsdi_common_otasp_otapa(
                        &task_cmd_ptr->cmd.cmd.session_otasp_otapa_req, protocol);
      break;

    case MMGSDI_SESSION_OTASP_BLOCK_REQ:
      mmgsdi_status = mmgsdi_common_otasp_block(
                        &task_cmd_ptr->cmd.cmd.session_otasp_block_req, protocol);
      break;

    case MMGSDI_SESSION_READ_PRL_REQ:
      mmgsdi_status = mmgsdi_util_get_slot_for_cmd(task_cmd_ptr, &slot_id);
      if (MMGSDI_FEATURE_ENABLED == mmgsdi_nv_get_jcdma_service_status(slot_id))
      {
        UIM_MSG_ERR_0("JCDMA enabled, PRL is not allow to read from the card");
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
      else
      {
        mmgsdi_status = mmgsdi_gen_session_read_prl(
                          &task_cmd_ptr->cmd.cmd.session_read_prl_req);
      }
      break;

    case MMGSDI_SESSION_SUB_OK_REQ:
      mmgsdi_status = mmgsdi_process_sub_ok_req(
                        &task_cmd_ptr->cmd.cmd.session_sub_ok_req);
      break;

    case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
      mmgsdi_status = mmgsdi_process_tp_download_complete(
                        &task_cmd_ptr->cmd.cmd.tp_dl_complete_req);
      break;

    case MMGSDI_RECOVERY_IND_REQ:
      mmgsdi_status = mmgsdi_process_recovery_ind_req(
                        &task_cmd_ptr->cmd.cmd.recovery_ind_req, protocol);
      break;

    case MMGSDI_EONS_SS_EVENT_REQ:
      mmgsdi_status = mmgsdi_process_eons_ss_event(
                        &task_cmd_ptr->cmd.cmd.eons_ss_event_req);
      break;

    case MMGSDI_FULL_SERVICE_REQ:
      mmgsdi_status = mmgsdi_util_get_prov_session_info(
                        task_cmd_ptr->cmd.cmd.simlock_ss_event_req.request_header.session_id,
                        NULL,
                        &slot_id,
                        &app_info_ptr);

      if(mmgsdi_status == MMGSDI_SUCCESS &&
         app_info_ptr != NULL &&
         app_info_ptr->service_status != SIMLOCK_SRV_STATUS_FULL)
      {
        /* For certain slot policies, the SimLock algorithm needs to
           be re-run when phones reaches full service for the first time. */
        app_info_ptr->service_status = SIMLOCK_SRV_STATUS_FULL;
#ifdef FEATURE_SIMLOCK
        mmgsdi_simlock_build_and_run_algo(FALSE);
#endif /* FEATURE_SIMLOCK */
      }
      break;

    case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
      mmgsdi_status = mmgsdi_process_get_operator_name(
                        &task_cmd_ptr->cmd.cmd.session_get_eons_req);
      break;

    case MMGSDI_NOTIFY_LINK_EST_REQ:
      mmgsdi_status = mmgsdi_card_init_process_link_est(
                        &task_cmd_ptr->cmd.cmd.link_est_req);
      break;

    case MMGSDI_NOTIFY_CARD_ERROR_REQ:
      UIM_MSG_MED_2("Received MMGSDI_NOTIFY_CARD_ERROR_REQ for slot 0x%x, cause 0x%x",
                    task_cmd_ptr->cmd.cmd.card_error_req.request_header.slot_id,
                    task_cmd_ptr->cmd.cmd.card_error_req.error_condition);
      mmgsdi_status = mmgsdi_card_init_process_card_error(
                        &task_cmd_ptr->cmd.cmd.card_error_req);
      break;

  case MMGSDI_NOTIFY_SIM_BUSY_REQ:
      UIM_MSG_MED_1("Received MMGSDI_NOTIFY_SIM_BUSY_REQ with sim_busy 0x%x",
                    task_cmd_ptr->cmd.cmd.sim_busy_req.sim_busy);
      mmgsdi_status = mmgsdi_card_init_process_sim_busy(
                        &task_cmd_ptr->cmd.cmd.sim_busy_req);
      break;

    case MMGSDI_INTERNAL_PUP_INIT_REQ:
      mmgsdi_status = mmgsdi_card_init_process_internal_pup(
                        &task_cmd_ptr->cmd.cmd.internal_pup_init_req);
      break;

    case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
      mmgsdi_status = mmgsdi_process_file_update_registration(&task_cmd_ptr->cmd.cmd.file_update_req);
      break; /* end case - MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ */

    case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
      mmgsdi_status = mmgsdi_uicc_gstk_loc_env_rsp_received(
                        &task_cmd_ptr->cmd.cmd.gstk_loc_env_rsp_received_req);
      break;

    case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
      mmgsdi_status = mmgsdi_eons_process_get_se13_plmn_names(
                        &task_cmd_ptr->cmd.cmd.get_se13_plmn_names_req);
      break;

    case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
      mmgsdi_status = mmgsdi_csg_process_update_acsgl_record_reqest(&task_cmd_ptr->cmd.cmd.update_acsgl_record_req);
      break; /* end case - MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ */

    case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ:
      mmgsdi_status = mmgsdi_se13_process_get_se13_plmn_info_by_name(
                        &task_cmd_ptr->cmd.cmd.get_se13_plmn_info_by_name_req);
      break;

    case MMGSDI_SET_BUILTIN_PLMN_LIST_REQ:
      mmgsdi_status = mmgsdi_process_set_builtin_plmn_list(
                        &task_cmd_ptr->cmd.cmd.set_builtin_plmn_list_req);
     break;

    case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
      mmgsdi_status = mmgsdi_process_continue_after_nv_refresh(
                        &task_cmd_ptr->cmd.cmd.continue_after_nv_refresh_req);
      break;

    case MMGSDI_HANDLE_NV_REFRESH_REQ:
      mmgsdi_status = mmgsdi_process_nv_refresh_req(
                       &task_cmd_ptr->cmd.cmd.mcfg_refresh_data);
      break;

    case MMGSDI_MANAGE_CHANNEL_REQ:
      mmgsdi_status = mmgsdi_uim_uicc_manage_channel_req(
                        &task_cmd_ptr->cmd.cmd.manage_channel_req);
      break;

    case MMGSDI_GET_SIM_PROFILE_REQ:
#ifdef FEATURE_UIM_MULTI_PROFILE
      mmgsdi_status = mmgsdi_multi_profile_get_sim_profile(
                        &task_cmd_ptr->cmd.cmd.get_sim_profile_req, protocol);
#endif /* FEATURE_UIM_MULTI_PROFILE */
      break;

    case MMGSDI_SET_SIM_PROFILE_REQ:
#ifdef FEATURE_UIM_MULTI_PROFILE
      mmgsdi_status = mmgsdi_multi_profile_set_sim_profile(
                        &task_cmd_ptr->cmd.cmd.set_sim_profile_req, protocol);
#endif /* FEATURE_UIM_MULTI_PROFILE */
      break;

    case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
      mmgsdi_cache_delete_pkcs15_attr_cache(
        task_cmd_ptr->cmd.cmd.start_silent_file_change_req.request_header.slot_id);
      break;

    case MMGSDI_SIMLOCK_RSP:
    case MMGSDI_SIMLOCK_EVT:
    case MMGSDI_SAP_REPORT_RSP:
    case MMGSDI_UIM_REPORT_RSP:
    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
    case MMGSDI_CLIENT_ID_REG_RSP:
    case MMGSDI_CNF:
    case MMGSDI_MAX_CMD_ENUM:
    /* Note - There is no default case here and it is done on purpose since we
       need to handle all values for mmgsdi_cmd_enum_type. Unhandled values
       will get flagged by lint error */
      /* Cmd not to be handled by process_command */
      return MMGSDI_ERROR;

  } /* end switch - command type */

  if( (mmgsdi_status != MMGSDI_SUCCESS) &&
      (mmgsdi_status != MMGSDI_REFRESH_IN_PROGRESS) )
  {
    mmgsdi_status = mmgsdi_queue_generic_err_rsp_and_index(
                       &task_cmd_ptr->cmd,
                       mmgsdi_status);
  } /* end if - mmgsdi_status is not SUCCESS */
  return mmgsdi_status;
} /* mmgsdi_process_command */


#ifdef FEATURE_MMGSDI_3GPP2
/*===========================================================================
FUNCTION MMGSDI_PROCESS_OTASP_COMMIT_RSP

DESCRIPTION
  This function, called in mmgsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate OTASP commit rsp
  function.

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
static mmgsdi_return_enum_type mmgsdi_process_otasp_commit_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr,
  mmgsdi_protocol_enum_type         protocol
)
{
  mmgsdi_return_enum_type       mmgsdi_status         = MMGSDI_SUCCESS;
  int32                         index                 = 0;
  mmgsdi_file_enum_type         items_not_deleted[1];
  uint16                        num_items             = 0;
  mmgsdi_session_type_enum_type session_type          = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_app_enum_type          app_type              = MMGSDI_APP_UNKNOWN;
  mmgsdi_return_enum_type       session_status        = MMGSDI_ERROR;
  mmgsdi_slot_data_type        *slot_data_ptr         = NULL;
  mmgsdi_uim_report_rsp_type   *otasp_commit_rsp_ptr  = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  index = uim_rsp_ptr->rsp_data_index;

  if ((index < 0) || (index >= MMGSDI_MAX_CLIENT_REQ_INFO) ||
      protocol == MMGSDI_NO_PROTOCOL)
  {
    UIM_MSG_ERR_2("Invalid index 0x%x or protocol 0x%x", index, protocol);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    UIM_MSG_ERR_1("Index 0x%x not populated in MMGSDI client req table", index);
    return MMGSDI_ERROR;
  }

  if(protocol == MMGSDI_UICC)
  {
    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(
                      mmgsdi_client_req_table_info_ptr[index]->slot_id);
    if(slot_data_ptr != NULL &&
       slot_data_ptr->upp_update_required &&
       uim_rsp_ptr->rsp_data.data_ptr[0] == MMGSDI_OTASP_COMMIT_RSP_SUCCESS)
    {
      /* OTASP Commit was successful and user profile parameter update is
         required. Make a copy of response data which will be freed after
         updating the user profile parameters. */
      mmgsdi_status = mmgsdi_util_alloc_copy_uim_rsp(&otasp_commit_rsp_ptr,
                                                     uim_rsp_ptr);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_1x_read_mdn_and_validate_upp_files(
                          mmgsdi_client_req_table_info_ptr[index]->session_id,
                          otasp_commit_rsp_ptr);
        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          return MMGSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE;
        }
      }
    }

    items_not_deleted[num_items++] = MMGSDI_CSIM_LANG_IND;
    app_type                       = MMGSDI_APP_CSIM;
  } /* protocol is MMGSDI_UICC */
  else
  {
    items_not_deleted[num_items++] = MMGSDI_CDMA_PREF_LANG;
    app_type             = MMGSDI_APP_RUIM;
  } /* protocol is MMGSDI_ICC */

  mmgsdi_status = mmgsdi_common_rsp_otasp_commit(uim_rsp_ptr, protocol);

  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (uim_rsp_ptr->rsp_data.data_ptr != NULL) &&
      (uim_rsp_ptr->rsp_data.data_ptr[0] == MMGSDI_OTASP_COMMIT_RSP_SUCCESS))
  {
    session_status = mmgsdi_util_get_session_type(
                      mmgsdi_client_req_table_info_ptr[index]->session_id,
                      &session_type, NULL);

    if (session_status == MMGSDI_SUCCESS)
    {
      (void)mmgsdi_cache_delete_prov_file_attr_cache(session_type,
                                                     items_not_deleted,
                                                     num_items);
      mmgsdi_cache_delete_record_selective_items(
        mmgsdi_client_req_table_info_ptr[index]->session_id, NULL, 0);

      (void)mmgsdi_cache_delete_selective_items(app_type,
                                                session_type,
                                                items_not_deleted,
                                                num_items);
      UIM_MSG_HIGH_0("CDMA cache cleared after OTASP commit");
    }
    else
    {
      UIM_MSG_ERR_1("Get session type err after OTASP commit.Status=0x%x",
                    mmgsdi_status);
    }
  }

  return mmgsdi_status;
}/* mmgsdi_process_otasp_commit_rsp*/
#endif /* FEATURE_MMGSDI_3GPP2 */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_SUB_OK_RSP

DESCRIPTION
  This function is called in mmgsdi_process_response.
  It serves mainly to pack the response and queue the confirmation

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_sub_ok_rsp (
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_request_header_type         req_header       = {0};
  int32                              index            = 0;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;

  /* Check input pointer */
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

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

  return mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,&req_header,
           extra_param_ptr,NULL,TRUE,uim_rsp_ptr->status_word);
} /* mmgsdi_process_sub_ok_rsp */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_RESPONSE

DESCRIPTION
  This function, called in gsdimain, is called to process an mmgsdi response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the response type, and call the appropriate response processing
  function.

DEPENDENCIES
  gsdimain must be finished initialization.

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
static mmgsdi_return_enum_type mmgsdi_process_response(
  const mmgsdi_task_cmd_type *  task_cmd_ptr /* GSDI response to be processed */
)
{

/*-----------------------------------------------------------------------------
  Initialize variables
-----------------------------------------------------------------------------*/
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_protocol_enum_type protocol      = MMGSDI_NO_PROTOCOL;
  mmgsdi_cmd_enum_type      cmd_req_enum  = MMGSDI_MAX_CMD_ENUM;
  int32                     index         = 0;
  boolean                   free_index    = TRUE;
  mmgsdi_slot_id_enum_type  slot_id       = MMGSDI_SLOT_AUTOMATIC;
  mmgsdi_task_enum_type     mmgsdi_task   = MMGSDI_TASK_MAX;

  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  /* Obtain the index and check the request type */
  switch(task_cmd_ptr->cmd.cmd_enum)
  {
    case MMGSDI_CLIENT_ID_REG_RSP:
      index = task_cmd_ptr->cmd.cmd.client_id_reg_rsp.rsp_data_index;
      break;

    case MMGSDI_UIM_REPORT_RSP:
      index = task_cmd_ptr->cmd.cmd.uim_report_rsp.rsp_data_index;
      break;

    default:
      UIM_MSG_ERR_1("Incorrect cmd enum type in process response 0x%x",
                    task_cmd_ptr->cmd.cmd_enum);
      return MMGSDI_ERROR;
  }

  if( ( index < 0 ) ||
      ( index >= MMGSDI_MAX_CLIENT_REQ_INFO ) ||
      mmgsdi_client_req_table_info_ptr[index] == NULL)
  {
    UIM_MSG_ERR_1("Index 0x%x, not populated in MMGSDI client req table", index);
    return MMGSDI_ERROR;
  }

  /* Get the current task */
  mmgsdi_task = mmgsdi_util_get_current_task();
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case current task is not retrieved,
       queue to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }

  /* Set UIM cmd_in_progress to FALSE if card response is received */
  if((mmgsdi_uim_cmd[mmgsdi_task].cmd_in_progress) &&
     (mmgsdi_uim_cmd[mmgsdi_task].index == index))
  {
    mmgsdi_uim_cmd[mmgsdi_task].cmd_in_progress = FALSE;
    mmgsdi_uim_cmd[mmgsdi_task].index = MMGSDI_MAX_CLIENT_REQ_INFO;
  }

  /* If a response was not received for this command before the last
     CARD_ERROR, free only the client_req_table index. A cnf for this command
     must have been sent at CARD_ERROR. */
  if(mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED)
  {
    UIM_MSG_HIGH_2("Free client_req_table index 0x%x without processing rsp for cmd 0x%x",
                   index, mmgsdi_client_req_table_info_ptr[index]->request_type);
    mmgsdi_util_free_client_request_table_index(index);
    return MMGSDI_SUCCESS;
  }

  /* Initial checks passed, now process this rsp */
  mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status = MMGSDI_UIM_RSP_PROCESSING;

  /* Clear the sanity timer associated with the command. For the command that
     didn't receive a response, the timer is cleared before sending the cnf
     at the last CARD_ERROR */
  (void)rex_clr_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task]);

  cmd_req_enum = mmgsdi_client_req_table_info_ptr[index]->request_type;
  slot_id      = mmgsdi_client_req_table_info_ptr[index]->slot_id;

  protocol = mmgsdi_util_determine_protocol(slot_id);

  UIM_MSG_HIGH_4("MMGSDI_PROCESS_RESPONSE for cmd 0x%x, task 0x%x, slot 0x%x, protocol 0x%x",
                  cmd_req_enum, mmgsdi_task, slot_id, protocol);

  /*----------------------------------------------------------------------------
    Dispatch to proper technology/command response handling function
  -----------------------------------------------------------------------------*/
  switch(cmd_req_enum)
  {
    case MMGSDI_READ_REQ:
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
      mmgsdi_status = MMGSDI_ERROR;
      }
      else if(protocol == MMGSDI_UICC)
      {
        mmgsdi_status = mmgsdi_uicc_rsp_read(&task_cmd_ptr->cmd.cmd.uim_report_rsp,
                                             NULL, FALSE);
      } /* end if - MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_rsp_read(&task_cmd_ptr->cmd.cmd.uim_report_rsp,
                                            NULL, FALSE);
      } /* end else - MMGSDI_ICC */
      break; /* end case - MMGSDI_READ_REQ*/

    case MMGSDI_WRITE_REQ:
      mmgsdi_status = mmgsdi_common_rsp_write(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp,
                        NULL,
                        FALSE,
                        protocol);
      break; /* end case - MMGSDI_WRITE_REQ*/

    case MMGSDI_INCREASE_REQ:
      mmgsdi_status = mmgsdi_common_rsp_increase(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp,
                        NULL,
                        FALSE,
                        protocol);
      break; /* end case - MMGSDI_INCREASE_REQ*/

    case MMGSDI_GET_FILE_ATTR_REQ:
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if(protocol == MMGSDI_UICC)
      {
        mmgsdi_status = mmgsdi_uicc_rsp_get_file_attr(&task_cmd_ptr->cmd.cmd.uim_report_rsp, NULL, FALSE);
      } /* end if - MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_rsp_get_file_attr(
                          &task_cmd_ptr->cmd.cmd.uim_report_rsp, NULL, FALSE);
      } /* end else - MMGSDI_ICC */
      break; /* end case - MMGSDI_GET_FILE_ATTRIBUTES */

    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
      /* cnf_enum if being set in the process function */
      mmgsdi_status = mmgsdi_process_client_reg_cnf(&task_cmd_ptr->cmd.cmd.client_id_reg_rsp);
      break;

    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
      /* cnf_enum if being set in the process function */
      mmgsdi_status = mmgsdi_process_client_dereg_cnf(
                       &task_cmd_ptr->cmd.cmd.client_id_reg_rsp);
      break;

    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_OPEN_EXT_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_SESSION_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
    case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
    case MMGSDI_SESSION_CLOSE_REQ:
    case MMGSDI_ISIM_AUTH_REQ:
      mmgsdi_status = mmgsdi_session_main_response(cmd_req_enum, &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_SESSION_XXX_REQ */

    case MMGSDI_SAP_CONNECT_REQ:
      mmgsdi_status = mmgsdi_sap_rsp_sap_connect(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_SAP_CONNECT_REQ*/

    case MMGSDI_SAP_DISCONNECT_REQ:
      mmgsdi_status = mmgsdi_sap_rsp_sap_disconnect(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SAP_GET_ATR_REQ:
      mmgsdi_status = mmgsdi_gen_rsp_get_atr(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
    break; /* end case - MMGSDI_SAP_GET_ATR_REQ*/

    case MMGSDI_SAP_SEND_APDU_REQ:
      mmgsdi_status = mmgsdi_sap_rsp_sap_send_apdu(&task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
    break; /* end case - MMGSDI_SAP_SEND_APDU_REQ*/

    case MMGSDI_SAP_POWER_ON_REQ:
      mmgsdi_status = mmgsdi_sap_rsp_sap_power_on(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
    break; /* end case - MMGSDI_SAP_POWER_ON_REQ*/

    case MMGSDI_SAP_POWER_OFF_REQ:
      mmgsdi_status = mmgsdi_sap_rsp_sap_power_off(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
       break; /* end case - MMGSDI_SAP_POWER_OFF_REQ*/

    case MMGSDI_SAP_RESET_REQ:
      mmgsdi_status = mmgsdi_sap_rsp_sap_reset(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_SAP_RESET_REQ*/

    case MMGSDI_CARD_PUP_REQ:
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_CARD_PUP_REQ */

    case MMGSDI_CARD_PDOWN_REQ:
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_CARD_PDOWN_REQ */

    case MMGSDI_CARD_STATUS_REQ:
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if(protocol == MMGSDI_UICC)
      {
        mmgsdi_status = mmgsdi_uicc_rsp_status(&task_cmd_ptr->cmd.cmd.uim_report_rsp, NULL, FALSE);
      } /* end if - MMGSDI_UICC */
      else
      {
        mmgsdi_status = mmgsdi_icc_rsp_status(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      } /* end else - MMGSDI_ICC */
      break; /* end case - MMGSDI_CARD_STATUS_REQ */

    case MMGSDI_CARD_RESET_REQ:
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_CARD_RESET_REQ */

    case MMGSDI_REFRESH_REQ:
      mmgsdi_status = mmgsdi_refresh_main_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_REFRESH_REQ */

    case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
      mmgsdi_status = mmgsdi_process_file_update_registration_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ */

    case MMGSDI_REHABILITATE_REQ:
      mmgsdi_status = mmgsdi_process_rehabilitate_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_INVALIDATE_REQ:
      mmgsdi_status = mmgsdi_process_invalidate_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_PIN_OPERATION_REQ:
      mmgsdi_status = mmgsdi_process_pin_op_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break; /* end case - MMGSDI_PIN_OPERATION_REQ */

    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
      mmgsdi_status = mmgsdi_process_get_all_pin_status_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break; /* end case - MMGSDI_GET_ALL_PIN_STATUS_REQ */

    case MMGSDI_SRV_AVAILABLE_REQ:
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else
      {
        mmgsdi_status = mmgsdi_common_rsp_srv_available(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      }
      break; /* end case - MMGSDI_SRV_AVAILABLE_REQ */

    case MMGSDI_USIM_AUTH_REQ:
    #ifdef FEATURE_MMGSDI_3GPP
      mmgsdi_status = mmgsdi_uicc_rsp_auth(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
    #else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
    #endif /* FEATURE_MMGSDI_3GPP */
      break; /* end case - MMGSDI_USIM_AUTH_REQ */

    case MMGSDI_COMPUTE_IP_AUTH_REQ:
      mmgsdi_status = mmgsdi_common_rsp_compute_ip_auth(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break; /* end case - MMGSDI_COMPUTE_IP_AUTH_REQ */

    case MMGSDI_RUN_CAVE_REQ:
      mmgsdi_status = mmgsdi_common_rsp_run_cave(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break; /* end case - MMGSDI_RUN_CAVE_REQ */

    case MMGSDI_SESSION_CDMA_AKA_REQ:
      mmgsdi_status = mmgsdi_common_rsp_cdma_aka_auth(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break; /* end case - MMGSDI_SESSION_CDMA_AKA_REQ */

    case MMGSDI_GENERATE_KEY_VPM_REQ:
      mmgsdi_status = mmgsdi_common_rsp_generate_key_vpm(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break; /* end case - MMGSDI_GENERATE_KEY_VPM_REQ */

    case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
      UIM_MSG_MED_0("Received respone for MMGSDI_JCDMA_GET_CARD_INFO_REQ");
      if (MMGSDI_FEATURE_ENABLED == mmgsdi_nv_get_jcdma_service_status(slot_id))
      {
        mmgsdi_status = mmgsdi_jcdma_rsp_get_card_info(
                          &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      }
      else
      {
        UIM_MSG_HIGH_0("JCDMA Service is disabled on ME");
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
      break;

    case MMGSDI_SEND_APDU_REQ:
    case MMGSDI_SEND_APDU_EXT_REQ:
      mmgsdi_status = mmgsdi_gen_rsp_send_apdu_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break; /* end case - MMGSDI_SEND_APDU_REQ*/

    case MMGSDI_GET_ATR_REQ:
      mmgsdi_status = mmgsdi_gen_rsp_get_atr(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

   case MMGSDI_BCAST_REQ:
      UIM_MSG_MED_0("Received response for MMGSDI_BCAST_REQ");
#ifdef FEATURE_MMGSDI_3GPP
      mmgsdi_status = mmgsdi_uicc_rsp_bcast(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP */
      break; /* end case - MMGSDI_BCAST_REQ */

    case MMGSDI_SEARCH_REQ:
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }

      mmgsdi_status = mmgsdi_common_rsp_search(
                          &task_cmd_ptr->cmd.cmd.uim_report_rsp,
                          NULL, FALSE, protocol);
      break; /* end case - MMGSDI_SEARCH_REQ*/

    case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
#ifdef FEATURE_MMGSDI_3GPP
      if(protocol == MMGSDI_NO_PROTOCOL)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      else if (protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_icc_rsp_run_gsm_algo(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      }
      else
      {
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      }
#else
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP */
      break; /* end case - MMGSDI_RUN_GSM_ALGO_REQ */

    case MMGSDI_SESSION_GET_INFO_REQ:
      mmgsdi_status = mmgsdi_gen_rsp_session_get_info(
                            &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_READ_PRL_REQ:
      mmgsdi_status = mmgsdi_gen_rsp_session_read_prl(
                            &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_STORE_ESN_REQ:
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
      break;

    case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ:
      mmgsdi_status = mmgsdi_gen_rsp_session_prov_app_init_complete(
                            &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
      /*If we are here, ENABLE SERVICE request handling failed */
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
      /*If we are here, DISABLE SERVICE request handling failed */
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
      mmgsdi_status = mmgsdi_gen_rsp_get_cphs_info(
                    &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
      mmgsdi_status = mmgsdi_gen_rsp_get_app_capabilities(
                         &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_SSD_UPDATE_REQ:
      mmgsdi_status = mmgsdi_common_rsp_ssd_update(
                         &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_SESSION_SSD_CONFIRM_REQ:
      mmgsdi_status = mmgsdi_common_rsp_ssd_confirm(
                         &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_SESSION_BS_CHAL_REQ:
      mmgsdi_status = mmgsdi_common_rsp_bs_chal(
                         &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_SESSION_OTASP_MS_KEY_REQ:
      mmgsdi_status = mmgsdi_common_rsp_otasp_ms_key(
                         &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_SESSION_OTASP_KEY_GEN_REQ:
      mmgsdi_status = mmgsdi_common_rsp_otasp_key_gen(
                         &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ:
      mmgsdi_status = mmgsdi_common_rsp_otasp_sspr_config(
                         &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_SESSION_OTASP_COMMIT_REQ:
#ifdef FEATURE_MMGSDI_3GPP2
      mmgsdi_status = mmgsdi_process_otasp_commit_rsp(
                         &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      if(mmgsdi_status == MMGSDI_SUCCESS_AFTER_CARD_INTERNAL_UPDATE)
      {
        /* If OTASP COMMIT was successful and user profile parameters have
           not been updated, then return here without freeing client req
           table entry. It will be freed when confirmation for OTASP is
           queued later. */
        return MMGSDI_SUCCESS;
      }
#else
      mmgsdi_status = MMGSDI_ERROR;
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    case MMGSDI_SESSION_OTASP_OTAPA_REQ:
      mmgsdi_status = mmgsdi_common_rsp_otasp_otapa(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_SESSION_OTASP_BLOCK_REQ:
      mmgsdi_status = mmgsdi_common_rsp_otasp_block(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp, protocol);
      break;

    case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
      mmgsdi_status = mmgsdi_gen_rsp_available_apps_req(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
      mmgsdi_status = MMGSDI_SUCCESS;
      break;

    case MMGSDI_SESSION_SUB_OK_REQ:
      mmgsdi_status = mmgsdi_process_sub_ok_rsp(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
      mmgsdi_status = mmgsdi_process_tp_dl_complete_response(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_RECOVERY_IND_REQ:
      mmgsdi_status = MMGSDI_SUCCESS;
      break;

    case MMGSDI_EONS_SS_EVENT_REQ:
      mmgsdi_status = mmgsdi_process_eons_ss_event_response(
                        &task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_HANDLE_NV_REFRESH_REQ:
      mmgsdi_status = mmgsdi_gen_rsp(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_MANAGE_CHANNEL_REQ:
      mmgsdi_status = mmgsdi_uicc_rsp_manage_channel(&task_cmd_ptr->cmd.cmd.uim_report_rsp);
      break;

    case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
      mmgsdi_status = MMGSDI_SUCCESS;
      break;

    case MMGSDI_MAX_CMD_ENUM:
      mmgsdi_status = mmgsdi_util_is_client_request_table_index_free(index, &free_index);
      if ((mmgsdi_status == MMGSDI_SUCCESS) && (free_index == TRUE))
      {
        UIM_MSG_ERR_0("Extra Rsp Message was built, drop the message");
        return mmgsdi_status;
      }
      mmgsdi_status = MMGSDI_ERROR;
      break;

    default:
      mmgsdi_status = MMGSDI_ERROR;
      break; /* end default case */
  } /* end switch - req type */

  if((mmgsdi_status != MMGSDI_SUCCESS) &&
     (cmd_req_enum != MMGSDI_TP_DOWNLOAD_COMPLETE_REQ))
  {
    UIM_MSG_ERR_2("Req 0x%x, Unable to build proper Cnf in mmgsdi_process_response, 0x%x",
                  cmd_req_enum, mmgsdi_status);

    mmgsdi_status = mmgsdi_build_and_queue_static_cnf(index, mmgsdi_status);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Impossible to return cnf back to client");
    }
  } /* end if - mmgsdi_status is not SUCCESS */

  /*----------------------------------------------------------------------------
    Free buffers: Elements in mmgsdi client req table
  -----------------------------------------------------------------------------*/
  mmgsdi_util_free_client_request_table_index(index);

  return mmgsdi_status;
} /* mmgsdi_process_response */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_SAP_RESPONSE

DESCRIPTION
  This function, called in gsdimain, is called to process an mmgsdi sap rsp.

DEPENDENCIES
  gsdimain must be finished initialization.

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
static mmgsdi_return_enum_type mmgsdi_process_sap_response(
  const mmgsdi_generic_rsp_type *  response_ptr /* GSDI response to be processed */
)
{
/*-----------------------------------------------------------------------------
  Initialize variables
-----------------------------------------------------------------------------*/
  mmgsdi_cmd_enum_type      cmd_req_enum  = MMGSDI_MAX_CMD_ENUM;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL(response_ptr);

  /* Obtain the index and check the request type */
  if( ( response_ptr->rsp_data_index < 0 ) ||
      ( response_ptr->rsp_data_index >= MMGSDI_MAX_CLIENT_REQ_INFO ) ||
      mmgsdi_client_req_table_info_ptr[response_ptr->rsp_data_index] == NULL)
  {
    UIM_MSG_ERR_1("invalid index 0x%x", response_ptr->rsp_data_index);
    return MMGSDI_ERROR;
  }

  /* If a confirmation has already been sent for this command as part of
     CARD_ERROR handling, free only the client_req_table index. */
  if(mmgsdi_client_req_table_info_ptr[response_ptr->rsp_data_index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED)
  {
    UIM_MSG_HIGH_2("Free client_req_table index 0x%x for cmd 0x%x",
                   response_ptr->rsp_data_index,
                   mmgsdi_client_req_table_info_ptr[response_ptr->rsp_data_index]->request_type);

    mmgsdi_util_free_client_request_table_index(response_ptr->rsp_data_index);
    return MMGSDI_SUCCESS;
  }

  cmd_req_enum = mmgsdi_client_req_table_info_ptr[response_ptr->rsp_data_index]->request_type;

  /*----------------------------------------------------------------------------
    Dispatch to proper technology/command response handling function
  -----------------------------------------------------------------------------*/
  switch(cmd_req_enum)
  {
    case MMGSDI_SAP_DISCONNECT_REQ:
      UIM_MSG_HIGH_1("Received SAP response from BT %x", cmd_req_enum);
      mmgsdi_status = mmgsdi_sap_rsp_build_cnf(response_ptr);
      break;
    default:
      UIM_MSG_HIGH_1("Unknown Response %x", cmd_req_enum);
      return MMGSDI_ERROR;

  } /* end switch - req type */

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("Req 0x%x, Unable to build proper Cnf in mmgsdi_process_sap_response, 0x%x",
                  cmd_req_enum, mmgsdi_status);

    UIM_MSG_ERR_0("Use static cnf buffer to build cnf notification");

    mmgsdi_status = mmgsdi_build_and_queue_static_cnf(response_ptr->rsp_data_index,
                                                      mmgsdi_status);

    if (mmgsdi_status != MMGSDI_SUCCESS)
      UIM_MSG_ERR_0("Impossible to return cnf back to client");

  } /* end if - mmgsdi_status is not SUCCESS */

  /*----------------------------------------------------------------------------
    Free buffers: Elements in mmgsdi client req table
  -----------------------------------------------------------------------------*/
  mmgsdi_util_free_client_request_table_index(response_ptr->rsp_data_index);

  return mmgsdi_status;
} /* mmgsdi_process_sap_response */


/*===========================================================================
FUNCTION MMGSDI_SET_CHANNEL_STATE_TO_READY

DESCRIPTION
  This function changes channel state to READY if a non provisioning session has
  been opened on the channel.

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_set_channel_state_to_ready(
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_session_id_type    session_id       = MMGSDI_INVALID_SESSION_ID;
  mmgsdi_channel_info_type *channel_info_ptr = NULL;
  mmgsdi_session_info_type *session_info_ptr = NULL;
  mmgsdi_return_enum_type   return_status    = MMGSDI_ERROR;

  /*return if cnf ptr is NULL*/
  if(cnf_ptr == NULL)
  {
    return;
  }

  /*get session id*/
  session_id = cnf_ptr->response_header.session_id;

  /*get channel and session info*/
  return_status = mmgsdi_util_get_session_and_channel_info(session_id,
                    &session_info_ptr, &channel_info_ptr);

  /*Set the channel state to READY if a non-prov session is opened successfully
    .Previous state should be NON_PROV_ACT_IN_PROG because if this channel is
     shared with other sessions and streaming is already done, it will set
     state back to READY from READY_WITH_STREAMING which is incorrect.*/

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  if((return_status == MMGSDI_SUCCESS) &&
     (channel_info_ptr != NULL) &&
     (session_info_ptr != NULL) &&
     mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) &&
     (channel_info_ptr->channel_state == MMGSDI_CHANNEL_STATE_NON_PROV_ACT_IN_PROG))
  {
    channel_info_ptr->channel_state = MMGSDI_CHANNEL_STATE_READY;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
}/* mmgsdi_set_channel_state_to_ready */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_CNF

DESCRIPTION
  This function, called in gsdimain, is called to process an mmgsdi
  confirmation.
  It will send out the confirmation to the user as necessary

DEPENDENCIES
  gsdimain must be finished initialization.

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
static mmgsdi_return_enum_type mmgsdi_process_cnf(
  mmgsdi_callback_cnf_type *  cnf_ptr
)
{

/*-----------------------------------------------------------------------------
  Initialize variables
-----------------------------------------------------------------------------*/
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_0 ("MMGSDI_PROCESS_CNF");

  if (cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Not Able to Allocate Confirmation Ptr in mmgsdi_process_cnf()");
    return MMGSDI_ERROR;
  }

  if(cnf_ptr->client_req_cb == NULL)
  {
    UIM_MSG_ERR_0("Client request response CB is Null in mmgsdi_process_cnf()");
    /*Free buffer elements in Confirmation ptr before returning ERROR*/
    mmgsdi_util_free_response_ptr(&cnf_ptr->mmgsdi_cnf);
    return MMGSDI_ERROR;
  }

  /* Send confirmation and check if event needs to be sent out or not */
  mmgsdi_cnf_generate_rsp_and_evt(cnf_ptr);

  /*check if cnf type is either SESSION_OPEN_CNF or SESSION_OPEN_WITH_MF_CNF
    or SESSION_OPEN_WITH_SELECT_WITH_RSP_CNF.If a non-prov. session is opened,
    then channel state needs to be updated to ready to indicate that app
    activation is complete*/
  if(mmgsdi_status == MMGSDI_SUCCESS &&
     (cnf_ptr->mmgsdi_cnf.response_header.response_type ==
        MMGSDI_SESSION_OPEN_CNF ||
      cnf_ptr->mmgsdi_cnf.response_header.response_type ==
        MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_CNF ||
      cnf_ptr->mmgsdi_cnf.response_header.response_type ==
        MMGSDI_SESSION_OPEN_WITH_MF_CNF))
  {
    /* Check if channel state is required to set to ready */
    mmgsdi_set_channel_state_to_ready(
      &cnf_ptr->mmgsdi_cnf);
  }

  /*----------------------------------------------------------------------------
    Free buffers: Elements in Confirmation ptr
  -----------------------------------------------------------------------------*/
  mmgsdi_util_free_response_ptr(&cnf_ptr->mmgsdi_cnf);

  return mmgsdi_status;
} /* mmgsdi_process_cnf */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_OFFLINE_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process an offline signal.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_handle_offline_sig
(
  void
)
{
  UIM_MSG_HIGH_0("MMGSDI: GOT TASK_OFFLINE_SIG");

  /* Store NITZ info on file system to be persistent across power cycle */
  if(MMGSDI_TASK_MAIN == mmgsdi_util_get_current_task())
  {
    (void)mmgsdi_eons_store_nitz_info();
  }

  /* Clear signal */
  (void) rex_clr_sigs(rex_self(), TASK_OFFLINE_SIG);
} /* mmgsdi_handle_offline_sig */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_STOP_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process an stop signal.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_handle_stop_sig
(
  void
)
{
  mmgsdi_task_enum_type  mmgsdi_task = MMGSDI_TASK_MAX;

   /* Clear signal */
  (void) rex_clr_sigs(rex_self(), MMGSDI_TASK_STOP_SIG);

  mmgsdi_task = mmgsdi_util_get_current_task();
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    UIM_MSG_ERR_0("TASK_STOP_SIG received in non-MMGSDI context");
    return;
  }

  UIM_MSG_HIGH_1("MMGSDI: GOT TASK_STOP_SIG IN TASK 0x%x", mmgsdi_task);

  /* Store NITZ info on file system to be persistent across power cycle */
  if(MMGSDI_TASK_MAIN == mmgsdi_task)
  {
    (void)mmgsdi_eons_store_nitz_info();

    /* To store UICC profile data in SFS for PSM mode*/
    mmgsdi_psm_store_data();
  }

  /* Don't process items in Queue */
  /* Sort of flushes the Queue    */
  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  mmgsdi_task_state[mmgsdi_task] = MMGSDI_TASK_STOPPED;
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /*-----------------------------------------------------------------------------
      Vote off the UIM
      1 Stop Polling the SIM/USIM in the case we are still in a Traffic Channel
    -----------------------------------------------------------------------------*/
    (void)uim_power_control_ds(UIM_SLOT_1,UIM_TC,FALSE, UIM_CALL_TYPE_NOT_APPLICABLE);
    (void)uim_power_control_ds(UIM_SLOT_2,UIM_TC,FALSE, UIM_CALL_TYPE_NOT_APPLICABLE);
    (void)uim_power_control_ds(UIM_SLOT_3,UIM_TC,FALSE, UIM_CALL_TYPE_NOT_APPLICABLE);

    /*-----------------------------------------------------------------------------
      Vote off the UIM
      2 Ensure the USIM Session is still not voted on.
    -----------------------------------------------------------------------------*/
    (void)uim_power_control_ds(UIM_SLOT_1,UIM_USIM_SESSION,FALSE, UIM_CALL_TYPE_NOT_APPLICABLE);
    (void)uim_power_control_ds(UIM_SLOT_2,UIM_USIM_SESSION,FALSE, UIM_CALL_TYPE_NOT_APPLICABLE);
    (void)uim_power_control_ds(UIM_SLOT_3,UIM_USIM_SESSION,FALSE, UIM_CALL_TYPE_NOT_APPLICABLE);
#endif /*!FEATURE_UIM_TEST_FRAMEWORK*/
} /* mmgsdi_handle_stop_sig */


/* ==========================================================================
FUNCTION MMGSDI_FLUSH_CURRENT_AWAIT_CMDS

DESCRIPTION
  This function flushes the commands of task waiting for UIM response
  in case of Timer Expiry.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  Void

SIDE EFFECTS
  NONE
==========================================================================*/
static void mmgsdi_flush_current_await_cmds
(
  mmgsdi_task_enum_type  mmgsdi_task
)
{
  uint8                   index         = 0;
  mmgsdi_task_cmd_type   *rsp_q_ptr     = NULL;
  boolean                 rsp_found     = FALSE;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  for(index = 0; index < MMGSDI_MAX_CLIENT_REQ_INFO; index++)
  {
    /* If rsp_status is already MMGSDI_UIM_RSP_NOT_RECEIVED, it means an error
       cnf is already sent for this. If rsp_status is MMGSDI_UIM_RSP_PROCESSING
       it means this is ongoing response handling. MMGSDI RSP handling will
       send the cnf */
    if(mmgsdi_client_req_table_info_ptr[index] == NULL ||
       mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED ||
       mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status == MMGSDI_UIM_RSP_PROCESSING)
    {
      continue;
    }

    /* Flush the command if timer expiry is received on task from which it is queued*/
    if(mmgsdi_client_req_table_info_ptr[index]->task_id != mmgsdi_task )
    {
      continue;
    }

    /* Check if the response corresponding to the index is already on response
       queue of the respective task */
    rsp_found = FALSE;
    rsp_q_ptr = (mmgsdi_task_cmd_type *)q_check(&mmgsdi_task_rsp_q[mmgsdi_task]);

    while(rsp_q_ptr != NULL)
    {
      if(rsp_q_ptr->cmd.cmd_enum == MMGSDI_UIM_REPORT_RSP &&
         rsp_q_ptr->cmd.cmd.uim_report_rsp.rsp_data_index == index)
      {
        /* Response is already queued by UIM. Nothing to do here. */
        mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status = MMGSDI_UIM_RSP_INBOUND;
        rsp_found = TRUE;
        break;
      }
      rsp_q_ptr = (mmgsdi_task_cmd_type *)q_next(&mmgsdi_task_rsp_q[mmgsdi_task], &rsp_q_ptr->link);
    }

    if(rsp_found)
    {
      continue;
    }

    /* No response is found in mmgsdi_task_rsp_q. Queue an error cnf */
    mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status = MMGSDI_UIM_RSP_NOT_RECEIVED;

    mmgsdi_status = mmgsdi_build_and_queue_static_cnf(index, MMGSDI_ERROR);

    UIM_MSG_HIGH_2("Queuing error cnf for not-responded-to cmd 0x%x, 0x%x",
                   mmgsdi_client_req_table_info_ptr[index]->request_type,
                   mmgsdi_status);
  }
} /* mmgsdi_flush_current_await_cmds */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_SANITY_TIMER_EXPIRE_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process sanity timer
  expiration signal.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_handle_sanity_timer_expire_sig
(
  void
)
{
  mmgsdi_task_enum_type   mmgsdi_task     = MMGSDI_TASK_MAX;
  mmgsdi_slot_data_type  *slot_data_ptr   = NULL;

  UIM_MSG_HIGH_0("MMGSDI: Got MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG");

  /* Clear the signal */
  (void) rex_clr_sigs(rex_self(), MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG);

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    UIM_MSG_ERR_0("SANITY_TIMER_EXPIRY_SIG received in non-MMGSDI context");
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr_for_task(mmgsdi_task);
  if(slot_data_ptr != NULL && slot_data_ptr->sim_busy)
  {
  /* Ignore sanity timer timeout if the card is busy and return without
     flushing commands. Eventually either the sim busy state will end and we
     will receive the responses, or drivers will send a card error/removed */
    UIM_MSG_HIGH_0("Ignoring sanity timer timeout due to sim busy state");
    return;
  }

  /* Flush all commands queued from this task and waiting responses */
  (void) mmgsdi_flush_current_await_cmds(mmgsdi_task);
} /* mmgsdi_handle_sanity_timer_expire_sig */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_REFRESH_QUEUE_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process a signals of
  refresh queue.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_handle_refresh_queue_sig
(
  void
)
{
  mmgsdi_task_cmd_type    * mmgsdi_task_ptr  = NULL;
  mmgsdi_task_enum_type     mmgsdi_task      = MMGSDI_TASK_MAX;

  (void) rex_clr_sigs(rex_self(), MMGSDI_REFRESH_CMD_Q_SIG);

  /* Get the current MMGSDI task */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if (mmgsdi_task != MMGSDI_TASK_MAIN)
  {
    UIM_MSG_ERR_1("Current not processing MMGSDI main task: 0x%x", mmgsdi_task);
    return;
  }

  while (q_cnt(&mmgsdi_refresh_cmd_q) != 0)
  {
    mmgsdi_task_ptr = (mmgsdi_task_cmd_type*)(q_get(&mmgsdi_refresh_cmd_q));

    (void)mmgsdi_cmd_ext(mmgsdi_task_ptr, MMGSDI_TASK_MAIN);
  }
} /* mmgsdi_handle_refresh_queue_sig */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_SIMLOCK_QUEUE_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process a signals of
  the simlock queue.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
#ifdef FEATURE_SIMLOCK
static void mmgsdi_handle_simlock_queue_sig
(
  void
)
{
  mmgsdi_task_cmd_type    * mmgsdi_task_ptr  = NULL;
  mmgsdi_task_enum_type     mmgsdi_task      = MMGSDI_TASK_MAX;

  (void) rex_clr_sigs(rex_self(), MMGSDI_SIMLOCK_MSG_SIG);

  /* Get the current MMGSDI task */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if (mmgsdi_task != MMGSDI_TASK_MAIN)
  {
    return;
  }

  while (q_cnt(&mmgsdi_simlock_rsp_q) != 0)
  {
    mmgsdi_task_ptr = (mmgsdi_task_cmd_type*)(q_get(&mmgsdi_simlock_rsp_q));

    if (mmgsdi_task_ptr == NULL)
    {
      continue;
    }

    switch (mmgsdi_task_ptr->cmd.cmd_enum)
    {
      case MMGSDI_SIMLOCK_RSP:
        mmgsdi_simlock_process_response(&mmgsdi_task_ptr->cmd.cmd.simlock_rsp);
        break;

      case MMGSDI_SIMLOCK_EVT:
        mmgsdi_simlock_process_event(&mmgsdi_task_ptr->cmd.cmd.simlock_evt);
        break;

      default:
        UIM_MSG_ERR_0("Invalid SIM Lock response queue command");
        return;
    }

    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_task_ptr);
  }
} /* mmgsdi_handle_simlock_queue_sig */
#endif /* FEATURE_SIMLOCK */


/*===========================================================================
FUNCTION MMGSDI_IS_TP_DOWNLOAD_REQ_IN_QUEUE

DESCRIPTION
  This function is called by q_linear_delete_new. This function will return TRUE only when the
  command id is that of a TP_DOWNLOAD_REQ.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  ptr to param

DEPENDENCIES
  None

RETURN VALUE
  1 - if there is a match
  0 - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static int mmgsdi_is_tp_download_req_in_queue(
  void *item_ptr,
  void *param_ptr
)
{
  mmgsdi_task_cmd_type      *task_cmd_ptr   = NULL;

  (void)param_ptr;

  if(item_ptr == NULL)
  {
    return 0;
  }

  task_cmd_ptr = (mmgsdi_task_cmd_type*)item_ptr;

  if(task_cmd_ptr->cmd.cmd_enum == MMGSDI_TP_DOWNLOAD_COMPLETE_REQ)
  {
    UIM_MSG_HIGH_0("MMGSDI_TP_DOWNLOAD_COMPLETE_REQ found in queue");
    return 1;
  }

  return 0;
} /* mmgsdi_is_tp_download_req_in_queue */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_QUEUE_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process a signals of
  command and response queue.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  If a command is being processed by mmgsdi, the task would no longer
  wait for MMGSDI_TASK_CMD_Q_SIG.
===========================================================================*/
static void mmgsdi_handle_queue_sig
(
  rex_sigs_type sigs_rcvd,
  rex_sigs_type *waiting_for_sigs
)
{
  mmgsdi_task_cmd_type *    mmgsdi_task_ptr  = NULL;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_task_enum_type     mmgsdi_task      = MMGSDI_TASK_MAX;
  boolean                   block_processing_for_psm = FALSE;

  /* Get the current MMGSDI task */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if (mmgsdi_task == MMGSDI_TASK_MAX)
  {
    UIM_MSG_ERR_0("Unable to determine the current MMGSDI task");
    return;
  }

  if((mmgsdi_task == MMGSDI_TASK_SLOT1) && mmgsdi_psm_get_status_early_init())
  {
    block_processing_for_psm = TRUE;
  }

  if(sigs_rcvd & MMGSDI_TASK_RSP_Q_SIG)
  {
    /* Process Responses first, before processing a new MMGSDI command */
    if ((mmgsdi_task_ptr =
        ((mmgsdi_task_cmd_type*)(q_get(&mmgsdi_task_rsp_q[mmgsdi_task])))) != NULL)
    {
      switch (mmgsdi_task_ptr->cmd.cmd_enum)
      {
        case MMGSDI_UIM_REPORT_RSP:
        case MMGSDI_CLIENT_ID_REG_RSP:
          mmgsdi_status = mmgsdi_process_response(mmgsdi_task_ptr);

          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
            UIM_MSG_HIGH_1("Error processing mmgsdi response in mmgsdi_task, 0x%x",
                           mmgsdi_task);
          }/* end if !MMGSDI_SUCCESS */
          break; /* end case MMGSDI_UIM_REPORT_RSP, MMGSDI_CLIENT_ID_REG_RSP */

        case MMGSDI_SAP_REPORT_RSP:
          mmgsdi_status = mmgsdi_process_sap_response(&mmgsdi_task_ptr->cmd.cmd.sap_report_rsp);

          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
            UIM_MSG_HIGH_1("Error processing mmgsdi sap response in mmgsdi_task, 0x%x",
                           mmgsdi_task);
          }/* end if !MMGSDI_SUCCESS */
          break;/* end case MMGSDI_SAP_REPORT_RSP */

        default:
          if(mmgsdi_task_ptr->cmd.cmd.mmgsdi_cnf_ptr != NULL)
          {
            mmgsdi_status = mmgsdi_process_cnf(mmgsdi_task_ptr->cmd.cmd.mmgsdi_cnf_ptr);
          }

          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
            UIM_MSG_HIGH_1("Error processing mmgsdi response in mmgsdi_task, 0x%x",
                           mmgsdi_task);
          }/* end if !MMGSDI_SUCCESS */
          break; /* end case default */
      }

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_task_ptr);
    } /* end if mmgsdi_task_rsp_q is not empty */

    (void) rex_clr_sigs(rex_self(), MMGSDI_TASK_RSP_Q_SIG);

    /* Set the signal if there are one or more responses to look at */
    if (q_cnt(&mmgsdi_task_rsp_q[mmgsdi_task]) > 0)
    {
      (void) rex_set_sigs(rex_self(), MMGSDI_TASK_RSP_Q_SIG);
    }
  } /* end if MMGSDI_TASK_RSP_Q_SIG */
  else if (sigs_rcvd & MMGSDI_CARD_STATUS_CMD_Q_SIG)
  {
    mmgsdi_task_ptr = (mmgsdi_task_cmd_type*)(q_get(&mmgsdi_card_status_cmd_q[mmgsdi_task]));

    if (mmgsdi_task_ptr != NULL)
    {
      UIM_MSG_HIGH_1("MMGSDI_CARD_STATUS_CMD_Q_SIG received in task 0x%x",
                     mmgsdi_task);

      mmgsdi_status = mmgsdi_process_command(mmgsdi_task_ptr);

      if( (mmgsdi_status != MMGSDI_SUCCESS) &&
          (mmgsdi_status != MMGSDI_REFRESH_IN_PROGRESS) &&
          (mmgsdi_status != MMGSDI_CMD_QUEUED_TO_SLOT_TASK) )
      {
        UIM_MSG_HIGH_1("Error 0x%x processing MMGSDI command; No Response will be sent back to Client",
                   mmgsdi_status);
      }/* end if !MMGSDI_SUCCESS */

      /* Do not free the task cmd ptr, if the cmd is re-queued to
         slot specific task */
      if(mmgsdi_status != MMGSDI_CMD_QUEUED_TO_SLOT_TASK)
      {
        MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_task_ptr);
      }
    }

    (void) rex_clr_sigs(rex_self(), MMGSDI_CARD_STATUS_CMD_Q_SIG);

    /* Set the signal if there are one or more commands to look at */
    if (q_cnt(&mmgsdi_card_status_cmd_q[mmgsdi_task]) > 0 )
    {
      /*  Items on the queue. Set the signal */
      (void) rex_set_sigs(rex_self(), MMGSDI_CARD_STATUS_CMD_Q_SIG);
    }
  } /* end if MMGSDI_CARD_STATUS_CMD_Q_SIG */
  else if((sigs_rcvd & MMGSDI_TASK_CMD_Q_SIG) && !mmgsdi_uim_cmd[mmgsdi_task].cmd_in_progress)
  {
    /* Do not process high pri queue if PSM init is in progress and card is not up - applicable to slot1 only*/
    if (block_processing_for_psm == FALSE)
    {
      /* Check the high priority queue first */
      mmgsdi_task_ptr = (mmgsdi_task_cmd_type*)(q_get(&mmgsdi_high_pri_cmd_q[mmgsdi_task]));
    }

    /* If no commands are present on the high priority queue,
       check the internal queue */
    if(mmgsdi_task_ptr == NULL)
    {
      /* Do not process internal queue if PSM init is in progress and card is not up - applicable to slot1 only*/
      if (block_processing_for_psm == FALSE)
      {
        mmgsdi_task_ptr = (mmgsdi_task_cmd_type*)(q_get(&mmgsdi_internal_cmd_q[mmgsdi_task]));
      }

      /* If no commands are present on the internal queue,
         check the regular queue */
      if(mmgsdi_task_ptr == NULL)
      {
        /* Do not process any other command from the queue
           except TP DL response if PSM init is in progress and real card is not up - applicable to slot1 only
           Search TP DL in task command queue. q_linear_delete_new returns NULL
           if command is not present in queue */
        if (block_processing_for_psm)
        {
          mmgsdi_task_ptr = (mmgsdi_task_cmd_type*) q_linear_delete_new (
                              &mmgsdi_task_cmd_q[mmgsdi_task],
                              mmgsdi_is_tp_download_req_in_queue,
                              NULL,
                              NULL,
                              NULL);
        }
        else
        {
          mmgsdi_task_ptr = (mmgsdi_task_cmd_type*)(q_get(&mmgsdi_task_cmd_q[mmgsdi_task]));
        }
      }
    }

    /* If there is a command on either of the queues */
    if(mmgsdi_task_ptr != NULL)
    {
      mmgsdi_status = mmgsdi_process_command(mmgsdi_task_ptr);

      if( (mmgsdi_status != MMGSDI_SUCCESS) &&
          (mmgsdi_status != MMGSDI_REFRESH_IN_PROGRESS) &&
          (mmgsdi_status != MMGSDI_CMD_QUEUED_TO_SLOT_TASK) )
      {
        UIM_MSG_HIGH_1("Error 0x%x processing MMGSDI command; No Response will be sent back to Client",
                       mmgsdi_status);
      }/* end if !MMGSDI_SUCCESS */

      /* Do not free the task cmd ptr, if the cmd is re-queued to
         slot specific task */
      if(mmgsdi_status != MMGSDI_CMD_QUEUED_TO_SLOT_TASK)
      {
        MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_task_ptr);
      }
    }

    (void) rex_clr_sigs(rex_self(), MMGSDI_TASK_CMD_Q_SIG);

    /* Set the signal if there are one or more commands to look at */
    if (((q_cnt(&mmgsdi_high_pri_cmd_q[mmgsdi_task]) > 0) ||
        (q_cnt(&mmgsdi_internal_cmd_q[mmgsdi_task]) > 0) ||
        (q_cnt(&mmgsdi_task_cmd_q[mmgsdi_task]) > 0)) &&
         (block_processing_for_psm == FALSE))
    {
      /*  Items on the queue. Set the signal */
      (void) rex_set_sigs(rex_self(), MMGSDI_TASK_CMD_Q_SIG);
    }
  } /* end if MMGSDI_TASK_CMD_Q_SIG and ! mmgsdi_uim_cmd[mmgsdi_task].cmd_in_progress */

  /* If a command has been queued to UIM, then do not handle new
     card bound commands till the card response is received.
     Remove MMGSDI_TASK_CMD_Q_SIG from the wait signal mask */
  if(mmgsdi_uim_cmd[mmgsdi_task].cmd_in_progress)
  {
    *waiting_for_sigs &= ~(MMGSDI_TASK_CMD_Q_SIG);
  }
  else
  {
    *waiting_for_sigs |= (MMGSDI_TASK_CMD_Q_SIG) ;
  }
} /* mmgsdi_handle_queue_sig */


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
)
{
  /* Setting global variable to indicate that QMI UIM signal is being
     processed to simulate as if it is executed in a different context
     than MMGSDI */
  mmgsdi_process_qmi_uim_sig = TRUE;

  /* Even though only the main gsdi task is expected to receive this signal,
     just for a more robust design, we are using rex_self() here instead of
     UIM_GSDI_TCB while clearing the sig so that if accidently the signal is
     set on tasks other than the main task (maybe because of a sig clash),
     we should be able to clear it in order to avoid infinite sig-processing
     loops */
  (void)rex_clr_sigs(rex_self(), MMGSDI_QMI_UIM_SIG);
  if(qmi_uim_sig_handler() == FALSE)
  {
    /* If QMI UIM command queue is not empty, reset signal to
       continue processing commands */
    (void)rex_set_sigs(UIM_GSDI_TCB, MMGSDI_QMI_UIM_SIG);
  }
  mmgsdi_process_qmi_uim_sig = FALSE;
} /* mmgsdi_handle_qmi_uim_sig */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_NV_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process the signal
  from NV.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_handle_nv_sig
(
  void
)
{
  nv_cmd_type              *gsdi_nv_cmd_ptr  = NULL;

  if(q_cnt(&mmgsdi_nv_q) > 0 &&
    ((gsdi_nv_cmd_ptr = ((nv_cmd_type*)(q_get(&mmgsdi_nv_q)))) != NULL))
  {
    if(gsdi_nv_cmd_ptr->status != NV_DONE_S)
    {
       UIM_MSG_ERR_2("NV Write Failed Item %d Code 0x%x",
                     gsdi_nv_cmd_ptr->item, gsdi_nv_cmd_ptr->status);
    }
    else
    {
      UIM_MSG_HIGH_1("NV ITEM: %d PUT SUCCESS", gsdi_nv_cmd_ptr->item);
    }
    MMGSDIUTIL_TMC_MEM_FREE(gsdi_nv_cmd_ptr);
  }
  else
  {
    UIM_MSG_ERR_0("MMGSDI_NV_WRITE_SIG set with no command in the mmgsdi_nv_q");
  }

  /* Even though NV writes are handled in the main MMGSDI task and
     so only main task would receive this signal, just for a more robust
     design, we are using rex_self() here instead of UIM_GSDI_TCB so that
     if accidently the signal is set on tasks other than the main task
     (maybe because of a sig clash), we should be able to clear it in order
     to avoid infinite sig-processing loops */
  (void)rex_clr_sigs(rex_self(), MMGSDI_NV_WRITE_SIG);
  if(q_cnt(&mmgsdi_nv_q) > 0)
  {
    (void)rex_set_sigs(UIM_GSDI_TCB, MMGSDI_NV_WRITE_SIG);
  }
}/* mmgsdi_handle_nv_sig */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_QCSI_QMI_SIM_INTERNAL_SIG

DESCRIPTION
  This function, called in mmgsdi_task, is called to process a qcsi qmi sim sig.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
static void mmgsdi_handle_qcsi_qmi_sim_internal_sig
(
  void
)
{
  /* Even though only the main gsdi task is registered for this signal and
     so only main task would receive this signal, just for a more robust
     design, we are using rex_self() here instead of UIM_GSDI_TCB so that
     if accidently the signal is set on tasks other than the main task
     (maybe because of a sig clash), we should be able to clear it in order
     to avoid infinite sig-processing loops */
  (void) rex_clr_sigs(rex_self(), (rex_sigs_type)MMGSDI_QCSI_QMI_SIM_INTERNAL_SIG);
  UIM_MSG_HIGH_0("MMGSDI: GOT MMGSDI_QCSI_QMI_SIM_INTERNAL_SIG : cleared");

  qmi_sim_internal_handle_event(UIM_GSDI_TCB,
                                (rex_sigs_type)MMGSDI_QCSI_QMI_SIM_INTERNAL_SIG);
} /* mmgsdi_handle_qcsi_qmi_sim_internal_sig */
#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE */


/*===========================================================================
FUNCTION MMGSDI_HANDLE_OBDAPP_SIG

DESCRIPTION
  This function processes the OBDAPP sig.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void mmgsdi_handle_obdapp_sig
(
  void
)
{
  /* Even though only the main gsdi task is registered for this signal and
     so only main task would receive this signal, just for a more robust
     design, we are using rex_self() here instead of UIM_GSDI_TCB so that
     if accidently the signal is set on tasks other than the main task
     (maybe because of a sig clash), we should be able to clear it in order
     to avoid infinite sig-processing loops */
  (void) rex_clr_sigs(rex_self(), (rex_sigs_type)MMGSDI_OBDAPP_SIG);

  UIM_MSG_HIGH_0("MMGSDI: Got MMGSDI_OBDAPP_SIG");
#ifndef  FEATURE_UIM_TEST_FRAMEWORK
  obd_handle_new_state();
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
} /* mmgsdi_handle_obdapp_sig */


/*================================================================
FUNCTION  MMGSDI_FLUSH_QUEUES

DESCRIPTION:
  This function cancels pending commands using CLIENT_ID

INPUT PARAMETERS:
  None

DEPENDENCIES:
  All spawned GSDI tasks including MAIN task should have ended

LIMITATIONS:
  None

RETURN VALUE:
  void

SIDE EFFECTS:
  None
===============================================================*/
static void mmgsdi_flush_queues
(
  void
)
{
  uint8 client_index  = 0;

  UIM_MSG_LOW_0("Cancel pending commands");

  for (client_index = 0;
       client_index < sizeof(mmgsdi_client_id_reg_table)/
                      sizeof(mmgsdi_client_id_reg_table[0]);
       client_index++)
  {
    if(mmgsdi_client_id_reg_table[client_index])
    {
      /* Cancel pending commands using CLIENT_ID */
      mmgsdi_cancel_requests(mmgsdi_client_id_reg_table[client_index]->client_id);
    }
  }
} /* mmgsdi_flush_queues */


/*================================================================
FUNCTION  MMGSDI_DELETE_TIMERS

DESCRIPTION:
  This function is called to delete timers owned by GSDI task
  after each task ends

INPUT PARAMETERS:
  mmgsdi_task

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
static void mmgsdi_delete_timers
(
  mmgsdi_task_enum_type mmgsdi_task
)
{
  uint8                       slot_index              = MMGSDI_SLOT_1_INDEX;
  rex_tcb_type               *mmgsdi_current_tcb      = NULL;
  timer_attrib_struct_type    timer_attrib_struct_data;
  mmgsdi_slot_id_enum_type    slot_id                 = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_task_enum_type       task_index              = MMGSDI_TASK_MAX;
  mmgsdi_return_enum_type     mmgsdi_status           = MMGSDI_SUCCESS;

  /* Get the current TCB before clearing any timers */
  mmgsdi_current_tcb = rex_self();

  /* Clear timers owned by tasks */
  (void)rex_undef_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task]);
  (void)rex_undef_timer(&mmgsdi_timed_sleep_timer[mmgsdi_task]);

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

  /* Clear slot dependent timers held by current TCB */
  for(slot_index = MMGSDI_SLOT_1_INDEX; slot_index < MMGSDI_MAX_NUM_SLOTS; slot_index++)
  {
    (void)mmgsdi_util_get_slot_id (slot_index, &slot_id);

    task_index = mmgsdi_util_get_task_from_slot(slot_id);
    if (task_index < MMGSDI_TASK_SLOT1 || task_index >= MMGSDI_NUM_TASKS)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if (mmgsdi_task_state[task_index] == MMGSDI_TASK_INACTIVE)
    {
      continue;
    }

    timer_get_params(&mmgsdi_refresh_timer[slot_index], &timer_attrib_struct_data);
    if(timer_attrib_struct_data.tcb_sigs_obj != NULL &&
       timer_attrib_struct_data.tcb_sigs_obj == mmgsdi_current_tcb)
    {
      (void)rex_undef_timer(&mmgsdi_refresh_timer[slot_index]);
    }

    timer_get_params(&mmgsdi_refresh_auto_trigger_timer[slot_index], &timer_attrib_struct_data);
    if(timer_attrib_struct_data.tcb_sigs_obj != NULL &&
       timer_attrib_struct_data.tcb_sigs_obj == mmgsdi_current_tcb)
    {
      (void)rex_undef_timer(&mmgsdi_refresh_auto_trigger_timer[slot_index]);
    }
  }

  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  timer_get_params(&mmgsdi_refresh_retry_timer, &timer_attrib_struct_data);
  if(timer_attrib_struct_data.tcb_sigs_obj != NULL &&
     timer_attrib_struct_data.tcb_sigs_obj == mmgsdi_current_tcb)
  {
    (void)rex_undef_timer(&mmgsdi_refresh_retry_timer);
  }
} /* mmgsdi_delete_timers */


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*================================================================
FUNCTION  MMGSDI_TASK_ENDED

DESCRIPTION:
  This function is called to cleanup after task is ended

INPUT PARAMETERS:
  None

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
static void mmgsdi_task_ended
(
  void
)
{
  mmgsdi_task_enum_type     running_task            = MMGSDI_TASK_MAX;
  uint8                     index                   = 0;
  boolean                   mmgsdi_all_tasks_ended  = TRUE;

  /* Do not proceed, if the current task is undetermined */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&running_task);


  if(running_task >= MMGSDI_TASK_MAX)
  {
    return;
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

  /* Set the task state to ended before taking any action*/
  mmgsdi_task_state[running_task] = MMGSDI_TASK_ENDED;

  /* Check if all spawned GSDI tasks ended */
  for (index = 0; index < MMGSDI_NUM_TASKS; index++)
  {
    if(mmgsdi_task_state[index] == MMGSDI_TASK_ACTIVE ||
       mmgsdi_task_state[index] == MMGSDI_TASK_STOPPED)
    {
      mmgsdi_all_tasks_ended = FALSE;
      break;
    }
  }
  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  /* Deregister dog if leaving the task so that we don't ERR_FATAL */
  dog_hb_deregister(gsdi_dog_rpt_var[running_task]);

  /* Clear timers for ended GSDI task */
  mmgsdi_delete_timers(running_task);

  /* Clean up if all spawned GSDI tasks ended */
  if(mmgsdi_all_tasks_ended)
  {
    /* Cancel pending commands */
    mmgsdi_flush_queues();

#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  }
} /* mmgsdi_task_ended */


/*===========================================================================
  FUNCTION: mmgsdi_cm_call_evt_cb

  DESCRIPTION:
    Event callback handler for cm call events.

  PARAMETERS:
    call_event      -> cm call event type
    call_info_ptr   -> more information regarding the call event
===========================================================================*/
void mmgsdi_cm_call_evt_cb
(
  cm_call_event_e_type           call_event,
  const cm_mm_call_info_s_type * call_info_ptr
)
{
  boolean                       call_active            = FALSE;
  mmgsdi_session_type_enum_type mmgsdi_gw_session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_session_type_enum_type mmgsdi_1x_session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type       mmgsdi_status          = MMGSDI_SUCCESS;
  uint8                         mmgsdi_gw_prov_index   = MMGSDI_MAX_PROV_APPS;
  uint8                         mmgsdi_1x_prov_index   = MMGSDI_MAX_PROV_APPS;

  if (call_info_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_3("mmgsdi_cm_call_evt_cb, call_event:0x%x, call_type:0x%x, call_state:0x%x",
                 call_event, call_info_ptr->call_type, call_info_ptr->call_state);

  mmgsdi_status = mmgsdi_util_get_session_type_from_as_id(
                    call_info_ptr->asubs_id,
                    &mmgsdi_gw_session_type,
                    &mmgsdi_1x_session_type);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid AS ID: 0x%x", call_info_ptr->asubs_id);
    return;
  }

  /* Derive GW & 1x prov index to update emergency call status */
  mmgsdi_gw_prov_index = mmgsdi_util_get_prov_index_from_session(mmgsdi_gw_session_type);
  if(mmgsdi_gw_prov_index == MMGSDI_MAX_PROV_APPS)
  {
    return;
  }

  mmgsdi_1x_prov_index = mmgsdi_util_get_prov_index_from_session(mmgsdi_1x_session_type);
  if(mmgsdi_1x_prov_index == MMGSDI_MAX_PROV_APPS)
  {
    return;
  }

  if (CM_CALL_STATE_ORIG           == call_info_ptr->call_state ||
      CM_CALL_STATE_INCOM          == call_info_ptr->call_state ||
      CM_CALL_STATE_CONV           == call_info_ptr->call_state ||
      CM_CALL_STATE_CC_IN_PROGRESS == call_info_ptr->call_state)
  {
    call_active = TRUE;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* Update the call state based on the number of active calls.
     We are interested in only voice & data call types */
  switch (call_info_ptr->call_type)
  {
    case CM_CALL_TYPE_VOICE:
      mmgsdi_generic_data_ptr->voice_call_active = call_active;
      break;

    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_PS_DATA:
      mmgsdi_generic_data_ptr->data_call_active = call_active;
      break;

    case CM_CALL_TYPE_EMERGENCY:
      if(call_info_ptr->call_state == CM_CALL_STATE_ORIG)
      {
        mmgsdi_generic_data_ptr->emergency_call_active[mmgsdi_gw_prov_index] = TRUE;
        mmgsdi_generic_data_ptr->emergency_call_active[mmgsdi_1x_prov_index] = TRUE;
      }
      else if(call_info_ptr->call_state == CM_CALL_STATE_IDLE)
      {
        mmgsdi_generic_data_ptr->emergency_call_active[mmgsdi_gw_prov_index] = FALSE;
        mmgsdi_generic_data_ptr->emergency_call_active[mmgsdi_1x_prov_index] = FALSE;
      }
      break;

    default:
      /* Not interested */
      break;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

} /* mmgsdi_cm_call_evt_cb */


/*===========================================================================
  FUNCTION: mmgsdi_cm_ph_event_cb

  DESCRIPTION:
    Event callback handler for cm ph events.

  PARAMETERS:
    ph_event      -> cm ph event type
    ph_info_ptr   -> more information regarding the cm ph event

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
===========================================================================*/
static void mmgsdi_cm_ph_event_cb
(
  cm_ph_event_e_type          ph_event,
  const cm_ph_info_s_type    *ph_info_ptr
)
{
  if (ph_info_ptr == NULL || mmgsdi_generic_data_ptr == NULL)
  {
    return;
  }

  UIM_MSG_LOW_2("mmgsdi_cm_ph_event_cb, ph_event:0x%x, oprt_mode:0x%x",
                 ph_event, ph_info_ptr->oprt_mode);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  switch (ph_event)
  {
    case CM_PH_EVENT_SYS_SEL_PREF:
      if(ph_info_ptr->ph_state == CM_PH_STATE_EMERG_CB)
      {
        mmgsdi_generic_data_ptr->ecbm_mode_active = TRUE;
      }
      else
      {
        mmgsdi_generic_data_ptr->ecbm_mode_active = FALSE;
      }
      break;

    default:
      /* Not interested */
      break;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
} /* mmgsdi_cm_ph_event_cb */


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
void mmgsdi_cm_init (void)
{
  mmgsdi_generic_data_ptr->mmgsdi_cm_id = 0;

  /*---------------------------------------------------------------------------
    Register with CM for all Call Events.
    -------------------------------------------------------------------------*/

  if (cm_client_init(CM_CLIENT_TYPE_GSDI,
                     &mmgsdi_generic_data_ptr->mmgsdi_cm_id) != CM_CLIENT_OK ||
                     !mmgsdi_generic_data_ptr->mmgsdi_cm_id)
  {
    UIM_MSG_ERR_0("Unable to Register with CM");
  }
  else
  {
    if(cm_client_act(mmgsdi_generic_data_ptr->mmgsdi_cm_id) != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_0("Unable to Activate Call Manager");
    }
    /* Register callbacks */
    (void) cm_mm_client_call_reg(
             mmgsdi_generic_data_ptr->mmgsdi_cm_id,
             mmgsdi_cm_call_evt_cb,
             CM_CLIENT_EVENT_REG,
             CM_CALL_EVENT_ORIG,
             CM_CALL_EVENT_CONNECT,
             NULL);

    (void) cm_mm_client_ss_reg_msim(mmgsdi_generic_data_ptr->mmgsdi_cm_id,
                                    mmgsdi_cm_ss_event_cb,
                                    CM_CLIENT_EVENT_REG,
                                    CM_SS_EVENT_SRV_CHANGED,
                                    CM_SS_EVENT_SRV_CHANGED,
                                    NULL,
                                    SYS_MODEM_AS_ID_MASK_ANY);

    (void) cm_client_ph_reg(mmgsdi_generic_data_ptr->mmgsdi_cm_id,
                            mmgsdi_cm_ph_event_cb,
                            CM_CLIENT_EVENT_REG,
                            CM_PH_EVENT_OPRT_MODE,
                            CM_PH_EVENT_SYS_SEL_PREF,
                            NULL);
  }
} /*mmgsdi_cm_init*/


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
  mmgsdi_client_name_type      client_name)
{
  int                     i                   = 0;
  uint64                  client_id           = 0;

  MMGSDIUTIL_RETURN_IF_NULL(client_id_out_ptr);

  for (i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if (mmgsdi_client_id_reg_table[i] == NULL)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_client_id_reg_table[i],
        sizeof(mmgsdi_client_id_reg_table_type));

      if (mmgsdi_client_id_reg_table[i] == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      /* Use the complement of the client_id_reg_table entry address for
         client id. The most significant 32 bits are used here to maintain
         legacy behavior. */
      client_id = ~(uint32)mmgsdi_client_id_reg_table[i];
      client_id = client_id << MMGSDI_CLIENT_ID_SHIFT;
      mmgsdi_client_id_reg_table[i]->client_id = client_id;

      strlcpy(mmgsdi_client_id_reg_table[i]->client_name, client_name, MMGSDI_TASK_NAME_LEN);
      *client_id_out_ptr = mmgsdi_client_id_reg_table[i]->client_id;
      mmgsdi_client_id_reg_table[i]->cmd_rsp_cb_ptr = cmd_rsp_cb_ptr;
      mmgsdi_client_id_reg_table[i]->is_client_evt_reg_cnf_sent = FALSE;
      return MMGSDI_SUCCESS;
    }
  }
  UIM_MSG_ERR_0("Entry could not be added in client id reg table");
  return MMGSDI_ERROR;
} /* mmgsdi_add_client_id_entry */


#ifndef FEATURE_UIM_TEST_FRAMEWORK
/*===========================================================================
FUNCTION MMGSDI_HANDLE_CM_TASK_READY

DESCRIPTION
  Function called to handle the task-ready notification from RCEVT for
  CM task. This notification is received when CM task calls
  rcevt_signal_name(). This function should only be called once during
  task initialization.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_handle_cm_task_ready
(
  void
)
{
  UIM_MSG_HIGH_0("CM task is up...");

  /* Unregister from RCEVT for CM-task-ready notification
     and clear the corresponding signal. */
  (void)rcevt_unregister_sigex_name("cm:ready",
                                    RCEVT_SIGEX_TYPE_SIGREX,
                                    &mmgsdi_rcevt_siginfo_cm);

  /* Even though only the main gsdi task is registered for this signal and
     so only main task would receive this signal, just for a more robust
     design, we are using rex_self() here instead of UIM_GSDI_TCB so that
     if accidently the signal is set on tasks other than the main task
     (maybe because of a sig clash), we should be able to clear it in order
     to avoid infinite sig-processing loops */
  rex_clr_sigs(rex_self(), MMGSDI_RCEVT_CM_TASK_READY_SIG);

  /* Make sure we don't re-do the initialization incase the signal is
     erroneously received more than once */
  if(!(mmgsdi_generic_data_ptr->ready_tasks & (0x00000001 << (uint8) MMGSDI_CM_TYPE)))
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /* Register with CM */
    mmgsdi_cm_init();
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

    /* Mark that the CM task is ready. mmgsdi_init_rtre_config should be
       called after marking CM ready */
    mmgsdi_generic_data_ptr->ready_tasks |= (0x00000001 << (uint8)MMGSDI_CM_TYPE);

    mmgsdi_init_rtre_config();
  }
} /* mmgsdi_handle_cm_task_ready */


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
)
{
  UIM_MSG_HIGH_0("QMI modem task is up...");

  /* Unregister from RCEVT for QMIMODEM-task-ready notification
     and clear the corresponding signal. */
  (void)rcevt_unregister_sigex_name("QMI_MODEM: READY",
                                    RCEVT_SIGEX_TYPE_SIGREX,
                                    &mmgsdi_rcevt_siginfo_qmi_modem);

  /* Even though only the main gsdi task is registered for this signal and
     so only main task would receive this signal, just for a more robust
     design, we are using rex_self() here instead of UIM_GSDI_TCB so that
     if accidently the signal is set on tasks other than the main task
     (maybe because of a sig clash), we should be able to clear it in order
     to avoid infinite sig-processing loops */
  rex_clr_sigs(rex_self(), MMGSDI_RCEVT_QMI_MODEM_TASK_READY_SIG);

  /* Make sure we don't re-do the initialization incase the signal is
     erroneously received more than once */
  if(!(mmgsdi_generic_data_ptr->ready_tasks & (0x00000001 << (uint8) MMGSDI_QMI_MODEM_TYPE)))
  {
    /* Initialize the QMI_UIM service with the QMI framework */
    qmi_uim_init_post_startup();

    /* ------------------------------------------------------------------------
    ** QMI sim_internal service init
    ** ----------------------------------------------------------------------*/
#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
    if(!(qmi_sim_internal_init(UIM_GSDI_TCB,
                               (rex_sigs_type)MMGSDI_QCSI_QMI_SIM_INTERNAL_SIG)))
    {
      UIM_MSG_ERR_0("Fail to init QMI_SIM_INTERNAL service");
    }
#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE*/

    /* Mark that the QMI MODEM task is ready */
    mmgsdi_generic_data_ptr->ready_tasks |= (0x00000001 << (uint8)MMGSDI_QMI_MODEM_TYPE);
  }
} /* mmgsdi_handle_qmi_modem_task_ready */
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================
FUNCTION GSDIMAIN

DESCRIPTION
  Entry point for the MMGSDI task.  This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gsdimain (
  dword dummy
)
{
  /* List of sigs all the gsdi tasks will wait for. Later on if we figure out
     out that this function is intered for gsdi main task, we will add some
     gsdi main task specific sigs to the wait sigs list as well */
  rex_sigs_type               sigs         = MMGSDI_TASK_RSP_Q_SIG                 |
                                             MMGSDI_REFRESH_CMD_Q_SIG              |
                                             MMGSDI_TASK_STOP_SIG                  |
                                             MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG    |
                                             TASK_OFFLINE_SIG;
  rex_sigs_type               sigs_rcvd    = 0x00;
  mmgsdi_task_enum_type       running_task = MMGSDI_TASK_MAX;
  mmgsdi_task_state_enum_type task_state   = MMGSDI_TASK_INACTIVE;
  RCEVT_SIGEX_SIGREX          stop_sig;
  RCINIT_GROUP                gsdi_group;

  (void)dummy;

  /* Clear all signals */
  (void) rex_clr_sigs(rex_self(), (rex_sigs_type)~0);

  /* Get the task to initialize */
  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&running_task);
  if (running_task >= MMGSDI_NUM_TASKS)
  {
    UIM_MSG_ERR_1("Invalid running_task: 0x%x", running_task);
    return;
  }

  stop_sig.signal = rex_self();
  stop_sig.mask = MMGSDI_TASK_STOP_SIG;

  gsdi_group = rcinit_lookup_group_rextask(rex_self());

  rcinit_register_term_group(gsdi_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);

  /* Set the task to ACTIVE once it is running in MMGSDI context */
  mmgsdi_task_state[running_task] = MMGSDI_TASK_ACTIVE;

  if (running_task == MMGSDI_TASK_MAIN)
  {
  /* Register for CM/QMIMODEM ready signals and publish task ready evt for
     main task only.
     Perform task-ready sig registrations before doing the rcinit_handshake
     to avoid race-conditions where the notifying tasks might start early,
     before MMGSDI is even able to register for task-ready sigs for those
     tasks */
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    mmgsdi_rcevt_siginfo_cm.signal = rex_self();
    mmgsdi_rcevt_siginfo_cm.mask   = MMGSDI_RCEVT_CM_TASK_READY_SIG;
    (void)rcevt_register_sigex_name("cm:ready",
                                    RCEVT_SIGEX_TYPE_SIGREX,
                                    &mmgsdi_rcevt_siginfo_cm);

    mmgsdi_rcevt_siginfo_qmi_modem.signal = rex_self();
    mmgsdi_rcevt_siginfo_qmi_modem.mask   = MMGSDI_RCEVT_QMI_MODEM_TASK_READY_SIG;
    (void)rcevt_register_sigex_name("QMI_MODEM: READY",
                                    RCEVT_SIGEX_TYPE_SIGREX,
                                    &mmgsdi_rcevt_siginfo_qmi_modem);
#endif /*!FEATURE_UIM_TEST_FRAMEWORK*/

    /* QMI UIM globals and queue need to be initalized before RC INIT handshake
       so UIM drivers can post messages to QMI UIM immediately after handshake.*/
    qmi_uim_init_pre_startup();
  }

  /* Handshake with RCInit framework */
  rcinit_handshake_startup();

  /* Complete remaining initialization after handshaking with RCInit */
  mmgsdi_init_pre_startup();

  if (running_task == MMGSDI_TASK_MAIN)
  {
    (void)rcevt_signal_name("MM_GSDI: READY");
  }

  mmgsdi_init_post_startup();

  /* These signals are supposed to be set only on the main GSDI task. So
     for a robust design, include them in the wait sig list ONLY for the
     main GSDI task so that the tasks other than main task do not process
     them if they accidently receive these signals (maybe because of some
     sig clashes) */
  if (running_task == MMGSDI_TASK_MAIN)
  {
    sigs =                           sigs                                  |
#ifdef FEATURE_SIMLOCK
                                     MMGSDI_SIMLOCK_MSG_SIG                |
#endif /* FEATURE_SIMLOCK */
                                     MMGSDI_RCEVT_CM_TASK_READY_SIG        |
                                     MMGSDI_RCEVT_QMI_MODEM_TASK_READY_SIG |
                                     MMGSDI_TASK_CMD_Q_SIG                 |
                                     MMGSDI_CARD_STATUS_CMD_Q_SIG          |
                                     MMGSDI_NV_WRITE_SIG                   |
                                     MMGSDI_QMI_UIM_SIG                    |
#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
                                     MMGSDI_QCSI_QMI_SIM_INTERNAL_SIG      |
#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE*/
                                     MMGSDI_OBDAPP_SIG;

    /* Main task is ready with client id added in client_id_reg table and event
       callback function updated. Set ready signal to slot tasks to listen for
       CARD_STATUS_CMD_Q signal */

    MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

    if(UIM_GSDI_SLOT1_TCB != NULL &&
       mmgsdi_task_state[MMGSDI_TASK_SLOT1] != MMGSDI_TASK_ENDED)
    {
      rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_MAIN_TASK_READY_SIG);
    }
    if(UIM_GSDI_SLOT2_TCB != NULL &&
       mmgsdi_task_state[MMGSDI_TASK_SLOT2] != MMGSDI_TASK_ENDED)
    {
      rex_set_sigs(UIM_GSDI_SLOT2_TCB, MMGSDI_MAIN_TASK_READY_SIG);
    }
    if(UIM_GSDI_SLOT3_TCB != NULL &&
       mmgsdi_task_state[MMGSDI_TASK_SLOT3] != MMGSDI_TASK_ENDED)
    {
      rex_set_sigs(UIM_GSDI_SLOT3_TCB, MMGSDI_MAIN_TASK_READY_SIG);
    }

    MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

  }
  else
  {
    /* Slot tasks only listen for main task ready signal.at init time Once it receives
     ready signal from main task it starts listening to card status signals.
     This is done to make sure that MMGSDI client id is added to client reg
     table and updated with the event call back function in main task context */
    sigs = sigs | MMGSDI_MAIN_TASK_READY_SIG;
  }

  /* ------------------------------------------------------------------------
  ** Main Task Loop
  ** ----------------------------------------------------------------------*/
  for (;;)
  {
    /*-----------------------------------------------------------------------------
                            Wait on signal
    -----------------------------------------------------------------------------*/
    sigs_rcvd = mmgsdi_wait( sigs );

    /*-----------------------------------------------------------------------------
                            Wait for task sync crit section
    -----------------------------------------------------------------------------*/
    MMGSDIUTIL_ENTER_TASK_SYNC_CRIT_SECT;

    MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
    task_state = mmgsdi_task_state[running_task];
    MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

    /*-----------------------------------------------------------------------------
                          Process Signal - TASK_OFFLINE_SIG
    -----------------------------------------------------------------------------*/

    if (sigs_rcvd & sigs & TASK_OFFLINE_SIG)
    {
      mmgsdi_handle_offline_sig();
    } /* end if TASK_OFFLINE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - TASK_STOP_SIG/MMGSDI_TASK_STOP_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & sigs & MMGSDI_TASK_STOP_SIG)
    {
      mmgsdi_handle_stop_sig();
      rcinit_unregister_term_group(gsdi_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);
      rcinit_handshake_term();
      task_state = MMGSDI_TASK_STOPPED;
    } /* end if MMGSDI_TASK_STOP_SIG/MMGSDI_TASK_STOP_SIG */

#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_RCEVT_QMI_MODEM_TASK_READY_SIG -
                          only main task will receive this signal
    -----------------------------------------------------------------------------*/
    else if ((sigs_rcvd & sigs & MMGSDI_RCEVT_QMI_MODEM_TASK_READY_SIG) &&
             (running_task == MMGSDI_TASK_MAIN))
    {
      mmgsdi_handle_qmi_modem_task_ready();
    }

    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_RCEVT_CM_TASK_READY_SIG -
                          only main task will receive this signal
    -----------------------------------------------------------------------------*/
    else if ((sigs_rcvd & sigs & MMGSDI_RCEVT_CM_TASK_READY_SIG) &&
             (running_task == MMGSDI_TASK_MAIN))
    {
      mmgsdi_handle_cm_task_ready();
    }
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_UIM_SIM_INTERNAL_INTERFACE
    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_QMI_SIM_INTERNAL_SIG
    -----------------------------------------------------------------------------*/
    else if ((sigs_rcvd & sigs & MMGSDI_QCSI_QMI_SIM_INTERNAL_SIG) &&
             (running_task == MMGSDI_TASK_MAIN))
    {
      mmgsdi_handle_qcsi_qmi_sim_internal_sig();
    } /* end if MMGSDI_QCSI_QMI_SIM_INTERNAL_SIG */
#endif /* FEATURE_UIM_SIM_INTERNAL_INTERFACE */

    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_MAIN_TASK_READY_SIG
    -----------------------------------------------------------------------------*/
    else if ((sigs_rcvd & sigs & MMGSDI_MAIN_TASK_READY_SIG) &&
             (running_task != MMGSDI_TASK_MAIN))
    {
      /* Make slot task listen for card status command signals now as main task
         is ready */
      sigs = sigs                         |
             MMGSDI_CARD_STATUS_CMD_Q_SIG |
             MMGSDI_TASK_CMD_Q_SIG;
      sigs = sigs & ~(MMGSDI_MAIN_TASK_READY_SIG);
      (void)rex_clr_sigs(rex_self(), MMGSDI_MAIN_TASK_READY_SIG);
    }

    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_TASK_RSP_Q_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & sigs & MMGSDI_TASK_RSP_Q_SIG)
    {
      /* Do not handle response if GSDI task is not active */
      if(mmgsdi_generic_data_ptr &&
         task_state == MMGSDI_TASK_ACTIVE)
      {
        mmgsdi_handle_queue_sig(sigs_rcvd, &sigs);
      }
    } /* end if MMGSDI_TASK_RSP_Q_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - GSDI_UIM_SANITY_TIMER_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & sigs & MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG)
    {
      mmgsdi_handle_sanity_timer_expire_sig();
    } /* end if MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_NV_WRITE_SIG
    -----------------------------------------------------------------------------*/
    else if ((sigs_rcvd & sigs & MMGSDI_NV_WRITE_SIG) &&
             (running_task == MMGSDI_TASK_MAIN))
    {
      mmgsdi_handle_nv_sig();
    } /* end if MMGSDI_NV_WRITE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_CARD_STATUS_CMD_Q_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & sigs & MMGSDI_CARD_STATUS_CMD_Q_SIG)
    {
      /* Do not handle command if GSDI task is not active */
      if(mmgsdi_generic_data_ptr &&
         task_state == MMGSDI_TASK_ACTIVE)
      {
        mmgsdi_handle_queue_sig(sigs_rcvd, &sigs);
      }
    } /* end if MMGSDI_CARD_STATUS_CMD_Q_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_REFRESH_CMD_Q_SIG
    -----------------------------------------------------------------------------*/
    else if ((sigs_rcvd & sigs & MMGSDI_REFRESH_CMD_Q_SIG) &&
             (running_task == MMGSDI_TASK_MAIN))
    {
      /* Do not handle command if GSDI task is not active */
      if(mmgsdi_generic_data_ptr &&
         task_state == MMGSDI_TASK_ACTIVE)
      {
        mmgsdi_handle_refresh_queue_sig();
      }
    } /* end if MMGSDI_REFRESH_CMD_Q_SIG */

#ifdef FEATURE_SIMLOCK
    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_SIMLOCK_MSG_SIG
    -----------------------------------------------------------------------------*/
    else if ((sigs_rcvd & sigs & MMGSDI_SIMLOCK_MSG_SIG) &&
             (running_task == MMGSDI_TASK_MAIN))
    {
      /* Do not handle command if GSDI task has been stopped */
      if(mmgsdi_generic_data_ptr &&
         task_state == MMGSDI_TASK_ACTIVE)
      {
        mmgsdi_handle_simlock_queue_sig();
      }
    } /* end if MMGSDI_SIMLOCK_MSG_SIG */
#endif /* FEATURE_SIMLOCK */

    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_TASK_CMD_Q_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & sigs & MMGSDI_TASK_CMD_Q_SIG)
    {
      /* Do not handle command if GSDI task is not active */
      if(mmgsdi_generic_data_ptr &&
         task_state == MMGSDI_TASK_ACTIVE)
      {
        mmgsdi_handle_queue_sig(sigs_rcvd, &sigs);
      }
    } /* end if MMGSDI_TASK_CMD_Q_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - MMGSDI_QMI_UIM_SIG
    -----------------------------------------------------------------------------*/
    else if ((sigs_rcvd & sigs & MMGSDI_QMI_UIM_SIG) &&
             (running_task == MMGSDI_TASK_MAIN))
    {
      mmgsdi_handle_qmi_uim_sig();
    } /* end if MMGSDI_QMI_UIM_SIG */

    /*-----------------------------------------------------------------------------
            Process Signal - MMGSDI_OBDAPP_SIG
    -----------------------------------------------------------------------------*/
    else if ((sigs_rcvd & sigs & MMGSDI_OBDAPP_SIG) &&
             (running_task == MMGSDI_TASK_MAIN))
    {
      mmgsdi_handle_obdapp_sig();
    } /* end if MMGSDI_OBDAPP_SIG */
    else
    {
      UIM_MSG_ERR_1("MMGSDI TASK INVALID SIGS: %x", sigs_rcvd);

      /* Reset the Sigs by Masking out Good Sigs and
      ** Clearing out Bad SIGS
      */
      (void)rex_clr_sigs(rex_self(), (sigs_rcvd & ~sigs) );

      /* Preserve any Signals currently Set and wipe */
      /* out the bad Signals                         */
      sigs_rcvd &= sigs;
    } /* end else - Invalid signal in GSDI task */

    /*-----------------------------------------------------------------------------
                            Leave the task sync crit section
    -----------------------------------------------------------------------------*/
    MMGSDIUTIL_LEAVE_TASK_SYNC_CRIT_SECT;

    if(task_state == MMGSDI_TASK_STOPPED)
    {
      break;
    }
  } /* end for(;;) */

  /* Cleanup after GSDI tasks are stopped
     - Deregister from dog heart beat
     - Clear timers
     - Flush cmds in queues
     - Free memory held by MMGSDI */
  mmgsdi_task_ended();
} /* mmgsdi_task */


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
  mmgsdi_app_state_enum_type *curr_state_ptr)
{
  mmgsdi_return_enum_type       mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type     *int_app_info_ptr = NULL;

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    NULL,
                                                    &int_app_info_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    /* Invalid session_id*/
    return;
  }

  if (curr_state_ptr != NULL)
  {
    *curr_state_ptr = int_app_info_ptr->app_state;
  }

  if (constraint_state != MMGSDI_APP_STATE_ENUM_MAX)
  {
    if (int_app_info_ptr->app_state == constraint_state)
    {
      int_app_info_ptr->app_state = new_state;
    }
  }
} /* mmgsdi_state_update_app_state_for_prov_app */


/*===========================================================================
FUNCTION MMGSDI_STATE_UPDATE_CARD_INSERTED_STATE

DESCRIPTION
  This function updates the card state and the app states when the CARD_INSERTED
  event is received

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.

 CRITICAL SECTIONS:
  The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
  to slot data ptr
===========================================================================*/
static void mmgsdi_state_update_card_inserted_state(
  const mmgsdi_card_inserted_evt_info_type *evt_ptr)
{
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;
  uint32                  i             = MMGSDI_MAX_EXT_APP_INFO;
  uint32                  j             = 0;

  if(evt_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null evt_ptr in mmgsdi_state_update_card_inserted_state");
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(evt_ptr->slot);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  slot_data_ptr->mmgsdi_state = MMGSDI_STATE_CARD_INSERTED;

  if (slot_data_ptr->app_info_pptr != NULL)
  {
    for (i = 0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
    {
      if(slot_data_ptr->app_info_pptr[i] != NULL)
      {
        for (j = 0; j < evt_ptr->num_aids_avail && j < MMGSDI_MAX_APP_INFO; j++)
        {
          if ((!memcmp(&slot_data_ptr->app_info_pptr[i]->app_data,
                   &evt_ptr->aid_info[j],
                     sizeof(mmgsdi_aid_type))))
          {
            slot_data_ptr->app_info_pptr[i]->app_state =
              MMGSDI_APP_STATE_DETECTED;
          }
          if ((evt_ptr->aid_info[j].app_type == MMGSDI_APP_RUIM) ||
            (evt_ptr->aid_info[j].app_type == MMGSDI_APP_CSIM))
          {
#ifdef FEATURE_MMGSDI_3GPP2
            /* In the case of CSIM/RUIM to avoid deadlock between NV and GSDI,
               the session id for NVRUIM is assigned after session changed event
               by that time GSDI has cached it's NV items*/
            nvruim_dir_present |= NVRUIM_CDMA_SUBSCRIP_PRESENT;
#endif /* FEATURE_MMGSDI_3GPP2 */
          }
        }
      }
    }
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;
} /* mmgsdi_state_update_card_inserted_state */


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

 CRITICAL SECTIONS:
  The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
  to slot data ptr.
===========================================================================*/
void mmgsdi_state_update_card_error_state(
  mmgsdi_slot_id_enum_type slot_id)
{
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  slot_data_ptr->mmgsdi_state          = MMGSDI_STATE_NO_CARD;
  slot_data_ptr->sfi_supported         = FALSE;
  memset(&slot_data_ptr->card_atr, 0x00, sizeof(slot_data_ptr->card_atr));
  memset(&slot_data_ptr->hist_char, 0x00, sizeof(slot_data_ptr->hist_char));

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  /* Reset GSTK loc envelope (VPE) response count */
  mmgsdi_util_reset_gstk_loc_env_rsp_count(slot_id);
} /* mmgsdi_state_update_card_error_state */


/*===========================================================================
FUNCTION MMGSDI_STATE_PIN1_UPDATE

DESCRIPTION
  Function to update the state or global info as needed when PIN1 event
  is received

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void mmgsdi_state_pin1_update(
  mmgsdi_session_id_type session_id)
{
  boolean                       can_proceed      = FALSE;
  mmgsdi_return_enum_type       mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type     *int_app_info_ptr = NULL;
  mmgsdi_app_state_enum_type    old_state        = MMGSDI_APP_STATE_UNKNOWN;
  mmgsdi_slot_id_enum_type      slot_id          = MMGSDI_SLOT_AUTOMATIC;

  UIM_MSG_HIGH_0("mmgsdi_state_pin1_update");

  /* Update to reflect that the new state if we were in the ACTIVATED state
     before */
  mmgsdi_state_update_app_state_for_prov_app(session_id,
                                             MMGSDI_APP_STATE_ACTIVATED,
                                             MMGSDI_APP_STATE_PIN_EVT_SENT,
                                             &old_state);

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    &int_app_info_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    /* Invalid session_id*/
    return;
  }
  /* Whenever we get the PIN event, we first check the Pin Status */
  can_proceed = mmgsdi_util_can_proceed_to_post_pin1(
                  (const mmgsdi_int_app_info_type*)int_app_info_ptr);
  if(can_proceed)
  {
    /* If every thing is good, then allow to move our APP state if required */
    if ((old_state == MMGSDI_APP_STATE_ACTIVATED)||
        (old_state == MMGSDI_APP_STATE_PIN_EVT_SENT))
    {
      /* Update PIN_EVT_SENT to READY_TO_PERSO State */
      mmgsdi_state_update_app_state_for_prov_app(
                                             session_id,
                                             MMGSDI_APP_STATE_PIN_EVT_SENT,
                                             MMGSDI_APP_STATE_READY_FOR_PERSO,
                                             NULL);

      (void)mmgsdi_card_init_build_internal_pup_req(
              session_id,
              slot_id,
              MMGSDI_PIN1_EVT,
              MMGSDI_STATE_CARD_INSERTED,
              MMGSDI_APP_STATE_READY_FOR_PERSO);
    }
    /* If the old state is not equal to ACTIVATED or PIN_EVENT_SENT state,
       that means we already moved to READY_TO_PERSO state. */
  }
  else
  {
    /* If PIN1 is blocked and we are in middle of ready state,
       then we need to move APP state back to PIN EVT Sent state */
    if ((old_state == MMGSDI_APP_STATE_READY_FOR_PERSO)||
        (old_state == MMGSDI_APP_STATE_PERSO_VERIFIED))
    {
      int_app_info_ptr->app_state = MMGSDI_APP_STATE_PIN_EVT_SENT;
      int_app_info_ptr->perso_state = MMGSDI_SIMLOCK_NOT_VERIFIED;
    }
    /*If the old state is not equal to READY_TO_PERSO or PERSO_VERIFIED then,
      No need to move our state, as PIN is blocked after ready state */
  }
} /* mmgsdi_state_pin1_update */


/*===========================================================================
FUNCTION MMGSDI_STATE_PERSO_UPDATE

DESCRIPTION
  Function to update the state or global info as needed when Perso is completed

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void mmgsdi_state_perso_update(
  mmgsdi_session_id_type        session_id,
  mmgsdi_perso_status_enum_type perso_status)
{
  mmgsdi_int_app_info_type *int_app_info_ptr = NULL;
  mmgsdi_return_enum_type   mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type  slot_id          = MMGSDI_SLOT_AUTOMATIC;

  UIM_MSG_HIGH_0("mmgsdi_state_perso_update");

  if(!mmgsdi_is_perso_completed(session_id))
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    &slot_id,
                                                    &int_app_info_ptr);

  if ((mmgsdi_status != MMGSDI_SUCCESS) || (int_app_info_ptr == NULL))
  {
    /* Invalid session_id*/
    return;
  }

  /* Only if the app has just finished Perso init, trigger the
     state transition */
  if(int_app_info_ptr->app_state != MMGSDI_APP_STATE_READY_FOR_PERSO)
  {
    return;
  }

  /* Update to reflect the new state if we were in the READY_FOR_PERSO state
   before */
  if(perso_status == MMGSDI_PERSO_STATUS_WAIT_FOR_EXT_PERSO)
  {
    mmgsdi_state_update_app_state_for_prov_app(session_id,
                                               MMGSDI_APP_STATE_READY_FOR_PERSO,
                                               MMGSDI_APP_STATE_WAIT_FOR_SUB_OK,
                                               NULL);
  }
  /*If there is no need of external perso */
  else if(perso_status == MMGSDI_PERSO_STATUS_DONE)
  {
    /* Move the app state to PERSO_VERIFIED */
    mmgsdi_state_update_app_state_for_prov_app(session_id,
                                               MMGSDI_APP_STATE_READY_FOR_PERSO,
                                               MMGSDI_APP_STATE_PERSO_VERIFIED,
                                               NULL);

    /* Build internal pup request after the app (1x or GW) moved to
       perso_verified */
    (void)mmgsdi_card_init_build_internal_pup_req(session_id,
                                                  slot_id,
                                                  MMGSDI_PIN1_EVT,
                                                  MMGSDI_STATE_CARD_INSERTED,
                                                  MMGSDI_APP_STATE_PERSO_VERIFIED);
  }
} /* mmgsdi_state_perso_update */


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
rex_sigs_type mmgsdi_wait
(
  rex_sigs_type requested_mask
)
{
   rex_sigs_type          rex_signals_mask = 0x00;  /* Mask of signals returned by REX */
   mmgsdi_task_enum_type  mmgsdi_task      = MMGSDI_TASK_MAX;

   /* Get the current task */
   mmgsdi_task = mmgsdi_util_get_current_task();
   if(mmgsdi_task == MMGSDI_TASK_MAX)
   {
     /* In case current task is undetermined,
        queue to main task */
     mmgsdi_task = MMGSDI_TASK_MAIN;
   }

   MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

   /* Stop listening to signals corresponding to the MMGSDI command
      and response queues and error signals if GSDI task is not active */
   if(mmgsdi_task_state[mmgsdi_task] != MMGSDI_TASK_ACTIVE)
   {
     requested_mask &= ~(MMGSDI_TASK_CMD_Q_SIG            |
                         MMGSDI_TASK_RSP_Q_SIG            |
                         MMGSDI_CARD_STATUS_CMD_Q_SIG     |
                         MMGSDI_REFRESH_CMD_Q_SIG         |
#ifdef FEATURE_SIMLOCK
                         MMGSDI_SIMLOCK_MSG_SIG           |
#endif /* FEATURE_SIMLOCK */
                         MMGSDI_UIM_SANITY_TIMER_EXPIRE_SIG);
   }

   MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;

   /* --------------------------------------------------------------------
   Loop until the current signals from REX match with one or more of
   the requested signals.
   -------------------------------------------------------------------- */
   do
   {
      /* ---------------------------------------------------------------
      Perform the real wait to wait for a command...or to force a
      context switch to allow higher priority tasks to run.
      ---------------------------------------------------------------- */
      rex_signals_mask = rex_wait(requested_mask | MMGSDI_RPT_TIMER_SIG);
      /* DOG HeartBeat
         1. A single shared timer resource among all clients is used to register for dog.
         2. Dog will send the signals to the registered tasks (“ping”) instead of going through timer,
            through dog’s own heartbeat, and the tasks will respond to wdog (“pong”)
            Voluntary dog reporting is supported (pongs without pings).
         3. Hence we use dog_hb_report() to report to the dog and in turn pet it.
      */
      (void) rex_clr_sigs( rex_self(), MMGSDI_RPT_TIMER_SIG );
      dog_hb_report(gsdi_dog_rpt_var[mmgsdi_task]);

   /* do while mask requested does not match current mask */
   } while ((rex_signals_mask & requested_mask) == 0);

   return (rex_signals_mask);
} /* mmgsdi_wait() */


/*===========================================================================
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
void mmgsdi_evt_cb (const mmgsdi_event_data_type *event_ptr)
{
  mmgsdi_slot_id_enum_type           slot             = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_slot_data_type             *slot_data_ptr    = NULL;
  mmgsdi_session_info_type          *session_info_ptr = NULL;
  mmgsdi_onchip_sim_state_enum_type  onchip_state     = MMGSDI_ONCHIP_SIM_STATE_MAX_ENUM;
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_SUCCESS;
  gstk_slot_id_enum_type             gstk_slot        = GSTK_SLOT_ID_MAX;
  uint8                              slot_index       = MMGSDI_SLOT_1_INDEX;
  mmgsdi_session_type_enum_type      session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;

  if (event_ptr == NULL)
  {
    UIM_MSG_ERR_0("In mmgsdi_evt_cb, event_ptr is NULL" );
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(
                    event_ptr->session_id,
                    &slot,
                    NULL,
                    NULL,
                    NULL,
                    &session_info_ptr,
                    NULL);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

    if(slot_data_ptr != NULL)
    {
      onchip_state = slot_data_ptr->onchip_sim_data.state;
    }
  }

  UIM_MSG_HIGH_1("Received event: 0x%x in mmgsdi_evt_cb", event_ptr->evt);

  /* Do not handle events if received as part of early PSM init
     Check if slot is SLOT1 and PSM early init state to skip processing events */
  if (mmgsdi_psm_get_status_early_init())
  {
    /* If slot is not extracted already(to extract onchip state) above for cases like
       card and perso events, extract it explicitly */
    if (slot == MMGSDI_MAX_SLOT_ID_ENUM)
    {
      slot = mmgsdi_evt_get_slot_from_event_data(event_ptr);
    }

    if (slot == MMGSDI_SLOT_1)
    {
      UIM_MSG_HIGH_0("Do not process mmgsdi event during psm early init");
      return;
    }
  }

  switch(event_ptr->evt)
  {
    case MMGSDI_SESSION_CHANGED_EVT:
       (void)mmgsdi_util_get_session_type(event_ptr->session_id, &session_type, NULL);
       UIM_MSG_HIGH_4("MMGSDI_SESSION_CHANGED_EVT, app: 0x%x, slot: 0x%x, activated: 0x%x, session_type: 0x%x",
                      event_ptr->data.session_changed.app_info.app_data.app_type,
                      event_ptr->data.session_changed.app_info.slot,
                      event_ptr->data.session_changed.activated,
                      session_type);

      if (event_ptr->data.session_changed.activated)
      {
        mmgsdi_state_update_app_state_for_prov_app(event_ptr->session_id,
                                                   MMGSDI_APP_STATE_DETECTED,
                                                   MMGSDI_APP_STATE_ACTIVATED,
                                                   NULL);
        if(onchip_state != MMGSDI_ONCHIP_SIM_INIT)
        {
          (void)mmgsdi_card_init_build_internal_pup_req(
                  event_ptr->session_id,
                  event_ptr->data.session_changed.app_info.slot,
                  event_ptr->evt,
                  MMGSDI_STATE_CARD_INSERTED,
                  MMGSDI_APP_STATE_ACTIVATED);
        }

        if (MMGSDI_FEATURE_ENABLED ==
              mmgsdi_nv_get_jcdma_service_status(event_ptr->data.session_changed.app_info.slot))
        {
          UIM_MSG_HIGH_0("JCDMA Service is enabled, opening non prov sesssion");
          mmgsdi_jcdma_open_non_prov_session(
            &event_ptr->data.session_changed.app_info);
        }

        (void)mmgsdi_cache_create_prov_file_cache_clear_caches(
                event_ptr->session_id,
                TRUE);

        gstk_slot = mmgsdi_util_convert_gstk_slot(event_ptr->data.session_changed.app_info.slot);
        if(GSTK_SLOT_ID_MAX == gstk_slot)
        {
          return;
        }

        if(event_ptr->data.session_changed.app_info.app_data.app_type == MMGSDI_APP_USIM)
        {
          mmgsdi_set_ens_support_flag(event_ptr->session_id,
                                      event_ptr->data.session_changed.app_info.slot,
                                      event_ptr->data.session_changed.app_info.app_data.app_type);

          /* Update GSTK to enable FETCH as ENS initialisation has happened */
          gstk_gsdi_set_fetch_status(gstk_slot, TRUE);
        }

        if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_MULTISIM_AUTO_PROVISIONING, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
           == MMGSDI_FEATURE_ENABLED)
        {
          (void)mmgsdi_update_efs_subs_prov(&event_ptr->data.session_changed,
                                            event_ptr->session_id);
        }
      }
      else
      {
        UIM_MSG_HIGH_1("Session closed cause: 0x%x",
                       event_ptr->data.session_changed.cause);

        (void)mmgsdi_cache_create_prov_file_cache_clear_caches(
                event_ptr->session_id,
                FALSE);

        if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_MULTISIM_AUTO_PROVISIONING, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
           == MMGSDI_FEATURE_ENABLED)
        {
          if((event_ptr->data.session_changed.cause ==
                MMGSDI_SESSION_CLOSED_CAUSE_CLIENT_REQUEST) ||
             (event_ptr->data.session_changed.cause ==
                MMGSDI_SESSION_CLOSED_CAUSE_MISMATCH_FOUND))
          {
            mmgsdi_clear_subs_prov_app_info(&event_ptr->data.session_changed);
          }
        }

        /* Clear refresh retry table entries for the deactivated app */
        mmgsdi_refresh_retry_clear_app_entries(
          event_ptr->data.session_changed.app_info.app_data.app_type);

#ifdef FEATURE_SIMLOCK
        if (event_ptr->data.session_changed.cause != MMGSDI_SESSION_CLOSED_CAUSE_PERSO_OPERATION_FAIL)
        {
          /* Need to re-run the SIM Lock algorithm, in case certain applications in
             the current SIM Lock configuration required this application to reach/stay
             in sub ready and are no longer valid. */
          mmgsdi_status = mmgsdi_simlock_build_and_run_algo(FALSE);

          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            UIM_MSG_ERR_1("Failed to rerun SIM Lock Algorithm, mmgsdi_status 0x%x",
                            mmgsdi_status);
          }
        }
#endif /* FEATURE_SIMLOCK */
      }

#ifdef FEATURE_UIM_PERSISTENT_CACHE
      /* deactive persistent cache disregard of session change with activated is TRUE or FALSE
         persistent cache will be activated when IMSI is read. */
      if (session_info_ptr != NULL)
      {
        (void)mmgsdi_persistent_cache_deactivate_profile(event_ptr->data.session_changed.app_info.slot,
                                                         session_info_ptr->session_type);
      }
#endif /* FEATURE_UIM_PERSISTENT_CACHE */
      break;

    case MMGSDI_PIN1_EVT:
      UIM_MSG_HIGH_2("MMGSDI_PIN1_EVT, status: 0x%x, slot: 0x%x",
                     event_ptr->data.pin.pin_info.status,
                     event_ptr->data.pin.slot);

      if(onchip_state != MMGSDI_ONCHIP_SIM_INIT)
      {
        mmgsdi_state_pin1_update(event_ptr->session_id);
      }
      break;

    case MMGSDI_PIN2_EVT:
      UIM_MSG_HIGH_2("MMGSDI_PIN2_EVT, status: 0x%x, slot: 0x%x",
                     event_ptr->data.pin.pin_info.status,
                     event_ptr->data.pin.slot);
      break;

    case MMGSDI_UNIVERSAL_PIN_EVT:
      UIM_MSG_HIGH_2("MMGSDI_UNIVERSAL_PIN_EVT, status: 0x%x, slot: 0x%x",
                     event_ptr->data.pin.pin_info.status,
                     event_ptr->data.pin.slot);

      if(onchip_state != MMGSDI_ONCHIP_SIM_INIT)
      {
        mmgsdi_state_pin1_update(event_ptr->session_id);
      }
      break;

    case MMGSDI_PERSO_EVT:
      UIM_MSG_HIGH_2("MMGSDI_PERSO_EVT, feature: 0x%x, status: 0x%x",
                     event_ptr->data.perso.feature, event_ptr->data.perso.status);

      if(onchip_state != MMGSDI_ONCHIP_SIM_INIT)
      {
        mmgsdi_state_perso_update(event_ptr->session_id,
                                  event_ptr->data.perso.status);
      }
      break;

    case MMGSDI_HIDDENKEY_EVT:
      UIM_MSG_HIGH_3("MMGSDI_HIDDENKEY_EVT, status: 0x%x, dir_index: 0x%x, slot: 0x%x",
                     event_ptr->data.pin.pin_info.status,
                     event_ptr->data.pin.dir_index,
                     event_ptr->data.pin.slot);
      break;

    case MMGSDI_SUBSCRIPTION_READY_EVT:
      UIM_MSG_HIGH_2("MMGSDI_SUBSCRIPTION_READY_EVT, app: 0x%x, slot: 0x%x",
                     event_ptr->data.subscription_ready.app_info.app_data.app_type,
                     event_ptr->data.subscription_ready.app_info.slot);
      if (MMGSDI_APP_CSIM == event_ptr->data.subscription_ready.app_info.app_data.app_type)
      {
        slot_data_ptr = mmgsdi_util_get_slot_data_ptr(event_ptr->data.subscription_ready.app_info.slot);
        if (slot_data_ptr != NULL)
        {
          gstk_slot = mmgsdi_util_convert_gstk_slot(event_ptr->data.subscription_ready.app_info.slot);
          if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH, gstk_slot))
          {
            if(event_ptr->data.subscription_ready.activation_status == MMGSDI_ACT_STATUS_ACTIVATED)
            {
              slot_data_ptr->imsi_m_programmed = TRUE;
            }
            else
            {
              slot_data_ptr->imsi_m_programmed = FALSE;
            }
            UIM_MSG_HIGH_2("CSIM IMSI_M programmed status for slot 0x%x: 0x%x",
                           event_ptr->data.subscription_ready.app_info.slot,
                           slot_data_ptr->imsi_m_programmed);
          }
        }
      }

      if (MMGSDI_FEATURE_ENABLED ==
              mmgsdi_nv_get_jcdma_service_status(event_ptr->data.subscription_ready.app_info.slot))
      {
        UIM_MSG_HIGH_0("JCDMA Service is enabled, getting card attr");
        (void)mmgsdi_jcdma_get_uicc_card_attr(
          event_ptr->data.subscription_ready.app_info.app_data.app_type,
          event_ptr->data.subscription_ready.app_info.slot);
        mmgsdi_jcdma_close_non_prov_session(
          &event_ptr->data.subscription_ready.app_info);
      }

#ifdef FEATURE_MMGSDI_3GPP2
      /* If SUB_READY is for CSIM/RUIM app then check whether PRL init is done
         or not because it is possible that during post pin init, SD task was
         not ready and PRL validation was not success. If that is the case,
         then do PRL init here. If by this time also SD task is not READY
         then PRL init will be done while handling any READ_PRL_REQ */
      if (session_info_ptr != NULL &&
          mmgsdi_util_is_1x_prov_session(session_info_ptr->session_type))
      {
        uint8                       prl_index  = 0;
        mmgsdi_technology_enum_type dummy_tech = MMGSDI_TECH_UNKNOWN;
        mmgsdi_attribute_enum_type  attribute  = MMGSDI_ATTR_UNKNOWN;

        (void)mmgsdi_util_get_tech_and_attr_from_session_type(
                                               session_info_ptr->session_type,
                                               &dummy_tech,
                                               &attribute);
        prl_index = (uint8)attribute;
        if (prl_index <= MMGSDI_TER_PRL_INDEX)
        {
          if (mmgsdi_1x_prl[prl_index].prl_validated == FALSE)
          {
            (void)mmgsdi_card_init_1x_read_prl_init(event_ptr->session_id);
          }
        }
      }
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    case MMGSDI_REFRESH_EVT:
      mmgsdi_refresh_process_evt(event_ptr);
      break;

    /* Slot related Events */
    case MMGSDI_CARD_INSERTED_EVT:
      UIM_MSG_HIGH_1("MMGSDI_CARD_INSERTED_EVT, slot: 0x%x",
                     event_ptr->data.card_inserted.slot);

      slot      = event_ptr->data.card_inserted.slot;
      gstk_slot = mmgsdi_util_convert_gstk_slot(slot);

      /* Now that card inserted event has been sent out, enable FETCH and
         therefore potential proactive commands for non-ATT cards */
      if(!mmgsdi_nv_is_ens_enabled(slot))
      {
        gstk_gsdi_set_fetch_status(gstk_slot, TRUE);
      }

      mmgsdi_status = mmgsdi_util_get_slot_index(slot, &slot_index);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        if ((slot_index == MMGSDI_SLOT_1_INDEX) ||
            (slot_index == MMGSDI_SLOT_2_INDEX) ||
            (slot_index == MMGSDI_SLOT_3_INDEX))
        {
          /* Since the card has come back up, REFRESH is complete */
          if((mmgsdi_refresh_info_ptr[slot_index] != NULL) &&
             (mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET ||
              mmgsdi_refresh_info_ptr[slot_index]->mode == MMGSDI_REFRESH_RESET_AUTO) &&
             (mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id == slot))
          {
            (void) mmgsdi_refresh_send_complete(event_ptr->client_id,
                                                0,
                                                slot,
                                                TRUE);
          }
        }
      }
      /* Update to card state and the underlying app state */
      mmgsdi_state_update_card_inserted_state(&event_ptr->data.card_inserted);

      slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);

      if(slot_data_ptr != NULL)
      {
        if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH, gstk_slot))
        {
          /* Set the imsi_m_programmed flag now to preserve normal behavior in
             case a Refresh is triggered before SUBSCRIPTION_READY_EVT and updating
             of IMSI_M status from cache */
          slot_data_ptr->imsi_m_programmed = TRUE;
        }

        onchip_state = slot_data_ptr->onchip_sim_data.state;
      }

      if(onchip_state != MMGSDI_ONCHIP_SIM_INIT)
      {
        (void)mmgsdi_card_init_build_internal_pup_req(
                0,
                slot,
                event_ptr->evt,
                MMGSDI_STATE_CARD_INSERTED,
                MMGSDI_APP_STATE_DETECTED);
      }
      mmgsdi_init_rtre_config();
      break;

    case MMGSDI_CARD_ERROR_EVT:
    case MMGSDI_CARD_REMOVED_EVT:
      UIM_MSG_HIGH_2("MMGSDI_CARD_ERROR/REMOVED_EVT, condition: 0x%x, slot: 0x%x",
                     event_ptr->data.card_error.info,
                     event_ptr->data.card_error.slot);

      slot = event_ptr->data.card_error.slot;
      /* Update to card state and the underlying app state */
      mmgsdi_state_update_card_error_state(slot);

      mmgsdi_init_rtre_config();
      /* clean up the refresh pointer to avoid memory leak as well as the
         inability to do hot swap due to the refresh state */
      mmgsdi_status = mmgsdi_util_get_slot_index(slot, &slot_index);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        if ((slot_index == MMGSDI_SLOT_1_INDEX) ||
            (slot_index == MMGSDI_SLOT_2_INDEX) ||
            (slot_index == MMGSDI_SLOT_3_INDEX))
        {
          if((mmgsdi_refresh_info_ptr[slot_index] != NULL) &&
             (mmgsdi_refresh_info_ptr[slot_index]->request_header.slot_id == slot))
          {
            UIM_MSG_ERR_0("CARD_ERROR received, hence stopping the current REFRESH");

            mmgsdi_refresh_info_ptr[slot_index]->refresh_status = MMGSDI_CAN_NOT_REFRESH;

            (void)mmgsdi_refresh_rsp_send_cnf_to_gstk(
                    &mmgsdi_refresh_info_ptr[slot_index]->request_header);
          }
        }
      }

      /* Refresh Retry is supported only on Slot 1 */
      if(slot == MMGSDI_SLOT_1)
      {
        mmgsdi_refresh_retry_clear_table(MMGSDI_SLOT_1_INDEX);
      }

#ifdef FEATURE_UIM_PERSISTENT_CACHE
      /* delete entire persistent cache */
      (void)mmgsdi_persistent_cache_delete(slot,
                                           MMGSDI_MAX_SESSION_TYPE_ENUM,
                                           FALSE);
#endif /* FEATURE_UIM_PERSISTENT_CACHE */

      /* Freeing the pb related cache data */
      mmgsdi_pb_free_cache_data(slot, MMGSDI_INVALID_SESSION_ID, TRUE);
      break;

    case MMGSDI_PROFILE_DATA_EVT:
      UIM_MSG_HIGH_3("MMGSDI_PROFILE_DATA_EVT received, slot: 0x%x, profile id: 0x%x, profile type: 0x%x",
                     event_ptr->data.profile_data.slot,
                     event_ptr->data.profile_data.profile.profile_id,
                     event_ptr->data.profile_data.profile.profile_type);
      break;

    case MMGSDI_PROFILE_SWITCHING_EVT:
      UIM_MSG_HIGH_1("MMGSDI_PROFILE_SWITCHING_EVT received, slot: 0x%x",
                     event_ptr->data.profile_switch.slot);
      break;

    default:
      break;
  }
}/* mmgsdi_evt_cb*/


/*===========================================================================
FUNCTION   mmgsdi_convert_nv_to_cm_rtre

DESCRIPTION
  Convert the RTRE define used by NV to the one used by CM

DEPENDENCIES
  None

RETURNS
  CM RTRE Config

SIDE EFFECTS
  None
===========================================================================*/
cm_rtre_config_e_type mmgsdi_convert_nv_to_cm_rtre(uint32 nv_rtre)
{
  cm_rtre_config_e_type cm_rtre;

  switch (nv_rtre)
  {
    case NV_RTRE_CONFIG_SIM_ACCESS:
      cm_rtre = CM_RTRE_CONFIG_SIM_ACCESS;
      break;

    case NV_RTRE_CONFIG_RUIM_ONLY:
      cm_rtre = CM_RTRE_CONFIG_RUIM_ONLY;
      break;

    case NV_RTRE_CONFIG_NV_ONLY:
      cm_rtre = CM_RTRE_CONFIG_NV_ONLY;
      break;

    case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
    default:
      cm_rtre = CM_RTRE_CONFIG_RUIM_OR_DROP_BACK;
      break;
  }
  return cm_rtre;
}/* mmgsdi_convert_nv_to_cm_rtre */


/*===========================================================================
FUNCTION   MMGSDI_CM_RTRE_CB

DESCRIPTION
  Callback for RTRE notification to the CM.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_cm_rtre_cb(void *data_block, cm_ph_cmd_e_type ph_cmd,
                cm_ph_cmd_err_e_type ph_cmd_err)
{
  (void) data_block;

  if (ph_cmd_err != CM_PH_CMD_ERR_NOERR)
  {
    UIM_MSG_ERR_2("RTRE Command 0x%x is completed with Error 0x%x ",
                ph_cmd, ph_cmd_err);
  }
  else
  {
    UIM_MSG_HIGH_1("RTRE Command 0x%x is completed successfully", ph_cmd);
  }
} /* mmgsdi_rtre_cb */


/*===========================================================================
FUNCTION   mmgsdi_init_rtre_config

DESCRIPTION
  Initializes the RTRE config with CM

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  This function is passsing the RTRE Mode / RTRE Configuration value to the CM and
  later CM will notify this RTRE configuration value to its clients.
===========================================================================*/
void mmgsdi_init_rtre_config(void)
{
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  nv_item_type *    nv_rtre_config;
  mmgsdi_nv_item_cache_type mmgsdi_nv_data = {0};
  nv_stat_enum_type result;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(nv_rtre_config, sizeof(nv_item_type))
  if (NULL == nv_rtre_config)
  {
    return;
  }

  /* Get RTRE configuration */
  result = mmgsdi_nv_get_item(NV_RTRE_CONFIG_I, &mmgsdi_nv_data, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  if(result != NV_DONE_S)
  {
    nv_rtre_config->rtre_config = NV_RTRE_CONFIG_RUIM_ONLY;
    (void) mmgsdi_nv_put_item(NV_RTRE_CONFIG_I, nv_rtre_config, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
  }
  else
  {
    nv_rtre_config->rtre_config = mmgsdi_nv_data.item_value.rtre_config;
  }

  /* If CM task is up */
  if((mmgsdi_generic_data_ptr->ready_tasks & (00000001 << (uint8) MMGSDI_CM_TYPE)))
  {
    UIM_MSG_HIGH_1("Notifying CM for 0x%x RTRE configuration ",
                   nv_rtre_config->rtre_config);
    if ( FALSE == cm_ph_cmd_change_rtre_config(mmgsdi_cm_rtre_cb, NULL, mmgsdi_generic_data_ptr->mmgsdi_cm_id,
                                               mmgsdi_convert_nv_to_cm_rtre(nv_rtre_config->rtre_config)))
    {
      UIM_MSG_ERR_0("cm_ph_cmd_change_rtre_config is failed ");
    }
  }
  else
  {
    /* Otherwise, send the rtre control command directly to NV, bypassing CM */
    mmgsdi_nv_send_rtre_command_to_nv(nv_rtre_config);
  }

  MMGSDIUTIL_TMC_MEM_FREE(nv_rtre_config);
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* mmgsdi_init_rtre_config */


/*===========================================================================
FUNCTION MMGSDI_ADD_SESSION_ID_INIT_ENTRY

DESCRIPTION
  Function to add an entry for session id during mmgsdi_init.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.

CRITICAL SECTIONS:
  The calling function must enter mmgsdi_client_app_data_crit_sect_ptr crit
  sect before invoking this function.
===========================================================================*/
static void mmgsdi_add_session_id_init_entry(
  int32                         client_index,
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_evt_callback_type     *evt_cb_ptr,
  boolean                       set_notify,
  mmgsdi_session_id_type       *session_id_ptr )
{
  int32                     session_index     = 0;
  mmgsdi_return_enum_type   mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_session_info_type *session_ptr       = NULL;

  if(session_id_ptr == NULL)
  {
    UIM_MSG_ERR_0("session_id_ptr NULL");
    return;
  }

  if(mmgsdi_client_id_reg_table[client_index] == NULL)
  {
    return;
  }

  /* Find free session index in the session_info_ptr table and allocate memory */
  mmgsdi_status = mmgsdi_util_get_free_session_index(client_index,
                                                     &session_index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index],
    sizeof(mmgsdi_session_info_type));
  if(mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index] == NULL)
  {
    return;
  }

  session_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  /* Assign complement of session_ptr as session id. Use least significant
     32 bits to maintain legacy behavior. */
  *session_id_ptr = (~(uint32)session_ptr) & 0xFFFFFFFF;

  mmgsdi_client_id_reg_table[client_index]->num_session++;

  /* Populate following entities in session record from req_ptr
     1. Session Id
     2. app_type
     3. session_evt_cb_ptr
     4. notify_init */
  session_ptr->session_id           = *session_id_ptr;
  session_ptr->session_type         = session_type;
  session_ptr->session_evt_cb_ptr   = evt_cb_ptr;
  session_ptr->notify_init          = set_notify;
  session_ptr->channel_info_index   = MMGSDI_MAX_CHANNEL_INFO;
  session_ptr->init_complete        = FALSE;

  /* These are internally opened sessions.
     Hence set session open cnf set as TRUE */
  session_ptr->is_session_open_cnf_sent = TRUE;
} /* mmgsdi_add_session_id_init_entry */


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
void mmgsdi_open_base_sessions(void)
{
  int32                   index         = 0;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  mmgsdi_status = mmgsdi_util_get_client_id_index(mmgsdi_generic_data_ptr->client_id,
                                                  &index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return;
  }
  /* mmgsdi_util_get_client_id_index success guarantees this dereference is ok */
  mmgsdi_client_id_reg_table[index]->evt_cb_ptr = mmgsdi_evt_cb;

  if(mmgsdi_generic_data_ptr->pri_gw_session_id == 0)
  {
    mmgsdi_add_session_id_init_entry(index,
                                     MMGSDI_GW_PROV_PRI_SESSION,
                                     mmgsdi_evt_cb,
                                     FALSE,
                                     &mmgsdi_generic_data_ptr->pri_gw_session_id);

    /*Register for Refresh*/
    (void)mmgsdi_refresh_register(mmgsdi_generic_data_ptr->pri_gw_session_id, TRUE);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_pri_gw_cache_tbl_ptr,
                                         sizeof(mmgsdi_attr_cache_type));
  }

  if(mmgsdi_generic_data_ptr->sec_gw_session_id == 0)
  {
    mmgsdi_add_session_id_init_entry(index,
                                     MMGSDI_GW_PROV_SEC_SESSION,
                                     mmgsdi_evt_cb,
                                     FALSE,
                                     &mmgsdi_generic_data_ptr->sec_gw_session_id);

    /*Register for Refresh*/
    (void)mmgsdi_refresh_register(mmgsdi_generic_data_ptr->sec_gw_session_id, TRUE);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_sec_gw_cache_tbl_ptr,
                                         sizeof(mmgsdi_attr_cache_type));
  }

  if(mmgsdi_generic_data_ptr->ter_gw_session_id == 0)
  {
    mmgsdi_add_session_id_init_entry(index,
                                     MMGSDI_GW_PROV_TER_SESSION,
                                     mmgsdi_evt_cb,
                                     FALSE,
                                     &mmgsdi_generic_data_ptr->ter_gw_session_id);

    /*Register for Refresh*/
    (void)mmgsdi_refresh_register(mmgsdi_generic_data_ptr->ter_gw_session_id, TRUE);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_ter_gw_cache_tbl_ptr,
                                         sizeof(mmgsdi_attr_cache_type));
  }

#ifdef FEATURE_MMGSDI_3GPP2
  if(mmgsdi_generic_data_ptr->pri_1x_session_id == 0)
  {
    mmgsdi_add_session_id_init_entry(index,
                                     MMGSDI_1X_PROV_PRI_SESSION,
                                     mmgsdi_evt_cb,
                                     FALSE,
                                     &mmgsdi_generic_data_ptr->pri_1x_session_id);

    /*Register for Refresh*/
    (void)mmgsdi_refresh_register(mmgsdi_generic_data_ptr->pri_1x_session_id, FALSE);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_pri_1x_cache_tbl_ptr,
                                         sizeof(mmgsdi_attr_cache_type));
  }

  if(mmgsdi_generic_data_ptr->sec_1x_session_id == 0)
  {
    mmgsdi_add_session_id_init_entry(index,
                                     MMGSDI_1X_PROV_SEC_SESSION,
                                     mmgsdi_evt_cb,
                                     FALSE,
                                     &mmgsdi_generic_data_ptr->sec_1x_session_id);

    /*Register for Refresh*/
    (void)mmgsdi_refresh_register(mmgsdi_generic_data_ptr->sec_1x_session_id, FALSE);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_sec_1x_cache_tbl_ptr,
                                         sizeof(mmgsdi_attr_cache_type));
  }

  if(mmgsdi_generic_data_ptr->ter_1x_session_id == 0)
  {
    mmgsdi_add_session_id_init_entry(index,
                                     MMGSDI_1X_PROV_TER_SESSION,
                                     mmgsdi_evt_cb,
                                     FALSE,
                                     &mmgsdi_generic_data_ptr->ter_1x_session_id);

    /*Register for Refresh*/
    (void)mmgsdi_refresh_register(mmgsdi_generic_data_ptr->ter_1x_session_id, FALSE);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_ter_1x_cache_tbl_ptr,
                                         sizeof(mmgsdi_attr_cache_type));
  }

#endif /* FEATURE_MMGSDI_3GPP2 */

  if(mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_1_INDEX] == 0)
  {
    mmgsdi_add_session_id_init_entry(index,
                                     MMGSDI_CARD_SESSION_SLOT_1,
                                     mmgsdi_evt_cb,
                                     FALSE,
                                     &mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_1_INDEX]);
    /* Channel info data to be mapped to channel 0 and will be done when
       channel 0 app is identified */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_slot1_common_cache_tbl_ptr,
                                         sizeof(mmgsdi_attr_cache_type));
  }

  if(mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_2_INDEX] == 0)
  {
    mmgsdi_add_session_id_init_entry(index,
                                     MMGSDI_CARD_SESSION_SLOT_2,
                                     mmgsdi_evt_cb,
                                     FALSE,
                                     &mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_2_INDEX]);
    /* Channel info data to be mapped to channel 0 and will be done when
       channel 0 app is identified */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_slot2_common_cache_tbl_ptr,
                                         sizeof(mmgsdi_attr_cache_type));
  }

  if(mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_3_INDEX] == 0)
  {
    mmgsdi_add_session_id_init_entry(index,
                                     MMGSDI_CARD_SESSION_SLOT_3,
                                     mmgsdi_evt_cb,
                                     FALSE,
                                     &mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_3_INDEX]);
    /* Channel info data to be mapped to channel 0 and will be done when
       channel 0 app is identified */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_slot3_common_cache_tbl_ptr,
                                         sizeof(mmgsdi_attr_cache_type));
  }
}/*mmgsdi_open_base_sessions*/


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_CLIENT_REG_REQ

   DESCRIPTION:
     This function will process the client ID registration request

   DEPENDENCIES:

   LIMITATIONS:
     This function currently does not support event notification

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
    The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
    to client id reg table.
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_client_reg_req(
  mmgsdi_client_id_reg_req_type * req_ptr
)
{
  mmgsdi_return_enum_type               build_status    = MMGSDI_ERROR;
  mmgsdi_return_enum_type               mmgsdi_status   = MMGSDI_ERROR;
  int32                                 i               = 0;
  int32                                 index           = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  mmgsdi_status = mmgsdi_add_client_id_entry(
                    &req_ptr->request_header.client_id,
                    req_ptr->request_header.response_cb,
                    req_ptr->client_name);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_status = mmgsdi_util_get_client_id_index(req_ptr->request_header.client_id,
                                                    &i);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* mmgsdi_util_get_client_id_index success guarantees this dereference is ok */
      mmgsdi_client_id_reg_table[i]->evt_cb_ptr = req_ptr->evt_cb_ptr;
    }
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Error in processing CLIENT ID_AND_EVT_REG REQ status=0x%x",
                  mmgsdi_status);
    return mmgsdi_status;
  }

  build_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (build_status == MMGSDI_SUCCESS)
  {
    build_status = mmgsdi_util_populate_client_request_table_info(index,
                                                                  &req_ptr->request_header,
                                                                  NULL, NULL);
    if (build_status == MMGSDI_SUCCESS)
    {
      build_status = mmgsdi_build_and_queue_client_reg_rsp(index, mmgsdi_status,
                                                            &req_ptr->request_header);
      if(build_status != MMGSDI_SUCCESS)
      {
        mmgsdi_util_free_client_request_table_index(index);
      }
    }
    else
    {
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  return build_status;
} /* mmgsdi_process_client_reg_req */


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_CLIENT_REG_CNF

   DESCRIPTION:
     This function will process the client registration confirmation

   DEPENDENCIES:

   LIMITATIONS:
     This function currently does not support event notification

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_client_reg_cnf(
  const mmgsdi_client_id_reg_rsp_type   * client_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_request_header_type         req_header       = {0};
  int32                              index            = 0;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_sw_status_type              status_word;

  /* status word is not applicable in client reg processing */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(client_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = client_rsp_ptr->rsp_data_index;

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
    Build confirmation message based on Client Registration or Client Event
    Callback Registration
    in case of MMGSDI_CLIENT_REG_MAX_ENUM, a client_reg_cnf_ptr will be built
    with mmgsdi_status sets to MMGSDI_ERROR
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cnf_build_and_queue(client_rsp_ptr->status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE, /* status word present
                                                        flag */
                                             status_word);
  return mmgsdi_status;
} /* mmgsdi_process_client_reg_cnf */


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_CLIENT_DEREG_REQ

   DESCRIPTION:
     This function will process the client ID deregistration request.

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
    The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
    to client id reg table.
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_client_dereg_req(
  const mmgsdi_client_id_dereg_req_type * req_ptr
)
{
  mmgsdi_return_enum_type build_status     = MMGSDI_ERROR;
  mmgsdi_return_enum_type mmgsdi_status    = MMGSDI_ERROR;
  int32                   client_index     = 0;
  int32                   client_req_index = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  mmgsdi_status = mmgsdi_util_get_client_id_index(
                    req_ptr->request_header.client_id,
                    &client_index);

  /* mmgsdi_util_get_client_id_index success guarantees a valid id_reg_table entry */
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    boolean client_has_non_prov_open = TRUE;

    mmgsdi_status = mmgsdi_check_client_has_non_prov_open(
                                     req_ptr->request_header.client_id,
                                     &client_has_non_prov_open);
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      if(client_has_non_prov_open)
      {
        UIM_MSG_ERR_0("Client still has a non-prov session open, need to close first");
        mmgsdi_status = MMGSDI_ERROR;
      }
      else
      {
        /* Now any open sessions should be card or prov
           sessions which can easily be released */
        uint8 i;

        for (i = 0; i < MMGSDI_MAX_SESSION_INFO ; i++)
        {
          mmgsdi_session_info_type *session_info_ptr =
                 mmgsdi_client_id_reg_table[client_index]->session_info_ptr[i];

          if ((session_info_ptr != NULL) &&
              (mmgsdi_util_is_prov_session(session_info_ptr->session_type) ||
               mmgsdi_util_is_card_session(session_info_ptr->session_type)))
          {
            if(MMGSDI_SUCCESS != mmgsdi_session_release_info(session_info_ptr,
                                                             MMGSDI_SESSION_CLOSE_REQ))
            {
              /* If any of the session_release_info calls fail then return
                 error in the rsp, but continue looping through sessions
                 attempting to release all of them */
              mmgsdi_status = MMGSDI_ERROR;
            }
          }
          else
          {
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_info_ptr);
          }
        }
        MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_client_id_reg_table[client_index]);
      }
    }
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  build_status = mmgsdi_util_get_client_request_table_free_index(&client_req_index);
  if (build_status == MMGSDI_SUCCESS)
  {
    build_status = mmgsdi_util_populate_client_request_table_info(client_req_index,
                                                        &req_ptr->request_header,
                                                         NULL,
                                                         NULL);
    if (build_status == MMGSDI_SUCCESS)
    {
      build_status = mmgsdi_build_and_queue_client_reg_rsp(client_req_index,
                                                     mmgsdi_status,
                                                     &req_ptr->request_header);
      if(build_status != MMGSDI_SUCCESS)
      {
        mmgsdi_util_free_client_request_table_index(client_req_index);
      }
    }
    else
    {
      mmgsdi_util_free_client_request_table_index(client_req_index);
    }
  }
  return build_status;
} /* mmgsdi_process_client_dereg_req */


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_CLIENT_DEREG_CNF

   DESCRIPTION:
     This function will process the client ID deregistration confirmation

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_client_dereg_cnf(
  const mmgsdi_client_id_reg_rsp_type   * client_rsp_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_request_header_type         req_header;
  int32                              index            = 0;
  uim_cmd_type                      *uim_cmd_ptr      = NULL;
  mmgsdi_client_req_extra_info_type *extra_param_ptr  = NULL;
  mmgsdi_sw_status_type              status_word;

  /* status word is not applicable in client dereg processing */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  /* Check input pointers */
  MMGSDIUTIL_RETURN_IF_NULL(client_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = client_rsp_ptr->rsp_data_index;

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
    Build confirmation message based on Client Registration or Client Event
    Callback Registration
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cnf_build_and_queue(client_rsp_ptr->status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE, /* status word present
                                                       flag */
                                             status_word);

  return mmgsdi_status;
} /* mmgsdi_process_client_dereg_cnf */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_TP_DOWNLOAD_COMPLETE

DESCRIPTION
  This function performs the actions required after TP download completes

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_tp_download_complete(
  mmgsdi_tp_dl_complete_req_type * req_ptr
)
{
#ifdef FEATURE_GSTK
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_ERROR;
  int32                               index           = 0;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /*----------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   -----------------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    NULL,
                    NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index,
                    MMGSDI_TP_DOWNLOAD_COMPLETE_REQ,
                    mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
#endif /* FEATURE_GSTK */
} /* mmgsdi_process_tp_download_complete */


/*===========================================================================
FUNCTION MMGSDI_BUILD_RECOVERY_IND_REQ

DESCRIPTION
  This function builds RECOVERY IND command and queue it

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
)
{
  mmgsdi_return_enum_type        mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_task_cmd_type          *task_cmd_ptr      = NULL;
  mmgsdi_recovery_ind_req_type  *msg_ptr           = NULL;
  int32                          total_mem_len     = 0;
  uint32                         task_cmd_len      = 0;
  uint8                          client_cnt        = 0;
  uint8                          session_cnt       = 0;
  int32                          num_sessions      = 0;
  uint16                         close_session_cnt = 0;
  mmgsdi_session_info_type      *session_info_ptr  = NULL;
  int32                          var_len           = 0;
  int32                          temp_mem          = 0;

  UIM_MSG_HIGH_2("MMGSDI BUILD RECOVERY IND REQ : slot id = 0x%x protocol = 0x%x",
                 slot_id, protocol);

  /*if protocol is UICC get total number of sessions to be closed,
    as mmgsdi provides recovery data in UIM context so it  is required to
    determine sessions to be closed at same time.*/
  if(protocol == MMGSDI_UICC)
  {
    for(client_cnt = 0; client_cnt < MMGSDI_MAX_CLIENT_INFO; client_cnt++)
    {
      if(mmgsdi_client_id_reg_table[client_cnt] == NULL)
      {
        continue;
      }

      num_sessions = mmgsdi_client_id_reg_table[client_cnt]->num_session;

      for(session_cnt = 0; (num_sessions > 0) && (session_cnt < MMGSDI_MAX_SESSION_INFO); session_cnt++)
      {
        session_info_ptr =
          mmgsdi_client_id_reg_table[client_cnt]->session_info_ptr[session_cnt];

        if(session_info_ptr == NULL)
        {
          /*Invalid Session, Continue to next*/
          continue;
        }

        num_sessions--;

        /*Close the session if it is a non provisioning session and
          APDU streaming has been done*/
        if((session_info_ptr != NULL) &&
           mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) &&
           (session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == slot_id) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_state ==
              MMGSDI_CHANNEL_STATE_APDU_STREAMING_DONE))
        {
          UIM_MSG_HIGH_2("Session to be closed: session id 0x%x channel id 0x%x",
                         session_info_ptr->session_id,
                         mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id);
          close_session_cnt++;
        }
      }
    }

    /*number of sessions to be closed are 0, no need to queue command*/
    if(close_session_cnt == 0)
    {
      UIM_MSG_HIGH_0("On UICC, number of sessions to be closed is 0, dont queue cmd");
      return MMGSDI_SUCCESS;
    }
  }

  var_len = close_session_cnt * sizeof(mmgsdi_session_id_type);
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + var_len;

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                     task_cmd_len);
  if(task_cmd_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_util_align_mem(&task_cmd_ptr->cmd.cmd.recovery_ind_req,
                        MMGSDI_RECOVERY_IND_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.recovery_ind_req;

  /*---------------------------------------------------------------------------
       Populating mmgsdi_recovery_ind_req_type header
        1) Client ID
        2) Request TYPE
        3) Client Data Pointer
        4) Response CallBack
        5) Payload len and request len will be populated after the content
            has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_RECOVERY_IND_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.request_type      = MMGSDI_RECOVERY_IND_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_RECOVERY_IND_REQ;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = NULL;
  msg_ptr->request_header.slot_id           = slot_id;

  /*Copy session id of all the session to be closed if protocol is UICC*/
  if(protocol == MMGSDI_UICC)
  {
    msg_ptr->num_of_sessions = close_session_cnt;
    close_session_cnt = 0;
    for(client_cnt = 0; client_cnt < MMGSDI_MAX_CLIENT_INFO; client_cnt++)
    {
      if(mmgsdi_client_id_reg_table[client_cnt] == NULL)
      {
        continue;
      }

      num_sessions = mmgsdi_client_id_reg_table[client_cnt]->num_session;

      for(session_cnt = 0; (num_sessions > 0) && (session_cnt < MMGSDI_MAX_SESSION_INFO); session_cnt++)
      {
        session_info_ptr =
          mmgsdi_client_id_reg_table[client_cnt]->session_info_ptr[session_cnt];

        if(session_info_ptr == NULL)
        {
          /*Invalid Session, Continue to next*/
          continue;
        }

        num_sessions--;

        if((session_info_ptr != NULL) &&
            mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) &&
            (session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
            (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
            (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == slot_id) &&
            (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_state ==
              MMGSDI_CHANNEL_STATE_APDU_STREAMING_DONE))
        {
          mmgsdi_memscpy(&msg_ptr->session_id_ptr[close_session_cnt],
                         var_len - (close_session_cnt * sizeof(mmgsdi_session_id_type)),
                         &session_info_ptr->session_id,
                         sizeof(mmgsdi_session_id_type));
          close_session_cnt++;
        }
      }
    }
  }
  else if(protocol == MMGSDI_ICC)
  {
    /*For ICC card, it is good to determine the sessions to be closed as late
      as possible. As mmgsdi handle response of any session related command to
      card first and then process recovery indication cmd, processing of
      response might change state of channel. if determination of sessions
      is done here we might end up in not closing sessions i.e. when response
      of manage channel cmd is in mmgsdi queue and recovery triggered, sessions
      does not have valid channel info ptr as it is assigned only when manage
      channel response is handled. So we will not close the sessions but
      respective channels will not be recovered*/
    msg_ptr->num_of_sessions = 0;
    msg_ptr->session_id_ptr  = NULL;
  }

  /*---------------------------------------------------------------------------
       Calculate the payload len and request len
    ---------------------------------------------------------------------------*/
  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len =
  total_mem_len - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* Put on mmgsdi high priority queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
           onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_RECOVERY_IND_REQ command status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_build_recovery_ind_req */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_RECOVERY_IND_REQ

DESCRIPTION
  This function closes the non prov session and notify the clients as part of
  recovery procedure.
  ICC Protocol: Close all the non prov sessions and don't notify clients if
                session activation/deactivation is in progress.
  UICC Protocol: if streaming is done close non prov sessions and notify the
                client

DEPENDENCIES
  None

LIMITATIONS:
     None

RETURN VALUE
  mmgsdi_return_enum_type
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_recovery_ind_req(
  mmgsdi_recovery_ind_req_type* req_ptr,
  mmgsdi_protocol_enum_type     protocol
)
{
  mmgsdi_return_enum_type             mmgsdi_status     = MMGSDI_ERROR;
  uint8                               client_cnt        = 0;
  uint8                               session_cnt       = 0;
  mmgsdi_session_info_type           *session_info_ptr  = NULL;
  mmgsdi_session_id_type              session_id        = 0;
  int32                               session_index     = 0;
  int32                               client_index      = 0;
  int32                               index             = 0;
  int32                               num_sessions      = 0;
  uint32                              session_close_cnt = 0;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  if(protocol == MMGSDI_ICC)
  {
    /*if the protocol is ICC all the sessions which has valid channel info ptr
    (channel has been opened) are required to be closed. Identification of
    sessions to be closed being done in mmgsdi context, not in UIM context.
    Because when response of manage channel cmd is in mmgsdi queue, session does not have
    valid channel info ptr as it is assigned only when response is handled. In this scenerio if
    recovery happens and identification is done in UIM context, we will end up
    in not closing these sessions */
    for(client_cnt = 0; client_cnt < MMGSDI_MAX_CLIENT_INFO; client_cnt++)
    {
      if(mmgsdi_client_id_reg_table[client_cnt] == NULL)
      {
        continue;
      }

      num_sessions = mmgsdi_client_id_reg_table[client_cnt]->num_session;

      for(session_cnt = 0; (num_sessions > 0) && (session_cnt < MMGSDI_MAX_SESSION_INFO); session_cnt++)
      {
        session_info_ptr =
          mmgsdi_client_id_reg_table[client_cnt]->session_info_ptr[session_cnt];

        if(session_info_ptr == NULL)
        {
          /*Invalid Session, Continue to next*/
          continue;
        }
        num_sessions--;

        if((session_info_ptr != NULL) &&
           mmgsdi_util_is_non_prov_session(session_info_ptr->session_type) &&
           (session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id
              == req_ptr->request_header.slot_id))
        {
          /*if refresh is in progress on this session, first stop the refresh*/
          mmgsdi_status = mmgsdi_util_stop_refresh(
                            session_info_ptr->session_id,
                            req_ptr->request_header.slot_id);
          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
            /*Print error*/
            UIM_MSG_ERR_3("mmgsdi_util_stop_refresh failed session id 0x%x slot id 0x%x status 0x%x",
                          session_info_ptr->session_id,
                          req_ptr->request_header.slot_id,
                          mmgsdi_status);
          }

          /*close the session and notify the client*/
          if((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_state ==
                MMGSDI_CHANNEL_STATE_READY) ||
             (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_state ==
                MMGSDI_CHANNEL_STATE_APDU_STREAMING_DONE))
          {
            mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                              session_info_ptr->session_id,
                              &client_index,
                              &session_index);
            /*Notify the client only when channel has been opened*/
            if(mmgsdi_status == MMGSDI_SUCCESS)
            {
              /* Update the session closure cause. The session closed event
                 broadcasts the cause */
              if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL)
              {
                mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->session_closed_cause =
                  MMGSDI_SESSION_CLOSED_CAUSE_RECOVERY;
              }
              mmgsdi_util_session_close_cleanup_and_notify(client_index, session_index);
            }
            else
            {
              /*Print error*/
              UIM_MSG_ERR_2("mmgsdi_util_get_client_and_session_index failed session id 0x%x status 0x%x",
                            session_info_ptr->session_id, mmgsdi_status);
            }
          }
          else
          {
            /*Don't notify the client when session activation/deactivation is
              in progress*/
            mmgsdi_status = mmgsdi_session_release_info(session_info_ptr,
                              MMGSDI_SESSION_CLOSE_REQ);
          }
        }
      }
    }
  }
  else if(protocol == MMGSDI_UICC)
  {
    if(req_ptr->num_of_sessions == 0 || req_ptr->session_id_ptr == NULL)
    {
      /*Print error*/
      UIM_MSG_ERR_2("number of sessions 0x%x session id ptr 0x%x",
                    req_ptr->num_of_sessions, req_ptr->session_id_ptr);
      return MMGSDI_ERROR;
    }

    for(session_close_cnt = 0; session_close_cnt < req_ptr->num_of_sessions;
        session_close_cnt++)
    {
      mmgsdi_memscpy(&session_id, sizeof(session_id),
                     &req_ptr->session_id_ptr[session_close_cnt],
                     sizeof(mmgsdi_session_id_type));

      /*if refresh is in progress on this session, first stop the refresh*/
      mmgsdi_status = mmgsdi_util_stop_refresh(session_id,
                                               req_ptr->request_header.slot_id);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        /*Print error*/
        UIM_MSG_ERR_3("mmgsdi_util_stop_refresh failed session id 0x%x slot id 0x%x status 0x%x",
                      session_id,
                      req_ptr->request_header.slot_id,
                      mmgsdi_status);
      }

      /*Send session close event and close the session*/
      mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                         session_id,
                         &client_index,
                         &session_index);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        if(mmgsdi_client_id_reg_table[client_index] == NULL)
        {
          UIM_MSG_HIGH_1("No valid client at index: 0x%x", client_index);
          continue;
        }
        session_info_ptr = mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];
        if(session_info_ptr == NULL)
        {
          UIM_MSG_HIGH_0("Session_info_ptr is NULL");
          continue;
        }

        /* Update the session closure cause. The session closed event
           broadcasts the cause */
        if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == req_ptr->request_header.slot_id) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL) &&
           (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->session_closed_cause !=
              MMGSDI_SESSION_CLOSED_CAUSE_RECOVERY))
        {
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->session_closed_cause
            = MMGSDI_SESSION_CLOSED_CAUSE_RECOVERY;
        }
        mmgsdi_util_session_close_cleanup_and_notify(client_index, session_index);
      }
    }
  }

  /*----------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   -----------------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    NULL,
                    NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index,
                    MMGSDI_RECOVERY_IND_REQ,
                    mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_process_recovery_ind_req */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_TP_DL_COMPLETE_RESPONSE

DESCRIPTION
  This function performs the actions required after TP download completes

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_tp_dl_complete_response(
  const mmgsdi_uim_report_rsp_type  * rsp_ptr
)
{
#ifdef FEATURE_GSTK
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type              * slot_data_ptr   = NULL;
  gstk_slot_id_enum_type               gstk_slot       = GSTK_SLOT_ID_MAX;
  mmgsdi_slot_id_enum_type             sess_slot       = MMGSDI_MAX_SLOT_ID_ENUM;
  int32                                index           = 0x00;
  int32                                app_index       = 0x00;
  mmgsdi_request_header_type           req_header;
  uim_cmd_type                       * uim_cmd_ptr     = NULL;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr = NULL;
  mmgsdi_session_id_type               session_id_1x   = 0x00;
  mmgsdi_session_id_type               session_id_gw   = 0x00;
  mmgsdi_int_app_info_type           * app_info_ptr_gw = NULL;
  mmgsdi_int_app_info_type           * app_info_ptr_1x = NULL;
  uim_slot_type                        uim_slot        = UIM_SLOT_NONE;

  MMGSDIUTIL_RETURN_IF_NULL_2(mmgsdi_generic_data_ptr, rsp_ptr);

  if(rsp_ptr->mmgsdi_error_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Unsuccessful TP 0x%x", rsp_ptr->mmgsdi_error_status);
    return MMGSDI_ERROR;
  }

  index = rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info(index,
                                                            &req_header,
                                                            &uim_cmd_ptr,
                                                            &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  UIM_MSG_HIGH_1("TP Download Complete Response for slot 0x%x",
                 req_header.slot_id);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(req_header.slot_id, MMGSDI_SLOT_1, MMGSDI_SLOT_3);

  (void)mmgsdi_util_convert_uim_slot(req_header.slot_id, &uim_slot);

  UIM_MSG_HIGH_1("TP RSP: mmgsdi_state = 0x%x", slot_data_ptr->mmgsdi_tp_state);

  if(((slot_data_ptr->mmgsdi_tp_state != MMGSDI_TP_STATE_NOT_DONE) ||
       (slot_data_ptr->mmgsdi_tp_state != MMGSDI_TP_STATE_DONE))   &&
      (slot_data_ptr->tp_ref_count    == req_header.client_data))
  {
    if(slot_data_ptr->protocol == MMGSDI_UICC)
    {
      /* If TP was not sent to card, indicate TP state as not needed */
      if(slot_data_ptr->mmgsdi_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)
      {
        slot_data_ptr->mmgsdi_tp_state = MMGSDI_TP_STATE_NOT_NEEDED;
      }
      else
      {     
        slot_data_ptr->mmgsdi_tp_state = MMGSDI_TP_STATE_DONE;
      }

      /* For UICC we are still in NOT_INIT and will move to CARD_INSRETED when
      we recieve CARD_INSTERED  */

      /* Invoke PSM to perform special processing if early init was done on slot1*/
      if (req_header.slot_id == MMGSDI_SLOT_1 &&
          mmgsdi_psm_get_status_early_init())
      {
        UIM_MSG_HIGH_0("TP DL Done, trigger in PSM init for real card");
        (void)mmgsdi_psm_handle_card_init_post_tp_dl();

        /* As part of PSM handling, USIM and/or CSIM init is done or
           if card mismatch, reset is triggered which further triggers a new link est
           Nothing to process beyond this */
        return MMGSDI_SUCCESS;
      }

      if(slot_data_ptr->mmgsdi_state == MMGSDI_STATE_NOT_INIT)
      {
        boolean    fail_reason_card_error = FALSE;
        boolean    ef_dir_not_found       = FALSE;

        /* Check if there are any valid UICC applications on the card */
        mmgsdi_status = mmgsdi_uicc_get_available_aids(req_header.slot_id,
                                                       slot_data_ptr,
                                                       &app_index,
                                                       &fail_reason_card_error,
                                                       &ef_dir_not_found);

        /* If there are valid UICC apps found... */
        if ((mmgsdi_status == MMGSDI_SUCCESS) && (app_index > 0))
        {
#ifdef FEATURE_MMGSDI_3GPP
          if (mmgsdi_util_get_usim_apps(req_header.slot_id))
          {
            slot_data_ptr->card_apps_available |= MMGSDI_USIM_APP_MASK;
          }
#endif /* FEATURE_MMGSDI_3GPP */
#ifdef FEATURE_MMGSDI_3GPP2
          if (mmgsdi_util_get_csim_apps(req_header.slot_id))
          {
            slot_data_ptr->card_apps_available |= MMGSDI_CSIM_APP_MASK;
            nvruim_set_uim_dir_present(NVRUIM_CDMA_SUBSCRIP_PRESENT);
          }
#endif /* FEATURE_MMGSDI_3GPP2 */
          UIM_MSG_HIGH_3("No. of Apps present in UICC is 0x%x, USIM present: %x, CSIM present: %x",
                         app_index,
                         slot_data_ptr->card_apps_available & MMGSDI_USIM_APP_MASK,
                         slot_data_ptr->card_apps_available & MMGSDI_CSIM_APP_MASK);
        }

        /* Some eUICCs do not have a correct information in the ATR
           regarding eUICC support. In such cases, slot_data_ptr->is_euicc
           could be set to FALSE. We need to find out if it is really an
           eUICC or not by sending a SELECT on ISDR. If SELECT on ISDR
           succeeds, it is an eUICC, otherwise not */
        if(!slot_data_ptr->is_euicc && ef_dir_not_found)
        {
          boolean is_isdr_found     = FALSE;

          if(MMGSDI_SUCCESS == mmgsdi_euicc_is_isdr_found(req_header.slot_id,
                                                          &is_isdr_found) &&
             is_isdr_found)
          {
            slot_data_ptr->is_euicc = TRUE;
          }
        }

        /* If valid USIM/CSIM apps are available on the card then check for
           profile info global. If the profile info global is NULL then it is
           expected that the card does not support multiple profiles. In this
           case, send the card inserted event. Otherwise, if the card
           supports to provide the active profile then we first determine the
           current active profile present in the card and set it to REGULAR
           if it is in EMERGENCY. If the card does not support to provide the
           current active profile then we blindly send the set sim profile
           request with requested profile type as REGULAR. Once the switching
           is done, we make the reg_profile_detected flag in profile info global
           to TRUE so that in the subsequent card resets we dont need
           to determine the profile.
           If the card is eUICC, we need to send card_inserted irrespective of
           any default profile present or not which means even if EF-DIR read
           fails (num_app == 0) we have to publish card_inserted */
        if (slot_data_ptr->card_apps_available != 0x00 || slot_data_ptr->is_euicc)
        {
          boolean send_card_inserted = TRUE;
#ifdef FEATURE_UIM_MULTI_PROFILE
          if(req_header.slot_id == MMGSDI_SLOT_1)
          {
            send_card_inserted = mmgsdi_multi_profile_set_regular_profile(req_header.slot_id);
          }
#endif /* FEATURE_UIM_MULTI_PROFILE */

          if(send_card_inserted)
          {
            UIM_MSG_HIGH_1("MMGSDI_CARD_INSERTED for SLOT 0x%x", req_header.slot_id);
            mmgsdi_evt_build_and_send_card_inserted(TRUE,
                                                    (mmgsdi_client_id_type)0,
                                                    req_header.slot_id);
          }
        }
        /* If fail_reason_card_error is TRUE, it means UIMDRV has not processed
           this cmd because of some internal error or card_error. In that case,
           there is no need to switch the protocol */
        else if (fail_reason_card_error == FALSE)
        {
          UIM_MSG_HIGH_1("No valid UICC app. found on slot 0x%x, switching protocol to ICC",
                         req_header.slot_id);

          mmgsdi_util_preinit_mmgsdi((mmgsdi_session_id_type)0,
                                     MMGSDI_APP_NONE,
                                     MMGSDI_REFRESH_RESET,
                                     req_header.slot_id,
                                     FALSE);

          /* Switch card protocol to ICC */
          mmgsdi_status = mmgsdi_gen_card_reset_sync(req_header.slot_id, UIM_ICC);
        }
      }
    }
    else if((slot_data_ptr->protocol == MMGSDI_ICC) &&
            (slot_data_ptr->mmgsdi_state == MMGSDI_STATE_CARD_INSERTED))
    {
      gstk_slot = mmgsdi_util_convert_gstk_slot(req_header.slot_id);
      if(gstk_slot != GSTK_SLOT_ID_MAX)
      {
        gstk_gsdi_set_fetch_status(gstk_slot, TRUE);
      }

      /* Find all the apps which needs activation from this slot
         Can be both GSM and RUIM or GSM alone or RUIM alone*/
      session_id_gw = mmgsdi_generic_data_ptr->pri_gw_session_id;
      mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_id_gw,
                        &sess_slot, NULL, &app_info_ptr_gw, NULL, NULL, NULL);
      if(((app_info_ptr_gw != NULL) && (mmgsdi_status == MMGSDI_SUCCESS)) ||
         ((app_info_ptr_gw == NULL) && (mmgsdi_status != MMGSDI_SUCCESS)))
      {
        /* sess_slot will be MMGSDI_MAX_SLOT_ID_ENUM when there is no associated App */
        if(sess_slot != req_header.slot_id)
        {
          session_id_gw = mmgsdi_generic_data_ptr->sec_gw_session_id;
          mmgsdi_status = mmgsdi_util_get_session_app_info(
                            session_id_gw,
                            &sess_slot, NULL, &app_info_ptr_gw, NULL, NULL, NULL);
          if(((app_info_ptr_gw != NULL) && (mmgsdi_status == MMGSDI_SUCCESS)) ||
             ((app_info_ptr_gw == NULL) && (mmgsdi_status != MMGSDI_SUCCESS)))
          {
            /* sess_slot will be MMGSDI_MAX_SLOT_ID_ENUM when there is no associated App */
            if(sess_slot != req_header.slot_id)
            {
              session_id_gw = mmgsdi_generic_data_ptr->ter_gw_session_id;
              mmgsdi_status = mmgsdi_util_get_session_app_info(
                                session_id_gw,
                                &sess_slot, NULL, &app_info_ptr_gw, NULL, NULL, NULL);

              if((app_info_ptr_gw == NULL) && (mmgsdi_status != MMGSDI_SUCCESS))
              {
                UIM_MSG_ERR_1("Could not retrieve GW app info for the slot 0x%x",
                              req_header.slot_id);
              }
            }
          }
        }
      }

      session_id_1x = mmgsdi_generic_data_ptr->pri_1x_session_id;
      mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_id_1x,
                        &sess_slot, NULL, &app_info_ptr_1x, NULL, NULL, NULL);
      if(((app_info_ptr_1x != NULL) && (mmgsdi_status == MMGSDI_SUCCESS)) ||
         ((app_info_ptr_1x == NULL) && (mmgsdi_status != MMGSDI_SUCCESS)))
      {
        /* sess_slot will be MMGSDI_MAX_SLOT_ID_ENUM when there is no associated App */
        if(sess_slot != req_header.slot_id)
        {
          session_id_1x = mmgsdi_generic_data_ptr->sec_1x_session_id;
          mmgsdi_status = mmgsdi_util_get_session_app_info(
                            session_id_1x,
                            &sess_slot, NULL, &app_info_ptr_1x, NULL, NULL, NULL);
          if(((app_info_ptr_1x != NULL) && (mmgsdi_status == MMGSDI_SUCCESS)) ||
             ((app_info_ptr_1x == NULL) && (mmgsdi_status != MMGSDI_SUCCESS)))
          {
            /* sess_slot will be MMGSDI_MAX_SLOT_ID_ENUM when there is no associated App */
            if(sess_slot != req_header.slot_id)
            {
              session_id_1x = mmgsdi_generic_data_ptr->ter_1x_session_id;
              mmgsdi_status = mmgsdi_util_get_session_app_info(
                                session_id_1x,
                                &sess_slot, NULL, &app_info_ptr_1x, NULL, NULL, NULL);

              if((app_info_ptr_1x == NULL) && (mmgsdi_status != MMGSDI_SUCCESS))
              {
                UIM_MSG_ERR_1("Could not retrieve 1X app info for the slot 0x%x",
                              req_header.slot_id);
              }
            }
          }
        }
      }

      if(app_info_ptr_gw != NULL)
      {
        UIM_MSG_HIGH_1("GSM App TP state 0x%x", app_info_ptr_gw->app_tp_state);
        if(((app_info_ptr_gw->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_OK)      ||
            (app_info_ptr_gw->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)) &&
           (app_info_ptr_gw->app_state     == MMGSDI_APP_STATE_READY_FOR_PERSO))
        {
          mmgsdi_set_ens_support_flag(session_id_gw,
                                      req_header.slot_id,
                                      MMGSDI_APP_SIM);

          (void)mmgsdi_card_init_build_internal_pup_req(
                  session_id_gw,
                  req_header.slot_id,
                  MMGSDI_PIN1_EVT,
                  MMGSDI_STATE_CARD_INSERTED,
                  MMGSDI_APP_STATE_READY_FOR_PERSO);
        }
      }

      if(app_info_ptr_1x != NULL)
      {
        UIM_MSG_HIGH_1("CDMA App TP state 0x%x", app_info_ptr_1x->app_tp_state);
        if(((app_info_ptr_1x->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_OK)      ||
            (app_info_ptr_1x->app_tp_state == MMGSDI_TP_STATE_IN_PROGRESS_NOT_OK)) &&
           (app_info_ptr_1x->app_state    == MMGSDI_APP_STATE_READY_FOR_PERSO))
        {
          (void)mmgsdi_card_init_build_internal_pup_req(
                  session_id_1x,
                  req_header.slot_id,
                  MMGSDI_PIN1_EVT,
                  MMGSDI_STATE_CARD_INSERTED,
                  MMGSDI_APP_STATE_READY_FOR_PERSO);
        }
      }

      mmgsdi_status = mmgsdi_util_set_tp_states(slot_data_ptr, app_info_ptr_gw, app_info_ptr_1x);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("mmgsdi_util_set_tp_states returned 0x%x", mmgsdi_status);
      }
    }
    else
    {
      UIM_MSG_ERR_1("Unknown Slot Protocol: 0x%x", slot_data_ptr->protocol);
    }
  }
  else
  {
    UIM_MSG_ERR_3("unexpected TP response: expected 0x%x, received 0x%x, state 0x%x",
                  slot_data_ptr->tp_ref_count,
                  req_header.client_data,
                  slot_data_ptr->mmgsdi_tp_state);
  }
  return MMGSDI_SUCCESS;
#endif /* FEATURE_GSTK */
} /* mmgsdi_process_tp_dl_complete_response */


/*===========================================================================
FUNCTION MMGSDI_BUILD_REFRESH_REQ

DESCRIPTION
  This API is used to queue a REFRESH request to MMGSDI

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_build_refresh_req(
    mmgsdi_client_id_type           client_id,
    mmgsdi_session_id_type          session_id,
    mmgsdi_slot_id_enum_type        card_slot,
    const mmgsdi_refresh_req_type * req_ptr,
    boolean                         register_for_steering,
    mmgsdi_callback_type            response_cb_ptr,
    mmgsdi_client_data_type         client_ref,
    boolean                         requeue_flag
)
{
  mmgsdi_task_cmd_type     * task_cmd_ptr    = NULL;
  mmgsdi_return_enum_type    mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_refresh_req_type *  msg_ptr         = NULL;
  int32                      total_mem_len   = 0;
  uint32                     var_len         = 0;
  int32                      temp_mem        = 0;
  uint32                     task_cmd_len    = 0;
  mmgsdi_cmd_enum_type       mmgsdi_orig_req = MMGSDI_REFRESH_REQ;
  uint32                     i               = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response Callback Pointer Check
    2) Verify the Params provided are within the valid range
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(response_cb_ptr, req_ptr);

  switch(req_ptr->refresh_req_type)
  {
    case MMGSDI_REFRESH_ORIG_REFRESH_REQ:
      if(req_ptr->refresh.card_req.refresh_mode ==
                                   MMGSDI_REFRESH_STEERING_OF_ROAMING)
      {
        var_len = int32touint32(req_ptr->refresh.card_req.plmn_list.data_len);
      }
      else
      {
        var_len = int32touint32(req_ptr->refresh.card_req.refresh_files.data_len);
      }
      break;

    case MMGSDI_REFRESH_ORIG_REG_REQ:
    case MMGSDI_REFRESH_ORIG_DEREG_REQ:
      /* This MACRO checks if client sends both file enums/
         and file paths with reg/dereg request */
      MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(
        req_ptr->refresh.reg_req.refresh_files.file_list_ptr,
        req_ptr->refresh.reg_req.refresh_files.file_path_ptr,
        mmgsdi_status);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        return mmgsdi_status;
      }
      if(req_ptr->refresh.reg_req.refresh_files.file_list_ptr != NULL)
      {
        var_len = req_ptr->refresh.reg_req.refresh_files.num_files *
                     sizeof(mmgsdi_file_enum_type);
      }
      if(req_ptr->refresh.reg_req.refresh_files.file_path_ptr != NULL)
      {
        var_len = req_ptr->refresh.reg_req.refresh_files.num_file_paths *
                     sizeof(mmgsdi_path_type);
      }
      break;

    case MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ:
    case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
    case MMGSDI_REFRESH_ORIG_PROCEED_WITH_REFRESH_REQ:
    case MMGSDI_REFRESH_ORIG_COMPLETE_REQ:
    case MMGSDI_REFRESH_ORIG_TIMER_EXPIRY_REQ:
    case MMGSDI_REFRESH_ORIG_REG_ALL_REQ:
    case MMGSDI_REFRESH_ORIG_DEREG_ALL_REQ:
      var_len = 0;
      break;

    case MMGSDI_REFRESH_ORIG_PUP_REQ:
      var_len = 0;
      mmgsdi_orig_req = MMGSDI_CARD_PUP_REQ;
      break;

    case MMGSDI_REFRESH_ORIG_ENUM_MAX:
      UIM_MSG_ERR_1("Invalid Orig Refresh Enum 0x%x", req_ptr->refresh_req_type);
      return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
   Allocate the memory for this request
   ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + var_len;

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd.refresh_req.refresh.card_req.refresh_mode =
                                          req_ptr->refresh.card_req.refresh_mode;
  task_cmd_ptr->cmd.cmd.refresh_req.refresh_req_type = req_ptr->refresh_req_type;
  if (var_len > 0)
  {
    mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.refresh_req, MMGSDI_REFRESH_REQ);
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.refresh_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_REFRESH_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_REFRESH_REQ;
  msg_ptr->request_header.orig_request_type = mmgsdi_orig_req;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->request_header.request_len = total_mem_len;
  msg_ptr->request_header.payload_len = total_mem_len
                                        - uint32toint32(sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Populate the command details with the information provided by the client
    ------------------------------------------------------------------------*/
  if ( req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_REFRESH_REQ||
       req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_AUTO_REFRESH_REQ ||
       req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_PUP_REQ)
  {
    mmgsdi_memscpy(&msg_ptr->refresh.card_req.aid,
                   sizeof(msg_ptr->refresh.card_req.aid),
                   &req_ptr->refresh.card_req.aid,
                   sizeof(mmgsdi_aid_type));

    msg_ptr->refresh.card_req.refresh_mode =
      req_ptr->refresh.card_req.refresh_mode;

    msg_ptr->refresh.card_req.retry_info.retry_count =
      req_ptr->refresh.card_req.retry_info.retry_count;

    msg_ptr->refresh.card_req.retry_info.retry_limit =
      req_ptr->refresh.card_req.retry_info.retry_limit;

    msg_ptr->refresh.card_req.retry_info.retry_rsp_cb =
      req_ptr->refresh.card_req.retry_info.retry_rsp_cb;

    msg_ptr->refresh.card_req.source =
      req_ptr->refresh.card_req.source;

    if(req_ptr->refresh.card_req.refresh_mode ==
                                 MMGSDI_REFRESH_STEERING_OF_ROAMING)
    {
      if(var_len)
      {
        if(msg_ptr->refresh.card_req.plmn_list.data_ptr != NULL)
        {
          mmgsdi_memscpy(msg_ptr->refresh.card_req.plmn_list.data_ptr,
                         var_len,
                         req_ptr->refresh.card_req.plmn_list.data_ptr,
                         var_len);

          msg_ptr->refresh.card_req.plmn_list.data_len =
            req_ptr->refresh.card_req.plmn_list.data_len;
        }
        else
        {
          UIM_MSG_ERR_0("No data for plmn list");
          MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
          return MMGSDI_ERROR;
        }
      }
    }
    else
    {
      if(var_len)
      {
        mmgsdi_memscpy(msg_ptr->refresh.card_req.refresh_files.data_ptr,
                       var_len,
                       req_ptr->refresh.card_req.refresh_files.data_ptr,
                       var_len);
      }

      msg_ptr->refresh.card_req.num_files =
        req_ptr->refresh.card_req.num_files;

      msg_ptr->refresh.card_req.refresh_files.data_len =
        req_ptr->refresh.card_req.refresh_files.data_len;
    }
  }
  else if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_REG_REQ ||
          req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_DEREG_REQ )
  {
    msg_ptr->refresh.reg_req.steering_of_roaming = register_for_steering;

    if(req_ptr->refresh.reg_req.refresh_files.file_list_ptr != NULL)
    {
      mmgsdi_memscpy(msg_ptr->refresh.reg_req.refresh_files.file_list_ptr,
                     var_len,
                     req_ptr->refresh.reg_req.refresh_files.file_list_ptr,
                     var_len);
      msg_ptr->refresh.reg_req.refresh_files.file_path_ptr = NULL;
    }
    if(req_ptr->refresh.reg_req.refresh_files.file_path_ptr != NULL)
    {
      /* No need to malloc for file path buff here */
      for(i = 0; i < req_ptr->refresh.reg_req.refresh_files.num_file_paths; i++)
      {
        if(req_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_len == 0)
        {
          UIM_MSG_ERR_1("Refresh:Zero path_len at file_path_ptr[0x%x].path_len", i);
          /* Free the task_cmd_ptr */
          MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
          return MMGSDI_ERROR;
        }
        mmgsdi_memscpy(msg_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_buf,
          sizeof(msg_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_buf),
          req_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_buf,
          sizeof(req_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_buf));

        msg_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_len =
          req_ptr->refresh.reg_req.refresh_files.file_path_ptr[i].path_len;
      }
      msg_ptr->refresh.reg_req.refresh_files.file_list_ptr = NULL;
    }
    msg_ptr->refresh.reg_req.refresh_files.num_files =
      req_ptr->refresh.reg_req.refresh_files.num_files;

    msg_ptr->refresh.reg_req.refresh_files.num_file_paths =
      req_ptr->refresh.reg_req.refresh_files.num_file_paths;
    msg_ptr->refresh.reg_req.vote_for_init =
      req_ptr->refresh.reg_req.vote_for_init;
    msg_ptr->refresh.reg_req.ok_to_proceed =
      req_ptr->refresh.reg_req.ok_to_proceed;
  }
  else if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ)
  {
    msg_ptr->refresh.ok_req = req_ptr->refresh.ok_req;
  }
  else if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_COMPLETE_REQ)
  {
    msg_ptr->refresh.pass_fail = req_ptr->refresh.pass_fail;
  }
  else if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_TIMER_EXPIRY_REQ)
  {
    msg_ptr->refresh.timer_count = req_ptr->refresh.timer_count;
  }
  else if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_PROCEED_WITH_REFRESH_REQ)
  {
    msg_ptr->refresh.notify_pdown = req_ptr->refresh.notify_pdown;
  }
  else if(req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_REG_ALL_REQ ||
          req_ptr->refresh_req_type == MMGSDI_REFRESH_ORIG_DEREG_ALL_REQ )
  {
    msg_ptr->refresh.reg_req.reg_all_files =
      req_ptr->refresh.reg_req.reg_all_files;;
    msg_ptr->refresh.reg_req.vote_for_init =
      req_ptr->refresh.reg_req.vote_for_init;
  }

  if (requeue_flag)
  {
    if (q_cnt(&mmgsdi_refresh_cmd_q) >= MMGSDI_REFRESH_CMD_QUEUE_MAX_SIZE)
    {
      UIM_MSG_HIGH_0("refresh command q is full");
      /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
      return MMGSDI_ERROR;
    }
    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

    /* Put the message on the refresh queue */
    q_put(&mmgsdi_refresh_cmd_q, &task_cmd_ptr->link);

    UIM_MSG_HIGH_1("Requeued of refresh command: MMGSDI_REFRESH_REQ type 0x%x",
                   req_ptr->refresh_req_type);

    mmgsdi_status = MMGSDI_SUCCESS;
  }
  else
  {
    /* --------------------------------------------------------------------------
       Queue the command, free the command, and return the status
       ------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
    }

    UIM_MSG_HIGH_2("Queue of MMGSDI command: MMGSDI_REFRESH_REQ type 0x%x Status 0x%x",
                   req_ptr->refresh_req_type, mmgsdi_status);
  }

  return mmgsdi_status;
}/*mmgsdi_build_refresh_req*/


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
)
{
  mmgsdi_task_state_enum_type  task_state    = MMGSDI_TASK_INACTIVE;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;

  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  if(mmgsdi_task != MMGSDI_TASK_MAIN  &&
     mmgsdi_task != MMGSDI_TASK_SLOT1 &&
     mmgsdi_task != MMGSDI_TASK_SLOT2 &&
     mmgsdi_task != MMGSDI_TASK_SLOT3)
  {
    UIM_MSG_ERR_1("Unexpected task 0x%x", mmgsdi_task);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;
  task_state = mmgsdi_task_state[mmgsdi_task];

  do
  {
    /* Do not queue command if GSDI task is not active */
    if(task_state != MMGSDI_TASK_ACTIVE)
    {
      UIM_MSG_ERR_1("MMGSDI Task 0x%x stopped, cannot queue command", mmgsdi_task);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    switch (task_cmd_ptr->cmd.cmd_enum)
    {
      case MMGSDI_UIM_REPORT_RSP:
      case MMGSDI_SAP_REPORT_RSP:
      case MMGSDI_CLIENT_ID_REG_RSP:
      case MMGSDI_CNF:
        (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

        /* Put the message on the queue */
        q_put(&mmgsdi_task_rsp_q[mmgsdi_task], &task_cmd_ptr->link);

        /* Set the command queue signal */
        if(mmgsdi_task == MMGSDI_TASK_MAIN)
        {
           /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_TASK_RSP_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT1)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_TASK_RSP_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT2)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT2_TCB, MMGSDI_TASK_RSP_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT3)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT3_TCB, MMGSDI_TASK_RSP_Q_SIG);
        }

        mmgsdi_status = MMGSDI_SUCCESS;
        break;

      case MMGSDI_NOTIFY_LINK_EST_REQ:
      case MMGSDI_NOTIFY_CARD_ERROR_REQ:
      case MMGSDI_RECOVERY_IND_REQ:
      case MMGSDI_NOTIFY_SIM_BUSY_REQ:
        (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

        /* Put the message on the queue */
        q_put(&mmgsdi_card_status_cmd_q[mmgsdi_task], &task_cmd_ptr->link);

        /* Set the command queue signal */
        if(mmgsdi_task == MMGSDI_TASK_MAIN)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_CARD_STATUS_CMD_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT1)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_CARD_STATUS_CMD_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT2)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT2_TCB, MMGSDI_CARD_STATUS_CMD_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT3)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT3_TCB, MMGSDI_CARD_STATUS_CMD_Q_SIG);
        }

        mmgsdi_status = MMGSDI_SUCCESS;
        break;

      case MMGSDI_INTERNAL_PUP_INIT_REQ:
        (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

        /* Put the message on the queue */
        q_put(&mmgsdi_internal_cmd_q[mmgsdi_task], &task_cmd_ptr->link);

        /* Set the command queue signal */
        if(mmgsdi_task == MMGSDI_TASK_MAIN)
        {
           /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_TASK_CMD_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT1)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_TASK_CMD_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT2)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT2_TCB, MMGSDI_TASK_CMD_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT3)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT3_TCB, MMGSDI_TASK_CMD_Q_SIG);
        }

        mmgsdi_status = MMGSDI_SUCCESS;
        break;

      case MMGSDI_READ_REQ:
        /* If the command is an NVRUIM request or a read of a file required
           to acquire service then place the request in a high priority
           queue else break and place command in regular queue */
        if (((task_cmd_ptr->cmd.cmd.read_req.request_header.client_data &
             MMGSDI_NVRUIM_CLIENT_CMD) == MMGSDI_NVRUIM_CLIENT_CMD) ||
            ((task_cmd_ptr->cmd.cmd.read_req.access.access_method == MMGSDI_EF_ENUM_ACCESS) &&
             (task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_TELECOM_MLPL ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_TELECOM_MSPL ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_USIM_EHPLMN ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_USIM_PLMNWACT ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_USIM_OPLMNWACT ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_USIM_IMSI ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_USIM_AD ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_USIM_7F66_PROP1_RAT ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_CSIM_OTASP_OTAPA_FEATURES ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_CSIM_RUIM_ID ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_CSIM_SPC_STATUS ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_GSM_PLMN ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_GSM_PLMNWACT ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_GSM_OPLMNWACT ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_GSM_IMSI ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_GSM_AD ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_CDMA_OTASP_OTAPA_FEATURES ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_CDMA_RUIM_ID ||
             task_cmd_ptr->cmd.cmd.read_req.access.file.file_enum == MMGSDI_CDMA_SPC_STATUS)))
        {
          if (q_cnt(&mmgsdi_high_pri_cmd_q[mmgsdi_task]) >= MMGSDI_HIGH_PRI_CMD_QUEUE_MAX_SIZE)
          {
            UIM_MSG_HIGH_0("High pri q maxed out, trying regluar q");
            break;
          }
          (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

          /* Put the message on the queue */
          q_put(&mmgsdi_high_pri_cmd_q[mmgsdi_task], &task_cmd_ptr->link);

          /* Set the command queue signal for the GSDI task */
          if(mmgsdi_task == MMGSDI_TASK_MAIN)
          {
             /* Set the command queue signal */
            (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_TASK_CMD_Q_SIG);
          }
          else if(mmgsdi_task == MMGSDI_TASK_SLOT1)
          {
            /* Set the command queue signal */
            (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_TASK_CMD_Q_SIG);
          }
          else if(mmgsdi_task == MMGSDI_TASK_SLOT2)
          {
            /* Set the command queue signal */
            (void) rex_set_sigs(UIM_GSDI_SLOT2_TCB, MMGSDI_TASK_CMD_Q_SIG);
          }
          else if(mmgsdi_task == MMGSDI_TASK_SLOT3)
          {
            /* Set the command queue signal */
            (void) rex_set_sigs(UIM_GSDI_SLOT3_TCB, MMGSDI_TASK_CMD_Q_SIG);
          }

          mmgsdi_status = MMGSDI_SUCCESS;
        }
        break;

      case MMGSDI_WRITE_REQ:
      case MMGSDI_INCREASE_REQ:
      case MMGSDI_GET_FILE_ATTR_REQ:
      case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
      case MMGSDI_SESSION_OPEN_REQ:
      case MMGSDI_SESSION_OPEN_EXT_REQ:
      case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
      case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
        /* Process NVRUIM commands via high priority q */
        /* Note: Bit-wise AND used to check client data since client data is used
           in certain cases to hold addtional session information */
        if ((task_cmd_ptr->cmd.cmd.read_req.request_header.client_data &
             MMGSDI_NVRUIM_CLIENT_CMD) == MMGSDI_NVRUIM_CLIENT_CMD)
        {
          if (q_cnt(&mmgsdi_high_pri_cmd_q[mmgsdi_task]) >= MMGSDI_HIGH_PRI_CMD_QUEUE_MAX_SIZE)
          {
            UIM_MSG_HIGH_0("High pri q maxed out, trying regluar q");
            break;
          }
          (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

          /* Put the message on the queue */
          q_put(&mmgsdi_high_pri_cmd_q[mmgsdi_task], &task_cmd_ptr->link);

          /* Set the command queue signal for the GSDI task */
          if(mmgsdi_task == MMGSDI_TASK_MAIN)
          {
             /* Set the command queue signal */
            (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_TASK_CMD_Q_SIG);
          }
          else if(mmgsdi_task == MMGSDI_TASK_SLOT1)
          {
            /* Set the command queue signal */
            (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_TASK_CMD_Q_SIG);
          }
          else if(mmgsdi_task == MMGSDI_TASK_SLOT2)
          {
            /* Set the command queue signal */
            (void) rex_set_sigs(UIM_GSDI_SLOT2_TCB, MMGSDI_TASK_CMD_Q_SIG);
          }
          else if(mmgsdi_task == MMGSDI_TASK_SLOT3)
          {
            /* Set the command queue signal */
            (void) rex_set_sigs(UIM_GSDI_SLOT3_TCB, MMGSDI_TASK_CMD_Q_SIG);
          }

          mmgsdi_status = MMGSDI_SUCCESS;
        }
        break;

      case MMGSDI_ISIM_AUTH_REQ:
      case MMGSDI_USIM_AUTH_REQ:
      case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
      case MMGSDI_COMPUTE_IP_AUTH_REQ:
      case MMGSDI_RUN_CAVE_REQ:
      case MMGSDI_GENERATE_KEY_VPM_REQ:
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
      case MMGSDI_SESSION_SELECT_AID_REQ:
      case MMGSDI_SESSION_DEACTIVATE_REQ:
        /* If session_deactivate is because of session_close then it should be
           in high priority queue to avoid any request handling in between */
        if (task_cmd_ptr->cmd.cmd_enum == MMGSDI_SESSION_DEACTIVATE_REQ &&
            task_cmd_ptr->cmd.cmd.session_deact_req.in_priority_queue == FALSE)
        {
          break;
        }
        if (q_cnt(&mmgsdi_high_pri_cmd_q[mmgsdi_task]) >= MMGSDI_HIGH_PRI_CMD_QUEUE_MAX_SIZE)
        {
          UIM_MSG_HIGH_0("High pri q maxed out, trying regluar q");
          break;
        }
        (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

        /* Put the message on the queue */
        q_put(&mmgsdi_high_pri_cmd_q[mmgsdi_task], &task_cmd_ptr->link);

        /* Set the command queue signal for the GSDI task */
        if(mmgsdi_task == MMGSDI_TASK_MAIN)
        {
           /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_TASK_CMD_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT1)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_TASK_CMD_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT2)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT2_TCB, MMGSDI_TASK_CMD_Q_SIG);
        }
        else if(mmgsdi_task == MMGSDI_TASK_SLOT3)
        {
          /* Set the command queue signal */
          (void) rex_set_sigs(UIM_GSDI_SLOT3_TCB, MMGSDI_TASK_CMD_Q_SIG);
        }

        mmgsdi_status = MMGSDI_SUCCESS;
        break;

      case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
      case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
      case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
      case MMGSDI_SESSION_CLOSE_REQ:
      case MMGSDI_SEND_APDU_REQ:
      case MMGSDI_SEND_APDU_EXT_REQ:
      case MMGSDI_SAP_CONNECT_REQ:
      case MMGSDI_SAP_DISCONNECT_REQ:
      case MMGSDI_SAP_GET_ATR_REQ:
      case MMGSDI_SAP_SEND_APDU_REQ:
      case MMGSDI_SAP_POWER_ON_REQ:
      case MMGSDI_SAP_POWER_OFF_REQ:
      case MMGSDI_SAP_RESET_REQ:
      case MMGSDI_CARD_PUP_REQ:
      case MMGSDI_CARD_PDOWN_REQ:
      case MMGSDI_CARD_STATUS_REQ:
      case MMGSDI_REFRESH_REQ:
      case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
      case MMGSDI_REHABILITATE_REQ:
      case MMGSDI_INVALIDATE_REQ:
      case MMGSDI_PIN_OPERATION_REQ:
      case MMGSDI_GET_ALL_PIN_STATUS_REQ:
      case MMGSDI_SRV_AVAILABLE_REQ:
      case MMGSDI_JCDMA_GET_CARD_INFO_REQ:
      case MMGSDI_CARD_RESET_REQ:
      case MMGSDI_GET_ATR_REQ:
      case MMGSDI_BCAST_REQ:
      case MMGSDI_SEARCH_REQ:
      case MMGSDI_SESSION_GET_INFO_REQ:
      case MMGSDI_SESSION_STORE_ESN_REQ:
      case MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ:
      case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
      case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
      case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
      case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
      case MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ:
      case MMGSDI_TP_DOWNLOAD_COMPLETE_REQ:
      case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
      case MMGSDI_EONS_SS_EVENT_REQ:
      case MMGSDI_FULL_SERVICE_REQ:
      case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
      case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
      case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
      case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
      case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ:
      case MMGSDI_SET_BUILTIN_PLMN_LIST_REQ:
      case MMGSDI_SESSION_CDMA_AKA_REQ:
      case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
      case MMGSDI_HANDLE_NV_REFRESH_REQ:
      case MMGSDI_MANAGE_CHANNEL_REQ:
      case MMGSDI_SET_SIM_PROFILE_REQ:
      case MMGSDI_GET_SIM_PROFILE_REQ:
      case MMGSDI_START_POTENTIAL_SILENT_FILE_CHANGE_REQ:
        /* Attempt to add req to regular q outside switch case */
        break;

      default:
        UIM_MSG_ERR_1("Unsupported cmd type 0x%x", task_cmd_ptr->cmd.cmd_enum);
        mmgsdi_status = MMGSDI_NOT_SUPPORTED;
        break;
    }

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Command was added in high-priority queue */
      break;
    }
    if (mmgsdi_status == MMGSDI_NOT_SUPPORTED)
    {
      /* Command is not supported */
      break;
    }

    /* Continue with normal priority queue */
    if (q_cnt(&mmgsdi_task_cmd_q[mmgsdi_task]) >= MMGSDI_CMD_QUEUE_MAX_SIZE)
    {
      UIM_MSG_ERR_1("Maximum number of commands have been put onto cmd_q failed command 0x%x",
                    task_cmd_ptr->cmd.cmd_enum);
      overflow_num++;
      if(overflow_num < MMGSDI_MAX_OVERFLOW_SIZE)
      {
        overflow_session_id[overflow_num-1] = task_cmd_ptr->cmd.cmd.request_header.session_id;
        overflow_client_id[overflow_num-1]  = task_cmd_ptr->cmd.cmd.request_header.client_id;
        UIM_MSG_HIGH_3("Total number of commands rejected: %d. client - id_low = 0x%x, id_high = 0x%x",
                       overflow_num,
                       (overflow_client_id[overflow_num-1]&0xFFFFFFFF),
                       (overflow_client_id[overflow_num-1] >> 32 & 0xFFFFFFFF));
        UIM_MSG_HIGH_2("session - id_low = 0x%x, id_high = 0x%x",
                       (overflow_session_id[overflow_num-1]&0xFFFFFFFF),
                       (overflow_session_id[overflow_num-1] >> 32 & 0xFFFFFFFF));
      }

      /* Queue is full */
      mmgsdi_status = MMGSDI_CMD_QUEUE_FULL;
    }
    else
    {
      (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);

      q_put(&mmgsdi_task_cmd_q[mmgsdi_task], &task_cmd_ptr->link);

      /* Set the command queue signal for the GSDI task */
      if(mmgsdi_task == MMGSDI_TASK_MAIN)
      {
         /* Set the command queue signal */
        (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_TASK_CMD_Q_SIG);
      }
      else if(mmgsdi_task == MMGSDI_TASK_SLOT1)
      {
        /* Set the command queue signal */
        (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_TASK_CMD_Q_SIG);
      }
      else if(mmgsdi_task == MMGSDI_TASK_SLOT2)
      {
        /* Set the command queue signal */
        (void) rex_set_sigs(UIM_GSDI_SLOT2_TCB, MMGSDI_TASK_CMD_Q_SIG);
      }
      else if(mmgsdi_task == MMGSDI_TASK_SLOT3)
      {
        /* Set the command queue signal */
        (void) rex_set_sigs(UIM_GSDI_SLOT3_TCB, MMGSDI_TASK_CMD_Q_SIG);
      }

      mmgsdi_status = MMGSDI_SUCCESS;
    }
  }while(0);

  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;
  return mmgsdi_status;
} /* mmgsdi_cmd_ext */


/*===========================================================================
   FUNCTION:      MMGSDI_CMD

   DESCRIPTION:
     This function allows client to put command into the MMGSDI command queue

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
mmgsdi_return_enum_type  mmgsdi_cmd( mmgsdi_task_cmd_type * task_cmd_ptr )
{
  return mmgsdi_cmd_ext(task_cmd_ptr, MMGSDI_TASK_MAIN);
} /* mmgsdi_cmd */


/*===========================================================================
FUNCTION MMGSDI_CLIENT_BUILD_REFRESH_REQ

DESCRIPTION
  This API is used by the Generic SIM Toolkit Task to request that a REFRESH
  be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 8 USIM
  Refresh Modes (as defined by the enum gsdi_refresh_modes_enum_type).

  MMGSDI_REFRESH_NAA_INIT_FULL_FCN      (SIM/USIM)
  MMGSDI_REFRESH_FCN                    (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT_FCN           (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT               (SIM/USIM)
  MMGSDI_REFRESH_RESET                  (SIM/USIM)
  MMGSDI_REFRESH_NAA_APP_RESET          (USIM)
  MMGSDI_REFRESH_3G_SESSION_RESET       (USIM)
  MMGSDI_REFRESH_STEERING_OF_ROAMING    (USIM)

  The API is used for Client based REFRESH requests.

DEPENDENCIES
 None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  Depending on the Mode requested, various events are generated as a result
  of the REFRESH Peformed.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_client_build_refresh_req(
  mmgsdi_client_id_type           client_id,
  mmgsdi_slot_id_enum_type        card_slot,
  const mmgsdi_refresh_req_type * req_ptr,
  boolean                         register_for_steering,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref,
  boolean                         requeue_flag
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type     session_id = MMGSDI_SESSION_ID_ZERO;

  /* Validate the Client ID */
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(
                    client_id,
                    MMGSDI_REFRESH_REQ);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* Validate Slot ID */
  if((card_slot != MMGSDI_SLOT_1) &&
     (card_slot != MMGSDI_SLOT_2) &&
     (card_slot != MMGSDI_SLOT_3))
  {
    if((req_ptr->refresh_req_type ==
        MMGSDI_REFRESH_ORIG_REG_REQ) ||
       (req_ptr->refresh_req_type ==
        MMGSDI_REFRESH_ORIG_DEREG_REQ))
    {
      card_slot = MMGSDI_SLOT_AUTOMATIC;
    }
    else
    {
      UIM_MSG_ERR_2("Slot 0x%x is incorrect for Refresh Req Type 0x%x",
                    card_slot, req_ptr->refresh_req_type);

      return MMGSDI_INCORRECT_PARAMS;
    }
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* req_ptr->request_header.session_id would not be a valid assigned session
     id incase we reached here due to naa_refresh call from GSTK (that is,
     when the card triggered the refresh) */
  if(mmgsdi_util_is_session_id_valid(req_ptr->request_header.session_id) !=
       MMGSDI_SESSION_ID_UNASSIGNED)
  {
    session_id = req_ptr->request_header.session_id;
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_build_refresh_req(client_id,
                                  session_id,
                                  card_slot,
                                  req_ptr,
                                  register_for_steering,
                                  response_cb_ptr,
                                  client_ref,
                                  requeue_flag);

}/* mmgsdi_client_build_refresh_req */


/*===========================================================================
FUNCTION MMGSDI_SESSION_BUILD_REFRESH_REQ

DESCRIPTION
  This API is used by the Generic SIM Toolkit Task to request that a REFRESH
  be performed on the Mobile.  There are 5 GSM SIM Refresh Modes and 8 USIM
  Refresh Modes (as defined by the enum gsdi_refresh_modes_enum_type).

  MMGSDI_REFRESH_NAA_INIT_FULL_FCN      (SIM/USIM)
  MMGSDI_REFRESH_FCN                    (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT_FCN           (SIM/USIM)
  MMGSDI_REFRESH_NAA_INIT               (SIM/USIM)
  MMGSDI_REFRESH_RESET                  (SIM/USIM)
  MMGSDI_REFRESH_NAA_APP_RESET          (USIM)
  MMGSDI_REFRESH_3G_SESSION_RESET       (USIM)
  MMGSDI_REFRESH_STEERING_OF_ROAMING    (USIM)

  The API is used for Session based REFRESH requests.

DEPENDENCIES
 None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  Depending on the Mode requested, various events are generated as a result
  of the REFRESH Peformed.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_build_refresh_req(
  mmgsdi_session_id_type          session_id,
  const mmgsdi_refresh_req_type * req_ptr,
  boolean                         register_for_steering,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;

  /* Validate the Session ID */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_REFRESH_REQ);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Client id and Slot id are populated in GSDI context */
  return mmgsdi_build_refresh_req(0,
                                  session_id,
                                  MMGSDI_SLOT_AUTOMATIC,
                                  req_ptr,
                                  register_for_steering,
                                  response_cb_ptr,
                                  client_ref,
                                  FALSE);

}/* mmgsdi_session_build_refresh_req */


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
)
{
  mmgsdi_aid_type               aid_data;
  mmgsdi_refresh_req_type       req;

  (void)option;
  (void)alpha_string_ptr;
  memset(&aid_data, 0x0, sizeof(mmgsdi_aid_type));
  memset(&req, 0x0, sizeof(mmgsdi_refresh_req_type));

  mmgsdi_memscpy(&req.refresh.card_req.aid,
                 sizeof(req.refresh.card_req.aid),
                 &aid,
                 sizeof(mmgsdi_aid_type));

  if(refresh_mode == MMGSDI_REFRESH_STEERING_OF_ROAMING)
  {
    mmgsdi_memscpy(&req.refresh.card_req.plmn_list,
                   sizeof(req.refresh.card_req.plmn_list),
                   &refresh_data,
                   sizeof(mmgsdi_data_type));
  }
  else
  {
    mmgsdi_memscpy(&req.refresh.card_req.refresh_files,
                   sizeof(req.refresh.card_req.refresh_files),
                   &refresh_data,
                   sizeof(mmgsdi_data_type));
  }

  req.refresh_req_type              = MMGSDI_REFRESH_ORIG_REFRESH_REQ;
  req.refresh.card_req.refresh_mode = refresh_mode;
  req.refresh.card_req.num_files    = num_files;
  req.refresh.card_req.source       = source;

  if(MMGSDI_SUCCESS != mmgsdi_copy_info_to_refresh_data_info(&req,
                                                             card_slot))
  {
    UIM_MSG_HIGH_0("Could not initialize refresh_data_info_ptr");
    return MMGSDI_ERROR;
  }

  return mmgsdi_client_build_refresh_req(client_id,
                                         card_slot,
                                         &req,
                                         FALSE,
                                         response_cb_ptr,
                                         client_ref,
                                         FALSE);
}/*mmgsdi_naa_refresh_ext*/


/*===========================================================================
FUNCTION MMGSDI_GET_ME_CONFIG_PARAM_FROM_NV

DESCRIPTION
This function gets the me capabilities for fdn, bdn and acl.
Reads the nv_item and sets the global data. If the nv_item is not enabled, then
the function writes the defaul value to the nv. If the nv_item could not be
read, a failure is return.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 If nv operation is not succesful, this function returns error and can put
 the me to no card.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_get_me_config_params_from_nv(void)
{
  nv_item_type           * nv_item_ptr = NULL;
  uint32                   config_value   = 0;
  nv_stat_enum_type        nv_status      = NV_DONE_S;
  mmgsdi_nv_item_cache_type mmgsdi_nv_data = {0};

  UIM_MSG_LOW_0("Entered mmgsdi_nv_config_values");

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(nv_item_ptr, sizeof(nv_item_type))

  if(nv_item_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  nv_status = mmgsdi_nv_get_item(NV_MMGSDI_ME_CONFIG_PARAM_I,
                                 &mmgsdi_nv_data,MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
  if ( nv_status == NV_NOTACTIVE_S  )
  {
    UIM_MSG_HIGH_0("NV Item is not initialised, so write it to a default value.");
    nv_item_ptr->mmgsdi_me_config_param = int32touint32(MMGSDI_DEFAULT_ME_CONFIG_VALUE);
    mmgsdi_nv_data.item_value.mmgsdi_me_config_param = MMGSDI_DEFAULT_ME_CONFIG_VALUE;
    nv_status = mmgsdi_nv_put_item(NV_MMGSDI_ME_CONFIG_PARAM_I,
                                   nv_item_ptr,MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
  }

  if(nv_status != NV_DONE_S)
  {

    UIM_MSG_ERR_1("Couldn't get the nv parameter, this should not happen!!! 0x%x",
                  nv_status);
    MMGSDIUTIL_TMC_MEM_FREE(nv_item_ptr );
    return MMGSDI_ERROR;
  }
#endif/* !FEATURE_UIM_TEST_FRAMEWORK*/

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif/*FEATURE_UIM_TEST_FRAMEWORK */

  /* nv read succesful */
  config_value = mmgsdi_nv_data.item_value.mmgsdi_me_config_param;

  if(config_value & MMGSDI_FDN_SUPPORTED_MASK)
  {
    mmgsdi_generic_data_ptr->me_capabilities.me_supports_fdn = TRUE;
  }
  else
  {
    mmgsdi_generic_data_ptr->me_capabilities.me_supports_fdn = FALSE;
    UIM_MSG_HIGH_0("ME doesn't support FDN");
  }

  if(config_value & MMGSDI_BDN_SUPPORTED_MASK)
  {
    mmgsdi_generic_data_ptr->me_capabilities.me_supports_bdn = TRUE;

    UIM_MSG_HIGH_0("ME supports BDN");
  }
  else
  {
    mmgsdi_generic_data_ptr->me_capabilities.me_supports_bdn = FALSE;
    UIM_MSG_HIGH_0("ME doesn't support BDN ");
  }

  if(config_value & MMGSDI_ACL_SUPPORTED_MASK)
  {
    mmgsdi_generic_data_ptr->me_capabilities.me_supports_acl = TRUE;
    UIM_MSG_HIGH_0("ME supports ACL");
  }
  else
  {
    mmgsdi_generic_data_ptr->me_capabilities.me_supports_acl = FALSE;
    UIM_MSG_HIGH_0("ME doesn't support ACL");
  }
  MMGSDIUTIL_TMC_MEM_FREE(nv_item_ptr );
  return MMGSDI_SUCCESS;
} /* mmgsdi_get_me_config_params_from_nv */


/*===========================================================================

FUNCTION MMGSDI_MALLOC

DESCRIPTION
  mmgsdi_malloc calls modem_mem_calloc to request memory from the Modem Heap.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/


/*===========================================================================

FUNCTION MMGSDI_FREE

DESCRIPTION
  The mmgsdi_free frees the pointer from the Modem Heap.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK*/


/*===========================================================================

FUNCTION MMGSDI_PARSE_REFRESH_FILE_LIST

DESCRIPTION
  This function parses the refresh files from refresh registration request to the FORMAT reuired by mmgsdi.

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_parse_refresh_file_list(
  mmgsdi_refresh_file_list_type  refresh_files,
  mmgsdi_refresh_req_type      * req_ptr
)
{
  uint32                  i             = 0;
  uint8                 * temp_path_ptr = NULL;
  int32                   temp_len      = 0;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(refresh_files.file_list_ptr != NULL)
  {
    req_ptr->refresh.reg_req.refresh_files.file_list_ptr =
                                           refresh_files.file_list_ptr;
    req_ptr->refresh.reg_req.refresh_files.num_files =
                                           refresh_files.num_files;

    /* set file path ptr to NULL and num_file_paths to 0 */
    req_ptr->refresh.reg_req.refresh_files.file_path_ptr = NULL;
    req_ptr->refresh.reg_req.refresh_files.num_file_paths = 0;
  }

  /* copy the file path */
  if(refresh_files.file_path_ptr != NULL)
  {
    if(refresh_files.file_path_len < MMGSDI_MIN_EF_PATH_LEN)
    {
      UIM_MSG_ERR_1("Invalid file path len value 0x%x for refresh op.",
                    refresh_files.file_path_len);
      return MMGSDI_INCORRECT_PARAMS;
    }
    /* Assign file path pointer to request structure, allocate temporary
       buffer to account for number of path as first byte */
    /* require this temp to satisfy lint and compilation warning fixes */
    temp_len = uint32toint32((refresh_files.file_path_len + 1) * sizeof(uint8));

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(temp_path_ptr, temp_len);
    if (temp_path_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    /* Determine the number of file path */
    for (i = 0; i < (refresh_files.file_path_len - 1); i+=2)
    {
      if ((refresh_files.file_path_ptr[i] == MMGSDIUTIL_MF_HIGH_BYTE) &&
          (refresh_files.file_path_ptr[i+1] == 0x00))
      {
        temp_path_ptr[0] += 1;
      }
    }

    mmgsdi_memscpy(&temp_path_ptr[1],
                   temp_len - 1,
                   refresh_files.file_path_ptr,
                   refresh_files.file_path_len);

    /* Convert file data into file path array */
    mmgsdi_status = mmgsdi_util_convert_to_refresh_file_paths(
                        refresh_files.file_path_len+1,
                        temp_path_ptr,
                        &req_ptr->refresh.reg_req.refresh_files.file_path_ptr,
                        (uint8*)&req_ptr->refresh.reg_req.refresh_files.num_file_paths);

    MMGSDIUTIL_TMC_MEM_FREE(temp_path_ptr);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Refresh: Could not decode and copy file path");
      return mmgsdi_status;
    }

    /* Set file enum list ptr to NULL and num files to  0*/
    req_ptr->refresh.reg_req.refresh_files.file_list_ptr = NULL;
    req_ptr->refresh.reg_req.refresh_files.num_files = 0;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_parse_refresh_file_list */


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
void mmgsdi_cache_meid_data(void)
{
  mmgsdi_nv_item_cache_type mmgsdi_nv_data = {0};

  if (NV_DONE_S == mmgsdi_nv_get_item(NV_MEID_ME_I,
                                      &mmgsdi_nv_data,
                                      MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC))
  {
    UIM_MSG_HIGH_0("MEID is enabled");
    mmgsdi_data_meid.meid_available = TRUE;
    qw_equ(mmgsdi_data_meid.meid, mmgsdi_nv_data.item_value.meid);
  }
  else
  {
    UIM_MSG_HIGH_0("MEID is not enabled");
    mmgsdi_data_meid.meid_available = FALSE;
  }
} /* mmgsdi_cache_meid_data */


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

mmgsdi_meid_data_type mmgsdi_get_meid(void)
{
  return mmgsdi_data_meid;
} /* mmgsdi_get_meid */


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
)
{
#ifdef FEATURE_MMGSDI_3GPP
  mmgsdi_write_req_type             * req_ptr                     = NULL;
  mmgsdi_write_cnf_type             * cnf_ptr                     = NULL;
  mmgsdi_return_enum_type             act_HPLMN_read_status       = MMGSDI_ERROR;
  mmgsdi_file_enum_type               mmgsdi_spt_filename         = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type               mmgsdi_act_hplmn_filename   = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type               mmgsdi_rpm_version_filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_return_enum_type             mmgsdi_status               = MMGSDI_ERROR;
  mmgsdi_len_type                     spt_file_len                = 0;
  uint32                              data_len                    = 0;
  mmgsdi_data_type                    read_data;
  mmgsdi_cached_nv_item_data          nv_item;
  mmgsdi_get_file_attr_cnf_type      *select_cnf_ptr              = NULL;
  mmgsdi_get_file_attr_req_type      *select_ptr                  = NULL;
  uint8                               mmgsdi_rpm_version_value[MMGSDI_RPM_VERSION_DATA_LEN];
#ifdef FEATURE_LTE
  mcs_modem_cap_return_enum           lte_available               = MCS_MODEM_CAP_UNKNOWN;
  mmgsdi_session_type_enum_type       session_type                = MMGSDI_MAX_SESSION_TYPE_ENUM;
#ifdef LTE_BAND_NUM
  sys_lte_band_mask_e_type            lte_bands                   = SYS_LTE_BAND_MASK_CONST_NONE;
#else
  sys_band_mask_type                  lte_bands                   = 0;
#endif
#endif /*FEATURE_LTE*/
  mmgsdi_nv_context_type              sub_nv_context              = MMGSDI_NV_CONTEXT_INVALID;
#ifdef FEATURE_FEMTO_CSG
  sys_csg_support_e_type              csg_support_status          = SYS_CSG_SUPPORT_NONE;
  uim_common_efs_context_type         efs_context                 = UIM_COMMON_EFS_CONTEXT_MAX;
#endif /* FEATURE_FEMTO_CSG */

  /* check if ens is enabled in nv*/
  if( !mmgsdi_nv_is_ens_enabled(slot))
  {
    UIM_MSG_HIGH_1("ENS_SLOT_ENABLED not enabled for slot 0x%x", slot);
    return;
  }

  memset(&read_data, 0x00, sizeof(mmgsdi_data_type));
  memset(&nv_item, 0x00, sizeof(mmgsdi_cached_nv_item_data));
  gstk_gsdi_continue_task(FALSE);

  switch (app_type)
  {
    case MMGSDI_APP_USIM:
      mmgsdi_spt_filename = MMGSDI_USIM_7F66_PROP1_SPT_TABLE;
      mmgsdi_act_hplmn_filename = MMGSDI_USIM_7F66_PROP1_ACT_HPLMN;
      mmgsdi_rpm_version_filename = MMGSDI_USIM_7F66_PROP1_RPM_VI;
      break;
    case MMGSDI_APP_SIM:
      mmgsdi_spt_filename = MMGSDI_SIM_7F66_PROP1_SPT_TABLE;
      mmgsdi_act_hplmn_filename = MMGSDI_SIM_7F66_PROP1_ACT_HPLMN;
      mmgsdi_rpm_version_filename = MMGSDI_SIM_7F66_PROP1_RPM_VI;
      break;
    default:
      UIM_MSG_ERR_1("mmgsdi_set_ens_support_flag : Unknown App Type 0x%x",
                    app_type);
      gstk_gsdi_continue_task(TRUE);
      return;
  }

  nv_item.nv_item_enum = MMGSDI_NV_RAT_BALANCING_SUPPORT_I;
  mmgsdi_status = mmgsdi_get_cached_nv_item(&nv_item);
  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (nv_item.nv_item_data.uicc_rat_balancing_support))
  {
    /* Send a read req for EF-RAT */
    mmgsdi_status = mmgsdi_card_init_cache_binary(session_id, app_type, slot,
                                                  MMGSDI_USIM_7F66_PROP1_RAT,
                                                  NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_HIGH_1("EF RAT could not be read, status = 0x%x", mmgsdi_status);
    }
  }
  else
  {
    UIM_MSG_HIGH_0("RAT indicator support not available");
  }

  /* Read EF-TST and check it's size is atleast 0x08 (GSDI_SPT_DATA_MIN_LEN) */
  mmgsdi_status = mmgsdi_card_init_cache_binary(session_id, app_type, slot,
                                                mmgsdi_spt_filename, &read_data);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (read_data.data_len < GSDI_SPT_DATA_MIN_LEN))
  {
    UIM_MSG_ERR_1("Could not read EF Terminal Support Table:status = 0x%x",
                  mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_data.data_ptr);
    gstk_gsdi_continue_task(TRUE);
    return;
  }

   /* Save the spt file len */
  spt_file_len = read_data.data_len;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_data.data_ptr);
  read_data.data_ptr = NULL;

  nv_item.nv_item_enum = MMGSDI_NV_ACTING_HPLMN_RESCAN_FEATURE_SUPPORT_I;
  mmgsdi_status = mmgsdi_get_cached_nv_item(&nv_item);
  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (nv_item.nv_item_data.acting_hplmn_rescan_feature_support))
  {
    mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,app_type, slot,
                                                  mmgsdi_act_hplmn_filename,
                                                  &read_data);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("EF Acting HPLMN could not be read:status = 0x%x",
                    mmgsdi_status);
    }
  }
  else
  {
    UIM_MSG_HIGH_0("Acting HPLMN rescan support not available");
    mmgsdi_status = MMGSDI_ERROR;
  }

  act_HPLMN_read_status = mmgsdi_status;
  if(act_HPLMN_read_status == MMGSDI_SUCCESS &&
     read_data.data_ptr != NULL)
  {
#if defined(FEATURE_WCDMA) && defined(FEATURE_GSM)
    if(read_data.data_len >= 0x04)
    {
      if(read_data.data_ptr[0] == 0x01 && read_data.data_ptr[1] == 0x01 &&
         read_data.data_ptr[2] == 0x01 && read_data.data_ptr[3] == 0x01)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_data.data_ptr);
        gstk_gsdi_continue_task(TRUE);
        return;
      }
    }
#else
    if(read_data.data_len >= 0x03)
    {
      if(read_data.data_ptr[0] == 0x01 && read_data.data_ptr[1] == 0x01 &&
         read_data.data_ptr[2] == 0x01)
      {
        /* Data is already Set to 1. No need to write anything */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_data.data_ptr);
        gstk_gsdi_continue_task(TRUE);
        return;
      }
    }
#endif /*defined(FEATURE_WCDMA) && defined(FEATURE_MMGSDI_GSM)*/
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_data.data_ptr);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr,
                                     sizeof(mmgsdi_write_req_type));
  if(req_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Failed to allocate memory for write Request");
    gstk_gsdi_continue_task(TRUE);
    return;
  }

  data_len = int32touint32(spt_file_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr->data.data_ptr, data_len);
  if(req_ptr->data.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    gstk_gsdi_continue_task(TRUE);
    return;
  }

  req_ptr->request_header.session_id         = session_id;
  req_ptr->request_header.slot_id            = slot;
  req_ptr->request_header.request_type       = MMGSDI_WRITE_REQ;
  req_ptr->request_header.orig_request_type  = MMGSDI_WRITE_REQ;
  req_ptr->access.access_method              = MMGSDI_EF_ENUM_ACCESS;
  req_ptr->access.file.file_enum             = mmgsdi_spt_filename;
  req_ptr->rec_num                           = 0;
  req_ptr->offset                            = 0;
  req_ptr->file_type                         = MMGSDI_TRANSPARENT_FILE;
  req_ptr->data.data_len                     = spt_file_len;

  nv_item.nv_item_enum = MMGSDI_NV_SCAN_CONTROL_APPLICATION_SUPPORT_I;
  mmgsdi_status = mmgsdi_get_cached_nv_item(&nv_item);
  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (nv_item.nv_item_data.scan_control_application_support))
  {
    req_ptr->data.data_ptr[0] = 0x01; /* Scan Control Feature. */
  }

  nv_item.nv_item_enum = MMGSDI_NV_NETWORK_SELECTION_MENU_CONTROL_APPLICATION_SUPPORT_I;
  mmgsdi_status = mmgsdi_get_cached_nv_item(&nv_item);
  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (nv_item.nv_item_data.nw_selection_menu_control_application_support))
  {
    req_ptr->data.data_ptr[1] = 0x01; /* Network Selection Menu */
  }

  /* Acting-HPLMN rescan feature */
  nv_item.nv_item_enum = MMGSDI_NV_ACTING_HPLMN_RESCAN_FEATURE_SUPPORT_I;
  mmgsdi_status = mmgsdi_get_cached_nv_item(&nv_item);
  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (nv_item.nv_item_data.acting_hplmn_rescan_feature_support))
  {
    req_ptr->data.data_ptr[2] = (act_HPLMN_read_status == MMGSDI_SUCCESS) ? 0x01 : 0x00;
  }

#if defined(FEATURE_WCDMA) && defined(FEATURE_GSM)
  /*3G Flag*/
  req_ptr->data.data_ptr[3] = 0x01;
#endif /*defined(FEATURE_WCDMA) && defined(FEATURE_GSM)*/

#ifdef FEATURE_LTE
  /* For the time being, LTE is available for GW_PRI_SESSION only */
  if (MMGSDI_SUCCESS == mmgsdi_util_get_session_type(session_id, &session_type, NULL) &&
      MMGSDI_GW_PROV_PRI_SESSION == session_type)
  {
    /* Check whether LTE RAT is available */
    lte_available = mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_LTE);
  }
  UIM_MSG_HIGH_2("session_type=0x%x, LTE available: 0x%x",
                 session_type, lte_available);
#endif /*FEATURE_LTE*/

  nv_item.nv_item_enum = MMGSDI_NV_RAT_BALANCING_SUPPORT_I;
  mmgsdi_status = mmgsdi_get_cached_nv_item(&nv_item);
  if ((mmgsdi_status == MMGSDI_SUCCESS) &&
      (nv_item.nv_item_data.uicc_rat_balancing_support))
  {
    if(app_type == MMGSDI_APP_USIM)
    {
      req_ptr->data.data_ptr[4] = 0x01;
    }
#ifdef FEATURE_LTE
    if(MCS_MODEM_CAP_AVAILABLE == lte_available &&
       MMGSDI_APP_SIM          == app_type)
    {
      req_ptr->data.data_ptr[4] = 0x01;
    }
#endif /*FEATURE_LTE*/
  }

  /* The Byte 6 (BIP Feature) in EF-TST (req_ptr->data.data_ptr[5]) is not
     updated because it is not meant for the ENS but to indicate whether
     a device supports the BIP feature or not. The BIP indicator here is
     meant for telecom applications and not for non-telecom applications such
     as NFC. The operator confirmed that it is not needed at this time. */

#ifdef FEATURE_LTE
  /* Get LTE bands that are enabled */
  policyman_get_hardware_rats_bands(NULL, NULL, &lte_bands, NULL);

  if((MCS_MODEM_CAP_AVAILABLE == lte_available) &&
#ifdef LTE_BAND_NUM
     (!SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_bands))
#else
     (lte_bands != 0)
#endif
    )
  {
#ifdef FEATURE_3GPP_CSFB
    mmgsdi_nv_item_cache_type mmgsdi_nv_data = {0};
    nv_stat_enum_type         nv_status      = NV_NOTALLOC_S;

    /* LTE Data and Voice using CSFB */
    req_ptr->data.data_ptr[6] = 0x02;

    /* LTE Data and Voice using IMS and CSFB if both of the following are true:
       1. IMS is enabled (IMS session based NV item 67218 IMS_ENABLE)
       2. CM NV 66048 VOICE_DOMAIN_PREFERENCE has any of the following values:
            SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY
            SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED
            SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_PREFERRED
     */
    if((mmgsdi_util_get_efs_item_index(session_id, &sub_nv_context)   == MMGSDI_SUCCESS) &&
       (mmgsdi_nv_get_external_feature_status(
         MMGSDI_EXT_FEAT_IMS_CFG_FEATURE_IMS_SUPPORT, sub_nv_context) == MMGSDI_FEATURE_ENABLED) &&
       (mmgsdi_nv_get_external_feature_status(
         MMGSDI_EXT_FEAT_VOLTE_USING_IMS, sub_nv_context)             == MMGSDI_FEATURE_ENABLED))
    {
      req_ptr->data.data_ptr[6] = 0x03;
    }

    if (mmgsdi_util_get_efs_item_index(session_id, &sub_nv_context) == MMGSDI_SUCCESS)
    {
      nv_status = mmgsdi_nv_get_item(NV_SERVICE_DOMAIN_PREF_I, &mmgsdi_nv_data, sub_nv_context);
    }
    if (nv_status == NV_DONE_S)
    {
      /* Information was successfully obtained */
      UIM_MSG_HIGH_1("NV_SERVICE_DOMAIN_PREF_I value: 0x%X",
                     mmgsdi_nv_data.item_value.service_domain_pref.srv_domain);

      switch ( mmgsdi_nv_data.item_value.service_domain_pref.srv_domain )
      {
        case NV_SRV_DOMAIN_PREF_PS_ONLY:
          req_ptr->data.data_ptr[6] = 0x01;
          break;
        case NV_SRV_DOMAIN_PREF_CS_PS:
          /* If CS + PS, check if device supports voice or not */
          memset((unsigned char*)(&mmgsdi_nv_data), 0x00, sizeof(mmgsdi_nv_item_cache_type));
          nv_status = mmgsdi_nv_get_item(NV_DISABLE_CM_CALL_TYPE_I, &mmgsdi_nv_data, sub_nv_context);
          if (nv_status == NV_DONE_S)
          {
            /* Information was successfully obtained */
            UIM_MSG_HIGH_1("NV_DISABLE_CM_CALL_TYPE_I value: 0x%X",
                            mmgsdi_nv_data.item_value.disable_cm_call_type);

            /* Voice call is disabled if bit 0 is set */
            if (( mmgsdi_nv_data.item_value.disable_cm_call_type & 0x01) == 1)
            {
              req_ptr->data.data_ptr[6] = 0x01;
            }
          }
          break;
        default:
          UIM_MSG_HIGH_0("Using default value of 0x02");
          break;
      }
    }
#else
    /* LTE Data Only Support */
    req_ptr->data.data_ptr[6] = 0x01;
#endif /* FEATURE_3GPP_CSFB */
  }
#endif /* FEATURE_LTE */

  /* CSG Support Byte 8 3G CSG Support and Byte 9 LTE CSG Support */
#ifdef FEATURE_FEMTO_CSG
  /* Set default values */
#ifdef FEATURE_DISABLE_CSG_BYDEFAULT
  /* Default value is CSG support disabled */
  req_ptr->data.data_ptr[7] = 0x00;
#else
  /* Default value is CSG support for W is enabled, for LTE is disabled */
  req_ptr->data.data_ptr[7] = 0x01;
#endif /* FEATURE_DISABLE_CSG_BYDEFAULT */

  if (req_ptr->data.data_len > GSDI_SPT_DATA_MIN_LEN)
  {
    req_ptr->data.data_ptr[8] = 0x00;
  }

  /* Read CSG SUPPORT CONFIGURATION (NV 70361) from EFS */
  if ((mmgsdi_util_get_efs_item_context(session_id, &efs_context) == MMGSDI_SUCCESS) &&
      (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_CSG_SUPPORT_CONFIG,
                           UIM_COMMON_EFS_ITEM_FILE_TYPE,
                           efs_context,
                           (uint8 *)&csg_support_status,
                           sizeof(csg_support_status)) ==
       UIM_COMMON_EFS_SUCCESS))
  {
    UIM_MSG_MED_1("csg_support_status=0x%x", csg_support_status);
    switch (csg_support_status)
    {
      case SYS_CSG_SUPPORT_DISABLED:
        req_ptr->data.data_ptr[7] = 0x00;
        break;
      case SYS_CSG_SUPPORT_WCDMA:
        req_ptr->data.data_ptr[7] = 0x01;
        break;
      case SYS_CSG_SUPPORT_WCDMA_LTE:
        req_ptr->data.data_ptr[7] = 0x01;
        if (req_ptr->data.data_len > GSDI_SPT_DATA_MIN_LEN)
        {
          req_ptr->data.data_ptr[8] = 0x01;
        }
        break;
      default:
        break;
    }
  }
#endif /* FEATURE_FEMTO_CSG */

  if(app_type == MMGSDI_APP_USIM)
  {
    mmgsdi_status = mmgsdi_uim_uicc_write_transparent(req_ptr, TRUE, &cnf_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_uim_icc_write_transparent(req_ptr, TRUE, &cnf_ptr);
  }
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("Failed to write to SPT Table: Status = 0x%x", mmgsdi_status);
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr->data.data_ptr);
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);

  do
  {
    /* Before updating the EF RPM Version Implemented, the file must be checked
       that it is present and that the file size is correct as per RPM requirements */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
                                       sizeof(mmgsdi_get_file_attr_req_type));

    if(select_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    }

    select_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
    select_ptr->request_header.session_id        = session_id;
    select_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
    select_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
    select_ptr->access.access_method             = MMGSDI_EF_ENUM_ACCESS;
    select_ptr->access.file.file_enum            = mmgsdi_rpm_version_filename;
    select_ptr->request_header.request_len       =
      uint32toint32(sizeof(mmgsdi_get_file_attr_req_type));
    select_ptr->request_header.payload_len       = 0;
    select_ptr->request_header.slot_id           = slot;
    select_ptr->request_header.client_data       = 0;
    select_ptr->request_header.response_cb       = NULL;
    select_ptr->activate_aid                     = FALSE;
    select_ptr->skip_uicc_arr                    = TRUE;

    if(app_type == MMGSDI_APP_USIM)
    {
      mmgsdi_status = mmgsdi_uim_uicc_select(select_ptr,
                                             TRUE,
                                             &select_cnf_ptr,
                                             TRUE);
    }
    else
    {
      mmgsdi_status = mmgsdi_uim_icc_select(select_ptr,
                                            TRUE,
                                            &select_cnf_ptr);
    }

    MMGSDIUTIL_TMC_MEM_FREE(select_ptr);

    if((mmgsdi_status != MMGSDI_SUCCESS) || (select_cnf_ptr == NULL))
    {
      UIM_MSG_HIGH_0("Select of EF RPM VI failed");
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_cnf_ptr);
      break;
    }

    if((select_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS) ||
       (select_cnf_ptr->file_attrib.file_size != MMGSDI_RPM_VERSION_DATA_LEN))
    {
      UIM_MSG_ERR_2("Invalid select: Unable to proceed with update EF RPM VI status: 0x%x, file size: 0x%x",
                    select_cnf_ptr->response_header.mmgsdi_status,
                    select_cnf_ptr->file_attrib.file_size);
      mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_cnf_ptr);
      break;
    }

    mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_cnf_ptr);

    /* Updating RPM Version Implemented file*/
    req_ptr->request_header.client_id          = mmgsdi_generic_data_ptr->client_id;
    req_ptr->request_header.session_id         = session_id;
    req_ptr->request_header.slot_id            = slot;
    req_ptr->request_header.request_type       = MMGSDI_WRITE_REQ;
    req_ptr->request_header.orig_request_type  = MMGSDI_WRITE_REQ;
    req_ptr->access.access_method              = MMGSDI_EF_ENUM_ACCESS;
    req_ptr->access.file.file_enum             = mmgsdi_rpm_version_filename;
    req_ptr->rec_num                           = 0;
    req_ptr->offset                            = 0;
    req_ptr->file_type                         = MMGSDI_TRANSPARENT_FILE;
    req_ptr->access.file.file_enum             = mmgsdi_rpm_version_filename;

    if ((mmgsdi_util_get_efs_item_index(session_id, &sub_nv_context) == MMGSDI_SUCCESS) &&
        (mmgsdi_nv_get_external_feature_status(MMGSDI_EXT_FEAT_RPM_ENABLED, sub_nv_context) == MMGSDI_FEATURE_ENABLED))
    {
      mmgsdi_rpm_version_value[0] = MMGSDI_RPM_VERSION;
    }
    else
    {
      mmgsdi_rpm_version_value[0] = 0;
    }

    req_ptr->data.data_len = sizeof(mmgsdi_rpm_version_value);
    req_ptr->data.data_ptr = mmgsdi_rpm_version_value;

    /* Write EF-RPM VERSION Implemented */
    if(app_type == MMGSDI_APP_USIM)
    {
      mmgsdi_status = mmgsdi_uim_uicc_write_transparent(req_ptr, TRUE, &cnf_ptr);
    }
    else
    {
      mmgsdi_status = mmgsdi_uim_icc_write_transparent(req_ptr, TRUE, &cnf_ptr);
    }
  }
  while (0);

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr);
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  gstk_gsdi_continue_task(TRUE);
#else
  (void)session_id;
  (void)slot;
  (void)app_type;
#endif /* FEATURE_MMGSDI_3GPP */
} /* mmgsdi_set_ens_support_flag */


/*================================================================
FUNCTION  MMGSDI_GSTK_TERMINAL_PROFILE_CB

DESCRIPTION:
  This function is called to send an indication to MMGSDI that
  terminal profile download is complete.

  INPUT PARAMETERS:
    gstk_tp_response_ptr : TP response data from GSTK

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===============================================================*/
void mmgsdi_gstk_terminal_profile_cb
(
  gstk_cmd_from_card_type * gstk_tp_response_ptr
)
{
  mmgsdi_task_cmd_type            * task_cmd_ptr   = NULL;
  mmgsdi_tp_dl_complete_req_type  * msg_ptr        = NULL;
  mmgsdi_slot_id_enum_type          slot_id        = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_return_enum_type           mmgsdi_status  = MMGSDI_ERROR;
  int32                             total_mem_len  = 0;
  uint32                            task_cmd_len   = 0;

  if(gstk_tp_response_ptr == NULL)
  {
    UIM_MSG_HIGH_0("NULL input pointer");
    return;
  }

  if(gstk_tp_response_ptr->hdr_cmd.command_id != GSTK_PROFILE_DL_IND_RSP)
  {
    UIM_MSG_ERR_1("Unexpected GSTK message received 0x%x",
                  gstk_tp_response_ptr->hdr_cmd.command_id);
    return;
  }

  UIM_MSG_HIGH_1("mmgsdi_tp_download_complete for slot 0x%x",
                 gstk_tp_response_ptr->hdr_cmd.sim_slot_id);


  switch(gstk_tp_response_ptr->hdr_cmd.sim_slot_id)
  {
    case GSTK_SLOT_1:
      slot_id = MMGSDI_SLOT_1;
      break;
    case GSTK_SLOT_2:
      slot_id = MMGSDI_SLOT_2;
      break;
    case GSTK_SLOT_3:
      slot_id = MMGSDI_SLOT_3;
      break;
    default:
     UIM_MSG_HIGH_1("Invalid Slot ID 0x%x",
                    gstk_tp_response_ptr->hdr_cmd.sim_slot_id);
     return;
  }

  total_mem_len = uint32toint32(sizeof(mmgsdi_task_cmd_type));

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  mmgsdi_util_align_mem(&task_cmd_ptr->cmd.cmd.tp_dl_complete_req,
                        MMGSDI_TP_DOWNLOAD_COMPLETE_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.tp_dl_complete_req;

  /*---------------------------------------------------------------------------
       Populating mmgsdi_tp_dl_complete_req_type header
        1) Client ID
        2) Request TYPE
        3) Client Data Pointer
        4) Response CallBack
        5) Payload len and request len will be populated after the content
            has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_TP_DOWNLOAD_COMPLETE_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.request_type      = MMGSDI_TP_DOWNLOAD_COMPLETE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_TP_DOWNLOAD_COMPLETE_REQ;
  msg_ptr->request_header.client_data       = gstk_tp_response_ptr->hdr_cmd.user_data;
  msg_ptr->request_header.response_cb       = mmgsdi_util_internal_cb;
  msg_ptr->request_header.slot_id           = slot_id;

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

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_TP_DOWNLOAD_COMPLETE_REQ status 0x%x",
             mmgsdi_status);

  return;
} /* mmgsdi_gstk_terminal_profile_cb */


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
  mmgsdi_session_id_type session_id
)
{
  boolean result = TRUE;

#ifdef FEATURE_SIMLOCK
  mmgsdi_int_app_info_type *app_info_ptr = NULL;
  mmgsdi_return_enum_type   mmgsdi_status= MMGSDI_ERROR;

  mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                    NULL,
                                                    NULL,
                                                    &app_info_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS ||
      app_info_ptr  == NULL ||
      app_info_ptr->perso_state != MMGSDI_SIMLOCK_VERIFIED )
  {
    result = FALSE;
  }
#else
  (void)session_id;
#endif /* FEATURE_SIMLOCK */
  return result;
}/* mmgsdi_is_perso_completed */


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
boolean mmgsdi_is_ens_enabled_in_nv(void)
{
  nv_stat_enum_type nv_status = NV_DONE_S;
  mmgsdi_nv_item_cache_type mmgsdi_nv_data = {0};

  nv_status = mmgsdi_nv_get_item(NV_ENS_ENABLED_I,
                                 &mmgsdi_nv_data,
                                 MMGSDI_NV_CONTEXT_PRIMARY);

  if( (nv_status == NV_DONE_S) && (mmgsdi_nv_data.item_value.ens_enabled))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* mmgsdi_is_ens_enabled_in_nv */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_GET_OPERATOR_NAME

DESCRIPTION
  This function queues a response for handling get operator name in the cnf

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_process_get_operator_name(
  mmgsdi_session_get_eons_req_type * req_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_ERROR;
  int32                               index           = 0;
  mmgsdi_client_req_extra_info_type  *extra_param_ptr = NULL;
  mmgsdi_session_type_enum_type       session_type    = MMGSDI_MAX_SESSION_TYPE_ENUM;
  uint32                              plmn_ids_cnt    = 0;
  mmgsdi_nv_context_type              nv_context      = MMGSDI_NV_CONTEXT_INVALID;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /*----------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   -----------------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status =  mmgsdi_util_get_session_type(
                     req_ptr->request_header.session_id,
                     &session_type,
                     NULL);

  /*Process get operator name command only when session type is
       GW PRI, GW SEC or 1X PRI*/
  if(MMGSDI_SUCCESS != mmgsdi_status ||
     (session_type != MMGSDI_GW_PROV_PRI_SESSION &&
      session_type != MMGSDI_GW_PROV_SEC_SESSION &&
      session_type != MMGSDI_GW_PROV_TER_SESSION &&
      session_type != MMGSDI_1X_PROV_PRI_SESSION ))
  {
    return MMGSDI_ERROR;
  }

  /* When CSG feature is disabled, check if CSG ID is appropriately set */
  if((mmgsdi_util_get_efs_item_index(req_ptr->request_header.session_id, &nv_context) ==
        MMGSDI_SUCCESS) &&
     (mmgsdi_nv_get_external_feature_status(MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG, nv_context) ==
       MMGSDI_FEATURE_DISABLED))
  {
    for (plmn_ids_cnt = 0; plmn_ids_cnt < req_ptr->plmn_id_list.num_of_plmn_ids; plmn_ids_cnt++)
    {
      if ((req_ptr->plmn_id_list.plmn_list_ptr != NULL) &&
          (req_ptr->plmn_id_list.plmn_list_ptr[plmn_ids_cnt].csg_id != MMGSDI_EONS_INVALID_CSG_ID))
      {
        return MMGSDI_INCORRECT_PARAMS;
      }
    }
  }

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(
                    MMGSDI_SESSION_GET_OPERATOR_NAME_REQ,
                    (void*)req_ptr,
                    &extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }/* end of if (mmgsdi_status != MMGSDI_SUCCESS) */

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(
                    index,
                    &req_ptr->request_header,
                    NULL,
                    extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index,
                    MMGSDI_SESSION_GET_OPERATOR_NAME_REQ,
                    mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
}/* mmgsdi_process_get_operator_name */


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
)
{
  uint8  index = 0;

  for(index = 0; index < MMGSDI_MAX_CLIENT_REQ_INFO; index++)
  {
    if(mmgsdi_client_req_table_info_ptr[index] == NULL)
    {
      continue;
    }

    if(mmgsdi_client_req_table_info_ptr[index]->slot_id == slot_id &&
       mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED)
    {
      UIM_MSG_HIGH_2("Freeing client_req_table index 0x%x for pending command 0x%x",
                     index, mmgsdi_client_req_table_info_ptr[index]->request_type);
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
} /* mmgsdi_free_client_req_data_for_pending_commands */


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
)
{
  uint8                   index         = 0;
  mmgsdi_task_cmd_type *  rsp_q_ptr     = NULL;
  boolean                 rsp_found     = FALSE;
  mmgsdi_task_enum_type   mmgsdi_task   = MMGSDI_TASK_MAX;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  mmgsdi_task = mmgsdi_util_get_current_task();
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }

  for(index = 0; index < MMGSDI_MAX_CLIENT_REQ_INFO; index++)
  {
    if(mmgsdi_client_req_table_info_ptr[index] == NULL ||
       mmgsdi_client_req_table_info_ptr[index]->slot_id != slot_id)
    {
      continue;
    }

    /* Do nothing for the entry currently being processed since
       mmgsdi_process_response() always generates a cnf. Also, do nothing
       if the cnf already sent to client and uim_rsp_status marked as
       MMGSDI_UIM_RSP_NOT_RECEIVED */
    if(mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status == MMGSDI_UIM_RSP_PROCESSING ||
       mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED)
    {
      continue;
    }

    rsp_found = FALSE;
    rsp_q_ptr = (mmgsdi_task_cmd_type *)q_check(&mmgsdi_task_rsp_q[mmgsdi_task]);

    while(rsp_q_ptr != NULL)
    {
      if(rsp_q_ptr->cmd.cmd_enum == MMGSDI_UIM_REPORT_RSP &&
         rsp_q_ptr->cmd.cmd.uim_report_rsp.rsp_data_index == index)
      {
        /* Response is already queued by UIM. Nothing to do here. */
        mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status = MMGSDI_UIM_RSP_INBOUND;
        rsp_found = TRUE;
        break;
      }
      rsp_q_ptr = (mmgsdi_task_cmd_type *)q_next(&mmgsdi_task_rsp_q[mmgsdi_task], &rsp_q_ptr->link);
    }

    if(rsp_found)
    {
      continue;
    }

    /* No response was found in mmgsdi_task_rsp_q, nor are we currently
       processing the rsp, so queue an error cnf */
    mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status = MMGSDI_UIM_RSP_NOT_RECEIVED;

    /* Clear the sanity timer associated with this request */
    (void)rex_clr_timer(&mmgsdi_uim_sanity_timer[mmgsdi_task]);

    mmgsdi_status = mmgsdi_build_and_queue_static_cnf(index, MMGSDI_ERROR);
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      /* Reset cmd_in_progress flag for this task after sending error cnf
         for pending commands so that future commands can be processed after
         card is inserted. */
      if (mmgsdi_uim_cmd[mmgsdi_task].index == index &&
          mmgsdi_uim_cmd[mmgsdi_task].cmd_in_progress == TRUE)
      {
        mmgsdi_uim_cmd[mmgsdi_task].cmd_in_progress = FALSE;
      }
    }
    UIM_MSG_HIGH_2("Error cnf queuing status 0x%x for not-responded-to cmd 0x%x",
                   mmgsdi_status,
                   mmgsdi_client_req_table_info_ptr[index]->request_type);
  }
} /* mmgsdi_queue_error_cnf_for_pending_commands */


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
)
{
  boolean qmi_uim_processing = FALSE;

  if(UIM_GSDI_TCB == task_tcb_ptr &&
     mmgsdi_process_qmi_uim_sig)
  {
    qmi_uim_processing = TRUE;
  }
  return qmi_uim_processing;
}/* mmgsdi_is_qmi_uim_processing */
