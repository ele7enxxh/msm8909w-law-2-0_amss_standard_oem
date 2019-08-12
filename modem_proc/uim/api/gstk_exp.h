#ifndef GSTK_EXP_H
#define GSTK_EXP_H

/**
  @file gstk_exp.h
  @brief This is the GSTK export header file for the GSTK client. It contains
  command prototype and function protocol that the client can use to communicate
  with the GSTK.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The gstk_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      gstk_mainpage.dox file or contact Tech Pubs.
=============================================================================*/
/*=============================================================================
   Copyright (c) 2003-2014 QUALCOMM Technologies, Inc(QTI).
   All rights reserved.
   QUALCOMM Technologies Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/gstk_exp.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/12/15   dy      Enabling Toolkit-initiated GBA TP bit (byte 25 bit 5)
11/07/14   bd      Ignore keep_ch_open flag in UICC server mode case
09/25/14   gm      Added support to send failure indication for OTASP
09/10/14   gm      Added support for SMS_PP data download retry
09/04/14   sw      Donot reset NAS and CM related variables for REFRESH_NAA_INIT
08/05/14   lxu     Support feature FEATURE_MODE_SWITCH_STK_SWITCH
07/21/14   sw      Display alpha related changes
07/11/14   shr     Add Support for NV Refresh
06/20/14   gm      Dynamically disable TP related to LTE
06/18/14   bd      Changes for including BIP channel ID in CLOSE CH CNF event
05/28/14   gm      Support of CSG Rel-11 feature
05/06/14   gs      Additional comments for gstk_client_init_ext
04/29/14   dy      Added ext browser HTML support bit to TP
03/17/14   gm      Added support for IMS communication control
03/04/14   shr     Do not wait for 1x to become available on non-SVLTE targets
                   w.r.t. card activation via OTASP
03/03/14   shr     ESTK BIP clean-up and enhancements
02/20/14   dy      Verify TR response for raw TRs
02/20/14   gm      Clear globals after REFRESH
01/30/14   vr      SIM initiated call control handling
01/09/14   kb      Cleanup global variables while handling STOP signal
12/16/13   bd      Added support for new CM SS event handling
12/05/13   hn      Support DNS query and Channel reuse
11/28/13   gm      Remove "Remove "GSTK_TERMINAL_PROFILE_EXT_CMD"
11/13/13   shr     Added support for BIP over WLAN
10/25/13   vr      Corrected GSTK_HCI_CONNECTIVITY_EVT_SUPPORT value to 0x1920
09/13/13   gm      Support for Recovery and hot-swap
08/06/13   vr      Centralized Call Control changes
06/20/13   gm      Passing proactive_cmd_pending through TAL
07/23/13   sw      Support for DS callback to obtain RAT information.
07/01/13   hh      Added support for passing RAT through MO SMS CTRL API
06/24/13   xz      Support IMS Registration event download
06/10/13   vr      updated the TP bit of GSTK_ACTIVATE_SUPPORT to 0x1E10
06/07/13   yt      Added intermediate status words to TAL UIM report structure
05/17/13   hn      Support for Alpha for Refresh
05/16/13   hh      Fix KW errors
04/02/13   av      Move UIM tasks to group4 of RCINIT
02/28/13   bd      Support for dynamic reg, dereg and call related evt download
02/22/13   yt      Support for slot 3
02/11/13   bd      Support for (e)HRPD access tech
01/31/13   gm      Changes for SMS-PP Security improvement
01/29/13   hn      Added BIP status events
11/13/12   bd      Support for Closed Subscriber Group feature
09/27/12   tkl     Added Browser Status evt bitmask for TP
09/10/12   sg      Handle SCWS alpha
08/10/12   av      Removed RPC support along with RPC style comments
04/24/12   dd      Fix the MEID bitmask in TP byte 23
02/23/12   bd      Enhancements for location information in CC
11/30/11   nb      GSDI/GSTK Cleanup
09/14/11   sg      Class L and M support
07/19/11   js      1)Edited/added Doxygen markup and comments.
                   2)Add support to report LS in TAL
04/22/11   dd      Add SCWS APIs
03/10/11   dd      Move GSTK_PROVIDE_DATE_TIME_CNF
03/03/11   dd      Send PLI Date/time to clients
02/02/11   js      Fix for async processing of TAL commands
10/20/10   dd      Move the APIs to gstk_exp.h from gstk_exp_v.h
08/16/10   js      Update for gstk_tal_client_registration()
07/29/10   js      Changes to support envelope response on TAL.
07/20/10   js      RPC update for gstk_tal_cmd_details_type, Add tag to the
                   raw and decoded hdr for Set Up Call
07/14/10   js      Toolkit Arbitration Layer (TAL) APIs update
07/09/10   sg      Add support for Toolkit Arbitration Layer (TAL)
07/07/10   bd      Added API gstk_is_service_enabled_ext
06/09/10   xz      Add support of EPS Call Control
05/24/10   nb      Merged changes for DS DS
05/11/10   xz      Add support of network rejection event
03/23/10   xz      Add GSTK_APP_CSIM enum
03/17/10   xz      Add support of saving LTE BIP
03/16/10   sg      Location information envelope command
03/11/10   xz      Export gstk_slot_send_envelope_cc_command() and
                   gstk_address_to_ascii()
02/19/10   bd      Changes for caching proactive commands
02/19/10   xz      Fix compilation error
02/15/10   nb      Moved Dual Slot Changes
02/09/10   xz      IMSI_M provision enhancement
12/17/09   sg      Added GSTK_GET_CURR_ACCESS_TECH_CMD for ESTK BIP
11/09/09   dd      Add APITRIM metacomments
11/03/09   dd      Adding gstk_send_envelope_mo_sms_ctrl_command
09/30/09   xz      Change GSTK_MAX_CCP_LEN to 16
09/02/09   dd      Adding gstk_is_mo_sms_ctrl_required and gstk_is_ussd_object_supported
08/25/09   sg      Changed argument type of mask in gstk_client_reg to uint64
07/29/09   xz      Add gstk_util_parse_ccp()
07/09/09   bd      Adding declaration for gstk_task function
04/15/09   xz      Support client id clean up of modem restart
04/07/09   gg      Added define to set bit 3 of 17th byte in Treminal Profile
03/18/09   xz      Fix RPC compilation error
03/05/09   sun     Added comments
03/02/09   sun     Removed Private file header inclusion
03/03/09   gg      moved GSTK_REG_PROVIDE_BATTERY_STATE_EVT from
                   GSTK_REG_ALL_EVT to GSTK_REG_PROACTIVE_CMD_EVT
03/02/09   xz      Provide SW1 and SW2 support for raw APDU responses
02/26/09   gg      Introduced Class B event registration
12/17/08   nb      Common Modem Interface
11/26/08   nb      Removed Featurization Flags for Common Modem Interface.
10/13/08   yb      Removed Lint Errors
10/10/08   yb      Fixed documentation
09/14/08   sk      Added fix for lint errors
09/08/08   sk      Added support for FEATURE_ESTK
09/06/08   sk      Fixed meta comments for RPC
08/21/08   sk      Added support for enhanced GSTK registration that includes
                   support for multiple clients to perform terminal profile
                   download and support for clients to register for a subset of
                   proactive command in GSTK or RAW format.
08/08/08   xz      Added support of raw proactive command, raw terminal response
                   and raw envelope command
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
11/07/07   sk      Added gstk enum for card type
09/25/07   sk      Changed tp dl bytes to allow alpha and display of extn text
                   support indication to come from the app
08/29/07   sk      Added additional info enums
03/01/07   sk      Fixed RPC metacomments
01/11/07   tml     Added rec_num for icon ID
11/22/06   sk      Updated RPC Metacomments
10/08/06   sk      Added support for unknown unsupported proactive commands.
09/22/06   jk      Fix RPC Metacomments bug
09/20/06   jk      Update RPC Metacomments
08/18/06   sk      Made gstk_nw_search_mode_enum_type spec compliant
07/27/06   jk      Updated Metacomments for 7200
06/27/06   jk      Added RPC support for function gstk_client_reg_ext
06/06/06   sk      Added support for network search mod
06/02/06   tml     GPRS Call Control support
06/01/06   sk      Fixed warnings
06/01/06   sk      Added support for channel status and data available
                   registrations.
05/08/06   jk      RPC Metadata comments for dual-proc
05/03/06   tml     Lint
04/06/06   sk      Updated Terminal Profile values.
04/04/06   sk      Added support for immediate digit response and variable
                   timeout object for get inkey cmd.
03/06/06   sk      Added support for duration object in display text cmd.
                   Added support for battery status.
09/01/05   sun     Fixed Enum Values
07/13/05   sk      Added Access Technology support
05/11/05   sst     Lint fixes
04/06/05   tml     Exposed utility functions for alpha GSM default unicode
                   conversion
01/10/05   sk      Added original dcs from SIM in ussd string type and add
                   number of channel in channel status response
12/06/04   sk      Added macro for GSTK_MAX_GPRS_CHANNELS
                   Changed gstk_get_ch_status_rsp_extra_param_type
                   struct type to support maximum GPRS channels.
12/04/04   tml     move dcs enum decode and unpack function to exp.h
11/23/04  sk/tml   Added SAT/USAT data support
09/20/04   tml     Added 8 bit to 7 bit decoding function
08/19/04   tml     Added cell broadcast support
07/22/04   tml     Added gstk_additional_info_ptr_type such that
                   greater than GSTK_MAX_ADDITIONAL_INFO can be passed down
                   to the card
06/14/04   tml     Added gstk_end_proactive_req_type
06/09/04   tml     Added Terminal profile version support, added
                   packed/unpacked enum for get input, added color coding
                   scheme for icon
05/11/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/16/04   tml     linted
03/01/04   tml     Merged from tip: added provide local lang info support
09/12/03   tml     Added ascii to gstk_address_type and vice versa conversaton
                   Changed mode_type to mode_enum_type and
                   slot_id_type_to slot_id_enum_type
09/12/03   tml     Increased the GSTK_MAX_ADDRESS_LEN
09/04/03   tml     Dual slot support
09/03/03   tml     Added Slot ID
05/16/03   tml     Linted
04/21/03   tml     Added Sim busy for envelope response enum type.  Added
                   supports to handle UTK
03/03/03   tml     Changed byte to uint8, update enum names
02/26/03   tml     Added user data in the hdr command type
                   Changed library function to take function pointer instead of
                   pointer to function
02/25/03   tml     changed ERROR to ERROR_TONE to avoid conflict with atcop
02/12/03   tml     Added gstk_map_proactive_req_to_cnf
                   Removed gstk_find_expected_proactive_cmd_rsp
02/07/03 tml/jar   Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/* apitrim -include */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/** @addtogroup genDefs
  @{
*/
/*
   Capabilities
  ------------
  (other internal capabilities also defined in gstk.h)
*/
#define GSTK_MAX_PROACTIVE_COMMAND             8
  /**< Maximum number of commands that can be queued to the GSTK for proactive
       or envelope commands. */
#define GSTK_MAX_ADDRESS_LEN                   200
  /**< Maximum length for the address TLV. */
#define GSTK_MAX_SUBADDR_LEN                   20
  /**< Call Manager maximum call party subaddress is 20 characters. */ /* CALLED_PARTY_SUBADDRESS_LENGTH */

#define GSTK_IPV4_ADDRESS_LEN                  4
  /**< Length for the IPV4 address TLV. */
#define GSTK_IPV6_ADDRESS_LEN                  16
  /**< Length for the IPV6 address TLV. */

#define GSTK_MAX_CCP_LEN                       16
  /**< Maximum length of the capability configuration parameters TLV. */ /* GSDI_CCP_LEN */
#define GSTK_MAX_PROVISIONING_FILE_REF         20
  /**< Maximum number of provisioning file references. */
#define GSTK_MAX_ADDITIONAL_INFO               10
  /**< Maximum size of the additional information TLV. */

/* Duration units (For redial timeout & tones. See also 11.14) */
#define GSTK_DURATION_MINUTES                  0x00
  /**< Redial timeout and tone duration in minutes. */
#define GSTK_DURATION_SECONDS                  0x01
  /**< Redial timeout and tone duration in seconds. */
#define GSTK_DURATION_TENTHS_OF_SECONDS        0x02
  /**< Redial timeout and tone duration in tenths of a second. */

#define GSTK_LANG_CODE_LEN                     0x02
  /**< Length of the language code information. */

#define GSTK_MAX_GPRS_CHANNELS                 0x07
  /**< Maximum number of GPRS channels. */

#define GSTK_MAX_PDP_ACT_PARAM_SIZE            0xFF
  /**< Maximum size of the PDP activation parameter. */

#define GSTK_EXP_RPC_MAX_OUTPUT_DATA 500
  /**< Output length of the GSTK decode functions. Increase this value as
       larger capacity is required. */
#define GSTK_MAX_DATA_SIZE                       0x100
  /**< Maximum length of a command inclusive of all TLVs. */
#define GSTK_GET_INKEY_USER_INPUT_DATA_LEN       0x04
  /**< Length of the input data from the user for the Get Inkey event. */
#define GSTK_MAX_PLMN_LEN                        0x03
  /**< Maximum public land mobile network length. */
#define GSTK_MAX_LAC_LEN                         0x02
  /**< Maximum location area code length. */
#define GSTK_MAX_TP_DATA_SIZE                    0x200
  /**< Maximum size of the terminal profile data. */
#define GSTK_MAX_CELL_ID_LEN                     0x04
  /**< Maximum cell ID size. */
#define GSTK_MAX_APP_ID_LEN                      0x10
  /**< Maximum App ID size. */
#define GSTK_MAX_IMS_CALL_PARAM_SIZE             0xF6
  /**< Maximum size of the IMS call parameter. */
#define GSTK_MAX_OTASP_RETRY_CNT                 0x3
  /**< Maximum number of OTASP activation retry. */  
/** @} */ /* end_addtogroup gen_defs */

/**
  @addtogroup regBitmask
  @{
*/
#define GSTK_REG_UNSPECIFIED_CATEGORY_CMD       0xFFFFFFFF
  /**< Unspecified category for the command.  */
#define GSTK_REG_CATEGORY_0_CMD                 0x00000000
  /**< General category of toolkit functionality commands. */
#define GSTK_REG_CATEGORY_C_CMD                 0x00000001
  /**< Browser-related proactive and envelope commands. */
#define GSTK_REG_CATEGORY_D_CMD                 0x00000002
  /**<  Category D commands; currently none. */
#define GSTK_REG_CATEGORY_E_CMD                 0x00000003
  /**< BIP (GPRS)-related proactive and envelope commands. */
#define GSTK_REG_CATEGORY_B_CMD                 0x00000004
  /**<  Category B commands; currently none. */
#define GSTK_REG_CATEGORY_L_CMD                 0x00000005
  /**<  Category L commands; currently Activate proactive command. */
#define GSTK_REG_CATEGORY_M_CMD                 0x00000006
  /**<  Category M commands; HCI connectivity evt download. */

/** @name Category 0
  @{
*/
/* Class "0" */
#define GSTK_REG_DISPLAY_TEXT_EVT               0x00000001
  /**< Registers for the Display Text event. */
#define GSTK_REG_GET_INKEY_EVT                  0x00000002
  /**< Registers for the Get Inkey event. */
#define GSTK_REG_GET_INPUT_EVT                  0x00000004
  /**< Registers for the Get Input event. */
#define GSTK_REG_MORE_TIME_EVT                  0x00000008
  /**< Registers for the More Time event. */

#define GSTK_REG_PLAY_TONE_EVT                  0x00000010
  /**< Registers for the Play Tone event. */
#define GSTK_REG_POLL_INTERVAL_EVT              0x00000020
  /**< Registers for the Poll Interval event. */
#define GSTK_REG_PROVIDE_LOCAL_INFO_EVT         0x00000040
  /**< Registers for the Provide Local Information event. */
#define GSTK_REG_REFRESH_EVT                    0x00000080
  /**< Registers for the Refresh event. */

#define GSTK_REG_SELECT_ITEM_EVT                0x00000100
  /**< Registers for the Select Item event. */
#define GSTK_REG_SEND_SMS_EVT                   0x00000200
  /**< Registers for the Send SMS event. */
#define GSTK_REG_SEND_SS_EVT                    0x00000400
  /**< Registers for the Send SS event. */
#define GSTK_REG_SEND_USSD_EVT                  0x00000800
  /**< Registers for the Send USSD event.*/

#define GSTK_REG_SETUP_CALL_EVT                 0x00001000
  /**< Registers for the Set Up Call event. */
#define GSTK_REG_SETUP_EVENT_LIST_EVT           0x00002000
  /**< Registers for the Set Up Event List event. */
#define GSTK_REG_SETUP_MENU_EVT                 0x00004000
  /**< Registers for the Set Up Menu event. */
#define GSTK_REG_SETUP_IDLE_TEXT_EVT            0x00008000
  /**< Registers for the Set Up Idle Text event. */

#define GSTK_REG_POLLING_OFF_EVT                0x00010000
  /**< Registers for the Polling Off event. */
#define GSTK_REG_PROVIDE_LANG_INFO_EVT          0x00020000
  /**< Registers for the Provide Language Information event. */
#define GSTK_REG_LANG_NOTIFY_EVT                0x00040000
  /**< Registers for the Language Notification event. */
#define GSTK_REG_TIMER_MANAGEMENT_EVT           0x00080000
  /**< Registers for the Timer Management event. */

#define GSTK_REG_SEND_DTMF_EVT                  0x00100000
  /**< Registers for the Send DTMF event. */
#define GSTK_REG_SETUP_EVT_USER_ACT_EVT         0x00200000
  /**< Registers for the Set Up User Action event.  */
#define GSTK_REG_SETUP_EVT_IDLE_MENU_EVT        0x00400000
  /**< Registers for the Set Up Idle Menu event. */
#define GSTK_REG_SETUP_EVT_LANG_SEL_EVT         0x00800000
  /**< Registers for the Set Up Language Selection event. */


#define GSTK_REG_PROFILE_DL_RSP_EVT             0x01000000
  /**< Registers for Profile Download Response events. */
#define GSTK_REG_PROVIDE_BATTERY_STATE_EVT      0x02000000
  /**< Registers for Provide Battery Status request events. */
#define GSTK_REG_EVT_DOWNLOAD_RSP_EVT           0x04000000
  /**< Registers for envelope response events. */
#define GSTK_REG_MENU_SEL_RSP_EVT               0x08000000
  /**< Registers for Menu Selection response events. */
#define GSTK_REG_PROVIDE_DATE_TIME_EVT          0x10000000
  /**< Registers for Provide Local Information (Date/Time) events. */
/** @} */ /* end_name Category 0 */


/** @name Category C
  @{
*/
/* Class C commands */
#define GSTK_REG_LAUNCH_BROWSER_EVT             0x20000000
  /**< Registers for Launch Browser events. */
#define GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT     0x40000000
  /**< Registers for Browser Termination events. */
/** @} */ /* end_name Category C */

/** @name Category E
  @{
*/
/* Class E commands */
#define GSTK_REG_DATA_EVT                       0x80000000
  /**< Registers for Data Events. */
#define GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT   0x0000000100000000LL
  /**< Registers for Data Available events. */
#define GSTK_REG_SCWS_EVT                       0x0000000200000000LL
  /**< Registers for Smart Card Web Server events. */

/* Register for Channel Status Event */
#define GSTK_REG_SETUP_EVT_CH_STATUS_EVT        0x0000000400000000LL
  /**< Registers for Channel Status events. */
/** @} */ /* end_addtogroup categoryE */

/** @name Category B
  @{
*/
/* Class B command */
#define GSTK_REG_RUN_AT_CMDS_EVT                0x0000000800000000LL
  /**< Registers for run at commands */
/** @} */ /* end_name Category B */

/** @name Category L
  @{
*/
/* Class L commands */
#define GSTK_REG_ACTIVATE_EVT                   0x0000001000000000LL
  /**< Registers for Activate Command */
/** @} */ /* end_name Category L */

/** @name Category M
  @{
*/
/* Class M commands */
#define GSTK_REG_SETUP_EVT_HCI_CONN_EVT         0x0000002000000000LL
  /**< Registers for HCI Connectivity event */
/** @} */ /* end_name Category M */

/** @cond
*/
/** Registers for all proactive command events. */
#define GSTK_REG_PROACTIVE_CMD_EVT             \
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
  GSTK_REG_LAUNCH_BROWSER_EVT            |     \
  GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT    |     \
  GSTK_REG_DATA_EVT                      |     \
  GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT  |     \
  GSTK_REG_SETUP_EVT_CH_STATUS_EVT       |     \
  GSTK_REG_RUN_AT_CMDS_EVT               |     \
  GSTK_REG_PROVIDE_DATE_TIME_EVT         |     \
  GSTK_REG_ACTIVATE_EVT                  |     \
  GSTK_REG_SETUP_EVT_HCI_CONN_EVT)

/** Registers for all events including proactive command events. */
#define GSTK_REG_ALL_EVT                       \
  (GSTK_REG_PROACTIVE_CMD_EVT            |     \
  GSTK_REG_PROFILE_DL_RSP_EVT            |     \
  GSTK_REG_EVT_DOWNLOAD_RSP_EVT          |     \
  GSTK_REG_MENU_SEL_RSP_EVT)
/** @endcond */
/** @} */ /* end_addtogroup regBitmask */

/** @addtogroup commandInfo
  @{
*/
#define GSTK_CMD_STK_REFRESH                0x01
  /**< Refresh proactive command tag. */
#define GSTK_CMD_STK_MORE_TIME              0x02
  /**< More Time proactive command tag. */
#define GSTK_CMD_STK_POLL_INTERVAL          0x03
  /**< Poll Interval proactive command tag. */
#define GSTK_CMD_STK_POLLING_OFF            0x04
  /**< Polling Off proactive command tag. */
#define GSTK_CMD_STK_SET_UP_EVENT_LIST      0x05
  /**< Set Up Event List proactive command tag. */
#define GSTK_CMD_STK_SET_UP_CALL            0x10
  /**< Set Up Call proactive command tag. */
#define GSTK_CMD_STK_SEND_SS                0x11
  /**< Send SS proactive command tag. */
#define GSTK_CMD_STK_USSD                   0x12
  /**< Send USSD proactive command tag. */
#define GSTK_CMD_STK_SEND_SHORT_MESSAGE     0x13
  /**< Send Short Message proactive command tag. */
#define GSTK_CMD_STK_SEND_DTMF              0x14
  /**< Send DTMF proactive command tag. */
#define GSTK_CMD_STK_LAUNCH_BROWSER         0x15
  /**< Launch Browser proactive command tag. */
#define GSTK_CMD_STK_PLAY_TONE              0x20
  /**< Play Tone proactive command tag. */
#define GSTK_CMD_STK_DISPLAY_TEXT           0x21
  /**< Display Text proactive command tag. */
#define GSTK_CMD_STK_GET_INKEY              0x22
  /**< Get Inkey proactive command tag. */
#define GSTK_CMD_STK_GET_INPUT              0x23
  /**< Get Input proactive command tag. */
#define GSTK_CMD_STK_SELECT_ITEM            0x24
  /**< Select Item proactive command tag. */
#define GSTK_CMD_STK_SET_UP_MENU            0x25
  /**< Set Up Menu proactive command tag. */
#define GSTK_CMD_STK_PROVIDE_LOCAL_INFO     0x26
  /**< Provide Local Information proactive command tag. */
#define GSTK_CMD_STK_TIMER_MANAGEMENT       0x27
  /**< Timer Management proactive command tag. */
#define GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT  0x28
  /**< Set Up Idle Mode Text proactive command tag. */
#define GSTK_CMD_STK_LANG_NOTIFICATION      0x35
  /**< Language Notification proactive command tag. */
#define GSTK_CMD_STK_END_OF_PROACTIVE_SES   0x81
  /**< End of Proactive Session tag. */
#define GSTK_CMD_STK_OPEN_CHANNEL           0x40
  /**< Open Channel proactive command tag. */
#define GSTK_CMD_STK_CLOSE_CHANNEL          0x41
  /**< Close Channel proactive command tag. */
#define GSTK_CMD_STK_RECEIVE_DATA           0x42
  /**< Receive Data proactive command tag. */
#define GSTK_CMD_STK_SEND_DATA              0x43
  /**< Send Data proactive command tag. */
#define GSTK_CMD_STK_GET_CHANNEL_STATUS     0x44
  /**< Get Channel Status proactive command tag. */
#define GSTK_CMD_STK_ACTIVATE               0x70
  /**< Activate proactive command tag. */

/* not supported for PHASE 1 development */
#define GSTK_CMD_STK_PERFORM_CARD_APDU      0x30
  /**< Currently not supported. */
#define GSTK_CMD_STK_POWER_ON_CARD          0x31
  /**< Currently not supported. */
#define GSTK_CMD_STK_POWER_OFF_CARD         0x32
  /**< Currently not supported. */
#define GSTK_CMD_STK_GET_READER_STATUS      0x33
  /**< Currently not supported. */
#define GSTK_CMD_STK_RUN_AT_COMMAND         0x34
  /**< Currently not supported. */

/** @} */ /* end_addtogroup commandInfo */

/** @addtogroup group1
  @{
*/
/* ---------------- FIRST GROUP ------------------- */
/** @name First Byte - Upper Layer Information
  @{ */
#define GSTK_SMS_PP_DL_SUPPORT                        0x0102
  /**< SMS point-to-point download support. */ /* value = (1,0) */
#define GSTK_MENU_SEL_SUPPORT                         0x0108
  /**< Menu selection support. */ /* value = (1,0) */
/* For SAT */
#define GSTK_USSD_DO_IN_CC_SUPPORT                    0x0140
  /**< USSD string data object supported. */ /* value = (1,0) */
/** @} */ /* end_name First Byte - Upper Layer Information */

/** @name Second Byte - Upper Layer Information
  @{ */
#define GSTK_CC_SUPPORT                               0x0202
  /**< Call control support. */ /* value = (1,0) */
#define GSTK_MO_SMS_CONTROL_SUPPORT                   0x0208
  /**< MO SMS control support. */ /* value = (1,0) */
#define GSTK_SUPPORT_FOR_ALPHA_IN_CC                  0x0210
  /**< Support for alpha in call control. */  /* value = (1,0) */
#define GSTK_UCS2_ENTRY_SUPPORT                       0x0220
  /**< UCS-2 entry support. */ /* value = (1,0) */
#define GSTK_UCS2_DISPLAY_SUPPORT                     0x0240
  /**< UCS-2 display support. */ /* value = (1,0) */
/* For USAT */
#define GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8        0x0280
  /**< Display support. */ /* value = (1,0) */
/* For SAT */
#define GSTK_DISPLAY_OF_EXTN_TEXT                     0x0280
  /**< Display of extension support. */ /* value = (1,0) */
/** @} */ /* end_name Second Byte - Upper Layer Information */

/** @name Third Byte - Upper Layer Information
  @{ */
#define GSTK_DISPLAY_TEXT_SUPPORT                     0x0301
  /**< Display text support for SAT. */ /* value = (1,0) */
#define GSTK_GET_INKEY_SUPPORT                        0x0302
  /**< Get inkey support. */ /* value = (1,0) */
#define GSTK_GET_INPUT_SUPPORT                        0x0304
  /**< Get input support. */ /* value = (1,0) */
#define GSTK_PLAY_TONE_SUPPORT                        0x0310
  /**< Play tone support. */ /* value = (1,0) */
/** @} */ /* end_name Third Byte - Upper Layer Information */

/** @name Fourth Byte - Upper Layer Information
  @{ */
#define GSTK_SELECT_ITEM_SUPPORT                      0x0401
  /**< Select item support. */ /* value = (1,0) */
#define GSTK_SEND_SHORT_MSG_SUPPORT                   0x0402
  /**< Send short message support. */ /* value = (1,0) */
#define GSTK_SEND_SS_SUPPORT                          0x0404
  /**< Send SS support. */ /* value = (1,0) */
#define GSTK_SEND_USSD_SUPPORT                        0x0408
  /**< Send USSD support. */ /* value = (1,0) */
#define GSTK_SET_UP_CALL_SUPPORT                      0x0410
  /**< Set up call support. */ /* value = (1,0) */
#define GSTK_SET_UP_MENU_SUPPORT                      0x0420
  /**< Set up menu support. */ /* value = (1,0) */
/** @} */ /* end_name Fourth Byte - Upper Layer Information */

/** @name Fifth Byte - Upper Layer Information
  @{ */
#define GSTK_USER_ACT_EVT_SUPPORT                     0x0520
  /**< User activity event support. */ /* value = (1,0) */
#define GSTK_IDLE_SCRN_AVAIL_EVT_SUPPORT              0x0540
  /**< Idle screen event support. */ /* value = (1,0) */
/** @} */ /* end_name Fifth Byte - Upper Layer Information */

/** @name Sixth Byte - Upper Layer Information
  @{ */
#define GSTK_LANG_SEL_EVT_SUPPORT                     0x0601
  /**< Language selection support. */ /* value = (1,0) */
#define GSTK_BROWSER_TERM_EVT_SUPPORT                 0x0602
  /**< Browser Termination event support. */ /* value = (1,0) */

#define GSTK_DATA_AVAIL_EVT_SUPPORT                   0x0604
  /**< Data available event support. */ /* value = (1,0) */  /* default = 0 */
#define GSTK_CH_STATUS_EVT_SUPPORT                    0x0608
  /**< Channel status event support. */ /* value = (1,0) */  /* default = 0 */
/** @} */ /* end_name Sixth Byte - Upper Layer Information */

/** @name Eighth Byte - Upper Layer Information
  @{ */
#define GSTK_SET_UP_IDLE_MODE_TEXT_SUPPORT            0x0810
  /**< Set up Idle mode text support. */ /* value = (1,0) */
#define GSTK_USAT_SETUP_CALL_SUPPORT_BYTE_8_BIT_7     0x0840
  /**< Set up call support. */ /* value = (1,0) */
#define GSTK_SECOND_ALPHA_IN_SETUP_CALL_SUPPORT       0x0840
  /**< Second alpha in set up call support. */ /* value = (1,0) */
#define GSTK_SECOND_CAP_CONFIG_PARAM_SUPPORT          0x0880
  /**< Second capability configuration parameter support. */ /* value = (1,0) */
/** @} */ /* end_name Eighth Byte - Upper Layer Information */

/** @name Ninth Byte - Upper Layer Information
  @{ */
#define GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1      0x0901
  /**< Display text support for USAT. */ /* value = (1,0) */
#define GSTK_SUSTAINED_DISP_TEXT_SUPPORT              0x0901
  /**< Sustained display text support. */ /* value = (1,0) */
#define GSTK_SEND_DTMF_SUPPORT                        0x0902
  /**< Send DTMF support. */ /* value = (1,0) */
#define GSTK_PLI_NMR_BCCH                             0x0904
  /**< Provide local information (NMR) support. */ /* value = (1,0) */
#define GSTK_PROVIDE_LOCAL_INFO_LANG_SUPPORT          0x0908
  /**< Provide local information (Language) support. */ /* value = (1,0) */
#define GSTK_LANGUAGE_NOTIFICATION_SUPPORT            0x0920
  /**< Language notification support. */ /* value = (1,0) */
#define GSTK_LAUNCH_BROWSER_SUPPORT                   0x0940
  /**< Launch browser support. */ /* value = (1,0) */
/** @} */ /* end_name Ninth Byte - Upper Layer Information */

/** @name Tenth Byte - Upper Layer Information
  @{ */
#define GSTK_SK_SUPPORT_IN_SEL_ITEM                   0x0A01
  /**< Soft key support in the Select Item event. */ /* value = (1,0) */
#define GSTK_SK_SUPPORT_IN_SET_UP_MENU                0x0A02
  /**< Soft key support in the Set Up Menu event. */ /* value = (1,0) */
/** @} */ /* end_name Tenth Byte - Upper Layer Information */

/** @name 11th Byte - Upper Layer Information
  @{ */
#define GSTK_MAX_SK_SUPPORT                           0x0BFF
  /**< Maximum number of soft key support. */ /* value = 1 byte of info */
/** @} */ /* end_name 11th Byte - Upper Layer Information */

/** @name 12th Byte - Upper Layer Information
  @{ */
#define GSTK_OPEN_CH_SUPPORT                          0x0C01
  /**< Open channel support. */ /* value = (1,0) */
#define GSTK_CLOSE_CH_SUPPORT                         0x0C02
  /**< Close channel support. */ /* value = (1,0) */
#define GSTK_RECEIVE_DATA_SUPPORT                     0x0C04
  /**< Receive data support */ /* value = (1,0) */
#define GSTK_SEND_DATA_SUPPORT                        0x0C08
  /**< Send data support. */ /* value = (1,0) */
#define GSTK_GET_CH_STATUS_SUPPORT                    0x0C10
  /**< Get channel status support. */ /* value = (1,0) */
/** @} */ /* end_name 12th Byte - Upper Layer Information */

/** @name 13th Byte - Upper Layer Information
  @{ */
#define GSTK_CSD_SUPPORT                              0x0D01
  /**< Circuit-switch data support. */ /* value = (1,0) */
#define GSTK_GPRS_SUPPORT                             0x0D02
  /**< General packet radio services support. */ /* value = (1,0) */
/** @} */ /* end_name 13th Byte - Upper Layer Information */

/** @name 14th Byte - Upper Layer Information
  @{ */
#define GSTK_NUM_CHAR_SUPPORT_DOWN_ME                 0x0E1F
  /**< Five bits of information. The number of characters supported without
       scrolling. */
#define GSTK_SCREEN_SIZE_SUPPORT                      0x0E80
  /**< Screen size support. */ /* value = (1,0); if user can change the
       number of chararacters across/down the display or if the ME can
       dynamically change the number of characters supported on the display,
       then value = 1. */
/** @} */ /* end_name 14th Byte - Upper Layer Information */

/** @name 15th Byte - Upper Layer Information
  @{ */
#define GSTK_NUM_CHAR_SUPPORT_ACROSS_ME               0x0F7F
  /**< Seven bits of information. The number of chararacters
       supported without scrolling. */
#define GSTK_VARIABLE_FONT_SIZE_SUPPORT               0x0F80
  /**< Variable font size support */ /* value = (1,0) */
/** @} */ /* end_name 15th Byte - Upper Layer Information */

/** @name 16th Byte - Upper Layer Information
  @{ */
#define GSTK_DISPLAY_RESIZED_SUPPORT                  0x1001
  /**< Display resized support. */ /* value = (1,0) */
#define GSTK_TEXT_WRAPPING_SUPPORT                    0x1002
  /**< Text wrapping support. */ /* value = (1,0) */
#define GSTK_TEXT_SCROLLING_SUPPORT                   0x1004
  /**< Text scrolling support. */ /* value = (1,0) */
#define GSTK_WIDTH_REDUCE_IN_MENU_SUPPORT             0x10E0
  /**< Three bits of information comprise the number of chararacters across the
       display without scrolling (set with the Display Text command), minus the
       number of characters across the display without scrolling (set with the
       Select Item command). */
/** @} */ /* end_name 16th Byte - Upper Layer Information */

/** @name 17th Byte - Upper Layer Information
  @{ */
#define GSTK_TCP_SUPPORT                              0x1101
  /**< TCP support. */ /* value = (1,0) */
#define GSTK_UDP_SUPPORT                              0x1102
  /**< UDP support. */ /* value = (1,0) */
#define GSTK_TCP_SERVER_MODE_SUPPORT                  0x1104
  /**< TCP Server mode support. */ /* value = (1,0) */
#define GSTK_USAT_EUTRAN_CLASS_E_SUPPORT_BYTE_17_BIT_7  0x1140
  /**< Class E support for EUTRAN */ /* value = {1,0} */
#define GSTK_USAT_HDPA_CLASS_E_SUPPORT_BYTE_17_BIT_8    0x1180
  /**< Class E support for HDPA */ /* value = {1,0} */
/** @} */ /* end_name 17th Byte - Upper Layer Information */

/** @name 18th Byte - Upper Layer Information
  @{ */
#define GSTK_DISPLAY_TXT_VARIABLE_TIMEOUT_SUPPORT     0x1201
  /**< Display text (variable timeout) support. */
#define GSTK_INKEY_HELP_FOR_VARIABLE_TIMEOUT_AND_IMMEDIATE_DIGIT_RSP     0x1202
  /**< Get inkey help is supported while waiting for an immediate response or a
       variable timeout. */
#define GSTK_INKEY_VARIABLE_TIMEOUT_SUPPORT           0x1208
  /**< Get inkey variable timeout support. */
/** @} */ /* end_name 18th Byte - Upper Layer Information */

/** @name 19th Byte - Upper Layer Information
  @{ */
#define GSTK_PROTOCOL_VERSION                         0x130F
  /**< Protocol version support. */ /* 4 bits of protocol version info */
/** @} */ /* end_name 19th Byte - Upper Layer Information */

/** @name 20th Byte - Upper Layer Information
  @{ */
#define GSTK_CDMA_SEND_SMS_SUPPORT                    0x1401
  /**< CDMA send short message support. */
#define GSTK_CDMA_SMS_PP_SUPPORT                      0x1402
  /**< CDMA SMS PP download support. */
/** @} */ /* end_name 20th Byte - Upper Layer Information */

/** @name 21st Byte - Upper Layer Information
  @{ */
#define GSTK_EXT_LAUNCH_BROWSER_WML_SUPPORT           0x1501
  /**< Extended Launch Browser WML support. */
#define GSTK_EXT_LAUNCH_BROWSER_XHTML_SUPPORT         0x1502
  /**< Extended Launch Browser XHTML support. */
#define GSTK_EXT_LAUNCH_BROWSER_HTML_SUPPORT          0x1504
  /**< Extended Launch Browser HTML support. */
#define GSTK_EXT_LAUNCH_BROWSER_CHTML_SUPPORT         0x1508
  /**< Extended Launch Browser CHTML support. */
/** @} */ /* end_name 21st Byte - Upper Layer Information */

/** @name 22nd Byte - Upper Layer Information
  @{ */
#define GSTK_USAT_UTRAN_PS_EXT_PARAM_CLASS_E_SUPPORT_BYTE_22_BIT_1  0x1601
  /**< Support for UTRAN PS with extra parameters */ /* value = {1,0} */
#define GSTK_PROVIDE_LOCAL_INFO_BATTERY_STATE_SUPPORT               0x1602
  /**< Provide local information (battery state) support. */
#define GSTK_TOOLKIT_INITIATED_GBA_SUPPORT                          0x1610
  /**< Provide Toolkit Initiated GBA support */
/** @} */ /* end_name 22nd Byte - Upper Layer Information */

/** @name 23rd Byte - Upper Layer Information
  @{ */
#define GSTK_ALPHA_REFRESH_SUPPORT                    0x1708
  /**< Alpha Identifier in Refresh support.               */
/** @} */ /* end_name 23rd Byte - Upper Layer Information */

/** @name 30th Byte - Upper Layer Information
  @{ */
#define GSTK_ACTIVATE_SUPPORT                         0x1E10
  /**< ACtivate Proactive command support. */
/** @} */ /* end_name 30th Byte - Upper Layer Information */
/** @} */ /* end_addtogroup group1 */

/** @addtogroup group2
  @{
*/
/* ------------------ SECOND GROUP -------------------------- */

/** @name First Byte - Predefined API Support Values
  @{ */
#define GSTK_PROFILE_DL_SUPPORT                       0x0101
  /**< Profile download support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_CELL_BROADCAST_DATA_DL_SUPPORT           0x0104
  /**< Cell broadcast data download support. Supported values: 1 and 0 (Default). */ /* value = (1,0) */  /* default = 0 */
#define GSTK_TIMER_EXPIRATION_SUPPORT                 0x0120
  /**< Timer expiration support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
/** @} */ /* end_name First Byte - Predefined API Support Values */

/** @name Third Byte - Predefined API Support Values
  @{ */
#define GSTK_MORE_TIME_SUPPORT                        0x0308
  /**< More time support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_POLL_INTERVAL_SUPPORT                    0x0320
  /**< Poll interval support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_POLLING_OFF_SUPPORT                      0x0340
  /**< Polling off support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_REFRESH_SUPPORT                          0x0380
  /**< Refresh support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
/** @} */ /* end_name Third Byte - Predefined API Support Values */

/** @name Fourth Byte - Predefined API Support Values
  @{ */
#define GSTK_PROVIDE_LOCAL_INFO_LOCATION_INFO_SUPPORT 0x0440
  /**< Provide local information (location information) support. Supported
       values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_PROVIDE_LOCAL_INFO_NMR_SUPPORT           0x0480
  /**< Provide local information (NMR) support. Supported values: 1 (Default)
       and 0. */ /* value = (1,0) */  /* default = 1 */
/** @} */ /* end_name Fourth Byte - Predefined API Support Values */

/** @name Fifth Byte - Predefined API Support Values
  @{ */
#define GSTK_SET_UP_EVT_LIST_SUPPORT                  0x0501
  /**< Setup event list support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_MT_CALL_EVT_SUPPORT                      0x0502
  /**< MT call event support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_CALL_CONNECTED_EVT_SUPPORT               0x0504
  /**< Call connected event support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_CALL_DISCONNECTED_EVT_SUPPORT            0x0508
  /**< Call disconnected event support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_LOCATION_STATUS_EVT_SUPPORT              0x0510
  /**< Location status event support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_CARD_READER_ST_EVT_SUPPORT               0x0580
  /**< Card reader status event support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 0 */
/** @} */ /* end_name Fifth Byte - Predefined API Support Values */

/** @name Seventh Byte - Predefined API Support Values
  @{ */
#define GSTK_POWER_ON_CARD_SUPPORT                    0x0701
  /**< Power on card support. Supported values: 1 and 0 (Default). */ /* value = (1,0) */  /* default = 0 */
#define GSTK_POWER_OFF_CARD_SUPPORT                   0x0702
  /**< Power off card support. Supported values: 1 and 0 (Default). */ /* value = (1,0) */  /* default = 0 */
#define GSTK_PERFORM_CARD_APDU_SUPPORT                0x0704
  /**< Perform card APDU support. Supported values: 1 and 0 (Default). */ /* value = (1,0) */  /* default = 0 */
#define GSTK_GET_READER_ST_SUPPORT                    0x0708
  /**< Get reader status support. Supported values: 1 and 0 (Default). */ /* value = (1,0) */  /* default = 0 */
#define GSTK_GET_READER_ST_CARD_ID_SUPPORT            0x0710
  /**< Get reader status card ID support. Supported values: 1 and 0 (Default). */ /* value = (1,0) */  /* default = 0 */
/** @} */ /* end_name Seventh Byte - Predefined API Support Values */

/** @name Eighth Byte - Predefined API Support Values
  @{ */
#define GSTK_TIMER_START_STOP_SUPPORT                 0x0801
  /**< Timer start/stop support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_TIMER_GET_CURR_VALUE_SUPPORT             0x0802
  /**< Get current value fir the timer support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_PROVIDE_LOCAL_INFO_TIME_SUPPORT          0x0804
  /**< Provide local information (date/time) support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
#define GSTK_RUN_AT_CMD_SUPPORT                       0x0820
  /**< Run AT command support. Supported values: 1 and 0 (Default). */ /* value = (1,0) */  /* default = 0 */
/** @} */ /* end_name Eighth Byte - Predefined API Support Values */

/** @name Ninth Byte - Predefined API Support Values
  @{ */
#define GSTK_PROVIDE_LOCAL_INFO_TIME_ADV_SUPPORT      0x0910
  /**< Provide local information (time advance) support. Supported values: 1 (Default) and 0. */ /* value = (1,0) */  /* default = 1 */
/** @} */ /* end_name Ninth Byte - Predefined API Support Values */

/** @name 13th Byte - Predefined API Support Values
  @{ */
#define GSTK_NUM_CH_SUPPORT                           0x0DE0
  /**< Number of BIP channels supported. Supported values: 0 (Default) to 7. */ /* 3 bits of info */ /* default = 0 */
/** @} */ /* end_name 13th Byte - Predefined API Support Values */

/** @name 18th Byte - Predefined API Support Values
  @{ */
#define GSTK_PROVIDE_LOCAL_INFO_ESN_SUPPORT           0x1210
  /**< Provide location information (ESN) support. */
#define GSTK_PROVIDE_LOCAL_INFO_IMEISV_SUPPORT        0x1240
  /**< Provide local information (IMEISV) support. */
#define GSTK_PROVIDE_LOCAL_INFO_NETWORK_SEARCH_MODE_SUPPORT              0x1280
  /**< Provide local information (Network Search mode) support. */
/** @} */ /* end_name 18th Byte - Predefined API Support Values */

/** @name 23rd Byte - Predefined API Support Values
  @{ */
#define GSTK_PROVIDE_LOCAL_INFO_MEID_SUPPORT          0x1720
  /**< Provide local information (MEID) support. */

#define GSTK_BROWSER_STATUS_EVT_SUPPORT               0x1901
  /**< Browser Status event support. */

#define GSTK_HCI_CONNECTIVITY_EVT_SUPPORT             0x1920
  /**< HCI Connectivity event support. */

#define GSTK_MAX_PROFILE_DL_UPDATE_SUPPORT            0x4C
  /**< Maximum number of profile download information that can be updated by
       other layers. */

#define GSTK_MAX_MENU_ITEMS                           0x32
  /**< Maximum number of menu items. */
/** @} */ /* end_name 23rd Byte - Predefined API Support Values */
/** @} */ /* end_addtogroup group2 */

/** @addtogroup group3
  @{
*/
/* ------------------ THIRD GROUP ------------------------ */
/** @name First Byte - Terminal Profile Information
  @{ */
#define GSTK_USAT_SMS_PP_DL_SUPPORT_BYTE1_BIT_5       0x0110
  /**< SMS PP download support. Supported values for USAT: 1 and 0. */ /* value = (1,0) */ /* For USAT */
#define GSTK_9EXX_RSP_CODE_SUPPORT                    0x0110
  /**< 9Exx response code support. Supported values for SAT: 1 and 0.  */ /* value = (1,0) */ /* for SAT */
#define GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_7             0x0140
  /**< Call control support. Supported values for USAT: 1 and 0.  */ /* value = (1,0) */ /* For USAT */
#define GSTK_CC_ALWAYS_SENT_DURING_REDIAL             0x0180
  /**< Call control always sent during redial support. Supported values: 1 and 0.  */ /* value = (1,0) */
#define GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_8             0x0180
  /**< Call control support. Supported values: 1 and 0.  */ /* value = (1,0) */
/** @} */ /* end_name First Byte - Terminal Profile Information */

/** @name Second Byte - Terminal Profile Bytes
  @{ */
#define GSTK_USAT_CC_SUPPORT_BYTE_2_BIT_5             0x0210
  /**< Call control support. Supported values for USAT: 1 and 0. */ /* value = (1,0) */ /* For USAT */
/** @} */ /* end_name Second Byte - Terminal Profile Bytes */

/** @name Eighth Byte - Terminal Profile Bytes
  @{ */
#define GSTK_USAT_CC_SUPPORT_BYTE_8_BIT_8             0x0880
  /**< Call control support. Supported values: 1 and 0.  */ /* value = (1,0) */
/** @} */ /* end_name Eighth Byte - Terminal Profile Bytes */

/** @name Ninth Byte - Terminal Profile Bytes
  @{ */
#define GSTK_PLI_NMR_UTRAN                            0x0904
  /**< Provide local information (NMR) support. Supported values: 1 and 0.  */ /* value = (1,0) */
#define GSTK_TR_MAX_BYTES                             512
  /**< Maximum supported length for the terminal response. */
/** @} */ /* end_name Ninth Byte - Terminal Profile Bytes */


/** @name Thirty-first Byte - Terminal Profile Bytes
  @{ */
#define GSTK_COMMUNICATION_CONTROL_FOR_IMS_SUPPORT 0x1F08
  /**<Communication control for IMS support. Supported values: 1 and 0.  */ /* value = (1,0) */
/** @} */ /* end_name Thirty-first Byte - Terminal Profile Bytes */
/** @} */ /* end_addtogroup group3 */

/** @addtogroup clientBitmask
  @{
*/
#define  GSTK_TAL_SUPPORT_1X_VOICE                   0x00000001
  /**< TAL 1X voice support. */
#define  GSTK_TAL_SUPPORT_GW_VOICE                   0x00000002
  /**< TAL GW voice support. */
#define  GSTK_TAL_SUPPORT_SEND_SS                    0x00000004
  /**< TAL SEND SS support. */
#define  GSTK_TAL_SUPPORT_SEND_USSD                  0x00000008
  /**< TAL Send USSD support. */
#define  GSTK_TAL_SUPPORT_BIP_1X                     0x00000010
  /**< TAL CDMA 1X BIP support. */
#define  GSTK_TAL_SUPPORT_BIP_DO                     0x00000020
  /**< TAL CDMA EVDO BIP support. */
#define  GSTK_TAL_SUPPORT_BIP_GW                     0x00000040
  /**< TAL GW BIP support. */
#define  GSTK_TAL_SUPPORT_BIP_LTE                    0x00000080
  /**< TAL LTE BIP support. */
#define  GSTK_TAL_SUPPORT_SEND_SMS                   0x00000100
  /**< TAL Send Short Message support. */
/** @} */ /* end_addtogroup clientBitmask */

/** @addtogroup gstkCmdStructs
  @{
*/
/*===========================================================================
                             Type Defs
===========================================================================*/
/** Client reference data. */
typedef uint32 gstk_client_ref_data_type;

/** Client ID type. */
typedef uint32 gstk_client_id_type;

/** @} */ /* end_addtogroup gstkCmdStructs  */

/** @addtogroup enums
  @{
*/
/*===========================================================================
                             Enum Definitions
===========================================================================*/
/*===========================================================================
--  Enum: gstk_battery_state_enum_type
===========================================================================*/
/**
  Battery status.
*/
typedef enum
{
  GSTK_BATTERY_ST_VERY_LOW = 0x00, /**< Very low. */
  GSTK_BATTERY_ST_LOW      = 0x01, /**< Low. */
  GSTK_BATTERY_ST_AVG      = 0x02, /**< Average. */
  GSTK_BATTERY_ST_GOOD     = 0x03, /**< Good. */
  GSTK_BATTERY_ST_FULL     = 0x04, /**< Full. */
  GSTK_BATTERY_ST_NA       = 0xFE, /**< Status is not applicable. */
  GSTK_BATTERY_ST_UNKNOWN  = 0xFF  /**< Unknown (i.e., battery
                                        is charging). */
}
gstk_battery_state_enum_type;

/*===========================================================================
--  Enum: gstk_target_enum_type
===========================================================================*/
/**
  Target Type.
*/
typedef enum
{
  GSTK_TARGET_NONE         = 0x00, /**< RFU. */
  GSTK_TARGET_UICC_CLF     = 0x01, /**< UICC-CLF. */
  GSTK_TARGET_MAX          = 0xFF  /**< Maximum value of the GSTK target
                                    enumerator. */
}
gstk_target_enum_type;

/*===========================================================================
--  Enum: gstk_tal_client_type
===========================================================================*/
/**
  TAL Client Type
*/
typedef enum
{
  GSTK_TAL_LOCAL_CLIENT = 0,       /**< GSTK is the client of TAL on
                                        the local processor. */
  GSTK_TAL_REMOTE_CLIENT,          /**< GSTK is the client of TAL on
                                        the remote processor. */

  /* Do not add any enums below this line */
  GSTK_TAL_MAX_CLIENTS,            /**< Maximum value of the TAL client
                                        type. */
}gstk_tal_client_type;

/*===========================================================================
--  Enum: gstk_tal_rpt_status_enum_type
===========================================================================*/
/**
  Indicates the TAL UIM report status.
*/
typedef enum {
  GSTK_TAL_UIM_FAIL = 0,            /**< Fail. */
  GSTK_TAL_UIM_PASS = 1             /**< Pass. */
}gstk_tal_rpt_status_enum_type;

/*===========================================================================
--  Enum: gstk_access_technology_type
===========================================================================*/
/**
  Indicates the subscription to radio access technology is available for
  clients of TAL.
*/
typedef enum
{
  GSTK_ACCESS_TECH_GSM               = 0x00, /**< GSM RAT is available. */
  GSTK_ACCESS_TECH_UTRAN             = 0x03, /**< UTRAN is available. */
  GSTK_ACCESS_TECH_CDMA              = 0x06, /**< CDMA RAT is available. */
  GSTK_ACCESS_TECH_HRPD              = 0x07, /**< HRPD is available. */
  GSTK_ACCESS_TECH_LTE               = 0x08, /**< LTE RAT is available. */
  GSTK_ACCESS_TECH_EHRPD             = 0x09, /**< EHRPD is available. */
  GSTK_ACCESS_TECH_WLAN              = 0x0A, /**<WLAN is available>**/
  GSTK_ACCESS_NONE                   = 0x7FFFFFFF
    /**< Maximum value of the Access Technology enumerator. */
}gstk_access_technology_type;

/*===========================================================================
--  Enum: gstk_client_type
===========================================================================*/
/**
  GSTK client type.
*/
typedef enum {
  GSTK_GSTK_TYPE     = 0, /**< GSTK itself. */
  GSTK_DS_TYPE,           /**< Data-related client. */
  GSTK_UI_TYPE,           /**< User interface-related client. */
  GSTK_TEST_TYPE,         /**< Test interface-related client. */
  GSTK_ESTK_TYPE,         /**< Enhanced toolkit-related client. */
  GSTK_WAKEUP_TYPE,       /**< Client can be in Sleep mode, and then woken
                               up. */
  GSTK_IMS_TYPE,         /**<  IMS-related client. */
  GSTK_NO_TYPE,           /**< Default value during powerup. */
  GSTK_TYPE_MAX      = 0x7FFFFFFF
                          /**< Maximum value of the GSTK client type. */
}
gstk_client_type;

/*===========================================================================
--  Enum: gstk_tasks_type
===========================================================================*/
/**
  Tasks that GSTK interacts with that need to be ready in order
  to kick-off various initializations .
*/
typedef enum {
  GSTK_GSTK_TASK     = 0, /**< GSTK itself. */
  GSTK_QMI_MODEM_TASK,    /**< QMI modem task. */
  GSTK_CM_TASK,           /**< CM task. */
  GSTK_RRC_TASK,          /**< RRC task. */
  GSTK_TDS_RRC_TASK,      /**< TDSRRC task. */

  GSTK_TASK_MAX      = 0x7FFFFFFF
                          /**< Maximum value of the task. */
}
gstk_tasks_type;

/*===========================================================================
--  Enum: gstk_client_reg_cb_func_format_enum_type
===========================================================================*/
/**
  Callback function type used when a client registers with the
  GSTK for proactive commands and events.
*/
typedef enum {
  GSTK_CLI_CB_NONE                       = 0x00,
    /**< Default type during powerup. */
  GSTK_CLI_CB_WITH_GSTK_FORMAT_SUPPORT,
    /**< Callback supporting the GSTK format only. */
  GSTK_CLI_CB_WITH_MULTI_FORMAT_SUPPORT,
    /**< Callback supporting multiple formats: RAW or GSTK. */
  GSTK_CLI_CB_MAX                        = 0x7FFFFFFF
    /**< Maximum value of the callback function type. */
}
gstk_client_reg_cb_func_format_enum_type;

/*===========================================================================
--  Enum: gstk_client_pro_cmd_data_format_enum_type
===========================================================================*/
/**
  Format of the proactive commands to be registered by the GSTK client.
*/
typedef enum
{
  GSTK_FORMAT_NONE  = 0x00,
    /**< Default format during powerup. The client can specify
         either GSTK_RAW_FORMAT or GSTK_GSTK_FORMAT in its request.*/
  GSTK_RAW_FORMAT,
    /**< Proactive command in RAW format. */
  GSTK_GSTK_FORMAT,
    /**< Proactive command in GSTK format. */
  GSTK_FORMAT_MAX   = 0x7FFFFFFF
    /**< Maximum value for the format type. */
} gstk_client_pro_cmd_data_format_enum_type;

/*===========================================================================
--  Enum: gstk_tk_functionality_enum_type
===========================================================================*/
/**
  Used by the GSTK client to specify one of the
  functionalities of each proactive command that it is to process.
*/
typedef enum
{
  GSTK_HANDLE_FUNC_NONE           = 0x00,
    /**< Default value during powerup. The client must specify only
         valid values, such as Display, Modem, or All. */
  GSTK_HANDLE_DISPLAY_FUNC_ONLY,
    /**< Display functionality only. */
  GSTK_HANDLE_MODEM_FUNC_ONLY,
    /**< Modem functionality only. */
  GSTK_HANDLE_ALL_FUNC,
    /**< All functionality. */
  GSTK_HANDLE_FUNC_MAX            = 0x7FFFFFFF
    /**< Maximum value for the functionality. */
} gstk_tk_functionality_enum_type;

/*=============================================================================
--  Enum: gstk_status_enum_type
=============================================================================*/
/**
  @xreflabel{hdr:gstkStatusEnumType}
  Determines whether the function call into the GSTK is successful.
*/
typedef enum {
  GSTK_SUCCESS,
    /**< Function call was successful. */
  GSTK_ERROR,
    /**< Fatal error. */
  GSTK_DUPLICATE_REGISTRATION,
    /**< One client is already registered. The GSTK expects only one client to
         be registered for a specific client type. */
  GSTK_CLIENT_NOT_REGISTERED,
    /**< Invalid client ID for the specified functions. */
  GSTK_NO_CLIENT_REG_FOUND,
    /**< No client has registered for a command. */
  GSTK_INVALID_CLIENT_TYPE,
    /**< Invalid client ID type. */
  GSTK_PROFILE_VALUE_NOT_ALLOWED,
    /**< Client tried to set the value of a profile download bitmask that has a
         default value of 0 to 1. */
  GSTK_PROFILE_ALREADY_SENT,
    /**< Terminal profile already sent to the card. */
  GSTK_MEMORY_ERROR,
    /**< No memory avaliable for allocation. */
  GSTK_CLIENT_SPACE_FULL,
    /**< No more client registrations can be allowed. */
  GSTK_BAD_PARAM,
    /**< Invalid parameter has been passed into the function. */
  GSTK_NULL_INPUT_PARAM,
    /**< At least one of the pointer input parameters is NULL. */
  GSTK_PARAM_EXCEED_BOUNDARY,
    /**< Boundry for the input parameter is exceeded. */
  GSTK_INVALID_LENGTH,
    /**< Command passed in by the client has an invalid length. */
  GSTK_EXTRA_PARAM_MISSING,
    /**< Extra parameter is missing in the terminal response. */
  GSTK_EXTRA_PARAM_NOT_REQUIRED,
    /**< Extra parameter is not required in the terminal response. */
  GSTK_UNSUPPORTED_COMMAND,
    /**< Invalid timer request. */
  GSTK_INVALID_COMMAND,
    /**< Invalid command. */
  GSTK_INSUFFICIENT_INFO,
    /**< Command passed in by the client has insufficient information. */
  GSTK_FILE_ACCESS_FAIL,
    /**< File access returned by the GSDI is FALSE when a SIM/USIM file access is
         required (i.e., the GSTK tries to obtain icon information based on a
         proactive command request). */ /* e.g., when accessing EF-IMG failed */
  GSTK_CONTRADICTING_TIMER_REQUEST,
    /**< Contradiction with the current timer state. */
  GSTK_NOT_INIT,
    /**< Attempt was made to utilize the GSTK Library functions before the GSTK
         entered GSTK_IDLE_S. */
  GSTK_INVALID_DEVICE_COMMAND,
    /**< Device command is invalid. */
  GSTK_RPC_ERROR,
    /**< RPC error. */
  GSTK_STATUS_MAX                    = 0x7FFFFFFF
    /**< Maximum value of the GSTK status. */
}
gstk_status_enum_type;

/*===========================================================================
--  Enum: gstk_cmd_enum_type
===========================================================================*/
/**
  Command message IDs comprise four different message ID groups:

  - Proactive command -- Requests sent from the SIM/USIM to the ME. Message IDs
                         for proactive command messages sent to the upper
                         layers. These are used in the command header message
                         ID portion of the structure.
  - Envelope response -- Message IDs for envelope command response messages
                         from the SIM/USIM in response to an earlier envelope
                         command sent by the upper layers. These are used in
                         the command header message ID portion of the structure.
  - Terminal response -- Requests from the ME to SIM/USIM. Message IDs for terminal
                         responses of proactive command messages sent from the
                         upper layers. Upper layers use this in the library
                         function gstk_send_terminal_response() as the
                         indication of what terminal response it is going to
                         request the GSTK to pack and send to the UIM driver.
  - Envelope command  -- Message IDs for envelope command messages sent from the
                         upper layers. Upper layers must send this as
                         the first parameter in the library function
                         gstk_send_envelope_evt_download_command() to indicate to the GSTK
                         which type of envelope the GSTK is being requested to pack and
                         send forward to the UIM driver.
*/
typedef enum {
  GSTK_CMD_ENUM_NOT_USE          = 0x00,  /**< Default value. */

  /* Proactive Command request */
  GSTK_DISPLAY_TEXT_REQ               = 0x01, /**< Proactive command: Display Text. */
  GSTK_GET_INKEY_REQ                  = 0x02, /**< Proactive command: Get Inkey. */
  GSTK_GET_INPUT_REQ                  = 0x03, /**< Proactive command: Get Input. */
  GSTK_LAUNCH_BROWSER_REQ             = 0x04, /**< Proactive command: Launch Browser. */
  GSTK_MORE_TIME_REQ                  = 0x05, /**< Proactive command: More Time. */
  GSTK_PLAY_TONE_REQ                  = 0x06, /**< Proactive command: Play Tone. */
  GSTK_POLL_INTERVAL_REQ              = 0x07, /**< Proactive command: Poll Interval. */
  GSTK_PROVIDE_LOCAL_INFO_REQ         = 0x08, /**< Proactive command: Provide Local Information. */
  GSTK_REFRESH_REQ                    = 0x09, /**< Proactive command: Refresh. */
  GSTK_RUN_AT_CMDS_REQ                = 0x0A, /**< Proactive command: Run AT Command. */
  GSTK_SELECT_ITEM_REQ                = 0x0B, /**< Proactive command: Select Item. */
  GSTK_SEND_SMS_REQ                   = 0x0C, /**< Proactive command: Send Short Message. */
  GSTK_SEND_SS_REQ                    = 0x0D, /**< Proactive command: Send SS. */
  GSTK_SEND_USSD_REQ                  = 0x0E, /**< Proactive command: Send USSD. */
  GSTK_SETUP_CALL_REQ                 = 0x0F, /**< Proactive command: Set Up Call. */
  GSTK_SETUP_EVENT_LIST_REQ           = 0x10, /**< Proactive command: Set Up Event List. */
  GSTK_SETUP_MENU_REQ                 = 0x11, /**< Proactive command: Set Up Menu. */
  GSTK_SETUP_IDLE_TEXT_REQ            = 0x12, /**< Proactive command: Set Up Idle Mode Text. */
  GSTK_POLLING_OFF_REQ                = 0x13, /**< Proactive command: Polling Off. */
  GSTK_END_PROACTIVE_CMD_REQ          = 0x14, /**< Proactive command: End of Proactive Command Session. */
  GSTK_TOOLKIT_MODE_CHANGE_REQ        = 0x15, /**< GSTK internal command: Mode Change. */
  GSTK_PROVIDE_LANG_INFO_REQ          = 0x16, /**< Proactive command: Provide Local Information (Language). */
  GSTK_SEND_DTMF_REQ                  = 0x17, /**< Proactive command: Send DTMF. */
  GSTK_LANG_NOTIFICATION_REQ          = 0x18, /**< Proactive command: Language Notification. */
  GSTK_TIMER_MANAGEMENT_REQ           = 0x19, /**< Proactive command: Timer Management. */
  GSTK_OPEN_CH_REQ                    = 0x1A, /**< Proactive command: Open Channel. */
  GSTK_CLOSE_CH_REQ                   = 0x1B, /**< Proactive command: Close Channel. */
  GSTK_SEND_DATA_REQ                  = 0x1C, /**< Proactive command: Send Data. */
  GSTK_RECEIVE_DATA_REQ               = 0x1D, /**< Proactive command: Receive Data. */
  GSTK_GET_CH_STATUS_REQ              = 0x1E, /**< Proactive command: Get Channel Status. */
  GSTK_PROVIDE_BATTERY_STATUS_REQ     = 0x1F, /**< Proactive command: Provide Local Information (Batter). */
  GSTK_PROVIDE_DATE_TIME_REQ          = 0x20, /**< Proactive command: Provide Local Information (Date/Time). */
  GSTK_ACTIVATE_REQ                   = 0x21, /**< Proactive command: Activate. */
   /* Msg ID for SCWS */
  GSTK_SCWS_OPEN_CH_REQ               = 0x31, /**< Proactive command: Smart Card Web Server (Open Channel). */
  GSTK_SCWS_CLOSE_CH_REQ              = 0x32, /**< Proactive command: Smart Card Web Server (Close Channel). */
  GSTK_SCWS_SEND_DATA_REQ             = 0x33, /**< Proactive command: Smart Card Web Server (Send Data). */
  GSTK_SCWS_DATA_AVAIL_RSP_REQ        = 0x34, /**< Proactive command: Smart Card Web Server (Data Available Response). */

  /* Envelope Indication Responses */
  GSTK_PROFILE_DL_IND_RSP              = 0x81, /**< Terminal profile download response. */
  GSTK_MENU_IND_RSP                    = 0x82, /**< Envelope response: Menu Selection. */
  GSTK_SMS_PP_DOWNLOAD_IND_RSP         = 0x83, /**< Envelope response: SMS PP Download. */
  GSTK_MT_CALL_EVT_IND_RSP             = 0x84, /**< Envelope response: MT Call event. */
  GSTK_CALL_CONNECTED_EVT_IND_RSP      = 0x85, /**< Envelope response: Call Connection event. */
  GSTK_CALL_DISCONNECTED_EVT_IND_RSP   = 0x86, /**< Envelope response: Call Disconnected event. */
  GSTK_IDLE_SCRN_EVT_IND_RSP           = 0x87, /**< Envelope response: Idle Screen Available event. */
  GSTK_LOCATION_STATUS_EVT_IND_RSP     = 0x88, /**< Envelope response: Location Status event. */
  GSTK_USER_ACTIVITY_EVT_IND_RSP       = 0x89, /**< Envelope response: User Activity event. */
  GSTK_MO_SMS_CTRL_IND_RSP             = 0x8A, /**< Envelope response: MO Short Message Control. */
  GSTK_CC_IND_RSP                      = 0x8B, /**< Envelope response: Call Control. */
  GSTK_TIMER_EXPIRE_IND_RSP            = 0x8C, /**< Envelope response: Timer Expiration. */
  GSTK_LANG_SELECTION_EVT_IND_RSP      = 0x8D, /**< Envelope response: Language Selection. */
  GSTK_BROWSER_TERMINATION_EVT_IND_RSP = 0x8E, /**< Envelope response: Browser Termination. */
  GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP = 0x8F, /**< Envelope response: Cell Broadcast Download. */
  GSTK_DATA_AVAIL_EVT_IND_RSP          = 0x90, /**< Envelope response: Data Available. */
  GSTK_CH_STATUS_EVT_IND_RSP           = 0x91, /**< Envelope response: Channel Status. */
  GSTK_ACCESS_TECH_EVT_IND_RSP         = 0x92, /**< Envelope response: Access Technology Change. */
  GSTK_NW_SEARCH_MODE_EVT_IND_RSP      = 0x93, /**< Envelope response: Search Mode Change. */
  GSTK_RAW_ENVELOPE_RSP                = 0x94, /**< Envelope response: Raw Envelope. */
  GSTK_LOC_INFO_ENVELOPE_RSP           = 0x95, /**< Envelope response: Location Information. */
  GSTK_NW_REJ_EVT_IND_RSP              = 0x96, /**< Envelope response: Network Rejection. */
  GSTK_HCI_CONNECTIVITY_IND_RSP        = 0x97, /**< Envelope response: HCI Connectivity. */
  GSTK_CSG_CELL_SELECTION_EVT_IND_RSP  = 0x98, /**< Envelope response: CSG Cell Selection. */
  GSTK_IMS_REG_EVT_IND_RSP             = 0x99, /**< Envelope response: IMS Registration. */

  /* Proactive Command Confirmations (Terminal Response) */
  GSTK_DISPLAY_TEXT_CNF              = 0x41, /**< Terminal response: Display Text. */
  GSTK_GET_INKEY_CNF                 = 0x42, /**< Terminal response: Get Inkey. */
  GSTK_GET_INPUT_CNF                 = 0x43, /**< Terminal response: Get Input. */
  GSTK_LAUNCH_BROWSER_CNF            = 0x44, /**< Terminal response: Launch Browser. */
  GSTK_MORE_TIME_CNF                 = 0x45, /**< Terminal response: Moer Time. */
  GSTK_PLAY_TONE_CNF                 = 0x46, /**< Terminal response: Play Tone. */
  GSTK_POLL_INTERVAL_CNF             = 0x47, /**< Terminal response: Poll Interval. */
  GSTK_PROVIDE_LOCAL_INFO_CNF        = 0x48, /**< Terminal response: Provide Local Information. */
  GSTK_REFRESH_CNF                   = 0x49, /**< Terminal response: Refresh. */
  GSTK_RUN_AT_CMDS_CNF               = 0x4A, /**< Terminal response: Run AT Command. */
  GSTK_SELECT_ITEM_CNF               = 0x4B, /**< Terminal response: Select Item. */
  GSTK_SEND_SMS_CNF                  = 0x4C, /**< Terminal response: Send Short Message. */
  GSTK_SEND_SS_CNF                   = 0x4D, /**< Terminal response: Send SS. */
  GSTK_SEND_USSD_CNF                 = 0x4E, /**< Terminal response: Send USSD. */
  GSTK_SETUP_CALL_CNF                = 0x4F, /**< Terminal response: Set Up Call. */
  GSTK_SETUP_EVENT_LIST_CNF          = 0x50, /**< Terminal response: Set Up Event List. */
  GSTK_SETUP_MENU_CNF                = 0x51, /**< Terminal response: Set Up Menu. */
  GSTK_SETUP_IDLE_TEXT_CNF           = 0x52, /**< Terminal response: Set Up Idle Mode Text. */
  GSTK_POLLING_OFF_CNF               = 0x53, /**< Terminal response: Polling Off. */
  GSTK_PROVIDE_LANG_INFO_CNF         = 0x54, /**< Terminal response: Provide Local Information (Language). */
  GSTK_SEND_DTMF_CNF                 = 0x55, /**< Terminal response: Send DTMF. */
  GSTK_LANG_NOTIFICATION_CNF         = 0x56, /**< Terminal response: Language Notification. */
  GSTK_TIMER_MANAGEMENT_CNF          = 0x57, /**< Terminal response: Timer Management. */
  GSTK_OPEN_CH_CNF                   = 0x58, /**< Terminal response: Open Channel. */
  GSTK_CLOSE_CH_CNF                  = 0x59, /**< Terminal response: Close Channel. */
  GSTK_SEND_DATA_CNF                 = 0x5A, /**< Terminal response: Send Data. */
  GSTK_RECEIVE_DATA_CNF              = 0x5B, /**< Terminal response: Receive Data. */
  GSTK_GET_CH_STATUS_CNF             = 0x5C, /**< Terminal response: Get Channel Status. */
  GSTK_PROVIDE_ACCESS_TECH_CNF       = 0x5D, /**< Terminal response: Provide Local Information (Access Technology). */
  GSTK_PROVIDE_BATT_STATUS_CNF       = 0x5E, /**< Terminal response: Provide Local Information (Battery). */
  GSTK_PROVIDE_NW_SEARCH_MODE_CNF    = 0x5F, /**< Terminal response: Provide Local Information (Network Search Mode). */
  GSTK_RAW_TERMIMAL_RESPONSE         = 0x60, /**< Terminal response: Raw Terminal Response. */
  GSTK_PROVIDE_CSG_INFO_CNF          = 0x306,/**< Terminal response: Provide Local Information (CSG information). */

  /* Envelope Command Indication request */
  GSTK_PROFILE_DOWNLOAD_IND          = 0x61, /**< Terminal profile download. */
  GSTK_MENU_SELECTION_IND            = 0x62, /**< Envelope command: Menu Selection. */
  GSTK_SMS_PP_DOWNLOAD_IND           = 0x63, /**< Envelope command: SMS PP Download. */
  GSTK_MT_CALL_EVT_IND               = 0x64, /**< Envelope command: MT Call. */
  GSTK_CALL_CONNECTED_EVT_IND        = 0x65, /**< Envelope command: Call Connected. */
  GSTK_CALL_DISCONNECTED_EVT_IND     = 0x66, /**< Envelope command: Call Disconnected. */
  GSTK_IDLE_SCRN_EVT_IND             = 0x67, /**< Envelope command: Idle Screen. */
  GSTK_LOCATION_STATUS_EVT_IND       = 0x68, /**< Envelope command: Location Status. */
  GSTK_USER_ACTIVITY_EVT_IND         = 0x69, /**< Envelope command: User Activity. */
  GSTK_MO_SMS_CTRL_IND               = 0x6A, /**< Envelope command: MO Short Message Control. */
  GSTK_CC_IND                        = 0x6B, /**< Envelope command: Call Control. */
  GSTK_TIMER_EXPIRE_IND              = 0x6C, /**< Envelope command: Timer Expiration. */
  GSTK_LANG_SELECTION_EVT_IND        = 0x6D, /**< Envelope command: Language Selection. */
  GSTK_BROWSER_TERMINATION_EVT_IND   = 0x6E, /**< Envelope command: Browser Termination. */
  GSTK_CELL_BROADCAST_DOWNLOAD_IND   = 0x6F, /**< Envelope command: Cell Broadcast. */
  GSTK_DATA_AVAIL_EVT_IND            = 0x70, /**< Envelope command: Data Available. */
  GSTK_CH_STATUS_EVT_IND             = 0x71, /**< Envelope command: Channel Status. */
  GSTK_ACCESS_TECH_EVT_IND           = 0x72, /**< Envelope command: Access Technology Change. */
  GSTK_NW_SEARCH_MODE_EVT_IND        = 0x73, /**< Envelope command: Network Search Mode Change. */
  GSTK_RAW_ENVELOPE_IND              = 0x74, /**< Envelope command: Raw Envelope Command. */
  GSTK_NW_REJ_EVT_IND                = 0x75, /**< Envelope command: Network Rejection. */
  GSTK_HCI_CONNECTIVITY_IND          = 0x76, /**< Envelope command: HCI Connectivity. */
  GSTK_CSG_CELL_SELECTION_EVT_IND    = 0x77, /**< Envelope command: CSG Cell Selection. */
  GSTK_IMS_REG_EVT_IND               = 0x78, /**< Envelope command: IMS Registration. */
  GSTK_LOCATION_INFORMATION_IND      = 0xDF, /**< Envelope command: Location information. */
  /*
  ** Message ID's for envelope command notification as a result of
  ** the setup event list
  */
  GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY  = 0x100,/**< Envelope command: Set Up Event List (Idle Screen). */
  GSTK_SETUP_EVT_USER_ACT_NOTIFY     = 0x101,/**< Envelope command: Set Up Event LIst (User Activity). */
  GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY = 0x102,/**< Envelope command: Set Up Event List (Browser Termination). */
  GSTK_SETUP_EVT_LANG_SEL_NOTIFY     = 0x103,/**< Envelope command: Set Up Event List (Language Selection). */
  GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY   = 0x104,/**< Envelope command: Set Up Event List (Data Available). */
  GSTK_SETUP_EVT_CH_STATUS_NOTIFY    = 0x105,/**< Envelope command: Set Up Event List (Channel Status). */
  GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY  = 0x106,/**< Envelope command: Set Up Event List (HCI Connect). */
  GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF   = 0x200,/**< Envelope command: Unsupported/unknown command. */
  GSTK_PROVIDE_DATE_TIME_CNF         = 0x201,/**< Terminal response: Provide Local Information (Date/Time). */
  GSTK_ACTIVATE_CNF                  = 0x202,/**< Terminal response: Activate. */

  /*
  ** Message ID's for internal commands
  */
  GSTK_CLIENT_ID_REG_REQ             = 0x300,/**< Internal command: Client ID registration. */
  GSTK_CLIENT_PRO_CMD_REG_REQ        = 0x301,/**< Internal command: Proactive command registration. */
  GSTK_TERM_RSP_SW1_SW2_RPT          = 0x302,/**< Internal command: Terminal response Status Word (SW1/SW2). */
  GSTK_DS_SYSTEM_STATUS_EX_IND       = 0x303,/**< DS System Status Info */
  GSTK_DS_WLAN_MAC_ADDR_CHANGE_IND   = 0x304,/**< DS Indication: WLAN MAC Address change */
  GSTK_DS_WLAN_SSID_CHANGE_IND       = 0x305,/**< DS Indication: WLAN SSID change */
  GSTK_CMD_ENUM_MAX                  = 0x7FFFFFFF/**< Maximum value of the GSTK command enumerator. */
}
gstk_cmd_enum_type;

/*===========================================================================
--  Enum: gstk_general_result_enum_type
===========================================================================*/
/**
  Terminal response result codes destined for the STK application
  as defined in 3GPP TS 11.14 @xhyperref{S1,[S1]}. The upper layer specifies the code
  in the second parameter in gstk_send_terminal_response() to indicate
  the status of the command being executed.

  @note1 All values are based on 3GPP TS 11.14 @xhyperref{S1,[S1]}.
*/
typedef enum {
  GSTK_COMMAND_PERFORMED_SUCCESSFULLY                              = 0x00,
    /**< Command was performed successfully. */
  GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION                = 0x01,
    /**< Command was performed with partial comprehension. */
  GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION                  = 0x02,
    /**< Command was performed with information missing. */
  GSTK_PCMD_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ             = 0x03,
    /**< Refresh was performed with additional EFS Read. */
  GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED         = 0x04,
    /**< Command was successful but the requested icon is not displayed. */
  GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM       = 0x05,
    /**< Command was performed but was modified by call control by the SIM. */

  GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE                          = 0x06,
    /**< Command was successful (Limited Service). */
  GSTK_COMMAND_PERFORMED_WITH_MODIFICATION                         = 0x07,
    /**< Command was performed with modifications. */
  GSTK_REFRESH_PERFORMED_BUT_USIM_NOT_ACTIVE                       = 0x08,
    /**< Refresh was performed but the USIM was not active. */

  GSTK_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER                    = 0x10,
    /**< Proactive SIM session was terminated by the user. */
  GSTK_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER                  = 0x11,
    /**< Backward move in the session was requested by the user. */
  GSTK_NO_RESPONSE_FROM_USER                                       = 0x12,
    /**< No response from the user. */
  GSTK_HELP_INFORMATION_REQUIRED_BY_THE_USER                       = 0x13,
    /**< Help information is required by the user. */

  GSTK_USSD_SS_TERMINATED_BY_THE_USER                              = 0x14,
    /**< USSD/SS was terminated by the user. */

  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND                      = 0x20,
    /**< ME is currently unable to process the command. */
  GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND                 = 0x21,
    /**< Network is currently unable to process the command. */
  GSTK_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST                     = 0x22,
    /**< User did not accept the Call Set Up request. */
  GSTK_USER_CLEARED_DOWN_CALL_BEFORE_CONNECTION_OR_NETWORK_RELEASE = 0x23,
    /**< User cleared the downed call before connection or network release. */
  GSTK_ACTION_IN_CONTRADICTION_WITH_THE_CURRENT_TIMER_STATE        = 0x24,
    /**< Action in contrdiction with the current timer state. */
  GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM      = 0x25,
    /**< Temporary problem exists in the interaction by the SIM with call
         control. */

  GSTK_LAUNCH_BROWSER_GENERIC_ERROR                                = 0x26,
    /**< Launch Browser generic error. */

  GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES                          = 0x30,
    /**< Command is beyond the capabilities of the ME. */
  GSTK_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME                           = 0x31,
    /**< Command type is not understood by the ME. */
  GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME                           = 0x32,
    /**< Command data is not understood by the ME. */
  GSTK_COMMAND_NUMBER_NOT_KNOWN_BY_ME                              = 0x33,
    /**< Command number is not known by the ME. */
  GSTK_SS_RETURN_ERROR                                             = 0x34,
    /**< SS return error. */
  GSTK_SMS_RP_ERROR                                                = 0x35,
    /**< SMS PP error. */
  GSTK_ERROR_REQUIRED_VALUES_ARE_MISSING                           = 0x36,
    /**< Required values are missing. */
  GSTK_USSD_RETURN_ERROR                                           = 0x37,
    /**< USSE return error */
  GSTK_MULTIPLE_CARD_COMMANDS_ERROR                                = 0x38,
    /**< Multiple card commands error. */
  GSTK_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM  = 0x39,
    /**< Problem exists in the interaction by the SIM with call control or MO SM control. */

  GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR                           = 0x3A,
    /**< Bearer Independent Protocol error. */
  GSTK_ACCESS_TECH_UNABLE_TO_PROCESS_CMD                           = 0x3B,
    /**< Access Technology is unable to process the command. */
  GSTK_GENERAL_RESULT_MAX                                          = 0xFFFF
    /**< Maximum value of the general result. */
}
gstk_general_result_enum_type;

/*===========================================================================
--  Enum: gstk_additional_info_code_enum_type
===========================================================================*/
/**
  Additional information codes based on #gstk_general_result_enum_type.
  Following are values that may be required in the
  additional_result parameter in the gstk_send_terminal_response() function.

  @note1hang No further parsing or changes in value is to be performed by the
             GSTK. The values are based on ETSI TS 102.223 @xhyperref{S7,[S7]}.

  For GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND:\n
     - GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN
     - GSTK_SCREEN_IS_BUSY
     - GSTK_ME_CURRENTLY_BUSY_ON_CALL
     - GSTK_ME_CURRENTLY_BUSY_ON_SS_TRANSACTION
     - GSTK_NO_SERVICE_AVAILABLE
     - GSTK_ACCESS_CONTROL_CLASS_BAR
     - GSTK_RADIO_RESOURCE_NOT_GRANTED
     - GSTK_NOT_IN_SPEECH_CALL
     - GSTK_ME_CURRENTLY_BUSY_ON_USSD_TRANSACTION
     - GSTK_ME_CURRENTLY_BUSY_ON_SEND_DTMF_COMMAND
     - GSTK_NO_USIM_ACTIVE

  For the Successful Send SS command:\n
     - First byte -- SS return result operation code.\n
     - Second byte -- SS return result parameters.

  For GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND:\n
     - First byte -- Cause value of the Cause information element from the network;
                     Bit 8 = 1.
     - 0x00 -- No specific cause.

  For GSTK_SS_RETURN_ERROR:\n
    - First byte -- Error value in the Facility information element returned
                    by the network;
    - 0x00 -- No specific cause.

  For GSTK_SMS_RP_ERROR:\n
     - First byte -- Cause value in the RP-Cause element of the RP-ERROR
                     message from the network; Bit 8 = 0.
     - 0x00 -- No specific cause.

  For GSTK_USSD_RETURN_ERROR:\n
     - First byte -- Error value in the Facility information from the network.
     - 0x00 -- No specific cause.

  For GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM and
  GSTK_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM:
     - GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN
     - GSTK_ACTION_NOT_ALLOWED
     - GSTK_REQUEST_CHANGED

 For GSTK_LAUNCH_BROWSER_GENERIC_ERROR:\n
     - GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN
     - GSTK_BEARER_UNAVAILABLE
     - GSTK_BROWSER_UNAVAILABLE
     - GSTK_ME_UNABLE_TO_READ_PROVISIONING_DATA
*/
typedef enum {

  GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN                             = 0x00,
    /**< No specific cause. */
  GSTK_SCREEN_IS_BUSY                                             = 0x01,
    /**< Screen is busy. */
  GSTK_ACTION_NOT_ALLOWED                                         = 0x01,
    /**< Action is not allowed. */
  GSTK_BEARER_UNAVAILABLE                                         = 0x01,
    /**< Bearer is unavailable. */
  GSTK_CH_UNAVAILABLE                                             = 0x01,
    /**< Channel is unavailable. */

  GSTK_REQUEST_CHANGED                                            = 0x02,
    /**< Request has been changed. */
  GSTK_ME_CURRENTLY_BUSY_ON_CALL                                  = 0x02,
    /**< Mobile equipment is currently busy on a call. */
  GSTK_BROWSER_UNAVAILABLE                                        = 0x02,
    /**< Browser is unavailable. */
  GSTK_CH_CLOSED                                                  = 0x02,
    /**< Channel is closed. */

  GSTK_ME_UNABLE_TO_READ_PROVISIONING_DATA                        = 0x03,
    /**< Mobile equipment is unable to read the provisioning data. */
  GSTK_ME_CURRENTLY_BUSY_ON_SS_TRANSACTION                        = 0x03,
    /**< Mobile equipment is currently busy with a supplementary services
         transaction. */
  GSTK_CH_ID_INVALID                                              = 0x03,
    /**< Channel ID is invalid. */

  GSTK_NO_SERVICE_AVAILABLE                                       = 0x04,
    /**< No service is available. */
  GSTK_BUFFER_SIZE_UNAVAILABLE                                    = 0x04,
    /**< Buffer size is unavailable. */

  GSTK_ACCESS_CONTROL_CLASS_BAR                                   = 0x05,
    /**< Access Control Class is barred on the serving network. */
  GSTK_SECURITY_ERROR_UNSUCCESSFUL_AUTH                           = 0x05,
    /**< Security error occurred. The authorization attempt was unsuccessful. */

  GSTK_RADIO_RESOURCE_NOT_GRANTED                                 = 0x06,
    /**< Radio resource was not granted. */
  GSTK_SIM_ME_INTERFACE_TRANSPORT_LV_UNAVAILABLE                  = 0x06,
    /**< SIM/ME interface transport level is unavailable. */

  GSTK_NOT_IN_SPEECH_CALL                                         = 0x07,
    /**< Not in a speech call. */

  GSTK_ME_CURRENTLY_BUSY_ON_USSD_TRANSACTION                      = 0x08,
    /**< Mobile equipment is currently busy on an unstructured supplementary
         service data transaction. */

  GSTK_ME_CURRENTLY_BUSY_ON_SEND_DTMF_COMMAND                     = 0x09,
    /**< Mobile equipment is currently busy sending the Dual-tone
         Multifrequency command. */

  GSTK_NO_USIM_ACTIVE                                             = 0x0A,
    /**< No universal subscriber identity module is active. */

  GSTK_ADDITIONAL_INFO_CODE_MAX                                   = 0xFFFF
    /**< Maximum value of the addition information code. */
}
gstk_additional_info_code_enum_type;

/* ----------------------------- Envelope response general return status ----------------------------------- */


/*=============================================================================
--  Enum: gstk_general_envelope_rsp_enum_type
=============================================================================*/
/**
  Used in the envelop response (i.e., Menu Selection and SMS PP download
  envelope responses) when there is no additional data returned from the card.
*/
typedef enum {
  GSTK_ENVELOPE_CMD_SUCCESS,   /**< Maps to the status word: 0x9000 or 0x91xx. */
  GSTK_ENVELOPE_CMD_CARD_BUSY, /**< Card is busy. The mobile equipment can
                                    choose to try later: 0x9300. */
  GSTK_ENVELOPE_CMD_FAIL,      /**< Other status word. */
  GSTK_ENVELOPE_CMD_RSP_MAX               = 0xFFFF
    /**< Maximum value of the envelope response enumerator. */
}
gstk_general_envelope_rsp_enum_type;


/*===========================================================================
--  Enum: gstk_terminal_profile_rsp_enum_type
===========================================================================*/
/**
  Used in the terminal profile response.
*/
typedef enum {
  GSTK_PROFILE_RSP_SUCCESS, /**< Maps to the status word: 0x9000 or 0x91xx. */
  GSTK_PROFILE_RSP_FAIL,    /**< Other status word. */
  GSTK_PROFILE_RSP_MAX                    = 0xFFFF
    /**< Maximum value of the profile response. */
}
gstk_terminal_profile_rsp_enum_type;

/*===========================================================================
--  Enum: gstk_input_format_enum_type
===========================================================================*/
/**
  Input format.
*/
typedef enum {
  GSTK_SMS_DEF_ALPHABET               = 0x00,  /**< SMS default alphabet. */
  GSTK_YES_NO                         = 0x01,  /**< Yes or No. */
  GSTK_NUMERICAL_ONLY                 = 0x02,  /**< Numerical only. */
  GSTK_UCS2_ALPHABET                  = 0x03,  /**< UCS-2 alphabet. */
  GSTK_IMMEDIATE_DIGIT_RSP            = 0x04,  /**< Immediate Digit Response. */
  GSTK_YES_NO_AND_IMM_DGT_RSP         = 0x05,  /**< Yes/No and Immediate Digit Response. */
  GSTK_INPUT_FORMAT_MAX               = 0xFFFF /**< Maximum value of the input format. */
}
gstk_input_format_enum_type;

/*===========================================================================
--  Enum: gstk_input_pack_format_enum_type
===========================================================================*/
/**
  Pack/unpack format of the input.
*/
typedef enum {
  GSTK_UNPACK_FORMAT              = 0x00,  /**< Unpacked format. */
  GSTK_PACK_FORMAT                = 0x01,  /**< Packed format. */
  GSTK_INPUT_PACK_FORMAT_MAX      = 0xFFFF /**< Maximum value of the input pack format. */
}
gstk_input_pack_format_enum_type;

/*===========================================================================
--  Enum: gstk_menu_present_enum_type
===========================================================================*/
/**
  Menu presentation format available for the select item based on the
  command detail parsing in the Select Item proactive command.
*/
typedef enum {
  GSTK_PRESENTATION_NOT_SPECIFIED     = 0x00,  /**< Presentation is not specified. */
  GSTK_DATA_VALUE_PRESENTATION        = 0x01,  /**< Data value presentation. */
  GSTK_NAVIGATION_PRESENTATION        = 0x02,  /**< Navigation presentation. */
  GSTK_MENU_PRESENT_MAX               = 0xFFFF /**< Maximum value of the menu presentation type. */
}
gstk_menu_present_enum_type;

/*===========================================================================
--  Enum: gstk_tone_enum_type
===========================================================================*/
/**
  Tones available for the Play Tone command as specified in the corresponding
  command detail TLV.

  @note1 All values are based on ETSI TS 102.223 @xhyperref{S7,[S7]}.
*/
typedef enum {
  GSTK_DIAL_TONE                       = 0x01,  /**< Dial tone. */
  GSTK_CALLED_SUBSCRIBER_BUSY          = 0x02,  /**< Called Subscriber Busy tone. */
  GSTK_CONGESTION                      = 0x03,  /**< Congestion tone. */
  GSTK_RADIO_PATH_ACK                  = 0x04,  /**< Radio Path Acknowledgement tone. */
  GSTK_RADIO_PATH_NOT_AVAIL_CALL_DROP  = 0x05,  /**< Radio Path Not Availabe Call Dorp tone. */
  GSTK_ERROR_TONE                      = 0x06,  /**< Error tone. */
  GSTK_CALL_WAITING_TONE               = 0x07,  /**< Call Waiting tone. */
  GSTK_RINGING_TONE                    = 0x08,  /**< Ring tone. */
  GSTK_GENERAL_BEEP                    = 0x10,  /**< General beep. */
  GSTK_POSITIVE_ACK_TONE               = 0x11,  /**< Positive Acknowledgement tone. */
  GSTK_NEGATIVE_ACK_TONE               = 0x12,  /**< Negative Acknowledgement tone. */
  /* UTK */
  GSTK_RINGING_TONE_SELECTED_BY_USER   = 0x13,  /**< Ring tone selected by the user. */
  GSTK_SMS_ALERT_TONE_SELECTED_BY_USER = 0x14,  /**< Short Message Alert tone selected by the user. */
  /* to indicate it is optional */
  GSTK_NOT_IN_USE                      = 0xAA,  /**< Tone is optional. */
  GSTK_TONE_MAX                        = 0xFFFF /**< Maximum value of the tone type. */
}
gstk_tone_enum_type;

/*===========================================================================
--  Enum: gstk_call_setup_req_enum_type
===========================================================================*/
/**
  Call setup parameters as specified in the command detail.
*/
typedef enum {
  GSTK_NO_OTHER_CALLS                 = 0x00,  /**< No other calls. */
  GSTK_HOLD_ACTIVE_CALLS              = 0x01,  /**< Hold active calls. */
  GSTK_DISCONNECT_ACTIVE_CALLS        = 0x02,  /**< Disconnect active calls. */
  GSTK_CALL_SETUP_REQ_MAX             = 0xFFFF /**< Maximum value of the Call Setup Parameter type. */
}
gstk_call_setup_req_enum_type;

/*===========================================================================
--  Enum: gstk_launch_mode_enum_type
===========================================================================*/
/**
  Browser launching modes as specified in the command detail.

  @note1 All values are based on ETSI TS 102.223 @xhyperref{S7,[S7]}.
*/
typedef enum {
    /* values based on standard */
  GSTK_LAUNCH_IF_NOT_ALREADY_LAUNCH   = 0x00,  /**< Launch the browser if not already launched. */
  GSTK_USE_EXISTING_BROWSER           = 0x02,  /**< Use the existing browser. */
  GSTK_CLOSE_EXISTING_BROWSER         = 0x03,  /**< Close the existing browser. */
  GSTK_LAUNCH_MODE_MAX                = 0xFFFF /**< Maximum value of the Launch Browser mode. */
}
gstk_launch_mode_enum_type;

/*===========================================================================
--  Enum: gstk_bearer_code_enum_type
===========================================================================*/
/**
  Bearer type of the Bearer Independent Protocol.

  @note1 All values are based on ETSI TS 102.223 @xhyperref{S7,[S7]}.
*/
typedef enum {
  GSTK_SMS                            = 0x00,   /**< Short message. */
  GSTK_CSD                            = 0x01,   /**< CSD. */
  GSTK_USSD_BEARER_CODE               = 0x02,   /**< USSD.  */
  GSTK_GPRS                           = 0x03,   /**< GPRS. */
  GSTK_BEARER_DEFAULT                 = 0x04,   /**< Default bearer. */
  GSTK_EUTRAN_EXT_PARAM               = 0x05,   /**< E-UTRAN extended. */
  GSTK_EUTRAN_MAPPED_UTRAN_PS         = 0x06,   /**< E-UTRAN mapped UTRAN packet switch. */
  GSTK_RFU                            = 0x00FF, /**< Reserved. */
  GSTK_BEARER_CODE_MAX                = 0xFFFF  /**< Maximum value of the bearer type. */ /* 0x04 - 0xFF = RFU */
}
gstk_bearer_code_enum_type;

/*=============================================================================
--  Enum: gstk_call_enum_type
=============================================================================*/
/**
  Enables the GSTK or client to set the correct union structure type according
  to whether it is a voice call, SS, or USSD.
*/
typedef enum {
  GSTK_VOICE,            /**< Voice. */
  GSTK_SS,               /**< SS. */
  GSTK_USSD,             /**< USSD. */
  GSTK_PDP_CONTEXT_ACT,  /**< PDP context activation. */
  GSTK_NONE,             /**< No voice, SS, or USSD-related structure type is
                              required when the field is optional. */
  GSTK_EPS_PDN_CONN_ACT, /**< EPS PDN connect activation. */
  GSTK_IMS_CALL,         /**< EPS PDN connect activation. */
  GSTK_CALL_MAX        = 0xFFFF /**< Maximum value of the call type. */
}gstk_call_enum_type;

/*===========================================================================
--  Enum: gstk_control_result_enum_type
===========================================================================*/
/**
  Call control result.

  @note1 All values are based on ETSI TS 102.223 @xhyperref{S7,[S7]}.
*/
typedef enum {
  GSTK_ALLOWED_NO_MOD     = 0x00,  /**< Allow no modifications. */
  GSTK_NOT_ALLOWED        = 0x01,  /**< Not allowed. */
  GSTK_ALLOWED_BUT_MOD    = 0x02,  /**< Allowed but modified. */
  GSTK_CONTROL_RESULT_MAX = 0xFFFF /**< Maximum value of the call control result. */
}
gstk_control_result_enum_type;

/*===========================================================================
--  Enum: gstk_bc_repeat_ind_enum_type
===========================================================================*/
/**
  Bearer capability repeat indication.

  @note1 All values are based on ETSI TS 102.223 @xhyperref{S7,[S7]}.
*/
typedef enum {
  GSTK_ALTERNATE_MODE     = 0x01,  /**< Alternate mode. */
  GSTK_SEQUENTIAL_MODE    = 0x03,  /**< Sequential mode. */
  GSTK_BC_REPEAT_IND_MAX  = 0xFFFF /**< Maximum value of the repeat indication type. */
}
gstk_bc_repeat_ind_enum_type;

/*===========================================================================
--  Enum: gstk_ton_enum_type
===========================================================================*/
/**
  Type of number type.

  @note1 All values are based on ETSI TS 102.223 @xhyperref{S7,[S7]}.
*/
typedef enum {
   GSTK_UNKNOWN_TON          = 0x00,  /**< Unknown. */
   GSTK_INTERNATIONAL_NUM    = 0x01,  /**< International number. */
   GSTK_NATIONAL_NUM         = 0x02,  /**< National number. */
   GSTK_NETWORK_SPECIFIC_NUM = 0x03,  /**< Network-specific number. */
   // QUESTION
   GSTK_RFU_TON              = 0x00FF,/**< Reserved. */
   GSTK_TON_MAX              = 0xFFFF /**< Maximum value of the TON type. */
}
gstk_ton_enum_type;

/*===========================================================================
--  Enum: gstk_npi_enum_type
===========================================================================*/
/**
  Numbering plan identifier type.

  @note1 All values are based on ETSI TS 102.223 @xhyperref{S7,[S7]}.
*/
typedef enum {
   GSTK_UNKNOWN_NPI          = 0x00,   /**< Unknown NPI. */
   GSTK_ISDN_TELEPHONY_NPI   = 0x01,   /**< ISDN Telephony NPI. */
   GSTK_DATA_NPI             = 0x03,   /**< Data NPI. */
   GSTK_TELEX_NPI            = 0x04,   /**< Telex NPI. */
   GSTK_PRIVATE_NPI          = 0x09,   /**< Private NPI. */
   GSTK_EXT_RESERVED_NPI     = 0x0F,   /**< Extend/reserved NPI. */
   GSTK_RFU_NPI              = 0x00FF, /**< Reserved. */
   GSTK_NPI_MAX              = 0xFFFF  /**< Maximum value of the NPI type. */
}
gstk_npi_enum_type;

/*===========================================================================
--  Enum: gstk_slot_id_enum_type
===========================================================================*/
/**
  Slot ID.
*/
typedef enum {
  GSTK_SLOT_AUTO           = 0x00,  /**< Do not use in functions that require
                                         slot information. */
  GSTK_SLOT_1              = 0x01,  /**< Slot 1 */
  GSTK_SLOT_2              = 0x02,  /**< Slot 2 */
  GSTK_SLOT_3              = 0x03,  /**< Slot 3 */
  GSTK_SLOT_ID_MAX         = 0xFF /**< Maximum value of the slot ID. */
}
gstk_slot_id_enum_type;

/*===========================================================================
--  Enum: gstk_tag_enum_type
===========================================================================*/
/**
  Intended for the Set Up Call command only. Helps ESTK clients distinguish
  between the User Confirmation Alpha or Display Alpha stage.
*/
typedef enum {
  GSTK_TAG_ORIGINAL_CMD             = 0x00,  /**< Tag for original cmd. */
  GSTK_TAG_USER_CONF_ALPHA          = 0x01,  /**< User Confirmation Alpha. */
  GSTK_TAG_DISPLAY_ALPHA            = 0x02,  /**< Display Alpha. */
  GSTK_TAG_RECOVERY                 = 0x03,  /**< Recovery. */
  GSTK_TAG_CARD_ERROR               = 0x04,  /**< CARD error */
  GSTK_TAG_POWER_DOWN               = 0x05,  /**< Power Down */
  GSTK_TAG_REFRESH_RESET            = 0x06,  /**< REFRESH RESET */
  GSTK_TAG_REFRESH_2G_INIT          = 0x07,  /**< REFRESH NAA INIT */
  GSTK_TAG_TP_DOWNLOAD              = 0X0A,  /**< TP DOWNLOAD */
  GSTK_TAG_MAX                      = 0xFFFF /**< Maximum value of the tag type. */
}
gstk_tag_enum_type;

/*===========================================================================
--  Enum: gstk_img_code_enum_type
===========================================================================*/
/**
  Image code scheme.
*/
typedef enum {
  GSTK_UNKNOWN_IMAGE_CODE_SCHEME, /**< Unknow Image Code scheme. */
  GSTK_BASIC_IMAGE_CODE_SCHEME,   /**< Basic Image Code scheme. */
  GSTK_COLOUR_IMAGE_CODE_SCHEME,  /**< Color Image Code scheme. */
  GSTK_IMAGE_CODE_SCHEME_MAX        = 0xFFFF /**< Maximum value of the Image Code scheme. */
}
gstk_img_code_enum_type;

/*===========================================================================
    Enum: gstk_dcs_enum_type
===========================================================================*/
/**
  Data coding schemes.
*/
typedef enum {
  GSTK_DCS_8_BIT,      /**< 8-bit coding scheme. */
  GSTK_DCS_UCS2_8_BIT, /**< UCS-2 8-bit coding scheme. */
  GSTK_DCS_UCS2_7_BIT, /**< UCS-2 7-bit coding scheme. */
  GSTK_DCS_7_BIT,      /**< 7-bit coding scheme. */
  GSTK_DCS_NONE,       /**< Coding scheme is not specified. */
  GSTK_DCS_MAX                      = 0xFFFF
    /**< Maximum value of the coding scheme type. */
}
gstk_dcs_enum_type;

/*===========================================================================
    Enum: gstk_app_enum_type
===========================================================================*/
/**
SIM/USIM application type.
*/
typedef enum{
 GSTK_APP_NONE,   /**< Not Specified. */
 GSTK_APP_SIM,    /**< SIM application. */
 GSTK_APP_RUIM,   /**< RSIM application. */
 GSTK_APP_USIM,   /**< USIM application. */
 GSTK_APP_CSIM,   /**< CSIM application. */
 GSTK_APP_UNKNOWN /**< Unknown application type. */
}gstk_app_enum_type;

/*===========================================================================
    Enum: gstk_cmd_group_enum_type
===========================================================================*/
/**
  Category of the command.
*/
typedef enum
{
  GSTK_PROACTIVE_CMD,              /**< Proactive command. */
  GSTK_TERMINAL_RSP,               /**< Terminal response. */
  GSTK_ENVELOPE_CMD,               /**< Envelope command. */
  GSTK_ENVELOPE_RSP,               /**< Envelope response. */
  GSTK_TERMINAL_PROFILE_CMD,       /**< Terminal profile update. */
  GSTK_TERMINAL_PROFILE_RSP,       /**< Terminal profile response. */
  GSTK_STOP_CMD,                   /**< Power off. */
  GSTK_MMGSDI_CMD,                 /**< MMGSDI events. */
  GSTK_TIMER_EXP_CMD,              /**< Wathcdog expiration command. */
  GSTK_MM_IDLE_CMD,                /**< Mobility management enters the Idle state. */
  GSTK_RRC_CELL_CHANGE_IND,        /**< RRC cell change indication. */
  GSTK_MODE_CHANGE_IND,            /**< Call manager mode change. */
  GSTK_TIME_ZONE_CHANGE_IND,       /**< Call manager Time Zone change. */
  GSTK_CM_PH_IND,                  /**< CM PHONE event indication */
  GSTK_CLIENT_REG_REQ_CMD,         /**< Used by clients to register for
                                        proactive commands and for the client ID
                                        registrations. */
  GSTK_GET_CURR_ACCESS_TECH_CMD,   /**< Gets the current access technology. */
  GSTK_RETRY_ENVELOPE_CMD,         /**< Internal command to retry the envelope
                                        command. */
  GSTK_SEND_POSTPONE_ENVELOPE_CMD, /**< Sends a postponed envelope command. */
  GSTK_TIMER_FOR_ENV_RETRY_CMD,    /**< Internal command to retry the envelope
                                        command. */
  GSTK_SETUP_OTASP_CALL_CMD,       /**< Initiates an over-the-air service
                                        programming call to program the EF
                                        IMSI_M. */
  GSTK_TAL_CMD,                    /**< Command sent by the TAL interface. */
  GSTK_TAL_SEND_LS_CMD,            /**<  Command used by TAL to send Location Status */
  GSTK_CM_CSG_CHANGE_IND,          /**<  Call Manager CSG information change */
  GSTK_RECOVERY_CMD,               /**< SIM recovery command from UIMDRV */
  GSTK_DS_EVT_CMD,                 /**< DS Events */
  GSTK_TR_REPORT,                  /**< Send SW1 and SW2 to registered clients*/
  GSTK_CM_SUBS_CHANGE_IND,         /**< Call manager Subscription info change */
  GSTK_NV_REFRESH_CMD,             /**< NV Refresh notification from MCFG */
  GSTK_WMS_EVT_IND,                /**< WMS event */
  GSTK_OTASP_REG_CMD,              /**< OTASP registration command */  
  GSTK_MAX_CMD = 0x7FFFFFFF        /**< Maximum value of the Command Group type. */
}
gstk_cmd_group_enum_type;

/*===========================================================================
    Enum: gstk_local_info_gen_enum_type
===========================================================================*/
/**
  Type of location information for the Provide Local Information command.
*/
typedef enum
{
  GSTK_GSM_UMTS_LOCATION_INFO         = 0x00, /**< GSM/UMTS location information. */
  GSTK_CDMA_LOCATION_INFO             = 0x01, /**< CDMA location information. */
  GSTK_NO_SRVC_LOCATION_INFO          = 0x02, /**< No Service location information. */
  GSTK_IMEI_INFO                      = 0x03, /**< IMEI information. */
  GSTK_NMR_BCCH_INFO                  = 0x04, /**< NMR (BCCH information. */
  GSTK_NMR_UTRAN_INFO                 = 0x05, /**< UTRAN NMR information. */
  GSTK_DATE_TIME_ZONE_INFO            = 0x06, /**< Date/time/timezone information. */
  GSTK_LANGUAGE_SETTING_INFO          = 0x07, /**< Language setting information. */
  GSTK_TIMING_ADVANCE_INFO            = 0x08, /**< Timing advance information. */
  GSTK_ACCESS_TECHNOLOGY_INFO         = 0x09, /**< Access technology information. */
  GSTK_ESN_INFO                       = 0x0A, /**< ESN location information. */
  GSTK_IMEISV_INFO                    = 0x0B, /**< IMEISV location information. */
  GSTK_BATTERY_STATUS_INFO            = 0x0C, /**< Battery status. */
  GSTK_MEID_INFO                      = 0x0D, /**< MEID information. */
  GSTK_NETWORK_SEARCH_MODE_INFO       = 0x0E, /**< Network Search mode. */
  GSTK_CSG_CELL_INFO                  = 0x0F, /**< CSG Cell infromation. */
  GSTK_INVALID_LOC_INFO               = 0xFF  /**< Invalid type. */
}
gstk_local_info_gen_enum_type;

/*===========================================================================
    Enum: gstk_csd_bearer_speed_enum_type
===========================================================================*/
/**
  Bearer speeds for the circuit-switch data.
*/
typedef enum {
  GSTK_CSD_BEARER_SPEED_AUTO                    = 0, /**< Automatic. */
  GSTK_CSD_BEARER_SPEED_300_V21                 = 1, /**< 300 bps (V21). */
  GSTK_CSD_BEARER_SPEED_1200_V22                = 2, /**< 1200 bps (V22). */
  GSTK_CSD_BEARER_SPEED_1200_75_V23             = 3, /**< 1200 bps (V23). */
  GSTK_CSD_BEARER_SPEED_2400_V22                = 4, /**< 2400 bps (V24). */
  GSTK_CSD_BEARER_SPEED_2400_V26                = 5, /**< 2400 bps (V26). */
  GSTK_CSD_BEARER_SPEED_4800_V32                = 6, /**< 4800 bps (V32). */
  GSTK_CSD_BEARER_SPEED_9600_V32                = 7, /**< 9600 bps (V32). */
  GSTK_CSD_BEARER_SPEED_9600_V34                = 12, /**< 9600 bps (V34). */
  GSTK_CSD_BEARER_SPEED_14400_V34               = 14, /**< 14400 bps (V34). */
  GSTK_CSD_BEARER_SPEED_19200_V34               = 15, /**< 19200 bps (V34). */
  GSTK_CSD_BEARER_SPEED_28800_V34               = 16, /**< 28800 bps (V34). */
  GSTK_CSD_BEARER_SPEED_33600_V34               = 17, /**< 33600 bps (V34). */
  GSTK_CSD_BEARER_SPEED_1200_V120               = 34, /**< 1200 bps (V120). */
  GSTK_CSD_BEARER_SPEED_2400_V120               = 36, /**< 2400 bps (V120). */
  GSTK_CSD_BEARER_SPEED_4800_V120               = 38, /**< 4800 bps (V120). */
  GSTK_CSD_BEARER_SPEED_9600_V120               = 39, /**< 9600 bps (V120). */
  GSTK_CSD_BEARER_SPEED_14400_V120              = 43, /**< 14400 bps (V120). */
  GSTK_CSD_BEARER_SPEED_19200_V120              = 47, /**< 19200 bps (V120). */
  GSTK_CSD_BEARER_SPEED_28800_V120              = 48, /**< 28800 bps (V120). */
  GSTK_CSD_BEARER_SPEED_38400_V120              = 49, /**< 38400 bps (V120). */
  GSTK_CSD_BEARER_SPEED_48000_V120              = 50, /**< 48000 bps (V120). */
  GSTK_CSD_BEARER_SPEED_56000_V120              = 51, /**< 56000 bps (V120). */
  GSTK_CSD_BEARER_SPEED_300_V110                = 65, /**< 300 bps (V110). */
  GSTK_CSD_BEARER_SPEED_1200_V110               = 66, /**< 120 bps (V110). */
  GSTK_CSD_BEARER_SPEED_2400_V110_X31           = 68, /**< 2400 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_4800_V110_X31           = 70, /**< 4800 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_9600_V110_X31           = 71, /**< 9600 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_14400_V110_X31          = 75, /**< 14400 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_19200_V110_X31          = 79, /**< 19200 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_28800_V110_X31          = 80, /**< 28800 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_38400_V110_X31          = 81, /**< 38400 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_48000_V110_X31          = 82, /**< 48000 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_56000_V110_X31          = 83, /**< 56000 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_64000_V110_X31          = 84, /**< 64000 bps (V110 X31). */
  GSTK_CSD_BEARER_SPEED_56000_BIT_TRANSPARENT   = 115, /**< 56000 bps (Transparent). */
  GSTK_CSD_BEARER_SPEED_64000_BIT_TRANSPARENT   = 116, /**< 64000 bps (Transparent). */
  GSTK_CSD_BEARER_SPEED_32000_PIAFS32K          = 120, /**< 32000 bps (PIAFS32K). */
  GSTK_CSD_BEARER_SPEED_64000_PIAFS32K          = 121, /**< 64000 bps (PIAFS32K). */
  GSTK_CSD_BEARER_SPEED_28800_MULTIMEDIA        = 130, /**< 28800 bps Multimedia. */
  GSTK_CSD_BEARER_SPEED_32000_MULTIMEDIA        = 131, /**< 32000 bps Multimedia. */
  GSTK_CSD_BEARER_SPEED_33600_MULTIMEDIA        = 132, /**< 33600 bps Multimedia. */
  GSTK_CSD_BEARER_SPEED_56000_MULTIMEDIA        = 133, /**< 56000 bps Multimedia. */
  GSTK_CSD_BEARER_SPEED_64000_MULTIMEDIA        = 134, /**< 64000 bps Multimedia. */
  GSTK_CSD_BEARER_SPEED_MAX                     = 0xFFFF /**< Maximum value of the CSD speed. */
}
gstk_csd_bearer_speed_enum_type;

/*===========================================================================
    Enum: gstk_csd_bearer_name_enum_type
===========================================================================*/

/** Bearer name for the circuit-switch data. */
typedef enum
{
  GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_UDI_3_1KHZ = 0x0000,
    /**< Data Circuit Asynchronous 3.1 kHz. */
  GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_UDI_3_1KHZ  = 0x0001,
    /**< Data Circuit Synchronous UDI 3.1 kHz. */
  GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_UDI           = 0x0002,
    /**< PAD Access Asynchronous UDI. */
  GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNCH_UDI        = 0x0003,
    /**< Packet Access Synchronous UDI. */
  GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_RDI        = 0x0004,
    /**< Data Circuit Asynchronous RDI. */
  GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_RDI         = 0x0005,
    /**< Data Circuit Synchronous RDI. */
  GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_RDI           = 0x0006,
    /**< PAD Access Asynchronous RDI. */
  GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNC_RDI         = 0x0007,
    /**< Packet Access Synchronous RDI. */
  GSTK_CSD_BEARER_NAME_MAX                            = 0xFFFF
    /**< Maximum value of the bearer name type. */
}
gstk_csd_bearer_name_enum_type;

/*===========================================================================
    Enum: gstk_csd_bearer_conn_element_enum_type
===========================================================================*/
/**
  CSD bearer connection element type.
*/
typedef enum
{
  GSTK_CSD_BEARER_CE_TRANSPARENT                    = 0x0000,
    /**< CSD bearer is transparent. */
  GSTK_CSD_BEARER_CE_NON_TRANSPARENT                = 0x0001,
    /**< CSD bearer is non-transparent. */
  GSTK_CSD_BEARER_CE_BOTH_TRANSPARENT_PREF          = 0x0002,
    /**< CSD bearer is both transparent and preferred. */
  GSTK_CSD_BEARER_CE_BOTH_NON_TRANSPARENT_PREF      = 0x0003,
    /**< CSD bearer is both non-transparent and preferred. */
  GSTK_CSD_BEARER_CE_MAX                            = 0xFFFF
    /**< Maximum value of the CSD bearer connection element type. */
}
gstk_csd_bearer_conn_element_enum_type;

/*===========================================================================
--  Enum: gstk_packet_data_protocol_enum_type
===========================================================================*/
/**
  Packet data protocol type.
*/
typedef enum {
  GSTK_PKT_DATA_IP                     = 0x02,  /**< IP protocol. */
  GSTK_PKT_DATA_PROTOCOL_MAX           = 0xFFFF /**< Maximum value of the data protocol type. */
}
gstk_packet_data_protocol_enum_type;

/*===========================================================================
--  Enum: gstk_ch_status_additional_info_enum_type
===========================================================================*/
/**
  Additional information for the channel status.
*/
typedef enum {
    GSTK_CH_STATUS_NO_FURTHER_INFO,          /**< No further information can be given. */
    GSTK_CH_STATUS_LINK_DROPPED,             /**< Link dropped. */
    GSTK_CH_STATUS_ADDITIONAL_INFO_NOT_USED, /**< Additional information is not used. */
    GSTK_CH_STATUS_ADDITIONAL_INFO_RFU,      /**< Reserved. */
    GSTK_CH_STATUS_ADDITIONAL_INFO_MAX       = 0xFFFF
      /**< Maximum value of the additional information of the channel status. */
}
gstk_ch_status_additional_info_enum_type;

/*===========================================================================
--  Enum: gstk_pkt_data_address_enum_type
===========================================================================*/
/**
  Data IP address format.
*/
typedef enum {
  GSTK_PKT_DATA_NO_ADDRESS_GIVEN, /**< No address is given. */
  GSTK_PKT_DATA_DYNAMIC_ADDRESS,  /**< Dynamic address. */
  GSTK_PKT_DATA_IPV4_ADDRESS,     /**< IPv4 address. */
  GSTK_PKT_DATA_IPV6_ADDRESS,     /**< IPv6 address. */
  GSTK_PKT_DATA_ADDRESS_MAX                     = 0xFFFF
    /**< Maximum value of the address type. */
}
gstk_pkt_data_address_enum_type;

/*===========================================================================
--  Enum: gstk_pkt_data_transport_level_enum_type
===========================================================================*/
/**
  Indicates whether the UDP or TCP transport protocol is to be used
  when the SAT/USAT application provides a value that falls in the RFU. When
  used, the GSTK rejects the command and sends the terminal response with
  #GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME.
 */
typedef enum {
  GSTK_PKT_DATA_NOT_PRESENT, /**< Not present. */
  GSTK_PKT_DATA_UDP,         /**< UDP. */
  GSTK_PKT_DATA_TCP,         /**< TCP. */
  GSTK_PKT_DATA_TCP_UICC_SERVER_MODE, /**< TCP Server mode. */
  GSTK_PKT_DATA_TRANSPORT_LEVEL_MAX              = 0xFFFF
    /**< Maximum value of the transport level type. */
}
gstk_pkt_data_transport_level_enum_type;

/*===========================================================================
--  Enum: gstk_browser_term_cause_enum_type
===========================================================================*/
/**
  Cause of the browser termination.
*/
typedef enum
{
  GSTK_BROWSER_USER_TERMINATED    = 0x00,  /**< User terminated the browser. */
  GSTK_BROWSER_ERROR_TERMINATED   = 0x01,  /**< Browser terminated due to error. */
  GSTK_BROWSER_TERM_CAUSE_MAX     = 0xFFFF /**< Maximum value of the browser
                                                termination cause. */
}
gstk_browser_term_cause_enum_type;

/*===========================================================================
--  Enum: gstk_service_table_enum_type
===========================================================================*/
/**
  Query type of the UICC service table settings.
*/
typedef enum {
  GSTK_SRV_TBL_CC_TYPE      = 0x00, /**< Query call control. */
  GSTK_SRV_TBL_GPRS_CC_TYPE = 0x01, /**< Query GPRS call control. */
  GSTK_SRV_TBL_MAX          = 0xFF  /**< Maximum value of the service table
                                         query type. */
}
gstk_service_info_enum_type;

/*===========================================================================
--  Enum: gstk_scws_ch_state_enum_type
===========================================================================*/
/**
  SCWS channel state.
*/
typedef enum {
  GSTK_SCWS_TCP_IN_CLOSED_STATE      = 0x00,   /**< Channel in the Closed state. */
  GSTK_SCWS_TCP_IN_LISTEN_STATE      = 0x01,   /**< Channel in the Listen state. */
  GSTK_SCWS_TCP_IN_ESTABLISHED_STATE = 0x02,   /**< Channel in the Established state. */
  GSTK_SCWS_TCP_STATE_MAX            = 0xFFFF  /**< Maximum value of the SCWS channel state. */
}
gstk_scws_ch_state_enum_type;

/*===========================================================================
  Enum - gstk_location_status_enum_type - location status information
===========================================================================*/
/**
  GSTK Location Status.
*/
typedef enum
{
  /*  values based on standard */
  GSTK_NORMAL_SERVICE                 = 0x00, /**< UE is in normal service. */
  GSTK_LIMITED_SERVICE                = 0x01, /**< UE is in limited service. */
  GSTK_NO_SERVICE                     = 0x02  /**< UE is in no service. */
}
gstk_location_status_enum_type;


/*===========================================================================
  Enum - gstk_modem_cap_enum_type - modem capability information
===========================================================================*/
/**
  GSTK Modem Capability.
*/
typedef enum
{
  /* Modem capability for LTE */
  GSTK_MODEM_CAPABILITY_FEATURE_LTE                 = 0x01,
  GSTK_MODEM_UNKNOWN                                = 0xFF
}
gstk_modem_cap_enum_type;

/** @} */ /* end_addtogroup enums */

/** @addtogroup gstkCmdStructs
  @{
*/
/*===========================================================================
                             Struct Definitions
===========================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      GSTK_TAL_SEND_PRO_CMD_ARG_TYPE
------------------------------------------------------------------------------*/
/**
  @brief Stores arguments for the Send proactive command from the UIM.
*/
typedef struct {
  gstk_slot_id_enum_type  slot;        /**< GSTK SIM card slot type. */
  uint32                  data_length; /**< Length of the buffer. */
  uint8                   *data_ptr;   /**< Data buffer. */
}gstk_tal_send_pro_cmd_arg_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GSTK_TAL_UIM_REPORT_TYPE
------------------------------------------------------------------------------*/
/**
  @brief Maps and stores the responses from the UIM driver to be sent to TAL
  clients.
*/
typedef struct{
  uint32                        user_data;   /**< UIM reference type. */
  uint32                        sw1;         /**< UIM status word 1. */
  uint32                        sw2;         /**< UIM status word 2. */
  uint8                         get_rsp_sw1; /**< UIM intermediate status word 1. */
  uint8                         get_rsp_sw2; /**< UIM intermediate status word 2. */
  boolean                       proactive_cmd_pending; /**< Is proactive command pending flag. */
  gstk_tal_rpt_status_enum_type rpt_status;  /**< Report status. */
  uint16                        data_length; /**< Length of the data returned. */
  uint8                         *data_ptr;   /**< Data returned. */
}gstk_tal_uim_report_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      GSTK_TAL_CMD_DETAILS_TYPE
------------------------------------------------------------------------------*/
/**
  @brief Stores the proactive command, terminal response, and UIM response details.
*/
typedef struct{
  gstk_cmd_group_enum_type        cmd_group;     /**< GSTK command group. */
  union {
  gstk_tal_uim_report_type        tal_uim_rpt;   /**< UIM report function calback. */
  gstk_tal_send_pro_cmd_arg_type  tal_pro_cmd;   /**< Proactive command data. */
  }cmd_details;
}gstk_tal_cmd_details_type;

/*=============================================================================
-- Prototype for TAL callback function.
=============================================================================*/
/**
  Used by the GSTK TAL to send commands from the UIM to the GSTK.
*/
typedef void (*gstk_tal_reg_cb_type)(
   gstk_tal_cmd_details_type    *cmd_details);

/* ----------------------------------------------------------------------------
   STRUCTURE:      GSTK_TAL_UIM_CMD_TYPE
------------------------------------------------------------------------------*/
/**
  @brief Stores the data contents of the terminal response, terminal profile,
  and envelope commands.
*/
typedef struct{
  gstk_cmd_group_enum_type  cmd_type;     /**< GSTK command group. */
  uint32                    user_data;    /**< User reference data. */
  gstk_slot_id_enum_type    slot;         /**< GSTK slot information. */
  uint16                    num_bytes;    /**< Length of the data buffer. */
  uint8                     *data_ptr;    /**< Data buffer. */
}gstk_tal_uim_cmd_type;
/** @} */ /* end_addtogroup gstkCmdStructs */

/** @addtogroup termProfStruct
  @{
*/
/*===========================================================================
--  Struct: gstk_profile_dl_support_type
===========================================================================*/
/**
  @brief Contains the profile download information that must be
  filled in by the client during profile download.
*/
typedef struct {
  uint8         value;              /**< Terminal profile item value. */
  uint32        profile_dl_bitmask; /**< Terminal profile item bitmask. */
}
gstk_profile_dl_support_type;

/*===========================================================================
--  Struct: gstk_profile_dl_support_ext_type
===========================================================================*/
/**
  @brief Contains the profile download information that must be filled in by
  the client during profile download. This structure differs from
  gstk_profile_dl_support_type in that it differentiates between SAT and USAT
  TP download.
*/
typedef struct {
  uint8         value;              /**< Terminal profile item value. */
  uint32        profile_dl_bitmask; /**< Terminal profile item bitmask. */
  uint8         card_app_type;      /**< Terminal profile application type (e.g., SAT/USAT). */
}
gstk_profile_dl_support_ext_type;

/** @} */ /* end_addtogroup termProfStruct */

/** @addtogroup gstkCmdStructs
  @{
*/
/*===========================================================================
--  Struct: gstk_exp_hdr_type
===========================================================================*/
/**
  @brief Used by all commands between the GSTK and the clients of the GSTK.
*/
typedef struct {
  gstk_slot_id_enum_type      sim_slot_id;
    /**< SIM card slot ID. */
  gstk_cmd_enum_type          command_id;
    /**< GSTK command ID. */
  uint32                      cmd_detail_reference;
    /**< Client is to pass the same ref_id back to the GSTK in the terminal
         response. */
  uint8                       command_number;
    /**< Valid for proactive commands only. */
  uint32                      user_data;
    /**< User reference data. */
  gstk_tag_enum_type          tag;
    /**< Differentiates between the User Confimation Alpha or Display Alpha stage for
         Set Up Call. */
}
gstk_exp_hdr_type;
/** @} */ /* end_addtogroup gstkCmdStructs */

/** @addtogroup genStructUSAT
  @{
*/
/* ----------------------------------- Structures to be ----------------------------------------------------
---------------------------------- used in various gstk_xxxx_req_type, -------------------------------------
---------------------------------- gstk_xxxx_rsp_type, the terminal response function ----------------------
---------------------------------- parameter and envelope command function parameter ----------------------*/

/*---------------------------------------- General structures ------------------------------*/

/*===========================================================================
--  Struct: gstk_string_type
===========================================================================*/
/**
  @brief String information for the commands.
*/
typedef struct {
  uint8                        dcs;    /**< GSM default character set (0x04).\n
                                            UCS-2 formatting (0x08). */
  uint8                        length; /**< Length of the text. */
  uint8*                       text;   /**< Text string. */
}
gstk_string_type;

/*===========================================================================
--  Struct: gstk_cb_page_type
===========================================================================*/
/**
  @brief Cell broadcast page message. This structure contains the USSD string.
*/
typedef struct {
  uint8                                length;
    /**< Length of the cell broadcast page. */
  uint8                              * cb_page_data;
    /**< Cell broadcast page data. */
}
gstk_cb_page_type;

/*===========================================================================
--  Struct: gstk_icon_type
===========================================================================*/
/**
  @brief Icon information for the proactive commands that have a TLV icon type.

  @note1hang Only one instance of the icon is obtained even when there are
  multiple icon instances residing in the same record in the EF_IMG file of the
  SIM/USIM card.
*/
typedef struct {
  boolean                       present;
    /**< Indicates whether the icon type is presented. */
  uint8                         qualifier;
    /**< Indicates whether the display includes text.\n
         - 0 -- Display without text string.
         - 1 -- Display together with text. */
  uint8                         height;
    /**< Icon height from the EF_IMG file (file ID 4F20) expressed in raster
         image points. */
  uint8                         width;
    /**< Icon width from the EF_IMG file (file ID 4F20) expressed in raster
         image points. */
  uint32                        size;
    /**< Size of the image instance obtained by the GSTK from the record's
         image instance data descriptor, based on the file ID from the EF_IMG
         file. */
  gstk_img_code_enum_type       image_coding_scheme;
    /**< Image coding scheme. */
  uint8                       * data;
    /**< Pointer to the image instance data from the image instance data
         files. */
  uint8                         rec_num;
    /**< Record number in the EF_IMG file. */
}
gstk_icon_type;

/*===========================================================================
--  Struct: gstk_item_object_type
===========================================================================*/
/**
  @brief Item information for the Select Item and Set Up Menu proactive
  commands.
 */
typedef struct {
  uint8                        item_id;   /**< Item ID. */
  uint8                        length;    /**< Length of the text.*/
  uint8                      * item_text; /**< Pointer to the next item. */
}
gstk_item_object_type;

/*===========================================================================
--  Struct: gstk_next_item_ind_type
===========================================================================*/
/**
  @brief Contains the next item indication.
*/
typedef struct {
  uint8                        num_items;
    /**< Number from the length field. */
  uint8                      * next_action_list;
    /**< Next action list (based on 3GPP TS 27.007 @xhyperref{S4,[S4]})\n
         - CMD_STK_SET_UP_CALL
         - CMD_STK_SEND_SS
         - CMD_STK_USSD
         - MD_STK_SEND_SHORT_MESSAGE
         - CMD_STK_LAUNCH_BROWSER
         - CMD_STK_PLAY_TONE
         - CMD_STK_DISPLAY_TEXT
         - CMD_STK_GET_INKEY
         - CMD_STK_GET_INPUT
         - CMD_STK_SELECT_ITEM
         - CMD_STK_SET_UP_MENU
         - CMD_STK_SET_UP_IDLE_MODE_TEXT
         - CMD_STK_END_OF_PROACTIVE_SES

         Future:\n
         - CMD_STK_PERFORM_CARD_APDU
         - CMD_STK_POWER_ON_CARD
         - CMD_STK_POWER_OFF_CARD
         - CMD_STK_GET_READER_STATUS
         - CMD_STK_OPEN_CHANNEL
         - CMD_STK_CLOSE_CHANNEL
         - CMD_STK_RECEIVE_DATA
         - CMD_STK_SEND_DATA
         - CMD_STK_GET_CHANNEL_STATUS */
}
gstk_next_item_ind_type;

/*===========================================================================
--  Struct: gstk_icon_id_list_type
===========================================================================*/
/**
  @brief Icon list.
*/
typedef struct {
  boolean                      display_icon_only;
    /**< Indicates whether to display the icon only. */
  uint8                        num_items;
    /**< Number from the length field. */
  gstk_icon_type             * icon_list[GSTK_MAX_MENU_ITEMS];
    /**< Qualifier from the icon ID list TLV to be populated to each
         gstk_icon_type qualifier element. */
}
gstk_icon_id_list_type;

/*===========================================================================
--  Struct: gstk_at_cmd_type
===========================================================================*/
/**
  @brief Attention command.
 */
typedef struct {
  uint8                        length; /**< Length of the AT command. */
  uint8                      * at_cmd; /**< Pointer to the AT command. */
}
gstk_at_cmd_type;

/*===========================================================================
--  Struct: gstk_duration_type
===========================================================================*/
/**
  @brief Duration information.
 */
typedef struct {
  boolean                      present;
    /**< Indicates whether this structure is optional. */
  uint8                        units;
    /**< Indicates the units:\n
         - GSTK_DURATION_MINUTES
         - GSTK_DURATION_SECONDS
         - GSTK_DURATION_TENTHS_OF_SECONDS */
  uint8                        length;
    /**< Time interval. Supported values: 0 to 255. */
}
gstk_duration_type;

/*===========================================================================
--  Struct: gstk_address_type
===========================================================================*/
/**
  @brief Address information.
*/
typedef struct {
  gstk_ton_enum_type           TON;
    /**< Type of number type. */
  gstk_npi_enum_type           NPI;
    /**< Numbering plan identifier type. */
  uint8                        length;
    /**< Length of the address field. */
  uint8                        address[GSTK_MAX_ADDRESS_LEN];
    /**< Address expressed in hex (i.e., 1 is represented as 0 x 01). */
}
gstk_address_type;

/*===========================================================================
--  Struct: gstk_subaddress_type
===========================================================================*/
/**
  @brief Subaddress information.
*/
typedef struct {
  uint8                        length;
  /**< Length of the subaddress. */
  uint8                        subaddress[GSTK_MAX_SUBADDR_LEN];
  /**< Address expressed in the format that is returned by the card. */
}
gstk_subaddress_type;

/*===========================================================================
--  Struct: gstk_ussd_string_type
===========================================================================*/
/**
  @brief USSD string.
*/
typedef struct {
  uint8                        orig_dcs_from_sim;
    /**< Code scheme Sent by the UICC card. */
  uint8                        dcs;
    /**< As coded in for the cell broadcast. */
  uint8                        length;
    /**< Length of the text field. */
  uint8                      * text;
    /**< Text string. */
}
gstk_ussd_string_type;

/*===========================================================================
--  Struct: gstk_sms_tpdu_type
===========================================================================*/
/**
  @brief SMS transfer protocol data unit information.
*/
typedef struct {
  uint8                        length;
    /**< Length of the SMS TPDU. */
  uint8                      * tpdu;
    /**< Includes the TP-Message Reference.\n
         @note1hang The TP-Message Reference provided by the card might not
         be valid. The ME must check and modify this value accordingly. */
}
gstk_sms_tpdu_type;

/*===========================================================================
--  Struct: gstk_cap_config_type
===========================================================================*/
/**
  @brief Capability configuration information used by the commands.
 */
typedef struct {
  uint8                        length;
  /**< Length of the capability configuration parameters. Refer to
       3GPP TS 24.008 @xhyperref{S2,[S2]}, Section 8.4. */
  uint8                        ccp[GSTK_MAX_CCP_LEN];
  /**< Capability configuration parameters are coded as in the elementary file
       CCP. */
}
gstk_cap_config_type;

/*===========================================================================
--  Struct: gstk_redial_type
===========================================================================*/
/**
  @brief Redial information used by the Set Up Call command.
*/
typedef struct {
  boolean                      necessary;
    /**< Indicates whether the redial information is required. */
  gstk_duration_type           timeout;
    /**< Type of duration from the optional duration TLV. */
}
gstk_redial_type;

/*===========================================================================
--  Struct: gstk_url_type
===========================================================================*/
/**
  @brief URL used by the Launch Browser command.
*/
typedef struct {
    uint8                        length;
      /**< When the length is set to 0, the application must use the default
           URL.*/
    uint8                      * url;
      /**< Coded in the SMS 7-bit default alphabet with bit 8 equal to 0. */
}
gstk_url_type;

/*===========================================================================
--  Struct: gstk_bearer_type
===========================================================================*/
/**
  @brief Bearer information used by the Launch Browser command.
*/
typedef struct {
  uint8                        length;      /**< Length of bearer list. */
  gstk_bearer_code_enum_type * bearer_list; /**< List of bearers. */
}
gstk_bearer_type;

/*===========================================================================
--  Struct: gstk_file_list_type
===========================================================================*/
/**
  @brief File list information used by the Refresh or Launch Browser command.

  Three refresh modes that the UICC can request include: \n
  - FCN
  - USIM Initialization & FCN
  - 3G Session Reset

  For other modes, the GSTK sets the length to 0 to indicate that this
  structure is not required in the Refresh command process.

  This structure is also used as an optional provisioning file reference in the
  Launch Browser command.
*/
typedef struct {
    uint8                        length;
      /**< Total length of the valid data in file_paths. */
    uint8                        num_of_files;
      /**< Number of files. */
    uint8                      * file_paths;
      /**< Full path name for all the files specified from the Refresh command
           (e.g., file_paths = 3F002FE23F007F102F00). */
}
gstk_file_list_type;

/*===========================================================================
--  Struct: gstk_app_id_type
===========================================================================*/
/* Used in Refresh cmd
** when length != 0
**      => Refresh should be performed on the USIM Application indicated
**         in the AID
** when length == 0
**      => Refresh should be performed on the current USIM App
*/
typedef struct
{
   uint8                        aid_len;
   uint8                        aid_data[GSTK_MAX_APP_ID_LEN];
}
gstk_app_id_type;

/*===========================================================================
-- Struct: gstk_generic_data_type
===========================================================================*/
/**
  @brief GSTK generic data type.
*/
typedef struct
{
    uint8  *data_buffer_ptr; /**< Data buffer pointer. */
    int32   data_len;        /**< Length of the data. */
}
gstk_generic_data_type;

/*===========================================================================
--  Struct: gstk_ch_status_type
===========================================================================*/
/**
  @brief Channel status information.
 */
typedef struct {
  int32                                     ch_id;
    /**< Channel ID. */
  boolean                                   is_link_est;
    /**< Indicates whether the link is established or the PDP context
         is activated. */
  boolean                                   is_scws_ch;
    /**< Indicates whether the channel is a SCWS channel. */
  gstk_scws_ch_state_enum_type              scws_ch_status;
    /**< Status of the channel. */
  gstk_ch_status_additional_info_enum_type  ch_status_additional_info;
    /**< Additional channel status information. */
}
gstk_ch_status_type;

/*===========================================================================
--  Struct: gstk_data_avail_evt_ind_type
===========================================================================*/
/**
  @brief Information clients must provide in the Data Available event download.
*/
typedef struct {
  gstk_ch_status_type                ch_status;
    /**< Status of the channel. */
  int32                              ch_len;
    /**< Length of the data available. */
}
gstk_data_avail_evt_ind_type;

/*===========================================================================
--  Struct: gstk_alphabet_type
===========================================================================*/
/**
  @brief Byte arrays with each element represented in an 8-bit UCS-2 coding
  scheme.
*/
typedef struct {
    boolean                      alphabet_given;
      /**< Indicates whether the alphabet type is given. */
    int32                        length;
      /**< Number of uint16 in the alphabet buffer. */
    uint16                     * alphabet;
      /**< Alphabet string. */
}
gstk_alphabet_type;

/*===========================================================================
--  Struct: gstk_csd_bearer_param_type
      This structure contains the parameter information for CSD
      speed:              "speed" subparameter in 27.007
      name:               "name" subparameter in 27.007
      connection_element: "ce" subparameter in 27.007
===========================================================================*/
/**
  @brief CSD parameter information.
*/
typedef struct {
  gstk_csd_bearer_speed_enum_type        speed;
    /**< CSD bearer speed. */
  gstk_csd_bearer_name_enum_type         name;
    /**< CSD bearer name. */
  gstk_csd_bearer_conn_element_enum_type connection_element;
    /**< CSD bearer connection element. */
}
gstk_csd_bearer_param_type;

/*===========================================================================
--  Struct: gstk_gprs_bearer_param_type
      This structure contains the parameter information for GPRS
      precedence_cls:     value corresponding to "precedence" subparameter in 27.007
      delay_cls:          value corresponding to "delay" subparameter in 27.007
      reliability_cls:    value corresponding to "reliability" subparameter in 27.007
      peak_throughput:    value corresponding to "peak" subparameter in 27.007
      mean_throughput:    value corresponding to "mean" subparameter in 27.007
      pkt_data_protocol:  GSTK_PKT_DATA_IP or GSTK_PKT_DATA_RFU
===========================================================================*/
/**
  @brief GPRS parameter information based on 3GPP TS 27.007 @xhyperref{S4,[S4]}.
*/
typedef struct {
  int32                                precedence_cls;
    /**< Precedence class. */
  int32                                delay_cls;
    /**< Delay class. */
  int32                                reliability_cls;
    /**< Reliability class. */
  int32                                peak_throughput;
    /**< Peak throughput. */
  int32                                mean_throughput;
    /**< Mean throughput. */
  gstk_packet_data_protocol_enum_type  pkt_data_protocol;
    /**< GSTK_PKT_DATA_IP or GSTK_PKT_DATA_RFU. */
}
gstk_gprs_bearer_param_type;

/*===========================================================================
--  Struct: gstk_eutran_ext_param_type
      This structure contains the parameter information for GPRS
        traffic_class: the "Traffic class" subparameter, defined in TS 27.007
        max_bitrate_ul: the "Maximum bitrate UL" subparameter, defined in TS 27.007
        max_bitrate_dl: the "Maximum bitrate DL" subparameter, defined in TS 27.007
        guaranteed_bitrate_ul: the "Guaranteed bitrate UL" subparameter, defined in TS 27.007
        guaranteed_bitrate_dl: the "Guaranteed bitrate DL" subparameter, defined in TS 27.007
        delivery_order: the "Delivery order" subparameter, defined in TS 27.007
        max_sdu_size: the "Maximum SDU size" subparameter, defined in TS 27.007
        sdu_error_ratio: the "SDU error ratio" subparameter, defined in TS 27.007
        residual_bit_error_ratio: the "Residual bit error ratio" subparameter, defined in TS 27.007
        delivery_of_err_sdu: the "Delivery of erroneous SDUs" subparameter, defined in TS 27.007
        transfer_delay: the "Transfer delay" subparameter, defined in TS 27.007
        traffic_handling_pri: the "Traffic handling priority" subparameter, defined in TS 27.007
        pdp_type: the "PDP_type" subparameter, defined in TS 27.007
===========================================================================*/
/**
  @brief GPRS parameter information based on 3GPP TS 27.007 @xhyperref{S4,[S4]}.
*/
typedef struct {
  uint8                                traffic_class;
    /**< Traffic class. */
  uint32                               max_bitrate_ul;
    /**< Maximum uplink bitrate. */
  uint32                               max_bitrate_dl;
    /**< Maximum downlink bitrate. */
  uint32                               guaranteed_bitrate_ul;
    /**< Guaranteed uplink bitrate. */
  uint32                               guaranteed_bitrate_dl;
    /**< Guaranteed downlink bitrate. */
  uint8                                delivery_order;
    /**< Delivery order. */
  uint32                               max_sdu_size;
    /**< Maximum SDU size. */
  uint8                                sdu_error_ratio;
    /**< SDU error ratio. */
  uint8                                residual_bit_error_ratio;
    /**< Residual bit error ratio. */
  uint8                                delivery_of_err_sdu;
    /**< Delivery of error SDU. */
  uint32                               transfer_delay;
    /**< Transfer delay. */
  uint32                               traffic_handling_pri;
    /**< Traffic handling priority. */
  gstk_packet_data_protocol_enum_type  pdp_type;
    /**< PDP type. */
}
gstk_eutran_ext_param_type;

/*===========================================================================
--  Struct: gstk_eutran_mapped_utran_ps_param_type
      This structure contains the parameter information for GPRS
        qci: byte 3 of EPS quality of service defined in 24.301
        max_bitrate_ul: byte 4 of EPS quality of service defined in 24.301
        max_bitrate_dl: byte 5 of EPS quality of service defined in 24.301
        guaranteed_bitrate_ul: byte 6 of EPS quality of service defined in 24.301
        guaranteed_bitrate_dl: byte 7 of EPS quality of service defined in 24.301
        max_bitrate_ul_ext: byte 8 of EPS quality of service defined in 24.301
        max_bitrate_dl_ext: byte 9 of EPS quality of service defined in 24.301
        guaranteed_bitrate_ul_ext: byte 10 of EPS quality of service defined in 24.301
        guaranteed_bitrate_dl_ext: byte 11 of EPS quality of service defined in 24.301
        pdp_type: the "PDP_type" subparameter, defined in TS 27.007
===========================================================================*/
/**
  @brief GPRS parameter information based on 3GPP TS 24.301 @xhyperref{S3,[S3]}.
*/
typedef struct {
  uint8                                qci;
    /**< QoS class identifier. */
  uint8                                max_bitrate_ul;
    /**< Maximum uplink bitrate. */
  uint8                                max_bitrate_dl;
    /**< Maximum downlink bitrate. */
  uint8                                guaranteed_bitrate_ul;
    /**< Guaranteed uplink bitrate. */
  uint8                                guaranteed_bitrate_dl;
    /**< Guaranteed downlink bitrate. */
  uint8                                max_bitrate_ul_ext;
    /**< Maximum uplink bitrate (extension). */
  uint8                                max_bitrate_dl_ext;
    /**< Maximum downlink bitrate (extension). */
  uint8                                guaranteed_bitrate_ul_ext;
    /**< Guaranteed uplink bitrate (extension). */
  uint8                                guaranteed_bitrate_dl_ext;
    /**< Guaranteed downlink bitrate (extension). */
  gstk_packet_data_protocol_enum_type  pdp_type;
    /**< PDP type. */ /* this one is based on TS 27.007 */
}
gstk_eutran_mapped_utran_ps_param_type;

/*===========================================================================
--  Struct: gstk_bearer_description_type
      This structure contains the bearer type and bearer parameters
      bearer_type:  GSTK_CSD, GSTK_GPRS or GSTK_DEFAULT
        GSTK_CSD: bearer_params.csd_bearer should be looked at in establishing the
                  bearer for the transport protocol
        GSTK_GPRS: bearer_params.gprs_bearer should be looked at in establishing the
                   bearer for the transport protocol
        GSTK_EUTRAN_EXT_PARAM: bearer_params.eutran_ext_bearer should be looked
                   at in establishing the bearer for the transport protocol
        GSTK_EUTRAN_MAPPED_UTRAN_PS: bearer_params.eutran_mapped_utran_ps_bearer
                   should be looked at in establishing the bearer for the
                   transport protocol
        GSTK_DEFAULT: mobile should use default bearer for thet transport
                      protocol specified.  There is no member in the union that
                      mobile needs to look at in related to the bearer
===========================================================================*/
/**
  @brief Bearer type and parameters.
*/
typedef struct {
  gstk_bearer_code_enum_type               bearer_type;
    /**< Bearer type. Supported values: GSTK_CSD, GSTK_GPRS, and GSTK_DEFAULT. */

  /** Bearer parameters. */
  union {
    gstk_csd_bearer_param_type             csd_bearer;
      /**< CSD bearer parameters. */
    gstk_gprs_bearer_param_type            gprs_bearer;
      /**< GPRS bearer parameters. */
    gstk_eutran_ext_param_type             eutran_ext_bearer;
      /**< E-UTRAN extended bearer parameters. */
    gstk_eutran_mapped_utran_ps_param_type eutran_mapped_utran_ps_bearer;
      /**< E-UTRAN mapped UTRAN PS bearer parameters. */
  } bearer_params;
}
gstk_bearer_description_type;

/*===========================================================================
--  Struct: gstk_data_address_type
===========================================================================*/
/**
  @brief Packet data address.
*/
typedef struct {
  int32                        length;
    /**< Length of the address buffer. */
  uint8                        *address;
    /**< For address type GSTK_PKT_DATA_IPV4_ADDRESS, the 4-byte IP4
         address is given with bit 8 of the first byte being the most
         significant bit of the IP address.

         For address type GSTK_PKT_DATA_IPV6_ADDRESS, the 16-byte IP6
         address is given with bit 8 of the first byte being the most
         significant bit of the IP address. */
}
gstk_data_address_type;

/*===========================================================================
--  Struct: gstk_dns_addrs_type
===========================================================================*/
/**
  @brief DNS addresses.
*/
typedef struct {
  int32                                   num_addresses;
     /**< Number of DNS addresses included. */
  gstk_data_address_type                  *addresses;
     /**< DNS addresses array */
}
gstk_dns_addrs_type;

/*===========================================================================
--  Struct: gstk_pkt_data_address_type
===========================================================================*/
/**
  @brief Packet data address information.
*/
typedef struct {
  gstk_pkt_data_address_enum_type     address_type;
    /**< Address types. Supported values:\n
         - GSTK_PKT_DATA_DYNAMIC_ADDRESS (no information for pkt_address)
         - GSTK_PKT_DATA_IPV4_ADDRESS or GSTK_PKT_DATA_IPV6_ADDRESS
         - GSTK_PKT_DATA_NO_ADDRESS_GIVEN */
  gstk_data_address_type              pkt_address;
    /**< Bearer parameter for GSTK_CSD. */
}
gstk_pkt_data_address_type;

/*===========================================================================
--  Struct: gstk_pkt_data_transport_level_type
===========================================================================*/
/**
  @brief Transport protocol and port number.
*/
typedef struct {
  gstk_pkt_data_transport_level_enum_type   transport_protocol;
    /**< Transport protocols. Supported values:\n
         - GSTK_PKT_DATA_UDP or GSTK_PKT_DATA_TCP
         - GSTK_PKT_DATA_NOT_PRESENT
         - GSTK_PKT_DATA_MAX (port number is invalid in this case) */
  uint32                                    port_number;
    /**< Port number. */
}
gstk_pkt_data_transport_level_type;

/*===========================================================================
--  Struct: gstk_ch_data_type
===========================================================================*/
/**
  @brief Channel data.
*/
typedef struct
{
    int32                data_len; /**< Length of the data pointer. */
    uint8              * data;     /**< Pointer to the data. */
}
gstk_ch_data_type;
/** @} */ /* end_addtogroup genStructUSAT */

/** @addtogroup termRspStructs
  @{
*/

/*---------------------------------------- Terminal Response related structures ------------------------------*/

/* structures used when communication goes from ME -> SIM/USIM */


/*===========================================================================
--  Struct: gstk_additional_info_type
===========================================================================*/
/**
  @brief Additional information used by all terminal responses that use stack
  memory.
*/
typedef struct {
    uint8                      length;
      /**< When there is no additional information, the length is set to 0. */
    uint8                      additional_info[GSTK_MAX_ADDITIONAL_INFO];
      /**< Value can be directly from the
           #gstk_additional_info_code_enum_type enumerator type
           or from a cause, result, or error value from the network. */
}
gstk_additional_info_type;

/** @} */ /* end_addtogroup termRspStructs */

/** @addtogroup genStructUSAT
  @{
*/
/*===========================================================================
--  Struct: gstk_additional_info_ptr_type
===========================================================================*/
/**
  @brief Additional information used by all terminal responses that use heap
  memory.
 */
typedef struct {
    uint8                      length;
      /**< When there is no additional information, the length is set to 0. */
    uint8                    * additional_info_ptr;
      /**< Value can be directly from the #gstk_additional_info_code_enum_type
           enumerator type or from a cause, result, or error value from the
           network. */
}
gstk_additional_info_ptr_type;

/*===========================================================================
--  Struct: gstk_cc_voice_type
===========================================================================*/
/**
  @brief  Voice call control information used in terminal and envelope responses.
*/
typedef struct {
  gstk_address_type            address;    /**< Address. */
  gstk_subaddress_type         subaddress; /**< Subaddress. */
  gstk_cap_config_type         ccp1;       /**< Capability configuration parameter 1. */
  gstk_cap_config_type         ccp2;       /**< Capability configuration parameter 2. */
}
gstk_cc_voice_type;

/** @} */ /* end_addtogroup genStructUSAT */

/** @addtogroup proactCmdStructs
  @{
*/

/*===========================================================================
--  Struct: gstk_pdp_context_act_param_type
===========================================================================*/
/**
  @brief PDP context activation information.
*/
typedef struct {
  int32          length;
    /**< Length of the PDP context activation parameter field. */
  uint8          pdp_act_param[GSTK_MAX_PDP_ACT_PARAM_SIZE];
    /**< PDP context activation information. */
}
gstk_pdp_context_act_param_type;

/*===========================================================================
--  Struct: gstk_eps_pdn_conn_act_param_type
===========================================================================*/
/**
  @brief EPS activation parameter.
*/
typedef struct {
  int32          length;
    /**< Length of the EPS PDN connection activation parameter field. */
  uint8          eps_act_param[GSTK_MAX_PDP_ACT_PARAM_SIZE];
    /**< EPS PDN connection activation information. */
}
gstk_eps_pdn_conn_act_param_type;

/*===========================================================================
--  Struct: gstk_ims_uri_type
===========================================================================*/
/**
  @brief IMS call SIP invite parameter
*/
typedef struct {
  int32          length;
    /**< Length of the IMS URI. */
  uint8          ims_uri[GSTK_MAX_IMS_CALL_PARAM_SIZE];
    /**< IMS call SIP invite message parameter. */
} gstk_ims_call_param_type;

/** @} */  /* end_addtogroup proactCmdStructs */

/** @addtogroup envCmdStructs
  @{
*/
/*===========================================================================
--  Struct: gstk_cc_req_action_type
===========================================================================*/
/**
  @brief Call control request action information used in terminal and envelope
  responses.
*/
typedef struct {
  uint8                        length;
    /**< Length of the Call Control envelope command value. */
  gstk_call_enum_type          call_type;
    /**< When call_type is GSTK_NONE, there is no optional gstk_address_string,
         which means there is no change in the call type, address, SS string,
         or USSD string. */
  union {
    gstk_cc_voice_type                voice_info;
      /**< Sets up a call. */
    gstk_address_type                 ss_string;
      /**< Sends the SS. */
    gstk_ussd_string_type             ussd_string;
      /**< Sends the USSD. */
    gstk_pdp_context_act_param_type   pdp_context_param;
      /**< PDP context parameter. */
    gstk_eps_pdn_conn_act_param_type  eps_conn_act;
      /**< EPS connection activation parameter. */
    gstk_ims_call_param_type          ims_call;
      /**< EPS connection activation parameters. */
    }gstk_address_string;
      /**< Address string. */
    gstk_string_type             alpha;
      /**< Alpha information to be displayed. */
    gstk_bc_repeat_ind_enum_type bc_repeat_ind;
      /**< Avaliable only when CCP2 is presented. */
    /* section 7.3.1.6 response data */
}
gstk_cc_req_action_type;
/** @} */ /* end_addtogroup envCmdStructs */

/** @addtogroup envRspStructs
  @{
*/

/*---------------------- envelop Response related structures, from Card to ME --------------------------*/

/*===========================================================================
--  Struct: gstk_transaction_id_type
===========================================================================*/
/**
  @brief Transaction ID used in the MT Call, Call Connected, and Call
  Disconnected event download commands. */
typedef struct{
    uint8                        length;
      /**< Length of the transaction list. */
    uint8                      * transaction_list;
      /**< List of transaction IDs. Each transaction ID must not appear more
           than once in the list.\n
           - Bits 1-4 -- RFU
           - Bits 5-7 -- TI value
           - Bits 8 -- TI flag

           The TI value and flag are coded as defined in 3GPP TS 24.007
           @xhyperref{S4,[S4]}. */
}
gstk_transaction_id_type;

/*===========================================================================
--  Struct: gstk_cause_type
===========================================================================*/
/**
  @brief Cause information used in the MT event download command.
*/
typedef struct {
    boolean                      present;
      /**< Indication of the presence of cause. */
    uint8                        length;
      /**< Radio link timeout when the length value is 0. */
    uint8                      * cause;
      /**< Coded according to the cause call control information element
           in 3GPP TS 24.008 @xhyperref{S2,[S2]}, starting at octet 3 (i.e.,
           use the length and IEI information from the specification).*/
}
gstk_cause_type;
/** @} */ /* end_addtogroup envRspStructs  */

/** @addtogroup termRspStructs
  @{
*/
/*---------------------------------------------------------------
            Upper Layers <-> GSTK Interface Definitions
----------------------------------------------------------------*/

/*---------------------------------------- Terminal Response related structures ------------------------------------
------------------------------------------ used in the gsdi_send_terminal_response()------------------------------*/


/*=============================================================================
--  Struct: gstk_call_ss_rsp_extra_param_type
=============================================================================*/
/**
  @brief Extra parameter information used when sending a terminal response to a
  Set Up Call or Send SS proactive command request.
*/
typedef struct {
  boolean                             has_cc_type_modification;
    /**< Indicates whether this structure is required in the terminal response.
         Only required when the type of call has been changed. */
  gstk_general_result_enum_type       command_result2;
    /**< Results of the new call type response from the network. */
  gstk_additional_info_ptr_type       additional_info2_ptr;
    /**< Addition information pointer. */
  gstk_string_type                    text_string2;
    /**< Only required when the SIM/UICC converts Set Up Call to Send USSD. */
}
gstk_call_ss_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_get_inkey_rsp_extra_param_type
===========================================================================*/
/**
  @brief Extra parameter information used when sending a terminal response to a
  Get Inkey proactive command request.
 */
typedef struct {
  gstk_string_type   user_get_inkey_input;
    /**< User input. */
  gstk_duration_type duration_of_exec;
    /**< Duration of the command execution. */
}
gstk_get_inkey_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_ussd_rsp_extra_param_type
===========================================================================*/
/**
  @brief Extra parameter information used when sending a terminal response to a
  Send USSD proactive command request.
 */
typedef struct {
  boolean                             has_cc_type_modification;
    /**< Indicates whether this structure is required in the terminal response.
         Only required when the call type has been changed. */
  gstk_string_type                    data;
    /**< User input. */
  gstk_general_result_enum_type       command_result2;
    /**< Results of the new call type response from the network. */
  gstk_additional_info_ptr_type       additional_info2_ptr;
    /**< Addition information pointer. */
}
gstk_ussd_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_provide_lang_rsp_extra_param_type
===========================================================================*/
/**
  @brief Extra parameter information used when sending a terminal response to a
  Provide Local Information for Language proactive command request.
*/
typedef struct {
   uint8                                lang_code[2];
     /**< Language code as defined in spec. */
}
gstk_provide_lang_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_open_ch_rsp_extra_param_type
===========================================================================*/
/**
  @brief Extra parameter information used when sending a terminal response to
  an Open Channel proactive command request.
*/
typedef struct {
  gstk_ch_status_type                     ch_status;
    /**< Channel status. */
  gstk_bearer_description_type            bearer_description;
    /**< Bearer description. */
  int32                                   buffer_size;
    /**< Size of the buffer. */
  gstk_dns_addrs_type                     dns_addrs;
    /**< DNS addresses. */
}
gstk_open_ch_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_close_ch_rsp_extra_param_type
===========================================================================*/
/**
  @brief Extra parameter information used when sending a terminal response to
  an Close Channel proactive command request.
*/
typedef struct {
  gstk_ch_status_type                     ch_status;
    /**< Channel status. */
}
gstk_close_ch_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_receive_data_rsp_extra_param_type
===========================================================================*/
/**
  @brief Extra parameter information used when sending a terminal response to a
  Receive Data proactive command request.
*/
typedef struct {
   gstk_ch_data_type                       ch_data;
     /**< Channel data. */
   int32                                   ch_data_remaining_len;
     /**< Remaining length of the channel data. */
}
gstk_receive_data_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_send_data_rsp_extra_param_type
===========================================================================*/
/**
  @brief Extra parameter information used when sending a terminal response to a
  Send Data proactive command request.
*/
typedef struct {
   int32                                   ch_data_remain_buf_len;
     /**< Remaining buffer length of the channel data. */
}
gstk_send_data_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_get_ch_status_rsp_extra_param_type
===========================================================================*/
/**
  @brief Extra parameter information used when sending a terminal response to a
  Get Channel Status proactive command request.
*/
typedef struct {
   int32                                  cnt;
     /**< Number of the channel status included. */
   gstk_ch_status_type                    ch_status[GSTK_MAX_GPRS_CHANNELS];
     /**< Status of the channel. */
}
gstk_get_ch_status_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_terminal_rsp_extra_param_type

      run_at_response; (future )
      setup_call_extra_param, send_ss_extra_param, send_ussd_extra_param:
       the following extra param is needed when cc response changed the call
       type except in the gstk_ussd_rsp_extra_param_type, which include a
       mandatory ussd string type for the regular send USSD command
       most of the information in these structures are passed in from
       the GSTK upon CC responses, upon the client sends the new call
       request to the network and after receiving the response, it will
       require to send both this information plus the new call type's
       network response back to GSTK
===========================================================================*/
/**
  @brief Extra parameters in the terminal response.

  Follow are proactive commands that require extra parameters in the terminal
  response according the standard:\n
  - Poll Interval -- Actual polling interval used by the ME.
  - Get Inkey -- User input.
  - Get Input -- User input.
  - Select Item -- User selection.
  - Provide Local Information -- Local information based on the specific
    information requested.
  - Run AT Command -- Run AT response.
  - Set Up Call -- Only required when call control has changed Set Up Call to
    Send SS or Send USSD.
  - Send SS -- Only required when call control has changed Send SS to Set Up
    Call or Send USSD.
  - Send USSD -- Only required when call control has changed Send USSD to Set Up
    Call or Send SS.

  Because the GSTK processes some of the commands internally, clients are only
  required to provide extra parameters for:\n
  - Get Inkey
  - Get Input
  - Select Item
  - Run AT Command (future)
  - Set Up Call
  - Send SS
  - Send USSD

  Local information is handled in the GSTK internally. only one type of local
  information can be requested from the SIM/USIM at one time because the
  information is specified in the command detail, which is an exclusive OR for
  the information.

*/
typedef struct {
  boolean                                  present;
    /**< Indicates whether there are any extra parameters required. */
  gstk_cmd_enum_type                       rsp_type;
    /**< Indicates which confirmation the union is to use. Supported values:\n
         - GSTK_GET_INKEY_CNF
         - GSTK_GET_INPUT_CNF
         - GSTK_POLL_INTERVAL_CNF
         - GSTK_SELECT_ITEM_CNF
         - GSTK_SEND_SS_CNF
         - GSTK_SEND_USSD_CNF
         - GSTK_SETUP_CALL_CNF
         - GSTK_PROVIDE_LANG_INFO_CNF
         - GSTK_OPEN_CH_CNF
         - GSTK_SEND_DATA_CNF
         - GSTK_RECEIVE_DATA_CNF
         - GSTK_GET_CH_STATUS_CNF */

  /** Extra parameters required for proactive command terminal responses
      currently supported by the GSTK. */
  union {
    gstk_duration_type                            poll_duration;
      /**< Poll duration. */
    gstk_string_type                              user_get_input;
      /**< String type used by users to get input. */
    gstk_get_inkey_rsp_extra_param_type           get_inkey_extra_param;
      /**< Get Inkey extra parameter. */  /* get in key extension */
    uint8                                         select_item_chosen_item;
      /**< Chosen item. */
    gstk_call_ss_rsp_extra_param_type             setup_call_extra_param;
      /**< Set Up Call extra parameters. */
    gstk_call_ss_rsp_extra_param_type             send_ss_extra_param;
      /**< Send SS extra parameters. */
    gstk_ussd_rsp_extra_param_type                send_ussd_extra_param;
      /**< Send USSD extra parameters. */
    gstk_provide_lang_rsp_extra_param_type        provide_lang_info_extra_param;
      /**< Provide Local Information (Language) extra parameters. */
    gstk_battery_state_enum_type                  battery_status_info_extra_param;
      /**< Provide Local Information (Battery) extra parameters. */
    gstk_open_ch_rsp_extra_param_type             open_ch_extra_param;
      /**< Open Channel extra parameters. */
    gstk_close_ch_rsp_extra_param_type            close_ch_extra_param;
      /**< Close Channel extra parameters. */
    gstk_receive_data_rsp_extra_param_type        receive_data_extra_param;
      /**< Receive Data extra parameters. */
    gstk_send_data_rsp_extra_param_type           send_data_extra_param;
      /**< Send Data extra parameters. */
    gstk_get_ch_status_rsp_extra_param_type       get_ch_status_extram_param;
      /**< Get Channel Status extra parameters. */
    }extra_param;
}
gstk_terminal_rsp_extra_param_type;
/** @} */ /* end_termRspStructs */

/** @addtogroup proactCmdStructs
  @{
*/
/*---------------------------------------- Proactive Command related structures ------------------------------*/

/*
** Structure of proactive commands sent from the SIM/USIM to the upper layers.
** _req_type is proactive cmds populated in the GSTK and sent to upper layers
*/

/*===========================================================================
--  Struct: gstk_display_text_req_type
===========================================================================*/
/**
  @brief GSTK parses the Display Text proactive command into this structure.
*/
typedef struct {
  gstk_string_type                   text_string;
    /**< Text string to be displayed. */
  boolean                            high_priority;
    /**< Indicates whether the command is high priority. */
  boolean                            user_control;
    /**< Indicates whether the client is allowed to clear the screen. */
  gstk_icon_type                     icon;
    /**< Icon data. (Optional) */
  boolean                            immediate_response_required;
    /**< Indicates whether an immediate response is required. */
  gstk_duration_type                 duration;
    /**< Duration of sustained display text. */
}
gstk_display_text_req_type;

/*===========================================================================
--  Struct: gstk_get_inkey_req_type
===========================================================================*/
/**
  @brief GSTK parses the Get Inkey proactive command into this structure.
*/
typedef struct {
  gstk_string_type                   text_string;
    /**< Title to be displayed. */
  gstk_input_format_enum_type        response_format;
    /**< Response format. Supported values:\n
         - GSTK_YES_NO -- Yes/No value.
         - GSTK_NUMERICAL_ONLY -- Numerical only value.
         - GSTK_SMS_DEF_ALPHABET_UNPACKED -- Packed or unpacked in the Get
           Inkey proactive command because the response is only one character.
         - GSTK_UCS2_ALPHABET -- UCS-2 alphabet value. */
  boolean                            help_available;
    /**< Indicates whether help is available. */
  gstk_icon_type                     icon;
    /**< Icon data. (Optional) */
  gstk_duration_type                 variable_timeout;
    /**< Duration of the timeout variable. */
}
gstk_get_inkey_req_type;

/*===========================================================================
--  Struct: gstk_get_input_req_type
===========================================================================*/
/**
  @brief GSTK parses the Get Input proactive command into this structure.
*/
typedef struct {
  gstk_string_type                   text_string;
    /**< Title to be displayed. */
  gstk_input_format_enum_type        response_format;
    /**< Format of the response. */
  gstk_input_pack_format_enum_type   respone_packing_format;
    /**< Response packing format. Supported values:\n
         - GSTK_NUMERICAL_ONLY -- Numberical only value.
         - GSTK_SMS_DEF_ALPHABET_UNPACKED -- SMS default unpacked value.
         - GSTK_UCS2_ALPHABET -- UCS-2 alphabet value. */
  uint8                              maximum_user_input;
    /**< Maximum number of user input. */
  uint8                              minimum_user_input;
    /**< Minimum number of user input. */
  boolean                            help_available;
    /**< Indicates whether help is available. */
  boolean                            show_user_input;
    /**< Indicates whether the user input is to be echoed:
         - TRUE -- Echos the user input.
         - FALSE -- ME displays asterisks. */
  gstk_string_type                   default_text;
    /**< Default text. (Optional) */
  gstk_icon_type                     icon;
    /**< Icon data. */
}
gstk_get_input_req_type;

/*===========================================================================
--  Struct: gstk_launch_browser_req_type
===========================================================================*/
/**
  @brief GSTK parses the Launch Browser proactive command into this structure.
*/
typedef struct {
  gstk_launch_mode_enum_type         launch_mode;
    /**< Launch mode. */
  gstk_url_type                      url;
    /**< When the length is 0, the application must use the default
         URL. The url is coded in the SMS 7-bit default alphabet with bit 8 equal
         to 0. */
  uint8                              browser_id;
    /**< Default browser (0x00 ). The only value currently supported in the
         standard. (Optional) */
  gstk_bearer_type                   bearer;
    /**< Bearer type. */
  uint32                             num_provisioning_files;
    /**< Number of provisioning files. */
  gstk_file_list_type                provisioning_file_ref[GSTK_MAX_PROVISIONING_FILE_REF];
    /**< Provisioning file references. */
  gstk_string_type                   gateway_proxy_text_string;
    /**< Gateway. */
  gstk_string_type                   user_confirm_alpha;
    /**< Confirmation alpha. */
  gstk_icon_type                     user_confirm_icon;
    /**< Confirmation icon. */
}
gstk_launch_browser_req_type;

/*===========================================================================
--  Struct: gstk_play_tone_req_type
===========================================================================*/
/**
  @brief GSTK parses the Play Tone proactive command into this structure.
*/
typedef struct {
  gstk_string_type                   alpha;
    /**< Alpha to be displayed. (Optional) */
  gstk_tone_enum_type                tone;
    /**< Tone type. */
  gstk_duration_type                 duration;
    /**< Duration to play the tone. */
  gstk_icon_type                     icon;
    /**< Icon data. */
}
gstk_play_tone_req_type;

/*===========================================================================
--  Struct: gstk_run_at_cmd_req_type
===========================================================================*/
/**
  @brief GSTK parses the Run AT Command proactive command into this structure.
*/
typedef struct {
  gstk_at_cmd_type                   at_cmd;
    /**< AT command. */
  gstk_string_type                   alpha;
    /**< Alpha to be displayed. (Optional) */
  gstk_icon_type                     icon;
    /**< Icon data. */
}
gstk_run_at_cmd_req_type;

/*===========================================================================
--  Struct: gstk_select_item_req_type
===========================================================================*/
/**
  @brief GSTK parses the Select Item proactive command into this structure.
*/
typedef struct {
  boolean                            help_available;
    /**< Indicates whether help is available. */
  gstk_menu_present_enum_type        presentation;
    /**< Presentation type. */
  boolean                            softkey_selection;
    /**< Indicates whether the selection using soft key is preferred. */
  uint8                              num_items;
    /**< Number of menu items. */
  gstk_item_object_type              items[GSTK_MAX_MENU_ITEMS];
    /**< Menu items. */
  gstk_string_type                   title;
    /**< Title. (Optional) */
  uint8                              default_item;
    /**< Default menu item to be chosen. */  /* optional if value set to 0xFF  */
  gstk_next_item_ind_type            next_action_indicator;
    /**< Next action indicator. */
  gstk_icon_type                     icon;
    /**< Icon data. */
  gstk_icon_id_list_type             icon_id_list;
    /**< Icon ID list. */
}
gstk_select_item_req_type;

/*===========================================================================
--  Struct: gstk_setup_menu_req_type
===========================================================================*/
/**
  @brief GSTK parses the Set Up Menu proactive command into this structure.
*/
typedef struct {
  boolean                            help_available;
    /**< Indicates whether help is available. */
  boolean                            softkey_selection;
    /**< Indicates whether soft key selection is preferred. */
  uint8                              num_items;
    /**< Number of items. */
  gstk_item_object_type              items[GSTK_MAX_MENU_ITEMS];
    /**< Menu items. */
  gstk_string_type                   title;
    /**< Title. (Optional) */
  uint8                              default_item;
    /**< Default item to be chosen. */ /* optional if value set to 0xFF  */
  gstk_next_item_ind_type            next_action_indicator;
    /**< Next action indicator. */
  gstk_icon_type                     icon;
    /**< Icon data. */
  gstk_icon_id_list_type             icon_id_list;
    /**< Icon ID list. */
}
gstk_setup_menu_req_type;

/*===========================================================================
--  Struct: gstk_setup_call_req_type
===========================================================================*/
/**
  @brief GSTK parses the Set Up Call proactive command into this structure.
*/
typedef struct {
  gstk_call_setup_req_enum_type      call_setup_requirement;
    /**< Call setup parameters:\n
         - Hold active calls
         - Terminate active calls
         - No other calls */
  gstk_address_type                  address;
    /**< Address. */
  gstk_redial_type                   redial;
    /**< Indicates whether redial is required. The redial duration is optional. */
  gstk_string_type                   user_cnf_alpha;
    /**< User confirmation alpha. (Optional) */
  gstk_string_type                   setup_call_display_alpha;
    /**< Display phase alpha. */
  gstk_icon_type                     user_cnf_icon;
    /**< User confirmation icon. */
  gstk_icon_type                     setup_call_display_icon;
    /**< Display phase icon. */
  gstk_subaddress_type               destination_subaddress;
    /**< Destination address. */
  gstk_cap_config_type               capability_config;
    /**< Capability configurations. */
}
gstk_setup_call_req_type;

/*===========================================================================
--  Struct: gstk_setup_idle_mode_text_req_type
===========================================================================*/
/**
  @brief GSTK parses the Set Up Idle Mode Text proactive command into this
  structure.
*/
typedef struct {
  gstk_string_type                   text_string;
    /**< Text string to be set. */
  gstk_icon_type                     icon;
    /**< Icon data. (Optional) */
}
gstk_setup_idle_mode_text_req_type;

/*===========================================================================
--  Struct: gstk_send_ss_req_type
===========================================================================*/
/**
  @brief GSTK parses the Send SS proactive command into this structure.
*/
typedef struct {
  gstk_address_type                  ss_string;
    /**< SS string. */
  gstk_string_type                   alpha;
    /**< Alpha to be displayed. (Optional) */
  gstk_icon_type                     icon;
    /**< Icon data. */
}
gstk_send_ss_req_type;

/*===========================================================================
--  Struct: gstk_send_ussd_req_type
===========================================================================*/
/**
  @brief GSTK parses the Send USSD proactive command into this structure.
*/
typedef struct {
  gstk_ussd_string_type              ussd_string;
    /**< USSD string. */
  gstk_string_type                   alpha;
    /**< Alpha to be displayed. (Optional) */
  gstk_icon_type                     icon;
    /**< Icon data. */
}
gstk_send_ussd_req_type;

/*===========================================================================
--  Struct: gstk_send_sms_req_type
===========================================================================*/
/**
  @brief GSTK parses the Send SMS proactive command into this structure.
*/
typedef struct {
  boolean                            packing_required;
    /**< Indicates whether packing is required. */
  gstk_sms_tpdu_type                 sms_tpdu;
    /**< SMS TPDU. */
  boolean                            is_cdma_sms;
    /**< Indicates whether this is CDMA short message format. */
  gstk_address_type                  address;
    /**< Address. */
  gstk_string_type                   alpha;
    /**< Alpha to be displayed. (Optional) */
  gstk_icon_type                     icon;
    /**< Icon data. */
}
gstk_send_sms_req_type;

/*===========================================================================
--  Struct: gstk_refresh_req_type
===========================================================================*/
/**
  @brief GSTK parses the Refresh proactive command into this structure.
*/
typedef struct {
  gstk_file_list_type                 file_list;
    /**< File list */
  gstk_app_id_type                    app_id;
    /**< App ID */
  gstk_generic_data_type              plmn_wact_list;
    /**< PLMN with Access Technology list */
  gstk_string_type                    alpha;
    /**< Alpha to be displayed. (Optional) */
  gstk_icon_type                      icon;
    /**< Icon data. */
}
gstk_refresh_req_type;

/*===========================================================================
--  Struct: gstk_lang_type
===========================================================================*/
/**
  @brief Language code based on 3GPP TS 11.14 @xhyperref{S1,[S1]}.
*/
typedef struct
{
    uint8                              lang_code[GSTK_LANG_CODE_LEN];
      /**< List of lanugage code. */
}
gstk_lang_type;

/*===========================================================================
--  Struct: gstk_dtmf_type
===========================================================================*/
/**
  @brief DTMF information.
*/
typedef struct {
    uint8                              length; /**< Length of the DTMF field. */
    uint8                             * dtmf;  /**< String in BCD format. */
}
gstk_dtmf_type;

/*===========================================================================
--  Struct: gstk_send_dtmf_req_type
===========================================================================*/
/**
  @brief GSTK parses the Send DTMF proactive command into this structure.
*/
typedef struct {
  gstk_dtmf_type                     dtmf;  /**< BCD format. */
  gstk_string_type                   alpha; /**< Alpha to be displayed. */
  gstk_icon_type                     icon;  /**< Icon data. */
}
gstk_send_dtmf_req_type;

/*===========================================================================
--  Struct: gstk_lang_notification_req_type
===========================================================================*/
/**
  @brief GSTK parses the Language Notification proactive command into this
  structure.\n
  @note1hang COMMAND_SUCCESSFUL is to be sent by the device even when lang_code
  is not understood by the device.
*/
typedef struct {
  boolean                            specific_lang_notification;
    /**< Indicates whether the next member of the lang structure is required. */
  gstk_lang_type                     lang;
    /**< Language requested by the SIM/USIM for use during the proactive
       sessions. */
}
gstk_lang_notification_req_type;

/*===========================================================================
--  Struct: gstk_evt_notify_type
===========================================================================*/
/**
  @brief Indicates whether notification is required.
*/
typedef struct {
    boolean                            require_notification;
    /**< Indicates whether notification is required. */
}
gstk_evt_notify_type;

/*===========================================================================
--  Struct: gstk_open_ch_req_type
===========================================================================*/
/**
  @brief GSTK parses the Open Channel proactive command into this structure.
*/
typedef struct {
  boolean                            on_demand_link_est;
    /**< Indicates whether on demand link has been established. */
  gstk_alphabet_type                 alpha;
    /**< Alpha to be displayed. */
  gstk_icon_type                     icon;
    /**< Icon data. */
  gstk_bearer_description_type       bearer;
    /**< Bearer parameters. */
  int32                              buffer_size;
    /**< Buffer size. */
  gstk_alphabet_type                 access_point_name;
    /**< Access point name. */
  gstk_pkt_data_address_type         local_address;
    /**< Local address. */
  gstk_alphabet_type                 text_string_user_login;
    /**< User login. */
  gstk_alphabet_type                 text_string_user_passwd;
    /**< User password. */
  gstk_pkt_data_transport_level_type transport_level;
    /**< Transport level parameters. */
  gstk_pkt_data_address_type         data_dest_address;
    /**< Destination address. */
  boolean                            dns_requested;
    /**< Indicates whether this OPEN CH requests DNS lookup */
}
gstk_open_ch_req_type;

/*===========================================================================
--  Struct: gstk_activate_req_type
===========================================================================*/
/**
  @brief GSTK parses the Activate proactive command into this structure.
*/
typedef struct {
  gstk_target_enum_type                    target;
    /**< Target. */
}
gstk_activate_req_type;

/*===========================================================================
--  Struct: gstk_scws_open_ch_req_type
===========================================================================*/
/**
  @brief Used by the ESTK to send the SCWS Open Channel command to subscribed clients.
*/
typedef struct {
  uint32                                    ch_id;
    /**< Channel ID. */
  int32                                     buffer_size;
    /**< Buffer size. */
  uint32                                    port_number;
    /**< Port number. */
  gstk_alphabet_type                        alpha;
    /**< Alpha. */
}
gstk_scws_open_ch_req_type;

/*===========================================================================
--  Struct: gstk_scws_close_ch_req_type
===========================================================================*/
/**
  @brief Used by the ESTK to send the SCWS Close Channel command to subscribed clients.
*/
typedef struct {
  int32                              ch_id;
    /**< Channel ID. */
  gstk_scws_ch_state_enum_type       channel_state;
    /**< State of the channel. */
}
gstk_scws_close_ch_req_type;

/*===========================================================================
--  Struct: gstk_scws_send_data_req_type
===========================================================================*/
/**
  @brief Used by the ESTK to send the SCWS Send Data command to subscribed clients.
*/
typedef struct {
  int32                              ch_id;
    /**< Channel ID. */
  gstk_ch_data_type                  ch_data;
    /**< Channel data. */
}
gstk_scws_send_data_req_type;

/*===========================================================================
--  Struct: gstk_scws_data_avail_rsp_req_type
===========================================================================*/
/**
  @brief Used by the ESTK to send the SCWS Data Available Response command to
  subscribed clients.
*/
typedef struct {
  int32                              ch_id;
    /**< Channel ID. */
  boolean                            success;
    /**< Indicates whether the request was successful. */
}
gstk_scws_data_avail_rsp_req_type;

/*===========================================================================
--  Struct: gstk_close_ch_req_type
===========================================================================*/
/**
  @brief GSTK parses the Close Channel proactive command into this structure.
*/
typedef struct {
  int32                              ch_id;
    /**< Channel ID. */
  gstk_scws_ch_state_enum_type       ch_state;
    /**< State of the channel. */
  gstk_alphabet_type                 alpha;
    /**< Alphabet is to be displayed. */
  gstk_icon_type                     icon;
    /**< Icon data. */
  uint8                              qualifier;
    /**< Indicates whether this CHANNEL should be kept open */
}
gstk_close_ch_req_type;

/*===========================================================================
--  Struct: gstk_receive_data_req_type
===========================================================================*/
/**
  @brief GSTK parses the Receive Data proactive command into this structure.
*/
typedef struct {
  int32                              ch_id;
    /**< Channel ID. */
  int32                              ch_data_length;
    /**< Channel data length. */
  gstk_alphabet_type                 alpha;
    /**< Alphabet is to be displayed. */
  gstk_icon_type                     icon;
    /**< Icon data. */
}
gstk_receive_data_req_type;

/*===========================================================================
--  Struct: gstk_send_data_req_type
===========================================================================*/
/**
  @brief GSTK parses the Send Data proactive command into this structure.
*/
typedef struct {
  int32                              ch_id;
    /**< Channel ID. */
  boolean                            send_data_immediately;
    /**< Indicates whether data is to be sent immediately. */
  gstk_ch_data_type                  ch_data;
    /**< Channel data. */
  gstk_alphabet_type                 alpha;
    /**< Alpha to be displayed. */
  gstk_icon_type                     icon;
    /**< Icon data. */
}
gstk_send_data_req_type;


/*===========================================================================
--  Struct: gstk_get_ch_status_req_type
===========================================================================*/
/**
  @brief GSTK parses the Get Channel Status proactive command into this
  structure.
*/
typedef struct {
    int32                              ch_id;
    /**< Channel ID. */
}
gstk_get_ch_status_req_type;
/** @} */ /* end_proactCmdStructs */


/*===========================================================================
--  Struct: gstk_terminal_response_type
===========================================================================*/
/**
  @brief Terminal Response for proactive command.
*/
typedef struct {
    gstk_general_result_enum_type general_result;
    /**< General result. */
    gstk_additional_info_type additional_result;
    /**< Additional result. */
    gstk_terminal_rsp_extra_param_type other_info;
    /**< Other info. */
}
gstk_terminal_response_type;

/** @addtogroup envCmdStructs
  @{
*/
/* ----------------------------------- Envelope command type structures, from ME to CARD ------------------------------*/

/* Used in the various send envelope functions */


/*===========================================================================
--  Struct: gstk_mt_call_evt_ind_type
===========================================================================*/
/**
  @brief Client information supplied during the MT Call event download.
*/
typedef struct {
  gstk_transaction_id_type           transaction_id;
    /**< Transaction ID in the SETUP message from the network. */
  gstk_address_type                  address;
    /**< Included when the Calling Party BCD number is received by the ME in
         the SETUP message. */ /* C */
  gstk_subaddress_type               subaddress;
    /**< Included when the Calling Party Subaddress is received by the ME in
         the SETUP message. */
}
gstk_mt_call_evt_ind_type;

/*===========================================================================
--  Struct: gstk_call_connected_evt_ind_type
===========================================================================*/
/**
  @brief Client information supplied during the Call Connected Event Download.
*/
typedef struct {
  gstk_transaction_id_type           transaction_id;
    /**< Transaction ID in the CONNECT message. */
  boolean                            near_end;
    /**< MT call or MO call */
}
gstk_call_connected_evt_ind_type;

/*===========================================================================
--  Struct: gstk_call_disconnected_evt_ind_type
===========================================================================*/
/**
  @brief Client information supplied during the Call Disconnected Event
  Download.
*/
typedef struct {
  gstk_transaction_id_type           transaction_id;
    /**< List of transaction IDs for all calls being disconnected. */
  gstk_cause_type                    cause;
  /**< Reflects the CC-Cause information element sent or received in the
       DISCONNECT, RELEASE or RELEASE COMPLETE message. For a radio link
       timeout, the cause data object is included, with a length value of 0.
       (Optional) */
  boolean                            near_end;
    /**< Near end disconnection or far end disconnection */
}
gstk_call_disconnected_evt_ind_type;

/*===========================================================================
--  Struct: gstk_ims_reg_evt_ind_type
===========================================================================*/
/**
  @brief Information clients must provide in the IMS Registration event download.
*/
typedef struct {
  uint8                              impu_list_len;
    /**< Length of IMPU List */
  uint8                              *impu_list;
    /**< IMPU List */
  uint8                              status_code_len;
    /**< Length of IMS Status-code */
  uint8                              *status_code;
    /**< IMS Status-code */
}
gstk_ims_reg_evt_ind_type;

/*===========================================================================
--  Struct: gstk_evt_dl_ind_type
===========================================================================*/
/**
  @brief Client information supplied in the event download envelope command.
*/
typedef struct {
  gstk_cmd_enum_type                        evt_type;
    /**< Event type. */

  /** Event data. */
  union {
    gstk_mt_call_evt_ind_type               mt_call_evt_ind;
      /**< MT Call event data. */
    gstk_call_connected_evt_ind_type        call_connected_evt_ind;
      /**< Call Connected event data. */
    gstk_call_disconnected_evt_ind_type     call_disconnected_evt_ind;
      /**< Call Disconnected event data. */
    gstk_lang_type                          lang_sel_evt_ind;
      /**< Language Selection event data. */
    gstk_browser_term_cause_enum_type       browser_terminate_evt_ind;
      /**< Browser Termination event data. */
    gstk_data_avail_evt_ind_type            data_avail_evt_ind;
      /**< Data Available event data. */
    gstk_ch_status_type                     ch_status_evt_ind;
      /**< Channel Status event data. */
    gstk_ims_reg_evt_ind_type               ims_reg_evt_ind;
      /**< IMS Registration event data. */
    /* location info indication is being handled within GSTK                  */
    }evt_ind;
}
gstk_evt_dl_ind_type;

/*===========================================================================
--  Struct: gstk_mo_sms_ctrl_ind_type
===========================================================================*/
/**
  @brief Client information supplied in the MO SMS Control command.
*/
typedef struct {
  gstk_address_type                  rp_dest_address;
    /**< Address of the service center to which the ME sends the
         message. */
  gstk_address_type                  tp_dest_address;
    /**< Address to which the ME sends the message. This
         required location information is populated in the GSTK. */
}
gstk_mo_sms_ctrl_ind_type;

/*===========================================================================
--  Struct: gstk_gwl_loc_info
===========================================================================*/
/**
  @brief GSM/WCDMA/LTE location information
*/
typedef struct {
  uint8                        mcc_and_mnc[GSTK_MAX_PLMN_LEN];
      /**< MCC and MNC information. */
  uint8                        loc_area_code[GSTK_MAX_LAC_LEN];
      /**< Location/Tracking area code. */
  uint8                        cell_id[GSTK_MAX_CELL_ID_LEN];
      /**< Cell id information. */
  int32                        cell_id_len;
      /**< Cell id length. */
}
gstk_gwl_loc_info;

/*===========================================================================
--  Struct: gstk_cdma_loc_info
===========================================================================*/
/**
  @brief CDMA location information
*/
typedef struct {
  word                         mcc;
      /**< Mobile Country Code. */
  byte                         imsi_11_12;
      /**< IMSI_11_12. */
  uint16                       sid;
      /**< System Identification number. */
  uint16                       nid;
      /**< Network Identification number. */
  word                         base_id;
      /**< Base station Identification number. */
  int32                        base_lat;
      /**< Base Station latitude. */
  int32                        base_long;
      /**< Base Station longitude. */
}
gstk_cdma_loc_info;


/*===========================================================================
--  Struct: gstk_cc_ind_type_ext
===========================================================================*/
/**
  @brief Client information supplied in the new Call Control API
*/
typedef struct {
  gstk_call_enum_type                call_enum_type;
    /**< Sets the correct call type. */
  gstk_access_technology_type        rat;
    /**< Radio Access Technology. */
  union {
    gstk_gwl_loc_info                gwl_info;
      /**< GSM/WCDMA/LTE location information. */
    gstk_cdma_loc_info               cdma_info;
      /**< CDMA location information. */
  } loc_info;
  union {
    gstk_cc_voice_type               voice;
      /**< Sets up a call. */
    gstk_address_type                ss_string;
      /**< Sends the SS. */
    gstk_ussd_string_type            ussd_string;
      /**< Sends the USSD. */
    gstk_pdp_context_act_param_type  pdp_context;
      /**< PDP context parameters. */
    gstk_eps_pdn_conn_act_param_type eps_conn_act;
      /**< EPS connection activation parameters. */
    gstk_ims_call_param_type         ims_call;
      /**< EPS connection activation parameters. */
  } gstk_address_string;
}
gstk_cc_ind_type_ext;

/*===========================================================================
--  Struct: gstk_mo_sms_ctrl_ind_ext_type
===========================================================================*/
/**
  @brief Client information supplied in the MO SMS Control command.
*/
typedef struct {
  gstk_access_technology_type        rat;
  gstk_address_type                  rp_dest_address;
    /**< Address of the service center to which the ME sends the
         message. */
  gstk_address_type                  tp_dest_address;
    /**< Address to which the ME sends the message. This
         required location information is populated in the GSTK. */
  union {
    gstk_gwl_loc_info                gwl_info;
      /**< GSM/WCDMA/LTE location information. */
    gstk_cdma_loc_info               cdma_info;
      /**< CDMA location information. */
  } loc_info;
}
gstk_mo_sms_ctrl_ind_ext_type;

/** @} */ /* end_addtogroup envCmdStructs */

/** @addtogroup envRspStructs
  @{
*/
/* ----------------------------------- Envelope command responses type structures ------------------------------*/

/* _rsp_type is envelope response from the card to a previously ME issued envelope cmd
*/

/*===========================================================================
--  Struct: gstk_sms_pp_download_ack_type
===========================================================================*/
/**
  @brief Card information returned in response to the SMS PP Download command.
  This structure is used only when the card response acknowledges the SMS PP
  download with certain acknowledgement data.
*/
typedef struct {
    uint8                              length;
      /**< Maximum of 128; based on ETSI TS 102.223 @xhyperref{S7,[S7]}. */
    uint8                              *ack;
      /**< Size of the pointer is based on the length field. */
}
gstk_sms_pp_download_ack_type;

/*===========================================================================
--  Struct: gstk_sms_pp_download_rsp_type
===========================================================================*/
/**
  @brief Card information returned upon receiving the SMS PP Download command.

  The response of the SMS PP Download envelope command might not result in a
  data response from the SIM/USIM. When the SIM/USIM returns a data response, the
  data information is presented in the form of #gstk_sms_pp_download_ack_type.
  When there is no data response, the GSTK indicates the success/failure of the
  SMS PP Download command response as #gstk_general_envelope_rsp_enum_type.
*/
typedef struct {
  gstk_general_envelope_rsp_enum_type general_resp;
    /**< Indicates the success or failure of the SMS PP download response. */
  boolean                             response_data_present;
    /**< Indicates whether the card returned the response data to the SMS PP
         envelope command:\n
         - TRUE -- Response data was returned. Client must look at
                   #gstk_sms_pp_download_ack_type.
         - FALSE -- Response data was not returned. */
  gstk_sms_pp_download_ack_type       ack;
    /**< Type of acknowledgement. */
  uint8                               sw1;
    /**<status word 1 from card>**/
  uint8                               sw2;
    /**<status word 2 from card>**/
}
gstk_sms_pp_download_rsp_type;

/*===========================================================================
--  Struct: gstk_raw_envelope_rsp_type
===========================================================================*/
/**
  @brief Card information returned upon receiving the RAW envelope command.

  The response of the RAW envelope command might not result in a data response
  from the SIM/USIM. When the SIM/USIM returns a data response, the data
  information is presented in rsp_data_ptr.

  There is no payload returned from the envelope response from the card when
  the response is for one of the event download envelopes.
*/
typedef struct {
  gstk_general_envelope_rsp_enum_type general_resp;
    /**< Indicates the success or failure of the RAW Envelope response. */
  uint32                              length;
    /**< Length of rsp_data_ptr. */
  uint8                               *rsp_data_ptr;
    /**< Pointer to the response data. */
  uint8                               sw1;
    /**< Value of SW1 of the response as defined in 3GPP TS 11.11
         @xhyperref{S11,[S11]} for ICC and ETSI TS 102 221
         @xhyperref{S12,[S12]} for UICC. */
  uint8                               sw2;
    /**< Value of SW2 of the response as defined in 3GPP TS 11.11
         @xhyperref{S11,[S11]} for ICC and ETSI TS 102 221
         @xhyperref{S12,[S12]} for UICC. */
} gstk_raw_envelope_rsp_type;

/*===========================================================================
--  Struct: gstk_term_rsp_sw1_sw2_rpt_type
===========================================================================*/
/**
  @brief Reports Status Word 1 and Status Word 2 information from the terminal
  response.
*/
typedef struct {
  uint8 sw1;
    /**< Value of SW1 as defined in 3GPP TS 11.11 @xhyperref{S11,[S11]} for ICC
         and ETSI TS 102 221 @xhyperref{S12,[S12]} for UICC. */
  uint8 sw2;
    /**< Value of SW2 as defined in 3GPP TS 11.11 @xhyperref{S11,[S11]} for ICC
         and ETSI TS 102 221 @xhyperref{S12,[S12]} for UICC. */
} gstk_term_rsp_sw1_sw2_rpt_type;

/*===========================================================================
--  Struct: gstk_mo_sms_ctrl_rsp_type
===========================================================================*/
/**
  @brief Card information returned upon receiving an MO SMS Control request.
*/
typedef struct {
  gstk_general_envelope_rsp_enum_type general_resp;
    /**< Indicates whether the envelope command was successful. */
    /* ENVELOPE SUCCESS or FAIL */
  gstk_control_result_enum_type       mo_sms_ctrl_result;
    /**< MO Short Message Control result. (Optional) */

  gstk_address_type                   rp_dest_address;
    /**< Address of the service center. When not present, there is
         no modification. */
  gstk_address_type                   tp_dest_address;
    /**< Address to which the ME is proposing to send the short message. When
         not present, there is no modification. */
  gstk_string_type                    alpha;
    /**< Alpha to be displayed. */
}
gstk_mo_sms_ctrl_rsp_type;

/*===========================================================================
--  Struct: gstk_cc_rsp_type
===========================================================================*/
/**
  @brief Card information returned upon receiving a Call Control request.
*/
typedef struct {
  gstk_general_envelope_rsp_enum_type general_resp;
    /**< Indicates whether the envelope command was successful. */
    /* ENVELOPE SUCCESS or FAIL */
  gstk_control_result_enum_type       call_ctrl_result;
    /**< Based on the information available, the ME must update the
         information when call_ctrl_result is GSTK_ALLOWED_BUT_MOD. */
  gstk_cc_req_action_type             cc_req_action;
    /**< Call Control action requested. (Optional) */
}
gstk_cc_rsp_type;
/** @} */ /* end_addtogroup envRspStructs */

/** @addtogroup proactCmdStructs
  @{
*/
/*===========================================================================
--  Struct: gstk_end_proactive_req_type
===========================================================================*/
/**
  @brief Indicates whether the End proactive session is generated by the
  GSTK because there is no more Fetch command or generated from the card.
*/
typedef struct {
    boolean                            end_from_card;
    /**< Indicates whether the proactive session ended because of the card. */
}
gstk_end_proactive_req_type;
/** @} */ /* end_addtogroup proactCmdStructs */

/** @addtogroup gstkCmdStructs
  @{
*/
/*===========================================================================
--  Struct: gstk_cmd_from_card_type
===========================================================================*/
/**
  @xreflabel{hdr:gstkCmdFromCardType}
  @brief Commands to be received from the card through the GSTK. This structure
  can be used to get the command ID, command number, and command reference
  number for a more time request.\n
  Poll Interval, Refresh, Set Up Event List events are handled internally in
  the GSTK.
*/
typedef struct {
  gstk_exp_hdr_type                             hdr_cmd;
    /**< Allows the correct casting of the union member. */

  /** Command data. */
  union {
      /*
      ** GSTK_PROACTIVE_CMD_REQ message
      */
      gstk_display_text_req_type              display_text_pro_cmd_req;
        /**< Display Text command data. */
      gstk_get_inkey_req_type                 get_inkey_pro_cmd_req;
        /**< Get Inkey command data. */
      gstk_get_input_req_type                 get_input_pro_cmd_req;
        /**< Get Input command data. */
      gstk_launch_browser_req_type            launch_browser_pro_cmd_req;
        /**< Launch Browser command data. */
      /* more time has only hdr info */
      gstk_play_tone_req_type                 play_tone_pro_cmd_req;
        /**< Play Tone command data. */
      gstk_run_at_cmd_req_type                run_at_cmd_pro_cmd_req;
        /**< RUN AT Command command data. */
      gstk_select_item_req_type               select_item_pro_cmd_req;
        /**< Select Item command data. */
      gstk_send_sms_req_type                  send_sms_pro_cmd_req;
        /**< Send Short Message command data. */
      gstk_refresh_req_type                   refresh_pro_cmd_req;
        /**< Send Refresh command data. */
      gstk_send_ss_req_type                   send_ss_pro_cmd_req;
        /**< Send SS command data. */
      gstk_send_ussd_req_type                 send_ussd_pro_cmd_req;
        /**< Send USSD command data. */
      gstk_setup_call_req_type                setup_call_pro_cmd_req;
        /**< Set Up Call command data. */
      gstk_setup_idle_mode_text_req_type      setup_idle_mode_txt_pro_cmd_req;
        /**< Set Up Idle Mode Text command data. */
      gstk_setup_menu_req_type                setup_menu_pro_cmd_req;
        /**< Set Up Menu command data. */
      gstk_send_dtmf_req_type                 send_dtmf_pro_cmd_req;
        /**< Send DTMF command data. */
      gstk_lang_notification_req_type         lang_notification_pro_cmd_req;
        /**< Language Notification command data. */
      gstk_end_proactive_req_type             end_pro_cmd_req;
        /**< End of Proactive Session command data. */
      gstk_open_ch_req_type                   open_ch_pro_cmd_req;
        /**< Open Channel command data. */
      gstk_close_ch_req_type                  close_ch_pro_cmd_req;
        /**< Close Channel command data. */
      gstk_receive_data_req_type              receive_data_pro_cmd_req;
        /**< Recieve Data command data. */
      gstk_send_data_req_type                 send_data_pro_cmd_req;
        /**< Send Data command data. */
      gstk_get_ch_status_req_type             get_ch_status_pro_cmd_req;
        /**< Get Channel Status command data. */
      gstk_activate_req_type                  activate_pro_cmd_req;
        /**< Activate command data. */
      gstk_scws_open_ch_req_type              scws_open_ch_cmd_req;
        /**< SCWS Open Channel command data. */
      gstk_scws_close_ch_req_type             scws_close_ch_cmd_req;
        /**< SCWS Close Channel command data. */
      gstk_scws_send_data_req_type            scws_send_data_cmd_req;
        /**< SCWS Send Data command data. */
      gstk_scws_data_avail_rsp_req_type       scws_data_avail_rsp_cmd_req;
        /**< SCWS Data Available response data. */

      /* polling interval, polling off, refresh,
         set up event list are handled in the GSTK internally */

      gstk_evt_notify_type                    evt_notify_req;
        /**< As a result of the Set Up Event List command, Idle Screen, User Activity,
             Browser Termination, and Language Select events use this structure to
             notify the client for upcoming envelope command event download
             procedures. */
      /*
      ** GSTK_ENVELOPE_RSP message
      */
      gstk_sms_pp_download_rsp_type           sms_pp_download_rsp;
        /**< SMS PP Download response data. */
      gstk_mo_sms_ctrl_rsp_type               mo_sms_cc_rsp;
        /**< MO Short Message Control response data. */
      gstk_cc_rsp_type                        cc_rsp;
        /**< Call Control response data. */
      gstk_general_envelope_rsp_enum_type     envelope_general_rsp;
        /**< Used for Menu Selection envelope response. */
      gstk_raw_envelope_rsp_type              raw_envelope_rsp;
        /**< RAW envelope response data. */

      /* GSTK Terminal Profile Response message
      */
      gstk_terminal_profile_rsp_enum_type     terminal_profile_rsp;
        /**< Terminal profile download response data. */

      /* GSTK terminal response SW1/SW2 notification message */
      gstk_term_rsp_sw1_sw2_rpt_type          term_rsp_sw1_sw2_rpt;
        /**< Terminal response SW1/SW2 data. */

      /* GSTK terminal response message */
      gstk_terminal_response_type             terminal_response;
      /**< Terminal response data. */
    }cmd;
}
gstk_cmd_from_card_type;

/*===========================================================================
-- Struct: gstk_toolkit_class_cmd_reg_info_type
===========================================================================*/
/**
  @brief Defines a set of proactive commands belonging to a class.
*/
typedef struct
{
  uint32                               class_bit_mask;
    /**< Bitmask identifying a class of proactive commands. Supported values:\n
         - GSTK_REG_CATEGORY_0_CMD
         - GSTK_REG_CATEGORY_B_CMD
         - GSTK_REG_CATEGORY_C_CMD
         - GSTK_REG_CATEGORY_D_CMD
         - GSTK_REG_CATEGORY_E_CMD */
  uint64                               cmd_reg_bit_mask;
    /**< Bitmask identifying a single proactive command belonging to a specific
         class (class_bit_mask). Refer to 3GPP TS 11.14 @xhyperref{S1,[S1]},
         3GPP TS 31.111 @xhyperref{S5,[S5]}, and 3GPP TS 102 223 Annex A
         @xhyperref{S7,[S7]}. */
}gstk_toolkit_class_cmd_reg_info_type;

/*===========================================================================
-- Struct: gstk_toolkit_cmd_reg_info_type
===========================================================================*/
/**
  @brief Client registration. This structure helps the client specify the
  functionality to be processed.
*/
typedef struct
{
  gstk_toolkit_class_cmd_reg_info_type reg_set;
    /**< Bitmask identifying a set of proactive commands belonging to
         a specific class. */
  gstk_tk_functionality_enum_type      client_func_type;
    /**< Valid client function types include UI, MODEM, and ALL. */
}gstk_toolkit_cmd_reg_info_type;

/** @} */  /* end_addtogroup gstkCmdStructs */

/** @addtogroup proactCmdStructs
  @{
*/
/*===========================================================================
-- Struct:  gstk_raw_bytes_hdr_type
===========================================================================*/
/**
  @brief Denotes the header for the RAW proactive commands
  for which a client has registered.
*/
typedef struct
{
  gstk_slot_id_enum_type    sim_slot_id;
    /**< SIM card slot ID. */
  uint32                    cmd_ref_id;
    /**< GSTK command reference. */
  gstk_client_ref_data_type user_data;
    /**< User reference data. */
  gstk_tag_enum_type        tag;
    /**< Differentiates the User Confirmation Alpha or Display Alpha stage for
         the Set Up Call command. */
}gstk_raw_bytes_hdr_type;

/*===========================================================================
-- Struct: gstk_raw_cmd_from_card_type
===========================================================================*/
/**
  @brief Used by the GSTK to send bytes information for the registered
  proactive commands in RAW format as requested by the client.
*/
typedef struct
{
  gstk_raw_bytes_hdr_type   raw_hdr;
    /**< Command header of the RAW data command. */
  gstk_generic_data_type    payload;
    /**< Data sent by the card. */
}gstk_raw_cmd_from_card_type;
/** @} */ /* end_addtogroup proactCmdStructs */


/** @cond
*/
/*===========================================================================
-- Struct: gstk_io_find_client_with_registered_event_in_param_type
===========================================================================*/
/**
  @brief Obtains details about a client registered to a particular event with
  the GSTK. This is the input parameter of gstk_io_ctrl() when the I/O command
  is GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT.
*/
typedef struct
{
  gstk_cmd_enum_type                        cmd_type;
    /**< Command type that the GSTK searches for in the registration table. */
  gstk_client_pro_cmd_data_format_enum_type format_type;
    /**< Format type: RAW or GSTK. */
  gstk_tk_functionality_enum_type           cli_func_type;
    /**< Client functionality type, includes MODEM, UI, and ALL. */
}
gstk_io_find_client_with_registered_event_in_param_type;

/*===========================================================================
-- Struct: gstk_io_set_curr_session_ui_related_in_param_type
===========================================================================*/
/**
  @brief Indicates whether the current session in the GSTK is related to the
  UI. This is the input parameter of gstk_io_ctrl() when the I/O command
  is GSTK_IO_SET_CURR_SESSION_UI_RELATED.
*/
typedef struct
{
  gstk_slot_id_enum_type                    slot;
    /**< Slot ID. */
  boolean                                   is_curr_session_ui_related;
    /**< Indicates whether the current session is related to the UI:\n
         - TRUE -- Current session is related.
         - FALSE -- Current session is not related. */
}
gstk_io_set_curr_session_ui_related_in_param_type;

/*===========================================================================
-- Struct:  gstk_io_call_client_evt_cb_param_type
===========================================================================*/
/**
  @brief Finds and calls the callback of the GSTK client. This is the input
  parameter of gstk_io_ctrl() function when I/O command
  is GSTK_IO_CALL_CLIENT_EVT_CB.
*/
typedef struct
{
  gstk_client_id_type                    client_id;
    /**< Client ID. */
  gstk_cmd_from_card_type                *gstk_req_ptr;
    /**< Pointer to command information sent by the card. */
}
gstk_io_call_client_evt_cb_param_type;
/** @endcond */

/*===========================================================================
-- Struct: gstk_client_pro_cmd_reg_data_type
===========================================================================*/
/**
  @brief Used by the GSTK to send the bytes information for the registered
  proactive commands in the format requested by the client.
*/
typedef struct
{
  gstk_client_pro_cmd_data_format_enum_type data_format_type;
    /**< Data format type. Supported values: RAW and GSTK. */

  /** Proactive command registration information. */
  union
  {
    gstk_raw_cmd_from_card_type  *raw_req_rsp_ptr;
      /**< Callback parameter to receive the command in RAW format. */
    gstk_cmd_from_card_type      *gstk_req_rsp_ptr;
      /**< Callback parameter to receive the command in GSTK format. */
  }pro_cmd_reg_data;
} gstk_client_pro_cmd_reg_data_type;

/*===========================================================================
--  Struct: gstk_service_info_type
===========================================================================*/
/**
  @brief Service table entry information.
*/
typedef struct {
    gstk_service_info_enum_type  service;
      /**< Type of service table entry. */
    gstk_slot_id_enum_type       slot;
      /**< Slot on the SIM/USIM card. */
    boolean                      is_enabled;
      /**< Indicates whether the service is being enabled in the service
           table:\n
           - TRUE -- Service is enabled.
           - FALSE -- Service is not enabled. */
}
gstk_service_info_type;

/** @} */ /* end_addtogroup gstkCmdStructs */

/** @addtogroup gstkCmdStructs
  @{
*/
/*===========================================================================
-- Definition for client init callback function.
===========================================================================*/
/**
  Used by the GSTK to report the status of registration to the client.

  Failure reasons can have the following status:
  - GSTK_INVALID_CLIENT_TYPE -- Invalid client type provided by the client.
  - GSTK_CLIENT_SPACE_FULL -- GSTK has exhausted the client ID and no more
    client IDs can be provided at this point. The client can try again later.
*/
typedef void (*gstk_client_init_cb_func_type)(
  gstk_status_enum_type             gstk_reg_status,
    /**< Returns a valid client ID when the client ID registration is
         successful. Indicates the failure reason when the client ID
         registration fails. */
  gstk_client_id_type               client_id,
    /**< Client ID. */
  gstk_client_ref_data_type         user_data
    /**< User data. */
  )

;

/*===========================================================================
-- Definition for register callback function.
===========================================================================*/
/**
  Used by the GSTK to report the registration status to the client.
  The registration status is applicable to the entire set of commands. If a
  particular command in the client set failed to register, the GSTK does not
  perform registration for any of the commands in the request and returns a
  failure message to the client.
*/
typedef void (*gstk_reg_rsp_cmd_cb_func_type)(
  gstk_status_enum_type             gstk_reg_status,
    /**< Registration status returned. */
  gstk_client_ref_data_type         user_data
    /**< User data. */
  )
;

/*===========================================================================
--  Definition for client callback function for proactive cmd registration
===========================================================================*/
/**
  Used by the GSTK to send proactive commands to the registered client in RAW
  or GSTK format. This callback differs from gstk_evt_cb_funct_type() in that
  the GSTK uses this callback to notify clients of proactive commands
  in the format that the client requests. The gstk_evt_cb_funct_type() callback
  has support for GSTK format only, whereas this callback has support for
  both RAW and GSTK formats.

  When this callback is invoked in the GSTK context, the client must
  copy the command into its buffer/queue as soon as possible.

  For GSTK format, the client follows the same parsing instructions as for
  gstk_evt_cb_funct_type().

  For RAW format, the client gets the proactive command information in
  RAW byte format.

  With either of these formats, the client copies the information and lets the
  callback return immediately because the callback is called in the GSTK
  context.
*/
typedef void (*gstk_toolkit_evt_cb_func_type)(
  gstk_client_pro_cmd_reg_data_type *req_rsp_ptr
    /**< Proactive command information passed into the client's callback. */
);

/*===========================================================================
--  Definition for callback function.
===========================================================================*/
/**
  Upon receiving the events, the client must copy the command into its
  buffer/queue as soon as possible. The length information is encoded in
  gstk_exp_hdr_type, and the callback function has a relatively short execution
  time because it is still running in the GSTK context. In processing
  gstk_cmd_from_card_type eventually, the client can first based on
  #gstk_cmd_enum_type inside gstk_cmd_from_card_type, gstk_exp_hdr_type
  structure to cast the command into the corresponding structure for further
  processing.
*/
typedef void (*gstk_evt_cb_funct_type) (
  gstk_cmd_from_card_type *         req_rsp
    /**< Proactive command information passed into the client's callback. */
);

/* apitrim -end-include */

/** @} */ /* end_addtogroup gstkCmdStructs */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/** @addtogroup clientInit
  @{
*/
/*===========================================================================
-- FUNCTION: GSTK_SLOT_CLIENT_INIT_EXT
===========================================================================*/
/**
  Requests a GSTK client ID for a specified SIM/USIM slot. Two other
  functions (gstk_client_init_ext() and gstk_client_init_ext()) return the
  client ID for SLOT 1 by default.

  @param[in] slot             Slot ID.
  @param[in] client_type      Indicates the type of calling client. Supported
                              values:\n
                              - GSTK_DS_TYPE
                              - GSTK_UI_TYPE
                              - GSTK_ESTK_TYPE
  @param[in] response_cb      Client callback used by the GSTK to return the
                              client ID.
  @param[in,out] client_ref   Client reference data.

  @return
  GSTK_SUCCESS             -- Processing of client_init was successful.\n
  GSTK_INVALID_CLIENT_TYPE -- Client type is invalid.\n
  GSTK_BAD_PARAM           -- Client callback is NULL.\n
  GSTK_MEMORY_ERROR        -- GSTK queue is full.\n
  GSTK_UNSUPPORTED_COMMAND -- Enhanced multiple client support is not turned
                              on.

  @dependencies
  None.
*/
extern gstk_status_enum_type gstk_slot_client_init_ext (
  gstk_slot_id_enum_type         slot,
  gstk_client_type               client_type,
  gstk_client_init_cb_func_type  response_cb,
  gstk_client_ref_data_type      client_ref);

/*===========================================================================
-- FUNCTION: GSTK_CLIENT_INIT_EXT
===========================================================================*/
/**
  Enhancement to the gstk_client_init() function. Here the
  client request to the GSTK for the client ID is treated as an asynchronous call.
  Therefore, the client must provide a valid callback function pointer
  so the GSTK can use it to return the client ID. In functionality, this function
  is the same as the gstk_client_init() function.

  @param[in] client_type      Client type of the calling client. Supported
                              values:\n
                              - GSTK_DS_TYPE
                              - GSTK_UI_TYPE
                              - GSTK_ESTK_TYPE
  @param[in] response_cb      Client callback used by the GSTK to return the
                              client ID.
  @param[in,out] client_ref   Client reference data.

  @return
  GSTK_SUCCESS             -- Processing of client_init is successful.\n
  GSTK_INVALID_CLIENT_TYPE -- Client type is invalid.\n
  GSTK_BAD_PARAM           -- Client callback is NULL.\n
  GSTK_MEMORY_ERROR        -- GSTK queue is full.\n
  GSTK_UNSUPPORTED_COMMAND -- Enhanced multiple client support is not turned
                              on.

  @dependencies
  FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT must be defined.
*/
extern gstk_status_enum_type gstk_client_init_ext (
  gstk_client_type               client_type,
  gstk_client_init_cb_func_type  response_cb,
  gstk_client_ref_data_type      client_ref);

/** @} */ /* end_addtogroup clientInit */

/** @addtogroup clientReg
  @{
*/
/*===========================================================================
-- FUNCTION: GSTK_CLIENT_TOOLKIT_CMD_REG
===========================================================================*/
/**
  Event registration and deregistration for RAW format.

  This new registration function is used by clients to request the GSTK
  to send proactive commands. To receive GSTK events, all tasks can use this
  function to register their callback functions with the GSTK.
  The gstk_evt_cb_funct_type() callback specifies the callback function to be
  called.

  Based on evt_reg_bitmask, the GSTK only sends event/information to
  the client registered for that event. The callback function follows
  the definition stated in gstk_evt_cb_funct_type() and is aware
  of the total processing time, because the callback runs in the GSTK
  context.

  Most of the proactive commands allow only one client registration, this
  prevents multiple terminal responses sent by various clients for the same
  single proactive command. Exceptions to this are More Time and End of
  Proactive Command commands. Without client registration for these
  specific events, the GSTK broadcasts the events to all its clients.

  @param[in] client_id         Client ID.
  @param[in] user_data         Client user data.
  @param[in] data_format_type  Data format.
  @param[in] num               Number of commands for which the client is
                               registering.
  @param[in] reg_list_ptr      Pointer to the registration list.
  @param[in] client_cmd_cb     Immediate callback invoked to reflect the
                               registration status.
  @param[in] client_evt_cb     Callback to receive the command in RAW or
                               GSTK format.

  @return
  GSTK_CLIENT_NOT_REGISTERED -- Client ID is not recognized.\n
  GSTK_BAD_PARAM             -- Input parameter is invalid.\n
  GSTK_NULL_INPUT_PARAM      -- NULL input pointer was provided.\n
  GSTK_SUCCESS               -- Command was queued successfully to the GSTK.

  @dependencies
  The gstk_client_init_ext() function must be called before this function is
  called. When the client deregisters for all commands, the GSTK releases the
  client ID. To reregister, the client must get a new client ID before
  registering for new commands. However, new command registration is to be done
  only at the time of SIM initialization and before the terminal profile
  download is sent by all clients. FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT must
  be defined.

  @sideeffects
  This function can be called multiple times by the same client. This can occur
  when:\n
  - The client has different callbacks for different commands.
  - The client requires some commands in RAW format and some commands in GSTK
    format.

  @par
  All clients are expected to use only this function to register for commands
  in either of the above mentioned formats.\n
  - Same client, multiple registrations -- When a client registers for a command
    in one format by calling this function and then tries to register for the
    same command again in the another format with a second call to the
    function, the second registration for the specific proactive command is
    returned as an error and only the first registration is considered valid by
    the GSTK.
  - Different clients, multiple registrations -- When a client registers for a
    command in a format by calling this function and a second client tries to
    request for the same proactive command by calling this function, the
    registration by the second client for the specific proactive command is
    returned as an error.
  - The End of Proactive Command Session event is broadcast to clients who have
    registered with the GSTK even if the proactive command session is not for
    the command for which the client registered (e.g., only one client
    registers the Set Up Menu command but the client receives the End of
    Proactive Command Session event when the Display Text proactive command
    session ends).
*/
extern gstk_status_enum_type gstk_client_toolkit_cmd_reg(
  gstk_client_id_type                         client_id,
  gstk_client_ref_data_type                   client_user_data,
  gstk_client_pro_cmd_data_format_enum_type   data_format_type,
  uint32                                      num,
  const gstk_toolkit_cmd_reg_info_type        *reg_list_ptr,
  gstk_reg_rsp_cmd_cb_func_type               client_cmd_cb,
  gstk_toolkit_evt_cb_func_type               client_evt_cb
);

/** @} */  /* end_addtogroup clientReg */

/** @addtogroup termRspLib
  @{
*/
/*===========================================================================
FUNCTION GSTK_SEND_TERMINAL_RESPONSE
===========================================================================*/
/**
  Sends a terminal response to a library function.

  This function serves as a utility function to the GSTK client, so that
  the client can call this function in sending a terminal response to
  a proactive command.

  @param[in] client_id          Client ID of the calling client. This value is
                                returned from the gstk_client_init_ext() function.
  @param[in] user_data          User data to be tracked by the GSTK for the
                                client.
  @param[in] cmd_detail_ref_id  Specifies the reference ID for the command
                                details of the proactive command to which the
                                client sends a terminal response. The reference
                                ID value is sent to the client earlier in the
                                structure of the proactive command request,
                                inside gstk_exp_hdr_type.
  @param[in] command_number     Same value as that passed to the client in the
                                corresponding proactive command request.
  @param[in] terminal_response_enum Specifies the confirmation for the response to
                                the proactive command that the client wants to
                                notify the SIM/USIM.
  @param[in] general_result     Specifies the result of the proactive command
                                operation.
  @param[in] additional_result  Pointer to the optional additional result
                                for the proactive command. The length field in
                                #gstk_additional_info_type specifies the number of
                                additional_info in the structure.
  @param[in] other_info         Union pointer to the extra parameters for
                                the proactive command response. See
                                #gstk_terminal_rsp_extra_param_type for more
                                information.

  @return
  GSTK_SUCCESS\n
  GSTK_ERROR\n
  GSTK_CLIENT_NOT_REGISTERED\n
  GSTK_MEMORY_ERROR\n
  GSTK_BAD_PARAM\n
  GSTK_NULL_INPUT_PARAM\n
  GSTK_PARAM_EXCEED_BOUNDARY\n
  GSTK_INVALID_LENGTH\n
  GSTK_EXTRA_PARAM_MISSING\n
  GSTK_EXTRA_PARAM_NOT_REQUIRED\n
  GSTK_UNSUPPORTED_COMMAND\n
  GSTK_UNKNOWN_COMMAND\n
  GSTK_INSUFFICIENT_INFO\n
  GSTK_NOT_INIT

  @dependencies
  The gstk_client_init_ext() function must be called before this function is called.

  @example
  Send a terminal response to a Display Text proactive command:
  @verbatim

#include "gstk_exp.h"

// Assume the client has already obtained a valid client ID from the GSTK,
// and the client has registered to receive the corresponding events.
static gstk_status_enum_type  client_id_from_gstk;
static gstk_cmd_from_card_type cmd;

client_process_terminal_response_function()
{
. . . . . .
gstk_status_enum_type                 gstk_status = GSTK_SUCCESS;
uint32                                user_data = 5000; // Any user data.
gstk_additional_info_type             additional_result;
gstk_terminal_rsp_extra_param_type    other_info;

// Has a valid client ID from the GSTK and has registered events with the GSTK.

// Populate the additional_result.
additional_result.length = 0; // No additional_result.
// Populate extra_param.
extra_param.present = FALSE; // DISPLAY TEXT does not have a extra parameter.

gstk_status = gstk_send_terminal_response(
                    client_id_from_gstk,
                    user_data,
                    cmd.hdr_cmd.cmd_detail_reference,
                    cmd.hdr_cmd.command_number,
                    GSTK_DISPLAY_TEXT_CNF,
                    GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                    &additional_result,
                    &other_info );

    if (gstk_status == GSTK_SUCCESS) {
        // Successfully put the terminal response in the GSTK queue.
    }
    else {
        // Was unable to put the terminal response in the GSTK queue.
    }
}
  @endverbatim
*/
extern gstk_status_enum_type gstk_send_terminal_response (
  gstk_client_id_type                     client_id,
  uint32                                  user_data,
  uint32                                  cmd_detail_ref_id,
  uint8                                   command_number,
  gstk_cmd_enum_type                      terminal_response_enum,
  gstk_general_result_enum_type           general_result,
  const gstk_additional_info_type         *additional_result,
  gstk_terminal_rsp_extra_param_type      *other_info );

/*===========================================================================
-- FUNCTION: GSTK_SEND_TERMINAL_RESPONSE_EXT
===========================================================================*/
/**
  Sends a terminal response library function with an additional information
  pointer type.

  This function serves as a utility function to the GSTK client that enables
  the client to call this function when sending a terminal response to
  a proactive command.

  In this function, the GSTK copies all the information in the parameter
  list into the gstk_cmd type and assigns the correct length,
  cmd_detail_reference, to gstk_exp_hdr_type. The GSTK signals
  GSTK_TERMINAL_RESPONSE so that when the GSTK
  starts processing this command from its command queue, the GSTK knows which
  command it is and the corresponding command details to be used based on
  ref_id.

  This function differs from gstk_send_terminal_response() in that it takes
  gstk_additional_info_ptr_type instead of gstk_additional_info_type
  (i.e., when the client must pass in more than #GSTK_MAX_ADDITIONAL_INFO
  byte of additional information, they can use this utility function.)

  @param[in] client_id          Client ID of the calling client. This value is
                                returned from the gstk_client_init_ext() function.
  @param[in] user_data          User data to be tracked by the GSTK for the
                                client.
  @param[in] cmd_detail_ref_id  Specifies the reference ID for command details
                                of the proactive command to which the client is
                                sending a terminal response. The reference ID
                                value is sent to the client earlier in the
                                structure of the proactive command request,
                                inside gstk_exp_hdr_type.
  @param[in] command_number     Same value as that passed to the client in the
                                corresponding proactive command request.
  @param[in] terminal_response_enum  Specifies the confirmation for the response to
                                the proactive command that the client wants to
                                notify the SIM/USIM.
  @param[in] general_result     Specifies the result of the proactive command
                                operation.
  @param[in] additional_result  Pointer specifies the optional additional result
                                for the proactive command. The length field in
                                gstk_additonal_info_ptr_type specifies the
                                number of additional_info_ptr in the structure.
  @param[in] other_info         Union pointer to the extra parameters for
                                the proactive command response. See
                                #gstk_terminal_rsp_extra_param_type for more
                                information.

  @return
  GSTK_SUCCESS\n
  GSTK_ERROR\n
  GSTK_CLIENT_NOT_REGISTERED\n
  GSTK_MEMORY_ERROR\n
  GSTK_BAD_PARAM\n
  GSTK_NULL_INPUT_PARAM\n
  GSTK_PARAM_EXCEED_BOUNDARY\n
  GSTK_INVALID_LENGTH\n
  GSTK_EXTRA_PARAM_MISSING\n
  GSTK_EXTRA_PARAM_NOT_REQUIRED\n
  GSTK_UNSUPPORTED_COMMAND\n
  GSTK_UNKNOWN_COMMAND\n
  GSTK_INSUFFICIENT_INFO\n
  GSTK_NOT_INIT

  @dependencies
  The gstk_client_init_ext() function must be called before this function is called.

  @example
  Send a terminal response to a Display Text proactive command:
  @verbatim

#include "gstk_exp.h"

// Assume the client has already obtained a valid client ID from the GSTK,
// and the client has registered to receive the corresponding events.
static gstk_status_enum_type  client_id_from_gstk;
static gstk_cmd_from_card_type cmd;

client_process_terminal_response_function()
{
. . . . . .
gstk_status_enum_type                  gstk_status = GSTK_SUCCESS;
uint32                                 user_data = 5000; // Any user data.
gstk_additional_info_ptr_type          additional_result;
gstk_terminal_rsp_extra_param_type     other_info;

// Has a valid client ID from the GSTK, and has registered events with the GSTK.

// Populate additional_result.
additional_result.length = 0; // No additional_result.
// Populate extra_param.
extra_param.present = FALSE; // DISPLAY TEXT does not have an extra parameter.

gstk_status = gstk_send_terminal_response(
                    client_id_from_gstk,
                    user_data,
                    cmd.hdr_cmd.cmd_detail_reference,
                    cmd.hdr_cmd.command_number,
                    GSTK_DISPLAY_TEXT_CNF,
                    GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                    &additional_result,
                    &other_info );

    if (gstk_status == GSTK_SUCCESS) {
      // Successfully put the terminal response in the GSTK queue.
    }
    else {
      // Was unable to put the terminal response in the GSTK queue.
    }
}
  @endverbatim
*/
extern gstk_status_enum_type gstk_send_terminal_response_ext (
  gstk_client_id_type                     client_id,
  uint32                                  user_data,
  uint32                                  cmd_detail_ref_id,
  uint8                                   command_number,
  gstk_cmd_enum_type                      terminal_response_enum,
  gstk_general_result_enum_type           general_result,
  gstk_additional_info_ptr_type          *additional_result,
  gstk_terminal_rsp_extra_param_type     *other_info );

/*===========================================================================
-- FUNCTION: GSTK_SEND_RAW_TERMINAL_RESPONSE
===========================================================================*/
/**
  Sends a RAW terminal response to a proactive command. Applicable proactive
  commands:
  - DISPLAY TEXT
  - GET INKEY
  - GET INPUT
  - PLAY TONE
  - REFRESH
  - SET UP MENU
  - SELECT ITEM
  - SET UP IDLE MODE TEXT
  - SEND DTMF
  - LAUNCH BROWSER

  @param[in] client_id          Client ID of the calling client. This value is
                                returned from the gstk_client_init_ext() function.
  @param[in] user_data          User data to be tracked by the GSTK for the
                                client.
  @param[in] cmd_detail_ref_id  Specifies the reference ID for command details
                                of the proactive command to which the client
                                sends a terminal response. The reference ID
                                value is sent to the client earlier in the
                                structure of the proactive command request,
                                inside gstk_raw_bytes_hdr_type.
  @param[in] term_rsp           RAW terminal response to be sent by the client.

  @return
  GSTK_SUCCESS                -- Terminal response is sent successfully.\n
  GSTK_CLIENT_NOT_REGISTERED  -- Client ID passed in is invalid.\n
  GSTK_BAD_PARAM              -- Input parameter is invalid.\n
  GSTK_MEMORY_ERROR           -- Cannot allocate memory.

  @dependencies
  The gstk_client_init_ext() function must be called before calling this function.
  FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT must be defined.

  @dependencies
  After a client sends a RAW terminal response, it receives the
  GSTK_TERM_RSP_SW1_SW2_RPT event, which is used to report status words (SW1
  and SW2) to the client through the event callback function when
  FEATURE_GSTK_REPORT_SW1_SW2 is enabled. Both
  FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT and
  FEATURE_GSTK_STREAMING_APDU_SUPPORT must be enabled as defined.
*/
extern gstk_status_enum_type gstk_send_raw_terminal_response (
  gstk_client_id_type                     client_id,
  gstk_client_ref_data_type               user_data,
  uint32                                  cmd_detail_ref_id,
  gstk_generic_data_type                  term_resp);

/** @} */  /* end_addtogroup termRspLib */

/** @addtogroup envCmd
  @{
*/
/*===========================================================================
-- FUNCTION: GSTK_SLOT_SEND_ENVELOPE_MO_SMS_CTRL_COMMAND_EXT
===========================================================================*/
/**
  Sends the MO SMS Control to a specific SIM/USIM slot
  within the gstk_mo_sms_ctrl_ind_type structure. The client must supply
  the values for rp_dest_address and tp_dest_address.

  @param[in] slot                 SIM slot ID.
  @param[in] user_data            User data to be tracked by the GSTK for the
                                  client.
  @param[in] mo_sms_control       Pointer to the MO SMS Control information.
  @param[in] gstk_mo_sms_ctrl_cb  Function pointer to which the GSTK sends
                                  the card response data that corresponds to
                                  the MO SMS Control command.

  @return
  #gstk_status_enum_type

  @dependencies
  The gstk_client_init_ext() function must be called before this function is called.
*/
gstk_status_enum_type gstk_slot_send_envelope_mo_sms_ctrl_command_ext (
    gstk_slot_id_enum_type                slot,
    uint32                                user_data,
    const gstk_mo_sms_ctrl_ind_ext_type   *mo_sms_control_data_ptr,
    gstk_evt_cb_funct_type                gstk_mo_sms_ctrl_cb
);

/*===========================================================================
-- FUNCTION: GSTK_SEND_ENVELOPE_EVT_DOWNLOAD_COMMAND
===========================================================================*/
/**
  Sends an event download envelope command library function to the SIM/USIM.

  The client must send evt_list as the parameter to this function.

  The first element of the gstk_evt_dl_ind_type structure is an
  enumerator type that indicates which events are sent to the SIM/USIM. Based
  on the enumerator value, the client must provide the information-related
  to the event indication.

  This function does not require the client ID because it is called by internal
  modules.

  @param[in] client_id  Client ID of the calling client. This value is returned
                        from the gstk_client_init_ext() function.
  @param[in] user_data  User data to be tracked by the GSTK for the client.
  @param[in] evt_list   Pointer to the event download information.

  @return
  GSTK_SUCCESS\n
  GSTK_ERROR\n
  GSTK_CLIENT_NOT_REGISTERED\n
  GSTK_MEMORY_ERROR\n
  GSTK_BAD_PARAM\n
  GSTK_NULL_INPUT_PARAM\n
  GSTK_PARAM_EXCEED_BOUNDARY\n
  GSTK_INVALID_LENGTH\n
  GSTK_UNSUPPORTED_COMMAND\n
  GSTK_UNKNOWN_COMMAND\n
  GSTK_INSUFFICIENT_INFO\n
  GSTK_NOT_INIT

  @dependencies
  The gstk_client_init_ext() function must be called before this function is called.

  @sideeffects
  Upon calling this function, the client can expect to receive
  #gstk_general_envelope_rsp_enum_type in its callback function.

  @example
  Send an Idle Screen Available envelope command:
  @verbatim

#include "gstk_exp.h"

// Assume the client has already obtained a valid client ID from the GSTK,
// and the client has registered to receive the corresponding events.
static gstk_status_enum_type  client_id_from_gstk;

client_process_envelope_event_download_function()
{
. . . . . .
gstk_status_enum_type           gstk_status = GSTK_SUCCESS;
uint32                          user_data = 7745; // Any user data.
gstk_evt_dl_ind_type            evt_list;

// Has a valid client ID from the GSTK, and has registered events with the GSTK.

// Populate evt_list.
evt_list.evt_type = GSTK_IDLE_SCRN_EVT_IND;
// An extra parameter is required for this event list. The union structure in
// evt_list can be ignored.
gstk_status = gstk_send_envelope_evt_download_command (
                     client_id_from_gstk,
                     user_data,
                     &evt_list);
. . . . . .
}
  @endverbatim
*/
extern gstk_status_enum_type gstk_send_envelope_evt_download_command (
  gstk_client_id_type         client_id,
  uint32                      user_data,
  gstk_evt_dl_ind_type        *evt_list );

/** @} */ /* end_addtogroup envCmd */

/** @addtogroup envCmd
  @{
*/
/*===========================================================================
-- FUNCTION: GSTK_SEND_ENVELOPE_MENU_SEL_COMMAND
===========================================================================*/
/**
  Sends a Menu Selection envelope command library function to the SIM/USIM.
  Clients must provide the item identifier for the user selected item.

  @param[in] client_id   Client ID of the calling client. This is the value
                         returned from the gstk_client_init_ext() function.
  @param[in] user_data   User data to be tracked by the GSTK for the client.
  @param[in] item_chosen User selected item.\n
                         - Valid value [0x01, 0xFF].
                         - 0x00 indicates a NULL item identifier.
  @param[in] help_requested  Indicates whether help is requested (Optional):\n
                         - TRUE  -- Help is requested.
                         - FALSE -- Help is not requested and is not
                                    sent to the SIM/USIM.

  @return
  GSTK_SUCCESS\n
  GSTK_ERROR\n
  GSTK_CLIENT_NOT_REGISTERED\n
  GSTK_MEMORY_ERROR\n
  GSTK_BAD_PARAM\n
  GSTK_NULL_INPUT_PARAM\n
  GSTK_PARAM_EXCEED_BOUNDARY\n
  GSTK_INVALID_LENGTH\n
  GSTK_UNSUPPORTED_COMMAND\n
  GSTK_UNKNOWN_COMMAND\n
  GSTK_INSUFFICIENT_INFO\n
  GSTK_NOT_INIT

  @dependencies
  The gstk_client_init_ext() function must be called before this function is
  called.

  @sideeffects
  Upon calling this function, the client can expect to receive
  #gstk_general_envelope_rsp_enum_type in its callback function.

  @example
  Send menu selection envelope command.
  @verbatim

#include "gstk_exp.h"

// Assume the client has already obtained a valid client ID from the GSTK,
// and the client has registered to receive the corresponding events.
static gstk_status_enum_type  client_id_from_gstk;

client_process_menu_selection_function()
{
. . . . . .
gstk_status_enum_type        gstk_status = GSTK_SUCCESS;
uint32                       user_data = 90118; // Any user data.

// Has a valid client ID from the GSTK. and has registered events with the GSTK.

gstk_status = gstk_send_envelope_menu_sel_command (
                          client_id_from_gstk,
                          user_data,
                          1,
                          TRUE );
. . . . . .
}
  @endverbatim
*/
extern gstk_status_enum_type gstk_send_envelope_menu_sel_command (
  gstk_client_id_type          client_id,
  uint32                       user_data,
  uint8                        item_chosen,
  boolean                      help_requested );

/** @} */ /* end_addtogroup envCmd */

/** @addtogroup envCmd
  @{
*/
/*===========================================================================
-- FUNCTION: GSTK_SEND_RAW_ENVELOPE_COMMAND
===========================================================================*/
/**
  Sends an envelope command in RAW format to the SIM, USIM, or RUIM.

  @param[in] client_id   Client ID of the calling client. This is the value
                         returned from the gstk_client_init_ext() function.
  @param[in] user_data   User data to be tracked by the GSTK for the client.
  @param[in] env_cmd     Envelope command data to be sent to the SIM, USIM, or
                         RUIM.
  @param[in] gstk_raw_env_cb  Callback function.

  @return
  GSTK_SUCCESS\n
  GSTK_ERROR\n
  GSTK_CLIENT_NOT_REGISTERED\n
  GSTK_MEMORY_ERROR\n
  GSTK_BAD_PARAM\n
  GSTK_NULL_INPUT_PARAM\n
  GSTK_PARAM_EXCEED_BOUNDARY\n
  GSTK_INVALID_LENGTH\n
  GSTK_UNSUPPORTED_COMMAND\n
  GSTK_UNKNOWN_COMMAND\n
  GSTK_INSUFFICIENT_INFO\n
  GSTK_NOT_INIT

  @dependencies
  The gstk_client_init_ext() function must be called before this function is called.
  #FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT must be defined.

  @sideeffects
  Upon calling this function, the client can expect to receive
  #gstk_raw_envelope_rsp_type in its callback function.
  @par
  When FEATURE_GSTK_REPORT_SW1_SW2 is enabled, the status words (SW1 and SW2)
  are also passed to the client along with the RAW envelope response. Both
  FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT and FEATURE_GSTK_STREAMING_APDU_SUPPORT
  must be enabled.
*/
extern gstk_status_enum_type gstk_send_raw_envelope_command (
  gstk_client_id_type          client_id,
  uint32                       user_data,
  gstk_generic_data_type       env_cmd,
  gstk_evt_cb_funct_type       gstk_raw_env_cb);

/** @} */ /* end_addtogroup envCmd */

/** @addtogroup termProfDnld
  @{
*/
/*===========================================================================
-- FUNCTION: GSTK_UPDATE_PROFILE_DOWNLOAD_VALUE
===========================================================================*/
/**
  Sends a terminal profile command library function.

  This function allows clients to update the terminal profile before the GSTK
  sends it down to the GSDI and UIM server.

  The GSTK sends the profile download events to the card after it receives
  the first profile download update value from one of its clients. The GSTK
  ignores the subsequent profile download updates.

  @param[in] client_id    Client ID of the calling client. This is the
                          value returned from the gstk_client_init_ext() function.
  @param[in] user_data    User data to be tracked by the GSTK for the client.
  @param[in] num_updates  Specifies the number of updates
                          (gstk_profile_dl_support_type).
  @param[in] value_list   Pointer to the list of new updates.

  @return
  GSTK_SUCCESS\n
  GSTK_CLIENT_NOT_REGISTERED\n
  GSTK_MEMORY_ERROR\n
  GSTK_NULL_INPUT_PARAM

  @dependencies
  The gstk_client_init_ext() function must be called before this function is called.
  Following are values returned when the GSTK processes the command after
  dequeuing from the command queue.
  - GSTK_SUCCESS
  - GSTK_ERROR
  - GSTK_CLIENT_NOT_REGISTERED
  - GSTK_PROFILE_VALUE_NOT_ALLOWED
  - GSTK_PROFILE_ALREADY_SENT
  - GSTK_MEMORY_ERROR
  - GSTK_BAD_PARAM
  - GSTK_NULL_INPUT_PARAM
  - GSTK_PARAM_EXCEED_BOUNDARY
  - GSTK_INVALID_LENGTH
  - GSTK_UNSUPPORTED_COMMAND
  - GSTK_UNKNOWN_COMMAND
  - GSTK_INSUFFICIENT_INFO
  - GSTK_FILE_ACCESS_FAIL
  - GSTK_NOT_INIT

  @example
  Send terminal profile updates.
  @verbatim

#include "gstk_exp.h"

// Assume a client has already obtained a valid client ID from the GSTK.
// and the client has registered to receive the corresponding events.
static gstk_status_enum_type  client_id_from_gstk;

client_process_terminal_profile_function()
{
. . . . . .
gstk_status_enum_type              gstk_status = GSTK_SUCCESS;
uint32                             user_data = 90118; // Any user data.
gstk_profile_dl_support_type       value_list[6] ;

// Has a valid client ID from the GSTK, and has registered events with the GSTK.

// Populate the profile download list.
    value_list[0].profile_dl_bitmask = GSTK_NUM_CH_SUPPORT;
    value_list[0].value = 1;
    value_list[1].profile_dl_bitmask = GSTK_SEND_DATA_SUPPORT;
    value_list[1].value = 1;
    . . . . . .
    value_list[5].profile_dl_bitmask = GSTK_SK_SUPPORT_IN_SEL_ITEM;
    value_list[5].value = 0;

gstk_status = gstk_update_profile_download_value (
                           client_id_from_gstk,
                           46,                          // user_data
                           6,                           // num_updates
                           value_list);
   . . . . . .
}
  @endverbatim
*/
extern gstk_status_enum_type gstk_update_profile_download_value (
  gstk_client_id_type                    client_id,
  uint32                                 user_data,
  uint8                                  num_updates,
  const gstk_profile_dl_support_type    *value_list );

/*===========================================================================
-- FUNCTION: GSTK_UPDATE_PROFILE_DOWNLOAD_VALUE_EXT
===========================================================================*/
/**
  @xreflabel{hdr:gstkUpdateProfileDownloadValueExt}
  Allows clients to update the terminal profile before the GSTK sends it to the
  GSDI and the UIM server.

  The GSTK sends the profile download events to the card after it receives
  the first profile download update value from one of its clients. The GSTK
  ignores subsequent profile download updates.

  This function differs from the gstk_update_profile_download_value() function
  in that it supports differentiating between the SAT and USAT terminal profile
  while gstk_update_profile_download_value() does not.

  @param[in] client_id       Client ID of the calling client. This value is
                             returned from the gstk_client_init_ext() function.
  @param[in] user_data       User data to be tracked by the GSTK for the
                             client.
  @param[in] num_updates     Number of updates (gstk_profile_dl_support_type)
                             specified by the client.
  @param[in] value_list_ptr  Pointer to the list of new updates.

  @return
  GSTK_SUCCESS\n
  GSTK_CLIENT_NOT_REGISTERED\n
  GSTK_MEMORY_ERROR\n
  GSTK_NULL_INPUT_PARAM

  @dependencies
  The gstk_client_init_ext() function must be called before this function is
  called.
  Following are values returned when the GSTK processes the command
  after dequeuing from the command queue.
  - GSTK_SUCCESS
  - GSTK_ERROR
  - GSTK_CLIENT_NOT_REGISTERED
  - GSTK_PROFILE_VALUE_NOT_ALLOWED
  - GSTK_PROFILE_ALREADY_SENT
  - GSTK_MEMORY_ERROR
  - GSTK_BAD_PARAM
  - GSTK_NULL_INPUT_PARAM
  - GSTK_PARAM_EXCEED_BOUNDARY
  - GSTK_INVALID_LENGTH
  - GSTK_UNSUPPORTED_COMMAND
  - GSTK_UNKNOWN_COMMAND
  - GSTK_INSUFFICIENT_INFO
  - GSTK_FILE_ACCESS_FAIL
  - GSTK_NOT_INIT
*/
extern gstk_status_enum_type gstk_update_profile_download_value_ext (
  gstk_client_id_type                        client_id,
  uint32                                     user_data,
  uint8                                      num_updates,
  const gstk_profile_dl_support_ext_type    *value_list_ptr );

/** @} */ /* end_addtogroup termProfDnld */

/** @addtogroup gstkUtil
  @{
*/
/*===========================================================================
FUNCTION gstk_map_proactive_req_to_cnf
===========================================================================*/
/**
  Maps the passed in command request type to the corresponding response type.

  @param[in] request_cmd           Original proactive command request to be
                                   mapped.
  @param[in,out] expected_cmd_rsp  Pointer to the response command type (CNF)
                                   to be populated.
  @return
  GSTK_SUCCESS\n
  GSTK_UNSUPPORTED_COMMAND\n
  GSTK_INVALID_COMMAND

  @dependencies
  None.
*/
extern gstk_status_enum_type gstk_map_proactive_req_to_cnf(
                      gstk_cmd_enum_type  request_cmd,
                      gstk_cmd_enum_type *expected_cmd_rsp)  ;

/*===========================================================================
-- FUNCTION: gstk_util_decode_dcs_type
===========================================================================*/
/**
  Determines whether the DCS is 7-bit, 8-bit, or UCS-2, depending
  on whether it is a cell broadcast DCS.

  @param[in] is_cb_dcs Indicates whether the DCS is for USSD.
  @param[in] dcs       Data coding scheme.

  @return
  GSTK_DCS_8_BIT\n
  GSTK_DCS_UCS2_8_BIT\n
  GSTK_DCS_UCS2_7_BIT\n
  GSTK_DCS_7_BIT\n
  GSTK_DCS_NONE\n
  GSTK_DCS_MAX

  @dependencies
  None.
*/
extern gstk_dcs_enum_type gstk_util_decode_dcs_type(boolean is_cb_dcs, uint8 dcs);

/*===========================================================================
-- FUNCTION: gstk_util_unpack_7_bit_chars_ext
===========================================================================*/
/**
  Unpacks the input data from SMS 7-bit format into ASCII format
  and returns the actual data length without the NULL-terminated byte.

  @param[in] in       Pointer to the input data.
  @param[in] in_len   Length of the input data.
  @param[in,out] out  Pointer to the output data.

  @return
  Length of the output data.\n
  A negative value is an error.

  @dependencies
  None.
*/
extern int32 gstk_util_unpack_7_bit_chars_ext
(
  const uint8       * in,
  int32             in_len,
  uint16            * out
);

/*===========================================================================
-- FUNCTION: gstk_decode_h80_usc2_alphabet
===========================================================================*/
/**
  Used by the GSTK to convert a UCS-2 buffer to a uint16 buffer.

  @param[in] in_len           Length of the input buffer.
  @param[in] ucs2_default_in  Pointer to the UCS-2 buffer.
  @param[out] alphabet_out    Pointer to the output buffer of type uint16.

  @return
  Length of the NULL-terminated output buffer.\n
  A negative value is an error.

  @dependencies
  None.
*/
int32 gstk_decode_h80_usc2_alphabet(int32               in_len,
                                    uint8              *ucs2_default_in,
                                    uint16             *alphabet_out);

/*===========================================================================
-- FUNCTION: gstk_decode_h81_usc2_alphabet
===========================================================================*/
/**
  Used by the GSTK to convert a UCS-2 buffer to a uint16 buffer.

  @param[in] in_len           Length of the input buffer.
  @param[in] ucs2_default_in  Pointer to the UCS-2 buffer.
  @param[out] alphabet_out    Pointer to the output buffer of type uint16.

  @return
  Length of the NULL-terminated output buffer.\n
  A negative value is an error.

  @dependencies
  None.
*/
int32 gstk_decode_h81_usc2_alphabet(int32               in_len,
                                    uint8              *ucs2_default_in,
                                    uint16             *alphabet_out);

/*===========================================================================
-- FUNCTION: gstk_decode_h82_usc2_alphabet
===========================================================================*/
/**
  Used by the GSTK to convert a UCS-2 buffer to a uint16 buffer.

  @param[in] in_len           Length of the input buffer.
  @param[in] ucs2_default_in  Pointer to the UCS-2 buffer.
  @param[out] alphabet_out    Pointer to the output buffer of type uint16.

  @return
  Length of the NULL-terminated output buffer.\n
  A negative value is an error.

  @dependencies
  None.
*/
int32 gstk_decode_h82_usc2_alphabet(int32               in_len,
                                    uint8              *ucs2_default_in,
                                    uint16             *alphabet_out);

/*============================================================================
-- FUNCTION: GSTK_CLIENT_ID_FORCE_CLEANUP
========================================================================== */
/**
  Performs cleanup for the client ID when the RPC indicates that
  the application processor client has been terminated.

  @param[in] response_cb_ptr  Callback that maps to the client ID that enables
                              the GSTK to perform the corresponding client ID
                              deregistration for the client.

  @return
  GSTK_SUCCESS             -- Processing of client_init was successful.\n
  GSTK_INVALID_CLIENT_TYPE -- Client type is invalid.\n
  GSTK_BAD_PARAM           -- Client callback is NULL.\n
  GSTK_MEMORY_ERROR        -- GSTK queue is full.\n
  GSTK_UNSUPPORTED_COMMAND -- Enhanced multiple client support is not turned
                              on.

  @dependencies
  None.

  @sideeffects
  Client IDs linked to the corresponding response_cb parameter during the
  gstk_client_init_ext() function are deregistered.
*/
void gstk_client_id_force_cleanup(
  gstk_client_init_cb_func_type response_cb_ptr
);
/** @cond
*/
/*============================================================================
-- FUNCTION: GSTK_UTIL_PARSE_CCP
========================================================================== */
/**
  Parses the capability configuration parameter TLV into
  cm_bearer_capability_T, which is used by the CM.

  @param[in] ccp_ptr           Pointer to the capability configuration
                               parameter data.
  @param[out] bearer_cap_ptr   Pointer to cm_bearer_capability_T.
  @param[in] len               Size of cm_bearer_capability_T.

  @return
  GSTK_SUCCESS   -- Processing of client_init was successful.\n
  GSTK_BAD_PARAM -- Client callback is NULL.\n
  GSTK_ERROR     -- Failed to parse the CCP data.

  @dependencies
  None.

  @sa
  The cm_bearer_capability_T structure definition is in mn_cm_exp.h.
*/
gstk_status_enum_type gstk_util_parse_ccp(
  gstk_cap_config_type      *ccp_ptr,
  uint8                     *bearer_cap_ptr,
  uint32                    len
);
/** @endcond */

/*===========================================================================
-- FUNCTION: gstk_get_card_event_data
===========================================================================*/
/**
  Gets the cached proactive command.

  @param[in] client_id      Indicates the client ID received during client
                            registration with the GSTK.
  @param[in] cmd_ref_id     Command reference ID sent in the event. The client
                            uses the same ID to get the event data.
  @param[in,out] event_ptr  Pointer to the client memory where proactive
                            command details are copied. data_format_type is
                            filled in by the client. The client fills in the
                            format type and allocated memory for the Copy
                            command.

  @return
  GSTK_SUCCESS -- Command is copied without errors.\n
  GSTK_ERROR   -- Otherwise.

  @dependencies
  None.

  @comments
  The client is responsible for freeing the member fields in event_ptr after
  calling this function.\n
  @vertspace
  When the client does not call this function to retrieve the command, the
  internal pointers in event_ptr that are shallow copied are freed while the
  terminal response is sent.
*/
extern gstk_status_enum_type gstk_get_card_event_data(
  gstk_client_id_type                   client_id, /* for future use */
  uint32                                cmd_ref_id,
  gstk_client_pro_cmd_reg_data_type*    event_ptr);

/*===========================================================================
-- FUNCTION: gstk_task
===========================================================================*/
/** @cond
*/
/**
  GSTK main task.

  @param[in] dummy  no use!

  @return
  void

  @dependencies
  None.
*/
extern void gstk_task(dword dummy);
/** @endcond */
/** @} */ /* end_addtogroup gstkUtil */

/** @addtogroup envCmd
  @{
*/
/*=============================================================================
-- FUNCTION: GSTK_SLOT_SEND_ENVELOPE_CC_COMMAND_EXT
=============================================================================*/
/**
  Sends a Call Control envelope command library function to the SIM/USIM based
  on the slot information provided.

  Clients provide #gstk_cc_ind_type_ext as the parameter.
  Also, the gstk_address_string union is required based on whether the call is
  a voice call, an SS, or a USSD.
  Also, the rat and loc_info is required

  Three data fields are optional for the voice call-related Call Control
  envelope command: subaddress, ccp1 and ccp2.

  This function does not require the client ID because it is intended to be called
  by internal modules.

  @param[in] slot          Indicates whether the client requires Slot 1 or Slot
                           2 for the call control request.
  @param[in] user_data     User data to be tracked by the GSTK for the client.
  @param[in] cc_ind_param  Pointer to the Call Control envelope command
                           information.
  @param[in] gstk_cc_cb    Function pointer to which the GSTK sends the card
                           response data corresponding to the Call Control
                           command.

  @return
  GSTK_SUCCESS\n
  GSTK_NULL_INPUT_PARAM

  @dependencies
  The gstk_client_init_ext() function must be called before before this function is
  called.
*/
extern gstk_status_enum_type gstk_slot_send_envelope_cc_command_ext (
  gstk_slot_id_enum_type       slot,
  uint32                       user_data,
  const gstk_cc_ind_type_ext*  cc_ind_param,
  gstk_evt_cb_funct_type       gstk_cc_cb
);
/** @} */ /* end_addtogroup envCmd */
/** @addtogroup gstkUtil
  @{
*/
/*===========================================================================
-- FUNCTION: gstk_address_to_ascii
===========================================================================*/
/**
  Converts the GSTK address format to an ASCII value.

  @param[in] gstk_addr       Pointer to the input address.
  @param[in,out] ascii_addr  Pointer to the output address.
  @param[in] ascii_len       Length of the ascii_addr buffer. This value
                             ensures the size of ascii_addr does not exceed the
                             amount of information being converted.

  @return
  GSTK_SUCCESS\n
  GSTK_NULL_INPUT_PARAM

  @dependencies
  None.
*/
extern gstk_status_enum_type gstk_address_to_ascii (
  const gstk_address_type * gstk_addr,
  uint8                   * ascii_addr,
  uint8                     ascii_len
);

/** @} */ /* end_addtogroup gstkUtil */

/** @addtogroup clientReg
  @{
*/
/*=============================================================================
-- FUNCTION: GSTK_TAL_CLIENT_REGISTRATION
=============================================================================*/
/**
  Used by the GSTK local and remote to register with the TAL.

  @param[in] client_type        Indicates whether the client resides on the
                                local or remote processor.
  @param[in] gstk_tal_cb        Callback used by the TAL to inform the clients
                                about the UIM driver response or to send
                                proactive commands.
  @param[in] tech_support_mask  Bitmask used by clients to inform the TAL about
                                supported technologies or proactive commands.

  @return
  GSTK_SUCCESS       -- Command was queued onto the GSTK queue.\n
  GSTK_BAD_PARAM     -- Invalid input parameter.\n
  GSTK_MEMORY_ERROR  -- Error while allocating memory on the heap.\n
  GSTK_ERROR         -- Generic error; the function is not supported.

  @dependencies
  None.
*/
extern gstk_status_enum_type gstk_tal_client_registration(
  gstk_tal_client_type                  client_type,
  gstk_tal_reg_cb_type                  gstk_tal_cb,
  uint32                                tech_support_mask);
/** @} */ /* end_addtogroup clientReg */

/** @addtogroup termRspLib
  @{
*/
/*=============================================================================
-- FUNCTION: GSTK_TAL_SEND_CMD
=============================================================================*/
/**
  Used by the GSTK local and remote processor to send a terminal response,
  a terminal profile, and envelope commands to the TAL asynchronously. The TAL
  sends this command to the UIM driver and notifies the caller with the result.

  @param[in] client_type  Client local or remote processor that sent the
                          command.
  @param[in] tal_cmd      Pointer to the TAL command data to send to the UIM
                          driver.

  @return
  GSTK_SUCCESS      -- Command was successfully queued to the GSTK command
                       buffer.\n
  GSTK_BAD_PARAM    -- Invalid input parameter.\n
  GSTK_MEMORY_ERROR -- Error in allocating memory.\n
  GSTK_ERROR        -- Generic error; the function is not supported.

  @dependencies
  None.
*/
extern gstk_status_enum_type gstk_tal_send_cmd(
  gstk_tal_client_type   client_type,
  gstk_tal_uim_cmd_type *tal_cmd);

/** @} */ /* end_addtogroup termRspLib */

/** @addtogroup gstkUtil
  @{
*/
/*=============================================================================
-- FUNCTION: GSTK_TAL_NOTIFY_RAT
=============================================================================*/
/**
  Used by the GSTK local and remote processor to notify the TAL about the
  subscription availibility to the radio access technology. The TAL stores
  these values and uses them to make decisions when routing the proactive
  commands to the respective GSTK clients.

  @param[in] client_type  Client local or remote processor that sent the
                          command.
  @param[in] rat          Subscription to the radio access technology
                          is available.

  @return
  GSTK_SUCCESS -- Command was performed successfully.\n
  GSTK_ERROR   -- Error in processing the command.

  @dependencies
  None.
*/
gstk_status_enum_type gstk_tal_notify_rat(
  gstk_tal_client_type           client_type,
  gstk_access_technology_type    rat);

/*===========================================================================
-- FUNCTION: gstk_send_icon_display_rsp
===========================================================================*/
/**
  Library function called by the client to determine whether the icon was
  displayed.

  @param[in] client_id   Client ID.
  @param[in] cmd_ref_id  GSTK command reference ID in the GSTK header
                         information sent to the client in the callback.
  @param[in] icon_disp   Indicates whether the icon is to be displayed.

  @return
  GSTK_CLIENT_NOT_REGISTERED -- Client ID is not valid.\n
  GSTK_MEMORY_ERROR          -- ESTK queue has no free space.\n
  GSTK_BAD_PARAM             -- Invalid input parameter.\n
  GSTK_SUCCESS               -- Command was queued successfully to the ESTK.

  @dependencies
  The ESTK command buffer must be available.
*/
gstk_status_enum_type gstk_send_icon_display_rsp(
  gstk_client_id_type  client_id,
  uint32               cmd_ref_id,
  boolean              icon_disp
);

/*===========================================================================
-- FUNCTION: gstk_send_user_cnf_alpha_and_icon_disp_rsp
===========================================================================*/
/**
  Library function called by the client to vote YES or NO as a response for
  the user confirmation alpha sent by the ESTK for the Set Up Call command.

  @param[in] client_id       Client ID.
  @param[in] gstk_user_data  User data.
  @param[in] yes_no_rsp      Indicates whether the call is to be set up.
  @param[in] icon_disp       Indicates whether the icon is to be displayed.


  @return
  GSTK_CLIENT_NOT_REGISTERED -- Client ID is not valid.\n
  GSTK_MEMORY_ERROR          -- ESTK queue has no free space.\n
  GSTK_BAD_PARAM             -- Invalid input parameter.\n
  GSTK_SUCCESS               -- Command was queued successfully to the ESTK.

  @dependencies
  The ESTK command buffer must be available.
*/
gstk_status_enum_type gstk_send_user_cnf_alpha_and_icon_disp_rsp(
  gstk_client_id_type  client_id,
  uint32               gstk_user_data,
  boolean              yes_no_rsp,
  boolean              icon_disp
);

/*=============================================================================
FUNCTION   gstk_scws_ch_status_req
=============================================================================*/
/**
  Library function called by the client as notification of a channel status change.

  @param[in] client_id  Client ID.
  @param[in] ch_id      Channel ID.
  @param[in] ch_status  Status of the channel.

  @dependencies
  The ESTK command buffer must be available.

  @return
  GSTK_CLIENT_NOT_REGISTERED -- Client ID is not valid.\n
  GSTK_MEMORY_ERROR          -- No free space in the ESTK queue.\n
  GSTK_BAD_PARAM             -- Bad parameter was passed in.\n
  GSTK_SUCCESS               -- Command was queued successfully to the ESTK.
*/
extern gstk_status_enum_type  gstk_scws_ch_status_req(
  gstk_client_id_type           client_id,
  uint32                        ch_id,
  gstk_scws_ch_state_enum_type  ch_status
);

/*===========================================================================
FUNCTION   gstk_scws_data_avail_req_type
===========================================================================*/

/**
  Library function called by the client as notification that data is available.

  @param[in] client_id      Client ID.
  @param[in] ch_id          Channel ID.
  @param[in] data           Data from the SCWS socket.
  @param[in] data_remaining Amount of data that is left.

  @dependencies
  The ESTK command buffer must be available.

  @return
  GSTK_CLIENT_NOT_REGISTERED -- Client ID is not valid.\n
  GSTK_MEMORY_ERROR          -- ESTK queue has no free space.\n
  GSTK_BAD_PARAM             -- Bad parameter was passed in.\n
  GSTK_SUCCESS               -- Command was queued successfully to the ESTK.
*/
extern gstk_status_enum_type  gstk_scws_data_avail_req_type(
  gstk_client_id_type      client_id,
  uint32                   ch_id,
  gstk_generic_data_type   data,
  uint32                   data_remaining
 );

/*=============================================================================
FUNCTION   gstk_scws_open_ch_rsp
=============================================================================*/
/**
  Library function called by the client as notification ofthe Open Channel
  Response status.

  @param[in] client_id  Client ID.
  @param[in] cmd_ref_id GSTK command reference ID in the GSTK header
                        information sent to the client in the callback.
  @param[in] ch_id      Channel ID.
  @param[in] ch_status  Status of the channel.

  @return
  GSTK_CLIENT_NOT_REGISTERED -- Client ID is not valid.\n
  GSTK_MEMORY_ERROR          -- ESTK queue has no free space.\n
  GSTK_BAD_PARAM             -- Bad parameter was passed in.\n
  GSTK_SUCCESS               -- Command was queued successfully to the ESTK.

  @dependencies
  The ESTK command buffer must be available.
*/
gstk_status_enum_type  gstk_scws_open_ch_rsp(
  gstk_client_id_type           client_id,
  uint32                        cmd_ref_id,
  uint32                        ch_id,
  gstk_scws_ch_state_enum_type  ch_status
);

/*===========================================================================
FUNCTION   gstk_scws_close_ch_rsp
===========================================================================*/
/**
  Library function called by the client as notification of the Channel Closed
  status.

  @param[in] client_id  Client ID.
  @param[in] cmd_ref_id GSTK command reference ID in the GSTK
                        header information sent to the client in the callback.
  @param[in] ch_id      Channel ID.
  @param[in] ch_status  Status of the channel.

  @return
  GSTK_CLIENT_NOT_REGISTERED -- Client ID is not valid.\n
  GSTK_MEMORY_ERROR          -- ESTK queue has no free space.\n
  GSTK_BAD_PARAM             -- Bad parameter was passed in.\n
  GSTK_SUCCESS               -- Command was queued successfully to the ESTK.

  @dependencies
  The ESTK command buffer must be available.
*/
gstk_status_enum_type  gstk_scws_close_ch_rsp(
  gstk_client_id_type          client_id,
  uint32                       cmd_ref_id,
  uint32                       ch_id,
  gstk_scws_ch_state_enum_type ch_status
);

/*=============================================================================
FUNCTION   gstk_scws_send_data_rsp
=============================================================================*/
/**
  Library function called by the client to indicate whether Send Data was successful.

  @param[in] client_id     Client ID.
  @param[in] cmd_ref_id    GSTK command reference ID in the GSTK header
                           information sent to the client in the callback.
  @param[in] ch_id         Status of the channel.
  @param[in] write_success Indicates whether the write operation was successful.

  @return
  GSTK_CLIENT_NOT_REGISTERED -- Client ID is not valid.\n
  GSTK_MEMORY_ERROR          -- No free space in the ESTK queue.\n
  GSTK_BAD_PARAM             -- Bad parameter was passed in.\n
  GSTK_SUCCESS               -- Command was queued successfully to the ESTK.

  @dependencies
  The ESTK command buffer must be available.
*/
gstk_status_enum_type  gstk_scws_send_data_rsp(
  gstk_client_id_type      client_id,
  uint32                   cmd_ref_id,
  uint32                   ch_id,
  boolean                  write_success
);

/* ----------------------------------------------------------------------------
   FUNCTION:      GSTK_TAL_NOTIFY_LS
=============================================================================*/
/**
  This function is used by GSTK local and remote to notify TAL about
  current Location Status

  @params[in] client_type  Tal Client Type Value (Local/Remote)
  @params[in] ls           Current location status value

  @return
  GSTK_SUCCESS:          The cmd was successfully queued to GSTK Queue
  GSTK_ERROR:            There was an error queueing command to GSTK.

  @dependencies
  None.
*/
gstk_status_enum_type gstk_tal_notify_ls(
  gstk_tal_client_type             client_type,
  gstk_location_status_enum_type   ls);

/** @} */ /* end_addtogroup gstkUtil */

#endif  /* GSTK_EXP_H */
