/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   P O L I C Y  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock policy functions.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_policy.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   bcho    Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_policy.h"
#include "simlock_util.h"

/*===========================================================================

                   ENUMERATED DATA DECLARATIONS

===========================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_CARD_INVALID_REASON_TYPE

   DESCRIPTION:
     This enum specifies the reason for considering a card as invalid
-----------------------------------------------------------------------------*/
typedef enum
{
  SIMLOCK_CARD_INVALID_REASON_NONE,
  SIMLOCK_CARD_INVALID_REASON_NO_SUBSCRIPTION,
  SIMLOCK_CARD_INVALID_REASON_INVALID_SUBSCRIPTION,
} simlock_card_invalid_reason_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_IS_CARD_VALID

DESCRIPTION
  This function checks if a card on a slot is valid. All subscriptions on card
  must be valid for the card to be considered valid

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE   : If the card is valid
  FALSE  : Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
static boolean simlock_is_card_valid
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_run_algorithm_msg_resp_type        * result_msg_ptr,
  simlock_slot_enum_type                       slot,
  simlock_card_invalid_reason_type           * invalid_reason_ptr
)
{
  uint8       num_sub     = 0;
  uint8       slot_index  = 0;

  if((req_msg_ptr == NULL) ||
     (result_msg_ptr == NULL) ||
     (invalid_reason_ptr == NULL))
  {
    return FALSE;
  }

  if((simlock_util_get_slot_index(slot, &slot_index) !=
        SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    return FALSE;
  }

  /* Cards with no valid subscription should not be considered valid */
  if(req_msg_ptr->card_data[slot_index].num_subscriptions == 0 ||
     req_msg_ptr->card_data[slot_index].num_subscriptions > SIMLOCK_SUBS_COUNT_MAX)
  {
    *invalid_reason_ptr = SIMLOCK_CARD_INVALID_REASON_NO_SUBSCRIPTION;
    return FALSE;
  }

  /* All subscriptions on card must be valid for the card to be considered valid */
  for(num_sub = 0; num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions; num_sub++)
  {
    if(!result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid)
    {
      *invalid_reason_ptr = SIMLOCK_CARD_INVALID_REASON_INVALID_SUBSCRIPTION;
      return FALSE;
    }
  }

  return TRUE;
} /* simlock_is_card_valid */


/*===========================================================================
FUNCTION SIMLOCK_APPLY_VALID_3GPP_OR_3GPP2_POLICY

DESCRIPTION
  This function updates the validity of subscriptions on a card, based on
 validity of either 3GPP or 3GPP2 subscription.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_apply_valid_3gpp_or_3gpp2_policy
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_run_algorithm_msg_resp_type        * result_msg_ptr,
  simlock_slot_enum_type                       slot
)
{
  uint8                                       num_sub              = 0;
  uint8                                       slot_index           = 0;
  boolean                                     is_card_valid        = FALSE;

  if((req_msg_ptr == NULL) ||
     (result_msg_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if((simlock_util_get_slot_index(slot, &slot_index) !=
        SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* If any 3GPP or 3GPP2 subscription is valid, means card is valid */
  for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                    num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
  {
    if(req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
         SIMLOCK_SUBSCRIPTION_3GPP ||
       req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
         SIMLOCK_SUBSCRIPTION_3GPP2)
    {
      if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid)
      {
        is_card_valid = TRUE;
        break;
      }
    }
  }

  /* If no valid subscription was found, then nothing to do. */
  if(!is_card_valid)
  {
    return SIMLOCK_SUCCESS;
  }

  /* Update the validity of the subscriptions */
  for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                    num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
  {
    result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
      TRUE;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_apply_valid_3gpp_or_3gpp2_policy */


/*===========================================================================
FUNCTION SIMLOCK_APPLY_SUB_POLICY

DESCRIPTION
  This function updates the validity of a subscription on a card, based on
  the subscription policy set in the SIMLock feature.

  If the sub policy is set to one of the following, then the card is
  considered as valid only if the respective subscription(s) is/are present
  and valid on the card.
  1. Valid 3GPP required
  2. Valid 3GPP2 required
  3. Valid 3GPP and 3GPP2 required
  4. Valid 3GPP or 3GPP2 required

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_apply_sub_policy
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_run_algorithm_msg_resp_type        * result_msg_ptr,
  simlock_subscription_policy_enum_type        sub_policy,
  simlock_slot_enum_type                       slot
)
{
  uint8                                       num_sub              = 0;
  uint8                                       slot_index           = 0;
  boolean                                     is_card_valid        = TRUE;
  boolean                                     is_3gpp_app_present  = FALSE;
  boolean                                     is_3gpp2_app_present = FALSE;
  simlock_subscription_invalid_reason_type    failure_reason       = SIMLOCK_SUBSCRIPTION_INVALID_REASON_CONFIG_POLICY;
  simlock_category_enum_type                  failed_category      = SIMLOCK_CATEGORY_3GPP_NW;
  uint32                                      retries_max          = 0;
  uint32                                      retries_current      = 0;

  if((req_msg_ptr == NULL) ||
     (result_msg_ptr == NULL))
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if((simlock_util_get_slot_index(slot, &slot_index) !=
        SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* This is the only policy that makes valid an invalid session, so we need
     special handling. All other policies restrict valid sessions and are
     handled below */
  if(sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_OR_3GPP2_REQUIRED)
  {
    return simlock_apply_valid_3gpp_or_3gpp2_policy(req_msg_ptr, result_msg_ptr, slot);
  }

  /* apply the subscription policy */
  for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                    num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
  {
    if((sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_REQUIRED) ||
       (sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_AND_3GPP2_REQUIRED))
    {
      if(req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
           SIMLOCK_SUBSCRIPTION_3GPP)
      {
        is_3gpp_app_present = TRUE;
        if(!result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid)
        {
          failure_reason  = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_LOCKED;
          failed_category = result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failed_category;
          retries_max     = result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].num_retries_max;
          retries_current = result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].curr_retries;
          is_card_valid   = FALSE;
          break;
        }
      }
    }
    else if ((sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP2_REQUIRED) ||
             (sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_AND_3GPP2_REQUIRED))
    {
      if(req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
           SIMLOCK_SUBSCRIPTION_3GPP2)
      {
        is_3gpp2_app_present = TRUE;
        if(!result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid)
        {
          failure_reason  = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_LOCKED;
          failed_category = result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failed_category;
          retries_max     = result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].num_retries_max;
          retries_current = result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].curr_retries;
          is_card_valid   = FALSE;
          break;
        }
      }
    }
    else
    {
      /* Nothing to do */
    }
  }

  if(is_card_valid)
  {
    switch(sub_policy)
    {
      case SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_REQUIRED:
        /* if the 3gpp app presence is not known,
        mark card as not valid */
        if(!is_3gpp_app_present)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_NOT_PRESENT;
          is_card_valid = FALSE;
        }
        break;

      case SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP2_REQUIRED:
        /* if the 3gpp2 app presence is not known,
         mark card as not valid */
        if(!is_3gpp2_app_present)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_NOT_PRESENT;
          is_card_valid = FALSE;
        }
        break;

      case SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_AND_3GPP2_REQUIRED:
        /* if the 3gpp/3gpp2 app presence is not known,
         mark card as not valid */
        if(!is_3gpp_app_present)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_NOT_PRESENT;
          is_card_valid = FALSE;
        }
        else if(!is_3gpp2_app_present)
        {
          failure_reason = SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_REQUIRED_BUT_NOT_PRESENT;
          is_card_valid = FALSE;
        }
        break;

      default:
         break;
    }
  }

  /* if the card is valid, it means the policy check
     is OK. So need to update the card validity status */
  if(is_card_valid)
  {
    return SIMLOCK_SUCCESS;
  }

  /* Update the validity of the subscriptions on a slot based on the
     subscription policy */
  for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                    num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
  {
    if(((sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_REQUIRED) &&
        (req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
            SIMLOCK_SUBSCRIPTION_3GPP2))
                            ||
       ((sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP2_REQUIRED) &&
        (req_msg_ptr->card_data[slot_index].subscription_data[num_sub].sub_type ==
            SIMLOCK_SUBSCRIPTION_3GPP))
                            ||
       (sub_policy == SIMLOCK_SUBSCRIPTION_POLICY_VALID_3GPP_AND_3GPP2_REQUIRED))
    {
      result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
          FALSE;
      if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
           SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
      {
        result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
          failure_reason;
        result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failed_category =
          failed_category;
        result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].num_retries_max =
          retries_max;
        result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].curr_retries =
          retries_current;
      }
    }
  }

  return SIMLOCK_SUCCESS;
} /* simlock_apply_sub_policy */


/*===========================================================================
FUNCTION SIMLOCK_APPLY_CONFIG_POLICY

DESCRIPTION
  This function updates the validity of a subscription on a card, based on
  the slot and the subscription policy set in the SIMLock feature.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_apply_config_policy
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  simlock_config_data_type                   * config_data_ptr,
  simlock_run_algorithm_msg_resp_type        * result_msg_ptr
)
{
  simlock_result_enum_type               simlock_status = SIMLOCK_SUCCESS;
  simlock_slot_policy_enum_type          slot_policy    = SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS;
  simlock_subscription_policy_enum_type  sub_policy     = SIMLOCK_SUBSCRIPTION_POLICY_INDEPENDENT;
  simlock_slot_enum_type                 slot           = SIMLOCK_SLOT_1;
  simlock_card_invalid_reason_type       invalid_reason = SIMLOCK_CARD_INVALID_REASON_NONE;
  uint8                                  slot_index     = 0;
  uint8                                  sub_index      = 0;
  uint8                                  num_sub        = 0;
  boolean                                is_card_valid  = TRUE;

  if((req_msg_ptr == NULL)     ||
     (config_data_ptr == NULL) ||
     (result_msg_ptr == NULL))
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  slot_policy = config_data_ptr->slot_policy;
  sub_policy  = config_data_ptr->sub_policy;

  SIMLOCK_MSG_MED_2("simlock_apply_config_policy: slot_policy=0x%x, sub_policy=0x%x",
                    slot_policy, sub_policy);

  /* first apply the sub policy to subcriptions on all supported slots */
  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
  {
    if(simlock_apply_sub_policy(req_msg_ptr, result_msg_ptr, sub_policy, slot) !=
                  SIMLOCK_SUCCESS)
    {
      return SIMLOCK_GENERIC_ERROR;
    }
  }

  /* now apply the slot policy */
  switch(slot_policy)
  {
    case SIMLOCK_SLOT_POLICY_LOCK_ONLY_SLOT_1:
      for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              TRUE;
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_VALID_CARD_MUST_ON_SLOT_1:
      /* Unless all the categories on slot-1 are disabled,
         check if there is a valid card on slot-1 */
      if(simlock_util_all_categories_disabled(SIMLOCK_SLOT_1,
                                              config_data_ptr))
      {
        is_card_valid = TRUE;
      }
      else
      {
        is_card_valid = simlock_is_card_valid(req_msg_ptr, result_msg_ptr, SIMLOCK_SLOT_1, &invalid_reason);
      }

      for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
            if(!is_card_valid)
            {
              if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
              {
                if(invalid_reason == SIMLOCK_CARD_INVALID_REASON_NO_SUBSCRIPTION)
                {
                  result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                      SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_ON_SLOT1_REQUIRED_BUT_NOT_PRESENT;
                }
                else if(invalid_reason == SIMLOCK_CARD_INVALID_REASON_INVALID_SUBSCRIPTION)
                {
                  result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                      SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_ON_SLOT1_REQUIRED_BUT_LOCKED;
                }
              }
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_VALID_CARD_MUST_ON_ANY_SLOT:
      /* check if there is a valid card on any slot */
      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        is_card_valid = simlock_is_card_valid(req_msg_ptr, result_msg_ptr, slot, &invalid_reason);
        if(is_card_valid)
        {
          break;
        }
      }

      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
            if(!is_card_valid)
            {
              if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
              {
                if(invalid_reason == SIMLOCK_CARD_INVALID_REASON_INVALID_SUBSCRIPTION)
                {
                  result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                      SIMLOCK_SUBSCRIPTION_INVALID_REASON_APP_ON_ANY_SLOT_REQUIRED_BUT_LOCKED;
                }
              }
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_MASTER_SLOT_1:
      if(simlock_util_all_categories_disabled(SIMLOCK_SLOT_1,
                                              config_data_ptr))
      {
        is_card_valid = TRUE;
      }
      else
      {
        is_card_valid = simlock_is_card_valid(req_msg_ptr, result_msg_ptr, SIMLOCK_SLOT_1, &invalid_reason);
      }
      if(is_card_valid)
      {
        for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
        {
          simlock_status = simlock_util_get_slot_index(slot, &slot_index);
          if((simlock_status == SIMLOCK_SUCCESS) &&
             (slot_index < SIMLOCK_SLOT_COUNT_MAX))
          {
            for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                              num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
            {
              result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
                TRUE;
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_BLOCK_ALL_SLOTS_IF_SLOT_1_EXPIRED:
      is_card_valid = TRUE;
      for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                         sub_index < req_msg_ptr->card_data[0].num_subscriptions; sub_index++)
      {
        if(req_msg_ptr->card_data[0].subscription_data[sub_index].service_status == SIMLOCK_SRV_STATUS_FULL)
        {
          is_card_valid = TRUE;
          break;
        }
        else if(req_msg_ptr->card_data[0].subscription_data[sub_index].service_status == SIMLOCK_SRV_STATUS_EXPIRED)
        {
          is_card_valid = FALSE;
        }
      }

      for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
            if(!is_card_valid)
            {
              if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
              {
                result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_SUB_ON_SLOT1_EXPIRED;
              }
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_WAIT_FOR_FULL_SERVICE_ON_SLOT_1:
      is_card_valid = FALSE;
      for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                         sub_index < req_msg_ptr->card_data[0].num_subscriptions; sub_index++)
      {
        if(req_msg_ptr->card_data[0].subscription_data[sub_index].service_status == SIMLOCK_SRV_STATUS_FULL)
        {
          is_card_valid = TRUE;
          break;
        }
      }

      for(slot = SIMLOCK_SLOT_2; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
            if(!is_card_valid)
            {
              if(result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason ==
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_NONE)
              {
                result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].failure_reason =
                    SIMLOCK_SUBSCRIPTION_INVALID_REASON_WAIT_FOR_FULL_SRV_ON_SLOT1;
              }
            }
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_BLOCK_ALL_SLOTS_IF_ALL_VALID_SLOTS_EXPIRED:
      is_card_valid = FALSE;
      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        if(simlock_is_card_valid(req_msg_ptr, result_msg_ptr, slot, &invalid_reason) &&
           simlock_util_get_slot_index(slot, &slot_index) == SIMLOCK_SUCCESS &&
           slot_index < SIMLOCK_SLOT_COUNT_MAX)
        {
          for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                             sub_index < req_msg_ptr->card_data[slot_index].num_subscriptions; sub_index++)
          {
            if(req_msg_ptr->card_data[slot_index].subscription_data[sub_index].service_status != SIMLOCK_SRV_STATUS_EXPIRED)
            {
              is_card_valid = TRUE;
              break;
            }
          }

          if(is_card_valid)
          {
            break;
          }
        }
      }

      /* If all valid slots are expired then there is nothing to update 
         for other subscriptions */
      if(!is_card_valid)
      {
        break;
      }

      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(num_sub = 0; (num_sub < req_msg_ptr->card_data[slot_index].num_subscriptions &&
                            num_sub < SIMLOCK_SUBS_COUNT_MAX); num_sub++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid =
              is_card_valid;
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_WAIT_FOR_FULL_SERVICE_ON_ANY_VALID_SLOT:
      is_card_valid   = FALSE;
      /* Check if there is a valid card on any slot */
      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        if(simlock_is_card_valid(req_msg_ptr, result_msg_ptr, slot, &invalid_reason) &&
           simlock_util_get_slot_index(slot, &slot_index) == SIMLOCK_SUCCESS         &&
           slot_index < SIMLOCK_SLOT_COUNT_MAX)
        {
          /* Check if there a subscription which has full service that satisfies enabled categories */
          for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                             sub_index < req_msg_ptr->card_data[slot_index].num_subscriptions; sub_index++)
          {
            if(req_msg_ptr->card_data[slot_index].subscription_data[sub_index].service_status == SIMLOCK_SRV_STATUS_FULL)
            {
              is_card_valid = TRUE;
              break;
            }
          }

          if(is_card_valid)
          {
            break;
          }
        }
      }

      /* Not able to find any subscription with full service. So, nothing to
         update for other subscriptions, but allow only the cards which are
         valid to possibly get full service on it */
      if(!is_card_valid)
      {
        break;
      }

      for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
      {
        simlock_status = simlock_util_get_slot_index(slot, &slot_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (slot_index < SIMLOCK_SLOT_COUNT_MAX))
        {
          for(sub_index = 0; sub_index < SIMLOCK_SUBS_COUNT_MAX &&
                             sub_index < req_msg_ptr->card_data[slot_index].num_subscriptions; sub_index++)
          {
            result_msg_ptr->card_validity_data[slot_index].subscription_data[num_sub].is_subscription_valid = 
              is_card_valid;
          }
        }
      }
      break;

    case SIMLOCK_SLOT_POLICY_SAME_ON_ALL_SLOTS:
    case SIMLOCK_SLOT_POLICY_UNIQUE_FOR_EACH_SLOT:
      /* Nothing to do */
      break;

    default:
      return SIMLOCK_GENERIC_ERROR;
  }

  return simlock_status;
} /* simlock_apply_config_policy */

#endif /* FEATURE_SIMLOCK */

