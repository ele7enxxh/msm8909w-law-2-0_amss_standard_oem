/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K   V A L I D I T Y   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock validity functions.


                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_validity.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/15   bcho    Free memory on reception of task stop sig
07/06/15   bcho    Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_validity.h"
#include "simlock_modem.h"
#include "simlock_util.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define SIMLOCK_IMSI_COUNT_MAX (SIMLOCK_SLOT_COUNT_MAX * SIMLOCK_SUBS_COUNT_MAX)

/* Critical section to protect accesses to valid IMSI list */
static rex_crit_sect_type  simlock_validity_crit_sect;

/* IMSI list for subscriptions validted by SIMLOCK algorithm */
static simlock_imsi_type * simlock_validity_imsi_list_ptr[SIMLOCK_IMSI_COUNT_MAX] = {NULL};

/* ===========================================================================
 MACRO:       SIMLOCK_VALIDITY_ENTER_IMSI_LIST_CRIT_SECT
 DESCRIPTION: To enter rex critical section for SIMLOCK valid IMSI list
 ===========================================================================*/
#define SIMLOCK_VALIDITY_ENTER_IMSI_LIST_CRIT_SECT                                 \
  rex_enter_crit_sect(&simlock_validity_crit_sect);

/* ===========================================================================
 MACRO:       SIMLOCK_VALIDITY_LEAVE_IMSI_LIST_CRIT_SECT
 DESCRIPTION: To leave rex critical section for SIMLOCK valid IMSI list
 ===========================================================================*/
#define SIMLOCK_VALIDITY_LEAVE_IMSI_LIST_CRIT_SECT                                 \
  rex_leave_crit_sect(&simlock_validity_crit_sect);

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION SIMLOCK_VALIDITY_INIT

DESCRIPTION
  This function initializes critical section for valid IMSI list

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_validity_init
(
  void
)
{
  rex_init_crit_sect(&simlock_validity_crit_sect);
}/* simlock_validity_init */


/*===========================================================================
FUNCTION SIMLOCK_VALIDITY_CHECK_IMSI

DESCRIPTION
  This function checks whether subscription related to IMSI is valid or not.
  If this API is invoked before SIMLOCK algo run, it would return FALSE as
  it is not concluded by SIMLOCK module whether subscription is valid or not.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  TRUE : If subscription, related to IMSI, is valid.
  FALSE: Otherwise

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_validity_check_imsi
(
  simlock_imsi_type imsi
)
{
  boolean is_imsi_valid  = FALSE;
  uint8   index          = 0;

  SIMLOCK_VALIDITY_ENTER_IMSI_LIST_CRIT_SECT;

  for(index = 0; index < SIMLOCK_IMSI_COUNT_MAX; index++)
  {
    if(simlock_validity_imsi_list_ptr[index] == NULL)
    {
      break;
    }

    if((imsi.imsi_len == simlock_validity_imsi_list_ptr[index]->imsi_len) &&
       (memcmp(imsi.imsi, simlock_validity_imsi_list_ptr[index]->imsi, imsi.imsi_len) == 0))
    {
      is_imsi_valid = TRUE;
      break;
    }
  }
  SIMLOCK_VALIDITY_LEAVE_IMSI_LIST_CRIT_SECT;

  SIMLOCK_MSG_MED_1("IMSI validity status 0x%x", is_imsi_valid);
  return is_imsi_valid;
} /* simlock_validity_check_imsi */


/*===========================================================================
FUNCTION SIMLOCK_VALIDITY_RESET_IMSI_INFO

DESCRIPTION
  This function resets IMSI validity info

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_validity_reset_imsi_info
(
  void
)
{
  uint8 index = 0;

  SIMLOCK_VALIDITY_ENTER_IMSI_LIST_CRIT_SECT;

  for(index = 0; index < SIMLOCK_IMSI_COUNT_MAX; index++)
  {
    if(simlock_validity_imsi_list_ptr[index] == NULL)
    {
      break;
    }
    SIMLOCK_MEM_FREE(simlock_validity_imsi_list_ptr[index]);
  }

  SIMLOCK_VALIDITY_LEAVE_IMSI_LIST_CRIT_SECT;
} /* simlock_validity_reset_imsi_info */


/*===========================================================================
FUNCTION SIMLOCK_VALIDITY_UPDATE_IMSI_INFO

DESCRIPTION
  This function updates IMSI validity info

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_validity_update_imsi_info
(
  const simlock_run_algorithm_msg_req_type   * req_msg_ptr,
  const simlock_run_algorithm_msg_resp_type  * resp_msg_ptr
)
{
  simlock_result_enum_type           simlock_status       = SIMLOCK_SUCCESS;
  simlock_slot_enum_type             slot                 = SIMLOCK_SLOT_1;
  uint8                              sub_cnt              = 0;
  uint8                              slot_index           = 0;
  uint8                              imsi_index           = 0;

  if((req_msg_ptr == NULL)  ||
     (resp_msg_ptr == NULL))
  {
    return;
  }

  SIMLOCK_VALIDITY_ENTER_IMSI_LIST_CRIT_SECT;

  /* Delete valid IMSI list */
  simlock_validity_reset_imsi_info();

  /* validate the subscriptions on the available slots */
  for(slot = SIMLOCK_SLOT_1;
      slot < SIMLOCK_SLOT_NUM && imsi_index < SIMLOCK_IMSI_COUNT_MAX;
      slot++)
  {
    simlock_status = simlock_util_get_slot_index(slot, &slot_index);
    if((simlock_status != SIMLOCK_SUCCESS) ||
       (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
    {
      continue;
    }

    for(sub_cnt = 0;
        (sub_cnt < req_msg_ptr->card_data[slot_index].num_subscriptions &&
         sub_cnt < SIMLOCK_SUBS_COUNT_MAX &&
         imsi_index < SIMLOCK_IMSI_COUNT_MAX);
        sub_cnt++)
    {
      if((req_msg_ptr->card_data[slot_index].subscription_data[sub_cnt].sub_type ==
                                                                SIMLOCK_SUBSCRIPTION_3GPP ||
          req_msg_ptr->card_data[slot_index].subscription_data[sub_cnt].sub_type ==
                                                                SIMLOCK_SUBSCRIPTION_3GPP2) &&
         resp_msg_ptr->card_validity_data[slot_index].subscription_data[sub_cnt].is_subscription_valid)
      {
        SIMLOCK_CHECK_AND_MEM_MALLOC(simlock_validity_imsi_list_ptr[imsi_index],
                                     sizeof(simlock_imsi_type));

        if(simlock_validity_imsi_list_ptr[imsi_index] != NULL)
        {
          simlock_memscpy(simlock_validity_imsi_list_ptr[imsi_index],
                          sizeof(simlock_imsi_type),
                          &req_msg_ptr->card_data[slot_index].subscription_data[sub_cnt].imsi,
                          sizeof(simlock_imsi_type));
          imsi_index++;
        }
      }
    } /* end for loop, sub_cnt = 0 */
  } /* end for loop, slot = SIMLOCK_SLOT_1 */

  SIMLOCK_VALIDITY_LEAVE_IMSI_LIST_CRIT_SECT;
} /* simlock_validity_update_imsi_info */


/*===========================================================================
FUNCTION SIMLOCK_VALIDITY_DEINIT

DESCRIPTION
  This function frees memory allocated to store IMSI list

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_validity_deinit
(
  void
)
{
  simlock_validity_reset_imsi_info();
}/* simlock_validity_deinit */

#endif /* FEATURE_SIMLOCK */

