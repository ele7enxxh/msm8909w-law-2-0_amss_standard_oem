/*!
  @file
  lte_mac_qos.h

  @brief
  This file contains all the QoS define structures/function prototypes 
  for sharing between MAC Control and MAC Up Link Task.


*/

/*===========================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly 
  reserved. By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_qos.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/16   al      CR942705: FR33651 Fast SR enhancement to counter no scheduling 
                   post CDRX in ZTE drops.
04/07/16   al      CR847294: FR 27570 Improved UE Procedure for CSFB page monitoring in L+G/1xSRLTE UE
10/12/15   el      CR 884865: FR 29101 Fast RLF Declaration for fast resync with eNB
09/03/14   el      CR687377: Move periodic logic from CTRL to UL task
08/03/14   mg      CR702710: QSH analysis and reset
07/07/14   el      CR 688341: SPS fix for no SR sending after reconnection
06/26/14   el      CR 639130: Delay SR if SFN is unknown and decide RACH with 
                   config identical checking
03/18/14   mg      CR631036: UL allowed throughput numbers calculated are 
                   exceeding the maximum defined rates in some intervals
03/05/14   mg      CR626402: Increased fb_circ_q size to 32
02/25/14   mg      CR580979: UE does not reset SPS implicit release counter at 
                   SPS reactivation
02/04/14   mg      CR607809: Increased the value of inactivity timer to 60000 
12/06/13   mg      Made the inacivity timer configurable 
11/27/13   mg      Allowed throughput API for Bolt
10/07/13   ta      CR 529514: DSDS feature. Support for RF Available/Unavailable
                   message
09/16/13   ta      CR 541914: Increased fb_circ_q size to 16, keep track of max
                   number of feedback buffers used
03/01/13   st      CR 458002: Increased fb_circ_q size to 8
02/15/13   st      CR 399602: Added VoLTE lcid handling under flow control conditions
01/29/13   st      CR 413398: HDA arrival SR trigger corner cases fix needed
                   some changes to the info between QoS and UL tasks
09/28/12   st      CR 371806: Added a new flag to maintain SPS state
05/01/12   st      CR 353599: Optimized SR logic
04/06/12   st      CR 333488: MAC enhancement for configurable variable TDD 
                   delay
04/03/12   bn      Enhance MAC stat and add logic to support for SPS release
01/03/12   st      CR #328377: QOS fix for same priority, infinite PBR LCIDs
12/16/11   ax      feature clean up FEATURE_LTE_MAC_FC_UL
11/04/11   st      Fixed a compilation error
10/27/11   bn      Enhance QoS multiple LCIDs of infinity PBR same priority
10/25/11   st      Made some Ping RTT related enhancements
10/07/11   bn      Replaced double buffer by circular queue for QoS feedback
07/13/11   bn      Added in MAC support for statistics
06/30/11   bn      Used the true QoS buffer size in RA group selection
06/16/11   bn      Moved the delay from SR to UL RACH 
12/10/10   bn      Enhance UL FC logic to selective build Control/SRB data
11/18/10   bn      Force padding when UL FC enters zero BSR state
08/11/10   bn      Added in define accumulated used size/PBR max ratio 
08/05/10   bn      Used the defined the FC robustness timer
06/11/10   bn      In FC, forced per-BSR to default value when no configure
06/02/10   bn      Added in logic to delay the SR request
03/31/10   bn      Added in support MAC FC for Up Link
03/18/10   bn      Updated SR logic based on 09/10 MAC SPEC SR enhanched CR
03/12/10   bn      Moved periodic flush timer for UL TB log to MAC log files
01/18/10   bn      Optimized the BSR timers logic and events
09/29/09   bn      Added in data structure to support QoS of same priority
09/24/09   bn      Corrected the QoS UL log logic and Optimized QoS logic
08/13/09   bn      Indicated to ML1 if MAC can't send PHR
08/04/09   bn      Added in support to cover a corner case in detecting HDA
07/26/09   bn      Fixed UL QoS log
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_QOS_H
#define LTE_MAC_QOS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <lte_as.h>
#include <lte_circ_q.h>
#include <lte_l2_common.h>
#include "lte_mac_common.h"
#include "lte_mac_rrc.h"
#include "lte_mac_rlc.h"
#include "lte_mac_diag.h"
#include "lte_mac_log.h"
#include "lte_mac_fc.h"
#include "qsh.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief defined QoS total priority level. According to RRC spec, 
    the priority level range from 1 to 16. Note that any reference to this
    is for handling the QoS case of equal priority LCIDs
*/
#define LTE_MAC_QOS_TOTAL_LC_PRIORITY_LEVEL     17

/*! @brief defined QoS LC delete priority is the lowest priority
*/
#define LTE_MAC_QOS_LC_DELETE_PRIORITY          255

/*! @brief defined UL RACH Invalid delay 
*/
#define LTE_MAC_QOS_UL_RACH_INVALID_DELAY       255

/*! @brief defined UL RACH delay in ms 
*/
#define LTE_MAC_QOS_UL_RACH_DELAY_DURATION      5

/*! @brief defined max accumulated use to PBR ratio. This is
  used for the case the PBR is zero (x / 0 = infinity)
*/
#define LTE_MAC_QOS_ACC_USE_TO_PBR_RATIO_MAX    0xFFFF0000

/*! @brief defined the range of LCID for SRB. 0 to 2
*/
#define LTE_MAC_QOS_SRB_LCID_RANGE              0x2

/*! @brief defined QoS feedback info circular buffer size
*/
#define LTE_MAC_QOS_FB_CIRCQ_BUF_SIZE           32

/*! @brief Define MAC CTRL traffic inactivity timer
*/
#define LTE_MAC_QOS_TRAFFIC_INACTIVITY_TIMER  60000

/*! @brief defined the alpha value for the allowed throughput to 50*/
#define LTE_MAC_QOS_DEFAULT_ALLOWED_TPUT_ALPHA               0x32

/*! @brief defined the time of each calculation cycle to 300 msec*/
#define LTE_MAC_QOS_ALLOWED_TPUT_TIMER               0x12C

/*! @brief Define default MAC TA interval to trigger SR
*/
#define LTE_MAC_QOS_TA_SR_TRIGGER_THRESHOLD_DEFAULT 200

/*! @brief defined bit mask to indicate SPS configuration is present
*/
#define LTE_MAC_QOS_SPS_CFG_BM             0x01

/*! @brief defined bit mask to indicate bsr configuration is present
*/
#define LTE_MAC_QOS_BSR_CFG_BM             0x02

/* define the BSR period for triggering fast robustness bsr */
#define LTE_MAC_QOS_FAST_ROBUSTNESS_BSR_PERIOD   20

/*! @brief Enumeration of QoS BSR event type
*/
typedef enum
{
  LTE_MAC_QOS_NO_EVENT_BSR,
  LTE_MAC_QOS_PERIODIC_BSR,
  LTE_MAC_QOS_HIGH_DATA_ARRIVAL_BSR,
  LTE_MAC_QOS_ROBUSTNESS_BSR,
  LTE_MAC_QOS_UL_BSR_BASED_FC
} lte_mac_qos_bsr_event_e;

/*! @brief Enumeration of SR pending type
*/
typedef enum
{
  LTE_MAC_QOS_SR_NOT_PENDING,
  LTE_MAC_QOS_SR_NO_CFG_PENDING,
  LTE_MAC_QOS_SR_WITH_CFG_PENDING
} lte_mac_qos_sr_pending_e;

/*! @brief Enumeration of special SR trigger 
*/
typedef enum
{
  LTE_MAC_QOS_SSR_NONE,
  LTE_MAC_QOS_SSR_INACTIVITY,
  LTE_MAC_QOS_SSR_RF_TUNE_BACK
} lte_mac_qos_special_sr_trigger_e;

/*! @brief Enumeration of QoS BSR triggered type
*/
typedef enum
{
  LTE_MAC_QOS_NO_BSR,
  LTE_MAC_QOS_CANCELLED_BSR,
  LTE_MAC_QOS_LONG_BSR,
  LTE_MAC_QOS_SHORT_BSR,
  LTE_MAC_QOS_PADDING_LONG_BSR,
  LTE_MAC_QOS_PADDING_SHORT_BSR,
  LTE_MAC_QOS_PADDING_TRUNCATED_BSR
} lte_mac_qos_bsr_triggered_e;

/*! @brief QoS Logical channel Preprocessing Data Structure */
typedef struct
{
  boolean       high_priority;     /*!< Flag to indicate no FC for this LCID */
  lte_lc_id_t   lcid;              /*!< Logical channel ID */
  uint32  max_suggested_allotment; /*!< max suggested allotment for this TTI,
                                   it should be the left over grant */
  uint32  min_suggested_allotment; /*!< min suggested allotment for this TTI,
                                   it should be the current token bucket size */
  boolean included_in_bsr_report;  /*!< flag to indicate if buffer size should be 
                                   included in BSR report*/
  lte_mac_bsr_lcgroup_e lcgroup;   /*!< which LC group it belongs to*/
  uint32       total_size;         /*!< total size in bytes for the logical channel */
} lte_mac_qos_prep_lc_data_s;

/*! @brief QoS Preprocessing Information Structure from 
MAC Control Task to MAC Up Link Task. As far as the BSR report and QoS 
Allotment, this is all the informations that MAC Up Link Task needs */
typedef struct
{
  uint8 total_act_lc;  /*!< Total number of active LC in lc_data array */
  uint8 last_none_zero_bs_index; /*!< last none zero buffer size index*/
  lte_mac_qos_prep_lc_data_s lc_data[LTE_MAX_LC];
  uint32  bsr_group_buf_size[LTE_MAC_BSR_LCGROUP_MAX];     /*!< BS reports*/
  uint8   lcgroup_priority_level[LTE_MAC_BSR_LCGROUP_MAX]; /*!< LCG priority level*/
  uint16  bsr_instance_number; /*!< indicate the current request BSR instance number*/
  lte_mac_qos_bsr_event_e  bsr_event; /*!< BSR Event */
  lte_mac_fc_ul_pkt_s    fc_ul;       /*!< Flow Control information for UL*/
  uint32 sr_send_cnt;  /*!< To convey number of times SR is requested to UL */
} lte_mac_qos_prep_info_s;

/*! @brief QoS Logical channel feedback Data Structure */
typedef struct
{
  lte_lc_id_t   lcid;         /*!< Logical channel ID */
  uint16        used_size;    /*!< Used size for this TTI */
  uint16        sp_used_size;    /*!< Used size for this TTI */
  uint16        min_alotment; /*!< Actual min allotment */
  uint16        max_alotment; /*!< Actual max allotment */
} lte_mac_qos_feedback_lc_data_s;

/*! @brief QoS same priority related info structure */
typedef struct
{
  /*!< total number of LC have the same priority */
  uint8       total_lc;        
  /*! Flag to indicate if the accumulated grant is overflow */
  boolean     acc_used_overflow;
} lte_mac_qos_same_prio_info_s;

/*! @brief QoS feedback Information Structure from 
MAC Up Link Task to MAC Control Task. This will help the MAC Control
to update it BSR logic and QoS/Token bucket logic */
typedef struct
{
  uint8 total_act_lc;  /*!< Total number of active LC in lc_data array */
  lte_mac_qos_feedback_lc_data_s lc_data[LTE_MAX_LC];
  lte_mac_qos_bsr_triggered_e bsr_triggered; /*!< indicate the last BSR triggered*/
  uint16  padding_byte;       /*!< padding byte */
  uint16  grant;              /*!< grant in byte */
  lte_l1_sfn_t sys_fn;        /*!< System Frame number */
  lte_l1_subfn_t  sub_fn;     /*!< Subframe number */
  lte_rnti_type_e  rnti_type; /*!< RNTI type */
  uint32      rlc_used_size;  /*!< total RLC used size */
  boolean send_phr_failed;    /*!< Don't have enough grant to include PHR */
  /*!UL FC is sending zero BSR now, stop reloading
  the rexted timer so that the timer will tick until
  it is expired for the next starting FC ON/OFF period*/
  boolean ul_fc_zero_bsr_enable;
  uint32 highest_nz_lcid;     /*!< Highest priority LCID which is not empty */
  uint32 sr_send_cnt_echo;    /*!< Echo of the SR req counter sent by QoS */
  boolean new_sps_activation; /*!< Whether it is new sps activation or not*/
  lte_mac_qos_bsr_event_e  bsr_event; /*!< BSR Event */
} lte_mac_qos_feedback_info_s;

/*! @brief Configuration using in QoS logic/processing */
typedef struct
{
  lte_mac_lcqos_cfg_s  lcqos[LTE_MAX_LC]; /*!< QoS info of all logical channel*/
  boolean  sr_resource_present; /*!< indicated if SR recourse is available or not*/
  uint8    ul_rach_delay_duration; /*! @brief defined UL RACH delay in ms */
  uint16   sr_periodicity;      /*!< Periodicity of triggering SR in ms*/
  uint16   retxed_bsr_timer;    /*!< retxed BSR periodicity timer in ms */
  uint32   cdrx_length;         /*!< cdrx_length in ms */
} lte_mac_qos_cfg_s;

/*! @brief internal QoS active LC info */
typedef struct
{
  lte_lc_id_t  lcid;
  uint8        priority;     /*!< Priority Level    */
  uint32       us_pbr_ratio; /*!< Total used size / PBR */
} lte_mac_qos_active_lc_info_s;

/*! VoLTE LCID cfg backup structure type */
typedef struct
{
  uint8        priority;  /*!< Priority Level */
  uint8        lcgroup;   /*!< LC group */
  lte_lc_id_t  lcid;
} lte_mac_volte_lcid_cfg_s;

/*! @brief Structure for allowed throughput API*/
typedef struct
{
  uint32 alpha; /*The value of alpha in percentage*/
  uint32 global_timer; /*Timer for the calculation cycle*/
  uint32 act_tx_time; /*Time which is actually used to transmit data*/
  uint64 prev_data_txed; /*Total data txed by MAC at the last time*/
  uint64 cur_data_txed;  /*Total data txed by MAC at the current time*/
  uint32 prev_tput; /*previous throughput calculation in bytes/msec */
  uint32 cur_tput; /*current throughput calculation in bytes/msec */
}lte_mac_qos_allowed_tput_s;

/*! @brief Data structure for QoS logic*/
typedef struct
{
  lte_mac_qos_bsr_triggered_e bsr_triggered;  /*!< current BSR triggered */
  lte_mac_qos_bsr_event_e     last_bsr_event; /*!< last BSR event */
  uint16  retxed_bsr_timer_count;     /*!< retxed BSR timer counter in ms*/
  lte_mac_qos_sr_pending_e sr_request_pending;  /*!< SR request pending state to L1*/
  uint16  request_bsr_instance_number; /*!< indicate the request BSR instance number*/
  uint8   last_high_data_arrival_pri;  /*!< last high data arrival priority */
  uint8   total_active_lc;     /*!< total active LCID inthe sorted_active_lc*/
  lte_mac_qos_active_lc_info_s sorted_active_lc[LTE_MAX_LC]; /*!< sorted list active LC*/
  lte_mac_buffer_info_s        lc_buffer_info;  /*!< LC buffer information */ 
  int32  acc_bucket_level[LTE_MAX_LC];  /*!< accumulated bucket level */
  uint8  ul_highest_nz_prio;       /*!< Indicate that a HDA event is needed to due buffer dip */
  lte_mac_qos_prep_info_s  curr_prep_info; /*!< working */
  uint32  acc_used_size[LTE_MAX_LC]; /*!< accumulate used size */
  /*! QoS information for all the LC which has the same priority */
  lte_mac_qos_same_prio_info_s  same_prio_info[LTE_MAC_QOS_TOTAL_LC_PRIORITY_LEVEL];
  uint8   last_qos_event_bitmasks;   /*! last QoS event bit masks*/
  boolean fc_bsr_trigger;    /*! flag to indicate if the FC BSR is triggered*/
  uint16 fc_bak_rbsr_timer;  /*! Backup Flow Control Robustness BSR timer */

  /* define default value of retxed BSR interval. For FC, this value will
  be used. It is a compromise between maximizing SR internal and stastifying
  TCP windows */
  uint16 lte_mac_qos_fc_integration_peroid;
  uint8   delay_ul_rach_duration; /*! delay UL RACH request in ms */
  /* total buffer size from all the active LCIDs */
  uint32  total_lc_buffer_size;
  uint32  sr_send_cnt;  /*!< Count of SR requests to ML1;to sync QoS and UL */
  /*! Flag to indicate that MAC is already working on an external high data
     arrival SR trigger from PDCP */
  boolean ext_hdr_trig_pending;
  /*! BSR event tracker for external high data arrival */
  lte_mac_qos_bsr_event_e ext_hdr_bsr_event_type; 
  /*! Static message structure to send high data arrival message */
  lte_mac_intmsg_ext_hd_arrival_ind_s send_sr_ind_msg;
  /*! VoLTE LCID configuration backup */
  lte_mac_volte_lcid_cfg_s volte_cfg_bkp;
  /*! enumeration of special SR trigger */
  lte_mac_qos_special_sr_trigger_e special_sr_trigger;
  /*! current sr trigger type */
  lte_cphy_sr_trigger_type_e sr_type;
  /*! TA interval threshold to trigger SR */
  uint32 ta_interval_sr_trigger_threshold;
  /*!< snapshot of dl task dequeue count */
  uint32                      dl_task_num_rlc_enq;
  /*!< monitor traffic activity */
  uint32                      traffic_monitor_timer;
  /*!< Traffic monitor active */
  boolean                     traffic_monitor_active;
  /*!< Traffic inactivity timer - Time before inactivity logic kicks in*/
  uint32                      traffic_inactivity_timer;
  
  /*!< Variable to implement the MAC allowed throughput API*/
  lte_mac_qos_allowed_tput_s  all_tput;
  /*! SFN Known flag. Set to FALSE when handover starts and later 
  to TRUE once PBCH is decoded. Default is TRUE */
  boolean               sfn_known;
  /*! counter in ms to count time elapse after tuneaway, will be reset to 
  0 after tuneaway and keep increasing until it hit the configured cdrx length */
  uint32                cdrx_count;
} lte_mac_qos_data_s;

/*! @brief Data structure for QoS statistics*/
typedef struct
{
  uint32 num_retxed_bsr;
  /*! number of times the sr request fails*/
  uint32 num_sr_req_fail;
}lte_mac_qos_stats_s;


/*! @brief QoS local data structure definition
*/
typedef struct
{
  /*! QoS Preprocessing Information Double Buffer Structure. MAC Control
  Task will update the new info, and MAC Up Link Task will read it */
  lte_mac_qos_prep_info_s      prep_info_buf[2];  
  /*! QoS Preprocessing Information double buffering */
  lte_mac_common_dbuf_s        prep_info_dbuf;
  /*! QoS feedback Information circular buffer size. MAC UL packetbuild
  is the producer and MAC QoS is the consummer */
  lte_mac_qos_feedback_info_s  feedback_info_buf[LTE_MAC_QOS_FB_CIRCQ_BUF_SIZE];
  /*! QoS Data feedback circular queue */
  lte_circ_q_s                 feedback_info_circq;
  /*! QoS configuration info */
  lte_mac_qos_cfg_s            cfg;
  /*! QoS operated data */
  lte_mac_qos_data_s           data;
  /*! QoS stats*/
  lte_mac_qos_stats_s          stats;
  /*! Pointer to global MAC Log structure */
  lte_mac_log_s*      log;
  /* !SFN info of the last UL packet build indication. Note that
  it will set to 0xFFFF if there is no packet build indication */
  uint16  qos_fb_sfn;
  /*! QoS maximum number of feedback buffer that are processed/consumed 
  contiguously */
  uint8   max_num_feedback_buf;
  /*! QoS Number of build indications that are processed/consumed contiguosly currently*/
  uint8   count_feedback_buffer;
} lte_mac_qos_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  lte_mac_qos_init

===========================================================================*/
/*!
    @brief
    This function is initialized all the QoS/BSR data structures.

    @detail

    @return
    None.

    @see related_function()


*/
/*=========================================================================*/
extern void lte_mac_qos_init
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_get_data_ptr

===========================================================================*/
/*!
    @brief
    This function returns the pointer to the main QoS data structure

    @detail

    @return
    None.

    @see related_function()


*/
/*=========================================================================*/
extern  lte_mac_qos_s *lte_mac_qos_get_data_ptr
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_lc_info_config_process

===========================================================================*/
/*!
    @brief
    This function handles LC/QoS configuration. Based on the config pointer,
    it will delate/add/modify the active LC list and also update the QoS 
    infomation of the active LC.

    @detail

    @return
    None.

    @see related_function()


*/
/*=========================================================================*/
extern void lte_mac_qos_lc_info_config_process
(
  lte_mac_lc_info_cfg_s  *cfg_ptr /*!< LC Info configuration pointer*/
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_ul_config_process

===========================================================================*/
/*!
    @brief
    This function handles UL QoS configuration. This configuration will 
    have information for BSR timer and SR resource/periodicity

    @detail

    @return
    None.

    @see related_function()


*/
/*=========================================================================*/
extern void lte_mac_qos_ul_config_process
(
  lte_mac_ul_cfg_s  *cfg_ptr /*!< UL configuration pointer*/
);
/*===========================================================================

  FUNCTION:  lte_mac_qos_main_handler

===========================================================================*/
/*!
    @brief
    This function is the main handler for QoS processing. It will call
    functions to compute all the QoS parameters for the packet build in the 
    UL task also check if BSR or SR request should be started. 

    @detail
    For SR, it will send a LTE_CPHY_SEND_UL_SR_REQ to L1. Depend on the 
    status of LTE_CPHY_SEND_UL_SR_CNF, MAC Control task will trigger a RACH
    or not
    @return
    None

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_qos_main_handler
(
  void
);


/*===========================================================================

  FUNCTION:  lte_mac_qos_enable_sr_request

===========================================================================*/
/*!
    @brief
    This function will reset the SR request pending flag so that the new
    SR request can be made

    @detail

    @return
    None.

    @see related_function()


*/
/*=========================================================================*/
extern void lte_mac_qos_enable_sr_request
(
  void
);
/*===========================================================================

  FUNCTION:  lte_mac_qos_reset

===========================================================================*/
/*!
    @brief
    This function resets all the QoS/BSR data structures for various causes
    like connection release, HO or radio failure

    @detail

    @return
    None.

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_qos_reset
(
  lte_mac_inform_action_e   inform_action
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_get_total_buffer_size_of_all_lcids

===========================================================================*/
/*!
    @brief
    This function returns the total buffer sizes of all active LC IDs

    @detail
 
    @return
    total buffer size .  

    @see related_function()

*/
/*=========================================================================*/
extern uint32 lte_mac_qos_get_total_buffer_size_of_all_lcids
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_update_statistic

===========================================================================*/
/*!
    @brief
    This function initializes the MAC QoS related data for the MAC 
    statistics data.

    @detail
 
    @return
    total buffer size .  

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_qos_update_statistic 
(
  lte_mac_stats_data_s  *stats_ptr  /*!< pointer to statistic data */
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_ext_hdr_handler

===========================================================================*/
/*!
    @brief
    This function decides if the external high data arrival is sufficient to
    trigger an SR

    @detail
 
    @return


    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_qos_ext_hdr_handler
(
  lte_lc_id_t lcid
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_sps_release_process

===========================================================================*/
/*!
    @brief
    This function handles the logic for SPS release

    @detail
 
    @return


    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_qos_sps_release_process
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_proc_volte_lcid

===========================================================================*/
/*!
  @brief
  Handles the VoLTE LCID notification from RLC

  @return
  none 
*/
/*=========================================================================*/
extern void lte_mac_qos_proc_volte_lcid
(
  lte_mac_ctrl_msg_u *msg_ptr
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_set_special_sr_trigger_flag
===========================================================================*/
/*!
  @brief
  This function is set the flag to trigger the retxed BSR/SR after RF tune
  back 

  @return
  none 
*/
/*=========================================================================*/
extern void lte_mac_qos_set_special_sr_trigger_flag(void);


/*===========================================================================

  FUNCTION:  lte_mac_qos_activate_traffic_monitor

===========================================================================*/
/*!
    @brief
    API to activate traffic monitor functionality

    @detail


    @return
    None

    @see related_function()

*/
/*=========================================================================*/
void lte_mac_qos_activate_traffic_monitor
(
  lte_mac_common_state_e mac_state
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_deactivate_traffic_monitor

===========================================================================*/
/*!
    @brief
    API to deactivate traffic monitor functionality

    @detail


    @return
    None

    @see related_function()

*/
/*=========================================================================*/
void lte_mac_qos_deactivate_traffic_monitor
(
  void
);
/*===========================================================================

  FUNCTION:  lte_mac_qos_allowed_tput_process

===========================================================================*/
/*!
    @brief
    This function does the necessary processing of the data for getting the 
    allowed throughput

    @detail
   

    @return
   
    @see

*/
/*=========================================================================*/
extern void lte_mac_qos_allowed_tput_process
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_calc_allowed_tput

===========================================================================*/
/*!
    @brief
    This function calculates the allowed throughput

    @detail
   

    @return
    Allowed throughput in bytes/msec.
   
    @see

*/
/*=========================================================================*/
extern uint32 lte_mac_qos_calc_allowed_tput
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_sr_cnf_handler

===========================================================================*/
/*!
    @brief
    This function handles SR cnf from ML1

    @detail
   
    @return
   
    @see

*/
/*=========================================================================*/
extern void lte_mac_qos_sr_cnf_handler
(
  lte_mac_ctrl_msg_u*       msg_ptr, /*!< input message pointer */
  lte_mac_common_state_e    curr_state  /*!< current state */
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_send_access_problem_ind

===========================================================================*/
/*!
    @brief
    This function send access problem ind with different reason

    @detail
   
    @return
   
    @see

*/
/*=========================================================================*/
extern void lte_mac_qos_send_access_problem_ind
(
  lte_mac_access_reason_e access_reason /*!< access problem reason */
);

/*===========================================================================

  FUNCTION:  lte_mac_qos_update_sfn_status

===========================================================================*/
/*!
    @brief
    API to update the SFN status when receiving the indication from ML1

    @detail


    @return
    None

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_qos_update_sfn_status
(
  boolean status
);

/*==============================================================================

  FUNCTION:  lte_mac_qos_qsh_analysis_routine

==============================================================================*/
/*!
    @brief
    This API runs the MAC QOS QSH analysis routine

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC CTRL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_qos_qsh_analysis_routine 
(
  void
);

/*==============================================================================

  FUNCTION:  lte_mac_qos_qsh_reset_stats

==============================================================================*/
/*!
    @brief
    This API resets the MAC QOS Stats

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC CTRL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_qos_qsh_reset_stats
(
  void
);


/*==============================================================================

  FUNCTION:  lte_mac_qos_get_sr_cnt

==============================================================================*/
/*!
    @brief
    This API returns the send_sr_cnt

    @detail

    @return
    None

*/
/*============================================================================*/

extern uint32 lte_mac_qos_get_sr_cnt
(
  void
);


#endif /* LTE_MAC_QOS_H */
