/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M L O C K   T I M E R   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock timer functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_timer.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/22/16   vdc     Remove F3 messages for memory allocation failure
08/12/15   stv     Clear temp unlock data when temp unlock expires
06/29/15   stv     Start time in temp unlock req as current time
                   Graceful handling of PMIC reset during temp unlock
05/15/15   vv      Support for RSU set configuration in simlock
09/29/14   vv      Added support for the remote SFS
08/29/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_timer.h"
#include "simlock.h"
#include "simlock_modem.h"
#include "simlock_util.h"
#ifdef FEATURE_SIMLOCK_RSU
#include "simlock_time.h"
#include "simlock_temp_unlock.h"
#endif /* FEATURE_SIMLOCK_RSU */

#include "rex.h"

#define SIMLOCK_TIMER_EXPIRY_SLOT_ID_SHIFT  16
#define SIMLOCK_TIMER_EXPIRY_CATEGORY_MASK  0xFFFF

#define SIMLOCK_TIMER_BUILD_PARAM_DATA(slot, category)               \
         (((uint32)slot << SIMLOCK_TIMER_EXPIRY_SLOT_ID_SHIFT) |     \
          ((uint32)category & SIMLOCK_TIMER_EXPIRY_CATEGORY_MASK))

#define SIMLOCK_TIMER_EXTRACT_PARAM_DATA(timer_param, slot, category)                                 \
          slot = (simlock_slot_enum_type)(timer_param >> SIMLOCK_TIMER_EXPIRY_SLOT_ID_SHIFT);         \
          category = (simlock_category_enum_type)(timer_param & SIMLOCK_TIMER_EXPIRY_CATEGORY_MASK);

#define SIMLOCK_MILLISECONDS_IN_A_MINUTE         60000

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* ----------------------------------------------------------------------------
 Temporary unlock timers
------------------------------------------------------------------------------*/
static rex_timer_type    *simlock_temporary_unlock_timer_ptr[SIMLOCK_SLOT_COUNT_MAX][SIMLOCK_CATEGORY_COUNT_MAX];


/*===========================================================================
FUNCTION SIMLOCK_TIMER_BUILD_EXPIRY_REQUEST

DESCRIPTION
  The function builds the temporary unlock timer expiry request
  to the simlock task

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static simlock_result_enum_type simlock_timer_build_expiry_request
(
  simlock_slot_enum_type       slot,
  simlock_category_enum_type   category
)
{
  simlock_result_enum_type     simlock_status = SIMLOCK_SUCCESS;
  simlock_task_cmd_type      * task_cmd_ptr   = NULL;

  SIMLOCK_CHECK_AND_MEM_MALLOC(task_cmd_ptr,
                               sizeof(simlock_task_cmd_type));
  if (task_cmd_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  memset(task_cmd_ptr, 0x00, sizeof(simlock_task_cmd_type));

  task_cmd_ptr->cmd_type                         = SIMLOCK_TEMPORARY_UNLOCK_TIMER_EXPIRY_CMD;
  task_cmd_ptr->data.timer_expiry_msg.slot       = slot;
  task_cmd_ptr->data.timer_expiry_msg.category   = category;

  /* Put on SIM Lock command queue */
  simlock_status = simlock_queue_cmd(task_cmd_ptr);
  if (simlock_status != SIMLOCK_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    SIMLOCK_MEM_FREE(task_cmd_ptr);
  }

  SIMLOCK_MSG_MED_1("queue of timer expiry command, status 0x%x",
                    simlock_status);

  return simlock_status;
} /* simlock_timer_build_expiry_request*/


/*===========================================================================
FUNCTION SIMLOCK_TEMPORARY_UNLOCK_TIMER_EXPIRY

DESCRIPTION
  This function handles the temporary unlock timer expiry

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_temporary_unlock_timer_expiry
(
  unsigned long timer_slot_category
)
{
  simlock_result_enum_type    simlock_status   = SIMLOCK_SUCCESS;
  simlock_slot_enum_type      slot             = SIMLOCK_SLOT_1;
  simlock_category_enum_type  category         = SIMLOCK_CATEGORY_3GPP_NW;

  SIMLOCK_TIMER_EXTRACT_PARAM_DATA(timer_slot_category, slot, category);

  SIMLOCK_MSG_MED_2("temporary unlock timer expired, slot: 0x%x, category: 0x%x",
                     slot, category);

  /* build the request to simlock task */
  simlock_status = simlock_timer_build_expiry_request(slot, category);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_MED_1("failed to build timer expiry request, status: 0x%x",
                      simlock_status);
  }
} /* simlock_temporary_unlock_timer_expiry */


/*===========================================================================
FUNCTION SIMLOCK_TIMER_START

DESCRIPTION 
  This function starts a temporary unlock timer for a category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_timer_start
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category,
  simlock_time_minutes_type      unlock_minutes
)
{
  simlock_result_enum_type   simlock_status      = SIMLOCK_SUCCESS;
  uint8                      slot_index          = 0;
  uint8                      category_index      = 0;
  rex_timer_cnt_type         timer_value         = 0;
  uint32                     timer_slot_category = 0;

  SIMLOCK_MSG_LOW_2("simlock_timer_start for slot 0x%x cat 0x%x", slot, category);

  simlock_status = simlock_util_get_slot_index(slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get slot index failed for slot, 0x%x", slot);
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_util_get_category_index(category,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get category index failed for category, 0x%x",
                      category);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* stop the timer, in case it already exists */
  (void)simlock_timer_stop(slot, category);

  timer_slot_category = SIMLOCK_TIMER_BUILD_PARAM_DATA(slot, category);

  simlock_temporary_unlock_timer_ptr[slot_index][category_index] =
                  rex_create_timer_ex(simlock_temporary_unlock_timer_expiry, timer_slot_category);

  if(simlock_temporary_unlock_timer_ptr[slot_index][category_index] == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  SIMLOCK_MSG_MED_1("timer duration, 0x%x", unlock_minutes);

  /* convert to milli seconds */
  timer_value = (rex_timer_cnt_type)(unlock_minutes * SIMLOCK_MILLISECONDS_IN_A_MINUTE);

  /* start the timer */
  (void)rex_set_timer(simlock_temporary_unlock_timer_ptr[slot_index][category_index], timer_value);

#ifdef FEATURE_SIMLOCK_RSU
  simlock_time_periodic_backup(TRUE);
#endif /* FEATURE_SIMLOCK_RSU */

  return SIMLOCK_SUCCESS;
} /* simlock_timer_start */


/*===========================================================================
FUNCTION SIMLOCK_TIMER_STOP

DESCRIPTION
  This function stops an existing temporary unlock timer

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_timer_stop
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category
)
{
  simlock_result_enum_type   simlock_status      = SIMLOCK_SUCCESS;
  uint8                      slot_index          = 0;
  uint8                      category_index      = 0;

  simlock_status = simlock_util_get_slot_index(slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get slot index failed for slot, 0x%x", slot);
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_status = simlock_util_get_category_index(category,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get category index failed for category, 0x%x",
                      category);
    return SIMLOCK_GENERIC_ERROR;
  }

  /* if timer doesn't exist, return error */
  if(simlock_temporary_unlock_timer_ptr[slot_index][category_index] == NULL)
  {
    return SIMLOCK_INVALID_STATE;
  }

  /* stop the timer */
  (void)rex_clr_timer(simlock_temporary_unlock_timer_ptr[slot_index][category_index]);

  /* deallocate the timer */
  rex_delete_timer_ex(simlock_temporary_unlock_timer_ptr[slot_index][category_index]);

  /* mark the timer as inactive */
  simlock_temporary_unlock_timer_ptr[slot_index][category_index] = NULL;

#ifdef FEATURE_SIMLOCK_RSU
  if(!simlock_temp_unlock_is_active())
  {
    simlock_time_periodic_backup(FALSE);
  }
#endif /* FEATURE_SIMLOCK_RSU */

  SIMLOCK_MSG_LOW_2("simlock_timer_stop for slot 0x%x cat 0x%x", slot, category);

  return SIMLOCK_SUCCESS;
} /* simlock_timer_stop */


/*===========================================================================
FUNCTION SIMLOCK_TIMER_CHECK_IS_TEMPORARY_UNLOCK_SET

DESCRIPTION
  This function checks if a temporary unlock is currently set for a category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_timer_check_is_temporary_unlock_set
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category
)
{
  simlock_result_enum_type   simlock_status      = SIMLOCK_SUCCESS;
  uint8                      slot_index       = 0;
  uint8                      category_index   = 0;

  simlock_status = simlock_util_get_slot_index(slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get slot index failed for slot, 0x%x", slot);
    return FALSE;
  }

  simlock_status = simlock_util_get_category_index(category,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    SIMLOCK_MSG_ERR_1("get category index failed for category, 0x%x",
                      category);
    return FALSE;
  }

  /* check if timer exists */
  if(simlock_temporary_unlock_timer_ptr[slot_index][category_index] != NULL)
  {
    return TRUE;
  }

  return FALSE;
} /* simlock_timer_check_is_temporary_unlock_set */


/*===========================================================================
FUNCTION SIMLOCK_TIMER_PROCESS_EXPIRY_REQUEST

DESCRIPTION
  The function processes the temporary unlock timer expiry request
  in the simlock task

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_timer_process_expiry_request
(
  const simlock_timer_expiry_msg_req_type  *  timer_expiry_req_ptr
)
{
  if(timer_expiry_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("invalid input parameters");
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* stop the corresponding timer */
  (void)simlock_timer_stop(timer_expiry_req_ptr->slot,
                           timer_expiry_req_ptr->category);

  /* send the temporary unlock event.
     This causes the client to trigger simlock_run_algo request */
  simlock_send_temporary_unlock_event(timer_expiry_req_ptr->slot,
                                      timer_expiry_req_ptr->category);

#ifdef FEATURE_SIMLOCK_RSU
  simlock_temp_unlock_reset_config(SIMLOCK_TEMP_UNLOCK_RESET_CONFIG_IF_UNLOCK_EXPIRED);
#endif/* FEATURE_SIMLOCK_RSU */

  return SIMLOCK_SUCCESS;
} /* simlock_timer_process_expiry_request */


/*===========================================================================
FUNCTION SIMLOCK_TIMER_GET_REMAINING_TIME

DESCRIPTION
  The function gets the time remaining in msecs, before a timer expires

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  Number of milliseconds left, before timer expires

SIDE EFFECTS
  None
===========================================================================*/
uint32 simlock_timer_get_remaining_time
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category
)
{
  simlock_result_enum_type             simlock_status      = SIMLOCK_SUCCESS;
  uint8                                slot_index          = 0;
  uint8                                category_index      = 0;

  simlock_status = simlock_util_get_slot_index(slot, &slot_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (slot_index >= SIMLOCK_SLOT_COUNT_MAX))
  {
    return 0;
  }

  simlock_status = simlock_util_get_category_index(category,
                                                   &category_index);
  if((simlock_status != SIMLOCK_SUCCESS) ||
     (category_index >= SIMLOCK_CATEGORY_COUNT_MAX))
  {
    return 0;
  }

  /* check if timer exists */
  if(simlock_temporary_unlock_timer_ptr[slot_index][category_index] == NULL)
  {
    return 0;
  }

  /* returns the time remaininig in msecs */
  return rex_get_timer(simlock_temporary_unlock_timer_ptr[slot_index][category_index]);
} /* simlock_timer_get_remaining_time */

#endif /* FEATURE_SIMLOCK */

