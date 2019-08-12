#ifndef __LMTSMGR_I_H__
#define __LMTSMGR_I_H__
/*!
  @file lmtsmgr_i.h

  @brief
   Internal APIs/structs used by LMTSMGR module only

*/

/*=============================================================================

 Copyright (c) 2012-15 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/core/inc/lmtsmgr_i.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/06/15   rj      Move Translation functions in Limitsmgr to a seperate file
11/25/14   ag      Store macro_state and sub id per tech
11/05/14   rj      Cleanup DDR related processing from Core file
11/04/14   ag      Added TTL markers for profiling 
10/27/14   rj      Cleanup SAR related processing
10/10/14   ag      Added support for coex tech state indication
10/10/14   ag      Added support for call in distress indication
10/08/14   jm      FreqId generation support in sim mode
09/09/14   rj      Merged non-IM BackOff and Blanking EFS
08/25/14   jm      Circular buffer for slot level activity tl
08/12/14   jm      Slot level timeline support
08/13/14   rj      Disable SVLTE+G Idle Mode Hopping
08/04/14   jm      channel id support for Single-TX DSDA
07/30/14   sg      Added check for DR-DSDS mode
07/22/14   rj      Cleanup FilterPath related BOLT feature in Lmtsmgr
07/03/14   jm      Support for desense calculation in DR-DSDS
06/23/14   sg      desense indication mechanism
05/04/14   rj      Filter Path support to send GSM Tx specific flag info to RF
04/28/14   jm      L+G Band Avoidance support
04/07/14   ag      Simultaneous capacity in WWAN coexistence is set to FALSE when 
                   not in SGLTE mode or when roaming mode is not set
03/17/14   jm      WWAN coex based off of RF device assignment
02/05/14   jm      Slot level backoff support for SGLTE/SGTDS
01/14/14   jm      Ensure freqID is 16-bit value
12/12/13   jm      GPS support for DDR coex
12/09/13   jm      DDR management phase 2 support (diversity power)
11/21/13   jm      Adding DDR management support
11/17/13   ag      Fix for HDR-GSM IM check
11/16/13   rj      Adding macro prio callback support
09/19/13   rj      Adding support for GSM to ASID mapping
04/23/13   rj      Fix for sending pwr limits to GSM and WCDMA
04/06/13   ag      New interfaces for WWAN Coexistence 
03/07/13   rj      Handling new MSGR for LTE BW, Rx freq and sys band to RF band mapping
02/20/13   rj      Fix CW issues
04/22/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "wwan_coex_mgr.h" 
#include "sar_efs.h"
#include "subs_prio.h"
#include "rfm_mode_types.h"

#include "ttl_map.h"
/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/
/* Default limit used as MAX Power limit */
#define DEFAULT_MAX_PLIMIT       32000

/* Default limit used when a technology stops transmitting */
#define DEFAULT_TXSTOP_LIMIT     30000

/* 10*100logx is done to provide 0.5 resolution and round-off to nearest integer */
/* Approx value for 1000log2 = 1000*0.3010 ~ 301 */
#define LMTSMGR_1000_LOG_2_VALUE     301

/* Approx value for 1000log3 = 1000*0.4771 ~ 477 */
#define LMTSMGR_1000_LOG_3_VALUE     477

/* Macro to find nearest integer round-off */
#define LMTSMGR_NEAREST_INT_ROUND_OFF(value)     ((int16)(((value)+5)/10))

/*! Number of GSM priority sets */
#define LMTSMGR_NUM_GSM_PRIO_SETS  8

#define LMTSMGR_NUM_DESENSE_CLIENTS 5

/* MACROS for using rex based critical sections on Q6 platforms */

/* Atomic section macros for initialization of critical section */
#define LMTSMGR_ENTER_ATOMIC_SECT()     REX_DISABLE_PREMPTION()
#define LMTSMGR_LEAVE_ATOMIC_SECT()     REX_ENABLE_PREMPTION()

/* Critical section macros */
#define LMTSMGR_INIT_CRIT_SECT(lock)    rex_init_crit_sect(&(lock))
#define LMTSMGR_ENTER_CRIT_SECT(lock)   rex_enter_crit_sect(&(lock))
#define LMTSMGR_LEAVE_CRIT_SECT(lock)   rex_leave_crit_sect(&(lock))

/* macro to prevent compiler warnings when the payload parameter is not used
   by state entry, exit, and transition functions, or when a variable is only
   used in MSG_XXXX calls that may be compiled out */
#define LMTSMGR_UNUSED(i) if(i){}

/*! Maximum number of freqIds allowed per tech  ~ (CXM_UNKNOWN_FREQID-1)/cxm_tech_type */
#define LMTSMGR_MAX_FREQIDS_PER_TECH    (uint32)8189

/* Bit position for a technology */
#define COEX_TECH_BIT(tech)   (1 << (uint8)(tech))

#define IS_C2K_TECH(tech)   (((tech)==CXM_TECH_ONEX)||((tech)==CXM_TECH_HDR))

/* gets the other C2K tech... should only be called if IS_C2K_TECH() is TRUE */
#define GET_OTHER_C2K_TECH(tech) (((tech)==CXM_TECH_ONEX) ? CXM_TECH_HDR : CXM_TECH_ONEX)

#define IS_GSM_TECH(tech) (((tech)==CXM_TECH_GSM1)||((tech)==CXM_TECH_GSM2)||((tech)==CXM_TECH_GSM3))

#define IS_IN_CHANNEL_RANGE(val, min_ch, max_ch) ((val>=min_ch) && (val<=max_ch))

#define LMTSMGR_IS_TECH_VALID(tech) ((tech > CXM_TECH_DFLT_INVLD) && (tech < CXM_TECH_MAX))

#define LMTSMGR_IS_TECH_ACTIVE(tech) (lmtsmgr.tech_state[tech].currList.num_entries > 0)

#define LMTSMGR_IS_TECH_UL_ACTIVE(tech) (lmtsmgr.tech_state[tech].currList.num_ul_entries > 0)

/* GSM Rx Power Limit to enable Idle Mode Hopping */
#define LMTSMGR_GSM_RXPOWER_HOPPING_LIMIT (-10000)

/* Checks if the multimode ASID has SGLTE feature mode enabled in policy manager */
#define LMTSMGR_IS_SGLTE_FEATURE_MODE() \
( lmtsmgr.subs_prio_list.active_tech_info.subs_feature_mode[lmtsmgr.subs_prio_list.tech_asid_map.multimode_as_id] == SYS_SUBS_FEATURE_MODE_SGLTE)

/* Checks if the multimode ASID has SVLTE feature mode enabled in policy manager */
#define LMTSMGR_IS_SVLTE_FEATURE_MODE() \
( lmtsmgr.subs_prio_list.active_tech_info.subs_feature_mode[lmtsmgr.subs_prio_list.tech_asid_map.multimode_as_id] == SYS_SUBS_FEATURE_MODE_SVLTE)

/* Checks if Single-TX DSDA mode is enabled in TRM */
#define LMTSMGR_IS_SINGLE_TX_DSDA_ENABLED() \
( lmtsmgr.trm_single_tx_dsda == TRM_SINGLE_TX_DSDA_ENABLED )

/* Checks if the device is in DR-DSDS mode */
#define LMTSMGR_IS_DRDSDS_ENABLED() \
( (lmtsmgr.trm_simul_cap & TRM_DR_DSDS_IS_ENABLED) == TRM_DR_DSDS_IS_ENABLED)

/* Number of history records to keep in slot level activity buffer */
#define LMTSMGR_TIMING_HIST_RECS 4

/* Maximum number of entires possible in a single priority tier */
#define LMTSMGR_MAX_TIER_TECHS  1

/* Max concurrent UL techs is 2 */
#define LMTSMGR_MAX_CONCURRENT_UL_TECHS 2

#define LMTSMGR_COMPILE_ASSERT(a)  switch(0){case 0: case (a):;}

/*=============================================================================

                             TYPEDEFS

=============================================================================*/
/* Enum of special indications that need to be sent to some client */
typedef enum
{
  FILTERED_PATH,
  FILTERED_PATH_GSM_TX_ONLY
} lmtsmgr_special_inds_type;

/* Enum of special indications that need to be sent to some client */
typedef enum
{
  MILD_DESENSE,
  SEVERE_DESENSE
} lmtsmgr_desense_status_e;

typedef enum 
{
  LMTSMGR_TIER_1,     /* Highest priority */
  LMTSMGR_TIER_2,     /* Second highest priority */
  LMTSMGR_TIER_MAX    /* Max or invalid */
}lmtsmgr_priority_type;

typedef struct {
  /* Technology identifier */
  cxm_tech_type           tech;
  /* Which RF Device is used for Tx? */
  cxm_rfdev_type          tx_rfdev;
  /* Tx'ing on which band? */
  uint32                  band;
  /* Priority of this transmitter */
  lmtsmgr_priority_type       pri;
} lmtsmgr_core_prio_table_type;

typedef struct
{
  /* SAR Tx power limit */
  int16                   sar_plimit[SAR_GSM_MAX_LIMIT_TYPE];
  /* Number of valid SAR power limits */
  uint8 num_valid_sar;
  /* Coex Tx power limit */
  int16                   coex_plimit;
  /* Battery current Tx power limit */
  int16                   batt_plimit;
  /* Slot Level Coex Tx power limit */
  int16                   coex_slot_plimit;
} lmtsmgr_pwr_limit_type;


typedef struct
{
  /* SAR Tx power limit */
  int16                   sar_plimit[SAR_GSM_MAX_LIMIT_TYPE];
  /* Number of valid SAR power limits */
  uint8 num_valid_sar;
  /* Coex Tx power limit wrt second active tech*/
  int16                   coex_plimit[CXM_TECH_MAX];
  /* Battery current Tx power limit */
  int16                   batt_plimit;
  /* Slot Level Coex Tx power limit wrt second active tech */
  int16                   coex_slot_plimit[CXM_TECH_MAX];

}lmtsmgr_new_pwr_limit_type;


typedef struct
{
  /* Is Tx power valid? */
  boolean                 tx_pwr_valid;
  /* Time averaged Tx power in dBm*10 */
  int16                   tx_power;
  /* Limits that are currently being set for this tech */
  lmtsmgr_pwr_limit_type      curr_limits;
  /* New limits that are being processed for this tech */
  lmtsmgr_new_pwr_limit_type  new_limits;

}lmtsmgr_power_info_type;

typedef struct 
{
  coex_freqid_link_s      freqInfo;
  coex_power_link_s       powerInfo;
} lmtsmgr_tech_link_type;

typedef enum
{
  LMTSMGR_TECH_INACTIVE,
  LMTSMGR_TECH_RX_ACTIVE,
  LMTSMGR_TECH_RXTX_ACTIVE
} lmtsmgr_tech_rxtx_state_e;

typedef enum
{
  LMTSMGR_DDR_RX_THRSHD_NEGATIVE = -1,
  /* Neutral is required when the Power is received 1st time */
  LMTSMGR_DDR_RX_THRSHD_NEUTRAL,
  LMTSMGR_DDR_RX_THRSHD_POSITIVE
} lmtsmgr_rx_pwr_thrshd_state_e;

typedef struct
{
  lmtsmgr_tech_link_type   links[CXM_MAX_SUPPORTED_LINK_SETS];
  uint32       num_entries;
  uint32       freqId_offset; 

  uint32       dl_freqList[CXM_MAX_SUPPORTED_LINK_SETS];
  uint32       num_dl_entries;
  uint32       ul_freqList[CXM_MAX_SUPPORTED_LINK_SETS];
  uint32       num_ul_entries;

  int32        xo_error;
  uint16       xo_uncertainty;
  
  uint32       num_serving_dl_entries;
}lmtsmgr_tech_list_type;

typedef struct 
{
  lmtsmgr_power_info_type  pwr_info;

  /* For WWAN Coexistence Management */

  /*! Current list of active frequencies */
  lmtsmgr_tech_list_type   currList;
 
  /*! Allowed range of freqIds for the tech */
  uint32      freqId_range_start;
  uint32      freqId_range_end;

  /*! frequency change flag... */
  boolean     freq_change;

  /*! power change flag... */
  boolean     power_change;

  /*! Distress state */
  cxm_distress_lvl_type   distress_state;

  /*! Macro Priority state */
  sp_subs_states_enum_t   macro_state;
  
  /*! Subscription ID */
  sys_modem_as_id_e_type  sub_id;

#ifdef FEATURE_MODEM_DDR_MGMT
  /*! Is diversity tech currently holding chain */
  boolean     div_hold_cur;

  /*! Was diversity tech holding chain */
  boolean     div_hold_prev;

  /* Reason for which client is holding chain */
  trm_reason_enum_t reason;
  /* Band class used */
  trm_band_type band;
  boolean band_changed;

  /* Channel used */
  uint32        channel;
  boolean       channel_changed;

  /* LTE CA */
  uint32        lte_ca_channel;
  boolean       lte_ca_channel_changed;

  uint8         priority;
  uint64        pwr_thrshd_switchover_time;
  uint64        curr_pwr_update_time;
  /* Power Threshold is crossed for duration */
  lmtsmgr_rx_pwr_thrshd_state_e       thrshd_switchover_sign;
#endif
} lmtsmgr_tech_state_type;

typedef struct
{
  uint32 frequency;                  /*!< Tech operating frequency in KHz */
  uint32 bandwidth;                  /*!< Tech operating bandwidth in Hz */
  uint32 band;                       /*!< Tech operating band */
  uint32 channel;                    /*!< Tech operating channel */
  int16  power;                      /*!< 10*dBm */
}lmtsmgr_ba_serv_info_type;

typedef struct
{
  uint16             bl_id;
  wwcoex_fa_res_type response;
}lmtsmgr_ba_blist_rsp_type;

typedef struct
{
  uint32                        num_entries;
  uint32                        num_ul_entries;
  uint32                        num_dl_entries;
  lmtsmgr_ba_serv_info_type     serv_info_ul_list[CXM_MAX_SUPP_FREQ_LINK_BA];
  lmtsmgr_ba_serv_info_type     serv_info_dl_list[CXM_MAX_SUPP_FREQ_LINK_BA];
  boolean                       ba_rsp_changed;
  boolean                       freq_changed;
  boolean                       power_changed;
  lmtsmgr_ba_blist_rsp_type     blist_rsp;
  wwan_coex_desense_mode        serv_desense_mode;
}lmtsmgr_tech_ba_type;

typedef struct
{
  lmtsmgr_tech_ba_type   tech_info[CXM_TECH_MAX];
  coex_ba_blist_data     currBlacklist;
  cxm_tech_type          currBlTech;
}lmtsmgr_ba_info_type;

typedef struct
{
  msgr_umid_type  umid;
}lmtsmgr_desense_ind_client_type;

typedef struct
{
  uint32 count;
  lmtsmgr_desense_ind_client_type client[LMTSMGR_NUM_DESENSE_CLIENTS];
}lmtsmgr_desense_ind_type;


typedef struct
{
  lmtsmgr_desense_status_e  status;
  uint32     cxm_report_count;   /* count of total report in the timer period      */
  uint32     cxm_entries_count;  /* total count of frequency list reported         */
  uint32     cxm_desense_count;  /* total count of frequency list that have issues */
  uint32     delta_t_msec;       /* Expected time between power updates in traffic */
  uint32     mild_coex_thresh;   /* threhsold used when coex is mild               */
  uint32     severe_coex_thresh; /* threshold used when coex is severe             */
  uint64     mild_coex_time;     /* time used when coex is mild                    */
  uint64     severe_coex_time;   /* time used when coex is severe                  */
  timer_type desense_timer;      /* timer used for desense detection               */
}lmgtsmgr_desense_stats;

typedef enum
{
  LMTSMGR_UNINITIALIZED,
  LMTSMGR_INITIALIZING,
  LMTSMGR_INITIALIZED
} lmtsmgr_task_state_type;

typedef enum
{
  /*! No criteria is met */
  PRIO_CRITERIA_NONE,
  
  /*! Criteria for Data + Data_high is satisfied */
  PRIO_CRITERIA_DATA_DATA_HIGH,
  
  /*! Criteria for Voice Call in distress + Data call is satisfied */ 
  PRIO_CRITERIA_VOICE_DISTRESS_DATA,
  
  /*! Criteria for Voice Call in distress + Data_High Call is satisfied */
  PRIO_CRITERIA_VOICE_DISTRESS_DATA_HIGH
}lmtsmgr_prio_criteria_type;

typedef struct
{
  /* Is task initialized */
  lmtsmgr_task_state_type   task_status;

  /* Tech state information */
  lmtsmgr_tech_state_type      tech_state[CXM_TECH_MAX];

#ifdef FEATURE_MODEM_DDR_MGMT
  /* GPS state information */
  boolean                   gps_tech_state_on;
#endif

  /* Critical section */
  rex_crit_sect_type        crit_sect;

  /* Subscription Priority list */
  sp_subscription_priority_list  subs_prio_list;

  /* Subscription capability tech mapping as per multimode subs */
  cxm_tech_type          gsm_id_list[MAX_NUM_OF_SUBSCRIPTIONS];

  /* CXM Tech to TRM Chain Mapping */
  cxm_tech_type          chain_owner[TRM_MAX_CHAINS];

  /* Coex variables */

  /*! current coex state */
  cxm_wwcoex_state_info_s  state_info;

  /*! mitigation mask (Disable) 
       Bit 1 - WWAN Coex Mitigation DISABLED
       Bit 2 - DDR Coex Mitigation DISABLED   */
  uint32 mitigation_mask;

  /*! mitigation mask (Enable) 
       Bit 3 - Band Avoidance Coex Mitigation ENABLED */
  uint32 en_mitigation_mask;

  /*! device mode */
  uint32 device_mode;

  /* Simul capacity provided by TRM */
  trm_simul_cap_return_data  trm_simul_cap;

  /*! Single-TX DSDA mode enbabled -- provided by TRM */
  trm_single_tx_dsda_enum_type trm_single_tx_dsda;

  /*! current priority offset should be < LMTSMGR_NUM_GSM_PRIO_SETS*/
  uint8 curr_prio_offset;

  /*! for special indications like using RF filtered path */
  uint32 special_ind_mask;

#ifdef FEATURE_MODEM_DDR_MGMT
  /* client order list */
  cxm_tech_type   order[CXM_TECH_MAX];
  uint8           num_order;
#endif

  lmtsmgr_ba_info_type   band_avoidance;

  lmtsmgr_desense_ind_type desense_ind;

  lmgtsmgr_desense_stats   desense_stats;

  int16 idle_mode_hopping_rxpwr_lmt;
  /* This is needed to keep track of Hopping inputs to TRM */
  trm_modify_hop_behavior_data hop_input;
  /*! Slot Level Activity & DTx Timing Info */
  cxm_timing_info_s       tech_timing_info[LMTSMGR_TIMING_HIST_RECS];

  uint8                   tech_timing_info_index;

  //lmtsmgr_tech_filter_info_type* filter_data;
  //uint32                         num_fp_data;   /*!< Number of filter paths */
} lmtsmgr_struct;

/* Global LMTSMGR state variables */
extern lmtsmgr_struct lmtsmgr;

typedef struct
{
  boolean  filtered_path_flag;
  boolean  gsm_aggr_filter_path_flag; /* This flag is for GSM Tx filter path i.e GSM as aggressor */
}lmtsmgr_filter_path_type;

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  lmtsmgr_process_txstate_change

=============================================================================*/
/*!
    @brief
    Processes any change in the Tx on/off state for a tech
 
    @return
    None
*/
/*===========================================================================*/

void lmtsmgr_process_txstate_change
(
  cxm_tech_type   tech_id,
  uint32          prev_num_ul,
  uint32          new_num_ul
);

/*=============================================================================

  FUNCTION:  lmtsmgr_notify_tech_plimit

=============================================================================*/
/*!
    @brief
    Notify a tech about the new power limits
 
    @return
    None
*/
/*===========================================================================*/
extern void lmtsmgr_notify_tech_plimit
(
  cxm_tech_type  tech
);

/*=============================================================================

  FUNCTION:  lmtsmgr_init

=============================================================================*/
/*!
    @brief
    Initializes LMTSMGR and its global variables
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_init(void);


void lmtsmgr_subs_prio_callback(sp_subscription_priority_list  subs_prio_list, 
                                                   sp_client_enum_t client_id);


void lmtsmgr_sp_callback_process_input(void);



/*=============================================================================

  FUNCTION:  lmtsmgr_process_coex_input

=============================================================================*/
/*!
    @brief
    Processes any change in the coex input variables
 
    @return
    None
*/
/*===========================================================================*/
extern void lmtsmgr_process_coex_input(void);

/*=============================================================================

  FUNCTION:  lmtsmgr_send_freqId_list

=============================================================================*/
/*!
    @brief
    Send freqId list to tech
 
    @return
    None
*/
/*===========================================================================*/
extern void lmtsmgr_send_freqId_list
(
  cxm_tech_type  tech
);

/*=============================================================================

  FUNCTION:  lmtsmgr_update_wwcoex_state

=============================================================================*/
/*!
    @brief
    Update WWAN Coex state
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_update_wwcoex_state
(
  cxm_wwcoex_state_info_s new_state
);

/*=============================================================================

  FUNCTION:  lmtsmgr_send_activity_info_req

=============================================================================*/
/*!
    @brief
    Send request for activity timeline information
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_send_activity_info_req
(
   /*! Tech that we need information from */
  cxm_tech_type           tech_id, 

  /*! Type of activity - UL or DL */
  cxm_tech_link_direction  direction,

  /*! Start or Stop request */
  boolean                  reg_flag
);

/*=============================================================================

  FUNCTION:  lmtsmgr_reset_ddr_pwr_info

=============================================================================*/
/*!
    @brief
    Reset DDR power info 
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_reset_ddr_pwr_info (cxm_tech_type client);

/*=============================================================================

  FUNCTION:  lmtsmgr_trm_state_update_callback

=============================================================================*/
/*!
    @brief
    This is a callback funtion which is being used to get TRM callbacks 
 
    @return
    None
*/
/*===========================================================================*/

void lmtsmgr_trm_state_update_callback 
(
  trm_state_info_type trm_state_info
);

/*=============================================================================

  FUNCTION:  lmtsmgr_process_band_avoidance_input

=============================================================================*/
/*!
    @brief
    Processes any change in the band avoidance inputs (serving frequency,
    serving power, blacklist response)
 
    @details
    This should only be called when serving frequency, serving power,
    or a blacklist response has been sent from Tech L1
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_band_avoidance_input(void);

/*=============================================================================

  FUNCTION:  lmtsmgr_process_slot_lvl_act_input

=============================================================================*/
/*!
    @brief
    Determines the conflict type for each Tx/Rx activity slot reported and
    sends this information (along with the original Tx/Rx info) to other active
    tech
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_slot_lvl_act_input(boolean is_sim_input);


/*=============================================================================

  FUNCTION:  lmtsmgr_core_deinit

=============================================================================*/
/*!
    @brief
    De-Initializes LMTSMGR and its global variables
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_core_deinit(void);

/*=============================================================================

  FUNCTION:  wwcoex_is_gsm_channel_victim

=============================================================================*/
/*!
    @brief
    Checks the GSM ARFCN if it is in the victim channel range. This is primarily
    for SGLTE Coexistence in LTE/TDS B39 and GSM 1800
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
boolean wwcoex_is_gsm_channel_victim
(
   uint32 band,
   uint32 channel
);

/*=============================================================================

  FUNCTION:  wwcoex_is_gsm_channel_aggr

=============================================================================*/
/*!
    @brief
    Checks the GSM ARFCN if it is in the aggr channel range. This is primarily
    for SGLTE Coexistence in LTE/TDS B39/B34 and GSM 1800
 
    @return
    TRUE - Victim can be desensed
    FALSE - Victim is not desensed
*/
/*===========================================================================*/
boolean wwcoex_is_gsm_channel_aggr
(
   uint32 band,
   uint32 channel
);

/*=============================================================================

  FUNCTION:  lmtsmgr_send_filter_path_ind

=============================================================================*/
/*!
    @brief
    Send RF filter path indication
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_send_filter_path_ind
(
  lmtsmgr_filter_path_type send_ind
);

/*=============================================================================

  FUNCTION:  lmtsmgr_process_chain_owner_input

=============================================================================*/
/*!
    @brief
    Processes any change in the chain owner inputs
 
    @details
    This should only be called when chain owner is updated in TRM
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_process_chain_owner_input(void);

/*=============================================================================

  FUNCTION:  lmtsmgr_send_blacklist_info

=============================================================================*/
/*!
    @brief
    Send Blacklist information to appropriate tech for Band Avoidance
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_send_blacklist_info
(
  cxm_tech_type tech_id,
  boolean       send_blank_bl
);

/*=============================================================================

  FUNCTION:  lmtsmgr_update_hopping_behavior

=============================================================================*/
/*!
    @brief
    Checks Techs behavior and Calls TRM API to update 
    whether Idle Mode Hopping is to be enabled
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_update_hopping_behavior (void);

/*=============================================================================

  FUNCTION:  lmtsmgr_send_sl_activity_info

=============================================================================*/
/*!
    @brief
    Send Blacklist information to appropriate tech for Slot Level Activity
 
    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_send_sl_activity_info
(
  /*! Receiver of this timeline information */
  cxm_tech_type tech_id_rec,

  /*! Activity specifics of sending tech */
  cxm_timing_info_s* msg_ind
);

/*=======================================================================
FUNCTION LMTSMGR_UPDATE_CLIENT_PRIO_TABLE
    
DESCRIPTION
  This function will update client micro prio table of low macro clients 
  when any criteria is met. If criteria is not met, it will reset the prios
  to orginial state.

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/

void lmtsmgr_update_client_prio_table ( void );

/*=======================================================================
FUNCTION lmtsmgr_process_coex_tech_state_change
    
DESCRIPTION
  This function will go through all the clients and create a sorted list
(based on macro priority) of active clients along with their Rx/Tx band, 
macro state, distress state, etc

DEPENDENCIES
  None
    
RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void lmtsmgr_process_coex_tech_state_change( void );

/*=============================================================================
  
  FUNCTION:  lmtsmgr_core_get_priority

=============================================================================*/
/*!
    @brief
    Updates the priorities for the two simultaneously active techs
 
    @return
    None
*/
/*===========================================================================*/
boolean lmtsmgr_core_get_priority 
(
  lmtsmgr_core_prio_table_type *tech1_prio,
  lmtsmgr_core_prio_table_type *tech2_prio
);

#endif /* __LMTSMGR_I_H__ */
