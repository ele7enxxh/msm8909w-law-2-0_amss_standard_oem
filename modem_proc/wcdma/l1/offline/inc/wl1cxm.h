#ifndef WL1CXM_H
#define WL1CXM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            WCDMA L1 CXM

GENERAL DESCRIPTION:
********************
  CXM stands for Co-Existence Management
  It serves as an interface for the identification and mitigation of various
  de-sense issues

  For all types of de-sense issues, the identification aspect is usually
  handled/implemented in MCS, whereas the mitigation aspect can be handled
  by various entities depending on the source of de-sense

  In order to help identify and mitigate these potential de-sense issues
  which might impact WCDMA RF performance, L1 needs to interact and provide
  a few input parameters such as current WCDMA freq/band, power and priority
  info to these identification(MCS) and mitigation(MCS/WFW/CFW/RPM) entities
  using the CXM interafce

  This file contains the source code for all these L1 interactions
  which use the CXM interface

EXTERNALIZED FUNCTIONS:
***********************
  wl1_cxm_pwr_init

  wl1_cxm_pwr_cleanup

  wl1_cxm_report_pwr_info

  wl1_cxm_link_info_mgr

  wl1_cxm_freqid_msg_handler

  wl1_cxm_dch_cleanup

  wl1_cxm_get_curr_link_count

  wl1_cxm_get_curr_link_list_freqid_info

  wl1_cxm_is_link_freq_info_needed

  wl1_multisim_cxm_init

  wl1_multisim_cxm_is_g_tx_on

  wl1_multisim_cxm_log_pkt_handler

  wl1_multisim_cxm_filter_rx_power

  wl1_multisim_cxm_event_handler

  wl1_multisim_cxm_get_mode_pref

  wl1_multisim_cxm_update_tech_state

  wl1_multisim_cxm_get_curr_priority_tbl

  wl1_multisim_cxm_report_curr_freq_as_pri_rx_freqid

  wl1_multisim_cxm_phychan_timing_update

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1cxm.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what
--------   ---     ----------------------------------------------------------
02/24/15   ap      Initial check-in for WL1 QSH framework 
02/19/16   gm      Mods to back out CR970711.
02/07/16   pvr     Mods to reset CXM database, if FreqID callback from MCS is still pending
                   before going to deactivate WCDMA stack.
11/27/14   ar      Support for sending PWR report to MCS
11/27/14   abs     Set WFW_COEX_ACTIVITY_SEARCHER priority
                   to TIER_10 only for PICH Wake-up's
10/13/14   abs     Add support to register InterF nbrs for desensing during meas
10/07/14   abs     Skip CXM updates to WFW if WFW not in DSDX mode
09/15/14   abs     Skip CXM updates to WFW during QTA
08/18/14   abs     WL1CXM support for DR-DSDS feature
08/14/14   abs     Generate/Update new ACTIVITY -- PRIORITY table to WFW
                   upon receiving new TIER -- PRIORITY table update from MCS
07/25/14   abs     Backing out previous check-in
07/24/14   abs     WL1CXM support for DR-DSDS feature
07/23/14   abs     Mods to support 0X41B6 log packet Version-2
06/19/14   pg      Mods to change the Rx filtering algorithm for reporting 
                   power to CXM.
06/03/14   yw      Mods to support secondary carrier Tx freq/pwr reporting
05/27/14   pkg     Changes to report Tx power during RACH.
04/04/14   abs     Mods to block TX only link_info registrations
04/15/14   abs     Fixing Bolt 2.0 DS compilation errors
04/04/14   abs     Mods to support CM W2W Interf Meas freq reporting
04/04/14   abs     Mods to support secondary carrier Rx freq/pwr reporting
03/28/14   abs     Fixed over-sized local variables
03/26/14   abs     Mods to initialize link_info_mgr for wl1m_init
01/24/14   abs     Mods to support DDR(FR 17739) requirements for DSDA
01/21/14   abs     Initial version created
===========================================================================*/

/*===========================================================================

                INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mdspsync.h"
#include "rex.h"
#include "stringl.h"
#include "wl1_msgr.h"
#include "wl1trm.h"
#include "wwan_coex_mgr.h"

#ifdef FEATURE_WCDMA_COEXISTENCE_SW
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/****************************************************************************
                MACROS
****************************************************************************/
/*DL Frequency Channel Type*/
#define WL1_CXM_DL_UARFCN 0x00

/*UL Frequency Channel Type*/
#define WL1_CXM_UL_UARFCN 0x01

/*Max number of freqId callback
**entities/modules link_info_mgr can support*/
#define WL1_CXM_MAX_FREQID_CB_ENTITIES 10

/*Max number of links link_info_mgr can support*/
#define WL1_CXM_MAX_NUM_RADIO_LINKS 10

/*WL1 CXM Rx Power reporting Timer(20 msecs)*/
#define WL1_CXM_PWR_REPORTING_TMR 20

/*WCDMA BandWidth*/
#define WL1_CXM_WCDMA_BANDWIDTH 5000000

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
/*Multisim CXM FreqId update callback entity type*/
#define WL1_MULTISIM_CXM_FREQID_CB_ENTITY 0xDEAD

/* WL1 CXM Logging USTMR Interval, in ticks */
#define WL1_MULTISIM_CXM_WFW_USTMR_INT 6400

/* WL1 CXM Logging WSTMR Interval, in cx1*/
#define WL1_MULTISIM_CXM_WFW_WSTMR_INT 1280

/* Max value a UINT32 variable can hold */
#define WL1_MULTISIM_CXM_UINT32_MAX 0X7FFFFFFF

/* Filtering Coeff. For Rx filter algorithm, Numerator and Denominator.
 * This is the universal IIR filtering equation
 *     Filtered value=(Filtering Coeff*Old Filtered value) + (New Slot sampled value*(1-Filtering Coeff.))
 * Where Filtering coeff. is computed as follows
 *     = (1 Frame Time/No_of_slots_in_a_Frame) / (Time_constant+ (1 Frame Time/No_of_slots_in_a_Frame)
 * In our case the filtering coefficient will be (10/15) / (20 + (10/15)) where 20 is our time constant
 */
#define WL1_MULTISIM_CXM_RX_FILT_COEF_NR 30
#define WL1_MULTISIM_CXM_RX_FILT_COEF_DR 31

/* Minimum WCDMA RX Power Level that the CXM algorithm can support in dbm */
#define WL1_MULTISIM_CXM_MIN_RX_POWER_DBM -106

/* Maximum WCDMA RX Power Level that the CXM algorithm can support in dbm */
#define WL1_MULTISIM_CXM_MAX_RX_POWER_DBM -29

/* Minimum WCDMA RX AGC Level that the CXM algorithm can support in db10 */
#define WL1_MULTISIM_CXM_RFAGC_MIN_AGC_VAL_DB10 -512

/* Maximum WCDMA RX AGC Level that the CXM algorithm can support in db10 */
#define WL1_MULTISIM_CXM_RFAGC_MAX_AGC_VAL_DB10 258

/*Scaling Factor for Wl1_CXM_Rx_Power_Reporting*/
#define WL1_MULTISIM_CXM_SCALING_FACTOR_1 1100

/*Scaling Factor for Wl1_CXM_Rx_Power_Reporting*/
#define WL1_MULTISIM_CXM_SCALING_FACTOR_2 10

/*Scaling Factor for Wl1_CXM_Tx_Power_Reporting*/
#define WL1_MULTISIM_CXM_TX_PWR_SCAL_FACT 691

/*Max number of Physical Channels*/
#define WL1_MULTISIM_CXM_MAX_NUM_PHYCHANS 6
#endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */

/****************************************************************************
                ENUMS
****************************************************************************/
/*CXM Radio Link types*/
typedef enum
{
  /*Primary RX Radio Link type*/
  WL1_CXM_PRI_RX_LINK,

  /*Diversity RX Radio Link type*/
  WL1_CXM_DIV_RX_LINK,

  /*Dual Carrier RX Radio Link type*/
  WL1_CXM_SEC_RX_LINK,

  /*CM W2W Interfreq RX Radio Link type*/
  WL1_CXM_W2W_CM_RX_LINK,

  /*IDLE W2W Interfreq Neighbour 1 RX Radio Link type*/
  WL1_CXM_W2W_NBR1_RX_LINK,

  /*IDLE W2W Interfreq Neighbour 2 RX Radio Link type*/
  WL1_CXM_W2W_NBR2_RX_LINK,

  /*Primary TX Radio Link type*/
  WL1_CXM_PRI_TX_LINK,

  /*Dual Carrier TX Radio Link type*/
  WL1_CXM_SEC_TX_LINK,

  /*Number of Radio Links*/
  WL1_CXM_MAX_LINKS
} wl1_cxm_link_entity_enum_type;

/*CXM Link Action Mask types*/
typedef enum
{
  /*Add Link Entity Info to Link List db*/
  WL1_CXM_ADD_LINK_INFO,

  /*Remove Link Entity Info to Link List db*/
  WL1_CXM_REMOVE_LINK_INFO,

  /*Update Link Entity Info to Link List db*/
  WL1_CXM_UPDATE_LINK_INFO,

  /*De-Register all the existing Link
  **Entities from Link List db*/
  WL1_CXM_DE_REG_ALL_LINKS,

  /*Invalid Action type*/
  WL1_CXM_INVALID_ACTION_TYPE
} wl1_cxm_link_action_enum_type;

/*CXM Link Manager States*/
typedef enum
{
  /*Link Info Mgr is Idle*/
  WL1_CXM_IDLE_STATE,

  /*Link Info Mgr is Waiting on FreqId Report
  **from MCS for recently reported link_list info
  **place any new Link Reequests in Pending database*/
  WL1_CXM_WAIT_STATE,

  /*Invalid Link Info Mgr state*/
  WL1_CXM_INVALID_STATE
} wl1_cxm_link_mgr_state_enum_type;

/*CXM MSG types*/
typedef enum
{
  /*Intialize Link Info Mgr module*/
  WL1_CXM_INIT_MSG,

  /*Link Freq Request Msg type, 
  **typically used by entities to inform link_info_mgr
  **to register/de-register their freq info with MCS*/
  WL1_CXM_FREQ_MSG,

  /*FreqId Report Msg type, 
  **typically used to forward the freqId report 
  **received from MCS to link_info_mgr*/
  WL1_CXM_FREQID_MSG,

  /*Msg type for an entity/module to register
  **a cb func to receive freqId updates*/
  WL1_CXM_FREQID_CB_REG_MSG,

  /*Msg type for an entity/module to de-register
  **its cb func to not receive freqId updates*/
  WL1_CXM_FREQID_CB_DE_REG_MSG,

  /*Invalid Msg type*/
  WL1_CXM_INVALID_MSG_TYPE
} wl1_cxm_link_mgr_msg_enum_type;

/*CXM PWR Reporting Mode types*/
typedef enum
{
  /*Power Reporting for DDR*/
  WL1_CXM_DDR_PWR_MODE,

  /*Power Reporting for DSDA*/
  WL1_CXM_MULTISIM_CXM_PWR_MODE,

  /*Invalid PWR mode type*/
  WL1_CXM_INVALID_PWR_MODE
} wl1_cxm_pwr_mode_enum_type;

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
/*Multisim CXM state machine event types*/
typedef enum
{
  /*Enable Multisim CXM state machine*/
  WL1_MULTISIM_CXM_ENABLE_EVT = 0,

  /*Callback event from wl1_cxm_link_info_mgr()
  **indicating a change in FreqId's, update new FreqId's to WFW */
  WL1_MULTISIM_CXM_FREQID_UPDATE_CB_EVT = 1,

  /*Report Channel Id to WFW*/
  WL1_MULTISIM_CXM_UPDATE_CHANID_TO_WFW_EVT = 2,

  /*Report CXM params (FreqId, PriTbl, ChanId) to WFW
  **using async I/F for PICH Wake-up's*/
  WL1_MULTISIM_CXM_UPDATE_PARAMS_TO_WFW_FOR_PICH_WAKEUP = 3,

  /*Report CXM params (FreqId, PriTbl, ChanId) to WFW
  **using async I/F for NON-PICH Wake-up's*/
  WL1_MULTISIM_CXM_UPDATE_PARAMS_TO_WFW_FOR_NON_PICH_WAKEUP = 4,

  /*Update WFW with default priorities received from CXM*/
  WL1_MULTISIM_CXM_HDLE_DEFAULT_DSDA_PRI_EVT = 5,

  /*Update WFW with default priorities received from CXM*/
  WL1_MULTISIM_CXM_HDLE_DEFAULT_DSDS_PRI_EVT = 6,

  /*Set priority of WFW_COEX_ACTIVITY_SEARCHER
  **to LOW and update WFW with the latest priority table*/
  WL1_MULTISIM_CXM_HDLE_SRCHR_PRI_LOW_EVT = 7,

  /*Set priority of WFW_COEX_ACTIVITY_SEARCHER
  **to HIGH and update WFW with the latest priority table*/
  WL1_MULTISIM_CXM_HDLE_SRCHR_PRI_HIGH_EVT = 8,

  /*Set priority of WFW_COEX_ACTIVITY_R99_DPCCH_TX
  **to LOW and update WFW with the latest priority table*/
  WL1_MULTISIM_CXM_HDLE_R99_DPCCH_PRI_LOW_EVT = 9,

  /*Set priority of WFW_COEX_ACTIVITY_R99_DPCCH_TX
  **to LOW and update WFW with the latest priority table*/
  WL1_MULTISIM_CXM_HDLE_R99_DPCCH_PRI_HIGH_EVT = 10,

  /*Generate/Update the new ACTIVITY -- PRIORITY table to WFW
  **upon receiving the new TIER -- PRIORITY table from MCS*/
  WL1_MULTISIM_CXM_HDLE_PRI_TBL_UPDATE_EVT = 11,

  /*Report RACH MSG PWR to MCS*/
  WL1_MULTISIM_CXM_REPORT_RACH_MSG_PWR_EVT = 12,

  /*Freeze CXM updates to WFW as part QTA start*/
  Wl1_MULTISIM_CXM_FREEZE_CXM_FOR_QTA_EVT = 13,

  /*Un-Freeze CXM updates to WFW as part of QTA stop*/
  Wl1_MULTISIM_CXM_UNFREEZE_CXM_FOR_QTA_EVT = 14,

  /*START WFW command trigger,
  **used to skip CXM params update if WFW not in DSDX mode*/
  Wl1_MULTISIM_CXM_START_WFW_COMMAND_SENT_EVT = 15,

  /*Update IDLE Mode W2W InterF Neighbour 1 frequency Id
  **as Serving Cell frequency Id to WFW*/
  Wl1_MULTISIM_CXM_UPDATE_W2W_NBR1_FREQID_TO_WFW_EVT = 16,

  /*Update IDLE Mode W2W InterF Neighbour 2 frequency Id
  **as Serving Cell frequency Id to WFW*/
  Wl1_MULTISIM_CXM_UPDATE_W2W_NBR2_FREQID_TO_WFW_EVT = 17,

  /*Update W Pri RX frequency Id
  **as Serving Cell frequency Id to WFW*/
  Wl1_MULTISIM_CXM_UPDATE_PRI_RX_FREQID_TO_WFW_EVT = 18,

  /*Report Serving Cell RX PWR to MCS before going to SLEEP*/
  WL1_MULTISIM_CXM_REPORT_PRI_RX_PWR_EVT = 19,

  /*Disable Multisim CXM state machine*/
  WL1_MULTISIM_CXM_DISABLE_EVT = 20
} wl1_multisim_cxm_evt_enum_type;

typedef enum
{
  /*Uplink Dedicated Physical Control Channel && 
  **Uplink Enhanced Dedicated Channel*/
  WL1_MULTISIM_CXM_UL_DPCCH_EDCH,

  /*E-DCH Absolute Grant Channel*/
  WL1_MULTISIM_CXM_AGCH,

  /*E-DCH Serving Hybrid ARQ Indicator Channel*/
  WL1_MULTISIM_CXM_S_HICH,

  /*High Speed Shared Control Channel*/
  WL1_MULTISIM_CXM_HS_SCCH,

  /*High Speed Physical Downlink Shared Channel*/
  WL1_MULTISIM_CXM_HS_PDSCH,

  /*High Speed Dedicated Physical Control Channel*/
  WL1_MULTISIM_CXM_HS_DPCCH
} wl1_multisim_cxm_phychan_enum_type;
#endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */

/****************************************************************************
                CALLBACK FUNCTION DECLRATIONS FOR MODULE
****************************************************************************/
/*===========================================================================
CALLBACK     WL1_CXM_POST_FREQID_CB_FN

DESCRIPTION
  Callback function typedef that entities or modules can use
  to register for freqId updates

DEPENDENCIES
  The freqId info of the resp. link or link_list must be deduced
  by the entity/module in this callback funtion itself by querying
  wl1_cxm_get_curr_link_list_info( )

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
typedef void (*wl1_cxm_post_freqid_cb_fn)(void);

/****************************************************************************
                STRUCTS
****************************************************************************/
typedef struct
{
  /*If TRUE: Indication from link entity to link info mgr
  **to generate its frequency parameters from uarfcn
  **Else If FALSE: Indication from link entity to link info mgr 
  **to use link_freq_info(from WL1_CXM_FREQ_MSG) as its frequency parameters*/ 
  boolean generate_link_freq_info;

  /*Freq channel type(DL || UL)*/
  uint8 uarfcn_type;

  /*Action mask for the link request*/
  wl1_cxm_link_action_enum_type action_mask;

  /*Freq channel of link entity*/
  uint16 uarfcn;

  /*Link entity's Frequency ID*/
  uint32 freqid;

  /*Link Entity Type*/
  wl1_cxm_link_entity_enum_type link_type;

  /*Freq params of Link entity*/
  cxm_tech_link_info_s link_freq_info;

  /*Link entity callback function
  **to receive freqId updates*/
  wl1_cxm_post_freqid_cb_fn link_cb;
} wl1_cxm_link_info_struct_type;

typedef struct
{
  /*Link Entity Type*/
  wl1_cxm_link_entity_enum_type link_type;

  /*Link entity's Frequency ID*/
  uint32 freqid;
} wl1_cxm_link_freqid_info_struct_type;

typedef struct
{
  /*Number of link entiy's in freqId list*/
  uint8 num_links;

  /*Array of link entity freqId's*/
  wl1_cxm_link_freqid_info_struct_type link_list[WL1_CXM_MAX_NUM_RADIO_LINKS];
} wl1_cxm_link_list_freqid_info_struct_type;

typedef struct
{
  /*Number of link entiy's in link_list*/
  uint8 num_links;

  /*Array of link entities and their link info*/
  wl1_cxm_link_info_struct_type link_list[WL1_CXM_MAX_NUM_RADIO_LINKS];
} wl1_cxm_link_list_info_struct_type;

typedef struct
{
  /*Callback module/entity type*/
  uint16 cb_entity;
  /*Post freqId update Callback function*/
  wl1_cxm_post_freqid_cb_fn cb_fn;
}wl1_cxm_freqid_cb_struct_type;

typedef union
{
  /*Link Request Msg*/
  wl1_cxm_link_info_struct_type freq;

  /*FreqId Report Msg*/
  wl1_cxm_link_list_freqid_info_struct_type freqid;

  /*FreqId updates Callback Registration Msg*/
  wl1_cxm_freqid_cb_struct_type freqid_cb;
} wl1_cxm_link_mgr_msg_union_type;

typedef struct
{
  /*Link Mgr msg type*/
  wl1_cxm_link_mgr_msg_enum_type type;

  /*Link Mgr msg parameters*/
  wl1_cxm_link_mgr_msg_union_type params;
} wl1_cxm_link_mgr_msg_struct_type;

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
typedef struct
{
  /*Physical Channel Type*/
  wl1_multisim_cxm_phychan_enum_type type;

  /*Physical Channel Enabled || Disabled indicator*/
  boolean active;

  /*Physical Channel Action or Start SUBFN*/
  uint16 subfn;

  /*Physical Channel Action or Start BPG*/
  uint8 bpg;
} wl1_multisim_cxm_phychan_info_struct_type;

typedef struct
{
  /*Number of Active Uplink Physical Channels*/
  uint8 num_ul_phychans;

  /*Number of Active Downlink Physical Channels*/
  uint8 num_dl_phychans;

  /*Array of Physical Channels and their Timing Info*/
  wl1_multisim_cxm_phychan_info_struct_type phychan_list[WL1_MULTISIM_CXM_MAX_NUM_PHYCHANS];
} wl1_multisim_cxm_phychan_list_info_struct_type;
#endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */

/****************************************************************************
                WL1 CXM BLANKING INFO LOG PACKET DEFINITIONS
****************************************************************************/

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
/*0x41B6 Blanking info definitions*/
#define CXM_LOGPKT_NUM_BPGS_PER_SAMPLE 5
#define CXM_LOGPKT_MAX_NUM_SAMPLE_DATA 18
#define CXM_LOGPKT_MAX_NUM_SAMPLES 30
#define CXM_LOGPKT_INVALID_ENTRY 0xFFFFFFFF
#define CXM_LOGPKT_REF_SAMPLE_ENTRY 0x0F0F0F0F
#define CXM_LOGPKT_SAMPLE_IDX_MASK 24
#define CXM_LOGPKT_SAMPLE_BMSK_MASK 0X00FFFFFF

/*0x41B6 log packet column indices*/
#define CXM_LOGPKT_USTMR_COLUMN_IDX 0
#define CXM_LOGPKT_WSTMR_COLUMN_IDX 1
#define CXM_LOGPKT_RX_PRI_COLUMN_IDX 2
#define CXM_LOGPKT_TX_PRI_COLUMN_IDX 3
#define CXM_LOGPKT_RXB_START_COLUMN_IDX 4
#define CXM_LOGPKT_RXB_END_COLUMN_IDX 5
#define CXM_LOGPKT_TXB_START_COLUMN_IDX 6
#define CXM_LOGPKT_TXB_END_COLUMN_IDX 7

/*0x41B6 Pattern info definitions*/
#define CXM_LOGPKT_RX_BLANK_PATTERN_IND 1
#define CXM_LOGPKT_TX_BLANK_PATTERN_IND 2
#define CXM_LOGPKT_BPG_PATTERN_INDEX 10
#define CXM_LOGPKT_BLANKING_PATTERN_LENGTH 10
#define CXM_LOGPKT_BLANKING_PATTERN_OFFSET 10
#define CXM_LOGPKT_BLANKING_PATTERN_LENGTH_OFFSET 2

/*0x41B6 Channel info definitions*/
#define CXM_LOGPKT_DL_PHYCHAN_IND 0
#define CXM_LOGPKT_UL_PHYCHAN_IND 1
#define CXM_LOGPKT_PHYCHAN_PARTIAL_SUBFN_BPG_FORMAT_IND 10
#define CXM_LOGPKT_NUM_SUBFN_PER_PHYCHAN 6
#define CXM_LOGPKT_PHYCHAN_TYPE_OFFSET 7
#define CXM_LOGPKT_NUM_SUBFN_PER_PHYCHAN_OFFSET 4

/*Generic*/
#define CXM_LOGPKT_NUM_BITS_PER_BYTE 8
#define CXM_LOGPKT_MAX_BIT_IDX_IN_BYTE 7
#define CXM_LOGPKT_NUM_BYTES_PER_WORD 2
#define CXM_LOGPKT_NUM_BYTES_PER_DWORD 4

/*MACROS used in Table-1 and Table-2 Computations*/
#define CXM_LOGPKT_SET_SAMPLE_IDX(ptr, idx) ( *ptr |= (((uint32) idx) << CXM_LOGPKT_SAMPLE_IDX_MASK) )
#define CXM_LOGPKT_GET_SAMPLE_IDX(ptr) ( (uint8) (*ptr >> CXM_LOGPKT_SAMPLE_IDX_MASK) )
#define CXM_LOGPKT_SET_SAMPLE_BMSK(ptr, bmask) ( *ptr |= bmask )
#define CXM_LOGPKT_GET_SAMPLE_BMSK(ptr) ( *ptr & (CXM_LOGPKT_SAMPLE_BMSK_MASK) )
#define CXM_LOGPKT_UPDATE_SAMPLE_DATA_CNT(cnt, bmask) ( cnt += num_set_bits32(bmask) )
#define CXM_LOGPKT_BLANK_POS_FROM_START_POS(start_idx, start_ustmr, blank_ustmr) ( ((start_idx+2) * CXM_LOGPKT_NUM_BPGS_PER_SAMPLE) +((blank_ustmr -start_ustmr) / 1280) )
#define CXM_LOGPKT_GET_HALF_SLOT_IDX(cfnStartUstmr, start_ustmr) (CXM_LOGPKT_MAX_NUM_SAMPLES - ((cfnStartUstmr -start_ustmr) / 6400) )
#define CXM_LOGPKT_GET_BYTE_OFFSET(val) (val / CXM_LOGPKT_NUM_BITS_PER_BYTE)
#define CXM_LOGPKT_GET_BIT_OFFSET(val) (val % CXM_LOGPKT_NUM_BITS_PER_BYTE)
#define CXM_LOGPKT_SET_BITS_IN_VAL(val, pos1, pos2) (val = ((0XFF >> (CXM_LOGPKT_MAX_BIT_IDX_IN_BYTE + pos1 - pos2)) << (CXM_LOGPKT_MAX_BIT_IDX_IN_BYTE - pos2)) )
#define CXM_LOGPKT_GET_IDX_FROM_BYTE_BIT_OFFSET(byte_offset, bit_offset) ( (byte_offset * CXM_LOGPKT_NUM_BITS_PER_BYTE) + bit_offset )

/*Packed Structure for blanking info*/
typedef PACKED struct PACKED_POST
{
  /*Sample Data Header Info*/
  uint32 sample_hdr;

  /*Sample data*/
  uint32 sample_data[CXM_LOGPKT_MAX_NUM_SAMPLE_DATA];
} blanking_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /*Pattern Data Header Info*/
  uint16 pattern_hdr;

  /*RX Blanking Pattern Info*/
  uint16 rx_blank_pattern[CXM_LOGPKT_BLANKING_PATTERN_LENGTH];

  /*TX Blanking Pattern Info*/
  uint16 tx_blank_pattern[CXM_LOGPKT_BLANKING_PATTERN_LENGTH];
} blanking_bpg_pattern_info_struct_type;

typedef PACKED struct PACKED_POST
{
  /*Physical Channel Type*/
  wl1_multisim_cxm_phychan_enum_type chan_type;

  /*Start SUBFN value of Physical Channel*/
  uint16 start_subfn;

  /*Start BPG value of Physical Channel*/
  uint16 start_bpg;
} channel_info_struct_type;

LOG_RECORD_DEFINE(WCDMA_CXM_LOG_PACKET)
  /*Version Number of log packet*/
  uint8 version;
  /*Number of samples*/
  uint8 num_samples;
  /*Number of channels*/
  uint8 num_channels;
  /*Half Slot index start value*/
  uint8 half_slot_idx;
  /*CFN for the Log Packet w.r.t DL DPCH*/
  uint16 cfn;
  /*SFN for the Log Packet*/
  uint16 sfn;
  /*WSTMR at CFN start w.r.t DL DPCH*/
  uint32 wstmr_at_cfn_start;
  /*USTMR at CFN start in ticks*/
  uint32 ustmr_at_cfn_start;
  /*WL1 CXM Logging WSTMR Interval in cx8*/
  uint16 wstmr_int;
  /*WL1 CXM Logging USTMR Interval, in ticks*/
  uint16 ustmr_int;
  /*Array of blanking info structures*/
  blanking_info_struct_type blanking_info[CXM_LOGPKT_MAX_NUM_SAMPLES];
  /*RX/TX bpg blanking info pattern Structure*/
  blanking_bpg_pattern_info_struct_type pattern_info;
  /*Array of channel info structures*/
  channel_info_struct_type channel_info[WL1_MULTISIM_CXM_MAX_NUM_PHYCHANS];
LOG_RECORD_END

/*0x41B6 Log Packet Header Size*/
#define CXM_LOGPKT_HEADER_SIZE() ( FPOS(WCDMA_CXM_LOG_PACKET_type, blanking_info) )

/*Blanking Info Size*/
#define CXM_LOGPKT_BLANKING_INFO_SIZE(num_samples, total_sample_data_cnt) ( \
            (num_samples + total_sample_data_cnt) * (sizeof(uint32)) )

/*Pattern Info Header Size*/
#define CXM_LOGPKT_PATTERN_INFO_HEADER_SIZE() (sizeof(uint16))

/*Pattern Info Size*/
#define CXM_LOGPKT_PATTERN_INFO_SIZE(num_patterns) ( (num_patterns == 0) ? 0 : (\
            CXM_LOGPKT_PATTERN_INFO_HEADER_SIZE() + \
            (num_patterns * CXM_LOGPKT_BLANKING_PATTERN_LENGTH * sizeof(uint16))) )

/*Channel Info Size*/
#define CXM_LOGPKT_CHANNEL_INFO_SIZE(num_phychans) ( \
            ((num_phychans) * sizeof(channel_info_struct_type)) )

/*0x41B6 Log Packet Size*/
#define CXM_LOGPKT_SIZE(num_samples, total_sample_data_cnt, num_patterns, num_phychans) ( \
            CXM_LOGPKT_HEADER_SIZE() + \
            CXM_LOGPKT_BLANKING_INFO_SIZE(num_samples, total_sample_data_cnt) + \
            CXM_LOGPKT_PATTERN_INFO_SIZE(num_patterns)+ \
            CXM_LOGPKT_CHANNEL_INFO_SIZE(num_phychans) )
#endif /*FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW*/

/****************************************************************************
                Globals Declarations
****************************************************************************/

/*WL1 CXM Critical Section*/
extern rex_crit_sect_type wl1_cxm_lock;

/*CXM Rx power filtering timer*/
extern rex_timer_type wl1_cxm_pwr_timer;

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
/*WL1 MULTISIM CXM Critical Section*/
extern rex_crit_sect_type wl1_multisim_cxm_lock;
#endif /*FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW*/

/*===========================================================================

                FUNCTION DECLARATIONS FOR MODULE

=========================================================================*/
/*===========================================================================
FUNCTION     WL1_CXM_LINK_INFO_MGR

DESCRIPTION
  This API is the central entity for reporting/maintaining the
  frequency params of all the relevant active WL1 radio links
  to MCS using the CXM interface

  Additionally this API is responsibile for the following:
  **Maintain a database, which contains the link info(freq/freqId info)
    of all the currently registered WL1 link entities
  **Handle WL1_CXM_FREQ_MSG
    i.e requests from various link entities for ADDing/REMOVing/UPDATing
    their link info i.e frequency params and registering the updated
    WL1 link list database info with MCS
  **Handle WL1_CXM_FREQID_MSG
    i.e receive the freqId's for all the registered links from MCS and
    update the database accordingly
  **Execute FreqId callback's for the registered modules/link entities

DEPENDENCIES
  Link entities which wish to register their frequency with MCS
  need to pass valid/relevant  freq params to wl1_cxm_link_info_mgr
  using the input parameter "wl1_cxm_link_mgr_msg_struct_type* msg_ptr"

  Link Mgr would be in WL1_CXM_WAIT_STATE while wating for the freqId
  report for the registered link info from MCS, during which it'll place all
  new link requests in a pending db and process them once the freqId
  is received and processed
  In all other cases Link Mgr would be in WL1_CXM_IDLE_STATE during
  which it can process all new link_requests

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_cxm_link_info_mgr(wl1_cxm_link_mgr_msg_struct_type* msg_ptr);

/*===========================================================================
FUNCTION     WL1_CXM_FREQID_MSG_HANDLER

DESCRIPTION
  This API handles the freq ID messages received from MCS
  via MSGR interface

DEPENDENCIES
  Gets invoked from wcdma_l1_task when L1_MSGR_SIG is
  set by MSGR indicating (freq id) is being sent by MCS

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_cxm_freqid_msg_handler(wl1_msgr_cmd_type* cmd_ptr);

/*===========================================================================
FUNCTION     WL1_CXM_DCH_CLEANUP

DESCRIPTION
  This API de-registers the DC freq info with MCS
  as part of the l1m_dch_cleanup
  **DC Module cleans up its freqInfo for DC->SC Reconfig only
  **for all other scenarios where we exit DCH state without
  **DC -> SC Reconfig the DC freq de-registration is handled here

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_cxm_dch_cleanup(void);

/*===========================================================================
FUNCTION     WL1_CXM_GET_CURR_LINK_COUNT

DESCRIPTION
  This API when invoked returns a count of the current number of
  radio links which have been registered by WL1 with MCS

DEPENDENCIES
  NONE

RETURN VALUE
  uint8: Count of current number of registered links

SIDE EFFECTS
  NONE
===========================================================================*/
extern uint8 wl1_cxm_get_curr_link_count(void);

/*===========================================================================
FUNCTION     WL1_CXM_GET_CURR_LINK_LIST_FREQID_INFO

DESCRIPTION
  This API when invoked with input freqid_info(by reference)
  populates it with the link_entity/freqId info of all the current
  radio links which have been registered by WL1 with MCS

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_cxm_get_curr_link_list_freqid_info(wl1_cxm_link_list_freqid_info_struct_type* freqid_info);

/*===========================================================================
FUNCTION     WL1_CXM_IS_LINK_FREQ_INFO_NEEDED

DESCRIPTION
  This API gets called by various link entities, namely:
  Primary RX/TX, Diversity RX, Dual Carrier RX and W2W CM Meas
  in order to check if the State/Frequency info of the resp. links
  needs to be reported to MCS or not

  The decision about whether a particular link info needs to be reported
  or not is made based off the definitions of the resp feature flags and
  based of the device mode preference i.e SSSS; DSDS; DSDA or TSTS
  In event of more than one feature flag being defined/enabled a
  "proper superset" of the requirements gets executed

  State/Frequency reporting requirements for the various Features are:
  --------------------------------------------------------------------
  **#ifdef FEATURE_WCDMA_DIME_TX_STATE_REPORTING**
    Only PRI TX LINK info needs to be reported in SSSS mode

  **#ifdef FEATURE_MODEM_DDR_MGMT
    Primary RX/TX and Diversity RX freq info need to be
    reported across all modes i.e SSSS; DSDS; DSDA; TSTS

  **#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW (DSDA CXM)
    Primary RX/TX, Diversity RX, DC RX and W2W CM freq info
    need to be reported only in DSDA mode

DEPENDENCIES
  NONE

RETURN VALUE
  boolean ret_val
  TRUE => Link Freq Info needs to be registered with MCS
  FALSE => Link Freq Info need not be registered with MCS

SIDE EFFECTS
  NONE
===========================================================================*/
extern boolean wl1_cxm_is_link_freq_info_needed(wl1_cxm_link_entity_enum_type link_type);

/*===========================================================================
FUNCTION     WL1_CXM_PWR_INIT

DESCRIPTION
  This API initializes the periodic(every 20msecs) power reporting
  to MCS while entering FACH || DCH states if the following conditions
  are met:
  **If defined FEATURE_MODEM_DDR_MGMT || \
  **If in DSDA mode

DEPENDENCIES
  Gets invoked from l1m_fach_init || l1m_dch_init
  while entering those resp. states

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_cxm_pwr_init(void);

/*===========================================================================
FUNCTION     WL1_CXM_PWR_CLEANUP

DESCRIPTION
  This API de-initializes the periodic(every 20msecs) power reporting
  to MCS while exiting FACH || DCH states if the following conditions
  are met:
  **If defined FEATURE_MODEM_DDR_MGMT || \
  **If in DSDA mode

DEPENDENCIES
  Gets invoked from l1m_fach_cleanup || l1m_dch_cleanup
  while exiting those resp. states

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_cxm_pwr_cleanup(void);

/*===========================================================================
FUNCTION     WL1_CXM_REPORT_PWR_INFO

DESCRIPTION
  This API gets invoked each time the wl1_cxm_rx_pwr_timer == 20(msecs)
   expires upon which the RX/TX pwr(in 10*dBm units) are reported to CXM
  The timer is set only once in (fach_init || dch_init) and recursively
  set under wl1_cxm_report_pwr_info( ) each time after reporting the pwr
  to CXM as long as we are in (FACH || DCH)
  The timer is cleared upon exit from (FACH || DCH) states
  under their resp. cleanups
  For runtime DSDA->SSSS transition in either (FACH || DCH) the timer is
  cleared while handling the WL1_CXM_DISABLE_CXM_EVT
  Handles DDR and DSDA pwr reporting dynamically based off "PWR MODE"
  **WL1_CXM_DDR_PWR_MODE
      Report Rx and RxD powers periodically
  **WL1_CXM_MULTISIM_CXM_PWR_MODE
      Report Rx, Tx and DC powers periodically

DEPENDENCIES
  The power of a link will be reported only if it is associated with a
  valid freqId(i.e its link/freq info has been registered with MCS)

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_cxm_report_pwr_info(uint32 unused);

#ifdef FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
/*===========================================================================
FUNCTION     WL1_MULTISIM_CXM_INIT

DESCRIPTION
  This API takes care of all the registrations/initializations
  for the WL1_MULTISIM_CXM Operations

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_multisim_cxm_init(void);

/*===========================================================================
FUNCTION     WL1_MULTISIM_CXM_IS_G_TX_ON

DESCRIPTION
  This API checks and returns the current G Tx state

DEPENDENCIES
  wl1_cxm_state_info gets updated in wl1_cxm_msg_handler
  each time there is a change in any of the tech's Tx state

RETURN VALUE
  boolean wl1_cxm_is_g_tx_on;
  TRUE implies G Tx is ON
  FALSE implies G Tx is OFF

SIDE EFFECTS
  NONE
===========================================================================*/
extern boolean wl1_multisim_cxm_is_g_tx_on(void);

/*===========================================================================
FUNCTION     WL1_MULTISIM_CXM_LOGPKT_HANDLER

DESCRIPTION
  This API logs the 0x41B6 WCDMA CxM log packet every 10ms in FACH and DCH states
  The API gets triggered from seq_non_dch_sfn_cfn_update_event_handler every 10 msecs
  in DCH and FACH states; upon getting invoked the API it allocated the memory for the
  log pkt populates the blanking data from the WFW async buffer and commits the log
  pkt to the DIAG

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_multisim_cxm_log_pkt_handler(void);

/*===========================================================================
FUNCTION     WL1_MULTISIM_CXM_FILTER_RX_POWER

DESCRIPTION
  This API gets triggered every 10msec from seq_non_dch_sfn_cfn_update_event_handler
  in DCH and FACH states, however we only post a local command from the ISR context
  in order to not overwhelm the ISR
  Upon getting triggered the slot level Rx AGC(dB10 ) for the previous frame excluding
  CM & CPC-DRX cycles are fetched from the WFW~WSW shared Async circular buffer
  and the corresponding Rx power in dBm is computed from it and stored in a global
  to be reported later to CXM every 20msecs by wl1_cxm_report_rx_power

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_multisim_cxm_filter_rx_power(void);

/*===========================================================================
FUNCTION     WL1_MULTISIM_CXM_EVENT_HANDLER

DESCRIPTION
  This API is an event handler for the MULTISIM CXM state machine
  Handles priority table and freq Id reporting to WFW
  in various scenarios

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_multisim_cxm_event_handler(wl1_multisim_cxm_evt_enum_type evt_type);

/*===========================================================================
FUNCTION     WL1_MULTISIM_CXM_DESENSE_MSG_HANDLER

DESCRIPTION
  This API handles the De-sense State Update messages received from MCS
  via MSGR interface

DEPENDENCIES
  Gets invoked from wcdma_l1_task when L1_MSGR_SIG is
  set by MSGR indicating (De-sense state update) is being sent by MCS

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_multisim_cxm_desense_msg_handler(wl1_msgr_cmd_type* cmd_ptr);

/*===========================================================================
FUNCTION     WL1_MULTISIM_CXM_GET_MODE_PREF

DESCRIPTION
  This API returns the current mode pref config of the device

DEPENDENCIES
  NONE

PARAMETERS
  NONE

RETURN VALUE
  wl1_mode_pref_enum_type:
    SSSS: WL1_SINGLE_SIM_SINGLE_STANDBY_MODE
    DSDS: WL1_DUAL_SIM_DUAL_STANDBY_MODE
    DSDA: WL1_DUAL_SIM_DUAL_ACTIVE_MODE
===========================================================================*/
extern wl1_mode_pref_enum_type wl1_multisim_cxm_get_mode_pref(void);

/*===========================================================================
FUNCTION  WL1_MULTISIM_CXM_UPDATE_TECH_STATE

DESCRIPTION
  This API reports the current tech state to MCS

  Tech State Types:
  =================
  **W is Monitoring paging channel i.e wakeup from sleep
    TRM_DEMOD_PAGE
  **W is entering traffic call i.e Voice call
    TRM_TRAFFIC
  **W is in or entering Background Traffici.e DATA Call
    TRM_BG_TRAFFIC
  **W is going to sleep
    TRM_NUM_REASONS

DEPENDENCIES
  Tech State is only updated if device is in
  DSDA mode and DDR freq Mgmt is enabled

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_multisim_cxm_update_tech_state(trm_reason_enum_t reason);

/*===========================================================================
FUNCTION  WL1_MULTISIM_CXM_GET_CURR_PRIORITY_TBL

DESCRIPTION
  This API when invoked with the input pri_tbl_info (by reference)
  populates it with the current WL1 priority table received from MCS

DEPENDENCIES
  void wl1_multisim_cxm_get_curr_priority_tbl(cxm_activity_table_s* pri_tbl_info)
  expects "cxm_activity_table_s* pri_tbl_info" to be the base address of an
  array of structures(cxm_activity_table_s)

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_multisim_cxm_get_curr_priority_tbl(cxm_activity_table_s* pri_tbl_info);

/*===========================================================================
FUNCTION     WL1_MULTISIM_CXM_REPORT_CURR_FREQ_AS_PRI_RX_FREQID

DESCRIPTION
  This API when invoked with a valid uarfcn value identifies
  the frequency Id associated with the uarfcn and reports it
  as the Serving Cell or Pri RX frequency Id

  Motivation:
  ===========
  Currently WFW cannot identify Idle mode W2W InterF
  Measurement scenarios as a result it performs CXM conflict
  checks using the Serving cell frequency, which could lead
  to incorrect blanking decisions getting triggered on either
  RAT's(W && G)
  To help cope with this situation we register our W2W InterF
  Measurement frequencies with MCS as Neighbours ahead of time
  and apply these frequency Id's as our Serving Cell or Pri RX
  frequency Id in WFW

DEPENDENCIES
  Idle Meas Module is responsible for toggling b/w Serving and
  Neighbour frequency Id's and handling the 30 bpg wait times
  needed for the frequency Id to take effect

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_multisim_cxm_report_curr_freq_as_pri_rx_freqid(uint16 uarfcn);

/*===========================================================================
FUNCTION     WL1_MULTISIM_CXM_PHYCHAN_TIMING_HANDLER

DESCRIPTION
  This API gets called by Uplink/HS modules whenever an Uplink/HS physical
  channel gets enabled or disabled along with the timing information of the
  phychan namely "Action Time"(START_SUBFN, START_BPG info of channel)
  and "STMR Action Time"(32 bit WRC snapshot at Action Time), upon which this
  API computes/stores "Action Time to STMR Action time" timing offset to be
  used to compute "Channel Specific BPG level Blanking Info" for version-2
  of the 0X41B6 log packet

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
extern void wl1_multisim_cxm_phychan_timing_update(uint32 stmrActionTime,
                                         boolean chan_active,
                                         wl1_multisim_cxm_phychan_enum_type chan_type,
                                         int16 chan_subfn,
                                         int8 chan_bpg);

/*===========================================================================
FUNCTION     wl1_multisim_cxm_get_fw_in_dsdx_mode

DESCRIPTION
  The function returns TRUE if FW is in dsdx mode

DEPENDENCIES
  NONE

RETURN VALUE
  wl1_cxm_link_list_info_struct_type*

SIDE EFFECTS
  NONE
===========================================================================*/
extern boolean wl1_multisim_cxm_get_fw_in_dsdx_mode(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION     wl1_multisim_cxm_freq_db_ptr

DESCRIPTION
  The function returns a pointer to wl1_cxm_freq_db

DEPENDENCIES
  NONE

RETURN VALUE
  wl1_cxm_link_list_info_struct_type*

SIDE EFFECTS
  NONE
===========================================================================*/
extern wl1_cxm_link_list_info_struct_type * wl1_multisim_cxm_freq_db_ptr(void);
#endif /* FEATURE_QSH_DUMP */

#endif /* FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW */

#endif /* FEATURE_WCDMA_COEXISTENCE_SW */

#endif /* WL1CXM_H */
