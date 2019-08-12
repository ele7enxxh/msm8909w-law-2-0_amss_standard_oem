#ifndef __WWAN_COEX_MGR_H__
#define __WWAN_COEX_MGR_H__
/*!
  @file wwan_coex_mgr.h

  @brief
   APIs exposed by the CXM for WWAN-WWAN COEX

*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/wwan_coex_mgr.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/11/14   ag      Added interface for combining micro prios for FDD techs
10/10/14   ag      Added interface for distress, tech coex state
10/07/14   jm      Add new field in SL Timeline to indicate frame boundary
08/11/14   jm      Adding Band Avoidance support for 1x/HDR/WCDMA/TDSCDMA
07/31/14   ag      Added TIER_30 and support for unified micro priority
07/21/14   jm      Enhancements to GSM slot level activity messaging
06/23/14   jm      Add support for GSM Slot Level Activity Messaging
06/23/14   sg      desense indication mechanism
04/28/14   jm      L+G Band Avoidance support
03/27/14   jm      Added API to query UL/DL status of all techs
03/19/14   jm      WWAN coex based off of RF device assignment
03/19/14   ag      Added TIER_3 priority for ASDIV
01/15/14   jm      Ensure freqID is 16-bit value
01/14/14   rj      Adding support for SGLTE+G channel ID info 
12/07/13   ag      Added new activity tiers for T+G
11/06/13   rj      Adding prob_aggr_mask support
04/05/13   rj      GPIO Based SAR Control added
04/03/13   ag      WWAN Coex Mgr interface version 1
01/16/13   ckk     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "cxm.h"
#include "trm.h"
#include "subs_prio.h"

/* DAL APIs */
#include "DDITlmm.h"

/*=============================================================================

                       COMMON TYPES AND DEFINES

=============================================================================*/
/*! Number of inactivity slots that will be communicated at a time */
#define CXM_NUM_INACTVITY_SETS  15

/*! Unknown Power indicator: To be used when the power information is not 
  available. */
#define CXM_UNKNOWN_POWER  (int32)(-32765)

/*! Unknown freqId: To be used by tech to register with FW for very short time 
  period activities */
#define CXM_UNKNOWN_FREQID  0x0000FFF0

/*! VALID Channel ID: To be used to check whether channel ID is valid */
#define CXM_CHANNELID_VALID  0x80

/*! Maximum number of entries supported in the Band Avoidance Blacklist */
#define CXM_MAX_SUPP_BL_ENTRIES  24

/*! Maximum number of serving frequencies supported in Band Avoidance */
#define CXM_MAX_SUPP_FREQ_LINK_BA 64

/*! Utilizing this flag indicates that there is not a 1:1 mapping with frequency list.
    Tx/Rx entry in the power list shall be applied to all frequencies */
#define BA_PWR_LIST_FILL_ALL 0x1

/*! Maximum number of entries supported in slot timing info list. 
    4 frame look-ahead * 8 slots per frame */
#define CXM_MAX_NUM_TIMING_SLOTS 32

/*! Maximum number of GSM frames supported in the slot timing info list. */
#define CXM_MAX_GSM_LOOK_AHEAD_FRAMES 4

/*! To be used by tech to indicate unknown/invalid time for filling
    USTMR fields */
#define CXM_INVALID_USTMR_TIME 0xFFFFFFFF

/*! Maximum number of GSM specific conditional metrics */
#define CXM_MAX_GSM_METRIC_TYPES 3

/*! Maximum number of concurrent tech registration supported */
#define CXM_MAX_CONCURRENT_TECHS 4

/*! Maximum number of carriers supported in UL/DL multi-carrier operation */
#define CXM_MAX_CARRIERS  3

/*! @brief Enum for Coex State */
typedef enum
{
  /*! None of the technology is in Tx state so coex module is in inactive 
      state */
  COEX_INACTIVE_ST,
  
  /*! Single Tech is in Tx state */
  COEX_SINGLE_ACTIVE_ST,

  /*! Two technologies are in active Tx state  */
  COEX_DUAL_ACTIVE_ST
}wwcoex_state_type;

/*! @brief Enum for Activity Tiers 
           This enum alongwith the techId determine the priority value
 */
typedef enum
{
  /*! Highest Priority Tier: To be used only for ASDIV activity */
  ACTIVITY_TIER_3,

  /*! Second highest Priority Tier (Super Critical Tier): Should be used for
   activities which needs to be protected from the other tech  */
  ACTIVITY_TIER_5,

  /*! Third highest Priority Tier (Nominal Tier): Should be used for activities 
    which needs to be protected from the other tech            */
  ACTIVITY_TIER_10,

  /*! Fourth highest Priority Tier (Nominal- Tier): Used for W2G IRAT Rx measurements */
  ACTIVITY_TIER_15,
  
  /*! Third lowest Tier (Low Tier): Second highest priority for activities that
    can be blanked when it conflicts with other tech's higher priority tier */
  ACTIVITY_TIER_20,

  /*! Second Lowest Tier (Opportunistic Tier): Third highest priority for activities
    that can be blanked when it conflicts with other tech's higher priority tier. */
  ACTIVITY_TIER_25,

  /*! Lowest Tier: To be used only during tuneaway gap by data tech. */
  ACTIVITY_TIER_30,

  /* End of the list indicator */
  MAX_ACTIVITY_TIERS
}cxm_activity_type;

/*! @brief Enum for FA Response*/
typedef enum{
  FREQ_AVOID_INVALID,   /*! Not to be used */
  FREQ_AVOID_NACK,      /*! Unable to change serv freq */
  FREQ_AVOID_NO_PROBLEM /*! Current serv freq is acceptable - only used when serv freq is victim */
}wwcoex_fa_res_type; 

/*! @brief Serving Frequency State */
typedef enum{
  SERV_FREQ_OK,         /*! Serving frequency has no desense issue with other active tech */
  SERV_FREQ_AVOID_REQ,  /*! Serving frequency has issue with other active tech - response expected by MCS */
  SERV_FREQ_MAX         /*! End of list indicator */
}wwcoex_serv_freq_state_type;

/*! @brief Blacklist desense type */
typedef enum{
  DESENSE_NONE,           /*! No desense detected */
  DESENSE_AGGR,           /*! Desense is caused by aggressor (Tx) */
  DESENSE_VICTIM,         /*! Desense is caused by victim (Rx) */
  DESENSE_AGGR_VICTIM,    /*! Both Aggressor and Victim */
  DESENSE_MAX             /*! End of list indicator */
}wwan_coex_desense_mode;

/*! @brief Enum for GSM call type */
typedef enum
{
  /*! GSM is in no call */
  GSM_CALL_NONE,
  
  /*! GSM call is NOT voice-only */
  GSM_CALL_NOT_VOICE_ONLY,
  
  /*! CS Voice call with TCH half-rate codec & sub-channel 0 */
  GSM_CALL_VOICE_TCH_HALF_RATE_SUB_0,
  
  /*! CS Voice call with TCH half-rate codec & sub-channel 1 */
  GSM_CALL_VOICE_TCH_HALF_RATE_SUB_1,
  
  /*! CS Voice call with TCH full-rate codec */
  GSM_CALL_VOICE_TCH_FULL_RATE,
  
  /*! CS Voice call with AMR half-rate codec & sub-channel 0 */
  GSM_CALL_VOICE_AMR_HALF_RATE_SUB_0,
  
  /*! CS Voice call with AMR half-rate codec & sub-channel 1 */
  GSM_CALL_VOICE_AMR_HALF_RATE_SUB_1,  
  
  /*! CS Voice call with AMR full-rate codec */
  GSM_CALL_VOICE_AMR_FULL_RATE,
  
  /*! CS Signalling Call */
  GSM_CALL_CS_SDDCH,
  
  /*! Dual Transfer Mode (Voice+Data) with half-rate codec & sub-channel 0 */
  GSM_CALL_DTM_HALF_RATE_SUB_0,
  
  /*! Dual Transfer Mode (Voice+Data) with half-rate codec & sub-channel 1 */
  GSM_CALL_DTM_HALF_RATE_SUB_1,
  
  /*! Dual Transfer Mode (Voice+Data) with full-rate codec */
  GSM_CALL_DTM_FULL_RATE,
  
  /*! Packet Switched call */
  GSM_CALL_PS
  
}wwcoex_gsm_call_type;

typedef enum
{
  /*! Activity has no known conflict with other active tech */
  CXM_TL_CONFLICT_NONE,
  
  /*! Activity has potential desense issue with other active tech's Rx */
  CXM_TL_CONFLICT_POT_DESENSE_RX,
  
  /*! Activity has desense issue with other active tech's Rx */
  CXM_TL_CONFLICT_DESENSE_RX,
  
  /*! Activity has potential desense issue with other active tech's Tx */
  CXM_TL_CONFLICT_POT_DESENSE_TX,
  
  /*! Activity has desense issue with other active tech's Tx */
  CXM_TL_CONFLICT_DESENSE_TX,
  
  /*! Activity has Tx channel conflict with other active tech's Tx */
  CXM_TL_CONFLICT_CHANNEL_TX
  
}cxm_tl_conflict_type;

typedef enum
{
  /*! Tech is not in distress */
  DISTRESS_NONE,        

  /*! Tech is in distress     */
  DISTRESS_HIGH       
  
} cxm_distress_lvl_type;

/*! @brief Frequency Link information */
typedef struct
{
  /* Frequency ID */
  uint32  freqid;

  /* Frequency information corresponding to the freqId */
  cxm_tech_link_info_s link_info;
}coex_freqid_link_s;

/*! @brief  Frequency ID List */
typedef struct
{
  uint8 num_link_info_sets;
  coex_freqid_link_s  link_list[CXM_MAX_SUPPORTED_LINK_SETS];
}coex_freqid_list_s;


/*! @brief Power Link information */
typedef struct
{
  /* Frequency ID for which power is being updated */
  uint32  freqid;

  /* Direction: UL if Tx Power is updated, DL if Rx is updated 
     or UL_AND_DL if both are updated */
  cxm_tech_link_direction direction;

  /* Tx Power in dBm*10 format. 
     Will be accessed only if direction = UL or UL_AND_DL */
  int32   tx_power;

  /* Rx Power in dBm*10 format. 
     Will be accessed only if direction = DL or UL_AND_DL */
  int32   rx_power;
}coex_power_link_s;


/*! @brief  Power List */
typedef struct
{
  uint8 num_link_info_sets;
  coex_power_link_s  link_list[CXM_MAX_SUPPORTED_LINK_SETS];
}coex_power_list_s;

/*! @brief Activity Table for querying priority corr to an activity */
typedef struct
{
  /* Activity for which priority is desired */
  cxm_activity_type  activity;

  /* Priority value */
  uint32             priority;
}cxm_activity_table_s;

/*! @brief  Inactivity Slot Timing Info */
typedef struct
{
  /*! Start time (in USTMR) of the inactivity slot */
  uint32         start_time;

  /*! Duration (in USTMR) of the inactivity slot */
  uint32         duration;
}cxm_inactivity_slot_list_s;

/*! @brief  WWCoex State Info */
typedef struct
{
  /*! WWCoex State */
  wwcoex_state_type    coex_st;

  /*! Bitmask of probable victim techs */
  uint32               prob_victim_mask;

  /*! Bitmask of probable aggr techs */
  uint32               prob_aggr_mask;

}cxm_wwcoex_state_info_s;

/*! @brief Band Avoidance - Serving Frequency Entry */
typedef struct{
  uint32 frequency;                  /*!< Tech operating frequency in KHz */
  uint32 bandwidth;                  /*!< Tech operating bandwidth in Hz */
  uint32 band;                       /*!< Tech operating band (sys_band_class_e_type) */
  uint32 channel;                    /*!< Tech operating channel */
  cxm_tech_link_direction direction; /*!< Direction: UL/DL; if UL & DL is selected, 
                                          offset will be used to generate DL frequency */
}coex_ba_serv_freq_list_s;

/*! @brief Band Avoidance - Serving Frequency Info */
typedef struct{
  cxm_tech_type               tech_id;
  uint16                      num_entries;                           /*! Number of serving frequencies in list */
  coex_ba_serv_freq_list_s    freq_list[CXM_MAX_SUPP_FREQ_LINK_BA];  /*! List of serving frequencies */
}coex_ba_freq_data;

typedef struct{
  cxm_tech_link_direction   direction; /*! UL if Tx, DL if RX */
  int32                     tx_pwr;    /*! Filled when direction is Tx */
  int32                     rx_pwr;    /*! Filled when direction is Rx */
}coex_ba_serv_pwr_list_s;

/*! @brief Band Avoidance - Serving Power Info */
typedef struct{
  cxm_tech_type              tech_id;
  uint16                     num_entries;                         /*! Number of serving power entries in list */
  uint16                     handle_flag;                         /*! Options to handle pwr_list (Bitwise-OR) */
  coex_ba_serv_pwr_list_s    pwr_list[CXM_MAX_SUPP_FREQ_LINK_BA]; /*! Power provided in same order as Serving
                                                                      Frequency Info (unless handle flag is set) */
}coex_ba_pwr_data;

/*! @brief Band Avoidance - Blacklist Response */
typedef struct{
  cxm_tech_type             tech_id;
  uint16                    blacklist_id;   /*! Correlates w/ Blacklist data */
  wwcoex_fa_res_type        freq_avoid_res; /*! Blacklist response from L1 if serving
                                                frequency has not changed */
}coex_ba_blist_res_data;

/*! @brief Band Avoidance - Blacklist Entry */
typedef struct{
  uint32                    band;         /*!< sys_band_class_e_type */
  uint32                    freq_lower;   /*!< Lower bounds (KHz) - any freq beyond the band should
                                               be considered as MIN(band) */
  uint32                    freq_upper;   /*!< Upper bounds (KHz) - any freq beyond the band should 
                                               be considered as MAX(band) */
  wwan_coex_desense_mode    desense_mode; /*!< Victim (DL) or Aggressor (UL) */
  int16                     rxpwr_thresh; /*!< Rx Power in dBm*10 format; filled when desense mode is Victim;
                                               CXM_UNKNOWN_POWER if no threshold */
  int16                     txpwr_thresh; /*!< Tx Power in dBm*10 format; filled when desense mode is Aggr;
                                               CXM_UNKNOWN_POWER if no threshold */
}coex_ba_list_s;

/*! @brief Band Avoidance - Blacklist Data */
typedef struct{
  uint16                      blacklist_id;                       /*! Correlates w/ blacklist response */
  wwcoex_serv_freq_state_type serv_freq_state;                    /*! Indicates if serving frequency must be changed */
  wwan_coex_desense_mode      serv_freq_mode;                     /*! Desense type of serving frequency (if any) */
  uint16                      num_entries;                        /*! Number of entries in blacklist */
  coex_ba_list_s              freq_list[CXM_MAX_SUPP_BL_ENTRIES]; /*! List of frequency ranges which 
                                                                  have desense issues with other tech*/
}coex_ba_blist_data;

/*! @brief Coex desense indication */
typedef struct{
  boolean                     is_reg;    /*! is the client registered for desense indication  */
  msgr_umid_type              umid;      /*! UMID used for sending indication to clients. 
                                             Clients define their own UMID and pass it to CXM */
}coex_desense_ind_reg_s;

/*! @brief Coex desense indication */
typedef struct{
  boolean                     is_desense;    /*! TRUE indicates Desense. FALSE indicates No Desense  */
}coex_desense_ind_s;

/*! @brief Coex Tech State for broadcast indication */
typedef struct {

  cxm_tech_type             tech_id;

  /*! TRUE when tech registers DL freq with MCS e.g entering acquisition state
     FALSE when tech deregisters DL freq with MCS e.g going inactive    */      
  boolean                   rx_active;

  /*! TRUE when tech registers UL freq with MCS e.g entering access/connected state
      FALSE when tech deregisters UL freq with MCS e.g exiting access/connected state */
  boolean                   tx_active; 
   	  
  /*! Distress state */
  cxm_distress_lvl_type      distress_state;

  /*! Subscription ID */
  sys_modem_as_id_e_type     as_id;

  /*! Macro priority SUB state from subs_prio.h e.g Voice High for active voice call,
      Idle and Acq cannot be distinguished. for both, this state will be BASELINE. */
  sp_subs_states_enum_t      macro_state; 
  
  /*! Number of active UL carriers */
  uint8                      num_ul_carriers; 

  /*! Number of active DL carriers */
  uint8                      num_dl_carriers; 
  
  /*! UL Band information of each carrier */
  sys_band_class_e_type      ul_bands[CXM_MAX_CARRIERS];

  /*! DL Band information of each carrier */
  sys_band_class_e_type      dl_bands[CXM_MAX_CARRIERS];

}cxm_coex_tech_data;

/*! @brief Input structure to cxm_get_combined_priority api */
typedef struct 
{
  /*! Tech Id requesting for prio combining */
  cxm_tech_type    tech_id;
  
  /*! Set to FALSE if other arguments are the numeric priority values
      Set to TRUE if other arguments are cxm_activity_type enum values
      The output will also be of the corresponding type (activity or priority) */  
  boolean    are_inputs_tier_based;
  
  /*! Pointer to Rx priority or activity tier information */
  uint8      *rx_prio;
  
  /*! Pointer to Tx priority or activity tier information */
  uint8      *tx_prio;
} cxm_combine_prio_input_type;
/*=============================================================================

                       MESSAGE DATA TYPES/STRUCTURES

=============================================================================*/

/*! @brief Message for WWAN Coexistence Frequency ID Indication sent to tech L1*/
typedef struct
{
  msgr_hdr_struct_type      msg_hdr; /*!< msgr header contianing msg_id */
  coex_freqid_list_s        freq_data;
}cxm_freqid_info_ind_s;

/*! @brief Message for WWAN Coexistence Power Indication sent to MCS */
typedef struct
{
  msgr_hdr_struct_type      msg_hdr; /*!< msgr header contianing msg_id */
  cxm_tech_type             tech_id;
  coex_power_list_s         power_data;
}cxm_coex_power_ind_s;

/*! @brief Message for Activity Timeline Indication sent to MCS */
typedef struct
{
  msgr_hdr_struct_type      msg_hdr;

  /*! Technology Identifier */
  cxm_tech_type             tech_id;

  /*! Type of activity being registered, Uplink/Downlink/Both */
  cxm_tech_link_direction   direction;

  /*! Start time (in USTMR units) of the activity in the next frame */
  uint32                    start_time;

  /*! End time (in USTMR units) of the activity in the next frame */
  uint32                    end_time;

  /*! Period (USTMR units) with which the activity repeats */
  uint32                    period;
}cxm_activity_timeline_s;

/*! @brief Message for Client Registration for GSM Tx inactivity sent to MCS */
typedef struct
{
  msgr_hdr_struct_type      msg_hdr;

  /*! Technology Identifier of the client that is registering */
  cxm_tech_type             tech_id;

  /*! Registration flag should be set to TRUE when interested in the Tx 
      inactivity information. For deregistration, this flag should be set to
      FALSE */
  boolean                   reg_flag;
}cxm_inactivity_client_reg_s;

/*! @brief GSM Tx Inactivity information for 1X/HDR L1 */
typedef struct
{
  msgr_hdr_struct_type        msg_hdr;

  /*! This flag will be set to FALSE if the inactivity is not slotted and TRUE 
    otherwise. e.g if GSM Tx is off, then this flag will be FALSE. */
  boolean                     is_slotted;

  /*! List of inactivity slots. This should be accessed only if is_slotted 
    is TRUE */
  cxm_inactivity_slot_list_s  slot_list[CXM_NUM_INACTVITY_SETS];

}cxm_inactivity_report_s;

/*! @brief Message for registering/deregistering with GSM for Activity 
           information */
typedef struct
{
  msgr_hdr_struct_type        msg_hdr;

  /*! Registration flag should be set to TRUE when interested in the Tx 
      activity information. For deregistration, this flag should be set to
      FALSE */
  boolean                     reg_flag;

  /*! Type of activity for which timeline is required (e.g Uplink,Downlink) */
  cxm_tech_link_direction     direction;

}cxm_request_activity_info_s;

/*! @brief Timing Slot Info */
typedef struct {
  /*! Frequency ID */
  uint32 freqid; 

  /*! Frequency (kHz) */
  uint32 frequency;
  
  /*!< Tech operating bandwidth in Hz */
  uint32 bandwidth;

  /*!< Tech operating band (sys_band_class_e_type) */
  uint32 band;

  /*! Type of activity being registered, Uplink/Downlink - Both is considered invalid */ 
  cxm_tech_link_direction direction;

  /*! Link Type of slot (Normal/Diversity/PowerMonitor) */
  cxm_tech_link_type link_type;

  /*! Start time (in USTMR units) of the activity in the next slot (unpadded)*/ 
  uint32 start_time;

  /*! End time (in USTMR units) of the activity in the next slot (unpadded)*/ 
  uint32 end_time; 

  /*! Micro priority which maps to ACTIVITY_TIER */ 
  uint32 micro_prio;

  /*! Relative priority of activity */
  cxm_slot_prio_e wlan_prio;

  /*! Power for slot (dBm * 10) */ 
  int32 pwr;
  
  /*! Bitwise mask based off of cxm_tl_conflict_type; to be filled by MCS only
      Example: 0b1010 =
      (1 << CXM_TL_CONFLICT_DESENSE_RX) | (1 << CXM_TL_CONFLICT_CHANNEL_TX) */
  uint16 conflict_mask;

}cxm_timing_slot_entry; 

/*! @brief GSM specific metrics for slot timeline info */
typedef struct
{  

  /*! GSM Call type */
  wwcoex_gsm_call_type call_type;
  
  /*! Start frame boundary (in USTMR units) of each
      frame in the message  */ 
  uint32 frame_start_time[CXM_MAX_GSM_LOOK_AHEAD_FRAMES];  
  
  /*! Start time (in USTMR units) of a new block, which indicates
      any activity reported after this time is NOT accurate.
      Macro CXM_INVALID_USTMR_TIME indicates all activity comes
      from same block */ 
  uint32 new_block_time;
    
  /*! Bit-wise enable validity indicator for GSM specific
      conditional metrics (defined below).  */
  uint8 metrics_valid_mask;
  
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

}cxm_gsm_metric_info_s;

/*! @brief Union for all TECH specific metrics for timeline info */
typedef union
{
  cxm_gsm_metric_info_s gsm_metrics; /*!< GSM specific parameters */
  
}cxm_tech_specific_metrics_u;

typedef struct {

  /*! DTx state indicator */
  boolean in_dtx_state;
  
  /*! Time (in USTMR units) where DTx state toggles within the
      four frame reporting. Macro CXM_INVALID_USTMR_TIME indicates
      all activity is in same state */
  uint32 dtx_switch_time;
  
  /*! DTx time metric:
      - Current time elapsed (in ms) in DTx state if indicator is TRUE
      - Current time elapse (in ms) out of DTx if indicator is FALSE */
  uint32 dtx_time;  
  
}cxm_dtx_state_info;

/*! @brief Message for Slot Level Timing Indication sent to MCS */ 
typedef struct { 

  msgr_hdr_struct_type msg_hdr; 

  /*! Technology identifier */ 
  cxm_tech_type tech_id; 

  /*!  Number of activity slots in list */ 
  uint8 num_entries;

  /*! List of slots - DTx activity should not be included */
  cxm_timing_slot_entry slot_list[CXM_MAX_NUM_TIMING_SLOTS];
  
  /*! Information of UL DTx */
  cxm_dtx_state_info dtx_info;
   
  /*! Tech specific metrics */
  cxm_tech_specific_metrics_u tech_metrics;

}cxm_timing_info_s;

/*! @brief Message for sending the WWCOEX state information */
typedef struct
{
  msgr_hdr_struct_type        msg_hdr;

  /*! Wwcoex state information... */
  cxm_wwcoex_state_info_s     st_info;

}cxm_wwcoex_state_update_s;

/*! @brief structure for setting DSI value
     this structure will be passed as payload to msgr */
typedef struct
{
  msgr_hdr_struct_type        msg_hdr; /*!< msgr header contianing msg_id */
  cxm_sar_dsi_type            dsi_val; /*!< DSI value */
  boolean                     is_gpio_valid;/*!< GPIO value valid */
  DALGpioValueType            gpio_value; /*!< GPIO value */
}cxm_sar_set_dsi_s;

/*! @brief structure for getting RF Device from Tech L1
     this structure will be passed as input argument to get channel_ID */
typedef struct
{
    cxm_tech_type techid;
    uint8   rf_device; /* RF device (rfm_device_enum_type) returned by TRM */
}cxm_channel_in_type;

/*! @brief structure for channel ID corresponding to RF Device
     this structure will be returned from API call     */
typedef struct
{
   uint8  rx_channel_id;   /* Channel Id for Rx operation */
   uint8  tx_channel_id;   /* Channel Id for Tx operation..ignore it for div device */
}cxm_channel_out_type;


typedef struct 
{
  cxm_tech_type  chain[TRM_MAX_CHAINS];
}cxm_chain_owner_info_type;

/*! @brief structure for chain owner message     */
typedef struct
{
  msgr_hdr_struct_type        msg_hdr; /*!< msgr header contianing msg_id */ 
  cxm_chain_owner_info_type   owner;
}cxm_chain_owner_msg_s;

/*! @brief structure for UL/DL status for a single tech  */
typedef struct
{
  boolean                     is_ul_active; /*!< Has UL info been reported by tech */
  boolean                     is_dl_active; /*!< Has DL info been reported by tech */
}cxm_conn_tbl_entry;

/*! @brief structure for UL/DL status for all techs 
     this structure will be returned from API call     */
typedef struct
{
  cxm_conn_tbl_entry    conn_state[CXM_TECH_MAX]; /*! Array of all techs */
}cxm_conn_tbl_type;

/*! @brief structure for serving frequency indication message (Band Avoidance) */
typedef struct{
  msgr_hdr_struct_type      msg_hdr;
  coex_ba_freq_data         serv_freq_data;
}cxm_coex_ba_freq_ind_s;

/*! @brief structure for serving power indication message (Band Avoidance) */
typedef struct{
  msgr_hdr_struct_type      msg_hdr;
  coex_ba_pwr_data          serv_pwr_data;
}cxm_coex_ba_pwr_ind_s;

/*! @brief structure for Blacklist response message from L1 (Band Avoidance) */
typedef struct{
  msgr_hdr_struct_type       msg_hdr;
  coex_ba_blist_res_data     blist_res_data;
}cxm_coex_ba_blist_res_s;

/*! @brief structure for Blacklist info message to L1 (Band Avoidance) */
typedef struct{
  msgr_hdr_struct_type        msg_hdr;
  coex_ba_blist_data          blacklist_data;
}cxm_coex_ba_ind_s; 

/*! @brief structure for requesting desense indication from MCS */
typedef struct{
  msgr_hdr_struct_type        msg_hdr;
  coex_desense_ind_reg_s      desense_reg;
}cxm_coex_desense_ind_req_s; 


/*! @brief structure for returning desense indication to clients */
typedef struct{
  msgr_hdr_struct_type        msg_hdr;
  coex_desense_ind_s          desense_ind;
}cxm_coex_desense_ind_s; 

/*! @brief structure for obtaining distress indication from techs */
typedef struct 
{
    msgr_hdr_struct_type          msg_hdr;
    cxm_tech_type                 tech_id;
    cxm_distress_lvl_type         distress_lvl;
} cxm_tech_distress_ind_s;

/*! @brief structure for returning active tech coex state broadcast indication from MCS to clients */
typedef struct{
  msgr_hdr_struct_type       msg_hdr;
  
  /*! Number of active techs */
  uint8                      num_active_techs;

  /*! State of active techs in order of macro priority */
  cxm_coex_tech_data         tech_state[CXM_MAX_CONCURRENT_TECHS];
}cxm_coex_tech_state_bcast_ind_s;
/*=============================================================================

                        MSGR UMID Definitions

=============================================================================*/

/*! @brief These indications are used for WWAN Coexistence mitigation
 */
enum 
{
  /*! Tech Power Indication ( Tech L1 to MCS ) for updating Tx/Rx power */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_POWER,
                    COEX_POWER_ID, cxm_coex_power_ind_s ),  

  /*! Tech Tx activity indication (GSM L1 to MCS) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, SET_ACTIVITY_TIMELINE,
                    SET_ACTIVITY_TL_ID, cxm_activity_timeline_s),

  /*! Tech interested in inactivity information can register/deregister 
    using this. (1x/HDR L1 to MCS)*/
  MSGR_DEFINE_UMID( MCS, CXM, IND, INACTIVITY_CLIENT_REGISTRATION,
                    INACTIVITY_CLIENT_REG_ID, cxm_inactivity_client_reg_s),

  /*! Tech Tx/Rx slot level activity indication (L1 to MCS) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, SET_SLOT_ACTIVITY_TL,
                    SET_SLOT_ACT_TL_ID, cxm_timing_info_s),
                    
  /*! Tech Tx/Rx slot level activity indication (MCS to TDS) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, SET_SLOT_ACTIVITY_TL_TDS_REC,
                    SET_SLOT_ACT_TL_ID_TDS_REC, cxm_timing_info_s),

  /*! Tech Tx/Rx slot level activity indication (MCS to W) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, SET_SLOT_ACTIVITY_TL_WCDMA_REC,
                    SET_SLOT_ACT_TL_ID_WCDMA_REC, cxm_timing_info_s),
                    
  /*! Tech Tx/Rx slot level activity indication (MCS to LTE) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, SET_SLOT_ACTIVITY_TL_LTE_REC,
                    SET_SLOT_ACT_TL_ID_LTE_REC, cxm_timing_info_s),
                   

  /* FreqId Indications sent by MCS to various Tech L1s */

  /*! GSM1 FreqId Indication ( MCS to GSM L1 ) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, FREQID_LIST_GSM1,
                    FREQID_LIST_GSM1_ID, cxm_freqid_info_ind_s ),

  /*! GSM2 FreqId Indication ( MCS to GSM L1 ) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, FREQID_LIST_GSM2,
                    FREQID_LIST_GSM2_ID, cxm_freqid_info_ind_s ),

  /*! WCDMA FreqId Indication ( MCS to WCDMA L1 ) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, FREQID_LIST_WCDMA,
                    FREQID_LIST_WCDMA_ID, cxm_freqid_info_ind_s ),

  /*! 1X FreqId Indication ( MCS to 1x L1 ) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, FREQID_LIST_ONEX,
                    FREQID_LIST_ONEX_ID, cxm_freqid_info_ind_s ),

  /*! HDR FreqId Indication ( MCS to HDR L1 ) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, FREQID_LIST_HDR,
                    FREQID_LIST_HDR_ID, cxm_freqid_info_ind_s ),

  /*! TDSCDMA FreqId Indication ( MCS to TDS L1 ) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, FREQID_LIST_TDSCDMA,
                    FREQID_LIST_TDSCDMA_ID, cxm_freqid_info_ind_s ),

  /*! LTE FreqId Indication ( MCS to LTE L1 ) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, FREQID_LIST_LTE,
                    FREQID_LIST_LTE_ID, cxm_freqid_info_ind_s ),

  /*! GSM3 FreqId Indication ( MCS to GSM L1 ) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, FREQID_LIST_GSM3,
                    FREQID_LIST_GSM3_ID, cxm_freqid_info_ind_s ),

  /* Messages to register for and publish GSM Tx inactivity */

  /*! GSM Tx Inactivity information (MCS to 1X L1) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, REPORT_INACTIVITY_INFO_ONEX,
                    INACTIVITY_REPORT_ONEX_ID, cxm_inactivity_report_s),

  /*! GSM Tx Inactivity information (MCS to HDR L1) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, REPORT_INACTIVITY_INFO_HDR,
                    INACTIVITY_REPORT_HDR_ID, cxm_inactivity_report_s),

  /*! Register/deregister for GSM1 Tx Activity information (MCS to GSM L1) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, REQUEST_ACTIVITY_INFO_GSM1,
                    REQUEST_ACTIVITY_INFO_GSM1_ID, cxm_request_activity_info_s),

  /*! Register/deregister for GSM2 Tx Activity information (MCS to GSM L1) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, REQUEST_ACTIVITY_INFO_GSM2,
                    REQUEST_ACTIVITY_INFO_GSM2_ID, cxm_request_activity_info_s),

  /*! Register/deregister for GSM3 Tx Activity information (MCS to GSM L1) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, REQUEST_ACTIVITY_INFO_GSM3,
                    REQUEST_ACTIVITY_INFO_GSM3_ID, cxm_request_activity_info_s),

  /*! WWAN Coex state update indication (MCS to tech L1) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, WWCOEX_STATE_UPDATE,
                    WWCOEX_STATE_UPDATE_ID, cxm_wwcoex_state_update_s),

  /*! @brief It is used for SAR DSI value manipulation via diag command */
  MSGR_DEFINE_UMID( MCS, CXM, IND, SAR_SET_DSI,
                    SAR_SET_DSI_ID, cxm_sar_set_dsi_s ),

  /*! @brief This is used for TRM chain owner update */
  MSGR_DEFINE_UMID( MCS, CXM, IND, CHAIN_OWNER_UPDATE,
                    CHAIN_OWNER_UPDATE_ID, cxm_chain_owner_msg_s ),

  /*! Band Avoidance serving frequency information (Tech L1 to MCS) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_FREQ,
                    BAND_AVOID_FREQ_ID, cxm_coex_ba_freq_ind_s ),

  /*! Band Avoidance serving power information (Tech L1 to MCS) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_PWR,
                    BAND_AVOID_PWR_ID, cxm_coex_ba_pwr_ind_s ),

  /*! Band Avoidance Blacklist response (Tech L1 to MCS) */
  MSGR_DEFINE_UMID( MCS, CXM, RSP, BAND_AVOID_BLIST,
                    BAND_AVOID_BLIST_RSP_ID, cxm_coex_ba_blist_res_s ),

  /*! Band Avoidance Blacklist indication (MCS to GSM1) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_BLIST_GSM1,
                    BAND_AVOID_BLIST_IND_GSM1_ID, cxm_coex_ba_ind_s ),

  /*! Band Avoidance Blacklist indication (MCS to GSM2) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_BLIST_GSM2,
                    BAND_AVOID_BLIST_IND_GSM2_ID, cxm_coex_ba_ind_s ),

  /*! Band Avoidance Blacklist indication (MCS to GSM3) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_BLIST_GSM3,
                    BAND_AVOID_BLIST_IND_GSM3_ID, cxm_coex_ba_ind_s ),

  /*! Band Avoidance Blacklist indication (MCS to LTE) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_BLIST_LTE,
                    BAND_AVOID_BLIST_IND_LTE_ID, cxm_coex_ba_ind_s ),

  /*! Band Avoidance Blacklist indication (MCS to ONEX) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_BLIST_ONEX,
                    BAND_AVOID_BLIST_IND_ONEX_ID, cxm_coex_ba_ind_s ),

  /*! Band Avoidance Blacklist indication (MCS to HDR) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_BLIST_HDR,
                    BAND_AVOID_BLIST_IND_HDR_ID, cxm_coex_ba_ind_s ),

  /*! Band Avoidance Blacklist indication (MCS to TDSCDMA) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_BLIST_TDSCDMA,
                    BAND_AVOID_BLIST_IND_TDS_ID, cxm_coex_ba_ind_s ),

  /*! Band Avoidance Blacklist indication (MCS to WCDMA) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, BAND_AVOID_BLIST_WCDMA,
                    BAND_AVOID_BLIST_IND_WCDMA_ID, cxm_coex_ba_ind_s ),

  /*! Desense indication request (Data Services to MCS) */
  MSGR_DEFINE_UMID( MCS, CXM, REQ, DESENSE_IND,
                    DESENSE_IND_REQ_ID, cxm_coex_desense_ind_req_s ),

  /*! Tech call in distress indication (Tech L1 to MCS) */
  MSGR_DEFINE_UMID( MCS, CXM, IND, TECH_DISTRESS,
                    TECH_DISTRESS_ID, cxm_tech_distress_ind_s ),

  /*! Tech Coex State Broadcast Indication (MCS to Tech L1,RRC,etc) 
      Clients expected to deregister this message when its not interested */
  MSGR_DEFINE_UMID( MCS, CXM, IND, COEX_TECH_STATE_BCAST,
                    COEX_TECH_STATE_BCAST_ID, cxm_coex_tech_state_bcast_ind_s)
};


/*=============================================================================

                        Function Declarations

=============================================================================*/


/*============================================================================

CALLBACK CXM_PRIO_TABLE_CB_T

DESCRIPTION
  Callback that techs can register at bootup to get its complete list of
  priorities. This callback will be called whenever there is a change in
  priorities due to user input like voice call put on hold or priority of
  subscription is changed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
typedef void (*cxm_prio_table_cb_t)
(
  cxm_tech_type  tech_id,
  cxm_activity_table_s activity_tbl[MAX_ACTIVITY_TIERS]
);

/*============================================================================

FUNCTION CXM_REGISTER_PRIO_TABLE_CB

DESCRIPTION
  Function used by tech L1 to register its priority table callback.
  To deregister, NULL can be passed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
boolean cxm_register_prio_table_cb
(
  cxm_tech_type       tech_id,
  cxm_prio_table_cb_t  prio_cb
);

/*============================================================================

FUNCTION CXM_GET_CHANNEL_ID

DESCRIPTION
  Function used by tech L1 to get channel ID corresponding to RF device.
  
DEPENDENCIES
  None

RETURN VALUE
  Channel ID for both Rx and Tx chain for RF device 

SIDE EFFECTS
  None

============================================================================*/
cxm_channel_out_type cxm_get_channel_id
(
  cxm_channel_in_type input
);

/*============================================================================

FUNCTION CXM_GET_CONN_STATE_TBL

DESCRIPTION
  Function used by tech L1 to retrieve the UL/DL status of all techs
  
DEPENDENCIES
  None

RETURN VALUE 
  Table indicating UL/DL status of each tech 

SIDE EFFECTS
  This function is not mutex protected so results may not reflect
  the latest status

============================================================================*/
cxm_conn_tbl_type cxm_get_conn_state_tbl(void);

/*============================================================================

FUNCTION CXM_GET_COMBINED_PRIORITY

DESCRIPTION
  Function used by tech L1 to combine Tx and Rx priority. This is typically
  required for FDD techs in single Tx DSDA to ensure Rx and Tx priority are 
  equal and it doesnt cause double blanking.
  So in single Tx DSDA, this api returns a MAX priority in both Tx and Rx prio
  pointers. In other scenarios, it will not change the priority values.
  The api also provides option to enter arguments as TIERs or priority values.
  Please refer to enum cxm_combine_prio_input_type...
       
DEPENDENCIES
  None

RETURN VALUE
  FALSE if NULL pointers are passed as arguments
  TRUE otherwise with the priority/tier value being updated in the pointer 
  memory locations

SIDE EFFECTS
  None

============================================================================*/  
boolean cxm_get_combined_priority
(
  cxm_combine_prio_input_type  input
);

#endif /* __WWAN_COEX_MGR_H__ */
