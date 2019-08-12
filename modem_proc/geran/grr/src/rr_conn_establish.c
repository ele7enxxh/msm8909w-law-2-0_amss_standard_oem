/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Connection Establishment Module

GENERAL DESCRIPTION
   This module contains the functions and sub-state machine associated
   with RR connection establishment functionality

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 by QUALCOMM Technologies, Inc. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_conn_establish.c_v   1.12   17 Jul 2002 11:51:36   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_conn_establish.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
Feb/14/13   sjw     Defer RR_PS_ABORT_REQ if first RACH response is still pending
Jun/27/12   sjw     Reinitialise and ignore DSC during circuit-switched RACH in DSDS mode
Jan/18/12   sjw     Correct EDTM functional breakage caused by FEATURE_LTE_TO_GSM
Jul/22/09   sjw     Added support for FEATURE_UMTS_VOICE_CIPHERING_IND
Mar/11/09   sjw     Moved global voice and data support flags into rr_channel_mode.c
Feb/13/09   sjw     Reworked local storage for ciphering module data
Sep/26/08   sjw     Reworked classmark enquiry interface
Jun/21/04   NJT     Reset back to CP_INACTIVE state if IA recieved after IARej
Jun/04/04   DMB     Better handling of returning to Idle in RR_CP_ASSIGNMENT_REJECTION_RECEIVED after T3146
May/26/04   DMB     Added MPH_START_IDLE_MODE_CNF handling
May/18/04   VT      Removed two unwanted ASSERTs.
05/07/04    gc      Enhanced Trace Buffer Code
04/29/04    gc      CR42000: Fixes to handle Early Classmark Sending Restriction
Mar/09/04    VT     In rr_connection_establishment_control(), on expiry of
                      T3126/T3146, restored call to (void)rr_timer_cancel(),
                      as the timer state is set to STOPPED inside (void)rr_timer_cancel(),
                      even if it has already expired.
Feb/24/04   VT      In rr_ce_process_page_request_type_1/2(), now ERR_FATAL
                      instead of ASSERT if
                      identity_length > RR_MAX_IDENTITY_SIZE
Feb/19/04   VT      In rr_connection_establishment_control(), call to
                      rr_gprs_suspend_camped_sys_info_handling() is enclosed in
                      #ifdef FEATURE_GSM_GPRS
01/07/04    gc      Fixes to SI Optimisation
Oct/21/03    VT     In rr_ce_process_immediate_assignment(), now passing an additional
                      parameter, starting_time_valid, while calling rr_decode_ia_rest_octets().
05/08/03    he      Used separate debug_str buffer for logging.
02/18/03    JAC     Lint Cleanup
02/14/03    RG      Added support for G2W blind Handover
01/22/03    he      Send UTRAN Classmark Change to network.
01/13/03    JAC     Added transfer of control to GRR/RR during conn establishment failures
01/08/02    rmc     Added flag to determine when returning to idle due to Immediate Assignment
                    reject message
10/30/02    he      Added rr_ce_set_waiting_for_mac_ul_tbf_cnf and various GPRS support.
10/28/02    JAC     removed rr_set_channel_frequency_list
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
08/10/02    rmc     Changes for cell selection optimizations
07/22/02    Rao     Adding mm_as_status to abort_cnf.
07/29/02    rmc     Store reselection trigger for T3126 timeout and logging failure when IA
                    procedure is aborted
07/15/02    rmc     Added new paging_type parameter to rr_send_paging_ind for GPRS support
07/15/02    rmc     New classmark function interfaces
07/01/02    rmc     Add debug check for CallReestablishment reasons
06/19/02    JAC     Added rr_check_page, interface changes to paging handlers
06/14/02    rmc     Backing out changes related to l3_message_length field in RR_EST_REQ and
                    RR_DATA_REQ
05/17/02    Rao     Added RR_ABORT_CNF support for Dual Mode.
05/16/02    Rao     Added RR_PAGING_IND support for Dual Mode.
04/08/02    Rao     Added RR_ABORT_REQ handling support
03/12/02    rmc     Changes for dual mode cell selection
02/21/02    rmc     Using current page mode accessors. New logic for cases where
                    page mode changes to PAGING_REORGANIZATION to start reading
                    BCCH data until the page mode changes (CR20636)
02/04/02    RMC     QXDM cleanup
02/04/02    rao     Started using new local rr sim info accessor functions.
01/15/02    atm     Added RAR logging
12/05/01    RMC     Fixed problems with connection establishment procedure and
                    added comments in several places. General code cleanup
11/12/01    RMC     Fixed problem of not initializing channel mode to signalling
                    only when successful immediate assignment is received
12/11/00    TS      Initial Revision.

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include <stdlib.h>
#include "rr_seg_load.h"
#include "rr_control.h"
#include "rr_conn_establish.h"
#include "ms.h"
#include "rr_l2.h"
#include "rr_general.h"
#include "rr_gprs_debug.h"
#include "rr_l1_send.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_classmark.h"
#include "rr_decode_ie.h"
#include "rr_conn_release.h"
#include "rr_sys_info.h"
#include "rr_channel_mode.h"
#include "cause.h"
#include "amssassert.h"
#include "rr_log.h"           /* for logging */
#include "mac_grr.h"
#include "gmacgrrsig.h"
#include "rr_gprs_conn_establish.h"
#include "csn.h"
#include "rr_gprs_defs.h"
#include "gmac.h"
#include "rr_resel.h"
#include "rr_sys_info_si.h"
#include "secapi.h" 

#include "rr_hz.h"

#ifdef FEATURE_GSM_DTM
#include "rr_dtm.h"
#ifdef FEATURE_GSM_EDTM
#include "rr_gprs_edtm.h"

#define RR_CE_EDTM_REQUEST(req) ((req).frame_number == RR_EDTM_FN)

#endif /* FEATURE_EDTM */
#endif /* FEATURE_DTM */

#include "rr_cell_reestablish.h"
#include "rr_cipher.h"
#include "rr_sys_info_pcell.h"
#include "ghdi_exp.h"
#include "rr_multi_sim.h"
#include "rr_gprs_dsc.h"
#include "rr_ded_config.h"
#include "rr_sim.h"
#include "rr_ps_access.h"
#include "rr_ps_domain.h"
#include "rr_l1_idle_mode.h"
#include "rr_if.h"
#include "rr_ce_paging.h"
#include "stringl/stringl.h" 
#include "rr_sys_info_cache.h"

#define RRCE_NOT_IDLE_NO_PAGING 0x00
#define RRCE_RESPOND_AS_IF_IDLE 0x01
#define RRCE_RESPOND_TO_CS_PAGE 0x02

#define RR_PAGING_RESPONSE_LENGTH 16

/* this value must be less than 255 */
#define RR_MS_MAX_FAILED_RACH_ATTEMPTS 10

/* this value must be greater than 42432 */
#define RR_CE_INVALID_RACH_FRAME_NUMBER 0xDEAD

#define RR_CE_LOG_UNEXPECTED_MESSAGE(_mp)  LOG_UNEXPECTED_MESSAGE(                                       \
                                             message_ptr,                                                \
                                             rr_con_est_state_name(rr_conn_est_info_ptr->con_est_state)  \
                                           );

#define RR_CE_LOG_UNEXPECTED_EVENT(_e)     LOG_UNEXPECTED_EVENT(                                         \
                                             rr_event_name(con_est_event),                               \
                                             rr_con_est_state_name(rr_conn_est_info_ptr->con_est_state)  \
                                           );


static rr_conn_est_info_T rr_conn_est_info[NUM_GERAN_DATA_SPACES];

#define RRCE_SET_CAMPED_FLAG(cf,gi) {\
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gi);\
  if (scell_info_ptr != NULL)\
  {\
    scell_info_ptr->gsm.camped_on_flag = (cf);\
  }\
  else\
  {\
    MSG_GERAN_ERROR_0_G("Unable to set camped_on_flag");\
  }\
}

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_conn_est_info_T*
 */
static rr_conn_est_info_T *rr_conn_est_get_data_ptr(const gas_id_t gas_id)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_conn_est_info[as_index];
}

/**
  @brief Checks whether a PCCO is in progress, according to the local
         Connection Establishment pcco_in_progress flag.

  This function is preferred over checking whether T3174 is running as it will
  return the correct value even if T3174 has expired, causing the failure of
  a PCCO procedure.
*/
static boolean rr_ce_pcco_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);

  if (rr_conn_est_info_ptr != NULL)
  {
    if (rr_conn_est_info_ptr->pcco_in_progress)
    {
      MSG_GERAN_HIGH_0_G("rr_ce_cco_in_progress: TRUE");
      return TRUE;
    }
  }

  MSG_GERAN_HIGH_0_G("rr_ce_cco_in_progress: FALSE");
  return FALSE;
}

/*!
 * \brief Called when a connection is established and confirmed to NAS (i.e. RR_EST_CNF/RR_EST_IND)
 */
static void rr_ce_conn_confirmed_to_nas(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->confirmed_to_nas = TRUE;
}

/**
 * The connection establishment has failed.
 * Perform any cleanup required and notify MM of the release/abort if requested
 */
static void rr_conn_est_failed(
  boolean notify_mm,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /* Restore the saved channel description */
  scell_info_ptr->gsm.cell_channel_description = scell_info_ptr->gsm.SI_cell_channel_description;

  if (notify_mm)
  {
    /* Send appropriate release/abort indication to MM */
    rr_conn_inform_mm_of_channel_release(FALSE, gas_id);
  }
} /* end rr_conn_failed_start_idle() */


/* ACCESS FUNCTIONS FOR STATIC DATA IN THIS MODULE */

void rr_ce_store_cs_page_info(
  byte *identity_ptr,
  byte channel_needed,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  if (identity_ptr != NULL)
  {
    //Using memscpy rather memcpy
    memscpy(
      rr_conn_est_info_ptr->paged_identity,
      sizeof(rr_conn_est_info_ptr->paged_identity), 
      identity_ptr, 
      RR_MAX_IDENTITY_SIZE
    );  
  }
  else
  {
    memset(rr_conn_est_info_ptr->paged_identity, 0, sizeof(rr_conn_est_info_ptr->paged_identity));
  }
  rr_conn_est_info_ptr->channel_needed = channel_needed;
}

#ifdef FEATURE_GSM_EDTM
/**
 * Copies the supplied identity into the local establishment details store
 *
 * @param id_ptr - array of bytes to copy (at least RR_MAX_IDENTITY_SIZE)
 */
void rr_ce_edtm_set_paged_identity(
  byte *identity_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

    //Using memscpy rather memcpy
  memscpy(rr_conn_est_info_ptr->paged_identity,sizeof(rr_conn_est_info_ptr->paged_identity), identity_ptr, RR_MAX_IDENTITY_SIZE);
}
#endif // FEATURE_GSM_EDTM

void rr_ce_set_access_reason(
  byte access_reason,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->access_reason = access_reason;

  if (access_reason == (byte) MM_RR_EMERGENCY_CALL)
  {
    rr_conn_est_info_ptr->emergency_call = TRUE;
  }
  else
  if (access_reason != (byte) MM_RR_CALL_REEST)
  {
    rr_conn_est_info_ptr->emergency_call = FALSE;
  }

} /* end rr_ce_set_access_reason() */

void rr_ce_set_egprs_radio_priority(
  uint8 egprs_radio_priority,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->egprs_radio_priority = (byte) egprs_radio_priority;
}

void rr_ce_set_pkt_est_cause(
  rr_pkt_est_cause_t pkt_est_cause,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->pkt_est_cause = pkt_est_cause;

  /* publish the access reason to the GCOMMON utilities */
  switch(pkt_est_cause)
  {
    case RR_PKT_EST_CAUSE_USER_DATA:
    case RR_PKT_EST_CAUSE_PAGE_RESP:
      geran_set_traffic_reason(GERAN_TRAFFIC_PS_DATA, gas_id);
      break;

    case RR_PKT_EST_CAUSE_CELL_UPD:
    case RR_PKT_EST_CAUSE_MM:
      geran_set_traffic_reason(GERAN_TRAFFIC_PS_SIGNALLING, gas_id);
      break;

    case RR_PKT_EST_CAUSE_UNKNOWN:
    default:
      geran_set_traffic_reason(GERAN_TRAFFIC_PS_OTHER, gas_id);
      break;
  }

  return;
}

/**
 *
 * @return pkt_est_cause for current connection establishment attempt
 */
rr_pkt_est_cause_t rr_ce_get_pkt_est_cause(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  return rr_conn_est_info_ptr->pkt_est_cause;
} /* rr_ce_access_reason */

/**
 *
 * @return access reason for current connection establishment attempt
 */
byte rr_ce_access_reason(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  return rr_conn_est_info_ptr->access_reason;
} /* rr_ce_access_reason */

byte rr_ce_egprs_radio_priority(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  return rr_conn_est_info_ptr->egprs_radio_priority;
}

boolean rr_ce_egprs_access_valid(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  return rr_conn_est_info_ptr->egprs_access_valid;
}

void rr_ce_reset_egprs_access(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->egprs_access_valid = FALSE;

} /* end rr_ce_reset_egprs_access() */


/**
 *
 * @return TRUE if access is for a GPRS reason (extend the macro below for EDGE)
 */
boolean rr_ce_for_gprs_access(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  return IS_GPRS_ACCESS_REASON(rr_conn_est_info_ptr->access_reason);
}

boolean rr_ce_for_emergency_call(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  return rr_conn_est_info_ptr->emergency_call;
} /* end rr_ce_for_emergency_call() */

/**
 * Uses the given system information source to determine the value to be used for
 * setting T3126 or T3146 (in milliseconds)
 *
 * @param sys_info_ptr pointer to database source to use to compute timer value
 *
 * @return computed timeout value in milliseconds
 */
word rr_compute_rach_timeout_value(system_information_database_T *sys_info_ptr)
{
  boolean combined;
  int     T_value;
  int     S_value;
  int     timer_value;

  /***************************************************/
  /* The MINIMUM value is equal to the time taken by */
  /* T + 2S slots and the MAXIMUM value is 5 seconds */
  /* See 3GPP 04.08 11.1.1                           */
  /***************************************************/

  /* determine whether the control channel is combined */
  if (sys_info_ptr->control_channel_description.BS_CCCH_SDCCH_COMB == 1)
  {
    combined = TRUE;
  }
  else
  {
    combined = FALSE;
  }

  /* extract the TX integer value from the serving cell info */
  T_value = sys_info_ptr->RACH_control_parameters.tx_integer;

  /* use combined and T_value to determine the S parameter */
  /* lookup values based on 3GPP 04.18 table 3.3.1.1.2.1   */
  switch (T_value)
  {
    case 3:
    case 8:
    case 14:
    case 50:
      if (!combined)
      {
        S_value = 55;
      }
      else
      {
        S_value = 41;
      }
      break;

    case 4:
    case 9:
    case 16:
      if (!combined)
      {
        S_value = 76;
      }
      else
      {
        S_value = 52;
      }
      break;

    case 5:
    case 10:
    case 20:
      if (!combined)
      {
        S_value = 109;
      }
      else
      {
        S_value = 58;
      }
      break;

    case 6:
    case 11:
    case 25:
      if (!combined)
      {
        S_value = 163;
      }
      else
      {
        S_value = 86;
      }
      break;

    case 7:
    case 12:
    case 32:
      if (!combined)
      {
        S_value = 217;
      }
      else
      {
        S_value = 115;
      }
      break;

    default:
      S_value = 0;
      ERR("Bad value(%d) for tx_integer",(int)T_value,0,0);
      break;
  }

  /* NOTE: a timeslot takes just over 0.5 milliseconds,  */
  /* so the scaling factor used below spreads the result */
  /* between the minimum and maximum values of the timer */
  /* This scaling factor should be between 1 and 10      */
  timer_value = (T_value + 2*S_value) * RACH_TIMEOUT_SCALING_FACTOR;

  /* maximum value is 5 seconds */
  if (timer_value > 5000)
  {
    timer_value = 5000;
  }

  return((word)timer_value);
} /* rr_compute_rach_timeout_value */

/**
 * Encodes the paging response OTA message into the given layer3_message buffer
 *
 * @param cipher_key_sequence_number
 *                           cipher key sequence number value to use while encoding the message
 *
 * @param mobile_identity_ie mobile identity IE that was used to page the mobile
 *
 * @param classmark_2        classmark 2 IE (encoded)
 *
 * @param layer3_message     [out] buffer where the layer 3 message for PAGING RESPONSE should be encoded
 *
 * @param message_length_ptr [out] set to the length of the encoded layer3 message
 */
static void rr_encode_paging_response(
  byte              *mobile_identity_ie,
  classmark_2_ie_T   classmark_2,
  byte              *layer3_message,
  byte              *message_length_ptr,
  const gas_id_t     gas_id
)
{
  byte index;
  byte i;
  byte identity_length;
  byte paging_response[RR_PAGING_RESPONSE_LENGTH];

  index = 0;

  /************************************************************/
  /* Encode the Protocol Discriminator and the Skip Indicator */
  /* The Skip Indicator is set to 0                           */
  /************************************************************/
  paging_response[index] = RADIO_RESSOURCES;
  index++;

  /************************************************/
  /* Encode the Message Type as a PAGING_RESPONSE */
  /************************************************/

  paging_response[index] = PAGING_RESPONSE;
  index++;

  /************************************************/
  /* Encode the Ciphering Key Sequence Number and */
  /* the Spare Half Octet                         */
  /************************************************/

  paging_response[index] = rr_get_cipher_key_sequence_number(gas_id);
  index++;

  /*****************************************/
  /* Encode the mobile station classmark 2 */
  /*****************************************/

  paging_response[index] = classmark_2.length;
  index++;

  for (i = 0; i < 3; i++)
  {
    paging_response[index] = classmark_2.data[i];
    index++;
  }

  /**************************************************/
  /* Encode the mobile identity information element */
  /**************************************************/
  identity_length = mobile_identity_ie[0];

  for (i = 0; i < identity_length + 1; i++)
  {
    if (index + i < RR_PAGING_RESPONSE_LENGTH)
    {
      paging_response[index + i] = mobile_identity_ie[i];
    }
  }

  *message_length_ptr = (byte) ( 7 + identity_length + 1);

  for (i = 0; i < *message_length_ptr; i++)
  {
    layer3_message[i] = paging_response[i];
  }
} /* rr_encode_paging_response */

/**
 * Determines the size of the Layer3 message inside the rr_est_req message sent by
 * MM
 *
 * @param messagePtr pointer to RR_EST_REQ message sent by MM
 *
 * @return size of L3 message in RR_EST_REQ sent by MM
 */
byte rr_get_rr_est_req_L3MsgSize(rr_est_req_T *messagePtr)
{
  ASSERT(messagePtr != NULL);

  /* the maximum allowed size for L3 messages in RR_EST_REQ is N201_MAX since
   * they must be sent in SABM frame to perform contention resolution
   */
  return MIN(messagePtr->l3_message_size, N201_MAX);
} /* rr_get_rr_est_req_L3MsgSize */

/**
 * Construct a Paging Response message (used by EDTM and non-L2G builds)
 *
 * @param l3_msg_ptr - pointer to output buffer
 *
 * @return number of octets in output buffer
 */
static byte rr_construct_paging_response(byte *l3_msg_ptr, const gas_id_t gas_id)
{
  classmark_2_ie_T classmark_2;
  byte             msg_len = 0;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  /**************************************************/
  /* Use the classmark module to encode Classmark 2 */
  /**************************************************/
   if ((l3_msg_ptr == NULL) ||
       (rr_encode_classmark(RR_CLASSMARK_2_ONLY, &classmark_2, NULL, gas_id) == FALSE))
   {
     return(0);
   }

  /******************************************************/
  /* Select the same same mobile identity type received */
  /* from the network directly, or via the EDTM module  */
  /****************************************************(*/
  rr_encode_paging_response(
    rr_conn_est_info_ptr->paged_identity,
    classmark_2,
    l3_msg_ptr,
    &msg_len,
    gas_id
  );

  return(msg_len);
}

/**
 * Sends a DL_ESTABLISH_REQ message to L2 to establish the SAPI0 link using the
 * contention resolution procedure. The SABM frame sent to the network will
 * carry the first layer3 message exchanged with the network.
 *
 * In case of a mobile terminated call, this will be a page response
 *
 * In case of a mobile originated call, it will be the L3 message sent by MM
 * in the RR_EST_REQ message.
 */
static void rr_ce_send_first_message(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

#ifndef FEATURE_LTE
  /* determine whether RR should send a PAGING RESPONSE message */
  if (rr_ce_access_reason(gas_id) == (byte) MM_RR_ANSWER_TO_PAGING)
  {
    byte layer3_message[N201_MAX];
    byte message_length;

    /* RR should send a PAGE RESPONSE message to the network */
    message_length = rr_construct_paging_response(layer3_message, gas_id);

    MSG_GERAN_HIGH_0_G("OMsg(NW,ANSWER_TO_PAGING)");
    rr_send_dl_establish_req(
      SAPI0,
      DCCH,
      layer3_message,
      message_length,
      CONTENTION_RESOLUTION,
      gas_id
    );
  }
  else /* access reason something other than answer to paging */
#endif /* !FEATURE_LTE */
  {
    /****************************************************************/
    /* Use the saved layer 3 message received from MM in RR_EST_REQ */
    /****************************************************************/
    MSG_GERAN_HIGH_1_G("OMsg(NW,1stL3Msg) Length %d",rr_conn_est_info_ptr->cm_service_request_msg_length);
    rr_send_dl_establish_req(
      SAPI0,
      DCCH,
      rr_conn_est_info_ptr->service_request_message,
      rr_conn_est_info_ptr->cm_service_request_msg_length,
      CONTENTION_RESOLUTION,
      gas_id
    );
  }

  /*******************************************************/
  /* Reset the ciphering flags for the new RR connection */
  /*******************************************************/
  rr_set_ciphering_mode_command_received(FALSE, gas_id);

  return;
} /* rr_ce_send_first_message */

/**
 * Called once the SAPI0 link has been established. Sends the appropriate message
 * to MM and performs Early Classmark Sending if appropriate
 */
static rr_event_T rr_ce_handle_communication_establishment(const gas_id_t gas_id)
{
  if (rr_ce_access_reason(gas_id) == (byte) MM_RR_ANSWER_TO_PAGING)
  {
    MSG_GERAN_HIGH_0_G("RR MT connection established");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("RR MO connection established");
  }

  /* a SAPI0 link has been established */
  rr_set_link_status(SAPI0_LINK, gas_id);

  /* Perform Early Classmark Sending if required by the network */
  rr_perform_classmark_sending(RR_CLASSMARK_CLIENT_ECSC, gas_id);

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
  if (rr_gprs_edtm_cs_establishment_in_progress(gas_id))
  {
    rr_gprs_edtm_complete_cs_establishment(gas_id);

    if (rr_ce_conn_is_requested_by_nas(gas_id))
    {
      /* send the establishment confirm to MM */
      rr_send_est_cnf(gas_id);
    }
    else
    {
      rr_initialize_nas_message_buffer(gas_id);
      rr_send_est_ind(gas_id);
    }
  }
  else
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
  {
    /* Suspend GPRS in GMM */
    rr_gmm_suspend_gprs(gas_id);

    /* send the establishment confirm to MM */
    rr_send_est_cnf(gas_id);
  }

  rr_ce_conn_confirmed_to_nas(gas_id);

  return(EV_RR_EST_CNF);
} /* rr_handle_communication_establishment */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/**
 * Determines whether state machine can safely process RR_PS_ABORT_REQ
 *
 */
boolean rr_ce_can_process_ps_abort(const gas_id_t gas_id)
{
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  if (RR_CP_SENDING_RANDOM_ACCESS == rr_conn_est_info_ptr->con_est_state)
  {
    if (rr_ce_for_gprs_access(gas_id) && rr_timer_is_running(GRR_GUARD_TIMER, gas_id))
    {
      /* L1 has not yet responded to the first random access request */
      /* allow that to complete before trying to abort packet access */
      return(FALSE);
    }
  }

  return(TRUE);
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/**
 * Abort the Immediate Assignment procedure by sending the RR_ABORT_IND to MM
 * only if IA was initiated by MM and not as a response to a PagingRequest).
 *
 * @param RR_abort_cause reason the immediate assignment procedure is being aborted
 *
 * @param MM_fail_cause failure cause (may indicate failure caused by radio availability)
 *
 * @param return_to_idle TRUE if RR needs to return to idle mode, FALSE if already in idle mode
 *
 * @return rr_event a event to trigger a state transition in rr_control
 *
 */
static rr_event_T rr_ce_abort_immed_assign_procedure(
  byte RR_abort_cause,
  mm_rr_failure_cause_e_type MM_fail_cause,
  boolean return_to_idle,
  const gas_id_t gas_id
)
{
  /* cancel T3126 timer if it is running */
  if (rr_timer_cancel(T3126, gas_id))
  {
    MSG_GERAN_HIGH_0_G("Stopped T3126");
  }

  /* MM should be notified of CS immediate assignment procedure abort */
  if (rr_x2g_cco_in_progress(gas_id) == FALSE)
  {
    rr_send_abort_ind(RR_abort_cause, MM_fail_cause, gas_id);
  }
  MSG_GERAN_HIGH_3_G("Abort IA proc.(cause=%d, fail=%d, back to idle=%d)",
                     (int)RR_abort_cause,
                     (int)MM_fail_cause,
                     (int)return_to_idle);

  /* RR should start idle mode on the cell */
  if (return_to_idle)
  {
    /* Cleanup; Don't notify MM of release/abort */
    rr_conn_est_failed(FALSE, gas_id);

    return EV_START_IDLE;
  }
  else /* RR should not return to idle (could already be in idle) */
  {
    if (rr_is_camped(gas_id))
    {
      gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

      if (scell_info_ptr != NULL)
      {
        /* restore the Cell Channel Description */
        scell_info_ptr->gsm.cell_channel_description = scell_info_ptr->gsm.SI_cell_channel_description;
      }

      /* already in idle, determine whether to transfer control to GRR */
      return(EV_GO_TO_IDLE);
    }
    else /* RR is no longer camped */
    {
      /*********************************************************************************/
      /* At this point, RR is considered to have lost service since the immediate      */
      /* assignment procedure has failed. RR should return to idle as soon as possible */
      /* to start listening for pages once again                                       */
      /*********************************************************************************/
      MSG_GERAN_HIGH_0_G("Conn failed and not camped");

      /* log the loss of service */
      /*rr_log_service_change(FALSE, gas_id) */
    }
  }

  return(EV_NO_EVENT);
} /* rr_ce_abort_immed_assign_procedure */


/** 
* Determines whether a cell should be forbidden
*
* * @param con_estevent : The conn establishment event
*
* * @return void
*/

static void rr_ce_handle_scell_dsf_event( const rr_event_T con_est_event, const gas_id_t gas_id )
{
  if (con_est_event != EV_DSF_FOR_DENIALS)
  {   
    rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
    
    rr_start_forbidden_timer(gas_id);
    
  }
}/* rr_ce_handle_scell_dsf_event */


/**
 * Called to release the physical channel allocated during the immediate assignment
 * procedure. Sends a MPH_CHANNEL_RELEASE_REQ to L1.
 *
 * @param repeat_assignment_procedure
 *               indicates if the IA procedure is being repeated or not
 */
static void rr_ce_perform_ia_connection_release(
  boolean repeat_assignment_procedure,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  (void) repeat_assignment_procedure; /* not currently used */

  /* always attempt to camp on the serving cell again */
  rr_send_mph_channel_release_req(
    TRUE,
    scell_info_ptr->gsm.BCCH_ARFCN,
    scell_info_ptr->gsm.BSIC,
    gas_id
  );
} /* rr_ce_perform_ia_connection_release */

static void rr_start_t3126(const gas_id_t gas_id)
{
  if (!rr_timer_is_running(T3126, gas_id))
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    word t3126_timeout_ms = rr_compute_rach_timeout_value(&scell_info_ptr->gsm);

    MSG_GERAN_HIGH_1_G("Starting T3126(%d)",(int) t3126_timeout_ms);
    (void)rr_timer_start(T3126,(dword)(t3126_timeout_ms),gas_id);
  }

} /* end rr_start_t3126() */

/**
 * Reset all CS access restrictions
 *
 * @param NONE
 *
 * @return VOID
 */
static void rr_reset_t3122_info(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->t3122_info.t3122_rejected_cause = (byte)MM_RR_NO_CNM_SERVICE;
  rr_conn_est_info_ptr->t3122_info.t3122_serving_cell.num  = INVALID_ARFCN;
  rr_conn_est_info_ptr->t3122_info.t3122_serving_cell.band = SYS_BAND_NONE;
  rr_conn_est_info_ptr->t3122_info.t3122_serving_bsic.BS_colour_code = INVALID_BSIC_CC;
  rr_conn_est_info_ptr->t3122_info.t3122_serving_bsic.PLMN_colour_code = INVALID_PLMN_CC;

} /* end rr_reset_t3122_info() */

void rr_stop_t3122(const gas_id_t gas_id)
{
  (void)rr_timer_cancel(T3122, gas_id);
  rr_reset_t3122_info(gas_id);

} /* end rr_stop_t3122() */

/** Reset the contents of the request reference array
 *
 */
void rr_reset_rach_references(const gas_id_t gas_id)
{
  byte index;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  for (index = 0; index < RR_REQ_REF_ARRAY_SIZE; index++)
  {
    rr_conn_est_info_ptr->request_references[index].access_reference = 0;
    rr_conn_est_info_ptr->request_references[index].frame_number     = RR_CE_INVALID_RACH_FRAME_NUMBER;
    rr_conn_est_info_ptr->request_references[index].egprs_access_reference = 0;
  }

  return;
}

/* Generate an access reference as specified in 3GPP 44.018 section 9.1.8 */
byte rr_ce_generate_access_reference(
  byte access_reason,
  byte *rand_length_ptr,
  const gas_id_t gas_id
)
{
  boolean rr_hr_data_supported;
  byte    access_reference        = 0x00;
  byte    random_reference_length = 0;
  byte    channel_needed;
  byte    NECI = 0;
  byte    rr_ms_capability;
  uint8   random_bits;        
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  channel_needed = rr_conn_est_info_ptr->channel_needed;

  NECI = scell_info_ptr->gsm.cell_selection_parameters.NECI;

  /* if caller has requested the random length, generate the random bits now */
  if (rand_length_ptr != NULL)
  {
    
    if (secapi_get_random(SECAPI_SECURE_RANDOM, &random_bits, 1) != E_SUCCESS)
    {
      ERR_GERAN_FATAL_0_G("ERROR secapi_get_random() returned FAILURE");
    }
  }
  else
  {
    /* otherwise, the random bits will always be set to zero    */
    /* this will be the case when the MS sends a Pkt CS Request */
    random_bits = 0;
  }

  /* query the current capabilities */
  rr_get_ms_capabilities(&rr_ms_capability, &rr_hr_data_supported, gas_id);

  /*****************************************************************/
  /* See GSM04.08 9.1.8 for establishment cause mappings to access */
  /* reference and random reference lengths                        */
  /*****************************************************************/
  if (access_reason == (byte) MM_RR_EMERGENCY_CALL)
  {
    access_reference |= 0xA0;

    /*******************************************************/
    /* Get a random number which will be used as a random  */
    /* reference. This has to be between 0 and 31 (5 bits) */
    /*******************************************************/

    access_reference |= random_bits & 0x1f; 
    random_reference_length = 5;
  }
  else if (access_reason == (byte) MM_RR_CALL_REEST)
  {
    uint8 num_of_ded_channels;
    channel_type_T channel_type_1;
    channel_type_T channel_type_2;

    num_of_ded_channels = rr_cell_reest_get_channel_info(
      &channel_type_1,
      &channel_type_2,
      gas_id
    );

    if (num_of_ded_channels == 1)
    {
      if ((channel_type_1 == FULL_RATE_TRAFFIC) ||
          ((channel_type_1 == HALF_RATE_TRAFFIC) && (NECI == 0)))
      {
        access_reference |= 0xC0;

        /*******************************************************/
        /* Get a random number which will be used as a random  */
        /* reference. This has to be between 0 and 31 (5 bits) */
        /*******************************************************/

        access_reference |= random_bits & 0x1f; 
        random_reference_length = 5;
      }

      else if ((channel_type_1 == HALF_RATE_TRAFFIC) && (NECI == 1))
      {
        access_reference |= 0x68;

        /*******************************************************/
        /* Get a random number which will be used as a random  */
        /* reference. This has to be between 0 and 3 (2 bits)  */
        /*******************************************************/

        access_reference |= random_bits & 03; 
        random_reference_length = 2;
      }
    }
    else if (num_of_ded_channels == 2)
    {
      if ((channel_type_1 == HALF_RATE_TRAFFIC) &&
          (channel_type_2 == HALF_RATE_TRAFFIC) &&
          (NECI == 1))
      {
        access_reference |= 0x6C;

        /*******************************************************/
        /* Get a random number which will be used as a random  */
        /* reference. This has to be between 0 and 3 (2 bits)  */
        /*******************************************************/

        access_reference |= random_bits & 03; 
        random_reference_length = 2;
      }
    }
    else /* num_of_ded_channels != 1 or 2 */
    {
      /* this should never happen */
      MSG_GERAN_ERROR_1_G("num_of_ded_channels=%d",(int)num_of_ded_channels);
    }
  } /* MM_RR_CALL_REEST */
  else if (access_reason == (byte) MM_RR_ANSWER_TO_PAGING)
  {
    if (channel_needed == ANY_CHANNEL)
    {
      access_reference |= 0x80;

      /*******************************************************/
      /* Get a random number which will be used as a random  */
      /* reference. This has to be between 0 and 31 (5 bits) */
      /*******************************************************/

      access_reference |= random_bits & 0x1f; 
      random_reference_length = 5;
    }
    else if (channel_needed == SDCCH_NEEDED)
    {
      access_reference |= 0x10;

      /*******************************************************/
      /* Get a random number which will be used as a random  */
      /* reference. This has to be between 0 and 15 (4 bits) */
      /*******************************************************/

      access_reference |= random_bits & 0x0f; 
      random_reference_length = 4;
    }
    else if (channel_needed == FULL_RATE_TRAFFIC)
    {
      if (rr_ms_capability == FULL_RATE_ONLY)
      {
        access_reference |= 0x80;

        /*******************************************************/
        /* Get a random number which will be used as a random  */
        /* reference. This has to be between 0 and 31 (5 bits) */
        /*******************************************************/

        access_reference |= random_bits & 0x1f; 
        random_reference_length = 5;
      }
      else if (rr_ms_capability == DUAL_RATE_SUPPORTED)
      {
        access_reference |= 0x20;

        /*******************************************************/
        /* Get a random number which will be used as a random  */
        /* reference. This has to be between 0 and 15 (4 bits) */
        /*******************************************************/

        access_reference |= random_bits & 0x0f; 
        random_reference_length = 4;
      }
      else if (rr_ms_capability == SDCCH_ONLY)
      {
        access_reference |= 0x10;

        /*******************************************************/
        /* Get a random number which will be used as a random  */
        /* reference. This has to be between 0 and 15 (4 bits) */
        /*******************************************************/

        access_reference |= random_bits & 0x0f; 
        random_reference_length = 4;
      }
    }
    else if (channel_needed == FULL_RATE_OR_HALF_RATE_TRAFFIC)
    {
      if (rr_ms_capability == FULL_RATE_ONLY)
      {
        access_reference |= 0x80;

        /*******************************************************/
        /* Get a random number which will be used as a random  */
        /* reference. This has to be between 0 and 31 (5 bits) */
        /*******************************************************/

        access_reference |= random_bits & 0x1f; 
        random_reference_length = 5;
      }
      else if (rr_ms_capability == DUAL_RATE_SUPPORTED)
      {
        access_reference |= 0x30;

        /*******************************************************/
        /* Get a random number which will be used as a random  */
        /* reference. This has to be between 0 and 15 (4 bits) */
        /*******************************************************/

        access_reference |= random_bits & 0x0f; 
        random_reference_length = 4;
      }
      else if (rr_ms_capability == SDCCH_ONLY)
      {
        access_reference |= 0x10;

        /*******************************************************/
        /* Get a random number which will be used as a random  */
        /* reference. This has to be between 0 and 15 (4 bits) */
        /*******************************************************/

        access_reference |= random_bits & 0x0f; 
        random_reference_length = 4;
      }
    }
  }
  else if (((access_reason == (byte) MM_RR_MO_FULL_SPEECH) && (rr_ms_capability == FULL_RATE_ONLY)) ||
           ((access_reason == (byte) MM_RR_MO_FULL_SPEECH) && (NECI == 0) && (rr_ms_capability == DUAL_RATE_SUPPORTED)) ||
           ((access_reason == (byte) MM_RR_MO_HALF_SPEECH) && (rr_ms_capability == FULL_RATE_ONLY)) ||
           ((access_reason == (byte) MM_RR_MO_DATA_9600)   && (rr_ms_capability == FULL_RATE_ONLY)) ||
           ((access_reason == (byte) MM_RR_MO_DATA_4800)   && (rr_ms_capability == FULL_RATE_ONLY)) ||
           ((access_reason == (byte) MM_RR_MO_DATA_2400)   && (rr_ms_capability == FULL_RATE_ONLY)) ||
           ((access_reason == (byte) MM_RR_MO_HALF_SPEECH) && (NECI == 0)) ||
           ((access_reason == (byte) MM_RR_MO_DATA_9600)   && (NECI == 0)) ||
           ((access_reason == (byte) MM_RR_MO_DATA_4800)   && (NECI == 0)) ||
           ((access_reason == (byte) MM_RR_MO_DATA_2400)   && (NECI == 0)) ||
           ((access_reason == (byte) MM_RR_SHORT_MESSAGE)  && (NECI == 0)) ||
           ((access_reason == (byte) MM_RR_IMSI_DETACH)    && (NECI == 0)) ||
           ((access_reason == (byte) MM_RR_SUPPLEMENTARY_SERVICE) && (NECI == 0)))
  {
    access_reference |= 0xE0;

    /*******************************************************/
    /* Get a random number which will be used as a random  */
    /* reference. This has to be between 0 and 31 (5 bits) */
    /*******************************************************/

    access_reference |= random_bits & 0x1f; 
    random_reference_length = 5;
  }
  else if (((access_reason == (byte) MM_RR_MO_FULL_SPEECH) || (access_reason == (byte) MM_RR_MO_HALF_SPEECH))  &&
           (NECI == 1) && (rr_ms_capability == DUAL_RATE_SUPPORTED))
  {
    access_reference |= 0x40;

    /*******************************************************/
    /* Get a random number which will be used as a random  */
    /* reference. This has to be between 0 and 15 (4 bits) */
    /*******************************************************/

    access_reference |= random_bits & 0x0f; 
    random_reference_length = 4;
  }
  else if (((access_reason == (byte) MM_RR_MO_DATA_9600) || (access_reason == (byte) MM_RR_MO_DATA_4800) ||
           (access_reason == (byte) MM_RR_MO_DATA_2400)) && (NECI == 1) &&
           (rr_ms_capability == DUAL_RATE_SUPPORTED) && (rr_hr_data_supported))
  {
    access_reference |= 0x50;

    /*******************************************************/
    /* Get a random number which will be used as a random  */
    /* reference. This has to be between 0 and 15 (4 bits) */
    /*******************************************************/

    access_reference |= random_bits & 0x0f; 
    random_reference_length = 4;
  }
  else if (((access_reason == (byte) MM_RR_MO_DATA_9600) || (access_reason == (byte) MM_RR_MO_DATA_4800) ||
           (access_reason == (byte) MM_RR_MO_DATA_2400))  && (!rr_hr_data_supported) &&
           (NECI == 1) && (rr_ms_capability == DUAL_RATE_SUPPORTED))
  {
    access_reference |= 0xE0;

    /*******************************************************/
    /* Get a random number which will be used as a random  */
    /* reference. This has to be between 0 and 15 (4 bits) */
    /*******************************************************/

    access_reference |= random_bits & 0x0f; 
    random_reference_length = 4;
  }
  else if ((access_reason == (byte) MM_RR_LOCATION_UPDATE) && (NECI == 0))
  {
    /*******************************************************/
    /* Get a random number which will be used as a random  */
    /* reference. This has to be between 0 and 31 (5 bits) */
    /*******************************************************/

    access_reference |= random_bits & 0x1f; 
    random_reference_length = 5;
  }
  else if ((access_reason == (byte) MM_RR_LOCATION_UPDATE) && (NECI == 1))
  {
    /*******************************************************/
    /* Get a random number which will be used as a random  */
    /* reference. This has to be between 0 and 15 (4 bits) */
    /*******************************************************/

    access_reference |= random_bits & 0x0f; 
    random_reference_length = 4;
  }
  else if (((access_reason == (byte) MM_RR_SHORT_MESSAGE)         && (NECI == 1)) ||
           ((access_reason == (byte) MM_RR_IMSI_DETACH)           && (NECI == 1)) ||
           ((access_reason == (byte) MM_RR_SUPPLEMENTARY_SERVICE) && (NECI == 1)))
  {
    access_reference |= 0x10;

    /*******************************************************/
    /* Get a random number which will be used as a random  */
    /* reference. This has to be between 0 and 15 (4 bits) */
    /*******************************************************/

    access_reference |= random_bits & 0x0f; 
    random_reference_length = 4;
  }

  /* if caller has provided an output pointer, copy over the random length */
  if (rand_length_ptr != NULL)
  {
    *rand_length_ptr = random_reference_length;
  }

  return(access_reference);
}

/* Determine the L2 channel type to use in a DL_RANDOM_ACCESS_REQ */
/* FEATURE_GPRS_FAST_RACH allows the UE to send the first PS RACH */
/* with no delay - under certain conditions */
static l2_channel_type_T rr_ce_get_rach_channel_type(const gas_id_t gas_id)
{
  l2_channel_type_T rach_type;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  /* If the call is CS, l2_channel_type is always set to L2_RACH        */
  /* If the call is PS, pkt paging responses should always use L2_RACH  */
  /* For all other PS calls, the 1st attempt uses L2_RACH_WITH_NO_DELAY */
  if (rr_ce_for_gprs_access(gas_id))
  {
    if ((rr_conn_est_info_ptr->channel_request_counter == 1) &&
        (rr_ce_get_pkt_est_cause(gas_id) != RR_PKT_EST_CAUSE_PAGE_RESP))
    {
      rach_type = L2_RACH_WITH_NO_DELAY;
    }
    else
    {
      rach_type = L2_RACH;
    }
  }
  else
  {
    rach_type = L2_RACH;
  }

  return(rach_type);
}

/**
 * Send the first random access (CHANNEL_REQUEST) message to the network.
 * The access reference is coded based on the access_reason global variable
 * extracted from the RR_EST_REQ (or set to MM_RR_ANSWER_TO_PAGING if access is
 * for a page response)
 *
 * See GSM 04.18 9.1.8
 */
static void rr_ce_initiate_random_access(const gas_id_t gas_id)
{
  rr_event_T guard_timer_expiry_event;
  byte       access_reason;
  byte       access_reference = 0x00;
  byte       rand_length;

  word       egprs_access_reference = 0x0000;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  /* Reset EGPRS access flag before initiating any access */
  rr_conn_est_info_ptr->egprs_access_valid = FALSE;

  rr_conn_est_info_ptr->immd_assign_rej_counter = 0;


  /* the access reason has been stored in this module's local data */
  access_reason = rr_conn_est_info_ptr->access_reason;

  if (IS_STANDARD_GPRS_ACCESS_REASON(access_reason))
  {
    /* For 51 Packet Access, call a GRR function to generate the reference */
    access_reference = grr_get_gprs_access_ref(access_reason, &rand_length);

    guard_timer_expiry_event = EV_RACH_FOR_PS_FAILURE;

    MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for GPRS (reason=%d)",
               (int)access_reference,(int)access_reason);
  }
  else if (IS_ENHANCED_GPRS_ACCESS_REASON(access_reason))
  {
    /* For 51 Packet Access on EDGE, call an EGPRS function to generate the reference */
    egprs_access_reference = grr_get_egprs_access_ref(
      access_reason, &rand_length,
      rr_ce_egprs_radio_priority(gas_id),
      gas_id
    );

    rr_conn_est_info_ptr->egprs_access_valid = TRUE;

    guard_timer_expiry_event = EV_RACH_FOR_PS_FAILURE;

    MSG_GERAN_HIGH_2_G("StartRA(0x%04X) for EGPRS (reason=%d)",
               (int)egprs_access_reference,(int)access_reason);
  }
  else
  {
    /*************************************************************/
    /* We have to form the access reference which corresponds    */
    /* to the establishment cause given by the MM sublayer and   */
    /* the broadcast NECI value, or which corresponds to one of  */
    /* the establishment causes "answer to paging" given by the  */
    /* RR entity in response to a PAGING_REQUEST message         */
    /* including the channel_needed information                  */
    /*************************************************************/
    access_reference = rr_ce_generate_access_reference(access_reason, &rand_length, gas_id);

    guard_timer_expiry_event = EV_RACH_FOR_CS_FAILURE;

    /* publish the access reason to the GCOMMON utilities */
    switch(access_reason)
    {
    case MM_RR_MO_FULL_SPEECH:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_VOICE, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for Full Speech (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_MO_HALF_SPEECH:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_VOICE, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for Half Speech (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_MO_DATA_9600:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_DATA, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for Data 9600 (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_MO_DATA_4800:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_DATA, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for Data 4800 (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_MO_DATA_2400:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_DATA, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for Data 2400 (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_SHORT_MESSAGE:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_DATA, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for SMS (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_SUPPLEMENTARY_SERVICE:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_SIGNALLING, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for Data Supp. Service (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_LOCATION_UPDATE:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_SIGNALLING, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for LAU (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_IMSI_DETACH:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_SIGNALLING, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for IMSI Detach (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_CALL_REEST:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_VOICE, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for Call Reestablishment (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_EMERGENCY_CALL:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_VOICE, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for Emergency Call (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    case MM_RR_ANSWER_TO_PAGING:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_VOICE, gas_id);
      MSG_GERAN_HIGH_3_G("StartRA(0x%02X) for Paging Response (reason=%d, chan=%d)",
               (int)access_reference,(int)access_reason,
               (int)rr_conn_est_info_ptr->channel_needed);
      break;

    case MM_RR_NO_CNM_SERVICE:
      geran_set_traffic_reason(GERAN_TRAFFIC_CS_SIGNALLING, gas_id);
      MSG_GERAN_HIGH_2_G("StartRA(0x%02X) for No CNM Service (reason=%d)",
               (int)access_reference,(int)access_reason);
      break;

    default:
      geran_set_traffic_reason(GERAN_TRAFFIC_DEFAULT, gas_id);
      MSG_GERAN_ERROR_1_G("Unknown access reason=%d", (int)access_reason);
      break;
    }
  }

  if (guard_timer_expiry_event == EV_RACH_FOR_PS_FAILURE)
  {
    // TODO DSDA
    rr_hz_notify_state_change(RR_HZ_STATE_PS_CONNECTION, gas_id);
  }
  else
  {
    rr_hz_notify_state_change(RR_HZ_STATE_CS_CONNECTION, gas_id);
  }

  if (rr_conn_est_info_ptr->egprs_access_valid)
  {
    rr_conn_est_info_ptr->egprs_pending_access_reference = egprs_access_reference;
  }
  else
  {
    /* store the current access reference to associate with dl_random_access_cnf */
    rr_conn_est_info_ptr->pending_access_reference = access_reference;
  }

  /* store the random reference length (in bits) that was used */
  rr_conn_est_info_ptr->random_reference_length = rand_length;

  /**********************************************************************/
  /* channel_request_counter keeps track of the number of random access */
  /* messages sent to the network. Reset to 1 since this is the first   */
  /* such message sent for this immediate assignment procedure          */
  /**********************************************************************/
  rr_conn_est_info_ptr->channel_request_counter = 1;
  rr_conn_est_info_ptr->failed_rach_count       = 0;

  /* reset the release reason */
  rr_conn_est_info_ptr->release_reason = REASON_NOT_AVAILABLE;

  /*************************************/
  /* Initialise the request references */
  /* array to wipe out any existing    */
  /* values from previous IA procedure */
  /*************************************/
  rr_reset_rach_references(gas_id);

  /* Allow 2 secs for L1/L2 to acknowledge that RACHing has started */
  /* See L1_calculate_RA_delay() in l1_rach_if.c for calculation    */
  /* of the maximum delay - it is approximately 1.45 seconds        */
  grr_start_guard_timer(GRR_TIMEOUT_1_SECOND * 2, guard_timer_expiry_event, gas_id);

  if (rr_conn_est_info_ptr->egprs_access_valid)
  {
    /************************************************************************************/
    /* Send the DL_RANDOM_ACCESS_REQ to L2 to send the first EGPRS Packet Channel Req   */
    /* message to the network using the computed egprs_access_reference                 */
    /************************************************************************************/
    rr_send_dl_random_access_req_for_egprs(
      egprs_access_reference,
      rr_ce_get_rach_channel_type(gas_id),
      gas_id
    );
  }
  else
  {
    /************************************************************************************/
    /* Send the DL_RANDOM_ACCESS_REQ to L2 to send the first CHANNEL_REQUEST message to */
    /* the network using the computed access_reference                                  */
    /************************************************************************************/
    rr_send_dl_random_access_req(
      access_reference,
      rr_ce_get_rach_channel_type(gas_id),
      gas_id
    );
  }

  rr_gprs_odm_info_rach_attempt(gas_id);

  return;
} /* rr_ce_initiate_random_access() */

static boolean rr_ce_max_rach_bursts_sent(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  gprs_scell_info_t  *scell_ptr            = rr_gprs_get_scell_info(gas_id);

  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  if (rr_conn_est_info_ptr->channel_request_counter < (scell_ptr->gsm.RACH_control_parameters.max_retrans + 1))
  {
    return(FALSE);
  }

  return(TRUE);
} /* rr_ce_max_rach_bursts_sent() */

/**
 * Stores the request_reference of the last random access message sent to the
 * network in the request_references buffer. If the maximum number of random
 * access messages sent to the network has not been reached, sends a new
 * CHANNEL_REQUEST random access message to the network.
 *
 * @param cnf_ptr - pointer to received DL_RANDOM_ACCESS_CNF signal
 *
 * @return An event to be processed by the connection establishment state machine
 */
static rr_event_T rr_ce_handle_access_confirm(
  const dl_random_access_cnf_T *cnf_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T new_event = EV_NO_EVENT;
  word       frame_number;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  /* response received from lower layers, stop the guard timer */
  grr_stop_guard_timer(gas_id);

  /***********************************************************************************************/
  /* store the random access request reference of the last message sent and shift the other ones */
  /* by one (may loose the oldest reference value, but RR should only keep track of the last 3)  */
  /***********************************************************************************************/
  rr_conn_est_info_ptr->request_references[2] = rr_conn_est_info_ptr->request_references[1];
  rr_conn_est_info_ptr->request_references[1] = rr_conn_est_info_ptr->request_references[0];

  if (rr_conn_est_info_ptr->egprs_access_valid)
  {
    rr_conn_est_info_ptr->request_references[0].egprs_access_reference =
      rr_conn_est_info_ptr->egprs_pending_access_reference;
  }
  else
  {
    rr_conn_est_info_ptr->request_references[0].access_reference =
      rr_conn_est_info_ptr->pending_access_reference;
  }

  /***********************************************************************/
  /* This is the frame number where the last random access message was   */
  /* sent. It is part of the request reference used to match the network */
  /* response                                                            */
  /***********************************************************************/
  frame_number = ((word)(cnf_ptr->frame_number.msb)) << 8;
  frame_number += (word)(cnf_ptr->frame_number.lsb);
  rr_conn_est_info_ptr->request_references[0].frame_number = frame_number;

  /* continue to RACH if the maximum number of bursts have not been sent */
  if (rr_ce_max_rach_bursts_sent(gas_id) == FALSE)
  {
    word random;

    if (secapi_get_random(SECAPI_SECURE_RANDOM, (uint8 *)&random, sizeof(random)) != E_SUCCESS)
    {
      ERR_GERAN_FATAL_0_G("ERROR secapi_get_random() returned FAILURE");
    }

    /* if the current access is for EGPRS Packet Channel Request */
    if (rr_conn_est_info_ptr->egprs_access_valid)
    {
      word egprs_access_ref = 0x0000;

      /* The last sent EGPRS Packet Channel Request is stored at entry 0 of request_references.
         The random element of this msg is re-calculated.  It is then resent */
      egprs_access_ref = rr_conn_est_info_ptr->request_references[0].egprs_access_reference;

      switch (rr_conn_est_info_ptr->random_reference_length)
      {
        case 3:
          egprs_access_ref &= 0x07F8;
          egprs_access_ref |= (random % 8) & 0x07; 
          break;

        case 5:
          egprs_access_ref &= 0x07E0;
          egprs_access_ref |= (random % 32) & 0x1f; 
          break;

        default:
          MSG_GERAN_ERROR_1_G("Bad EGPRS rand ref len %d", rr_conn_est_info_ptr->random_reference_length);
          break;
      }

      rr_conn_est_info_ptr->egprs_pending_access_reference = egprs_access_ref;

      /* log channel_request message before sending next one */
      rr_log_random_access_req(
        rr_conn_est_info_ptr->channel_request_counter,
        frame_number,
        rr_ce_access_reason(gas_id),
        gas_id
      );

      /* one more channel_request message has been sent */
      rr_conn_est_info_ptr->channel_request_counter++;

      rr_send_dl_random_access_req_for_egprs(
        egprs_access_ref,
        rr_ce_get_rach_channel_type(gas_id),
        gas_id
      );
    }
    else
    {
      byte access_reference;

      /**************************************************/
      /* We have to pick up the last channel request    */
      /* sent and to change the random reference in it  */
      /* The last channel request sent is placed in the */
      /* entry 0 of the request_references table        */
      /**************************************************/
      access_reference = rr_conn_est_info_ptr->request_references[0].access_reference;

      /**********************************************************************/
      /* random_reference_length indicates the number of bits in the        */
      /* request_reference (least significant) that are randomly determined */
      /* based on the establishment cause & SI parameters. The value was    */
      /* already determined when the first CHANNEL_REQUEST random           */
      /* access message was sent. Just change the random portion of the     */
      /* access_reference based on this length                              */
      /**********************************************************************/
      switch (rr_conn_est_info_ptr->random_reference_length)
      {
        case 2:
          access_reference &= 0xFC;
          access_reference |= (random % 4); 
          break;

        case 3:
          access_reference &= 0xF8;
          access_reference |= (random % 8); 
          break;

        case 4:
          access_reference &= 0xF0;
          access_reference |= (random % 16); 
          break;

        case 5:
          access_reference &= 0xE0;
          access_reference |= (random % 32); 
          break;

        default:
          /* invalid random_reference_length value */
          MSG_GERAN_ERROR_1_G("Bad ref. length(%d)",(int)rr_conn_est_info_ptr->random_reference_length);
          break;
      } /* switch */

      /* store the access_reference to be placed in the buffer when the CNF is received */
      rr_conn_est_info_ptr->pending_access_reference = access_reference;

      /* log channel_request message before sending next one */
      rr_log_random_access_req(
        rr_conn_est_info_ptr->channel_request_counter,
        frame_number,
        rr_conn_est_info_ptr->access_reason,
        gas_id
      );

      /* one more channel_request message has been sent */
      rr_conn_est_info_ptr->channel_request_counter++;

      rr_send_dl_random_access_req(
        access_reference,
        rr_ce_get_rach_channel_type(gas_id),
        gas_id
      );
    }
  }
  else /* otherwise, the maximum number of CHANNEL_REQUEST messages have been sent */
  {
    /* log channel_request message for the last message sent */
    rr_log_random_access_req(
      rr_conn_est_info_ptr->channel_request_counter,
      frame_number,
      rr_conn_est_info_ptr->access_reason,
      gas_id
    );

    MSG_GERAN_HIGH_1_G("Max(%d) RACH messages sent",(int) rr_conn_est_info_ptr->channel_request_counter);
    /* notify state machine that maximum has been sent */
    new_event = EV_MAX_RETRIES_ATTEMPTED;

    if (IS_GPRS_ACCESS_REASON(rr_conn_est_info_ptr->access_reason))
    {
      grr_start_t3146(gas_id);
    }
    else
    {
      rr_start_t3126(gas_id);
    }
  }

  return(new_event);
} /* rr_ce_handle_access_confirm */

static rr_event_T rr_ce_handle_access_failure(
  const dl_random_access_cnf_T *cnf_ptr,
  const gas_id_t gas_id
)
{
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  /* increment the number of failed transmissions */
  rr_conn_est_info_ptr->failed_rach_count++;

  /* emit debug to describe the failure */
  if (GERAN_ACCESS_DENIED_BY_TRM == cnf_ptr->result)
  {
    MSG_GERAN_HIGH_0_G("RACH burst was not sent OTA, TRM denial");
  }
  else if (GERAN_ACCESS_BLANKED_BY_CXM == cnf_ptr->result)
  {
    MSG_GERAN_HIGH_0_G("RACH burst was not sent OTA, CXM blanking");
  }
  else if (GERAN_ACCESS_IN_IDLE_STATE == cnf_ptr->result)
  {
    /* this should only ever happen for the first RACH attempt */
    rr_conn_est_info_ptr->failed_rach_count--;
    MSG_GERAN_HIGH_1_G("RACH attempt ignored in idle mode, failure count %d", (int)(rr_conn_est_info_ptr->failed_rach_count));
  }
  else if (GERAN_ACCESS_FAIL_NO_RETRY == cnf_ptr->result)
  {
    rr_conn_est_info_ptr->failed_rach_count = RR_MS_MAX_FAILED_RACH_ATTEMPTS;
    MSG_GERAN_HIGH_0_G("RACH burst was not sent OTA, no retry");
  }  
  else
  {
    MSG_GERAN_ERROR_1_G("RACH burst was not sent OTA, result %d", (int)(cnf_ptr->result));
  }

  /* stop the guard timer if the first attempt has been handled */
  /* N.B. this leaves the timer running if L1 rejected the RACH */
  if (rr_conn_est_info_ptr->failed_rach_count != 0)
  {
    grr_stop_guard_timer(gas_id);
  }

  /* IMPORTANT NOTE: GRLC has a timer (RM_TMR_AWAIT_ASS_GUARD_DUR=30s) */
  /* which causes a panic reset if the packet assignment is delayed    */
  /* Need to be careful here to finish the retries before that expiry  */
  if (rr_conn_est_info_ptr->failed_rach_count < RR_MS_MAX_FAILED_RACH_ATTEMPTS)
  {
    /* resend the same request down to Layer 2, no need to recalculate anything */
    if (rr_conn_est_info_ptr->egprs_access_valid)
    {
      rr_send_dl_random_access_req_for_egprs(
        rr_conn_est_info_ptr->egprs_pending_access_reference,
        rr_ce_get_rach_channel_type(gas_id),
        gas_id);
    }
    else
    {
      rr_send_dl_random_access_req(
        rr_conn_est_info_ptr->pending_access_reference,
        rr_ce_get_rach_channel_type(gas_id),
        gas_id);
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("Failed to send RACH after %d attempts", RR_MS_MAX_FAILED_RACH_ATTEMPTS);
    if (rr_ce_for_gprs_access(gas_id))
    {
      return(EV_GPRS_CONNECTION_ABORTION);
    }
    else
    {
      return(EV_CONNECTION_ABORTION);
    }
  }

  return(EV_NO_EVENT);
}

/**
 * Called when an RR_EST_REQ message is received from MM, and stores
 * the L3 message buffer included in that request
 *
 * @param message_ptr pointer to the establishment request message sent by MM
 *
 * @return length of the first message (probably a CM service request)
 */
static byte rr_ce_store_first_message(
  rr_est_req_T *message_ptr,
  const gas_id_t gas_id
)
{
  byte index;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  /* store the L3 message length in a global variable */
  rr_conn_est_info_ptr->cm_service_request_msg_length =
    rr_get_rr_est_req_L3MsgSize(message_ptr);

  /* stored the received L3 message, it will be used later when the SAPI0 link is established */
  for (index = 0; index < rr_conn_est_info_ptr->cm_service_request_msg_length; index++)
  {
    rr_conn_est_info_ptr->service_request_message[index] =
      message_ptr->layer3_message[index];
  }

  return(rr_conn_est_info_ptr->cm_service_request_msg_length);
}

/**
 * Called when an RR_EST_REQ message is received from MM to request
 * the establishment of an RR connection
 *
 * @param message_ptr pointer to the establishment request message sent by MM
 *        output_len_ptr - optional pointer to hold the length of the first message
 *
 * @return event processed by caller
 */
rr_event_T rr_ce_process_est_req(
  rr_est_req_T *message_ptr,
  byte *output_len_ptr,
  const gas_id_t gas_id
)
{
  establishment_cause_T     access_reason;
  access_result_T           access_result;
  rr_event_T                new_event = EV_NO_EVENT;
  byte                      first_msg_length;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  ASSERT(message_ptr != NULL);

  /* store the establishment cause in the message as the access reason for the request */
  access_reason = (establishment_cause_T)message_ptr->establishment_cause;
  rr_conn_est_info_ptr->access_reason = (byte) access_reason;

#ifdef FEATURE_LTE
  rr_conn_est_info_ptr->is_periodic_lau = message_ptr->is_periodic_lau;
#endif /* FEATURE_LTE */

  /* store any L3 message that was sent with the request */
  first_msg_length = rr_ce_store_first_message(message_ptr, gas_id);
  if (output_len_ptr != NULL)
  {
    /* caller has requested an indication of the length */
    *output_len_ptr = first_msg_length;
  }

  /*******************************************/
  /* Verify if the access is allowed for the */
  /* subscriber, ie if the current access    */
  /* class is authorized to access the cell  */
  /*******************************************/
  access_result = rr_check_access_class_for_raching(access_reason, gas_id);

  /* Check to see if the establishment is allowed or should be denied */
  if (access_reason == MM_RR_EMERGENCY_CALL)
  {
    if ((access_result == EMERGENCY_CALLS_ALLOWED) || (access_result == ALL_CALLS_ALLOWED))
    {
      new_event = EV_INIT_ACCESS_PROCEDURE;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("Emergency call not allowed(access class)");
      new_event = EV_ACCESS_CLASS_NOT_ALLOWED;
    }
  }
  else /* access reason other than emergency call */
  {
    if ((access_result == NORMAL_CALLS_ALLOWED) || (access_result == ALL_CALLS_ALLOWED))
    {
      new_event = EV_INIT_ACCESS_PROCEDURE;
    }
    else
    {
      MSG_GERAN_HIGH_0_G("RR ConnEst not allowed(access class)");
      new_event = EV_ACCESS_CLASS_NOT_ALLOWED;
    }
  }

  /* Clear NAS message buffer during start of call */
  if (new_event == EV_INIT_ACCESS_PROCEDURE)
  {
    rr_initialize_nas_message_buffer(gas_id);

    rr_ce_init_conn_info(
      TRUE,    // requested_by_nas
      gas_id
    );
  }

  return new_event;
} /* rr_ce_process_est_req */


#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
void rr_ce_set_edtm_access_parameters(
  byte access_reason,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->request_references[0].frame_number = RR_EDTM_FN;
  rr_conn_est_info_ptr->channel_request_counter            = 1;
  rr_conn_est_info_ptr->access_reason                      = access_reason;

#ifdef FEATURE_LTE
  /* for L2G builds, GRR must encode any paging response itself, */
  /* and store it as though it had been received in a RR_EST_REQ */
  if (access_reason == (byte)MM_RR_ANSWER_TO_PAGING)
  {
    rr_conn_est_info_ptr->cm_service_request_msg_length = rr_construct_paging_response(
      rr_conn_est_info_ptr->service_request_message,
      gas_id
    );
  }
#endif /* FEATURE_LTE */

  return;
}
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

/**
 * Attempts to match the immediate assignment request reference IE against
 * the request references array (assumed of size 3)
 *
 * @param request_references_array
 *                             request reference array
 *
 * @param request_reference_IE received request reference value
 *
 * @param channel_requests_sent
 *                             number of channel request messages sent to the network
 *
 * @return TRUE if a match is found, FALSE otherwise
 */
boolean rr_match_IA_request_reference(
  request_references_T  *request_references_array,
  byte                  *request_reference_IE,
  int                   channel_requests_sent
)
{
  int index;
  byte access_reference;
  word frame_number;

  /* number of request references to check depends on channel requests sent up to the max of 3 */
  int num_req_refs =
    ((channel_requests_sent > RR_REQ_REF_ARRAY_SIZE) ? RR_REQ_REF_ARRAY_SIZE : channel_requests_sent);

  /***********************************/
  /* Decode the request reference IE */
  /***********************************/
  rr_decode_request_reference(request_reference_IE,&access_reference,&frame_number);

  /************************************************************/
  /* Attempt to match it against the request references array */
  /************************************************************/
  for (index = 0; index < num_req_refs; index++)
  {
    if ((request_references_array[index].access_reference == access_reference) &&
        (request_references_array[index].frame_number == frame_number))
    {
      return TRUE;
    }
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
    else if (RR_CE_EDTM_REQUEST(request_references_array[index]))
    {
      MSG_GERAN_HIGH_0("Matched E-DTM request reference");
      return(TRUE);
    }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
  }

  return FALSE;
} /* rr_match_IA_request_reference */


/**
 * Attempts to match the immediate assignment Rest Octets Extended RA against
 * the request references array.
 *
 * @param request_references_array
 *                             request reference array
 *
 *
 * @param channel_requests_sent
 *                             number of channel request messages sent to the network
 *
 * @param extended_ra
 *
 *
 * @return TRUE if a match is found, FALSE otherwise
 */

boolean rr_match_IA_extended_request_reference(
  request_references_T  *request_references_array,
  int                   channel_requests_sent,
  word                  extended_ra,
  word                  frame_number
)
{
  int index;

  /* number of request references to check depends on channel requests sent up to the max of 3 */
  int num_req_refs =
    ((channel_requests_sent > RR_REQ_REF_ARRAY_SIZE) ? RR_REQ_REF_ARRAY_SIZE : channel_requests_sent);

  /************************************************************/
  /* Attempt to match it against the request references array */
  /************************************************************/
  for (index = 0; index < num_req_refs; index++)
  {
    /* Match the Extended RA (5 bit field) with the least significant 5 bits of the EGPRS Packet Channel Request */
    if (( (request_references_array[index].egprs_access_reference & 0x1f) == (extended_ra & 0x1f) ) &&
          (request_references_array[index].frame_number == frame_number))
    {
      return TRUE;
    }
  }

  return FALSE;
}


/**
 * Sets the channel configuration based on the information in the SI database
 * and the values received in the immediate assignment message
 *
 * @param channel_info_after_ptr
 *                            pointer to channel configuration received in IA message
 *
 * @param starting_time_valid starting time value received was valid or not
 *
 * @param starting_time       starting time value
 *
 * @param before_parameters_valid
 *                            before parameters value received was valid or not
 *
 * @param channel_info_before_ptr
 *                            pointer to channel configuration for before starting time
 *
 * @param DTX_indicator       dtx indicator value
 */
static void rr_set_channel_configuration_for_IA(
  channel_information_T         *channel_info_after_ptr,
  boolean                       starting_time_valid,
  word                          starting_time,
  boolean                       before_parameters_valid,
  channel_information_T         *channel_info_before_ptr,
  boolean                       DTX_indicator,
  const gas_id_t                gas_id
)
{
  rr_dedicated_data_T *dedic_ptr;
  channel_spec_T      *chan_spec_ptr;
  gprs_scell_info_t   *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  dedic_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (dedic_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("Unable to set channel config for IA");
    return;
  }
  chan_spec_ptr = rr_get_channel_spec(dedic_ptr, gas_id);

  /* Immediate Assignments do not contain a MultiRate IE, */
  /* so RR's global store can be initialised here as this */
  /* represents the initial entry point to dedicated mode */
  rr_invalidate_multirate_config(&(dedic_ptr->curr_amr_config));

  chan_spec_ptr->number_of_ded_channels = 1;
  chan_spec_ptr->power_level =
    scell_info_ptr->gsm.cell_selection_parameters.MS_TXPWR_MAX_CCH;

  chan_spec_ptr->before_parameters_valid = before_parameters_valid;
  chan_spec_ptr->channel_info_1_before   = *channel_info_before_ptr;
  chan_spec_ptr->channel_info_1_after    = *channel_info_after_ptr;

  /********************************************************************************************/
  /* GSM04.18 section 3.3.1.1.3.1 indicates the channel mode should be set to signalling only */
  /********************************************************************************************/
  chan_spec_ptr->channel_mode_1      = SIGNALLING_ONLY;
  chan_spec_ptr->channel_mode_2      = SIGNALLING_ONLY;
  chan_spec_ptr->DTX_indicator       = DTX_indicator;
  chan_spec_ptr->starting_time_valid = starting_time_valid;
  chan_spec_ptr->starting_time[0]    = starting_time >> 8;
  chan_spec_ptr->starting_time[1]    = starting_time & 255;
  chan_spec_ptr->cipher_flag         = FALSE;
  chan_spec_ptr->cipher_algorithm    = 0;
  rr_ciphering_has_started(FALSE, gas_id);

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
  if (RR_CE_EDTM_REQUEST(rr_conn_est_info_ptr->request_references[0]))
  {
    rr_gprs_edtm_store_channel_needed(channel_info_after_ptr->channel_type, gas_id);
  }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

  /********************************************************************/
  /* Send the RR_SYNC_IND to MM if the IA was an early TCH assignment */
  /********************************************************************/
  if ((channel_info_after_ptr->channel_type == FULL_RATE_TRAFFIC) ||
      (channel_info_after_ptr->channel_type == HALF_RATE_TRAFFIC))
  {
    rr_send_sync_ind(
      MM_RR_RES_ASS,
      channel_info_after_ptr->channel_type,
      SIGNALLING_ONLY,
      channel_info_after_ptr->subchannel,
      gas_id
    );

#ifdef FEATURE_LTE
    if (rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id))
    {
      rr_connection_release_set_g2l_blind_redir_flag(gas_id);
    }
#endif /* FEATURE_LTE */

  }

  /* log the initial channel configuration */
  (void)rr_log_channel_configuration(gas_id);

  return;
} /* rr_set_channel_configuration_for_IA */

/**
 * Validates the Immediate Assignment OTA message and decodes the parameters
 * in the message.
 *
 * @param first_IE_ptr      - (input) pointer to the first IE of the IA message
 *
 * @param before_params_ptr - (output) pointer to "before time" channel info
 *
 * @param after_params_ptr  - (output) pointer to "after time" channel info
 *
 * @param ia_params_ptr     - (output) pointer to block of other IA settings
 *
 * @paran check_reference   - TRUE if the function should match the request ref IE
 *
 * @return RR event to be processed by caller
 */
rr_event_T rr_respond_to_immediate_assignment(
  ie_descriptor_T       *first_IE_ptr,
  channel_information_T *before_params_ptr,
  channel_information_T *after_params_ptr,
  rr_ia_params_T        *ia_params_ptr,
  boolean                check_reference,
  uint8                 *end_of_msg_ptr,
  const gas_id_t         gas_id
)
{
  rr_event_T ia_event = EV_NO_EVENT;
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  if ((before_params_ptr != NULL) &&
      (after_params_ptr  != NULL) &&
      (ia_params_ptr     != NULL) &&
      (first_IE_ptr      != NULL))
  {
    ie_descriptor_T *current_IE_ptr;
    boolean          is_for_mobile;
    boolean          decoding_ok = TRUE;

    /* start at the second IE (Channel Description) */
    current_IE_ptr = first_IE_ptr->next_IE;
    RR_CHECK_MANDATORY_IE(current_IE_ptr, EV_NO_EVENT);

    /* decode the channel description here but validate it later */
    rr_decode_channel_description(
      current_IE_ptr->start_IE,
      after_params_ptr,
      &decoding_ok,
      gas_id
    );

    /* for CS allocation using a single ARFCN, check the band */
    if (after_params_ptr->hopping_flag == FALSE)
    {
      rr_internal_band_T cs_band;

      cs_band = GET_ARFCN_BAND(after_params_ptr->frequency_list.channel[0]);

      if (rr_gapi_is_supported_band((sys_band_T)cs_band, gas_id) == FALSE)
      {
        MSG_GERAN_MED_1_G("CS channel in unsupported band(%d)", (int)cs_band);
        decoding_ok = FALSE;
      }
    }

    /* advance to the next IE (Request Reference) */
    current_IE_ptr = current_IE_ptr->next_IE;
    RR_CHECK_MANDATORY_IE(current_IE_ptr, EV_NO_EVENT);

    if (check_reference == TRUE)
    {
      /*******************************************************************/
      /* Determine if the immediate assignment request reference matches */
      /* any of the last 3 random access (CHANNEL_REQUEST) messages sent */
      /* to the network                                                  */
      /*******************************************************************/
      /* determine whether the ongoing RACH attempt is for PS establishment */
      if (rr_ce_for_gprs_access(gas_id))
      {
        MSG_GERAN_MED_0_G("CS immediate assignment discarded during PS access");
        is_for_mobile = FALSE;
      }
      else
      {
        is_for_mobile = rr_match_IA_request_reference(
                          rr_conn_est_info_ptr->request_references,
                          current_IE_ptr->start_IE,
                          rr_conn_est_info_ptr->channel_request_counter);
      }
    }
    else
    {
      is_for_mobile = TRUE;
    }

    /* process the remainder of the message, since it is addressed to this UE */
    /* (or in the case of EDTM, the addressing has been validated elsewhere)  */
    if (is_for_mobile == TRUE)
    {
      if (check_reference == TRUE)
      {
        MSG_GERAN_HIGH_0_G("Immediate Assignment (CS) is for mobile");
      }

      /* validate the channel description now */
      if ((rr_channel_type_supported(after_params_ptr->channel_type, gas_id) == FALSE) ||
          (decoding_ok == FALSE))
      {
        /* channel description unacceptable, abort the establishment */
        ia_event = EV_CONNECTION_ABORTION;
        decoding_ok = FALSE;
        MSG_GERAN_HIGH_1_G("Channel description is unacceptable (type=%d)",
                 (int)after_params_ptr->channel_type);
      }

      if (decoding_ok == TRUE)
      {
        mobile_allocation_T mobile_allocation = {0};

        /*********************************/
        /* Timing Advance is the next IE */
        /*********************************/
        current_IE_ptr = current_IE_ptr->next_IE;
        RR_CHECK_MANDATORY_IE(current_IE_ptr, EV_NO_EVENT);

        ia_params_ptr->timing_advance = (byte) (*(current_IE_ptr->start_IE) & 0x3F);

        /************************************/
        /* Mobile Allocation is the next IE */
        /************************************/
        current_IE_ptr = current_IE_ptr->next_IE;
        RR_CHECK_MANDATORY_IE(current_IE_ptr, EV_NO_EVENT);

        /* mobile allocation is only needed for hopping channels */
        if (after_params_ptr->hopping_flag == TRUE)
        {
          rr_decode_mobile_allocation(current_IE_ptr->start_IE, &mobile_allocation);
        }

        /************************************************/
        /* Get the list of frequencies for the channel  */
        /* use the cell_channel_description from SIDB   */
        /************************************************/
        if ((after_params_ptr->hopping_flag == TRUE) && (mobile_allocation.no_of_entries == 0))
        {
          decoding_ok = FALSE;
          MSG_GERAN_ERROR_0_G("frequency hopping with 0 frequency in mobile allocation");
        }
        else
        {
            decoding_ok = rr_set_channel_frequency_list_from_mobile_allocation(
            after_params_ptr,
            &mobile_allocation,
            &(scell_info_ptr->gsm.cell_channel_description),
            gas_id
          );
        }

        if (decoding_ok == FALSE)
        {
          MSG_GERAN_ERROR_0_G("Unable to set frequency list from IA allocation");
          ia_event = EV_CONNECTION_ABORTION;
        }
        else
        {
          /************************************************/
          /* Decode the starting time if it is present    */
          /************************************************/
          current_IE_ptr = current_IE_ptr->next_IE;
          RR_CHECK_OPTIONAL_IE(current_IE_ptr, EV_NO_EVENT);

          if (current_IE_ptr->start_IE != NULL)
          {
            rr_decode_starting_time(current_IE_ptr->start_IE,
                                    &(ia_params_ptr->starting_time),
                                    &decoding_ok);
            ia_params_ptr->starting_time_valid = TRUE;

            /******************************************************************/
            /* Since there is a starting time, the channel description before */
            /* starting time values may need to be decoded from IA rest octets*/
            /* The only thing that can change is the MAIO and the frequency   */
            /* list, all the other parameters are the same as the channel     */
            /* info after values                                              */
            /******************************************************************/
            before_params_ptr->channel_type = after_params_ptr->channel_type;
            before_params_ptr->subchannel   = after_params_ptr->subchannel;
            before_params_ptr->TN           = after_params_ptr->TN;
            before_params_ptr->TSC          = after_params_ptr->TSC;
            before_params_ptr->hopping_flag = after_params_ptr->hopping_flag;
            before_params_ptr->HSN          = after_params_ptr->HSN;
          }
          else
          {
            /* no starting time */
            ia_params_ptr->starting_time = 0;
            ia_params_ptr->starting_time_valid = FALSE;
          }

          current_IE_ptr = current_IE_ptr->next_IE;
          RR_CHECK_OPTIONAL_IE(current_IE_ptr, EV_NO_EVENT);

          if (current_IE_ptr->start_IE != NULL)
          {
            boolean ia_rest_valid = FALSE;
            byte    MAIO          = 0;
            boolean irat_ho_info_compressed = FALSE;

            /* 3GPP TS 04.18 Section: 9.1.18.3
             * If the starting time IE is not present,
             * the mobile station shall consider the frequency parameters,
             * before time construction as unnecessary in the message.
             * So, pass starting_time_valid as a parameter
             */
            rr_decode_ia_rest_octets(
              current_IE_ptr->start_IE,
              &ia_rest_valid,
              &MAIO,
              &mobile_allocation,
              ia_params_ptr->starting_time_valid,
              &irat_ho_info_compressed,
              end_of_msg_ptr,
              &decoding_ok
            );

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
            rr_classmark_set_irat_ho_info_compressed(
              irat_ho_info_compressed,
              gas_id
            );
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

            if (ia_rest_valid == TRUE)
            {
              /*********************************************************************/
              /* IA Rest Octets have valid information for the channel description */
              /* before the starting time. Channel should be a hopping channel     */
              /*********************************************************************/
              ia_params_ptr->before_params_valid = TRUE;

              before_params_ptr->MAIO = MAIO;

              /**************************************************************/
              /* Translate the mobile_allocation into a list of frequencies */
              /**************************************************************/
              decoding_ok = rr_set_channel_frequency_list_from_mobile_allocation(
                 before_params_ptr,
                 &mobile_allocation,
                 &(scell_info_ptr->gsm.cell_channel_description),
                 gas_id
               );

              if (decoding_ok == FALSE)
              {
                MSG_GERAN_ERROR_0_G("Unable to set frequency list from IA allocation");
                ia_event = EV_CONNECTION_ABORTION;
              }
            }
            else /* IA rest octets do not include "before time" frequency info */
            {
              ia_params_ptr->before_params_valid = FALSE;
            }
          } /* IA rest octets present */
        } /* mobile allocation was processed correctly */
      } /* CS channel description is valid */

      /* if the OK flag is still TRUE at this point, the assignment is valid */
      if (decoding_ok == TRUE)
      {
        ia_event = EV_IMMEDIATE_ASSIGNMENT;
      }
    } /* IA was addressed to this mobile */
  } /* function input parameters are valid */

  return(ia_event);
}

/**
 * Processes the Immediate Assignment OTA message and decodes the information
 * in the message. If the message is destined to the mobile, initiates the
 * allocation of the physical channel by sending the appropriate message to L1
 *
 * @param own_paging_block indicates if the message was received in the mobile's own paging block or not
 *
 * @param rr_in_idle       indicates if rr is in IDLE mode or not
 *
 * @return event to be processed by calling state machine
 */
rr_event_T rr_ce_process_immediate_assignment(
  boolean own_paging_block,
  boolean rr_in_idle,
  uint8 *end_of_msg_ptr,
  const gas_id_t gas_id
)
{
  ie_descriptor_T *current_IE;
  paging_mode_T    paging_mode;
  rr_event_T       new_event;
  byte             dedicated_mode_or_tbf;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(IMMEDIATE_ASSIGNMENT, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  // Extract the Page Mode
  // A change in Page Mode is checked for after the rest of the message is decoded
  paging_mode = (byte) (*(current_IE->start_IE) & 0x0F);

  /*********************************************************************************/
  /* For GSM, this half octet is spare and would be set to 0.  For GPRS this half  */
  /* octet is 'Dedicated mode or TBF IE.  Check bit 0 to determine which it is     */
  /*********************************************************************************/
  dedicated_mode_or_tbf = (byte) (((*(current_IE->start_IE)) & 0xF0) >> 4);

  if (dedicated_mode_or_tbf & 0x01)
  {
    if (grr_cs_conn_est_in_progress(gas_id))
    {
      MSG_GERAN_HIGH_0_G("GPRS IA ignored as waiting for IA for CS");
      new_event = EV_NO_EVENT;
    }
#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
    else if (rr_macinfo_get_mac_status(gas_id) == MAC_TRANSFER) /* GPRS transfer in progress */
    {
      MSG_GERAN_HIGH_0_G("GPRS IA ignored as MAC is already in TRANSFER");
      new_event = EV_NO_EVENT;
    }
#endif /* FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */
    else
    {
      // Initialisation of control/data structures is required now to enable processing of the message, unless
      // this is the second part of a two-part IA (EGPRS-only).

      /*Check for consistence of action of flag waiting_for_2nd_msg before   */
      /* processing the IA.                                                  */
      if ( (rr_conn_est_info_ptr->waiting_for_2nd_msg == TRUE &&
                 rr_timer_is_running(TMULTIFRM, gas_id) == FALSE) )
      {
        /* Check to see if IA is actually the 2nd part of a 2 part IA From   */
        /* "Dedicated mode or TBF information element IE " 10.5.2.25b.2.     */
        /* for 2nd part : tma = 0, downlink =0, T/D =1 (ie 1st 3 bits should */
        /* be equal to 0x01                                                  */
        MSG_GERAN_HIGH_0_G("GPRS rectifying anomaly detected in 2nd part of IA handling:");

        if ( (dedicated_mode_or_tbf & 0x07) == 0x01 )
        {
          MSG_GERAN_HIGH_1_G("GPRS 2nd part of IA, dedicated_mode_or_tbf %x = ",(dedicated_mode_or_tbf & 0x07));
        }
        else
        {
          MSG_GERAN_HIGH_1_G("GPRS not 2nd part of IA, dedicated_mode_or_tbf %x = ",(dedicated_mode_or_tbf & 0x07));
          rr_tbf_est_discard_data(gas_id);
          rr_conn_est_info_ptr->waiting_for_2nd_msg = FALSE;
        }
      }

      if ((rr_conn_est_info_ptr->waiting_for_2nd_msg == TRUE) && 
          (rr_timer_is_running(TMULTIFRM, gas_id) == FALSE) && 
          (grr_return_tbf_data_structure_pointer(gas_id)== TRUE)
         )
      {
        MSG_GERAN_HIGH_0_G("Waiting_for_2nd message is TRUE and data pointer is NULL so recover connection establish");
        rr_conn_est_info_ptr->waiting_for_2nd_msg = FALSE;
      }

      if (!rr_conn_est_info_ptr->waiting_for_2nd_msg)
      {
        rr_tbf_est_activate_init(gas_id);
      }

      current_IE = current_IE->next_IE;

      new_event = grr_respond_to_gprs_immediate_assignment(
        current_IE,
        dedicated_mode_or_tbf,
        &rr_conn_est_info_ptr->waiting_for_2nd_msg,
        rr_conn_est_info_ptr->channel_request_counter,
        rr_conn_est_info_ptr->request_references,
        gas_id
      );

      if (!rr_conn_est_info_ptr->waiting_for_2nd_msg)
      {
        // If the IA processing is now complete, the processing data can be discarded.
        rr_tbf_est_discard_data(gas_id);
      }

      if (new_event != EV_GPRS_IMMEDIATE_ASSIGNMENT)
      {
        // Check for updates in the Page Mode
        rr_ce_check_for_page_mode_update(
          own_paging_block,
          paging_mode,
          gas_id
        );
      }
    }

    return(new_event);
  }

  /* continue only if RR is NOT in idle mode (i.e. attempting access) */
  if (rr_in_idle == FALSE)
  {
    channel_information_T channel_info_before = {0};
    channel_information_T channel_info_after  = {0};
    rr_ia_params_T        ia_params           = {0};

    new_event = rr_respond_to_immediate_assignment(
      current_IE, /* first IE */
      &channel_info_before,
      &channel_info_after,
      &ia_params,
      TRUE, /* validate the ref */
      end_of_msg_ptr,
      gas_id
    );

    if (new_event == EV_IMMEDIATE_ASSIGNMENT)
    {
      boolean DTX_indicator = FALSE;

      /* Override the DTX indication, as L1 assumes a 'FALSE' until the indication */
      /* given in the dedicated update request */
      rr_dtx_rr_override_on(gas_id);

      /**************************************************************************/
      /* Send a message to L1 to allocate the physical channel described in the */
      /* IA message                                                             */
      /**************************************************************************/
      rr_send_mph_immed_assign_req(
        DTX_indicator,
        ia_params.timing_advance,
        ia_params.starting_time_valid,
        ia_params.starting_time,
        ia_params.before_params_valid,
        &channel_info_before,
        &channel_info_after,
        gas_id
      );


      /* set the channel_configuration based on the values received in IA message */
      rr_set_channel_configuration_for_IA(
        &channel_info_after,
        ia_params.starting_time_valid,
        ia_params.starting_time,
        ia_params.before_params_valid,
        &channel_info_before,
        DTX_indicator,
        gas_id
      );

      /**************************************************************************/
      /* Set current_dedicated_band to the first arfcn in the after time        */
      /* frequency list. After time frequency parameters are always present     */
      /* in IA message. If present, before time frequency parameters fall       */
      /* into the same band as those in after time                              */
      /**************************************************************************/
      rr_set_current_dedicated_band(
        (rr_internal_band_T)channel_info_after.frequency_list.channel[0].band,
        gas_id
      );

      /* output some additional debug */
      if (((ia_params.starting_time_valid == TRUE) && (ia_params.before_params_valid == TRUE))
         &&(channel_info_before.frequency_list.channel[0].band
            != channel_info_after.frequency_list.channel[0].band))
      {
        MSG_GERAN_HIGH_2_G("before time band %d, after time band %d",
                 channel_info_before.frequency_list.channel[0].band,
                 channel_info_after.frequency_list.channel[0].band);
      }
    } /* return event indicates valid assignment */
  }
  else
  {
    /* otherwise, RR is in idle mode and the IA message should not be processed */
    new_event = EV_NO_EVENT;
  }

  if (new_event != EV_IMMEDIATE_ASSIGNMENT)
  {
    // Check for updates in the Page Mode
    rr_ce_check_for_page_mode_update(
      own_paging_block,
      paging_mode,
      gas_id
    );
  }

  return(new_event);
} /* rr_ce_process_immediate_assignment */

/**
 * Processes the Immediate Assignment Extended OTA message and decodes the information
 * in the message. If the message is destined to the mobile, initiates the
 * allocation of the physical channel by sending the appropriate message to L1
 *
 * @param own_paging_block indicates if the message was received in the mobile's own paging block or not
 *
 * @param rr_in_idle       indicates if rr is in IDLE mode or not
 *
 * @return event to be processed by calling state machine
 */
rr_event_T rr_ce_process_immediate_assignment_extended(
  boolean own_paging_block,
  boolean rr_in_idle,
  const gas_id_t gas_id
)
{
  rr_event_T new_event = EV_NO_EVENT;
  ie_descriptor_T *current_IE;
  boolean match_reference_1 = FALSE;
  boolean match_reference_2 = FALSE;
  byte timing_advance_1;
  byte timing_advance_2;
  boolean decoding_ok;
  paging_mode_T paging_mode;
  channel_information_T channel_info_1_after = {0};
  channel_information_T channel_info_2_after = {0};

  /**************************************************************************/
  /* If one of the IA values is meant for the mobile, the correct values to */
  /* use are stored in these variables                                      */
  /**************************************************************************/
  channel_information_T *channel_info_after_ptr = NULL;
  byte *timing_advance_ptr = NULL;
  gprs_scell_info_t     *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(IMMEDIATE_ASSIGNMENT_EXTENDED, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  // Extract the Page Mode
  paging_mode = (paging_mode_T) (*(current_IE->start_IE) & 0x0F);

  if (!rr_in_idle)
  {
    /* determine whether the ongoing RACH attempt is for PS establishment */
    if (rr_ce_for_gprs_access(gas_id))
    {
      MSG_GERAN_HIGH_0_G("IA-EXT discarded during PS access");
      return EV_NO_EVENT;
    }

    /**********************************/
    /* Decode the channel description */
    /**********************************/
    current_IE = current_IE->next_IE;
    rr_decode_channel_description(
      current_IE->start_IE,
      &channel_info_1_after,
      &decoding_ok,
      gas_id
    );

    /**********************************************/
    /* have a look at the first request reference */
    /**********************************************/
    current_IE = current_IE->next_IE;

    if (rr_match_IA_request_reference(
          rr_conn_est_info_ptr->request_references,
          current_IE->start_IE,
          rr_conn_est_info_ptr->channel_request_counter
        ))
    {
      MSG_GERAN_HIGH_0_G("IAEXT is for mobile(ID1)");
      match_reference_1 = TRUE;
    }

    /***********************************/
    /* Decode the first timing Advance */
    /***********************************/
    current_IE = current_IE->next_IE;
    timing_advance_1 = *(current_IE->start_IE) & 0x3f;

    /*****************************************/
    /* Decode the second channel description */
    /*****************************************/
    if (decoding_ok == TRUE)
    {
      current_IE = current_IE->next_IE;

      rr_decode_channel_description(
        current_IE->start_IE,
        &channel_info_2_after,
        &decoding_ok,
        gas_id
      );
    }

    /*********************************************/
    /* have a look at the next request reference */
    /*********************************************/
    current_IE = current_IE->next_IE;

    if (rr_match_IA_request_reference(
          rr_conn_est_info_ptr->request_references,
          current_IE->start_IE,
          rr_conn_est_info_ptr->channel_request_counter
        ))
    {
      MSG_GERAN_HIGH_0_G("IAEXT is for mobile(ID2)");
      match_reference_2 = TRUE;
    }

    /************************************/
    /* Decode the second timing advance */
    /************************************/
    current_IE = current_IE->next_IE;
    timing_advance_2 = *(current_IE->start_IE) & 0x3f;

    if ((match_reference_1 == TRUE) && (decoding_ok == TRUE))
    {
      /***********************************************/
      /* The IMMEDIATE ASSIGNMENT message is for the */
      /* current mobile (use first channel info.)    */
      /***********************************************/
      channel_info_after_ptr  = &channel_info_1_after;
      timing_advance_ptr      = &timing_advance_1;
    } /* match_reference_1 is true */
    else if ((match_reference_2 == TRUE) && (decoding_ok == TRUE))
    {
      /***********************************************/
      /* The IMMEDIATE ASSIGNMENT message is for the */
      /* current mobile (use second channel info.)   */
      /***********************************************/
      channel_info_after_ptr  = &channel_info_2_after;
      timing_advance_ptr      = &timing_advance_2;
    } /* match_reference_2 & decoding OK */

    if (decoding_ok == TRUE && (match_reference_1 == TRUE || match_reference_2 == TRUE))
    {
      mobile_allocation_T mobile_allocation;
      byte DTX_indicator;
      word starting_time = 0;
      boolean starting_time_valid = FALSE;

      /********************************************************************************/
      /* The IAExt message was destined for the mobile and all the information was    */
      /* successfully decoded                                                         */
      /********************************************************************************/
      if (channel_info_after_ptr == NULL)
      {
        MSG_GERAN_ERROR_0_G("channel_info_after_ptr is NULL");
        /* fix it so code doesn't access NULL pointer below */
        channel_info_after_ptr = &channel_info_1_after;
      }

      if (timing_advance_ptr == NULL)
      {
        MSG_GERAN_ERROR_0_G("timing_advance_ptr is NULL");
        /* fix it so code doesn't access NULL pointer below */
        timing_advance_ptr = &timing_advance_1;
      }

      /* move to the next IE(mobile allocation) in case it is needed */
      current_IE = current_IE->next_IE;

      if (channel_info_after_ptr->hopping_flag == TRUE)
      {
        /*********************************************************/
        /* Mobile allocation is only needed for hopping channels */
        /*********************************************************/
        rr_decode_mobile_allocation(current_IE->start_IE, &mobile_allocation);
      }

      /***********************************************************/
      /* Set the list of frequencies for the channel information */
      /***********************************************************/
      decoding_ok = rr_set_channel_frequency_list_from_mobile_allocation(
        channel_info_after_ptr,
        &mobile_allocation,
        &scell_info_ptr->gsm.cell_channel_description,
        gas_id
      );

      if (!decoding_ok)
      {
        new_event = EV_CONNECTION_ABORTION;
      }

      if (decoding_ok)
      {
        channel_information_T dummy_channel_info_before = {0};
        boolean irat_ho_info_compressed = FALSE;

        current_IE = current_IE->next_IE;
        if (current_IE->start_IE != NULL)
        {
          rr_decode_starting_time(current_IE->start_IE, &starting_time, &decoding_ok);

          if (!decoding_ok)
          {
            return EV_NO_EVENT;
          }
          starting_time_valid = TRUE;
        }

        /* move to the next IE(IAX rest octets)  */
        current_IE = current_IE->next_IE;
        if (current_IE->start_IE != NULL)
        {
          /* irat_ho_info_compressed is in the octet after frequency parameters, before time,
           * 1st highest bit
           */
          irat_ho_info_compressed = (boolean) (((current_IE->start_IE[0] ^ 0x2b) & 0x80) >> 7);
        }
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
        rr_classmark_set_irat_ho_info_compressed(irat_ho_info_compressed, gas_id);
#endif /*  FEATURE_WCDMA || FEATURE_GSM_TDS*/

        #if 0
        /*********************************/
        /* set the DTX Indicator         */
        /*********************************/
        if ((scell_info_ptr->gsm.cell_options.DTX == 0) ||
            (scell_info_ptr->gsm.cell_options.DTX == 1))
        {
          DTX_indicator = TRUE;
        }
        else
        {
          DTX_indicator = FALSE;
        }
        #endif

        /* Override the DTX indication, as L1 assumes a 'FALSE' until the indication */
        /* given in the dedicated update request */
        DTX_indicator = FALSE;
        rr_dtx_rr_override_on(gas_id);

        /******************************************************/
        /* Send IA req to L1 to allocate the physical channel */
        /******************************************************/
        rr_send_mph_immed_assign_req(
          DTX_indicator,
          *timing_advance_ptr,
          starting_time_valid,
          starting_time,
          FALSE,                      /* no channel information before */
          &dummy_channel_info_before, /* ignored */
          channel_info_after_ptr,
          gas_id
        );

        rr_set_channel_configuration_for_IA(
          channel_info_after_ptr,
          starting_time_valid,
          starting_time,
          FALSE,                        /* no channel_info_before for IAExt message */
          &dummy_channel_info_before,   /* use a dummy value initialized to 0 */
          DTX_indicator,
          gas_id
        );

        /* Set current_dedicated_band to the first arfcn in the after time
           frequency list. After time frequency parameters are always present
           in IA message. */

        rr_set_current_dedicated_band(
          (rr_internal_band_T)channel_info_1_after.frequency_list.channel[0].band,
          gas_id
        );

        /**************************************************************************/
        /* The immediate assignment was destined for the mobile. RR has requested */
        /* L1 to allocate a physical channel.                                     */
        /**************************************************************************/
        new_event = EV_IMMEDIATE_ASSIGNMENT;
      } /* decoding OK */
    } /* IAExt was destined for the mobile and message was decoded */
  }
  /* otherwise, RR is in idle and the message should not be processed */

  if (new_event != EV_IMMEDIATE_ASSIGNMENT)
  {
    // Check for updates in the Page Mode
    rr_ce_check_for_page_mode_update(
      own_paging_block,
      paging_mode,
      gas_id
    );
  }

  return new_event;
} /* rr_ce_process_immediate_assignment_extended */

/**
 * Processes the Immediate Assignment Reject OTA message and decodes the information
 * in the message. If the message is destined to the mobile, indicates that an
 * immediate assignment was rejected and starts T3122 (and T3126 if not already started)
 *
 * @param own_paging_block indicates if the message was received in the mobile's own paging block or not
 *
 * @param rr_in_idle       indicates if rr is in IDLE mode or not
 *
 * @return event to be processed by calling state machine
 */
rr_event_T rr_ce_process_assignment_reject(
  boolean own_paging_block,
  boolean rr_in_idle,
  const gas_id_t gas_id
)
{
  rr_event_T new_event = EV_NO_EVENT;
  byte request_index;
  byte paging_mode;
  ie_descriptor_T *current_IE;
  boolean match_reference = FALSE;
  ie_descriptor_T  *temp_IE_ptr;
  iar_rest_octets_t iar_rest_octets;
  word              frame_number = 0;
  byte              access_reference = 0;

  uint8 remaining_length = 0;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

   /* get a pointer to the first IE of the OTA message */
  current_IE = rr_get_first_ie_of_message(IMMEDIATE_ASSIGNMENT_REJECT, gas_id);

  if (current_IE == NULL)
  {
    MSG_GERAN_ERROR_0_G("current_IE == NULL");
    return EV_NO_EVENT;
  }

  // Extract the Page Mode
  paging_mode = (byte) (*(current_IE->start_IE) & 0x0F);

  if (!rr_in_idle)
  {
    if ( TRUE == rr_conn_est_info_ptr->egprs_access_valid )
    {
       /* decode the IAR rest octets before entering the loop */

       temp_IE_ptr = current_IE;

       /* advance to the IA Rest Octets IE*/
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Request Ref 1 */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Wait Ind 1 */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Request Ref 2 */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Wait Ind 2 */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Request Ref 3 */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Wait Ind 3 */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Request Ref 4 */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Wait Ind 4 */
       temp_IE_ptr = temp_IE_ptr->next_IE; /* Rest Octets */

       remaining_length = rr_length_to_end_of_ota_msg(
         IMMEDIATE_ASSIGNMENT_REJECT,   // msg_type
         temp_IE_ptr->start_IE,         // csn1_start_ie_ptr
         IAR_REST_OCTETS,               // csn_ie_type
         gas_id
       );

       (void) gcsn1_rr_decoder(
         IAR_REST_OCTETS,                     // message_type
         temp_IE_ptr->start_IE,               // csn1_ie_src
         &iar_rest_octets,remaining_length,   // len_to_end_of_buf
         gas_id
       );
    }
    else
    {
      /* initialise the IAR Rest Octets structure to avoid LINT warning */
      memset(&iar_rest_octets, 0, sizeof(iar_rest_octets_t));
    }

    /***************************************************************/
    /* The ImmediateAssignmentReject can address 4 mobile stations */
    /***************************************************************/
    for (request_index = 0; request_index < 4; request_index++)
    {
      if ( TRUE == rr_conn_est_info_ptr->egprs_access_valid )
      {
        current_IE = current_IE->next_IE;
        rr_decode_request_reference(current_IE->start_IE,&access_reference,&frame_number);

        if ( EGPRS_EXTENDED_RA_INDICATOR == access_reference )
        {
          if ( TRUE == iar_rest_octets.ext_ra[request_index].ext_ra_present )
          {
            if ( TRUE == rr_match_IA_extended_request_reference(
                           rr_conn_est_info_ptr->request_references,
                           rr_conn_est_info_ptr->channel_request_counter,
                           iar_rest_octets.ext_ra[request_index].ext_ra,
                           frame_number ) )
            {
              MSG_GERAN_HIGH_0_G("EGPRS IAREJ is for mobile");
              match_reference = TRUE;
            }
          }
        }
      }
      else
      {


        /**********************************************/
        /* have a look at the next request reference  */
        /**********************************************/
        current_IE = current_IE->next_IE;
        if (rr_match_IA_request_reference(
              rr_conn_est_info_ptr->request_references,
              current_IE->start_IE,
              rr_conn_est_info_ptr->channel_request_counter)
           )
        {
          MSG_GERAN_HIGH_0_G("IAREJ is for mobile");
          match_reference = TRUE;
        }
      }

      if (match_reference == TRUE)
      {
        rr_conn_est_info_ptr->immd_assign_rej_counter++;

        new_event = EV_ASSIGNMENT_REJECTION;

        current_IE = current_IE->next_IE;

        if (current_IE && current_IE->start_IE)
        {
          byte t3122_t3142_timeout_value = *(current_IE->start_IE);

          // If the access reason is for PS then the timeout value relates to T3142, otherwise
          // for GSM it is for T3122
          if ( IS_GPRS_ACCESS_REASON(rr_conn_est_info_ptr->access_reason) )
          {

            grr_start_t3146(gas_id);
            grr_start_t3142(t3122_t3142_timeout_value, gas_id);
          }
          else
          {
            rr_start_t3126(gas_id);
            rr_start_t3122(t3122_t3142_timeout_value, rr_conn_est_info_ptr->access_reason, gas_id);
          }
        }

        /* exit out of loop, no point in looking at other request references */
        break;
      }
      else /* did not match the current mobile */
      {
        /******************************************************************/
        /* skip wait indication in preparation for next request reference */
        /******************************************************************/
        current_IE = current_IE->next_IE;
      }
    } /* for all the request references */
  }
  /* otherwise, RR is in idle and should not process the message */

  // Check for updates in the Page Mode
  rr_ce_check_for_page_mode_update(
    own_paging_block,
    paging_mode,
    gas_id
  );

  return new_event;
} /* rr_ce_process_assignment_reject */

/**
 * Process the dl_unit_data_ind message for connection establishment purposes
 *
 * @param message_ptr      pointer to dl_unit_data_ind message received from L2
 *
 * @param rr_in_idle       TRUE if RR is in idle mode, FALSE otherwise
 *
 * @param ignore_IA_reject TRUE if immediate assignment messages should be ignored
 *
 * @return new event for caller
 */
static rr_event_T rr_con_est_process_dl_unit_data_ind(
  dl_unit_data_ind_T  *message_ptr,
  boolean             rr_in_idle,
  boolean             ignore_IA_reject,
  const gas_id_t      gas_id
)
{
  rr_event_T new_con_est_event = EV_NO_EVENT;
  boolean    own_paging_block;
  byte       message_type;

  /* the L3 message type */
  message_type = message_ptr->layer3_message[2];

  /**************************************************************/
  /* own_paging_block set to true if message is received in the */
  /* MS's own paging subchannel (see GSM 04.08 3.3.2.1)         */
  /**************************************************************/
  own_paging_block = message_ptr->own_paging_block;

  switch (message_type)
  {
    /*****************************************************/
    /* An IMMEDIATE ASSIGNMENT message has been received */
    /* from the network. We have to check if it is for   */
    /* the current mobile or not.                        */
    /*****************************************************/
    case IMMEDIATE_ASSIGNMENT :
    {
      new_con_est_event = rr_ce_process_immediate_assignment(
        own_paging_block,
        rr_in_idle,
        (&message_ptr->layer3_message[0] + message_ptr->l3_message_length),
        gas_id
      );
    }
    break;

    /******************************************************/
    /* An IMMEDIATE ASSIGNMENT EXTENDED message has been  */
    /* received from the network. We have to check if it  */
    /* is for the current mobile or not.                  */
    /******************************************************/
    case IMMEDIATE_ASSIGNMENT_EXTENDED :
    {
      new_con_est_event = rr_ce_process_immediate_assignment_extended(
        own_paging_block,
        rr_in_idle,
        gas_id
      );
    }
    break;

    /******************************************************/
    /* An IMMEDIATE ASSIGNMENT REJECT message has been    */
    /* received from the network. We have to check if it  */
    /* is for the current mobile or not.                  */
    /******************************************************/
    case IMMEDIATE_ASSIGNMENT_REJECT :
    {
      if (!ignore_IA_reject)
      {
        new_con_est_event = rr_ce_process_assignment_reject(
          own_paging_block,
          rr_in_idle,
          gas_id
        );
      }
    }
    break;

    default:
    {
      /* other types of OTA messages are ignored in this method */
      break;
    }
  } /* switch(message_type) */

  if ( (rr_in_idle == FALSE) &&
       ( (new_con_est_event == EV_IMMEDIATE_ASSIGNMENT) ||
         (new_con_est_event == EV_ASSIGNMENT_REJECTION) ) )
  {
    rr_gprs_odm_info_rach_success(gas_id);
  }

  return new_con_est_event;
} /* rr_con_est_process_dl_unit_data_ind */

/**
 * Returns the name of the given connection establishment state for debug
 *
 * @param state  connection establishment state
 *
 * @return text string corresponding to the given name
 */
static char * rr_con_est_state_name(rr_con_est_state_T state)
{
  switch (state)
  {
    case RR_CP_INACTIVE:                         return "INACTIVE";
    case RR_CP_SENDING_RANDOM_ACCESS:            return "SENDING_RANDOM_ACCESS";
    case RR_CP_MAX_RETRIES_ATTEMPTED:            return "MAX_RETRIES_ATTEMPTED";
    case RR_CP_AWAITING_PHYSICAL_LINK:           return "AWAITING_PHYSICAL_LINK";
    case RR_CP_AWAITING_LINK_ESTABLISHMENT:      return "AWAITING_LINK_ESTABLISHMENT";
    case RR_CP_LINK_ESTABLISHED:                 return "LINK_ESTABLISHED";
    case RR_CP_ASSIGNMENT_REJECTION_RECEIVED:    return "ASSIGNMENT_REJECTION_RECEIVED";
    case RR_CP_DISCONNECT_PHYSICAL_LINK:         return "DISCONNECT_PHYSICAL_LINK";
    case RR_CP_IA_ABORTED:                       return "IA_ABORTED";
    case RR_CP_PCCO_FAILED:                      return "PCCO_FAILED";
    case RR_CP_T3146_EXPIRED:                    return "T3146_EXPIRED";
    case RR_CP_T3146_EXPIRED_AFTER_REJECT:       return "T3146_EXPIRED_AFTER_REJECT";
    default: ;
  }

  return "?";
}

/**
 * Logs the change of connection establishment state
 *
 * @param new_state new state value
 *
 * @param old_state old state value
 */
static void rr_show_con_est_state_change(rr_con_est_state_T new_state, rr_con_est_state_T old_state)
{
  MSG_SPRINTF_2(MSG_SSID_DFLT,
                MSG_LEGACY_MED,
                "ConEstState(%s to %s)",
                rr_con_est_state_name(old_state),
                rr_con_est_state_name(new_state));

  return;
} /* rr_show_con_est_state_change */


/**
 * Called after a connest has been aborted at the request of MM (directly or implicitly)
 *
 * @param can_return_to_idle indicates if RR can return to Idle or NOT
 *
 * @return an event to be processed by the main RR state machine
 */
static rr_event_T rr_ce_complete_mm_abort_processing(
  boolean can_return_to_idle,
  const gas_id_t gas_id
)
{
  rr_conn_release_cause_t release_cause = rr_conn_get_release_cause(gas_id);

  /* cancel T3126 (in case it is running) */
  (void)rr_timer_cancel(T3126, gas_id);

  /* Assume for now RR is no longer considered camped - this will be reverted below if idle started */
  RRCE_SET_CAMPED_FLAG(FALSE, gas_id);

  /* If the conn est was aborted due to an implicit NAS request (e.g. a PLMN_LIST_REQ was */
  /* received), then the appropriate response is a RR_ABORT_IND, followed by processing the */
  /* NAS request. No RR_SERVICE_IND is required. */
  /* If the conn est was directly aborted by NAS with a RR_ABORT_REQ, the appropriate response */
  /* is a RR_ABORT_CNF, followed by a RR_SERVICE_IND (after returning to Idle if appropriate) */
  if (release_cause == RELEASE_CAUSE_IMPLICIT_NAS_REQUEST)
  {
    if (can_return_to_idle)
    {
      byte message_id;

      if (grr_examine_saved_mm_message(&message_id, gas_id))
      {
        switch (message_id)
        {
          case RR_EST_REQ:
          {
            break;
          }

          default:
          {
            if (rr_x2g_cco_in_progress(gas_id))
            {
              can_return_to_idle = TRUE;
            }
            else
            {
              can_return_to_idle = FALSE;
            }
          }
        }
      }
    }

    if (can_return_to_idle == FALSE)
    {
      /* Send RR_ABORT_CNF / RR_ABORT_IND, no RR_SERVICE_IND */
      rr_conn_inform_mm_of_channel_release(
        FALSE,    // send_rr_service_ind
        gas_id
      );

      return RR_EV_PROCESS_SAVED_MESSAGE;
    }
  }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
  else if (release_cause == RELEASE_CAUSE_X2G_CCO_FAILURE)
  {
    /* Send RR_ABORT_CNF / RR_ABORT_IND, no RR_SERVICE_IND */
    rr_conn_inform_mm_of_channel_release(
      FALSE,    // send_rr_service_ind
      gas_id
    );

    return RR_EV_PROCESS_SAVED_MESSAGE;
  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

  if (can_return_to_idle)
  {
    /* If the camped_on_flag is FALSE when idle is started, the pending */
    /* channel data will be copied over, which is not applicable here */
    RRCE_SET_CAMPED_FLAG(TRUE, gas_id);

    /* Cleanup; Notify MM of release/abort */
    rr_conn_est_failed(
      TRUE,    // notify_mm
      gas_id
    );

    return EV_START_IDLE;
  }

  /* Send RR_ABORT_CNF / RR_ABORT_IND, followed by RR_SERVICE_IND indicating no service */
  rr_conn_inform_mm_of_channel_release(
    TRUE,    // send_rr_service_ind
    gas_id
  );

  return EV_NO_BCCH_FOUND;

} /* rr_ce_complete_mm_abort_processing */

/**
 * Called when L1 has confirmed that dedicated/dual transfer mode has started
 * (using either MPH_IMMED_ASSIGN_CNF or MPH_DTM_CHANNEL_ASSIGN_CNF for EDTM)
 *
 */
static void rr_ce_l1_in_dedicated_mode(rr_l1_message_id_enum_T message_id, const gas_id_t gas_id)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if (message_id == MPH_IMMED_ASSIGN_CNF)
  {
    MSG_GERAN_MED_2_G("RR: L1 in dedicated mode (ECSC=%1d,ES_IND=%1d)",
             (int)scell_info_ptr->gsm.ecsc, (int)ES_IND);
  }
#ifdef FEATURE_GSM_DTM
  else if (message_id == MPH_DTM_CHANNEL_ASSIGN_CNF)
  {
    MSG_GERAN_MED_2_G("RR: L1 in dual transfer mode (ECSC=%1d,ES_IND=%1d)",
             (int)scell_info_ptr->gsm.ecsc, (int)ES_IND);
  }

#ifdef FEATURE_GSM_EDTM
  rr_gprs_edtm_dedicated_mode_started(message_id, gas_id);
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

  return;
} /* rr_ce_l1_in_dedicated_mode */

/* one-time initialisation of this module */
void rr_initialise_connection_establishment(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  memset((void *)rr_conn_est_info_ptr, 0, sizeof(rr_conn_est_info_T));
  rr_conn_est_info_ptr->con_est_state     = RR_CP_INACTIVE;
  rr_conn_est_info_ptr->old_con_est_state = RR_CP_INACTIVE;
  rr_conn_est_info_ptr->pkt_est_cause     = RR_PKT_EST_CAUSE_UNKNOWN;
  rr_conn_est_info_ptr->return_rr_event   = EV_NO_EVENT;
  rr_reset_t3122_info(gas_id);

  return;
}

#ifdef FEATURE_LTE

/**
 @brief Determines if the connection establishment is for a periodic LAU.

 @return TRUE if the connection establishment is for periodic LAU, FALSE
         if it is not.
*/
boolean rr_ce_is_periodic_lau(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  return rr_conn_est_info_ptr->is_periodic_lau;
}

#endif /* FEATURE_LTE */

boolean rr_ce_can_process_mm_message(const gas_id_t gas_id)
{
  boolean process = TRUE;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  switch (rr_conn_est_info_ptr->con_est_state)
  {
    case RR_CP_IA_ABORTED:
    case RR_CP_PCCO_FAILED:
    case RR_CP_T3146_EXPIRED:
    case RR_CP_T3146_EXPIRED_AFTER_REJECT:
    {
      process = FALSE;
      break;
    }

    case RR_CP_INACTIVE:
    {
      if (rr_conn_est_info_ptr->starting_idle)
      {
        /* Starting idle after RR_ABORT_REQ in MAX_RETRIES state. */
        process = FALSE;
      }
      break;
    }

    default: ;
  }

  return process;
}

boolean rr_ce_can_process_rr_grr_gprs_ia_ind(const gas_id_t gas_id)
{
  boolean process = TRUE;

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  switch (rr_conn_est_info_ptr->con_est_state)
  {
    case RR_CP_IA_ABORTED:
    case RR_CP_PCCO_FAILED:
    case RR_CP_T3146_EXPIRED:
    case RR_CP_T3146_EXPIRED_AFTER_REJECT:
    {
      process = FALSE;
      break;
    }

    case RR_CP_INACTIVE:
    {
      if (rr_conn_est_info_ptr->starting_idle)
      {
        process = FALSE;
      }
      break;
    }

    default: ;
  }

  return process;
}

/**
 * Handler for MPH_BLOCK_QUALITY_IND in access states
 *
 * @return new event for calling state machine
 */
static rr_event_T rr_ce_handle_block_quality(
  mph_block_quality_ind_T *block_qual_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T dsc_event;

  /* PS reselection enhancement, update the Downlink Signalling Counter (CR #98608) */
  dsc_event = rr_gprs_check_dsc_in_idle(block_qual_ptr->block_quality,
                                        block_qual_ptr->l2_channel_type,
                                        gas_id);
  return(dsc_event);
}

/**
 * Connection Establishment State machine
 *
 * @param con_est_event event for the state machine
 *
 * @param new_message   pointer to input message for state machine (only valid if event if EV_INPUT_MESSAGE)
 *
 * @return new event for calling state machine
 */
rr_event_T rr_connection_establishment_control(
  rr_event_T      con_est_event,
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T                 new_con_est_event = EV_NO_EVENT;
  rr_event_T                 new_rr_event      = EV_NO_EVENT;
  byte                       message_id        = 0;
  byte                       message_set       = 0;
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  while (con_est_event != EV_NO_EVENT)
  {
    /***********************************************************************/
    /* State independant message / event handling                          */
    /***********************************************************************/

    if (con_est_event == EV_RESET_SOFTWARE)
    {
      MSG_GERAN_HIGH_0_G("Resetting Connection Establishment module");

      rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

      /* reset global variables */
      rr_conn_est_info_ptr->procedure_counter = 0;
      rr_conn_est_info_ptr->repeat_assignment_procedure = FALSE;
      rr_conn_est_info_ptr->pcco_in_progress = FALSE;
      rr_conn_est_info_ptr->immd_assign_rej_counter = 0;
      rr_gprs_notify_sys_info_ia_reject(FALSE, gas_id);
    }
    else if (con_est_event == EV_GPRS_RESET_RR_CE_STATE)
    {
      if (rr_conn_est_info_ptr->con_est_state != RR_CP_INACTIVE)
      {
        /* Reset state to Inactive */
        rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        MSG_GERAN_HIGH_0_G("Reset connection establishment state");
      }

      /* Cancel TMULTIFRM timer */
      if (rr_timer_cancel(TMULTIFRM, gas_id))
      {
        MSG_GERAN_HIGH_0_G("Stopped TMULTIFRM timer");
        rr_conn_est_info_ptr->waiting_for_2nd_msg = FALSE;
        rr_gprs_two_part_assignment_failure(gas_id);

        // A two-part DL IA was expected but the 2nd part has not been received.
        // Discard any processing data
        rr_tbf_est_discard_data(gas_id);
      }

      /* Cancel T3146, if it's running */
      if (rr_timer_cancel(T3146, gas_id))
      {
        MSG_GERAN_HIGH_0_G("Stopped T3146 timer");
      }

      /* Cancel T3126, if it's running */
      if (rr_timer_cancel(T3126, gas_id))
      {
        MSG_GERAN_HIGH_0_G("Stopped T3126 timer");
      }

      /* ignore the reset event */
      con_est_event = EV_NO_EVENT;
    }

    else if (con_est_event == EV_INPUT_MESSAGE)
    {
      if (message_ptr != NULL)
      {
        /* set the message_set & message_id for all input messages */
        message_set = message_ptr->message_header.message_set;
        message_id =  message_ptr->message_header.message_id;

        /***********************************************************************/
        /* RR_ABORT_REQ                                                        */
        /* The conn est needs to be aborted                                    */
        /***********************************************************************/
        if ( ( message_set == MS_MM_RR ) && ( message_id == (byte) RR_ABORT_REQ ) )
        {
          MSG_GERAN_HIGH_0_G("Abort conn est requested by MM");

          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_TO_UPPER_LAYER_TRIGGER,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
          if (RR_CE_EDTM_REQUEST(rr_conn_est_info_ptr->request_references[0]))
          {
            (void)rr_gprs_edtm_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
          }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

          /* Note why the conn est is being aborted - this determines the response */
          /* to MM once this is complete */
          rr_conn_set_release_cause(RELEASE_CAUSE_NAS_REQUEST, gas_id);

          /* Now make this into a generic abort */
          con_est_event = RR_EV_MM_REQ_RECEIVED_ABORT;
        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("message_ptr was NULL");

        /* exit from the loop now */
        con_est_event = EV_NO_EVENT;
      }
    } /* EV_INPUT_MESSAGE */

    else
    /***********************************************************************/
    /* RR_EV_MM_REQ_RECEIVED_ABORT                                         */
    /* A NAS message was received which implicitly aborts the conn est     */
    /***********************************************************************/
    if ( con_est_event == RR_EV_MM_REQ_RECEIVED_ABORT )
    {
      MSG_GERAN_HIGH_0_G("Abort conn est to process MM message");

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
      if (RR_CE_EDTM_REQUEST(rr_conn_est_info_ptr->request_references[0]))
      {
        (void)rr_gprs_edtm_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
      }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

      /* Note why the conn est is being aborted - this determines the response */
      /* to MM once this is complete */
      rr_conn_set_release_cause(RELEASE_CAUSE_IMPLICIT_NAS_REQUEST, gas_id);
    }

    /* an EV_NO_EVENT should not be processed by state handlers */
    if (con_est_event == EV_NO_EVENT)
    {
      continue;
    }

    switch(rr_conn_est_info_ptr->con_est_state)
    {
      /***************************************************************************/
      /* No connection establishment is in process. The RR is likely to be       */
      /* in the Idle state listening for PagingRequest messages from the network */
      /* or RR_EST_REQ messages from the MM.                                     */
      /***************************************************************************/
      case RR_CP_INACTIVE :
      {
        if ((con_est_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (message_ptr->rr.header.rr_message_set)
            {
              case RR_PS_OMSG:
              {
                switch (message_ptr->rr.header.imh.message_id)
                {
                  case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                  {
                    // Some cases send the request to block PS access and then change state to RR_CP_INACTIVE
                    // but need to return different events to RR
                    // (see EV_DSF_RESELECTION, RR_EV_START_RESELECTION_POWER_SCAN and
                    // EV_FORCE_CELL_RESELECTION)

                    new_rr_event = rr_conn_est_info_ptr->return_rr_event;
                    rr_conn_est_info_ptr->return_rr_event = EV_NO_EVENT;
                    break;
                  }

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                }
                break;
              }

              case RR_L1_IDLE_OMSG:
              {
                switch (message_ptr->rr.header.imh.message_id)
                {
                  case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                  {
                    boolean send_rr_service_ind = TRUE;

                    /* Clear the flag that indicates we were waiting to start idle mode. */
                    rr_conn_est_info_ptr->starting_idle = FALSE;

                    switch (rr_conn_est_info_ptr->return_rr_event)
                    {
                      case EV_TBF_CONN_EST_FAILURE:
                      {
                        send_rr_service_ind = FALSE;
                        new_rr_event = rr_conn_est_info_ptr->return_rr_event;
                        break;
                      }

                      default:
                      {
                        new_rr_event = EV_GO_TO_IDLE;
                      }
                    }

                    if (send_rr_service_ind)
                    {
                      gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

                      rr_send_service_ind(
                        MM_AS_SERVICE_AVAILABLE,
                        &(scell_info_ptr->gsm.location_area_identification),
                        &(scell_info_ptr->gsm),
                        gas_id
                      );
                    }

                    rr_conn_est_info_ptr->return_rr_event = EV_NO_EVENT;
                    break;
                  }

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                }
                break;
              }

              default:
              {
                RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
              }
            }
          }

          else
          if ((message_set == MS_RR_L2) && (message_id == (byte) DL_UNIT_DATA_IND))
          {
            gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
            word                           channel_number;

            /****************************************************************************/
            /* DL_UNIT_DATA_IND carries over the air (OTA) messages from the network in */
            /* unacknowledged mode. These could be PagingRequest messages or            */
            /* ImmediateAssignment messages                                             */
            /****************************************************************************/
            channel_number = message_ptr->dl_unit_data_ind.channel_number.num;

            if ((rr_is_camped(gas_id) == TRUE) &&
                (channel_number != scell_info_ptr->gsm.BCCH_ARFCN.num))
            {
              /* should only be receiving OTA messages for the serving cell */
              MSG_GERAN_ERROR_2_G("BadARFCN:exp(%d),act(%d)", (int)scell_info_ptr->gsm.BCCH_ARFCN.num,(int)channel_number);
            }
            else
            {
              gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

              if ((rr_is_camped(gas_id) == FALSE) &&
                  (channel_number != pcell_info_ptr->gsm.BCCH_ARFCN.num))
              {
                /* During reselection and Early Camping pages will
                come in for the pending cell */
                MSG_GERAN_LOW_2_G("BadARFCN:exp(%d),act(%d)", (int)pcell_info_ptr->gsm.BCCH_ARFCN.num,(int) channel_number);
              }
              else
              {
                // The DL_UNIT_DATA_IND could contain IA or IA Reject
                new_con_est_event = rr_con_est_process_dl_unit_data_ind(
                  &message_ptr->dl_unit_data_ind,
                  TRUE,    // rr_in_idle
                  FALSE,   // ignore_ia_reject
                  gas_id   // gas_id
                );
              }

              MSG_GERAN_HIGH_2_G("L2 msg causes event %d (domain=%d)",
                       new_con_est_event,
                       rr_PLMN_request_details_ptr->service_domain);
            } /* the channel_number corresponds to the serving cell */
          } /* DL_UNIT_DATA_IND */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RANDOM_ACCESS_CNF))
          {
            /******************************************************************/
            /* Can be received in certain race conditions. Should be ignored. */
            /******************************************************************/
          } /* DL_RANDOM_ACCESS_CNF */

          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_EST_REQ))
          {
            /******************************************************/
            /* RR_EST_REQ is a request from MM to establish an RR */
            /* connection. The message contains the establishment */
            /* cause to be used in the CHANNEL_REQUEST messages   */
            /******************************************************/
            new_con_est_event = rr_ce_process_est_req(&message_ptr->rr_est_req, NULL, gas_id);

            #ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
            (void)rr_timer_cancel(RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER, gas_id);
            #endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
          } /* RR_EST_REQ message */

          else if ((message_set == MS_RR_L1) && (message_id == (byte)MPH_BLOCK_QUALITY_IND))
          {
            ;  // After a DSF is triggered, the state returns to RR_CP_INACTIVE while waiting for
               // PS access to be blocked. It is possible that more MPH_BLOCK_QUALITY_IND messages
               // could be received during this time
          }

          else
          {
            RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
          }
        } /* EV_INPUT_MESSAGE event */

        else if (con_est_event == EV_INIT_ACCESS_PROCEDURE)
        {
          /* Start the access procedure in response to a RR_EST_REQ */

          /* Set the pcco_in_progress flag appropriately. */
          if (rr_timer_is_running(T3174, gas_id))
          {
            rr_conn_est_info_ptr->pcco_in_progress = TRUE;
          }
          else
          {
            rr_conn_est_info_ptr->pcco_in_progress = FALSE;
          }

          /* Stop any L1 sys info acquisition if in progress */
          rr_gprs_suspend_camped_sys_info_handling(FALSE, gas_id);

#ifdef FEATURE_GSM_DTM
          /* Notify DTM controller that CS conn est is starting */
          rr_dtm_send_imsg_cs_est_ind(gas_id);
#endif /* FEATURE_GSM_DTM */

          /* send the first random access (CHANNEL_REQUEST) to network */
          rr_conn_est_info_ptr->con_est_state = RR_CP_SENDING_RANDOM_ACCESS;
          rr_ce_initiate_random_access(gas_id);

          /***************************************************************/
          /* Reset the procedure_counter to 0 since this is a completely */
          /* new IA procedure and not a repeat of a previous one         */
          /***************************************************************/
          rr_conn_est_info_ptr->procedure_counter = 0;

          /*****************************************************************/
          /* indicate to RR state machine that random access procedure has */
          /* been started                                                  */
          /*****************************************************************/
          if (!rr_is_control_state(RR_CONNECTION_PENDING, gas_id))
          {
            new_rr_event = EV_RR_EST_REQ;
          }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          if (rr_x2g_cco_for_csfb_in_progress(gas_id))
          {
            rr_ce_send_omsg_x2g_cco_status_ind(RR_CCO_STATUS_SUCCESS, gas_id);
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
        } /* EV_INIT_ACCESS_PROCEDURE */

        /***************************************************************/
        /* We are ready to initiate the access procedure, after        */
        /* receiving a request from MAC for packet access on the 51    */
        /* frame CCCH                                                  */
        /***************************************************************/
        else if (con_est_event == EV_GPRS_INIT_ACCESS_PROCEDURE)
        {
          /* Set the pcco_in_progress flag appropriately. */
          if (rr_timer_is_running(T3174, gas_id))
          {
            rr_conn_est_info_ptr->pcco_in_progress = TRUE;
          }
          else
          {
            rr_conn_est_info_ptr->pcco_in_progress = FALSE;
          }

          /* Stop any L1 sys info acquisition if in progress */
          rr_gprs_suspend_camped_sys_info_handling(FALSE, gas_id);

          /* send the first random access (CHANNEL_REQUEST) to network */
          rr_conn_est_info_ptr->con_est_state = RR_CP_SENDING_RANDOM_ACCESS;
          rr_ce_initiate_random_access(gas_id);

          /***************************************************************/
          /* Reset the procedure_counter to 0 since this is a completely */
          /* new IA procedure and not a repeat of a previous one         */
          /***************************************************************/
          rr_conn_est_info_ptr->procedure_counter = 0;

          /* Put L1 in MM non-DRX mode if this was requested by MM */
          rr_gprs_set_l1_in_mm_non_drx(gas_id);

          /*****************************************************************/
          /* indicate to RR state machine that random access procedure has */
          /* been started                                                  */
          /*****************************************************************/
          new_rr_event = EV_GRR_EST_REQ;
        }

        else if (con_est_event == EV_ACCESS_CLASS_NOT_ALLOWED)
        {
          /***********************************************************/
          /* The user can't access the network, because the class    */
          /* they belong to is not authorised                        */
          /***********************************************************/

          /* just abort the immediate assignment procedure */
          /* RR should already be in IDLE state */
          (void)rr_ce_abort_immed_assign_procedure(ACCESS_CLASS_NOT_ALLOWED,
                                                   MM_RR_OTHER_FAILURE,
                                                   FALSE, gas_id);

          new_rr_event = EV_ACCESS_CLASS_NOT_ALLOWED;
        } /* EV_ACCESS_CLASS_NOT_ALLOWED */

        /*********************************************************************/
        /* An GPRS IMMEDIATE ASSIGNMENT message has been received and is for */
        /* the current mobile. MAC has been sent a request for establishment */
        /*********************************************************************/
        else if (con_est_event == EV_GPRS_IMMEDIATE_ASSIGNMENT)
        {
          /* For the case where GRR main state is GRR_CONNECTION_PENDING, and GRR is */
          /* preparing to start UL TBF establishment (i.e. GRR_MAC_RACH_REQ has been */
          /* sent, and GRR is awaiting a response), move the state back to GRR_CAMPED */
          new_rr_event = EV_GRR51_GO_TO_PACKET_IDLE;
        }

        else if (con_est_event == EV_RESET_SOFTWARE)
        {
          /* Nothing to do */
          ;
        }

        else if (con_est_event == EV_CHANNEL_DEACTIVATED)
        {
          /*
          Can be received when in CP_INACTIVE is a connection establishment attempt was
          aborted before it was established (e.g. no Imm Ass received)
          */
#ifdef FEATURE_GSM_DTM
          rr_dtm_send_imsg_cs_rel_ind(gas_id);
#endif /* FEATURE_GSM_DTM */
        }

#ifdef FEATURE_WCDMA
        // W2G HO will establish a RR connection without going through
        // normal RR connection establishment procedure
        else if (con_est_event == EV_CONNECTION_EXISTS)
        {
          rr_conn_est_info_ptr->con_est_state = RR_CP_LINK_ESTABLISHED;
        }
#endif /* FEATURE_WCDMA */

        /*****************************************************************/
        /* RR_EV_MM_REQ_RECEIVED_ABORT                                   */
        /* An abort has come from NAS                                    */
        /*****************************************************************/
        else if (con_est_event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_TO_UPPER_LAYER_TRIGGER,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );
 
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          /* complete abort processing; a return to idle is allowed if required */
          new_con_est_event = rr_ce_complete_mm_abort_processing(TRUE, gas_id);
        }

        /*****************************************************************/
        /* EV_GO_TO_IDLE                                                 */
        /* EV_NO_BCCH_FOUND                                              */
        /* RR_EV_PROCESS_SAVED_MESSAGE                                   */
        /* Processing is complete, return Conn Est to Idle               */
        /*****************************************************************/
        else if ( ( con_est_event == EV_GO_TO_IDLE ) ||
                  ( con_est_event == EV_NO_BCCH_FOUND ) ||
                  ( con_est_event == RR_EV_PROCESS_SAVED_MESSAGE ) )
        {
          new_rr_event = con_est_event;
        }

        else if (con_est_event == EV_START_IDLE)
        {
          rr_l1_idle_send_imsg_start_idle_req(gas_id);
        }

        /**********************************************************************/
        /* This timer is used to receive the 2nd of a 2 part IA msg within a  */
        /* specified time. If the timer has expired then reset the flag which */
        /* indicates a 2nd msg is being waited for.                           */
        /**********************************************************************/
        else if (con_est_event == EV_TMULTIFRM_EXPIRY)
        {
          MSG_GERAN_HIGH_1_G("Timer TMULTIFRM expired in state %d",(int)(rr_conn_est_info_ptr->con_est_state));
          /* Reset the flag, since the 2nd IA msg didn't arrive in time */
          rr_conn_est_info_ptr->waiting_for_2nd_msg = FALSE;
          rr_gprs_two_part_assignment_failure(gas_id);

          // A two-part DL IA was expected but the 2nd part has not been received.
          // Discard any processing data
          rr_tbf_est_discard_data(gas_id);
        }

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
        else if (con_est_event == EV_EDTM_IMM_ASSIGNMENT_RECEIVED)
        {
          /* shift into a state that handles the Immediate Assignment */
          rr_conn_est_info_ptr->con_est_state = RR_CP_SENDING_RANDOM_ACCESS;
        }
        else if (con_est_event == EV_EDTM_DTM_ASSIGNMENT_RECEIVED)
        {
          /* L1 has successfully set up the physical channel   */
          rr_ce_l1_in_dedicated_mode(MPH_DTM_CHANNEL_ASSIGN_CNF, gas_id);

          /* start the process of setting up the L2 SAPI0 link */
          rr_ce_send_first_message(gas_id);

          rr_conn_est_info_ptr->con_est_state = RR_CP_AWAITING_LINK_ESTABLISHMENT;

          new_rr_event = EV_RR_EST_L1_IN_DED;
        }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        else if (con_est_event == EV_T3174_EXPIRY)
        {
          if (rr_x2g_cco_in_progress(gas_id))
          {
            rr_ce_send_omsg_x2g_cco_status_ind(RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY, gas_id);
          }
        }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
      } /* RR_CP_INACTIVE state */
      break;

      /********************************************************************************/
      /* RR has sent a CHANNEL_REQUEST message to the network (random access) and     */
      /* is waiting for an ImmediateAssignment message to continue the immediate      */
      /* assignment procedure. It will continue sending random access messages        */
      /* until either an immediate assignment message is received, or the max. number */
      /* of random access messages have been sent (max. is based on SI parameters)    */
      /********************************************************************************/
      case RR_CP_SENDING_RANDOM_ACCESS:
      {
        if ((con_est_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (message_ptr->rr.header.rr_message_set)
            {
              case RR_L1_IDLE_OMSG:
              {
                switch (message_ptr->rr.header.imh.message_id)
                {
                  case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                  {
                    new_con_est_event = rr_conn_est_info_ptr->return_rr_event;
                    rr_conn_est_info_ptr->return_rr_event = EV_NO_EVENT;
                    break;
                  }

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                }
                break;
              }

              case RR_PS_OMSG:
              {
                switch (message_ptr->rr.header.imh.message_id)
                {
                  case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                  {
                    /* MAC has returned to idle after a GPRS connection failure */
                    /* abort the RACHing and clear DRX */
                    rr_send_mph_abort_ra_req(gas_id);
                    rr_gprs_clear_l1_in_mm_non_drx(gas_id);

                    /* Cleanup; Don't notify MM of release/abort */
                    rr_conn_est_failed(FALSE, gas_id);

                    rr_conn_est_info_ptr->return_rr_event = EV_TBF_CONN_EST_FAILURE;
                    rr_l1_idle_send_imsg_start_idle_req(gas_id);

                    rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
                    break;
                  }

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                }
                break;
              }

              default:
              {
                RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
              }
            }
          }

          else
          if (message_set == MS_RR_L2)
          {
            /****************************************************************************/
            /* DL_UNIT_DATA_IND carries over the air (OTA) messages from the network in */
            /* unacknowledged mode. These could be PagingRequest messages (ignored) or  */
            /* ImmediateAssignment messages                                             */
            /****************************************************************************/
            if (message_id == (byte) DL_UNIT_DATA_IND)
            {
              // The DL_UNIT_DATA_IND could contain IA or IA Reject
              new_con_est_event = rr_con_est_process_dl_unit_data_ind(
                &message_ptr->dl_unit_data_ind,
                FALSE,    // rr_in_idle
                FALSE,    // ignore_ia_reject
                gas_id    // gas_id
              );
            } /* DL_UNIT_DATA_IND */

            /**************************************************/
            /* A DL_RANDOM_ACCESS_CONFIRM has been received.  */
            /* Indication from L2 that it has sent the random */
            /* access message to the network.                 */
            /**************************************************/
            else if (message_id == (byte) DL_RANDOM_ACCESS_CNF)
            {
              dl_random_access_cnf_T *cnf_ptr = &(message_ptr->dl_random_access_cnf);

              if (GERAN_ACCESS_GRANTED != cnf_ptr->result)
              {
                /* the RACH burst was not transmitted over the air */
                new_con_est_event = rr_ce_handle_access_failure(cnf_ptr, gas_id);
              }
              else
              {
                /************************************************************/
                /* store the request reference (random access conent)       */
                /* of the last message sent and send the next random access */
                /* message if appropriate (returns EV_MAX_RETRIES_ATTEMPTED */
                /* if the max number of access messages have been sent      */
                /************************************************************/
                new_con_est_event = rr_ce_handle_access_confirm(cnf_ptr, gas_id);
              }
            } /* DL_RANDOM_ACCESS_CNF */

            else
            {
              RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
            }
          } /* MessageSet is L2 */

          /*********************************/
          /* Handling of the DSC messaging */
          /*********************************/
          else if ((message_set == MS_RR_L1) && (message_id == (byte)MPH_BLOCK_QUALITY_IND))
          {
            new_con_est_event = rr_ce_handle_block_quality(
              (mph_block_quality_ind_T *)message_ptr,
              gas_id
            );
          }

          else
          {
            RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
          }
        } /* EV_INPUT_MESSAGE */

        else if((con_est_event == EV_DSF_RESELECTION) || (con_est_event == EV_DSF_FOR_DENIALS))
        {
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_TO_DOWNLINK_SIGNAL_FAILURE,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );

          /* Cancel TMULTIFRM timer */
          if (rr_timer_cancel(TMULTIFRM, gas_id))
          {
            MSG_GERAN_HIGH_0_G("Stopped TMULTIFRM timer");
            rr_conn_est_info_ptr->waiting_for_2nd_msg = FALSE;
            rr_gprs_two_part_assignment_failure(gas_id);

            // A two-part DL IA was expected but the 2nd part has not been received.
            // Discard any processing data
            rr_tbf_est_discard_data(gas_id);
          }

          /* abort any Random Access's in progress (L1 stays in RACH state) */
          rr_send_mph_abort_ra_req(gas_id);

          /* Aborted RACH procedure. so stop GRR guard timer it may be running*/
          grr_stop_guard_timer(gas_id);

          if (rr_timer_is_running(T3174, gas_id))
          {
            MSG_GERAN_HIGH_0_G("DSF Failure happened while PCCO in progress");

            if (rr_ce_for_gprs_access(gas_id))
            {
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                DEL_ONE,    // delete_pdu
                gas_id      // gas_id
              );
            }
            else
            {
              if (rr_x2g_cco_in_progress(gas_id) == FALSE)
              {
                rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
              }

              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                NO_DEL,     // delete_pdu
                gas_id      // gas_id
              );
            }

            /* Make the current cell forbidden as it should not be selected
               unless there are no other suitable cells. */
            rr_ce_handle_scell_dsf_event(con_est_event, gas_id);
            rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);
            rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_DSF;
            rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
          }
          else
          {
            /* RR is no longer considered camped since this is a reselection trigger */
            RRCE_SET_CAMPED_FLAG(FALSE, gas_id);

            if (rr_ce_for_gprs_access(gas_id))
            {
              if (rr_timer_cancel(T3176, gas_id))
              {
                MSG_GERAN_HIGH_0_G("Stopped T3176 timer");
              }
              /* As TBF establishment has failed, suspend MAC and instruct
                 RLC to delete it's current PDU. */
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                DEL_ONE,    // delete_pdu
                gas_id      // gas_id
              );

              // Record RR event to be returned when PS access blocked
              rr_conn_est_info_ptr->return_rr_event = con_est_event;
            }
            else
            {
              /***********************************************************************/
              /* abort the IA procedure and say that RR should not return to idle (  */
              /* RR is not really in idle but since a reselection is being performed */
              /* there is no point returning to idle now)                            */
              /***********************************************************************/
              (void)rr_ce_abort_immed_assign_procedure(RANDOM_ACCESS_FAILURE,
                                                       MM_RR_OTHER_FAILURE,
                                                       FALSE, gas_id);

              new_rr_event = con_est_event;
            }

            /* store the reselection trigger */
            rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_DSF, gas_id);

            /* connection establishment aborted */
            rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

            /* Make the current cell forbidden as it should not be selected
               unless there are no other suitable cells. */
            rr_ce_handle_scell_dsf_event(con_est_event, gas_id);
          }
        }

        else if (con_est_event == EV_CONNECTION_ABORTION)
        {
          mm_rr_failure_cause_e_type MM_fail_cause;

          /*****************************************************************/
          /* The EV_CONNECTION_ABORTION event is sent if the channel       */
          /* described in an Immediate Assignment message is unacceptable  */
          /* for some reason (e.g. the channel is in an unsupported band), */
          /* or the RACH bursts could not be sent OTA by the lower layers  */
          /*****************************************************************/
          if (rr_conn_est_info_ptr->failed_rach_count < RR_MS_MAX_FAILED_RACH_ATTEMPTS)
          {
            MM_fail_cause = MM_RR_OTHER_FAILURE;
          }
          else
          {
            MM_fail_cause = MM_RR_NO_RESOURCE_AVAILABLE;
          }

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
          if (RR_CE_EDTM_REQUEST(rr_conn_est_info_ptr->request_references[0]))
          {
            /* notify EDTM module of assignment failure */
           (void)rr_gprs_edtm_control(EV_CONNECTION_ABORTION, NULL, gas_id);
          }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

          /* abort the IA procedure and return to RR IDLE */
          new_con_est_event = rr_ce_abort_immed_assign_procedure(REASON_NOT_AVAILABLE,
                                                                 MM_fail_cause,
                                                                 TRUE, gas_id);
        } /* EV_CONNECTION_ABORTION */

        else if (con_est_event == EV_GPRS_CONNECTION_ABORTION)
        {
          if (rr_timer_cancel(T3176, gas_id))
          {
            MSG_GERAN_HIGH_0_G("Stopped T3176 timer");
          }

          // Send NO_PS_ACCESS_IND{DEL_ONE}.
          // Remain in this state until MAC indicates IDLE
          rr_ps_send_imsg_no_ps_access_req(
            FALSE,      // suspend_gmm
            DEL_ONE,    // delete_pdu
            gas_id      // gas_id
          );
        }

        /************************************************************/
        /* EV_T3174_EXPIRY                                          */
        /* Stop the establishment in this case. The PCCO has failed */
        /************************************************************/
        else if ( con_est_event == EV_T3174_EXPIRY )
        {
          rr_send_mph_abort_ra_req(gas_id);

          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          if (rr_ce_for_gprs_access(gas_id))
          {
            RR_ASSERT_WARN(rr_timer_is_running(T3146, gas_id) == FALSE);

            rr_ps_send_imsg_no_ps_access_req(
              FALSE,      // suspend_gmm
              DEL_ONE,    // delete_pdu
              gas_id      // gas_id
            );
          }
          else
          {
            RR_ASSERT_WARN(rr_timer_is_running(T3126, gas_id) == FALSE);

            /* RR_ABORT_IND is only sent for G2G PCCO, for X2G CCO it is
            not required by NAS. */
            if (!rr_x2g_cco_in_progress(gas_id))
            {
              rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
            }
            rr_ps_send_imsg_no_ps_access_req(
              FALSE,      // suspend_gmm
              NO_DEL,     // delete_pdu
              gas_id      // gas_id
            );
          }

          /* Make the current cell forbidden as it should not be selected
             unless there are no other suitable cells. */
          rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
          rr_start_forbidden_timer(gas_id);

          rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);
          rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY;
          rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
        }

        /************************************************************/
        /* EV_T3176_EXPIRY                                          */
        /* Stop the establishment if is for single block. In such a */
        /* case MAC is trying to send a PCCF. If not singl block MAC*/
        /* is also establishing for data transfer.                  */
        /************************************************************/
        else if ( con_est_event == EV_T3176_EXPIRY )
        {
          MSG_GERAN_HIGH_0_G("T3176 expiry, T3146 stopped");
          (void)rr_timer_cancel(T3146, gas_id);

          /* abort any Random Access's in progress and re-camp L1*/
          rr_send_mph_abort_ra_req(gas_id);

          // Record the RR event to be returned once L1 in idle
          rr_conn_est_info_ptr->return_rr_event = EV_T3176_TIMEOUT;

          // Send request to put L1 in idle. This will return RR_L1_IDLE_OMSG_START_IDLE_CNF
          rr_l1_idle_send_imsg_start_idle_req(gas_id);
        }

        /************************************************************/
        /* EV_T3176_TIMEOUT                                         */
        /* Notify the calling state machine of T3176 expiry         */
        /************************************************************/
        else if ( con_est_event == EV_T3176_TIMEOUT )
        {
          new_rr_event = EV_T3176_TIMEOUT;
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        }

        else if (con_est_event == EV_IMMEDIATE_ASSIGNMENT)
        {
          /******************************************************************/
          /* The previously IMMEDIATE ASSIGNMENT message is for the current */
          /* mobile, and a channel has been requested from L1               */
          /******************************************************************/

          /* await the response from L1 */
          rr_conn_est_info_ptr->con_est_state = RR_CP_AWAITING_PHYSICAL_LINK;
        } /* EV_IMMEDIATE_ASSIGNMENT */

        /***********************************************************************/
        /* The previous IMMEDIATE ASSIGNMENT message is for the current mobile */
        /* and was a GPRS IA. MAC has been sent a request for establishment    */
        /***********************************************************************/
        else if (con_est_event == EV_GPRS_IMMEDIATE_ASSIGNMENT)
        {
          /* abort any Random Access's in progress (L1 stays in RACH state) */
          rr_send_mph_abort_ra_req(gas_id);

          /*Aborted RACH procedure. so stop GRR guard timer it may be running*/
          grr_stop_guard_timer(gas_id);

          /* Conn Establishment done, MAC handles everything else from here on */
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
          new_rr_event = EV_GRR51_GO_TO_PACKET_IDLE;
        }

        else if (con_est_event == EV_MAX_RETRIES_ATTEMPTED)
        {
          /******************************************************************/
          /* The maximum RANDOM ACCESS messages has been sent over the air. */
          /* We can't send more, so we have now to wait for a eventual      */
          /* response from the network or the expiration of T3126           */
          /******************************************************************/
          rr_conn_est_info_ptr->con_est_state = RR_CP_MAX_RETRIES_ATTEMPTED;
        } /* EV_MAX_RETRIES_ATTEMPTED */

        else if (con_est_event == EV_ASSIGNMENT_REJECTION)
        {
          /*************************************************************/
          /* The network has sent an ImmediateAssignmentReject message */
          /* indicating that it does not have any channels available   */
          /* to establish a connection with the mobile at this time.   */
          /* Network may include a T3122 value to discourage mobile    */
          /* from accessing the network again for a while to reduce    */
          /* unnecessary traffic                                       */
          /* See GSM04.08 3.3.1.1.3.2                                  */
          /*************************************************************/

          /****************************************************************/
          /* RR should not send any more RACH messages after the IAREJ is */
          /* received. Ask L1 to abort any that are still pending.        */
          /****************************************************************/
          rr_send_mph_abort_ra_req(gas_id);

          if (rr_timer_is_running(T3174, gas_id))
          {
            if (rr_ce_for_gprs_access(gas_id))
            {
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                DEL_ONE,    // delete_pdu
                gas_id      // gas_id
              );
            }
            else
            {
              if (rr_x2g_cco_in_progress(gas_id) == FALSE)
              {
                rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
              }

              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                NO_DEL,     // delete_pdu
                gas_id      // gas_id
              );
            }

            /* Make the current cell forbidden as it should not be selected
               unless there are no other suitable cells. */
            rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
            rr_start_forbidden_timer(gas_id);

            rr_resel_set_pccf_cause_to_assignment_reject_on_target(gas_id);
            rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_IA_REJECT;
            rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
          }
          else
          {
            byte no_of_rach_ota = rr_conn_est_info_ptr->channel_request_counter;                      
            /* move to the reject state and await timer expiries */
            rr_conn_est_info_ptr->con_est_state = RR_CP_ASSIGNMENT_REJECTION_RECEIVED;

            /***********************************************************************************/
            /* From 44.018 */
            /* As an option the mobile station may return to CCCH idle mode as soon as it has received */
            /* responses from the network on all, or in case more than 3 were sent the last 3, of its CHANNEL */
            /* REQUEST messages. Hence to summarise: */
            /* 1. If mobile sent one channel request then as soon as mobile receives IAREJ for this channel */
            /* request it can return to CCCH idle mode. */
            /* 2. If mobile sent two channel requests then as soon as mobile receives IAREJ for both channel */
            /* requests it can return to CCCH idle mode. */
            /* If network has only sent IAREJ for one of the two channel requests then MS is to wait for */
            /* response to the other channel request. */
            /* 3. If mobile sent three channel requests then as soon as mobile receives IAREJ for all three */
            /* channel requests it can return to CCCH idle mode. */
            /* If network has sent IAREJ for less than three channel requests then MS is to wait for a */
            /* response for the remaining channel requests. */
            /* If mobile sent more than three channel requests then as soon as mobile receives IAREJ for the */
            /* last 3 channel requests it can return to CCCH idle mode. */
            /************************************************************************************/
      
            no_of_rach_ota--; /*GRR updates the count first and then sends DL_RANDOM_ACCESS_REQ*/
            MSG_GERAN_HIGH_1_G("No. of RACH sent is %d ",no_of_rach_ota);
            MSG_GERAN_HIGH_1_G("No. of IArej received is %d ",rr_conn_est_info_ptr->immd_assign_rej_counter);

            if (no_of_rach_ota >=3)
            {
              no_of_rach_ota = 3;
            }

            if (no_of_rach_ota == rr_conn_est_info_ptr->immd_assign_rej_counter)
            {
              if ( IS_GPRS_ACCESS_REASON(rr_conn_est_info_ptr->access_reason) )
              {
                MSG_GERAN_HIGH_0_G("No. of IAREJ match with No. of RACH hence implicly expire T3146 ");
                (void)rr_timer_cancel(T3146, gas_id);
                new_con_est_event = EV_T3146_EXPIRY;
                rr_conn_est_info_ptr->immd_assign_rej_counter =0;
              }
              else
              {
                MSG_GERAN_HIGH_0_G("No. of IAREJ match with No. of RACH hence implicly expire T3126 ");
                (void)rr_timer_cancel(T3126, gas_id);
                new_con_est_event = EV_T3126_EXPIRY;
                rr_conn_est_info_ptr->immd_assign_rej_counter =0;
              }
            }
          }
        } /* EV_ASSIGNMENT_REJECTION */

        else if (con_est_event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
 
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          /* Wait for the RACH response from L2 in RR_CP_IA_ABORTED state, then abort */
          rr_conn_est_info_ptr->con_est_state = RR_CP_IA_ABORTED;
        }

        /**********************************************************************/
        /* This timer is used to receive the 2nd of a 2 part IA msg within a  */
        /* specified time. If the timer has expired then reset the flag which */
        /* indicates a 2nd msg is being waited for.                           */
        /**********************************************************************/
        else if (con_est_event == EV_TMULTIFRM_EXPIRY)
        {
          MSG_GERAN_HIGH_1_G("Timer TMULTIFRM expired in state %d",(int)(rr_conn_est_info_ptr->con_est_state));
          /* Reset the flag, since the 2nd IA msg didn't arrive in time */
          rr_conn_est_info_ptr->waiting_for_2nd_msg = FALSE;
          rr_gprs_two_part_assignment_failure(gas_id);

          // A two-part DL IA was expected but the 2nd part has not been received.
          // Discard any processing data
          rr_tbf_est_discard_data(gas_id);
        }

        /*****************************************************************/
        /* EV_GO_TO_IDLE                                                 */
        /* L1 has returned to Idle, return to GRR                        */
        /*****************************************************************/
        else if ( con_est_event == EV_GO_TO_IDLE )
        {
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
          new_rr_event = EV_GO_TO_IDLE;
        }

        else if ( con_est_event == EV_START_IDLE )
        {
          rr_l1_idle_send_imsg_start_idle_req(gas_id);
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        }

        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
      } /* RR_CP_SENDING_RANDOM_ACCESS */
      break;

      /*********************************************************************/
      /* RR has sent the maximum number of CHANNEL_REQUEST (random access) */
      /* messages to the network and is now waiting for a response in the  */
      /* form of an immediate assignment message or for T3126 to expire    */
      /*********************************************************************/
      case RR_CP_MAX_RETRIES_ATTEMPTED :
      {
        if ((con_est_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (message_ptr->rr.header.rr_message_set)
            {
              case RR_L1_IDLE_OMSG:
              {
                switch (message_ptr->rr.header.imh.message_id)
                {
                  case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                  {
                    new_con_est_event = rr_conn_est_info_ptr->return_rr_event;
                    rr_conn_est_info_ptr->return_rr_event = EV_NO_EVENT;
                    break;
                  }

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                }
                break;
              }

              case RR_PS_OMSG:
              {
                switch (message_ptr->rr.header.imh.message_id)
                {
                  case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                  {
                    /* MAC has returned to idle after a GPRS connection failure */
                    /* abort the RACHing and clear DRX */
                    rr_send_mph_abort_ra_req(gas_id);
                    rr_gprs_clear_l1_in_mm_non_drx(gas_id);

                    /* Cleanup; Don't notify MM of release/abort */
                    rr_conn_est_failed(FALSE, gas_id);

                    rr_conn_est_info_ptr->return_rr_event = EV_TBF_CONN_EST_FAILURE;
                    rr_l1_idle_send_imsg_start_idle_req(gas_id);

                    rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
                    break;
                  }

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                }
                break;
              }

              default:
              {
                RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
              }
            }
          }

          else
          if (message_set == MS_RR_L2)
          {
            if (message_id == (byte) DL_UNIT_DATA_IND)
            {
              // The DL_UNIT_DATA_IND could contain IA or IA Reject
              new_con_est_event = rr_con_est_process_dl_unit_data_ind(
                &message_ptr->dl_unit_data_ind,
                FALSE,    // rr_in_idle
                FALSE,    // ignore_ia_reject
                gas_id    // gas_id
              );
            } /* DL_UNIT_DATA_IND */

            else if (message_id == (byte) DL_RANDOM_ACCESS_CNF)
            {
              /******************************************************************/
              /* Can be received in certain race conditions. Should be ignored. */
              /******************************************************************/
            } /* DL_RANDOM_ACCESS_CNF */

            else
            {
              RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
            }
          } /* MessageSet is L2 */

          /*********************************/
          /* Handling of the DSC messaging */
          /*********************************/
          else if ((message_set == MS_RR_L1) && (message_id == (byte)MPH_BLOCK_QUALITY_IND))
          {
            new_con_est_event = rr_ce_handle_block_quality(
              (mph_block_quality_ind_T *)message_ptr,
              gas_id
            );
          }

          else
          {
            RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
          }
        } /* EV_INPUT_MESSAGE */

        else if ((con_est_event == EV_DSF_RESELECTION) || (con_est_event == EV_DSF_FOR_DENIALS))
        {
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_TO_DOWNLINK_SIGNAL_FAILURE,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );

          if (rr_timer_is_running(T3174, gas_id))
          {
            /* Make the current cell forbidden as it should not be selected
               unless there are no other suitable cells. */

            rr_ce_handle_scell_dsf_event(con_est_event, gas_id);
            if (rr_ce_for_gprs_access(gas_id))
            {
              /* Cancel T3146, if it's running */
              if (rr_timer_cancel(T3146, gas_id))
              {
                MSG_GERAN_HIGH_0_G("Stopped T3146 timer");
              }

              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                DEL_ONE,    // delete_pdu
                gas_id      // gas_id
              );
            }
            else
            {
              /* Cancel T3126, if it's running */
              if (rr_timer_cancel(T3126, gas_id))
              {
                MSG_GERAN_HIGH_0_G("Stopped T3126 timer");
              }

              if (rr_x2g_cco_in_progress(gas_id) == FALSE)
              {
                rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
              }

              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                NO_DEL,     // delete_pdu
                gas_id      // gas_id
              );
            }

            MSG_GERAN_HIGH_0_G("DSF Failure happened while PCCO in progress");
            rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);
            rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_DSF;
            rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
          }
          else
          {
            /* RR is no longer considered camped since this is a reselection trigger */
            RRCE_SET_CAMPED_FLAG(FALSE, gas_id);

            if (rr_ce_for_gprs_access(gas_id))
            {
              if (rr_timer_cancel(T3176, gas_id))
              {
                MSG_GERAN_HIGH_0_G("Stopped T3176 timer");
              }
              /* Cancel T3146, if it's running */
              if (rr_timer_cancel(T3146, gas_id))
              {
                MSG_GERAN_HIGH_0_G("Stopped T3146 timer");
              }
              /* As TBF establishment has failed, suspend MAC and instruct
                 RLC to delete it's current PDU. */
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                DEL_ONE,    // delete_pdu
                gas_id      // gas_id
              );

              // Record RR event to be returned when PS access blocked
              rr_conn_est_info_ptr->return_rr_event = con_est_event;
            }
            else
            {
              /***********************************************************************/
              /* abort the IA procedure and say that RR should not return to idle (  */
              /* RR is not really in idle but since a reselection is being performed */
              /* there is no point returning to idle now)                            */
              /***********************************************************************/
              (void)rr_ce_abort_immed_assign_procedure(RANDOM_ACCESS_FAILURE,
                                                       MM_RR_OTHER_FAILURE,
                                                       FALSE, gas_id);

              new_rr_event = con_est_event;
            }

            /* store the reselection trigger */
            rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_DSF, gas_id);

            /* connection establishment aborted */
            rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

            /* Make the current cell forbidden as it should not be selected
               unless there are no other suitable cells. */
            rr_ce_handle_scell_dsf_event(con_est_event, gas_id);
          }
        }

        else if (con_est_event == EV_CONNECTION_ABORTION)
        {
          /*****************************************************************/
          /* The EV_CONNECTION_ABORTION event is sent if the channel       */
          /* described in an ImmediateAssignment message is not acceptable */
          /* to the UE (e.g. the channel is in an unsuported band)         */
          /*****************************************************************/

          /* abort the IA procedure and return to RR IDLE (RR no longer camped) */
          new_con_est_event = rr_ce_abort_immed_assign_procedure(REASON_NOT_AVAILABLE,
                                                                 MM_RR_OTHER_FAILURE,
                                                                 TRUE,
                                                                 gas_id);
        } /* EV_CONNECTION_ABORTION */

        else if (con_est_event == EV_GPRS_CONNECTION_ABORTION)
        {
          if (rr_timer_cancel(T3176, gas_id))
          {
            MSG_GERAN_HIGH_0_G("Stopped T3176 timer");
          }
          // Send NO_PS_ACCESS_IND{DEL_ONE}.
          // Remain in this state until MAC indicates IDLE
          rr_ps_send_imsg_no_ps_access_req(
            FALSE,      // suspend_gmm
            DEL_ONE,    // delete_pdu
            gas_id      // gas_id
          );
        }
        /************************************************************/
        /* EV_T3174_EXPIRY                                          */
        /* Stop the establishment in this case. The PCCO has failed */
        /************************************************************/
        else if ( con_est_event == EV_T3174_EXPIRY )
        {
          rr_send_mph_abort_ra_req(gas_id);

          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }


          if (rr_ce_for_gprs_access(gas_id))
          {
            (void) rr_timer_cancel(T3146, gas_id);

            rr_ps_send_imsg_no_ps_access_req(
              FALSE,      // suspend_gmm
              DEL_ONE,    // delete_pdu
              gas_id      // gas_id
            );
          }
          else
          {
            (void) rr_timer_cancel(T3126, gas_id);

            /* RR_ABORT_IND is only sent for G2G PCCO, for X2G CCO it is
            not required by NAS. */
            if (!rr_x2g_cco_in_progress(gas_id))
            {
              rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
            }
            rr_ps_send_imsg_no_ps_access_req(
              FALSE,      // suspend_gmm
              NO_DEL,     // delete_pdu
              gas_id      // gas_id
            );
          }

          /* Make the current cell forbidden as it should not be selected
             unless there are no other suitable cells. */
          rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
          rr_start_forbidden_timer(gas_id);

          rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);
          rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY;
          rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
        }

        /************************************************************/
        /* EV_T3176_EXPIRY                                          */
        /* Stop the establishment if is for single block. In such a */
        /* case MAC is trying to send a PCCF. If not singl block MAC*/
        /* is also establishing for data transfer.                  */
        /************************************************************/
        else if ( con_est_event == EV_T3176_EXPIRY )
        {
          MSG_GERAN_HIGH_0_G("T3176 expired, T3146 stopped");
          (void)rr_timer_cancel(T3146, gas_id);

          /* abort any Random Access's in progress and re-camp L1*/
          rr_send_mph_abort_ra_req(gas_id);

          // Record the RR event to be returned once L1 in idle
          rr_conn_est_info_ptr->return_rr_event = EV_T3176_TIMEOUT;

          // Send request to put L1 in idle. This will return RR_L1_IDLE_OMSG_START_IDLE_CNF
          rr_l1_idle_send_imsg_start_idle_req(gas_id);
        }

        /************************************************************/
        /* EV_T3176_TIMEOUT                                         */
        /* Notify the calling state machine of T3176 expiry         */
        /************************************************************/
        else if ( con_est_event == EV_T3176_TIMEOUT )
        {
          new_rr_event = EV_T3176_TIMEOUT;
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        }

        else if (con_est_event == EV_IMMEDIATE_ASSIGNMENT)
        {
          /******************************************************************/
          /* The previously IMMEDIATE ASSIGNMENT message was for the current*/
          /* mobile. A layer 1 physical channel has been requested          */
          /******************************************************************/
          rr_conn_est_info_ptr->con_est_state = RR_CP_AWAITING_PHYSICAL_LINK;

          /* received a valid IA message, cancel the T3126 timer */
          MSG_GERAN_HIGH_0_G("Stopping T3126");
          (void)rr_timer_cancel(T3126, gas_id);
        } /* EV_IMMEDIATE_ASSIGNMENT */

        /***********************************************************************/
        /* The previous IMMEDIATE ASSIGNMENT message is for the current mobile */
        /* and was a GPRS IA. MAC has been sent a request for establishment    */
        /***********************************************************************/
        else if (con_est_event == EV_GPRS_IMMEDIATE_ASSIGNMENT)
        {
          /* abort any Random Access's in progress (L1 stays in RACH state) */
          rr_send_mph_abort_ra_req(gas_id);

          /* Conn Establishment done, MAC handles everything else from here on */
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
          new_rr_event = EV_GRR51_GO_TO_PACKET_IDLE;

          /* received a valid IA message, cancel the T3146 timer */
          MSG_GERAN_HIGH_0_G("Stopping T3146");
          (void)rr_timer_cancel(T3146, gas_id);
        }

        else if (con_est_event == EV_ASSIGNMENT_REJECTION)
        {
          /****************************************************************/
          /* RR should not send any more RACH messages after the IAREJ is */
          /* received. Ask L1 to abort any that are still pending.        */
          /****************************************************************/
          rr_send_mph_abort_ra_req(gas_id);

          /* TODO: Using state of T3174 to determine whether a (P)CCO is in
          progress is potentially unsafe and should be reworked. */
          if (rr_timer_is_running(T3174, gas_id))
          {
            if (rr_ce_for_gprs_access(gas_id))
            {
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                DEL_ONE,    // delete_pdu
                gas_id      // gas_id
              );
            }
            else
            {
              if (rr_x2g_cco_in_progress(gas_id) == FALSE)
              {
                rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
              }

              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                NO_DEL,     // delete_pdu
                gas_id      // gas_id
              );
            }

            /* Make the current cell forbidden as it should not be selected
               unless there are no other suitable cells. */
            rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
            rr_start_forbidden_timer(gas_id);

            rr_resel_set_pccf_cause_to_assignment_reject_on_target(gas_id);
            rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_IA_REJECT;
            rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
          }
          else
          {
            /*************************************************************/
            /* The network has sent an ImmediateAssignmentReject message */
            /* indicating that it does not have any channels available   */
            /* to establish a connection with the mobile at this time.   */
            /* Network may include a T3122 value to discourage mobile    */
            /* from accessing the network again for a while to reduce    */
            /* unnecessary traffic                                       */
            /* See GSM04.08 3.3.1.1.3.2                                  */
            /*************************************************************/

            rr_conn_est_info_ptr->con_est_state = RR_CP_ASSIGNMENT_REJECTION_RECEIVED;

            /**
             * According to 44.018: 
             *  
             * As an option the mobile station may return to CCCH idle mode as soon as it has received 
             * responses from the network on all, or in case more than 3 were sent the last 3, of its CHANNEL 
             * REQUEST messages. Hence to summarise: 
             *  
             * 1. If mobile sent one channel request then as soon as mobile receives IAREJ for this channel 
             * request it can return to CCCH idle mode. 
             *  
             * 2. If mobile sent two channel requests then as soon as mobile receives IAREJ for both channel 
             * requests it can return to CCCH idle mode. 
             * If network has only sent IAREJ for one of the two channel requests then MS is to wait for 
             * response to the other channel request. 
             *  
             * 3. If mobile sent three channel requests then as soon as mobile receives IAREJ for all three 
             * channel requests it can return to CCCH idle mode. 
             * If network has sent IAREJ for less than three channel requests then MS is to wait for a 
             * response for the remaining channel requests. 
             *  
             * 4. If mobile sent more than three channel requests then as soon as mobile receives IAREJ for 
             * the last 3 channel requests it can return to CCCH idle mode. 
             */
             
            /* Here the rach counter will be set to max and hence no need to decrement the count */
            MSG_GERAN_HIGH_1_G("No. of RACH sent in MAX tried state is %d ",rr_conn_est_info_ptr->channel_request_counter);
            MSG_GERAN_HIGH_1_G("No. of IArej received is %d ",rr_conn_est_info_ptr->immd_assign_rej_counter);
          
            if(rr_conn_est_info_ptr->channel_request_counter == rr_conn_est_info_ptr->immd_assign_rej_counter)
            {
              if ( IS_GPRS_ACCESS_REASON(rr_conn_est_info_ptr->access_reason) )
              {
                MSG_GERAN_HIGH_0_G("No. of IAREJ match with No. of RACH hence implicly expire T3146 ");
               (void)rr_timer_cancel(T3146, gas_id);
                new_con_est_event = EV_T3146_EXPIRY;
                rr_conn_est_info_ptr->immd_assign_rej_counter = 0;
              }
              else
              {
                MSG_GERAN_HIGH_0_G("No. of IAREJ match with No. of RACH hence implicly expire T3126 ");
                (void)rr_timer_cancel(T3126, gas_id);  
                new_con_est_event = EV_T3126_EXPIRY;
                rr_conn_est_info_ptr->immd_assign_rej_counter = 0;
              }
            }   
          }
        } /* EV_ASSIGNMENT_REJECTION */

        else if (con_est_event == EV_T3126_EXPIRY)
        {
          /************************************************************/
          /* T3126 has expired. The network has not responded to the  */
          /* CHANNEL_REQUEST messages in time. The IA procedure needs */
          /* to be aborted.                                           */
          /************************************************************/

          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_x2g_cco_in_progress(gas_id) == FALSE)
          {
            (void) rr_gmm_suspend_gprs(gas_id);
          }

          /* T3126 timer remains in an EXPIRED state - important for reselection */

          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_TO_RACH_FAILURE,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );

          if (rr_timer_is_running(T3174, gas_id))
          {
            MSG_GERAN_HIGH_0_G("T3126 expired, PCCO in progress");

            /* Make the current cell forbidden as it should not be selected
            unless there are no other suitable cells. */
            rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
            rr_start_forbidden_timer(gas_id);

            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
            }

            rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);
            rr_send_mph_abort_ra_req(gas_id);
            /* This call to block PS access isn't strictly necessary as MAC
            should already be blocked (for the CS connection establishment)
            but it keeps the state transitions the same as for the T3146 expiry
            case. */
            rr_ps_send_imsg_no_ps_access_req(
              FALSE,      // suspend_gmm
              NO_DEL,     // delete_pdu
              gas_id      // gas_id
            );
            rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_NO_RESPONSE;
            rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
          }
          else
          {
            // Default abort cause is RANDOM_ACCESS_FAILURE for T3126 expiry
            byte abort_cause = RANDOM_ACCESS_FAILURE;

            // expect for emergency calls, where silent re-dial needs to be suppressed
            if(rr_ce_access_reason(gas_id) == MM_RR_EMERGENCY_CALL)
            {
              abort_cause = RR_ABORT_REDIAL_NOT_ALLOWED;
            }

            MSG_GERAN_HIGH_0_G("Timer T3126 Expired");

            /* Make the current cell forbidden as it should not be selected
            unless there are no other suitable cells. */
            rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
            rr_start_forbidden_timer(gas_id);

            /* RR is no longer considered camped since this is a reselection trigger */
            RRCE_SET_CAMPED_FLAG(FALSE, gas_id);

            /***********************************************************************/
            /* abort the IA procedure and say that RR should not return to idle (  */
            /* RR is not really in idle but since a reselection is being performed */
            /* there is no point returning to idle now)                            */
            /***********************************************************************/
            (void)rr_ce_abort_immed_assign_procedure(abort_cause,
                                                     MM_RR_OTHER_FAILURE,
                                                     FALSE, gas_id);
            rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

            /* store the reselection trigger */
            rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_T3126_TIMEOUT, gas_id);

            /*****************************************************************/
            /* force a cell reselection since the IA procedure failed:if the */
            /* surround measurements are too old, perform a power scan first */
            /*****************************************************************/
            if (rr_timer_has_expired(GRR_SURROUND_MEAS_TIMER, gas_id))
            {
              /* reset the timer to clear its expired state */
              (void)rr_timer_cancel(GRR_SURROUND_MEAS_TIMER, gas_id);
              new_rr_event = RR_EV_START_RESELECTION_POWER_SCAN;
              MSG_GERAN_MED_1_G("Surround meas older than %dms: perform RPS",
                      GRR_SURROUND_MEAS_TIMEOUT);
            }
            else
            {
              new_rr_event = EV_FORCE_CELL_RESELECTION;
            }
          }
        } /* EV_T3126_EXPIRY */

        /************************************************************/
        /* T3146 has expired. The network has not responded to the  */
        /* CHANNEL_REQUEST messages in time. The GPRS IA procedure  */
        /* needs to be aborted.                                     */
        /************************************************************/
        else if (con_est_event == EV_T3146_EXPIRY)
        {
          /* T3146 timer remains in an EXPIRED state - important for reselection */

          if (rr_timer_is_running(T3174, gas_id))
          {
            MSG_GERAN_HIGH_0_G("T3146 expired, PCCO in progress");
            /* Make the current cell forbidden as it should not be selected
            unless there are no other suitable cells. */
            rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
            rr_start_forbidden_timer(gas_id);

            rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);
            rr_send_mph_abort_ra_req(gas_id);

            rr_ps_send_imsg_no_ps_access_req(
              FALSE,      // suspend_gmm
              DEL_ONE,    // delete_pdu
              gas_id      // gas_id
            );

            rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_NO_RESPONSE;
            rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
          }
          else
          {
            if (rr_timer_cancel(T3176, gas_id))
            {
              MSG_GERAN_HIGH_0_G("Stopped T3176 timer");
            }

            MSG_GERAN_HIGH_0_G("Timer T3146 expired - no response");

            /* Make the current cell forbidden as it should not be selected
            unless there are no other suitable cells. */
            rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
            rr_start_forbidden_timer(gas_id);

            /* RR is no longer considered camped since this is a reselection trigger */
            RRCE_SET_CAMPED_FLAG(FALSE, gas_id);

            /* As TBF establishment has failed, suspend MAC and instruct
            RLC to delete it's current PDU. */
            rr_ps_send_imsg_no_ps_access_req(
              FALSE,      // suspend_gmm
              DEL_ONE,    // delete_pdu
              gas_id      // gas_id
            );

            // Wait for PS access blocked confirmation
            rr_conn_est_info_ptr->con_est_state = RR_CP_T3146_EXPIRED;
          }
        }

        /**********************************************************************/
        /* This timer is used to receive the 2nd of a 2 part IA msg within a  */
        /* specified time. If the timer has expired then reset the flag which */
        /* indicates a 2nd msg is being waited for.                           */
        /**********************************************************************/
        else if (con_est_event == EV_TMULTIFRM_EXPIRY)
        {
          MSG_GERAN_HIGH_1_G("Timer TMULTIFRM expired in state %d",(int)(rr_conn_est_info_ptr->con_est_state));
          /* Reset the flag, since the 2nd IA msg didn't arrive in time */
          rr_conn_est_info_ptr->waiting_for_2nd_msg = FALSE;
          rr_gprs_two_part_assignment_failure(gas_id);

          // A two-part DL IA was expected but the 2nd part has not been received.
          // Discard any processing data
          rr_tbf_est_discard_data(gas_id);
        }

        /*****************************************************************/
        /* RR_EV_MM_REQ_RECEIVED_ABORT                                   */
        /* An abort has come from NAS                                    */
        /*****************************************************************/
        else if (con_est_event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_TO_UPPER_LAYER_TRIGGER,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );

          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          /* complete abort processing; a return to idle is allowed if appropiate */
          new_con_est_event = rr_ce_complete_mm_abort_processing(TRUE, gas_id);
        }

        /*****************************************************************/
        /* EV_GO_TO_IDLE                                                 */
        /* EV_NO_BCCH_FOUND                                              */
        /* RR_EV_PROCESS_SAVED_MESSAGE                                   */
        /* Processing is complete, return Conn Est to Idle               */
        /*****************************************************************/
        else if ( ( con_est_event == EV_GO_TO_IDLE ) ||
                  ( con_est_event == EV_NO_BCCH_FOUND ) ||
                  ( con_est_event == RR_EV_PROCESS_SAVED_MESSAGE ) )
        {
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
          new_rr_event = con_est_event;
        }

        else if ( con_est_event == EV_START_IDLE )
        {
          rr_l1_idle_send_imsg_start_idle_req(gas_id);
          rr_conn_est_info_ptr->starting_idle = TRUE;
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        }

        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
      } /* RR_CP_MAX_RETRIES_ATTEMPTED */
      break;

      /********************************************************/
      /* RR received an ImmediateAssignmentReject OTA message */
      /* and is waiting for either the network to send an     */
      /* immediate assignment message for the mobile or for   */
      /* T3126 to expire                                      */
      /********************************************************/
      case RR_CP_ASSIGNMENT_REJECTION_RECEIVED:
      {
        if ((con_est_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if ((message_set == MS_RR_L2) && (message_id == (byte) DL_UNIT_DATA_IND))
          {
            // The DL_UNIT_DATA_IND could contain IA or IA Reject
            new_con_est_event = rr_con_est_process_dl_unit_data_ind(
              &message_ptr->dl_unit_data_ind,   // message_ptr
              FALSE,                            // rr_in_idle
              TRUE,                             // ignore_ia_reject
              gas_id                            // gas_id
            );
          } /* DL_UNIT_DATA_IND */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RANDOM_ACCESS_CNF))
          {
            /******************************************************************/
            /* Can be received in certain race conditions. Should be ignored. */
            /******************************************************************/
          } /* DL_RANDOM_ACCESS_CNF */

          /*********************************/
          /* Handling of the DSC messaging */
          /*********************************/
          else if ((message_set == MS_RR_L1) && (message_id == (byte)MPH_BLOCK_QUALITY_IND))
          {
            new_con_est_event = rr_ce_handle_block_quality(
              (mph_block_quality_ind_T *)message_ptr,
              gas_id
            );
          }

          else
          {
            RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
          }
        } /* EV_INPUT_MESSAGE */

        else if((con_est_event == EV_DSF_RESELECTION) || (con_est_event == EV_DSF_FOR_DENIALS))
        {
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          if (rr_timer_is_running(T3174, gas_id))
          {
            /* Make the current cell forbidden as it should not be selected
               unless there are no other suitable cells. */
               
            rr_ce_handle_scell_dsf_event(con_est_event, gas_id);
            if (rr_ce_for_gprs_access(gas_id))
            {
              /* Cancel T3146, if it's running */
              if (rr_timer_cancel(T3146, gas_id))
              {
                MSG_GERAN_HIGH_0_G("Stopped T3146 timer");
              }

              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                DEL_ONE,    // delete_pdu
                gas_id      // gas_id
              );
            }
            else
            {
              /* Cancel T3126, if it's running */
              if (rr_timer_cancel(T3126, gas_id))
              {
                MSG_GERAN_HIGH_0_G("Stopped T3126 timer");
              }

              if (rr_x2g_cco_in_progress(gas_id) == FALSE)
              {
                rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
              }

              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                NO_DEL,     // delete_pdu
                gas_id      // gas_id
              );
            }

            MSG_GERAN_HIGH_0_G("DSF Failure happened while PCCO in progress");
            rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);

            rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_DSF;
            rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
          }
          else
          {
            /* RR is no longer considered camped since this is a reselection trigger */
            RRCE_SET_CAMPED_FLAG(FALSE, gas_id);

            if (rr_ce_for_gprs_access(gas_id))
            {
              if (rr_timer_cancel(T3176, gas_id))
              {
                MSG_GERAN_HIGH_0_G("Stopped T3176 timer");
              }
              /* Cancel T3146, if it's running */
              if (rr_timer_cancel(T3146, gas_id))
              {
                MSG_GERAN_HIGH_0_G("Stopped T3146 timer");
              }

              /* As TBF establishment has failed, suspend MAC and instruct
                 RLC to delete it's current PDU. */
              rr_ps_send_imsg_no_ps_access_req(
                FALSE,      // suspend_gmm
                DEL_ONE,    // delete_pdu
                gas_id      // gas_id
              );

              // Record RR event to be returned when PS access blocked
              rr_conn_est_info_ptr->return_rr_event = con_est_event;
            }
            else
            {
              /***********************************************************************/
              /* abort the IA procedure and say that RR should not return to idle (  */
              /* RR is not really in idle but since a reselection is being performed */
              /* there is no point returning to idle now)                            */
              /***********************************************************************/
              (void)rr_ce_abort_immed_assign_procedure(RANDOM_ACCESS_FAILURE,
                                                       MM_RR_OTHER_FAILURE,
                                                       FALSE, gas_id);

              new_rr_event = con_est_event;
            }

            /* store the reselection trigger */
            rr_log_set_reselection_trigger(RR_RESELECTION_TRIGGER_DSF, gas_id);

            /* connection establishment aborted */
            rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

            /* Make the current cell forbidden as it should not be selected
               unless there are no other suitable cells. */
            rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
            rr_start_forbidden_timer(gas_id);
          }
        }

        else if (con_est_event == EV_CONNECTION_ABORTION)
        {
          /*****************************************************************/
          /* The EV_CONNECTION_ABORTION event is sent if the channel       */
          /* described in an ImmediateAssignment message is not acceptable */
          /* to the UE (e.g. the channel is in an unsuported band)         */
          /*****************************************************************/

          /* abort the IA procedure and return to RR IDLE (RR no longer camped) */
          new_con_est_event = rr_ce_abort_immed_assign_procedure(REASON_NOT_AVAILABLE,
                                                                 MM_RR_OTHER_FAILURE,
                                                                 TRUE, gas_id);
        } /* EV_CONNECTION_ABORTION */

        else if (con_est_event == EV_IMMEDIATE_ASSIGNMENT)
        {
          /******************************************************************/
          /* The previously IMMEDIATE ASSIGNMENT message was for the current*/
          /* mobile. A layer 1 physical channel has been requested          */
          /******************************************************************/
          rr_conn_est_info_ptr->con_est_state = RR_CP_AWAITING_PHYSICAL_LINK;

          /* received a valid IA message, cancel the T3126 timer */
          MSG_GERAN_HIGH_0_G("Stopping T3126");
          (void)rr_timer_cancel(T3126, gas_id);

          /* stop T3122 and reset access restriction */
          rr_stop_t3122(gas_id);
        } /* EV_IMMEDIATE_ASSIGNMENT */

        /***********************************************************************/
        /* The previous IMMEDIATE ASSIGNMENT message is for the current mobile */
        /* and was a GPRS IA. MAC has been sent a request for establishment    */
        /***********************************************************************/
        else if (con_est_event == EV_GPRS_IMMEDIATE_ASSIGNMENT)
        {
          /* abort any Random Access's in progress (L1 stays in RACH state) */
          rr_send_mph_abort_ra_req(gas_id);

          /* received a valid IA message, cancel the T3146 and T3142 timers */
          MSG_GERAN_HIGH_0_G("Stopping T3146");
          (void)rr_timer_cancel(T3146, gas_id);

          MSG_GERAN_HIGH_0_G("Stopping T3142");
          (void)rr_timer_cancel(T3142, gas_id);

          /* Conn Establishment done, MAC handles everything else from here on */
          new_rr_event = EV_GRR51_GO_TO_PACKET_IDLE;

          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        }

        else if (con_est_event == EV_T3126_EXPIRY)
        {
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_x2g_cco_in_progress(gas_id) == FALSE)
          {
            (void) rr_gmm_suspend_gprs(gas_id);
          }

          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_IMMEDIATE_ASSIGNMENT_REJECT,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );

          /**************************************************************/
          /* T3126 has expired. The network has responded to the        */
          /* CHANNEL_REQUEST bursts with an Immediate Assignment Reject */
          /* message. The reject timer T3122 may still be running, so   */
          /* abort the connection attempt by MM and return to idle now  */
          /**************************************************************/
          MSG_GERAN_HIGH_0_G("T3126 expiry after IAR");

          /* make sure the timer is now in a STOPPED state */
          /* this is important, as the reselection engine checks the timer state */
          (void)rr_timer_cancel(T3126, gas_id);

          /*******************************************************************/
          /* notify sys info control that RR is returning to idle because of */
          /* an immediate assignment reject condition (SI refresh required)  */
          /*******************************************************************/
          rr_gprs_notify_sys_info_ia_reject(TRUE, gas_id);

          /* abort the IA procedure and return to idle mode */
          /* T3122 status is included in the RR_SERVICE_IND */
          new_con_est_event = rr_ce_abort_immed_assign_procedure(RR_ABORT_IA_REJECT_RECEIVED,
                                                                 MM_RR_OTHER_FAILURE,
                                                                 TRUE, gas_id);
        } /* T3126 expiry */

        /*****************************************************************/
        /* T3146 has expired. The network has not responded to the       */
        /* CHANNEL_REQUEST messages in time. The GPRS IA procedure needs */
        /* to be aborted. Since an ImmediateAssignmentReject message     */
        /* was also received, the RR must enter a state that disallows   */
        /* new RR connection establishment attempts until T3142 expires  */
        /*****************************************************************/
        else if (con_est_event == EV_T3146_EXPIRY)
        {
          if (rr_timer_is_running(T3174, gas_id))
          {
            MSG_GERAN_HIGH_0_G("T3146 expired after IA Reject, PCCO in progress");
            rr_resel_set_pccf_cause_to_assignment_reject_on_target(gas_id);

            rr_ps_send_imsg_no_ps_access_req(
              FALSE,      // suspend_gmm
              DEL_ONE,    // delete_pdu
              gas_id      // gas_id
            );

            rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_NO_RESPONSE;
            rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
          }
          else
          {
            if (rr_timer_cancel(T3176, gas_id))
            {
              MSG_GERAN_HIGH_0_G("Stopped T3176 timer");
            }

            /* make sure the timer is now in a STOPPED state */
            /* this is important, as the reselection engine may check the timer state */
            (void)rr_timer_cancel(T3146, gas_id);

            rr_ps_send_imsg_no_ps_access_req(
              FALSE,      // suspend_gmm
              DEL_ONE,    // delete_pdu
              gas_id      // gas_id
            );

            // Wait for PS access blocked confirmation then return to Packet Idle
            rr_conn_est_info_ptr->con_est_state = RR_CP_T3146_EXPIRED_AFTER_REJECT;
          }
        }

        /*****************************************************************/
        /* RR_EV_MM_REQ_RECEIVED_ABORT                                   */
        /* An abort has come from NAS                                    */
        /*****************************************************************/
        else if (con_est_event == RR_EV_MM_REQ_RECEIVED_ABORT)
        {
          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_TO_UPPER_LAYER_TRIGGER,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );
 
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          /* complete abort processing; a return to idle is allowed if required */
          new_con_est_event = rr_ce_complete_mm_abort_processing(TRUE, gas_id);
        }

        /*****************************************************************/
        /* EV_GO_TO_IDLE                                                 */
        /* L1 has returned to Idle, return to GRR                        */
        /*****************************************************************/
        else if ( con_est_event == EV_GO_TO_IDLE )
        {
          /* As T3126 expired after a IARej was received and before any other IA */
          /* was received (which sets the IARej flag), further calls are prohibited */
          /* until T3122 expires. */
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

          new_rr_event = EV_GO_TO_IDLE;
        }

        else if ( con_est_event == EV_START_IDLE )
        {
          rr_l1_idle_send_imsg_start_idle_req(gas_id);
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        }

        /*****************************************************************/
        /* EV_NO_BCCH_FOUND                                              */
        /* RR_EV_PROCESS_SAVED_MESSAGE                                   */
        /* Processing is complete, return Conn Est to Idle               */
        /*****************************************************************/
        else if ( ( con_est_event == EV_NO_BCCH_FOUND ) ||
                  ( con_est_event == RR_EV_PROCESS_SAVED_MESSAGE ) )
        {
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
          new_rr_event = con_est_event;
        }

        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
      } /* RR_CP_ASSIGNMENT_REJECTION_RECEIVED */
      break;

      /*********************************************************************/
      /* RR is waiting for an indication from L1 that the physical channel */
      /* has been set up. Once the physical channel is setup, the L2 SAPI0 */
      /* link can be established.                                          */
      /*********************************************************************/
      case RR_CP_AWAITING_PHYSICAL_LINK:
      {
        if ((con_est_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          /***************************************************************/
          /* The Layer 1 has set up the physical channels. We have also  */
          /* to initiate the Link establishment at the layer 2           */
          /***************************************************************/
          if ((message_set == MS_RR_L1) && (message_id == (byte) MPH_IMMED_ASSIGN_CNF))
          {
            if (message_ptr->mph_immed_assign_cnf.error_code == NO_ERROR)
            {
              /* L1 has successfully set up the physical channel   */
              rr_ce_l1_in_dedicated_mode(MPH_IMMED_ASSIGN_CNF, gas_id);

              /* start the process of setting up the L2 SAPI0 Link */
              rr_ce_send_first_message(gas_id);
              rr_conn_est_info_ptr->con_est_state = RR_CP_AWAITING_LINK_ESTABLISHMENT;

              new_rr_event = EV_RR_EST_L1_IN_DED;
            }
            else
            {
              /* abort the IA procedure and return to RR IDLE */
              new_con_est_event = rr_ce_abort_immed_assign_procedure(REASON_NOT_AVAILABLE,
                                                                     MM_RR_OTHER_FAILURE,
                                                                     TRUE, gas_id);
            }
          } /* MPH_IMMED_ASSIGN_CNF */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RANDOM_ACCESS_CNF))
          {
            /******************************************************************/
            /* Can be received in certain race conditions. Should be ignored. */
            /******************************************************************/
          } /* DL_RANDOM_ACCESS_CNF */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_UNIT_DATA_IND))
          {
            /* can be ignored in this state */
          }

          else
          {
            RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
          }
        } /* EV_INPUT_MESSAGE */

        /*****************************************************************/
        /* EV_GO_TO_IDLE                                                 */
        /* L1 has returned to Idle, return to GRR                        */
        /*****************************************************************/
        else if ( con_est_event == EV_GO_TO_IDLE )
        {
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
          new_rr_event = EV_GO_TO_IDLE;
        }

        else if ( con_est_event == EV_START_IDLE )
        {
          rr_l1_idle_send_imsg_start_idle_req(gas_id);
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        }

        /*****************************************************************/
        /* RR_EV_MM_REQ_RECEIVED_ABORT                                   */
        /* Implicit NAS abort                                            */
        /*****************************************************************/
        else if ( con_est_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_TO_UPPER_LAYER_TRIGGER,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );

          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }


          if (rr_x2g_cco_in_progress(gas_id))
          {
            rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_ABORTED_HIGH_PRIORITY_REQUEST;
          }

          /* Wait for the MPH_IMMED_ASSIGN_CNF in RR_CP_IA_ABORTED state, then abort */
          rr_conn_est_info_ptr->con_est_state = RR_CP_IA_ABORTED;
        }

        /**
         * T3174 has expired.  RR is still waiting for a confirmation from L1,
         * so switch state to allow this to be received before releasing the
         * connection to allow the PCCO failure procedure to take place.
         */
        else if (con_est_event == EV_T3174_EXPIRY)
        {
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_x2g_cco_in_progress(gas_id) == FALSE)
          {
            (void) rr_gmm_suspend_gprs(gas_id);
          }

          /* Make the current cell forbidden as it should not be selected
             unless there are no other suitable cells. */
          rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
          rr_start_forbidden_timer(gas_id);

          if (rr_x2g_cco_in_progress(gas_id))
          {
            rr_conn_set_release_cause(RELEASE_CAUSE_X2G_CCO_FAILURE, gas_id);
          }
          else
          {
            rr_conn_set_release_cause(RELEASE_CAUSE_PCCO_FAILURE, gas_id);
          }

          rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);
          rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY;
          rr_conn_est_info_ptr->con_est_state = RR_CP_IA_ABORTED;
        }

        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
      } /* RR_CP_AWAITING_PHYSICAL_LINK */
      break;

      /***************************************************************/
      /* RR has started the process of establishing the L2 SAPI0     */
      /* link and is waiting for an indication from L2 that the link */
      /* has been established                                        */
      /***************************************************************/
      case RR_CP_AWAITING_LINK_ESTABLISHMENT:
      {
        if ((con_est_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_L2)
          {
            if (message_id == (byte) DL_ESTABLISH_CNF)
            {
              /**********************************************/
              /* Indication from L2 that the SAPI0 link has */
              /* been successfully established              */
              /**********************************************/
              new_rr_event = rr_ce_handle_communication_establishment(gas_id);

              rr_conn_est_info_ptr->con_est_state = RR_CP_LINK_ESTABLISHED;

              /*
              44.018 3.4.25.2 MS in class B mode of operation
              The GPRS suspension procedure is initiated by the mobile station by sending
              a GPRS SUSPENSION REQUEST message with the appropriate suspension cause.
              This can be done as early as possible after access but shall be done after
              sending a CLASSMARK CHANGE message
              */
              if (grr_determine_if_gprs_suspension_should_be_sent_ota(gas_id))
              {
                grr_send_gprs_suspension_ota(
                  (establishment_cause_T) rr_conn_est_info_ptr->access_reason,
                  gas_id
                );
              }
            } /* DL_ESTABLISH_CNF */

            /*******************************************************/
            /* L2 sends the DL_RELEASE_IND message if a contention */
            /* resolution information field mismatch is detected   */
            /*******************************************************/
            else if (message_id == (byte) DL_RELEASE_IND)
            {
              MSG_GERAN_HIGH_0_G("Contention resolution failure detected");
              /***********************************************************/
              /* GSM 04.18 3.3.1.1.5 indicates that in cases of          */
              /* contention resolution information field mismatch        */
              /* RR should repeat the connection establishment procedure */
              /* once before giving up                                   */
              /***********************************************************/

              if (rr_x2g_cco_in_progress(gas_id))
              {
                rr_log_gsm_access_failure(
                  RR_LOG_ACCESS_FAILURE_DUE_L2_LINK_ESTABLISHMENT_FAILURE,
                  rr_ce_access_reason(gas_id),
                  rr_conn_est_info_ptr->channel_request_counter,
                  gas_id
                );

                MSG_GERAN_HIGH_0_G("Second time, releasing connection");
                /* cannot repeat again, release the physical channel and abort */
                rr_conn_est_info_ptr->release_reason = RR_CONNECTION_FAILURE;
                rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_CONTENTION_RESOLUTION_FAILURE;
                rr_ce_perform_ia_connection_release(FALSE, gas_id);
                rr_conn_est_info_ptr->repeat_assignment_procedure = FALSE;
              }
              else
              /* if the connection establishment procedure has already been repeated */
              if (rr_conn_est_info_ptr->procedure_counter >= 1)
              {
                // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
                // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
                // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
                // PDUs will be deleted for other scenarios.
                (void) rr_gmm_suspend_gprs(gas_id);

                rr_log_gsm_access_failure(
                  RR_LOG_ACCESS_FAILURE_DUE_L2_LINK_ESTABLISHMENT_FAILURE,
                  rr_ce_access_reason(gas_id),
                  rr_conn_est_info_ptr->channel_request_counter,
                  gas_id
                );

                MSG_GERAN_HIGH_0_G("Second time, releasing connection");
                /* cannot repeat again, release the physical channel and abort */
                rr_conn_est_info_ptr->release_reason = RR_CONNECTION_FAILURE;
                rr_ce_perform_ia_connection_release(FALSE, gas_id);
                rr_conn_est_info_ptr->repeat_assignment_procedure = FALSE;

                if (rr_pcco_in_progress(gas_id))
                {
                  rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);
                }
              }
              else /* connection establishment procedure has not been repeated */
              {
                MSG_GERAN_HIGH_0_G("First time, repeating IA procedure");
                /* release the physical channel asking L1 to camp on serving cell
                 * and repeat the procedure
                 */
                rr_ce_perform_ia_connection_release(TRUE, gas_id);
                rr_conn_est_info_ptr->repeat_assignment_procedure = TRUE;
              }

              /************************************************************/
              /* depending on the value of repeat_assignment_procedure RR */
              /* will either repeat or abort the IA procedure when the    */
              /* physical channel is released. For now, RR must wait for  */
              /* the channel release CNF from L1.                         */
              /************************************************************/

              rr_conn_est_info_ptr->con_est_state = RR_CP_DISCONNECT_PHYSICAL_LINK;
            } /* DL_RELEASE_IND */

            else if (message_id == (byte) MDL_ERROR_IND)
            {
              /******************************************************/
              /* MDL_ERROR_IND is an indication from L2 that there  */
              /* was an L2 level error while establishing the SAPI0 */
              /* LINK                                               */
              /******************************************************/
              MSG_GERAN_HIGH_0_G("RR connection: lower layer failure");

              // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
              // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
              // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
              // PDUs will be deleted for other scenarios.
              if (rr_ce_for_gprs_access(gas_id) == FALSE)
              {
                if (rr_x2g_cco_in_progress(gas_id) == FALSE)
                {
                  (void) rr_gmm_suspend_gprs(gas_id);
                }
              }

              rr_log_gsm_access_failure(
                RR_LOG_ACCESS_FAILURE_DUE_L2_LINK_ESTABLISHMENT_FAILURE,
                rr_ce_access_reason(gas_id),
                rr_conn_est_info_ptr->channel_request_counter,
                gas_id
              );

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
              if (RR_CE_EDTM_REQUEST(rr_conn_est_info_ptr->request_references[0]))
              {
                /* for EDTM conections, treat this like an implicit NAS abort */
                new_con_est_event = RR_EV_MM_REQ_RECEIVED_ABORT;
              }
              else
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
              {
                /**********************************************/
                /* Release the physical channel and abort the */
                /* IA procedure (repeat set to FALSE)         */
                /**********************************************/
                if ((rr_x2g_cco_in_progress(gas_id) == FALSE) &&
                     message_ptr->mdl_error_ind.error_code == T200_EXPIRY)
                {
                  rr_conn_est_info_ptr->t200_expired = TRUE;
                }
                else
                {
                  rr_conn_est_info_ptr->t200_expired = FALSE;
                }

                rr_conn_est_info_ptr->release_reason = RR_CONNECTION_FAILURE;
                rr_ce_perform_ia_connection_release(FALSE, gas_id);

                /* This may not be the correct cause to use here, but there isn't
                really an appropriate one for the error case. */
                rr_conn_est_info_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_NO_RESPONSE;

                {
                  /*flush the SI cache as the L2 link failed and this might be due to stale SI data*/
                   gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

                   if (scell_info_ptr->gsm.valid_data_flag &&
                       scell_info_ptr->gsm.cell_identity_valid_flag)
                   {
                     MSG_GERAN_HIGH_0_G("L2 link failed flush the SI cache");
                     rr_csi_flush_data(&(scell_info_ptr->gsm.location_area_identification),
                                       &(scell_info_ptr->gsm.cell_identity),
                                       CSI_FLUSH_CONTENTION_FAILURE,
                                       gas_id);
                   }
                }

                rr_conn_est_info_ptr->con_est_state = RR_CP_DISCONNECT_PHYSICAL_LINK;
              }
            } /* MDL_ERROR_IND */

            else if (message_id == (byte) DL_UNIT_DATA_IND)
            {
              /* ignored in this state */
            }

            else
            {
              RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
            }
          } /* set is L2 */

          else if (message_set == MS_RR_L1)
          {
            if (message_id == (byte) MPH_BLOCK_QUALITY_IND)
            {
              /* ignored in this state */
            }
            else
            {
              RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
            }
          } /* set is L1 */

          else
          {
            RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
          }
        } /* EV_INPUT_MESSAGE */

        /*****************************************************************/
        /* RR_EV_MM_REQ_RECEIVED_ABORT                                   */
        /* Implicit NAS abort                                            */
        /*****************************************************************/
        else if ( con_est_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
#ifdef FEATURE_LTE        
          boolean g2l_blind_redir_after_csfb_required = FALSE;
          boolean g2l_blind_redir_required = FALSE;
#endif /* FEATURE_LTE */
          MSG_GERAN_HIGH_0_G("Aborting RR connection via conn release");

          rr_log_gsm_access_failure(
            RR_LOG_ACCESS_FAILURE_DUE_TO_UPPER_LAYER_TRIGGER,
            rr_ce_access_reason(gas_id),
            rr_conn_est_info_ptr->channel_request_counter,
            gas_id
          );

          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }

#ifdef FEATURE_LTE
              /* Check, G2L Blind Redirections for CS Connections ONLY */
              g2l_blind_redir_after_csfb_required = 
              rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id);
              g2l_blind_redir_required = 
              rr_connection_release_get_g2l_blind_redir_flag(gas_id);
#endif /* FEATURE_LTE */
          }

          /* set up connection release to handle the messaging from here */
          (void) rr_connection_release(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_connection_release(EV_CONNECTION_EXISTS, NULL, gas_id);

          /* set the release cause here (conn release has just reset) */
          rr_conn_set_release_cause(RELEASE_CAUSE_IMPLICIT_NAS_REQUEST, gas_id);

          /* reset the Connection Establishment state machine */
          new_con_est_event = EV_RESET_SOFTWARE;
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

#ifdef FEATURE_LTE
          /* Prior to NAS aborting connection, it is possible for G2L blind redirection
             to have been enabled.  Informing connection release of
             EV_RESET_SOFTWARE will clear the flag, so we re-enable the flag after
             informing rr_connection_release here. */
            if (g2l_blind_redir_after_csfb_required)
            {
              rr_connection_release_set_g2l_blind_redir_after_csfb_flag(gas_id);
            }
		  
            if (g2l_blind_redir_required)
            {
              rr_connection_release_set_g2l_blind_redir_flag(gas_id);
            }
#endif /* FEATURE_LTE */

          /* and output an abort event to RR control */
          new_rr_event = EV_CONNECTION_ABORTION;
        }

        else if (con_est_event == EV_T3174_EXPIRY)
        {
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          /* Inform the RR connection release state machine that a connection
          exists, so that it can handle it's release appropriately.  This must
          be done before the call to rr_conn_set_release_cause(), or the
          release cause will be cleared. */

          (void) rr_connection_release(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_connection_release(EV_CONNECTION_EXISTS, NULL, gas_id);

          /* Make the current cell forbidden as it should not be selected
             unless there are no other suitable cells. */
          rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
          rr_start_forbidden_timer(gas_id);

          if (rr_x2g_cco_in_progress(gas_id))
          {
            rr_conn_set_release_cause(RELEASE_CAUSE_X2G_CCO_FAILURE, gas_id);
          }
          else
          {
            rr_conn_set_release_cause(RELEASE_CAUSE_PCCO_FAILURE, gas_id);
          }

          rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);

          /* reset the Connection Establishment state machine */
          new_con_est_event = EV_RESET_SOFTWARE;
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

          /* and output an abort event to RR control */
          new_rr_event = EV_CONNECTION_ABORTION;
        }

        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
      } /* RR_CP_AWAITING_LINK_ESTABLISHMENT state */
      break;

      /****************************************/
      /* The SAPI0 link is fully established. */
      /****************************************/
      case RR_CP_LINK_ESTABLISHED:
      {
        if (con_est_event == EV_INPUT_MESSAGE)
        {
          RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
        }
        /************************************************************************/
        /* The dedicated channel has been released and RR is going back to idle */
        /************************************************************************/
        else if (con_est_event == EV_CHANNEL_DEACTIVATED)
        {
#ifdef FEATURE_GSM_DTM
          rr_dtm_send_imsg_cs_rel_ind(gas_id);
#endif /* FEATURE_GSM_DTM */

          /* Reset the DTX override tracking */
          rr_dtx_override_reset(gas_id);

          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        }

        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
      } /* RR_CP_LINK_ESTABLISHED */
      break;

      /********************************************************************/
      /* RR was in the process of establishing the RR connection and had  */
      /* allocated a physical channel when it decided that the connection */
      /* establishment could not take place. The RR initiates the release */
      /* of the physical channel and goes into this state to wait for the */
      /* response                                                         */
      /********************************************************************/
      case RR_CP_DISCONNECT_PHYSICAL_LINK :
      {
        if ((con_est_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (message_ptr->rr.header.rr_message_set)
            {
              case RR_L1_IDLE_OMSG:
              {
                switch (message_ptr->rr.header.imh.message_id)
                {
                  case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                  {
                    new_con_est_event = rr_conn_est_info_ptr->return_rr_event;
                    rr_conn_est_info_ptr->return_rr_event = EV_NO_EVENT;
                    break;
                  }

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                }
                break;
              }

              default:
              {
                RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
              }
            }
          }

          else
          /***********************************************************************/
          /* L1->RR: MPH_CHANNEL_RELEASE_CNF                                     */
          /* The physical channel has been released                              */
          /***********************************************************************/
          if ((message_set == MS_RR_L1) && (message_id == (byte) MPH_CHANNEL_RELEASE_CNF))
          {
            /*T200 expired then attempt reselection to another cell. 44.018 3.3.1.15*/
            if (rr_conn_est_info_ptr->t200_expired)
            {             
              byte abort_cause;

              /* expect for emergency calls, where silent re-dial needs to be suppressed*/
              if(rr_ce_access_reason(gas_id) == MM_RR_EMERGENCY_CALL)
              {
                abort_cause = RR_ABORT_REDIAL_NOT_ALLOWED;
              }
              else
              {
                abort_cause = REASON_NOT_AVAILABLE;
              }

              rr_conn_est_info_ptr->t200_expired = FALSE;

              MSG_GERAN_HIGH_0_G("Timer T200 Expired force reselection");

              /* Make the current cell forbidden as it should not be selected
              unless there are no other suitable cells. */
              rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
              rr_start_forbidden_timer(gas_id);

              /* RR is no longer considered camped since this is a reselection trigger */
              RRCE_SET_CAMPED_FLAG(FALSE, gas_id);

              (void)rr_ce_abort_immed_assign_procedure(abort_cause,
                                                       MM_RR_OTHER_FAILURE,
                                                       FALSE, gas_id);
              
              /*****************************************************************/
              /* force a cell reselection since T200 expired:if the            */
              /* surround measurements are too old, perform a power scan first */
              /*****************************************************************/
              if (rr_timer_has_expired(GRR_SURROUND_MEAS_TIMER, gas_id))
              {
                MSG_GERAN_MED_1_G("Surround meas older than %dms: perform RPS", GRR_SURROUND_MEAS_TIMEOUT);
                /* reset the timer to clear its expired state */
                (void)rr_timer_cancel(GRR_SURROUND_MEAS_TIMER, gas_id);
                new_rr_event = RR_EV_START_RESELECTION_POWER_SCAN;     
              }
              else
              {
                new_rr_event = EV_FORCE_CELL_RESELECTION;
              }

              rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
            }
            /* if message indicates success (L1 was able to camp on serving cell) */
            else if (message_ptr->mph_channel_release_cnf.error_code == NO_ERROR)
            {
              /* if the IA procedure should be repeated */
              if (rr_conn_est_info_ptr->repeat_assignment_procedure)
              {
                // Record the RR event to be returned once L1 in idle
                rr_conn_est_info_ptr->return_rr_event = EV_REPEAT_ASSIGNMENT_PROCEDURE;

                // Send request to put L1 in idle. This will return RR_L1_IDLE_OMSG_START_IDLE_CNF
                rr_l1_idle_send_imsg_start_idle_req(gas_id);
              }
              else /* IA procedure should not be repeated */
              {
                if (rr_timer_is_running(T3174, gas_id))
                {
                  if (rr_x2g_cco_in_progress(gas_id) == FALSE)
                  {
                    /* NAS is not informed of CS connection establishment failure
                    in X2G CCO case, this is implied by ACTIVATION_IND sent by
                    the originating RAT when RR returns control to it. */
                    rr_send_abort_ind(rr_conn_est_info_ptr->release_reason, MM_RR_OTHER_FAILURE, gas_id);
                  }

                  rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);

                  rr_ps_send_imsg_no_ps_access_req(
                    FALSE,      // suspend_gmm
                    DEL_ONE,    // delete_pdu
                    gas_id      // gas_id
                  );

                  ASSERT(rr_conn_est_info_ptr->x2g_cco_status != RR_CCO_STATUS_UNDEFINED);
                  rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;

                  rr_conn_est_info_ptr->procedure_counter = 0;
                  rr_conn_est_info_ptr->repeat_assignment_procedure = FALSE;
                }
                else
                {
                  new_con_est_event = rr_ce_abort_immed_assign_procedure(rr_conn_est_info_ptr->release_reason,
                                                                         MM_RR_OTHER_FAILURE,
                                                                         TRUE, gas_id);
                  rr_conn_est_info_ptr->procedure_counter = 0;
                  rr_conn_est_info_ptr->repeat_assignment_procedure = FALSE;
                }
              }
            }
            else /* RR was not able to camp on serving cell */
            {
              /* L1 never sets the error_code to anything other than NO_ERROR
              so this code is redundant. */
              if (rr_timer_is_running(T3174, gas_id))
              {
                MSG_GERAN_ERROR_1_G("Unhandled error_code %d during PCCO",
                  message_ptr->mph_channel_release_cnf.error_code);
              }

              rr_conn_est_info_ptr->procedure_counter = 0;
              rr_conn_est_info_ptr->repeat_assignment_procedure = FALSE;
              rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

              /* RR has lost service, indicate this to MM */
              rr_send_no_service_ind(gas_id);

              /* RR is no longer camped */
              RRCE_SET_CAMPED_FLAG(FALSE, gas_id);

              new_rr_event = EV_NO_BCCH_FOUND;
            }
          } /* MPH_CHANNEL_RELEASE_CNF */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_UNIT_DATA_IND))
          {
            /********************************************************************/
            /* OTA messages are being ignored in this state since page messages */
            /* should not be answered                                           */
            /********************************************************************/
          } /* DL_UNIT_DATA_IND */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RELEASE_IND))
          {
            /* ignore this message in this state, it is probably sent after */
            /* an MDL_ERROR_IND indicating the link has been released */
          }

          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_EST_REQ))
          {
            /******************************************************/
            /* RR_EST_REQ is a request from MM to establish an RR */
            /* connection.                                        */
            /******************************************************/
            MSG_GERAN_ERROR_0_G("MO establishment not allowed (releasing connection)");
            rr_send_abort_ind(REASON_NOT_AVAILABLE, MM_RR_OTHER_FAILURE, gas_id);
            /* remain in the same state waiting for physical channel release */
          } /* RR_EST_REQ message */

          else
          {
            RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
          }
        } /* EV_INPUT_MESSAGE */

        /*****************************************************************/
        /* EV_GO_TO_IDLE                                                 */
        /* L1 has returned to Idle, return to GRR                        */
        /*****************************************************************/
        else if ( con_est_event == EV_GO_TO_IDLE )
        {
          rr_conn_est_info_ptr->t200_expired = FALSE;
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;

          if (rr_pcco_in_progress(gas_id) &&
              rr_conn_est_info_ptr->release_reason == RR_CONNECTION_FAILURE)
          {
            new_rr_event = EV_PCCO_FAILED;
          }
          else
          {
            new_rr_event = EV_GO_TO_IDLE;
          }
        }

        else if ( con_est_event == EV_START_IDLE )
        {
          rr_l1_idle_send_imsg_start_idle_req(gas_id);
          rr_conn_est_info_ptr->starting_idle = TRUE;
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
        }

        else if ( con_est_event == EV_REPEAT_ASSIGNMENT_PROCEDURE )
        {
          rr_conn_est_info_ptr->t200_expired = FALSE;

          /* repeat the immediate assignment procedure once again */
          rr_conn_est_info_ptr->procedure_counter += 1;
          rr_conn_est_info_ptr->repeat_assignment_procedure = FALSE;
          rr_ce_initiate_random_access(gas_id);
          rr_conn_est_info_ptr->con_est_state = RR_CP_SENDING_RANDOM_ACCESS;
          new_rr_event = EV_RR_EST_L2_CONT_RES_FAIL;
        }

        else if ( con_est_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {

          rr_conn_est_info_ptr->t200_expired = FALSE;

 
          // It is possible to get 'stuck' in a loop retrying connection establishment if the signal level
          // is poor and/or the network is not responding. If GMM is not suspended, PDU build-up can occur.
          // This is only done for CS establishment attempts, excluding X2G PCCO. This is because one/all
          // PDUs will be deleted for other scenarios.
          if (rr_ce_for_gprs_access(gas_id) == FALSE)
          {
            if (rr_x2g_cco_in_progress(gas_id) == FALSE)
            {
              (void) rr_gmm_suspend_gprs(gas_id);
            }
          }

          /* Wait for the MPH_CHANNEL_RELEASE_CNF in RR_CP_IA_ABORTED state, then abort */
          rr_conn_est_info_ptr->con_est_state = RR_CP_IA_ABORTED;
        }

        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
      } /* RR_CP_DISCONNECT_PHYSICAL_LINK state */
      break;

      /********************************************************************************/
      /* RR has received an RR_ABORT_REQ from MM and is waiting for the connection to */
      /* be released before responding                                                */
      /********************************************************************************/
      case RR_CP_IA_ABORTED:
      {
        if ((con_est_event == EV_INPUT_MESSAGE) && (message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (message_ptr->rr.header.rr_message_set)
            {
              case RR_L1_IDLE_OMSG:
              {
                switch (message_ptr->rr.header.imh.message_id)
                {
                  case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                  {
                    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

                    if (rr_conn_est_info_ptr->return_rr_event == EV_REPEAT_ASSIGNMENT_PROCEDURE)
                    {
                      MSG_GERAN_MED_2_G("RR abandons IA repeat due to MM abort (flag=%1d,count=%1d)",
                              (int)rr_conn_est_info_ptr->repeat_assignment_procedure,
                              (int)rr_conn_est_info_ptr->procedure_counter);

                      rr_conn_inform_mm_of_channel_release(FALSE, gas_id);

                      /* then reset the repeat assignment fields */
                      rr_conn_est_info_ptr->procedure_counter = 0;
                      rr_conn_est_info_ptr->repeat_assignment_procedure = FALSE;
                      rr_conn_est_info_ptr->return_rr_event = EV_NO_EVENT;
                    }

                    if (rr_ce_should_confirm_conn_rel_to_nas(gas_id))
                    {
                      rr_send_service_ind(
                        MM_AS_SERVICE_AVAILABLE,
                        &(scell_info_ptr->gsm.location_area_identification),
                        &(scell_info_ptr->gsm),
                        gas_id
                      );
                    }

                    rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
                    new_rr_event = EV_GO_TO_IDLE;
                    break;
                  }

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                }
                break;
              }

              default:
              {
                RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
              }
            }
            break;
          }

          else
          /****************************************************************/
          /* The response from L1 for the release of the physical channel */
          /****************************************************************/
          if ((message_set == MS_RR_L1) && (message_id == (byte) MPH_CHANNEL_RELEASE_CNF))
          {
            if (rr_x2g_cco_in_progress(gas_id) || rr_ce_pcco_in_progress(gas_id))
            {
              if (rr_conn_est_info_ptr->x2g_cco_status == RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY)
              {
                /* Clear the flag now as we are finished with it and it must not
                remain set. */
                rr_conn_est_info_ptr->pcco_in_progress = FALSE;

                if (rr_ce_for_gprs_access(gas_id))
                {
                  rr_ps_send_imsg_no_ps_access_req(
                    FALSE,      // suspend_gmm
                    DEL_ONE,    // delete_pdu
                    gas_id      // gas_id
                  );
                }
                else
                {
                  /* RR_ABORT_IND is only sent for G2G PCCO, for X2G CCO it is
                  not required by NAS. */
                  if (!rr_x2g_cco_in_progress(gas_id))
                  {
                    rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
                  }
                  rr_ps_send_imsg_no_ps_access_req(
                    FALSE,      // suspend_gmm
                    NO_DEL,     // delete_pdu
                    gas_id      // gas_id
                  );
                }

                /* Make the current cell forbidden as it should not be selected
                unless there are no other suitable cells. */
                rr_add_forbidden_cell(rr_gprs_get_scell_arfcn(gas_id), gas_id);
                rr_start_forbidden_timer(gas_id);

                rr_resel_set_pccf_cause_to_no_response_on_target(gas_id);
                rr_conn_est_info_ptr->con_est_state = RR_CP_PCCO_FAILED;
              }
              else
              {
                /* if message indicates success (L1 was able to camp on serving cell) */
                if (message_ptr->mph_channel_release_cnf.error_code == NO_ERROR)
                {
                  /* complete the processing indicating RR can return to idle */
                  new_con_est_event = rr_ce_complete_mm_abort_processing(TRUE, gas_id);
                }
                else /* RR was not able to camp on serving cell */
                {
                  /* complete the processing indicating RR can't return to idle */
                  new_con_est_event = rr_ce_complete_mm_abort_processing(FALSE, gas_id);
                }
              }
            }
            else
            {
              /* if message indicates success (L1 was able to camp on serving cell) */
              if (message_ptr->mph_channel_release_cnf.error_code == NO_ERROR)
              {
                /* complete the processing indicating RR can return to idle */
                new_con_est_event = rr_ce_complete_mm_abort_processing(TRUE, gas_id);
              }
              else /* RR was not able to camp on serving cell */
              {
                /* complete the processing indicating RR can't return to idle */
                new_con_est_event = rr_ce_complete_mm_abort_processing(FALSE, gas_id);
              }
            }
          } /* MPH_CHANNEL_RELEASE_CNF */

          else if ((message_set == MS_RR_L1) && (message_id == (byte) MPH_IMMED_ASSIGN_CNF))
          {
            /* initiate the release of the physical channel without repeating IA procedure */
            rr_ce_perform_ia_connection_release(FALSE, gas_id);

            /* wait in this state for response from L1 */
          } /* MPH_IMMED_ASSIGN_CNF */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_UNIT_DATA_IND))
          {
            /********************************************************************/
            /* OTA messages are being ignored in this state since page messages */
            /* should not be answered                                           */
            /********************************************************************/
          } /* DL_UNIT_DATA_IND */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RANDOM_ACCESS_CNF))
          {
            dl_random_access_cnf_T *cnf_ptr = &(message_ptr->dl_random_access_cnf);

            if (GERAN_ACCESS_GRANTED != cnf_ptr->result)
            {
              /* the RACH burst was NOT transmitted over the air */
              /* but Layer 1 will be in its decoupled-idle state */
              /* so send MPH_ABORT_RA_REQ to complete this abort */
              rr_send_mph_abort_ra_req(gas_id);
            }

            /* abort can be completed now, RR can return to IDLE */
            MSG_GERAN_HIGH_1_G("DL_RANDOM_ACCESS_CNF with result %d, aborting", (int)(message_ptr->dl_random_access_cnf.result));
            new_con_est_event = rr_ce_complete_mm_abort_processing(TRUE, gas_id);
          } /* DL_RANDOM_ACCESS_CNF */

          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RELEASE_IND))
          {
            /* L2 has completed local end release of data link layer */
            /* initiate the release of the physical channel without repeating IA procedure */
            rr_ce_perform_ia_connection_release(FALSE, gas_id);

            /* wait in this state for response from L1 */
          } /* DL_RELEASE_IND */

          else
          {
            RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
          }
        } /* EV_INPUT_MESSAGE */

        /**********************************************************************/
        /* EV_GO_TO_IDLE                                                      */
        /*   L1 has returned to Idle, return to GRR                           */
        /* EV_NO_BCCH_FOUND                                                   */
        /*  RR is no longer camped                                            */
        /* RR_EV_PROCESS_SAVED_MESSAGE                                        */
        /*  The conn est was aborted to process a MM message, which was saved */
        /**********************************************************************/
        else if ( ( con_est_event == EV_GO_TO_IDLE ) ||
                  ( con_est_event == EV_NO_BCCH_FOUND ) ||
                  ( con_est_event == RR_EV_PROCESS_SAVED_MESSAGE ) )
        {
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
          new_rr_event = con_est_event;
        }
        else if (con_est_event == EV_START_IDLE)
        {
          rr_l1_idle_send_imsg_start_idle_req(gas_id);
        }
        else if ( con_est_event == EV_REPEAT_ASSIGNMENT_PROCEDURE )
        {
          /* For this event to occur here indicates that Layer 1 has resumed
           * idle mode and informed RR with an IDLE_MODE_CNF. This REPEAT event is
           * then triggered as the pending "camped on cell action". However, since
           * requesting idle mode in L1, RR has been asked to abort the connection,
           * thereby dropping us into this IA_ABORTED state: the appropriate response
           * is to inform MM, abandon the repeat assignment, and go inactive.
           */
          MSG_GERAN_MED_2_G("RR abandons IA repeat due to MM abort (flag=%1d,count=%1d)",
                  (int)rr_conn_est_info_ptr->repeat_assignment_procedure,
                  (int)rr_conn_est_info_ptr->procedure_counter);

          /* send abort and service indications */
          rr_conn_inform_mm_of_channel_release(TRUE, gas_id);

          /* then reset the repeat assignment fields */
          rr_conn_est_info_ptr->procedure_counter = 0;
          rr_conn_est_info_ptr->repeat_assignment_procedure = FALSE;

          /* go to inactive state and inform main RR state machine */
          rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
          new_rr_event = EV_GO_TO_IDLE;
        }
        else if ( con_est_event == RR_EV_MM_REQ_RECEIVED_ABORT )
        {
          /* output some debug here, as this event means that NAS is */
          /* still waiting for GRR to abort this connection attempt  */
          MSG_GERAN_ERROR_0_G("NAS abort request, GRR still waiting for release");
        }
        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
        break;
      } /* RR_CP_IA_ABORTED */

      case RR_CP_PCCO_FAILED:
      {
        // Note: NAS messages are blocked in this state (see rr_ce_can_process_mm_message())

        ASSERT(rr_conn_est_info_ptr->x2g_cco_status != RR_CCO_STATUS_UNDEFINED);

        if (con_est_event == EV_INPUT_MESSAGE)
        {
          switch (message_set)
          {
            case MS_RR_RR:
            {
              switch (message_ptr->rr.header.rr_message_set)
              {
                case RR_PS_OMSG:
                {
                  switch (message_ptr->rr.header.imh.message_id)
                  {
                    case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                    {
                      (void)rr_timer_cancel(T3174, gas_id);
 
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
#ifdef FEATURE_SGLTE
                      if (rr_x2g_cco_in_progress(gas_id) && !rr_is_sglte_mode_active(gas_id))
#else
                      if (rr_x2g_cco_in_progress(gas_id))
#endif /* FEATURE_SGLTE */
                      {
                        rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
                        rr_ps_no_ps_access_del_all(gas_id);
                        rr_ce_send_omsg_x2g_cco_status_ind(rr_conn_est_info_ptr->x2g_cco_status, gas_id);
                      }
                      else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
                      {
                        // Send request to put L1 in idle. This will return RR_L1_IDLE_OMSG_START_IDLE_CNF
                        rr_l1_idle_send_imsg_start_idle_req(gas_id);
                      }
                      break;
                    }

                    default:
                    {
                      RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                    }
                  }
                  break;
                } // case RR_PS_OMSG_NO_PS_ACCESS_CNF

                case RR_L1_IDLE_OMSG:
                {
                  switch (message_ptr->rr.header.imh.message_id)
                  {
                    case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                    {
                      MSG_GERAN_HIGH_0_G("L1 in Idle after T3146 expiry, indicate PCCO failure to RR-RESEL");

#ifdef FEATURE_SGLTE
                      if (rr_x2g_cco_in_progress(gas_id) && rr_is_sglte_mode_active(gas_id))
                      {
                        new_rr_event = EV_SGLTE_X2G_CCO_FAILED_GO_TO_IDLE;
                      }
                      else
#endif /* FEATURE_SGLTE */
                      {
                        new_rr_event = EV_PCCO_FAILED;
                      }
                      rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
                      break;
                    }

                    default:
                    {
                      RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                    }
                  }
                  break;
                } // RR_L1_IDLE_OMSG

                default:
                {
                  RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                }
              }
              break;
            } // case MS_RR_RR

            case MS_RR_L2:
            {
              switch (message_id)
              {
                case DL_RANDOM_ACCESS_CNF:
                {
                  /* DL_RANDOM_ACCESS_CNF may be received if IA Reject was
                  received before the last RACH was sent. */
                  break;
                } /* DL_RANDOM_ACCESS_CNF */

                default:
                {
                  RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                }
              }
              break;
            } /* MR_RR_L2 */

            default:
            {
              RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
            }
          }
        }
        else
        {
          RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
        }
        break;
      } // RR_CP_PCCO_FAILED

      case RR_CP_T3146_EXPIRED:
      {
        // Note: NAS messages are blocked in this state (see rr_ce_can_process_mm_message())

        switch (con_est_event)
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_FATAL(message_ptr);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_PS_OMSG:
                  {
                    switch (message_ptr->rr.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        /**
                         * Force a cell reselection since the IA procedure failed. 
                         * If the surround measurements are too old, perform a power-scan first. 
                         */
                        if (rr_timer_has_expired(GRR_SURROUND_MEAS_TIMER, gas_id))
                        {
                          /* reset the timer to a "stopped" state */
                          (void)rr_timer_cancel(GRR_SURROUND_MEAS_TIMER, gas_id);

                          MSG_GERAN_MED_1_G(
                            "Surround meas older than %dms: perform RPS",
                            GRR_SURROUND_MEAS_TIMEOUT
                          );

                          // Event to be returned to GRR-CONTROL
                          new_rr_event = RR_EV_START_RESELECTION_POWER_SCAN;
                        }
                        else
                        {
                          // Record RR event to be returned when PS access blocked
                          new_rr_event = EV_FORCE_CELL_RESELECTION;
                        }

                        rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
                        break;
                      } // case RR_PS_OMSG_NO_PS_ACCESS_CNF

                      default:
                      {
                        RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                      }
                      break;
                    } // switch message_id
                    break;
                  } // case RR_PS_OMSG

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                } // switch rr_message_set
                break;
              } // case MS_RR_RR

              case MS_RR_L2:
              {
                switch (message_id)
                {
                  case DL_UNIT_DATA_IND:
                  {
                    /**
                     * The DL_UNIT_DATA_IND could contain IA or IA Reject. 
                     * L1 is still reading AGCH at this time, so reception of a IA or RA Reject is possible.
                     * If the IA is for UL, then it is too late. 
                     * If the IA is for DL, then this could be passed to MAC after RR/L1 return to Packet Idle 
                     * and PS access is re-enabled. However, in the current implementaion this is problematic, 
                     * so it is safer to ignore it until it can be properly supported. 
                     */
                    MSG_GERAN_HIGH_0_G("L2 message ignored - returning to Packet Idle");
                    break;
                  } // case DL_UNIT_DATA_IND

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                } // switch message_id
                break;
              } // case MS_RR_L2

              default:
              {
                RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
              }
            } // switch message_set
            break;
          } // case EV_INPUT_MESSAGE

          default:
          {
            RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
          }
        } // switch con_est_event
        break;
      } // case RR_CP_T3146_EXPIRED

      case RR_CP_T3146_EXPIRED_AFTER_REJECT:
      {
        switch (con_est_event)
        {
          case EV_INPUT_MESSAGE:
          {
            RR_NULL_CHECK_FATAL(message_ptr);

            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (message_ptr->rr.header.rr_message_set)
                {
                  case RR_PS_OMSG:
                  {
                    switch (message_ptr->rr.header.imh.message_id)
                    {
                      case RR_PS_OMSG_NO_PS_ACCESS_CNF:
                      {
                        if (rr_timer_is_running(T3142, gas_id))
                        {
                          MSG_GERAN_HIGH_0_G("T3146 expiry after IAR - T3142 running");
                        }
                        else
                        {
                          MSG_GERAN_HIGH_0_G("T3146 expiry after IAR - no T3142");
                        }

                        /**
                         * If this access attempt was for a GMM procedure, GMM will have instructed L1 to 
                         * enter MM non-DRX mode. Normally, it would instruct L1 to exit this mode upon 
                         * completion of the GMM procedure (e.g. Attach or RAU). In this case, GMM has no 
                         * knowledge that the access attempt was rejected, so L1 is taken out of MM non-DRX 
                         * here, and will re-enter when GMM retries the procedure. 
                         */
                        rr_gprs_clear_l1_in_mm_non_drx(gas_id);

                        MSG_GERAN_HIGH_0_G("RACH abort; Start GPRS Idle in L1");

                        // Send request to put L1 in idle. This will return RR_L1_IDLE_OMSG_START_IDLE_CNF
                        rr_l1_idle_send_imsg_start_idle_req(gas_id);
                        break;
                      }

                      default:
                      {
                        RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                      }
                    }
                    break;
                  } // case RR_PS_OMSG

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (message_ptr->rr.header.imh.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_IDLE_CNF:
                      {
                        rr_conn_est_info_ptr->con_est_state = RR_CP_INACTIVE;
                        new_rr_event = EV_TBF_CONN_EST_FAILURE;
                        break;
                      } // case RR_L1_IDLE_OMSG_START_IDLE_CNF

                      default:
                      {
                        RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                      }
                    } // switch message_id
                    break;
                  } // case RR_L1_IDLE_OMSG

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                } // switch rr_message_set
                break;
              } // case MS_RR_RR

              case MS_RR_L2:
              {
                switch (message_id)
                {
                  case DL_UNIT_DATA_IND:
                  {
                    /**
                     * The DL_UNIT_DATA_IND could contain IA or IA Reject. 
                     * L1 is still reading AGCH at this time, so reception of a IA or RA Reject is possible.
                     * If the IA is for UL, then it is too late. 
                     * If the IA is for DL, then this could be passed to MAC after RR/L1 return to Packet Idle 
                     * and PS access is re-enabled. However, in the current implementaion this is problematic, 
                     * so it is safer to ignore it until it can be properly supported. 
                     */
                    MSG_GERAN_HIGH_0_G("L2 message ignored - returning to Packet Idle");
                    break;
                  } // case DL_UNIT_DATA_IND

                  default:
                  {
                    RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
                  }
                } // switch message_id
                break;
              } // case MS_RR_L2

              default:
              {
                RR_CE_LOG_UNEXPECTED_MESSAGE(message_ptr);
              }
            } // switch message_set
            break;
          } // case EV_INPUT_MESSAGE

          default:
          {
            RR_CE_LOG_UNEXPECTED_EVENT(con_est_event);
          }
        } // switch con_est_event
        break;
      } // case RR_CP_T3146_EXPIRED_AFTER_REJECT

      default:
        MSG_GERAN_ERROR_1_G("Bad state(%d) in ConnectionEstablishment",(int)(rr_conn_est_info_ptr->con_est_state));
        break;
    } /* switch on state */

#ifdef DEBUG_RR_TRACE_MSG
    if (((con_est_event != new_con_est_event) && (new_con_est_event != EV_NO_EVENT)) ||
        (rr_conn_est_info_ptr->old_con_est_state != rr_conn_est_info_ptr->con_est_state))
    {
       rr_store_trace_msg_buf(
         RR_CONN_ESTABLISH_SM,
         new_con_est_event,
         (byte)(rr_conn_est_info_ptr->con_est_state),
         message_ptr,
         gas_id
       );
    }
#endif

    if (new_con_est_event != EV_NO_EVENT)
    {
      con_est_event = new_con_est_event;
      new_con_est_event = EV_NO_EVENT;
    }
    else
    {
      con_est_event = EV_NO_EVENT;
    }
  } /* while there are more events to process */

  if (rr_conn_est_info_ptr->con_est_state != rr_conn_est_info_ptr->old_con_est_state)
  {
    if (rr_conn_est_info_ptr->con_est_state == RR_CP_INACTIVE)
    {
      /* State has changed back to INACTIVE, reset PCCO flag as it is no longer
      required. */
      rr_conn_est_info_ptr->pcco_in_progress = FALSE;
    }

    rr_show_con_est_state_change(rr_conn_est_info_ptr->con_est_state,
                                 rr_conn_est_info_ptr->old_con_est_state);
    rr_conn_est_info_ptr->old_con_est_state = rr_conn_est_info_ptr->con_est_state;
  }

  return new_rr_event;
} /* rr_connection_establishment_control */

/**
 * Notification that an Immediate Assignment Reject was received
 *
 * @param t3122_timeout : Wait Indication specified in the IA Reject
 * @param access_reason : specific CS establishment cause (e.g. response to paging)
 *
 * @return VOID
 */
void rr_start_t3122(
  byte t3122_timeout_secs, byte access_reason,
  const gas_id_t gas_id
)
{
  if (t3122_timeout_secs != 0)
  {
    int t3122_timeout_ms = (int) t3122_timeout_secs * 1000;
    rr_conn_est_info_T *rr_conn_est_info_ptr;
    gprs_scell_info_t *scell_info_ptr;

    // Obtain a pointer to the module data
    rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
    RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

    scell_info_ptr = rr_gprs_get_scell_info(gas_id);
    RR_NULL_CHECK_FATAL(scell_info_ptr);

    MSG_GERAN_HIGH_1_G("Starting T3122(%d)",(int)t3122_timeout_secs);
    (void)rr_timer_start(T3122,(dword)t3122_timeout_ms,gas_id);

    /* store the ARFCN of the serving cell for which this reject was received */
    rr_conn_est_info_ptr->t3122_info.t3122_serving_cell = scell_info_ptr->gsm.BCCH_ARFCN;  // use access func here

    /* store the BSIC of the serving cell for which this reject was received */
    rr_conn_est_info_ptr->t3122_info.t3122_serving_bsic = scell_info_ptr->gsm.BSIC;

    /* store the establishment cause that was in progress */
    rr_conn_est_info_ptr->t3122_info.t3122_rejected_cause = access_reason;
  }
  else
  {
    /* only start T3122 if the timeout value in IARej is not zero */
    MSG_GERAN_HIGH_0_G("T3122 timeout is 0, not starting T3122");
  }

} /* end rr_start_t3122() */


static boolean rr_check_t3122_running(const gas_id_t gas_id)
{
  boolean t3122_running = rr_timer_is_running(T3122, gas_id);

  if (t3122_running)
  {
    ARFCN_T cell_arfcn;
    BSIC_T cell_bsic;

    // Obtain a pointer to the module data
    rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
    RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

    /* preprocessing of RR_EST_REQ may result in this function being called */
    /* when RR is just about to transition to a camped state, so check the  */
    /* appropriate cell database to determine whether access is restricted. */
    if (rr_is_camped(gas_id))
    {
      gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

      cell_arfcn = scell_info_ptr->gsm.BCCH_ARFCN;
      cell_bsic  = scell_info_ptr->gsm.BSIC;
    }
    else
    {
      gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

      cell_arfcn = pcell_info_ptr->gsm.BCCH_ARFCN;
      cell_bsic  = pcell_info_ptr->gsm.BSIC;
    }

    // Check if still on the same cell where T3122 was started
    if ((ARFCNS_EQUAL(rr_conn_est_info_ptr->t3122_info.t3122_serving_cell, cell_arfcn) &&
         rr_same_BSIC(&(rr_conn_est_info_ptr->t3122_info.t3122_serving_bsic),&cell_bsic)))
    {
      MSG_GERAN_MED_0_G("T3122 running");
    }
    else
    {
      MSG_GERAN_MED_0_G("T3122 stopped as cell changed");

      rr_stop_t3122(gas_id);
      t3122_running = FALSE;
    }
  }
  else
  {
    MSG_GERAN_LOW_0_G("T3122 not running");
  }

  return t3122_running;

} /* end rr_check_t3122_running() */


boolean rr_get_t3122_running_for_nas(const gas_id_t gas_id)
{
  boolean t3122_running = rr_check_t3122_running(gas_id);

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->t3122_info.nas_t3122_running = t3122_running;
  rr_conn_est_info_ptr->t3122_info.nas_update_required = FALSE;

  return t3122_running;

} /* end rr_get_t3122_running_for_nas() */


void rr_check_nas_t3122_status(const gas_id_t gas_id)
{
  boolean t3122_running = rr_check_t3122_running(gas_id);

  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  if (t3122_running != rr_conn_est_info_ptr->t3122_info.nas_t3122_running)
  {
    grr_update_mm_with_rr_service_ind(gas_id);
  }

} /* end rr_check_nas_t3122_status() */


/**
 * Determines whether a CS connection is permitted
 *
 * @param new_est_cause : reason for the CS connection (MO/EMERGENCY/LAU etc.)
 *
 * @return TRUE if CS connection establishment can proceed
 */
boolean rr_ce_allow_cs_connection(
  byte new_est_cause,
  const gas_id_t gas_id
)
{
  boolean permitted = TRUE;

  /* Check if T3122 is running (also checks if the cell has changed) */
  if (rr_check_t3122_running(gas_id))
  {
    // Obtain a pointer to the module data
    rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
    RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

    /* T3122 was started on this cell and is still running */
    if (rr_conn_est_info_ptr->t3122_info.t3122_rejected_cause == (byte)MM_RR_EMERGENCY_CALL)
    {
      /* an emergency call was rejected, which means no other calls are allowed */
      permitted = FALSE;
    }
    else if (new_est_cause != (byte)MM_RR_EMERGENCY_CALL)
    {
      /* a non-emergency call was rejected, and a non-emergency call is requested */
      permitted = FALSE;
    }
    else
    {
      /* a non-emergency call was rejected, and an emergency call is requested */
      MSG_GERAN_MED_0_G("Emergency call allowed when T3122");
    }
  }

  return(permitted);
}

/*!
 * \brief Called when a connection establishment attempt is started
 * 
 * \param requested_by_nas (in) - TRUE if NAS requested this connection, FALSE if not (e.g. E-DTM IA)
 */
void rr_ce_init_conn_info(
  boolean requested_by_nas,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->requested_by_nas = requested_by_nas;
  rr_conn_est_info_ptr->confirmed_to_nas = FALSE;
  rr_conn_est_info_ptr->originated_on_other_rat = FALSE;

  rr_conn_set_release_cause(
    RELEASE_CAUSE_NONE,   // release_cause
    gas_id                // gas_id
  );
}

/*!
 * \brief Called when a connection establishment attempt is started
 * 
 * \param requested_by_nas (in) - TRUE if NAS requested this connection, FALSE if not (e.g. E-DTM IA)
 */
void rr_ce_init_conn_info_after_x2g_ho(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  rr_conn_est_info_ptr->requested_by_nas = TRUE;
  rr_conn_est_info_ptr->confirmed_to_nas = TRUE;
  rr_conn_est_info_ptr->originated_on_other_rat = TRUE;

  rr_conn_set_release_cause(
    RELEASE_CAUSE_NONE,   // release_cause
    gas_id                // gas_id
  );
}

/*!
 * \brief Indicates if the connection was requested by NAS
 * 
 * \return boolean - TRUE if the connection was requested by NAS
 */
boolean rr_ce_conn_is_requested_by_nas(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  return rr_conn_est_info_ptr->requested_by_nas;
}

/*!
 * \brief Indicates if NAS should be informed if the connection is released/aborted
 * 
 * \return boolean - TRUE if the NAS should be informed (i.e. RR_REL_IND/RR_ABORT_IND)
 */
boolean rr_ce_should_confirm_conn_rel_to_nas(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  // If NAS did not request the connection and is not aware of it, then it should not be updated if the
  // connection fails / is aborted
  if ((rr_conn_est_info_ptr->requested_by_nas == FALSE) &&
      (rr_conn_est_info_ptr->confirmed_to_nas == FALSE))
  {
    return FALSE;
  }

  return TRUE;
}

/*!
 * \brief Indicates if the connection was originated on another RAT (i.e. X2G HO occurred).
 * 
 * \return boolean - TRUE if the connection was originated on another RAT
 */
boolean rr_ce_conn_originated_on_other_rat(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);

  return rr_conn_est_info_ptr->originated_on_other_rat;
}

/**
 *
 * @return Init the access reason
 */
void rr_ce_init_access_reason(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_conn_est_info_T *rr_conn_est_info_ptr = rr_conn_est_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_conn_est_info_ptr);
  rr_conn_est_info_ptr->access_reason = MM_RR_NO_CNM_SERVICE;
} /* rr_ce_init_access_reason */

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/* EOF */
