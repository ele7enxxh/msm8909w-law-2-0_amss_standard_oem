#ifndef A2_POWERI_H 
#define A2_POWERI_H
/*!
  @file a2_poweri.h

  @brief
   The internal interface header file for power collapse.

   This header file exposes an interface to the power collapse procedure to the 
   a2 driver task.  

  @author
  araina
*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/a2/driver/src/a2_poweri.h#1 $$

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/02/15   am      CR945970 Remove compiler warnings
11/02/15   am      CR924224 Implementing try lock function for a2 power 
                   mutex.
10/20/15   am      CR922363: Remove SNOC voting in Lykan
05/19/15   yjz     CR818722: FR27594, A2 PC synchronization with CDRX
01/08/14   sn      CR776718: qtf changes
12/05/14   sn      CR767131: QSH initial version
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "utils_variation.h"
#include "a2_cfg.h"
#include "a2_dbg.h"
#include "a2_power.h"
#include "a2_task.h"
#include "a2_msg.h"
#include "a2_util.h"
#include "a2_dl_peri.h"
#include "a2_ul_peri.h"
#include "a2_ul_phy.h"
#include "a2_ul_per_task.h"
#include "a2_ul_phy_hspa.h"
#include "a2_dl_phy_hspa.h"
#include "a2_ul_sec.h"
#include "a2_dl_phy.h"
#include "a2_frag_dispenser.h"
#include "a2_platform.h"
#include "a2_test_reg.h"
#include "a2_mem.h"
#ifdef FEATURE_A2_ON_TARGET
#include <smsm.h>
#include <mcpm_api.h>
#include <msmhwioreg.h>
#include <msmhwio.h>
#include <npa.h>
#ifdef FEATURE_JOLOKIA_MODEM
  #include <VCSDefs.h>
#endif /* FEATURE_JOLOKIA_MODEM */
#endif /* FEATURE_A2_ON_TARGET */
#include <fs_lib.h>
#include <rex.h>
#include <timetick.h>
#include <a2_sio_ext.h>
#include <DDIPlatformInfo.h>
#include <a2_spsdebug.h>
#include <a2_taskq_hal.h>

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*!
   @brief
    macro used to indicate that A2 POWER is already initialized as
    part of A2 driver initialization
*/
#define A2_POWER_INITIALIZED                    0xFEEDC0FE

/* TODO: Needs to be removed after SMD check-in */
#ifndef SMSM_NETWORK_DRIVER_UNMOUNT 
#define SMSM_NETWORK_DRIVER_UNMOUNT         0x01000000
#endif /* SMSM_NETWORK_DRIVER_UNMOUNT  */

/* TODO: Needs to be removed after SMD check-in */
#ifndef SMSM_NETWORK_DRIVER_UNMOUNT_ACK 
#define SMSM_NETWORK_DRIVER_UNMOUNT_ACK         0x01000000
#endif /* SMSM_NETWORK_DRIVER_UNMOUNT_ACK  */

/*!
   @brief
    size of the wake up and shut down circular buffer
*/
#define A2_POWER_LOG_SIZE                            128

/*!
   @brief
    mask value for A2_POWER_LOG_SIZE
*/
#define A2_POWER_LOG_SIZE_MASK                       0x7F

/*!
   @brief
    size of the wake up and shut down circular buffer
*/
#define A2_POWER_APPS_SMSM_WAKE_UP_REQ_LOG_SIZE      32

/*!
   @brief
    mask value for A2_POWER_LOG_SIZE
*/
#define A2_POWER_APPS_SMSM_WAKE_UP_REQ_LOG_SIZE_MASK 0x1F

/*! @brief file name/location of A2 pc feature EFS file 
  bit 0 = enable(1)/disable(0) a2 pc feature
  bit 1 = enable/disabe BAM related handshake 
  between A2 driver and BAM DMUX when A2 PC is disabled
  0 = A2_POWER_CLIENT_INTERNAL_PER client will be used to wake up A2 which will 
  keep BAM ON always
  1 = A2_POWER_CLIENT_INTERNAL_PHY client will be used to wake up A2 which will 
  result in BAM related handshake every time apps/a2 per needs the BAM
  */
#define A2_POWER_COLLAPSE_EFS_FILE \
  "/nv/item_files/modem/utils/a2/a2_pc"

/* bit mask to indicate if a2 pc enable bit is set or not */
#define A2_POWER_ENABLE_PC_MASK                             0x1

/*! bit mask to indicate if bam handshaking is reqd when a2 pc is disabled */
#define A2_POWER_ENABLE_BAM_HANDSHAKE_WHEN_PC_DISABLED_MASK 0x2

/*! bit mask to disable XO shutdown */
#define A2_POWER_DISABLE_XO_SHUTDOWN_MASK                   0x4

/*!
   @brief
    user data for registering callback with smsm for apps vote bit 
*/
#define A2_POWER_APPS_VOTE_SMSM_CB_USER_DATA                 0xDEADC0FE

/*!
   @brief
   user data for registering callback with smsm for apps ack bit 
*/
#define A2_POWER_APPS_ACK_SMSM_CB_USER_DATA                  0xBEADC0FE

/*!
   @brief
   user data for registering callback with smsm for HSIC reset bit 
*/
#define A2_POWER_APPS_NAK_SMSM_CB_USER_DATA                   0xABCDBEEF

/*!
   @brief
   user data for registering callback with smsm for APPS mount unmount bit
*/
#define A2_POWER_APPS_MOUNT_UNMOUNT_SMSM_CB_USER_DATA        0xFACEFACE

/*!
   @brief
   maximum expected a2 wake up latency in microseconds
*/
#define A2_POWER_MAX_EXPECTED_A2_WAKEUP_LATENCY_IN_US        1000

/*!
   @brief
   maximum expected a2 shut down latency in microseconds
*/
#define A2_POWER_MAX_EXPECTED_A2_SHUTDOWN_LATENCY_IN_US      1000

/*!
   @brief
   bitmask for checking if all the taskqs except DL PHY HW thread are idle
*/
#define A2_POWER_DBG_TF_A2_IDLE_EXCEPT_DL_PHY_HW_BM \
  (A2_HW_DBG_TF_CUR_STATE_BM |\
   A2_HW_DBG_TF_TD_CUR_STATE_BM |\
   A2_HW_DBG_TF_CUR_ARB_STATE_BM |\
   A2_HW_DBG_TF_DMA_REQ_BM)

/*!
   @brief
   bitmask for checking if all the taskqs and hw threads are idle
*/
#define A2_POWER_DBG_TF_A2_IDLE_BM \
  (A2_HW_DBG_TF_CUR_STATE_BM |\
   A2_HW_DBG_TF_TD_CUR_STATE_BM |\
   A2_HW_DBG_TF_CUR_ARB_STATE_BM |\
   A2_HW_DBG_TF_DMA_REQ_BM |\
   A2_HW_DBG_TF_DECOB_TRB_ARB_LOCK_BM)

/*!
   @brief
   macro returns the power vote for requested client, 0=SHUT DOWN, WAKE UP!=0
*/
#define A2_POWER_GET_VOTE(client)\
  ((a2_power.state.req_bmask) & (0x1 << client))

/*!
   @brief
   macro sets(1=WAKE UP) the power vote for requested client
*/
#define A2_POWER_SET_VOTE(client)\
  ((a2_power.state.req_bmask) |= (0x1 << client))

/*!
   @brief
   macro resets (0=SHUT DOWN) the power vote for requested client
*/
#define A2_POWER_RESET_VOTE(client)\
  ((a2_power.state.req_bmask) &= (~((uint32)0x1 << (client))))

/*!
   @brief
   maximum number of times a2 task can be put to sleep during per
   reset check
*/
#define A2_POWER_MAX_NUM_OF_REX_SLEEP      900

/*!
   @brief
   maximum number of times the per reset checks can run in while loop
   before a2 task is put to sleep 
*/
#define A2_POWER_MAX_NUM_OF_TRIAL_BEFORE_SLEEP      50

/*!
   @brief
   maximum number of times the while loop needs to run for flushing UL PHY
   before acquiring the mutex
*/
#define A2_POWER_MAX_NUM_OF_LOOP_BEFORE_ACQ_UL_PHY_MUTEX      4

/*!
   @brief
   enum for state of apps SMSM ack
*/
typedef enum
{
  A2_POWER_APPS_SMSM_ACK_STATE_NOT_PENDING = 0,
  A2_POWER_APPS_SMSM_ACK_STATE_PENDING     = 1,
  A2_POWER_APPS_SMSM_ACK_STATE_MAX         = 7
} a2_power_apps_smsm_ack_state_e;

/*!
   @brief
   enum for state of apps mount/unmount
*/
typedef enum
{
  A2_POWER_APPS_SMSM_STATE_MOUNT       = 0,
  A2_POWER_APPS_SMSM_STATE_UNMOUNT     = 1,
} a2_power_apps_mount_unmount_state_e;

/*!
   @brief
   enum for state of bus config
*/
typedef enum
{
  A2_POWER_MSS_CFG_BUS_INVALID   = 0,  
  A2_POWER_MSS_CFG_BUS_19  = 1, /*relinquish the vote for Q6 SW*/
  A2_POWER_MSS_CFG_BUS_72  = 2, 
  A2_POWER_MSS_CFG_BUS_144 = 3,
  A2_POWER_MSS_CFG_BUS_MAX
}a2_power_mss_config_bus_state_e;
/*!
   @brief structure holds the apps and modem smsm information
*/
typedef struct
{
  /*! current bit value of the apps smsm vote bit */
  uint32 apps_smsm_vote_bit;

  /*! current bit value of the apps smsm ack bit */
  uint32 apps_smsm_ack_bit;

  /*! current bit value of the apps smsm nak bit */
  uint32 apps_smsm_nak_bit;

  /*! current bit value of the modem smsm a2 state bit */
  uint32 modem_smsm_a2_state_bit;

  /*! current bit value of the modem smsm ack bit */
  uint32 modem_smsm_ack_bit;

  /*! current bit value of the modem smsm nak bit */
  uint32 modem_smsm_nak_bit;

  /*! current state of the modem mount/unmount smsm bit */
  a2_power_apps_mount_unmount_state_e apps_unmount_state;

  /*! current bit value of the modem mount/unmount ack smsm bit */
  uint32 modem_mount_unmount_ack_bit;

  /*! whether apps smsm ack is PENDING or NOT */
  a2_power_apps_smsm_ack_state_e apps_smsm_ack_state;

} a2_power_smsm_info_s;

/*!
   @brief structure holds the overall a2 state (hw + smsm) variables
*/
typedef struct
{
  /*! smsm state variable */
  a2_power_smsm_info_s smsm_state;

  /*! a2 hw state variable */
  volatile a2_power_hw_state_e hw_state;

  /*! bitmask holds the vote value from all clients 0 = A2_POWER_REQ_SHUT_DOWN, 
    1 = A2_POWER_REQ_WAKE_UP  */
  uint32              req_bmask;

  /*! mss config bus frequency state*/
  a2_power_mss_config_bus_state_e mss_cfg_bus_state;

  /*! boolean to indicate that SMSM ACK is pending for the APPS A2 PC REQ */
  boolean             ack_for_apps_a2_pc_req_pending;

} a2_power_state_s;

/*!
   @brief structure to hold the info of currently serviced power request
*/
typedef struct
{
  /*! client requesting for a2 services */
  a2_power_client_e client;

  /*! type of req */
  a2_power_req_e    req;

} a2_power_req_info_s;

/*!
   @brief Structure to log the timing info of received messages
*/
typedef struct
{
  /*! SCLK timestamp when the request is sent by caller */
  a2_timetick_t req_rcvd_timestamp;

  /*!  SCLK Time when a2 starts processing the request */
  a2_timetick_t req_proc_start_time;

  /*! Total processing time (in micro seconds) a2 takes from the start of the 
    processing to the end of processing of the request */
  a2_timetick_t total_req_proc_time_in_us;

  /*!  power request call back processing time */  
  a2_timetick_t req_proc_cb_time;

} a2_power_req_time_s;

/*!
   @brief Structure to log the power requests and a2 state changes
*/
typedef struct
{
  /*! input into an a2 state */
  a2_power_req_info_s input;

  /*! new a2 state */
  a2_power_state_s    new_state;

  /* request timing info */
  a2_power_req_time_s time;

} a2_power_log_s;

/*!
   @brief Structure to hold a2 power wake up/shut down related stats
*/
typedef struct
{
  /*! num wake up requests received per client since boot up */
  uint32 num_wake_up_req_per_client[A2_POWER_CLIENT_MAX];

  /*! num shut down requests received per client since boot up */
  uint32 num_shut_down_req_per_client[A2_POWER_CLIENT_MAX];

  /*! num times a2 hw is powered up */
  uint32 num_total_a2_wake_ups;

  /*! num times a2 hw is powered down */
  uint32 num_total_a2_shut_downs;

  /*! num times a2 receive VOLTE sleep from L1*/
  uint32 num_total_a2_volte_sleeps;

  /*! num times apps voted for a2 wake up by setting the smsm bit */
  uint32 num_apps_smsm_vote_bit_set;

  /*! num times apps voted for a2 shut down by clearing the smsm bit */
  uint32 num_apps_smsm_vote_bit_reset;

  /*! num times apps smsm nak bit toggled i.e. num times apps requested for 
  force shutdown with data pending in a2 WMs/taskq/BAM */
  uint32 num_apps_smsm_nak_bit_toggled;

  /*! num times apps acked the modem a2 state smsm bit request */
  uint32 num_apps_smsm_ack_bit_cb_called;

  /*! num times modem informed apps about a2 wake up by setting the smsm bit */
  uint32 num_modem_smsm_a2_state_bit_set;

  /*! num times modem informed apps about a2 shut down by setting the smsm bit*/
  uint32 num_modem_smsm_a2_state_bit_reset;

  /*! num times modem acked the apps request by toggling the smsm ack bit */
  uint32 num_modem_smsm_ack_bit_toggled;

  /*! num times modem nacked the apps request by toggling the smsm nak bit */
  uint32 num_modem_smsm_nak_bit_toggled;

  /*! stat to indicate if apps smsm had requested for a2 wake up before modem 
    boot up, a2 will check this only once i.e. during a2 boot up */
  uint32 num_apps_smsm_wake_up_req_before_modem_bootup;

  /*! num times A2 receive callback from MCPM to force stop inactivity timer*/
  uint32 num_inactivity_timer_force_stop;

  /*! stat to indicate max a2 wake up latency in micro seconds */
  uint32 max_a2_wake_up_latency_in_us;

  /*! req bitmask at the time of max wake up latency */
  uint32 max_a2_wake_up_latency_req_bitmask;

  /*! num time a2 wake up latencies were higher than expected */
  uint32 num_high_a2_wake_up_latency;

  /*! stat to indicate max a2 shut down latency in micro seconds */
  uint32 max_a2_shut_down_latency_in_us;

  /*! req bitmask at the time of max shut down latency */
  uint32 max_a2_shut_down_latency_req_bitmask;

  /*! num time a2 shut down latencies were higher than expected */
  uint32 num_high_a2_shut_down_latency;

  /*! num times modem mount unmount ack bit was toggled */
  uint32 num_modem_mount_unmount_ack_bit_toggled;

  /*! latency in milliseconds that apps took to process a modem request and 
    send the ack for the first modem smsm request at bootup */
  uint32 apps_smsm_req_proc_latency_at_bootup;

  /*! max latency in milliseconds that apps took to process a modem request and 
    send the ack */
  uint32 max_apps_smsm_req_proc_latency;

  /* Max time in us for A2_DBG_TASK_FETCHER to go idle */
  uint32 max_dbg_task_fetcher_go_idle_time;

  /* Max time in us for MCDMA_LOG_CH_BUSY to go idle */
  uint32 max_mcdma_log_ch_busy_go_idle_time;

  /* Max number of call backs called */
  uint32 num_power_req_proc_cbs_called[A2_POWER_CLIENT_MAX];
  /* Max number of un registered call backs called */
  uint32 num_power_req_proc_with_null_cbs[A2_POWER_CLIENT_MAX];

  /* Max PHY mutex lock time during per reset in us */
  uint32 max_phy_mutex_lock_time_during_per_reset_in_us;

  /* Max sleep count for acquire PHY mutex before lock*/
  uint32 max_sleep_count_to_acquire_ul_phy_mutex_before_lock;

  /* Max sleep count for acquire PHY mutex after lock*/
  uint32 max_sleep_count_to_acquire_ul_phy_mutex_after_lock;

  /*! max number of times a2 task was put to sleep during an entire per reset */
  uint32 max_num_of_sleep_for_pc;

  /*! max number of times a2 sleep 1 ms for A2 HW to setup properly*/
  uint32 max_num_of_sleep_for_wakeup;

  /* Max PER reset procedure time in us */
  uint32 max_per_reset_time_in_us;

  /* Number of times apps smsm req skipped during unmount state */
  uint32 num_skipped_apps_smsm_req;

  /* Number of times apps smsm ack skipped during unmount state */
  uint32 num_skipped_apps_smsm_ack;

  /* Number of times apps smsm nak skipped during unmount state */
  uint32 num_skipped_apps_smsm_nak;

  /* Number of times there were pending packets in UL per statusq
     after disabling per */
  uint32 num_total_pending_ul_per_data_at_per_reset;

  /* Number of pending data on DL PHY statusq when ML1 voted for
     shut down */
  uint32 num_pending_data_on_dl_phy_at_shutdown;
  /* Number of pending data on DL PHY statusq when ML1 voted for
     wake up */
  uint32 num_pending_data_on_dl_phy_at_wakeup;
} a2_power_stats_s;

/*!
   @brief Structure for a2 per reset debug variables
*/
typedef struct
{
  /* 1 tick = 52 nanoseconds */
  boolean per_reset_in_progress;
  a2_timetick_t per_reset_start_time;
  a2_timetick_t ul_per_bam_idle_time;
  a2_timetick_t dl_per_bam_idle_time;
  a2_timetick_t dl_per_rd_wr_ptrs_match_time;
  a2_timetick_t per_idle_time;
  a2_timetick_t ul_bam_idle_after_per_disable_time;
  a2_timetick_t per_idle_after_per_disable_time;
  a2_timetick_t all_mcdma_channels_idle_b4_a2_bam_reset_time;
  a2_timetick_t a2_before_bam_reset_time;
  a2_timetick_t a2_after_bam_reset_time;
  a2_timetick_t ul_phy_rd_wr_ptrs_match_time_before_lock;
  a2_timetick_t ul_phy_rd_wr_ptrs_match_time_after_lock;
  a2_timetick_t phy_mutexes_acquire_time;
  a2_timetick_t ul_sec_rd_wr_ptrs_match_time;
  a2_timetick_t dl_phy_rd_wr_ptrs_match_time;
  a2_timetick_t all_taskqs_idle_after_phy_lockdown_time;
  a2_timetick_t ul_sec_stall_time;
  a2_timetick_t all_taskqs_idle_after_ul_sec_restore_time;
  a2_timetick_t all_mcdma_channels_idle_b4_per_reset_time;
  a2_timetick_t all_taskqs_idle_after_per_reset_time;
  a2_timetick_t phy_mutexes_release_time;
  a2_timetick_t per_reset_end_time;
  a2_timetick_t total_reset_time_in_us;

  /*! number of times a2 task was put to sleep during each per reset check */
  uint32 loop_count_for_each_pc_check;

  /*! total number of times a2 task was put to sleep during the last per reset */
  uint32 total_num_of_sleep_for_pc;

#ifdef FEATURE_A2_PC_DBG
  a2_dbg_hw_regs_s hw_reg_dump_before_per_reset;
  a2_dbg_hw_regs_s hw_reg_dump_after_per_reset;
#endif /*FEATURE_A2_PC_DBG*/
} a2_power_per_reset_dbg_s;

/*!
   @brief Structure to hold a2 power shut fown call backs and user data
*/
typedef struct
{
  /* to store the registered call back function*/
  a2_power_req_proc_cb     req_proc_cb;
  /* to store user data which is passed along with cb funciton  */
  uint32                   user_data;
}a2_power_req_proc_cb_info_s;

/*!
   @brief Structure to hold all global variables used in this file
*/
typedef struct
{
  /*! variable to keep track whether a2 power structure is initialized or
      not. This strcut cannot be initialized more than once after power up */
  uint32                   initialized;

  /*! index into the a2_power_log circular buffer for the newly received msg */
  uint32                   new_req_log_idx;

  /*! index into the a2_power_log circular buffer for the currently processed 
    msg */
  uint32                   req_log_idx;

  /*! logging for wake up and shut down requests */
  a2_power_log_s           req_log[A2_POWER_LOG_SIZE];

  /*! index into the apps smsm wake up requests log */
  uint32                   apps_smsm_req_log_idx;

  /*! apps smsm wake up request log, this is needed since the apps wake up
  requests may not get processed right away in case of back to back a2
  shut down and wake up */
  a2_timetick_t           apps_smsm_wake_up_req_log[
                               A2_POWER_APPS_SMSM_WAKE_UP_REQ_LOG_SIZE];

  /*! statistics for power module */
  a2_power_stats_s         stats;

  /*! current power request being serviced by A2 driver */
  a2_power_req_info_s      curr_req;

  /*! variable holds the current state of A2 (HW + SMSM) */
  a2_power_state_s         state;

  /*! variable holds the perceived A2 HW state per client basis */
  a2_power_hw_state_e      hw_state_per_client[A2_POWER_CLIENT_MAX];

/*! back-up static memory buffer for a2 internal memory */
  a2_mem_mapping_s             int_mem_backup;

#ifdef FEATURE_A2_ON_TARGET
  /*! a2 npa client to vote for/against SW Q6 wake up or power collapse */
  npa_client_handle        q6sw_npa_client;
  /*! a2 npa client to vote for q6 cpu clock frequency */
  npa_client_handle        q6sw_cpu_freq_npa_client;
  /*! a2 npa client to vote for modem clock\config bus */
  npa_client_handle        mss_config_bus;
#ifndef FEATURE_A2_LYKAN
  /*! a2 npa client to vote for/against SNOC */
  npa_client_handle        bus_snoc_npa_client;
#endif
  /*! a2 npa client to vote for/against PCNOC */
  npa_client_handle		 bus_pcnoc_npa_client;
  /*! a2 npa client to vote for/against bimc memory bus */
  npa_client_handle        mem_bimc_npa_client;
  /*! a2 npa client to vote against xo shutdown */
  npa_client_handle        cx_npa_client;
  /*! boolean to indicate whether a2_power_config_modem_for_peak_data_rates()
      api is called or not */
  boolean                  voted_for_peak_speed;
  /*! boolean to indicate if a2 driver has voted for all the necessary A2 PER
  related clocks */
  boolean                  voted_for_per_clocks;
#endif /* FEATURE_A2_ON_TARGET */

/*! volatile variable to read A2 register values, needed to add SW delays 
    during the A2 wake up/shut down procedure */
  volatile uint32 reg_value;

  /*! time when A2 sets or resets the modem power control bit */
  a2_timetick_t modem_power_ctrl_set_time;

  /*! Critical section for shutting down A2 */
  a2_crit_sect_s           pc_crit_sect;
  boolean                  npa_rumi_mode;
 /*! a2 power process call backs array of structs*/
  a2_power_req_proc_cb_info_s  req_proc_cb_info[A2_POWER_CLIENT_MAX];

  /*! debug variables for per reset */
  a2_power_per_reset_dbg_s per_reset_dbg;
} a2_power_s;

/*==============================================================================

  FUNCTION:  a2_power_get_top_lvl_ptr

==============================================================================*/
/*!
  @brief
  Returns a pointer to the top-level data structure.
*/
/*============================================================================*/
const a2_power_s * a2_power_get_top_lvl_ptr
(
  void
);

#endif /* A2_POWERI_H */
