#ifndef RR_LOG_TYPES_H
#define RR_LOG_TYPES_H

/*!
  @file rr_log_types.h

  @brief
  Exports structures used by the RR logging module, rr_log.c.
*/

/*===========================================================================

  Copyright (c) 2005-2013 Qualcomm Technologies, Inc.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_log_types.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2010-04-07   GDJ     Initial revision created by moving types defined in
                     rr_log.h.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "log.h"
#ifdef FEATURE_WCDMA
#include "rrc_rr_types.h"
#endif /* FEATURE_WCDMA */

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/**
  @brief Values for protocol error log packets.
*/
typedef enum rr_protocol_error_enum
{
  RR_PROTOCOL_ERROR_MESSAGE_TOO_SHORT = 0,
  RR_PROTOCOL_ERROR_UNKNOWN_PD,
  RR_PROTOCOL_ERROR_UNKNOWN_TYPE,
  RR_PROTOCOL_ERROR_INVALID_MANDATORY_INFORMATION,
  RR_PROTOCOL_ERROR_OTHER
} rr_protocol_error_enum_type;

/**
  @brief Indicates whether a signaling message is an uplink or downlink
         message.
*/
typedef enum
{
  RR_UPLINK_MSG   = 0, /*!< Message is an uplink message. */
  RR_DOWNLINK_MSG = 1  /*!< Message is a downlink message. */
} rr_signaling_message_direction_T;

/**
  @brief Enumeration of different types of handover that are possible.
*/
typedef enum
{
  RR_LOG_HO_TYPE_GSM_TO_GSM    = 0x0, /*!< GSM to GSM inter-cell handover. */
  RR_LOG_HO_TYPE_WCDMA_TO_GSM  = 0x1, /*!< WCDMA to GSM inter-RAT handover. */
  RR_LOG_HO_TYPE_GSM_TO_WCDMA  = 0x2, /*!< GSM to WCDMA inter-RAT handover. */
  RR_LOG_HO_TYPE_LTE_TO_GSM    = 0x3, /*!< LTE to GSM inter-RAT handover (SRVCC). */
  RR_LOG_HO_TYPE_COUNT
} rr_log_handover_type_T;

/**
  @brief Enumeration of handover failure reasons.
*/
typedef enum
{
  RR_LOG_HO_FAILURE_PROTOCOL_ERROR              = 0x00,
  /*!< The HANDOVER COMMAND message was invalid. */

  RR_LOG_HO_FAILURE_CONFIGURATION_NOT_SUPPORTED = 0x01,
  /*!< The configuration requested by the network is not supported. */

  RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE    = 0x02,
  /*!< Physical Channel Failure during handover. */

  RR_LOG_HO_FAILURE_HANDOVER_ABORTED            = 0x03,
  /*!< Handover was aborted for some reason. */

  RR_LOG_G2WHO_FAILURE_REASON_OFFSET            = 0x04,
  /*!< Not used, but retained for numbering purposes. */

  RR_LOG_G2WHO_INVALID_STATE                    = 0x05,
  /*!< G2W handover failed due to invalid state. */

  RR_LOG_G2WHO_WCDMA_PROTOCOL_ERROR             = 0x06,
  /*!< G2W handover failed due to WCDMA protocol error. */

  RR_LOG_G2WHO_INVALID_CONFIGURATION            = 0x07,
  /*!< G2W handover failed due to an invalid or unsupported configuration
  requested by the network. */

  RR_LOG_G2WHO_PHYSICAL_CHANNEL_FAILURE         = 0x08,
  /*!< G2W handover failed due to physical channel failure. */

  RR_LOG_G2WHO_INVALID_CIPHER_ALGORITHM         = 0x09,
  /*!< G2W handover failed due to an invalid cipher algorithm. */

  RR_LOG_G2WHO_MAX_FAILURE_CAUSE                = 0x0a,
  /*!< Not used, but retained for numbering purposes. */

  RR_LOG_G2WHO_CANNOT_SUSPEND_GL2               = 0x0b
  /*!< G2W handover failed as L2 failed to suspend. */

} rr_log_handover_failure_T;

/**
  @brief Enumeration of reselection trigger reasons.ed)
*/
typedef enum
{
  RR_RESELECTION_TRIGGER_NONE             = 0x0,
  /*!< Unspecified reason. */

  RR_RESELECTION_TRIGGER_C1               = 0x1,
  /*!< C1 of serving cell falls below 0 for more than 5 seconds. */

  RR_RESELECTION_TRIGGER_C2               = 0x2,
  /*!< C2 of a neighbour cell is better than serving cell C2 for more than 5
  seconds. */

  RR_RESELECTION_TRIGGER_DSF              = 0x3,
  /*!< Downlink signalling failure occurred. */

  RR_RESELECTION_TRIGGER_SERVING_BARRED   = 0x4,
  /*!< Serving cell has become barred. */

  RR_RESELECTION_TRIGGER_T3126_TIMEOUT    = 0x5,
  /*!< T3126 expiry during connection establishment while waiting for a
  response from the network. */

  RR_RESELECTION_TRIGGER_SERVING_BSIC_CHANGED = 0x6,
  /*! BSIC of the serving cell has changed. */

  RR_RESELECTION_TRIGGER_GSM_TO_WCDMA     = 0x7,
  /*!< Inter-RAT reselection to a WCDMA cell has been triggered. */

  RR_RESELECTION_TRIGGER_WCDMA_TO_GSM     = 0x8,
  /*!< Inter-RAT reselection from a WCDMA cell has been triggered. */

  RR_RESELECTION_TRIGGER_T3146_TIMEOUT    = 0x9,
  /*!< T3146 expiry during connection establishment while waiting for a
  reponse from the network. */

  RR_RESELECTION_TRIGGER_C32_C31          = 0x0A,
  /*!< C32 of a neighbour cell is better than the serving cell's C32 value
  while C31 is above zero. */

  RR_RESELECTION_TRIGGER_C32              = 0x0B,
  /*!< C32 of a neighbour cell is better than the serving cell's C32 while C31
  is below zero. */

  RR_RESELECTION_TRIGGER_N3102            = 0x0C,
  /*!< N3102 RLC counter fell below zero. */

  RR_RESELECTION_TRIGGER_SI13             = 0x0D,
  /*!< SI13 acquisition timed out. */

  RR_RESELECTION_TRIGGER_PSI1             = 0x0E,
  /*!< PSI1 acquisition timed out. */

  RR_RESELECTION_TRIGGER_PCCO             = 0x0F,
  /*!< PCCO received from the network. */

  RR_RESELECTION_TRIGGER_ACTIVATED_PBCCH  = 0x10,
  /*!< PBCCH activation on the serving cell triggered reselection. */

  RR_RESELECTION_TRIGGER_DEACTIVATED_PBCCH  = 0x11,
  /*!< PBCCH deactivation on the serving cell triggered reselection. */

  RR_RESELECTION_TRIGGER_PCCO_3G_TARGET = 0x12,
  /*!< PCCO to a WCDMA cell received from the network. */

  RR_SLOW_SNR_RESELECTION_TRIGGER = 0x13,
  /*!< Slow SNR reselection algorithm triggered reselection. */

  RR_FAST_SNR_RESELECTION_TRIGGER = 0x14,
  /*!< Fast SNR reselection algorithm triggered reselection. */

  RR_RESELECTION_TRIGGER_GSM_TO_WCDMA_FAILED = 0x15,
  /*!< Inter-RAT G2W reselection has failed, reselection back to GSM is
  triggered. */

  RR_XFER_SNR_RESELECTION_TRIGGER = 0x16,
  /*!< Serving cell SNR triggered reselection while in transfer mode. */

  RR_XFER_LARA_SNR_RESELECTION_TRIGGER = 0x17,
  /*!< Serving cell SNR triggered reselection to a cell in a different
  location area or routing area while in transfer mode. */

  RR_RR_EST_REQ_SNR_RESELECTION_TRIGGER = 0x18,
  /*!< RR_EST_REQ received while SNR counter for the serving cell is zero,
  causing RR to trigger an immediate reselection in order to improve the chance
  of successfully establishing the connection on the new cell. */

  RR_UL_TBF_REQ_SNR_RESELECTION_TRIGGER = 0x19,
  /*!< Uplink TBF request received while SNR counter for the serving cell is
  zero, causing RR to trigger an immediate reselection in order to improve the
  chance of successfully establishing the connection on the new cell. */

  RR_RESELECTION_TRIGGER_W2G_NACC_STARTED = 0x1A,
  /*!< Inter-RAT W2G NACC procedure is triggered. */

  RR_RESELECTION_TRIGGER_PCCO_FAILED = 0x1b,
  /*!< Reselection is triggered after PCCO failure. */

  RR_RESELECTION_TRIGGER_SGLTE_BAND_BARRED = 0x1c,
  /*!< Reselection is triggered because the serving cell band has been locally
  barred in GERAN due to band co-existence. */

  RR_RESELECTION_TRIGGER_MAX
} rr_log_reselection_trigger_T;

/**
 * Reasons why a reselection procedure is considered to have failed.
 * Note that for reselection power scan, the reselection procedure is considered
 * to have failed even though RR is attempting to maintain service by performing
 * the reselection power scan.
 */
typedef enum
{
  /**
   * reselection was aborted due to a page
   */
  RR_LOG_RES_FAILURE_ABORT_DUE_TO_PAGE          = 0x0,

  /**
   * reselection was aborted due to an MM request
   */
  RR_LOG_RES_FAILURE_ABORT_DUE_TO_MM_REQ        = 0x1,

  /**
   * reselection was aborted due to a cell reselection abort timeout
   */
  RR_LOG_RES_FAILURE_RESELECTION_ABORT_TIMEOUT  = 0x2,

  /**
   * C2 based reselection aborted due to not finding suitable neighbors
   */
  RR_LOG_RES_ABORT_C2_RETURN_TO_SERVING         = 0x3,

  /**
   * a cell selection has been triggered
   */
  RR_LOG_RES_FAILURE_CELL_SELECTION             = 0x4,

  /**
   * RR was unable to find service on any cell in the BA list. RR has lost service
   */
  RR_LOG_RES_FAILURE_NO_BCCH_FOUND              = 0x5

  /**
   * C32 based reselection aborted due to not finding suitable neighbors
   */
  ,RR_LOG_RES_ABORT_C32_RETURN_TO_SERVING        = 0x6,

  /**
   * No response received from target cell
   */
  RR_LOG_RES_ABORT_NO_RESPONSE_FROM_TARGET      = 0x7,

  /**
   * UL access Request for cell update is rejected on target cell
   */
  RR_LOG_RES_ABORT_UL_ACCESS_REJECT_ON_TARGET   = 0x9,

  /**
   * During cell update or routing update, the network can force MS to standby mode
   */
  RR_LOG_RES_ABORT_FORCED_TO_STANDBY_ON_TARGET  = 0xA,

  RR_LOG_RES_FAILURE_NO_PBCCH_FOUND             = 0xB


  /* RR_LOG_G2WR_RESELECT_REJ_REASON_OFFSET  is defined for easy mapping of the
  RRC reject reason to the failure reason reported in the event */
  /* failure reason in the event = rrc_interrat_reselect_rej->reject_reason +
                              RR_LOG_G2WR_RESELECT_REJ_REASON_OFFSET + 1 */
  ,
  RR_LOG_G2WR_RESELECT_REJ_REASON_OFFSET = 0xC,
  RR_LOG_G2WR_WCDMA_NONE,
  RR_LOG_G2WR_WCDMA_INAVALID_STATE,
  RR_LOG_G2WR_WCDMA_CELL_BARRED,
  RR_LOG_G2WR_WCDMA_PLMN_MISMATCH,
  RR_LOG_G2WR_WCDMA_LOW_S_VALUE,
  RR_LOG_G2WR_WCDMA_ACQUISITION_FAIL,
  RR_LOG_G2WR_WCDMA_SIBS_FAILED_LOWER_LAYERS,
  RR_LOG_G2WR_WCDMA_INVALID_SIBS,
  RR_LOG_G2WR_WCDMA_SIBS_TIMEOUT,
  RR_LOG_G2WR_WCDMA_SIBS_FAILED_OTHER,
  RR_LOG_G2WR_WCDMA_FORBIDDEN_LA,
  RR_LOG_G2WR_WCDMA_CELL_QUALITY_FAILURE,
  RR_LOG_G2WR_WCDMA_CELL_NOT_SUITABLE,
  RR_LOG_G2WR_WCDMA_CELL_NOT_EVALUATED,
  RR_LOG_G2WR_WCDMA_CELL_CHANGE_FAILURE,
  RR_LOG_W2G_NACC_DISABLED_BY_NVMEM,
  RR_LOG_W2G_NACC_VALIDATION_FAILED,
  RR_LOG_RES_FAILURE_T3174_TIMEOUT,

  RR_LOG_RESELECTION_FAILURE_CAUSE_UNKNOWN,
  RR_LOG_RESELECTION_FAILURE_CAUSE_CELL_BARRED,
  RR_LOG_RESELECTION_FAILURE_CAUSE_PLMN_MISMATCH,
  RR_LOG_RESELECTION_FAILURE_CAUSE_FORBIDDEN_TA,
  RR_LOG_RESELECTION_FAILURE_CAUSE_ACQ_FAILED,
  RR_LOG_RESELECTION_FAILURE_CAUSE_DL_WEAK,
  RR_LOG_RESELECTION_FAILURE_CAUSE_CFG_FAILURE,
  RR_LOG_RESELECTION_FAILURE_CAUSE_CELL_SELECT_FAILURE,
  RR_LOG_RESELECTION_FAILURE_CAUSE_SIB_READ_ERROR,
  RR_LOG_RESELECTION_FAILURE_CAUSE_FREQ_BARRED,
  RR_LOG_RESELECTION_FAILURE_CAUSE_FORBIDDEN_CSG_ID,
  RR_LOG_RESELECTION_FAILURE_CAUSE_SIB_TIMEOUT_ERROR,
  RR_LOG_RESELECTION_FAILURE_CAUSE_SIB_INVALID_ERROR,
  
#ifdef FEATURE_GSM_TDS
  RR_LOG_G2T_RESEL_STATUS_SUCCESS,
#endif

  RR_LOG_G2W_RESEL_STATUS_SUCCESS = 0xFF

} rr_log_reselection_failure_T;

/**
 * Reasons why a camping attempt may fail
 */
typedef enum
{
  /**
   * BCCH was not found by L1 when asked to acquire it
   */
  RR_LOG_CAMPING_FAILURE_BCCH_NOT_FOUND       = 0x0,

  /**
   * a BCCH decode failure was declared by L1
   */
  RR_LOG_CAMPING_FAILURE_BCCH_DECODE_FAILURE  = 0x1,

  /**
   * a BCCH decode timeout occurred
   */
  RR_LOG_CAMPING_FAILURE_BCCH_DECODE_TIMEOUT  = 0x2,

  /**
   * cell was unsuitable/unacceptable for camping when all SI was collected
   */
  RR_LOG_CAMPING_FAILURE_CELL_NOT_SUITABLE    = 0x3

  /**
   * PBCCH was not found by L1 when asked to acquire it
   */
  ,RR_LOG_CAMPING_FAILURE_PBCCH_NOT_FOUND     = 0x4,  // not used

  /**
   * a PBCCH decode failure was declared by L1
   */
  RR_LOG_CAMPING_FAILURE_PBCCH_DECODE_FAILURE = 0x5,  // not used

  /**
   * Requested PLMN not found
   */
  RR_LOG_CAMPING_FAILURE_PLMN_NOT_FOUND      = 0x6,

  /**
   * Cell is barred for access
   */
  RR_LOG_CAMPING_FAILURE_CELL_BARRED         = 0x7,

  /**
   * C1 falls below zero on camped attempt cell
   */
  RR_LOG_CAMPING_FAILURE_C1_BELOW_ZERO       = 0x8,

  /**
   * Better C2 not found on camped attempt cell
   */
  RR_LOG_CAMPING_FAILURE_BETTER_C2_NOT_FOUND = 0x9,

  /**
   * Failure in acquiring Packet System Information 1
   */
  RR_LOG_CAMPING_FAILURE_PSI1_DECODE_FAILURE  = 0xA   // not used

} rr_log_camping_attempt_failure_T;

/**
 * reason for GSM access failure
 */
typedef enum
{
  /**
   * gsm access failure due to rach failure
   */
  RR_LOG_ACCESS_FAILURE_DUE_TO_RACH_FAILURE       = 0x0,

  /**
   * gsm access failure due to downlink signal failure
   */
  RR_LOG_ACCESS_FAILURE_DUE_TO_DOWNLINK_SIGNAL_FAILURE= 0x1,

  /**
   * gsm access failure due to Immediate_assignment reject
   */
  RR_LOG_ACCESS_FAILURE_DUE_IMMEDIATE_ASSIGNMENT_REJECT = 0x02,

  /**
   * gsm access failure due to L2 LINK EST failure
   */
  RR_LOG_ACCESS_FAILURE_DUE_L2_LINK_ESTABLISHMENT_FAILURE = 0x03,

  /**
   * gsm access failure due to upper layer trigger
   */
  RR_LOG_ACCESS_FAILURE_DUE_TO_UPPER_LAYER_TRIGGER= 0x04

} rr_log_access_failure_reason_T;

/**
 * reason for GSM call drop
 */
typedef enum
{
  /**
   * gsm call dropped due to radio link failure
   */
  RR_LOG_CALL_DROPPED_DUE_TO_RADIO_LINK_FAILURE       = 0x0,

  /**
   * gsm call dropped due to network abnormal release
   */
  RR_LOG_CALL_DROPPED_DUE_TO_NW_ABNORMAL_RELEASE = 0x01,

   /**
   * gsm call dropped due to L2 Link Failure
   */
  RR_LOG_CALL_DROPPED_DUE_TO_L2_LINK_FAILURE = 0x02,
  /**
   * gsm call dropped due to Upper layer trigger
   */
  RR_LOG_CALL_DROPPED_DUE_TO_UPPER_LAYER_TRIGGER= 0x3

} rr_log_call_dropped_reason_T;

/**
 * GSM call type
 */
typedef enum
{
  RR_LOG_VOICE_CALL = 0x0,

  RR_LOG_DATA_CALL= 0x1,

  RR_LOG_SMS_CALL = 0x02,

  RR_LOG_SIGNALLING_CALL= 0x3

} rr_log_call_type_T;

/**
 * reason why the cell selection is starting
 */
typedef enum
{
  /**
   * cell selection started due to RR_PLMN_SELECT_REQ from MM
   */
  RR_LOG_CELL_SELECTION_START_PLMN_SELECT_REQ       = 0x0,

  /**
   * cell selection started autonomously by RR
   */
  RR_LOG_CELL_SELECTION_START_AUTONOMOUS_SELECTION  = 0x1,

  /**
   * cell selection started due to W2G Service Redirection request.
   */
  RR_LOG_CELL_SELECTION_START_W2G_SERVICE_REDIRECTION = 0x02

} rr_log_cell_selection_start_reason_T;

/**
 * search mode for the PLMN list operation
 */
typedef enum
{
  /**
   * Immediate PLMN list search (service is lost)
   */
  RR_LOG_PLMN_LIST_IMMEDIATE    = 0x0,

  /**
   * Background PLMN list search
   */
  RR_LOG_PLMN_LIST_BACKGROUND   = 0x1,

  /* Manual CSG List Search */
  RR_LOG_PLMN_LIST_MANUAL_CSG_LIST_SEARCH = 0x02,

  /* Automatic CSG List Search */
  RR_LOG_PLMN_LIST_AUTOMATIC_CSG_LIST_SEARCH = 0x03
} rr_log_plmn_list_search_mode_T;

/**
 * reason why a cell selection is ending
 */
typedef enum
{
  /**
   * successful end (RR has camped on a new cell)
   */
  RR_LOG_CELL_SELECTION_END_NEW_CELL            = 0x0,

  /**
   * RR is already camped on a cell of the requested PLMN
   */
  RR_LOG_CELL_SELECTION_END_ALREADY_CAMPED      = 0x1,

  /**
   * a cell was not found (suitable or acceptable depending on request)
   */
  RR_LOG_CELL_SELECTION_END_CELL_NOT_FOUND      = 0x2,

  /**
   * the cell selection abort timer has expired
   */
  RR_LOG_CELL_SELECTION_END_ABORT_TIMEOUT       = 0x3,

  /**
   * an MM request is interrupting the cell selection
   */
  RR_LOG_CELL_SELECTION_END_ABORT_MM_REQUEST    = 0x4
} rr_log_cell_selection_end_reason_T;

/**
 * the status of the power scan operation
 */
typedef enum
{
  /**
   * power scan has been started
   */
  RR_LOG_PSCAN_STARTED    = 0x0,

  /**
   * power scan has been completed
   */
  RR_LOG_PSCAN_COMPLETED  = 0x1,

  /* Reselection power scan events */
  RR_LOG_RESELECTION_PSCAN_STARTED  = 0x02,
  RR_LOG_RESELECTION_PSCAN_COMPLETED  = 0x03

} rr_log_power_scan_status_T;



/**
 * Reasons why a surrounding search of decoding system information messages can fail
 */
typedef enum
{
  /**
   * a BCCH decode timeout occurred
   */
  RR_LOG_GPRS_BCCH_DECODING_TIMEOUT       = 0x0,

  /**
   * NCELL was not found by L1 when asked to acquire it
   */
  RR_LOG_GPRS_NCELL_NOT_FOUND_BY_L1       = 0x1,

  RR_LOG_GPRS_ABORT_DUE_TO_PAGE           = 0x4


} rr_log_gprs_surround_search_failure_T;


typedef PACKED struct PACKED_POST
{
    byte attempt;
    word FNmod42432;
    byte access_reason;
} rr_log_rar_event_T;

LOG_RECORD_DEFINE(LOG_GSM_RR_STATE_C)
  /* RR state (or state machine) */
  byte state;

  /* Substate (depends on rr_state value) */
  byte substate;
  byte status;

  /* Only applicable to dedicated mode - 0 Voice, 1 Data */
  byte mode;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_RR_PROTOCOL_ERROR_C)
  /* the OTA message type (GSM04.18) */
  byte message_type;

  /* protocol error */
  byte protocol_error;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_RR_CELL_RESELECTION_PARAMS_C)
  byte    CELL_RESELECT_HYSTERESIS;
  byte    MS_TXPWR_MAX_CCH;
  byte    RXLEV_ACCESS_MIN;
  byte    power_offset_valid;
  byte    POWER_OFFSET;
  byte    NECI;
  byte    ACS;
  byte    CELL_RESELECT_PARAM_IND;
  byte    CELL_BAR_QUALIFY;
  byte    CELL_RESELECT_OFFSET;
  byte    TEMPORARY_OFFSET;
  byte    PENALTY_TIME;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_RR_RACH_CONTROL_PARAMS_C)
  byte    max_retrans;
  byte    tx_integer;
  byte    REESTABLISHMENT_ALLOWED;
  byte    CELL_BAR_ACCESS;
  byte    access_control[2];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_RR_CONTROL_CHANNEL_PARAMS_C)
  byte    ATT;
  byte    BS_AG_BLKS_RES;
  byte    BS_CC_CHANS;
  byte    BS_CCCH_SDCCH_COMB;
  byte    BS_PA_MFRMS;
  byte    T3212_timeout;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_RR_CELL_OPTIONS_C)
  byte    PWRC;
  byte    DTX;
  byte    RADIO_LINK_TIMEOUT;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_RR_CELL_INFO_C)
  word  BCCH_ARFCN;
  byte  BSIC[2];
  word  cell_id;
  byte  lai[5];
  byte  cell_selection_priority;
  byte  NCC_permitted;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_RR_CIPHERING_MODE_C)
  byte ciphering_flag;
  byte ciphering_algorithm;
LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  word  BCCH_ARFCN;
  byte  RXLEV_average;
  int   C1;
  int   C2;
  byte  five_second_timer_state;
  byte  cell_reselect_status;
  byte  serving_lai;
} rr_log_surround_info_T;

#define RR_LOG_MAX_NUM_NEIGHBORS 6

LOG_RECORD_DEFINE(LOG_GSM_RR_CELL_RESELECTION_MEASMNTS_C)
  word                    serving_BCCH_ARFCN;
  byte                    serving_RXLEV_average;
  int                     serving_C1;
  int                     serving_C2;
  byte                    serving_five_second_timer_state;
  byte                    cell_reselect_status;
  byte                    hysteresis_used;
  byte                    recent_cell_reselection;
  byte                    neighbor_cell_count;
  rr_log_surround_info_T  neighbor_cell_info[RR_LOG_MAX_NUM_NEIGHBORS];
LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  /* ARFCN of the cell */
  word                    ARFCN;

  word                    location_area_code;

  /* last received RXLEV value for the cell */
  byte                    RXLEV_average;

  /* 0 - Low priority, 1 - Normal Priority, 2 - Barred */
  byte                    cell_type;

  /* 1 - Cell supports GPRS (when PS is supported), 0 otherwise */
  byte                    GPRS_indicator;

} rr_log_cell_entry_T;

#define RR_LOG_MAX_CELLS_IN_PLMN_LIST_ENTRY 3

typedef PACKED struct PACKED_POST
{
  /* MCC in BCD format */
  byte                    MCC[3];
  /* MNC in BCD format */
  byte                    MNC[3];

  /* number of valid entries in cells array */
  byte                    num_frequencies;

  /* array of cell information for PLMN */
  rr_log_cell_entry_T     cells[RR_LOG_MAX_CELLS_IN_PLMN_LIST_ENTRY];
} rr_log_plmn_list_entry_T;

#define RR_LOG_MAX_PLMN_LIST_ENTRIES 10

LOG_RECORD_DEFINE(LOG_GSM_RR_AVAILABLE_PLMN_LIST_C)
  byte                      plmn_list_count;
  rr_log_plmn_list_entry_T  available_plmn_list[RR_LOG_MAX_PLMN_LIST_ENTRIES];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_RR_DSC_COUNTER_C)
  int                     DSC_counter;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GSM_RR_S_COUNTER_C)
  byte                    S_counter;
LOG_RECORD_END


LOG_RECORD_DEFINE(LOG_GPRS_INDICATORS_C)
  byte ra_colour;
  byte si13_position;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_GENERAL_PARAMS_C)
  byte spgc_ccch_sup;
  byte priority_access_thr;
  byte network_control_order;
  byte rac;
  byte sgsnr;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_CELL_OPTIONS_C)
  byte nmo;
  byte t3168;
  byte t3192;
  byte drx_timer_max;
  byte access_burst_type;
  byte control_ack_type;
  byte bs_cv_max;
  byte pan_dec;
  byte pan_inc;
  byte pan_max;
  byte bss_paging_coordination;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_POWER_CONTROL_PARAMS_C)
  byte alpha;
  byte t_avg_w;
  byte t_avg_t;
  byte pc_meas_chan;
  byte n_avg_i;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_MOBILE_ALLOCATION_C)
  byte hsn;
  byte gprs_ma_arfcn_count;
  word arfcn[64];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_PBCCH_DESCRIPTION_C)
  byte pb;
  byte tsc;
  byte tn;
  word arfcn;
  byte maio;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_GRR_STATE_C)
  byte active_state;
  byte grr_state;
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_RR_CELL_RESELECTION_PARAMS_C)
  byte    PBCCH_PRESENT;
  byte    RXLEV_ACCESS_MIN;
  byte    MS_TXPWR_MAX_CCH;
  byte    HSC_PARAM_PRESENT;
  byte    HCS_THR;
  byte    PRIORITY_CLASS;
  byte    CELL_RESELECT_HYST;
  byte    C31_HYST;
  byte    C31_QUAL;
  byte    RA_RESELECT_HYST_PRESENT;
  byte    RA_RESELECT_HYSTERESIS;
  byte    RA_CODE;
  byte    RA_COLOUR;
  byte    CELL_BAR_ACCESS_2;
  byte    NECI;
  byte    ACS;
  byte    POWER_OFFSET_PRESENT;
  byte    POWER_OFFSET;
  byte    OPT_RESELECT_PARAM_IND;
  byte    CELL_BAR_QUALIFY;
  byte    CELL_BAR_ACCESS;
LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  word  BCCH_ARFCN;
  word  PBCCH_ARFCN;
  byte  Priority_Class;
  byte  RXLEV_average;
  int   C1;
  int   C2;
  int   C31;
  int   C32;
  byte  five_second_timer_state;
  byte  cell_reselect_status;
  byte  serving_lai_or_ra;
} rr_log_gprs_surround_info_T;

LOG_RECORD_DEFINE(LOG_GPRS_RR_CELL_RESELECTION_MEASMNTS_C)
  word                    serving_BCCH_ARFCN;
  word                    serving_PBCCH_ARFCN;
  byte                    serving_PRIORITY_CLASS;
  byte                    serving_RXLEV_average;
  int                     serving_C1;
  int                     serving_C2;
  int                     serving_C31;
  int                     serving_C32;
  byte                    serving_five_second_timer_state;
  byte                    cell_reselect_status;
  byte                    recent_cell_reselection;
  byte                    neighbor_cell_count;
  rr_log_gprs_surround_info_T  neighbor_cell_info[RR_LOG_MAX_NUM_NEIGHBORS];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_GPRS_RR_PSI1_C)
  byte                            pbcch_change_mark;
  byte                            psi_count_lr;
  byte                            psi_count_hr;
  byte                            measurement_order;
  word                            acc_contr_class;
  byte                            max_retrans[4];
  byte                            s;
  byte                            tx_int;
  byte                            persistence_level[4];
  byte                            bs_pcc_rel;
  byte                            bs_pbcch_blks;
  byte                            bs_prach_blks;
  byte                            bs_pag_blks_res;
  byte                            psi_status_ind;
  byte                            sgsnr;
  byte                            mscr;
LOG_RECORD_END


LOG_RECORD_DEFINE(LOG_GPRS_RR_PSI2_C)
  byte                     plmn_id[3];
  word                     location_area_code;
  byte                     rac;
  word                     cell_identity;
  byte                     non_gprs_cell_option_valid;
  byte                     att;
  byte                     t3212;
  byte                     neci;
  byte                     pwrc;
  byte                     dtx;
  byte                     radio_lnk_to;
  byte                     bs_ag_blks_res;
  byte                     bs_cc_chans;
  byte                     bs_ccch_sdcch_comb;
  byte                     bs_pa_mfrms;
  byte                     max_retrans;
  byte                     tx_integer;
  byte                     access_control;
  byte                     ms_txpwr_max_ccch;
LOG_RECORD_END

#define BITMASK_EXCLUSIVE_ACCESS    (1 << 0)
#define BITMASK_CELL_BAR_ACCESS_2   (1 << 1)

/* ncell psi3 fields */
#define BITMASK_SI13_PBCCH_RPESENT  (1 << 2)
#define BITMASK_PBCCH_PRESENT       (1 << 3)
#define BITMASK_SI13_EXTENDED       (1 << 4)
#define PBCCH_LOCATION              (3 << 5)

/* serving cell fields */
#define BITMASK_RANDOM_ACCESS       (1 << 2)
#define BITMASK_MULTIBAND_RPT       (3 << 3)

typedef PACKED struct PACKED_POST
{
  uint16                 bcch_arfcn;
  byte                   bsic;
  byte                   psi3_flags;
} psi3_ncell_t;

LOG_RECORD_DEFINE(LOG_GPRS_RR_PSI3_C)
  byte                   serving_cell_flags;
  byte                   ncells_nums;
  psi3_ncell_t           ncell_info[32];
LOG_RECORD_END


// -----------LOG_GSM_SACCH_RPT----------------
typedef PACKED struct PACKED_POST
{
  word BCCH_ARFCN;
  byte RXLEV_average;
  byte BSIC;
} sacch_measurement_T; /* used for conversion to non-quadband structure */

#define MAX_SACCH_SURROUND_MEASUREMENTS_SIZE 32 /* Matches MAX_NUMBER_OF_SURROUND_CELLS defined in rr_l1.h */

typedef PACKED struct PACKED_POST
{
  uint16              no_of_entries;
  sacch_measurement_T measurement[MAX_SACCH_SURROUND_MEASUREMENTS_SIZE];
} sacch_surround_measurements_T; /* used for conversion to non-quadband structure */

LOG_RECORD_DEFINE(LOG_GSM_RR_SACCH_RPT_C)
  word serving_BCCH_ARFCN;
  byte RXLEV_FULL_average;
  byte RXLEV_SUB_average;
  byte RXQUAL_FULL_average;
  byte RXQUAL_SUB_average;
  byte surr_cell_measurements[sizeof(sacch_surround_measurements_T)];
LOG_RECORD_END


// -----------LOG_GSM_RR_BA_LIST----------------

#define RR_LOG_BA_MAX_LEN 32 /* Matches BA_MAX_LAN defined in rr_l1.h */

LOG_RECORD_DEFINE(LOG_GSM_RR_BA_LIST_C)
   byte        no_of_entries;
   word        num[RR_LOG_BA_MAX_LEN];
LOG_RECORD_END

#define INVALID_LOG_ARFCN 0x0FFF /* omit the top 4 bits used for band logging */

LOG_RECORD_DEFINE(LOG_GSM_RR_RANDOM_ACCESS_REQUEST_C)
  uint8 establishment_cause;
  uint8 random_access_content;
LOG_RECORD_END

#ifdef FEATURE_WCDMA

LOG_RECORD_DEFINE(LOG_GPRS_RR_3G_MEASUREMENT_PARAMS_C)
  byte qsearch_p;
  byte qsearch_i;
  byte qsearch_c_initial;
  byte fdd_qoffset;
  byte fdd_gprs_qoffset;
  byte fdd_qmin;
  byte _3g_search_prio;
  byte _3g_ba_ind;
  byte fdd_rep_quant;
  byte fdd_multirat_reporting;
  byte fdd_reporting_offset;
  byte fdd_reporting_threshold;
  byte report_type;
  byte index_start_3g;
  byte qsearch_c;
  byte mi_3g_wait;
LOG_RECORD_END

typedef PACKED struct PACKED_POST
{
  uint16 uarfcn;
  uint16 scrambling_code;
  byte diversity;
  int16 rscp;
  byte _5_second_timer_running;
} _3g_best_six_list_t;

LOG_RECORD_DEFINE(LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C)
  byte                no_of_entries;
  _3g_best_six_list_t _3g_best_six_list[6];
LOG_RECORD_END

typedef PACKED struct PACKED_POST {
  word    uarfcn;
  word    scrambling_code;
  boolean diversity;
  uint16   squalmin;
  uint16   srxlevmin;
} reject_thr_3g_cell_t;

typedef PACKED struct PACKED_POST {
  word                         uarfcn;
  word                         scrambling_code;
  boolean                      diversity;
  rrc_interrat_reject_reason_T reject_reason;
  uint32                       reject_timer_value;
} reject_timer_3g_cell_t;

/* This packet is generated whenever a UTRAN cell is rejected by the
WCDMA */
LOG_RECORD_DEFINE(LOG_GSM_IDLE_MODE_3G_RESELECT_REJ_C)
  byte num_3g_thr_cells;
  byte num_3g_timer_cells;
  reject_thr_3g_cell_t reject_thr_3g_cells[6];
  reject_timer_3g_cell_t reject_timer_3g_cells[6];
LOG_RECORD_END

/* This packet is generated with the best 24 cells 8 each from three 3 different
frequencies every SACCH block - reduce from 96 to 24*/

typedef PACKED struct PACKED_POST {
  byte CELL_INDEX;
  word UARFCN;
  word SC_CODE;
  byte DIVERSITY;
  word PN_POS;
  int16 RSSI;
  int16 ENERGY;
  word EXPIRES;
  byte REPORT_FIELDS;
} rr_3g_report_t;

#define RR_LOG_MAX_WCDMA_STORED_DED_MEASUREMENTS  32  /*matches the no. of MAX_WCDMA_STORED_DED_MEASUREMENTS defined in rr_defs.h*/

LOG_RECORD_DEFINE(LOG_GSM_DED_3G_MEASUREMENTS_C)
  byte NUM_CELLS;
  rr_3g_report_t rr_3g_report[RR_LOG_MAX_WCDMA_STORED_DED_MEASUREMENTS];
LOG_RECORD_END

#endif /* FEATURE_WCDMA */

typedef enum
{
  RR_LOG_RAT_UNDEFINED = 0x00,
  RR_LOG_RAT_GSM       = 0x01,
  RR_LOG_RAT_WCDMA     = 0x02,
  RR_LOG_RAT_LTE       = 0x03,
  RR_LOG_RAT_MAX
} rr_log_rat_e;

typedef enum
{
  RR_LOG_G2L_RESEL_STATUS_SUCCESS                           = 0x00,
  RR_LOG_G2L_RESEL_STATUS_ABORTED                           = 0x01,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_UNKNOWN             = 0x02,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_CELL_BARRED         = 0x03,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_PLMN_MISMATCH       = 0x04,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_FORBIDDEN_TA        = 0x05,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_ACQ_FAILED          = 0x06,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_DL_WEAK             = 0x07,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_CFG_FAILURE         = 0x08,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_CELL_SELECT_FAILURE = 0x09,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_FREQ_BARRED         = 0x0a,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_FORBIDDEN_CSG_ID    = 0x0b,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_SIB_TIMEOUT_ERROR   = 0x0c,
  RR_LOG_G2L_RESEL_STATUS_FAILURE_CAUSE_SIB_INVALID_ERROR   = 0x0d,
  RR_LOG_G2L_RESEL_STATUS_MAX
} rr_log_g2l_resel_status_e;

typedef enum
{
  RR_LOG_G2L_REDIR_STATUS_SUCCESS = 0x00,
  RR_LOG_G2L_REDIR_STATUS_ABORTED = 0x01,
  RR_LOG_G2L_REDIR_STATUS_FAILURE = 0x02,
  RR_LOG_G2L_REDIR_STATUS_MAX
} rr_log_g2l_redir_status_e;

typedef enum
{
  RR_LOG_G2W_REDIR_STATUS_SUCCESS = 0x00,
  RR_LOG_G2W_REDIR_STATUS_ABORTED = 0x01,
  RR_LOG_G2W_REDIR_STATUS_FAILURE = 0x02,
  RR_LOG_G2W_REDIR_STATUS_MAX
} rr_log_g2w_redir_status_e;

typedef enum
{
  RR_LOG_G2T_REDIR_STATUS_SUCCESS = 0x00,
  RR_LOG_G2T_REDIR_STATUS_ABORTED = 0x01,
  RR_LOG_G2T_REDIR_STATUS_FAILURE = 0x02,
  RR_LOG_G2T_REDIR_STATUS_MAX
} rr_log_g2t_redir_status_e;


#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/* LOG_GSM_RR_IRAT_MEASUREMENTS_C */

#define RR_LOG_IRAT_RAT_UNDEFINED 0U
#define RR_LOG_IRAT_RAT_WCDMA     1U
#define RR_LOG_IRAT_RAT_LTE       2U

typedef PACK(struct)
{
  uint16  scrambling_code;
  boolean diversity;
  int16   rscp;
  int16   ecnox2;
  int16   rssi;
  uint16  pn_offset;
} rr_log_wcdma_measurement_t;

#define RR_LOG_MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY 8U

typedef PACK(struct)
{
  uint16 uarfcn;
  uint8  no_of_entries;
  rr_log_wcdma_measurement_t entries[RR_LOG_MAX_WCDMA_MEASUREMENTS_PER_FREQUENCY];
} rr_log_wcdma_measurements_t;

#define RR_LOG_MAX_LTE_MEASUREMENTS_PER_FREQUENCY 16U

typedef PACK(struct)
{
  uint16 pcid;
  int16  rsrp;
  int16  rsrq;
} rr_log_lte_measurement_t;

typedef PACK(struct)
{
  uint16 earfcn;
  uint8  no_of_entries;
  rr_log_lte_measurement_t entries[RR_LOG_MAX_LTE_MEASUREMENTS_PER_FREQUENCY];
} rr_log_lte_measurements_t;

typedef union
{
  rr_log_wcdma_measurements_t wcdma;
  rr_log_lte_measurements_t   lte;
} rr_log_irat_measurements_u;

typedef PACK(struct)
{
  log_hdr_type               hdr;
  uint8                      version;
  uint8                      rat;
  uint8                      mac_status;
  rr_log_irat_measurements_u measurements;
} rr_log_irat_measurements_t;

/* LOG_GSM_RR_IRAT_PRIORITY_LIST_C */

#define RR_LOG_MAX_IRAT_PRIORITY_LIST_LENGTH 12

typedef PACK(struct)
{
  uint8 rat;
  union
  {
    PACK(struct)
    {
      uint16  uarfcn;
      uint16  scrambling_code;
      boolean diversity;
      int16   rscp;
      int16   ecno;
      int16   rssi;
      uint16  pn_offset;
    } wcdma;
    PACK(struct)
    {
      uint16 earfcn;
      uint16 pcid;
      int16  rsrp;
      int16  rsrq;
    } lte;
  } cell;

  boolean rank_resel_timer_running;
  boolean thresh_high_timer_running;
  boolean thresh_low_timer_running;
  boolean h_prio_timer_running;

  int16 s_value;
  uint8 priority;
  int16 thresh_high;
  int16 thresh_low;
} rr_log_irat_priority_list_entry_t;

typedef PACK(struct)
{
  log_hdr_type hdr;
  uint8        version;
  PACK(struct)
  {
    uint16 arfcn;
    uint8  band;
    uint8  priority;
    uint8  rxlev;
    int16  s_value;
  } scell;
  uint8                             length;
  rr_log_irat_priority_list_entry_t entries[RR_LOG_MAX_IRAT_PRIORITY_LIST_LENGTH];
} rr_log_irat_priority_list_t;

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

#define LOG_MAX_CA_LEN    64

typedef PACK(struct)
{
  log_hdr_type            hdr;
  uint8                   version;
  uint16                  arfcn;
  uint8                   bsic;
  byte                    plmn_id[3];
  uint16                  cell_id;
  uint16                  location_area_code;
  boolean                 si_mask_valid;
  uint16                  broadcast_si_mask;
  uint8                   ra_color;
  uint8                   si13_position;
  uint8                   reselection_offset;
  uint8                   t3212;
  uint8                   rx_lev_min;
  uint8                   MSCR;
  boolean                 si2_received;
  uint8                   si_ca_length;
  uint16                  si1_cell_allocation[LOG_MAX_CA_LEN];
}rr_log_fake_cell_data_t;


#endif /* RR_LOG_TYPES_H */
