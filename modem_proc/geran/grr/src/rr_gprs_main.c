/*****************************************************************************
***
*** TITLE
***
***  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_main.c#1 $
***
*** DESCRIPTION
***
***  Equivalent module to rr_main.c for GPRS signal handling
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h"  /* Contains FEATURE_... defines */
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_gprs_main.h"
#include "rr_general.h"
#include "rr_public_store.h"
#include "mac_grr.h"
#include "csn.h"
#include "rr_sys_info.h"
#include "rr_resel_nacc.h"
#ifdef FEATURE_GSM_EDTM
#include "rr_gprs_edtm.h"
#endif /* FEATURE_GSM_EDTM */
#include "rr_sys_info_si.h"
#include "rr_sys_info_pacch.h"
#include "rr_ps_access.h"
#include "rr_mac_send.h"
#include "gprs_mem.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"
#include "rr_mm_send.h"
#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */

/*****************************************************************************
***
***     External Variables & Functions
***
*****************************************************************************/


/*****************************************************************************
***
***     Local Typedefs, #defines & Variables
***
*****************************************************************************/

typedef struct
{
  rr_internal_band_T gprs_test_mode_band;
  rr_mac_status_t    mac_status;
  boolean            l1_in_test_mode;
} rr_gprs_main_data_t;

static rr_gprs_main_data_t rr_gprs_main_data[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Local Function Prototypes
***
*****************************************************************************/

static rr_event_T rr_gprs_message_from_mac(rr_cmd_bdy_type * message, const gas_id_t gas_id);
static rr_event_T rr_gprs_message_from_l1(rr_cmd_bdy_type * message, const gas_id_t gas_id);

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_gprs_main_data_t*
 */
static rr_gprs_main_data_t *rr_gprs_main_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_gprs_main_data[as_index];
}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================

FUNCTION rr_gprs_process_gprs_messages

DESCRIPTION
  Processes GPRS messages passed in from rr_main.c
  Returns TRUE if message processed; FALSE otherwise

PARAMS
  None

RETURN VALUE
  rr_event_T - event to be passed into rr_control()

===========================================================================*/

rr_event_T rr_gprs_process_gprs_messages(rr_cmd_bdy_type * message, const gas_id_t gas_id)
{
  rr_event_T  rr_event;

  switch ( message->message_header.message_set )
  {
    case MS_MAC_RR:
      rr_event = rr_gprs_message_from_mac(message, gas_id);
      break;

    case MS_RR_L1:
      rr_event = rr_gprs_message_from_l1(message, gas_id);
      break;

    default:
      rr_event = EV_INPUT_MESSAGE;
      break;
  }

  return rr_event;

} /* end rr_gprs_process_gprs_messages() */



/*===========================================================================

FUNCTION rr_macinfo_get_mac_status

DESCRIPTION
  This returns the last MAC status received in a MAC_GRR_MAC_STATUS_IND message

PARAMS
  None

RETURN VALUE
  rr_mac_status_t - { MAC_IDLE; MAC_IDLE_BLOCKED; MAC_TRANSFER; MAC_TRANSFER_BLOCKED }

===========================================================================*/

rr_mac_status_t rr_macinfo_get_mac_status(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_main_data_t *rr_gprs_main_data_ptr = rr_gprs_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_main_data_ptr);

  return rr_gprs_main_data_ptr->mac_status;

} /* end rr_macinfo_get_mac_status() */


/*****************************************************************************
***
***     Local Functions
***
*****************************************************************************/

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)

/*===========================================================================

FUNCTION rr_gprs_process_pkt_cs_command

DESCRIPTION
  Converts the contents of a PACKET CS COMMAND into a DL_DATA_IND primitive,
  and then passes that message into the main RR state machine.

PARAMS
  mac_data_ind_ptr : pointer to decoded Pkt CS Command from GMAC

RETURN VALUE
  Void

===========================================================================*/
static void rr_gprs_process_pkt_cs_command(
  mac_grr_data_ind_t *mac_data_ind_ptr,
  const gas_id_t gas_id
)
{
  packet_cs_command_t *pcsc_ptr;
  dl_data_ind_T       *cmd_data_ptr;
  rr_cmd_bdy_type     *rr_cmd_ptr;

  /* cast pointer to input structure */
  pcsc_ptr = (packet_cs_command_t *)mac_data_ind_ptr->data_ptr.data_ptr;
  if ((pcsc_ptr->container_length == 0) ||
      (pcsc_ptr->container_length >  MAX_OCTETS_L3_MESSAGE))
  {
    MSG_GERAN_ERROR_1_G("Container data in Pkt CS Command has incorrect length(%d)",
              (int)pcsc_ptr->container_length);
    return;
  }

  rr_cmd_ptr = GPRS_MEM_MALLOC(sizeof(rr_cmd_bdy_type));

  if (rr_cmd_ptr != NULL)
  {
    /* cast pointer to output structure */
    cmd_data_ptr = &(rr_cmd_ptr->dl_data_ind);

    /* wrap the incoming L3 message data in its own DL_DATA_IND */
    cmd_data_ptr->message_header.message_set = MS_RR_L2;
    cmd_data_ptr->message_header.message_id  = (byte)DL_DATA_IND;
    PUT_IMH_LEN(sizeof(dl_data_ind_T)-sizeof(IMH_T), &(cmd_data_ptr->message_header));

    /* set channel details accordingly */
    cmd_data_ptr->SAPI            = SAPI0;
    cmd_data_ptr->l2_channel_type = DCCH;

    /* sanity check the container length */
    if (pcsc_ptr->container_length <= (CSN_PKT_CS_COMMAND_MAX_OCTETS+1))
    {
      /* extract the containerised Layer 3 data */
	  //Replace memcpy with memspcy
      memscpy(cmd_data_ptr->layer3_message,
	  	      sizeof(cmd_data_ptr->layer3_message),
              pcsc_ptr->container_data,
              pcsc_ptr->container_length);


      cmd_data_ptr->l3_message_length = pcsc_ptr->container_length;

      /* and then pass the resulting input message into RR */
      rr_control(EV_INPUT_MESSAGE, rr_cmd_ptr, gas_id);
    }
    else
    {
      MSG_GERAN_ERROR_1_G("PCSC container length %d is too long",
                (int)pcsc_ptr->container_length);
    }

    GPRS_MEM_FREE(rr_cmd_ptr);
  }

  return;
}

#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */


/*===========================================================================

FUNCTION rr_gprs_message_from_mac

DESCRIPTION
  Processes GPRS messages passed in from rr_main.c
  Returns TRUE if message processed; FALSE otherwise

PARAMS
  rr_cmd_bdy_type * message - input message

RETURN VALUE
  boolean - TRUE if message processing; FALSE otherwise

===========================================================================*/

static rr_event_T rr_gprs_message_from_mac(
  rr_cmd_bdy_type * message,
  const gas_id_t gas_id
)
{
  rr_event_T rr_event = EV_NO_EVENT;
  rr_gprs_main_data_t *rr_gprs_main_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_main_data_ptr = rr_gprs_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_main_data_ptr);

  if ( message->message_header.message_id == (byte)MAC_GRR_DATA_IND )
  {
    rlc_mac_cntrl_t *header_ptr = message->mac_grr_sig.mac_grr_data_ind.data_ptr.header_ptr;

    switch (header_ptr->message_type)
    {
      case PACKET_PSI1:
      case PACKET_PSI2:
      case PACKET_PSI3:
      case PACKET_PSI3_BIS:
      case PACKET_PSI3_QUATER:
      case PACKET_PSI4:
      case PACKET_PSI5:
      {
        MSG_GERAN_HIGH_0_G("PSI message ignored");
        break;
      }

      case PACKET_PSI13:
      case PACKET_PSI14:
      case PACKET_PSI15:
        // Ignore all sys info messages if L1 is in GPRS Test Mode
        if (grr_get_l1_in_test_mode(gas_id) == FALSE)
        {
          rr_gprs_new_psi(
            header_ptr,
            message->mac_grr_sig.mac_grr_data_ind.data_ptr.data_ptr,
            message->mac_grr_sig.mac_grr_data_ind.bcch_arfcn,
            gas_id
          );
        }
        break;

      case PACKET_SI_DATA:
      case PACKET_SERVING_CELL_SI:
        // Ignore all sys info messages if L1 is in GPRS Test Mode
        if (grr_get_l1_in_test_mode(gas_id) == FALSE)
        {
          rr_gprs_process_sys_info_from_pacch(
            &(message->mac_grr_sig.mac_grr_data_ind),
            gas_id
          );
        }
        break;

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
      case PACKET_CS_COMMAND:
        rr_gprs_process_pkt_cs_command(
          &(message->mac_grr_sig.mac_grr_data_ind),
          gas_id
        );
        break;
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

      default:
        /* MAC_GRR_DATA_IND messages are passed into rr_control() here, because */
        /* the dynamic memory has to be freed after the message is processed    */
        rr_control(EV_INPUT_MESSAGE, message, gas_id);
        break;
    }

    /* Free up the dynamic memory used by the CSN1 decoder */
    free_csn_decoder_ptrs_t(&message->mac_grr_sig.mac_grr_data_ind.data_ptr);
  }
  else
  if ( message->message_header.message_id == (byte)MAC_GRR_STORE_UPDATE )
  {
    rr_process_mac_grr_store_update(
      &message->mac_grr_sig.mac_grr_store_update,
      gas_id
    );
  }
  else
  if ( message->message_header.message_id == (byte)MAC_GRR_MAC_STATUS )
  {
    rr_mac_status_t  mac_status = message->mac_grr_sig.mac_grr_mac_status.mac_status;

    if (mac_status == MAC_IDLE)
    {
      grr_stop_guard_timer(gas_id);

      rr_hz_notify_ps_status(FALSE, gas_id);

      /* If MAC has moved from a non-Idle start to Idle, then GPRS activity has come */
      /* (maybe temporarily) to an end */
      if (rr_gprs_main_data_ptr->mac_status != MAC_IDLE)
      {
        /* Don't start the GPRS Recent Activity Timer if MAC is Idle due to PS access being blocked */
        if (rr_mac_ps_access_granted(gas_id))
        {
          grr_start_gprs_recent_activity_timer(gas_id);
        }
#ifdef FEATURE_GERAN_BSR_PS_STATUS
        else
        {
          rr_mm_send_rr_ps_status_change_ind(
            FALSE,    // ps_is_active
            gas_id    // gas_id
          );
        }
#endif /* FEATURE_GERAN_BSR_PS_STATUS */

#ifdef FEATURE_IDLE_DRX_SCALING
        rr_idle_drx_control(EV_MAC_IDLE_MODE, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */
      }
    }
    else
    {
      boolean ps_active = (boolean)((mac_status == MAC_TRANSFER) || (mac_status == MAC_TRANSFER_BLOCKED));

      rr_hz_notify_ps_status(ps_active, gas_id);

#ifdef FEATURE_GERAN_BSR_PS_STATUS
      if (ps_active)
      {
        // Update NAS with 'PS active' indication
        rr_mm_send_rr_ps_status_change_ind(
          TRUE,    // ps_is_active
          gas_id   // gas_id
        );

        // Note: 'PS idle' is indicated only when the GPRS_RECENT_ACTIVITY_TIMER expires
      }
#endif // FEATURE_GERAN_BSR_PS_STATUS
    }

    /* RESET dl_msg_sent  */
    /* Note: dl_msg_sent SET when DL TBF or SB assignment is sent, to avoid processing MPLMN(if any)from NAS */
    rr_mac_send_set_dl_msg_sent(FALSE,gas_id);

    /*If no longer in packet transfer reset si status timers*/
    rr_gprs_reset_si_status_timers(
      message->mac_grr_sig.mac_grr_mac_status.mac_status,
      gas_id
    );

    /*if MS in going into transfer and full set of SI's not rcvd use SI status procedure to acquire missing SI's*/
    rr_gprs_send_si_status_during_si_acquisition(
      message->mac_grr_sig.mac_grr_mac_status.mac_status,
      gas_id
    );

    /* Store local copy of MAC status, and then pass on to rr_control */
    rr_gprs_main_data_ptr->mac_status = mac_status;

    rr_gprs_nacc_notify_mac_status_changed(
      mac_status,
      gas_id
    );

    rr_event = EV_INPUT_MESSAGE;
  }
  else if ( message->message_header.message_id == (byte)MAC_GRR_NCELL_DATA_IND )
  {
    /* incoming NCELL data from MAC */
    rr_gprs_nacc_store_ncell_data(
      &(message->mac_grr_sig.mac_grr_ncell_data_ind),
      gas_id
    );
  }
  else if ( message->message_header.message_id == (byte)MAC_GRR_NCELL_DATA_CNF )
  {
    rr_event = rr_gprs_nacc_handle_ncell_data_cnf(message, gas_id);
  }
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
  else if ( message->message_header.message_id == (byte)MAC_GRR_EDTM_PKT_CS_EST_REJ )
  {
    mac_grr_edtm_pkt_cs_est_rej_t *pkt_cs_est_rej_ptr;

    pkt_cs_est_rej_ptr = &(message->mac_grr_sig.mac_grr_edtm_pkt_cs_est_rej);

    rr_event = rr_gprs_edtm_process_pkt_cs_est_reject(
      pkt_cs_est_rej_ptr,
      gas_id
    );
  }
  else if ( message->message_header.message_id == (byte)MAC_GRR_EDTM_CS_RELEASE_IND )
  {
    mac_grr_edtm_cs_release_ind_t *pkt_cs_release_ptr;

    pkt_cs_release_ptr = &(message->mac_grr_sig.mac_grr_edtm_cs_release_ind);

    rr_event = rr_gprs_edtm_process_pkt_cs_release(
      pkt_cs_release_ptr,
      gas_id
    );
  }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
  else if (message->message_header.message_id == (byte)MAC_GRR_PS_ACCESS_CNF)
  {
    grr_stop_guard_timer(gas_id);
    rr_event = EV_INPUT_MESSAGE;
  }
  else
  {
    /* All other messages from MAC pass to rr_control */
    rr_event = EV_INPUT_MESSAGE;
  }

  return rr_event;

} /* end rr_gprs_message_from_mac() */


/*===========================================================================

FUNCTION rr_gprs_message_from_l1

DESCRIPTION
  Processes GPRS messages passed in from rr_main.c
  Returns TRUE if message processed; FALSE otherwise

PARAMS
  rr_cmd_bdy_type * message - input message

RETURN VALUE
  boolean - TRUE if message processed; FALSE otherwise

===========================================================================*/

static rr_event_T rr_gprs_message_from_l1(rr_cmd_bdy_type * message, const gas_id_t gas_id)
{
  rr_event_T  rr_event = EV_NO_EVENT;
  rr_gprs_main_data_t *rr_gprs_main_data_ptr;

  // Obtain a pointer to the module data
  rr_gprs_main_data_ptr = rr_gprs_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_main_data_ptr);

  switch ( message->message_header.message_id )
  {
    /********************************************************************/
    /* MPH_READ_SYS_INFO_FAILURE_IND                                    */
    /********************************************************************/
    case MPH_READ_SYS_INFO_FAILURE_IND:

      rr_event = rr_gprs_bsi_acquisition_control(
        EV_INPUT_MESSAGE,
        (void *)message,
        gas_id
      );
      break;

    case MPH_L1_TEST_MODE_START_IND:
      rr_gprs_main_data_ptr->l1_in_test_mode = TRUE;
      grr_set_test_mode_band(rr_gapi_get_current_band(gas_id), gas_id);
      rr_gprs_deactivate_timed_sys_info_refresh(gas_id);
      break;

    case MPH_L1_TEST_MODE_STOP_IND:
      rr_gprs_main_data_ptr->l1_in_test_mode = FALSE;
      grr_set_test_mode_band(BAND_NONE, gas_id);
      rr_gprs_activate_camped_sys_info_handling(gas_id);
      break;

#ifdef FEATURE_GSM_DTM
    case MPH_DTM_CHANNEL_ASSIGN_CNF:
      /* notify the DTM module, so that it can track system info */
      rr_dtm_dual_transfer_confirmed(rr_gprs_main_data_ptr->mac_status, gas_id);
      rr_event = EV_INPUT_MESSAGE;
      break;

#ifdef FEATURE_GSM_EDTM
    case MPH_DTM_CHANNEL_RELEASE_IND:
      /* preprocess the DTM release message in the EDTM module */
      rr_gprs_edtm_cache_dtm_release(message, gas_id);
      rr_event = EV_INPUT_MESSAGE;
      break;
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

    default:
      rr_event = EV_INPUT_MESSAGE;
      break;
  }

  return rr_event;

} /* end rr_gprs_message_from_l1() */


/*===========================================================================

FUNCTION grr_get_l1_in_test_mode

DESCRIPTION
  Return TRUE if L1 is in test mode; FALSE otherwise

PARAMS
  None

RETURN VALUE
  boolean

===========================================================================*/

boolean grr_get_l1_in_test_mode(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_main_data_t *rr_gprs_main_data_ptr = rr_gprs_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_main_data_ptr);

  return rr_gprs_main_data_ptr->l1_in_test_mode;

} /* end grr_get_l1_in_test_mode() */


/*===========================================================================

FUNCTION grr_set_test_mode_band

DESCRIPTION
  Sets the current test mode band variable

PARAMS
  curr_band - one of {BAND_EGSM, BAND_PGSM, BAND_PCS, BAND_DCS, BAND_GSM850, BAND_NONE}

RETURN VALUE
  VOID

===========================================================================*/
void grr_set_test_mode_band(rr_internal_band_T curr_band, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_main_data_t *rr_gprs_main_data_ptr = rr_gprs_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_main_data_ptr);

  rr_gprs_main_data_ptr->gprs_test_mode_band = curr_band;

  return;
}


/*===========================================================================

FUNCTION grr_get_test_mode_band

DESCRIPTION
  Return the current test mode band variable

PARAMS
  None

RETURN VALUE
  Current test mode band

===========================================================================*/
rr_internal_band_T grr_get_test_mode_band(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_main_data_t *rr_gprs_main_data_ptr = rr_gprs_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_main_data_ptr);

  return(rr_gprs_main_data_ptr->gprs_test_mode_band);
}


/**
  @brief Initialises this module's local data structures.

  This function should be called at power on, and after a panic reset.
*/
void rr_gprs_main_init(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_gprs_main_data_t *rr_gprs_main_data_ptr = rr_gprs_main_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_gprs_main_data_ptr);

  rr_gprs_main_data_ptr->gprs_test_mode_band = BAND_NONE;
  rr_gprs_main_data_ptr->mac_status = MAC_IDLE;
  rr_gprs_main_data_ptr->l1_in_test_mode = FALSE;

} /* end rr_gprs_main_init() */


/**
  @brief Initialisation function. Should be called once at RR task start

  Note: The primary purpose of this function is to initialise pointers to control/data structures.
*/
void rr_gprs_main_task_start_init(void)
{
  // Zero the control/data structure(s)
  memset(rr_gprs_main_data, 0, sizeof(rr_gprs_main_data));

  // Setup any specific values not setup by the general module initialisation function

} /* end rr_gprs_main_task_start_init() */

/* EOF */

