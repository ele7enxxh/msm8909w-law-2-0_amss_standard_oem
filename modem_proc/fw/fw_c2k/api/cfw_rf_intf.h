/*!
  @file
  cfw_rf_msg.h

  @brief
  C2K Firmware RF Interface Common Definitons

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/
/** 
  @mainpage Nike-L C2K Common Firmware Interface Document
  @authors Kai Ting, Vijay Ramasami
  @section intro Revision
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_c2k/api/cfw_rf_intf.h#1 $
  @section whatsnew What's New
*/


#ifndef _CFW_RF_INTF_H_
#define _CFW_RF_INTF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"
#include "rfcmd_msg.h"

#ifdef __cplusplus
}  // extern "C"
#endif


/*============================================================================
 *
 * DOXYGEN GROUPINGS
 *
 * ==========================================================================*/
/**
 * \defgroup cfwrxagc RxAGC and LNA Interface
 * \defgroup cfwrxtune RX Tune and RxLM Interface
 * \defgroup cfwtxtune TX Tune and TxLM Interface
 * \defgroup cfwrfconst RF Tune Interface Constants
 * \defgroup cfwrflinconst RF linearizer Constants
 * \addtogroup cfwrxtune
 * @{
 *    \defgroup cfwrxtunemsg Command Message Definitions
 *    \defgroup cfwrxtunersp Response Message Definitions
 *    \defgroup cfwrxmisc Common Structure Definitions.
 *    \defgroup cfwrxtunecommon Miscellaneous
 * @}
 */

/** 
 * \addtogroup cfwrflinconst RF linearizer Constants
 * @{
 */

/*!@brief Defines total number transactions for allocating memory in
 * linearizer table */
/* Whenever a new type of transaction is added, make sure that the below
 * macros are defined as max value among the types of same transaction. 
 * i.e if in future Q_CUR task gets SSBI transactions, then
 * CFW_NUM_PA_Q_CUR_REG_WRITES_DEFINED should be the max of
 * CFW_RF_TX_TPL_PA_Q_CUR_NUM_RFFE and CFW_RF_TX_TPL_PA_Q_CUR_NUM_SSGI
 * - This ensures that any change/addition to template doesn't affect the
 *   linearizer interface */

#define CFW_NUM_PA_Q_CUR_REG_WRITES_DEFINED CFW_RF_TX_TPL_PA_Q_CUR_NUM_RFFE 
#define CFW_NUM_QPOET_REG_WRITES_DEFINED    CFW_RF_TX_TPL_APT_NUM_RFFE 
// PA writes are available as RFFE and SSBI transactions, since num_rffe (8) > num_ssbi (4) 
// the writes are defined by max value (num_rffe).
#define CFW_NUM_PA_REG_WRITES_DEFINED       CFW_RF_TX_TPL_PA_NUM_RFFE 
#define CFW_NUM_AVGA_REG_WRITES_DEFINED     CFW_RF_TX_TPL_AVGA_NUM_SSBI  

/**@}*/

/** 
 * \addtogroup cfwrfconst RF Interface Constants 
 * @{
 */
/*!< Defines the total # of RX antennas */
#define CFW_NUM_ANTENNAS                      2

/*!@brief Defines total number of LNA states */
/* Do NOT change this value from 5, the LNA FSM implementation relies on this assumption*/
#define CFW_NUM_LNA_STATES                    5

/*!< Max # of tasks supported by LNA RF events */
#define CFW_MAX_LNA_TASKS_PER_STATE           2

/*!< Max # of payload pairs supported by the LNA SM (over *all* tasks) */
#define CFW_MAX_LNA_PAYLOAD_PAIRS_PER_STATE   2

/**@}*/

/** 
 * \addtogroup cfwrfevents RF Event Scripts
 * @{
 */

/*! @brief Number of SBI addr/ data pairs in the buffer */
#define CFW_RF_EVENT_MAX_NUM_OF_SBI    8 

/*! @brief Number of RFFE addr/ data pairs in the buffer */
#define CFW_RF_EVENT_MAX_NUM_OF_RFFE   8

/*! @brief Payload buffer size for cfw_rf_event_infor_type */
#define CFW_RF_EVENT_DATA_BUF_SIZE_WORD ((CFW_RF_EVENT_MAX_NUM_OF_SBI*2 + \
                                              CFW_RF_EVENT_MAX_NUM_OF_RFFE*2 + 3)/4)

/*! @brief Number of static script buffer location provided in 
 * 1x/DO's shared memory */
#define CFW_RF_STATIC_EVENT_TECH_MEM_NUM_SCRIPTS 1

/*!@brief set RF script index to >=16 to indicate that FW should take scripts from
 * 1x or DO's shared memory instead of the FW RF common IRAT script buffer. In this
 * case the actual index is "index - CFW_RF_STATIC_EVENT_TECH_MEM_IDX_MIN" */
#define CFW_RF_STATIC_EVENT_TECH_MEM_IDX_MIN 16

/*!
  @brief
  Data structure for passing event based short scripts to CFW.
*/
typedef struct
{
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[CFW_RF_EVENT_DATA_BUF_SIZE_WORD];
} ALIGN(4) cfw_rf_event_infor_type;

/*! @brief RF dynamic events (sent during call) used for 1x and DO */

typedef struct {

  /*! Script for PA, TX and potentially DAC on events. Timing is 
      0 referenced at PA/TX/DAC turn on time */
  cfw_rf_event_infor_type tx_rf_on;

  /*! Script for PA, TX and potentially DAC off events */
  cfw_rf_event_infor_type tx_rf_off;

  /*! Use this script when sending CFW_HDET_MEAS_RF_SCRIPT in
   * cfw_tx_hdet_request_msg_t*/
  cfw_rf_event_infor_type tx_hdet_meas;

  /*! Use this script when sending CFW_HDET_MEAS_ANT_TUN_RF_SCRIPT in
   * cfw_tx_hdet_request_msg_t*/
  cfw_rf_event_infor_type hdet_ant_tun;

} cfw_rf_events_dynamic_set_t;

/*! @brief RF dynamic events (sent during call) used for 1x and DO */

typedef struct {

  /*! Script for D-LNA RF off*/
  cfw_rf_event_infor_type dlna_rf_off;

  /*! Script for D-LNA RF on*/
  cfw_rf_event_infor_type dlna_rf_on;

} cfw_rf_rx_events_dynamic_set_t;



/*! @brief RF events used for 1x and DO */
typedef struct
{
  /*! RF events for LNA state machine (one event per state). 
   * 
   * Restrictions:
   * 
   * 1. It is expected that the first task in the event is a SSBI write transaction, and 
   *    that the first task is used to trigger the SBI_DONE event.
   * 2. The number of tasks per event should be <= CFW_MAX_LNA_TASKS_PER_STATE
   * 3. The # of payload pairs (over all tasks) per event should be <= CFW_MAX_LNA_PAYLOAD_PAIRS_PER_STATE.
   * 4. The size of the ISSUE_SEQ generated from such an event should be less than 128
   *    bytes (otherwise, FW will crash with an appropriate error message).
   *
   */
  cfw_rf_event_infor_type lna[CFW_NUM_ANTENNAS][CFW_NUM_LNA_STATES];

  union {
    struct {
      /*! Script for PA, TX and potentially DAC on events. Timing is 
        0 referenced at PA/TX/DAC turn on time */
      cfw_rf_event_infor_type tx_rf_on;

      /*! Script for PA, TX and potentially DAC off events */
      cfw_rf_event_infor_type tx_rf_off;

      /*! Use this script when sending CFW_HDET_SCHED_SCRIPT in
       * cfw_tx_hdet_request_msg_t*/
      cfw_rf_event_infor_type tx_hdet_meas;

      /*! Use this script when sending CFW_ANT_TUN_SCHED_SCRIPT in
       * cfw_tx_hdet_request_msg_t*/
      cfw_rf_event_infor_type hdet_ant_tun;

    };
    cfw_rf_events_dynamic_set_t rf_dynamic;
  };

  union {
    struct {
      // RF on/off scripts for primary antenna
      cfw_rf_rx_events_dynamic_set_t rf_rx_dynamic;

      // RF on/off scripts for diversity antenna
      cfw_rf_rx_events_dynamic_set_t rf_rx_dynamic_div;
    };

    // RF on/off events for primary and diversity antennas
    cfw_rf_rx_events_dynamic_set_t rf_rx_events_dynamic[CFW_NUM_ANTENNAS];
  };

} ALIGN(4) cfw_rf_events_set_t;

/**@}*/
#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY

/*! @brief Antenna Switch Execute Command
*
* L1 command to tell FW to schedule an antenna switch. 
* It is not valid to send ASDIV command between DAC_START and
* DAC_START_RSP as well as DAC_STOP and DAC_STOP_RESP
*
* It is not valid to send this message when the previous antenna switch is still pending.
* FW will crash with a error message when these scenarios happen.
*
*/
typedef struct {
 
  /*! MSGR header */
  msgr_hdr_struct_type hdr;
  
} cfw_rf_execute_ant_switch_msg_t; 

#endif

/** 
 * \addtogroup cfwrfconst RF Interface Constants 
 * @{
 */
/*! @brief CFW Logical Antenna Definitions */
typedef enum {
  CFW_PRIMARY_ANTENNA = 0,
  CFW_SECONDARY_ANTENNA
} cfw_logical_antenna_t;

/** 
 * \addtogroup Constants to denote xpt mode for a PA state.
 * @{
 */
/*! @brief */
typedef enum {
  CFW_APT_MODE = 0,
  CFW_EPT_MODE,
  CFW_ET_MODE
} cfw_xpt_mode_t;

/*! \brief Common Defs for 1x and DO. Used internally in FW for now. RFSW can
 * switch later */

/*! \brief Number of PA states
 * */
#define CFW_NUM_PA_STATES 4

/*! \brief Number of PA state switch points
 * */
#define CFW_NUM_PA_STATE_THRESH_PARAMS (2*(CFW_NUM_PA_STATES-1))

/*! \brief Number of rows of Lin table for each PA state
 * */
#define CFW_RFA_LUT_SIZE 64

/* ******************************************************************** */
/*! \brief The following CDMAFW.* and HDRFW.* defintions are to be removed
 * once RFSW switches to the common definitions above. */
/* ******************************************************************** */

/*! \brief # of PA states */
#define CDMAFW_NUM_PA_STATES              CFW_NUM_PA_STATES

/*! \brief # of PA state threshold params */
#define CDMAFW_NUM_PA_STATE_THRESH_PARAMS CFW_NUM_PA_STATE_THRESH_PARAMS 

/*! \brief # of LUT entries for RFA LUT */
#define CDMAFW_RFA_LUT_SIZE               CFW_RFA_LUT_SIZE 

/*! \brief # of PA states */
#define HDRFW_NUM_PA_STATES               CFW_NUM_PA_STATES

/*! \brief # of PA state threshold params */
#define HDRFW_NUM_PA_STATE_THRESH_PARAMS  CFW_NUM_PA_STATE_THRESH_PARAMS 

#define HDRFW_RFA_LUT_SIZE                CFW_RFA_LUT_SIZE 

/*! # Max # of SSBI words for HDET events. The number of words actually used
 * is SW-programmable.*/
#define CFW_MAX_HDET_EVENT_SCRIPT_SIZE    3

/*! # of SSBI words for PDET on configuration for HDET */
//RK:todo, the following there should be removed in sync with SW.
#define CFW_HDET_PDET_ON_SCRIPT_SIZE    3

///*! # of SSBI words for HKADC conversion configuration for HDET */
#define CFW_HDET_HKADC_TRIG_SCRIPT_SIZE CFW_HDET_PDET_ON_SCRIPT_SIZE    

///*! # of SSBI words for PDET off configuration for HDET */
#define CFW_HDET_PDET_OFF_SCRIPT_SIZE   CFW_HDET_PDET_ON_SCRIPT_SIZE    

/*!@brief Defines total number of WB chains.*/
#define CFW_URXFE_NUM_WB                4

// This defines the MAX number of WB chains across BOLT/BOLT+ etc.
#define CFW_URXFE_NUM_WB_MAX            4

/*!@brief Defines total number of WB chains supported by HDR. */
#define HDRFW_URXFE_NUM_WB              2

/*!@brief Defines total number of NB chains supported in AGC configuration interface*/
#define CFW_URXFE_MAX_NB_PER_WB         3

/*!@brief Defines loop order of FGDC */
#define CFW_FGDC_LOOP_ORDER                2

/*!@brief Defines NULL script index */
#define  CFW_NULL_SCRIPT_IDX            0xFFFFFFFF
/*!@brief Number of RGI's EPT is supported on*/
#define CFW_EPT_MAX_NUM_RGI             20 

/*!@brief Defines total number of carriers supported for PA Backoff for DO */
#define HDRFW_PA_BACKOFF_CARRIERS        3
/**@}*/

/** 
 * \addtogroup cfwrxmisc
 * @{
 */

/*! \brief RX_STOP timed trigger status */
typedef enum {
  RX_STOP_SUCCESS,
  RX_STOP_FAILURE
} cfw_rf_timed_rx_stop_status_e;

/*! \brief RX config response structure */
typedef struct {

  /*! \brief Message received time stamp. Cx2 RTC for 1x, HSTR count for DO */
  uint32                    msg_rcvd_time_stamp;
   
  /*! \brief Stream Trigger Time Stamp. Time at which chain activation was scheduled. */
  uint32                    stream_trigger_time_stamp;

  /*! USTMR Activation Time.
   *
   * USTMR time stamp at which sample streaming was triggerred. Useful for logging purposes.
   */
  uint32                    ustmr_activation_time;

  /*! VSRC Initialization Count
   *
   * Initial load value of VSRC in Cx32. This is the same as the RTC time at which sample streaming
   * started. Useful for logging purposes.
   */
  uint32                    vsrc_init_count_cx32;

  /*! Validity of RX_STOP trigger time for timed trigger. Set to FAILURE if RX_STOP trigger time 
      has expired or is >5ms in the future */
  cfw_rf_timed_rx_stop_status_e timed_stop_status;

  /*  Sequence No. provided in CMD message */
  uint32                        sequence_number;

  /*! \brief Done time stamp when response is sent. Same definition as start time_stamp */
  uint32                    done_time_stamp;

  /*! \brief RxFE Trigger Value.
   *
   * The value of RXFE_Cn_TRIG_RD after STMR trigger. This should match USTMR activation time
   * for VSRC and RTC to match each other.
   */
  uint32                   rxfe_trig_rd;

} cfw_rx_config_rsp_info_t;

/*! Common RX configuration structured used in start/stop/intelliceiver update messages. */
typedef struct {
  
  /*! Antenna Index. This is the logical antenna associated with the WB index embedded in 
      the RxLM buffer. The association should have been established with RX_AGC_CFG message.
      This is a sanity check/convenience for FW. */
  uint32                    antenna;

  /*! RxLM Buffer Index */
  uint32                    rxlm_buffer_idx;
  
  /*! RF script index. 0xFFFFFFFF (msb=1, -ve) to indicate firmware should not program 
      any RF scripts. In this mode firmware still counts the settling time. 

      >= 16 script index indicates that firmware should use per tech's shared memory
      cfw_rf_events_set_t::rf_state[rf_script_idx - 16] as script buffer.

   */
  uint32                    rf_script_idx;
  
  /*! Cx1 delta from start time to settling of RF tune script*/
  uint16                   settling_rtc_cx1;

  /*! User specified sequence number */
  uint32                   sequence_number;

} cfw_rx_config_msg_core_t;

/* CFW Rx start message internal definition.  This is basically rx_start_cfg and rx_start combined.
   Not exposed to software.
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  cfw_rx_config_msg_core_t  rx_config;
 
  /*! Start Time - must be within 10ms of current system time. */
  uint32                    rtc_cx1_trigger;

  /*! Immediate Start Flag */
  uint16                    immediate_trigger;

} cfw_rx_start_internal_msg_t;
/**@}*/

/**
 * \addtogroup cfwrxtunemsg
 * @{
 */
/*! CFW Rx start config message definition. Configuration only, actual action gated by RX_START's trigger mode.
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  cfw_rx_config_msg_core_t      rx_config;

} cfw_rx_start_cfg_msg_t;


/*! CFW Rx start message definition. This message is used to initiate RF tune and U-RxFE configuration through
    supplied RF tune and RxLM buffers. Message implies start of RxAGC after specified delay from start time.
    For non-immediate start, rtc_cx1_trigger must be within 10ms of current system time.
    Initial CSR_OFFSET should be passed in through RxLM buffer. RX_START_CFG message must be sent prior to this 
    message.

    UMID: CDMA_FW_RX_START_CMD, HDR_FW_RX_START_CMD
    Response to SW: CDMA_FW_RX_START_RSP, HDR_FW_RX_START_RSP \sa cfw_rx_start_rsp_msg_t

    Indication to SW: HDR_FW_RXAGC_TRACK_IND after ACQ->Track AGC state transition. Does not apply to 1x.
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! Start Time - must be within 10ms of current system time. */
  uint32                    rtc_cx1_trigger;

  /*! Immediate Start Flag */
  uint16                    immediate_trigger;

  /*! User specified sequence number */
  uint32                    sequence_number;
} cfw_rx_start_msg_t;

/**@}*/

/**
 * \addtogroup cfwrxagc
 * @{
 */

/*! RxAgc Acq params for DO.
 */
typedef struct {
  /*! AGC Accum Init value in FW Units to put LNA to state 3 */
  int16  lnaState3AgcAccumInit;
  /*! AGC Accum Init value in FW Units to put LNA to state 2 */
  int16  lnaState2AgcAccumInit;
} mcdo_rxagc_acq_params_t;

/*! \brief DO RX_AGC_CFG force timing modes */
typedef enum {
  /*! setting 0 will clear the force timing flag in DO FW and
      we will start doing unlocked AGC if no fingers are in lock */
  MCDO_RXAGC_NO_TIMING = 0,

  /*! setting 1 will force DO to use the new timing provided in 
      rtc_offset field */
  MCDO_RXAGC_FORCE_TIMING,

  /*! setting 2, will preserve the previous timing mode being used 
      for AGC */
  MCDO_RXAGC_PRESERVE_TIMING

} mcdo_rxagc_force_timing_modes_e;

/*! CFW RxAGC configuration message. This message only sets up parameters for RxAGC. 
    This does not imply the start or stop of RxAGC. RxAGC start is mostly implied
    by other configuration messages (such as RX_START) where RxAGC information
    is deposited earlier than the required start action.
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Logical antenna index. 0=Primary, 1=Diversity */
  uint32                    antenna;

  /*! \brief Associated WB chain index. This field and antenna above defines
      the relationship between logical antenna and the physical wideband chain */
  uint32                    wb_idx;

  /*! RTC offset for fast acquisition. DO only? This is not a per-chain information !
      Value specified here will override any previous rtc_offset passed. */
  uint32                    rtc_offset;

  /*! Force timing. If this flag is set to 1, system time comes from rtc_offset field
      NOTE: both antennae use a single timing mode in DO */
  mcdo_rxagc_force_timing_modes_e force_timing;

  /*! Acquisition gain */
  int16                     acq_gain;
 /*! Acquisition duration units is half-slots for DO and 64-chip units for 1x */ 
  uint32                    acq_duration;
  /*! Tracking gain */
  int16                     trk_gain;
 /*! Set Point  - if we desire a value other than the ideal of 288 for 6 bits I/Q*/
  int16                     set_point;
  /*! RxAGC Filter gain in Q15 - 
      For TC = t ms, DO gain = (1.67/t)*2^15; 1x gain = (1.25/t)*2^15. 
      This is latched when accum_latch is set (which is when even accum_init 
      is latched into AGCAccum */
  int16                     agc_filt_gain;                                   
  /*! Initial AGC accumulation values for each NB chain. */
  int16                     accum_init[CFW_URXFE_MAX_NB_PER_WB];
 /*! accumulator latch,depending on bits 0,1,or 2 set one or more accumulators
     initialized */
  uint32                    accum_latch;
  /*! sequence number */
  uint32                    sequence_number;
  
  /*! Params for DO RxAGC */
  mcdo_rxagc_acq_params_t   rxagc_acq_params;
  
} cfw_rx_agc_cfg_msg_t;

/*! LNA STATE override for callibration purposes. FSM bypassed. When this message is received, 
    only if the LNA state is different from the current, SSBI sent out and the LNA sequence
    will be executed. FSM bypassed till a message is sent with the flag set to zero*/
typedef struct{
    /*! \brief Message header */
    msgr_hdr_struct_type      hdr;
    /*! Antenna for which override message is sent */
    int32 antenna;
    /*! Override Mode 1=override, 0=normal*/
    int32 override_flag;
    /*! Override value */
    int16 override_state;
} cfw_lna_override_msg_t;

/*! For calibration purposes. Contains calibration gain offset for a state */
typedef struct{
    /*! \brief Message header */
    msgr_hdr_struct_type      hdr;
    /*! Antenna for which msg sent */
    int32 antenna;
    /*! Narrow band offset w.r.t NB Base Idx for this WB, i.e nb_offset = nbIdx - 3*WbIdx to limit to (0,1,2)
     *  For 1X This field is ignored
     * */   
    int32 nb_offset;
    /*! The gain offset value */
    int16 gain_offset;
    /*! State for which offset is provided */
    int16 state;
} cfw_lna_gain_offset_msg_t;
/*! CFW RxAGC stop message */

typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! \brief Structure indicating which NB chains to have AGC disabled. */
  union {
    struct {
      uint32 nb_idx_0:4; /*!< Set to 0xF for disable */
      uint32 nb_idx_1:4; /*!< Set to 0xF for disable */
      uint32 nb_idx_2:4; /*!< Set to 0xF for disable */
      uint32 ant_idx:4;   /*!< Antenna to apply the disable */
      uint32 unused:16;
    };
    uint32 intval;
  } chain_info;
} cfw_rx_agc_stop_msg_t;

/**@}*/

/**
 * \addtogroup cfwrxtunemsg
 * @{
 */
/*! CFW RX stop message. Implies RxAGC stop.
 
    UMID: CDMA_FW_RX_STOP_CMD, HDR_FW_RX_STOP_CMD
    Response to SW: CDMA_FW_RX_STOP_RSP, HDR_FW_RX_STOP_RSP \sa cfw_rx_stop_rsp_msg_t

 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! RF tune/RxLM config information */
  cfw_rx_config_msg_core_t      rx_config;
  
  /*! Stop Time - must be within 10ms of current system time. */
  uint32                    rtc_cx1_trigger;

  /*! Immediate Stop Flag */
  uint32                    immediate_trigger;

  /*! @brief OQPCH Search Flag
   * 
   * Set this flag to TRUE (1) to indicate that this is a RX_STOP intended for OQPCH. When
   * this flag is set, default RX_STOP behaviour will change as follows:
   * 
   * - Mempool lines will not be freed during message processing. This allows offline
   *   searches to be run once RX_STOP finishes its operation. Once OQPCH searches are 
   *   finished, SW must send a OQPCH_SEARCH_DONE message to indicate FW that the Mempool
   *   contents are no longer needed and the mempool can be freed.
   *   
   * - FW will use TX Task Queue to process RX_STOP message. This implies that we cannot
   *   send a OQPCH search command while TX is active (a reasonable assumption). FW will
   *   ASSERT when this happens.
   *
   * Set this flag to FALSE (0) for default RX_STOP operation.
   *
   * \sa cdmafw_oqpch_search_done_msg_t
   */
  uint32                    oqpch_srch_flag;

} cfw_rx_stop_msg_t;

/*! @brief CFW Intelliceiver update message. 
 *
 * Assumes RxAGC stop message already received. Implies resume of AGC after settling time.
 *
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  cfw_rx_config_msg_core_t      rx_config;

  /*! Start Time - must be within 10ms of current system time. */
  uint32                    rtc_cx1_trigger;

  /*! Immediate Start Flag */
  uint32                    immediate_trigger;

} cfw_intelliceiver_update_msg_t;

/*! @brief CFW Dynamic TXC update message. 
 *
 * FW updates DP IQ gain on receiving this message. Done asynchronously to
 * the on-going TX operations.
 *
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! TXC configuration buffer index from TxLM */
  uint32 txlm_buffer_idx;

  /*! TXC configuration mask. Should take only the following three values:
   *  FW_TXLM_TXC0_DP_IQ_GAIN_MASK => TXC0 will be updated 
   *  FW_TXLM_TXC1_DP_IQ_GAIN_MASK => TXC1 will be updated 
   *  FW_TXLM_TXC0_DP_IQ_GAIN_MASK | FW_TXLM_TXC1_DP_IQ_GAIN_MASK => Both
   *  TXC0 and TXC1 will be updated.*/
  uint32 txc_update_mask;

} cfw_dynamic_txc_update_msg_t;

/*!
 * @brief EPT DPD data config message type. Indicates sharedmem updates are
 * available for firmware to process. In calibration mode, only
 * dpdDataInfoUpdate is expected to be TRUE because FW maintains the dpdData
 * table internally. In online mode, both dpdDataInfoUpdate and dpdDataUpdate
 * are expected to be set because the DPD data comes from SW in online mode.
 *
 * eptEnable when set to TRUE enables EPT feature in FW in online mode.
 * To enable EPT in CAL mode use cdmafw_ept_override_msg_t
 *
 * When sent to HDR FW, this message is associated with hdrfw_ept_dpd_data_config_t
 * When sent to 1x FW, this message is associated with cdmafw_ept_dpd_data_config_t
 *
 */
typedef struct {

  /*! @brief Message header */
  msgr_hdr_struct_type hdr;

  /*! @brief Set to true if dpdDataInfo is updated in SW writable shared mem*/
  boolean dpdDataInfoUpdate;

  /*! @brief Set to true if dpdData is updated in SW writable shared mem*/
  boolean dpdDataUpdate;

  /*! @brief Set to true to enable EPT */
  boolean eptEnable;

  /*! @brief ENV scale used for EPT RGIs. This is irrelevant when APT is in
   * use.*/
  uint16 envScale;

  /*! @brief mem_address and mem_size are used only in cal mode. In online
   * mode FW ignores these.  These provide the buffer address and size for
   * sample capture.*/
  uint32 mem_address;
  uint32 mem_size;

} cfw_ept_config_msg_t;

/* !
 * @brief EPT sample capture can happen in firmware only memory or in the SW
 * allocated memory. This message allows for TLB mapping of SW allocated
 * memory.
 * */
typedef struct {
 /*! @brief Message header */
  msgr_hdr_struct_type hdr;
  /*! @brief S/W mem address for IQ sample capture - mapped to FW address
   * space */
  uint32 mem_address;

  /*! @brief S/W mem size for IQ sample capture - mapped to FW address
   * space */
  uint32 mem_size;
} cfw_ept_sample_buffer_cfg_msg_t;

/*! @brief IQ_CAPTURE
 *
 *   This is the message definition for IQ_CAPTURE Configuration
 */
typedef struct {
   
  /*! @brief 0 if primary,  1 indicates for diversity */
  uint32  antSel;
  
  /*! @brief User data to be returned back in response message */
  uint32  userData;
} cfw_iq_capture_cfg_t;

/*@{*/
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  /*! \brief IQ Capture configuration */
  cfw_iq_capture_cfg_t cfg;
} cfw_iq_capture_msg_t;                     

/*! @brief IQ_CAPTURE Response
 *
 *   This is the message definition for IQ_CAPTURE Response
 */
typedef enum {
  CFW_IQ_CAPTURE_SUCCESS=0,
  CFW_IQ_CAPTURE_FAILURE=0xFF
} cfw_iq_capture_status_e;

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  /*! \brief Status of IQ_CAPTURE command */
  cfw_iq_capture_status_e status;
  
    /*! \brief Start address of IQ capture buffer in shared memory. */
  uint32  startAddress;

  /*! @brief User data that was set in IQ capture command */
  uint32  userData;
} cfw_iq_capture_rsp_msg_t;

/*@}*/

/*! CFW dynamic rxfe update message.
*/
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

  /*! RxLM Buffer Index */
  uint32                    rxlm_buffer_idx;

} cfw_urxfe_dyn_update_msg_t;

/**@}*/

/**
 * \addtogroup cfwrxtunersp
 * @{
 */
/*! 
 * @brief RX_START Message Response 
 *
 * \sa cfw_rx_start_msg_t
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  cfw_rx_config_rsp_info_t rsp_info;

} cfw_rx_start_rsp_msg_t;
  
/*! 
 * @brief IRAT_RX_START Message Response 
 *
 * \sa cfw_irat_rx_start_msg_t
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  cfw_rx_config_rsp_info_t rsp_info;

} cfw_irat_rx_start_rsp_msg_t;
  
/*! 
 * @brief RX_STOP Message Response 
 *
 * \sa cfw_rx_stop_msg_t
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;
  
  cfw_rx_config_rsp_info_t rsp_info;

} cfw_rx_stop_rsp_msg_t;
 
/** 
 * @brief INTELLICEIVER_UPDATE response.
 *
 * \sa cfw_intelliceiver_update_msg_t 
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  
  cfw_rx_config_rsp_info_t rsp_info;

} cfw_intelliceiver_update_rsp_msg_t;

/** 
 * @brief Response to cfw_tx_dac_cal_config_msg_t
 *
 * \sa Sent when the calibration is done.
 */
typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;

} cfw_dac_cal_config_rsp_msg_t;

/**@}*/

/** 
 * \addtogroup cfwrxagc
 * @{
 */

/*! CFW LNA state machine configuration shared memory interface.
 */
typedef struct ALIGN(32) {
  /*! Rise Threshold  */
  union {
    int16                     rise_threshold[CFW_NUM_LNA_STATES-1]  __attribute__ ((aligned(8)));
    uint64                    rise_threshold_vect;
  };

  /*! Fall Threshold  */
  union {
    int16                     fall_threshold[CFW_NUM_LNA_STATES-1]  __attribute__ ((aligned(8)));
    uint64                    fall_threshold_vect;
  };
  /*! Bypass Timer - in units of half-slots in case of DO, in units of 64 chips in case of 1x. */
  uint32                    bypass_timer_values[CFW_NUM_LNA_STATES-1];
  /*! Non-bypass Timer - in units of half-slots in case of DO, in units of 64 chips in case of 1x. */
  uint32                    nonbypass_timer_values[CFW_NUM_LNA_STATES-1];

  /*! LNA offset - per nb i/f. For 1x use index 0. For DO, index this with carrier index.
    The offset is applied to the assigned narrowband of the carrier automatically.
    */
  int16                     cal_gain_offset[CFW_URXFE_MAX_NB_PER_WB][CFW_NUM_LNA_STATES];

  /*! LNA offset - legacy interface for 1x*/
  //int16                     calibrated_gain_offset[CFW_NUM_LNA_STATES]; 
  /*! Per LNA state AGC accum min values. */
  int16                     agc_min[CFW_NUM_LNA_STATES];
  /*! AGC accum max values. */
  int16                     agc_max;
  /*! DC Acquisition duration: Acq gain is applied as soon as the SSBI is sent out
    assumption is that duration is the total lump some of time to sbi_done + DC delay +
    actual acq duration from the time the accumalors cleared - units of cx8*/  
  uint32                    dc_acq_duration;
  /*! DC FG est1 left shift value for acq */
  uint32                    dc_acq_shift_val1;
  /*! DC FG est2 left shift value for acq */
  uint32                    dc_acq_shift_val2;
  /*! DC FG est1 left shift value for trk */
  uint32                    dc_trk_shift_val1;
  /*! DC FG est2 left shift value for trk */
  uint32                    dc_trk_shift_val2;
  /*! FGDC accum init I/Q : This is for the implementation where we do not re-acquire DC*/
  int64                     fgdc_accum_init_i[CFW_FGDC_LOOP_ORDER][CFW_NUM_LNA_STATES];
  int64                     fgdc_accum_init_q[CFW_FGDC_LOOP_ORDER][CFW_NUM_LNA_STATES];
  /*! CSR Phase offset per LNA gain state */
  int32                     csr_phase_offset[CFW_NUM_LNA_STATES];
  /*! DVGA update delay after SBI_DONE - RXFE_NBn_DVGA_CFG:UPDATE_DELAY */
  int32                     dvga_sbi_delay;
  /*! WBDC update delay after SBI_DONE - RXFE_WBw_WBDC_FG_UPDATE_DELAY:UPDATE_DELAY*/
  int32                     wbdc_sbi_delay;
  /*! CSR update delay after SBI_DONE - RXFE_NBn_CSR_CFG:DELAY_VALUE*/
  int32                     csr_sbi_delay;
  /*! SSBI Master ID */
  uint32                    SSBI_master_ID; 
  /*! SSBI address and data for each gain state */
  uint8                     ssbi_addr;
  uint8                     ssbi_data[CFW_NUM_LNA_STATES];

} cfw_rf_LNA_interface_t;

/**@}*/

/**
   \defgroup cfwtx RF-FW Tx Interface
    */
/*@{*/

/**
   \defgroup cfwtxstsp Tx Start/Stop Messages
    */
/*@{*/


/*! TX_START message interface.
    
    UMID: CDMA_FW_TX_START_CMD, HDR_FW_TX_START_CMD
    Response to SW: CDMA_FW_TX_START_RSP, HDR_FW_TX_START_RSP \sa cfw_tx_start_msg_t

 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  /*! TBD (Optional): TX chain/band number (will be extraced from TxLM once 
   * an API is available) */
  uint16 tx_chain;

  /*! TxLM buffer index */
  uint32 txlm_buffer_idx;

  /*! RF script index. 0xFFFFFFFF (msb=1, -ve) to indicate firmware should not program 
      any RF scripts. In this mode firmware still counts the settling time. 

      >= 16 script index indicates that firmware should use per tech's shared memory
      cfw_rf_events_set_t::rf_state[rf_script_idx -16] as script buffer.
   */

  uint32 rf_script_idx;
  
  /*! TX stop script sent at TX start for STX dsda*/
  uint32 stx_rf_stop_script_idx;
  
  /*! UNUSED !! */
  uint16 start_rtc_cx1;

  /*! User specified sequence number  */
  uint32 sequence_number;

} cfw_tx_start_msg_t;

/*! TX_DAC_START message interface.
    Trigger the DAC specified in TxLM buffer. Must be sent after TX_START.
    No response sent to SW.

    In case of HDR, PN long code state and long code masks (i and q) for all
    carriers are latched at DAC_START. See hdrfw_tx_pn_long_state_cfg_t.
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  /*! STMR trigger time. FW will use this to configure the STMR trigger
   * that starts the TX chain. Set this according to the contents of the
   * prefill buffer. If prefill buffer data starts at the frame boundary,
   * then set this to the upcoming frame boundary.
   *
   *
   * This field allows TX trigger time specification in Cx8 resolution.
   * RTC count at TX trigger.
   * This parameter specifies the "RTC" time at which TX is triggered.
   * This time need not correspond to a frame boundary, but it needs to meet
   * the following conditions:
   * - The initial PN state and LC state specifications *must* match this
   *   trigger time.
   * - It must correspond to atleast a 1/2 PCG boundary (in resolution).
   * - It must be atleast 2 PCGs ahead of the present RTC to avoid problems
   *   with missing the TX trigger time which can result in PN mismatch and
   *   timing errors.
   *
   * @note This field is use by 1xFW to trigger the chain on Jolokia.
   * 
   */
  uint32 start_rtc_cx8;

  /*! RTC count at a frame boundary in Cx8.
   * This parameter specifies the "RTC" time that corresponds to any frame boundary
   * (an example: TX SYNC80 = RX SYNC80 - BTF, or can be zero-offset SYNC20 boundary)
   * This parameter is only used to initialize the TX VSTMRs and doesn't impact the
   * modulator configuration. SW can set this to whichever RTC that should reset frame,
   * PCG and symbol counters.
   *
   * This parameter along with start_rtc_cx8 establishes all TX timing. A mismatch
   * between these two RTCs can result in FW and SW being out-of-sync with the
   * modulator with respect to frame timing.
   * If txTriggerRTC == frameBndryRTC == TX SYNC 80 => we revert to legacy (pre-Bolt)
   * behavior.
   */
   uint32 frame_bndry_rtc_cx8;

  /*! STMR trigger time. FW will use this to configure the STMR trigger
   * that starts the TX chain. Set this according to the contents of the
   * prefill buffer. If prefill buffer data starts at the frame boundary,
   * then set this to the upcoming frame boundary 
   *
   * This field allows TX trigger time specfication in Cx1 resolution.
   * 
   * @note Ignored by 1X FW.
   * 
   */
  uint32 start_rtc_cx1;

} cfw_tx_dac_start_msg_t;


/*! TX_STOP message interface.
    UMID: CDMA_FW_TX_STOP_CMD, HDR_FW_TX_STOP_CMD
    Response to SW: CDMA_FW_TX_STOP_RSP, HDR_FW_TX_STOP_RSP \sa cfw_tx_stop_rsp_msg_t
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  /*! TBD (Optional): TX chain/band number (will be extraced from TxLM once 
   * an API is available */
  uint16 tx_chain;

  /*! TxLM buffer index  (XXX - may not be needed) */
  uint32 txlm_buffer_idx;
  
  /*! RF script index. 0xFFFFFFFF (msb=1, -ve) to indicate firmware should not program 
      any RF scripts. In this mode firmware still counts the settling time. 

      >= 16 script index indicates that firmware should use per tech's shared memory
      cfw_rf_events_set_t::rf_state[rf_script_idx - 16] as script buffer.

   */
  uint32 rf_script_idx;
  
  /*! User specified sequence number */
  uint32 sequence_number;
  
} cfw_tx_stop_msg_t;


/*! TX_DAC_STOP message interface.
    Disables DAC immediately. Must be sent before TX_STOP 
 */

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

} cfw_tx_dac_stop_msg_t;

/*! TX_START Message Response 
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  /*! Sequence number that was provided in the CMD message */
  uint32               sequence_number;

} cfw_tx_start_rsp_msg_t;
  
/*! TX_STOP Message Response 
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  /*! Sequence number that was provided in the CMD message */
  uint32               sequence_number;

} cfw_tx_stop_rsp_msg_t;

/*! TX_DAC_START Message Response 
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  uint32               dac_start_ustmr;

} cfw_tx_dac_start_rsp_msg_t;
  
/*! TX_DAC_STOP Message Response 
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  uint32               dac_stop_ustmr;

} cfw_tx_dac_stop_rsp_msg_t;


/*! Response to be sent after latching Pa params from PA_PARAMS_CONFIG msg. 
 *  Replaced by cfw_xpt_trans_compl_rsp_msg_t .
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

} cfw_pa_params_rsp_msg_t;

/*! \brief Payload infor for xpt_trans_compl response */
typedef struct {
  /*! \brief Pa state thresholds FW is using when this message is sent in
   * response xpt_config message */
  cfw_xpt_mode_t cur_mode;

  /*  Sequence No. provided in xpt_config message */
  uint32         sequence_number;
} cfw_xpt_trans_compl_payload_t;
 
/*! Response to be sent after switching to the Pa state thresholds as
 * indicated in the xpt_config message. If et_enable is set to TRUE in
 * xpt_config message, FW will use CFW_ET_MODE state thresholds. Otherwise,
 * FW will use CFW_EPT_MODE state thresholds.
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  /* Payload info */
  cfw_xpt_trans_compl_payload_t payload;

} cfw_xpt_trans_compl_rsp_msg_t;

/*! Response to be sent after processing cfw_xpt_config_msg_t
 */
typedef struct {

  /*! \brief Message header */
  msgr_hdr_struct_type hdr;

  /*  Sequence No. provided in xpt_config message */
  uint32               sequence_number;

} cfw_xpt_config_rsp_msg_t;
 
/*\}*/

/**
   \defgroup cfwtxgen General TX Interface Structures
    */
/*@{*/

/*! A structure that defines a single SSBI transaction. It can be used to build scripts 
 * to be used for multiple purposes.
 */
typedef struct
{
  /*! @brief  SBI transaction time relative to the burst (usually = 0)*/
  /*! @brief  Warning: Currently unused in FW.                        */
  int16     start_delta;
     
  /*! @brief  SBI register */
  uint8     data;

  /*! @brief  SBI address */
  uint8     addr;

  /*! @brief  SBI  bus */
  uint8     sbi_bus;

} cfw_sbi_config_t;

/*\}*/

/**
   \defgroup cfwhdet HDET FW Trigger
    */
/*@{*/

/*! \brief HDET ciruit may be in WTR or Ant Tuner boards. FW picks one based
 * on thse enums sent in hdet_sched_req */
typedef enum {
  CFW_HDET_MEAS_RF_SCRIPT,
  CFW_HDET_MEAS_ANT_TUN_RF_SCRIPT,
} cfw_rf_hdet_script_e;

/*! @brief HDET Request Message 
 *
 * This message triggers a HDET measurement in FW. When this message is received, FW
 * will start the HDET process in symbol 22 of PCG(n) for a measurement that occurs 
 * over the duration of the first 6 symbols of PCG(n+1), only when PCG(n+1) is not
 * blanked.
 *
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  /*! @brief 
   * HDET settling time. This counts from the beginning of the symbol FW
   * references the HDET script. This is currently symbol 3 in a PCG. 
   * So, if entire script (i.e PDET_ON, HKADC_ON, PDET_OFF) takes 250 cx1, 
   * provide 250 in this field. FW will use this to determine how long to
   * freeze TxAGC. In the above example of 250 cx1, FW will see that TxPower
   * is constant for 6 symbols beginning from symbol 3. (ceil(250/192) = 2;
   * => 2 * 3 symbols = 6 symbols)
   *
   */
  uint16 hdet_stl_time_cx1;

  /*! @brief Specify Time Out Count to FW. FW will bail out after this many
   * attempts of scheduling a HDET event.
   *
   * For DO, the units of the count is Half slots.
   * For 1x, it is in 192 chips.
   */
  uint16 hdet_time_out_cnt;

  /*! @brief Specify which script to pick between tx_hdet_meas and
   * hdet_ant_tun.
   */
  cfw_rf_hdet_script_e script_type;

  /*! @brief Tx AGC Threshold above which the event will be scheduled. Units
   * same as txTotal i.e -1/640dB
   */
  int16 tx_total_thresh;

} cfw_tx_hdet_request_msg_t;

/*! @brief HDET RESET MESSAGE
 *
 * This message triggers a reset of HDET state machine in FW. Primarily used
 * in hard hand over scenarios of 1x where SW is not sending a TX_STOP before
 * re-tuning. After FW finishes processing this message all the HDET state
 * variables are reset to their default values. SW should not issue a hdet
 * request message to FW until this reset is processed successfully. SW can
 * either poll on the state variables or wait sufficient amount of time after
 * sending this reset message and before issuing a hdet request MSG. Two back
 * to back resets are not supported; i.e SW can't issue a reset before the
 * previous reset is processed successfully.
 *
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

} cfw_tx_hdet_reset_msg_t;

/*! @brief HDET Response Message
 *
 * This is the response message that FW sends when it completes a HDET request made
 * by SW.
 *
 */
typedef struct {

  msgr_hdr_struct_type hdr;
  /*!< MSGR header */

  uint32 hdet_status;

  /*! @brief HDET Status. The actual definition is TBD.
   */

} cfw_tx_hdet_response_msg_t;

/*! @brief HDET Static Configuration Parameters
 * These parameters configure the HDET triggering algorithm in FW. 
 */
typedef struct {

  /*! @brief RF script for turning ON PDET. */
  cfw_sbi_config_t pdet_on_script[CFW_MAX_HDET_EVENT_SCRIPT_SIZE];

  /*! @brief RF script for HKADC conversion. */
  cfw_sbi_config_t hkadc_trig_script[CFW_MAX_HDET_EVENT_SCRIPT_SIZE];

  /*! @brief RF script for turning OFF PDET. */
  cfw_sbi_config_t pdet_off_script[CFW_MAX_HDET_EVENT_SCRIPT_SIZE];

  /* @brief HDET LPF Settling time in Cx1. It will be counted starting from the TX update 
   * boundary at which the measurement process started. HKADC conversion gets 
   * triggered after settling time. 
   */
  uint32 lpf_settle_time_cx1;

  /* @brief HKADC Conversion Time in Cx1. PDET turn OFF will be scheduled @ 
   * TX HS boundary + LPF settling + HKADC conversion time.
   */
  uint32 hkadc_conversion_time_cx1;

  /* @brief # of words actually used by FW from pdet_on_script. Should be 
   * >= 1 and <= CFW_MAX_HDET_EVENT_SCRIPT_SIZE
   */
  uint8 pdet_on_script_size;

  /* @brief # of words actually used by FW from hkadc_trig_script.Should be 
   * >= 1 and <= CFW_MAX_HDET_EVENT_SCRIPT_SIZE
   */
  uint8 hkadc_trig_script_size;

  /* @brief # of words actually used by FW from pdet_off_script.Should be 
   * >= 1 and <= CFW_MAX_HDET_EVENT_SCRIPT_SIZE
   */
  uint8 pdet_off_script_size;

} cfw_tx_hdet_config_t;

/*\}*/

/**
   \defgroup cfw_tx_agc_cfg TX AGC configuration messages and shared memory interfaces
    */
/*@{*/

/*! \brief Number of PA states */
#define CFW_NUM_PA_STATES         4

/*! \brief Number of PA threshold parameters */
#define CFW_NUM_PA_THRESH         (2*(CFW_NUM_PA_STATES-1))

/*!< Logical -> Physical PA Mapping.
*
* The following union specifies all possible types of mapping between "logical" PA
* state (0, 1, 2, 3) to "physical" PA HW updates.
*
* The actual mapping that is used in FW depends on the TX template selected.
*/
typedef union {
 
  /*!< PA Range Map.
   *
   * This is a bitmask that contains the GRFCs that need to be enabled for setting
   * a specific PA range.
   *
   * A PA range configuration requires 2 GRFC tasks.
   * 1. The first task clears both GRFCs.
   * 2. The second task enables GRFCs using the bit-mask/state provided below.
   *
   \endverbatim
   */
  uint32 pa_grfc_config[CFW_NUM_PA_STATES];

} cfw_tx_agc_pa_mapping_t;

/*! \brief  Power Amplifier (PA) Control */
typedef struct {

  /*!The initial counter value for the PA Hysteresis down counter. In 1x,
   * this is in units of PCGs i.e 1.25 ms units. In DO, this is units of
   * half-slots i.e 833us. This and only this is used to enable PA
   * hysteresis.  See the above note about pa_hysteresis_enable. Firmware
   * enables hysteresis when this is non-zero.  Disables when zero */
  int16 pa_hysteresis_timer;

  /*!< Logical -> Physical PA mapping 
   *
   * \sa cfw_tx_agc_pa_mapping_t
   */
  cfw_tx_agc_pa_mapping_t pa_mapping;
  
}  cfw_tx_agc_pa_cfg_t;

/*! @brief PA Phase Compensation Configuration 
 *
 * This is presently applicable to 1x mode only, but could be useful for HDR once
 * DTX is supported.
 */
typedef struct {
  
  /*! @brief PA Phase Compensation Enable Flag 
   * 
   * \verbatim
   * 0 -> disable
   * 1 -> enable \endverbatim
   */
  uint32 enable;

  /*! @brief PA Phase Compensation Delay 
   *
   * This value represents a delay (in Cx1) from the time at which the PA range 
   * transaction is dispatched by toggling GRFCs. The actual HW PA state change 
   * will happen after a group delay from the time at which the GRFCs were toggled.
   */
  int32 phase_delay_cx1;

  /*! @brief PA Phase Compensation Values
   *
   * These represent "MSB aligned" PA phase values. The actual units for these 
   * depends on target configuration. 
   *
   * For Nike-L, FW passes these values through to the following registers:
   *
   * \verbatim
   * REVMOD_ROTANGLE_0__PA_STATE00_ANGLE - PA range 00
   * REVMOD_ROTANGLE_0__PA_STATE01_ANGLE - PA range 01
   * REVMOD_ROTANGLE_1__PA_STATE10_ANGLE - PA range 10
   * REVMOD_ROTANGLE_1__PA_STATE11_ANGLE - PA range 11 \endverbatim
   *
   * Hence, each PA phase value is 9-bit MSB aligned to a 32-bit quantity.
   *
   * This table is indexed based on PA State. The above registers are setup during
   * TX initialization as a part of TX_START message processing. Hence, it is assumed
   * that TX_AGC_CFG message is received prior to receiving the TX_START message.
   *
   */
  int32 pa_phase[CFW_NUM_PA_STATES];

} cfw_pa_phase_comp_config_t;

/*! @brief TX Calibration Mode Configuration
 *
 * This section applies to 1X only.
 *
 * This flag tells FW if it is in FTM calibration mode or otherwise. This flag should 
 * be set to CFW_TX_MODE_NORMAL for ALL cases, except for full-calibration override mode.
 * 
 * When this flag is set, FW will disable all calls to FED API once TX is started. FED
 * has full control over DTR and RF FW. FW will only ensure that a valid TX waveform is
 * generated at the output of the uK.
 *
 */
typedef enum {
  CFW_TX_MODE_NORMAL = 0,  
  CFW_TX_MODE_CALIBRATION  /** Use ONLY for TX full-calibration override */
} cfw_tx_cal_mode_t;

/*!  @brief factor is the scale factor to be applied to nominal IQ_GAIN.
 * factor is in Unsigned Q2.14 format. Therefore, it's maximum value = 0xFFFF
 * = 3.99993896484375 => max(lutDbOffset) = 20log(3.99993896484375) = 12dB.
 * lutDbOffset is in 1/640 units.  */
typedef struct {
  uint16 factor;
  int16  lutDbOffset;
} cfw_tx_iq_scale_data_t;

#define CFW_IQ_GAIN_DATA_SIZE_CDMA1X  6
#define CFW_IQ_GAIN_DATA_SIZE_MCDO   19 
#define CFW_IQ_GAIN_DATA_MCDO_REV0_OFFSET   0
#define CFW_IQ_GAIN_DATA_MCDO_REVA_OFFSET   6

/*!  @brief Calibrate DAC for Ant 0 or Ant1 . Only one DAC can be calibrated
 * at a time. Since FW MSG processing thread is blocked for ~2ms, this should
 * be used with caution. FW can't react to any commands from SW for ~2+ ms
 * once a DAC calibration is issued. This should NEVER be used when the phone
 * is in a call. The TXLM buffer is used to update the TXC/TXR registers
 * before starting the dac calibration. The chain should be properly set in
 * the TXLM buffet. FW assumes all the parameters in the TXLM buffer are
 * valid and hence will perform FULL_CFG.  */
typedef struct
{
  /*!  @brief MSGR header */
  msgr_hdr_struct_type hdr;

  /*! @brief Whether to do dac cal on ant 0 */
  uint16 dac_cal_ant0: 1;       /* Bit 0 */

  /*! @brief Whether to do dac cal on ant 1 */
  uint16 dac_cal_ant1: 1;       /* Bit 1 */

  uint16 : 14;                /* Bits 15:2 */

  /*! TXC configuration buffer index from TxLM */
  uint32 txlm_buffer_idx;

} cfw_tx_dac_cal_config_msg_t;

/*! @brief Open Loop Turnaround Constant */
typedef enum {
  
  /*!< Specifies -73 dB (low bands) */
  CFW_OPEN_LOOP_CONST_73DB = 0,

  /*!< Specifies -76 dB (high bands) */
  CFW_OPEN_LOOP_CONST_76DB
    
} cfw_open_loop_turnaround_const_t;

/*! TX_AGC related configurations
*/
typedef struct {

  /*!  @brief MSGR header */
  msgr_hdr_struct_type hdr;

  /*! @brief Back-To-Future (BTF) offset in cx8. This applies to HDR only. */
  uint16 tx_btf_offset;

  /*! @brief Open Loop Antenna Index.
   * 
   * This is the logical antenna index from which TxAGC will obtain its Open
   * Loop Value. This is used to implement second-chain test call.
   *
   * \sa cfw_logical_antenna_t
   */
  cfw_logical_antenna_t tx_ol_antenna;

  /*! @brief AGC PA control */
  cfw_tx_agc_pa_cfg_t tx_agc_pa_cfg __attribute__((deprecated));

  /*! @brief PA Phase Compensation Configuration
   */
  cfw_pa_phase_comp_config_t pa_phase_comp_config __attribute__((deprecated));

  /*! @brief TX Calibration Mode 
   * 
   * Set to either normal or calibration mode (1X only).
   *
   * \sa cfw_tx_cal_mode_t
   */
  cfw_tx_cal_mode_t tx_cal_mode;

  /*! @brief Register address to program "PA current"
   */
  uint32 pa_current_address __attribute__((deprecated));

  /*! @brief xPT mode map. Assign AT/EPT/ET modes to different PA states.
   */
  uint8 xpt_mode[CFW_NUM_PA_STATES] __attribute__((deprecated));

  /*! @brief Indicate whether to use the old aptSmps or the new RFFE
   */
  uint8 apt_mode __attribute__((deprecated));

  /*! @brief Indicates the Open Loop turnaround constant for this band class.
   * *** Used for Logging Purposes Only ***
   */
  cfw_open_loop_turnaround_const_t open_loop_turnaround_const;

  /*! @brief Predictive clipping threshold for cdma1x & rev0.
  * a value of 0 indicates that clipping should be disabled by FW. 
  */
  uint16 wav_pred_clip_threshold;

  /*! @brief Predictive clipping threshold for hdr revA.
  * a value of 0 indicates that clipping should be disabled by FW.
  */
  uint16 wav_pred_clip_threshold_hdr_revA;

  /* @brief threshold above which predictive clipper is enabled for dynamic predictive clipping */
  uint16 wav_pred_clip_enable_thresh;

  /* @brief threshold below which predictive clipper is disabled for dynamic predictive clipping */
  uint16 wav_pred_clip_disable_thresh;

 } cfw_tx_agc_cfg_msg_t;

/*! @brief XPT configuration message. Carries info specific to ET and EPT modes.
 */
typedef struct {
  /*! MSGR header */
  msgr_hdr_struct_type hdr;

  /*! @brief Feature enable flags for EPT.
   */
  boolean ept_enable;

  /*! @brief Feature enable flags for ET
   */
  boolean et_enable;

  /*! @brief Delay parameter for the Envelope path in ET mode in 1/8th of
   * nano second units.
   */
  int32 et_env_delay;

  /*! @brief Nominal ENV scale value for EPT .
   */
  int32 env_scale_nom_ept;

  /*! @brief Nominal ENV scale value for ET
   */
  int32 env_scale_nom_et;

  /*! @brief TxLM buf ID to be used for ET chain setup. 
   */
  uint8 et_txlm_buf_id;

  /*! @brief TxLM buf ID for the primary chain configuration. This is the
   * txlm buf you would be sending in the TX_START message.
   */
  uint8 prim_txlm_buf_id;

  /*! @brief Primary tx_chain id. 0 for TXC0 and 1 for TXC1.
   */
  uint8 prim_tx_chain;

  /*! @brief Sequence number to help RFSW keep track of responses from FW.
   * This sequence number is sent in two responses: cfw_xpt_config_rsp_msg_t
   * & cfw_xpt_trans_compl_rsp_msg_t
   */
  uint32 sequence_number;

} cfw_xpt_config_msg_t;

/*! @brief Message to update ET delay during a call. Use similar
 * parameters that exist in cfw_xpt_config_msg_t for initializing ET delay. 
 * This message is ignored if ET is not already enabled via the
 * cfw_xpt_config_msg_t 
 */
typedef struct {
  /*! MSGR header */
  msgr_hdr_struct_type hdr;

  /*! @brief Delay parameter for the Envelope path in ET mode in 1/8th of
   * nano second units.
   */
  int32 et_env_delay;

  /*! @brief FW ignores this as there is no response corresponding to this
   * message
   */
  uint32 sequence_number;

} cfw_xpt_delay_update_msg_t;

/*! @brief RK:todo TO BE REMOVED after RFSW gets rid of it's references to
 * this.
 * HDET configuration message. Expected to be much less frequent
 * compared the HDET REQ MSG.
 */
typedef struct {

  /*! MSGR header */
  msgr_hdr_struct_type hdr;

} cfw_tx_hdet_config_msg_t;

/*! @brief PA Backoff Parameters Message
 *
 * Used as a part of TX limit parameters in 
 * structure cfw_tx_limit_config_msg_t.
 */
 typedef struct {

   uint32 newPABackOffLUTAvailable;
  /*!< Pa Back off LUT table available flag.
   *
   * 1 -> LUT will be latched from SMEM.
   * 0 -> No need to update the LUT from SMEM.
   */

   /*! HDET and Temp correction used in PA BACK OFF correction*/
   int16 HdetAndTempCorrection;

   /*! LUT indiactor.index 0,1,2 respectively determine which LUT(Hi/L0) to use*/
   int16 LutIndicator[HDRFW_PA_BACKOFF_CARRIERS];

} cfw_pa_backoff_config_t;

/*! @brief TX Limit Parameters Message
 *
 * Defines TX limit parameters. Latched at 2 places:
 * - Each frame boundary.
 * - On TX init.
 */
typedef struct {

  /*! MSGR header */
  msgr_hdr_struct_type hdr;

  /*! MAX txTotal (FW units: -1/640 dB) */
  int16 tx_total_max;

  /*! MIN txTotal (FW units: -1/640 dB) */
  int16 tx_total_min;
  
  /*! PA Backoff Params*/
  cfw_pa_backoff_config_t PaBackoffParams;

} cfw_tx_limit_config_msg_t;


/** Table size for TxAGC power and APT LUT tables. */
#define CFW_RFA_LUT_SIZE                      64

/*!
 * @brief TX timing info when DAC_START message is received.
 *
 * This structure contains useful TX timing information when TX DAC_START message is
 * received.
 */
typedef struct {

  /** Value of [REV]MOD_TX_SYS_TIME when DAC start message is received.*/
  uint32 txSampleCount;
  /** Value of RTC_[REV]MOD_TX_SYS_TIME when DAC start message is received. */
  uint32 txSTMRTime;
  /** Value of RTC counter when DAC start message is received */
  uint32 rtcCntCx8;
  /** Value of RTC action time sent in the DAC start message */
  uint32 rtcActionTimeCx8;
  /** Value of current USTMR */
  uint32 ustmrCurrentTime;
  /** Value of USTMR action time (determined using time transfer) */
  uint32 ustmrActionTime; 

} cfw_tx_dac_start_timing_info_t;

/*!
 * @brief Contents of linearizer table in packed format. Note that power
 * values are not included in this. Only AgcAdj, IQGAIN and SMPS are
 * included. iqGain values is the value after Temp Comp. 
 *
 */
typedef union ALIGN(8) {

  struct {
    /*RGI i.e Tx analog gain or txAgcAdj */ 
    uint32 rgi;

    /*IQ gain for this row */ 
    uint16 iqGain;

    /* APT value */
    // int16 aptSmps;
  };

  int64 d;

} cfw_tx_lin_row_t;

/* ************************ */
// RK:new Lin table

/*! @brief Structure for sending bank & base for AMAM, AMPM, ET detrough to FW . 
 * One instance of this is needed per linearizer row per table type. (table
 * type = AMAM or AMPM or ET detrough). ET detrough info is unused for EPT PA
 * state*/
typedef struct ALIGN(4)
{
  /*! @brief Base addr (offset within bank) */
  uint16 base;
  /*! @brief Bank ID . This doesn't have to be 2 bytes. 1 byte is enough. But
   * for word alignment, we are using 16 bits. */
  uint16 bank_id;
} cfw_xpt_bank_info_lin_row_t;

/*! @brief Bank size information for a linearizer table. Size could be
 * different for AMAM, AMPM and Detrough tables. Therefore, we make separate
 * variables for each. One instance of this is needed for each PA state that
 * supports EPT/ET.
 */
typedef struct {

  rfcmd_xpt_bank_size_e amam;

  rfcmd_xpt_bank_size_e ampm;

  rfcmd_xpt_bank_size_e epdt;

} cfw_xpt_bank_size_info_per_lin_t;

/*! @brief TX power column in the linearizer table. This stands alone from
 * rest of the Lin table contents because FW can copy this into TCM when
 * latching from SMEM. Only TX power needs to be in TCM as it is used for
 * binary search. 
 */
typedef struct ALIGN(32) {

  /*!< * Power column in the new linearizer table. Expected to be static
   * update only at the beginning of the call. 
   */
  int16 txPwrT[CFW_NUM_PA_STATES][CFW_RFA_LUT_SIZE];

} cfw_rfa_lin_tx_pwr_t;

/*! @brief Structure for sending bank and base for AMAM,AMPM and ET detrough
 * info to FW . This structure holds info for one row of a Linearizer table
 * i.e PA state.  Size = 4 * 3 (bytes)= 12 bytes . Hence Aligned to word
 * boundary .*/
typedef struct ALIGN(4)
{
  /*! @brief AMAM table information */
  cfw_xpt_bank_info_lin_row_t amam;

  /*! @brief AMPM table information */
  cfw_xpt_bank_info_lin_row_t ampm;

  /*! @brief EPDT (Envelope path detrough table) information */
  cfw_xpt_bank_info_lin_row_t epdt;
} cfw_xpt_bank_info_row_t;

/*! @brief Static linearizer data (except Tx power) per row. Size = 24 bytes
 * = 3 double words. Align to double word boundary. 
 * */
typedef struct ALIGN(8)
{
  /*! @All XPT bank info */
  cfw_xpt_bank_info_row_t xpt_info;

} cfw_static_lin_info_row_t;

/*! @brief Static linearizer data (except Tx power) for one PA state.  PA
 * states 2 and 3 support XPT but PA states 0 and 1 do not. Therefore, XPT
 * info for PA states 0 and 1 is ignored by FW. We specifically did not split
 * linearizer into two types, one for PA states 0 and 1, and one for PA
 * states 2 and 3. This is because we valued uniformity over saving 768 bytes
 * of memory.  
 * Size = 24 * 64 (bytes) = 192 double words.  Aligned to double
 * words.  */
typedef struct ALIGN(8)
{
  cfw_static_lin_info_row_t row[CFW_RFA_LUT_SIZE];
} cfw_static_lin_info_pa_state_t;

/*! @brief CDMA 1x Linearizer table. All static Linearizer data is here
 * except TX power column which is defined as a separate structure. See
 * cfw_rfa_lin_tx_pwr_t.  */
typedef struct ALIGN(8)
{
  cfw_static_lin_info_pa_state_t pa_state[CFW_NUM_PA_STATES];
} cfw_static_lin_table_t;

/*! @brief Bank size information for the DPD banks. bank_size[0] and
 * bank_size[1] are ignored by FW. bank_size[2] and bank_size[3] are used for
 * PA states 2 and 3 respectively.
 *  */
typedef struct
{
  cfw_xpt_bank_size_info_per_lin_t bank_size[4];
} cfw_xpt_bank_size_info_t;

/*! @brief Dynamic portion of the linearizer table. Dynamic here means this
 * data can be changed during a call for reasons like Temperature
 * Compensation or HDET correction.
 *  */
typedef struct ALIGN(32) {

  /*!< *  Packed into a 64 bit word so that all dynamic updates look atomic
   * to the TX-ISR (the reading thread)
   */
    cfw_tx_lin_row_t T[CFW_NUM_PA_STATES][CFW_RFA_LUT_SIZE];

} cfw_rfa_lut_tables_packed_t;

/*! @brief PA state rise and fall thresholds
 *  
 *  */
typedef struct {
  // RISE1,RISE2 and RISE3 values. RISEx refers to the rise threshold to 
  // rise to state x (from state x-1). 
  int16 RiseThresholds[(CFW_NUM_PA_STATES-1)];

  // FALL1,FALL2 and FALL3 values. FALLx refers to the fall threshold to 
  // fall from state x (to state x-1). 
  int16 FallThresholds[(CFW_NUM_PA_STATES-1)];
} cfw_pa_state_thresholds_t;


// RK:new Lin table end
/* ************************ */

/*! @brief Command to configure SAR and Coex TX power filters.
 *
 */
typedef struct {

  /*!< MSGR header */
  msgr_hdr_struct_type hdr;

  /*!< HE estimation filter alpha update flag . Used for SAR/MTPL*/
  boolean HEFilterAlphaUpdate;

  /*!< Coex TX Power estimation filter alpha update flag. Set this to TRUE if
   * you want to update coexTxFiltAlpha and send your coefficient in coexTxFiltAlpha */
  boolean coexTxFiltAlphaUpdate;

  /*!< HE estimation filter alpha.
    This is the IIR filter coefficient for the headroom estimation filter for
    SVDO.  It filters the 1x TX power in linear domain once every PCG. The
    coefficient can be computed as follows: alpha = RND(32768*1536*Tc/T),
    where Tc = chip duration and T is the time constant in seconds. The time
    constant is a value determined by Modem Systems.
   
    For DO, filter runs every 1024 chips. FW default TC is 100ms for DOs
    well. This will translate into alpha = RND(32768*1024*Tc/T = 273)
  */
  uint16 HEFilterAlpha;

  /*!< alpha for C+G Coexistence Tx pwr estimation filter.
    Unlike the HEFilter ( see above), the Time Constant can't be quantified
    in ms since this filter runs only on non-DTXed half-slots which are not
    predictable. Therefore, pick a safe co-eff in the units of Txed Half
    slots.

    For DO, filter runs every 1024 chips. FW default alpha is 1365 (in Q15) which is
    equal to 20ms of truly transmitted signal duration. 

    For 1x, filter runs every PCG . FW default alpha is 2050 (in Q15) which is
    equal to 20ms of truly transmitted signal duration. 
  */
  uint16 coexTxFiltAlpha;

  /*!< HE filter Reset.
    When set (= 1), the HE filter is held in RESET state with output = 0x7FFF (min power).
    When reset (= 0), the HE filter operates normally (filters TxTotal).
  */
  boolean HEFilterReset;

  /*!< Coex filter Reset.
    When set (= 1), the Coex TX filter is held in RESET state with output = 0x7FFF (min power).
    When reset (= 0), the HE filter operates normally (filters TxTotal).
  */
  boolean coexTxFilterReset;

} cfw_rl_pwr_est_filter_cfg_msg_t;


/*----------------------------------------------------------------------------*/
/*! 
  @brief   CFW sleep modes 

  @details
  For features like DLNA , FW can call the rx_on_off_ctrl API, with different
  sleep modes. Each sleep mode is responsible for a different set of register 
  writes i.e. micro sleep mode will not disable the PLL on the WTR and will
  be a lower scale sleep, while milli sleep will put the PLL to sleep as well
  each sleep mode has its own use-case. This enum matches the FED API sleep
  mode enum.

*/
typedef enum 
{
  /*! Micro Sleep mode, micro sleep is used in features like DLNA or enhanced 
  DLNA where the Rx path is shut off for shorter durations and PLL is still 
  locked */ 
  CFW_MICRO_SLEEP_MODE =  0     ,

  /*----------------------------------*/
  /*! Milli sleep mode with off only mode , RFLM will schdule ccs events 
  for enabling Rx chain only and not disabling it (not 
  supported right now) */ 
  CFW_MILLI_SLEEP_ON_ONLY_MODE  , 

  /*----------------------------------*/
  /*! Milli sleep mode with off only mode , RFLM will schdule ccs events 
  for disabling Rx chain only and not enabling it (not 
  supported right now) */ 
  CFW_MILLI_SLEEP_OFF_ONLY_MODE  , 
   
} cfw_sleep_mode_e;

typedef struct {
  /*! \brief Message header */
  msgr_hdr_struct_type  hdr;

  /*! \brief
  1: Enable the debug feature to force microsleep every half slot
  0: Disable the feature
  This flag is checked every half slot and will be latched at
  the next half slot. */
  uint32 forceSleepEnableDisable;
       
  /*! \brief
  The delta from the USTMR execution time where the RF will be scheduled.
  A value of 0 results in an immediate RF off. */
  uint32 rfOffUstmrDelta;
       
  /*! \brief
  The delta from the USTMR execution time where RF should be settled. */
  uint32 rfOnUstmrDelta;

  /*! \brief
  For HDR, this field specifies the number of half slots to
  consecutively force sleep. */
  uint32 duration;
  
  /*! @brief 
  Micro/milli sleep, enum internal to c2k to determine whether 
  the sleep is micro or milli type.*/
  cfw_sleep_mode_e sleepMode;
  
} cfw_rx_exercise_milli_micro_sleep_msg_t;



/*\}*/

/*\}*/
#endif

