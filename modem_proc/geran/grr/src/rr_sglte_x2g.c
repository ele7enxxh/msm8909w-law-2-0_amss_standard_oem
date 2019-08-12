/*============================================================================
  @file rr_sglte_x2g.c

  @brief This module implements a state machine that manages X2G IRAT for SGLTE

                Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_sglte_x2g.c#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_SGLTE
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_sglte_x2g.h"
#include "rr_defs.h"
#include "rr_gprs_debug.h"
#include "gprs_mem.h"
#include "rr_msgr.h"
#include "rr_mm_send.h"
#include "rr_general.h"
#include "rr_l1_send.h"
#include "rr_cell_selection.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

typedef enum
{
  RR_SGLTE_X2G_STATE_NULL,
  RR_SGLTE_X2G_STATE_ACTIVATING_MM,
  RR_SGLTE_X2G_STATE_CHANGING_L1_MODE,
  RR_SGLTE_X2G_STATE_MAX
} rr_sglte_x2g_state_e;

typedef struct
{
  rr_sglte_x2g_state_e         state;
  rr_sglte_x2g_state_e         old_state;
  sys_rat_pri_list_info_s_type rat_pri_list_info;
  sglte_l2g_resel_req_ded_pri_info_t lte_dedicated_pri; 
} rr_sglte_x2g_data_t;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

static rr_sglte_x2g_data_t * rr_sglte_x2g_data_ptrs[NUM_GERAN_DATA_SPACES];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

static rr_sglte_x2g_data_t * rr_sglte_get_x2g_data_ptr(const gas_id_t gas_id)
{
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return(rr_sglte_x2g_data_ptrs[as_index]);
}

static const char * rr_sglte_x2g_state_name(rr_sglte_x2g_state_e state)
{
  switch (state)
  {
    case RR_SGLTE_X2G_STATE_NULL:
      return "NULL";
    case RR_SGLTE_X2G_STATE_ACTIVATING_MM:
      return "ACTIVATING_MM";
    case RR_SGLTE_X2G_STATE_CHANGING_L1_MODE:
      return "CHANGING_L1_MODE";
    default:
      MSG_GERAN_ERROR_1("Unexpected state: %d", state);
      return "?";
  }
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

void rr_sglte_x2g_init(const gas_id_t gas_id)
{
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);
    ;
  if (rr_sglte_x2g_data_ptrs[as_index] == NULL)
  {
    rr_sglte_x2g_data_t *local_ptr;

    local_ptr = (rr_sglte_x2g_data_t *)GPRS_MEM_CALLOC(sizeof(rr_sglte_x2g_data_t));
    RR_NULL_CHECK_FATAL(local_ptr);

    /* initialise and store the pointer in the array of data spaces */
    rr_sglte_x2g_data_ptrs[as_index] = local_ptr;
  }

  return;
}

void rr_sglte_x2g_deinit(const gas_id_t gas_id)
{
  int as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  if (rr_sglte_x2g_data_ptrs[as_index] != NULL)
  {
    GPRS_MEM_FREE(rr_sglte_x2g_data_ptrs[as_index]);
    rr_sglte_x2g_data_ptrs[as_index] = NULL;
  }

  return;
}

void rr_sglte_x2g_control(rr_cmd_bdy_type *message, const gas_id_t gas_id)
{
  rr_sglte_x2g_data_t *rr_sglte_x2g_data_ptr = rr_sglte_get_x2g_data_ptr(gas_id);

  /* Validate module-global data */
  RR_NULL_CHECK_FATAL(rr_sglte_x2g_data_ptr);

  /* Validate input */
  RR_NULL_CHECK_RETURN_VOID(message);

  switch (rr_sglte_x2g_data_ptr->state)
  {
    case RR_SGLTE_X2G_STATE_NULL:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_SGLTE_X2G_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_SGLTE_X2G_IMSG_RESEL_REQ:
                {
                  rr_sglte_x2g_imsg_resel_req_t * resel_req = &message->rr.sglte_x2g.imsg.resel_req;

                  rr_sglte_x2g_data_ptr->rat_pri_list_info = resel_req->rat_pri_list_info;

                  /*temp store dedicated priories only overwrite current values if RR becomes*/
                  /*active RAT if resel fails we don't want to overwrite current values      */
                  rr_sglte_x2g_data_ptr->lte_dedicated_pri = resel_req->lte_dedicated_pri;
 
                  rr_send_activation_ind(resel_req->activation_reason, gas_id);

                  rr_sglte_x2g_data_ptr->state = RR_SGLTE_X2G_STATE_ACTIVATING_MM;
                  break;
                } /* RR_SGLTE_X2G_IMSG_SGLTE_MODE_ACTIVE_IND */

                case RR_SGLTE_X2G_IMSG_ABORT_REQ:
                {
                  rr_sglte_x2g_send_omsg(RR_SGLTE_X2G_OMSG_ABORT_CNF, gas_id);
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
                }
              }
              break;
            } /* RR_SGLTE_X2G_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
            }
          }
          break;
        } /* MS_RR_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
        }
      }
      break;
    } /* RR_SGLTE_X2G_STATE_NULL */

    case RR_SGLTE_X2G_STATE_ACTIVATING_MM:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_SGLTE_X2G_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_SGLTE_X2G_IMSG_ABORT_REQ:
                {
                  MSG_GERAN_HIGH_0_G("ABORT_REQ received in ACTIVATING_MM");
                  rr_sglte_x2g_data_ptr->state = NULL;
                  rr_sglte_x2g_send_omsg(RR_SGLTE_X2G_OMSG_ABORT_CNF, gas_id);
                  break;
                }
                case RR_SGLTE_X2G_IMSG_REJECT_REQ:
                {
                  MSG_GERAN_HIGH_0_G("Reject received in ACTIVATING_MM state due to high prio NAS message");
                  rr_sglte_x2g_data_ptr->state = NULL;
                  rr_sglte_x2g_send_omsg(RR_SGLTE_X2G_OMSG_REJECT_CNF, gas_id);
				  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
                }
              }
              break;
            } /* RR_SGLTE_X2G_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MM_RR:
        {
          switch (message->message_header.message_id)
          {
            case RR_ACTIVATION_RSP:
            {
              rr_PLMN_request_details_T * rr_plmn_request_details;
              boolean                     old_gsm_only;

              /* Activate pending RAT priority list to enable multi-RAT support. */
              rr_plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);
              old_gsm_only            = rr_gsm_only(gas_id);

              rr_plmn_request_details->rat_pri_list_info = rr_sglte_x2g_data_ptr->rat_pri_list_info;


              /*update dedicated priorities as RR is now active rat*/
              rr_sglte_dedicated_priority_info_store(&rr_sglte_x2g_data_ptr->lte_dedicated_pri, gas_id);


              if (rr_gsm_only(gas_id) == FALSE)
              {
                /* Update TDSCDMA band pref. */
                rr_update_band_preference_for_tdsrrc(rr_band_pref_utran_tdd(gas_id), gas_id);
              }

              if (old_gsm_only != rr_gsm_only(gas_id))
              {
                rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id), gas_id);
                rr_sglte_x2g_data_ptr->state = RR_SGLTE_X2G_STATE_CHANGING_L1_MODE;
              }
              else
              {
                MSG_GERAN_ERROR_0_G("GSM-only mode unchanged, SGLTE X2G procedure completed unexpectedly");
                rr_sglte_x2g_send_omsg(RR_SGLTE_X2G_OMSG_RESEL_CNF, gas_id);
              }              
              break;
            } /* RR_ACTIVATION_RSP */

            case RR_EST_REQ:
            {
              MSG_GERAN_HIGH_0_G("RR_EST_REQ received when activating MM in SGLTE X2G, abort rr est");
              rr_send_abort_ind(OTHER_REASON, MM_RR_OTHER_FAILURE, gas_id);
              grr_throwaway_saved_message(gas_id);
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
            }
          }
          break;
        } /* MS_MM_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
        }
      }
      break;
    } /* RR_SGLTE_X2G_STATE_ACTIVATING_MM */

    case RR_SGLTE_X2G_STATE_CHANGING_L1_MODE:
    {
      switch (message->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (message->rr.header.rr_message_set)
          {
            case RR_SGLTE_X2G_IMSG:
            {
              switch (message->message_header.message_id)
              {
                case RR_SGLTE_X2G_IMSG_ABORT_REQ:
                {
                  MSG_GERAN_ERROR_0_G("TODO");
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
                }
              }
              break;
            } /* RR_SGLTE_X2G_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_RR_L1:
        {
          switch (message->message_header.message_id)
          {
            case MPH_UE_MODE_CHANGE_CNF:
            {
              MSG_GERAN_HIGH_0_G("L1 confirms mode change complete, SGLTE X2G procedure completed");

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
              {
                gprs_scell_info_t  *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

                if ( (scell_info_ptr->gsm.two_quater_position == SI2Q_POSITION_NONE) ||
                     (rr_si2quater_all_instances_received(rr_get_instances_ptr(SI2Q_INSTANCE,gas_id))) )
                {
                  MSG_GERAN_HIGH_1_G("PSEUDO SI2Q: SI2Q not broadcasted or All SI2Q instances received, LTE config ncells:%d",scell_info_ptr->gsm.lte_neighbour_list.count);

                  if (scell_info_ptr->gsm.lte_neighbour_list.count == 0 )
                  {
                    /* If all Si2Q blocks received and NO LTE on all the SI2 Blocks */            
                    if ( rr_check_and_populate_default_irat_prior_params(gas_id) )
                    {
                      MSG_GERAN_HIGH_0_G("PSEUDO SI2Q Setting is a Success");
                    }  
                  }
                }
              }
#endif /*FEATURE_FAST_G2L_PSEUDO_SI2Q*/

              rr_sglte_x2g_data_ptr->state = RR_SGLTE_X2G_STATE_NULL;
              rr_sglte_x2g_send_omsg(RR_SGLTE_X2G_OMSG_RESEL_CNF, gas_id);
              break;
            } /* MPH_UE_MODE_CHANGE_CNF */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
            }
          }
          break;
        } /* MS_MM_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(message, rr_sglte_x2g_state_name(rr_sglte_x2g_data_ptr->state));
        }
      }
      break;
    } /* RR_SGLTE_X2G_STATE_CHANGING_L1_MODE */

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected state %d, resetting to NULL", rr_sglte_x2g_data_ptr->state);
      rr_sglte_x2g_data_ptr->state = RR_SGLTE_X2G_STATE_NULL;
    }
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(RR_SGLTE_X2G_SM, 
                         EV_INPUT_MESSAGE,
                         (byte) rr_sglte_x2g_data_ptr->state,
                         message,
                         gas_id);
#endif

  if (rr_sglte_x2g_data_ptr->state != rr_sglte_x2g_data_ptr->old_state)
  {
    RR_LOG_STATE_TRANSITION("rr_sglte_x2g_control",
                            rr_sglte_x2g_data_ptr->old_state,
                            rr_sglte_x2g_data_ptr->state,
                            rr_sglte_x2g_state_name, gas_id);
    
    rr_sglte_x2g_data_ptr->old_state = rr_sglte_x2g_data_ptr->state;
  }
}

#endif /* FEATURE_SGLTE */
