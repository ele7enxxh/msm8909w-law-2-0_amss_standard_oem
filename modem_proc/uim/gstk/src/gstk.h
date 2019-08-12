#ifndef GSTK_H
#define GSTK_H
/*===========================================================================


                  G S T K    M O D U L E     H E A D E R


GENERAL DESCRIPTION

  This header file is the gstk module header protocol and function
  prototypes.

EXTERNALIZED FUNCTIONS
  gstk_task_put_cmd_buf
    This function put the command onto gstk task command queue

  gstk_cmd
    This function allows clients to put the gstk_cmd onto the GSTK command
    queue.

  gstk_power_down
    This function flushes GSTK queue, vote UIM off and Ack TMC

  gstk_process_state
    This function dispatches the command to the current state of the GSTK state
    machine.

  gstk_build_timer_expire_envelope
    This function build an envelope command when the timer was started and
    expired

  gstk_rex_timer_elapsed_expire_cb
    This function will add the elapsed time value to the time field of the
    timer management table and restart the timer_elapsed

  gstk_rex_timer_expire_cb
    This function builds the timer expiration envelope command to be sent
    to the card

  gstk_switch_to_instance
    This function switches current context data (private data) to a specific
    instance assiciated with specified SIM slot id


INITIALIZATION AND SEQUENCING REQUIREMENTS


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
01/08/16   vr      Reset usr_activity and idle_scrn for raw envelope response evt dl
05/12/15   dy      Enabling Toolkit-initiated GBA TP bit (byte 25 bit 5)
04/27/15   vr      Non UI and non NW proactive command parallel processing
01/27/15   gm      Retry call control after recovery
12/19/14   gm      Added support to read envelope retry data from NV
11/27/14   vr      Cache setup event list after sending TR to setup evt list
11/11/14   shr     Fixed compilation warnings
10/28/14   gs      Removed gstk_check_for_pending_send_sms_term_resp()
10/08/14   bd      Changes to control eHRPD loc info feature with NV
09/25/14   gm      Added support to send failure indication for OTASP
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info
09/23/14   gs      Make gstk_num_of_instances as part of gstk_shared_data_type
09/10/14   gm      Added support for SMS_PP data download retry
09/02/14   dy      Adding handling for session changed in non-idle states
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/14/14   vr      Use mmgsdi card slot session to read EF-IMG
08/13/14   dy      Add feature to send limited svc to card only upon NW reject
08/06/14   ky      Adding multislot support
08/06/14   gm      Removed unused variable
08/05/14   lxu     Use NV Bitmask to control the Dual STK App
08/05/14   lxu     Support feature FEATURE_MODE_SWITCH_STK_SWITCH
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
07/01/14   kr      Removing tmc from UIM codebase
06/03/14   sw      Implemented retry for call-control envelopes
05/28/14   gm      Support of CSG Rel-11 feature
05/23/14   gm      Verifying TR response report
05/21/14   kr      Fixed compilation errors when WCDMA and TDS disabled
05/20/14   dy      Fixed handling of apps_enabled in mmgsdi_session_changed
05/06/14   gs      GSTK Featurization enhancements and clean up
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/24/14   sw      Add support for BIP Status DIAG event
03/17/14   gm      Added support for IMS communication control
03/11/14   kb      Added TASK_EXIT_SIG to synchronize exiting of all tasks
03/04/14   shr     Do not wait for 1x to become available on non-SVLTE targets
                   w.r.t. card activation via OTASP
03/04/14   shr     Fix Subscription Ready caching logic
03/03/14   shr     ESTK BIP clean-up and enhancements
02/20/14   dy      Verify TR response for raw TRs
02/19/14   gs      Fixed infinite IMSI refresh issue
01/30/14   vr      SIM initiated call control handling
01/31/14   vr      Allow the fetch command based on the fetch status from MMGSDI
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/15/14   kb      Added GSTK_RETURN_IF_NULL macro
01/09/14   hn      Call wrapper API for segment loading
01/03/14   sw      Update active slot during BIP open channel
12/16/13   bd      Added support for new CM SS event handling
12/05/13   shr     Added support for preferred activation over 1x
12/05/13   hn      Support DNS query and Channel reuse
11/01/13   sw      BIP for Multi-slot changes
11/29/13   vr      Enable feature GSTK_CFG_FEATURE_ENH_RAT_AND_LOC_EVT in
                   recovery
11/12/13   sw      Removed FEATURE_GSTK_TEST_AUTOMATION code
11/13/13   shr     Added support for BIP over WLAN
09/13/13   gm      Support for Recovery and hot-swap
09/04/13   jd      Start the No Service timer on receiving MM IDLE No Service
                   indication only when in SGLTE mode and both MM are active
08/27/13   jd      Fix of data card TP issue
08/22/13   vr      Klocwork fixes
08/19/13   hn      Move lte_nmr_ref_id out of FEATURE_LTE and rename it
08/06/13   vr      Centralized Call Control Changes
07/26/13   hh      Support for E-UTRAN Inter-frequency measurements
07/24/13   sw      Executing location env rsp rcvd confirmation in gstk context
07/23/13   sw      Removed DS API to query for RAT information
07/22/13   hn      Support MM Indications
07/01/13   bd      Added support for maintaining separate cache for cell
                   information from different RRCs
07/01/13   hn      Support SGLTE dual MMs
06/24/13   xz      Support IMS Registration event download
06/20/13   vr      Triton TSTS changes
06/19/13   hh      Send error response to clients when env ref table is full
06/19/13   gs      Removed Unused Variables
05/21/13   gm      Update cell_info_type based on latest rrcmmif.h
05/17/13   abd     Added support for reading default lang from NV
05/01/13   vr      Fix command processing logic in GSTK
04/29/13   gm      Removed unused GSTK function
04/12/13   av      Move UIM tasks to group4 of RCINIT
04/11/13   bd      Avoid notifying NAS to ATTACH before IMSI switch
02/21/13   hn      Enhancement of Loc Status and VPE retry
02/14/13   tl      Added GSTK_QMI_CAT_SIG comment to track signal
02/11/13   bd      Support for (e)HRPD access tech
02/11/13   bd      Task stop and RCINIT registration changes for Dime
01/22/13   hn      Support 1xRTT CSIM programming retries
01/17/13   hn      Support multiple sessions per subscription
10/15/12   bd      Support for Closed Subscriber Group feature
09/20/12   yt      Manage concurrency between BIP and OTASP
09/12/12   hk      Support to read IMSI after getting VPE resp
08/06/12   sg      Fix starvation when reading IMSI_M failed
08/07/12   sg      Add enum for QCI type for eutran_mapped_utran
07/31/12   bd      Clean up the hack to calculate correct direction for call end event
                   download envelopes
06/06/12   sg      Add enum for command ID
05/22/12   nb      Convert setting and resetting of gstk_proactive_cmd_in_progress
                   flag to MACRO
05/17/12   nb      Added prototype of gstk_get_and_store_curr_app_type
03/09/12   bd      Enhancements for location information in CC envelope
03/28/12   av      Replaced feature flag with NVITEM
03/20/12   dd      Delay VPE if sent before TP
02/18/12   av      Cleaned up remaining ALWAYS ON/OFF features
02/08/12   nb      ThreadSafe - Remove blocking calls out of critical section
02/07/12   sg      Optimize EFS read
01/19/12   bd      Remove unused function gstk_cm_access_tech_cb()
12/20/11   nb      GSDI Cleanup
12/05/11   xz      Fix the issue of loc info in CC envelope cmd
11/09/11   sg      Move GSTK_EUTRAN_CLASS_E_SUPPORTED_BYTE_17_BIT_7
                   to gstk_exp_v.h
10/31/11   sg      Class L and class M support
10/20/11   dd      Fix CDMA TP
10/20/11   sg      Add enums and globals for gstk support simultaneous RATs
10/18/11   av      Removed features that are always OFF
10/13/11   nb      Full recovery support
10/03/11   dd      Fix Get Input TP field for data cards
09/28/11   xz      Send AcT and Loc envelope when receive SETUP EVT LIST cmd
08/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
09/15/11   nb      Reverting changes to block polling
09/12/11   js      Add support of CDMA CC envelope
07/26/11   js      Change to support AcT event processing in TAL
06/08/11   js      Update GPRS Anite NV after subscription ready.
05/18/11   xz      Use MMGSDI evt to track apps enabled
05/14/11   bd      Thread safe changes - timer management
05/14/11   bd      Removed the global is_gstk_rrc_registered
05/11/11   xz      Updated GSTK_LOC_INFO_ENVELOPE_WAIT_TIME
05/03/11   xz      1) Provide LTE only support
                   2) Fix CMI compile issue
04/27/11   yt      Included private GSDI header
04/26/11   yt      Replaced references to TMC heap with Modem heap
04/26/11   dd      Fix compilation error
04/24/11   dd      SCWS support
04/13/11   xz      Fix issue of unexpected NO SERVICE envelope cmd
04/05/11   xz      Changes to support VPE ATTACH CTRL
03/03/11   dd      Send PLI Date/time to clients
02/22/11   js      Renaming fusion features
02/16/11   bd      Support for delaying the TR when card is repeating the
                   same proactive command continuously.
02/02/11   dd      Process LTE AcT correctly
01/28/11   bd      Fixed issue of wrong data structure passed to call back
01/26/11   xz      Add support of HRPD access tech
01/20/11   xz      Fix compiler warning
01/26/11   xz      Define gstk_otasp_cmd_enum_type
01/24/11   js      Fix GSTK command buffer queue leak
01/20/11   dd      Enable FEATURE_CAT_REL7
01/13/11   xz      Fix the issue of CM triggering (E)UTRAN NO SERVICE event
12/23/10   bd      Fixed issue of passing wrong data structure object to
                   client call back
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/13/10   adp     Is subscription ready field is valid only if
                   FEATURE_MMGSDI_SESSION_LIB is defined
12/10/10   sg      Port timer for 1x Location Status Envelope
11/29/10   tkl     Removed RRC private header file
11/30/10   yt      Including private GSTK header
11/10/10   nb      Added definition of gsdi_get_nv_ext
11/04/10   js      Fix for handling UI related set up events in TAL
11/02/10   bd      Changed TP wait timer to 2 minutes
10/22/10   js      Make GSTK do malloc on modem_heap
10/21/10   dd      Set default time zone to 0xFF until it is received from NW
10/11/10   sg      Port IMSI_M enhancement and multiple OTASP calls
10/07/10   ms      Fixed Lint errors
10/05/10   bd      Fixed issue of TP DL
09/08/10   nb      Updates for new polling timer
08/31/10   dd      Changes for compilation for LTE only build
08/30/10   xz      Extend TR wait timer to 125 sec
08/26/10   xz      Add support of EPS Call Control
08/19/10   xz      Block OPEN CH until MMGSDI_SUBSCRIPTION_READY_EVT
08/18/10   dd      Correct default TP for ICC
07/28/10   xz      Read ENS NV during initialization
07/26/10   xz      Add support of MM and EMM idle events simultaneously
07/26/10   nb      Added variables to track if FETCH is blocked by MMGSDI
07/24/10   js      TAL command processing fixes
07/16/10   xz      Fix the issue of queuing too many cmd in MMGSDI to read
                   service table
07/13/10   sg      Feature TAL
07/09/10   dd      Check for CM Sups events
07/07/10   xz      New Call Control API support
06/30/10   dd      Icon display confirmation
06/29/10   nb      Merged Changes for DS DS
06/21/10   xz      KW fixes
06/21/10   xz      Use correct feature mask
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
05/28/10   xz      Extend Loc Info Envelope timer to 10 seconds
05/24/10   sg      Send Location information envelope only when it has changed
05/26/10   bd      Fixed compilation error
05/23/10   xz      Add support of LTE NMR
05/11/10   xz      Add support of network rejection event
04/27/10   xz      Add support of EMM-IDLE event
04/20/10   xz      Add support of LTE-RRC LTE_RRC_SERVICE_IND
04/09/10   ssr     Merged gobi changes
04/07/10   xz      Fix the issue of delay in FETCH after envelope rsp
03/19/10   dd      Move the GSTK_SLOT_AUTO fix to gstklib.c
03/18/10   dd      Add GSTK_SLOT_AUTO  to the list of valid slots
03/15/10   xz      Add support of LTE BIP
03/08/10   sg      Location information envelope command
02/19/10   bd      Support for caching proactive commands
02/15/10   nb      Moved Dual Slot Changes
02/10/10   dd      Provide local time and time zone in PLI
02/09/10   xz      IMSI_M provision enhancement
02/04/10   sg      Add global for open channel timeout TR extra parameters
12/17/09   sg      Fix compilation errors
11/23/09   dd      Use GMT time zone in PLI
11/18/09   bd      Support for delaying sending of CC envelope to card
10/28/09   nb      Release 7 Enhanced Network Selection
10/20/09   ssr     Extend extend GSTK_TP_CMD_WAIT_TIME value
10/08/09   dd      Extend TP DL and TR time for Symbian targets
10/08/09   kp      Replacing FEATURE_MODEM_HEAP with
                   FEATURE_UIM_ZI_MEMORY_REDUCTION
09/26/09   kp      ZI memory reduction changes
07/21/09   dd      Adding Time Zone information
06/18/09   dd      Change GSTK_MAX_RAW_SMS_LEN to 255
06/03/09   xz      extend GSTK_TP_CMD_WAIT_TIME value
06/03/09   yb      Changing GSTK_TP_CMD_WAIT_TIME timer value for win mob to
                   2 minutes
04/20/09   yb      Fixed compilation errors
04/15/09   xz      Support client id clean up of modem restart
03/31/09   yb      Fixed Number of TPs expected for Non UI builds with ESTK
                   enabled
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id
                   when parse client TP and simplify TP download procedure
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
03/13/09   gg      Define class bit masks using bitwise OR of cmd bit masks
03/11/09   yb      Fixed Warnings for using 'free' as variable name
03/02/09   sun     Added Private header file inclusion
02/26/09   gg      Introduced Class B event registration
02/17/09   nb      Fixed Compilation Warning
01/16/09   xz      Add gstk_priv_data to store GSTK private data
01/19/09   yb      Added secondary support for cell id information when
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
10/22/08   sk      Removed gstk_terminal_profile_data_from_client
10/16/08   ssr     Fixed GSTK_TP_EIGHTEENTH_BYTE value for RUIM Enablement
10/13/08   yb      Removed Lint Errors
10/06/08   yb      Sending BIP envelope after TR while BIP is working
10/01/08   sk      Increased Terminal Profile download wait time if
                   FEATURE_ESTK is turned on
09/24/08   xz      Removed FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
09/08/08   sk      Added support for FEATURE_ESTK
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/29/08   yb      Fixed alignment
08/28/08   sk      Added fix for enhanced registration
08/28/08   yb      Added support for longer text string in terminal response
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session
08/21/08   sk      Added support for enhanced GSTK registration that includes
                   support for multiple clients to perform terminal profile
                   download and support for clients to register for a subset of
                   proactive command in GSTK or RAW format.
08/08/08   xz      Added support for raw proactive command, raw terminal response
                   and raw envelope command.
05/06/08   sk      Added support for event download envelope retry
04/04/08   sk      Fixed access technology enum
03/28/08   sk      Fixed warnings
02/06/08   sk      Support for PLI UTRAN NMR
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
11/07/07   sk      Added support for NAA Refresh
11/20/07   tml     Added support to allow delay in sending TR until client
                   responded back for certain internal command that was originally
                   handled by GSTK
10/04/07   sk      Added UTRAN measurement tag
08/07/07   sk      Fixed compilation / warning
08/01/07   sk      Fixed lint error
07/14/07   sk      Added support to requeue sms pp envelope if there is a
                   pending send sms tr
06/06/07   st      Added GSTK_DCTSTK_SIG for data card toolkit support in UNDP-1
05/07/07   jar     Increased the GSTK Timeout Value to 5 mins for WM builds
04/23/07   nk      Added gstk_offset_memcpy function
04/12/07   jk      Added GSTK MALLOC and FREE functions
03/16/07   tml     Fixed AID tag value
02/27/07   jk      Lower Command Buffer Size for Memory Reduction
01/26/07   tml     Fixed AID tag value
01/17/07   tml     Fix TP timeout for features
01/03/06   tml     Increase TP timeout time for WINCE target
08/25/06   sk      Fixed defintion for TP 18th byte
08/25/06   sk      Featurized uim recovery
08/17/06   sk      Defined 18th and 23rd byte of Terminal Profile
06/24/06   sk      Lint fixes
06/17/06   jar     Added FEATURE_GSTK_FOR_NON_LTK_BUILD around RRC Includes
                   to support VSIM in LTK.
06/13/06   sk      UIM recovery support
06/06/06   sk      Added support for network search mode
05/15/06   tml     GPRS Call Control support
05/03/06   tml     compilation fix
05/03/06   tml     lint and Add PDP Content Act Param Tag
04/20/06   sk      Added nv header file.
04/04/06   sk      Added support to obtain battery status, ESN, MEID, IMEISV.
                   Added prototypes for functions that obtain cell id info from
                   RRC and RR.
                   Added support for immediate digit response and variable
                   timeout in get inkey cmd.
                   Added support for 1X related location information.
01/18/06   sk      Added new macro for Refresh Type
11/14/05   sk      Fixed lint errors
11/03/05   sk      Fixed compilation warning
10/18/05   tml     Added protocol info
10/10/05   tml     Added Curr Rat and Next Curr Rat support
10/07/05   tml     Added plmn and lac in rrc cell change ind type
09/30/05   sk      Changed prototype for gstk_rrc_cell_change_ind_cb()
                   Added two new tables gstk_loci_table and rrc_loci_table
                   for maintaining plmn/lac/cell_id info from mm and rrc
                   respectively.
                   Defined two new macros GSTK_MAX_PLMN_LEN and
                   GSTK_MAX_LAC_LEN
09/26/05   tml     Updated RRC cell id received variable to ensure location
                   status for normal service is only sent when both MM and RRC
                   info are received
09/20/05   tml     Update Access Technology tag value
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID
                   changed during normal service
07/20/05   tml     Added extended cell id support
07/19/05   sk      Added new macro GSTK_LOWER_NIBBLE_MASK
07/13/05   sk      Added Access Technology support
06/09/05   sun     Check for flag before processing commands
05/11/05   sst     Lint fixes
04/06/05   tml     Reduce proactive command cache size to 6
03/05/05   tml     Added tag comprehension support
02/02/05   sk      Added new enum type gstk_search_string_type
02/02-05   sk      Added macros to support Network Access Name
12/06/04   sk      Changed the 'get_channel_status' terminal response structure
                   to include status of all the open and dropped channels.
12/04/04   tml     move dcs enum to exp.h
11/23/04  sk/tml   Added SAT/USAT data support
08/30/04   tml     Added gstk_profile_received
08/19/04   tml     Added cell broadcast support
08/18/04   tml     Added gstk_dcs_enum_type
08/10/04   tml     Added GSTK_SEND_POSTPONE_ENVELOPE signal handling
08/03/04   tml     Added comment for timer_value_tlv format
06/14/04   tml     Added GSTK_GENERATED_END_SES and GSTK_GENERATED_END_SES_INDEX
05/11/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/16/04   tml     linted
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
03/01/04   tml     Merged from branch: Fixes for location status env command
09/13/03   tml     Changed slot_id_type to slot_id_enum_type
09/12/03   tml     Increased GSTK cmd pool size and GSTK address len
09/04/03   tml     Dual slot support
05/16/03   tml     linted
04/28/03   tml     Added GSDI access signal
04/02/03   tml     Added UTK support
03/20/03   tml     Added GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT and
                   GSTK_2_BYTE_LENGTH_FIRST_VALUE, GSTK_TAG_LENGTH_LEN
03/06/03   tml     Prefix enum with GSTK_
02/28/03   jar     Increased Terminal Response from 1 second to 2 Minutes.
                   1000 -> 12000.
02/11/03   tml     Remove GSTK_UIM_REPORT_SIG
02/07/03  tml/jar  Initial Version



===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "cmd.h"
#include "mmgsdilib_p.h"
#ifdef  FEATURE_GSTK_TAL
#include "gstk_tal.h"
#endif /*  FEATURE_GSTK_TAL */
#if defined (FEATURE_GSTK_FUSION_REMOTE) || defined (FEATURE_GSTK_TAL)
#include "gstk_terminal_rsp.h"
#endif /* FEATURE_GSTK_FUSION_REMOTE || FEATURE_GSTK_TAL */

#include "cm.h"
#include "cm_dualsim.h"
#include "rex.h"
#include "intconv.h"
#include "modem_mem.h"
#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include "ds_profile_3gpp2.h"
#include "ds_sys.h"
#include "ds_sys_event.h"
#include "ds_sys_conf.h"

#ifdef FEATURE_SEGMENT_LOADING
/* Segment Loading headers */
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#endif /* FEATURE_SEGMENT_LOADING */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#include "uim_msg.h"
#include "gstki.h"
#include "gstk_exp.h"
#include "gstk_nv_p.h"
#include "gstk_term_profile.h"

#if defined(FEATURE_WCDMA)
#include "rrcmmif.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
#include "msgr_msg.h"
#include "lte_rrc_ext_msg.h"
#endif /*FEATURE_LTE*/

/* IMSI_M read by path name */
#define GSTK_CSIM_IMSI_M_PATH_BUF_1       0x3F00
#define GSTK_CSIM_IMSI_M_PATH_BUF_2       0x7FFF
#define GSTK_CSIM_IMSI_M_PATH_BUF_3       0x6F22
#define GSTK_CSIM_IMSI_M_PATH_BUF_4       0xFFFF
#define GSTK_CSIM_IMSI_M_PATH_BUF_5       0xFFFF

/* Client type and client ID for GSTK such that it can use the GSTK
library functions */
#define GSTK_CLIENT_ID  1

#define NUM_OF_MM_PER_GSTK_INST             2
#define GSTK_MAX_BCD_ADDRESS_LEN            100 /* TP_BCD_NO_LENGTH */

#define GSTK_CELL_BROADCAST_PAGE_LEN        88

#define GSTK_GENERATED_END_SES              0xFF
#define GSTK_GENERATED_END_SES_INDEX        0x06

/* BER-Tag values used to describe the envelope contents [ME -> CARD]*/
#define GSTK_PROACTIVE_SIM_COMMAND_TAG      0xD0
#define GSTK_SMS_PP_DOWNLOAD_TAG            0xD1
#define GSTK_CELL_BROADCAST_DOWNLOAD_TAG    0xD2
#define GSTK_MENU_SELECTION_TAG             0xD3
#define GSTK_CALL_CONTROL_TAG               0xD4
#define GSTK_MO_SHORT_MESSAGE_CONTROL_TAG   0xD5
#define GSTK_EVENT_DOWNLOAD_TAG             0xD6
#define GSTK_TIMER_EXPIRY_TAG               0xD7
#define GSTK_TERMINAL_RESPONSE_TAG          0xFF
#define GSTK_LOC_INFO_ENVELOPE_TAG          0xDF
/* general data object lengths */
#define GSTK_COMMAND_DETAILS_LEN            0x03
#define GSTK_DEVICE_IDENTITY_LEN            0x02
#define GSTK_GEN_RESULT_LEN                 0x01
#define GSTK_DURATION_LEN                   0x02
#define GSTK_ITEM_IDENTIFIER_LEN            0x01
#define GSTK_LOCATION_INFORMATION_LEN       0x07
#define GSTK_CDMA_LOCATION_INFORMATION_LEN  0x0F
#define GSTK_1X_ADV_LOCI_LEN                0x04
#define GSTK_HDR_LOCI_LEN                   0x1B
#define GSTK_HELP_REQUEST_LEN               0x00
#define GSTK_EVENT_LIST_LEN                 0x01
#define GSTK_LOCATION_STATUS_LEN            0x01
#define GSTK_DATE_TIME_AND_TIME_ZONE_LEN    0x07
#define GSTK_TIMING_ADVANCE_LEN             0x02
#define GSTK_LANGUAGE_LEN                   0x02
#define GSTK_ACCESS_TECHNOLOGY_LEN          0x01
#define GSTK_BATTERY_STATUS_LEN             0x01
#define GSTK_NW_SEARCH_MODE_LEN             0x01
#define GSTK_BC_REPEAT_IND_LEN              0x01
#define GSTK_RESPONSE_LEN_LEN               0x02
#define GSTK_ICON_ID_LEN                    0x02
#define GSTK_TONE_LEN                       0x01
#define GSTK_BROWSER_ID_LEN                 0x01
#define GSTK_BROWSER_TERMINATION_CAUSE_LEN  0x01
#define GSTK_TIMER_VALUE_LEN                0x03
#define GSTK_TIMER_IDENTITY_LEN             0x01
#define GSTK_CHANNEL_STATUS_LEN             0x02
#define GSTK_CHANNEL_DATA_LEN_LEN           0x01
#define GSTK_BUFFER_SIZE_LEN                0x02
#define GSTK_ACTIVATE_DESCRIPTION_LEN       0x01
#define GSTK_CSD_BEARER_PARAM_LEN           0x03
#define GSTK_GPRS_BEARER_PARAM_LEN          0x06
#define GSTK_EUTRAN_EXT_PARAM_LEN           0x11
#define GSTK_EUTRAN_MAPPED_UTRAN_PS_PARAM_LEN 0x0A
#define GSTK_SIM_ME_INTERFACE_TRANSPORT_LVL_LEN 0x03
#define GSTK_UTRAN_NMR_QUAL_LEN             0x01
#define GSTK_ERROR_RESP_RESULT_LEN          0x02
#define GSTK_CSG_CELL_SELECTION_STATUS_LEN  0x02
#define GSTK_CSG_ID_LEN                     0x04
#define GSTK_HNB_NAME_LEN                   0x30

#define GSTK_PROACTIVE_CMD_LEN_OFFSET       0x01

/* tag index location in command */
/* index location for command details and
** device id is based on total size of the
** length field == 1 */
#define GSTK_PROACTIVE_CMD_TAG_INDEX    0
#define GSTK_COMMAND_DETAILS_TAG_INDEX  2
#define GSTK_DEVICE_ID_TAG_INDEX        7

/* GSTK capabilities */
#define GSTK_TP_LEN                     9    /* Length of terminal profile  */
#define GSTK_MAX_RAW_SMS_LEN            255

#define GSTK_MAX_EVENT_LIST_LEN         0x18  /* based on Standard, there are only 0x16 events
                                             ** can be specified at this time
                                             */

/* max length of data that requires 2 bytes to indicate length field */
#define GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT    0x7F
#define GSTK_2_BYTE_LENGTH_FIRST_VALUE         0x81
#define GSTK_TAG_LENGTH_LEN                    0x02

#define GSTK_LOWER_NIBBLE_MASK                 (uint8)0x0F

/* mask and shift value for TON and NPI */
#define GSTK_TON_SHIFT_VALUE 0x04
#define GSTK_TON_MASK_VALUE 0x70
#define GSTK_NPI_MASK_VALUE 0x0F

#define GSTK_TASK_CMD_Q_SIG                   0x00000001
#define GSTK_RPT_TIMER_SIG                    0x00000002
#define GSTK_GSDI_CMD_RSP_SIG                 0x00000080
#define GSTK_RCEVT_CM_TASK_READY_SIG          0x00000100 /* To receive notification from RCINIT
                                                            that CM task is up */
#define GSTK_RCEVT_RRC_TASK_READY_SIG         0x00000200 /* To receive notification from RCINIT
                                                            that RRC task is up */
#define GSTK_NV_SIG                           0x00000400
#define GSTK_RCEVT_QMI_MODEM_TASK_READY_SIG   0x00000800 /* To receive notification from RCINIT
                                                            that QMI MODEM task is up */
#define GSTK_RCEVT_TDS_RRC_TASK_READY_SIG     0x00001000 /* To receive notification from RCINIT
                                                            that TDSRRC task is up */
#define GSTK_TASK_OFFLINE_SIG                 0x00002000
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
#define GSTK_TASK_STOP_SIG                    0x01000000
#define GSTK_TASK_START_SIG                   0x00008000
#define GSTK_CLASH_WITH_GRP_ACK_SIG           0x00010000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_VS_OP_COMPLETE_SIG    0x00040000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_FS_OP_COMPLETE_SIG    0x00080000 /* RESERVED SIGNAL */
#define GSTK_CLASH_WITH_REX_SLEEP_SIG         0x00010000 /* RESERVED SIGNAL */
#ifdef FEATURE_LTE
#define GSTK_RRC_MSG_Q_SIG                    0x00020000
#endif /*FEATURE_LTE*/
//#ifdef  FEATURE_ESTK
//#define GSTK_ESTK_CMD_SIG                   0x00400000 /* Defined in gstk_p.h */
//#endif /* FEATURE_ESTK */
#define GSTK_QMI_CAT_SIG                      0x00800000 /* QMI CAT */

#define GSTK_RRC_MAX_MCC_DIGITS                    3
#define GSTK_RRC_MAX_MNC_DIGITS                    3
#define GSTK_RRC_MAX_GSMMAP_LAC_LENGTH             2   /* Length of LAC in bytes */


#ifdef FEATURE_GSTK_MEMORY_REDUCTION
#define GSTK_NUM_CMD_BUFS  8
#else
#define GSTK_NUM_CMD_BUFS  15
#endif /* FEATURE_GSTK_MEMORY_REDUCTION */

/* command details reference table */
#define GSTK_CMD_DETAILS_REF_TABLE_FULL 0xFF
#define GSTK_NO_MORE_TLV_OFFSET -1

/* client table */
/* LIMITATION: the max should be less than 32 */
#define GSTK_MAX_CLIENT 16

#define GSTK_HELP_AVAILABLE_BOOL_SHIFT 7

/* Timer values */
#if defined(FEATURE_ESTK)
#define GSTK_TP_CMD_WAIT_TIME                   120000
#else
#define GSTK_TP_CMD_WAIT_TIME                   10000
#endif /* FEATURE_ESTK */
#define GSTK_LOC_INFO_ENVELOPE_WAIT_TIME        20000
#define GSTK_CLIENTS_REG_WAIT_TIME              60000
#define GSTK_MM_IDLE_NO_SERVICE_WAIT_TIME       30000
#define GSTK_CDMA_LOC_STATUS_ENVELOPE_WAIT_TIME 5000
#define GSTK_ATTACH_CTRL_TIME                   2000
#define GSTK_1X_AVAIL_WAIT_TIMER                30000
#define GSTK_TERM_RSP_WAIT_TIME                 125000 /* 2 Minutes and 5 seconds */
/* For BIP OPEN CHANNEL and SEND DATA, extend the TR timer by 30 sec, to
   accomodate for potential fallback from WLAN to Cellular bearer or
   vice-versa */
#define GSTK_BIP_TERM_RSP_WAIT_TIME             185000 /* 2 Minutes and 35 seconds */
#define GSTK_ENV_RETRY_CMD_WAIT_TIME            10000  /* 10 sec */
#define GSTK_DEFAULT_NUM_OF_ENV_RETRY           3

/* SET UP MENU BIT MASKS */
#define GSTK_SETUP_MENU_SEL_PREF_BITMASK    0x01
#define GSTK_SETUP_MENU_RFU_MASK            0x7E
#define GSTK_SETUP_MENU_HELP_AVAIL_BITMASK  0x80
#define GSTK_SETUP_MENU_MENU_TITLE_TAG      0x85

/* SELECT ITEM BIT MASKS */
#define GSTK_SEL_ITEM_PRESENTATION_BITMASK        0x01
#define GSTK_SEL_ITEM_PRESENTATION_AS_NAV_BITMASK 0x02
#define GSTK_SEL_ITEM_SEL_PREF_BITMASK            0x04
#define GSTK_SEL_ITEM_HELP_AVAIL_BITMASK          0x80

/* Call control/ MO SMS control term profile index and mask */
#define GSTK_CC_TERM_PROFILE_INDEX           0x01
#define GSTK_CC_TERM_PROFILE_MASK            0x02
#define GSTK_MO_SMS_CTRL_TERM_PROFILE_INDEX  0x01
#define GSTK_MO_SMS_CTRL_TERM_PROFILE_MASK   0x08
#define GSTK_SMS_PP_TERM_PROFILE_INDEX       0x00
#define GSTK_SMS_PP_TERM_PROFILE_MASK        0x02
#define GSTK_CELL_BROADCAST_TERM_PROFILE_INDEX 0x00
#define GSTK_CELL_BROADCAST_TERM_PROFILE_MASK  0x04

#define GSTK_USSD_OBJECT_IN_CC_BYTE_1_BIT_7           0x0140 /* value = {1,0} */
#define GSTK_HANDLING_OF_ALPHA_BYTE_2_BIT_5           0x0210 /* value = {1,0} */
#define GSTK_NO_DISPLAY_SUPPORT                       0x0E20 /* value = {0,1} */
#define GSTK_NO_KEYBOARD_SUPPORT                      0x0E40 /* value = {0,1} */

/* MANDATORY AND OPTIONAL FIELD SPECIFIER */
#define GSTK_MANDATORY 0
#define GSTK_OPTIONAL  1


/* APN Network  ID and APN Operator ID */
#define GSTK_APN_NET_ID 0
#define GSTK_APN_OP_ID 1


/* Min and Max no of bytes in an APN - Spec 11.14 */
#define GSTK_MIN_APN_LEN 1
#define GSTK_MAX_APN_LEN 100

/* Min and Max len of the APN Operator Id labels */
#define GSTK_MIN_OP_LABEL_LEN 0x04
#define GSTK_MAX_OP_LABEL_LEN 0x06

#define RRC_LOCI_UPDATE            1
#define MM_GSTK_LOCI_UPDATE        2

/* SIM Refresh types */
#define GSTK_SIM_REFRESH_RESET 0x04

/* Max number of cells to be measured in UTRAN NMR */
#define GSTK_MAX_UTRAN_NMR_NO_OF_CELLS  0x06

/* max num of modem as info table entries */
#define GSTK_MODEM_AS_INFO_SIZE         GSTK_MAX_NUM_OF_INSTANCES

/* max num of mmgsdi session info table entries */
#define GSTK_MMGSDI_SESS_INFO_SIZE      GSTK_MAX_NUM_OF_INSTANCES*3

#define GSTK_MIN(A,B) ( (A) < (B) ? (A):(B))

#ifdef FEATURE_LTE
#define MSGR_MODULE_STK 0x01
#define MSGR_UIM_STK    MSGR_TECH_MODULE( MSGR_TECH_UIM, MSGR_MODULE_STK )
#endif /* FEATURE_LTE */

/* According to 11.14, 31.11 and 102 223, the terminal response
 * has following format:
 * byte | description
 * 1    | command details tag (0x81)
 * 2    | the length of command details (0x03 - 3 bytes)
 * 3    | command number
 * 4    | command type
 * ...
 *
 * Hence, the third byte is command number and the fourth byte is the type of
 * command that the terminal response corresponds to. We are doing sanity check
 * of those two fields agains the item referred by cmd_detail_ref_id in
 * command_details_ref_table.
 */
/* define byte offset according to above */
#define GSTK_CMD_DETAILS_TAG_OFFSET     0x00
#define GSTK_CMD_DETAILS_LEN_OFFSET     0x01
#define GSTK_CMD_NUMBER_OFFSET          0x02
#define GSTK_CMD_TYPE_OFFSET            0x03

/* class "0", class "c", class "e", class "b" begin and terminating index into
 * the class event reg array "gstk_class_evt_reg_table"
 * NOTE: The class event table is a linear array with index 0 - X belonging
 * to class 0, index X+1 - X+2 belonging to class "C" and index X+3 -
 * X+5 belonging to class "E" and X+6 belonging to class "B".
 * The number of elements in each class have been determined from the
 * number of masks belonging to a specific class that have been defined
 * in gstk_exp.h.
 */
#define GSTK_CLASS_0_BEGIN_INDEX  0
#define GSTK_CLASS_0_TERM_INDEX   28

#define GSTK_CLASS_C_BEGIN_INDEX   (GSTK_CLASS_0_TERM_INDEX + 1)
#define GSTK_CLASS_C_TERM_INDEX    (GSTK_CLASS_0_TERM_INDEX + 2)

#define GSTK_CLASS_E_BEGIN_INDEX   (GSTK_CLASS_C_TERM_INDEX + 1)
#define GSTK_CLASS_E_TERM_INDEX    (GSTK_CLASS_C_TERM_INDEX + 4)

#define GSTK_CLASS_B_BEGIN_INDEX   (GSTK_CLASS_E_TERM_INDEX + 1)
#define GSTK_CLASS_B_TERM_INDEX    (GSTK_CLASS_E_TERM_INDEX + 1)

#define GSTK_CLASS_L_BEGIN_INDEX   (GSTK_CLASS_B_TERM_INDEX + 1)
#define GSTK_CLASS_L_TERM_INDEX    (GSTK_CLASS_B_TERM_INDEX + 1)

#define GSTK_CLASS_M_BEGIN_INDEX   (GSTK_CLASS_L_TERM_INDEX + 1)
#define GSTK_CLASS_M_TERM_INDEX    (GSTK_CLASS_L_TERM_INDEX + 1)

#define GSTK_TOTAL_ELEMENTS_IN_CLASS_ARRAY (GSTK_CLASS_M_TERM_INDEX + 1)

#define GSTK_CATEGORY_0_EVT_BITMASK            \
  (GSTK_REG_DISPLAY_TEXT_EVT             |     \
  GSTK_REG_GET_INKEY_EVT                 |     \
  GSTK_REG_GET_INPUT_EVT                 |     \
  GSTK_REG_MORE_TIME_EVT                 |     \
  GSTK_REG_PLAY_TONE_EVT                 |     \
  GSTK_REG_POLL_INTERVAL_EVT             |     \
  GSTK_REG_PROVIDE_LOCAL_INFO_EVT        |     \
  GSTK_REG_REFRESH_EVT                   |     \
  GSTK_REG_SELECT_ITEM_EVT               |     \
  GSTK_REG_SEND_SMS_EVT                  |     \
  GSTK_REG_SEND_SS_EVT                   |     \
  GSTK_REG_SEND_USSD_EVT                 |     \
  GSTK_REG_SETUP_CALL_EVT                |     \
  GSTK_REG_SETUP_EVENT_LIST_EVT          |     \
  GSTK_REG_SETUP_MENU_EVT                |     \
  GSTK_REG_SETUP_IDLE_TEXT_EVT           |     \
  GSTK_REG_POLLING_OFF_EVT               |     \
  GSTK_REG_PROVIDE_LANG_INFO_EVT         |     \
  GSTK_REG_LANG_NOTIFY_EVT               |     \
  GSTK_REG_TIMER_MANAGEMENT_EVT          |     \
  GSTK_REG_SEND_DTMF_EVT                 |     \
  GSTK_REG_SETUP_EVT_USER_ACT_EVT        |     \
  GSTK_REG_SETUP_EVT_IDLE_MENU_EVT       |     \
  GSTK_REG_SETUP_EVT_LANG_SEL_EVT        |     \
  GSTK_REG_PROVIDE_BATTERY_STATE_EVT     |     \
  GSTK_REG_PROFILE_DL_RSP_EVT            |     \
  GSTK_REG_EVT_DOWNLOAD_RSP_EVT          |     \
  GSTK_REG_MENU_SEL_RSP_EVT              |     \
  GSTK_REG_PROVIDE_DATE_TIME_EVT)

#define GSTK_CATEGORY_C_EVT_BITMASK            \
  (GSTK_REG_LAUNCH_BROWSER_EVT           |     \
  GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT)

#define GSTK_CATEGORY_E_EVT_BITMASK            \
  (GSTK_REG_DATA_EVT                     |     \
  GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT  |     \
  GSTK_REG_SETUP_EVT_CH_STATUS_EVT       |     \
  GSTK_REG_SCWS_EVT)

#define GSTK_CATEGORY_B_EVT_BITMASK            \
  (GSTK_REG_RUN_AT_CMDS_EVT)

#define GSTK_CATEGORY_L_EVT_BITMASK            \
  (GSTK_REG_ACTIVATE_EVT)

#define GSTK_CATEGORY_M_EVT_BITMASK            \
  (GSTK_REG_SETUP_EVT_HCI_CONN_EVT)

/* Required for new tookit registration support to be backward compatible
   with old registration API
   Note: Currently, category C and category E do not have commands starting
   from 1. Hence these variables are needed in order to limit the size of the
   class event table to the number of events that actually exist in each class.
*/
#define GSTK_CATEGORY_C_ADJ_OFFSET    GSTK_CLASS_C_BEGIN_INDEX
#define GSTK_CATEGORY_E_ADJ_OFFSET    GSTK_CLASS_E_BEGIN_INDEX
#define GSTK_CATEGORY_B_ADJ_OFFSET    GSTK_CLASS_B_BEGIN_INDEX
#define GSTK_CATEGORY_L_ADJ_OFFSET    GSTK_CLASS_L_BEGIN_INDEX
#define GSTK_CATEGORY_M_ADJ_OFFSET    GSTK_CLASS_M_BEGIN_INDEX


#define GSTK_IS_VALID_SLOT_ID(x)                                 \
          ((gstk_slot_id_enum_type)(x) > GSTK_SLOT_AUTO &&       \
           (int)(x) <= gstk_shared_data.gstk_num_of_instances && \
           (int)(x) <= GSTK_MAX_NUM_OF_INSTANCES)

#define GSTK_RETURN_IF_NULL_PTR(x)                              \
          if ((x) == NULL)                                      \
          {                                                     \
            return;                                             \
          }

#define GSTK_RETURN_ERROR_IF_NULL_PTR(x, y)                     \
          if ((x) == NULL)                                      \
          {                                                     \
            return y;                                           \
          }

/* ===========================================================================
   MACRO:       GSTK_MSG_MALLOC_DEBUG_LOG
   DESCRIPTION: This is used to log the message.
   ===========================================================================*/
#ifdef FEATURE_UIM_MODEM_HELP_DEBUG
  #define GSTK_MSG_MALLOC_DEBUG_LOG(p_mem , ptr_size)   UIM_MSG_HIGH_2("GSTK Malloc 0x%x, size=0x%x", (p_mem), (ptr_size));
#else
  #define GSTK_MSG_MALLOC_DEBUG_LOG(p_mem , ptr_size)
#endif/* FEATURE_UIM_MODEM_HELP_DEBUG */

/* ===========================================================================
   MACRO:       GSTK_MSG_FREE_DEBUG_LOG
   DESCRIPTION: This is used to log the message..
   ===========================================================================*/
#ifdef FEATURE_UIM_MODEM_HELP_DEBUG
  #define GSTK_MSG_FREE_DEBUG_LOG(p_mem)   UIM_MSG_HIGH_1("GSTK Mem Free 0x%x", (p_mem));
#else
  #define GSTK_MSG_FREE_DEBUG_LOG(p_mem)
#endif /* FEATURE_UIM_MODEM_HELP_DEBUG*/

/* ===========================================================================
   MACRO:       GSTK_SET_PROACTIVE_CMD_IN_PROGRESS_FLAG
   DESCRIPTION: This is used set proactive command in progress flag along with a trace message.
   ===========================================================================*/
#define GSTK_SET_PROACTIVE_CMD_IN_PROGRESS_FLAG(x, y)                  \
{                                                                      \
  UIM_MSG_HIGH_1("Setting proactive_cmd_in_progress flag to 0x%x", y); \
  (x = y);                                                             \
}

#ifdef FEATURE_CDMA
#define GSTK_SET_CM_SS_MODE_CHANGE_MASK(mask, cat_ver)               \
{                                                                    \
  mask = CM_SS_SYS_MODE_MASK | CM_SS_ROAM_STATUS_MASK |              \
         CM_SS_SID_MASK | CM_SS_NID_MASK |                           \
            CM_SS_BASE_STATION_PARMS_CHGD_MASK |                     \
            CM_SS_MCC_MASK | CM_SS_IMSI_11_12_MASK |                 \
            CM_SS_SRV_STATUS_MASK | CM_SS_HDR_SRV_STATUS_MASK |      \
            CM_SS_HDR_ROAM_STATUS_MASK | CM_SS_SRV_DOMAIN_MASK |     \
            CM_SS_HDR_PERSONALITY_MASK;                              \
  UIM_MSG_HIGH_1("GSTK Mode Change Mask 0x%x", mask);                \
}
#define GSTK_SET_CM_SS_MODE_CHANGE_MASK2(mask, cat_ver)              \
{                                                                    \
  mask = 0;                                                          \
  if(cat_ver >= GSTK_CFG_CAT_VER6)                                   \
  {                                                                  \
    mask = CM_SS_EVT_POSITION_INFO_MASK |                            \
           CM_SS_EVT_HDR_MCC_MASK |                                  \
           CM_SS_EVT_SYS_ID_MASK |                                   \
           CM_SS_EVT_HDR_SYS_TYPE_MASK |                             \
           CM_SS_EVT_ACTIVE_BAND_MASK |                              \
           CM_SS_EVT_ACTIVE_CHANNEL_MASK |                           \
           CM_SS_EVT_HDR_BASE_STATION_PARMS_MASK;                    \
  }                                                                  \
  UIM_MSG_HIGH_1("GSTK Mode Change Mask 2 0x%x", mask);              \
}
#else
#define GSTK_SET_CM_SS_MODE_CHANGE_MASK(mask, cat_ver)               \
{                                                                    \
  mask = (CM_SS_SYS_MODE_MASK | CM_SS_ROAM_STATUS_MASK);             \
  UIM_MSG_HIGH_1("GSTK Mode Change mask 0x%x", mask);                \
}
#define GSTK_SET_CM_SS_MODE_CHANGE_MASK2(mask, cat_ver)              \
{                                                                    \
  mask = 0;                                                          \
  UIM_MSG_HIGH_1("GSTK Mode Change mask 2 0x%x", mask);              \
}
#endif

/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/

/* gstk envelope processing substate
   NOTE: Currently applicable to only event download envelopes, call-control and mo-sms-control
*/
typedef enum {
    GSTK_SUB_ST_PENDING_ENV_NONE,
    GSTK_SUB_ST_PENDING_ENV_IN_QUEUE,
    GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD
}
gstk_env_proc_substate_enum_type;

typedef enum {
    GSTK_MM_IDLE_FROM_NAS      = 0x0,
    GSTK_MM_IDLE_FROM_REQUEUE  = 0x2,
    GSTK_MM_IDLE_FROM_TIMER    = 0x3,
    GSTK_MM_IDLE_FROM_IND      = 0x4,
    GSTK_MM_INDICATION         = 0x5
}
gstk_mm_idle_cmd_id_enum_type;

typedef enum {
    GSTK_MODE_CHANGE_FROM_CM        = 0x1,
    GSTK_MODE_CHANGE_FROM_REQUEUE   = 0x2,
    GSTK_MODE_CHANGE_FROM_TIMER     = 0x3
}
gstk_mode_change_cmd_id_enum_type;

/* QCI type */
typedef enum{
  GSTK_EUTRAN_MAPPED_UTRAN_QCI_1_GBR = 1,
  GSTK_EUTRAN_MAPPED_UTRAN_QCI_2_GBR = 2,
  GSTK_EUTRAN_MAPPED_UTRAN_QCI_3_GBR = 3,
  GSTK_EUTRAN_MAPPED_UTRAN_QCI_4_GBR = 4,
  GSTK_EUTRAN_MAPPED_UTRAN_QCI_5_NON_GBR = 5,
  GSTK_EUTRAN_MAPPED_UTRAN_QCI_6_NON_GBR = 6,
  GSTK_EUTRAN_MAPPED_UTRAN_QCI_7_NON_GBR = 7,
  GSTK_EUTRAN_MAPPED_UTRAN_QCI_8_NON_GBR = 8,
  GSTK_EUTRAN_MAPPED_UTRAN_QCI_9_NON_GBR = 9
}
gstk_eutran_mapped_utran_qci_bitrate_enum_type;

/* Enum for proactive command ids used under special scenarios, normal value is 0,
   when IMSI_M read fails a new command if is created to track this scenario */
typedef enum
{
  GSTK_PRO_CMD_ID_DEFAULT                   = 0x0,
  GSTK_PRO_CMD_ID_OPEN_CH_IMSI_M_READ_FAIL  = 0x1,
  GSTK_PRO_CMD_ID_MAX                       = 0x2
}
gstk_proactive_cmd_id_enum_type;

/* toolkit support enum */
typedef enum {
    GSTK_UNKNOWN_TK_CAPABLE,
    GSTK_NO_TK_CAPABLE,
    GSTK_BOTH_TK_CAPABLE,
    GSTK_ONE_TK_CAPABLE
}
gstk_toolkit_cap_enum_type;

/* timer type */
typedef enum {
    GSTK_TP_TIMER         = 0x00000000,
    GSTK_TR_TIMER1,
    GSTK_TR_TIMER2,
    GSTK_TR_TIMER3,
    GSTK_TR_TIMER4,
    GSTK_TR_TIMER5,
    GSTK_TR_TIMER6,
    GSTK_TR_TIMER7,
    GSTK_TR_TIMER8,
    GSTK_LOC_INFO_TIMER,
    GSTK_CLIENTS_REG_TIMER,
    GSTK_TR_DELAY_TIMER,
    GSTK_TIMER_MGMT_TIMER,
    GSTK_ATTACH_CTRL_TIMER,
    GSTK_TR_TIMER_INVALID,
    GSTK_TR_TIMER_MAX     = 0x7FFFFFFF
}
gstk_timer_enum_type;

typedef enum
{
  GSTK_SEARCH_BEGIN_STR =  0,
  GSTK_SEARCH_END_STR,
  GSTK_SEARCH_SINGLE_CHAR,
  GSTK_SEARCH_MAX       = 0xFF
}
gstk_search_string_type;

/* This is used for substration from the channel ID provided by the SIM */
#define GSTK_CHANNEL_ID_BASE      0x20
/* This is the max Channel ID value according to spec */
#define GSTK_CHANNEL_ID_MAX_VALUE 0x27

/* Refresh Mode Types - Spec Values */
typedef enum
{
  GSTK_REFRESH_SIM_INIT_AND_FULL_FILE_CHANGE = 0,
  GSTK_REFRESH_FILE_CHANGE_NOTIFICATION,
  GSTK_REFRESH_SIM_INIT_AND_FILE_CHANGE,
  GSTK_REFRESH_SIM_INIT,
  GSTK_REFRESH_SIM_RESET,
  GSTK_REFRESH_USIM_APP_RESET,
  GSTK_REFRESH_3G_SESSION_RESET,
  GSTK_REFRESH_STEERING_OF_ROAMING,

  GSTK_REFRESH_REFRESH_MODE_NONE,
  GSTK_MAX_REFRESH_MODES
} gstk_refresh_modes_enum_type;

typedef enum {
    GSTK_AS_1X_SESSION = 0,
    GSTK_AS_GW_SESSION,
    GSTK_AS_MAX_SESSION
}
gstk_as_sessions_enum_type;

/* -------------------------------------------------------------------------
                            Terminal Profile Type Defs
---------------------------------------------------------------------------*/

/* List of Tags, e.g., 0x01 or 0x81 = same tag */
typedef enum
{
    GSTK_NO_TAG                                               = 0x00,
    GSTK_COMMAND_DETAILS_TAG                                  = 0x01,
    GSTK_DEVICE_IDENTITY_TAG                                  = 0x02,
    GSTK_RESULT_TAG                                           = 0x03,
    GSTK_DURATION_TAG                                         = 0x04,
    GSTK_ALPHA_IDENTIFIER_TAG                                 = 0x05,
    GSTK_ADDRESS_TAG                                          = 0x06,
    GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG              = 0x07,
    GSTK_CALLED_PARTY_SUB_ADDRESS_TAG                         = 0x08,
    GSTK_SS_STRING_TAG                                        = 0x09,
    GSTK_USSD_STRING_TAG                                      = 0x0A,
    GSTK_SMS_TPDU_TAG                                         = 0x0B,
    GSTK_CELL_BROADCAST_PAGE_TAG                              = 0x0C,
    GSTK_TEXT_STRING_TAG                                      = 0x0D,
    GSTK_TONE_TAG                                             = 0x0E,
    GSTK_ITEM_TAG                                             = 0x0F,
    GSTK_ITEM_IDENTIFIER_TAG                                  = 0x10,
    GSTK_RESPONSE_LENGTH_TAG                                  = 0x11,
    GSTK_FILE_LIST_TAG                                        = 0x12,
    GSTK_LOCATION_INFORMATION_TAG                             = 0x13,
    GSTK_IMEI_TAG                                             = 0x14,
    GSTK_HELP_REQUEST_TAG                                     = 0x15,
    GSTK_NETWORK_MEASUREMENT_RESULTS_TAG                      = 0x16,
    GSTK_DEFAULT_TEXT_TAG                                     = 0x17,
    GSTK_ITEMS_NEXT_ACTION_INDICATOR_TAG                      = 0x18,
    GSTK_EVENT_LIST_TAG                                       = 0x19,
    GSTK_CAUSE_TAG                                            = 0x1A,
    GSTK_LOCATION_STATUS_TAG                                  = 0x1B,
    GSTK_TRANSACTION_IDENTIFIER_TAG                           = 0x1C,
    GSTK_BCCH_CHANNEL_LIST_TAG                                = 0x1D,
    GSTK_ICON_IDENTIFIER_TAG                                  = 0x1E,
    GSTK_ITEM_ICON_IDENTIFIER_LIST_TAG                        = 0x1F,

    GSTK_CARD_READER_STATUS_TAG                               = 0x20,
    GSTK_CARD_ATR_TAG                                         = 0x21,
    GSTK_C_APDU_TAG                                           = 0x22,
    GSTK_R_APDU_TAG                                           = 0x23,
    GSTK_TIMER_IDENTIFIER_TAG                                 = 0x24,
    GSTK_TIMER_VALUE_TAG                                      = 0x25,
    GSTK_DATE_TIME_AND_TIME_ZONE_TAG                          = 0x26,
    GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG                    = 0x27,
    GSTK_AT_COMMAND_TAG                                       = 0x28,
    GSTK_AT_RESPONSE_TAG                                      = 0x29,
    GSTK_BC_REPEAT_INDICATOR_TAG                              = 0x2A,
    GSTK_IMMEDIATE_RESPONSE_TAG                               = 0x2B,
    GSTK_DTMF_STRING_TAG                                      = 0x2C,
    GSTK_LANGUAGE_TAG                                         = 0x2D,
    GSTK_TIMING_ADVANCE_TAG                                   = 0x2E,
    GSTK_AID_TAG                                              = 0x2F,

    GSTK_BROWSER_IDENTITY_TAG                                 = 0x30,
    GSTK_URL_TAG                                              = 0x31,
    GSTK_IMS_URI_TAG                                          = 0x31,
    GSTK_BEARER_TAG                                           = 0x32,
    GSTK_PROVISIONING_REF_FILE_TAG                            = 0x33,
    GSTK_BROWSER_TERMINATION_CAUSE_TAG                        = 0x34,
    GSTK_BEARER_DESCRIPTION_TAG                               = 0x35,
    GSTK_CHANNEL_DATA_TAG                                     = 0x36,
    GSTK_CHANNEL_DATA_LEN_TAG                                 = 0x37,
    GSTK_CHANNEL_STATUS_TAG                                   = 0x38,
    GSTK_BUFFER_SIZE_TAG                                      = 0x39,
    GSTK_CARD_READER_IDENTIFIER_TAG                           = 0x3A,

    GSTK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG                = 0x3C,

    GSTK_OTHER_ADDRESS_TAG                                    = 0x3E,
    GSTK_ACCESS_TECHNOLOGY_TAG                                = 0x3F,

    GSTK_DNS_ADDRESS_TAG                                      = 0x40,

    GSTK_ESN_TAG                                              = 0x46,
    GSTK_NETWORK_ACCESS_NAME_TAG                              = 0x47,
    GSTK_CDMA_SMS_TPDU_TAG                                    = 0x48,

    GSTK_TEXT_ATTRIBUTE_TAG                                   = 0x50,
    GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG                     = 0x52,

    GSTK_CSG_CELL_SLECTION_STATUS_TAG                         = 0x55,
    GSTK_CSG_ID_TAG                                           = 0x56,
    GSTK_HNB_NAME_TAG                                         = 0x57,

    GSTK_IMEISV_TAG                                           = 0x62,
    GSTK_BATTERY_STATUS_TAG                                   = 0x63,
    GSTK_NW_SEARCH_MODE_TAG                                   = 0x65,
    GSTK_FRAME_IDENTIFIER_TAG                                 = 0x68,
    GSTK_UTRAN_MEASUREMENT_TAG                                = 0x69,
    GSTK_MEID_TAG                                             = 0x6D,
    GSTK_PLMNWACT_LIST_TAG                                    = 0x72,
    GSTK_ROUTING_AREA_TAG                                     = 0x73,
    GSTK_UPDATE_ATTACH_TYPE_TAG                               = 0x74,
    GSTK_REJECTION_CAUSE_TAG                                  = 0x75,
    GSTK_IMPU_LIST_TAG                                        = 0x77,
    GSTK_IMS_STATUS_CODE_TAG                                  = 0x78,
    GSTK_ACTIVATE_DESCRIPTOR_TAG                              = 0x7B,
    GSTK_EPS_PDN_CONN_PARAM_TAG                               = 0x7C,
    GSTK_TRACKING_AREA_TAG                                    = 0x7D,
    GSTK_CSG_ID_LIST_TAG                                      = 0x7E
    /* 0x3B, 0x3D and 0x40-0x46, 0x49-0x4F, 0x51, 0x53-0x61, 0x64, 0x66-0x67,
     * 0x6A-0x6C, 0x6E-0x71 not used */
}
tags_enum_type;

typedef enum
{
  GSTK_MMGSDI_CARD_ERROR_EVT           = 0x00,
  GSTK_MMGSDI_CARD_INSERTED_EVT,
  GSTK_MMGSDI_TERMINAL_PROFILE_DL_EVT,
  GSTK_MMGSDI_SESSION_CHANGED_EVT,
  GSTK_MMGSDI_SUBSCRIPTION_READY_EVT,
  GSTK_MMGSDI_REFRESH_EVT,
  GSTK_MMGSDI_LOC_ENV_RSP_RCVD_CNF_EVT,
  GSTK_MMGSDI_EVT_MAX                  = 0x7F
}
gstk_mmgsdi_evt_enum_type;

typedef enum
{
  GSTK_RECOVERY_START_CMD,
  GSTK_RECOVERY_END_CMD
}
gstk_recovery_cmd_enum_type;

typedef enum
{
  GSTK_TLV_PARSING_ERROR,
  GSTK_TLV_PARSING_CONTINUE,
  GSTK_TLV_PARSING_DONE
}
gstk_parsing_result_enum_type;

typedef enum
{
  GSTK_MM_IDLE_EVT                = 0x00,
  GSTK_EMM_IDLE_EVT               = 0x01,
  GSTK_IDLE_EVT_MAX               = 0x02  /* max is used for boundary check */
}
gstk_idle_evt_enum_type;

typedef enum
{
  GSTK_OTASP_CALL_ORIG            = 0x00,
  GSTK_OTASP_CALL_CMD_ERROR       = 0x01,
  GSTK_OTASP_CALL_MAX             = 0x7F
}
gstk_otasp_cmd_enum_type;

typedef enum
{
  GSTK_APP_BMSK_NONE              = 0x00000000,
  GSTK_APP_SIM_BMSK               = 0x00000001,
  GSTK_APP_RUIM_BMSK              = 0x00000002,
  GSTK_APP_USIM_BMSK              = 0x00000004,
  GSTK_APP_CSIM_BMSK              = 0x00000008,
  GSTK_APP_ALL_BMSK               = 0X0000000F
}
gstk_app_bmsk_enum_type;

typedef enum
{
  GSTK_ACCESS_TECH_3GPP              = 0x00, /**< 3GPP is available. */
  GSTK_ACCESS_TECH_3GPP2             = 0x01, /**< 3GPP2 is available. */
  GSTK_ACCESS_TECH_NONE              = 0x7FFFFFFF
    /**< Maximum value of the Access Technology group senumerator. */
}gstk_access_technology_group_type;

typedef enum
{
  GSTK_BIP_SESSION_IN_PROGRESS    = 0x00,
  GSTK_BIP_SESSION_END            = 0x01
}
gstk_bip_status_data_type;

#define GSTK_TIMER_ELAPSED 1800000 /* 30 minutes */
typedef struct
{
   rex_timer_type         timer;
   uint32                 time; /* millisecond */
   boolean                timer_started;
   boolean                pending;
   boolean                waiting_rsp;
   rex_timer_type         timer_elapsed;
} gstk_rex_timer_type;

/*----------------------------------------------------------------------------
         SIMPLE TLVs that are required for the Terminal Response
-----------------------------------------------------------------------------*/

/* 8.1, 8.14 */
/* map to gstk_address_type */
typedef struct
{
   uint8              tag;                           /* GSTK_ADDRESS_TAG,
                                                       GSTK_SS_STRING_TAG */
   uint8              length;                        /*Q: various size length */
   uint8              ton;                           /* coded in ADN EF */
   uint8              address[GSTK_MAX_BCD_ADDRESS_LEN];
                                                    /* coded as for ADN EF, may
                                                       include DTMF separators
                                                       and DTMF digits */
}
gstk_address_tag_tlv_type;

/* 8.2 */
/* map to gstk_string_type, ignore dcs */
typedef struct
{
    uint8             tag;                           /* GSTK_ALPHA_IDENTIFIER_TAG */
    uint8             length;
    uint8             *alpha;
}
gstk_alpha_tlv_type;

/* 8.3 */
/* map to gstk_subaddress_type */
typedef struct
{
    uint8             tag;                           /* GSTK_CALLED_PARTY_SUB_ADDRESS_TAG */
    uint8             length;                        /*Q: various size length!!! */
    uint8             subaddress[GSTK_MAX_SUBADDR_LEN];         /* include all information specified
                                                          ** in 3G 24.008 except the information element id
                                                          ** and length
                                                          */
}
gstk_subaddress_tlv_type;

/* 8.4 */
/* map to gstk_cap_config_type */
typedef struct
{
    uint8             tag;                                 /* GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG */
    uint8             length; /*Q: various size length!!! */
    uint8             ccp[GSTK_MAX_CCP_LEN];                    /* Capability Configuration Parameters */
               /* ccp is coded as in EF-CCP */
               /* Questionssss What kind of parsing is being done and by whom in the current EF-CCP read?
                  which module is using this information?
                  if the parsing function exists, can other layers use this api to parse?
                  what kind of format is expected for ccp before it is being sent down to lower layer,
                  who expects that? cm? nas????
                  */
}
gstk_cap_config_tlv_type;

/* 8.5 */
/* map to gstk_cb_page_type */
typedef struct
{
  uint8                                    tag;                 /* GSTK_CELL_BROADCAST_PAGE_TAG */
  uint8                                    length;
  uint8                                   *cb_page;
}
gstk_cell_broadcast_page_tag_tlv_type;

typedef struct
{
   uint8              tag;                               /* GSTK_DEVICE_IDENTITY_TAG */
   uint8              device_tag_length;                 /* GSTK_DEVICE_IDENTITY_LEN */
   uint8              source_device;
   uint8              target_device;
}
gstk_device_identities_tag_tlv_type;

typedef struct
{
  uint8               tag;
  uint8               location_info_length;
  uint8               mcc_and_mnc[3];
}
gstk_location_information_tag_tlv_type;

/* 8.8 */
/* map to gstk_duration_type */
typedef struct
{
   uint8               tag;                              /* GSTK_DURATION_TAG */
   uint8               length;                           /* GSTK_DURATION_LEN */
   uint8               time_unit;                        /* GSTK_DURATION_MINUTES,
                                                           GSTK_DURATION_SECONDS,
                                                           GSTK_DURATION_TENTHS_OF_SECONDS */
   uint8               time_interval;                    /* 0x00 - reserved
                                                           0x01 - 1 unit
                                                           0x02 - 2 units
                                                            ...
                                                           0xFF - 255 units */
}
gstk_duration_tlv_type;

/* 8.10 */
typedef struct
{
   uint8               tag;                             /* GSTK_ITEM_IDENTIFIER_TAG */
   uint8               length;                          /* GSTK_ITEM_IDENTIFIER_LEN */
   uint8               identifier;                      /* value between 0x01 - 0xFF,
                                                          null item = 0x00 */
}
gstk_item_identifier_tlv_type;

/* 8.12 */
/* map to gstk_general_result_enum_type and gstk_additonal_info_code_enum_type
** in regular terminal response from the gstk_send_terminal_response()
** the same structure is used in terminal response of a set up call/send ss or send ussd
** which has a new call type, in that case, the result, and additional_info can be
** obtained from the members of the gstk_ussd_rsp_extra_param_type and
** gstk_call_ss_rsp_extra_param_type in the gstk_send_terminal_response()
*/
typedef struct
{
    uint8                                    result_tag;           /* GSTK_RESULT_TAG */
    uint8                                    result_tag_length; /*Q: variable size length!!! */
    uint8                                    result;               /* defined in gstk_exp.h */
    uint8                                    *additional_info;      /* defined in gstk_exp.h */
}
gstk_result_tag_tlv_type;

/* 8.13 */
/* map to gstk_sms_tpdu_type */
typedef struct
{
    uint8             tag;                               /* GSTK_SMS_TPDU_TAG */
    uint8             length;                       /*Q: variable size length!!! */
    uint8             sms_tpdu[GSTK_MAX_RAW_SMS_LEN];
}
gstk_sms_tpdu_tag_tlv_type;

/* 8.15, 8.17 */
/* map to gstk_string_type, gstk_ussd_string_type */
typedef struct
{
   uint8              tag;                          /* GSTK_TEXT_STRING_TAG, GSTK_USSD_STRING_TAG */
   uint8              length;                       /* null text string => length == 0 */
   /* Q: variable size length */
   uint8              data_coding_scheme;           /* as in SMS data coding scheme in TS 23.038 */
   uint8              *text;
}
gstk_string_tlv_type;

/* 8.19 */
/* Used in Event Download for Location Status, MO SMS Control and CC
** GSTK will request the mcc, mnc, lac and cell id from RR, RRC or MM
** accordingly
*/

typedef struct
{
   /* based on 3G 24.008 */
   uint8                        mcc_and_mnc[3];
   uint8                        loc_area_code[2];
   uint8                        cell_id[2];        /* Converted from a 16-bit word */
   uint8                        ext_cell_id[2];
}
gstk_location_info_tlv_rsp_type;

typedef struct
{
   /* based on CCAT 3GPP2 C.S0035*/
   uint8                        mcc[2];
   uint8                        imsi_11_12;
   uint8                        sid[2];
   uint8                        nid[2];
   uint8                        base_id[2];
   uint8                        base_lat[3];
   uint8                        base_long[3];
}
gstk_cdma_location_info_tlv_rsp_type;

/* map to gstk_location_info_rsp_type */
typedef struct
{
   uint8                            tag;             /* GSTK_LOCATION_INFORMATION_TAG */
   uint8                            length;          /* GSTK_LOCATION_INFORMATION_LEN */
   gstk_location_info_tlv_rsp_type  location_info;
}
gstk_location_information_tlv_type;

typedef struct
{
   uint8                                  tag;             /* GSTK_LOCATION_INFORMATION_TAG */
   uint8                                  length;          /* GSTK_LOCATION_INFORMATION_LEN */
   gstk_cdma_location_info_tlv_rsp_type   cdma_loc_info;
}
gstk_cdma_location_information_tlv_type;

typedef struct
{
   uint8                                  tag;             /* GSTK_LOCATION_INFORMATION_TAG */
   uint8                                  length;          /* GSTK_LOCATION_INFORMATION_LEN */
   uint8                                  *loc_info;
}
gstk_cdma_loci_ext_tlv_type;

/* 8.20 */
typedef struct
{
   uint8                         tag;            /* GSTK_IMEI_TAG */
   uint8                         length;         /* GSTK_IMEI_LEN */
   uint8                         imei[GSTK_IMEI_LEN]; /* based on 3G 24.008 */
}
gstk_imei_tlv_type;

typedef struct
{
   uint8                         tag;                /* GSTK_ESN_TAG */
   uint8                         length;             /* GSTK_ESN_LEN */
   uint8                         esn[GSTK_ESN_LEN];  /* based on CAT Spec */
}
gstk_esn_tlv_type;

typedef struct
{
   uint8                         tag;                      /* GSTK_IMEISV_TAG */
   uint8                         length;                   /* GSTK_IMEISV_LEN */
   uint8                         imeisv[GSTK_IMEI_SV_LEN]; /* based on CAT spec */
}
gstk_imeisv_tlv_type;

typedef struct
{
   uint8                         tag;                 /* GSTK_MEID_TAG */
   uint8                         length;              /* GSTK_MEID_LEN */
   uint8                         meid[GSTK_MEID_LEN]; /* based on CAT Spec */
}
gstk_meid_tlv_type;

/* 8.21 */
typedef struct
{
   uint8                         tag;             /* GSTK_HELP_REQUEST_TAG */
   uint8                         length;          /* GSTK_HELP_REQUEST_LEN */
}
gstk_help_request_tlv_type;

/* 8.22 */
typedef struct
{
   uint8                         tag;                 /* GSTK_NETWORK_MEASUREMENT_RESULTS_TAG */
   uint8                         length;              /* GSTK_NMR_LEN */
   uint8                         nmr[GSTK_NMR_LEN];   /* based on 3G 24.008 starting at octet 2 */
}
gstk_nmr_tlv_type;

/* 8.25 */

/* Set Up event proactive command request is transformed to various registration to various clients
** in GSTK, thus, there is no req_type for setup_evt proactive cmd, and GSTK will build the TR based
** on whether the registration with the client's event notification function is successful or not */


/* The possible event that the set up event list
** command that the SIM/USIM can request the ME
** to monitor
*/
typedef enum
{
   /* values based on standard */
   GSTK_MT_CALL                        = 0x00,
   GSTK_CALL_CONNECTED                 = 0x01,
   GSTK_CALL_DISCONNECTED              = 0x02,
   GSTK_LOCATION_STATUS                = 0x03,
   GSTK_USER_ACTIVITY                  = 0x04,
   GSTK_IDLE_SCRN_AVAILABLE            = 0x05,
   GSTK_CARD_READER_STATUS             = 0x06,
   GSTK_LANGUAGE_SELECTION             = 0x07,
   GSTK_BROWSER_TERMINATION            = 0x08,
   GSTK_DATA_AVAILABLE                 = 0x09,
   GSTK_CHANNEL_STATUS                 = 0x0A,
   GSTK_ACCESS_TECH                    = 0x0B,
   GSTK_NW_SEARCH_MODE                 = 0x0E,
   GSTK_NW_REJECTION                   = 0x12,
   GSTK_HCI_CONNECTIVITY               = 0x13,
   GSTK_CSG_CELL_SELECTION             = 0x15,
   GSTK_IMS_REGISTRATION               = 0x17
}
gstk_evt_list_code_enum_type;

/* The possible originations for call to gstk_util_process_mm_or_mode_change API
   either MM idle or Mode change*/
typedef enum
{
  GSTK_MODE_CHANGE_ORIG                = 0x0,
  GSTK_MM_IDLE_ORIG                    = 0x1,
  GSTK_ORIG_INVALID                    = 0x3
}
gstk_cmd_check_orig_enum_type;

typedef enum
{
  GSTK_RAT_UMTS_IDX                   = 0x0,
  GSTK_RAT_LTE_IDX                    = 0x1,
  GSTK_RAT_MAX_IDX
}gstk_rat_idx_enum_type;

/* map to gstk_evt_list_type */

typedef struct
{
   uint8                         tag;             /* GSTK_EVENT_LIST_TAG */
   uint8                         length;          /* variable size length field
                                                 ** according to standard
                                                 ** however, at this point,
                                                 ** since GSTK_MAX_EVENT_LIST_LEN == 11
                                                 ** we can safely set this member
                                                 ** as a uint8
                                                 ** Changes will require when more
                                                 ** events are supported in the
                                                 ** standard */
   uint8                         event_list[GSTK_MAX_EVENT_LIST_LEN]; /* map to gstk_evt_list_code_enum_type */
}
gstk_evt_list_tlv_type;


/* 8.26 */
/* map to gstk_cause_type */
typedef struct
{
   uint8                         tag;             /* GSTK_CAUSE_TAG */
   uint8                         length;          /* valid values: 0, [2, 30] */
   uint8                         *cause;
    /* coded according to Cause call control information element in 3G 24.008,
       starting at octet 3, i.e., remove the  length and IEI information from 3G 24.008
       - radio link timeout is indicated by the cause data object having a value part of 0 length */

}
gstk_cause_tlv_type;

/* 8.27 */


/* map to gstk_location_status_enum_type */
typedef struct
{
   uint8                           tag;             /* GSTK_LOCATION_STATUS_TAG */
   uint8                           length;          /* GSTK_LOCATION_STATUS_LEN */
   uint8                           location_status;  /* = gstk_location_status_enum_type */
                                                   /* GSTK_NORMAL_SERVICE
                                                   ** GSTK_LIMITED_SERVICE
                                                   ** GSTK_NO_SERVICE
                                                   */
}
gstk_location_status_tlv_type;

/* 8.28 */
/* map to gstk_transaction_id_type */
typedef struct
{
   uint8                           tag;             /* GSTK_TRANSACTION_IDENTIFIER_TAG */
   uint8                           length;
   uint8                           *transaction_list;
        /* - each transaction id shall not appear more than once in the list
           - bits 1-4: RFU
             bits 5-7: TI value
             bits 8  : TI flag
           - TI value and flag are coded as defined in 3G 24.007
        */
}
gstk_transaction_id_tlv_type;

/* 8.29 */
typedef struct
{
   uint8                         tag;            /* GSTK_BCCH_CHANNEL_LIST_TAG */
   uint8                         length;
   uint8                         *channel_list;
         /* list of absolute RF channels for BCCH carriers,
         ** it consists of 1-3 BCCH channel sub lists
         ** each ARFCN is represented by 10 bits, with spare bits == 0
         ** section 8.29
         */
}
gstk_bcch_ch_list_tlv_type;

typedef struct
{
     gstk_nmr_tlv_type                  network_measurement;
     gstk_bcch_ch_list_tlv_type         bcch_ch_list;
}
gstk_nmr_bcch_tlvs_type;

typedef struct
{
   uint8                         tag;
   uint8                         length;
   uint8                         *measurement_report_ptr;
}
gstk_nmr_utran_tlv_type;

/* 8.42 */
/* map to gstk_bc_repeat_ind_enum_type */
typedef struct
{
    uint8                         tag;             /* GSTK_BC_REPEAT_INDICATOR_TAG */
    uint8                         length;          /* GSTK_BC_REPEAT_IND_LEN */
    uint8                         bc_repeat_indicator;
}
gstk_bc_repeat_ind_tlv_type;

/* 8.30 */
/* map to gstk_cc_req_action_type
** Procedure:
** length = gstk_cc_req_action_type's length
** type casting based on gstk_cc_req_action_type's call_type
** after casting, gstk_address_string_tlv will be mapped to the correct
** gstk_address_string from the gstk_cc_req_action_type
** map the rest according too
**
** When this tlv is used in the TR of setup call, send ss and send ussd,
** the card can base on the total length field to determine if these
** info are included, and if they are included, the extra result
** field in the corresponding terminal response will
** indicate the response of the CC initiated command.
** and the tag field of the gstk_address_string_tlv union can be used
** to determine what kind of
** call we are switching to
**
** SIM/USIM's CC envelope response is also expressed in this format,
** in that case, GSTK do not populate this structure, instead it will
** populate the gstk_cc_req_action_type for the client directly
** based on the Tag value of the "union", the gstk_cc_req_action_type'
** call type can be set correctly as well as other field
*/
typedef struct
{
   uint8                           tag;            /* GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG */
   uint8                           length;         /* Q:  variable size len */
   /* cc requested action, starting from the first optional tlv in the
   ** envelope response to a Call Control envelope response */

   /* Optional in the envelope response to CC */

   union
    {
        gstk_address_tag_tlv_type          address;         /* set up call */
        gstk_address_tag_tlv_type          ss_string;       /* send ss     */
        gstk_string_tlv_type               ussd_string;     /* send ussd   */
    }gstk_address_string_tlv;

   gstk_subaddress_tlv_type         subaddress;      /* for set up call only */
   gstk_cap_config_tlv_type         ccp1;            /* for set up call only */
   gstk_cap_config_tlv_type         ccp2;            /* for set up call only */
   gstk_alpha_tlv_type              alpha;
   gstk_bc_repeat_ind_tlv_type      bc_repeat_ind;   /* avaliable only if ccp2 is presented */

   /* section 7.3.1.6 response data */
}
gstk_cc_req_action_tlv_type;

/* 8.39 */
/* map to gstk_date_time_rsp_type */
typedef struct
{
   uint8                         tag;            /* GSTK_DATE_TIME_AND_TIME_ZONE_TAG */
   uint8                         length;         /* GSTK_DATE_TIME_AND_TIME_ZONE_LEN */
   gstk_date_time_rsp_type       date_time;      /* for time zone field, FF indicates unknown value */
}
gstk_date_time_tlv_type;

/* 8.41 */
/* map to at_rsp_type */
typedef struct
{
   uint8                         tag;             /* GSTK_AT_RESPONSE_TAG */
/* JAR
** Compiler Error
** Undefined
*/
//   gstk_at_rsp_type             at_rsp;
   /* rsp string */
}
gstk_at_rsp_tlv_type;

/* 8.45 */
typedef struct
{
   uint8                         tag;            /* GSTK_LANGUAGE_TAG */
   uint8                         length;         /* GSTK_LANGUAGE_LEN */
   uint8                         lang_code[2];   /* coded in SMS default 7-bit alphabet, bit 8 = 0 */
}
gstk_lang_tlv_type;

/* 8.46 */
/* GSM technology only */
/* map to gstk_time_Advance_rsp_type */
typedef struct
{
   uint8                         tag;            /* GSTK_TIMING_ADVANCE_TAG */
   uint8                         length;         /* GSTK_TIMING_ADVANCE_LEN */
   gstk_time_advance_rsp_type    timing_advance;
}
gstk_timing_advance_tlv_type;

typedef struct
{
    uint8                         tag;
    uint8                         length;
    uint8                         access_technology;
}
gstk_access_technology_tlv_type;

typedef struct
{
    uint8                         tag;
    uint8                         length;
    uint8                         battery_status;
}
gstk_battery_status_tlv_type;

/* 8.37 */
typedef struct
{
   uint8                         tag;            /* GSTK_TIMER_IDENTITY_TAG */
   uint8                         length;         /* GSTK_TIMER_IDENTITY_LEN */
   uint8                         timer_id;       /* [1, 8], other values == RFU */
}
gstk_timer_id_tlv_type;

/* 8.38 */
typedef struct
{
   uint8                         tag;            /* GSTK_TIMER_VALUE_TAG */
   uint8                         length;         /* GSTK_TIMER_VALUE_LEN */
   gstk_timer_value_type         timer_value;    /* The timer value in the structure
                                                    has to be packed accordinging to
                                                    24.030, even gstk_timer_value_type
                                                    doesn't represent the value in
                                                    24.030 format */
}
gstk_timer_value_tlv_type;

/* 8.51 */
typedef struct
{
   uint8                         tag;            /* GSTK_BROWSER_TERMINATION_CAUSE_TAG */
   uint8                         length;         /* GSTK_BROWSER_TERMINATION_CAUSE_LEN */
   uint8                         cause;          /* 0 = User Terminated
                                                    1 = Error Terminated */
}
gstk_browser_term_cause_tlv_type;

/* 8.52 */
typedef struct
{
    uint8                         tag;                              /* GSTK_BEARER_DESCRIPTION_TAG */
    uint8                         bearer_description_length;        /* variable */
    uint8                         bearer_type;
    uint8                         *bearer_param;
}
gstk_bearer_description_tlv_type;

/* 8.53 */
typedef struct
{
    uint8                         tag;                   /* GSTK_CHANNEL_DATA_TAG */
    uint8                         ch_data_length;        /* variable */
    uint8                         *ch_data;
}
gstk_ch_data_tlv_type;

/* 8.54 */
typedef struct
{
    uint8                         tag;                       /* GSTK_CHANNEL_DATA_LEN_TAG */
    uint8                         ch_data_len_length;        /* GSTK_CHANNEL_DATA_LEN_LEN */
    uint8                         ch_data_len;
}
gstk_ch_data_len_tlv_type;

/* 8.55 */
typedef struct
{
    uint8                         tag;                       /* GSTK_BUFFER_SIZE_TAG */
    uint8                         buffer_size_length;        /* GSTK_BUFFER_SIZE_LEN */
    uint8                         buffer_size[GSTK_BUFFER_SIZE_LEN];
}
gstk_buffer_size_tlv_type;

/* 8.56 */
typedef struct
{
    uint8                         tag;                     /* GSTK_CHANNEL_STATUS_TAG */
    uint8                         ch_status_length;        /* GSTK_CHANNEL_STATUS_LEN */
    uint8                         ch_status[GSTK_CHANNEL_STATUS_LEN];
}
gstk_ch_status_tlv_type;

/* 8.101 */
typedef struct
{
    uint8                         tag;                     /* GSTK_CSG_CELL_SLECTION_STATUS_TAG */
    uint8                         selection_status_len;    /* GSTK_CSG_CELL_SELECTION_STATUS_LEN */
    uint8                         selection_status[GSTK_CSG_CELL_SELECTION_STATUS_LEN];
}
gstk_csg_cell_selection_status_tlv_type;

/* 8.102 */
typedef struct
{
    uint8                         tag;                     /* GSTK_CSG_ID_TAG */
    uint8                         csg_id_len;              /* GSTK_CSG_ID_LEN */
    uint8                         csg_id[GSTK_CSG_ID_LEN];
}
gstk_csg_id_tlv_type;

/* 8.103 */
typedef struct
{
    uint8                         tag;                     /* GSTK_HNB_NAME_TAG */
    uint8                         hnb_name_len;            /* GSTK_HNB_NAME_LEN */
    uint8                         hnb_name[GSTK_HNB_NAME_LEN];
}
gstk_hnb_name_tlv_type;

/* 8.104 */
typedef struct
{
    uint8                         tag;                     /* GSTK_DNS_ADDR_TAG */
    uint8                         length;                  /* variable */
    uint8                         type;                    /* IPv4 or IPv6 */
    uint8                         *addr;
}
gstk_dns_addr_tlv_type;

/* 8.111 */
typedef struct
{
    uint8                         tag;
    uint8                         impu_list_len;
    uint8                         *impu_list;
}
gstk_ims_reg_impu_list_tlv_type;

/* 8.112 */
typedef struct
{
    uint8                         tag;
    uint8                         status_code_len;
    uint8                         *status_code;
}
gstk_ims_reg_status_code_tlv_type;

typedef struct
{
    uint8                         tag;
    uint8                         length;
    uint8                         *csg_info;
}
gstk_csg_info_tlv_type;

typedef struct
{
    uint8                         tag;
    uint8                         length;
    uint8                         nw_search_mode;
}
gstk_nw_search_mode_tlv_type;

/*
** Structure common to all terminal responses
*/

typedef struct
{
   gstk_command_details_tlv_type        command_details;
   gstk_device_identities_tag_tlv_type  device_id;
   gstk_result_tag_tlv_type             result;
}
gstk_terminal_response_header_type;

/*
** Generic
*/
typedef struct
{
 //  gstk_hdr_type                      message_header;
   uint8                               num_bytes;
   gstk_terminal_response_header_type  data;
}
gstk_generic_terminal_response_msg_type;

/*
** Get Inkey or Get Input TR
*/
/* Questions: need message_header? queue_id? client_id?
** replace by D0/D1, etc to indicate to UIM that it is a
** TERMINAL_RESPONSE,
** if gstk is not synchronous response with uim,
them client_id or user_data passed in won't be
tracked all the way down andup?  only during the
instant that GSTK processes the commands?  i.e., to check
for validity? */
/* Questions to UIM: do they look at client_id or user_data if
** we pass down?  i.e., will they return in their confirmation the same info that
gstk passed down? */
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_string_tlv_type                text_string; /* map to gstk_string_type the clients pass
                                                   ** in the gstk_send_terminal_responsed()
                                                   */
}
gstk_get_input_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_string_tlv_type                text_string; /* map to gstk_string_type the clients pass
                                                   ** in the gstk_send_terminal_responsed()
                                                   */
   gstk_duration_tlv_type              duration;
}
gstk_get_inkey_terminal_response_type;

/*
** Select Item TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_item_identifier_tlv_type       item_chosen; /* map to the select_item_chosen_item
                                                   ** in the gstk_send_terminal_response()
                                                   */
}
select_item_terminal_response_type;

typedef struct
{
  uint8                   num_nmr;
  gstk_nmr_utran_tlv_type *nmr_utran_ptr;
}
gstk_nmr_utran_tlv_info_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;  /* gstk_additional_info is required only
                                               ** when command_result =
                                               ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                               ** the additional_info then should be one of the value
                                               ** in gstk_additional_info_code_enum_type */

   /* based on the match between qualifier in the command detail and
   ** gstk_local_info_enum_type, GSTK will make the correct cast to
   ** the right structure in the union
   */
   union{
     gstk_location_information_tlv_type location_information;
     gstk_cdma_loci_ext_tlv_type        cdma_ext_loc_info;
     gstk_imei_tlv_type                 imei;
     gstk_imeisv_tlv_type               imeisv;
     gstk_esn_tlv_type                  esn;
     gstk_meid_tlv_type                 meid;
     gstk_nmr_bcch_tlvs_type            nmr_bcch;
     gstk_nmr_utran_tlv_info_type       nmr_utran_info;
     gstk_date_time_tlv_type            date_time_zone;
     gstk_lang_tlv_type                 language;
     gstk_timing_advance_tlv_type       timing_advance;
     gstk_access_technology_tlv_type    access_technology;
     gstk_battery_status_tlv_type       battery_status;
     gstk_nw_search_mode_tlv_type       nw_search_mode;
     gstk_csg_info_tlv_type             csg_info;
   }local_info_rsp;
}
provide_local_info_terminal_response_type;

typedef struct
{
   uint32                              num_bytes;
   gstk_terminal_response_header_type  header;
         /* for send USSD
            additional info required when command_result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_USSD_RETURN_ERROR
                - first byte = error value given in the Facility (Return result)
                               information element returned by the network
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed       */
   gstk_string_tlv_type               text_string; /* from client response in the
                                                   ** gstk_send_terminal_response()'s
                                                   ** gstk_ussd_rsp_extra_param_type
                                                   */
   /* the following 2 tlv structure types are only required if
   ** the send USSD command has been changed to another type
   ** of command due to call control, i.e., send USSD
   ** is being changed to send SS or set up call commands
   */
   gstk_cc_req_action_tlv_type        cc_req_action;
   gstk_result_tag_tlv_type           result2_for_cc;  /* result for the new type of request
                                                       ** from network that is
                                                       ** initiated by the CC */
     /* when the new call type is SS
        additional info is required when the command_result =
            SUCCESSFUL command results
                - first byte = SS return operation code
                  rest =       SS return parameters
                from the return result component from the network
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_SS_RETURN_ERROR
                - first byte = error value given in the Facility (return result)
                               information element returned by the network
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
     */
     /* when the new call type is Set Up Call
        the additional info is required when command_result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
     */
}
send_ussd_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;
        /* additional info regular for send SS is required when command_result =
            SUCCESSFUL command results
                - first byte = SS return operation code
                  rest =       SS return parameters
                from the return result component from the network
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_SS_RETURN_ERROR
                - first byte = error value given in the Facility (return result)
                               information element returned by the network
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
          the same applies in the result2 parameters in this structure if the setup
          call is being changed to send SS
        */
        /* additional info for regular setup call when command result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
          the same applies in the result2 parameters in this structure if the send ss call
          is being changed to setup call
        */

   /* the following 2 tlv structure types are only required if
   ** the send SS or set up call command has been changed to another
   ** type of command due to call control, i.e., send SS
   ** is being changed to send USSD or set up call commands
   ** or a setup call command is being changed to send SS or send USSD
   */
   gstk_cc_req_action_tlv_type        cc_req_action;
   gstk_result_tag_tlv_type           result2_for_cc;
   /* result for the new call type response from the network */
   /* When the call type changed to USSD, additional info is required when the command_result =
            SUCCESSFUL command results
                - first byte = SS return operation code
                  rest =       SS return parameters
                from the return result component from the network
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_SS_RETURN_ERROR
                - first byte = error value given in the Facility (return result)
                               information element returned by the network
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed
    */
    /* when the call change to setup call or send SS, additional info is similar to what is
       being specified in the first result tag
    */

   /* this structure is required only if the send SS or set up call
   ** command is being modified to a send USSD after call control
   ** process in the SIM/USIM
   */
   gstk_string_tlv_type          cc_mod_text2;
}
setup_call_send_ss_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;
     /* additional info is required when command_result =
              GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
        the additional info then will be = gstk_additional_info_code_enum_type */
   gstk_duration_tlv_type              duration;
}
polling_interval_terminal_response_type;

/* additional info for the following proactive command terminal response types
** are only required if the command_result =
              GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
        the additional info then will be = gstk_additional_info_code_enum_type */
typedef gstk_generic_terminal_response_msg_type display_text_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type refresh_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type play_tone_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type setup_menu_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type more_time_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type setup_evt_list_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type setup_idle_mode_txt_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type lang_notification_terminal_response_type;
typedef gstk_generic_terminal_response_msg_type activate_terminal_response_type;

/* additional result for send DTMF terminal response is required when command_result =
              GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
              if call ends before completion of send DTMF, additional_info should
              be set to GSTK_NOT_IN_SPEECH_CALL
*/
typedef gstk_generic_terminal_response_msg_type send_dtmf_terminal_response_type;

typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;
     /* additional info is required when command_result =
              GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
        the additional info then will be = gstk_additional_info_code_enum_type */
   gstk_timer_id_tlv_type              timer_id;
   gstk_timer_value_tlv_type           timer_value;
}
timer_management_terminal_response_type;

/* additional result info for launch browser terminal response is required when command_result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_LAUNCH_BROWSER_GENERIC_ERROR
                - 00 = no specific cause can be given
                  01 = bearer unavailable
                  02 = browser unavailable
                  03 = ME unable to read the provisioning data
*/
typedef gstk_generic_terminal_response_msg_type launch_browser_terminal_response_type;

/* additional result info for send sms termainl response is required when command_result =
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_SMS_RP_ERROR
                - first byte = cause value in the RP-Cause element of the
                               RP-ERROR message returned by the network,
                               bit 8 = 0
            GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed       */
typedef gstk_generic_terminal_response_msg_type send_sms_terminal_response_type;

/*
** Open Channel TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_ch_status_tlv_type             ch_status;  /* map to the ch_status in the open_ch_extra_param
                                                   ** in the gstk_send_terminal_response()
                                                   */
   gstk_bearer_description_tlv_type    bearer_description;  /* map to the bearer description in the
                                                            ** open_ch_extra_param in the
                                                            ** gstk_send_terminal_response() */
   gstk_buffer_size_tlv_type           buffer_size; /* map to the buffer size in the
                                                    ** open_ch_extra_param in the
                                                    ** gstk_send_terminal_response() */
   gstk_dns_addr_tlv_type              *dns_addrs;  /* map to the DNS addresses */
   uint8                               num_dns_addrs; /* number of DNS addresses */
}
open_ch_terminal_response_type;

/*
** Close Channel TR
*/
typedef gstk_generic_terminal_response_msg_type close_ch_terminal_response_type;

/*
** Send Data TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_ch_data_len_tlv_type           ch_data_len;   /* map to the channel data len in the
                                                      ** send_data_extra_param in the
                                                      ** gstk_send_terminal_response() */
}
send_data_terminal_response_type;

/*
** Receive Data TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   gstk_ch_data_tlv_type               ch_data;       /* map to the channel data in the
                                                      ** receive_data_extra_param */
   gstk_ch_data_len_tlv_type           ch_data_remaining_len;   /* map to the channel data len in the
                                                      ** receive_data_extra_param in the
                                                      ** gstk_send_terminal_response() */
}
receive_data_terminal_response_type;

/*
** Get channel Status TR
*/
typedef struct
{
   uint8                               num_bytes;
   gstk_terminal_response_header_type  header;      /* additional info required when command_result =
                                                   ** GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                   ** in that case, the additonal info
                                                   ** = gstk_additional_info_code_enum_type
                                                   */
   /* map to the ch_status in the get_ch_status_extra_param
   ** in the gstk_send_terminal_response()
   */
   gstk_ch_status_tlv_type             ch_status[GSTK_MAX_GPRS_CHANNELS];
}
get_ch_status_terminal_response_type;

/*----------------------- Envelope Command (ME -> SIM/USIM) -------------------------------*/


typedef struct
{
   uint8                                 tag;               /* GSTK_EVENT_DOWNLOAD_TAG */
   uint8                                 length;            /* length of the entire evt dl cmd */
   gstk_device_identities_tag_tlv_type   device_id;
   gstk_evt_list_tlv_type                evt_list;          /* the tlv should be of length length since
                                                           ** only 1 event can be notified at a time
                                                           */
}
gstk_envelope_event_dl_cmd_header_type;

typedef gstk_envelope_event_dl_cmd_header_type gstk_generic_envelope_event_dl_command_type;

/* based on gstk_send_envelope_evt_download_command's gstk_evt_dl_ind_type */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_transaction_id_tlv_type             transacton_id;  /* 1 transaction id in the SETUP
                                                            message from the network */
   /* C */
   gstk_address_tag_tlv_type                address;        /* include when Calling Party BCD number
                                                            is received by ME in the SETUP message */
   gstk_subaddress_tlv_type                 subaddress;     /* include when Calling Party Subaddress
                                                            is received by ME in the SETUP message */
}
gstk_envelope_mt_evt_dl_command_type;

/* based on gstk_send_envelope_evt_download_command's gstk_evt_dl_ind_type */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   /* for "near end" disconnection: source = ME
   ** for "far end" disconnecton:   source = NETWORK
   */
   gstk_transaction_id_tlv_type             transacton_id;
}
gstk_envelope_call_connected_evt_dl_command_type;

/* based on gstk_send_envelope_evt_download_command's gstk_evt_dl_ind_type */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   /* for "near end" disconnection: source = ME
   ** for "far end" disconnecton:   source = NETWORK
   */
   gstk_transaction_id_tlv_type             transacton_id;
   /* Optional */
   gstk_cause_tlv_type                      cause;
}
gstk_envelope_call_disconnected_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_location_status_tlv_type            location_st;
   /* C, required if location status == NORMAL SERVICE */
   union
   {
     gstk_location_information_tlv_type       gsm_umts_location_info;
     gstk_cdma_loci_ext_tlv_type              cdma_ext_loc_info;
   }location_info;
}
gstk_envelope_location_status_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_lang_tlv_type                       language;
}
gstk_envelope_language_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_access_technology_tlv_type          access_tech;
}
gstk_envelope_access_tech_evt_dl_command_type;

typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_access_technology_tlv_type          access_tech;
   gstk_csg_cell_selection_status_tlv_type  selection_status;
   gstk_csg_id_tlv_type                     csg_id;
   gstk_hnb_name_tlv_type                   hnb_name;
}
gstk_envelope_csg_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_browser_term_cause_tlv_type         cause;
}
gstk_envelope_browser_termination_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_ch_status_tlv_type                  ch_status;
   gstk_ch_data_len_tlv_type                ch_data_len;
}
gstk_envelope_data_avail_evt_dl_command_type;

/* based on event information from various client layers */
typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   gstk_ch_status_tlv_type                  ch_status;
}
gstk_envelope_ch_status_evt_dl_command_type;

/* based on timer id and timer value for the timer expiration */
typedef struct
{
   uint8                                    tag;                 /* GSTK_TIMER_EXPIRY_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   gstk_timer_id_tlv_type                   timer_id;
   gstk_timer_value_tlv_type                timer_value;
}
gstk_envelope_timer_expiration_command_type;

typedef struct
{
   gstk_envelope_event_dl_cmd_header_type   event_download_hdr;
   /* C(onditional) */
   /* IMS Registration SUCCESS */
   gstk_ims_reg_impu_list_tlv_type          impu_list;
   /* IMS Registration FAILURE */
   gstk_ims_reg_status_code_tlv_type        status_code;
}
gstk_envelope_ims_reg_evt_dl_command_type;

/* based on the gstk_cmd_enum_type within gstk_evt_dl_ind_type in
** the gstk_send_envelope_evt_download_command */
typedef gstk_generic_envelope_event_dl_command_type gstk_envelope_user_act_evt_dl_command_type;
typedef gstk_generic_envelope_event_dl_command_type gstk_envelope_idle_scrn_evt_dl_command_type;
typedef gstk_generic_envelope_event_dl_command_type gstk_envelope_hci_conn_evt_dl_command_type;

/* based on gstk_send_envelope_menu_sel_command's help_requested and item_chosen params */
typedef struct
{
   uint8                                    tag;                 /* GSTK_MENU_SELECTION_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   gstk_item_identifier_tlv_type            item_chosen;
   /* Optional */
   gstk_help_request_tlv_type               request_help;
}
gstk_envelope_menu_selection_command_type;

/* based on gstk_send_envelope_menu_sel_command's help_requested and item_chosen params */
typedef struct
{
   uint8                                    tag;                 /* GSTK_MENU_SELECTION_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   gstk_location_information_tag_tlv_type   loc_info;
}
gstk_envelope_location_information_command_type;

/* based on gstk_send_envelope_sms_pp_dl_command's sms_tpdu and address params */
typedef struct
{
   uint8                                    tag;                 /* GSTK_SMS_PP_DOWNLOAD_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   /* Optional */
   gstk_address_tag_tlv_type                address;
   /* Mandatory */
   gstk_sms_tpdu_tag_tlv_type               tpdu;
}
gstk_envelope_sms_pp_download_command_type;

/* based on gstk_envelope_cell_broadcast_download_command_type's cb page params */
typedef struct
{
   uint8                                    tag;                 /* GSTK_SMS_PP_DOWNLOAD_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   /* Mandatory */
   gstk_cell_broadcast_page_tag_tlv_type    cb_page;
}
gstk_envelope_cell_broadcast_download_command_type;

typedef struct
{
   uint8                                    tag;                 /* GSTK_CALL_CONTROL_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   union {
     gstk_address_tag_tlv_type                address;         /* set up call */
     gstk_address_tag_tlv_type                ss_string;       /* send ss     */
     gstk_string_tlv_type                     ussd_string;     /* send ussd   */
   }gstk_address_string_tlv;
   gstk_cap_config_tlv_type                 ccp1;
   /* Optional */
   gstk_subaddress_tlv_type                 subaddress;
   /* Mandatory location information will be populated in GSTK */
   union {
     gstk_location_information_tlv_type       gw_loc_info;
     gstk_cdma_loci_ext_tlv_type              cdma_ext_loc_info;
   } loc_info;
   /* Optional */
   gstk_cap_config_tlv_type                 ccp2;
}
gstk_envelope_cc_command_type;

/* based on gstk_mo_sms_ctrl_ind_type from the gstk_send_envelope_mo_sms_ctrl_command */
typedef struct
{
   uint8                                    tag;                 /* GSTK_CALL_CONTROL_TAG */
   uint8                                    length;
   gstk_device_identities_tag_tlv_type      device_id;
   gstk_address_tag_tlv_type                address1;            /* rp_destination_address */
   gstk_address_tag_tlv_type                address2;            /* tp_destination_address */
   /* Mandatory location information will be populated in GSTK */
   union {
     gstk_location_information_tlv_type       gw_loc_info;
     gstk_cdma_loci_ext_tlv_type              cdma_ext_loc_info;
   } loc_info;
}
gstk_envelope_mo_sms_ctrl_command_type;

/* Used internally in GSTK: represent the Device ID source and destination*/
typedef struct
{
  gstk_device_identity_type  src;
  gstk_device_identity_type  dest;
} gstk_device_id_type;

/* Used internally in GSTK: represent the general location information*/
typedef struct
{
  gstk_local_info_gen_enum_type       location_info_tech;
  union
  {
    gstk_location_info_rsp_type   gsm_umts_loc_info;
    gstk_cdma_location_info_type  cdma_loc_info;
  } location_info;
}gstk_location_info_gen_type;

/* Used internally in GSTK: contains the index into the cmd details table,
   the command response type expected and the command number.
*/
typedef struct
{
  uint32                        ref_id;
  uint32                        command_rsp_type;
  uint8                         command_number;
}gstk_cmd_info_type;

/* Used internally in GSTK: contains the result and additional result values.
*/
typedef struct
{
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;
}gstk_result_type;

typedef struct
{
   gstk_device_id_type                     dev_id_data;
   gstk_cmd_info_type                      cmd_info_data;
   gstk_result_type                        result_info_data;
}gstk_cmd_term_rsp_common_type;

/* Used internally in GSTK: represent union structure for different TLV that GSTK is
   responsible for building when sending the terminal response or envelope
   command to the card */
typedef union
{
    gstk_device_id_type                     dev_id_data;
    gstk_location_info_gen_type             location_info_data;
    gstk_pdp_context_act_param_type         pdp_context_act_param_data;
    gstk_eps_pdn_conn_act_param_type        pdn_conn_act_param_data;
    gstk_cmd_info_type                      cmd_info_data;
    gstk_result_type                        result_info_data;
    gstk_nw_search_mode_enum_type           nw_search_mode_data;
    gstk_evt_list_code_enum_type            evt_list_info_data;
    gstk_ims_call_param_type                ims_call_param_data;
}
gstk_cmd_to_uim_type;

/*  GSTK_RETRY_TYPE
**  This structure contains the command retry information that is used
**  in the envelope details reference table.
*/
typedef struct
{
  boolean                retry_flag;        /* Should retry be attempted for this envelope */
  int32                  retry_counter;     /* retry counter (modified as retries are attempted by gstk */
  gstk_cmd_enum_type     cmd_type;/* the command we attempt to retry */
}
gstk_retry_type;

/*  GSTK_PENDING_ENV_INFO_TYPE
**  This structure contains the retry and processing state of the event download
**  envelope that is currently being processed.
*/
typedef struct
{
    gstk_env_proc_substate_enum_type  gstk_env_state;
    gstk_retry_type                   retry_info;
    /* actual payload bytes sent through uim */
    gstk_generic_data_type            env_data_bytes;
    /* Refers to the envelope reference id in the envelope reference table */
    uint32                            env_ref_id;
    /* Timer to retry envelope command to the card - 15 seconds timer */
    rex_timer_type                    gstk_env_retry_timer;
}
gstk_pending_env_info_type;

typedef struct
{
    boolean                                  is_free;
    gstk_client_id_type                      client_id;
    gstk_cmd_enum_type                       expected_cmd_rsp_type;
    uint32                                   user_data;
    uint32                                   unique_ref_id;
    gstk_client_reg_cb_func_format_enum_type evt_cb_type;
    union
    {
      gstk_evt_cb_funct_type                  client_env_cb;
      gstk_toolkit_evt_cb_func_type           new_client_env_cb;
    }env_cb;
    gstk_cc_ind_type_ext                     gstk_cc_cache_info;
    gstk_evt_list_code_enum_type             evt_list_code;
}
gstk_env_cmd_ref_table_type;

typedef struct
{
    boolean                                 near_end;
    gstk_transaction_id_type                transaction_id;
}
gstk_transaction_id_direction_type;

typedef struct
{
    boolean                      is_loc_info_valid;
    gstk_cdma_location_info_type loc_info;
}
gstk_cdma_loc_info_cache_type;

typedef struct
{
    boolean                           is_cdma_info_valid;
    boolean                           is_advance_info_valid;
    boolean                           is_hdr_info_valid;
    gstk_cdma_loci_ext_type           data;
}
gstk_cdma_loci_ext_cache_type;

/* Table to maintain RRC PLMN/LAC/Cell information */
typedef struct
{
  uint8 plmn_id[GSTK_MAX_PLMN_LEN];
  uint8 lac[GSTK_MAX_LAC_LEN];
  gstk_cell_id_type cell_info;
}gstk_rrc_plmn_lac_cellInfo_table;

/* Table to maintain MM PLMN/LAC/Location Status information */
typedef struct
{
  uint8 plmn_id[GSTK_MAX_PLMN_LEN];
  uint8 lac[GSTK_MAX_LAC_LEN];
  gstk_cell_id_type cell_info;
  gstk_location_status_enum_type loc_status;
}gstk_plmn_lac_cellInfo_table;

#ifdef FEATURE_LTE
typedef union
{
  msgr_hdr_s                             msg_hdr; /*!< Message router header */
  lte_rrc_service_ind_s                  rrc_service_ind ;
  lte_rrc_plmn_search_cnf_s              rrc_plmn_search_cnf;
  lte_rrc_nmr_info_cnf_s                 rrc_nmr_cnf;
} gstk_rrc_msgr_cmd_type ;

typedef struct
{
  q_link_type         link; /*!< Queue link. This must be the first element of the cmd_type.
                            MSGR expects q_link to be the first element and the msgr_hdr_s
                            as the second element.So, after link_type no element should be
                            added and the only element is the union below*/


  /*! @brief List all the incoming messages/commands*/
  gstk_rrc_msgr_cmd_type  rrc_msgr_cmd ;
} gstk_rrc_msg_type;
#endif /* FEATURE_LTE */

/* Table to maintain mapping of modem AS id and mmgsdi sessio type */
typedef struct
{
  boolean                       is_valid;
  sys_modem_as_id_e_type        as_id;
  mmgsdi_session_type_enum_type sess_types[GSTK_AS_MAX_SESSION];
} gstk_modem_as_info_type;

/* Table to maintain mmgsdi sessions information */
typedef struct
{
  mmgsdi_session_id_type        sess_id;
  mmgsdi_session_type_enum_type sess_type;
  gstk_slot_id_enum_type        slot;
} gstk_mmgsdi_session_info_type;

#if (defined(FEATURE_WCDMA) || defined(FEATURE_LTE))
typedef struct
{
  /* Mobile Country Code - one digit per byte range 0..9)  */
  byte mcc[GSTK_RRC_MAX_MCC_DIGITS];
  /* Number of MNC digits */
  byte num_mnc_digits;
  /* Mobile Network Code - one digit per byte (range 0..9) */
  byte mnc[GSTK_RRC_MAX_MNC_DIGITS];
}gstk_rrc_plmn_identity_type;

typedef struct
{
  /* Indicates the service cell ID */
  uint32 cell_id;
  /* Indicates the selected PLMN */
  gstk_rrc_plmn_identity_type plmn_id;
  /* Indicates the LAC ID */
  uint8                     lac[GSTK_RRC_MAX_GSMMAP_LAC_LENGTH];
  /* Indicates the serving PSC */
  uint16 psc;
  /*DL UARFCN*/
  uint16 uarfcn_dl;
  /*UL UARFCN*/
  uint16 uarfcn_ul;
  sys_modem_as_id_e_type as_id;
} gstk_cell_info_type;
#endif /* FEATURE_WCDMA || FEATURE_LTE */


/*===========================================================================

  STRUCT: GSTK_MMGSDI_TP_DATA

  PURPOSE:
    The structure is used to hold MMGSDI TP related information
==========================================================================*/
typedef struct
{
  uint32                      user_data;
  gstk_evt_cb_funct_type      gstk_tp_dl_cb;
} gstk_mmgsdi_tp_data;

/*===========================================================================

  STRUCT: GSTK_WLAN_INFO_TYPE

  PURPOSE:
    The structure is used to hold info on the WLAN status
==========================================================================*/
typedef struct
{
  /* Is WLAN (local breakout) available */
  boolean                   is_wlan_available;
  /* Current WLAN MAC address */
  ds_sys_wlan_mac_addr_type wlan_mac_addr;
  /* Current WLAN SSID */
  ds_sys_wlan_ssid_type     wlan_ssid;
} gstk_wlan_info_type;

/*===========================================================================

  STRUCT: GSTK_SUBS_READY_INFO_TYPE

  PURPOSE:
    The structure is used to hold info on MMGSDI Subscription Ready
    received status
==========================================================================*/
typedef struct
{
  /* SUBS_READY status for the MMGSDI GW Session */
  boolean                   subs_ready_gw;
  /* SUBS_READY status for the MMGSDI 1X Session */
  boolean                   subs_ready_1x;
} gstk_subs_ready_info_type;

/*===========================================================================

  STRUCT: GSTK_NW_REJ_INFO

  PURPOSE:
    The structure is used to hold information related to network reject information
    given by NAS
==========================================================================*/
typedef struct
{
  /* PLMN/LAC of NW reject from NAS */
  uint8                        mcc_mnc[GSTK_MAX_PLMN_LEN];
  uint8                        lac[GSTK_MAX_LAC_LEN];
  /* RAT */
  gstk_access_technology_type  access_tech;
} gstk_nw_rej_info;

/*===========================================================================

  STRUCT: GSTK_PRIV_DATA_TYPE

  PURPOSE:
    The structure is to hold GSTK private data
==========================================================================*/
typedef struct
{
  gstk_slot_id_enum_type             slot_id;
  /* the cache of setup evt list terminal response */
  gstk_setup_evt_list_cnf_type       setup_evt_list_cnf_cache;
  /* GSTK event registration information table */
  gstk_class_evt_reg_info_table_type gstk_class_evt_reg_table[GSTK_TOTAL_ELEMENTS_IN_CLASS_ARRAY];
  /* the cache of TPs sent by GSTK clients */
  gstk_terminal_profile_cmd_type   * client_tp_ptr[GSTK_MAX_CLIENT];
  /* This variable is set to TRUE when gstk allows UIM to fetch proactive command
   from card. And it is set to FALSE when gstk sends Terminal Response to UIM.
   It is used to check if a proactive command is in progress from card and TR
   has not been sent to it */
  boolean                            gstk_proactive_cmd_in_progress;
  /* location information */
  uint8                              gstk_curr_mcc_mnc[GSTK_MAX_PLMN_LEN];
  uint8                              gstk_curr_lac[GSTK_MAX_LAC_LEN];
  gstk_cell_id_type                  gstk_curr_cell_id;
  gstk_location_status_enum_type     gstk_curr_location_status;
  uint8                              gstk_next_curr_mcc_mnc[GSTK_MAX_PLMN_LEN];
  uint8                              gstk_next_curr_lac[GSTK_MAX_LAC_LEN];
  gstk_cell_id_type                  gstk_next_curr_cell_id;
  gstk_location_status_enum_type     gstk_next_curr_location_status;
  gstk_access_technology_type        gstk_curr_rat;
  gstk_access_technology_type        gstk_next_curr_rat;
  gstk_bearer_description_type       gstk_curr_bearer_description;
  int32                              gstk_curr_bearer_buffer_size;
  /* Count of terminal profile downloads sent by various clients to GSTK */
  uint8                              gstk_num_of_tp_dl_received;
  /* indicator of recieving MM parameters */
  boolean                            gstk_mm_param_received;
  /* indicator of pending location event envelope command */
  boolean                            gstk_pending_location_evt_envelope;
  /* indicator of MM state being IDLE */
  boolean                            gstk_mm_state_is_idle;
  /* PLMN given by NAS in the network reject event */
  gstk_nw_rej_info                   nw_rej;
  /* array of timer that GSTK will manage for timer management command */
  gstk_rex_timer_type                gstk_timer_management[GSTK_MAX_TIMER_SUPPORTED];
#ifdef FEATURE_CDMA
  /* 3GPP2 extended location information cache */
  gstk_cdma_loci_ext_cache_type      cdma_ext_loci_cache;
#endif /* FEATURE_CDMA */
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
  /* WCDMA location information */
  boolean                            is_rrc_cell_id_received[GSTK_RAT_MAX_IDX];
  gstk_rrc_plmn_lac_cellInfo_table   rrc_loci_table[GSTK_RAT_MAX_IDX];
#endif /* FEATURE_WCDMA || FEATURE_LTE */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
  /* consolidated location information cache */
  gstk_plmn_lac_cellInfo_table       gstk_loci_table;
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE */
  /* Cache of TZ information from CM */
  gstk_cm_ss_tz_info                 tz_info[GSTK_CM_NUM_STACKS];
  /* indicator of GSTK having recieved TP update */
  boolean                            gstk_profile_received;
  /* Current Card App Type */
  mmgsdi_protocol_enum_type          card_protocol;
  /* envelope payload, retry, state info of the currently processed envelope */
  gstk_pending_env_info_type         gstk_curr_envelope_info;
  /* Flag to indicate if current proactive session involves UI */
  boolean                            gstk_is_curr_session_ui_related;
  /* Flag to indicate, the fetch request from UIMDRV is ignored/rejected */
  boolean                            is_fetch_ignored;
  /* hold the current GSTK gstk */
  gstk_state_enum_type               gstk_state;
  /* GSTK substate */
  gstk_sub_state_enum_type           gstk_sub_state;
  /* gstk client id for current instance */
  gstk_client_id_type                client_id;
  /* terminal profile cache */
  uint8                              gstk_terminal_profile_cache[GSTK_TERMINAL_PROFILE_SIZE];
  /* event list */
  uint8                              gstk_evt_list[GSTK_MAX_EVENT_LIST_LEN];
  /* temporary cache until setup event list TR is sent*/
  uint8                              gstk_temp_evt_list[GSTK_MAX_EVENT_LIST_LEN];

  /* indicator of call being connected */
  boolean                            gstk_cm_call_is_connected;
  /* mmgsdi session id */
  mmgsdi_session_id_type             gstk_mmgsdi_session_id;
  /* flag to indicate if current proactive command is cached or not */
  boolean                            gstk_is_msg_cached;
  /* Loc Info Envelope */
  rex_timer_type                     gstk_loc_info_envelope_timer; /* 2 seconds timer */
  /* timer to wait for correct BASE ID for 1x loc status envelopes */
  rex_timer_type                     gstk_cdma_loc_status_timer; /* 5 seconds timer */
  rex_timer_type                     gstk_mm_idle_no_service_timer; /* 30 seconds timer */
  gstk_cmd_type                      *gstk_mm_idle_no_srv_from_timer;
  gstk_cmd_type                      *gstk_cdma_loc_status_cmd_ptr;
  boolean                            is_first_loc_info_envelope_sent;
  uint8                              gstk_last_loc_info_sent[3];
  gstk_subs_ready_info_type          is_subs_ready;
  boolean                            imsi_switched;
  int32                              otasp_call_id;
  /* RAT reported by (E)MM */
  gstk_access_technology_type        nas_rat;
  /* Bit mask of all 3GPP2 RATs in service */
  uint16                             bmsk_3gpp2;
  gstk_cmd_cache_type                gstk_cmd_cache;
  rex_timer_type                     gstk_terminal_rsp_delay_timer;
  /* loc info status query task*/
  gstk_attach_ctrl_type              *attach_ctrl_p;
  gstk_attach_ctrl_result_enum_type  attach_ctrl_res;
  gstk_attach_ctrl_result_enum_type  loc_info_env_res;
  rex_timer_type                     attach_ctrl_timer;
  boolean                            read_imsi;
  /* cmd details ref id of the pro cmd in progress */
  uint32                             in_prog_cmd_ref_id;
  /* indicate if already notify GSDI TP DL complete */
  boolean                            is_tp_dl_done;
  /* the bitmask of all enabled apps on the card */
  uint32                             apps_enabled;
  /* GSTK TP data */
  gstk_mmgsdi_tp_data                tp_info;
  /* the counter of SETUP EVETN LIST cmd recieved */
  uint32                             setup_evt_list_cnt;
  /* Save the MM Idle command for GSTK supporting simultaneous multi-RATs*/
  gstk_cmd_type                      *mm_idle_pri_ptr;
  /* MM Idle command cache for different stacks */
  gstk_cmd_type                      *mm_idle_cmd_ptrs[NUM_OF_MM_PER_GSTK_INST];
  /* Save the Mode Change command for GSTK supporting simultaneous multi-RATs*/
  gstk_cmd_type                      *mode_change_cmd_ptr;
  boolean                            is_tp_rsp_received;
  boolean                            bip_blocked;
  boolean                            otasp_blocked;
  /* Count of number of 1xRTT CSIM Programming attempts */
  uint8                              otasp_failure_cnt;
  boolean                            recv_otasp_commit;
    /* This variable is set to TRUE when gstk deques a BIP Envelope command from its
   command queue. And it is set to FALSE when receives an Envelope response for
   BIP Envelope command.
   It is used to check if a BIP Envelope is in progress from gstk and it has not
   yet received and Envelope response for that. */
  boolean                            gstk_bip_env_in_progress;
  /* This variable decides whether or not CALL CONTROL(CC) envelope should be blocked.
     Currently we are blocking CC envelope for - RECEIVE DATA, SEND DATA. */
  uint8                              gstk_block_cc_env;
  /*open channel pointer*/
  gstk_proactive_cmd_type*           open_ch_imsi_m_ptr;
  boolean                            recovery_enable_ls_act;
  ds_sys_system_status_ex_type       ds_sys_status_info;
#ifdef FEATURE_GSTK_1X_ACTIVATION_PREFERRED
  /* Indicates if OTASP was re-queued due to 1X becoming available/1X
     availability timer expiry */
  boolean                            otasp_requeued;
#endif /* FEATURE_GSTK_1X_ACTIVATION_PREFERRED */
  boolean                            cdma_loc_st_disabled;
  /* unique_ref_id used to check for duplicate TR responses */
  uint32                             tr_unique_ref_id;
  /* Mode of the subscription */
  gstk_subs_feature_mode_enum_type   subs_feature_mode;
  /* Is nw_search_mode successfully sent to card */
  boolean is_nw_search_mode_sent;
  /* Network Search Mode */
  gstk_nw_search_mode_enum_type      nw_search_mode;
  /* Is csg_info successfully sent to card */
  boolean is_csg_info_sent;
  /* CSG information cache */
  gstk_csg_info_type                 csg_info;
  /* List of available networks available */
  sys_detailed_plmn_list_s_type      available_csg_list;
  gstk_access_technology_group_type  gstk_curr_tech_group;
}
gstk_priv_data_type;

/*===========================================================================

  STRUCT: GSTK_SHARED_DATA_TYPE

  PURPOSE:
    The structure is to hold data shared among all GSTK instances.
==========================================================================*/
typedef struct
{
  /* Timer definitions */
  rex_timer_type                        gstk_rpt_timer;
  /* the timer to wait for client doing TP Dl */
  rex_timer_type                        gstk_tp_wait_timer;
  /* timer to wait for clients' registration */
  rex_timer_type                        gstk_clients_reg_timer;
  /* client arrays */
  gstk_client_table_type                gstk_client_table[GSTK_MAX_CLIENT];
  /* envelope command request arrays
     last slot reserved to hold env req info when env_ref_table is full */
  gstk_env_cmd_ref_table_type           envelope_cmd_ref_table[GSTK_MAX_PROACTIVE_COMMAND+1];
  /* client id from cm */
  cm_client_id_type                     gstk_client_id_from_cm;
  /* mmgsdi client id */
  mmgsdi_client_id_type                 gstk_mmgsdi_client_id;
  /* gstk critical section */
  rex_crit_sect_type                    gstk_crit_sect;
  /* Mask of signals interested in */
  rex_sigs_type                         gstk_requested_mask;
  /* flag of GSTK continuing on processing command */
  boolean                               gstk_continue_task;
  /* slot that has UI/NW proactive command session */
  gstk_slot_id_enum_type                ui_nw_proactive_session_slot_in_progress;
  /* Fetch command is in processing, to avoid parallel fetch in multi slot */
  uint8                                 fetch_is_in_progress;
#ifdef FEATURE_LTE
  msgr_client_t                         msgr_rrc_client;
  q_type                                rrc_msg_q;
  q_type                                rrc_msg_q_free;
  gstk_rrc_msg_type                     rrc_msg_buff[GSTK_NUM_CMD_BUFS];
#endif /* FEATURE_LTE */
  uint32                                nmr_ref_id;
  uint32                                num_of_expected_tp;
  gstk_modem_as_info_type               modem_as_info[GSTK_MODEM_AS_INFO_SIZE];
  gstk_mmgsdi_session_info_type         mmgsdi_sess_info[GSTK_MMGSDI_SESS_INFO_SIZE];
  /* Fetch Allowed for Slots */
  boolean                               fetch_allowed[GSTK_MAX_NUM_OF_INSTANCES];
  /* is imsi_m provisioned */
  boolean                               gstk_is_imsi_m_provisioned;
  /* is it okay to dispatch open channel to ESTK */
  boolean                               is_open_ch_ok;
  /* Pointer to cache command, when an envelope is already in progress*/
  gstk_task_cmd_type*                   gstk_cmd_ptr;
  /* GSM Loc Info reported by CM */
  uint8                                 rr_mcc_mnc[3];
  uint8                                 rr_lac[2];
  uint8                                 rr_cell_id[2];
  /* indicate if BIP and OTASP should not occur concurrently */
  /* Bitmask for marking which modem tasks are ready */
  uint16                                ready_tasks;
  gstk_io_send_mm_ind_enum_type         mm_ind_cache;
  /* Cellular DATA availability status as extracted from
     DS_SYS_EVENT_SYSTEM_STATUS_EX */
  boolean                               cellular_data_available;
  /* WLAN availability status as extracted from
     DS_SYS_EVENT_SYSTEM_STATUS_EX */
  gstk_wlan_info_type                   wlan_info;
  sys_modem_as_id_e_type                active_data_subs;
  gstk_cc_req_action_type               cc_req_action;
  wms_client_id_type                    wms_client_id;
  uint8                                 gstk_num_of_instances;
  gstk_client_otasp_status_cb_func_type gstk_otasp_status_cb;
}
gstk_shared_data_type;

typedef struct
{
  uint8                        mcc1;
  uint8                        mcc2;
  uint8                        mcc3;
}gstk_mcc_data_type;

/* the pointer to private data of current GSTK instance */
extern gstk_priv_data_type   *gstk_curr_inst_ptr;

/* the pointer to the shared data of GSTK */
extern gstk_shared_data_type gstk_shared_data;

/* the pointer to array of all GSTK instances */
extern gstk_priv_data_type   *gstk_instances_ptr[GSTK_MAX_NUM_OF_INSTANCES];

extern gstk_cmd_details_ref_table_type *command_details_ref_table_ptr;

#ifdef FEATURE_SEGMENT_LOADING
/* Interface pointers for dynamically-loaded modules TDSCDMA/WCDMA */
extern interface_t *gstk_pi_tdscdma;
extern interface_t *gstk_pi_wcdma;
#endif /* FEATURE_SEGMENT_LOADING */

/*===========================================================================
FUNCTION GSTK_WAIT

DESCRIPTION
  Main wait routine for GSTK task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals).  The calling unit can then check for the reason for
  exiting the wait.

===========================================================================*/
extern rex_sigs_type  gstk_wait( rex_sigs_type wait_sigs );

/*===========================================================================
FUNCTION GSTK_PROCESS_STATE

DESCRIPTION

  This function dispatches the command to the current state of the GSTK state
  machine.

PARAMETERS
  cmd_p: [Input] Pointer to the command to be processed by various the
                 GSTK state machine

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
extern gstk_status_enum_type gstk_process_state ( gstk_cmd_type * cmd_p );

/*===========================================================================
FUNCTION gstk_power_down

DESCRIPTION

  This function
  - flushes GSTK queue
  - vote UIM off
  - Ack TMC

PARAMETERS
  None

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
extern gstk_status_enum_type gstk_power_down ( void );


/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_BUF

DESCRIPTION
  This function puts the item at the end of the queue and sets the GSTK cmd
  queue signal

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern void gstk_task_put_cmd_buf (
  gstk_task_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_BUF_TO_HEAD

DESCRIPTION
  This function puts the item at the front of the queue and sets the GSTK cmd
  queue signal

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern void gstk_task_put_cmd_buf_to_head (
  gstk_task_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION GSTK_TASK_PUT_CMD_PRIORITY_BUF

DESCRIPTION
  This function puts the item at the end of the queue and sets the GSTK cmd
  queue signal

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern void gstk_task_put_cmd_priority_buf (
  gstk_task_cmd_type *cmd_ptr
);


/*===========================================================================
FUNCTION gstk_build_timer_expired_cmd

DESCRIPTION

  This function build the timer expiration command based on the passed in
  timer type and put onto the command queue

PARAMETERS
  gstk_timer_type - [INPUT] gstk_timer_enum_type, use to build the command

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
extern void gstk_build_timer_expired_cmd(
  unsigned long gstk_timer_type
);

/*===========================================================================
FUNCTION gstk_terminal_rsp_delay_cb

DESCRIPTION

  This function build the timer expiration command for Terminal Response Delay Timer

PARAMETERS
  gstk_instance - [Input] Used to identify the GSTK instance

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
void gstk_terminal_rsp_delay_cb(
  unsigned long  gstk_instance
);

/*===========================================================================
FUNCTION gstk_rex_timer_elapsed_expire_cb

DESCRIPTION
  This function will add the elapsed time value to the time field of the
  timer management table and restart the timer_elapsed

PARAMETERS
  param: [Input] Specifies which timer has expired

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_rex_timer_elapsed_expire_cb( unsigned long param);

/*===========================================================================
FUNCTION gstk_rex_timer_expire_cb

DESCRIPTION
  This function trigger the timer expiration event envelope command.
  This will only build an envelope command when the timer was started and
  expired

PARAMETERS
  param: [Input] Specifies which timer has expired

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_rex_timer_expire_cb ( unsigned long param);

/*===========================================================================
FUNCTION gstk_build_timer_expire_envelope

DESCRIPTION
  This function build an envelope command when the timer was started and
  expired

PARAMETERS
  slot:  [Input] The SIM slot where the envelope command to be sent
  param: [Input] Specifies which timer has expired

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_build_timer_expire_envelope(
  gstk_slot_id_enum_type slot,
  uint8                  param
);

/*===========================================================================
FUNCTION gstk_set_additional_info

DESCRIPTION
  Sets gstk_additional_info_type->additional_info to field.

PARAMETERS
  rec: The record to set the field in.
  out: The field used to set the record with.

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_set_additional_info
(
  gstk_additional_info_type*          rec,
  gstk_additional_info_code_enum_type field
);

/*===========================================================================
FUNCTION gstk_set_command_id

DESCRIPTION
  Sets the command id.

PARAMETER
  header: The header to set the command_id for.
  command_id: The value to set the command_id to.

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_set_command_id(gstk_hdr_type* header,
                                gstk_cmd_enum_type command_id);

/*===========================================================================
FUNCTION gstk_gsdi_continue_task

DESCRIPTION
  Sets the flag.

PARAMETER
  boolean val.

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_gsdi_continue_task(boolean);

/*===========================================================================
FUNCTION   gstk_check_for_pending_pro_cmd

DESCRIPTION
  This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

PARAMETERS
  env_cmd_index: [Input] Index into the envelope command reference table

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_check_for_pending_pro_cmd(void);

/*===========================================================================
FUNCTION   gstk_cm_ss_event_cb

DESCRIPTION
  Serving system (SS) event callback function, called when an SS event is happening.
  This function populates the various call related indication types and
  puts the command in the GSTK command queue.

PARAMETERS
  ss_event: [Input] CM serving system event
  ss_info_ptr: [Input] Pointer to the ss info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_cm_ss_event_cb (
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type*  ss_info_ptr
);

/*===========================================================================
FUNCTION gstk_ds_sys_evt_cb

DESCRIPTION
  Callback registered with DS which will be invoked when DS serving system
  changes

PARAMETERS
  Technology for which the event is being sent
  Event name
  DS Subscription ID
  Event information
  User data being returned to client

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void  gstk_ds_sys_evt_cb
(
  ds_sys_tech_enum_type                          tech_type,
  ds_sys_event_enum_type                         event_name,
  ds_sys_subscription_enum_type                  subscription_id,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/*===========================================================================
FUNCTION gstk_uim_polling_report

DESCRIPTION

  This is the callback function for polling response from UIM.
  UIM will call this function when it finishes it's polling operation

PARAMETERS
  report: [Input] Pointer to the UIM polling report

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_uim_polling_report (uim_rpt_type *report);

/*===========================================================================
FUNCTION gstk_get_rr_cell_id_info

DESCRIPTION
  Function used to query RR for the Cell Id.

PARAMETER
  Input:   const uint8       * mcc_mnc_p      : Current PLMN Information
  Input:   const uint8       *lac_p           : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Input:   gstk_slot_id_enum_type slot        : Slot id
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  RR has to be able to provide a valid Cell ID:

RETURN VALUE
  boolean:  TRUE:  Cell ID information is set.  It may be default data
            FALSE: Cell ID information is not check.  An error occurred.
COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_get_rr_cell_id_info(
  const uint8       * mcc_mnc_p,
  const uint8       * lac_p,
  gstk_cell_id_type * cell_id_info_p,
  gstk_slot_id_enum_type  slot
);

/*===========================================================================
FUNCTION gstk_get_rrc_cell_id_info

DESCRIPTION
  Function used to query RRC Table information populated by RRC.

PARAMETER
  Input:  mcc_mnc_p      : Current PLMN Information
  Input:  lac_p          : Current LAC  Information
  Input:  cell_id_info_p : Structure to populated
  Input:  slot           : Slot id
  Output: cell_id_info_p : Cell id information

DEPENDENCIES
  RRC would have populated the RRC Loci Table

RETURN VALUE
  boolean:  TRUE:  Cell ID information is set.  It may be default data
            FALSE: Cell ID information is not check.  An error occurred.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_rrc_cell_id_info (
  const uint8*                mcc_mnc_p,
  const uint8*                lac_p,
  gstk_cell_id_type*          cell_id_info_p,
  gstk_slot_id_enum_type      slot,
  gstk_access_technology_type access_tech
);

#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_cache_1x_location_info

DESCRIPTION
  Caches 1x location information

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gstk_cache_1x_location_info(
  gstk_cdma_loci_ext_cache_type  *cache,
  gstk_cdma_location_info_type   loc_info
);

/*===========================================================================
FUNCTION gstk_copy_1x_info_from_cm

DESCRIPTION
  Copies 1x related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_1x_info_from_cm(
  gstk_cdma_loci_ext_type*                cdma_ext_loc_info,
  const cm_mm_msim_ss_stack_info_s_type*  cm_stack_info_ptr
);
#endif /* #ifdef FEATURE_CDMA */

/*===========================================================================
FUNCTION gstk_copy_advance_info_from_cm

DESCRIPTION
  Copies 1X Advance related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_advance_info_from_cm(
  gstk_cdma_loci_ext_type              * cdma_ext_loc_info,
  const cm_mm_msim_ss_stack_info_s_type* cm_stack_info_ptr
);

/*===========================================================================
FUNCTION gstk_copy_hdr_info_from_cm

DESCRIPTION
  Copies HDR related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_hdr_info_from_cm(
  gstk_cdma_loci_ext_type              * hdr_loc_info,
  const cm_mm_msim_ss_stack_info_s_type* cm_stack_info_ptr
);

/*===========================================================================
FUNCTION gstk_cache_advance_location_info

DESCRIPTION
  Caches 1x advance location information

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gstk_cache_advance_location_info(
  gstk_cdma_loci_ext_cache_type *cache,
  gstk_cdma_loci_ext_type loc_info
);

/*===========================================================================
FUNCTION gstk_cache_hdr_location_info

DESCRIPTION
  Caches HDR location information

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void gstk_cache_hdr_location_info(
  gstk_cdma_loci_ext_cache_type *cache,
  gstk_cdma_loci_ext_type loc_info
);

/*===========================================================================
FUNCTION   gstk_cm_ph_error_event_cb

DESCRIPTION
  Callback function called as immediate response when GSTK sends a request
  for Network Search Mode to CM as part of the Proactive Command Provide Local
  Info.

PARAMETERS
  void*                 data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_cm_ph_error_event_cb (
  void*                 data_block_ptr,
  cm_ph_cmd_e_type      cmd,
  cm_ph_cmd_err_e_type  cmd_err
);

/*===========================================================================
FUNCTION   gstk_cm_ph_event_cb

DESCRIPTION
  Phone (PH) event callback function, called when a PH event is happening.
  This function populates the preferred network search mode and
  puts the command in the GSTK command queue.

PARAMETERS
  ph_event: [Input] CM phone event
  ph_info_ptr: [Input] Pointer to the ph info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_cm_ph_event_cb (
  cm_ph_event_e_type          ph_event,
  const cm_ph_info_s_type    *ph_info_ptr
);

/*===========================================================================
FUNCTION: gstk_wms_event_cb

DESCRIPTION:
  This function is the event callback function for WMS.

PARAMETERS:
  wms_event:  [Input] Command whose status is being reported
  info_ptr:   [Input] WMS info buffer
  shared_ptr: [Input] WMS indication of whether the info can be passed to
                      other clients.

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
extern void gstk_wms_event_cb(
  wms_msg_event_e_type              wms_event,
  const wms_msg_event_info_s_type  *info_ptr,
  const boolean                    *shared_ptr);

/*===========================================================================

FUNCTION GSTK_FREE

DESCRIPTION
  The gstk_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void gstk_free(void * ptr);

/*===========================================================================

FUNCTION gstk_offset_memcpy

DESCRIPTION
  gstk_offset_memcpy first checks if there is sufficient memory malloced
  before calling memcpy

PARAMETERS
  dest_ptr:   Pointer to copy data to.
  src_ptr: Pointer to copy data from.
  dest_offset:        offset from where to start copying data to
  copy_size:     size of data to copy
  dest_max_size:      size of destination buffer
  src_max_size: size of source buffer, if 0 then no check on source size done

DEPENDENCIES
  None.

RETURN VALUE
  the number of bytes copied

SIDE EFFECTS
  data is copied to the destination buffer
===========================================================================*/
size_t  gstk_byte_offset_memcpy(
    byte *dest_ptr,
    const void *src_ptr,
    uint32 dest_offset,
    size_t copy_size,
    size_t dest_max_size,
    size_t src_max_size);

/*===========================================================================
FUNCTION gstk_handle_mmgsdi_refresh_cnf

DESCRIPTION
  This is the function that is called from gstk callback from MMGSDI to
  process refresh cnf.

PARAMETERS
  mmgsdi_return_enum_type  mmgsdi_status     - MMGSDI status
  mmgsdi_refresh_cnf_type *refresh_cnf_ptr   - Refresh conf

DEPENDENCIES
  None

RETURN VALUE
  None
COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_handle_mmgsdi_refresh_cnf (
  mmgsdi_return_enum_type        mmgsdi_status,
  const mmgsdi_refresh_cnf_type *refresh_cnf_ptr
);

/*===========================================================================
FUNCTION GSTK_MMGSDI_CMD_CB

DESCRIPTION
  GSTK Callback for Client Id and Refresh from MMGSDI

PARAMETER
  status  [Input]: status of command returned from MMGSDI
  cnf     [Input]: confirmation type
  cnf_ptr [Input]: confirmation pointer

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_mmgsdi_cmd_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);

/*===========================================================================

FUNCTION gstk_get_and_store_curr_card_protocol

DESCRIPTION
  This function stores the current card protocol as obtained from MMGSDI

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gstk_get_and_store_curr_card_protocol( void);

/*===========================================================================

FUNCTION gstk_process_client_reg_req

DESCRIPTION
  This function processes the client registration request to GSTK

PARAMETERS
  cmd_ptr[Input] - Pointer to command

DEPENDENCIES
  None.

RETURN VALUE
  gstk_status_enum_type
  GSTK_SUCCESS
    if client registration was processed succesfully.
    Note: This only means that the client will receive a response from GSTK
    for the registration. It however does not mean that the registration
    was successful.
  GSTK_NULL_INPUT_PARAM
    This denotes that the parameter provided to this function was  NULL and
    hence the request could not be processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_client_reg_req(const gstk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION gstk_process_client_pro_cmd_reg_req

DESCRIPTION
  This function processes the client registration request for processing a
  practive command.

PARAMETERS
  cmd_ptr[Input] - Pointer to command

DEPENDENCIES
  None.

RETURN VALUE
  gstk_status_enum_type
  GSTK_SUCCESS
    if a response was sent to the client with a client id in case of SUCCESS
    OR
    a failure reason was returned to client
  GSTK_NULL_INPUT_PARAM
    This denotes that the parameter provided to this function was NULL and
    hence the request could not be processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_client_pro_cmd_reg_req(
  const gstk_client_evt_reg_type *cmd_ptr
);

/*===========================================================================

FUNCTION gstk_process_client_init_req

DESCRIPTION
  This function processes the client init request to GSTK. If the processing
  is successful, it provides the client with a client id.

PARAMETERS
  cmd_ptr[Input] - Pointer to command

DEPENDENCIES
  None.

RETURN VALUE
  gstk_status_enum_type
  GSTK_SUCCESS
    if a response was sent to the client with a client id in case of SUCCESS
    OR
    a failure reason was returned to client
  GSTK_NULL_INPUT_PARAM
    This denotes that the parameter privided to this function was  NULL and
    hence the request could not be processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_client_init_req(
  const gstk_client_id_reg_type *cmd_ptr);

/*===========================================================================

FUNCTION gstk_process_client_pro_cmd_dereg_req

DESCRIPTION
  This function processes the client deregistration request for a
  proactive command.

PARAMETERS
  cmd_ptr[Input] - Pointer to command

DEPENDENCIES
  None.

RETURN VALUE
  gstk_status_enum_type
  GSTK_SUCCESS
    if a response was sent to the client with a client id in case of SUCCESS
    OR
    a failure reason was returned to client
  GSTK_NULL_INPUT_PARAM
    This denotes that the parameter provided to this function was  NULL and
    hence the request could not be processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_client_pro_cmd_dereg_req(
  const gstk_client_evt_reg_type *cmd_ptr);

/*===========================================================================

FUNCTION gstk_delete_client_from_class_evt_reg_table

DESCRIPTION
  This function deletes a client registration for all commands from the
  class event registration "gstk_class_evt_reg_table" table.

PARAMETERS
  client_id[Input] - client identity

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gstk_delete_client_from_class_evt_reg_table(
  gstk_client_id_type client_id);

/*===========================================================================

FUNCTION gstk_process_class_evt_registration

DESCRIPTION
  This function processes the event registration for proactive commands
  received from gstk clients. It also performs the necessary checks for
  duplicate and invalid registrations.

PARAMETERS
  client_id            [Input] - client identity
  class_bitmask        [Input] - Letter classes for ME hardware dependant
                                 commands
  evt_reg_bitmask      [Input] - event bitmask
  format_type          [Input] - format type
  func_type            [Input] - functionality type
  need_classification  [Input] - whether classfication of events is
                                 required.

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client is not registered
    GSTK_BAD_PARAM             - bad input parameters
    GSTK_ERROR                 - if no class evt registration was valid
    GSTK_SUCCESS               - if client registrations were successfully
                                 processed.

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_process_class_evt_registration(
  gstk_client_id_type                       client_id,
  uint32                                    class_bitmask,
  uint64                                    evt_reg_bitmask,
  gstk_client_pro_cmd_data_format_enum_type format_type,
  gstk_tk_functionality_enum_type           func_type,
  boolean                                   need_classification
);

/*===========================================================================

FUNCTION gstk_add_client_to_class_evt_reg_table

DESCRIPTION
  This function registers a client to receive notification for a
  specific proactive command.

PARAMETERS
  client_id            [Input] - client identity
  class_bitmask        [Input] - Letter classes for ME hardware dependant
                                 commands
  evt_reg_bitmask      [Input] - event bitmask
  format_type          [Input] - format type
  func_type            [Input] - functionality type

DEPENDENCIES
  None

COMMENTS
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client is not registered
    GSTK_BAD_PARAM             - bad input parameters
    GSTK_ERROR                 - wrong index calculated
    GSTK_SUCCESS               - if client registrations were
                                 successfully added to the table

SIDE EFFECTS
  None
===========================================================================*/
gstk_status_enum_type gstk_add_client_to_class_evt_reg_table(
  gstk_client_id_type                       client_id,
  uint32                                    class_bitmask,
  uint64                                    evt_reg_bitmask,
  gstk_client_pro_cmd_data_format_enum_type format_type,
  gstk_tk_functionality_enum_type           func_type
);

/*===========================================================================
FUNCTION   gstk_fill_error_response

DESCRIPTION
  This function fills uim cmd ptr with error response when the length of
  terminal response exceeds the max chars limit for uim.

PARAMETERS
  uim_cmd_ptr          :  Pointer to UIM command
  STK_response_header  :  Terminal Response header type
  byte_offset          :  offset
DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_fill_error_response(
  uim_cmd_type                        *uim_cmd_ptr,
  gstk_terminal_response_header_type  STK_response_header,
  int                                 byte_offset
);

/*===========================================================================
FUNCTION  gstk_is_env_cmd_to_be_skipped

DESCRIPTION
  This function checks if the first command in gstk command queue is to
  be skipped for processing or not.

PARAMETERS
  None

DEPENDENCIES
  gstk command queue should be created in advance

RETURN VALUE
  boolean

SIDE EFFECTS
  This will determine if fetch can be handeled by UIM or not. Also it will
  be used to check if BIP envelope can be processed by gstk or not.

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_is_env_cmd_to_be_skipped (void);

/*===========================================================================
FUNCTION   gstk_switch_to_instance

DESCRIPTION
  This function switches current context data (private data) to a specific
  instance assiciated with specified SIM slot id

PARAMETERS
  sim_slot_id

DEPENDENCIES
  gstk command queue should be created in advance

RETURN VALUE
  boolean

SIDE EFFECTS
  This will determine if fetch can be handeled by UIM or not. Also it will
  be used to check if BIP envelope can be processed by gstk or not.

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_switch_to_instance(gstk_slot_id_enum_type sim_slot_id);

/*===========================================================================

FUNCTION gstk_memcpy

DESCRIPTION
  gstk_memcpy first checks if the size requested does not exceed the source
  size and that the size of the data to be copied does not exceed the
  max data size the destination buffer can accomodate before preceeding with
  the memcpy

PARAMETERS
  dest_ptr:   Pointer to copy data to.
  src_ptr: Pointer to copy data from.
  copy_size:     size of data to copy
  dest_max_size:      size of destination buffer
  src_max_size: size of source buffer, if 0 then no check on source size done

DEPENDENCIES
  None.

RETURN VALUE
  the number of bytes copied

SIDE EFFECTS
  data is copied to the destination buffer.  the copy may be truncated if the
  size of data to be copied exceeds the size of the source buffer or if the
  size of data to be copied exceeds the size of the destination buffer.
===========================================================================*/
size_t  gstk_memcpy(
  void *dest_ptr,
  const void *src_ptr,
  size_t copy_size,
  size_t dest_max_size,
  size_t src_max_size);

/*=============================================================================
FUNCTION: gstk_cm_cmd_cb

DESCRIPTION:
  Command callback from CM to report the status of queuing a request to CM

PARAMETERS:
  *data_block_ptr    : [Input]  estk data block pointer
  call_cmd           : [Input]  cm command type
  call_cmd_err       : [Input]  cm command error type
  call_cmd_err_cause : [Input]  cm command error cause
  alpha              : [Input]  cm modified alpha

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void gstk_cm_cmd_cb (
  void                         *data_block_ptr,
  cm_call_cmd_e_type            call_cmd,
  cm_call_cmd_err_e_type        call_cmd_err,
  cm_call_cmd_err_cause_e_type  call_cmd_err_cause,
  cm_alpha_s_type               alpha
);

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_LOC_INFO_COMMAND

DESCRIPTION

  This function sends the location information envelope command to the USIM
  This function does not require client ID because it is intended to be called
  by internal modules.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_client_init_ext()
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
  This function should only be called after gstk_client_init_ext has been called

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_general_envelope_rsp_enum_type in its callback function.

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_send_envelope_loc_info_command (
        gstk_client_id_type         client_id,
        const uint8                 * loc_info,
        uint32                      user_data);

#ifdef FEATURE_LTE
void gstk_process_rrc_msgr_cmd(void);
#endif /* FEATURE_LTE */

/*===========================================================================
FUNCTION gstk_process_mmgsdi_evt

DESCRIPTION
  Process MMGSDI events

PARAMETERS
  cmd_ptr: [Input] MMGSDI event ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_mmgsdi_evt(
  gstk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION gstk_process_ds_evt_cmd

DESCRIPTION
  Processes DS events

PARAMETERS
  cmd_ptr: [Input] DS command ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_ds_evt_cmd(
  gstk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION gstk_process_otasp_reg_cmd

DESCRIPTION
  Processes otasp reg cmd

PARAMETERS
  cmd_ptr: [Input] command ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_otasp_reg_cmd(
  gstk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION gstk_process_cm_subs_change_ind

DESCRIPTION
  Processes the Subscription change indication from Call Mamager

PARAMETERS
  cmd_ptr: [Input] CM indication ptr

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_cm_subs_change_ind(
  gstk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION gstk_process_mode_change_update_3gpp2_bmsk

DESCRIPTION
  This function processes the CM mode change event and:
  - Updates the variable 3gpp2_bmsk in current instance pointer
  - If 1x RAT becomes available when GSTK is waiting for it to trigger OTASP
    call for card acivation, an OTASP command is queued

PARAMETER
  mode_change_ind : CM mode change indication command

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_mode_change_update_3gpp2_bmsk(
  gstk_mode_change_ind_type* mode_change_ind
);

/*===========================================================================
FUNCTION gstk_get_curr_cell_id_info_int

DESCRIPTION
  Function called to retrieve the Cell ID information based on the RAT
  and current PLMN Information. This is internal funciton.

PARAMETER
  Input:   sys_radio_access_type_e_type radio_access_tech : Current RAT
  Input:   uint8  * mcc_mnc_p                 : Current PLMN Information
  Input:   uint8  * lac_p                     : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Input:   gstk_slot_id_enum_type slot        : Slot id
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  TODO

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_curr_cell_id_info_int(
  gstk_sys_radio_access_tech_e_type radio_access_tech,
  const uint8       *               mcc_mnc,
  const uint8       *               lac,
  gstk_cell_id_type *               cell_id_info_p,
  gstk_slot_id_enum_type            slot
);

/*===========================================================================
FUNCTION gstk_attach_ctrl_timer_exp_cb

DESCRIPTION

  This function build the timer expiration command if the attach query timer
  expires

PARAMETERS
  slot - [Input] slot of which GSTK instance's timer expires

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_attach_ctrl_timer_exp_cb(
  unsigned long  slot
);

/*===========================================================================
FUNCTION   gstk_cleanup_cmd_queue_slot_id

DESCRIPTION
  This function will removes the commands from the gstk command queue with
  specified SIM slot id.

PARAMETERS
  sim_slot_id   : SIM slot id

DEPENDENCIES
  gstk command queue should be created in advance

RETURN VALUE
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_cleanup_cmd_queue_slot_id(gstk_slot_id_enum_type sim_slot_id);

/*===========================================================================
FUNCTION   gstk_queue_rrc_utran_nmr_rsp_ext

DESCRIPTION
  Library function called by RRC to post an NMR response to GSTK for network
  measurement results for utran.

PARAMETERS
  status:       result status from RRC
  length:       Length of data returned
  nmr_info_ptr: UTRAN NMR info returned from RRC
  data_ptr:     GSTK reference data returned back by RRC
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
  gstk_utran_status_enum_type status,
  uint32                      length,
  const uint8               * nmr_info_ptr,
  const void                * data_ptr,
  gstk_nmr_utran_qualifier_enum_type  nmr_utran_qualifier);

/*===========================================================================
FUNCTION gstk_update_active_data_slot

DESCRIPTION
  Update slot details which is having an active data connection

PARAMETERS
  as_id

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_update_active_data_slot(sys_modem_as_id_e_type as_id);

/*===========================================================================
FUNCTION gstk_cm_subs_evt_cb

DESCRIPTION
  CMPH Subscription Event callback. Currently handled only
  CM_PH_EVENT_SUBSCRIPTION_PREF_INFO.

PARAMETERS
  subs_evt                  Subscription Event Enum
  subs_info_ptr             Subscription Event Data

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_subs_evt_cb(
  cm_ph_event_e_type        subs_evt,
  const cm_subs_info_s_type *subs_info_ptr
);

/*===========================================================================
FUNCTION gstk_process_session_changed_evt

DESCRIPTION
  Processes GSTK_MMGSDI_SESSION_CHANGED_EVT

PARAMETERS
  cmd_ptr of the MMGSDI event

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_process_session_changed_evt(gstk_cmd_type *cmd_ptr);
#endif /* GSTK_H */
