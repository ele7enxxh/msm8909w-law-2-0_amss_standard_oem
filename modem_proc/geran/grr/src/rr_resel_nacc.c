/*==============================================================================
  FILE:         rr_resel_nacc.c

  OVERVIEW:     This module contains functions related to the Network Assisted
                Cell Change procedure.

  DEPENDENCIES: None

                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_nacc.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"


#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_gprs_defs.h"
#include "rr_resel_nacc.h"
#include "rr_resel_utils.h"
#include "rr_mac_send.h"
#include "rr_l1_send.h"
#include "rr_resel.h"
#include "rr_control.h"
#include "rr_sys_info_ncell.h"
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_pacch.h"
#ifdef FEATURE_WCDMA
#include "rr_sys_info_3g.h"
#include "rr_g2w.h"
#endif /* FEATURE_WCDMA */
#include "rr_nv.h"
#include "rr_gprs_dsc.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"
#include "geran_multi_sim_api.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#define NACC_CONTAINER_TIMER(n) ((rr_timer_id_T)((n) + GRR_NACC_CONTAINER_TIMER_0))

#define NUM_NACC_CONTAINERS  MAX_NUMBER_OF_NACC_NCELLS

#define MAX_NUM_NACC_STORED_MSGS 16 /* this value may need to be tweaked */
#define MAX_NUM_QUEUED_MSGS       3 /* this value may need to be tweaked */

#define NACC_BA_GPRS_THRESHOLD    2
#define NACC_STATUS_TIMEOUT      (GRR_TIMEOUT_1_SECOND+GRR_TIMEOUT_500_MILLISECONDS)

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef struct
{
  csn_decoder_ptrs_t csn_decode_struct;
  uint8              container_id;
  uint8              protocol;
} nacc_stored_msg_t;

typedef enum
{
  NACC_INACTIVE,
  NACC_PROCESS,
  NACC_COMPLETE
} nacc_state_t;

typedef enum
{
  CONTAINER_EMPTY, /* no SI or PSI messages found for the specified container           */
  CONTAINER_SI_OK, /* one or more SI messages were found for the specified container    */
  CONTAINER_NO_SI  /* no SI messages were found for the specified container (PSIs only) */
} container_info_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

typedef struct
{
  uint8                 nacc_msg_count;
  boolean               nacc_trigger_actioned;
  word                  nacc_received_si_mask;
  rr_l1_handover_type_T nacc_RAT_type;
  nacc_state_t          old_nacc_state;
  nacc_state_t          nacc_state;
  ARFCN_T               nacc_status_arfcn;
  nacc_stored_msg_t     nacc_stored_msgs[MAX_NUM_NACC_STORED_MSGS];
} rr_resel_nacc_data_t;

#define _RR_RESEL_NACC_DATA_INIT {0}

static rr_resel_nacc_data_t rr_resel_nacc_data[NUM_GERAN_DATA_SPACES] =
                      { INITIAL_VALUE(_RR_RESEL_NACC_DATA_INIT) };
#undef _RR_RESEL_NACC_DATA_INIT

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_resel_nacc_data_t*
 */
static rr_resel_nacc_data_t *rr_resel_nacc_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_resel_nacc_data[as_index];
}

static const char *ccn_event_name(
  ccn_event_t event
)
{
  switch (event)
  {
    case CCN_RESET:            return "CCN_RESET";
    case CCN_RESPONSE_FROM_NW: return "CCN_RESPONSE_FROM_NW";
    case CCN_REQUESTED_TO_L1:  return "CCN_REQUESTED_TO_L1";
    case CCN_MAC_IN_TRANSFER:  return "CCN_MAC_IN_TRANSFER";
    case CCN_MAC_IN_IDLE:      return "CCN_MAC_IN_IDLE";
    default:
    {
      MSG_GERAN_ERROR_1("Unexpected event: %d", event);
      return "?";
    }
  }
}

static const char *nacc_state_name(
  nacc_state_t state
)
{
  switch (state)
  {
    case NACC_INACTIVE: return "NACC_INACTIVE";
    case NACC_PROCESS:  return "NACC_PROCESS";
    case NACC_COMPLETE: return "NACC_COMPLETE";
    default:
    {
      MSG_GERAN_ERROR_1("Unexpected state: %d", state);
      return "?";
    }
  }
}

/****************************************************************************
 *   Function name:  rr_gprs_nacc_container_info
 *   -------------
 *
 *   Description:
 *   ------------
 *   Determines whether stored msgs for a specific container include BCCH SIs
 *
 *   Parameters:
 *   ----------
 *   container_id - the ID of the container to be checked
 *
 *   Returns:
 *   --------
 *   CONTAINER_EMPTY, CONTAINER_SI_OK or CONTAINER_NO_SI
 *
 ****************************************************************************/
static container_info_t rr_gprs_nacc_container_info(
  uint8 container_id,
  const gas_id_t gas_id
)
{
  nacc_stored_msg_t *nacc_msg_ptr;
  container_info_t   container_info = CONTAINER_EMPTY;
  int8               index;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  nacc_msg_ptr = rr_resel_nacc_data_ptr->nacc_stored_msgs;
  for (index = 0; index < rr_resel_nacc_data_ptr->nacc_msg_count; index++, nacc_msg_ptr++)
  {
    /* if the container ID matches, check the contents */
    if (nacc_msg_ptr->container_id == container_id)
    {
      if (nacc_msg_ptr->protocol == BCCH_PD)
      {
        container_info = CONTAINER_SI_OK;
      }
      else
      {
        /* first stored message for this container is PBCCH protocol */
        /* since SIs would be stored first, this means that none are available */
        container_info = CONTAINER_NO_SI;
      }
      /* exit loop now */
      break;
    }
  }

  return(container_info);
}

/****************************************************************************
 *   Function name:  rr_gprs_nacc_data_ind
 *   -------------
 *
 *   Description:
 *   ------------
 *   Takes a message stored in this module's local data and converts it into
 *   a MAC_GRR_DATA_IND signal that can be handled as a PSI or SI message
 *
 *   Parameters:
 *   ----------
 *   nacc_msg_ptr - pointer to an element in local NACC data store
 *   arfcn        - the ARFCN that the resulting signal will appear to be from
 *
 *   Returns:
 *   --------
 *   TRUE if the signal was queued successfully
 *
 ****************************************************************************/
static boolean rr_gprs_nacc_data_ind(
  const nacc_stored_msg_t * nacc_msg_ptr,
  ARFCN_T                   arfcn,
  const gas_id_t            gas_id
)
{
  mac_grr_data_ind_t mac_sig;

  mac_sig.message_header.message_set = MS_MAC_RR;
  mac_sig.message_header.message_id  = (byte)MAC_GRR_DATA_IND;
  PUT_IMH_LEN((sizeof(mac_grr_data_ind_t)-sizeof(IMH_T)), &(mac_sig.message_header));

  mac_sig.data_ptr   = nacc_msg_ptr->csn_decode_struct;
  mac_sig.bcch_arfcn = arfcn;
  mac_sig.gas_id     = gas_id;

  return(gs_send_message(GS_QUEUE_RR, &mac_sig, TRUE) == GS_SUCCESS);
}

/****************************************************************************
 *   Function name:  rr_gprs_nacc_free_container_data
 *   -------------
 *
 *   Description:
 *   ------------
 *   Frees all stored ncell data in this module for the specified container
 *
 *   Parameters:
 *   ----------
 *   container_id - NACC container ID that should be freed
 *   bReleaseGMAC - boolean indication of whether to release data in GMAC
 *
 *   Returns:
 *   --------
 *   Void
 *
 ****************************************************************************/
static void rr_gprs_nacc_free_container_data(
  uint8 container_id,
  boolean bReleaseGMAC,
  const gas_id_t gas_id
)
{
  nacc_stored_msg_t *nacc_msg_ptr;
  uint8              msg_index = 0;
  uint8              new_count;
  uint8              new_pos;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  nacc_msg_ptr = rr_resel_nacc_data_ptr->nacc_stored_msgs;
  new_count    = rr_resel_nacc_data_ptr->nacc_msg_count;

  while (msg_index < rr_resel_nacc_data_ptr->nacc_msg_count)
  {
    if (nacc_msg_ptr->container_id == container_id)
    {
      /* free the memory allocated by the CSN1 decoder */
      free_csn_decoder_ptrs_t(&(nacc_msg_ptr->csn_decode_struct));
      nacc_msg_ptr->container_id = INVALID_CONTAINER_ID;
      new_count--;

      /* move every following entry up one position */
      for (new_pos = msg_index;
           (new_pos < (rr_resel_nacc_data_ptr->nacc_msg_count-1)) && (new_pos < (MAX_NUM_NACC_STORED_MSGS - 1));
           ++new_pos)
      {
        rr_resel_nacc_data_ptr->nacc_stored_msgs[new_pos] = rr_resel_nacc_data_ptr->nacc_stored_msgs[new_pos+1];
        rr_resel_nacc_data_ptr->nacc_stored_msgs[new_pos+1].container_id = INVALID_CONTAINER_ID;
      }
    }
    else
    {
      /* no match on this entry, move to the next one */
      msg_index++;
      nacc_msg_ptr++;
    }
  }

  /* update the global count */
  rr_resel_nacc_data_ptr->nacc_msg_count = new_count;

  if ((container_id < NUM_NACC_CONTAINERS) && (bReleaseGMAC == TRUE))
  {
    /* ask GMAC to release neighbour cell data for this container */
    rr_send_grr_mac_nacc_release_req(container_id, NACC_RELEASE_ONE, gas_id);
  }

  return;
}

/****************************************************************************
 *   Function name: rr_gprs_nacc_force_sys_info_refresh
 *   -------------
 *
 *   Description:
 *   ------------
 *   Forces a refresh of Si2 during transfer mode
 *
 *   Parameters:
 *   ----------
 *   None
 *
 *   Returns:
 *   --------
 *   State transition value (always NACC_INACTIVE)
 *
 ****************************************************************************/
static nacc_state_t rr_gprs_nacc_force_sys_info_refresh(
  const gas_id_t gas_id
)
{
  inter_task_BCCH_allocation_T *ba_ptr = rr_gprs_get_ba_gprs(gas_id);

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  /* If BA(GPRS) is suspiciously short, force a SI2 refresh */
  /* Note that this only happens on CCCH cells, as there is */
  /* a better chance of receiving a BA list during transfer */
  /* on a PBCCH cell (via PSI3 on the PACCH).               */
  if ((ba_ptr->no_of_entries <= NACC_BA_GPRS_THRESHOLD) &&
      (rr_resel_nacc_data_ptr->nacc_RAT_type         != NACC_WCDMA_GSM))
  {
    /* ask for SI2 to be read as it is probably missing */
    rr_send_mph_read_sys_info_req(RR_L1_SI_TYPE_2, gas_id);
  }
  else
  {
    MSG_GERAN_HIGH_2_G("NACC status: RAT=%1d, BA=%d",
             (int)rr_resel_nacc_data_ptr->nacc_RAT_type, (int)ba_ptr->no_of_entries);
  }

  return(NACC_INACTIVE);
}

/****************************************************************************
 *   Function name: rr_gprs_nacc_send_status
 *   -------------
 *
 *   Description:
 *   ------------
 *   Attempts to send SI Status once the MS has resumed transfer after NACC
 *
 *   Parameters:
 *   ----------
 *   None
 *
 *   Returns:
 *   --------
 *   nacc_state_t - NACC state to transition to (dependent on success of status)
 *
 ****************************************************************************/
static nacc_state_t rr_gprs_nacc_send_status(
  const gas_id_t gas_id
)
{
  nacc_state_t  next_state = NACC_INACTIVE;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  /* set sys info status parameters */
  rr_gprs_set_sysinfo_status_params(
    rr_resel_nacc_data_ptr->nacc_received_si_mask,
    FALSE,
    FALSE,
    gas_id
  );

  /* then reset the mask of SIs received during the NACC procedure */
  /* NOTE: this does not affect the contents of the SI/PSI status  */
  rr_resel_nacc_data_ptr->nacc_received_si_mask = 0x0000;

  /* try to send Pkt SI Status or Pkt PSI Status on the current cell */
  if (rr_gprs_send_sysinfo_status(NACC_STATUS_TIMEOUT, gas_id))
  {
    /* successful NACC procedure can now go inactive */
    next_state = NACC_INACTIVE;
  }
  else
  {
    next_state = rr_gprs_nacc_force_sys_info_refresh(gas_id);
  }

  /* the NACC RAT type can now be reset to the default value */
  rr_resel_nacc_data_ptr->nacc_RAT_type = NACC_GSM_GSM;

  return(next_state);
}

#ifdef FEATURE_WCDMA

/****************************************************************************
 *   Function name:  rr_gprs_nacc_wrap_si_data
 *   -------------
 *
 *   Description:
 *   ------------
 *   Wraps a SI message in the NCELL_DATA_IND structure
 *
 *   Parameters:
 *   ----------
 *   wrapper_ptr - output pointer to NCELL_DATA_IND
 *   msg_ptr     - pointer to message data (single SI in encoded form)
 *   num_octets  - length of the message data supplied
 *
 *   Returns:
 *   --------
 *   TRUE if the operation was successful
 *
 ****************************************************************************/
static boolean rr_gprs_nacc_wrap_si_data(
  mac_grr_ncell_data_ind_t * wrapper_ptr,
  const uint8 *              msg_ptr,
  uint8                      num_octets,
  const gas_id_t             gas_id
)
{
  if (num_octets <= N201_MAX)
  {
    uint16 dummy;
    uint8  si_buffer[N201_MAX+2];

    /* load the local buffer with a CSN message type, length, and message data */
    dummy = ((uint8)PACKET_SI_DATA) << 2;
    si_buffer[0] = (uint8)dummy;
    si_buffer[1] = num_octets;
    //Replace memcpy with memscpy
  memscpy(si_buffer+2,sizeof(si_buffer)-2, msg_ptr, num_octets);


    /* call the CSN decoder to create a correctly formatted output structure */
    wrapper_ptr->data_ptr = gcsn1_decoder(si_buffer, &dummy,N201_MAX+2, gas_id);

    /* and check the CSN decoder return value */
    if (wrapper_ptr->data_ptr.header_ptr->error_code == CSN_DECODE_NO_ERROR)
    {
      return(TRUE);
    }
  }

  return(FALSE);
}

#endif /* FEATURE_WCDMA */

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 *---------------------------------------------------------------------------*/

/****************************************************************************
 *   Function name:  rr_gprs_nacc_pncd_in_progress
 *   -------------
 *
 *   Description:
 *   ------------
 *   Indicates whether PNCD dispatch has been triggered
 *
 *   Parameters:
 *   ----------
 *   Void
 *
 *   Returns:
 *   --------
 *   TRUE if the PNCD dispatch has been started
 *
 ****************************************************************************/
boolean rr_gprs_nacc_pncd_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  return(rr_resel_nacc_data_ptr->nacc_trigger_actioned);
}

/****************************************************************************
 *   Function name:  rr_gprs_nacc_store_trigger_info
 *   -------------
 *
 *   Description:
 *   ------------
 *   Stores the container ID, PNCD type and RAT when the NACC trigger occurs
 *
 *   Parameters:
 *   ----------
 *   container_id - container ID used by the trigger
 *
 *   Returns:
 *   --------
 *   Void
 *
 ****************************************************************************/
void rr_gprs_nacc_store_trigger_info(
  uint8 container_id,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  if (container_id != INVALID_CONTAINER_ID)
  {
    if (container_id < NUM_NACC_CONTAINERS)
    {
      rr_resel_nacc_data_ptr->nacc_RAT_type  = NACC_GSM_GSM;
    }
    else if (container_id == INTERRAT_CONTAINER_ID_1)
    {
      rr_resel_nacc_data_ptr->nacc_RAT_type  = NACC_WCDMA_GSM;
    }
    else
    {
      /* no other container ID is valid here */
      MSG_GERAN_ERROR_1_G("NACC: Invalid container ID %d", container_id);
      rr_resel_nacc_data_ptr->nacc_RAT_type  = NACC_GSM_GSM;
    }
  }
  else
  {
    /* invalid container ID, reset flags */
    rr_resel_nacc_data_ptr->nacc_RAT_type  = NACC_GSM_GSM;
  }

  return;
}

/**
  @brief Initialises all NACC data, ensures all NACC timers are stopped, frees
  any memory associated with NACC data stores (e.g. from CSN decoder).

*/
void rr_nacc_init(
  const gas_id_t gas_id
)
{
  nacc_stored_msg_t *nacc_msg_ptr;
  uint8              timer_stopped[NUM_NACC_CONTAINERS] = {0,0,0,0};
  uint8              container_id;
  uint8              index;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  nacc_msg_ptr = rr_resel_nacc_data_ptr->nacc_stored_msgs;

  /* discard any previously stored info */
  for (index = 0; index < MAX_NUM_NACC_STORED_MSGS; index++, nacc_msg_ptr++)
  {
    if (index < rr_resel_nacc_data_ptr->nacc_msg_count)
    {
      /* could be previous data at this position */
      container_id = nacc_msg_ptr->container_id;

      if (container_id < NUM_NACC_CONTAINERS)
      {
        /* stop the container timer if necessary */
        if (timer_stopped[container_id] == 0)
        {
          (void)rr_timer_cancel(NACC_CONTAINER_TIMER(container_id), gas_id);
          timer_stopped[container_id] = 1;
        }
      }

      if (container_id != INVALID_CONTAINER_ID)
      {
        /* free the memory allocated by the CSN1 decode */
        free_csn_decoder_ptrs_t(&(nacc_msg_ptr->csn_decode_struct));
      }
    }

    /* invalidate the store entry */
    nacc_msg_ptr->container_id = INVALID_CONTAINER_ID;
    nacc_msg_ptr->protocol     = BCCH_PD;
    nacc_msg_ptr->csn_decode_struct.header_ptr = NULL;
    nacc_msg_ptr->csn_decode_struct.data_ptr   = NULL;
  }

  /* reset global variables */
  rr_gprs_nacc_store_trigger_info(INVALID_CONTAINER_ID, gas_id);
  rr_resel_nacc_data_ptr->nacc_msg_count  = 0;
  rr_resel_nacc_data_ptr->nacc_trigger_actioned = FALSE;
}

/**
  @brief Requests MAC to release any NACC containers if required, then
  reinitialises all NACC-related data in RR.

  If any NACC Container timers are running, this function will send
  MAC a GRR_MAC_NACC_RELEASE_REQ indicating that it should release all NACC
  containers.  After this, rr_nacc_init() is called, which will cancel all
  of these timers and clear all NACC-related data in RR.
*/
void rr_gprs_nacc_reset(
  const gas_id_t gas_id
)
{
  /* only release GMAC if it has anything to release, i.e. if any of the
  NACC timers are running. */
  if (rr_timer_is_running(GRR_NACC_CONTAINER_TIMER_0, gas_id) ||
      rr_timer_is_running(GRR_NACC_CONTAINER_TIMER_1, gas_id) ||
      rr_timer_is_running(GRR_NACC_CONTAINER_TIMER_2, gas_id) ||
      rr_timer_is_running(GRR_NACC_CONTAINER_TIMER_3, gas_id))
  {
    /* ask GMAC to release neighbour cell data for ALL containers */
    rr_send_grr_mac_nacc_release_req(0, NACC_RELEASE_ALL, gas_id);
  }

  rr_nacc_init(gas_id);
  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_nacc_store_ncell_data
 *   -------------
 *
 *   Description:
 *   ------------
 *   Accepts incoming NACC data from MAC and stores it locally in this module
 *   Starts a timer for the container that included this data
 *
 *   Parameters:
 *   ----------
 *   ncell_ind_ptr - pointer to message received from MAC
 *
 *   Returns:
 *   --------
 *   Void
 *
 ****************************************************************************/
void rr_gprs_nacc_store_ncell_data(
  mac_grr_ncell_data_ind_t *ncell_ind_ptr,
  const gas_id_t gas_id
)
{
  uint8 container_id;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  if (ncell_ind_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("Null ptr supplied as NACC data");
    return;
  }

  /* extract the container ID */
  container_id = ncell_ind_ptr->container_id;

  /* cancel the re-transmission timer for PCCN */
  (void)rr_timer_cancel(T3210, gas_id);

  if (rr_resel_nacc_data_ptr->nacc_msg_count < MAX_NUM_NACC_STORED_MSGS)
  {
    nacc_stored_msg_t *nacc_msg_ptr;

    /* store the incoming message in the global store in the last unused space */
    nacc_msg_ptr = rr_resel_nacc_data_ptr->nacc_stored_msgs + rr_resel_nacc_data_ptr->nacc_msg_count;

    /* only store data for valid protocols */
    if (ncell_ind_ptr->protocol != DUMMY_PD)
    {
      /* nacc_msg_ptr is now indicating the new location */
      nacc_msg_ptr->csn_decode_struct = ncell_ind_ptr->data_ptr;
      nacc_msg_ptr->protocol          = ncell_ind_ptr->protocol;
      nacc_msg_ptr->container_id      = container_id;

      /* one more message has been stored */
      rr_resel_nacc_data_ptr->nacc_msg_count++;
    }
    else if (rr_resel_nacc_data_ptr->nacc_msg_count > 0)
    {
      /* The "dummy" PD indicates that GMAC has received PNCD instances, but has */
      /* not yet received enough data to extract complete system info from them. */
      /* This message is only used by GRR to start the relevant container timer. */
      /* For debug purposes, output a message if this module already has some    */
      /* data stored locally for the same container ID. This can happen if the   */
      /* NW has chosen to repeat all PNCD instances before sending PCCC or PCCO. */
      if (rr_gprs_nacc_container_info(container_id, gas_id) != CONTAINER_EMPTY)
      {
        MSG_GERAN_HIGH_2_G("Neighbour cell data restart for container %1d (%d)",
                 (int)container_id, (int)(rr_resel_nacc_data_ptr->nacc_msg_count));
      }
    }

    if (container_id < NUM_NACC_CONTAINERS)
    {
      rr_timer_id_T timer_id;

      /* (re)start a 30 second timer for the specified container (2G NACC only) */
      timer_id = NACC_CONTAINER_TIMER(container_id);
      (void)rr_timer_restart(timer_id, GRR_TIMEOUT_30_SECONDS, gas_id);
      MSG_GERAN_HIGH_2_G("NACC data(%1X) for container %1d, timer started", ncell_ind_ptr->protocol, container_id);
    }
  }
  else
  {
    /* no storage available, free the data contents */
    if (ncell_ind_ptr->data_ptr.header_ptr)
    {
      free_csn_decoder_ptrs_t(&(ncell_ind_ptr->data_ptr));
    }
    MSG_GERAN_ERROR_2_G("NACC msg discarded: container %d, count %d", container_id, rr_resel_nacc_data_ptr->nacc_msg_count);
  }

  return;
}

#ifdef FEATURE_WCDMA

/****************************************************************************
 *   Function name:  rr_gprs_nacc_store_wtog_data
 *   -------------
 *
 *   Description:
 *   ------------
 *   Accepts incoming NACC data from a WTOG source and stores it locally.
 *   Reuses existing functionality by creating a wrapper for the incoming data
 *
 *   Parameters:
 *   ----------
 *   data_type  - one of the GERAN_INFO_TYPE_ constants from rrc_rr_types.h
 *   msg_ptr    - pointer to message data (single SI or PSI message in encoded form)
 *   num_octets - length of the data supplied
 *
 *   Returns:
 *   --------
 *   Void
 *
 ****************************************************************************/
void rr_gprs_nacc_store_wtog_data(
  geran_sysinfo_choice_e_type data_type,
  uint8 *msg_ptr,
  uint8 num_octets,
  const gas_id_t gas_id
)
{
  mac_grr_ncell_data_ind_t wrapper;

  /* default to a protocol error value */
  wrapper.protocol = DUMMY_PD;

  if ((data_type  == GERAN_INFO_TYPE_NONE) ||
      (msg_ptr    == NULL)                 ||
      (num_octets == 0)                    ||
      (num_octets >  N201_MAX))
  {
    /* supplied parameters indicate invalid type, null pointer, or bad length */
    MSG_GERAN_ERROR_3_G("W2G NACC: invalid data typ=%d ptr=%08X len=%d", data_type, msg_ptr, num_octets);
  }
  else if (data_type == GERAN_INFO_TYPE_SI)
  {
    /* attempt to wrap the SI message data */
    if (rr_gprs_nacc_wrap_si_data(&wrapper, msg_ptr, num_octets, gas_id))
    {
      /* flag the wrapper as SI data */
      wrapper.protocol = BCCH_PD;

      /* advance the pointer to the message type (for debug output) */
      msg_ptr += 2;
    }
    else
    {
      MSG_GERAN_ERROR_1_G("W2G NACC: unable to wrap SI data (%d)", (int)num_octets);
    }
  }

  if (wrapper.protocol != DUMMY_PD)
  {
    /* finalise the wrapper and pass it to the normal NACC data handler */
    MSG_GERAN_HIGH_2_G("W2G NACC: storing msg type 0x%02X (prot=%1d)", (int)(*msg_ptr), (int)wrapper.protocol);
    if (wrapper.protocol == BCCH_PD)
    {
      wrapper.container_id = INTERRAT_CONTAINER_ID_1;
    }
    rr_gprs_nacc_store_ncell_data(&wrapper, gas_id);
  }

  return;
}

#endif /* FEATURE_WCDMA */



/****************************************************************************
 *   Function name:  rr_gprs_nacc_handle_timer_expiry
 *   -------------
 *
 *   Description:
 *   ------------
 *   Handles the timer expiry that indicates a container is now invalid
 *   Should indicate to MAC that the container can now be released
 *
 *   Parameters:
 *   ----------
 *   timer_id - RR timer ID which can be matched to a specific container ID
 *
 *   Returns:
 *   --------
 *   An event for RR control
 *
 ****************************************************************************/
rr_event_T rr_gprs_nacc_handle_timer_expiry(
  rr_timer_id_T timer_id,
  const gas_id_t gas_id
)
{
  uint8 container_id;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  for (container_id = 0; container_id < NUM_NACC_CONTAINERS; container_id++)
  {
    if (timer_id == NACC_CONTAINER_TIMER(container_id))
    {
      MSG_GERAN_HIGH_2_G("NACC timer expiry for container ID %1d (%d)",
               (int)container_id, (int)(rr_resel_nacc_data_ptr->nacc_msg_count));
      rr_gprs_nacc_free_container_data(container_id, TRUE, gas_id);
      /* exit from loop - only one container per timer ID */
      break;
    }
  }

  return(EV_NO_EVENT);
}



/****************************************************************************
 *   Function name:  rr_gprs_nacc_trigger_confirm
 *   -------------
 *
 *   Description:
 *   ------------
 *   Posts a signal to the RR task queue to indicate that confirm that NACC
 *   data has been triggered. This signal can be used to trigger further data.
 *
 *   Parameters:
 *   ----------
 *   container_id - the ID of the container used by the trigger
 *   arfcn_ptr    - pointer to ARFCN used by the trigger
 *   msgs_left    - the number of messages that remain in the NACC data store
 *
 *   Returns:
 *   --------
 *   TRUE if the confirmation signal was queued successfully
 *
 ****************************************************************************/
boolean rr_gprs_nacc_trigger_confirm(
  uint8           container_id,
  const ARFCN_T * arfcn_ptr,
  uint8           msgs_left,
  const gas_id_t  gas_id
)
{
  mac_grr_ncell_data_cnf_t mac_sig;

  mac_sig.message_header.message_set = MS_MAC_RR;
  mac_sig.message_header.message_id  = (byte)MAC_GRR_NCELL_DATA_CNF;
  PUT_IMH_LEN((sizeof(mac_grr_ncell_data_cnf_t)-sizeof(IMH_T)), &(mac_sig.message_header));

  mac_sig.container_id = container_id;
  if (arfcn_ptr != NULL)
  {
    mac_sig.arfcn.num = arfcn_ptr->num;
    mac_sig.arfcn.band = arfcn_ptr->band;
  }
  mac_sig.msgs_left = msgs_left;
  mac_sig.gas_id    = gas_id;

  return(gs_send_message(GS_QUEUE_RR, &mac_sig, TRUE) == GS_SUCCESS);
}

/****************************************************************************
 *   Function name:  rr_gprs_nacc_si1_detection
 *   -------------
 *
 *   Description:
 *   ------------
 *   Checks the supplied NACC message store pointer for Sys Info Type 1
 *   If not found, flags the message as present in the pending channel DB
 *   so that GRR does not wait to receive SI1 on the target cell later
 *
 *   Parameters:
 *   ----------
 *   nacc_msg_ptr  - pointer to stored NACC message
 *
 *   Returns:
 *   --------
 *   Void
 *
 ****************************************************************************/
static void rr_gprs_nacc_si1_detection(
  nacc_stored_msg_t *nacc_msg_ptr,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *pcell_ptr;
  uint8              msg_type;

  /* check for SI1 presence in this message */
  if ((nacc_msg_ptr->csn_decode_struct.data_ptr != NULL) &&
      (nacc_msg_ptr->container_id < NUM_NACC_CONTAINERS) &&
      (nacc_msg_ptr->protocol == BCCH_PD))
  {
    /* extract the message type from the received structure */
    pcell_ptr = rr_gprs_get_pcell_info(gas_id);
    msg_type  = *(nacc_msg_ptr->csn_decode_struct.data_ptr + 2);
    MSG_GERAN_HIGH_2_G("First BCCH msg for container %d is 0x%02X", (int)(nacc_msg_ptr->container_id), (int)msg_type);
  }
  else
  {
    /* set dummy defaults here to bypass the SI1 detection below */
    pcell_ptr = NULL;
    msg_type  = SYSTEM_INFORMATION_TYPE_1;
  }

  /* update the pending channel database according to SI1 detection */
  if ((pcell_ptr != NULL) &&
      (msg_type  != SYSTEM_INFORMATION_TYPE_1))
  {
    /* See 3GPP spec CR GP-041089: SI1 should be */
    /* noted as NOT BROADCAST in the target cell */
    pcell_ptr->gsm.update_message_received |= MASK_SI_1;
  }

  return;
}

/****************************************************************************
 *   Function name:  rr_gprs_nacc_trigger_ncell_data
 *   -------------
 *
 *   Description:
 *   ------------
 *   Initiates the dispatch of stored NACC messages to the RR task queue.
 *   This function should be called once the network has confirmed that a
 *   particular container ID and ARFCN is to be used for reselection.
 *
 *   Parameters:
 *   ----------
 *   container_id  - the ID of the container used by the trigger
 *   arfcn         - the ARFCN used by the trigger
 *   new_procedure - whether this trigger indicates the start of a new NACC procedure
 *
 *   Returns:
 *   --------
 *   Void
 *
 ****************************************************************************/
uint8 rr_gprs_nacc_trigger_ncell_data(
  uint8 container_id,
  ARFCN_T target_arfcn,
  boolean new_procedure,
  const gas_id_t gas_id
)
{
  nacc_stored_msg_t *nacc_msg_ptr;
  uint8              num_stored = 0;
  uint8              index;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  if (new_procedure)
  {
    /* New NACC trigger procedure, reset global flags */
    rr_gprs_nacc_store_trigger_info(container_id, gas_id);
    rr_resel_nacc_data_ptr->nacc_received_si_mask = 0x0000;
    rr_resel_nacc_data_ptr->nacc_trigger_actioned = TRUE;
  }

  /* scan the NACC store for msgs for the specified container */
  nacc_msg_ptr = rr_resel_nacc_data_ptr->nacc_stored_msgs;
  for (index = 0; index < rr_resel_nacc_data_ptr->nacc_msg_count; index++, nacc_msg_ptr++)
  {
    if (nacc_msg_ptr->container_id == container_id)
    {
      num_stored++;
    }
    else if (nacc_msg_ptr->container_id < NUM_NACC_CONTAINERS)
    {
      /* for any other (non-interRAT) container, just free the message now */
      free_csn_decoder_ptrs_t(&(nacc_msg_ptr->csn_decode_struct));
      nacc_msg_ptr->container_id = INVALID_CONTAINER_ID;
    }
  }

  if (num_stored == 0)
  {
    MSG_GERAN_HIGH_1_G("NACC: no msgs for container %d", container_id);
    rr_resel_nacc_data_ptr->nacc_msg_count = 0;
  }
  else
  {
    uint8 num_queued = 0;

    if (container_id < NUM_NACC_CONTAINERS)
    {
      /* stop the container timer now, in case it expires during the dispatch process */
      (void)rr_timer_cancel(NACC_CONTAINER_TIMER(container_id), gas_id);
    }

    /* now process msgs for the specified container */
    nacc_msg_ptr = rr_resel_nacc_data_ptr->nacc_stored_msgs;
    for (index = 0; index < rr_resel_nacc_data_ptr->nacc_msg_count; index++, nacc_msg_ptr++)
    {
      /* if the container ID matches, dispatch the stored message */
      if (nacc_msg_ptr->container_id == container_id)
      {
        /* check the first message being dispatched for this container */
        if ((new_procedure == TRUE) && (num_queued == 0))
        {
          rr_gprs_nacc_si1_detection(nacc_msg_ptr, gas_id);
        }

        if (rr_gprs_nacc_data_ind(nacc_msg_ptr, target_arfcn, gas_id))
        {
          /* if successful, mark the message as dispatched */
          nacc_msg_ptr->container_id = INVALID_CONTAINER_ID;

          /* update counters to track those that remain */
          num_queued++;
          num_stored--;

          /* check watermark level */
          if (num_queued == MAX_NUM_QUEUED_MSGS)
          {
            /* drop out of the loop, to avoid queueing too many messages */
            break;
          }
        }
      }
    }
  }

  /* send a trigger confirmation indicating number of messages left */
  (void)rr_gprs_nacc_trigger_confirm(container_id, &target_arfcn, num_stored, gas_id);

  return(num_stored);
}

/****************************************************************************
 *   Function name:  rr_gprs_nacc_type
 *   -------------
 *
 *   Description:
 *   ------------
 *   Returns the current NACC type (G2G,W2G or G2W)
 *
 *   Parameters:
 *   ----------
 *   None
 *
 *   Returns:
 *   --------
 *   NACC_GSM_GSM, NACC_WCDMA_GSM, or NACC_GSM_WCDMA
 *
 ****************************************************************************/
rr_l1_handover_type_T rr_gprs_nacc_type(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  return(rr_resel_nacc_data_ptr->nacc_RAT_type);
}

/****************************************************************************
 *   Function name:  rr_gprs_nacc_additional_si_required
 *   -------------
 *
 *   Description:
 *   ------------
 *   Returns a mask of SIs that are still required on the NACC target cell
 *
 *   Parameters:
 *   ----------
 *   scell_ptr - pointer to the new serving cell info
 *
 *   Returns:
 *   --------
 *   16-bit SI mask (see rr_defs.h, e.g. MASK_SI_2)
 *
 ****************************************************************************/
uint16 rr_gprs_nacc_additional_si_required(
  system_information_database_T *si_db_ptr,
  const boolean                  include_3G
)
{
  uint16 bsi_mask = MASK_SI_NONE;

  if (si_db_ptr != NULL)
  {
    /* Generally speaking, after a NACC reselection to a CCCH target, the BA list
     * is incomplete because SI2 (and its variants) have not been received yet
     * The NACC code flags this by setting a bit in the cell's BA_IND.
     */
    if (si_db_ptr->SI2_BA_list.BA_IND & 0x02)
    {
      bsi_mask |= MASK_SI_2;
    }
    
    if ((si_db_ptr->SI2bis_BA_list.BA_IND & 0x02) &&
        (si_db_ptr->decode_SI2bis != RR_SI_NOT_REQUIRED))
    {
      bsi_mask |= MASK_SI_2_BIS;
    }
    
    if ((si_db_ptr->SI2ter_BA_list.BA_IND & 0x02) &&
        (si_db_ptr->two_ti != RR_SI2TER_NOT_EXPECTED) &&
        (si_db_ptr->decode_SI2ter != RR_SI_NOT_REQUIRED))
    {
      bsi_mask |= MASK_SI_2_TER;
      if (include_3G)
      {
        bsi_mask |= MASK_SI_2_TER_REST;
      }
    }
  }

  return(bsi_mask);
}

/****************************************************************************
 *   Function name: rr_gprs_nacc_handle_ncell_data_cnf
 *   -------------
 *
 *   Description:
 *   ------------
 *   Responds to a NCELL_DATA_CNF message received by GRR main task
 *   This confirmation signal can be generated for two reasons:
 *     - internal use by GRR, to trigger dispatch of PNCD into its queue;
 *     - by MAC, to indicate that the first PCCN has been sent successfully.
 *
 *   Parameters:
 *   ----------
 *   message_ptr - point to RR command message
 *
 *   Returns:
 *   --------
 *   Event for RR
 *
 ****************************************************************************/
rr_event_T rr_gprs_nacc_handle_ncell_data_cnf(
  rr_cmd_bdy_type *message_ptr,
  const gas_id_t gas_id
)
{
  mac_grr_ncell_data_cnf_t *data_cnf_ptr;
  rr_event_T                rr_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  data_cnf_ptr = &(message_ptr->mac_grr_sig.mac_grr_ncell_data_cnf);

  if (data_cnf_ptr->msgs_left)
  {
    uint8 msgs_left_after_dispatch;

    /* there are more messages in the NACC queue, trigger them to be dispatched */
    msgs_left_after_dispatch = rr_gprs_nacc_trigger_ncell_data(
      data_cnf_ptr->container_id,
      data_cnf_ptr->arfcn,
      FALSE,
      gas_id
    );

    if (msgs_left_after_dispatch == data_cnf_ptr->msgs_left)
    {
      /* for some reason, the trigger did not dispatch any of the remaining messages */
      /* this indicates a possible error in the queue dispatch mechanism */
#ifdef PERLUTF
      #error code not present
#else
      MSG_GERAN_ERROR_1_G("NACC queue jammed with %d msgs", msgs_left_after_dispatch);
#endif

      rr_event = EV_ALL_NACC_PNCD_DISPATCHED;

      /* reset NACC in GRR (and for the 2G case, in GMAC): will cause context switch */
      rr_gprs_nacc_reset(gas_id);
    }
  }
  else
  {
    if (data_cnf_ptr->container_id < NUM_NACC_CONTAINERS)
    {
      /* GRR has no data left for this container, ask MAC to release it */
      rr_send_grr_mac_nacc_release_req(
        data_cnf_ptr->container_id,
        NACC_RELEASE_ONE,
        gas_id
      );

      /* and generate a completion event for the reselection engine */
      rr_event = EV_ALL_NACC_PNCD_DISPATCHED;
    }
    else if (data_cnf_ptr->container_id == INTERRAT_CONTAINER_ID_1)
    {
      /* generate a completion event for the W2G cell selection engine */
      rr_event = EV_ALL_NACC_PNCD_DISPATCHED;
    }
    else
    {
      /* this DATA_CNF (from MAC) indicates that a PCCN has been sent */
      MSG_GERAN_HIGH_0_G("PCCN_DATA_CNF received from MAC");

      /* check whether this confirmation is for the first PCCN */
      if (rr_gprs_nacc_check_pccn_flag(gas_id))
      {
        /* start the PCCN resend timer */
        (void)rr_timer_start(T3210, T3210_TIMEOUT, gas_id);

        /* restart T3208 to guard the entire NACC procedure */
        (void)rr_timer_restart(T3208, T3208_TIMEOUT, gas_id);
      }
      else
      {
        MSG_GERAN_HIGH_0_G("PCCN resend completed");
      }
    }
  }

  /* if the dispatch procedure is now complete, reset the RAT & trigger flags */
  if (rr_event != EV_NO_EVENT)
  {
    rr_resel_nacc_data_ptr->nacc_RAT_type = NACC_GSM_GSM;
    rr_resel_nacc_data_ptr->nacc_trigger_actioned = FALSE;
  }

  return(rr_event);
}

/****************************************************************************
 *   Function name: rr_gprs_nacc_notify_mac_status_changed
 *   -------------
 *
 *   Description:
 *   ------------
 *   Notifies the NACC state machine of latest MAC status
 *
 *   Parameters:
 *   ----------
 *   mac_status - the new status of MAC (TRANSFER, IDLE etc)
 *
 *   Returns:
 *   --------
 *   Void
 *
 ****************************************************************************/
void rr_gprs_nacc_notify_mac_status_changed(
  rr_mac_status_t mac_status,
  const gas_id_t gas_id
)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  /* Access attempts on the new cell after W2G CCO will cause a MAC status change */
  /* either as a result of GPRS suspension for LAU, or TBF establishment for RAU  */
  /* Use this opportunity to set up the RAT type correctly for any NACC activity  */
  if (rr_x2g_cco_in_progress(gas_id))
  {
    MSG_GERAN_HIGH_1_G("W2G NACC: MAC status %1d", (int)mac_status);
    rr_resel_nacc_data_ptr->nacc_RAT_type = NACC_WCDMA_GSM;
  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  if (mac_status == MAC_TRANSFER)
  {
    rr_gprs_nacc_notify_event(CCN_MAC_IN_TRANSFER, gas_id);
  }
  else if (mac_status == MAC_IDLE)
  {
    rr_gprs_nacc_notify_event(CCN_MAC_IN_IDLE, gas_id);
  }

  return;
}

/****************************************************************************
 *   Function name: rr_gprs_nacc_notify_bcch_msg_received
 *   -------------
 *
 *   Description:
 *   ------------
 *   Notifies this module that a BCCH message was received in PNCD, PSCD or PSCS
 *
 *   Parameters:
 *   ----------
 *   l3_msg_ptr - the BCCH L3 message (usually a System Information message)
 *   source_msg - the RLC/MAC message type used to deliver the L3 data
 *
 *   Returns:
 *   --------
 *   VOID
 *
 ****************************************************************************/
void rr_gprs_nacc_notify_bcch_msg_received(
  const byte * l3_msg_ptr,
  uint8        source_msg,
  const gas_id_t gas_id
)
{
  byte message_octet;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  if (l3_msg_ptr == NULL)
  {
    return;
  }

  message_octet = *(l3_msg_ptr+2);

  switch(message_octet & 0x3F)
  {
  case SYSTEM_INFORMATION_TYPE_1:
    rr_resel_nacc_data_ptr->nacc_received_si_mask |= MASK_SI_1;
    break;

  case SYSTEM_INFORMATION_TYPE_3:
    rr_resel_nacc_data_ptr->nacc_received_si_mask |= MASK_SI_3;
    break;

  case SYSTEM_INFORMATION_TYPE_7:
    /* ignore this empty message used by the W->G NACC code */
    break;

  case SYSTEM_INFORMATION_TYPE_13:
    rr_resel_nacc_data_ptr->nacc_received_si_mask |= MASK_SI_13;
    break;

  default:
    if (rr_resel_nacc_data_ptr->nacc_trigger_actioned)
    {
      /* NACC trigger is in progress, message must be from PNCD */
      MSG_GERAN_MED_1_G("BCCH message 0x%02X received in Pkt Neighbour Cell Data", message_octet);
    }
    else if (source_msg == PACKET_SI_DATA)
    {
      /* message must be from PSCD (since no PNCD is in progress) */
      MSG_GERAN_MED_1_G("BCCH message 0x%02X received in Pkt Serving Cell Data", message_octet);
    }
    else if (source_msg == PACKET_SERVING_CELL_SI)
    {
      /* message is from PSCS - higher priority debug for these Rel 6 messages */
      MSG_GERAN_HIGH_1_G("BCCH message 0x%02X received in Pkt Serving Cell SI", message_octet);
    }
    else
    {
      /* source is unknown */
      MSG_GERAN_ERROR_2_G("BCCH message 0x%02X received from unknown source 0x%02X", message_octet, source_msg);
    }
    break;
  }

  return;
}

/****************************************************************************
 *   Function name: rr_gprs_nacc_notify_event
 *   -------------
 *
 *   Description:
 *   ------------
 *   Notifies the CCN "state machine" of an event relevant to reselection
 *
 *   Parameters:
 *   ----------
 *   event_in - see definition of ccn_event_t for possible values
 *
 *   Returns:
 *   --------
 *   Void
 *
 ****************************************************************************/
void rr_gprs_nacc_notify_event(
  ccn_event_t event_in,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  MSG_SPRINTF_1(
    MSG_SSID_DFLT,
    MSG_LEGACY_MED,
    "rr_gprs_nacc_notify_event(event_in => %s)",
    ccn_event_name(event_in)
  );

  /* Note how this "state machine" is really just tracking an event sequence. */
  /* This allows GRR to follow the CCN->RESELECTION->TRANSFER trail of events */
  /* in order to be able to send a sys info status message at the right time. */
  switch(event_in)
  {
  case CCN_RESET:
    /* reset to an inactive state */
    rr_resel_nacc_data_ptr->nacc_state = NACC_INACTIVE;
    break;

  case CCN_RESPONSE_FROM_NW:
    /* a CCN response was received from the NW, change state */
    rr_resel_nacc_data_ptr->nacc_state = NACC_PROCESS;
    break;

  case CCN_REQUESTED_TO_L1:
    if (rr_resel_nacc_data_ptr->nacc_state == NACC_PROCESS)
    {
      /* previously initiated a NACC process, reselection has now started */
      rr_resel_nacc_data_ptr->nacc_state = NACC_COMPLETE;
    }
    else
    {
      /* reselection engine has started a non-NACC reselection, reset now */
      rr_resel_nacc_data_ptr->nacc_state = NACC_INACTIVE;
    }
    break;

  case CCN_MAC_IN_TRANSFER:
    if (rr_resel_nacc_data_ptr->nacc_state == NACC_COMPLETE)
    {
      /* MAC has resumed transfer mode after a NACC process */
      rr_resel_nacc_data_ptr->nacc_state = rr_gprs_nacc_send_status(gas_id);
    }
    break;

  case CCN_MAC_IN_IDLE:
    break;

  default:
    MSG_GERAN_ERROR_2_G("NACC: invalid event %1d in state %1d", (int)event_in, (int)(rr_resel_nacc_data_ptr->nacc_state));
    break;
  }

  if (rr_resel_nacc_data_ptr->nacc_state != rr_resel_nacc_data_ptr->old_nacc_state)
  {
    MSG_SPRINTF_2(
      MSG_SSID_DFLT,
      MSG_LEGACY_MED,
      "nacc_state(%s->%s)",
      nacc_state_name(rr_resel_nacc_data_ptr->old_nacc_state),
      nacc_state_name(rr_resel_nacc_data_ptr->nacc_state)
    );

    rr_resel_nacc_data_ptr->old_nacc_state = rr_resel_nacc_data_ptr->nacc_state;
  }

  return;
}

/****************************************************************************
 *   Function name: rr_gprs_nacc_exit_ccn_mode
 *   -------------
 *
 *   Description:
 *   ------------
 *   Leaves the CCN mode by setting a new state and event
 *
 *   Parameters:
 *   ----------
 *   state_ptr, event_ptr: new data for calling state machine
 *
 *   Returns:
 *   --------
 *   EV_RESUME_CELL_RESELECTION if normal GSM<->GSM cell reselection should
 *   be resumed.
 *
 *   RR_EV_FORCE_WCDMA_CELL_RESELECTION if GSM->WCDMA cell reselection should
 *   be resumed.
 *
 ****************************************************************************/
rr_event_T rr_gprs_nacc_exit_ccn_mode(
  const gas_id_t gas_id
)
{
  rr_event_T output_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  if (rr_resel_nacc_data_ptr->nacc_RAT_type == NACC_WCDMA_GSM)
  {
    /* this is an error condition, as W2G NACC does not enter the CCN mode */
    /* reset the NACC utilities here and recover state as much as possible */
    MSG_GERAN_ERROR_0_G("Unexpected NACC RAT type, resume reselection");
    rr_gprs_nacc_reset(gas_id);
    rr_resel_nacc_data_ptr->nacc_RAT_type = NACC_GSM_GSM;
  }

  if (rr_resel_nacc_data_ptr->nacc_RAT_type == NACC_GSM_GSM)
  {
    output_event = EV_RESUME_CELL_RESELECTION;
    MSG_GERAN_HIGH_0_G("Leaving CCN state, continue normal reselection");
  }
  else if (rr_resel_nacc_data_ptr->nacc_RAT_type == NACC_GSM_WCDMA)
  {
    /* to exit from the CCN state back to the G2W reselection, */
    /* just regenerate the FORCE_WCDMA event, but this time    */
    /* NACC will be disabled because the RAT type is still G2W */
    output_event = RR_EV_FORCE_WCDMA_CELL_RESELECTION;
    MSG_GERAN_HIGH_0_G("Leaving CCN state, continue G2W reselection");
  }

  return(output_event);
}

/****************************************************************************
 *   Function name: rr_gprs_nacc_is_allowed
 *   -------------
 *
 *   Description:
 *   ------------
 *   Determines whether NV support for NACC is enabled on this MS
 *
 *   Parameters:
 *   ----------
 *   rat_type - the type of NACC procedure required (G2G, W2G or G2W)
 *
 *   Returns:
 *   --------
 *   TRUE if the NV indicates this type of NACC is allowed on this MS
 *
 ****************************************************************************/
boolean rr_gprs_nacc_is_allowed(
  rr_l1_handover_type_T rat_type,
  const gas_id_t gas_id
)
{
  boolean bAllowed;

  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  bAllowed = rr_get_nv_geran_feature_pack_1_enabled(as_id);

  if ((bAllowed == TRUE) && (rat_type != NACC_GSM_GSM))
  {
    uint8 irat_support = rr_get_nv_interrat_nacc_support(as_id);

    if (rat_type == NACC_WCDMA_GSM)
    {
      bAllowed = (boolean)((irat_support & WTOG_NACC_SUPPORTED) == WTOG_NACC_SUPPORTED);
    }
    else
    {
      bAllowed = (boolean)((irat_support & GTOW_NACC_SUPPORTED) == GTOW_NACC_SUPPORTED);
    }
  }

  return(bAllowed);
}

/****************************************************************************
 *   Function name: rr_gprs_nacc_initialise
 *   -------------
 *
 *   Description:
 *   ------------
 *   Performs per-RAT initialisation for NACC
 *
 *   Parameters:
 *   ----------
 *   rat_type - indicates either G->G or G->W NACC
 *
 *   Returns:
 *   --------
 *   Void
 *
 ****************************************************************************/
void rr_gprs_nacc_initialise(
  rr_l1_handover_type_T rat_type,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_resel_nacc_data_ptr);

  rr_resel_nacc_data_ptr->nacc_RAT_type = rat_type;

  return;
}


boolean rr_gprs_nacc_container_empty(
  uint8 container_id,
  const gas_id_t gas_id
)
{
  container_info_t info = rr_gprs_nacc_container_info(container_id, gas_id);
  if (info == CONTAINER_EMPTY)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */


/****************************************************************************
 *   Function name: rr_gprs_is_nacc_active
 *   -------------
 *
 *   Description:
 *   ------------
 *
 *
 *   Parameters:
 *   ----------
 *
 *
 *   Returns:
 *   --------
 *   TRUE if NACC procedure is not in the inactive state
 *
 ****************************************************************************/
boolean rr_gprs_is_nacc_active(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_resel_nacc_data_t *rr_resel_nacc_data_ptr = rr_resel_nacc_get_data_ptr(gas_id);

  if (rr_resel_nacc_data_ptr != NULL)
  {
    if (rr_resel_nacc_data_ptr->nacc_state != NACC_INACTIVE)
    {
      return (TRUE);
    }
  }

  return (FALSE);
}


/* EOF */

