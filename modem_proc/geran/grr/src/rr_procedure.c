/*! \file rr_procedure.c

  This module contains functionality related to co-ordination of dedicated procedures.

                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_procedure.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_gprs_defs.h"
#include "rr_gprs_debug.h"
#include "rr_procedure.h"
#include "ms.h"
#include "rr_inter_ho.h"
#include "rr_intra_ho.h"
#include "rr_channel_mode.h"
#include "rr_cipher.h"
#include "rr_general.h"
#include "rr_classmark.h"
#include "rr_freq_redef.h"
#include "rr_conn_release.h"
#include "msg.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rr_sapi3.h"
#include "rr_g2w_handover.h"
#include "rr_gprs_defs.h"
#include "rr_l2_send.h"
#include "rr_log.h"
#include "rr_conn_establish.h"
#include "rr_ded_config.h"
#include "rr_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  DISP_INACTIVE_STATE,
  INTRA_CELL_HANDOVER_STATE,
  INTER_CELL_HANDOVER_STATE,
  CIPHERING_STATE,
#ifdef FEATURE_GSM_DTM
  DTM_ASSIGNMENT_STATE
#endif /* FEATURE_GSM_DTM */
} dispatch_state_T;

typedef struct
{
  dispatch_state_T dispatch_state;
  dispatch_state_T old_dispatch_state;
} rr_procedure_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

#define _RR_PROC_DATA_INIT { \
                            DISP_INACTIVE_STATE, \
                            DISP_INACTIVE_STATE \
                          }

static rr_procedure_data_t rr_procedure_data[NUM_GERAN_DATA_SPACES] =
                                 { INITIAL_VALUE(_RR_PROC_DATA_INIT) };
#undef _RR_PROC_DATA_INIT

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Return a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_procedure_data_t*
 */
rr_procedure_data_t *rr_procedure_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_procedure_data[as_index];
}

/*!
 * \brief Returns a string representation of the dispatch state name.
 *
 * \param dispatch_state_to_log
 *
 * \return char*
 */
static char *rr_dispatch_state_name(
  dispatch_state_T dispatch_state
)
{
  char *s;

  switch (dispatch_state)
  {
    case DISP_INACTIVE_STATE:
      s = "Inactive";
      break;

    case INTRA_CELL_HANDOVER_STATE:
      s = "IntraHO";
      break;

    case INTER_CELL_HANDOVER_STATE:
      s = "InterHO";
      break;

    case CIPHERING_STATE:
      s = "Ciphering";
      break;

#ifdef FEATURE_GSM_DTM
    case DTM_ASSIGNMENT_STATE:
      s = "DTMAssignment";
      break;
#endif /* FEATURE_GSM_DTM */

    default:
      s = "Unknown";
      break;
  }

  return s;
}

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

/*!
 * \brief Checks whether there is an RR procedure currently ongoing or not.
 *
 * \param gas_id
 *
 * \return boolean - TRUE if a procedure is ongoing; FALSE otherwise
 */
boolean rr_ded_proc_in_progress(
  const gas_id_t gas_id
)
{
  rr_procedure_data_t *rr_procedure_data_ptr = rr_procedure_get_data_ptr(gas_id);

  if (rr_procedure_data_ptr != NULL)
  {
    if ((rr_procedure_data_ptr->dispatch_state != DISP_INACTIVE_STATE) &&
        (rr_procedure_data_ptr->dispatch_state != DTM_ASSIGNMENT_STATE))
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*!
 * \brief Top-level controller for dedicated procedures.
 *
 * \param proc_dispatch_event
 * \param new_message
 * \param gas_id
 *
 * \return rr_event_T
 */
rr_event_T rr_handle_procedures(
  rr_event_T proc_dispatch_event,
  void *new_message,
  const gas_id_t gas_id
)
{
  rr_event_T new_proc_dispatch_event = EV_NO_EVENT;
  rr_event_T rr_event = EV_NO_EVENT;
  byte message_type;
  IMH_T *message_header=NULL;
  dl_data_ind_T *dl_data_ind;
  dl_unit_data_ind_T *dl_unit_data_ind;
  SAPI_T SAPI;
  rr_procedure_data_t *rr_procedure_data_ptr;

  // Obtain a pointer to the module data
  rr_procedure_data_ptr = rr_procedure_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_procedure_data_ptr);

  if ( proc_dispatch_event == EV_RESET_SOFTWARE )
  {
    rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;
    rr_procedure_data_ptr->old_dispatch_state = DISP_INACTIVE_STATE;
    proc_dispatch_event = EV_NO_EVENT;

    /* reset state machines that depend on the RR procedures */
    (void) rr_cipher_control(EV_RESET_SOFTWARE, NULL, gas_id);
  }
  else if ((proc_dispatch_event == EV_INPUT_MESSAGE) && (new_message != NULL))
  {
    message_header = (IMH_T *) new_message;

    if ((message_header->message_set == MS_RR_L2) &&
        (message_header->message_id == (byte) DL_DATA_IND))
    {
      dl_data_ind = (dl_data_ind_T *) new_message;
      message_type = dl_data_ind->layer3_message[1];

      if (message_type == HANDOVER_COMMAND)
      {
        proc_dispatch_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }

      else if (message_type == ASSIGNMENT_COMMAND)
      {
        proc_dispatch_event = rr_intra_cell_handover_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }

      else if (message_type == ADDITIONAL_ASSIGNMENT)
      {
        /* this OTA message is not supported */
        MSG_GERAN_ERROR_0_G("No support for additional assignment");
        proc_dispatch_event = EV_NO_EVENT;
      }

      else if (message_type == CHANNEL_MODE_MODIFY)
      {
        /* handle the OTA message in a stateless manner */
        rr_handle_channel_mode_modify_msg(gas_id);
        proc_dispatch_event = EV_NO_EVENT;
      }

      else if (message_type == CIPHERING_MODE_COMMAND)
      {
        proc_dispatch_event = rr_cipher_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }

      else if (message_type == CLASSMARK_ENQUIRY)
      {
        rr_perform_classmark_sending(RR_CLASSMARK_CLIENT_NETW, gas_id);
        proc_dispatch_event = EV_NO_EVENT;
      }

      else if (message_type == FREQUENCY_REDEFINITION)
      {
        proc_dispatch_event = rr_handle_frequency_redefinition_msg(gas_id);
      }

#ifdef FEATURE_WCDMA
      else if (message_type == INTERSYSTEM_TO_UTRAN_HANDOVER_COMMAND)
      {
        proc_dispatch_event = rr_g2w_handover_control(RR_EV_G2W_HO_STARTED, new_message, gas_id);
      }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_DTM
      else if ( (message_type == DTM_ASSIGNMENT_COMMAND) ||
                (message_type == PACKET_ASSIGNMENT) ||
                (message_type == DTM_REJECT) ||
                (message_type == DTM_INFORMATION) ||
                (message_type == GPRS_INFORMATION) ||
                (message_type == PACKET_NOTIFICATION) )
      {
        proc_dispatch_event = rr_dtm_control(EV_INPUT_MESSAGE, new_message, gas_id);
      }
#endif /* FEATURE_GSM_DTM */
    }

    else if ((message_header->message_set == MS_RR_L2) &&
             (message_header->message_id == (byte) DL_UNIT_DATA_IND))
    {
      dl_unit_data_ind = (dl_unit_data_ind_T *) new_message;
      message_type = dl_unit_data_ind->layer3_message[1];

      switch (message_type)
      {
        case PHYSICAL_INFORMATION:
        {
          proc_dispatch_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message, gas_id);
          break;
        }

#ifdef FEATURE_GSM_DTM
        case PACKET_NOTIFICATION:
        {
          proc_dispatch_event = rr_dtm_control(EV_INPUT_MESSAGE, new_message, gas_id);
          break;
        }
#endif /* FEATURE_GSM_DTM */

        default: ;
      }
    }

    else if ((message_header->message_set == MS_RR_L1) &&
             (message_header->message_id  == (byte) MPH_FREQUENCY_REDEFINITION_IND))
    {
      proc_dispatch_event = rr_handle_frequency_redefinition_ind(new_message, gas_id);
    }

    else if ((message_header->message_set == MS_RR_L1) &&
             (message_header->message_id == (byte) MPH_CHANNEL_MODE_MODIFY_CNF))
    {
      rr_handle_channel_mode_modify_cnf(new_message, gas_id);
      proc_dispatch_event = EV_NO_EVENT;
    }

    else if (((message_header->message_set == MS_RR_L2) &&
              ((message_header->message_id == (byte) DL_SUSPEND_CNF)         ||
               (message_header->message_id == (byte) DL_RESUME_CNF)          ||
               (message_header->message_id == (byte) DL_RECONNECT_CNF)       ||
               (message_header->message_id == (byte) DL_RELEASE_IND)         ||
               (message_header->message_id == (byte) DL_RANDOM_ACCESS_CNF))) ||
             ((message_header->message_set == MS_RR_L1) &&
              ((message_header->message_id == (byte) MPH_CHANNEL_ASSIGN_CNF) ||
               (message_header->message_id == (byte) MPH_HANDOVER_IND) ||
               (message_header->message_id == (byte) MPH_HANDOVER_CNF) ||
               (message_header->message_id == (byte) MPH_STOP_HANDOVER_ACCESS_CNF) ||
               (message_header->message_id == (byte) MPH_SET_CIPHERING_CNF) ||
               (message_header->message_id == (byte) MPH_RECONNECT_CHANNEL_CNF))))
    {
      proc_dispatch_event = EV_NEW_MESSAGE;
    }

    else if ((message_header->message_set == MS_RR_L2) && (message_header->message_id == (byte) MDL_ERROR_IND))
    {
      error_code_T error_code = ((mdl_error_ind_T *)new_message)->error_code;

      /*
       * if RR is not in a procedure, and receives MDL_ERROR_IND,
       * ignore it, unless the error code is specifically requesting
       * abnormal release of the local link, in which case it will be
       * treated as a Radio Link Failure (RLF).
       */
      if ((error_code == UNSOLICITED_DM_RESPONSE_PAR) ||
          (error_code == N200_EXCEEDED_PAR)           ||
          (error_code == SEQUENCE_ERROR))
      {
        MSG_GERAN_HIGH_1_G( "L2 Radio Link Failure due to error %d",(int)error_code);

        /* allow the message to go through */
        proc_dispatch_event = EV_NEW_MESSAGE;
      }
      else
      {
        /* message should be ignored */
        proc_dispatch_event = EV_NO_EVENT;
        MSG_GERAN_HIGH_1_G( "Ignoring MDL_ERROR_IND with error_code %d",(int)error_code);
      }
    }

    else if (message_header->message_set == MS_GHDI_EXPORT)
    {
      if (message_header->message_id == GHDI_CLASSMARK_CHANGED_IND)
      {
        rr_perform_classmark_sending(RR_CLASSMARK_CLIENT_GHDI, gas_id);
        proc_dispatch_event = EV_NO_EVENT;
      }
    }

#ifdef FEATURE_GSM_DTM
    else if ( (message_header->message_set == MS_RR_L1) &&
              ( (message_header->message_id == (byte) MPH_DTM_CHANNEL_ASSIGN_CNF) ||
                (message_header->message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND) ) )
    {
      proc_dispatch_event = EV_NEW_MESSAGE;
    }
    else if ( message_header->message_set == MS_MAC_RR )
    {
      proc_dispatch_event = rr_dtm_control(EV_INPUT_MESSAGE, new_message, gas_id);
    }
    else if ( (message_header->message_set == MS_MM_RR ) &&
              (message_header->message_id == (byte) RR_GMM_GPRS_SERVICE_CNF) )
    {
      proc_dispatch_event = EV_NEW_MESSAGE;
    }
#endif /* FEATURE_GSM_DTM */
  }

  while (proc_dispatch_event != EV_NO_EVENT)
  {
    switch (rr_procedure_data_ptr->dispatch_state)
    {
      case DISP_INACTIVE_STATE :
      {
        if (proc_dispatch_event == EV_INTER_CELL_HANDOVER)
        {
          #ifdef FEATURE_GSM_DTM
          // Assume initially that the cell won't change, so treat this as an intra-cell HO.
          // Once the new cell details are checked, if the HANDOVER COMMAND specifies a
          // different cell, rr_inter_cell_handover_control will send another
          // EV_INTER_CELL_HANDOVER event
          (void) rr_dtm_control(EV_INTRA_CELL_HANDOVER, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          rr_procedure_data_ptr->dispatch_state = INTER_CELL_HANDOVER_STATE;
        }

        else if (proc_dispatch_event == EV_G2W_HANDOVER)
        {
          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_INTRA_CELL_HANDOVER, NULL, gas_id);
          (void) rr_dtm_control(EV_G2W_HANDOVER, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          rr_procedure_data_ptr->dispatch_state = INTER_CELL_HANDOVER_STATE;
        }

        else if (proc_dispatch_event == EV_INTRA_CELL_HANDOVER)
        {
          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_INTRA_CELL_HANDOVER, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          rr_procedure_data_ptr->dispatch_state = INTRA_CELL_HANDOVER_STATE;
        }

        #ifdef FEATURE_GSM_DTM
        else if (proc_dispatch_event == EV_DTM_SI6_RECEIVED)
        {
          (void) rr_dtm_control(proc_dispatch_event, NULL, gas_id);
        }
        #endif /* FEATURE_GSM_DTM */

        else if (proc_dispatch_event == EV_CIPHERING)
        {
          rr_procedure_data_ptr->dispatch_state = CIPHERING_STATE;
        }

        else if (proc_dispatch_event == EV_CONNECTION_RELEASED)
        {
          rr_event = EV_CONNECTION_RELEASED;
        }

        else if (proc_dispatch_event == EV_RADIO_LINK_FAILURE)
        {
          rr_event = EV_RADIO_LINK_FAILURE;
        }

        #ifdef FEATURE_GSM_DTM
        else if (proc_dispatch_event == EV_DTM_ASSIGNMENT_RECEIVED )
        {
          rr_procedure_data_ptr->dispatch_state = DTM_ASSIGNMENT_STATE;
        }
        #endif /* FEATURE_GSM_DTM */

        else if (proc_dispatch_event == EV_NEW_MESSAGE)
        {
          /*
           * if RR is not in a procedure, and receives MDL_ERROR_IND,
           * ignore it, unless the error code is specifically requesting
           * abnormal release of the local link, in which case it will be
           * treated as a Radio Link Failure (RLF).
           */
          if ( (message_header != NULL) &&
               (message_header->message_set == MS_RR_L2) &&
               (message_header->message_id == (byte) MDL_ERROR_IND)
             )
          {
            if (
                (new_message != NULL) &&
                (
                 ((mdl_error_ind_T *)new_message)->error_code == UNSOLICITED_DM_RESPONSE_PAR ||
                 ((mdl_error_ind_T *)new_message)->error_code == N200_EXCEEDED_PAR           ||
                 ((mdl_error_ind_T *)new_message)->error_code == SEQUENCE_ERROR
                )
               )
            {
              SAPI = ((mdl_error_ind_T *)new_message)->SAPI;
              if (SAPI == SAPI3)
              {
                rr_event = rr_control_SAPI3_link(new_message, gas_id);
              }
              else
              {
                rr_event = EV_RADIO_LINK_FAILURE;
              }
            }
            else
            {
              rr_event = EV_NO_EVENT;

              if (new_message != NULL)
              {
                MSG_GERAN_HIGH_1_G( "Ignoring MDL_ERROR_IND with error_code %d",((mdl_error_ind_T *)new_message)->error_code);
              }
            }
          }
          else if ((message_header != NULL ) &&
                   (message_header->message_set == MS_RR_L2) &&
                   (message_header->message_id == (byte) DL_RELEASE_IND))
          {

            SAPI = ((dl_release_ind_T *)new_message)->SAPI;

            if (SAPI == SAPI3)
            {
              (void) rr_control_SAPI3_link(new_message, gas_id);
            }
            else
            {
              (void) rr_log_gsm_call_drop(
                RR_LOG_CALL_DROPPED_DUE_TO_L2_LINK_FAILURE,
                rr_ce_access_reason(gas_id),
                gas_id
              );

              rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message, gas_id);
            }
          }

          #ifdef FEATURE_GSM_DTM
          else if ((message_header != NULL ) &&
                   (message_header->message_set == MS_RR_L1) &&
                   (message_header->message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND))
          {
            (void) rr_dtm_control(EV_INPUT_MESSAGE, new_message, gas_id);
          }
          #endif /* FEATURE_GSM_DTM */
        }

        break;
      }

      case INTRA_CELL_HANDOVER_STATE :
      {
        if (proc_dispatch_event == EV_NEW_MESSAGE)
        {
          new_proc_dispatch_event = rr_intra_cell_handover_control(EV_INPUT_MESSAGE, new_message, gas_id);
        }

        #ifdef FEATURE_GSM_DTM
        else if (proc_dispatch_event == EV_DTM_SI6_RECEIVED)
        {
          ;  /* Ignore SI6 reception until handover completed & L2 resumed */
        }
        #endif /* FEATURE_GSM_DTM */

        else if (proc_dispatch_event == EV_HANDOVER_TERMINATED)
        {
          rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_HANDOVER_COMPLETED, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          rr_event = EV_PROCEDURE_COMPLETE;
        }
        else if (proc_dispatch_event == EV_RADIO_LINK_FAILURE)
        {
          rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;
          rr_event = EV_RADIO_LINK_FAILURE;
        }
        break;
      }

      case INTER_CELL_HANDOVER_STATE :
      {
        if (proc_dispatch_event == EV_NEW_MESSAGE)
        {
          new_proc_dispatch_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message, gas_id);
        }

        else if (proc_dispatch_event == EV_T3124_TIMEOUT)
        {
          new_proc_dispatch_event = rr_inter_cell_handover_control(EV_T3124_TIMEOUT, NULL, gas_id);
        }

        #ifdef FEATURE_GSM_DTM
        else if (proc_dispatch_event == EV_INTER_CELL_HANDOVER)
        {
          // Update RR-DTM module that the cell has changed during this HO
          (void) rr_dtm_control(EV_INTER_CELL_HANDOVER, NULL, gas_id);
        }
        else if (proc_dispatch_event == EV_DTM_SI6_RECEIVED)
        {
          ;  /* Ignore SI6 reception until handover completed & L2 resumed */
        }
        #endif /* FEATURE_GSM_DTM */

        else if (proc_dispatch_event == EV_HANDOVER_TERMINATED)
        {
          rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;
          (void) rr_inter_cell_handover_control(EV_RESET_SOFTWARE, NULL, gas_id);
          rr_event = EV_PROCEDURE_COMPLETE;

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_HANDOVER_COMPLETED, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */
        }

        else if (proc_dispatch_event == EV_RADIO_LINK_FAILURE)
        {
          rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;
          (void) rr_inter_cell_handover_control(EV_RESET_SOFTWARE, NULL, gas_id);
          rr_event = EV_RADIO_LINK_FAILURE;
        }

        else if (proc_dispatch_event == RR_EV_G2W_HO_FAIL)
        {
          new_proc_dispatch_event = rr_inter_cell_handover_control(proc_dispatch_event, NULL, gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(RR_EV_G2W_HO_FAIL, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */
        }

        else if (proc_dispatch_event == RR_EV_G2W_HO_SUCCESS)
        {
          rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;
          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(RR_EV_G2W_HO_SUCCESS, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */
        }
        /* handle_procedure has to be  cleared in case of G2W abortion refer to CR554022*/
        else if (proc_dispatch_event == RR_EV_G2W_HO_ABORTED)
        {
          rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;
        }

        break;
      }

      case CIPHERING_STATE :
      {
        if (proc_dispatch_event == EV_NEW_MESSAGE)
        {
          #ifdef FEATURE_GSM_DTM
          if ((message_header != NULL) &&
              (message_header->message_set == MS_RR_L1) &&
              (message_header->message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND))
          {
            (void) rr_dtm_control(EV_INPUT_MESSAGE, new_message, gas_id);
          }
          else
          #endif /* FEATURE_GSM_DTM */
          new_proc_dispatch_event = rr_cipher_control(EV_INPUT_MESSAGE, new_message, gas_id);
        }

        #ifdef FEATURE_GSM_DTM
        else if (proc_dispatch_event == EV_DTM_SI6_RECEIVED)
        {
          ;  /* Ignore SI6 reception until procedure completed */
        }
        #endif /* FEATURE_GSM_DTM */

        else if (proc_dispatch_event == EV_CIPHERING_TERMINATED)
        {
          rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;
          rr_event = EV_PROCEDURE_COMPLETE;
        }

        break;
      }

      #ifdef FEATURE_GSM_DTM
      case DTM_ASSIGNMENT_STATE:
      {
        if (proc_dispatch_event == EV_NEW_MESSAGE)
        {
          new_proc_dispatch_event = rr_dtm_control(EV_INPUT_MESSAGE, new_message, gas_id);
        }

        else if (proc_dispatch_event == EV_DTM_SI6_RECEIVED)
        {
          ;  /* Ignore SI6 reception until assignment completed & L2 resumed */
        }

        else if (proc_dispatch_event == EV_DTM_ASSIGNMENT_COMPLETE)
        {
          rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;
          rr_event = EV_PROCEDURE_COMPLETE;
        }

        else if (proc_dispatch_event == EV_RADIO_LINK_FAILURE)
        {
          rr_procedure_data_ptr->dispatch_state = DISP_INACTIVE_STATE;
          rr_event = EV_RADIO_LINK_FAILURE;
        }

        break;
      }
      #endif /* FEATURE_GSM_DTM */

      default:
        MSG_GERAN_ERROR_1_G("Unexpected state(%d) for procedure control",(int) rr_procedure_data_ptr->dispatch_state);
        break;
    } /* switch (rr_procedure_data_ptr->dispatch_state) */

#ifdef DEBUG_RR_TRACE_MSG
    if ( ( (proc_dispatch_event != new_proc_dispatch_event) && (new_proc_dispatch_event != EV_NO_EVENT)) || (rr_procedure_data_ptr->old_dispatch_state != rr_procedure_data_ptr->dispatch_state))
    {
       rr_store_trace_msg_buf(
         RR_HANDLE_PROC_SM,
         new_proc_dispatch_event,
         (byte) rr_procedure_data_ptr->dispatch_state,
         new_message,
         gas_id
       );
    }
#endif
    if (new_proc_dispatch_event != EV_NO_EVENT)
    {
      proc_dispatch_event = new_proc_dispatch_event;
      new_proc_dispatch_event = EV_NO_EVENT;
    }
    else
    {
      proc_dispatch_event = EV_NO_EVENT;
    }
  }


  if (rr_procedure_data_ptr->old_dispatch_state != rr_procedure_data_ptr->dispatch_state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_handle_procedures",
      rr_procedure_data_ptr->old_dispatch_state,
      rr_procedure_data_ptr->dispatch_state,
      rr_dispatch_state_name,
      gas_id
    );

    rr_procedure_data_ptr->old_dispatch_state = rr_procedure_data_ptr->dispatch_state;
  }


  return rr_event;
} /* rr_handle_procedures */

/*!
 * \brief Resets all SAPIs in response to a DL_ESTABLISH_IND for SAPI0.
 *
 * \param msg_ptr - DL_ESTABLISH_IND message
 * \param gas_id
 */
void rr_handle_l2_reestablishment(
  dl_establish_ind_T *msg_ptr,
  const gas_id_t gas_id
)
{
  rr_reset_sapi3(gas_id);
  rr_initialize_nas_message_buffer(gas_id);

  /* SAPI0 link still exists */
  rr_set_link_status(SAPI0_LINK, gas_id);

  MSG_GERAN_HIGH_1_G( "SAPI0 re-establishment: %d msgs discarded",
           (int)msg_ptr->num_discarded_msgs);

  return;
}

/*!
 * \brief Resets L2 and RR SAPI controls in response to a lower layer failure.
 *
 * \param gas_id
 */
void rr_reset_sapis_due_to_failure(
  const gas_id_t gas_id
)
{
  /* send a MDL_RELEASE_REQ to Layer 2 to reset all SAPIs */
  rr_send_mdl_release_req(SAPI0, DCCH, gas_id);

  /* explicitly clear down the RR SAPI3 state machine */
  rr_reset_sapi3(gas_id);

  /* reset the dedicated channel link status */
  rr_set_link_status(NO_LINK, gas_id);

  return;
}

/*!
 * \brief Processes the SAPI0 DL_DATA_ACK_IND for a specific L3 message number.
 *
 * \param l3_msg_number
 * \param gas_id
 */
void rr_handle_signalling_acknowledgement(
  uint8 l3_msg_number,
  const gas_id_t gas_id
)
{
  /* process each msg number as appropriate */
  switch(l3_msg_number)
  {
  case RR_MSG_ID_CIPHER_MODE_COMPLETE:
    rr_confirm_ciphering_completed(gas_id);
    break;

  default:
    MSG_GERAN_MED_1_G( "Received DL_DATA_ACK_IND for RR message (%d)",(int)l3_msg_number);
    break;
  }

  return;
}

/* EOF */

