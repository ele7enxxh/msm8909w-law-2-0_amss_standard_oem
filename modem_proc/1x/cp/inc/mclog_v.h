#ifndef MCLOG_V_H
#define MCLOG_V_H

/*===========================================================================

                  Main Control Event and Logging Functions

GENERAL DESCRIPTION
  This module contains utilities and functions related to logging of events
  and DIAG/DM packets inside MC

  Copyright (c) 2000 - 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mclog.h_v   1.10   02 Oct 2002 22:52:32   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mclog_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/15   agh     Added Diag cmd to modify Ovhds Refresh time for testing
07/21/14   gga     FR 20298: SIM Swap without RESET if carrier settings have 
                   changed
06/09/14   agh     FR 17879: Added support for extended EARFCN
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
08/13/13   agh     Changed size of str in mc_diag_df_items_query_rsp_type to
                   handle buffer overflow in mccap_df_create_discovery_string
02/20/13   ppr     Added 1xCSFB: 1x_to_lte_reselection start event
01/18/13   ppr     Changes for 1xCSFB:Event for UHDM/ECAM/Redirection call types
11/14/12   ppr     Added Page match event
07/02/12   jj      Added event upon EHOCM is sent on UL.
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
05/09/12   ppr     Feature Cleanup
03/13/11   jj      Added new CSFB events to indicate Time Trasfer ,
                   Handover comletion and Receipt of GCSNA UL/DL messages.
02/29/12   ssh     Added events for 1X to LTE reselection.
02/04/12   ppr     Feature Cleanup
01/23/12   srk     Feature Cleanup
05/02/11   ssh     Added Diag request support for getting SPM, ESPM, ITSPM.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/10/11   ag      Increased size of Markov statistics variables for eMSO to
                   32 bits from 16 bits to support long duration testing.
06/15/10   ag      Added support for RDA testing.
05/26/10   ag      Replaced PACKED with PACK() to support QDSP6.
02/25/10   ag      Support to display SO74 frame statistics
08/02/09   jtm     Lint fixes.
06/30/09   ag      Diag decoupling changes for CMI
06/23/09   ag      Support to display network parameters on UI
05/14/09   jj      Split the file as part of CMI phase-II
02/15/07   sb      Removed featurization from VOIP states.
02/14/07   sb      Added VOIP logging structure.
11/21/06   sb      Added support for VOIP-1X handoffs.
07/10/06   fc      Added log packet definition for UMTS.
06/13/06   fc      Added support for status service order.
06/02/06   fc      Added IHO/AHO/APHO event definitions.
10/15/05   fc      Cleanup NCC featurization.
07/05/05   sb      Added MEID support.
06/17/05   fc      Fixed the issue of MER counters being incremented
                   incorrectly if multiple GPM received in the assigned slot.
                   Added the suport for MER benchmark in non-slotted mode.
03/23/05   fc      Merged the following from MSMSHARED:
03/25/05   an       Changed the DIAG SUBSYS CMD values for DF
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/25/04   fc      Merged the following from MSMSHARED:
02/17/05   sb       Added event to indicate a registration was suppressed.
09/27/04   va      Merged from REL B/C archive:
07/20/04   bkm      Update MER support for 6100.
07/08/04   bkm      Re-organize support for MER counters.
08/23/04   az      Rel D Negative SCI - changes for dynamic feature
08/17/04   az      Diag command MC_DIAG_SUBSYS_SET_CUR_PREF_SCI for setting
                   current preferred sci
06/24/04   sb      Release C addendum changes for PLCM.
06/16/04   fc      Added DF support for Sync ID and Reconnect message.
03/15/04   yll     Added one more byte payload in WAKEUP_SLOT event.
12/16/03   fc      Added support for EVENT_SEG_UPM_ADDR_MISMATCH.
12/02/03   bkm     Updated CH_IND/EXT_CH_IND event reporting.
11/06/03   bkm     Added diag command to retrieve Paging MER counters.
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
10/03/03   bkm     Add match type to EVENT_CP_MATCHED_MSG.
08/01/03   yll     Added support for Dynamic Feature.
12/02/02   yll     Added event logging for wakeup slot, matched msg, call
                   release, pref system reselection, etc.
11/07/02   az      Mainlined FEATURE_IS95B
10/02/02   yll     Linted the code.
10/02/02   az      Mainlined FEATURE_IS2000
03/27/02   sb      Added bcch_acquired event.
03/27/02   jrp     Moved mclog_ms_handoff_type outside FEATURE_IS2000_REL_A ifdef
03/26/02   va      Added mclog_ms_handoff_type for HO event reporting.
03/14/02   va      Added mclog_event_cc_instantiated_type and mclog_event_cc
                   _conref_change_type
02/26/02   fc      Added support for common channel monitored event.
02/13/02   va      Support for CC related events
06/26/01   sb      Merged the following from common archive:
           06/06/01   lcc     Added BS P_REV, and F/R RCs for FCH, DCCH,
                      and SCH to the IS2000 status packet.
04/27/01   fc      Changed function prototype of
                   mclog_event_report_pch_acquired.
04/18/01   fc      Featurized mclog_report_event_ch_ind under FEATURE_IS2000.
04/12/01   fc      Added/Moved all events logging data types / function
                   prototypes from mc.h and mccdma.h to here.
04/10/01   jq      Created the file to handle logging and diagpkt needs.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "log.h"
#include "diagpkt.h"
#include "mccapdf_v.h"
#include "mccdma.h"
#include "mclog.h"
#include "cdma2kdiag.h"
#include "modem_1x_defs.h"
#include "mccdma_i.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

typedef PACK (struct)
{
  boolean enable;
  word code_chan;         /* code for current channel - cdma only */
  byte qof_mask_id;

} chan_info_type;

typedef PACK (struct)
{
  word    pilot_pn;     /* Pilot PNs currently in the active set */
  byte pilot_rec_type;
  chan_info_type fch;
  chan_info_type dcch;
  chan_info_type sch;

} mclog_active_set_type;

typedef PACK (struct)
{
  boolean active;
  byte rate;
  byte walsh_id;

} mclog_aset_rsch_type;

LOG_RECORD_DEFINE(LOG_ASET_INFO_C)
  byte    rf_mode;                /* 0->analog,  1->cdma         */
  byte    band_class;             /* 0->Cellular, 1->PCS etc              */
  word    curr_chan;              /* center frequency channel             */
  byte    num_pilots;             /* number of pilots */

  mclog_active_set_type aset[CAI_N6M];
  byte    fsch_rate;      // the rate is the same for all FSCHs, only valid when at least one F-SCH is active
  mclog_aset_rsch_type rsch; // we only support one supplemental CH

LOG_RECORD_END

typedef LOG_ASET_INFO_C_type       log_aset_info_type;

typedef PACK (struct)
{
  boolean enable;
  byte gating_rate; /* only applicable when control hold is enabled */

} mclog_control_hold_type;

DIAGPKT_REQ_DEFINE(DIAG_IS2000_STATUS_F)
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE( DIAG_IS2000_STATUS_F )
  byte    rf_mode;                /* 0->analog,  1->cdma         */
  byte    band_class;             /* 0->Cellular, 1->PCS etc              */

  word    cdma_rx_state;          /* current state for cdma only          */
  int1    cdma_good_frames;       /* whether or not frames we are
                                     receiving are good -- cdma only      */
  word    analog_corrected_frames;/* good frame count - analog only       */
  word    analog_bad_frames;      /* bad frame count - analog only        */
  word    analog_word_syncs;      /*  -- analog only                      */

  word    entry_reason;           /* entry reason                         */
  word    curr_chan;              /* center frequency channel             */

  word    sid;                    /* Current System ID                    */
  word    nid;                    /* Current Network ID                   */
  word    locaid;                 /* Current Location ID                  */
  word    rssi;                   /* Current RSSI level                   */
  byte    power;                  /* Current mobile output power level    */
  byte    num_pilots;             /* number of pilots */

  mclog_active_set_type aset[CAI_N6M];

  mclog_control_hold_type control_hold;
  byte    fsch_rate;      // the rate is the same for all FSCHs
  mclog_aset_rsch_type rsch; // we only support one supplemental CH
  byte    bs_p_rev;
  PACK (struct)
  {
    byte  for_rc;
    byte  rev_rc;
  } fch, dcch, sch;
DIAGPKT_RSP_END

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumerations for event report                                           */

/* ------------------------------
** ENUM for 1X CSFB Call types
** ------------------------------ */
typedef enum {
  CSFB_CALL_UNKNOWN,    /* 1X CP Internal error type */
  CSFB_CALL_UHDM,
  CSFB_CALL_ECAM,
  CSFB_CALL_REDIRECTION
}csfb_call_info_type;

/* 1x_csfb_call_type event */
typedef PACK(struct)
{
  csfb_call_info_type csfb_call_info; /* CSFB call type: UDHM / ECAM / REDIRECTION */
}event_csfb_call_type;

/* Operation modes */
typedef enum
{
  MCLOG_OP_MODE_POWERUP, /* Power up mode */
  MCLOG_OP_MODE_AMPS,    /* Analog operation mode */
  MCLOG_OP_MODE_CDMA     /* CDMA operation mode */

} mclog_event_op_mode_type;

/* Page match event type */
typedef enum {
  SO_VOICE,
  SO_SMS,
  SO_DATA,
  SO_SIGNALING
}so_call_type;

typedef PACK(struct)
{
  word service_option;     /* Service Option (SO) included (Unsinged 16 bit value type) */
  so_call_type call_type;  /* Voice or SMS or DATA call type */
}event_page_match_type;

#ifdef FEATURE_1X_TO_LTE
/* ENUM to define the type of the status of the reselection from 1x to lte. */
typedef enum
{
  ONEX_TO_LTE_RESEL_ABORTED,
  ONEX_TO_LTE_RESEL_FAILED,
  ONEX_TO_LTE_RESEL_SUCCESS
} mclog_event_1x_to_lte_resel_status_type;

/* 1x_to_lte reselection start type event */
typedef PACK(struct)
{
  lte_earfcn_t earfcn_id; /* Frequency of the LTE cell to be selected */
  uint16 cell_id; /* Cell id of the LTE cell to be selected */
}mclog_event_1x_to_lte_resel_start_type;

#endif /* FEATURE_1X_TO_LTE */

/* Call processing states */
typedef enum
{
  CDMA_SYS_DET_S,    /* System determination state */
  CDMA_PIL_ACQ_S,    /* Pilot channel acquisition state */
  CDMA_SYNC_ACQ_S,   /* Sync channel acquisition state */
  CDMA_TIME_CHG_S,   /* Timing change state */
  CDMA_IDLE_S,       /* Idle state */
  CDMA_UPDATE_OVH_S, /* Update Overhead Information */
  CDMA_ORIG_S,       /* Mobile Station Origination Attempt */
  CDMA_PAGE_RES_S,   /* Page Response */
  CDMA_ORD_RES_S,    /* Mobile Station Order/Message Response */
  CDMA_REG_S,        /* Registration Access */
  CDMA_MSG_S,        /* Message transmission */
  CDMA_PACA_CAN_S,   /* PACA Cancel Substate */
  CDMA_TCI_S,        /* Traffic Channel Initialization */

  #ifdef FEATURE_IS2000_REL_A
  CDMA_TRAFFIC_S,        /* Traffic Channel State */
  #else
  CDMA_WFO_S,        /* Waiting For Order */
  CDMA_WFA_S,        /* Waiting for Mobile Station Answer */
  CDMA_CONV_S,       /* Conversation */
  #endif

  CDMA_RELEASE_S,    /* Release */

  /* The following states are for a VOIP-1X handoff */
  CDMA_WAIT_FOR_SRDM_S,   /* Wait for a SRDM from CM */
  CDMA_GET_TIMING_S,      /* Transfer timing from DO */
  CDMA_WAIT_FOR_UHDM_S,   /* Wait for a UHDM from CM */
  CDMA_GO_TO_TCH_S,       /* Go to the Traffic Channel */

  CDMA_MAX_S

} mclog_event_cp_state_type;

#ifdef FEATURE_IS2000_REL_A
/* Call control substates -- cannot have more than 16, including 0 (event payload is 4 bits) */
#define MAX_NUM_CALL_CONTROL_SUBSTATES 16

typedef enum
{
  CDMA_CC_WFO_S,     /* Waiting for Order */
  CDMA_CC_WFA_S,     /* Waiting for Answer */
  CDMA_CC_CONV_S,    /* Conversation */
  CDMA_CC_RELEASE_S, /* Release */
  CDMA_CC_MAX_S      /* Maximum value, used for range-checking */

} mclog_event_cc_state_type;

typedef PACK (struct)
{
  unsigned int old_con_ref : 8;
  unsigned int new_con_ref : 8;

} mclog_event_cc_conref_change_type;

typedef PACK (struct)
{
  unsigned int con_ref : 8;
  unsigned int init_substate : 4;

} mclog_event_cc_instantiated_type;
#endif

typedef PACK (struct)
{
  unsigned short pn[6];
  PACK (struct)
  {
    unsigned int s1       : 1;
    unsigned int s2       : 1;
    unsigned int s3       : 1;
    unsigned int s4       : 1;
    unsigned int s5       : 1;
    unsigned int s6       : 1;
    unsigned int reserved : 10;
  } in_sch_aset;

  PACK (struct)
  {
    unsigned int sr1       : 1;
    unsigned int sr2       : 1;
    unsigned int sr3       : 1;
    unsigned int sr4       : 1;
    unsigned int sr5       : 1;
    unsigned int sr6       : 1;
    unsigned int reserved  : 10;
  } pwr_comb_ind;

} mclog_ms_handoff_type;

typedef PACK (struct)
{
  unsigned int pch_active          : 1;
  unsigned int qpch_active         : 1;
  unsigned int reserved            : 11;

} event_common_channel_monitored_type;

/* Obsolete, use mclog_event_wakeup_slot_v2_type */
typedef byte mclog_event_wakeup_slot_type;

typedef PACK (struct)
{
  byte  slot_mask;
    /* Wakeup slot mask:
     *   Page Slot:           1
     *   Broadcast Page Slot: 2
     *   Broadcast Data Slot: 4
     */
  boolean slept;
    /* Indiate if Search has slept before wakeup. */

} mclog_event_wakeup_slot_v2_type;

/* This enum is defined specifically for reporting the match
** type in EVENT_CP_MATCHED_MSG.  Note that the MS, BCAST, ANN
** values allow for a bit-mask-like combinations if needed
** in the future.
*/
typedef enum
{
  MCLOG_MSG_MATCH             = 0,      /* a catch all case */
  MCLOG_PAGE_MS_DIR_MATCH     = 1,      /* PAGE_MATCH */
  MCLOG_PAGE_BCAST_MATCH      = 2,      /* BROADCAST_MATCH */
  MCLOG_PAGE_ANN_MATCH        = 4       /* ANNOUCEMENT_MATCH */

} mclog_match_type;

typedef PACK (struct)
{
  byte msg_type;       /* csch msg id */
  byte match_type;     /* contains mclog_match_type as byte */

} mclog_event_cp_matched_msg_type;

typedef byte mclog_event_release_type;

typedef enum
{
  MCLOG_RELEASE_MS_ORDER,          /* MS Release Order */
  MCLOG_RELEASE_BS_ORDER,          /* BS Release Order */
  MCLOG_RELEASE_ERM_ALL_CH,        /* ERM w/ CH_ID = all channels */

  MCLOG_RELEASE_MAX

} mclog_release_reason_type;

typedef PACK (struct)
{
  unsigned short pn[CAI_N6M];
  PACK (struct)
  {
    unsigned int sr2       : 1;
    unsigned int sr3       : 1;
    unsigned int sr4       : 1;
    unsigned int sr5       : 1;
    unsigned int sr6       : 1;
    unsigned int reserved  : 11;
  } sr;

} mclog_fch_dcch_acq_type;

typedef byte mclog_event_registration_suppressed_type;

typedef PACK (struct)
{
  byte using_private_lcm;           /* True if mobile is using private long code mask */
  byte public_long_code_mask_type;  /* Type of public long code mask */

} mclog_event_long_code_mask_type;

/* ----------------------------------
** Event Report Packet for Idle, Reacq,
** Fast Reacq, and Access handoffs.
** ---------------------------------- */
typedef PACK (struct)
{
  uint16 new_pilot;
  int8 ecio_old_pilot;
  uint8 ecio_old_pilot_frac;
  int8 ecio_new_pilot;
  uint8 ecio_new_pilot_frac;
  uint16 new_freq;
  uint8 new_band_class;
  uint8 idle_ho_type;  /* mcc_idle_ho_enum_type cast as uint8 */

} mclog_event_idl_acc_handoff_type;

typedef enum
{
  MCLOG_IDLE_HO = 0,
  MCLOG_IDLE_HO_REACQ,
  MCLOG_IDLE_HO_FAST_REACQ_CHEAP,
  MCLOG_IDLE_HO_FAST_REACQ_EXPENSIVE

} mclog_idle_ho_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Diag Subsystem Dispatch Command Code CALLP1X. see diagcmd.h */
#define MC_DIAG_SUBSYS_STATE_QUERY       2
#define MC_DIAG_SUBSYS_PAGING_MER_STAT   3
#define MC_DIAG_SUBSYS_SET_CUR_PREF_SCI  6
#define MC_DIAG_SUBSYS_DF_ITEMS_QUERY    7
#define MC_DIAG_SUBSYS_DF_ITEM_VERIFY    8
#define MC_DIAG_SUBSYS_DF_ITEMS_WRITE    9
#define MC_DIAG_SUBSYS_GET_NW_PARAMS    10
#ifdef FEATURE_IS2000_1X_ADV
#define MC_DIAG_SUBSYS_ENH_MARKOV_STAT  11
#endif
#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
#define MC_DIAG_SUBSYS_MAR_RDA_TEST_STAT 12
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */
#ifdef FEATURE_1XCP_FTD
#define MC_DIAG_SYS_PARAMS_QUERY        13
#endif /* FEATURE_1XCP_FTD */
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#define MC_DIAG_SUBSYS_RELOAD_NV_ITEMS  14
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#define MC_DIAG_SUBSYS_SET_OVHD_REFRESH_TIME 15

/*************** MC_DIAG_SUBSYS_DF_ITEMS_QUERY ***************/

/* Query the DF items. CP returns a string indicating the supported
 * DF items, with their corresponding values and other information */
typedef PACK (struct)
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEMS_QUERY */
  diagpkt_subsys_header_type    hdr;

} mc_diag_df_items_query_req_type;

typedef PACK (struct)
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEMS_QUERY */
  diagpkt_subsys_header_type    hdr;
  /* Status indicates whether Dynamic Feature is supported */
  byte                          status;
  /* DF String buffer. This string buffer stores MCCAP_DF_MAX no. of strings
   * with maximum size MAX_DF_STR_LEN along with some additional data. The
   * size of the buffer can be arrived from the way this string is built in
   * the function mccap_df_create_discovery_string*/
  char                          str[((uint32)MCCAP_DF_MAX * (MAX_DF_STR_LEN + 6))+2];
} mc_diag_df_items_query_rsp_type;

/*************** MC_DIAG_SUBSYS_DF_ITEM_VERIFY ***************/

/* Verify a change made by the user */
typedef PACK (struct)
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEM_VERIFY */
  diagpkt_subsys_header_type      hdr;
  /* First byte is the item no. Second byte is the item value and the third
   * byte is the esn hash */
  char                            str[3];

} mc_diag_df_item_verify_req_type;

typedef PACK (struct)
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEM_VERIFY */
  diagpkt_subsys_header_type      hdr;
  /* Status is TRUE if the esn hash in the request string matches with the phones
   * esn hash */
  byte                            status;

  char                            str[(uint32) MCCAP_DF_MAX * (MAX_DF_STR_LEN + 6)];
} mc_diag_df_item_verify_rsp_type;

/*************** MC_DIAG_SUBSYS_DF_ITEMS_WRITE ***************/

/* Write the DF items back to the NV */
typedef PACK (struct)
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEMS_WRITE */
  diagpkt_subsys_header_type      hdr;
  char                            str[(uint32) MCCAP_DF_MAX * (MAX_DF_STR_LEN + 6)];

} mc_diag_df_items_write_req_type;

typedef PACK (struct)
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEMS_WRITE */
  diagpkt_subsys_header_type      hdr;
  /* Status indicates whether DF NV write is successful */
  byte                            status;

} mc_diag_df_items_write_rsp_type;

/*************** MC_DIAG_SUBSYS_STATE_QUERY ***************/

/* Query the MC state.   */
#define MCLOG_MAX_NUM_CONNECTIONS CAI_MAX_NUM_CONNECTIONS

typedef PACK (struct)
{
  diagpkt_subsys_header_type      hdr;

} mclog_diag_state_query_req_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type      hdr;
  uint8                           mc_state; /* same as returned by phone state - 63 */
  uint16                          event_count; /* same as returned by phone state - 63 */
   /* All information below is relevant only if mc_state is DIAG_D_CONV_S (0x86) */
  uint8                           num_cc;   /* Num Call Control Instances */
  PACK (struct)
  {
    uint8    call_id;
    uint8    con_ref;
    uint16   so;
    uint8    sr_id;
    uint8    substate;
   } cc[MCLOG_MAX_NUM_CONNECTIONS];

} mclog_diag_state_query_rsp_type;

/*************** MC_DIAG_SUBSYS_SET_CUR_PREF_SCI ***************/

/* Set the current preferred slot cycle index. */

typedef PACK (struct)
{
  int8 curr_pref_sci;

} mc_diag_sci_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type    hdr;
    /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_SET_CUR_PREF_SCI */
  mc_diag_sci_type              sci;

} mc_diag_cur_pref_sci_req_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type      hdr;
    /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_VERIFY */
  mc_diag_sci_type                sci;

} mc_diag_cur_pref_sci_rsp_type;

/*************** MC_DIAG_SUBSYS_SET_OVHD_REFRESH_TIME ***************/

/* Set the current preferred slot cycle index. */

typedef PACK (struct)
{
  /* overhead refresh time in seconds */
  uint16 ovhd_refresh_time;

} mc_diag_refresh_time_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type    hdr;
    /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_SET_OVHD_REFRESH_TIME */
  mc_diag_refresh_time_type     refresh_time;

} mc_diag_ovhd_refresh_time_req_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type      hdr;
    /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_SET_OVHD_REFRESH_TIME */
  mc_diag_refresh_time_type     refresh_time;

} mc_diag_ovhd_refresh_time_rsp_type;


/*************** MC_DIAG_SUBSYS_GET_NW_PARAMS ***************/

/* Get the network parameters. */

typedef enum
{
  INTER_FREQ_IDLE_HO_COUNT,
  /* Interfrequency idle handoff count since last powerup */

  HARD_HO_COUNT
  /* Traffic channel hard handoff count since last powerup */
} mc_diag_nw_param_type;

typedef PACK (struct)
{
  uint32 inter_freq_idle_ho_count;
  /* Interfrequency idle handoff count since last powerup */

  uint32 hard_ho_count;
  /* Traffic channel hard handoff count since last powerup */

  uint16 fwd_sch0_mux_option;
  /* Current Multiplex Option for F-SCH0 */

  uint8 fwd_sch0_radio_config;
  /* Current Radio Configuration for F-SCH0 */

  uint16 rev_sch0_mux_option;
  /* Current Multiplex Option for R-SCH0 */

  uint8 rev_sch0_radio_config;
  /* Current Radio Configuration for R-SCH0 */

} mc_diag_nw_params_values_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type    hdr;
    /* cmd_code, subsys_id, subsys_cmd_code:MC_DIAG_SUBSYS_GET_NW_PARAMS */

} mc_diag_get_nw_params_req_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type      hdr;
    /* cmd_code, subsys_id, subsys_cmd_code, paramater values:
       MC_DIAG_SUBSYS_GET_NW_PARAMS */
  mc_diag_nw_params_values_type     params;

} mc_diag_get_nw_params_rsp_type;

#ifdef FEATURE_1XCP_FTD
typedef PACK (struct)
{
  diagpkt_subsys_header_type    hdr;
    /* cmd_code, subsys_id, subsys_cmd_code:MC_DIAG_SUBSYS_GET_NW_PARAMS */

} mc_diag_get_system_params_req_type;

typedef PACK (struct)
{
  word pilot_pn;              /* Pilot PN sequence offset index */
  byte config_msg_seq;        /* Configuration message sequence number */
  word sid;                   /* System identification */
  word nid;                   /* Network identification */
  word reg_zone;              /* Registration zone */
  byte total_zones;           /* Number of registration zones
                                 to be retained */
  byte zone_timer;            /* Zone timer length */
  boolean mult_sids;          /* Multiple SID storage indicator */
  boolean mult_nids;          /* Multiple NID storage indicator */
  word base_id;               /* Base station identification */
  byte base_class;            /* Base station class */
  byte page_chan;             /* Number of Paging Channels */
  byte max_slot_cycle_index;  /* Maximum slot cycle index */
  boolean home_reg;           /* Home registration indicator */
  boolean for_sid_reg;        /* SID roamer registration indicator */
  boolean for_nid_reg;        /* NID roamer registration indicator */
  boolean power_up_reg;       /* Power-up registration indicator */
  boolean power_down_reg;     /* Power-down registration indicator */
  boolean parameter_reg;      /* Parameter-change registration indicator */
  byte reg_prd;               /* Registration period */
  dword base_lat;             /* Base station latitude */
  dword base_long;            /* Base station longitude */
  word reg_dist;              /* Registration distance */
  byte srch_win_a;            /* Search window size for the Active Set and
                                 Candidate Set */
  byte srch_win_n;            /* Search window size for the Neighbor Set */
  byte srch_win_r;            /* Search window size for the Remaining Set */
  byte nghbr_max_age;         /* Neighbor Set maximum AGE */
  byte pwr_rep_thresh;        /* Power control reporting threshold */
  byte pwr_rep_frames;        /* Power control reporting frame count */
  boolean pwr_thresh_enable;  /* Threshold report mode indicator */
  boolean pwr_period_enable;  /* Threshold report mode indicator */
  byte pwr_rep_delay;         /* Power report delay */
  boolean rescan;             /* Rescan indicator */
  byte t_add;                 /* Pilot detection threshold */
  byte t_drop;                /* Pilot drop threshold */
  byte t_comp;                /* Active Set versus Candidate Set
                                 comparison threshold */
  byte t_tdrop;               /* Drop timer value */
  boolean ext_sys_parameter;  /* Extended System Parameters Msg indicator */
  boolean ext_nghbr_list;     /* Extended Neighbor List Message indicator */
  boolean gen_nghbr_list;     /* General Neighbor List Message indicator */
  boolean global_redirect;    /* Global Service Redirection Msg indicator */
  boolean is2000_incl;        /* is2000 fields included indicator */
  boolean pri_nghbr_list;     /* Private Neighbor List Message indicator */
  boolean user_zone_id;       /* User Zone Identification Message indicator */
  boolean ext_global_redirect;/* Extended Global Service Redirection
                                  Message indicator */
  boolean ext_chan_list;      /* Extended CDMA Channel List Message
                                  indicator */
  boolean is2000_rel_c_incl;  /* IS2000 Release C fields included indicator */
  boolean t_tdrop_range_incl; /* Drop timer range value included indicator */
  byte t_tdrop_range;         /* Drop timer range value */
} mc_diag_spm;

typedef PACK (struct)
{
  word pilot_pn;             /* Pilot PN sequence offset index */
  byte config_msg_seq;       /* Configuration message sequence number */
  boolean delete_for_tmsi;   /* Delete foreign TMSI */
  byte pref_msid_type;       /* Preferred Access Channel MSID type */
  word mcc;                  /* Mobile country code */
  byte imsi_11_12;           /* 11th and 12th digits of the IMSI */
  byte tmsi_zone_len;        /* TMSI zone length */
  qword tmsi_zone;           /* TMSI zone */
  byte bcast_index;          /* Broadcast slot cycle index */
  boolean is95b_incl;             /* is95b fields included indicator */
  boolean imsi_t_supported;       /* IMSI_T_SUPPORTED flag */
  byte p_rev;                     /* P_REV supported by BS */
  byte min_p_rev;                 /* Minimum P_REV required for access */
  byte soft_slope;                /* Soft slop for soft handoff */
  byte add_intercept;             /* For IS-95B soft handoff */
  byte drop_intercept;            /* For IS-95B soft handoff */
  byte packet_zone_id;            /* Packet Data service zone identifier */
  byte max_num_alt_so;            /* Max. Num. of alternative service options */
  boolean reselect_included;      /* System Reselection parameters included */
  byte ec_thresh;                 /* Pilot Power Threshold */
  byte ec_io_thresh;              /* Pilot Ec/Io Threshold */
  boolean pilot_report;           /* Pilot reporting indicator */
  boolean nghbr_set_entry_info;   /* Access entry handoff info included */
  boolean acc_ent_ho_order;       /* Entry handoff permitted indicator */
  boolean nghbr_set_access_info;  /* Neighbor set access handoff included */
  boolean access_ho;              /* Access handoff permitted indicator */
  boolean access_ho_msg_rsp;      /* AHO permitted for msg response */
  boolean access_probe_ho;        /* APHO permitted indicator */
  boolean acc_ho_list_upd;        /* APHO List Update permitted indicator */
  boolean acc_probe_ho_other_msg; /* APHO for msg other than ORIG/PR */
  byte max_num_probe_ho;          /* Times limit for access probe handoff */
  byte nghbr_set_size;            /* Size of neighbor set */
  boolean access_entry_ho[MAX_NGHBR_SET_SIZE];   /* Entry handoff permitted */
  boolean access_ho_allowed[MAX_NGHBR_SET_SIZE]; /* AHO and APHO permitted */
  boolean broadcast_gps_asst;     /* Broadcast GPS Assist indicator */
  boolean qpch_supported;         /* Quick Paging Channel Supported indicator */
  byte num_qpch;                  /* Number of Quick Paging Channels */
  boolean qpch_rate;              /* Quick Paging Channel Indicator rate */
  byte qpch_power_level_page;     /* Quick Paging Channel paging indicator
                                     transmit power level */
  boolean qpch_cci_supported;     /* Quick Paging Channel configuration change
                                     indicator supported */
  byte qpch_power_level_config;   /* Quick Paging Channel configuration change
                                      indicator transmit power level */
  boolean sdb_supported;          /* Short Data Burst supported indicator */
  byte rlgain_traffic_pilot;      /* Gain Adjustment of the Reverse Traffic
                                     Channel relative to the Reverse Pilot
                                     Channel for Radio Configurations greater
                                     than 2 */
  boolean rev_pwr_cntl_delay_incl;
  byte rev_pwr_cntl_delay;

  /* Rel A fields. */
  boolean is_is2000_rel_a_incl;     /* IS2000 Release A fields included indicator */
  boolean auto_msg_supported;       /* autonomous message supported indicator */
  byte auto_msg_interval;           /* autonomous message interval */
  boolean mob_qos;                  /* permission indicator for QoS in Origination message */
  boolean enc_supported;            /* encryption fields included */
  byte sig_encrypt_sup;             /* signaling encryption supported indicator */
  byte ui_encrypt_sup;              /* user information encryption supported indicator */
  boolean use_sync_id;              /* use sync_id indicator */
  boolean cs_supported;             /* concurrent service supported */
  boolean bcch_supported;           /* BCCH supported indicator */
  boolean ms_init_pos_loc_sup_ind;  /* MS initiated position location supported */
  boolean pilot_info_req_supported; /* pilot information request supported indicator */


  /*REL B fields. */
  boolean is_is2000_rel_b_incl;     /* IS2000 Release B fields included indicator */
  boolean qpch_bi_supported;        /* QPCH broadcast indicator supported */
  boolean qpch_power_level_bcast;   /* QPCH broadcast indicator transmit power level */
  byte band_class_info_req;         /* Band class information request indicator */
  byte alt_band_class;              /* Alternate CDMA band class supported by the base station */
  boolean cdma_off_time_rep_sup_ind;/* CDMA off time rport supported indicator */
  boolean cdma_off_time_rep_threshold_unit; /* CDMA off time report unit */
  byte cdma_off_time_rep_threshold; /* CDMA off time report threshold */

  /* REL C feilds. */
  boolean is_is2000_rel_c_incl;     /* IS2000 Release C fields included indicator */
  boolean chm_supported;            /* Control Mode Mode supported indicator */
  boolean release_to_idle_ind;      /* Release to idle allowed indicator */
  boolean reconnect_msg_ind;        /* Reconnect Message supported indicator */
  boolean msg_integrity_sup;        /* Message integrity supported indicator */
  boolean sig_integrity_sup_incl;   /* Signaling message integrity information
                                       included indicator */
  byte sig_integrity_sup;           /* Signaling integrity algorithm supported
                                       by the base station */
  boolean for_pdch_supported;       /* Forward Packet Data Channel supported
                                       indicator */
  boolean pdch_chm_supported;       /* PDCH Control Hold Mode supported
                                       indicator */
  boolean pdch_parms_incl;          /* Forward Packet Data Channel related
                                       parameters included indicator */
  boolean for_pdch_rlgain_incl;     /* F-PDCH parameters related to reverse
                                       link adjustment gains included indicator */
  byte rlgain_ackch_pilot;          /* R-ACKCH to pilot adjustment gain */
  byte rlgain_cqich_pilot;          /* R-CQICH to pilot adjustment gain */
  byte num_soft_switching_frames;   /* Number of frames for R-CQICH soft
                                       switching */
  byte num_softer_switching_frames; /* Number of frames for R-CQICH softer
                                       switching */
  byte num_soft_switching_slots;    /* Number of slots per frame for R-CQICH
                                          soft switching */
  byte num_softer_switching_slots;  /* Number of slots per frame for R-CQICH
                                          softer switching */
  byte pdch_soft_switching_delay;   /* F-PDCH soft switching delay */
  byte pdch_softer_switching_delay; /* F-PDCH softer switching delay */
  byte walsh_table_id;              /* The index of the Walsh Table used */
  byte num_pdcch;                   /* The number of Packet Data Control */
  byte for_pdcch_walsh[CAI_MAX_NUM_FOR_PDCCH]; /* Forward packet data control
                                                  walsh code */
  boolean imsi_10_incl;             /* IMSI 10 included */
  byte imsi_10;                     /* The least significant digit of MNC when MNC
                                       is a 3-digit number */
} mc_diag_espm;

typedef PACK (struct)
{
  word sid;                  /* System identification */
  word nid;                  /* Network identification */
  byte srch_win_a;           /* Characteristic field for Active Pilot Set */
  byte srch_win_n;           /* Characteristic field for Neighbor Pilot Set */
  byte srch_win_r;           /* Characteristic field for the Remaining Pilot set */
  byte t_add;                /* Pilot detection threshold */
  byte t_drop;               /* Pilot drop threshold */
  byte t_comp;               /* Active Set vs Candidate set comparsion
                                threshold */
  byte t_tdrop;              /* Drop timer value */
  byte nghbr_max_age;        /* Neighbor set member maximum age for retention */
  byte p_rev;                /* Protocol Revision Level */
  byte soft_slope;           /* Slope in criterion to add/drop active set pilots*/
  byte add_intercept;        /* Intercept in criterion to add to active set */
  byte drop_intercept;       /* Intercept in criterion for dropping pilot */
  byte packet_zone_id;       /* Packet data services zone identifier */
  boolean extension;         /* Indicator that extension fields are present */
  byte t_mulchan;            /* Supplemental Channel Request Message pilot
                                strength reporting offset. */
  byte begin_preamble;       /* Number of preamble frames at beginning */
  byte resume_preamble;      /* Number of preamble frames at resumption */
  boolean t_slotted_incl;
  byte t_slotted;

  /* REL A fields. */
  boolean is_is2000_rel_a_incl;     /* IS2000 Release A fields included indicator */
  boolean enc_supported;
  byte    sig_encrypt_sup;
  byte    ui_encrypt_sup;
  boolean cs_supported;

  /* REL C fields. */
  boolean is_is2000_rel_c_incl;     /* IS2000 Release C fields included indicator */
  boolean chm_supported;
  boolean cdma_off_time_rep_sup_ind;
  boolean cdma_off_time_rep_threshold_unit;
  byte    cdma_off_time_rep_threshold;
  boolean t_tdrop_range_incl;
  byte    t_tdrop_range;
  boolean for_pdch_supported;
  boolean pdch_chm_supported;
  boolean sdb_supported;
  boolean mob_qos;
  boolean ms_init_pos_loc_sup_ind;
} mc_diag_itspm;

typedef PACK (struct)
{
  boolean        spm_included;
  mc_diag_spm    spm;

  boolean        espm_included;
  mc_diag_espm   espm;

  boolean        itspm_included;
  mc_diag_itspm  itspm;
} mc_diag_system_params_values_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type      hdr;
    /* cmd_code, subsys_id, subsys_cmd_code, paramater values:
       MC_DIAG_SUBSYS_GET_NW_PARAMS */
  byte version;
    /* Version will be used in future if any other parameters are included.
       For now the version value is 1. */
  mc_diag_system_params_values_type     params;
    /* This will contain SPM, ESPM, ITSPM related fields. */
} mc_diag_get_system_params_rsp_type;

#endif /* FEATURE_1XCP_FTD */

#ifdef FEATURE_IS2000_1X_ADV
/*************** MC_DIAG_SUBSYS_ENH_MARKOV_STAT ***************/
/* Enhanced Markov Statistics */

typedef PACK (struct)
{
  diagpkt_subsys_header_type    hdr;
    /* cmd_code, subsys_id, subsys_cmd_code:MC_DIAG_SUBSYS_ENH_MARKOV_STAT */

} mc_diag_enh_markov_stat_req_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type      hdr;
    /* cmd_code, subsys_id, subsys_cmd_code:MC_DIAG_SUBSYS_ENH_MARKOV_STAT */

  PACK (struct)
  {
    byte mode;                   /* rxc mode                            */
    word markov_rate;            /* rate for markov processing          */
    word rx_state;               /* receive task state                  */
    dword total_frames;          /* total number of Markov frames       */
    dword bad_frames;            /* total number of bad frames          */
    dword data[ 5 ][ 10 ];        /* mar_test array items for expected
                                    full, half, quarter, eighth and zero
                                    rate packets                        */
    dword bit_errs;              /* Markov bit errors                   */
    dword good_errs[4];           /* Rate decision good,  but bits bad   */
  } stat;

} mc_diag_enh_markov_stat_rsp_type;
#endif

#ifdef FEATURE_MODEM_1X_MSO_RDA_TEST
/*************** MC_DIAG_SUBSYS_MAR_RDA_TEST_STAT ***************/
/* RDA testing using Markov frame count Statistics */

typedef PACK (struct)
{
  diagpkt_subsys_header_type    hdr;
    /* cmd_code, subsys_id, subsys_cmd_code:MC_DIAG_SUBSYS_MAR_RDA_TEST_STAT */

} mc_diag_mar_rda_test_stat_req_type;

typedef PACK (struct)
{
  diagpkt_subsys_header_type      hdr;
    /* cmd_code, subsys_id, subsys_cmd_code:MC_DIAG_SUBSYS_MAR_RDA_TEST_STAT */

  PACK (struct)
  {
    byte mode;                   /* rxc mode                            */
    word markov_rate;            /* rate for markov processing          */

    dword good_frames[ 5 ][ 5 ];   /* Frames without any bit errors.
    Five rows represent expected full, half, quarter, eighth and zero rate frames
    and five columns represent RDA-marked full rate, half rate, quarter rate,
    eighth rate and erasure frames */

    dword bad_frames[ 5 ][ 5 ];    /* Bad frames i.e. with bit errors
    Five rows represent expected full, half, quarter, eighth and zero rate frames
    and five columns represent RDA-marked full rate, half rate, quarter rate,
    eighth rate and erasure frames */
  } stat;

} mc_diag_mar_rda_test_stat_rsp_type;
#endif /* FEATURE_MODEM_1X_MSO_RDA_TEST */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*************** MC_DIAG_SUBSYS_RELOAD_NV_ITEMS ***************/

typedef PACK (struct)
{
  diagpkt_subsys_header_type    hdr;
    
} mc_diag_reload_nv_items_req_type;


typedef PACK (struct)
{
  diagpkt_subsys_header_type      hdr;
  /* Status indicates reload NV items command was sent to MC */
  byte                            status;

}mc_diag_reload_nv_items_rsp_type;

#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*************** MC_DIAG_SUBSYS_PAGING_MER_STAT ***************/

/* Defines and Externs for MC_DIAG_PAGING_MER_STAT Diag command */
#define MC_DIAG_PAG_RXED_REAL_SCI_ASGN_SLOT 0x01
#define MC_DIAG_PAG_RXED_REAL_SCI_NEXT_SLOT 0x02
#define MC_DIAG_PAG_RXED_TELE_SCI_ASGN_SLOT 0x04
#define MC_DIAG_PAG_RXED_TELE_SCI_NEXT_SLOT 0x08
#define MC_DIAG_REAL_SCI_PAGE_SLOT          0x10
#define MC_DIAG_TELE_SCI_PAGE_SLOT          0x20

/* One 80ms slot in 20ms */
#define MCLOG_ONE_SLOT 4

/* Variables for MER statistics */
typedef struct
{
  uint32 page_rxed_real_sci_asgn_slot_ctr;
  uint32 page_rxed_real_sci_next_slot_ctr;
  uint32 page_rxed_tele_sci_asgn_slot_ctr;
  uint32 page_rxed_tele_sci_next_slot_ctr;
  uint32 real_sci_page_slot_ctr;
  uint32 tele_sci_page_slot_ctr;
  boolean mer_stat_enabled;
    /* Indicates if MER statistical operation is enabled for a slot. */
  qword slot_sys_time;
    /* System time of slot */

} mclog_mer_type;

DIAGPKT_SUBSYS_REQ_DEFINE(CALLP1X, MC_DIAG_SUBSYS_PAGING_MER_STAT)
  byte   counters_bitmask;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CALLP1X, MC_DIAG_SUBSYS_PAGING_MER_STAT)
  uint32          page_rxed_real_sci_asgn_slot_ctr;
    /* Counter of pages received in assigned slot via real SCI */

  uint32          page_rxed_real_sci_next_slot_ctr;
    /* Counter of pages received in next slot via real SCI*/

  uint32          page_rxed_tele_sci_asgn_slot_ctr;
    /* Counter of pages received in assigned slot via telescoping SCI */

  uint32          page_rxed_tele_sci_next_slot_ctr;
    /* Counter of pages received in next slot via telescoping SCI */

  uint32          real_sci_page_slot_ctr;
    /* Counter of real SCI slot */

  uint32          tele_sci_page_slot_ctr;
    /* Counter of telecoping SCI slot */

DIAGPKT_RSP_END


/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_OP_MODE

DESCRIPTION
   Updates internal data and report events if necessary when operation mode
   changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_op_mode
(
  mclog_event_op_mode_type new_op_mode
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_MSG_RXED

DESCRIPTION
  Report event Message received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_msg_rxed
(
  cai_rx_channel_id_type channel_id,
  byte *msg_ptr
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_MSG_TXED

DESCRIPTION
  Report event Message transmitted.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_msg_txed
(
  cai_tx_channel_id_type channel_id,
  byte *msg_ptr
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_TIMER

DESCRIPTION
  Report event Timers T1m, T5m, T20m, T21m, T30m, T72m, T40m, T41m, T42m,
  T50m, T51m, T52m, T53m, T55m, T57m, T64m, T65m or T68m expired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_timer
(
  cai_timer_id_type timer_id
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_COUNTER

DESCRIPTION
  Report event Counters N1m, N14m, N15m, N2m, N3m, N5m, N11m threshold
  reached.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_counter
(
  cai_counter_id_type counter_id
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CP_STATE

DESCRIPTION
  Report events Call processing state change, Slotted mode operation. and
  QPCH in use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_cp_state
(
  word new_state
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CC_CREATE

DESCRIPTION
  Reports Call Control instantiated event for Release A and a
  corresponding CP state change event for NON-Release A

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_cc_create
(
 uint8 con_ref,
 cdma_cc_state_type new_state
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CC_STATE

DESCRIPTION
  Report event Call Control State Change

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_cc_state
(
 cdma_cc_state_type old_state,
 cdma_cc_state_type new_state,
 uint8 con_ref
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_PCH_ACQUIRED

DESCRIPTION
  Report event paging channel acquired for the first time after power up or
  operation mode change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_pch_acquired
(
  boolean pagech_acq,
    /* Flag indicates whether MS acquires the paging channel */
  byte pagech,
    /* Paging channel number of the acquired PCH */
  word pn_offset
    /* PN offset of the BS whose paging channel is acquired */
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CSCH_HO

DESCRIPTION
  Report event idle handoff, access probe handoff and access handoff.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_csch_ho
(
  cdma_ho_type ho_state,
  boolean cheap_idle_ho,
  word new_pilot,
  word new_freq,
  byte new_band_class,
  int8 ecio_old_pilot,
  int8 ecio_new_pilot,
  cdma_ho_type ho_type
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_CH_IND

DESCRIPTION
  Report event channel indicator change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_ch_ind
(
  cai_chind_type     ota_ch_ind,       /* Channel indicator */
  cai_ext_chind_type  ota_ext_ch_ind /* Extended Channel indicator */
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_F_FDCCH_ACQUIRED

DESCRIPTION
  Report event F-FCH acquired or F-DCCH acquired or F-FCH plus F-DCCH
  acquired.

DEPENDENCIES
  mcc_event.ch_ind must already be set correctly.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_f_fdcch_acquired(void);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_COMMON_CHANNEL_MONITORED

DESCRIPTION
  Report event common channel monitored.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_common_channel_monitored
(
  byte active_channel_mask
);

/*===========================================================================

FUNCTION MCLOG_REPORT_WAKEUP_SLOT

DESCRIPTION
  Report event of the wakeup slot

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_wakeup_slot
(
  byte      mask,
  boolean   slept
);

/*===========================================================================

FUNCTION MCLOG_REPORT_MATCHED_MSG

DESCRIPTION
  Report the csch mobile addressed message, after it passes address matching
  and duplicate detection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_matched_msg
(
  byte  msg_type,
  mclog_match_type  match_type
);

/*===========================================================================

FUNCTION MCLOG_REPORT_RELEASE

DESCRIPTION
  Report the call release with release reason

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_release
(
  mclog_release_reason_type reason
);

/*===========================================================================

FUNCTION MCLOG_REPORT_REGISTRATION_SUPPRESSED

DESCRIPTION
  Report that a Registration Message was suppressed.  The payload is the
  type of Registration that was suppressed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_registration_suppressed
(
  byte registration_suppressed_type
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_LONG_CODE_MASK

DESCRIPTION
  Report a change in the long code mask.  The mobile may switch between
  the private long code mask and public long code masks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_long_code_mask
(
  boolean always_report
  /* Normally, we check the new value against the old value to see if we
    need to report, but in the case of ECAM, we always need to report */
);

/*===========================================================================

FUNCTION MC_DIAG_INIT
  This function registers the subsystem dispatch command handler table with
  Diag.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mc_diag_init (void);

/*===========================================================================

FUNCTION MCLOG_MER_CONFIG
  This function enables or disables MER counter update. If MER counter update
  is enabled, update real/telecoping SCI page slot counters accordingly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mclog_mer_config
(
  boolean enable_mer_stat,
  qword slot_sys_time
);

/*===========================================================================

FUNCTION MCLOG_MER_UPDATE_COUNTER
  This function updates MER counters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mclog_mer_update_counter
(
  boolean in_tele, /* Indicates if it is telescoping or not */
  boolean in_slot, /* Indicates if it is in-slot or not */
  qword   page_rxed_sys_time /* System time that Page rxed */
);

/*===========================================================================

FUNCTION MCLOG_INC_NW_PARAM
  This function increments of the value of the network paramater.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mclog_inc_nw_param
(
  mc_diag_nw_param_type param_type
);

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_CSFB_TT_STATUS

DESCRIPTION
  Report event to indicate TT success/failure upon receiving UHDM/ECAM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_csfb_tt_status
(
  boolean tt_status,
  uint32  tt_ticks
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_HO_STATUS

DESCRIPTION
  Report event to indicate 1xcsfb handover success. This is sent after receiving
  ack order for handover completion message or if there is a failure encountered.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_csfb_ho_status
(
  boolean ho_status
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_GCSNA_DL_MSG

DESCRIPTION
  Report GCSNA event every time a GCSNA msg is received.
  Payload indicatse the message_id (1=GCSNA1xServie, 2=L2ACK, etc) and cdma_msg_type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_gcsna_dl_msg
(
  byte gcsna_msg_id,
  byte cdma_msg_id
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_GCSNA_UL_MSG

DESCRIPTION
  Report GCSNA event every time a GCSNA msg is sent.
  Payload indicatse the message_id (1=GCSNA1xServie, 2=L2ACK, etc) and cdma_msg_type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_gcsna_ul_msg
(
  byte gcsna_msg_id,
  byte cdma_msg_id
);

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_1X_TO_LTE
/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_1X_TO_LTE_RESEL_START

DESCRIPTION
  Report event 1X to LTE reselection is started.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mclog_report_event_1x_to_lte_resel_start
(
  lte_earfcn_t earfcn_id, /* Frequency of the LTE cell to be selected */
  uint16 cell_id   /* Cell id of the LTE cell to be selected */
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_1X_TO_LTE_RESEL_DONE

DESCRIPTION
  Report event 1X to LTE reselection is completed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mclog_report_event_1x_to_lte_resel_done
(
  mccidl_1x_to_lte_resel_status_type resel_status
);
#endif /* FEATURE_1X_TO_LTE */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_HOCM_SENT

DESCRIPTION
  Report event whenever (E)HOCM is sent on RL after Hard-Handoff.

DEPENDENCIES
None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_hocm_sent(void);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_PAGE_MATCH

DESCRIPTION
  Report page match event

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mclog_report_event_page_match( word service_option);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CSFB_CALL_TYPE

DESCRIPTION
  Report 1x csfb call type event

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mclog_report_event_csfb_call_type( csfb_call_info_type csfb_call_info);

#endif  /* MCLOG_H */
