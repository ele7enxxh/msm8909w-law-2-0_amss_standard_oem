#ifndef GSTK_EXP_V_H
#define GSTK_EXP_V_H


/*===========================================================================


                     G S T K _E X P_V    H E A D E R

GENERAL DESCRIPTION
  This is the GSTK internal header file for GSTK client.  It contains
  command prototype and function protocol that client can use to communicate
  with GSTK.


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2008-2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.QUALCOMM Technologies Proprietary.
Export of this technology or software is regulated by the
U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/api/gstk_exp_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
05/12/15   dy      Enabling Toolkit-initiated GBA TP bit (byte 25 bit 5)
10/08/14   gm      Remove GSTK registration with WMS if FEATURE_ESTK is enabled
09/25/14   gm      Added support to send failure indication for OTASP
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info
09/10/14   gm      Added support for SMS_PP data download retry
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
07/11/14   bd      Enable retry for call control envelopes by default
06/19/14   sw      Call/SMS Control retry
06/17/14   dy      Remove GSTK RAT use in ESTK setup call
06/13/14   shr     Enable continous read into BIP Rx buffer based on NV config
05/28/14   gm      Support of CSG Rel-11 feature
05/28/14   dy      Add GSTK_SUBS_FEATURE_MODE_SRLTE
05/27/14   bd      Changes to hold null alpha and no alpha indications
05/19/14   dy      Fix mapping of ds sub id to slot id
05/13/14   gs      Changes in the enum varibale names of gstk_cfg_features_bmask_enum
05/06/14   gs      GSTK Featurization enhancements and clean up
04/25/14   hn      Add GSTK_CFG_CAT_VER12 for DNS query/Channel Reuse TP
03/04/14   shr     Do not wait for 1x to become available on non-SVLTE targets
                   w.r.t. card activation via OTASP
03/03/14   shr     ESTK BIP clean-up and enhancements
02/03/14   dy      Remove usage of GSTK_CFG_FEATURE_CUST_TP_31_32_BYTE
01/30/14   vr      SIM initiated call control handling
01/14/14   lxu     add GSTK_CFG_FEATURE_CUST_TP_25_BYTE
12/16/13   bd      Added support for new CM SS event handling
12/11/13   gs      Remove session_id from gstk_mm_idle_state_type
12/06/13   bd      Changes in GSTK to support MM IDLE events for SGLTE+G
12/05/13   hn      Support DNS query and Channel reuse
11/13/13   shr     Added support for BIP over WLAN
09/20/13   shr     LTE multi-SIM support
07/26/13   hh      Support for E-UTRAN Inter-frequency measurements
07/25/13   hn      Enable FEATURE_ESTK_BIP_SERVER_MODE, add control bitmask
07/24/13   sw      Executing location env rsp rcvd confirmation in gstk context
07/23/13   sw      Support for DS callback to obtain RAT information.
07/22/13   hn      Support MM Indications
07/03/13   kb      Remove write of tp_client when feature is disabled
07/01/13   hh      Added support for passing RAT through MO SMS CTRL API
07/01/13   hn      Support SGLTE MMs on different stacks
06/24/13   xz      Support IMS Registration event download
05/24/13   hn      Does APN mapping based on NV item
05/17/13   abd     Added support for reading default lang from NV
03/11/13   gm      Disable transcoding in GSTK using NV
02/28/13   bd      Support for dynamic reg and dereg
02/14/13   vs      Added default TAL config creation support
02/11/13   bd      Support for (e)HRPD access tech
11/29/12   bd      Clean up of redundant members from client table
11/13/12   bd      Support for Closed Subscriber Group feature
09/20/12   yt      Manage concurrency between BIP and OTASP
08/15/12   sg      Add item to read qmi cat configuration
08/13/12   sg      Add NV to Control mandatory additional info byte with NV
06/14/12   dd      Add support for Setup Call 1.11B
06/06/12   sg      Add Hybrid stack variables for Mode Change
03/30/12   av      Replaced feature flags with NVITEM
03/30/12   yt      Defeaturized declaration of gstk_get_curr_cell_id_info()
02/28/12   bd      Enhancements for location information in CC
02/22/12   sg      Add cfg to set TP byte 31 and 32 to customized values
01/03/12   nb      Support to retrieve terminal profile
12/21/11   kk      More GSDI Cleanup
12/13/11   dd      Do not overwrite tp_clients if less clients are found
11/21/11   sg      Add RRC rat for tracking cell change from LTE vs WCDMA
11/09/11   sg      Add TP bit MACROs for UTRAN, EUTRAN and HSDPA
10/31/11   sg      Class L and M support
09/28/11   xz      Featurized declaration of gstk_get_curr_cell_id_info()
09/28/11   xz      Send AcT and Loc envelope when receive SETUP EVT LIST cmd
09/23/11   nb      Async TP enhancement and moving GSTK to MMGSDI events
07/26/11   js      Change to support AcT event processing in TAL
06/27/11   yt      Removed featurization from UIM public headers
06/13/11   sg      Move FEATURE_ESTK_IGNORE_ALPHA to NV
06/01/11   yt      Reverted defeaturization of gstk_get_curr_cell_id_info()
05/17/11   nb      ThreadSafe - Added calback field to envelope cmd structures
05/02/11   yt      Defeaturized declaration of gstk_get_curr_cell_id_info()
04/28/11   nk      Fixed Warning for incorrect prototype
04/24/11   dd      SCWS support
04/13/11   ea      Add proactive cmd enable / disable support
04/05/11   xz      Changes to support VPE ATTACH CTRL
03/21/11   sg      Add config for GSTK_CFG_FEATURE_ESTK_BIP_IP_DEFAULT
03/18/11   ea      Fix initialized value greater than int32 compiler warning
03/03/11   dd      Send PLI Date/time to clients
02/22/11   js      Fix for async processing of TAL commands
02/15/11   ea      Response timer removal & other_info->present check feature bitmask
02/09/10   bd      Support for delaying the TR when card is repeating the
                   same proactive command continuously.
12/17/10   xz      Decouple feature bitmask
12/16/10   nb      Added prototype of function gstk_send_envelope_menu_sel_command_ext
11/23/10   sg      Add support of IO ctrl to get current loc status
11/30/10   yt      Moving data types and APIs used only within UIM to private
                   header
11/26/10   adp     Fixing compilation error due to function prototype mismatch
11/18/10   bd      Fixed compilation errors
10/27/10   js      Fix for open channel terminal rsp not getting sent
10/20/10   dd      Move the APIs to gstk_exp.h from gstk_exp_v.h
10/20/10   xz      Move gstk_queue_rrc_utran_nmr_rsp() to gstk_exp_v.h file
10/18/10   xz      Add support of QCN backup
10/05/10   xz      Move NMR definition to public header
09/08/10   ms      Added DSDS UT Framework changes
08/19/10   xz      1) Add NV support of GSTK feature bitmask
                   2) Block OPEN CH until MMGSDI_SUBSCRIPTION_READY_EVT
08/12/10   dd      Set correct TP for data cards
07/26/10   nb      Added new API gstk_gsdi_set_fetch_status
07/15/10   sg      Export gstk_access_technology_type for TAL
07/07/10   sg      Added API to send proactive command to TAL
06/30/10   dd      Icon support
06/29/10   nb      Lint Fix
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
05/21/10   xz      Correct LTE access tech value
05/11/10   xz      1) Add support of network rejection event
                   2) Add support of GSTK_IO_SEND_EMM_REGISTERED_EVENT
04/27/10   xz      Add support of EMM-IDLE event
03/15/10   xz      Add support of LTE access tech
03/08/10   sg      Location information envelope command
03/11/10   xz      Move out gstk_slot_send_envelope_cc_command() and
                   gstk_address_to_ascii()
02/19/10   bd      Support for caching proactive commands
02/19/10   xz      Fix compilation error
02/15/10   nb      Moved Dual Slot Changes
01/14/10   sg      Added gstk_io_ctrl api
12/17/09   sg      Fix compilation errors when FEATURE_ESTK is enabled
11/12/09   bd      Fixed a compiler warning
11/03/09   dd      Move gstk_send_envelope_mo_sms_ctrl_command to gstk_exp.h
10/28/09   nb      Release 7 enhanced network selection
09/03/09   dd      Move gstk_is_mo_sms_ctrl_required to gstk_exp.h
07/08/09   dd      Fix IMEISV length
06/12/09   adp     Move gstk_is_mo_sms_ctrl_required,gstk_is_sms_pp_supported,
                   gstk_is_cell_broadcast_dl_supported to gstk_exp_v.h
04/21/09   yb      Fixed compilation errors
04/20/09   yb      Implemented a mechanism to notify client when TP DL fails
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id
                   when parse client TP and simplify TP download procedure
03/02/09   sun     Removed inclusion of private header file
02/26/09   gg      Introduced Class B event registration
02/17/09   nb      Fixed Compilation Warning
01/28/09   sun     Added Meta Comments
12/17/08   nb      Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "cmd.h"
#include "gstk_exp.h"
#include "uim_v.h"
#include "mmgsdilib_common.h"
#include "ds_sys.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* This Maximum is used for the output length of the gstk decode functions,
and will need to be expanded as larger capacity is needed */
#define GSTK_MCC_LEN                             0x02
#define GSTK_BASE_LAT_LEN                        0x03
#define GSTK_BASE_LONG_LEN                       0x03
#define GSTK_MAX_SID_LEN                         0x02
#define GSTK_MAX_BASE_ID_LEN                     0x02
#define GSTK_MAX_NID_LEN                         0x02
#define GSTK_IMEI_LEN                            0x08
#define GSTK_IMEI_SV_LEN                         0x09
#define GSTK_ESN_LEN                             0x04
#define GSTK_MEID_LEN                            0x08
#define GSTK_NMR_LEN                             0x10
#define GSTK_MIN_CELL_ID_LEN                     0x02
#define GSTK_BA_MAX_LEN                          0x20
#define GSTK_RR_USAT_SIZE_BCCH_LIST              ((GSTK_BA_MAX_LEN*10)/8+1)
#define GSTK_HEIGHT_LEN                          0x03
#define GSTK_HDR_SECTOR_ID_LEN                   0x10
#define GSTK_CDMA_CHANNEL_LEN                    0x03

/* Maximum Home (e)NodeB Name length. The HNB Name length shall not exceed
   48x8 bits as per TS 122 220*/
#define GSTK_HNB_NAME_MAX_LEN                    48

/* This is generally expected to be equal to CM_NO_STACKS (cm.h) */
#define GSTK_CM_NUM_STACKS 2

/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/
/* According to spec 102.223 */
typedef enum
{
  /* Network search mode - Manual */
  GSTK_NW_SEARCH_MODE_MANUAL    = 0x00,
  /* Network search mode - Automatic */
  GSTK_NW_SEARCH_MODE_AUTOMATIC = 0x01,
  /* Network search mode - Invalid */
  GSTK_NW_SEARCH_MODE_MAX       = 0xFF
}
gstk_nw_search_mode_enum_type;

typedef enum
{
  /* Invalid CM_PH event For Internal use */
  GSTK_CM_PH_EVENT_NONE                     = 0x00,
  /* CM_PH event for phone infromation */
  GSTK_CM_PH_EVENT_INFO                     = 0x01,
  /* CM_PH event for System selection preference change */
  GSTK_CM_PH_EVENT_SYS_SEL_PREF             = 0x02,
  /* CM_PH event for GW system preferences change */
  GSTK_CM_PH_EVENT_DUAL_STANDBY_PREF        = 0x03,
  /* CM_PH event for list of available networks */
  GSTK_CM_PH_EVENT_AVAILABLE_NETWORKS_CONF  = 0x04,
  /* Invalid CM_PH event For Internal use */
  GSTK_CM_PH_EVENT_MAX                      = 0xFF
}
gstk_cm_ph_event_enum_type;

typedef enum
{
  /* WMS event for MT MESSAGE ERROR*/
  GSTK_WMS_MSG_EVENT_MT_MESSAGE_ERROR                  = 0x01,
  /* Invalid WMS event For Internal use */
  GSTK_WMS_EVENT_MAX                                   = 0xFF
}
gstk_wms_event_enum_type;

/*===========================================================================
--  Enum: gstk_nmr_utran_qualifier_enum_type
      Various Provide local information utran nmr measurement qualifiers
      that the USIM can request from the ME
      GSTK_NMR_NONE_MEAS                invalid value
      GSTK_NMR_INTRA_FREQ_MEAS          UTRAN Intra-frequency measurements
      GSTK_NMR_INTER_FREQ_MEAS          UTRAN Inter-frequency measurements
      GSTK_NMR_INTER_RAT_MEAS           UTRAN Inter-RAT (GERAN) measurements
      GSTK_NMR_INTER_RAT_LTE_MEAS       UTRAN Inter-RAT (E-UTRAN) measurements
      GSTK_NMR_LTE_INTRA_FREQ_MEAS      E-UTRAN Intra-frequency measurements
      GSTK_NMR_LTE_INTER_FREQ_MEAS      E-UTRAN Inter-frequency measurements
      GSTK_NMR_LTE_INTER_RAT_GERAN_MEAS E-UTRAN Inter-RAT (GERAN) measurements
      GSTK_NMR_LTE_INTER_RAT_UTRAN_MEAS E-UTRAN Inter-RAT (UTRAN) measurements
===========================================================================*/
typedef enum
{
    /* These are one byte values */
    GSTK_NMR_NONE_MEAS                     = 0x00,
    GSTK_NMR_INTRA_FREQ_MEAS               = 0x01,
    GSTK_NMR_INTER_FREQ_MEAS               = 0X02,
    GSTK_NMR_INTER_RAT_MEAS                = 0X03,
    GSTK_NMR_INTER_RAT_LTE_MEAS            = 0x04,
    GSTK_NMR_LTE_INTRA_FREQ_MEAS           = 0x05,
    GSTK_NMR_LTE_INTER_FREQ_MEAS           = 0x06,
    GSTK_NMR_LTE_INTER_RAT_GERAN_MEAS      = 0x07,
    GSTK_NMR_LTE_INTER_RAT_UTRAN_MEAS      = 0x08
}
gstk_nmr_utran_qualifier_enum_type;

/*===========================================================================
--  Enum: gstk_browser_term_cause_enum_type
      Various (E)UTRAN NMR processing results that RRC can return to GSTK
      GSTK_UTRAN_NMR_RESULT_INVALID             invalid value
      GSTK_UTRAN_NMR_RESULT_SUCCESS             succeed in NMR
      GSTK_UTRAN_NMR_RESULT_FAIL_UE_ON_GERAN    fail due to UE on different RAT
      GSTK_UTRAN_NMR_RESULT_FAIL_UE_OOS         fail due to UE out of service
      GSTK_UTRAN_NMR_RESULT_FAIL_ASID_MISMATCH  fail due to AS ID mismatch
      GSTK_UTRAN_NMR_RESULT_FAIL_OTHER          fail due to other reasons
===========================================================================*/
typedef enum
{
    GSTK_UTRAN_NMR_RESULT_INVALID,
    GSTK_UTRAN_NMR_RESULT_SUCCESS,
    GSTK_UTRAN_NMR_RESULT_FAIL_UE_ON_GERAN,
    GSTK_UTRAN_NMR_RESULT_FAIL_UE_OOS,
    GSTK_UTRAN_NMR_RESULT_FAIL_ASID_MISMATCH,
    GSTK_UTRAN_NMR_RESULT_FAIL_OTHER
}
gstk_utran_status_enum_type;

/*===========================================================================
--  Enum: gstk_io_ctrl_enum_type
      The I/O command supported by GSTK. These enums are used by
      gstk_io_ctrl() function.
===========================================================================*/
typedef enum {
  GSTK_IO_GET_CLIENT_TABLE_NEW_CLIENT_CB,
  GSTK_IO_GET_CLIENT_TABLE_SLOT_ID,
  GSTK_IO_GET_CLIENT_TABLE_USER_DATA,
  GSTK_IO_GET_CMD_RAW_APDU,
  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
  GSTK_IO_SET_CURR_SESSION_UI_RELATED,
  GSTK_IO_CALL_CLIENT_EVT_CB,
  GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
  GSTK_IO_SLOT_ID_TO_MODEM_AS_ID,
  GSTK_IO_DS_SUBSCRIPTION_ID_TO_SLOT_ID,
  GSTK_IO_GET_CURR_ACCESS_TECH,
  GSTK_IO_SEND_MM_IDLE_EVENT,
  GSTK_IO_SEND_EMM_REGISTERED_EVENT,
  GSTK_IO_SEND_MM_INDICATION,
  GSTK_IO_SEND_NW_REJ_EVENT,
  GSTK_IO_GET_CURR_LOC_STATUS,
  GSTK_IO_GET_TERMINAL_PROFILE_DATA,
  GSTK_IO_MANAGE_BIP_OTASP_CONCURRENCY,
  GSTK_IO_GET_DS_SYSTEM_RAT_INFO,
  GSTK_IO_GET_WLAN_AVAILABILITY,
  /* Used by CM to send SIM initiated call modified call control response */
  GSTK_IO_SIM_INITIATED_MODIFIED_CC_RESP,
  GSTK_IO_GET_ACTIVE_DATA_SUB_SLOT_ID,
  GSTK_IO_SEND_WMS_MT_MESSAGE_ERROR_EVENT,
  GSTK_IO_CMD_MAX                            = 0x7FFFFFFF
}
gstk_io_ctrl_enum_type;

/*===========================================================================
--  Enum: gstk_io_send_mm_ind_enum_type
      The types of MM indications supported by GSTK. These enums are used by
      gstk_io_ctrl() function.
===========================================================================*/
typedef enum {
  GSTK_IO_MM_SGLTE_IND,
  GSTK_IO_MM_CSFB_IND,
  GSTK_IO_MM_SGLTE_MERGE_IND,
  GSTK_IO_MM_SGLTE_SPLIT_IND,
  GSTK_IO_MM_CMD_MAX                         = 0x7FFFFFFF
}
gstk_io_send_mm_ind_enum_type;

/**
** The following enumerated type defines the radio access technologies.
*/
typedef enum
{
  GSTK_SYS_RAT_NONE = -1,
  GSTK_SYS_RAT_GSM_RADIO_ACCESS,
  GSTK_SYS_RAT_UMTS_RADIO_ACCESS,
  GSTK_SYS_RAT_MAX,
  GSTK_SYS_RAT_BIG  = 0x10000000
} gstk_sys_radio_access_tech_e_type;

/* the type of network rejection */
typedef enum
{
  GSTK_LOCATION_UPDATE_REJ,
  GSTK_GPRS_ATTACH_REJ,
  GSTK_ROUTING_AREA_UPDATE_REJ,
  GSTK_EMM_ATTACH_REJ,
  GSTK_TRACKING_AREA_UPDATE_REJ
}
gstk_nw_rej_enum_type;

typedef enum {
  GSTK_CFG_CAT_VER99                   = 0x00,
  GSTK_CFG_CAT_VER4                    = 0x04,
  GSTK_CFG_CAT_VER5                    = 0x05,
  GSTK_CFG_CAT_VER6                    = 0x06,
  GSTK_CFG_CAT_VER7                    = 0x07,
  GSTK_CFG_CAT_VER8                    = 0x08,
  GSTK_CFG_CAT_VER9                    = 0x09,
  GSTK_CFG_CAT_VER12                   = 0x0C,
  GSTK_CFG_CAT_VER_MAX                 = 0xFF
}
gstk_cfg_cat_version_enum;

/*===========================================================================
--  Enum: gstk_update_attach_type_enum_type
      The type of update/attach message. These enums are used in network
      rejection event.
      GSTK_NORMAL_LOC_UPDATE
        "Normal Location Updating" in the case of a LOCATION UPDATING REQUEST
        message
      GSTK_PERIODIC_LOC_UPDATE
        "Periodic Updating" in the case of a LOCATION UPDATING REQUEST message
      GSTK_IMSI_ATTACH
        "IMSI Attach" in the case of a LOCATION UPDATING REQUEST message
      GSTK_GPRS_ATTACH
        "GPRS Attach" in the case of a GPRS ATTACH REQUEST message
      GSTK_COMBINED_GSRS_IMSI_ATTACH
        "Combined GPRS/IMSI Attach" in the case of a GPRS ATTACH REQUEST message
      GSTK_ROUTING_AREA_UPDATE
        "RA Updating" in the case of a ROUTING AREA UPDATE REQUEST message
      GSTK_COMBINED_RA_LA_UPDATE
        "Combined RA/LA Updating" in the case of a ROUTING AREA UPDATE REQUEST
        message
      GSTK_COMBINED_RA_LA_UPDATE_IMSI_ATTACH
        "Combined RA/LA Updating with IMSI Attach" in the case of a ROUTING AREA
        UPDATE REQUEST message
      GSTK_PERIODIC_RA_UPDATE
        "Periodic Updating" in the case of a ROUTING AREA UPDATE REQUEST message
      GSTK_EPS_ATTACH
        "EPS Attach" in the case of an EMM ATTACH REQUEST message
      GSTK_COMBINED_EPS_IMSI_ATTACH
        "Combined EPS/IMSI Attach" in the case of an EMM ATTACH REQUEST message
      GSTK_EMM_TA_UPDATE
        "TA updating " in the case of an EMM TRACKING AREA UPDATE REQUEST message
      GSTK_COMBINED_TA_LA_UPDATE
        "Combined TA/LA updating" in the case of an EMM TRACKING AREA UPDATE
        REQUEST message
      GSTK_COMBINED_TA_LA_UPDATE_IMSI_ATTACH
        "Combined TA/LA updating with IMSI attach" in the case of an EMM TRACKING
        AREA UPDATE REQUEST message
      GSTK_PERIODIC_TA_UPDATE
        "Periodic updating" in the case of an EMM TRACKING AREA UPDATE REQUEST
        message
      GSTK_UPDATE_ATTACH_TYPE_MAX
        Max value of enum

===========================================================================*/
typedef enum
{
  GSTK_NORMAL_LOC_UPDATE                 = 0x00,
  GSTK_PERIODIC_LOC_UPDATE               = 0x01,
  GSTK_IMSI_ATTACH                       = 0x02,
  GSTK_GPRS_ATTACH                       = 0x03,
  GSTK_COMBINED_GSRS_IMSI_ATTACH         = 0x04,
  GSTK_ROUTING_AREA_UPDATE               = 0x05,
  GSTK_COMBINED_RA_LA_UPDATE             = 0x06,
  GSTK_COMBINED_RA_LA_UPDATE_IMSI_ATTACH = 0x07,
  GSTK_PERIODIC_RA_UPDATE                = 0x08,
  GSTK_EPS_ATTACH                        = 0x09,
  GSTK_COMBINED_EPS_IMSI_ATTACH          = 0x0A,
  GSTK_EMM_TA_UPDATE                     = 0x0B,
  GSTK_COMBINED_TA_LA_UPDATE             = 0x0C,
  GSTK_COMBINED_TA_LA_UPDATE_IMSI_ATTACH = 0x0D,
  GSTK_PERIODIC_TA_UPDATE                = 0x0E,
  GSTK_UPDATE_ATTACH_TYPE_MAX            = 0x0F
}
gstk_update_attach_type_enum_type;

/*===========================================================================
--  Enum: gstk_attach_ctrl_result_enum_type
      The type of status after sending location information envelope cmd
      GSTK_ATTACH_CTRL_INVALID
        Invalid status
      GSTK_ATTACH_CTRL_ALLOWED
        Attach is allowed
      GSTK_ATTACH_CTRL_NOT_ALLOWED
        Attach is not allowed
      GSTK_ATTACH_CTRL_TIMER_EXP
        No conclusion due to timer expiration
===========================================================================*/
typedef enum
{
  GSTK_ATTACH_CTRL_INVALID              = 0,
  GSTK_ATTACH_CTRL_ALLOWED,
  GSTK_ATTACH_CTRL_NOT_ALLOWED,
  GSTK_ATTACH_CTRL_TIMER_EXP
}
gstk_attach_ctrl_result_enum_type;

/*===========================================================================
--  Enum: gstk_tz_avail_enum_type
      The type of status after sending location information envelope cmd
      GSTK_TZ_NO_CHANGE
        Invalid status
      GSTK_TZ_NOT_AVAILABLE
        Attach is allowed
      GSTK_TZ_AVAILABLE
        Attach is not allowed
===========================================================================*/
typedef enum
{
  GSTK_TZ_NO_CHANGE      = 0x00,
  GSTK_TZ_NOT_AVAILABLE  = 0x01,
  GSTK_TZ_AVAILABLE      = 0x02,
}
gstk_tz_avail_enum_type;

/*===========================================================================
--  Enum: gstk_subs_feature_mode_enum_type
      The mode corresponding to the Subscription
      GSTK_SUBS_FEATURE_MODE_NONE
        Mode unknown
      GSTK_SUBS_FEATURE_MODE_NORMAL
        Subs configured for normal mode
      GSTK_SUBS_FEATURE_MODE_SVLTE
        Subs configured for SVLTE
      GSTK_SUBS_FEATURE_MODE_SGLTE
        Subs configured for SGLTE
      GSTK_SUBS_FEATURE_MODE_SRLTE
        Subs configured for SRLTE
===========================================================================*/
typedef enum
{
  GSTK_SUBS_FEATURE_MODE_NONE   = 0x00,
  GSTK_SUBS_FEATURE_MODE_NORMAL,
  GSTK_SUBS_FEATURE_MODE_SVLTE,
  GSTK_SUBS_FEATURE_MODE_SGLTE,
  GSTK_SUBS_FEATURE_MODE_SRLTE
}
gstk_subs_feature_mode_enum_type;

/*===========================================================================
--  Enum: gstk_otasp_act_status_enum_type
      Status of OTASP call
      GSTK_OTASP_ACT_STATUS_START
        OTASP activation start
      GSTK_OTASP_ACT_STATUS_SUCCESS
        OTASP activation success
      GSTK_OTASP_ACT_STATUS_PERM_FAIL
        OTASP activation fail permanently
===========================================================================*/
typedef enum 
{
   GSTK_OTASP_ACT_STATUS_START, 
   GSTK_OTASP_ACT_STATUS_SUCCESS,
   GSTK_OTASP_ACT_STATUS_PERM_FAIL
}
gstk_otasp_act_status_enum_type;


/*===========================================================================
 --  FUNCTION POINTERS 
===========================================================================*/
typedef void (*gstk_client_otasp_status_reg_cb_func_type)(
  gstk_status_enum_type             gstk_reg_status,
  gstk_client_ref_data_type         user_data);

typedef void (*gstk_client_otasp_status_cb_func_type)(
  gstk_slot_id_enum_type slot, gstk_otasp_act_status_enum_type status);


/*===========================================================================
   when command_group ==
   1. GSTK_PROACTIVE_CMD, command_id doesn't have to be filled in by the
      other layers
   2. GSTK_TERMINAL_RSP, command_id is required to be filled in by GSTK utility function
      or other layers who do not utilitize the utility function
   3. GSTK_ENVELOPE_CMD, command_id is required to be filled in byt eh GSTK utility function
      of by other layers who do not utilitize the utility function
   4. GSTK_ENVELOPE_RSP, command_id filled in by GSTK based on the user_data that UIM put.
                         the user_data is going to be the index location for the envelope
                         command that GSTK sends to UIM server.
                         GSTK will base on this user data and perform a lookup for the
                         response type == GSTK_SMS_PP_DOWNLOAD_IND_RSP,GSTK_MO_SMS_CTRL_IND_RSP,
                                       GSTK_CC_IND_RSP etc,  in the state machine processing
   5. GSTK_TERMINAL_PROFILE_CMD, command_id is irrelevant
   6. GSTK_TERMINAL_PROFILE_RSP, command_id = GSTK_PROFILE_DL_IND_RSP
   7. GSTK_STOP_CMD, command_id is irrelevant
   8. GSTK_GSDI_CMD, command_id will be the gsdi_sim_events_T
   9. GSTK_TIMER_EXP_CMD, command_id depends on whether it is TP or TR timer
   10. GSTK_MM_IDLE_CMD, command_id irrelevant
   11. GSTK_RRC_CELL_CHANGE_IND
   12. GSTK_MODE_CHANGE_IND, command_id irrelevant
===========================================================================*/
typedef struct
{
  gstk_slot_id_enum_type          sim_slot_id;
  gstk_cmd_group_enum_type        command_group;
  uint32                          command_id;       /* gstk_cmd_enum_type or GSTK_FSM_CMD or GSTK_ERROR_CMD */
  uint32                          user_data;        /* in communicating with UIM, GSTK will fill in user_Data which
                                                                                            ** UIM has to return the same info upon information received
                                                                                            ** from CARD */
                                                    /* user_data becomes env_ref_id in case of envelope_cmd */
  uint32                          client_id;        /* client id */
}
gstk_hdr_type;

typedef gstk_hdr_type gstk_power_down_type;

typedef struct
{
  gstk_hdr_type                  message_header;
  uint32                         length;          /* length of data */
  uint8                          data[GSTK_MAX_DATA_SIZE];
}
gstk_general_cmd_type;

typedef gstk_general_cmd_type gstk_proactive_cmd_type;

typedef gstk_general_cmd_type gstk_error_msg_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
}
gstk_unsupported_proactive_cmd_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_display_text_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  boolean                            data_present;
  /* data len for the user inputs: 1 or 0 */
  uint8                              returned_data_len;
  /* user input */
  uint8                              data[GSTK_GET_INKEY_USER_INPUT_DATA_LEN];
  uint8                              data_coding_scheme;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
  gstk_duration_type                 duration_of_exec;
}
gstk_get_inkey_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  boolean                            data_present;
  /* data len for the user inputs */
  uint8                              returned_data_len;
  /* user inputs */
  uint8                              *data;
  uint8                              data_coding_scheme;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_get_input_cnf_type;

typedef struct
{
  int32             id_len;
  uint8             sid[GSTK_MAX_SID_LEN];
} gstk_sid_type;

typedef struct
{
  int32             id_len;
  uint8             nid[GSTK_MAX_NID_LEN];
} gstk_nid_type;

typedef struct
{
  int32             id_len;
  uint8             base_id[GSTK_MAX_BASE_ID_LEN];
} gstk_base_id_type;

typedef struct
{
  uint8                        mcc[GSTK_MCC_LEN];
  uint8                        imsi_11_12;
  gstk_sid_type                sid_info;
  gstk_nid_type                nid_info;
  gstk_base_id_type            base_id_info;
  uint8                        base_lat[GSTK_BASE_LAT_LEN];
  uint8                        base_long[GSTK_BASE_LONG_LEN];
}
gstk_cdma_location_info_type;

/* Extended 3GPP2 location info */
typedef struct
{
  gstk_cdma_location_info_type cdma_loc_info;
  uint8                        height[GSTK_HEIGHT_LEN];
  uint8                        horizontal_uncertainty;
  uint8                        country_code[GSTK_MCC_LEN];
  uint8                        sector_id[GSTK_HDR_SECTOR_ID_LEN];
  uint8                        cdma_channel[GSTK_CDMA_CHANNEL_LEN];
  uint8                        base_lat[GSTK_BASE_LAT_LEN];
  uint8                        base_long[GSTK_BASE_LONG_LEN];
}
gstk_cdma_loci_ext_type;

/*
** Mode change indication
*/
typedef struct
{
  gstk_hdr_type msg_hdr;
  struct
  {
    gstk_access_technology_type access_tech;
    gstk_location_status_enum_type loc_status;
    boolean advance_info_changed;
    union
    {
      gstk_cdma_loci_ext_type cdma_ext;
      /* Other RATs’ loc info can be added above (in future) */
    } loc_info;
  } cm_stack[GSTK_CM_NUM_STACKS];
}
gstk_mode_change_ind_type;

/*
** Time zone information
*/
typedef struct
{
  gstk_access_technology_type access_tech;
  gstk_tz_avail_enum_type tz_avail;
  int tz; /* Used for G/W/L only */
} gstk_cm_ss_tz_info;

typedef struct
{
  gstk_hdr_type msg_hdr;
  gstk_cm_ss_tz_info cm_stack[GSTK_CM_NUM_STACKS];
}
gstk_tz_ind_type;

/*
**  gstk_cell_selection_status_gen_info
*/
typedef struct
{
  boolean                        is_camped;
}
gstk_cell_selection_status_gen_info;

/*
**  gstk_cell_selection_status_addi_info
*/
typedef struct
{
  boolean                         is_addi_info_present;
}
gstk_cell_selection_status_addi_info;

/*
** gstk_cell_selection_status_type
*/
typedef struct
{
  gstk_cell_selection_status_gen_info     general_info;
  gstk_cell_selection_status_addi_info    addi_info;
}
gstk_cell_selection_status_type;

/*===========================================================================

  STRUCT: GSTK_CSG_INFO_TYPE

  PURPOSE:
     Structure for CSG information
==========================================================================*/
typedef struct
{
  /* CSG ID */
    uint32                            csg_id;
  /* Lenght of hnb_name */
    uint8                             len;
  /* hone node-b name */
    uint8                             hnb_name[GSTK_HNB_NAME_MAX_LEN];
  /*Access technology*/
  gstk_access_technology_type      access_tech;
  /* Selection status - Automatic or manual*/
    gstk_cell_selection_status_type   selection_status;
  /* PLMN identity */
  byte                             plmn_info[3];
}
gstk_csg_info_type;

/*
** CSG information change indication
*/
typedef struct
{
  gstk_hdr_type                       message_header;
  gstk_csg_info_type                  cmd;
}
gstk_csg_change_ind_type;

/*
** SUBS information change indication
*/
typedef struct
{
  gstk_hdr_type                     message_header;
  gstk_subs_feature_mode_enum_type  subs_feature_mode;
}
gstk_subs_change_ind_type;

typedef struct
{
  gstk_hdr_type message_header;
  uint32        type;            /**< Refresh for slot or sub specific items */
  uint32        slot_index;      /**< Slot index for this refresh */
  uint32        sub_index;       /**< Subscription index for this refresh */
  uint32        reference_id;    /**< Reserved. */
} gstk_nv_refresh_cmd_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  gstk_additional_info_ptr_type      result_additional_info;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  ** NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **            - first byte = cause value of the Cause information from
  **                           network, bit 8 = 1
  ** LAUNCH_BROWSER_GENERIC_ERROR
  **         - 00 = no specific cause can be given
  **           01 = bearer unavailable
  **           02 = browser unavailable
  **           03 = ME unable to read the provisioning data
  */
}
gstk_launch_browser_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_more_time_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_play_tone_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_duration_type                 duration;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_poll_interval_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_refresh_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_run_at_cmd_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  uint8                              chosen_item_identifier;
  boolean                            data_present;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_select_item_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  gstk_additional_info_ptr_type      result_additional_info;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                           :gstk_additional_info_code_enum_type
  ** NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **        - first byte = cause value of the Cause information from network,
  **                       bit 8 = 1
  ** GSTK_SMS_RP_ERROR
  **        - first byte = cause value in the RP-Cause element of the RP-ERROR
  **                       message returned by the network, bit 8 = 0
  ** INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
  **        - first byte: 0x00 = no specific cause can be given
  **                      0x01 = action not allowed
  **                      0x02 = the type of request has changed
  */
}
gstk_send_sms_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  gstk_call_ss_rsp_extra_param_type  extra_param;
  gstk_additional_info_ptr_type      result_additional_info;
  /*
  ** used when command_result = SUCCESSFUL command results
  **             - first byte = SS return operation code
  **                     rest = SS return parameters from the return result
  **                            component from the network
  ** ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
  ** NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **             - first byte = cause value of the Cause information from
  **                            network, bit 8 = 1
  ** SS_RETURN_ERROR
  **             - first byte = error value given in the Facility (return result)
  **                            information element returned by the network
  ** INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
  **             - first byte: 0x00 = no specific cause can be given
  **                           0x01 = action not allowed
  **                           0x02 = the type of request has changed
  */
}
gstk_send_ss_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  gstk_ussd_rsp_extra_param_type     ussd_extra_param;
  gstk_additional_info_ptr_type      result_additional_info;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                           :gstk_additional_info_code_enum_type
  ** NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **             - first byte = cause value of the Cause information from
  **                            network, bit 8 = 1
  ** USSD_RETURN_ERROR
  **             - first byte = error value given in the Facility (Return result)
  **                            information element returned by the network
  ** INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
  **            - first byte: 0x00 = no specific cause can be given
  **                          0x01 = action not allowed
  **                          0x02 = the type of request has changed
  */
}
gstk_send_ussd_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  gstk_call_ss_rsp_extra_param_type  extra_param;
  gstk_additional_info_ptr_type      result_additional_info;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  ** NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **             - first byte = cause value of the Cause information from
  **                            network, bit 8 = 1
  ** INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
  **             - first byte: 0x00 = no specific cause can be given
  **                           0x01 = action not allowed
  **                           0x02 = the type of request has changed
  */
}
gstk_setup_call_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_setup_evt_list_cnf_type;

typedef struct
{
  uint8                               hour;
  uint8                               minute;
  uint8                               second;
}
gstk_timer_value_type;

typedef struct
{
  uint8                               timer_id;  /* valid value: [0x01, 0x08] */
  gstk_timer_value_type               time_value;
}
gstk_timer_info_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  boolean                            timer_info_present;
  gstk_timer_info_type               time_info;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_timer_manage_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_lang_notification_cnf_type;

typedef struct
{
  /* based on 3G 24.008 */
  uint8                        mcc_and_mnc[3];
  uint8                        loc_area_code[2];
  uint8                        cell_id[4];        /* Converted from a 16-bit word */
  int32                        cell_id_len;
}
gstk_location_info_rsp_type;

/* UTRAN NMR response structure
*/
typedef struct
{
  uint32                          length;
  uint8 *                         nmr_ptr;
  gstk_nmr_utran_qualifier_enum_type  nmr_utran_qualifier;
}
gstk_nmr_utran_rsp_type;

typedef struct
{
  uint8                         year;
  uint8                         month;
  uint8                         day;
  uint8                         hour;
  uint8                         minute;
  uint8                         second;
  uint8                         zone;
}
gstk_date_time_rsp_type;

typedef struct
{
  uint8                        me_status;      /* 0x00 = idle state */
                                               /* 0x01 = not in idle state */
  uint8                        timing_advance;
}
gstk_time_advance_rsp_type;

/*===========================================================================

  STRUCT: GSTK_AVAILABLE_CSG_LIST_TYPE

  PURPOSE:
    The structure is for grouping list of available CSG information
==========================================================================*/
typedef struct
{
  uint8 no_of_plmn_entry;
  struct
  {
    byte                       plmn[3];
    uint8                      no_of_csg_entry;
    gstk_csg_info_type         csg_info[SYS_PLMN_LIST_MAX_LENGTH];
  }available_plmn_info[SYS_PLMN_LIST_MAX_LENGTH];
}
gstk_available_csg_list_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_setup_menu_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_setup_idle_text_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
  boolean                            other_data_present;
  boolean                            is_scws_ch;
  boolean                            ch_status_present;
  gstk_ch_status_type                ch_status;
  gstk_bearer_description_type       bearer_description;
  int32                              buffer_size;
  gstk_dns_addrs_type                dns_addrs;
}
gstk_open_ch_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_close_ch_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_activate_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
  boolean                            data_present;
  int32                              ch_data_remain_buf_len;
}
gstk_send_data_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
  boolean                            data_present;
  gstk_ch_data_type                  ch_data;
  int32                              ch_data_remaining_len;
}
gstk_receive_data_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  */
  gstk_additional_info_ptr_type      result_additional_info;
  boolean                            data_present;
  int32                              ch_cnt;
  gstk_ch_status_type                ch_status[GSTK_MAX_GPRS_CHANNELS];
}
gstk_get_ch_status_cnf_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint32                             cmd_details_ref_id;
  uint8                              command_number;
  gstk_general_result_enum_type      command_result;
  /*
  ** used when command_result = ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
  **                          = gstk_additional_info_code_enum_type
  **                           (not_in_speech_call if the call has been terminated
  **                           or torn down before completion of this command
  */
  gstk_additional_info_ptr_type      result_additional_info;
}
gstk_send_dtmf_cnf_type;

typedef struct
{
  gstk_hdr_type                             message_header;
  gstk_client_id_type                       client_id;
  uint32                                    cmd_details_ref_id;
  gstk_generic_data_type                    raw_term_resp;
}
gstk_client_raw_term_resp_type;

/* Envelope command indication type */
typedef struct
{
  gstk_hdr_type                      message_header;
  uint8                              chosen_item_id;
  /* Optional */
  boolean                            request_help;
  gstk_evt_cb_funct_type             client_cb;
}
gstk_envelope_menu_selection_ind_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  uint8                              mcc_mnc[GSTK_MAX_PLMN_LEN];
}
gstk_envelope_location_information_ind_type;

typedef struct
{
  gstk_hdr_type                      message_header;
}
gstk_envelope_hci_connectivity_ind_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  boolean                            is_cdma_sms_pp;
  gstk_sms_tpdu_type                 tpdu;
  /* Optional */
  gstk_address_type                  address;
  gstk_evt_cb_funct_type             client_cb;
}
gstk_envelope_sms_pp_download_ind_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  gstk_cb_page_type                  cb_page;
  gstk_evt_cb_funct_type             client_cb;
}
gstk_envelope_cell_broadcast_download_ind_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  gstk_cc_ind_type_ext               cc_data;
  boolean                            slot_info_present;
  gstk_slot_id_enum_type             slot;
  gstk_evt_cb_funct_type             client_cb;
}
gstk_envelope_cc_ind_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  gstk_mo_sms_ctrl_ind_ext_type      mo_sms_ctrl_data;
  gstk_evt_cb_funct_type             client_cb;
}
gstk_envelope_mo_sms_ctrl_ind_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  gstk_mt_call_evt_ind_type          mt_evt_data;
}
gstk_envelope_mt_evt_ind_type;

typedef struct
{
  gstk_hdr_type                      message_header;
  gstk_transaction_id_type           transaction_id;
  boolean                            near_end;
}
gstk_envelope_call_connected_evt_ind_type;

typedef struct
{
  gstk_hdr_type                       message_header;
  gstk_call_disconnected_evt_ind_type call_disconnected_data;
  boolean                             near_end;
}
gstk_envelope_call_disconnected_evt_ind_type;

typedef struct
{
  gstk_hdr_type                        message_header;
}
gstk_envelope_idle_scrn_evt_ind_type;

typedef struct
{
  gstk_hdr_type                        message_header;
}
gstk_envelope_user_act_evt_ind_type;


typedef struct
{
  gstk_hdr_type                        message_header;
  gstk_timer_info_type                 timer_info;
}
gstk_envelope_timer_expire_ind_type;

typedef struct
{
  gstk_hdr_type                        message_header;
  gstk_browser_term_cause_enum_type    browser_termination_cause;
}
gstk_envelope_browser_term_evt_ind_type;

typedef struct
{
  gstk_hdr_type                        message_header;
  /* 2 bytes of alphanumeric char coded in SMS default 7-bit with bit 8 == 0 */
  gstk_lang_type                       lang_code;
}
gstk_envelope_lang_sel_evt_ind_type;

typedef struct
{
  gstk_hdr_type                            message_header;
  gstk_ch_status_type                      ch_status_info;
  int32                                    ch_data_len;
}
gstk_envelope_data_avail_evt_ind_type ;

typedef struct
{
  gstk_hdr_type                            message_header;
  gstk_ch_status_type                      ch_status_info;
}
gstk_envelope_ch_status_evt_ind_type;

typedef struct
{
  gstk_hdr_type                        message_header;
  uint8                                access_tech;
}
gstk_envelope_access_tech_ind_type;

typedef struct
{
  gstk_hdr_type                        message_header;
  uint8                                nw_search_mode;
}gstk_envelope_nw_search_mode_evt_ind_type;

typedef struct
{
  gstk_hdr_type                        message_header;
  gstk_ims_reg_evt_ind_type            ims_reg;
}
gstk_envelope_ims_registration_evt_ind_type;

typedef struct
{
  gstk_hdr_type                         message_header;
  gstk_general_envelope_rsp_enum_type   rsp_status;
  uint8                                 sw1;
  uint8                                 sw2;
  uint32                                length;     /* length of data */
  uint8                                 data[GSTK_MAX_DATA_SIZE];
}
gstk_envelope_resp_type;

/* differentiates term profile for sat and usat */
typedef struct
{
  gstk_hdr_type                  message_header;
  uint32                         length;     /* length of data */
  uint8                          data[GSTK_MAX_TP_DATA_SIZE];
}
gstk_terminal_profile_cmd_type;

typedef struct
{
  gstk_hdr_type                       message_header;
  boolean                             success;
}
gstk_term_profile_rsp_type;


typedef struct
{
  gstk_hdr_type                            message_header;
  mmgsdi_session_id_type                   session_id;
  gstk_evt_cb_funct_type                   client_cb;
  union
  {
    mmgsdi_card_inserted_evt_info_type        card_inserted;
    mmgsdi_session_changed_evt_info_type      session_changed;
    mmgsdi_subscription_ready_evt_info_type   subscription_ready;
    mmgsdi_tp_dl_evt_info_type                terminal_profile;
    mmgsdi_refresh_evt_info_type              refresh;
    mmgsdi_card_err_evt_info_type             card_error;
  } evt;
} gstk_mmgsdi_evt_type;


typedef struct
{
  int32               cell_len;
  uint8               cell_id[GSTK_MAX_CELL_ID_LEN];
} gstk_cell_id_type;

typedef struct
{
  gstk_hdr_type                        message_header;
  uint8                                mcc_mnc[GSTK_MAX_PLMN_LEN];
  uint8                                lac[GSTK_MAX_LAC_LEN];
  uint8                                location_state; /* Full/Limited/No service */
  uint8                                rat; /* gsm or wcdma or LTE */
  gstk_cell_id_type                    cell_id;
  sys_modem_stack_id_e_type            stack_id;
}
gstk_mm_idle_state_type;

typedef struct
{
  gstk_hdr_type                        message_header;
  uint8                                mcc_mnc[GSTK_MAX_PLMN_LEN];
  uint8                                lac[GSTK_MAX_LAC_LEN];
  uint8                                location_state; /* full service, limited service, no service */
  gstk_cell_id_type                    cell_info;
  gstk_access_technology_type          rrc_rat;
}
gstk_rrc_cell_change_ind_type;

typedef struct
{
   gstk_hdr_type                        message_header;
   ds_sys_system_status_ex_type         ds_sys_status_info;
   ds_sys_subscription_enum_type        subscription_id;
}
gstk_ds_sys_status_ex_ind_type;

typedef struct
{
   gstk_hdr_type                        message_header;
   ds_sys_wlan_mac_addr_type            ds_wlan_mac_addr_info;
}
gstk_ds_wlan_mac_addr_change_ind_type;

typedef struct
{
   gstk_hdr_type                        message_header;
   ds_sys_wlan_ssid_type                ds_wlan_ssid_info;
}
gstk_ds_wlan_ssid_change_ind_type;

typedef struct
{
   gstk_hdr_type                        message_header;
}
gstk_report_time_msg_type;

typedef struct
{
  gstk_hdr_type                  message_header;
  gstk_nw_search_mode_enum_type  ph_nw_sel_pref;
}
gstk_ph_nw_sel_pref_change_ind_type;

typedef struct
{
  /* Message header */
  gstk_hdr_type                    message_header;
  /* GSTK CM_PH event */
  gstk_cm_ph_event_enum_type       cm_ph_event;
  /* Network search mode type */
  gstk_nw_search_mode_enum_type    nw_search_mode;
  /* Enumeration of Active Subscription ID's */
  sys_modem_as_id_e_type           active_data_subs;
  /* Detailed PLMN list */
  sys_detailed_plmn_list_s_type    available_networks;
}
gstk_cm_ph_ind_type;

/* Raw Envelope Indication type.  env_len indicates the size of the
   env_data_ptr.  env_data_ptr should contains the envelope command as defined
   in 11.14 or 31.111 */
typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             env_len;
   uint8                            * env_data_ptr;
   gstk_evt_cb_funct_type             client_cb;
}
gstk_raw_envelope_ind_type;

typedef struct
{
  uint8         length;
  uint8         bcch_list[GSTK_RR_USAT_SIZE_BCCH_LIST];
}
gstk_rr_bcch_list_type;

typedef struct
{
    uint8                         nmr[GSTK_NMR_LEN];
    gstk_rr_bcch_list_type        bcch;
}
gstk_nmr_bcch_rsp_type;

typedef struct
{
  gstk_local_info_gen_enum_type       info_type;

  union{
    gstk_location_info_rsp_type   location_info;
    gstk_cdma_loci_ext_type       cdma_ext_loc_info;
    uint8                         imei[GSTK_IMEI_LEN];
    uint8                         imei_sv[GSTK_IMEI_SV_LEN];
    uint8                         esn[GSTK_ESN_LEN];
    uint8                         meid[GSTK_MEID_LEN];
    gstk_nmr_bcch_rsp_type        nmr_bcch;
    gstk_nmr_utran_rsp_type       nmr_utran;
    gstk_date_time_rsp_type       time;
    uint8                         lang_code[2];
    gstk_time_advance_rsp_type    time_adv;
    gstk_access_technology_type   gstk_access_tech;
    gstk_battery_state_enum_type  battery_status;
    gstk_nw_search_mode_enum_type nw_search_mode;
    sys_detailed_plmn_list_s_type csg_info;
  }info_data;
}
gstk_local_info_rsp_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_local_info_rsp_type           local_info_rsp;
   uint8                              returned_data_len;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;  /* used when command_result =
                                                                  ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                  = gstk_additional_info_code_enum_type */
}
gstk_provide_local_info_cnf_type;

typedef struct
{
  gstk_hdr_type                         message_header;
  gstk_client_type                      client_type;
  gstk_client_init_cb_func_type         client_cb_ptr;
}
gstk_client_id_reg_type;

typedef struct
{
   gstk_hdr_type                        message_header;
   uint8                                location_status;
   gstk_local_info_rsp_type             location_info;
}
gstk_envelope_location_statue_ind_type;

/* GSTK_CLIENT_DETAILS_TYPE
** This structure stores client information of a single "modem/ui" or "all"
** functionality client of GSTK in the gstk class event registration table.
** NOTE: Used only if FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT is turned on
*/
typedef struct
{
  boolean                                   is_reg;
  gstk_client_pro_cmd_data_format_enum_type data_format_type;
  gstk_client_id_type                       client_id;
}
gstk_client_details_type;

/* GSTK_CLASS_EVT_REG_INFO_TABLE_TYPE
** This structure forms a single element or record of the gstk class
** event registration table, a table used to store registration information
** of multiple toolkit clients to GSTK.
** NOTE: Used only if FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT is turned on
*/
typedef struct
{
  gstk_client_details_type gstk_ui_func_client;
  gstk_client_details_type gstk_modem_func_client;
}
gstk_class_evt_reg_info_table_type;

typedef struct
{
  gstk_hdr_type                             message_header;
  gstk_client_id_type                       client_id;
  uint32                                    num_of_cmds;
  gstk_toolkit_cmd_reg_info_type            *reg_info_ptr;
  gstk_reg_rsp_cmd_cb_func_type             client_cmd_cb;
  gstk_toolkit_evt_cb_func_type             client_evt_cb;
  gstk_client_pro_cmd_data_format_enum_type data_format_type;
}
gstk_client_evt_reg_type;

/*===========================================================================
-- Definition for current access technology callback function.
   This client callback is invoked when GSTK wants to report the current
   access technology to the client.
===========================================================================*/
typedef void (*gstk_curr_access_tech_cb_func_type)(
        uint8                             access_tech);

typedef struct
{
  gstk_hdr_type                             message_header;
  gstk_curr_access_tech_cb_func_type        client_cb_ptr;
}
gstk_curr_access_tech_type;

/*===========================================================================
-- The structure of network rejection event:
     nw_rej_type      the type of network rejection
     mcc_mnc:         MCC/MNC
     lac:             LAC or TAC
     rac:             routing area code
     rat:             values defined in sys_radio_access_tech_e_type
     upd_attach_type  update/attach type defined in gstk_update_attach_type_enum_type
     rej_cause        rejection cause defined in 24.008 or 24.301
     as_id:           subscription ID
===========================================================================*/
typedef struct
{
  gstk_nw_rej_enum_type                    nw_rej_type;
  uint8                                    mcc_mnc[GSTK_MAX_PLMN_LEN];
  uint8                                    lac[GSTK_MAX_LAC_LEN];
  uint8                                    rac;
  uint8                                    rat;
  gstk_update_attach_type_enum_type        upd_attach_type;
  uint8                                    rej_cause;
  sys_modem_as_id_e_type                   as_id;
}
gstk_nw_rej_evt_type;

/*===========================================================================
-- The structure of network rejection indication command
===========================================================================*/
typedef struct
{
  gstk_hdr_type                             message_header;
  gstk_nw_rej_evt_type                      nw_rej_evt;
}
gstk_nw_rej_evt_ind_type;

/*===========================================================================
-- The structure of query location information envelope cmd status
===========================================================================*/
typedef struct
{
  void                                      *task_tcb;
  uint32                                    sig_to_wait;
  uint8                                     mcc_mnc[GSTK_MAX_PLMN_LEN];
}
gstk_attach_ctrl_type;

/*===========================================================================
-- The structure of location information envelope cmd status indication
===========================================================================*/
typedef struct
{
  gstk_hdr_type                             message_header;
  gstk_attach_ctrl_type                     attach_ctrl;
}
gstk_attach_ctrl_ind_type;

/*===========================================================================
-- The structure of location envelope response received confirmation
===========================================================================*/
typedef struct
{
  gstk_hdr_type                             message_header;
  boolean                                   imsi_switched;
}
gstk_location_env_resp_rcvd_cnf_type;

/*===========================================================================
-- The structure of GSTK TAL Command
-- message_header - Standard GSTK command message header
-- client_type    - Defines the GSTK TAL Client type
-- slot           - GSTK Slot type
-- gstk_tal_cb    - GSTK TAL Registration Calback
-- uim_cmd        - Details of the command to be sent to UIM
===========================================================================*/
typedef struct
{
  gstk_hdr_type             message_header; /* Standard GSTK Message header */
  gstk_tal_client_type      client_type;   /* GSTK TAL client type */
  gstk_tal_reg_cb_type      gstk_tal_cb;  /* TAL Callback Info */
  gstk_tal_uim_cmd_type     uim_cmd;    /* details of command to be sent to */
}
gstk_tal_cmd_type;

/*===========================================================================
-- The structure of WMS event processing
-- message_header - Standard GSTK command message header
-- wms_event      - WMS event
===========================================================================*/
typedef struct
{
  gstk_hdr_type                 message_header; /* Standard GSTK Message header */
  gstk_wms_event_enum_type      wms_event; /* WMS event */
}
gstk_wms_evt_ind_type;

/*===========================================================================
-- The structure of OTASP REGISTRATION Command
-- message_header       - Standard GSTK command message header
-- user_data            - Defines the GSTK TAL Client type
-- gstk_otasp_evt_cb    -  Event callback
-- gstk_otasp_reg_cb    - Registration Calback
===========================================================================*/
typedef struct
{
  gstk_hdr_type                              message_header; /* Standard GSTK Message header */
  gstk_client_ref_data_type                  user_data;  
  gstk_client_otasp_status_cb_func_type      gstk_otasp_status_cb;  
  gstk_client_otasp_status_reg_cb_func_type  gstk_otasp_reg_cb;
}
gstk_otasp_reg_cmd_type;

/*
** GSTK_TERMINAL_RSP_CNF message
*/
typedef union
{
    gstk_general_cmd_type                          general_cmd;
    gstk_proactive_cmd_type                        proactive_cmd;
    gstk_mode_change_ind_type                      mode_change_ind;
    gstk_tz_ind_type                               time_zone_ind;
    gstk_csg_change_ind_type                       csg_change_ind;
    gstk_ph_nw_sel_pref_change_ind_type            ph_nw_sel_pref_ind;
    gstk_subs_change_ind_type                      subs_change_ind;
    /* Terminal response confirmation type */
    gstk_unsupported_proactive_cmd_cnf_type        unknown_unsupported_term_rsp_cnf;
    gstk_display_text_cnf_type                     display_text_term_rsp_cnf;
    gstk_get_inkey_cnf_type                        get_inkey_term_rsp_cnf;
    gstk_get_input_cnf_type                        get_input_term_rsp_cnf;
    gstk_launch_browser_cnf_type                   launch_browser_term_rsp_cnf;
    gstk_more_time_cnf_type                        more_time_term_rsp_cnf;
    gstk_play_tone_cnf_type                        play_tone_term_rsp_cnf;
    gstk_poll_interval_cnf_type                    poll_interval_term_rsp_cnf;
    gstk_provide_local_info_cnf_type               provide_local_info_term_rsp_cnf;
    gstk_refresh_cnf_type                          refresh_term_rsp_cnf;
    gstk_run_at_cmd_cnf_type                       run_at_cmd_term_rsp_cnf;
    gstk_select_item_cnf_type                      select_item_term_rsp_cnf;
    gstk_send_sms_cnf_type                         send_sms_term_rsp_cnf;
    gstk_send_ss_cnf_type                          send_ss_term_rsp_cnf;
    gstk_send_ussd_cnf_type                        send_ussd_term_rsp_cnf;
    gstk_setup_call_cnf_type                       setup_call_term_rsp_cnf;
    gstk_setup_evt_list_cnf_type                   setup_evt_list_term_rsp_cnf;
    gstk_setup_menu_cnf_type                       setup_menu_term_rsp_cnf;
    gstk_setup_idle_text_cnf_type                  setup_idle_text_term_rsp_cnf;
    gstk_send_dtmf_cnf_type                        send_dtmf_term_rsp_cnf;
    gstk_timer_manage_cnf_type                     timer_manage_term_rsp_cnf;
    gstk_lang_notification_cnf_type                lang_notification_term_rsp_cnf;
    gstk_open_ch_cnf_type                          open_ch_term_rsp_cnf;
    gstk_close_ch_cnf_type                         close_ch_term_rsp_cnf;
    gstk_send_data_cnf_type                        send_data_term_rsp_cnf;
    gstk_receive_data_cnf_type                     receive_data_term_rsp_cnf;
    gstk_get_ch_status_cnf_type                    get_ch_status_term_rsp_cnf;
    gstk_activate_cnf_type                         activate_term_rsp_cnf;
    /* raw client terminal response */
    gstk_client_raw_term_resp_type                 client_raw_term_resp;
    /* Envelope Command */
    gstk_envelope_menu_selection_ind_type          menu_selection_envelope_ind;
    gstk_envelope_sms_pp_download_ind_type         sms_pp_envelope_ind;
    gstk_envelope_cell_broadcast_download_ind_type cell_broadcast_envelope_ind;
    gstk_envelope_cc_ind_type                      cc_envelope_ind;
    gstk_envelope_mo_sms_ctrl_ind_type             mo_sms_ctrl_envelope_ind;
    gstk_envelope_mt_evt_ind_type                  mt_evt_envelope_ind;
    gstk_envelope_call_connected_evt_ind_type      call_connected_envelope_ind;
    gstk_envelope_call_disconnected_evt_ind_type   call_disconnected_envelope_ind;
    gstk_envelope_idle_scrn_evt_ind_type           idle_scrn_evt_envelope_ind;
    gstk_envelope_user_act_evt_ind_type            user_act_evt_envelope_ind;
    gstk_envelope_location_statue_ind_type         location_st_envelope_ind;
    gstk_envelope_timer_expire_ind_type            timer_expire_envelope_ind;
    gstk_envelope_browser_term_evt_ind_type        browser_term_evt_envelope_ind;
    gstk_envelope_lang_sel_evt_ind_type            lang_sel_evt_envelope_ind;
    gstk_envelope_data_avail_evt_ind_type          data_avail_evt_envelope_ind;
    gstk_envelope_ch_status_evt_ind_type           ch_status_evt_envelope_ind;
    gstk_envelope_access_tech_ind_type             access_tech_evt_envelope_ind;
    gstk_envelope_nw_search_mode_evt_ind_type      nw_search_mode_evt_envelope_ind;
    gstk_envelope_location_information_ind_type    location_information_envelope_ind;
    gstk_envelope_hci_connectivity_ind_type        hci_evt_envelope_ind;
    gstk_envelope_ims_registration_evt_ind_type    ims_reg_evt_envelope_ind;
    gstk_raw_envelope_ind_type                     raw_envelope_ind;
    /* Envelope Response */
    gstk_envelope_resp_type                        envelope_response;

    /* Terminal Profile download command supports differentiating sat and usat */
    gstk_terminal_profile_cmd_type                 terminal_profile_cmd;

    /* Terminal Profile response */
    gstk_term_profile_rsp_type                     terminal_profile_rsp;

    /* mmgsdi event notification */
    gstk_mmgsdi_evt_type                           mmgsdi_evt;

    /* mm enter idle notification */
    gstk_mm_idle_state_type                        mm_idle_state;

    /*  rrc cell change notification */
    gstk_rrc_cell_change_ind_type                  rrc_cell_change_ind;

    /* ds extended system status notification */
    gstk_ds_sys_status_ex_ind_type                 ds_sys_status_ex_ind;

    /* ds WLAN MAC address change notification */
    gstk_ds_wlan_mac_addr_change_ind_type          ds_wlan_mac_addr_change_ind;

    /* ds WLAN SSID change notification */
    gstk_ds_wlan_ssid_change_ind_type              ds_wlan_ssid_change_ind;

    /* stop sig command */
    gstk_power_down_type                           power_off;

    /* Error messages type */
    gstk_error_msg_type                            error_msg;

    /* Report Timer type */
    gstk_report_time_msg_type                      timer_msg;

    /* client id registration */
    gstk_client_id_reg_type                        client_id_reg;

    /* client event registration */
    gstk_client_evt_reg_type                       client_evt_reg;

    /* current access technology for gstk */
    gstk_curr_access_tech_type                     curr_access_tech;

    /* network rejection event */
    gstk_nw_rej_evt_ind_type                       nw_rej_ind;

    /* command sent by TAL module */
    gstk_tal_cmd_type                              tal_cmd;

    /* attach query request */
    gstk_attach_ctrl_ind_type                      attach_ctrl_ind;

    /* location envelope response indication */
    gstk_location_env_resp_rcvd_cnf_type           loc_env_rsp_rcvd_cnf;

    /* cm ph event indication */
    gstk_cm_ph_ind_type                            cm_ph_ind;

    /* NV refresh command from MCFG */
    gstk_nv_refresh_cmd_type                       nv_refresh_cmd;

    /* WMS event indication */
    gstk_wms_evt_ind_type                          wms_evt_ind;

    /* OTASP event registration */
    gstk_otasp_reg_cmd_type    otasp_reg_cmd;
}
gstk_cmd_type;

/* The common command header for all commands received by GSDI task */
typedef struct {
    cmd_hdr_type              cmd_hdr;
    gstk_cmd_type             cmd;
}
gstk_task_cmd_type;

/*===========================================================================
-- Struct for input paramter of gstk_io_ctrl() function when I/O command
   is GSTK_IO_SEND_MM_IDLE_EVENT. When (E)MM module enters IDLE state, it shall
   notify location information defined in this structure.
     mcc_mnc:         MCC/MNC
     lac:             LAC or TAC
     location_state:  values defined in sys_srv_status_e_type
     rat:             values defined in sys_radio_access_tech_e_type
     cell_id:         Cell Identity
     as_id:           Subscription ID from CM
     stack_id:        NAS stack ID
===========================================================================*/
typedef struct {
  uint8                     mcc_mnc[GSTK_MAX_PLMN_LEN];
  uint8                     lac[GSTK_MAX_LAC_LEN];
  uint8                     location_state; /* full/limited/no service */
  uint8                     rat; /* gsm or wcdma or lte*/
  gstk_cell_id_type         cell_id;
  sys_modem_as_id_e_type    as_id;
  sys_modem_stack_id_e_type stack_id;
}
gstk_io_send_mm_idle_evt_type;


/*===========================================================================
-- Struct for input paramter of gstk_io_ctrl() function when I/O command
   is GSTK_IO_SEND_MM_INDICATION.
     type:         MERGE/SPLIT or MODE CHANGE (SGLTE/CSFB)
     as_id:        Subscription ID from CM
     stackk id:    NAS stack ID
===========================================================================*/
typedef struct
{
  gstk_io_send_mm_ind_enum_type type;
  sys_modem_as_id_e_type        as_id;
  sys_modem_stack_id_e_type     stack_id;
}
gstk_io_send_mm_ind_param_type;

/*===========================================================================
-- struct for caching proactive command and respective terminal response
   cmd:              Last proactive command from card
   cmd_length:       Length of last proactive command
   term_resp:        Last terminal response to card
   term_resp_length: Length of last terminal response
   dupe_count:       counter to keep track of proacitve commands repeated by card
===========================================================================*/
typedef struct
{
    uint8    cmd[GSTK_MAX_DATA_SIZE];
    uint32   cmd_length;
    uint8    term_resp[UIM_MAX_TR_BYTES];
    uint32   term_resp_length;
    uint8    dupe_count;
    boolean  tr_rcvd;
}
gstk_cmd_cache_type;

/*===========================================================================
-- Struct for input paramter of gstk_io_ctrl() function when I/O command
   is GSTK_IO_SEND_EMM_REGISTERED_EVENT. When (E)MM module enters IDLE state,
   it shall notify location information defined in this structure.
     mcc_mnc:         MCC/MNC
     lac:             LAC or TAC
     location_state:  values defined in sys_srv_status_e_type
     rat:             values defined in sys_radio_access_tech_e_type
     cell_id:         Cell Identity
===========================================================================*/
typedef gstk_io_send_mm_idle_evt_type gstk_io_send_emm_registered_evt_type;

/*===========================================================================
-- Struct for input paramter of gstk_io_ctrl() function when I/O command
   is GSTK_IO_SEND_NW_REJ_EVENT. This is used by (E)MM to notify the network
   rejection events.
===========================================================================*/
typedef gstk_nw_rej_evt_type gstk_io_send_nw_rej_evt_type;

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_SMS_PP_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a sms pp download command to the
  SIM/USIM.

PARAMETERS
  user_data:         [Input] User data that client wants GSTK to keep track of
  is_cdma_sms_pp:    [Input] Indicates whether it is CDMA or non CDMA SMS
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
  sms_tpdu:          [Input] Pointer indicating the SMS TPDU for the PP download
                             command
  gstk_sms_pp_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             SMS PP download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_send_envelope_sms_pp_dl_command (
  uint32                         user_data,
  boolean                        is_cdma_sms_pp,
  const gstk_address_type      * address,
  const gstk_sms_tpdu_type     * sms_tpdu,
  gstk_evt_cb_funct_type         gstk_sms_pp_dl_cb
);

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
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_envelope_sms_pp_dl_command (
    gstk_slot_id_enum_type         slot,
    uint32                         user_data,
    boolean                        is_cdma_sms_pp,
    const gstk_address_type       *address,
    const gstk_sms_tpdu_type      *sms_tpdu,
    gstk_evt_cb_funct_type         gstk_sms_pp_dl_cb
);

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_CELL_BROADCAST_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a cell broadcast download command to the
  SIM/USIM.

PARAMETERS
  user_data:     [Input] User data that client wants GSTK to keep track of
  cb_page:       [Input] Pointer indicating the Cell Broadcast page for the
                         download command.  The length field should be set to
                         GSTK_CELL_BROADCAST_PAGE_LEN
  gstk_cb_dl_cb: [Input] Function pointer to which GSTK will send the card response
                         data corresponding to the Cell Broadcast download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_send_envelope_cell_broadcast_dl_command (
  uint32                      user_data,
  const gstk_cb_page_type   * cb_page_ptr,
  gstk_evt_cb_funct_type      gstk_cb_dl_cb
);

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
  This function should only be called after gstk_client_init has been called.
  Client is responsible to pad the page to length 88 (31.111)

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_envelope_cell_broadcast_dl_command (
  gstk_slot_id_enum_type     slot,
  uint32                     user_data,
  const gstk_cb_page_type   *cb_page_ptr,
  gstk_evt_cb_funct_type     gstk_cb_dl_cb
);

/*===========================================================================
FUNCTION gstk_parse_ton_npi

DESCRIPTION
  This function parse the ton and npi byte into gstk enum types

PARAMETERS
  TON_NPI: [Input] byte that contains TON and NPI info
  TON:     [Input/Output] Pointer to TON
  NPI:     [Input/Output] Pointer to NPI


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
extern gstk_status_enum_type gstk_parse_ton_npi (
  uint8                     TON_NPI,
  gstk_ton_enum_type      * TON,
  gstk_npi_enum_type      * NP
);

/*===========================================================================
FUNCTION gstk_ascii_to_gstk_address_type

DESCRIPTION
  This function converts the ASCII value to gstk address format

PARAMETERS
  ascii_addr: [Input] Pointer to input address
  ascii_len:  [Input] input address length
  gstk_addr:  [Input/Output] Pointer to output address


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
extern gstk_status_enum_type gstk_ascii_to_gstk_address_type (
  const uint8         * ascii_addr,
  uint8                 ascii_len,
  gstk_address_type   * gstk_addr
);


/*===========================================================================
FUNCTION gstk_task_get_cmd_buf

DESCRIPTION
  This function get the gstk_task_cmd_type from the GSTK free queue.
  Calling client SHOULD NOT change information on the gstk_task_cmd_type's
  header.

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_task_cmd_type pointer

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_task_cmd_type * gstk_task_get_cmd_buf (
  void
);


/*===========================================================================
FUNCTION gstk_cmd

DESCRIPTION

  This function allows clients to put the gstk_cmd onto the GSTK command
  queue.

PARAMETER
  gstk_cmd [Input]:  Pointer of the task_cmd_type

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
extern void gstk_cmd (
  gstk_task_cmd_type * gstk_cmd
);


/*===========================================================================
FUNCTION gstk_get_curr_cell_id_info

DESCRIPTION
  Function called to retrieve the Cell ID information based on the RAT
  and current PLMN Information.

PARAMETER
  Input:   sys_radio_access_type_e_type radio_access_tech : Current RAT
  Input:   uint8  * mcc_mnc_p                 : Current PLMN Information
  Input:   uint8  * lac_p                     : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_curr_cell_id_info (
  gstk_sys_radio_access_tech_e_type  radio_access_tech,
  const uint8*                       mcc_mnc,
  const uint8*                       lac,
  gstk_cell_id_type*                 cell_id_info_p,
  mmgsdi_session_type_enum_type      sess_type
);

/*===========================================================================
FUNCTION gstk_send_proactive_cmd

DESCRIPTION

  This function allows clients to put a proactive cmd request before parsing
  onto GSTK command queue.

PARAMETERS
  data_length: [Input] Length of the data
  data: [Input] Pointer to the data buffer that contains the Proactive cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_send_proactive_cmd(
  uim_slot_type slot,
  uint32        data_length,
  const uint8  *data );

/*============================================================================
  FUNCTION:      GSTK_IO_CTRL

  DESCRIPTION:
    General interface to set/get GSTK data. The I/O operations currently
    supported are:
    1) GSTK_IO_GET_CURR_ACCESS_TECH: read evt_cb_type of
       gstk_client_table. param_in_ptr points to client ID and param_out_ptr
       points to a variable of gstk_access_technology_type.

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
========================================================================== */
extern gstk_status_enum_type gstk_io_ctrl(
  gstk_io_ctrl_enum_type    io_cmd,
  const void               *param_in_ptr,
  void                     *param_out_ptr
);

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
  const void                * data_ptr);

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_MENU_SEL_COMMAND_EXT

DESCRIPTION

  This function sends the menu selection envelope command down to SIM/USIM.
  Clients need to provide the item identifier for the user selected item.

PARAMETERS
  client_id     : [Input] Client id of the calling client. This should be the value
                          returned from the gstk_client_init()
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
  This function should only be called after gstk_client_init has been called
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
extern gstk_status_enum_type gstk_send_envelope_menu_sel_command_ext (
        gstk_client_id_type            client_id,
        uint32                         user_data,
        uint8                          item_chosen,
        boolean                        help_requested,
        gstk_evt_cb_funct_type         gstk_raw_env_cb);

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
);

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
gstk_attach_ctrl_result_enum_type gstk_get_attach_ctrl_res(void);

/*===========================================================================
FUNCTION: gstk_enable_estk_evt_handling

DESCRIPTION:
  This function enable ESTK to register events that it's going to handle with
  GSTK. ESTK will only handle MODEM functionality of those proactive commands:
  1) SETUP CALL
  2) SEND DTMF
  3) SEND SMS
  4) SEND SS
  5) SEND USSD
  6) BIP

PARAMETERS:
  slot_id: [Input] the slot where ESTK event processing is enabled
                   GSTK_SLOT_AUTO means to enable event handling on all slots
  evt_bmsk:[Input] the bitmask of all events to be enabled

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
gstk_status_enum_type gstk_enable_estk_evt_handling(
   gstk_slot_id_enum_type slot,
   uint64                 evt_bmsk
);

/*===========================================================================
FUNCTION: gstk_otasp_status_reg

DESCRIPTION:
  This function enable client to register with GSTK to receive OTASP status event through call back.
  
PARAMETERS:
  user_data: [Input]

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
gstk_status_enum_type gstk_otasp_status_reg (
  gstk_client_ref_data_type                 user_data,
  gstk_client_otasp_status_reg_cb_func_type client_reg_cb,
  gstk_client_otasp_status_cb_func_type     otasp_status_cb
);
#endif /* GSTK_EXP_V_H */

