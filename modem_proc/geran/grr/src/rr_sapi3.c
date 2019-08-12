/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR SAPI3 State Machine

GENERAL DESCRIPTION
  This module contains the SAPI3 state machine handler

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_sapi3.c_v   1.4   14 Jun 2002 17:22:10   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sapi3.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/08   sjw     Reworked release code for SAPI3, added new MDL_ERROR_IND handler
05/07/04   gc      Enhanced Trace Buffer Code
02/19/03   Rao     Fix for Releasing SAPIO link when SAPI3 Establishment fails.
06/14/02   rmc     Backing out changes related to l3_message_length field in RR_DATA_REQ
                   fixed lint warnings
05/20/02   rmc     Dual mode changes (interface for rr_data_req changes)
02/06/02   rmc     QXDM Cleanup
01/30/02   RMC     Added support to log channel configuration when it changes
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "rr_seg_load.h"
#include "comdef.h"
#include "rr_sapi3.h"
#include "gprs_mem.h"
#include "rr_l2.h"
#include "ms.h"
#include "rr_l2_send.h"
#include "msg.h"
#include "err.h"
#include "rr_log.h"
#include "rr_general.h"
#include "rr_gprs_debug.h"
#include "rr_conn_release.h"
#include "rr_conn_establish.h"
#include "amssassert.h"
#include "rr_ded_config.h"
#include "rr_multi_sim.h"

typedef enum
{
  RR_SAPI3_MOBILE_ORIGINATED,
  RR_SAPI3_MOBILE_TERMINATED
} rr_sapi3_est_cause_t;

#define RR_SAPI3_FIRST_MESSAGE_NUMBER 1


#define MAX_RR_SAPI3_REQUESTS 4

typedef struct
{
  byte                l3_message_number;
  rr_data_req_cb_fn_T callback_fn;
} rr_sapi3_callback_entry_T;

typedef struct
{
  byte *msg;
  byte  msg_length;
} rr_sapi3_pending_l3_msg_T;

typedef struct
{
  rr_sapi3_est_cause_t      SAPI3_est_cause;
  SAPI3_link_state_T        SAPI3_link_state;
  byte                      rr_sapi3_msg_number;
  SAPI3_link_state_T        old_SAPI3_link_state;
  rr_sapi3_callback_entry_T rr_sapi3_callback_list[MAX_RR_SAPI3_REQUESTS];
  rr_sapi3_pending_l3_msg_T pending_l3_messages[MAX_RR_SAPI3_REQUESTS];
  byte                      pending_l3_message_count;
} rr_sapi3_data_t;

#define _RR_SAPI3_DATA_INIT { \
                             RR_SAPI3_MOBILE_ORIGINATED, \
                             NO_SAPI3_LINK, \
                             RR_SAPI3_FIRST_MESSAGE_NUMBER, \
                             NO_SAPI3_LINK \
                           }

static rr_sapi3_data_t rr_sapi3_data[NUM_GERAN_DATA_SPACES] =
                                       { INITIAL_VALUE(_RR_SAPI3_DATA_INIT) };
#undef _RR_SAPI3_DATA_INIT

/*===========================================================================

FUNCTION  rr_sapi3_get_data_ptr
  Returns a pointer to the module data

===========================================================================*/
static rr_sapi3_data_t *rr_sapi3_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_sapi3_data[as_index];
}

/*===========================================================================

FUNCTION  rr_sapi3_get_free_list_entry
  Find the first unused entry in the callback list

===========================================================================*/
static rr_sapi3_callback_entry_T * rr_sapi3_get_free_list_entry(
  const gas_id_t gas_id
)
{
  rr_sapi3_data_t           *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);
  rr_sapi3_callback_entry_T *entry_ptr = rr_sapi3_data_ptr->rr_sapi3_callback_list;
  int                        index;

  for (index = 0; index < MAX_RR_SAPI3_REQUESTS; index++, entry_ptr++)
  {
    if (entry_ptr->callback_fn == NULL)
    {
      /* found a free space */
      return(entry_ptr);
    }
  }

  return(NULL);
}

/*===========================================================================

FUNCTION  rr_sapi3_process_outstanding_callbacks
  Scan the current callback list, and execute all that are outstanding
  Set transmit_ok to FALSE for each call, to indicate acknowledgement failed

===========================================================================*/
static void rr_sapi3_process_outstanding_callbacks(
  const gas_id_t gas_id
)
{
  rr_sapi3_data_t           *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);
  rr_sapi3_callback_entry_T *entry_ptr = rr_sapi3_data_ptr->rr_sapi3_callback_list;
  int                        index;

  for (index = 0; index < MAX_RR_SAPI3_REQUESTS; index++, entry_ptr++)
  {
    if (entry_ptr->callback_fn != NULL)
    {
      MSG_GERAN_HIGH_1_G("RR has no ack for SAPI3 msg number %d", (int)entry_ptr->l3_message_number);
      /* call the callback with the transmit_ok flag set to FALSE */
      entry_ptr->callback_fn(FALSE);
      entry_ptr->callback_fn = NULL;
    }
    entry_ptr->l3_message_number = 0;
  }

  return;
}

/*===========================================================================

FUNCTION  rr_sapi3_find_list_entry_for_message
  Scan the callback list for a specific message number, and return the entry

===========================================================================*/
static rr_sapi3_callback_entry_T *rr_sapi3_find_list_entry_for_message(
  byte l3_message_number,
  const gas_id_t gas_id
)
{
  rr_sapi3_data_t           *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);
  rr_sapi3_callback_entry_T *entry_ptr = rr_sapi3_data_ptr->rr_sapi3_callback_list;
  int                        index;

  for (index = 0; index < MAX_RR_SAPI3_REQUESTS; index++, entry_ptr++)
  {
    if (entry_ptr->l3_message_number == l3_message_number)
    {
      return(entry_ptr);
    }
  }

  return(NULL);
}


/*===========================================================================

FUNCTION  rr_sapi3_link_established
  Updates local and global stores to reflect establishment of the SAPI3 link

===========================================================================*/
static void rr_sapi3_link_established(
  rr_sapi3_est_cause_t est_cause,
  const gas_id_t gas_id
)
{
  rr_sapi3_data_t *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);

  /* update the global channel config store */
  rr_set_link_status(SAPI0_AND_SAPI3_LINK, gas_id);

  /* as well as this module's state variable */
  rr_sapi3_data_ptr->SAPI3_link_state = SAPI3_LINK_ESTABLISHED;

  /* and the establishment cause */
  rr_sapi3_data_ptr->SAPI3_est_cause = est_cause;

  /* and log the change of configuration */
  (void)rr_log_channel_configuration(gas_id);

  if (rr_sapi3_data_ptr->SAPI3_est_cause == RR_SAPI3_MOBILE_ORIGINATED)
  {
    MSG_GERAN_HIGH_0_G("SAPI3 link established by UE");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("SAPI3 link established by NW");
  }

  return;
}

/*===========================================================================

FUNCTION  rr_sapi3_release_link
  Updates local and global stores to reflect a release of the SAPI3 link

===========================================================================*/
static SAPI3_link_state_T rr_sapi3_release_link(
  boolean bChangeLinkStatus,
  const gas_id_t gas_id
)
{
  rr_sapi3_data_t *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);

  rr_sapi3_process_outstanding_callbacks(gas_id);

  rr_sapi3_data_ptr->SAPI3_est_cause = RR_SAPI3_MOBILE_ORIGINATED;

  if (bChangeLinkStatus)
  {
    /* SAPI0 link must exist, so drop back to it */
    rr_set_link_status(SAPI0_LINK, gas_id);

    /* log the changes to the channel configuration */
    (void)rr_log_channel_configuration(gas_id);
  }

  MSG_GERAN_HIGH_0_G("SAPI3 link released");

  return(NO_SAPI3_LINK);
}

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/*===========================================================================

FUNCTION  rr_sapi3_process_rr_data_req
  Process a RR_DATA_REQ primitive from NAS: validate the length fields

===========================================================================*/
static byte rr_sapi3_process_rr_data_req(
  rr_data_req_T *rr_data_req_ptr,
  const gas_id_t gas_id
)
{
  byte l3_message_length;
  rr_sapi3_data_t *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);

  /* extract the message length */
  l3_message_length = rr_data_req_ptr->l3_message_length;

  MSG_GERAN_HIGH_1_G("Processing RR_DATA_REQ for SAPI3 (%d)",(int)l3_message_length);

#ifdef PERLUTF
    #error code not present
#endif /* PERLUTF */

  if (rr_data_req_ptr->callback_fn != NULL)
  {
    rr_sapi3_callback_entry_T *entry_ptr = rr_sapi3_get_free_list_entry(gas_id);

    if (entry_ptr != NULL)
    {
      /* store the callback provided by MM, with the current message number */
      entry_ptr->callback_fn = rr_data_req_ptr->callback_fn;
      entry_ptr->l3_message_number = rr_sapi3_data_ptr->rr_sapi3_msg_number;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("No free list entry to store callback info");
    }
  }
  else
  {
    MSG_GERAN_MED_0_G("No callback supplied for this request");
  }

  /* sanity check the length to avoid exceeding array bounds */
  if (l3_message_length > MAX_OCTETS_L3_MESSAGE)
  {
    MSG_GERAN_ERROR_0_G("RR_DATA_REQ too long, truncating");
    l3_message_length = MAX_OCTETS_L3_MESSAGE;
  }

  return(l3_message_length);
}

/*===========================================================================

FUNCTION  rr_sapi3_add_pending_l3_message
  Adds one layer 3 message to pending messages list.

===========================================================================*/
static void rr_sapi3_add_pending_l3_message(
  rr_data_req_T *rr_data_req,
  const gas_id_t gas_id
)
{
  rr_sapi3_data_t           *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);
  rr_sapi3_pending_l3_msg_T *pend_msg = NULL;
  int                        index = 0;

  RR_NULL_CHECK_RETURN_VOID(rr_data_req);
  RR_NULL_CHECK_RETURN_VOID(rr_sapi3_data_ptr);

  if (rr_sapi3_data_ptr->pending_l3_message_count < MAX_RR_SAPI3_REQUESTS)
  {
    pend_msg = &rr_sapi3_data_ptr->pending_l3_messages[rr_sapi3_data_ptr->pending_l3_message_count];

    /* process the message header and return a validated L3 length */
    pend_msg->msg_length = rr_sapi3_process_rr_data_req(
      rr_data_req,    // rr_data_req_ptr
      gas_id          // gas_id
    );
  
    /* copy the L3 message into a pending buffer until the link is established */
    pend_msg->msg = (byte *)GPRS_MEM_CALLOC(pend_msg->msg_length);
    
    if (pend_msg->msg != NULL)
    {
      for (index = 0; index < pend_msg->msg_length; index++)
      {
        pend_msg->msg[index] = rr_data_req->layer3_message[index];
      }
      
      rr_sapi3_data_ptr->pending_l3_message_count++;
      MSG_GERAN_HIGH_1_G("L3 message stored in pending list (count=%d)", rr_sapi3_data_ptr->pending_l3_message_count);
    }
    else
    {
      MSG_GERAN_ERROR_0_G("L3 message storing failed");
    }
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Overflow detected!!! Not possible to store L3 message");
  }
}

/*===========================================================================

FUNCTION  rr_sapi3_remove_pending_l3_messages
  Frees all pending layer 3 messages.

===========================================================================*/
static void rr_sapi3_remove_pending_l3_messages(
  const gas_id_t gas_id
)
{
  rr_sapi3_data_t *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);
  int              index = 0;

  RR_NULL_CHECK_RETURN_VOID(rr_sapi3_data_ptr);

  MSG_GERAN_HIGH_1_G("Removing pending L3 messages (count=%d)", rr_sapi3_data_ptr->pending_l3_message_count);
  for (index = 0; index < rr_sapi3_data_ptr->pending_l3_message_count; index++)
  {
    rr_sapi3_data_ptr->pending_l3_messages[index].msg_length = 0;
    
    if(rr_sapi3_data_ptr->pending_l3_messages[index].msg != NULL)
    {
      GPRS_MEM_FREE(rr_sapi3_data_ptr->pending_l3_messages[index].msg);
      rr_sapi3_data_ptr->pending_l3_messages[index].msg = NULL;
    }
    else
    {
      MSG_GERAN_ERROR_0_G("Mem Corruption detected!!! Trying to FREE NULL pointer.");
    }
  }

  rr_sapi3_data_ptr->pending_l3_message_count = 0;
}

/*===========================================================================

FUNCTION  rr_sapi3_process_pending_l3_messages
  Sends all pending layer 3 messages to L2.

===========================================================================*/
static void rr_sapi3_process_pending_l3_messages(
  l2_channel_type_T l2_channel_type,
  const gas_id_t    gas_id
)
{
  rr_sapi3_data_t *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);
  int              index = 0;
  byte             l3_msg_num = 0;

  RR_NULL_CHECK_RETURN_VOID(rr_sapi3_data_ptr);

  MSG_GERAN_HIGH_1_G("Processing pending L3 messages (count=%d)", rr_sapi3_data_ptr->pending_l3_message_count);
  for (index = 0; index < rr_sapi3_data_ptr->pending_l3_message_count; index++)
  {
    l3_msg_num = rr_send_dl_data_req(
      rr_sapi3_data_ptr->pending_l3_messages[index].msg,         // layer3_message
      rr_sapi3_data_ptr->pending_l3_messages[index].msg_length,  // length
      SAPI3,                                                     // SAPI
      l2_channel_type,                                           // l2_channel_type
      gas_id                                                     // gas_id
    );
    
    if ((index == 0) && (l3_msg_num != RR_SAPI3_FIRST_MESSAGE_NUMBER))
    {
      /* for MO establishment, always expect the first message number here */
      MSG_GERAN_ERROR_1_G("RR SAPI3 message number (%d) mismatch in L2", l3_msg_num);
    }
  }
  
  rr_sapi3_remove_pending_l3_messages(gas_id);
}

/*===========================================================================

FUNCTION  rr_sapi3_initialise_message_tracking
  Sets up this module's local stores used to track Layer 2 data requests

===========================================================================*/
static void rr_sapi3_initialise_message_tracking(
  const gas_id_t gas_id
)
{
  rr_sapi3_data_t           *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);

  rr_sapi3_callback_entry_T *entry_ptr = rr_sapi3_data_ptr->rr_sapi3_callback_list;
  int                        index;

  for (index = 0; index < MAX_RR_SAPI3_REQUESTS; index++, entry_ptr++)
  {
    entry_ptr->l3_message_number = 0;
    entry_ptr->callback_fn       = NULL;
  }

  rr_sapi3_data_ptr->rr_sapi3_msg_number = RR_SAPI3_FIRST_MESSAGE_NUMBER;

  rr_sapi3_remove_pending_l3_messages(gas_id);

  return;
}

/*===========================================================================

FUNCTION  rr_sapi3_state_name
  Returns a text string to describe the specified SAPI3 state

===========================================================================*/
static char * rr_sapi3_state_name(SAPI3_link_state_T state)
{
  char *s = "";

  switch (state)
  {
    case NO_SAPI3_LINK:
      s = "NoLink";
      break;

    case SAPI3_LINK_PENDING:
      s = "LinkPending";
      break;

    case SAPI3_LINK_ESTABLISHED:
      s = "LinkEst";
      break;

    default:
      s = "Unknown";
      break;
  }

  return s;
}

/*===========================================================================

FUNCTION  rr_sapi3_handle_mdl_error
  Responds to the MDL_ERROR_IND received from Layer 2

===========================================================================*/
static rr_event_T rr_sapi3_handle_mdl_error(
  SAPI_T sapi,
  error_code_T error_code,
  l2_channel_type_T error_chan,
  const gas_id_t gas_id
)
{
  rr_event_T output_event;
  rr_sapi3_data_t *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);

  if (sapi == SAPI3)
  {
    byte access_reason;

    /* In general, an error on SAPI3 should not affect SAPI0 unless the */
    /* data link layer is being used specifically for an SMS connection */
    access_reason = rr_ce_access_reason(gas_id);

    /* the connection may be mobile-originated or mobile-terminated */
    if (access_reason == (byte)MM_RR_ANSWER_TO_PAGING)
    {
      channel_spec_T *chan_spec_ptr;
      channel_mode_T  current_mode;

      /* determine the current mode of the dedicated channel */
      chan_spec_ptr = rr_get_channel_spec(NULL, gas_id);
      if (chan_spec_ptr != NULL)
      {
        current_mode = chan_spec_ptr->channel_mode_1;
      }
      else
      {
        current_mode = SIGNALLING_ONLY;
      }

      if ((rr_sapi3_data_ptr->SAPI3_link_state == SAPI3_LINK_ESTABLISHED)     &&
          (rr_sapi3_data_ptr->SAPI3_est_cause  == RR_SAPI3_MOBILE_TERMINATED) &&
          (current_mode     == SIGNALLING_ONLY))
      {
        /* assume that the page was purely for the SMS connection */
        access_reason = (byte)MM_RR_SHORT_MESSAGE;
      }
    }
    else if (access_reason == (byte)MM_RR_SHORT_MESSAGE)
    {
      /* if SAPI3 is using the SACCH, then there is a concurrency with a traffic call */
      /* override local reason with MO speech such that SAPI0 is protected from a RLF */
      if (SACCH == error_chan)
      {
        access_reason = (byte)MM_RR_MO_FULL_SPEECH;
        MSG_GERAN_HIGH_0_G("SAPI3 failure on SACCH, assume DCCH being used for speech");
      }
    }

    if (access_reason == (byte)MM_RR_SHORT_MESSAGE)
    {
      /* any error on SAPI3 during a dedicated SMS connection triggers RLF */
      /* since there is no point in keeping the channel for other purposes */
      output_event = EV_RADIO_LINK_FAILURE;
    }
    else
    {
      /* existing CS connection may be in use by something other than SMS */
      if (error_code == T200_EXPIRY)
      {
        /* Layer 2 only sends this if an establishment has failed */
        /* and will send a DL_RELEASE_IND immediately afterwards  */
        MSG_GERAN_HIGH_0_G("SAPI3 establishment failure");
      }
      else
      {
        /* release SAPI3 locally */
        rr_release_sapi3(gas_id);
      }

      /* allow SAPI0 to persist, since the connection may be in use for speech */
      output_event = EV_NO_EVENT;
    }
  }
  else
  {
    /* this is not expected here, perform abnormal release of all SAPIs */
    MSG_GERAN_ERROR_1_G("MDL_ERROR_IND on SAPI%d passed to SAPI3 control", (int)sapi);

    rr_sapi3_data_ptr->SAPI3_link_state = rr_sapi3_release_link(
      TRUE,     // bChangeLinkStatus
      gas_id    // gas_id
    );

    output_event = EV_RADIO_LINK_FAILURE;
  }

  return(output_event);
}


/*===========================================================================

FUNCTION  rr_control_SAPI3_link
  Main state machine for control of the SAPI3 link by RR

===========================================================================*/
rr_event_T rr_control_SAPI3_link(
  void *new_message,
  const gas_id_t gas_id
)
{
  l2_channel_type_T         l2_channel_type;
  IMH_T                    *message_header;
  byte                      message_id;
  byte                      message_set;
  rr_sapi3_data_t          *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);

  message_header = (IMH_T *)new_message;
  message_id     = message_header->message_id;
  message_set    = message_header->message_set;

  l2_channel_type = rr_get_l2_channel_type(gas_id);

  /* Handle DL_RELEASE_IND, MDL_ERROR_IND and MPH_RESET_IND independent of state */
  if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RELEASE_IND))
  {
    rr_sapi3_data_ptr->SAPI3_link_state = rr_sapi3_release_link(
      TRUE,     // bChangeLinkStatus
      gas_id    // gas_id
    );
    rr_sapi3_remove_pending_l3_messages(gas_id);
  }
  else if ((message_set == MS_RR_L2) && (message_id == (byte) MDL_ERROR_IND))
  {
    l2_channel_type_T error_chan = ((mdl_error_ind_T *)new_message)->l2_channel_type;
    error_code_T      error_code = ((mdl_error_ind_T *)new_message)->error_code;
    SAPI_T            error_sapi = ((mdl_error_ind_T *)new_message)->SAPI;

    /**************************************************/
    /* MDL_ERROR_IND is an indication from L2 that an */
    /* error has occurred in the data link layer, so  */
    /* determine the appropriate response here.       */
    /**************************************************/
    rr_sapi3_remove_pending_l3_messages(gas_id);
    return(rr_sapi3_handle_mdl_error(error_sapi, error_code, error_chan, gas_id));
  }
  else if ((message_set == MS_RR_L1) && (message_id == (byte)MPH_RESET_IND))
  {
    /* reset everything and return immediately */
    rr_sapi3_data_ptr->SAPI3_link_state = rr_sapi3_release_link(
      FALSE,    // bChangeLinkStatus
      gas_id    // gas_id
    );
    rr_sapi3_data_ptr->old_SAPI3_link_state   = rr_sapi3_data_ptr->SAPI3_link_state;
    rr_sapi3_remove_pending_l3_messages(gas_id);

    return(EV_NO_EVENT);
  }

  switch (rr_sapi3_data_ptr->SAPI3_link_state)
  {
    case NO_SAPI3_LINK:
    {
      if ((message_set == MS_MM_RR) && (message_id == (byte) RR_DATA_REQ))
      {
        rr_data_req_T *rr_data_req = (rr_data_req_T *)new_message;

        /* initialise message tracking before processing the first request */
        rr_sapi3_initialise_message_tracking(gas_id);
        
        rr_sapi3_add_pending_l3_message(rr_data_req, gas_id);

        MSG_GERAN_HIGH_1_G("Establishing SAPI3 Link(MO,ch=%1d)", (int)l2_channel_type);
        rr_sapi3_data_ptr->SAPI3_link_state = SAPI3_LINK_PENDING;

        /* SAPI3 always uses normal establishment - no contention resolution */
        rr_send_dl_establish_req(
          SAPI3,                  // SAPI
          l2_channel_type,        // l2_channel_type
          NULL,                   // layer3_message
          0,                      // message_length
          NORMAL_ESTABLISHMENT,   // establish_mode
          gas_id                  // gas_id
        );
      }
      else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_ESTABLISH_IND))
      {
        dl_establish_ind_T *dl_establish_ind = (dl_establish_ind_T *)new_message;

        /* initialise message tracking before MT establishment */
        rr_sapi3_initialise_message_tracking(gas_id);

        MSG_GERAN_HIGH_1_G("Establishing SAPI3 Link(MT,ch=%1d)", (int)dl_establish_ind->l2_channel_type);
        rr_sapi3_link_established(
          RR_SAPI3_MOBILE_TERMINATED,   // est_cause
          gas_id                        // gas_id
        );
      }
      break;
    }

    case SAPI3_LINK_PENDING:
    {
      if ((message_set == MS_RR_L2) && (message_id == (byte) DL_ESTABLISH_CNF))
      {
        rr_sapi3_link_established(
          RR_SAPI3_MOBILE_ORIGINATED,   // est_cause
          gas_id                        // gas_id
        );

        /* send the L3 message that was queued up when establishment was requested */
        rr_sapi3_process_pending_l3_messages(l2_channel_type, gas_id);
      }
      
      else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_DATA_REQ))
      {
        rr_data_req_T *rr_data_req = (rr_data_req_T *)new_message;
        
        rr_sapi3_add_pending_l3_message(rr_data_req, gas_id);
      }

      else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RELEASE_CNF))
      {
        /* DL_RELEASE_REQ was sent whilst waiting for the link to establish */
        rr_sapi3_data_ptr->SAPI3_link_state = rr_sapi3_release_link(
          TRUE,     // bChangeLinkStatus
          gas_id    // gas_id
        );
        rr_sapi3_remove_pending_l3_messages(gas_id);
      }
      else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_ESTABLISH_IND))
      {
        dl_establish_ind_T *dl_establish_ind = (dl_establish_ind_T *)new_message;

        /* this is an (unexpected) establishment collision scenario */
        MSG_GERAN_ERROR_1_G("SAPI3 establishment collision(MT,ch=%1d)", (int)dl_establish_ind->l2_channel_type);

        /* set the link state to established, then send the pending message */
        rr_sapi3_link_established(
          RR_SAPI3_MOBILE_TERMINATED,   // est_cause
          gas_id                        // gas_id
        );

        /* send the L3 message that was queued up when establishment was requested */
        rr_sapi3_process_pending_l3_messages(l2_channel_type, gas_id);
      }
      break;
    }

    case SAPI3_LINK_ESTABLISHED:
    {
      if ((message_set == MS_RR_L2) && (message_id == (byte) DL_RELEASE_CNF))
      {
        rr_sapi3_data_ptr->SAPI3_link_state = rr_sapi3_release_link(
          TRUE,     // bUpdateLinkStatus
          gas_id    // gas_id
        );
        rr_sapi3_remove_pending_l3_messages(gas_id);
      }
      else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_DATA_REQ))
      {
        rr_data_req_T *rr_data_req = (rr_data_req_T *)new_message;
        byte           l3_message_length;

        l3_message_length = rr_sapi3_process_rr_data_req(rr_data_req, gas_id);

        (void)rr_send_dl_data_req(
          rr_data_req->layer3_message,    // layer3_message
          l3_message_length,              // length
          SAPI3,                          // SAPI
          l2_channel_type,                // l2_channel_type
          gas_id                          // gas_id
        );
      }
      else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_ESTABLISH_IND))
      {
        dl_establish_ind_T *dl_establish_ind = (dl_establish_ind_T *)new_message;

        /* this is a SAPI3 re-establishment caused by a downlink SABM */
        MSG_GERAN_HIGH_2_G("SAPI3 re-establishment (chan=%d): %d msgs discarded",
                 (int)dl_establish_ind->l2_channel_type,
                 (int)dl_establish_ind->num_discarded_msgs);

        rr_sapi3_process_outstanding_callbacks(gas_id);

        /* no need to change state, but SAPI3 queue has been cleared */
        rr_sapi3_initialise_message_tracking(gas_id);
      }
      break;
    }

    default:
      MSG_GERAN_ERROR_1_G("Unknown SAPI3 State: %d",(int)rr_sapi3_data_ptr->SAPI3_link_state);
      break;
  } /* switch (rr_sapi3_data_ptr->SAPI3_link_state) */

  if (rr_sapi3_data_ptr->SAPI3_link_state != rr_sapi3_data_ptr->old_SAPI3_link_state)
  {
#ifdef DEBUG_RR_TRACE_MSG
    rr_store_trace_msg_buf(
       RR_CONTROL_SAPI3_SM,
       EV_NO_EVENT,
       (byte)rr_sapi3_data_ptr->SAPI3_link_state,
       new_message,
       gas_id
     );
#endif /* DEBUG_RR_TRACE_MSG */

    RR_LOG_STATE_TRANSITION(
     "rr_sapi3_control",
     rr_sapi3_data_ptr->old_SAPI3_link_state,
     rr_sapi3_data_ptr->SAPI3_link_state,
     rr_sapi3_state_name,
     gas_id
   );

    rr_sapi3_data_ptr->old_SAPI3_link_state = rr_sapi3_data_ptr->SAPI3_link_state;
  }

  return(EV_NO_EVENT);
}

/*===========================================================================

FUNCTION  rr_sapi3_process_dl_data_ack_ind
  Handles the DL_DATA_ACK_IND signal from Layer 2

===========================================================================*/
void rr_sapi3_process_dl_data_ack_ind(
  byte l3_message_number,
  const gas_id_t gas_id
)
{
  rr_sapi3_callback_entry_T *entry_ptr;

  entry_ptr = rr_sapi3_find_list_entry_for_message(l3_message_number, gas_id);

  /* if a data request callback was supplied, call it now */
  if (entry_ptr != NULL)
  {
    MSG_GERAN_HIGH_2_G("RR SAPI3 callback 0x%08X for msg %d",
            (uint32)entry_ptr->callback_fn,
            (int)entry_ptr->l3_message_number);
    entry_ptr->l3_message_number = 0;
    if (entry_ptr->callback_fn != NULL)
    {
      /* call the callback with the transmit_ok flag set to TRUE */
      entry_ptr->callback_fn(TRUE);
      entry_ptr->callback_fn = NULL;
    }
  }
  else
  {
    MSG_GERAN_HIGH_1_G("No RR SAPI3 callback for msg number %d", (int)l3_message_number);
  }

  return;
}

/*===========================================================================

FUNCTION  rr_sapi3_get_next_message_number
  Returns the next message number in sequence
  Wraparound (0xFF to 0x00) is acceptable

===========================================================================*/
byte rr_sapi3_get_next_message_number(
  const gas_id_t gas_id
)
{
  byte result;
  rr_sapi3_data_t *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);

  result = rr_sapi3_data_ptr->rr_sapi3_msg_number;

  rr_sapi3_data_ptr->rr_sapi3_msg_number++;
  if (rr_sapi3_data_ptr->rr_sapi3_msg_number == 0)
  {
    rr_sapi3_data_ptr->rr_sapi3_msg_number = RR_SAPI3_FIRST_MESSAGE_NUMBER;
  }

  return(result);
}

/*===========================================================================

FUNCTION  rr_reset_sapi3
  Uses the MPH_RESET_IND signal to reset the RR SAPI3 state machine

===========================================================================*/
void rr_reset_sapi3(
  const gas_id_t gas_id
)
{
  IMH_T reset_msg = {0};

  /* pass a reset indication into RR's own SAPI3 state machine */
  reset_msg.message_set = (byte)MS_RR_L1;
  reset_msg.message_id  = (byte)MPH_RESET_IND;

  (void)rr_control_SAPI3_link(&reset_msg, gas_id);

  return;
}


/*===========================================================================

FUNCTION  rr_release_sapi3

  Releases any established (or pending) SAPI3 link

===========================================================================*/

void rr_release_sapi3(
  const gas_id_t gas_id
)
{
  rr_sapi3_data_t *rr_sapi3_data_ptr = rr_sapi3_get_data_ptr(gas_id);

  if ((rr_sapi3_data_ptr->SAPI3_link_state == SAPI3_LINK_ESTABLISHED) ||
      (rr_sapi3_data_ptr->SAPI3_link_state == SAPI3_LINK_PENDING))
  {
    rr_send_dl_release_req(
      LOCAL_END_RELEASE,                // release_mode
      SAPI3,                            // SAPI
      rr_get_l2_channel_type(gas_id),   // l2_channel_type
      gas_id                            // gas_id
    );

    /* L2 should immediately signal the DL_RELEASE_CNF to RR */
    /* which will cause the SAPI3 state machine to shut down */
  }

  return;
}

/* EOF */

