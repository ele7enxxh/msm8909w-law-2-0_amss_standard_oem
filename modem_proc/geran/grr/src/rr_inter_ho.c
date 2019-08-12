/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Inter Cell Handover

GENERAL DESCRIPTION
  This module handles the inter cell handover processing

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_inter_ho.c_v   1.8   30 May 2002 14:34:40   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_inter_ho.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
Apr/18/12   sjw     Implement 3GPP spec change GP-111808 (to replace GP-052760)
Jun/14/10   sjw     Reject a W2GHO command if ciphering config is inconsistent
Jul/22/09   sjw     Added support for FEATURE_UMTS_VOICE_CIPHERING_IND
May/06/09   sjw     Moved handover_details and assignment_details out of rr_data.c
May/05/09   sjw     Reworked interfaces between G2G and G2W handover code
Mar/09/09   sjw     Relocated rr_channel_mode_supported() to rr_channel_mode.c
Feb/13/09   sjw     Reworked local storage for ciphering module data
Nov/07/08   sjw     Allow SAPI3 control to decide whether release is required
May/17/04   VT      In rr_inter_cell_handover_control(), in states
                      HO_AWAITING_PHYSICAL_RECONNECTION and
                      HO_AWAITING_RECONNECTION, changed the ASSERT for unkown
                      handover_type to MSG_ERROR.
05/07/04    gc      Enhanced Trace Buffer Code
03/05/04    gc      CR31210: Fixes to W2G HO Failure
Feb/27/04    VT     In rr_inter_cell_handover_control(), filling the correct value for
                      handover_details.band_indicator
                      if FEATURE_GSM_QUADBAND is not defined.
12/09/03    guru    Added SI Optimisation Changes
05/08/03    he      Used separate debug_str buffer for logging.
02/27/03    RG      Release SAPI3 link (when present) during an inter cell handover
02/17/03    JAC     lint changes
02/14/03    RG      Added support for G2W blind Handover
11/12/02    JAC     Put back cipher command check in handover
11/07/02    JAC     Added support for AMR in handover command
11/04/02    Rao     Added Vocoder control related changes.
                    Also WCDMA to GSM Ciphering related changes.
10/30/02    Rao     Fix related to setting the gsm_only flag during InterRAT HO.
10/28/02    Rao     Fix related to setting the service domain from Activation rsp.
10/15/02    Rao     changed the RRC interrat measurement Details copy from memcp.
09/03/02    rmc     Need to handle MDL_ERROR_IND as well as DL_RELEASE_IND
08/30/02    Rao     Fixed the problem which is causing Handovers Failures.
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
08/22/02    Rao     Fixed the issue of setting the PLMN_id from Activation Rsp.
08/16/02    Rao     Modified to send the correct HO reason log event.
08/15/02    Rao     Fixed couple of InterRAT HO issues found in Integration.
08/07/02    he      Reseting TC counter in system information database structure.
07/20/02    Rao     Added InterRAT ( WCDMA To GSM )  Handover support
07/29/02    rmc     Adding logging support for HandoverStart/HandoverEnd events
05/30/02    rmc     Compilation warnings for MULTIBAND_SUPPORT flag
05/22/02    rmc     Use MM_RR_HANDOVER as sync reason for dual mode
05/15/02    JAC     set link status after DL_RESUME_CNF
02/05/02    rmc     QXDM Cleanup + fix from GSM L1 for T3124
01/30/02    RMC     Added support to log channel configuration when it changes
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include <string.h>
#include "rr_seg_load.h"
#include "rr_inter_ho.h"
#include "ms.h"
#include "rr_l2.h"
#include "rr_decode_ie.h"
#include "rr_l1_send.h"
#include "rr_l2_send.h"
#include "rr_general.h"
#include "rr_init.h"
#include "cause.h"
#include "aconfig.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rr_log.h"
#include "rr_mm_send.h"
#include "rr_gprs_conn_establish.h"
#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "rr_cell_selection.h"
#include "rr_sys_info.h"
#include "rr_cipher.h"
#include "gprs_mem.h"
#include "rr_channel_mode.h"
#include "rr_sapi3.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
#include "rr_rrc_send.h"
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
#include "rr_mode.h"
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#include "rr_ded_meas_reporting.h"
#include "rr_g2w_handover.h"
#include "rr_sys_info_pcell.h"
#include "rr_ded_config.h"
#include "rr_sim.h"
#include "rr_ps_domain.h"
#ifdef FEATURE_GSM_DTM
#include "rr_nv.h"
#endif /* FEATURE_GSM_DTM */
#ifdef FEATURE_GSM_TDS
#include "rr_tds_send.h"
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
#include "rr_msgr.h"
#endif /* FEATURE_LTE */
#include "rr_multi_sim.h"
#include "rr_conn_establish.h"
#include "stringl/stringl.h"
#ifdef FEATURE_SGLTE
#include "rr_sglte.h"
#endif /* FEATURE_SGLTE */

#define RR_HO_SYSTEM_DELAY 0
#define T3124_DURATION_SDCCH (675 + RR_HO_SYSTEM_DELAY)
#define T3124_DURATION_OTHER (320 + RR_HO_SYSTEM_DELAY)

#define HANDOVER_COMPLETE_MSG_LENGTH 8
#define HANDOVER_FAILURE_MSG_LENGTH  3

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
/* Maximum number of the GSM commands */
#define RR_X2G_HO_MAX_GSM_COMMANDS 4 /* define as 4 for now, it should only be one */
/* Maximum length of the GSM Command */
#define RR_X2G_HO_GSM_MESSAGE_LENGTH 256
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

typedef enum
{
  HANDOVER_INACTIVE,
  AWAITING_LOCAL_END_SUSPENSION,
  AWAITING_HANDOVER_INDICATION,
  AWAITING_RANDOM_ACCESS_CONFIRMATION,
  AWAITING_HANDOVER_CONFIRM,
  AWAITING_PHYSICAL_INFORMATION,
  AWAITING_STOP_CONFIRMATION,
  AWAITING_HANDOVER_COMPLETION,
  AWAITING_L2_RESUME_CONFIRM,
  HO_AWAITING_PHYSICAL_RECONNECTION,
  HO_AWAITING_RECONNECTION,
  RR_W2G_HO_GOING_INACTIVE,
  RR_W2G_HO_GOING_ACTIVE,
  RR_W2G_HO_PROCESS_NAS_DATA,
  RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP,
  RR_HANDOVER_STATE_COUNT
} handover_state_T;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
typedef enum
{
  RR_X2G_HO_FAILURE_INVALID_STATE,
  RR_X2G_HO_FAILURE_PROTOCOL_ERROR,
  RR_X2G_HO_FAILURE_INVALID_CONFIGURATION,
  RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE,
  RR_X2G_HO_FAILURE_INVALID_CIPHER_ALGORITHM,
  RR_X2G_HO_FAILURE_MAX
} rr_x2g_ho_failure_e;

/* GSM command type */
typedef struct
{
  /* Length of the command */
  uint8 message_length;
 
  /* Message payload */
  uint8 payload[RR_X2G_HO_GSM_MESSAGE_LENGTH];
} rr_x2g_ho_gsm_command_t;

/* GSM command list type */
typedef struct
{
  /* Number of the GSM Commands */
  uint8                   number_of_commands;
 
  /* GSM command list */
  rr_x2g_ho_gsm_command_t gsm_commands[RR_X2G_HO_MAX_GSM_COMMANDS];
} rr_x2g_ho_gsm_command_list_t;         
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

typedef struct
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  /* X2G HO failure reason */
  rr_x2g_ho_failure_e          rr_x2g_ho_failure_reason;
  /* Flag to indicate if RR received RR_ABORT_REQ from MM */
  boolean                      rr_mm_abort_received;
  /* Flag to indicate if RR received Handover Abort from other RAT */
  boolean                      rr_interrat_ho_abort_received;
  /* GSM Command List received from RRC in Inter RAT Handover Req */
  rr_x2g_ho_gsm_command_list_t gsm_command_list;
  rr_l1_band_indicator_T       rr_x2g_ho_band_ind;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

  /* Message Queues to be used to store NAS Messages during WCDMA to GSM Handover */
  rr_w2g_nas_message_entry_T *rr_w2g_nas_msg_queue;
  rr_w2g_nas_message_entry_T *rr_w2g_rrc_msg_queue;

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

  handover_state_T   handover_state;
  handover_state_T   old_handover_state;
  rr_internal_band_T old_band;
  byte               handover_cause;
  byte               last_timing_advance;
  dword              observed_time_difference;
  boolean            panic_reset_pending;

#ifdef FEATURE_LTE
  lte_irat_dedicated_priority_info_s dedicated_priority_info;
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
  boolean                          tds_pseudo_fr_active;
  tdsrr_ho_pseudo_fast_return_type tds_pseudo_fr_info;
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/
} rr_inter_ho_data_t;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
#define _RR_INTER_HO_DATA_INIT                                         \
  {                                                                    \
    RR_X2G_HO_FAILURE_INVALID_STATE, /*rr_x2g_ho_failure_reason*/      \
    FALSE,                           /*rr_mm_abort_received*/          \
    FALSE,                           /*rr_interrat_ho_abort_received*/ \
    {0},                             /*gsm_command_list*/              \
    RR_L1_DCS_1800,                  /*rr_x2g_ho_band_ind*/            \
    NULL,                            /*rr_w2g_nas_msg_queue*/          \
    NULL,                            /*rr_w2g_rrc_msg_queue*/          \
    HANDOVER_INACTIVE,               /*handover_state*/                \
    HANDOVER_INACTIVE,               /*old_handover_state*/            \
    BAND_NONE,                       /*old_band*/                      \
    0,0,0                                                              \
  }
#else /* FEATURE_WCDMA || FEATURE_GSM_TDS */
#define _RR_INTER_HO_DATA_INIT                                         \
  {                                                                    \
    RR_X2G_HO_FAILURE_INVALID_STATE, /*rr_x2g_ho_failure_reason*/      \
    FALSE,                           /*rr_mm_abort_received*/          \
    FALSE,                           /*rr_interrat_ho_abort_received*/ \
    {0},                             /*gsm_command_list*/              \
    RR_L1_DCS_1800,                  /*rr_x2g_ho_band_ind*/            \
    HANDOVER_INACTIVE,               /*handover_state*/                \
    HANDOVER_INACTIVE,               /*old_handover_state*/            \
    BAND_NONE,                       /*old_band*/                      \
    0,0,0                                                              \
  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
#else /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#define _RR_INTER_HO_DATA_INIT                                         \
  {                                                                    \
    HANDOVER_INACTIVE,               /*handover_state*/                \
    HANDOVER_INACTIVE,               /*old_handover_state*/            \
    BAND_NONE,                       /*old_band*/                      \
    0,0,0                                                              \
  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

static rr_inter_ho_data_t rr_inter_ho_data[NUM_GERAN_DATA_SPACES] =
                    { INITIAL_VALUE(_RR_INTER_HO_DATA_INIT) };
#undef _RR_INTER_HO_DATA_INIT

/*****************************************************************************
***
***     Function Prototypes
***
*****************************************************************************/

static void rr_ho_reset_internal_variables(handover_details_T *handover_details_ptr, const gas_id_t gas_id);
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
static void rr_w2g_process_stored_nas_messages(const gas_id_t gas_id);
static void rr_w2g_store_rrc_message(const rr_data_req_T *, const gas_id_t gas_id);
static void rr_w2g_store_mm_message(const rr_data_req_T *, const gas_id_t gas_id);
static void rr_w2g_empty_rrc_message_queue(const gas_id_t gas_id);
static void rr_w2g_empty_mm_message_queue(const gas_id_t gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
static void rr_w2g_store_gsm_commands(const rr_interrat_handover_req_type *irat_ho_req, const gas_id_t gas_id);
static void rr_convert_x2g_ho_gsm_cmds_to_wcdma(rr_x2g_ho_gsm_command_list_t *x2g_cmd_list, rrc_rr_gsm_command_list_type *w_gsm_cmd_list);
static rrc_rr_failure_cause_e_type rr_convert_rr_x2g_ho_failure_cause_to_wcdma(rr_x2g_ho_failure_e rr_x2g_cause);
static boolean rr_store_nas_data_ind(const void *, const gas_id_t gas_id);
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
static void rr_l2g_store_gsm_commands(const geran_grr_lte_ho_req_t *lte_ho_req, const gas_id_t gas_id);
static geran_grr_lte_ho_failure_cause_type_e rr_convert_rr_x2g_ho_failure_cause_to_lte(rr_x2g_ho_failure_e rr_x2g_cause);
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
static void rr_t2g_store_gsm_commands(tdsrr_interrat_handover_req_type *tds_ho_req, const gas_id_t gas_id);
static void rr_convert_x2g_ho_gsm_cmds_to_tds(rr_x2g_ho_gsm_command_list_t *x2g_cmd_list, tdsrrc_rr_gsm_command_list_type *tds_cmd_list);
static tdsrrc_rr_failure_cause_e_type rr_convert_rr_x2g_ho_failure_cause_to_tds(rr_x2g_ho_failure_e rr_x2g_cause);
static boolean rr_store_tds_nas_data_ind(const void *message_ptr, const gas_id_t gas_id);
#endif /*FEATURE_GSM_TDS*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_ps_access_data_t*
 */
static rr_inter_ho_data_t *rr_inter_ho_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_inter_ho_data[as_index];
}

static sys_ho_type_e_type rr_map_rr_l1_handover_type_to_sys_ho_type(
  rr_l1_handover_type_T rr_l1_handover_type
)
{
  switch (rr_l1_handover_type)
  {
    case RR_L1_GSM_GSM: return SYS_HO_TYPE_GSM_GSM;
#ifdef FEATURE_WCDMA
    case RR_L1_WCDMA_GSM: return SYS_HO_TYPE_UMTS_GSM;
    case RR_L1_GSM_WCDMA: return SYS_HO_TYPE_GSM_UMTS;
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
    case RR_L1_TDS_GSM: return SYS_HO_TYPE_NONE;
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
    case RR_L1_LTE_GSM: return SYS_HO_TYPE_LTE_GSM_SRVCC;
#endif /* FEATURE_LTE */
    default:
    {
      MSG_ERROR("Unexpected handover type: %d", 0, 0, 0);
      return SYS_HO_TYPE_NONE;
    }
  }
}

/*===========================================================================

FUNCTION  rr_handover_state_name
  Returns handover state name in readable form
===========================================================================*/
static char * rr_handover_state_name(handover_state_T hs)
{
  char *s = "";

  switch (hs)
  {
    case (HANDOVER_INACTIVE):
      s = "Inactive";
      break;

    case (AWAITING_LOCAL_END_SUSPENSION):
      s = "AwaitSuspend";
      break;

    case (AWAITING_HANDOVER_INDICATION):
      s = "AwaitHOInd";
      break;

    case (AWAITING_RANDOM_ACCESS_CONFIRMATION):
      s = "AwaitRAConf";
      break;

    case (AWAITING_HANDOVER_CONFIRM):
      s = "AwaitHOConf";
      break;

    case (AWAITING_PHYSICAL_INFORMATION):
      s = "AwaitPhysInfo";
      break;

    case (AWAITING_STOP_CONFIRMATION):
      s = "AwaitStopConf";
      break;

    case (AWAITING_HANDOVER_COMPLETION):
      s = "AwaitHOCompl";
      break;

    case (HO_AWAITING_PHYSICAL_RECONNECTION):
      s = "AwaitPhysRecon";
      break;

    case (HO_AWAITING_RECONNECTION):
      s = "AwaitRecon";
      break;

    case (RR_W2G_HO_GOING_INACTIVE):
      s = "GoingInactive";
      break;

    case (RR_W2G_HO_GOING_ACTIVE):
      s = "GoingActive";
      break;

    case (RR_W2G_HO_PROCESS_NAS_DATA):
      s = "ProcessNasData";
      break;

    case (RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP):
      s = "AwaitingActivationRsp";
      break;

    case (AWAITING_L2_RESUME_CONFIRM):
      s = "AwaitL2ResumeCnf";
      break;

    default:
      s = "Unknown";
      break;
  }

  return s;
} /* rr_handover_state_name */

void send_handover_failure_ota(
  byte RR_cause,
  const gas_id_t gas_id
)
{
  byte l3_msg[3];

  l3_msg[0] = RADIO_RESSOURCES;
  l3_msg[1] = HANDOVER_FAILURE;
  l3_msg[2] = RR_cause;

  MSG_GERAN_HIGH_1_G("OMsg(NW,HANDOVER_FAILURE(%d)",(int)RR_cause);

  rr_l2_send_l3_msg_ota(
    l3_msg,   // l3_msg
    3,        // l3_msg_len
    gas_id    // gas_id
  );
}


static void restart_l2_and_send_handover_failure_ota(
  byte RR_cause,
  rr_l2_resumption_method_t method,
  const gas_id_t gas_id
)
{
  byte l3_msg[3];

  l3_msg[0] = RADIO_RESSOURCES;
  l3_msg[1] = HANDOVER_FAILURE;
  l3_msg[2] = RR_cause;

  MSG_GERAN_HIGH_1_G("OMsg(NW,HANDOVER_FAILURE(%d)",(int)RR_cause);

  rr_l2_restart_l2_and_send_l3_msg_ota(
    method,   // method
    l3_msg,   // l3_msg
    3,        // l3_msg_len
    gas_id    // gas_id
  );
}

/**
 * Instruct the GSM mode controller to deactivate
 * due to some failure in an X2G handover procedure
 *
 * @param gas_id - access stratum identifier
 * @return New event for the handover state machine
 */
static rr_event_T deactivate_during_x2g_handover(const gas_id_t gas_id)
{
  /* advise the multi-SIM controller of the failure */
  rr_ms_x2g_procedure_failure(RR_X2G_HANDOVER_FAILURE, gas_id);

  /* and then perform a normal deactivation */
  return(rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id));
}

/**
 * Perform a handover failure operation (send the
 * appropriate message to the network) and if requested
 * initiate the reconnection of the old channel
 *
 * @param RR_cause reason for the handover failure
 *
 * @param reconnect      if true, use dl_reconnect_req to send HANDOVER_FAILURE
 *                       message, otherwise use a simple dl_data_req
 */
static void rr_perform_handover_failure(
  byte RR_cause,
  boolean reconnect,
  const gas_id_t gas_id
)
{
  if (reconnect)
  {
    restart_l2_and_send_handover_failure_ota(
      RR_cause,                   // RR_cause
      RR_L2_RECONNECT_REQUIRED,   // method
      gas_id                      // gas_id
    );
  }
  else
  {
    send_handover_failure_ota(
      RR_cause,   // RR_cause
      gas_id      // gas_id
    );
  }

} /* rr_perform_handover_failure */

/**
 * Sends the HANDOVER COMPLETE message to the network,
 * possibly after filling in the observed time difference
 * IE in the message
 *
 * @param handover_details_ptr - pointer to global handover info structure
 *
 * @param observed_time_difference
 *                            observed time difference value
 */
static void rr_complete_handover(
  handover_details_T *handover_details_ptr,
  const gas_id_t gas_id
)
{
  byte layer3_message[HANDOVER_COMPLETE_MSG_LENGTH];
  byte message_length;
  rr_inter_ho_data_t *rr_inter_ho_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(handover_details_ptr);

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  /* Encode the Protocol Discriminator and the Skip Indicator */
  layer3_message[0] = RADIO_RESSOURCES;

  /* Encode the message type */
  layer3_message[1] = HANDOVER_COMPLETE;

  /* Encode the RR cause as Normal Event */
  layer3_message[2] = RR_NORMAL_EVENT;

  /*******************************************************/
  /* If the ROT bit was set in HANDOVER COMMAND, report  */
  /* the mobile time difference to the Base Station      */
  /*******************************************************/
  if (handover_details_ptr->ROT == 1)
  {
    dword mobile_time_difference;

    /* and optional IE with IEI of 0x77 */
    layer3_message[3] = MOBILE_TIME_DIFFERENCE_IEI;
    /* TLV value of length 3 */
    layer3_message[4] = 3;

    /**********************************************************/
    /* BE CAREFUL : The OTD is expressed in bit period.        */
    /*               The TA expressed in bit period = T0       */
    /*             in half bit periods                         */
    /*         mobile_time_difference is a mod 2^21 number     */
    /*             in half bit periods                         */
    /* see 04.08 para 10.5.2.21a  and 05.10 appendix A.1      */
    /**********************************************************/
    mobile_time_difference =
      (rr_inter_ho_data_ptr->observed_time_difference * 2) +  rr_inter_ho_data_ptr->last_timing_advance;

    /* lowest 5 bits */
    layer3_message[7] = (byte) ((mobile_time_difference % 32) << 3);
    /* bits 6-13 */
    layer3_message[6] = (byte)((mobile_time_difference / 32) %  256);

    /* bits 14-21 */
    layer3_message[5] = (byte)(mobile_time_difference / (32 * 256));

    /* length = 3 + 5 for optional IE */
    message_length = HANDOVER_COMPLETE_MSG_LENGTH;
  }
  else
  {
    message_length = 3;
  }

  if (handover_details_ptr->handover_type == RR_L1_GSM_GSM)
  {
    MSG_GERAN_HIGH_1_G("OMsg(NW,HANDOVER_COMPLETE(%d)",(int)handover_details_ptr->ROT);
  }
#ifdef FEATURE_WCDMA
  else if (handover_details_ptr->handover_type == RR_L1_WCDMA_GSM)
  {
    MSG_GERAN_HIGH_1_G("OMsg(NW,W2G_HANDOVER_COMPLETE(%d)",(int)handover_details_ptr->ROT);
  }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  else if (handover_details_ptr->handover_type == RR_L1_TDS_GSM)
  {
    MSG_GERAN_HIGH_1_G("OMsg(NW,T2G_HANDOVER_COMPLETE(%d)",(int)handover_details_ptr->ROT);
  }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
  else if (handover_details_ptr->handover_type == RR_L1_LTE_GSM)
  {
    MSG_GERAN_HIGH_1_G("OMsg(NW,L2G_HANDOVER_COMPLETE(%d)", handover_details_ptr->ROT);
  }
#endif /* FEATURE_LTE */

  rr_send_dl_resume_req(
    handover_details_ptr->handover_type,    // resume_reason
    layer3_message,                         // layer3_message
    message_length,                         // message_length
    gas_id                                  // gas_id
  );
} /* rr_complete_handover */

/**
 * Processes the MPH_HANDOVER_IND signal from L1
 */
static void rr_process_handover_indication(
  rr_dedicated_data_T *dedic_ptr,
  const gas_id_t gas_id
)
{
  if (dedic_ptr != NULL)
  {
    handover_details_T *handover_details_ptr = &(dedic_ptr->handover_details);

    /* Update RR's global channel stores now */
    dedic_ptr->curr_channel_spec = handover_details_ptr->channel_spec;
    rr_set_link_status(SAPI0_LINK_IN_PROGRESS, gas_id);
    rr_switch_to_next_multirate_config(dedic_ptr);
    rr_set_traffic_reason_from_channel_mode(dedic_ptr->curr_channel_spec.channel_mode_1, gas_id);

    rr_send_dl_random_access_req(
      handover_details_ptr->handover_reference,   // access_reference
      DCCH,                                       // l2_channel_type
      gas_id                                      // gas_id
    );

    rr_log_random_access_req(
      1,        // attempt
      0x0,      // frameNum
      0xFF,     // reason
      gas_id    // gas_id
    );
  }

  return;
} /* rr_process_handover_indication */

#ifdef FEATURE_LTE

static void rr_send_lte_ho_failed_rsp(
  const geran_grr_lte_ho_failure_cause_type_e failure_cause,
  const gas_id_t gas_id
)
{
  geran_grr_lte_ho_failed_rsp_t rsp;
  errno_enum_type               result;

  memset(&rsp, 0, sizeof(geran_grr_lte_ho_failed_rsp_t));

  msgr_init_hdr(
    &rsp.hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_LTE_HO_FAILED_RSP
  );

  rsp.ho_fail_cause = failure_cause;

  result = rr_msgr_send(&rsp.hdr, sizeof(rsp), gas_id);

  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1("rr_msgr_send failed: %d", result);
  }
}

static void rr_send_lte_abort_ho_rsp(const gas_id_t gas_id)
{
  msgr_hdr_struct_type rsp;
  errno_enum_type      result;

  memset(&rsp, 0, sizeof(msgr_hdr_struct_type));

  msgr_init_hdr(
    &rsp,
    MSGR_GERAN_GRR,
    GERAN_GRR_LTE_ABORT_HO_RSP
  );

  result = rr_msgr_send(&rsp, sizeof(rsp), gas_id);

  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1("rr_msgr_send failed: %d", result);
  }
}

#endif /* FEATURE_LTE */

/**
 * Process a HANDOVER command received from the network
 *
 * @param dedic_ptr Pointer to the dedicated mode data store.
 * @param activate_new_cipher_key_if_ciphering_enabled Used during UTRAN to GSM
 *        handover, inidcates if RR should activate a new cipher key received
 *        in RR_SIM_UPDATE_REQ before the handover request is received from
 *        WCDMA or TDSCDMA.
 * @return New event for the handover state machine
 */
static rr_event_T rr_respond_to_handover_command(
  rr_dedicated_data_T * dedic_ptr,
  boolean               activate_new_cipher_key_if_ciphering_enabled,
  const                 gas_id_t gas_id
)
{
  handover_details_T           *handover_details_ptr;
  rr_event_T                    new_event = EV_NO_EVENT;
  byte                          ca_index;
  ie_descriptor_T              *current_IE;
  cell_description_T            cell_description;
  byte                          power_level;
  byte                          synchronisation_indication;
  boolean                       first_chan_desc_before_received  = FALSE;
  boolean                       second_chan_desc_before_received = FALSE;
  boolean                       decoding_ok = TRUE;
  frequency_list_T              frequency_list_after = {0};
  frequency_list_T              frequency_list_before = {0};
  boolean                       frequency_list_after_time = FALSE;
  boolean                       frequency_list_before_time = FALSE;
  boolean                       ma_after_time = FALSE;
  boolean                       ma_before_time = FALSE;
  cell_channel_description_T    cell_allocation = {0};
  mobile_allocation_T           ma_after;
  mobile_allocation_T           ma_before;
  channel_mode_T                channel_mode_1 = SIGNALLING_ONLY;
  channel_mode_T                channel_mode_2;
  word                          starting_time = 0;
  byte                          real_time_difference;
  byte                          timing_advance;
  boolean                       start_ciphering = FALSE;
  cipher_algorithm_T            cipher_algorithm;
  boolean                       freq_chan_seq_after_time = FALSE;
  boolean                       freq_chan_seq_before_time = FALSE;
  byte                          index;
  byte                          no_of_entries;
  boolean                       handover_failure = FALSE;
  byte                          RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
  byte                          rr_status = RR_PROTOCOL_ERROR_UNSPECIFIED;
  boolean                       multirate_ie_present = FALSE;
  channel_spec_T               *chan_spec_ptr;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  boolean                       cipher_mode_setting_present = FALSE;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  gprs_scell_info_t            *scell_info_ptr;
  boolean                       cipher_key_present;
  uint8                         msg_length = 0;
  rr_inter_ho_data_t           *rr_inter_ho_data_ptr;

  RR_NULL_CHECK_RETURN_PARAM(dedic_ptr, EV_NO_EVENT);

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info_ptr);

  if (dedic_ptr == NULL)
  {
    return(EV_NO_EVENT);
  }
  handover_details_ptr = &(dedic_ptr->handover_details);
  chan_spec_ptr        = &(handover_details_ptr->channel_spec);

  handover_details_ptr->channel_spec.number_of_ded_channels  = 1;
  handover_details_ptr->channel_spec.starting_time_valid     = FALSE;
  handover_details_ptr->channel_spec.before_parameters_valid = FALSE;
  handover_details_ptr->real_time_difference_valid           = FALSE;
  handover_details_ptr->timing_advance_valid                 = FALSE;

  rr_invalidate_multirate_config(&(dedic_ptr->next_amr_config));

  if (handover_details_ptr->handover_type == RR_L1_GSM_GSM)
  {
    /*************************************************************************/
    /* Some of the ie's are optional so we need to use the old configuration */
    /* if they are not present, we will overwrite these values if the ie is  */
    /* present                                                               */
    /*************************************************************************/
    handover_details_ptr->old_channel_mode = dedic_ptr->curr_channel_spec.channel_mode_1;

    handover_details_ptr->channel_spec.channel_mode_1 = dedic_ptr->curr_channel_spec.channel_mode_1;
    handover_details_ptr->channel_spec.channel_mode_2 = dedic_ptr->curr_channel_spec.channel_mode_2;

    handover_details_ptr->channel_spec.cipher_flag = dedic_ptr->curr_channel_spec.cipher_flag;
    handover_details_ptr->channel_spec.cipher_algorithm = dedic_ptr->curr_channel_spec.cipher_algorithm;
  }
#ifdef FEATURE_WCDMA
  else if (handover_details_ptr->handover_type == RR_L1_WCDMA_GSM)
  {
    /************************************************************************/
    /* TODO We cannot use the channel configuration since we just switched  */
    /* from WCDMA, we may need to explicitly check for the presence of some*/
    /* ies                                                                  */
    /************************************************************************/
    handover_details_ptr->channel_spec.cipher_flag = FALSE;
    // For InterRAT Handover Scenario this should be set to AMR
    handover_details_ptr->old_channel_mode = SPEECH_V3;
    // By Default initialize the new_channel_mode to AMR in case if this is not provided
    // in HO command.
    handover_details_ptr->new_channel_mode = SPEECH_V3;
    handover_details_ptr->channel_spec.channel_mode_1 = SPEECH_V3;
    MSG_GERAN_HIGH_0_G("For InterRAT (WCDMA to GSM HO) using AMR as old channel Mode");
  }
#endif /* #ifdef FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  else if (handover_details_ptr->handover_type == RR_L1_TDS_GSM)
  {
    /************************************************************************/
    /* TODO We cannot use the channel configuration since we just switched  */
    /* from TDSCDMA, we may need to explicitly check for the presence of some*/
    /* ies                                                                  */
    /************************************************************************/
    handover_details_ptr->channel_spec.cipher_flag = FALSE;
    // For InterRAT Handover Scenario this should be set to AMR
    handover_details_ptr->old_channel_mode = SPEECH_V3;
    // By Default initialize the new_channel_mode to AMR in case if this is not provided
    // in HO command.
    handover_details_ptr->new_channel_mode = SPEECH_V3;
    handover_details_ptr->channel_spec.channel_mode_1 = SPEECH_V3;
    MSG_GERAN_HIGH_0_G("For InterRAT (TDSCDMA to GSM HO) using AMR as old channel Mode");
  }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
  else if (handover_details_ptr->handover_type == RR_L1_LTE_GSM)
  {
    /************************************************************************/
    /* TODO We cannot use the channel configuration since we just switched  */
    /* from WCDMA, we may need to explicitly check for the presence of some*/
    /* ies                                                                  */
    /************************************************************************/
    handover_details_ptr->channel_spec.cipher_flag = FALSE;
    // For InterRAT Handover Scenario this should be set to AMR
    handover_details_ptr->old_channel_mode = SPEECH_V3;
    // By Default initialize the new_channel_mode to AMR in case if this is not provided
    // in HO command.
    handover_details_ptr->new_channel_mode = SPEECH_V3;
    handover_details_ptr->channel_spec.channel_mode_1 = SPEECH_V3;
    MSG_GERAN_HIGH_0_G("For InterRAT (LTE to GSM HO) using AMR as old channel Mode");
  }
#endif /* #ifdef FEATURE_LTE */
  else
  {
    MSG_GERAN_ERROR_0_G("Unknown handover type");
  }

  handover_details_ptr->valid_CA_received = FALSE;

  /****************************************************/
  /* get a pointer to the first IE of the OTA message */
  /*****************************************************/
  current_IE = rr_get_first_ie_and_length(
    HANDOVER_COMMAND,
    &msg_length,
    gas_id
  );

  /***************************************************/
  /* Decode the cell description Information Element */
  /***************************************************/
  rr_decode_cell_description(
    current_IE->start_IE,
    &cell_description,
    gas_id
  );

  handover_details_ptr->BSIC = cell_description.BSIC;
  handover_details_ptr->BCCH_ARFCN = cell_description.ARFCN;

  if (GET_ARFCN_BAND(handover_details_ptr->BCCH_ARFCN) == BAND_NONE)
  {
    /* the ARFCN belongs to a non-supported band */
    RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
    handover_failure = TRUE;
  }
  else
  {
    /***********************************************************/
    /* Decode the Description of the first channel, after time */
    /***********************************************************/
    current_IE = current_IE->next_IE;
    if ((current_IE != NULL) && (current_IE->start_IE != NULL))
    {
      channel_information_T channel_info_1_after = {0};

      rr_decode_channel_description_2(
        current_IE->start_IE,
        &channel_info_1_after,
        &decoding_ok,
        gas_id
      );

      if (decoding_ok == TRUE)
      {
        handover_details_ptr->channel_spec.channel_info_1_after = channel_info_1_after;

        if (channel_info_1_after.hopping_flag == FALSE)
        {
          /*
           * Check to see if the ARFCN is valid for our configuration
           * only do if no hopping as frequencies used for hopping are
           * checked elsewhere
           */
          ARFCN_T ARFCN = channel_info_1_after.frequency_list.channel[0];

          if (GET_ARFCN_BAND(ARFCN) == BAND_NONE)
          {
            /* the ARFCN belongs to a non-supported band */
            RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
            handover_failure = TRUE;
            MSG_GERAN_ERROR_1_G("Channel number=%d not supported",(int) ARFCN.num);
          }
        }
      }
    }
    else
    {
      decoding_ok = FALSE;
    }
  }

  /********************************************/
  /* Decode the Handover Reference            */
  /* Decode the power command and access type */
  /********************************************/
  if ((decoding_ok == TRUE) && (handover_failure == FALSE))
  {
    /* handover reference IE */
    current_IE = current_IE->next_IE;
    handover_details_ptr->handover_reference = *(current_IE->start_IE);

    /* power command and access type IE */
    current_IE = current_IE->next_IE;
    power_level = (*(current_IE->start_IE)) & 0x1F;
    handover_details_ptr->channel_spec.power_level = power_level;
  }

  /***************************************************************************/
  /* The following IE are optionals, so we have to check the IEI to find out */
  /* what they really are                                                    */
  /***************************************************************************/
  if ((decoding_ok == TRUE) && (handover_failure == FALSE))
  {
    current_IE = current_IE->next_IE;
    if (current_IE->start_IE != NULL)
    {
      synchronisation_indication = *(current_IE->start_IE) & 0x03;

      if (synchronisation_indication == 0x00)
      {
        handover_details_ptr->synchronisation_indication = NON_SYNCHRONISED;
      }

      else if (synchronisation_indication == 0x01)
      {
        handover_details_ptr->synchronisation_indication = SYNCHRONISED;
      }

      else if (synchronisation_indication == 0x02)
      {
        handover_details_ptr->synchronisation_indication = PRE_SYNCHRONISED;
      }

      else if (synchronisation_indication == 0x03)
      {
        handover_details_ptr->synchronisation_indication = PSEUDO_SYNCHRONISED;
      }

      handover_details_ptr->ROT = (byte) (*(current_IE->start_IE) & 0x04) >> 2;

      handover_details_ptr->NCI = (byte) (*(current_IE->start_IE) & 0x08) >> 3;
    }
    else
    {

      /************************************************************/
      /* The Synchronisation Indication element does not appear   */
      /* The assumed value is "NON_SYNCHRONISED" (04.08 9.1.15.1) */
      /************************************************************/

      handover_details_ptr->synchronisation_indication = NON_SYNCHRONISED;
    }
  }

  while ((decoding_ok == TRUE) && (handover_failure == FALSE) && (current_IE->next_IE != NULL))
  {
    current_IE = current_IE->next_IE;

    switch (current_IE->IEI)
    {
      case FREQ_SHORT_LIST_AFTER_TIME_IEI:
        /**********************************************/
        /* Decode the Frequency Short List after time */
        /**********************************************/
        if (current_IE->start_IE != NULL)
        {
          rr_decode_short_freq_list(
            current_IE->start_IE,
            &frequency_list_after,
            &decoding_ok,
            gas_id
          );

          handover_failure = rr_check_frequency_list(
            &frequency_list_after,
            gas_id
          );

          if (handover_failure)
          {
            /*
             * if channels not all in one band
             */
            RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
            ERR("HO:Mixed bands in FreqList",0,0,0);
          }
          else
          {
            frequency_list_after_time = TRUE;
          }
        }
        break;

      case FREQUENCY_LIST_AFTER_TIME_IEI:
        /*****************************/
        /* Decode the Frequency List */
        /* after time                */
        /*****************************/
        if (current_IE->start_IE != NULL)
        {
          // Only when HO_CMD length is sufficient, try to decode Frequency list.
          if (dedic_ptr->handover_cmd_length > msg_length)
          {
            MSG_GERAN_HIGH_1_G("HO command Length, %d ",dedic_ptr->handover_cmd_length );
            rr_decode_frequency_list(
              current_IE->start_IE,
              scell_info_ptr->gsm.BCCH_ARFCN,
              &frequency_list_after,
              &decoding_ok,
              gas_id
            );


            handover_failure = rr_check_frequency_list(
              &frequency_list_after,
              gas_id
            );

            if (handover_failure)
            {
              /*
               * if channels not all in one band
               */

              RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
              ERR("HO:Mixed bands in FreqList",0,0,0);
            }
            else
            {
              frequency_list_after_time = TRUE;
            }
          }
          else
          {
            MSG_GERAN_ERROR_1_G("Something wrong, HO command got corrupted, %d ",dedic_ptr->handover_cmd_length );
          }
        }
        break;

      case CELL_CHANNEL_DESCRIPTION_IEI:
        /***************************************************************/
        /* Decode the Cell Channel Description, ie the Cell Allocation */
        /* (CA) used for the decoding of the Mobile Allocation (MA)    */
        /* information                                                 */
        /***************************************************************/

        if (current_IE->start_IE != NULL)
        {
          rr_decode_cell_channel_description(
            current_IE->start_IE,
            &cell_allocation,
            &decoding_ok,
            gas_id
          );

          if (decoding_ok == TRUE)
          {
            handover_details_ptr->valid_CA_received = TRUE;
            handover_details_ptr->cell_channel_description.no_of_entries = cell_allocation.no_of_entries;

            for (ca_index = 0;ca_index < cell_allocation.no_of_entries; ca_index++)
            {
              handover_details_ptr->cell_channel_description.channel_number[ca_index] =
               cell_allocation.channel_number[ca_index];
            }
          }
        }
        break;

      case MODE_OF_FIRST_CHANNEL_IEI:
        /***************************************/
        /* Decode the Channel Mode Information */
        /* for the first channel               */
        /***************************************/
        if (current_IE->start_IE != NULL)
        {
          rr_decode_channel_mode(current_IE->start_IE,&channel_mode_1,&decoding_ok);

          if (decoding_ok == FALSE)
          {
            /********************************************************/
            /* As the information element is OPTIONAL, we ignore it */
            /********************************************************/
            decoding_ok = TRUE;
          }
          else
          {
            /******************************************************/
            /* The channel mode of the first previously allocated */
            /* channel is assumed                                 */
            /******************************************************/
            handover_details_ptr->channel_spec.channel_mode_1 = channel_mode_1;

            handover_details_ptr->new_channel_mode = channel_mode_1;

          }
        }
        break;

      case SECOND_CHAN_DESC_AFTER_TIME_IEI:
        /**********************************************/
        /* Decode the channel description information */
        /* for the second channel after time          */
        /**********************************************/
        if (current_IE->start_IE != NULL)
        {
          channel_information_T channel_info_2_after;
          memset((void *) &channel_info_2_after,0,sizeof(channel_info_2_after));

          rr_decode_channel_description(
            current_IE->start_IE,
            &channel_info_2_after,
            &decoding_ok,
            gas_id
          );

          if (decoding_ok == FALSE)
          {
            /********************************************************/
            /* As the information Element is OPTIONAL, we ignore it */
            /********************************************************/
            decoding_ok = TRUE;
          }
          else
          {
            /* since the channel info 2 IE was present, it indicates 2 dedicated channels */
            handover_details_ptr->channel_spec.number_of_ded_channels = 2;
            handover_details_ptr->channel_spec.channel_info_2_after = channel_info_2_after;
          }
        }
        break;

      case MODE_OF_SECOND_CHANNEL_IEI:
        /***************************************/
        /* Decode the Channel Mode Information */
        /* for the second channel              */
        /***************************************/

        if (current_IE->start_IE != NULL)
        {
          rr_decode_channel_mode(
            current_IE->start_IE,
            &channel_mode_2,
            &decoding_ok
          );

          if (decoding_ok == FALSE)
          {
            /********************************************************/
            /* As the information element is OPTIONAL, we ignore it */
            /********************************************************/
            decoding_ok = TRUE;
          }
          else
          {
            handover_details_ptr->channel_spec.channel_mode_2 = channel_mode_2;
          }
        }
        break;

      case FREQ_CHAN_SEQUENCE_AFTER_TIME_IEI:
        /*****************************************************/
        /* Decode the Frequency Channel Sequence Information */
        /* after time                                        */
        /*****************************************************/

        if (current_IE->start_IE != NULL)
        {
          rr_decode_frequency_channel_sequence(
            current_IE->start_IE,
            &frequency_list_after,
            &decoding_ok,
            gas_id
          );

          freq_chan_seq_after_time = TRUE;
        }
        break;

      case MOBILE_ALLOC_AFTER_TIME_IEI:
        /***************************************************/
        /* Decode the Mobile Allocation (MA) after time    */
        /***************************************************/

        if (current_IE->start_IE != NULL)
        {
          rr_decode_mobile_allocation(
            current_IE->start_IE,
            &ma_after
          );

          ma_after_time = TRUE;
        }
        break;

      case STARTING_TIME_IEI:
        /****************************************/
        /* Decode the Starting Time Information */
        /****************************************/
        if (current_IE->start_IE != NULL)
        {
          rr_decode_starting_time(
            current_IE->start_IE,
            &starting_time,
            &decoding_ok
          );

          handover_details_ptr->channel_spec.starting_time_valid = TRUE;
          handover_details_ptr->channel_spec.starting_time[0] = starting_time >> 8;
          handover_details_ptr->channel_spec.starting_time[1] = starting_time & 255;
        }
        break;

      case REAL_TIME_DIFFERENCE_IEI:
        /***********************************/
        /* Decode the real time difference */
        /***********************************/
        if (current_IE->start_IE != NULL)
        {
          real_time_difference = *(current_IE->start_IE + 1);
          handover_details_ptr->real_time_difference_valid = TRUE;
          handover_details_ptr->real_time_difference = real_time_difference;
        }
        break;

      case TIMING_ADVANCE_IEI:
        /*****************************/
        /* Decode the timing advance */
        /*****************************/
        if (current_IE->start_IE != NULL)
        {
          timing_advance = *(current_IE->start_IE);
          handover_details_ptr->timing_advance_valid = TRUE;
          handover_details_ptr->timing_advance = timing_advance;
        }
        break;

      case FREQ_SHORT_LIST_BEFORE_TIME_IEI:
        /***********************************************/
        /* Decode the frequency short list before time */
        /***********************************************/
        if ((current_IE->start_IE != NULL) &&
            (handover_details_ptr->channel_spec.starting_time_valid == TRUE))
        {
          rr_decode_short_freq_list(
            current_IE->start_IE,
            &frequency_list_before,
            &decoding_ok,
            gas_id
          );


          handover_failure = rr_check_frequency_list(
            &frequency_list_before,
            gas_id
          );

          if (handover_failure)
          {
            /*
             * if channels not all in one band
             */
            RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
            ERR("HO:Mixed bands in FreqList",0,0,0);
          }
          else
          {
            frequency_list_before_time = TRUE;
            handover_details_ptr->channel_spec.before_parameters_valid = TRUE;
          }
        }
        break;

      case FREQUENCY_LIST_BEFORE_TIME_IEI:
        /*****************************************/
        /* Decode the frequency list before time */
        /*****************************************/
        if ((current_IE->start_IE != NULL) &&
            (handover_details_ptr->channel_spec.starting_time_valid == TRUE))
        {
          rr_decode_frequency_list(
            current_IE->start_IE,
            scell_info_ptr->gsm.BCCH_ARFCN,
            &frequency_list_before,
            &decoding_ok,
            gas_id
          );

          handover_failure = rr_check_frequency_list(
            &frequency_list_before,
            gas_id
          );

          if (handover_failure)
          {
            /*
             * if channels not all in one band
             */

            RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
            ERR("HO:Mixed bands in FreqList",0,0,0);
          }
          else
          {
            frequency_list_before_time = TRUE;
            handover_details_ptr->channel_spec.before_parameters_valid = TRUE;
          }
        }
        break;

      case FIRST_CHAN_DESC_BEFORE_TIME_IEI:
        /***********************************************************/
        /* Decode the description of the first channel before time */
        /***********************************************************/
        if ((current_IE->start_IE != NULL) &&
            (handover_details_ptr->channel_spec.starting_time_valid == TRUE))
        {
          channel_information_T channel_info_1_before;
          memset((void *) &channel_info_1_before,0,sizeof(channel_info_1_before));

          rr_decode_channel_description_2(
            current_IE->start_IE,
            &channel_info_1_before,
            &decoding_ok,
            gas_id
          );

          if (decoding_ok == FALSE)
          {
            /********************************************************/
            /* As the information element is OPTIONAL, we ignore it */
            /********************************************************/
            decoding_ok = TRUE;
          }
          else
          {
            handover_details_ptr->channel_spec.channel_info_1_before = channel_info_1_before;
            handover_details_ptr->channel_spec.before_parameters_valid = TRUE;
            first_chan_desc_before_received = TRUE;
          }
        }
        break;

      case SECOND_CHAN_DESC_BEFORE_TIME_IEI:
        /*************************************************/
        /* Description of the second channel before time */
        /*************************************************/
        if ((current_IE->start_IE != NULL) &&
            (handover_details_ptr->channel_spec.starting_time_valid == TRUE))
        {
          channel_information_T channel_info_2_before;
          memset((void *) &channel_info_2_before, 0, sizeof(channel_info_2_before));

          rr_decode_channel_description(
            current_IE->start_IE,
            &channel_info_2_before,
            &decoding_ok,
            gas_id
          );

          if (decoding_ok == FALSE)
          {
            /********************************************************/
            /* As the information element is OPTIONAL, we ignore it */
            /********************************************************/
            decoding_ok = TRUE;
          }
          else
          {
            handover_details_ptr->channel_spec.channel_info_2_before = channel_info_2_before;
            handover_details_ptr->channel_spec.before_parameters_valid = TRUE;
            second_chan_desc_before_received = TRUE;
          }
        }
        break;

      case FREQ_CHAN_SEQUENCE_BEFORE_TIME_IEI:
        /*****************************************************/
        /* Decode the frequency channel sequence before time */
        /*****************************************************/
        if ((current_IE->start_IE != NULL) &&
            (handover_details_ptr->channel_spec.starting_time_valid == TRUE))
        {
          rr_decode_frequency_channel_sequence(
            current_IE->start_IE,
            &frequency_list_before,
            &decoding_ok,
            gas_id
          );

          handover_details_ptr->channel_spec.before_parameters_valid = TRUE;
          freq_chan_seq_before_time = TRUE;
        }
        break;

      case MOBILE_ALLOC_BEFORE_TIME_IEI:
        /********************************************/
        /* Decode the mobile allocation before time */
        /********************************************/

        if ((current_IE->start_IE != NULL) &&
            (handover_details_ptr->channel_spec.starting_time_valid == TRUE))
        {
          rr_decode_mobile_allocation(current_IE->start_IE,&ma_before);

          ma_before_time = TRUE;
          handover_details_ptr->channel_spec.before_parameters_valid = TRUE;
        }
        break;

      case CIPHER_MODE_SETTING_IEI:
        if (current_IE->start_IE != NULL)
        {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          cipher_mode_setting_present = TRUE;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

          start_ciphering = *(current_IE->start_IE) & 0x01;

          if (start_ciphering == TRUE)
          {
            /*************************************************/
            /* The network wants the MS to set the ciphering */
            /*************************************************/
            cipher_algorithm = (cipher_algorithm_T) (*(current_IE->start_IE) & 0x0E) >> 1;

            handover_details_ptr->channel_spec.cipher_flag = rr_cipher_mode_supported(
              cipher_algorithm,
              gas_id
            );

            if (handover_details_ptr->channel_spec.cipher_flag == TRUE)
            {
              handover_details_ptr->channel_spec.cipher_algorithm = cipher_algorithm;
            }
            else
            {
              MSG_GERAN_ERROR_1_G("HO failure, ciphering algorithm %d not supported", cipher_algorithm);

              /* the UE does not support the indicated ciphering algorithm */
              handover_failure = TRUE;
              decoding_ok      = FALSE;
              RR_cause         = RR_CHANNEL_MODE_UNACCEPTABLE;
            }
          }
          else
          {
            /*****************************************************/
            /* The network wants the MS not to set the ciphering */
            /*****************************************************/
            handover_details_ptr->channel_spec.cipher_flag = FALSE;
          }
        }
        break;

      case HO_MULTIRATE_CONFIG_IEI:
        /********************************************/
        /* Decode the multirate configuration       */
        /********************************************/
        if (current_IE->start_IE != NULL)
        {
          multirate_ie_present = TRUE;

          rr_decode_multirate_config_ie(
            current_IE->start_IE,
            &(dedic_ptr->next_amr_config),
            &decoding_ok,
            channel_mode_1,
            gas_id
          );

          if (!decoding_ok)
          {
            MSG_GERAN_ERROR_0_G("HO failure while decoding MULTIRATE CONFIGRATION IE");
            handover_failure = TRUE;
            RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          }
        }
        break;

      default :
        MSG_GERAN_MED_1_G("Unhandled IEI%d",current_IE->IEI);
        break;

    } /* Switch */
  }


  /* At this point, RR should have enough information to send the handover start event */
  if (handover_details_ptr->handover_type == RR_L1_GSM_GSM)
  {
    rr_log_handover_start(
      RR_LOG_HO_TYPE_GSM_TO_GSM,
      handover_details_ptr->BCCH_ARFCN,
      handover_details_ptr->BSIC,
      handover_details_ptr->handover_reference,
      gas_id
    );
  }

#ifdef FEATURE_WCDMA
  else if (handover_details_ptr->handover_type == RR_L1_WCDMA_GSM)
  {
    rr_log_handover_start(
      RR_LOG_HO_TYPE_WCDMA_TO_GSM,
      handover_details_ptr->BCCH_ARFCN,
      handover_details_ptr->BSIC,
      handover_details_ptr->handover_reference,
      gas_id
    );
  }
#endif /* #ifdef FEATURE_WCDMA */
#ifdef FEATURE_LTE
  else if (handover_details_ptr->handover_type == RR_L1_LTE_GSM)
  {
    rr_log_handover_start(RR_LOG_HO_TYPE_LTE_TO_GSM,
                          handover_details_ptr->BCCH_ARFCN,
                          handover_details_ptr->BSIC,
                          handover_details_ptr->handover_reference,
                          gas_id);
  }
#endif /* FEATURE_LTE */

  if ((decoding_ok == TRUE) && (handover_failure == FALSE))
  {
    /******************************************************************/
    /* Take care of the channel mode 1 and 2 information              */
    /******************************************************************/
    if (rr_channel_mode_supported(handover_details_ptr->channel_spec.channel_mode_1, gas_id) == FALSE)
    {
      MSG_GERAN_ERROR_1_G("HO failure, channel mode 1 (%d) is unnacceptable", handover_details_ptr->channel_spec.channel_mode_1);
      handover_failure = TRUE;
      RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
    }
    /* channel_mode_1 is supported, if there are 2 dedicated channels specified */
    else if (handover_details_ptr->channel_spec.number_of_ded_channels == 2)
    {
      /* need to check that the second channel mode is also supported */
      if (rr_channel_mode_supported(handover_details_ptr->channel_spec.channel_mode_2, gas_id) == FALSE)
      {
        MSG_GERAN_ERROR_1_G("HO failure, channel mode 2 (%d) is unnacceptable", handover_details_ptr->channel_spec.channel_mode_2);
        handover_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
      }
    }

    /******************************************************************/
    /* Check if multirate config valid for channel mode 1             */
    /******************************************************************/
    if ((IS_AMR_CHANNEL_MODE(handover_details_ptr->channel_spec.channel_mode_1)) &&
        (dedic_ptr->next_amr_config.valid == FALSE))
    {
      if ((IS_AMR_CHANNEL_MODE(dedic_ptr->curr_channel_spec.channel_mode_1)) &&
          (dedic_ptr->curr_amr_config.valid == TRUE))
      {

#ifdef FEATURE_VAMOS_II
        if(rr_vamos2_rcvd_chan_mode_matches_curr_chan_mode(handover_details_ptr->channel_spec.channel_mode_1,
                                                           dedic_ptr->curr_channel_spec.channel_mode_1) == TRUE)
#else
        if (handover_details_ptr->channel_spec.channel_mode_1 == dedic_ptr->curr_channel_spec.channel_mode_1)
#endif /*FEATURE_VAMOS_II*/
        {
          /* multirate config can be inherited from the dedicated data */
          rr_use_existing_multirate_config(dedic_ptr);
        }
        else
        {
          /* AMR channel mode has changed but no multirate config was provided */
          MSG_GERAN_ERROR_0_G("HO failure, No multirate config in HANDOVER COMMAND");
          handover_failure = TRUE;
          RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          MSG_GERAN_HIGH_2_G("current AMR type = %d New AMR type = %d",(int) dedic_ptr->curr_channel_spec.channel_mode_1,
            (int)handover_details_ptr->channel_spec.channel_mode_1);
        }
      }
      else
      {
        /* existing channel config has no valid multirate config */
        MSG_GERAN_ERROR_0_G("HO failure, multirate config not available from previous assignment");
        handover_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
      }
    }

#ifdef FEATURE_VAMOS_II
    if (handover_details_ptr->channel_spec.channel_mode_1 == SPEECH_V3 ||
        handover_details_ptr->channel_spec.channel_mode_1 == SPEECH_V3_VAMOS2)
#else
    if (handover_details_ptr->channel_spec.channel_mode_1 == SPEECH_V3)
#endif /*FEATURE_VAMOS_II*/
    {
      /* For half rate the last 2 bits in the codec modes of the Active Codec Set that correspond
      to the rates of 12.2 and 10.2 should not be set  */
      if ((dedic_ptr->next_amr_config.valid == TRUE) &&
          (handover_details_ptr->channel_spec.channel_info_1_after.channel_type == HALF_RATE_TRAFFIC))
      {
        if (dedic_ptr->next_amr_config.codec_modes & 0xC0)
        {
          handover_failure = TRUE;
          RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          MSG_GERAN_ERROR_0_G("HO failure, unsupported Codec modes for Half-rate");
        }
      }
    }

#ifdef FEATURE_GSM_AMR_WB

#ifdef FEATURE_VAMOS_II
    if (handover_details_ptr->channel_spec.channel_mode_1 == SPEECH_V5 ||
        handover_details_ptr->channel_spec.channel_mode_1 == SPEECH_V5_VAMOS2)
#else
    if (handover_details_ptr->channel_spec.channel_mode_1 == SPEECH_V5)
#endif /*FEATURE_VAMOS_II*/

    {
      /* For SPEECH_V5 channel mode, channel type should be full rate*/
      if (handover_details_ptr->channel_spec.channel_info_1_after.channel_type == HALF_RATE_TRAFFIC)
      {
        handover_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
        MSG_GERAN_ERROR_0_G("HO failure, unsupported Channel type for AMR WB");
      }
    }
#endif /*FEATURE_GSM_AMR_WB*/

    /* The MultiRate Configuration IE shall be included in the case of full rate
    channel to half rate channel handover. If not included in this case, the
    mobile station shall behave as if the MultiRate Configuration IE was
    inconsistent.*/

#ifdef FEATURE_VAMOS_II
    if ( ((dedic_ptr->curr_channel_spec.channel_mode_1 == SPEECH_V3) &&
         (handover_details_ptr->channel_spec.channel_mode_1 == SPEECH_V3))
         ||
         ((dedic_ptr->curr_channel_spec.channel_mode_1 == SPEECH_V3_VAMOS2) &&
         (handover_details_ptr->channel_spec.channel_mode_1 == SPEECH_V3_VAMOS2))
       )
#else
    if ((dedic_ptr->curr_channel_spec.channel_mode_1 == SPEECH_V3) &&
        (handover_details_ptr->channel_spec.channel_mode_1      == SPEECH_V3))
#endif /*FEATURE_VAMOS_II*/
    {
      if (
          (dedic_ptr->curr_channel_spec.channel_info_1_after.channel_type == FULL_RATE_TRAFFIC) &&
          (handover_details_ptr->channel_spec.channel_info_1_after.channel_type == HALF_RATE_TRAFFIC)
         )
      {
        if (multirate_ie_present == FALSE)
        {
          handover_failure = TRUE;
          RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
          MSG_GERAN_ERROR_0_G("HO failure, multi-rate IE should be present");
        }
      }
    }

    /******************************************************************/
    /* Take care of the channel information 1 after time              */
    /******************************************************************/
    /* verify that the channel type is supported */
    if (rr_channel_type_supported(
          handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
          gas_id
        ) == FALSE)
    {
      handover_failure = TRUE;
      RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
    }
    /* channel type is acceptable, check the hopping requirements */
    else if (handover_details_ptr->channel_spec.channel_info_1_after.hopping_flag == TRUE)
    {
      if (ma_after_time == TRUE)
      {
        /* the mobile allocation after time was received */
        if (handover_details_ptr->valid_CA_received == TRUE)
        {
          boolean result;
          /******************************************************/
          /* set the frequency list from the mobile allocation  */
          /* using the cell channel description received in the */
          /* HANDOVER COMMAND message itself                    */
          /******************************************************/
          result = rr_set_channel_frequency_list_from_mobile_allocation(
            &(handover_details_ptr->channel_spec.channel_info_1_after),
            &(ma_after),
            &(handover_details_ptr->cell_channel_description),
            gas_id
          );

          /* if there was a problem setting the frequency list */
          if (result == FALSE)
          {
            handover_failure = TRUE;
            RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
            MSG_GERAN_ERROR_0_G("HO failure, After MA freq not in same band");
          }
        }
        else /* valid CA not received in HANDOVER command */
        {
          MSG_GERAN_ERROR_0_G("HO failure, No CA in HANDOVER Command");
          /**********************************************/
          /* No CA is available. We can't decode the MA */
          /**********************************************/
          handover_failure = TRUE;
          RR_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
        }
      }
      else if (frequency_list_after_time == TRUE)
      {
        handover_details_ptr->channel_spec.channel_info_1_after.frequency_list = frequency_list_after;
      }
      else if (freq_chan_seq_after_time == TRUE)
      {
        handover_details_ptr->channel_spec.channel_info_1_after.frequency_list = frequency_list_after;
      }
      else /* missing conditional IE */
      {
        MSG_GERAN_HIGH_0_G("CondIE: FreqInfo not received for hopping channel");
        decoding_ok = FALSE;
        rr_status = RR_CONDITIONAL_IE_ERROR;
      }
    }

    /******************************************************************/
    /* Take care of the channel information 2 after time              */
    /******************************************************************/
    if (handover_details_ptr->channel_spec.number_of_ded_channels == 2)
    {
      /* verify that the channel type is supported */
      if (rr_channel_type_supported(
            handover_details_ptr->channel_spec.channel_info_2_after.channel_type,
            gas_id
          ) == FALSE)
      {
        handover_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
      }
      else if (handover_details_ptr->channel_spec.channel_info_2_after.hopping_flag == TRUE)
      {
        if (ma_after_time == TRUE)
        {
          if (handover_details_ptr->valid_CA_received == TRUE)
          {
            boolean result;
            /******************************************************/
            /* set the frequency list from the mobile allocation  */
            /* using the cell channel description received in the */
            /* HANDOVER COMMAND message itself                    */
            /******************************************************/
            result = rr_set_channel_frequency_list_from_mobile_allocation(
              &(handover_details_ptr->channel_spec.channel_info_2_after),
              &(ma_after),
              &(handover_details_ptr->cell_channel_description),
              gas_id
            );

            /* if there was a problem setting the frequency list */
            if (result == FALSE)
            {
              handover_failure = TRUE;
              RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
              MSG_GERAN_ERROR_0_G("HO failure, After MA freq not in same band");
            }
          }
          else
          {
            MSG_GERAN_ERROR_0_G("HO failure, No CA in HANDOVER Command");
           /**********************************************/
            /* No CA is available. We can't decode the MA */
            /**********************************************/
            handover_failure = TRUE;
            RR_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
          }
        }
        else if (frequency_list_after_time == TRUE)
        {
          handover_details_ptr->channel_spec.channel_info_2_after.frequency_list = frequency_list_after;
        }
        else if (freq_chan_seq_after_time == TRUE)
        {
          handover_details_ptr->channel_spec.channel_info_2_after.frequency_list = frequency_list_after;
        }
      }
      else /* missing conditional IE */
      {
        MSG_GERAN_HIGH_0_G("CondIE: FreqInfo not received for hopping channel");
        decoding_ok = FALSE;
        rr_status = RR_CONDITIONAL_IE_ERROR;
      }
    }  /* handover_details_ptr->channel_spec.number_of_ded_channels == 2 */

    /* if the starting time was present, there may be before channel information available */
    if (!handover_failure && decoding_ok && handover_details_ptr->channel_spec.starting_time_valid == TRUE)
    {
      if ((first_chan_desc_before_received == FALSE) &&
          (handover_details_ptr->channel_spec.before_parameters_valid == TRUE))
      {
        /****************************************************************/
        /* Copy the channel description of the first channel after time */
        /* in the handover details for the first channel before time    */
        /****************************************************************/
        handover_details_ptr->channel_spec.channel_info_1_before = handover_details_ptr->channel_spec.channel_info_1_after;
      }
      /* otherwise, use the received channel description before */

      if ((second_chan_desc_before_received == FALSE) &&
          (handover_details_ptr->channel_spec.before_parameters_valid == TRUE) &&
          (handover_details_ptr->channel_spec.number_of_ded_channels == 2))
      {
        /*****************************************************************/
        /* Copy the channel description of the second channel after time */
        /* in the handover_details_ptr->channel_information_2_before_time     */
        /*****************************************************************/
        handover_details_ptr->channel_spec.channel_info_2_before = handover_details_ptr->channel_spec.channel_info_2_after;
      }
      /* otherwise, use the received channel description before */

      /******************************************************************/
      /* Take care of the channel information 1 before time             */
      /******************************************************************/
      if (rr_channel_type_supported(
            handover_details_ptr->channel_spec.channel_info_1_before.channel_type,
            gas_id
          ) == FALSE)
      {
        handover_failure = TRUE;
        RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
      }
      /* if the channel type is supported and it is a hopping channel */
      else if (handover_details_ptr->channel_spec.channel_info_1_before.hopping_flag == TRUE)
      {
        if (ma_before_time == TRUE)
        {
          if (handover_details_ptr->valid_CA_received == TRUE)
          {
            /*************************************/
            /* Translate the mobile_allocation   */
            /* into a list of frequencies        */
            /*************************************/
            boolean result;
            /******************************************************/
            /* set the frequency list from the mobile allocation  */
            /* using the cell channel description received in the */
            /* HANDOVER COMMAND message itself                    */
            /******************************************************/
            result = rr_set_channel_frequency_list_from_mobile_allocation(
              &(handover_details_ptr->channel_spec.channel_info_1_before),
              &(ma_before),
              &(handover_details_ptr->cell_channel_description),
              gas_id
            );

            /* if there was a problem setting the frequency list */
            if (result == FALSE)
            {
              handover_failure = TRUE;
              RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
              MSG_GERAN_ERROR_0_G("HO failure, Before MA freq not in same band");
            }
          }
          else /* valid CA not received */
          {
            MSG_GERAN_ERROR_0_G("HO failure, No CA in HANDOVER Command");
            /**********************************************/
            /* No CA is available. We can't decode the MA */
            /**********************************************/
            handover_failure = TRUE;
            RR_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
          }
        }
        else if (freq_chan_seq_before_time == TRUE)
        {
          handover_details_ptr->channel_spec.channel_info_1_before.frequency_list = frequency_list_before;
        }
        else if (frequency_list_before_time == TRUE)
        {
          handover_details_ptr->channel_spec.channel_info_1_before.frequency_list = frequency_list_before;
        }
        else /* no frequency parameters before */
        {
          /***********************************************************/
          /* No frequency parameters before time were present in the */
          /* message                                                 */
          /* Pick up the after parameters                            */
          /***********************************************************/

          no_of_entries = handover_details_ptr->channel_spec.channel_info_1_after.frequency_list.no_of_items;

          for (index = 0; index < no_of_entries; index++)
          {
            handover_details_ptr->channel_spec.channel_info_1_before.frequency_list.channel[index] =
             handover_details_ptr->channel_spec.channel_info_1_after.frequency_list.channel[index];
          }
          handover_details_ptr->channel_spec.channel_info_1_before.frequency_list.no_of_items = no_of_entries;
        } /* no frequency parameters before */
      } /* channel info1 before is hopping */

      /******************************************************************/
      /* Take care of the channel information 2 before time             */
      /******************************************************************/
      if (handover_details_ptr->channel_spec.number_of_ded_channels == 2)
      {
        if (rr_channel_type_supported(
              handover_details_ptr->channel_spec.channel_info_2_before.channel_type,
              gas_id
            ) == FALSE)
        {
          MSG_GERAN_ERROR_1_G("HO failure, channel_info_2_before.channel_type %d not supported",
                              handover_details_ptr->channel_spec.channel_info_2_before.channel_type);

          handover_failure = TRUE;
          RR_cause = RR_CHANNEL_MODE_UNACCEPTABLE;
        }
        else if (handover_details_ptr->channel_spec.channel_info_2_before.hopping_flag == TRUE)
        {
          if (ma_before_time == TRUE)
          {
            if (handover_details_ptr->valid_CA_received == TRUE)
            {
              /*************************************/
              /* Translate the mobile_allocation   */
              /* into a list of frequencies        */
              /*************************************/
              boolean result;
              /******************************************************/
              /* set the frequency list from the mobile allocation  */
              /* using the cell channel description received in the */
              /* HANDOVER COMMAND message itself                    */
              /******************************************************/
              result = rr_set_channel_frequency_list_from_mobile_allocation(
                &(handover_details_ptr->channel_spec.channel_info_2_before),
                &(ma_before),
                &(handover_details_ptr->cell_channel_description),
                gas_id
              );

              /* if there was a problem setting the frequency list */
              if (result == FALSE)
              {
                handover_failure = TRUE;
                RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
                MSG_GERAN_ERROR_0_G("HO failure, Before MA freq not in same band");
              }
            }
            else
            {
              MSG_GERAN_ERROR_0_G("HO failure, No CA in HANDOVER Command");
              /**********************************************/
              /* No CA is available. We can't decode the MA */
              /**********************************************/
              handover_failure = TRUE;
              RR_cause = RR_NO_CELL_ALLOCATION_AVAILABLE;
            }
          }
          else if (freq_chan_seq_before_time == TRUE)
          {
            handover_details_ptr->channel_spec.channel_info_2_before.frequency_list = frequency_list_before;
          }
          else if (frequency_list_before_time == TRUE)
          {
            handover_details_ptr->channel_spec.channel_info_2_before.frequency_list = frequency_list_before;
          }
          else
          {
            /***********************************************************/
            /* No frequency parameters before time were present in the */
            /* message                                                 */
            /* Pick up the after parameters                            */
            /***********************************************************/

            no_of_entries = handover_details_ptr->channel_spec.channel_info_2_before.frequency_list.no_of_items;

            for (index = 0; index < no_of_entries; index++)
            {
              handover_details_ptr->channel_spec.channel_info_2_before.frequency_list.channel[index] =
               handover_details_ptr->channel_spec.channel_info_2_after.frequency_list.channel[index];
            }
            handover_details_ptr->channel_spec.channel_info_2_before.frequency_list.no_of_items = no_of_entries;
          }
        } /* channel 2 is a hopping channel */
      } /* number of dedicated channels is 2 */
    } /* if starting time is valid */

    /*************************************************************/
    /* We have now to check the information embedded in the      */
    /* HANDOVER_COMMAND message to perform the according actions */
    /*************************************************************/
    /* Before starting ciphering, check whether we need to activate a pending
    ciphering key.  This will be the case if this is a UTRAN to GSM handover
    and ciphering was not enabled on UTRAN, but is to be enabled on GSM. */
    if (start_ciphering && activate_new_cipher_key_if_ciphering_enabled)
    {
      MSG_GERAN_HIGH_0_G("Activating pending ciphering key for UTRAN to GSM handover");
      rr_siminfo_activate_new_cipher_keys(gas_id);
    }

    cipher_key_present = rr_siminfo_get_cipher_key(handover_details_ptr->cipher_key,sizeof(handover_details_ptr->cipher_key),
                                                   handover_details_ptr->cipher_key_128,
                                                   sizeof(handover_details_ptr->cipher_key_128),
                                                   gas_id
                                                   );

    switch (handover_details_ptr->handover_type)
    {
      case RR_L1_GSM_GSM:
      {
        /* HANDOVER COMMAND message shall not contain a cipher mode setting IE
         * that indicates "start ciphering" unless a CIPHERING MODE COMMAND
         * message has been transmitted previously in this instance of the
         * dedicated mode or ciphering has been started earlier in UTRAN
         * HANDOVER FAILURE  message with cause "Protocol error unspecified" shall be returned immediately,
         */
         
        if (start_ciphering == TRUE)
        {
          if (rr_get_ciphering_mode_command_received(gas_id))
          {
            rr_ciphering_has_started(TRUE, gas_id);
          }
          else
          {
            MSG_GERAN_ERROR_0_G("HO failure, ciphering mode command not received");
            handover_failure = TRUE;
            RR_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;
          }
        }
        break;
      }

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      case RR_L1_WCDMA_GSM:
      case RR_L1_TDS_GSM:
      case RR_L1_LTE_GSM:
      {
        /* WCDMA Security Mode Control procedure must have completed in order to get this far */
        rr_set_ciphering_mode_command_received(TRUE, gas_id);

        if (cipher_mode_setting_present == FALSE)
        {
#ifdef PERLUTF
          #error code not present
#else
          MSG_GERAN_ERROR_0_G("Missing cipher mode setting IE for W2G Handover");
#endif /* PERLUTF */
        }

        /* Fail the InterRAT HO if cipher key from MM is not available, or */
        /* if the configuration is inconsistent with that indicated by RRC */
        if (start_ciphering == TRUE)
        {
          if (cipher_key_present)
          {
            rr_ciphering_has_started(TRUE, gas_id);
          }
          else
          {
            /* ciphering was requested, but RR didn't have the Cipher Key from MM */
            handover_failure = TRUE;
            MSG_GERAN_ERROR_0_G("Cipher Key is not available from MM, but Network wants to enable Ciphering");
            /* rr_x2g_ho_failure_reason will be set to RR_X2G_HO_FAILURE_INVALID_CONFIGURATION */
          }
        }
        break;
      }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
      default:
        MSG_GERAN_ERROR_1_G("RR unknown handover type: %d",handover_details_ptr->handover_type);
      break;
    }

    if (decoding_ok &&
        (rr_check_frequency_lists_supported(&(handover_details_ptr->channel_spec), gas_id) == FALSE)
       )
    {
      MSG_GERAN_ERROR_0_G("HO failure, Frequency not implemented in Handover");
      handover_failure = TRUE;
      RR_cause = RR_FREQUENCY_NOT_IMPLEMENTED;
    }

    if (handover_failure || !decoding_ok)
    {
      /* the kind of failure event to generate depends on whether the decoding was done or not */
      if (decoding_ok)
      {
        /* must be a configuration problem */
        rr_log_handover_failure(RR_LOG_HO_FAILURE_CONFIGURATION_NOT_SUPPORTED, gas_id);
      }
      else /* decoding was not OK */
      {
        /* must be a protocol error */
        rr_log_handover_failure(RR_LOG_HO_FAILURE_PROTOCOL_ERROR, gas_id);
      }
      /****************************************************/
      /* Generate a HANDOVER_FAILURE message with cause   */
      /* messsage type not compatible with protocol state */
      /****************************************************/

      switch (handover_details_ptr->handover_type)
      {
        case RR_L1_GSM_GSM:
        {
          if (handover_failure)
          {
            /* No need to wait for SI5, SI5bis & SI6 before sending measurement reports */
            rr_ded_set_measurement_reports_invalid(FALSE, gas_id);

            rr_perform_handover_failure(
              RR_cause,   // RR_cause
              FALSE,      // reconnect
              gas_id      // gas_id
            );
          }
          else /* decoding not OK */
          {
            /************************************************************/
            /* The MS ignores the message, and, as an RR connection     */
            /* exists, an RR_STATUS message is sent to the network      */
            /************************************************************/
            rr_send_status(
              rr_status,    // RR_cause
              gas_id        // gas_id
            );

            MSG_GERAN_HIGH_1_G("OMsg(NW,STATUS(%d)",(int) rr_status);
          }
          break;
        }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        case RR_L1_WCDMA_GSM:
        case RR_L1_TDS_GSM:
        case RR_L1_LTE_GSM:
        {
          rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_INVALID_CONFIGURATION;
          new_event = RR_EV_W2G_HO_PROTOCOL_ERROR;
          break;
        }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
        default:
        {
          MSG_GERAN_ERROR_1_G("Invalid handover type=%d",(int) handover_details_ptr->handover_type);
          break;
        }
      }
    }
    else
    {
      if (handover_details_ptr->handover_type == RR_L1_GSM_GSM)
      {
        /* SAPI3 link may be pending or established, but any action */
        /* required must be decided by the separate RR SAPI3 module */
        rr_release_sapi3(gas_id);

        /* suspend SAPI0 for this inter-cell handover */
        rr_send_dl_suspend_req(RR_L1_GSM_GSM, gas_id);
      }

      /* Set current_dedicated_channel_band. Channel info for first channel
         after start time is always present. After time and before time band
         for first and second channels are considered as the same whether it
         is hopping or not. Therefore, the band of first element in frequency
         list for first channel after time is taken as the current dedicated
         band */
      rr_set_current_dedicated_band(
        (rr_internal_band_T) chan_spec_ptr->channel_info_1_after.frequency_list.channel[0].band,
        gas_id
      );

      /* Debug messages */
      if (chan_spec_ptr->before_parameters_valid &&
          (chan_spec_ptr->channel_info_1_before.frequency_list.channel[0].band
           != chan_spec_ptr->channel_info_1_after.frequency_list.channel[0].band)
         )
      {
        MSG_GERAN_HIGH_3_G("chan 1: before band %d, after band %d",
        chan_spec_ptr->channel_info_1_before.frequency_list.channel[0].band,
        chan_spec_ptr->channel_info_1_after.frequency_list.channel[0].band,
        0);
      }

      if(chan_spec_ptr->before_parameters_valid &&
        (chan_spec_ptr->channel_info_2_before.frequency_list.channel[0].band
        !=chan_spec_ptr->channel_info_2_after.frequency_list.channel[0].band))
      {
        MSG_GERAN_HIGH_3_G("chan 2: before band %d, after band %d",
        chan_spec_ptr->channel_info_2_before.frequency_list.channel[0].band,
        chan_spec_ptr->channel_info_2_after.frequency_list.channel[0].band,
        0);
      }
      new_event = EV_HANDOVER_COMMAND_CHECKED;
    }
  }
  else /* decoding not ok, but syntactically errors found */
  {
    if (handover_failure == FALSE)
    {
      /* must be a protocol error */
      rr_log_handover_failure(RR_LOG_HO_FAILURE_PROTOCOL_ERROR, gas_id);

      /*****************************************************************/
      /* one of the Information Element has is syntactically incorrect */
      /* The action taken depends on the Presence parameter of the IE  */
      /*****************************************************************/

      // Default action - send a RR Status message with cause #111 "protocol error unspecified"
      RR_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;

      if (current_IE != NULL)
      {
        if (MANDATORY_IE == current_IE->presence)
        {
          MSG_GERAN_HIGH_0_G("OMsg(NW,STATUS(InvalidMandatory");

          // Send a RR Status message with cause #96 "invalid mandatory information"
          RR_cause = RR_INVALID_MANDATORY_INFORMATION;
        }
        else
        if (CONDITIONAL_IE == current_IE->presence)
        {
          MSG_GERAN_HIGH_0_G("OMsg(NW,STATUS(ConditionalIE");

          // Send a RR Status message with cause #100 "conditional IE error"
          RR_cause = RR_CONDITIONAL_IE_ERROR;
        }
      }

      switch (handover_details_ptr->handover_type)
      {
        case RR_L1_GSM_GSM:
        {
          rr_send_status(RR_cause, gas_id);
          break;
        }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          case RR_L1_WCDMA_GSM:
          case RR_L1_TDS_GSM:
          case RR_L1_LTE_GSM:
          {
            rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PROTOCOL_ERROR;
            new_event = RR_EV_W2G_HO_PROTOCOL_ERROR;
             break;
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
        default:
        {
          MSG_GERAN_ERROR_1_G("Invalid handover type=%d",(int) handover_details_ptr->handover_type);
          break;
        }
      }
    }
    else
    {
      /* must be a configuration problem */
      rr_log_handover_failure(RR_LOG_HO_FAILURE_CONFIGURATION_NOT_SUPPORTED, gas_id);

      /******************************************************/
      /* handover_failure is TRUE, and RR_cause contains    */
      /* the reason, so....                                 */
      /* Generate a HANDOVER_FAILURE message with cause     */
      /* identified above, using DL_DATA_REQ                */
      /******************************************************/

      switch (handover_details_ptr->handover_type)
      {
        case RR_L1_GSM_GSM:
        {
          rr_perform_handover_failure(
            RR_cause,   // RR_cause
            FALSE,      // reconnect
            gas_id      // gas_id
          );
          break;
        }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        case RR_L1_WCDMA_GSM:
        case RR_L1_TDS_GSM:
        case RR_L1_LTE_GSM:
        {
          rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_INVALID_CONFIGURATION;
          new_event = RR_EV_W2G_HO_PROTOCOL_ERROR;
          break;
        }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
        default:
        {
          MSG_GERAN_ERROR_1_G("Invalid handover type=%d",(int) handover_details_ptr->handover_type);
          break;
        }
      }
    }
  }

  return new_event;
} /* rr_respond_to_handover_command */

/**
 * rr_set_handover_band_info() : updates the band info in the handover data storage
 *
 * @param handover_details_ptr - pointer top global handover info
 *
 * @param rr_band_ind - RR's representation of the band indicator
 *
 * @param l1_band_ind - L1's representation of the band indicator
 *
 * @return void
 */
static void rr_set_handover_band_info(
  handover_details_T *handover_details_ptr,
  si_psi_band_ind_T rr_band_ind,
  rr_l1_band_indicator_T l1_band_ind,
  const gas_id_t gas_id
)
{
  if (handover_details_ptr != NULL)
  {
    /* quadband support */
    handover_details_ptr->BCCH_ARFCN.band = (sys_band_T) rr_get_band_using_band_indicator(
       handover_details_ptr->BCCH_ARFCN.num,
       rr_band_ind,
       gas_id
     );

    if ((int)rr_band_ind != (int)l1_band_ind)
    {
      MSG_GERAN_ERROR_2_G("Handover band indicator RR=%d L1=%d", (int)rr_band_ind, (int)l1_band_ind);
    }
  }

  return;
}


/* rr_init_handover_details() : initialise the handover_details structure
 *
 * @param handover_details_ptr -  pointer to global handover storae
 *        handover_type - one of the GSM or inter-RAT handover types
 *
 * @return void
 */
static void rr_init_handover_details(
  handover_details_T *handover_details_ptr,
  rr_l1_handover_type_T handover_type,
  const gas_id_t gas_id
)
{
  if (handover_details_ptr != NULL)
  {
    memset((void *)handover_details_ptr, 0, sizeof(handover_details_T));

    handover_details_ptr->handover_type   = handover_type;
    handover_details_ptr->BCCH_ARFCN.num  = INVALID_ARFCN;
    handover_details_ptr->BCCH_ARFCN.band = SYS_BAND_NONE;
  }

  if (handover_type != RR_L1_GSM_GSM)
  {
    rr_ce_init_conn_info_after_x2g_ho(
      gas_id   // gas_id
    );
  }

  return;
}


/**
  @brief Check whether an inter-cell handover is in progress.

  An inter-cell handover is in progress if the current handover_state
  is anything other than HANDOVER_INACTIVE.

  @return TRUE if an inter-cell handover is in progress, FALSE otherwise.
*/
boolean rr_inter_ho_handover_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_inter_ho_data_t *rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);

  if (rr_inter_ho_data_ptr != NULL)
  {
    if (rr_inter_ho_data_ptr->handover_state != HANDOVER_INACTIVE)
    {
      return TRUE;
    }
  }

  return FALSE;
}

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_WCDMA)
/**
  @brief Check whether a W2G inter-RAT handover is starting.

  An inter-RAT W2G handover is starting if the current handover_state
  is RR_W2G_HO_GOING_ACTIVE.

  @return TRUE if an inter-RAT W2G handover is starting, FALSE otherwise.
*/
boolean rr_inter_ho_w2g_starting(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_inter_ho_data_t *rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);

  if (rr_inter_ho_data_ptr != NULL)
  {
    if (rr_inter_ho_data_ptr->handover_state == RR_W2G_HO_GOING_ACTIVE)
    {
      return TRUE;
    }
  }

  return FALSE;
}
#endif /* FEATURE_DUAL_SIM && FEATURE_WCDMA */

/**
 * inter cell handover main state machine
 *
 * @param handover_event event for the state machine
 *
 * @param new_message    pointer to message (only valid if event is EV_INPUT_MESSAGE)
 *
 * @return event for calling state machine
 */
rr_event_T rr_inter_cell_handover_control(
  rr_event_T handover_event,
  rr_cmd_bdy_type *new_message,
  const gas_id_t gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;
  handover_details_T  *handover_details_ptr;
  rr_event_T           new_handover_event  = EV_NO_EVENT;
  rr_event_T           proc_dispatch_event = EV_NO_EVENT;
  byte                 message_set = 0;
  byte                 message_id  = 0;
  gprs_scell_info_t   *scell_info_ptr;
  gprs_scell_info_t   *pcell_info_ptr;
  rr_inter_ho_data_t  *rr_inter_ho_data_ptr;

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  RR_NULL_CHECK_FATAL(scell_info_ptr);

  pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  RR_NULL_CHECK_FATAL(pcell_info_ptr);

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(dedic_ptr);

  handover_details_ptr = &(dedic_ptr->handover_details);

  handover_event = rr_extract_message_header_details(
    handover_event,
    new_message,
    &message_set,
    &message_id
  );

  while (handover_event != EV_NO_EVENT)
  {
    if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
    {
      /* Some messages need to be handled in state independent manner */
      if ((message_set == MS_GHDI_EXPORT) && (message_id == GHDI_CLASSMARK_CHANGED_IND))
      {
        MSG_GERAN_HIGH_0_G("Received CLASSMARK_CHANGED_IND");
      }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
      {
        MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from NAS");
        rr_inter_ho_data_ptr->rr_mm_abort_received = TRUE;
      }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#ifdef FEATURE_WCDMA
      else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
      {
        MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
        rr_inter_ho_data_ptr->rr_interrat_ho_abort_received = TRUE;
      }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
      else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
      {
        MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
        rr_inter_ho_data_ptr->rr_interrat_ho_abort_received = TRUE;
      }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
      else if (message_set == MS_MSGR_RR && new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
      {
        MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from LTE RRC");
        rr_inter_ho_data_ptr->rr_interrat_ho_abort_received = TRUE;
      }
#endif /* FEATURE_LTE */
    } /* EV_INPUT_MESSAGE */

    else if (handover_event == EV_RESET_SOFTWARE)
    {
      MSG_GERAN_HIGH_0_G("Reset Handover state machine ");
      rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
      rr_inter_ho_data_ptr->old_band = BAND_NONE;

      /* Reset the state machine variables */
      rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
    } /* EV_RESET_SOFTWARE */

    switch (rr_inter_ho_data_ptr->handover_state)
    {
      /***************************************************************/
      /* STATE: HANDOVER_INACTIVE                                    */
      /* This is the Initial/default GSM to GSM / WCDMA to GSM       */
      /* handover State Machine, also this is the end state after    */
      /* after handover is successfully completed/terminated         */
      /***************************************************************/
      case HANDOVER_INACTIVE :
        /* default to protocol error, unspecified */
        rr_inter_ho_data_ptr->handover_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if ((message_set == MS_RR_L2) && (message_id == (byte) DL_DATA_IND))
          {
            dl_data_ind_T *dl_data_ind = (dl_data_ind_T *)new_message;

            /******************************************************/
            /* A message has been received from the network in an */
            /* acknowledged mode. It should be a HANDOVER COMMAND */
            /******************************************************/
            if (dl_data_ind->layer3_message[1] == HANDOVER_COMMAND)
            {
              MSG_GERAN_HIGH_0_G("HandoverCommand received");

              /*****************************************************/
              /* Perform the handover details store initialisation */
              /*****************************************************/
              dedic_ptr->handover_cmd_length = dl_data_ind->l3_message_length;

              rr_init_handover_details(
                handover_details_ptr,   // handover_details_ptr
                RR_L1_GSM_GSM,          // handover_type
                gas_id                  // gas_id
              );

              rr_set_handover_band_info(
                handover_details_ptr,       // handover_details_ptr
                SI_PSI_BAND_IND_DCS1800,    // rr_band_ind
                RR_L1_DCS_1800,             // l1_band_ind
                gas_id                      // gas_id
              );

              rr_inter_ho_data_ptr->old_band = rr_get_current_dedicated_band(gas_id);

              new_handover_event = rr_respond_to_handover_command(dedic_ptr, FALSE, gas_id);
            }
          } /* MS_RR_L2 */
#ifdef FEATURE_WCDMA
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_REQ))
          {
            rr_interrat_handover_req_type *rr_interrat_handover_req;
            rr_x2g_ho_gsm_command_t       *rr_x2g_ho_gsm_cmd_ptr;
            int                            i;
            rr_PLMN_request_details_T     *rr_PLMN_request_details_ptr;

            rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

            rr_interrat_handover_req = (&((rr_cmd_bdy_type *) new_message)->rr_interrat_handover_req);

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
            rr_tds_pseudo_fr_reset_dedicated_info(gas_id);
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

            /*****************************************************/
            /* Perform the handover details store initialisation */
            /*****************************************************/
            rr_init_handover_details(
              handover_details_ptr,   // handover_details_ptr
              RR_L1_WCDMA_GSM,        // handover_type
              gas_id                  // gas_id
            );

            rr_PLMN_request_details_ptr->rat_pri_list_info = rr_interrat_handover_req->rat_pri_list_info;

            rr_PLMN_request_details_ptr->network_select_mode = rr_interrat_handover_req->network_select_mode;

            if (rr_interrat_handover_req->frequency_band == RRC_RR_PCS_1900)
            {
              rr_inter_ho_data_ptr->rr_x2g_ho_band_ind = RR_L1_PCS_1900;
              pcell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_PCS1900;
              scell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_PCS1900;
            }
            else
            {
              rr_inter_ho_data_ptr->rr_x2g_ho_band_ind = RR_L1_DCS_1800;
              pcell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_DCS1800;
              scell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_DCS1800;
            }

#ifdef FEATURE_LTE
            rr_inter_ho_data_ptr->dedicated_priority_info = rr_interrat_handover_req->dedicated_priority_info;
#endif /* FEATURE_LTE */

            /* Store the GSM command info from the inter-RAT handover request */
            rr_w2g_store_gsm_commands(rr_interrat_handover_req, gas_id);

            /* copy the measurement data from rrc handover req */
            handover_details_ptr->interrat_measurement_data.number_of_cells =
                rr_interrat_handover_req->interrat_measurement_data.number_of_cells;

            for (i=0;i < handover_details_ptr->interrat_measurement_data.number_of_cells; i++)
            {
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].bcch_arfcn =
                rr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].bcch_arfcn;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].bsic_bcc =
                rr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].bsic_bcc;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].bsic_ncc =
                rr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].bsic_ncc;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].fn_offset =
                rr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].fn_offset;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].qbit_offset =
                rr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].qbit_offset;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].rssi_dBm_x16 =
                rr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].rssi_dBm_x16;

              /* @@OG: fine freq */
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].frequency_offset =
                rr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].frequency_offset;
            }

            /* set a pointer to the first command in the array */
            rr_x2g_ho_gsm_cmd_ptr = rr_inter_ho_data_ptr->gsm_command_list.gsm_commands;
            dedic_ptr->handover_cmd_length = rr_x2g_ho_gsm_cmd_ptr->message_length;

            /* if ciphering was enabled on RRC Side, RR should behave as if
             * cipher has been activated on 2G
             */
            rr_set_ciphering_mode_command_received(
              rr_interrat_handover_req->cipher_enabled,   // cmd_rcvd
              gas_id                                      // gas_id
            );

            /******************************************************/
            /* Verify the Command received from RRC               */
            /******************************************************/
            if (rr_x2g_ho_gsm_cmd_ptr->payload[1] == HANDOVER_COMMAND)
            {
              RR_cause_T error_type;
              boolean    message_ok;
#ifdef FEATURE_LTE
              boolean    redir = rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id);
              boolean    redir_after_csfb = rr_g2l_blind_redir_after_csfb_enabled(gas_id);
#endif /* FEATURE_LTE */

              /* log the signalling message here */
              (void) rr_log_signaling_message(
                DCCH,
                HANDOVER_COMMAND,
                rr_x2g_ho_gsm_cmd_ptr->message_length,
                rr_x2g_ho_gsm_cmd_ptr->payload,
                RR_DOWNLINK_MSG,
                gas_id
              );

              rr_check_for_protocol_errors(
                rr_x2g_ho_gsm_cmd_ptr->payload,
                rr_x2g_ho_gsm_cmd_ptr->message_length,
                DCCH,   /* treat as DCCH */
                FALSE,  /* not short header format */
                &message_ok,
                &error_type,
                gas_id
              );

              if (message_ok == TRUE)
              {
                MSG_GERAN_HIGH_1_G("InterRAT WCDMA To GSM HandoverCommand received (cipher_enabled: %d)",
                                   rr_interrat_handover_req->cipher_enabled);
                /* If ciphering is not enabled on WCDMA, but the handover
                command indicates that ciphering should be started on GSM then
                this indicates that a Security Mode Procedure has been
                completed on WCDMA.  In this case, RR will have received a new
                cipher key in RR_SIM_UPDATE_REQ, and will have buffered this
                cipher key.  As ciphering was not enabled in WCDMA, there will
                have been no RRC_CIPHER_SYNC_IND, so the new cipher key has not
                yet been activated.  Before starting ciphering, RR must
                activate the new cipher key.

                If ciphering is enabled on WCDMA, RR should use the current
                cipher key, so any new key should not be activated. */
                if (rr_interrat_handover_req->cipher_enabled == FALSE)
                {
                  new_handover_event = rr_respond_to_handover_command(dedic_ptr, TRUE, gas_id);
                }
                else
                {
                  new_handover_event = rr_respond_to_handover_command(dedic_ptr, FALSE, gas_id);
                }

#ifdef FEATURE_LTE
                MSG_GERAN_HIGH_3_G("G2L blind redir NV: after_voice_ss_or_periodic_lau=%d, after_CSFB=%d, csfb_call_status=%d",
                    redir, redir_after_csfb, rr_interrat_handover_req->csfb_call_status);
                if (redir_after_csfb && rr_interrat_handover_req->csfb_call_status)
                {
                  rr_connection_release_set_g2l_blind_redir_after_csfb_flag(gas_id);
                }
                else if(redir)
                {
                  rr_connection_release_set_g2l_blind_redir_flag(gas_id);
                }
#endif /* FEATURE_LTE */
              }
              else
              {
                ERR("ProtocolError in Received WCDMA To GSM HandoverCommand",0,0,0);
                rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PROTOCOL_ERROR;
                new_handover_event = RR_EV_W2G_HO_PROTOCOL_ERROR;
              }
            } /* (message_type == HANDOVER_COMMAND) */
            else
            {
              ERR("Something wrong with Received WCDMA To GSM HandoverCommand",0,0,0);
              rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PROTOCOL_ERROR;
              new_handover_event = RR_EV_W2G_HO_PROTOCOL_ERROR;
            }
          }
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            ERR("Received Inter-RAT reselection Abort from RRC",0,0,0);
            rr_send_interrat_ho_abort_cnf(RRC_RR_SUCCESS, gas_id);
            rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
            rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
          }
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            ERR("Received Abort Req from MM",0,0,0);
            rr_send_abort_cnf(MM_AS_INTERRAT_HANDOVER, gas_id);
            rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
            rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_REQ))
          {
            tdsrr_interrat_handover_req_type *tdsrr_interrat_handover_req;
            rr_x2g_ho_gsm_command_t          *rr_x2g_ho_gsm_cmd_ptr;
            int                               i;
            rr_PLMN_request_details_T        *rr_PLMN_request_details_ptr;

            rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
            rr_tds_pseudo_fr_reset_dedicated_info(gas_id);
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

            tdsrr_interrat_handover_req = (&((rr_cmd_bdy_type *) new_message)->tdsrr_ho_req);

            /*****************************************************/
            /* Perform the handover details store initialisation */
            /*****************************************************/
            rr_init_handover_details(
              handover_details_ptr,   // handover_details_ptr
              RR_L1_TDS_GSM,          // handover_type
              gas_id                  // gas_id
            );

            rr_eng_mode_update_codec_ho_info(
              dedic_ptr->curr_channel_spec.channel_mode_1,
              handover_details_ptr->channel_spec.channel_mode_1,
              handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
              FALSE,
              SYS_HO_START,
              SYS_HO_TYPE_NONE,
              gas_id
            );

            rr_eng_mode_update_codec_ho_info(
              dedic_ptr->curr_channel_spec.channel_mode_1,
              handover_details_ptr->channel_spec.channel_mode_1,
              handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
              FALSE,
              SYS_SPEECH_CODEC_INFO,
              SYS_HO_TYPE_NONE,
              gas_id
            );

            rr_PLMN_request_details_ptr->rat_pri_list_info = tdsrr_interrat_handover_req->rat_pri_list_info;

            rr_PLMN_request_details_ptr->network_select_mode = tdsrr_interrat_handover_req->network_select_mode;

            if (tdsrr_interrat_handover_req->frequency_band == TDSRR_PCS_1900)
            {
              rr_inter_ho_data_ptr->rr_x2g_ho_band_ind = RR_L1_PCS_1900;
              pcell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_PCS1900;
              scell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_PCS1900;
            }
            else
            {
              rr_inter_ho_data_ptr->rr_x2g_ho_band_ind = RR_L1_DCS_1800;
              pcell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_DCS1800;
              scell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_DCS1800;
            }

#ifdef FEATURE_LTE
            #ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
            if (rr_dedicated_priority_info_valid(&tdsrr_interrat_handover_req->dedicated_priority_info))
            {
              rr_inter_ho_data_ptr->dedicated_priority_info = tdsrr_interrat_handover_req->dedicated_priority_info;
            }
            else
            #endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/
            {
              memset(&rr_inter_ho_data_ptr->dedicated_priority_info,0,sizeof(rr_inter_ho_data_ptr->dedicated_priority_info));
            }
#endif /* FEATURE_LTE */

            /* Store the GSM command info from the inter-RAT handover request */
            rr_t2g_store_gsm_commands(tdsrr_interrat_handover_req, gas_id);

            /* copy the measurement data from rrc handover req */
            handover_details_ptr->interrat_measurement_data.number_of_cells =
                tdsrr_interrat_handover_req->interrat_measurement_data.number_of_cells;

            for (i=0;i < handover_details_ptr->interrat_measurement_data.number_of_cells; i++)
            {
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].bcch_arfcn =
                tdsrr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].bcch_arfcn;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].bsic_bcc =
                tdsrr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].bsic_bcc;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].bsic_ncc =
                tdsrr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].bsic_ncc;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].fn_offset =
                tdsrr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].fn_offset;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].qbit_offset =
                tdsrr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].qbit_offset;
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].rssi_dBm_x16 =
                tdsrr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].rssi_dBm_x16;

              /* @@OG: fine freq */
              handover_details_ptr->interrat_measurement_data.interrat_measurements[i].frequency_offset =
                tdsrr_interrat_handover_req->interrat_measurement_data.interrat_measurements[i].frequency_offset;
            }

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
            if (tdsrr_interrat_handover_req->pseudo_fr_active)
            {
              tdsrr_ho_pseudo_fast_return_type pseudo_fr = tdsrr_interrat_handover_req->pseudo_fr_info;
              utran_tdd_reselection_freq_list_T l1_tdd_freq_list;

              MSG_GERAN_MED_1_G("RR TDS Pseudo FR enabled, num_freq: %d",pseudo_fr.num_frequencies);

              /*Pseudo FR error checking,  check num frequencies is not greater than the TDS supplied list OR list that
                is going to be send to L1 in the MPH_DED_WCDMA_CELL_UPDATE_LIST*/
              if ( (pseudo_fr.num_frequencies > ARR_SIZE(tdsrr_interrat_handover_req->pseudo_fr_info.frequency_list)) ||
                   (pseudo_fr.num_frequencies > ARR_SIZE(l1_tdd_freq_list.uarfcn)) )
              {
                MSG_GERAN_ERROR_3_G("RR PFR num frequencies %d exceeds array, src max:%d, dest max:%d",
                          pseudo_fr.num_frequencies,
                          ARR_SIZE(tdsrr_interrat_handover_req->pseudo_fr_info.frequency_list),
                          ARR_SIZE(l1_tdd_freq_list.uarfcn));

                if (ARR_SIZE(tdsrr_interrat_handover_req->pseudo_fr_info.frequency_list) >
                    ARR_SIZE(l1_tdd_freq_list.uarfcn))
                {
                  pseudo_fr.num_frequencies = ARR_SIZE(l1_tdd_freq_list.uarfcn);
                }
                else
                {
                  pseudo_fr.num_frequencies = ARR_SIZE(tdsrr_interrat_handover_req->pseudo_fr_info.frequency_list);
                }

                MSG_GERAN_ERROR_1_G("RR PFR num frequencies changed to %d",pseudo_fr.num_frequencies);
              }

              if (pseudo_fr.rscp_threshold > 90)
              {
                MSG_GERAN_ERROR_1_G("RR PFR rscp_threshold out of range %d, changed to 90",pseudo_fr.rscp_threshold);
                pseudo_fr.rscp_threshold = 90;
              }

              rr_inter_ho_data_ptr->tds_pseudo_fr_active = TRUE;
              rr_inter_ho_data_ptr->tds_pseudo_fr_info = pseudo_fr;
            }
            else
            {
              rr_inter_ho_data_ptr->tds_pseudo_fr_active = FALSE;
            }
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

            /* set a pointer to the first command in the array */
            rr_x2g_ho_gsm_cmd_ptr = rr_inter_ho_data_ptr->gsm_command_list.gsm_commands;
            dedic_ptr->handover_cmd_length = rr_x2g_ho_gsm_cmd_ptr->message_length;

            /* if ciphering was enabled on RRC Side, RR should behave as if
             * cipher has been activated on 2G
             */
            rr_set_ciphering_mode_command_received(
              tdsrr_interrat_handover_req->cipher_enabled,    // cmd_rvcd
              gas_id                                          // gas_id
            );

            /******************************************************/
            /* Verify the Command received from RRC               */
            /******************************************************/
            if (rr_x2g_ho_gsm_cmd_ptr->payload[1] == HANDOVER_COMMAND)
            {
              RR_cause_T error_type;
              boolean    message_ok;
#ifdef FEATURE_LTE
              boolean    redir = rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id);
#endif /*FEATURE_LTE*/
              /* log the signalling message here */
              (void) rr_log_signaling_message(
                DCCH,
                HANDOVER_COMMAND,
                rr_x2g_ho_gsm_cmd_ptr->message_length,
                rr_x2g_ho_gsm_cmd_ptr->payload,
                RR_DOWNLINK_MSG,
                gas_id
              );

              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->curr_channel_spec.channel_mode_1,
                handover_details_ptr->channel_spec.channel_mode_1,
                handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
                TRUE,
                SYS_HO_START,
                SYS_HO_TYPE_NONE,
                gas_id
              );

              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->curr_channel_spec.channel_mode_1,
                handover_details_ptr->channel_spec.channel_mode_1,
                handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
                TRUE,
                SYS_SPEECH_CODEC_INFO,
                SYS_HO_TYPE_NONE,
                gas_id
              );

              rr_check_for_protocol_errors(
                rr_x2g_ho_gsm_cmd_ptr->payload,
                rr_x2g_ho_gsm_cmd_ptr->message_length,
                DCCH,   /* treat as DCCH */
                FALSE,  /* not short header format */
                &message_ok,
                &error_type,
                gas_id
              );

              if (message_ok == TRUE)
              {
                MSG_GERAN_HIGH_1_G("InterRAT TDS To GSM HandoverCommand received (cipher_enabled: %d)",
                                   tdsrr_interrat_handover_req->cipher_enabled);
                /* If ciphering is not enabled on TDSCDMA, but the handover
                command indicates that ciphering should be started on GSM then
                this indicates that a Security Mode Procedure has been
                completed on TDSCDMA.  In this case, RR will have received a new
                cipher key in RR_SIM_UPDATE_REQ, and will have buffered this
                cipher key.  As ciphering was not enabled in WCDMA, there will
                have been no TDSRRRR_CIPHER_SYNC_IND, so the new cipher key has not
                yet been activated.  Before starting ciphering, RR must
                activate the new cipher key.

                If ciphering is enabled on WCDMA, RR should use the current
                cipher key, so any new key should not be activated. */
                if (tdsrr_interrat_handover_req->cipher_enabled == FALSE)
                {
                  new_handover_event = rr_respond_to_handover_command(dedic_ptr, TRUE, gas_id);
                }
                else
                {
                  new_handover_event = rr_respond_to_handover_command(dedic_ptr, FALSE, gas_id);
                }
#ifdef FEATURE_LTE
                MSG_GERAN_HIGH_1_G("T2G HO :G2L blind redir NV: after_voice_ss_or_periodic_lau=%d",redir);

                if(redir)
                {
                  rr_connection_release_set_g2l_blind_redir_flag(gas_id);
                }
#endif /*FEATURE_LTE*/
              }
              else
              {
                ERR("ProtocolError in Received TDS To GSM HandoverCommand",0,0,0);
                rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PROTOCOL_ERROR;
                new_handover_event = RR_EV_W2G_HO_PROTOCOL_ERROR;
              }
            } /* (message_type == HANDOVER_COMMAND) */
            else
            {
              ERR("Something wrong with Received TDS To GSM HandoverCommand",0,0,0);
              rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PROTOCOL_ERROR;
              new_handover_event = RR_EV_W2G_HO_PROTOCOL_ERROR;
            }
          }
          else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            ERR("Received Inter-RAT reselection Abort from TDSRRC",0,0,0);
            rr_send_tds_interrat_ho_abort_cnf(TDSRRC_RR_SUCCESS, gas_id);
            rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
            rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
          }
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            ERR("Received Abort Req from MM",0,0,0);
            rr_send_abort_cnf(MM_AS_INTERRAT_HANDOVER, gas_id);
            rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
            rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
          }
#endif /* FEATURE_GSM_TDS */

          else if (message_set == MS_MSGR_RR)
          {
            switch (new_message->rr_msgr_msg.message.hdr.id)
            {
#ifdef FEATURE_LTE
              case GERAN_GRR_LTE_HO_REQ:
              {
                geran_grr_lte_ho_req_t    * lte_ho_req = &new_message->rr_msgr_msg.message.geran_grr_msg_req.lte_ho_req;
                rr_x2g_ho_gsm_command_t   * rr_x2g_ho_gsm_cmd_ptr;
                rr_PLMN_request_details_T * rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
                uint32                      i;

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
                rr_tds_pseudo_fr_reset_dedicated_info(gas_id);
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

                /*****************************************************/
                /* Perform the handover details store initialisation */
                /*****************************************************/
                rr_init_handover_details(handover_details_ptr, RR_L1_LTE_GSM, gas_id);

                rr_PLMN_request_details_ptr->rat_pri_list_info = lte_ho_req->rat_pri_list_info;

                rr_PLMN_request_details_ptr->network_select_mode = lte_ho_req->network_select_mode;

                if (lte_ho_req->band_ind == GERAN_GRR_BAND_IND_DCS_1800)
                {
                  rr_inter_ho_data_ptr->rr_x2g_ho_band_ind = RR_L1_DCS_1800;
                  pcell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_DCS1800;
                  scell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_DCS1800;
                }
                else
                {
                  rr_inter_ho_data_ptr->rr_x2g_ho_band_ind = RR_L1_PCS_1900;
                  pcell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_PCS1900;
                  scell_info_ptr->gsm.band_indicator = SI_PSI_BAND_IND_PCS1900;
                }

                rr_siminfo_store_cipher_keys(
                  lte_ho_req->security_keys.kc,          // cipher_key
                  lte_ho_req->security_keys.kc_128bit,   // cipher_key_128
                  TRUE,                                  // rr_active
                  geran_map_gas_id_to_nas_id(gas_id)     // as_id
                );

                rr_siminfo_store_cipher_key_seq(
                  lte_ho_req->security_keys.kcsn,      // cipher_key_seq
                  TRUE,                                // rr_active
                  geran_map_gas_id_to_nas_id(gas_id)   // as_id
                );

                /* Store the GSM command info from the inter-RAT handover request */
                rr_l2g_store_gsm_commands(lte_ho_req, gas_id);

                /* copy the measurement data from rrc handover req */
                handover_details_ptr->interrat_measurement_data.number_of_cells =
                  (uint8) MIN(lte_ho_req->meas_data_list.number_of_freq, GERAN_GRR_LTE_MAX_INTERRAT_MEASUREMENTS);

                MSG_GERAN_HIGH_1_G("Num of cells is %d",
                                   handover_details_ptr->interrat_measurement_data.number_of_cells);

                for (i = 0; i < handover_details_ptr->interrat_measurement_data.number_of_cells; ++i)
                {
                  rr_l1_interrat_measurement_T * dst = &handover_details_ptr->interrat_measurement_data.interrat_measurements[i];

                  MSG_GERAN_HIGH_1_G("IRAT Measurement [%d]:", i);

                  dst->bcch_arfcn.num = lte_ho_req->meas_data_list.geran_measurements[i].arfcn;

                  if (lte_ho_req->meas_data_list.geran_measurements[i].band_ind == GERAN_GRR_BAND_IND_PCS_1900)
                  {
                    dst->bcch_arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
                      dst->bcch_arfcn.num,
                      SI_PSI_BAND_IND_PCS1900,
                      gas_id
                    );
                  }
                  else
                  {
                    dst->bcch_arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
                      dst->bcch_arfcn.num,
                      SI_PSI_BAND_IND_DCS1800,
                      gas_id
                    );
                  }
                  dst->bsic_bcc = lte_ho_req->meas_data_list.geran_measurements[i].bsic.bcc;
                  dst->bsic_ncc = lte_ho_req->meas_data_list.geran_measurements[i].bsic.ncc;
                  dst->fn_offset = lte_ho_req->meas_data_list.geran_measurements[i].fn_offset;
                  dst->qbit_offset = lte_ho_req->meas_data_list.geran_measurements[i].qbit_offset;
                  dst->rssi_dBm_x16 = lte_ho_req->meas_data_list.geran_measurements[i].rssi_dBm_x16;
                  dst->frequency_offset = lte_ho_req->meas_data_list.geran_measurements[i].frequency_offset;
                  MSG_GERAN_HIGH_3_G("BCC, NCC, Fn_Offset are %d, %d, %d",
                                     dst->bsic_bcc ,
                                     dst->bsic_ncc,
                                     dst->fn_offset);
                  MSG_GERAN_HIGH_3_G("QBIT_offset, RSSI, Freq Offset are %d, %d, %d",
                                     dst->qbit_offset,
                                     dst->rssi_dBm_x16,
                                     dst->frequency_offset);
                }

                /* set a pointer to the first command in the array */
                rr_x2g_ho_gsm_cmd_ptr = rr_inter_ho_data_ptr->gsm_command_list.gsm_commands;
                dedic_ptr->handover_cmd_length = rr_x2g_ho_gsm_cmd_ptr->message_length;

                /* if ciphering was enabled on RRC Side, RR should behave as if
                 * cipher has been activated on 2G
                 */
                //rr_set_ciphering_mode_command_received(rr_interrat_handover_req->cipher_enabled);

                /******************************************************/
                /* Verify the Command received from RRC               */
                /******************************************************/
                if (rr_x2g_ho_gsm_cmd_ptr->payload[1] == HANDOVER_COMMAND)
                {
                  RR_cause_T error_type;
                  boolean    message_ok;

                  /* log the signalling message here */
                  (void)rr_log_signaling_message(DCCH,
                                                 HANDOVER_COMMAND,
                                                 rr_x2g_ho_gsm_cmd_ptr->message_length,
                                                 rr_x2g_ho_gsm_cmd_ptr->payload,
                                                 RR_DOWNLINK_MSG,
                                                 gas_id);

                  rr_check_for_protocol_errors(rr_x2g_ho_gsm_cmd_ptr->payload,
                                               rr_x2g_ho_gsm_cmd_ptr->message_length,
                                               DCCH,   /* treat as DCCH */
                                               FALSE,  /* not short header format */
                                               &message_ok,
                                               &error_type,
                                               gas_id);

                  if (message_ok == TRUE)
                  {
                    MSG_GERAN_HIGH_0_G("InterRAT LTE To GSM HandoverCommand received");
                    new_handover_event = rr_respond_to_handover_command(dedic_ptr, FALSE, gas_id);
                  }
                  else
                  {
                    MSG_GERAN_ERROR_0_G("ProtocolError in Received LTE To GSM HandoverCommand");
                    rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PROTOCOL_ERROR;
                    new_handover_event = RR_EV_W2G_HO_PROTOCOL_ERROR;
                  }

                  if (new_handover_event != EV_HANDOVER_COMMAND_CHECKED )
                  {
                     sys_codec_ho_info ho_codec_info = {0};
                     ho_codec_info.cmd = SYS_HO_START;
                     ho_codec_info.ho_type = SYS_HO_TYPE_LTE_GSM_SRVCC;
                     ho_codec_info.speech_codec = SYS_SPEECH_CODEC_NONE;
                     ho_codec_info.speech_enc_samp_freq = 0;
                     MSG_GERAN_HIGH_0_G("Notify CM about the erroneous HO cmd for SRVCC");
                     geran_eng_mode_data_write(ENG_MODE_CODEC_HO_INFO,&ho_codec_info,TRUE,gas_id);
                  }
                } /* (message_type == HANDOVER_COMMAND) */
                else
                {
                  MSG_GERAN_ERROR_0_G("Something wrong with Received LTE To GSM HandoverCommand");
                  rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PROTOCOL_ERROR;
                  new_handover_event = RR_EV_W2G_HO_PROTOCOL_ERROR;
                }
                break;
              } /* GERAN_GRR_LTE_HO_REQ */

              case GERAN_GRR_LTE_ABORT_HO_REQ:
              {
                MSG_GERAN_ERROR_0_G("Unexpected handover abort request from LTE RRC");
                rr_send_lte_abort_ho_rsp(gas_id);
                rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
                rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
                break;
              } /* GERAN_GRR_LTE_ABORT_REQ */
#endif /* FEATURE_LTE */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message, "HANDOVER_INACTIVE");
              }
            }
          }

        }
        else if (handover_event == EV_HANDOVER_COMMAND_CHECKED)
        {
          switch (handover_details_ptr->handover_type)
          {
            case RR_L1_GSM_GSM:
              proc_dispatch_event = EV_INTER_CELL_HANDOVER;


              rr_eng_mode_update_codec_ho_info(dedic_ptr->curr_channel_spec.channel_mode_1,
                                               handover_details_ptr->channel_spec.channel_mode_1,
                                               handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
                                               FALSE,
                                               SYS_HO_START,
                                               SYS_HO_TYPE_GSM_GSM,
                                               gas_id);

              rr_eng_mode_update_codec_ho_info(dedic_ptr->curr_channel_spec.channel_mode_1,
                                               handover_details_ptr->channel_spec.channel_mode_1,
                                               handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
                                               FALSE,
                                               SYS_SPEECH_CODEC_INFO,
                                               SYS_HO_TYPE_GSM_GSM,
                                               gas_id);

              rr_inter_ho_data_ptr->handover_state = AWAITING_LOCAL_END_SUSPENSION;
              break;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
            case RR_L1_WCDMA_GSM:
            case RR_L1_TDS_GSM:
            case RR_L1_LTE_GSM:
            {
              /* Set the global stop reason to RR_L1_RAT_CHANGE - if this
              procedure fails RR will stop L1 and return control to WCDMA RRC.
              This stop reason is sent to L1 in MPH_STOP_GSM_MODE_REQ and must
              be configured correctly or clock regime failures occur on
              resumption of WCDMA. */
              rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

              if (rr_inter_ho_data_ptr->rr_x2g_ho_band_ind == RR_L1_PCS_1900)
              {
                rr_set_handover_band_info(
                  handover_details_ptr,       // handover_details_ptr
                  SI_PSI_BAND_IND_PCS1900,    // rr_band_ind
                  RR_L1_PCS_1900,             // l1_band_ind
                  gas_id                      // gas_id
                );
              }
              else
              {
                rr_set_handover_band_info(
                  handover_details_ptr,       // handover_details_ptr
                  SI_PSI_BAND_IND_DCS1800,    // rr_band_ind
                  RR_L1_DCS_1800,             // l1_band_ind
                  gas_id                      // gas_id
                );
              }
              MSG_GERAN_HIGH_1_G("X2GHO: rr_x2g_ho_band_ind=%d", (int)rr_inter_ho_data_ptr->rr_x2g_ho_band_ind);

              /* time to Activate the GSM RAT */
              rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_ACTIVE;

              /* Send Activate event to mode control */
              new_handover_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);

              rr_eng_mode_update_codec_ho_info(dedic_ptr->curr_channel_spec.channel_mode_1,
                                               handover_details_ptr->channel_spec.channel_mode_1,
                                               handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
                                               TRUE,
                                               SYS_HO_START,
                                               rr_map_rr_l1_handover_type_to_sys_ho_type(handover_details_ptr->handover_type),
                                               gas_id);

              rr_eng_mode_update_codec_ho_info(dedic_ptr->curr_channel_spec.channel_mode_1,
                                               handover_details_ptr->channel_spec.channel_mode_1,
                                               handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
                                               TRUE,
                                               SYS_SPEECH_CODEC_INFO,
                                               rr_map_rr_l1_handover_type_to_sys_ho_type(handover_details_ptr->handover_type),
                                               gas_id);
              break;
            }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
            default:
            {
              MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
            }
          }
        } /* (handover_event == EV_HANDOVER_COMMAND_CHECKED) */
        else if (handover_event == EV_RESET_SOFTWARE)
        {
          /* ignore in this state */
        }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        else if (handover_event == RR_EV_W2G_HO_PROTOCOL_ERROR)
        {
          if (handover_details_ptr->handover_type == RR_L1_WCDMA_GSM)
          {
#ifdef FEATURE_WCDMA
            // Send handover confirm with Failure reason
            rrc_rr_gsm_command_list_type wcdma_gsm_cmd_list;

            rr_convert_x2g_ho_gsm_cmds_to_wcdma(
              &rr_inter_ho_data_ptr->gsm_command_list,
              &wcdma_gsm_cmd_list
            );

            rr_send_interrat_ho_cnf(
              RRC_RR_FAILURE,
              rr_convert_rr_x2g_ho_failure_cause_to_wcdma(rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason),
              &wcdma_gsm_cmd_list,
              gas_id
            );
#endif /* FEATURE_WCDMA */
          }
#ifdef FEATURE_GSM_TDS
          else if (handover_details_ptr->handover_type == RR_L1_TDS_GSM)
          {
            tdsrrc_rr_gsm_command_list_type tds_gsm_cmd_list;

            rr_convert_x2g_ho_gsm_cmds_to_tds(
              &rr_inter_ho_data_ptr->gsm_command_list,
              &tds_gsm_cmd_list
            );

            rr_send_tds_interrat_ho_cnf(
              TDSRRC_RR_FAILURE,
              rr_convert_rr_x2g_ho_failure_cause_to_tds(rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason),
              &tds_gsm_cmd_list,
              gas_id
            );
          }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
          else if (handover_details_ptr->handover_type == RR_L1_LTE_GSM)
          {
            rr_send_lte_ho_failed_rsp(
              rr_convert_rr_x2g_ho_failure_cause_to_lte(rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason),
              gas_id
            );
            rr_send_srvcc_handover_failure_ind(gas_id);
          }
#endif /* FEATURE_LTE */
          proc_dispatch_event = RR_EV_W2G_GO_TO_INACTIVE;
          rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
        }
        else if (handover_event == RR_EV_G2W_HO_FAIL)
        {
          rr_send_mph_reconnect_channel_req(gas_id);
          /* Pre-set the number_of_ded_channels to 1; If reconnect fails this
             should be set to 0 */
          dedic_ptr->curr_channel_spec.number_of_ded_channels = 1;
          rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;
        }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in INACTIVE");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }
        else
        {
          MSG_GERAN_ERROR_2_G("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state);
        }
        break;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    /****************************************************************/
    /* STATE: RR_W2G_HO_GOING_ACTIVE                                */
    /* In this state RR is waiting for the GSM AS to be activated   */
    /* before continuing process the Inter-RAT handover sequence    */
    /****************************************************************/
    case RR_W2G_HO_GOING_ACTIVE :

      /***************************************************************/
      /* Here is the confirmation from mode control that GSM L1      */
      /* is activated.                                               */
      /***************************************************************/

      if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
      {
        if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
        {
          ERR("RR shouldn't get this message, MM is weird state",0,0,0);
          // No Handover Cnf will be sent to RRC in this case
          new_handover_event = deactivate_during_x2g_handover(gas_id);
          rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
        }
#ifdef FEATURE_WCDMA
        else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
        {
          MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
          new_handover_event = deactivate_during_x2g_handover(gas_id);
          rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
        }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
        else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
        {
          MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
          new_handover_event = deactivate_during_x2g_handover(gas_id);
          rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
        }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
        else if (message_set == MS_MSGR_RR && new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
        {
          MSG_GERAN_HIGH_0_G("Received abort from LTE RRC");
          new_handover_event = deactivate_during_x2g_handover(gas_id);
          rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
        }
#endif /* FEATURE_LTE */
#ifdef FEATURE_SGLTE
        else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_DEACT_REQ))
        {
          MSG_GERAN_HIGH_0_G("Deact rcvd abort the ongoing L2g Ho process");
          new_handover_event = deactivate_during_x2g_handover(gas_id);
          rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
        }
#endif /*FEATURE_SGLTE*/
        else
        {
          ERR("Unexpected message (%d,%d) in RR_W2G_HO_GOING_ACTIVE",(int) message_set, (int) message_id,0);
        }
      }  /* EV_INPUT_MESSAGE */

      else if (handover_event == EV_GSM_MODE_ACTIVATED)
      {
        // Suspend the L2 link
        rr_send_dl_suspend_req(handover_details_ptr->handover_type, gas_id);
        rr_inter_ho_data_ptr->handover_state = AWAITING_LOCAL_END_SUSPENSION;
        handover_event = EV_NO_EVENT;
      }
      else if (handover_event == EV_PANIC_RESET_REQ)
      {
        MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in GOING_ACTIVE");
        proc_dispatch_event = EV_PANIC_RESET_CNF;
      }
      else
      {
        ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
      }
      break;

      /****************************************************************/
      /* STATE: RR_W2G_HO_GOING_INACTIVE                             */
      /* In this state RR is waiting for the GSM AS to be deactivated*/
      /* and gets ready for next handover command processing         */
      /***************************************************************/

    case RR_W2G_HO_GOING_INACTIVE :

      /***************************************************************/
      /* Here is the confirmation from mode control that GSM L1      */
      /* is deactivated.                                             */
      /***************************************************************/

      if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
      {
        if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
        {
          // No Need to do any thing here.
          // Since once GSM Mode is deactivated, appropriate action will be taken
        }
#ifdef FEATURE_WCDMA
        else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
        {
          // No need to do any thing here.
          // Since once GSM Mode is deactivated, appropriate action will be taken
        }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
        else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
        {
          /* No need to do any thing here.
             Since once GSM Mode is deactivated, appropriate action will be taken */
        }
#endif /*FEATURE_GSM_TDS*/
        else if ((message_set == MS_RR_L2) &&
                 ((message_id == (byte) DL_RELEASE_IND) ||
                  (message_id == (byte) MDL_ERROR_IND)))
        {
          // Time to send Responses to MM / RRC ( which ever is appropriate )
          // and move to INACTIVE state.
          if (!rr_inter_ho_data_ptr->rr_interrat_ho_abort_received)
          {
            if(rr_inter_ho_data_ptr->rr_mm_abort_received)
            {
              rr_send_abort_cnf(MM_AS_INTERRAT_HANDOVER, gas_id);
            }
            else
            {
              if (handover_details_ptr->handover_type == RR_L1_WCDMA_GSM)
              {
#ifdef FEATURE_WCDMA
                rrc_rr_gsm_command_list_type wcdma_gsm_cmd_list;
    
                rr_convert_x2g_ho_gsm_cmds_to_wcdma(
                  &rr_inter_ho_data_ptr->gsm_command_list,
                  &wcdma_gsm_cmd_list
                );
    
                rr_send_interrat_ho_cnf(
                  RRC_RR_FAILURE,
                  rr_convert_rr_x2g_ho_failure_cause_to_wcdma(rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason),
                  &wcdma_gsm_cmd_list,
                  gas_id
                );
#endif /* FEATURE_WCDMA */
              }
 #ifdef FEATURE_GSM_TDS
              else if (handover_details_ptr->handover_type == RR_L1_TDS_GSM)
              {
                tdsrrc_rr_gsm_command_list_type tds_gsm_cmd_list;

                rr_convert_x2g_ho_gsm_cmds_to_tds(
                  &rr_inter_ho_data_ptr->gsm_command_list,
                  &tds_gsm_cmd_list
                );

                rr_send_tds_interrat_ho_cnf(
                  TDSRRC_RR_FAILURE,
                  rr_convert_rr_x2g_ho_failure_cause_to_tds(rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason),
                  &tds_gsm_cmd_list,
                  gas_id
                );
              }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
              else if (handover_details_ptr->handover_type == RR_L1_LTE_GSM)
              {
                rr_send_lte_ho_failed_rsp(
                  rr_convert_rr_x2g_ho_failure_cause_to_lte(rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason),
                  gas_id
                );
                rr_send_srvcc_handover_failure_ind(gas_id);
              }
#endif /* FEATURE_LTE */
            }
          }
          else // We must have received the HO_ABORT_REQ
          {
            if(rr_inter_ho_data_ptr->rr_mm_abort_received)
            {
              rr_send_abort_cnf(MM_AS_INTERRAT_HANDOVER, gas_id);
            }

            if (handover_details_ptr->handover_type == RR_L1_WCDMA_GSM)
            {
#ifdef FEATURE_WCDMA
              rr_send_interrat_ho_abort_cnf(RRC_RR_SUCCESS, gas_id);
#endif /* FEATURE_WCDMA */
            }
#ifdef FEATURE_GSM_TDS
            else if (handover_details_ptr->handover_type == RR_L1_TDS_GSM)
            {
              rr_send_tds_interrat_ho_abort_cnf(TDSRRC_RR_SUCCESS, gas_id);
            }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
            else if (handover_details_ptr->handover_type == RR_L1_LTE_GSM)
            {
              rr_send_lte_abort_ho_rsp(gas_id);
            }
#endif /* FEATURE_LTE */
          }
          rr_eng_mode_update_codec_ho_info(
            dedic_ptr->curr_channel_spec.channel_mode_1,
            handover_details_ptr->channel_spec.channel_mode_1,
            handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
            TRUE,
            SYS_HO_FAIL,
            rr_map_rr_l1_handover_type_to_sys_ho_type(handover_details_ptr->handover_type),
            gas_id
          );


          rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
          // send this to put L2 in a known state.
          rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
          proc_dispatch_event = RR_EV_W2G_GO_TO_INACTIVE;
        }
        else
        {
          ERR("Unexpected message (%d,%d) in RR_W2G_HO_GOING_INACTIVE",(int) message_set, (int) message_id,0);
        }
      }
      else if (handover_event == EV_GSM_MODE_DEACTIVATED)
      {
        // GSM RAT is deactivated, send mdl_release_req to L2.
        rr_send_mdl_release_req(
          SAPI0,    // SAPI
          DCCH,     // l2_channel_type
          gas_id    // gas_id
        );
      }
      else if (handover_event == EV_PANIC_RESET_REQ)
      {
        MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in GOING_INACTIVE");
        proc_dispatch_event = EV_PANIC_RESET_CNF;
      }
      else
      {
        MSG_GERAN_ERROR_2_G("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state);
      }
      break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

      /****************************************************************/
      /* STATE: AWAITING_LOCAL_END_SUSPENSION                        */
      /* In this state we are waiting for the Local Data link to be  */
      /* Suspended.                                                  */
      /***************************************************************/

      case AWAITING_LOCAL_END_SUSPENSION:

        /***************************************************************/
        /* Here is the confirmation that the layer 2 is in a suspended */
        /* state                                                       */
        /***************************************************************/

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if (message_set == MS_RR_L2)
          {
            if (message_id == (byte) DL_SUSPEND_CNF)
            {
              /* Save the current channel details in case L2 resumption fails, */
              /* and we have to return to the old channel, and resume there    */
              rr_preserve_current_dedicated_config(gas_id);

              rr_send_mph_handover_req(
                handover_details_ptr,
                &(dedic_ptr->next_amr_config),
                gas_id
              );

              /* L1 stops using DTX on receipt of HANDOVER command. Keep the DTX */
              /* indication FALSE until L2 is resumed again */
              rr_dtx_rr_override_on(gas_id);

              rr_inter_ho_data_ptr->handover_state = AWAITING_HANDOVER_INDICATION;
            }
            else if ((message_id == (byte) DL_RELEASE_IND) ||
                     (message_id == (byte) MDL_ERROR_IND))
            {
              /*
               * a lower layer failure has occurred while we are
               * waiting for L2 suspension, so give up and go
               * back to idle
               */

              /* log the handover failure */
              rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

              switch (handover_details_ptr->handover_type)
              {
                case RR_L1_GSM_GSM:
                {
                  rr_inter_ho_data_ptr->handover_state =HANDOVER_INACTIVE;
                  proc_dispatch_event = EV_RADIO_LINK_FAILURE;
                  break;
                }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_L1_WCDMA_GSM:
                case RR_L1_TDS_GSM:
                case RR_L1_LTE_GSM:
                {
                  new_handover_event = deactivate_during_x2g_handover(gas_id);
                  rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
                  rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
                  break;
                }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                default:
                {
                  MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                  break;
                }
              }
            } /* (message_id == DL_RELEASE_IND or message_id == MDL_ERROR_IND) */
          } /* ( message_set == MS_RR_L2 ) */
#ifdef FEATURE_WCDMA
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_WCDMA*/
#ifdef FEATURE_GSM_TDS
          else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
          else if (message_set == MS_MSGR_RR && new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
          {
            MSG_GERAN_HIGH_0_G("Received abort from LTE RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_LTE */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Abort Req from MM");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#ifdef FEATURE_SGLTE
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_DEACT_REQ))
          {
             MSG_GERAN_HIGH_0_G("Deact rcvd abort the ongoing L2g Ho process");
             new_handover_event = deactivate_during_x2g_handover(gas_id);
             rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_SGLTE*/
          else
          {
            ERR("UnexMsg(%d,%d) in State=%d",(int) message_set, (int) message_id,(int) rr_inter_ho_data_ptr->handover_state);
          }
        } /* EV_INPUT_MESSAGE */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in AWAITING_LOCAL_END_SUSPENSION");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }
        break;

      /****************************************************************/
      /* STATE: AWAITING_HANDOVER_INDICATION                          */
      /* In this state we are waiting for Handover Indiacation From   */
      /* Layer 1.                                                     */
      /****************************************************************/
      case AWAITING_HANDOVER_INDICATION:

        /*********************************************************/
        /* The layer 1 has switched its channels to the new cell */
        /*********************************************************/

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if ( message_set == MS_RR_L1 )
          {
            if (message_id == (byte) MPH_HANDOVER_IND)
            {
              mph_handover_ind_T *mph_handover_ind;

              mph_handover_ind = (mph_handover_ind_T *) new_message;

              if ( mph_handover_ind->error_code == NO_ERROR )
              {
                switch (handover_details_ptr->handover_type)
                {
                  case RR_L1_GSM_GSM:
                  {
                    if ((handover_details_ptr->BCCH_ARFCN.num !=
                      scell_info_ptr->gsm.BCCH_ARFCN.num) ||
                      (handover_details_ptr->BSIC.PLMN_colour_code !=
                      scell_info_ptr->gsm.BSIC.PLMN_colour_code) ||
                      (handover_details_ptr->BSIC.BS_colour_code !=
                      scell_info_ptr->gsm.BSIC.BS_colour_code) ||
                      !rr_bands_equal(rr_inter_ho_data_ptr->old_band, rr_get_current_dedicated_band(gas_id)))
                    {
                      rr_clear_si_sacch(gas_id);
                      rr_ded_set_measurement_reports_invalid(TRUE, gas_id);

#ifdef FEATURE_GSM_DTM
                      // The RR-DTM module is informed initially that this is an intra-cell HO
                      // (i.e. the cell has not changed). This is because a HANDOVER COMMAND may
                      // specify the same cell. So here, where the new cell details are checked, if
                      // the cell has changed, update RR-DTM that this is in fact a inter-cell HO.
                      proc_dispatch_event = EV_INTER_CELL_HANDOVER;
#endif // FEATURE_GSM_DTM
                    }
                    break;
                  }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                  case RR_L1_WCDMA_GSM:
                  case RR_L1_TDS_GSM:
                  case RR_L1_LTE_GSM:
                  {
                    rr_ded_meas_reporting_l1_in_dedicated(gas_id);
                    rr_clear_si_sacch(gas_id);
                    rr_ded_set_measurement_reports_invalid(TRUE, gas_id);
                    break;
                  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                  default:
                  {
                    MSG_GERAN_ERROR_1_G("RR Bad handover_type: %d",handover_details_ptr->handover_type);
                    break;
                  }
                }

                /* complete the handover indication processing
                 * including update of RR's global channel data
                 */
                rr_process_handover_indication(dedic_ptr, gas_id);

                rr_inter_ho_data_ptr->handover_state = AWAITING_RANDOM_ACCESS_CONFIRMATION;

                /* Log the new channel configuration. */
                (void) rr_log_channel_configuration(gas_id);
              }
              else
              {
                /* log the handover failure */
                rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

                switch (handover_details_ptr->handover_type)
                {
                  case RR_L1_GSM_GSM:
                  {
                    rr_inter_ho_data_ptr->handover_state = HO_AWAITING_RECONNECTION;

                    /**
                     * Do not clear SACCH sys info as the serving cell has not changed
                     * this allows measurement reporting to resume more quickly using
                     * the same BA list we had before the handover command was received
                     */

                    /* Restore previously-saved channel spec and AMR config */
                    rr_restore_previous_dedicated_config(gas_id);

                    /* send the HANDOVER FAILURE message to the NW */
                    if (mph_handover_ind->error_code == HANDOVER_OUT_OF_RANGE)
                    {
                      rr_perform_handover_failure(
                        RR_HANDOVER_IMPOSSIBLE_TA_OUT_OF_RANGE,   // RR_cause
                        TRUE,                                     // reconnect
                        gas_id                                    // gas_id
                      );
                    }
                    else
                    {
                      rr_perform_handover_failure(
                        RR_PROTOCOL_ERROR_UNSPECIFIED,    // RR_cause
                        TRUE,                             // reconnect
                        gas_id                            // gas_id
                      );
                    }

                    /* Log the channel configuration as it has now been restored. */
                    (void) rr_log_channel_configuration(gas_id);

                    break;
                  }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                  case RR_L1_WCDMA_GSM:
                  case RR_L1_TDS_GSM:
                  case RR_L1_LTE_GSM:
                  {
                    // Ensure the L1 stop reason is 'RAT change' and not 'Deep Sleep'
                    rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);

                    rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
                    new_handover_event = deactivate_during_x2g_handover(gas_id);
                    rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;

                    /* No need to log the channel configuration after W2G HO
                    failure as there is no old configuration available. */

                    break;
                  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                  default:
                  {
                    MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                  }
                }
              }
            }
          }
#ifdef FEATURE_WCDMA
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_WCDMA*/
#ifdef FEATURE_GSM_TDS
          else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
          else if (message_set == MS_MSGR_RR && new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
          {
            MSG_GERAN_HIGH_0_G("Received abort from LTE RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_LTE */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            ERR("Received Abort Req from MM",0,0,0);
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#ifdef FEATURE_SGLTE
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_DEACT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Deact rcvd abort the ongoing L2g Ho process");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_SGLTE*/
          else
          {
            ERR("Unexpected message (%d,%d) in AWAITING_HANDOVER_INDICATION",(int) message_set, (int) message_id,0);
          }
        } /* (handover_event == EV_INPUT_MESSAGE) */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in AWAITING_HANDOVER_INDICATION");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }
        break;

    /****************************************************************/
    /* STATE: AWAITING_RANDOM_ACCESS_CONFIRMATION                  */
    /* In this state we are waiting for Random Access Confirmation */
    /* From Layer 1.                                               */
    /***************************************************************/

    case AWAITING_RANDOM_ACCESS_CONFIRMATION :

        /*************************************************************/
        /* Here is the confirmation that the first random access has */
        /* been sent                                                 */
        /*************************************************************/

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if (message_set == MS_RR_L2)
          {
            if (message_id == (byte) DL_RANDOM_ACCESS_CNF)
            {
              if ((handover_details_ptr->synchronisation_indication ==
                   SYNCHRONISED) ||
                  (handover_details_ptr->synchronisation_indication ==
                   PRE_SYNCHRONISED) ||
                  (handover_details_ptr->synchronisation_indication ==
                   PSEUDO_SYNCHRONISED))
              {
                rr_inter_ho_data_ptr->handover_state = AWAITING_HANDOVER_CONFIRM;
              }
              else
              {
                /***********************/
                /* Set the timer T3124 */
                /***********************/
                if (dedic_ptr->curr_channel_spec.
                    channel_info_1_after.channel_type == SDCCH_4 ||
                    dedic_ptr->curr_channel_spec.
                    channel_info_1_after.channel_type == SDCCH_8)
                {
                  /*****************************************/
                  /* The main dedicated channel is a SDCCH */
                  /*****************************************/
                  MSG_GERAN_HIGH_1_G("Starting T3124(%d) SDCCH",(int)T3124_DURATION_SDCCH);
                  (void)rr_timer_start(T3124, (dword)T3124_DURATION_SDCCH, gas_id);
                }
                else
                {
                  /**********************************************/
                  /* The main dedicated channel is not an SDCCH */
                  /**********************************************/

                  MSG_GERAN_HIGH_1_G("Starting T3124(%d) TCH",(int)T3124_DURATION_OTHER);
                  (void)rr_timer_start(T3124, (dword)T3124_DURATION_OTHER, gas_id);
                }
                rr_inter_ho_data_ptr->handover_state = AWAITING_PHYSICAL_INFORMATION;
              }
            }
            else if ((message_id == (byte) DL_RELEASE_IND) ||
                     (message_id == (byte) MDL_ERROR_IND))
            {
              /* log the handover failure */
              rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

              switch (handover_details_ptr->handover_type)
              {
                case RR_L1_GSM_GSM:
                {
                  rr_send_mph_reconnect_channel_req(gas_id);
                  /* set the handover cause here */
                  rr_inter_ho_data_ptr->handover_cause = RR_LOWER_LAYER_FAILURE;
                  rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;
                  break;
                }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_L1_WCDMA_GSM:
                case RR_L1_TDS_GSM:
                case RR_L1_LTE_GSM:
                {
                  rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
                  new_handover_event = deactivate_during_x2g_handover(gas_id);
                  rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
                  break;
                }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                default:
                {
                  MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                }
              }
            }
          } /* MS_RR_L2 */
#ifdef FEATURE_WCDMA
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
          else if (message_set == MS_MSGR_RR && new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
          {
            MSG_GERAN_HIGH_0_G("Received abort from LTE RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_LTE */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            ERR("Received Abort Req from MM",0,0,0);
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#ifdef FEATURE_SGLTE
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_DEACT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Deact rcvd abort the ongoing L2g Ho process");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_SGLTE*/
          else
          {
            ERR("Unexpected message (%d,%d) in AWAITING_RANDOM_ACCESS_CONFIRMATION",(int) message_set, (int) message_id,0);
          }
        }

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in AWAITING_RANDOM_ACCESS_CONFIRMATION");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }
        break;

      /****************************************************************/
      /* STATE: AWAITING_HANDOVER_CONFIRM                            */
      /* In this state we are waiting for Handover Confirmation      */
      /* From Layer 1.                                               */
      /***************************************************************/

      case AWAITING_HANDOVER_CONFIRM :

        /**************************************************************/
        /* This is the confirmation that the 4 access bursts with     */
        /* the included random reference have been sent other the air */
        /**************************************************************/

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if (message_set == MS_RR_L1)
          {
            if (message_id == (byte) MPH_HANDOVER_CNF)
            {
              mph_handover_cnf_T *mph_handover_cnf;

              mph_handover_cnf = (mph_handover_cnf_T *) new_message;

              if (mph_handover_cnf->error_code == NO_ERROR)
              {
                /* preserve last TA and OTD for all types of Handovers */
                rr_inter_ho_data_ptr->last_timing_advance = mph_handover_cnf->last_timing_advance;

                rr_inter_ho_data_ptr->observed_time_difference =
                  mph_handover_cnf->observed_time_difference[0]* 256 * 256 +
                  mph_handover_cnf->observed_time_difference[1]* 256 +
                  mph_handover_cnf->observed_time_difference[2];

                switch (handover_details_ptr->handover_type)
                {
                  case RR_L1_GSM_GSM:
                  {
                    if ((handover_details_ptr->BCCH_ARFCN.num !=
                      scell_info_ptr->gsm.BCCH_ARFCN.num) ||
                      (handover_details_ptr->BSIC.PLMN_colour_code !=
                      scell_info_ptr->gsm.BSIC.PLMN_colour_code) ||
                      (handover_details_ptr->BSIC.BS_colour_code !=
                      scell_info_ptr->gsm.BSIC.BS_colour_code) ||
                      !rr_bands_equal(rr_inter_ho_data_ptr->old_band, rr_get_current_dedicated_band(gas_id)))
                    {
                      MSG_GERAN_MED_0_G("Clearing SACCH SIs, invalidate_measurement_reports = TRUE");
                      rr_clear_si_sacch(gas_id);
                      rr_ded_set_measurement_reports_invalid(TRUE, gas_id);
                    }

                    /* L2 needs to be Resumed to set-up SAPI link   */
                    /* rr_complete_handover will kick L2 to do this */
                    /* Send Sync Indication to MM After L2 CONF     */
                    rr_complete_handover(handover_details_ptr, gas_id);
                    rr_inter_ho_data_ptr->handover_state = AWAITING_L2_RESUME_CONFIRM;
                    break;
                  }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                  case RR_L1_WCDMA_GSM:
                  case RR_L1_TDS_GSM:
                  case RR_L1_LTE_GSM:
                  {
                    // Send InterRAT Handover Success to RRC
                    // other two params should be ignored by RRC.

                    MSG_GERAN_MED_0_G("Clearing SACCH SIs, invalidate_measurement_reports = TRUE");
                    rr_clear_si_sacch(gas_id);
                    rr_ded_set_measurement_reports_invalid(TRUE, gas_id);

                    rr_complete_handover(handover_details_ptr, gas_id);
                    rr_inter_ho_data_ptr->handover_state = AWAITING_L2_RESUME_CONFIRM;
                    break;
                  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                  default:
                  {
                    ERR("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type,0,0);
                    break;
                  }
                }
              }
              else /* ( mph_handover_cnf->error_code == NO_ERROR ) */
              {
                /* log the handover failure */
                rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

                switch (handover_details_ptr->handover_type)
                {
                  case RR_L1_GSM_GSM:
                  {
                    rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;

                    if (mph_handover_cnf->error_code == HANDOVER_OUT_OF_RANGE)
                    {
                      rr_inter_ho_data_ptr->handover_cause = RR_HANDOVER_IMPOSSIBLE_TA_OUT_OF_RANGE;
                    }
                    else
                    {
                      rr_inter_ho_data_ptr->handover_cause = RR_LOWER_LAYER_FAILURE;
                    }

                    rr_send_mph_reconnect_channel_req(gas_id);
                    break;
                  }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                  case RR_L1_WCDMA_GSM:
                  case RR_L1_TDS_GSM:
                  case RR_L1_LTE_GSM:
                  {
                    rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
                    new_handover_event = deactivate_during_x2g_handover(gas_id);
                    rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
                    break;
                  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                  default:
                  {
                    MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                    break;
                  }
                }
              }
            }
          }

          else if (message_set == MS_RR_L2)
          {
            if ((message_id == (byte) DL_RELEASE_IND) ||
                (message_id == (byte) MDL_ERROR_IND))
            {
              rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

              switch (handover_details_ptr->handover_type)
              {
                case RR_L1_GSM_GSM:
                {
                  /* Set the handover cause here */
                  rr_inter_ho_data_ptr->handover_cause = RR_LOWER_LAYER_FAILURE;
                  rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;

                  rr_send_mph_reconnect_channel_req(gas_id);
                  break;
                }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_L1_WCDMA_GSM:
                case RR_L1_TDS_GSM:
                case RR_L1_LTE_GSM:
                {
                  rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
                  new_handover_event = deactivate_during_x2g_handover(gas_id);
                  rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
                  break;
                }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                default:
                {
                  MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                  break;
                }
              }
            }
          }
#ifdef FEATURE_WCDMA
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
          else if (message_set == MS_MSGR_RR && new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
          {
            MSG_GERAN_HIGH_0_G("Received abort from LTE RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_LTE */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            ERR("Received Abort Req from MM",0,0,0);
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#ifdef FEATURE_SGLTE
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_DEACT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Deact rcvd abort the ongoing L2g Ho process");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_SGLTE*/
          else
          {
            ERR("Unexpected message (%d,%d) in AWAITING_HANDOVER_CONFIRM",(int) message_set, (int) message_id,0);
          }

        } /* EV_INPUT_MESSAGE */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in AWAITING_HANDOVER_CONFIRM");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }
        break;



     /****************************************************************/
     /* STATE: AWAITING_L2_RESUME_CONFIRM                            */
     /* Wait for L2 Resume Confirm before Informing NAS and/or RRC   */
     /****************************************************************/
     case AWAITING_L2_RESUME_CONFIRM:

       MSG_GERAN_HIGH_0_G("State: AWAITING_L2_RESUME_CONFIRM");

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          /***********************************************************/
          /* The link layer has been properly resumed. The HANDOVER  */
          /* COMPLETE will be sent in a acknowledge mode             */
          /***********************************************************/
          if (message_set == MS_RR_L2)
          {
            if (message_id == (byte) DL_RESUME_CNF)
            {
              /*****************************************/
              /* Copy the CA from handover details to  */
              /* system_information_database, if valid */
              /*****************************************/

              if (handover_details_ptr->valid_CA_received == TRUE)
              {
                scell_info_ptr->gsm.valid_CA_received = RR_CA_RECEIVED;
                scell_info_ptr->gsm.cell_channel_description =
                handover_details_ptr->cell_channel_description;
              }
              else
              {
                scell_info_ptr->gsm.valid_CA_received = RR_CA_NOT_RECEIVED;
              }
              rr_set_link_status(SAPI0_LINK, gas_id);

              /* Inform NAS w/ a SYNC Indication */
              switch (handover_details_ptr->handover_type)
              {
                case RR_L1_GSM_GSM:
                {
                  /***********************************************************************/
                  /* Vocoder control related changes, send the Sync_ind to MM            */
                  /***********************************************************************/
                  rr_send_handover_sync_ind(dedic_ptr, gas_id);

                  /* Revert the DTX indication to match that broadcast by the network */
                  /* (This will take effect on the next reception of SI5/6) */
                  rr_dtx_rr_override_off(gas_id);

                  rr_inter_ho_data_ptr->handover_state = AWAITING_HANDOVER_COMPLETION;
                  new_handover_event = EV_HANDOVER_COMPLETED;
                  break;
                }
#ifdef FEATURE_WCDMA
                case RR_L1_WCDMA_GSM:
                {
                  /* Send InterRAT Handover Success to RRC */
                  /* other two params should be ignored by RRC */
                  rrc_rr_gsm_command_list_type wcdma_gsm_cmd_list;
      
                  rr_convert_x2g_ho_gsm_cmds_to_wcdma(
                    &rr_inter_ho_data_ptr->gsm_command_list,
                    &wcdma_gsm_cmd_list
                  );
      
                  rr_send_interrat_ho_cnf(
                    RRC_RR_SUCCESS,
                    rr_convert_rr_x2g_ho_failure_cause_to_wcdma(rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason),
                    &wcdma_gsm_cmd_list,
                    gas_id
                  );

                  rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_PROCESS_NAS_DATA;
                  break;
                }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
                case RR_L1_TDS_GSM:
                {
                  tdsrrc_rr_gsm_command_list_type tds_gsm_cmd_list;

                  rr_convert_x2g_ho_gsm_cmds_to_tds(
                    &rr_inter_ho_data_ptr->gsm_command_list,
                    &tds_gsm_cmd_list
                  );

                  rr_send_tds_interrat_ho_cnf(
                    TDSRRC_RR_SUCCESS,
                    rr_convert_rr_x2g_ho_failure_cause_to_tds(rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason),
                    &tds_gsm_cmd_list,
                    gas_id
                  );

                  rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_PROCESS_NAS_DATA;
                  break;
                }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
                case RR_L1_LTE_GSM:
                {
                  /* LTE RRC does not buffer NAS data during handover. */
#ifdef FEATURE_SGLTE
                  if((rr_is_sglte_mode_active(gas_id)) && (rr_sglte_saved_l2g_ho_being_processed(gas_id)))
                  {
                    rr_send_activation_ind(MM_AS_LTOG_SGLTE_SRVCC_CS_HANDOVER, gas_id);
                  }
                  else
#endif /*FEATURE_SGLTE*/
                  {
                    rr_send_activation_ind(MM_AS_LTOG_SRVCC_CS_HANDOVER, gas_id);
                  }

				  /*******************************************************************************/
				  /* Vocoder control related changes, send the Sync_ind and HO complete before 	 */
				  /* RR_ACTIVATION_RSP is received for SRVCC case						*/
				  /*******************************************************************************/
                  MSG_GERAN_HIGH_0_G("Send Sync ind and Ho cmplt before act_rsp rcvd for L2G SRVCC");					  
                  rr_send_sync_ind(
		            MM_RR_WCDMA_GSM_HO,
		     	    dedic_ptr->curr_channel_spec.channel_info_1_after.channel_type,
		            dedic_ptr->curr_channel_spec.channel_mode_1,
		            dedic_ptr->curr_channel_spec.channel_info_1_after.subchannel,
		            gas_id
		          );
                  rr_eng_mode_update_codec_ho_info(
                    dedic_ptr->prev_channel_spec.channel_mode_1,
                    handover_details_ptr->channel_spec.channel_mode_1,
                    handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
                    TRUE,
                    SYS_HO_COMPLETE,
                    rr_map_rr_l1_handover_type_to_sys_ho_type(handover_details_ptr->handover_type), 
                    gas_id
                  );
                  rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP;
                  break;
                }
#endif /* FEATURE_LTE */
                default:
                {
                  MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                  break;
                }
              }
            }/* DL_RESUME_CNF */

            else if (message_id == (byte) MDL_ERROR_IND)
            {
              mdl_error_ind_T *mdl_error_ind = (mdl_error_ind_T *) new_message;

              rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

              if (mdl_error_ind->error_code == UNSOLICITED_DM_RESPONSE_PAR ||
                  mdl_error_ind->error_code == N200_EXCEEDED_PAR           ||
                  mdl_error_ind->error_code == SEQUENCE_ERROR)
              {
                rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
                proc_dispatch_event = EV_RADIO_LINK_FAILURE;
              }
              else
              {
                switch (handover_details_ptr->handover_type)
                {
                  case RR_L1_GSM_GSM:
                  {
                    rr_send_mph_reconnect_channel_req(gas_id);
                    /* Set the handover cause here */
                    rr_inter_ho_data_ptr->handover_cause = RR_LOWER_LAYER_FAILURE;
                    rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;
                    break;
                  }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                  case RR_L1_WCDMA_GSM:
                  case RR_L1_TDS_GSM:
                  case RR_L1_LTE_GSM:
                  {
                    // Deactivate GSM RAT and RRC will be informed in RR_W2G_HO_GOING_INACTIVE.
                    MSG_GERAN_HIGH_0_G("L2_RESUME error while waiting for DL_RESUME_CNF");
                    rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
                    new_handover_event = deactivate_during_x2g_handover(gas_id);
                    rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
                    break;
                  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                  default:
                  {
                    MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                    break;
                  }
                }
              }
            }

            else if (message_id == (byte) DL_RELEASE_IND)
            {
              rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

              switch (handover_details_ptr->handover_type)
              {
                case RR_L1_GSM_GSM:
                {
                  rr_send_mph_reconnect_channel_req(gas_id);
                  /* Set the handover cause here */
                  rr_inter_ho_data_ptr->handover_cause = RR_LOWER_LAYER_FAILURE;
                  rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;
                  break;
                }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_L1_WCDMA_GSM:
                case RR_L1_TDS_GSM:
                case RR_L1_LTE_GSM:
                {
                  // Deactivate GSM RAT and RRC will be informed in RR_W2G_HO_GOING_INACTIVE.
                  MSG_GERAN_HIGH_0_G("L2_RESUME error while waiting for DL_RESUME_CNF");
                  rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
                  new_handover_event = deactivate_during_x2g_handover(gas_id);
                  rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
                  break;
                }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                default:
                {
                  MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                  break;
                }
              }
            } /* message_id == DL_RELEASE_IND || message_id == MDL_ERROR_IND */
          }   /* Message set = MS_RR_L2 */
#ifdef FEATURE_WCDMA
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
          {
             MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
             new_handover_event = deactivate_during_x2g_handover(gas_id);
             rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
          {
             MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
             new_handover_event = deactivate_during_x2g_handover(gas_id);
             rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
          else if (message_set == MS_MSGR_RR && new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
          {
            MSG_GERAN_HIGH_0_G("Received abort from LTE RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_LTE */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
             MSG_GERAN_HIGH_0_G("RR Received ABORT_REQ from MM");
             // No Handover Cnf will be sent to RRC in this case ?
             new_handover_event = deactivate_during_x2g_handover(gas_id);
             rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#ifdef FEATURE_SGLTE
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_DEACT_REQ))
          {
             MSG_GERAN_HIGH_0_G("Deact rcvd abort the ongoing L2g Ho process");
             new_handover_event = deactivate_during_x2g_handover(gas_id);
             rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_SGLTE*/
          else
          {
            ERR("Unexpected message (%d,%d) in AWAITING_L2_RESUME_CONFIRM",(int) message_set, (int) message_id,0);
          }

        } /* EV_INPUT_MESSAGE */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in AWAITING_L2_RESUME_CONFIRM");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }

        break;


      /***************************************************************/
      /* STATE: AWAITING_PHYSICAL_INFORMATION                        */
      /* In this state we are waiting for physical information       */
      /* From Layer 2.                                               */
      /***************************************************************/

      case AWAITING_PHYSICAL_INFORMATION :

        /***************************************************************/
        /* The PHYSICAL INFORMATION has been received from the network */
        /***************************************************************/

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if (message_set == MS_RR_L2)
          {
            if (message_id == (byte) DL_UNIT_DATA_IND)
            {
              dl_unit_data_ind_T *dl_unit_data_ind;

              dl_unit_data_ind = (dl_unit_data_ind_T *) new_message;

              if (dl_unit_data_ind->layer3_message[1] == PHYSICAL_INFORMATION)
              {
                byte timing_advance;

                timing_advance = dl_unit_data_ind->layer3_message[2];
                rr_send_mph_stop_handover_access_req(timing_advance, gas_id);

                MSG_GERAN_HIGH_0_G("Stopping T3124");
                (void)rr_timer_cancel(T3124, gas_id);

                rr_inter_ho_data_ptr->handover_state = AWAITING_STOP_CONFIRMATION;
              }
            }
            else if ((message_id == (byte) DL_RELEASE_IND) ||
                     (message_id == (byte) MDL_ERROR_IND))
            {
              /* log the handover failure */
              rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

              switch (handover_details_ptr->handover_type)
              {
                case RR_L1_GSM_GSM:
                {
                  MSG_GERAN_HIGH_0_G("Stopping T3124");
                  (void)rr_timer_cancel(T3124, gas_id);
                  rr_send_mph_reconnect_channel_req(gas_id);
                  /* Set the handover cause here */
                  rr_inter_ho_data_ptr->handover_cause = RR_LOWER_LAYER_FAILURE;
                  rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;
                  break;
                }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_L1_WCDMA_GSM:
                case RR_L1_TDS_GSM:
                case RR_L1_LTE_GSM:
                {
                  rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
                  new_handover_event = deactivate_during_x2g_handover(gas_id);
                  rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
                  break;
                }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                default:
                {
                  MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                  break;
                }
              }
            }
          }
#ifdef FEATURE_WCDMA
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /*FEATURE_GSM_TDS*/
#ifdef FEATURE_LTE
          else if (message_set == MS_MSGR_RR && new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
          {
            MSG_GERAN_HIGH_0_G("Received abort from LTE RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_LTE */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            ERR("Received Abort Req from MM",0,0,0);
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
          else
          {
            ERR("Unexpected message (%d,%d) in AWAITING_PHYSICAL_INFORMATION",(int) message_set, (int) message_id,0);
          }
        }
        else if (handover_event == EV_T3124_TIMEOUT)
        {
          /* log the handover failure */
          rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

          switch (handover_details_ptr->handover_type)
          {
            case RR_L1_GSM_GSM:
            {
              MSG_GERAN_HIGH_0_G("T3124 expired");
              rr_send_mph_reconnect_channel_req(gas_id);
              /* set the handover cause here */
              rr_inter_ho_data_ptr->handover_cause = RR_ABNORMAL_RELEASE_TIMER_EXPIRED;
              rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;
              break;
            }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
            case RR_L1_WCDMA_GSM:
            case RR_L1_TDS_GSM:
            case RR_L1_LTE_GSM:
            {
              MSG_GERAN_HIGH_0_G("T3124 expired");
              rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
              new_handover_event = deactivate_during_x2g_handover(gas_id);
              rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
              break;
            }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
            default:
            {
              MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
              break;
            }
          }
        } /* (handover_event == EV_T3124_TIMEOUT) */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in AWAITING_PHYSICAL_INFORMATION");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }
        break;

      /***************************************************************/
      /* STATE: AWAITING_STOP_CONFIRMATION                           */
      /* In this state we are waiting for stop confirmation          */
      /* From Layer 1.                                               */
      /***************************************************************/

      case AWAITING_STOP_CONFIRMATION :

        /******************************************************/
        /* We stopped sending the access bursts other the air */
        /******************************************************/

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if (message_set == MS_RR_L1)
          {
            if (message_id == (byte) MPH_STOP_HANDOVER_ACCESS_CNF)
            {
              mph_stop_handover_access_cnf_T *mph_stop_handover_access_cnf;

              mph_stop_handover_access_cnf =
               (mph_stop_handover_access_cnf_T *) new_message;

              // Need to remember the Last Timing advance
              // and OTD ( Observed Time Difference )
              rr_inter_ho_data_ptr->last_timing_advance = mph_stop_handover_access_cnf->last_timing_advance;

              rr_inter_ho_data_ptr->observed_time_difference =
                mph_stop_handover_access_cnf->observed_time_difference[0] * 256 * 256 +
                mph_stop_handover_access_cnf->observed_time_difference[1] * 256 +
                mph_stop_handover_access_cnf->observed_time_difference[2];

              switch (handover_details_ptr->handover_type)
              {
                case RR_L1_GSM_GSM:
                {

                  mph_stop_handover_access_cnf =
                    (mph_stop_handover_access_cnf_T *) new_message;

                  rr_complete_handover(handover_details_ptr, gas_id);

                  rr_inter_ho_data_ptr->handover_state = AWAITING_L2_RESUME_CONFIRM;
                  break;
                }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_L1_WCDMA_GSM:
                case RR_L1_TDS_GSM:
                case RR_L1_LTE_GSM:
                {
                  rr_complete_handover(handover_details_ptr, gas_id);
                  rr_inter_ho_data_ptr->handover_state = AWAITING_L2_RESUME_CONFIRM;
                  handover_event = EV_NO_EVENT;
                  break;
                }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                default:
                {
                  ERR("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type,0,0);
                  break;
                }
              }
            }
          }
          else if (message_set == MS_RR_L2)
          {
            if ((message_id == (byte) DL_RELEASE_IND) ||
                (message_id == (byte) MDL_ERROR_IND))
            {
              /* log the handover failure */
              rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

              switch (handover_details_ptr->handover_type)
              {
                case RR_L1_GSM_GSM:
                {
                  rr_send_mph_reconnect_channel_req(gas_id);
                  /* Set the handover cause here */
                  rr_inter_ho_data_ptr->handover_cause = RR_LOWER_LAYER_FAILURE;
                  rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;
                  break;
                }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_L1_WCDMA_GSM:
                case RR_L1_TDS_GSM:
                case RR_L1_LTE_GSM:
                {
                  rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason = RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE;
                  new_handover_event = deactivate_during_x2g_handover(gas_id);
                  rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
                  break;
                }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                default:
                {
                  MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                  break;
              }
            }
          }
          }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_ABORT_REQ))
          {
            ERR("RR shouldn't get this message, MM in unexpected state",0,0,0);
            // No Handover Cnf will be sent to RRC in this case
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
#ifdef FEATURE_WCDMA
          else if ((message_set == MS_RRC_RR) && (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          else if ((message_set == MS_TDSRRC_RR) && (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
          {
            MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
          else if (message_set == MS_MSGR_RR && new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
          {
            MSG_GERAN_HIGH_0_G("Received abort from LTE RRC");
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_LTE */
          else
          {
            ERR("Unexpected message (%d,%d) in AWAITING_STOP_CONFIRMATION",(int) message_set, (int) message_id,0);
          }
        } /* EV_INPUT_MESSAGE */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in AWAITING_STOP_CONFIRMATION");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }
        break;

      /***************************************************************/
      /* STATE: AWAITING_HANDOVER_COMPLETION                         */
      /* In this state we are waiting for Handover completion        */
      /* ( dl_resume_cnf) From Layer 2.                              */
      /***************************************************************/

      case AWAITING_HANDOVER_COMPLETION:
        if (handover_event == EV_HANDOVER_COMPLETED)
        {
          /*
          * handover has finally been successful. Now need to
          * check that the BCCH in the handover command is the
          * same as the last one we were camped on. If not, then
          * we need to invalidate the old system information
          * database to ensure that when we finally return
          * to idle, the system information can be checked
          * again.
          * Also need to ensure that we reset the measurement information
          * database at this point so any MI messages received on the new
          * cell will be processed correctly.
          */

          // For GSM to GSM Handovers check if the Cell changed
          switch (handover_details_ptr->handover_type)
          {
            case RR_L1_GSM_GSM:
            {
              if ((handover_details_ptr->BCCH_ARFCN.num !=
                scell_info_ptr->gsm.BCCH_ARFCN.num) ||
                (handover_details_ptr->BSIC.PLMN_colour_code !=
                scell_info_ptr->gsm.BSIC.PLMN_colour_code) ||
                (handover_details_ptr->BSIC.BS_colour_code !=
                scell_info_ptr->gsm.BSIC.BS_colour_code) ||
                !rr_bands_equal(rr_inter_ho_data_ptr->old_band, rr_get_current_dedicated_band(gas_id)))
              {
                /*
                All the system information is now invalid and will have to be
                re-acquired once the CS connection is released.
                This call also stops any sys info acquisition timers, etc.
                */
                /*
                Ensure the system information data is re-initialised and
                setup for correct operation following connection release
                */
                rr_gprs_setup_sys_info_after_handover(
                  handover_details_ptr->BCCH_ARFCN,
                  handover_details_ptr->BSIC,
                  gas_id
                );

                if (!rr_bands_equal(rr_inter_ho_data_ptr->old_band, rr_get_current_dedicated_band(gas_id)))
                {
                  /* If the old band and new band are different, we need to swap
                  the cached SI5 and SI5ter BA lists around.  SI5 always contains
                  in-band neighbour cells, and SI5ter contains the out-of-band
                  neighbour cells, but if we have just changed band then the old
                  SI5 needs to be treated as if it contains out-of-band neighbour
                  cells, and vice-versa.

                  As an example, consider two cells with the following neighbour
                  lists:

                  Cell A (ARFCN 10)  has neighbour list 20, 30, 40, 610, 620, 630, 640
                  Cell B (ARFCN 610) has neighbour list 620, 630, 640, 10, 20, 30, 40

                  For Cell A SI5 contains 20, 30, 40 and SI5ter
                  contains 610, 620, 630, 640.

                  For Cell B SI5 contains 620, 630, 640 and SI5ter
                  contains 10, 20, 30, 40.

                  If SI5 is then received on the new cell (containing in-band cells),
                  we want RR to construct it's BA list to send to L1 based on the new
                  cells SI5 and also the neighbour cells from the old cell that are
                  in a different band to the new cell (i.e. the old cell's SI5
                  neighbours).

                  For simplicity, BA list construction code always uses SI5/5ter from
                  the new cell if they are available, and if not it will access
                  the old cell SI5 and/or SI5ter as necessary.  It is simplest in the
                  case of a band change to switch the old cell's SI5/5ter lists here
                  rather than having to make a decision at the time of BA list
                  construction.

                  So, on receipt of SI5 on the new cell RR will construct a BA list
                  based on SI5 from the new cell: 620, 630, 640
                  and what is now called SI5 for the old cell (but was actually
                  broadcast as SI5ter): 20, 30, 40
                  Therefore the complete BA list sent to L1 would be:

                  620, 630, 640, 20, 30, 40

                  The purpose of this mechanism is to keep L1 synced to as many
                  neighbour cells as possible, as there is likely to be
                  significant overlap between neighbouring cells' BA lists.
                  Keeping L1 synced to these cells until such a time as we receive
                  the complete BA list of the new cell means that we can start
                  Measurement Reporting sooner on the new cell.
                  */
                  rr_swap_old_si5_and_si5ter_ba_lists(gas_id);
                }
              }
              else
              {
                /* No change to ARFCN/frequency band, so system information
                may be preserved. */
                MSG_GERAN_MED_0_G("ARFCN/Frequency band not changed after HANDOVER_COMMAND, preserving SACCH system information");

                /*
                Need to ensure that we send an MPH_DEDICATED_UPDATE_REQ
                (based on the old values) to L1 anyway this triggers it
                to en/disable DTX in the vocoder.
                */
                rr_l1_update_dedicated_mode(
                  &scell_info_ptr->gsm,
                  gas_id
                );
              }
              proc_dispatch_event = EV_HANDOVER_TERMINATED;
              break;
            } /* For InterRAT Handovers No Need to check */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
            case RR_L1_WCDMA_GSM:
            case RR_L1_TDS_GSM:
            case RR_L1_LTE_GSM:
            {
              /*
              Ensure the system information data is setup for correct operation
              following connection release
              */
              rr_gprs_setup_sys_info_after_handover(
                handover_details_ptr->BCCH_ARFCN,
                handover_details_ptr->BSIC,
                gas_id
              );

              /*
              Assume a MSCR of R99. There will probably be a Classmark Query received,
              and if the UE is set to 'Dynamic' R97/R99 mode, then the MSCR value
              stored in system_information_database_ptr->MSCR is queried to determine
              whether the UE advertises R99 support or not. R99 support should always
              be assumed where there is a W-RAT
              */
              scell_info_ptr->gsm.control_channel_description.MSCR = (uint8) MSCR_REV_99;

#ifdef FEATURE_SGLTE
              if( rr_is_sglte_mode_active(gas_id) && 
                  (ARFCNS_EQUAL(handover_details_ptr->BCCH_ARFCN,rr_sglte_get_stored_serv_ARFCN(gas_id))) && 
                   rr_sglte_saved_l2g_ho_being_processed(gas_id)
                )  
              {
                MSG_GERAN_HIGH_0_G("SGLTE SRVCC on same cell, set ded mode param to SI2q"); 
                rr_dedicated_mode_process_initial_params(scell_info_ptr,rr_get_instances_ptr(SI2Q_INSTANCE,gas_id),gas_id);
              }
              else
 #endif /*FEATURE_SGLTE*/
              {
                MSG_GERAN_HIGH_0_G("Set ded mode param to default value"); 
                rr_dedicated_mode_process_initial_params(scell_info_ptr, NULL,gas_id);

                /**
                 * Support of GPRS in the cell is not known at this point. However, it is reasonable to assume
                 * for an IRAT-capable network that GPRS would be supported, so make this the default 
                 * assumption rather than assume GPRS is not possible. 
                 * If GPRS is indicated as not supported, this could cause a local PS deregistration in NAS, 
                 * which is not desirable if GPRS is in fact supported. 
                 * Once the CS connection is released and SIs are read, then the correct indication will be 
                 * given in the RR_SERVICE_IND. 
                 */
                scell_info_ptr->gsm.gprs_indicator = TRUE;
              }

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
              if (handover_details_ptr->handover_type == RR_L1_TDS_GSM)
              {
                rr_pseudo_fr_info_t *pfr_store_ptr = &dedic_ptr->tds_pseudo_fr;

                if (rr_inter_ho_data_ptr->tds_pseudo_fr_active)
                {
                  tdsrr_ho_pseudo_fast_return_type *pseudo_fr_ptr = &rr_inter_ho_data_ptr->tds_pseudo_fr_info;
                  uint8 i = 0;

                  rr_l1_send_mph_ded_wcdma_cell_upd_list_req_for_pseudo_fr(
                    pseudo_fr_ptr,
                    gas_id
                  );

                  /*save Pseudo FR info for G->T redirection decision when releasing the call*/
                  rr_tds_pseudo_fr_reset_dedicated_info(gas_id);
                  pfr_store_ptr->active = TRUE;

                  /*Supplied RSCP range 0..90.  Maps to -115dBm .. -25dBm.  3Gpp TS23-331 section 10.3.7.19 */
                  pfr_store_ptr->rscp_threshold = ((int16)pseudo_fr_ptr->rscp_threshold) - 115;
                  pfr_store_ptr->num_freq = pseudo_fr_ptr->num_frequencies;

                  MSG_GERAN_MED_2_G("RR TDS Pseudo FR num requested freq:%d, RSCP threshold: %ddBm",pfr_store_ptr->num_freq,pfr_store_ptr->rscp_threshold);

                  for (i=0; i<pfr_store_ptr->num_freq; i++)
                  {
                     pfr_store_ptr->freq_list[i].measured = FALSE;
                     pfr_store_ptr->freq_list[i].uarfcn = pseudo_fr_ptr->frequency_list[i];
                     MSG_GERAN_MED_1_G("RR TDS Pseudo FR UARFCN %d",pfr_store_ptr->freq_list[i].uarfcn);
                  }
                }
                else
                {
                  pfr_store_ptr->active = FALSE;
                }
              }
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

              if (rr_inter_ho_data_ptr->panic_reset_pending)
              {
                MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Process buffered Panic Reset in AWAITING_HANDOVER_COMPLETION");
                proc_dispatch_event = EV_PANIC_RESET_CNF;
              }
              else
              {
                proc_dispatch_event = RR_EV_W2G_HO_COMPLETED;
              }
              break;
            }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
            default:
            {
              MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
              break;
            }
          }

          /* Reset the S_counter to RADIO_LINK_TIMEOUT */
          rr_reset_S_counter(gas_id);

          {
            boolean irat_ho = FALSE;

            switch (handover_details_ptr->handover_type)
            {
              case RR_L1_GSM_GSM:
                irat_ho = FALSE;
              break;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
              case RR_L1_WCDMA_GSM:
              case RR_L1_TDS_GSM:
              case RR_L1_LTE_GSM:
                irat_ho = TRUE;
              break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

              default:
                irat_ho = FALSE;
              break;
            }

#ifdef FEATURE_GSM_BAND_AVOIDANCE
            if (rr_channel_mode_is_voice(handover_details_ptr->channel_spec.channel_mode_1))
            {
              rr_band_avoid_ded_freq_upd_send_imsg(&handover_details_ptr->channel_spec.channel_info_1_after.frequency_list, gas_id);
            }
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

            if (handover_details_ptr->handover_type != RR_L1_LTE_GSM)
            {
              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->prev_channel_spec.channel_mode_1,
                handover_details_ptr->channel_spec.channel_mode_1,
                handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
                irat_ho,
                SYS_HO_COMPLETE,
                rr_map_rr_l1_handover_type_to_sys_ho_type(handover_details_ptr->handover_type), 
                gas_id
              );
            }
            else
            {
              MSG_GERAN_HIGH_1_G("No HO complete to CM for handover type = %d ,after Act_rsp",handover_details_ptr->handover_type);
            }
          }

#ifdef FEATURE_LTE
          if((handover_details_ptr->handover_type == RR_L1_LTE_GSM) && (RR_EV_W2G_HO_COMPLETED == proc_dispatch_event) )
          {
            if(rr_g2l_blind_redir_after_srvcc_enabled(gas_id))
            {
              rr_connection_release_set_g2l_blind_redir_flag(gas_id);
              MSG_GERAN_HIGH_0_G("SRVCC Handover completed G2L blind redirection will performed after release");
            }
            else
            {
              MSG_GERAN_HIGH_0_G("SRVCC Handover completed G2L blind redirection not enabled");
            }
          }
#endif /* FEATURE_LTE */

          rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
          rr_log_handover_success(gas_id);
          MSG_GERAN_HIGH_0_G("Handover completed successfully");
          rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
          (void)rr_timer_start(SIGNAL_STRENGTH_TIMER,SIGNAL_STRENGTH_TIMEOUT, gas_id);
        } // handover_event == EV_HANDOVER_COMPLETED

        else if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if (message_set == MS_RR_L2)
          {
            if ((message_id == (byte) DL_RELEASE_IND) || (message_id == (byte) MDL_ERROR_IND))
            {
              rr_log_handover_failure(RR_LOG_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE, gas_id);

              switch (handover_details_ptr->handover_type)
              {
                case RR_L1_GSM_GSM:
                {
                  rr_send_mph_reconnect_channel_req(gas_id);
                  /* Set the handover cause here */
                  rr_inter_ho_data_ptr->handover_cause = RR_LOWER_LAYER_FAILURE;
                  rr_inter_ho_data_ptr->handover_state = HO_AWAITING_PHYSICAL_RECONNECTION;
                  break;
                }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                case RR_L1_WCDMA_GSM:
                case RR_L1_TDS_GSM:
                case RR_L1_LTE_GSM:
                {
                  // It is too late to inform RRC since GSM RAT is already activated.
                  // So treat it as a Radio Link failure in Data Transfer State.

                  MSG_GERAN_HIGH_0_G("L2 error while waiting for DL_RESUME_CNF");
                  MSG_GERAN_HIGH_0_G("Since can't reconnect (Inter RAT HO) so declaring RLF, ");
                  proc_dispatch_event = EV_RADIO_LINK_FAILURE;
                  rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
                  rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
                  break;
                }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                default:
                {
                  MSG_GERAN_ERROR_1_G("Invalid Handover Type = %d",(int)handover_details_ptr->handover_type);
                  break;
                }
              }
            } /* message_id == DL_RELEASE_IND || message_id == MDL_ERROR_IND */
          } /* message_set == MS_RR_L2 */
          else
          {
            ERR("Unexpected message (%d,%d) in AWAITING_HANDOVER_COMPLETION",(int) message_set, (int) message_id,0);
          }
        } /* EV_INPUT_MESSAGE */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in AWAITING_HANDOVER_COMPLETION");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }

        break;

      /***************************************************************/
      /* STATE: HO_AWAITING_PHYSICAL_RECONNECTION                    */
      /* In this state we are waiting for Channel Reconnect cnf      */
      /* From Layer 1.                                               */
      /***************************************************************/

      case HO_AWAITING_PHYSICAL_RECONNECTION :

        /***********************************************************/
        /* The old channels have been reactivated : We are able to */
        /* send the HANDOVER FAILURE message                       */
        /***********************************************************/

        /* ASSERT(handover_details_ptr->handover_type == RR_L1_GSM_GSM ||
                  handover_details_ptr->handover_type == RR_L1_GSM_WCDMA); */
        if ( (handover_details_ptr->handover_type != RR_L1_GSM_GSM) &&
               (handover_details_ptr->handover_type != RR_L1_GSM_WCDMA))
        {
          MSG_GERAN_ERROR_0_G("Unknown handover_type!!!");
        }

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if ( message_set == MS_RR_L1)
          {
            /*
            Ensure the L1 idle mode is set to something other than 'NONE'.
            If there have been previous handovers, then the true cell capabilities may not be known, so the
            correct value can't be determined.
            But in the case of failed G2W HO, this mode will have been reset to 'NONE' when L1 was stopped.
            (For G-G & W-G HOs this is done in rr_gprs_setup_sys_info_after_handover()[rr_sys_info_main.c])
            */
            scell_info_ptr->l1_idle_mode = GRR_L1_IDLE_MODE_GSM;

            if (message_id == (byte) MPH_RECONNECT_CHANNEL_CNF)
            {
              /**
               * Clear the SACCH system information as some may have been
               * received on the new cell which will not be valid for the
               * old cell.
               */
              rr_clear_si_sacch(gas_id);

              /* Restore previously-saved channel spec and AMR config */
              rr_restore_previous_dedicated_config(gas_id);

#ifdef FEATURE_WCDMA
              if (handover_details_ptr->handover_type == RR_L1_GSM_WCDMA)
              {
                /* translate the failure cause set by the G2W module */
                rr_inter_ho_data_ptr->handover_cause = rr_g2w_map_handover_failure_cause(gas_id);

                if (!rr_gsm_only(gas_id))
                {

                  if (scell_info_ptr->gsm.wcdma_ded_neighbor_list.valid_data)
                  {
                    rr_send_mph_dedicated_wcdma_cell_update_list_req(
                      NULL,                                           // wcdma_idle_neighbor_list
                      &scell_info_ptr->gsm.wcdma_ded_neighbor_list,   // wcdma_ded_neighbor_list
                      scell_info_ptr->gsm.dedicated_mode_params,
                      gas_id                                          // gas_id
                    );
                  }
                  else if (scell_info_ptr->gsm.utran_neighbor_list.valid_data == TRUE)
                  {
                    rr_send_mph_dedicated_wcdma_cell_update_list_req(
                      &scell_info_ptr->gsm.utran_neighbor_list,   // wcdma_idle_neighbor_list
                      NULL,                                       // wcdma_ded_neighbor_list
                      scell_info_ptr->gsm.dedicated_mode_params,
                      gas_id                                      // gas_id
                    );
                  }
                  else
                  {
                    rr_send_mph_dedicated_wcdma_cell_update_list_req(
                      NULL,     // wcdma_idle_neighbor_list
                      NULL,     // wcdma_ded_neighbor_list
                      scell_info_ptr->gsm.dedicated_mode_params,
                      gas_id    // gas_id
                    );
                  }
                }
                else
                {
                  /* if in GSM only mode flush the list at L1 */
                  rr_send_mph_dedicated_wcdma_cell_update_list_req(
                    NULL,     // wcdma_idle_neighbor_list
                    NULL,     // wcdma_ded_neighbor_list
                    NULL,
                    gas_id    // gas_id
                  );
                }
              } /* if (handover_details_ptr->handover_type == RR_L1_GSM_WCDMA) */
#endif /* FEATURE_WCDMA */

              /* when returning to a 2G cell, cached sys info can be reloaded */
              rr_restore_dedicated_sys_info(
                scell_info_ptr->gsm.BCCH_ARFCN,
                gas_id
              );

              rr_perform_handover_failure(
                rr_inter_ho_data_ptr->handover_cause,   // RR_cause
                TRUE,                                   // reconnect
                gas_id                                  // gas_id
              );

              /* the SAPI0 link is now in progress */
              rr_set_link_status(SAPI0_LINK_IN_PROGRESS, gas_id);

              /* reset the cause */
              rr_inter_ho_data_ptr->handover_cause = RR_PROTOCOL_ERROR_UNSPECIFIED;

              rr_inter_ho_data_ptr->handover_state = HO_AWAITING_RECONNECTION;

              /* log the new channel configuration */
              (void)rr_log_channel_configuration(gas_id);
            }
          }
          else if (message_set == MS_RR_L2)
          {
            if ((message_id == (byte) DL_RELEASE_IND) ||
                (message_id == (byte) MDL_ERROR_IND))
            {
              // A L2 failure has occurred while waiting for L1 reconnection - release the connection
              rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
              proc_dispatch_event = EV_RADIO_LINK_FAILURE;
            }
          }
        } /* EV_INPUT_MESSAGE */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in HO_AWAITING_PHYSICAL_RECONNECTION");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }
        break;

      /***************************************************************/
      /* STATE: HO_AWAITING_RECONNECTION                             */
      /* In this state we are waiting for Reconnect cnf              */
      /* From Layer 2.                                               */
      /***************************************************************/

      case HO_AWAITING_RECONNECTION :

        /*************************************************************/
        /* The reconnection of the link at the layer 2 has well been */
        /* performed.                                                */
        /* We are able to go back to the INACTIVE state. More over,  */
        /* we have to generate an event which has to tell the        */
        /* handover dispatcher that the handover procedure is        */
        /* terminated                                                */
        /*************************************************************/
        /* ASSERT(handover_details_ptr->handover_type == RR_L1_GSM_GSM ||
                  handover_details_ptr->handover_type == RR_L1_GSM_WCDMA); */
        if ( (handover_details_ptr->handover_type != RR_L1_GSM_GSM) &&
               (handover_details_ptr->handover_type != RR_L1_GSM_WCDMA))
        {
          MSG_GERAN_ERROR_0_G("Unknown handover_type!!!");
        }

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if (message_set == MS_RR_L2)
          {
            if (message_id == (byte) DL_RECONNECT_CNF)
            {
              /* Reset the S_counter to RADIO_LINK_TIMEOUT */
              rr_reset_S_counter(gas_id);

              proc_dispatch_event = EV_HANDOVER_TERMINATED;
              rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;

              /* SAPI0 link has been re-established */
              rr_set_link_status(SAPI0_LINK, gas_id);

              /* Revert the DTX indication to match that broadcast by the network */
              /* (This will take effect on the next reception of SI5/6) */
              rr_dtx_rr_override_off(gas_id);

              rr_eng_mode_update_codec_ho_info(
                dedic_ptr->curr_channel_spec.channel_mode_1,
                handover_details_ptr->channel_spec.channel_mode_1,
                handover_details_ptr->channel_spec.channel_info_1_after.channel_type,
                FALSE,
                SYS_HO_FAIL,
                rr_map_rr_l1_handover_type_to_sys_ho_type(handover_details_ptr->handover_type),
                gas_id
              );
            }
            else if ((message_id == (byte) DL_RELEASE_IND) ||
                     (message_id == (byte) MDL_ERROR_IND))
            {
              // A L2 failure has occurred while waiting for L2 reconnection - release the connection
              rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
              proc_dispatch_event = EV_RADIO_LINK_FAILURE;
            }
          }  /* message_set == MS_RR_L2 */
        } /* EV_INPUT_MESSAGE  */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in HO_AWAITING_RECONNECTION");
          proc_dispatch_event = EV_PANIC_RESET_CNF;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }

        break;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
      /******************************************************************/
      /* STATE: RR_W2G_HO_PROCESS_NAS_DATA                           */
      /* This State is appliable only in WCDMA (or TDS) to GSM Handover */
      /* In this state we are waiting for NAS_DATA_IND from RRC      */
      /* RR will queue the NAS Data from RRC till it receives a      */
      /* message with '0' length, and after that activation ind will */
      /* be sent to MM, indicating GSM is now the active RAT         */
      /******************************************************************/

      case RR_W2G_HO_PROCESS_NAS_DATA :

        if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
        {
          if (message_set == MS_RRC_RR)
          {
#ifdef FEATURE_WCDMA
            if (message_id == (byte) RR_NAS_DATA_IND)
            {
              /*************************************************************/
              /* Store in rr_rrc_nas_message_buffer                        */
              /* till we get the msg length = 0, send activation_ind to MM */
              /* then move to RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP         */
              /* RRC will set the msg length to 0 when there no more NAS   */
              /* NAS messages to be sent                                   */
              /*************************************************************/
              if (rr_store_nas_data_ind(new_message, gas_id) == FALSE)
              {
                rr_send_activation_ind(MM_AS_INTERRAT_HANDOVER, gas_id);
                rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP;
              }
            }
            else if (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ)
            {
              MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from RRC");
              new_handover_event = deactivate_during_x2g_handover(gas_id);
              rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
            }
#endif /* FEATURE_WCDMA */
          } /* MS_RRC_RR */

#ifdef FEATURE_GSM_TDS
          else if (message_set == MS_TDSRRC_RR)
          {
            if (message_id == (byte) TDSRR_NAS_DATA_IND)
            {
              /*************************************************************/
              /* Store in rr_rrc_nas_message_buffer                        */
              /* till we get the msg length = 0, send activation_ind to MM */
              /* then move to RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP         */
              /* RRC will set the msg length to 0 when there no more NAS   */
              /* NAS messages to be sent                                   */
              /*************************************************************/
              if (rr_store_tds_nas_data_ind(new_message, gas_id) == FALSE)
              {
                rr_send_activation_ind(MM_AS_INTERRAT_HANDOVER, gas_id);
                rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP;
              }
            }
            else if (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ)
            {
              MSG_GERAN_HIGH_0_G("Received Inter-RAT reselection Abort from TDSRRC");
              new_handover_event = deactivate_during_x2g_handover(gas_id);
              rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
            }
          } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/

          else if (message_set == MS_MM_RR)
          {
            if (message_id == (byte) RR_ABORT_REQ)
            {
              ERR("RR shouldn't get this message, MM is weird state",0,0,0);
              // No Handover Cnf will be sent to RRC in this case
              new_handover_event = deactivate_during_x2g_handover(gas_id);
              rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
            }
          } /* MS_MM_RR */
          else
          {
            ERR("Unexpected message (%d,%d) in RR_W2G_HO_PROCESS_NAS_DATA",(int) message_set, (int) message_id,0);
          }
        } /* EV_INPUT_MESSAGE */

        else if (handover_event == EV_PANIC_RESET_REQ)
        {
          MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in RR_W2G_HO_PROCESS_NAS_DATA  (buffered)");
          rr_inter_ho_data_ptr->panic_reset_pending = TRUE;
        }

        else
        {
          ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
        }

        break;


    /***************************************************************/
    /* STATE: RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP                 */
    /* This State is appliable only in WCDMA to GSM Handover       */
    /* In this state we are waiting for Activation Rsp             */
    /* From MM.                                                    */
    /***************************************************************/

    case RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP :

      /*************************************************************/
      /*  RR is waiting for the activation response from MM        */
      /*  to confirm that GSM is now active RAT.                   */
      /*************************************************************/
      if ( (handover_event == EV_INPUT_MESSAGE) && (new_message != NULL) )
      {
        if (message_set == MS_RRC_RR)
        {
#ifdef FEATURE_WCDMA
          if (message_id == (byte) RR_NAS_DATA_IND)
          {
            /* store the message only if we received a non-zero length */
            if (rr_store_nas_data_ind(new_message, gas_id) == FALSE)
            {
              MSG_GERAN_ERROR_0_G("RRC sent invalid L3 message (length=0)");
            }
          }
          else if (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ)
          {
            ERR("Actually It is little late for Handover Abort",0,0,0);
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_WCDMA */
        }
#ifdef FEATURE_GSM_TDS
        else if (message_set == MS_TDSRRC_RR)
        {
          if (message_id == (byte) TDSRR_NAS_DATA_IND)
          {
            /* store the message only if we received a non-zero length */
            if (rr_store_tds_nas_data_ind(new_message, gas_id) == FALSE)
            {
              MSG_GERAN_ERROR_0_G("TDSRRC sent invalid L3 message (length=0)");
            }
          }
          else if (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ)
          {
            ERR("Actually It is little late for TDS Handover Abort",0,0,0);
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
        }
#endif /*FEATURE_GSM_TDS*/
        else if (message_set == MS_MSGR_RR)
        {
#ifdef FEATURE_LTE
          if (new_message->rr_msgr_msg.message.hdr.id == GERAN_GRR_LTE_ABORT_HO_REQ)
          {
            new_handover_event = deactivate_during_x2g_handover(gas_id);
            rr_inter_ho_data_ptr->handover_state = RR_W2G_HO_GOING_INACTIVE;
          }
#endif /* FEATURE_LTE */
        }
        else if (message_set == MS_MM_RR)
        {
          if (message_id == (byte) RR_DATA_REQ)
          {
            rr_data_req_T rr_data_req = ((rr_cmd_bdy_type *) new_message)->rr_data_req;

            rr_w2g_store_mm_message(&rr_data_req, gas_id);
          }
          else if (message_id == (byte) RR_ACTIVATION_RSP)
          {
            rr_PLMN_request_details_T *rr_PLMN_request_details_ptr;

            // Process all the rr_rrc_nas_messages first
            // Then rr_mm_nas_messages next.
            // and then send the dl_resume_req to L2
            rr_activation_rsp_T *rr_activation_rsp;

            rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
            RR_NULL_CHECK_FATAL(rr_PLMN_request_details_ptr);

            /* As far as NAS is concerned GERAN is now the active RAT, so
            we can clear out our saved panic reset message (which should
            be an IRAT reselection request). */
            rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

            rr_w2g_process_stored_nas_messages(gas_id);

            /*******************************************************************************/
            /* Vocoder control related changes, send the Sync_ind to MM     */
            /* only for non SRVCC case                       */
            /*******************************************************************************/
            if (handover_details_ptr->handover_type != RR_L1_LTE_GSM )
            {
              rr_send_sync_ind(
                MM_RR_WCDMA_GSM_HO,
                dedic_ptr->curr_channel_spec.channel_info_1_after.channel_type,
                dedic_ptr->curr_channel_spec.channel_mode_1,
                dedic_ptr->curr_channel_spec.channel_info_1_after.subchannel,
                gas_id
              );
            }
            else
            {
              MSG_GERAN_HIGH_1_G("No RR_SYNC_IND after Act_rsp for HO type = %d",handover_details_ptr->handover_type);
            }

            if (handover_details_ptr->handover_type == RR_L1_LTE_GSM )
            {
#ifdef FEATURE_WCDMA
              /* key is available, advise RRC */
              rr_send_cipher_sync_ind(gas_id);
#endif /* FEATURE_WCDMA */
            }

            /* Revert the DTX indication to match that broadcast by the network */
            /* (This will take effect on the next reception of SI5/6) */
            rr_dtx_rr_override_off(gas_id);

            // Set the PLMN and any cell selection Mode from Activation_rsp
            rr_activation_rsp = (&((rr_cmd_bdy_type *) new_message)->rr_activation_rsp);

            /* ignore any_cell_selection_mode in RR_ACTIVATION_RSP */
            rr_store_service_domain(rr_activation_rsp->service_domain,gas_id);


            rr_store_plmn_id(
              &rr_activation_rsp->PLMN_id,    // PLMN_id
              gas_id                          // gas_id
            );
#ifdef FEATURE_SGLTE
            if(( rr_is_sglte_mode_active(gas_id)) &&
		   	  ( rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_ONLY)
		   	 ) 
           {
             MSG_GERAN_HIGH_0_G("SGLTE service domain is CS_ONLY, reset RAT_PRI_LIST to GSM only");
             rr_sglte_set_rat_priority_list_gsm_only(gas_id);
           }	 
#endif /*FEATURE_SGLTE*/			

            /* Initialize the Radio link Timeout value until we get SI */
            scell_info_ptr->gsm.cell_options.RADIO_LINK_TIMEOUT = RR_DEF_RLT_COUNTER;

            scell_info_ptr->gsm.RLT_valid = FALSE;

            /* Setup the "previous cell camped onto" information */
            rr_set_previous_LAI(
              rr_activation_rsp->PLMN_id,               // plmn_id
              rr_activation_rsp->location_area_code,    // lac
              gas_id                                    // gas_id
            );

#ifdef FEATURE_GSM_DTM
#ifdef FEATURE_WCDMA
            if (rr_get_nv_dtm_enabled(RR_GAS_ID_TO_AS_ID))
            {
              rr_dtm_w2g_ho(
                rr_activation_rsp->PS_Attached,   // ps_attached
                gas_id                            // gas_id
              );
            }
            else
#endif /* FEATURE_WCDMA */
#endif /* FEATURE_GSM_DTM */
            {
              /* Set the default resumption indication to GPRS Resumption Failure. This is in */
              /* case of abnormal call release, e.g. Radio Link Failure */
              grr_set_gprs_resumption_indicator(FALSE, gas_id);

              /* Suspend GPRS in GMM and the network if ps-attached on W */
              if (rr_activation_rsp->PS_Attached)
              {
                grr_send_gprs_suspension_ota_after_w2g_ho(gas_id);
              }

              /*
              GMM is always suspended following W2G HO (if the service domain allows PS)
              This mimics the behaviour when a CS conn is being established in G
              (see handling of RR_EST_REQ in GRR_CAMPED state)
              */
              rr_gmm_suspend_gprs(gas_id);
            }

#ifdef FEATURE_LTE
            /* Store Dedicated Priority information. */
            rr_dedicated_priority_info_store(
              &rr_inter_ho_data_ptr->dedicated_priority_info,
              &handover_details_ptr->BCCH_ARFCN,
              rr_activation_rsp->PLMN_id,
              gas_id
            );

            rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */

            new_handover_event = EV_HANDOVER_COMPLETED;
            rr_inter_ho_data_ptr->handover_state = AWAITING_HANDOVER_COMPLETION;
          }
          else if (message_id == (byte) RR_ABORT_REQ)
          {
            // At this stage, this will be treated as abort in Data Transfer
            proc_dispatch_event = RR_EV_W2G_HO_ABORTED;
            rr_ho_reset_internal_variables(handover_details_ptr, gas_id);
            rr_inter_ho_data_ptr->handover_state = HANDOVER_INACTIVE;
          }
        }
        else
        {
          ERR("Unexpected message (%d,%d) in RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP",(int) message_set, (int) message_id,0);
        }

      }

      else if (handover_event == EV_PANIC_RESET_REQ)
      {
        MSG_GERAN_HIGH_0_G("rr_inter_cell_handover_control: Panic Reset in RR_W2G_HO_WAITING_FOR_ACTIVATION_RSP (buffered)");
        rr_inter_ho_data_ptr->panic_reset_pending = TRUE;
      }

      else
      {
        ERR("UnexEV. Ev(%d), State(%d)",(int) handover_event,(int) rr_inter_ho_data_ptr->handover_state,0);
      }

      break;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

      default:
      {
        ERR("Bad Handover State %d",(int)rr_inter_ho_data_ptr->handover_state,0,0);
      }
      break;

    } /* switch */

#ifdef DEBUG_RR_TRACE_MSG
    if ( ((handover_event != new_handover_event) && (new_handover_event != EV_NO_EVENT)) || (rr_inter_ho_data_ptr->old_handover_state != rr_inter_ho_data_ptr->handover_state))
    {
        rr_store_trace_msg_buf(
          RR_INTER_CELL_HO_SM,
          new_handover_event,
          (byte) rr_inter_ho_data_ptr->handover_state,
          new_message,
          gas_id
        );
    }
#endif

    if (new_handover_event != EV_NO_EVENT)
    {
      handover_event = new_handover_event;
      new_handover_event = EV_NO_EVENT;
    }
    else
    {
      handover_event = EV_NO_EVENT;
    }
  }

  if (rr_inter_ho_data_ptr->old_handover_state != rr_inter_ho_data_ptr->handover_state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_inter_cell_handover_control",
      rr_inter_ho_data_ptr->old_handover_state,
      rr_inter_ho_data_ptr->handover_state,
      rr_handover_state_name,
      gas_id
    );

    rr_inter_ho_data_ptr->old_handover_state = rr_inter_ho_data_ptr->handover_state;
  }

  return proc_dispatch_event;
} /* rr_inter_cell_handover_control */


/*===========================================================================

FUNCTION  rr_ho_reset_internal_variables

DESCRIPTION  Resets the WCDMA to GSM Inter Rat Handover State Machine
             and associated internal variables, after successful Handover
             or termination.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/
static void rr_ho_reset_internal_variables(
  handover_details_T *handover_details_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_inter_ho_data_t *rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  /*****************************************************/
  /* Perform the handover details store initialisation */
  /*****************************************************/
  if (handover_details_ptr != NULL)
  {
    rr_init_handover_details(
      handover_details_ptr,     // handover_details_ptr
      RR_L1_GSM_GSM,            // handover_type
      gas_id                    // gas_id
    );

    rr_set_handover_band_info(
      handover_details_ptr,      // handover_details_ptr
      SI_PSI_BAND_IND_DCS1800,   // rr_band_ind
      RR_L1_DCS_1800,            // l1_band_ind
      gas_id                     // gas_id
    );
  }

  /*****************************************************************************/
  /* NOTE: do not change the cipher_mode_command_received flag here, it should */
  /* only be modified when the call is release                                 */
  /*****************************************************************************/
  /* Reset all the other Handover State Machine variables */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_inter_ho_data_ptr->rr_x2g_ho_failure_reason      = RR_X2G_HO_FAILURE_INVALID_STATE;
  rr_inter_ho_data_ptr->rr_mm_abort_received          = FALSE;
  rr_inter_ho_data_ptr->rr_interrat_ho_abort_received = FALSE;
  rr_inter_ho_data_ptr->rr_x2g_ho_band_ind            = RR_L1_DCS_1800;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
  rr_inter_ho_data_ptr->tds_pseudo_fr_active = FALSE;
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

  /* If the handover was successful then the message queues should be empty.
  If the handover was aborted, there may be buffered messages that need to be
  deleted.
  */
  if (rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue != NULL)
  {
    MSG_GERAN_MED_0_G("NAS message queue is not empty, deleting all remaining messages");
    rr_w2g_empty_mm_message_queue(gas_id);
  }
  if (rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue != NULL)
  {
    MSG_GERAN_MED_0_G("RRC message queue is not empty, deleting all remaining messages");
    rr_w2g_empty_rrc_message_queue(gas_id);
  }

  rr_inter_ho_data_ptr->panic_reset_pending = FALSE;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/
}

#ifdef FEATURE_GSM_TDS
/**
 * Converts a TDSRR_NAS_DATA_IND into a RR_DATA_REQ and stores
 * in local RR queue Returns TRUE if the NAS message was valid
 * (i.e. has a non-zero length).
 */
static boolean rr_store_tds_nas_data_ind(
  const void *message_ptr,
  const gas_id_t gas_id
)
{
  tdsrr_nas_data_ind_type rr_nas_data_ind;
  boolean                 bStored;
  uint8                   message_length;

  RR_NULL_CHECK_FATAL(message_ptr);

  rr_nas_data_ind = ((rr_cmd_bdy_type *)message_ptr)->tdsrr_nas_data_ind;

  message_length = rr_nas_data_ind.nas_data.message_length;

  if (message_length != 0)
  {
    rr_data_req_T rr_data_req;

    RR_ZERO_STRUCTURE(rr_data_req);

    /* sanity check the length */
    if (message_length > MAX_OCTETS_L3_MESSAGE)
    {
      MSG_GERAN_ERROR_0_G("WARNING: NAS payload too large for RR, truncating");
      message_length = MAX_OCTETS_L3_MESSAGE;
    }

    /* copy over the L3 data */
    memscpy(rr_data_req.layer3_message,
            sizeof(rr_data_req.layer3_message),
            rr_nas_data_ind.nas_data.payload,
            message_length);

  /* set length and header fields appropriately */
    rr_data_req.l3_message_length = message_length;
    PUT_IMH_LEN(sizeof(rr_data_req_T) - sizeof(IMH_T), &(rr_data_req.message_header));

    /* store the message */
    rr_w2g_store_rrc_message(
      &rr_data_req,   // rrc_message
      gas_id          // gas_id
    );

    bStored = TRUE;
  }
  else
  {
    bStored = FALSE;
  }

  return(bStored);
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/**
 * Converts a RR_NAS_DATA_IND into a RR_DATA_REQ and stores in local RR queue
 * Returns TRUE if the NAS message was valid (i.e. has a non-zero length).
 */
static boolean rr_store_nas_data_ind(
  const void *message_ptr,
  const gas_id_t gas_id
)
{
  rr_nas_data_ind_type rr_nas_data_ind;
  boolean              bStored;
  uint8                message_length;

  RR_NULL_CHECK_FATAL(message_ptr);

  rr_nas_data_ind = ((rr_cmd_bdy_type *)message_ptr)->rr_nas_data_ind;

  message_length = rr_nas_data_ind.nas_data.message_length;

  if (message_length != 0)
  {
    rr_data_req_T rr_data_req;

    RR_ZERO_STRUCTURE(rr_data_req);

    /* sanity check the length */
    if (message_length > MAX_OCTETS_L3_MESSAGE)
    {
      MSG_GERAN_ERROR_0_G("WARNING: NAS payload too large for RR, truncating");
      message_length = MAX_OCTETS_L3_MESSAGE;
    }

    /* copy over the L3 data */
    memscpy(rr_data_req.layer3_message,
       sizeof(rr_data_req.layer3_message),
           rr_nas_data_ind.nas_data.payload,
           message_length);


    /* set length and header fields appropriately */
    rr_data_req.l3_message_length = message_length;
    PUT_IMH_LEN(sizeof(rr_data_req_T) - sizeof(IMH_T), &(rr_data_req.message_header));

    /* store the message */
    rr_w2g_store_rrc_message(
      &rr_data_req,   // rrc_message
      gas_id          // gas_id
    );

    bStored = TRUE;
  }
  else
  {
    bStored = FALSE;
  }

  return(bStored);
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_WCDMA
/*===========================================================================

FUNCTION  rr_w2g_empty_mm_message_queue

DESCRIPTION  Clears the MM NAS message queue

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/

static void rr_w2g_empty_mm_message_queue(const gas_id_t gas_id)
{
  rr_w2g_nas_message_entry_T *next_nas_msg;
  rr_inter_ho_data_t  *rr_inter_ho_data_ptr;

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  while (rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue != NULL)
  {
    next_nas_msg = rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue->next_message;
    GS_FREE(rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue);
    rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue = next_nas_msg;
  }
}

/*===========================================================================

FUNCTION  rr_w2g_empty_rrc_message_queue

DESCRIPTION  Clears the RRC NAS message buffer

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/

static void rr_w2g_empty_rrc_message_queue(const gas_id_t gas_id)
{
  rr_w2g_nas_message_entry_T *next_rrc_msg;
  rr_inter_ho_data_t  *rr_inter_ho_data_ptr;

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  while (rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue != NULL)
  {
    next_rrc_msg = rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue->next_message;
    GS_FREE(rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue);
    rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue = next_rrc_msg;
  }
}

/*===========================================================================

FUNCTION  rr_w2g_store_mm_message

DESCRIPTION  Stores the NAS message sent by MM in a local message queue
             to process afterwards.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/

static void rr_w2g_store_mm_message(
  const rr_data_req_T *nas_message,
  const gas_id_t gas_id
)
{
  rr_w2g_nas_message_entry_T *temp;
  rr_w2g_nas_message_entry_T *new_nas_message;
  rr_inter_ho_data_t  *rr_inter_ho_data_ptr;

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  temp = rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue;

  if (rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue != NULL)
  {
    /*************************************/
    /* Add the message in the send_queue */
    /*************************************/

    temp = rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue;

    while (temp->next_message != NULL)
    {
      temp = temp->next_message;
    }

    new_nas_message = GS_ALLOC(sizeof(rr_w2g_nas_message_entry_T));

    if (new_nas_message == NULL)
    {
      ERR("Couldn't Allocate Memory",0,0,0);
      return;
    }

    new_nas_message->next_message = NULL;

  new_nas_message->nas_message = *nas_message;

    temp->next_message = new_nas_message;

  }
  else /* rr_w2g_nas_msg_queue != NULL */
  {
    /* Add message as the first entry */

    new_nas_message = GS_ALLOC(sizeof(rr_w2g_nas_message_entry_T));

    if (new_nas_message == NULL)
    {
      ERR("Couldn't Allocate Memory",0,0,0);
      return;
    }

    new_nas_message->next_message = NULL;

  new_nas_message->nas_message = *nas_message;


    rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue = new_nas_message;
  }
}

/*===========================================================================

FUNCTION  rr_w2g_store_rrc_message

DESCRIPTION  Stores the NAS message sent by RRC in RRC message buffer
             to process after wards.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/

static void rr_w2g_store_rrc_message(
  const rr_data_req_T *rrc_message,
  const gas_id_t gas_id
)
{
  rr_w2g_nas_message_entry_T      *temp;
  rr_w2g_nas_message_entry_T      *new_rrc_message;
  rr_inter_ho_data_t  *rr_inter_ho_data_ptr;

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  if (rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue != NULL)
  {
    /*************************************/
    /* Add the message queue             */
    /*************************************/

    temp = rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue;

    while (temp->next_message != NULL)
    {
      temp = temp->next_message;
    }
    new_rrc_message = GS_ALLOC(sizeof(rr_w2g_nas_message_entry_T));

    if (new_rrc_message == NULL)
    {
      ERR("Couldn't Allocate Memory",0,0,0);
      return;
    }
    new_rrc_message->next_message = NULL;
  new_rrc_message->nas_message = *rrc_message;

    temp->next_message = new_rrc_message;
  }
  else
  {
    new_rrc_message = GS_ALLOC(sizeof(rr_w2g_nas_message_entry_T));

    if (new_rrc_message == NULL)
    {
      ERR("Couldn't Allocate Memory",0,0,0);
      return;
    }
    new_rrc_message->next_message = NULL;


  new_rrc_message->nas_message = *rrc_message;


    rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue = new_rrc_message;
  }
}

/*===========================================================================

FUNCTION  rr_w2g_process_rrc_nas_messages

DESCRIPTION  Processes the queued NAS messages ( sent by RRC & MM )

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

===========================================================================*/
static void rr_w2g_process_stored_nas_messages(const gas_id_t gas_id)
{
  rr_w2g_nas_message_entry_T *rrc_message;
  rr_w2g_nas_message_entry_T *nas_message;
  rr_inter_ho_data_t *rr_inter_ho_data_ptr;

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  /*************************************/
  /* Send out the RRC messages first   */
  /*************************************/

  if (rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue != NULL)
  {
    rrc_message = rr_inter_ho_data_ptr->rr_w2g_rrc_msg_queue;
    while (rrc_message != NULL)
    {
      rr_process_rr_data_req(&rrc_message->nas_message, gas_id);
      rrc_message = rrc_message->next_message;
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Nothing to send in RRC message queue");
  }
  /*************************************/
  /* Next send out the MM NAS messages */
  /*************************************/
  if (rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue != NULL)
  {
    nas_message = rr_inter_ho_data_ptr->rr_w2g_nas_msg_queue;
    while (nas_message != NULL)
    {
      rr_process_rr_data_req(&nas_message->nas_message, gas_id);
      nas_message = nas_message->next_message;
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("Nothing to send in MM NAS message queue");
  }
  /* Clear both RRC and MM NAS message queues */
  rr_w2g_empty_mm_message_queue(gas_id);
  rr_w2g_empty_rrc_message_queue(gas_id);
}
#endif /* FEATURE_WCDMA */

void rr_set_handover_type(rr_l1_handover_type_T ho_type, const gas_id_t gas_id)
{
  rr_dedicated_data_T *dedic_ptr;

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr != NULL)
  {
    dedic_ptr->handover_details.handover_type = ho_type;
  }

  return;
}

#ifdef FEATURE_WCDMA
/*===========================================================================
FUNCTION  rr_w2g_store_gsm_commands

DESCRIPTION  Stores the GSM command list from an WCDMA H/O request in

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS Updates the static gsm_command_list data in this module
===========================================================================*/
static void rr_w2g_store_gsm_commands(
  const rr_interrat_handover_req_type *irat_ho_req,
  const gas_id_t gas_id
)
{
  uint8 num_cmds;
  uint8 msg_len;
  uint8 i;
  rr_inter_ho_data_t *rr_inter_ho_data_ptr;

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  num_cmds = irat_ho_req->gsm_command_list.number_of_commands;
  rr_inter_ho_data_ptr->gsm_command_list.number_of_commands = 0;

  for (i = 0; ((i < num_cmds) && (i < RRC_RR_MAX_GSM_COMMANDS)); i++)
  {
    msg_len = irat_ho_req->gsm_command_list.gsm_commands[i].message_length;

    if (i < RR_X2G_HO_MAX_GSM_COMMANDS)
    {
      rr_inter_ho_data_ptr->gsm_command_list.gsm_commands[i].message_length = msg_len;
      memscpy(rr_inter_ho_data_ptr->gsm_command_list.gsm_commands[i].payload,
        sizeof(rr_inter_ho_data_ptr->gsm_command_list.gsm_commands[i].payload),
        irat_ho_req->gsm_command_list.gsm_commands[i].payload,
        msg_len);
  
      rr_inter_ho_data_ptr->gsm_command_list.number_of_commands++;
    }
    else
    {
      MSG_GERAN_ERROR_2_G("RR W2G number of GSM commands: %d greater than max: %d",
        num_cmds,
        RR_X2G_HO_MAX_GSM_COMMANDS
      );
      break;
    }
  }

  return;
}

/*===========================================================================
FUNCTION  rr_convert_x2g_ho_gsm_cmds_to_wcdma

DESCRIPTION  Convert the RR X2G HO GSM Commands List to the WCDMA type.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS Write to w_gsm_cmd_list
===========================================================================*/
static void rr_convert_x2g_ho_gsm_cmds_to_wcdma(
  rr_x2g_ho_gsm_command_list_t *x2g_cmd_list,
  rrc_rr_gsm_command_list_type *w_gsm_cmd_list
)
{
  uint8 i = 0;

  memset (w_gsm_cmd_list, 0, sizeof (rrc_rr_gsm_command_list_type));

  if (RRC_RR_MAX_GSM_MESSAGE_LENGTH != RR_X2G_HO_GSM_MESSAGE_LENGTH)
  {
    MSG_GERAN_ERROR_2("RR RRC_RR_MAX_GSM_MESSAGE_LENGTH: %d, RR_X2G_HO_GSM_MESSAGE_LENGTH: %d mismatch",
        RRC_RR_MAX_GSM_MESSAGE_LENGTH, RR_X2G_HO_GSM_MESSAGE_LENGTH);
  }

  if (x2g_cmd_list->number_of_commands > RRC_RR_MAX_GSM_COMMANDS)
  {
    MSG_GERAN_ERROR_1("RR number_of_commands: %d, out of range",x2g_cmd_list->number_of_commands);
    return;
  }

  w_gsm_cmd_list->number_of_commands = x2g_cmd_list->number_of_commands;

  for (i=0; i<x2g_cmd_list->number_of_commands; i++)
  {
    #if RRC_RR_MAX_GSM_MESSAGE_LENGTH < 256  /* the following test is tautologous and causes a compiler warning */
    if (x2g_cmd_list->gsm_commands[i].message_length > RRC_RR_MAX_GSM_MESSAGE_LENGTH)
    {
      MSG_GERAN_ERROR_1("RR X2G HO GSM Cmd message length out of range: %d",x2g_cmd_list->gsm_commands[i].message_length);
      memset (w_gsm_cmd_list, 0, sizeof (rrc_rr_gsm_command_list_type));
      return;
    }
    #endif

    w_gsm_cmd_list->gsm_commands[i].message_length = x2g_cmd_list->gsm_commands[i].message_length;
    memscpy(w_gsm_cmd_list->gsm_commands[i].payload,
      sizeof(w_gsm_cmd_list->gsm_commands[i].payload),
      x2g_cmd_list->gsm_commands[i].payload,
      x2g_cmd_list->gsm_commands[i].message_length); 
  }
}

/*===========================================================================
FUNCTION  rr_convert_rr_x2g_ho_failure_cause_to_lte

DESCRIPTION  Convert the RR X2G HO failure cause to the WCDMA cause

DEPENDENCIES None

RETURN VALUE rrc_rr_failure_cause_e_type

SIDE EFFECTS None
===========================================================================*/
static rrc_rr_failure_cause_e_type rr_convert_rr_x2g_ho_failure_cause_to_wcdma(
  rr_x2g_ho_failure_e rr_x2g_cause
)
{
  rrc_rr_failure_cause_e_type failure_cause = RRC_RR_INVALID_STATE;

  switch (rr_x2g_cause)
  {
    case RR_X2G_HO_FAILURE_INVALID_STATE:
      failure_cause = RRC_RR_INVALID_STATE;
    break; 
    case RR_X2G_HO_FAILURE_PROTOCOL_ERROR:
      failure_cause = RRC_RR_GSM_PROTOCOL_ERROR;
    break;
    case RR_X2G_HO_FAILURE_INVALID_CONFIGURATION:
      failure_cause = RRC_RR_INVALID_CONFIGURATION;
    break;
    case RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE:
      failure_cause = RRC_RR_PHYSICAL_CHANNEL_FAILURE;
    break;
    case RR_X2G_HO_FAILURE_INVALID_CIPHER_ALGORITHM:
      failure_cause = RRC_RR_CIPHER_KEY_NOT_AVAILABLE;
    break;
    default:
      MSG_GERAN_ERROR_1("Bad X2G HO failure cause: %d", rr_x2g_cause);
    break;
  }

  return failure_cause;
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
/*===========================================================================
FUNCTION  rr_t2g_store_gsm_commands

DESCRIPTION  Stores the GSM command list from an TDS H/O request in the
             generic rr_inter_ho_data_ptr->gsm_command_list store.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS Updates the static gsm_command_list data in this module
===========================================================================*/
static void rr_t2g_store_gsm_commands(
  tdsrr_interrat_handover_req_type *tds_ho_req,
  const gas_id_t gas_id
)
{
  uint8 num_cmds;
  uint8 msg_len;
  uint8 i;
  rr_inter_ho_data_t *rr_inter_ho_data_ptr;

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  num_cmds = tds_ho_req->gsm_command_list.number_of_commands;
  rr_inter_ho_data_ptr->gsm_command_list.number_of_commands = 0;

  if (TDSRR_MAX_GSM_MESSAGE_LENGTH != RR_X2G_HO_GSM_MESSAGE_LENGTH)
  {
    MSG_GERAN_ERROR_0_G("RR TDSRR_MAX_GSM_MESSAGE_LENGTH and RR_X2G_HO_GSM_MESSAGE_LENGTH are different");
    return;
  }

  for (i = 0; ((i < num_cmds) && (i < TDSRR_MAX_GSM_COMMANDS)); i++)
  {
    msg_len = tds_ho_req->gsm_command_list.gsm_commands[i].message_length;

    if (i < RR_X2G_HO_MAX_GSM_COMMANDS)
    {
      rr_inter_ho_data_ptr->gsm_command_list.gsm_commands[i].message_length = msg_len;
  
      memscpy(rr_inter_ho_data_ptr->gsm_command_list.gsm_commands[i].payload,
        sizeof(rr_inter_ho_data_ptr->gsm_command_list.gsm_commands[i].payload),
        tds_ho_req->gsm_command_list.gsm_commands[i].payload,
        msg_len);
  
      rr_inter_ho_data_ptr->gsm_command_list.number_of_commands++;
    }
    else
    {
      MSG_GERAN_ERROR_2_G("RR T2G number of GSM commands: %d greater than max: %d",
        num_cmds,
        RR_X2G_HO_MAX_GSM_COMMANDS
      );
      break;
    }
  }

  return;
}

/*===========================================================================
FUNCTION  rr_convert_x2g_ho_gsm_cmds_to_tds

DESCRIPTION  Convert the RR X2G HO GSM Commands List to the TDS type.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS Write to t_gsm_cmd_list
===========================================================================*/
static void rr_convert_x2g_ho_gsm_cmds_to_tds(
  rr_x2g_ho_gsm_command_list_t *x2g_cmd_list,
  tdsrrc_rr_gsm_command_list_type *t_gsm_cmd_list
)
{
  uint8 i = 0;

  memset(t_gsm_cmd_list, 0, sizeof (tdsrrc_rr_gsm_command_list_type));

  if (TDSRR_MAX_GSM_MESSAGE_LENGTH != RR_X2G_HO_GSM_MESSAGE_LENGTH)
  {
    MSG_GERAN_ERROR_2("RR TDSRR_MAX_GSM_MESSAGE_LENGTH: %d, RR_X2G_HO_GSM_MESSAGE_LENGTH: %d mismatch",
        TDSRR_MAX_GSM_MESSAGE_LENGTH, RR_X2G_HO_GSM_MESSAGE_LENGTH);
  }

  if (x2g_cmd_list->number_of_commands > TDSRR_MAX_GSM_COMMANDS)
  {
    MSG_GERAN_ERROR_1("RR number_of_commands: %d, out of range",x2g_cmd_list->number_of_commands);
    return;
  }

  t_gsm_cmd_list->number_of_commands = x2g_cmd_list->number_of_commands;

  for (i=0; i<x2g_cmd_list->number_of_commands; i++)
  {
    #if TDSRR_MAX_GSM_MESSAGE_LENGTH < 256  /* the following test is tautologous and causes a compiler warning */
    if (x2g_cmd_list->gsm_commands[i].message_length > TDSRR_MAX_GSM_MESSAGE_LENGTH)
    {
      MSG_GERAN_ERROR_1("RR X2G HO GSM Cmd message length out of range: %d",x2g_cmd_list->gsm_commands[i].message_length);
      memset(t_gsm_cmd_list, 0, sizeof (tdsrrc_rr_gsm_command_list_type));
      return;
    }
    #endif

    t_gsm_cmd_list->gsm_commands[i].message_length = x2g_cmd_list->gsm_commands[i].message_length;
    memscpy(t_gsm_cmd_list->gsm_commands[i].payload,
      sizeof(t_gsm_cmd_list->gsm_commands[i].payload),
      x2g_cmd_list->gsm_commands[i].payload,
      x2g_cmd_list->gsm_commands[i].message_length); 
  }
}

/*===========================================================================
FUNCTION  rr_convert_rr_x2g_ho_failure_cause_to_tds

DESCRIPTION  Convert the RR X2G HO failure cause to the TDS cause

DEPENDENCIES None

RETURN VALUE tdsrrc_rr_failure_cause_e_type

SIDE EFFECTS None
===========================================================================*/
static tdsrrc_rr_failure_cause_e_type rr_convert_rr_x2g_ho_failure_cause_to_tds(
  rr_x2g_ho_failure_e rr_x2g_cause
)
{
  tdsrrc_rr_failure_cause_e_type tds_cause = TDSRRC_RR_INVALID_STATE;

  switch (rr_x2g_cause)
  {
    case RR_X2G_HO_FAILURE_INVALID_STATE:
      tds_cause = TDSRRC_RR_INVALID_STATE;
    break;
    case RR_X2G_HO_FAILURE_PROTOCOL_ERROR:
      tds_cause = TDSRRC_RR_GSM_PROTOCOL_ERROR;
    break;
    case RR_X2G_HO_FAILURE_INVALID_CONFIGURATION:
      tds_cause = TDSRRC_RR_INVALID_CONFIGURATION;
    break;
    case RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE:
      tds_cause = TDSRRC_RR_PHYSICAL_CHANNEL_FAILURE;
    break;
    case RR_X2G_HO_FAILURE_INVALID_CIPHER_ALGORITHM:
      tds_cause = TDSRRC_RR_CIPHER_KEY_NOT_AVAILABLE;
    break;
    default:
      MSG_GERAN_ERROR_1("Bad X2G HO failure cause: %d", rr_x2g_cause);
    break;
  }

  return tds_cause;
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
/*===========================================================================
FUNCTION  rr_l2g_store_gsm_commands

DESCRIPTION  Stores the GSM command list from an LTE H/O request in the
             generic rr_inter_ho_data_ptr->gsm_command_list store.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS Updates the static gsm_command_list data in this module
===========================================================================*/
static void rr_l2g_store_gsm_commands(
  const geran_grr_lte_ho_req_t * lte_ho_req,
  const gas_id_t gas_id
)
{
  rr_inter_ho_data_t      * rr_inter_ho_data_ptr;
  msgr_attach_struct_type * attachment;

  RR_NULL_CHECK_RETURN_VOID(lte_ho_req);

  // Obtain a pointer to the module data
  rr_inter_ho_data_ptr = rr_inter_ho_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_inter_ho_data_ptr);

  attachment = msgr_get_attach(&lte_ho_req->msg_hdr, 0);

  if (attachment != NULL)
  {
    dsm_item_type * dsm_ptr = NULL;

    msgr_get_dsm_attach(attachment, &dsm_ptr);

    if (dsm_ptr != NULL)
    {
      uint32 message_length = dsm_length_packet(dsm_ptr);

      if (message_length > RR_X2G_HO_GSM_MESSAGE_LENGTH)
      {
        MSG_GERAN_ERROR_1_G("Truncating handover command (length %d)", message_length);
        message_length = RR_X2G_HO_GSM_MESSAGE_LENGTH;
      }

      dsm_pullup(&dsm_ptr, rr_inter_ho_data_ptr->gsm_command_list.gsm_commands[0].payload, (uint16) message_length);
      rr_inter_ho_data_ptr->gsm_command_list.gsm_commands[0].message_length = (uint8) message_length;
      rr_inter_ho_data_ptr->gsm_command_list.number_of_commands = 1;
      dsm_free_packet(&dsm_ptr);
    }
  }
}

/*===========================================================================
FUNCTION  rr_convert_rr_x2g_ho_failure_cause_to_lte

DESCRIPTION  Convert the RR X2G HO failure cause to the LTE cause

DEPENDENCIES None

RETURN VALUE geran_grr_lte_ho_failure_cause_type_e

SIDE EFFECTS None
===========================================================================*/
static geran_grr_lte_ho_failure_cause_type_e rr_convert_rr_x2g_ho_failure_cause_to_lte(
  rr_x2g_ho_failure_e rr_x2g_cause
)
{
  geran_grr_lte_ho_failure_cause_type_e failure_cause = GERAN_GRR_LTE_HO_FAILURE_CAUSE_UNDEFINED;

  switch (rr_x2g_cause)
  {
    case RR_X2G_HO_FAILURE_INVALID_STATE:
      failure_cause = GERAN_GRR_LTE_HO_FAILURE_CAUSE_INVALID_STATE;
    break; 
    case RR_X2G_HO_FAILURE_PROTOCOL_ERROR:
      failure_cause = GERAN_GRR_LTE_HO_FAILURE_CAUSE_PROTOCOL_ERROR;
    break;
    case RR_X2G_HO_FAILURE_INVALID_CONFIGURATION:
      failure_cause = GERAN_GRR_LTE_HO_FAILURE_CAUSE_INVALID_CONFIGURATION;
    break;
    case RR_X2G_HO_FAILURE_PHYSICAL_CHANNEL_FAILURE:
      failure_cause = GERAN_GRR_LTE_HO_FAILURE_CAUSE_PHYSICAL_CHANNEL_FAILURE;
    break;
    case RR_X2G_HO_FAILURE_INVALID_CIPHER_ALGORITHM:
      failure_cause = GERAN_GRR_LTE_HO_FAILURE_CAUSE_INVALID_CIPHER_ALGORITHM;
    break;
    default:
      MSG_GERAN_ERROR_1("Bad X2G HO failure cause: %d", rr_x2g_cause);
    break;
  }

  return failure_cause;
}
#endif /* FEATURE_LTE */

/* EOF */
