/*================================================================================

DESCRIPTION


Copyright (c) 2009-2014, 2016 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimqmi/src/qmi_cat_decoded.c#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/16/16   vdc     Add indication support for TR and Envelope cmd
09/24/14   gm      Remove GOBI mode code
08/29/14   hh      Fix compliation warnings
08/05/14   hh      Added support to retrieve supported proactive commands
07/02/14   hh      Correct value for CATI_GET_EVENT_REPORT_COMMAND_TLV 
05/07/14   gm      Add break statement
04/22/14   at      Critical KW fixes
03/27/14   vr      Item with DCS is wrongly coded if text length is 0
03/17/14   gm      Added support for IMS communication control
01/10/14   df      Use v2.0 diag macros
10/09/13   tkl     Fixed handling of slot 3 in parsing slot tlv
10/08/13   gm      Removed unused comments
10/04/13   gm      F3 message reduction
06/12/13   kb      Added QMI malloc and free wrapper functions
05/17/13   tkl     Added support for Refresh Alpha
05/16/13   gm      Moved memcpy() and memmove() to memscpy() and memsmove()
04/30/13   tkl     Added support for 3rd party IMS client in raw format
04/10/13   tkl     Fix size for bip_status and resp_type TLV
03/13/13   tkl     Filter optional address TLV for cc_sms decoded envelope rsp
02/28/13   tkl     Added BIP status events
02/22/13   yt      Support for slot 3
01/28/13   tkl     Added BIP status events
11/09/12   yt      Move QMI CAT to UIM heap
10/11/12   at      Casting of variables to avoid compiler warnings
10/03/12   tkl     Added support for Browswer Termination event indication
10/02/12   tkl     Added SMS-PP download in envelope command
09/18/12   tkl     Added optional Alpha TLV for SCWS open channel ind
09/09/12   at      Removal of private DS functions and headers
09/07/12   tkl     Fixed Slot ID TLV for SCWS indications
05/23/12   hn      Enable 1-byte alignment packing on Windows environment
04/24/12   at      Correct function call when parsing decoded proactive cmd
03/21/12   at      Added browser termination event in decoded envelope command
03/09/12   tkl     Fixed Browser ID TLV
02/23/12   tkl     Removed dcs from network access name TLV
02/23/12   nmb     QMI QMUX Transition
01/19/12   at      Fixed TLV tag ids for QMI_CAT_SET_CONFIGURATION
01/17/12   mib     Fixed handling of default bearer in open channel command
01/10/12   vv      Fixed slot check for set_event_report request
01/10/12   tkl     Added support for set/get configuration
01/10/12   tkl     Added custom TP support
01/03/12   nb      Addded support to send status words for TR
01/03/11   nb      Addded support to retrieve terminal profile
12/20/11   hn      Add support for off-target testing
11/30/11   tkl     Added slot tlv check for set_event_report request
11/17/11   tkl     Fixed dcs type for alphabet TLV
11/16/11   tkl     Added APDU support for raw envelope
11/08/11   tkl     Added support for Class M and L
10/26/11   tkl     Added Items with dcs TLV
10/25/11   tkl     Added Get InKey Yes/No TLV for decoded TR
10/20/11   tkl     Fixed cc_bc_repeat TLV
06/06/11   tkl     Fixed TON and NPI in Address TLV with RFU values
06/05/11   vs      Decoded call control envelope
06/03/11   yt      Added slot id to QMI_CAT_SET_EVENT_REPORT
05/05/11   tkl     Fixed data_len in ch data TLV for decoded BIP command
04/20/11   tkl     Added SCWS support
04/11/11   tkl     Fixed invalid size for next action indicator TLV
02/20/11   tkl     Added BIP support in decoded format
01/14/11   mib     Review of parsing routines
01/04/11   mib     Fixed invalid TLV in Icon ID list
12/21/10   tkl     Fixed Alpha TLV in Select Item
11/03/10   mib     Fixed compiler warnings
10/06/10   tkl     Fixed extra optional TLV in launch browser indication
09/24/10   mib     Fixed compiler warnings
09/20/10   mib     Fixed Klocwork warnings
08/05/10   tkl     QMI UIM & QMI CAT split from data package
07/27/10   tkl     Removed extra NULL char at the end of USSD & DTMF string.
07/21/10   tkl     Added support to use tag for type of setup call cmd.
07/09/10   mib     Fixed redial TLV as mandatory TLV
07/07/10   tkl     Removed extra NULL char at the end of text string in
                   Select Item & Setup Menu
07/07/10   mib     Added provide local info in next action
07/05/10   mib     Fixed length of bearer list
06/22/10   tkl     Added support for LANGUAGE INFO tlv for PLI-Language and
                   fixed memcpy of item text from GSTK indication into QMI TLV
06/14/10   mib     Removed NULL termination from alpha strings
05/13/10   mib     Initial version
=============================================================================*/


#include "uim_variation.h"
#include "customer.h"
#include "comdef.h"
#include "dsm.h"
#include "amssassert.h"
#include "modem_mem.h"
#include "qmi_svc_utils.h"
#include "qmi_cat_decoded.h"
#include "qmi_uim_cat_common.h"
#include "gstk_p.h"
#include <stringl/stringl.h>
#include "uim_msg.h"

/* These includes need to always be at bottom - offtarget use only */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*---------------------------------------------------------------------------
  Tags used for proactive commands in decoded format
---------------------------------------------------------------------------*/
#define CATI_PRM_TYPE_END_PROACTIVE_SESSION           (0x1A)
#define CATI_PRM_TYPE_DECODED_HEADER_ID               (0x1B)
#define CATI_PRM_TYPE_TEXT_STRING                     (0x1C)
#define CATI_PRM_TYPE_HIGH_PRIORITY                   (0x1D)
#define CATI_PRM_TYPE_USER_CONTROL                    (0x1E)
#define CATI_PRM_TYPE_ICON                            (0x1F)
#define CATI_PRM_TYPE_DURATION                        (0x20)
#define CATI_PRM_TYPE_RESPONSE_FORMAT                 (0x21)
#define CATI_PRM_TYPE_HELP_AVAILABLE                  (0x22)
#define CATI_PRM_TYPE_RESPONSE_PACKING_FORMAT         (0x23)
#define CATI_PRM_TYPE_RESPONSE_LENGTH                 (0x24)
#define CATI_PRM_TYPE_SHOW_USER_INPUT                 (0x25)
#define CATI_PRM_TYPE_TONE                            (0x26)
#define CATI_PRM_TYPE_SOFT_KEY_SELECTION              (0x27)
#define CATI_PRM_TYPE_MENU_ITEMS                      (0x28)
#define CATI_PRM_TYPE_DEFAULT_ITEM                    (0x29)
#define CATI_PRM_TYPE_NEXT_ACTION_INDICATOR           (0x2A)
#define CATI_PRM_TYPE_ICON_ID_LIST                    (0x2B)
#define CATI_PRM_TYPE_PRESENTATION                    (0x2C)
#define CATI_PRM_TYPE_PACKING_REQUIRED                (0x2D)
#define CATI_PRM_TYPE_SMS_TPDU                        (0x2E)
#define CATI_PRM_TYPE_IS_CDMA_SMS                     (0x2F)
#define CATI_PRM_TYPE_ADDRESS                         (0x30)
#define CATI_PRM_TYPE_CALL_SETUP_REQUIREMENT          (0x31)
#define CATI_PRM_TYPE_REDIAL                          (0x32)
#define CATI_PRM_TYPE_SUBADDRESS                      (0x33)
#define CATI_PRM_TYPE_CAPABILITY_CONFIG               (0x34)
#define CATI_PRM_TYPE_DTMF                            (0x35)
#define CATI_PRM_TYPE_SPECIFIC_LANG_NOTIFY            (0x36)
#define CATI_PRM_TYPE_LANGUAGE                        (0x37)
#define CATI_PRM_TYPE_LAUNCH_MODE                     (0x38)
#define CATI_PRM_TYPE_URL                             (0x39)
#define CATI_PRM_TYPE_BROWSER_ID                      (0x3A)
#define CATI_PRM_TYPE_BEARER_ID                       (0x3B)
#define CATI_PRM_TYPE_NUM_OF_PROVISIONING_FILES       (0x3C)
#define CATI_PRM_TYPE_USSD_STRING                     (0x3D)
#define CATI_PRM_TYPE_DEFAULT_TEXT                    (0x3E)
#define CATI_PRM_TYPE_IMMEDIATE_RESPONSE_REQ          (0x3F)
#define CATI_PRM_TYPE_USER_CNF_ALPHA                  (0x40)
#define CATI_PRM_TYPE_SETUP_CALL_DISPLAY_ALPHA        (0x41)
#define CATI_PRM_TYPE_USER_CONF_ICON                  (0x42)
#define CATI_PRM_TYPE_SETUP_CALL_DISPLAY_ICON         (0x43)
#define CATI_PRM_TYPE_GATEWAY_PROXY                   (0x44)
#define CATI_PRM_TYPE_ALPHA                           (0x45)
#define CATI_PRM_TYPE_REQUIRE_NOTIFICATION            (0x46)
#define CATI_PRM_TYPE_SLOT_ID                         (0x50)
#define CATI_PRM_TYPE_ON_DEMAND_LINK_EST              (0x55)
#define CATI_PRM_TYPE_CDS_BEARER                      (0x56)
#define CATI_PRM_TYPE_GPRS_BEARER                     (0x57)
#define CATI_PRM_TYPE_EUTRAN_EXT_PARAMS_BEARER        (0x58)
#define CATI_PRM_TYPE_EUTRAN_MAPPED_UTRAN_PS_BEARER   (0x59)
#define CATI_PRM_TYPE_BUFFER_SIZE                     (0x5A)
#define CATI_PRM_TYPE_NETWORK_ACCESS_NAME             (0x5B)
#define CATI_PRM_TYPE_OTHER_ADDRESS                   (0x5C)
#define CATI_PRM_TYPE_USER_LOGIN                      (0x5D)
#define CATI_PRM_TYPE_USER_PASSWORD                   (0x5E)
#define CATI_PRM_TYPE_TRANSPORT_LEVEL                 (0x5F)
#define CATI_PRM_TYPE_DATA_DEST_ADDRESS               (0x60)
#define CATI_PRM_TYPE_CHANNEL_DATA_LEN                (0x61)
#define CATI_PRM_TYPE_SEND_DATA_IMMEDIATELY           (0x62)
#define CATI_PRM_TYPE_CHANNEL_DATA                    (0x63)
#define CATI_PRM_TYPE_CHANNEL_ID                      (0x64)
#define CATI_PRM_TYPE_MENU_ITEMS_WITH_DCS             (0x65)
#define CATI_PRM_TYPE_ACTIVATE_TARGET                 (0x67)
#define CATI_PRM_TYPE_RESPONSE_TYPE                   (0x68)
#define CATI_PRM_TYPE_BIP_STATUS                      (0x69)

/*---------------------------------------------------------------------------
  Tags used for scws commands in decoded format
---------------------------------------------------------------------------*/
#define CATI_PRM_TYPE_SCWS_OPEN_CHANNEL_INFO          (0x10)
#define CATI_PRM_TYPE_SCWS_SLOT_ID                    (0x11)
#define CATI_PRM_TYPE_SCWS_ALPHA                      (0x12)
#define CATI_PRM_TYPE_SCWS_CHANNEL_STATE              (0x10)
#define CATI_PRM_TYPE_SCWS_CHANNEL_DATA               (0x10)

/*----------------------------------------------------------
  Tags used for decoded envelope response
----------------------------------------------------------*/
#define CATI_PRM_TYPE_CC_RESULT                     (0x10)
#define CATI_PRM_TYPE_CC_ADDRESS                    (0x11)
#define CATI_PRM_TYPE_CC_SUBADDRESS                 (0x12)
#define CATI_PRM_TYPE_CC_CCP1                       (0x13)
#define CATI_PRM_TYPE_CC_CCP2                       (0x14)
#define CATI_PRM_TYPE_CC_USSD_STR                   (0x15)
#define CATI_PRM_TYPE_CC_PDP_ACT                    (0x16)
#define CATI_PRM_TYPE_CC_EPS_PDN_ACT                (0x17)
#define CATI_PRM_TYPE_CC_ALPHA                      (0x18)
#define CATI_PRM_TYPE_CC_BC_REPEAT_IND              (0x19)
#define CATI_PRM_TYPE_SMS_PP_UICC_ACK               (0x1A)
#define CATI_PRM_TYPE_RP_ADDRESS                    (0x1B)
#define CATI_PRM_TYPE_TP_ADDRESS                    (0x1C)
#define CATI_PRM_TYPE_CC_IMS_REQUEST_URI            (0x1D)
#define CATI_PRM_TYPE_DECODED_ENV_RSP_TOKEN         (0x1E)

/*----------------------------------------------------------
  Tags used for decoded envelope indication
----------------------------------------------------------*/
#define CATI_PRM_TYPE_DECODED_ENV_IND_TOKEN         (0x01)

/*----------------------------------------------------------
  Tags used for raw envelope response
----------------------------------------------------------*/
#define CATI_PRM_TYPE_RAW_ENV_RSP                   (0x10)
#define CATI_PRM_TYPE_RAW_ENV_RSP_TOKEN             (0x11)

/*----------------------------------------------------------
  Tags used for raw envelope indication
----------------------------------------------------------*/
#define CATI_PRM_TYPE_RAW_ENV_IND_TOKEN             (0x01)

/*---------------------------------------------------------------------------
  Sizes used for TLVs
---------------------------------------------------------------------------*/
#define CATI_TLV_BEARER_LIST_MAX                      (255)
#define CATI_TLV_ICON_SIZE_MAX                        (512)
#define CATI_TLV_FILE_PATH_MAX                        (255)
#define CATI_TLV_URL_MAX                              (255)
#define CATI_TLV_NEXT_ACTION_MAX                      (255)
#define CATI_TLV_DATA_ADDRESS_MAX                     (16)
#define CATI_TLV_DTMF_MAX                             (255)
#define CATI_TLV_LANGUAGE_MAX                         (2)
#define CATI_TLV_BIP_DATA_MAX                         (255)
#define CATI_TLV_RAW_ENV_DATA_MAX                     (255)
#define CATI_TLV_RAW_TR_DATA_MAX                      (255)
#define CATI_TLV_SMS_PP_UICC_ACK_MAX_LENGTH           (128)


/*---------------------------------------------------------------------------
  Response format
---------------------------------------------------------------------------*/
#define CATI_RESPONSE_FORMAT_SMS_DEF_ALPHABET         (0x00)
#define CATI_RESPONSE_FORMAT_YES_NO                   (0x01)
#define CATI_RESPONSE_FORMAT_NUMBERICAL_ONLY          (0x02)
#define CATI_RESPONSE_FORMAT_UCS2_ALPHABET            (0x03)
#define CATI_RESPONSE_FORMAT_IMMEDIATE_DIGIT_RSP      (0x04)
#define CATI_RESPONSE_FORMAT_YES_NO_AND_IMM_DIGT_RSP  (0x05)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SET_EVENT_REPORT
---------------------------------------------------------------------------*/
#define CATI_SET_EVENT_REPORT_RAW_TLV                 (0x10)
#define CATI_SET_EVENT_REPORT_DECODED_TLV             (0x11)
#define CATI_SET_EVENT_REPORT_SLOT_TLV                (0x12)
#define CATI_SET_EVENT_REPORT_FULL_FUNC_TLV           (0x13)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_GET_EVENT_REPORT
---------------------------------------------------------------------------*/
#define CATI_GET_EVENT_REPORT_COMMAND_TLV             (0x01)

/*---------------------------------------------------------------------------
  TLV IDs for SET_EVENT_REPORT
---------------------------------------------------------------------------*/
#define CATI_SET_EVENT_REPORT_RAW_RESP_TLV            (0x10)
#define CATI_SET_EVENT_REPORT_DECODED_RESP_TLV        (0x11)
#define CATI_SET_EVENT_REPORT_FULL_FUNC_RESP_TLV      (0x12)

/*---------------------------------------------------------------------------
  TLV IDs for GET_SERVICE_STATE
---------------------------------------------------------------------------*/
#define CATI_GET_SERVICE_STATE_RAW_TLV                (0x01)
#define CATI_GET_SERVICE_STATE_DECODED_TLV            (0x10)
#define CATI_GET_SERVICE_STATE_FULL_FUNC_TLV          (0x11)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_TR
---------------------------------------------------------------------------*/
#define CATI_RAW_TR_TERMINAL_RESPONSE_TLV             (0x01)
#define CATI_RAW_TR_SLOT_TLV                          (0x10)
#define CATI_RAW_TR_INDICATION_TOKEN_TLV              (0x11)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_TR_RSP
---------------------------------------------------------------------------*/
#define CATI_RAW_TR_RSP_TR_RESPONSE_TLV               (0x10)
#define CATI_RAW_TR_RSP_TOKEN_TLV                     (0x11)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_TR_IND
---------------------------------------------------------------------------*/
#define CATI_RAW_TR_IND_TOKEN_TLV                     (0x01)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_ENVELOPE_CMD
---------------------------------------------------------------------------*/
#define CATI_RAW_ENVELOPE_ENVELOPE_TLV                (0x01)
#define CATI_RAW_ENVELOPE_SLOT_TLV                    (0x10)
#define CATI_RAW_ENVELOPE_INDICATION_TOKEN_TLV        (0x11)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_DECODED_TR
---------------------------------------------------------------------------*/
#define CATI_DECODED_TR_TERMINAL_RESPONSE_TLV         (0x01)
#define CATI_DECODED_TR_TEXT_STRING_TLV               (0x10)
#define CATI_DECODED_TR_ITEM_IDENTIFIER_TLV           (0x11)
#define CATI_DECODED_TR_GET_INKEY_EXTRA_TLV           (0x12)
#define CATI_DECODED_TR_LANGUAGE_TLV                  (0x13)
#define CATI_DECODED_TR_SLOT_TLV                      (0x14)
#define CATI_DECODED_TR_GET_INKEY_YES_NO_TLV          (0x15)
#define CATI_DECODED_TR_INDICATION_TOKEN_TLV          (0x17)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_DECODED_TR_RSP
---------------------------------------------------------------------------*/
#define CATI_DECODED_TR_RSP_TR_RESPONSE_TLV           (0x10)
#define CATI_DECODED_TR_RSP_TOKEN_TLV                 (0x11)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SEND_DECODED_TR_IND
---------------------------------------------------------------------------*/
#define CATI_DECODED_TR_IND_TOKEN_TLV                 (0x01)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_DECODED_ENVELOPE_CMD
---------------------------------------------------------------------------*/
#define CATI_TYPE_ENVELOPE_REQUEST_REQUIRED           (0x01)
#define CATI_ENV_TYPE_ITEM_CHOSEN                     (0x10)
#define CATI_ENV_TYPE_HELP_REQUESTED                  (0x11)
#define CATI_ENV_TYPE_LANGUAGE                        (0x12)
#define CATI_ENV_TYPE_SLOT                            (0x13)
#define CATI_ENV_TYPE_ADDRESS                         (0x14)
#define CATI_ENV_TYPE_SUBADDRESS                      (0x15)
#define CATI_ENV_TYPE_CAPABILITY_CONF_1               (0x16)
#define CATI_ENV_TYPE_CAPABILITY_CONF_2               (0x17)
#define CATI_ENV_TYPE_USSD_STRING                     (0x18)
#define CATI_ENV_TYPE_PDP_CONTEXT_ACT                 (0x19)
#define CATI_ENV_TYPE_EPS_PDN_CONNECT_ACT             (0x1A)
#define CATI_ENV_TYPE_BROWSER_TERM_CAUSE              (0x1B)
#define CATI_ENV_TYPE_SMS_TPDU                        (0x1C)
#define CATI_ENV_TYPE_IS_CDMA_SMS                     (0x1D)
#define CATI_ENV_TYPE_RAT                             (0x1E)
#define CATI_ENV_TYPE_CALL_TYPE                       (0x1F)
#define CATI_ENV_TYPE_TRANSACTION_ID                  (0x20)
#define CATI_ENV_TYPE_RP_ADDRESS                      (0x21)
#define CATI_ENV_TYPE_TP_ADDRESS                      (0x22)
#define CATI_ENV_TYPE_CAUSE                           (0x23)
#define CATI_ENV_TYPE_IMS_REQUEST_URI                 (0x24)
#define CATI_ENV_TYPE_INDICATION_TOKEN                (0x26)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_EVENT_CONFIRMATION
---------------------------------------------------------------------------*/
#define CATI_EVENT_CONFIRMATION_USER_CONFIRMED_TLV    (0x10)
#define CATI_EVENT_CONFIRMATION_ICON_DISPLAYED_TLV    (0x11)
#define CATI_EVENT_CONFIRMATION_SLOT_TLV              (0x12)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SCWS_OPEN_CHANNEL
  TLV tag ids for QMI_CAT_SCWS_CLOSE_CHANNEL
  TLV tag ids for QMI_CAT_SCWS_CHANNEL_STATUS
---------------------------------------------------------------------------*/
#define CATI_SCWS_CHANNEL_STATE_TLV                   (0x01)
#define CATI_SCWS_CHANNEL_STATE_SLOT_TLV              (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SCWS_SEND_DATA
---------------------------------------------------------------------------*/
#define CATI_SCWS_SEND_DATA_CHANNEL_DATA_RESULT_TLV   (0x01)
#define CATI_SCWS_SEND_DATA_SLOT_TLV                  (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SCWS_DATA_AVAILABLE
---------------------------------------------------------------------------*/
#define CATI_SCWS_DATA_AVAILABLE_CHANNEL_DATA_TLV     (0x01)
#define CATI_SCWS_DATA_AVAILABLE_REMAIN_DATA_LEN_TLV  (0x02)
#define CATI_SCWS_DATA_AVAILABLE_SLOT_TLV             (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for CATI_GET_TERMINAL_PROFILE_SLOT_TLV
---------------------------------------------------------------------------*/
#define CATI_GET_TERMINAL_PROFILE_SLOT_TLV            (0x10)

/*---------------------------------------------------------------------------
  TLV IDs for GET_TERMINAL_PROFILE
---------------------------------------------------------------------------*/
#define CATI_GET_TERMINAL_PROFILE_DATA_TLV            (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_SET_CONFIGURATION
---------------------------------------------------------------------------*/
#define CATI_SET_CONFIGURATION_CONFIG_MODE_TLV        (0x01)
#define CATI_SET_CONFIGURATION_CUSTOM_TP_TLV          (0x10)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_GET_CONFIGURATION
---------------------------------------------------------------------------*/
#define CATI_GET_CONFIGURATION_CONFIG_MODE_TLV        (0x10)
#define CATI_GET_CONFIGURATION_CUSTOM_TP_TLV          (0x11)

/*---------------------------------------------------------------------------
  TLV tag ids for QMI_CAT_GET_CACHED_PROACTIVE_CMD
---------------------------------------------------------------------------*/
#define CATI_GET_CACHED_PROACTIVE_CMD_CMD_TLV         (0x01)
#define CATI_GET_CACHED_PROACTIVE_CMD_SLOT_TLV        (0x10)

/*---------------------------------------------------------------------------
  Misc
---------------------------------------------------------------------------*/
#define QMI_CATI_SMS_GENERAL_DATA_CODING_MASK           0xCC
#define QMI_CATI_SMS_8_BIT_GENERAL_DATA_CODING_DCS      0x04
#define QMI_CATI_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS 0x08
#define QMI_CATI_SMS_DATA_CODING_MASK                   0xFC
#define QMI_CATI_SMS_8_BIT_DCS                          0xF4

#define QMI_CATI_CBS_DATA_CODING_MASK                   0xFC
#define QMI_CATI_CBS_8_BIT_DCS                          0xF4
#define QMI_CATI_CBS_GENERAL_DATA_CODING_MASK           0xCC
#define QMI_CATI_CBS_8_BIT_GENERAL_DATA_CODING_DCS      0x44
#define QMI_CATI_CBS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS 0x48
#define QMI_CATI_CBS_UCS2_7_BIT_DCS                     0x11

/*---------------------------------------------------------------------------
  Command id in TLV for decoded proactive command
---------------------------------------------------------------------------*/
#define CATI_EVENT_DISPLAY_TEXT_REQ                     0x01
#define CATI_EVENT_GET_INKEY_REQ                        0x02
#define CATI_EVENT_GET_INPUT_REQ                        0x03
#define CATI_EVENT_LAUNCH_BROWSER_REQ                   0x04
#define CATI_EVENT_PLAY_TONE_REQ                        0x05
#define CATI_EVENT_SELECT_ITEM_REQ                      0x06
#define CATI_EVENT_SEND_SMS_REQ                         0x07
#define CATI_EVENT_SEND_SS_REQ                          0x08
#define CATI_EVENT_SEND_USSD_REQ                        0x09
#define CATI_EVENT_SETUP_CALL_USER_CONFIRMATION_REQ     0x0A
#define CATI_EVENT_SETUP_CALL_ALPHA_DISPLAY_REQ         0x0B
#define CATI_EVENT_SETUP_MENU_REQ                       0x0C
#define CATI_EVENT_SETUP_IDLE_TEXT_REQ                  0x0D
#define CATI_EVENT_PROVIDE_LANGUAGE_INFO_REQ            0x0E
#define CATI_EVENT_SEND_DTMF_REQ                        0x0F
#define CATI_EVENT_LANGUAGE_NOTIFICATION_REQ            0x10
#define CATI_EVENT_SETUP_EVENT_USER_ACTIVITY_REQ        0x11
#define CATI_EVENT_SETUP_EVENT_IDLE_SCREEN_REQ          0x12
#define CATI_EVENT_SETUP_EVENT_LANGUAGE_SEL_REQ         0x13
#define CATI_EVENT_OPEN_CHANNEL_REQ                     0x14
#define CATI_EVENT_CLOSE_CHANNEL_REQ                    0x15
#define CATI_EVENT_RECEIVE_DATA_REQ                     0x16
#define CATI_EVENT_SEND_DATA_REQ                        0x17
#define CATI_EVENT_ACTIVATE_REQ                         0x18
#define CATI_EVENT_SETUP_EVENT_HCI_CONN_REQ             0x19
#define CATI_EVENT_REFRESH_ALPHA_REQ                    0x1A
#define CATI_EVENT_SETUP_EVENT_BROWSER_TERM_REQ         0x20


/*===========================================================================

                             MACROS

===========================================================================*/

/*===========================================================================
  This macro is used to skip a TLV
===========================================================================*/
#define QMI_CAT_SKIP_TLV(sdu_in, len, result)                                \
        UIM_MSG_HIGH_0("TLV type not required");                             \
        /* We need to pullout this item */                                   \
        if (len != dsm_pullup(sdu_in, NULL, len))                            \
        {                                                                    \
          UIM_MSG_ERR_0("QMI_CAT DSM pullout error");                        \
          result = QMI_ERR_MALFORMED_MSG;                                    \
        }


/*---------------------------------------------------------------------------
  Macro is used to check slot id TLV
---------------------------------------------------------------------------*/
#define QMI_CAT_CHECK_SLOT_ID_TLV(ptr, result)                               \
        /* Check for Slot id TLV range as defined in QMI CAT ICD*/           \
        if (ptr->slot_present &&                                             \
            ptr->slot.slot != 1 &&                                           \
            ptr->slot.slot != 2 &&                                           \
            ptr->slot.slot != 3 &&                                           \
            ptr->slot.slot != 4 &&                                           \
            ptr->slot.slot != 5 )                                            \
        {                                                                    \
          UIM_MSG_ERR_0("QMI_CAT invalid slot id TLV");                      \
          result = QMI_ERR_INVALID_ARG;                                      \
        }

/*===========================================================================

                          FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CATI_CONVERT_GSTK_DCS_TO_QMI

  DESCRIPTION
    to convert from GSTK to QMI DCS value

  PARAMETERS
    gstk_dcs : GSTK DCS value
    qmi_dcs  : QMI DCS value
    cb       : Cb dcs
  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_cati_convert_gstk_dcs_to_qmi
(
  uint8      gstk_dcs,
  uint8   *  qmi_dcs,
  boolean    cb
)
{
  ASSERT(qmi_dcs != NULL);

  if (!cb)
  {
    if((gstk_dcs & QMI_CATI_SMS_GENERAL_DATA_CODING_MASK) == QMI_CATI_SMS_8_BIT_GENERAL_DATA_CODING_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_8_BIT;
      return;
    }
    if((gstk_dcs & QMI_CATI_SMS_GENERAL_DATA_CODING_MASK) == QMI_CATI_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_UCS2_8_BIT;
       return;
    }
    if((gstk_dcs & QMI_CATI_SMS_DATA_CODING_MASK) == QMI_CATI_SMS_8_BIT_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_8_BIT;
      return;
    }
  }
  else
  {
    if ((gstk_dcs & QMI_CATI_CBS_DATA_CODING_MASK) == QMI_CATI_CBS_8_BIT_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_8_BIT;
      return;
    }
    if ((gstk_dcs & QMI_CATI_CBS_GENERAL_DATA_CODING_MASK) == QMI_CATI_CBS_8_BIT_GENERAL_DATA_CODING_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_8_BIT;
      return;
    }
    if ((gstk_dcs & QMI_CATI_CBS_GENERAL_DATA_CODING_MASK) == QMI_CATI_CBS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_UCS2_8_BIT;
      return;
    }
    if ((gstk_dcs & QMI_CATI_CBS_UCS2_7_BIT_DCS) == QMI_CATI_CBS_UCS2_7_BIT_DCS)
    {
      *qmi_dcs = (uint8)CATI_DCS_UCS2_7_BIT;
      return;
    }
  }

  *qmi_dcs = (uint8)CATI_DCS_7_BIT;
} /* qmi_cati_convert_gstk_dcs_to_qmi */


/*===========================================================================
  FUNCTION QMI_CATI_SLOT_ID_TLV

  DESCRIPTION
    Adds the TLV for the slot id

  PARAMETERS
    slot_id : slot id
    ind     :  tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_slot_id_tlv
(
  gstk_slot_id_enum_type         slot_id,
  dsm_item_type               ** ind,
  uint8                          prm_type
)
{
  PACKED struct PACKED_POST
  {
    uint8       slot_id;
  } tlv_data;

  switch(slot_id)
  {
    case GSTK_SLOT_1:
      tlv_data.slot_id = 1;
      break;
    case GSTK_SLOT_2:
      tlv_data.slot_id = 2;
      break;
    case GSTK_SLOT_3:
      tlv_data.slot_id = 3;
      break;
    default:
      /* Invalid slot id... return SUCCESS, without adding any TLV. */
      return QMI_ERR_NONE;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     prm_type,
                                     sizeof(tlv_data),
                                     (void *)&tlv_data) )
  {
    UIM_MSG_MED_0("Unable to send SLot id TLV!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_slot_id_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_RESPONSE_TLV

  DESCRIPTION
      Adds the TLV for the response tlv

  PARAMETERS
    HDR Tag : hdr tag
    ind     :  tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_response_tlv
(
  gstk_tag_enum_type             tag,
  dsm_item_type               ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint32       tag;
  } tlv_data;

  switch(tag)
  {
    case GSTK_TAG_ORIGINAL_CMD:
      tlv_data.tag = 0;
      break;
    case GSTK_TAG_USER_CONF_ALPHA:
    case GSTK_TAG_DISPLAY_ALPHA:
      tlv_data.tag = 1;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_RESPONSE_TYPE,
                                     sizeof(tlv_data),
                                     (void *)&tlv_data) )
  {
    UIM_MSG_MED_0("Unable to send RESPONSE TLV!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_response_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_PRO_CMD_HEADER_TLV

  DESCRIPTION
    to send the proactive command header tlv

  PARAMETERS
    event_id : QMI event id
    gstk_hdr : GSTK header
    ind      : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_pro_cmd_header_tlv
(
  uint8                          qmi_event_id,
  const gstk_exp_hdr_type        gstk_hdr,
  dsm_item_type               ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8       command_id;
    uint32      uim_ref_id;
    uint8       command_number;
  } cmd_header_tlv;

  cmd_header_tlv.command_id       = qmi_event_id;
  cmd_header_tlv.uim_ref_id       = gstk_hdr.cmd_detail_reference;
  cmd_header_tlv.command_number   = gstk_hdr.command_number;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_DECODED_HEADER_ID,
                                     sizeof(cmd_header_tlv),
                                     (void *)&cmd_header_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_DECODED_HEADER_ID!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_pro_cmd_header_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_TEXT_STRING_TLV

  DESCRIPTION
    to send the text string tlv

  PARAMETERS
    gstk_dcs : Decoded text string
    ind      : tlv
    prm_type : Type of text

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_text_string_tlv
(
  const gstk_string_type  * gstk_text_string,
  dsm_item_type          ** ind,
  uint8                     prm_type
)
{
  PACKED struct PACKED_POST
  {
    uint8                    dcs;
    uint8                    length_of_string;
    uint8                    text[CAT_TLV_TEXT_STRING_MAX];
  } text_string_tlv;

  ASSERT(gstk_text_string != NULL);

  qmi_cati_convert_gstk_dcs_to_qmi(gstk_text_string->dcs,
                                   (uint8 *)&text_string_tlv.dcs,
                                   FALSE);

  text_string_tlv.length_of_string = gstk_text_string->length;

  /* GSTK adds an extra bite at the end with NULL termination for legacy
     clients. We need to remove it, if it's present */
  if (gstk_text_string->length > 0 &&
      gstk_text_string->text != NULL &&
      gstk_text_string->text[text_string_tlv.length_of_string - 1] == 0x0)
  {
    text_string_tlv.length_of_string--;
  }

  /* No need to check max size... length_of_string is uint8 and
     CAT_TLV_TEXT_STRING_MAX is 255 */
  if (text_string_tlv.length_of_string > 0)
  {
    if (gstk_text_string->text != NULL)
    {
      (void)memscpy((void*)text_string_tlv.text,
                    sizeof(text_string_tlv.text),
                    gstk_text_string->text,
                    text_string_tlv.length_of_string);
    }
    else
    {
      return QMI_ERR_INTERNAL;
    }
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     prm_type,
                                     sizeof(uint8) + sizeof(uint8) + text_string_tlv.length_of_string,
                                     (void *)&text_string_tlv) )
  {
    UIM_MSG_MED_0("Unable to send text string!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_text_string_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_HIGH_PRIORITY_TLV

  DESCRIPTION
    to send the high priority tlv

  PARAMETERS
    gstk_high_priority : High priority boolean
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_high_priority_tlv
(
  boolean          gstk_high_priority,
  dsm_item_type ** ind
)
{
  uint8 high_priority_tlv;

  if (gstk_high_priority)
  {
    high_priority_tlv = 1;
  }
  else
  {
    high_priority_tlv = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_HIGH_PRIORITY,
                                     sizeof(high_priority_tlv),
                                     (void *)&high_priority_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_HIGH_PRIORITY!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_high_priority_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_USER_CONTROL_TLV

  DESCRIPTION
    to send the user control tlv

  PARAMETERS
    gstk_user_control : User control boolean
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_user_control_tlv
(
  boolean          gstk_user_control,
  dsm_item_type ** ind
)
{
  uint8 user_control_tlv;

  if (gstk_user_control)
  {
    user_control_tlv = 1;
  }
  else
  {
    user_control_tlv = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_USER_CONTROL,
                                     sizeof(user_control_tlv),
                                     (void *)&user_control_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_USER_CONTROL!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_user_control_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_IMMEDIATE_RESPONSE_TLV

  DESCRIPTION
    to send the immediate_response tlv

  PARAMETERS
    gstk_immediate_response: Immediate response boolean
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_immediate_response_tlv
(
  boolean          gstk_immediate_response,
  dsm_item_type ** ind
)
{
  uint8 immediate_response_tlv;

  if (gstk_immediate_response)
  {
    immediate_response_tlv = 1;
  }
  else
  {
    immediate_response_tlv = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_IMMEDIATE_RESPONSE_REQ,
                                     sizeof(immediate_response_tlv),
                                     (void *)&immediate_response_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_IMMEDIATE_RESPONSE_REQ!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_immediate_response_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_ICON_TLV

  DESCRIPTION
    to send the icon tlv

  PARAMETERS
    gstk_icon : Icon structure
    ind : tlv
    prm_type: type of icon

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_icon_tlv
(
  const gstk_icon_type  * gstk_icon,
  dsm_item_type        ** ind,
  uint8                   prm_type
)
{
  PACKED struct PACKED_POST
  {
    uint8       qualifier;
    uint8       height;
    uint8       width;
    uint8       ics;
    uint8       rec_num;
    uint16      data_size;
    uint8       data[CATI_TLV_ICON_SIZE_MAX];
  } icon_tlv;

  ASSERT(gstk_icon != NULL);

  memset((void *)&icon_tlv, 0, sizeof(icon_tlv));

  icon_tlv.qualifier = gstk_icon->qualifier;
  icon_tlv.height    = gstk_icon->height;
  icon_tlv.width     = gstk_icon->width;
  icon_tlv.data_size = (uint16)gstk_icon->size;
  icon_tlv.rec_num   = gstk_icon->rec_num;

  /* Color scheme */
  switch(gstk_icon->image_coding_scheme)
  {
    case GSTK_UNKNOWN_IMAGE_CODE_SCHEME:
      icon_tlv.ics = 0;
      break;
    case GSTK_BASIC_IMAGE_CODE_SCHEME:
      icon_tlv.ics = 1;
      break;
    case GSTK_COLOUR_IMAGE_CODE_SCHEME:
      icon_tlv.ics = 2;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }

  /* Data */
  if (gstk_icon->size > CATI_TLV_ICON_SIZE_MAX)
  {
    return QMI_ERR_INTERNAL;
  }
  if (gstk_icon->size > 0 &&
      gstk_icon->data != NULL)
  {
    (void)memscpy((void*)icon_tlv.data,
                  sizeof(icon_tlv.data),
                  gstk_icon->data,
                  gstk_icon->size);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     prm_type,
                                     (5 * sizeof(uint8)) + sizeof(uint16) + (uint16)gstk_icon->size,
                                     (void *)&icon_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_ICON!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_icon_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_DURATION_TLV

  DESCRIPTION
    to send the duration tlv

  PARAMETERS
    gstk_durstion : GSTK duration structure


  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_duration_tlv
(
  gstk_duration_type    gstk_duration,
  dsm_item_type      ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8   units;
    uint8   interval;
  } duration_tlv;

  duration_tlv.units     = gstk_duration.units;
  duration_tlv.interval  = gstk_duration.length;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_DURATION,
                                     sizeof(duration_tlv),
                                     (void *)&duration_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_DURATION!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_duration_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_RESPONSE_FORMAT_TLV

  DESCRIPTION
    to send the response tlv

  PARAMETERS
    gstk_response_format : Response format structure
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_response_format_tlv
(
  gstk_input_format_enum_type    gstk_response_format,
  dsm_item_type               ** ind
)
{
  uint8            response_format_tlv;

  switch(gstk_response_format)
  {
    case GSTK_SMS_DEF_ALPHABET:
      response_format_tlv = CATI_RESPONSE_FORMAT_SMS_DEF_ALPHABET;
      break;

    case GSTK_YES_NO:
      response_format_tlv = CATI_RESPONSE_FORMAT_YES_NO;
      break;

    case GSTK_NUMERICAL_ONLY:
      response_format_tlv = CATI_RESPONSE_FORMAT_NUMBERICAL_ONLY;
      break;

    case GSTK_UCS2_ALPHABET:
      response_format_tlv = CATI_RESPONSE_FORMAT_UCS2_ALPHABET;
      break;

    case GSTK_IMMEDIATE_DIGIT_RSP:
      response_format_tlv = CATI_RESPONSE_FORMAT_IMMEDIATE_DIGIT_RSP;
      break;

    case GSTK_YES_NO_AND_IMM_DGT_RSP:
      response_format_tlv = CATI_RESPONSE_FORMAT_YES_NO_AND_IMM_DIGT_RSP;
      break;

    default:
      return QMI_ERR_INTERNAL;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_RESPONSE_FORMAT,
                                     sizeof(response_format_tlv),
                                     (void *)&response_format_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_RESPONSE_FORMAT!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_response_format_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_HELP_AVAILABLE_TLV

  DESCRIPTION
    to send the help available tlv

  PARAMETERS
    gstk_help_available : Help available
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_help_available_tlv
(
  boolean          gstk_help_available,
  dsm_item_type ** ind
)
{
  uint8     help_available_tlv;

  if (gstk_help_available)
  {
    help_available_tlv = 1;
  }
  else
  {
    help_available_tlv = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_HELP_AVAILABLE,
                                     sizeof(uint8),
                                     (void *)&help_available_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_HELP_AVAILABLE!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_help_available_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_RESPONSE_PACKING_FORMAT_TLV

  DESCRIPTION
    to send the response packing format tlv

  PARAMETERS
    gstk_resp_packing_format : Response packing format
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_response_packing_format_tlv
(
  gstk_input_pack_format_enum_type    gstk_response_packing_format,
  dsm_item_type                    ** ind
)
{
  uint8   response_packing_format_tlv;

  switch(gstk_response_packing_format)
  {
    case GSTK_UNPACK_FORMAT:
      response_packing_format_tlv = 0;
      break;
    case GSTK_PACK_FORMAT:
      response_packing_format_tlv = 1;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_RESPONSE_PACKING_FORMAT,
                                     sizeof(uint8),
                                     (void *)&response_packing_format_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_RESPONSE_PACKING_FORMAT!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_response_packing_format_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_RESPONSE_LENGTH_TLV

  DESCRIPTION
    to send the response length tlv

  PARAMETERS
    gstk_max_user_input : Maximum user input
    gstk_min_user_input : Minimum user input
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_response_length_tlv
(
  uint8            gstk_max_user_input,
  uint8            gstk_min_user_input,
  dsm_item_type ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 maximum_user_input;
    uint8 minimum_user_input;
  } response_length_tlv;

  response_length_tlv.maximum_user_input = gstk_max_user_input;
  response_length_tlv.minimum_user_input = gstk_min_user_input;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_RESPONSE_LENGTH,
                                     sizeof(response_length_tlv),
                                     (void *)&response_length_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_RESPONSE_LENGTH!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_response_length_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_SHOW_USER_INPUT_TLV

  DESCRIPTION
    to send the show user input tlv

  PARAMETERS
    gstk_show_user_input : Show user input
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_show_user_input_tlv
(
  boolean          gstk_show_user_input,
  dsm_item_type ** ind
)
{
  uint8           show_user_input_tlv;

  if (gstk_show_user_input)
  {
    show_user_input_tlv = 1;
  }
  else
  {
    show_user_input_tlv = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_SHOW_USER_INPUT,
                                     sizeof(uint8),
                                     (void *)&show_user_input_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_SHOW_USER_INPUT!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_show_user_input_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_LAUNCH_MODE_TLV

  DESCRIPTION
    to send the launch mode tlv

  PARAMETERS
    gstk_launch_mode: Launch mode
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_launch_mode_tlv
(
  gstk_launch_mode_enum_type    gstk_launch_mode,
  dsm_item_type              ** ind
)
{
  uint8   launch_mode_tlv;

  switch(gstk_launch_mode)
  {
    case GSTK_LAUNCH_IF_NOT_ALREADY_LAUNCH:
      launch_mode_tlv = 0;
      break;
    case GSTK_USE_EXISTING_BROWSER:
      launch_mode_tlv = 1;
      break;
    case GSTK_CLOSE_EXISTING_BROWSER:
      launch_mode_tlv = 2;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_LAUNCH_MODE,
                                     sizeof(uint8),
                                     (void *)&launch_mode_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_LAUNCH_MODE!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_launch_mode_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_URL_TLV

  DESCRIPTION
    to send the url tlv

  PARAMETERS
    gstk_url: URL
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_url_tlv
(
  const gstk_url_type  * gstk_url,
  dsm_item_type       ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 length;
    uint8 url[CATI_TLV_URL_MAX];
  } url_tlv;

  ASSERT(gstk_url != NULL);

  url_tlv.length = gstk_url->length;

  if (gstk_url->length > 0)
  {
    (void)memscpy((void*)url_tlv.url,
                  sizeof(url_tlv.url),
                  gstk_url->url,
                  gstk_url->length);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_URL,
                                     sizeof(uint8) + gstk_url->length,
                                     (void *)&url_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_URL!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_url_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_BROWSER_ID_TLV

  DESCRIPTION
    to send the browser id tlv

  PARAMETERS
    gstk_browser_id: Browser id
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_browser_id_tlv
(
  uint8            gstk_browser_id,
  dsm_item_type ** ind)
{
  uint8              browser_id_tlv;

  browser_id_tlv = gstk_browser_id;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_BROWSER_ID,
                                     sizeof(uint8),
                                     (void *)&browser_id_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_BROWSER_ID!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_browser_id_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_BEARER_TYPE_TLV

  DESCRIPTION
    to send the bearer type tlv

  PARAMETERS
    gstk_bearer: GSTK Bearer
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_bearer_type_tlv
(
  const gstk_bearer_type  * gstk_bearer,
  dsm_item_type          ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint16 length;
    uint8  bearer_list[CATI_TLV_BEARER_LIST_MAX];
  } bearer_type_tlv;

  ASSERT(gstk_bearer != NULL);

  bearer_type_tlv.length = (uint16)gstk_bearer->length;

  if (gstk_bearer->length > 0)
  {
    uint8 i = 0;

    if (gstk_bearer->bearer_list == NULL)
    {
      return QMI_ERR_INTERNAL;
    }
    for (i = 0; i < gstk_bearer->length; i++)
    {
      switch(gstk_bearer->bearer_list[i])
      {
        case GSTK_SMS:
          bearer_type_tlv.bearer_list[i] = 0;
          break;
        case GSTK_CSD:
          bearer_type_tlv.bearer_list[i] = 1;
          break;
        case GSTK_USSD_BEARER_CODE:
          bearer_type_tlv.bearer_list[i] = 2;
          break;
        case GSTK_GPRS:
          bearer_type_tlv.bearer_list[i] = 3;
          break;
        case GSTK_BEARER_DEFAULT:
          bearer_type_tlv.bearer_list[i] = 4;
          break;
        default:
          return QMI_ERR_INTERNAL;
      }
    }
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_BEARER_ID,
                                     sizeof(uint16) + gstk_bearer->length,
                                     (void *)&bearer_type_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_BEARER_ID!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_bearer_type_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_PROVISIONING_FILES_TLV

  DESCRIPTION
    to send the number of provisioning files tlv

  PARAMETERS
    gstk_num_prov_files: Number of provisioning files
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_provisioning_files_tlv
(
  uint32                       gstk_num_prov_files,
  const gstk_file_list_type  * gstk_prov_files,
  dsm_item_type             ** ind
)
{
  uint32             i          = 0;
  uint32             total_size = 0;
  uint8            * tlv_data   = NULL;
  uint8            * current    = NULL;
  qmi_error_e_type   ret_value  = QMI_ERR_NONE;

  /* Check parameters */
  if (gstk_num_prov_files > 0 && gstk_prov_files == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  /* Calculate required memory */
  total_size = sizeof(uint32);
  for (i = 0; i < gstk_num_prov_files; i++)
  {
    total_size += (sizeof(uint8) + gstk_prov_files[i].length);
  }

  /* Allocate memory */
  tlv_data = (uint8*)uimqmi_malloc(total_size);
  if (tlv_data == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }
  current = tlv_data;

  /* Copy number of files */
  ASSERT(current + sizeof(uint32) <= tlv_data + total_size);
  (void)memscpy(current, total_size, &gstk_num_prov_files, sizeof(uint32));
  current += sizeof(uint32);

  /* Copy each file */
  for (i = 0; i < gstk_num_prov_files; i++)
  {
    ASSERT(current + sizeof(uint8) <= tlv_data + total_size);
    (void)memscpy(current, total_size - (current - tlv_data), &gstk_prov_files[i].length, sizeof(uint8));
    current += sizeof(uint8);

    if (gstk_prov_files[i].length > 0)
    {
      ASSERT(current + gstk_prov_files[i].length <= tlv_data + total_size);
      (void)memscpy(current, total_size - (current - tlv_data), &gstk_prov_files[i].file_paths, gstk_prov_files[i].length);
      current += gstk_prov_files[i].length;
    }
  }

  if (FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_NUM_OF_PROVISIONING_FILES,
                                     (uint16)(current - tlv_data),
                                     (void *)tlv_data))
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_NUM_OF_PROVISIONING_FILES!");
    ret_value = QMI_ERR_NO_MEMORY;
  }

  /* Free memory */
  uimqmi_free(tlv_data);

  return ret_value;
} /* qmi_cati_provisioning_files_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_TONE_TLV

  DESCRIPTION
    to send the tone tlv

  PARAMETERS
    gstk_tone : Tone
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_tone_tlv
(
  gstk_tone_enum_type    gstk_tone,
  dsm_item_type       ** ind
)
{
  uint8      tone_tlv;

  switch(gstk_tone)
  {
    case GSTK_DIAL_TONE:
      tone_tlv = 1;
      break;
    case GSTK_CALLED_SUBSCRIBER_BUSY:
      tone_tlv = 2;
      break;
    case GSTK_CONGESTION:
      tone_tlv = 3;
      break;
    case GSTK_RADIO_PATH_ACK:
      tone_tlv = 4;
      break;
    case GSTK_RADIO_PATH_NOT_AVAIL_CALL_DROP:
      tone_tlv = 5;
      break;
    case GSTK_ERROR_TONE:
      tone_tlv = 6;
      break;
    case GSTK_CALL_WAITING_TONE:
      tone_tlv = 7;
      break;
    case GSTK_RINGING_TONE:
      tone_tlv = 8;
      break;
    case GSTK_GENERAL_BEEP:
      tone_tlv = 9;
      break;
    case GSTK_POSITIVE_ACK_TONE:
      tone_tlv = 10;
      break;
    case GSTK_NEGATIVE_ACK_TONE:
      tone_tlv = 11;
      break;
    case GSTK_RINGING_TONE_SELECTED_BY_USER:
      tone_tlv = 12;
      break;
    case GSTK_SMS_ALERT_TONE_SELECTED_BY_USER:
      tone_tlv = 13;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_TONE,
                                     sizeof(uint8),
                                     (void *)&tone_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_TONE!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_tone_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_PRESENTATION_TLV

  DESCRIPTION
    to send the presentation tlv

  PARAMETERS
    gstk_presentation: Presentation of the menu
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_presentation_tlv
(
  gstk_menu_present_enum_type    gstk_presentation,
  dsm_item_type               ** ind
)
{
  uint8           presentation_tlv;

  switch(gstk_presentation)
  {
    case GSTK_PRESENTATION_NOT_SPECIFIED:
      presentation_tlv = 0;
      break;
    case GSTK_DATA_VALUE_PRESENTATION:
      presentation_tlv = 1;
      break;
    case GSTK_NAVIGATION_PRESENTATION:
      presentation_tlv = 2;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_PRESENTATION,
                                     sizeof(uint8),
                                     (void *)&presentation_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_PRESENTATION!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_presentation_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_SOFT_KEY_SELECTION_TLV

  DESCRIPTION
    to send the soft key selection tlv

  PARAMETERS
    gstk_soft_key_selection: Is soft key selected
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_soft_key_selection_tlv
(
  boolean          gstk_soft_key_selection,
  dsm_item_type ** ind
)
{
  uint8       soft_key_selection_tlv;

  if (gstk_soft_key_selection)
  {
    soft_key_selection_tlv = 1;
  }
  else
  {
    soft_key_selection_tlv = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_SOFT_KEY_SELECTION,
                                     sizeof(uint8),
                                     (void *)&soft_key_selection_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_SOFT_KEY_SELECTION!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_soft_key_selection_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_ITEMS_TLV

  DESCRIPTION
    to send the item tlv

  PARAMETERS
    gstk_item : Send item
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_items_tlv
(
  uint8                          num_of_items,
  const gstk_item_object_type  * item_list,
  dsm_item_type               ** ind,
  boolean                        with_dcs
)
{
  uint32             i           = 0;
  uint32             total_size  = 0;
  uint8            * tlv_data    = NULL;
  uint8            * current     = NULL;
  qmi_error_e_type   ret_value   = QMI_ERR_NONE;
  uint8              item_length = 0;
  uint8              dcs         = 0;

  /* Check parameters */
  if (num_of_items > 0 && item_list == NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  if (num_of_items > GSTK_MAX_MENU_ITEMS && item_list != NULL)
  {
    return QMI_ERR_INTERNAL;
  }

  /* Calculate required memory */
  total_size = sizeof(uint8);
  for (i = 0; i < num_of_items; i++)
  {
    if(with_dcs)
    {
      total_size += sizeof(uint8);
    }
    total_size += (sizeof(uint8) + sizeof(uint8) + item_list[i].length);
  }

  /* Allocate memory */
  tlv_data = (uint8*)uimqmi_malloc(total_size);
  if (tlv_data == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }
  current = tlv_data;

  /* Copy number of items */
  ASSERT(current + sizeof(uint8) <= tlv_data + total_size);
  (void)memscpy(current, total_size, &num_of_items, sizeof(uint8));
  current += sizeof(uint8);

  /* Copy each item */
  for (i = 0; i < num_of_items; i++)
  {
    /* Item id */
    ASSERT(current + sizeof(uint8) <= tlv_data + total_size);
    (void)memscpy(current, total_size - (current - tlv_data), &item_list[i].item_id, sizeof(uint8));
    current += sizeof(uint8);

    item_length = item_list[i].length;
    /* GSTK adds an extra bite at the end with NULL termination for legacy
       clients. We need to remove it, if it's present */
    if (item_list[i].length > 0 &&
        item_list[i].item_text != NULL &&
        item_list[i].item_text[item_list[i].length - 1] == 0x0)
    {
      item_length--;
    }
    else if(item_list[i].item_text == NULL)
    {
      item_length = 0;
    }

    /* find dcs value of items */
    if(with_dcs)
    {
      /* set default to 8 bit SMS default DCS */
      dcs = CATI_DCS_8_BIT;

      /* update dcs for unicode if required */
      if ((item_length > 0) && 
          (item_list[i].item_text != NULL) &&
          ((item_list[i].item_text[0] == 0x80) ||
          (item_list[i].item_text[0] == 0x81) ||
          (item_list[i].item_text[0] == 0x82)))
      {
        /* unicode */
        dcs = CATI_DCS_UCS2_8_BIT;
      }

      /* dcs for item */
      ASSERT(current + sizeof(uint8) <= tlv_data + total_size);
      (void)memscpy(current, total_size - (current - tlv_data), &dcs, sizeof(uint8));
      current += sizeof(uint8);
    }

    /* Text length */
    ASSERT(current + sizeof(uint8) <= tlv_data + total_size);
    (void)memscpy(current, total_size - (current - tlv_data), &item_length, sizeof(uint8));
    current += sizeof(uint8);

    /* Text item */
    if (item_length > 0 && item_list[i].item_text != NULL)
    {
      ASSERT(current + item_length <= tlv_data + total_size);
      (void)memscpy(current, total_size - (current - tlv_data), item_list[i].item_text, item_length);
      current += item_length;
    }
  }

  if (FALSE == qmi_svc_put_param_tlv(ind,
                                     with_dcs? CATI_PRM_TYPE_MENU_ITEMS_WITH_DCS :
                                               CATI_PRM_TYPE_MENU_ITEMS,
                                     (uint16)(current - tlv_data),
                                     (void *)tlv_data))
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_NUM_OF_PROVISIONING_FILES!");
    ret_value = QMI_ERR_NO_MEMORY;
  }

  /* Free memory */
  uimqmi_free(tlv_data);

  return ret_value;
} /* qmi_cati_items_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_DEFAULT_ITEM_TLV

  DESCRIPTION
    to send the default item tlv

  PARAMETERS
    gstk_default_item : Send default item
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_default_item_tlv
(
  uint8            gstk_default_item,
  dsm_item_type ** ind
)
{
  uint8           default_item_tlv;

  default_item_tlv = gstk_default_item;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_DEFAULT_ITEM,
                                     sizeof(uint8),
                                     (void *)&default_item_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_DEFAULT_ITEM!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_default_item_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_NEXT_ACTION_INDICATOR_TLV

  DESCRIPTION
    to send the next action indicator tlv

  PARAMETERS
    gstk_next_action_indicator : Next action indicator
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_next_action_indicator_tlv
(
  const gstk_next_item_ind_type  * gstk_next_action_indicator,
  dsm_item_type                 ** ind
)
{
  uint8 i = 0;
  uint8 j = 0;

  PACKED struct PACKED_POST
  {
    uint8  num_of_items;
    uint8  next_action_list[CATI_TLV_NEXT_ACTION_MAX];
  } next_action_indicator_tlv;

  ASSERT(gstk_next_action_indicator != NULL);

  next_action_indicator_tlv.num_of_items = gstk_next_action_indicator->num_items;

  for (i = 0; i < gstk_next_action_indicator->num_items; i++)
  {
    switch(gstk_next_action_indicator->next_action_list[i])
    {
      case GSTK_CMD_STK_SET_UP_CALL:
        next_action_indicator_tlv.next_action_list[j++] = 0x0;
        break;
      case GSTK_CMD_STK_SEND_SS:
        next_action_indicator_tlv.next_action_list[j++] = 0x1;
        break;
      case GSTK_CMD_STK_USSD:
        next_action_indicator_tlv.next_action_list[j++] = 0x2;
        break;
      case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
        next_action_indicator_tlv.next_action_list[j++] = 0x3;
        break;
      case GSTK_CMD_STK_LAUNCH_BROWSER:
        next_action_indicator_tlv.next_action_list[j++] = 0x4;
        break;
      case GSTK_CMD_STK_PLAY_TONE:
        next_action_indicator_tlv.next_action_list[j++] = 0x5;
        break;
      case GSTK_CMD_STK_DISPLAY_TEXT:
        next_action_indicator_tlv.next_action_list[j++] = 0x6;
        break;
      case GSTK_CMD_STK_GET_INKEY:
        next_action_indicator_tlv.next_action_list[j++] = 0x7;
        break;
      case GSTK_CMD_STK_GET_INPUT:
        next_action_indicator_tlv.next_action_list[j++] = 0x8;
        break;
      case GSTK_CMD_STK_SELECT_ITEM:
        next_action_indicator_tlv.next_action_list[j++] = 0x9;
        break;
      case GSTK_CMD_STK_SET_UP_MENU:
        next_action_indicator_tlv.next_action_list[j++] = 0xA;
        break;
      case GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
        next_action_indicator_tlv.next_action_list[j++] = 0xB;
        break;
      case GSTK_CMD_STK_END_OF_PROACTIVE_SES:
        next_action_indicator_tlv.next_action_list[j++] = 0xC;
        break;
      case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
        next_action_indicator_tlv.next_action_list[j++] = 0xD;
        break;
      default:
        /* Skipping unknown item: decrase total count */
        next_action_indicator_tlv.num_of_items--;
        break;
    }
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_NEXT_ACTION_INDICATOR,
                                     sizeof(uint8) + (next_action_indicator_tlv.num_of_items * sizeof(uint8)),
                                     (void *)&next_action_indicator_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_NEXT_ACTION_INDICATOR!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_next_action_indicator_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_ICON_ID_LIST_TLV

  DESCRIPTION
    to send the icon id list tlv

  PARAMETERS
    gstk_icon_id_list : Icon id list
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_icon_id_list_tlv
(
  const gstk_icon_id_list_type  * gstk_icon_id_list,
  dsm_item_type                ** ind
)
{
  uint32             i          = 0;
  uint32             total_size = 0;
  uint8            * tlv_data   = NULL;
  uint8            * current    = NULL;
  qmi_error_e_type   ret_value  = QMI_ERR_NONE;

  PACKED struct PACKED_POST
  {
    uint8  display_icon_only;
    uint8  num_of_items;
  } icon_id_list_tlv;

  PACKED struct PACKED_POST
  {
    uint8       qualifier;
    uint8       height;
    uint8       width;
    uint8       ics;
    uint8       rec_num;
    uint16      data_size;
    uint8       data[CATI_TLV_ICON_SIZE_MAX];
  } icon_tlv;

  ASSERT(gstk_icon_id_list != NULL);

  /* Calculate required memory: this might be more than required, as each
     icon could be smaller than max size */
  total_size = sizeof(icon_id_list_tlv) +
               (gstk_icon_id_list->num_items * sizeof(icon_tlv));

  /* Allocate memory */
  tlv_data = (uint8*)uimqmi_malloc(total_size);
  if (tlv_data == NULL)
  {
    return QMI_ERR_NO_MEMORY;
  }
  current = tlv_data;

  /* Set initial part of TLV */
  if (gstk_icon_id_list->display_icon_only)
  {
    icon_id_list_tlv.display_icon_only = 1;
  }
  else
  {
    icon_id_list_tlv.display_icon_only = 0;
  }
  icon_id_list_tlv.num_of_items = gstk_icon_id_list->num_items;

  /* Copy number of icons */
  ASSERT(current + sizeof(icon_id_list_tlv) <= tlv_data + total_size);
  (void)memscpy(current, total_size, (void *)&icon_id_list_tlv, sizeof(icon_id_list_tlv));
  current += sizeof(icon_id_list_tlv);

  /* loop thru each icon */
  for (i = 0; i < gstk_icon_id_list->num_items; i++)
  {
    uint32 icon_size = 0;

    if (gstk_icon_id_list->icon_list[i] == NULL)
    {
      uimqmi_free(tlv_data);
      return QMI_ERR_INTERNAL;
    }

    memset((void *)&icon_tlv, 0, sizeof(icon_tlv));

    /* Basic icon properties */
    icon_tlv.qualifier = gstk_icon_id_list->icon_list[i]->qualifier;
    icon_tlv.height    = gstk_icon_id_list->icon_list[i]->height;
    icon_tlv.width     = gstk_icon_id_list->icon_list[i]->width;
    icon_tlv.data_size = (uint16)gstk_icon_id_list->icon_list[i]->size;
    icon_tlv.rec_num   = gstk_icon_id_list->icon_list[i]->rec_num;

    /* Color scheme */
    switch(gstk_icon_id_list->icon_list[i]->image_coding_scheme)
    {
      case GSTK_UNKNOWN_IMAGE_CODE_SCHEME:
        icon_tlv.ics = 0;
        break;
      case GSTK_BASIC_IMAGE_CODE_SCHEME:
        icon_tlv.ics = 1;
        break;
      case GSTK_COLOUR_IMAGE_CODE_SCHEME:
        icon_tlv.ics = 2;
        break;
      default:
        return QMI_ERR_INTERNAL;
    }

    /* Data */
    if (gstk_icon_id_list->icon_list[i]->size > CATI_TLV_ICON_SIZE_MAX)
    {
      uimqmi_free(tlv_data);
      return QMI_ERR_INTERNAL;
    }
    if (gstk_icon_id_list->icon_list[i]->size > 0 &&
        gstk_icon_id_list->icon_list[i]->data != NULL)
    {
      (void)memscpy((void*)icon_tlv.data,
              sizeof(icon_tlv.data),
              gstk_icon_id_list->icon_list[i]->data,
              gstk_icon_id_list->icon_list[i]->size);
    }

    /* Calculate icon size */
    icon_size = (5 * sizeof(uint8)) + sizeof(uint16) +
                gstk_icon_id_list->icon_list[i]->size;

    /* Copy icon */
    ASSERT(current + icon_size <= tlv_data + total_size);
    (void)memscpy(current, total_size - (current - tlv_data), (void *)&icon_tlv, icon_size);
    current += icon_size;
  }

  if (FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_ICON_ID_LIST,
                                     (uint16)(current - tlv_data),
                                     (void *)tlv_data))
  {
    UIM_MSG_MED_0("Unable to send icon id list!");
    ret_value = QMI_ERR_NO_MEMORY;
  }

  /* Free memory */
  uimqmi_free(tlv_data);

  return ret_value;
} /* qmi_cati_icon_id_list_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_PACKING_REQUIRED_TLV

  DESCRIPTION
    to send the packing required tlv

  PARAMETERS
    gstk_packing_required: Packing required
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_packing_required_tlv
(
  boolean          gstk_packing_required,
  dsm_item_type ** ind
)
{
  uint8          packing_required_tlv;

  if (gstk_packing_required)
  {
    packing_required_tlv = 1;
  }
  else
  {
    packing_required_tlv = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_PACKING_REQUIRED,
                                     sizeof(uint8),
                                     (void *)&packing_required_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_PACKING_REQUIRED!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_packing_required_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_SMS_TPDU_TLV

  DESCRIPTION
    to send the sms tpdu tlv

  PARAMETERS
    gstk_sms_tpdu: Sms tpdu
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_sms_tpdu_tlv
(
  const gstk_sms_tpdu_type  * gstk_sms,
  dsm_item_type            ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8  length;
    uint8  sms_tpdu[CATI_TLV_SMS_TPDU_MAX];
  } sms_tpdu_tlv;

  ASSERT(gstk_sms != NULL);

  sms_tpdu_tlv.length = gstk_sms->length;

  if (gstk_sms->length > 0)
  {
    (void)memscpy((void*)sms_tpdu_tlv.sms_tpdu,
                  sizeof(sms_tpdu_tlv.sms_tpdu),
                  gstk_sms->tpdu,
                  gstk_sms->length);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_SMS_TPDU,
                                     sizeof(uint8) + gstk_sms->length,
                                     (void *)&sms_tpdu_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_SMS_TPDU!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_sms_tpdu_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_IS_CDMA_SMS_TLV

  DESCRIPTION
    to send the is cdma sms tlv

  PARAMETERS
    gstk_is_cdma_sms: Indicates if it is a CDMA Sms
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_is_cdma_sms_tlv
(
  boolean          gstk_is_cdma_sms,
  dsm_item_type ** ind
)
{
  uint8            is_cdma_sms_tlv;

  if (gstk_is_cdma_sms)
  {
    is_cdma_sms_tlv = 1;
  }
  else
  {
    is_cdma_sms_tlv = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_IS_CDMA_SMS,
                                     sizeof(uint8),
                                     (void *)&is_cdma_sms_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_IS_CDMA_SMS!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_is_cdma_sms_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_ADDRESS_TLV

  DESCRIPTION
    to send the address tlv

  PARAMETERS
    gstk_address: address for calls
    ind : tlv
    prm_type: Tag value

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_address_tlv
(
  const gstk_address_type  * gstk_address,
  dsm_item_type           ** ind,
  byte                       prm_type
)
{
  PACKED struct PACKED_POST
  {
    uint8 ton;
    uint8 npi;
    uint8 length;
    uint8 address[CATI_TLV_ADDRESS_MAX];
  } address_tlv;

  ASSERT(gstk_address != NULL);

  UIM_MSG_HIGH_3("TON : 0x%x, NPI : 0x%x, length : 0x%x in CATI_PRM_TYPE_ADDRESS",
                 gstk_address->TON, gstk_address->NPI, gstk_address->length);

  switch(gstk_address->TON)
  {
    case GSTK_UNKNOWN_TON:
    case GSTK_RFU_TON:
      address_tlv.ton = 0;
      break;
    case GSTK_INTERNATIONAL_NUM:
      address_tlv.ton = 1;
      break;
    case GSTK_NATIONAL_NUM:
      address_tlv.ton = 2;
      break;
    case GSTK_NETWORK_SPECIFIC_NUM:
      address_tlv.ton = 3;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }

  switch(gstk_address->NPI)
  {
    case GSTK_UNKNOWN_NPI:
    case GSTK_RFU_NPI:
      address_tlv.npi = 0;
      break;
    case GSTK_ISDN_TELEPHONY_NPI:
      address_tlv.npi = 1;
      break;
    case GSTK_DATA_NPI:
      address_tlv.npi = 2;
      break;
    case GSTK_TELEX_NPI:
      address_tlv.npi = 3;
      break;
    case GSTK_PRIVATE_NPI:
      address_tlv.npi = 4;
      break;
    case GSTK_EXT_RESERVED_NPI:
      address_tlv.npi = 0xF;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }

  address_tlv.length = gstk_address->length;

  if (gstk_address->length > CATI_TLV_ADDRESS_MAX)
  {
    return QMI_ERR_INTERNAL;
  }
  if (gstk_address->length > 0)
  {
    (void)memscpy((void*)address_tlv.address,
                  sizeof(address_tlv.address),
                  gstk_address->address,
                  gstk_address->length);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     prm_type,
                                     sizeof(uint8) + sizeof(uint8) + sizeof(uint8) + gstk_address->length,
                                     (void *)&address_tlv) )
  {
    UIM_MSG_MED_1("Unable to send address for for TLV with tag 0x%x",
                  prm_type);
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_address_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_USSD_STRING_TLV

  DESCRIPTION
    to send the ussd string tlv

  PARAMETERS
    gstk_ussd: USSD String
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_ussd_string_tlv
(
  const gstk_ussd_string_type  * gstk_ussd,
  dsm_item_type               ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 orig_dcs_from_sim;
    uint8 dcs;
    uint8 length;
    uint8 text[CATI_TLV_USSD_STRING_MAX];
  } ussd_string_tlv;

  ASSERT(gstk_ussd != NULL);

  qmi_cati_convert_gstk_dcs_to_qmi(gstk_ussd->orig_dcs_from_sim,
                                   (uint8 *)&ussd_string_tlv.orig_dcs_from_sim,
                                   TRUE);

  qmi_cati_convert_gstk_dcs_to_qmi(gstk_ussd->dcs,
                                   (uint8 *)&ussd_string_tlv.dcs,
                                   TRUE);

  ussd_string_tlv.length = gstk_ussd->length;

  /* GSTK adds an extra bite at the end with NULL termination for legacy
     clients. We need to remove it, if it's present */
  if (gstk_ussd->length > 0 &&
      gstk_ussd->text != NULL &&
      gstk_ussd->text[ussd_string_tlv.length - 1] == 0x0)
  {
    ussd_string_tlv.length--;
  }

  /* No need to check max size... length is uint8 and
     CATI_TLV_USSD_STRING_MAX is 255 */
  if (ussd_string_tlv.length > 0 &&
      gstk_ussd->text != NULL)
  {
    (void)memscpy((void*)ussd_string_tlv.text,
                  sizeof(ussd_string_tlv.text),
                  gstk_ussd->text,
                  ussd_string_tlv.length);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_USSD_STRING,
                                     (ussd_string_tlv.length + 3),
                                     (void *)&ussd_string_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_USSD_STRING!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_ussd_string_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_CALL_SETUP_REQ_TLV

  DESCRIPTION
    to send the call setup requirement tlv

  PARAMETERS
    gstk_call_setup_req: Call setup requirement
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_call_setup_req_tlv
(
  gstk_call_setup_req_enum_type    gstk_call_setup_req,
  dsm_item_type                 ** ind
)
{
  uint8    call_setup_req_tlv;

  switch(gstk_call_setup_req)
  {
    case GSTK_NO_OTHER_CALLS:
      call_setup_req_tlv = 0;
      break;
    case GSTK_HOLD_ACTIVE_CALLS:
      call_setup_req_tlv = 1;
      break;
    case GSTK_DISCONNECT_ACTIVE_CALLS:
      call_setup_req_tlv = 2;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_CALL_SETUP_REQUIREMENT,
                                     sizeof(uint8),
                                     (void *)&call_setup_req_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_CALL_SETUP_REQUIREMENT!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_call_setup_req_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_REDIAL_TLV

  DESCRIPTION
    to send the redial tlv

  PARAMETERS
    gstk_redial: Redial
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_redial_tlv
(
  const gstk_redial_type  * gstk_redial,
  dsm_item_type          ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 redial_necessary;
    uint8 units;
    uint8 interval;
  } redial_tlv;

  ASSERT(gstk_redial != NULL);

  if (gstk_redial->necessary)
  {
    redial_tlv.redial_necessary = 1;
  }
  else
  {
    redial_tlv.redial_necessary = 0;
  }
  redial_tlv.units    = gstk_redial->timeout.units;
  redial_tlv.interval = gstk_redial->timeout.length;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_REDIAL,
                                     sizeof(redial_tlv),
                                     (void *)&redial_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_REDIAL!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_redial_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_SUBADDRESS_TLV

  DESCRIPTION
    to send the subaddress tlv

  PARAMETERS
    gstk_subaddress: Subaddress
    ind : tlv
    prm_type: Tag value

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_subaddress_tlv
(
  const gstk_subaddress_type  * gstk_subaddress,
  dsm_item_type              ** ind,
  byte                          prm_type
)
{
  PACKED struct PACKED_POST
  {
    uint8  length;
    uint8 subaddress[CATI_TLV_SUBADDRESS_MAX];
  } subaddress_tlv;

  ASSERT(gstk_subaddress != NULL);

  subaddress_tlv.length = gstk_subaddress->length;

  if (gstk_subaddress->length > CATI_TLV_SUBADDRESS_MAX)
  {
    return QMI_ERR_INTERNAL;
  }
  if (gstk_subaddress->length > 0)
  {
    (void)memscpy((void*)subaddress_tlv.subaddress,
                  sizeof(subaddress_tlv.subaddress),
                  gstk_subaddress->subaddress,
                  gstk_subaddress->length);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     prm_type,
                                     sizeof(uint8) + gstk_subaddress->length,
                                     (void *)&subaddress_tlv) )
  {
    UIM_MSG_MED_1("Unable to send subaddress for TLV with tag 0x%x", prm_type);
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_subaddress_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_CCP_TLV

  DESCRIPTION
    to send the capability config data tlv

  PARAMETERS
    gstk_ccp: Capability config data
    ind : tlv
    prm_type: Tag value

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_ccp_tlv
(
  const gstk_cap_config_type  * gstk_ccp,
  dsm_item_type              ** ind,
  byte                          prm_type
)
{
  PACKED struct PACKED_POST
  {
    uint8 length;
    uint8 capability_config_data[CATI_TLV_CCP_MAX];
  } ccp_tlv;

  ASSERT(gstk_ccp != NULL);

  ccp_tlv.length = gstk_ccp->length;

  if (gstk_ccp->length > CATI_TLV_CCP_MAX)
  {
    return QMI_ERR_INTERNAL;
  }
  if (gstk_ccp->length > 0)
  {
    (void)memscpy((void*)ccp_tlv.capability_config_data,
                  sizeof(ccp_tlv.capability_config_data),
                  gstk_ccp->ccp,
                  gstk_ccp->length);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     prm_type,
                                     sizeof(uint8) + gstk_ccp->length,
                                     (void *)&ccp_tlv) )
  {
    UIM_MSG_MED_1("Unable to send ccp for TLV with tag 0x%x", prm_type);
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_ccp_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_DTMF_TLV

  DESCRIPTION
    to send the dtmf tlv

  PARAMETERS
    gstk_dtmf: DTMF data
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_dtmf_tlv
(
  const gstk_dtmf_type  * gstk_dtmf,
  dsm_item_type        ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8  length;
    uint8  dtmf_data[CATI_TLV_DTMF_MAX];
  } dtmf_tlv;

  ASSERT(gstk_dtmf != NULL);

  dtmf_tlv.length = gstk_dtmf->length;

  /* GSTK adds an extra bite at the end with NULL termination for legacy
     clients. We need to remove it, if it's present */
  if (gstk_dtmf->length > 0 &&
      gstk_dtmf->dtmf != NULL &&
      gstk_dtmf->dtmf[dtmf_tlv.length - 1] == 0x0)
  {
    dtmf_tlv.length--;
  }

  /* No need to check max size... length is uint8 and
     CATI_TLV_DTMF_MAX is 255 */
  if (dtmf_tlv.length > 0 &&
      gstk_dtmf->dtmf != NULL)
  {
    (void)memscpy((void*)dtmf_tlv.dtmf_data,
                  sizeof(dtmf_tlv.dtmf_data),
                  gstk_dtmf->dtmf,
                  dtmf_tlv.length);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_DTMF,
                                     sizeof(uint8) + gstk_dtmf->length,
                                     (void *)&dtmf_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_DTMF!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_dtmf_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_SPECIFIC_LANG_NOTIFY_TLV

  DESCRIPTION
    to send the specific lang notify tlv

  PARAMETERS
    gstk_specific_lang_notify: Specific language notification
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_specific_lang_notify_tlv
(
  boolean          gstk_specific_lang_notify,
  dsm_item_type ** ind)
{
  uint8   specific_lang_notification_tlv;

  if (gstk_specific_lang_notify)
  {
    specific_lang_notification_tlv = 1;
  }
  else
  {
    specific_lang_notification_tlv = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_SPECIFIC_LANG_NOTIFY,
                                     sizeof(uint8),
                                     (void *)&specific_lang_notification_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_SPECIFIC_LANG_NOTIFY!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_specific_lang_notify_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_LANGUAGE_TLV

  DESCRIPTION
    to send the language tlv

  PARAMETERS
    gstk_language: Language notification
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_language_tlv
(
  gstk_lang_type    gstk_language,
  dsm_item_type  ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 language[CATI_TLV_LANGUAGE_MAX];
  } language_tlv;

  (void)memscpy((void*)&language_tlv.language[0],
                sizeof(language_tlv.language),
                gstk_language.lang_code,
                sizeof(gstk_language));

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_LANGUAGE,
                                     sizeof(language_tlv),
                                     (void *)&language_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_LANGUAGE!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_language_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_EVENT_NOTIFY_TLV

  DESCRIPTION
    to send the setup event list tlv

  PARAMETERS
    gstk_language: Language notification
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_event_notify_tlv
(
  boolean           require_notification,
  dsm_item_type  ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 req_notification;
  } event_notify_tlv;

  if (require_notification)
  {
    event_notify_tlv.req_notification = 1;
  }
  else
  {
    event_notify_tlv.req_notification = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_REQUIRE_NOTIFICATION,
                                     sizeof(event_notify_tlv),
                                     (void *)&event_notify_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_REQUIRE_NOTIFICATION!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_event_notify_tlv */

/*===========================================================================
  FUNCTION QMI_CATI_ALPHABET_TLV

  DESCRIPTION
    to send the alphabet tlv

  PARAMETERS
    gstk_alphabet : Decoded alphabet
    ind      : tlv
    prm_type : Type of text

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_alphabet_tlv
(
  const gstk_alphabet_type  * gstk_alphabet,
  dsm_item_type            ** ind,
  uint8                       prm_type
)
{
  uint8         i                = 0;
  uint8         text_len         = 0;

  PACKED struct PACKED_POST
  {
    uint8                   dcs;
    uint8                   length_of_string;
    uint8                   text[CAT_TLV_TEXT_STRING_MAX];
  } alphabet_tlv;

  ASSERT(gstk_alphabet != NULL);

  /* find dcs value of items */
  /* set default to 8 bit SMS default DCS */
  alphabet_tlv.dcs = CATI_DCS_8_BIT;

  if(gstk_alphabet->length > 0 &&
     gstk_alphabet->alphabet != NULL)
  {
    /* update dcs for unicode if required */
    /* alphabet ptr doesn't contain DCS byte. poke into first byte
       to check for unicode encoding*/
    if ((gstk_alphabet->alphabet[0] & 0xFF80) > 0)
    {
      /* unicode */
      alphabet_tlv.dcs = CATI_DCS_UCS2_8_BIT;
      if(gstk_alphabet->length > (CAT_TLV_TEXT_STRING_MAX / 2))
      {
        UIM_MSG_MED_0("Invalid GSTK alphabet length for UCS2");
        return QMI_ERR_NO_MEMORY;
      }
    }
    else if((gstk_alphabet->alphabet[0] & 0x007F) > 0)
    {
      alphabet_tlv.dcs = CATI_DCS_8_BIT;
      if(gstk_alphabet->length > CAT_TLV_TEXT_STRING_MAX)
      {
        UIM_MSG_MED_0("Invalid GSTK alphabet length");
        return QMI_ERR_NO_MEMORY;
      }
    }
    else
    {
      UIM_MSG_ERR_1("Invalid DCS type 0x%x", alphabet_tlv.dcs );
      return QMI_ERR_INTERNAL;
    }
  }

  alphabet_tlv.length_of_string = (uint8)(gstk_alphabet->length);

  /* GSTK adds an extra byte at the end with NULL termination for legacy
     clients. We need to remove it, if it's present */
  if (gstk_alphabet->length > 0 &&
      gstk_alphabet->alphabet != NULL &&
      gstk_alphabet->alphabet[gstk_alphabet->length - 1] == 0x0)
  {
    alphabet_tlv.length_of_string--;
  }

  if(alphabet_tlv.length_of_string > 0)
  {
    if (gstk_alphabet->alphabet != NULL)
    {
      if(alphabet_tlv.dcs == CATI_DCS_8_BIT)
      {
        for(i = 0; i < alphabet_tlv.length_of_string; i++)
        {
          alphabet_tlv.text[i] = (uint8)(gstk_alphabet->alphabet[i] & 0x00FF);
        }
        text_len = alphabet_tlv.length_of_string;
      }
      else if (alphabet_tlv.dcs == CATI_DCS_UCS2_8_BIT)
      {
        for(i = 0; i < alphabet_tlv.length_of_string; i++)
        {
          /* 2 bytes for each UCS2 text*/
          alphabet_tlv.text[i*2] = (uint8)(gstk_alphabet->alphabet[i] >> 8);
          alphabet_tlv.text[i*2 + 1] = (uint8)(gstk_alphabet->alphabet[i] & 0x00FF);
        }
        text_len = alphabet_tlv.length_of_string * 2;
      }
      else
      {
        UIM_MSG_ERR_1("Invalid DCS type 0x%x", alphabet_tlv.dcs );
        return QMI_ERR_INTERNAL;
      }
    }
    else
    {
      return QMI_ERR_INTERNAL;
    }
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     prm_type,
                                     sizeof(uint8) + sizeof(uint8) + text_len,
                                     (void *)&alphabet_tlv) )
  {
    UIM_MSG_MED_0("Unable to send alphabet!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_alphabet_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_NETWORK_ACCESS_NAME_TLV

  DESCRIPTION
    to send the network access name tlv

  PARAMETERS
    gstk_network_access_name : Decoded alphabet
    ind      : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_network_access_name_tlv
(
  const gstk_alphabet_type  * gstk_network_access_name,
  dsm_item_type            ** ind
)
{
  uint8         i                = 0;
  uint8         text_len         = 0;

  PACKED struct PACKED_POST
  {
    uint8                   length_of_string;
    uint8                   text[CAT_TLV_TEXT_STRING_MAX];
  } network_access_name_tlv;

  ASSERT(gstk_network_access_name != NULL);

  if(gstk_network_access_name->length > 0 &&
     gstk_network_access_name->alphabet != NULL)
  {
    /* Each element of gstk alphabet ptr is represented in UCSII 8 bit, length
       field indicates the number of uint16 in the alphabet buffer. note that
       first element contains the DCS byte*/
    if((gstk_network_access_name->alphabet[0] & 0x007F) > 0)
    {
      if(gstk_network_access_name->length > CAT_TLV_TEXT_STRING_MAX)
      {
        UIM_MSG_MED_0("Invalid GSTK alphabet length");
        return QMI_ERR_NO_MEMORY;
      }
    }
    else if ((gstk_network_access_name->alphabet[0] & 0xFF80) > 0)
    {
      /* from 3GPP, APN is with ascii encoding, skip sending TLV
         if DCS shows other encoding type*/
      UIM_MSG_MED_0("Invalid UCSII DCS for APN.");
      return QMI_ERR_NONE;
    }
    else
    {
      UIM_MSG_ERR_1("Invalid DCS type 0x%x",
                    gstk_network_access_name->alphabet[0]  );
      return QMI_ERR_INTERNAL;
    }
  }

  network_access_name_tlv.length_of_string = (uint8)(gstk_network_access_name->length);

  /* GSTK adds an extra byte at the end with NULL termination for legacy
     clients. We need to remove it, if it's present */
  if (gstk_network_access_name->length > 0 &&
      gstk_network_access_name->alphabet != NULL &&
      gstk_network_access_name->alphabet[gstk_network_access_name->length - 1] == 0x0)
  {
    network_access_name_tlv.length_of_string--;
  }

  if(network_access_name_tlv.length_of_string > 0)
  {
    if (gstk_network_access_name->alphabet != NULL)
    {
      for(i = 0; i < network_access_name_tlv.length_of_string; i++)
      {
        network_access_name_tlv.text[i] = (uint8)(gstk_network_access_name->alphabet[i] & 0x00FF);
      }
      text_len = network_access_name_tlv.length_of_string;
    }
    else
    {
      return QMI_ERR_INTERNAL;
    }
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_NETWORK_ACCESS_NAME,
                                     sizeof(uint8) + text_len,
                                     (void *)&network_access_name_tlv) )
  {
    UIM_MSG_MED_0("Unable to send alphabet!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_network_access_name_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_ON_DEMAND_LINK_EST_TLV

  DESCRIPTION
    to send the on demand link est tlv

  PARAMETERS
    gstk_on_demand_link_est
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_on_demand_link_est_tlv
(
  boolean          gstk_on_demand_link_est,
  dsm_item_type ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 on_demand_line_est;
  } on_demand_link_est_tlv;

  /* GSTK sets gstk_on_demand_link_est flag to FALSE for send immediately*/
  if (gstk_on_demand_link_est)
  {
    on_demand_link_est_tlv.on_demand_line_est = 0;
  }
  else
  {
    on_demand_link_est_tlv.on_demand_line_est = 1;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_ON_DEMAND_LINK_EST,
                                     sizeof(on_demand_link_est_tlv),
                                     (void *)&on_demand_link_est_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_ON_DEMAND_LINK_EST!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_on_demand_link_est_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_CDS_BEARER_TLV

  DESCRIPTION
    to send the csd bearer description tlv

  PARAMETERS
    gstk_cds_bearer
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_csd_bearer_tlv
(
  gstk_csd_bearer_param_type       gstk_cds_bearer,
  dsm_item_type                 ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8                    speed;
    uint8                    name;
    uint8                    connection_element;
  } cds_bearer_tlv;

  cds_bearer_tlv.speed = gstk_cds_bearer.speed;
  cds_bearer_tlv.name = gstk_cds_bearer.name;
  cds_bearer_tlv.connection_element = gstk_cds_bearer.connection_element;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_CDS_BEARER,
                                     sizeof(cds_bearer_tlv),
                                     (void *)&cds_bearer_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_CDS_BEARER!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_csd_bearer_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_GPRS_BEARER_TLV

  DESCRIPTION
    to send the gprs bearer description tlv

  PARAMETERS
    gstk_gprs_bearer
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_gprs_bearer_tlv
(
  gstk_gprs_bearer_param_type      gstk_gprs_bearer,
  dsm_item_type                 ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8                               precedence_cls;
    uint8                               delay_cls;
    uint8                               reliability_cls;
    uint8                               peak_throughput;
    uint8                               mean_throughput;
    uint8                               pkt_data_protocol;
  } gprs_bearer_tlv;

  gprs_bearer_tlv.precedence_cls = (uint8)gstk_gprs_bearer.precedence_cls;
  gprs_bearer_tlv.delay_cls = (uint8)gstk_gprs_bearer.delay_cls;
  gprs_bearer_tlv.reliability_cls = (uint8)gstk_gprs_bearer.reliability_cls;
  gprs_bearer_tlv.peak_throughput = (uint8)gstk_gprs_bearer.peak_throughput;
  gprs_bearer_tlv.mean_throughput = (uint8)gstk_gprs_bearer.mean_throughput;
  gprs_bearer_tlv.pkt_data_protocol = gstk_gprs_bearer.pkt_data_protocol;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_GPRS_BEARER,
                                     sizeof(gprs_bearer_tlv),
                                     (void *)&gprs_bearer_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_GPRS_BEARER!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_gprs_bearer_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_EUTRAN_EXT_BEARER_TLV

  DESCRIPTION
    to send the eutran ext bearer description tlv

  PARAMETERS
    gstk_eutran_ext_bearer
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_eutran_ext_bearer_tlv
(
  gstk_eutran_ext_param_type       gstk_eutran_ext_bearer,
  dsm_item_type                 ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8                                traffic_class;
    uint16                               max_bitrate_ul;
    uint16                               max_bitrate_dl;
    uint16                               guaranteed_bitrate_ul;
    uint16                               guaranteed_bitrate_dl;
    uint8                                delivery_order;
    uint8                                max_sdu_size;
    uint8                                sdu_error_ratio;
    uint8                                residual_bit_error_ratio;
    uint8                                delivery_of_err_sdu;
    uint8                                transfer_delay;
    uint8                                traffic_handling_pri;
    uint8                                pdp_type;
  } eutran_ext_bearer_tlv;

  eutran_ext_bearer_tlv.traffic_class = gstk_eutran_ext_bearer.traffic_class;
  eutran_ext_bearer_tlv.max_bitrate_ul = (uint16)gstk_eutran_ext_bearer.max_bitrate_ul;
  eutran_ext_bearer_tlv.max_bitrate_dl = (uint16)gstk_eutran_ext_bearer.max_bitrate_dl;
  eutran_ext_bearer_tlv.guaranteed_bitrate_ul = (uint16)gstk_eutran_ext_bearer.guaranteed_bitrate_ul;
  eutran_ext_bearer_tlv.guaranteed_bitrate_dl = (uint16)gstk_eutran_ext_bearer.guaranteed_bitrate_dl;
  eutran_ext_bearer_tlv.delivery_order = gstk_eutran_ext_bearer.delivery_order;
  eutran_ext_bearer_tlv.max_sdu_size = (uint8)gstk_eutran_ext_bearer.max_sdu_size;
  eutran_ext_bearer_tlv.sdu_error_ratio = gstk_eutran_ext_bearer.sdu_error_ratio;
  eutran_ext_bearer_tlv.residual_bit_error_ratio = gstk_eutran_ext_bearer.residual_bit_error_ratio;
  eutran_ext_bearer_tlv.delivery_of_err_sdu = gstk_eutran_ext_bearer.delivery_of_err_sdu;
  eutran_ext_bearer_tlv.transfer_delay = (uint8)gstk_eutran_ext_bearer.transfer_delay;
  eutran_ext_bearer_tlv.traffic_handling_pri = (uint8)gstk_eutran_ext_bearer.traffic_handling_pri;

  switch(gstk_eutran_ext_bearer.pdp_type)
  {
    case GSTK_PKT_DATA_IP:
      eutran_ext_bearer_tlv.pdp_type = 0x02;
      break;
    default:
      eutran_ext_bearer_tlv.pdp_type = 0xFF;
      break;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_EUTRAN_EXT_PARAMS_BEARER,
                                     sizeof(eutran_ext_bearer_tlv),
                                     (void *)&eutran_ext_bearer_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_EUTRAN_EXT_PARAMS_BEARER!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_eutran_ext_bearer_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_EUTRAN_MAPPED_UTRAN_PS_BEARER_TLV

  DESCRIPTION
    to send the eutran mapped utran ps bearer description tlv

  PARAMETERS
    gstk_eutran_mapped_utran_ps_bearer
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_eutran_mapped_utran_ps_bearer_tlv
(
  gstk_eutran_mapped_utran_ps_param_type       gstk_eutran_mapped_utran_ps_bearer,
  dsm_item_type                             ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8                                qci;
    uint8                                max_bitrate_ul;
    uint8                                max_bitrate_dl;
    uint8                                guaranteed_bitrate_ul;
    uint8                                guaranteed_bitrate_dl;
    uint8                                max_bitrate_ul_ext;
    uint8                                max_bitrate_dl_ext;
    uint8                                guaranteed_bitrate_ul_ext;
    uint8                                guaranteed_bitrate_dl_ext;
    uint8                                pdp_type;
  } eutran_mapped_utran_ps_bearer_tlv;

  eutran_mapped_utran_ps_bearer_tlv.qci =
         gstk_eutran_mapped_utran_ps_bearer.qci;
  eutran_mapped_utran_ps_bearer_tlv.max_bitrate_ul =
         gstk_eutran_mapped_utran_ps_bearer.max_bitrate_ul;
  eutran_mapped_utran_ps_bearer_tlv.max_bitrate_dl =
         gstk_eutran_mapped_utran_ps_bearer.max_bitrate_dl;
  eutran_mapped_utran_ps_bearer_tlv.guaranteed_bitrate_ul =
         gstk_eutran_mapped_utran_ps_bearer.guaranteed_bitrate_ul;
  eutran_mapped_utran_ps_bearer_tlv.guaranteed_bitrate_dl =
         gstk_eutran_mapped_utran_ps_bearer.guaranteed_bitrate_dl;
  eutran_mapped_utran_ps_bearer_tlv.max_bitrate_ul_ext =
         gstk_eutran_mapped_utran_ps_bearer.max_bitrate_ul_ext;
  eutran_mapped_utran_ps_bearer_tlv.max_bitrate_dl_ext =
         gstk_eutran_mapped_utran_ps_bearer.max_bitrate_dl_ext;
  eutran_mapped_utran_ps_bearer_tlv.guaranteed_bitrate_ul_ext =
         gstk_eutran_mapped_utran_ps_bearer.guaranteed_bitrate_ul_ext;
  eutran_mapped_utran_ps_bearer_tlv.guaranteed_bitrate_dl_ext =
         gstk_eutran_mapped_utran_ps_bearer.guaranteed_bitrate_dl_ext;
  eutran_mapped_utran_ps_bearer_tlv.pdp_type =
         gstk_eutran_mapped_utran_ps_bearer.pdp_type;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_EUTRAN_MAPPED_UTRAN_PS_BEARER,
                                     sizeof(eutran_mapped_utran_ps_bearer_tlv),
                                     (void *)&eutran_mapped_utran_ps_bearer_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_EUTRAN_EXT_PARAMS_BEARER!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_eutran_mapped_utran_ps_bearer_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_BUFFER_SIZE_TLV

  DESCRIPTION
    to send the buffer size tlv

  PARAMETERS
    gstk_buffer_size : BUFFER SIZE
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_buffer_size_tlv
(
  int32            gstk_buffer_size,
  dsm_item_type ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint16 buffer_size;
  } buffer_size_tlv;

  buffer_size_tlv.buffer_size = (uint16)gstk_buffer_size;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_BUFFER_SIZE,
                                     sizeof(buffer_size_tlv),
                                     (void *)&buffer_size_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_BUFFER_SIZE!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_buffer_size_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_DATA_ADDRESS_TLV

  DESCRIPTION
    to send the data address tlv

  PARAMETERS
    gstk_data_address
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_data_address_tlv
(
  const gstk_pkt_data_address_type    * gstk_data_address,
  dsm_item_type                      ** ind,
  uint8                                 prm_type
)
{
  PACKED struct PACKED_POST
  {
    uint8 address_type;
    uint8 length;
    uint8 address[CATI_TLV_DATA_ADDRESS_MAX];
  } data_address_tlv;

  ASSERT(gstk_data_address);

  data_address_tlv.address_type = gstk_data_address->address_type;
  data_address_tlv.length = (uint8)gstk_data_address->pkt_address.length;

  if (gstk_data_address->pkt_address.length > CATI_TLV_DATA_ADDRESS_MAX)
  {
    return QMI_ERR_NO_MEMORY;
  }

  if (gstk_data_address->pkt_address.length > 0)
  {
    (void)memscpy((void*)&data_address_tlv.address[0],
                  sizeof(data_address_tlv.address),
                  gstk_data_address->pkt_address.address,
                  gstk_data_address->pkt_address.length);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     prm_type,
                                     sizeof(uint8) + sizeof(uint8) + data_address_tlv.length,
                                     (void *)&data_address_tlv) )
  {
    UIM_MSG_MED_0("Unable to send Data Address!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_data_address_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_TRANSPORT_LEVEL_TLV

  DESCRIPTION
    to send the transport level tlv

  PARAMETERS
    gstk_data_transport_level
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_transport_level_tlv
(
  gstk_pkt_data_transport_level_type       gstk_data_transport_level,
  dsm_item_type                         ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8                   transport_protocol;
    uint16                  port_number;
  } data_transport_level_tlv;

  data_transport_level_tlv.transport_protocol =
                                  gstk_data_transport_level.transport_protocol;

  switch(gstk_data_transport_level.transport_protocol)
  {
    case GSTK_PKT_DATA_NOT_PRESENT:
      data_transport_level_tlv.transport_protocol = 0x00;
      break;
    case GSTK_PKT_DATA_UDP:
      data_transport_level_tlv.transport_protocol = 0x01;
      break;
    case GSTK_PKT_DATA_TCP:
      data_transport_level_tlv.transport_protocol = 0x02;
      break;
    default:
      return QMI_ERR_INTERNAL;
  }
  data_transport_level_tlv.port_number = (uint16) gstk_data_transport_level.port_number;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_TRANSPORT_LEVEL,
                                     sizeof(data_transport_level_tlv),
                                     (void *)&data_transport_level_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_TRANSPORT_LEVEL!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_transport_level_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_CHANNEL_ID_TLV

  DESCRIPTION
    to send the channel id tlv

  PARAMETERS
    gstk_ch_id : channel id
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_channel_id_tlv
(
  int32            gstk_ch_id,
  dsm_item_type ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 ch_id;
  } ch_id_tlv;

  ch_id_tlv.ch_id = (uint8)gstk_ch_id;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_CHANNEL_ID,
                                     sizeof(ch_id_tlv),
                                     (void *)&ch_id_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_CHANNEL_ID!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_channel_id_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_BIP_STATUS_TLV

  DESCRIPTION
    to send the channel id tlv

  PARAMETERS
    gstk_ch_id : channel id
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_bip_status_tlv
(
  uint8                         gstk_ch_id,
  qmi_cati_bip_status_e_type    bip_status,
  dsm_item_type              ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 ch_id;
    uint32 bip_status;
  } bip_status_tlv;

  bip_status_tlv.ch_id = gstk_ch_id;
  switch (bip_status)
  {
     case CATI_BIP_SESSION_IN_PROGRESS:
       bip_status_tlv.bip_status = 0;
       break;
     case CATI_BIP_SESSION_END:
       bip_status_tlv.bip_status = 1;
       break;
     default:
       UIM_MSG_ERR_1("Invalid BIP status 0x%x :", bip_status);
       return QMI_ERR_INTERNAL;
  }
  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_BIP_STATUS,
                                     sizeof(bip_status_tlv),
                                     (void *)&bip_status_tlv))
  {
    UIM_MSG_ERR_0("Unable to send CATI_PRM_TYPE_BIP_STATUS!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_bip_status_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_CHANNEL_DATA_LEN_TLV

  DESCRIPTION
    to send the channel data len tlv

  PARAMETERS
    gstk_ch_data_len : channel data len
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_channel_data_len_tlv
(
  int32            gstk_ch_data_len,
  dsm_item_type ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 ch_data_len;
  } ch_data_len_tlv;

  if(gstk_ch_data_len > CATI_TLV_BIP_DATA_MAX)
  {
    UIM_MSG_MED_0("Invalid channel data len.");
    return QMI_ERR_INTERNAL;
  }

  ch_data_len_tlv.ch_data_len = (uint8)gstk_ch_data_len;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_CHANNEL_DATA_LEN,
                                     sizeof(ch_data_len_tlv),
                                     (void *)&ch_data_len_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_CHANNEL_DATA_LEN!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_channel_data_len_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_CHANNEL_DATA_TLV

  DESCRIPTION
    to send the channel data tlv

  PARAMETERS
    gstk_channel_data : channel data
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_channel_data_tlv
(
  gstk_ch_data_type       gstk_channel_data,
  dsm_item_type        ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint16          data_len;
    uint8           data[CATI_TLV_BIP_DATA_MAX];
  } ch_data_tlv;

  ch_data_tlv.data_len = (uint16) gstk_channel_data.data_len;

  if (gstk_channel_data.data_len> CATI_TLV_BIP_DATA_MAX)
  {
    return QMI_ERR_NO_MEMORY;
  }

  if(gstk_channel_data.data_len > 0)
  {
    (void)memscpy((void*)&ch_data_tlv.data[0],
                  sizeof(ch_data_tlv.data),
                  gstk_channel_data.data,
                  gstk_channel_data.data_len);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_CHANNEL_DATA,
                                     sizeof(uint16) + ch_data_tlv.data_len,
                                     (void *)&ch_data_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_CHANNEL_DATA!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_channel_data_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_SEND_DATA_IMMEDIATELY_TLV

  DESCRIPTION
    to send the send data immediately tlv

  PARAMETERS
    gstk_send_data_immediately : send data immediately
    ind : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_send_data_immediately_tlv
(
  boolean          gstk_send_data_immediately,
  dsm_item_type ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8 send_data_immediately;
  } send_data_immediately_tlv;

  if (gstk_send_data_immediately)
  {
    send_data_immediately_tlv.send_data_immediately = 1;
  }
  else
  {
    send_data_immediately_tlv.send_data_immediately = 0;
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_SEND_DATA_IMMEDIATELY,
                                     sizeof(send_data_immediately_tlv),
                                     (void *)&send_data_immediately_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_SEND_DATA_IMMEDIATELY!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_send_data_immediately_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_SCWS_OPEN_CHANNEL_INFO_TLV

  DESCRIPTION
    to send the scws open channel info tlv

  PARAMETERS
    gstk_ch_id       : CHANNEL ID
    gstk_port        : PORT NUMBER
    gstk_buffer_size : BUFFER SIZE
    ind              : TLV

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_scws_open_channel_info_tlv
(
  uint32            gstk_ch_id,
  uint32            gstk_port,
  int32             gstk_buffer_size,
  dsm_item_type  ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint32 ch_id;
    uint16 port;
    uint16 buffer_size;
  } open_ch_info_tlv;

  open_ch_info_tlv.ch_id = gstk_ch_id;
  open_ch_info_tlv.port = (uint16)gstk_port;
  open_ch_info_tlv.buffer_size = (uint16)gstk_buffer_size;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_SCWS_OPEN_CHANNEL_INFO,
                                     sizeof(open_ch_info_tlv),
                                     (void *)&open_ch_info_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_SCWS_OPEN_CHANNEL_INFO!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_scws_open_channel_info_tlv */

/*===========================================================================
  FUNCTION QMI_CATI_SCWS_CHANNEL_STATE_TLV

  DESCRIPTION
    to send the scws channel state tlv

  PARAMETERS
    gstk_ch_id       : CHANNEL ID
    gstk_state       : CHANNEL STATE
    ind              : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_scws_channel_state_tlv
(
  uint32                        gstk_ch_id,
  gstk_scws_ch_state_enum_type  gstk_state,
  dsm_item_type              ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint32 ch_id;
    uint8  state;
  } channel_state_tlv;

  channel_state_tlv.ch_id = gstk_ch_id;

  switch(gstk_state)
  {
    case GSTK_SCWS_TCP_IN_CLOSED_STATE:
      channel_state_tlv.state = 0x00;
      break;

    case GSTK_SCWS_TCP_IN_LISTEN_STATE:
      channel_state_tlv.state = 0x01;
      break;

    case GSTK_SCWS_TCP_IN_ESTABLISHED_STATE:
      channel_state_tlv.state = 0x02;
      break;

    default:
      return QMI_ERR_INTERNAL;
  }
  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_SCWS_CHANNEL_STATE,
                                     sizeof(channel_state_tlv),
                                     (void *)&channel_state_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_SCWS_CHANNEL_STATE!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_scws_channel_state_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_SCWS_CHANNEL_DATA_TLV

  DESCRIPTION
    to send the scws channel data tlv

  PARAMETERS
    gstk_ch_id          : CHANNEL ID
    gstk_channel_data   : CHANNEL DATA
    ind                 : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_scws_channel_data_tlv
(
  uint32                        gstk_ch_id,
  gstk_ch_data_type             gstk_channel_data,
  uint16                        data_index,
  dsm_item_type              ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint32          ch_id;
    uint8           total_pkg;
    uint8           current_pkg;
    uint16          data_len;
    uint8           data[CAT_TLV_SCWS_SEND_DATA_MAX];
  } ch_data_tlv;

  ch_data_tlv.ch_id = gstk_ch_id;
  ch_data_tlv.total_pkg = (uint8) (gstk_channel_data.data_len / CAT_TLV_SCWS_SEND_DATA_MAX
                          + (((gstk_channel_data.data_len % CAT_TLV_SCWS_SEND_DATA_MAX) > 0) ? 1 : 0));
  ch_data_tlv.current_pkg = data_index / CAT_TLV_SCWS_SEND_DATA_MAX + 1;
  if(gstk_channel_data.data_len == 0)
  {
    ch_data_tlv.data_len = (uint16) gstk_channel_data.data_len;
  }
  if ((gstk_channel_data.data_len - data_index) > CAT_TLV_SCWS_SEND_DATA_MAX)
  {
    ch_data_tlv.data_len = CAT_TLV_SCWS_SEND_DATA_MAX;
  }
  else
  {
    ch_data_tlv.data_len = (uint16) gstk_channel_data.data_len - data_index;
  }

  if (ch_data_tlv.data_len> CAT_TLV_SCWS_SEND_DATA_MAX)
  {
    UIM_MSG_HIGH_0("qmi_cati_scws_channel_data_tlv: invalid data len!");
    return QMI_ERR_NO_MEMORY;
  }

  if(ch_data_tlv.data_len > 0)
  {
    (void)memscpy((void*)&ch_data_tlv.data[0],
                  sizeof(ch_data_tlv.data),
                  &gstk_channel_data.data[data_index],
                  ch_data_tlv.data_len);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_SCWS_CHANNEL_DATA,
                                     sizeof(uint32) + sizeof(uint16) + 2 * sizeof(uint8) + ch_data_tlv.data_len,
                                     (void *)&ch_data_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_SCWS_CHANNEL_DATA!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_scws_channel_data_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_ACTIVATE_TARGET_TLV

  DESCRIPTION
    to send the activate target tlv

  PARAMETERS
    gstk_ch_id       : CHANNEL ID
    gstk_state       : CHANNEL STATE
    ind              : tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_activate_target_tlv
(
  gstk_target_enum_type         gstk_activate_target,
  dsm_item_type              ** ind
)
{
  PACKED struct PACKED_POST
  {
    uint8  activate_target;
  } activate_target_tlv;

  switch(gstk_activate_target)
  {
    case GSTK_TARGET_UICC_CLF:
      activate_target_tlv.activate_target = 0x01;
      break;

    case GSTK_TARGET_NONE:
    case GSTK_TARGET_MAX:
    default:
      UIM_MSG_ERR_1("Invalid activate target value 0x%x",
                    gstk_activate_target);
      return QMI_ERR_INTERNAL;
  }
  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_ACTIVATE_TARGET,
                                     sizeof(activate_target_tlv),
                                     (void *)&activate_target_tlv) )
  {
    UIM_MSG_MED_0("Unable to send CATI_PRM_TYPE_ACTIVATE_TARGET!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_activate_target_tlv */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_DISPLAY_TEXT_COMMAND()

  DESCRIPTION
    to populate the tlvs from the display text proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_display_text_command
(
  const gstk_display_text_req_type  * display_text_cmd,
  const gstk_exp_hdr_type             gstk_hdr,
  dsm_item_type                    ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(display_text_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_DISPLAY_TEXT_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the string TLV */
  errval = qmi_cati_text_string_tlv(&display_text_cmd->text_string, resp, CATI_PRM_TYPE_TEXT_STRING);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the High Priority TLV */
  errval = qmi_cati_high_priority_tlv(display_text_cmd->high_priority, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the User Control TLV */
  errval = qmi_cati_user_control_tlv(display_text_cmd->user_control, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the immediate response required tlv */
  errval = qmi_cati_immediate_response_tlv(display_text_cmd->immediate_response_required, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Check if the icon is present */
  if (display_text_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&display_text_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the duration is present */
  if (display_text_cmd->duration.present)
  {
    errval = qmi_cati_duration_tlv(display_text_cmd->duration, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_display_text_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_GET_INKEY_COMMAND()

  DESCRIPTION
    to populate the tlvs from the get inkey proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_get_inkey_command
(
  const gstk_get_inkey_req_type  * get_inkey_cmd,
  const gstk_exp_hdr_type          gstk_hdr,
  dsm_item_type                 ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(get_inkey_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_GET_INKEY_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the string TLV */
  errval = qmi_cati_text_string_tlv(&get_inkey_cmd->text_string, resp, CATI_PRM_TYPE_TEXT_STRING);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Response format TLV */
  errval = qmi_cati_response_format_tlv(get_inkey_cmd->response_format, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Help Available TLV */
  errval = qmi_cati_help_available_tlv(get_inkey_cmd->help_available, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }


  /* Check if the icon is present */
  if (get_inkey_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&get_inkey_cmd->icon,resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the variable timeout is present */
  if (get_inkey_cmd->variable_timeout.present)
  {
    errval = qmi_cati_duration_tlv(get_inkey_cmd->variable_timeout, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
 return errval;
} /* qmi_cati_populate_get_inkey_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_GET_INPUT_COMMAND()

  DESCRIPTION
    to populate the tlvs from the get input proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_get_input_command
(
  const gstk_get_input_req_type  * get_input_cmd,
  const gstk_exp_hdr_type          gstk_hdr,
  dsm_item_type                 ** resp
)
{
  qmi_error_e_type errval=QMI_ERR_NONE;

  ASSERT(get_input_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_GET_INPUT_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the string TLV */
  errval = qmi_cati_text_string_tlv(&get_input_cmd->text_string, resp, CATI_PRM_TYPE_TEXT_STRING);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Response format TLV */
  errval = qmi_cati_response_format_tlv(get_input_cmd->response_format, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Response Packing format TLV */
  errval = qmi_cati_response_packing_format_tlv(get_input_cmd->respone_packing_format, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Response Length TLV */
  errval = qmi_cati_response_length_tlv(get_input_cmd->maximum_user_input, get_input_cmd->minimum_user_input, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Help Available TLV */
  errval = qmi_cati_help_available_tlv(get_input_cmd->help_available, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Show user input TLV */
  errval = qmi_cati_show_user_input_tlv(get_input_cmd->show_user_input, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Check if the icon is present */
  if (get_input_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&get_input_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  if (get_input_cmd->default_text.length != 0)
  {
    errval = qmi_cati_text_string_tlv(&get_input_cmd->default_text, resp, CATI_PRM_TYPE_DEFAULT_TEXT);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_get_input_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_LAUNCH_BROWSER_COMMAND()

  DESCRIPTION
    to populate the tlvs from the launch browser command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_launch_browser_command
(
  const gstk_launch_browser_req_type * launch_browser_cmd,
  const gstk_exp_hdr_type              gstk_hdr,
  dsm_item_type **                     resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(launch_browser_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_LAUNCH_BROWSER_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the launch mode TLV */
  errval = qmi_cati_launch_mode_tlv(launch_browser_cmd->launch_mode, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the url TLV */
  errval = qmi_cati_url_tlv(&launch_browser_cmd->url, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the browser id TLV */
  errval = qmi_cati_browser_id_tlv(launch_browser_cmd->browser_id, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the bearer type TLV */
  if(launch_browser_cmd->bearer.length > 0)
  {
    errval = qmi_cati_bearer_type_tlv(&launch_browser_cmd->bearer, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send the provisioning files TLV */
  if(launch_browser_cmd->num_provisioning_files > 0)
  {
    errval = qmi_cati_provisioning_files_tlv(launch_browser_cmd->num_provisioning_files,
                                             launch_browser_cmd->provisioning_file_ref,
                                             resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  if (launch_browser_cmd->gateway_proxy_text_string.length != 0)
  {
    /* Send the string TLV */
    errval = qmi_cati_text_string_tlv(&launch_browser_cmd->gateway_proxy_text_string, resp, CATI_PRM_TYPE_GATEWAY_PROXY);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  if (launch_browser_cmd->user_confirm_alpha.length != 0)
  {
    /* Send the string TLV */
    errval = qmi_cati_text_string_tlv(&launch_browser_cmd->user_confirm_alpha, resp, CATI_PRM_TYPE_USER_CNF_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (launch_browser_cmd->user_confirm_icon.present)
  {
    errval = qmi_cati_icon_tlv(&launch_browser_cmd->user_confirm_icon, resp, CATI_PRM_TYPE_USER_CONF_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_launch_browser_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_PLAY_TONE_COMMAND()

  DESCRIPTION
    to populate the tlvs from the play tone proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_play_tone_command
(
  const gstk_play_tone_req_type  * play_tone_cmd,
  const gstk_exp_hdr_type          gstk_hdr,
  dsm_item_type                 ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(play_tone_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_PLAY_TONE_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the alpha TLV */
  if(play_tone_cmd->alpha.length != 0)
  {
    errval = qmi_cati_text_string_tlv(&play_tone_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA );
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }
  /* Send the tone TLV */
  if(play_tone_cmd->tone != GSTK_NOT_IN_USE)
  {
    errval = qmi_cati_tone_tlv(play_tone_cmd->tone, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }
  /* Send the Duration TLV */
  if(play_tone_cmd->duration.present)
  {
    errval = qmi_cati_duration_tlv(play_tone_cmd->duration, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (play_tone_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&play_tone_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;

} /* qmi_cati_populate_play_tone_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SELECT_ITEM_COMMAND()

  DESCRIPTION
    to populate the tlvs from the select item proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_select_item_command
(
  const gstk_select_item_req_type  * select_item_cmd,
  const gstk_exp_hdr_type            gstk_hdr,
  dsm_item_type                   ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(select_item_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SELECT_ITEM_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Help Available TLV */
  errval = qmi_cati_help_available_tlv(select_item_cmd->help_available, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Presentation TLV */
  errval = qmi_cati_presentation_tlv(select_item_cmd->presentation, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_soft_key_selection_tlv(select_item_cmd->softkey_selection, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the items TLV */
  errval = qmi_cati_items_tlv(select_item_cmd->num_items,
                              select_item_cmd->items,
                              resp, FALSE);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the items with dcs TLV */
  errval = qmi_cati_items_tlv(select_item_cmd->num_items,
                              select_item_cmd->items,
                              resp, TRUE);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if (select_item_cmd->title.length != 0)
  {
    errval = qmi_cati_text_string_tlv(&select_item_cmd->title, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  if (select_item_cmd->default_item != 0x00)
  {
    errval = qmi_cati_default_item_tlv(select_item_cmd->default_item, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  if (select_item_cmd->next_action_indicator.num_items != 0)
  {
    errval = qmi_cati_next_action_indicator_tlv(&select_item_cmd->next_action_indicator, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (select_item_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&select_item_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send icon id list TLV */
  if (select_item_cmd->icon_id_list.num_items > 0)
  {
    errval = qmi_cati_icon_id_list_tlv(&select_item_cmd->icon_id_list, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_select_item_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SEND_SMS_COMMAND()

  DESCRIPTION
    to populate the tlvs from the send sms proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_send_sms_command
(
  const gstk_send_sms_req_type * send_sms_cmd,
  const gstk_exp_hdr_type        gstk_hdr,
  dsm_item_type               ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(send_sms_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SEND_SMS_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send packing required tlv */
  errval = qmi_cati_packing_required_tlv(send_sms_cmd->packing_required, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send SMS tpdu tlv */
  errval = qmi_cati_sms_tpdu_tlv(&send_sms_cmd->sms_tpdu, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_is_cdma_sms_tlv(send_sms_cmd->is_cdma_sms, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if(send_sms_cmd->address.length != 0)
  {
    errval = qmi_cati_address_tlv(&send_sms_cmd->address, resp, CATI_PRM_TYPE_ADDRESS);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  if(send_sms_cmd->alpha.length != 0)
  {
    errval = qmi_cati_text_string_tlv(&send_sms_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (send_sms_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&send_sms_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_send_sms_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SEND_SS_COMMAND()

  DESCRIPTION
    to populate the tlvs from the send ss proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_send_ss_command
(
  const gstk_send_ss_req_type  * send_ss_cmd,
  const gstk_exp_hdr_type        gstk_hdr,
  dsm_item_type               ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(send_ss_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SEND_SS_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_address_tlv(&send_ss_cmd->ss_string, resp, CATI_PRM_TYPE_ADDRESS);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if(send_ss_cmd->alpha.length != 0)
  {
    errval = qmi_cati_text_string_tlv(&send_ss_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (send_ss_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&send_ss_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_send_ss_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SEND_USSD_COMMAND()

  DESCRIPTION
    to populate the tlvs from the send ussd proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_send_ussd_command
(
  const gstk_send_ussd_req_type  * send_ussd_cmd,
  const gstk_exp_hdr_type          gstk_hdr,
  dsm_item_type                 ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(send_ussd_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SEND_USSD_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if(send_ussd_cmd->alpha.length != 0)
  {
    errval = qmi_cati_text_string_tlv(&send_ussd_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (send_ussd_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&send_ussd_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send USSD string */
  errval = qmi_cati_ussd_string_tlv(&send_ussd_cmd->ussd_string, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

send_result:
  return errval;
} /* qmi_cati_populate_send_ussd_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SETUP_CALL_COMMAND()

  DESCRIPTION
    to populate the tlvs from the setup call proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_setup_call_command
(
  const gstk_setup_call_req_type  * setup_call_cmd,
  const gstk_exp_hdr_type           gstk_hdr,
  dsm_item_type                  ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(setup_call_cmd != NULL);

  /* Send the command header TLV */
  /* check tag for type of setup call cmd*/
  switch(gstk_hdr.tag)
  {
    case GSTK_TAG_DISPLAY_ALPHA:
    case GSTK_TAG_ORIGINAL_CMD:
      errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SETUP_CALL_ALPHA_DISPLAY_REQ, gstk_hdr, resp);
      break;
    case GSTK_TAG_USER_CONF_ALPHA:
      errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SETUP_CALL_USER_CONFIRMATION_REQ, gstk_hdr, resp);
      break;
    default:
      errval = QMI_ERR_INTERNAL;
      break;
  }
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send call setup requirement TLV */
  errval = qmi_cati_call_setup_req_tlv(setup_call_cmd->call_setup_requirement, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send address TLV */
  errval = qmi_cati_address_tlv(&setup_call_cmd->address, resp, CATI_PRM_TYPE_ADDRESS);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send redial TLV, even if not necessary */
  errval = qmi_cati_redial_tlv(&setup_call_cmd->redial, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if (setup_call_cmd->user_cnf_alpha.length != 0)
  {
    /* Send the string TLV */
    errval = qmi_cati_text_string_tlv(&setup_call_cmd->user_cnf_alpha, resp, CATI_PRM_TYPE_USER_CNF_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  if (setup_call_cmd->setup_call_display_alpha.length != 0)
  {
    /* Send the string TLV */
    errval = qmi_cati_text_string_tlv(&setup_call_cmd->setup_call_display_alpha, resp, CATI_PRM_TYPE_SETUP_CALL_DISPLAY_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (setup_call_cmd->user_cnf_icon.present)
  {
    errval = qmi_cati_icon_tlv(&setup_call_cmd->user_cnf_icon, resp, CATI_PRM_TYPE_USER_CONF_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (setup_call_cmd->setup_call_display_icon.present)
  {
    errval = qmi_cati_icon_tlv(&setup_call_cmd->setup_call_display_icon, resp, CATI_PRM_TYPE_SETUP_CALL_DISPLAY_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  if(setup_call_cmd->destination_subaddress.length != 0)
  {
    errval = qmi_cati_subaddress_tlv(&setup_call_cmd->destination_subaddress, resp, CATI_PRM_TYPE_SUBADDRESS);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  if(setup_call_cmd->capability_config.length != 0)
  {
    errval =  qmi_cati_ccp_tlv(&setup_call_cmd->capability_config, resp, CATI_PRM_TYPE_CAPABILITY_CONFIG);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }
send_result:
  return errval;
} /* qmi_cati_populate_setup_call_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SETUP_MENU_COMMAND()

  DESCRIPTION
    to populate the tlvs from the setup menu proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_setup_menu_command
(
  const gstk_setup_menu_req_type  * setup_menu_cmd,
  const gstk_exp_hdr_type           gstk_hdr,
  dsm_item_type                  ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(setup_menu_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SETUP_MENU_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the Help Available TLV */
  errval = qmi_cati_help_available_tlv(setup_menu_cmd->help_available, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the soft key TLV */
  errval = qmi_cati_soft_key_selection_tlv(setup_menu_cmd->softkey_selection, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the items TLV */
  errval = qmi_cati_items_tlv(setup_menu_cmd->num_items,
                              setup_menu_cmd->items,
                              resp, FALSE);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the items with dcs TLV */
  errval = qmi_cati_items_tlv(setup_menu_cmd->num_items,
                              setup_menu_cmd->items,
                              resp, TRUE);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the alpha TLV */
  if (setup_menu_cmd->title.length != 0)
  {
    errval = qmi_cati_text_string_tlv(&setup_menu_cmd->title, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send the default item TLV */
  if (setup_menu_cmd->default_item != 0x00)
  {
    errval = qmi_cati_default_item_tlv(setup_menu_cmd->default_item, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send the next action indicator TLV */
  if (setup_menu_cmd->next_action_indicator.num_items != 0)
  {
    errval = qmi_cati_next_action_indicator_tlv(&setup_menu_cmd->next_action_indicator, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (setup_menu_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&setup_menu_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send icon id list TLV */
  if (setup_menu_cmd->icon_id_list.num_items > 0)
  {
    errval = qmi_cati_icon_id_list_tlv(&setup_menu_cmd->icon_id_list, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_setup_menu_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SETUP_IDLE_TEXT_COMMAND()

  DESCRIPTION
    to populate the tlvs from the get input proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_setup_idle_text_command
(
  const gstk_setup_idle_mode_text_req_type  * setup_idle_text_cmd,
  const gstk_exp_hdr_type                     gstk_hdr,
  dsm_item_type                            ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(setup_idle_text_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SETUP_IDLE_TEXT_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the string TLV */
  errval = qmi_cati_text_string_tlv(&setup_idle_text_cmd->text_string, resp, CATI_PRM_TYPE_TEXT_STRING);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Check if the icon is present */
  if (setup_idle_text_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&setup_idle_text_cmd->icon,resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_setup_idle_text_command */



/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SEND_DTMF_COMMAND()

  DESCRIPTION
    to populate the tlvs from the send dtmf proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_send_dtmf_command
(
  const gstk_send_dtmf_req_type  * send_dtmf_cmd,
  const gstk_exp_hdr_type          gstk_hdr,
  dsm_item_type                 ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(send_dtmf_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SEND_DTMF_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_dtmf_tlv(&send_dtmf_cmd->dtmf, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if(send_dtmf_cmd->alpha.length != 0)
  {
    errval = qmi_cati_text_string_tlv(&send_dtmf_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if (send_dtmf_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&send_dtmf_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_send_dtmf_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_LANG_NOTIFICATION_COMMAND()

  DESCRIPTION
    to populate the tlvs from the lang notification proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_lang_notification_command
(
  const gstk_lang_notification_req_type  * lang_notification_cmd,
  const gstk_exp_hdr_type                  gstk_hdr,
  dsm_item_type                         ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(lang_notification_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_LANGUAGE_NOTIFICATION_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_specific_lang_notify_tlv(lang_notification_cmd->specific_lang_notification, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_language_tlv(lang_notification_cmd->lang, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

send_result:
  return errval;
} /* qmi_cati_populate_lang_notification_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SETUP_EVENT_LIST_COMMAND()

  DESCRIPTION
    to populate the tlvs from the setup event list proactive commands

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_setup_event_list_command
(
  const gstk_evt_notify_type  * evt_notify_req,
  const gstk_exp_hdr_type       gstk_hdr,
  dsm_item_type              ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(evt_notify_req != NULL);

  /* Send the command header TLV */
  switch(gstk_hdr.command_id)
  {
    case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
      errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SETUP_EVENT_USER_ACTIVITY_REQ, gstk_hdr, resp);
      break;
    case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
      errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SETUP_EVENT_IDLE_SCREEN_REQ, gstk_hdr, resp);
      break;
    case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
      errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SETUP_EVENT_LANGUAGE_SEL_REQ, gstk_hdr, resp);
      break;
    case GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY:
      errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SETUP_EVENT_HCI_CONN_REQ, gstk_hdr, resp);
      break;
    case GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY:
      errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SETUP_EVENT_BROWSER_TERM_REQ, gstk_hdr, resp);
      break;
    default:
      errval = QMI_ERR_INTERNAL;
      break;
  }
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_event_notify_tlv(evt_notify_req->require_notification, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

send_result:
  return errval;
} /* qmi_cati_populate_setup_event_list_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_PROVIDE_LANG_INFO_COMMAND()

  DESCRIPTION
    to populate the tlvs from the provide language info proactive command

  PARAMETERS

    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_provide_lang_info_command
(
  const gstk_exp_hdr_type               gstk_hdr,
  dsm_item_type                         ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_PROVIDE_LANGUAGE_INFO_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

send_result:
  return errval;
} /* qmi_cati_populate_provide_lang_info_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_OPEN_CHANNEL_COMMAND()

  DESCRIPTION
    to populate the tlvs from the open channel proactive command

  PARAMETERS
    open_channel_cmd     : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_open_channel_command
(
  const gstk_open_ch_req_type  * open_channel_cmd,
  const gstk_exp_hdr_type        gstk_hdr,
  dsm_item_type               ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(open_channel_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_OPEN_CHANNEL_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send on demand link est from command details */
  errval = qmi_cati_on_demand_link_est_tlv(open_channel_cmd->on_demand_link_est, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send alpha identifier if present*/
  if(open_channel_cmd->alpha.alphabet_given)
  {
    errval = qmi_cati_alphabet_tlv(&open_channel_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if(open_channel_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&open_channel_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Send bearer description according to bearer type */
  switch(open_channel_cmd->bearer.bearer_type)
  {
    case GSTK_BEARER_DEFAULT:
      /* No additional TLV to add */
      break;
    case GSTK_CSD:
      errval = qmi_cati_csd_bearer_tlv(
                        open_channel_cmd->bearer.bearer_params.csd_bearer,
                        resp);
      break;
    case GSTK_GPRS:
      errval = qmi_cati_gprs_bearer_tlv(
                        open_channel_cmd->bearer.bearer_params.gprs_bearer,
                        resp);
      break;
    case GSTK_EUTRAN_EXT_PARAM:
      errval = qmi_cati_eutran_ext_bearer_tlv(
                   open_channel_cmd->bearer.bearer_params.eutran_ext_bearer,
                   resp);
      break;
    case GSTK_EUTRAN_MAPPED_UTRAN_PS:
      errval = qmi_cati_eutran_mapped_utran_ps_bearer_tlv(
                   open_channel_cmd->bearer.bearer_params.eutran_mapped_utran_ps_bearer,
                   resp);
      break;
    default:
      UIM_MSG_MED_0("Unable to send bearer description");
      errval = QMI_ERR_MISSING_ARG;
      break;
  }
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* buffer size tlv */
  errval = qmi_cati_buffer_size_tlv(open_channel_cmd->buffer_size,
                                           resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* network access point tlv */
  if(open_channel_cmd->access_point_name.alphabet_given)
  {
    errval = qmi_cati_network_access_name_tlv(&open_channel_cmd->access_point_name, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* local address (other address) tlv */
  if(open_channel_cmd->local_address.pkt_address.length > 0)
  {
    errval = qmi_cati_data_address_tlv(&open_channel_cmd->local_address, resp,
                                   CATI_PRM_TYPE_OTHER_ADDRESS);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* user login tlv */
  if(open_channel_cmd->text_string_user_login.alphabet_given)
  {
    errval = qmi_cati_alphabet_tlv(&open_channel_cmd->text_string_user_login, resp,
                                   CATI_PRM_TYPE_USER_LOGIN);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* user password tlv */
  if(open_channel_cmd->text_string_user_passwd.alphabet_given)
  {
    errval = qmi_cati_alphabet_tlv(&open_channel_cmd->text_string_user_passwd, resp,
                                   CATI_PRM_TYPE_USER_PASSWORD);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* UICC/terminal interface transport level */
  if(open_channel_cmd->transport_level.transport_protocol != GSTK_PKT_DATA_NOT_PRESENT)
  {
    errval = qmi_cati_transport_level_tlv(open_channel_cmd->transport_level, resp);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* data destination address tlv */
  if(open_channel_cmd->data_dest_address.pkt_address.length > 0)
  {
    errval = qmi_cati_data_address_tlv(&open_channel_cmd->data_dest_address, resp,
                                   CATI_PRM_TYPE_DATA_DEST_ADDRESS);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_open_channel_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_CLOSE_CHANNEL_COMMAND()

  DESCRIPTION
    to populate the tlvs from the close channel proactive command

  PARAMETERS
    close_channel_cmd    : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_close_channel_command
(
  const gstk_close_ch_req_type  * close_channel_cmd,
  const gstk_exp_hdr_type         gstk_hdr,
  dsm_item_type                ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(close_channel_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_CLOSE_CHANNEL_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_channel_id_tlv(close_channel_cmd->ch_id, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if(close_channel_cmd->alpha.alphabet_given)
  {
    errval = qmi_cati_alphabet_tlv(&close_channel_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if(close_channel_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&close_channel_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }
send_result:
  return errval;
} /* qmi_cati_populate_close_channel_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_RECEIVE_DATA_COMMAND()

  DESCRIPTION
    to populate the tlvs from the receive data proactive command

  PARAMETERS
    receive_data_cmd     : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_receive_data_command
(
  const gstk_receive_data_req_type  * receive_data_cmd,
  const gstk_exp_hdr_type             gstk_hdr,
  dsm_item_type                    ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(receive_data_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_RECEIVE_DATA_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_channel_id_tlv(receive_data_cmd->ch_id, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_channel_data_len_tlv(receive_data_cmd->ch_data_length, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if(receive_data_cmd->alpha.alphabet_given)
  {
    errval = qmi_cati_alphabet_tlv(&receive_data_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if(receive_data_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&receive_data_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_receive_data_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SEND_DATA_COMMAND()

  DESCRIPTION
    to populate the tlvs from the send data proactive command

  PARAMETERS
    send_data_cmd        : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_send_data_command
(
  const gstk_send_data_req_type     * send_data_cmd,
  const gstk_exp_hdr_type             gstk_hdr,
  dsm_item_type                    ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(send_data_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_SEND_DATA_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_channel_id_tlv(send_data_cmd->ch_id, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_channel_data_tlv(send_data_cmd->ch_data, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* send data immediately bit from GSTK command details*/
  errval = qmi_cati_send_data_immediately_tlv(send_data_cmd->send_data_immediately, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  if(send_data_cmd->alpha.alphabet_given)
  {
    errval = qmi_cati_alphabet_tlv(&send_data_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

  /* Check if the icon is present */
  if(send_data_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&send_data_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_send_data_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_ACTIVATE_COMMAND()

  DESCRIPTION
    to populate the tlvs from the activate proactive command

  PARAMETERS
    gstk_activate_req_type : Decoded activate cmd
    gstk_hdr               : Header of the proactive command
    resp                   : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_activate_command
(
  const gstk_activate_req_type        * activate_cmd,
  const gstk_exp_hdr_type               gstk_hdr,
  dsm_item_type                         ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_ACTIVATE_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  errval = qmi_cati_activate_target_tlv(activate_cmd->target, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

send_result:
  return errval;
} /* qmi_cati_populate_activate_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_REFRESH_ALPHA_COMMAND()

  DESCRIPTION
    to populate the tlvs from the refresh proactive command

  PARAMETERS
    gstk_decoded_command : Decoded proactive command
    gstk_hdr             : Header of the proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_refresh_alpha_command
(
  const gstk_refresh_req_type  * refresh_cmd,
  const gstk_exp_hdr_type        gstk_hdr,
  dsm_item_type               ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(refresh_cmd != NULL);

  /* Send the command header TLV */
  errval = qmi_cati_pro_cmd_header_tlv(CATI_EVENT_REFRESH_ALPHA_REQ, gstk_hdr, resp);
  if (errval != QMI_ERR_NONE)
  {
    goto send_result;
  }

  /* Send the alpha TLV */
  if(refresh_cmd->alpha.length != 0)
  {
    errval = qmi_cati_text_string_tlv(&refresh_cmd->alpha, resp, CATI_PRM_TYPE_ALPHA );
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }
  /* Check if the icon is present */
  if (refresh_cmd->icon.present)
  {
    errval = qmi_cati_icon_tlv(&refresh_cmd->icon, resp, CATI_PRM_TYPE_ICON);
    if (errval != QMI_ERR_NONE)
    {
      goto send_result;
    }
  }

send_result:
  return errval;
} /* qmi_cati_populate_refresh_alpha_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SCWS_OPEN_CHANNEL_COMMAND()

  DESCRIPTION
    to populate the tlvs from the send data proactive command

  PARAMETERS
    scws_open_ch_cmd     : Decoded scws command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_scws_open_channel_command
(
  const gstk_scws_open_ch_req_type     * scws_open_ch_cmd,
  dsm_item_type                       ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(scws_open_ch_cmd != NULL);

  errval = qmi_cati_scws_open_channel_info_tlv(scws_open_ch_cmd->ch_id,
                                               scws_open_ch_cmd->port_number,
                                               scws_open_ch_cmd->buffer_size,
                                               resp);
  if (errval != QMI_ERR_NONE)
  {
    return errval;
  }

  if(scws_open_ch_cmd->alpha.alphabet_given)
  {
    errval = qmi_cati_alphabet_tlv(&scws_open_ch_cmd->alpha,
                                   resp,
                                   CATI_PRM_TYPE_SCWS_ALPHA);
  }

  return errval;
} /* qmi_cati_populate_scws_open_channel_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SCWS_CLOSE_CHANNEL_COMMAND()

  DESCRIPTION
    to populate the tlvs from the send data proactive command

  PARAMETERS
    send_data_cmd        : Decoded proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_scws_close_channel_command
(
  const gstk_scws_close_ch_req_type     * scws_close_ch_cmd,
  dsm_item_type                        ** resp
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(scws_close_ch_cmd != NULL);

  errval = qmi_cati_scws_channel_state_tlv(scws_close_ch_cmd->ch_id,
                                           scws_close_ch_cmd->channel_state,
                                           resp);

  return errval;
} /* qmi_cati_populate_scws_close_channel_command */


/*===========================================================================
  FUNCTION QMI_CATI_POPULATE_SCWS_SEND_DATA_COMMAND()

  DESCRIPTION
    to populate the tlvs from the send data proactive command

  PARAMETERS
    send_data_cmd        : Decoded proactive command
    resp                 : pointer to tlv

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_scws_send_data_command
(
  const gstk_scws_send_data_req_type     * scws_send_data_cmd,
  dsm_item_type                         ** resp,
  uint16                                   index
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  ASSERT(scws_send_data_cmd != NULL);

  errval = qmi_cati_scws_channel_data_tlv(scws_send_data_cmd->ch_id,
                                          scws_send_data_cmd->ch_data,
                                          index,
                                          resp);

  return errval;
} /* qmi_cati_populate_scws_send_data_command */

/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_DECODED_PROACTIVE_COMMAND()

  DESCRIPTION
    to populate the tlvs from the decoded proactive command

  PARAMETERS
    ind             : output sdu
    decoded_cmd     : GSTK decoded proactive command

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_decoded_proactive_command
(
  dsm_item_type                 ** ind,
  const gstk_cmd_from_card_type  * decoded_cmd
)
{
  qmi_error_e_type  errval = QMI_ERR_NONE;

  ASSERT(decoded_cmd != NULL);

  switch (decoded_cmd->hdr_cmd.command_id)
  {
    case GSTK_DISPLAY_TEXT_REQ:
      errval = qmi_cati_populate_display_text_command(
                    &decoded_cmd->cmd.display_text_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_GET_INKEY_REQ:
      errval = qmi_cati_populate_get_inkey_command(
                    &decoded_cmd->cmd.get_inkey_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_GET_INPUT_REQ:
      errval = qmi_cati_populate_get_input_command(
                    &decoded_cmd->cmd.get_input_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_LAUNCH_BROWSER_REQ:
       errval = qmi_cati_populate_launch_browser_command(
                    &decoded_cmd->cmd.launch_browser_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_PLAY_TONE_REQ:
      errval = qmi_cati_populate_play_tone_command(
                    &decoded_cmd->cmd.play_tone_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SELECT_ITEM_REQ:
      errval = qmi_cati_populate_select_item_command(
                    &decoded_cmd->cmd.select_item_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SEND_SMS_REQ:
      errval = qmi_cati_populate_send_sms_command(
                    &decoded_cmd->cmd.send_sms_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SEND_SS_REQ:
      errval = qmi_cati_populate_send_ss_command(
                    &decoded_cmd->cmd.send_ss_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SEND_USSD_REQ:
      errval = qmi_cati_populate_send_ussd_command(
                    &decoded_cmd->cmd.send_ussd_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SETUP_CALL_REQ:
      errval = qmi_cati_populate_setup_call_command(
                    &decoded_cmd->cmd.setup_call_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SETUP_MENU_REQ:
      errval = qmi_cati_populate_setup_menu_command(
                    &decoded_cmd->cmd.setup_menu_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SETUP_IDLE_TEXT_REQ:
      errval = qmi_cati_populate_setup_idle_text_command(
                    &decoded_cmd->cmd.setup_idle_mode_txt_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_PROVIDE_LANG_INFO_REQ:
      errval = qmi_cati_populate_provide_lang_info_command(
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SEND_DTMF_REQ:
      errval = qmi_cati_populate_send_dtmf_command(
                    &decoded_cmd->cmd.send_dtmf_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_LANG_NOTIFICATION_REQ:
      errval = qmi_cati_populate_lang_notification_command(
                    &decoded_cmd->cmd.lang_notification_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
    case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
    case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
    case GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY:
    case GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY:
      errval = qmi_cati_populate_setup_event_list_command(
                    &decoded_cmd->cmd.evt_notify_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_OPEN_CH_REQ:
      errval = qmi_cati_populate_open_channel_command(
                    &decoded_cmd->cmd.open_ch_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_CLOSE_CH_REQ:
      errval = qmi_cati_populate_close_channel_command(
                    &decoded_cmd->cmd.close_ch_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_SEND_DATA_REQ:
      errval = qmi_cati_populate_send_data_command(
                    &decoded_cmd->cmd.send_data_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_RECEIVE_DATA_REQ:
      errval = qmi_cati_populate_receive_data_command(
                    &decoded_cmd->cmd.receive_data_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_ACTIVATE_REQ:
      errval = qmi_cati_populate_activate_command(
                    &decoded_cmd->cmd.activate_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    case GSTK_REFRESH_REQ:
      errval = qmi_cati_populate_refresh_alpha_command(
                    &decoded_cmd->cmd.refresh_pro_cmd_req,
                    decoded_cmd->hdr_cmd,
                    ind);
      break;

    default:
      /* Unhandled GSTK event */
      errval = QMI_ERR_INTERNAL;
      break;
  }

  /* In all cases, insert the slot id */
  if (errval == QMI_ERR_NONE)
  {
    errval = qmi_cati_slot_id_tlv(
                  decoded_cmd->hdr_cmd.sim_slot_id,
                  ind,
                  CATI_PRM_TYPE_SLOT_ID);
  }

  /* check on hdr tag */
  if (errval == QMI_ERR_NONE &&
      (decoded_cmd->hdr_cmd.command_id == GSTK_SEND_SMS_REQ ||
       decoded_cmd->hdr_cmd.command_id == GSTK_SETUP_CALL_REQ ||
       decoded_cmd->hdr_cmd.command_id == GSTK_SEND_DTMF_REQ ||
       decoded_cmd->hdr_cmd.command_id == GSTK_SEND_USSD_REQ ||
       decoded_cmd->hdr_cmd.command_id == GSTK_SEND_SS_REQ))
  {
    UIM_MSG_HIGH_0("Include qmi_cati_response_tlv for network related cmd");
    errval = qmi_cati_response_tlv(
                  decoded_cmd->hdr_cmd.tag,
                  ind);
  }
  return errval;
} /* qmi_cat_response_decoded_proactive_command */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_DECODED_BIP_STATUS()

  DESCRIPTION
    to populate the tlvs from the decoded proactive command

  PARAMETERS
    ind            : output sdu
    bip cnf        : BIP cnf

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_decoded_bip_status
(
  dsm_item_type                         ** ind,
  const qmi_cat_decoded_bip_status_type  * bip_status
)
{
  qmi_error_e_type  errval = QMI_ERR_NONE;

  ASSERT(bip_status != NULL);

  errval = qmi_cati_bip_status_tlv(bip_status->ch_id,
                                   bip_status->bip_status,
                                   ind);

  if (errval == QMI_ERR_NONE)
  {
    errval = qmi_cati_slot_id_tlv(
                  bip_status->slot_id,
                  ind,
                  CATI_PRM_TYPE_SLOT_ID);
  }

  return errval;
} /* qmi_cat_response_decoded_bip_status */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_SCWS_COMMAND()

  DESCRIPTION
    to populate the tlvs from the scws command

  PARAMETERS
    ind             : output sdu
    scws_cmd        : GSTK decoded scws command

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_scws_command
(
  dsm_item_type                 ** ind,
  const gstk_cmd_from_card_type  * scws_cmd,
  uint16                           index
)
{
  qmi_error_e_type  errval  = QMI_ERR_NONE;

  ASSERT(scws_cmd != NULL);

  switch (scws_cmd->hdr_cmd.command_id)
  {
    case GSTK_SCWS_OPEN_CH_REQ:
      errval = qmi_cati_populate_scws_open_channel_command(
                    &scws_cmd->cmd.scws_open_ch_cmd_req,
                    ind);
      break;

    case GSTK_SCWS_CLOSE_CH_REQ:
      errval = qmi_cati_populate_scws_close_channel_command(
                    &scws_cmd->cmd.scws_close_ch_cmd_req,
                    ind);
      break;

    case GSTK_SCWS_SEND_DATA_REQ:
      errval = qmi_cati_populate_scws_send_data_command(
                    &scws_cmd->cmd.scws_send_data_cmd_req,
                    ind, index);
      break;

    default:
      /* Unhandled GSTK event */
      errval = QMI_ERR_INTERNAL;
      break;
  }

  /* In DSDS, insert the slot id */
  if (errval == QMI_ERR_NONE)
  {
    errval = qmi_cati_slot_id_tlv(
                  scws_cmd->hdr_cmd.sim_slot_id,
                  ind,
                  CATI_PRM_TYPE_SCWS_SLOT_ID);
  }

  return errval;
} /* qmi_cat_response_scws_command */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_RAW_PROACTIVE_COMMAND()

  DESCRIPTION
    Populates the tlvs from the raw proactive command

  PARAMETERS
    ind             : output sdu
    cmd_ref_id      : command reference id
    raw_data_len    : length of the raw proactive command
    raw_data_ptr    : value of the raw proactive command
    slot_id         : slot id

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_raw_proactive_command
(
  byte                             tlv_id,
  dsm_item_type                 ** ind,
  uint32                           cmd_ref_id,
  uint16                           raw_data_len,
  const uint8                    * raw_data_ptr,
  gstk_slot_id_enum_type           slot_id,
  uint8                            response_type
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  PACKED struct PACKED_POST
  {
    uint32  cmd_ref_id;
    uint16  raw_data_len;
    uint8   raw_data[CAT_RAW_MAX_DATA_LEN];
  } tlv_data;

  /* Check length */
  if (raw_data_len > CAT_RAW_MAX_DATA_LEN)
  {
    return QMI_ERR_NO_MEMORY;
  }

  memset(&tlv_data, 0, sizeof(tlv_data));

  tlv_data.cmd_ref_id = cmd_ref_id;
  if (raw_data_len > 0 && raw_data_ptr != NULL)
  {
    tlv_data.raw_data_len = raw_data_len;
    (void)memscpy(tlv_data.raw_data,
                  sizeof(tlv_data.raw_data),
                  raw_data_ptr,
                  raw_data_len);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     tlv_id,
                                     (sizeof(tlv_data.cmd_ref_id) + sizeof(tlv_data.raw_data_len)+ tlv_data.raw_data_len),
                                     (void *)&tlv_data) )
  {
    UIM_MSG_HIGH_0("Unable to generate TLV for raw proactive cmd!");
    return QMI_ERR_NO_MEMORY;
  }

  /* Insert the slot id */
  errval = qmi_cati_slot_id_tlv(
                slot_id,
                ind,
                CATI_PRM_TYPE_SLOT_ID);

  /* Insert response type tlv */
  if (errval == QMI_ERR_NONE)
  {
    UIM_MSG_HIGH_1("Include qmi_cati_response_tlv, 0x%x", response_type);
    errval = qmi_cati_response_tlv(
                  response_type,
                  ind);
  }

  return errval;
} /* qmi_cat_response_raw_proactive_command */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_END_PROACTIVE_SESSION()

  DESCRIPTION
    Populates the tlv for the end proactive session

  PARAMETERS
    ind            : output sdu
    end_type       : type of end of proactive session
    slot_id        : slot id

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_end_proactive_session
(
  dsm_item_type                 ** ind,
  uint8                            end_type,
  gstk_slot_id_enum_type           slot_id
)
{
  qmi_error_e_type errval = QMI_ERR_NONE;

  PACKED struct PACKED_POST
  {
    uint8  proactive_session_end_type;
  } tlv_data;

  memset(&tlv_data, 0, sizeof(tlv_data));

  tlv_data.proactive_session_end_type  = end_type;

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     CATI_PRM_TYPE_END_PROACTIVE_SESSION,
                                     sizeof(tlv_data),
                                     (void *)&tlv_data) )
  {
    UIM_MSG_HIGH_0("Unable to generate TLV for end proactive session!");
    return QMI_ERR_NO_MEMORY;
  }

  errval = qmi_cati_slot_id_tlv(
                slot_id,
                ind,
                CATI_PRM_TYPE_SLOT_ID);

  return errval;
} /* qmi_cat_response_end_proactive_session */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_SET_EVENT_REPORT()

  DESCRIPTION
    Populates the tlv for the set event report

  PARAMETERS
    ind                 : output sdu
    raw_mask            : raw mask
    decoded_mask        : decoded mask
    full_func_mask      : full function mask

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_set_event_report
(
  dsm_item_type                 ** response,
  uint32                           raw_mask,
  uint32                           decoded_mask,
  uint32                           full_func_mask
)
{
  struct
  {
    uint32 mask;
  } tlv_data;

  if (raw_mask != 0)
  {
    tlv_data.mask = raw_mask;
    if (FALSE == qmi_svc_put_param_tlv(response,
                                       CATI_SET_EVENT_REPORT_RAW_RESP_TLV,
                                       sizeof(tlv_data),
                                       (void *)&tlv_data ))
    {
      UIM_MSG_HIGH_0("Unable to send response for SET EVENT REPORT");
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (decoded_mask != 0)
  {
    tlv_data.mask = decoded_mask;
    if (FALSE == qmi_svc_put_param_tlv(response,
                                     CATI_SET_EVENT_REPORT_DECODED_RESP_TLV,
                                     sizeof(tlv_data),
                                     (void *)&tlv_data ))
    {
      UIM_MSG_HIGH_0("Unable to send response for SET EVENT REPORT");
      return QMI_ERR_NO_MEMORY;
    }
  }

  if (full_func_mask != 0)
  {
    tlv_data.mask = full_func_mask;
    if (FALSE == qmi_svc_put_param_tlv(response,
                                       CATI_SET_EVENT_REPORT_FULL_FUNC_RESP_TLV,
                                       sizeof(tlv_data),
                                       (void *)&tlv_data ))
    {
      UIM_MSG_HIGH_0("Unable to send response for SET EVENT REPORT");
      return QMI_ERR_NO_MEMORY;
    }
  }
  return QMI_ERR_NONE;
} /* qmi_cat_response_set_event_report */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_GET_SERVICE_STATE()

  DESCRIPTION
    Populates the tlv for the response of get service state

  PARAMETERS
    ind                 : output sdu
    raw_client_mask     : raw mask of this client
    raw_all_mask        : raw mask of all clients
    decoded_client_mask : decoded mask of this client
    decoded_all_mask    : decoded mask of all clients
    full_func_mask      : full function mask for all slots

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_get_service_state
(
  dsm_item_type                 ** response,
  uint32                           raw_client_mask,
  uint32                           raw_global_mask,
  uint32                           decoded_client_mask,
  uint32                           decoded_global_mask,
  uint32                           full_func_global_mask
)
{
  struct
  {
    uint32  global_mask;
    uint32  client_mask;
  } raw_mask;

  struct
  {
    uint32  global_mask;
    uint32  client_mask;
  } decoded_mask;

  struct
  {
    uint32  full_func_mask;
  } full_func_mask;

  raw_mask.client_mask          = raw_client_mask;
  raw_mask.global_mask          = raw_global_mask;
  decoded_mask.client_mask      = decoded_client_mask;
  decoded_mask.global_mask      = decoded_global_mask;
  full_func_mask.full_func_mask = full_func_global_mask;

  if (FALSE == qmi_svc_put_param_tlv(response,
                                     CATI_GET_SERVICE_STATE_RAW_TLV,
                                     sizeof(raw_mask),
                                     (void *)&raw_mask ) ||
      FALSE == qmi_svc_put_param_tlv(response,
                                     CATI_GET_SERVICE_STATE_DECODED_TLV,
                                     sizeof(decoded_mask),
                                     (void *)&decoded_mask ) ||
      FALSE == qmi_svc_put_param_tlv(response,
                                     CATI_GET_SERVICE_STATE_FULL_FUNC_TLV,
                                     sizeof(full_func_mask),
                                     (void *)&full_func_mask ))
  {
    UIM_MSG_HIGH_0("Unable to send response for GET SERVICE STATUS!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cat_response_get_service_state */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_GET_TERMINAL_PROFILE()

  DESCRIPTION
    Populates the tlvs for TP content

  PARAMETERS
    ind               : output sdu
    buf_len           : buffer length
    buf_ptr           : pointer to data buffer

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_get_terminal_profile
(
  dsm_item_type                 ** response,
  uint32                           buf_len,
  uint8                          * buf_ptr
)
{
  struct
  {
    uint8  tp_data_len;
    uint8  tp_data[GSTK_TERMINAL_PROFILE_SIZE];
  } tp_buffer;

  if(buf_len > GSTK_TERMINAL_PROFILE_SIZE)
  {
    return QMI_ERR_INTERNAL;
  }

  tp_buffer.tp_data_len = (uint8)buf_len;
  (void)memscpy(tp_buffer.tp_data,
                sizeof(tp_buffer.tp_data),
                buf_ptr,
                buf_len);

  if (FALSE == qmi_svc_put_param_tlv(response,
                                     CATI_GET_TERMINAL_PROFILE_DATA_TLV,
                                     (uint16)(buf_len + sizeof(uint8)),
                                     (void *)&tp_buffer))
  {
    UIM_MSG_HIGH_0("Unable to send response for GET terminal profile!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cat_response_get_terminal_profile */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_GET_CONFIGURATION()

  DESCRIPTION
    Populates the tlv for get configuration

  PARAMETERS
    ind                 : output sdu
    tp_len              : len of custom_tp
    custom_tp_ptr       : value of custom_tp

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_get_configuration
(
  dsm_item_type                 ** response,
  uint8                            config_mode,
  uint8                            custom_tp_len,
  uint8                          * custom_tp_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint8   config_mode;
  } tlv_config_mode;

  PACKED struct PACKED_POST
  {
    uint8   custom_tp_len;
    uint8   custom_tp[CATI_TLV_CUSTOM_TP_MAX];
  } tlv_custom_tp;

    /* Check length */
  if (custom_tp_len > CATI_TLV_CUSTOM_TP_MAX)
  {
    return QMI_ERR_NO_MEMORY;
  }

  memset(&tlv_config_mode, 0, sizeof(tlv_config_mode));
  memset(&tlv_custom_tp, 0, sizeof(tlv_custom_tp));

  tlv_config_mode.config_mode = config_mode;

  if( FALSE == qmi_svc_put_param_tlv(response,
                                     CATI_GET_CONFIGURATION_CONFIG_MODE_TLV,
                                     sizeof(uint8),
                                     (void *)&tlv_config_mode))
  {
    UIM_MSG_HIGH_0("Unable to generate TLV for config_mode!");
    return QMI_ERR_NO_MEMORY;
  }

  if(custom_tp_len == 0)
  {
    UIM_MSG_HIGH_0("custom_tp not available, skip tlv");
  }
  else if (custom_tp_len > 0 && custom_tp_ptr != NULL)
  {
    tlv_custom_tp.custom_tp_len = custom_tp_len;
    (void)memscpy(tlv_custom_tp.custom_tp,
                  sizeof(tlv_custom_tp.custom_tp),
                  custom_tp_ptr,
                  custom_tp_len);

    if( FALSE == qmi_svc_put_param_tlv(response,
                                       CATI_GET_CONFIGURATION_CUSTOM_TP_TLV,
                                       (sizeof(uint8) + tlv_custom_tp.custom_tp_len),
                                       (void *)&tlv_custom_tp))
    {
      UIM_MSG_HIGH_0("Unable to generate TLV for custom_tp!");
      return QMI_ERR_NO_MEMORY;
    }
  }
  else
  {
    UIM_MSG_ERR_0("Unable to generate TLV for custom_tp!");
    return QMI_ERR_INTERNAL;
  }

  return QMI_ERR_NONE;
} /* qmi_cat_response_get_configuration */


/*===========================================================================
  FUNCTION QMI_CAT_EXTRACT_PACKED_TLV()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    sdu_in          : input sdu
    tlv_len         : length of the TLV
    tlv_present_ptr : boolean that indicates if the TLV is already present
    min_buffer_size : minimum size of the TLV
    max_buffer_size : maximum size of the TLV (corresponding to buffer size)
    buffer_ptr      : buffer where TLV is extracted

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cat_extract_packed_tlv
(
  dsm_item_type                     ** sdu_in,
  uint16                               tlv_len,
  boolean                            * tlv_present_ptr,
  uint16                               min_buffer_size,
  uint16                               max_buffer_size,
  void                               * buffer_ptr
)
{
  int32 temp = 0;

  ASSERT(sdu_in != NULL);
  ASSERT(tlv_present_ptr != NULL);
  ASSERT(buffer_ptr != NULL);

  /* Check if the TLV is already present */
  if (*tlv_present_ptr)
  {
    UIM_MSG_ERR_0("Duplicate TLV");
    return QMI_ERR_MALFORMED_MSG;
  }

  /* Check length */
  if (tlv_len < min_buffer_size)
  {
    UIM_MSG_ERR_0("QMI_CAT Invalid length");
    return QMI_ERR_MALFORMED_MSG;
  }
  if (tlv_len > max_buffer_size)
  {
    UIM_MSG_ERR_0("QMI_CAT Invalid length");
    return QMI_ERR_ARG_TOO_LONG;
  }

  /* Extract TLV */
  temp = dsm_pullup(sdu_in, (void*)buffer_ptr, tlv_len);
  if (temp != tlv_len)
  {
    UIM_MSG_ERR_0("QMI_CAT DSM pullout error");
    return QMI_ERR_MALFORMED_MSG;
  }
  *tlv_present_ptr = TRUE;

  return QMI_ERR_NONE;
} /* qmi_cat_extract_packed_tlv */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SET_EVENT_REPORT()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_set_event_report
(
  dsm_item_type                     ** sdu_in,
  qmi_cat_in_set_event_report_type   * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_set_event_report_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_SET_EVENT_REPORT_RAW_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->raw_mask_present,
                                            sizeof(parsed_data_ptr->raw_mask),
                                            sizeof(parsed_data_ptr->raw_mask),
                                            &parsed_data_ptr->raw_mask);
        break;

      case CATI_SET_EVENT_REPORT_DECODED_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->decoded_mask_present,
                                            sizeof(parsed_data_ptr->decoded_mask),
                                            sizeof(parsed_data_ptr->decoded_mask),
                                            &parsed_data_ptr->decoded_mask);
        break;

      case CATI_SET_EVENT_REPORT_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot_mask),
                                            sizeof(parsed_data_ptr->slot_mask),
                                            &parsed_data_ptr->slot_mask);
        break;

      case CATI_SET_EVENT_REPORT_FULL_FUNC_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->full_func_mask_present,
                                            sizeof(parsed_data_ptr->full_func_mask),
                                            sizeof(parsed_data_ptr->full_func_mask),
                                            &parsed_data_ptr->full_func_mask);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  if (result == QMI_ERR_NONE)
  {
    /* Check that at least one of the 3 TLV is received */
    if (parsed_data_ptr->raw_mask_present == FALSE &&
        parsed_data_ptr->decoded_mask_present == FALSE &&
        parsed_data_ptr->full_func_mask_present == FALSE)
    {
      result = QMI_ERR_MISSING_ARG;
    }

    /* Check on slot */
    if (parsed_data_ptr->slot_present &&
        (parsed_data_ptr->slot_mask & 0x07) == 0)
    {
      result = QMI_ERR_INVALID_ARG;
    }
  }

  return result;
} /* qmi_cat_parse_request_set_event_report */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_GET_EVENT_REPORT()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_get_event_report
(
  dsm_item_type                     ** sdu_in,
  qmi_cat_in_get_event_report_type   * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_get_event_report_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_GET_EVENT_REPORT_COMMAND_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->proactive_command_present,
                                            sizeof(parsed_data_ptr->proactive_command),
                                            sizeof(parsed_data_ptr->proactive_command),
                                            &parsed_data_ptr->proactive_command);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that mandatory TLV is received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->proactive_command_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on format */
    if (parsed_data_ptr->proactive_command_present &&
        parsed_data_ptr->proactive_command.format != 1 &&
        parsed_data_ptr->proactive_command.format != 2)
    {
      result = QMI_ERR_INVALID_ARG;
    }
  }

  return result;
} /* qmi_cat_parse_request_get_event_report */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_RAW_TR()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_raw_tr
(
  dsm_item_type           ** sdu_in,
  qmi_cat_in_raw_tr_type   * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_raw_tr_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_RAW_TR_TERMINAL_RESPONSE_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->term_resp_present,
                                            QMI_CAT_RAW_TR_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->term_resp),
                                            &parsed_data_ptr->term_resp);
        break;

      case CATI_RAW_TR_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            &parsed_data_ptr->slot);
        break;
      case CATI_RAW_TR_INDICATION_TOKEN_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->ind_token_present,
                                            sizeof(parsed_data_ptr->ind_token),
                                            sizeof(parsed_data_ptr->ind_token),
                                            &parsed_data_ptr->ind_token);
        break;
      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that all mandatory TLV were received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->term_resp_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on terminal response */
    if (parsed_data_ptr->term_resp_present &&
        parsed_data_ptr->term_resp.tr_len >
          sizeof(parsed_data_ptr->term_resp.terminal_response))
    {
      result = QMI_ERR_MALFORMED_MSG;
    }
    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_raw_tr */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_RAW_ENVELOPE()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_raw_envelope
(
  dsm_item_type                 ** sdu_in,
  qmi_cat_in_raw_envelope_type   * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_raw_envelope_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_RAW_ENVELOPE_ENVELOPE_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->envelope_present,
                                            QMI_CAT_RAW_ENVELOPE_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->envelope),
                                            &parsed_data_ptr->envelope);
        break;

      case CATI_RAW_ENVELOPE_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            &parsed_data_ptr->slot);
        break;

      case CATI_RAW_ENVELOPE_INDICATION_TOKEN_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->ind_token_present,
                                            sizeof(parsed_data_ptr->ind_token),
                                            sizeof(parsed_data_ptr->ind_token),
                                            &parsed_data_ptr->ind_token);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that all mandatory TLV were received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->envelope_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on terminal response */
    if (parsed_data_ptr->envelope_present &&
        parsed_data_ptr->envelope.env_cmd_len >
          sizeof(parsed_data_ptr->envelope.envelope_data))
    {
      result = QMI_ERR_MALFORMED_MSG;
    }
    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_raw_envelope */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_DECODED_TR()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_decoded_tr
(
  dsm_item_type               ** sdu_in,
  qmi_cat_in_decoded_tr_type   * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_decoded_tr_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_DECODED_TR_TERMINAL_RESPONSE_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->term_resp_present,
                                            QMI_CAT_DECODED_TR_TERM_RESP_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->term_resp),
                                            &parsed_data_ptr->term_resp);
        break;

      case CATI_DECODED_TR_TEXT_STRING_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->text_string_present,
                                            QMI_CAT_DECODED_TR_TEXT_STRING_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->text_string),
                                            &parsed_data_ptr->text_string);
        break;

      case CATI_DECODED_TR_ITEM_IDENTIFIER_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->item_id_present,
                                            sizeof(parsed_data_ptr->item_id),
                                            sizeof(parsed_data_ptr->item_id),
                                            &parsed_data_ptr->item_id);
        break;

      case CATI_DECODED_TR_GET_INKEY_EXTRA_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->get_inkey_term_resp_extra_present,
                                            QMI_CAT_DECODED_TR_GET_INKEY_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->get_inkey_term_resp_extra),
                                            &parsed_data_ptr->get_inkey_term_resp_extra);
        break;

      case CATI_DECODED_TR_LANGUAGE_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->language_present,
                                            sizeof(parsed_data_ptr->language),
                                            sizeof(parsed_data_ptr->language),
                                            &parsed_data_ptr->language);
        break;

      case CATI_DECODED_TR_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            &parsed_data_ptr->slot);
        break;

      case CATI_DECODED_TR_GET_INKEY_YES_NO_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->get_inkey_term_resp_yes_no_present,
                                            sizeof(parsed_data_ptr->get_inkey_term_resp_yes_no),
                                            sizeof(parsed_data_ptr->get_inkey_term_resp_yes_no),
                                            &parsed_data_ptr->get_inkey_term_resp_yes_no);
        break;

      case CATI_DECODED_TR_INDICATION_TOKEN_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->ind_token_present,
                                            sizeof(parsed_data_ptr->ind_token),
                                            sizeof(parsed_data_ptr->ind_token),
                                            &parsed_data_ptr->ind_token);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that all mandatory TLV were received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->term_resp_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on additional info */
    if (parsed_data_ptr->term_resp_present &&
        parsed_data_ptr->term_resp.additional_info_length >
          sizeof(parsed_data_ptr->term_resp.additional_info_data))
    {
      result = QMI_ERR_MALFORMED_MSG;
    }
    /* Check on text string */
    if (parsed_data_ptr->text_string_present &&
        parsed_data_ptr->text_string.length_of_string >
          ((sizeof(parsed_data_ptr->text_string.text))))
    {
      result = QMI_ERR_MALFORMED_MSG;
    }
    /* Check on get inkey */
    if (parsed_data_ptr->get_inkey_term_resp_extra_present &&
        parsed_data_ptr->get_inkey_term_resp_extra.length_of_string >
          ((sizeof(parsed_data_ptr->get_inkey_term_resp_extra.text))))
    {
      result = QMI_ERR_MALFORMED_MSG;
    }
    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
    /* Check on get inkey yes / no info */
    if (parsed_data_ptr->get_inkey_term_resp_yes_no_present &&
        parsed_data_ptr->get_inkey_term_resp_yes_no.yes_no != 0 &&
        parsed_data_ptr->get_inkey_term_resp_yes_no.yes_no != 1)
    {
      result = QMI_ERR_INVALID_ARG;
    }
  }

  return result;
} /* qmi_cat_parse_request_decoded_tr */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_DECODED_ENVELOPE()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_decoded_envelope
(
  dsm_item_type                    ** sdu_in,
  qmi_cat_in_decoded_envelope_type  * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output values */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_decoded_envelope_type));

  while (*sdu_in)
  {
    uint8            type             = 0;
    uint16           len              = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_TYPE_ENVELOPE_REQUEST_REQUIRED:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->env_hdr_present,
                                            sizeof(parsed_data_ptr->env_hdr),
                                            sizeof(parsed_data_ptr->env_hdr),
                                            &parsed_data_ptr->env_hdr);
        break;

      case CATI_ENV_TYPE_ITEM_CHOSEN:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->item_id_present,
                                            sizeof(parsed_data_ptr->item_id),
                                            sizeof(parsed_data_ptr->item_id),
                                            &parsed_data_ptr->item_id);
        break;

      case CATI_ENV_TYPE_HELP_REQUESTED:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->help_request_present,
                                            sizeof(parsed_data_ptr->help_request),
                                            sizeof(parsed_data_ptr->help_request),
                                            &parsed_data_ptr->help_request);
        break;

      case CATI_ENV_TYPE_LANGUAGE:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->language_present,
                                            sizeof(parsed_data_ptr->language),
                                            sizeof(parsed_data_ptr->language),
                                            &parsed_data_ptr->language);
        break;

      case CATI_ENV_TYPE_SLOT:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            &parsed_data_ptr->slot);
        break;

      case CATI_ENV_TYPE_ADDRESS:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->address_present,
                                            QMI_CAT_DECODED_ENVELOPE_ADDRESS_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->address),
                                            &parsed_data_ptr->address);
        break;

      case CATI_ENV_TYPE_SUBADDRESS:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->subaddress_present,
                                            QMI_CAT_DECODED_ENVELOPE_SUBADDRESS_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->subaddress),
                                            &parsed_data_ptr->subaddress);
        break;

      case CATI_ENV_TYPE_CAPABILITY_CONF_1:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->ccp1_present,
                                            QMI_CAT_DECODED_ENVELOPE_CCP_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->ccp1),
                                            &parsed_data_ptr->ccp1);
        break;

      case CATI_ENV_TYPE_CAPABILITY_CONF_2:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->ccp2_present,
                                            QMI_CAT_DECODED_ENVELOPE_CCP_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->ccp2),
                                            &parsed_data_ptr->ccp2);
        break;

      case CATI_ENV_TYPE_USSD_STRING:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->ussd_string_present,
                                            QMI_CAT_DECODED_ENVELOPE_USSD_STR_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->ussd_string),
                                            &parsed_data_ptr->ussd_string);
        break;

      case CATI_ENV_TYPE_PDP_CONTEXT_ACT:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->pdp_context_act_present,
                                            QMI_CAT_DECODED_ENVELOPE_ACT_PARAM_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->pdp_context_act),
                                            &parsed_data_ptr->pdp_context_act);
        break;

      case CATI_ENV_TYPE_EPS_PDN_CONNECT_ACT:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->eps_pdn_conn_act_present,
                                            QMI_CAT_DECODED_ENVELOPE_ACT_PARAM_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->eps_pdn_connect_act),
                                            &parsed_data_ptr->eps_pdn_connect_act);
        break;

      case CATI_ENV_TYPE_BROWSER_TERM_CAUSE:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->browser_term_cause_present,
                                            sizeof(parsed_data_ptr->browser_term_cause),
                                            sizeof(parsed_data_ptr->browser_term_cause),
                                            &parsed_data_ptr->browser_term_cause);
        break;

       case CATI_ENV_TYPE_SMS_TPDU:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->sms_tpdu_present,
                                            QMI_CAT_DECODED_ENVELOPE_SMS_TPDU_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->sms_tpdu),
                                            &parsed_data_ptr->sms_tpdu);
        break;

       case CATI_ENV_TYPE_IS_CDMA_SMS:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->is_cdma_sms_present,
                                            sizeof(parsed_data_ptr->is_cdma_sms),
                                            sizeof(parsed_data_ptr->is_cdma_sms),
                                            &parsed_data_ptr->is_cdma_sms);
        break;

       case CATI_ENV_TYPE_RAT:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->rat_present,
                                            sizeof(parsed_data_ptr->rat),
                                            sizeof(parsed_data_ptr->rat),
                                            &parsed_data_ptr->rat);
        break;

       case CATI_ENV_TYPE_CALL_TYPE:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->call_type_present,
                                            sizeof(parsed_data_ptr->call_type),
                                            sizeof(parsed_data_ptr->call_type),
                                            &parsed_data_ptr->call_type);
        break;

       case CATI_ENV_TYPE_TRANSACTION_ID:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->transaction_id_present,
                                            QMI_CAT_DECODED_ENVELOPE_TRANS_ID_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->transaction_id),
                                            &parsed_data_ptr->transaction_id);
        break;

       case CATI_ENV_TYPE_RP_ADDRESS:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->rp_address_present,
                                            QMI_CAT_DECODED_ENVELOPE_ADDRESS_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->rp_address),
                                            &parsed_data_ptr->rp_address);
        break;

       case CATI_ENV_TYPE_TP_ADDRESS:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->tp_address_present,
                                            QMI_CAT_DECODED_ENVELOPE_ADDRESS_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->tp_address),
                                            &parsed_data_ptr->tp_address);
        break;

       case CATI_ENV_TYPE_CAUSE:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->cause_present,
                                            QMI_CAT_DECODED_ENVELOPE_CAUSE_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->cause),
                                            &parsed_data_ptr->cause);
        break;

      case CATI_ENV_TYPE_IMS_REQUEST_URI:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->ims_uri_present,
                                            QMI_CAT_DECODED_ENVELOPE_IMS_REQUEST_URI_MIN_SIZE,
                                            sizeof(parsed_data_ptr->ims_uri),
                                            &parsed_data_ptr->ims_uri);        
        break;                                            

      case CATI_ENV_TYPE_INDICATION_TOKEN:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->ind_token_present,
                                            sizeof(parsed_data_ptr->ind_token),
                                            sizeof(parsed_data_ptr->ind_token),
                                            &parsed_data_ptr->ind_token);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that all mandatory TLV were received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->env_hdr_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_decoded_envelope */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_EVENT_CONFIRMATION()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_event_confirmation
(
  dsm_item_type                      ** sdu_in,
  qmi_cat_in_event_confirmation_type  * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_event_confirmation_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_EVENT_CONFIRMATION_USER_CONFIRMED_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->user_confirmed_present,
                                            sizeof(parsed_data_ptr->user_confirmed),
                                            sizeof(parsed_data_ptr->user_confirmed),
                                            &parsed_data_ptr->user_confirmed);
        break;

      case CATI_EVENT_CONFIRMATION_ICON_DISPLAYED_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->icon_display_present,
                                            sizeof(parsed_data_ptr->icon_display),
                                            sizeof(parsed_data_ptr->icon_display),
                                            &parsed_data_ptr->icon_display);
        break;

      case CATI_EVENT_CONFIRMATION_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            &parsed_data_ptr->slot);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on user confirmation */
    if (parsed_data_ptr->user_confirmed_present &&
        parsed_data_ptr->user_confirmed.user_confirmed != 0 &&
        parsed_data_ptr->user_confirmed.user_confirmed != 1)
    {
      result = QMI_ERR_INVALID_ARG;
    }
    /* Check on icon display */
    if (parsed_data_ptr->icon_display_present &&
        parsed_data_ptr->icon_display.icon_display != 0 &&
        parsed_data_ptr->icon_display.icon_display != 1)
    {
      result = QMI_ERR_INVALID_ARG;
    }
    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_event_confirmation */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SCWS_OPEN_CHANNEL_STATE()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_scws_open_channel_state
(
  dsm_item_type                           ** sdu_in,
  qmi_cat_in_scws_open_channel_state_type  * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_scws_open_channel_state_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_SCWS_CHANNEL_STATE_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->channel_state_present,
                                            sizeof(parsed_data_ptr->channel_state),
                                            sizeof(parsed_data_ptr->channel_state),
                                            (void *)&parsed_data_ptr->channel_state);
        break;

      case CATI_SCWS_CHANNEL_STATE_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            (void *)&parsed_data_ptr->slot);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that mandatory TLV is received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->channel_state_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on channel state */
    if (parsed_data_ptr->channel_state_present &&
        parsed_data_ptr->channel_state.ch_state != 0 &&
        parsed_data_ptr->channel_state.ch_state != 1 &&
        parsed_data_ptr->channel_state.ch_state != 2)
    {
      result = QMI_ERR_INVALID_ARG;
    }

    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_scws_open_channel_state */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SCWS_CLOSE_CHANNEL_STATE()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_scws_close_channel_state
(
  dsm_item_type                            ** sdu_in,
  qmi_cat_in_scws_close_channel_state_type  * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_scws_close_channel_state_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_SCWS_CHANNEL_STATE_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->channel_state_present,
                                            sizeof(parsed_data_ptr->channel_state),
                                            sizeof(parsed_data_ptr->channel_state),
                                            (void *)&parsed_data_ptr->channel_state);
        break;

      case CATI_SCWS_CHANNEL_STATE_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            (void *)&parsed_data_ptr->slot);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that mandatory TLV is received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->channel_state_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on channel state */
    if (parsed_data_ptr->channel_state_present &&
        parsed_data_ptr->channel_state.ch_state != 0 &&
        parsed_data_ptr->channel_state.ch_state != 1 &&
        parsed_data_ptr->channel_state.ch_state != 2)
    {
      result = QMI_ERR_INVALID_ARG;
    }

    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_scws_close_channel_state */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SCWS_CHANNEL_STATUS_STATE()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_scws_channel_status_state
(
  dsm_item_type                         ** sdu_in,
  qmi_cat_in_scws_channel_status_state_type   * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_scws_channel_status_state_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_SCWS_CHANNEL_STATE_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->channel_state_present,
                                            sizeof(parsed_data_ptr->channel_state),
                                            sizeof(parsed_data_ptr->channel_state),
                                            (void *)&parsed_data_ptr->channel_state);
        break;

      case CATI_SCWS_CHANNEL_STATE_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            (void *)&parsed_data_ptr->slot);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that mandatory TLV is received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->channel_state_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on channel state */
    if (parsed_data_ptr->channel_state_present &&
        parsed_data_ptr->channel_state.ch_state != 0 &&
        parsed_data_ptr->channel_state.ch_state != 1 &&
        parsed_data_ptr->channel_state.ch_state != 2)
    {
      result = QMI_ERR_INVALID_ARG;
    }

    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_scws_channel_status_state */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SCWS_SEND_DATA()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_scws_send_data
(
  dsm_item_type                      ** sdu_in,
  qmi_cat_in_scws_send_data_type      * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_scws_send_data_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_SCWS_SEND_DATA_CHANNEL_DATA_RESULT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->channel_data_result_present,
                                            sizeof(parsed_data_ptr->channel_data_result),
                                            sizeof(parsed_data_ptr->channel_data_result),
                                            (void *)&parsed_data_ptr->channel_data_result);
        break;

      case CATI_SCWS_SEND_DATA_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            (void *)&parsed_data_ptr->slot);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that mandatory TLV is received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->channel_data_result_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on channel data result */
    if (parsed_data_ptr->channel_data_result_present &&
        parsed_data_ptr->channel_data_result.result != 0 &&
        parsed_data_ptr->channel_data_result.result != 1)
    {
      result = QMI_ERR_INVALID_ARG;
    }

    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_scws_send_data */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SCWS_DATA_AVAILABLE()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_scws_data_available
(
  dsm_item_type                      ** sdu_in,
  qmi_cat_in_scws_data_available_type      * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_scws_data_available_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_SCWS_DATA_AVAILABLE_CHANNEL_DATA_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->channel_data_present,
                                            QMI_CAT_SCWS_CHANNEL_DATA_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->channel_data),
                                            (void *)&parsed_data_ptr->channel_data);
        break;

      case CATI_SCWS_DATA_AVAILABLE_REMAIN_DATA_LEN_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->remain_data_len_present,
                                            sizeof(parsed_data_ptr->remain_data_len),
                                            sizeof(parsed_data_ptr->remain_data_len),
                                            (void *)&parsed_data_ptr->remain_data_len);
        break;

      case CATI_SCWS_DATA_AVAILABLE_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            (void *)&parsed_data_ptr->slot);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that mandatory TLV is received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->channel_data_present ||
        !parsed_data_ptr->remain_data_len_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on channel data */
    if (parsed_data_ptr->channel_data_present &&
        parsed_data_ptr->channel_data.data_len >
        sizeof(parsed_data_ptr->channel_data.data))
    {
      result = QMI_ERR_MALFORMED_MSG;
    }

    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_scws_data_available */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_SET_CONFIGURATION()

  DESCRIPTION
    Parses the TLV content of QMI_UIM message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_set_configuration
(
  dsm_item_type                     ** sdu_in,
  qmi_cat_in_set_configuration_type   * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(qmi_cat_in_set_configuration_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_SET_CONFIGURATION_CONFIG_MODE_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->config_mode_present,
                                            sizeof(parsed_data_ptr->config_mode),
                                            sizeof(parsed_data_ptr->config_mode),
                                            &parsed_data_ptr->config_mode);
        break;

      case CATI_SET_CONFIGURATION_CUSTOM_TP_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->custom_tp_present,
                                            QMI_CAT_CUSTOM_TP_TLV_MIN_SIZE,
                                            sizeof(parsed_data_ptr->custom_tp),
                                            &parsed_data_ptr->custom_tp);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that at least one of the 2 TLV is received */
  if (result == QMI_ERR_NONE)
  {
    if (parsed_data_ptr->config_mode_present == FALSE)
    {
      result = QMI_ERR_MISSING_ARG;
    }

    /* Additional checks on custom_tp */
    if (parsed_data_ptr->custom_tp_present &&
        parsed_data_ptr->custom_tp.tp_len >
        sizeof(parsed_data_ptr->custom_tp.tp_bitmask))
    {
      result = QMI_ERR_MALFORMED_MSG;
    }
  }

  return result;
} /* qmi_cat_parse_request_set_configuration */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_GET_CACHED_PROACTIVE_CMD()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_get_cached_proactive_cmd
(
  dsm_item_type                             ** sdu_in,
  qmi_cat_in_get_cached_proactive_cmd_type   * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0, sizeof(*parsed_data_ptr));

  while (*sdu_in)
  {
    uint8     type = 0;
    uint16    len  = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_GET_CACHED_PROACTIVE_CMD_CMD_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in,
                                            len,
                                            &parsed_data_ptr->command_id_present,
                                            sizeof(parsed_data_ptr->command_id),
                                            sizeof(parsed_data_ptr->command_id),
                                            &parsed_data_ptr->command_id);
        break;

      case CATI_GET_CACHED_PROACTIVE_CMD_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in,
                                            len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            &parsed_data_ptr->slot);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check that mandatory TLV is received */
  if (result == QMI_ERR_NONE)
  {
    if (!parsed_data_ptr->command_id_present)
    {
      result = QMI_ERR_MISSING_ARG;
    }
  }

  /* Additional checks on data */
  if (result == QMI_ERR_NONE)
  {
    /* Check on slot */
    QMI_CAT_CHECK_SLOT_ID_TLV(parsed_data_ptr, result);
  }

  return result;
} /* qmi_cat_parse_request_get_cached_proactive_cmd */


/*===========================================================================
  FUNCTION  QMI_CATI_POPULATE_DECODED_ENV_CC_RESULT()

  DESCRIPTION
    Process the call control result

  PARAMETERS
   gstk_control_result_enum_type : CC result enumeration
   dsm_item_type **              : Pointer to DSM item pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_decoded_env_cc_result
(
  gstk_control_result_enum_type          cc_result,
  dsm_item_type                       ** response_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint8 call_control_result;
  }cc_result_tlv_data;

  switch (cc_result)
  {
    case GSTK_ALLOWED_NO_MOD:
      cc_result_tlv_data.call_control_result = 0x00;
      break;
    case GSTK_NOT_ALLOWED:
      cc_result_tlv_data.call_control_result = 0x01;
      break;
    case GSTK_ALLOWED_BUT_MOD:
      cc_result_tlv_data.call_control_result = 0x02;
      break;
    default:
      UIM_MSG_ERR_1("Unsupported CC result 0x%x", cc_result);
      return QMI_ERR_INTERNAL;
  }

  if (FALSE == qmi_svc_put_param_tlv(response_ptr, CATI_PRM_TYPE_CC_RESULT,
                                     sizeof(cc_result_tlv_data),
                                     (void *)&cc_result_tlv_data))
  {
    UIM_MSG_HIGH_0("Failed TLV put for CC result");
    return QMI_ERR_NO_MEMORY;
  }
  return QMI_ERR_NONE;
} /* qmi_cati_populate_decoded_env_cc_result */


/*===========================================================================
  FUNCTION  QMI_CATI_POPULATE_DECODED_ENV_CC_VOICE()

  DESCRIPTION
    Process the call control response (voice call type) from GSTK

  PARAMETERS
   gstk_cc_req_action_type : Pointer to call control request action message
   dsm_item_type **        : Pointer to DSM item pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_decoded_env_cc_voice
(
  const gstk_cc_req_action_type  * req_ptr,
  dsm_item_type                 ** response_ptr
)
{
  qmi_error_e_type   errval       = QMI_ERR_NONE;

  if (req_ptr->gstk_address_string.voice_info.address.length > 0)
  {
    errval = qmi_cati_address_tlv(&req_ptr->gstk_address_string.voice_info.address,
                                  response_ptr, CATI_PRM_TYPE_CC_ADDRESS);
  }
  if (errval != QMI_ERR_NONE)
  {
    return errval;
  }

  if (req_ptr->gstk_address_string.voice_info.subaddress.length > 0)
  {
    errval = qmi_cati_subaddress_tlv(&req_ptr->gstk_address_string.voice_info.subaddress,
                                     response_ptr, CATI_PRM_TYPE_CC_SUBADDRESS);
  }
  if (errval != QMI_ERR_NONE)
  {
    return errval;
  }

  if (req_ptr->gstk_address_string.voice_info.ccp1.length > 0)
  {
    errval = qmi_cati_ccp_tlv(&req_ptr->gstk_address_string.voice_info.ccp1,
                              response_ptr, CATI_PRM_TYPE_CC_CCP1);
  }
  if (errval != QMI_ERR_NONE)
  {
    return errval;
  }

  if (req_ptr->gstk_address_string.voice_info.ccp2.length > 0)
  {
    errval = qmi_cati_ccp_tlv(&req_ptr->gstk_address_string.voice_info.ccp2,
                              response_ptr, CATI_PRM_TYPE_CC_CCP2);
  }

  return errval;
} /* qmi_cati_populate_decoded_env_cc_voice */


/*===========================================================================
  FUNCTION  QMI_CATI_POPULATE_DECODED_ENV_USSD_STRING()

  DESCRIPTION
    Process response USSD strings

  PARAMETERS
   gstk_ussd_string_type  : Pointer to call control request action message
   dsm_item_type **        : Pointer to DSM item pointer
   byte                    : Type of text (USSD or alpha)
   uint8                   : Value of DCS
   boolean                 : Treatment of DCS coding

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_decoded_env_ussd_string
(
  const gstk_ussd_string_type    * ussd_string_ptr,
  dsm_item_type                 ** response_ptr,
  byte                             type,
  uint8                            dcs,
  boolean                          use_cbs_coding
)
{
  const uint8   * src_str_ptr  = NULL;
  uint8           src_str_len  = 0;

  src_str_len = ussd_string_ptr->length;
  src_str_ptr = ussd_string_ptr->text;

  if ((src_str_len > 0) && (src_str_ptr != NULL))
  {
    PACKED struct PACKED_POST
    {
      uint8 dcs;
      uint8 length;
      uint8 text[CAT_TLV_TEXT_STRING_MAX];
    } ussd_string_tlv_data;

    qmi_cati_convert_gstk_dcs_to_qmi(dcs,
                                     &ussd_string_tlv_data.dcs,
                                     use_cbs_coding);

    ussd_string_tlv_data.length = src_str_len;

    /* GSTK adds an extra byte at the end with NULL termination for legacy clients.
       Needs to be removed if present */
    if (src_str_ptr[src_str_len - 1] == 0x0)
    {
      ussd_string_tlv_data.length--;
    }

    /* Max size check unnecessary as length is uint8 */
    (void)memscpy(ussd_string_tlv_data.text,
                  sizeof(ussd_string_tlv_data.text),
                  src_str_ptr,
                  ussd_string_tlv_data.length);

    if (FALSE == qmi_svc_put_param_tlv(response_ptr, type,
                                       sizeof(uint8) + sizeof(uint8) + ussd_string_tlv_data.length,
                                       (void *)&ussd_string_tlv_data))
    {
      UIM_MSG_ERR_1("Failed ussd string TLV put for type 0x%x", type);
      return QMI_ERR_NO_MEMORY;
    }
  }
  return QMI_ERR_NONE;
} /* qmi_cati_populate_decoded_env_ussd_string */


/*===========================================================================
  FUNCTION  QMI_CATI_POPULATE_DECODED_ENV_STRING()

  DESCRIPTION
    Process the esponse strings

  PARAMETERS
   gstk_string_type        : Pointer to call control request action message
   dsm_item_type **        : Pointer to DSM item pointer
   byte                    : Type of text (USSD or alpha)
   uint8                   : Value of DCS
   boolean                 : Treatment of DCS coding

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_decoded_env_string
(
  const gstk_string_type         * string_ptr,
  dsm_item_type                 ** response_ptr,
  byte                             type,
  uint8                            dcs,
  boolean                          use_cbs_coding
)
{
  const uint8   * src_str_ptr  = NULL;
  uint8           src_str_len  = 0;

  src_str_len = string_ptr->length;
  src_str_ptr = string_ptr->text;

  if ((src_str_len > 0) && (src_str_ptr != NULL))
  {
    PACKED struct PACKED_POST
    {
      uint8 dcs;
      uint8 length;
      uint8 text[CAT_TLV_TEXT_STRING_MAX];
    } string_tlv_data;

    qmi_cati_convert_gstk_dcs_to_qmi(dcs,
                                     &string_tlv_data.dcs,
                                     use_cbs_coding);

    string_tlv_data.length = src_str_len;

    /* GSTK adds an extra byte at the end with NULL termination for legacy clients.
       Needs to be removed if present */
    if (src_str_ptr[src_str_len - 1] == 0x0)
    {
      string_tlv_data.length--;
    }

    /* Max size check unnecessary as length is uint8 */
    (void)memscpy(string_tlv_data.text,
                  sizeof(string_tlv_data.text),
                  src_str_ptr,
                  string_tlv_data.length);

    if (FALSE == qmi_svc_put_param_tlv(response_ptr, type,
                                       sizeof(uint8) + sizeof(uint8) + string_tlv_data.length,
                                       (void *)&string_tlv_data))
    {
      UIM_MSG_ERR_1("Failed string TLV put for type 0x%x", type);
      return QMI_ERR_NO_MEMORY;
    }
  }
  return QMI_ERR_NONE;
} /* qmi_cati_populate_decoded_env_cc_string */


/*===========================================================================
  FUNCTION  QMI_CATI_POPULATE_DECODED_ENV_CC_ACT()

  DESCRIPTION
    Process the call control response activation params (PDP context or
    EPS PDN connect type)

  PARAMETERS
   gstk_cc_req_action_type : Pointer to call control request action message
   dsm_item_type **        : Pointer to DSM item pointer
   byte                    : Type of activation (PDP or EPS PDN)

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_decoded_env_cc_act
(
  const gstk_cc_req_action_type  * req_ptr,
  dsm_item_type                 ** response_ptr,
  byte                             type
)
{
  const uint8   * src_act_ptr  = NULL;
  uint8           src_act_len  = 0;

  if (type == CATI_PRM_TYPE_CC_PDP_ACT)
  {
    src_act_len = (uint8) req_ptr->gstk_address_string.pdp_context_param.length;
    src_act_ptr = req_ptr->gstk_address_string.pdp_context_param.pdp_act_param;
  }
  else
  {
    src_act_len = (uint8) req_ptr->gstk_address_string.eps_conn_act.length;
    src_act_ptr = req_ptr->gstk_address_string.eps_conn_act.eps_act_param;
  }

  if (src_act_len > 0)
  {
    PACKED struct PACKED_POST
    {
      uint8 length;
      uint8 act_param[CATI_TLV_ACT_PARAM_MAX];
    } act_tlv_data;

    act_tlv_data.length = src_act_len;

    (void)memscpy(act_tlv_data.act_param,
                  sizeof(act_tlv_data.act_param),
                  src_act_ptr,
                  act_tlv_data.length);

    if (FALSE == qmi_svc_put_param_tlv(response_ptr, type,
                                       sizeof(uint8) + act_tlv_data.length,
                                       (void *)&act_tlv_data))
    {
      UIM_MSG_ERR_1("qmi_cati_populate_decoded_env_cc_act:Failed TLV put for type 0x%x",
                    type);
      return QMI_ERR_NO_MEMORY;
    }
  }
  return QMI_ERR_NONE;
} /* qmi_cati_populate_decoded_env_cc_act */


/*===========================================================================
  FUNCTION  QMI_CATI_POPULATE_DECODED_ENV_CC_IMS_URI()

  DESCRIPTION
    Process the call control response (IMS URI type) from GSTK
    
  PARAMETERS
   gstk_cc_req_action_type : Pointer to call control request action message
   dsm_item_type **        : Pointer to DSM item pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_decoded_env_cc_ims_uri
(
  const gstk_cc_req_action_type  * req_ptr,
  dsm_item_type                 ** response_ptr
)
{
  if (req_ptr->gstk_address_string.ims_call.length > CATI_TLV_IMS_URI_MAX)
  {
    return QMI_ERR_NO_MEMORY;
  }

  if (req_ptr->gstk_address_string.ims_call.length > 0)
  {
    PACKED struct PACKED_POST
    {
      uint8 length;
      uint8 ims_request_uri[CATI_TLV_IMS_URI_MAX];
    } ims_call_tlv_data;

    ims_call_tlv_data.length = (uint8) req_ptr->gstk_address_string.ims_call.length;

    memset(ims_call_tlv_data.ims_request_uri, 0, sizeof(ims_call_tlv_data.ims_request_uri));

    (void)memscpy(ims_call_tlv_data.ims_request_uri,
                  sizeof(ims_call_tlv_data.ims_request_uri),
                  req_ptr->gstk_address_string.ims_call.ims_uri,
                  ims_call_tlv_data.length);

    if (FALSE == qmi_svc_put_param_tlv(response_ptr, CATI_PRM_TYPE_CC_IMS_REQUEST_URI,
                                       sizeof(uint8) + ims_call_tlv_data.length,
                                       (void *)&ims_call_tlv_data))
    {
      UIM_MSG_ERR_0("qmi_cati_populate_decoded_env_cc_ims_uri : Failed TLV");
      return QMI_ERR_NO_MEMORY;
    }
  }
  return QMI_ERR_NONE;
} /* qmi_cati_populate_decoded_env_cc_ims_uri */


/*===========================================================================
  FUNCTION  QMI_CATI_POPULATE_DECODED_ENV_CC_BC_REPEAT()

  DESCRIPTION
    Process the call control BC repeat indicator field if applicable

  PARAMETERS
   gstk_cc_req_action_type : Pointer to call control request action message
   dsm_item_type **        : Pointer to DSM item pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_decoded_env_cc_bc_repeat
(
  const gstk_cc_req_action_type  * req_ptr,
  dsm_item_type                 ** response_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint8 ind;
  }bc_rep_ind_tlv;

  switch (req_ptr->bc_repeat_ind)
  {
    case GSTK_ALTERNATE_MODE:
      bc_rep_ind_tlv.ind = 0x01;
      break;
    case GSTK_SEQUENTIAL_MODE:
      bc_rep_ind_tlv.ind = 0x03;
      break;
    default:
      return QMI_ERR_NONE;
  }

  if (FALSE == qmi_svc_put_param_tlv(response_ptr, CATI_PRM_TYPE_CC_BC_REPEAT_IND,
                                     sizeof(bc_rep_ind_tlv),
                                     (void *)&bc_rep_ind_tlv))
  {
    UIM_MSG_ERR_0("Failed TLV put for CC BC ind");
    return QMI_ERR_NO_MEMORY;
  }
  return QMI_ERR_NONE;
} /* qmi_cati_populate_decoded_env_cc_bc_repeat */


/*===========================================================================
  FUNCTION  QMI_CATI_POPULATE_DECODED_ENV_SMS_PP_UICC_ACK()

  DESCRIPTION
    Process SMS PP UICC ack

  PARAMETERS
   gstk_sms_tpdu : sms_tpdu
   dsm_item_type **        : Pointer to DSM item pointer
   byte                    : Type of text (USSD or alpha)
   uint8                   : Value of DCS
   boolean                 : Treatment of DCS coding

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cati_populate_decoded_env_sms_pp_uicc_ack
(
  const gstk_sms_pp_download_ack_type  * sms_pp_ack_ptr,
  dsm_item_type                       ** response_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint8 length;
    uint8 ack[CATI_TLV_SMS_PP_UICC_ACK_MAX_LENGTH];
  } sms_pp_ack_data;

  ASSERT(sms_pp_ack_ptr != NULL);
  memset(&sms_pp_ack_data, 0, sizeof(sms_pp_ack_data));

  sms_pp_ack_data.length = sms_pp_ack_ptr->length;

  if(sms_pp_ack_ptr->length < CATI_TLV_SMS_PP_UICC_ACK_MAX_LENGTH)
  {
    if (sms_pp_ack_ptr->length != 0)
    {
      (void)memscpy(sms_pp_ack_data.ack,
                    sizeof(sms_pp_ack_data.ack),
                    sms_pp_ack_ptr->ack,
                    sms_pp_ack_ptr->length);
    }
  }
  else
  {
    UIM_MSG_ERR_1("Failed sms_pp_ack TLV with invalid len: 0x%x",
                  sms_pp_ack_ptr->length);
    return QMI_ERR_INTERNAL;
  }

  if (FALSE == qmi_svc_put_param_tlv(response_ptr,
                                     CATI_PRM_TYPE_SMS_PP_UICC_ACK,
                                     sizeof(uint8) + sms_pp_ack_data.length,
                                     (void *)&sms_pp_ack_data))
  {
    UIM_MSG_ERR_0("Failed  TLV put for CATI_PRM_TYPE_SMS_PP_UICC_ACK");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cati_populate_decoded_env_sms_pp_uicc_ack */


/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_DECODED_ENV_CALL_CONTROL()

  DESCRIPTION
    Process Call Control decoded envelope

  PARAMETERS
   gstk_cc_rsp_type        : cc_rsp_ptr
   dsm_item_type **        : Pointer to DSM item pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cat_response_decoded_env_call_control
(
  const gstk_cc_rsp_type         * cc_rsp_ptr,
  dsm_item_type                 ** response_ptr
)
{
  const gstk_cc_req_action_type  * cc_action_from_card_ptr = NULL;
  qmi_error_e_type                 errval                  = QMI_ERR_NONE;

  errval = qmi_cati_populate_decoded_env_cc_result(cc_rsp_ptr->call_ctrl_result,
                                                   response_ptr);
  if (errval != QMI_ERR_NONE)
  {
    return errval;
  }

  cc_action_from_card_ptr = &cc_rsp_ptr->cc_req_action;

  switch (cc_action_from_card_ptr->call_type)
  {
    case GSTK_VOICE:
      errval = qmi_cati_populate_decoded_env_cc_voice(cc_action_from_card_ptr,
                                                      response_ptr);
      if (errval != QMI_ERR_NONE)
      {
        return errval;
      }
      break;

    case GSTK_SS:
      errval = qmi_cati_address_tlv(&cc_action_from_card_ptr->gstk_address_string.ss_string,
                                    response_ptr, CATI_PRM_TYPE_CC_ADDRESS);
      if (errval != QMI_ERR_NONE)
      {
        return errval;
      }
      break;

     case GSTK_USSD:
      errval = qmi_cati_populate_decoded_env_ussd_string(
                        &cc_action_from_card_ptr->gstk_address_string.ussd_string,
                        response_ptr,
                        CATI_PRM_TYPE_CC_USSD_STR,
                        cc_action_from_card_ptr->gstk_address_string.ussd_string.dcs,
                        TRUE);

      if (errval != QMI_ERR_NONE)
      {
        return errval;
      }
      break;

    case GSTK_PDP_CONTEXT_ACT:
      errval = qmi_cati_populate_decoded_env_cc_act(cc_action_from_card_ptr,
                                                    response_ptr,
                                                    CATI_PRM_TYPE_CC_PDP_ACT);
      if (errval != QMI_ERR_NONE)
      {
        return errval;
      }
      break;

    case GSTK_EPS_PDN_CONN_ACT:
      errval = qmi_cati_populate_decoded_env_cc_act(cc_action_from_card_ptr,
                                                    response_ptr,
                                                    CATI_PRM_TYPE_CC_EPS_PDN_ACT);
      if (errval != QMI_ERR_NONE)
      {
        return errval;
      }
      break;

    case GSTK_IMS_CALL:
      errval = qmi_cati_populate_decoded_env_cc_ims_uri(cc_action_from_card_ptr,
                                                                response_ptr);
      if (errval != QMI_ERR_NONE)
      {
        return errval;
      }
      break;

    default:
      break;
  }
  errval = qmi_cati_populate_decoded_env_string(&cc_action_from_card_ptr->alpha,
                                                   response_ptr,
                                                   CATI_PRM_TYPE_CC_ALPHA,
                                                   cc_action_from_card_ptr->alpha.dcs,
                                                   FALSE);
  if (errval != QMI_ERR_NONE)
  {
    return errval;
  }

  errval = qmi_cati_populate_decoded_env_cc_bc_repeat(cc_action_from_card_ptr,
                                                  response_ptr);

  return errval;
} /* qmi_cat_response_decoded_env_call_control */


/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_DECODED_ENV_MO_SMS_CONTROL()

  DESCRIPTION
    Process MO SMS Call Control decoded envelope

  PARAMETERS
   gstk_cc_rsp_type        : cc_rsp_ptr
   dsm_item_type **        : Pointer to DSM item pointer

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cat_response_decoded_env_mo_sms_control
(
  const gstk_mo_sms_ctrl_rsp_type       * cc_sms_rsp_ptr,
  dsm_item_type                        ** response_ptr
)
{
  qmi_error_e_type                 errval                  = QMI_ERR_NONE;

  errval = qmi_cati_populate_decoded_env_cc_result(cc_sms_rsp_ptr->mo_sms_ctrl_result,
                                                   response_ptr);
  if (errval != QMI_ERR_NONE)
  {
    return errval;
  }

  if(cc_sms_rsp_ptr->rp_dest_address.length > 0)
  {
    errval = qmi_cati_address_tlv(&cc_sms_rsp_ptr->rp_dest_address,
                                  response_ptr, CATI_PRM_TYPE_RP_ADDRESS);
    if (errval != QMI_ERR_NONE)
    {
      return errval;
    }
  }

  if(cc_sms_rsp_ptr->tp_dest_address.length > 0)
  {
    errval = qmi_cati_address_tlv(&cc_sms_rsp_ptr->tp_dest_address,
                                  response_ptr, CATI_PRM_TYPE_TP_ADDRESS);
    if (errval != QMI_ERR_NONE)
    {
      return errval;
    }
  }

  errval = qmi_cati_populate_decoded_env_string(&cc_sms_rsp_ptr->alpha,
                                                   response_ptr,
                                                   CATI_PRM_TYPE_CC_ALPHA,
                                                   cc_sms_rsp_ptr->alpha.dcs,
                                                   FALSE);

  return errval;
} /* qmi_cat_response_decoded_env_mo_sms_control */


/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_DECODED_ENV_SMS_PP_DOWNLOAD()

  DESCRIPTION
    Process Call Control decoded envelope

  PARAMETERS
   gstk_sms_pp_download_rsp_type : sms_pp_dl_rsp_ptr
   dsm_item_type **              : response_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static qmi_error_e_type qmi_cat_response_decoded_env_sms_pp_download
(
  const gstk_sms_pp_download_rsp_type  * sms_pp_dl_rsp_ptr,
  dsm_item_type                       ** response_ptr
)
{
  return qmi_cati_populate_decoded_env_sms_pp_uicc_ack(&sms_pp_dl_rsp_ptr->ack,
                                                       response_ptr);

} /* qmi_cat_response_decoded_env_sms_pp_download */


/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_DECODED_ENVELOPE()

  DESCRIPTION
    Process the decoded call control evnvelope response

  PARAMETERS
   gstk_cmd_from_card_type      : Pointer to command from card
   dsm_item_type **             : Pointer to DSM item pointer
   qmi_cati_ind_token_info_type : indication token

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_decoded_envelope
(
  const gstk_cmd_from_card_type         * decoded_env_rsp_ptr,
  dsm_item_type                        ** response_ptr,
  qmi_cati_ind_token_info_type            ind_token
)
{
  qmi_error_e_type                 errval                  = QMI_ERR_NONE;

  switch(decoded_env_rsp_ptr->hdr_cmd.command_id)
  {
    case GSTK_CC_IND_RSP:
      errval = qmi_cat_response_decoded_env_call_control(&decoded_env_rsp_ptr->cmd.cc_rsp, response_ptr);
      break;

    case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
      errval = qmi_cat_response_decoded_env_sms_pp_download(&decoded_env_rsp_ptr->cmd.sms_pp_download_rsp,
                                                             response_ptr);
      break;

    case GSTK_MO_SMS_CTRL_IND_RSP:
      errval = qmi_cat_response_decoded_env_mo_sms_control(&decoded_env_rsp_ptr->cmd.mo_sms_cc_rsp,
                                                            response_ptr);
      break;

    default:
      break;
  }

  if (ind_token.is_valid &&
      QMI_ERR_NONE != qmi_cat_response_indication_token(response_ptr,
                                                        ind_token.token,
                                                        CATI_CMD_VAL_DECODED_ENVELOPE_CMD,
                                                        CATI_IND_MSG_TYPE))
  {
    UIM_MSG_MED_0("Unable to include indication token");
    return QMI_ERR_NO_MEMORY;
  }
  return errval;
} /* qmi_cat_response_decoded_envelope */


/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_RAW_ENVELOPE()

  DESCRIPTION
    Process the raw evnvelope response

  PARAMETERS
   gstk_cmd_from_card_type      : Pointer to command from card
   dsm_item_type **             : Pointer to DSM item pointer
   qmi_cati_ind_token_info_type : indication token

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_raw_envelope
(
  const gstk_cmd_from_card_type         * env_rsp_ptr,
  dsm_item_type                        ** response_ptr,
  qmi_cati_ind_token_info_type            ind_token
)
{
  PACKED struct PACKED_POST
  {
    uint8 sw1;
    uint8 sw2;
    uint8 data_len;
    uint8 env_resp_data[CATI_TLV_RAW_ENV_DATA_MAX];
  }raw_env_rsp_tlv;

  ASSERT(env_rsp_ptr);

  if (ind_token.is_valid &&
      QMI_ERR_NONE != qmi_cat_response_indication_token(response_ptr,
                                                        ind_token.token,
                                                        CATI_CMD_VAL_SEND_ENVELOPE_CMD,
                                                        CATI_IND_MSG_TYPE))
  {
    UIM_MSG_MED_0("Unable to include indication token");
    return QMI_ERR_NO_MEMORY;
  }

  /* Raw envelope response is currently supported only for call control, 
     MO SMS control and SMS-PP */
  if (env_rsp_ptr->hdr_cmd.command_id != GSTK_CC_IND_RSP &&
      env_rsp_ptr->hdr_cmd.command_id != GSTK_SMS_PP_DOWNLOAD_IND_RSP &&
      env_rsp_ptr->hdr_cmd.command_id != GSTK_MO_SMS_CTRL_IND_RSP)
  {
    return QMI_ERR_NONE;
  }

  raw_env_rsp_tlv.sw1 = env_rsp_ptr->cmd.raw_envelope_rsp.sw1;
  raw_env_rsp_tlv.sw2 = env_rsp_ptr->cmd.raw_envelope_rsp.sw2;
  raw_env_rsp_tlv.data_len = (uint8)env_rsp_ptr->cmd.raw_envelope_rsp.length;

  /* Data */
  if (env_rsp_ptr->cmd.raw_envelope_rsp.length > CATI_TLV_RAW_ENV_DATA_MAX)
  {
    UIM_MSG_HIGH_0("qmi_cat_response_envelope CATI_TLV_RAW_ENV_DATA_MAX");
    return QMI_ERR_INTERNAL;
  }

  if (env_rsp_ptr->cmd.raw_envelope_rsp.length > 0 &&
      env_rsp_ptr->cmd.raw_envelope_rsp.rsp_data_ptr != NULL)
  {

    UIM_MSG_HIGH_1(" length 0x%x", env_rsp_ptr->cmd.raw_envelope_rsp.length);
    (void)memscpy((void*)raw_env_rsp_tlv.env_resp_data,
                  sizeof(raw_env_rsp_tlv.env_resp_data),
                  env_rsp_ptr->cmd.raw_envelope_rsp.rsp_data_ptr,
                  env_rsp_ptr->cmd.raw_envelope_rsp.length);
  }

  if (FALSE == qmi_svc_put_param_tlv(response_ptr, CATI_PRM_TYPE_RAW_ENV_RSP,
                                     sizeof(uint8)*3 + raw_env_rsp_tlv.data_len,
                                     (void *)&raw_env_rsp_tlv))
  {
    UIM_MSG_ERR_0("Failed TLV put for raw envelope rsp");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cat_response_raw_envelope */


/*===========================================================================
  FUNCTION QMI_CAT_PARSE_REQUEST_GET_TERMINAL_PROFILE()

  DESCRIPTION
    Parses the TLV content of QMI_CAT message

  PARAMETERS
    sdu_in      : input sdu
    parsed_data : output with parsed values

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_parse_request_get_terminal_profile
(
  dsm_item_type                         ** sdu_in,
  qmi_cat_in_get_terminal_profile_type   * parsed_data_ptr
)
{
  qmi_error_e_type  result = QMI_ERR_NONE;

  ASSERT(sdu_in);
  ASSERT(parsed_data_ptr);

  /* Initialize the output */
  memset(parsed_data_ptr, 0x00, sizeof(qmi_cat_in_get_terminal_profile_type));

  while (*sdu_in)
  {
    uint8     type   = 0;
    uint16    len    = 0;

    if( !qmi_svc_get_tl( sdu_in, &type, &len ) )
    {
      continue;
    }

    switch(type)
    {
      case CATI_GET_TERMINAL_PROFILE_SLOT_TLV:
        result = qmi_cat_extract_packed_tlv(sdu_in, len,
                                            &parsed_data_ptr->slot_present,
                                            sizeof(parsed_data_ptr->slot),
                                            sizeof(parsed_data_ptr->slot),
                                            &parsed_data_ptr->slot);
        break;

      default:
        QMI_CAT_SKIP_TLV(sdu_in, len, result);
        break;
    }

    if (result != QMI_ERR_NONE)
    {
      UIM_MSG_ERR_2("Error 0x%x parsing TLV type 0x%x", result, type);
      break;
    }
  }

  /* Check on slot */
  if (parsed_data_ptr->slot_present     &&
      parsed_data_ptr->slot.slot != 0x1 &&
      parsed_data_ptr->slot.slot != 0x2)
  {
    result = QMI_ERR_INVALID_ARG;
  }

  return result;
} /* qmi_cat_parse_request_get_terminal_profile */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_INDICATION_TOKEN()

  DESCRIPTION
    Populates the tlv for indication token

  PARAMETERS
    response                 : output sdu
    token                    : indication token for response
    cmd_id                   : cmd id for the response
    msg_type                 : e.g. response msg or indication msg

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_indication_token
(
  dsm_item_type                 ** response,
  uint32                           token,
  qmi_cati_cmd_val_e_type          cmd_id,
  qmi_cati_message_e_type          msg_type
)
{
  struct
  {
    uint32 token;
  } tlv_data;

  uint8    prm_type = 0;

  tlv_data.token = token;

  switch (cmd_id)
  {
     case CATI_CMD_VAL_SEND_TR:
       prm_type = (msg_type == CATI_IND_MSG_TYPE ?
                            CATI_RAW_TR_IND_TOKEN_TLV : CATI_RAW_TR_RSP_TOKEN_TLV);
       break;

     case CATI_CMD_VAL_SEND_DECODED_TR:
       prm_type = (msg_type == CATI_IND_MSG_TYPE ?
                            CATI_DECODED_TR_IND_TOKEN_TLV : CATI_DECODED_TR_RSP_TOKEN_TLV);
       break;

     case CATI_CMD_VAL_SEND_ENVELOPE_CMD:
       prm_type = (msg_type == CATI_IND_MSG_TYPE ?
                            CATI_PRM_TYPE_RAW_ENV_IND_TOKEN : CATI_PRM_TYPE_RAW_ENV_RSP_TOKEN);
       break;

     case CATI_CMD_VAL_DECODED_ENVELOPE_CMD:
       prm_type = (msg_type == CATI_IND_MSG_TYPE ?
                            CATI_PRM_TYPE_DECODED_ENV_IND_TOKEN : CATI_PRM_TYPE_DECODED_ENV_RSP_TOKEN);
       break;
	   
     default:
       break;
  }

  if (prm_type == CATI_CMD_VAL_MAX)
  {
    /* indication token should not be included other than TR and envelope cmd,
	   return error none to skip including the TLV */
    return QMI_ERR_NONE;
  }

  if (FALSE == qmi_svc_put_param_tlv(response,
                                     prm_type,
                                     sizeof(tlv_data),
                                     (void *)&tlv_data ))
  {
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cat_response_indication_token */


/*===========================================================================
  FUNCTION  QMI_CAT_RESPONSE_SEND_TR()

  DESCRIPTION
    Process the TR response

  PARAMETERS
   dsm_item_type **               : Pointer to DSM item pointer
   gstk_term_rsp_sw1_sw2_rpt_type : Pointer to status words from card
   qmi_cati_ind_token_info_type   : indication token

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_send_tr
(
  dsm_item_type                        ** response_ptr,
  const gstk_term_rsp_sw1_sw2_rpt_type  * msg_ptr,
  qmi_cati_ind_token_info_type            ind_token
)
{
  qmi_error_e_type                 errval = QMI_ERR_NONE;
  PACKED struct PACKED_POST
  {
    uint8 sw1;
    uint8 sw2;
    uint8 data_len;
    uint8 tr_response_data[CATI_TLV_RAW_TR_DATA_MAX];
  } tr_response_data;

  ASSERT(msg_ptr);

  tr_response_data.sw1      = msg_ptr->sw1;
  tr_response_data.sw2      = msg_ptr->sw2;
  tr_response_data.data_len = 0;

  if( FALSE == qmi_svc_put_param_tlv(response_ptr,
                                     CATI_RAW_TR_RSP_TR_RESPONSE_TLV,
                                     sizeof(uint8)*3 + tr_response_data.data_len,
                                     (void *)&tr_response_data) )
  {
    UIM_MSG_MED_0("Unable to send TR response data");
    return QMI_ERR_NO_MEMORY;
  }

  if (ind_token.is_valid &&
      QMI_ERR_NONE != qmi_cat_response_indication_token(response_ptr,
                                                        ind_token.token,
                                                        CATI_CMD_VAL_SEND_TR,
                                                        CATI_IND_MSG_TYPE))
  {
    UIM_MSG_MED_0("Unable to include indication token");
    return QMI_ERR_NO_MEMORY;
  }

  return errval;
} /* qmi_cat_response_send_tr */


/*===========================================================================
  FUNCTION QMI_CAT_RESPONSE_CACHED_PROACTIVE_CMD()

  DESCRIPTION
    Populates the tlvs from the raw proactive command

  PARAMETERS
    tlv_id          : tag to be used to pack response
    ind             : output sdu
    cmd_ref_id      : command reference id
    raw_data_len    : length of the raw proactive command
    raw_data_ptr    : value of the raw proactive command

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_cat_response_cached_proactive_cmd
(
  byte                             tlv_id,
  dsm_item_type                 ** ind,
  uint32                           cmd_ref_id,
  uint16                           raw_data_len,
  const uint8                    * raw_data_ptr
)
{
  PACKED struct PACKED_POST
  {
    uint32  cmd_ref_id;
    uint16  raw_data_len;
    uint8   raw_data[CAT_RAW_MAX_DATA_LEN];
  } tlv_data;

  /* Check length */
  if (raw_data_len > sizeof(tlv_data.raw_data))
  {
    return QMI_ERR_NO_MEMORY;
  }

  memset(&tlv_data, 0, sizeof(tlv_data));
  tlv_data.cmd_ref_id = cmd_ref_id;
  if (raw_data_len && raw_data_ptr)
  {
    tlv_data.raw_data_len = raw_data_len;
    (void)memscpy(tlv_data.raw_data,
                  sizeof(tlv_data.raw_data),
                  raw_data_ptr,
                  raw_data_len);
  }

  if( FALSE == qmi_svc_put_param_tlv(ind,
                                     tlv_id,
                                     (sizeof(tlv_data.cmd_ref_id) + sizeof(tlv_data.raw_data_len)+ tlv_data.raw_data_len),
                                     (void *)&tlv_data) )
  {
    UIM_MSG_HIGH_0("Unable to generate TLV for cached raw proactive cmd!");
    return QMI_ERR_NO_MEMORY;
  }

  return QMI_ERR_NONE;
} /* qmi_cat_response_cached_proactive_cmd */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
