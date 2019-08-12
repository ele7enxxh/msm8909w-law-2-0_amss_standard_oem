#ifndef WL1ANTSWITCH_H
#define WL1ANTSWITCH_H
/*===========================================================================

                    wl1antswitch header file

DESCRIPTION
  This file contains global declarations and external references
  for the ant switch feature.

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2012-2015 by Qualcomm Technologies, Inc. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1antswitch.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
----------------------------------------------------------------------------------------
08/15/15   hdk   Update state to ASD TRM for all state transitions.
03/06/15   jhl   Added mutex for asdiv
02/18/15   jhl   Account for MPR value for MTPL based algorithm 
01/13/15   hdk   Updating ACQ state to TRM
11/26/14   hdk   Removing FEATURE_WCDMA_JOLOKIA_MODEM from DSDA ASDiv changes.
11/21/14   hdk   ASDiv Change for DRDSDS
11/10/14   hdk   ASDiv test mode enhanced
10/06/14   sd    Fixed MSIM compilation warnings
10/07/14   sks   Compilation warning fix.
06/26/14   hdk   Aborting Type1 Switch if RxD is off while issuing ASDiv switch
06/24/14   hdk   Featurizing DSDA ASDiv changes under FEATURE_WCDMA_JOLOKIA_MODEM
06/13/14   hdk   ASDiv DSDA feature
03/20/14   hdk   Adding support for Suspended case for DSDS
03/04/14   hdk   Adding support for OOS case for DSDS.
02/08/14   vs    Asdiv DSDS changes.
12/02/13   vs    Added support for the API wcdma_asdiv_disable_mtpl_algorithm.
09/17/13   hk    added Event logs for ASDiv
02/05/13   vs    added the macro WL1AS_AVG_RSCP_THRESH_FOR_LOOK_BACK
01/30/13   dm    Keep the finger profile same when switching antennas for type 2 mode.
01/21/13   vs    Skip antenna switch measurements till the physical channel est completes.
01/12/13   rc    Added code stop As-div algo on DCH cleanup.
01/09/13   rc    Added function prototype.
12/19/12   rc    Added function prototype.
12/12/12   rc    Added into AS database.
12/06/12   vs    Added support for idle mode antenna switch diversity
12/05/12   vs    Added support for the type 2 algorithm.
12/05/12   vs    Prototype for the antenna switch mdsp pre callback.
11/28/12   vs    changed the element "counter_suspend_Ant_switching" to unit16
11/19/12   vs    added support for the MTPL algorithm and test mode 3.
11/17/12   vs    Turning ON RxD even when the RxD is On by default.
11/12/12   vs    Support for the dynamic algorithm changes and loopback probing.
11/12/12   vs    Handling of the condition when we are about to send the command to FW and RxD is not ON.
11/10/12   vs    Modified the arrays rscp_prim and rscp_div to uint16.
10/28/12   vs    created the new file for the antenna switch feature

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
#include "customer.h"
#include "l1const.h"
#include "l1mathutil.h"
#include "nv.h"
#include "wfw_sw_cmd_proc_intf.h"
#include "trm.h"
#include "wl1m.h"
#include "wl1tri.h"
#include "event_defs.h"

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* Global Flags for the blocking and unblocking ASDiv algorithm */
extern boolean wl1_as_block_asdiv_flag;
extern boolean wl1_as_pending_block_asdiv_flag;

/* logging related : ASDiv metrics logging starts */
#define WCDMA_ANT_SWITCH_METRICS_calltype_BMSK 0x0000000F
#define WCDMA_ANT_SWITCH_METRICS_calltype_SHFT 0

#define WCDMA_ANT_SWITCH_METRICS_rxdstate_BMSK 0x00000010
#define WCDMA_ANT_SWITCH_METRICS_rxdstate_SHFT 4

#define WCDMA_ANT_SWITCH_METRICS_antennaindex_BMSK 0x00000020
#define WCDMA_ANT_SWITCH_METRICS_antennaindex_SHFT 5

#define WCDMA_ANT_SWITCH_METRICS_eul_BMSK 0x00000040
#define WCDMA_ANT_SWITCH_METRICS_eul_SHFT 6

#define WCDMA_ANT_METRICS_SET_VAL(buf, field, val) \
  (buf) &= (~WCDMA_ANT_SWITCH_METRICS_##field##_BMSK); \
  (buf) |= (((val) << WCDMA_ANT_SWITCH_METRICS_##field##_SHFT) & \
            (WCDMA_ANT_SWITCH_METRICS_##field##_BMSK))


#ifndef WCDMA_ANT_SWITCH_METRICS_LOG_PKT
#define WCDMA_ANT_SWITCH_METRICS_LOG_PKT WCDMA_ANT_SWITCH_METRICS_0x41B5_LOG_PKT
#else
#error "Redefinition of the log command code: WCDMA_ANT_SWITCH_METRICS_LOG_PKT"
#endif

#define WCDMA_ANT_SWITCH_METRICS_INVALID_VAL 0xFF
#define WCDMA_ANT_SWITCH_METRICS_LOG_NUM_BUF 2
#define NUM_MAX_WCDMA_ANT_SWITCH_METRICS_SAMPLES 50
#define WCDMA_ANT_SWITCH_METRICS_SAMPLE_SIZE sizeof(wcdma_ant_switch_metrics_info_struct_type)

#define WCDMA_ANT_SWITCH_METRICS_LOG_MAX_BUF_SIZE  ((sizeof(wcdma_ant_switch_metrics_info_struct_type)) * \
                                           NUM_MAX_WCDMA_ANT_SWITCH_METRICS_SAMPLES)

/* uplink API returns txagc/mpr value of 766 if they are invalid*/
#define WCDMA_ANT_SWITCH_TXAGC_MPR_INVALID_VALUE 766

/* Header definition since the header definition is defined in log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).      */
} wcdma_ant_switch_metrics_log_hdr_struct_type;


typedef PACKED struct PACKED_POST
{
  uint8 cfn_info;
  /* Number of frames before probing */
  uint16 frame_cnt2probe;
  /* Gives the info of the call type,antenna index etc */
  uint8 current_frame_stat;
  /* Gives the RSCP of the antenna 0 */
  uint16 rscp_0;
  /* Gives the RSCP of the antenna 1 */
  uint16 rscp_1;
  /* Gives the RxAgc of the antenna 0 */
  int16 rxagc_0;
  /* Gives the RxAgc of the antenna 1 */
  int16 rxagc_1;
  /* indicates if the probe is active for that frame or not */
  uint8 probe_indicator;
  /* indicates the txagc value in that frame */
  int16 txagc_value;
  /* indicates the MTPL value in that frame */
  int16 mtpl_value;
  /* indicates the counter 1 value of the MTPL algorithm */
  uint8 mtpl_counter1;
  /* indicates the counter 2 value of the MTPL algorithm */
  uint8 mtpl_counter2;
  /* indicates the counter 3 value of the MTPL algorithm */
  uint8 mtpl_counter3;
} wcdma_ant_switch_metrics_info_struct_type;

/* structure for the log packet wcdma_ant_switch_metrics */
LOG_RECORD_DEFINE(WCDMA_ANT_SWITCH_METRICS_LOG_PKT)
  /* Version field */
  uint8 version;
  /* Number of samples */
  uint8 num_samples;
  /* wcdma ant switch metrics samples */
  uint8 info[WCDMA_ANT_SWITCH_METRICS_LOG_MAX_BUF_SIZE];
LOG_RECORD_END

/* Globals for all the NV item reads related to the Antenna switching feature */
extern boolean enable_disable_ant_switch_feature;

/* Flag to store whether a ANT SWITCH METRICS submit local command is pending or not */
extern boolean wcdma_ant_switch_metrics_log_cmd_pending;
/* Flag for the status of the log code in QXDM */
extern boolean wcdma_ant_switch_metrics_logging_enabled_qxdm;
/* Buffer index where samples are accumulated */
extern uint8 wcdma_ant_switch_metrics_log_info_buf_idx;
/* Log buffer availability status database */
extern boolean wcdma_ant_switch_metrics_log_buf_avail_status[WCDMA_ANT_SWITCH_METRICS_LOG_NUM_BUF];
/* The log buffer where samples are accumulated */
extern WCDMA_ANT_SWITCH_METRICS_LOG_PKT_type wcdma_ant_switch_metrics_log_pkt_buf[WCDMA_ANT_SWITCH_METRICS_LOG_NUM_BUF];
/* Flag indicating whether a packet has been submitted to diag */
extern boolean wcdma_ant_switch_metrics_log_submit;
/* The log buffer index to be submitted */
extern uint8 wcdma_ant_switch_metrics_log_buf_idx_to_submit;
/* offset value in the log buffer*/
extern uint16 wcdma_ant_switch_metrics_sample_buf_offset;
/* logging related : ASDiv metrics logging ends */

/* To handle the handover scenarios */
extern boolean ant_switch_measurements_skip_for_phy_chan_ext;
extern uint8 counter_skip_ant_switch_measurements_for_phy_chan_ext;

#define ANT_SWITCH_TIMER_VALUE 45 
#define WL1AS_AVG_RSCP_THRESH_FOR_LOOK_BACK 80
#define ANT_SWITCH_ACQ_TIMER_VALUE 30
/* The structure is the database which will store all the counters/flag and the state of the Antenna switch algorithm */
typedef struct 
{
  int16 rscp_threshold;
  /* To store the rscp threshold at which the probe 2 will be triggered for non RxD case. This is read from the NV */
  uint8 tx_pwr_headroom;
  /* variable is used to compare the Tx power after the switch to the MTPL */
  uint32 confirm_cnt;
  /* Counter to count the number of successive hits in probe step 2 */
  uint8 probe_time;
  /* this is equal to 2 frames. Time we need to wait for the RxD to turn ON */
  uint8 probe_wait;
  /* Counter we decrement. when it is equal to 0, RxD would be ON */
  uint8 RSSI_switch_threshold;
  /* Used to store the power delta between the Pri and Div antennas. This value is read from the NV and saved here*/
  boolean in_probe;
  /* Set to TRUE when we are in probe state */
  uint16 probe_interval;
  /* Saves the number of frames we need to count down so that we can probe again */
  uint16 frame_cnt2probe;
  /* Counter to count down. When it reaches 0, we probe */
  boolean rxd_flag_ant_probe;
  /* this is for info that RxD is ON since Ant Switch has asked it to be ON */
  boolean suspend_Ant_switching_algo;
  /* Flag and counter for the shutting the algorithm down for specific frames after the switch*/
  uint16 counter_suspend_Ant_switching;

  /* Flags related to the searcher interaction */ 
  boolean ant_switch_searcher_probing_in_progress;
  /* This flag is set when the search results are being collected */
  boolean ant_switch_searcher_results_available;
  /* The flag is set to TRUE when the 4 searcher results are available for Pri and Div antennas*/
  uint8 search_results_counter;
  /* Counter used to count the number of searcher results. */
  uint16 rscp_prim[4];
  /* Array to store the Pri rscp values */
  uint16 rscp_div[4];
  /* Array to store the Div rscp values */

  /* Flag and counter for the After switching protection algorithm */ 
  boolean ant_switch_protection_flag;
  
  boolean ant_switch_second_level_protection_flag;
  /* This flag is to avoid the ping pong when we switch back after the recent switch */

  /* array to save the last 50 frame's rscp values */
  uint16 rscp_values[50];
  uint8 rscp_array_index;

  /* Flag related to the FW switch command*/
  boolean ant_switch_command_in_progress;
  boolean ant_switch_command_to_send;
  /* Counter to make sure RxD is indeed ON before actually proceeding with the antenna switch command */
  uint8 make_sure_rxd_on_counter;

  /* Loopback algorithm declarations */
  uint16 rscp_avg;
  int16 rscp_drop;
  uint8 rscp_buffer_size;
  uint8 look_back_dist;
  uint8 look_back_threshold;
  uint16 rscp_avg_stored[7];
  /* Counter to make sure if we have valid 7 readings in the array*/
  uint8 counter_loopback;
  
  /* Dynamic probing */
  int16 probe_delta_prev_1;
  int16 probe_delta_prev_2;
  /* This flag is used to keep the UE in step 2 for a longer time */
  boolean proceed_to_step2_directly;

  /* Counter for waiting for 2 frames before calling the searcher APIs in case of the RxD ON case.*/
  uint8 counter_wait_for_rxd_on;
  boolean wait_for_rxd_on;

  /* MTPL based algorithm*/
  /* To save the ul tx samples of the first set of 50 samples */
  int16 ul_tx_samples1[50];
  /* To save the ul tx samples of the second set of 50 samples */
  int16 ul_tx_samples2[50];
  /* To keep the array index of ul_tx_samples1 */
  uint8 txagc_array1_index;
  /* To keep the array index of ul_tx_samples2 */
  uint8 txagc_array2_index;
  /* Counter for the percentage of time the Tx power touches the MTPL for the first set of samples */
  uint8 ul_tx_percentage_counter1;
  /* Counter for the percentage of time the Tx power touches the MTPL for the second set of samples */
  uint8 ul_tx_percentage_counter2;
  /* Flag to indicate that the ul_tx_samples2 values needs to be accumulated */
  boolean collect_txagc_2nd_array_samples;
  /* to store the percentage of time the tx power  */
  uint8 mtpl_percentage_pre_switch;
  /* To store the pre switch counter values */
  int16 mtpl_avg_pre_switch;
  /* To store the pre switch average value */
  /* post switch MTPL value array */
  int16 ul_tx_samples3[50];
  uint8 ul_tx_percentage_counter3;
  int16 probe_delta_latest;
  boolean skip_RSCP_decision;
  boolean ant_switch_algo_type1_stopped;

  /* Flag to make sure we skip the UL TX analysis when PA was off for even 1 frame */
  boolean pa_not_always_on_in_evaluation;
}wl1_as_db_type;


/* The structure is the database which will store all the counters/flag and the state of the Antenna switch algorithm type 2*/
typedef struct 
{

  uint16 frame_cnt2probe;
  /* Counter to count down. When it reaches 0, we probe */
  boolean suspend_Ant_switching_algo;
  /* Flag and counter for the shutting the algorithm down for specific frames after the switch*/
  uint16 counter_suspend_Ant_switching;

  boolean ant_switch_second_level_protection_flag;
  /* This flag is to avoid the ping pong when we switch back after the recent switch */

  /* array to save the last 50 frame's rscp values */
  uint16 rscp_values[50];
  uint8 rscp_array_index;

  /* Flag related to the FW switch command*/
  boolean ant_switch_command_in_progress;
  boolean ant_switch_command_to_send;

  /* Loopback algorithm declarations */
  uint16 rscp_avg;
  int16 rscp_drop;
  uint8 rscp_buffer_size;
  uint8 look_back_dist;
  uint8 look_back_threshold;
  uint16 rscp_avg_stored[7];
  /* Counter to make sure if we have valid 7 readings in the array*/
  uint8 counter_loopback;
  

  /* MTPL based algorithm*/
  /* To save the ul tx samples of the first set of 50 samples */
  int16 ul_tx_samples1[50];
  /* To save the ul tx samples of the second set of 50 samples */
  int16 ul_tx_samples2[50];
  /* To keep the array index of ul_tx_samples1 */
  uint8 txagc_array1_index;
  /* To keep the array index of ul_tx_samples2 */
  uint8 txagc_array2_index;
  /* Counter for the percentage of time the Tx power touches the MTPL for the first set of samples */
  uint8 ul_tx_percentage_counter1;
  /* Counter for the percentage of time the Tx power touches the MTPL for the second set of samples */
  uint8 ul_tx_percentage_counter2;
  /* Flag to indicate that the ul_tx_samples2 values needs to be accumulated */
  boolean collect_txagc_2nd_array_samples;
  /* to store the percentage of time the tx power  */
  uint8 mtpl_percentage_pre_switch;
  /* To store the pre switch counter values */
  int16 mtpl_avg_pre_switch;
  /* To store the pre switch average value */

  /* post switch MTPL value array */
  int16 ul_tx_samples_post_switch[45];
  uint8 ul_tx_percentage_counter3;
  int16 rscp_values_post_switch[45];

  /* to store rscp avgerage */
  uint16 rscp_avg_pre_switch;

  /* variables for the type 2 algorithm thresholds */
  uint8 threshold_lut[9];
  uint8 threshold_lut_array_index;

  boolean ant_switch_algo_type2_stopped;
  /* Flag to make sure we skip the UL TX analysis when PA was off for even 1 frame */
  boolean pa_not_always_on_in_evaluation;
}wl1_as_db_type2;


/* Typedef declaration for the CME callback function */
typedef void ANT_SWITCH_CME_CB_FUNC_TYPE(void);
/* Global to store the Switch config from MCS */
trm_ant_switch_div_config_type wl1_as_switch_config;

/* Enum for the current antenna switch algorithm*/
typedef enum 
{
  WL1_ANT_SWITCH_ALGO_NONE,
  WL1_ANT_SWITCH_ALGO_TYPE1,
  WL1_ANT_SWITCH_ALGO_TYPE2
}wl1_as_algo_type;

/* ASDiv Config packet */
#ifndef WCDMA_ANT_SWITCH_CONFIG_LOG_PKT
#define WCDMA_ANT_SWITCH_CONFIG_LOG_PKT WCDMA_ANT_SWITCH_CONFIG_0x41B4_LOG_PKT
#else
#error "Redefinition of the log command code: WCDMA_ANT_SWITCH_CONFIG_LOG_PKT"
#endif

#define WCDMA_ANT_SWITCH_CONFIG_LOG_NUM_BUF 1

/* Header definition since the header definition is defined in log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).      */
} wcdma_ant_switch_config_log_hdr_struct_type;

/* structure for the log packet ant_switch_config */
LOG_RECORD_DEFINE(WCDMA_ANT_SWITCH_CONFIG_LOG_PKT)
  /* Version field */
  uint8 version;
  /* UE state..DCH or FACH */
  uint8 ue_state;
  /* type of the algorithm configured */
  uint8 asdiv_algorithm_type;
  /* rscp power delta for the type 1 algorithm */
  uint8 type1_rscp_power_delta_threshold;
  /* rscp threshold for the type1 switch */
  uint8 type1_rscp_switch_threshold;
  /* rscp threshold for the loopback switching type1  */
  uint8 type1_rscp_loopback_threshold;
  /* type2 rscp switch threshold */
  uint8 type2_rscp_switch_threshold;
  /* threshold for comparing the counter values */
  uint8 type2_mtpl_switch_threshold;
  /* Number of frames for which the algo will be suspended after a switchback */
  uint16 type2_post_switchback_sus_time;
  /* the threshold base used to compare the post switch evaluation */
  int8 type2_post_switch_evaluation_base_threshold;
  /* type 2 algorithm rscp drop threshold */
  uint8 type2_rscp_drop_threshold;
  /* rscp threshold value when comparing the rscp values post switchback */
  uint8 type2_avg_rscp_threshold_loopback;
  /* band 1 bitmask */
  uint64 band1_bitmask;
  /* band 2 bitmask */
  uint64 band2_bitmask;
  /* rxd nv item value */
  uint8 rxd_nv_config;
  /* Switch config . 0:straight and 1:CrossSwitch*/
  uint8 antenna_switch_config;
  /* the test mode control for antenna diversity */
  uint8 antenna_switch_testmode_control;
LOG_RECORD_END

/* Flag for the status of the log code in QXDM */
extern boolean wcdma_periodic_stat_logging_enabled_qxdm;

/* The log buffer where samples are accumulated */
extern WCDMA_ANT_SWITCH_CONFIG_LOG_PKT_type wcdma_ant_switch_config_log_pkt_buf[WCDMA_ANT_SWITCH_CONFIG_LOG_NUM_BUF];
/* ASDiv Config packet ends */


/* STRUCTURE DEFINITIONS related to the EVENTS */
/* --------------------------------------------- */

/* EVENT_ASDIV_TYPE1_SWITCH_RSCP_BASED */ 
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint8 switched_antID;
  uint16 pri_rscp_avg_value;
  uint16 div_rscp_avg_value;
  int16 probe_delta;
  int16 probe_threshold;
}
wcdma_asdiv_type1_switch_rscp_based_evt_type;

/* EVENT_ASDIV_TYPE1_SWITCH_TXAGC_BASED */
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint8 switched_antID;
  uint8 counter1;
  uint8 counter2;
}
wcdma_asdiv_type1_switch_txagc_based_evt_type;


/* EVENT_ASDIV_TYPE1_SWITCHBACK_MTPL_BASED */
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint8 switched_antID;
  uint8 preswitch_counter_value;
  uint8 postswitch_counter_value;
  uint8 threshold;
}
wcdma_asdiv_type1_switchback_mtpl_based_evt_type;

/* EVENT_ASDIV_TYPE1_SWITCHBACK_TXAGC_BASED */
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint8 switched_antID;
  int16 preswitch_txagc_avg;
  int16 postswitch_txagc_avg;
  uint8 threshold;
}
wcdma_asdiv_type1_switchback_txagc_based_evt_type;

/* EVENT_ASDIV_TYPE2_SWITCH_RSCP_BASED */
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint8 switched_antID;
  uint16 rscp_avg;
  uint16 rscp_threshold;
}
wcdma_asdiv_type2_rscp_based_evt_type;

/* EVENT_ASDIV_TYPE2_SWITCH_MTPL_BASED */
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint8 switched_antID;
  uint8 counter1;
  uint8 counter2;
}
wcdma_asdiv_type2_mtpl_based_evt_type;

/* EVENT_ASDIV_TYPE2_SWITCHBACK_RSCP_LOOPBACK_BASED */
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint8 switched_antID;
  uint16 rscp_drop;
  uint16 prev_rscp_value;
  uint16 current_rscp_value;
  uint8 loopback_threshold;
  uint8 rscp_threshold;
}
wcdma_asdiv_type2_switchback_rscp_loopback_based_evt_type;

/* EVENT_ASDIV_TYPE2_SWITCHBACK_DEGRADATION_BASED */
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint8 switched_antID;
  int16 ul_degradation;
  int16 dl_degradation;
  uint16 threshold;
}
wcdma_asdiv_type2_switchback_degradation_evt_type;


/* EVENT_ASDIV_PROBE_BOTH_ANTENNAS */
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint16 pri_avg_rscp;
  uint16 div_avg_rscp;
}
wcdma_asdiv_Probe_both_antennas_evt_type;


/*Structure for switching during IDLE and ACQ (non Connected modes)*/
typedef PACKED struct PACKED_POST
{
  uint8 cfn;
  uint8 switched_antID;
  int16 pri_ant_rxagc;
  int16 div_ant_rxagc;
}wcdma_asdiv_Idl_Acq_sw_evt_type;


/* Global to store the Switch config from MCS. this will be used for TRM usage */

typedef enum 
{
  WL1_ANT_ACQ,
  WL1_ANT_FACH,
  WL1_ANT_DCH,
  WL1_ANT_FACH_EXIT,
  WL1_ANT_DCH_EXIT,
  WL1_ANT_IDLE_WAKEUP,
  WL1_ANT_IDLE_SLEEP,
  WL1_ANT_OOS,
  WL1_ANT_SUSPEND
 } l1_state_information_for_trm_type;


extern rex_crit_sect_type asdiv_mutex;
/* --------------------------------------
***     Function Prototypes 
----------------------------------------*/ 


/*===========================================================================
FUNCTION wl1_as_antenna_switch
DESCRIPTION
  Checks for a number of prerequisites before calling the mdsp command to send the antenna switch command to FW.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_antenna_switch(void);

/*===========================================================================
FUNCTION wl1_as_antenna_switch_type2
DESCRIPTION
  Checks for a number of prerequisites before calling the mdsp command to send the antenna switch command to FW.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_antenna_switch_type2(void);
/*===========================================================================
FUNCTION wl1_as_antenna_switch_done
DESCRIPTION
  Callback function which would be called after the switch is done by the FW.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_as_antenna_switch_pre_callback(void);

/*===========================================================================
FUNCTION wl1_as_antenna_switch_cmd_done

DESCRIPTION
    This is supposed to be executed after the FW sends the antenna switch command done.
 
RETURN VALUE
   None.
SIDE EFFECTS
  None.
=============================================================================*/

extern void wl1_as_antenna_switch_cmd_done(uint16 status);
/*===========================================================================
FUNCTION wl1_as_antenna_switch_done
DESCRIPTION
  Callback function which would be called after the switch is done by the FW.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_antenna_switch_done(void);


/*===========================================================================
FUNCTION wl1m_as_determine_as_switch_support
DESCRIPTION

DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/

extern void wl1m_as_determine_as_switch_support(void);

/*===========================================================================
FUNCTION wl1_as_cleanup
DESCRIPTION
 
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/

extern void wl1_as_cleanup(void);

/*===========================================================================
FUNCTION wl1_as_antenna_switch_done_type2
DESCRIPTION
  Callback function which would be called after the switch is done by the FW.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_antenna_switch_done_type2(void);
/*===========================================================================
FUNCTION wl1_as_init
DESCRIPTION
  This initializes all the variables and counters in the global database for antenna switch
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_init(void);
/*===========================================================================
FUNCTION wl1_as_init_type2

DESCRIPTION
    AS init
 
RETURN VALUE
   None.
SIDE EFFECTS
  None.
=============================================================================*/
extern void wl1_as_init_type2(void);
/*===========================================================================
FUNCTION wl1_as_reset
DESCRIPTION
  This reinitializes all the variables and counters in the global database for antenna switch
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_reset(void);
/*===========================================================================
FUNCTION wl1_as_reset_type2

DESCRIPTION
    AS reset
 
RETURN VALUE
   None.
SIDE EFFECTS
  None.
=============================================================================*/
extern void wl1_as_reset_type2(void);
/*===========================================================================
FUNCTION wl1_as_controller_type2
DESCRIPTION
  The main function which runs the algorithm for probing in the antenna switch feature algorithm type2.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_controller_type2(uint8 cfn_number);
/*===========================================================================
FUNCTION wl1_as_controller
DESCRIPTION
  The main function which runs the algorithm for probing in the antenna switch feature.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_controller(uint8 cfn_number);

/*===========================================================================
FUNCTION wl1_ant_nv_read
DESCRIPTION
  Reads the AS NV value and sets the overrides appropriately.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_ant_nv_read(void);

/*===========================================================================
FUNCTION wl1_ant_switch_band_nv_read
DESCRIPTION
  Reads the AS NV value and sets the overrides appropriately.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_ant_switch_band_nv_read(void);
/*===========================================================================
FUNCTION ant_switch_register_with_cme_cb
DESCRIPTION
  The function is used by CME to register the callback with antenna switch module.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void ant_switch_register_with_cme_cb(ANT_SWITCH_CME_CB_FUNC_TYPE *ant_switch_cme_cb);
/*===========================================================================
FUNCTION ant_switch_deregister_with_cme_cb
DESCRIPTION
  The function is used by CME to deregister the callback with antenna switch module.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void ant_switch_deregister_with_cme_cb(void);

/*===========================================================================
FUNCTION wl1_as_searcher_results_collection
DESCRIPTION
  The function collects the searcher results.
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_searcher_results_collection(void);
/*===========================================================================
FUNCTION wl1_as_searcher_results_verdict
DESCRIPTION
  The function gives the verdict about the probe based on the searcher results
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_as_searcher_results_verdict(int16 temp_probe_delta);
/*===========================================================================
FUNCTION wl1_as_searcher_results_verdict
DESCRIPTION
  The function gives the verdict about the probe based on the searcher results
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_ant_switch_idle_mode(event_id_enum_type event_id);


/*===========================================================================
FUNCTION wl1_ant_switch_acq_mode
DESCRIPTION
  
DEPENDENCIES
  None.
RETURN VALUE
  None
SIDE EFFECTS
  None
 ===========================================================================*/
extern boolean wl1_ant_switch_acq_mode(void);

/*===========================================================================

FUNCTION wl1_ant_switch_measurements_skip_for_phy_chan_ext

DESCRIPTION
  This function sets the flag "ant_switch_measurements_skip_for_phy_chan_ext" to TRUE

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/

extern void wl1_ant_switch_measurements_skip_for_phy_chan_ext(boolean);

/*===========================================================================

FUNCTION wl1_ant_switch_get_algo_type

DESCRIPTION
  This function returns the variable value of the global "ant_switch_algorithm_type"

DEPENDENCIES
  None.

RETURN VALUE
  uint8

SIDE EFFECTS
  None
 ===========================================================================*/

extern uint8 wl1_ant_switch_get_algo_type(void);

/*===========================================================================
FUNCTION ant_switch_config_submit_log_buffer

DESCRIPTION
  This function is called to post ant switch config submit command. If the 
  buffer is not ready to be submitted to diag the function returns without
  doing anything. This can so happen because multiple logs are instructed
  to be submitted to diag by the same L1 command.
  It checks for the log code in diag and submits the packet.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ant_switch_config_submit_log_buffer(void);

/*===========================================================================
FUNCTION ant_switch_config_submit_log_cmd

DESCRIPTION
  This function send ANT SWITCH CONFIG log packet local command to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ant_switch_config_submit_log_cmd(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_config_log_update

DESCRIPTION
  This function is used for the  ant switch config logging accumulation.
  The accumulation happens one time.
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
extern void wcdma_ant_switch_config_log_update(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_type1_switch_rscp_evt_logging

DESCRIPTION
  This function generates the event related to ASDiv feature. This is called 
  for logging the event EVENT_ASDIV_TYPE1_SWITCH_RSCP_BASED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wcdma_ant_switch_type1_switch_rscp_evt_logging(
     int16 probe_delta,int16 probe_threshold);

/*===========================================================================
FUNCTION wcdma_ant_switch_type1_switch_txagc_evt_logging

DESCRIPTION
  This function generates the event related to ASDiv feature. This is called 
  for logging the event EEVENT_ASDIV_TYPE1_SWITCH_TXAGC_BASED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wcdma_ant_switch_type1_switch_txagc_evt_logging(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_type1_switchback_mtpl_evt_logging

DESCRIPTION
  This function generates the event related to ASDiv feature. This is called 
  for logging the event EVENT_ASDIV_TYPE1_SWITCHBACK_MTPL_BASED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wcdma_ant_switch_type1_switchback_mtpl_evt_logging(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_type1_switchback_txagc_evt_logging

DESCRIPTION
  This function generates the event related to ASDiv feature. This is called 
  for logging the event EVENT_ASDIV_TYPE1_SWITCHBACK_TXAGC_BASED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wcdma_ant_switch_type1_switchback_txagc_evt_logging(int16 postswitch_txagc);

/*===========================================================================
FUNCTION wcdma_ant_switch_type2_switch_rscp_evt_logging

DESCRIPTION
  This function generates the event related to ASDiv feature. This is called 
  for logging the event EVENT_ASDIV_TYPE2_SWITCH_RSCP_BASED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void wcdma_ant_switch_type2_switch_rscp_evt_logging(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_type2_switch_mtpl_evt_logging

DESCRIPTION
  This function generates the event related to ASDiv feature. This is called 
  for logging the event  EVENT_ASDIV_TYPE2_SWITCH_MTPL_BASED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wcdma_ant_switch_type2_switch_mtpl_evt_logging(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_type2_switch_rscp_loopback_evt_logging

DESCRIPTION
  This function generates the event related to ASDiv feature. This is called 
  for logging the event  EVENT_ASDIV_TYPE2_SWITCH_RSCP_LOOPBACK_BASED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wcdma_ant_switch_type2_switch_rscp_loopback_evt_logging(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_type2_switchback_degradation_evt_logging

DESCRIPTION
  This function generates the event related to ASDiv feature. This is called 
  for logging the event  EVENT_ASDIV_TYPE2_SWITCHBACK_DEGRADATION_BASED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wcdma_ant_switch_type2_switchback_degradation_evt_logging(int16 dl_degradation,int16 ul_degradation);

/*===========================================================================
FUNCTION wcdma_ant_switch_type2_probe_both_antennas_evt_logging

DESCRIPTION
  This function generates the event related to ASDiv feature. This is called 
  for logging the event  EVENT_ASDIV_PROBE_BOTH_ANTENNAS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wcdma_ant_switch_type2_probe_both_antennas_evt_logging(uint32 rscp_pri,uint32 rscp_div);

/*===========================================================================
FUNCTION wcdma_ant_switch_metrics_log_init
 
DESCRIPTION This function initializes the log states for ant switch metrics 
            log packet
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wcdma_ant_switch_metrics_log_init(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_metrics_log_update

DESCRIPTION
  This function is used for the  wcdma ant switch metrics logging accumulation.
  
  It is called every frame to collect the data corresponding to the sample.
  
DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
void wcdma_ant_switch_metrics_log_update(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_metrics_stop_logging

DESCRIPTION
  This function prepares the wcdma ant switch metrics buffer to be submitted to diag
  It calls the function that posts a wcdma periodic stat log submit local command and then
  clears the state of wcdma ant switch metrics logging.
  
DEPENDENCIES
  wcdma_ant_switch_metrics_log_info_buf_idx should be valid while making
  a call to this function.

RETURN VALUE
  None

SIDE EFFECTS
  ===========================================================================*/
void wcdma_ant_switch_metrics_stop_logging(void);


/*===========================================================================
FUNCTION wcdma_ant_switch_metrics_send_log_submit_cmd

DESCRIPTION
  This function makes a request to post a local command to submit the 
  currently accumulated log packet to diag. Before it does so, it checks if 
  a previous log packet that was submitted to diag has been  serviced or not.
  If not, it drops the current packet and proceeds further.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void wcdma_ant_switch_metrics_send_log_submit_cmd(
  /* wcdma periodic stat log buffer index to submit */
  uint8 wcdma_ant_switch_metrics_log_info_buf_idx);

/*===========================================================================
FUNCTION wcdma_ant_switch_metrics_submit_log_buffer

DESCRIPTION
  This function is called to post wcdma ant switch metrics submit command. If the 
  buffer is not ready to be submitted to diag the function returns without
  doing anything. This can so happen because multiple logs are instructed
  to be submitted to diag by the same L1 command.
  It checks for the log code in diag and submits the packet.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Flag indicating the log buffer is submitted for flushing is reset.
===========================================================================*/
void wcdma_ant_switch_metrics_submit_log_buffer(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_metrics_submit_log_cmd

DESCRIPTION
  This function send ANT SWITCH METRICS submit log packet local command to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void wcdma_ant_switch_metrics_submit_log_cmd(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_algorithm_block

DESCRIPTION
  This function sets the FLAG to block the ASDiv algorithm.

DEPENDENCIES
  None

RETURN VALUE
  boolean. 
Returns TRUE if the algorithm can be blocked.
Returns FALSE if the algorithm cannot be blocked now.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean wcdma_ant_switch_algorithm_block(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_algorithm_unblock

DESCRIPTION
  This function resets the FLAG to unblock the ASDiv algorithm.

DEPENDENCIES
  None

RETURN VALUE
  boolean. 
Returns TRUE if the algorithm can be unblocked.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean wcdma_ant_switch_algorithm_unblock(void);

/*===========================================================================
FUNCTION wcdma_is_ant_switch_blocked

DESCRIPTION
  This function returns the FLAG VALUE of block_asdiv_flag

DEPENDENCIES
  None

RETURN VALUE
  boolean. 

SIDE EFFECTS
  None
===========================================================================*/

boolean wcdma_is_ant_switch_blocked(void);

/*===========================================================================
FUNCTION wcdma_ant_switch_toggled_value

DESCRIPTION
  This function returns the toggled value of the switch config.
  It doesnt toggle the global value of the switch config.

DEPENDENCIES
  None

RETURN VALUE
  trm_ant_switch_div_config_type. 

SIDE EFFECTS
  None
===========================================================================*/

trm_ant_switch_div_config_type  wcdma_ant_switch_toggled_value(void);

/*===========================================================================
FUNCTION wcdma_asdiv_disable_mtpl_algorithm

DESCRIPTION
  This function sets the global flag "asdiv_mtpl_algorithm_disable".
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wcdma_asdiv_disable_mtpl_algorithm(boolean value);


/*=========================================================================

 FUNCTION     :wl1_ant_set_switch_cb
 
 DESCRIPTION  : This is the callback function for the SGLTE ANT SWITCH to switch antenna
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
 	trm_ant_switch_set_mode_return_type set_mode_status,
 	trm_ant_switch_div_config_type switch_div_config

 RETURN VALUE : 
       
 SIDE EFFECTS : 
==========================================================================*/
extern void wl1_ant_set_switch_cb(trm_ant_switch_cb_info_type switch_info);

/*===========================================================================

FUNCTION wl1_ant_update_trm_about_state_transition

DESCRIPTION
  This function calls the TRM api and registers the callback for the antenna switch

DEPENDENCIES
  None.

RETURN VALUE
  uint8

SIDE EFFECTS
  None
 ===========================================================================*/
extern void wl1_ant_update_trm_about_state_transition(l1m_state_enum_type state);

/*===========================================================================

FUNCTION wl1_ant_switch_set_control_with_trm

DESCRIPTION
  This function calls the TRM api and registers the control for the antenna switch

DEPENDENCIES
  None.

RETURN VALUE
  uint8

SIDE EFFECTS
  None
 ===========================================================================*/

void wl1_ant_switch_set_control_with_trm(trm_ant_switch_set_ctrl_enum req_type);

/*===========================================================================

FUNCTION wl1_ant_switch_qta_state_update

DESCRIPTION
  This function updates the QTA state for antenna switch diversity and locks TRM ASDiv Switch.
  If the TRM ASDiv Switch lock fails, it returns FALSE else TRUE.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None
 ===========================================================================*/

extern boolean wl1_ant_switch_qta_state_update(boolean status);


/*===========================================================================

FUNCTION wl1_ant_switch_idle_wake_up

DESCRIPTION
  This function is called in wl1sleep module while waking up from Sleep

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None
 ===========================================================================*/
void wl1_ant_switch_idle_wake_up(void);
/*===========================================================================

FUNCTION wl1_ant_switch_oos_idle_sleep

DESCRIPTION
This function determines is sleep a result of OOS or Idle sleep. If it is OOS sleep then performs 
an antenna switch.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None
 ===========================================================================*/
void wl1_ant_switch_oos_idle_sleep(void);

/*===========================================================================

FUNCTION wl1_ant_switch_set_mode_switch_permission_with_trm_for_sleep

DESCRIPTION
  This function calls the TRM api and registers the callback for the antenna switch

DEPENDENCIES
  None.

RETURN VALUE
  uint8

SIDE EFFECTS
  None
 ===========================================================================*/

extern void wl1_ant_switch_set_mode_switch_permission_with_trm_for_sleep(l1_state_information_for_trm_type state);

#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
#else
/*=========================================================================

 FUNCTION     :wl1_as_trm_passive_switch_cmd
 
 DESCRIPTION  : This call back function is used by TRM to notify WL1 for INITATING an Antenna 
 Switch and indicating the Antenna Switch is COMPLETE.
              
 DEPENDENCIES : 

 INPUT PARAMETERS: trm_ant_switch_cb_info_type

 RETURN VALUE : void
       
 SIDE EFFECTS : 
==========================================================================*/

void wl1_as_trm_passive_switch_cmd(trm_ant_switch_cb_info_type ant_cfg_info);

#endif /*JOLOKIA MODEM*/

/*===========================================================================

FUNCTION wl1_ant_switch_idle_testmode

DESCRIPTION
  This function toggles the preferred antenna every ant_switch_dwelling_time sleep cycles.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
 ===========================================================================*/

void wl1_ant_switch_idle_testmode( );

/*===========================================================================
FUNCTION wl1_as_get_txagc_and_mpr_hist

DESCRIPTION
  This function is called from wl1 antenna switch controller and updates history
  table of txAGC values for past 15 slots in TX AGC units, taking into
  account HDET corrections and CM slots

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The average value of txagc and mpr from the past 15 slots will be calculated 
==========================================================================*/
void wl1_as_get_txagc_and_mpr_hist(int16 *temp_txagc_average, int16 *temp_mpr_average);

#endif /*Antenna Switch feature*/
#endif /* WL1ANTSWITCH_H ends */
