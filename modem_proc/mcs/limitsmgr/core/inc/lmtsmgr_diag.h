#ifndef __LMTSMGR_DIAG_H__
#define __LMTSMGR_DIAG_H__
/*!
  @file lmtsmgr_diag.h

  @brief
   Diag interface used by LMTSMGR module

*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/core/inc/lmtsmgr_diag.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/10/14   jm      Slot Level activity log support for frame boundary
08/25/14   jm      Circular buffer for slot level activity tl
08/12/14   jm      Slot level activity timeline support
04/28/14   jm      L+G Band Avoidance support
03/02/14   ag      Added type to the power info log packet
04/12/13   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "log_codes.h"
#include "log.h"
#include "wwan_coex_mgr.h"
#include "wwcoex_action_iface.h"
#include "wwcoex_conflict_table.h"
/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/
#define FEATURE_LOG_TECH_STATE
#define FEATURE_LOG_SCENARIO_TBL

#define WWCOEX_LOG_POWER_SUBCODE  2
#define WWCOEX_LOG_FREQUENCY_SUBCODE 3
#define WWCOEX_LOG_BLIST_RSP_SUBCODE 4

#define WWCOEX_LOG_MAX_TL_ENTRIES ((CXM_MAX_NUM_TIMING_SLOTS*LMTSMGR_TIMING_HIST_RECS)+LMTSMGR_TIMING_HIST_RECS)

/*=============================================================================

                             TYPEDEFS

=============================================================================*/
/*---------------------------------------------------------------------------
  Header Type (AFC logging)
---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  cxm_action_type action;
  wwan_coex_desense_type desense;
  uint16 spur_handle1;
  uint16 spur_handle2;
}wwcoex_log_arr_entry_type;

typedef PACK(struct)
{
  wwcoex_log_arr_entry_type current_tbl[CXM_MAX_SUPPORTED_LINK_SETS * CXM_MAX_SUPPORTED_LINK_SETS];
//  wwcoex_log_arr_entry_type old_tbl[CXM_MAX_SUPPORTED_LINK_SETS*CXM_MAX_SUPPORTED_LINK_SETS];
//  uint32  num_old_entries;
}wwcoex_log_tbl_type;

typedef PACK(struct)
{
  uint32                  freqId;
  uint32                  frequency; /*!< Tech operating frequency in KHz */
  uint32                  bandwidth; /*!< Tech operating bandwidth in Hz */
  int16                   power;     /*!< Power associated with this */
  uint8                   band;      /*!< Operating Sys Band */
  uint8                   direction; /*!< Direction (Up-link/Down-Link/Both) */
  uint8                   type;      /*!< Type (PM or Div or Unused) */
}wwcoex_log_freq_link_type;

typedef PACK(struct)
{
  wwcoex_log_freq_link_type   links[CXM_MAX_SUPPORTED_LINK_SETS];
}wwcoex_log_tech_freq_state;


typedef PACK(struct)
{
  uint8  subcode;
  uint8  num_t1entries;
  uint8  num_t2entries;
  uint8  tech1;
  uint8  tech2;
  uint32 tech1_curr_fid_offset;
  uint32 tech2_curr_fid_offset;
}wwcoex_state_log_hdr_type;

typedef PACK(struct)
{
  log_hdr_type hdr;
  uint8  version;
  wwcoex_state_log_hdr_type state_hdr;
  wwcoex_log_tbl_type scenario_tbls;
} wwcoex_state_log_type;

typedef PACK(struct)
{
  uint8  subcode;
  uint8  tech_id;
  uint8  num_entries;
}wwcoex_power_log_hdr_type;

typedef PACK(struct)
{
  log_hdr_type hdr;
  uint8  version;
  wwcoex_power_log_hdr_type power_hdr;
  wwcoex_log_tech_freq_state tech_list;
}wwcoex_power_log_type;


typedef PACK(struct)
{
  uint8  bl_id;
  uint8  response;
}wwcoex_log_tech_blist_rsp_type;

typedef PACK(struct)
{
  uint32  frequency; /*!< Tech operating frequency in KHz */
  uint32  bandwidth; /*!< Tech operating bandwidth in Hz */
  int16   power;     /*!< Power associated with this (dBm*10) */
  uint8   band;      /*!< Operating Sys Band */
  uint8   direction; /*!< Direction (Up-link/Down-Link/Both) */
}wwcoex_log_serv_freq_entry_type;

typedef PACK(struct)
{
  wwcoex_log_serv_freq_entry_type entries[CXM_MAX_SUPP_FREQ_LINK_BA];
}wwcoex_log_tech_serv_freq_state;

typedef PACK(struct)
{
  log_hdr_type                                hdr;
  uint8                                   version;
  wwcoex_power_log_hdr_type             power_hdr;
  union wwcoex_serv_subcode
  {
    wwcoex_log_tech_serv_freq_state  serv_freq_list;
    wwcoex_log_tech_blist_rsp_type   bl_rsp;
  }u;
}wwcoex_ba_serv_power_log_type;


typedef PACK(struct)
{
  uint8  blacklist_id;
  uint8  tech_id;
  uint8  serv_freq_state;
  uint8  serv_freq_mode;
  uint8  num_entries;
}wwcoex_ba_blist_hdr_type;

typedef PACK(struct)
{
  uint8                   band;         /*!< Operating Sys Band */
  uint32                  freq_lower;   /*!< Lower bounds (KHz) */
  uint32                  freq_upper;   /*!< Upper bounds (KHz) */
  uint8                   desense_mode; /*!< Victim (DL) or Aggressor (UL) */
  int16                   rxpwr_thresh; /*!< Rx Power in dBm*10 format; filled when desense mode is Victim;
                                               CXM_UNKNOWN_POWER if no threshold */
  int16                   txpwr_thresh; /*!< Tx Power in dBm*10 format; filled when desense mode is Aggr;
                                               CXM_UNKNOWN_POWER if no threshold */
}wwcoex_log_ba_list_entry_type;

typedef PACK(struct)
{
  wwcoex_log_ba_list_entry_type entries[CXM_MAX_SUPP_FREQ_LINK_BA];
}wwcoex_log_blist_state;

typedef PACK(struct)
{
  log_hdr_type hdr;
  uint8  version;
  wwcoex_ba_blist_hdr_type        blist_hdr;
  wwcoex_log_blist_state          blacklist;
}wwcoex_ba_blist_log_type;

typedef PACK(struct)
{
  /*! GSM Call type */
  uint8 call_type;
  
  /*! Start time (in USTMR units) of a new block, which indicates
      any activity reported after this time is NOT accurate.
      Macro CXM_INVALID_USTMR_TIME indicates all activity comes
      from same block */ 
  uint32 new_block_time;
    
  /*! Bit-wise enable validity indicator for GSM specific
      conditional metrics (defined below).  */
  uint8 metrics_valid_mask;

  /*! Start frame boundary (in USTMR units) of each
      frame in the message  */ 
  uint32 frame_start_time[CXM_MAX_GSM_LOOK_AHEAD_FRAMES];  
  
  /*! GSM Specific Conditional Metrics.
  
    Position 0: Max sustainable conecutive GSM UL DTx-ed frames after
    current GSM frame, measured in time (ms)
	
    Position 1: Max cluster of GSM UL DTx-ed frames with 1 intermittent
    Tx burst (SACCH) counted after current GSM frame,
    measured in time (ms)
	
    Position 2: Max cluster of GSM UL DTx-ed frames with 2 intermittent
    Tx bursts (SACCH) counted after current GSM frame,
    measured in time (ms)
  */
  uint32 cond_metics[CXM_MAX_GSM_METRIC_TYPES];
}wwcoex_gsm_metrics_type;

typedef PACK(union)
{
  wwcoex_gsm_metrics_type gsm_metrics; /*!< GSM specific parameters */ 
}wwcoex_timing_info_tech_info_type;

typedef PACK(struct)
{
  uint8  send_tech_id;
  uint8  rec_tech_id;
  uint8  in_dtx_state;
  uint32 dtx_switch_time;
  uint32 dtx_time;
  uint8  num_entries;
  wwcoex_timing_info_tech_info_type tech_metrics;
}wwcoex_timing_info_hdr_type;

typedef PACK(struct)
{
  uint32 freqid;        /*! Frequency ID */
  uint32 frequency;     /*! Frequency (kHz) */
  uint8  band;          /*!< Tech operating band (sys_band_class_e_type) */
  uint8  direction;     /*! Type of activity being registered, Uplink/Downlink */ 
  uint8  link_type;      /*! Link Type of slot (Normal/Diversity/PowerMonitor) */
  uint32 start_time;    /*! Start time (in USTMR units) of the activity in the next slot (unpadded)*/ 
  uint32 end_time;      /*! End time (in USTMR units) of the activity in the next slot (unpadded)*/ 
  uint8  micro_prio;    /*! Micro priority which maps to ACTIVITY_TIER */ 
  uint16 conflict_mask; /*! Bitwise mask based off of cxm_tl_conflict_type */
}wwcoex_act_tl_list_entry_type;

typedef PACK(struct)
{
  wwcoex_act_tl_list_entry_type entries[WWCOEX_LOG_MAX_TL_ENTRIES];
}wwcoex_act_tl_list_type;

typedef PACK(struct)
{
  log_hdr_type hdr;
  uint8  version;
  wwcoex_timing_info_hdr_type     timing_hdr[LMTSMGR_TIMING_HIST_RECS];
  wwcoex_act_tl_list_type         sl_act_list;
}wwcoex_timing_info_log_type;

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  wwcoex_log_power

=============================================================================*/
/*!
    @brief
    Logs the power and frequency information for a particular tech
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_power
(
  cxm_tech_type tech_id,
  uint8 subcode
);

/*=============================================================================

  FUNCTION:  wwcoex_log_state

=============================================================================*/
/*!
    @brief
    Logs all the WWAN Coex state information
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_state
(
  cxm_tech_type tech1,
  cxm_tech_type tech2,
  wwcoex_tbl_type* result_tbl
);

/*=============================================================================

  FUNCTION:  wwcoex_log_serv_power

=============================================================================*/
/*!
    @brief
    Logs the serving power and frequency information for a particular tech
    (ie for Band Avoidance)
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_serv_power
(
  cxm_tech_type tech_id,
  uint8 subcode
);

/*=============================================================================

  FUNCTION:  wwcoex_log_blacklist

=============================================================================*/
/*!
    @brief
    Logs the blacklist information (ie for Band Avoidance)
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_blacklist
(
  cxm_tech_type tech_id
);

/*=============================================================================

  FUNCTION:  wwcoex_log_sl_act_tl

=============================================================================*/
/*!
    @brief
    Logs the slot level activity timeline
 
    @details
    L1->MCS sends the bulk of this information but MCS adds
    potential conflict type to each activity slot
 
    @return
    None
*/
/*===========================================================================*/
void wwcoex_log_sl_act_tl
(
  cxm_tech_type rec_tech
);

#endif /* __LMTSMGR_DIAG_H__ */

