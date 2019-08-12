/*==========================================================================

                         Q M I _ C A T . C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Card Application Toolkit Service source
 file.

                        COPYRIGHT INFORMATION

Copyright (c) 2005 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_cat.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/22/16    vdc    Remove F3 messages for memory allocation failure
05/16/16    vdc    Add indication support for TR and Envelope cmd
12/09/15    vdc    Fix card state update when process TP DL indication
11/25/15    sp     Updated copyright
11/23/15    vdc    Multi-profile switch handling
10/14/15    vr     Fix for STK not displayed with security activated
09/07/15    bcho   Migrate to mcfg EFS wrappers
08/27/15    vdc    Do not retry service registration if it fails once
04/14/15    vr     Fix custom TP update failure
02/18/15    vr     qmi_cati_read_configuration in pre_task_startup
02/13/15    hh     Split qmi_cat_init into pre and post startup
01/09/15    hh     Set correct message type for SCWS_OPEN_CH_REQ
11/14/14    hh     Fix compilation warnings
10/30/14    gm     Move enable/disable of raw unsupported features to NV
10/30/14    hh     Support for GSTK_CFG_FEATURE_ME_PLI_DATE_TIME_APPS removal 
10/22/14    am     Fix compile warning 
10/08/14    tkl    Fix KW error
10/06/14    hh     Set full_func_evt[] for new UICC proactive command only 
09/30/14    gm     Remove GOBI mode code
09/29/14    av     Featurize all QMI messages
09/16/14    hh     Correct message type for raw proactive command
09/15/14    tkl    Clear BIP status when End session with error during BIP session
09/11/14    hh     Call rex_undef_timer() when timer is no longer needed 
09/09/14    bd     Moved an enum to header file and clean up
08/29/14    hh     Fix compliation warnings
08/26/14    tkl    Fixed handling of PLI Lang cmd in check_last_gstk_cmd
08/26/14    hh     Fix array boundary checking
08/25/14    hh     Correctly handle Setup Evt List proactive command
08/22/14    hh     Moved qmi_cati_custom_profile_value_list to heap
08/05/14    hh     Added support to retrieve supported proactive commands
07/02/14    shr    Add Support for NV Refresh
07/02/14    hh     Removed check of cmd_ref_id against CATI_GSTK_PROACTIVE_COMMANDS_MAX
06/18/14    tkl    Update to use BIP channel ID in CLOSE CH CNF instead of REQ
06/17/14    hh     Clear and/or Overwrite cached proactive command with new one
06/02/14    vr     Off target support for qmi cat NV
05/15/14    tkl    Add duration TP bitmask for Decoded format
03/27/14    gs     Removed the GSTK featurization
04/29/14    dy     Added ext browser HTML support bit to TP
04/16/16    vr     Discard invalid TR or event confirmation
04/09/14    yt     Make RAT TLV mandatory for Call Control envelope
04/07/14    gm     Added support for call type as IMS
04/01/14    av     Rename QMI_CAT_FREE_IF_NULL to QMI_CAT_FREE_IF_NOT_NULL
03/27/14    at     Usage of common QMI APIs from DS header
03/19/14    tkl    Added refresh alpha TP for android profile
03/17/14    gm     Added support for IMS communication control
03/12/14    gm     Moved qmi_cat_state and qmi_cati_global to heap
03/06/14    tkl    Update address/sub-address as optional TLVs for MT call env
03/06/14    gm     Moved to new API for client alloc/dealloc result
02/24/14    av     Do not rely on CATI_CMD_MAX when traversing through cmd cb
02/11/14    tkl    Free callback user data after sending response
01/28/14    df     Use q_cnt() instead directly accessing cnt
01/10/14    kb     Add de-init function when gstk task is stopped
01/10/14    df     Use v2.0 diag macros
09/27/13    sw     Corrected the BIP channel Ids
12/02/13    tkl    Use global slot count for looping instead of max card count
10/21/13    gm     Extended ref_command_id for multi card support
10/09/12    tkl    Fixed handling of slot 3 in parsing slot tlv
10/08/13    gm     Removed unused comments
10/04/13    gm     F3 message reduction
09/10/13    tkl    Removed duplicate free of cc_sms_ind_ptr in decoded env
09/04/13    tkl    Moved to new MO SMS envelope API with proper LS filler.
07/27/13    tkl    Fixed merge error
07/26/13    tkl    Add NV support for enabling use of SMS-PP envelope command
07/22/13    gs     Added mapping of GSTK_UNSUPPORTED_COMMAND to QMI error code
07/02/13    yt     Dynamically determine number of slots from UIMDRV
06/12/13    kb     Added QMI malloc and free wrapper functions
05/20/13    tkl    Fix to ensure Close Ch REQ is processed before BIP CNF checks
05/17/13    tkl    Added support for Refresh Alpha
05/15/13    gm     Moved memcpy() and memmove() to memscpy() and memsmove()
05/06/13    tkl    Set location info data for CC envelope with filler
05/03/13    tkl    Return appropriate error code when TR timer expires
04/30/13    tkl    Added support for 3rd party IMS client in raw format
04/23/13    tkl    Fixed sending of BIP status with correct event report bitmask
04/11/13    tkl    Add NV suport for SETUP Call display alpha value
04/02/13    av     Move UIM tasks to group4 of RCINIT
03/18/13    at     Fix for NULL pointer dereference of cmd_buf_p
02/28/13    tkl    Added support for 3rd party IMS client
02/22/13    yt     Support for slot 3
02/19/13    av     Merge mmgsi,gstk,qmiuim/cat conf files to mmgsdi.conf
02/14/13    tl     Move QMI CAT to GSTK context from QMI modem context
01/28/13    tkl    Added BIP status events
11/15/12    xz     Move SCWS TP bit setting into ESTK
11/09/12    yt     Move QMI CAT to UIM heap
10/21/12    av     Update conf file only if necessary
10/03/12    tkl    Added support for Browswer Termination event indication
10/03/12    tkl    Added raw hdr tag to handle GSTK_TAG_SETUP_CALL_DISPLAY_ALPHA
10/02/12    tkl    Added SMS-PP download in envelope command
09/27/12    tkl    Added Browser Status evt bitmask for TP
09/20/12    at     Fix for sending TR when language code NV is set
09/18/12    tkl    Added optional Alpha TLV for SCWS open channel ind
09/09/12    at     Removal of private DS functions and headers
09/07/12    tkl    Fix compliation error when BIP server mode is enabled
09/07/12    tl     Added support for SGLTE dual slots
07/20/12    tkl    Added display alpha support for Custom TP raw format
07/16/12    at     Read NV to send language code in default TR
07/16/12    at     Fix to clean the client queues before cl_id is deallocated
05/23/12    hn     Enable 1-byte alignment packing on Windows environment
05/10/12    nmb    Correct dangling pointer when deallocating clients
03/26/12    at     Extra checks when parsing custom tp values from NV
03/21/12    at     Added browser termination event in decoded envelope command
02/23/12    tkl    MPSS/QMI support interfaces logging
02/23/12    tkl    Added envelope cmd for Call Control & HCI Conn
02/23/12    nmb    QMI QMUX Transition
02/02/12    at     Updation of custom_tp NV with max bytes
02/02/12    nb     Featurized support for "User Activity" and "Language Settings"
                   in android TP
01/12/12    at     Fixed compilation errors
01/10/12    tkl    Added support for set/get configuration
01/10/12    tkl    Added custom TP support
01/05/12    vv     Removed unneeded ASSERT statements
01/03/12    nb     Addded support to send status words for TR
01/03/12    nb     Addded support to retrieve terminal profile
12/22/11    tkl    Fixed crash in SCWS data available with fuzz string
12/21/11    nb     GSDI Cleanup
12/20/11    hn     Add support for off-target testing
12/13/11    mib    Fixed compilation error
12/06/11    tkl    Removed TP bits & evt registration for network related cmd
12/05/11    vv     Send error response for OPEN CHANNEL when cache is busy
11/17/11    tkl    Caching PLI language and PLI time cmd
11/16/11    tkl    Added APDU support for raw envelope
11/08/11    tkl    Added support for Class M and L
11/03/11    tkl    Sent event conf when indication is not sent & not cached
10/25/11    tkl    Added Get InKey Yes/No TLV for decoded TR
10/24/11    tkl    Add duration TP bitmask for Android format
10/20/11    tkl    Fixed KW error
10/11/11    tkl    Disabled Language Notification in TP bitmask for Android
09/26/11    nmb    Maintain command buffers internally
07/11/11    tkl    Disabled User Activity in Terminal Profile for Android
07/07/11    tkl    Fixed uim_ref_id in the QMI CAT cache
06/07/11    yt     Added NV item for CAT VS service
06/06/11    tkl    Added 7 bit DCS encoding for text string in decoded TR
06/05/11    vs     Decoded call control envelope
06/03/11    yt     Added slot id to QMI_CAT_SET_EVENT_REPORT
05/31/11    tkl    Updated read configuration for QCN restore
05/24/11    tkl    Added support for CATI_CONFIG_MODE_DISABLED mode
05/12/11    tkl    Updated SCWS Data Avail reqest as asynchronous
05/10/11    tkl    Fixed terminal_response_cnf for GSTK_PROVIDE_LANG_INFO_REQ
04/20/11    tkl    Added SCWS support
04/14/11    tkl    Updated icon conf value for Setup call display request
03/22/11    tkl    Fixed covert raw mask to TLV id for Send USSD
03/16/11    tkl    Added caching for Display Text cmd
03/12/11    tkl    Fixed compliation errors
03/08/11    tkl    Added support for PLI-Time proactive cmd
02/20/11    tkl    Added BIP support in decoded format
02/07/11    tkl    Fixed qmux register service for vendor specific ID
01/24/11    tkl    Added support for slot id
01/14/11    mib    Review of parsing routines
01/19/10    tkl    Mainlined service anywhere feature
12/21/10    tkl    Added support for End proactive session in PULL only mode
11/30/10    yt     Including private GSTK header
11/05/10    tkl    Fixed incorrect data type in proactive_session_end_type
11/03/10    tkl    Fixed raw_data_len to support size of CATI_TLV_PC_LEN_MAX
10/06/10    bd     Fixed wrong handling of NW commands when no client exists
10/06/10    tkl    Fixed incorrect error code returned from qmi event confirmation
10/01/10    tkl    Fixed soft key max number not set in terminal profile
09/24/10    mib    Fixed compiler warnings
09/24/10    tkl    Added support for BIP related proactive command.
09/21/10    mib    Added .conf file for QMI CAT NV items
09/20/10    tkl    Added support for setup evt list browser termination in
                   raw format
09/10/10    tkl    Fixed send TR when Ind is not sent and event is not cached.
09/09/10    tkl    Fixed len compare in qmi_cati_get_event_report
26/08/10    bd     Fixed wrong terminal response when client is not registered
08/27/10    dd     Fix Fusion TP
08/26/10    dd     Fixed default TP
08/20/10    bd     Fixed CATI_TLV_PC_LEN_MAX value
08/05/10    tkl    QMI UIM & QMI CAT split from data package
08/02/10    js     Modified CLASS0 cmd mask not to include
                   GSTK_REG_SETUP_EVENT_LIST_EVT in GSTK registration
07/28/10    tkl    Fixed deep copy of icon_list for Setup Menu & Select item
07/26/10    tkl    Allowed use of client id only in Gobi mode.
07/21/10    tkl    Added support to use tag for type of setup call cmd.
07/15/10    tkl    Added support for Setup evt list cmd in raw format
07/14/10    tkl    Added support for playtone & network cmd in raw format
                   & fix typo for config_mode.
07/08/10    tkl    Added support for Setup call user alpha & icon conf
07/08/10    mib    Fixed size of config file
06/25/10    mib    Added support for unknown envelope command
06/22/10    tkl    Added support for LANGUAGE INFO tlv for PLI-Language
06/16/10    tkl    Added support for user confirmation
06/14/10    mib    Fixed duration for GET INKEY in decoded TR
03/25/10    dd     Adding decoded tlvs
11/01/09    mj     Initial check-in of QMI_CAT
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stdio.h>
#include <stringl/stringl.h>

#include "nv.h"
#include "uim_msg.h"
#include "err.h"
#include "amssassert.h"
#include "fs_lib.h"
#include "fs_public.h"
#include "ps_in.h"

#include "qmi_framework.h"
#include "qmi_svc_utils.h"

#include "gstk_exp.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"

/* needed for int conversion functions */
#include "intconv.h"

#include "qmi_cat.h"
#include "qmi_cat_decoded.h"
#include "qmi_uim_cat_common.h"
#include "ds_qmi_fw_common.h"

#include "modem_mem.h"

/* Temporarily include gstk_p.h for Setup Call user & icon confirmation */
#include "gstk_nv_p.h"
#include "gstk_exp_v.h"

#include "qmi_si.h"
#include "qmi_idl_lib.h"
#include "card_application_toolkit_v02.h"
#include "card_application_toolkit_impl_v02.h"

#include "uim_p.h"
#include "uim_common_efs.h"

/* These includes need to always be at bottom - offtarget use only */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Service management
---------------------------------------------------------------------------*/
#define CATI_MAX_CLIDS                (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)
#define CATI_INVALID_INSTANCE         (-1)
#define CATI_RETRY_INTERVAL           (75)

/*---------------------------------------------------------------------------
  Major and Minor Version Nos for CAT
---------------------------------------------------------------------------*/
#define CATI_BASE_VER_MAJOR          (2)
#define CATI_BASE_VER_MINOR          (24)
#define CATI_ADDENDUM_VER_MAJOR      (0)
#define CATI_ADDENDUM_VER_MINOR      (0)

/*---------------------------------------------------------------------------
  Number of cards
---------------------------------------------------------------------------*/
#define QMI_CAT_MAX_CARD_COUNT                    (3)

/*---------------------------------------------------------------------------
  Max number of SCWS Channel
---------------------------------------------------------------------------*/
#define QMI_CATI_MAX_SCWS_CHANNEL                 (7)

/*---------------------------------------------------------------------------
  Max value of the number of TP DL type sent to GSTK
---------------------------------------------------------------------------*/
#define QMI_CAT_MODE_CUSTOM_TP_DL_MAX_VALUE      (GSTK_MAX_TP_DATA_SIZE / 4)

/*---------------------------------------------------------------------------
  Configuration file
---------------------------------------------------------------------------*/
#define CATI_CONFIG_DIR1             "/nv/item_files/modem"
#define CATI_CONFIG_DIR2             "/nv/item_files/modem/qmi"
#define CATI_CONFIG_DIR3             "/nv/item_files/modem/qmi/cat"

/*---------------------------------------------------------------------------
  Event report TLV definitions
---------------------------------------------------------------------------*/
#define CATI_PRM_TYPE_DISPLAY_TEXT                  (0x10)
#define CATI_PRM_TYPE_GET_INKEY                     (0x11)
#define CATI_PRM_TYPE_GET_INPUT                     (0x12)
#define CATI_PRM_TYPE_SETUP_MENU                    (0x13)
#define CATI_PRM_TYPE_SELECT_ITEM                   (0x14)
#define CATI_PRM_TYPE_SETUP_IDLE_MODE_TEXT          (0x17)
#define CATI_PRM_TYPE_LANG_NOTIFICATION             (0x18)
#define CATI_PRM_TYPE_PLAY_TONE_EVENT               (0x47)
#define CATI_PRM_TYPE_SETUP_CALL_EVENT              (0x48)
#define CATI_PRM_TYPE_SEND_DTMF_EVENT               (0x49)
#define CATI_PRM_TYPE_LAUNCH_BROWSER_EVENT          (0x4A)
#define CATI_PRM_TYPE_SEND_SMS_EVENT                (0x4B)
#define CATI_PRM_TYPE_SEND_SS_EVENT                 (0x4C)
#define CATI_PRM_TYPE_SEND_USSD_EVENT               (0x4D)
#define CATI_PRM_TYPE_PROVIDE_LOCAL_INFO            (0x4E)
#define CATI_PRM_TYPE_SETUP_EVENT_LIST_RAW          (0x4F)
#define CATI_PRM_TYPE_SLOT                          (0x50)
#define CATI_PRM_TYPE_OPEN_CHANNEL                  (0x51)
#define CATI_PRM_TYPE_CLOSE_CHANNEL                 (0x52)
#define CATI_PRM_TYPE_SEND_DATA                     (0x53)
#define CATI_PRM_TYPE_RECEIVE_DATA                  (0x54)
#define CATI_PRM_TYPE_ACTIVATE                      (0x66)
#define CATI_PRM_TYPE_REFRESH_ALPHA                 (0x6A)

/*---------------------------------------------------------------------------
  Tags used for cached proactive commands in raw format
---------------------------------------------------------------------------*/
#define CATI_PRM_TYPE_CACHED_SETUP_MENU             (0x10)
#define CATI_PRM_TYPE_CACHED_SETUP_EVT_LIST         (0x11)
#define CATI_PRM_TYPE_CACHED_SETUP_IDLE_MODE_TEXT   (0x12)

/*---------------------------------------------------------------------------
  CAT Proactive Cmd length
---------------------------------------------------------------------------*/
#define CATI_CMD_DETAILS_LEN    (5)

/*---------------------------------------------------------------------------
  Envelope content tag
---------------------------------------------------------------------------*/
#define CATI_SMS_PP_DOWNLOAD_TAG     (0xD1)

/*---------------------------------------------------------------------------
  Macro used in command handlers (common)
---------------------------------------------------------------------------*/
#define CHECK_RETVAL()  if (FALSE == retval) { dsm_free_packet(&response); \
                                               return NULL; }

/*---------------------------------------------------------------------------
  Macro used to free pointers
---------------------------------------------------------------------------*/
#define QMI_CAT_FREE_IF_NOT_NULL(ptr)                       \
      if (ptr)                                              \
      {                                                     \
        uimqmi_free(ptr);                                   \
        ptr = NULL;                                         \
      }

/*---------------------------------------------------------------------------
  Values used for cache (raw)
---------------------------------------------------------------------------*/
#define CATI_CACHED_EVT_SETUP_MENU                          (0)
#define CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT                (1)
#define CATI_CACHED_EVT_SETUP_EVT_LIST                      (2)
#define CATI_CACHED_EVT_DISPLAY_TEXT                        (3)
#define CATI_CACHED_EVT_PROVIDE_LOCAL_INFO                  (4)
#define CATI_MAX_RAW_CACHED_EVTS                            (5)

/*---------------------------------------------------------------------------
  Values used for cache (decoded)
---------------------------------------------------------------------------*/
#define CATI_CACHED_EVT_DECODED_SETUP_MENU                   (0)
#define CATI_CACHED_EVT_DECODED_SETUP_IDLE_MODE_TEXT         (1)
#define CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_IDLE_SCREEN   (2)
#define CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_USER_ACT      (3)
#define CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_LANG_SEL      (4)
#define CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_HCI_CONN      (5)
#define CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_BROWSER_TERM  (6)
#define CATI_CACHED_EVT_DECODED_DISPLAY_TEXT                 (7)
#define CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL            (8)
#define CATI_CACHED_EVT_DECODED_PROVIDE_LOCAL_INFO_LANG      (9)
#define CATI_MAX_DECODED_CACHED_EVTS                        (10)

/*---------------------------------------------------------------------------
  Values used for recovery cache (raw)
---------------------------------------------------------------------------*/
#define CATI_QMI_CLIENT_CACHED_EVT_SETUP_MENU                (0)
#define CATI_QMI_CLIENT_CACHED_EVT_SETUP_IDLE_MODE_TEXT      (1)
#define CATI_QMI_CLIENT_CACHED_EVT_SETUP_EVT_LIST            (2)
#define CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS                  (3)

/*---------------------------------------------------------------------------
  Values used for DCS conversions
---------------------------------------------------------------------------*/
#define QMI_CATI_SMS_7_BIT_PACKED_GENERAL_DATA_CODING_DCS (0x00)
#define QMI_CATI_SMS_8_BIT_GENERAL_DATA_CODING_DCS        (0x04)
#define QMI_CATI_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS   (0x08)

/*---------------------------------------------------------------------------
  Values used for Access Technology
---------------------------------------------------------------------------*/
#define QMI_CATI_ACCESS_NONE            (0x00)
#define QMI_CATI_ACCESS_TECH_GSM        (0x01)
#define QMI_CATI_ACCESS_TECH_UTRAN      (0x02)
#define QMI_CATI_ACCESS_TECH_CDMA       (0x03)
#define QMI_CATI_ACCESS_TECH_LTE        (0x04)
#define QMI_CATI_ACCESS_TECH_WLAN       (0x05)

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Set Event Report Raw event bitmask
---------------------------------------------------------------------------*/
#define CATI_REPORT_STATUS_NONE                                (0x0000000)
#define CATI_REPORT_STATUS_EV_DISPLAY_TEXT                     (0x0000001)
#define CATI_REPORT_STATUS_EV_GET_INKEY                        (0x0000002)
#define CATI_REPORT_STATUS_EV_GET_INPUT                        (0x0000004)
#define CATI_REPORT_STATUS_EV_SETUP_MENU                       (0x0000008)
#define CATI_REPORT_STATUS_EV_SELECT_ITEM                      (0x0000010)
#define CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID                 (0x0000020)
#define CATI_REPORT_STATUS_EV_SETUPEVT_USER_ACT                (0x0000040)
#define CATI_REPORT_STATUS_EV_SETUPEVT_IDL_SCR_NOTIFY          (0x0000080)
#define CATI_REPORT_STATUS_EV_SETUPEVT_LANGSEL_NOTIFY          (0x0000100)
#define CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT              (0x0000200)
#define CATI_REPORT_STATUS_EV_LANG_NOTIFICATION                (0x0000400)
#define CATI_REPORT_STATUS_EV_REFRESH                          (0x0000800)
#define CATI_REPORT_STATUS_EV_END_PROACTIVE_SESSION            (0x0001000)
#define CATI_REPORT_STATUS_EV_PLAY_TONE                        (0x0002000)
#define CATI_REPORT_STATUS_EV_SETUP_CALL                       (0x0004000)
#define CATI_REPORT_STATUS_EV_SEND_DTMF                        (0x0008000)
#define CATI_REPORT_STATUS_EV_LAUNCH_BROWSER                   (0x0010000)
#define CATI_REPORT_STATUS_EV_SEND_SS                          (0x0020000)
#define CATI_REPORT_STATUS_EV_SEND_USSD                        (0x0040000)
#define CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_LANG          (0x0080000)
#define CATI_REPORT_STATUS_EV_DATA_EVT                         (0x0100000)
#define CATI_REPORT_STATUS_EV_SETUPEVT_BROWSER_TERM            (0x0200000)
#define CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_TIME          (0x0400000)
#define CATI_REPORT_STATUS_EV_SCWS_EVT                         (0x0800000)
#define CATI_REPORT_STATUS_EV_ACTIVATE                         (0x1000000)
#define CATI_REPORT_STATUS_EV_SETUPEVT_HCI_CONN                (0x2000000)
#define CATI_REPORT_STATUS_EV_DATA_EVT_STATUS                  (0x4000000)

/*---------------------------------------------------------------------------
  Set Event Report Decoded event bitmask
---------------------------------------------------------------------------*/
#define CATI_REPORT_STATUS_DECODED_NONE                        CATI_REPORT_STATUS_NONE
#define CATI_REPORT_STATUS_EV_DECODED_DISPLAY_TEXT             CATI_REPORT_STATUS_EV_DISPLAY_TEXT
#define CATI_REPORT_STATUS_EV_DECODED_GET_INKEY                CATI_REPORT_STATUS_EV_GET_INKEY
#define CATI_REPORT_STATUS_EV_DECODED_GET_INPUT                CATI_REPORT_STATUS_EV_GET_INPUT
#define CATI_REPORT_STATUS_EV_DECODED_SETUP_MENU               CATI_REPORT_STATUS_EV_SETUP_MENU
#define CATI_REPORT_STATUS_EV_DECODED_SELECT_ITEM              CATI_REPORT_STATUS_EV_SELECT_ITEM
#define CATI_REPORT_STATUS_EV_DECODED_SEND_SMS                 CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID
#define CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_USER_ACT        CATI_REPORT_STATUS_EV_SETUPEVT_USER_ACT
#define CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_IDL_SCR_NOTIFY  CATI_REPORT_STATUS_EV_SETUPEVT_IDL_SCR_NOTIFY
#define CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_LANGSEL_NOTIFY  CATI_REPORT_STATUS_EV_SETUPEVT_LANGSEL_NOTIFY
#define CATI_REPORT_STATUS_EV_DECODED_SETUP_IDLE_MODE_TEXT     CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT
#define CATI_REPORT_STATUS_EV_DECODED_LANG_NOTIFICATION        CATI_REPORT_STATUS_EV_LANG_NOTIFICATION
#define CATI_REPORT_STATUS_EV_DECODED_REFRESH                  CATI_REPORT_STATUS_EV_REFRESH
#define CATI_REPORT_STATUS_EV_DECODED_END_PROACTIVE_SESSION    CATI_REPORT_STATUS_EV_END_PROACTIVE_SESSION
#define CATI_REPORT_STATUS_EV_DECODED_PLAY_TONE                CATI_REPORT_STATUS_EV_PLAY_TONE
#define CATI_REPORT_STATUS_EV_DECODED_SETUP_CALL               CATI_REPORT_STATUS_EV_SETUP_CALL
#define CATI_REPORT_STATUS_EV_DECODED_SEND_DTMF                CATI_REPORT_STATUS_EV_SEND_DTMF
#define CATI_REPORT_STATUS_EV_DECODED_LAUNCH_BROWSER           CATI_REPORT_STATUS_EV_LAUNCH_BROWSER
#define CATI_REPORT_STATUS_EV_DECODED_SEND_SS                  CATI_REPORT_STATUS_EV_SEND_SS
#define CATI_REPORT_STATUS_EV_DECODED_SEND_USSD                CATI_REPORT_STATUS_EV_SEND_USSD
#define CATI_REPORT_STATUS_EV_DECODED_PROVIDE_LOCAL_INFO_LANG  CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_LANG
#define CATI_REPORT_STATUS_EV_DECODED_DATA_EVT                 CATI_REPORT_STATUS_EV_DATA_EVT
#define CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_BROWSER_TERM    CATI_REPORT_STATUS_EV_SETUPEVT_BROWSER_TERM
#define CATI_REPORT_STATUS_EV_DECODED_SCWS_EVT                 CATI_REPORT_STATUS_EV_SCWS_EVT
#define CATI_REPORT_STATUS_EV_DECODED_ACTIVATE                 CATI_REPORT_STATUS_EV_ACTIVATE
#define CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_HCI_CONN        CATI_REPORT_STATUS_EV_SETUPEVT_HCI_CONN
#define CATI_REPORT_STATUS_EV_DECODED_DATA_EVT_STATUS          CATI_REPORT_STATUS_EV_DATA_EVT_STATUS

/*---------------------------------------------------------------------------
  Set Event Report Full Function registration event bitmask
---------------------------------------------------------------------------*/
#define CATI_REPORT_STATUS_EV_FULL_FUNC_NONE                   (0x000000)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS               (0x000001)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL             (0x000002)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF              (0x000004)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS                (0x000008)
#define CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD              (0x000010)

/*---------------------------------------------------------------------------
  Event list indication
---------------------------------------------------------------------------*/
#define CATI_SETUP_EVT_LIST_MASK_USER_ACT                       0x01
#define CATI_SETUP_EVT_LIST_MASK_SCR_NOTIFY                     0x02
#define CATI_SETUP_EVT_LIST_MASK_LANGSEL_NOTIFY                 0x04
#define CATI_SETUP_EVT_LIST_MASK_HCI_CONNECITVITY               0x08

/*---------------------------------------------------------------------------
  Raw Envelope Cmd type
---------------------------------------------------------------------------*/
#define CATI_ENV_CMD_TYPE_MENU_SEL                              0x01
#define CATI_ENV_CMD_TYPE_EVT_DL_USER_ACT                       0x02
#define CATI_ENV_CMD_TYPE_EVT_DL_IDL_SCREEN_AVAIL               0x03
#define CATI_ENV_CMD_TYPE_EVT_DL_LANG_SEL                       0x04
#define CATI_ENV_CMD_TYPE_UNKNOWN                               0x05
#define CATI_ENV_CMD_TYPE_EVT_DL_BROWSER_TERM                   0x06
#define CATI_ENV_CMD_TYPE_SEND_CALL_CONTROL                     0x07
#define CATI_ENV_CMD_TYPE_EVT_DL_HCI_CONN                       0x08
#define CATI_ENV_CMD_TYPE_SMS_PP_DOWNLOAD                       0x09
#define CATI_ENV_CMD_TYPE_EVT_DL_MT_CALL                        0x0A
#define CATI_ENV_CMD_TYPE_EVT_DL_CALL_CONNECTED                 0x0B
#define CATI_ENV_CMD_TYPE_EVT_DL_CALL_DISCONNECTED              0x0C

/*---------------------------------------------------------------------------
  Type of QMI CAT messages posted to DCC task
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND,
  QMI_CAT_MESSAGE_DECODED_PROACTIVE_COMMAND,
  QMI_CAT_MESSAGE_DECODED_BIP_STATUS,
  QMI_CAT_MESSAGE_SCWS_COMMAND,
  QMI_CAT_MESSAGE_ENVELOPE_RESPONSE,
  QMI_CAT_MESSAGE_DECODED_ENVELOPE_RESPONSE,
  QMI_CAT_MESSAGE_END_PROACTIVE_SESSION,
  QMI_CAT_MESSAGE_TERM_RSP_SW1_SW2,
  QMI_CAT_MESSAGE_TERM_RSP_INDICATION,
  QMI_CAT_MESSAGE_ALLOC_CLID,
  QMI_CAT_MESSAGE_DEALLOC_CLID,
  QMI_CAT_MESSAGE_FRAMEWORK_CMD,
  QMI_CAT_MESSAGE_MMGSDI_CNF,
  QMI_CAT_MESSAGE_MMGSDI_EVT,
  QMI_CAT_MESSAGE_RECOVERY_COMPLETE,
  QMI_CAT_MESSAGE_TP_DL_IND
} qmi_cat_message_enum_type;

/*---------------------------------------------------------------------------
  Structure for QMI CAT messages
---------------------------------------------------------------------------*/
typedef struct
{
  /* This needs to be in first position */
  q_link_type                               link_next;

  qmi_cat_message_enum_type                 message_type;

  union
  {
    struct
    {
      gstk_slot_id_enum_type                slot_id;
      uint32                                cmd_ref_id;
      uint8                                 raw_hdr_tag;
      int32                                 raw_data_len;
      uint8                                 raw_data_buffer[ CAT_RAW_MAX_DATA_LEN ];
      boolean                               is_cached_cmd;
    }                                       raw_proactive_cmd;
    struct
    {
      gstk_cmd_from_card_type               decoded_cmd;
      boolean                               is_cached_cmd;
    }                                       decoded_proactive_cmd;
    struct
    {
      qmi_cat_decoded_bip_status_type       bip_status;
    }                                       decoded_bip_status;
    struct
    {
      gstk_cmd_from_card_type               scws_cmd;
    }                                       scws_cmd;
    struct
    {
      boolean                               envelope_data_not_present;
      gstk_cmd_from_card_type               envelope_rsp;
      void *                                user_data;
    }                                       envelope_rsp;
    struct
    {
      gstk_cmd_from_card_type               envelope_rsp;
    }                                       decoded_envelope_rsp;
    struct
    {
      gstk_slot_id_enum_type                slot_id;
      uint8                                 proactive_session_end_type;
      gstk_tag_enum_type                    tag;
    }                                       end_proactive_cmd;
    struct
    {
      gstk_cmd_from_card_type               sw1_sw2_data;
    }                                       terminal_response_sw1_sw2_info;
    struct
    {
      qmi_framework_common_msg_hdr_type     cmd_hdr;
    }                                       clid_cb;
    struct
    {
      qmi_framework_msg_hdr_type            cmd_hdr;
      dsm_item_type                       * sdu;
    }                                       framework_cmd;
    struct
    {
      mmgsdi_cnf_enum_type                  cnf_type;
      mmgsdi_cnf_type                       cnf_value;
    }                                       mmgsdi_cnf;
    struct
    {
      mmgsdi_events_enum_type               evt;
      mmgsdi_slot_id_enum_type              slot;
    }                                       mmgsdi_evt;
    struct
    {
      mmgsdi_slot_id_enum_type              slot;
    }                                       recovery_ind;
    struct
    {
      gstk_slot_id_enum_type                slot_id;
    }                                       tp_dl_ind;
    struct
    {
      gstk_cmd_from_card_type               terminal_response_rsp;
    }                                       terminal_response_rsp;
  }                                         data;
} qmi_cat_message_type;

/*---------------------------------------------------------------------------
  QMI CAT dispaly alpha confirmation type
---------------------------------------------------------------------------*/
typedef enum
{
  CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF                   = 0x00,
  CATI_DISPLAY_ALPHA_AUTOMATIC_FALSE                     = 0x01,
  CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE                      = 0x02
} qmi_cati_display_alpha_conf_e_type;

/*---------------------------------------------------------------------------
  Decoded Envelope command type
---------------------------------------------------------------------------*/
#define CATI_ENV_CMD_TYPE_DECODED_MENU_SEL                             0x01
#define CATI_ENV_CMD_TYPE_DECODED_EVT_DL_LANGUAGE                      0x02
#define CATI_ENV_CMD_TYPE_DECODED_EVT_USER_ACTIVITY                    0x03
#define CATI_ENV_CMD_TYPE_DECODED_EVT_IDLE_SCREEN                      0x04
#define CATI_ENV_CMD_TYPE_DECODED_SEND_CALL_CONTROL                    0x05
#define CATI_ENV_CMD_TYPE_DECODED_EVT_DL_HCI_CONN                      0x06
#define CATI_ENV_CMD_TYPE_DECODED_EVT_BROWSER_TERM                     0x07
#define CATI_ENV_CMD_TYPE_DECODED_SMS_PP_DOWNLOAD                      0x08
#define CATI_ENV_CMD_TYPE_DECODED_EVT_DL_MT_CALL                       0x09
#define CATI_ENV_CMD_TYPE_DECODED_EVT_DL_MT_CALL_CONNECTED             0x0A
#define CATI_ENV_CMD_TYPE_DECODED_EVT_DL_MO_CALL_CONNECTED             0x0B
#define CATI_ENV_CMD_TYPE_DECODED_EVT_DL_CALL_DISCONNECTED_NEAR_END    0x0C
#define CATI_ENV_CMD_TYPE_DECODED_EVT_DL_CALL_DISCONNECTED_FAR_END     0x0D

/*---------------------------------------------------------------------------
  Send TR command type
---------------------------------------------------------------------------*/
#define CATI_EVENT_DISPLAY_TEXT_CNF                      0x01
#define CATI_EVENT_GET_INKEY_CNF                         0x02
#define CATI_EVENT_GET_INPUT_CNF                         0x03
#define CATI_EVENT_LAUNCH_BROWSER_CNF                    0x04
#define CATI_EVENT_PLAY_TONE_CNF                         0x05
#define CATI_EVENT_SELECT_ITEM_CNF                       0x06
#define CATI_EVENT_SETUP_MENU_CNF                        0x07
#define CATI_EVENT_SETUP_IDLE_TEXT_CNF                   0x08
#define CATI_EVENT_PROVIDE_LANGUAGE_INFO_CNF             0x09
#define CATI_EVENT_USER_ACTIVITY_CNF                     0x0A
#define CATI_EVENT_IDLE_SCREEN_CNF                       0x0B
#define CATI_EVENT_LANGUAGE_SEL_CNF                      0x0C
#define CATI_EVENT_LANGUAGE_NOTIFICATION_CNF             0x0D
#define CATI_EVENT_ACTIVATE_CNF                          0x0E
#define CATI_EVENT_HCI_CONNECTIVITY_CNF                  0x0F
#define CATI_EVENT_BROWSER_TERM_CNF                      0x10
#define CATI_EVENT_SEND_SMS_CNF                          0x11
#define CATI_EVENT_SETUP_CALL_CNF                        0x12
#define CATI_EVENT_SEND_DTMF_CNF                         0x13
#define CATI_EVENT_SEND_SS_CNF                           0x14
#define CATI_EVENT_SEND_USSD_CNF                         0x15

/*---------------------------------------------------------------------------
  Get Cached Proactive Command command_id type
---------------------------------------------------------------------------*/
#define CATI_CACHED_CMD_TYPE_SETUP_MENU                  0x01
#define CATI_CACHED_CMD_TYPE_SETUP_EVT_LIST              0x02
#define CATI_CACHED_CMD_TYPE_SETUP_IDLE_MODE_TEXT        0x03

/*---------------------------------------------------------------------------
  QMI CAT card states
---------------------------------------------------------------------------*/
typedef enum
{
  /* when card is not present in the device or any card error */
  QMI_CAT_CARD_STATE_ERROR,
  /* card state between tp_dl and card insert event */
  QMI_CAT_CARD_STATE_INIT_IN_PROGRESS,
  /* card state after card insert event */
  QMI_CAT_CARD_STATE_PRESENT,
  /* card state during profile switch */
  QMI_CAT_CARD_STATE_INVALID
} qmi_cati_card_state_e_type;

typedef enum
{
  CATI_DATA_EVENT_NONE                   = 0,
  CATI_DATA_EVENT_OPEN_CHANNEL,
  CATI_DATA_EVENT_CLOSE_CHANNEL,
  CATI_DATA_EVENT_SEND_DATA,
  CATI_DATA_EVENT_RECEIVE_DATA,
} qmi_cati_data_event_e_type;

#define QMI_CAT_GSTK_EVT_MASK_CLASSC (GSTK_REG_LAUNCH_BROWSER_EVT                 |     \
                                      GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_FULL (GSTK_REG_DISPLAY_TEXT_EVT              |     \
                                           GSTK_REG_GET_INKEY_EVT                 |     \
                                           GSTK_REG_GET_INPUT_EVT                 |     \
                                           GSTK_REG_PLAY_TONE_EVT                 |     \
                                           GSTK_REG_SELECT_ITEM_EVT               |     \
                                           GSTK_REG_SETUP_MENU_EVT                |     \
                                           GSTK_REG_SETUP_IDLE_TEXT_EVT           |     \
                                           GSTK_REG_PROVIDE_LANG_INFO_EVT         |     \
                                           GSTK_REG_LANG_NOTIFY_EVT               |     \
                                           GSTK_REG_SETUP_EVT_USER_ACT_EVT        |     \
                                           GSTK_REG_SETUP_EVT_IDLE_MENU_EVT       |     \
                                           GSTK_REG_SETUP_EVT_LANG_SEL_EVT        |     \
                                           GSTK_REG_EVT_DOWNLOAD_RSP_EVT          |     \
                                           GSTK_REG_MENU_SEL_RSP_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_FULL_DATE_TIME (GSTK_REG_PROVIDE_DATE_TIME_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_REFRESH (GSTK_REG_REFRESH_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_WMS (GSTK_REG_SEND_SMS_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_CM_SUPPS ( GSTK_REG_SEND_SS_EVT               |     \
                                                        GSTK_REG_SEND_USSD_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_CM_VOICE_CALL ( GSTK_REG_SETUP_CALL_EVT             |     \
                                                             GSTK_REG_SEND_DTMF_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASSE_DISPLAY (GSTK_REG_DATA_EVT)

#define QMI_CAT_GSTK_EVT_MASK_SCWS_DISPLAY (GSTK_REG_SCWS_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASSL_DISPLAY (GSTK_REG_ACTIVATE_EVT)

#define QMI_CAT_GSTK_EVT_MASK_CLASSM_DISPLAY (GSTK_REG_SETUP_EVT_HCI_CONN_EVT)

static gstk_toolkit_cmd_reg_info_type  qmi_cati_complete_reg_list[] =
{
  { { GSTK_REG_CATEGORY_C_CMD, QMI_CAT_GSTK_EVT_MASK_CLASSC}, GSTK_HANDLE_ALL_FUNC},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_FULL}, GSTK_HANDLE_ALL_FUNC},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_REFRESH}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_WMS}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_CM_SUPPS}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_DISPLAY_CM_VOICE_CALL}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_E_CMD, QMI_CAT_GSTK_EVT_MASK_CLASSE_DISPLAY}, GSTK_HANDLE_DISPLAY_FUNC_ONLY},
  { { GSTK_REG_CATEGORY_M_CMD, QMI_CAT_GSTK_EVT_MASK_CLASSM_DISPLAY}, GSTK_HANDLE_ALL_FUNC},
  { { GSTK_REG_CATEGORY_L_CMD, QMI_CAT_GSTK_EVT_MASK_CLASSL_DISPLAY}, GSTK_HANDLE_ALL_FUNC}
};

static gstk_toolkit_cmd_reg_info_type  qmi_cati_scws_reg_list[] =
{
  { { GSTK_REG_CATEGORY_E_CMD, QMI_CAT_GSTK_EVT_MASK_SCWS_DISPLAY}, GSTK_HANDLE_DISPLAY_FUNC_ONLY}
};

static gstk_toolkit_cmd_reg_info_type  qmi_cati_date_time_reg_list[] =
{
  { { GSTK_REG_CATEGORY_0_CMD, QMI_CAT_GSTK_EVT_MASK_CLASS0_FULL_DATE_TIME}, GSTK_HANDLE_ALL_FUNC}
};

/* Empty TERMINAL PROFILE support from QMI-CAT */
static gstk_profile_dl_support_ext_type qmi_cati_empty_profile_value_list[1] = {{0}};

/* Custom raw/decoded format TERMINAL PROFILE support from QMI-CAT */
static struct
{
  gstk_profile_dl_support_ext_type  *value_list_ptr;
  uint8                              tp_count;
}qmi_cati_custom_profile = {NULL, 0};

/* Decoded format TERMINAL PROFILE support from QMI-CAT */
static gstk_profile_dl_support_ext_type qmi_cati_decoded_profile_value_list[] =
{
  {1, GSTK_MENU_SEL_SUPPORT,                     (uint8)GSTK_APP_NONE},
  {1, GSTK_USSD_DO_IN_CC_SUPPORT,                (uint8)GSTK_APP_SIM},
  {1, GSTK_UCS2_ENTRY_SUPPORT,                   (uint8)GSTK_APP_NONE},
  {1, GSTK_UCS2_DISPLAY_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_DISPLAY_OF_EXTN_TEXT,                 (uint8)GSTK_APP_SIM},
  {1, GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8,    (uint8)GSTK_APP_USIM},
  {1, GSTK_DISPLAY_TEXT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_GET_INKEY_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_GET_INPUT_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_SELECT_ITEM_SUPPORT,                  (uint8)GSTK_APP_NONE},
  {1, GSTK_PLAY_TONE_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_SET_UP_MENU_SUPPORT,                  (uint8)GSTK_APP_NONE},
  {1, GSTK_IDLE_SCRN_AVAIL_EVT_SUPPORT,          (uint8)GSTK_APP_NONE},
  {1, GSTK_LANG_SEL_EVT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_SET_UP_IDLE_MODE_TEXT_SUPPORT,        (uint8)GSTK_APP_NONE},
  {1, GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1,  (uint8)GSTK_APP_USIM},
  {1, GSTK_SUSTAINED_DISP_TEXT_SUPPORT,          (uint8)GSTK_APP_SIM},
  {1, GSTK_LAUNCH_BROWSER_SUPPORT,               (uint8)GSTK_APP_NONE},
  {1, GSTK_BROWSER_STATUS_EVT_SUPPORT,           (uint8)GSTK_APP_NONE},
  {1, GSTK_PROVIDE_LOCAL_INFO_LANG_SUPPORT,      (uint8)GSTK_APP_NONE},
  {1, GSTK_SK_SUPPORT_IN_SEL_ITEM,               (uint8)GSTK_APP_NONE},
  {1, GSTK_SK_SUPPORT_IN_SET_UP_MENU,            (uint8)GSTK_APP_NONE},
  {1, GSTK_OPEN_CH_SUPPORT,                      (uint8)GSTK_APP_NONE},
  {1, GSTK_CLOSE_CH_SUPPORT,                     (uint8)GSTK_APP_NONE},
  {1, GSTK_RECEIVE_DATA_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_SEND_DATA_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_BROWSER_TERM_EVT_SUPPORT,             (uint8)GSTK_APP_NONE},
  {2, GSTK_MAX_SK_SUPPORT,                       (uint8)GSTK_APP_NONE},
  {1, GSTK_LANGUAGE_NOTIFICATION_SUPPORT,        (uint8)GSTK_APP_NONE},
  {1, GSTK_USER_ACT_EVT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_ACTIVATE_SUPPORT,                     (uint8)GSTK_APP_NONE},
  {1, GSTK_HCI_CONNECTIVITY_EVT_SUPPORT,         (uint8)GSTK_APP_NONE},
  {1, GSTK_ALPHA_REFRESH_SUPPORT,                (uint8)GSTK_APP_NONE},
  {1, GSTK_EXT_LAUNCH_BROWSER_HTML_SUPPORT,      (uint8)GSTK_APP_NONE},
  {1, GSTK_DISPLAY_TXT_VARIABLE_TIMEOUT_SUPPORT, (uint8)GSTK_APP_USIM},
  {1, GSTK_INKEY_HELP_FOR_VARIABLE_TIMEOUT_AND_IMMEDIATE_DIGIT_RSP,
                                                 (uint8)GSTK_APP_USIM},
  {1, GSTK_INKEY_VARIABLE_TIMEOUT_SUPPORT,       (uint8)GSTK_APP_USIM}
};

/* Android TERMINAL PROFILE support from QMI-CAT */
static gstk_profile_dl_support_ext_type qmi_cati_android_profile_value_list[] =
{
  {1, GSTK_MENU_SEL_SUPPORT,                     (uint8)GSTK_APP_NONE},
  {1, GSTK_USSD_DO_IN_CC_SUPPORT,                (uint8)GSTK_APP_SIM},
  {1, GSTK_UCS2_ENTRY_SUPPORT,                   (uint8)GSTK_APP_NONE},
  {1, GSTK_UCS2_DISPLAY_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_DISPLAY_OF_EXTN_TEXT,                 (uint8)GSTK_APP_SIM},
  {1, GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8,    (uint8)GSTK_APP_USIM},
  {1, GSTK_DISPLAY_TEXT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_GET_INKEY_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_GET_INPUT_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_SELECT_ITEM_SUPPORT,                  (uint8)GSTK_APP_NONE},
  {1, GSTK_PLAY_TONE_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_SET_UP_MENU_SUPPORT,                  (uint8)GSTK_APP_NONE},
  {1, GSTK_IDLE_SCRN_AVAIL_EVT_SUPPORT,          (uint8)GSTK_APP_NONE},
  {1, GSTK_LANG_SEL_EVT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_SET_UP_IDLE_MODE_TEXT_SUPPORT,        (uint8)GSTK_APP_NONE},
  {1, GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1,  (uint8)GSTK_APP_USIM},
  {1, GSTK_SUSTAINED_DISP_TEXT_SUPPORT,          (uint8)GSTK_APP_SIM},
  {1, GSTK_LAUNCH_BROWSER_SUPPORT,               (uint8)GSTK_APP_NONE},
  {1, GSTK_BROWSER_STATUS_EVT_SUPPORT,           (uint8)GSTK_APP_NONE},
  {1, GSTK_PROVIDE_LOCAL_INFO_LANG_SUPPORT,      (uint8)GSTK_APP_NONE},
  {1, GSTK_PROVIDE_LOCAL_INFO_TIME_SUPPORT,      (uint8)GSTK_APP_NONE},
  {1, GSTK_SK_SUPPORT_IN_SEL_ITEM,               (uint8)GSTK_APP_NONE},
  {1, GSTK_SK_SUPPORT_IN_SET_UP_MENU,            (uint8)GSTK_APP_NONE},
  {1, GSTK_OPEN_CH_SUPPORT,                      (uint8)GSTK_APP_NONE},
  {1, GSTK_CLOSE_CH_SUPPORT,                     (uint8)GSTK_APP_NONE},
  {1, GSTK_RECEIVE_DATA_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_SEND_DATA_SUPPORT,                    (uint8)GSTK_APP_NONE},
  {1, GSTK_BROWSER_TERM_EVT_SUPPORT,             (uint8)GSTK_APP_NONE},
  {2, GSTK_MAX_SK_SUPPORT,                       (uint8)GSTK_APP_NONE},
  {1, GSTK_DISPLAY_TXT_VARIABLE_TIMEOUT_SUPPORT, (uint8)GSTK_APP_USIM},
  {1, GSTK_INKEY_HELP_FOR_VARIABLE_TIMEOUT_AND_IMMEDIATE_DIGIT_RSP,
                                                 (uint8)GSTK_APP_USIM},
  {1, GSTK_INKEY_VARIABLE_TIMEOUT_SUPPORT,       (uint8)GSTK_APP_USIM},
  {1, GSTK_ACTIVATE_SUPPORT,                     (uint8)GSTK_APP_NONE},
  {1, GSTK_HCI_CONNECTIVITY_EVT_SUPPORT,         (uint8)GSTK_APP_NONE},
  {1, GSTK_ALPHA_REFRESH_SUPPORT,                (uint8)GSTK_APP_NONE},
  {1, GSTK_LANGUAGE_NOTIFICATION_SUPPORT,        (uint8)GSTK_APP_NONE},
  {1, GSTK_USER_ACT_EVT_SUPPORT,                 (uint8)GSTK_APP_NONE},
  {1, GSTK_EXT_LAUNCH_BROWSER_HTML_SUPPORT,      (uint8)GSTK_APP_NONE}
};

#define CATI_RAW_PROACTIVE_CMD_MASK ((CATI_REPORT_STATUS_EV_DISPLAY_TEXT)       | \
                                     (CATI_REPORT_STATUS_EV_GET_INKEY)          | \
                                     (CATI_REPORT_STATUS_EV_GET_INPUT)          | \
                                     (CATI_REPORT_STATUS_EV_SETUP_MENU)         | \
                                     (CATI_REPORT_STATUS_EV_SELECT_ITEM)        | \
                                     (CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT)| \
                                     (CATI_REPORT_STATUS_EV_LANG_NOTIFICATION)  | \
                                     (CATI_REPORT_STATUS_EV_PLAY_TONE)          | \
                                     (CATI_REPORT_STATUS_EV_SETUP_CALL)         | \
                                     (CATI_REPORT_STATUS_EV_SEND_DTMF)          | \
                                     (CATI_REPORT_STATUS_EV_LAUNCH_BROWSER)     | \
                                     (CATI_REPORT_STATUS_EV_SEND_SS)            | \
                                     (CATI_REPORT_STATUS_EV_SEND_USSD)          | \
                                     (CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID)   | \
                                     (CATI_REPORT_STATUS_EV_DATA_EVT)           | \
                                     (CATI_REPORT_STATUS_EV_SCWS_EVT)           | \
                                     (CATI_REPORT_STATUS_EV_ACTIVATE)           | \
                                     (CATI_REPORT_STATUS_EV_REFRESH))

#define CATI_SETUP_EVT_LIST_MASK ((CATI_REPORT_STATUS_EV_SETUPEVT_USER_ACT) | \
                            (CATI_REPORT_STATUS_EV_SETUPEVT_IDL_SCR_NOTIFY) | \
                            (CATI_REPORT_STATUS_EV_SETUPEVT_LANGSEL_NOTIFY) | \
                            (CATI_REPORT_STATUS_EV_SETUPEVT_BROWSER_TERM) | \
                            (CATI_REPORT_STATUS_EV_SETUPEVT_HCI_CONN))

#define CATI_PROVIDE_LOCAL_INFO_MASK ((CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_LANG) | \
                            (CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_TIME))

#define CATI_CACHED_EVT_MASK ((CATI_REPORT_STATUS_EV_DISPLAY_TEXT)           | \
                              (CATI_REPORT_STATUS_EV_SETUP_MENU)             | \
                              (CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT)    | \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_USER_ACT)      | \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_IDL_SCR_NOTIFY)| \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_LANGSEL_NOTIFY)| \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_HCI_CONN)      | \
                              (CATI_REPORT_STATUS_EV_SETUPEVT_BROWSER_TERM)  | \
                              (CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_LANG)| \
                              (CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_TIME))

#define CATI_DECODED_PROACTIVE_CMD_MASK ((CATI_REPORT_STATUS_EV_DECODED_DISPLAY_TEXT)            | \
                                         (CATI_REPORT_STATUS_EV_DECODED_GET_INKEY)               | \
                                         (CATI_REPORT_STATUS_EV_DECODED_GET_INPUT)               | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SETUP_MENU)              | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SELECT_ITEM)             | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SEND_SMS)                | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_USER_ACT)       | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_IDL_SCR_NOTIFY) | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_LANGSEL_NOTIFY) | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SETUP_IDLE_MODE_TEXT)    | \
                                         (CATI_REPORT_STATUS_EV_DECODED_LANG_NOTIFICATION)       | \
                                         (CATI_REPORT_STATUS_EV_DECODED_PLAY_TONE)               | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SETUP_CALL)              | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SEND_DTMF)               | \
                                         (CATI_REPORT_STATUS_EV_DECODED_LAUNCH_BROWSER)          | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SEND_SS)                 | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SEND_USSD)               | \
                                         (CATI_REPORT_STATUS_EV_DECODED_PROVIDE_LOCAL_INFO_LANG) | \
                                         (CATI_REPORT_STATUS_EV_DECODED_DATA_EVT)                | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_BROWSER_TERM)   | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SCWS_EVT)                | \
                                         (CATI_REPORT_STATUS_EV_DECODED_ACTIVATE)                | \
                                         (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_HCI_CONN)       | \
                                         (CATI_REPORT_STATUS_EV_DECODED_DATA_EVT_STATUS)         | \
                                         (CATI_REPORT_STATUS_EV_DECODED_REFRESH))

#define CATI_DECODED_CACHED_EVT_MASK ((CATI_REPORT_STATUS_EV_DECODED_DISPLAY_TEXT)               | \
                                      (CATI_REPORT_STATUS_EV_DECODED_SETUP_MENU)                 | \
                                      (CATI_REPORT_STATUS_EV_DECODED_SETUP_IDLE_MODE_TEXT)       | \
                                      (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_USER_ACT)          | \
                                      (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_IDL_SCR_NOTIFY)    | \
                                      (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_LANGSEL_NOTIFY)    | \
                                      (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_HCI_CONN)          | \
                                      (CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_BROWSER_TERM)      | \
                                      (CATI_REPORT_STATUS_EV_DECODED_SCWS_EVT)                   | \
                                      (CATI_REPORT_STATUS_EV_DECODED_PROVIDE_LOCAL_INFO_LANG))

#define CATI_PROACTIVE_SESSION_END_DUE_TO_CARD  0x01
#define CATI_PROACTIVE_SESSION_END_INTERNAL     0x02

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/
/*---------------------------------------------------------------------------
  QMI Card Application Toolkit Service (CAT) instance state definition &
  CAT client state definition
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_common_client_state_type   common; // must be first since we alias
  uint32                         evt_report_mask[QMI_CAT_MAX_CARD_COUNT];
  uint32                         decoded_evt_report_mask[QMI_CAT_MAX_CARD_COUNT];
  int16                          instance;
  int16                          service_id;
} qmi_cati_client_state_type;

/*---------------------------------------------------------------------------
  Watermark queue to hold deferred responses
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_common_svc_state_type    common; // must be first since we alias
  qmi_cati_client_state_type * client[CATI_MAX_CLIDS];
  uint32                       client_reg_count[CATI_MAX_CLIDS];
} qmi_cati_state_type;

/*---------------------------------------------------------------------------
  Proactive Cmd Evt Report type
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                  mask;
  gstk_slot_id_enum_type  slot_id;
  uint8                   response_type;

  /* Proactive command evt */
  union
  {
    /* raw evt */
    struct
    {
      uint32                       cmd_ref_id;
      uint16                       raw_data_len;
      const uint8*                 raw_data_ptr;
      qmi_cati_data_event_e_type   data_evt_type;
    }cati_raw_pc_evt;

    /* end proactive session evt */
    struct
    {
      uint8     proactive_session_end_type;
    }cati_end_proactive_cmd_evt;

  } cati_evt_rpt;
} qmi_cati_evt_rpt_info_s_type;

/*---------------------------------------------------------------------------
  Structure holding cached proactive command (raw format)
---------------------------------------------------------------------------*/
typedef struct
{
  union
  {
    /* raw evt (setup menu or setup evt list) */
    struct
    {
      uint32    cmd_ref_id;
      uint16    raw_data_len;
      uint8     raw_data[CAT_RAW_MAX_DATA_LEN];
    } cati_raw_pc_evt;

    /* setup evt list */
    struct
    {
      uint32     pc_setup_evt_list;
    } cati_setup_evt_list_cmd_evt;
  } cati_evt_rpt;
} qmi_cati_cached_evt_info_type;

/*---------------------------------------------------------------------------
  Structure holding cached proactive command
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_cati_cached_evt_info_type     cached_event;
} qmi_cati_raw_cache_type;

typedef struct
{
  gstk_cmd_from_card_type           cached_event;
} qmi_cati_decoded_cache_type;

/*---------------------------------------------------------------------------
  Structure holding userdata required during GSTK response
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_cati_cmd_val_e_type               request_id;
  uint8                                 clid;
  uint32                                client_reg_count;
  qmi_cati_ind_token_info_type          ind_token;
  void *                                cmd_buf_ptr;
} qmi_cati_userdata_type;

/*---------------------------------------------------------------------------
  Structure holding dynamically allocated cache
---------------------------------------------------------------------------*/
struct qmi_cati_cache_node
{
  struct qmi_cati_cache_node      * next_ptr;
  qmi_cati_userdata_type          * cb_userdata_ptr;
};

typedef struct qmi_cati_cache_node qmi_cati_cmd_buf_cache_type;

/*---------------------------------------------------------------------------
  QMI service command handlers
  forward declarations & cmd handler dispatch table definition
---------------------------------------------------------------------------*/
static dsm_item_type* qmi_cati_reset(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_set_event_report(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_get_service_state(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_send_tr(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_send_envelope_cmd(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_get_event_report(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_send_decoded_tr(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_decoded_envelope_cmd(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_event_confirmation(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_scws_open_channel(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_scws_close_channel(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_scws_send_data(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_scws_data_available(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_scws_change_status(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_get_terminal_profile(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_set_configuration(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_get_configuration(void*, void*, void*, dsm_item_type **);
static dsm_item_type* qmi_cati_get_cached_proactive_cmd(void*, void*, void*, dsm_item_type **);


#define CAT_HDLR(a,b)  QMI_SVC_HDLR( a, (qmi_svc_hdlr_ftype)b )

static qmi_svc_cmd_hdlr_type  qmi_cati_cmd_callbacks[] =
{
  CAT_HDLR( CATI_CMD_VAL_RESET,                     qmi_cati_reset),
  CAT_HDLR( CATI_CMD_VAL_SET_EVENT_REPORT,          qmi_cati_set_event_report),
  CAT_HDLR( CATI_CMD_VAL_GET_SERVICE_STATE,         qmi_cati_get_service_state),
  CAT_HDLR( CATI_CMD_VAL_SEND_TR,                   qmi_cati_send_tr ),
  CAT_HDLR( CATI_CMD_VAL_SEND_ENVELOPE_CMD,         qmi_cati_send_envelope_cmd ),
  CAT_HDLR( CATI_CMD_VAL_GET_EVENT_REPORT,          qmi_cati_get_event_report),
  CAT_HDLR( CATI_CMD_VAL_SEND_DECODED_TR,           qmi_cati_send_decoded_tr),
  CAT_HDLR( CATI_CMD_VAL_DECODED_ENVELOPE_CMD,      qmi_cati_decoded_envelope_cmd),
  CAT_HDLR( CATI_CMD_VAL_EVENT_CONFIRMATION_CMD,    qmi_cati_event_confirmation),
  CAT_HDLR( CATI_CMD_VAL_SCWS_OPEN_CH,              qmi_cati_scws_open_channel),
  CAT_HDLR( CATI_CMD_VAL_SCWS_CLOSE_CH,             qmi_cati_scws_close_channel),
  CAT_HDLR( CATI_CMD_VAL_SCWS_SEND_DATA,            qmi_cati_scws_send_data),
  CAT_HDLR( CATI_CMD_VAL_SCWS_DATA_AVAILABLE,       qmi_cati_scws_data_available),
  CAT_HDLR( CATI_CMD_VAL_SCWS_CHANNEL_STATUS,       qmi_cati_scws_change_status),
  CAT_HDLR( CATI_CMD_VAL_GET_TERMINAL_PROFILE,      qmi_cati_get_terminal_profile),
  CAT_HDLR( CATI_CMD_VAL_SET_CONFIGURATION,         qmi_cati_set_configuration),
  CAT_HDLR( CATI_CMD_VAL_GET_CONFIGURATION,         qmi_cati_get_configuration),
  CAT_HDLR( CATI_CMD_VAL_GET_CACHED_PROACTIVE_CMD,  qmi_cati_get_cached_proactive_cmd)
  /* NOTE: When adding an item to this table, also add it to
     qmi_cat_is_command_disabled() */
};

/*---------------------------------------------------------------------------
  QMI instance configurations
---------------------------------------------------------------------------*/
static void     qmi_cati_reset_client(qmi_cati_client_state_type *);
/* Prototype for QMI CAT generic message processing */
static void     qmi_cat_process_message(void);

static qmi_cati_state_type *qmi_cat_state_ptr = NULL;

/*---------------------------------------------------------------------------
  QMI_CAT Global state information - initialized in qmi_cat_init()
  Global to all QMI instances
---------------------------------------------------------------------------*/
typedef struct
{
  /* Message queue */
  q_type                                     qmi_cat_q;
  /* Client ids */
  gstk_client_id_type                        cat_gstk_client_id[QMI_CAT_MAX_CARD_COUNT];
  gstk_client_id_type                        cat_gstk_client_id_external[QMI_CAT_MAX_CARD_COUNT];
  /* Number of slots device supports */
  uint8                                      slot_count;
  /* Global masks of registered events */
  uint32                                     evt_reg_mask[QMI_CAT_MAX_CARD_COUNT];
  uint32                                     decoded_evt_reg_mask[QMI_CAT_MAX_CARD_COUNT];
  uint32                                     full_func_evt_reg_mask[QMI_CAT_MAX_CARD_COUNT];
  /* Flag to set if the event received is registered as a full functionality for the ongoing proactive command */
  boolean                                    full_func_evt[QMI_CAT_MAX_CARD_COUNT];
  /* Cache of proactive commands */
  qmi_cati_raw_cache_type                  * raw_cache[QMI_CAT_MAX_CARD_COUNT][CATI_MAX_RAW_CACHED_EVTS];
  qmi_cati_raw_cache_type                  * recovery_raw_cache[QMI_CAT_MAX_CARD_COUNT][CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS];
  qmi_cati_decoded_cache_type              * decoded_cache[QMI_CAT_MAX_CARD_COUNT][CATI_MAX_DECODED_CACHED_EVTS];
  gstk_cmd_enum_type                         last_gstk_cmd[QMI_CAT_MAX_CARD_COUNT];
  mmgsdi_client_id_type                      cat_mmgsdi_client_id;
  /* Configuration mode */
  qmi_cati_config_mode_e_type                config_mode;
  /* Flag to indication return value for display alpha confirmation */
  qmi_cati_display_alpha_conf_e_type         display_alpha_conf;
  /* Flag to control use of sms_pp_envelope commands */
  boolean                                    sms_pp_envelope_blocked;
  /* Reference command id for event confirmation */
  uint32                                     ref_command_id[QMI_CAT_MAX_CARD_COUNT];
  /* BIP channel status */
  qmi_cati_bip_status_e_type                 bip_status_cache[QMI_CAT_MAX_CARD_COUNT][QMI_CATI_MAX_BIP_CHANNEL];
  /* Track numbers of SETUP_EVENT_XX_NOTIFY that is waiting for TR */
  uint8                                      decoded_setup_evt_in_progress[QMI_CAT_MAX_CARD_COUNT];
  /* command buffer of pending scws requests*/
  qmi_cati_cmd_buf_cache_type              * scws_cmd_buf[QMI_CAT_MAX_CARD_COUNT][QMI_CATI_MAX_SCWS_CHANNEL];
  /* card state */
  qmi_cati_card_state_e_type                 card_state[QMI_CAT_MAX_CARD_COUNT];
}qmi_cati_global_type;

static qmi_cati_global_type                  *qmi_cati_global_ptr = NULL;

typedef struct
{
  gstk_cmd_enum_type  command_id;  /* command id */
  uint8               cache_index; /* index to qmi_cati_global_type.decoded_cache[] */
}qmi_cat_decoded_cache_cmd_index_type;

const static qmi_cat_decoded_cache_cmd_index_type  qmi_cat_decoded_cache_cmd_index[] =
{
  {GSTK_SETUP_MENU_REQ,                 CATI_CACHED_EVT_DECODED_SETUP_MENU},
  {GSTK_SETUP_IDLE_TEXT_REQ,            CATI_CACHED_EVT_DECODED_SETUP_IDLE_MODE_TEXT},
  {GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY,   CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_IDLE_SCREEN},
  {GSTK_SETUP_EVT_USER_ACT_NOTIFY,      CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_USER_ACT},
  {GSTK_SETUP_EVT_LANG_SEL_NOTIFY,      CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_LANG_SEL},
  {GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY,   CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_HCI_CONN},
  {GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY,  CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_BROWSER_TERM},
  {GSTK_DISPLAY_TEXT_REQ,               CATI_CACHED_EVT_DECODED_DISPLAY_TEXT},
  {GSTK_SCWS_OPEN_CH_REQ,               CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL},
  {GSTK_PROVIDE_LANG_INFO_REQ,          CATI_CACHED_EVT_DECODED_PROVIDE_LOCAL_INFO_LANG}
};

typedef struct
{
  uint8  command_id;  /* command id */
  uint8  cache_index; /* index to qmi_cati_global_type.raw_cache[] */
}qmi_cat_raw_cache_cmd_index_type;

const static qmi_cat_raw_cache_cmd_index_type  qmi_cat_raw_cache_cmd_index[] =
{
  {GSTK_CMD_STK_SET_UP_MENU,            CATI_CACHED_EVT_SETUP_MENU},
  {GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT,  CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT},
  {GSTK_CMD_STK_SET_UP_EVENT_LIST,      CATI_CACHED_EVT_SETUP_EVT_LIST},
  {GSTK_CMD_STK_DISPLAY_TEXT,           CATI_CACHED_EVT_DISPLAY_TEXT},
  {GSTK_CMD_STK_PROVIDE_LOCAL_INFO,     CATI_CACHED_EVT_PROVIDE_LOCAL_INFO}
};

const static qmi_cat_raw_cache_cmd_index_type  qmi_recovery_cat_raw_cache_cmd_index[] =
{
  {GSTK_CMD_STK_SET_UP_MENU,            CATI_QMI_CLIENT_CACHED_EVT_SETUP_MENU},
  {GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT,  CATI_QMI_CLIENT_CACHED_EVT_SETUP_IDLE_MODE_TEXT},
  {GSTK_CMD_STK_SET_UP_EVENT_LIST,      CATI_QMI_CLIENT_CACHED_EVT_SETUP_EVT_LIST},
};

typedef struct
{
  uint8               stk_cmd_id;
  gstk_cmd_enum_type  gstk_cmd_id;
}qmi_cat_stk_cmd_to_gstk_cmd_type;

const static qmi_cat_stk_cmd_to_gstk_cmd_type qmi_cat_stk_cmd_to_gstk_cmd[] =
{
  {GSTK_CMD_STK_DISPLAY_TEXT,           GSTK_DISPLAY_TEXT_REQ},
  {GSTK_CMD_STK_GET_INKEY,              GSTK_GET_INKEY_REQ},
  {GSTK_CMD_STK_GET_INPUT,              GSTK_GET_INPUT_REQ},
  {GSTK_CMD_STK_SET_UP_MENU,            GSTK_SETUP_MENU_REQ},
  {GSTK_CMD_STK_SELECT_ITEM,            GSTK_SELECT_ITEM_REQ},
  {GSTK_CMD_STK_SEND_SHORT_MESSAGE,     GSTK_SEND_SMS_REQ},
  {GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT,  GSTK_SETUP_IDLE_TEXT_REQ},
  {GSTK_CMD_STK_LANG_NOTIFICATION,      GSTK_LANG_NOTIFICATION_REQ},
  {GSTK_CMD_STK_PLAY_TONE,              GSTK_PLAY_TONE_REQ},
  {GSTK_CMD_STK_SET_UP_CALL,            GSTK_SETUP_CALL_REQ},
  {GSTK_CMD_STK_SEND_DTMF,              GSTK_SEND_DTMF_REQ},
  {GSTK_CMD_STK_LAUNCH_BROWSER,         GSTK_LAUNCH_BROWSER_REQ},
  {GSTK_CMD_STK_SEND_SS,                GSTK_SEND_SS_REQ},
  {GSTK_CMD_STK_USSD,                   GSTK_SEND_USSD_REQ},
  {GSTK_CMD_STK_PROVIDE_LOCAL_INFO,     GSTK_PROVIDE_LOCAL_INFO_REQ},
  {GSTK_CMD_STK_SET_UP_EVENT_LIST,      GSTK_SETUP_EVENT_LIST_REQ},
  {GSTK_CMD_STK_OPEN_CHANNEL,           GSTK_OPEN_CH_REQ},
  {GSTK_CMD_STK_CLOSE_CHANNEL,          GSTK_CLOSE_CH_REQ},
  {GSTK_CMD_STK_RECEIVE_DATA,           GSTK_RECEIVE_DATA_REQ},
  {GSTK_CMD_STK_SEND_DATA,              GSTK_SEND_DATA_REQ},
  {GSTK_CMD_STK_ACTIVATE,               GSTK_ACTIVATE_REQ},
  {GSTK_CMD_STK_REFRESH,                GSTK_REFRESH_REQ}
};

/*===========================================================================

                       FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/* GSTK CB's*/
static void qmi_cati_gstk_init_cb ( gstk_status_enum_type      gstk_reg_status,
                                    gstk_client_id_type        client_id,
                                    gstk_client_ref_data_type  user_data );

static void qmi_cati_gstk_init_cb_external( gstk_status_enum_type      gstk_reg_status,
                                            gstk_client_id_type        client_id,
                                            gstk_client_ref_data_type  user_data );

static void qmi_cati_gstk_cmd_cb( gstk_status_enum_type        gstk_reg_status,
                                  gstk_client_ref_data_type    user_data );

static void qmi_cati_gstk_evt_cb( gstk_client_pro_cmd_reg_data_type *req_ptr );

static void qmi_cati_gstk_raw_env_cb( gstk_cmd_from_card_type  *env_rsp_ptr );

static void qmi_cati_gstk_decoded_env_cb( gstk_cmd_from_card_type  *env_rsp_ptr );

static void qmi_cat_mmgsdi_response_cb (mmgsdi_return_enum_type status,
                                        mmgsdi_cnf_enum_type    cnf_type,
                                        const mmgsdi_cnf_type  *cnf_ptr);

static void qmi_cat_mmgsdi_evt_cb(const mmgsdi_event_data_type   *mmgsdi_event);

/* Prototypes of QMI CAT Framework communication */
static boolean qmi_cati_send_response(qmi_cmd_buf_type *cmd_buf_p,
                                      dsm_item_type    *msg_ptr);

static boolean qmi_cati_send_indication(uint8           client_id,
                                        uint16          cmd_type,
                                        dsm_item_type  *ind);

/* Local utility functions */
static uint8 qmi_cati_find_length_of_length_value(const uint8 * length_value);


/*===========================================================================

                             LOCAL FUNCTIONS

===========================================================================*/


/*===========================================================================
  FUNCTION QMI_CATI_MALLOC_COPY

  DESCRIPTION
    Makes a deep copy of a proactive command in decoded format

  PARAMETERS
    gstk_cmd_from_card_type: source procative command
    gstk_cmd_from_card_type: destination procative command

  RETURN VALUE
    boolean: result of the copy

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_malloc_copy
(
  void       ** dst,
  const void  * src,
  uint32        size
)
{
  ASSERT(dst != NULL);

  *dst = NULL;

  if (src != NULL && size > 0)
  {
    *dst = uimqmi_malloc(size);
    if (*dst != NULL)
    {
      (void)memscpy(*dst, size, src, size);
    }
  }
} /* qmi_cati_malloc_copy */


/*===========================================================================
  FUNCTION QMI_CATI_CLONE_DECODED_PROACTIVE_COMMAND

  DESCRIPTION
    Makes a deep copy of a proactive command in decoded format

  PARAMETERS
    gstk_cmd_from_card_type: source procative command
    gstk_cmd_from_card_type: destination procative command

  RETURN VALUE
    boolean: result of the copy

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_clone_decoded_proactive_command
(
  gstk_cmd_from_card_type       * dst,
  const gstk_cmd_from_card_type * src
)
{
  uint16 i = 0;

  ASSERT(src != NULL);
  ASSERT(dst != NULL);

  /* Copy header */
  (void)memscpy(&dst->hdr_cmd,
                sizeof(gstk_exp_hdr_type),
                &src->hdr_cmd,
                sizeof(gstk_exp_hdr_type));

  switch(src->hdr_cmd.command_id)
  {
    case GSTK_DISPLAY_TEXT_REQ:
      (void)memscpy(&dst->cmd.display_text_pro_cmd_req,
                    sizeof(gstk_display_text_req_type),
                    &src->cmd.display_text_pro_cmd_req,
                    sizeof(gstk_display_text_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.display_text_pro_cmd_req.text_string.text,
                           (void*)src->cmd.display_text_pro_cmd_req.text_string.text,
                            src->cmd.display_text_pro_cmd_req.text_string.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.display_text_pro_cmd_req.icon.data,
                           (void*)src->cmd.display_text_pro_cmd_req.icon.data,
                            src->cmd.display_text_pro_cmd_req.icon.size);
      break;
    case GSTK_GET_INKEY_REQ:
      (void)memscpy(&dst->cmd.get_inkey_pro_cmd_req,
                    sizeof(gstk_get_inkey_req_type),
                    &src->cmd.get_inkey_pro_cmd_req,
                    sizeof(gstk_get_inkey_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.get_inkey_pro_cmd_req.text_string.text,
                           (void*)src->cmd.get_inkey_pro_cmd_req.text_string.text,
                            src->cmd.get_inkey_pro_cmd_req.text_string.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.get_inkey_pro_cmd_req.icon.data,
                           (void*)src->cmd.get_inkey_pro_cmd_req.icon.data,
                            src->cmd.get_inkey_pro_cmd_req.icon.size);
      break;
    case GSTK_GET_INPUT_REQ:
      (void)memscpy(&dst->cmd.get_input_pro_cmd_req,
                    sizeof(gstk_get_input_req_type),
                    &src->cmd.get_input_pro_cmd_req,
                    sizeof(gstk_get_input_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.get_input_pro_cmd_req.text_string.text,
                           (void*)src->cmd.get_input_pro_cmd_req.text_string.text,
                            src->cmd.get_input_pro_cmd_req.text_string.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.get_input_pro_cmd_req.default_text.text,
                           (void*)src->cmd.get_input_pro_cmd_req.default_text.text,
                            src->cmd.get_input_pro_cmd_req.default_text.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.get_input_pro_cmd_req.icon.data,
                           (void*)src->cmd.get_input_pro_cmd_req.icon.data,
                            src->cmd.get_input_pro_cmd_req.icon.size);
      break;
    case GSTK_LAUNCH_BROWSER_REQ:
      (void)memscpy(&dst->cmd.launch_browser_pro_cmd_req,
                    sizeof(gstk_launch_browser_req_type),
                    &src->cmd.launch_browser_pro_cmd_req,
                    sizeof(gstk_launch_browser_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.launch_browser_pro_cmd_req.url.url,
                           (void*)src->cmd.launch_browser_pro_cmd_req.url.url,
                            src->cmd.launch_browser_pro_cmd_req.url.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.launch_browser_pro_cmd_req.bearer.bearer_list,
                           (void*)src->cmd.launch_browser_pro_cmd_req.bearer.bearer_list,
                            src->cmd.launch_browser_pro_cmd_req.bearer.length * sizeof(gstk_bearer_code_enum_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text,
                           (void*)src->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text,
                            src->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text,
                           (void*)src->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text,
                            src->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data,
                           (void*)src->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data,
                            src->cmd.launch_browser_pro_cmd_req.user_confirm_icon.size);
      break;
    case GSTK_PLAY_TONE_REQ:
      (void)memscpy(&dst->cmd.play_tone_pro_cmd_req,
                    sizeof(gstk_play_tone_req_type),
                    &src->cmd.play_tone_pro_cmd_req,
                    sizeof(gstk_play_tone_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.play_tone_pro_cmd_req.alpha.text,
                           (void*)src->cmd.play_tone_pro_cmd_req.alpha.text,
                            src->cmd.play_tone_pro_cmd_req.alpha.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.play_tone_pro_cmd_req.icon.data,
                           (void*)src->cmd.play_tone_pro_cmd_req.icon.data,
                            src->cmd.play_tone_pro_cmd_req.icon.size);
      break;
    case GSTK_SELECT_ITEM_REQ:
      (void)memscpy(&dst->cmd.select_item_pro_cmd_req,
                    sizeof(gstk_select_item_req_type),
                    &src->cmd.select_item_pro_cmd_req,
                    sizeof(gstk_select_item_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.select_item_pro_cmd_req.title.text,
                           (void*)src->cmd.select_item_pro_cmd_req.title.text,
                            src->cmd.select_item_pro_cmd_req.title.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.select_item_pro_cmd_req.icon.data,
                           (void*)src->cmd.select_item_pro_cmd_req.icon.data,
                            src->cmd.select_item_pro_cmd_req.icon.size);
      qmi_cati_malloc_copy((void**)&dst->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list,
                           (void*)src->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list,
                            src->cmd.select_item_pro_cmd_req.next_action_indicator.num_items);
      for (i = 0; i < src->cmd.select_item_pro_cmd_req.num_items && i < GSTK_MAX_MENU_ITEMS; i++)
      {
        qmi_cati_malloc_copy((void**)&dst->cmd.select_item_pro_cmd_req.items[i].item_text,
                             (void*)src->cmd.select_item_pro_cmd_req.items[i].item_text,
                              src->cmd.select_item_pro_cmd_req.items[i].length);
      }
      for (i = 0; i < src->cmd.select_item_pro_cmd_req.icon_id_list.num_items && i < GSTK_MAX_MENU_ITEMS; i++)
      {
        qmi_cati_malloc_copy((void**)&dst->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i],
                             (void*)src->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i],
                              sizeof(gstk_icon_type));
        if (dst->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL &&
            src->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL)
        {
          qmi_cati_malloc_copy((void**)&dst->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data,
                               (void*)src->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data,
                                src->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->size);
        }
      }
      break;
    case GSTK_SEND_SMS_REQ:
      (void)memscpy(&dst->cmd.send_sms_pro_cmd_req,
                    sizeof(gstk_send_sms_req_type),
                    &src->cmd.send_sms_pro_cmd_req,
                    sizeof(gstk_send_sms_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu,
                           (void*)src->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu,
                            src->cmd.send_sms_pro_cmd_req.sms_tpdu.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.send_sms_pro_cmd_req.alpha.text,
                           (void*)src->cmd.send_sms_pro_cmd_req.alpha.text,
                            src->cmd.send_sms_pro_cmd_req.alpha.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.send_sms_pro_cmd_req.icon.data,
                           (void*)src->cmd.send_sms_pro_cmd_req.icon.data,
                            src->cmd.send_sms_pro_cmd_req.icon.size);
      break;
    case GSTK_SEND_SS_REQ:
      (void)memscpy(&dst->cmd.send_ss_pro_cmd_req,
                    sizeof(gstk_send_ss_req_type),
                    &src->cmd.send_ss_pro_cmd_req,
                    sizeof(gstk_send_ss_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.send_ss_pro_cmd_req.alpha.text,
                           (void*)src->cmd.send_ss_pro_cmd_req.alpha.text,
                            src->cmd.send_ss_pro_cmd_req.alpha.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.send_ss_pro_cmd_req.icon.data,
                           (void*)src->cmd.send_ss_pro_cmd_req.icon.data,
                            src->cmd.send_ss_pro_cmd_req.icon.size);
      break;
    case GSTK_SEND_USSD_REQ:
      (void)memscpy(&dst->cmd.send_ussd_pro_cmd_req,
                    sizeof(gstk_send_ussd_req_type),
                    &src->cmd.send_ussd_pro_cmd_req,
                    sizeof(gstk_send_ussd_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.send_ussd_pro_cmd_req.ussd_string.text,
                           (void*)src->cmd.send_ussd_pro_cmd_req.ussd_string.text,
                            src->cmd.send_ussd_pro_cmd_req.ussd_string.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.send_ussd_pro_cmd_req.alpha.text,
                           (void*)src->cmd.send_ussd_pro_cmd_req.alpha.text,
                            src->cmd.send_ussd_pro_cmd_req.alpha.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.send_ussd_pro_cmd_req.icon.data,
                           (void*)src->cmd.send_ussd_pro_cmd_req.icon.data,
                            src->cmd.send_ussd_pro_cmd_req.icon.size);
      break;
    case GSTK_SETUP_CALL_REQ:
      (void)memscpy(&dst->cmd.setup_call_pro_cmd_req,
                    sizeof(gstk_setup_call_req_type),
                    &src->cmd.setup_call_pro_cmd_req,
                    sizeof(gstk_setup_call_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text,
                           (void*)src->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text,
                            src->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text,
                           (void*)src->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text,
                            src->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.setup_call_pro_cmd_req.user_cnf_icon.data,
                           (void*)src->cmd.setup_call_pro_cmd_req.user_cnf_icon.data,
                            src->cmd.setup_call_pro_cmd_req.user_cnf_icon.size);
      qmi_cati_malloc_copy((void**)&dst->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data,
                           (void*)src->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data,
                            src->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size);
      break;
    case GSTK_SETUP_MENU_REQ:
      (void)memscpy(&dst->cmd.setup_menu_pro_cmd_req,
                    sizeof(gstk_setup_menu_req_type),
                    &src->cmd.setup_menu_pro_cmd_req,
                    sizeof(gstk_setup_menu_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.setup_menu_pro_cmd_req.title.text,
                           (void*)src->cmd.setup_menu_pro_cmd_req.title.text,
                            src->cmd.setup_menu_pro_cmd_req.title.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list,
                           (void*)src->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list,
                            src->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items);
      qmi_cati_malloc_copy((void**)&dst->cmd.setup_menu_pro_cmd_req.icon.data,
                           (void*)src->cmd.setup_menu_pro_cmd_req.icon.data,
                            src->cmd.setup_menu_pro_cmd_req.icon.size);
      for (i = 0; i < src->cmd.setup_menu_pro_cmd_req.num_items && i < GSTK_MAX_MENU_ITEMS; i++)
      {
        qmi_cati_malloc_copy((void**)&dst->cmd.setup_menu_pro_cmd_req.items[i].item_text,
                             (void*)src->cmd.setup_menu_pro_cmd_req.items[i].item_text,
                              src->cmd.setup_menu_pro_cmd_req.items[i].length);
      }
      for (i = 0; i < src->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items && i < GSTK_MAX_MENU_ITEMS; i++)
      {
        qmi_cati_malloc_copy((void**)&dst->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i],
                             (void*)src->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i],
                              sizeof(gstk_icon_type));
        if (dst->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL &&
            src->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL)
        {
          qmi_cati_malloc_copy((void**)&dst->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data,
                               (void*)src->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data,
                                src->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->size);
        }
      }
      break;
    case GSTK_SETUP_IDLE_TEXT_REQ:
      (void)memscpy(&dst->cmd.setup_idle_mode_txt_pro_cmd_req,
                    sizeof(gstk_setup_idle_mode_text_req_type),
                    &src->cmd.setup_idle_mode_txt_pro_cmd_req,
                    sizeof(gstk_setup_idle_mode_text_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text,
                           (void*)src->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text,
                            src->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data,
                           (void*)src->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data,
                            src->cmd.setup_idle_mode_txt_pro_cmd_req.icon.size);
      break;
    case GSTK_SEND_DTMF_REQ:
      (void)memscpy(&dst->cmd.send_dtmf_pro_cmd_req,
                    sizeof(gstk_send_dtmf_req_type),
                    &src->cmd.send_dtmf_pro_cmd_req,
                    sizeof(gstk_send_dtmf_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf,
                           (void*)src->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf,
                            src->cmd.send_dtmf_pro_cmd_req.dtmf.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.send_dtmf_pro_cmd_req.alpha.text,
                           (void*)src->cmd.send_dtmf_pro_cmd_req.alpha.text,
                            src->cmd.send_dtmf_pro_cmd_req.alpha.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.send_dtmf_pro_cmd_req.icon.data,
                           (void*)src->cmd.send_dtmf_pro_cmd_req.icon.data,
                            src->cmd.send_dtmf_pro_cmd_req.icon.size);
      break;
    case GSTK_LANG_NOTIFICATION_REQ:
      (void)memscpy(&dst->cmd.lang_notification_pro_cmd_req,
                    sizeof(gstk_lang_notification_req_type),
                    &src->cmd.lang_notification_pro_cmd_req,
                    sizeof(gstk_lang_notification_req_type));
      break;
    case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
    case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
    case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
    case GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY:
    case GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY:
      (void)memscpy(&dst->cmd.evt_notify_req,
                    sizeof(gstk_evt_notify_type),
                    &src->cmd.evt_notify_req,
                    sizeof(gstk_evt_notify_type));
      break;
    case GSTK_PROVIDE_LOCAL_INFO_REQ:
    case GSTK_PROVIDE_LANG_INFO_REQ:
    case GSTK_PROVIDE_DATE_TIME_REQ:
      /* Nothing to copy for these commands */
      break;
    case GSTK_OPEN_CH_REQ:
      (void)memscpy(&dst->cmd.open_ch_pro_cmd_req,
                    sizeof(gstk_open_ch_req_type),
                    &src->cmd.open_ch_pro_cmd_req,
                    sizeof(gstk_open_ch_req_type));
      /* alphabet is type uint16, byte size for copying is alphabet.length * 2 */
      qmi_cati_malloc_copy((void**)&dst->cmd.open_ch_pro_cmd_req.alpha.alphabet,
                           (void*)src->cmd.open_ch_pro_cmd_req.alpha.alphabet,
                           (src->cmd.open_ch_pro_cmd_req.alpha.length * 2));
      qmi_cati_malloc_copy((void**)&dst->cmd.open_ch_pro_cmd_req.icon.data,
                           (void*)src->cmd.open_ch_pro_cmd_req.icon.data,
                            src->cmd.open_ch_pro_cmd_req.icon.size);
      qmi_cati_malloc_copy((void**)&dst->cmd.open_ch_pro_cmd_req.access_point_name.alphabet,
                           (void*)src->cmd.open_ch_pro_cmd_req.access_point_name.alphabet,
                           (src->cmd.open_ch_pro_cmd_req.access_point_name.length * 2));
      qmi_cati_malloc_copy((void**)&dst->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address,
                           (void*)src->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address,
                           src->cmd.open_ch_pro_cmd_req.local_address.pkt_address.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet,
                           (void*)src->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet,
                           (src->cmd.open_ch_pro_cmd_req.text_string_user_login.length * 2));
      qmi_cati_malloc_copy((void**)&dst->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet,
                           (void*)src->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet,
                           (src->cmd.open_ch_pro_cmd_req.text_string_user_passwd.length *2));
      qmi_cati_malloc_copy((void**)&dst->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address,
                           (void*)src->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address,
                           src->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length);
      break;
    case GSTK_CLOSE_CH_REQ:
      (void)memscpy(&dst->cmd.close_ch_pro_cmd_req,
                    sizeof(gstk_close_ch_req_type),
                    &src->cmd.close_ch_pro_cmd_req,
                    sizeof(gstk_close_ch_req_type));
      /* alphabet is type uint16, byte size for copying is alphabet.length * 2 */
      qmi_cati_malloc_copy((void**)&dst->cmd.close_ch_pro_cmd_req.alpha.alphabet,
                           (void*)src->cmd.close_ch_pro_cmd_req.alpha.alphabet,
                           (src->cmd.close_ch_pro_cmd_req.alpha.length * 2));
      qmi_cati_malloc_copy((void**)&dst->cmd.close_ch_pro_cmd_req.icon.data,
                           (void*)src->cmd.close_ch_pro_cmd_req.icon.data,
                            src->cmd.close_ch_pro_cmd_req.icon.size);
      break;
    case GSTK_SEND_DATA_REQ:
      (void)memscpy(&dst->cmd.send_data_pro_cmd_req,
                    sizeof(gstk_send_data_req_type),
                    &src->cmd.send_data_pro_cmd_req,
                    sizeof(gstk_send_data_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.send_data_pro_cmd_req.ch_data.data,
                           (void*)src->cmd.send_data_pro_cmd_req.ch_data.data,
                           src->cmd.send_data_pro_cmd_req.ch_data.data_len);
      /* alphabet is type uint16, byte size for copying is alphabet.length * 2 */
      qmi_cati_malloc_copy((void**)&dst->cmd.send_data_pro_cmd_req.alpha.alphabet,
                           (void*)src->cmd.send_data_pro_cmd_req.alpha.alphabet,
                           (src->cmd.send_data_pro_cmd_req.alpha.length * 2));
      qmi_cati_malloc_copy((void**)&dst->cmd.send_data_pro_cmd_req.icon.data,
                           (void*)src->cmd.send_data_pro_cmd_req.icon.data,
                            src->cmd.send_data_pro_cmd_req.icon.size);
      break;
    case GSTK_RECEIVE_DATA_REQ:
      (void)memscpy(&dst->cmd.receive_data_pro_cmd_req,
                    sizeof(gstk_receive_data_req_type),
                    &src->cmd.receive_data_pro_cmd_req,
                    sizeof(gstk_receive_data_req_type));
      /* alphabet is type uint16, byte size for copying is alphabet.length * 2 */
      qmi_cati_malloc_copy((void**)&dst->cmd.receive_data_pro_cmd_req.alpha.alphabet,
                           (void*)src->cmd.receive_data_pro_cmd_req.alpha.alphabet,
                           (src->cmd.receive_data_pro_cmd_req.alpha.length * 2));
      qmi_cati_malloc_copy((void**)&dst->cmd.receive_data_pro_cmd_req.icon.data,
                           (void*)src->cmd.receive_data_pro_cmd_req.icon.data,
                            src->cmd.receive_data_pro_cmd_req.icon.size);
      break;
    case GSTK_SCWS_OPEN_CH_REQ:
      (void)memscpy(&dst->cmd.scws_open_ch_cmd_req,
                    sizeof(gstk_scws_open_ch_req_type),
                    &src->cmd.scws_open_ch_cmd_req,
                    sizeof(gstk_scws_open_ch_req_type));
      /* alphabet is type uint16, byte size for copying is alphabet.length * 2 */
      qmi_cati_malloc_copy((void**)&dst->cmd.scws_open_ch_cmd_req.alpha.alphabet,
                           (void*)src->cmd.scws_open_ch_cmd_req.alpha.alphabet,
                           (src->cmd.scws_open_ch_cmd_req.alpha.length * 2));
      break;
    case GSTK_SCWS_CLOSE_CH_REQ:
      (void)memscpy(&dst->cmd.scws_close_ch_cmd_req,
                    sizeof(gstk_scws_close_ch_req_type),
                    &src->cmd.scws_close_ch_cmd_req,
                    sizeof(gstk_scws_close_ch_req_type));
      break;
    case GSTK_SCWS_SEND_DATA_REQ:
      (void)memscpy(&dst->cmd.scws_send_data_cmd_req,
                    sizeof(gstk_scws_send_data_req_type),
                    &src->cmd.scws_send_data_cmd_req,
                    sizeof(gstk_scws_send_data_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.scws_send_data_cmd_req.ch_data.data,
                           (void*)src->cmd.scws_send_data_cmd_req.ch_data.data,
                           src->cmd.scws_send_data_cmd_req.ch_data.data_len);
      break;
    case GSTK_SCWS_DATA_AVAIL_RSP_REQ:
      (void)memscpy(&dst->cmd.scws_data_avail_rsp_cmd_req,
                    sizeof(gstk_scws_data_avail_rsp_req_type),
                    &src->cmd.scws_data_avail_rsp_cmd_req,
                    sizeof(gstk_scws_data_avail_rsp_req_type));
      break;
    case GSTK_ACTIVATE_REQ:
      (void)memscpy(&dst->cmd.activate_pro_cmd_req,
                    sizeof(gstk_activate_req_type),
                    &src->cmd.activate_pro_cmd_req,
                    sizeof(gstk_activate_req_type));
      break;

    case GSTK_TERM_RSP_SW1_SW2_RPT:
      (void)memscpy(&dst->cmd.term_rsp_sw1_sw2_rpt,
                    sizeof(gstk_term_rsp_sw1_sw2_rpt_type),
                    &src->cmd.term_rsp_sw1_sw2_rpt,
                    sizeof(gstk_term_rsp_sw1_sw2_rpt_type));
      break;

    case GSTK_REFRESH_REQ:
      (void)memscpy(&dst->cmd.refresh_pro_cmd_req,
                    sizeof(gstk_refresh_req_type),
                    &src->cmd.refresh_pro_cmd_req,
                    sizeof(gstk_refresh_req_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.refresh_pro_cmd_req.alpha.text,
                           (void*)src->cmd.refresh_pro_cmd_req.alpha.text,
                            src->cmd.refresh_pro_cmd_req.alpha.length);
      qmi_cati_malloc_copy((void**)&dst->cmd.refresh_pro_cmd_req.icon.data,
                           (void*)src->cmd.refresh_pro_cmd_req.icon.data,
                            src->cmd.refresh_pro_cmd_req.icon.size);
      break;
    default:
      UIM_MSG_ERR_1("QMI_CAT: Invalid proactive command: 0x%x",
                    src->hdr_cmd.command_id);
      return FALSE;
  }

  return TRUE;
} /* qmi_cati_clone_decoded_proactive_command */


/*===========================================================================
  FUNCTION QMI_CATI_FREE_DECODED_PROACTIVE_COMMAND

  DESCRIPTION
    Makes a deep copy of a proactive command in decoded format

  PARAMETERS
    gstk_cmd_from_card_type: source procative command
    gstk_cmd_from_card_type: destination procative command

  RETURN VALUE
    boolean: result of the copy

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_free_decoded_proactive_command
(
  gstk_cmd_from_card_type       * command
)
{
  uint16 i = 0;

  ASSERT(command != NULL);

  switch(command->hdr_cmd.command_id)
  {
    case GSTK_DISPLAY_TEXT_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.display_text_pro_cmd_req.text_string.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.display_text_pro_cmd_req.icon.data);
      break;
    case GSTK_GET_INKEY_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.get_inkey_pro_cmd_req.text_string.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.get_inkey_pro_cmd_req.icon.data);
      break;
    case GSTK_GET_INPUT_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.get_input_pro_cmd_req.text_string.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.get_input_pro_cmd_req.default_text.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.get_input_pro_cmd_req.icon.data);
      break;
    case GSTK_LAUNCH_BROWSER_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.launch_browser_pro_cmd_req.url.url);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.launch_browser_pro_cmd_req.bearer.bearer_list);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data);
      break;
    case GSTK_PLAY_TONE_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.play_tone_pro_cmd_req.alpha.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.play_tone_pro_cmd_req.icon.data);
      break;
    case GSTK_SELECT_ITEM_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.select_item_pro_cmd_req.title.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.select_item_pro_cmd_req.icon.data);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list);
      for (i = 0; i < command->cmd.select_item_pro_cmd_req.num_items && i < GSTK_MAX_MENU_ITEMS; i++)
      {
        QMI_CAT_FREE_IF_NOT_NULL(command->cmd.select_item_pro_cmd_req.items[i].item_text);
      }
      for (i = 0; i < command->cmd.select_item_pro_cmd_req.icon_id_list.num_items && i < GSTK_MAX_MENU_ITEMS; i++)
      {
        if (command->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i])
        {
          QMI_CAT_FREE_IF_NOT_NULL(command->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data);
        }
        QMI_CAT_FREE_IF_NOT_NULL(command->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]);
      }
      break;
    case GSTK_SEND_SMS_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_sms_pro_cmd_req.alpha.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_sms_pro_cmd_req.icon.data);
      break;
    case GSTK_SEND_SS_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_ss_pro_cmd_req.alpha.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_ss_pro_cmd_req.icon.data);
      break;
    case GSTK_SEND_USSD_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_ussd_pro_cmd_req.ussd_string.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_ussd_pro_cmd_req.alpha.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_ussd_pro_cmd_req.icon.data);
      break;
    case GSTK_SETUP_CALL_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_call_pro_cmd_req.user_cnf_icon.data);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data);
      break;
    case GSTK_SETUP_MENU_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_menu_pro_cmd_req.title.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_menu_pro_cmd_req.icon.data);
      for (i = 0; i < command->cmd.setup_menu_pro_cmd_req.num_items && i < GSTK_MAX_MENU_ITEMS; i++)
      {
        QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_menu_pro_cmd_req.items[i].item_text);
      }
      for (i = 0; i < command->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items && i < GSTK_MAX_MENU_ITEMS; i++)
      {
        if (command->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i])
        {
          QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data);
        }
        QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]);
      }
      break;
    case GSTK_SETUP_IDLE_TEXT_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data);
      break;
    case GSTK_SEND_DTMF_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_dtmf_pro_cmd_req.alpha.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_dtmf_pro_cmd_req.icon.data);
      break;
    case GSTK_LANG_NOTIFICATION_REQ:
    case GSTK_PROVIDE_LOCAL_INFO_REQ:
    case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
    case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
    case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
    case GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY:
    case GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY:
    case GSTK_PROVIDE_LANG_INFO_REQ:
    case GSTK_SCWS_CLOSE_CH_REQ:
    case GSTK_SCWS_DATA_AVAIL_RSP_REQ:
    case GSTK_ACTIVATE_REQ:
      /* Nothing to deep free for these commands */
      break;
    case GSTK_OPEN_CH_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.open_ch_pro_cmd_req.alpha.alphabet);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.open_ch_pro_cmd_req.icon.data);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.open_ch_pro_cmd_req.access_point_name.alphabet);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address);
      break;
    case GSTK_CLOSE_CH_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.close_ch_pro_cmd_req.alpha.alphabet);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.close_ch_pro_cmd_req.icon.data);
      break;
    case GSTK_SEND_DATA_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_data_pro_cmd_req.ch_data.data);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_data_pro_cmd_req.alpha.alphabet);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.send_data_pro_cmd_req.icon.data);
      break;
    case GSTK_RECEIVE_DATA_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.receive_data_pro_cmd_req.alpha.alphabet);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.receive_data_pro_cmd_req.icon.data);
      break;
    case GSTK_SCWS_OPEN_CH_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.scws_open_ch_cmd_req.alpha.alphabet);
      break;
    case GSTK_SCWS_SEND_DATA_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.scws_send_data_cmd_req.ch_data.data);
      break;
    case GSTK_REFRESH_REQ:
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.refresh_pro_cmd_req.alpha.text);
      QMI_CAT_FREE_IF_NOT_NULL(command->cmd.refresh_pro_cmd_req.icon.data);
      break;
    default:
      UIM_MSG_ERR_1("QMI_CAT: Invalid proactive command: 0x%x",
                    command->hdr_cmd.command_id);
      break;
  }

  /* Zero the command, as it's no longer required */
  memset(command, 0, sizeof(gstk_cmd_from_card_type));
} /* qmi_cati_free_decoded_proactive_command */


/*===========================================================================
  FUNCTION QMI_CATI_CLONE_ENVELOPE_RSP

  DESCRIPTION
    Makes a deep copy of an envelope rsp in raw format

  PARAMETERS
    gstk_cmd_from_card_type: source env rsp
    gstk_cmd_from_card_type: destination env rsp

  RETURN VALUE
    boolean: result of the copy

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_clone_envelope_rsp
(
  gstk_cmd_from_card_type       * dst,
  const gstk_cmd_from_card_type * src
)
{
  ASSERT(src != NULL);
  ASSERT(dst != NULL);

  memset(dst, 0, sizeof(gstk_exp_hdr_type));

  /* Copy header */
  (void)memscpy(&dst->hdr_cmd,
                sizeof(gstk_exp_hdr_type),
                &src->hdr_cmd,
                sizeof(gstk_exp_hdr_type));

  if (src->hdr_cmd.command_id == GSTK_RAW_ENVELOPE_RSP)
  {
    (void)memscpy(&dst->cmd.raw_envelope_rsp,
                  sizeof(gstk_raw_envelope_rsp_type),
                  &src->cmd.raw_envelope_rsp,
                  sizeof(gstk_raw_envelope_rsp_type));
    qmi_cati_malloc_copy((void**)&dst->cmd.raw_envelope_rsp.rsp_data_ptr,
                         (void*)src->cmd.raw_envelope_rsp.rsp_data_ptr,
                         src->cmd.raw_envelope_rsp.length);
  }
  else
  {
    UIM_MSG_ERR_1("qmi_cati_clone_envelope_rsp:Invalid cmd id 0x%x",
                  src->hdr_cmd.command_id);
    return FALSE;
  }
  return TRUE;
} /* qmi_cati_clone_envelope_rsp */


/*===========================================================================
  FUNCTION QMI_CATI_FREE_ENVELOPE_RSP

  DESCRIPTION
    Frees previously allocated memory for envelope response

  PARAMETERS
    gstk_cmd_from_card_type: Envelope response

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_free_envelope_rsp
(
  gstk_cmd_from_card_type       * command_ptr
)
{
  ASSERT(command_ptr != NULL);

  switch(command_ptr->hdr_cmd.command_id)
  {
    case GSTK_RAW_ENVELOPE_RSP:
      QMI_CAT_FREE_IF_NOT_NULL(command_ptr->cmd.raw_envelope_rsp.rsp_data_ptr);
      break;

    default:
      UIM_MSG_ERR_1("qmi_cati_free_envelope_rsp: Invalid cmd id 0x%x",
                    command_ptr->hdr_cmd.command_id);
      break;
  }
  /* Zero the command, as it's no longer required */
  memset(command_ptr, 0, sizeof(gstk_cmd_from_card_type));
} /* qmi_cati_free_envelope_rsp */


/*===========================================================================
  FUNCTION QMI_CATI_CLONE_DECODED_ENVELOPE_RSP

  DESCRIPTION
    Makes a deep copy of an envelope rsp in decoded format

  PARAMETERS
    gstk_cmd_from_card_type: source env rsp
    gstk_cmd_from_card_type: destination env rsp

  RETURN VALUE
    boolean: result of the copy

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_clone_decoded_envelope_rsp
(
  gstk_cmd_from_card_type       * dst,
  const gstk_cmd_from_card_type * src
)
{
  ASSERT(src != NULL);
  ASSERT(dst != NULL);

  memset(dst, 0, sizeof(gstk_exp_hdr_type));

  /* Copy header */
  (void)memscpy(&dst->hdr_cmd,
                sizeof(gstk_exp_hdr_type),
                &src->hdr_cmd,
                sizeof(gstk_exp_hdr_type));

  switch (src->hdr_cmd.command_id)
  {
    case GSTK_CC_IND_RSP:
      (void)memscpy(&dst->cmd.cc_rsp,
                    sizeof(gstk_cc_rsp_type),
                    &src->cmd.cc_rsp,
                    sizeof(gstk_cc_rsp_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.cc_rsp.cc_req_action.alpha.text,
                           (void*)src->cmd.cc_rsp.cc_req_action.alpha.text,
                           src->cmd.cc_rsp.cc_req_action.alpha.length);
      if (src->cmd.cc_rsp.cc_req_action.call_type == GSTK_USSD)
      {
        qmi_cati_malloc_copy((void**)&dst->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text,
                             (void*)src->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text,
                             src->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.length);
      }
      break;

    case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
      (void)memscpy(&dst->cmd.sms_pp_download_rsp,
                    sizeof(gstk_sms_pp_download_rsp_type),
                    &src->cmd.sms_pp_download_rsp,
                    sizeof(gstk_sms_pp_download_rsp_type));
      qmi_cati_malloc_copy((void**)&dst->cmd.sms_pp_download_rsp.ack.ack,
                           (void*)src->cmd.sms_pp_download_rsp.ack.ack,
                           src->cmd.sms_pp_download_rsp.ack.length);
      break;

    case GSTK_MO_SMS_CTRL_IND_RSP:
       (void)memscpy(&dst->cmd.mo_sms_cc_rsp,
                     sizeof(gstk_mo_sms_ctrl_rsp_type),
                     &src->cmd.mo_sms_cc_rsp,
                     sizeof(gstk_mo_sms_ctrl_rsp_type));
       qmi_cati_malloc_copy((void**)&dst->cmd.mo_sms_cc_rsp.alpha.text,
                            (void*)src->cmd.mo_sms_cc_rsp.alpha.text,
                            src->cmd.mo_sms_cc_rsp.alpha.length);
       break;

    default:
      break;
  }
  return TRUE;
} /* qmi_cati_clone_decoded_envelope_rsp */


/*===========================================================================
  FUNCTION QMI_CATI_FREE_DECODED_ENVELOPE_RSP

  DESCRIPTION
    Frees previously allocated memory for decoded envelope response

  PARAMETERS
    gstk_cmd_from_card_type: Envelope response

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_free_decoded_envelope_rsp
(
  gstk_cmd_from_card_type       * command_ptr
)
{
  ASSERT(command_ptr != NULL);

  switch(command_ptr->hdr_cmd.command_id)
  {
    case GSTK_CC_IND_RSP:
      QMI_CAT_FREE_IF_NOT_NULL(command_ptr->cmd.cc_rsp.cc_req_action.alpha.text);
      if (command_ptr->cmd.cc_rsp.cc_req_action.call_type == GSTK_USSD)
      {
        QMI_CAT_FREE_IF_NOT_NULL(command_ptr->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text);
      }
      break;
    case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
      QMI_CAT_FREE_IF_NOT_NULL(command_ptr->cmd.sms_pp_download_rsp.ack.ack);
      break;
    case GSTK_MO_SMS_CTRL_IND_RSP:
      QMI_CAT_FREE_IF_NOT_NULL(command_ptr->cmd.mo_sms_cc_rsp.alpha.text);
      break;
    default:
      UIM_MSG_ERR_1("qmi_cati_free_decoded_envelope_rsp: Invalid cmd id 0x%x",
                    command_ptr->hdr_cmd.command_id);
      break;
  }
  /* Zero the command, as it's no longer required */
  memset(command_ptr, 0, sizeof(gstk_cmd_from_card_type));
} /* qmi_cati_free_decoded_envelope_rsp */


/*===========================================================================
QMI_CAT_SIG_HANDLER

DESCRIPTION
  QMI CAT signal handler for GSTK signals

PARAMETERS
  None.

  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
boolean qmi_cat_sig_handler
(
  void
)
{
  ASSERT(qmi_cati_global_ptr);

  qmi_cat_process_message();

  /* This signal is set asyncronously however the framework handles
     concurrency issues so no need to worry about it here */
  if (q_cnt(&qmi_cati_global_ptr->qmi_cat_q) == 0)
  {
    return TRUE;
  }

  return FALSE;
} /* qmi_cat_sig_handler */


/*===========================================================================
FUNCTION QMI_CAT_FREE_MESSAGE()

DESCRIPTION
  This function frees cat_message_ptr

PARAMETERS
  message_type: type of QMI CAT message

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void qmi_cat_free_message
(
  qmi_cat_message_type * cat_message_ptr
)
{
  if(cat_message_ptr != NULL)
  {
    switch(cat_message_ptr->message_type)
    {
      case QMI_CAT_MESSAGE_ENVELOPE_RESPONSE:
        qmi_cati_free_envelope_rsp(&(cat_message_ptr->data.envelope_rsp.envelope_rsp));
        break;
      case QMI_CAT_MESSAGE_DECODED_ENVELOPE_RESPONSE:
        qmi_cati_free_decoded_envelope_rsp(
            &(cat_message_ptr->data.decoded_envelope_rsp.envelope_rsp));
        break;
      case QMI_CAT_MESSAGE_DECODED_PROACTIVE_COMMAND:
        qmi_cati_free_decoded_proactive_command(
            &(cat_message_ptr->data.decoded_proactive_cmd.decoded_cmd));
        break;
      case QMI_CAT_MESSAGE_SCWS_COMMAND:
        qmi_cati_free_decoded_proactive_command(&(cat_message_ptr->data.scws_cmd.scws_cmd));
        break;
      default:
        /* Nothing to free here */
        break;
    }
    uimqmi_free(cat_message_ptr);
  }
} /* qmi_cat_free_message */


/*===========================================================================
  FUNCTION QMI_CAT_CREATE_MESSAGE

  DESCRIPTION
    Allocates the memory for a QMI CAT message and initializes it.

  PARAMETERS
    message_type  : type of QMI CAT message

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cat_message_type * qmi_cat_create_message
(
  qmi_cat_message_enum_type message_type
)
{
  qmi_cat_message_type * msg_ptr = NULL;

  msg_ptr = (qmi_cat_message_type*)uimqmi_malloc(sizeof(qmi_cat_message_type));
  if (msg_ptr != NULL)
  {
    memset(msg_ptr, 0, sizeof(qmi_cat_message_type));
    msg_ptr->message_type = message_type;
  }
  else
  {
    UIM_MSG_ERR_0("Error allocating memory for QMI CAT message");
  }

  return msg_ptr;
} /* qmi_cat_create_message */


/*===========================================================================
  FUNCTION QMI_CAT_POST_MESSAGE

  DESCRIPTION
    Adds a message to the QMI CAT queue and posts an event to DCC task,
    so that it can be processed later in the right context.

  PARAMETERS
    cat_message_ptr  : QMI CAT message

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_post_message
(
  qmi_cat_message_type * cat_message_ptr
)
{
  ASSERT(qmi_cati_global_ptr);

  if(cat_message_ptr == NULL)
  {
    return;
  }

  /* Add the message to the QMI CAT queue */
  q_put(&qmi_cati_global_ptr->qmi_cat_q, &cat_message_ptr->link_next);

  /* Enable the GSTK_QMI_CAT_SIG to process in GSTK context*/
  gstk_set_qmi_cat_sig();
} /* qmi_cat_post_message */


/*===========================================================================
  FUNCTION QMI_CAT_GET_MESSAGE

  DESCRIPTION
    Retrieve a message from the queue of QMI CAT

  PARAMETERS
    None

  RETURN VALUE
    qmi_cat_message_type : Message extracted from the queue

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cat_message_type * qmi_cat_get_message
(
  void
)
{
  qmi_cat_message_type * cat_message_ptr = NULL;

  ASSERT(qmi_cati_global_ptr);

  if (q_cnt(&qmi_cati_global_ptr->qmi_cat_q) > 0)
  {
    cat_message_ptr = (qmi_cat_message_type*)q_get(&qmi_cati_global_ptr->qmi_cat_q);
  }

  return cat_message_ptr;
} /* qmi_cat_get_message */


/*===========================================================================
  FUNCTION QMI_CATI_ALLOC_CLID

  DESCRIPTION
    Allocate client command from the framework.  Post a message
    to handle the allocation in GSTK context.

  PARAMETERS
    svc_common_hdr : framework service header

  RETURN VALUE
    True : Message posted successfully
    False: Allocation error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_alloc_clid
(
  qmi_framework_common_msg_hdr_type * svc_common_hdr
)
{
  qmi_cat_message_type * msg_ptr = NULL;

  ASSERT(svc_common_hdr);

  UIM_MSG_HIGH_1("Incoming Client allocation for instance:0x%x",
                 svc_common_hdr->qmi_instance);

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_ALLOC_CLID);

  if (msg_ptr != NULL)
  {
    msg_ptr->data.clid_cb.cmd_hdr = *svc_common_hdr;

    /* Send command to be processed in GSTK context */
    qmi_cat_post_message(msg_ptr);
  }
  else
  {
    UIM_MSG_ERR_0("Could not allocate QMI CAT CB MESSAGE");
    return FALSE;
  }
  return TRUE;
} /* qmi_cati_alloc_clid */


/*===========================================================================
  FUNCTION QMI_CATI_DEALLOC_CLID

  DESCRIPTION
    Deallocate client command from the framework.  Post a message
    to handle the operation in GSTK context.

  PARAMETERS
    svc_common_hdr : framework service header

  RETURN VALUE
    True : Message posted successfully
    False: Allocation error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_dealloc_clid
(
  qmi_framework_common_msg_hdr_type * svc_common_hdr
)
{
  qmi_cat_message_type * msg_ptr = NULL;

  ASSERT(svc_common_hdr);

  UIM_MSG_HIGH_1("Incoming Client deallocation for instance:0x%x",
                 svc_common_hdr->qmi_instance);

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_DEALLOC_CLID);

  if (msg_ptr != NULL)
  {
    msg_ptr->data.clid_cb.cmd_hdr = *svc_common_hdr;

    /* Send command to be processed in GSTK context */
    qmi_cat_post_message(msg_ptr);
  }
  else
  {
    UIM_MSG_ERR_0("Could not allocate QMI CAT CB MESSAGE");
  }
} /* qmi_cati_dealloc_clid */


/*===========================================================================
  FUNCTION QMI_CATI_INIT_CBACK

  DESCRIPTION
    Callback for the qmi framework registration.

  PARAMETERS
    num_instances : The number of qmi_cat instances

  RETURN VALUE
    None

  DEPENDENCIES
    Must have registered with the qmi framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_init_cback
(
  uint16 num_instances
)
{
  /* Nothing to do here. Callback required by qmi_framework_reg_service() */
  UIM_MSG_HIGH_1("qmi_cati_init_cback instances:0x%x", num_instances);
} /* qmi_cati_init_cback */


/*===========================================================================
  FUNCTION QMI_CATI_CMD_HDLR

  DESCRIPTION
    Function to route external commands sent to QMI_CAT service
    to the appropriate internal service handling routine.

  PARAMETERS
    msg_hdr  : CMD data including hdr packaged by qmi framework
    sdu      : Incoming request
===========================================================================*/
static void qmi_cati_cmd_hdlr
(
  qmi_framework_msg_hdr_type * msg_hdr_ptr,
  dsm_item_type ** sdu
)
{
  qmi_cat_message_type       * qmi_cat_message_ptr = NULL;

  ASSERT(msg_hdr_ptr);
  ASSERT(sdu);

  qmi_cat_message_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_FRAMEWORK_CMD);

  if(qmi_cat_message_ptr == NULL)
  {
    dsm_free_packet(sdu);
  }
  else
  {
    /* Save incoming framework message header and command */
    qmi_cat_message_ptr->data.framework_cmd.cmd_hdr = *msg_hdr_ptr;
    qmi_cat_message_ptr->data.framework_cmd.sdu     = *sdu;

    qmi_cat_post_message(qmi_cat_message_ptr);
  }
} /* qmi_cati_cmd_hdlr */


/*===========================================================================
  FUNCTION QMI_CATI_MAP_GSTK_STATUS_TO_ERRVAL

  DESCRIPTION
    Map gstk status to QMI-Error value

  PARAMETERS
    gstk_status_enum_type: gstk_status

  RETURN VALUE
    qmi_error_e_type: QMI Error type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_map_gstk_status_to_errval
(
  gstk_status_enum_type  gstk_status
)
{
  qmi_error_e_type  errval = QMI_ERR_NONE;

  UIM_MSG_HIGH_1("Received gstk_status %d", gstk_status);

  switch(gstk_status)
  {
    case GSTK_SUCCESS:
      errval = QMI_ERR_NONE;
      break;

    case GSTK_EXTRA_PARAM_NOT_REQUIRED:
    case GSTK_EXTRA_PARAM_MISSING:
    case GSTK_INVALID_LENGTH:
    case GSTK_BAD_PARAM:
      errval = QMI_ERR_INVALID_ARG;
      break;

    case GSTK_MEMORY_ERROR:
    case GSTK_CLIENT_SPACE_FULL:
      errval = QMI_ERR_NO_MEMORY;
      break;

    case GSTK_UNSUPPORTED_COMMAND:
      errval = QMI_ERR_INVALID_OPERATION;
      break;

    default:
      UIM_MSG_MED_0("Unhandled gstk_status received");
      errval = QMI_ERR_INTERNAL;
  }

  return errval;
} /* qmi_cati_map_gstk_status_to_errval */


/*===========================================================================
  FUNCTION QMI_CATI_CONVERT_QMI_CNF_TO_GSTK_CNF()

  DESCRIPTION
    to get the GSTK cnf from the qmi terminal response cnf

  PARAMETERS
    qmi_tr_cnf : QMI TR CNF
    gstk_tr_cnf : Gstk TR_CNF

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_convert_qmi_cnf_to_gstk_cnf
(
  uint8                qmi_tr_cnf,
  gstk_cmd_enum_type * gstk_tr_cnf
)
{
  ASSERT(gstk_tr_cnf != NULL);

  switch (qmi_tr_cnf)
  {
    case CATI_EVENT_DISPLAY_TEXT_CNF:
      *gstk_tr_cnf = GSTK_DISPLAY_TEXT_CNF;
      break;

    case CATI_EVENT_GET_INKEY_CNF:
      *gstk_tr_cnf = GSTK_GET_INKEY_CNF;
      break;

    case CATI_EVENT_GET_INPUT_CNF:
      *gstk_tr_cnf = GSTK_GET_INPUT_CNF;
      break;

    case CATI_EVENT_LAUNCH_BROWSER_CNF:
      *gstk_tr_cnf = GSTK_LAUNCH_BROWSER_CNF;
      break;

    case CATI_EVENT_PLAY_TONE_CNF:
      *gstk_tr_cnf = GSTK_PLAY_TONE_CNF;
      break;

    case CATI_EVENT_SELECT_ITEM_CNF:
      *gstk_tr_cnf = GSTK_SELECT_ITEM_CNF;
      break;

    case CATI_EVENT_SETUP_MENU_CNF:
      *gstk_tr_cnf = GSTK_SETUP_MENU_CNF;
      break;

    case CATI_EVENT_SETUP_IDLE_TEXT_CNF:
      *gstk_tr_cnf = GSTK_SETUP_IDLE_TEXT_CNF;
      break;

    case CATI_EVENT_PROVIDE_LANGUAGE_INFO_CNF:
      *gstk_tr_cnf= GSTK_PROVIDE_LANG_INFO_CNF;
      break;

    case CATI_EVENT_LANGUAGE_NOTIFICATION_CNF:
      *gstk_tr_cnf = GSTK_LANG_NOTIFICATION_CNF;
      break;

    case CATI_EVENT_USER_ACTIVITY_CNF:
    case CATI_EVENT_IDLE_SCREEN_CNF:
    case CATI_EVENT_LANGUAGE_SEL_CNF:
    case CATI_EVENT_HCI_CONNECTIVITY_CNF:
    case CATI_EVENT_BROWSER_TERM_CNF:
      *gstk_tr_cnf = GSTK_SETUP_EVENT_LIST_CNF;
      break;

    case CATI_EVENT_ACTIVATE_CNF:
      *gstk_tr_cnf = GSTK_ACTIVATE_CNF;
    break;

    case CATI_EVENT_SEND_SMS_CNF:
      *gstk_tr_cnf = GSTK_SEND_SMS_CNF;
      break;

    case CATI_EVENT_SETUP_CALL_CNF:
      *gstk_tr_cnf = GSTK_SETUP_CALL_CNF;
      break;

    case CATI_EVENT_SEND_DTMF_CNF:
      *gstk_tr_cnf = GSTK_SEND_DTMF_CNF;
      break;

    case CATI_EVENT_SEND_SS_CNF:
      *gstk_tr_cnf = GSTK_SEND_SS_CNF;
      break;

    case CATI_EVENT_SEND_USSD_CNF:
      *gstk_tr_cnf = GSTK_SEND_USSD_CNF;
      break;

    default:
      return QMI_ERR_INVALID_ARG;
  }
  return QMI_ERR_NONE;
} /* qmi_cati_convert_qmi_cnf_to_gstk_cnf */


/*===========================================================================
  FUNCTION QMI_CATI_CONVERT_GSTK_SLOT_ID_TO_QMI_SLOT_INDEX

  DESCRIPTION
    to get qmi slot index from gstk slot id

  PARAMETERS
    gstk_slot_id
    qmi_slot_index

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_convert_gstk_slot_id_to_qmi_slot_index
(
  gstk_slot_id_enum_type    gstk_slot_id,
  uint8                   * slot_index_ptr
)
{
  ASSERT (slot_index_ptr != NULL);
  ASSERT(qmi_cati_global_ptr);

  /* Retrieve the slot index */
  if((gstk_slot_id == GSTK_SLOT_1) &&
     (qmi_cati_global_ptr->slot_count >= 1))
  {
    *slot_index_ptr = 0;
  }
  else if((gstk_slot_id == GSTK_SLOT_2) &&
          (qmi_cati_global_ptr->slot_count >= 2))
  {
    *slot_index_ptr = 1;
  }
  else if((gstk_slot_id == GSTK_SLOT_3) &&
          (qmi_cati_global_ptr->slot_count >= 3))
  {
    *slot_index_ptr = 2;
  }
  else
  {
    UIM_MSG_ERR_1("Device doesn't support operation on slot 0x%x",
                  gstk_slot_id);
    return QMI_ERR_OP_DEVICE_UNSUPPORTED;
  }
  return QMI_ERR_NONE;
} /* qmi_cati_convert_gstk_slot_id_to_qmi_slot_index*/


/*===========================================================================
  FUNCTION QMI_CATI_CONVERT_QMI_CHANNEL_STATE_TO_GSTK_CHANNEL_STATE

  DESCRIPTION
    to get gstk channel state from qmi channel state

  PARAMETERS
    qmi_ch_state
    gstk_ch_state_ptr

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_convert_qmi_channel_state_to_gstk_channel_state
(
  uint8                                 qmi_ch_state,
  gstk_scws_ch_state_enum_type        * gstk_ch_state_ptr
)
{
  ASSERT (gstk_ch_state_ptr != NULL);

  /* Retrieve channel state */
  switch(qmi_ch_state)
  {
    case 0x00:
      *gstk_ch_state_ptr = GSTK_SCWS_TCP_IN_CLOSED_STATE;
      break;
    case 0x01:
      *gstk_ch_state_ptr = GSTK_SCWS_TCP_IN_LISTEN_STATE;
      break;
    case 0x02:
      *gstk_ch_state_ptr = GSTK_SCWS_TCP_IN_ESTABLISHED_STATE;
      break;
    default:
      UIM_MSG_ERR_1("Invalid qmi channel state : 0x%x", qmi_ch_state);
      return QMI_ERR_INVALID_ARG;
  }
  return QMI_ERR_NONE;
} /* qmi_cati_convert_qmi_channel_state_to_gstk_channel_state*/

/*===========================================================================
  FUNCTION QMI_CATI_CONVERT_QMI_RAT_TO_GSTK_RAT

  DESCRIPTION
    to get gstk rat from qmi rat

  PARAMETERS
    qmi_rat
    gstk_rat_ptr

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_convert_qmi_rat_to_gstk_rat
(
  uint32                               qmi_rat,
  gstk_access_technology_type        * gstk_rat_ptr
)
{
  ASSERT (gstk_rat_ptr != NULL);

  /* Retrieve channel state */
  switch(qmi_rat)
  {
    case QMI_CATI_ACCESS_TECH_GSM:
      *gstk_rat_ptr = GSTK_ACCESS_TECH_GSM;
      break;
    case QMI_CATI_ACCESS_TECH_UTRAN:
      *gstk_rat_ptr = GSTK_ACCESS_TECH_UTRAN;
      break;
    case QMI_CATI_ACCESS_TECH_CDMA:
      *gstk_rat_ptr = GSTK_ACCESS_TECH_CDMA;
      break;
    case QMI_CATI_ACCESS_TECH_LTE:
      *gstk_rat_ptr = GSTK_ACCESS_TECH_LTE;
      break;
    case QMI_CATI_ACCESS_TECH_WLAN:
      *gstk_rat_ptr = GSTK_ACCESS_TECH_WLAN;
      break;
    case QMI_CATI_ACCESS_NONE:
      *gstk_rat_ptr = GSTK_ACCESS_NONE;
      break;
    default:
      UIM_MSG_ERR_1("Invalid qmi rat : 0x%x", qmi_rat);
      return QMI_ERR_INVALID_ARG;
  }
  return QMI_ERR_NONE;
} /* qmi_cati_convert_qmi_rat_to_gstk_rat*/


/*===========================================================================
  FUNCTION QMI_CATI_CHECK_LAST_GSTK_CMD

  DESCRIPTION
    check last GSTK command with TR / Event confirmation received from client

  PARAMETERS
    slot_index: slot index
    is_tr: TRUE if the request is for Terminal response
           FALSE if the request is for event confirmation.

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_check_last_gstk_cmd
(
  uint8    slot_index,
  boolean  is_tr
)
{
  ASSERT(qmi_cati_global_ptr);

  /* if last cmd is end session or invalid command reference id,
     then GSTK is not expecting a TR or Evt Conf */
  if (qmi_cati_global_ptr->last_gstk_cmd[slot_index] == GSTK_END_PROACTIVE_CMD_REQ ||
      qmi_cati_global_ptr->ref_command_id[slot_index] == 0)
  {
    UIM_MSG_ERR_0("Invalid TR/Event Conf");
    return QMI_ERR_INVALID_OPERATION;
  }

  /* TR is expected for these commands */
  switch(qmi_cati_global_ptr->last_gstk_cmd[slot_index])
  {
    /* TR is expected for these commands */
    case GSTK_DISPLAY_TEXT_REQ:
    case GSTK_GET_INKEY_REQ:
    case GSTK_GET_INPUT_REQ:
    case GSTK_SETUP_MENU_REQ:
    case GSTK_SELECT_ITEM_REQ:
    case GSTK_SETUP_IDLE_TEXT_REQ:
    case GSTK_LANG_NOTIFICATION_REQ:
    case GSTK_PLAY_TONE_REQ:
    case GSTK_LAUNCH_BROWSER_REQ:
    case GSTK_PROVIDE_LOCAL_INFO_REQ:
    case GSTK_PROVIDE_LANG_INFO_REQ:
    case GSTK_ACTIVATE_REQ:
      if(is_tr)
      {
        return QMI_ERR_NONE;
      }
      break;
    case GSTK_SETUP_EVENT_LIST_REQ:
      /* For GSTK_FORMAT, for the same SETUP_EVENT_LIST proactive command, GSTK
         sends each setup event back to back, i.e., more than one TRs could be
         pending for the same proactive command, reject TR only if all the pending
         TRs for the same SETUP_EVENT_LIST proactive commmand are received.
       */
      if(is_tr &&
         ((qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_DECODED &&
           qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_CUSTOM_DECODED) ||
          qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index] > 0))
      {
        return QMI_ERR_NONE;
      }
      break;
    /* Event conf is expected for these commands */
    case GSTK_OPEN_CH_REQ:
    case GSTK_CLOSE_CH_REQ:
    case GSTK_RECEIVE_DATA_REQ:
    case GSTK_SEND_DATA_REQ:
    case GSTK_REFRESH_REQ:
    case GSTK_SCWS_OPEN_CH_REQ:
    case GSTK_SCWS_CLOSE_CH_REQ:
    case GSTK_SCWS_SEND_DATA_REQ:
    case GSTK_SCWS_DATA_AVAIL_RSP_REQ:
      if(!is_tr)
      {
        return QMI_ERR_NONE;
      }
      break;
    /* Event conf/TR is expected for these commands */
    case GSTK_SETUP_CALL_REQ:
    case GSTK_SEND_SMS_REQ:
    case GSTK_SEND_USSD_REQ:
    case GSTK_SEND_DTMF_REQ:
    case GSTK_SEND_SS_REQ:
      /* TR is expected only if the clients registers for full functionality */
      if(is_tr == qmi_cati_global_ptr->full_func_evt[slot_index])
      {
        return QMI_ERR_NONE;
      }
      break;
    default:
      break;
  }

  return QMI_ERR_INVALID_OPERATION;
} /* qmi_cati_check_last_gstk_cmd*/


/*===========================================================================
  FUNCTION QMI_CATI_READ_CONFIGURATION()

  DESCRIPTION
    Read the configuration mode from the EFS

  PARAMETERS
    None

  RETURN VALUE
    Configuration mode

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cati_config_mode_e_type qmi_cati_read_configuration
(
  void
)
{
  uim_common_efs_status_enum_type status     = UIM_COMMON_EFS_ERROR;
  uint8                           value      = 0;
  uint32                          size       = 0;

  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_MODE,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             &value,
             sizeof(uint8));

  UIM_MSG_HIGH_2("QMI CAT configuration: status: 0x%x value: 0x%x",
                 status, value);

  if (status == UIM_COMMON_EFS_SUCCESS)
  {
    switch(value)
    {
      case 0:
      case 1:
        /* GOBI mode is no longer supported by QMI_CAT */ 
        return CATI_CONFIG_MODE_DISABLED;        
      case 2:
        return CATI_CONFIG_MODE_ANDROID;
      case 3:
        return CATI_CONFIG_MODE_DECODED;
      case 4:
        return CATI_CONFIG_MODE_DECODED_PULLONLY;
      case 5:
        return CATI_CONFIG_MODE_CUSTOM_RAW;
      case 6:
        return CATI_CONFIG_MODE_CUSTOM_DECODED;
      default:
        break;
    }
  }
  else
  {
    /* Create directory for cat config file */
    if (uim_common_efs_get_dir_stat(
          UIM_COMMON_EFS_MODEM_CONFIG_DIR1,
          &size) != UIM_COMMON_EFS_SUCCESS)
    {
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_MODEM_CONFIG_DIR1, 0777);
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI_CONFIG_DIR2, 0777);
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_DIR3, 0777);
    }
    else if (uim_common_efs_get_dir_stat(
          UIM_COMMON_EFS_UIMQMI_QMI_CONFIG_DIR2,
          &size) != UIM_COMMON_EFS_SUCCESS)
    {
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI_CONFIG_DIR2, 0777);
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_DIR3, 0777);
    }
    else if (uim_common_efs_get_dir_stat(
          UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_DIR3,
          &size) != UIM_COMMON_EFS_SUCCESS)
    {
      (void)uim_common_efs_create_dir(UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_DIR3, 0777);
    }
  }

  /* NV item is not present or has an invalid value:
     return default value.*/
  return CATI_CONFIG_MODE_DECODED;
} /* qmi_cati_read_configuration */


/*===========================================================================
  FUNCTION QMI_CAT_DISABLE_RAW_UNSUPPORTED_FEATURES()

  DESCRIPTION
    Disable the raw unsupported features from TP, To disable features we are  
    setting value to zero in TP structure

  PARAMETERS
    profile_value_list_ptr    : List of TP structure 
    profile_value_list_length : Number of TP structure 

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_disable_raw_unsupported_features
(
  gstk_profile_dl_support_ext_type *profile_value_list_ptr,
  uint8                             profile_value_list_length
)
{
  uim_common_efs_status_enum_type status     = UIM_COMMON_EFS_ERROR;
  uint8                           i          = 0;

  /* Structure holding unsupported raw feature */
  PACKED struct PACKED_POST
  {
    boolean skip_language_notification;
    boolean skip_user_activity_event;
    boolean rfu[18];
  } raw_unsupported_features;

  if(profile_value_list_ptr == NULL || 
     profile_value_list_length == 0)
  {
    return;
  }

  memset(&raw_unsupported_features, 0 , sizeof(raw_unsupported_features));

  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_RAW_UNSUPPORTED_FEATURES,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             (uint8 *)&raw_unsupported_features,
             sizeof(raw_unsupported_features));

  UIM_MSG_LOW_1("QMI CAT RAW UNSUPPORTED FEATURES: status: 0x%x", status);

  for(i = 0; i < profile_value_list_length; i++)
  {
    switch(profile_value_list_ptr[i].profile_dl_bitmask)
    {
      case GSTK_LANGUAGE_NOTIFICATION_SUPPORT:
        if(raw_unsupported_features.skip_language_notification)
        {
          profile_value_list_ptr[i].value = 0;
        }
        break;
      case GSTK_USER_ACT_EVT_SUPPORT:
        if(raw_unsupported_features.skip_user_activity_event)
        {
          profile_value_list_ptr[i].value = 0;
        }
        break;
      default:
        break;
    }
  }
} /* qmi_cat_disable_raw_unsupported_features */

  
/*===========================================================================
  FUNCTION QMI_CATI_READ_DISPLAY_ALPHA_CONF_CONF()

  DESCRIPTION
    Read the configuration mode from the EFS

  PARAMETERS
    None

  RETURN VALUE
    Configuration mode

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cati_display_alpha_conf_e_type qmi_cati_read_display_alpha_conf
(
  void
)
{
  uim_common_efs_status_enum_type status     = UIM_COMMON_EFS_ERROR;
  uint8                           value      = 0;

  ASSERT(qmi_cati_global_ptr);

  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_DISPLAY_ALPHA_FILE,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             &value,
             sizeof(uint8));

  UIM_MSG_HIGH_2("QMI CAT display alpha file status: 0x%x value: 0x%x",
                 status, value);

  if (status == UIM_COMMON_EFS_SUCCESS)
  {
    switch(value)
    {
      case 0:
        return CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF;
      case 1:
        return CATI_DISPLAY_ALPHA_AUTOMATIC_FALSE;
      case 2:
        return CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE;
      default:
        break;
    }
  }

  /* NV item is not present or has an invalid value:
     return default value.*/
  switch(qmi_cati_global_ptr->config_mode)
  {
    /* For Custom raw config_mode (WP): WP doesn't support display alpha,
       thus set default value to FALSE.*/
    case CATI_CONFIG_MODE_CUSTOM_RAW:
      return CATI_DISPLAY_ALPHA_AUTOMATIC_FALSE;
    /* For Android config_mode: QMI sends conformation to GSTK for
       Setup call display request without sending indication to UI
       Limitation in Android: UI has no knowledge if icon is being
       displayed, assume icon display successfully */
    case CATI_CONFIG_MODE_ANDROID:
      return CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE;
    default:
      break;
  }
  return CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF;
} /* qmi_cati_read_display_alpha_conf */


/*===========================================================================
  FUNCTION QMI_CATI_READ_SMS_PP_ENVELOPE_CONF()

  DESCRIPTION
    Read the SMS PP DL blocking flag from the EFS

  PARAMETERS
    None

  RETURN VALUE
    Configuration mode

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_read_sms_pp_envelope_conf
(
  void
)
{
  uim_common_efs_status_enum_type status     = UIM_COMMON_EFS_ERROR;
  boolean                         value      = 0;
  
  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_BLOCK_SMS_PP_ENV,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             &value,
             sizeof(uint8));

  if (status == UIM_COMMON_EFS_SUCCESS && value == 0)
  {
    return FALSE;
  }

  /* NV item is not present or has an invalid value:
     return default value as blocked */
  UIM_MSG_HIGH_0("SMS PP DL envelope is blocked due to security");
  return TRUE;
} /* qmi_cati_read_sms_pp_envelope_conf */

/*===========================================================================
  FUNCTION QMI_CATI_GET_CUSTOM_TP()

  DESCRIPTION
    Reads custom TP from NV and allocate memory to save the parsed TP
    
  PARAMETERS
    profile_value_list_pptr : Input address of TP value list pointer
    tp_count_ptr            : Output TP count pointer

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_get_custom_tp
(
  gstk_profile_dl_support_ext_type  **profile_value_list_pptr,
  uint8                              *tp_count_ptr
)
{
  int                                file_len                          = 0;
  uint16                             cmd_bitmask                       = 0;
  uint16                             tp_byte                           = 0;
  uint8                              tp_count                          = 0;
  uint8                              index                             = 0;
  uint8                              i                                 = 0;
  uint8                              mask                              = 1;
  uint8                              custom_tp[CATI_TLV_CUSTOM_TP_MAX] = {0};
  gstk_profile_dl_support_ext_type  *custom_profile_ptr                = NULL;
  uim_common_efs_status_enum_type status            = UIM_COMMON_EFS_ERROR;

  ASSERT(profile_value_list_pptr && NULL == *profile_value_list_pptr);
  ASSERT(tp_count_ptr);

  file_len = sizeof(uint8) * CATI_TLV_CUSTOM_TP_MAX;

  /* Retrieve NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_TP,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             custom_tp,
             file_len);

  UIM_MSG_HIGH_1("qmi_cati_get_custom_tp file_len: 0x%x", file_len);

  if(status != UIM_COMMON_EFS_SUCCESS)
  {
    return QMI_ERR_INTERNAL;
  }

  for(i = 0; i < file_len; i++ )
  {
    UIM_MSG_MED_2("QMI CAT custom TP: custom_tp[0x%x]:0x%x ", i, custom_tp[i]);
  }

  /* Find out tp_count */
  for(index = 0; index < file_len; index++)
  {
    tp_byte = (index + 1) << 8;
    cmd_bitmask = custom_tp[index];
    mask = 1;
    for(i = 0; i < 8 && tp_count < QMI_CAT_MODE_CUSTOM_TP_DL_MAX_VALUE; i++)
    {
      if(cmd_bitmask & mask)
      {
        /* add new entry for profile value with same dl bit mask*/
        if((GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8   == (uint32)(tp_byte | mask)) ||
           (GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1 == (uint32)(tp_byte | mask)))
        {
          tp_count++;
        }
        tp_count++;
      }
      mask = mask << 1;
    }
  }

  if (0 == tp_count)
  {
    *tp_count_ptr = 0;
    return QMI_ERR_NONE;
  }

  /* Allocate memory based on tp_count */
  *profile_value_list_pptr = (gstk_profile_dl_support_ext_type *) uimqmi_malloc(
                                tp_count * sizeof(gstk_profile_dl_support_ext_type));
  if (NULL == *profile_value_list_pptr)
  {
    UIM_MSG_ERR_0("Unable to allocate memory for custom TP");
    return QMI_ERR_NO_MEMORY;
  }

  /* Parse and set values for each download profile */
  *tp_count_ptr      = tp_count;
  tp_count           = 0;
  custom_profile_ptr = *profile_value_list_pptr;
  for(index = 0; index < file_len; index++)
  {
    tp_byte = (index + 1) << 8;
    cmd_bitmask = custom_tp[index];
    mask = 1;
    for(i = 0; i < 8 && tp_count < *tp_count_ptr; i++)
    {
      if(cmd_bitmask & mask)
      {
        custom_profile_ptr->profile_dl_bitmask = (uint32)(tp_byte | mask);

        /* Soft Key is controlled by 2 keys value, thus set TP bitmask value to 2*/
        if(custom_profile_ptr->profile_dl_bitmask == GSTK_MAX_SK_SUPPORT)
        {
          custom_profile_ptr->value = 2;
        }
        else
        {
          custom_profile_ptr->value = 1;
        }

        if((GSTK_USSD_DO_IN_CC_SUPPORT == custom_profile_ptr->profile_dl_bitmask) ||
           (GSTK_DISPLAY_OF_EXTN_TEXT  == custom_profile_ptr->profile_dl_bitmask) ||
           (GSTK_SUSTAINED_DISP_TEXT_SUPPORT == custom_profile_ptr->profile_dl_bitmask))
        {
          custom_profile_ptr->card_app_type = (uint8)GSTK_APP_SIM;
        }
        else
        {
          custom_profile_ptr->card_app_type = (uint8)GSTK_APP_NONE;
        }

        /* add new entry for profile value with same dl bit mask*/
        if((GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8   == custom_profile_ptr->profile_dl_bitmask) ||
           (GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1 == custom_profile_ptr->profile_dl_bitmask))
        {
          UIM_MSG_HIGH_3("Profile dl bitmask added: 0x%x, tp_byte: 0x%x, mask 0x%x",
                         custom_profile_ptr->profile_dl_bitmask,
                         tp_byte,
                         mask);
          tp_count++;
          custom_profile_ptr++;
          custom_profile_ptr->value              = 1;
          custom_profile_ptr->profile_dl_bitmask = (uint32)(tp_byte | mask);
          custom_profile_ptr->card_app_type      = (uint8)GSTK_APP_USIM;
        }

        UIM_MSG_HIGH_3("Profile dl bitmask added: 0x%x, tp_byte: 0x%x, mask 0x%x",
                       custom_profile_ptr->profile_dl_bitmask,
                       tp_byte,
                       mask);

        tp_count++;
        custom_profile_ptr++;
      }
      mask = mask << 1;
    }
  }

  return QMI_ERR_NONE;
} /* qmi_cati_get_custom_tp */


/*===========================================================================
  FUNCTION QMI_CATI_GET_TP_FROM_NV()

  DESCRIPTION
    sgt the terminal profile list from NV

  PARAMETERS
    None

  RETURN VALUE
    TP length

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 qmi_cati_get_tp_from_nv
(
  uint8   * custom_tp_ptr,
  uint8     custom_tp_len
)
{
  UIM_MSG_HIGH_0("qmi_cati_get_tp_from_nv");

  ASSERT (custom_tp_ptr != NULL);
  ASSERT (custom_tp_len > 0);

  memset( custom_tp_ptr, 0, custom_tp_len);

  /* Retrieve custom TP NV value from EFS */
  if(uim_common_efs_read(
       UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_TP,
       UIM_COMMON_EFS_ITEM_FILE_TYPE,
       UIM_COMMON_EFS_DEVICE,
       custom_tp_ptr,
       custom_tp_len) != UIM_COMMON_EFS_SUCCESS)
  {
    return 0;
  }

  UIM_MSG_HIGH_1("qmi_cati_get_tp_from_nv file_len: 0x%x", custom_tp_len);

  return custom_tp_len;
} /* qmi_cati_get_tp_from_nv */


/*===========================================================================
  FUNCTION QMI_CATI_STORE_TP_TO_NV()

  DESCRIPTION
    store the custom tp bitmaskt to NV

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_store_tp_to_nv
(
  uint8   * custom_tp_bitmask_ptr,
  uint8     custom_tp_bitmask_len
)
{
  ASSERT (custom_tp_bitmask_ptr != NULL);

  /* Store NV value to EFS */
  if (uim_common_efs_write(
        UIM_COMMON_EFS_UIMQMI_QMI_CAT_CONFIG_TP,
        UIM_COMMON_EFS_ITEM_FILE_TYPE,
        UIM_COMMON_EFS_DEVICE,
        (const char *)custom_tp_bitmask_ptr,
        (uint32)custom_tp_bitmask_len)
      != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to write custom TP file");
    return QMI_ERR_INTERNAL;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_store_tp_to_nv */


/*===========================================================================
  FUNCTION QMI_CATI_STORE_CONFIG_MODE_TO_NV()

  DESCRIPTION
    store the qmi config mode to NV

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_store_config_mode_to_nv
(
  uint8 config_mode
)
{
  /* expected directory and file created during bootup*/
  /* Store NV value to EFS */
  if (uim_common_efs_write(
        UIM_COMMON_EFS_UIMQMI_QMI_CAT_MODE,
        UIM_COMMON_EFS_ITEM_FILE_TYPE,
        UIM_COMMON_EFS_DEVICE,
        (const char *)&config_mode,
        sizeof(config_mode))
      != UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_0("failed to write config mode file");
    return QMI_ERR_INTERNAL;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_store_config_mode_to_nv */


/*===========================================================================
  FUNCTION QMI_CATI_READ_LANG_FROM_NV()

  DESCRIPTION
    Read the default language codes from NV & updates the passed pointer
    appropriately. If we fail to read NV, pointer is not updated

  PARAMETERS
    None

  RETURN VALUE
    TRUE if read successfully, else FALSE.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_read_lang_from_nv
(
  gstk_general_result_enum_type        * general_result_ptr,
  gstk_terminal_rsp_extra_param_type   * extra_info_ptr
)
{
  uint8                           lang_code[GSTK_LANG_CODE_LEN] = {0};
  uim_common_efs_status_enum_type status                        = UIM_COMMON_EFS_ERROR;

  ASSERT (extra_info_ptr != NULL);
  ASSERT (general_result_ptr != NULL);

  /* Retrieve language code NV value from EFS */
  status = uim_common_efs_read(
             UIM_COMMON_EFS_UIMQMI_QMI_CAT_DEFAULT_LANG,
             UIM_COMMON_EFS_ITEM_FILE_TYPE,
             UIM_COMMON_EFS_DEVICE,
             lang_code,
             GSTK_LANG_CODE_LEN);

  UIM_MSG_HIGH_1("qmi_cati_read_lang_from_nv status: 0x%x", status);

  if(status != UIM_COMMON_EFS_SUCCESS)
  {
    return FALSE;
  }

  /* Update the Language parameters */
  *general_result_ptr      = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  extra_info_ptr->present  = TRUE;
  extra_info_ptr->rsp_type = GSTK_PROVIDE_LANG_INFO_CNF;
  (void)memscpy(extra_info_ptr->extra_param.provide_lang_info_extra_param.lang_code,
                sizeof(extra_info_ptr->extra_param.provide_lang_info_extra_param.lang_code),
                lang_code,
                GSTK_LANG_CODE_LEN);
  return TRUE;
} /* qmi_cati_read_lang_from_nv */


/*===========================================================================
  FUNCTION  CATI_CONV_RAW_MASK_TO_TLV_ID()

  DESCRIPTION
    Converts the raw mask to prm type.

  PARAMETERS
    event_mask : Event info

  RETURN VALUE
    Converted Prm type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static uint8 cati_conv_raw_mask_to_tlv_id
(
  uint32                        ev_mask,
  qmi_cati_data_event_e_type    data_evt
)
{
  uint8 ret_val = 0;

  switch (ev_mask)
  {
    case CATI_REPORT_STATUS_EV_DISPLAY_TEXT:
      ret_val = CATI_PRM_TYPE_DISPLAY_TEXT;
      break;

    case CATI_REPORT_STATUS_EV_GET_INKEY:
      ret_val = CATI_PRM_TYPE_GET_INKEY;
      break;

    case CATI_REPORT_STATUS_EV_GET_INPUT:
      ret_val = CATI_PRM_TYPE_GET_INPUT;
      break;

    case CATI_REPORT_STATUS_EV_SETUP_MENU:
      ret_val = CATI_PRM_TYPE_SETUP_MENU;
      break;

    case CATI_REPORT_STATUS_EV_SELECT_ITEM:
      ret_val = CATI_PRM_TYPE_SELECT_ITEM;
      break;

    case CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT:
      ret_val = CATI_PRM_TYPE_SETUP_IDLE_MODE_TEXT;
      break;

    case CATI_REPORT_STATUS_EV_LANG_NOTIFICATION:
      ret_val = CATI_PRM_TYPE_LANG_NOTIFICATION;
      break;

    case CATI_REPORT_STATUS_EV_PLAY_TONE:
      ret_val = CATI_PRM_TYPE_PLAY_TONE_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SETUP_CALL:
      ret_val = CATI_PRM_TYPE_SETUP_CALL_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SEND_DTMF:
      ret_val = CATI_PRM_TYPE_SEND_DTMF_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_LAUNCH_BROWSER:
      ret_val = CATI_PRM_TYPE_LAUNCH_BROWSER_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID:
      ret_val = CATI_PRM_TYPE_SEND_SMS_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SEND_SS:
      ret_val = CATI_PRM_TYPE_SEND_SS_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_SEND_USSD:
      ret_val = CATI_PRM_TYPE_SEND_USSD_EVENT;
      break;

    case CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_LANG:
    case CATI_REPORT_STATUS_EV_PROVIDE_LOCAL_INFO_TIME:
    case CATI_PROVIDE_LOCAL_INFO_MASK:
      ret_val = CATI_PRM_TYPE_PROVIDE_LOCAL_INFO;
      break;

    case CATI_SETUP_EVT_LIST_MASK:
    case CATI_REPORT_STATUS_EV_SETUPEVT_USER_ACT:
    case CATI_REPORT_STATUS_EV_SETUPEVT_IDL_SCR_NOTIFY:
    case CATI_REPORT_STATUS_EV_SETUPEVT_LANGSEL_NOTIFY:
    case CATI_REPORT_STATUS_EV_SETUPEVT_BROWSER_TERM:
    case CATI_REPORT_STATUS_EV_SETUPEVT_HCI_CONN:
      ret_val = CATI_PRM_TYPE_SETUP_EVENT_LIST_RAW;
      break;

    /* Data Event ev_mask has more than 1 possible prm type conversion.*/
    case CATI_REPORT_STATUS_EV_DATA_EVT:
      switch(data_evt)
      {
        case CATI_DATA_EVENT_OPEN_CHANNEL:
          ret_val = CATI_PRM_TYPE_OPEN_CHANNEL;
          break;

        case CATI_DATA_EVENT_CLOSE_CHANNEL:
          ret_val = CATI_PRM_TYPE_CLOSE_CHANNEL;
          break;

        case CATI_DATA_EVENT_SEND_DATA:
          ret_val = CATI_PRM_TYPE_SEND_DATA;
          break;

        case CATI_DATA_EVENT_RECEIVE_DATA:
          ret_val = CATI_PRM_TYPE_RECEIVE_DATA;
          break;

        default:
          UIM_MSG_ERR_1("Unhandled Raw DATA_EVT mask 0x%x", data_evt);
          break;
      }
      break;

    case CATI_REPORT_STATUS_EV_ACTIVATE:
      ret_val = CATI_PRM_TYPE_ACTIVATE;
      break;

    case CATI_REPORT_STATUS_EV_REFRESH:
      ret_val = CATI_PRM_TYPE_REFRESH_ALPHA;
      break;

    default:
      UIM_MSG_ERR_1("Unhandled Raw ev_mask to prm type conversion 0x%x",
                    ev_mask);
      break;
  }
  return ret_val;
} /* cati_conv_raw_mask_to_tlv_id */


/*===========================================================================
  FUNCTION QMI_CAT_GET_CB_USERDATA

  DESCRIPTION
    To obtain the userdata used in GSTK callbacks

  PARAMETERS
    request_id             : Type of the request
    cmd_buf_ptr            : QMI command buffer required for response
    ind_token              : indication token

  RETURN VALUE
    qmi_cati_userdata_type : Poiner to the structure used for userdata

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_cati_userdata_type* qmi_cat_get_cb_userdata
(
  qmi_cati_cmd_val_e_type              request_id,
  qmi_cmd_buf_type *                   cmd_buf_ptr,
  qmi_cati_ind_token_info_type         ind_token
)
{
  qmi_cati_userdata_type  * userdata_ptr = NULL;

  ASSERT(cmd_buf_ptr);
  ASSERT(qmi_cat_state_ptr);

  /* Check command buffer & retrieve clid */
  if (cmd_buf_ptr->x_p == NULL || cmd_buf_ptr->x_p->cl_sp == NULL)
  {
    UIM_MSG_HIGH_0("Transaction ptr or client state pointer is NULL");
    return NULL;
  }

  /* CHeck Client ID validity */
  if((cmd_buf_ptr->x_p->cl_sp->clid == 0) ||
     (cmd_buf_ptr->x_p->cl_sp->clid > CATI_MAX_CLIDS))
  {
    UIM_MSG_HIGH_1("Invalid Client ID: 0x%x", cmd_buf_ptr->x_p->cl_sp->clid);
    return NULL;
  }

  /* Allocate userdata dynamically */
  userdata_ptr = uimqmi_malloc(sizeof(qmi_cati_userdata_type));
  if (userdata_ptr == NULL)
  {
    UIM_MSG_ERR_0("Dynamic allocation of userdata buffer failed! ");
    return NULL;
  }

  /* Update parameters */
  memset(userdata_ptr, 0, sizeof(qmi_cati_userdata_type));
  userdata_ptr->request_id       = request_id;
  userdata_ptr->cmd_buf_ptr      = cmd_buf_ptr;
  userdata_ptr->clid             = cmd_buf_ptr->x_p->cl_sp->clid;
  userdata_ptr->client_reg_count = qmi_cat_state_ptr->client_reg_count[userdata_ptr->clid - 1];

  if (ind_token.is_valid)
  {
    userdata_ptr->ind_token.is_valid = ind_token.is_valid;
    userdata_ptr->ind_token.token = ind_token.token;
  }

  return userdata_ptr;
} /* qmi_cat_get_cb_userdata */


/*===========================================================================
  FUNCTION QMI_CAT_FREE_CB_USERDATA

  DESCRIPTION
    To free the structure used for MMGSDI callbacks

  PARAMETERS
    cb_userdata_ptr : Poiner to the structure used for userdata

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_free_cb_userdata
(
  qmi_cati_userdata_type*  cb_userdata_ptr
)
{
  if (cb_userdata_ptr != NULL)
  {
    /* Free the dynamically allocated pointer earlier in the request */
    memset(cb_userdata_ptr, 0, sizeof(qmi_cati_userdata_type));
    uimqmi_free(cb_userdata_ptr);
    cb_userdata_ptr = NULL;
  }
} /* qmi_cat_free_cb_userdata */


/*===========================================================================
  FUNCTION QMI_CAT_CHECK_CLID()

  DESCRIPTION
    To validate if the passed client id is stil valid within the passed CAT
    service state ptr

  PARAMETERS
    clid             : Client ID
    client_reg_count : Client's count when the command was requested

  RETURN VALUE
    TRUE if successful, else FALSE

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cat_check_clid
(
  uint8                         clid,
  uint32                        client_reg_count
)
{
  int    client_index = 0;

  ASSERT(qmi_cat_state_ptr);

  for(client_index = 0; client_index < (int)CATI_MAX_CLIDS; client_index++)
  {
    if ((qmi_cat_state_ptr->client[client_index] != NULL) &&
        (qmi_cat_state_ptr->client[client_index]->common.clid == clid))
    {
      if(qmi_cat_state_ptr->client_reg_count[client_index] == client_reg_count)
      {
        return TRUE;
      }
      else
      {
        UIM_MSG_HIGH_3("Client reg. count mismatch: 0x%x and 0x%x on clid",
                       client_reg_count,
                       qmi_cat_state_ptr->client_reg_count[client_index],
                       clid);

        return FALSE;
      }
    }
  }

  UIM_MSG_HIGH_1("Invalid clid: 0x%x", clid);

  return FALSE;
} /* qmi_cat_check_clid */


/*===========================================================================
  FUNCTION QMI_CAT_SERVICE_INITIALIZE()

  DESCRIPTION
    Initializes the QMI CAT module. This function is executed only once,
    at the very beginning.

  PARAMETERS
    None

  RETURN VALUE
    Boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cat_service_initialize
(
  void
)
{
  int                     card_index    = 0;
  gstk_status_enum_type   gstk_status   = GSTK_SUCCESS;
  uim_hw_slot_info_type   slot_info;

  /* QMI CAT global variable expected to be allocated in the pre_init phase,
     so it should be present now. */
  if (qmi_cati_global_ptr == NULL)
  {
    return FALSE;
  }

  /* Get count of available slots */
  slot_info = uim_get_hw_slot_info();
  for (card_index = 0 ; card_index < UIM_MAX_NUMBER_INSTANCES; card_index++)
  {
    if((slot_info.slot_status[card_index].slot_available) &&
       (slot_info.slot_status[card_index].slot_enabled))
    {
      qmi_cati_global_ptr->slot_count++;
    }
  }

  /* Read EFS for display alpha confirmation value*/
  qmi_cati_global_ptr->display_alpha_conf = qmi_cati_read_display_alpha_conf();

  /* Read EFS for SMS PP envelope cmd control */
  qmi_cati_global_ptr->sms_pp_envelope_blocked = qmi_cati_read_sms_pp_envelope_conf();

  /* Skip GSTK registration for disabled mode */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DISABLED)
  {
    return TRUE;
  }

  /* Register UI type client of GSTK and get a client id */
  if (qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_DECODED_PULLONLY)
  {
    gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_1, GSTK_UI_TYPE,
                                            qmi_cati_gstk_init_cb,
                                            GSTK_SLOT_1);
    if (GSTK_SUCCESS != gstk_status)
    {
      /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
      (half initialized). So we can simply ignore the error, print an ERR trace and continue */
      UIM_MSG_MED_0("Client init failed for SLOT_1");
    }

    if(qmi_cati_global_ptr->slot_count > 1)
    {
      gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_2, GSTK_UI_TYPE,
                                              qmi_cati_gstk_init_cb,
                                              GSTK_SLOT_2);
      if (GSTK_SUCCESS != gstk_status)
      {
        /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
        (half initialized). So we can simply ignore the error, print an ERR trace and continue */
        UIM_MSG_MED_0("Client init failed for SLOT_2");
      }
    }

    if(qmi_cati_global_ptr->slot_count > 2)
    {
      gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_3, GSTK_UI_TYPE,
                                              qmi_cati_gstk_init_cb,
                                              GSTK_SLOT_3);
      if (GSTK_SUCCESS != gstk_status)
      {
        /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
        (half initialized). So we can simply ignore the error, print an ERR trace and continue */
        UIM_MSG_MED_0("Client init failed for SLOT_3");
      }
    }
  }

  /* Register PULL type client of GSTK and get a client id for Slot 1*/
  gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_1, GSTK_WAKEUP_TYPE,
                                          qmi_cati_gstk_init_cb_external,
                                          GSTK_SLOT_1);
  if (GSTK_SUCCESS != gstk_status)
  {
    /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
    (half initialized). So we can simply ignore the error, print an ERR trace and continue */
    UIM_MSG_MED_0("Client init failed for SLOT_1");
  }

  if(qmi_cati_global_ptr->slot_count > 1)
  {
    /* Register PULL type client of GSTK and get a client id for Slot 2 */
    gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_2, GSTK_WAKEUP_TYPE,
                                            qmi_cati_gstk_init_cb_external,
                                            GSTK_SLOT_2);
    if (GSTK_SUCCESS != gstk_status)
    {
      /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
      (half initialized). So we can simply ignore the error, print an ERR trace and continue */
      UIM_MSG_MED_0("Client init failed for SLOT_2");
    }
  }

  if(qmi_cati_global_ptr->slot_count > 2)
  {
    /* Register PULL type client of GSTK and get a client id for Slot 3 */
    gstk_status = gstk_slot_client_init_ext(GSTK_SLOT_3, GSTK_WAKEUP_TYPE,
                                            qmi_cati_gstk_init_cb_external,
                                            GSTK_SLOT_3);
    if (GSTK_SUCCESS != gstk_status)
    {
      /* Part of QMI_CAT init succeeded which maye leave QMI CAT in a weird state
      (half initialized). So we can simply ignore the error, print an ERR trace and continue */
      UIM_MSG_MED_0("Client init failed for SLOT_3");
    }
  }

  if(MMGSDI_SUCCESS != mmgsdi_client_id_and_evt_reg(qmi_cat_mmgsdi_evt_cb,
                                                    qmi_cat_mmgsdi_response_cb,
                                                    0))
  {
    UIM_MSG_ERR_0("QMI-CAT : mmgsdi_client_evt_reg failed");
  }

  return TRUE;
} /* qmi_cat_service_initialize */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CAT_INIT_PRE_STARTUP()

  DESCRIPTION
    Initializes the QMI CAT global data and message queues

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_init_pre_startup
(
  void
)
{
  int  card_index  = 0;
  int  cache_index = 0;
  
  UIM_MSG_MED_0 ("QMI CAT qmi_cat_init_pre_startup");

  if (qmi_cat_state_ptr == NULL)
  {
    qmi_cat_state_ptr = uimqmi_malloc(sizeof(qmi_cati_state_type));
    ASSERT(qmi_cat_state_ptr != NULL);
    memset(qmi_cat_state_ptr, 0, sizeof(qmi_cati_state_type));
  }

  /* Do nothing if qmi_cati_global_ptr is already initialized */
  if (qmi_cati_global_ptr != NULL)
  {
    return;
  }

  qmi_cati_global_ptr = uimqmi_malloc(sizeof(qmi_cati_global_type));
  ASSERT(qmi_cati_global_ptr != NULL)
  memset(qmi_cati_global_ptr, 0, sizeof(qmi_cati_global_type));

  /* Initialize message queue */
  (void) q_init(&qmi_cati_global_ptr->qmi_cat_q);

  /* Init cache (raw and decoded) and event masks */
  for (card_index = 0; card_index < QMI_CAT_MAX_CARD_COUNT; card_index++)
  {
    qmi_cati_global_ptr->evt_reg_mask[card_index]           = CATI_REPORT_STATUS_NONE;
    qmi_cati_global_ptr->decoded_evt_reg_mask[card_index]   = CATI_REPORT_STATUS_DECODED_NONE;
    qmi_cati_global_ptr->full_func_evt_reg_mask[card_index] = CATI_REPORT_STATUS_EV_FULL_FUNC_NONE;
    qmi_cati_global_ptr->last_gstk_cmd[card_index]          = GSTK_END_PROACTIVE_CMD_REQ;
    qmi_cati_global_ptr->ref_command_id[card_index]         = 0;
    qmi_cati_global_ptr->full_func_evt[card_index]          = FALSE;

    for (cache_index = 0; cache_index < CATI_MAX_RAW_CACHED_EVTS; cache_index++)
    {
      qmi_cati_global_ptr->raw_cache[card_index][cache_index] = NULL;
    }
    for (cache_index = 0; cache_index < CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS; cache_index++)
    {
      qmi_cati_global_ptr->recovery_raw_cache[card_index][cache_index] = NULL;
    }
    for (cache_index = 0; cache_index < CATI_MAX_DECODED_CACHED_EVTS; cache_index++)
    {
      qmi_cati_global_ptr->decoded_cache[card_index][cache_index] = NULL;
    }
    for (cache_index = 0; cache_index < QMI_CATI_MAX_BIP_CHANNEL; cache_index++)
    {
      qmi_cati_global_ptr->bip_status_cache[card_index][cache_index] = CATI_BIP_SESSION_END;
    }
    for (cache_index = 0; cache_index < QMI_CATI_MAX_SCWS_CHANNEL; cache_index++)
    {
      qmi_cati_global_ptr->scws_cmd_buf[card_index][cache_index] = NULL;
    }
  }
  
  /* Read configuration mode, this is required, so that GSTK can send the TERMINAL PROFILE correctly,
     before QMI task is fully initialized */
  qmi_cati_global_ptr->config_mode = qmi_cati_read_configuration();
} /* qmi_cat_init_pre_startup */


/*===========================================================================
  FUNCTION QMI_CAT_INIT_POST_STARTUP()

  DESCRIPTION
    Registers the QMI CAT service to the QMI framework to initalize the service
    and registers with MMGSDI, GSTK.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_init_post_startup
(
  void
)
{
  qmi_framework_err_e_type      errval       = QMI_FRAMEWORK_ERR_NONE;
  qmi_framework_svc_config_type qmi_cati_cfg;
  qmi_idl_service_object_type   svc_obj;

  UIM_MSG_MED_0 ("QMI CAT qmi_cat_init_post_startup");

  /* Initialize global data
     We must do this now before we register for service because
     at the time of the reg_service_cb we need to have the
     internal qmi_message q initialized */
  if (qmi_cat_service_initialize() == FALSE)
  {
    return;
  }

  memset(&qmi_cati_cfg, 0, sizeof(qmi_cati_cfg));
  qmi_cati_cfg.base_version.major     = CATI_BASE_VER_MAJOR;
  qmi_cati_cfg.base_version.minor     = CATI_BASE_VER_MINOR;
  qmi_cati_cfg.addendum_version.major = CATI_ADDENDUM_VER_MAJOR;
  qmi_cati_cfg.addendum_version.minor = CATI_ADDENDUM_VER_MINOR;
  qmi_cati_cfg.cbs.alloc_clid         = qmi_cati_alloc_clid;
  qmi_cati_cfg.cbs.dealloc_clid       = qmi_cati_dealloc_clid;
  qmi_cati_cfg.cbs.init_cback         = qmi_cati_init_cback;
  qmi_cati_cfg.cbs.cmd_hdlr           = qmi_cati_cmd_hdlr;

  errval = qmi_framework_reg_service(QMUX_SERVICE_CAT, &qmi_cati_cfg);
  if(errval != QMI_FRAMEWORK_ERR_NONE)
  {
    UIM_MSG_ERR_1("Could not register QMI CAT with QMI Framework - errval:0x%x",
                  errval);
    return;
  }

  UIM_MSG_HIGH_0("CAT Services registered with QMI Framework");

  /* register for MPSS/QMI support interfaces logging */
  svc_obj =  cat_get_service_object_v02();
  (void) qmi_si_register_object (svc_obj,
                                 0, /* Service Instance */
                                 cat_get_service_impl_v02() );
} /* qmi_cat_init_post_startup */


/*===========================================================================
  FUNCTION QMI_CAT_GET_TERMINAL_PROFILE_VALUE_LIST()

  DESCRIPTION
    Get terminal profile value list according to the QMI_CAT config mode.

  PARAMETERS
    none

  RETURN VALUE
    gstk_profile_dl_support_ext_type* : array of terminal profile

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
gstk_profile_dl_support_ext_type* qmi_cat_get_terminal_profile_value_list
(
  uint8                            * tp_count_ptr
)
{
  qmi_error_e_type                    errval             = QMI_ERR_NONE;
  qmi_cati_config_mode_e_type         config_mode        = CATI_CONFIG_MODE_DISABLED;

  if (tp_count_ptr == NULL)
  {
    return NULL;
  }

  *tp_count_ptr = 0;

  if (qmi_cati_global_ptr != NULL)
  {
    /* Read configuration mode from global */
    config_mode = qmi_cati_global_ptr->config_mode;
  }
  else
  {
    /* QMI CAT is not init yet, read configuration mode from NV */
    config_mode = qmi_cati_read_configuration();
  }

  /* Send Terminal Profile download to GSTK */
  if (config_mode == CATI_CONFIG_MODE_ANDROID)
  {
    * tp_count_ptr = sizeof(qmi_cati_android_profile_value_list) / sizeof(gstk_profile_dl_support_ext_type);
    qmi_cat_disable_raw_unsupported_features(qmi_cati_android_profile_value_list, *tp_count_ptr);
    return qmi_cati_android_profile_value_list;
  }
  else if (config_mode == CATI_CONFIG_MODE_DECODED ||
           config_mode == CATI_CONFIG_MODE_DECODED_PULLONLY)
  {
    * tp_count_ptr = sizeof(qmi_cati_decoded_profile_value_list) / sizeof(gstk_profile_dl_support_ext_type);
    return qmi_cati_decoded_profile_value_list;
  }
  else if (config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED ||
           config_mode == CATI_CONFIG_MODE_CUSTOM_RAW)
  {
    UIM_MSG_HIGH_1("update profile for QMI_CAT custom mode 0x%x", config_mode);

    /* qmi_cat_get_terminal_profile_value_list() is called for each GSTK instance.
       Allocate memory for qmi_cati_custom_profile_value_list_ptr and reads TP
       only if the memory is not already allocated
     */
    if (NULL == qmi_cati_custom_profile.value_list_ptr)
    {
      errval = qmi_cati_get_custom_tp(&qmi_cati_custom_profile.value_list_ptr,
                                      &qmi_cati_custom_profile.tp_count);
    }

    if (errval != QMI_ERR_NONE || 0 == qmi_cati_custom_profile.tp_count)
    {
      UIM_MSG_HIGH_0("Unable to get custom TP from NV or 0 TP, default registration to GSTK.");

      /* Free allocated memory */
      QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_custom_profile.value_list_ptr);

      /* Clear qmi_cati_custom_tp_count */
      qmi_cati_custom_profile.tp_count = 0;

      /* If there was an error reading custom_tp values from NV, we send an empty
         profile value list. */
      return qmi_cati_empty_profile_value_list;
    }

    /* Set tp_count */
    *tp_count_ptr = qmi_cati_custom_profile.tp_count;
    return qmi_cati_custom_profile.value_list_ptr;
  }

  UIM_MSG_HIGH_0("CATI config mode from the NV is mode_disabled or is undefined.");
  return qmi_cati_empty_profile_value_list;
} /* qmi_cat_get_terminal_profile_value_list */


/*===========================================================================
FUNCTION: qmi_cati_gstk_init_cb

DESCRIPTION:
  Callback function from GSTK providing QMI-CAT with client id if successful.

PARAMETERS:
  gstk_reg_status : gstk client id registration status
  client_id       : client id is registration is successful
  user_data       : user data

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_init_cb
(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_id_type               client_id,
  gstk_client_ref_data_type         user_data
)
{
  gstk_status_enum_type  gstk_status = GSTK_SUCCESS;
  uint8                  slot_index  = 0;

  ASSERT(qmi_cati_global_ptr);

  (void)user_data;

  UIM_MSG_HIGH_2("qmi_cati_gstk_init_cb(): gstk_reg_status=0x%x, user_data=0x%x",
                 gstk_reg_status, user_data);

  if (gstk_reg_status != GSTK_SUCCESS)
  {
    return;
  }

  /* Retrieve the slot index */
  if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                                    (gstk_slot_id_enum_type)user_data,
                                    &slot_index) != QMI_ERR_NONE)
  {
    return;
  }

  /* Store client id */
  qmi_cati_global_ptr->cat_gstk_client_id[slot_index] = client_id;

  /* Android mode: everything in raw format */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_ANDROID ||
      qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_RAW)
  {
    gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_RAW_FORMAT,
                          sizeof(qmi_cati_complete_reg_list)/sizeof(gstk_toolkit_cmd_reg_info_type),
                          qmi_cati_complete_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_LOW_1("QMI-CAT raw reg for proactive command failed 0x%x", gstk_status);
      return;
    }

    gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_RAW_FORMAT,
                          sizeof(qmi_cati_date_time_reg_list)/sizeof(qmi_cati_date_time_reg_list[0]),
                          qmi_cati_date_time_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_LOW_1("QMI-CAT raw reg for DATE_TIME failed 0x%x", gstk_status);
      return;
    }

    gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_GSTK_FORMAT,
                          sizeof(qmi_cati_scws_reg_list)/sizeof(gstk_toolkit_cmd_reg_info_type),
                          qmi_cati_scws_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_LOW_1("QMI-CAT raw reg for scws failed 0x%x", gstk_status);
      return;
    }
  }

  /* Decoded mode: everything in decoded format */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DECODED ||
      qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED)
  {
    gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_GSTK_FORMAT,
                          sizeof(qmi_cati_complete_reg_list)/sizeof(gstk_toolkit_cmd_reg_info_type),
                          qmi_cati_complete_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_LOW_1("QMI-CAT gstk reg for proactive command failed 0x%x",
                    gstk_status);
      return;
    }

    gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_GSTK_FORMAT,
                          sizeof(qmi_cati_scws_reg_list)/sizeof(gstk_toolkit_cmd_reg_info_type),
                          qmi_cati_scws_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_LOW_1("QMI-CAT gstk reg for scws failed 0x%x", gstk_status);
      return;
    }
  }
  /* Terminal profile is sent only when the PULL type is initialized
     bacause that is done in all cases. So, no need to do it here. */
}/* qmi_cati_gstk_init_cb */


/*===========================================================================
FUNCTION: qmi_cati_gstk_init_cb_external

DESCRIPTION:
  Callback function from GSTK providing QMI-CAT with client id if successful.

PARAMETERS:
  gstk_reg_status : gstk client id registration status
  client_id       : client id is registration is successful
  user_data       : user data

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_init_cb_external
(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_id_type               client_id,
  gstk_client_ref_data_type         user_data
)
{
  uint8                             slot_index  = 0;

  ASSERT(qmi_cati_global_ptr);

  if (gstk_reg_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("Error retrieving GSTK client id: 0x%x", gstk_reg_status);
    return;
  }

  /* Retrieve the slot index */
  if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                                    (gstk_slot_id_enum_type)user_data,
                                    &slot_index) != QMI_ERR_NONE)
  {
    UIM_MSG_ERR_1("Invalid slot id for proactive command: 0x%x",
                  (gstk_slot_id_enum_type)user_data);
    return;
  }

  /* Store client id */
  qmi_cati_global_ptr->cat_gstk_client_id_external[slot_index] = client_id;

  /* For PULL only configuration, UI client has not been registered,
     so we can set Wakeup client type in both global spaces */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DECODED_PULLONLY)
  {
    qmi_cati_global_ptr->cat_gstk_client_id[slot_index] = client_id;
  }
} /* qmi_cati_gstk_init_cb_external */


/*===========================================================================
FUNCTION: qmi_cati_gstk_cmd_cb

DESCRIPTION:
  Immediate callback invoked to reflect status of registration

PARAMETERS:
  gstk_reg_status : gstk client id registration status
  user_data       : user data

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_cmd_cb
(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_ref_data_type         user_data
)
{
  (void)user_data;

  /* Nothing to do... just print the result in the log */
  if (GSTK_SUCCESS != gstk_reg_status)
  {
    UIM_MSG_ERR_1("QMI-CAT: Registration with GSTK failed 0x%x",
                  gstk_reg_status);
  }
} /* qmi_cati_gstk_cmd_cb */


/*===========================================================================
FUNCTION: QMI_CATI_COVERT_STK_CMD_ID_TO_GSTK_CMD_ID

DESCRIPTION
  This function converts an STK command to a GSTK command type

PARAMETERS
  stk_cmd_id     : [Input]        STK command type
  gstk_cmd_id_ptr: [Input/Output] GSTK command type

DEPENDENCIES
  None

RETURN VALUE
  qmi_error_e_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static qmi_error_e_type qmi_cati_covert_stk_cmd_id_to_gstk_cmd_id
(
  uint8                stk_cmd_id,
  gstk_cmd_enum_type  *gstk_cmd_id_ptr
)
{
  qmi_error_e_type  ret_val = QMI_ERR_INVALID_ARG;
  uint8             i       = 0;

  if (NULL == gstk_cmd_id_ptr)
  {
    return QMI_ERR_INTERNAL;
  }

  for(i = 0;
      i < sizeof(qmi_cat_stk_cmd_to_gstk_cmd)/sizeof(qmi_cat_stk_cmd_to_gstk_cmd[0]);
      i++)
  {
    if (qmi_cat_stk_cmd_to_gstk_cmd[i].stk_cmd_id == stk_cmd_id)
    {
      *gstk_cmd_id_ptr = qmi_cat_stk_cmd_to_gstk_cmd[i].gstk_cmd_id;
      ret_val = QMI_ERR_NONE;
      break;
    }
  }

  if (QMI_ERR_NONE != ret_val)
  {
    UIM_MSG_HIGH_1("qmi_cati_covert_stk_cmd_id_to_gstk_cmd_id(): unhandled stk_cmd_id:0x%x",
                   stk_cmd_id);
  }
  return ret_val;
}/* qmi_cati_covert_stk_cmd_id_to_gstk_cmd_id */


/*===========================================================================
FUNCTION: qmi_cati_gstk_evt_cb

DESCRIPTION:
  Callback function from GSTK providing QMI-CAT with the registered
  proactive command

PARAMETERS:
  req_ptr : [Input] proactive command information in the format client
                    requested (GSTK/RAW)

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_evt_cb
(
  gstk_client_pro_cmd_reg_data_type * req_ptr
)
{
  qmi_cat_message_type  * msg_ptr              = NULL;
  gstk_cmd_enum_type      cati_gstk_command_id = GSTK_CMD_ENUM_NOT_USE;
  uint8                   card_index           = 0;

  ASSERT(qmi_cati_global_ptr);

  /* Check parameters */
  if (req_ptr == NULL)
  {
    UIM_MSG_ERR_0("qmi_cati_gstk_evt_cb: Null ptr req_rsp_ptr");
    return;
  }

  /* Check the configuration mode: events from GSTK should be handled
     only for PUSH mode */
  if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DECODED_PULLONLY)
  {
    UIM_MSG_ERR_0("qmi_cati_gstk_evt_cb: Invalid mode");
    return;
  }

  /* Put the command in the DS cmd queue to be handled in DS context */
  switch (req_ptr->data_format_type)
  {
    /* RAW Format */
    case GSTK_RAW_FORMAT:
      if(!req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr)
      {
        UIM_MSG_ERR_0("Null ptr req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr");
        return;
      }

      /* Validate the slot_id passed by GSTK */
      if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                     req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.sim_slot_id,
                     &card_index) != QMI_ERR_NONE )
      {
        UIM_MSG_ERR_1("qmi_cati_gstk_evt_cb: Invalid card_index: 0x%x",
                          req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.sim_slot_id);
        return;
      }

      msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND);
      if(NULL == msg_ptr)
      {
        return;
      }

      msg_ptr->data.raw_proactive_cmd.cmd_ref_id =
        req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.cmd_ref_id;
      msg_ptr->data.raw_proactive_cmd.slot_id =
        req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.sim_slot_id;

      /* GSTK sets raw_hdr.tag for setup call cmd, where
         user conf alpha setup call cmd = GSTK_TAG_USER_CONF_ALPHA
         display setup call cmd = GSTK_TAG_DISPLAY_ALPHA
         other commands are set to GSTK_TAG_ORIGINAL_CMD. */
      msg_ptr->data.raw_proactive_cmd.raw_hdr_tag =
        req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->raw_hdr.tag;

      msg_ptr->data.raw_proactive_cmd.raw_data_len =
            req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_len;

      /* data len is less expected to be less than 255 */
      ASSERT(req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_len <= CAT_RAW_MAX_DATA_LEN )

      (void)memscpy(msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                    sizeof(msg_ptr->data.raw_proactive_cmd.raw_data_buffer),
                    req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_buffer_ptr,
                    (uint32)req_ptr->pro_cmd_reg_data.raw_req_rsp_ptr->payload.data_len);

      qmi_cat_post_message(msg_ptr);
      break;

    /* GSTK Format */
    case GSTK_GSTK_FORMAT :
      if (!req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr)
      {
        UIM_MSG_ERR_0("Null ptr req_ptr->gstk_req_rsp_ptr");
        return;
      }

      /* Validate slot_id passed by GSTK */
      if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                     req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id,
                     &card_index) != QMI_ERR_NONE )
      {
        UIM_MSG_ERR_1("qmi_cati_gstk_evt_cb: Invalid card_index: 0x%x",
                          req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id);
        return;
      }

      /* Copy the command id */
      cati_gstk_command_id = req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.command_id;
      UIM_MSG_HIGH_1("qmi_cati_gstk_evt_cb : Command Id 0x%x",
                     cati_gstk_command_id);
      switch(cati_gstk_command_id)
      {
        case GSTK_END_PROACTIVE_CMD_REQ:
          /* END Proactive session cmd is always made available in GSTK format.
             The same indication is used in all modes (decoded, raw) */
          msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_END_PROACTIVE_SESSION);
          if(NULL == msg_ptr)
          {
            return;
          }

          /* Store slot id */
          msg_ptr->data.end_proactive_cmd.slot_id =
            req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id;

          /* Store GSTK tag */
          msg_ptr->data.end_proactive_cmd.tag =
            req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.tag;

          /* Store end session type */
          if (req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.end_pro_cmd_req.end_from_card)
          {
            msg_ptr->data.end_proactive_cmd.proactive_session_end_type =
                  CATI_PROACTIVE_SESSION_END_DUE_TO_CARD;
          }
          else
          {
            msg_ptr->data.end_proactive_cmd.proactive_session_end_type =
                  CATI_PROACTIVE_SESSION_END_INTERNAL;
          }

          qmi_cat_post_message(msg_ptr);
          break;

        case GSTK_PROFILE_DL_IND_RSP:
          /* TP download indication is always made available in GSTK format.
             The same indication is used in all modes (decoded, raw) */
          msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_TP_DL_IND);
          if(NULL == msg_ptr)
          {
            return;
          }

          /* Store slot id */
          msg_ptr->data.tp_dl_ind.slot_id =
            req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id;

          qmi_cat_post_message(msg_ptr);
          break;

        case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
        case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
        case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
          msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_DECODED_PROACTIVE_COMMAND);
          if(NULL == msg_ptr)
          {
            return;
          }

          /* Clone decoded proactive command */
          (void)qmi_cati_clone_decoded_proactive_command(
                      &(msg_ptr->data.decoded_proactive_cmd.decoded_cmd),
                      req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr);

          qmi_cat_post_message(msg_ptr);
          break;

        case GSTK_DISPLAY_TEXT_REQ:
        case GSTK_GET_INKEY_REQ:
        case GSTK_GET_INPUT_REQ:
        case GSTK_LAUNCH_BROWSER_REQ:
        case GSTK_PLAY_TONE_REQ:
        case GSTK_SELECT_ITEM_REQ:
        case GSTK_SEND_SMS_REQ:
        case GSTK_SEND_SS_REQ:
        case GSTK_SEND_USSD_REQ:
        case GSTK_SETUP_CALL_REQ:
        case GSTK_SETUP_IDLE_TEXT_REQ:
        case GSTK_SETUP_MENU_REQ:
        case GSTK_SEND_DTMF_REQ:
        case GSTK_LANG_NOTIFICATION_REQ:
        case GSTK_PROVIDE_LANG_INFO_REQ:
        case GSTK_OPEN_CH_REQ:
        case GSTK_CLOSE_CH_REQ:
        case GSTK_SEND_DATA_REQ:
        case GSTK_RECEIVE_DATA_REQ:
        case GSTK_ACTIVATE_REQ:
        case GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY:
        case GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY:
        case GSTK_REFRESH_REQ:
          msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_DECODED_PROACTIVE_COMMAND);
          if(NULL == msg_ptr)
          {
            return;
          }

          /* Clone decoded proactive command */
          (void)qmi_cati_clone_decoded_proactive_command(
                        &(msg_ptr->data.decoded_proactive_cmd.decoded_cmd),
                        req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr);

          qmi_cat_post_message(msg_ptr);
          break;

        case GSTK_OPEN_CH_CNF:
          if (req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.terminal_response.general_result ==
              GSTK_COMMAND_PERFORMED_SUCCESSFULLY)
          {
            msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_DECODED_BIP_STATUS);
            if(NULL == msg_ptr)
            {
              return;
            }
            msg_ptr->data.decoded_bip_status.bip_status.ch_id
                = (uint8)req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.terminal_response.other_info.extra_param.open_ch_extra_param.ch_status.ch_id;
            msg_ptr->data.decoded_bip_status.bip_status.slot_id
              = req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id;
            msg_ptr->data.decoded_bip_status.bip_status.bip_status = CATI_BIP_SESSION_IN_PROGRESS;
            qmi_cat_post_message(msg_ptr);
          }
          else
          {
            UIM_MSG_ERR_2("Drop CNF: TR result 0x%x, ch id in progress 0x%x",
                          req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.terminal_response.general_result,
                          req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.terminal_response.other_info.extra_param.open_ch_extra_param.ch_status.ch_id);
          }
          break;

        case GSTK_CLOSE_CH_CNF:
          if (req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.terminal_response.general_result ==
              GSTK_COMMAND_PERFORMED_SUCCESSFULLY)
          {
            msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_DECODED_BIP_STATUS);
            if(NULL == msg_ptr)
            {
              return;
            }
            /* Since GSTK may send back to back CLOSE_CH REQ/CNF before REQ is
               processed in QMI CAT context. Thus retrieve ch id from close ch in
               progress during the process of CNF in QMI CAT context instead.
               */
            msg_ptr->data.decoded_bip_status.bip_status.ch_id
                 = req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.terminal_response.other_info.extra_param.close_ch_extra_param.ch_status.ch_id;
            msg_ptr->data.decoded_bip_status.bip_status.slot_id
              = req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id;
            msg_ptr->data.decoded_bip_status.bip_status.bip_status = CATI_BIP_SESSION_END;
            qmi_cat_post_message(msg_ptr);
          }
          else
          {
            UIM_MSG_ERR_2("Drop CNF: TR result 0x%x, ch id in progress 0x%x",
                          req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.terminal_response.general_result,
                          req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.terminal_response.other_info.extra_param.close_ch_extra_param.ch_status.ch_id);
          }
          break;

        case GSTK_SCWS_OPEN_CH_REQ:
        case GSTK_SCWS_CLOSE_CH_REQ:
        case GSTK_SCWS_SEND_DATA_REQ:
        case GSTK_SCWS_DATA_AVAIL_RSP_REQ:
          /* SCWS commands */
          msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_SCWS_COMMAND);
          if(NULL == msg_ptr)
          {
            return;
          }

          /* Clone scws command */
          (void)qmi_cati_clone_decoded_proactive_command(
                         &(msg_ptr->data.scws_cmd.scws_cmd),
                         req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr);

          qmi_cat_post_message(msg_ptr);
          break;

        case GSTK_TERM_RSP_SW1_SW2_RPT:
          msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_TERM_RSP_SW1_SW2);
          if(NULL == msg_ptr)
          {
            return;
          }

          /* Clone responses SW1 & SW2 data */
          (void)qmi_cati_clone_decoded_proactive_command(
                         &(msg_ptr->data.terminal_response_sw1_sw2_info.sw1_sw2_data),
                         req_ptr->pro_cmd_reg_data.gstk_req_rsp_ptr);

          qmi_cat_post_message(msg_ptr);
          break;

        default :
          UIM_MSG_ERR_1("Unhandled command id in GSTK format 0x%x",
                        cati_gstk_command_id);
          break;
      }
      break;

    default :
      UIM_MSG_ERR_1("Unhandled data format 0x%x", req_ptr->data_format_type);
      break;
  }
} /* qmi_cati_gstk_evt_cb */


/*===========================================================================
FUNCTION: qmi_cati_gstk_raw_env_cb

DESCRIPTION:
  Callback function from GSTK providing QMI-CAT with the envelope response
  from the card

PARAMETERS:
  env_rsp_ptr : [Input] Envelope rsp ptr (GSTK/RAW)

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_raw_env_cb
(
  gstk_cmd_from_card_type * env_rsp_ptr
)
{
  qmi_cat_message_type * msg_ptr = NULL;

  UIM_MSG_HIGH_1("qmi_cati_gstk_raw_env_cb env_rsp_ptr: 0x%x", env_rsp_ptr);

  if (!env_rsp_ptr)
  {
    return;
  }

  /* Put the command in the DS cmd queue to be handled in DS context */
  switch (env_rsp_ptr->hdr_cmd.command_id)
  {
    case GSTK_RAW_ENVELOPE_RSP :
      /* Queue this for processing in the ds task context */
      msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_ENVELOPE_RESPONSE);
      if(NULL == msg_ptr)
      {
        return;
      }

      /* Clone envelope rsp */
      (void)qmi_cati_clone_envelope_rsp(
        &(msg_ptr->data.envelope_rsp.envelope_rsp), env_rsp_ptr);
      msg_ptr->data.envelope_rsp.user_data       = (void *)env_rsp_ptr->hdr_cmd.user_data;

      qmi_cat_post_message(msg_ptr);
      break;

      default :
        UIM_MSG_ERR_1("qmi_cati_gstk_raw_env_cb:Unhandled data format 0x%x ",
                      env_rsp_ptr->hdr_cmd.command_id);
        break;
   }
}/* qmi_cati_gstk_raw_env_cb */


/*===========================================================================
FUNCTION: qmi_cati_gstk_decoded_env_cb

DESCRIPTION:
  Callback function from GSTK providing QMI-CAT with the envelope response
  from the card

PARAMETERS:
  env_rsp_ptr : [Input] Envelope rsp ptr

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cati_gstk_decoded_env_cb
(
  gstk_cmd_from_card_type * env_rsp_ptr
)
{
  qmi_cat_message_type * msg_ptr = NULL;

  if (!env_rsp_ptr)
  {
    UIM_MSG_ERR_0("qmi_cati_gstk_decoded_env_cb:Null env_rsp_ptr");
    return;
  }

  /* Queue this for processing in the ds task context */
  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_DECODED_ENVELOPE_RESPONSE);
  if(NULL == msg_ptr)
  {
    return;
  }
  /* Clone decoded envelope rsp */
  (void)qmi_cati_clone_decoded_envelope_rsp(
    &(msg_ptr->data.decoded_envelope_rsp.envelope_rsp), env_rsp_ptr);
  qmi_cat_post_message(msg_ptr);
}/* qmi_cati_gstk_decoded_env_cb */


/*===========================================================================
  FUNCTION QMI_CAT_MMGSDI_RESPONSE_CB()

  DESCRIPTION
    Callback function called by mmgsdi to return status for
    client id and evt regs api's

  PARAMETERS
    status  : return status
    cnf     : type of cnf response
    cnf_ptr : response data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_mmgsdi_response_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  qmi_cat_message_type  *msg_ptr = NULL;

  if (status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("qmi_cat_mmgsdi_response_cb(): MMGSDI op failed, status=0x%x, cnf_type=0x%x",
                  status, cnf_type);
    return;
  }

  if (NULL == cnf_ptr)
  {
    UIM_MSG_ERR_0("qmi_cat_mmgsdi_response_cb(): NULL cnf_ptr");
    return;
  }

  switch(cnf_type)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      if (MMGSDI_SUCCESS != cnf_ptr->client_id_and_evt_reg_cnf.response_header.mmgsdi_status)
      {
        UIM_MSG_ERR_0("qmi_cat_mmgsdi_response_cb(): Client id and evt registration failed");
        break;
      }
      msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_MMGSDI_CNF);
      if (msg_ptr)
      {
        (void)memscpy(&msg_ptr->data.mmgsdi_cnf.cnf_value.client_id_and_evt_reg_cnf,
                      sizeof(mmgsdi_client_id_and_evt_reg_cnf_type),
                      &cnf_ptr->client_id_and_evt_reg_cnf,
                      sizeof(mmgsdi_client_id_and_evt_reg_cnf_type));
        msg_ptr->data.mmgsdi_cnf.cnf_type = cnf_type;
        qmi_cat_post_message(msg_ptr);
      }
      break;

    default:
      break;
  }
} /* qmi_cat_mmgsdi_response_cb */


/*===========================================================================
FUNCTION QMI_CAT_MMGSDI_EVT_CB

DESCRIPTION
  Callback function to return SIM card status.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
static void qmi_cat_mmgsdi_evt_cb
(
    const mmgsdi_event_data_type   *mmgsdi_event
)
{
  qmi_cat_message_type     *msg_ptr    = NULL;
  mmgsdi_slot_id_enum_type  slot_id    = MMGSDI_MAX_SLOT_ID_ENUM;
  
  ASSERT(qmi_cati_global_ptr);

  if (NULL == mmgsdi_event)
  {
    UIM_MSG_ERR_0("qmi_cat_mmgsdi_evt_cb: Unexpected NULL pointer");
    return;
  }

  UIM_MSG_HIGH_1("qmi_cat_mmgsdi_evt_cb: evt=0x%x", mmgsdi_event->evt);

  /* Clear globals on recovery, UICC_RESET, hot swap, card power cycle */
  switch (mmgsdi_event->evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      slot_id = mmgsdi_event->data.card_inserted.slot;
      break;
    case MMGSDI_CARD_ERROR_EVT:
      slot_id = mmgsdi_event->data.card_error.slot;
      break;
    case MMGSDI_CARD_REMOVED_EVT:
      slot_id = mmgsdi_event->data.card_removed.slot;
      break;
    default:
      break;
  }

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_MMGSDI_EVT);
  if(msg_ptr)
  {
    msg_ptr->data.mmgsdi_evt.evt  = mmgsdi_event->evt;
    msg_ptr->data.mmgsdi_evt.slot = slot_id;
    qmi_cat_post_message(msg_ptr);
  }
} /* qmi_cat_mmgsdi_evt_cb */


/*===========================================================================
FUNCTION: QMI_CAT_SEND_BUFFERED_RAW_PROACTIVE_CMD_BY_INDEX

DESCRIPTION:
  QMI CAT send buffered commands at the given index to the control point that
  registers for the event

PARAMETERS:
  slot_index  : Index to card slot
  cache_index : Index to raw cache

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
static void qmi_cat_send_buffered_raw_proactive_cmd_by_index
(
  uint8  slot_index,
  uint8  cache_index
)
{
  qmi_cat_message_type     *msg_ptr   = NULL;
  qmi_cati_raw_cache_type  *cache_ptr = NULL;

  ASSERT(qmi_cati_global_ptr);

  if (slot_index  >= QMI_CAT_MAX_CARD_COUNT ||
      slot_index  >= qmi_cati_global_ptr->slot_count ||
      cache_index >= CATI_MAX_RAW_CACHED_EVTS)
  {
    return;
  }

  cache_ptr = qmi_cati_global_ptr->raw_cache[slot_index][cache_index];
  if (NULL == cache_ptr)
  {
    return;
  }

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND);
  if(NULL == msg_ptr)
  {
    return;
  }

  msg_ptr->data.raw_proactive_cmd.slot_id =
      (gstk_slot_id_enum_type)(GSTK_SLOT_1 + slot_index);
  msg_ptr->data.raw_proactive_cmd.cmd_ref_id =
      cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id;
  msg_ptr->data.raw_proactive_cmd.raw_data_len =
      cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len;
  (void)memscpy(msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                sizeof(msg_ptr->data.raw_proactive_cmd.raw_data_buffer),
                cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data,
                (uint32)msg_ptr->data.raw_proactive_cmd.raw_data_len);
  msg_ptr->data.raw_proactive_cmd.is_cached_cmd = TRUE;
  
  qmi_cat_post_message(msg_ptr);

  /* Mark cache as invalid */
  QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][cache_index]);
}/* qmi_cat_send_buffered_raw_proactive_cmd_by_index */


/*===========================================================================
FUNCTION: QMI_CAT_SEND_RECOVERY_CACHE_PROACTIVE_CMD_BY_INDEX

DESCRIPTION:
  QMI CAT send recovery cache commands at the given index to the control point
  that registers for the event

PARAMETERS:
  slot_index  : Index to card slot
  cache_index : Index to raw cache

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
static void qmi_cat_send_recovery_cache_proactive_cmd_by_index
(
  uint8  slot_index,
  uint8  cache_index
)
{
  qmi_cat_message_type     *msg_ptr   = NULL;
  qmi_cati_raw_cache_type  *cache_ptr = NULL;

  ASSERT(qmi_cati_global_ptr);

  if (slot_index  >= QMI_CAT_MAX_CARD_COUNT ||
      slot_index  >= qmi_cati_global_ptr->slot_count ||
      cache_index >= CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS)
  {
    return;
  }

  cache_ptr = qmi_cati_global_ptr->recovery_raw_cache[slot_index][cache_index];
  if (NULL == cache_ptr)
  {
    return;
  }

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND);
  if(NULL == msg_ptr)
  {
    return;
  }

  msg_ptr->data.raw_proactive_cmd.slot_id =
      (gstk_slot_id_enum_type)(GSTK_SLOT_1 + slot_index);
  msg_ptr->data.raw_proactive_cmd.cmd_ref_id =
      cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id;
  msg_ptr->data.raw_proactive_cmd.raw_data_len =
      cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len;
  (void)memscpy(msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                sizeof(msg_ptr->data.raw_proactive_cmd.raw_data_buffer),
                cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data,
                (uint32)msg_ptr->data.raw_proactive_cmd.raw_data_len);
  msg_ptr->data.raw_proactive_cmd.is_cached_cmd = TRUE;

  qmi_cat_post_message(msg_ptr);
}/* qmi_cat_send_recovery_cache_proactive_cmd_by_index */


/*===========================================================================
FUNCTION: QMI_CAT_SEND_BUFFERED_RAW_PROACTIVE_CMD

DESCRIPTION:
  QMI CAT send buffered commands when the control point registers for these
  later

PARAMETERS:
  mask : [Input] mask for sending the buffered command

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
static void qmi_cat_send_buffered_raw_proactive_cmd
(
  uint32 mask,
  uint8  card_index
)
{
  ASSERT(qmi_cati_global_ptr);

  if (card_index  >= QMI_CAT_MAX_CARD_COUNT ||
      card_index  >= qmi_cati_global_ptr->slot_count)
  {
    return;
  }

  /* If there is a cached Setup menu not sent yet, Queue this proactive command
     else if TR is pending send the recovery cached command */
  if (mask & CATI_REPORT_STATUS_EV_SETUP_MENU)
  {
    if(qmi_cati_global_ptr->raw_cache[card_index][CATI_CACHED_EVT_SETUP_MENU] != NULL)
    {
      qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                       CATI_CACHED_EVT_SETUP_MENU);
    }
    else if(qmi_cati_global_ptr->last_gstk_cmd[card_index] == GSTK_SETUP_MENU_REQ)
    {
      qmi_cat_send_recovery_cache_proactive_cmd_by_index(card_index,
                                                         CATI_QMI_CLIENT_CACHED_EVT_SETUP_MENU);
    }
  }

  /* If there is a cached Display text not sent yet,
     Queue this proactive command for DS task */
  if (mask & CATI_REPORT_STATUS_EV_DISPLAY_TEXT)
  {
    qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                     CATI_CACHED_EVT_DISPLAY_TEXT);
  }

  /* If there is a cached Setup Idle Mode Text not sent yet, Queue this proactive command
     else if TR is pending send the recovery cached command */
  if (mask & CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT)
  {
    if(qmi_cati_global_ptr->raw_cache[card_index][CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT] != NULL)
    {
      qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                       CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT);
    }
    else if(qmi_cati_global_ptr->last_gstk_cmd[card_index] == GSTK_SETUP_IDLE_TEXT_REQ)
    {
      qmi_cat_send_recovery_cache_proactive_cmd_by_index(card_index,
                                                         CATI_QMI_CLIENT_CACHED_EVT_SETUP_IDLE_MODE_TEXT);
    }
  }

  /* If there is a cached Setup Event List not sent yet, Queue this proactive command
     else if TR is pending send the recovery cached command  */
  if (mask & CATI_SETUP_EVT_LIST_MASK)
  {
    if(qmi_cati_global_ptr->raw_cache[card_index][CATI_CACHED_EVT_SETUP_EVT_LIST] != NULL)
    {
      qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                       CATI_CACHED_EVT_SETUP_EVT_LIST);
    }
    else if(qmi_cati_global_ptr->last_gstk_cmd[card_index] == GSTK_SETUP_EVENT_LIST_REQ)
    {
      qmi_cat_send_recovery_cache_proactive_cmd_by_index(card_index,
                                                         CATI_QMI_CLIENT_CACHED_EVT_SETUP_EVT_LIST);
    }
  }

  /* If there is a cached PLI not sent yet,
     Queue this proactive command for DS task */
  if (mask & CATI_PROVIDE_LOCAL_INFO_MASK)
  {
    qmi_cat_send_buffered_raw_proactive_cmd_by_index(card_index,
                                                     CATI_CACHED_EVT_PROVIDE_LOCAL_INFO);
  }
} /* qmi_cat_send_buffered_raw_proactive_cmd */


/*===========================================================================
FUNCTION: QMI_CAT_SEND_BUFFERED_DECODED_PROACTIVE_CMD_BY_INDEX

DESCRIPTION:
  QMI CAT send buffered commands at the given index to the control point that
  registers for the event

PARAMETERS:
  slot_index  : Index to card slot
  cache_index : Index to decoded cache

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
static void qmi_cat_send_buffered_decoded_proactive_cmd_by_index
(
  uint8  slot_index,
  uint8  cache_index
)
{
  qmi_cat_message_type  * msg_ptr = NULL;

  ASSERT(qmi_cati_global_ptr);

  if (slot_index  >= QMI_CAT_MAX_CARD_COUNT ||
      slot_index  >= qmi_cati_global_ptr->slot_count ||
      cache_index >= CATI_MAX_DECODED_CACHED_EVTS)
  {
    UIM_MSG_ERR_2("Invalid slot_index: 0x%x or cache_index: 0x%x",
                  slot_index, cache_index);
    return;
  }

  if (NULL == qmi_cati_global_ptr->decoded_cache[slot_index][cache_index])
  {
    return;
  }

  if (CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL == cache_index)
  {
    msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_SCWS_COMMAND);
  }
  else
  {
    msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_DECODED_PROACTIVE_COMMAND);
  }
  if (NULL == msg_ptr)
  {
    return;
  }

  msg_ptr->data.decoded_proactive_cmd.is_cached_cmd = TRUE;

  /* Clone decoded proactive command */
  (void)qmi_cati_clone_decoded_proactive_command(
      &(msg_ptr->data.decoded_proactive_cmd.decoded_cmd),
      &qmi_cati_global_ptr->decoded_cache[slot_index][cache_index]->cached_event);
  qmi_cat_post_message(msg_ptr);

  /* Free the cache */
  qmi_cati_free_decoded_proactive_command(
      &qmi_cati_global_ptr->decoded_cache[slot_index][cache_index]->cached_event);
  uimqmi_free(qmi_cati_global_ptr->decoded_cache[slot_index][cache_index]);
  qmi_cati_global_ptr->decoded_cache[slot_index][cache_index] = NULL;
}/* qmi_cat_send_buffered_decoded_proactive_cmd_by_index */


/*===========================================================================
FUNCTION: QMI_CAT_SEND_BUFFERED_DECODED_PROACTIVE_CMD

DESCRIPTION:
  QMI CAT send buffered commands when the control point registers for these
  later

PARAMETERS:
  mask : [Input] mask for sending the buffered command

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
static void qmi_cat_send_buffered_decoded_proactive_cmd
(
  uint32 mask,
  uint8  card_index
)
{
  ASSERT(qmi_cati_global_ptr);

  /* SETUP MENU */
  if (mask & CATI_REPORT_STATUS_EV_DECODED_SETUP_MENU)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_SETUP_MENU);
  }

  /* DISPLAY TEXT */
  if (mask & CATI_REPORT_STATUS_EV_DECODED_DISPLAY_TEXT)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_DISPLAY_TEXT);
  }

  /* SETUP IDLE MODE TEXT */
  if (mask & CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_SETUP_IDLE_MODE_TEXT);
  }

  /* SETUP EVENT LIST: User activity */
  if (mask & CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_USER_ACT)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_USER_ACT);
  }

  /* SETUP EVENT LIST: Idle screen */
  if (mask & CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_IDL_SCR_NOTIFY)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_IDLE_SCREEN);
  }

  /* SETUP EVENT LIST: Language selection */
  if (mask & CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_LANGSEL_NOTIFY)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_LANG_SEL);
  }

  /* SETUP EVENT LIST: HCI Connectivity */
  if (mask & CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_HCI_CONN)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_HCI_CONN);
  }

  /* SETUP EVENT LIST: Browser Termination */
  if (mask & CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_BROWSER_TERM)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_SETUP_EVT_LIST_BROWSER_TERM);
  }

  /* SCWS OPEN CHANNEL */
  if (mask & CATI_REPORT_STATUS_EV_DECODED_SCWS_EVT)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL);
  }

  /* PLI Language*/
  if (mask & CATI_REPORT_STATUS_EV_DECODED_PROVIDE_LOCAL_INFO_LANG)
  {
    qmi_cat_send_buffered_decoded_proactive_cmd_by_index(
                              card_index,
                              CATI_CACHED_EVT_DECODED_PROVIDE_LOCAL_INFO_LANG);
  }
} /* qmi_cat_send_buffered_decoded_proactive_cmd */


/*===========================================================================
FUNCTION: QMI_CAT_SEND_BUFFERED_BIP_STATUS

DESCRIPTION:
  QMI CAT send buffered commands when the control point registers for these
  later

PARAMETERS:
  mask : [Input] mask for sending the buffered command

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:

===========================================================================*/
static void qmi_cat_send_buffered_bip_status
(
  uint8  card_index
)
{
  qmi_cat_message_type  * msg_ptr    = NULL;
  uint8                   i          = 0;

  ASSERT(qmi_cati_global_ptr);

  if (card_index >= QMI_CAT_MAX_CARD_COUNT)
  {
    UIM_MSG_ERR_1("Invalid card_index 0x%x", card_index);
    return;
  }

  for (i=0; i < QMI_CATI_MAX_BIP_CHANNEL; i++)
  {
    msg_ptr = NULL;
    switch(qmi_cati_global_ptr->bip_status_cache[card_index][i])
    {
      case CATI_BIP_SESSION_IN_PROGRESS:
        msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_DECODED_BIP_STATUS);
        if(NULL == msg_ptr)
        {
          return;
        }

        msg_ptr->data.decoded_bip_status.bip_status.slot_id = (gstk_slot_id_enum_type)(GSTK_SLOT_1 + card_index);
        msg_ptr->data.decoded_bip_status.bip_status.bip_status = CATI_BIP_SESSION_IN_PROGRESS;
        msg_ptr->data.decoded_bip_status.bip_status.ch_id = i + 1;
        qmi_cat_post_message(msg_ptr);
        break;

     case CATI_BIP_SESSION_END:
     default:
       break;
    }
  }
} /* qmi_cat_send_buffered_bip_status */


/*===========================================================================
  FUNCTION QMI_CATI_EVENT_REPORT_IND()

  DESCRIPTION
    Is called when any of the conditions set in set_event_report becomes true
    Sends an indication to the client

  PARAMETERS
    cl_sp          : client state pointer
    event_info_ptr : pointer to the event report

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_event_report_ind
(
  uint8                                clid,
  const qmi_cati_evt_rpt_info_s_type * event_info_ptr
)
{
  dsm_item_type * ind      = NULL;

  ASSERT(event_info_ptr);
  ASSERT(qmi_cati_global_ptr);

  if (event_info_ptr->mask &  CATI_RAW_PROACTIVE_CMD_MASK ||
           event_info_ptr->mask &  CATI_PROVIDE_LOCAL_INFO_MASK)
  {
    uint8  tlv_id = 0;

    /* Populate evt tlv for raw cmd types */
    tlv_id = cati_conv_raw_mask_to_tlv_id(
                          event_info_ptr->mask,
                          event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.data_evt_type);

    if (QMI_ERR_NONE != qmi_cat_response_raw_proactive_command(
                              tlv_id,
                              &ind,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.raw_data_len,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.raw_data_ptr,
                              event_info_ptr->slot_id,
                              event_info_ptr->response_type))
    {
      UIM_MSG_HIGH_0("Unable to generate CATI Event Report indication for raw proactive cmd!");
      dsm_free_packet(&ind);
      return;
    }
  }
  else if(event_info_ptr->mask & CATI_SETUP_EVT_LIST_MASK)
  {
    if (QMI_ERR_NONE != qmi_cat_response_raw_proactive_command(
                              CATI_PRM_TYPE_SETUP_EVENT_LIST_RAW,
                              &ind,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.raw_data_len,
                              event_info_ptr->cati_evt_rpt.cati_raw_pc_evt.raw_data_ptr,
                              event_info_ptr->slot_id,
                              event_info_ptr->response_type))
    {
      UIM_MSG_HIGH_0("Unable to generate CATI Event Report indication for Setup Evt list!");
      dsm_free_packet(&ind);
      return;
    }
  }
  else if(event_info_ptr->mask & CATI_REPORT_STATUS_EV_END_PROACTIVE_SESSION)
  {
    if (QMI_ERR_NONE != qmi_cat_response_end_proactive_session(
                              &ind,
                              event_info_ptr->cati_evt_rpt.cati_end_proactive_cmd_evt.proactive_session_end_type,
                              event_info_ptr->slot_id))
    {
      UIM_MSG_HIGH_0("Unable to generate CATI Event Report indication for end session!");
      dsm_free_packet(&ind);
      return;
    }
  }
  else
  {
    UIM_MSG_ERR_0("Should not be here ... unsupported mask in qmi_cati_event_report_ind!");
    return;
  }

  (void)qmi_cati_send_indication(clid, CATI_CMD_VAL_SET_EVENT_REPORT, ind);
} /* qmi_cati_event_report_ind */


/*===========================================================================
  FUNCTION QMI_CATI_DECODED_BIP_STATUS_EVENT_REPORT_IND()

  DESCRIPTION
    Is called when BIP status event set in becomes true
    Sends an indication to the client

  PARAMETERS
    clid          : client id
    bip_status    : BIP status

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_decoded_bip_status_event_report_ind
(
  uint8                                    clid,
  const qmi_cat_decoded_bip_status_type  * bip_status
)
{
  dsm_item_type    * ind    = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;

  ASSERT(bip_status != NULL);

  ind = NULL;

  errval = qmi_cat_response_decoded_bip_status(&ind, bip_status);
  if (errval != QMI_ERR_NONE)
  {
    UIM_MSG_ERR_0("Unable to generate decoded Event Report indication!");
    dsm_free_packet(&ind);
    return;
  }

  (void)qmi_cati_send_indication(clid, CATI_CMD_VAL_SET_EVENT_REPORT, ind);
} /* qmi_cati_decoded_bip_status_event_report_ind */


/*===========================================================================
FUNCTION qmi_cati_find_length_of_length_value

DESCRIPTION
  This function determines the size of the length field:
  first byte of length field [0x00, 0x7F] -> length field size == 1
  first byte of length field == 0x81 -> length field size == 2

PARAMETERS
  length_value: [Input] pointer to the length value field

DEPENDENCIES
  None

RETURN VALUE
  byte -> length of the length field in TLV

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static uint8 qmi_cati_find_length_of_length_value
(
  const uint8 * length_value
)
{
  uint8 ret_val = 0;

  if (NULL == length_value)
  {
    UIM_MSG_ERR_0("qmi_cati_find_length_of_length_value: length_value ERR:NULL");
    return ret_val;
  }

  UIM_MSG_LOW_0(" == IN qmi_cati_find_length_of_length_value");

  /* This length field can either be 0x00-0x7F or
     0x81 if the 2nd byte is used */
  if(length_value[0] <= 0x7F)
  {
    ret_val = 1;
    UIM_MSG_LOW_2(" == length_value[0] = %x ret_val = %x",
                  length_value[0], ret_val);
  }
  else if(length_value[0] == 0x81)
  {
    ret_val = 2;
    UIM_MSG_LOW_2(" == length_value[0] = %x ret_val = %x",
                  length_value[0], ret_val);
  }
  else
  {
    UIM_MSG_ERR_1("qmi_cati_find_length_of_length_value,Wrong length field value: %x",
                  length_value[0]);
  }
  return ret_val;
} /* qmi_cati_find_length_of_length_value */


/*===========================================================================
FUNCTION qmi_cati_send_raw_term_response

DESCRIPTION
  This function sends the raw Term Response

PARAMETERS
  mask: Input mask specifying the event
  cmd_ref_id :

DEPENDENCIES
  None

RETURN VALUE
  byte -> length of the length field in TLV

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type qmi_cati_send_raw_term_response
(
  gstk_slot_id_enum_type         slot_id,
  uint32                         cmd_ref_id,
  int32                          raw_data_len,
  const uint8*                   raw_data_ptr,
  gstk_general_result_enum_type  result
)
{
  int32                  length_of_TLV_length_field = 0;
  gstk_status_enum_type  gstk_status                = GSTK_SUCCESS;
  gstk_generic_data_type term_resp                  = {0};
  uint8                  slot_index                 = 0;

  /* Struct used for the raw msg type */
  PACKED struct PACKED_POST
  {
    uint8     command_details[CATI_CMD_DETAILS_LEN];
    uint8     device_id_tag;
    uint8     device_tag_length;
    uint8     source_device;
    uint8     target_device;
    uint8     result_tag;
    uint8     result_tag_length;
    uint8     result;
    uint8     addl_result;
  }STK_response;

  ASSERT(qmi_cati_global_ptr);

  memset(&STK_response, 0, sizeof(STK_response));

  if (raw_data_len < 1 || raw_data_ptr == NULL)
  {
    UIM_MSG_HIGH_2("Invalid raw length (0x%x) or pointer (0x%x)",
                   raw_data_len, raw_data_ptr);
    return GSTK_ERROR;
  }

  /* Retrieve the slot index */
  if( qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(slot_id,
                                                      &slot_index) != QMI_ERR_NONE)
  {
    UIM_MSG_ERR_1("qmi_cati_send_term_response: fail to covert slot_id, 0x%x",
                  slot_id);
    return GSTK_ERROR;
  }

  /* Populate the generic successful resp here for the cached cmds */
  length_of_TLV_length_field = qmi_cati_find_length_of_length_value(raw_data_ptr + 1);

  if(length_of_TLV_length_field != 1 &&
     length_of_TLV_length_field != 2)
  {
    /* Invalid BER TLV len , ignore the command */
    UIM_MSG_MED_1(" ERROR:  Length of TLV Invalid: 0x%x",
                  length_of_TLV_length_field);
    return GSTK_ERROR;
  }

  if (raw_data_len < length_of_TLV_length_field + 1 + (int32) sizeof(STK_response.command_details))
  {
    UIM_MSG_HIGH_1("Invalid raw length: 0x%x", raw_data_len);
    return GSTK_ERROR;
  }

  /* 4th location after total length field is the type of command
  ** 0   Proactive SIM Command Tag
  ** 1-2 total length
  ** +1  Command Details tag
  ** +1  Command details len = 3
  ** +1  Command number
  ** +1  Command Type
  ** +1  Command Qualifier */

  /* Pack various TLVs */
  /* Command details */
  (void)memscpy((void *) STK_response.command_details,
                sizeof(STK_response.command_details),
                (raw_data_ptr + length_of_TLV_length_field + 1),
                sizeof(STK_response.command_details));

  /* device ID */
  STK_response.device_id_tag     = 0x02; /* GSTK_DEVICE_IDENTITY_TAG */
  STK_response.device_tag_length = 0x02; /* GSTK_DEVICE_IDENTITY_LEN */
  STK_response.source_device     = 0x82; /* GSTK_ME_DEVICE */
  STK_response.target_device     = 0x81; /* GSTK_UICC_SIM_DEVICE */

  /* result tag */
  STK_response.result_tag        = 0x03; /* GSTK_RESULT_TAG */
  STK_response.result_tag_length = (result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY) ? 1: 2;
  STK_response.result            = (uint8)result;

  /* total data length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */

  /* Fill in the term_resp now for the  raw format api */
  term_resp.data_len = (result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY) ?
                         (sizeof(STK_response) - sizeof(STK_response.addl_result)) :
                         sizeof(STK_response);
  term_resp.data_buffer_ptr = (uint8 *)&STK_response;

  gstk_status = gstk_send_raw_terminal_response (qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                  0,
                                                  cmd_ref_id,
                                                  term_resp );

  if(GSTK_SUCCESS != gstk_status)
  {
    UIM_MSG_ERR_1("qmi_cati_send_term_response: tr fail, 0x%x", gstk_status);
  }

  return gstk_status;
}/* qmi_cati_send_raw_term_response */


/*===========================================================================
FUNCTION: qmi_cat_end_bip_session

DESCRIPTION:
  This function is used to clean-up BIP session

PARAMETERS:
  slot_index: slot index

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cat_end_bip_session
(
  uint8  slot_index
)
{
  uint8                             i           = 0;
  uint8                             j           = 0;  
  qmi_cat_decoded_bip_status_type   bip_status;  
  qmi_cati_client_state_type      * cl_sp       = NULL;  

  /* Send END BIP SESSION status active clients */
  for (i = 0; i < QMI_CATI_MAX_BIP_CHANNEL; i++)
  {
    if (qmi_cati_global_ptr->bip_status_cache[slot_index][i] == CATI_BIP_SESSION_IN_PROGRESS)
    {
      memset(&bip_status, 0, sizeof(qmi_cat_decoded_bip_status_type));
      bip_status.slot_id = slot_index;
      bip_status.ch_id = i + 1;
      bip_status.bip_status = CATI_BIP_SESSION_END;

      for (j = 0; j < CATI_MAX_CLIDS; j++)
      {
        cl_sp = qmi_cat_state_ptr->client[j];

        /* check if this service instance is bound to the qmi device */
        if(NULL == cl_sp)
        {
          continue;
        }

        if( (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
            (cl_sp->decoded_evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_DECODED_DATA_EVT_STATUS))
        {
          qmi_cati_decoded_bip_status_event_report_ind(cl_sp->common.clid,
                                                       &bip_status);
        }
      }
      /* update BIP Status */
      qmi_cati_global_ptr->bip_status_cache[slot_index][i] = CATI_BIP_SESSION_END;
    }
  }
} /* qmi_cat_end_bip_session */


/*===========================================================================
FUNCTION: qmi_cat_cleanup_globals

DESCRIPTION:
  This function is used to free the qmicat globals

PARAMETERS:
  card_index: slot index

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
static void qmi_cat_cleanup_globals
(
  uint8 card_index
)
{
  uint8                  cache_index     = 0;

  qmi_cati_global_ptr->last_gstk_cmd[card_index]                 = GSTK_END_PROACTIVE_CMD_REQ;
  qmi_cati_global_ptr->ref_command_id[card_index]                = 0;
  qmi_cati_global_ptr->full_func_evt[card_index]                 = FALSE;
  qmi_cati_global_ptr->decoded_setup_evt_in_progress[card_index] = 0;
  qmi_cati_global_ptr->card_state[card_index]                    = QMI_CAT_CARD_STATE_ERROR;

  /* Clear QMI cache*/
  for (cache_index = 0; cache_index < QMI_CATI_MAX_BIP_CHANNEL; cache_index++)
  {
    qmi_cati_global_ptr->bip_status_cache[card_index][cache_index] = CATI_BIP_SESSION_END;
  }

  for(cache_index = 0; cache_index < CATI_MAX_RAW_CACHED_EVTS; cache_index++)
  {
    if(qmi_cati_global_ptr->raw_cache[card_index][cache_index] != NULL)
    {
      uimqmi_free(qmi_cati_global_ptr->raw_cache[card_index][cache_index]);
      qmi_cati_global_ptr->raw_cache[card_index][cache_index] = NULL;
    }
  }

  for(cache_index = 0; cache_index < CATI_MAX_RAW_QMI_CLIENT_CACHED_EVTS; cache_index++)
  {
    if(qmi_cati_global_ptr->recovery_raw_cache[card_index][cache_index] != NULL)
    {
      uimqmi_free(qmi_cati_global_ptr->recovery_raw_cache[card_index][cache_index]);
      qmi_cati_global_ptr->recovery_raw_cache[card_index][cache_index] = NULL;
    }
  }

  for(cache_index = 0; cache_index < CATI_MAX_DECODED_CACHED_EVTS; cache_index++)
  {
    if(qmi_cati_global_ptr->decoded_cache[card_index][cache_index] != NULL)
    {
      qmi_cati_free_decoded_proactive_command(
          &qmi_cati_global_ptr->decoded_cache[card_index][cache_index]->cached_event);
      uimqmi_free(qmi_cati_global_ptr->decoded_cache[card_index][cache_index]);
      qmi_cati_global_ptr->decoded_cache[card_index][cache_index] = NULL;
    }
  }

  for(cache_index = 0; cache_index < QMI_CATI_MAX_SCWS_CHANNEL; cache_index++)
  {
    qmi_cati_cmd_buf_cache_type *current_node_ptr = NULL;

    current_node_ptr = qmi_cati_global_ptr->scws_cmd_buf[card_index][cache_index];
    while (current_node_ptr != NULL)
    {
      qmi_cati_cmd_buf_cache_type *next_node_ptr = NULL;

      next_node_ptr = current_node_ptr->next_ptr;
      qmi_cat_free_cb_userdata((qmi_cati_userdata_type *)current_node_ptr->cb_userdata_ptr);
      uimqmi_free(current_node_ptr);
      current_node_ptr = next_node_ptr;
    }
    qmi_cati_global_ptr->scws_cmd_buf[card_index][cache_index] = NULL;
  }
} /* qmi_cat_cleanup_globals */


/*===========================================================================
  FUNCTION  QMI_CAT_CACHE_RAW_PROACTIVE_COMMAND()

  DESCRIPTION
    Store proactive command in QMI CAT regular or recovery raw cache

  PARAMETERS
    cat_msg_ptr       : Pointer to qmi_cat_message_type
    command_id        : Command id, e.g. GSTK_CMD_STK_SET_UP_MENU
    slot_index        : Slot on which the command is for
    is_recovery_cache : is the proactive command to be stored in recovery cache

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cat_cache_raw_proactive_command
(
  const qmi_cat_message_type  *cat_msg_ptr,
  uint8                        command_id,
  uint8                        slot_index,
  boolean                      is_recovery_cache
)
{
  uint8                      i              = 0;
  qmi_cati_raw_cache_type  **raw_cache_pptr = NULL;

  UIM_MSG_HIGH_3("qmi_cat_cache_raw_proactive_command: command_id=0x%x, slot_index=0x%x, is_recovery_cache=0x%x",
                 command_id, slot_index, is_recovery_cache);

  if (NULL == cat_msg_ptr || NULL == qmi_cati_global_ptr)
  {
    UIM_MSG_ERR_0("NULL cat_msg_ptr or qmi_cati_global_ptr");
    return QMI_ERR_INTERNAL;
  }

  if (slot_index >= QMI_CAT_MAX_CARD_COUNT ||
      slot_index >= qmi_cati_global_ptr->slot_count)
  {
    UIM_MSG_ERR_1("Invalid slot_index: 0x%x", slot_index);
    return QMI_ERR_INTERNAL;
  }

  if (cat_msg_ptr->message_type != QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("Incorrect message_type: 0x%x", cat_msg_ptr->message_type);
    return QMI_ERR_INTERNAL;
  }

  if (qmi_cati_global_ptr->card_state[slot_index] == QMI_CAT_CARD_STATE_INVALID ||
      qmi_cati_global_ptr->card_state[slot_index] == QMI_CAT_CARD_STATE_ERROR)
  {
    UIM_MSG_ERR_1("Incorrect card state: 0x%x", qmi_cati_global_ptr->card_state[slot_index]);
    return QMI_ERR_INTERNAL;
  }

  if (FALSE == is_recovery_cache)
  {
    for (i = 0;
         i < (sizeof(qmi_cat_raw_cache_cmd_index)/sizeof(qmi_cat_raw_cache_cmd_index[0]));
         i++)
    {
      if (qmi_cat_raw_cache_cmd_index[i].command_id == command_id)
      {
        raw_cache_pptr =
          &qmi_cati_global_ptr->raw_cache[slot_index][qmi_cat_raw_cache_cmd_index[i].cache_index];
        break;
      }
    }
  }
  else
  {
    for (i = 0;
         i < (sizeof(qmi_recovery_cat_raw_cache_cmd_index)/sizeof(qmi_recovery_cat_raw_cache_cmd_index[0]));
         i++)
    {
      if (qmi_recovery_cat_raw_cache_cmd_index[i].command_id == command_id)
      {
        raw_cache_pptr =
          &qmi_cati_global_ptr->recovery_raw_cache[slot_index][qmi_recovery_cat_raw_cache_cmd_index[i].cache_index];
        break;
      }
    }
  }

  if (NULL == raw_cache_pptr)
  {
    return QMI_ERR_INTERNAL;
  }

  /* Check raw data len */
  if (cat_msg_ptr->data.raw_proactive_cmd.raw_data_len > CAT_RAW_MAX_DATA_LEN)
  {
    /* Free and invalidate the existing cache entry if not NULL */
    QMI_CAT_FREE_IF_NOT_NULL(*raw_cache_pptr);
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Cache the proactive command */
  if (NULL == *raw_cache_pptr)
  {
    *raw_cache_pptr =
      (qmi_cati_raw_cache_type *)uimqmi_malloc(sizeof(qmi_cati_raw_cache_type));
    if (NULL == *raw_cache_pptr)
    {
      UIM_MSG_ERR_1("Failed to allocate memory for raw cache, command_id=0x%x",
                    command_id);
      return QMI_ERR_NO_MEMORY;
    }
  }
  else
  {
    memset(*raw_cache_pptr,
           0x00,
           sizeof(**raw_cache_pptr));
  }


  (*raw_cache_pptr)->cached_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id =
      cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id;
  (*raw_cache_pptr)->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len =
    (uint16)cat_msg_ptr->data.raw_proactive_cmd.raw_data_len;
  (void)memscpy((*raw_cache_pptr)->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data,
                sizeof((*raw_cache_pptr)->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data),
                cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                (uint32)cat_msg_ptr->data.raw_proactive_cmd.raw_data_len);

  return QMI_ERR_NONE;
}/* qmi_cat_cache_raw_proactive_command */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_RAW_PROACTIVE_COMMAND()

  DESCRIPTION
    To process the proactive command from GSTK in QMI context

  PARAMETERS
    cmd_ref_id   : Reference ID unique to command
    raw_data_len : Length of raw command data
    raw_data_ptr : Raw command data buffer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_raw_proactive_command
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  qmi_cati_client_state_type * cl_sp                           = NULL;
  uint8                        i                               = 0;
  uint8                        type_of_command                 = 0;
  uint8                        length_of_TLV_length_field      = 1;
  gstk_status_enum_type        gstk_status                     = GSTK_SUCCESS;
  qmi_cati_evt_rpt_info_s_type cati_event                      = {0};
  boolean                      ind_sent                        = FALSE;
  uint8                        slot_index                      = 0;

  ASSERT(cat_msg_ptr != NULL);
  ASSERT(qmi_cati_global_ptr);
  ASSERT(qmi_cat_state_ptr);

  /* Retrieve the slot index */
  if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                                    cat_msg_ptr->data.raw_proactive_cmd.slot_id,
                                    &slot_index) != QMI_ERR_NONE)
  {
    UIM_MSG_ERR_1("Invalid slot id for proactive command: 0x%x",
                  cat_msg_ptr->data.raw_proactive_cmd.slot_id);
    return;
  }

  /* Set slot id in the cat event */
  cati_event.slot_id = cat_msg_ptr->data.raw_proactive_cmd.slot_id;

  /* Set response_type */
  cati_event.response_type = cat_msg_ptr->data.raw_proactive_cmd.raw_hdr_tag;

  /* BIP related commands require raw mask and data_evt_type to determine for
     tlv_id*/
  cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_NONE;

  /* Find the BER-TLV Length: This length field can either be 0x00-0x7F
     or 0x81 if the 2nd byte is used */
  length_of_TLV_length_field =
            qmi_cati_find_length_of_length_value(cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer + 1);

  if ((1 != length_of_TLV_length_field) && (2 != length_of_TLV_length_field))
  {
    /* Invalid BER TLV len, ignore the command */
    UIM_MSG_MED_1(" ERROR:  Length of TLV Invalid: 0x%x",
                  length_of_TLV_length_field);
    return;
  }

  /* 4th location after total length field is the type of command:
     0   Proactive SIM Command Tag
     1-2 Total length
     +1  Command Details tag
     +1  Command details len = 3
     +1  Command number
     +1  Command Type
     +1  Command Qualifier */
  type_of_command = *(cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer + length_of_TLV_length_field + 4);

  /* For newly received proactive commands, set last_gstk_cmd, ref_command_id,
     and full_func_evt so that those info can be used later to correctly track
     whether a TR/Event should be rejected or allowed.
     last_gstk_cmd, ref_command_id, and full_func_evt are cleared when proactive
     session ends
     Note:
       last_gstk_cmd, ref_command_id, and full_func_evt should be in sync with
       the ongoing UICC proactive command, therefore, for proactive commands
       resent from cache, do not change any of the variables to avoid incorrectly
       set the variables when the processing of the related UICC proactive command
       has already ended.
   */
  if (FALSE == cat_msg_ptr->data.raw_proactive_cmd.is_cached_cmd)
  {
    /* Set last_gstk_cmd */
    if (QMI_ERR_NONE != qmi_cati_covert_stk_cmd_id_to_gstk_cmd_id(
                            type_of_command,
                            &(qmi_cati_global_ptr->last_gstk_cmd[slot_index])))
    {
      return;
    }

    /* Set full_func_evt for proactive commands that support event routing if
       this is a new UICC proactive command */
    if (qmi_cati_global_ptr->ref_command_id[slot_index] !=
        cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id)
    {
      switch (type_of_command)
      {
        case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS)? TRUE:FALSE;
          break;

        case GSTK_CMD_STK_SET_UP_CALL:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL)? TRUE:FALSE;
          break;

        case GSTK_CMD_STK_SEND_DTMF:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF)? TRUE:FALSE;
          break;
          
        case GSTK_CMD_STK_SEND_SS:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS)? TRUE:FALSE;
          break;

        case GSTK_CMD_STK_USSD:
          qmi_cati_global_ptr->full_func_evt[slot_index] =
            (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
             CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS)? TRUE:FALSE;
          break;

        default:
          break;
      }
    }

    /* Set cmd_ref_id */
    qmi_cati_global_ptr->ref_command_id[slot_index] = cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id;
  }

  /* Store supported proactive commands in recovery cache */
  if (CATI_CONFIG_MODE_ANDROID    == qmi_cati_global_ptr->config_mode ||
      CATI_CONFIG_MODE_CUSTOM_RAW == qmi_cati_global_ptr->config_mode)
  {
    (void)qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                              type_of_command,
                                              slot_index,
                                              TRUE);
  }

  switch(type_of_command)
  {
    case GSTK_CMD_STK_DISPLAY_TEXT:
      cati_event.mask = CATI_REPORT_STATUS_EV_DISPLAY_TEXT;

      /* Check if any client is registered for handling this event */
      if (!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_REPORT_STATUS_EV_DISPLAY_TEXT))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_DISPLAY_TEXT]);
      }
      break;

    case GSTK_CMD_STK_GET_INKEY:
      cati_event.mask = CATI_REPORT_STATUS_EV_GET_INKEY;
      break;

    case GSTK_CMD_STK_GET_INPUT:
      cati_event.mask = CATI_REPORT_STATUS_EV_GET_INPUT;
      break;

    case GSTK_CMD_STK_SET_UP_MENU:
      cati_event.mask = CATI_REPORT_STATUS_EV_SETUP_MENU;

      /* Check if any client is registered for handling this event */
      if (!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_REPORT_STATUS_EV_SETUP_MENU))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_SETUP_MENU]);
      }
      break;

    case GSTK_CMD_STK_SELECT_ITEM:
      cati_event.mask = CATI_REPORT_STATUS_EV_SELECT_ITEM;
      break;

    case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
      cati_event.mask = CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID;
      break;

    case GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
      cati_event.mask = CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT;

      /* Check if any client is registered for handling this event */
      if (!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_REPORT_STATUS_EV_SETUP_IDL_MODE_TEXT))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_SETUP_IDLE_MODE_TEXT]);
      }
      break;

    case GSTK_CMD_STK_LANG_NOTIFICATION:
      cati_event.mask = CATI_REPORT_STATUS_EV_LANG_NOTIFICATION;
      break;

    case GSTK_CMD_STK_PLAY_TONE:
      cati_event.mask = CATI_REPORT_STATUS_EV_PLAY_TONE;
      break;

    case GSTK_CMD_STK_SET_UP_CALL:
      cati_event.mask = CATI_REPORT_STATUS_EV_SETUP_CALL;

      /* display_alpha_conf can be set by NV
         default values (if NV is not set):
           - ANDROID: CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE
                      -- due to limitation in Android: UI has no knowledge if icon
                      is being displayed, assume icon display successfully.
           - CUSTOM_RAW: CATI_DISPLAY_ALPHA_AUTOMATIC_FALSE
                         -- since WP doesn't support icon display.
           - Others: CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF */
      if(cat_msg_ptr->data.raw_proactive_cmd.raw_hdr_tag == GSTK_TAG_DISPLAY_ALPHA
         && qmi_cati_global_ptr->display_alpha_conf != CATI_DISPLAY_ALPHA_NO_AUTOMATIC_CONF)
      {
        gstk_status = gstk_send_icon_display_rsp(
                     qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                     cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                     (qmi_cati_global_ptr->display_alpha_conf == CATI_DISPLAY_ALPHA_AUTOMATIC_TRUE) ? TRUE : FALSE
                     );
        if (gstk_status != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_1("failed to send cmd to GSTK: gstk_status = status %d",
                        gstk_status);
        }

        /* Reset last_gstk_cmd, ref_command_id, full_func_evt */
        qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
        qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
        qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;
        return;
      }
      break;

    case GSTK_CMD_STK_SEND_DTMF:
      cati_event.mask = CATI_REPORT_STATUS_EV_SEND_DTMF;
      break;

    case GSTK_CMD_STK_LAUNCH_BROWSER:
      cati_event.mask = CATI_REPORT_STATUS_EV_LAUNCH_BROWSER;
      break;

    case GSTK_CMD_STK_SEND_SS:
      cati_event.mask = CATI_REPORT_STATUS_EV_SEND_SS;
      break;

    case GSTK_CMD_STK_USSD:
      cati_event.mask = CATI_REPORT_STATUS_EV_SEND_USSD;
      break;

    case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
      cati_event.mask = CATI_PROVIDE_LOCAL_INFO_MASK;

      /* Check if any client is registered for handling this event */
      if(!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_PROVIDE_LOCAL_INFO_MASK))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_PROVIDE_LOCAL_INFO]);
      }
      break;

    case GSTK_CMD_STK_SET_UP_EVENT_LIST:
      cati_event.mask = CATI_SETUP_EVT_LIST_MASK;

      /* Check if any client is registered for handling this event */
      if (!(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_SETUP_EVT_LIST_MASK))
      {
        /* If no client reg for this save it for future and send it from the cache
           Fall through if caching failed so that failure TR can be sent back to GSTK
         */
        if (QMI_ERR_NONE == qmi_cat_cache_raw_proactive_command(cat_msg_ptr,
                                                                type_of_command,
                                                                slot_index,
                                                                FALSE))
        {
          return;
        }
      }
      else
      {
        /* Free and Invalidate the cache for the evt to be sure */
        QMI_CAT_FREE_IF_NOT_NULL(qmi_cati_global_ptr->raw_cache[slot_index][CATI_CACHED_EVT_SETUP_EVT_LIST]);
      }
      break;

    case GSTK_CMD_STK_OPEN_CHANNEL:
      cati_event.mask = CATI_REPORT_STATUS_EV_DATA_EVT;
      cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_OPEN_CHANNEL;
      break;

    case GSTK_CMD_STK_CLOSE_CHANNEL:
      cati_event.mask = CATI_REPORT_STATUS_EV_DATA_EVT;
      cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_CLOSE_CHANNEL;
      break;

    case GSTK_CMD_STK_RECEIVE_DATA:
      cati_event.mask = CATI_REPORT_STATUS_EV_DATA_EVT;
      cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_RECEIVE_DATA;
      break;

    case GSTK_CMD_STK_SEND_DATA:
      cati_event.mask = CATI_REPORT_STATUS_EV_DATA_EVT;
      cati_event.cati_evt_rpt.cati_raw_pc_evt.data_evt_type = CATI_DATA_EVENT_SEND_DATA;
      break;

    case GSTK_CMD_STK_ACTIVATE:
      cati_event.mask = CATI_REPORT_STATUS_EV_ACTIVATE;
      break;

    case GSTK_CMD_STK_REFRESH:
      cati_event.mask = CATI_REPORT_STATUS_EV_REFRESH;
      break;

    default:
      UIM_MSG_ERR_0("Received unhandled Raw Proactive cmd");
      return;
  }

  UIM_MSG_MED_2("Proactive Cmd in process - 0x%x, qmi_cat card state - 0x%x",
                type_of_command, qmi_cati_global_ptr->card_state[slot_index]);

  if(qmi_cati_global_ptr->card_state[slot_index] == QMI_CAT_CARD_STATE_PRESENT)
  {
    /* Copy the raw command */
    cati_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id = cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id;
    cati_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len = (uint16)cat_msg_ptr->data.raw_proactive_cmd.raw_data_len;
    cati_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_ptr = cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer;

    /* Generate event report ind for clients registered for the proactive cmd */
    for (i = 0; i < CATI_MAX_CLIDS && !ind_sent; i++)
    {
      cl_sp = (qmi_cati_client_state_type*)qmi_cat_state_ptr->client[i];
      /* check if this service instance is bound to the qmi device */
      if(NULL == cl_sp)
      {
        continue;
      }

      /* Dont send to ALL clients just the one that registered for this */
      if( (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
          (cl_sp->evt_report_mask[slot_index] & cati_event.mask ) )
      {
        qmi_cati_event_report_ind(cl_sp->common.clid,
                                  &cati_event);
        ind_sent = TRUE;
      }
    }
  }

  /* Send a TR if command is not sent to QMI client for any reason
     and
     The command being processed is not a cached UICC proactive command
     whose processing has already ended
   */
  if (!ind_sent &&
      qmi_cati_global_ptr->ref_command_id[slot_index] ==
      cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id)
  {
    UIM_MSG_HIGH_0("Indication not sent... sending TR or confirmation");

    /* Reset last last_gstk_cmd,  ref_command_id to filter duplicate TR */
      qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
      qmi_cati_global_ptr->ref_command_id[slot_index] = 0;

    /* if the event is full functionality event send error TR */
    if(qmi_cati_global_ptr->full_func_evt[slot_index])
    {
      qmi_cati_global_ptr->full_func_evt[slot_index] = FALSE;
      gstk_status = qmi_cati_send_raw_term_response(cat_msg_ptr->data.raw_proactive_cmd.slot_id,
                                                    cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                                                    cat_msg_ptr->data.raw_proactive_cmd.raw_data_len,
                                                    cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                                                    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_0 ("QMI-CAT : TR not sent!");
      }
    }
    else
    {
      switch (type_of_command)
      {
        case GSTK_CMD_STK_CLOSE_CHANNEL:
        case GSTK_CMD_STK_RECEIVE_DATA:
        case GSTK_CMD_STK_SEND_DATA:
        case GSTK_CMD_STK_REFRESH:
        case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
        case GSTK_CMD_STK_SEND_DTMF:
        case GSTK_CMD_STK_SEND_SS:
        case GSTK_CMD_STK_USSD:
          /* Send icon display confirmation for above commands */
          gstk_status = gstk_send_icon_display_rsp(qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                   cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                                                   FALSE );
          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0 ("QMI-CAT : Confirmation not sent!");
          }
          break;
        case GSTK_CMD_STK_OPEN_CHANNEL:
        case GSTK_CMD_STK_SET_UP_CALL:
          /* Send user confirmation for open channel and setup call */
          gstk_status = gstk_send_user_cnf_alpha_and_icon_disp_rsp(qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                                   cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                                                                   FALSE,
                                                                   FALSE);
          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0 ("QMI-CAT : Confirmation not sent!");
          }
          break;
        default:
          gstk_status = qmi_cati_send_raw_term_response(cat_msg_ptr->data.raw_proactive_cmd.slot_id,
                                                        cat_msg_ptr->data.raw_proactive_cmd.cmd_ref_id,
                                                        cat_msg_ptr->data.raw_proactive_cmd.raw_data_len,
                                                        cat_msg_ptr->data.raw_proactive_cmd.raw_data_buffer,
                                                        GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND);
          if (gstk_status != GSTK_SUCCESS)
          {
            UIM_MSG_ERR_0 ("QMI-CAT : TR not sent!");
          }
          break;
      }
    }
  }
} /* qmi_cat_process_raw_proactive_command */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_ENVELOPE_RESPONSE()

  DESCRIPTION
    Process the raw envelope response from GSTK in QMI context

  PARAMETERS
    user_data  : User data
    env_result : Envelope result

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_envelope_response
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  const gstk_cmd_from_card_type * env_rsp_ptr      = NULL;
  qmi_cmd_buf_type *              cmd_buf_p        = NULL;
  dsm_item_type *                 response         = NULL;
  qmi_error_e_type                errval           = QMI_ERR_NONE;
  qmi_result_e_type               result           = QMI_RESULT_SUCCESS;
  boolean                         retval           = TRUE;
  uint8                           clid             = QMI_SVC_CLID_UNUSED;
  uint32                          client_reg_count = 0;
  qmi_cati_userdata_type *        cb_userdata_ptr  = NULL;
  qmi_cati_cmd_val_e_type         cmd_id           = CATI_CMD_VAL_WIDTH;
  qmi_cati_ind_token_info_type    ind_token        = {FALSE, 0};

  ASSERT(cat_msg_ptr != NULL);

  UIM_MSG_HIGH_0("Sending response for raw env rsp");

  /* First set the response pointer */
  env_rsp_ptr = &(cat_msg_ptr->data.envelope_rsp.envelope_rsp);

  /* Retrieve userdata */
  cb_userdata_ptr = (qmi_cati_userdata_type *)cat_msg_ptr->data.envelope_rsp.user_data;
  if (cb_userdata_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Null userdata ptr in response");
    return;
  }

  /* Retrieve info */
  cmd_id           = cb_userdata_ptr->request_id;
  clid             = cb_userdata_ptr->clid;
  client_reg_count = cb_userdata_ptr->client_reg_count;
  cmd_buf_p        = (qmi_cmd_buf_type*)cb_userdata_ptr->cmd_buf_ptr;
  ind_token        = cb_userdata_ptr->ind_token;

  /* Free userdata since it is no longer needed */
  qmi_cat_free_cb_userdata(cb_userdata_ptr);
  cb_userdata_ptr = NULL;

  /* Check for client's command buffer pointers for response */
  if (!ind_token.is_valid &&
      (cmd_buf_p == NULL ||  cmd_buf_p->x_p == NULL || cmd_buf_p->x_p->cl_sp == NULL))
  {
    UIM_MSG_HIGH_0("Null cmd_buf pointers in response");
    return;
  }

  /* Check if the requested client is still valid */
  if (qmi_cat_check_clid(clid, client_reg_count) == FALSE)
  {
    UIM_MSG_MED_0("Invalid Client, not sending a response");
    return;
  }

  /* Build the response from back to front */
  if (GSTK_ENVELOPE_CMD_SUCCESS == env_rsp_ptr->cmd.raw_envelope_rsp.general_resp)
  {
    errval = qmi_cat_response_raw_envelope(env_rsp_ptr, &response, ind_token);
    UIM_MSG_HIGH_1("Sending response for raw env rsp: errval 0x%x", errval);
  }
  else if (GSTK_ENVELOPE_CMD_CARD_BUSY == env_rsp_ptr->cmd.raw_envelope_rsp.general_resp)
  {
    errval = QMI_ERR_DEVICE_IN_USE;
  }
  else if (GSTK_ENVELOPE_CMD_FAIL == env_rsp_ptr->cmd.raw_envelope_rsp.general_resp)
  {
    errval = QMI_ERR_GENERAL;
  }

  /* Send response */
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  if(FALSE == retval)
  {
    dsm_free_packet(&response);
    return;
  }

  if(ind_token.is_valid == FALSE)
  {
    if (FALSE == qmi_cati_send_response(cmd_buf_p,
                                        response))
    {
      UIM_MSG_MED_0 ("Unable to send response for SEND ENV CMD");

      /* No need to free ind, taken care of in make_message() or send() */
      dsm_free_packet(&response);
      ds_qmi_fw_free_cmd_buf( &cmd_buf_p );
    }
  }
  else
  {
    (void)qmi_cati_send_indication(clid, cmd_id, response);
  }
} /* qmi_cat_process_envelope_response */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_DECODED_ENVELOPE_RESPONSE()

  DESCRIPTION
    To process the decoded envelope command response from GSTK in QMI context

  PARAMETERS
   qmi_cat_message_type : Pointer to cat message

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_process_decoded_envelope_response
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  const gstk_cmd_from_card_type  * decoded_env_rsp_ptr     = NULL;
  qmi_cmd_buf_type               * cmd_buf_p               = NULL;
  dsm_item_type                  * response_ptr            = NULL;
  qmi_error_e_type                 errval                  = QMI_ERR_NONE;
  qmi_result_e_type                result                  = QMI_RESULT_SUCCESS;
  uint8                            clid                    = QMI_SVC_CLID_UNUSED;
  uint32                           client_reg_count        = 0;
  qmi_cati_userdata_type         * cb_userdata_ptr         = NULL;
  qmi_cati_cmd_val_e_type          cmd_id                  = CATI_CMD_VAL_WIDTH;
  qmi_cati_ind_token_info_type     ind_token               = {FALSE, 0};

  ASSERT(cat_msg_ptr != NULL);

  UIM_MSG_HIGH_0("Sending response for decoded env rsp");

  /* First set the response pointer */
  decoded_env_rsp_ptr = &(cat_msg_ptr->data.decoded_envelope_rsp.envelope_rsp);

  /* Retrieve userdata */
  cb_userdata_ptr = (qmi_cati_userdata_type *)decoded_env_rsp_ptr->hdr_cmd.user_data;
  if (cb_userdata_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Null userdata ptr in response");
    return;
  }

  /* Retrieve info */
  cmd_id           = cb_userdata_ptr->request_id;
  clid             = cb_userdata_ptr->clid;
  client_reg_count = cb_userdata_ptr->client_reg_count;
  cmd_buf_p        = (qmi_cmd_buf_type*)cb_userdata_ptr->cmd_buf_ptr;
  ind_token        = cb_userdata_ptr->ind_token;

  /* Free userdata since it is no longer needed */
  qmi_cat_free_cb_userdata(cb_userdata_ptr);
  cb_userdata_ptr = NULL;

  /* Check for client's command buffer pointers */
  if (!ind_token.is_valid &&
      (cmd_buf_p == NULL ||  cmd_buf_p->x_p == NULL || cmd_buf_p->x_p->cl_sp == NULL))
  {
    UIM_MSG_HIGH_0("Null cmd_buf pointers in response");
    return;
  }

  /* Check if the requested client is still valid */
  if (qmi_cat_check_clid(clid, client_reg_count) == FALSE)
  {
    UIM_MSG_MED_0("Invalid Client, not sending a response");
    return;
  }

  /* Determine the type of decoded envelope response
     if indication_token is not set, the raw_envelope_resp for other
     raw env cmd are sent in qmi_cati_decoded_envelope_cmd().
     we should not get a rsp for those raw env cmd here.*/
  if (!ind_token.is_valid &&
      decoded_env_rsp_ptr->hdr_cmd.command_id != GSTK_CC_IND_RSP &&
      decoded_env_rsp_ptr->hdr_cmd.command_id != GSTK_MO_SMS_CTRL_IND_RSP &&
      decoded_env_rsp_ptr->hdr_cmd.command_id != GSTK_SMS_PP_DOWNLOAD_IND_RSP )
  {
    UIM_MSG_ERR_1("qmi_cat_proc_decoded_env_rsp:invalid cmd 0x%x",
                  decoded_env_rsp_ptr->hdr_cmd.command_id);
    errval = QMI_ERR_GENERAL;
    goto send_result;
  }

  UIM_MSG_HIGH_1("qmi_cat_proc_decoded_env_rsp:  general_resp: 0x%x",
                 decoded_env_rsp_ptr->cmd.cc_rsp.general_resp);

  if (decoded_env_rsp_ptr->cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS)
  {
    errval = qmi_cat_response_decoded_envelope(decoded_env_rsp_ptr,&response_ptr,ind_token);
  }
  else if(decoded_env_rsp_ptr->cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_CARD_BUSY)
  {
    errval = QMI_ERR_DEVICE_IN_USE;
  }
  else if(decoded_env_rsp_ptr->cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_FAIL)
  {
    errval = QMI_ERR_GENERAL;
  }
  else
  {
    errval = QMI_ERR_GENERAL;
  }

  /* Send response */
send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  if(FALSE == qmi_svc_put_result_tlv(&response_ptr, result, errval))
  {
    dsm_free_packet(&response_ptr);
    return;
  }

  if(ind_token.is_valid == FALSE)
  {
    if (FALSE == qmi_cati_send_response(cmd_buf_p,
                                       response_ptr))
    {
      UIM_MSG_ERR_0("Unable to send response for decoded envelope");
      dsm_free_packet(&response_ptr);
      ds_qmi_fw_free_cmd_buf( &cmd_buf_p );
    }
  }
  else
  {
    (void)qmi_cati_send_indication(clid, cmd_id, response_ptr);
  }
} /* qmi_cat_process_decoded_envelope_response */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_END_PROACTIVE_SESSION()

  DESCRIPTION
    Process end of proactive command from gstk in QMI context

  PARAMETERS
    proactive_session_end_type : Proactive session end type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_end_proactive_session
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  qmi_cati_client_state_type      * cl_sp      = NULL;
  uint8                             j          = 0;
  uint8                             slot_index = 0;
  qmi_cati_evt_rpt_info_s_type      cati_event;

  ASSERT(cat_msg_ptr != NULL);
  ASSERT(qmi_cati_global_ptr);
  ASSERT(qmi_cat_state_ptr);

  UIM_MSG_HIGH_1("Generating CAT EVT REPORT IND for --- END Proactive Cmd with tag 0x%x ",
                 cat_msg_ptr->data.end_proactive_cmd.tag);

  /* Retrieve the slot index */
  if(qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                                    cat_msg_ptr->data.end_proactive_cmd.slot_id,
                                    &slot_index) != QMI_ERR_NONE)
  {
    UIM_MSG_ERR_1("Invalid slot id for end proactive session command: 0x%x",
                  cat_msg_ptr->data.end_proactive_cmd.slot_id);
    return;
  }

  /* Keep track of end session cmd for checking invalid TR / Evt Conf */
  qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
  qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
  qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;
  qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index] = 0;

  memset(&cati_event, 0, sizeof(qmi_cati_evt_rpt_info_s_type));
  cati_event.mask = CATI_REPORT_STATUS_EV_END_PROACTIVE_SESSION;
  cati_event.cati_evt_rpt.cati_end_proactive_cmd_evt.proactive_session_end_type =
    cat_msg_ptr->data.end_proactive_cmd.proactive_session_end_type;
  cati_event.slot_id = cat_msg_ptr->data.end_proactive_cmd.slot_id;

  /* Generate event report ind for clients registered for the proactive event.
     The same event is used for both raw and decoded format */
  for (j = 0; j < CATI_MAX_CLIDS; j++)
  {
    cl_sp = qmi_cat_state_ptr->client[j];
    if (NULL == cl_sp)
    {
      continue;
    }

    if ((cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
        ((cl_sp->evt_report_mask[slot_index] & cati_event.mask) ||
         (cl_sp->decoded_evt_report_mask[slot_index] & cati_event.mask)))
    {
      qmi_cati_event_report_ind(cl_sp->common.clid,
                                &cati_event);
    }
  }
} /* qmi_cat_process_end_proactive_session */


/*===========================================================================
  FUNCTION QMI_CAT_CMD_RSP_SW1_SW2()

  DESCRIPTION
    To process exclusively the status words messages for command response as reported by GSTK

  PARAMETERS
    cat_msg_ptr: Pointer to data from GSTK

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_term_rsp_sw1_sw2
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  qmi_cmd_buf_type *           cmd_buf_p        = NULL;
  dsm_item_type *              response         = NULL;
  qmi_error_e_type             errval           = QMI_ERR_NONE;
  qmi_result_e_type            result           = QMI_RESULT_SUCCESS;
  boolean                      retval           = TRUE;
  uint8                        clid             = QMI_SVC_CLID_UNUSED;
  uint32                       client_reg_count = 0;
  qmi_cati_userdata_type *     cb_userdata_ptr  = NULL;
  qmi_cati_cmd_val_e_type      cmd_id           = CATI_CMD_VAL_WIDTH;
  qmi_cati_ind_token_info_type ind_token        = {FALSE, 0};

  ASSERT(cat_msg_ptr != NULL);

  UIM_MSG_HIGH_0("Processing Status words for terminal response");

  /* Retrieve userdata */
  cb_userdata_ptr = (qmi_cati_userdata_type *)
    cat_msg_ptr->data.terminal_response_sw1_sw2_info.sw1_sw2_data.hdr_cmd.user_data;
  if (cb_userdata_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Null userdata ptr in response");
    return;
  }

  /* Retrieve info */
  cmd_id           = cb_userdata_ptr->request_id;
  clid             = cb_userdata_ptr->clid;
  client_reg_count = cb_userdata_ptr->client_reg_count;
  cmd_buf_p        = (qmi_cmd_buf_type*)cb_userdata_ptr->cmd_buf_ptr;
  ind_token        = cb_userdata_ptr->ind_token;

  /* Free userdata since it is no longer needed */
  qmi_cat_free_cb_userdata(cb_userdata_ptr);
  cb_userdata_ptr = NULL;

  /* Check for client's command buffer pointers */
  if (!ind_token.is_valid &&
      (cmd_buf_p == NULL ||  cmd_buf_p->x_p == NULL || cmd_buf_p->x_p->cl_sp == NULL))
  {
    UIM_MSG_HIGH_0("Null cmd_buf ptr in response");
    return;
  }

  /* Check if the requested client is still valid */
  if (qmi_cat_check_clid(clid, client_reg_count) == FALSE)
  {
    UIM_MSG_MED_0("Invalid Client, not sending a response");
    return;
  }

  errval = qmi_cat_response_send_tr(
               &response,
               &cat_msg_ptr->data.terminal_response_sw1_sw2_info.sw1_sw2_data.cmd.term_rsp_sw1_sw2_rpt,
               ind_token);

  if (errval != QMI_ERR_NONE)
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
  }

  /* Send response */
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  if(FALSE == retval)
  {
    dsm_free_packet(&response);
    return;
  }

  if(ind_token.is_valid == FALSE)
  {
    if (FALSE == qmi_cati_send_response(cmd_buf_p,
                                        response))
    {
      UIM_MSG_MED_0 ("Unable to send response for SEND TR CMD");

      /* No need to free ind, taken care of in make_message() or send() */
      dsm_free_packet(&response);
      ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    }
  }
  else
  {
    (void)qmi_cati_send_indication(clid, cmd_id, response);
  }
} /* qmi_cat_process_term_rsp_sw1_sw2 */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_TERM_RSP_INDICATION()

  DESCRIPTION
    To process terminal response indication

  PARAMETERS
    cat_msg_ptr: Pointer to data

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_term_rsp_indication
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  dsm_item_type *              response         = NULL;
  qmi_error_e_type             errval           = QMI_ERR_NONE;
  boolean                      retval           = FALSE;
  uint8                        clid             = QMI_SVC_CLID_UNUSED;
  uint32                       client_reg_count = 0;
  qmi_cati_userdata_type *     cb_userdata_ptr  = NULL;
  qmi_cati_cmd_val_e_type      cmd_id           = CATI_CMD_VAL_WIDTH;
  qmi_cati_ind_token_info_type ind_token        = {FALSE, 0};

  ASSERT(cat_msg_ptr != NULL);

  UIM_MSG_HIGH_0("Processing terminal response indication");

  /* Retrieve userdata */
  cb_userdata_ptr = (qmi_cati_userdata_type *)
    cat_msg_ptr->data.terminal_response_rsp.terminal_response_rsp.hdr_cmd.user_data;
  if (cb_userdata_ptr == NULL)
  {
    UIM_MSG_HIGH_0("Null userdata ptr in response");
    return;
  }

  /* Retrieve info */
  cmd_id           = cb_userdata_ptr->request_id;
  clid             = cb_userdata_ptr->clid;
  client_reg_count = cb_userdata_ptr->client_reg_count;
  ind_token        = cb_userdata_ptr->ind_token;

  /* Free userdata since it is no longer needed */
  qmi_cat_free_cb_userdata(cb_userdata_ptr);
  cb_userdata_ptr = NULL;

  /* Check if the requested client is still valid */
  if (qmi_cat_check_clid(clid, client_reg_count) == FALSE)
  {
    UIM_MSG_MED_0("Invalid Client, not sending a response");
    return;
  }

  if (!ind_token.is_valid)
  {
    UIM_MSG_MED_0("indication token is not set");
    return;
  }

  errval = qmi_cat_response_indication_token(&response,
                                             ind_token.token,
                                             cmd_id,
                                             CATI_IND_MSG_TYPE);

  if (errval != QMI_ERR_NONE)
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
  }

  retval = qmi_svc_put_result_tlv(&response, QMI_RESULT_SUCCESS, QMI_ERR_NONE);
  if (FALSE == retval)
  {
    UIM_MSG_MED_0("Unable to generate TR rsp indication!");
    dsm_free_packet(&response);
    return;
  }

  /* Update result TLV and Send indication */
  (void)qmi_cati_send_indication(clid, cmd_id, response);

} /* qmi_cat_process_term_rsp_indication */


/*===========================================================================
  FUNCTION QMI_CATI_DECODED_EVENT_REPORT_IND()

  DESCRIPTION
    Is called when any of the conditions set in set_event_report becomes true
    Sends an indication to the client

  PARAMETERS
    clid               : client id
    gstk_decoded_cmd   : pointer to decoded command
    event_id           : proactive command mask

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_decoded_event_report_ind
(
  uint8                           clid,
  const gstk_cmd_from_card_type * gstk_decoded_cmd,
  uint32                          event_id
)
{
  dsm_item_type    * ind    = NULL;
  qmi_error_e_type   errval = QMI_ERR_NONE;

  ASSERT(gstk_decoded_cmd != NULL);

  ind = NULL;

  if ((event_id & CATI_DECODED_PROACTIVE_CMD_MASK) == 0)
  {
    UIM_MSG_ERR_0("Unsupported mask in qmi_cati_decoded_event_report_ind!");
    return;
  }

  errval = qmi_cat_response_decoded_proactive_command(&ind, gstk_decoded_cmd);

  if (errval != QMI_ERR_NONE)
  {
    UIM_MSG_MED_0("Unable to generate decoded Event Report indication!");
    dsm_free_packet(&ind);
    return;
  }

  (void)qmi_cati_send_indication(clid, CATI_CMD_VAL_SET_EVENT_REPORT, ind);
} /* qmi_cati_decoded_event_report_ind */


/*===========================================================================
  FUNCTION QMI_CATI_SCWS_IND()

  DESCRIPTION
    Is called when scws data event is set in set_event_report becomes true
    Sends an indication to the client

  PARAMETERS
    clid            : client id
    gstk_scws_cmd   : pointer to gstk scws command
    event_id        : SCWS event mask

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_scws_ind
(
  uint8                           clid,
  const gstk_cmd_from_card_type * gstk_scws_cmd,
  uint32                          event_id
)
{
  dsm_item_type                * ind                 = NULL;
  qmi_error_e_type               errval              = QMI_ERR_NONE;
  qmi_cati_cmd_val_e_type        cmd_val             = CATI_CMD_VAL_MIN;
  uint16                         data_len            = 0;
  uint16                         i                   = 0;
  uint8                          num_of_ind          = 0;

  ASSERT(gstk_scws_cmd != NULL);

  ind = NULL;

  if ((event_id & CATI_REPORT_STATUS_EV_DECODED_SCWS_EVT) == 0)
  {
    UIM_MSG_ERR_0("Unsupported mask in qmi_cati_scws_ind!");
    return;
  }

  switch (gstk_scws_cmd->hdr_cmd.command_id)
  {
    case GSTK_SCWS_OPEN_CH_REQ:
      cmd_val = CATI_CMD_VAL_SCWS_OPEN_CH;
      break;

    case GSTK_SCWS_CLOSE_CH_REQ:
      cmd_val = CATI_CMD_VAL_SCWS_CLOSE_CH;
      break;

    case GSTK_SCWS_SEND_DATA_REQ:
      cmd_val = CATI_CMD_VAL_SCWS_SEND_DATA;
      break;

    default:
      /* Unhandled GSTK event */
      errval = QMI_ERR_INTERNAL;
      break;
  }

  if(gstk_scws_cmd->hdr_cmd.command_id == GSTK_SCWS_SEND_DATA_REQ)
  {
    data_len = (uint16)gstk_scws_cmd->cmd.scws_send_data_cmd_req.ch_data.data_len;

    /* number of ind to be sent for scws send data */
    num_of_ind = ((data_len % CAT_TLV_SCWS_SEND_DATA_MAX > 0) ? 1 : 0)
                  + data_len / CAT_TLV_SCWS_SEND_DATA_MAX;

    for(i = 0; i < num_of_ind; i++)
    {
      ind = NULL;
      errval = qmi_cat_response_scws_command(&ind,
                                             gstk_scws_cmd,
                                             i * CAT_TLV_SCWS_SEND_DATA_MAX);

      if (errval != QMI_ERR_NONE)
      {
        UIM_MSG_MED_0("Unable to generate decoded Event Report indication!");
        dsm_free_packet(&ind);
        return;
      }

      (void)qmi_cati_send_indication(clid, cmd_val, ind);
      UIM_MSG_HIGH_0("qmi_cati_scws_ind : sent scws indication");
    }
  }
  else
  {
    errval = qmi_cat_response_scws_command(&ind, gstk_scws_cmd, 0);

    if (errval != QMI_ERR_NONE)
    {
      UIM_MSG_MED_0("Unable to generate decoded Event Report indication!");
      dsm_free_packet(&ind);
      return;
    }

    (void)qmi_cati_send_indication(clid, cmd_val, ind);
    UIM_MSG_HIGH_0("qmi_cati_scws_ind : sent scws indication");
  }
} /* qmi_cati_scws_ind */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_DECODED_PROACTIVE_COMMAND()

  DESCRIPTION
    To process the proactive command from GSTK in QMI context

  PARAMETERS

   decoded_cmd : Pointer to decoded command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_process_decoded_proactive_command
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  const gstk_cmd_from_card_type    * decoded_cmd           = NULL;
  qmi_cati_client_state_type * cl_sp                 = NULL;
  uint8                        i                     = 0;
  boolean                      ind_sent              = FALSE;
  uint32                       event_id              = 0;
  gstk_status_enum_type        gstk_status           = GSTK_SUCCESS;
  gstk_cmd_enum_type           terminal_response_cnf = GSTK_CMD_ENUM_NOT_USE;
  boolean                      clone_status          = FALSE;
  uint8                        slot_index            = 0;

  ASSERT(cat_msg_ptr != NULL);
  ASSERT(qmi_cati_global_ptr);
  ASSERT(qmi_cat_state_ptr);

  decoded_cmd = &(cat_msg_ptr->data.decoded_proactive_cmd.decoded_cmd);

  /* Retrieve the slot index */
  if (qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(decoded_cmd->hdr_cmd.sim_slot_id,
                                                      &slot_index) != QMI_ERR_NONE)
  {
    UIM_MSG_ERR_1("process_decoded_proactive_command: fail to covert slot_id, 0x%x",
                  decoded_cmd->hdr_cmd.sim_slot_id);
    return;
  }

  /* Get mask of the proactive command in order to send it to the right client
     For the following proactive commands that support event routing:
       GSTK_SEND_SMS_REQ
       GSTK_SEND_SS_REQ
       GSTK_SEND_USSD_REQ
       GSTK_SETUP_CALL_REQ
       GSTK_SEND_DTMF_REQs
     if the proactive command has more than 1 events that need client confirmation,
     client may change registration while proactive command is still ongoing,
     change full_func_evt only if this is a new UICC proactive command.
   */
  switch(decoded_cmd->hdr_cmd.command_id)
  {
    case GSTK_DISPLAY_TEXT_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_DISPLAY_TEXT;
      terminal_response_cnf = GSTK_DISPLAY_TEXT_CNF;
      break;
    case GSTK_GET_INKEY_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_GET_INKEY;
      terminal_response_cnf = GSTK_GET_INKEY_CNF;
      break;
    case GSTK_GET_INPUT_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_GET_INPUT;
      terminal_response_cnf = GSTK_GET_INPUT_CNF;
      break;
    case GSTK_LAUNCH_BROWSER_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_LAUNCH_BROWSER;
      terminal_response_cnf = GSTK_LAUNCH_BROWSER_CNF;
      break;
    case GSTK_PLAY_TONE_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_PLAY_TONE;
      terminal_response_cnf = GSTK_PLAY_TONE_CNF;
      break;
    case GSTK_SELECT_ITEM_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SELECT_ITEM;
      terminal_response_cnf = GSTK_SELECT_ITEM_CNF;
      break;
    case GSTK_SEND_SMS_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SEND_SMS;
      if (qmi_cati_global_ptr->ref_command_id[slot_index] !=
          decoded_cmd->hdr_cmd.cmd_detail_reference)
      {
        qmi_cati_global_ptr->full_func_evt[slot_index] =
          (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS)? TRUE:FALSE;
      }
      if (qmi_cati_global_ptr->full_func_evt[slot_index])
      {
        terminal_response_cnf = GSTK_SEND_SMS_CNF;
      }
      break;
    case GSTK_SEND_SS_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SEND_SS;
      if (qmi_cati_global_ptr->ref_command_id[slot_index] !=
          decoded_cmd->hdr_cmd.cmd_detail_reference)
      {
        qmi_cati_global_ptr->full_func_evt[slot_index] =
          (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS)? TRUE:FALSE;
      }
      if (qmi_cati_global_ptr->full_func_evt[slot_index])
      {
        terminal_response_cnf = GSTK_SEND_SS_CNF;
      }
      break;
    case GSTK_SEND_USSD_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SEND_USSD;
      if (qmi_cati_global_ptr->ref_command_id[slot_index] !=
          decoded_cmd->hdr_cmd.cmd_detail_reference)
      {
        qmi_cati_global_ptr->full_func_evt[slot_index] =
          (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD)? TRUE:FALSE;
      }
      if (qmi_cati_global_ptr->full_func_evt[slot_index])
      {
        terminal_response_cnf = GSTK_SEND_USSD_CNF;
      }
      break;
    case GSTK_SETUP_CALL_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SETUP_CALL;
      if (qmi_cati_global_ptr->ref_command_id[slot_index] !=
          decoded_cmd->hdr_cmd.cmd_detail_reference)
      {
        qmi_cati_global_ptr->full_func_evt[slot_index] =
          (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
           CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL)? TRUE:FALSE;
      }
      if (qmi_cati_global_ptr->full_func_evt[slot_index])
      {
        terminal_response_cnf = GSTK_SETUP_CALL_CNF;
      }
      break;
    case GSTK_SETUP_MENU_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SETUP_MENU;
      terminal_response_cnf = GSTK_SETUP_MENU_CNF;
      break;
    case GSTK_SETUP_IDLE_TEXT_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SETUP_IDLE_MODE_TEXT;
      terminal_response_cnf = GSTK_SETUP_IDLE_TEXT_CNF;
      break;
    case GSTK_LANG_NOTIFICATION_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_LANG_NOTIFICATION;
      terminal_response_cnf = GSTK_LANG_NOTIFICATION_CNF;
      break;
    case GSTK_SEND_DTMF_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SEND_DTMF;
      if (qmi_cati_global_ptr->ref_command_id[slot_index] !=
          decoded_cmd->hdr_cmd.cmd_detail_reference)
      {
        qmi_cati_global_ptr->full_func_evt[slot_index] =
          (qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] &
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF)? TRUE:FALSE;
      }
      if (qmi_cati_global_ptr->full_func_evt[slot_index])
      {
        terminal_response_cnf = GSTK_SEND_DTMF_CNF;
      }
      break;
    case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_IDL_SCR_NOTIFY;
      /* No TR sent from UI */
      break;
    case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_USER_ACT;
      /* No TR sent from UI */
      break;
    case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_LANGSEL_NOTIFY;
      /* No TR sent from UI */
      break;
  case GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_HCI_CONN;
      /* No TR sent from UI */
      break;
  case GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY:
      event_id = CATI_REPORT_STATUS_EV_DECODED_SETUPEVT_BROWSER_TERM;
      /* No TR sent from UI */
      break;
    case GSTK_PROVIDE_LANG_INFO_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_PROVIDE_LOCAL_INFO_LANG;
      terminal_response_cnf = GSTK_PROVIDE_LANG_INFO_CNF;
      /* No TR sent from UI */
      break;
    case GSTK_OPEN_CH_REQ:
    case GSTK_SEND_DATA_REQ:
    case GSTK_RECEIVE_DATA_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_DATA_EVT;
      /* No TR sent from UI */
      break;
    case GSTK_CLOSE_CH_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_DATA_EVT;
      /* No TR sent from UI */
      break;
    case GSTK_ACTIVATE_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_ACTIVATE;
      break;
    case GSTK_REFRESH_REQ:
      event_id = CATI_REPORT_STATUS_EV_DECODED_REFRESH;
      /* No TR sent from UI */
      break;
    default:
      /* reset last GSTK command if no event_id is set */
      qmi_cati_global_ptr->last_gstk_cmd[slot_index] = GSTK_END_PROACTIVE_CMD_REQ;
      break;
  }

  /* For newly received proactive commands, set last_gstk_cmd, ref_command_id,
     and decoded_setup_evt_in_progress so that those info can be used later to
     correctly determine whether a TR/Event should be rejected or allowed.
     Those 3 variables are cleared when proactive session ends
     Note:
       The above 3 variables should be in sync with the ongoing UICC
       proactive command, therefore, for proactive commands resent from cache,
       do not change any of those variables to avoid incorrectly set the variables
       when the related proactive command has already completed its processing.
   */
  if (FALSE  == cat_msg_ptr->data.decoded_proactive_cmd.is_cached_cmd)
  {
    switch (decoded_cmd->hdr_cmd.command_id)
    {
      case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
      case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
      case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
        /* For decoded format, setup events for the same proactive command
           are received back to back
           Reset decoded_setup_evt_in_progress if this is the first SETUP_EVT_XX
           received for the same proactive command
         */
        if (qmi_cati_global_ptr->last_gstk_cmd[slot_index]  != GSTK_SETUP_EVENT_LIST_REQ ||
            qmi_cati_global_ptr->ref_command_id[slot_index] !=
                  decoded_cmd->hdr_cmd.cmd_detail_reference)
        {
          qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index] = 0;
        }
        qmi_cati_global_ptr->last_gstk_cmd[slot_index] = GSTK_SETUP_EVENT_LIST_REQ;
        qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index]++;
        break;

      default:
        qmi_cati_global_ptr->last_gstk_cmd[slot_index] = decoded_cmd->hdr_cmd.command_id;
        qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index] = 0;
        break;
    }
    qmi_cati_global_ptr->ref_command_id[slot_index] = decoded_cmd->hdr_cmd.cmd_detail_reference;
  }

  if(qmi_cati_global_ptr->card_state[slot_index] == QMI_CAT_CARD_STATE_PRESENT)
  {
    /* loop thru all clients until indication is sent */
    for (i = 0; i < CATI_MAX_CLIDS && !ind_sent; i++)
    {
      cl_sp = qmi_cat_state_ptr->client[i];

      /* check if this service instance is bound to the qmi device */
      if(NULL == cl_sp)
      {
        continue;
      }

      if( (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
          (cl_sp->decoded_evt_report_mask[slot_index] & event_id ) )
      {
        qmi_cati_decoded_event_report_ind(cl_sp->common.clid,
                                          decoded_cmd,
                                          event_id);
        ind_sent = TRUE;
      }
    }
  }

  /* Handle the case where the command is not sent */
  if (!ind_sent)
  {
    gstk_terminal_rsp_extra_param_type  extra_info     = {0};
    gstk_general_result_enum_type       general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    uint8                               cache_index    = 0;

    UIM_MSG_HIGH_2("QMI-CAT: No client to send indication: command_id=0x%x, cmd_detail_reference=0x%x",
                   decoded_cmd->hdr_cmd.command_id,
                   decoded_cmd->hdr_cmd.cmd_detail_reference);

    for (i = 0;
         i < (sizeof(qmi_cat_decoded_cache_cmd_index)/sizeof(qmi_cat_decoded_cache_cmd_index[0]));
         i++)
    {
      /* Don't cache the commands in invalid and error card states, sent error TR */
      if (qmi_cat_decoded_cache_cmd_index[i].command_id == decoded_cmd->hdr_cmd.command_id &&
          qmi_cat_decoded_cache_cmd_index[i].cache_index < CATI_MAX_DECODED_CACHED_EVTS &&
          qmi_cati_global_ptr->card_state[slot_index] != QMI_CAT_CARD_STATE_INVALID &&
          qmi_cati_global_ptr->card_state[slot_index] != QMI_CAT_CARD_STATE_ERROR)
      {
        /* Found valid cache index, cache supported command */
        cache_index = qmi_cat_decoded_cache_cmd_index[i].cache_index;

        /* In case of GSTK_PROVIDE_LANG_INFO_REQ, we read NV to get the default language. If the NV is set,
           we skip the caching of language info & send TR with lang code. Clients expecting it to be cached
           should not set the default language NV item */
        if ((GSTK_PROVIDE_LANG_INFO_REQ != decoded_cmd->hdr_cmd.command_id) ||
            (qmi_cati_read_lang_from_nv(&general_result, &extra_info) == FALSE))
        {
          if (qmi_cati_global_ptr->decoded_cache[slot_index][cache_index])
          {
            qmi_cati_free_decoded_proactive_command(
              &qmi_cati_global_ptr->decoded_cache[slot_index][cache_index]->cached_event);
          }
          else
          {
            qmi_cati_global_ptr->decoded_cache[slot_index][cache_index] =
              (qmi_cati_decoded_cache_type *)uimqmi_malloc(sizeof(qmi_cati_decoded_cache_type));
            if (NULL == qmi_cati_global_ptr->decoded_cache[slot_index][cache_index])
            {
              UIM_MSG_ERR_0("Failed to allocate memory for decoded cache");

              /* Fall through to send TR if the command is not from obsolete cache */
              if (qmi_cati_global_ptr->ref_command_id[slot_index] != decoded_cmd->hdr_cmd.cmd_detail_reference)
              {
                return;
              }
            }
          }

          clone_status = qmi_cati_clone_decoded_proactive_command(
            &qmi_cati_global_ptr->decoded_cache[slot_index][cache_index]->cached_event,
            decoded_cmd);
          if (!clone_status)
          {
            uimqmi_free(qmi_cati_global_ptr->decoded_cache[slot_index][cache_index]);
            qmi_cati_global_ptr->decoded_cache[slot_index][cache_index] = NULL;
          }
        }
        break;
      }/* Found index */
    }

    /* Send error TR or event confirmation if the command is not from obsolete cache */
    if (!clone_status &&
        (qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_DECODED_PULLONLY) &&
        (qmi_cati_global_ptr->ref_command_id[slot_index] == decoded_cmd->hdr_cmd.cmd_detail_reference))
    {
      UIM_MSG_HIGH_0("QMI-CAT: Indication not cloned... send TR");

      /* Reset last_gstk_cmd, ref_command_id, full_func_evt to filter duplicate TR
         when all TRs are received for current proactive command
       */
      if (GSTK_SETUP_EVENT_LIST_REQ == qmi_cati_global_ptr->last_gstk_cmd[slot_index] &&
          qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index] > 0)
      {
        qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index]--;
      }
      /* For all other commands, decoded_setup_evt_in_progress should be 0 */
      if (0 == qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index])
      {
        qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
        qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
        qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;
      }

      /* If the indication is not sent and the event is not cached, send TR */
      if (terminal_response_cnf != GSTK_CMD_ENUM_NOT_USE)
      {
        gstk_additional_info_type           addl_info      = {0};

        gstk_status = gstk_send_terminal_response(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           0,
                           decoded_cmd->hdr_cmd.cmd_detail_reference,
                           decoded_cmd->hdr_cmd.command_number,
                           terminal_response_cnf,
                           general_result,
                           &addl_info,
                           &extra_info);

        UIM_MSG_HIGH_1("QMI-CAT : TR sent out: 0x%x", gstk_status);
      }
      else
      {
        switch (decoded_cmd->hdr_cmd.command_id)
        {
          case GSTK_CLOSE_CH_REQ:
          case GSTK_SEND_DATA_REQ:
          case GSTK_RECEIVE_DATA_REQ:
          case GSTK_SEND_SMS_REQ:
          case GSTK_SEND_SS_REQ:
          case GSTK_SEND_USSD_REQ:
          case GSTK_SEND_DTMF_REQ:
          case GSTK_REFRESH_REQ:
            gstk_status = gstk_send_icon_display_rsp(qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                     decoded_cmd->hdr_cmd.cmd_detail_reference,
                                                     FALSE );
            if (gstk_status != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0 ("QMI-CAT : Confirmation not sent!");
            }
            break;
          case GSTK_OPEN_CH_REQ:
          case GSTK_SETUP_CALL_REQ:
            /* Send user confirmation alpha for open channel and setup call */
            gstk_status = gstk_send_user_cnf_alpha_and_icon_disp_rsp(qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                                     decoded_cmd->hdr_cmd.cmd_detail_reference,
                                                                     FALSE,
                                                                     FALSE);
            if (gstk_status != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0 ("QMI-CAT : Confirmation not sent!");
            }
            break;
          default:
            UIM_MSG_ERR_0("QMI-CAT : TR or Confirmation not sent!");
            break;
        }
      }
    }
  }
} /* qmi_cat_process_decoded_proactive_command */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_DECODED_BIP_STATUS()

  DESCRIPTION
    To process the proactive command from gstk in QMI context

  PARAMETERS

   decoded_cmd : Pointer to decoded command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_process_decoded_bip_status
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  qmi_cati_client_state_type  * cl_sp                 = NULL;
  uint8                         i                     = 0;
  uint8                         slot_index            = 0;
  boolean                       ind_sent              = FALSE;

  ASSERT(cat_msg_ptr != NULL);
  ASSERT(qmi_cati_global_ptr);
  ASSERT(qmi_cat_state_ptr);

  /* Retrieve the slot index */
  if (qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(cat_msg_ptr->data.decoded_bip_status.bip_status.slot_id,
                                                      &slot_index) != QMI_ERR_NONE)
  {
    UIM_MSG_ERR_1("process_decoded_bip_status: fail to covert slot_id, 0x%x",
                  cat_msg_ptr->data.decoded_bip_status.bip_status.slot_id);
    return;
  }

  UIM_MSG_HIGH_2("ch_id 0x%x, qmi_cat card state 0x%x",
                  cat_msg_ptr->data.decoded_bip_status.bip_status.ch_id,
                  qmi_cati_global_ptr->card_state[slot_index]);

  /* Get mask of the proactive command in order to send it to the right client */
  if ((cat_msg_ptr->data.decoded_bip_status.bip_status.ch_id > 0) &&
      (cat_msg_ptr->data.decoded_bip_status.bip_status.ch_id <= QMI_CATI_MAX_BIP_CHANNEL))
  {
    qmi_cati_global_ptr->bip_status_cache[slot_index][cat_msg_ptr->data.decoded_bip_status.bip_status.ch_id - 1]
          = cat_msg_ptr->data.decoded_bip_status.bip_status.bip_status;

    if(qmi_cati_global_ptr->card_state[slot_index] == QMI_CAT_CARD_STATE_PRESENT)
    {
    /* loop thru all clients until indication is sent */
    for (i = 0; i < CATI_MAX_CLIDS && !ind_sent; i++)
    {
      cl_sp = qmi_cat_state_ptr->client[i];

      /* check if this service instance is bound to the qmi device */
      if(NULL == cl_sp)
      {
        continue;
      }

      if( (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
          (cl_sp->decoded_evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_DECODED_DATA_EVT_STATUS))
      {
        qmi_cati_decoded_bip_status_event_report_ind(cl_sp->common.clid,
                                                  &(cat_msg_ptr->data.decoded_bip_status.bip_status));
        ind_sent = TRUE;
      }
    }
  }
  }
} /* qmi_cat_process_decoded_bip_status */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_SCWS_COMMAND()

  DESCRIPTION
    To process the scws command from GSTK in QMI context

  PARAMETERS

   scws_cmd : Pointer to scws command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_process_scws_command
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  const gstk_cmd_from_card_type      * scws_cmd              = NULL;
  qmi_cati_client_state_type         * cl_sp                 = NULL;
  uint8                                i                     = 0;
  boolean                              ind_sent              = FALSE;
  uint32                               event_id              = 0;
  boolean                              clone_status          = FALSE;
  uint8                                slot_index            = 0;
  gstk_status_enum_type                gstk_status           = GSTK_SUCCESS;
  qmi_cmd_buf_type *                   cmd_buf_p             = NULL;
  dsm_item_type *                      response              = NULL;
  qmi_error_e_type                     errval                = QMI_ERR_NONE;
  qmi_result_e_type                    result                = QMI_RESULT_SUCCESS;
  boolean                              retval                = TRUE;
  qmi_cati_cmd_buf_cache_type        * current_node_ptr      = NULL;
  qmi_cati_cmd_buf_cache_type        * tail_ptr              = NULL;
  uint8                                clid                  = QMI_SVC_CLID_UNUSED;
  uint32                               client_reg_count      = 0;
  qmi_cati_userdata_type             * cb_userdata_ptr       = NULL;

  UIM_MSG_HIGH_0("qmi_cat_process_scws_command");

  ASSERT(cat_msg_ptr != NULL);
  ASSERT(qmi_cati_global_ptr);
  ASSERT(qmi_cat_state_ptr);

  scws_cmd = &(cat_msg_ptr->data.scws_cmd.scws_cmd);

  /* Retrieve the slot index */
  if (qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(scws_cmd->hdr_cmd.sim_slot_id,
                                                      &slot_index) != QMI_ERR_NONE)
  {
    UIM_MSG_ERR_2("process_scws_command: fail to covert slot_id, 0x%x for cmd id, 0x%x",
                  scws_cmd->hdr_cmd.sim_slot_id, scws_cmd->hdr_cmd.command_id);
    return;
  }

  /* set last GSTK cmd per slot */
  qmi_cati_global_ptr->last_gstk_cmd[slot_index] = scws_cmd->hdr_cmd.command_id;

  /* Get mask of the scws command */
  switch(scws_cmd->hdr_cmd.command_id)
  {
    case GSTK_SCWS_OPEN_CH_REQ:
    case GSTK_SCWS_CLOSE_CH_REQ:
    case GSTK_SCWS_SEND_DATA_REQ:
      if(qmi_cati_global_ptr->card_state[slot_index] == QMI_CAT_CARD_STATE_PRESENT)
      {
        event_id = CATI_REPORT_STATUS_EV_DECODED_SCWS_EVT;
  
        /* loop thru all clients until indication is sent */
        for (i = 0; i < CATI_MAX_CLIDS && !ind_sent; i++)
        {
          cl_sp = qmi_cat_state_ptr->client[i];
  
          /* check if this service instance is bound to the qmi device */
          if(NULL == cl_sp)
          {
            continue;
          }
  
          if( (cl_sp->common.clid != QMI_SVC_CLID_UNUSED) &&
              (cl_sp->decoded_evt_report_mask[slot_index] & event_id ) )
          {
            qmi_cati_scws_ind(cl_sp->common.clid,
                              scws_cmd,
                              event_id);
            ind_sent = TRUE;
          }
        }
      }

      /* Handle the case where the command is not sent */
      if (!ind_sent)
      {
        /* Don't cache the command if the card state is invalid or error */
        if(qmi_cati_global_ptr->card_state[slot_index] != QMI_CAT_CARD_STATE_INVALID ||
           qmi_cati_global_ptr->card_state[slot_index] != QMI_CAT_CARD_STATE_ERROR)
        {
          UIM_MSG_HIGH_1("QMI-CAT: No client to send indication: 0x%x",
                         scws_cmd->hdr_cmd.command_id);
          if (scws_cmd->hdr_cmd.command_id ==  GSTK_SCWS_OPEN_CH_REQ &&
              qmi_cati_global_ptr->decoded_cache[slot_index][CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL] == NULL)
          {
            qmi_cati_global_ptr->decoded_cache[slot_index][CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL] = (qmi_cati_decoded_cache_type *)uimqmi_malloc(
                                                                                                            sizeof(qmi_cati_decoded_cache_type));
            if (qmi_cati_global_ptr->decoded_cache[slot_index][CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL] == NULL)
            {
              return;
            }
            clone_status = qmi_cati_clone_decoded_proactive_command(
                 &qmi_cati_global_ptr->decoded_cache[slot_index][CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL]->cached_event,
                 scws_cmd);
            if (!clone_status)
            {
              uimqmi_free(qmi_cati_global_ptr->decoded_cache[slot_index][CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL]);
              qmi_cati_global_ptr->decoded_cache[slot_index][CATI_CACHED_EVT_DECODED_SCWS_OPEN_CHANNEL] = NULL;
            }
          }
        }
        if (!clone_status)
        {
          UIM_MSG_HIGH_0("QMI-CAT: Indication not cloned... send open ch rsp");

          /* If the indication is not sent and the event is not cached, send
             error rsp with ch state closed */
          if (scws_cmd->hdr_cmd.command_id == GSTK_SCWS_OPEN_CH_REQ)
          {
            gstk_status = gstk_scws_open_ch_rsp(
                             qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                             scws_cmd->hdr_cmd.cmd_detail_reference,
                             scws_cmd->cmd.scws_open_ch_cmd_req.ch_id,
                             GSTK_SCWS_TCP_IN_CLOSED_STATE);
            if (gstk_status != GSTK_SUCCESS)
            {
              UIM_MSG_ERR_0("QMI-CAT : Rsp not sent!");
            }
            else
            {
              UIM_MSG_HIGH_1("QMI-CAT : Rsp sent out: 0x%x", gstk_status);
            }
            qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
            break;
          }
        }
      }

      /* Store reference command id for scws request */
      qmi_cati_global_ptr->ref_command_id[slot_index] = scws_cmd->hdr_cmd.cmd_detail_reference;
      break;

    case GSTK_SCWS_DATA_AVAIL_RSP_REQ:
      UIM_MSG_HIGH_0("Sending response for scws data avail rsp");

      current_node_ptr = qmi_cati_global_ptr->scws_cmd_buf[slot_index][scws_cmd->cmd.scws_data_avail_rsp_cmd_req.ch_id];
      if(current_node_ptr == NULL)
      {
        UIM_MSG_ERR_1("GSTK_SCWS_DATA_AVAIL_RSP_REQ : no cmd_buf for ch_id 0x%x in cache",
                      scws_cmd->cmd.scws_data_avail_rsp_cmd_req.ch_id);
        return;
      }

      if(current_node_ptr->next_ptr == NULL)
      {
        /* for case that only 1 pending data avail req for given channel id */
        UIM_MSG_HIGH_0("Pending scws data avail rsp = 1");
        tail_ptr = current_node_ptr;
        qmi_cati_global_ptr->scws_cmd_buf[slot_index][scws_cmd->cmd.scws_data_avail_rsp_cmd_req.ch_id] = NULL;
      }
      else
      {
        tail_ptr = current_node_ptr->next_ptr;
        while (tail_ptr->next_ptr != NULL)
        {
          current_node_ptr = tail_ptr;
          tail_ptr = tail_ptr->next_ptr;
        }
      }

      /* Retrieve userdata */
      cb_userdata_ptr = (qmi_cati_userdata_type *)tail_ptr->cb_userdata_ptr;
      if (cb_userdata_ptr == NULL)
      {
        UIM_MSG_HIGH_0("Null userdata ptr in response");
        return;
      }

      /* Retrieve info */
      clid             = cb_userdata_ptr->clid;
      client_reg_count = cb_userdata_ptr->client_reg_count;
      cmd_buf_p        = (qmi_cmd_buf_type*)cb_userdata_ptr->cmd_buf_ptr;

      /* Free userdata since it is no longer needed */
      qmi_cat_free_cb_userdata(cb_userdata_ptr);
      cb_userdata_ptr = NULL;

      /* Check for client's command buffer pointers */
      if (cmd_buf_p == NULL ||  cmd_buf_p->x_p == NULL || cmd_buf_p->x_p->cl_sp == NULL)
      {
        UIM_MSG_HIGH_0("Null cmd_buf pointers in response");
        return;
      }

      /* Check if the requested client is still valid */
      if (qmi_cat_check_clid(clid, client_reg_count) == FALSE)
      {
        UIM_MSG_HIGH_0("Invalid Client, not sending a response");
        return;
      }

      /* Build the response from scws cmd */
      errval = ((scws_cmd->cmd.scws_data_avail_rsp_cmd_req.success) ? QMI_ERR_NONE
                                                                    : QMI_ERR_INTERNAL);

      /* Send response */
      result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                       : QMI_RESULT_FAILURE);
      retval = qmi_svc_put_result_tlv(&response, result, errval);
      if(FALSE == retval)
      {
        dsm_free_packet(&response);
        return;
      }

      if (FALSE == qmi_cati_send_response(cmd_buf_p,
                                         response))
      {
        UIM_MSG_HIGH_0("Unable to send response for SCWS DATA AVAIL CMD");

        /* No need to free ind, taken care of in make_message() or send() */
        dsm_free_packet(&response);
        ds_qmi_fw_free_cmd_buf( &cmd_buf_p );
      }

      /* free the command buffer */
      UIM_MSG_HIGH_0("Free cached scws data avail rsp");
      current_node_ptr->next_ptr = NULL;
      uimqmi_free(tail_ptr);
      tail_ptr = NULL;
      break;

    default:
      UIM_MSG_HIGH_1("unhandled scws cmd 0x%x", scws_cmd->hdr_cmd.command_id);
      qmi_cati_global_ptr->last_gstk_cmd[slot_index] = GSTK_END_PROACTIVE_CMD_REQ;
      return;
  }
} /* qmi_cat_process_scws_command */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_ALLOC_CLID()

  DESCRIPTION
    Process client allocation from QMI Framework

  PARAMETERS
    uim_message_ptr : message to the internal qmi_cat_message
                     containing alloc_clid data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI CAT must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_alloc_clid
(
  qmi_cat_message_type * cat_message_ptr
)
{
  qmi_cati_client_state_type * cl_sp     = NULL;
  qmi_result_e_type            result    = QMI_RESULT_FAILURE;
  uint8                        client_id = QMI_FRAMEWORK_SVC_MAX_CLIENTS;

  ASSERT(cat_message_ptr);
  ASSERT(qmi_cat_state_ptr);

  client_id = cat_message_ptr->data.clid_cb.cmd_hdr.client_id;

  UIM_MSG_HIGH_1( "Process clid allocation for clid:0x%x", client_id );

  if ((client_id == QMI_SVC_CLID_UNUSED ) ||
      (client_id >= QMI_FRAMEWORK_SVC_MAX_CLIENTS ))
  {
    qmi_framework_svc_send_alloc_clid_result_ex(result, &cat_message_ptr->data.clid_cb.cmd_hdr,
                                                QMI_ERR_INVALID_CLIENT_ID);
    return;
  }

  if (cat_message_ptr->data.clid_cb.cmd_hdr.service != QMUX_SERVICE_CAT)
  {
    qmi_framework_svc_send_alloc_clid_result_ex(result, &cat_message_ptr->data.clid_cb.cmd_hdr,
                                                QMI_ERR_INTERNAL);
    return;
  }

  cl_sp = uimqmi_malloc(sizeof(qmi_cati_client_state_type));
  if (cl_sp == NULL)
  {
    qmi_framework_svc_send_alloc_clid_result_ex(result, &cat_message_ptr->data.clid_cb.cmd_hdr,
                                                QMI_ERR_NO_MEMORY);
    return;
  }
  memset(cl_sp, 0, sizeof(qmi_cati_client_state_type));

  qmi_cat_state_ptr->client[client_id - 1] = cl_sp;
  ds_qmi_fw_common_cl_init(qmi_cat_state_ptr, (qmi_common_client_state_type *)cl_sp);
  cl_sp->common.clid = client_id;
  cl_sp->instance    = (uint16)cat_message_ptr->data.clid_cb.cmd_hdr.qmi_instance;
  cl_sp->service_id  = cat_message_ptr->data.clid_cb.cmd_hdr.service;

  UIM_MSG_HIGH_1("QMI CAT clid successful for clid:0x%x", cl_sp->common.clid );
  result = QMI_RESULT_SUCCESS;

  /* Notifying the clid allocation back to client via QMI Framework*/
  qmi_framework_svc_send_alloc_clid_result_ex(result, &cat_message_ptr->data.clid_cb.cmd_hdr,
                                              QMI_ERR_NONE);
} /* qmi_cat_process_alloc_clid */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_DEALLOC_CLID()

  DESCRIPTION
    Process client deallocation from QMI Framework

  PARAMETERS
    uim_message_ptr : message to the internal qmi_cat_message
                     containing dealloc_clid data

  RETURN VALUE
    None

  DEPENDENCIES
    QMI CAT must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_dealloc_clid
(
  qmi_cat_message_type * cat_message_ptr
)
{
  qmi_cati_client_state_type * cl_sp     = NULL;
  qmi_result_e_type            result    = QMI_RESULT_FAILURE;
  uint16                       client_id = QMI_FRAMEWORK_SVC_MAX_CLIENTS;

  ASSERT(cat_message_ptr);
  ASSERT(qmi_cat_state_ptr);

  client_id = cat_message_ptr->data.clid_cb.cmd_hdr.client_id;

  UIM_MSG_HIGH_1("QMI_CAT process deallocation for Client_id: 0x%x",
                 client_id );

  if ((client_id == QMI_SVC_CLID_UNUSED ) ||
      (client_id >= QMI_FRAMEWORK_SVC_MAX_CLIENTS ))
  {
    UIM_MSG_ERR_1("Invalid client ID clid:0x%x", client_id );
    qmi_framework_svc_send_dealloc_clid_result_ex(result, &cat_message_ptr->data.clid_cb.cmd_hdr,
                                                  QMI_ERR_INVALID_CLIENT_ID);
    return;
  }

  cl_sp = (qmi_cati_client_state_type *)
           qmi_cat_state_ptr->client[client_id  - 1];

  if (cl_sp != NULL && cl_sp->common.clid == client_id)
  {
    /* Increment Client reg. count when deactivation occurs. This ensures
       that when responses are recevied from GSTK for any pending command
       w.r.t. this client, no response is built */
    qmi_cat_state_ptr->client_reg_count[client_id - 1]++;
    qmi_cati_reset_client(cl_sp);
    q_destroy( &cl_sp->common.x_free_q );
    q_destroy( &cl_sp->common.cmd_free_q );
    uimqmi_free(cl_sp);
    qmi_cat_state_ptr->client[client_id  - 1] = NULL;
    cl_sp = NULL;
    UIM_MSG_HIGH_1("QMI CAT clid deallocation successful for clid:0x%x",
                   client_id );
    result = QMI_RESULT_SUCCESS;
  }
  else
  {
    UIM_MSG_ERR_0("INVALID CLIENT ID" );
  }

  /* Notifying the clid deallocation back to client via QMI Framework*/
  qmi_framework_svc_send_dealloc_clid_result_ex(result, &cat_message_ptr->data.clid_cb.cmd_hdr,
                                                QMI_ERR_NONE);
} /* qmi_cat_process_dealloc_clid */


/*===========================================================================
  FUNCTION  QMI_CAT_IS_COMMAND_DISABLED()

  DESCRIPTION
    Utility function to determine if the command is disabled at compile
    time or not

  PARAMETERS
    cmd_type : QMI CAT command

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cat_is_command_disabled
(
  uint16 cmd_type
)
{
  boolean is_disabled = FALSE;

  if(!(cmd_type != CATI_CMD_VAL_WIDTH
#ifdef REMOVE_QMI_CAT_RESET_V02
     && cmd_type != CATI_CMD_VAL_RESET
#endif /* REMOVE_QMI_CAT_RESET_V02 */
#ifdef REMOVE_QMI_CAT_SET_EVENT_REPORT_V02
     && cmd_type != CATI_CMD_VAL_SET_EVENT_REPORT
#endif /* REMOVE_QMI_CAT_SET_EVENT_REPORT_V02 */
#ifdef REMOVE_QMI_CAT_GET_SERVICE_STATE_V02
     && cmd_type != CATI_CMD_VAL_GET_SERVICE_STATE
#endif /* REMOVE_QMI_CAT_GET_SERVICE_STATE_V02 */
#ifdef REMOVE_QMI_CAT_SEND_TR_V02
     && cmd_type != CATI_CMD_VAL_SEND_TR
#endif /* REMOVE_QMI_CAT_SEND_TR_V02 */
#ifdef REMOVE_QMI_CAT_SEND_ENVELOPE_CMD_V02
     && cmd_type != CATI_CMD_VAL_SEND_ENVELOPE_CMD
#endif /* REMOVE_QMI_CAT_SEND_ENVELOPE_CMD_V02 */
#ifdef REMOVE_QMI_CAT_GET_EVENT_REPORT_V02
     && cmd_type != CATI_CMD_VAL_GET_EVENT_REPORT
#endif /* REMOVE_QMI_CAT_GET_EVENT_REPORT_V02 */
#ifdef REMOVE_QMI_CAT_SEND_DECODED_TR_V02
     && cmd_type != CATI_CMD_VAL_SEND_DECODED_TR
#endif /* REMOVE_QMI_CAT_SEND_DECODED_TR_V02 */
#ifdef REMOVE_QMI_CAT_SEND_DECODED_ENVELOPE_CMD_V02
     && cmd_type != CATI_CMD_VAL_DECODED_ENVELOPE_CMD
#endif /* REMOVE_QMI_CAT_SEND_DECODED_ENVELOPE_CMD_V02 */
#ifdef REMOVE_QMI_CAT_EVENT_CONFIRMATION_V02
     && cmd_type != CATI_CMD_VAL_EVENT_CONFIRMATION_CMD
#endif /* REMOVE_QMI_CAT_EVENT_CONFIRMATION_V02 */
#ifdef REMOVE_QMI_CAT_SCWS_OPEN_CHANNEL_V02
     && cmd_type != CATI_CMD_VAL_SCWS_OPEN_CH
#endif /* REMOVE_QMI_CAT_SCWS_OPEN_CHANNEL_V02 */
#ifdef REMOVE_QMI_CAT_SCWS_CLOSE_CHANNEL_V02
     && cmd_type != CATI_CMD_VAL_SCWS_CLOSE_CH
#endif /* REMOVE_QMI_CAT_SCWS_CLOSE_CHANNEL_V02 */
#ifdef REMOVE_QMI_CAT_SCWS_SEND_DATA_V02
     && cmd_type != CATI_CMD_VAL_SCWS_SEND_DATA
#endif /* REMOVE_QMI_CAT_SCWS_SEND_DATA_V02 */
#ifdef REMOVE_QMI_CAT_SCWS_DATA_AVAILABLE_V02
     && cmd_type != CATI_CMD_VAL_SCWS_DATA_AVAILABLE
#endif /* REMOVE_QMI_CAT_SCWS_DATA_AVAILABLE_V02 */
#ifdef REMOVE_QMI_CAT_SCWS_CHANNEL_STATUS_V02
     && cmd_type != CATI_CMD_VAL_SCWS_CHANNEL_STATUS
#endif /* REMOVE_QMI_CAT_SCWS_CHANNEL_STATUS_V02 */
#ifdef REMOVE_QMI_CAT_GET_TERMINAL_PROFILE_V02
     && cmd_type != CATI_CMD_VAL_GET_TERMINAL_PROFILE
#endif /* REMOVE_QMI_CAT_GET_TERMINAL_PROFILE_V02 */
#ifdef REMOVE_QMI_CAT_SET_CONFIGURATION_V02
     && cmd_type != CATI_CMD_VAL_SET_CONFIGURATION
#endif /* REMOVE_QMI_CAT_SET_CONFIGURATION_V02 */
#ifdef REMOVE_QMI_CAT_GET_CONFIGURATION_V02
     && cmd_type != CATI_CMD_VAL_GET_CONFIGURATION
#endif /* REMOVE_QMI_CAT_GET_CONFIGURATION_V02 */
#ifdef REMOVE_QMI_CAT_GET_CACHED_PROACTIVE_CMD_V02
     && cmd_type != CATI_CMD_VAL_GET_CACHED_PROACTIVE_CMD
#endif /* QMI_CAT_GET_CACHED_PROACTIVE_CMD_REQ_V02 */
    ))
  {
    is_disabled = TRUE;
  }

  return is_disabled;
} /* qmi_cat_is_command_disabled */


/*===========================================================================
  FUNCTION  QMI_CAT_PROCESS_FRAMEWORK_CMD()

  DESCRIPTION
    Generic QMI processing for external commands

  PARAMETERS
    cat_message_ptr : message to the internal qmi_cat_message
                     containing an external command

  RETURN VALUE
    None

  DEPENDENCIES
    QMI CAT must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_framework_cmd
(
  qmi_cat_message_type * cat_message_ptr
)
{
  qmi_framework_msg_hdr_type * fw_msg_hdr_ptr  = NULL;
  dsm_item_type              * sdu             = NULL;
  qmi_cati_state_type        * svc_sp          = NULL;
  qmi_cati_client_state_type * cl_sp           = NULL;
  qmi_svc_cmd_hdlr_type      * cmd_hdlr_ptr    = NULL;
  qmi_transaction_type       * x_p             = NULL;
  qmi_cmd_buf_type           * cmd_buf_p       = NULL;
  dsm_item_type *              response_ptr    = NULL;
  uint16                       cmd             = 0;
  int32                        temp            = 0;
  uint16                       cmd_type        = 0;
  uint16                       sdu_len         = 0;
  uint16                       remaining_bytes = 0;
  qmi_error_e_type             errval          = QMI_ERR_NONE;

  ASSERT(cat_message_ptr);
  ASSERT(cat_message_ptr->data.framework_cmd.sdu);
  ASSERT(qmi_cat_state_ptr);

  fw_msg_hdr_ptr = &cat_message_ptr->data.framework_cmd.cmd_hdr;
  sdu            = cat_message_ptr->data.framework_cmd.sdu;

  if(fw_msg_hdr_ptr->common_hdr.client_id    >  CATI_MAX_CLIDS   ||
     fw_msg_hdr_ptr->common_hdr.client_id    == 0)
  {
    UIM_MSG_ERR_0("Invalid qmi_framework message header");
    dsm_free_packet(&sdu);
    return;
  }

  /* Get a service state and client state pointer */
  svc_sp = qmi_cat_state_ptr;
  cl_sp  = svc_sp->client[fw_msg_hdr_ptr->common_hdr.client_id - 1];

  if(cl_sp == NULL)
  {
    UIM_MSG_ERR_0("Invalid client indicated by qmi_framework");
    dsm_free_packet(&sdu);
    return;
  }

  /* get a transaction container */
  x_p = ds_qmi_fw_get_transaction(((qmi_common_svc_state_type*)svc_sp),
                                  ((qmi_common_client_state_type*)cl_sp));
  if (x_p == NULL)
  {
    UIM_MSG_ERR_0("No transaction container!  Discarding QMI CAT framework message");
    dsm_free_packet(&sdu);
    return;
  }

  /* Get a QMI command buffer to hold the command */
  cmd_buf_p = ds_qmi_fw_new_cmd_buf(x_p);
  if (cmd_buf_p == NULL || cmd_buf_p->x_p == NULL || cmd_buf_p->x_p->cl_sp == NULL)
  {
    UIM_MSG_ERR_1("No QMI command buffer. Discarding service transaction: %d",
                  fw_msg_hdr_ptr->common_hdr.transaction_id);
    dsm_free_packet(&sdu);
    ds_qmi_fw_free_transaction(&x_p);
    return;
  }

  x_p->ctl  = fw_msg_hdr_ptr->msg_ctl_flag;
  x_p->x_id = fw_msg_hdr_ptr->common_hdr.transaction_id;

  /* Parse the incoming service message */
  remaining_bytes = (uint16) dsm_length_packet(sdu);

  /* Pull out the command ID */
  temp = dsm_pull16(&sdu);
  if (temp == -1)
  {
    UIM_MSG_ERR_1("Short QMI CAT svc header!  Discarding transaction %d",
                  fw_msg_hdr_ptr->common_hdr.transaction_id);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  cmd_type = ps_ntohs(temp);

  /* Pull out the length of the input sdu */
  temp = dsm_pull16(&sdu);
  if (temp == -1)
  {
    UIM_MSG_ERR_1("Short QMI CAT svc header!  Discarding transaction %d",
                  fw_msg_hdr_ptr->common_hdr.transaction_id);
    errval = QMI_ERR_INTERNAL;
    goto send_result;
  }
  sdu_len = ps_ntohs(temp);

  /* Framework input sdus have the command and length
     nibbles prepended to them.  Remove these bytes and
     insure we have one and only one input sdu
     remaining bytes must be >= 4 adding the check for lint */
  if(remaining_bytes >=4)
  {
    remaining_bytes -= 4;
  }

  if (sdu_len != remaining_bytes)
  {
    UIM_MSG_ERR_2("Invalid payload length - sdu len:0x%x svc payload len:0x%x",
                  sdu_len, remaining_bytes);
    errval = QMI_ERR_MALFORMED_MSG;
    goto send_result;
  }

  cmd_buf_p->cmd_type = cmd_type;

  /* search through the uim command callbacks for
     the command specified in the incoming sdu */
  cmd_hdlr_ptr = qmi_cati_cmd_callbacks;
  for(cmd = 0; cmd < sizeof(qmi_cati_cmd_callbacks)/sizeof(qmi_cati_cmd_callbacks[0]); cmd++, cmd_hdlr_ptr++)
  {
    if (cmd_buf_p->cmd_type == cmd_hdlr_ptr->cmd_type)
    {
      break;
    }
  }

  if((cmd == sizeof(qmi_cati_cmd_callbacks)/sizeof(qmi_cati_cmd_callbacks[0])) ||
     cmd_hdlr_ptr->request_hdlr == NULL ||
     qmi_cat_is_command_disabled(cmd_hdlr_ptr->cmd_type) != FALSE)
  {
    /* We couldn't find this command in the CAT command callbacks array */
    UIM_MSG_ERR_1("Unrecognized type 0x%x for QMI CAT CMD HDLR",
                  cmd_buf_p->cmd_type);
    errval = QMI_ERR_INVALID_QMI_CMD;
    goto send_result;
  }
  else
  {
    /* We found the specified command in the CAT command callbacks array */
    cmd_buf_p->in_progress = TRUE;
    if((cmd_buf_p->x_p->ctl & QMI_FLAG_MASK_MSGTYPE) == QMI_FLAG_MSGTYPE_CMD)
    {
      UIM_MSG_HIGH_1("Handling cmd:0x%x", cmd_hdlr_ptr->cmd_type);
      response_ptr = cmd_hdlr_ptr->request_hdlr(svc_sp, cmd_buf_p, cl_sp, &sdu);
    }
    else
    {
      UIM_MSG_ERR_2("Unhandled (0x%x cmd) msg type (0x%x)",
                    cmd_buf_p->cmd_type,
                    cmd_buf_p->x_p->ctl & QMI_FLAG_MASK_MSGTYPE);
      errval = QMI_ERR_MALFORMED_MSG;
    }
  }
  cmd_buf_p->in_progress = FALSE;

  if (response_ptr == QMI_SVC_RESPONSE_PENDING)
  {
    UIM_MSG_HIGH_1("Response pending for cmd 0x%x", cmd_buf_p->cmd_type);
    dsm_free_packet(&sdu);
    return;
  }

  send_result:
    UIM_MSG_HIGH_0("No response pending - execute send_response");
    /* If there was some error form the TLV to send. Otherwise
       the response_ptr will be populated by the cmd_hdlr. */
    if(response_ptr == NULL)
    {
      (void)qmi_svc_put_result_tlv(&response_ptr,
                                   QMI_RESULT_FAILURE,
                                   errval);
    }
    if(FALSE == qmi_cati_send_response(cmd_buf_p, response_ptr))
    {
      ds_qmi_fw_free_transaction(&x_p);
    }
    dsm_free_packet(&sdu);

} /* qmi_cat_process_framework_cmd */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_MMGSDI_CNF()

  DESCRIPTION
    To process MMGSDI confirmation

  PARAMETERS
    cat_message_ptr : message to the internal qmi_cat_message
                     containing an external command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_mmgsdi_cnf
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  ASSERT(cat_msg_ptr);
  ASSERT(qmi_cati_global_ptr);

  switch (cat_msg_ptr->data.mmgsdi_cnf.cnf_type)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      qmi_cati_global_ptr->cat_mmgsdi_client_id =
        cat_msg_ptr->data.mmgsdi_cnf.cnf_value.client_id_and_evt_reg_cnf.response_header.client_id;
      break;

    default:
      break;
  }
} /* qmi_cat_process_mmgsdi_cnf() */


/*===========================================================================
  FUNCTION QMI_CAT_CONVERT_MMGSDI_SLOT_ID_TO_SLOT_INDEX()

  DESCRIPTION
    to get slot index from mmgsdi_slot_id_enum_type

  PARAMETERS
    slot_id        : input
    slot_index_ptr : output

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type  qmi_cat_convert_mmgsdi_slot_id_to_slot_index
(
  mmgsdi_slot_id_enum_type  slot_id,
  uint8                    *slot_index_ptr
)
{
  ASSERT(slot_index_ptr);
  ASSERT(qmi_cati_global_ptr);

  if (MMGSDI_SLOT_1 == slot_id && qmi_cati_global_ptr->slot_count >= 1)
  {
    *slot_index_ptr = 0;
  }
  else if (MMGSDI_SLOT_2 == slot_id && qmi_cati_global_ptr->slot_count >= 2)
  {
    *slot_index_ptr = 1;
  }
  else if (MMGSDI_SLOT_3 == slot_id && qmi_cati_global_ptr->slot_count >= 3)
  {
    *slot_index_ptr = 2;
  }
  else
  {
    UIM_MSG_ERR_1("Invalid slot_id=0x%x", slot_id);
    return QMI_ERR_OP_DEVICE_UNSUPPORTED;
  }

  return QMI_ERR_NONE;
}/* qmi_cat_convert_mmgsdi_slot_id_to_slot_index */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_MMGSDI_EVT()

  DESCRIPTION
    To process mmgsdi events

  PARAMETERS
    cat_message_ptr : message to the internal qmi_cat_message
                     containing an external command

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_mmgsdi_evt
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  uint8 slot_index = 0;

  ASSERT(cat_msg_ptr);
  ASSERT(qmi_cati_global_ptr);

  if (MMGSDI_MAX_SLOT_ID_ENUM == cat_msg_ptr->data.mmgsdi_evt.slot                                ||
      (MMGSDI_SLOT_1 == cat_msg_ptr->data.mmgsdi_evt.slot && qmi_cati_global_ptr->slot_count < 1) ||
      (MMGSDI_SLOT_2 == cat_msg_ptr->data.mmgsdi_evt.slot && qmi_cati_global_ptr->slot_count < 2) ||
      (MMGSDI_SLOT_3 == cat_msg_ptr->data.mmgsdi_evt.slot && qmi_cati_global_ptr->slot_count < 3))
  {
    return;
  }

  if (QMI_ERR_NONE != qmi_cat_convert_mmgsdi_slot_id_to_slot_index(
                        cat_msg_ptr->data.mmgsdi_evt.slot,
                        &slot_index) ||
      slot_index >= QMI_CAT_MAX_CARD_COUNT )
  {
    return;
  }

  switch (cat_msg_ptr->data.mmgsdi_evt.evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
      qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_PRESENT;

      /* Re-queue the cached proactive commands received before the card Insert event to process
         and send them to client */
      if(qmi_cati_global_ptr->evt_reg_mask[slot_index] & CATI_CACHED_EVT_MASK)
      {
        qmi_cat_send_buffered_raw_proactive_cmd(qmi_cati_global_ptr->evt_reg_mask[slot_index],
                                                slot_index);
      }
      if(qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index] & CATI_DECODED_CACHED_EVT_MASK)
      {
        qmi_cat_send_buffered_decoded_proactive_cmd(qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index],
                                                    slot_index);
      }
      if (qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index] & QMI_CAT_DEC_EVT_REPORT_REQ_BEARER_INDEPENDENT_PROTOCOL_STATUS_MASK_V02)
      {
        qmi_cat_send_buffered_bip_status(slot_index);
      }
      break;

    case MMGSDI_PROFILE_SWITCHING_EVT:
      qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_INVALID;

      /* Re-queue the cached proactive commands received before the profile switch event to send TR to GSTK */
      qmi_cat_send_buffered_raw_proactive_cmd(qmi_cati_global_ptr->evt_reg_mask[slot_index],
                                              slot_index);
      qmi_cat_send_buffered_decoded_proactive_cmd(qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index],
                                                  slot_index);
      /* Send end BIP session to the clients */
      qmi_cat_end_bip_session(slot_index);
      break;

    case MMGSDI_CARD_ERROR_EVT:
    case MMGSDI_CARD_REMOVED_EVT:
        qmi_cat_end_bip_session(slot_index);
        qmi_cat_cleanup_globals(slot_index);
      qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_ERROR;
      break;

    default:
      /* Never gets here */
      break;
  }
} /* qmi_cat_process_mmgsdi_evt() */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_RECOVERY_COMPLETE()

  DESCRIPTION
    To process card recovery events

  PARAMETERS
    cat_msg_ptr : qmi_cat message pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_recovery_complete
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  uint8  slot_index = 0;

  ASSERT(cat_msg_ptr);
  ASSERT(qmi_cati_global_ptr);

  if (QMI_ERR_NONE == qmi_cat_convert_mmgsdi_slot_id_to_slot_index(
                            cat_msg_ptr->data.recovery_ind.slot,
                            &slot_index))
  {
    qmi_cati_card_state_e_type card_state =
                  qmi_cati_global_ptr->card_state[slot_index];

    qmi_cat_end_bip_session(slot_index);
    qmi_cat_cleanup_globals(slot_index);
    qmi_cati_global_ptr->card_state[slot_index] = card_state;
  }
} /* qmi_cat_process_recovery_complete() */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_TP_DL_IND()

  DESCRIPTION
    To process GSTK TP DL indication

  PARAMETERS
    cat_msg_ptr : qmi_cat message pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_tp_dl_ind
(
  const qmi_cat_message_type * cat_msg_ptr
)
{
  uint8  slot_index = 0;

  ASSERT(cat_msg_ptr);
  ASSERT(qmi_cati_global_ptr);

  if (QMI_ERR_NONE == qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                        cat_msg_ptr->data.tp_dl_ind.slot_id,
                        &slot_index))
  {
    /* for 2G card, when TP DL ind comes after mmgsdi card inserted evt,
       we need store the card state and restore the value after globals cleanup */
    qmi_cati_card_state_e_type card_state = qmi_cati_global_ptr->card_state[slot_index];

    qmi_cat_end_bip_session(slot_index);
    qmi_cat_cleanup_globals(slot_index);
    qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_INIT_IN_PROGRESS;

    if(card_state == QMI_CAT_CARD_STATE_PRESENT)
    {
      qmi_cati_global_ptr->card_state[slot_index] = QMI_CAT_CARD_STATE_PRESENT;
    }
  }
} /* qmi_cat_process_tp_dl_ind() */


/*===========================================================================
  FUNCTION QMI_CAT_PROCESS_MESSAGE()

  DESCRIPTION
    Process the messages in DCC task

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cat_process_message
(
  void
)
{
  qmi_cat_message_type * cat_msg_ptr = NULL;

  cat_msg_ptr = qmi_cat_get_message();
  if (cat_msg_ptr == NULL)
  {
    UIM_MSG_ERR_0("No message in QMI CAT queue!!!");
    return;
  }

  switch(cat_msg_ptr->message_type)
  {
    case QMI_CAT_MESSAGE_RAW_PROACTIVE_COMMAND:
      qmi_cat_process_raw_proactive_command(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_DECODED_PROACTIVE_COMMAND:
      qmi_cat_process_decoded_proactive_command(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_DECODED_BIP_STATUS:
      qmi_cat_process_decoded_bip_status(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_SCWS_COMMAND:
      qmi_cat_process_scws_command(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_ENVELOPE_RESPONSE:
      qmi_cat_process_envelope_response(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_DECODED_ENVELOPE_RESPONSE:
      qmi_cat_process_decoded_envelope_response(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_END_PROACTIVE_SESSION:
      qmi_cat_process_end_proactive_session(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_TERM_RSP_SW1_SW2:
      qmi_cat_process_term_rsp_sw1_sw2(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_TERM_RSP_INDICATION:
      qmi_cat_process_term_rsp_indication(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_ALLOC_CLID:
      qmi_cat_process_alloc_clid(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_DEALLOC_CLID:
      qmi_cat_process_dealloc_clid(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_FRAMEWORK_CMD:
      qmi_cat_process_framework_cmd(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_MMGSDI_CNF:
      qmi_cat_process_mmgsdi_cnf(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_MMGSDI_EVT:
      qmi_cat_process_mmgsdi_evt(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_RECOVERY_COMPLETE:
      qmi_cat_process_recovery_complete(cat_msg_ptr);
      break;
    case QMI_CAT_MESSAGE_TP_DL_IND:
      qmi_cat_process_tp_dl_ind(cat_msg_ptr);
      break;
    default:
      UIM_MSG_ERR_1("Unhandled CAT message: 0x%x", cat_msg_ptr->message_type);
      break;
  }

  qmi_cat_free_message(cat_msg_ptr);
  cat_msg_ptr = NULL;

} /* qmi_cat_process_message */


/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CATI_RESET()

  DESCRIPTION
    Reset the issuing CAT client's state

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_reset
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *              response = NULL;
  boolean                      retval   = FALSE;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  /*-------------------------------------------------------------------------
    Reset the client's state
  -------------------------------------------------------------------------*/
  qmi_cati_reset_client(cl_sp);

  /*-------------------------------------------------------------------------
    Build the response from back to front
    No error possible in reset, so result is success
  -------------------------------------------------------------------------*/
  response = NULL;

  retval = qmi_svc_put_result_tlv(&response,
                                  QMI_RESULT_SUCCESS,
                                  QMI_ERR_NONE);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_reset() */


/*===========================================================================
  FUNCTION QMI_CATI_SET_EVENT_REPORT()

  DESCRIPTION
    To set state for event reporting

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_set_event_report
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  qmi_error_e_type                  errval                 = QMI_ERR_NONE;
  qmi_result_e_type                 result                 = QMI_RESULT_SUCCESS;
  boolean                           retval                 = TRUE;
  dsm_item_type *                   response               = NULL;
  qmi_cati_client_state_type      * cat_client_sp          = NULL;
  uint32                            error_raw_mask         = 0;
  uint32                            error_decoded_mask     = 0;
  uint32                            error_full_func_mask   = 0;
  uint64                            disable_full_func_mask = 0;
  uint64                            enable_full_func_mask  = 0;
  gstk_toolkit_cmd_reg_info_type    full_func_reg_list;
  qmi_cat_in_set_event_report_type  tlv_data;
  uint8                             slot_index             = 0;
  gstk_status_enum_type             gstk_status            = GSTK_SUCCESS;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  UIM_MSG_HIGH_0("qmi_cati_set_event_report");

  memset(&tlv_data, 0, sizeof(qmi_cat_in_set_event_report_type));
  memset(&full_func_reg_list, 0, sizeof(gstk_toolkit_cmd_reg_info_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_set_event_report(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Retrieve the client */
  cat_client_sp = (qmi_cati_client_state_type *) cl_sp;

  /* Event report mask for raw format */
  if (tlv_data.raw_mask_present)
  {
    for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
    {
      if (tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
      {
        continue;
      }
      error_raw_mask = ((tlv_data.raw_mask) &
                        (~(cat_client_sp->evt_report_mask[slot_index])) &
                        (qmi_cati_global_ptr->evt_reg_mask[slot_index] |
                         qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index]));
      if(error_raw_mask)
      {
        break;
      }
    }

    if (error_raw_mask)
    {
      /* If some client already reg for this evt */
      errval = QMI_ERR_INVALID_OPERATION;
    }
    else
    {
      uint32 new_reg_mask    = 0;
      uint32 new_common_mask = 0;

      for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
      {
        if (tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
        {
          continue;
        }
        new_common_mask = (qmi_cati_global_ptr->evt_reg_mask[slot_index] | tlv_data.raw_mask);
        new_reg_mask = (tlv_data.raw_mask | cat_client_sp->evt_report_mask[slot_index]);
        cat_client_sp->evt_report_mask[slot_index] = new_reg_mask;
        qmi_cati_global_ptr->evt_reg_mask[slot_index] = new_common_mask;
      }

      /* If the new registration includes setup menu or setup idle mode
         text that were buffered before, queue sending the evt to the control point */
      for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
      {
        if(tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
        {
          continue;
        }
        if (cat_client_sp->evt_report_mask[slot_index] & CATI_CACHED_EVT_MASK)
        {
            qmi_cat_send_buffered_raw_proactive_cmd(cat_client_sp->evt_report_mask[slot_index],
                                                    slot_index);
          }
        }
      }
    }

  /* Event report mask for decoded format */
  if (tlv_data.decoded_mask_present)
  {
    for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
    {
      if(tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
      {
        continue;
      }
      error_decoded_mask = ((tlv_data.decoded_mask) &
                            (~(cat_client_sp->decoded_evt_report_mask[slot_index])) &
                            (qmi_cati_global_ptr->evt_reg_mask[slot_index] |
                             qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index]));
      if(error_decoded_mask)
      {
        break;
      }
    }

    if (error_decoded_mask)
    {
      /* If some client already reg for this evt */
      errval = QMI_ERR_INVALID_OPERATION;
    }
    else
    {
      uint32 new_reg_mask    = 0;
      uint32 new_common_mask = 0;

      for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
      {
        if(tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
        {
          continue;
        }
        new_common_mask = (qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index] | tlv_data.decoded_mask);
        new_reg_mask = (tlv_data.decoded_mask | cat_client_sp->decoded_evt_report_mask[slot_index]);
        cat_client_sp->decoded_evt_report_mask[slot_index] = new_reg_mask;
        qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index] = new_common_mask;
      }

      /* If the new registration includes setup menu or setup idle mode
         text that were buffered before, queue sending the evt to the control point */
      for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
      {
        if(tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
        {
          continue;
        }
        if (cat_client_sp->decoded_evt_report_mask[slot_index] & CATI_DECODED_CACHED_EVT_MASK)
        {
          qmi_cat_send_buffered_decoded_proactive_cmd(cat_client_sp->decoded_evt_report_mask[slot_index],
                                                      slot_index);
        }
        if (cat_client_sp->decoded_evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_DECODED_DATA_EVT_STATUS)
        {
          qmi_cat_send_buffered_bip_status(slot_index);
        }
      }
    }
  }

  /* Event report mask for full function mask*/
  if (tlv_data.full_func_mask_present)
  {
    uint32 new_full_func_common_mask = 0;

    for(slot_index = 0;slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
    {
      if(tlv_data.slot_present && ((tlv_data.slot_mask >> slot_index) & 0x01) == 0)
      {
        continue;
      }

      new_full_func_common_mask = qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index];
      UIM_MSG_HIGH_1("tlv_data.full_func_mask 0x%x", tlv_data.full_func_mask);

      /* Set the enable & disable bitmask for full function registeration to GSTK */
      if ((cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SENDSMS_ALPHA_ID) ||
          (cat_client_sp->decoded_evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_DECODED_SEND_SMS))
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SEND_SMS_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SEND_SMS_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS;
        }
      }
      else
      {
        if ((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS) ==
            CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SMS;
        }
      }

      if (cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SETUP_CALL ||
          cat_client_sp->decoded_evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_DECODED_SETUP_CALL)
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SETUP_CALL_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SETUP_CALL_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL;
        }
      }
      else
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL) ==
           CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SETUP_CALL;
        }
      }

      if (cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SEND_SS ||
          cat_client_sp->decoded_evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_DECODED_SEND_SS)
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SEND_SS_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SEND_SS_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS;
        }
      }
      else
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS) ==
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_SS;
        }
      }

      if (cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SEND_DTMF ||
          cat_client_sp->decoded_evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_DECODED_SEND_DTMF)
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SEND_DTMF_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SEND_DTMF_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF;
        }
      }
      else
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF) ==
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_DTMF;
        }
      }

      if (cat_client_sp->evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_SEND_USSD ||
          cat_client_sp->decoded_evt_report_mask[slot_index] & CATI_REPORT_STATUS_EV_DECODED_SEND_USSD)
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD) == 0)
        {
          disable_full_func_mask = disable_full_func_mask | GSTK_REG_SEND_USSD_EVT;
          new_full_func_common_mask = new_full_func_common_mask & (~CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD);
        }
        else
        {
          enable_full_func_mask = enable_full_func_mask | GSTK_REG_SEND_USSD_EVT;
          new_full_func_common_mask = new_full_func_common_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD;
        }
      }
      else
      {
        if((tlv_data.full_func_mask & CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD) ==
           CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD)
        {
          error_full_func_mask = error_full_func_mask | CATI_REPORT_STATUS_EV_FULL_FUNC_SEND_USSD;
        }
      }

      if (error_full_func_mask != 0)
      {
        errval = QMI_ERR_INVALID_ARG;
        UIM_MSG_ERR_1("QMI-CAT error_full_func_mask 0x%x",
                      error_full_func_mask);
        break;
      }

      if (disable_full_func_mask != 0)
      {
      UIM_MSG_HIGH_1("QMI-CAT disable_full_func_mask 0x%x",
                     disable_full_func_mask);
        gstk_status = gstk_enable_estk_evt_handling((gstk_slot_id_enum_type)tlv_data.slot_mask,
                                       disable_full_func_mask);

        if (GSTK_SUCCESS != gstk_status)
        {
          UIM_MSG_ERR_1("QMI-CAT disable_full_func_mask failed 0x%x",
                        gstk_status);
          errval = QMI_ERR_INTERNAL;
        }
      }
      if (errval != QMI_ERR_NONE)
      {
        break;
      }

      if (enable_full_func_mask != 0)
      {
        UIM_MSG_HIGH_1("QMI-CAT enable_full_func_mask 0x%x",
                       enable_full_func_mask);
        full_func_reg_list.reg_set.class_bit_mask = GSTK_REG_CATEGORY_0_CMD;
        full_func_reg_list.reg_set.cmd_reg_bit_mask = enable_full_func_mask;
        full_func_reg_list.client_func_type = GSTK_HANDLE_ALL_FUNC;

        /* Android mode: everything in raw format */
        if (qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_ANDROID ||
            qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_RAW)
        {
          gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_RAW_FORMAT,
                          1,
                          &full_func_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);

        }
        else if(qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_DECODED ||
                qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED)
        {
          gstk_status = gstk_client_toolkit_cmd_reg(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          0x00,
                          GSTK_GSTK_FORMAT,
                          1,
                          &full_func_reg_list,
                          qmi_cati_gstk_cmd_cb,
                          qmi_cati_gstk_evt_cb);
        }
        else
        {
          // full function capability only supported in Android or Decoded mode.
          errval = QMI_ERR_INVALID_OPERATION;
        }

        if (GSTK_SUCCESS != gstk_status)
        {
          UIM_MSG_ERR_1("QMI-CAT enable_full_func_mask failed 0x%x",
                        gstk_status);
          errval = QMI_ERR_INTERNAL;
        }
      }
      if (errval == QMI_ERR_NONE)
      {
        qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index] = new_full_func_common_mask;
      }
      else
      {
        break;
      }
    }
  }

  /* The response TLV is required only in case of error */
  if (errval != QMI_ERR_NONE)
  {
    if (QMI_ERR_NONE != qmi_cat_response_set_event_report(
                                     &response,
                                     error_raw_mask,
                                     error_decoded_mask,
                                     error_full_func_mask))
    {
      errval = QMI_ERR_NO_MEMORY;
      dsm_free_packet(&response);
    }
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_set_event_report() */


/*===========================================================================
  FUNCTION QMI_CATI_GET_SERVICE_STATE()

  DESCRIPTION
    Gives the status of the device power including battery strength of the
    device and whether the device is externally powered

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_get_service_state
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type              *  response            = NULL;
  qmi_cati_client_state_type *  cat_client_sp       = NULL;
  qmi_error_e_type              errval              = QMI_ERR_NONE;
  qmi_result_e_type             result              = QMI_RESULT_SUCCESS;
  boolean                       retval              = FALSE;
  uint32                        raw_client_mask     = 0;
  uint32                        raw_global_mask     = 0;
  uint32                        decoded_client_mask = 0;
  uint32                        decoded_global_mask = 0;
  uint32                        full_func_mask      = 0;
  uint8                         slot_index          = 0;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  /* Retrieve client */
  cat_client_sp = (qmi_cati_client_state_type *) cl_sp;

  /* Create response */
  for(slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
  {
    raw_global_mask     = raw_global_mask | qmi_cati_global_ptr->evt_reg_mask[slot_index];
    raw_client_mask     = raw_client_mask | cat_client_sp->evt_report_mask[slot_index];
    decoded_global_mask = decoded_global_mask | qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index];
    decoded_client_mask = decoded_client_mask | cat_client_sp->decoded_evt_report_mask[slot_index];
    full_func_mask      = full_func_mask | qmi_cati_global_ptr->full_func_evt_reg_mask[slot_index];

  }

  /* Add TLV to response */
  errval = qmi_cat_response_get_service_state(
                                     &response,
                                     raw_client_mask,
                                     raw_global_mask,
                                     decoded_client_mask,
                                     decoded_global_mask,
                                     full_func_mask);
  if (errval != QMI_ERR_NONE)
  {
    errval = QMI_ERR_NO_MEMORY;
    dsm_free_packet(&response);
  }

  result = (errval == QMI_ERR_NONE) ? QMI_RESULT_SUCCESS
                                    : QMI_RESULT_FAILURE;

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_get_service_state() */


/*===========================================================================
  FUNCTION QMI_CATI_SEND_TR()

  DESCRIPTION
    To send terminal response.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_send_tr
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type                * response         = NULL;
  qmi_error_e_type               errval           = QMI_ERR_NONE;
  qmi_result_e_type              result           = QMI_RESULT_SUCCESS;
  boolean                        retval           = FALSE;
  gstk_status_enum_type          gstk_status      = GSTK_SUCCESS;
  uint8                          slot_index       = 0;
  gstk_generic_data_type         term_resp;
  qmi_cat_in_raw_tr_type         tlv_data;
  qmi_cati_userdata_type       * cb_userdata_ptr  = NULL;
  qmi_cati_ind_token_info_type   ind_token        = {FALSE, 0};

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_raw_tr_type));
  memset(&term_resp, 0, sizeof(gstk_generic_data_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_raw_tr(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* SEND TR to the card in raw format via GSTK */
  term_resp.data_len = tlv_data.term_resp.tr_len;
  term_resp.data_buffer_ptr = (uint8 *) &(tlv_data.term_resp.terminal_response[0]);

  /* Result will be returned via indication if indication token is set*/
  if (tlv_data.ind_token_present)
  {
    ind_token.is_valid = TRUE;
    ind_token.token = tlv_data.ind_token.ind_token;
  }

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  errval = qmi_cati_check_last_gstk_cmd(slot_index, TRUE);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Reject TR if:
     1. cmd_ref_id does not match ref_command_id
   */
  if(tlv_data.term_resp.cmd_ref_id != qmi_cati_global_ptr->ref_command_id[slot_index])
  {
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  /* Create the userdata */
  cb_userdata_ptr = qmi_cat_get_cb_userdata(CATI_CMD_VAL_SEND_TR, cmd_buf_p, ind_token);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  gstk_status = gstk_send_raw_terminal_response(qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                (uint32)cb_userdata_ptr,
                                                tlv_data.term_resp.cmd_ref_id,
                                                term_resp);


  if(gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_1("qmi_cati_send_tr : gstk_status = status %d", gstk_status);
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);

    /* Free userdata since it is no longer needed */
    qmi_cat_free_cb_userdata(cb_userdata_ptr);
    cb_userdata_ptr = NULL;
    goto send_result;
  }

  /* Reset last_gstk_cmd, ref_command_id to filter duplicate TR */
  qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
  qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
  qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;

  if(!ind_token.is_valid && gstk_nv_get_feature_status(
                              GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2,
                              GSTK_SLOT_ID_MAX) == TRUE)
  {
    /* Expect a response for the terminal response comamnd
       With feature GSTK_CFG_FEATURE_REPORT_SW1_SW2 disabled GSTK is not expected to retrun the
       status words for this command */
    return QMI_SVC_RESPONSE_PENDING;
  }
  /* if client expects an IND, post an indication even status word
     result is not expected. In this way, the indication is sent to
     the client after the response for the command*/
  else if(ind_token.is_valid && gstk_nv_get_feature_status(
                                  GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2,
                                  GSTK_SLOT_ID_MAX) == FALSE)
  {
    qmi_cat_message_type * msg_ptr = NULL;

    msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_TERM_RSP_INDICATION);

    if(NULL != msg_ptr)
    {
      msg_ptr->data.terminal_response_rsp.terminal_response_rsp.hdr_cmd.user_data
        = (uint32)cb_userdata_ptr;
      qmi_cat_post_message(msg_ptr);
    }
  }

  if (ind_token.is_valid)
  {
    /* add indication token to the immediate response */
    errval = qmi_cat_response_indication_token(&response,
                                               ind_token.token,
                                               CATI_CMD_VAL_SEND_TR,
                                               CATI_RESP_MSG_TYPE);
    if (QMI_ERR_NONE != errval)
    {
      dsm_free_packet(&response);
    }
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_send_tr() */


/*===========================================================================
  FUNCTION QMI_CATI_SEND_ENVELOPE_CMD()

  DESCRIPTION
    Performs a manual service provisioning with data provided by the user

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_send_envelope_cmd
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type                * response         = NULL;
  qmi_error_e_type               errval           = QMI_ERR_NONE;
  qmi_result_e_type              result           = QMI_RESULT_SUCCESS;
  boolean                        retval           = TRUE;
  gstk_status_enum_type          gstk_status      = GSTK_SUCCESS;
  uint8                          slot_index       = 0;
  qmi_cat_in_raw_envelope_type   tlv_data;
  gstk_generic_data_type         envelope_cmd;
  qmi_cati_userdata_type       * cb_userdata_ptr  = NULL;
  qmi_cati_ind_token_info_type   ind_token        = {FALSE, 0};

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_raw_envelope_type));
  memset(&envelope_cmd, 0, sizeof(gstk_generic_data_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_raw_envelope(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Validate envelope command length */
  if (tlv_data.envelope.env_cmd_len < 2)
  {
    UIM_MSG_HIGH_1("Invalid envelope cmd len:0x%x ",
                   tlv_data.envelope.env_cmd_len);
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* Result will be returned via indication if indication token is set*/
  if (tlv_data.ind_token_present)
  {
    ind_token.is_valid = TRUE;
    ind_token.token = tlv_data.ind_token.ind_token;
  }

  /* Validate if SMS PP envelope command is blocked */
  if (tlv_data.envelope.envelope_data[0] == CATI_SMS_PP_DOWNLOAD_TAG &&
      qmi_cati_global_ptr->sms_pp_envelope_blocked)
  {
    UIM_MSG_HIGH_0("SMS PP DL is blocked for security.");
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  /* Validate the envelope command type */
  if (tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_MENU_SEL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_USER_ACT &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_IDL_SCREEN_AVAIL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_LANG_SEL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_UNKNOWN &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_BROWSER_TERM &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_SEND_CALL_CONTROL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_HCI_CONN &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_SMS_PP_DOWNLOAD &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_MT_CALL &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_CALL_CONNECTED &&
      tlv_data.envelope.env_cmd_type != CATI_ENV_CMD_TYPE_EVT_DL_CALL_DISCONNECTED)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }
  else if( tlv_data.envelope.env_cmd_type == CATI_ENV_CMD_TYPE_SMS_PP_DOWNLOAD &&
           (qmi_cati_global_ptr->sms_pp_envelope_blocked))
  {
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  /* SEND Envelope Cmd to the card in raw format via GSTK */
  envelope_cmd.data_len = tlv_data.envelope.env_cmd_len;
  envelope_cmd.data_buffer_ptr = (uint8 *) &(tlv_data.envelope.envelope_data[0]);

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Create the userdata */
  cb_userdata_ptr = qmi_cat_get_cb_userdata(CATI_CMD_VAL_SEND_ENVELOPE_CMD, cmd_buf_p, ind_token);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  gstk_status = gstk_send_raw_envelope_command ( qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                                 (uint32)cb_userdata_ptr,
                                                 envelope_cmd,
                                                 qmi_cati_gstk_raw_env_cb);

  if (GSTK_SUCCESS != gstk_status)
  {
    UIM_MSG_HIGH_1("qmi_cati_send_envelope_cmd : gstk_status = status %d",
                   gstk_status);
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
    /* Free userdata since it is no longer needed */
    qmi_cat_free_cb_userdata(cb_userdata_ptr);
    cb_userdata_ptr = NULL;
    goto send_result;
  }

  if (!ind_token.is_valid)
  {
    return QMI_SVC_RESPONSE_PENDING;
  }

  /* add indication token to the immediate response */
  errval = qmi_cat_response_indication_token(&response,
                                             ind_token.token,
                                             CATI_CMD_VAL_SEND_ENVELOPE_CMD,
                                             CATI_RESP_MSG_TYPE);
  if (QMI_ERR_NONE != errval)
  {
    dsm_free_packet(&response);
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_send_envelope_cmd() */


/*===========================================================================
  FUNCTION QMI_CATI_RESET_CLIENT()

  DESCRIPTION
    Resets the state for the given client

  PARAMETERS
    cl_sp_in :  client state pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_reset_client
(
  qmi_cati_client_state_type * cl_sp
)
{
  uint8                         slot_index      = 0;

  ASSERT(qmi_cati_global_ptr);

  ds_qmi_fw_common_reset_client((qmi_common_client_state_type *)cl_sp);

  /*-------------------------------------------------------------------------
    Clear the common mask bits specific to this client
  -------------------------------------------------------------------------*/
  for(slot_index = 0; slot_index < QMI_CAT_MAX_CARD_COUNT; slot_index ++)
  {
    qmi_cati_global_ptr->evt_reg_mask[slot_index] =
        ( qmi_cati_global_ptr->evt_reg_mask[slot_index] &
          (~cl_sp->evt_report_mask[slot_index]) );

    qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index] =
        ( qmi_cati_global_ptr->decoded_evt_reg_mask[slot_index] &
          (~cl_sp->decoded_evt_report_mask[slot_index]) );

    /* Clear event reporting */
    cl_sp->evt_report_mask[slot_index]         = CATI_REPORT_STATUS_NONE;
    cl_sp->decoded_evt_report_mask[slot_index] = CATI_REPORT_STATUS_DECODED_NONE;
  }
} /* qmi_cati_reset_client */


/*===========================================================================
  FUNCTION QMI_CATI_GET_EVENT_REPORT()

  DESCRIPTION
    Fetches the last proactive command from the modem

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_get_event_report
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type                    * response         = NULL;
  qmi_error_e_type                   errval           = QMI_ERR_NONE;
  qmi_result_e_type                  result           = QMI_RESULT_SUCCESS;
  boolean                            retval           = FALSE;
  gstk_status_enum_type              gstk_status      = GSTK_SUCCESS;
  uint8                              slot_index       = 0;
  gstk_client_pro_cmd_reg_data_type  proactive_cmd;
  qmi_cat_in_get_event_report_type   tlv_data;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_get_event_report_type));
  memset(&proactive_cmd, 0, sizeof(gstk_client_pro_cmd_reg_data_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_get_event_report(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* cmd_ref_id is no longer the index to GSTK's command_details_ref_table[],
     instead, it is a unique number of uint32, therefore, the check of
     cmd_ref_id against CATI_GSTK_PROACTIVE_COMMANDS_MAX is no longer valid
     and should be removed
   */

  /* Convert the format */
  switch(tlv_data.proactive_command.format)
  {
    case 1:
      proactive_cmd.data_format_type = GSTK_RAW_FORMAT;
      break;
    case 2:
      proactive_cmd.data_format_type = GSTK_GSTK_FORMAT;
      break;
    default:
      errval = QMI_ERR_INVALID_ARG;
      break;
  }
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Allocate memory for command from GSTK */
  proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr =
        (gstk_cmd_from_card_type*)uimqmi_malloc(sizeof(gstk_cmd_from_card_type));
  if (proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  /* Get command from GSTK */
  for (slot_index = 0; slot_index < qmi_cati_global_ptr->slot_count; slot_index++)
  {
    gstk_status = gstk_get_card_event_data(qmi_cati_global_ptr->cat_gstk_client_id_external[slot_index],
                                         tlv_data.proactive_command.cmd_ref_id,
                                         &proactive_cmd);

    if ((GSTK_SUCCESS != gstk_status) && (slot_index + 1) < qmi_cati_global_ptr->slot_count)
    {
      continue;
    }

    if (GSTK_SUCCESS != gstk_status)
    {
      UIM_MSG_HIGH_1("qmi_cati_get_event_report: gstk_status = status 0x%x",
                     gstk_status);

      errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
      uimqmi_free(proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr);
      goto send_result;
    }

    /* RAW format */
    if (proactive_cmd.data_format_type == GSTK_RAW_FORMAT)
    {
      /* get_event_report not supported in RAW FORMAT */
      if ((slot_index + 1) < qmi_cati_global_ptr->slot_count)
      {
        continue;
      }

      errval = QMI_ERR_INVALID_OPERATION;

      uimqmi_free(proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr);
      goto send_result;
    }

    /* GSTK format */
    if (proactive_cmd.data_format_type == GSTK_GSTK_FORMAT)
    {
      if (proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.command_id == GSTK_END_PROACTIVE_CMD_REQ)
      {
        uint8 end_session_type = 0;
        /* Populate End Proactive Session TLV */
        if (proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr->cmd.end_pro_cmd_req.end_from_card)
        {
          end_session_type = CATI_PROACTIVE_SESSION_END_DUE_TO_CARD;
        }
        else
        {
          end_session_type = CATI_PROACTIVE_SESSION_END_INTERNAL;
        }

        errval = qmi_cat_response_end_proactive_session(
                                  &response,
                                  end_session_type,
                                  proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr->hdr_cmd.sim_slot_id);
        if (errval != QMI_ERR_NONE)
        {
          UIM_MSG_HIGH_0("Unable to generate CATI Event Report indication for end session!");
          dsm_free_packet(&response);
        }
      }
      else
      {
      /* Convert GSTK event id to QMI TLV */
        errval = qmi_cat_response_decoded_proactive_command(
                                 &response,
                                 proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr);
        if (errval != QMI_ERR_NONE)
        {
          UIM_MSG_HIGH_0("Unable to generate CATI Event Report indication for end session!");
          dsm_free_packet(&response);
        }
      }

      uimqmi_free(proactive_cmd.pro_cmd_reg_data.gstk_req_rsp_ptr);
    }
    /* exit from for loop if there is a command for slot 1. If there are 2 commands,
       the function will return only the command in slot 1. User need to invoke again
       later for command in slot 2.*/
    break;
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_get_event_report() */


/*===========================================================================
  FUNCTION QMI_CATI_DECODED_ENVELOPE_CMD()

  DESCRIPTION
    Send the Decoded Envelope command to the SIM

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_decoded_envelope_cmd
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *                    response         = NULL;
  qmi_error_e_type                   errval           = QMI_ERR_NONE;
  qmi_result_e_type                  result           = QMI_RESULT_SUCCESS;
  boolean                            retval           = TRUE;
  gstk_status_enum_type              gstk_status      = GSTK_SUCCESS;
  uint8                              slot_index       = 0;
  qmi_cat_in_decoded_envelope_type * tlv_data_ptr     = NULL;
  gstk_evt_dl_ind_type               evt_list;
  gstk_cc_ind_type_ext             * cc_ind_ptr       = NULL;
  gstk_mo_sms_ctrl_ind_ext_type    * cc_sms_ind_ptr   = NULL;
  gstk_address_type                  sms_pp_dl_address;
  gstk_sms_tpdu_type                 sms_tpdu;
  boolean                            is_cdma_sms      = FALSE;
  gstk_slot_id_enum_type             req_slot         = GSTK_SLOT_1;
  qmi_cati_userdata_type           * cb_userdata_ptr  = NULL;
  qmi_cati_ind_token_info_type       ind_token        = {FALSE, 0};

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset((void*)&evt_list, 0, sizeof(gstk_evt_dl_ind_type));
  memset((void*)&sms_pp_dl_address, 0, sizeof(gstk_address_type));
  memset((void*)&sms_tpdu, 0, sizeof(gstk_sms_tpdu_type));

  tlv_data_ptr =
      (qmi_cat_in_decoded_envelope_type *)uimqmi_malloc(sizeof(qmi_cat_in_decoded_envelope_type));

  if (tlv_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("Decoded envelope command dropped.No memory");
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  memset((void*)tlv_data_ptr, 0, sizeof(qmi_cat_in_decoded_envelope_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_decoded_envelope(sdu_in, tlv_data_ptr);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Retrieve the slot index */
  if(tlv_data_ptr->slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data_ptr->slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Result will be returned via indication if indication token is set*/
  if (tlv_data_ptr->ind_token_present)
  {
    ind_token.is_valid = TRUE;
    ind_token.token = tlv_data_ptr->ind_token.ind_token;
  }

  /* Create the userdata */
  cb_userdata_ptr = qmi_cat_get_cb_userdata(CATI_CMD_VAL_DECODED_ENVELOPE_CMD, cmd_buf_p, ind_token);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  switch(tlv_data_ptr->env_hdr.env_cmd_type)
  {
    case CATI_ENV_CMD_TYPE_DECODED_MENU_SEL:
      /* Check mandatory items */
      if (!tlv_data_ptr->item_id_present)
      {
        errval = QMI_ERR_MISSING_ARG;
        goto send_result;
      }

      gstk_status = gstk_send_envelope_menu_sel_command(
                              qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                              (uint32)cb_userdata_ptr,
                              tlv_data_ptr->item_id.item_id,
                              (tlv_data_ptr->help_request_present) ?
                                (boolean)tlv_data_ptr->help_request.help_request :
                                FALSE);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_1("gstk_send_envelope_menu_sel_command : gstk_status = status %d",
                       gstk_status);
        errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
        goto send_result;
      }
      break;

    case CATI_ENV_CMD_TYPE_DECODED_EVT_DL_LANGUAGE:
      /* Check mandatory items */
      if (!tlv_data_ptr->language_present)
      {
        errval = QMI_ERR_MISSING_ARG;
        goto send_result;
      }

      evt_list.evt_type = GSTK_LANG_SELECTION_EVT_IND;

      (void)memscpy(evt_list.evt_ind.lang_sel_evt_ind.lang_code,
                    sizeof(evt_list.evt_ind.lang_sel_evt_ind.lang_code),
                    tlv_data_ptr->language.language,
                    GSTK_LANG_CODE_LEN);

      gstk_status = gstk_send_envelope_evt_download_command(
                                  qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                  (uint32)cb_userdata_ptr,
                                  &evt_list);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_1("gstk_send_envelope_evt_download_command : gstk_status = status %d",
                       gstk_status);
        errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
        goto send_result;
      }
      break;

    case CATI_ENV_CMD_TYPE_DECODED_EVT_USER_ACTIVITY:
      evt_list.evt_type = GSTK_USER_ACTIVITY_EVT_IND;

      gstk_status = gstk_send_envelope_evt_download_command(
                                  qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                  (uint32)cb_userdata_ptr,
                                  &evt_list);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_1("gstk_send_envelope_evt_download_command : gstk_status = status %d",
                       gstk_status);
        errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
        goto send_result;
      }
      break;

    case CATI_ENV_CMD_TYPE_DECODED_EVT_DL_HCI_CONN:
      evt_list.evt_type = GSTK_HCI_CONNECTIVITY_IND;

      gstk_status = gstk_send_envelope_evt_download_command(
                                  qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                  (uint32)cb_userdata_ptr,
                                  &evt_list);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_1("gstk_send_envelope_evt_download_command : gstk_status = status %d",
                       gstk_status);
        errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
        goto send_result;
      }
      break;

    case CATI_ENV_CMD_TYPE_DECODED_EVT_IDLE_SCREEN:
      evt_list.evt_type = GSTK_IDLE_SCRN_EVT_IND;

      gstk_status = gstk_send_envelope_evt_download_command(
                                  qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                  (uint32)cb_userdata_ptr,
                                  &evt_list);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_1("gstk_send_envelope_evt_download_command : gstk_status = status %d",
                       gstk_status);
        errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
        goto send_result;
      }
      break;

    case CATI_ENV_CMD_TYPE_DECODED_SEND_CALL_CONTROL:
      cc_ind_ptr =
         (gstk_cc_ind_type_ext *)uimqmi_malloc(sizeof(gstk_cc_ind_type_ext));

      if (cc_ind_ptr == NULL)
      {
        UIM_MSG_ERR_0("Decoded envelope CC command dropped.No memory");
        errval = QMI_ERR_NO_MEMORY;
        goto send_result;
      }

      memset((void*)cc_ind_ptr, 0, sizeof(gstk_cc_ind_type_ext));

      if((tlv_data_ptr->call_type_present &&
         (tlv_data_ptr->call_type.call_type == CATI_CC_CALL_TYPE_VOICE))||
         (!tlv_data_ptr->call_type_present &&
          tlv_data_ptr->address_present && tlv_data_ptr->subaddress_present &&
          tlv_data_ptr->ccp1_present && tlv_data_ptr->ccp2_present))
      {
        if ((tlv_data_ptr->address.length    > CATI_TLV_ADDRESS_MAX) ||
            (tlv_data_ptr->subaddress.length > CATI_TLV_SUBADDRESS_MAX) ||
            (!tlv_data_ptr->call_type_present &&
             ((tlv_data_ptr->ccp1.length       > CATI_TLV_CCP_MAX) ||
              (tlv_data_ptr->ccp2.length       > CATI_TLV_CCP_MAX))))
        {
          UIM_MSG_ERR_0("qmi_cati_decoded_envelope_cmd:CC voice invalid length");
          errval = QMI_ERR_INVALID_ARG;
          goto send_result;
        }

        cc_ind_ptr->call_enum_type = GSTK_VOICE;
        cc_ind_ptr->gstk_address_string.voice.address.TON =
          (gstk_ton_enum_type) tlv_data_ptr->address.ton;
        cc_ind_ptr->gstk_address_string.voice.address.NPI =
          (gstk_npi_enum_type) tlv_data_ptr->address.npi;
        cc_ind_ptr->gstk_address_string.voice.address.length = tlv_data_ptr->address.length;
        (void)memscpy(cc_ind_ptr->gstk_address_string.voice.address.address,
                      sizeof(cc_ind_ptr->gstk_address_string.voice.address.address),
                      tlv_data_ptr->address.address,
                      tlv_data_ptr->address.length);

        cc_ind_ptr->gstk_address_string.voice.subaddress.length = tlv_data_ptr->subaddress.length;
        (void)memscpy(cc_ind_ptr->gstk_address_string.voice.subaddress.subaddress,
                      sizeof(cc_ind_ptr->gstk_address_string.voice.subaddress.subaddress),
                      tlv_data_ptr->subaddress.subaddress,
                      tlv_data_ptr->subaddress.length);

        cc_ind_ptr->gstk_address_string.voice.ccp1.length = tlv_data_ptr->ccp1.length;
        (void)memscpy(cc_ind_ptr->gstk_address_string.voice.ccp1.ccp,
                      sizeof(cc_ind_ptr->gstk_address_string.voice.ccp1.ccp),
                      tlv_data_ptr->ccp1.ccp,
                      tlv_data_ptr->ccp1.length);

        cc_ind_ptr->gstk_address_string.voice.ccp2.length = tlv_data_ptr->ccp2.length;
        (void)memscpy(cc_ind_ptr->gstk_address_string.voice.ccp2.ccp,
                      sizeof(cc_ind_ptr->gstk_address_string.voice.ccp2.ccp),
                      tlv_data_ptr->ccp2.ccp,
                      tlv_data_ptr->ccp2.length);
      }
      else if ((tlv_data_ptr->call_type_present &&
                (tlv_data_ptr->call_type.call_type == CATI_CC_CALL_TYPE_SS))||
               (!tlv_data_ptr->call_type_present && tlv_data_ptr->address_present &&
                !tlv_data_ptr->subaddress_present && !tlv_data_ptr->ccp1_present
                && !tlv_data_ptr->ccp2_present))
      {
        if (tlv_data_ptr->address.length > CATI_TLV_ADDRESS_MAX)
        {
          UIM_MSG_ERR_1("qmi_cati_decoded_envelope_cmd:CC SS invalid address size 0x%x",
                        tlv_data_ptr->address.length);
          errval = QMI_ERR_INVALID_ARG;
          goto send_result;
        }

        cc_ind_ptr->call_enum_type = GSTK_SS;

        cc_ind_ptr->gstk_address_string.ss_string.TON =
          (gstk_ton_enum_type)tlv_data_ptr->address.ton;
        cc_ind_ptr->gstk_address_string.ss_string.NPI =
          (gstk_npi_enum_type)tlv_data_ptr->address.npi;
        cc_ind_ptr->gstk_address_string.ss_string.length = tlv_data_ptr->address.length;

        (void)memscpy(cc_ind_ptr->gstk_address_string.ss_string.address,
                      sizeof(cc_ind_ptr->gstk_address_string.ss_string.address),
                      tlv_data_ptr->address.address,
                      tlv_data_ptr->address.length);
      }
      else if (((tlv_data_ptr->call_type_present &&
               (tlv_data_ptr->call_type.call_type == CATI_CC_CALL_TYPE_USSD))||
               (!tlv_data_ptr->call_type_present)) &&
               tlv_data_ptr->ussd_string_present)
      {
        cc_ind_ptr->call_enum_type = GSTK_USSD;

        cc_ind_ptr->gstk_address_string.ussd_string.orig_dcs_from_sim = tlv_data_ptr->ussd_string.dcs;
        cc_ind_ptr->gstk_address_string.ussd_string.dcs = tlv_data_ptr->ussd_string.dcs;
        cc_ind_ptr->gstk_address_string.ussd_string.length = tlv_data_ptr->ussd_string.length;
        cc_ind_ptr->gstk_address_string.ussd_string.text = tlv_data_ptr->ussd_string.text;
      }
      else if (!tlv_data_ptr->call_type_present && tlv_data_ptr->pdp_context_act_present)
      {
        cc_ind_ptr->call_enum_type = GSTK_PDP_CONTEXT_ACT;

        cc_ind_ptr->gstk_address_string.pdp_context.length = tlv_data_ptr->pdp_context_act.length;
        /* No length check required as max length that can be input is 255 bytes
           and size of destination array is also 255 bytes */
        (void)memscpy(cc_ind_ptr->gstk_address_string.pdp_context.pdp_act_param,
                      sizeof(cc_ind_ptr->gstk_address_string.pdp_context.pdp_act_param),
                      tlv_data_ptr->pdp_context_act.pdp_act_param,
                      tlv_data_ptr->pdp_context_act.length);
      }
      else if (!tlv_data_ptr->call_type_present && tlv_data_ptr->eps_pdn_conn_act_present)
      {
        cc_ind_ptr->call_enum_type = GSTK_EPS_PDN_CONN_ACT;

        cc_ind_ptr->gstk_address_string.eps_conn_act.length = tlv_data_ptr->eps_pdn_connect_act.length;
        /* No length check required as max length that can be input is 255 bytes
           and size of destination array is also 255 bytes */
        (void)memscpy(cc_ind_ptr->gstk_address_string.eps_conn_act.eps_act_param,
                      sizeof(cc_ind_ptr->gstk_address_string.eps_conn_act.eps_act_param),
                      tlv_data_ptr->eps_pdn_connect_act.eps_act_param,
                      tlv_data_ptr->eps_pdn_connect_act.length);
      }
      else if (tlv_data_ptr->ims_uri_present &&
                ((tlv_data_ptr->call_type_present &&
                 tlv_data_ptr->call_type.call_type == CATI_CC_CALL_TYPE_IMS) ||
                (!tlv_data_ptr->call_type_present)))
      {
        cc_ind_ptr->call_enum_type = GSTK_IMS_CALL;

        cc_ind_ptr->gstk_address_string.ims_call.length = tlv_data_ptr->ims_uri.length;
        /* No length check required as max length that can be input is 255 bytes
           and size of destination array is also 255 bytes */
        (void)memscpy(cc_ind_ptr->gstk_address_string.ims_call.ims_uri,
                      sizeof(cc_ind_ptr->gstk_address_string.ims_call.ims_uri),
                      tlv_data_ptr->ims_uri.ims_uri,
                      tlv_data_ptr->ims_uri.length);
      }
      else if (tlv_data_ptr->call_type_present &&
               (tlv_data_ptr->call_type.call_type == CATI_CC_CALL_TYPE_SMS))
      {
        cc_sms_ind_ptr = (gstk_mo_sms_ctrl_ind_ext_type *)uimqmi_malloc(sizeof(gstk_mo_sms_ctrl_ind_ext_type));

        if (cc_sms_ind_ptr == NULL)
        {
          UIM_MSG_ERR_0("Decoded envelope CC SMS command dropped.No memory");
          errval = QMI_ERR_NO_MEMORY;
          goto send_result;
        }

        memset((void*)cc_sms_ind_ptr, 0, sizeof(gstk_mo_sms_ctrl_ind_ext_type));

        cc_sms_ind_ptr->rp_dest_address.TON =
          (gstk_ton_enum_type)tlv_data_ptr->rp_address.ton;
        cc_sms_ind_ptr->rp_dest_address.NPI =
          (gstk_npi_enum_type)tlv_data_ptr->rp_address.npi;
        cc_sms_ind_ptr->rp_dest_address.length = tlv_data_ptr->rp_address.length;

        (void)memscpy(cc_sms_ind_ptr->rp_dest_address.address,
                      sizeof(cc_sms_ind_ptr->rp_dest_address.address),
                      tlv_data_ptr->rp_address.address,
                      tlv_data_ptr->rp_address.length);

        cc_sms_ind_ptr->tp_dest_address.TON =
          (gstk_ton_enum_type)tlv_data_ptr->tp_address.ton;
        cc_sms_ind_ptr->tp_dest_address.NPI =
          (gstk_npi_enum_type)tlv_data_ptr->tp_address.npi;
        cc_sms_ind_ptr->tp_dest_address.length = tlv_data_ptr->tp_address.length;

        (void)memscpy(cc_sms_ind_ptr->tp_dest_address.address,
                      sizeof(cc_sms_ind_ptr->tp_dest_address.address),
                      tlv_data_ptr->tp_address.address,
                      tlv_data_ptr->tp_address.length);
      }
      else
      {
        UIM_MSG_ERR_0("qmi_cati_decoded_envelope_cmd:Unknown CC type");
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }

      if (!tlv_data_ptr->rat_present)
      {
        errval = QMI_ERR_MISSING_ARG;
        UIM_MSG_ERR_0("qmi_cati_decoded_envelope_cmd:RAT TLV not present");
        goto send_result;
      }

      if (tlv_data_ptr->call_type_present &&
          (tlv_data_ptr->call_type.call_type == CATI_CC_CALL_TYPE_SMS))
      {
        errval = qmi_cati_convert_qmi_rat_to_gstk_rat(tlv_data_ptr->rat.rat,
                                                      &(cc_sms_ind_ptr->rat));
        if (errval != QMI_ERR_NONE)
        {
          goto send_result;
        }

        /* Set location info data with 0xFF so GSTK will lookup for the
           data according to RAT info */
        switch(cc_sms_ind_ptr->rat)
        {
           case GSTK_ACCESS_TECH_GSM:
           case GSTK_ACCESS_TECH_UTRAN:
           case GSTK_ACCESS_TECH_LTE:
             memset((void*)&cc_sms_ind_ptr->loc_info.gwl_info, 0xFF, sizeof(gstk_gwl_loc_info));
             break;
           case GSTK_ACCESS_TECH_CDMA:
             memset((void*)&cc_sms_ind_ptr->loc_info.cdma_info, 0xFF, sizeof(gstk_cdma_loc_info));
             break;
           case GSTK_ACCESS_TECH_WLAN:
             /* Nothing to do in this case */
             break;
           /* GSTK doesn't support CC envelope for (e)HRPD */
           case GSTK_ACCESS_TECH_HRPD:
           case GSTK_ACCESS_TECH_EHRPD:
           default:
             UIM_MSG_ERR_1("unhandled RAT type 0x%x", cc_sms_ind_ptr->rat);
             break;
        }
        gstk_status = gstk_slot_send_envelope_mo_sms_ctrl_command_ext(req_slot,
                                                                      (uint32)cb_userdata_ptr,
                                                                      cc_sms_ind_ptr,
                                                                      qmi_cati_gstk_decoded_env_cb);
      }
      else
      {
        errval = qmi_cati_convert_qmi_rat_to_gstk_rat(tlv_data_ptr->rat.rat,
                                                      &(cc_ind_ptr->rat));
        if (errval != QMI_ERR_NONE)
        {
          goto send_result;
        }

        /* Set location info data with 0xFF so GSTK will lookup for the
           data according to RAT info */
        switch(cc_ind_ptr->rat)
        {
           case GSTK_ACCESS_TECH_GSM:
           case GSTK_ACCESS_TECH_UTRAN:
           case GSTK_ACCESS_TECH_LTE:
             memset((void*)&cc_ind_ptr->loc_info.gwl_info, 0xFF, sizeof(gstk_gwl_loc_info));
             break;
           case GSTK_ACCESS_TECH_CDMA:
             memset((void*)&cc_ind_ptr->loc_info.cdma_info, 0xFF, sizeof(gstk_cdma_loc_info));
             break;
           case GSTK_ACCESS_TECH_WLAN:
             break;
           /* GSTK doesn't support CC envelope for (e)HRPD */
           case GSTK_ACCESS_TECH_HRPD:
           case GSTK_ACCESS_TECH_EHRPD:
           default:
             UIM_MSG_ERR_1("unhandled RAT type 0x%x", cc_ind_ptr->rat);
             break;
        }
        gstk_status =
          gstk_slot_send_envelope_cc_command_ext(req_slot,
                                                 (uint32)cb_userdata_ptr,
                                                 cc_ind_ptr,
                                                 qmi_cati_gstk_decoded_env_cb);
      }

      if (gstk_status == GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_0("qmi_cati_decoded_envelope_cmd:CMD queued to GSTK");
        /* Handle the envelope response for call control in callback */
        QMI_CAT_FREE_IF_NOT_NULL(tlv_data_ptr);
        QMI_CAT_FREE_IF_NOT_NULL(cc_ind_ptr);
        QMI_CAT_FREE_IF_NOT_NULL(cc_sms_ind_ptr);
        return QMI_SVC_RESPONSE_PENDING;
      }
      errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
      break;

    case CATI_ENV_CMD_TYPE_DECODED_EVT_BROWSER_TERM:
      /* Check mandatory items */
      if (!tlv_data_ptr->browser_term_cause_present)
      {
        errval = QMI_ERR_MISSING_ARG;
        goto send_result;
      }

      /* Map cause type */
      switch (tlv_data_ptr->browser_term_cause.browser_term_cause)
      {
        case 0x00:
          evt_list.evt_ind.browser_terminate_evt_ind = GSTK_BROWSER_USER_TERMINATED;
          break;
        case 0x01:
          evt_list.evt_ind.browser_terminate_evt_ind = GSTK_BROWSER_ERROR_TERMINATED;
          break;
        default:
          UIM_MSG_ERR_1("qmi_cati_decoded_envelope_cmd: Invalid browser_term_cause 0x%x",
                        tlv_data_ptr->browser_term_cause.browser_term_cause);
          errval = QMI_ERR_INVALID_ARG;
          goto send_result;
      }

      evt_list.evt_type = GSTK_BROWSER_TERMINATION_EVT_IND;

      gstk_status = gstk_send_envelope_evt_download_command(
                                  qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                                  (uint32)cb_userdata_ptr,
                                  &evt_list);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_1("gstk_send_envelope_evt_download_command : gstk_status = status %d",
                       gstk_status);
        errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
        goto send_result;
      }
      break;

     case CATI_ENV_CMD_TYPE_DECODED_SMS_PP_DOWNLOAD:
       /* check if SMS PP envelope command is blocked */
       if (qmi_cati_global_ptr->sms_pp_envelope_blocked)
       {
         UIM_MSG_HIGH_0("SMS PP DL is blocked for security.");
         errval = QMI_ERR_INVALID_OPERATION;
         goto send_result;
       }
      /* Check mandatory items */
      if (!tlv_data_ptr->sms_tpdu_present ||
          (tlv_data_ptr->sms_tpdu.length > ((CATI_TLV_SMS_TPDU_MAX))) ||
          !tlv_data_ptr->address_present  ||
          (tlv_data_ptr->address.length > CATI_TLV_ADDRESS_MAX))
      {
        errval = QMI_ERR_MISSING_ARG;
        goto send_result;
      }

      /* Default to 3GPP SMS, ie. set to FALSE, if is_cdma_sms TLV is not present */
      if (tlv_data_ptr->is_cdma_sms_present)
      {
        is_cdma_sms = tlv_data_ptr->is_cdma_sms.is_cdma_sms;
      }

      sms_pp_dl_address.TON = (gstk_ton_enum_type) tlv_data_ptr->address.ton;
      sms_pp_dl_address.NPI = (gstk_npi_enum_type) tlv_data_ptr->address.npi;
      sms_pp_dl_address.length = tlv_data_ptr->address.length;
      (void)memscpy(sms_pp_dl_address.address,
                    sizeof(sms_pp_dl_address.address),
                    tlv_data_ptr->address.address,
                    tlv_data_ptr->address.length);

      sms_tpdu.length = tlv_data_ptr->sms_tpdu.length;
      sms_tpdu.tpdu = tlv_data_ptr->sms_tpdu.tpdu;

      gstk_status = gstk_send_envelope_sms_pp_dl_command((uint32)cb_userdata_ptr,
                                                         is_cdma_sms,
                                                         &sms_pp_dl_address,
                                                         &sms_tpdu,
                                                         qmi_cati_gstk_decoded_env_cb);
      if (gstk_status == GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_0("qmi_cati_decoded_envelope_cmd:CMD queued to GSTK");
        /* Handle the envelope response for sms pp downlaod in callback */
        uimqmi_free(tlv_data_ptr);
        return QMI_SVC_RESPONSE_PENDING;
      }
      errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
      break;

    case CATI_ENV_CMD_TYPE_DECODED_EVT_DL_MT_CALL:
      /* Check mandatory items */
      if (!tlv_data_ptr->transaction_id_present)
      {
        errval = QMI_ERR_MISSING_ARG;
        goto send_result;
      }

      /* validate TLVs */
      if ((tlv_data_ptr->transaction_id.length > ((CATI_TLV_TX_ID_MAX))) ||
          (tlv_data_ptr->address_present &&
           (tlv_data_ptr->address.length > CATI_TLV_ADDRESS_MAX)) ||
          (tlv_data_ptr->subaddress_present &&
           (tlv_data_ptr->subaddress.length > CATI_TLV_SUBADDRESS_MAX)))
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }

      evt_list.evt_type = GSTK_MT_CALL_EVT_IND;
      if (tlv_data_ptr->address_present)
      {
        evt_list.evt_ind.mt_call_evt_ind.address.TON = (gstk_ton_enum_type) tlv_data_ptr->address.ton;
        evt_list.evt_ind.mt_call_evt_ind.address.NPI = (gstk_npi_enum_type) tlv_data_ptr->address.npi;
        evt_list.evt_ind.mt_call_evt_ind.address.length = tlv_data_ptr->address.length;
        (void)memscpy(evt_list.evt_ind.mt_call_evt_ind.address.address,
                      sizeof(evt_list.evt_ind.mt_call_evt_ind.address.address),
                      tlv_data_ptr->address.address,
                      tlv_data_ptr->address.length);
      }
      else
      {
        /* GSTK will not send address TLV in the envelope if len = 0*/
        evt_list.evt_ind.mt_call_evt_ind.address.length = 0;
      }

      if (tlv_data_ptr->subaddress_present)
      {
        evt_list.evt_ind.mt_call_evt_ind.subaddress.length = tlv_data_ptr->subaddress.length;
        (void)memscpy(evt_list.evt_ind.mt_call_evt_ind.subaddress.subaddress,
                      sizeof(evt_list.evt_ind.mt_call_evt_ind.subaddress.subaddress),
                      tlv_data_ptr->subaddress.subaddress,
                      tlv_data_ptr->subaddress.length);
      }
      else
      {
        /* GSTK will not send sub-address TLV in the envelope if len = 0*/
        evt_list.evt_ind.mt_call_evt_ind.subaddress.length = 0;
      }

      evt_list.evt_ind.mt_call_evt_ind.transaction_id.length
        = tlv_data_ptr->transaction_id.length;
      evt_list.evt_ind.mt_call_evt_ind.transaction_id.transaction_list
        = tlv_data_ptr->transaction_id.transaction_id;

      gstk_status = gstk_send_envelope_evt_download_command(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          (uint32)cb_userdata_ptr,
                          &evt_list);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_1("gstk_send_envelope_evt_download_command : gstk_status = status %d",
                       gstk_status);
        errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
        goto send_result;
      }
      break;

    case CATI_ENV_CMD_TYPE_DECODED_EVT_DL_MT_CALL_CONNECTED:
    case CATI_ENV_CMD_TYPE_DECODED_EVT_DL_MO_CALL_CONNECTED:
      /* Check mandatory items */
      if (!tlv_data_ptr->transaction_id_present ||
          (tlv_data_ptr->transaction_id.length > ((CATI_TLV_TX_ID_MAX))))
      {
        errval = QMI_ERR_MISSING_ARG;
        goto send_result;
      }

      evt_list.evt_type = GSTK_CALL_CONNECTED_EVT_IND;
      evt_list.evt_ind.call_connected_evt_ind.transaction_id.length
        = tlv_data_ptr->transaction_id.length;
      evt_list.evt_ind.call_connected_evt_ind.transaction_id.transaction_list
        = tlv_data_ptr->transaction_id.transaction_id;
      if (tlv_data_ptr->env_hdr.env_cmd_type == CATI_ENV_CMD_TYPE_DECODED_EVT_DL_MT_CALL_CONNECTED)
      {
        evt_list.evt_ind.call_connected_evt_ind.near_end = TRUE;
      }
      else
      {
        evt_list.evt_ind.call_connected_evt_ind.near_end = FALSE;
      }

      gstk_status = gstk_send_envelope_evt_download_command(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          (uint32)cb_userdata_ptr,
                          &evt_list);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_1("gstk_send_envelope_evt_download_command : gstk_status = status %d",
                       gstk_status);
        errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
        goto send_result;
      }
      break;

    case CATI_ENV_CMD_TYPE_DECODED_EVT_DL_CALL_DISCONNECTED_NEAR_END:
    case CATI_ENV_CMD_TYPE_DECODED_EVT_DL_CALL_DISCONNECTED_FAR_END:
      /* Check mandatory items */
      if (!tlv_data_ptr->transaction_id_present ||
          (tlv_data_ptr->transaction_id.length > ((CATI_TLV_TX_ID_MAX))))
      {
        errval = QMI_ERR_MISSING_ARG;
        goto send_result;
      }

      evt_list.evt_type = GSTK_CALL_DISCONNECTED_EVT_IND;
      evt_list.evt_ind.call_disconnected_evt_ind.transaction_id.length
        = tlv_data_ptr->transaction_id.length;
      evt_list.evt_ind.call_disconnected_evt_ind.transaction_id.transaction_list
        = tlv_data_ptr->transaction_id.transaction_id;

      if(tlv_data_ptr->cause_present &&
         (tlv_data_ptr->cause.length <= CATI_TLV_CAUSE_MAX))
      {
        evt_list.evt_ind.call_disconnected_evt_ind.cause.present = TRUE;
        evt_list.evt_ind.call_disconnected_evt_ind.cause.length
          = tlv_data_ptr->cause.length;
        evt_list.evt_ind.call_disconnected_evt_ind.cause.cause
          = tlv_data_ptr->cause.cause;
      }
      else if (tlv_data_ptr->cause_present &&
               (tlv_data_ptr->cause.length > CATI_TLV_CAUSE_MAX))
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }
      else
      {
        evt_list.evt_ind.call_disconnected_evt_ind.cause.present = FALSE;
        evt_list.evt_ind.call_disconnected_evt_ind.cause.length = 0;
        evt_list.evt_ind.call_disconnected_evt_ind.cause.cause = NULL;
      }

      if (tlv_data_ptr->env_hdr.env_cmd_type == CATI_ENV_CMD_TYPE_DECODED_EVT_DL_CALL_DISCONNECTED_NEAR_END)
      {
        evt_list.evt_ind.call_disconnected_evt_ind.near_end = TRUE;
      }
      else
      {
        evt_list.evt_ind.call_disconnected_evt_ind.near_end = FALSE;
      }

      gstk_status = gstk_send_envelope_evt_download_command(
                          qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                          (uint32)cb_userdata_ptr,
                          &evt_list);

      if (gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_HIGH_1("gstk_send_envelope_evt_download_command : gstk_status = status %d",
                       gstk_status);
        errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
        goto send_result;
      }
      break;

    default:
      UIM_MSG_ERR_1("qmi_cati_decoded_envelope_cmd: Invalid cmd 0x%x",
                    tlv_data_ptr->env_hdr.env_cmd_type);
      errval = QMI_ERR_INVALID_ARG;
      break;
  }

  if (ind_token.is_valid)
  {
    /* add indication token to the immediate response */
    errval = qmi_cat_response_indication_token(&response,
                                               ind_token.token,
                                               CATI_CMD_VAL_DECODED_ENVELOPE_CMD,
                                               CATI_RESP_MSG_TYPE);
    if (QMI_ERR_NONE != errval)
    {
      dsm_free_packet(&response);
    }
  }

send_result:
  if (errval != QMI_ERR_NONE)
  {
    /* Free userdata since it is no longer needed */
    qmi_cat_free_cb_userdata(cb_userdata_ptr);
    cb_userdata_ptr = NULL;
  }

  QMI_CAT_FREE_IF_NOT_NULL(tlv_data_ptr);
  QMI_CAT_FREE_IF_NOT_NULL(cc_ind_ptr);
  QMI_CAT_FREE_IF_NOT_NULL(cc_sms_ind_ptr);

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_decoded_envelope_cmd */


/*===========================================================================
  FUNCTION QMI_CATI_CONVERT_QMI_DCS_TO_GSTK

  DESCRIPTION
    to convert from QMI to GSTK DCS value

  PARAMETERS
    qmi_dcs : QMI DCS value
    gstk_dcs: GSTK DCS value

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_convert_qmi_dcs_to_gstk
(
  uint8   qmi_dcs,
  uint8 * gstk_dcs
)
{
  switch (qmi_dcs)
  {
    case CATI_DCS_8_BIT:
      *gstk_dcs = QMI_CATI_SMS_8_BIT_GENERAL_DATA_CODING_DCS;
      break;
    case CATI_DCS_UCS2_8_BIT:
      *gstk_dcs = QMI_CATI_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS;
      break;
    case CATI_DCS_7_BIT:
      *gstk_dcs = QMI_CATI_SMS_7_BIT_PACKED_GENERAL_DATA_CODING_DCS;
      break;
    default:
       break;
  }
} /* qmi_cati_convert_qmi_dcs_to_gstk */


/*===========================================================================
  FUNCTION QMI_CATI_SEND_DECODED_TR()

  DESCRIPTION
    Send the Terminal Response to the SIM

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_send_decoded_tr
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *                     response              = NULL;
  qmi_error_e_type                    errval                = QMI_ERR_NONE;
  boolean                             retval                = TRUE;
  gstk_cmd_enum_type                  terminal_response_cnf = GSTK_CMD_ENUM_NOT_USE;
  gstk_status_enum_type               gstk_status           = GSTK_SUCCESS;
  qmi_result_e_type                   result                = QMI_RESULT_SUCCESS;
  uint8                               slot_index            = 0;
  qmi_cat_in_decoded_tr_type          tlv_data;
  gstk_additional_info_type           addl_info;
  gstk_terminal_rsp_extra_param_type  extra_info;
  qmi_cati_ind_token_info_type        ind_token             = {FALSE, 0};
  qmi_cati_userdata_type            * cb_userdata_ptr       = NULL;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset((void*)&tlv_data, 0, sizeof(qmi_cat_in_decoded_tr_type));
  memset((void*)&extra_info,  0, sizeof(gstk_terminal_rsp_extra_param_type));
  memset((void*)&addl_info,   0, sizeof(gstk_additional_info_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_decoded_tr(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Copy additional info */
  if (tlv_data.term_resp.additional_info_length > 0 &&
      tlv_data.term_resp.additional_info_length <= GSTK_MAX_ADDITIONAL_INFO)
  {
    addl_info.length = tlv_data.term_resp.additional_info_length;
    (void)memscpy((void*)addl_info.additional_info,
                  sizeof(addl_info.additional_info),
                  tlv_data.term_resp.additional_info_data,
                  tlv_data.term_resp.additional_info_length);
  }
  else
  {
    addl_info.length = 0;
    memset((void*)addl_info.additional_info, 0x00, sizeof(addl_info.additional_info));
  }

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* copy indication token */
  if (tlv_data.ind_token_present)
  {
    ind_token.is_valid = TRUE;
    ind_token.token = tlv_data.ind_token.ind_token;
  }

  errval = qmi_cati_check_last_gstk_cmd(slot_index, TRUE);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Reject TR if:
     1. cmd_ref_id does not match ref_command_id
   */
  if(tlv_data.term_resp.cmd_detail_ref_id != qmi_cati_global_ptr->ref_command_id[slot_index])
  {
    errval = QMI_ERR_INVALID_OPERATION;
    goto send_result;
  }

  switch (tlv_data.term_resp.response_cmd)
  {
    case CATI_EVENT_GET_INKEY_CNF:
      if (tlv_data.get_inkey_term_resp_extra_present &&
          tlv_data.get_inkey_term_resp_yes_no_present)
      {
        errval = QMI_ERR_INVALID_ARG;
        goto send_result;
      }
      else if (tlv_data.get_inkey_term_resp_extra_present)
      {
        gstk_get_inkey_rsp_extra_param_type * param_ptr = &extra_info.extra_param.get_inkey_extra_param;

        extra_info.present  = TRUE;
        extra_info.rsp_type = GSTK_GET_INKEY_CNF;

        /* Text */
        qmi_cati_convert_qmi_dcs_to_gstk(tlv_data.get_inkey_term_resp_extra.dcs,
                                         &param_ptr->user_get_inkey_input.dcs);
        param_ptr->user_get_inkey_input.length = tlv_data.get_inkey_term_resp_extra.length_of_string;
        param_ptr->user_get_inkey_input.text   = (uint8*)tlv_data.get_inkey_term_resp_extra.text;

        /* Duration */
        if (tlv_data.get_inkey_term_resp_extra.units == GSTK_DURATION_MINUTES ||
            tlv_data.get_inkey_term_resp_extra.units == GSTK_DURATION_SECONDS ||
            tlv_data.get_inkey_term_resp_extra.units == GSTK_DURATION_TENTHS_OF_SECONDS)
        {
          param_ptr->duration_of_exec.present  = TRUE;
          param_ptr->duration_of_exec.units    = tlv_data.get_inkey_term_resp_extra.units;
          param_ptr->duration_of_exec.length   = tlv_data.get_inkey_term_resp_extra.interval;
        }
        else
        {
          param_ptr->duration_of_exec.present  = FALSE;
        }
      }
      else if (tlv_data.get_inkey_term_resp_yes_no_present)
      {
        gstk_get_inkey_rsp_extra_param_type * param_ptr = &extra_info.extra_param.get_inkey_extra_param;

        extra_info.present  = TRUE;
        extra_info.rsp_type = GSTK_GET_INKEY_CNF;

        /* Yes / No */
        param_ptr->user_get_inkey_input.length = 1;
        param_ptr->user_get_inkey_input.dcs    = QMI_CATI_SMS_8_BIT_GENERAL_DATA_CODING_DCS;
        param_ptr->user_get_inkey_input.text   = (uint8*)(&tlv_data.get_inkey_term_resp_yes_no.yes_no);

        /* Duration */
        if (tlv_data.get_inkey_term_resp_yes_no.units == GSTK_DURATION_MINUTES ||
            tlv_data.get_inkey_term_resp_yes_no.units == GSTK_DURATION_SECONDS ||
            tlv_data.get_inkey_term_resp_yes_no.units == GSTK_DURATION_TENTHS_OF_SECONDS)
        {
          param_ptr->duration_of_exec.present  = TRUE;
          param_ptr->duration_of_exec.units    = tlv_data.get_inkey_term_resp_yes_no.units;
          param_ptr->duration_of_exec.length   = tlv_data.get_inkey_term_resp_yes_no.interval;
        }
        else
        {
          param_ptr->duration_of_exec.present  = FALSE;
        }
      }
      break;

    case CATI_EVENT_GET_INPUT_CNF:
      if (tlv_data.text_string_present)
      {
        extra_info.present = TRUE;
        extra_info.rsp_type = GSTK_GET_INPUT_CNF;

        qmi_cati_convert_qmi_dcs_to_gstk(tlv_data.text_string.dcs,
                                         &extra_info.extra_param.user_get_input.dcs);
        extra_info.extra_param.user_get_input.length = tlv_data.text_string.length_of_string;
        extra_info.extra_param.user_get_input.text   = (uint8*)tlv_data.text_string.text;
      }
      break;

    case CATI_EVENT_SELECT_ITEM_CNF:
      if (tlv_data.item_id_present)
      {
        extra_info.present = TRUE;
        extra_info.rsp_type = GSTK_SELECT_ITEM_CNF;

        extra_info.extra_param.select_item_chosen_item = tlv_data.item_id.item_id;
      }
      break;

    case CATI_EVENT_PROVIDE_LANGUAGE_INFO_CNF:
      if (tlv_data.language_present)
      {
        extra_info.present = TRUE;
        extra_info.rsp_type = GSTK_PROVIDE_LANG_INFO_CNF;
        (void)memscpy(extra_info.extra_param.provide_lang_info_extra_param.lang_code,
                      sizeof(extra_info.extra_param.provide_lang_info_extra_param.lang_code),
                      tlv_data.language.language,
                      GSTK_LANG_CODE_LEN);
      }
      break;

    default:
      break;
  }

  /* Conversion from QMI format to GSTK format */
  if (qmi_cati_convert_qmi_cnf_to_gstk_cnf(tlv_data.term_resp.response_cmd,
                                           &terminal_response_cnf) != QMI_ERR_NONE)
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* Create the userdata */
  cb_userdata_ptr = qmi_cat_get_cb_userdata(CATI_CMD_VAL_SEND_DECODED_TR, cmd_buf_p, ind_token);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  gstk_status = gstk_send_terminal_response(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           (uint32)cb_userdata_ptr,
                           tlv_data.term_resp.cmd_detail_ref_id,
                           tlv_data.term_resp.command_number,
                           terminal_response_cnf,
                           (gstk_general_result_enum_type)tlv_data.term_resp.general_result,
                           &addl_info,
                           &extra_info);

  if(gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_1("qmi_cati_send_decoded_tr : gstk_status = status %d",
                   gstk_status);
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
    /* Free userdata since it is no longer needed */
    qmi_cat_free_cb_userdata(cb_userdata_ptr);
    cb_userdata_ptr = NULL;
    goto send_result;
  }
  else
  {
    /* For GSTK_SETUP_EVENT_LIST_REQ, decrement setup_evt_in_progress
       For all other commands, setup_evt_in_progress should be 0
       When all TRs for current proactive command are recevied,
       reset last_gstk_cmd, ref_command_id to filter duplicate TR
     */
    if (GSTK_SETUP_EVENT_LIST_REQ == qmi_cati_global_ptr->last_gstk_cmd[slot_index] &&
        qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index])
    {
      qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index]--;
    }
    if (0 == qmi_cati_global_ptr->decoded_setup_evt_in_progress[slot_index])
    {
      qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
      qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
      qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;
    }
  }

  if (ind_token.is_valid)
  {
    qmi_cat_message_type * msg_ptr = NULL;

    /* add indication token to the immediate response */
    errval = qmi_cat_response_indication_token(&response,
                                               ind_token.token,
                                               CATI_CMD_VAL_SEND_DECODED_TR,
                                               CATI_RESP_MSG_TYPE);

    if (QMI_ERR_NONE != errval)
    {
      /* we are going to send TR response without indication token
      in the error case, that maybe an issue, but I don't see what else
      we can do */
      dsm_free_packet(&response);
    }
    else
    {
      /* post an indication*/
      msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_TERM_RSP_INDICATION);

      if(NULL != msg_ptr)
      {
        msg_ptr->data.terminal_response_rsp.terminal_response_rsp.hdr_cmd.user_data
          = (uint32)cb_userdata_ptr;
        qmi_cat_post_message(msg_ptr);
      }
    }
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_send_decoded_tr */


/*===========================================================================
  FUNCTION QMI_CATI_EVENT_CONFIRMATION()

  DESCRIPTION
    Send the confirmation for a network related command

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_event_confirmation
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *                     response     = NULL;
  qmi_error_e_type                    errval       = QMI_ERR_NONE;
  boolean                             retval       = TRUE;
  qmi_result_e_type                   result       = QMI_RESULT_SUCCESS;
  gstk_status_enum_type               gstk_status  = GSTK_SUCCESS;
  uint8                               slot_index   = 0;
  qmi_cat_in_event_confirmation_type  tlv_data;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_event_confirmation_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_event_confirmation(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  errval = qmi_cati_check_last_gstk_cmd(slot_index, FALSE);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send command to GSTK */
  if (tlv_data.user_confirmed_present)
  {
     gstk_status = gstk_send_user_cnf_alpha_and_icon_disp_rsp(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           qmi_cati_global_ptr->ref_command_id[slot_index],
                           tlv_data.user_confirmed.user_confirmed,
                           tlv_data.icon_display_present ? tlv_data.icon_display.icon_display : FALSE
                           );
  }
  else if(tlv_data.icon_display_present)
  {
      gstk_status = gstk_send_icon_display_rsp(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           qmi_cati_global_ptr->ref_command_id[slot_index],
                           tlv_data.icon_display.icon_display
                           );
  }
  else
  {
    UIM_MSG_HIGH_2("qmi_cati_event_confirmation : Got user = 0x%x, Got icon = 0x%x",
                   tlv_data.user_confirmed_present,
                   tlv_data.icon_display_present);
  }

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_1("qmi_cati_event_confirmation : gstk_status = status %d",
                   gstk_status);
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
  }
  else
  {
    /* reset last GSTK cmd to filter duplicate Event Confirmation */
    qmi_cati_global_ptr->last_gstk_cmd[slot_index]  = GSTK_END_PROACTIVE_CMD_REQ;
    qmi_cati_global_ptr->ref_command_id[slot_index] = 0;
    qmi_cati_global_ptr->full_func_evt[slot_index]  = FALSE;
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_event_confirmation */


/*===========================================================================
  FUNCTION QMI_CATI_SCWS_OPEN_CHANNEL()

  DESCRIPTION
    Send the confirmation with channel state for scws open channel cmd

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_scws_open_channel
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type                          * response            = NULL;
  qmi_error_e_type                         errval              = QMI_ERR_NONE;
  boolean                                  retval              = TRUE;
  qmi_result_e_type                        result              = QMI_RESULT_SUCCESS;

  gstk_status_enum_type                    gstk_status         = GSTK_SUCCESS;
  qmi_cat_in_scws_open_channel_state_type  tlv_data;
  uint8                                    slot_index          = 0;
  gstk_scws_ch_state_enum_type             gstk_channel_state  = GSTK_SCWS_TCP_STATE_MAX;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_scws_open_channel_state_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_scws_open_channel_state(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send command to GSTK */
  if (tlv_data.channel_state_present)
  {
    /* Update channel state TLV */
    errval = qmi_cati_convert_qmi_channel_state_to_gstk_channel_state(
                            tlv_data.channel_state.ch_state,
                            &gstk_channel_state);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }

    if(tlv_data.channel_state.ch_id >= QMI_CATI_MAX_SCWS_CHANNEL)
    {
      errval = QMI_ERR_INVALID_ARG;
      UIM_MSG_ERR_1("channel id out of range 0x%x",
                    tlv_data.channel_state.ch_id);
      goto send_result;
    }

    gstk_status = gstk_scws_open_ch_rsp(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           qmi_cati_global_ptr->ref_command_id[slot_index],
                           tlv_data.channel_state.ch_id,
                           gstk_channel_state);
  }
  else
  {
    UIM_MSG_HIGH_1(" qmi_cati_scws_open_channel : channel state present = 0x%x,",
                   tlv_data.channel_state_present);
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_1("qmi_cati_scws_open_channel : gstk_status = status %d",
                   gstk_status);
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
  }


send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_scws_open_channel() */


/*===========================================================================
  FUNCTION QMI_CATI_SCWS_CLOSE_CHANNEL()

  DESCRIPTION
    Send the confirmation with channel state for scws close channel cmd

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_scws_close_channel
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type                           * response            = NULL;
  qmi_error_e_type                          errval              = QMI_ERR_NONE;
  boolean                                   retval              = TRUE;
  qmi_result_e_type                         result              = QMI_RESULT_SUCCESS;

   gstk_status_enum_type                     gstk_status         = GSTK_SUCCESS;
  qmi_cat_in_scws_close_channel_state_type  tlv_data;
  uint8                                     slot_index          = 0;
  gstk_scws_ch_state_enum_type              gstk_channel_state  = GSTK_SCWS_TCP_STATE_MAX;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_scws_close_channel_state_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_scws_close_channel_state(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  UIM_MSG_HIGH_2(" qmi_cati_scws_close_channel : channel_state_present = 0x%x, slot_present = 0x%x",
                 tlv_data.channel_state_present, tlv_data.slot_present);

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send command to GSTK */
  if (tlv_data.channel_state_present)
  {
    /* Update channel state TLV */
    errval = qmi_cati_convert_qmi_channel_state_to_gstk_channel_state(
                            tlv_data.channel_state.ch_state,
                            &gstk_channel_state);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }

    if(tlv_data.channel_state.ch_id >= QMI_CATI_MAX_SCWS_CHANNEL)
    {
      errval = QMI_ERR_INVALID_ARG;
      UIM_MSG_ERR_1("channel id out of range 0x%x",
                    tlv_data.channel_state.ch_id);
      goto send_result;
    }

    gstk_status = gstk_scws_close_ch_rsp(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           qmi_cati_global_ptr->ref_command_id[slot_index],
                           tlv_data.channel_state.ch_id,
                           gstk_channel_state);
  }
  else
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if (gstk_status != GSTK_SUCCESS)
  {
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_scws_close_channel() */


/*===========================================================================
  FUNCTION QMI_CATI_SCWS_CHANGE_STATUS()

  DESCRIPTION
    Send the confirmation with channel state for scws change status cmd

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_scws_change_status
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type                             * response            = NULL;
  qmi_error_e_type                            errval              = QMI_ERR_NONE;
  boolean                                     retval              = TRUE;
  qmi_result_e_type                           result              = QMI_RESULT_SUCCESS;

  gstk_status_enum_type                       gstk_status         = GSTK_SUCCESS;
  qmi_cat_in_scws_channel_status_state_type   tlv_data;
  uint8                                       slot_index          = 0;
  gstk_scws_ch_state_enum_type                gstk_channel_state  = GSTK_SCWS_TCP_STATE_MAX;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_scws_channel_status_state_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_scws_channel_status_state(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  UIM_MSG_HIGH_2(" qmi_cati_scws_close_channel : channel_state_present = 0x%x, slot_present = 0x%x",
                 tlv_data.channel_state_present, tlv_data.slot_present);

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send command to GSTK */
  if (tlv_data.channel_state_present)
  {
    /* Update channel state TLV */
    errval = qmi_cati_convert_qmi_channel_state_to_gstk_channel_state(
                            tlv_data.channel_state.ch_state,
                            &gstk_channel_state);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }

    if(tlv_data.channel_state.ch_id >= QMI_CATI_MAX_SCWS_CHANNEL)
    {
      errval = QMI_ERR_INVALID_ARG;
      UIM_MSG_ERR_1("channel id out of range 0x%x",
                    tlv_data.channel_state.ch_id);
      goto send_result;
    }

    gstk_status = gstk_scws_ch_status_req(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           tlv_data.channel_state.ch_id,
                           gstk_channel_state);
  }
  else
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if (gstk_status != GSTK_SUCCESS)
  {
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_scws_change_status() */


/*===========================================================================
  FUNCTION QMI_CATI_SCWS_SEND_DATA()

  DESCRIPTION
    Send the confirmation with channel state for scws send data cmd

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_scws_send_data
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type                     * response     = NULL;
  qmi_error_e_type                    errval       = QMI_ERR_NONE;
  boolean                             retval       = TRUE;
  qmi_result_e_type                   result       = QMI_RESULT_SUCCESS;

   gstk_status_enum_type               gstk_status  = GSTK_SUCCESS;
  qmi_cat_in_scws_send_data_type      tlv_data;
  uint8                               slot_index   = 0;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_scws_send_data_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_scws_send_data(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  UIM_MSG_HIGH_2("qmi_cati_scws_close_channel : channel_data_result_present = 0x%x, slot_present = 0x%x",
                 tlv_data.channel_data_result_present, tlv_data.slot_present);

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send command to GSTK */
  if (tlv_data.channel_data_result_present)
  {
    if(tlv_data.channel_data_result.ch_id >= QMI_CATI_MAX_SCWS_CHANNEL)
    {
      errval = QMI_ERR_INVALID_ARG;
      UIM_MSG_ERR_1("channel id out of range 0x%x",
                    tlv_data.channel_data_result.ch_id);
      goto send_result;
    }

    gstk_status = gstk_scws_send_data_rsp(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           qmi_cati_global_ptr->ref_command_id[slot_index],
                           tlv_data.channel_data_result.ch_id,
                           tlv_data.channel_data_result.result);
  }
  else
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if (gstk_status != GSTK_SUCCESS)
  {
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_scws_send_data() */


/*===========================================================================
  FUNCTION QMI_CATI_SCWS_DATA_AVAILABLE()

  DESCRIPTION
    Send the confirmation with channel state for scws data available cmd

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_scws_data_available
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type                     * response                  = NULL;
  qmi_error_e_type                    errval                    = QMI_ERR_NONE;
  boolean                             retval                    = TRUE;
  qmi_result_e_type                   result                    = QMI_RESULT_SUCCESS;

   gstk_status_enum_type               gstk_status               = GSTK_SUCCESS;
  qmi_cat_in_scws_data_available_type tlv_data;
  gstk_generic_data_type              channel_data;
  uint8                               slot_index                = 0;
  qmi_cati_cmd_buf_cache_type       * new_node_ptr              = NULL;
  qmi_cati_userdata_type            * cb_userdata_ptr           = NULL;
  qmi_cati_ind_token_info_type        ind_token                 = {FALSE, 0};

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_scws_data_available_type));
  memset(&channel_data, 0, sizeof(gstk_generic_data_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_scws_data_available(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  UIM_MSG_HIGH_2(" qmi_cati_scws_close_channel : channel_data_present = 0x%x, slot_present = 0x%x",
                 tlv_data.channel_data_present, tlv_data.slot_present);

  /* Send Data Available channel data */
  channel_data.data_len = tlv_data.channel_data.data_len;
  channel_data.data_buffer_ptr = (uint8 *) &(tlv_data.channel_data.data[0]);

  /* Retrieve the slot index */
  if(tlv_data.slot_present)
  {
    errval = qmi_cati_convert_gstk_slot_id_to_qmi_slot_index(
                            (gstk_slot_id_enum_type)tlv_data.slot.slot,
                            &slot_index);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send command to GSTK */
  if (tlv_data.channel_data_present)
  {
    if(tlv_data.channel_data.ch_id >= QMI_CATI_MAX_SCWS_CHANNEL)
    {
      errval = QMI_ERR_INVALID_ARG;
      UIM_MSG_ERR_1("channel id out of range 0x%x",
                    tlv_data.channel_data.ch_id);
      goto send_result;
    }

    gstk_status = gstk_scws_data_avail_req_type(
                           qmi_cati_global_ptr->cat_gstk_client_id[slot_index],
                           tlv_data.channel_data.ch_id,
                           channel_data,
                           tlv_data.remain_data_len.data_len);
  }
  else
  {
    errval = QMI_ERR_MISSING_ARG;
    goto send_result;
  }

  if (gstk_status != GSTK_SUCCESS)
  {
    errval = qmi_cati_map_gstk_status_to_errval(gstk_status);
    goto send_result;
  }

  /* Caching scws data avail req command buffer*/
  new_node_ptr = uimqmi_malloc(sizeof(qmi_cati_cmd_buf_cache_type));

  if (new_node_ptr == NULL)
  {
    UIM_MSG_HIGH_0("qmi_cati_scws_data_available : failed to allocate memory");
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  /* Create the userdata */
  cb_userdata_ptr = qmi_cat_get_cb_userdata(CATI_CMD_VAL_SCWS_DATA_AVAILABLE, cmd_buf_p, ind_token);
  if (cb_userdata_ptr == NULL)
  {
    errval = QMI_ERR_NO_MEMORY;
    goto send_result;
  }

  memset(new_node_ptr, 0, sizeof(qmi_cati_cmd_buf_cache_type));
  new_node_ptr->next_ptr = qmi_cati_global_ptr->scws_cmd_buf[slot_index][tlv_data.channel_data.ch_id];
  new_node_ptr->cb_userdata_ptr = cb_userdata_ptr;

  qmi_cati_global_ptr->scws_cmd_buf[slot_index][tlv_data.channel_data.ch_id] = new_node_ptr;

  return QMI_SVC_RESPONSE_PENDING;

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_scws_data_available() */


/*===========================================================================
  FUNCTION QMI_CATI_GET_TERMINAL_PROFILE()

  DESCRIPTION
    To retrieve Terminal Profile configuration.

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_get_terminal_profile
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type                     *  response      = NULL;
  qmi_error_e_type                     errval        = QMI_ERR_NONE;
  qmi_result_e_type                    result        = QMI_RESULT_SUCCESS;
  boolean                              retval        = FALSE;
  gstk_status_enum_type                gstk_status   = GSTK_SUCCESS;

  uint8                                terminal_profile_cache[GSTK_TERMINAL_PROFILE_SIZE];
  qmi_cat_in_get_terminal_profile_type tlv_data;
  gstk_slot_id_enum_type               slot = GSTK_SLOT_ID_MAX;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  memset(&tlv_data, 0x00, sizeof(qmi_cat_in_get_terminal_profile_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_get_terminal_profile(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if(tlv_data.slot_present)
  {
    slot = (gstk_slot_id_enum_type)tlv_data.slot.slot;
  }
  else
  {
    slot = GSTK_SLOT_1;
  }

  gstk_status = gstk_io_ctrl(GSTK_IO_GET_TERMINAL_PROFILE_DATA,
                             &slot,
                             terminal_profile_cache);
  if(GSTK_SUCCESS != gstk_status)
  {
    if(GSTK_INSUFFICIENT_INFO == gstk_status)
    {
      errval = QMI_ERR_DEVICE_NOT_READY;
    }
    else
    {
      errval = QMI_ERR_INTERNAL;
    }
    goto send_result;
  }

  /* Add TLV to response */
  errval = qmi_cat_response_get_terminal_profile(&response,
                                                 (uint32)GSTK_TERMINAL_PROFILE_SIZE,
                                                 &terminal_profile_cache[0]);
  if (errval != QMI_ERR_NONE)
  {
    dsm_free_packet(&response);
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_get_terminal_profile() */


/*===========================================================================
  FUNCTION QMI_CATI_SET_CONFIGURATION()

  DESCRIPTION
    Set QMI_CAT configuration

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_set_configuration
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *                     response     = NULL;
  qmi_error_e_type                    errval       = QMI_ERR_NONE;
  boolean                             retval       = TRUE;
  qmi_result_e_type                   result       = QMI_RESULT_SUCCESS;
  qmi_cat_in_set_configuration_type   tlv_data;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);

  memset(&tlv_data, 0, sizeof(qmi_cat_in_set_configuration_type));

  /* Parse TLV */
  errval = qmi_cat_parse_request_set_configuration(sdu_in, &tlv_data);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if(tlv_data.config_mode.config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED ||
     tlv_data.config_mode.config_mode == CATI_CONFIG_MODE_CUSTOM_RAW)
  {
    if (!tlv_data.custom_tp_present)
    {
      UIM_MSG_ERR_1("custom tp TLV not present for custom config mode: 0x%x",
                    tlv_data.config_mode.config_mode);
      errval = QMI_ERR_MISSING_ARG;
      goto send_result;
    }

    if (tlv_data.custom_tp.tp_len > CATI_TLV_CUSTOM_TP_MAX)
    {
      UIM_MSG_ERR_0("custom tp len out of range");
      errval = QMI_ERR_INVALID_ARG;
      goto send_result;
    }

    /* Write to NV with the additionally padded zero bytes */
    errval = qmi_cati_store_tp_to_nv(tlv_data.custom_tp.tp_bitmask,
                                     sizeof(tlv_data.custom_tp.tp_bitmask));
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }
  else
  {
    if(tlv_data.custom_tp_present)
    {
      UIM_MSG_HIGH_1("Config mode : 0x%x doesn't support custom tp, ignoring TLV",
                     tlv_data.config_mode.config_mode);
    }
  }

  errval = qmi_cati_store_config_mode_to_nv(tlv_data.config_mode.config_mode);

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);

  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_set_configuration */


/*===========================================================================
  FUNCTION QMI_CATI_GET_CONFIGURATION()

  DESCRIPTION
    Get QMI_CAT configuration

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response or NULL if none.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_get_configuration
(
  void *              sp,
  void *              cmd_buf_p,
  void *              cl_sp,
  dsm_item_type **    sdu_in
)
{
  dsm_item_type *              response = NULL;
  boolean                      retval   = FALSE;
  qmi_error_e_type             errval   = QMI_ERR_NONE;
  qmi_result_e_type            result   = QMI_RESULT_SUCCESS;
  uint8                        tp_len   = 0;
  uint8                        tp[CATI_TLV_CUSTOM_TP_MAX];

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  /* Add TLV to response */
  if(qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_RAW ||
     qmi_cati_global_ptr->config_mode == CATI_CONFIG_MODE_CUSTOM_DECODED)
  {
    tp_len = qmi_cati_get_tp_from_nv(tp, sizeof(tp));
    if(tp_len > 0)
    {
      errval = qmi_cat_response_get_configuration(
                                         &response,
                                         qmi_cati_global_ptr->config_mode,
                                         tp_len,
                                         tp);
    }
    else
    {
      UIM_MSG_ERR_0("custom tp bitmask not set in NV");
      errval = QMI_ERR_INTERNAL;
    }
  }
  else
  {
    errval = qmi_cat_response_get_configuration(
                                       &response,
                                       qmi_cati_global_ptr->config_mode,
                                       0,
                                       NULL);
  }

  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_get_configuration() */


/*===========================================================================
  FUNCTION QMI_CATI_GET_CACHED_PROACTIVE_CMD()

  DESCRIPTION
    Fetches the requested proactive command from the recovery cache

  PARAMETERS
    sp            : service provided state pointer (user data)
    cmd_buf_p     : incoming request command buffer
    cl_sp         : client state pointer
    sdu_in        : incoming request

  RETURN VALUE
    ptr to response

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static dsm_item_type * qmi_cati_get_cached_proactive_cmd
(
  void            *sp,
  void            *cmd_buf_p,
  void            *cl_sp,
  dsm_item_type  **sdu_in
)
{
  dsm_item_type                             *response      = NULL;
  qmi_error_e_type                           errval        = QMI_ERR_NONE;
  qmi_result_e_type                          result        = QMI_RESULT_SUCCESS;
  boolean                                    retval        = FALSE;
  qmi_cat_in_get_cached_proactive_cmd_type   tlv_data_req;
  qmi_cati_raw_cache_type                   *raw_cache_ptr = NULL;
  uint8                                      slot_index    = 0;
  uint8                                      tlv_id        = 0;

  PACKED struct PACKED_POST
  {
    uint32  cmd_ref_id;
    uint16  raw_data_len;
    uint8   raw_data[CAT_RAW_MAX_DATA_LEN];
  } tlv_data_resp;

  ASSERT(sp);
  ASSERT(cmd_buf_p);
  ASSERT(cl_sp);
  ASSERT(sdu_in);
  ASSERT(qmi_cati_global_ptr);

  UIM_MSG_HIGH_1("qmi_cati_get_cached_proactive_cmd: config_mode=0x%x",
                 qmi_cati_global_ptr->config_mode);

  memset(&tlv_data_req, 0, sizeof(tlv_data_req));
  memset(&tlv_data_resp, 0, sizeof(tlv_data_resp));

  /* Parse TLV */
  errval = qmi_cat_parse_request_get_cached_proactive_cmd(sdu_in, &tlv_data_req);

  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Check configuration mode, currently supports Android mode and Custom Raw mode only */
  if (qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_ANDROID &&
      qmi_cati_global_ptr->config_mode != CATI_CONFIG_MODE_CUSTOM_RAW)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
    goto send_result;
  }

  /* Validate slot id */
  if (tlv_data_req.slot_present &&
      (tlv_data_req.slot.slot > qmi_cati_global_ptr->slot_count ||
       tlv_data_req.slot.slot > QMI_CAT_MAX_CARD_COUNT))
  {
    errval = QMI_ERR_INVALID_ARG;
    goto send_result;
  }

  /* Get slot_index */
  if (tlv_data_req.slot_present)
  {
    switch (tlv_data_req.slot.slot)
    {
      case CAT_SLOT1_V02:
        slot_index = 0;
        break;
      case CAT_SLOT2_V02:
        slot_index = 1;
        break;
      case CAT_SLOT3_V02:
        slot_index = 2;
        break;
      case CAT_SLOT4_V02:
        slot_index = 3;
        break;
      case CAT_SLOT5_V02:
        slot_index = 4;
        break;
      default:
        /* Won't get here since slot id is already validated */
        break;
    }
  }

  if (slot_index >= QMI_CAT_MAX_CARD_COUNT)
  {
    errval = QMI_ERR_OP_DEVICE_UNSUPPORTED;
    goto send_result;
  }

  /* Get proactive command from recovery cache only if proactive command is sent and
     the TR is not pending from the client, if TR is pending cached command will be
     sent during set event report no need to send again */
  switch (tlv_data_req.command_id.command_id)
  {
    case CATI_CACHED_CMD_TYPE_SETUP_MENU:
      if(qmi_cati_global_ptr->last_gstk_cmd[slot_index] != GSTK_SETUP_MENU_REQ)
      {
        tlv_id        = CATI_PRM_TYPE_CACHED_SETUP_MENU;
        raw_cache_ptr = qmi_cati_global_ptr->recovery_raw_cache[slot_index][CATI_QMI_CLIENT_CACHED_EVT_SETUP_MENU];
      }
      break;
    case CATI_CACHED_CMD_TYPE_SETUP_EVT_LIST:
      if(qmi_cati_global_ptr->last_gstk_cmd[slot_index] != GSTK_SETUP_EVENT_LIST_REQ)
      {
        tlv_id        = CATI_PRM_TYPE_CACHED_SETUP_EVT_LIST;
        raw_cache_ptr = qmi_cati_global_ptr->recovery_raw_cache[slot_index][CATI_QMI_CLIENT_CACHED_EVT_SETUP_EVT_LIST];
      }
      break;
    case CATI_CACHED_CMD_TYPE_SETUP_IDLE_MODE_TEXT:
      if(qmi_cati_global_ptr->last_gstk_cmd[slot_index] != GSTK_SETUP_IDLE_TEXT_REQ)
      {
        tlv_id        = CATI_PRM_TYPE_CACHED_SETUP_IDLE_MODE_TEXT;
        raw_cache_ptr = qmi_cati_global_ptr->recovery_raw_cache[slot_index][CATI_QMI_CLIENT_CACHED_EVT_SETUP_IDLE_MODE_TEXT];
      }
      break;
    default:
      errval = QMI_ERR_INVALID_ARG;
      break;
  }

  if (raw_cache_ptr == NULL)
  {
    /* Set QMI_ERR_INFO_UNAVAILABLE only if command_id is valid */
    if (QMI_ERR_NONE == errval)
    {
      errval = QMI_ERR_INFO_UNAVAILABLE;
    }
    goto send_result;
  }

  errval = qmi_cat_response_cached_proactive_cmd(
              tlv_id,
              &response,
              raw_cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.cmd_ref_id,
              raw_cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data_len,
              raw_cache_ptr->cached_event.cati_evt_rpt.cati_raw_pc_evt.raw_data);
  if (QMI_ERR_NONE != errval)
  {
    dsm_free_packet(&response);    
  }

send_result:
  result = (errval == QMI_ERR_NONE ? QMI_RESULT_SUCCESS
                                   : QMI_RESULT_FAILURE);
  retval = qmi_svc_put_result_tlv(&response, result, errval);
  CHECK_RETVAL();

  return response;
} /* qmi_cati_get_cached_proactive_cmd() */


/*===========================================================================
  FUNCTION QMI_CATI_SEND_INDICATION()

  DESCRIPTION
    This function is calls the QMI Framework API to send out the CAT service
    indication to client.

  PARAMETERS
    client_id: client ID
    cmd_type : type of indication
    ind      : dsm item

  RETURN VALUE
    TRUE  - For success
    FALSE - For qmi svc failure

  DEPENDENCIES
    QMI CAT service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_cati_send_indication
(
  uint8           client_id,
  uint16          cmd_type,
  dsm_item_type * ind
)
{
  qmi_framework_msg_hdr_type   framework_msg_hdr ;
  qmi_cati_client_state_type * cl_sp = NULL;

  ASSERT(ind);
  ASSERT(qmi_cat_state_ptr);

  if(client_id >  CATI_MAX_CLIDS ||
     client_id == QMI_SVC_CLID_UNUSED)
  {
    UIM_MSG_ERR_0("Invalid qmi_framework client_id message header");
    return FALSE;
  }

  cl_sp = qmi_cat_state_ptr->client[client_id - 1];
  if(cl_sp == NULL || cl_sp->service_id != QMUX_SERVICE_CAT)
  {
    UIM_MSG_ERR_0("Invalid qmi_framework message header");
    return FALSE;
  }

  /* for unicast indication, qmi framework maps the instance and no need to fill*/
  framework_msg_hdr.common_hdr.qmi_instance   = CATI_INVALID_INSTANCE;
  /* for indication the transaction ID can be ignored */
  framework_msg_hdr.common_hdr.transaction_id = 0;
  framework_msg_hdr.common_hdr.service        = (qmux_service_e_type)cl_sp->service_id;
  framework_msg_hdr.common_hdr.client_id      = client_id;
  framework_msg_hdr.msg_ctl_flag              = QMI_FLAG_MSGTYPE_IND;
  framework_msg_hdr.msg_len                   = (uint16) dsm_length_packet(ind);

  UIM_MSG_HIGH_2("Sending QMI CAT Indication to QMI Framework: clid:0x%x, ind_id:0x%x",
                 client_id, cmd_type);

  if(FALSE == qmi_svc_prepend_msg_hdr(&ind, cmd_type))
  {
    UIM_MSG_ERR_1("Unable to build indication for cmd 0x%x", cmd_type);
    return FALSE;
  }

  if(FALSE == qmi_framework_svc_send_ind(&framework_msg_hdr, ind))
  {
    UIM_MSG_ERR_0("Unable to send QMI CAT indication to QMI Framework" );
    return FALSE;
  }
  return TRUE;
} /* qmi_cati_send_indication */


/*===========================================================================
FUNCTION QMI_CATI_SEND_RESPONSE()

DESCRIPTION
  This function calls QMI Framework API to send a QMI CAT Service response to a
  clients.

PARAMETERS
  cmd_buf_p: command buffer
  msg_ptr  : dsm item

RETURN VALUE
  TRUE: If responses send to Framework
  FALSE: otherwise

DEPENDENCIES
  QMI CAT service must be initialized and registered with Framework

SIDE EFFECTS
  Frees cmd_buf_p
===========================================================================*/
static boolean qmi_cati_send_response
(
  qmi_cmd_buf_type           * cmd_buf_p,
  dsm_item_type              * msg_ptr
)
{
  qmi_transaction_type       * x_p;
  qmi_framework_msg_hdr_type   msg_hdr;
  qmi_cati_client_state_type * cl_sp;

  ASSERT (cmd_buf_p);
  ASSERT (cmd_buf_p->x_p);
  ASSERT (cmd_buf_p->x_p->cl_sp)
  ASSERT (msg_ptr);

  cl_sp = (qmi_cati_client_state_type *)cmd_buf_p->x_p->cl_sp;

  if(FALSE == qmi_svc_prepend_msg_hdr(&msg_ptr, cmd_buf_p->cmd_type))
  {
    UIM_MSG_ERR_1("Unable to send Response for cmd %d", cmd_buf_p->cmd_type);
    /* Freeing msg_ptr is done in qmi_svc_prepend_msg_hdr for failure */
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  /* Get the transaction from the command buffer*/
  x_p = cmd_buf_p->x_p;

  /* We do not support bundled commands only check the first command in list
     make sure we only have one command and be sure it's expecting a resp.
     validate service_id as well */
  if( (x_p->n_cmds != 1) ||
      (cmd_buf_p->cmd_no != 0) ||
      (cmd_buf_p != x_p->cmd_list[cmd_buf_p->cmd_no]) ||
      (x_p->resp_list[cmd_buf_p->cmd_no] != QMI_SVC_RESPONSE_PENDING) ||
      (cl_sp->service_id != QMUX_SERVICE_CAT))
  {
    UIM_MSG_ERR_0("Command is linked to transaction it doesn't belong to! discarding");
    dsm_free_packet(&msg_ptr);
    ds_qmi_fw_free_cmd_buf(&cmd_buf_p);
    return FALSE;
  }

  /* Freeup the transaction and the command buffer as it no longer used */
  ds_qmi_fw_free_cmd_buf(&cmd_buf_p);

  msg_hdr.common_hdr.client_id      = cl_sp->common.clid;
  msg_hdr.common_hdr.qmi_instance   = cl_sp->instance;
  msg_hdr.common_hdr.service        = (qmux_service_e_type)cl_sp->service_id;
  msg_hdr.common_hdr.transaction_id = x_p->x_id;

  msg_hdr.msg_ctl_flag = QMI_FLAG_MSGTYPE_RESP;

  msg_hdr.msg_len  = (uint16)dsm_length_packet(msg_ptr);

  /* qmi_framework_svc_send_response takes care of freeing msg_ptr */
  (void)qmi_framework_svc_send_response(&msg_hdr, msg_ptr);

  /* free the transaction buffer */
  ds_qmi_fw_free_transaction(&x_p);
  return TRUE;
} /* qmi_cati_send_response */


/*===========================================================================
  FUNCTION QMI_CAT_RECOVERY_COMPLETE()

  DESCRIPTION
    This function is called by the QMI UIM when a recovery indication
    has completed successfully

  PARAMETERS
    slot: Indicates the slot in which the recovery occured

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_cat_recovery_complete
(
  mmgsdi_slot_id_enum_type  slot
)
{
  qmi_cat_message_type * msg_ptr = NULL;

  UIM_MSG_HIGH_0("Received recovery indication");

  msg_ptr = qmi_cat_create_message(QMI_CAT_MESSAGE_RECOVERY_COMPLETE);
  if (msg_ptr)
  {
    msg_ptr->data.recovery_ind.slot = slot;
    qmi_cat_post_message(msg_ptr);
  }
} /* qmi_cat_recovery_complete*/


/*===========================================================================
FUNCTION: qmi_cat_deinit

DESCRIPTION:
  This function is used to free the qmicat private resources.
  This is called when GSTK is powering down.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void qmi_cat_deinit
(
  void
)
{
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
}/*qmi_cat_deinit */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
